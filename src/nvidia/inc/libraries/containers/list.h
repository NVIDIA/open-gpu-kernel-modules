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
#ifndef _NV_CONTAINERS_LIST_H_
#define _NV_CONTAINERS_LIST_H_

// Contains mix of C/C++ declarations.
#include "containers/type_safety.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"
#include "nvmisc.h"
#include "nvport/nvport.h"

/**
 * @defgroup NV_CONTAINERS_LIST List
 *
 * @brief List (sequence) of user-defined values.
 *
 * @details Order of values is not necessarily increasing or sorted, but order is
 * preserved across mutation. Please see
 * http://en.wikipedia.org/wiki/Sequence for a formal definition.
 *
 * The provided interface is abstract, decoupling the user from the underlying
 * list implementation. Two options are available with regard to memory
 * management, intrusive and non-intrusive. Users can select either one based
 * on different situations. Despite the two versions of the list, the following
 * implementation constraints are guaranteed.
 *
 * - Time Complexity:
 *  * Operations are \b O(1),
 *  * Unless stated otherwise.
 *
 * - Memory Usage:
 *  * \b O(N) memory is required for N values.
 *  * Intrusive and non-intrusive variants are provided.
 *    See @ref mem-ownership for further details.
 *
 * - Synchronization:
 *  * \b None. The container is not thread-safe.
 *  * Locking must be handled by the user if required.
 */

