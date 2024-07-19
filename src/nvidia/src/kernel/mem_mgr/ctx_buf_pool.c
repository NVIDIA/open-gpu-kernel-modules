/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file ctx_buf_pool.c
 * @brief This file defines interfaces that act as wrappers around the RM memory
 *        pool interfaces. These interfaces are used for creating RM memory pools for
 *        RM internal allocations like global (engine-specific) and local (context-specific)
 *        context buffers. While client page tables are also RM internal allocations
 *        and use RM memory pools, they DO NOT use interfaces defined in this file.
 */

#include "core/core.h"
#include "core/locks.h"
#include "mem_mgr/ctx_buf_pool.h"
#include "class/cl90f1.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "vgpu/vgpu_events.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/bus/kern_bus.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "gpu/mem_mgr/heap.h"

/*
 * @brief Are memory pools supported for context buffers
 *
 * @param[in] pGpu OBJGPU pointer
 *
 * @return NvBool
 */
NvBool
ctxBufPoolIsSupported
(
    OBJGPU *pGpu
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvBool bCallingContextPlugin;
    NvU32 gfid = GPU_GFID_PF;

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_MOVE_CTX_BUFFERS_TO_PMA))
    {
        NV_PRINTF(LEVEL_INFO, "Ctx buffers not supported in PMA\n");
        return NV_FALSE;
    }

    if (!memmgrIsPmaEnabled(pMemoryManager) || !memmgrIsPmaSupportedOnPlatform(pMemoryManager))
    {
        NV_PRINTF(LEVEL_INFO, "PMA is disabled. Ctx buffers will be allocated in RM reserved heap\n");
        return NV_FALSE;
    }

    // TODO remove when bug ID 3922001 for ap_sim_compute_uvm test case resolved
    if (!IS_SILICON(pGpu) &&
        memmgrBug3922001DisableCtxBufOnSim(pGpu, pMemoryManager) &&
        gpuIsSelfHosted(pGpu))
    {
        NV_PRINTF(LEVEL_INFO, "Ctx buffers not supported on simulation/emulation\n");
        return NV_FALSE;
    }

    if (IS_VIRTUAL(pGpu) || RMCFG_FEATURE_PLATFORM_GSP)
    {
        NV_PRINTF(LEVEL_INFO, "Guest RM/GSP don't support ctx buffers in PMA\n");
        return NV_FALSE;
    }

    //
    // In virtualized env, host RM should use CtxBuffer for all allocations made
    // on behalf of plugin or for PF usages
    //
    NV_ASSERT_OR_RETURN(vgpuIsCallingContextPlugin(pGpu, &bCallingContextPlugin) == NV_OK, NV_FALSE);
    NV_ASSERT_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK, NV_FALSE);
    if (hypervisorIsVgxHyper() && !bCallingContextPlugin && IS_GFID_VF(gfid))
    {
        NV_PRINTF(LEVEL_INFO, "ctx buffers in PMA not supported for allocations host RM makes on behalf of guest\n");
        return NV_FALSE;
    }

    NV_PRINTF(LEVEL_INFO, "Ctx buffer pool enabled. Ctx buffers will be allocated from PMA\n");
    return NV_TRUE;
}

/*
 * @brief Initializes all context buffer pools for a VA space
 *
 * @param[in]  pGpu         OBJGPU pointer
 * @param[in]  pHeap        Pointer to Heap object to whose PMA this pool is tied
 * @param[out] ppCtxBufPool Pointer to context buffer pool
 *
 * @return NV_STATUS
 */
