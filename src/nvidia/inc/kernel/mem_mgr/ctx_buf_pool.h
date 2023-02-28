/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _CTX_BUF_POOL_H_
#define _CTX_BUF_POOL_H_

/*!
 * @file ctx_buf_pool.h
 *
 * @brief Describes interfaces used for creating RM memory pools for
 *        RM internal allocations like global (engine-specific) and
 *        local (context-specific) context buffers.
 */

/* ---------------------------------Includes ------------------------------------ */
#include "core/core.h"
#include "mem_mgr/vaspace.h"
#include "mem_mgr/pool_alloc.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "kernel/gpu/gpu_engine_type.h"

// state of context buffer pools
struct CTX_BUF_POOL_INFO
{
    //
    // Each array index corresponds to a pointer to memory pool with
    // page size corresponding to RM_ATTR_PAGE_SIZE_*
    // Pool corresponding to RM_ATTR_PAGE_SIZE_DEFAULT will be left unused
    //
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemPool[RM_ATTR_PAGE_SIZE_INVALID];
};

// List of all context buffers supported by memory pools
typedef enum CTX_BUF_ID
{
    CTX_BUF_ID_INST_MEM,
    CTX_BUF_ID_RUNLIST,
    CTX_BUF_ID_GR_GLOBAL,
    CTX_BUF_ID_GR_LOCAL
}CTX_BUF_ID;

// context buffer info
struct CTX_BUF_INFO
{
    NvU64 size;
    NvU64 align;
    RM_ATTR_PAGE_SIZE attr;
    NvBool bContig;
};
typedef struct CTX_BUF_INFO CTX_BUF_INFO;

NV_STATUS ctxBufPoolInit(OBJGPU *pGpu, Heap *pHeap, CTX_BUF_POOL_INFO **ppCtxBufPool);
NV_STATUS ctxBufPoolReserve(OBJGPU *pGpu, CTX_BUF_POOL_INFO *pCtxBufPool, CTX_BUF_INFO *pBufInfoList, NvU32 bufCount);
NV_STATUS ctxBufPoolTrim(CTX_BUF_POOL_INFO *pCtxBufPool);
NV_STATUS ctxBufPoolAllocate(CTX_BUF_POOL_INFO *pCtxBufPool, PMEMORY_DESCRIPTOR pMemDesc);
NV_STATUS ctxBufPoolFree(CTX_BUF_POOL_INFO *pCtxBufPool, PMEMORY_DESCRIPTOR pMemDesc);
void      ctxBufPoolRelease(CTX_BUF_POOL_INFO *pCtxBufPool);
void      ctxBufPoolDestroy(CTX_BUF_POOL_INFO **ppCtxBufPool);
NvBool    ctxBufPoolIsSupported(OBJGPU *pGpu);
NV_STATUS ctxBufPoolGetSizeAndPageSize(CTX_BUF_POOL_INFO *pCtxBufPool, OBJGPU *pGpu, NvU64 alignment, RM_ATTR_PAGE_SIZE attr, NvBool bContig, NvU64 *pSize, NvU64 *pPageSize);
NV_STATUS ctxBufPoolGetGlobalPool(OBJGPU *pGpu, CTX_BUF_ID bufId, RM_ENGINE_TYPE rmEngineType, CTX_BUF_POOL_INFO **ppCtxBufPool);
NvBool    ctxBufPoolIsScrubSkipped(CTX_BUF_POOL_INFO *pCtxBufPool);
void      ctxBufPoolSetScrubSkip(CTX_BUF_POOL_INFO *pCtxBufPool, NvBool bSkipScrub);
#endif // _CTX_BUF_POOL_H_
