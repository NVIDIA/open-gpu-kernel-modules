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

#include "gpu/gpu.h"
#include "gpu/gpu_child_class_defs.h"
#include "kernel/gpu/intr/intr.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "published/ampere/ga100/dev_fb.h"
#include "published/ampere/ga100/dev_vm.h"
#include "published/ampere/ga100/dev_fuse.h"
#include "published/ampere/ga100/dev_top.h"
#include "virtualization/hypervisor/hypervisor.h"

// Error containment error id string description.
const char *ppErrContErrorIdStr[] = NV_ERROR_CONT_ERR_ID_STRING_PUBLIC;

// Error containment table
NV_ERROR_CONT_STATE_TABLE g_errContStateTable[] = NV_ERROR_CONT_STATE_TABLE_SETTINGS;

/*!
 * @brief Get index of specified errorCode in the Error Containment state table
 *
 * @param[in] pGpu              OBJGPU pointer
 * @param[in] errorCode         Error Containment error code
 * @param[in] pTableIndex       Index of specified errorCode in the Error Containment State table
 *
 * @returns NV_STATUS
 */
static NV_STATUS _gpuGetErrorContStateTableIndex_GA100(OBJGPU               *pGpu,
                                                       NV_ERROR_CONT_ERR_ID  errorCode,
                                                       NvU32                *pTableIndex);

/*!
 * @brief Send NV2080_NOTIFIER*
 *
 * @param[in]  pGpu                   OBJGPU pointer
 * @param[in]  errorCode              Error Containment error code
 * @param[in]  loc                    Location, SubLocation information
 * @param[in]  nv2080Notifier         NV2080_NOTIFIER*
 *
 * @returns NV_STATUS
 */
static NV_STATUS _gpuNotifySubDeviceEventNotifier_GA100(OBJGPU                 *pGpu,
                                                        NV_ERROR_CONT_ERR_ID    errorCode,
                                                        NV_ERROR_CONT_LOCATION  loc,
                                                        NvU32                   nv2080Notifier);

/*!
 * @brief Generate error log for corresponding error containment error code.
 *
 * @param[in]  pGpu                   OBJGPU pointer
 * @param[in]  errorCode              Error Containment error code
 * @param[in]  loc                    Location, SubLocation information
 * @param[in]  pErrorContSmcSetting   Error containment SMC Disable / Enable settings
 *
 * @returns NV_STATUS
 */
static NV_STATUS _gpuGenerateErrorLog_GA100(OBJGPU                           *pGpu,
                                            NV_ERROR_CONT_ERR_ID              errorCode,
                                            NV_ERROR_CONT_LOCATION            loc,
                                            NV_ERROR_CONT_SMC_DIS_EN_SETTING *pErrorContSmcSetting);

/*!
 * @brief Read fuse for display supported status.
 *        Some chips not marked displayless do not support display
 */
NvBool
gpuFuseSupportsDisplay_GA100
(
    OBJGPU *pGpu
)
{
    return GPU_FLD_TEST_DRF_DEF(pGpu, _FUSE, _STATUS_OPT_DISPLAY, _DATA, _ENABLE);
}

/*!
 * @brief Clear FBHUB POISON Interrupt state for Bug 2924523.
 * This HAL handles the CPU interrupt tree
 *
 * @param[in]      pGpu           OBJGPU pointer
 *
 * @return NV_OK if success, else appropriate NV_STATUS code
 */
NV_STATUS
gpuClearFbhubPoisonIntrForBug2924523_GA100
(
    OBJGPU *pGpu
)
{
    // INTR module is not stateloaded at gpuPostConstruct, so use HW default
    NvU32 intrVector = NV_PFB_FBHUB_POISON_INTR_VECTOR_HW_INIT;

    if (pGpu == NULL)
        return NV_OK;

    //
    // Check if FBHUB Poison interrupt got triggered before RM Init due
    // to VBIOS IFR on GA100. If yes, clear the FBHUB Interrupt. This WAR is
    // required for Bug 2924523 as VBIOS IFR causes FBHUB Poison intr.
    //
    if (intrIsVectorPending_HAL(pGpu, GPU_GET_INTR(pGpu), intrVector, NULL))
    {
        NV_PRINTF(LEVEL_ERROR, "FBHUB Interrupt detected. Clearing it.\n");
        intrClearLeafVector_HAL(pGpu, GPU_GET_INTR(pGpu), intrVector, NULL);
    }

    return NV_OK;
}

