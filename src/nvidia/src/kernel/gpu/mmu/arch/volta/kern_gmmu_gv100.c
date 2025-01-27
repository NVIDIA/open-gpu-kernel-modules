/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/mmu/kern_gmmu.h"
#include "gpu/gpu.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/intr/intr.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/uvm/uvm.h"
#include "vgpu/vgpu_events.h"
#include "vgpu/rpc.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "nverror.h"

#include "published/volta/gv100/dev_fb.h"
#include "published/volta/gv100/dev_ram.h"
#include "published/volta/gv100/dev_fault.h"

// Static function Definition
static NV_STATUS _kgmmuCreateExceptionDataFromPriv_GV100(OBJGPU *pGpu, KernelGmmu *pKernelGmmu,
                                                         MMU_FAULT_BUFFER_ENTRY *pParsedFaultEntry,
                                                         FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData);
static NvU32 _kgmmuResetFaultBufferError_GV100(NvU32 faultBufType);
static NV_STATUS _kgmmuServiceBar2Faults_GV100(OBJGPU *pGpu, KernelGmmu *pKernelGmmu, NvU32 faultStatus, NvU32 mmuFaultClientId);
static NV_STATUS _kgmmuHandleReplayablePrivFault_GV100(OBJGPU *pGpu, KernelGmmu *pKernelGmmu,
                                                       MMU_FAULT_BUFFER_ENTRY *pParsedFaultEntry);

/**
 * @brief      Initialize the supported GMMU HW format structures.
 * @details    GV100+ supports ATS NV4K 64K PTE encoding
 *
 * @param      pKernelGmmu  The KernelGmmu
 * @param      pGpu         The gpu
 */
NV_STATUS
kgmmuFmtFamiliesInit_GV100(OBJGPU *pGpu, KernelGmmu *pKernelGmmu)
{
    extern NV_STATUS kgmmuFmtFamiliesInit_GM200(OBJGPU *pGpu, KernelGmmu *pKernelGmmu);
    NvU32            v;
    NV_STATUS        result;
    GMMU_FMT_FAMILY *pFam;

    // setup nv4kPte endcoding: v - 0, vol - 1, priv - 1
    for (v = 0; v < GMMU_FMT_MAX_VERSION_COUNT; ++v)
    {
        pFam = pKernelGmmu->pFmtFamilies[v];
        if (NULL != pFam)
        {
            nvFieldSetBool(&pFam->pte.fldValid, NV_FALSE, pFam->nv4kPte.v8);
            nvFieldSetBool(&pFam->pte.fldVolatile, NV_TRUE, pFam->nv4kPte.v8);
            nvFieldSetBool(&pFam->pte.fldPrivilege, NV_TRUE, pFam->nv4kPte.v8);
        }
    }

    // inherit former FmtFamilies setup procedure
    result = kgmmuFmtFamiliesInit_GM200(pGpu, pKernelGmmu);

    return result;
}

NV_STATUS
kgmmuChangeReplayableFaultOwnership_GV100(OBJGPU *pGpu, KernelGmmu *pKernelGmmu, NvBool bOwnedByRm)
{
    //
    // Disable the interrupt when RM loses the ownership and enable it back when
    // RM regains it. At least nvUvmInterfaceOwnPageFaultIntr() relies on that behavior.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
        return NV_OK;

    if (bOwnedByRm)
        pKernelGmmu->uvmSharedIntrRmOwnsMask |= RM_UVM_SHARED_INTR_MASK_MMU_REPLAYABLE_FAULT_NOTIFY;
    else
        pKernelGmmu->uvmSharedIntrRmOwnsMask &= ~RM_UVM_SHARED_INTR_MASK_MMU_REPLAYABLE_FAULT_NOTIFY;

    //
    // Notably don't set the PDB_PROP_GPU_IGNORE_REPLAYABLE_FAULTS property as
    // on Volta that would mean masking out all MMU faults from pending
    // interrupts.
    //

    return NV_OK;
}

/*!
 * @brief Creates the shadow fault buffer for client handling of replayable/non-replayable faults
 *
 * @param[in] pGpu
 * @param[in] pKernelGmmu
 *
 * @returns
 */
NV_STATUS
kgmmuClientShadowFaultBufferAlloc_GV100
(
    OBJGPU           *pGpu,
    KernelGmmu       *pKernelGmmu,
    FAULT_BUFFER_TYPE index
)
{
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
        return NV_OK;

    if (pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED))
    {
        NV_PRINTF(LEVEL_ERROR, "Fault-Buffer is disabled. ShadowBuffer cannot be created\n");
        NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_STATE);
    }

    return kgmmuClientShadowFaultBufferAllocate(pGpu, pKernelGmmu, index);
}

/*!
 * @brief Frees the shadow fault buffer for client handling of non-replayable faults
 *
 * @param[in] pGpu
 * @param[in] pKernelGmmu
 *
 * @returns
 */
NV_STATUS
kgmmuClientShadowFaultBufferFree_GV100
(
    OBJGPU           *pGpu,
    KernelGmmu       *pKernelGmmu,
    FAULT_BUFFER_TYPE index
)
{
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
        return NV_OK;

    return kgmmuClientShadowFaultBufferDestroy(pGpu, pKernelGmmu, index);
}

/*!
 * @brief Writes the ATS properties to the instance block
 *
 * @param[in] pKernelGmmu
 * @param[in] pVAS            OBJVASPACE pointer
 * @param[in] subctxId        subctxId value
 * @param[in] pInstBlkDesc    Memory descriptor for the instance block of the engine
 *
 * @returns NV_STATUS
 */
NV_STATUS
kgmmuInstBlkAtsGet_GV100
(
    KernelGmmu          *pKernelGmmu,
    OBJVASPACE          *pVAS,
    NvU32                subctxId,
    NvU32               *pOffset,
    NvU32               *pData
)
{
    NvU32 pasid = 0;

    if (subctxId == FIFO_PDB_IDX_BASE)
    {
        // A channel is setting base PDB with a valid VAS. Otherwise, it should fail.
        if (pVAS != NULL)
        {
            // Since ct_assert has to be done within compile time, it has to be at the top of the scope. Otherwise, the build fails.
            ct_assert(SF_WIDTH(NV_RAMIN_PASID) <= 32);

            //
            // The PASID value is provided by the OS and out of client control
            // however if the PASID value is invalid the ATS feature will not function
            // as expected so check sanity and fail early
            //
            NV_ASSERT_OR_RETURN(NV_OK == vaspaceGetPasid(pVAS, &pasid),
                        NV_ERR_INVALID_DATA);
            if (pasid > MASK_BITS(SF_WIDTH(NV_RAMIN_PASID)))
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Invalid PASID %d (max width %d bits)\n", pasid,
                          SF_WIDTH(NV_RAMIN_PASID));
                return NV_ERR_OPERATING_SYSTEM;
            }

            *pOffset = SF_OFFSET(NV_RAMIN_ENABLE_ATS);
            *pData   = SF_NUM(_RAMIN, _ENABLE_ATS, vaspaceIsAtsEnabled(pVAS)) |
                       SF_NUM(_RAMIN, _PASID, pasid);
        }
        else
        {
            // We cannot set base PDB without pVAS!
            NV_ASSERT_OR_RETURN(pVAS != NULL, NV_ERR_INVALID_STATE);
        }
    }
    else
    {
        // In subcontext supported PDB, we set valid values with non-NULL VAS. Otherwise, PDB entry is invalid.
        if (pVAS != NULL)
        {
            ct_assert(SF_WIDTH(NV_RAMIN_SC_PASID(0)) <= 32);

            //
            // set ATS for legacy PDB if SubctxId is set to be FIFO_PDB_IDX_BASE
            // Otherwise, set PDB with given SubctxId.
            //
            NV_ASSERT_OR_RETURN(NV_OK == vaspaceGetPasid(pVAS, &pasid),
                        NV_ERR_INVALID_DATA);

            if (pasid > MASK_BITS(SF_WIDTH(NV_RAMIN_SC_PASID(subctxId))))
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Invalid PASID %d (max width %d bits)\n", pasid,
                          SF_WIDTH(NV_RAMIN_SC_PASID(subctxId)));
                return NV_ERR_OPERATING_SYSTEM;
            }

            *pData = SF_IDX_NUM(_RAMIN_SC, _ENABLE_ATS, vaspaceIsAtsEnabled(pVAS), subctxId) |
                     SF_IDX_NUM(_RAMIN_SC, _PASID, pasid, subctxId);
        }
        else
        {
            //
            // If pVAS is NULL, that means the PDB of this SubctxId is set to Invalid.
            // In this case, ATS should be Disabled.
            //
            *pData = NV_RAMIN_ENABLE_ATS_FALSE;
        }

        *pOffset = SF_OFFSET(NV_RAMIN_SC_ENABLE_ATS(subctxId));
    }

    return NV_OK;
}

/*!
 * @brief This gets the offset and data for vaLimit
 *
 * @param[in]  pKernelGmmu
 * @param[in]  pVAS            OBJVASPACE pointer
 * @param[in]  subctxId        subctxId value
 * @param[in]  pParams         Pointer to the structure containing parameters passed by the engine
 * @param[out] pOffset         Pointer to offset of NV_RAMIN_ADR_LIMIT_LO:NV_RAMIN_ADR_LIMIT_HI pair
 * @param[out] pData           Pointer to value to write
 *
 * @returns NV_STATUS
 */
NV_STATUS
kgmmuInstBlkVaLimitGet_GV100
(
    KernelGmmu           *pKernelGmmu,
    OBJVASPACE           *pVAS,
    NvU32                 subctxId,
    INST_BLK_INIT_PARAMS *pParams,
    NvU32                *pOffset,
    NvU64                *pData
)
{
    extern NV_STATUS kgmmuInstBlkVaLimitGet_GP100(KernelGmmu *pKernelGmmu, OBJVASPACE *pVAS, NvU32 subctxId, INST_BLK_INIT_PARAMS *pParams, NvU32 *pOffset, NvU64 *pData);

    if (subctxId == FIFO_PDB_IDX_BASE)
    {
        return kgmmuInstBlkVaLimitGet_GP100(pKernelGmmu, pVAS, subctxId, pParams,
                    pOffset, pData);
    }

    *pOffset = 0;
    *pData   = 0;

    return NV_OK;
}

/*!
 * @brief This gets the offsets and data for the PDB limit
 *
 * @param[in] pGpu
 * @param[in] pKernelGmmu
 * @param[in] pVAS            OBJVASPACE pointer
 * @param[in] pParams         Pointer to the structure containing parameters passed by the engine
 * @param[in] subctxId        subctxId value
 * @param[out] pOffsetLo      Pointer to low offset
 * @param[out] pDataLo        Pointer to data written at above offset
 * @param[out] pOffsetHi      Pointer to high offset
 * @param[out] pDataHi        Pointer to data written at above offset
 *
 * @returns
 */
NV_STATUS
kgmmuInstBlkPageDirBaseGet_GV100
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu,
    OBJVASPACE           *pVAS,
    INST_BLK_INIT_PARAMS *pParams,
    NvU32                 subctxId,
    NvU32                *pOffsetLo,
    NvU32                *pDataLo,
    NvU32                *pOffsetHi,
    NvU32                *pDataHi
)
{
    extern NV_STATUS kgmmuInstBlkPageDirBaseGet_GP100(OBJGPU *pGpu, KernelGmmu *pKernelGmmu, OBJVASPACE *pVAS, INST_BLK_INIT_PARAMS *pParams, NvU32 subctxid, NvU32 *pOffsetLo, NvU32 *pDataLo, NvU32 *pOffsetHi, NvU32 *pDataHi);

    if (subctxId == FIFO_PDB_IDX_BASE)
    {
        return kgmmuInstBlkPageDirBaseGet_GP100(pGpu, pKernelGmmu, pVAS,
                    pParams, subctxId, pOffsetLo, pDataLo, pOffsetHi, pDataHi);
    }
    else
    {
        KernelFifo        *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
        MEMORY_DESCRIPTOR *pPDB  = NULL;

        if (pParams->bIsZombieSubctx)
        {
                pPDB = kfifoGetDummyPageMemDesc(pKernelFifo);

            NV_ASSERT_OR_RETURN((pPDB != NULL), NV_ERR_INVALID_STATE);
        }
        else if (pVAS != NULL)
        {
            pPDB = (pParams->bIsClientAdmin) ?
                           vaspaceGetKernelPageDirBase(pVAS, pGpu) :
                           vaspaceGetPageDirBase(pVAS, pGpu);
        }

        if (pPDB == NULL)
        {
            //
            // The teardown model for subcontext with UVM + CUDA is as follows:
            //
            // Step 1:  Unregister(vas)  -->  UnsetPageDirectory(vas)
            // Step 2:  FreeSubcontext(vas)
            //
            // But new subcontext can be added between step 1 & step 2.
            // Currently RM doesn't support the notion of a subcontext with NULL PDB.
            // This results in RM failing subsequent subcontext allocation, causing the UNBOUND instance block failure in bug 1823795.
            // To fix this, we will allow a subcontext to exist with invalid PDB until it is freed later.
            // This shouldn't cause any functional issue as no access memory shouldn't happen from this subcontext.

            *pDataLo = NV_RAMIN_SC_PAGE_DIR_BASE_TARGET_INVALID;
            *pDataHi = NV_RAMIN_SC_PAGE_DIR_BASE_TARGET_INVALID;
        }
        else
        {
            RmPhysAddr physAdd  = memdescGetPhysAddr(pPDB, AT_GPU, 0);
            NvU32      aperture = kgmmuGetHwPteApertureFromMemdesc(pKernelGmmu, pPDB);
            NvU32      addrLo   = NvU64_LO32(physAdd >> NV_RAMIN_BASE_SHIFT);

            //
            // Volta only supports new page table format and 64KB big page size so
            // forcing _USE_VER2_PT_FORMAT to _TRUE and _BIG_PAGE_SIZE to 64KB.
            //
            *pDataLo =
                SF_IDX_NUM(_RAMIN_SC_PAGE_DIR_BASE, _TARGET, aperture,subctxId) |
                ((pParams->bIsFaultReplayable)?
                    SF_IDX_DEF(_RAMIN_SC_PAGE_DIR_BASE, _FAULT_REPLAY_TEX, _ENABLED,  subctxId) |
                    SF_IDX_DEF(_RAMIN_SC_PAGE_DIR_BASE, _FAULT_REPLAY_GCC, _ENABLED,  subctxId) :
                    SF_IDX_DEF(_RAMIN_SC_PAGE_DIR_BASE, _FAULT_REPLAY_TEX, _DISABLED, subctxId) |
                    SF_IDX_DEF(_RAMIN_SC_PAGE_DIR_BASE, _FAULT_REPLAY_GCC, _DISABLED, subctxId)) |
                SF_IDX_DEF(_RAMIN_SC, _USE_VER2_PT_FORMAT, _TRUE, subctxId) |
                SF_IDX_DEF(_RAMIN_SC, _BIG_PAGE_SIZE, _64KB, subctxId) |
                SF_IDX_NUM(_RAMIN_SC_PAGE_DIR_BASE, _VOL, memdescGetVolatility(pPDB), subctxId) |
                SF_IDX_NUM(_RAMIN_SC_PAGE_DIR_BASE, _LO, addrLo, subctxId);

            *pDataHi = SF_IDX_NUM(_RAMIN_SC_PAGE_DIR_BASE, _HI, NvU64_HI32(physAdd), subctxId);
        }

        *pOffsetLo = SF_OFFSET(NV_RAMIN_SC_PAGE_DIR_BASE_LO(subctxId));
        *pOffsetHi = SF_OFFSET(NV_RAMIN_SC_PAGE_DIR_BASE_HI(subctxId));
     }

    return NV_OK;
}