NV_STATUS
ctxBufPoolInit
(
    OBJGPU             *pGpu,
    Heap               *pHeap,
    CTX_BUF_POOL_INFO **ppCtxBufPool
)
{
    NV_STATUS status = NV_OK;
    CTX_BUF_POOL_INFO *pCtxBufPool = NULL;
    NvU32 i, poolConfig;

    NV_ASSERT_OR_RETURN(ppCtxBufPool != NULL, NV_ERR_INVALID_ARGUMENT);

    if (!ctxBufPoolIsSupported(pGpu))
    {
        return NV_OK;
    }

    pCtxBufPool = portMemAllocNonPaged(sizeof(CTX_BUF_POOL_INFO));
    NV_ASSERT_OR_RETURN((pCtxBufPool != NULL), NV_ERR_NO_MEMORY);
    portMemSet(pCtxBufPool, 0, sizeof(CTX_BUF_POOL_INFO));

    //
    // create a mem pool for each page size supported by RM
    // pool corresponding to RM_ATTR_PAGE_SIZE_DEFAULT remains unused
    //
    for (i = 0; i < RM_ATTR_PAGE_SIZE_INVALID; i++)
    {
        switch (i)
        {
            case RM_ATTR_PAGE_SIZE_DEFAULT:
            case RM_ATTR_PAGE_SIZE_4KB:
                poolConfig = POOL_CONFIG_CTXBUF_4K;
                break;
            case RM_ATTR_PAGE_SIZE_BIG:
                poolConfig = POOL_CONFIG_CTXBUF_64K;
                break;
            case RM_ATTR_PAGE_SIZE_HUGE:
                poolConfig = POOL_CONFIG_CTXBUF_2M;
                break;
            case RM_ATTR_PAGE_SIZE_512MB:
                poolConfig = POOL_CONFIG_CTXBUF_512M;
                break;
            case RM_ATTR_PAGE_SIZE_256GB:
                poolConfig = POOL_CONFIG_CTXBUF_256G;
                break;
            default:
                NV_PRINTF(LEVEL_ERROR, "Unsupported page size attr %d\n", i);
                return NV_ERR_INVALID_STATE;
        }
        NV_ASSERT_OK_OR_GOTO(status,
            rmMemPoolSetup((void*)&pHeap->pmaObject, &pCtxBufPool->pMemPool[i],
                           poolConfig),
            cleanup);

        // Allocate the pool in CPR in case of Confidential Compute
        if (gpuIsCCFeatureEnabled(pGpu))
        {
            rmMemPoolAllocateProtectedMemory(pCtxBufPool->pMemPool[i], NV_TRUE);
        }
    }
    NV_PRINTF(LEVEL_INFO, "Ctx buf pool successfully initialized\n");

cleanup:
    if (status != NV_OK)
    {
        if (pCtxBufPool != NULL)
        {
            ctxBufPoolDestroy(&pCtxBufPool);
        }
    }
    *ppCtxBufPool = pCtxBufPool;
    return status;
}

/*
 * @brief Destroys all context buffer pools for a VA space
 *
 * @param[in] ppCtxBufPool Pointer to context buffer pool
 *
 * @return
 */
void
ctxBufPoolDestroy
(
    CTX_BUF_POOL_INFO **ppCtxBufPool
)
{
    NvU32 i;
    CTX_BUF_POOL_INFO *pCtxBufPool;
    NV_ASSERT((ppCtxBufPool != NULL) && (*ppCtxBufPool != NULL));
    if ((ppCtxBufPool == NULL) || (*ppCtxBufPool == NULL))
    {
        NV_PRINTF(LEVEL_ERROR, "Ctx buf pool doesn't exist\n");
        return;
    }

    pCtxBufPool = *ppCtxBufPool;

    for (i = 0; i < RM_ATTR_PAGE_SIZE_INVALID; i++)
    {
        if (pCtxBufPool->pMemPool[i] != NULL)
        {
            rmMemPoolDestroy(pCtxBufPool->pMemPool[i]);
            pCtxBufPool->pMemPool[i] = NULL;
        }
    }
    portMemFree(pCtxBufPool);
    *ppCtxBufPool = NULL;
    NV_PRINTF(LEVEL_INFO, "Ctx buf pool destroyed\n");
}

