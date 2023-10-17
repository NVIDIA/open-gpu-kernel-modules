/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "ls10/ls10.h"
#include "ls10/inforom_ls10.h"
#include "inforom/ifrstruct.h"
#include "soe/soeififr.h"
#include "rmsoecmdif.h"
#include "flcn/flcn_nvswitch.h"
#include "rmflcncmdif_nvswitch.h"
#include "inforom/inforom_nvl_v3_nvswitch.h"
#include "inforom/inforom_nvl_v4_nvswitch.h"

NvlStatus
nvswitch_inforom_nvl_log_error_event_ls10
(
    nvswitch_device *device,
    void *pNvlGeneric,
    void *pNvlErrorEvent,
    NvBool *bDirty
)
{
    NvlStatus status;
    INFOROM_NVL_OBJECT_V4S *pNvlObject = &((PINFOROM_NVL_OBJECT)pNvlGeneric)->v4s;
    INFOROM_NVLINK_ERROR_EVENT *pErrorEvent = (INFOROM_NVLINK_ERROR_EVENT *)pNvlErrorEvent;
    INFOROM_NVL_OBJECT_V3_ERROR_ENTRY *pErrorEntry;
    NvU32 i;
    NvU32 sec;
    NvU8  header = 0;
    NvU16 metadata = 0;
    NvU8  errorSubtype;
    NvU64 accumTotalCount;
    INFOROM_NVL_ERROR_BLOCK_TYPE blockType;

    if (pErrorEvent->nvliptInstance > INFOROM_NVL_OBJECT_V3_NVLIPT_INSTANCE_MAX)
    {
        NVSWITCH_PRINT(device, ERROR,
            "object cannot log data for more than %u NVLIPTs (NVLIPT = %u requested)\n",
            INFOROM_NVL_OBJECT_V3_NVLIPT_INSTANCE_MAX, pErrorEvent->nvliptInstance);
        return -NVL_BAD_ARGS;
    }

    if (pErrorEvent->localLinkIdx > INFOROM_NVL_OBJECT_V3_BLOCK_ID_MAX)
    {
        NVSWITCH_PRINT(device, ERROR,
            "object cannot log data for more than %u internal links (internal link = %u requested)\n",
            INFOROM_NVL_OBJECT_V3_BLOCK_ID_MAX, pErrorEvent->localLinkIdx);
        return -NVL_BAD_ARGS;
    }

    sec = (NvU32) (nvswitch_os_get_platform_time_epoch() / NVSWITCH_INTERVAL_1SEC_IN_NS);

    status = inforom_nvl_v3_map_error(pErrorEvent->error, &header, &metadata,
                                   &errorSubtype, &blockType);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    metadata = FLD_SET_DRF_NUM(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA,
                               _NVLIPT_INSTANCE_ID, pErrorEvent->nvliptInstance, metadata);
    if (blockType == INFOROM_NVL_ERROR_BLOCK_TYPE_DL)
    {
        metadata = FLD_SET_DRF_NUM(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID,
                NV_INFOROM_NVL_OBJECT_V3_ERROR_METADATA_BLOCK_ID_DL(pErrorEvent->localLinkIdx),
                metadata);
    }
    else if (blockType == INFOROM_NVL_ERROR_BLOCK_TYPE_TLC)
    {
        metadata = FLD_SET_DRF_NUM(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _BLOCK_ID,
                NV_INFOROM_NVL_OBJECT_V3_ERROR_METADATA_BLOCK_ID_TLC(pErrorEvent->localLinkIdx),
                metadata);
    }
    else if (blockType == INFOROM_NVL_ERROR_BLOCK_TYPE_NVLIPT)
    {
        metadata = FLD_SET_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA,
                               _BLOCK_ID, _NVLIPT, metadata);
        status = inforom_nvl_v3_encode_nvlipt_error_subtype(pErrorEvent->localLinkIdx,
                                           &errorSubtype);
        if (status != NVL_SUCCESS)
        {
            return status;
        }
    }

    for (i = 0; i < INFOROM_NVL_OBJECT_V4S_NUM_ERROR_ENTRIES; i++)
    {
        pErrorEntry = &pNvlObject->errorLog[i];

        if ((pErrorEntry->header == INFOROM_NVL_ERROR_TYPE_INVALID) ||
            ((pErrorEntry->metadata == metadata) &&
                (pErrorEntry->errorSubtype == errorSubtype)))
        {
            break;
        }
    }

    if (i >= INFOROM_NVL_OBJECT_V4S_NUM_ERROR_ENTRIES)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: NVL error log is full -- unable to log error\n",
                       __FUNCTION__);
        return -NVL_ERR_INVALID_STATE;
    }

    if (pErrorEntry->header == INFOROM_NVL_ERROR_TYPE_INVALID)
    {
        pErrorEntry->header       = header;
        pErrorEntry->metadata     = metadata;
        pErrorEntry->errorSubtype = errorSubtype;
    }

    if (pErrorEntry->header == INFOROM_NVL_ERROR_TYPE_ACCUM)
    {
        accumTotalCount = NvU64_ALIGN32_VAL(&pErrorEntry->data.accum.totalCount);
        if (accumTotalCount != NV_U64_MAX)
        {
            if (pErrorEvent->count > NV_U64_MAX - accumTotalCount)
            {
                accumTotalCount = NV_U64_MAX;
            }
            else
            {
                accumTotalCount += pErrorEvent->count;
            }

            NvU64_ALIGN32_PACK(&pErrorEntry->data.accum.totalCount, &accumTotalCount);
            if (sec < pErrorEntry->data.accum.lastUpdated)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: System clock reporting earlier time than error timestamp\n",
                    __FUNCTION__);
            }
            pErrorEntry->data.accum.lastUpdated = sec;
            *bDirty = NV_TRUE;
        }
    }
    else if (pErrorEntry->header == INFOROM_NVL_ERROR_TYPE_COUNT)
    {
        if (pErrorEntry->data.event.totalCount != NV_U32_MAX)
        {
            pErrorEntry->data.event.totalCount++;
            if (sec < pErrorEntry->data.event.lastError)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: System clock reporting earlier time than error timestamp\n",
                    __FUNCTION__);
            }
            else
            {
                pErrorEntry->data.event.avgEventDelta =
                    (pErrorEntry->data.event.avgEventDelta + sec -
                         pErrorEntry->data.event.lastError) >> 1;
            }
            pErrorEntry->data.event.lastError = sec;
            *bDirty = NV_TRUE;
        }
    }
    else
    {
        return -NVL_ERR_INVALID_STATE;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_inforom_nvl_get_max_correctable_error_rate_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_MAX_CORRECTABLE_ERROR_RATES_PARAMS *params
)
{
    
    struct inforom *pInforom = device->pInforom;
    INFOROM_NVLINK_STATE *pNvlinkState;
    NvU8 linkID = params->linkId;

    if (linkID >= NVSWITCH_NUM_LINKS_LS10)
    {
        return -NVL_BAD_ARGS;
    }
    
    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pNvlinkState = pInforom->pNvlinkState;
    if (pNvlinkState == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    nvswitch_os_memset(params, 0, sizeof(NVSWITCH_GET_NVLINK_MAX_CORRECTABLE_ERROR_RATES_PARAMS));
    params->linkId = linkID;

    nvswitch_os_memcpy(&params->dailyMaxCorrectableErrorRates, &pNvlinkState->pNvl->v4s.maxCorrectableErrorRates.dailyMaxCorrectableErrorRates[0][linkID],
                       sizeof(params->dailyMaxCorrectableErrorRates));

    nvswitch_os_memcpy(&params->monthlyMaxCorrectableErrorRates, &pNvlinkState->pNvl->v4s.maxCorrectableErrorRates.monthlyMaxCorrectableErrorRates[0][linkID],
                       sizeof(params->monthlyMaxCorrectableErrorRates));

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_inforom_nvl_get_errors_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_ERROR_COUNTS_PARAMS *params
)
{
    struct inforom *pInforom = device->pInforom;
    INFOROM_NVLINK_STATE *pNvlinkState;
    NvU32 maxReadSize = sizeof(params->errorLog)/sizeof(NVSWITCH_NVLINK_ERROR_ENTRY);
    NvU32 errorLeftCount = 0, errorReadCount = 0, errIndx = 0;
    NvU32 errorStart = params->errorIndex;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pNvlinkState = pInforom->pNvlinkState;
    if (pNvlinkState == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (errorStart >= INFOROM_NVL_OBJECT_V4S_NUM_ERROR_ENTRIES)
    {
        return -NVL_BAD_ARGS;
    }

    nvswitch_os_memset(params->errorLog, 0, sizeof(params->errorLog));

    while (((errorStart + errorLeftCount) < INFOROM_NVL_OBJECT_V4S_NUM_ERROR_ENTRIES) &&
           (pNvlinkState->pNvl->v4s.errorLog[errorStart + errorLeftCount].header != INFOROM_NVL_ERROR_TYPE_INVALID))
    {
        errorLeftCount++;
    }

    if (errorLeftCount > maxReadSize)
    {
        errorReadCount = maxReadSize;
    }
    else
    {
        errorReadCount = errorLeftCount;
    }

    params->errorIndex = errorStart + errorReadCount;
    params->errorCount = errorReadCount;

    if (errorReadCount > 0)
    {
        for (errIndx = 0; errIndx < errorReadCount; errIndx++)
        {
            if (inforom_nvl_v3_map_error_to_userspace_error(device,
                                                            &pNvlinkState->pNvl->v4s.errorLog[errorStart+errIndx],
                                                            &params->errorLog[errIndx]) != NVL_SUCCESS)
            {
                return -NVL_ERR_NOT_SUPPORTED;
            }
        }
    }

    return NVL_SUCCESS;
}

NvlStatus nvswitch_inforom_nvl_update_link_correctable_error_info_ls10
(
    nvswitch_device *device,
    void *pNvlGeneric,
    void *pData,
    NvU8 linkId,
    NvU8 nvliptInstance,
    NvU8 localLinkIdx,
    void *pNvlErrorCounts,
    NvBool *bDirty
)
{
    INFOROM_NVL_OBJECT_V4S *pNvlObject = &((PINFOROM_NVL_OBJECT)pNvlGeneric)->v4s;
    INFOROM_NVL_CORRECTABLE_ERROR_RATE_STATE_V4S *pState =
                                        &((INFOROM_NVL_CORRECTABLE_ERROR_RATE_STATE *)pData)->v4s;
    INFOROM_NVLINK_CORRECTABLE_ERROR_COUNTS *pErrorCounts =
                                            (INFOROM_NVLINK_CORRECTABLE_ERROR_COUNTS *)pNvlErrorCounts;

    NvU32 i;
    NvU32 sec;
    NvU32 day, month, currentEntryDay, currentEntryMonth;
    INFOROM_NVL_OBJECT_V3_CORRECTABLE_ERROR_RATE *pErrorRate;
    INFOROM_NVL_OBJECT_V3_CORRECTABLE_ERROR_RATE *pOldestErrorRate = NULL;
    INFOROM_NVL_OBJECT_V4S_MAX_CORRECTABLE_ERROR_RATES *pCorrErrorRates;
    NvBool bUpdated = NV_FALSE;
    INFOROM_NVLINK_ERROR_EVENT errorEvent;
    NvU32 currentFlitCrcRate;
    NvU32 *pCurrentLaneCrcRates;

    if (bDirty == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    *bDirty = NV_FALSE;

    if (linkId >= INFOROM_NVL_OBJECT_V4S_NUM_LINKS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "object does not store data for more than %u links (linkId = %u requested)\n",
             INFOROM_NVL_OBJECT_V4S_NUM_LINKS, linkId);
        return -NVL_BAD_ARGS;
    }

    if (nvliptInstance > INFOROM_NVL_OBJECT_V3_NVLIPT_INSTANCE_MAX)
    {
        NVSWITCH_PRINT(device, ERROR,
            "object cannot log data for more than %u NVLIPTs (NVLIPT = %u requested)\n",
            INFOROM_NVL_OBJECT_V3_NVLIPT_INSTANCE_MAX, nvliptInstance);
        return -NVL_BAD_ARGS;
    }

    if (localLinkIdx > INFOROM_NVL_OBJECT_V3_BLOCK_ID_MAX)
    {
        NVSWITCH_PRINT(device, ERROR,
            "object cannot log data for more than %u internal links (internal link = %u requested)\n",
            INFOROM_NVL_OBJECT_V3_BLOCK_ID_MAX, localLinkIdx);
        return -NVL_BAD_ARGS;
    }

    sec = (NvU32) (nvswitch_os_get_platform_time_epoch() / NVSWITCH_INTERVAL_1SEC_IN_NS);
    inforom_nvl_v3_seconds_to_day_and_month(sec, &day, &month);

    inforom_nvl_v4_update_correctable_error_rates(pState, linkId, pErrorCounts);
    currentFlitCrcRate   = pState->errorsPerMinute[linkId].flitCrc;
    pCurrentLaneCrcRates = pState->errorsPerMinute[linkId].laneCrc;
    pCorrErrorRates      = &pNvlObject->maxCorrectableErrorRates;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pCorrErrorRates->dailyMaxCorrectableErrorRates); i++)
    {
        pErrorRate = &pCorrErrorRates->dailyMaxCorrectableErrorRates[i][linkId];
        inforom_nvl_v3_seconds_to_day_and_month(pErrorRate->lastUpdated, &currentEntryDay,
                                                &currentEntryMonth);

        if ((pErrorRate->lastUpdated == 0) || (currentEntryDay == day))
        {
            if (inforom_nvl_v3_should_replace_error_rate_entry(pErrorRate,
                                                             currentFlitCrcRate,
                                                             pCurrentLaneCrcRates))
            {
                inforom_nvl_v3_update_error_rate_entry(pErrorRate, sec,
                                                       currentFlitCrcRate,
                                                       pCurrentLaneCrcRates);
                bUpdated = NV_TRUE;
            }
            pOldestErrorRate = NULL;
            break;
        }
        else if ((pOldestErrorRate == NULL) ||
                 (pErrorRate->lastUpdated < pOldestErrorRate->lastUpdated))
        {
            pOldestErrorRate = pErrorRate;
        }
    }

    if (pOldestErrorRate != NULL)
    {
        inforom_nvl_v3_update_error_rate_entry(pOldestErrorRate, sec,
                                               currentFlitCrcRate,
                                               pCurrentLaneCrcRates);
        bUpdated = NV_TRUE;
    }

    for (i = 0; i < NV_ARRAY_ELEMENTS(pCorrErrorRates->monthlyMaxCorrectableErrorRates); i++)
    {
        pErrorRate = &pCorrErrorRates->monthlyMaxCorrectableErrorRates[i][linkId];
        inforom_nvl_v3_seconds_to_day_and_month(pErrorRate->lastUpdated, &currentEntryDay,
                                                &currentEntryMonth);

        if ((pErrorRate->lastUpdated == 0) || (currentEntryMonth == month))
        {
            if (inforom_nvl_v3_should_replace_error_rate_entry(pErrorRate,
                                                             currentFlitCrcRate,
                                                             pCurrentLaneCrcRates))
            {
                inforom_nvl_v3_update_error_rate_entry(pErrorRate, sec,
                                                       currentFlitCrcRate,
                                                       pCurrentLaneCrcRates);
                bUpdated = NV_TRUE;
            }
            pOldestErrorRate = NULL;
            break;
        }
        else if ((pOldestErrorRate == NULL) ||
                 (pErrorRate->lastUpdated < pOldestErrorRate->lastUpdated))
        {
            pOldestErrorRate = pErrorRate;
        }
    }

    if (pOldestErrorRate != NULL)
    {
        inforom_nvl_v3_update_error_rate_entry(pOldestErrorRate, sec,
                                               currentFlitCrcRate,
                                               pCurrentLaneCrcRates);
        bUpdated = NV_TRUE;
    }

    *bDirty = bUpdated;

    // Update aggregate error counts for each correctable error

    errorEvent.nvliptInstance = nvliptInstance;
    errorEvent.localLinkIdx   = localLinkIdx;

    if (pErrorCounts->flitCrc > 0)
    {
        errorEvent.error = INFOROM_NVLINK_DL_RX_FLIT_CRC_CORR;
        errorEvent.count = pErrorCounts->flitCrc;
        nvswitch_inforom_nvl_log_error_event_ls10(device,
                pNvlGeneric, &errorEvent, &bUpdated);
        *bDirty |= bUpdated;
    }

    if (pErrorCounts->rxLinkReplay > 0)
    {
        errorEvent.error = INFOROM_NVLINK_DL_RX_LINK_REPLAY_EVENTS_CORR;
        errorEvent.count = pErrorCounts->rxLinkReplay;
        bUpdated = NV_FALSE;
        nvswitch_inforom_nvl_log_error_event_ls10(device,
                pNvlGeneric, &errorEvent, &bUpdated);
        *bDirty |= bUpdated;
    }

    if (pErrorCounts->txLinkReplay > 0)
    {
        errorEvent.error = INFOROM_NVLINK_DL_TX_LINK_REPLAY_EVENTS_CORR;
        errorEvent.count = pErrorCounts->txLinkReplay;
        bUpdated = NV_FALSE;
        nvswitch_inforom_nvl_log_error_event_ls10(device,
                pNvlGeneric, &errorEvent, &bUpdated);
        *bDirty |= bUpdated;
    }

    if (pErrorCounts->linkRecovery > 0)
    {
        errorEvent.error = INFOROM_NVLINK_DL_LINK_RECOVERY_EVENTS_CORR;
        errorEvent.count = pErrorCounts->linkRecovery;
        bUpdated = NV_FALSE;
        nvswitch_inforom_nvl_log_error_event_ls10(device,
                pNvlGeneric, &errorEvent, &bUpdated);
        *bDirty |= bUpdated;
    }

    for (i = 0; i < 4; i++)
    {
        if (pErrorCounts->laneCrc[i] == 0)
        {
            continue;
        }

        errorEvent.error = INFOROM_NVLINK_DL_RX_LANE0_CRC_CORR + i;
        errorEvent.count = pErrorCounts->laneCrc[i];
        bUpdated = NV_FALSE;
        nvswitch_inforom_nvl_log_error_event_ls10(device,
                pNvlGeneric, &errorEvent, &bUpdated);
        *bDirty |= bUpdated;
    }

    return NVL_SUCCESS;
}

