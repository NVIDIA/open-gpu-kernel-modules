/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "core/core.h"
#include "kernel/gpu/intr/intr.h"
#include "gpu/gpu.h"
#include "core/locks.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/subdevice/subdevice.h"
#include "kernel/gpu/gr/fecs_event_list.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/intr/engine_idx.h"
#include "kernel/gpu/intr/intr_service.h"
#include "kernel/gpu/gsp/gsp_trace_rats_macro.h"
#include "gpu/mmu/kern_gmmu.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "os/os.h"
#include "resserv/rs_server.h"
#include "vgpu/rpc.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "gpu/gsp/kernel_gsp.h"
#include "platform/sli/sli.h"
#include "gpu/disp/kern_disp.h"


#include "nv_ref.h"
#include "nvrm_registry.h"
#include "nvmisc.h"


//
// Used by _intrServiceStallExactList inside a critical section.
// Declared here as it needs to be cleared only at top of DPC processing.
//
static struct
{
    NvU32 intrCount;
    NvU32 intrVal;
} stuckIntr[MC_ENGINE_IDX_MAX];

static NvBool _intrServiceStallExactList(OBJGPU *pGpu, Intr *pIntr, MC_ENGINE_BITVECTOR *pEngines);
static void _intrLogLongRunningInterrupts(Intr *pIntr);
static void _intrInitServiceTable(OBJGPU *pGpu, Intr *pIntr);


NV_STATUS
intrGetSubtreeRange_IMPL
(
    Intr                             *pIntr,
    NV2080_INTR_CATEGORY              category,
    NV2080_INTR_CATEGORY_SUBTREE_MAP *pRange
)
{
    NV_ASSERT_OR_RETURN(pRange != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(category < NV2080_INTR_CATEGORY_ENUM_COUNT,
                        NV_ERR_INVALID_ARGUMENT);
    *pRange = pIntr->subtreeMap[category];
    return NV_OK;
}


void
intrServiceStall_IMPL(OBJGPU *pGpu, Intr *pIntr)
{
    MC_ENGINE_BITVECTOR pendingEngines;
    NV_STATUS status;
    NvBool bPending;
    NvU16 nextEngine;

    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        //
        // If the GPU is off the BUS or surprise removed during servicing DPC for ISRs
        // we wont know about GPU state until after we start processing DPCs for every
        // pending engine. This is because, the reg read to determine pending engines
        // return 0xFFFFFFFF due to GPU being off the bus. To prevent further processing,
        // reading PMC_BOOT_0 register to check if the GPU was surprise removed/ off the bus
        // and setting PDB_PROP_GPU_SECONDARY_BUS_RESET_PENDING to attempt Secondary Bus reset
        // at lower IRQL later to attempt recover the GPU and avoid all ISR DPC processing till
        // GPU is recovered.
        //

        NvU32 regReadValue = GPU_REG_RD32(pGpu, NV_PMC_BOOT_0);

        if (regReadValue == GPU_REG_VALUE_INVALID)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed GPU reg read : 0x%x. Check whether GPU is present on the bus\n",
                      regReadValue);
        }

        if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
        {
            goto exit;
        }

        if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
        {
            goto exit;
        }
    }

    portMemSet(stuckIntr, 0, sizeof(stuckIntr));

    if (pIntr->getProperty(pIntr, PDB_PROP_INTR_DISABLE_PER_INTR_DPC_QUEUEING))
    {
        do {
            NV_ASSERT_OK_OR_ELSE(status, intrGetPendingStall_HAL(pGpu, pIntr, &pendingEngines, NULL /* threadstate */), return);
            bPending = _intrServiceStallExactList(pGpu, pIntr, &pendingEngines);
        } while (bPending);
    }
    else if (!pIntr->bDpcStarted)
    {
        intrGetPendingStall_HAL(pGpu, pIntr, &pendingEngines, NULL /* threadstate */);
        if (!bitVectorTestAllCleared(&pendingEngines))
        {
            nextEngine = bitVectorCountTrailingZeros(&pendingEngines);
            intrQueueInterruptBasedDpc(pGpu, pIntr, nextEngine);
            bitVectorClr(&pIntr->pmcIntrPending, nextEngine);
            pIntr->bDpcStarted = NV_TRUE;
            intrProcessDPCQueue_HAL(pGpu, pIntr);
        }
    }
    else
    {
        intrProcessDPCQueue_HAL(pGpu, pIntr);
    }

    _intrLogLongRunningInterrupts(pIntr);

exit:
    return;
}

NV_STATUS
subdeviceCtrlCmdMcServiceInterrupts_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS *pServiceInterruptParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    Intr *pIntr  = GPU_GET_INTR(pGpu);
    MC_ENGINE_BITVECTOR engines;
    NvBool bMIGInUse = IS_MIG_IN_USE(pGpu);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);

    bitVectorClrAll(&engines);

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        NV_STATUS status = NV_OK;

        //
        // Force kernel-RM to service interrupts from GSP-RM. This will allow
        // kernel-RM to write notifiers and send an ack back to GSP.
        // GSP waits for this ack before clearing fast path POSSIBLE_ERR interrupt.
        //
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_FASTPATH_SEQ_ENABLED))
        {
            intrServiceStallSingle_HAL(pGpu, pIntr, MC_ENGINE_IDX_GSP, NV_TRUE);
        }

        NV_RM_RPC_CONTROL(pGpu, pRmCtrlParams->hClient, pRmCtrlParams->hObject, pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams, pRmCtrlParams->paramsSize, status);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "NVRM_RPC: NV2080_CTRL_CMD_MC_SERVICE_INTERRUPTS failed "
                      "with error 0x%x\n", status);
            return status;
        }
    }

    // convert RMCTRL engine flags to internal engine flags
    if (pServiceInterruptParams->engines == NV2080_CTRL_MC_ENGINE_ID_ALL)
    {
        bitVectorSetAll(&engines);
    }

    // check for individual engines. (currently only GR)
    else if (pServiceInterruptParams->engines & NV2080_CTRL_MC_ENGINE_ID_GRAPHICS)
    {
        // If MIG is enabled, check for all GRs available in that GPU instance
        if (bMIGInUse)
        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            MIG_INSTANCE_REF ref;
            NvU32 grCount = 0;
            NvU32 i;

            NV_ASSERT_OK_OR_RETURN(
                kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref));

            // Compute instances always contain 1 GR
            grCount = 1;
            if (ref.pMIGComputeInstance == NULL)
            {
                //
                // If client is only subscribed to GPU instance, use all
                // GPU instance GR engines
                //
                grCount = kmigmgrCountEnginesOfType(&ref.pKernelMIGGpuInstance->resourceAllocation.engines,
                                                    RM_ENGINE_TYPE_GR(0));
            }

            for (i = 0; i < grCount; ++i)
            {
                RM_ENGINE_TYPE globalRmEngineType;
                NvU32 grIdx;

                NV_ASSERT_OK(
                    kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref,
                                                      RM_ENGINE_TYPE_GR(i),
                                                      &globalRmEngineType));

                grIdx = RM_ENGINE_TYPE_GR_IDX(globalRmEngineType);
                bitVectorSet(&engines, MC_ENGINE_IDX_GRn(grIdx));
            }
        }
        else
        {
            bitVectorSet(&engines, MC_ENGINE_IDX_GR);
        }
    }

    intrServiceStallList_HAL(pGpu, pIntr, &engines, NV_TRUE);
    
    return NV_OK;
}

