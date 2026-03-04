/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _OS_WORKITEM_H_
#define _OS_WORKITEM_H_

#include "nvtypes.h"
#include "nvstatus.h"

struct OBJGPU;


typedef enum
{
    WORKITEM_FLAGS_API_LOCK_NOT_ACQUIRED = 0,
    WORKITEM_FLAGS_API_LOCK_READ_WRITE,
    WORKITEM_FLAGS_API_LOCK_READ_ONLY
} WorkItemFlagsApiLock;

typedef struct
{
    NvU32 bDontFreeParams:1;
    NvU32 bFallbackToDpc:1;
    NvU32 bRequiresGpu:1;

    //
    // Lock flags:
    // Only one of the LOCK_GPU flags should be provided. If multiple are,
    // the priority ordering should be GPUS > GROUP_DEVICE > GROUP_SUBDEVICE
    //
    NvU32 bLockSema:1;
    WorkItemFlagsApiLock apiLock:2;
    NvU32 bLockGpus:1;
    NvU32 bLockGpuGroupDevice:1;
    NvU32 bLockGpuGroupSubdevice:1;

    //
    // Perform a GPU full power sanity after getting GPU locks.
    // One of the above LOCK_GPU flags must be provided when using this flag.
    //
    NvU32 bFullGpuSanity:1;
    NvU32 bForPmResume:1;

    NvU32 bDropOnUnloadQueueFlush:1;
} OsQueueWorkItemFlags;

typedef void OSWorkItemFunction(NvU32 gpuInstance, void *);
typedef void OSSystemWorkItemFunction(void *);
NV_STATUS  osQueueWorkItem(struct OBJGPU *pGpu, OSWorkItemFunction pFunction, void *pParams, OsQueueWorkItemFlags flags);

NV_STATUS  osQueueSystemWorkItem(OSSystemWorkItemFunction, void *);


#endif // _OS_WORKITEM_H_
