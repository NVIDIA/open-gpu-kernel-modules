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
 * - RC_ALL_CHANNELS_IN_VF                         : RC Recovery compute channels of only specific GFID
 *                                                   as that error can be attributed to a VF.
 * - RC_ALL_USER_CHANNELS                          : RC Recovery ALL user channels on a GPU.
 * - RC_ALL_COMPUTE_CHANNELS                       : RC Recovery ALL compute channels on a GPU that saw this interrupt.
 *                                                   (If MIG is enabled, then RC Recovery compute channels in all MIG partitions)
 * - CE_TSG_RESET                                  : Reset the halted CE Engine. Impacts the CE channels loaded on the TSG when the CE Halted.
 *                                                   This is used in NV_ERROR_CONT_ERR_ID_E12A_CE_POISON_IN_USER_CHANNEL &
 *                                                   NV_ERROR_CONT_ERR_ID_E12B_CE_POISON_IN_KERNEL_CHANNEL along with additional
 *                                                   Compute Channels RC policy (either RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE or
 *                                                   RC_ALL_COMPUTE_CHANNELS).
 */
static const NV_ERROR_CONT_STATE_TABLE g_errContStateTable_GB100[] =
{
    /* errorCode                                                ,  rcErrorCode                      , bGpuResetReqd, bGpuDrainAndResetReqd, bPrintSmcPartitionInfo, nv2080Notifier                             , Dynamic Page Blacklisting , RC_Recovery_Type                                    */
    { NV_ERROR_CONT_ERR_ID_E01_FB_ECC_DED                       , {{NO_XID                          , NV_FALSE     , NV_FALSE             , NV_FALSE              , NO_NV2080_NOTIFIER                      /* , Yes(PMA but not subheap)  , NO_RC                                               */ },
                                                                   {NO_XID                          , NV_FALSE     , NV_FALSE             , NV_FALSE              , NO_NV2080_NOTIFIER                      /* , Yes(PMA but not subheap)  , NO_RC                                               */ }}},
    { NV_ERROR_CONT_ERR_ID_E02_FB_ECC_DED_IN_CBC_STORE          , {{ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , Yes(PMA but not subheap)  , RC_ALL_COMPUTE_CHANNELS                             */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_TRUE              , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , Yes(PMA but not subheap)  , RC_ALL_COMPUTE_CHANNELS_IN_SPECIFIC_PARTITION       */ }}},
    { NV_ERROR_CONT_ERR_ID_E05_LTC_ECC_DSTG                     , {{NO_XID                          , NV_FALSE     , NV_FALSE             , NV_FALSE              , NO_NV2080_NOTIFIER                      /* , No                        , NO_RC                                               */ },
                                                                   {NO_XID                          , NV_FALSE     , NV_FALSE             , NV_FALSE              , NO_NV2080_NOTIFIER                      /* , No                        , NO_RC                                               */ }}},
    { NV_ERROR_CONT_ERR_ID_E07_LTC_ECC_TSTG                     , {{ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ },
                                                                   {ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ }}},
    { NV_ERROR_CONT_ERR_ID_E08_LTC_ECC_RSTG                     , {{ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_TRUE              , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_ALL_COMPUTE_CHANNELS_IN_SPECIFIC_PARTITION       */ }}},
    { NV_ERROR_CONT_ERR_ID_E09_FBHUB_POISON                     , {{ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_FALSE     , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ },
                                                                   {ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_FALSE     , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ }}},
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
                                                                   {ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ }}},
    { NV_ERROR_CONT_ERR_ID_E21A_XALEP_INGRESS_CONTAINED_POISON  , {{ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_ALL_CHANNELS_IN_VF                               */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_ALL_CHANNELS_IN_VF                               */ }}},
    { NV_ERROR_CONT_ERR_ID_E21B_XALEP_INGRESS_UNCONTAINED_POISON, {{ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ },
                                                                   {ROBUST_CHANNEL_UNCONTAINED_ERROR, NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ }}},
    { NV_ERROR_CONT_ERR_ID_E22_PMU_POISON                       , {{ROBUST_CHANNEL_CONTAINED_ERROR  , NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ }}},
    { NV_ERROR_CONT_ERR_ID_E23_SEC2_POISON                      , {{ROBUST_CHANNEL_CONTAINED_ERROR  , NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ }}},
    { NV_ERROR_CONT_ERR_ID_E24_GSP_POISON                       , {{ROBUST_CHANNEL_CONTAINED_ERROR  , NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_USER_CHANNELS                                */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_USER_CHANNELS                                */ }}},
    { NV_ERROR_CONT_ERR_ID_E25_FBFALCON_POISON                  , {{ROBUST_CHANNEL_CONTAINED_ERROR  , NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_TRUE      , NV_FALSE             , NV_FALSE              , NV2080_NOTIFIERS_POISON_ERROR_FATAL     /* , No                        , RC_ALL_COMPUTE_CHANNELS                             */ }}},
    { NV_ERROR_CONT_ERR_ID_E26_NVDEC_POISON                     , {{ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE                */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE                */ }}},
    { NV_ERROR_CONT_ERR_ID_E27_NVJPG_POISON                     , {{ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE                */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE                */ }}},
    { NV_ERROR_CONT_ERR_ID_E28_OFA_POISON                       , {{ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE                */ },
                                                                   {ROBUST_CHANNEL_CONTAINED_ERROR  , NV_FALSE     , NV_FALSE             , NV_TRUE               , NV2080_NOTIFIERS_POISON_ERROR_NON_FATAL /* , No                        , RC_COMPUTE_CHANNELS_IN_ADDRESS_SPACE                */ }}}
};

/*!
 * @brief Get the Error Containment state table and its size
 *
 * @param[in]  pGpu        OBJGPU pointer
 * @param[out] pTableSize  Size of the table
 *
 * @returns const NV_ERROR_CONT_STATE_TABLE pointer
 */
const NV_ERROR_CONT_STATE_TABLE*
gpuGetErrorContStateTableAndSize_GB100
(
    OBJGPU *pGpu,
    NvU32  *pTableSize
)
{
    NV_ASSERT_OR_RETURN(pTableSize != NULL, NULL);

    *pTableSize = NV_ARRAY_ELEMENTS(g_errContStateTable_GB100);

    return g_errContStateTable_GB100;
}