/*!
 * @brief Returns FLA VASpace Size for Ampere
 *
 * @param[in] pGpu                        OBJGPU pointer
 * @param[in] bNvSwitchVirtualization     boolean
 *
 * @returns NvU64 -> size of FLA VASpace
 */
NvU64
gpuGetFlaVasSize_GA100
(
    OBJGPU *pGpu,
    NvBool  bNvswitchVirtualization
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU64  totalFbSize = (pMemoryManager->Ram.fbTotalMemSizeMb << 20);

    if (bNvswitchVirtualization || totalFbSize <= NVBIT64(36))
    {
        return 0x2000000000;  // 128GB
    }
    else
    {
        return (totalFbSize * 2);
    }
}

/*!
 * @brief Is ctx buffer allocation in PMA supported
 */
NvBool
gpuIsCtxBufAllocInPmaSupported_GA100
(
    OBJGPU *pGpu
)
{
    //
    // This is supported by default on baremetal RM.
    // This has no impact in guest-RM since ctxBufPools are disabled on guest.
    // We leave this disabled on host-RM. TODO: Bug 4066846
    //
    if (!hypervisorIsVgxHyper())
        return NV_TRUE;
    return NV_FALSE;
}

//
// List of GPU children that present for the chip. List entries contain$
// {CLASS-ID, # of instances} pairs, e.g.: {CE, 2} is 2 instance of OBJCE. This$
// list controls only engine presence. Order is defined by$
// gpuGetChildrenOrder_HAL.$
//
// IMPORTANT: This function is to be deleted. Engine removal should instead be$
// handled by <eng>ConstructEngine returning NV_ERR_NOT_SUPPORTED. PLEASE DO NOT$
// FORK THIS LIST!$
//
// List entries contain {CLASS-ID, # of instances} pairs.
//

