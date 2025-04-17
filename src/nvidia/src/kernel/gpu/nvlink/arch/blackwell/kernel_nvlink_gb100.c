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

#define NVOC_KERNEL_NVLINK_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/diagnostics/nv_debug_dump.h"
#include "kernel/gpu_mgr/gpu_mgr.h"

NV_STATUS
knvlinkGetSupportedCounters_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS *pParams
)
{
    portMemSet(pParams->counterMask, 0x0, sizeof(pParams->counterMask));

#define SET_COUNTER_MASK(masks, bit)                                                                                            \
    (masks)[NV2080_CTRL_NVLINK_COUNTER_V2_GROUP(bit)] |= NV2080_CTRL_NVLINK_COUNTER_V2_COUNTER_MASK(bit);                       \

    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_XMIT_PACKETS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_XMIT_BYTES);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_RCV_PACKETS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_RCV_BYTES);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_LINK_ERROR_RECOVERY_COUNTER);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_LINK_DOWNED_COUNTER);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_LINK_RECOVERY_SUCCESSFUL_COUNTER);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_RCV_ERRORS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_RCV_REMOTE_ERRORS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_RCV_GENERAL_ERRORS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_RCV_MALFORMED_PKT_ERROR);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_RCV_BUFFER_OVERRUN_ERROR);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_RCV_VL15DROPPED_ERROR);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_LINK_INTEGRITY_ERRORS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_BUFFER_OVERRUN_ERRORS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_XMIT_WAIT_TIME);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_XMIT_ERRORS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_SINGLE_ERROR_BLOCKS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_CORRECTED_BLOCKS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_UNCORRECTED_BLOCKS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_CORRECTED_SYMBOLS_LANE_0);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_CORRECTED_SYMBOLS_LANE_1);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_CORRECTED_SYMBOLS_TOTAL);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_RAW_ERRORS_LANE_0);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_RAW_ERRORS_LANE_1);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_CORRECTED_BITS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_RAW_BER_LANE_0);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_RAW_BER_LANE_1);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_RAW_BER_TOTAL);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_NO_ERROR_BLOCKS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_EFFECTIVE_ERRORS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_EFFECTIVE_BER);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_SYMBOL_ERRORS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_SYMBOL_BER);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_RECEIVED_BITS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_SYNC_HEADER_ERRORS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_PLR_RCV_BLOCKS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_PLR_RCV_BLOCKS_WITH_UNCORRECTABLE_ERRORS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_PLR_RCV_BLOCKS_WITH_ERRORS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_PLR_XMIT_BLOCKS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_PLR_XMIT_RETRY_BLOCKS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_PLR_XMIT_RETRY_EVENTS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_PLR_BW_LOSS);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_NVLE_RX_GOOD);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_NVLE_RX_ERROR);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_NVLE_RX_AUTH);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_NVLE_TX_GOOD);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_NVLE_TX_ERROR);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_0);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_1);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_2);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_3);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_4);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_5);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_6);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_7);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_8);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_9);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_10);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_11);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_12);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_13);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_14);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_HISTORY_15);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_TP_RX_DATA);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_TP_TX_DATA);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_TP_RX_RAW);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_TP_TX_RAW);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_L1_ENTRY);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_L1_ENTRY_FORCE);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_L1_EXIT);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_L1_EXIT_RECAL);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_L1_EXIT_REMOTE);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_L1_LP_STEADY_STATE_TIME);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_L1_HIGH_SPEED_STEADY_STATE_TIME);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_L1_OTHER_STATE_TIME);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_LP_LOCAL_ENTRY_TIME);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_LP_LOCAL_EXIT_TIME);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_LP_LOCAL_FULL_BW_ENTRY_TIME);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_LP_LOCAL_FULL_BW_EXIT_TIME);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_LP_REMOTE_ENTRY_TIME);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_LP_REMOTE_EXIT_TIME);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_LP_REMOTE_FULL_BW_ENTRY_TIME);
    SET_COUNTER_MASK(pParams->counterMask, NV2080_CTRL_NVLINK_COUNTER_LP_REMOTE_FULL_BW_EXIT_TIME);

#undef SET_COUNTER_MASK

    return NV_OK;
}

/*!
 * @brief Report a link training failure and dump error info to logs
 */
