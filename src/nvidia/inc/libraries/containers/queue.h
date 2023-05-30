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
#ifndef NV_CONTAINERS_QUEUE_H
#define NV_CONTAINERS_QUEUE_H

#include "containers/type_safety.h"
#include "nvtypes.h"
#include "nvmisc.h"
#include "nvport/nvport.h"
#include "utils/nvassert.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAKE_QUEUE_CIRCULAR(queueTypeName, dataType)                         \
    typedef struct queueTypeName##Iter_UNUSED                                \
    {                                                                        \
        NvLength dummyElem;                                                  \
    } queueTypeName##Iter_UNUSED;                                            \
    typedef union queueTypeName                                              \
    {                                                                        \
        Queue real;                                                          \
        CONT_TAG_TYPE(Queue, dataType, queueTypeName##Iter_UNUSED);          \
        CONT_TAG_NON_INTRUSIVE(dataType);                                    \
    } queueTypeName

#define DECLARE_QUEUE_CIRCULAR(queueTypeName)                                \
    typedef struct queueTypeName##Iter_UNUSED queueTypeName##Iter_UNUSED;    \
    typedef union queueTypeName queueTypeName

struct Queue;
struct QueueContext;

typedef void QueueCopyData(NvLength msgSize, NvLength opIdx,
                           struct QueueContext *pCtx, void *pClientData,
                           NvLength count, NvBool bCopyIn);

typedef struct QueueContext {
    QueueCopyData *pCopyData;          // Function performing accesses to queue memory.
    void *pData;                       // Private data.
} QueueContext;

typedef struct Queue {
    NvLength capacity;                 // Queue Capacity
    PORT_MEM_ALLOCATOR *pAllocator;    // Set of functions used for managing queue memory
    void *pData;                       // Queue memory, if managed by pAllocator
    NvLength msgSize;                  // Message size produced by Producer
    NvLength getIdx NV_ALIGN_BYTES(64);// GET index modified by Consumer
    NvLength putIdx NV_ALIGN_BYTES(64);// PUT index modified by Producer
} Queue;

//for future use (more possible queues - just an example, currently only CIRCULAR will get implemented)
typedef enum
{
    QUEUE_TYPE_CIRCULAR = 1,
    //QUEUE_TYPE_LINEAR = 2,
    //QUEUE_TYPE_PRIORITY = 3,
}QUEUE_TYPE;

#define queueInit(pQueue, pAllocator, capacity)                              \
    circularQueueInit_IMPL(&((pQueue)->real), pAllocator,                    \
                  capacity, sizeof(*(pQueue)->valueSize))

#define queueInitNonManaged(pQueue, capacity)                                \
    circularQueueInitNonManaged_IMPL(&((pQueue)->real),                      \
                                     capacity, sizeof(*(pQueue)->valueSize))

#define queueDestroy(pQueue)                                                 \
    circularQueueDestroy_IMPL(&((pQueue)->real))

#define queueCount(pQueue)                                                   \
    circularQueueCount_IMPL(&((pQueue)->real))

#define queueCapacity(pQueue)                                                \
    circularQueueCapacity_IMPL(&((pQueue)->real))

#define queueIsEmpty(pQueue)                                                 \
    circularQueueIsEmpty_IMPL(&((pQueue)->real))

#define queuePush(pQueue, pElements, numElements)                            \
    circularQueuePush_IMPL(&(pQueue)->real,                                  \
        CONT_CHECK_ARG(pQueue, pElements), numElements)

#define queuePushNonManaged(pQueue, pCtx, pElements, numElements)            \
    circularQueuePushNonManaged_IMPL(&(pQueue)->real, pCtx,                  \
        CONT_CHECK_ARG(pQueue, pElements), numElements)

#define queuePeek(pQueue)                                                    \
    CONT_CAST_ELEM(pQueue, circularQueuePeek_IMPL(&((pQueue)->real)), circularQueueIsValid_IMPL)

#define queuePop(pQueue)                                                     \
    circularQueuePop_IMPL(&((pQueue)->real))

#define queuePopAndCopy(pQueue, pCopyTo)                                     \
    circularQueuePopAndCopy_IMPL(&((pQueue)->real),                          \
        CONT_CHECK_ARG(pQueue, pCopyTo))

#define queuePopAndCopyNonManaged(pQueue, pCtx, pCopyTo)                     \
    circularQueuePopAndCopyNonManaged_IMPL(&((pQueue)->real), pCtx,          \
        CONT_CHECK_ARG(pQueue, pCopyTo))

NV_STATUS circularQueueInit_IMPL(Queue *pQueue, PORT_MEM_ALLOCATOR *pAllocator,
                                 NvLength capacity, NvLength msgSize);
NV_STATUS circularQueueInitNonManaged_IMPL(Queue *pQueue, NvLength capacity,
                                           NvLength msgSize);
void circularQueueDestroy_IMPL(Queue *pQueue);
NvLength circularQueueCapacity_IMPL(Queue *pQueue);
NvLength circularQueueCount_IMPL(Queue *pQueue);
NvBool circularQueueIsEmpty_IMPL(Queue *pQueue);
NvLength circularQueuePush_IMPL(Queue *pQueue, void* pElements, NvLength numElements);
NvLength circularQueuePushNonManaged_IMPL(Queue *pQueue, QueueContext *pCtx,
                                          void* pElements, NvLength numElements);
void* circularQueuePeek_IMPL(Queue *pQueue);
void circularQueuePop_IMPL(Queue *pQueue);
NvBool circularQueuePopAndCopy_IMPL(Queue *pQueue, void *pCopyTo);
NvBool circularQueuePopAndCopyNonManaged_IMPL(Queue *pQueue, QueueContext *pCtx,
                                              void *pCopyTo);

NvBool circularQueueIsValid_IMPL(void *pQueue);

#ifdef __cplusplus
}
#endif

#endif // NV_CONTAINERS_QUEUE_H
