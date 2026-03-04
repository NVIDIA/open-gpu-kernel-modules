/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/error_cont.h"
#include "gpu/mig_mgr/kernel_mig_manager.h"

// Error Containment error ID string description
const char *ppErrContErrorIdStr[] = NV_ERROR_CONT_ERR_ID_STRING_PUBLIC;

/*!
 * Error Containment state table showing policy settings for each error ID
 *
 * Where:
 * RC_Recovery_Type:
 *      Type of RC recovery handling in response to a given error. Possible values:
 *
 * - NO_RC                                         : No RC Recovery performed. Subsequent 2nd interrupt by engine
 *                                                   consuming poison will determine the RC Recovery type.
 * - RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE          : RC Recovery compute channels of only the processes whose
 *                                                   channels were loaded on halted TSG when _E10_SM_POISON or
 *                                                   _E12A_CE_POISON_IN_USER_CHANNEL occurs.
 * - RC_ALL_COMPUTE_CHANNELS_IN_SPECIFIC_PARTITION : RC Recovery compute channels of only specific MIG partition
 *                                                   as that error can be attributed to a specific MIG partition.
 * - RC_ALL_COMPUTE_CHANNELS                       : RC Recovery ALL compute channels on a GPU that saw this interrupt.
 *                                                   (If MIG is enabled, then RC Recovery compute channels in all MIG partitions)
 * - CE_TSG_RESET                                  : Reset the halted CE Engine. Impacts the CE channels loaded on the TSG when the CE Halted.
 *                                                   This is used in NV_ERROR_CONT_ERR_ID_E12A_CE_POISON_IN_USER_CHANNEL &
 *                                                   NV_ERROR_CONT_ERR_ID_E12B_CE_POISON_IN_KERNEL_CHANNEL along with additional
 *                                                   Compute Channels RC policy (either RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE or
 *                                                   RC_ALL_COMPUTE_CHANNELS).
 */
static const NV_ERROR_CONT_STATE_TABLE g_errContStateTable_GA100[] =
{
    /* errorCode                                                ,  rcErrorCode                      , bGpuResetReqd, bGpuDrainAndResetReqd, bPrintSmcPartitionInfo, nv2080Notifier                             , Dynamic Page Blacklisting , RC_Recovery_Type                                    */
    { NV_ERROR_CONT_ERR_ID_E01_FB_ECC_DED                       , {{NO_XID                          , NV_FALSE     , NV_FALSE             , NV_FALSE              , NO_NV2080_NOTIFIER                      /* , Yes(PMA but not subheap)  , NO_RC                                               */ },
                                                                   {NO_XID                          , NV_FALSE     , NV_FALSE             , NV_FALSE              , NO_NV2080_NOTIFIER                      /* , Yes(PMA but not subheap)  , NO_RC                                               */ }}},
    { NV_ERROR_CONT_ERR_ID_E02_FB_ECC_DED_IN_CBC_STORE          , {{ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , Yes(PMA but not subheap)  , RC_ALL_COMPUTE_CHANNELS                             */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_TRUE              , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , Yes(PMA but not subheap)  , RC_ALL_COMPUTE_CHANNELS_IN_SPECIFIC_PARTITION       */ }}},
    { NV_ERROR_CONT_ERR_ID_E05_LTC_ECC_DSTG                     , {{NO_XID                          , NV_FALSE     , NV_FALSE             , NV_FALSE              , NO_NV2080_NOTIFIER                      /* , No                        , NO_RC                                               */ },
                                                                   {NO_XID                          , NV_FALSE     , NV_FALSE             , NV_FALSE              , NO_NV2080_NOTIFIER                      /* , No                        , NO_RC                                               */ }}},
    { NV_ERROR_CONT_ERR_ID_E06_LTC_UNSUPPORTED_CLIENT_POISON    , {{ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_FALSE     , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_ALL_COMPUTE_CHANNELS_IN_SPECIFIC_PARTITION       */ }}},
    { NV_ERROR_CONT_ERR_ID_E07_LTC_ECC_TSTG                     , {{ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ },
                                                                   {ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ }}},
    { NV_ERROR_CONT_ERR_ID_E08_LTC_ECC_RSTG                     , {{ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_TRUE              , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_ALL_COMPUTE_CHANNELS_IN_SPECIFIC_PARTITION       */ }}},
    { NV_ERROR_CONT_ERR_ID_E09_FBHUB_POISON                     , {{ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ },
                                                                   {ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ }}},
    { NV_ERROR_CONT_ERR_ID_E10_SM_POISON                        , {{ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE                */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE                */ }}},
    { NV_ERROR_CONT_ERR_ID_E12A_CE_POISON_IN_USER_CHANNEL       , {{ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE + CE_TSG_RESET */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE + CE_TSG_RESET */ }}},
    { NV_ERROR_CONT_ERR_ID_E12B_CE_POISON_IN_KERNEL_CHANNEL     , {{ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS              + CE_TSG_RESET */ },
                                                                   {ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS              + CE_TSG_RESET */ }}},
    { NV_ERROR_CONT_ERR_ID_E13_MMU_POISON                       , {{ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , NO_RC                                               */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , NO_RC                                               */ }}},
    { NV_ERROR_CONT_ERR_ID_E16_GCC_POISON                       , {{ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE                */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE                */ }}},
    { NV_ERROR_CONT_ERR_ID_E17_CTXSW_POISON                     , {{ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE                */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE                */ }}},
    { NV_ERROR_CONT_ERR_ID_E20_XALEP_EGRESS_POISON              , {{ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ },
                                                                   {ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ }}}
};

