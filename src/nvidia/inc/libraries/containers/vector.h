/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef NV_CONTAINERS_VECTOR_H
#define NV_CONTAINERS_VECTOR_H 1

#include "containers/type_safety.h"
#include "nvtypes.h"
#include "nvmisc.h"
#include "nvport/nvport.h"
#include "utils/nvassert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup NV_CONTAINERS_VECTOR Vector
 *
 * @brief Sequence of user-defined values.
 *
 * @details Order of values is not necessarily increasing or sorted, but order is
 * preserved across mutation. Please see
 * https://en.wikipedia.org/wiki/Sequence for a formal definition.
 *
 * - Time Complexity:
 *  * Operations are \b O(1),
 *  * Unless stated otherwise.
 *
 * - Memory Usage:
 *  * \b O(N) memory is required for N values.
 *  * See @ref mem-ownership for further details.
 *
 * - Synchronization:
 *  * \b None. The container is not thread-safe.
 *  * Locking must be handled by the user if required.
 *
 */

#define MAKE_VECTOR(vectTypeName, dataType)                  \
    typedef union vectTypeName##Iter                         \
    {                                                        \
        dataType      *pValue;                               \
        VectorIterBase iter;                                 \
    } vectTypeName##Iter;                                    \
    typedef union vectTypeName                               \
    {                                                        \
        VectorWrapper real;                                  \
        CONT_TAG_TYPE(Vector, dataType, vectTypeName##Iter); \
        CONT_TAG_NON_INTRUSIVE(dataType);                    \
    } vectTypeName

#define DECLARE_VECTOR(vectTypeName)                     \
    typedef union vectTypeName##Iter vectTypeName##Iter; \
    typedef union vectTypeName       vectTypeName

typedef struct Vector Vector;
typedef struct VectorIterBase VectorIterBase;
typedef struct VectorWrapper VectorWrapper;

/**
 * Note that the vector values are NvU32 and Iterator values are NvS32,
 * so in case there is a need for a vector with over ~2 billion entries
 * this might not work.
 */
struct VectorIterBase
{
    void   *pValue;
    Vector *pVector;
    NvS32   nextIndex;
    NvS32   prevIndex;
    NvS32   firstIndex;
    NvS32   lastIndex;
    NvBool  bForward;
#if PORT_IS_CHECKED_BUILD
    NvU32 versionNumber;
    NvBool bValid;
#endif
};

VectorIterBase vectIterRange_IMPL(Vector *pVector, void *pFirst, void *pLast);
CONT_VTABLE_DECL(Vector, VectorIterBase);

struct Vector
{
    CONT_VTABLE_FIELD(Vector);
    void               *pHead;
    PORT_MEM_ALLOCATOR *pAllocator;
    NvU32               valueSize;
    NvU32               capacity;
    NvU32               size;
#if PORT_IS_CHECKED_BUILD
    NvU32 versionNumber;
#endif
};

struct VectorWrapper
{
    Vector base;
};

#define vectInit(pVector, pAllocator, capacity) \
    vectInit_IMPL(&((pVector)->real.base),      \
                  pAllocator,                   \
                  capacity,                     \
                  sizeof(*(pVector)->valueSize))
#define vectDestroy(pVector)  vectDestroy_IMPL(&((pVector)->real.base))
#define vectClear(pVector)    vectClear_IMPL(&((pVector)->real.base))
#define vectCount(pVector)    vectCount_IMPL(&((pVector)->real.base))
#define vectCapacity(pVector) vectCapacity_IMPL(&((pVector)->real.base))
#define vectIsEmpty(pVector)  vectIsEmpty_IMPL(&((pVector)->real.base))
#define vectAt(pVector, index)                                  \
    CONT_CAST_ELEM((pVector),                                   \
                   vectAt_IMPL(&((pVector)->real.base), index), \
                   vectIsValid_IMPL)
#define vectInsert(pVector, index, pValue)                           \
    CONT_CAST_ELEM((pVector),                                        \
                   vectInsert_IMPL(&(pVector)->real.base,            \
                                   index,                            \
                                   CONT_CHECK_ARG(pVector, pValue)), \
                   vectIsValid_IMPL)
#define vectRemove(pVector, index) \
    vectRemove_IMPL(&((pVector)->real.base), index)
#define vectAppend(pVector, pValue)                                  \
    CONT_CAST_ELEM((pVector),                                        \
                   vectAppend_IMPL(&(pVector)->real.base,            \
                                   CONT_CHECK_ARG(pVector, pValue)), \
                   vectIsValid_IMPL)
#define vectPrepend(pVector, pValue)                                  \
    CONT_CAST_ELEM((pVector),                                         \
                   vectPrepend_IMPL(&(pVector)->real.base,            \
                                    CONT_CHECK_ARG(pVector, pValue)), \
                   vectIsValid_IMPL)
#define vectReserve(pVector, size) \
    vectReserve_IMPL(&((pVector)->real.base), size)
#define vectTrim(pVector, size) vectTrim_IMPL(&((pVector)->real.base), size)

#define vectIterAll(pVector) \
    vectIterRangeIndex(pVector, 0, vectCount(pVector) - 1)
#define vectIterRangeIndex(pVector, firstIndex, lastIndex) \
    vectIterRange(pVector,                                 \
                  vectAt(pVector, firstIndex),             \
                  vectAt(pVector, lastIndex))
#define vectIterRange(pVector, pFirst, pLast)        \
    CONT_ITER_RANGE(pVector,                         \
                    &vectIterRange_IMPL,             \
                    CONT_CHECK_ARG(pVector, pFirst), \
                    CONT_CHECK_ARG(pVector, pLast),  \
                    vectIsValid_IMPL)
#define vectIterNext(pIterator) \
    vectIterNext_IMPL(&((pIterator)->iter), (void **)&(pIterator)->pValue)
#define vectIterPrev(pIterator) \
    vectIterPrev_IMPL(&((pIterator)->iter), (void **)&(pIterator)->pValue)

NV_STATUS vectInit_IMPL(Vector             *pVector,
                        PORT_MEM_ALLOCATOR *pAllocator,
                        NvU32               capacity,
                        NvU32               valueSize);
void  vectDestroy_IMPL(Vector *pVector);
void  vectClear_IMPL(Vector *pVector);
NvU32 vectCount_IMPL(Vector *pVector);
NvU32 vectCapacity_IMPL(Vector *pVector);
NvBool vectIsEmpty_IMPL(Vector *pVector);

void *vectAt_IMPL(Vector *pVector, NvU32 index);
void *vectInsert_IMPL(Vector *pVector, NvU32 index, const void *pData);
void  vectRemove_IMPL(Vector *pVector, NvU32 index);
void *vectAppend_IMPL(Vector *pVector, const void *pData);
void *vectPrepend_IMPL(Vector *pvector, const void *pData);

NV_STATUS  vectReserve_IMPL(Vector *pVector, NvU32 n);
NV_STATUS  vectTrim_IMPL(Vector *pvector, NvU32 n);

VectorIterBase vectIterRange_IMPL(Vector *pVector, void *pFirst, void *pLast);
NvBool vectIterNext_IMPL(VectorIterBase *pIter, void **ppValue);
NvBool vectIterPrev_IMPL(VectorIterBase *pIter, void **ppValue);

NvBool vectIsValid_IMPL(void *pVect);

#ifdef __cplusplus
}
#endif

#endif // NV_CONTAINERS_VECTOR_H
