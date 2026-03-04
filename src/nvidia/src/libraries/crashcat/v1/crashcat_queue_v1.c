/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_CRASHCAT_QUEUE_H_PRIVATE_ACCESS_ALLOWED
#include "crashcat/crashcat_queue.h"
#include "crashcat/crashcat_engine.h"
#include "crashcat/crashcat_report.h"
#include "utils/nvassert.h"
#include "nv-crashcat-decoder.h"

static NV_STATUS _getCrashCatReportHalspecArgs
(
    void *pBuf,
    NvU8 *pFormatVersion,
    NvU64 *pImplementerSig
)
{
    NvCrashCatPacketHeader_V1 header = *(NvCrashCatPacketHeader_V1 *)pBuf;

    // Verify the report data looks sane
    NV_CHECK_OR_RETURN(LEVEL_ERROR, crashcatPacketHeaderValid(header), NV_ERR_INVALID_DATA);

    *pFormatVersion = crashcatPacketHeaderFormatVersion(header);
    switch (*pFormatVersion)
    {
        default:
            // Fall through for backward compatibility if version is not recognized
        case NV_CRASHCAT_PACKET_FORMAT_VERSION_1:
        {
            NvCrashCatReport_V1 *pReport = (NvCrashCatReport_V1 *)pBuf;
            *pImplementerSig = pReport->implementerSignature;
            break;
        }
    }

    return NV_OK;
}

CrashCatReport *crashcatQueueConsumeNextReport_V1(CrashCatQueue *pQueue)
{
    const NvU32 size = pQueue->config.size;
    const NvU32 put = crashcatEnginePriRead(pQueue->pEngine, pQueue->config.putRegOffset);
    NvU32 get = crashcatEnginePriRead(pQueue->pEngine, pQueue->config.getRegOffset);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, get < size, NULL);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, put < size, NULL);
    NV_CHECK_OR_RETURN(LEVEL_SILENT, put != get, NULL); // Nothing to read

    NvU32 readSize;
    void *pBuf;
    NvBool bAllocated = NV_FALSE;

    if (put > get)
    {
        // We can read directly from the mapping
        readSize = put - get;
        crashcatEngineSyncCrashBuffer(pQueue->pEngine, pQueue->pMapping, get, readSize);
        pBuf = (void *)((NvUPtr)pQueue->pMapping + get);
    }
    else if (put == 0)
    {
        // Buffer just wrapped, but we can still read directly from the mapping
        readSize = size - get;
        crashcatEngineSyncCrashBuffer(pQueue->pEngine, pQueue->pMapping, get, readSize);
        pBuf = (void *)((NvUPtr)pQueue->pMapping + get);
    }
    else
    {
        // Need to handle wraparound, allocate a temporary buffer to simplify decoding
        NvU32 preWrapSize = size - get;
        NvU32 postWrapSize = put;
        readSize = preWrapSize + postWrapSize;
        pBuf = portMemAllocNonPaged(readSize);
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pBuf != NULL, NULL);
        bAllocated = NV_TRUE;

        crashcatEngineSyncCrashBuffer(pQueue->pEngine, pQueue->pMapping, get, preWrapSize);
        portMemCopy(pBuf, preWrapSize,
                    (void *)((NvUPtr)pQueue->pMapping + get), preWrapSize);

        crashcatEngineSyncCrashBuffer(pQueue->pEngine, pQueue->pMapping, 0, postWrapSize);
        portMemCopy((void *)((NvUPtr)pBuf + preWrapSize), postWrapSize,
                    pQueue->pMapping, postWrapSize);
    }

    //
    // To create the CrashCatReport object, we pass the implementer signature as a halspec arg.
    // The implementer signature location is technically report-version-specific, so we need a
    // little adapter logic to get the right one before the report is created.
    //
    CrashCatReport *pReport = NULL;
    void *pReadBuf = pBuf;
    NV_STATUS status = NV_ERR_INVALID_DATA;
    NvU8 reportFormatVersion;
    NvU64 reportImplementer;

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        _getCrashCatReportHalspecArgs(pBuf, &reportFormatVersion, &reportImplementer),
        updateGetPointer);

    NV_CHECK_OK(status, LEVEL_ERROR,
        objCreate(&pReport, pQueue, CrashCatReport,
                  reportFormatVersion, reportImplementer,
                  &pReadBuf, readSize));

updateGetPointer:
    //
    // Update the get pointer based on how many bytes were read, or skip it all if there was an
    // extraction failure, so that we don't flood the logs with repeated failed extraction attempts.
    // TODO: log raw data somewhere so the failure can be analyzed.
    //
    {
        NvU64 diff = (NvU64)pReadBuf - (NvU64)pBuf;
        if ((status != NV_OK) || (diff == 0))
            diff = readSize;

        NV_ASSERT_CHECKED(NvU64_HI32(diff) == 0);
        get = (get + NvU64_LO32(diff)) % size;
        crashcatEnginePriWrite(pQueue->pEngine, pQueue->config.getRegOffset, get);
    }

    if (bAllocated)
        portMemFree(pBuf);

    return pReport;
}