/**
 * @brief Report MMU Fault buffer overflow errors. MMU Fault
 *        buffer overflow is a fatal error. Raise an assert and
 *        any client notifications if registered, to ensure
 *        overflow is debugged properly.
 *
 * @param[in]  pGpu
 * @param[in]  pKernelGmmu
 *
 * @returns
 */
NV_STATUS
kgmmuReportFaultBufferOverflow_GV100
(
    OBJGPU      *pGpu,
    KernelGmmu  *pKernelGmmu
)
{
    NV_STATUS rmStatus = NV_OK;
    NvU32   faultStatus = kgmmuReadMmuFaultStatus_HAL(pGpu, pKernelGmmu, GPU_GFID_PF);
    NvU32   faultBufferGet;
    NvU32   faultBufferPut;
    PEVENTNOTIFICATION *ppEventNotification  = NULL;
    NvU32   faultBufferSize;

    kgmmuReadFaultBufferGetPtr_HAL(pGpu, pKernelGmmu, NON_REPLAYABLE_FAULT_BUFFER,
                                  &faultBufferGet, NULL);
    faultBufferGet = DRF_VAL(_PFB_PRI, _MMU_FAULT_BUFFER_GET, _PTR, faultBufferGet);

    kgmmuReadFaultBufferPutPtr_HAL(pGpu, pKernelGmmu, NON_REPLAYABLE_FAULT_BUFFER,
                                  &faultBufferPut, NULL);
    faultBufferPut = DRF_VAL(_PFB_PRI, _MMU_FAULT_BUFFER_PUT, _PTR, faultBufferPut);

    faultBufferSize = kgmmuReadMmuFaultBufferSize_HAL(pGpu, pKernelGmmu, NON_REPLAYABLE_FAULT_BUFFER, GPU_GFID_PF);

    if (kgmmuIsNonReplayableFaultPending_HAL(pGpu, pKernelGmmu, NULL))
    {
        if (IsVOLTA(pGpu))
        {
            //
            // Check if Non_replayable interrupt is set when overflow is seen.
            // This shouldn't happen as this can cause a live-lock considering
            // top-half will kept on coming and will not let overflow interrupt
            // serviced. HW should disable the FAULT_INTR when overflow is
            // detected.
            //
            NV_PRINTF(LEVEL_ERROR, "MMU Fault: GPU %d: HW-BUG : "
                "NON_REPLAYABLE_INTR is high when OVERFLOW is detected\n",
                pGpu->gpuInstance);
            NV_ASSERT(0);
        }
        else
        {
            //
            // With message-based MMU interrupts (Turing onwards), it is
            // possible for us to get here - a real fault can happen while an
            // overflow happens, and there is no ordering guarantee about the
            // order of these interrupts in HW. However, if we write GET pointer
            // with GET != PUT while overflow is detected, the fault interrupt
            // will not be sent. Instead, the overflow interrupt will be sent,
            // so this will not cause an interrupt storm with message-based
            // interrupts. If HW does have a bug though, we'll see the below
            // print repeatedly which can point to a HW bug where it isn't
            // behaving the way it is designed to do.
            //
            NV_PRINTF(LEVEL_INFO, "MMU Fault: GPU %d: NON_REPLAYABLE_INTR "
                "is high when OVERFLOW is detected\n", pGpu->gpuInstance);
        }
    }

    // Check if overflow is due to incorrect fault buffer size or GET > SIZE
    if (FLD_TEST_DRF(_PFB_PRI, _MMU_FAULT_STATUS, _NON_REPLAYABLE_GETPTR_CORRUPTED, _SET, faultStatus) ||
        FLD_TEST_DRF(_PFB_PRI, _MMU_FAULT_STATUS, _REPLAYABLE_GETPTR_CORRUPTED, _SET, faultStatus))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "MMU Fault: GPU %d: Buffer overflow detected due to GET > SIZE\n",
                  pGpu->gpuInstance);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "MMU Fault: GPU %d: Buffer overflow detected due to incorrect SIZE\n",
                  pGpu->gpuInstance);

        NV_PRINTF(LEVEL_ERROR,
                  "MMU Fault: GPU %d: Buffer SIZE is expected to handle max faults "
                  "possible in system\n", pGpu->gpuInstance);
    }

    NV_PRINTF(LEVEL_ERROR,
              "MMU Fault: GPU %d: STATUS - 0x%x GET - 0x%x, PUT - 0x%x  SIZE - 0x%x\n",
              pGpu->gpuInstance, faultStatus, faultBufferGet, faultBufferPut,
              faultBufferSize);

    // Raise an event for Mods if registered as Mods checks for overflow
    if ((NV_OK == CliGetEventNotificationList(pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hFaultBufferClient,
                 pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hFaultBufferObject, NULL, &ppEventNotification)) && ppEventNotification)
    {
        MODS_ARCH_ERROR_PRINTF("MMU Fault Buffer overflow detected\n");
        rmStatus = notifyEvents(pGpu, *ppEventNotification, NVC369_NOTIFIER_MMU_FAULT_ERROR,
                                0, 0, NV_OK, NV_OS_WRITE_THEN_AWAKEN);
        //
        // Mods will check the error and clear error status. As Mods uses Async event
        // clearing the error in RM will cause a race with Mods
        //
        if (RMCFG_FEATURE_MODS_FEATURES)
            return rmStatus;
    }

    krcBreakpoint(GPU_GET_KERNEL_RC(pGpu));

    faultStatus = kgmmuReadMmuFaultStatus_HAL(pGpu, pKernelGmmu, GPU_GFID_PF);
    faultStatus = FLD_SET_DRF(_PFB_PRI, _MMU_FAULT_STATUS, _NON_REPLAYABLE_OVERFLOW, _RESET,
                              faultStatus);
    kgmmuWriteMmuFaultStatus_HAL(pGpu, pKernelGmmu, faultStatus);

    gpuMarkDeviceForReset(pGpu);
    return rmStatus;
}

/*!
 * @brief Get the engine ID associated with the Graphics Engine
 */
NvU32
kgmmuGetGraphicsEngineId_GV100
(
    KernelGmmu *pKernelGmmu
)
{
    return NV_PFAULT_MMU_ENG_ID_GRAPHICS;
}

/*!
 * @brief Reinit GMMU Peer PTE format to handle 47-bit peer addressing.
 *        This is controlled by NVSWITCH discovery and will not be enabled
 *        outside of specialized compute configurations.
 *
 * @param[in]   pGmmu  The valid gmmu
 */
NV_STATUS
kgmmuEnableNvlinkComputePeerAddressing_GV100(KernelGmmu *pKernelGmmu)
{
    NvU32       v;

    //
    // Recalculate the format structures
    //
    for (v = 0; v < GMMU_FMT_MAX_VERSION_COUNT; ++v)
    {
        if (!kgmmuFmtIsVersionSupported_HAL(pKernelGmmu, g_gmmuFmtVersions[v]))
            continue;

        kgmmuFmtInitPeerPteFld_HAL(pKernelGmmu, &pKernelGmmu->pFmtFamilies[v]->pte,
                                  g_gmmuFmtVersions[v]);
    }

    return NV_OK;
}

/**
 * @brief Print information about a MMU fault
 *
 * @param[in]  pGpu                  OBJGPU pointer
 * @param[in]  pKernelGmmu           KernelGmmu pointer
 * @param[in]  mmuFaultEngineId      Engine ID of the faulted engine
 * @param[in]  pMmuExceptionData     FIFO exception packet used
 *                                   for printing fault info
 * @returns
 */
void
kgmmuPrintFaultInfo_GV100
(
    OBJGPU                  *pGpu,
    KernelGmmu              *pKernelGmmu,
    NvU32                    mmuFaultEngineId,
    FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    NV_PRINTF(LEVEL_ERROR, "MMU Fault: inst:0x%x dev:0x%x subdev:0x%x\n",
              gpuGetInstance(pGpu), gpuGetDeviceInstance(pGpu),
              pGpu->subdeviceInstance);

    NV_PRINTF(LEVEL_ERROR,
        "MMU Fault: ENGINE 0x%x (%s %s) faulted @ 0x%x_%08x. Fault is of type 0x%x (%s). Access type is 0x%x (%s)",
        mmuFaultEngineId,
        kfifoPrintInternalEngine_HAL(pGpu, pKernelFifo, mmuFaultEngineId),
        kfifoGetClientIdString_HAL(pGpu, pKernelFifo, pMmuExceptionData),
        pMmuExceptionData->addrHi,
        pMmuExceptionData->addrLo,
        pMmuExceptionData->faultType,
        kgmmuGetFaultTypeString_HAL(GPU_GET_KERNEL_GMMU(pGpu),
                                    pMmuExceptionData->faultType),
        pMmuExceptionData->accessType,
        kfifoGetFaultAccessTypeString_HAL(pGpu, pKernelFifo,
                                          pMmuExceptionData->accessType));

    MODS_ARCH_ERROR_PRINTF("MMU Fault : ENGINE_%s %s %s %s",
        kfifoPrintInternalEngine_HAL(pGpu, pKernelFifo, mmuFaultEngineId),
        kgmmuGetFaultTypeString_HAL(GPU_GET_KERNEL_GMMU(pGpu),
                                    pMmuExceptionData->faultType),
        kfifoGetClientIdString_HAL(pGpu, pKernelFifo, pMmuExceptionData),
        kfifoGetFaultAccessTypeString_HAL(pGpu, pKernelFifo,
                                          pMmuExceptionData->accessType));

    {
        NvU32 engTag;
        NV_STATUS status = kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo, ENGINE_INFO_TYPE_MMU_FAULT_ID,
            mmuFaultEngineId, ENGINE_INFO_TYPE_ENG_DESC, &engTag);

        if ((status == NV_OK) && (IS_GR(engTag)))
        {
            NvU32 baseFaultId;

            if (pMmuExceptionData->bGpc)
            {
                NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_ERROR, " on GPC %d", pMmuExceptionData->gpcId);
            }

            status = kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo, ENGINE_INFO_TYPE_ENG_DESC,
                engTag, ENGINE_INFO_TYPE_MMU_FAULT_ID, &baseFaultId);
            if (status == NV_OK)
            {
                NvU32  subctxId = (mmuFaultEngineId - baseFaultId);

                NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_ERROR, " on VEID %d", subctxId);

            }
            else
            {
                DBG_BREAKPOINT();
            }
        }
    }

    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_ERROR, "\n");
    MODS_ARCH_ERROR_PRINTF("\n");
}

static NvBool
_kgmmuFaultBufferHasMapping(struct HW_FAULT_BUFFER *pFaultBuffer)
{
    return (pFaultBuffer->kernelVaddr || pFaultBuffer->pBufferPages) ? NV_TRUE : NV_FALSE;
}