/*
 * @brief Calculates total amount of memory required for all buffers in each pool and reserves the memory
 *
 * Q. Why do we need a separate function to calculate memory when we already know size of all buffers?
 * A. Memory required to allocate a buffer depends on 3 things: size, page size and alignment.
 *    context buffers don't have alignment requirements so alignment directly depends on page size.
 *    page size is determined based on the size of buffer and RM_ATTR_PAGE_SIZE parameter.
 *    Once we get the page size, we can align the size of buffer accordingly and also route it to correct pool.
 *    Each buffer has different size and attr and so will have different page size and will accordingly go to different pools.
 *    Today, alignment is determined at alloc time(inside heapAlloc) and usually page size in map call.
 *    We use the same algorithm (memmgrDeterminePageSize) below to determine alignment and page size for each buffer.
 *
 * Q. Why do we need a list of context buffers?
 * A. Reserving memory from PMA requires us to drop GPU lock. To determine
 *    page size we need GPU lock since we are accessing some global state here.
 *    So we first calculate memory requirements for each pool based on which buffers
 *    will eventually end up in those pools.
 *    Later we drop GPU lock and reserve memory for each pool.
 *    This avoids acquiring and dropping locks for each buffer and also avoids making a call to PMA
 *    for each buffer.
 *
 * @param[in] pCtxBufPool      Pointer to context buffer pool
 * @param[in] pBufInfoList     List of context buffers to reserve memory for
 * @param[in] bufcount         number of buffers to reserve memory for
 *
 * @return NV_STATUS
 */
NV_STATUS
ctxBufPoolReserve
(
    OBJGPU            *pGpu,
    CTX_BUF_POOL_INFO *pCtxBufPool,
    CTX_BUF_INFO      *pBufInfoList,
    NvU32              bufCount
)
{
    NV_STATUS status = NV_OK;
    NvU64 pageSize;
    NvU32 i;
    NvU64 totalSize[RM_ATTR_PAGE_SIZE_INVALID] = {0};
    NvU64 size;

    NV_ASSERT_OR_RETURN(pCtxBufPool != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pBufInfoList != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(bufCount > 0, NV_ERR_INVALID_ARGUMENT);

    for (i = 0; i < bufCount; i++)
    {
        size = pBufInfoList[i].size;

        // update size and pageSize based on buffer alignment requirement and buffer size
        NV_ASSERT_OK_OR_RETURN(ctxBufPoolGetSizeAndPageSize(pCtxBufPool, pGpu,
            pBufInfoList[i].align, pBufInfoList[i].attr, pBufInfoList[i].bContig, &size, &pageSize));

        //
        // Determine the pool(4K/64K/2M) from where this buffer will eventually
        // get allocated and mark that pool to reserve this memory.
        //
        switch(pageSize)
        {
            case RM_PAGE_SIZE:
                totalSize[RM_ATTR_PAGE_SIZE_4KB] += size;
                break;
            case RM_PAGE_SIZE_64K:
            case RM_PAGE_SIZE_128K:
                totalSize[RM_ATTR_PAGE_SIZE_BIG] += size;
                break;
            case RM_PAGE_SIZE_HUGE:
                totalSize[RM_ATTR_PAGE_SIZE_HUGE] += size;
                break;
            case RM_PAGE_SIZE_512M:
                totalSize[RM_ATTR_PAGE_SIZE_512MB] += size;
                break;
            case RM_PAGE_SIZE_256G:
                totalSize[RM_ATTR_PAGE_SIZE_256GB] += size;
                break;
            default:
                NV_PRINTF(LEVEL_ERROR, "Unrecognized/unsupported page size = 0x%llx\n", pageSize);
                NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_ARGUMENT);
        }
        NV_PRINTF(LEVEL_INFO, "Reserving 0x%llx bytes for buf Id = 0x%x in pool with page size = 0x%llx\n", size, i, pageSize);
    }

    for (i = 0; i < RM_ATTR_PAGE_SIZE_INVALID; i++)
    {
        if (totalSize[i] > 0)
        {
            NV_ASSERT_OK_OR_GOTO(status, rmMemPoolReserve(pCtxBufPool->pMemPool[i], totalSize[i], 0), done);
            NV_PRINTF(LEVEL_INFO, "Reserved 0x%llx bytes in pool with RM_ATTR_PAGE_SIZE_* = 0x%x\n", totalSize[i], i);
        }
    }

done:
    if (status != NV_OK)
    {
        NV_ASSERT_OK(ctxBufPoolTrim(pCtxBufPool));
        NV_PRINTF(LEVEL_ERROR, "Failed to reserve memory. trimming all pools\n");
    }
    return status;

}