/*!
 * @brief checks for GMMU interrupts
 *
 * @param[in]  pGpu
 * @param[in]  pIntr
 * @param[out] pEngines
 * @param[in]  pThreadState
 */
void
intrGetGmmuInterrupts_IMPL
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    MC_ENGINE_BITVECTOR *pEngines,
    THREAD_STATE_NODE   *pThreadState
)
{
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    bitVectorClrAll(pEngines);

    // Check if we have any Gmmu interrupt pending
    if (pKernelGmmu != NULL)
    {
        //
        // Read the hub interrupt register as we apply mask while grabbing RM lock at leaf levels and
        // that would disable the top level PMC interrupt.
        //
        intrGetHubLeafIntrPending_HAL(pGpu, pIntr, pEngines, pThreadState);

        // Check if any fault was copied only if any other interrupt on GMMU is not pending.
        if (!bitVectorTest(pEngines, MC_ENGINE_IDX_GMMU))
        {
            if (portAtomicOrS32(kgmmuGetFatalFaultIntrPendingState(pKernelGmmu, GPU_GFID_PF), 0))
            {
                bitVectorSet(pEngines, MC_ENGINE_IDX_GMMU);
            }
            else
            {
                bitVectorClrAll(pEngines);
            }
        }
    }

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IGNORE_REPLAYABLE_FAULTS))
    {
        bitVectorClr(pEngines, MC_ENGINE_IDX_GMMU);
    }
}

void
intrSetDefaultIntrEn_IMPL(Intr *pIntr, NvU32 intrEn0)
{
    NV_ASSERT(intrEn0 <= INTERRUPT_TYPE_MAX);
    pIntr->intrEn0 = pIntr->intrEn0Orig = intrEn0;
}

NvU32
intrGetDefaultIntrEn_IMPL(Intr *pIntr)
{
    return (pIntr->intrEn0Orig);
}

void
intrSetIntrEn_IMPL(Intr *pIntr, NvU32 intrEn0)
{
    if ( pIntr->halIntrEnabled == NV_FALSE )
    {
        // Interrupts cannot be enabled as long as the freeze is true
        NV_PRINTF(LEVEL_INFO,
                  "intrSetIntrEn: set interrupt refused since interrupts are disabled in the HAL\n");
        return;
    }

    NV_ASSERT(intrEn0 <= INTERRUPT_TYPE_MAX);
    pIntr->intrEn0 = intrEn0;
}

NvU32
intrGetIntrEn_IMPL(Intr *pIntr)
{
    if ( pIntr->halIntrEnabled == NV_FALSE )
    {
        NV_PRINTF(LEVEL_INFO,
                  "intrGetIntrEn: Returning interrupt disabled. Interrupts disabled in the HAL\n");
        return INTERRUPT_TYPE_DISABLED;
    }

    return (pIntr->intrEn0);
}

void
intrSetIntrMaskUnblocked_IMPL(Intr *pIntr, MC_ENGINE_BITVECTOR *pUnblockedEngines)
{
    NV_ASSERT(pUnblockedEngines);
    bitVectorCopy(&pIntr->intrMask.engMaskUnblocked, pUnblockedEngines);
}

void
intrGetIntrMaskUnblocked_IMPL(Intr *pIntr, MC_ENGINE_BITVECTOR *pUnblockedEngines)
{
    NV_ASSERT(pUnblockedEngines);
    bitVectorCopy(pUnblockedEngines, &pIntr->intrMask.engMaskUnblocked);
}

NvU32
intrGetIntrMaskFlags_IMPL(Intr *pIntr)
{
    return pIntr->intrMask.flags;
}

void
intrSetIntrMaskFlags_IMPL(Intr *pIntr, NvU32 flags)
{
    pIntr->intrMask.flags = flags;
}

void
intrQueueDpc_IMPL(OBJGPU *pGpu, Intr *pIntr, DPCQUEUE *pDPCQueue, DPCNODE *pNode)
{
    if (pDPCQueue->pFront == NULL)
    {
        pDPCQueue->pFront = pDPCQueue->pRear = pNode;
    }
    else
    {
        pDPCQueue->pRear->pNext = pNode;
        pDPCQueue->pRear = pNode;
    }
    pDPCQueue->numEntries++;
}

DPCNODE *
intrDequeueDpc_IMPL(OBJGPU *pGpu, Intr *pIntr, DPCQUEUE *pDPCQueue)
{
    DPCNODE  *pNode;

    pNode = pDPCQueue->pFront;
    if (pDPCQueue->pFront->pNext == NULL)
    {
        pDPCQueue->pFront = pDPCQueue->pRear = NULL;
    }
    else
    {
        pDPCQueue->pFront = pDPCQueue->pFront->pNext;
    }
    return pNode;
}

NvBool
intrIsDpcQueueEmpty_IMPL(OBJGPU *pGpu, Intr *pIntr, DPCQUEUE *pDPCQueue)
{
    return (NULL == pDPCQueue->pFront);
}

void intrQueueInterruptBasedDpc_IMPL(OBJGPU *pGpu, Intr *pIntr, NvU16 engine)
{
    DPCNODE  *pNode;
    DPCQUEUE *pDPCQueue = &pIntr->dpcQueue;

    pNode = portMemAllocNonPaged(sizeof(DPCNODE));
    if (NULL != pNode)
    {
        pNode->pNext        =   NULL;
        pNode->dpctype      =   INTERRUPT_BASED_DPC;
        bitVectorClrAll(&pNode->dpcdata.pendingEngines);
        bitVectorSet(&pNode->dpcdata.pendingEngines, engine);

        intrQueueDpc(pGpu, pIntr, pDPCQueue, pNode);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Cannot allocate memory for the DPC queue entry\n");
        DBG_BREAKPOINT();
    }
}

/*!
 * @brief Conditionally service interrupts across all gpus on the provided engine list.
 *
 * If GPU lock is held for all GPUs, then service interrupts for all GPUs.
 * Else, service the interrupts for the device corresponding to the input GPU.
 * Operations that use resources across multiple gpus may fail while interrupts on a gpu are pending.
 *
 * @param[in] pEngines   List of engines to be serviced.
 */

void
intrServiceStallListAllGpusCond_IMPL
(
    OBJGPU *pGpu,
    Intr *pIntr,
    MC_ENGINE_BITVECTOR *pEngines,
    NvBool checkIntrEnabled
)
{
    NvU32  gpuAttachCnt, gpuAttachMask, gpuInstance;
    NvBool bBCState = NV_FALSE;

    if (!rmGpuLockIsOwner())
    {
        //
        // We shouldn't service other GPU interrupts, if we don't have their lock.
        // Reason being that if an intr happens, it would trigger the isr if the locks are not held.
        // See bug 1911524
        //
        intrServiceStallListDevice(pGpu, pIntr, pEngines, checkIntrEnabled);
        return;
    }

    gpumgrGetGpuAttachInfo(&gpuAttachCnt, &gpuAttachMask);
    gpuInstance = 0;

    while ((pGpu = gpumgrGetNextGpu(gpuAttachMask, &gpuInstance)) != NULL)
    {
        pIntr = GPU_GET_INTR(pGpu);

        // Intr is missing on soc-disp
        if (pIntr == NULL)
        {
            continue;
        }

        //
        // deviceInstance can be invalid when we loop over all attached gpus
        // in SLI unlink path: Bug 2462254
        //
        if (IsDeviceDestroyed(pGpu))
            continue;

        // Check that the GPU state is neither loaded nor loading
        if (!gpuIsStateLoading(pGpu) && !gpuIsStateLoaded(pGpu))
        {
            continue;
        }

        //
        // checkIntrEnabled: Service intr for a GPU only if they are enabled
        // eg: In Unload path, the intr for the gpu that is being unloaded
        // are explicitly disabled and we do not wish to service those
        // But other GPU intr should make forward progress.
        //
        if (checkIntrEnabled && !intrGetIntrEn(pIntr))
            continue;

        bBCState = gpumgrGetBcEnabledStatus(pGpu);
        gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);

        // Dont service interrupts if GPU is not powered up or is Surprise Removed
        if (gpuIsGpuFullPower(pGpu) && API_GPU_ATTACHED_SANITY_CHECK(pGpu))
        {
            intrServiceStallList_HAL(pGpu, pIntr, pEngines, NV_FALSE);
        }

        gpumgrSetBcEnabledStatus(pGpu, bBCState);
    }
}