static const GPUCHILDPRESENT gpuChildrenPresent_GA100[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJGPUMON, 1),
    GPU_CHILD_PRESENT(OBJSWENG, 1),
    GPU_CHILD_PRESENT(OBJUVM, 1),
    GPU_CHILD_PRESENT(KernelBif, 1),
    GPU_CHILD_PRESENT(KernelBus, 1),
    GPU_CHILD_PRESENT(KernelCE, 10),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 8),
    GPU_CHILD_PRESENT(KernelHwpm, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelNvlink, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
    GPU_CHILD_PRESENT(KernelSec2, 1),
    GPU_CHILD_PRESENT(KernelGsp, 1),
    GPU_CHILD_PRESENT(ConfidentialCompute, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GA100(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GA100);
    return gpuChildrenPresent_GA100;
}

//
// List of GPU children that present for the chip. List entries contain$
// {CLASS-ID, # of instances} pairs, e.g.: {CE, 2} is 2 instance of OBJCE. This$
// list controls only engine presence. Order is defined by$
// gpuGetChildrenOrder_HAL.$
//
// IMPORTANT: This function is to be deleted. Engine removal should instead be$
// handled by <eng>ConstructEngine returning NV_ERR_NOT_SUPPORTED. PLEASE DO NOT$
// FORK THIS LIST!$
//
// List entries contain {CLASS-ID, # of instances} pairs.
//
static const GPUCHILDPRESENT gpuChildrenPresent_GA102[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJGPUMON, 1),
    GPU_CHILD_PRESENT(OBJSWENG, 1),
    GPU_CHILD_PRESENT(OBJUVM, 1),
    GPU_CHILD_PRESENT(KernelBif, 1),
    GPU_CHILD_PRESENT(KernelBus, 1),
    GPU_CHILD_PRESENT(KernelCE, 5),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 1),
    GPU_CHILD_PRESENT(KernelHwpm, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelNvlink, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
    GPU_CHILD_PRESENT(KernelSec2, 1),
    GPU_CHILD_PRESENT(KernelGsp, 1),
    GPU_CHILD_PRESENT(ConfidentialCompute, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GA102(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GA102);
    return gpuChildrenPresent_GA102;
}

/*! @brief Returns if a P2P object is allocated in SRIOV mode.
 *
 *  @param[in]   pGpu     OBJGPU pointer
 *
 *  @returns for baremetal, this should just return NV_TRUE
             for SRIOV, return the SRIOV Info
 */
NvBool
gpuCheckIsP2PAllocated_GA100
(
    OBJGPU *pGpu
)
{
    if (!IS_VIRTUAL(pGpu) && !gpuIsSriovEnabled(pGpu))
        return NV_TRUE;

    return pGpu->sriovState.bP2PAllocated;
}

/*!
 * @brief Get index of specified errorCode in the Error Containment state table
 *
 * @param[in] pGpu              OBJGPU pointer
 * @param[in] errorCode         Error Containment error code
 * @param[in] pTableIndex       Index of specified errorCode in the Error Containment state table
 *
 * @returns NV_STATUS
 */
static
NV_STATUS
_gpuGetErrorContStateTableIndex_GA100
(
    OBJGPU              *pGpu,
    NV_ERROR_CONT_ERR_ID errorCode,
    NvU32                *pTableIndex
)
{
    NvU32 index;
    NvU32 tableSize = NV_ARRAY_ELEMENTS(g_errContStateTable);

    NV_ASSERT_OR_RETURN(pTableIndex != NULL, NV_ERR_INVALID_ARGUMENT);

    for (index = 0; index < tableSize; index++)
    {
        if (errorCode == g_errContStateTable[index].errorCode)
        {
            *pTableIndex = index;
            return NV_OK;
        }
    }

    return NV_ERR_INVALID_ARGUMENT;
}

/*!
 * @brief Send NV2080_NOTIFIER*
 *
 * @param[in]  pGpu                   OBJGPU pointer
 * @param[in]  errorCode              Error Containment error code
 * @param[in]  loc                    Location, SubLocation information
 * @param[in]  nv2080Notifier         NV2080_NOTIFIER*
 *
 * @returns NV_STATUS
 */
static
NV_STATUS
_gpuNotifySubDeviceEventNotifier_GA100
(
    OBJGPU                 *pGpu,
    NV_ERROR_CONT_ERR_ID    errorCode,
    NV_ERROR_CONT_LOCATION  loc,
    NvU32                   nv2080Notifier
)
{
    NvV16 info16 = 0;
    NvV32 info32 = 0;
    RM_ENGINE_TYPE localRmEngineType = 0;

    // Return if no notifier needs to be sent for this errorCode.
    if (nv2080Notifier == NO_NV2080_NOTIFIER)
    {
        return NV_OK;
    }

    switch (errorCode)
    {
        // Intentional fall-through
        case NV_ERROR_CONT_ERR_ID_E01_FB_ECC_DED:
        case NV_ERROR_CONT_ERR_ID_E02_FB_ECC_DED_IN_CBC_STORE:
        case NV_ERROR_CONT_ERR_ID_E09_FBHUB_POISON:
        case NV_ERROR_CONT_ERR_ID_E20_XALEP_EGRESS_POISON:
        case NV_ERROR_CONT_ERR_ID_E21A_XALEP_INGRESS_CONTAINED_POISON:
        case NV_ERROR_CONT_ERR_ID_E21B_XALEP_INGRESS_UNCONTAINED_POISON:
            info16 = FB_MEMORY_ERROR;
            break;

        // Intentional fall-through
        case NV_ERROR_CONT_ERR_ID_E05_LTC_ECC_DSTG:
        case NV_ERROR_CONT_ERR_ID_E06_LTC_UNSUPPORTED_CLIENT_POISON:
        case NV_ERROR_CONT_ERR_ID_E07_LTC_ECC_TSTG:
        case NV_ERROR_CONT_ERR_ID_E08_LTC_ECC_RSTG:
            info16 = LTC_ERROR;
            break;

        case NV_ERROR_CONT_ERR_ID_E10_SM_POISON:
        case NV_ERROR_CONT_ERR_ID_E16_GCC_POISON:
        case NV_ERROR_CONT_ERR_ID_E17_CTXSW_POISON:
            info16 = ROBUST_CHANNEL_GR_EXCEPTION;
            break;

        // Intentional fall-through
        case NV_ERROR_CONT_ERR_ID_E12A_CE_POISON_IN_USER_CHANNEL:
        case NV_ERROR_CONT_ERR_ID_E12B_CE_POISON_IN_KERNEL_CHANNEL:
            NV_ASSERT_OR_RETURN(loc.locType == NV_ERROR_CONT_LOCATION_TYPE_ENGINE, NV_ERR_INVALID_ARGUMENT);
            //
            // If SMC is enabled, RM need to notify partition local engine ID. Convert
            // global ID to partition local if client has filled proper engine IDs
            //
            localRmEngineType = loc.locInfo.engineLoc.rmEngineId;
            if (IS_MIG_IN_USE(pGpu) &&
                RM_ENGINE_TYPE_IS_VALID(loc.locInfo.engineLoc.rmEngineId))
            {
                KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
                MIG_INSTANCE_REF ref;

                NV_ASSERT_OK_OR_RETURN(
                    kmigmgrGetInstanceRefFromDevice(pGpu,
                                                    pKernelMIGManager,
                                                    loc.locInfo.engineLoc.pDevice,
                                                    &ref));

                if (!kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, loc.locInfo.engineLoc.rmEngineId, ref))
                {
                    // Notifier is requested for an unsupported engine
                    NV_PRINTF(LEVEL_ERROR,
                              "Notifier requested for an unsupported rm engine id (0x%x)\n",
                              loc.locInfo.engineLoc.rmEngineId);
                    return NV_ERR_INVALID_ARGUMENT;
                }

                // Override the engine type with the local engine idx
                NV_ASSERT_OK_OR_RETURN(
                    kmigmgrGetGlobalToLocalEngineType(pGpu,
                                                      pKernelMIGManager,
                                                      ref,
                                                      loc.locInfo.engineLoc.rmEngineId,
                                                      &localRmEngineType));
            }

            info16 = ROBUST_CHANNEL_CE_ERROR(NV2080_ENGINE_TYPE_COPY_IDX(localRmEngineType));
            break;

        case NV_ERROR_CONT_ERR_ID_E13_MMU_POISON:
            info16 = ROBUST_CHANNEL_FIFO_ERROR_MMU_ERR_FLT;
            break;

        case NV_ERROR_CONT_ERR_ID_E22_PMU_POISON:
            info16 = PMU_ERROR;
            break;

        case NV_ERROR_CONT_ERR_ID_E23_SEC2_POISON:
            info16 = ROBUST_CHANNEL_SEC2_ERROR;
            break;

        case NV_ERROR_CONT_ERR_ID_E24_GSP_POISON:
            info16 = GSP_ERROR;
            break;

        case NV_ERROR_CONT_ERR_ID_E25_FBFALCON_POISON:
            info16 = ROBUST_CHANNEL_UNCONTAINED_ERROR;
            break;

        case NV_ERROR_CONT_ERR_ID_E26_NVDEC_POISON:
            localRmEngineType = loc.locInfo.engineLoc.rmEngineId;
            info16 = ROBUST_CHANNEL_NVDEC_ERROR(NV2080_ENGINE_TYPE_NVDEC_IDX(localRmEngineType));
            break;

        case NV_ERROR_CONT_ERR_ID_E27_NVJPG_POISON:
            localRmEngineType = loc.locInfo.engineLoc.rmEngineId;
            info16 = ROBUST_CHANNEL_NVJPG_ERROR(NV2080_ENGINE_TYPE_NVJPEG_IDX(localRmEngineType));
            break;

        case NV_ERROR_CONT_ERR_ID_E28_OFA_POISON:
            localRmEngineType = loc.locInfo.engineLoc.rmEngineId;
            info16 = ROBUST_CHANNEL_OFA_ERROR(NV2080_ENGINE_TYPE_OFA_IDX(localRmEngineType));
            break;
    }

    gpuNotifySubDeviceEvent(pGpu,
                            nv2080Notifier,
                            NULL,
                            0,
                            info32,         // Unused
                            info16);

    return NV_OK;
}