/*!
 * @brief Send NV2080 event notification
 *
 * @param[in] pGpu            OBJGPU pointer
 * @param[in] errorCode       Error Containment error code
 * @param[in] loc             Location, SubLocation information
 * @param[in] nv2080Notifier  NV2080 notifier type
 *
 * @returns NV_STATUS
 */
static NV_STATUS _gpuNotifySubDeviceEventNotifier(OBJGPU                 *pGpu,
                                                  NV_ERROR_CONT_ERR_ID    errorCode,
                                                  NV_ERROR_CONT_LOCATION  loc,
                                                  NvU32                   nv2080Notifier);

/*!
 * @brief Get index of specified errorCode in the Error Containment state table
 *
 * @param[in] pGpu         OBJGPU pointer
 * @param[in] errorCode    Error Containment error code
 * @param[in] pTableIndex  Index of specified errorCode in the Error Containment State table
 *
 * @returns NV_STATUS
 */
static NV_STATUS _gpuGetErrorContSettings(OBJGPU                           *pGpu,
                                          NV_ERROR_CONT_ERR_ID              errorCode,
                                          NvBool                            bIsMIGEnabled,
                                          NV_ERROR_CONT_SMC_DIS_EN_SETTING *pErrorContSettings);

/*!
 * @brief Generate error log for corresponding Error Containment error code
 *
 * @param[in] pGpu                   OBJGPU pointer
 * @param[in] errorCode              Error Containment error code
 * @param[in] loc                    Location, SubLocation information
 * @param[in] pErrorContSmcSettings  Error Containment SMC Disable/Enable settings
 *
 * @returns NV_STATUS
 */
static NV_STATUS _gpuGenerateErrorLog(OBJGPU                           *pGpu,
                                      NV_ERROR_CONT_ERR_ID              errorCode,
                                      NV_ERROR_CONT_LOCATION            loc,
                                      NV_ERROR_CONT_SMC_DIS_EN_SETTING *pErrorContSmcSettings);

/*!
 * @brief Send NV2080 event notification
 *
 * @param[in] pGpu            OBJGPU pointer
 * @param[in] errorCode       Error Containment error code
 * @param[in] loc             Location, SubLocation information
 * @param[in] nv2080Notifier  NV2080 notifier type
 *
 * @returns NV_STATUS
 */
static
NV_STATUS
_gpuNotifySubDeviceEventNotifier
(
    OBJGPU                 *pGpu,
    NV_ERROR_CONT_ERR_ID    errorCode,
    NV_ERROR_CONT_LOCATION  loc,
    NvU32                   nv2080Notifier
)
{
    NvV16 info16 = 0;
    RM_ENGINE_TYPE localRmEngineType = 0;

    // Return if no notifier needs to be sent for this errorCode
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

        // Intentional fall-through
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
            info16 = ROBUST_CHANNEL_CONTAINED_ERROR;
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

    gpuNotifySubDeviceEvent(pGpu, nv2080Notifier, NULL, 0, 0, info16);

    return NV_OK;
}

/*!
 * @brief Get recovery settings of specified errorCode in the Error Containment state table
 *
 * @param[in] pGpu         OBJGPU pointer
 * @param[in] errorCode    Error Containment error code
 * @param[in] pTableIndex  Index of specified errorCode in the Error Containment state table
 *
 * @returns NV_STATUS
 */
static
NV_STATUS
_gpuGetErrorContSettings
(
    OBJGPU                           *pGpu,
    NV_ERROR_CONT_ERR_ID              errorCode,
    NvBool                            bIsMIGEnabled,
    NV_ERROR_CONT_SMC_DIS_EN_SETTING *pErrorContSettings
)
{
    NvU32 tableSize = 0;
    NvU32 smcDisEnSettingIdx = bIsMIGEnabled ? 1 : 0;
    const NV_ERROR_CONT_STATE_TABLE *pErrContTable = gpuGetErrorContStateTableAndSize_HAL(pGpu, &tableSize);

    NV_ASSERT_OR_RETURN(pErrorContSettings != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pErrContTable != NULL, NV_ERR_INVALID_POINTER);

    for (NvU32 index = 0; index < tableSize; index++)
    {
        if (pErrContTable[index].errorCode == errorCode)
        {
            *pErrorContSettings = pErrContTable[index].smcDisEnSetting[smcDisEnSettingIdx];
            return NV_OK;
        }
    }

    NV_PRINTF(LEVEL_ERROR, "Invalid errorCode: 0x%x\n", errorCode);
    return NV_ERR_INVALID_ARGUMENT;
}

