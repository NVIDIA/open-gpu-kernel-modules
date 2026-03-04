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
#include "containers/list.h"
#include "utils/nvassert.h"

CONT_VTABLE_DEFN(ListBase, listIterRange_IMPL, NULL);

#if PORT_IS_CHECKED_BUILD
static NvBool _listIterRangeCheck(ListBase *pList, ListNode *pFirst,
                                  ListNode *pLast);
#endif
static void _listInsertBase(ListBase *pList, void *pNext, void *pValue);

void listInit_IMPL(NonIntrusiveList *pList, PORT_MEM_ALLOCATOR *pAllocator,
                   NvU32 valueSize)
{
    NV_ASSERT_OR_RETURN_VOID(NULL != pList);
    NV_ASSERT_OR_RETURN_VOID(NULL != pAllocator);

    portMemSet(&(pList->base), 0, sizeof(pList->base));
    CONT_VTABLE_INIT(ListBase, &pList->base);
    pList->pAllocator = pAllocator;
    pList->valueSize = valueSize;
    pList->base.nodeOffset = (NvS32)(0 - sizeof(ListNode));
}

void listInitIntrusive_IMPL(IntrusiveList *pList, NvS32 nodeOffset)
{
    NV_ASSERT_OR_RETURN_VOID(NULL != pList);
    portMemSet(&(pList->base), 0, sizeof(pList->base));
    CONT_VTABLE_INIT(ListBase, &pList->base);
    pList->base.nodeOffset = nodeOffset;
}

static void
_listDestroy(ListBase *pList, PORT_MEM_ALLOCATOR *pAllocator)
{
    ListNode *pNode;
    NV_ASSERT_OR_RETURN_VOID(NULL != pList);

    pNode = pList->pHead;

    pList->pHead = NULL;
    pList->pTail = NULL;
    pList->count = 0;
    NV_CHECKED_ONLY(pList->versionNumber++);

    while (pNode != NULL)
    {
        ListNode *pTemp = pNode;
        pNode = pNode->pNext;
        pTemp->pPrev = NULL;
        pTemp->pNext = NULL;
        NV_CHECKED_ONLY(pTemp->pList = NULL);
        if (NULL != pAllocator)
        {
            PORT_FREE(pAllocator, pTemp);
        }
    }
}

void listDestroy_IMPL(NonIntrusiveList *pList)
{
    _listDestroy(&pList->base, pList->pAllocator);
}

void listDestroyIntrusive_IMPL(ListBase *pList)
{
    _listDestroy(pList, NULL);
}

NvU32 listCount_IMPL(ListBase *pList)
{
    NV_ASSERT_OR_RETURN(pList, 0);
    return pList->count;
}

void *listInsertNew_IMPL(NonIntrusiveList *pList, void *pNext)
{
    void *pNode = NULL;
    void *pValue;

    NV_ASSERT_OR_RETURN(NULL != pList, NULL);

    pNode = PORT_ALLOC(pList->pAllocator, sizeof(ListNode) + pList->valueSize);
    NV_ASSERT_OR_RETURN(NULL != pNode, NULL);

    portMemSet(pNode, 0, sizeof(ListNode) + pList->valueSize);
    pValue = listNodeToValue(&pList->base, pNode);
    _listInsertBase(&(pList->base), pNext, pValue);

    return pValue;
}

void *listAppendNew_IMPL(NonIntrusiveList *pList)
{
    return listInsertNew_IMPL(pList, NULL);
}

void *listPrependNew_IMPL(NonIntrusiveList *pList)
{
    return listInsertNew_IMPL(pList, listHead_IMPL(&(pList->base)));
}

void *listInsertValue_IMPL
(
    NonIntrusiveList *pList,
    void             *pNext,
    const void       *pValue
)
{
    void *pCurrent;

    NV_ASSERT_OR_RETURN(NULL != pValue, NULL);

    pCurrent = listInsertNew_IMPL(pList, pNext);
    if (NULL == pCurrent)
        return NULL;

    return portMemCopy(pCurrent, pList->valueSize, pValue, pList->valueSize);
}

void *listAppendValue_IMPL(NonIntrusiveList *pList, const void *pValue)
{
    return listInsertValue_IMPL(pList, NULL, pValue);
}