NV_STATUS
knvlinkLogAliDebugMessages_GB100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvBool        bFinal
)
{
    NV_STATUS status;
    NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS *pParams;
    NvU32 linkMask;
    NvU32 failures[7];
    NvU32 failure;
    NvU32 link;

    pParams = portMemAllocNonPaged(sizeof(NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS));
    if (pParams == NULL)
        return NV_ERR_NO_MEMORY;

    portMemSet(pParams, 0, sizeof(*pParams));
    pParams->ErrInfoFlags |= NV2080_CTRL_NVLINK_ERR_INFO_FLAGS_ALI_STATUS;

    NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
        knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                            NV2080_CTRL_CMD_NVLINK_GET_ERR_INFO,
                            (void *)pParams,
                            sizeof(*pParams)),
        portMemFree(pParams);
        return status; );

    linkMask = 0x0;
    failure = 0;
    portMemSet(failures, 0x0, sizeof(failures));

    FOR_EACH_INDEX_IN_MASK(32, link, KNVLINK_GET_MASK(pKernelNvlink, postRxDetLinkMask, 32))
    {
        if ((pParams->linkErrInfo[link].DLStatMN00 & 0xffff) != 0x0)
        {
            NV_PRINTF(LEVEL_ERROR, "ALI Error for GPU %d::linkId %d: 0x%x\n",
                      pGpu->gpuInstance,
                      link,
                      pParams->linkErrInfo[link].DLStatMN00);

            if (failure < NV_ARRAY_ELEMENTS(failures))
                failures[failure++] = pParams->linkErrInfo[link].DLStatMN00;

            linkMask |= NVBIT32(link);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    if (bFinal)
    {
        nvErrorLog_va((void *)pGpu, ALI_TRAINING_FAIL,
                      "NVLink: Link training failed for links 0x%x (0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)\n",
                      linkMask,
                      failures[0],
                      failures[1],
                      failures[2],
                      failures[3],
                      failures[4],
                      failures[5],
                      failures[6]);
        gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_NVLINK_ERROR_FATAL, NULL, 0, 0x0, ALI_TRAINING_FAIL);
    }

    portMemFree(pParams);
    return NV_OK;
}

/*!
 * @brief Registers Callback to the existing dump framework
 */
void
knvlinkDumpCallbackRegister_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NvDebugDump         *pNvd               = GPU_GET_NVD(pGpu);
    NVD_ENGINE_CALLBACK *pEngineCallback    = NULL;

    // Return if pNvd is NULL
    if (pNvd == NULL)
        return;

    // See if already registered (can be if have multiple)
    if ((nvdFindEngine(pGpu, pNvd, NVDUMP_COMPONENT_ENG_NVLINK, &pEngineCallback) == NV_OK) &&
        (pEngineCallback != NULL))
        return;

    // Required to sign up for nvlink callback in Kernel RM in order to dump contents in GSP RM
    nvdEngineSignUp(pGpu,
                    pNvd,
                    NULL,
                    NVDUMP_COMPONENT_ENG_NVLINK,
                    REF_DEF(NVD_ENGINE_FLAGS_PRIORITY, _MED) |
                    REF_DEF(NVD_ENGINE_FLAGS_SOURCE,   _GSP),
                    (void *)pKernelNvlink);
}

static NvU8
_nvlinkLinkCountToRbmMode
(
    NvU8 linkCount
)
{
    NvU8 rbmMode = 0;

    rbmMode = FLD_SET_DRF_NUM(_GPU, _NVLINK, _BW_MODE,
                              GPU_NVLINK_BW_MODE_LINK_COUNT,
                              rbmMode);
    rbmMode = FLD_SET_DRF_NUM(_GPU, _NVLINK, _BW_MODE_LINK_COUNT,
                              linkCount, rbmMode);
    return rbmMode;
}

/*!
 * Check if requested RBM mode is supported by GPU
 */
NvBool
knvlinkIsBwModeSupported_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU8    mode
)
{
    NvU32 i;

    // Legacy BW modes are not supported except _FULL
    if ((DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) != GPU_NVLINK_BW_MODE_LINK_COUNT) &&
        (DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) != GPU_NVLINK_BW_MODE_FULL))
    {
        NV_PRINTF(LEVEL_ERROR, "Legacy BW modes are not supported on this platform.\n");
        return NV_FALSE;
    }

    if (DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) == GPU_NVLINK_BW_MODE_FULL)
    {
        // Requesting full bandwidth on GPU
        return NV_TRUE;
    }

    // Check if requested BW mode link count is supported by GFM
    if (DRF_VAL(_GPU, _NVLINK, _BW_MODE_LINK_COUNT, mode) > pKernelNvlink->maxRbmLinks)
    {
        NV_PRINTF(LEVEL_ERROR, "RBM not supported by GFM. LinkCount: %d; MaxLinkCount: %d\n",
                  DRF_VAL(_GPU, _NVLINK, _BW_MODE_LINK_COUNT, mode),
                  pKernelNvlink->maxRbmLinks);
        return NV_FALSE;
    }

    // Check if requested BW mode link count is supported by HSHUB
    for (i = 0; i < pKernelNvlink->totalRbmModes; i++)
    {
        if (mode == pKernelNvlink->hshubSupportedRbmModesList[i])
        {
            return NV_TRUE;
        }
    }

    NV_PRINTF(LEVEL_ERROR, "RBM requested is not supported. LinkCount: %d\n",
              DRF_VAL(_GPU, _NVLINK, _BW_MODE_LINK_COUNT, mode));
    return NV_FALSE;
}

/*!
 * Retrieve list of HSHUB supported RBM Modes
 */
