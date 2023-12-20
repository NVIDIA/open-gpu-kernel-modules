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

#include "poolalloc.h"
#include "nvmisc.h"
#include "utils/nvprintf.h"
#include "utils/nvassert.h"

// Local function declarations.
static NvU32 ntz_64 (NvU64 bits);
static NvU32 countZeros(NvU64 bits);

// Local helpers.
static NvU32
ntz_64
(
    NvU64 bits
)
{
    NvU64 bz, b5, b4, b3, b2, b1, b0;
    NvU64 y;

    y = bits & (~bits+1);
    bz = y ? 0 : 1;
    b5 = (y & 0x00000000FFFFFFFFULL) ? 0 : 32;
    b4 = (y & 0x0000FFFF0000FFFFULL) ? 0 : 16;
    b3 = (y & 0x00FF00FF00FF00FFULL) ? 0 : 8;
    b2 = (y & 0x0F0F0F0F0F0F0F0FULL) ? 0 : 4;
    b1 = (y & 0x3333333333333333ULL) ? 0 : 2;
    b0 = (y & 0x5555555555555555ULL) ? 0 : 1;

    return ((NvU32)(bz + b5 + b4 + b3 + b2 + b1 + b0));
}

static void
_setBitmap
(
    NvU64 *bitmap,
    NvU32 index
)
{
    NvU64 mask;
    mask = ((NvU64)1 << index);
    *bitmap = ((*bitmap) | mask);
}

static NvU32
countZeros
(
    NvU64 bits
)
{
    // Flip the bits to covert zeros to ones.
    bits = ~bits;
    return nvPopCount64(bits);
}


static void
poolListDestroy
(
    PoolNodeList *pList,
    POOLALLOC    *pPool
)
{
    POOLNODE *pNode;

    NV_ASSERT(pList != NULL);

    pNode = listHead(pList);
    while (pNode != NULL)
    {
        POOLALLOC_HANDLE freeHandle;
        POOLNODE         *pNextNode;

        pNextNode = listNext(pList, pNode);
        freeHandle.address = pNode->pageAddr;
        freeHandle.pMetadata = (pNode->pParent != NULL) ? pNode->pParent : pNode;

        (*((pPool->callBackInfo).freeCb))((pPool->callBackInfo).pUpstreamCtx,
            pPool->upstreamPageSize, &freeHandle);

        listRemove(pList, pNode);
        PORT_FREE(pPool->pAllocator, pNode);
        pNode = pNextNode;
    }
}

#if defined(DEBUG_VERBOSE)
static void
poolListPrint
(
    PoolNodeList *pList
)
{
    NvU32            length;
    PoolNodeListIter it;

    length = listCount(pList);
    NV_PRINTF(LEVEL_NOTICE, "Length = %d\n", length);

    it = listIterAll(pList);
    while (listIterNext(&it))
    {
        NV_PRINTF_EX(POOLALLOC, LEVEL_NOTICE,
                     "=> [pageAddr: 0x%llx, bitmap: 0x%llx]",
                      it.pValue->pageAddr, it.pValue->bitmap);
    }
    NV_PRINTF_EX(POOLALLOC, LEVEL_NOTICE, "\n");
}
#endif // defined(DEBUG_VERBOSE)

static void
allocFreeList
(
    POOLALLOC        *pPool,
    POOLALLOC_HANDLE *pPageHandle
)
{
    POOLNODE *pFirstFree;

    pFirstFree = listHead(&pPool->freeList);
    pFirstFree->bitmap = ~((NvU64)1);

    if(ntz_64(pFirstFree->bitmap) >= pPool->ratio)
    {
        // Move from partial list to full list
        listRemove(&pPool->freeList, pFirstFree);
        listPrependExisting(&pPool->fullList, pFirstFree);
    }
    else
    {
        // Move from free list to partial list.
        listRemove(&pPool->freeList, pFirstFree);
        listPrependExisting(&pPool->partialList, pFirstFree);
    }

    // Construct the page handle.
    pPageHandle->address    = pFirstFree->pageAddr;
    pPageHandle->pMetadata  = pFirstFree;
}