NvlStatus nvswitch_inforom_nvl_setL1Threshold_ls10
(
    nvswitch_device *device,
    void *pNvlGeneric,
    NvU32 word1,
    NvU32 word2
)
{
    INFOROM_NVL_OBJECT_V4S *pNvlObject = &((PINFOROM_NVL_OBJECT)pNvlGeneric)->v4s;

    pNvlObject->l1ThresholdData.word1 = word1;
    pNvlObject->l1ThresholdData.word2 = word2;

    return NVL_SUCCESS;
}

NvlStatus nvswitch_inforom_nvl_getL1Threshold_ls10
(
    nvswitch_device *device,
    void *pNvlGeneric,
    NvU32 *word1,
    NvU32 *word2
)
{
    INFOROM_NVL_OBJECT_V4S *pNvlObject = &((PINFOROM_NVL_OBJECT)pNvlGeneric)->v4s;

    *word1 = pNvlObject->l1ThresholdData.word1;
    *word2 = pNvlObject->l1ThresholdData.word2;

    return NVL_SUCCESS;
}

NvlStatus nvswitch_inforom_nvl_setup_nvlink_state_ls10
(
    nvswitch_device *device,
    INFOROM_NVLINK_STATE *pNvlinkState,
    NvU8 version
)
{
    if (version != 4)
    {
        NVSWITCH_PRINT(device, WARN, "NVL v%u not supported\n", version);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pNvlinkState->pFmt = INFOROM_NVL_OBJECT_V4S_FMT;
    pNvlinkState->pPackedObject = nvswitch_os_malloc(INFOROM_NVL_OBJECT_V4S_PACKED_SIZE);
    if (pNvlinkState->pPackedObject == NULL)
    {
       return -NVL_NO_MEM;
    }

    pNvlinkState->pNvl = nvswitch_os_malloc(sizeof(INFOROM_NVL_OBJECT));
    if (pNvlinkState->pNvl == NULL)
    {
       nvswitch_os_free(pNvlinkState->pPackedObject);
       return -NVL_NO_MEM;
    }

    pNvlinkState->bDisableCorrectableErrorLogging = NV_FALSE;

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_oms_inforom_flush_ls10
(
    nvswitch_device *device
)
{
    return -NVL_ERR_NOT_IMPLEMENTED;
}

void
nvswitch_initialize_oms_state_ls10
(
    nvswitch_device *device,
    INFOROM_OMS_STATE *pOmsState
)
{
    return;
}

NvBool
nvswitch_oms_get_device_disable_ls10
(
    INFOROM_OMS_STATE *pOmsState
)
{
    return NV_FALSE;
}

void
nvswitch_oms_set_device_disable_ls10
(
    INFOROM_OMS_STATE *pOmsState,
    NvBool bForceDeviceDisable
)
{
    return;
}

void
nvswitch_inforom_ecc_get_total_errors_ls10
(
    nvswitch_device    *device,
    INFOROM_ECC_OBJECT *pEccGeneric,
    NvU64              *pCorrectedTotal,
    NvU64              *pUncorrectedTotal
)
{
    return;
}

NvlStatus
nvswitch_inforom_load_obd_ls10
(
    nvswitch_device *device
)
{
    struct inforom *pInforom = device->pInforom;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return nvswitch_inforom_load_object(device, pInforom, "OBD",
                                        INFOROM_OBD_OBJECT_V2_XX_FMT,
                                        pInforom->OBD.packedObject.v2,
                                        &pInforom->OBD.object.v2);
}

NvlStatus
nvswitch_bbx_add_sxid_ls10
(
    nvswitch_device *device,
    NvU32            exceptionType,
    NvU32            data0,
    NvU32            data1,
    NvU32            data2
)
{
    NvlStatus                  status;
    FLCN                      *pFlcn;
    RM_FLCN_CMD_SOE            bbxCmd;
    NvU32                      cmdSeqDesc;
    NVSWITCH_TIMEOUT           timeout;

    if (!nvswitch_is_inforom_supported_ls10(device))
    {
        NVSWITCH_PRINT(device, INFO, "InfoROM is not supported, skipping\n");
        return NVL_SUCCESS;
    }

    // Avoid logging SOE related SXIDs to prevent recursive errors
    if (exceptionType > NVSWITCH_ERR_HW_SOE && exceptionType < NVSWITCH_ERR_HW_SOE_LAST)
    {
        NVSWITCH_PRINT(device, INFO, "Not logging SXID: %d to InfoROM\n", exceptionType);
        return NVL_SUCCESS;
    }

    pFlcn = device->pSoe->pFlcn;
    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);

    nvswitch_os_memset(&bbxCmd, 0, sizeof(bbxCmd));
    bbxCmd.hdr.unitId = RM_SOE_UNIT_IFR;
    bbxCmd.hdr.size = sizeof(bbxCmd);
    bbxCmd.cmd.ifr.cmdType = RM_SOE_IFR_BBX_SXID_ADD;
    bbxCmd.cmd.ifr.bbxSxidAdd.exceptionType = exceptionType;
    bbxCmd.cmd.ifr.bbxSxidAdd.data[0] = data0;
    bbxCmd.cmd.ifr.bbxSxidAdd.data[1] = data1;
    bbxCmd.cmd.ifr.bbxSxidAdd.data[2] = data2;

    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                (PRM_FLCN_CMD)&bbxCmd,
                                NULL,   // pMsg
                                NULL,   // pPayload
                                SOE_RM_CMDQ_LOG_ID,
                                &cmdSeqDesc,
                                &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: BBX cmd %d failed. rc:%d\n",
                        __FUNCTION__, bbxCmd.cmd.ifr.cmdType, status);
    }

    return status;
}