/*!
 * @brief Service interrupts on given gpu.
 *
 * Service pending interrupts of given device on the provided engine list.
 *
 * @param[in] pGpu      GPU object pointer
 * @param[in] pEngines  List of engines to be serviced.
 */
void
intrServiceStallListDevice_IMPL
(
    OBJGPU *pGpu,
    Intr *pIntr,
    MC_ENGINE_BITVECTOR *pEngines,
    NvBool checkIntrEnabled
)
{
    NvBool bBCState = NV_FALSE;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY);

    if (checkIntrEnabled && !intrGetIntrEn(GPU_GET_INTR(pGpu)))
        SLI_LOOP_CONTINUE;

    bBCState = gpumgrGetBcEnabledStatus(pGpu);
    gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);

    if (gpuIsGpuFullPower(pGpu))
    {
        pIntr = GPU_GET_INTR(pGpu);
        intrServiceStallList_HAL(pGpu, pIntr, pEngines, NV_FALSE);
    }

    gpumgrSetBcEnabledStatus(pGpu, bBCState);

    SLI_LOOP_END;
}


INTR_TABLE_ENTRY *
intrGetInterruptTableEntryFromEngineId_IMPL
(
    OBJGPU *pGpu,
    Intr   *pIntr,
    NvU16   mcEngineId,
    NvBool  bNonStall
)
{
    InterruptTable    *pIntrTable;
    InterruptTableIter iter;
    NV_STATUS          status;

    status = intrGetInterruptTable_HAL(pGpu, pIntr, &pIntrTable);
    if (status != NV_OK)
    {
        NV_ASSERT_OK_FAILED("Failed to get interrupt table", status);
        return NULL;
    }

    for (iter = vectIterAll(pIntrTable); vectIterNext(&iter);)
    {
        INTR_TABLE_ENTRY *pEntry = iter.pValue;
        if (pEntry->mcEngine == mcEngineId)
        {
            return pEntry;
        }
    }

    NV_PRINTF(LEVEL_ERROR,
              "Could not find the specified engine Id %u\n",
              mcEngineId);
    DBG_BREAKPOINT();
    return NULL;
}

/*!
 * @brief Get the interrupt vector for the given engine
 *
 * @param[in] pGpu         OBJGPU pointer
 * @param[in] pIntr        Intr pointer
 * @param[in] mcEngineId   Engine id (i.e. one of MC_ENGINE_IDX_* defines)
 * @param[in] bNonStall    NV_TRUE if non-stall, else NV_FALSE
 *
 * @returns the nonstall interrupt vector for that engine
 */
NvU32
intrGetVectorFromEngineId_IMPL
(
    OBJGPU *pGpu,
    Intr   *pIntr,
    NvU16   mcEngineId,
    NvBool  bNonStall
)
{
    NvU32 intrVector;
    INTR_TABLE_ENTRY *pEntry = intrGetInterruptTableEntryFromEngineId(pGpu,
        pIntr,
        mcEngineId,
        bNonStall);

    NV_ASSERT_OR_RETURN(pEntry != NULL, NV_INTR_VECTOR_INVALID);
    intrVector = bNonStall ? pEntry->intrVectorNonStall : pEntry->intrVector;

    if (intrVector == NV_INTR_VECTOR_INVALID)
    {
        NV_PRINTF(LEVEL_ERROR, "mcEngineIdx %d with bNonStall = %d has invalid vector\n",
                                mcEngineId, bNonStall);

        NV_ASSERT_OR_RETURN(intrVector != NV_INTR_VECTOR_INVALID, NV_INTR_VECTOR_INVALID);
    }

    return intrVector;
}


/*!
 * @brief Convert a general MC_ENGINE_BITVECTOR to its corresponding hardware
 *        interrupt mask in PMC.
 *
 * @param[in] engineMask    Mask of engines to get interrupt mask for
 *
 * @returns A bitmask of PMC interrupt bits corresponding to the engines
 *          specified in the engineMask parameter
 */
NvU32
intrConvertEngineMaskToPmcIntrMask_IMPL
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    PMC_ENGINE_BITVECTOR engineMask
)
{
    InterruptTable    *pIntrTable;
    InterruptTableIter iter;
    NvU32              pmcIntrMask = 0;

    if (NV_OK != intrGetInterruptTable_HAL(pGpu, pIntr, &pIntrTable))
    {
        NV_ASSERT(pmcIntrMask);
        return pmcIntrMask;
    }

    for (iter = vectIterAll(pIntrTable); vectIterNext(&iter);)
    {
        INTR_TABLE_ENTRY *pEntry = iter.pValue;
        if (bitVectorTest(engineMask, pEntry->mcEngine))
        {
            pmcIntrMask |= pEntry->pmcIntrMask;
        }
    }

    return pmcIntrMask;
}

/*!
 * @brief Convert a PMC interrupt mask to a general MC_ENGINE_BITVECTOR.
 *
 * @param[in] pmcIntrMask  Mask of PMC interrupt bits to get engine mask for
 *
 * @returns A bitmask of engines corresponding to the interrupts specified in
 *          the intrMask parameter
 */
void
intrConvertPmcIntrMaskToEngineMask_IMPL
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    NvU32                pmcIntrMask,
    PMC_ENGINE_BITVECTOR pEngines
)
{
    InterruptTable    *pIntrTable;
    InterruptTableIter iter;

    NV_ASSERT_OR_RETURN_VOID(pEngines != NULL);
    NV_ASSERT_OR_RETURN_VOID(
        intrGetInterruptTable_HAL(pGpu, pIntr, &pIntrTable) == NV_OK);

    bitVectorClrAll(pEngines);
    for (iter = vectIterAll(pIntrTable); vectIterNext(&iter);)
    {
        INTR_TABLE_ENTRY *pEntry = iter.pValue;
        if (pEntry->pmcIntrMask == NV_PMC_INTR_INVALID_MASK)
        {
            continue;
        }

        if (pEntry->pmcIntrMask & pmcIntrMask)
        {
            bitVectorSet(pEngines, pEntry->mcEngine);
        }
    }
}

/*!
 * @brief Returns the smallest notification interrupt vector, used to find the
 * interrupt vector space in which partition assigned interrupt vectors should
 * be remapped.
 */
