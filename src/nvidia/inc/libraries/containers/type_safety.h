/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _NV_CONTAINERS_TYPE_SAFETY_H_
#define _NV_CONTAINERS_TYPE_SAFETY_H_

#include "nvtypes.h"
#include "nvport/nvport.h"

// Check for typeof support. For now restricting to GNUC compilers.
#if defined(__GNUC__)
#define NV_TYPEOF_SUPPORTED 1
#else
#define NV_TYPEOF_SUPPORTED 0
#endif

/**
 * Tag a non-intrusive container union with the following info:
 *    valueSize : size of its element type for non-intrusive malloc
 *    kind      : non-intrusive kind ID for static dispatch
 */
#define CONT_TAG_NON_INTRUSIVE(elemType)                                     \
    struct {char _[sizeof(elemType)];}            *valueSize;                \
    struct {char _[CONT_KIND_NON_INTRUSIVE];}     *kind

/**
 * Tag an intrusive container union with the following info:
 *    nodeOffset : offset of the data structure node within element type
 *    kind       : intrusive kind ID for static dispatch
 */
// FIXME: Do not use this for any structure members with offset 0!
//        The size of a 0 length array is undefined according to the C99 standard
//        and we've seen non-zero values of sizeof(*nodeOffset) appear at runtime
//        leading to corruption. Filed Bug 2858103 to track work against this.
#define CONT_TAG_INTRUSIVE(elemType, node)                                   \
    struct {char _[NV_OFFSETOF(elemType, node)];} *nodeOffset;               \
    struct {char _[CONT_KIND_INTRUSIVE];}         *kind


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Utility identity function for several type-safety mechanisms.
 */
static NV_FORCEINLINE void *contId(void *pValue)
{
    return pValue;
}

#ifdef __cplusplus
}
#endif

/**
 * @def CONT_TAG_ELEM_TYPE
 * Tag a container union with element type info.
 */

/**
 * @def CONT_CHECK_ARG
 * Check that a value has a container's element type.
 */

/**
 * @def CONT_CAST_ELEM
 * Cast a void pointer to a container's element type.
 */

// With C++ we can use typedef and templates for 100% type safety.
#if defined(__cplusplus) && !defined(NV_CONTAINERS_NO_TEMPLATES)

#define CONT_TAG_TYPE(contType, elemType, iterType)                          \
    CONT_VTABLE_TAG(contType, elemType, iterType);                           \
    typedef contType ContType;                                               \
    typedef elemType ElemType;                                               \
    typedef iterType IterType

template <class T>
typename T::ElemType *CONT_CHECK_ARG(T *pCont, typename T::ElemType *pValue)
{
    return pValue;
}

template <class T>
typename T::ElemType *CONT_CAST_ELEM(T *pCont, void *pValue)
{
    return (typename T::ElemType *)pValue;
}

template <class T, class It>
typename T::IterType CONT_ITER_RANGE
(
    T    *pCont,
    It  (*pFunc)(typename T::ContType *, void *, void *),
    void *pFirst,
    void *pLast
)
{
    typename T::IterType temp;
    temp.iter = pFunc(&pCont->real.base, pFirst, pLast);
    return temp;
}

template <class T, class It>
typename T::IterType CONT_ITER_RANGE_INDEX
(
    T    *pCont,
    It  (*pFunc)(typename T::ContType *, NvU64, NvU64),
    NvU64 first,
    NvU64 last
)
{
    typename T::IterType temp;
    temp.iter = pFunc(&pCont->real.base, first, last);
    return temp;
}

// Without C++ we need more creativity. :)
#else

// Element tag is a pointer to the element type (no mem overhead in union).
#define CONT_TAG_TYPE(contType, elemType, iterType)                          \
    CONT_VTABLE_TAG(contType, elemType, iterType);                           \
    elemType *elem;                                                          \
    iterType *iter

// Argument check uses sizeof to get error message without runtime overhead.
#define CONT_CHECK_ARG(pCont, pValue)                                        \
    (sizeof((pCont)->elem = (pValue)) ? (pValue) : NULL)

