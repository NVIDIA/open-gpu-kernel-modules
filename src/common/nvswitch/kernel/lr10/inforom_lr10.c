/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "lr10/lr10.h"
#include "lr10/inforom_lr10.h"
#include "inforom/ifrstruct.h"
#include "nvswitch/lr10/dev_nvlsaw_ip.h"
#include "nvswitch/lr10/dev_nvlsaw_ip_addendum.h"
#include "nvswitch/lr10/dev_pmgr.h"

//
// TODO: Split individual object hals to their own respective files
//
static void _oms_parse(nvswitch_device *device, INFOROM_OMS_STATE *pOmsState);
static void _oms_refresh(nvswitch_device *device, INFOROM_OMS_STATE *pOmsState);
NvlStatus
nvswitch_inforom_nvl_log_error_event_lr10
(
    nvswitch_device *device,
    void *pNvlGeneric,
    void *pNvlErrorEvent,
    NvBool *bDirty
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_inforom_nvl_get_max_correctable_error_rate_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_MAX_CORRECTABLE_ERROR_RATES_PARAMS *params
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_inforom_nvl_get_errors_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_ERROR_COUNTS_PARAMS *params
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}
NvlStatus nvswitch_inforom_nvl_update_link_correctable_error_info_lr10
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
    return -NVL_ERR_NOT_SUPPORTED;
}
static
NvlStatus
_inforom_ecc_find_useable_entry_index
(
    INFOROM_ECC_OBJECT_V6_S0 *pEccObj,
    INFOROM_NVS_ECC_ERROR_EVENT *error_event,
    NvU8 *pEntryIndex
)
{
    NvU8 entry;

    //
    // The size of the "entry" variable needs to be updated if the InfoROM ECC
    // error log ever grows past 256
    //
    ct_assert(INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_MAX_COUNT <= NV_U8_MAX);

    for (entry = 0; entry < INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_MAX_COUNT; entry++)
    {
        INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER *pErrorEntry = &(pEccObj->errorEntries[entry]);

        //
        // Check if the entry already exists
        // Ideally the address should be verified only if it is valid, however
        // we scrub an invalid address early on so expect them to match the
        // recorded value in either case
        //
        if ((pErrorEntry->errId == error_event->sxid) &&
            FLD_TEST_DRF_NUM(_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER, _HEADER,
                             _ADDR_VALID, error_event->bAddressValid, pErrorEntry->header) &&
            (pErrorEntry->address == error_event->address) &&
            FLD_TEST_DRF_NUM(_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER, _LOCATION,
                             _LINK_ID, error_event->linkId, pErrorEntry->location))
            break;
        //
        // Encountering an empty entry indicates this is the first instance of the error
        // The ECC error log on the InfoROM is never sparse so we can terminate
        // the search early
        //
        else if (FLD_TEST_DRF(_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER, _HEADER,
                              _VALID, _FALSE, pErrorEntry->header))
            break;
    }

    if (entry == INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_MAX_COUNT)
        return -NVL_NOT_FOUND;

    *pEntryIndex = entry;

    return NVL_SUCCESS;
}

static
NvlStatus
_inforom_ecc_calc_timestamp_delta
(
    INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER *pErrorEntry,
    INFOROM_NVS_ECC_ERROR_EVENT *error_event,
    NvU64 existingCount
)
{
    //
    // Subtracting 1 from the existingCount to drop the first error event counter
    // Unfortunately we cannot track the first error events counts so assuming 1
    //

    NvlStatus status = NVL_SUCCESS;
    NvU32 currTime = error_event->timestamp;
    NvU64 tmp = ((NvU64) pErrorEntry->averageEventDelta) * (existingCount - 1);
    NvU64 ovfTmp = tmp + (currTime - pErrorEntry->lastErrorTimestamp);
    NvU64 totCnt, delta;

    if (ovfTmp < tmp)
    {
        status = -NVL_NO_MEM;
        goto _updateEntryTimeFailed;
    }

    totCnt = error_event->errorCount + existingCount - 1;
    delta = ovfTmp / totCnt;

    if (delta > NV_U32_MAX)
    {
        status = -NVL_NO_MEM;
        goto _updateEntryTimeFailed;
    }

    pErrorEntry->averageEventDelta = (NvU32) delta;

_updateEntryTimeFailed:
    return status;
}