/*!
 * @brief Generate error log for corresponding error containment error code.
 *
 * Format / example :
 *    1) Contained Error with SMC Partition attribution (Error attributable to SMC Partition or process in SMC partition):
 *    2) Contained Error with no SMC partitioning  (Error attributable to process on GPU):
 *    3) Uncontaned Error
 *
 * >> NVRM: Xid (PCI:0000:01:00 GPU-I:05): 94, pid=7194, Contained: CE User Channel (0x9). RST: No, D-RST: No
 * >> NVRM: Xid (PCI:0000:01:00): 94, pid=7062, Contained: CE User Channel (0x9). RST: No, D-RST: No
 * >> NVRM: Xid (PCI:0000:01:00): 95, pid=7062, Uncontained: LTC TAG (0x2,0x1). RST: Yes, D-RST: No
 *
 * @param[in]  pGpu                   OBJGPU pointer
 * @param[in]  errorCode              Error Containment error code
 * @param[in]  loc                    Location, SubLocation information
 * @param[in]  pErrorContSmcSetting   Error containment SMC Disable / Enable settings
 *
 * @returns NV_STATUS
 */
static
NV_STATUS
_gpuGenerateErrorLog_GA100(OBJGPU                           *pGpu,
                           NV_ERROR_CONT_ERR_ID              errorCode,
                           NV_ERROR_CONT_LOCATION            loc,
                           NV_ERROR_CONT_SMC_DIS_EN_SETTING *pErrorContSmcSetting)
{
    RM_ENGINE_TYPE localRmEngineType;
    NvU32 rcErrorCode = pErrorContSmcSetting->rcErrorCode;

    NV_ASSERT_OR_RETURN((pErrorContSmcSetting != NULL), NV_ERR_INVALID_ARGUMENT);

    switch (loc.locType)
    {
        case NV_ERROR_CONT_LOCATION_TYPE_DRAM:
            nvErrorLog_va((void *)pGpu,
                          rcErrorCode,
                          "%s: %s (0x%x,0x%x). physAddr: 0x%08llx RST: %s, D-RST: %s",
                          rcErrorCode == ROBUST_CHANNEL_CONTAINED_ERROR ?
                              ROBUST_CHANNEL_CONTAINED_ERROR_STR :
                              ROBUST_CHANNEL_UNCONTAINED_ERROR_STR,
                          ppErrContErrorIdStr[errorCode],
                          loc.locInfo.dramLoc.partition,
                          loc.locInfo.dramLoc.subPartition,
                          loc.locInfo.dramLoc.physicalAddress,
                          pErrorContSmcSetting->bGpuResetReqd ? "Yes" : "No",
                          pErrorContSmcSetting->bGpuDrainAndResetReqd ? "Yes" : "No");
            break;

        case NV_ERROR_CONT_LOCATION_TYPE_LTC:
            nvErrorLog_va((void *)pGpu,
                          rcErrorCode,
                          "%s: %s (0x%x,0x%x). RST: %s, D-RST: %s",
                          rcErrorCode == ROBUST_CHANNEL_CONTAINED_ERROR ?
                              ROBUST_CHANNEL_CONTAINED_ERROR_STR :
                              ROBUST_CHANNEL_UNCONTAINED_ERROR_STR,
                          ppErrContErrorIdStr[errorCode],
                          loc.locInfo.ltcLoc.partition,
                          loc.locInfo.ltcLoc.slice,
                          pErrorContSmcSetting->bGpuResetReqd ? "Yes" : "No",
                          pErrorContSmcSetting->bGpuDrainAndResetReqd ? "Yes" : "No");
            break;

        case NV_ERROR_CONT_LOCATION_TYPE_ENGINE:
            //
            // If SMC is enabled, RM need to notify partition local engine ID. Convert
            // global ID to partition local if client has filled proper engine IDs
            //
            localRmEngineType = loc.locInfo.engineLoc.rmEngineId;
            if (IS_MIG_IN_USE(pGpu) &&
                RM_ENGINE_TYPE_IS_VALID(loc.locInfo.engineLoc.rmEngineId))
            {
                KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
                MIG_INSTANCE_REF ref;
                NV_ASSERT_OK_OR_RETURN(kmigmgrGetMIGReferenceFromEngineType(pGpu,
                                                                            pKernelMIGManager,
                                                                            loc.locInfo.engineLoc.rmEngineId,
                                                                            &ref));
                // Override the engine type with the local engine idx
                NV_ASSERT_OK_OR_RETURN(kmigmgrGetGlobalToLocalEngineType(pGpu,
                                                                         pKernelMIGManager,
                                                                         ref,
                                                                         loc.locInfo.engineLoc.rmEngineId,
                                                                         &localRmEngineType));
            }

            nvErrorLog_va((void *)pGpu,
                          rcErrorCode,
                          "%s: %s (0x%x). RST: %s, D-RST: %s",
                          rcErrorCode == ROBUST_CHANNEL_CONTAINED_ERROR ?
                              ROBUST_CHANNEL_CONTAINED_ERROR_STR :
                              ROBUST_CHANNEL_UNCONTAINED_ERROR_STR,
                          ppErrContErrorIdStr[errorCode],
                          gpuGetNv2080EngineType(localRmEngineType),
                          pErrorContSmcSetting->bGpuResetReqd ? "Yes" : "No",
                          pErrorContSmcSetting->bGpuDrainAndResetReqd ? "Yes" : "No");
            break;

        case NV_ERROR_CONT_LOCATION_TYPE_VF:
            nvErrorLog_va((void *)pGpu,
                          rcErrorCode,
                          "%s: %s (0x%x). RST: %s, D-RST: %s",
                          rcErrorCode == ROBUST_CHANNEL_CONTAINED_ERROR ?
                              ROBUST_CHANNEL_CONTAINED_ERROR_STR :
                              ROBUST_CHANNEL_UNCONTAINED_ERROR_STR,
                          ppErrContErrorIdStr[errorCode],
                          loc.locInfo.vfGfid,
                          pErrorContSmcSetting->bGpuResetReqd ? "Yes" : "No",
                          pErrorContSmcSetting->bGpuDrainAndResetReqd ? "Yes" : "No");
            break;

        case NV_ERROR_CONT_LOCATION_TYPE_NONE:
            nvErrorLog_va((void *)pGpu,
                          rcErrorCode,
                          "%s: %s. RST: %s, D-RST: %s",
                          rcErrorCode == ROBUST_CHANNEL_CONTAINED_ERROR ?
                              ROBUST_CHANNEL_CONTAINED_ERROR_STR :
                              ROBUST_CHANNEL_UNCONTAINED_ERROR_STR,
                          ppErrContErrorIdStr[errorCode],
                          pErrorContSmcSetting->bGpuResetReqd ? "Yes" : "No",
                          pErrorContSmcSetting->bGpuDrainAndResetReqd ? "Yes" : "No");
            break;
    }

    return NV_OK;
}

