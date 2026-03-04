/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "containers/queue.h"

#define MEM_WR(a, d) portMemCopy((a), sizeof(*(a)), &(d), sizeof(d))
#define MEM_RD(v, a) portMemCopy(&(v), sizeof(v), (a), sizeof(*(a)))

static
NV_STATUS circularQueueInitCommon
(
    Queue                  *pQueue,
    void                   *pData,
    NvLength                capacity,
    PORT_MEM_ALLOCATOR     *pAllocator,
    NvLength                msgSize
)
{
    NV_ASSERT_OR_RETURN(pQueue != NULL, NV_ERR_INVALID_ARGUMENT);

    MEM_WR(&pQueue->pData, pData);
    MEM_WR(&pQueue->pAllocator, pAllocator);
    PORT_MEM_WR64(&pQueue->msgSize, msgSize);
    PORT_MEM_WR64(&pQueue->capacity, capacity);
    PORT_MEM_WR64(&pQueue->getIdx, 0);
    PORT_MEM_WR64(&pQueue->putIdx, 0);

    return NV_OK;
}

static
NvLength queueGetCount(Queue *pQueue)
{
    NvLength get = PORT_MEM_RD64(&pQueue->getIdx);
    NvLength put = PORT_MEM_RD64(&pQueue->putIdx);

    if (put >= get)
    {
        return put - get;
    }
    else
    {
        return put + PORT_MEM_RD64(&pQueue->capacity) - get;
    }
}

static
void managedCopyData(NvLength msgSize,
                     NvLength opIdx,
                     QueueContext *pCtx,
                     void  *pClientData,
                     NvLength count,
                     NvBool bCopyIn)
{
    NvLength size = msgSize * count;
    void *pQueueData = (NvU8 *)pCtx->pData + (opIdx * msgSize);
    void *src = bCopyIn ? pClientData : pQueueData;
    void *dst = bCopyIn ? pQueueData : pClientData;

    portMemCopy(dst, size, src, size);
}

NV_STATUS circularQueueInit_IMPL
(
    Queue                  *pQueue,
    PORT_MEM_ALLOCATOR     *pAllocator,
    NvLength                capacity,
    NvLength                msgSize
)
{
    void *pData = NULL;

    // One element is wasted as no separate count/full/empty state
    // is kept - only indices.
    // Managed queue, can hide this due to owning the buffer and
    // preserve original queue semantics.
    capacity += 1;

    NV_ASSERT_OR_RETURN(pAllocator != NULL, NV_ERR_INVALID_ARGUMENT);

    pData = PORT_ALLOC(pAllocator, capacity * msgSize);
    if (pData == NULL)
        return NV_ERR_NO_MEMORY;

    return circularQueueInitCommon(pQueue, pData, capacity, pAllocator, msgSize);
}

NV_STATUS circularQueueInitNonManaged_IMPL
(
    Queue                  *pQueue,
    NvLength                capacity,
    NvLength                msgSize
)
{
    return circularQueueInitCommon(pQueue, NULL /*pData*/, capacity, NULL /*pAllocator*/, msgSize);
}

void circularQueueDestroy_IMPL(Queue *pQueue)
{
    PORT_MEM_ALLOCATOR *pAllocator;

    NV_ASSERT_OR_RETURN_VOID(NULL != pQueue);

    PORT_MEM_WR64(&pQueue->capacity, 1);
    PORT_MEM_WR64(&pQueue->getIdx, 0);
    PORT_MEM_WR64(&pQueue->putIdx, 0);
    MEM_RD(pAllocator, &pQueue->pAllocator);

    if (pAllocator)
        PORT_FREE(pQueue->pAllocator, pQueue->pData);
}

NvLength circularQueueCapacity_IMPL(Queue *pQueue)
{
    NV_ASSERT_OR_RETURN(NULL != pQueue, 0);

    return PORT_MEM_RD64(&pQueue->capacity) - 1;
}

NvLength circularQueueCount_IMPL(Queue *pQueue)
{
    NV_ASSERT_OR_RETURN(NULL != pQueue, 0);

    return queueGetCount(pQueue);
}

NvBool circularQueueIsEmpty_IMPL(Queue *pQueue)
{
    NV_ASSERT_OR_RETURN(NULL != pQueue, 0);

    return queueGetCount(pQueue) == 0;
}