NvlStatus
nvswitch_bbx_unload_ls10
(
    nvswitch_device *device
)
{
    NvlStatus                  status;
    FLCN                      *pFlcn;
    RM_FLCN_CMD_SOE            bbxCmd;
    NvU32                      cmdSeqDesc;
    NVSWITCH_TIMEOUT           timeout;

    pFlcn = device->pSoe->pFlcn;
    nvswitch_timeout_create(NVSWITCH_INTERVAL_4SEC_IN_NS, &timeout);

    nvswitch_os_memset(&bbxCmd, 0, sizeof(bbxCmd));
    bbxCmd.hdr.unitId = RM_SOE_UNIT_IFR;
    bbxCmd.hdr.size = sizeof(bbxCmd);
    bbxCmd.cmd.ifr.cmdType = RM_SOE_IFR_BBX_SHUTDOWN;

    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                (PRM_FLCN_CMD)&bbxCmd,
                                NULL,   // pMsg
                                NULL,   // pPayload
                                SOE_RM_CMDQ_LOG_ID,
                                &cmdSeqDesc,
                                &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: BBX cmd %d failed. rc:%d\n",
                        __FUNCTION__, bbxCmd.cmd.ifr.cmdType, status);
    }

    return status;
}

NvlStatus
nvswitch_bbx_load_ls10
(
    nvswitch_device *device,
    NvU64 time_ns,
    NvU8 osType,
    NvU32 osVersion
)
{
    NvlStatus                  status;
    FLCN                      *pFlcn;
    RM_FLCN_CMD_SOE            bbxCmd;
    NvU32                      cmdSeqDesc = 0;
    NVSWITCH_TIMEOUT           timeout;

    pFlcn = device->pSoe->pFlcn;
    nvswitch_timeout_create(NVSWITCH_INTERVAL_4SEC_IN_NS, &timeout);

    nvswitch_os_memset(&bbxCmd, 0, sizeof(bbxCmd));
    bbxCmd.hdr.unitId = RM_SOE_UNIT_IFR;
    bbxCmd.hdr.size = sizeof(bbxCmd);
    bbxCmd.cmd.ifr.cmdType = RM_SOE_IFR_BBX_INITIALIZE;
    bbxCmd.cmd.ifr.bbxInit.osType = osType;
    bbxCmd.cmd.ifr.bbxInit.osVersion = osVersion;
    RM_FLCN_U64_PACK(&bbxCmd.cmd.ifr.bbxInit.time, &time_ns);

    NVSWITCH_PRINT(device, INFO, "RM_SOE_IFR_BBX_INITIALIZE called, time_ns=%llu \n", time_ns);

    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                (PRM_FLCN_CMD)&bbxCmd,
                                NULL,   // pMsg
                                NULL,   // pPayload
                                SOE_RM_CMDQ_LOG_ID,
                                &cmdSeqDesc,
                                &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: BBX cmd %d failed. rc:%d\n",
                        __FUNCTION__, bbxCmd.cmd.ifr.cmdType, status);
    }

    return status;
}

