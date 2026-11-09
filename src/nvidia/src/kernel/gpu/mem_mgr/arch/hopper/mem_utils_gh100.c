/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "class/clc8b5.h"  // HOPPER_DMA_COPY_A
#include "class/clc9b5.h"
#include "class/clcab5.h"


/*!
 * Create an engine object and setup SEC2 channel scheduling
 *
 * @param[in]     OBJCHANNEL - channel object 
 *
 * @returns       NV_OK
 */
NV_STATUS
memmgrMemUtilsSec2CtxInit_GH100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel
)
{
    NV_STATUS rmStatus = NV_OK;
    RM_API   *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    // Alloc sec2 context
    NV_CHECK_OK_OR_GOTO(
        rmStatus,
        LEVEL_ERROR,
        pRmApi->AllocWithHandle(pRmApi,
                                pChannel->hClient,
                                pChannel->channelId,
                                pChannel->engineObjectId,
                                pChannel->sec2Class,
                                NULL, 0),
        exit_free);

    NV_CHECK_OK_OR_GOTO(
        rmStatus,
        LEVEL_ERROR,
        memmgrMemUtilsChannelSchedulingSetup(pGpu, pMemoryManager, pChannel), exit_free);

    return NV_OK;

 exit_free:
    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hClient);
    NV_PRINTF(LEVEL_INFO, "end  NV_STATUS=0x%08x\n", rmStatus);
    return rmStatus;
}