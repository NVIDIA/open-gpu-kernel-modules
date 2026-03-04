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

#define NVOC_CRASHCAT_WAYFINDER_H_PRIVATE_ACCESS_ALLOWED
#include "crashcat/crashcat_wayfinder.h"
#include "crashcat/crashcat_engine.h"
#include "crashcat/crashcat_queue.h"
#include "crashcat/crashcat_report.h"
#include "utils/nvassert.h"
#include "nv-crashcat-decoder.h"

void crashcatWayfinderSetWFL0_V1(CrashCatWayfinder *pWayfinder, NvU32 wfl0)
{
    pWayfinder->v1.wfl0 = wfl0;
}

CrashCatQueue *crashcatWayfinderGetReportQueue_V1(CrashCatWayfinder *pWayfinder)
{
    if (pWayfinder->pQueue != NULL)
        return pWayfinder->pQueue;

    //
    // If we've already decoded WFL1 but don't have a queue, queue control object creation failed
    // and is unlikely to succeed on subsequent attempts, and we don't want to spam the logs.
    //
    if (pWayfinder->v1.wfl1 != 0)
        return NULL;

    CrashCatEngine *pEngine = CRASHCAT_GET_ENGINE(pWayfinder);
    NV_CRASHCAT_SCRATCH_GROUP_ID wfl1Location =
        crashcatWayfinderL0V1Wfl1Location(pWayfinder->v1.wfl0);

    // Read the L1 wayfinder to locate the queue
    const NvU32 *pScratchOffsets = crashcatEngineGetScratchOffsets(pEngine, wfl1Location);
    if (pScratchOffsets == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "invalid WFL1 scratch location %u\n", wfl1Location);
        return NULL;
    }

    //
    // In NV_CRASHCAT_WAYFINDER_VERSION_1, the WFL1 contains two 32-bit values specifying the queue
    // location, so we only need to read two registers. Where the scratch group contains 4
    // registers, the other two will be used for the queue control.
    //
    if ((pScratchOffsets[0] == 0) || (pScratchOffsets[1] == 0))
    {
        NV_PRINTF(LEVEL_ERROR, "insufficiently-sized L1 wayfinder scratch location %u\n",
                  wfl1Location);
        return NULL;
    }

    // Have we already decoded WFL1?
    if (pWayfinder->v1.wfl1 == 0)
    {
        pWayfinder->v1.wfl1 =
            ((NvU64)crashcatEnginePriRead(pEngine, pScratchOffsets[1]) << 32) |
                    crashcatEnginePriRead(pEngine, pScratchOffsets[0]);
    }

    CrashCatQueueConfig queueConfig;

    queueConfig.aperture = crashcatWayfinderL1V1QueueAperture(pWayfinder->v1.wfl1);
    queueConfig.size = crashcatWayfinderL1V1QueueSize(pWayfinder->v1.wfl1);
    queueConfig.offset = crashcatWayfinderL1V1QueueOffset(pWayfinder->v1.wfl1);

    if ((pScratchOffsets[2] != 0) && (pScratchOffsets[3] != 0))
    {
        //
        // If the scratch group has at least 4 32-bit registers, the 3rd and 4th are used for queue
        // put and get, respectively.
        //
        queueConfig.putRegOffset = pScratchOffsets[2];
        queueConfig.getRegOffset = pScratchOffsets[3];
    }
    else
    {
        //
        // If there are only 2 scratch registers in the group, the WFL1 is erased and the registers
        // are reused for queue put and get, respectively.
        //
        queueConfig.putRegOffset = pScratchOffsets[0];
        queueConfig.getRegOffset = pScratchOffsets[1];

        crashcatEnginePriWrite(pEngine, queueConfig.getRegOffset, 0);
        crashcatEnginePriWrite(pEngine, queueConfig.putRegOffset, 0);

        //
        // Producer watches WFL0 and waits for the _WFL1_LOCATION bits to be set to _NONE
        // before it will update the queue put pointer.
        //
        NvU32 wfl0Offset = crashcatEngineGetWFL0Offset(pEngine);
        NvU32 wfl0 = FLD_SET_DRF64(_CRASHCAT, _WAYFINDER_L0_V1, _WFL1_LOCATION, _NONE,
                                   pWayfinder->v1.wfl0);
        crashcatEnginePriWrite(pEngine, wfl0Offset, wfl0);
    }

    // Create the queue control object
    NV_STATUS status;
    NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
        objCreate(&pWayfinder->pQueue, pWayfinder, CrashCatQueue, &queueConfig),
        return NULL);

    return pWayfinder->pQueue;
}