/*
 * @brief Trims out additional memory from context buffer pools
 *
 * @param[in] pCtxBufPool Pointer to context buffer pool
 *
 * @return NV_STATUS
 */
NV_STATUS
ctxBufPoolTrim
(
    CTX_BUF_POOL_INFO *pCtxBufPool
)
{
    NvU32 i;
    NV_ASSERT_OR_RETURN(pCtxBufPool != NULL, NV_ERR_INVALID_ARGUMENT);

    for (i = 0; i < RM_ATTR_PAGE_SIZE_INVALID; i++)
    {
        rmMemPoolTrim(pCtxBufPool->pMemPool[i], 0, 0);
        NV_PRINTF(LEVEL_INFO, "Trimmed pool with RM_ATTR_PAGE_SIZE_* = 0x%x\n", i);
    }
    return NV_OK;
}

/*
 * @brief Releases all memory from context buffer pools
 *
 *        If there are pending allocations in any of the pools then
 *        this function just returns early.
 *
 * @param[in] pCtxBufPool Pointer to context buffer pool
 *
 * @return
 */
void
ctxBufPoolRelease
(
    CTX_BUF_POOL_INFO *pCtxBufPool
)
{
    NvU32 i;
    NV_ASSERT(pCtxBufPool != NULL);

    for (i = 0; i < RM_ATTR_PAGE_SIZE_INVALID; i++)
    {
        rmMemPoolRelease(pCtxBufPool->pMemPool[i], 0);
    }
}

/*
 * @brief Allocates memory from context buffer pools
 *
 * @param[in] pCtxBufPool Pointer to context buffer pool
 * @param[in] pMemDesc    Pointer to context buffer memory descriptor
 *
 * @return NV_STATUS
 */