NV_STATUS
knvlinkGetHshubSupportedRbmModes_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;
    NvU32 i;

    //
    // TODO: Update hardcoded list with list retrieved from HSHUB query rpc.
    // Current supported link counts: 0, 4, 8, 12
    //
    const NvU8 gpuNvlinkHshubSupportedRbmList[] =
    {
        _nvlinkLinkCountToRbmMode(0),
        _nvlinkLinkCountToRbmMode(4),
        _nvlinkLinkCountToRbmMode(8),
        _nvlinkLinkCountToRbmMode(12)
    };

    for (i = 0; i < NV_ARRAY_ELEMENTS(gpuNvlinkHshubSupportedRbmList); i++)
    {
        pKernelNvlink->hshubSupportedRbmModesList[i] = gpuNvlinkHshubSupportedRbmList[i];
    }
    pKernelNvlink->totalRbmModes = NV_ARRAY_ELEMENTS(gpuNvlinkHshubSupportedRbmList);

    return status;
}

/*!
 * Retrieve list of supported BW modes
 */
NV_STATUS
knvlinkGetSupportedBwMode_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS *pParams
)
{
    NvU32 i;

    for (i = 0; i < pKernelNvlink->totalRbmModes; i++)
    {
        // Need to filter HSHUB supported list with maxRbmLinks received from probe response
        if (pKernelNvlink->hshubSupportedRbmModesList[i] <= _nvlinkLinkCountToRbmMode(pKernelNvlink->maxRbmLinks))
        {
            pParams->rbmModesList[i] = pKernelNvlink->hshubSupportedRbmModesList[i];
        }
        else
        {
            // GFM no longer supports mode in supported list. Do not copy to client.
            break;
        }
    }
    pParams->rbmTotalModes = i;

    return NV_OK;
}

/*!
 * @brief   Validates fabric EGM base address.
 *
 * @param[in]  pGpu              OBJGPU pointer
 * @param[in]  pKernelNvlink     KernelNvlink pointer
 * @param[in]  fabricEgmBaseAddr Address to be validated
 *
 * @returns On success, NV_OK.
 *          On failure, returns NV_ERR_XXX.
 */
NV_STATUS
knvlinkValidateFabricEgmBaseAddress_GB100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU64         fabricEgmBaseAddr
)
{
    // SW WAR for HW bug 4851258 requires fabric address to use only bits 46:44
    if ((fabricEgmBaseAddr & ~(((NvU64)0x7ULL) << 44)) != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid EGM fabric address: 0x%llx\n", fabricEgmBaseAddr);
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

/*!
 * @brief callback after FIFO is done initializing and we are ready to handle RC
 */
static NV_STATUS
_knvlinkHandlePostSchedulingEnableCallback_GB100
(
    OBJGPU *pGpu,
    void *pUnusedData
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    // Trigger supressed error replay in gsp-rm
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                            NV2080_CTRL_CMD_INTERNAL_NVLINK_REPLAY_SUPPRESSED_ERRORS,
                            NULL,
                            0));
    return NV_OK;
}

/*!
 * @brief Registers Callback to process supressed link errors on boot
 */
void
knvlinkPostSchedulingEnableCallbackRegister_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_ASSERT_OK(
        kfifoAddSchedulingHandler(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
            _knvlinkHandlePostSchedulingEnableCallback_GB100, NULL, NULL, NULL));
}

/*!
 * @brief Unregisters Callback to process supressed link errors on boot
 */
void
knvlinkPostSchedulingEnableCallbackUnregister_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    kfifoRemoveSchedulingHandler(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
        _knvlinkHandlePostSchedulingEnableCallback_GB100, NULL, NULL, NULL);
}

/**
 * @brief Check if ENCRYPT_EN bit is set
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 * @return  NV_TRUE is ENCRYPT_EN is set, else NV_FALSE
 */

NvBool
knvlinkIsEncryptEnSet_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV2080_CTRL_NVLINK_GET_NVLE_ENCRYPT_EN_INFO_PARAMS params;
    NvU32 status;
    portMemSet(&params, 0, sizeof(params));

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                NV2080_CTRL_CMD_NVLINK_GET_NVLE_ENCRYPT_EN_INFO,
                                (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to execute GSP-RM GPC to get Nvlink Encrypt Enable Info\n");
        return NV_FALSE;
    }

    return params.bEncryptEnSet;
}

/*!
 * @brief  Check if NVLE PDB Property is set
 * 
 * @param[in]  pGpu              OBJGPU pointer
 * @param[in]  pKernelNvlink     KernelNvlink pointer
 *
 */
NvBool
knvlinkIsNvleEnabled_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    if (!(pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED)))
    {
        //
        // Nvlink Encryption PDB PROP is set when 
        // 1. Nvlink Encryption regkey has been enabled AND
        // 2. Encrypt Enable Bit is set by FSP AND
        // 3. Secure Scratch Register Bit is set by FSP after reading the NVLE PRC Knob
        //

        if (knvlinkIsEncryptEnSet_HAL(pGpu, pKernelNvlink) &&
            gpuIsNvleModeEnabledInHw_HAL(pGpu)
            )
        {
            pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED, NV_TRUE);
        }
    }

    return pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED);
}