NV_STATUS
intrGetSmallestNotificationVector_IMPL
(
    OBJGPU *pGpu,
    Intr   *pIntr,
    NvU32  *pSmallestVector
)
{
    InterruptTable    *pIntrTable;
    InterruptTableIter iter;
    NvU32              val = NV_INTR_VECTOR_INVALID;

    NV_ASSERT_OR_RETURN(pSmallestVector != NULL, NV_ERR_INVALID_ARGUMENT);
    *pSmallestVector = val;

    NV_ASSERT_OK_OR_RETURN(intrGetInterruptTable_HAL(pGpu, pIntr, &pIntrTable));

    for (iter = vectIterAll(pIntrTable); vectIterNext(&iter);)
    {
        val = NV_MIN(val,
                     ((INTR_TABLE_ENTRY *)iter.pValue)->intrVectorNonStall);
    }

    //
    // Return the smallest interrupt vector in the subtree to which val belongs.
    // This should be consistent across all chips even if they have different
    // floorsweeping configs
    //
    *pSmallestVector = NV_CTRL_INTR_LEAF_IDX_TO_GPU_VECTOR_START(
        NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_START(
            NV_CTRL_INTR_GPU_VECTOR_TO_SUBTREE(val)));
    return NV_OK;
}

/*!
 * @brief Reads NV_PFB_NISO_INTR register and determine if we have an interrupt pending
 *        The function returns NVBIT64(MC_ENGINE_IDX_GMMU) if any interrupt is found pending
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pIntr         Intr pointer
 * @param[out] pEngines      list of pending engines
 * @param[in]  pThreadState
 */
void
intrGetHubLeafIntrPending_STUB
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    MC_ENGINE_BITVECTOR *pEngines,
    THREAD_STATE_NODE   *pThreadState
)
{
    bitVectorClrAll(pEngines);
}

static void _intrInitRegistryOverrides(OBJGPU *, Intr *);

NV_STATUS
intrConstructEngine_IMPL
(
    OBJGPU *pGpu,
    Intr *pIntr,
    ENGDESCRIPTOR engDesc
)
{
    pIntr->dpcQueue.pFront = NULL;
    pIntr->dpcQueue.pRear  = NULL;
    pIntr->bDpcStarted     = NV_FALSE;

    if (!RMCFG_FEATURE_PLATFORM_WINDOWS)
    {
        pIntr->setProperty(pIntr, PDB_PROP_INTR_DISABLE_PER_INTR_DPC_QUEUEING, NV_TRUE);
    }

    NV_ASSERT_OK_OR_RETURN(vectInit(&pIntr->intrTable,
                                    portMemAllocatorGetGlobalNonPaged(),
                                    0 /* capacity */));

    return NV_OK;
}


void
intrDestruct_IMPL
(
    Intr *pIntr
)
{
    OBJGPU   *pGpu = ENG_GET_GPU(pIntr);
    DPCQUEUE *pDPCQueue = &pIntr->dpcQueue;
    DPCNODE  *pNode = NULL;

    while (!intrIsDpcQueueEmpty(pGpu, pIntr, pDPCQueue))
    {
        pNode = intrDequeueDpc(pGpu, pIntr, pDPCQueue);
        portMemFree(pNode);
    }

    vectDestroy(&pIntr->intrTable);

}


NV_STATUS
intrStatePreInitLocked_IMPL
(
    OBJGPU *pGpu,
    Intr   *pIntr
)
{
    NV_STATUS status = NV_OK;

    for (NvU32 i = 0; i < NV_ARRAY_ELEMENTS(pIntr->subtreeMap); i++)
    {
        pIntr->subtreeMap[i].subtreeStart = NV2080_INTR_INVALID_SUBTREE;
        pIntr->subtreeMap[i].subtreeEnd   = NV2080_INTR_INVALID_SUBTREE;
    }

    NV_ASSERT_OK_OR_GOTO(status, intrInitInterruptTable_HAL(pGpu, pIntr), exit);
    _intrInitServiceTable(pGpu, pIntr);

exit:

    return status;
}

NV_STATUS
intrStateInitUnlocked_IMPL
(
    OBJGPU *pGpu,
    Intr   *pIntr
)
{
    NvU32 data = 0;

    if (osReadRegistryDword(pGpu,
                            NV_REG_STR_RM_INTR_DETAILED_LOGS, &data) == NV_OK)
    {
        if (data == NV_REG_STR_RM_INTR_DETAILED_LOGS_ENABLE)
        {
            pIntr->setProperty(pIntr,
                               PDB_PROP_INTR_ENABLE_DETAILED_LOGS,
                               NV_TRUE);
        }
    }

    _intrInitRegistryOverrides(pGpu, pIntr);

    return NV_OK;
}


NV_STATUS
intrStateInitLocked_IMPL
(
    OBJGPU   *pGpu,
    Intr     *pIntr
)
{
    // Enable interrupts in the HAL
    pIntr->halIntrEnabled = NV_TRUE;

    // Enable the interrupt mapping within the chip
    intrSetDefaultIntrEn(pIntr, INTERRUPT_TYPE_HARDWARE);

    // Initially mask will allow all interrupts.
    pIntr->intrMask.cached = INTERRUPT_MASK_ENABLED;

    //initialize all GPU interrupts to be serviced by RM
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    pGpu->pmcRmOwnsIntrMask        = INTERRUPT_MASK_ENABLED;
    if (pKernelGmmu != NULL)
    {
        pKernelGmmu->uvmSharedIntrRmOwnsMask = RM_UVM_SHARED_INTR_MASK_ALL;
    }

    if (pIntr->getProperty(pIntr, PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING))
    {
        MC_ENGINE_BITVECTOR engines;

        bitVectorClrAll(&engines);
        bitVectorSet(&engines, MC_ENGINE_IDX_DISP);

        intrSetIntrMaskUnblocked(pIntr, &engines);
        // Hypervisor will set the intr unblocked mask later at the time of SWRL init.
    }

    return NV_OK;
}

void
intrStateDestroy_IMPL
(
    OBJGPU   *pGpu,
    Intr     *pIntr
)
{
    intrStateDestroyPhysical_HAL(pGpu, pIntr);

    // Disable interrupts in the HAL
    pIntr->halIntrEnabled = NV_FALSE;

    // Ensure we don't try to call through any stale IntrService pointers.
    portMemSet(pIntr->intrServiceTable, 0, sizeof(pIntr->intrServiceTable));

    NV_ASSERT_OK(intrDestroyInterruptTable_HAL(pGpu, pIntr));
}

/*
 * Service interrupts that may not have been queued due to GPU lock unavailablity
 *
 * @param[in] pGpu
 * @param[in] pIntr
 * @param[in] pThreadState
 */
NV_STATUS
intrServiceNonStallBottomHalf_IMPL
(
    OBJGPU            *pGpu,
    Intr              *pIntr,
    MC_ENGINE_BITVECTOR *pIntrPending,
    THREAD_STATE_NODE *pThreadState
)
{
    NV_STATUS status = NV_OK;
    NV_STATUS tmpStatus;

    tmpStatus = intrCheckAndServiceNonReplayableFault_HAL(pGpu, pIntr, pThreadState);
    if (tmpStatus != NV_OK)
        status = tmpStatus;

    tmpStatus = intrCheckAndServiceFecsEventbuffer(pGpu, pIntr, pIntrPending, pThreadState);
    if (tmpStatus != NV_OK)
        status = tmpStatus;

    return status;
}

