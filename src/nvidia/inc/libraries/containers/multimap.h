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
#ifndef _NV_CONTAINERS_MULTIMAP_H_
#define _NV_CONTAINERS_MULTIMAP_H_

// Contains mix of C/C++ declarations.
#include "containers/type_safety.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "containers/map.h"

/**
 * @defgroup NV_CONTAINERS_MULTIMAP Multimap
 *
 * @brief Two-layer multimap (ordered) from pairs of 64-bit unsigned integer
 * keys to user-defined values.
 *
 * @details The provided interface is abstract, decoupling the user from the
 * underlying ordered multimap implementation. Currently, memory management is
 * limited to non-intrusive container-managed memory. The following
 * implementation constraints are guaranteed.
 *
 * - Time Complexity:
 *  * Operations are \b O(log M + log N),
 *  * Unless stated otherwise,
 *  * Where M is the number of submaps and N is the total number of values in
 *    the map.
 *
 * - Memory Usage:
 *  * \b O(M + N) memory is required for M submaps and N values.
 *  * Only a non-intrusive variant is provided.
 *    See @ref mem-ownership for further details.
 *
 * - Synchronization:
 *  * \b None. The container is not thread-safe.
 *  * Locking must be handled by the user if required.
 *
 */

#define MAKE_MULTIMAP(multimapTypeName, dataType)                             \
    typedef struct multimapTypeName##Leaf                                     \
    {                                                                         \
        dataType data;                                                        \
        MultimapNode node;                                                    \
    } multimapTypeName##Leaf;                                                 \
    MAKE_INTRUSIVE_MAP(multimapTypeName##Submap, multimapTypeName##Leaf,      \
                       node.submapNode);                                      \
    MAKE_MAP(multimapTypeName##Supermap, multimapTypeName##Submap);           \
    typedef union multimapTypeName##Iter                                      \
    {                                                                         \
        dataType *pValue;                                                     \
        MultimapIterBase iter;                                                \
    } multimapTypeName##Iter;                                                 \
    typedef union multimapTypeName                                            \
    {                                                                         \
		CONT_TAG_TYPE(MultimapBase, dataType, multimapTypeName##Iter);        \
		struct { MultimapBase base; } real;                                   \
        struct                                                                \
        {                                                                     \
			/* This field simply aligns map with the one in MultimapBase */   \
            CONT_VTABLE_FIELD(MultimapBase);                                  \
            multimapTypeName##Supermap map;                                   \
        } type;                                                               \
        CONT_TAG_NON_INTRUSIVE(dataType);                                     \
        struct {char _[NV_OFFSETOF(multimapTypeName##Leaf, node)];} *nodeOffset; \
        struct {char _[sizeof(multimapTypeName##Submap)];} *submapSize;       \
    } multimapTypeName;

#define DECLARE_MULTIMAP(multimapTypeName)                                    \
    typedef struct multimapTypeName##Leaf multimapTypeName##Leaf;             \
    DECLARE_INTRUSIVE_MAP(multimapTypeName##Submap);                          \
    DECLARE_MAP(multimapTypeName##Supermap);                                  \
    typedef union multimapTypeName##Iter multimapTypeName##Iter;              \
    typedef union multimapTypeName multimapTypeName

/**
 * @brief Internal node structure associated with multimap values.
 */
typedef struct MultimapNode MultimapNode;

/**
 * @brief Base type common to all multimap iterator types.
 */
typedef struct MultimapIterBase MultimapIterBase;

/**
 * @brief Base type common to all multimap types.
 */
typedef struct MultimapBase MultimapBase;

struct MultimapNode
{
    void *pSubmap;
    MapNode submapNode;
};

struct MultimapIterBase
{
    void *pValue;
    MultimapBase *pMultimap;
    void *pNext;
    void *pLast;
};

CONT_VTABLE_DECL(MultimapBase, MultimapIterBase);

struct MultimapBase
{
    CONT_VTABLE_FIELD(MultimapBase);
    NonIntrusiveMap map;
    NvS32 multimapNodeOffset;
    NvU32 itemCount;
    NvU32 itemSize;
};


#define multimapInit(pMultimap, pAllocator)                                   \
    multimapInit_IMPL(&(pMultimap)->real.base, pAllocator,                    \
                      sizeof(*(pMultimap)->valueSize),                        \
                      sizeof(*(pMultimap)->nodeOffset),                       \
                      sizeof(*(pMultimap)->submapSize))

#define multimapDestroy(pMultimap)                                            \
    multimapDestroy_IMPL(&(pMultimap)->real.base)

#define multimapClear(pMultimap)                                              \
    multimapClear_IMPL(&(pMultimap)->real.base)

#define multimapCountSubmaps(pMultimap)                                       \
    mapCount(&(pMultimap)->type.map)

#define multimapCountItems(pMultimap)                                         \
    (pMultimap)->real.base.itemCount

#define multimapFindSubmap(pMultimap, submapKey)                              \
    CONT_CAST_ELEM(&(pMultimap)->type.map,                                    \
                   multimapFindSubmap_IMPL(&(pMultimap)->real.base, submapKey))

#define multimapFindSubmapLEQ(pMultimap, submapKey)                           \
    CONT_CAST_ELEM(&(pMultimap)->type.map,                                    \
		multimapFindSubmapLEQ_IMPL(&(pMultimap)->real.base, submapKey))

#define multimapFindSubmapGEQ(pMultimap, submapKey)                           \
    CONT_CAST_ELEM(&(pMultimap)->type.map,                                    \
		multimapFindSubmapGEQ_IMPL(&(pMultimap)->real.base, submapKey))

#define multimapCountSubmapItems(pMultimap, pSubmap)                          \
    mapCount(pSubmap)

#define multimapInsertItemNew(pMultimap, submapKey, itemKey)                  \
    CONT_CAST_ELEM(pMultimap,                                                 \
        multimapInsertItemNew_IMPL(&(pMultimap)->real.base, submapKey, itemKey))

#define multimapInsertItemValue(pMultimap, submapKey, itemKey, pValue)        \
    CONT_CAST_ELEM(pMultimap,                                                 \
        multimapInsertItemValue_IMPL(&(pMultimap)->real.base,                 \
                                     submapKey, itemKey, pValue))

#define multimapInsertSubmap(pMultimap, submapKey)                            \
    CONT_CAST_ELEM(&(pMultimap)->type.map,                                    \
		multimapInsertSubmap_IMPL(&(pMultimap)->real.base, submapKey))

#define multimapFindItem(pMultimap, submapKey, itemKey)                       \
    CONT_CAST_ELEM(pMultimap,                                                 \
        multimapFindItem_IMPL(&(pMultimap)->real.base, submapKey, itemKey))

#define multimapRemoveItem(pMultimap, pValue)                                 \
    multimapRemoveItem_IMPL(&(pMultimap)->real.base, pValue)

#define multimapRemoveSubmap(pMultimap, pSubmap)                              \
    multimapRemoveSubmap_IMPL(&(pMultimap)->real.base, &(pSubmap)->real.base)

#define multimapRemoveItemByKey(pMultimap, submapKey, itemKey)                \
    multimapRemoveItemByKey_IMPL(&(pMultimap)->real.base, submapKey, itemKey)

#define multimapNextItem(pMultimap, pValue)                                   \
    CONT_CAST_ELEM(pMultimap,                                                 \
                   multimapNextItem_IMPL(&(pMultimap)->real.base, pValue))

#define multimapPrevItem(pMultimap, pValue)                                   \
    CONT_CAST_ELEM(pMultimap,                                                 \
                   multimapPrevItem_IMPL(&(pMultimap)->real.base, pValue))

#define multimapFirstItem(pMultimap)                                          \
    CONT_CAST_ELEM(pMultimap, multimapFirstItem_IMPL(&(pMultimap)->real.base))

#define multimapLastItem(pMultimap)                                          \
    CONT_CAST_ELEM(pMultimap, multimapLastItem_IMPL(&(pMultimap)->real.base))

#define multimapItemIterAll(pMultimap)                                        \
    multimapItemIterRange(pMultimap,                                          \
        multimapFirstItem(pMultimap), multimapLastItem(pMultimap))

#define multimapItemIterRange(pMultimap, pFirst, pLast)                       \
	CONT_ITER_RANGE(pMultimap, multimapItemIterRange_IMPL,                    \
        CONT_CHECK_ARG(pMultimap, pFirst), CONT_CHECK_ARG(pMultimap, pLast))

#define multimapSubmapIterItems(pMultimap, pSubmap)                           \
    multimapItemIterRange(pMultimap,                                          \
        &mapFindGEQ(pSubmap, 0)->data, &mapFindLEQ(pSubmap, NV_U64_MAX)->data)

#define multimapItemIterNext(pIt)                                             \
    multimapItemIterNext_IMPL(&(pIt)->iter)

#define multimapSubmapIterAll(pMultimap)                                      \
    mapIterAll(&(pMultimap)->type.map)

#define multimapSubmapIterRange(pMultimap, pFirst, pLast)                     \
    mapIterRange(&(pMultimap)->type.map, pFirst, pLast)

#define multimapSubmapIterNext(pIt)                                           \
    mapIterNext(pIt)

#define multimapItemKey(pMultimap, pValue)                                    \
    multimapValueToNode(&(pMultimap)->real.base, pValue)->submapNode.key

#define multimapSubmapKey(pMultimap, pSubmap)                                 \
    mapKey(&(pMultimap)->type.map, pSubmap)

void multimapInit_IMPL(MultimapBase *pBase, PORT_MEM_ALLOCATOR *pAllocator,
		       NvU32 valueSize, NvS32 nodeOffset, NvU32 submapSize);
void multimapRemoveSubmap_IMPL(MultimapBase *pMultimap, MapBase *submap);
void multimapDestroy_IMPL(MultimapBase *pBase);
void multimapClear_IMPL(MultimapBase *pBase);

void *multimapInsertSubmap_IMPL(MultimapBase *pBase, NvU64 submapKey);

void *multimapFindSubmap_IMPL(MultimapBase *pBase, NvU64 submapKey);
void *multimapFindSubmapLEQ_IMPL(MultimapBase *pBase, NvU64 submapKey);
void *multimapFindSubmapGEQ_IMPL(MultimapBase *pBase, NvU64 submapKey);

void *multimapInsertItemNew_IMPL(MultimapBase *pBase, NvU64 submapKey,
				 NvU64 itemKey);
void *multimapInsertItemValue_IMPL(MultimapBase *pBase, NvU64 submapKey,
				   NvU64 itemKey, void *pValue);

void *multimapFindItem_IMPL(MultimapBase *pBase, NvU64 submapKey,
			    NvU64 itemKey);

void multimapRemoveItem_IMPL(MultimapBase *pBase, void *pLeaf);
void multimapRemoveItemByKey_IMPL(MultimapBase *pBase, NvU64 submapKey,
				  NvU64 itemKey);

void *multimapNextItem_IMPL(MultimapBase *pBase, void *pValue);
void *multimapPrevItem_IMPL(MultimapBase *pBase, void *pValue);

void *multimapFirstItem_IMPL(MultimapBase *pBase);
void *multimapLastItem_IMPL(MultimapBase *pBase);

MultimapIterBase multimapItemIterRange_IMPL(MultimapBase *pBase,
					    void *pFirst, void *pLast);
NvBool multimapItemIterNext_IMPL(MultimapIterBase *pIt);

static NV_FORCEINLINE MultimapNode *
multimapValueToNode(MultimapBase *pBase, void *pValue)
{
    if (NULL == pBase || NULL == pValue) return NULL;

    return (MultimapNode *)((NvU8*)pValue + pBase->multimapNodeOffset);
}
static NV_FORCEINLINE void *
multimapNodeToValue(MultimapBase *pBase, MultimapNode *pNode)
{
    if (NULL == pBase || NULL == pNode) return NULL;

    return (NvU8*)pNode - pBase->multimapNodeOffset;
}

#ifdef __cplusplus
}
#endif

#endif // _NV_CONTAINERS_MULTIMAP_H_
