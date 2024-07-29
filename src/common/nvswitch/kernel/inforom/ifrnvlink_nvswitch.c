/*
 * SPDX-FileCopyrightText: Copyright (c) 2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "common_nvswitch.h"
#include "error_nvswitch.h"

#include "inforom/inforom_nvswitch.h"

NvlStatus
nvswitch_inforom_nvlink_flush
(
    struct nvswitch_device *device
)
{
    NvlStatus status = NVL_SUCCESS;
    struct inforom *pInforom = device->pInforom;
    PINFOROM_NVLINK_STATE pNvlinkState;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pNvlinkState = pInforom->pNvlinkState;

    if (pNvlinkState != NULL && pNvlinkState->bDirty)
    {
        status = nvswitch_inforom_write_object(device, "NVL",
                                        pNvlinkState->pFmt, pNvlinkState->pNvl,
                                        pNvlinkState->pPackedObject);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "Failed to flush NVL object to InfoROM, rc: %d\n", status);
        }
        else
        {
            pNvlinkState->bDirty = NV_FALSE;
        }
    }

    return status;
}

static void
_inforom_nvlink_get_correctable_error_counts
(
    nvswitch_device                         *device,
    NvU32                                    linkId,
    INFOROM_NVLINK_CORRECTABLE_ERROR_COUNTS *pErrorCounts
)
{
    NvlStatus status;
    NvU32 lane, idx;
    NVSWITCH_NVLINK_GET_COUNTERS_PARAMS p = { 0 };

    ct_assert(NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE__SIZE <=
              INFOROM_NVL_OBJECT_MAX_SUBLINK_WIDTH);

    nvswitch_os_memset(pErrorCounts, 0, sizeof(*pErrorCounts));

    p.linkId = linkId;
    p.counterMask = NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_FLIT
                  | NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_REPLAY
                  | NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_RECOVERY
                  | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_REPLAY
                  | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L0
                  | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L1
                  | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L2
                  | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L3
                  | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L4
                  | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L5
                  | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L6
                  | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L7
                  | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L7;

    status = device->hal.nvswitch_ctrl_get_counters(device, &p);
    if (status != NVL_SUCCESS)
    {
        return;
    }

    pErrorCounts->flitCrc =
        p.nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_FLIT)];

    pErrorCounts->txLinkReplay =
        p.nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_REPLAY)];

    pErrorCounts->rxLinkReplay =
        p.nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_REPLAY)];

    pErrorCounts->linkRecovery =
        p.nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_RECOVERY)];

    for (lane = 0; lane < NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE__SIZE; lane++)
    {
        idx = BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L(lane));
        pErrorCounts->laneCrc[lane] = p.nvlinkCounters[idx];
    }
}

static void
_inforom_nvlink_update_correctable_error_rates
(
    nvswitch_device  *device,
    struct inforom   *pInforom

)
{
    PINFOROM_NVLINK_STATE pNvlinkState = pInforom->pNvlinkState;
    NvU64                 enabledLinkMask;
    NvU32                 linkId, publicId, localLinkIdx;
    NvBool                bDirty = NV_FALSE;
    NvBool                bDirtyTemp;
    INFOROM_NVLINK_CORRECTABLE_ERROR_COUNTS errorCounts = { 0 };

    if (pNvlinkState == NULL)
    {
        return;
    }

    enabledLinkMask = nvswitch_get_enabled_link_mask(device);

    FOR_EACH_INDEX_IN_MASK(64, linkId, enabledLinkMask)
    {
        if (device->hal.nvswitch_get_link_public_id(device, linkId, &publicId) != NVL_SUCCESS)
        {
            continue;
        }

        if (device->hal.nvswitch_get_link_local_idx(device, linkId, &localLinkIdx) != NVL_SUCCESS)
        {
            continue;
        }

        _inforom_nvlink_get_correctable_error_counts(device, linkId, &errorCounts);

        if (device->hal.nvswitch_inforom_nvl_update_link_correctable_error_info(device,
                pNvlinkState->pNvl, &pNvlinkState->correctableErrorRateState, linkId,
                publicId, localLinkIdx, &errorCounts, &bDirtyTemp) != NVL_SUCCESS)
        {
            continue;
        }

        bDirty |= bDirtyTemp;
    }
    FOR_EACH_INDEX_IN_MASK_END;

    pNvlinkState->bDirty |= bDirty;
}

static void _nvswitch_nvlink_1hz_callback
(
    nvswitch_device *device
)
{
    struct inforom *pInforom = device->pInforom;

    if ((pInforom == NULL) || (pInforom->pNvlinkState == NULL) ||
        pInforom->pNvlinkState->bCallbackPending)
    {
        return;
    }

    pInforom->pNvlinkState->bCallbackPending = NV_TRUE;
    _inforom_nvlink_update_correctable_error_rates(device, pInforom);
    pInforom->pNvlinkState->bCallbackPending = NV_FALSE;
}

static void
_inforom_nvlink_start_correctable_error_recording
(
    nvswitch_device *device,
    struct inforom  *pInforom
)
{
    PINFOROM_NVLINK_STATE pNvlinkState = pInforom->pNvlinkState;

    if (pNvlinkState == NULL)
    {
        return;
    }

    if (pNvlinkState->bDisableCorrectableErrorLogging)
    {

        NVSWITCH_PRINT(device, INFO,
                "%s: Correctable error recording disabled by regkey or unsupported\n",
                __FUNCTION__);
        return;
    }

    pNvlinkState->bCallbackPending = NV_FALSE;

    if (!nvswitch_is_tnvl_mode_enabled(device))
    {
        nvswitch_task_create(device, &_nvswitch_nvlink_1hz_callback,
                             NVSWITCH_INTERVAL_1SEC_IN_NS, 0);
    }
    else
    {
        NVSWITCH_PRINT(device, INFO, "Skipping NVLINK heartbeat task when TNVL is enabled\n");
    }
}

NvlStatus
nvswitch_inforom_nvlink_load
(
    nvswitch_device *device
)
{
    NvlStatus status;
    NvU8 version = 0;
    NvU8 subversion = 0;
    INFOROM_NVLINK_STATE *pNvlinkState = NULL;
    struct inforom *pInforom = device->pInforom;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    status = nvswitch_inforom_get_object_version_info(device, "NVL", &version,
                                                    &subversion);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, WARN, "no NVL object found, rc:%d\n", status);
        return NVL_SUCCESS;
    }

    if (!INFOROM_OBJECT_SUBVERSION_SUPPORTS_NVSWITCH(subversion))
    {
        NVSWITCH_PRINT(device, WARN, "NVL v%u.%u not supported\n",
                    version, subversion);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    NVSWITCH_PRINT(device, INFO, "NVL v%u.%u found\n", version, subversion);

    pNvlinkState = nvswitch_os_malloc(sizeof(INFOROM_NVLINK_STATE));
    if (pNvlinkState == NULL)
    {
        return -NVL_NO_MEM;
    }
    nvswitch_os_memset(pNvlinkState, 0, sizeof(INFOROM_NVLINK_STATE));

    pNvlinkState->bDirty = NV_FALSE;
    pNvlinkState->bDisableFatalErrorLogging = NV_FALSE;
    pNvlinkState->bDisableCorrectableErrorLogging = NV_TRUE;

    status = device->hal.nvswitch_inforom_nvl_setup_nvlink_state(device, pNvlinkState, version);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to set up NVL object, rc:%d\n", status);
        goto nvswitch_inforom_nvlink_version_fail;
    }

    status = nvswitch_inforom_read_object(device, "NVL", pNvlinkState->pFmt,
                                        pNvlinkState->pPackedObject,
                                        pNvlinkState->pNvl);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to read NVL object, rc:%d\n", status);
        goto nvswitch_inforom_read_fail;
    }

    status = nvswitch_inforom_add_object(pInforom, &pNvlinkState->pNvl->header);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to cache NVL object header, rc:%d\n",
                    status);
        goto nvswitch_inforom_read_fail;
    }

    pInforom->pNvlinkState = pNvlinkState;

    _inforom_nvlink_start_correctable_error_recording(device, pInforom);

    return NVL_SUCCESS;

nvswitch_inforom_read_fail:
    nvswitch_os_free(pNvlinkState->pPackedObject);
    nvswitch_os_free(pNvlinkState->pNvl);
nvswitch_inforom_nvlink_version_fail:
    nvswitch_os_free(pNvlinkState);

    return status;
}

void
nvswitch_inforom_nvlink_unload
(
    nvswitch_device *device
)
{
    INFOROM_NVLINK_STATE *pNvlinkState;
    struct inforom *pInforom = device->pInforom;

    if (pInforom == NULL)
    {
        return;
    }

    pNvlinkState = pInforom->pNvlinkState;
    if (pNvlinkState == NULL)
    {
        return;
    }

    if (nvswitch_inforom_nvlink_flush(device) != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to flush NVL object on object unload\n");
    }

    nvswitch_os_free(pNvlinkState->pPackedObject);
    nvswitch_os_free(pNvlinkState->pNvl);
    nvswitch_os_free(pNvlinkState);
    pInforom->pNvlinkState = NULL;
}

NvlStatus
nvswitch_inforom_nvlink_log_error_event
(
    nvswitch_device            *device,
    void                       *error_event
)
{
    NvlStatus status;
    NvBool bDirty = NV_FALSE;
    struct inforom *pInforom = device->pInforom;
    INFOROM_NVLINK_STATE *pNvlinkState;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pNvlinkState = pInforom->pNvlinkState;
    if (pNvlinkState == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    status = device->hal.nvswitch_inforom_nvl_log_error_event(device,
                                                        pNvlinkState->pNvl,
                                                        (INFOROM_NVLINK_ERROR_EVENT *)error_event,
                                                        &bDirty);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to log error to inforom, rc:%d\n",
                    status);
    }

    pNvlinkState->bDirty |= bDirty;

    return status;
}

NvlStatus
nvswitch_inforom_nvlink_get_max_correctable_error_rate
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_MAX_CORRECTABLE_ERROR_RATES_PARAMS *params
)
{
    struct inforom *pInforom = device->pInforom;

    if ((pInforom == NULL) || (pInforom->pNvlinkState == NULL))
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return device->hal.nvswitch_inforom_nvl_get_max_correctable_error_rate(device, params);
}

NvlStatus
nvswitch_inforom_nvlink_get_errors
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_ERROR_COUNTS_PARAMS *params
)
{
    struct inforom *pInforom = device->pInforom;

    if ((pInforom == NULL) || (pInforom->pNvlinkState == NULL))
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return device->hal.nvswitch_inforom_nvl_get_errors(device, params);
}

NvlStatus nvswitch_inforom_nvlink_setL1Threshold
(
    nvswitch_device *device,
    NvU32 word1,
    NvU32 word2
)
{
    struct inforom *pInforom = device->pInforom;
    INFOROM_NVLINK_STATE *pNvlinkState;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pNvlinkState = pInforom->pNvlinkState;
    if (pNvlinkState == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return device->hal.nvswitch_inforom_nvl_setL1Threshold(device,
                                                        pNvlinkState->pNvl,
                                                        word1,
                                                        word2);
}

NvlStatus nvswitch_inforom_nvlink_getL1Threshold
(
    nvswitch_device *device,
    NvU32 *word1,
    NvU32 *word2
)
{
    struct inforom *pInforom = device->pInforom;
    INFOROM_NVLINK_STATE *pNvlinkState;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pNvlinkState = pInforom->pNvlinkState;
    if (pNvlinkState == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return device->hal.nvswitch_inforom_nvl_getL1Threshold(device,
                                                        pNvlinkState->pNvl,
                                                        word1,
                                                        word2);
}

