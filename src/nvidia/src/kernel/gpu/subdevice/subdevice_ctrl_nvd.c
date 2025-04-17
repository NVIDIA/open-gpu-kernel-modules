/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvdump.h"
#include "os/os.h"
#include "diagnostics/nv_debug_dump.h"
#include "kernel/gpu/gpu_resource.h"
#include "kernel/gpu/subdevice/subdevice.h"

#include "lib/protobuf/prb.h"
#include "g_nvdebug_pb.h"
#include "lib/protobuf/prb_util.h"
#include "diagnostics/journal.h"

//
// NVD RM SubDevice Controls
//

/*!
 * @brief Get Dump Size. Returns an estimate of the number of bytes in the dump
 * that can be used to allocate a buffer. The size is based on the component
 * argument.
 *
 * @param[in] pSubDevice
 * @param[in] pDumpSizeParams
 *
 * @returns NV_OK on success
 */
NV_STATUS
subdeviceCtrlCmdNvdGetDumpSize_IMPL
(
    Subdevice                            *pSubdevice,
    NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS *pDumpSizeParams
)
{
    OBJGPU       *pGpu          = GPU_RES_GET_GPU(pSubdevice);
    NvDebugDump  *pNvd          = GPU_GET_NVD(pGpu);
    NVDUMP_BUFFER nvDumpBuffer  = {0};
    NV_STATUS     rmStatus;

    // Allow for the largest possible dump size, if needed
    nvDumpBuffer.size = NVDUMP_MAX_DUMP_SIZE;

    rmStatus = nvdDumpComponent(pGpu,
                                pNvd,
                                pDumpSizeParams->component,
                                &nvDumpBuffer,
                                NVDUMP_BUFFER_COUNT,
                                NULL);

    pDumpSizeParams->size = nvDumpBuffer.curNumBytes;

    return rmStatus;
}

/*!
 * @brief Get Dump. Returns a dump that includes the component specified
 * when the conditions in the trigger are set.
 *
 * @param[in] pSubDevice
 * @param[in] pDumpParams
 *
 * @returns NV_OK on success
 */
NV_STATUS
subdeviceCtrlCmdNvdGetDump_IMPL
(
    Subdevice                       *pSubdevice,
    NV2080_CTRL_NVD_GET_DUMP_PARAMS *pDumpParams
)
{
    OBJGPU       *pGpu          = GPU_RES_GET_GPU(pSubdevice);
    NvDebugDump  *pNvd          = GPU_GET_NVD(pGpu);
    NVDUMP_BUFFER nvDumpBuffer  = {0};
    NV_STATUS     rmStatus      = NV_OK;

    nvDumpBuffer.size    = pDumpParams->size;
    nvDumpBuffer.address = pDumpParams->pBuffer;

    // Dump the component
    rmStatus = nvdDumpComponent(pGpu,
                                pNvd,
                                pDumpParams->component,
                                &nvDumpBuffer,
                                NVDUMP_BUFFER_PROVIDED,
                                NULL);

    pDumpParams->size = nvDumpBuffer.curNumBytes;
    return rmStatus;

}
/*!
* @brief helper function to convert timestamps from hi res timer to time in ms since 1970
* OCA records time in tick since boot. so in order to convert to a time stamp we need to
* convert the ticks to ms & add it to the boot time.
*
* @returns time since 1970 in ms
*/
static NvU64 createTimestampFromTimer(NvU64 timerVal)
{
    NvU32 currTimeSec = 0;
    NvU32 currTimeUsec = 0;
    NvU64 currTimeMsec;

    NvU64 timeSinceBootNsec = 0;
    NvU64 timeSinceBootMsec = 0;

    NvU64 timerFreq;
    NvU64 timeValMsec;

    NvU64 timestampMs;

    // get all the current time info.
    timeSinceBootNsec = osGetCurrentTick();           // get the time since boot in ns
    osGetCurrentTime(&currTimeSec, &currTimeUsec);  // get the current time
    timerFreq = osGetTimestampFreq();               // get the ticks/second.

    // convert everything to the same base (ms)
    // convert the time value from ticks to ms since boot.
    timeValMsec = (timerVal * 1000) / timerFreq;

    // scale time since boot to from ns to ms
    timeSinceBootMsec = timeSinceBootNsec / 1000000;

    // put it together in ms
    currTimeMsec = currTimeSec;             // need to move this to the 64 bit value
    currTimeMsec *= 1000;                   // before multiply to avoid overflow.
    currTimeMsec += currTimeUsec / 1000;

    // put it all together.
    timestampMs = currTimeMsec - timeSinceBootMsec; // determine boot time.
    timestampMs += timeValMsec;                     // add in the timeVal since boot
    return timestampMs;
}

