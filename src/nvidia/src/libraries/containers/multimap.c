/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "containers/multimap.h"

CONT_VTABLE_DEFN(MultimapBase, multimapItemIterRange_IMPL, NULL);

void multimapInit_IMPL
(
    MultimapBase *pBase,
    PORT_MEM_ALLOCATOR *pAllocator,
    NvU32 valueSize,
    NvS32 nodeOffset,
    NvU32 submapSize
)
{
    NV_ASSERT_OR_RETURN_VOID(NULL != pBase);
    NV_ASSERT_OR_RETURN_VOID(NULL != pAllocator);
    mapInit_IMPL(&pBase->map, pAllocator, submapSize);
    CONT_VTABLE_INIT(MultimapBase, pBase);
    pBase->multimapNodeOffset = nodeOffset;
    pBase->itemCount = 0;
    pBase->itemSize = valueSize;
}

void multimapDestroy_IMPL
(
    MultimapBase *pBase
)
{
    void *pLeaf;
    IntrusiveMap *pSubmap;
    NV_ASSERT_OR_RETURN_VOID(NULL != pBase);

    pLeaf = multimapFirstItem_IMPL(pBase);
    while (NULL != pLeaf)
    {
        void *pNext = multimapNextItem_IMPL(pBase, pLeaf);
        multimapRemoveItem_IMPL(pBase, pLeaf);
        pLeaf = pNext;
    }

    while (NULL != (pSubmap = (IntrusiveMap *)mapFindGEQ_IMPL(&pBase->map.base, 0)))
    {
        mapDestroyIntrusive_IMPL(&pSubmap->base);
        mapRemove_IMPL(&pBase->map, pSubmap);
    }

    mapDestroy_IMPL(&pBase->map);
}

void multimapClear_IMPL
(
    MultimapBase *pBase
)
{
    PORT_MEM_ALLOCATOR *pAllocator;
    NvU32 valueSize;
    NvS32 nodeOffset;
    NvU32 submapSize;

    NV_ASSERT_OR_RETURN_VOID(NULL != pBase);
    pAllocator = pBase->map.pAllocator;
    valueSize = pBase->itemSize;
    nodeOffset = pBase->multimapNodeOffset;
    submapSize = pBase->map.valueSize;

    multimapDestroy_IMPL(pBase);
    multimapInit_IMPL(pBase, pAllocator, valueSize, nodeOffset, submapSize);
}

void *multimapInsertSubmap_IMPL(MultimapBase *pBase, NvU64 submapKey)
{
    void *pSubmap;
    NV_ASSERT_OR_RETURN(NULL != pBase, NULL);

    pSubmap = mapInsertNew_IMPL(&pBase->map, submapKey);
    if (NULL != pSubmap)
    {
        NvS32 submapNodeOffset = pBase->multimapNodeOffset +
                                 NV_OFFSETOF(MultimapNode, submapNode);
        mapInitIntrusive_IMPL((IntrusiveMap *)pSubmap, submapNodeOffset);
    }

    return pSubmap;
}

void *multimapFindSubmap_IMPL(MultimapBase *pBase, NvU64 submapKey)
{
    return mapFind_IMPL(&pBase->map.base, submapKey);
}

void *multimapFindSubmapLEQ_IMPL(MultimapBase *pBase, NvU64 submapKey)
{
    return mapFindLEQ_IMPL(&pBase->map.base, submapKey);
}

void *multimapFindSubmapGEQ_IMPL(MultimapBase *pBase, NvU64 submapKey)
{
    return mapFindGEQ_IMPL(&pBase->map.base, submapKey);
}