static void
_intrInitRegistryOverrides
(
    OBJGPU *pGpu,
    Intr *pIntr
)
{
    OBJGPU  *pGpuLoop;
    Intr *pIntrLoop;
    NvU32    gpuInstance;
    NvU32    gpuMask;
    NvU32    data = 0;

    // This code has only the ability to disable the mechanism and will not
    // enable it. This property is autoset only on GPUs on which it is
    // unsupported. And it is not reset anywhere which will allow the
    // mechanism to be enabled on the GPUs which are unsupported.
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_PER_INTR_DPC_QUEUING, &data) == NV_OK)
    {
        if(data)
        {
            gpuMask = gpumgrGetGpuMask(pGpu);
            gpuInstance = 0;
            while ((pGpuLoop = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
            {
                pIntrLoop = GPU_GET_INTR(pGpuLoop);
                pIntrLoop->setProperty(pIntrLoop, PDB_PROP_INTR_DISABLE_PER_INTR_DPC_QUEUEING, NV_TRUE);
            }
        }
    }

    pIntr->setProperty(pIntr, PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING, NV_FALSE);
    if (RMCFG_FEATURE_PLATFORM_WINDOWS || hypervisorIsVgxHyper())
    {
        // Enable IntrMask Locking by default if supported
        if (pIntr->getProperty(pIntr, PDB_PROP_INTR_MASK_SUPPORTED) && !IS_VIRTUAL(pGpu))
        {
            pIntr->setProperty(pIntr, PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING, NV_TRUE);
        }
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_INTR_LOCKING_MODE, &data) == NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NV_REG_STR_RM_INTR_LOCKING_MODE was set to: 0x%x\n", data);

        switch (data)
        {
            case NV_REG_STR_RM_INTR_LOCKING_MODE_INTR_MASK:
            {
                if (pIntr->getProperty(pIntr, PDB_PROP_INTR_MASK_SUPPORTED))
                {
                    pIntr->setProperty(pIntr, PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING, NV_TRUE);
                }
            }
            break;

            default:
            {
                pIntr->setProperty(pIntr, PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING, NV_FALSE);
            }
        }
    }

    pIntr->intrStuckThreshold = INTR_STUCK_THRESHOLD;
    if (osReadRegistryDword(pGpu, NV_REG_STR_INTR_STUCK_THRESHOLD, &data) == NV_OK)
    {
        pIntr->intrStuckThreshold = data;
    }
}

void
intrSaveIntrEn0FromHw_IMPL
(
    OBJGPU   *pGpu,
    Intr     *pIntr
)
{
    pIntr->saveIntrEn0 = intrGetIntrEnFromHw_HAL(pGpu, pIntr, NULL /* threadState */);
}

NV_STATUS
intrInitInterruptTable_KERNEL
(
    OBJGPU *pGpu,
    Intr   *pIntr
)
{
    RM_API   *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS status = NV_OK;
    NvU32     i;
    NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS *pParams;

    //
    // Unconditionally clear it. For _KERNEL, this function can be called to
    // re-fetch the KernelRM interrupt table every time PhysicalRM updates it
    // like after MIG partition configuration changes.
    //
    vectClear(&pIntr->intrTable);

    pParams = portMemAllocNonPaged(sizeof(*pParams));
    NV_ASSERT_TRUE_OR_GOTO(status, pParams != NULL, NV_ERR_NO_MEMORY, exit);

    NV_ASSERT_OK_OR_GOTO(status,
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_INTR_GET_KERNEL_TABLE,
                        pParams,
                        sizeof *pParams),
        exit);
    NV_ASSERT_TRUE_OR_GOTO(status,
                           pParams->tableLen <=
                               NV2080_CTRL_INTERNAL_INTR_MAX_TABLE_SIZE,
                           NV_ERR_OUT_OF_RANGE,
                           exit);

    NV_ASSERT_OK_OR_GOTO(status,
                         vectReserve(&pIntr->intrTable, pParams->tableLen),
                         exit);
    for (i = 0; i < pParams->tableLen; ++i)
    {
        INTR_TABLE_ENTRY entry = {0};
        entry.mcEngine           = pParams->table[i].engineIdx;
        entry.pmcIntrMask        = pParams->table[i].pmcIntrMask;
        entry.intrVector         = pParams->table[i].vectorStall;
        entry.intrVectorNonStall = pParams->table[i].vectorNonStall;
        NV_ASSERT_TRUE_OR_GOTO(status,
                               vectAppend(&pIntr->intrTable, &entry) != NULL,
                               NV_ERR_NO_MEMORY,
                               exit);
    }
    NV_ASSERT_OK_OR_GOTO(status, vectTrim(&pIntr->intrTable, 0), exit);

    portMemCopy(pIntr->subtreeMap,   sizeof pIntr->subtreeMap,
                pParams->subtreeMap, sizeof pParams->subtreeMap);

exit:
    portMemFree(pParams);
    return status;
}

static void
_intrInitServiceTable
(
    OBJGPU *pGpu,
    Intr *pIntr
)
{
    GPU_CHILD_ITER iter = {0};
    IntrService *pIntrService;

    portMemSet(pIntr->intrServiceTable, 0, sizeof(pIntr->intrServiceTable));

    while ((pIntrService = GPU_GET_NEXT_CHILD_OF_TYPE(pGpu, &iter, IntrService)))
    {
        intrservRegisterIntrService(pGpu, pIntrService, pIntr->intrServiceTable);
    }

    gpuRegisterGenericKernelFalconIntrService(pGpu, pIntr->intrServiceTable);
}

NvU32 intrServiceInterruptRecords_IMPL
(
    OBJGPU  *pGpu,
    Intr    *pIntr,
    NvU16    engineIdx,
    NvBool  *pServiced
)
{
    IntrService *pIntrService = pIntr->intrServiceTable[engineIdx].pInterruptService;
    NvU32 ret = 0;
    NvBool bShouldService;
    NvU64 intrTiming, intrTiming2;
    IntrServiceClearInterruptArguments clearParams = {engineIdx};
    IntrServiceServiceInterruptArguments serviceParams = {engineIdx};

    if (pIntrService == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Missing interrupt handler for engine idx %d\n", engineIdx);
        NV_ASSERT_FAILED("Missing interrupt handler");
        bShouldService = NV_FALSE;
    }
    else
    {
        bShouldService = intrservClearInterrupt(pGpu, pIntrService, &clearParams);
    }

    if (pServiced != NULL)
    {
        *pServiced = bShouldService;
    }

    if (bShouldService)
    {
        osGetPerformanceCounter(&intrTiming);

        GSP_TRACE_RATS_ADD_RECORD(NV_RATS_GSP_TRACE_TYPE_INTR_START, pGpu, (NvU32) engineIdx);
        ret = intrservServiceInterrupt(pGpu, pIntrService, &serviceParams);
        GSP_TRACE_RATS_ADD_RECORD(NV_RATS_GSP_TRACE_TYPE_INTR_END, pGpu, (NvU32) engineIdx);

        osGetPerformanceCounter(&intrTiming2);
        intrTiming = intrTiming2 - intrTiming;
        if (intrTiming > LONG_INTR_LOG_LENGTH_NS)
        {
            pIntr->longIntrStats[engineIdx].intrCount++;
            if (intrTiming > pIntr->longIntrStats[engineIdx].intrLength)
                pIntr->longIntrStats[engineIdx].intrLength = intrTiming;
        }
    }
    return ret;
}