static void
_kgmmuFaultEntryRmServiceable_GV100
(
    OBJGPU                 *pGpu,
    KernelGmmu             *pKernelGmmu,
    KernelFifo             *pKernelFifo,
    struct HW_FAULT_BUFFER *pFaultBuffer,
    NvU32                   entryIndex,
    NvU32                   hwBufferPut,
    NvBool                  bClientBufEnabled,
    NvBool                 *bRmServiceable,
    NvBool                 *bFaultValid,
    NvBool                  bPollForValidBit
)
{
    NvU32 faultEntry;
    NvBool bUvmHandledNonFatal, bEngineCE, bPbdmaFault;
    NvU32 engineId;
    NvU32 mwValid, mwUvmHandledNonFatal, mwEngineId;
    NvBool bUvmHandledReplayable;

    *bRmServiceable = NV_FALSE;

    // Get MW which contains Valid, replayable and Client Type
    mwValid = DRF_WORD_MW(NVC369_BUF_ENTRY_VALID);
    mwUvmHandledNonFatal = DRF_WORD_MW(NVC369_BUF_ENTRY_REPLAYABLE_FAULT_EN);
    mwEngineId = DRF_WORD_MW(NVC369_BUF_ENTRY_ENGINE_ID);

    //
    // Currently they all are in same DWORD, so we will read only one DWORD
    // Add assert for future HW changes
    //
    NV_ASSERT(mwValid == mwUvmHandledNonFatal);

    //
    // The caller specified that all packets between HW GET and HW PUT
    // need to be copied to the shadow buffer. RM will not optimize
    // the copy by skipping packets that are not marked valid and
    // rely on HW triggering a reentry to the top-half to service
    // the remaining faults.
    //
    if (bPollForValidBit == NV_TRUE)
    {
        RMTIMEOUT timeout;
        NV_STATUS status;

        //
        // For the replayable buffer we read HW put.
        // If the GET value equals PUT, we know there are no more faults to process.
        //
        if (entryIndex == hwBufferPut)
        {
            *bFaultValid = NV_FALSE;
            return;
        }

        gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

        do
        {
            faultEntry = MEM_RD32(((NvU32 *)kgmmuFaultBufferGetFault_HAL(pGpu, pKernelGmmu, pFaultBuffer, entryIndex) + mwValid));
            *bFaultValid = !!(NVBIT32(DRF_EXTENT_MW(NVC369_BUF_ENTRY_VALID)) & faultEntry);

            status = gpuCheckTimeout(pGpu, &timeout);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Timed out while waiting for valid bit.\n");
                gpuMarkDeviceForReset(pGpu);
                break;
            }
        } while (*bFaultValid != NV_TRUE);
    }
    else
    {
        faultEntry = MEM_RD32(((NvU32 *)kgmmuFaultBufferGetFault_HAL(pGpu, pKernelGmmu, pFaultBuffer, entryIndex) + mwValid));
        *bFaultValid = !!(NVBIT32(DRF_EXTENT_MW(NVC369_BUF_ENTRY_VALID)) & faultEntry);
    }

    bUvmHandledNonFatal = !!(NVBIT32(DRF_EXTENT_MW(NVC369_BUF_ENTRY_REPLAYABLE_FAULT_EN)) & faultEntry);
    bUvmHandledReplayable = !!(NVBIT32(DRF_EXTENT_MW(NVC369_BUF_ENTRY_REPLAYABLE_FAULT)) & faultEntry);

    //
    // Check engine Id. RM doesn't service CE faults with replayable_en bit set. Such faults are serviced by
    // clients. In case client wants to cancel such faults, it would need to make a RM control call for RCing
    // the channel.
    //
    if (*bFaultValid)
    {
        //
        // GPU is now done writing to this fault entry. A read memory barrier
        // here ensures that fault entry values are not read before the valid
        // bit is set. It is needed on architectures like PowerPC and ARM where
        // read instructions can be reordered.
        //
        portAtomicMemoryFenceLoad();

        faultEntry = MEM_RD32(((NvU32 *)kgmmuFaultBufferGetFault_HAL(pGpu, pKernelGmmu, pFaultBuffer, entryIndex) + mwEngineId));
        engineId = (faultEntry >> DRF_SHIFT_MW(NVC369_BUF_ENTRY_ENGINE_ID))
                                & DRF_MASK_MW(NVC369_BUF_ENTRY_ENGINE_ID);

        bEngineCE = ((engineId >= pKernelGmmu->minCeMmuFaultId) &&
                     (engineId <= pKernelGmmu->maxCeMmuFaultId));

        bPbdmaFault = kfifoIsMmuFaultEngineIdPbdma(pGpu, pKernelFifo, engineId);

        *bRmServiceable = (!bClientBufEnabled || !bUvmHandledNonFatal || !(bEngineCE || bPbdmaFault));

        if (gpuIsCCFeatureEnabled(pGpu) && gpuIsGspOwnedFaultBuffersEnabled(pGpu))
        {
            *bRmServiceable &= !bUvmHandledReplayable;
        }

    }
}

static inline NV_STATUS
_kgmmuCopyFaultPktInShadowBuf_GV100
(
    OBJGPU                   *pGpu,
    KernelGmmu               *pKernelGmmu,
    FAULT_BUFFER_TYPE         type,
    struct GMMU_FAULT_BUFFER *pFaultBuffer,
    NvU32                     getIndex,
    NvU32                     entriesToCopy,
    NvU32                     maxBufferEntries,
    NvBool                    bRmServiceable,
    THREAD_STATE_NODE        *pThreadState
)
{
    GMMU_SHADOW_FAULT_BUF *pRmShadowFaultBuf = NULL;
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuf = NULL;
    NvU32 copiedCount = 0;
    NvU32 copyIndex = 0;
    NvU32 origShadowBufPutIndex = 0;
    NvU32 shadowBufPutIndex = 0;
    struct HW_FAULT_BUFFER *pHwFaultBuffer;
    NV_STATUS status = NV_OK;

    pHwFaultBuffer = &pFaultBuffer->hwFaultBuffers[type];

    if (entriesToCopy == 0)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (bRmServiceable)
    {
        pRmShadowFaultBuf = KERNEL_POINTER_FROM_NvP64(GMMU_SHADOW_FAULT_BUF *, pFaultBuffer->pRmShadowFaultBuffer);
    }
    else
    {
        pClientShadowFaultBuf = pFaultBuffer->pClientShadowFaultBuffer[type];
        // Fetch the current put index for the appropriate shadow buffer
        shadowBufPutIndex = kgmmuReadClientShadowBufPutIndex_HAL(pGpu, pKernelGmmu,
                                                                 GPU_GFID_PF, type);
        origShadowBufPutIndex = shadowBufPutIndex;
    }

    if ((bRmServiceable && pRmShadowFaultBuf == NULL) ||
        (!bRmServiceable && pClientShadowFaultBuf == NULL))
    {
        return NV_ERR_INVALID_POINTER;
    }

    copyIndex = getIndex;

    while (entriesToCopy > 0)
    {
        if (bRmServiceable)
        {
            copiedCount = queuePush(pRmShadowFaultBuf,
                                    kgmmuFaultBufferGetFault_HAL(pGpu, pKernelGmmu, pHwFaultBuffer, copyIndex), 1);
            if (copiedCount != 1)
            {
                return NV_ERR_NO_MEMORY;
            }
        }
        else
        {
            status = kgmmuCopyFaultPacketToClientShadowBuffer_HAL(pGpu, pKernelGmmu,
                                                                  pFaultBuffer,
                                                                  type,
                                                                  copyIndex,
                                                                  shadowBufPutIndex,
                                                                  maxBufferEntries,
                                                                  pThreadState,
                                                                  &copiedCount);
            if (status != NV_OK)
            {
                return status;
            }

            // If nothing was copied, but the status is NV_OK check if PUT needs to be updated.
            if (copiedCount == 0)
            {
                //
                // Assert we only end up here in case of Replayable faults.
                // Non-Replayable copies always need to succeed.
                //
                NV_ASSERT_OR_RETURN(type == REPLAYABLE_FAULT_BUFFER, NV_ERR_INVALID_STATE);

                if (shadowBufPutIndex != origShadowBufPutIndex)
                {
                    goto update_client_put;
                }


                // Signal the caller that no copies have taken place.
                return NV_WARN_NOTHING_TO_DO;
            }

            shadowBufPutIndex = (shadowBufPutIndex + 1) % maxBufferEntries;
        }

        entriesToCopy--;
        copyIndex++;
        if (copyIndex == maxBufferEntries)
        {
            copyIndex = 0;
        }
    }

update_client_put:
    // Update the put index for CPU driver
    if (!bRmServiceable)
    {
        // Make sure the packet reaches memory before writing the PUT.
        portAtomicMemoryFenceStore();
        kgmmuWriteClientShadowBufPutIndex_HAL(pGpu, pKernelGmmu, GPU_GFID_PF,
                                              type, shadowBufPutIndex);
    }

    return status;
}

/*!
 * @brief Copy fault packets from RM owned HW fault buffer to Shadow faultBuffers
 *
 * @param[in]  pGpu             OBJGPU pointer
 * @param[in]  pKernelGmmu      KernelGmmu pointer
 * @param[in]  faultBufferType  Fault buffer Type [Replayable/Non-Replyabale]
 * @param[in]  pShadowFaultBuf  Shadow fault buffer pointer
 * @param[out] entriesCopied    Number of fault packets copied into shadow buffer
 * @param[in]  type             Replayable or non-replayable fault buffer
 *
 * @returns
 */
NV_STATUS
kgmmuCopyMmuFaults_GV100
(
     OBJGPU            *pGpu,
     KernelGmmu        *pKernelGmmu,
     THREAD_STATE_NODE *pThreadState,
     NvU32             *entriesCopied,
     FAULT_BUFFER_TYPE  type,
     NvBool             bPollForValidBit
)
{
    NV_STATUS rmStatus = NV_OK;
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    Intr *pIntr = GPU_GET_INTR(pGpu);
    GMMU_SHADOW_FAULT_BUF *pRmShadowFaultBuf = NULL;
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuf = NULL;
    NvU32 nextGetIndex = 0, curGetIndex = 0;
    NvU32 totalCount = 0, curCount = 0, rmPktCount = 0;
    NvU32 maxBufferEntries = 0;
    NvBool bRmServiceable = NV_FALSE, bPrevRmServiceable = NV_FALSE;
    NvBool bFaultValid = NV_FALSE;
    struct HW_FAULT_BUFFER *pHwFaultBuffer;
    struct GMMU_FAULT_BUFFER *pFaultBuffer;
    NvU32 hwBufferPut = 0;

    ct_assert(NV_PFB_PRI_MMU_NON_REPLAY_FAULT_BUFFER == NON_REPLAYABLE_FAULT_BUFFER);
    ct_assert(NV_PFB_PRI_MMU_REPLAY_FAULT_BUFFER == REPLAYABLE_FAULT_BUFFER);

    NV_ASSERT_OR_RETURN(type == REPLAYABLE_FAULT_BUFFER ||
                        type == NON_REPLAYABLE_FAULT_BUFFER,
                        NV_ERR_INVALID_ARGUMENT);

    *entriesCopied = 0;
    pFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF];
    pHwFaultBuffer = &pFaultBuffer->hwFaultBuffers[type];

    //
    // Sanity checks to see if SW is ready to handle interrupts. If interrupts are
    // not enabled in SW return fine as we don't want to error out top half.
    //
    if ((pIntr == NULL) || (intrGetIntrEn(pIntr) == INTERRUPT_TYPE_DISABLED) ||
        pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED) ||
        !pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pShadowFaultBufLock)
    {
        return NV_OK;
    }

    if ((!gpuIsCCFeatureEnabled(pGpu) || !gpuIsGspOwnedFaultBuffersEnabled(pGpu)) &&
        (type == REPLAYABLE_FAULT_BUFFER))
    {
        return NV_OK;
    }

    // If there is no replayable buffer registered, then there is no work to be done.
    if (type == REPLAYABLE_FAULT_BUFFER &&
        !pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_REPLAYABLE_FAULT_BUFFER_IN_USE))
    {
        // RM still needs to clear the interrupt to avoid an interrupt storm.
        kgmmuClearReplayableFaultIntr_HAL(pGpu, pKernelGmmu, pThreadState);
        return NV_OK;
    }

    portSyncSpinlockAcquire(pFaultBuffer->pShadowFaultBufLock);

    pRmShadowFaultBuf = KERNEL_POINTER_FROM_NvP64(GMMU_SHADOW_FAULT_BUF *, pFaultBuffer->pRmShadowFaultBuffer);
    pClientShadowFaultBuf = KERNEL_POINTER_FROM_NvP64(GMMU_CLIENT_SHADOW_FAULT_BUFFER *,
                                                      pFaultBuffer->pClientShadowFaultBuffer[type]);

    // Max entries exposed in register takes care of 1 entry used for Full detection
    NV_ASSERT(pHwFaultBuffer->faultBufferSize);
    maxBufferEntries = (pHwFaultBuffer->faultBufferSize / NVC369_BUF_SIZE);

    if (!_kgmmuFaultBufferHasMapping(pHwFaultBuffer))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU %d HW's fault buffer doesn't have kernel mappings\n",
                  pGpu->gpuInstance);
        rmStatus = NV_ERR_INVALID_STATE;
        goto done;
    }

    if (pRmShadowFaultBuf == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU %d RM's shadow buffer should be setup\n",
                  pGpu->gpuInstance);
        rmStatus = NV_ERR_INVALID_STATE;
        goto done;
    }

    //
    // For SRIOV vGPU, GET and PUT reg gets reset to zero during migration.
    // This results in wrong fault buffer entry referenced by RM after migration
    // for next fault entry read. So for SRIOV vgpu setup, instead of using RM cached value
    // always read NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_GET(index) reg in order to
    // identify next fault entry to read.
    //
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        kgmmuReadFaultBufferGetPtr_HAL(pGpu, pKernelGmmu, type,
                                      &pHwFaultBuffer->cachedGetIndex, pThreadState);
    }

    nextGetIndex = curGetIndex = pHwFaultBuffer->cachedGetIndex;

    if (bPollForValidBit == NV_TRUE)
    {
        kgmmuReadFaultBufferPutPtr_HAL(pGpu, pKernelGmmu, type,
                                       &hwBufferPut, pThreadState);
    }

    if (type == NON_REPLAYABLE_FAULT_BUFFER)
    {
        // Clear non replayable fault pulse interrupt
        kgmmuClearNonReplayableFaultIntr_HAL(pGpu, pKernelGmmu, pThreadState);
    }
    else
    {
        // Clear replayable fault pulse interrupt
        kgmmuClearReplayableFaultIntr_HAL(pGpu, pKernelGmmu, pThreadState);
    }

    //
    // Check how many entries are valid and serviceable by one driver in HW fault buffer.
    // We copy everything in one go to optimize copy performance
    //
    while (totalCount < maxBufferEntries)
    {
        _kgmmuFaultEntryRmServiceable_GV100(pGpu, pKernelGmmu, pKernelFifo,
                                            pHwFaultBuffer, nextGetIndex, hwBufferPut, (pClientShadowFaultBuf != NULL),
                                            &bRmServiceable, &bFaultValid, bPollForValidBit);

        if (!bFaultValid)
            break;

        // Non replayable fault copy path.
        if (type == NON_REPLAYABLE_FAULT_BUFFER)
        {
            // Check if servicing entity has changed
            if ((bPrevRmServiceable != bRmServiceable) && (curCount > 0))
            {
                rmStatus = _kgmmuCopyFaultPktInShadowBuf_GV100(pGpu, pKernelGmmu,
                                                               type,
                                                               pFaultBuffer,
                                                               curGetIndex,
                                                               curCount,
                                                               maxBufferEntries,
                                                               bPrevRmServiceable,
                                                               pThreadState);
                if (rmStatus != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Failed to copy faults into GPU %d's %s shadow buffer\n",
                              pGpu->gpuInstance,
                              (bPrevRmServiceable ? "RM" : "Client"));
                    gpuMarkDeviceForReset(pGpu);
                    goto done;
                }

                curGetIndex = (curGetIndex + curCount) % maxBufferEntries;
                curCount = 0;
            }
        }

        //
        // A Replayable fault is never RM servicable.
        // As a result, we will never perform the fault copy within this loop,
        // but only count the number of entries.
        //
        // The number of entries is used outside of the loop to
        // determine the number of packets to copy.
        //
        if (type == REPLAYABLE_FAULT_BUFFER)
        {
            NV_ASSERT_OR_RETURN(!bRmServiceable, NV_ERR_INVALID_STATE);
        }

        if (curCount == 0)
        {
            bPrevRmServiceable = bRmServiceable;
        }

        if (bRmServiceable)
        {
            rmPktCount++;
        }

        curCount++;
        totalCount++;
        nextGetIndex++;
        if (nextGetIndex == maxBufferEntries)
        {
            nextGetIndex = 0;
            if (type == NON_REPLAYABLE_FAULT_BUFFER)
            {
                pFaultBuffer->faultBufferGenerationCounter++;
            }
        }
    }

    // Nothing to copy
    if (totalCount == 0)
    {
        //
        // Writing to GET will cause the GET != PUT condition to get reevaluated
        // and the interrupt signal will be sent again if they are not equal.
        // This is necessary because the non replayable fault interrupt was cleared
        // earlier. Once the packet is ready and it is copied, GET == PUT and the
        // interrupt will not be retriggered.
        //

        NvU32 val;
        val = pHwFaultBuffer->cachedGetIndex;

        kgmmuWriteFaultBufferGetPtr_HAL(pGpu, pKernelGmmu, type,
                                        val, pThreadState);
        goto done;
    }

    // Copy all remaining entries
    if (curCount > 0)
    {
        rmStatus = _kgmmuCopyFaultPktInShadowBuf_GV100(pGpu, pKernelGmmu,
                                                       type,
                                                       pFaultBuffer,
                                                       curGetIndex,
                                                       curCount,
                                                       maxBufferEntries,
                                                       bPrevRmServiceable,
                                                       pThreadState);
        //
        // NV_WARN_NOTHING_TO_DO signals that no faults have been copied
        // in the replayable fault handling case. There is no need to notify the client.
        //
        if (rmStatus == NV_WARN_NOTHING_TO_DO)
        {
            rmStatus = NV_OK;
            goto done;
        }
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to copy faults into GPU %d's %s shadow buffer\n",
                      pGpu->gpuInstance,
                      (bPrevRmServiceable ? "RM" : "Client"));
            gpuMarkDeviceForReset(pGpu);
            goto done;
        }
    }

    //
    // Reset the valid bit in all these fault packets. It's easy to zero down the overall packets rather than writing
    // INVALID to one packet at a time
    //
    curGetIndex = pHwFaultBuffer->cachedGetIndex;
    curCount = totalCount;

    if ((curGetIndex + curCount) > maxBufferEntries)
    {
        kgmmuFaultBufferClearPackets_HAL(pGpu, pKernelGmmu, pHwFaultBuffer, curGetIndex, maxBufferEntries - curGetIndex);
        curCount = totalCount - (maxBufferEntries - curGetIndex);
        curGetIndex = 0;
    }

    kgmmuFaultBufferClearPackets_HAL(pGpu, pKernelGmmu, pHwFaultBuffer, curGetIndex, curCount);

    //
    // Ensure fatalFaultIntrPending is set only after fault buffer entries have been
    // copied into shadow buffer.
    //
    portAtomicMemoryFenceStore();

    //
    // Set the SW flag needed for interrupt processing. This should be set before we write GET pointer as moving
    // GET ptr can disable top level interrupt and we should not have a window where an interrupt pending state is lost
    // As we don't mask these interrupts, top level interrupts will be reset as soon as GET == PUT after copy. McService
    // in RM's bottom_half relies on top level interrupts for servicing, hence we need a SW flag to state that there is
    // an intr pending for servicing.
    //
    portAtomicSetS32(&pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].fatalFaultIntrPending, 1);

    //
    // Ensure all writes to the current entry are completed before updating the
    // GET pointer.
    //
    portAtomicMemoryFenceStore();

    // Increment the GET pointer to enable HW to write new fault packets
    kgmmuWriteFaultBufferGetPtr_HAL(pGpu, pKernelGmmu, type, nextGetIndex, pThreadState);

    *entriesCopied = rmPktCount;
    pHwFaultBuffer->cachedGetIndex = nextGetIndex;