NvLength circularQueuePushNonManaged_IMPL
(
    Queue *pQueue,
    QueueContext *pCtx,
    void* pElements,
    NvLength numElements
)
{
    void *src;
    NvLength cntLimit = 0;
    NvLength elemToCpy, srcSize;
    NvLength putIdx;
    NvLength msgSize;
    NvLength capacity;

    NV_ASSERT_OR_RETURN(NULL != pQueue, 0);

    putIdx = PORT_MEM_RD64(&pQueue->putIdx);
    msgSize = PORT_MEM_RD64(&pQueue->msgSize);
    capacity = PORT_MEM_RD64(&pQueue->capacity);

    // Calculate the elements to copy
    cntLimit = capacity - queueGetCount(pQueue) - 1;
    if (numElements > cntLimit)
    {
        numElements = cntLimit;
    }

    src = pElements;
    if (numElements > 0)
    {
        NvLength remainingElemToCpy = numElements;

        // We need a max of 2 copies to take care of wrapAround case. See if we have a wrap around
        if ((putIdx + numElements) > capacity)
        {
            // do the extra copy here
            elemToCpy = capacity - putIdx;
            srcSize = msgSize * elemToCpy;

            pCtx->pCopyData(msgSize, putIdx, pCtx, src, elemToCpy, NV_TRUE /*bCopyIn*/);

            // Update variables for next copy
            remainingElemToCpy -= elemToCpy;
            src = (void *)((NvU8 *)src + srcSize);

            putIdx = 0;
        }

        NV_ASSERT(remainingElemToCpy <= capacity - putIdx);

        pCtx->pCopyData(msgSize, putIdx, pCtx, src, remainingElemToCpy, NV_TRUE /*bCopyIn*/);

        // The data must land before index update.
        portAtomicMemoryFenceStore();
        PORT_MEM_WR64(&pQueue->putIdx, (putIdx + remainingElemToCpy) % capacity);
    }

    return numElements;
}

NvLength circularQueuePush_IMPL
(
    Queue *pQueue,
    void* pElements,
    NvLength numElements
)
{
    QueueContext ctx = {0};

    NV_ASSERT_OR_RETURN(pQueue != NULL, NV_FALSE);
    NV_ASSERT_OR_RETURN(pQueue->pAllocator != NULL, NV_FALSE);

    ctx.pCopyData = managedCopyData;
    ctx.pData = pQueue->pData;

    return circularQueuePushNonManaged_IMPL(pQueue, &ctx, pElements, numElements);
}

void* circularQueuePeek_IMPL(Queue *pQueue)
{
    void *top;

    NV_ASSERT_OR_RETURN(pQueue != NULL, 0);
    NV_ASSERT_OR_RETURN(pQueue->pAllocator != NULL, 0);

    if (queueGetCount(pQueue) == 0) return NULL;
    top = (void*)((NvU8*)pQueue->pData + pQueue->getIdx * pQueue->msgSize);
    return top;
}

void circularQueuePop_IMPL(Queue *pQueue)
{
    NvLength getIdx;
    NvLength capacity;

    NV_ASSERT_OR_RETURN_VOID(NULL != pQueue);

    getIdx = PORT_MEM_RD64(&pQueue->getIdx);
    capacity = PORT_MEM_RD64(&pQueue->capacity);

    if (queueGetCount(pQueue) > 0)
    {
        PORT_MEM_WR64(&pQueue->getIdx, (getIdx + 1) % capacity);
    }
}

NvBool circularQueuePopAndCopyNonManaged_IMPL(Queue *pQueue, QueueContext *pCtx, void *pCopyTo)
{
    NvLength capacity;
    NvLength msgSize;

    NV_ASSERT_OR_RETURN(pQueue != NULL, NV_FALSE);

    capacity = PORT_MEM_RD64(&pQueue->capacity);
    msgSize = PORT_MEM_RD64(&pQueue->msgSize);

    if (queueGetCount(pQueue) > 0)
    {
        NvLength getIdx = PORT_MEM_RD64(&pQueue->getIdx);
        pCtx->pCopyData(msgSize, getIdx, pCtx, pCopyTo, 1, NV_FALSE /*bCopyIn*/);

        // Update of index can't happen before we read all the data.
        portAtomicMemoryFenceLoad();

        PORT_MEM_WR64(&pQueue->getIdx, (getIdx + 1) % capacity);

        return NV_TRUE;
    }
    return NV_FALSE;
}

NvBool circularQueuePopAndCopy_IMPL(Queue *pQueue, void *pCopyTo)
{
    QueueContext ctx = {0};

    NV_ASSERT_OR_RETURN(pQueue != NULL, NV_FALSE);
    NV_ASSERT_OR_RETURN(pQueue->pAllocator != NULL, NV_FALSE);

    ctx.pCopyData = managedCopyData;
    ctx.pData = pQueue->pData;

    return circularQueuePopAndCopyNonManaged_IMPL(pQueue, &ctx, pCopyTo);
}


NvBool circularQueueIsValid_IMPL(void *pQueue)
{
    // No vtable for circularQueue
    return NV_TRUE;
}