/*!
 * @brief Determine Error Containment RC code, print Xid, send NV2080_NOTIFIER*,
 * mark device for reset or mark device for drain and reset as indicated in
 * error containment state table (refer gpu/error_cont.h).
 *
 * @param[in]  pGpu              OBJGPU pointer
 * @param[in]  errorCode         Error Containment error code
 * @param[in]  loc               Location, SubLocation information
 * @param[out] pRcErrorCode      RC Error code
 *
 * @returns NV_STATUS
 */
NV_STATUS
gpuUpdateErrorContainmentState_GA100
(
    OBJGPU                 *pGpu,
    NV_ERROR_CONT_ERR_ID    errorCode,
    NV_ERROR_CONT_LOCATION  loc,
    NvU32                  *pRcErrorCode
)
{
    NvU32 tableIndex = 0;
    NvBool bIsSmcEnabled = NV_FALSE;
    NvU32 smcDisEnSettingIndex = 0;
    NvU32 rcErrorCode = 0;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NV_ERROR_CONT_SMC_DIS_EN_SETTING *pErrorContSmcSetting = NULL;

    if (!gpuIsGlobalPoisonFuseEnabled(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_ASSERT_OK_OR_RETURN(_gpuGetErrorContStateTableIndex_GA100(pGpu, errorCode, &tableIndex));

    // Check if MIG GPU partitioning is enabled
    if (IS_MIG_IN_USE(pGpu))
    {
        bIsSmcEnabled = NV_TRUE;
    }

    // MIG Memory partitioning config entry index.
    if (pKernelMIGManager != NULL && kmigmgrIsMIGMemPartitioningEnabled(pGpu, pKernelMIGManager))
    {
        smcDisEnSettingIndex = 1;
    }

    pErrorContSmcSetting = &(g_errContStateTable[tableIndex].smcDisEnSetting[smcDisEnSettingIndex]);

    rcErrorCode = pErrorContSmcSetting->rcErrorCode;

    // Pass RC Error code if user requested it.
    if (pRcErrorCode != NULL)
    {
        *pRcErrorCode = rcErrorCode;
    }

    // Update partition attribution for this exception only if SMC is enabled.
    if (pErrorContSmcSetting->bPrintSmcPartitionInfo && bIsSmcEnabled)
    {
        // Fall through on error.
        gpuSetPartitionErrorAttribution_HAL(pGpu,
                                            errorCode,
                                            loc,
                                            rcErrorCode);
    }

    // Print Xid only if Ampere Error Containment XIDs printing is enabled and rcErrorCode is valid
    if (gpuIsAmpereErrorContainmentXidEnabled(pGpu) && rcErrorCode != NO_XID)
    {
        NV_ASSERT_OK_OR_RETURN(_gpuGenerateErrorLog_GA100(pGpu,
                                                          errorCode,
                                                          loc,
                                                          pErrorContSmcSetting));
    }

    // Send NV2080_NOTIFIER*
    if (pErrorContSmcSetting->nv2080Notifier != NO_NV2080_NOTIFIER)
    {
        NV_ASSERT_OK(_gpuNotifySubDeviceEventNotifier_GA100(pGpu,
                                                            errorCode,
                                                            loc,
                                                            pErrorContSmcSetting->nv2080Notifier));
    }

    // Set the scratch bit to indicate the GPU needs to be reset.
    if ((pErrorContSmcSetting->bGpuResetReqd) &&
        (gpuMarkDeviceForReset(pGpu) != NV_OK))
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to mark GPU for pending reset");
    }

    // Set the scratch bit to indicate the GPU needs to be reset.
    if (pErrorContSmcSetting->bGpuDrainAndResetReqd &&
        gpuMarkDeviceForDrainAndReset(pGpu) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to mark GPU for pending drain and reset");
    }

    return NV_OK;
}

NvBool
gpuCheckIfFbhubPoisonIntrPending_GA100
(
    OBJGPU *pGpu
)
{
    return intrIsVectorPending_HAL(pGpu, GPU_GET_INTR(pGpu), NV_PFB_FBHUB_POISON_INTR_VECTOR_HW_INIT, NULL);
}



NV_STATUS
gpuGetDeviceInfoTableSectionInfos_GA100
(
    OBJGPU                    *pGpu,
    DeviceInfoTableSectionVec *pVec
)
{
    NV_ASSERT_OR_RETURN(pVec != NULL, NV_ERR_INVALID_ARGUMENT);

    NvU32 deviceInfoCfg = GPU_REG_RD32(pGpu, NV_PTOP_DEVICE_INFO_CFG);

    DeviceInfoTableSection section = {
        .row0Addr   = NV_PTOP_DEVICE_INFO2(0),
        .maxDevices = DRF_VAL(_PTOP, _DEVICE_INFO_CFG, _MAX_DEVICES,
                              deviceInfoCfg),
        .maxRows    = DRF_VAL(_PTOP, _DEVICE_INFO_CFG, _NUM_ROWS,
                              deviceInfoCfg),
        .maxRowsPerDevice = DRF_VAL(_PTOP, _DEVICE_INFO_CFG,
                                    _MAX_ROWS_PER_DEVICE,
                                    deviceInfoCfg)};

    NV_ASSERT_OR_RETURN(vectAppend(pVec, &section) != NULL, NV_ERR_NO_MEMORY);

    return NV_OK;
};