done:
    portSyncSpinlockRelease(pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pShadowFaultBufLock);
    return rmStatus;
}

void
kgmmuFaultBufferClearPackets_GV100
(
    OBJGPU                 *pGpu,
    KernelGmmu             *pKernelGmmu,
    struct HW_FAULT_BUFFER *pFaultBuffer,
    NvU32                   beginIdx,
    NvU32                   numFaultPackets
)
{
    if (pFaultBuffer->kernelVaddr)
    {
        void *bufferAddr = (void *)(NvUPtr)(KERNEL_POINTER_FROM_NvP64(NvU64, pFaultBuffer->kernelVaddr) +
                                            (NvU64)(beginIdx * sizeof(GMMU_FAULT_PACKET)));
        portMemSet(bufferAddr, 0, sizeof(GMMU_FAULT_PACKET) * numFaultPackets);
    }
    else
    {
        NvU32 pktPerPage = RM_PAGE_SIZE / sizeof(GMMU_FAULT_PACKET);

        NvU32 pageNumber = beginIdx * sizeof(GMMU_FAULT_PACKET) / RM_PAGE_SIZE;
        NvU32 fstPktInPage = beginIdx % (RM_PAGE_SIZE / sizeof(GMMU_FAULT_PACKET));
        NvU32 clearInThisPage = NV_MIN(numFaultPackets, pktPerPage - fstPktInPage);

        while (numFaultPackets > 0)
        {
            NvP64 pAddress = pFaultBuffer->pBufferPages[pageNumber].pAddress;

            void *bufferAddr = (void *)(NvUPtr)(KERNEL_POINTER_FROM_NvP64(NvU64, pAddress) + fstPktInPage * sizeof(GMMU_FAULT_PACKET));

            portMemSet(bufferAddr, 0, clearInThisPage * sizeof(GMMU_FAULT_PACKET));

            pageNumber++;
            fstPktInPage = 0;
            numFaultPackets -= clearInThisPage;
            clearInThisPage = NV_MIN(pktPerPage, numFaultPackets);
        }
    }
}

GMMU_FAULT_PACKET *
kgmmuFaultBufferGetFault_GV100
(
    OBJGPU                 *pGpu,
    KernelGmmu             *pKernelGmmu,
    struct HW_FAULT_BUFFER *pFaultBuffer,
    NvU32                   idx)
{
    if (pFaultBuffer->kernelVaddr)
    {
        void *pFault = (void *)(NvUPtr)(KERNEL_POINTER_FROM_NvP64(NvU64, pFaultBuffer->kernelVaddr) +
                                        (NvU64)(idx * sizeof(GMMU_FAULT_PACKET)));
        return pFault;
    }
    else
    {
        NvU32 pageNumber = idx * sizeof(GMMU_FAULT_PACKET) / RM_PAGE_SIZE;
        NvP64 pAddress = pFaultBuffer->pBufferPages[pageNumber].pAddress;
        NvU32 inPageIdx = idx % (RM_PAGE_SIZE / sizeof(GMMU_FAULT_PACKET));
        void *pFault = (void *)(NvUPtr)(KERNEL_POINTER_FROM_NvP64(NvU64, pAddress) +
                                        inPageIdx * sizeof(GMMU_FAULT_PACKET));

        return pFault;
    }
}

/*
 * @brief Copies a single fault packet from the replayable/non-replayable
 *        HW fault buffer to the corresponding client shadow buffer
 *
 * @param[in]  pFaultBuffer       Pointer to GMMU_FAULT_BUFFER
 * @param[in]  type               Replayable/Non-replayable fault type
 * @param[in]  getIndex           Get pointer of the HW fault buffer
 * @param[in]  shadowBufPutIndex  Put pointer of the shadow buffer
 * @param[in]  maxBufferEntries   Maximum possible entries in the HW buffer
 * @param[in]  pThreadState       Pointer to THREAD_STATE_NODE
 * @param[out] pFaultsCopied      Number of fault packets copied by the function
 *
 * @returns NV_STATUS
 */
NvU32
kgmmuCopyFaultPacketToClientShadowBuffer_GV100
(
    OBJGPU                   *pGpu,
    KernelGmmu               *pKernelGmmu,
    struct GMMU_FAULT_BUFFER *pFaultBuffer,
    FAULT_BUFFER_TYPE         type,
    NvU32                     getIndex,
    NvU32                     shadowBufPutIndex,
    NvU32                     maxBufferEntries,
    THREAD_STATE_NODE        *pThreadState,
    NvU32                    *pFaultsCopied
)
{
    GMMU_SHADOW_FAULT_BUF *pQueue = NULL;
    struct HW_FAULT_BUFFER *pHwFaultBuffer = NULL;
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuf = NULL;

    pHwFaultBuffer = &pFaultBuffer->hwFaultBuffers[type];
    pClientShadowFaultBuf = pFaultBuffer->pClientShadowFaultBuffer[type];

    pQueue = (GMMU_SHADOW_FAULT_BUF *) pClientShadowFaultBuf->pQueueAddress;

    *pFaultsCopied = queuePushNonManaged(pQueue, &pClientShadowFaultBuf->queueContext,
                                         kgmmuFaultBufferGetFault_HAL(pGpu, pKernelGmmu,
                                                                      pHwFaultBuffer,
                                                                      getIndex),
                                         1);
    if (*pFaultsCopied == 0)
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    return NV_OK;
}


NV_STATUS
kgmmuInitCeMmuFaultIdRange_GV100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    pKernelGmmu->minCeMmuFaultId = NV_PFAULT_MMU_ENG_ID_CE0;
    pKernelGmmu->maxCeMmuFaultId = NV_PFAULT_MMU_ENG_ID_CE8;

    NV_PRINTF(LEVEL_INFO, "CE MMU Fault ID range [0x%x - 0x%x]\n",
                           pKernelGmmu->minCeMmuFaultId, pKernelGmmu->maxCeMmuFaultId);

    return NV_OK;
}


/*!
 * @brief Creates shadow fault buffer for RM fatal fault handling
 *
 * @param[in] pGpu            OBJGPU pointer
 * @param[in] pKernelGmmu     KernelGmmu pointer
 *
 * @returns
 */
static NV_STATUS
_kgmmuAllocShadowFaultBuffer_GV100
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu
)
{
    NV_STATUS rmStatus = NV_OK;
    GMMU_SHADOW_FAULT_BUF *gmmuShadowFaultBuf = NULL;
    NvU32 queueMaxEntries = 0;
    struct HW_FAULT_BUFFER *pFaultBuffer;

    // NvPort should be initialized in RM before these calls are made
    if (!portIsInitialized())
    {
        NV_PRINTF(LEVEL_ERROR, "NvPort needed but not initaiized\n");
        NV_ASSERT(0);
    }

    if (pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED))
    {
        NV_PRINTF(LEVEL_ERROR, "Fault-Buffer is disabled. ShadowBuffer cannot be created\n");
        NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_STATE);
    }

    pFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hwFaultBuffers[NV_PFB_PRI_MMU_NON_REPLAY_FAULT_BUFFER];
    NV_ASSERT_OR_RETURN(pFaultBuffer->faultBufferSize != 0, NV_ERR_INVALID_ARGUMENT);

    // Allocate memory for queue dataStructure and initialize queue
    gmmuShadowFaultBuf = portMemAllocNonPaged(sizeof(GMMU_SHADOW_FAULT_BUF));
    NV_ASSERT_OR_RETURN(gmmuShadowFaultBuf != NULL, NV_ERR_NO_MEMORY);

    queueMaxEntries = (pFaultBuffer->faultBufferSize / NVC369_BUF_SIZE);
    rmStatus = queueInit(gmmuShadowFaultBuf, portMemAllocatorGetGlobalNonPaged(), queueMaxEntries);
    if (rmStatus != NV_OK)
        goto error;

    //
    // SpinLock needed to protect shadow buffer setup as it gets accessed
    // from top half and clientShadowBuffer can get setup/destroy later
    //
    pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pShadowFaultBufLock = portSyncSpinlockCreate(portMemAllocatorGetGlobalNonPaged());
    if (!pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pShadowFaultBufLock)
    {
        rmStatus = NV_ERR_INSUFFICIENT_RESOURCES;
        goto error;
    }

    pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pRmShadowFaultBuffer = NV_PTR_TO_NvP64(gmmuShadowFaultBuf);
    return NV_OK;

error:
    portMemFree(gmmuShadowFaultBuf);
    return rmStatus;
}

