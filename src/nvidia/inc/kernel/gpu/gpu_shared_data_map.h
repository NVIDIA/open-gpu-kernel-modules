/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GPU_SHARED_DATA_MAP_H
#define GPU_SHARED_DATA_MAP_H

#include "core/core.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "class/cl00de.h"

#include "gpu/timer/tmr.h"

// ****************************************************************************
//                          Type definitions
// ****************************************************************************
typedef struct RusdQueryCache RUSD_QUERY_CACHE;

typedef struct GpuSharedDataMap {
    MEMORY_DESCRIPTOR *pMemDesc;
    NvP64 pMapBuffer;
    NvU64 lastPolledDataMask;
    NvU32 processId;

    TMR_EVENT *pRusdRefreshTmrEvent;

    NV00DE_SHARED_DATA data;

    // Private data to assist metrics query
    RUSD_QUERY_CACHE *pRusdQueryCache;
    NvU8 curGroup;
    NvBool bWorkItemPending;
} GpuSharedDataMap;

/**
 * Start data write, updates seq to indicate write in progress and returns data struct to write into
 *
 * After updating data in the returned NV00DE_SHARED_DATA struct,
 * call gpushareddataWriteFinish to mark data as valid.
 */
NV00DE_SHARED_DATA * gpushareddataWriteStart_INTERNAL(OBJGPU *pGpu, NvU64 offset);

#define gpushareddataWriteStart(pGpu, field) \
    &(gpushareddataWriteStart_INTERNAL(pGpu, NV_OFFSETOF(NV00DE_SHARED_DATA, field))->field)

/**
 * Finish data write, updates seq to indicate write is finished and data is valid.
 */
void gpushareddataWriteFinish_INTERNAL(OBJGPU *pGpu, NvU64 offset);

#define gpushareddataWriteFinish(pGpu, field) \
    gpushareddataWriteFinish_INTERNAL(pGpu, NV_OFFSETOF(NV00DE_SHARED_DATA, field))

#endif /* GPU_SHARED_DATA_MAP_H */