static void
allocPartialList
(
    POOLALLOC        *pPool,
    POOLALLOC_HANDLE *pPageHandle
)
{
    POOLNODE *pFirstPartial;
    NvU32 freeIdx;
    NvU64 bitmap, mask;

    pFirstPartial = listHead(&pPool->partialList);
    bitmap = pFirstPartial->bitmap;
    freeIdx = ntz_64(bitmap);
    mask = ~((NvU64)1 << freeIdx);

    NV_ASSERT(freeIdx < pPool->ratio);
    pFirstPartial->bitmap = bitmap & mask;
    if(ntz_64(pFirstPartial->bitmap) >= pPool->ratio)
    {
        // Move from partial list to full list
        listRemove(&pPool->partialList, pFirstPartial);
        listPrependExisting(&pPool->fullList, pFirstPartial);
    }

    // Construct the page handle
    pPageHandle->address    = pFirstPartial->pageAddr
                                + (freeIdx * pPool->allocPageSize);
    pPageHandle->pMetadata  = pFirstPartial;
}

void
poolAllocPrint
(
    POOLALLOC *pPool
)
{
#if defined(DEBUG_VERBOSE)
    NV_PRINTF(LEVEL_NOTICE, "upstreamPageSize = %lldKB, allocPageSize = %lld%s\n",
              (pPool->upstreamPageSize >> 10),
              (pPool->allocPageSize >> 10) ? pPool->allocPageSize >> 10 :
                                             pPool->allocPageSize,
              (pPool->allocPageSize >> 10) ? "KB" : "B");
    NV_PRINTF_EX(POOLALLOC, LEVEL_NOTICE, "freeList => ");
    poolListPrint(&(pPool->freeList));
    NV_PRINTF_EX(POOLALLOC, LEVEL_NOTICE, "partialList => ");
    poolListPrint(&(pPool->partialList));
    NV_PRINTF_EX(POOLALLOC, LEVEL_NOTICE, "fullList => ");
    poolListPrint(&(pPool->fullList));
#endif // defined(DEBUG_VERBOSE)
}


POOLALLOC *
poolInitialize
(
    NvU64              upstreamPageSize,
    NvU64              allocPageSize,
    allocCallback_t    allocCb,
    freeCallback_t     freeCb,
    void               *ctxPtr,
    PORT_MEM_ALLOCATOR *pAllocator,
    NvU32               flags
)
{
    POOLALLOC *pPool;

    pPool = PORT_ALLOC(pAllocator, sizeof(*pPool));
    if (pPool == NULL)
    {
        return NULL;
    }

    pPool->upstreamPageSize = upstreamPageSize;
    pPool->allocPageSize    = allocPageSize;
    pPool->ratio            = (NvU32)(upstreamPageSize / allocPageSize);
    pPool->flags            = flags;

    (pPool->callBackInfo).allocCb     = allocCb;
    (pPool->callBackInfo).freeCb      = freeCb;
    (pPool->callBackInfo).pUpstreamCtx = ctxPtr;

    pPool->pAllocator = pAllocator;

    listInitIntrusive(&pPool->freeList);
    listInitIntrusive(&pPool->fullList);
    listInitIntrusive(&pPool->partialList);

    NV_PRINTF(LEVEL_INFO, "Initialized pool with upstreamPageSize = %lldB, allocPageSize = %lldB and autoPopulate %s\n",
              pPool->upstreamPageSize, pPool->allocPageSize,
              ((pPool->flags & NV_RMPOOL_FLAGS_AUTO_POPULATE_ENABLE) ? "enabled" : "disabled"));
    return pPool;
}