NV_STATUS
kgmmuFaultBufferInit_GV100
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu
)
{
    NV_STATUS status;
    NvU32 faultBufferSize;

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED))
        return NV_OK;

    ct_assert(NV_PFB_PRI_MMU_REPLAY_FAULT_BUFFER == REPLAYABLE_FAULT_BUFFER);
    ct_assert(NV_PFB_PRI_MMU_NON_REPLAY_FAULT_BUFFER == NON_REPLAYABLE_FAULT_BUFFER);

    faultBufferSize = kgmmuSetAndGetDefaultFaultBufferSize_HAL(pGpu, pKernelGmmu,
                                                               NON_REPLAYABLE_FAULT_BUFFER,
                                                               GPU_GFID_PF);
    status = kgmmuFaultBufferAlloc(pGpu, pKernelGmmu,
                                   NON_REPLAYABLE_FAULT_BUFFER,
                                   faultBufferSize);
    if (status != NV_OK)
    {
        return status;
    }

    // Allocate shadow fault buffer for fatal fault handling inside RM
    status = _kgmmuAllocShadowFaultBuffer_GV100(pGpu, pKernelGmmu);
    if (status != NV_OK)
    {
        (void) kgmmuFaultBufferFree(pGpu, pKernelGmmu, NON_REPLAYABLE_FAULT_BUFFER);
        return status;
    }

    return NV_OK;
}

NV_STATUS
kgmmuFaultBufferLoad_GV100
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu,
    NvU32                 index,
    NvU32                 gfid
)
{
    NV_STATUS status;

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED))
        return NV_OK;

    // Skip the map if this is a GC6 cycle as FB is preserved.
    if (!IS_GPU_GC6_STATE_EXITING(pGpu))
    {
        status = kgmmuFaultBufferMap(pGpu, pKernelGmmu, index, gfid);
        if (status != NV_OK)
        {
            return status;
        }
        // Init the PUT pointer PRI before use. Applicable only to Hopper CC
        kgmmuWriteClientShadowBufPutIndex_HAL(pGpu, pKernelGmmu, gfid, index, 0);
    }

    status = kgmmuEnableFaultBuffer_HAL(pGpu, pKernelGmmu, index, NV_FALSE, gfid);
    if (status != NV_OK)
    {
        (void) kgmmuFaultBufferUnmap(pGpu, pKernelGmmu, index, gfid);
        return status;
    }

    return NV_OK;
}

NV_STATUS
kgmmuEnableFaultBuffer_GV100
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu,
    NvU32                 index,
    NvBool                bIsErrorRecovery,
    NvU32                 gfid
)
{
    NvU32 faultBufferHi;
    NvU32 faultBufferLo;
    NvU32 regVal;
    struct HW_FAULT_BUFFER *pFaultBuffer;

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)) ||
        pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED))
    {
        return NV_OK;
    }

    NV_ASSERT_OR_RETURN((index < NUM_FAULT_BUFFERS), NV_ERR_INVALID_ARGUMENT);

    pFaultBuffer = &pKernelGmmu->mmuFaultBuffer[gfid].hwFaultBuffers[index];

    faultBufferHi = NvU64_HI32(pFaultBuffer->bar2FaultBufferAddr);
    faultBufferLo = NvU64_LO32(pFaultBuffer->bar2FaultBufferAddr);
    kgmmuWriteMmuFaultBufferHiLo_HAL(pGpu, pKernelGmmu, index, faultBufferLo, faultBufferHi, gfid);

    kgmmuSetAndGetDefaultFaultBufferSize_HAL(pGpu, pKernelGmmu, index, gfid);

    regVal = kgmmuReadMmuFaultBufferSize_HAL(pGpu, pKernelGmmu, index, gfid);
    regVal = FLD_SET_DRF(_PFB_PRI, _MMU_FAULT_BUFFER_SIZE, _ENABLE, _TRUE, regVal);

    if (index == NON_REPLAYABLE_FAULT_BUFFER)
    {
        //
        // Non-Replayable Fault buffer needs overflow interrupt reporting
        // as overflow is considered fatal due to fault packet loss. Also
        // this interrupt will disable non_replayable interrupt when raised.
        //
        regVal = FLD_SET_DRF(_PFB_PRI, _MMU_FAULT_BUFFER_SIZE, _OVERFLOW_INTR, _ENABLE, regVal);
    }
    else
    {
        //
        // Replayable Fault buffer does not need overflow interrupt reporting
        // as overflow is not considered fatal. There is no fault packet loss
        // due to replays.
        //
        regVal = FLD_SET_DRF(_PFB_PRI, _MMU_FAULT_BUFFER_SIZE, _OVERFLOW_INTR, _DISABLE, regVal);
    }

    kgmmuWriteMmuFaultBufferSize_HAL(pGpu, pKernelGmmu, index, regVal, gfid);
    // Don't touch interrupts if called in error recovery path
    if (!bIsErrorRecovery)
    {
        kgmmuEnableMmuFaultInterrupts_HAL(pGpu, pKernelGmmu, index);
        kgmmuEnableMmuFaultOverflowIntr_HAL(pGpu, pKernelGmmu, index);
    }
    return NV_OK;
}

NV_STATUS
kgmmuDisableFaultBuffer_GV100
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu,
    NvU32                 index,
    NvBool                bIsErrorRecovery,
    NvU32                 gfid
)
{
    NV_STATUS rmStatus = NV_OK;
    NvU32     faultBufferSize;
    NV_ASSERT_OR_RETURN((index < NUM_FAULT_BUFFERS), NV_ERR_INVALID_ARGUMENT);

    if (pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_OK;
    }

    rmStatus = gpuSanityCheckRegisterAccess(pGpu, 0, NULL);
    if (rmStatus != NV_OK)
        return rmStatus;

    //
    // Before disabling fault buffer make sure that no packets are pending in pipe.
    // The status register here provides status for all fault buffers(replayable/non-replayable)
    // and ideally we should have separate status register for these buffers. This is tracked in
    // Bug 1848948
    //
    if (!bIsErrorRecovery)
    {
        RMTIMEOUT timeout;
        gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

        while (FLD_TEST_DRF(_PFB_PRI, _MMU_FAULT_STATUS, _BUSY, _TRUE,
                kgmmuReadMmuFaultStatus_HAL(pGpu, pKernelGmmu, gfid)))
        {
            rmStatus = gpuCheckTimeout(pGpu, &timeout);
            if (rmStatus == NV_ERR_TIMEOUT)
            {
                NV_PRINTF(LEVEL_ERROR, "HW couldn't flush %s buffer.\n",
                          (index == REPLAYABLE_FAULT_BUFFER) ? "REPLAYABLE_FAULT_BUFFER" : "NON_REPLAYABLE_FAULT_BUFFER");
                DBG_BREAKPOINT();
                break;
            }
        }
    }

    faultBufferSize = kgmmuReadMmuFaultBufferSize_HAL(pGpu, pKernelGmmu, index, gfid);
    faultBufferSize = FLD_SET_DRF(_PFB_PRI, _MMU_FAULT_BUFFER_SIZE, _ENABLE, _FALSE,
                                  faultBufferSize);
    kgmmuWriteMmuFaultBufferSize_HAL(pGpu, pKernelGmmu, index, faultBufferSize, gfid);

    return rmStatus;
}

NV_STATUS
kgmmuFaultBufferDestroy_GV100
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu
)
{
    NV_STATUS status;

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED))
        return NV_OK;

    status = kgmmuClientShadowFaultBufferFree_HAL(pGpu, pKernelGmmu, NON_REPLAYABLE_FAULT_BUFFER);
    if (status != NV_OK)
        return status;

    // Free RM shadow fault buffer created for fatal fault handling
    if (pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pRmShadowFaultBuffer)
    {
        queueDestroy(KERNEL_POINTER_FROM_NvP64(GMMU_SHADOW_FAULT_BUF *,
                                               pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pRmShadowFaultBuffer));
        portMemFree(NvP64_VALUE(pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pRmShadowFaultBuffer));
        pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pRmShadowFaultBuffer = NvP64_NULL;
    }

    if (pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pShadowFaultBufLock)
    {
        portSyncSpinlockDestroy(pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pShadowFaultBufLock);
        pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pShadowFaultBufLock = NULL;
    }

    (void) kgmmuFaultBufferFree(pGpu, pKernelGmmu, NON_REPLAYABLE_FAULT_BUFFER);

    return NV_OK;
}

NV_STATUS
kgmmuFaultBufferUnload_GV100
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu,
    NvU32                 index,
    NvU32                 gfid
)
{
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED))
        return NV_OK;

    kgmmuDisableFaultBuffer_HAL(pGpu, pKernelGmmu, index, NV_FALSE, gfid);

    // Skip the unmap if this is a GC6 cycle as FB is preserved.
    if (!IS_GPU_GC6_STATE_ENTERING(pGpu))
        kgmmuFaultBufferUnmap(pGpu, pKernelGmmu, index, gfid);

    return NV_OK;
}

/**
  * @brief  Sign extend a fault address to a supported width as per UVM requirements
  */
void
kgmmuSignExtendFaultAddress_GV100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU64      *pMmuFaultAddress
)
{
    NvU32 cpuAddrShift   = osGetCpuVaAddrShift();
    NvU32 gpuVaAddrShift = portUtilCountTrailingZeros64(pKernelGmmu->maxVASize);

    // Sign extend VA to ensure it's in canonical form if required
    if (gpuVaAddrShift >= cpuAddrShift)
    {
        switch (pGpu->busInfo.oorArch)
        {
            case OOR_ARCH_X86_64:
            case OOR_ARCH_ARM:
            case OOR_ARCH_AARCH64:
                *pMmuFaultAddress = (NvU64)(((NvS64)*pMmuFaultAddress << (64 - cpuAddrShift)) >>
                                            (64 - cpuAddrShift));
                break;
            case OOR_ARCH_PPC64LE:
                break;
            case OOR_ARCH_NONE:
                NV_ASSERT_FAILED("Invalid oor address mode type.");
                break;
        }
    }
    else
    {
        switch (pGpu->busInfo.oorArch)
        {
            case OOR_ARCH_X86_64:
                *pMmuFaultAddress = (NvU64)(((NvS64)*pMmuFaultAddress << (64 - 48)) >>
                                            (64 - 48));
                break;
            case OOR_ARCH_ARM:
            case OOR_ARCH_AARCH64:
                *pMmuFaultAddress = (NvU64)(((NvS64)*pMmuFaultAddress << (64 - 49)) >>
                                            (64 - 49));
                break;
            case OOR_ARCH_PPC64LE:
                break;
            case OOR_ARCH_NONE:
                NV_ASSERT_FAILED("Invalid oor address mode type.");
                break;
        }
    }
}

/*!
 * @brief Parses Faultbuffer entry and returns Fault Type
 *
 * @param[in]  fault          Fault Value
 * @param[out] pMmuFaultType  Fault Type
 *
 * @returns
 */