NvlStatus
nvswitch_bbx_get_sxid_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_SXIDS_PARAMS *params
)
{
    NvlStatus status;
    void *pDmaBuf;
    NvU64 dmaHandle;
    FLCN *pFlcn;
    RM_FLCN_CMD_SOE bbxCmd;
    NvU32 cmdSeqDesc;
    NVSWITCH_TIMEOUT timeout;
    NvU32 transferSize;
    RM_SOE_BBX_GET_SXID_DATA bbxSxidData;
    NvU32 sxidIdx;

    if (!nvswitch_is_inforom_supported_ls10(device))
    {
        NVSWITCH_PRINT(device, INFO, "%s: InfoROM is not supported\n", __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (params == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    transferSize = sizeof(bbxSxidData);
    status = nvswitch_os_alloc_contig_memory(device->os_handle, &pDmaBuf, transferSize,
                                            (device->dma_addr_width == 32));
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to allocate contig memory. rc:%d\n", __FUNCTION__, status);
        return status;
    }

    status = nvswitch_os_map_dma_region(device->os_handle, pDmaBuf, &dmaHandle,
                                        transferSize, NVSWITCH_DMA_DIR_TO_SYSMEM);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to map DMA region. rc:%d\n", __FUNCTION__, status);
        goto nvswitch_bbx_get_sxid_ls10_free_and_exit;
    }

    nvswitch_os_memset(pDmaBuf, 0, transferSize);

    pFlcn = device->pSoe->pFlcn;
    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);

    nvswitch_os_memset(&bbxCmd, 0, sizeof(bbxCmd));
    bbxCmd.hdr.unitId = RM_SOE_UNIT_IFR;
    bbxCmd.hdr.size = sizeof(bbxCmd);
    bbxCmd.cmd.ifr.cmdType = RM_SOE_IFR_BBX_SXID_GET;
    bbxCmd.cmd.ifr.bbxSxidGet.sizeInBytes = transferSize;
    RM_FLCN_U64_PACK(&bbxCmd.cmd.ifr.bbxSxidGet.dmaHandle, &dmaHandle);

    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                (PRM_FLCN_CMD)&bbxCmd,
                                NULL,   // pMsg
                                NULL,   // pPayload
                                SOE_RM_CMDQ_LOG_ID,
                                &cmdSeqDesc,
                                &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: BBX cmd %d failed. rc:%d\n",
                        __FUNCTION__, bbxCmd.cmd.ifr.cmdType, status);
        goto nvswitch_bbx_get_sxid_ls10_unmap_and_exit;
    }

    status = nvswitch_os_sync_dma_region_for_cpu(device->os_handle, dmaHandle,
                                                        transferSize,
                                                        NVSWITCH_DMA_DIR_TO_SYSMEM);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to sync DMA region. rc:%d\n", __FUNCTION__, status);
        goto nvswitch_bbx_get_sxid_ls10_unmap_and_exit;
    }

    nvswitch_os_memcpy((NvU8 *)&bbxSxidData, (NvU8 *)pDmaBuf, sizeof(bbxSxidData));

    // Copy out SXIDs
    params->sxidCount = bbxSxidData.sxidCount;
    for (sxidIdx = 0; sxidIdx < INFOROM_BBX_OBJ_XID_ENTRIES; sxidIdx++)
    {
        params->sxidFirst[sxidIdx].sxid = bbxSxidData.sxidFirst[sxidIdx].sxid;
        params->sxidFirst[sxidIdx].timestamp = bbxSxidData.sxidFirst[sxidIdx].timestamp;
        params->sxidLast[sxidIdx].sxid = bbxSxidData.sxidLast[sxidIdx].sxid;
        params->sxidLast[sxidIdx].timestamp = bbxSxidData.sxidLast[sxidIdx].timestamp;
    }

