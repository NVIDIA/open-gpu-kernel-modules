/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/rc/kernel_rc.h"

#include "published/volta/gv100/dev_fb.h"
#include "published/volta/gv100/dev_ram.h"
#include "published/volta/gv100/dev_fault.h"

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