/*!
* @brief Get the NOCAT journal Rpt. Returns the entries in the NOCAT Journal
*
* @returns NV_OK on success
*/
NV_STATUS
subdeviceCtrlCmdNvdGetNocatJournalRpt_IMPL
(
    Subdevice                       *pSubdevice,
    NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS *pReportParams
)
{
    OBJSYS                     *pSys = SYS_GET_INSTANCE();
    Journal                    *pRcdb = SYS_GET_RCDB(pSys);

    NvU32                       idx;
    NV_STATUS                   status;
    NvU32                       flags;

    if (pRcdb == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    // start with a clean slate
    flags = pReportParams->flags;
    portMemSet(pReportParams, 0, sizeof(*pReportParams));
    pReportParams->flags = flags;

    // get reports until we run out of reports or run out of space.
    for (idx = 0; idx < NV2080_NOCAT_JOURNAL_MAX_JOURNAL_RECORDS; idx++)
    {
        status = rcdbReportNextNocatJournalEntry(&pReportParams->journalRecords[idx]);

        if (status != NV_OK)
        {
            if ((status == NV_ERR_OBJECT_NOT_FOUND) || (idx != 0))
            {
                // call to get the next record failed,
                // either we have run out of records,
                // or we have put at least one record into report.
                // we will call that a success so we report the records we have, or a 0 count.
                // NOTE -- NvAPI translates OBJECT_NOT_FOUND to a general NVAPI_ERROR,
                // so the caller can not tell the reason for the failure is we ran out of records.
                // that is why we are translating that to a success here.
                status = NV_OK;
            }
            break;
        }
        // fix up the time stamp
        pReportParams->journalRecords[idx].timeStamp =
            createTimestampFromTimer(pReportParams->journalRecords[idx].timeStamp);
    }
    if (status == NV_OK)
    {
        //update the counters.
        pReportParams->nocatRecordCount = idx;
        pReportParams->nocatOutstandingRecordCount = rcdbGetNocatOutstandingCount(pRcdb);

        // add in the activity counters.
        portMemCopy(pReportParams->activityCounters, sizeof(pReportParams->activityCounters),
            pRcdb->nocatJournalDescriptor.nocatEventCounters,
            sizeof(pRcdb->nocatJournalDescriptor.nocatEventCounters));
    }
    return status;
}

/*!
* @brief Set the NOCAT TDR data collected by KMD in the NOCAT journal record
*
* @returns NV_OK on success
*/
NV_STATUS
subdeviceCtrlCmdNvdSetNocatJournalData_IMPL
(
    Subdevice                       *pSubdevice,
    NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS* pReportParams
)
{
    OBJSYS                     *pSys = SYS_GET_INSTANCE();
    Journal                    *pRcdb = SYS_GET_RCDB(pSys);
    OBJGPU* pGpu = GPU_RES_GET_GPU(pSubdevice);
    NOCAT_JOURNAL_PARAMS    newEntry;

    switch (pReportParams->dataType)
    {
    case NV2080_CTRL_NOCAT_JOURNAL_DATA_TYPE_TDR_REASON:
        rcdbSetNocatTdrReason(&pReportParams->nocatJournalData.tdrReason);
        break;

    case NV2080_CTRL_NOCAT_JOURNAL_DATA_TYPE_SET_TAG:
        if ((pReportParams->nocatJournalData.tagData.tag[0] == '\0') ||
            FLD_TEST_DRF(2080_CTRL, _NOCAT_TAG, _CLEAR, _YES,
                pReportParams->nocatJournalData.tagData.flags))
        {
            // clear the tag
            portMemSet(pRcdb->nocatJournalDescriptor.tag, 0,
                sizeof(pRcdb->nocatJournalDescriptor.tag));
        }
        else
        {
            // save the tag
            portStringCopy((char *)pRcdb->nocatJournalDescriptor.tag,
                NV2080_NOCAT_JOURNAL_MAX_STR_LEN,
                (char *)pReportParams->nocatJournalData.tagData.tag,
                portStringLength((char *)pReportParams->nocatJournalData.tagData.tag) + 1);
        }
        break;

    case NV2080_CTRL_NOCAT_JOURNAL_DATA_TYPE_RCLOG:
        portMemSet(&newEntry, 0, sizeof(newEntry));

        // fill in the newEntry structure rec type & source data.
        newEntry.recType = NV2080_NOCAT_JOURNAL_REC_TYPE_BUGCHECK;
        newEntry.pSource = "RCLOG";

        // point to the payload of the nocat Journal Data for an rclog entry.
        newEntry.pDiagBuffer = (void*)&pReportParams->nocatJournalData.rclog;
        newEntry.diagBufferLen = sizeof(pReportParams->nocatJournalData.rclog);

        rcdbNocatInsertNocatError(pGpu, &newEntry);
        break;


    default:
        break;
    }
    return NV_OK;
}

/*!
* @brief Set the NOCAT TDR data collected by KMD in the NOCAT journal record
*
* @returns NV_OK on success
*/
NV_STATUS
subdeviceCtrlCmdNvdInsertNocatJournalRecord_IMPL
(
    Subdevice                       *pSubdevice,
    NV2080_CTRL_CMD_NVD_INSERT_NOCAT_JOURNAL_RECORD_PARAMS* pRecordParams
)
{
    OBJSYS       *pSys = SYS_GET_INSTANCE();
    Journal      *pRcdb = SYS_GET_RCDB(pSys);
    OBJGPU       *pGpu = GPU_RES_GET_GPU(pSubdevice);

    NOCAT_JOURNAL_PARAMS    newEntry;

    portMemSet(&newEntry, 0, sizeof(newEntry));

    // fill in the newEntry structure with the data from the insertData.
    newEntry.timestamp          = pRecordParams->nocatJournalRecord.timestamp;
    newEntry.recType            = pRecordParams->nocatJournalRecord.recType;
    newEntry.bugcheck           = pRecordParams->nocatJournalRecord.bugcheck;
    newEntry.pSource            = (char *)pRecordParams->nocatJournalRecord.source;
    newEntry.subsystem          = pRecordParams->nocatJournalRecord.subsystem;
    newEntry.errorCode          = pRecordParams->nocatJournalRecord.errorCode;
    newEntry.pDiagBuffer        = pRecordParams->nocatJournalRecord.diagBuffer;
    newEntry.diagBufferLen      = pRecordParams->nocatJournalRecord.diagBufferLen;
    newEntry.pFaultingEngine    = (char *)pRecordParams->nocatJournalRecord.faultingEngine;
    newEntry.tdrReason          = pRecordParams->nocatJournalRecord.tdrReason;

    pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_RPC_INSERT_RECORDS_IDX]++;
    rcdbNocatInsertNocatError(pGpu, &newEntry);

    return NV_OK;
}

