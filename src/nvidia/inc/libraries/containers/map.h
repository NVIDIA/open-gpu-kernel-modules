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
#ifndef _NV_CONTAINERS_MAP_H_
#define _NV_CONTAINERS_MAP_H_

// Contains mix of C/C++ declarations.
#include "containers/type_safety.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"
#include "nvmisc.h"
#include "nvport/nvport.h"
#include "utils/nvassert.h"

/**
 * @defgroup NV_CONTAINERS_MAP Map
 *
 * @brief Map (ordered) from 64-bit integer keys to user-defined values.
 *
 * @details The provided interface is abstract, decoupling the user from the
 * underlying ordered map implementation. Two options are available with regard
 * to memory management, intrusive and non-intrusive. Users can select either
 * one based on different situations. Despite the two versions of the map,
 * the following implementation constraints are guaranteed.
 *
 * - Time Complexity:
 *  * Operations are \b O(log N),
 *  * Unless stated otherwise,
 *  * Where N is the number of values in the map.
 *
 * - Memory Usage:
 *  * \b O(N) memory is required for N values.
 *  * Intrusive and non-intrusive variants are provided.
 *    See @ref mem-ownership for further details.
 *
 * - Synchronization:
 *  * \b None. The container is not thread-safe.
 *  * Locking must be handled by the user if required.
 *
 */