void *listPrependValue_IMPL(NonIntrusiveList *pList, const void *pValue)
{
    return listInsertValue_IMPL(pList, listHead_IMPL(&(pList->base)), pValue);
}

void listInsertExisting_IMPL(IntrusiveList *pList, void *pNext, void *pValue)
{
    NV_ASSERT_OR_RETURN_VOID(NULL != pList);
    NV_ASSERT_OR_RETURN_VOID(NULL != pValue);
    _listInsertBase(&(pList->base), pNext, pValue);
}

void listAppendExisting_IMPL(IntrusiveList *pList, void *pValue)
{
    listInsertExisting_IMPL(pList, NULL, pValue);
}

void listPrependExisting_IMPL(IntrusiveList *pList, void *pValue)
{
    listInsertExisting_IMPL(pList, listHead_IMPL(&(pList->base)), pValue);
}

// for nonintrusive version
void listRemove_IMPL(NonIntrusiveList *pList, void *pValue)
{
    if (pValue == NULL)
        return;
    listRemoveIntrusive_IMPL(&(pList->base), pValue);
    PORT_FREE(pList->pAllocator, listValueToNode(&pList->base, pValue));
}

// intrusive version
void listRemoveIntrusive_IMPL
(
    ListBase    *pList,
    void        *pValue
)
{
    ListNode *pNode;

    if (pValue == NULL)
        return;

    pNode = listValueToNode(pList, pValue);
    NV_ASSERT_OR_RETURN_VOID(NULL != pNode);
    NV_ASSERT_CHECKED(pNode->pList == pList);

    if (pNode->pPrev != NULL)
        pNode->pPrev->pNext = pNode->pNext;
    else
        pList->pHead = pNode->pNext;

    if (pNode->pNext != NULL)
        pNode->pNext->pPrev = pNode->pPrev;
    else
        pList->pTail = pNode->pPrev;

    pNode->pNext = NULL;
    pNode->pPrev = NULL;

    pList->count--;
    NV_CHECKED_ONLY(pList->versionNumber++);
    NV_CHECKED_ONLY(pNode->pList = NULL);
}

// pvalue here means the value
void listRemoveFirstByValue_IMPL
(
    NonIntrusiveList *pList,
    void *pValue
)
{
    void *pValueFound = listFindByValue_IMPL(pList, pValue);
    if (pValueFound)
    {
        listRemove_IMPL(pList, pValueFound);
    }
}

void listRemoveAllByValue_IMPL
(
    NonIntrusiveList *pList,
    void *pValue
)
{
    void *pValueFound;
    ListNode *pNode;

    NV_ASSERT_OR_RETURN_VOID(NULL != pList);
    NV_ASSERT_OR_RETURN_VOID(NULL != pValue);

    pNode = pList->base.pHead;
    while (pNode != NULL)
    {
        pValueFound = listNodeToValue(&pList->base, pNode);
        pNode = pNode->pNext;

        if (portMemCmp(pValueFound, pValue, pList->valueSize) == 0)
        {
            listRemove_IMPL(pList, pValueFound);
            pValueFound = NULL;
        }
    }
}

void *listFindByValue_IMPL
(
    NonIntrusiveList *pList,
    void *pValue
)
{
    void *pResult;
    ListNode *pNode;

    NV_ASSERT_OR_RETURN(NULL != pList, NULL);
    NV_ASSERT_OR_RETURN(NULL != pValue, NULL);

    pNode = pList->base.pHead;
    while (pNode != NULL)
    {
        pResult = listNodeToValue(&pList->base, pNode);

        if (portMemCmp(pResult, pValue, pList->valueSize) == 0)
            return pResult;

        pNode = pNode->pNext;
    }

    return NULL;
}

void *listHead_IMPL
(
    ListBase *pList
)
{
    NV_ASSERT_OR_RETURN(NULL != pList, NULL);
    return listNodeToValue(pList, pList->pHead);
}

void *listTail_IMPL
(
    ListBase *pList
)
{
    NV_ASSERT_OR_RETURN(NULL != pList, NULL);
    return listNodeToValue(pList, pList->pTail);
}

