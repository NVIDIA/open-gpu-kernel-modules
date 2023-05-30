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
#ifndef NV_CONTAINERS_RINGBUF_H
#define NV_CONTAINERS_RINGBUF_H

#include "nvtypes.h"
#include "nvstatus.h"
#include "nvmisc.h"
#include "nvctassert.h"
#include "utils/nvassert.h"
#include "nvport/nvport.h"
#include "containers/type_safety.h"

typedef struct RingBufBase
{
    NvU64 head;
    NvU64 tail;
    NvU64 logSz;
    NvU8 *arr;
} RingBufBase;

#define RINGBUF_ARRAY_SIZE(pBuf) (1llu << ((pBuf)->logSz))

#define RINGBUF_ARRAY_MASK(pBuf) (RINGBUF_ARRAY_SIZE(pBuf) - 1llu)

#define MAKE_RINGBUF(containedType, outputType, logSz)                                                         \
    typedef struct outputType {                                                                                \
        union {                                                                                                \
            RingBufBase base;                                                                                  \
            PORT_MEM_ALLOCATOR *pAllocator;                                                                    \
        };                                                                                                     \
        union {                                                                                                \
            containedType elem[1llu << (logSz)];                                                               \
            NvU8 lgSz[((logSz) < 64) ? (logSz) : -1]; /* Ternary to ensure we get CT error when logSz >= 64 */ \
        };                                                                                                     \
    } outputType

#define MAKE_RINGBUF_DYNAMIC(containedType, outputType) \
    typedef union outputType {                          \
        struct {                                        \
            RingBufBase base;                           \
            PORT_MEM_ALLOCATOR *pAllocator;             \
        };                                              \
        containedType *elem;                            \
    } outputType

#if NV_TYPEOF_SUPPORTED

#define FOR_EACH_IN_RINGBUF(pBuf, idx, ptr)                                                                       \
    {                                                                                                             \
        (idx) = (pBuf)->base.tail;                                                                                \
        while((idx) != (pBuf)->base.head)                                                                         \
        {                                                                                                         \
            (ptr) = (typeof(&((pBuf)->elem)[0])) &(pBuf)->base.arr[((idx) & RINGBUF_ARRAY_MASK(&((pBuf)->base))) * \
                                                                  sizeof(*((pBuf)->elem))];                       \
            (idx)++;

#else

#define FOR_EACH_IN_RINGBUF(pBuf, idx, ptr)                                                                       \
    {                                                                                                             \
        (idx) = (pBuf)->base.tail;                                                                                \
        while((idx) != (pBuf)->base.head)                                                                         \
        {                                                                                                         \
            (ptr) = (void *)&(pBuf)->base.arr[((idx) & RINGBUF_ARRAY_MASK(&((pBuf)->base))) *                     \
                                                                  sizeof(*((pBuf)->elem))];                       \
            (idx)++;

#endif // NV_TYPEOF_SUPPORTED

#define FOR_EACH_END_RINGBUF() \
        }                      \
    }


#define ringbufConstructDynamic(pBuf, logSz, pAlloc) \
    (pBuf->pAllocator = pAlloc, ringbufConstructDynamic_IMPL(&((pBuf)->base), logSz, sizeof(*((pBuf)->elem)), pAlloc))

#define ringbufConstruct(pBuf) \
    ringbufConstruct_IMPL(&((pBuf)->base), sizeof((pBuf)->lgSz),  (void*)((pBuf)->elem))

#define ringbufDestruct(pBuf) \
    ringbufDestruct_IMPL(&((pBuf)->base), ((void*)&((pBuf)->pAllocator)) == ((void*)&((pBuf)->base)) ? NULL : ((pBuf)->pAllocator) )

#if NV_TYPEOF_SUPPORTED

#define ringbufPopN(pBuf, pMax) \
    (typeof(&((pBuf)->elem)[0])) ringbufPopN_IMPL(&((pBuf)->base), sizeof(*((pBuf)->elem)), pMax)

#define ringbufPeekN(pBuf, pMax) \
    (typeof(&((pBuf)->elem)[0])) ringbufPeekN_IMPL(&((pBuf)->base), sizeof(*((pBuf)->elem)), pMax)

#define ringbufAppendN(pBuf, pEles, num, bOver) \
    ringbufAppendN_IMPL(&((pBuf)->base), sizeof(*((pBuf)->elem)), (NvU8*)pEles, num, bOver)

#define ringbufPop(pBuf) \
    (typeof(&((pBuf)->elem)[0])) ringbufPop_IMPL(&((pBuf)->base), sizeof(*((pBuf)->elem)))

#define ringbufPeek(pBuf) \
    (typeof(&((pBuf)->elem)[0])) ringbufPeek_IMPL(&((pBuf)->base), sizeof(*((pBuf)->elem)))

#else

#define ringbufPopN(pBuf, pMax) \
    (void *)ringbufPopN_IMPL(&((pBuf)->base), sizeof(*((pBuf)->elem)), pMax)

#define ringbufPeekN(pBuf, pMax) \
    (void *)ringbufPeekN_IMPL(&((pBuf)->base), sizeof(*((pBuf)->elem)), pMax)

#define ringbufAppendN(pBuf, pEles, num, bOver) \
    ringbufAppendN_IMPL(&((pBuf)->base), sizeof(*((pBuf)->elem)), (NvU8*)pEles, num, bOver)

#define ringbufPop(pBuf) \
    (void *)ringbufPop_IMPL(&((pBuf)->base), sizeof(*((pBuf)->elem)))

#define ringbufPeek(pBuf) \
    (void *)ringbufPeek_IMPL(&((pBuf)->base), sizeof(*((pBuf)->elem)))

#endif // NV_TYPEOF_SUPPORTED

#define ringbufAppend(pBuf, pEle, bOver) \
    ringbufAppend_IMPL(&((pBuf)->base), sizeof(*((pBuf)->elem)), (NvU8*)pEle, bOver)

#define ringbufCurrentSize(pBuf) \
    ringbufCurrentSize_IMPL(&((pBuf)->base))

NvBool ringbufConstructDynamic_IMPL(RingBufBase *pBase, NvU64 logSz, NvU64 eleSz, PORT_MEM_ALLOCATOR *pAlloc);
NvBool ringbufConstruct_IMPL(RingBufBase *pBase, NvU64 logSz, void *arr);
void ringbufDestruct_IMPL(RingBufBase *pBase, void *alloc);
void *ringbufPopN_IMPL(RingBufBase *pBase, NvU64 eleSz, NvU64 *pMax);
void *ringbufPeekN_IMPL(RingBufBase *pBase, NvU64 eleSz, NvU64 *pMax);
NvBool ringbufAppendN_IMPL(RingBufBase *pBase, NvU64 eleSz, NvU8 *pEle, NvU64 num, NvBool bOverwrite);
NvU64 ringbufCurrentSize_IMPL(RingBufBase *pBase);

static inline void *ringbufPeek_IMPL(RingBufBase *pBase, NvU64 eleSz)
{
    NvU64 max = 1;
    return ringbufPeekN_IMPL(pBase, eleSz, &max);
}
static inline void *ringbufPop_IMPL(RingBufBase *pBase, NvU64 eleSz)
{
    NvU64 max = 1;
    return ringbufPopN_IMPL(pBase, eleSz, &max);
}
static inline NvBool ringbufAppend_IMPL(RingBufBase *pBase, NvU64 eleSz, NvU8 *pEle, NvBool bOverwrite)
{
    return ringbufAppendN_IMPL(pBase, eleSz, pEle, 1, bOverwrite);
}
#endif
