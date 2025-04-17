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

// Only return single range.
#define REUSE_MAPPING_DB_MAP_FLAGS_SINGLE_RANGE NVBIT(0)
// No reuse, call the map callback directly.
#define REUSE_MAPPING_DB_MAP_FLAGS_NO_REUSE  NVBIT(1)

// Remove this from defaults when we support multi-range reuse.
#define REUSE_MAPPING_DB_MAP_FLAGS_DEFAULT REUSE_MAPPING_DB_MAP_FLAGS_SINGLE_RANGE

typedef struct ReuseMappingDbEntry  ReuseMappingDbEntry;
typedef struct ReuseMappingDbEntry {
    NvU64 size;
    NvU64 refCount;

    //
    // trackingInfo is the data used to locate this entry within the reuse database.
    // This is in union with newMappingNode, which is a node used in a linked list when we
    // create new entries, before we actually insert into the reuse database.
    //
    union
    {
        struct
        {
            void *pAllocCtx;
            MapNode virtualNode;
            MapNode physicalNode;
        } trackingInfo;
        struct
        {
            ReuseMappingDbEntry *pNextEntry;
            NvU64 virtualOffset;
            NvU64 physicalOffset;
        } newMappingNode;
    };
} ReuseMappingDbEntry;

MAKE_INTRUSIVE_MAP(ReuseMappingDbPhysicalMap, ReuseMappingDbEntry, trackingInfo.physicalNode);
MAKE_INTRUSIVE_MAP(ReuseMappingDbVirtualMap, ReuseMappingDbEntry, trackingInfo.virtualNode);

//
// There are 2 levels of mapping here: the first maps from a given allocation context to a physical
// map, and a second that is an ordered map of physical offsets. All physical offsets are referenced
// to a given allocation context, and are not valid on there own (ie are not real/unique memory offsets) 
//
MAKE_MAP(ReuseMappingDbAllocCtxMap, ReuseMappingDbPhysicalMap);

//
// This function is called by the ReuseMappingDbMapFunction when it intends to add new mapped ranges.
// pToken is the same token passed into ReuseMappingDbMapFunction.
//
typedef NV_STATUS (*ReuseMappingDbAddMappingCallback)(void *pToken, NvU64 physicalOffset, NvU64 virtualOffset, NvU64 size);

// Map callback when the database doesn't contained cached mappings
typedef NV_STATUS (*ReuseMappingDbMapFunction)(void *pGlobalCtx, void *pAllocCtx, MemoryRange physicalRange, NvU64 cachingFlags, void *pToken, ReuseMappingDbAddMappingCallback fn);
// Unmap callback when refcount of any mapped range reaches 0
typedef void (*ReuseMappingDbUnnmapFunction)(void *pGlobalCtx, void *pAllocCtx, MemoryRange virtualRange);
// Callback for when a node is split in two. Performs any tracking or cleanup necessary.
typedef NV_STATUS (*ReuseMappingDbSplitMappingFunction)(void *pGlobalCtx, void *pAllocCtx, MemoryRange virtualRange, NvU64 boundary);

typedef struct ReuseMappingDb
{
    ReuseMappingDbAllocCtxMap allocCtxPhysicalMap;
    ReuseMappingDbVirtualMap virtualMap;
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

void reusemappingdbUnmap(ReuseMappingDb *pReuseMappingDb, void *pAllocCtx, MemoryRange range);

#ifdef __cplusplus
}
#endif

#endif // MAPPING_REUSE_H
