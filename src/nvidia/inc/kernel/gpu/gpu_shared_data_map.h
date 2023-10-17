/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "tmr.h"

// ****************************************************************************
//                          Type definitions
// ****************************************************************************
typedef struct RusdQueryCache RUSD_QUERY_CACHE;

typedef struct GpuSharedDataMap {
    MEMORY_DESCRIPTOR *pMemDesc;
    NvP64 pMapBuffer;
    NvP64 pMapBufferPriv;
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
 * Start data write, returns data struct to write into
 *
 * After updating data in the returned NV00DE_SHARED_DATA struct,
 * call gpushareddataWriteFinish to push the new data into the user mapping
 */
NV00DE_SHARED_DATA * gpushareddataWriteStart(OBJGPU *pGpu);

// Finish data write, pushes data cached by above into mapped data
void gpushareddataWriteFinish(OBJGPU *pGpu);

// Unmap and free memory in GpuSharedDataMap
void gpushareddataDestroy(OBJGPU *pGpu);

#endif /* GPU_SHARED_DATA_MAP_H */