/*!
 * @brief Generate error log for corresponding Error Containment error code
 *
 * Format/example:
 *    1) Contained Error with SMC Partition attribution (Error attributable to SMC Partition or process in SMC partition):
 *       "NVRM: Xid (PCI:0000:01:00 GPU-I:05): 94, pid=7194, Contained: CE User Channel (0x9). RST: No, D-RST: No"
 *
 *    2) Contained Error with no SMC partitioning  (Error attributable to process on GPU):
 *       "NVRM: Xid (PCI:0000:01:00): 94, pid=7062, Contained: CE User Channel (0x9). RST: No, D-RST: No"
 *
 *    3) Uncontaned Error:
 *       "NVRM: Xid (PCI:0000:01:00): 95, pid=7062, Uncontained: LTC TAG (0x2, 0x1). RST: Yes, D-RST: No"
 *
 * @param[in] pGpu                   OBJGPU pointer
 * @param[in] errorCode              Error Containment error code
 * @param[in] loc                    Location, SubLocation information
 * @param[in] pErrorContSmcSettings  Error Containment SMC Disable/Enable settings
 *
 * @returns NV_STATUS
 */
static
NV_STATUS
_gpuGenerateErrorLog
(
    OBJGPU                           *pGpu,
    NV_ERROR_CONT_ERR_ID              errorCode,
    NV_ERROR_CONT_LOCATION            loc,
    NV_ERROR_CONT_SMC_DIS_EN_SETTING *pErrorContSmcSettings
)
{
    RM_ENGINE_TYPE localRmEngineType;
    NvU32 rcErrorCode = pErrorContSmcSettings->rcErrorCode;

    NV_ASSERT_OR_RETURN((pErrorContSmcSettings != NULL), NV_ERR_INVALID_ARGUMENT);

    switch (loc.locType)
    {
        case NV_ERROR_CONT_LOCATION_TYPE_DRAM:
        {
            nvErrorLog_va((void *)pGpu,
                          rcErrorCode,
                          "%s: %s (0x%x, 0x%x). physAddr: 0x%08llx RST: %s, D-RST: %s",
                          rcErrorCode == ROBUST_CHANNEL_CONTAINED_ERROR ?
                              ROBUST_CHANNEL_CONTAINED_ERROR_STR :
                              ROBUST_CHANNEL_UNCONTAINED_ERROR_STR,
                          ppErrContErrorIdStr[errorCode],
                          loc.locInfo.dramLoc.partition,
                          loc.locInfo.dramLoc.subPartition,
                          loc.locInfo.dramLoc.physicalAddress,
                          pErrorContSmcSettings->bGpuResetReqd ? "Yes" : "No",
                          pErrorContSmcSettings->bGpuDrainAndResetReqd ? "Yes" : "No");
            break;
        }
        case NV_ERROR_CONT_LOCATION_TYPE_LTC:
        {
            nvErrorLog_va((void *)pGpu,
                          rcErrorCode,
                          "%s: %s (0x%x, 0x%x). RST: %s, D-RST: %s",
                          rcErrorCode == ROBUST_CHANNEL_CONTAINED_ERROR ?
                              ROBUST_CHANNEL_CONTAINED_ERROR_STR :
                              ROBUST_CHANNEL_UNCONTAINED_ERROR_STR,
                          ppErrContErrorIdStr[errorCode],
                          loc.locInfo.ltcLoc.partition,
                          loc.locInfo.ltcLoc.slice,
                          pErrorContSmcSettings->bGpuResetReqd ? "Yes" : "No",
                          pErrorContSmcSettings->bGpuDrainAndResetReqd ? "Yes" : "No");
            break;
        }
        case NV_ERROR_CONT_LOCATION_TYPE_ENGINE:
        {
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
                    kmigmgrGetMIGReferenceFromEngineType(pGpu,
                                                         pKernelMIGManager,
                                                         loc.locInfo.engineLoc.rmEngineId,
                                                         &ref));
                // Override the engine type with the local engine idx
                NV_ASSERT_OK_OR_RETURN(
                    kmigmgrGetGlobalToLocalEngineType(pGpu,
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
                          pErrorContSmcSettings->bGpuResetReqd ? "Yes" : "No",
                          pErrorContSmcSettings->bGpuDrainAndResetReqd ? "Yes" : "No");
            break;
        }
        case NV_ERROR_CONT_LOCATION_TYPE_VF:
        {
            nvErrorLog_va((void *)pGpu,
                          rcErrorCode,
                          "%s: %s (0x%x). RST: %s, D-RST: %s",
                          rcErrorCode == ROBUST_CHANNEL_CONTAINED_ERROR ?
                              ROBUST_CHANNEL_CONTAINED_ERROR_STR :
                              ROBUST_CHANNEL_UNCONTAINED_ERROR_STR,
                          ppErrContErrorIdStr[errorCode],
                          loc.locInfo.vfGfid,
                          pErrorContSmcSettings->bGpuResetReqd ? "Yes" : "No",
                          pErrorContSmcSettings->bGpuDrainAndResetReqd ? "Yes" : "No");
            break;
        }
        case NV_ERROR_CONT_LOCATION_TYPE_NONE:
        {
            nvErrorLog_va((void *)pGpu,
                          rcErrorCode,
                          "%s: %s. RST: %s, D-RST: %s",
                          rcErrorCode == ROBUST_CHANNEL_CONTAINED_ERROR ?
                              ROBUST_CHANNEL_CONTAINED_ERROR_STR :
                              ROBUST_CHANNEL_UNCONTAINED_ERROR_STR,
                          ppErrContErrorIdStr[errorCode],
                          pErrorContSmcSettings->bGpuResetReqd ? "Yes" : "No",
                          pErrorContSmcSettings->bGpuDrainAndResetReqd ? "Yes" : "No");
            break;
        }
    }

    return NV_OK;
}