static
NvlStatus
_inforom_ecc_record_entry
(
    INFOROM_ECC_OBJECT_V6_S0 *pEccObj,
    INFOROM_NVS_ECC_ERROR_EVENT *error_event,
    NvU8 entry
)
{
    NvBool bNewEntry;
    NvU32 *pErrCnt;

    INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER *pErrorEntry = &(pEccObj->errorEntries[entry]);

    bNewEntry = FLD_TEST_DRF(_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER, _HEADER,
                             _VALID, _FALSE, pErrorEntry->header);

    pErrCnt = ((error_event->bUncErr) ? &(pErrorEntry->uncorrectedCount) :
                                      &(pErrorEntry->correctedCount));

    if (bNewEntry)
    {
        pErrorEntry->errId = error_event->sxid;

        pErrorEntry->location = FLD_SET_DRF_NUM(_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER,
            _LOCATION, _LINK_ID, error_event->linkId, pErrorEntry->location);

        pErrorEntry->header = FLD_SET_DRF_NUM(_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER,
            _HEADER, _ADDR_VALID, error_event->bAddressValid, pErrorEntry->header);

        pErrorEntry->address = error_event->address;

        pErrorEntry->sublocation = 0;

        *pErrCnt = error_event->errorCount;

        pErrorEntry->averageEventDelta = 0;

        pErrorEntry->header = FLD_SET_DRF(_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER, _HEADER,
                                          _VALID, _TRUE, pErrorEntry->header);
    }
    else
    {
        NvlStatus status;
        NvU64 tmpCnt;
        NvU64 existingCnt = (NvU64) (pErrorEntry->uncorrectedCount + pErrorEntry->correctedCount);

        status = _inforom_ecc_calc_timestamp_delta(pErrorEntry, error_event, existingCnt);
        if (status != NVL_SUCCESS)
        {
            pErrorEntry->header = FLD_SET_DRF(_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER,
                _HEADER, _CORRUPT_TIMEDATA, _TRUE, pErrorEntry->header);
        }

        // Update error counts by summing them up
        tmpCnt = (NvU64) *pErrCnt + error_event->errorCount;

        // Saturate at NvU32 limit
        if (tmpCnt > NV_U32_MAX)
        {
            tmpCnt = NV_U32_MAX;
        }

        *pErrCnt = (NvU32) tmpCnt;
    }

    pErrorEntry->lastErrorTimestamp = error_event->timestamp;

    return NVL_SUCCESS;

}

NvlStatus
nvswitch_inforom_ecc_log_error_event_lr10
(
    nvswitch_device *device,
    INFOROM_ECC_OBJECT *pEccGeneric,
    INFOROM_NVS_ECC_ERROR_EVENT *err_event
)
{
    NvU8 entry;
    NvU64_ALIGN32 *pInforomTotalCount;
    NvU64 tmpCount;
    NvlStatus status;
    INFOROM_ECC_OBJECT_V6_S0 *pEccObj;

    if ((err_event == NULL) || (pEccGeneric == NULL))
        return -NVL_BAD_ARGS;

    pEccObj = &(pEccGeneric->v6s);

    //
    // Find the appropriate entry to log the error event
    // If the function returns "out of memory" error, indicates no free entries
    //
    status = _inforom_ecc_find_useable_entry_index(pEccObj, err_event, &entry);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "InfoROM ECC: Unable to find logging entry rc: %d\n", status);
        goto _ecc_log_error_event_lr10_failed;
    }

    //
    // Record the error data into appropriate members of the error entry struct
    // Also mark the entry as in-use if it is a new entry
    //
    status = _inforom_ecc_record_entry(pEccObj, err_event, entry);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "InfoROM ECC: Unable to record entry:%u rc:%d\n",
                       entry, status);

        goto _ecc_log_error_event_lr10_failed;
    }

    // Log the error count to the InfoROM total values
    if (err_event->bUncErr)
    {
        pInforomTotalCount = &(pEccObj->uncorrectedTotal);
    }
    else
    {
        pInforomTotalCount = &(pEccObj->correctedTotal);
    }

    NvU64_ALIGN32_UNPACK(&tmpCount, pInforomTotalCount);

    tmpCount += err_event->errorCount;
    if (tmpCount < err_event->errorCount)
    {
        tmpCount = NV_U64_MAX;
    }

    NvU64_ALIGN32_PACK(pInforomTotalCount, &tmpCount);

    // Update shared surface counts, non-fatal if we encounter a failure
    status = nvswitch_smbpbi_refresh_ecc_counts(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, WARN, "Failed to update ECC counts on SMBPBI "
                       "shared surface rc:%d\n", status);
    }

    return NVL_SUCCESS;

_ecc_log_error_event_lr10_failed:

    NVSWITCH_PRINT(device, ERROR, "Missed recording sxid=%u, linkId=%u, address=0x%04x, "
                   "timestamp=%u, errorCount=%u\n", err_event->sxid,
                   err_event->linkId, err_event->address, err_event->timestamp,
                   err_event->errorCount);

    return status;
}

