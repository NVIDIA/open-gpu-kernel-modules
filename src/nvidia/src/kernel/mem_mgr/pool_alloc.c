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
 * @file pool_alloc.c
 * @brief Defines the interfaces for managing the memory pools used for
 *        allocating and freeing the RM allocations. RM's
 *        internal page directories/tables are NOT managed by PMA and
 *        DO NOT use the interfaces defined in this file.
 */

/* ------------------------------------ Includes ----------------------------------- */
#include "mem_mgr/pool_alloc.h"
#include "mem_mgr/vaspace.h"
#include "gpu/mem_mgr/phys_mem_allocator/phys_mem_allocator.h"
#include "class/cl90f1.h"
#include "mmu/gmmu_fmt.h"
#include "gpu/gpu.h"

/* ------------------------------------ Local Defines ------------------------------ */
#define PMA_CHUNK_SIZE_256G (256ULL * 1024 * 1024 * 1024)
#define PMA_CHUNK_SIZE_512M (512 * 1024 * 1024)
#define PMA_CHUNK_SIZE_4M   (4 * 1024 * 1024)
#define PMA_CHUNK_SIZE_2M   (2 * 1024 * 1024)
#define PMA_CHUNK_SIZE_512K (512 * 1024)
#define PMA_CHUNK_SIZE_256K (256 * 1024)
#define PMA_CHUNK_SIZE_64K  (64 * 1024)

/*! PAGE SIZES FOR DIFFERENT POOL ALLOCATOR LEVELS
 *
 * CONTEXT BUFFER allocations
 *
 * When allocator is used for context buffers three page sizes
 * are supported as follows:
 *
 * For buffers >= 2MB, page size = 2MB
 * For buffers >= 32KB and < 2MB, page size = 64KB
 * For buffers < 32KB, page siez =  4KB
 *
 * PAGE TABLE allocations
 *
 * When the allocator is used for page tables the page sizes
 * supported by different allocator levels are calculated as follows:
 *
 * Pre-Pascal [Big page size = 128K]
 *   Size of a full PD0 (Root)       =  64 KBytes
 *   Size of a full Small Page Table = 256 KBytes
 *   Size of a full Big Page Table   =   8 KBytes
 *
 * Pre-Pascal [Big page size = 64K]
 *   Size of a full PD0 (Root)       = 128 KBytes
 *   Size of a full Small Page Table = 128 KBytes
 *   Size of a full Big Page Table   =   8 KBytes
 *
 * Pascal+
 *   Size of a full PD3 (Root)       =   4 KBytes
 *   Size of a full PD2              =   4 KBytes
 *   Size of a full PD1              =   4 KBytes
 *   Size of a full PD0              =   4 KBytes
 *   Size of a full Small Page Table =   4 KBytes
 *   Size of a full Big Page Table   = 256 Bytes
 *
 */
typedef enum
{
    RM_POOL_IDX_256G,
    RM_POOL_IDX_512M,
    RM_POOL_IDX_2M,
    RM_POOL_IDX_256K,
    RM_POOL_IDX_128K,
    RM_POOL_IDX_64K,
    RM_POOL_IDX_8K,
    RM_POOL_IDX_4K,
    RM_POOL_IDX_256B,
    NUM_POOLS          // This should always be the last entry!
}POOL_IDX;

/*!
 * This array contains the alloction sizes (in bytes) of each pool.
 */
static const NvU64 poolAllocSizes[] = {
    0x4000000000,
    0x20000000, 0x200000, 0x40000, 0x20000, 0x10000, 0x2000, 0x1000, 0x100
};

#define POOL_CONFIG_POOL_IDX       0
#define POOL_CONFIG_CHUNKSIZE_IDX  1

static const NvU64 poolConfig[POOL_CONFIG_MAX_SUPPORTED][POOL_CONFIG_CHUNKSIZE_IDX + 1] = {
     // page size        // chunk size
     { RM_POOL_IDX_256K, PMA_CHUNK_SIZE_512K},  // pool with pageSize = 256K for GMMU_FMT_VERSION_1
     { RM_POOL_IDX_4K,   PMA_CHUNK_SIZE_64K },  // pool with pageSize = 4K for GMMU_FMT_VERSION_2
     { RM_POOL_IDX_256G, PMA_CHUNK_SIZE_256G }, // pool with pageSize = 256G for RM allocated buffers (unused as of blackwell)
     { RM_POOL_IDX_512M, PMA_CHUNK_SIZE_512M }, // pool with pageSize = 512MB for RM allocated buffers (unused as of ampere)
     { RM_POOL_IDX_2M,   PMA_CHUNK_SIZE_4M },   // pool with pageSize = 2MB for RM allocated buffers
     { RM_POOL_IDX_64K,  PMA_CHUNK_SIZE_256K }, // pool with pageSize = 64K for RM allocated buffers
     { RM_POOL_IDX_4K,   PMA_CHUNK_SIZE_64K }   // pool with pageSize = 4K for RM allocated buffers
};

