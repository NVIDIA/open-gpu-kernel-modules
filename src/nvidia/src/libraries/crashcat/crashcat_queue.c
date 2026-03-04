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

NV_STATUS crashcatQueueConstruct_IMPL(CrashCatQueue *pQueue, CrashCatQueueConfig *pConfig)
{
    pQueue->pEngine = CRASHCAT_GET_ENGINE(pQueue);
    pQueue->config = *pConfig;
    pQueue->pMapping = crashcatEngineMapCrashBuffer(pQueue->pEngine, pQueue->config.aperture,
                                                    pQueue->config.offset, pQueue->config.size);
    NV_CHECK_OR_RETURN(LEVEL_WARNING, pQueue->pMapping != NULL, NV_ERR_INVALID_OBJECT_BUFFER);

    return NV_OK;
}

void crashcatQueueDestruct_IMPL(CrashCatQueue *pQueue)
{
    crashcatEngineUnmapCrashBuffer(pQueue->pEngine, pQueue->pMapping);
}