nvswitch_bbx_get_sxid_ls10_unmap_and_exit:
    nvswitch_os_unmap_dma_region(device->os_handle, pDmaBuf, dmaHandle,
                                        transferSize, NVSWITCH_DMA_DIR_FROM_SYSMEM);
nvswitch_bbx_get_sxid_ls10_free_and_exit:
    nvswitch_os_free_contig_memory(device->os_handle, pDmaBuf, transferSize);

    return status;
}

NvlStatus
nvswitch_bbx_get_data_ls10
(
    nvswitch_device *device,
    NvU8 dataType,
    void *params
)
{
    NvlStatus status;
    void *pDmaBuf;
    NvU64 dmaHandle;
    FLCN *pFlcn;
    RM_FLCN_CMD_SOE bbxCmd;
    NvU32 cmdSeqDesc;
    NVSWITCH_TIMEOUT timeout;
    NvU32 transferSize;

    if (!nvswitch_is_inforom_supported_ls10(device))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: InfoROM is not supported\n", __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (params == NULL)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: params is NULL\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    switch (dataType)
    {
        case RM_SOE_IFR_BBX_GET_SYS_INFO:
            transferSize = sizeof(NVSWITCH_GET_SYS_INFO_PARAMS);
        break;

        case RM_SOE_IFR_BBX_GET_TIME_INFO:
            transferSize = sizeof(NVSWITCH_GET_TIME_INFO_PARAMS);
        break;

        case RM_SOE_IFR_BBX_GET_TEMP_DATA:
            transferSize = sizeof(NVSWITCH_GET_TEMP_DATA_PARAMS);
        break;

        case RM_SOE_IFR_BBX_GET_TEMP_SAMPLES:
            transferSize = sizeof(NVSWITCH_GET_TEMP_SAMPLES_PARAMS);
        break;
        default:
            NVSWITCH_PRINT(device, ERROR, "Unknown dataType %d", dataType);
            return -NVL_BAD_ARGS;
        break;
    }

    status = nvswitch_os_alloc_contig_memory(device->os_handle, &pDmaBuf, transferSize,
                                            (device->dma_addr_width == 32));
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to allocate contig memory. rc:%d\n", __FUNCTION__, status);
        return status;
    }

    status = nvswitch_os_map_dma_region(device->os_handle, pDmaBuf, &dmaHandle,
                                        transferSize, NVSWITCH_DMA_DIR_TO_SYSMEM);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to map DMA region. rc:%d\n", __FUNCTION__, status);
        goto nvswitch_bbx_get_data_ls10_free_and_exit;
    }

    nvswitch_os_memset(pDmaBuf, 0, transferSize);

    pFlcn = device->pSoe->pFlcn;
    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);

    nvswitch_os_memset(&bbxCmd, 0, sizeof(bbxCmd));
    bbxCmd.hdr.unitId = RM_SOE_UNIT_IFR;
    bbxCmd.hdr.size = sizeof(bbxCmd);
    bbxCmd.cmd.ifr.cmdType = RM_SOE_IFR_BBX_DATA_GET;
    bbxCmd.cmd.ifr.bbxDataGet.sizeInBytes = transferSize;
    bbxCmd.cmd.ifr.bbxDataGet.dataType = dataType;
    RM_FLCN_U64_PACK(&bbxCmd.cmd.ifr.bbxDataGet.dmaHandle, &dmaHandle);

    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                (PRM_FLCN_CMD)&bbxCmd,
                                NULL,   // pMsg
                                NULL,   // pPayload
                                SOE_RM_CMDQ_LOG_ID,
                                &cmdSeqDesc,
                                &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: BX_GET_DATA type=%d failed. rc:%d\n",
                        __FUNCTION__, dataType, status);
        goto nvswitch_bbx_get_data_ls10_unmap_and_exit;
    }

    status = nvswitch_os_sync_dma_region_for_cpu(device->os_handle, dmaHandle,
                                                        transferSize,
                                                        NVSWITCH_DMA_DIR_TO_SYSMEM);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to sync DMA region. rc:%d\n", __FUNCTION__, status);
        goto nvswitch_bbx_get_data_ls10_unmap_and_exit;
    }

    if (dataType == RM_SOE_IFR_BBX_GET_SYS_INFO)
    {
        NVSWITCH_GET_SYS_INFO_PARAMS bbxSysInfoData = {0};

        nvswitch_os_memcpy((NvU8 *)&bbxSysInfoData, (NvU8 *)pDmaBuf, sizeof(NVSWITCH_GET_SYS_INFO_PARAMS));
        nvswitch_os_memcpy((NvU8 *)params, (NvU8 *)&bbxSysInfoData, sizeof(NVSWITCH_GET_SYS_INFO_PARAMS));
    }
    else if (dataType == RM_SOE_IFR_BBX_GET_TIME_INFO)
    {
        NVSWITCH_GET_TIME_INFO_PARAMS bbxTimeInfoData = {0};

        nvswitch_os_memcpy((NvU8 *)&bbxTimeInfoData, (NvU8 *)pDmaBuf, sizeof(NVSWITCH_GET_TIME_INFO_PARAMS));
        nvswitch_os_memcpy((NvU8 *)params, (NvU8 *)&bbxTimeInfoData, sizeof(NVSWITCH_GET_TIME_INFO_PARAMS));
    }
    else if (dataType == RM_SOE_IFR_BBX_GET_TEMP_DATA)
    {
        NVSWITCH_GET_TEMP_DATA_PARAMS *pBbxTempData = NULL;

        pBbxTempData = nvswitch_os_malloc(sizeof(NVSWITCH_GET_TEMP_DATA_PARAMS));
        if (pBbxTempData == NULL)
        {
            NVSWITCH_PRINT(device, ERROR, "Out of memory: dataType %d", dataType);
            status = -NVL_NO_MEM;
            goto nvswitch_bbx_get_data_ls10_unmap_and_exit;
        }
        
        nvswitch_os_memset(pBbxTempData, 0, sizeof(NVSWITCH_GET_TEMP_DATA_PARAMS));

        nvswitch_os_memcpy((NvU8 *)pBbxTempData, (NvU8 *)pDmaBuf, sizeof(NVSWITCH_GET_TEMP_DATA_PARAMS));
        nvswitch_os_memcpy((NvU8 *)params, (NvU8 *)pBbxTempData, sizeof(NVSWITCH_GET_TEMP_DATA_PARAMS));

        nvswitch_os_free(pBbxTempData);
    }
    else if (dataType == RM_SOE_IFR_BBX_GET_TEMP_SAMPLES)
    {
        NVSWITCH_GET_TEMP_SAMPLES_PARAMS *pBbxTempSamples = NULL;

        pBbxTempSamples = nvswitch_os_malloc(sizeof(NVSWITCH_GET_TEMP_SAMPLES_PARAMS));
        if (pBbxTempSamples == NULL)
        {
            NVSWITCH_PRINT(device, ERROR, "Out of memory: dataType %d", dataType);
            status = -NVL_NO_MEM;
            goto nvswitch_bbx_get_data_ls10_unmap_and_exit;
        }
        
        nvswitch_os_memset(pBbxTempSamples, 0, sizeof(NVSWITCH_GET_TEMP_SAMPLES_PARAMS));

        nvswitch_os_memcpy((NvU8 *)pBbxTempSamples, (NvU8 *)pDmaBuf, sizeof(NVSWITCH_GET_TEMP_SAMPLES_PARAMS));
        nvswitch_os_memcpy((NvU8 *)params, (NvU8 *)pBbxTempSamples, sizeof(NVSWITCH_GET_TEMP_SAMPLES_PARAMS));

        nvswitch_os_free(pBbxTempSamples);
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR, "Unknown dataType %d", dataType);
        goto nvswitch_bbx_get_data_ls10_unmap_and_exit;
    }

nvswitch_bbx_get_data_ls10_unmap_and_exit:
    nvswitch_os_unmap_dma_region(device->os_handle, pDmaBuf, dmaHandle,
                                        transferSize, NVSWITCH_DMA_DIR_FROM_SYSMEM);
nvswitch_bbx_get_data_ls10_free_and_exit:
    nvswitch_os_free_contig_memory(device->os_handle, pDmaBuf, transferSize);

    return status;
}
