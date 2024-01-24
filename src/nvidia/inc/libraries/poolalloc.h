/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file poolalloc.h
 * @brief This file contains the interfaces for pool allocator.
 * A chained sub-allocator originally designed to sub-allocate GPU
 * frame buffer given out by PMA (physical memory allocator).
 *
 * The only requirement of a node in the chained allocator is that the ratio
 * between upSTreamPageSize and allocPageSize is less or equal to 64.
 *
 * @bug Make more abstract -- fix up the variable names
 */


#ifndef _NV_POOLALLOC_H_
#define _NV_POOLALLOC_H_

#include "nvtypes.h"
#include "nvstatus.h"
#include "nvport/nvport.h"
#include "containers/list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct poolnode POOLNODE;

/*!
 * Each node corresponds to one page of upStreamPageSize
 * The pool allocator sub-allocates from each of these pages.
 */
struct poolnode
{
    NvU64    pageAddr;     // Address of the page to sub-allocate
    NvU64    bitmap;       // The bit map for this page. Only used if the
                           // node represents a partially allocated node
    POOLNODE *pParent;     // The upstream pool node in case this node is
                           // allocated from the upper pool.
    ListNode node;         // For intrusive lists.
};

MAKE_INTRUSIVE_LIST(PoolNodeList, POOLNODE, node);

/*!
 * The handle contains a generic metadata field that is needed for fast
 * access. In the case of a linked list implementation of the pool allocator,
 * the metadata is the pointer to the node that contains the page it was
 * sub-allocated from
 */
typedef struct poolallocHandle
{
    NvU64 address;      // The base address for this chunk
    void  *pMetadata;   // The opaque metadata for storing necessary info
} POOLALLOC_HANDLE;


// non-intrusive list of page handles
MAKE_LIST(PoolPageHandleList, POOLALLOC_HANDLE);

/*!
 * @brief Callback function to upstream allocators for allocating new pages
 *
 * This function can allocate multiple pages at a time
 *
 * @param[in]   ctxPtr      Provides context to upstream allocator
 * @param[in]   pageSize    Size of page to ask for from upstream
 * @param[in]   numPages    Number of pages to allocate
 * @param[out]  pPage       The output page handle from upstream
 *
 * @return NV_OK            if successfully allocated NvF32 totalTest, doneTest, failTest; the page
 *         NV_ERR_NO_MEMORY if allocator cannot allocate enough backing
 *         NV_ERR_BAD_PARAM if any parameter is invalid or size info is not
 *                          multiple of SMALLEST_PAGE_SIZE
 *
 */
typedef NV_STATUS (*allocCallback_t)(void *ctxPtr, NvU64 pageSize,
                   NvU64 numPages, POOLALLOC_HANDLE *pPage);

/*!
 * @brief Callback function to upstream allocators for freeing unused pages
 *
 * This function only allocate 1 page at a time right now
 *
 * @param[in]   ctxPtr      Provides context to upstream allocator
 * @param[in]   pageSize    Not really needed. For debugging only
 * @param[in]   pPage       The input page handle to be freed
 *
 */
typedef void (*freeCallback_t)(void *ctxPtr, NvU64 pageSize, POOLALLOC_HANDLE *pPage);

/*!
 * Structure representing a pool.
 */
typedef struct poolalloc
{
    PoolNodeList       freeList;         // List of nodes representing free pages
    PoolNodeList       fullList;         // List of nodes representing fully allocated pages
    PoolNodeList       partialList;      // List of nodes representing partially allocated pages

    PORT_MEM_ALLOCATOR *pAllocator;

    struct
    {
        allocCallback_t allocCb;       // Callback to upstream allocator
        freeCallback_t  freeCb;        // Callback to free pages
        void            *pUpstreamCtx; // The context to pass to upstream allocator
    } callBackInfo;

    NvU64              upstreamPageSize; // Page size for upstream allocations
    NvU64              allocPageSize;    // Page size to give out
    NvU32              ratio;            // Ratio == upstreamPageSize / allocPageSize
    NvU32              flags;            // POOLALLOC_FLAGS_*
} POOLALLOC;


/*!
 * Dump the lists maintained by the pools.
 */
void poolAllocPrint(POOLALLOC *pPool);

/*!
 *  If _AUTO_POPULATE is set to ENABLE then poolAllocate will call upstream function to repopulate
 *  the pool when it runs out of memory. If set to DISABLE, poolAllocate will fail when it runs out of memory
 *  By default this is disabled as for usecases like page tables or context buffers since upstream function can call
 *  into PMA with GPU lock held which has a possibility of deadlocking
 */
#define NV_RMPOOL_FLAGS_AUTO_POPULATE         1:0
#define NV_RMPOOL_FLAGS_AUTO_POPULATE_DEFAULT 0x0
#define NV_RMPOOL_FLAGS_AUTO_POPULATE_DISABLE 0x0
#define NV_RMPOOL_FLAGS_AUTO_POPULATE_ENABLE  0x1