void
nvswitch_inforom_ecc_get_total_errors_lr10
(
    nvswitch_device    *device,
    INFOROM_ECC_OBJECT *pEccGeneric,
    NvU64              *pCorrectedTotal,
    NvU64              *pUncorrectedTotal
)
{
    INFOROM_ECC_OBJECT_V6_S0 *pEccObj = &(pEccGeneric->v6s);

    NvU64_ALIGN32_UNPACK(pCorrectedTotal, &pEccObj->correctedTotal);
    NvU64_ALIGN32_UNPACK(pUncorrectedTotal, &pEccObj->uncorrectedTotal);
}

static void _nvswitch_inforom_map_ecc_error_to_userspace_error
(
    INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER *pEccError,
    NVSWITCH_ECC_ERROR_ENTRY *pErrorLog
)
{
    pErrorLog->sxid = pEccError->errId;
    pErrorLog->linkId = DRF_VAL(_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER, _LOCATION, _LINK_ID, pEccError->location);
    pErrorLog->lastErrorTimestamp = pEccError->lastErrorTimestamp;
    pErrorLog->bAddressValid = DRF_VAL(_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER, _HEADER, _ADDR_VALID, pEccError->header);
    pErrorLog->address = pEccError->address;
    pErrorLog->correctedCount = pEccError->correctedCount;
    pErrorLog->uncorrectedCount = pEccError->uncorrectedCount;
    return;
}