NV_STATUS
kgmmuGetFaultType_GV100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32       fault,
    FAULT_TYPE *pMmuFaultType
)
{
    NV_ASSERT_OR_RETURN(pMmuFaultType != NULL, NV_ERR_INVALID_POINTER);
    switch (fault)
    {
        case NV_PFAULT_FAULT_TYPE_PDE:
            *pMmuFaultType = fault_invalidPde;
            break;

        case NV_PFAULT_FAULT_TYPE_PDE_SIZE:
            *pMmuFaultType = fault_invalidPdeSize;
            break;

        case NV_PFAULT_FAULT_TYPE_PTE:
            *pMmuFaultType = fault_invalidPte;
            break;

        case NV_PFAULT_FAULT_TYPE_VA_LIMIT_VIOLATION:
            *pMmuFaultType = fault_limitViolation;
            break;

        case NV_PFAULT_FAULT_TYPE_UNBOUND_INST_BLOCK:
            *pMmuFaultType = fault_unboundInstBlock;
            break;

        case NV_PFAULT_FAULT_TYPE_PRIV_VIOLATION:
            *pMmuFaultType = fault_privViolation;
            break;

        case NV_PFAULT_FAULT_TYPE_RO_VIOLATION:
            *pMmuFaultType = fault_write;
            break;

        case NV_PFAULT_FAULT_TYPE_WO_VIOLATION:
            *pMmuFaultType = fault_read;
            break;

        case NV_PFAULT_FAULT_TYPE_PITCH_MASK_VIOLATION:
            *pMmuFaultType = fault_pitchMaskViolation;
            break;

        case NV_PFAULT_FAULT_TYPE_WORK_CREATION:
            *pMmuFaultType = fault_workCreation;
            break;

        case NV_PFAULT_FAULT_TYPE_UNSUPPORTED_APERTURE:
            *pMmuFaultType = fault_unsupportedAperture;
            break;

        case NV_PFAULT_FAULT_TYPE_COMPRESSION_FAILURE:
            *pMmuFaultType = fault_compressionFailure;
            break;

        case NV_PFAULT_FAULT_TYPE_UNSUPPORTED_KIND:
            *pMmuFaultType = fault_unsupportedKind;
            break;

        case NV_PFAULT_FAULT_TYPE_REGION_VIOLATION:
            *pMmuFaultType = fault_regionViolation;
            break;

        case NV_PFAULT_FAULT_TYPE_POISONED:
            *pMmuFaultType = fault_poison;
            break;

        case NV_PFAULT_FAULT_TYPE_ATOMIC_VIOLATION:
            *pMmuFaultType = fault_atomic;
            break;

        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

/*!
 * @brief Check if the given engineID is PHYSICAL
 *
 * @param[in] pKernelGmmu  KernelGmmu object
 * @param[in] engineID     Engine ID
 *
 * @return True if PHYSICAL
 */
NvBool
kgmmuIsFaultEnginePhysical_GV100
(
    KernelGmmu *pKernelGmmu,
    NvU32       engineID
)
{
    return (engineID == NV_PFAULT_MMU_ENG_ID_PHYSICAL);
}

/*!
 * @brief Parse fault Buffer Packet
 *
 * @param[in]  pGpu                  OBJGPU pointer
 * @param[in]  pKernelGmmu           KernelGmmu pointer
 * @param[in]  pFaultPacket          Raw Fault Packet data
 * @param[out] pParsedFaultEntry     Parsed Fault entry
 *
 * @returns
 */
NV_STATUS
kgmmuParseFaultPacket_GV100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvP64       pFaultPacket,
    NvP64       pParsedFaultEntry
)
{
    NV_STATUS rmStatus = NV_OK;
    NvU32 *faultEntry = KERNEL_POINTER_FROM_NvP64(NvU32 *, pFaultPacket);
    MMU_FAULT_BUFFER_ENTRY * pParsedEntry = KERNEL_POINTER_FROM_NvP64(MMU_FAULT_BUFFER_ENTRY *, pParsedFaultEntry);
    NvU64 addrHi, addrLo;
    NvU64 timestampLo, timestampHi;
    NvU32 tmp;

    addrLo = DRF_VAL_MW(C369, _BUF_ENTRY, _INST_LO, faultEntry);
    addrHi = DRF_VAL_MW(C369, _BUF_ENTRY, _INST_HI, faultEntry);
    pParsedEntry->mmuFaultInstBlock.address = addrLo + (addrHi << DRF_SIZE_MW(NVC369_BUF_ENTRY_INST_LO));
    pParsedEntry->mmuFaultInstBlock.address <<= 12;

    tmp = DRF_VAL_MW(C369, _BUF_ENTRY, _INST_APERTURE, faultEntry);
    pParsedEntry->mmuFaultInstBlock.aperture = tmp;
    pParsedEntry->mmuFaultInstBlock.gfid = GPU_GFID_PF;

    VERIFY_INST_BLOCK_APERTURE(NVC369_BUF_ENTRY_INST_APERTURE_VID_MEM,
                               NVC369_BUF_ENTRY_INST_APERTURE_SYS_MEM_COHERENT,
                               NVC369_BUF_ENTRY_INST_APERTURE_SYS_MEM_NONCOHERENT);

    addrLo = DRF_VAL_MW(C369, _BUF_ENTRY, _ADDR_LO, faultEntry);
    addrHi = DRF_VAL_MW(C369, _BUF_ENTRY, _ADDR_HI, faultEntry);
    pParsedEntry->mmuFaultAddress = (addrLo + (addrHi << DRF_SIZE_MW(NVC369_BUF_ENTRY_ADDR_LO))) <<
                                    (DRF_BASE_MW(NVC369_BUF_ENTRY_ADDR_LO));

    kgmmuSignExtendFaultAddress_HAL(pGpu, pKernelGmmu, &pParsedEntry->mmuFaultAddress);

    timestampLo = DRF_VAL_MW(C369, _BUF_ENTRY, _TIMESTAMP_LO, faultEntry);
    timestampHi = DRF_VAL_MW(C369, _BUF_ENTRY, _TIMESTAMP_HI, faultEntry);
    pParsedEntry->mmuFaultTimestamp = timestampLo + (timestampHi << DRF_SIZE_MW(NVC369_BUF_ENTRY_TIMESTAMP_LO));

    tmp = DRF_VAL_MW(C369, _BUF_ENTRY, _FAULT_TYPE, faultEntry);
    rmStatus = kgmmuGetFaultType_HAL(pGpu, pKernelGmmu, tmp, &pParsedEntry->mmuFaultType);
    NV_ASSERT_OR_RETURN(rmStatus == NV_OK, rmStatus);

    pParsedEntry->mmuFaultAccessType = DRF_VAL_MW(C369, _BUF_ENTRY, _ACCESS_TYPE, faultEntry);

    pParsedEntry->mmuFaultEngineId = DRF_VAL_MW(C369, _BUF_ENTRY, _ENGINE_ID, faultEntry);
    pParsedEntry->mmuFaultClientId = DRF_VAL_MW(C369, _BUF_ENTRY, _CLIENT, faultEntry);
    pParsedEntry->mmuFaultClientType = DRF_VAL_MW(C369, _BUF_ENTRY, _MMU_CLIENT_TYPE, faultEntry);
    pParsedEntry->mmuFaultGpcId = DRF_VAL_MW(C369, _BUF_ENTRY, _GPC_ID, faultEntry);
    pParsedEntry->bFaultEntryValid = DRF_VAL_MW(C369, _BUF_ENTRY, _VALID, faultEntry);
    pParsedEntry->bFaultInProtectedMode = DRF_VAL_MW(C369, _BUF_ENTRY, _PROTECTED_MODE, faultEntry);
    pParsedEntry->bFaultTypeReplayable = DRF_VAL_MW(C369, _BUF_ENTRY, _REPLAYABLE_FAULT, faultEntry);
    pParsedEntry->bReplayableFaultEn = DRF_VAL_MW(C369, _BUF_ENTRY, _REPLAYABLE_FAULT_EN, faultEntry);
    return rmStatus;
}

/*!
 * @brief Handles the nonreplayable fault based on a fault packet
 *
 * @param[in]  pGpu                  OBJGPU pointer
 * @param[in]  pKernelGmmu           KernelGmmu pointer
 * @param[in]  pFaultPacket          Raw Fault Packet data
 *
 * @returns
 *      NV_ERR_INVALID_DATA if fault packet data is not valid.
 */
NV_STATUS
kgmmuHandleNonReplayableFaultPacket_GV100
(
    OBJGPU            *pGpu,
    KernelGmmu        *pKernelGmmu,
    GMMU_FAULT_PACKET *pFaultPacket
)
{
    NV_STATUS rmStatus = NV_OK;
    MMU_FAULT_BUFFER_ENTRY parsedFaultEntry;
    FIFO_MMU_EXCEPTION_DATA MmuExceptionData = {0};

    // Parse the fault packet. RM fault buffer will have only RM serviceable packets
    rmStatus = kgmmuParseFaultPacket_HAL(pGpu, pKernelGmmu, NV_PTR_TO_NvP64(pFaultPacket),
                                         NV_PTR_TO_NvP64(&parsedFaultEntry));
    if (rmStatus != NV_OK)
        return NV_ERR_INVALID_DATA;

    // Let's create the exception data just for printing purposes
    MmuExceptionData.addrLo = (NvU32)(parsedFaultEntry.mmuFaultAddress & 0xFFFFFFFFUL);
    MmuExceptionData.addrHi = (NvU32)(parsedFaultEntry.mmuFaultAddress >> 32);
    MmuExceptionData.faultType = parsedFaultEntry.mmuFaultType;
    MmuExceptionData.clientId = parsedFaultEntry.mmuFaultClientId;
    if (parsedFaultEntry.mmuFaultClientType == NV_PFAULT_MMU_CLIENT_TYPE_GPC)
    {
        MmuExceptionData.bGpc  = NV_TRUE;
        MmuExceptionData.gpcId = parsedFaultEntry.mmuFaultGpcId;
    }
    else
    {
        MmuExceptionData.bGpc  = NV_FALSE;
        MmuExceptionData.gpcId = 0;
    }
    MmuExceptionData.accessType = parsedFaultEntry.mmuFaultAccessType;
    MmuExceptionData.faultEngineId = parsedFaultEntry.mmuFaultEngineId;

    kgmmuPrintFaultInfo_HAL(pGpu, pKernelGmmu, parsedFaultEntry.mmuFaultEngineId, &MmuExceptionData);

    rmStatus = kgmmuServiceMmuFault_HAL(pGpu, pKernelGmmu, NV_PTR_TO_NvP64(&parsedFaultEntry),
                                        &MmuExceptionData);
    NV_ASSERT(rmStatus == NV_OK);

    // Send event notifier in response to MMU Poison Fault.
    if ((parsedFaultEntry.mmuFaultType == fault_poison) &&
        gpuIsGlobalPoisonFuseEnabled(pGpu))
    {
        NV_ERROR_CONT_LOCATION loc = {0};

        // Error containment location information
        loc.locType = NV_ERROR_CONT_LOCATION_TYPE_NONE;

        // Generate Error Containment Xid, send NV2080_NOTIFIER*, mark device for Reset or Drain And Reset
        NV_ASSERT_OK_OR_RETURN(
            gpuUpdateErrorContainmentState_HAL(pGpu,
                                               NV_ERROR_CONT_ERR_ID_E13_MMU_POISON, // Error type
                                               loc,                                 // Loc
                                               NULL));                              // RC Error Code
    }

    return rmStatus;
}

NV_STATUS
kgmmuNotifyNonReplayableFault_GV100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvBool      bNotifyClient
)
{
    NV_STATUS rmStatus = NV_OK;
    PEVENTNOTIFICATION *ppEventNotification = NULL;
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowBuffer = NULL;

    //
    // We call clients unconditionally as clients should be walking thru their shadow buffer and filter all
    // redundant faults if any
    //
    if (CliGetEventNotificationList(pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hFaultBufferClient,
            pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hFaultBufferObject, NULL, &ppEventNotification) == NV_OK &&
        ppEventNotification != NULL)
    {
        //
        // Check if client has registered for shadow buffer. If not, we will call client only if RM has
        // serviced a fault packet. This reduces the duplicate notifications and enhances test speeds.
        //
        pClientShadowBuffer =
            pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pClientShadowFaultBuffer[NON_REPLAYABLE_FAULT_BUFFER];

        if (pClientShadowBuffer != NULL || bNotifyClient)
        {
            rmStatus = notifyEvents(pGpu, *ppEventNotification, NVC369_NOTIFIER_MMU_FAULT_NON_REPLAYABLE,
                                    0, 0, NV_OK, NV_OS_WRITE_THEN_AWAKEN);
            NV_ASSERT(rmStatus == NV_OK);
        }
    }

    return rmStatus;
}

NV_STATUS
kgmmuServiceNonReplayableFault_GV100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    NV_STATUS rmStatus = NV_OK;
    GMMU_SHADOW_FAULT_BUF *pShadowFaultBuf;
    GMMU_FAULT_PACKET faultPacket;
    NvBool bNotifyClient = NV_FALSE;

    pShadowFaultBuf = KERNEL_POINTER_FROM_NvP64(GMMU_SHADOW_FAULT_BUF *, pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pRmShadowFaultBuffer);
    NV_ASSERT_OR_RETURN(pShadowFaultBuf != NULL, NV_ERR_INVALID_POINTER);

    //
    // We don't disable Non-Replayable interrupts as we have copied all data from HW buffers and interrupts
    // should be enabled if new fault is generated
    // Walk thru the fault packets in shadow fault buffer and see if RM need to service anything
    //
    while (!queueIsEmpty(pShadowFaultBuf))
    {

        portMemSet((void *)&faultPacket, 0, sizeof(GMMU_FAULT_PACKET));
        queuePopAndCopy(pShadowFaultBuf, (void *)&faultPacket);

        rmStatus = kgmmuHandleNonReplayableFaultPacket_HAL(pGpu, pKernelGmmu, &faultPacket);
        if (rmStatus != NV_OK)
            return rmStatus;

        bNotifyClient = NV_TRUE;
    }

    rmStatus = kgmmuNotifyNonReplayableFault_HAL(pGpu, pKernelGmmu, bNotifyClient);
    return rmStatus;
}

/**
 * @brief handles an engine or PBDMA MMU fault
 *
 * "engine" is defined as an engine that is downstream of host (graphics, ce,
 * etc...).
 *
 * @param[in] pGpu                   OBJGPU pointer
 * @param[in] pKernelGmmu            KernelGmmu pointer
 * @param[in] pParsedFaultEntry      Parsed Fault entry
 * @param[in] pMmuExceptionData      FIFO exception packet used
 *                                   for printing fault info.
 *
 * @returns
 */