/*!
 * @brief Get the Error Containment state table and its size
 *
 * @param[in]  pGpu        OBJGPU pointer
 * @param[out] pTableSize  Number of entries in the table
 *
 * @returns const NV_ERROR_CONT_STATE_TABLE*
 */
const NV_ERROR_CONT_STATE_TABLE*
gpuGetErrorContStateTableAndSize_GA100
(
    OBJGPU *pGpu,
    NvU32  *pTableSize
)
{
    NV_ASSERT_OR_RETURN(pTableSize != NULL, NULL);

    *pTableSize = NV_ARRAY_ELEMENTS(g_errContStateTable_GA100);

    return g_errContStateTable_GA100;
}

/*!
 * @brief Determine Error Containment RC code, print Xid,
 * send NV2080 event notification, and mark device for reset or
 * drain and reset as indicated in the Error Containment state table
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  errorCode     Error Containment error code
 * @param[in]  loc           Location, SubLocation information
 * @param[out] pRcErrorCode  RC Error code
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
    NvU32  rcErrorCode   = 0;
    NvBool bIsSmcEnabled = IS_MIG_IN_USE(pGpu);
    NV_ERROR_CONT_SMC_DIS_EN_SETTING errorContSmcSettings;

    if (!gpuIsGlobalPoisonFuseEnabled(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // Get Error Containment settings
    NV_ASSERT_OK_OR_RETURN(_gpuGetErrorContSettings(pGpu, errorCode, bIsSmcEnabled, &errorContSmcSettings));

    rcErrorCode = errorContSmcSettings.rcErrorCode;

    // Pass RC error code if user requested it
    if (pRcErrorCode != NULL)
    {
        *pRcErrorCode = rcErrorCode;
    }

    // Update partition attribution for this exception only if SMC is enabled
    if (errorContSmcSettings.bPrintSmcPartitionInfo && bIsSmcEnabled)
    {
        // Fall through on error
        gpuSetPartitionErrorAttribution_HAL(pGpu, errorCode, loc, rcErrorCode);
    }

    // Print Xid only if Ampere Error Containment Xid printing is enabled and rcErrorCode is valid
    if (gpuIsAmpereErrorContainmentXidEnabled(pGpu) && (rcErrorCode != NO_XID))
    {
        NV_ASSERT_OK_OR_RETURN(_gpuGenerateErrorLog(pGpu, errorCode, loc, &errorContSmcSettings));
    }

    // Send event notification
    if (errorContSmcSettings.nv2080Notifier != NO_NV2080_NOTIFIER)
    {
        NV_ASSERT_OK(_gpuNotifySubDeviceEventNotifier(pGpu, errorCode, loc, errorContSmcSettings.nv2080Notifier));
    }

    // Set the scratch bit to indicate the GPU needs to be reset
    if (errorContSmcSettings.bGpuResetReqd && (gpuMarkDeviceForReset(pGpu) != NV_OK))
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to mark GPU for pending reset");
    }

    // Set the scratch bit to indicate the GPU needs to be reset
    if (errorContSmcSettings.bGpuDrainAndResetReqd && (gpuMarkDeviceForDrainAndReset(pGpu) != NV_OK))
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to mark GPU for pending drain and reset");
    }

    return NV_OK;
}
