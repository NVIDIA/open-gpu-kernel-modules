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
#include "containers/ringbuf.h"

/*
 * @brief Initialize ring buffer given pre-allocated array for storage
 *
 * @param pBase
 *     Pointer to RingBufBase structure
 * @param logSz
 *     Log base 2 of total size of storage, in # of elements
 * @param arr
 *     Storage for ring buffer
 *
 * @returns NvBool
 *     NV_TRUE if unable to construct, NV_FALSE otherwise
 */
NvBool
ringbufConstruct_IMPL
(
    RingBufBase *pBase,
    NvU64 logSz,
    void *arr
)
{
    pBase->head = 0;
    pBase->tail = 0;
    pBase->logSz = logSz;
    pBase->arr = arr;
    return pBase->arr == NULL;
}

/*
 * @brief Initialize dynamic ring buffer given a PORT_MEM_ALLOCATOR
 *
 * @param pBase
 *     Pointer to RingBufBase structure
 * @param logSz
 *     Log base 2 of total size of storage, in # of elements
 * @param eleSz
 *     Size per element
 * @param pAlloc
 *     Pointer to PORT_MEM_ALLOCATOR
 *
 * @returns NvBool
 *     NV_TRUE if unable to construct, NV_FALSE otherwise
 */
NvBool
ringbufConstructDynamic_IMPL
(
    RingBufBase *pBase,
    NvU64 logSz,
    NvU64 eleSz,
    PORT_MEM_ALLOCATOR *pAlloc
)
{
    NV_ASSERT_OR_RETURN(logSz < 64, NV_TRUE);
    return ringbufConstruct_IMPL(pBase, logSz, PORT_ALLOC(pAlloc, eleSz << logSz));
}

/*
 * @brief Destroy a dynamic ring buffer
 *
 * @param pBase
 *     Pointer to RingBufBase structure
 * @param pAlloc
 *     Pointer to PORT_MEM_ALLOCATOR used to construct Ring Buffer
 *
 * @returns void
 */
void
ringbufDestruct_IMPL
(
    RingBufBase *pBase,
    void *pAlloc
)
{
    if ((pAlloc != NULL) && (pBase->arr != NULL))
    {
        PORT_FREE((PORT_MEM_ALLOCATOR *) pAlloc, pBase->arr);
        pBase->arr = 0;
        pBase->logSz = 0;
    }
}

/*
 * @brief Peek up to *pMax elements from a ring buffer
 *
 * @param pBase
 *     Pointer to RingBufBase structure
 * @param eleSz
 *     Size per element
 * @param pMax
 *     Initially a pointer to number of elements to try to fetch. Returns number of elements actually gotten
 *
 * @returns void *
 *     Pointer to the start of the ringbuffer area containing pMax elements, NULL if *pMax == 0, initially or finally
 */
void *
ringbufPeekN_IMPL
(
    RingBufBase *pBase,
    NvU64 eleSz,
    NvU64 *pMax
)
{
    NvU64 mask = RINGBUF_ARRAY_MASK(pBase);
    NvU8 *ret = &pBase->arr[(pBase->tail & mask)*eleSz];
    NvU64 max = NV_ALIGN_UP64(pBase->tail + 1llu, RINGBUF_ARRAY_SIZE(pBase));
    max = max > pBase->head ? pBase->head : max;
    max -= pBase->tail;

    NV_ASSERT_OR_RETURN(pMax != NULL, NULL);

    max = (max > *pMax) ? *pMax : max;
    *pMax = max;
    return max == 0 ? NULL : ret;
}

/*
 * @brief Pop up to *pMax elements from a ring buffer
 *
 * @param pBase
 *     Pointer to RingBufBase structure
 * @param eleSz
 *     Size per element
 * @param pMax
 *     Initially a pointer to number of elements to try to fetch. Returns number of elements actually gotten
 *
 * @returns void *
 *     Pointer to the start of the ringbuffer area containing pMax elements, NULL if *pMax == 0, initially or finally
 */
void *
ringbufPopN_IMPL
(
    RingBufBase *pBase,
    NvU64 eleSz,
    NvU64 *pMax
)
{
    void *ret = ringbufPeekN_IMPL(pBase, eleSz, pMax);

    NV_CHECK_OR_RETURN(LEVEL_INFO, ret != NULL, NULL);

    pBase->tail += *pMax;
    return ret;
}

/*
 * @brief Append num elements to the ringbuffer
 *
 * @param pBase
 *     Pointer to RingBufBase structure
 * @param eleSz
 *     Size per element
 * @param pEle
 *     Pointer to buffer with num elements
 * @param num
 *     Number of elements
 * @param bOverwrite
 *     Whether to overwrite existing elements in ring buffer
 *
 * @returns NvBool
 *     NV_TRUE if success, NV_FALSE otherwise
 */
NvBool
ringbufAppendN_IMPL
(
    RingBufBase *pBase,
    NvU64 eleSz,
    NvU8 *pEle,
    NvU64 num,
    NvBool bOverwrite
)
{
    if ((RINGBUF_ARRAY_SIZE(pBase) - (bOverwrite ? 0llu : (pBase->head - pBase->tail))) < num)
    {
        return NV_FALSE;
    }

    while (num != 0)
    {
        NvU64 mask = RINGBUF_ARRAY_MASK(pBase);
        NvU8 *pPtr = &pBase->arr[(pBase->head & mask) * eleSz];
        NvU64 max = NV_ALIGN_UP64(pBase->head + 1llu, RINGBUF_ARRAY_SIZE(pBase));
        max -= pBase->head;
        max = (max > num) ? num : max;
        portMemCopy(pPtr, max * eleSz, pEle, max * eleSz);
        pBase->head += max;
        pEle = &pEle[max * eleSz];
        num -= max;
    }
    pBase->tail = (pBase->head - pBase->tail) > (1llu<<pBase->logSz) ? (pBase->head - (1llu<<pBase->logSz)) : pBase->tail;
    return NV_TRUE;
}

/*
 * @brief Get current size of ring buffer
 *
 * @param pBase
 *     Pointer to RingBufBase structure
 *
 * @returns NvU64
 *     Number of elements currently in ring buffer
 */
NvU64
ringbufCurrentSize_IMPL
(
    RingBufBase *pBase
)
{
    return pBase->head - pBase->tail;
}