/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef MAPPING_REUSE_H
#define MAPPING_REUSE_H

#include "containers/map.h"
#include "os/nv_memory_area.h"

#ifdef __cplusplus
extern "C" {
#endif

#define REUSE_MAPPING_DB_MAP_FLAGS_REUSE_ANY_RANGES 0x0
#define REUSE_MAPPING_DB_MAP_FLAGS_REUSE_NONE 0x1
#define REUSE_MAPPING_DB_MAP_FLAGS_REUSE_SINGLE_RANGE 0x2

#define REUSE_MAPPING_DB_MAP_FLAGS_DEFAULT REUSE_MAPPING_DB_MAP_FLAGS_REUSE_ANY_RANGES

typedef struct ReuseMappingDbEntry  ReuseMappingDbEntry;
typedef struct ReuseMappingDbEntry {
    NvU64 size;
    NvU64 refCount;
    struct
    {
        void *pAllocCtx;
        MapNode reverseNode;
        MapNode forwardNode;
    } trackingInfo;
} ReuseMappingDbEntry;

MAKE_INTRUSIVE_MAP(ReuseMappingDbForwardMap, ReuseMappingDbEntry, trackingInfo.forwardNode);
MAKE_INTRUSIVE_MAP(ReuseMappingDbReverseMap, ReuseMappingDbEntry, trackingInfo.reverseNode);

MAKE_MAP(ReuseMappingDbAllocCtxMap, ReuseMappingDbForwardMap);

typedef NV_STATUS (*ReuseMappingDbAddMappingCallback)(void *pToken, NvU64 forwardOffset, NvU64 reverseOffset, NvU64 size);

typedef NV_STATUS (*ReuseMappingDbMapFunction)(void *pGlobalCtx, void *pAllocCtx, MemoryRange forwardRange, void *pToken, ReuseMappingDbAddMappingCallback fn);
typedef void (*ReuseMappingDbUnnmapFunction)(void *pGlobalCtx, void *pAllocCtx, MemoryRange reverseRange);

typedef NV_STATUS (*ReuseMappingDbSplitMappingFunction)(void *pGlobalCtx, void *pAllocCtx, MemoryRange reverseRange, NvU64 boundary);

typedef struct ReuseMappingDb
{
    ReuseMappingDbAllocCtxMap forwardMap;
    ReuseMappingDbReverseMap reverseMap;
    void *pGlobalCtx;
    PORT_MEM_ALLOCATOR *pAllocator;

    ReuseMappingDbMapFunction pMapCb;
    ReuseMappingDbUnnmapFunction pUnmapCb;
    ReuseMappingDbSplitMappingFunction pSplitCb;
} ReuseMappingDb;

void reusemappingdbInit(ReuseMappingDb *pReuseMappingDb, PORT_MEM_ALLOCATOR *pAllocator,
        void *pGlobalCtx, ReuseMappingDbMapFunction pMapCb, ReuseMappingDbUnnmapFunction pUnmapCb,
        ReuseMappingDbSplitMappingFunction pSplitCb);

void reusemappingdbDestruct(ReuseMappingDb *pReuseMappingDb);

NV_STATUS reusemappingdbMap(ReuseMappingDb *pReuseMappingDb, void *pAllocCtx, MemoryRange range,
        MemoryArea *pMemoryArea, NvU64 cachingFlags);

void reusemappingdbUnmap(ReuseMappingDb *pReuseMappingDb, MemoryRange range);

#ifdef __cplusplus
}
#endif

#endif // MAPPING_REUSE_H