void *multimapInsertItemNew_IMPL
(
    MultimapBase *pBase,
    NvU64 submapKey,
    NvU64 itemKey
)
{
    IntrusiveMap *pSubmap;
    void *pLeaf;
    NvU32 leafSize;

    if (NULL == pBase)
        return NULL;

    pSubmap = (IntrusiveMap *)multimapFindSubmap_IMPL(pBase, submapKey);
    if (NULL == pSubmap)
        return NULL;

    leafSize = pBase->multimapNodeOffset + sizeof(MultimapNode);
    pLeaf = PORT_ALLOC(pBase->map.pAllocator, leafSize);

    if (NULL == pLeaf)
        return NULL;

    portMemSet(pLeaf, 0, leafSize);

    multimapValueToNode(pBase, pLeaf)->pSubmap = pSubmap;

    if (!mapInsertExisting_IMPL(pSubmap, itemKey, pLeaf))
    {
        PORT_FREE(pBase->map.pAllocator, pLeaf);
        return NULL;
    }

    pBase->itemCount++;

    return pLeaf;
}

void *multimapInsertItemValue_IMPL
(
    MultimapBase *pBase,
    NvU64 submapKey,
    NvU64 itemKey,
    void *pValue
)
{
    void *pLeaf;

    NV_ASSERT_OR_RETURN(NULL != pBase, NULL);
    NV_ASSERT_OR_RETURN(NULL != pValue, NULL);

    pLeaf = multimapInsertItemNew_IMPL(pBase, submapKey, itemKey);

    if (NULL == pLeaf)
        return NULL;

    return portMemCopy(pLeaf, pBase->itemSize, pValue, pBase->itemSize);
}

void *multimapFindItem_IMPL
(
    MultimapBase *pBase,
    NvU64 submapKey,
    NvU64 itemKey
)
{
    IntrusiveMap *pSubmap;

    NV_ASSERT_OR_RETURN(NULL != pBase, NULL);

    pSubmap = (IntrusiveMap *)multimapFindSubmap_IMPL(pBase, submapKey);
    if (NULL == pSubmap)
        return NULL;

    return mapFind_IMPL(&pSubmap->base, itemKey);
}

void multimapRemoveItem_IMPL(MultimapBase *pBase, void *pLeaf)
{
    IntrusiveMap *pSubmap;
    NvU32 itemCount;

    NV_ASSERT_OR_RETURN_VOID(NULL != pBase);
    NV_ASSERT_OR_RETURN_VOID(NULL != pLeaf);

    pSubmap = (IntrusiveMap *)multimapValueToNode(pBase, pLeaf)->pSubmap;
    NV_ASSERT_OR_RETURN_VOID(NULL != pSubmap);

    itemCount = pSubmap->base.count;
    mapRemoveIntrusive_IMPL(&pSubmap->base, pLeaf);
    // Only continue if an item was actually removed
    if (itemCount == pSubmap->base.count)
        return;

    PORT_FREE(pBase->map.pAllocator, pLeaf);

    pBase->itemCount--;
}

void multimapRemoveSubmap_IMPL
(
    MultimapBase *pBase,
    MapBase *pSubmap
)
{
    NV_ASSERT_OR_RETURN_VOID(NULL != pBase);
    NV_ASSERT_OR_RETURN_VOID(NULL != pSubmap);
    NV_ASSERT_OR_RETURN_VOID(pSubmap->count == 0);
    mapDestroyIntrusive_IMPL(pSubmap);
    mapRemove_IMPL(&pBase->map, pSubmap);
}

void multimapRemoveItemByKey_IMPL
(
    MultimapBase *pBase,
    NvU64 submapKey,
    NvU64 itemKey
)
{
    void *pLeaf = multimapFindItem_IMPL(pBase, submapKey, itemKey);
    if (NULL != pLeaf)
        multimapRemoveItem_IMPL(pBase, pLeaf);
}

void *multimapNextItem_IMPL(MultimapBase *pBase, void *pValue)
{
    IntrusiveMap *pSubmap;

    NV_ASSERT_OR_RETURN(NULL != pBase && NULL != pValue, NULL);

    pSubmap = (IntrusiveMap *)multimapValueToNode(pBase, pValue)->pSubmap;
    NV_ASSERT_OR_RETURN(NULL != pSubmap, NULL);

    pValue = mapNext_IMPL(&pSubmap->base, pValue);
    while (NULL == pValue)
    {
        pSubmap = (IntrusiveMap *)mapNext_IMPL(&pBase->map.base, pSubmap);
        if (NULL == pSubmap)
            return NULL;

        pValue = mapFindGEQ_IMPL(&pSubmap->base, 0);
    }

    return pValue;
}