NV_STATUS
kgmmuServiceMmuFault_GV100
(
    OBJGPU                 *pGpu,
    KernelGmmu             *pKernelGmmu,
    NvP64                   pParsedFaultInfo,
    FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData
)
{
    NV_STATUS rmStatus = NV_OK;
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    KernelChannel *pKernelChannel = NULL;
    MMU_FAULT_BUFFER_ENTRY *pParsedFaultEntry = KERNEL_POINTER_FROM_NvP64(MMU_FAULT_BUFFER_ENTRY *, pParsedFaultInfo);

    // Find channel ID from instance pointer
    rmStatus = kfifoConvertInstToKernelChannel_HAL(pGpu, pKernelFifo, &pParsedFaultEntry->mmuFaultInstBlock, &pKernelChannel);

    if (rmStatus != NV_OK || pKernelChannel == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not get chid from inst addr\n");
        DBG_BREAKPOINT();
        return rmStatus;
    }

    // Reset channel
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        NvBool bIsMmuDebugModeEnabled = NV_FALSE;
        RM_ENGINE_TYPE rmEngineType = kchannelGetEngineType(pKernelChannel);

        if ((rmStatus == NV_OK) && RM_ENGINE_TYPE_IS_GR(rmEngineType))
        {
            NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS params;

            portMemSet(&params, 0, sizeof(params));
            NV_RM_RPC_CONTROL(pGpu,
                              RES_GET_CLIENT_HANDLE(pKernelChannel),
                              RES_GET_HANDLE(pKernelChannel),
                              NV0090_CTRL_CMD_GET_MMU_DEBUG_MODE,
                              &params,
                              sizeof(params),
                              rmStatus);

            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "RM control call to read MMU debug mode failed, rmStatus 0x%x\n",
                          rmStatus);
                DBG_BREAKPOINT();
            }
            else
            {
                bIsMmuDebugModeEnabled = params.bMode;
            }
        }

        NV_PRINTF(LEVEL_INFO, "bIsMmuDebugModeEnabled: %s\n",
                  bIsMmuDebugModeEnabled ? "TRUE" : "FALSE");

        if (!bIsMmuDebugModeEnabled)
        {
            RmCtrlParams rmCtrlParams = {0};
            NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS resetChannelParams = {0};
            RsClient *pClient = RES_GET_CLIENT(pKernelChannel);
            Device *pDevice = GPU_RES_GET_DEVICE(pKernelChannel);
            NvU32 subdeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
            Subdevice *pSubDevice;
            RM_ENGINE_TYPE rmEngineType;

            rmStatus = subdeviceGetByInstance(pClient, RES_GET_HANDLE(pDevice),
                                              subdeviceInstance, &pSubDevice);
            if (rmStatus != NV_OK)
                return rmStatus;

            GPU_RES_SET_THREAD_BC_STATE(pSubDevice);

            rmCtrlParams.hClient    = RES_GET_CLIENT_HANDLE(pKernelChannel);
            rmCtrlParams.hObject    = RES_GET_HANDLE(pKernelChannel);
            rmCtrlParams.cmd        = NV906F_CTRL_CMD_RESET_CHANNEL;
            rmCtrlParams.pParams    = &resetChannelParams;
            rmCtrlParams.paramsSize = sizeof(NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS);

            if (kfifoIsMmuFaultEngineIdPbdma(pGpu, pKernelFifo, pParsedFaultEntry->mmuFaultEngineId))
            {
                rmEngineType = RM_ENGINE_TYPE_HOST;
            }
            else
            {
                rmStatus = kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                                    ENGINE_INFO_TYPE_MMU_FAULT_ID, pParsedFaultEntry->mmuFaultEngineId,
                                                    ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32 *)&rmEngineType);
                NV_ASSERT(rmStatus == NV_OK);
            }

            resetChannelParams.engineID          = gpuGetNv2080EngineType(rmEngineType);
            resetChannelParams.subdeviceInstance = pSubDevice->subDeviceInst;
            resetChannelParams.resetReason       = NV906F_CTRL_CMD_RESET_CHANNEL_REASON_MMU_FLT;

            // Update the per-channel error notifier before performing the RC
            rmStatus = krcErrorSetNotifier(pGpu, GPU_GET_KERNEL_RC(pGpu),
                pKernelChannel,
                ROBUST_CHANNEL_FIFO_ERROR_MMU_ERR_FLT,
                rmEngineType,
                RC_NOTIFIER_SCOPE_TSG);
            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to set error notifier, rmStatus 0x%x\n",
                          rmStatus);
                DBG_BREAKPOINT();
            }

            //
            // Reset rmStatus before calling reset channel RPC as we should return
            // status of this RPC which actually performs channel reset.
            //
            rmStatus = NV_OK;

            NV_RM_RPC_CONTROL(pGpu,
                              rmCtrlParams.hClient,
                              rmCtrlParams.hObject,
                              rmCtrlParams.cmd,
                              rmCtrlParams.pParams,
                              rmCtrlParams.paramsSize,
                              rmStatus
                              );
            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "RM control call to reset channel failed, rmStatus 0x%x\n",
                          rmStatus);
                DBG_BREAKPOINT();
            }
        }

        //
        // Fill exception info in FifoData.
        // Also mark this exception as notified to prevent duplication notification
        // in vgpuService when PF has done its RC.
        //
        kchannelFillMmuExceptionInfo(pKernelChannel, pMmuExceptionData);

        if (RM_ENGINE_TYPE_IS_GR(rmEngineType) && pMmuExceptionData->bGpc)
        {
            KernelGraphicsContext *pKernelGraphicsContext;

            NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
                                                kgrctxFromKernelChannel(pKernelChannel,
                                                &pKernelGraphicsContext));
            if (rmStatus == NV_OK)
            {
                kgrctxRecordMmuFault(pGpu, pKernelGraphicsContext,
                                     kgmmuGetFaultInfoFromFaultPckt_HAL(pKernelGmmu, pParsedFaultEntry),
                                     pParsedFaultEntry->mmuFaultAddress,
                                     pParsedFaultEntry->mmuFaultType,
                                     pParsedFaultEntry->mmuFaultAccessType);
            }
        }

        return rmStatus;
    }

    rmStatus = kgmmuServiceChannelMmuFault_HAL(pGpu, pKernelGmmu, pParsedFaultEntry,
                                               pMmuExceptionData, pKernelChannel);
    if (NV_OK != rmStatus)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not service MMU fault for chid %x\n",
                  kchannelGetDebugTag(pKernelChannel));
    }

    return rmStatus;
}

NvU32
kgmmuGetFaultInfoFromFaultPckt_GV100
(
    KernelGmmu             *pKernelGmmu,
    MMU_FAULT_BUFFER_ENTRY *pParsedFaultEntry
)
{
    NvU32 faultInfo = 0;

    if (pParsedFaultEntry == NULL)
        return 0;

    //
    // This is a bit insane. We don't have any protection against changing bit position.
    // Still copying the bits and relying on fact that MMU_FAULT_INFO will always keep info consistent
    //
    faultInfo = FLD_SET_DRF_NUM(_PFB_PRI, _MMU_FAULT_INFO, _FAULT_TYPE, pParsedFaultEntry->mmuFaultType, faultInfo);
    faultInfo = FLD_SET_DRF_NUM(_PFB_PRI, _MMU_FAULT_INFO, _CLIENT, pParsedFaultEntry->mmuFaultClientId, faultInfo);
    faultInfo = FLD_SET_DRF_NUM(_PFB_PRI, _MMU_FAULT_INFO, _ACCESS_TYPE, pParsedFaultEntry->mmuFaultAccessType, faultInfo);
    faultInfo = FLD_SET_DRF_NUM(_PFB_PRI, _MMU_FAULT_INFO, _CLIENT_TYPE, pParsedFaultEntry->mmuFaultClientType, faultInfo);
    faultInfo = FLD_SET_DRF_NUM(_PFB_PRI, _MMU_FAULT_INFO, _GPC_ID, pParsedFaultEntry->mmuFaultGpcId, faultInfo);
    faultInfo = FLD_SET_DRF_NUM(_PFB_PRI, _MMU_FAULT_INFO, _PROTECTED_MODE, pParsedFaultEntry->bFaultInProtectedMode, faultInfo);
    faultInfo = FLD_SET_DRF_NUM(_PFB_PRI, _MMU_FAULT_INFO, _REPLAYABLE_FAULT, pParsedFaultEntry->bFaultTypeReplayable, faultInfo);
    faultInfo = FLD_SET_DRF_NUM(_PFB_PRI, _MMU_FAULT_INFO, _VALID, pParsedFaultEntry->bFaultEntryValid, faultInfo);

    return faultInfo;
}

static NV_STATUS
_kgmmuCreateExceptionDataFromPriv_GV100
(
    OBJGPU                  *pGpu,
    KernelGmmu              *pKernelGmmu,
    MMU_FAULT_BUFFER_ENTRY  *pParsedFaultEntry,
    FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData
)
{
    NV_STATUS rmStatus = NV_OK;
    NvU32 regDataLo, regDataHi;
    NvU64 tempLo = 0;
    NvU64 tempHi = 0;
    NV_ASSERT_OR_RETURN(pParsedFaultEntry != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pMmuExceptionData != NULL, NV_ERR_INVALID_ARGUMENT);

    // FaultInstPtr
    {
        kgmmuReadMmuFaultInstHiLo_HAL(pGpu, pKernelGmmu, &regDataHi, &regDataLo);

        tempLo = DRF_VAL(_PFB_PRI, _MMU_FAULT_INST_LO, _ADDR, regDataLo);
        tempHi = DRF_VAL(_PFB_PRI, _MMU_FAULT_INST_HI, _ADDR, regDataHi);
        pParsedFaultEntry->mmuFaultInstBlock.address = tempLo + (tempHi << DRF_SIZE(NV_PFB_PRI_MMU_FAULT_INST_LO_ADDR));
        pParsedFaultEntry->mmuFaultInstBlock.address <<= DRF_BASE(NV_PFB_PRI_MMU_FAULT_INST_LO_ADDR);
        pParsedFaultEntry->mmuFaultInstBlock.aperture = DRF_VAL(_PFB_PRI, _MMU_FAULT_INST_LO, _APERTURE, regDataLo);
        pParsedFaultEntry->mmuFaultInstBlock.gfid = GPU_GFID_PF;
        pParsedFaultEntry->mmuFaultEngineId = DRF_VAL(_PFB_PRI, _MMU_FAULT_INST_LO, _ENGINE_ID, regDataLo);
    }

    // Fault Addr
    {
        kgmmuReadMmuFaultAddrHiLo_HAL(pGpu, pKernelGmmu, &regDataHi, &regDataLo);

        tempLo = DRF_VAL(_PFB_PRI, _MMU_FAULT_ADDR_LO, _ADDR, regDataLo);
        tempHi = DRF_VAL(_PFB_PRI, _MMU_FAULT_ADDR_HI, _ADDR, regDataHi);

        pParsedFaultEntry->mmuFaultAddress =  (tempLo + (tempHi << DRF_SIZE(NV_PFB_PRI_MMU_FAULT_ADDR_LO_ADDR))) <<
                                              (DRF_BASE(NV_PFB_PRI_MMU_FAULT_ADDR_LO_ADDR));

        kgmmuSignExtendFaultAddress_HAL(pGpu, pKernelGmmu, &pParsedFaultEntry->mmuFaultAddress);
    }

    // FaultInfo
    {
        regDataLo = kgmmuReadMmuFaultInfo_HAL(pGpu, pKernelGmmu);

        rmStatus = kgmmuGetFaultType_HAL(pGpu, pKernelGmmu, DRF_VAL(_PFB_PRI, _MMU_FAULT_INFO, _FAULT_TYPE, regDataLo),
                                         &pParsedFaultEntry->mmuFaultType);
        NV_ASSERT_OR_RETURN(rmStatus == NV_OK, rmStatus);
        pParsedFaultEntry->mmuFaultAccessType = DRF_VAL(_PFB_PRI, _MMU_FAULT_INFO, _ACCESS_TYPE, regDataLo);
        pParsedFaultEntry->mmuFaultClientId = DRF_VAL(_PFB_PRI, _MMU_FAULT_INFO, _CLIENT, regDataLo);
        pParsedFaultEntry->mmuFaultClientType = DRF_VAL(_PFB_PRI, _MMU_FAULT_INFO, _CLIENT_TYPE, regDataLo);
        pParsedFaultEntry->mmuFaultGpcId = DRF_VAL(_PFB_PRI, _MMU_FAULT_INFO, _GPC_ID, regDataLo);
        pParsedFaultEntry->bFaultEntryValid = DRF_VAL(_PFB_PRI, _MMU_FAULT_INFO, _VALID, regDataLo);
        pParsedFaultEntry->bFaultInProtectedMode = DRF_VAL(_PFB_PRI, _MMU_FAULT_INFO, _PROTECTED_MODE, regDataLo);
        pParsedFaultEntry->bFaultTypeReplayable = DRF_VAL(_PFB_PRI, _MMU_FAULT_INFO, _REPLAYABLE_FAULT, regDataLo);
        pParsedFaultEntry->bReplayableFaultEn = DRF_VAL(_PFB_PRI, _MMU_FAULT_INFO, _REPLAYABLE_FAULT_EN, regDataLo);
    }

    pMmuExceptionData->addrLo = (NvU32)(pParsedFaultEntry->mmuFaultAddress & 0xFFFFFFFFUL);
    pMmuExceptionData->addrHi = (NvU32)(pParsedFaultEntry->mmuFaultAddress >> 32);
    pMmuExceptionData->faultType = pParsedFaultEntry->mmuFaultType;
    pMmuExceptionData->clientId = pParsedFaultEntry->mmuFaultClientId;
    if (pParsedFaultEntry->mmuFaultClientType == NV_PFAULT_MMU_CLIENT_TYPE_GPC)
    {
        pMmuExceptionData->bGpc  = NV_TRUE;
        pMmuExceptionData->gpcId = pParsedFaultEntry->mmuFaultGpcId;
    }
    else
    {
        pMmuExceptionData->bGpc  = NV_FALSE;
        pMmuExceptionData->gpcId = 0;
    }
    pMmuExceptionData->accessType = pParsedFaultEntry->mmuFaultAccessType;
    pMmuExceptionData->faultEngineId = pParsedFaultEntry->mmuFaultEngineId;

    return rmStatus;
}

/**
 * @brief Resets REPLAYABLE/NON_REPLAYABLE FAULT error status
 *
 * @param[in]  faultBufType          FaultBuffer type
 *
 * @returns - Value resetting corresponding status fields
 */

static inline NvU32
_kgmmuResetFaultBufferError_GV100
(
    NvU32    faultBufType
)
{
    NvU32 faultBufStatus = 0;
    NV_ASSERT_OR_RETURN((faultBufType < NUM_FAULT_BUFFERS), NV_ERR_INVALID_ARGUMENT);

    if (faultBufType == REPLAYABLE_FAULT_BUFFER)
        return FLD_SET_DRF(_PFB_PRI, _MMU_FAULT_STATUS, _REPLAYABLE_ERROR, _RESET, faultBufStatus);
    else
        return FLD_SET_DRF(_PFB_PRI, _MMU_FAULT_STATUS, _NON_REPLAYABLE_ERROR, _RESET, faultBufStatus);
}

/**
 * @brief Handles all BAR2 faults including fault buffer and
 *        access counter BAR2 errors.
 *
 */