//
// Return checks are more problematic, but typeof is perfect when available.
// Without typeof we resort to a runtime vtable.
//
#if NV_TYPEOF_SUPPORTED

#define CONT_CAST_ELEM(pCont, ret) ((typeof((pCont)->elem))(ret))

//
// The dummy contId prevents compilers from warning about incompatible
// function casts. This is safe since we know the two return structures
// are identical (modulo alpha-conversion).
//
#define CONT_ITER_RANGE(pCont, pFunc, pFirst, pLast)                         \
    (((typeof(*(pCont)->iter)(*)(void *, void *, void *))contId(pFunc))(     \
        pCont, pFirst, pLast))

#define CONT_ITER_RANGE_INDEX(pCont, pFunc, first, last)                     \
    (((typeof(*(pCont)->iter)(*)(void *, NvU64, NvU64))contId(pFunc))(       \
        pCont, first, last))

#else

#define CONT_CAST_ELEM(pCont, ret) ((pCont)->vtable->checkRet(ret))

#define CONT_ITER_RANGE(pCont, pFunc, pFirst, pLast)                         \
    ((pCont)->vtable->iterRange(&(pCont)->real.base, pFirst, pLast))

#define CONT_ITER_RANGE_RANGE(pCont, pFunc, first, last)                     \
    ((pCont)->vtable->iterRangeIndex(&(pCont)->real.base, first, last))

#endif

#endif

#if NV_TYPEOF_SUPPORTED

#define CONT_VTABLE_DECL(contType, iterType)
#define CONT_VTABLE_DEFN(contType, contIterRange, contIterRangeIndex)
#define CONT_VTABLE_TAG(contType, elemType, iterType)
#define CONT_VTABLE_FIELD(contType)
#define CONT_VTABLE_INIT(contType, pCont)

#else

#define CONT_VTABLE_DECL(contType, iterType)                                 \
    typedef struct                                                           \
    {                                                                        \
        void    *(*checkRet)(void *pValue);                                  \
        iterType (*iterRange)(contType *pCont, void *pFirst, void *pLast);   \
        iterType (*iterRangeIndex)(contType *pCont, NvU64 first, NvU64 last);\
    } contType##_VTABLE;                                                     \

#define CONT_VTABLE_DEFN(contType, contIterRange, contIterRangeIndex)        \
    static const contType##_VTABLE g_##contType##_VTABLE =                   \
    {                                                                        \
        contId,                                                              \
        contIterRange,                                                       \
        contIterRangeIndex,                                                  \
    }

#define CONT_VTABLE_TAG(contType, elemType, iterType)                        \
    const struct                                                             \
    {                                                                        \
        elemType *(*checkRet)(void *pValue);                                 \
        iterType  (*iterRange)(contType *pCont, void *pFirst, void *pLast);  \
        iterType (*iterRangeIndex)(contType *pCont, NvU64 first, NvU64 last);\
    } *vtable

#define CONT_VTABLE_FIELD(contType) const contType##_VTABLE *vtable

#define CONT_VTABLE_INIT(contType, pCont)                                    \
    ((pCont)->vtable = &g_##contType##_VTABLE)

#endif

enum CONT_KIND
{
    CONT_KIND_NON_INTRUSIVE = 1,
    CONT_KIND_INTRUSIVE     = 2,
};

/**
 * Static dispatch uses sizeof with dummy arrays to select a path.
 *
 * With optimizations enabled the unused paths should be trimmed, so this
 * should have zero overhead in release builds.
*/
#define CONT_DISPATCH_ON_KIND(pCont, ret1, ret2, ret3)                       \
    ((sizeof(*(pCont)->kind) == CONT_KIND_NON_INTRUSIVE) ? (ret1) :          \
     (sizeof(*(pCont)->kind) == CONT_KIND_INTRUSIVE)     ? (ret2) :          \
                                                           (ret3))

/**
 * Utility stub useful for the above ret3 argument (unreachable path).
 * Add stubs for different return types as needed.
 */
static NV_FORCEINLINE void contDispatchVoid_STUB(void)
{
    PORT_BREAKPOINT();
}

#endif // _NV_CONTAINERS_TYPE_SAFETY_H_