NV_STATUS intrServiceNotificationRecords_IMPL
(
    OBJGPU            *pGpu,
    Intr              *pIntr,
    NvU16              engineIdx,
    THREAD_STATE_NODE *pThreadState
)
{
    NV_STATUS status;
    IntrService *pIntrService = pIntr->intrServiceTable[engineIdx].pNotificationService;
    IntrServiceServiceNotificationInterruptArguments params = {pThreadState, engineIdx};

    if (pIntrService == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Missing notification interrupt handler for engine idx %d\n", engineIdx);
        NV_ASSERT_FAILED("Missing notification interrupt handler");
        return NV_ERR_GENERIC;
    }

    status = intrservServiceNotificationInterrupt(pGpu, pIntrService, &params);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Could not service notification interrupt for engine idx %d; returned NV_STATUS = 0x%x\n",
            engineIdx, status);
        NV_ASSERT_FAILED("Could not service notification interrupt");
        return NV_ERR_GENERIC;
    }

    //
    // On Turing onwards, all non-stall interrupts, including the ones from
    // PBDMA, have moved to reporting on the runlist that is served by the
    // PBDMA. There are still some clients that use the PBDMA interrupts
    // but currently register for a notifier of type RM_ENGINE_TYPE_HOST.
    // Until those clients change to using the new notifiers, RM will fire
    // the host notifier for all non-stall interrupts from host-driven
    // engines. See bug 1866491.
    //
    if (pIntr->bDefaultNonstallNotify &&
        pGpu->activeFifoEventMthdNotifiers != 0 &&
        !pIntr->intrServiceTable[engineIdx].bFifoWaiveNotify)
    {
        engineNonStallIntrNotify(pGpu, RM_ENGINE_TYPE_HOST);
    }

    return NV_OK;
}

NV_STATUS intrCheckFecsEventbufferPending_IMPL
(
    OBJGPU *pGpu,
    Intr *pIntr,
    MC_ENGINE_BITVECTOR *pIntrPending,
    NvBool *pbCtxswLog
)
{
    NvU8 i;
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NvS16 fecsCtxswLogConsumerCount= fecsGetCtxswLogConsumerCount(pGpu, pKernelGraphicsManager);

    NV_ASSERT_OR_RETURN(pbCtxswLog != NULL, NV_ERR_INVALID_ARGUMENT);

    *pbCtxswLog = NV_FALSE;

    if (fecsCtxswLogConsumerCount <= 0)
    {
        NV_ASSERT(fecsCtxswLogConsumerCount == 0);
        return NV_OK;
    }

    for (i = 0; i < GPU_MAX_GRS; i++)
    {
        KernelGraphics *pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, i);
        if ((pKernelGraphics != NULL) &&
            (kgraphicsIsIntrDrivenCtxswLoggingEnabled(pGpu, pKernelGraphics)) &&
            bitVectorTest(pIntrPending, MC_ENGINE_IDX_GRn(i)))
        {
            if (!fecsBufferChanged(pGpu, pKernelGraphics))
                continue;

            if (fecsSignalIntrPendingIfNotPending(pGpu, pKernelGraphics))
            {
                *pbCtxswLog = kgraphicsIsBottomHalfCtxswLoggingEnabled(pGpu, pKernelGraphics);
            }
        }
    }

    return NV_OK;
}

NV_STATUS
intrCheckAndServiceFecsEventbuffer_IMPL
(
    OBJGPU           *pGpu,
    Intr             *pIntr,
    MC_ENGINE_BITVECTOR *pIntrPending,
    THREAD_STATE_NODE *pThreadState
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NvS16 fecsCtxswLogConsumerCount= fecsGetCtxswLogConsumerCount(pGpu, pKernelGraphicsManager);
    NvU8 i;

    if (bitVectorTestAllCleared(pIntrPending))
        return NV_OK;

    if (fecsCtxswLogConsumerCount <= 0)
    {
        NV_ASSERT(fecsCtxswLogConsumerCount == 0);
        return NV_OK;
    }

    for (i = 0; i < GPU_MAX_GRS; i++)
    {
        KernelGraphics *pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, i);
        if ((pKernelGraphics != NULL) &&
            bitVectorTest(pIntrPending, MC_ENGINE_IDX_GRn_FECS_LOG(i)) &&
            kgraphicsIsIntrDrivenCtxswLoggingEnabled(pGpu, pKernelGraphics) &&
            fecsClearIntrPendingIfPending(pGpu, pKernelGraphics))
        {
            nvEventBufferFecsCallback(pGpu, (void*)pKernelGraphics);
        }
    }

    return NV_OK;
}

NV_STATUS
intrGetInterruptTable_IMPL
(
    OBJGPU          *pGpu,
    Intr            *pIntr,
    InterruptTable **ppTable
)
{
    NV_ASSERT_OR_RETURN(ppTable != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(!vectIsEmpty(&pIntr->intrTable), NV_ERR_INVALID_STATE);
    *ppTable = &pIntr->intrTable;
    return NV_OK;
}


NV_STATUS
intrDestroyInterruptTable_IMPL
(
    OBJGPU *pGpu,
    Intr   *pIntr
)
{
    for (INTR_TREE tree = 0; tree < NV_ARRAY_ELEMENTS(pIntr->vectorToMcIdx);
         ++tree)
    {
        portMemFree(pIntr->vectorToMcIdx[tree]);
        pIntr->vectorToMcIdx[tree]       = NULL;
        pIntr->vectorToMcIdxCounts[tree] = 0;
    }

    vectClear(&pIntr->intrTable);
    return NV_OK;
}

void intrProcessDPCQueue_IMPL
(
    OBJGPU *pGpu,
    Intr *pIntr
)
{
    DPCQUEUE *pDPCQueue = &pIntr->dpcQueue;
    MC_ENGINE_BITVECTOR pendingEngines;
    NvU16 nextEngine;

    do
    {
        bitVectorClrAll(&pendingEngines);
        if (!intrIsDpcQueueEmpty(pGpu, pIntr, pDPCQueue))
        {
            DPCNODE *pNode;
            pNode = intrDequeueDpc(pGpu, pIntr, pDPCQueue);
            if (pNode->dpctype == INTERRUPT_BASED_DPC)
            {
                _intrServiceStallExactList(pGpu, pIntr, &pNode->dpcdata.pendingEngines);

                portMemFree(pNode);

                if (!bitVectorTestAllCleared(&pIntr->pmcIntrPending))
                {
                    nextEngine = bitVectorCountTrailingZeros(&pIntr->pmcIntrPending);
                    intrQueueInterruptBasedDpc(pGpu, pIntr, nextEngine);
                    bitVectorClr(&pIntr->pmcIntrPending, nextEngine);
                }
            }
        }

        if (intrIsDpcQueueEmpty(pGpu, pIntr, pDPCQueue))
        {
            //
            // Process all exceptions as required
            //
            intrGetPendingStall_HAL(pGpu, pIntr, &pendingEngines, NULL /* threadstate */);
            if (!bitVectorTestAllCleared(&pendingEngines))
            {
                nextEngine = bitVectorCountTrailingZeros(&pendingEngines);
                intrQueueInterruptBasedDpc(pGpu, pIntr, nextEngine);
                bitVectorCopy(&pIntr->pmcIntrPending, &pendingEngines);
                bitVectorClr(&pIntr->pmcIntrPending, nextEngine);
                pIntr->bDpcStarted = NV_TRUE;
            }
        }
    } while (!bitVectorTestAllCleared(&pendingEngines));
}

/*!
 * @brief Allow the unblocked intrMask isr to come in.
 *
 * Re-allow the isr to come in if it was previously disabled.
 *
 * @param[in]  pGpu         OBJGPU pointer
 * @param[in]  pIntr        Intr pointer
 */
static void
_intrReenableIntrMask
(
    OBJGPU            *pGpu,
    Intr              *pIntr
)
{
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);

    if (pIntr->getProperty(pIntr, PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING))
    {
        //
        // If separate latency line is present, it will always be enabled,
        // so no need to re-enable.
        //
        if ((pKernelDisplay != NULL) && !pKernelDisplay->getProperty(pKernelDisplay, PDB_PROP_KDISP_HAS_SEPARATE_LOW_LATENCY_LINE))
        {
            // Same code as locks.c
            NvU64 oldIrql;
            NvU32 intrMaskFlags;

            oldIrql = rmIntrMaskLockAcquire(pGpu);

            intrMaskFlags = intrGetIntrMaskFlags(pIntr);
            intrMaskFlags &= ~INTR_MASK_FLAGS_ISR_SKIP_MASK_UPDATE;
            intrSetIntrMaskFlags(pIntr, intrMaskFlags);

            // Allow some intrs to come in.
            if (pIntr->getProperty(pIntr, PDB_PROP_INTR_USE_TOP_EN_FOR_VBLANK_HANDLING))
            {
                intrSetDisplayInterruptEnable_HAL(pGpu, pIntr, NV_TRUE,  NULL /* threadstate */);
            }
            else
            {
                intrSetIntrMask_HAL(pGpu, pIntr, &pIntr->intrMask.engMaskUnblocked, NULL /* threadstate */);
            }

            intrSetIntrEnInHw_HAL(pGpu, pIntr, intrGetIntrEn(pIntr), NULL /* threadstate */);

            rmIntrMaskLockRelease(pGpu, oldIrql);
        }
    }
}