static NV_STATUS
_kgmmuServiceBar2Faults_GV100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32       faultStatus,
    NvU32       mmuFaultClientId
)
{
    NV_STATUS   rmStatus    = NV_OK;
    OBJUVM     *pUvm        = GPU_GET_UVM(pGpu);
    NvU32       i;

    NvBool replayableFaultError = FLD_TEST_DRF(_PFB_PRI, _MMU_FAULT_STATUS,
                                                    _REPLAYABLE_ERROR, _SET, faultStatus);
    NvBool nonReplayableFaultError = FLD_TEST_DRF(_PFB_PRI, _MMU_FAULT_STATUS,
                                                      _NON_REPLAYABLE_ERROR, _SET, faultStatus);
    NvBool bServiceActrs = kgmmuCheckAccessCounterBar2FaultServicingState_HAL(pGpu, pKernelGmmu);

    NvBool bVidmemAccessBitBufError = kgmmuTestVidmemAccessBitBufferError_HAL(pGpu, pKernelGmmu, faultStatus);

    //
    // If it's a MMU Fault buffer BAR2 error, disable and re-enable fault buffers after BAR2 rebind.
    // In case of BAR2 fault on both fault buffers, RM should see two error interrupts.
    // So, just handle one buffer here and next buffer can be reset with next incoming interrupt
    //
    NvU32 faultBufType = replayableFaultError ? REPLAYABLE_FAULT_BUFFER :
                                                NON_REPLAYABLE_FAULT_BUFFER;
    if (replayableFaultError || nonReplayableFaultError)
    {
        rmStatus = kgmmuDisableFaultBuffer_HAL(pGpu, pKernelGmmu, faultBufType, NV_TRUE, GPU_GFID_PF);
        if (rmStatus != NV_OK)
            return rmStatus;
    }

    // Access counter Bar2 fault handling
    if (bServiceActrs)
    {
        for (i = 0; i < pUvm->accessCounterBufferCount; i++)
        {
            rmStatus = uvmDisableAccessCntr_HAL(pGpu, pUvm, i, NV_TRUE);
            if (rmStatus != NV_OK)
                return rmStatus;
        }
    }

    // vidmem access bit buffer BAR2 fault handling
    if (bVidmemAccessBitBufError)
    {
        kgmmuDisableVidmemAccessBitBuf_HAL(pGpu, pKernelGmmu);
    }

    // do a rebind of bar2
    kbusBindBar2_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), BAR2_MODE_VIRTUAL);

    kgmmuServiceMthdBuffFaultInBar2Fault(pGpu, pKernelGmmu);

    if (replayableFaultError || nonReplayableFaultError)
    {
        rmStatus = kgmmuEnableFaultBuffer_HAL(pGpu, pKernelGmmu, faultBufType, NV_TRUE, GPU_GFID_PF);
        if (rmStatus != NV_OK)
            return rmStatus;

        faultStatus |= _kgmmuResetFaultBufferError_GV100(faultBufType);
    }

    if (bServiceActrs)
    {
        for (i = 0; i < pUvm->accessCounterBufferCount; i++)
        {
            rmStatus = uvmEnableAccessCntr_HAL(pGpu, pUvm, i, NV_TRUE);
            if (rmStatus != NV_OK)
                return rmStatus;
        }

        kgmmuClearAccessCounterWriteNak_HAL(pGpu, pKernelGmmu);
    }

    if (bVidmemAccessBitBufError)
    {
        rmStatus = kgmmuEnableVidmemAccessBitBuf_HAL(pGpu, pKernelGmmu);
        if (rmStatus != NV_OK)
            return rmStatus;
    }

    return rmStatus;
}

static NV_STATUS
_kgmmuHandleReplayablePrivFault_GV100
(
    OBJGPU                 *pGpu,
    KernelGmmu             *pKernelGmmu,
    MMU_FAULT_BUFFER_ENTRY *pParsedFaultEntry
)
{
    GMMU_FAULT_CANCEL_INFO cancelInfo;
    NvU32 faultStatus;

    // Fill in the structure used by cancel routine to cancel fault
    cancelInfo.clientId = pParsedFaultEntry->mmuFaultClientId;
    cancelInfo.gpcId = pParsedFaultEntry->mmuFaultGpcId;
    cancelInfo.instBlock = pParsedFaultEntry->mmuFaultInstBlock;

    //
    // Clear the VALID bit to ensure non_replayable fault shows up when replayable
    // fault is canceled
    //
    faultStatus = kgmmuReadMmuFaultStatus_HAL(pGpu, pKernelGmmu, GPU_GFID_PF);
    faultStatus = FLD_SET_DRF(_PFB_PRI, _MMU_FAULT_STATUS, _VALID, _CLEAR, faultStatus);
    kgmmuWriteMmuFaultStatus_HAL(pGpu, pKernelGmmu, faultStatus);

    return kgmmuFaultCancelTargeted_HAL(pGpu, pKernelGmmu, &cancelInfo);
}

/**
 * @brief Handles PRI MMU faults
 *        PRI MMU faults are used for BAR1/BAR2, Physical faults and
 *        capturing faults in the event fault buffers are disabled.
 *
 * @param[in]  pGpu                  OBJGPU pointer
 * @param[in]  pKernelGmmu           KernelGmmu pointer
 *
 * @returns
 */
NV_STATUS
kgmmuServicePriFaults_GV100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    NV_STATUS rmStatus = NV_OK;
    KernelRc *pKernelRC = GPU_GET_KERNEL_RC(pGpu);
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU32 faultStatus = kgmmuReadMmuFaultStatus_HAL(pGpu, pKernelGmmu, GPU_GFID_PF);
    PEVENTNOTIFICATION *ppEventNotification  = NULL;
    MMU_FAULT_BUFFER_ENTRY parsedFaultEntry = {{0}, 0};
    FIFO_MMU_EXCEPTION_DATA mmuExceptionData = {0};
    NvBool bBarFault = NV_FALSE;
    NvU32 vfFaultType = NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_TYPE_INVALID;

    if (FLD_TEST_DRF(_PFB_PRI, _MMU_FAULT_STATUS, _VALID, _SET, faultStatus))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, _kgmmuCreateExceptionDataFromPriv_GV100(pGpu, pKernelGmmu, &parsedFaultEntry, &mmuExceptionData));

        //
        // BAR faults mean incorrect BAR mappings. These are usually fatal with
        // fullchip reset that follows this.
        //
        if (kgmmuIsFaultEngineBar1_HAL(pKernelGmmu, parsedFaultEntry.mmuFaultEngineId))
        {
            vfFaultType = NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_TYPE_BAR1;

            // do a rebind of bar1
            kbusBar1InstBlkBind_HAL(pGpu, pKernelBus);
            NV_PRINTF(LEVEL_ERROR, "BAR1 MMU Fault\n");
            kgmmuPrintFaultInfo_HAL(pGpu, pKernelGmmu, parsedFaultEntry.mmuFaultEngineId, &mmuExceptionData);
            krcBreakpoint(GPU_GET_KERNEL_RC(pGpu));
        }
        else if (kgmmuIsFaultEngineBar2_HAL(pKernelGmmu, parsedFaultEntry.mmuFaultEngineId) &&
                 !kgmmuIsP2PUnboundInstFault_HAL(pKernelGmmu, parsedFaultEntry.mmuFaultType, parsedFaultEntry.mmuFaultClientId))
        {
            vfFaultType = NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_TYPE_BAR2;

            NV_PRINTF(LEVEL_ERROR, "BAR2 MMU Fault\n");
            kgmmuPrintFaultInfo_HAL(pGpu, pKernelGmmu, parsedFaultEntry.mmuFaultEngineId, &mmuExceptionData);
            rmStatus = _kgmmuServiceBar2Faults_GV100(pGpu, pKernelGmmu, faultStatus, parsedFaultEntry.mmuFaultClientId);
            NV_ASSERT(rmStatus == NV_OK);
            krcBreakpoint(GPU_GET_KERNEL_RC(pGpu));

            bBarFault = NV_TRUE;
        }
        else if (kgmmuIsFaultEnginePhysical_HAL(pKernelGmmu, parsedFaultEntry.mmuFaultEngineId))
        {
            vfFaultType = NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_TYPE_PHYSICAL;
            //
            // This fault usually means VPR or out of bounds physical accesses.
            // Nothing much we can do except notify clients and wait for a TDR
            //
            NV_PRINTF(LEVEL_ERROR, "Physical MMU fault\n");
            kgmmuPrintFaultInfo_HAL(pGpu, pKernelGmmu, parsedFaultEntry.mmuFaultEngineId, &mmuExceptionData);
            krcBreakpoint(GPU_GET_KERNEL_RC(pGpu));

            gpuNotifySubDeviceEvent(pGpu,
               NV2080_NOTIFIERS_PHYSICAL_PAGE_FAULT, NULL, 0, 0, 0);
        }
        else if (mmuExceptionData.faultType == NV_PFAULT_FAULT_TYPE_UNBOUND_INST_BLOCK)
        {
            NV_PRINTF(LEVEL_ERROR, "Unbound Instance MMU fault\n");
            kgmmuPrintFaultInfo_HAL(pGpu, pKernelGmmu, parsedFaultEntry.mmuFaultEngineId, &mmuExceptionData);
            krcBreakpoint(GPU_GET_KERNEL_RC(pGpu));
            vfFaultType = NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_TYPE_UNBOUND_INSTANCE;
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kgmmuServiceUnboundInstBlockFault_HAL(pGpu, pKernelGmmu, NV_PTR_TO_NvP64(&parsedFaultEntry),
                                                                                     &mmuExceptionData));
        }
        else
        {
            //
            // Check if any fault buffer is disabled. If yes, then service the fault
            // snapped in priv register.
            // Non-Replayable Faults - These faults are serviced as fault buffer faults i.e channel
            //                         will be RCed.
            // Replayable Faults - These faults will be cancelled as RM doesn't support replaying such
            //                     faults. Cancelling these faults will bring them back as non-replayable faults.
            //
            NvBool bReplayableBufDis = FLD_TEST_DRF(_PFB_PRI, _MMU_FAULT_BUFFER_SIZE,_ENABLE, _FALSE,
                                                    kgmmuReadMmuFaultBufferSize_HAL(pGpu, pKernelGmmu, REPLAYABLE_FAULT_BUFFER, GPU_GFID_PF));
            NvBool bNonReplayBufDis = FLD_TEST_DRF(_PFB_PRI, _MMU_FAULT_BUFFER_SIZE,_ENABLE, _FALSE,
                                                    kgmmuReadMmuFaultBufferSize_HAL(pGpu, pKernelGmmu, NON_REPLAYABLE_FAULT_BUFFER, GPU_GFID_PF));
            if (bReplayableBufDis || bNonReplayBufDis)
            {
                if (parsedFaultEntry.bFaultEntryValid)
                {
                    // Replayable faults snapped in privs are handled differently, so check if replayable
                    if (parsedFaultEntry.bFaultTypeReplayable)
                    {
                        NV_PRINTF(LEVEL_ERROR,
                                  "MMU Fault : Replayable fault with fault-buffer disabled. Initiating cancel\n");
                        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, _kgmmuHandleReplayablePrivFault_GV100(pGpu, pKernelGmmu, &parsedFaultEntry));
                    }
                    else
                    {
                        kgmmuPrintFaultInfo_HAL(pGpu, pKernelGmmu, parsedFaultEntry.mmuFaultEngineId,
                                                &mmuExceptionData);
                        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kgmmuServiceMmuFault_HAL(pGpu, pKernelGmmu, NV_PTR_TO_NvP64(&parsedFaultEntry),
                                                                                    &mmuExceptionData));
                    }
                }
            }
            else
            {
                kgmmuPrintFaultInfo_HAL(pGpu, pKernelGmmu, parsedFaultEntry.mmuFaultEngineId, &mmuExceptionData);
                krcBreakpoint(GPU_GET_KERNEL_RC(pGpu));
            }
        }

        // In case of SR-IOV vgpu, the fault needs to be cleared from the host side.
        if (IS_VIRTUAL_WITH_SRIOV(pGpu) && (vfFaultType != NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_TYPE_INVALID))
            rmStatus = kgmmuServiceVfPriFaults(pGpu, pKernelGmmu, vfFaultType);

        // Clear the VALID bit to indicate we have seen this.
        faultStatus = FLD_SET_DRF(_PFB_PRI, _MMU_FAULT_STATUS, _VALID, _CLEAR, faultStatus);
        kgmmuWriteMmuFaultStatus_HAL(pGpu, pKernelGmmu, faultStatus);

        if (bBarFault && pKernelRC != NULL && pKernelRC->bRcOnBar2Fault)
        {
            pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_FATAL_ERROR, NV_TRUE);
            (void)kfifoRecoverAllChannels_HAL(pGpu, GPU_GET_KERNEL_FIFO(pGpu), GPU_GFID_PF);
        }

        if ((NV_OK == CliGetEventNotificationList(pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hFaultBufferClient,
                     pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hFaultBufferObject, NULL, &ppEventNotification)) && ppEventNotification != NULL)
        {
            rmStatus = notifyEvents(pGpu, *ppEventNotification, NVC369_NOTIFIER_MMU_FAULT_NON_REPLAYABLE_IN_PRIV,
                                    0, 0, NV_OK, NV_OS_WRITE_THEN_AWAKEN);
        }

        // Send event notifier in response to MMU Poison Fault.
        if ((parsedFaultEntry.mmuFaultType == fault_poison) &&
            gpuIsGlobalPoisonFuseEnabled(pGpu))
        {
            NV_ERROR_CONT_LOCATION loc = {0};

            // Error containment location information
            loc.locType = NV_ERROR_CONT_LOCATION_TYPE_NONE;

            // Generate Error Containment Xid, send NV2080_NOTIFIER*, mark device for Reset or Drain And Reset
            NV_ASSERT_OK_OR_RETURN(
                gpuUpdateErrorContainmentState_HAL(pGpu,
                                                   NV_ERROR_CONT_ERR_ID_E13_MMU_POISON, // Error type
                                                   loc,                                 // Loc
                                                   NULL));                              // RC Error Code
        }
    }
    return rmStatus;
}

/*!
 *  @brief Checks whether the access counter reset sequence
 *         should be performed on BAR2 fault servicing
 *
 * @returns NvBool indicating whether servicing should be performed.
 */
NvBool
kgmmuCheckAccessCounterBar2FaultServicingState_GV100
(
    OBJGPU *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    return kgmmuTestAccessCounterWriteNak_HAL(pGpu, pKernelGmmu);
}