/*!
 *            Locking in the RM internal pool allocator
 *            ===================================
 *
 * - pPoolLock
 *     Mutex (a PORT_MUTEX instance)
 *
 *     The data inside RM_POOL_ALLOC_MEM_RESERVE_INFO is protected from concurrent access
 *     by this mutex. Any function accessing the RM_POOL_ALLOC_MEM_RESERVE_INFO data should
 *     acquire the mutex.
 *     We're using a mutex instead of a spinlock since we are allocating memory inside the
 *     lock. The allocation thread (pmaAllocatePages) may sleep on a semaphore (if scrubbing
 *     is in progress). So, spinlock is not an appropriate choice. The current assumption is that
 *     none of the functions defined here gets called in an interrupt/atomic context. We"ll
 *     assert in portSyncMutexAcquire() if any of this code ever gets called in an atomic
 *     context. The order in which locks are grabbed all the way from the point of entry into RM
 *     to the functions defined here is as follows.
 *
 *     @ref rmMemPoolReserve    API Lock -> pPoolLock (mutex) -> Locks inside PMA.
 *     @ref rmMemPoolAllocate   API Lock -> GPU Lock -> pPoolLock (mutex)
 *     @ref rmMemPoolFree       API Lock -> GPU Lock -> pPoolLock (mutex)
 *     @ref rmMemPoolRelease    API Lock -> GPU Lock -> pPoolLock (mutex)
 */

// State of memory pool
struct RM_POOL_ALLOC_MEM_RESERVE_INFO
{
    /*!
     * Pointer to the PMA object.
     */
    PMA *pPma;

    /*!
     * Mutex to provide exclusive access to the data inside this struct
     */
    PORT_MUTEX *pPoolLock;

    /*!
     * Index of the topmost pool in the hierarchy
     */
    POOL_IDX topmostPoolIndex;

    /*!
     * Size of topmost pool's upstream chunk allocated by PMA.
     */
    NvU64 pmaChunkSize;

    /*!
     * Array of memory pools.
     */
    POOLALLOC *pPool[NUM_POOLS];

    /*!
     * Num of allocations made from the pool.
     */
    NvU64 validAllocCount;

    /*!
     * Skip scrubbing for all allocations made from the pool.
     */
    NvBool bSkipScrub;

    /*!
     * Automatically trim memory pool when allocation is freed.
     */
    NvBool bTrimOnFree;

    /*!
     * Allocate pool in protected memory
     */
    NvBool bProtected;
};

/* ------------------------------------ Static functions --------------------------- */

/*!
 * @brief Used for allocating pages by the upstream allocator for the topmost
 *        pool.
 *
 * @param[in] pCtx     Context for upstream allocator.
 * @param[in] pageSize Page size to use when allocating from PMA
 * @param[in] pPage    Output page handle from upstream.
 *
 * @return NV_STATUS
 */
static NV_STATUS
allocUpstreamTopPool
(
    void             *pCtx,
    NvU64             pageSize,
    NvU64             numPages, 
    POOLALLOC_HANDLE *pPage
)
{
    PMA_ALLOCATION_OPTIONS      allocOptions = {0};
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo;
    NvU64 i, pageBegin;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(NULL != pCtx, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pPage, NV_ERR_INVALID_ARGUMENT);

    pMemReserveInfo = (RM_POOL_ALLOC_MEM_RESERVE_INFO *)pCtx;
    allocOptions.flags = PMA_ALLOCATE_PINNED | PMA_ALLOCATE_PERSISTENT;

    if (pMemReserveInfo->bSkipScrub)
    {
        allocOptions.flags |= PMA_ALLOCATE_NO_ZERO;
    }

    if (pMemReserveInfo->bProtected)
    {
        allocOptions.flags |= PMA_ALLOCATE_PROTECTED_REGION;
    }

    //
    // Some tests fail page table and directory allocation when close to all FB is allocated if we allocate contiguously.
    // For now, we're special-casing this supported pageSize and allocating the 64K pages discontigously.
    // TODO: Unify the codepaths so that all pages allocated discontiguously (not currently supported by PMA)
    //
    if (pageSize == PMA_CHUNK_SIZE_64K)
    {
        NvU64 *pPageStore = portMemAllocNonPaged(sizeof(NvU64) * numPages);
        NV_STATUS status = NV_OK;
        NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
            pmaAllocatePages(pMemReserveInfo->pPma,
                numPages,
                pageSize,
                &allocOptions,
                pPageStore),
            free_mem);

        for (i = 0; i < numPages; i++)
        {
            pPage[i].address = pPageStore[i];
            pPage[i].pMetadata = NULL;
        }
free_mem:
        portMemFree(pPageStore);
        return status;
    }

    allocOptions.flags |= PMA_ALLOCATE_CONTIGUOUS;

    for (i = 0; i < numPages; i++)
    {
        NV_ASSERT_OK_OR_GOTO(status, pmaAllocatePages(pMemReserveInfo->pPma,
            pageSize / PMA_CHUNK_SIZE_64K,
            PMA_CHUNK_SIZE_64K,
            &allocOptions,
            &pageBegin), err);
        pPage[i].address = pageBegin;
        pPage[i].pMetadata = NULL;
    }

    return NV_OK;