/*!
 * @brief This function initializes a pool allocator object
 *
 * This function establishes a link from this allocator to its upstream
 * allocator by registering a callback function that lazily allocates memory
 * if needed.
 *
 * @param[in]   upstreamPageSize    The page size granularity managed by
 *                                  the allocator
 * @param[in]   allocPageSize       The page size to hand out
 * @param[in]   allocCb             The allocation callback function
 * @param[in]   freeCb              The free callback function
 * @param[in]   pUpstreamCtxPtr     The context pointer for the upstream
 *                                  allocator, passed back on callback
 * @param[in]   mallocFun           The allocator for internal strutures
 * @param[in]   freeFun             The free for internal structures
 * @param[in]   pAllocCtxPtr        The context pointer for the special
 *                                  allocator
 * @param[in]   flags               POOLALLOC_FLAGS_*
 *
 * @return A pointer to a POOLALLOC structure if the initialization
 * succeeded; NULL otherwise
 *
 */

POOLALLOC *poolInitialize(NvU64 upstreamPageSize, NvU64 allocPageSize,
    allocCallback_t allocCb, freeCallback_t freeCb, void *pUpstreamCtxPtr,
    PORT_MEM_ALLOCATOR *pAllocator, NvU32 flags);


/*!
 * @brief Reserves numPages from upstream allocator. After the call
 * freeListSize will equal to/greater than numPages.
 *
 * Since it will call into the upstream allocator, the page size of those
 * pages will be the upstream page size.
 *
 * @param[in]   pPool       The pool allocator
 * @param[out]  numPages    Number of pages to reserve
 *
 * @return NV_OK            if successful
 *         NV_ERR_NO_MEMORY if allocator cannot allocate enough backing
 *         NV_ERR_BAD_PARAM if any parameter is invalid
 *
 */
NV_STATUS poolReserve(POOLALLOC *pPool, NvU64 numPages);


/*!
 * @brief This call will give back any free pages. After the call
 * freeListSize will be less or equal to preserveNum.
 *
 * If the allocator has less or equal number of pages than preserveNum before
 * the call, this function will simply return.
 *
 * @param[in] pPool         The pool allocator to trim from
 * @param[in] preserveNum   The number of pages that we try to preserve
 */
void poolTrim(POOLALLOC *pPool, NvU64 preserveNum);


/*!
 * @brief This function allocates memory from the allocator and returns one
 * page of the fixed allocPageSize as specified in the initialization function
 *
 * The implementation does not guarantee the allocated pages are contiguous.
 * Although there is no potential synchronization issues, if two allocation
 * happen to lie on upstream page bundaries, the allocation will most likely
 * be discontiguous.
 *
 * This function will also callback to upstream allocator to get more pages if
 * it does not have enough pages already reserved.
 *
 * @param[in]   pPool       The pool allocator
 * @param[out]  pPageHandle The allocation handle that contains address and
 *                          metadata for optimization
 *
 * @return NV_OK            if successful
 *         NV_ERR_NO_MEMORY if allocator cannot allocate enough backing
 *         NV_ERR_BAD_PARAM if any parameter is invalid
 */
NV_STATUS poolAllocate(POOLALLOC *pPool, POOLALLOC_HANDLE *pPageHandle);


/*!
 * @brief This function allocates memory from the allocator and returns numPages
 * of the fixed allocPageSize as specified in the initialization function
 *
 * These pages are allocated contiguously and the single start address is returned.
 * Although there is no potential synchronization issues, if two allocation
 * happen to lie on upstream page bundaries, the allocation will most likely
 * be discontiguous.
 *
 * This function will not callback to upstream allocator to get more pages as
 * this is relying on a single chunk of free pages to make contiguous allocations.
 * So the max number of pages that can be allocated contiguously is the number of pages
 * fit in upstream page size i.e the "ratio" of this pool
 *
 * @param[in]   pPool           The pool allocator
 * @param[in]   numPages        The number of pages requested to be allocated
 * @param[out]  pPageHandleList The allocation handles that contain addresses and
 *                              metadata for optimization
 *
 * @return NV_OK            if successful
 *         NV_ERR_NO_MEMORY if allocator cannot allocate enough backing
 *         NV_ERR_BAD_PARAM if any parameter is invalid
 */
NV_STATUS poolAllocateContig(POOLALLOC *pPool, NvU32 numPages, PoolPageHandleList *pPageHandleList);

/*!
 * @brief This function frees the page based on the allocPageSize
 *
 * @param[in]   pPool       The pool allocator
 * @param[out]  pPageHandle The allocation handle that contains address and
 *                          metadata for optimization
 *
 */
void poolFree(POOLALLOC *pPool, POOLALLOC_HANDLE *pPageHandle);


/*!
 * @brief Destroys the pool allocator and frees memory
 */
void poolDestroy(POOLALLOC *pPool);

/*!
 * @briefs Returns the lengths of a pool's lists
 */
void poolGetListLength(POOLALLOC *pPool, NvU32 *pFreeListLength,
                       NvU32 *pPartialListLength, NvU32 *pFullListLength);

#ifdef __cplusplus
}
#endif

#endif /* _NV_POOLALLOC_H_ */