void *listNext_IMPL
(
    ListBase    *pList,
    void        *pValue
)
{
    ListNode *pNode = listValueToNode(pList, pValue);
    NV_ASSERT_OR_RETURN(NULL != pNode, NULL);
    NV_ASSERT_CHECKED(pNode->pList == pList);
    return listNodeToValue(pList, pNode->pNext);
}

void *listPrev_IMPL
(
    ListBase *pList,
    void *pValue
)
{
    ListNode *pNode = listValueToNode(pList, pValue);
    NV_ASSERT_OR_RETURN(NULL != pNode, NULL);
    NV_ASSERT_CHECKED(pNode->pList == pList);
    return listNodeToValue(pList, pNode->pPrev);
}

ListIterBase listIterRange_IMPL
(
    ListBase    *pList,
    void        *pFirst,
    void        *pLast
)
{
    ListIterBase it;

    NV_ASSERT(NULL != pList);

    NV_CHECKED_ONLY(it.versionNumber = pList->versionNumber);
    it.pList = pList;
    it.pNode = listValueToNode(pList, pFirst);
    it.pLast = listValueToNode(pList, pLast);
    it.pValue = NULL;

    NV_ASSERT_CHECKED(it.pNode == NULL || it.pNode->pList == pList);
    NV_ASSERT_CHECKED(it.pLast == NULL || it.pLast->pList == pList);
    NV_ASSERT_CHECKED(_listIterRangeCheck(pList, it.pNode, it.pLast));
    NV_CHECKED_ONLY(it.bValid = NV_TRUE);

    return it;
}

NvBool listIterNext_IMPL(ListIterBase *pIt)
{
    NV_ASSERT_OR_RETURN(NULL != pIt, NV_FALSE);

#if PORT_IS_CHECKED_BUILD
    if (pIt->bValid && !CONT_ITER_IS_VALID(pIt->pList, pIt))
    {
        NV_ASSERT(CONT_ITER_IS_VALID(pIt->pList, pIt));
        PORT_DUMP_STACK();

        pIt->bValid = NV_FALSE;
    }
#endif

    if (!pIt->pNode)
        return NV_FALSE;

    pIt->pValue = listNodeToValue(pIt->pList, pIt->pNode);

    if (pIt->pNode == pIt->pLast)
        pIt->pNode = NULL;
    else
        pIt->pNode = pIt->pNode->pNext;

    return NV_TRUE;
}

#if PORT_IS_CHECKED_BUILD
// @todo: optimize for best average complexity
// assumption: nodes ownership checked in the caller function
// allow same node
static NvBool _listIterRangeCheck
(
    ListBase *pList,
    ListNode *pFirst,
    ListNode *pLast
)
{
    ListNode *pNode;

    for (pNode = pFirst; pNode != NULL; pNode = pNode->pNext)
    {
        if (pNode == pLast)
            return NV_TRUE;
    }

    // Check for both NULL (empty range) case.
    return pNode == pLast;
}
#endif

static void _listInsertBase
(
    ListBase    *pList,
    void        *pNextValue,
    void        *pValue
)
{
    ListNode *pNext = listValueToNode(pList, pNextValue);
    ListNode *pNode = listValueToNode(pList, pValue);

    pNode->pPrev = pNext ? pNext->pPrev : pList->pTail;
    pNode->pNext = pNext;

    if (pNode->pPrev)
        pNode->pPrev->pNext = pNode;
    else
        pList->pHead = pNode;

    if (pNode->pNext)
        pNode->pNext->pPrev = pNode;
    else
        pList->pTail = pNode;

    pList->count++;
    NV_CHECKED_ONLY(pList->versionNumber++);
    NV_CHECKED_ONLY(pNode->pList = pList);
}

NvBool listIsValid_IMPL(void *pList)
{
#if NV_TYPEOF_SUPPORTED
    return NV_TRUE;
#else
    if (CONT_VTABLE_VALID((ListBase*)pList))
        return NV_TRUE;

    NV_ASSERT_FAILED("vtable not valid!");
    CONT_VTABLE_INIT(ListBase, (ListBase*)pList);
    return NV_FALSE;
#endif
}