err:
    for (;i > 0; i--)
    {
        NvU32 flags = pMemReserveInfo->bSkipScrub ? PMA_FREE_SKIP_SCRUB : 0;
        pmaFreePages(pMemReserveInfo->pPma, &pPage[i - 1].address, 1,
            pageSize, flags);
    }
    return status;
}

/*!
 * @brief Used for allocating pages by the upstream allocator for the lower
 *        pools.
 *
 * @param[in] pCtx     Context for upstream allocator.
 * @param[in] pageSize Only for debugging.
 * @param[in] pPage    Output page handle from upstream.
 *
 * @return NV_STATUS
 */
static NV_STATUS
allocUpstreamLowerPools
(
    void             *pCtx,
    NvU64             pageSize,
    NvU64             numPages,
    POOLALLOC_HANDLE *pPage
)
{
    NV_STATUS status;
    NvU64 i;

    NV_ASSERT_OR_RETURN(NULL != pCtx, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pPage, NV_ERR_INVALID_ARGUMENT);

    for(i = 0; i < numPages; i++)
    {
        NV_ASSERT_OK_OR_GOTO(status,
            poolAllocate((POOLALLOC *)pCtx, &pPage[i]),
            cleanup);
    }
    return NV_OK;
cleanup:
    for(;i > 0; i--)
    {
        poolFree((POOLALLOC *)pCtx, &pPage[i-1]);
    }
    return status;
}

/*!
 * @brief Used for freeing pages by the upstream allocator for the topmost
 *        pool.
 *
 * @param[in] pCtx     Context for upstream allocator.
 * @param[in] pageSize Only for debugging.
 * @param[in] pPage    Page handle of page to be freed.
 *
 * @return
 */
static void
freeUpstreamTopPool
(
    void             *pCtx,
    NvU64             pageSize,
    POOLALLOC_HANDLE *pPage
)
{
    NvU32 flags = 0;
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo;

    NV_ASSERT_OR_RETURN_VOID(NULL != pCtx);
    NV_ASSERT_OR_RETURN_VOID(NULL != pPage);

    // TODO: Replace the direct call to PMA with function pointer.
    pMemReserveInfo = (RM_POOL_ALLOC_MEM_RESERVE_INFO *)pCtx;

    if (pMemReserveInfo->bSkipScrub)
    {
        flags |= PMA_FREE_SKIP_SCRUB;
    }

    pmaFreePages(pMemReserveInfo->pPma, &(pPage->address), 1,
                 pMemReserveInfo->pmaChunkSize, flags);
}

/*!
 * @brief Used for freeing pages by the upstream allocator for the lower
 *        pools.
 *
 * @param[in] pCtx     Context for upstream allocator.
 * @param[in] pageSize Only for debugging.
 * @param[in] pPage    Page handle of page to be freed.
 *
 * @return
 */
static void
freeUpstreamLowerPools
(
    void             *pCtx,
    NvU64             pageSize,
    POOLALLOC_HANDLE *pPage
)
{
    NV_ASSERT_OR_RETURN_VOID(NULL != pCtx);
    NV_ASSERT_OR_RETURN_VOID(NULL != pPage);

    poolFree((POOLALLOC *)pCtx, pPage);
}

/*!
 * @brief Increments the refcount whenever an allocation is made
 *        from the pool.
 *
 * @param[in] pMemReserveInfo Pointer to the RM_POOL_ALLOC_MEM_RESERVE_INFO data
 *
 * @return
 */
static void
rmMemPoolAddRef
(
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo
)
{
    NV_ASSERT_OR_RETURN_VOID(NULL != pMemReserveInfo);

    pMemReserveInfo->validAllocCount++;
}

