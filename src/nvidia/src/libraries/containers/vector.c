/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 #include "containers/vector.h"

CONT_VTABLE_DEFN(Vector, vectIterRange_IMPL, NULL);

/**
 * Check if the given index is contained in vector, that is if
 * ((index >= 0) && (index < pVector->size))
 */
static NvBool _vectIndexCheck(Vector *pVector, NvU32 index);

/**
 * Reallocates container.
 *
 * Allocate a memory of 'newSize' bytes, then copy 'copySize' bytes from the old
 * vector memory to the new one.
 */
static NvBool _vectReallocHelper(Vector *pVector, NvU32 newSize, NvU32 copySize);

NV_STATUS vectInit_IMPL
(
    Vector             *pVector,
    PORT_MEM_ALLOCATOR *pAllocator,
    NvU32               capacity,
    NvU32               valueSize
)
{
    NV_ASSERT_OR_RETURN(pVector != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pAllocator != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECKED_ONLY(pVector->versionNumber++);

    portMemSet(pVector, 0, sizeof(*pVector));
    CONT_VTABLE_INIT(Vector, pVector);
    pVector->pAllocator = pAllocator;
    pVector->valueSize  = valueSize;
    pVector->capacity   = capacity;
    pVector->size       = 0;

    if (capacity > 0)
    {
        pVector->pHead = PORT_ALLOC(pVector->pAllocator,
                                    capacity * pVector->valueSize);
        if (NULL == pVector->pHead)
        {
            return NV_ERR_NO_MEMORY;
        }

        portMemSet(pVector->pHead, 0, capacity * pVector->valueSize);
    }
    return NV_OK;
}

void vectDestroy_IMPL(Vector *pVector)
{
    NV_ASSERT_OR_RETURN_VOID(pVector != NULL);
    NV_CHECKED_ONLY(pVector->versionNumber++);

    if (pVector->pAllocator != NULL)
    {
        PORT_FREE(pVector->pAllocator, pVector->pHead);
    }
    pVector->pHead    = NULL;
    pVector->capacity = 0;
    pVector->size     = 0;
}

void vectClear_IMPL(Vector *pVector)
{
    NV_ASSERT_OR_RETURN_VOID(pVector != NULL);
    NV_CHECKED_ONLY(pVector->versionNumber++);
    pVector->size = 0;
}

void *vectAt_IMPL
(
    Vector *pVector,
    NvU32   index
)
{
    NV_ASSERT_OR_RETURN(pVector != NULL, NULL);
    if (pVector->size == 0)
    {
        // possible for empty vectors from vectIterAll, don't assert
        return NULL;
    }
    NV_ASSERT_OR_RETURN(_vectIndexCheck(pVector, index), NULL);
    return (void *)((NvU8 *)pVector->pHead + index * pVector->valueSize);
}

NvU32 vectCapacity_IMPL
(
    Vector *pVector
)
{
    NV_ASSERT_OR_RETURN(pVector != NULL, 0);
    return pVector->capacity;
}

NvU32 vectCount_IMPL
(
    Vector *pVector
)
{
    NV_ASSERT_OR_RETURN(pVector != NULL, 0);
    return pVector->size;
}

NvBool vectIsEmpty_IMPL
(
    Vector *pVector
)
{
    NV_ASSERT_OR_RETURN(pVector != NULL, 0);

    return pVector->size == 0;
}

NV_STATUS vectTrim_IMPL
(
    Vector *pVector,
    NvU32   n
)
{
    NV_ASSERT_OR_RETURN(pVector != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECKED_ONLY(pVector->versionNumber++);

    if (n > pVector->capacity)
    {
        return NV_OK;
    }

    if (n < pVector->size)
    {
        n = pVector->size;
    }

    if (!_vectReallocHelper(pVector,
                            n             * pVector->valueSize,
                            pVector->size * pVector->valueSize))
    {
        return NV_ERR_NO_MEMORY;
    }
    return NV_OK;
}

NV_STATUS vectReserve_IMPL
(
    Vector *pVector,
    NvU32   n
)
{
    NV_ASSERT_OR_RETURN(pVector != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(n > 0, NV_ERR_INVALID_ARGUMENT);
    NV_CHECKED_ONLY(pVector->versionNumber++);

    if (n > pVector->capacity)
    {
        if (!_vectReallocHelper(pVector,
                                n             * pVector->valueSize,
                                pVector->size * pVector->valueSize))
        {
            return NV_ERR_NO_MEMORY;
        }
    }
    return NV_OK;
}

void *vectInsert_IMPL
(
    Vector     *pVector,
    NvU32       index,
    const void *pData
)
{
    void *dst;
    void *src;
    NvU32 i;
    NV_ASSERT_OR_RETURN(pVector != NULL, NULL);
    NV_CHECKED_ONLY(pVector->versionNumber++);
    if (pVector->size != index)
    {
        NV_ASSERT_OR_RETURN(_vectIndexCheck(pVector, index), NULL);
    }
    if (pVector->size + 1 > pVector->capacity)
    {
        // resize the container by the factor of 2, newcapacity = capacity * 2
        NvU32 newCapacity = pVector->capacity == 0 ? 10 : pVector->capacity * 2;

        if (!_vectReallocHelper(pVector,
                                newCapacity   * pVector->valueSize,
                                pVector->size * pVector->valueSize))
            return NULL;
    }

    for (i = pVector->size; i > index; i--)
    {
        dst = (void *)((NvU8 *)pVector->pHead + i       * pVector->valueSize);
        src = (void *)((NvU8 *)pVector->pHead + (i - 1) * pVector->valueSize);
        portMemCopy(dst, pVector->valueSize, src, pVector->valueSize);
    }
    pVector->size++;
    dst = (void *)((NvU8 *)pVector->pHead + index * pVector->valueSize);
    portMemCopy(dst, pVector->valueSize, pData, pVector->valueSize);

    return dst;
}

void vectRemove_IMPL
(
    Vector *pVector,
    NvU32   index
)
{
    void *src;
    void *dst;
    NvU32 i;
    NV_ASSERT_OR_RETURN_VOID(pVector != NULL);
    NV_CHECKED_ONLY(pVector->versionNumber++);
    NV_ASSERT_OR_RETURN_VOID(_vectIndexCheck(pVector, index));

    for (i = index; i < pVector->size - 1; i++)
    {
        dst = (void *)((NvU8 *)pVector->pHead + i       * pVector->valueSize);
        src = (void *)((NvU8 *)pVector->pHead + (i + 1) * pVector->valueSize);
        portMemCopy(dst, pVector->valueSize, src, pVector->valueSize);
    }

    pVector->size--;
}

void *vectAppend_IMPL
(
    Vector     *pVector,
    const void *pData
)
{
    return vectInsert_IMPL(pVector, pVector->size, pData);
}

void *vectPrepend_IMPL
(
    Vector     *pVector,
    const void *pData
)
{
    return vectInsert_IMPL(pVector, 0, pData);
}

VectorIterBase vectIterRange_IMPL
(
    Vector *pVector,
    void   *pFirst,
    void   *pLast
)
{
    VectorIterBase it;
    NvU32          first = ~0U;
    NvU32          last  = ~0U;
    NV_ASSERT(pVector != NULL);

    if (pFirst != NULL)
    {
        first = (NvU32)(((NvU8 *)pFirst - (NvU8 *)pVector->pHead) /
                        pVector->valueSize);
    }
    if (pLast != NULL)
    {
        last = (NvU32)(((NvU8 *)pLast - (NvU8 *)pVector->pHead) /
                       pVector->valueSize);
    }

    NV_CHECKED_ONLY(it.versionNumber = pVector->versionNumber);
    NV_CHECKED_ONLY(it.bValid = NV_TRUE);

    if ((pVector->size == 0) || (pFirst == NULL) || (first >= pVector->size) ||
        (pLast == NULL) || (last >= pVector->size))
    {
        it.pVector    = pVector;
        it.nextIndex  = -1;
        it.prevIndex  = -1;
        it.firstIndex = -1;
        it.lastIndex  = -1;
        it.bForward   = NV_TRUE;
        it.pValue     = NULL;
        return it;
    }
    it.pVector    = pVector;
    it.nextIndex  = first;
    it.prevIndex  = last;
    it.firstIndex = first;
    it.lastIndex  = last;
    it.bForward   = (first <= last);
    it.pValue     = NULL;
    return it;
}

NvBool vectIterNext_IMPL
(
    VectorIterBase  *pIter,
    void           **ppValue
)
{
    NV_ASSERT_OR_RETURN(pIter != NULL, NV_FALSE);
    NV_ASSERT_OR_RETURN(ppValue != NULL, NV_FALSE);

    if (pIter->nextIndex == -1)
    {
        return NV_FALSE;
    }

#if PORT_IS_CHECKED_BUILD
    if (pIter->bValid && !CONT_ITER_IS_VALID(pIter->pVector, pIter))
    {
        NV_ASSERT(CONT_ITER_IS_VALID(pIter->pVector, pIter));
        PORT_DUMP_STACK();
        pIter->bValid = NV_FALSE;
    }
#endif

    *ppValue = (void *)((NvU8 *)pIter->pVector->pHead +
                        pIter->nextIndex * pIter->pVector->valueSize);

    pIter->prevIndex = pIter->bForward ? pIter->nextIndex - 1 :
                                         pIter->nextIndex + 1;

    if (pIter->nextIndex == pIter->lastIndex)
    {
        pIter->nextIndex = -1;
    }
    else
    {
        pIter->nextIndex = pIter->bForward ? pIter->nextIndex + 1 :
                                             pIter->nextIndex - 1;
    }

    return NV_TRUE;
}

NvBool vectIterPrev_IMPL
(
    VectorIterBase  *pIter,
    void           **ppValue
)
{
    NV_ASSERT_OR_RETURN(pIter != NULL, NV_FALSE);
    NV_ASSERT_OR_RETURN(ppValue != NULL, NV_FALSE);

    if (pIter->prevIndex == -1)
    {
        return NV_FALSE;
    }

#if PORT_IS_CHECKED_BUILD
    if (pIter->bValid && !CONT_ITER_IS_VALID(pIter->pVector, pIter))
    {
        NV_ASSERT(CONT_ITER_IS_VALID(pIter->pVector, pIter));
        PORT_DUMP_STACK();
        pIter->bValid = NV_FALSE;
    }
#endif

    *ppValue = (void *)((NvU8 *)pIter->pVector->pHead +
                        pIter->prevIndex * pIter->pVector->valueSize);

    pIter->nextIndex = pIter->bForward ? pIter->prevIndex + 1 :
                                         pIter->prevIndex - 1;

    if (pIter->prevIndex == pIter->firstIndex)
    {
        pIter->prevIndex = -1;
    }
    else
    {
        pIter->prevIndex = pIter->bForward ? pIter->prevIndex - 1 :
                                             pIter->prevIndex + 1;
    }

    return NV_TRUE;
}

static NvBool _vectReallocHelper
(
    Vector *pVector,
    NvU32   newSize,
    NvU32   copySize
)
{
    void *pNewArray;
    void *pCopiedArray;

    NV_ASSERT_OR_RETURN(newSize >= copySize, NV_FALSE);

    pNewArray = PORT_ALLOC(pVector->pAllocator, newSize);
    if (pNewArray == NULL && newSize > 0)
    {
        return NV_FALSE;
    }
    portMemSet(pNewArray, 0, newSize);

    if (copySize > 0)
    {
        pCopiedArray = portMemCopy(pNewArray,      newSize,
                                   pVector->pHead, copySize);
        if (NULL == pCopiedArray)
        {
            NV_ASSERT(pCopiedArray);
            PORT_FREE(pVector->pAllocator, pNewArray);
            return NV_FALSE;
        }

        PORT_FREE(pVector->pAllocator, pVector->pHead);
        pNewArray = pCopiedArray;
    }

    pVector->pHead    = pNewArray;
    pVector->capacity = newSize  / pVector->valueSize;
    pVector->size     = copySize / pVector->valueSize;

    return NV_TRUE;
}

static NvBool _vectIndexCheck
(
    Vector *pVector,
    NvU32   index
)
{
    void *pActualOffset, *pLastElem;

    if (pVector->size == 0)
    {
        return NV_FALSE;
    }

    pActualOffset = (void *)((NvU8 *)pVector->pHead +
                             index * pVector->valueSize);

    pLastElem = (void *)((NvU8 *)pVector->pHead +
                         (pVector->size - 1) * pVector->valueSize);

    return ((void *)pVector->pHead <= pActualOffset &&
            pActualOffset <= (void *)pLastElem);
}

NvBool vectIsValid_IMPL(void *pVect)
{
#if NV_TYPEOF_SUPPORTED
    return NV_TRUE;
#else
    if (CONT_VTABLE_VALID((Vector*)pVect))
        return NV_TRUE;

    NV_ASSERT_FAILED("vtable not valid!");
    CONT_VTABLE_INIT(Vector, (Vector*)pVect);
    return NV_FALSE;
#endif
}