static void
_intrLogLongRunningInterrupts(Intr *pIntr)
{
    NvU64 now;
    osGetPerformanceCounter(&now);

    for (NvU32 i = 0; i < MC_ENGINE_IDX_MAX; ++i)
    {
        if (pIntr->longIntrStats[i].intrCount > 0)
        {
            if (now - pIntr->longIntrStats[i].lastPrintTime > LONG_INTR_LOG_RATELIMIT_NS)
            {
                NV_PRINTF(LEVEL_WARNING, "%u long-running interrupts (%llu ns or slower) from engine %u, longest taking %llu ns\n",
                          pIntr->longIntrStats[i].intrCount, LONG_INTR_LOG_LENGTH_NS, i, pIntr->longIntrStats[i].intrLength);

                pIntr->longIntrStats[i].intrCount = 0;
                pIntr->longIntrStats[i].intrLength = 0;
                pIntr->longIntrStats[i].lastPrintTime = now;
            }
        }
    }
}

static NvBool
_intrServiceStallExactList
(
    OBJGPU   *pGpu,
    Intr     *pIntr,
    MC_ENGINE_BITVECTOR *pEngines
)
{
    NV_STATUS status;
    NvU32  engineIdx;
    NvU32  intr;
    NvBool bHandled;
    NvBool bIntrStuck = NV_FALSE;
    NvBool bPending   = NV_FALSE;
    InterruptTable    *pIntrTable;
    InterruptTableIter iter;

    if (bitVectorTestAllCleared(pEngines))
    {
        return NV_FALSE;
    }

    NV_ASSERT_OK_OR_ELSE(status,
                         intrGetInterruptTable_HAL(pGpu, pIntr, &pIntrTable),
                         return NV_FALSE);

    for (iter = vectIterAll(pIntrTable); vectIterNext(&iter);)
    {
        INTR_TABLE_ENTRY *pEntry = iter.pValue;

        // Skip servicing interrupts when GPU is off the bus
        if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
        {
            return NV_FALSE;
        }

        // Skip servicing interrupts when GPU is in Reset
        if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
        {
            return NV_FALSE;
        }

        engineIdx = pEntry->mcEngine;

        if (bitVectorTest(pEngines, engineIdx))
        {
            bHandled = NV_FALSE;
            intr = intrServiceInterruptRecords(pGpu, pIntr, engineIdx, &bHandled);

            if (bHandled)
            {
                if ((intr != 0) && (intr == stuckIntr[engineIdx].intrVal))
                {
                    stuckIntr[engineIdx].intrCount++;
                    if (stuckIntr[engineIdx].intrCount > pIntr->intrStuckThreshold)
                    {
                        NV_PRINTF(LEVEL_ERROR,
                                    "Stuck interrupt detected for mcEngine %u\n",
                                    engineIdx);
                        bIntrStuck = NV_TRUE;
                        NV_ASSERT(0);
                    }
                }

                stuckIntr[engineIdx].intrVal = intr;

                bPending = bPending || (intr != 0);
            }
        }
    }

    if (IS_VIRTUAL(pGpu) && bitVectorTest(pEngines, MC_ENGINE_IDX_VGPU))
    {
        vgpuService(pGpu);
    }

    if (bitVectorTest(pEngines, MC_ENGINE_IDX_DISP))
    {
        _intrReenableIntrMask(pGpu, pIntr);
    }

    if (bIntrStuck)
    {
        NV_PRINTF(LEVEL_ERROR,
                    "Interrupt is stuck. Bailing after %d iterations.\n",
                    pIntr->intrStuckThreshold);
        return NV_FALSE;
    }

    return bPending;
}

/*!
 * @brief Perform inline servicing of requested interrupts.
 *
 * Do inline servicing of all requested engines and VGPU.
 * If NULL is passed for pEngines, service all engines but not VGPU.
 *
 * This special casing of the VGPU interrupt servicing is a holdover from
 * before refactoring intrServiceStall and intrServiceStallList to use a common
 * inner loop; previously, intrServiceStallList simply didn't attempt to
 * service VGPU interrupts even if requested.
 *
 * @param[in] pGpu
 * @param[in] pIntr
 * @param[in] pEngines (See above for behavior when NULL)
 * @param[in] bLoop Continue servicing interrupts in loop until completed or stuck interrupt detected.
 *
 */
void
intrServiceStallList_IMPL
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    MC_ENGINE_BITVECTOR *pEngines,
    NvBool               bLoop
)
{
    NV_STATUS           status;
    MC_ENGINE_BITVECTOR exactEngines;
    NvBool              bPending;
    CALL_CONTEXT       *pOldContext = NULL;

    if (gpumgrGetBcEnabledStatus(pGpu) && !rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
    {
        NV_ASSERT_FAILED("intrServiceStallList_IMPL is expected to be unicast or own all its locks! Please post a stacktrace in bug 2003060!");
    }

    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        //
        // If the GPU is off the BUS or surprise removed during servicing DPC for ISRs
        // we wont know about GPU state until after we start processing DPCs for every
        // pending engine. This is because, the reg read to determine pending engines
        // return 0xFFFFFFFF due to GPU being off the bus. To prevent further processing,
        // reading PMC_BOOT_0 register to check if the GPU was surprise removed/ off the bus
        // and setting PDB_PROP_GPU_SECONDARY_BUS_RESET_PENDING to attempt Secondary Bus reset
        // at lower IRQL later to attempt recover the GPU and avoid all ISR DPC processing till
        // GPU is recovered.
        //

        NvU32 regReadValue = GPU_REG_RD32(pGpu, NV_PMC_BOOT_0);

        if (regReadValue == GPU_REG_VALUE_INVALID)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed GPU reg read : 0x%x. Check whether GPU is present on the bus\n",
                      regReadValue);
        }

        // Dont service interrupts if GPU is surprise removed
        if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu) || API_GPU_IN_RESET_SANITY_CHECK(pGpu))
        {
            return;
        }
    }

    NV_ASSERT_OK_OR_ELSE(status, resservSwapTlsCallContext(&pOldContext, NULL), return);

    portMemSet(stuckIntr, 0, sizeof(stuckIntr));

    do
    {
        NV_ASSERT_OK_OR_ELSE(status, intrGetPendingStall_HAL(pGpu, pIntr, &exactEngines, NULL /* threadstate */),
          goto done);

        if (pEngines == NULL)
        {
            bitVectorClr(&exactEngines, MC_ENGINE_IDX_VGPU);
        }
        else
        {
            bitVectorAnd(&exactEngines, &exactEngines, pEngines);
        }

        bPending = _intrServiceStallExactList(pGpu, pIntr, &exactEngines);
    }
    while (bPending && bLoop);