/*!
 * @brief Decrements the refcount whenever an allocation is freed and
 *        returned to the pool.
 *
 * @param[in] pMemReserveInfo Pointer to the RM_POOL_ALLOC_MEM_RESERVE_INFO data
 *
 * @return
 */
static void
rmMemPoolRemoveRef
(
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo
)
{
    NV_ASSERT_OR_RETURN_VOID(NULL != pMemReserveInfo);
    NV_ASSERT_OR_RETURN_VOID(pMemReserveInfo->validAllocCount > 0);

    pMemReserveInfo->validAllocCount--;
}

/*!
 * @brief Gets the number of vaspaces that are being served by the pools.
 *
 * @param[in] pMemReserveInfo Pointer to the RM_POOL_ALLOC_MEM_RESERVE_INFO data
 *
 * @return
 */
static NvU64
rmMemPoolGetRef
(
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo
)
{
    NV_ASSERT_OR_RETURN(NULL != pMemReserveInfo, 0);

    return pMemReserveInfo->validAllocCount;
}

/* -------------------------------------- Public functions ---------------------------------- */

NV_STATUS
rmMemPoolSetup
(
    void                             *pCtx,
    RM_POOL_ALLOC_MEM_RESERVE_INFO  **ppMemReserveInfo,
    POOL_CONFIG_MODE                  configMode
)
{
    NvS32                       poolIndex;
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo;
    NV_STATUS                   status;
    NvU32                       flags = 0;

    if (configMode >= POOL_CONFIG_MAX_SUPPORTED)
    {
        return NV_ERR_INVALID_PARAMETER;
    }

    pMemReserveInfo = (RM_POOL_ALLOC_MEM_RESERVE_INFO *)portMemAllocNonPaged(sizeof(*pMemReserveInfo));
    if (NULL == pMemReserveInfo)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pMemReserveInfo, 0, sizeof(*pMemReserveInfo));

    *ppMemReserveInfo = pMemReserveInfo;

    pMemReserveInfo->pPma = (PMA *)pCtx;

    //
    // poolConfig is a 2D array where each row is a pre-defined configuration mode
    // For example, POOL_CONFIG_GMMU_FMT_1 means this pool is used for allocating PTE/PDE entries for the GMMU_FMT_VERSION_1
    // First column in poolConfig corresponds to topmostPoolIndex for a given config
    // Second column in poolConfig corresponds to chunk size for a given config
    //
    pMemReserveInfo->topmostPoolIndex = poolConfig[configMode][POOL_CONFIG_POOL_IDX];
    pMemReserveInfo->pmaChunkSize     = poolConfig[configMode][POOL_CONFIG_CHUNKSIZE_IDX];

    //
    // The topmost pool is fed pages directly by PMA.
    //
    // Calling into PMA with GPU lock acquired may cause deadlocks in case RM
    // is operating along side UVM. Currently, we don't support UVM on Windows/MODS.
    // So, allow the topmost pool to call into PMA on Windows/MODS. This is not
    // permissible on platforms that support UVM like Linux.
    //
    if (RMCFG_FEATURE_PLATFORM_WINDOWS || RMCFG_FEATURE_PLATFORM_MODS)
    {
        flags = FLD_SET_DRF(_RMPOOL, _FLAGS, _AUTO_POPULATE, _ENABLE, flags);
    }
    else
    {
        flags = FLD_SET_DRF(_RMPOOL, _FLAGS, _AUTO_POPULATE, _DISABLE, flags);
    }
    pMemReserveInfo->pPool[pMemReserveInfo->topmostPoolIndex] = poolInitialize(
                                                 pMemReserveInfo->pmaChunkSize,
                                                 poolAllocSizes[pMemReserveInfo->topmostPoolIndex],
                                                 allocUpstreamTopPool,
                                                 freeUpstreamTopPool,
                                                 (void *)pMemReserveInfo,
                                                 portMemAllocatorGetGlobalNonPaged(),
                                                 flags);
    if (NULL == pMemReserveInfo->pPool[pMemReserveInfo->topmostPoolIndex])
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    //
    // The pools are nested. Starting with the second pool, each is fed
    // pages by the pool immediately above it in hierarchy.
    //
    flags = FLD_SET_DRF(_RMPOOL, _FLAGS, _AUTO_POPULATE, _ENABLE, flags);
    for (poolIndex = pMemReserveInfo->topmostPoolIndex + 1; poolIndex < NUM_POOLS; poolIndex++)
    {
        pMemReserveInfo->pPool[poolIndex] = poolInitialize(
                                                poolAllocSizes[poolIndex - 1],
                                                poolAllocSizes[poolIndex],
                                                allocUpstreamLowerPools,
                                                freeUpstreamLowerPools,
                                                (void *)pMemReserveInfo->pPool[poolIndex - 1],
                                                portMemAllocatorGetGlobalNonPaged(),
                                                flags);
        if (NULL == pMemReserveInfo->pPool[poolIndex])
        {
            status = NV_ERR_NO_MEMORY;
            goto done;
        }
    }

    pMemReserveInfo->pPoolLock = (PORT_MUTEX *)portMemAllocNonPaged(portSyncMutexSize);
    if (NULL == pMemReserveInfo->pPoolLock)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    status = portSyncMutexInitialize(pMemReserveInfo->pPoolLock);
    if (NV_OK != status)
    {
        portMemFree(pMemReserveInfo->pPoolLock);
        pMemReserveInfo->pPoolLock = NULL;
        goto done;
    }

    if ((configMode == POOL_CONFIG_CTXBUF_4K) ||
        (configMode == POOL_CONFIG_CTXBUF_64K) ||
        (configMode == POOL_CONFIG_CTXBUF_2M) ||
        (configMode == POOL_CONFIG_CTXBUF_512M))
    {
        pMemReserveInfo->bTrimOnFree = NV_FALSE;
    }
    else
    {
        pMemReserveInfo->bTrimOnFree = NV_TRUE;
    }