#define MAKE_LIST(listTypeName, dataType)                                    \
    typedef union listTypeName##Iter                                         \
    {                                                                        \
        dataType *pValue;                                                    \
        ListIterBase iter;                                                   \
    } listTypeName##Iter;                                                    \
    typedef union listTypeName                                               \
    {                                                                        \
        NonIntrusiveList real;                                               \
        CONT_TAG_TYPE(ListBase, dataType, listTypeName##Iter);               \
        CONT_TAG_NON_INTRUSIVE(dataType);                                    \
    } listTypeName

#define DECLARE_LIST(listTypeName)                                           \
    typedef union listTypeName##Iter listTypeName##Iter;                     \
    typedef union listTypeName listTypeName

#define MAKE_INTRUSIVE_LIST(listTypeName, dataType, node)                    \
    typedef union listTypeName##Iter                                         \
    {                                                                        \
        dataType *pValue;                                                    \
        ListIterBase iter;                                                   \
    } listTypeName##Iter;                                                    \
    typedef union listTypeName                                               \
    {                                                                        \
        IntrusiveList real;                                                  \
        CONT_TAG_TYPE(ListBase, dataType, listTypeName##Iter);               \
        CONT_TAG_INTRUSIVE(dataType, node);                                  \
    } listTypeName                                                           \

#define DECLARE_INTRUSIVE_LIST(listTypeName)                                 \
    typedef union listTypeName##Iter listTypeName##Iter;                     \
    typedef union listTypeName listTypeName

/**
* @brief Internal node structure to embed within intrusive list values.
*/
typedef struct ListNode ListNode;

/**
 * @brief Base type common to both intrusive and non-intrusive variants.
 */
typedef struct ListBase ListBase;

/**
 * @brief Non-intrusive list (container-managed memory).
 */
typedef struct NonIntrusiveList NonIntrusiveList;

/**
 * @brief Intrusive list (user-managed memory).
 */
typedef struct IntrusiveList IntrusiveList;

/**
 * @brief Iterator over a range of list values.
 *
 * See @ref iterators for usage details.
 */
typedef struct ListIterBase ListIterBase;

struct ListNode
{
    /// @privatesection
    ListNode *pPrev;
    ListNode *pNext;
#if PORT_IS_CHECKED_BUILD
    ListBase *pList;
#endif
};

struct ListIterBase
{
    void     *pValue;
    ListBase *pList;
    ListNode *pNode;
    ListNode *pLast;
#if PORT_IS_CHECKED_BUILD
    NvU32 versionNumber;
#endif
};

ListIterBase listIterRange_IMPL(ListBase *pList, void *pFirst, void *pLast);
CONT_VTABLE_DECL(ListBase, ListIterBase);

struct ListBase
{
    CONT_VTABLE_FIELD(ListBase);
    ListNode           *pHead;
    ListNode           *pTail;
    NvU32               count;
    NvS32               nodeOffset;
#if PORT_IS_CHECKED_BUILD
    NvU32 versionNumber;
#endif
};

struct NonIntrusiveList
{
    ListBase            base;
    PORT_MEM_ALLOCATOR *pAllocator;
    NvU32               valueSize;
};

struct IntrusiveList
{
    ListBase            base;
};

#define listInit(pList, pAllocator)                                          \
    listInit_IMPL(&((pList)->real), pAllocator, sizeof(*(pList)->valueSize))

#define listInitIntrusive(pList)                                             \
    listInitIntrusive_IMPL(&((pList)->real), sizeof(*(pList)->nodeOffset))

#define listDestroy(pList)                                                   \
    CONT_DISPATCH_ON_KIND(pList,                                             \
        listDestroy_IMPL((NonIntrusiveList*)&((pList)->real)),               \
        listDestroyIntrusive_IMPL(&((pList)->real.base)),                    \
        contDispatchVoid_STUB())

#define listCount(pList)                                                     \
    listCount_IMPL(&((pList)->real).base)

#define listInsertNew(pList, pNext)                                          \
    CONT_CAST_ELEM(pList,                                                    \
        listInsertNew_IMPL(&(pList)->real,                                   \
            CONT_CHECK_ARG(pList, pNext)))

#define listAppendNew(pList)                                                 \
    CONT_CAST_ELEM(pList, listAppendNew_IMPL(&(pList)->real))

#define listPrependNew(pList)                                                \
    CONT_CAST_ELEM(pList, listPrependNew_IMPL(&(pList)->real))

#define listInsertValue(pList, pNext, pValue)                                \
    CONT_CAST_ELEM(pList,                                                    \
        listInsertValue_IMPL(&(pList)->real,                                 \
            CONT_CHECK_ARG(pList, pNext),                                    \
            CONT_CHECK_ARG(pList, pValue)))

#define listAppendValue(pList, pValue)                                       \
    CONT_CAST_ELEM(pList,                                                    \
        listAppendValue_IMPL(&(pList)->real,                                 \
            CONT_CHECK_ARG(pList, pValue)))

#define listPrependValue(pList, pValue)                                      \
    CONT_CAST_ELEM(pList,                                                    \
        listPrependValue_IMPL(&(pList)->real,                                \
            CONT_CHECK_ARG(pList, pValue)))

#define listInsertExisting(pList, pNext, pValue)                             \
    listInsertExisting_IMPL(&(pList)->real,                                  \
        CONT_CHECK_ARG(pList, pNext),                                        \
        CONT_CHECK_ARG(pList, pValue))

#define listAppendExisting(pList, pValue)                                    \
    listAppendExisting_IMPL(&(pList)->real,                                  \
        CONT_CHECK_ARG(pList, pValue))

#define listPrependExisting(pList, pValue)                                   \
    listPrependExisting_IMPL(&(pList)->real,                                 \
        CONT_CHECK_ARG(pList, pValue))

#define listRemove(pList, pValue)                                            \
    CONT_DISPATCH_ON_KIND(pList,                                             \
        listRemove_IMPL((NonIntrusiveList*)&((pList)->real),                 \
            CONT_CHECK_ARG(pList, pValue)),                                  \
        listRemoveIntrusive_IMPL(&((pList)->real).base,                      \
            CONT_CHECK_ARG(pList, pValue)),                                  \
        contDispatchVoid_STUB())

#define listRemoveFirstByValue(pList, pValue)                                \
    listRemoveFirstByValue_IMPL(&(pList)->real,                              \
        CONT_CHECK_ARG(pList, pValue))

#define listRemoveAllByValue(pList, pValue)                                  \
    listRemoveAllByValue_IMPL(&(pList)->real,                                \
        CONT_CHECK_ARG(pList, pValue))

#define listClear(pList)                                                     \
    listDestroy(pList)

#define listFindByValue(pList, pValue)                                       \
    CONT_CAST_ELEM(pList,                                                    \
        listFindByValue_IMPL(&(pList)->real,                                 \
            CONT_CHECK_ARG(pList, pValue)))

#define listHead(pList)                                                      \
    CONT_CAST_ELEM(pList, listHead_IMPL(&((pList)->real).base))

#define listTail(pList)                                                      \
    CONT_CAST_ELEM(pList, listTail_IMPL(&((pList)->real).base))

#define listNext(pList, pValue)                                              \
    CONT_CAST_ELEM(pList,                                                    \
        listNext_IMPL(&((pList)->real).base,                                 \
            CONT_CHECK_ARG(pList, pValue)))

#define listPrev(pList, pValue)                                              \
    CONT_CAST_ELEM(pList,                                                    \
        listPrev_IMPL(&((pList)->real).base,                                 \
            CONT_CHECK_ARG(pList, pValue)))

#define listIterAll(pList)                                                   \
    listIterRange(pList, listHead(pList), listTail(pList))

#define listIterRange(pList, pFirst, pLast)                                  \
    CONT_ITER_RANGE(pList, &listIterRange_IMPL,                              \
        CONT_CHECK_ARG(pList, pFirst), CONT_CHECK_ARG(pList, pLast))

#define listIterNext(pIt)                                                    \
    listIterNext_IMPL(&((pIt)->iter))

void  listInit_IMPL(NonIntrusiveList *pList, PORT_MEM_ALLOCATOR *pAllocator,
                    NvU32 valueSize);
void  listInitIntrusive_IMPL(IntrusiveList *pList, NvS32 nodeOffset);
void  listDestroy_IMPL(NonIntrusiveList *pList);
void  listDestroyIntrusive_IMPL(ListBase *pList);

NvU32 listCount_IMPL(ListBase *pList);
void *listInsertNew_IMPL(NonIntrusiveList *pList, void *pNext);
void *listAppendNew_IMPL(NonIntrusiveList *pList);
void *listPrependNew_IMPL(NonIntrusiveList *pList);
void *listInsertValue_IMPL(NonIntrusiveList *pList, void *pNext, void *pValue);
void *listAppendValue_IMPL(NonIntrusiveList *pList, void *pValue);
void *listPrependValue_IMPL(NonIntrusiveList *pList, void *pValue);
void  listInsertExisting_IMPL(IntrusiveList *pList, void *pNext, void *pValue);
void  listAppendExisting_IMPL(IntrusiveList *pList, void *pValue);
void  listPrependExisting_IMPL(IntrusiveList *pList, void *pValue);
void  listRemove_IMPL(NonIntrusiveList *pList, void *pValue);
void  listRemoveIntrusive_IMPL(ListBase *pList, void *pValue);
void  listRemoveFirstByValue_IMPL(NonIntrusiveList *pList, void *pValue);
void  listRemoveAllByValue_IMPL(NonIntrusiveList *pList, void *pValue);

void *listFindByValue_IMPL(NonIntrusiveList *pList, void *pValue);
void *listHead_IMPL(ListBase *pList);
void *listTail_IMPL(ListBase *pList);
void *listNext_IMPL(ListBase *pList, void *pValue);
void *listPrev_IMPL(ListBase *pList, void *pValue);

ListIterBase listIterAll_IMPL(ListBase *pList);
ListIterBase listIterRange_IMPL(ListBase *pList, void *pFirst, void *pLast);
NvBool       listIterNext_IMPL(ListIterBase *pIt);

static NV_FORCEINLINE ListNode *
listValueToNode(ListBase *pList, void *pValue)
{
    if (NULL == pList) return NULL;
    if (NULL == pValue) return NULL;
    return (ListNode*)((NvU8*)pValue + pList->nodeOffset);
}

static NV_FORCEINLINE void *
listNodeToValue(ListBase *pList, ListNode *pNode)
{
    if (NULL == pList) return NULL;
    if (NULL == pNode) return NULL;
    return (NvU8*)pNode - pList->nodeOffset;
}

#ifdef __cplusplus
}
#endif

#endif // _NV_CONTAINERS_LIST_H_