NvlStatus
nvswitch_inforom_ecc_get_errors_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_ECC_ERROR_COUNTS_PARAMS *params
)
{
    struct inforom *pInforom = device->pInforom;
    PINFOROM_ECC_STATE pEccState;
    INFOROM_ECC_OBJECT  *pEcc;
    NvU32 errIndx;

    /*
     * Compile time check is needed here to make sure that the ECC_ERROR API interface query size is in sync 
     * with its internal counterpart. When the definition of the internal InfoROM error size limit changes, 
     * it will enforce API interface change as well, or use a retry style query with err_index 
     */
    ct_assert(NVSWITCH_ECC_ERRORS_MAX_READ_COUNT == INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER_MAX_COUNT);

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pEccState = pInforom->pEccState;
    if (pEccState == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pEcc = pEccState->pEcc;
    if (pEcc == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    nvswitch_os_memset(params->errorLog, 0, sizeof(params->errorLog));

    nvswitch_os_memcpy(&params->correctedTotal, &pEcc->v6s.correctedTotal, sizeof(params->correctedTotal));
    nvswitch_os_memcpy(&params->uncorrectedTotal, &pEcc->v6s.uncorrectedTotal, sizeof(params->uncorrectedTotal));

    for (errIndx = 0; errIndx < NVSWITCH_ECC_ERRORS_MAX_READ_COUNT; errIndx++)
    {
        if (FLD_TEST_DRF(_INFOROM_ECC_OBJECT_V6_S0_ERROR_COUNTER, _HEADER, _VALID, _FALSE,
                         pEcc->v6s.errorEntries[errIndx].header))
        {
            break; // the last entry
        }

        _nvswitch_inforom_map_ecc_error_to_userspace_error(&pEcc->v6s.errorEntries[errIndx],
                                                           &params->errorLog[errIndx]);
    }

    params->errorCount = errIndx;

    return NVL_SUCCESS;
}

static NvU8 _oms_dword_byte_sum(NvU16 dword)
{
    NvU8 i, sum = 0;
    for (i = 0; i < sizeof(dword); i++)
        sum += (NvU8)((dword >> (8*i)) & 0xFF);
    return sum;
}

static void _oms_update_entry_checksum
(
    INFOROM_OMS_OBJECT_V1S_SETTINGS_ENTRY *pEntry
)
{
    NvU8 datasum = 0;

    // Upper byte is the checksum
    datasum += _oms_dword_byte_sum(pEntry->data & ~0xFF00);

    pEntry->data = FLD_SET_REF_NUM(
        INFOROM_OMS_OBJECT_V1S_SETTINGS_ENTRY_DATA_ENTRY_CHECKSUM,
        0x00u - datasum, pEntry->data);
}

static void
_oms_reset_entry_iter
(
    INFOROM_OMS_STATE *pOmsState,
    NvBool bStart
)
{
    INFOROM_OMS_OBJECT_V1S *pOms = &pOmsState->pOms->v1s;
    INFOROM_OMS_V1S_DATA *pVerData = &pOmsState->omsData.v1s;

    if (bStart)
    {
        pVerData->pIter = &pOms->settings[0];
    }
    else
    {
        pVerData->pIter = &pOms->settings[
            INFOROM_OMS_OBJECT_V1S_NUM_SETTINGS_ENTRIES - 1];
    }
}

static NvBool
_oms_entry_available
(
    INFOROM_OMS_STATE *pOmsState
)
{
    INFOROM_OMS_OBJECT_V1S_SETTINGS_ENTRY *pEntry = pOmsState->omsData.v1s.pIter;

    if (pEntry == NULL)
        return NV_FALSE;

    return FLD_TEST_REF(INFOROM_OMS_OBJECT_V1_SETTINGS_ENTRY_DATA_ENTRY_AVAILABLE,
                        _YES, pEntry->data);
}

static NvBool
_oms_entry_valid
(
    INFOROM_OMS_STATE *pOmsState
)
{
    INFOROM_OMS_OBJECT_V1S_SETTINGS_ENTRY *pEntry = pOmsState->omsData.v1s.pIter;
    NvU8 sum;

    if (pEntry == NULL)
        return NV_FALSE;

    sum = _oms_dword_byte_sum(pEntry->data);

    return (sum == 0);
}

/*
 *
 * Sets nextIdx to one after currIdx. Returns NV_TRUE if nextIdx
 * is valid. NV_FALSE otherwise.
 *
 */
static NvBool
_oms_entry_iter_next
(
    INFOROM_OMS_STATE *pOmsState
)
{
    INFOROM_OMS_OBJECT_V1S *pOms = &pOmsState->pOms->v1s;
    INFOROM_OMS_V1S_DATA *pVerData = &pOmsState->omsData.v1s;

    if (pVerData->pIter >= pOms->settings +
                            INFOROM_OMS_OBJECT_V1S_NUM_SETTINGS_ENTRIES)
    {
        pVerData->pIter = NULL;
    }
    else
    {
        pVerData->pIter++;
    }

    return (pVerData->pIter != NULL);
}

static void
_oms_refresh
(
    nvswitch_device *device,
    INFOROM_OMS_STATE *pOmsState
)
{
    INFOROM_OMS_OBJECT_V1S *pOms = &pOmsState->pOms->v1s;

    nvswitch_os_memset(pOms->settings, 0xFF, sizeof(pOms->settings));
    pOms->lifetimeRefreshCount++;

    // This is guaranteed to find and set an UpdateEntry now
    _oms_parse(device, pOmsState);
}

static void
_oms_set_current_entry
(
    INFOROM_OMS_STATE *pOmsState
)
{
    pOmsState->omsData.v1s.prev = *pOmsState->omsData.v1s.pIter;
}

static void
_oms_set_update_entry
(
    INFOROM_OMS_STATE *pOmsState
)
{
    INFOROM_OMS_V1S_DATA *pVerData = &pOmsState->omsData.v1s;

    pVerData->pNext = pVerData->pIter;

    // Next settings always start out the same as the previous
    *pVerData->pNext = pVerData->prev;
}

static NvBool
_oms_entry_iter_prev
(
    INFOROM_OMS_STATE *pOmsState
)
{
    INFOROM_OMS_OBJECT_V1S *pOms = &pOmsState->pOms->v1s;
    INFOROM_OMS_V1S_DATA *pVerData = &pOmsState->omsData.v1s;

    if (pVerData->pIter <= pOms->settings)
    {
        pVerData->pIter = NULL;
    }
    else
    {
        pVerData->pIter--;
    }

    return (pVerData->pIter != NULL);
}

static void
_oms_parse
(
    nvswitch_device *device,
    INFOROM_OMS_STATE *pOmsState
)
{
    NvBool bCurrentValid = NV_FALSE;
    NvBool bIterValid = NV_TRUE;

    //
    // To find the "latest" entry - the one with the settings that were last
    // flushed to the InfoROM - scan from the end of the array until we find
    // an entry that is not available and is valid.
    //
    _oms_reset_entry_iter(pOmsState, NV_FALSE);
    while (bIterValid)
    {
        if (!_oms_entry_available(pOmsState) &&
            _oms_entry_valid(pOmsState))
        {
            _oms_set_current_entry(pOmsState);
            bCurrentValid = NV_TRUE;
            break;
        }

        bIterValid = _oms_entry_iter_prev(pOmsState);
    }

    //
    // To find the "next" entry - one that we will write to if a setting is
    // updated - start scanning from the entry after the latest entry to find
    // an available one. This will skip entries that were previously written
    // to but are invalid.
    //
    if (bCurrentValid)
    {
        bIterValid = _oms_entry_iter_next(pOmsState);
    }
    else
    {
        _oms_reset_entry_iter(pOmsState, NV_TRUE);
        bIterValid = NV_TRUE;
    }

    while (bIterValid)
    {
        if (_oms_entry_available(pOmsState))
        {
            _oms_set_update_entry(pOmsState);
            break;
        }

        bIterValid = _oms_entry_iter_next(pOmsState);
    }

    if (!bIterValid)
    {
        //
        // No more entries available, we will need to refresh the object.
        // We should have at least one valid recent entry in this case
        // (otherwise every entry is corrupted).
        //
        NVSWITCH_ASSERT(bCurrentValid);
        _oms_refresh(device, pOmsState);
    }
}

static NvBool
_oms_is_content_dirty
(
    INFOROM_OMS_STATE *pOmsState
)
{
    INFOROM_OMS_V1S_DATA *pVerData = &pOmsState->omsData.v1s;

    if (pVerData->pNext == NULL)
        return NV_FALSE;

    return (pVerData->pNext->data != pVerData->prev.data);
}

NvlStatus
nvswitch_oms_inforom_flush_lr10
(
    nvswitch_device *device
)
{
    NvlStatus status = NVL_SUCCESS;
    struct inforom *pInforom = device->pInforom;
    INFOROM_OMS_STATE *pOmsState;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pOmsState = pInforom->pOmsState;

    if (pOmsState != NULL && _oms_is_content_dirty(pOmsState))
    {
        status = nvswitch_inforom_write_object(device, "OMS",
                                            pOmsState->pFmt, pOmsState->pOms,
                                            pOmsState->pPackedObject);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "Failed to flush OMS object to InfoROM, rc: %d\n", status);
        }
        else
        {
            _oms_parse(device, pOmsState);
        }
    }

    return status;
}