done:
    if (NV_OK != status)
    {
        rmMemPoolDestroy(pMemReserveInfo);
    }
    return status;
}


NV_STATUS
rmMemPoolReserve
(
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo,
    NvU64                        size,
    NvU32                        flags
)
{
    NvU64     numChunks;
    NV_STATUS status = NV_ERR_NO_MEMORY;
    NvBool bPrevSkipScrubState = NV_FALSE;

    NV_ASSERT_OR_RETURN((NULL != pMemReserveInfo), NV_ERR_INVALID_ARGUMENT);

    portSyncMutexAcquire(pMemReserveInfo->pPoolLock);

    if (flags & VASPACE_FLAGS_SKIP_SCRUB_MEMPOOL)
    {
        bPrevSkipScrubState = pMemReserveInfo->bSkipScrub;
        pMemReserveInfo->bSkipScrub = NV_TRUE;
    }

    numChunks = NV_DIV_AND_CEIL(size, pMemReserveInfo->pmaChunkSize);

    // Reserve pages only in the topmost pool.
    if (NULL != pMemReserveInfo->pPool[pMemReserveInfo->topmostPoolIndex])
    {
        status = poolReserve(pMemReserveInfo->pPool[pMemReserveInfo->topmostPoolIndex], numChunks);

        /* Assert should not be fired when either status is NV_OK or status is OOM but retry flag is set */
        NV_ASSERT((status == NV_OK) || ((status == NV_ERR_NO_MEMORY) && (flags & VASPACE_FLAGS_RETRY_PTE_ALLOC_IN_SYS)));
    }

    if (flags & VASPACE_FLAGS_SKIP_SCRUB_MEMPOOL)
    {
        pMemReserveInfo->bSkipScrub = bPrevSkipScrubState;
    }

    portSyncMutexRelease(pMemReserveInfo->pPoolLock);
    return status;
}