#define MAKE_MAP(mapTypeName, dataType)                                      \
    typedef union mapTypeName##Iter                                          \
    {                                                                        \
        dataType *pValue;                                                    \
        MapIterBase iter;                                                    \
    } mapTypeName##Iter;                                                     \
    typedef union mapTypeName                                                \
    {                                                                        \
        NonIntrusiveMap real;                                                \
        CONT_TAG_TYPE(MapBase, dataType, mapTypeName##Iter);                 \
        CONT_TAG_NON_INTRUSIVE(dataType);                                    \
    } mapTypeName

#define DECLARE_MAP(mapTypeName)                                             \
    typedef union mapTypeName##Iter mapTypeName##Iter;                       \
    typedef union mapTypeName mapTypeName

#define MAKE_INTRUSIVE_MAP(mapTypeName, dataType, node)                      \
    typedef union mapTypeName##Iter                                          \
    {                                                                        \
        dataType *pValue;                                                    \
        MapIterBase iter;                                                    \
    } mapTypeName##Iter;                                                     \
    typedef union mapTypeName                                                \
    {                                                                        \
        IntrusiveMap real;                                                   \
        CONT_TAG_TYPE(MapBase, dataType, mapTypeName##Iter);                 \
        CONT_TAG_INTRUSIVE(dataType, node);                                  \
    } mapTypeName

#define DECLARE_INTRUSIVE_MAP(mapTypeName)                                   \
    typedef union mapTypeName##Iter mapTypeName##Iter;                       \
    typedef union mapTypeName mapTypeName

/**
 * @brief Internal node structure to embed within intrusive map values.
 */
typedef struct MapNode MapNode;

/**
 * @brief Base type common to both intrusive and non-intrusive variants.
 */
typedef struct MapBase MapBase;

/**
 * @brief Non-intrusive map (container-managed memory).
 */
typedef struct NonIntrusiveMap NonIntrusiveMap;

/**
 * @brief Intrusive map (user-managed memory).
 */
typedef struct IntrusiveMap IntrusiveMap;

/**
 * @brief Iterator over a range of map values.
 *
 * See @ref iterators for usage details.
 */
typedef struct MapIterBase MapIterBase;

struct MapNode
{
    /// @privatesection
    NvU64       key;
    MapNode    *pParent;
    MapNode    *pLeft;
    MapNode    *pRight;
    NvBool      bIsRed;
#if PORT_IS_CHECKED_BUILD
    MapBase    *pMap;
#endif
};

struct MapIterBase
{
    void               *pValue;
    MapBase            *pMap;
    MapNode            *pNode;
    MapNode            *pLast;
#if PORT_IS_CHECKED_BUILD
    NvU32               versionNumber;
#endif
};

MapIterBase mapIterRange_IMPL(MapBase *pMap, void *pFirst, void *pLast);
CONT_VTABLE_DECL(MapBase, MapIterBase);

struct MapBase
{
    CONT_VTABLE_FIELD(MapBase);
    MapNode    *pRoot;
    NvS32       nodeOffset;
    NvU32       count;
#if PORT_IS_CHECKED_BUILD
    NvU32       versionNumber;
#endif
};

struct NonIntrusiveMap
{
    MapBase             base;
    PORT_MEM_ALLOCATOR *pAllocator;
    NvU32               valueSize;
};

struct IntrusiveMap
{
    MapBase             base;
};

#define mapInit(pMap, pAllocator)                                            \
    mapInit_IMPL(&((pMap)->real), pAllocator, sizeof(*(pMap)->valueSize))

#define mapInitIntrusive(pMap)                                               \
    mapInitIntrusive_IMPL(&((pMap)->real), sizeof(*(pMap)->nodeOffset))

#define mapDestroy(pMap)                                                     \
    CONT_DISPATCH_ON_KIND(pMap,                                              \
        mapDestroy_IMPL((NonIntrusiveMap*)&((pMap)->real)),                  \
        mapDestroyIntrusive_IMPL(&((pMap)->real.base)),                      \
        contDispatchVoid_STUB())

#define mapCount(pMap)                                                       \
    mapCount_IMPL(&((pMap)->real).base)

#define mapKey(pMap, pValue)                                                 \
    mapKey_IMPL(&((pMap)->real).base, pValue)

#define mapInsertNew(pMap, key)                                              \
    CONT_CAST_ELEM(pMap, mapInsertNew_IMPL(&(pMap)->real, key))

#define mapInsertValue(pMap, key, pValue)                                    \
    CONT_CAST_ELEM(pMap,                                                     \
        mapInsertValue_IMPL(&(pMap)->real, key,                              \
            CONT_CHECK_ARG(pMap, pValue)))

#define mapInsertExisting(pMap, key, pValue)                                 \
    mapInsertExisting_IMPL(&(pMap)->real, key,                               \
        CONT_CHECK_ARG(pMap, pValue))

#define mapRemove(pMap, pValue)                                              \
    CONT_DISPATCH_ON_KIND(pMap,                                              \
        mapRemove_IMPL((NonIntrusiveMap*)&((pMap)->real),                    \
            CONT_CHECK_ARG(pMap, pValue)),                                   \
        mapRemoveIntrusive_IMPL(&((pMap)->real).base,                        \
            CONT_CHECK_ARG(pMap, pValue)),                                   \
        contDispatchVoid_STUB())

#define mapClear(pMap)                                                       \
    mapDestroy(pMap)

#define mapRemoveByKey(pMap, key)                                            \
    CONT_DISPATCH_ON_KIND(pMap,                                              \
        mapRemoveByKey_IMPL((NonIntrusiveMap*)&((pMap)->real), key),         \
        mapRemoveByKeyIntrusive_IMPL(&((pMap)->real).base, key),             \
        contDispatchVoid_STUB())

#define mapFind(pMap, key)                                                   \
    CONT_CAST_ELEM(pMap, mapFind_IMPL(&((pMap)->real).base, key))

#define mapFindGEQ(pMap, keyMin)                                             \
    CONT_CAST_ELEM(pMap,                                                     \
        mapFindGEQ_IMPL(&((pMap)->real).base, keyMin))

#define mapFindLEQ(pMap, keyMax)                                             \
    CONT_CAST_ELEM(pMap,                                                     \
        mapFindLEQ_IMPL(&((pMap)->real).base, keyMax))

#define mapNext(pMap, pValue)                                                \
    CONT_CAST_ELEM(pMap,                                                     \
        mapNext_IMPL(&((pMap)->real).base,                                   \
            CONT_CHECK_ARG(pMap, pValue)))

#define mapPrev(pMap, pValue)                                                \
    CONT_CAST_ELEM(pMap,                                                     \
        mapPrev_IMPL(&((pMap)->real).base,                                   \
            CONT_CHECK_ARG(pMap, pValue)))

#define mapIterAll(pMap)                                                     \
    mapIterRange(pMap, mapFindGEQ(pMap, 0), mapFindLEQ(pMap, NV_U64_MAX))

#define mapIterRange(pMap, pFirst, pLast)                                    \
    CONT_ITER_RANGE(pMap, &mapIterRange_IMPL,                                \
        CONT_CHECK_ARG(pMap, pFirst), CONT_CHECK_ARG(pMap, pLast))

#define mapIterNext(pIt)                                                     \
    mapIterNext_IMPL(&((pIt)->iter))

void mapInit_IMPL(NonIntrusiveMap *pMap,
                  PORT_MEM_ALLOCATOR *pAllocator, NvU32 valueSize);
void mapInitIntrusive_IMPL(IntrusiveMap *pMap, NvS32 nodeOffset);
void mapDestroy_IMPL(NonIntrusiveMap *pMap);
void mapDestroyIntrusive_IMPL(MapBase *pMap);

NvU32 mapCount_IMPL(MapBase *pMap);
NvU64 mapKey_IMPL(MapBase *pMap, void *pValue);

void *mapInsertNew_IMPL(NonIntrusiveMap *pMap, NvU64 key);
void *mapInsertValue_IMPL(NonIntrusiveMap *pMap, NvU64 key, void *pValue);
NvBool mapInsertExisting_IMPL(IntrusiveMap *pMap, NvU64 key, void *pValue);
void mapRemove_IMPL(NonIntrusiveMap *pMap, void *pValue);
void mapRemoveIntrusive_IMPL(MapBase *pMap, void *pValue);
void mapRemoveByKey_IMPL(NonIntrusiveMap *pMap, NvU64 key);
void mapRemoveByKeyIntrusive_IMPL(MapBase *pMap, NvU64 key);

void *mapFind_IMPL(MapBase *pMap, NvU64 key);
void *mapFindGEQ_IMPL(MapBase *pMap, NvU64 keyMin);
void *mapFindLEQ_IMPL(MapBase *pMap, NvU64 keyMax);
void *mapNext_IMPL(MapBase *pMap, void *pValue);
void *mapPrev_IMPL(MapBase *pMap, void *pValue);

MapIterBase mapIterAll_IMPL(MapBase *pMap);
NvBool mapIterNext_IMPL(MapIterBase *pIt);

static NV_FORCEINLINE MapNode *
mapValueToNode(MapBase *pMap, void *pValue)
{
    if (NULL == pMap) return NULL;
    if (NULL == pValue) return NULL;
    return (MapNode*)((NvU8*)pValue + pMap->nodeOffset);
}

static NV_FORCEINLINE void *
mapNodeToValue(MapBase *pMap, MapNode *pNode)
{
    if (NULL == pMap) return NULL;
    if (NULL == pNode) return NULL;
    return (NvU8*)pNode - pMap->nodeOffset;
}

#ifdef __cplusplus
}
#endif

#endif // _NV_CONTAINERS_MAP_H_