NV_STATUS
poolReserve
(
    POOLALLOC *pPool,
    NvU64     numPages
)
{
    NvU64            i, freeLength, totalAlloc;
    NV_STATUS status = NV_ERR_NO_MEMORY;
    allocCallback_t  allocCb;
    POOLALLOC_HANDLE *pPageHandle = NULL;
    POOLNODE *pNode = NULL;


    if (pPool == NULL || (pPool->callBackInfo).allocCb == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    freeLength = listCount(&pPool->freeList);
    if (freeLength >= numPages)
    {
        return NV_OK;
    }

    totalAlloc = numPages - freeLength;

    allocCb = (pPool->callBackInfo).allocCb;

    pPageHandle = PORT_ALLOC(pPool->pAllocator, totalAlloc * sizeof(POOLALLOC_HANDLE));
    NV_ASSERT_OR_GOTO(pPageHandle != NULL, free_none);

    NV_ASSERT_OK_OR_GOTO(status, 
        allocCb(pPool->callBackInfo.pUpstreamCtx, pPool->upstreamPageSize,
            totalAlloc, pPageHandle),
        free_page);

    status = NV_ERR_NO_MEMORY;

    for (i = 0; i < totalAlloc; i++)
    {
        pNode = PORT_ALLOC(pPool->pAllocator, sizeof(POOLNODE));
        NV_ASSERT_OR_GOTO(pNode != NULL, free_alloc);

        listPrependExisting(&pPool->freeList, pNode);
        pNode->pageAddr = pPageHandle[i].address;
        pNode->bitmap = NV_U64_MAX;
        pNode->pParent = pPageHandle[i].pMetadata;
    }

    status = NV_OK;
    freeLength = listCount(&pPool->freeList);
    NV_ASSERT(freeLength == numPages);
    goto free_page;
free_alloc:
    for(; i < totalAlloc; i++)
    {
        pPool->callBackInfo.freeCb(pPool->callBackInfo.pUpstreamCtx,
            pPool->upstreamPageSize, &pPageHandle[i]);
    }
free_page:
    PORT_FREE(pPool->pAllocator, pPageHandle);
free_none:
    return status;
}


void
poolTrim
(
    POOLALLOC *pPool,
    NvU64      preserveNum
)
{
    NvU64          i, freeLength;
    freeCallback_t freeCb;

    if (pPool == NULL || (pPool->callBackInfo).freeCb == NULL)
    {
        return;
    }

    freeLength = listCount(&pPool->freeList);
    if (freeLength <= preserveNum)
    {
        return;
    }

    freeCb = (pPool->callBackInfo).freeCb;

    for (i = 0; i < (freeLength - preserveNum); i++)
    {
        POOLNODE         *pNode;
        POOLALLOC_HANDLE freeHandle;

        pNode =  listHead(&pPool->freeList);
        freeHandle.address = pNode->pageAddr;
        freeHandle.pMetadata = (pNode->pParent != NULL) ? pNode->pParent : pNode;

        (*freeCb)((pPool->callBackInfo).pUpstreamCtx,
            pPool->upstreamPageSize, &freeHandle);

        listRemove(&pPool->freeList, pNode);
        PORT_FREE(pPool->pAllocator, pNode);
    }

    freeLength = listCount(&pPool->freeList);
    NV_ASSERT(freeLength == preserveNum);
}



// Allocating from partial and free lists will succeed
NV_STATUS
poolAllocate
(
    POOLALLOC        *pPool,
    POOLALLOC_HANDLE *pPageHandle
)
{
    allocCallback_t allocCb;

    // Trying allocating from the partial list first.
    if (listCount(&pPool->partialList) > 0)
    {
        allocPartialList(pPool, pPageHandle);
        return NV_OK;
    }

    // Nothing left in partial list! Try allocating from free list.
    if (listCount(&pPool->freeList) > 0)
    {
        allocFreeList(pPool, pPageHandle);
        return NV_OK;
    }

    allocCb = (pPool->callBackInfo).allocCb;

    //
    // Nothing left in free list as well!! Populate the pool if it is configured to be auto-populated
    // Once we have free list then allocate from free list.
    //
    if (FLD_TEST_DRF(_RMPOOL, _FLAGS, _AUTO_POPULATE, _ENABLE, pPool->flags))
    {
        if ((*allocCb)(pPool->callBackInfo.pUpstreamCtx, pPool->upstreamPageSize, 1, pPageHandle) == NV_OK)
        {
            POOLNODE *pNode;

            pNode = PORT_ALLOC(pPool->pAllocator, sizeof(*pNode));
            listPrependExisting(&pPool->freeList, pNode);

            pNode->pageAddr = pPageHandle->address;
            pNode->bitmap = NV_U64_MAX;
            pNode->pParent = pPageHandle->pMetadata;

            allocFreeList(pPool, pPageHandle);

            return NV_OK;
        }
    }

    return NV_ERR_NO_MEMORY;
}


NV_STATUS
poolAllocateContig
(
    POOLALLOC *pPool,
    NvU32 numPages,
    PoolPageHandleList *pPageHandleList
)
{
    POOLNODE *pFreeNode, *pPartialNode;
    POOLALLOC_HANDLE *pPageHandle;
    NV_STATUS status = NV_OK;
    NvU32 i;
    PoolPageHandleListIter it;
    NvU64 prevAddr, curAddr;

    // can't allocate more than one upstream chunk
    NV_ASSERT_OR_RETURN(numPages <= pPool->ratio, NV_ERR_INVALID_ARGUMENT);

    // Make sure free chunk is available
    NV_ASSERT_OR_RETURN(listCount(&pPool->freeList) > 0, NV_ERR_INVALID_STATE);

    // allocate first page from free node
    pFreeNode = listHead(&pPool->freeList);
    pPageHandle = listAppendNew(pPageHandleList);
    if (pPageHandle == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    allocFreeList(pPool, pPageHandle);
    prevAddr = pPageHandle->address;
    pPageHandle = NULL;

    // allocate subsequent pages from same node
    // which is now a partial node
    pPartialNode = listHead(&pPool->partialList);
    if (pPartialNode != pFreeNode)
    {
        status = NV_ERR_INVALID_STATE;
        goto cleanup;
    }
    for (i = 1; i < numPages; i++)
    {
        pPageHandle = listAppendNew(pPageHandleList);
        if (pPageHandle == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }
        allocPartialList(pPool, pPageHandle);
        curAddr = pPageHandle->address;
        if (curAddr != (prevAddr + pPool->allocPageSize))
        {
            status = NV_ERR_INVALID_STATE;
            goto cleanup;
        }
        prevAddr = curAddr;
    }

    return NV_OK;
cleanup:

    it = listIterAll(pPageHandleList);
    while (listIterNext(&it))
    {
        poolFree(pPool, it.pValue);
    }
    listClear(pPageHandleList);
    return status;
}

void
poolFree
(
    POOLALLOC        *pPool,
    POOLALLOC_HANDLE *pPageHandle
)
{
    POOLNODE *pNode;
    NvU32    freeIdx;
    NvU64    address, baseAddr;

    address = pPageHandle->address;
    pNode = (POOLNODE *) (pPageHandle->pMetadata);
    baseAddr = pNode->pageAddr;

    freeIdx = (NvU32)((address - baseAddr) / (pPool->allocPageSize));
    _setBitmap(&(pNode->bitmap), freeIdx);

#if defined(DEBUG_VERBOSE)
    poolAllocPrint(pPool);
#endif // defined(DEBUG_VERBOSE)

    // node was in full list and needs to move out of full list
    if ((countZeros(pNode->bitmap) + 1) == pPool->ratio)
    {
        listRemove(&pPool->fullList, pNode);

        // node needs to move to free list
        if (pNode->bitmap == NV_U64_MAX)
        {
            listPrependExisting(&pPool->freeList, pNode);
        }
        else
        {
            listPrependExisting(&pPool->partialList, pNode);
        }
    }
    // Node is in partial list
    else
    {
        // Node needs to move from partial list to free list
        if (pNode->bitmap == NV_U64_MAX)
        {
            listRemove(&pPool->partialList, pNode);
            listPrependExisting(&pPool->freeList, pNode);
        }
    }
}


void
poolDestroy
(
    POOLALLOC *pPool
)
{
    // call back to free all the pages
    poolListDestroy(&pPool->fullList, pPool);
    poolListDestroy(&pPool->partialList, pPool);
    poolListDestroy(&pPool->freeList, pPool);

    PORT_FREE(pPool->pAllocator, pPool);
}

void
poolGetListLength
(
    POOLALLOC *pPool,
    NvU32     *pFreeListLength,
    NvU32     *pPartialListLength,
    NvU32     *pFullListLength
)
{
    NV_ASSERT(pPool != NULL);

    if (pFreeListLength != NULL)
    {
        *pFreeListLength = listCount(&pPool->freeList);
    }
    if (pPartialListLength != NULL)
    {
        *pPartialListLength = listCount(&pPool->partialList);
    }
    if (pFullListLength != NULL)
    {
        *pFullListLength = listCount(&pPool->fullList);
    }
}