NV_STATUS
rmMemPoolAllocate
(
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo,
    RM_POOL_ALLOC_MEMDESC          *pPoolMemDesc
)
{
    POOLALLOC_HANDLE   *pPageHandle = NULL;
    PoolPageHandleList *pPageHandleList = NULL;
    NvS32               poolIndex = 0;
    NvS32               topPool;
    NV_STATUS           status = NV_OK;
    NvU64               allocSize;
    NvU32               freeListLength;
    MEMORY_DESCRIPTOR  *pMemDesc = (MEMORY_DESCRIPTOR*)pPoolMemDesc;
    NvU64              *pPhysicalAddresses = NULL;
    NvU32               numPages = 0;

    NV_ASSERT_OR_RETURN((NULL != pMemReserveInfo), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((NULL != pMemDesc), NV_ERR_INVALID_ARGUMENT);

    topPool = pMemReserveInfo->topmostPoolIndex;
    pPageHandleList = (PoolPageHandleList *)portMemAllocNonPaged(sizeof(*pPageHandleList));
    NV_ASSERT_OR_RETURN(pPageHandleList != NULL, NV_ERR_NO_MEMORY);

    portMemSet(pPageHandleList, 0, sizeof(*pPageHandleList));
    listInit(pPageHandleList, portMemAllocatorGetGlobalNonPaged());

    portSyncMutexAcquire(pMemReserveInfo->pPoolLock);

    poolGetListLength(pMemReserveInfo->pPool[topPool],
                      &freeListLength, NULL, NULL);
    NV_PRINTF(LEVEL_INFO,
        "Total size of memory reserved for allocation = 0x%llx Bytes\n",
        freeListLength * pMemReserveInfo->pmaChunkSize);

    //
    // The onus is on the caller to pass the correct size info after factoring
    // in any alignment requirements. The size after factoring in all alignment
    // requirements is tracked in the ActualSize field. The Size field tracks
    // the requested size and doesn't take any alignment requirements into
    // consideration.
    //
    allocSize = pMemDesc->ActualSize;

    if (allocSize > poolAllocSizes[topPool])
    {
        numPages = NvU64_LO32(NV_DIV_AND_CEIL(allocSize, poolAllocSizes[topPool]));
        poolIndex = topPool;
    }
    else
    {
        for (poolIndex = NUM_POOLS - 1; poolIndex >= topPool; poolIndex--)
        {
            if (allocSize <= poolAllocSizes[poolIndex])
            {
                NV_PRINTF(LEVEL_INFO,
                    "Allocating from pool with alloc size = 0x%llx Bytes\n",
                    poolAllocSizes[poolIndex]);
                break;
            }
        }

        if (poolIndex < 0)
        {
            status = NV_ERR_NO_MEMORY;
            goto done;
        }
    }

    //
    // If allocation request is greater than page size of top level pool then
    // allocate multiple pages from top-level pool
    //
    if (numPages > 1)
    {
        NvU32 index;

        NV_PRINTF(LEVEL_INFO,
            "Allocating from pool with alloc size = 0x%llx Bytes\n",
            poolAllocSizes[topPool] * numPages);

        if (memdescGetContiguity(pMemDesc, AT_GPU))
        {
            status = poolAllocateContig(pMemReserveInfo->pPool[topPool], numPages, pPageHandleList);
            if (status != NV_OK)
            {
                goto done;
            }
            pPageHandle = listHead(pPageHandleList);
            memdescDescribe(pMemDesc, ADDR_FBMEM, pPageHandle->address, pMemDesc->Size);
        }
        else
        {
            pPhysicalAddresses = (NvU64*)portMemAllocNonPaged(sizeof(*pPhysicalAddresses) * numPages);
            if (pPhysicalAddresses == NULL)
            {
                status = NV_ERR_NO_MEMORY;
                goto done;
            }
            portMemSet(pPhysicalAddresses, 0, sizeof(*pPhysicalAddresses) * numPages);

            for (index = 0; index < numPages; index++)
            {
                pPageHandle = listAppendNew(pPageHandleList);
                if (pPageHandle == NULL)
                {
                    status = NV_ERR_NO_MEMORY;
                    NV_ASSERT_OR_GOTO((pPageHandle != NULL), done);
                }
                status = poolAllocate(pMemReserveInfo->pPool[topPool], pPageHandle);
                if (status != NV_OK)
                {
                    //
                    // Remove current pageHandle from the list as its invalid
                    // and we don't want poolFree being called on it as a part of cleanup
                    //
                    listRemove(pPageHandleList, pPageHandle);
                    NV_ASSERT_OR_GOTO(0, done);
                    pPageHandle = NULL;
                }
                pPhysicalAddresses[index] = pPageHandle->address;
                pPageHandle = NULL;
            }
            memdescFillPages(pMemDesc, 0, pPhysicalAddresses, numPages, poolAllocSizes[topPool]);
            portMemFree(pPhysicalAddresses);
            pPhysicalAddresses = NULL;
        }
    }
    else
    {
        pPageHandle = listAppendNew(pPageHandleList);
        NV_ASSERT_OR_GOTO((NULL != pPageHandle), done);

        status = poolAllocate(pMemReserveInfo->pPool[poolIndex], pPageHandle);
        if (status != NV_OK)
        {
            listRemove(pPageHandleList, pPageHandle);
            goto done;
        }

        memdescDescribe(pMemDesc, ADDR_FBMEM, pPageHandle->address, pMemDesc->Size);
        // memdescDescribe() sets Size and ActualSize to same values. Hence, reassigning
        pMemDesc->ActualSize = allocSize;
        pPageHandle = NULL;
    }

    // save list of page handles in memdesc
    pMemDesc->pPageHandleList = pPageHandleList;

    // Refcount the pool.
    rmMemPoolAddRef(pMemReserveInfo);

done:
    portMemFree(pPhysicalAddresses);

    if ((status != NV_OK) && (pPageHandleList != NULL))
    {
        if (poolIndex >= 0)
        {
            PoolPageHandleListIter it = listIterAll(pPageHandleList);
            while (listIterNext(&it))
            {
                poolFree(pMemReserveInfo->pPool[poolIndex], it.pValue);
            }
        }

        listClear(pPageHandleList);
        portMemFree(pPageHandleList);
    }
    portSyncMutexRelease(pMemReserveInfo->pPoolLock);
    return status;
}

/*!
 * @brief Returns any unused nodes from the topmost level of a pool hierarchy
 *        back to PMA.
 *
 * @param[in] pMemReserveInfo Pointer to the RM_POOL_ALLOC_MEM_RESERVE_INFO data
 * @param[in] nodesToPreserve Number of nodes to preserve in the topmost pool
 * @param[in] flags           VASpace flags to skip scrubbing
 *
 * @return
 */
void
rmMemPoolTrim
(
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo,
    NvU32                           nodesToPreserve,
    NvU32                           flags
)
{
    NvBool bPrevSkipScrubState = NV_FALSE;

    NV_ASSERT_OR_RETURN_VOID(NULL != pMemReserveInfo);

    if (flags & VASPACE_FLAGS_SKIP_SCRUB_MEMPOOL)
    {
        bPrevSkipScrubState = pMemReserveInfo->bSkipScrub;
        pMemReserveInfo->bSkipScrub = NV_TRUE;
    }

    poolTrim(pMemReserveInfo->pPool[pMemReserveInfo->topmostPoolIndex],
             nodesToPreserve);

    if (flags & VASPACE_FLAGS_SKIP_SCRUB_MEMPOOL)
    {
        pMemReserveInfo->bSkipScrub = bPrevSkipScrubState;
    }
}

void
rmMemPoolFree
(
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo,
    RM_POOL_ALLOC_MEMDESC          *pPoolAllocMemDesc,
    NvU32                           flags
)
{
    MEMORY_DESCRIPTOR     *pMemDesc = (MEMORY_DESCRIPTOR*)pPoolAllocMemDesc;
    NvS32                  poolIndex = 0;
    NvU64                  allocSize;
    PoolPageHandleListIter it;
    NvU32                  topPool;

    NV_ASSERT_OR_RETURN_VOID(NULL != pMemReserveInfo);
    NV_ASSERT_OR_RETURN_VOID(NULL != pMemDesc);
    NV_ASSERT_OR_RETURN_VOID((pMemDesc->pPageHandleList != NULL) &&
                             (listCount(pMemDesc->pPageHandleList) != 0));

    portSyncMutexAcquire(pMemReserveInfo->pPoolLock);

    //
    // Refcount can be greater than 1 in case of shared vaspaces (as in UVM).
    // In this case, RM's internal PDB may be refcounted and a reference
    // stored internally for later revoke.
    //
    if (pMemDesc->RefCount > 1)
    {
        goto done;
    }

    topPool = pMemReserveInfo->topmostPoolIndex;

    // Use the ActualSize value to look up the pools
    allocSize = pMemDesc->ActualSize;

    //
    // If allocation was greater than page size of top level pool then
    // multiple pages were allocated from top pool and we need to free them all.
    //
    if (allocSize > poolAllocSizes[topPool])
    {
        poolIndex = topPool;
    }
    else
    {
        for (poolIndex = NUM_POOLS - 1; poolIndex >= 0; poolIndex--)
        {
            if ((NULL != pMemReserveInfo->pPool[poolIndex]) &&
                (allocSize <= poolAllocSizes[poolIndex]))
            {
                break;
            }
        }
    }
    NV_ASSERT_OR_GOTO((poolIndex >= 0), done);

    it = listIterAll(pMemDesc->pPageHandleList);
    while (listIterNext(&it))
    {
        poolFree(pMemReserveInfo->pPool[poolIndex], it.pValue);
    }
    listClear(pMemDesc->pPageHandleList);
    portMemFree(pMemDesc->pPageHandleList);
    pMemDesc->pPageHandleList = NULL;

    rmMemPoolRemoveRef(pMemReserveInfo);

    // Trim the topmost pool so that any unused pages are returned to PMA.
    if (pMemReserveInfo->bTrimOnFree)
    {
        rmMemPoolTrim(pMemReserveInfo, 1, flags);
    }
done:
    portSyncMutexRelease(pMemReserveInfo->pPoolLock);
}

void
rmMemPoolRelease
(
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo,
    NvU32                           flags
)
{
    NvS32 poolIndex;
    NvU32 freeListLength;
    NvU32 partialListLength;
    NvU32 fullListLenght;
    NvBool bPrevSkipScrubState = NV_FALSE;

    NV_ASSERT_OR_RETURN_VOID(NULL != pMemReserveInfo);

    portSyncMutexAcquire(pMemReserveInfo->pPoolLock);

    //
    // A refcount equal to zero implies that there are no unfreed page level
    // instances. At this point the lowermost pool should have only its freelist
    // non empty. An unfreed allocation in a lower level pool implies non empty
    // partial lists and full lists in the pools above it. We free the pools
    // from bottom to top so that by the time we come to the topmost pool, all
    // allocations are present in the freelist of the  topmost pool. The topmost
    // pool can then return all the memory back to PMA. The pools can get memory
    //  via a call to  rmMemPoolReserve()
    //
    if (rmMemPoolGetRef(pMemReserveInfo) != 0)
    {
        goto done;
    }

    if (flags & VASPACE_FLAGS_SKIP_SCRUB_MEMPOOL)
    {
        bPrevSkipScrubState = pMemReserveInfo->bSkipScrub;
        pMemReserveInfo->bSkipScrub = NV_TRUE;
    }

    for (poolIndex = NUM_POOLS - 1; poolIndex >= 0; poolIndex--)
    {
        if (NULL != pMemReserveInfo->pPool[poolIndex])
        {
            //
            // Since this function gets called only when validAlloCount is zero,
            // the fullList and the partialList are expected to be empty. All
            // allocations (if any) should be only in the freelist at this point.
            //
            poolGetListLength(pMemReserveInfo->pPool[poolIndex], &freeListLength,
                              &partialListLength, &fullListLenght);
            NV_ASSERT(partialListLength == 0);
            NV_ASSERT(fullListLenght == 0);

            // poolTrim() trims only the freelist.
            poolTrim(pMemReserveInfo->pPool[poolIndex], 0);
        }
    }

    if (flags & VASPACE_FLAGS_SKIP_SCRUB_MEMPOOL)
    {
        pMemReserveInfo->bSkipScrub = bPrevSkipScrubState;
    }

done:
    portSyncMutexRelease(pMemReserveInfo->pPoolLock);
}

void
rmMemPoolDestroy
(
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo
)
{
    NvS32 poolIndex;
    NvU32 freeListLength;
    NvU32 partialListLength;
    NvU32 fullListLenght;

    NV_ASSERT_OR_RETURN_VOID(NULL != pMemReserveInfo);

    NV_ASSERT(rmMemPoolGetRef(pMemReserveInfo) == 0);

    //
    // Always free pools from bottom to top since the lower pools return
    // their pages to the pool just above during free. The topmost pool will
    // return the it's pages back to PMA.
    //
    for (poolIndex = NUM_POOLS - 1; poolIndex >= 0; poolIndex--)
    {
        if (NULL != pMemReserveInfo->pPool[poolIndex])
        {
            poolGetListLength(pMemReserveInfo->pPool[poolIndex], &freeListLength,
                              &partialListLength, &fullListLenght);
            NV_ASSERT(freeListLength == 0);
            NV_ASSERT(partialListLength == 0);
            NV_ASSERT(fullListLenght == 0);

            poolDestroy(pMemReserveInfo->pPool[poolIndex]);
        }
    }

    if (NULL != pMemReserveInfo->pPoolLock)
    {
        portSyncMutexDestroy(pMemReserveInfo->pPoolLock);
        portMemFree(pMemReserveInfo->pPoolLock);
        pMemReserveInfo->pPoolLock = NULL;
    }

    portMemFree(pMemReserveInfo);
    pMemReserveInfo = NULL;
}

NvBool
rmMemPoolIsScrubSkipped
(
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo
)
{
    NV_ASSERT_OR_RETURN(pMemReserveInfo != NULL, NV_FALSE);
    return pMemReserveInfo->bSkipScrub;
}

void
rmMemPoolSkipScrub
(
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo,
    NvBool bSkipScrub
)
{
    NV_ASSERT_OR_RETURN_VOID(pMemReserveInfo != NULL);
    pMemReserveInfo->bSkipScrub = bSkipScrub;
}


NV_STATUS
rmMemPoolGetChunkAndPageSize
(
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo,
    NvU64 *pChunkSize,
    NvU64 *pPageSize
)
{
    NV_ASSERT_OR_RETURN(pMemReserveInfo != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pChunkSize != NULL) && (pPageSize != NULL), NV_ERR_INVALID_ARGUMENT);
    *pChunkSize = pMemReserveInfo->pmaChunkSize;
    *pPageSize = poolAllocSizes[pMemReserveInfo->topmostPoolIndex];
    return NV_OK;
}

void
rmMemPoolAllocateProtectedMemory
(
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemReserveInfo,
    NvBool bProtected
)
{
    NV_ASSERT_OR_RETURN_VOID(pMemReserveInfo != NULL);
    pMemReserveInfo->bProtected = bProtected;
}