NV_STATUS
ctxBufPoolAllocate
(
    CTX_BUF_POOL_INFO *pCtxBufPool,
    PMEMORY_DESCRIPTOR pMemDesc
)
{
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pPool = NULL;
    NV_ASSERT_OR_RETURN(pCtxBufPool != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_ADDRESS_SPACE addrSpace = memdescGetAddressSpace(pMemDesc);
    if (addrSpace != ADDR_FBMEM)
    {
        NV_PRINTF(LEVEL_ERROR, "ctx buf pool is only used for buffers to be allocated in FB\n"
                               "SYSMEM buffers don't need memory to be pre-reserved in pool\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // If page size is not set, then set it based on actual size of memdesc and its alignment
    NvU64 pageSize = memdescGetPageSize(pMemDesc, AT_GPU);
    if ((pageSize == 0) || (memdescGetContiguity(pMemDesc, AT_GPU)))
    {
        NvU64 newPageSize;
        NV_ASSERT_OK_OR_RETURN(ctxBufPoolGetSizeAndPageSize(pCtxBufPool, pMemDesc->pGpu,
            pMemDesc->Alignment, RM_ATTR_PAGE_SIZE_DEFAULT, memdescGetContiguity(pMemDesc, AT_GPU),
           &pMemDesc->ActualSize, &newPageSize));

        //
        // Update the page size in the memdesc only if it isn't set already.
        // This is ok as we get new page size only if no page size was set or if the buffer is contiguous or both.
        // For physically contig buffers, PA address in the memdesc remains the same irrespective of page size.
        // For such buffers, if pageSize was already set then we don't want to change it as it can change the way
        // buffers are mapped by RM vs HW(this is specifically applicable to main GR ctx buffer)
        //
        if (pageSize == 0)
        {
            memdescSetPageSize(pMemDesc, AT_GPU, newPageSize);
            NV_PRINTF(LEVEL_INFO, "Ctx buffer page size set to 0x%llx\n", newPageSize);
        }
        pageSize = newPageSize;
    }

    // Determine the pool(4K/64K/2M) from where this buffer is to be allocated
    switch(pageSize)
    {
        case RM_PAGE_SIZE:
            pPool = pCtxBufPool->pMemPool[RM_ATTR_PAGE_SIZE_4KB];
            break;
        case RM_PAGE_SIZE_64K:
        case RM_PAGE_SIZE_128K:
            pPool = pCtxBufPool->pMemPool[RM_ATTR_PAGE_SIZE_BIG];
            break;
        case RM_PAGE_SIZE_HUGE:
            pPool = pCtxBufPool->pMemPool[RM_ATTR_PAGE_SIZE_HUGE];
            break;
        case RM_PAGE_SIZE_512M:
            pPool = pCtxBufPool->pMemPool[RM_ATTR_PAGE_SIZE_512MB];
            break;
        case RM_PAGE_SIZE_256G:
            pPool = pCtxBufPool->pMemPool[RM_ATTR_PAGE_SIZE_256GB];
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Unsupported page size = 0x%llx set for context buffer\n", pageSize);
            NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_ARGUMENT);
    }
    NV_ASSERT_OK_OR_RETURN(rmMemPoolAllocate(pPool, (RM_POOL_ALLOC_MEMDESC*)pMemDesc));
    NV_PRINTF(LEVEL_INFO, "Buffer allocated from ctx buf pool with page size = 0x%llx\n", pageSize);
    return NV_OK;
}

/*
 * @brief Frees memory from context buffer pools
 *
 * @param[in] pCtxBufPool Pointer to context buffer pool
 * @param[in] pMemDesc    Pointer to context buffer memory descriptor
 *
 * @return NV_STATUS
 */
NV_STATUS
ctxBufPoolFree
(
    CTX_BUF_POOL_INFO *pCtxBufPool,
    PMEMORY_DESCRIPTOR pMemDesc
)
{
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pPool = NULL;
    NV_ASSERT_OR_RETURN(pCtxBufPool != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);

    NvU64 pageSize = memdescGetPageSize(pMemDesc, AT_GPU);

    //
    // If buffer is contiguous, then it may or may not be allocated from the same pool
    // as its page size. (see ctxBufPoolAllocate)
    // In such case, determine the size of buffer as done during allocation to route the
    // free to correct pool.
    //
    if (memdescGetContiguity(pMemDesc, AT_GPU))
    {
        NvU64 size = pMemDesc->ActualSize;
        NV_ASSERT_OK_OR_RETURN(ctxBufPoolGetSizeAndPageSize(pCtxBufPool, pMemDesc->pGpu,
            pMemDesc->Alignment, RM_ATTR_PAGE_SIZE_DEFAULT, NV_TRUE, &size, &pageSize));
    }

    switch(pageSize)
    {
        case RM_PAGE_SIZE:
            pPool = pCtxBufPool->pMemPool[RM_ATTR_PAGE_SIZE_4KB];
            break;
        case RM_PAGE_SIZE_64K:
        case RM_PAGE_SIZE_128K:
            pPool = pCtxBufPool->pMemPool[RM_ATTR_PAGE_SIZE_BIG];
            break;
        case RM_PAGE_SIZE_HUGE:
            pPool = pCtxBufPool->pMemPool[RM_ATTR_PAGE_SIZE_HUGE];
            break;
        case RM_PAGE_SIZE_512M:
            pPool = pCtxBufPool->pMemPool[RM_ATTR_PAGE_SIZE_512MB];
            break;
        case RM_PAGE_SIZE_256G:
            pPool = pCtxBufPool->pMemPool[RM_ATTR_PAGE_SIZE_256GB];
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Unsupported page size detected for context buffer\n");
            NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_STATE);
    }

    // If scrubber is being skipped by PMA we need to manually scrub this memory
    if (rmMemPoolIsScrubSkipped(pPool))
    {
        OBJGPU *pGpu = pMemDesc->pGpu;
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

        memmgrMemsetInBlocks(pMemoryManager, pMemDesc, 0, 0, pMemDesc->Size,
                                    TRANSFER_FLAGS_NONE, 0 /* Default Block Size */);
    }
    rmMemPoolFree(pPool, (RM_POOL_ALLOC_MEMDESC*)pMemDesc, 0);

    NV_PRINTF(LEVEL_INFO, "Buffer freed from ctx buf pool with page size = 0x%llx\n", pageSize);
    return NV_OK;
}

/*
 * @brief Returns memory pool for global buffers like runlists, GR global buffers etc.
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  bufId         Id to identify the buffer
 * @param[in]  rmEngineType  RM Engine Type
 * @param[out] ppCtxBufPool  Pointer to context buffer pool
 *
 * @return NV_STATUS
 */
NV_STATUS
ctxBufPoolGetGlobalPool
(
    OBJGPU *pGpu,
    CTX_BUF_ID bufId,
    RM_ENGINE_TYPE rmEngineType,
    CTX_BUF_POOL_INFO **ppCtxBufPool
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    CTX_BUF_POOL_INFO *pCtxBufPool = NULL;

    NV_ASSERT_OR_RETURN(ppCtxBufPool != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(RM_ENGINE_TYPE_IS_VALID(rmEngineType), NV_ERR_INVALID_ARGUMENT);

    switch (bufId)
    {
        case CTX_BUF_ID_RUNLIST:
            pCtxBufPool = kfifoGetRunlistBufPool(pGpu, pKernelFifo, rmEngineType);
            break;
        case CTX_BUF_ID_GR_GLOBAL:
        {
            KernelGraphics *pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, RM_ENGINE_TYPE_GR_IDX(rmEngineType));
            NV_ASSERT_OR_RETURN(RM_ENGINE_TYPE_IS_GR(rmEngineType), NV_ERR_INVALID_ARGUMENT);
            pCtxBufPool = kgraphicsGetCtxBufPool(pGpu, pKernelGraphics);
            break;
        }
        default:
            NV_PRINTF(LEVEL_ERROR, "Invalid buf Id = 0x%x requested\n", bufId);
            return NV_ERR_INVALID_ARGUMENT;
    }
    *ppCtxBufPool = pCtxBufPool;
    return NV_OK;
}

/*
 * @brief Get updated buffer size and page size for a context buffer
 *
 * @param[in]      pGpu         OBJGPU pointer
 * @param[in]      alignment    Expected buffer alignment
 * @param[in]      attr         Page size attribute for buffer
 * @param[in]      bContig      Is buffer physically contiguouss
 * @param[in/out]  pSize        Size of buffer
 * @param[out]     pPageSize    Page size for buffer
 *
 * @return NV_STATUS
 */
NV_STATUS
ctxBufPoolGetSizeAndPageSize
(
    CTX_BUF_POOL_INFO *pCtxBufPool,
    OBJGPU            *pGpu,
    NvU64              alignment,
    RM_ATTR_PAGE_SIZE  attr,
    NvBool             bContig,
    NvU64             *pSize,
    NvU64             *pPageSize
)
{
    MemoryManager          *pMemoryManager      = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS               status              = NV_OK;
    NvU64                   pageSize            = 0;
    NvU32                   allocFlags          = 0;
    NvU32                   retAttr             = 0;
    NvU32                   retAttr2            = 0;
    NvU64                   size                = 0;

    NV_ASSERT_OR_RETURN(pSize != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pPageSize != NULL, NV_ERR_INVALID_ARGUMENT);

    size = *pSize;
    retAttr = FLD_SET_DRF(OS32, _ATTR, _LOCATION, _VIDMEM, retAttr);

    switch (attr)
    {
        case RM_ATTR_PAGE_SIZE_DEFAULT:
            retAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _DEFAULT, retAttr);
            break;
        case RM_ATTR_PAGE_SIZE_4KB:
            retAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _4KB, retAttr);
            break;
        case RM_ATTR_PAGE_SIZE_BIG:
            retAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _BIG, retAttr);
            break;
        case RM_ATTR_PAGE_SIZE_HUGE:
            retAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _HUGE, retAttr);
            retAttr2 = FLD_SET_DRF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _2MB, retAttr2);
            break;
        case RM_ATTR_PAGE_SIZE_512MB:
            retAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _HUGE, retAttr);
            retAttr2 = FLD_SET_DRF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _512MB, retAttr2);
            break;
        case RM_ATTR_PAGE_SIZE_256GB:
            retAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _HUGE, retAttr);
            retAttr2 = FLD_SET_DRF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _256GB, retAttr2);
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "unsupported page size attr\n");
            return NV_ERR_NOT_SUPPORTED;
    }

    // Update the size of buffer based on requested alignment
    {
        NvU32 tempAttr = 0;
        NvU64 tempAlign = alignment;

        if (attr == RM_ATTR_PAGE_SIZE_DEFAULT)
            tempAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _4KB, retAttr);
        else
            tempAttr = retAttr;

        status = memmgrAllocDetermineAlignment_HAL(pGpu, pMemoryManager, &size, &tempAlign, 0,
                                                   allocFlags, tempAttr, retAttr2, 0);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Couldn't determine buffer alignment\n");
            DBG_BREAKPOINT();
            return status;
        }
    }

    //
    // If buffer needs to be allocated contiguously then we need to route it to a pool
    // whose chunk size >= buffer size
    //
    if (bContig)
    {
        NvU64 chunkSize = 0;
        NvU32 i;
        for (i = 0; i < RM_ATTR_PAGE_SIZE_INVALID; i++)
        {
            NV_ASSERT_OK_OR_RETURN(rmMemPoolGetChunkAndPageSize(pCtxBufPool->pMemPool[i], &chunkSize, &pageSize));
            if (chunkSize >= size)
            {
                size = chunkSize;
                break;
            }
        }
        if (i == RM_ATTR_PAGE_SIZE_INVALID)
        {
            NV_PRINTF(LEVEL_ERROR, "couldn't find pool with chunksize >= 0x%llx\n", size);
            DBG_BREAKPOINT();
            status = NV_ERR_NO_MEMORY;
            return status;
        }
    }
    else
    {
        // Determine page size based on updated buffer size
        pageSize = memmgrDeterminePageSize(pMemoryManager, 0, size, NVOS32_ATTR_FORMAT_PITCH,
                                           allocFlags, &retAttr, &retAttr2);
    }

    // make sure we get a valid page size and alignment is taken care of
    if ((pageSize == 0) || ((NvU64)pageSize < alignment))
    {
        NV_PRINTF(LEVEL_ERROR, "Incorrect page size determination\n");
        DBG_BREAKPOINT();
        status = NV_ERR_INVALID_STATE;
        return status;
    }

    // Align up buffer size based on page size
    size = NV_ALIGN_UP64(size, pageSize);

    *pPageSize = pageSize;
    *pSize = size;
    NV_PRINTF(LEVEL_INFO, "Buffer updated size = 0x%llx with page size = 0x%llx\n", size, pageSize);
    return status;
}