done:
    // Delay prints until after exiting critical sections to save perf impact
    _intrLogLongRunningInterrupts(pIntr);

    NV_ASSERT_OK(resservRestoreTlsCallContext(pOldContext));
}


void
intrServiceStallSingle_IMPL
(
    OBJGPU   *pGpu,
    Intr     *pIntr,
    NvU16     engIdx,
    NvBool    bLoop
)
{
    MC_ENGINE_BITVECTOR engines;
    bitVectorClrAll(&engines);
    bitVectorSet(&engines, engIdx);
    intrServiceStallList_HAL(pGpu, pIntr, &engines, bLoop);
}

NvU64
intrGetIntrTopCategoryMask_IMPL
(
    Intr *pIntr,
    NV2080_INTR_CATEGORY category
)
{
    NvU32 subtreeStart = pIntr->subtreeMap[category].subtreeStart;
    NvU32 subtreeEnd   = pIntr->subtreeMap[category].subtreeStart;
    NvU64 ret = 0x0;
    NvU32 i;

    if (subtreeStart == NV2080_INTR_INVALID_SUBTREE ||
        subtreeEnd   == NV2080_INTR_INVALID_SUBTREE)
    {
        return 0x0;
    }

    for (i = subtreeStart; i <= subtreeEnd; ++i)
    {
        ret |= NVBIT64(i);
    }

    return ret;
}


NvU64
intrGetIntrTopLegacyStallMask_IMPL
(
    Intr   *pIntr
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pIntr);
    NvU64 ret =
        intrGetIntrTopCategoryMask(pIntr, NV2080_INTR_CATEGORY_ESCHED_DRIVEN_ENGINE) |
        intrGetIntrTopCategoryMask(pIntr, NV2080_INTR_CATEGORY_RUNLIST) |
        intrGetIntrTopCategoryMask(pIntr, NV2080_INTR_CATEGORY_UVM_OWNED) |
        intrGetIntrTopCategoryMask(pIntr, NV2080_INTR_CATEGORY_UVM_SHARED);

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_SWRL_GRANULAR_LOCKING))
    {
        ret |= intrGetIntrTopCategoryMask(pIntr,
            NV2080_INTR_CATEGORY_RUNLIST_NOTIFICATION);
    }

    // Sanity check that Intr.subtreeMap is initialized
    NV_ASSERT(ret != 0);
    return ret;
}


NvU64
intrGetIntrTopLockedMask_IMPL
(
    OBJGPU *pGpu,
    Intr   *pIntr
)
{
    NvU64 ret =
        intrGetIntrTopCategoryMask(pIntr, NV2080_INTR_CATEGORY_ESCHED_DRIVEN_ENGINE) |
        intrGetIntrTopCategoryMask(pIntr, NV2080_INTR_CATEGORY_RUNLIST);

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_SWRL_GRANULAR_LOCKING))
    {
        ret |= intrGetIntrTopCategoryMask(pIntr,
            NV2080_INTR_CATEGORY_RUNLIST_NOTIFICATION);
    }

    // Sanity check that Intr.subtreeMap is initialized
    NV_ASSERT(ret != 0);
    return ret;
}


NV_STATUS intrSetInterruptEntry_IMPL
(
    Intr                 *pIntr,
    INTR_TREE             tree,
    NvU32                 vector,
    const InterruptEntry *pEntry
)
{
    NV_ASSERT_OR_RETURN(pEntry != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(tree < NV_ARRAY_ELEMENTS(pIntr->vectorToMcIdx),
                        NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pIntr->vectorToMcIdx[tree] != NULL,
                        NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(vector < pIntr->vectorToMcIdxCounts[tree],
                        NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(
        pIntr->vectorToMcIdx[tree][vector].mcEngine == MC_ENGINE_IDX_NULL,
        NV_ERR_INVALID_STATE);
    pIntr->vectorToMcIdx[tree][vector] = *pEntry;
    return NV_OK;
}


NvU64
intrGetUvmSharedLeafEnDisableMask_IMPL
(
    OBJGPU *pGpu,
    Intr   *pIntr
)
{
    NvU64 mask = 0;
    NV2080_INTR_CATEGORY_SUBTREE_MAP uvmShared;

    // GSP RM services both MMU non-replayable fault and FIFO interrupts
    if (IS_GSP_CLIENT(pGpu))
    {
        return ~mask;
    }

    NvU32 locklessRmVectors[2];
    intrGetLocklessVectorsInRmSubtree_HAL(pGpu, pIntr, &locklessRmVectors);

    NV_ASSERT_OK(intrGetSubtreeRange(pIntr,
                                     NV2080_INTR_CATEGORY_UVM_SHARED,
                                     &uvmShared));
    //
    // Ascertain that we only have 1 client subtree (we assume
    // this since we cache only 64 bits).
    //
    NV_ASSERT(uvmShared.subtreeStart == uvmShared.subtreeEnd);

    //
    // Ascertain that we only have 2 subtrees as this is what we currently
    // support by only caching 64 bits
    //
    NV_ASSERT(
        (NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_END(uvmShared.subtreeEnd) - 1) ==
        NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_START(uvmShared.subtreeStart));

    NvU32 i;
    for (i = 0; i < NV_ARRAY_ELEMENTS(locklessRmVectors); ++i)
    {
        NvU32 vector = locklessRmVectors[i];
        if (vector == NV_INTR_VECTOR_INVALID)
        {
            continue;
        }

        // Ascertain that they're in the first leaf
        NV_ASSERT(
            NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(vector) ==
            NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_START(uvmShared.subtreeStart));

        mask |= NVBIT64(NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(vector));
    }

    mask <<= 32;

    return ~mask;
}

NV_STATUS
intrRefetchInterruptTable_IMPL
(
    OBJGPU *pGpu,
    Intr   *pIntr
)
{
    NV_ASSERT_OK_OR_RETURN(intrStateUnload_HAL(pGpu, pIntr, GPU_STATE_FLAGS_PRESERVING));

    NV_ASSERT_OK_OR_RETURN(intrInitInterruptTable_HAL(pGpu, pIntr));

    NV_ASSERT_OK_OR_RETURN(intrStateLoad_HAL(pGpu, pIntr, GPU_STATE_FLAGS_PRESERVING));

    return NV_OK;
}