void
nvswitch_initialize_oms_state_lr10
(
    nvswitch_device *device,
    INFOROM_OMS_STATE *pOmsState
)
{
    pOmsState->omsData.v1s.pIter = pOmsState->omsData.v1s.pNext = NULL;
    pOmsState->omsData.v1s.prev.data =
            REF_DEF(INFOROM_OMS_OBJECT_V1_SETTINGS_ENTRY_DATA_ENTRY_AVAILABLE, _NO) |
            REF_DEF(INFOROM_OMS_OBJECT_V1_SETTINGS_ENTRY_DATA_FORCE_DEVICE_DISABLE, _NO);
    _oms_update_entry_checksum(&pOmsState->omsData.v1s.prev);

    _oms_parse(device, pOmsState);
}

NvBool
nvswitch_oms_get_device_disable_lr10
(
    INFOROM_OMS_STATE *pOmsState
)
{
    INFOROM_OMS_V1S_DATA *pVerData = &pOmsState->omsData.v1s;

    return FLD_TEST_REF(
                INFOROM_OMS_OBJECT_V1_SETTINGS_ENTRY_DATA_FORCE_DEVICE_DISABLE,
                _YES, pVerData->pNext->data);
}

void
nvswitch_oms_set_device_disable_lr10
(
    INFOROM_OMS_STATE *pOmsState,
    NvBool bForceDeviceDisable
)
{
    INFOROM_OMS_V1S_DATA *pVerData = &pOmsState->omsData.v1s;

    pVerData->pNext->data = FLD_SET_REF_NUM(
                INFOROM_OMS_OBJECT_V1_SETTINGS_ENTRY_DATA_FORCE_DEVICE_DISABLE,
                bForceDeviceDisable, pVerData->pNext->data);

    _oms_update_entry_checksum(pVerData->pNext);
}

NvlStatus
nvswitch_bbx_setup_prologue_lr10
(
    nvswitch_device    *device,
    void               *pInforomBbxState
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_bbx_setup_epilogue_lr10
(
    nvswitch_device    *device,
    void *pInforomBbxState
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_bbx_add_data_time_lr10
(
    nvswitch_device *device,
    void *pInforomBbxState,
    void *pInforomBbxData
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_bbx_add_sxid_lr10
(
    nvswitch_device *device,
    void *pInforomBbxState,
    void *pInforomBbxData
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_bbx_add_temperature_lr10
(
    nvswitch_device *device,
    void *pInforomBbxState,
    void *pInforomBbxData
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}
void
nvswitch_bbx_set_initial_temperature_lr10
(
    nvswitch_device *device,
    void *pInforomBbxState,
    void *pInforomBbxData
)
{
    return;
}

NvlStatus
nvswitch_inforom_bbx_get_sxid_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_SXIDS_PARAMS *params
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}