void *multimapPrevItem_IMPL(MultimapBase *pBase, void *pValue)
{
    IntrusiveMap *pSubmap;

    NV_ASSERT_OR_RETURN(NULL != pBase && NULL != pValue, NULL);

    pSubmap = (IntrusiveMap *)multimapValueToNode(pBase, pValue)->pSubmap;
    NV_ASSERT_OR_RETURN(NULL != pSubmap, NULL);

    pValue = mapPrev_IMPL(&pSubmap->base, pValue);
    while (NULL == pValue)
    {
        pSubmap = (IntrusiveMap *)mapPrev_IMPL(&pBase->map.base, pSubmap);
        if (NULL == pSubmap)
            return NULL;

        pValue = mapFindLEQ_IMPL(&pSubmap->base, NV_U64_MAX);
    }

    return pValue;
}

void *multimapFirstItem_IMPL(MultimapBase *pBase)
{
    IntrusiveMap *pSubmap;
    NV_ASSERT_OR_RETURN(NULL != pBase, NULL);

    pSubmap = mapFindGEQ_IMPL(&pBase->map.base, 0);
    while (NULL != pSubmap)
    {
        void *pItem = mapFindGEQ_IMPL(&pSubmap->base, 0);
        if (NULL != pItem)
            return pItem;
            
        pSubmap = mapNext_IMPL(&pBase->map.base, pSubmap);
    }

    return NULL;
}

void *multimapLastItem_IMPL(MultimapBase *pBase)
{
    IntrusiveMap *pSubmap;
    NV_ASSERT_OR_RETURN(NULL != pBase, NULL);

    pSubmap = mapFindLEQ_IMPL(&pBase->map.base, NV_U64_MAX);
    while (NULL != pSubmap)
    {
        void *pItem = mapFindLEQ_IMPL(&pSubmap->base, NV_U64_MAX);
        if (NULL != pItem)
            return pItem;
            
        pSubmap = mapPrev_IMPL(&pBase->map.base, pSubmap);
    }

    return NULL;
}

MultimapIterBase multimapItemIterRange_IMPL
(
    MultimapBase *pBase,
    void *pFirst,
    void *pLast
)
{
    MultimapIterBase it;

    portMemSet(&it, 0, sizeof(it));
    it.pMultimap = pBase;
    
    NV_ASSERT_OR_RETURN(NULL != pBase, it);

    if (pBase->itemCount == 0 || pFirst == NULL || pLast == NULL)
        return it;

    {
        MultimapNode *pFirstNode;
        MultimapNode *pLastNode;
        NvU64 firstKey, lastKey, firstSubmapKey, lastSubmapKey;

        pFirstNode = multimapValueToNode(pBase, pFirst);
        pLastNode = multimapValueToNode(pBase, pLast);

        firstKey = pFirstNode->submapNode.key;
        lastKey = pLastNode->submapNode.key;
        firstSubmapKey = mapValueToNode(&pBase->map.base, pFirstNode->pSubmap)->key;
        lastSubmapKey = mapValueToNode(&pBase->map.base, pLastNode->pSubmap)->key;

        NV_ASSERT(firstSubmapKey < lastSubmapKey ||
            (firstSubmapKey == lastSubmapKey && firstKey <= lastKey));
    }
    it.pNext = pFirst;
    it.pLast = pLast;
    return it;
}

NvBool multimapItemIterNext_IMPL(MultimapIterBase *pIt)
{
    NV_ASSERT_OR_RETURN(NULL != pIt, NV_FALSE);

    pIt->pValue = pIt->pNext;

    if (NULL == pIt->pNext)
        return NV_FALSE;

    if (pIt->pNext == pIt->pLast)
        pIt->pNext = NULL;
    else
        pIt->pNext = multimapNextItem_IMPL(pIt->pMultimap, pIt->pNext);

    return NV_TRUE;
}