/*
 * @brief Is scrubbing skipped for allocations in this ctx buf pool
 *
 * @param[in] pCtxBufPool  Pointer to context buffer pool
 *
 * @return NvBool
 */
NvBool
ctxBufPoolIsScrubSkipped
(
    CTX_BUF_POOL_INFO *pCtxBufPool
)
{
    NvU32 i;
    NV_ASSERT_OR_RETURN(pCtxBufPool != NULL, NV_ERR_INVALID_ARGUMENT);
    for (i = 0; i < RM_ATTR_PAGE_SIZE_INVALID; i++)
    {
        if (!rmMemPoolIsScrubSkipped(pCtxBufPool->pMemPool[i]))
            return NV_FALSE;
    }

    return NV_TRUE;
}

/*
 * @brief Set ctx buf pool to skip scrub for all its allocations
 *
 * @param[in] pCtxBufPool  Pointer to context buffer pool
 * @param[in] bSkipScrub   Should scrubbing be skipped
 *
 */
void
ctxBufPoolSetScrubSkip
(
    CTX_BUF_POOL_INFO *pCtxBufPool,
    NvBool             bSkipScrub
)
{
    NvU32 i;
    NV_ASSERT_OR_RETURN_VOID(pCtxBufPool != NULL);
    for (i = 0; i < RM_ATTR_PAGE_SIZE_INVALID; i++)
    {
        rmMemPoolSkipScrub(pCtxBufPool->pMemPool[i], bSkipScrub);
    }
}
