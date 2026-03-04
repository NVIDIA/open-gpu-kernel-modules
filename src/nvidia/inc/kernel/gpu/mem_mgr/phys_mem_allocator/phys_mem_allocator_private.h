/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef PHYS_MEM_ALLOCATOR_PRIVATE_H
#define PHYS_MEM_ALLOCATOR_PRIVATE_H

#include "nvport/nvport.h"
#include "map_defines.h"
#include "nvmisc.h"

#if defined(SRT_BUILD)
#define RMCFG_MODULE_x 1
#define RMCFG_FEATURE_x 1
#else
#include "rmconfig.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OBJMEMSCRUB OBJMEMSCRUB;
typedef struct SCRUB_NODE SCRUB_NODE;

#define PMA_REGION_SIZE 32

#define PMA_NUMA_NO_NODE      -1

// Maximum blacklist entries possible
#define PMA_MAX_BLACKLIST_ENTRIES 512

typedef struct _PMA_MAP_INFO PMA_MAP_INFO;
typedef struct _PMA PMA;

/*!
 * @brief Pluggable data structure management. Currently we have regmap.
 */
typedef void *(*pmaMapInit_t)(NvU64 numFrames, NvU64 addrBase, PMA_STATS *pPmaStats, NvBool bProtected);
typedef void  (*pmaMapDestroy_t)(void *pMap);
typedef void  (*pmaMapChangeStateAttrib_t)(void *pMap, NvU64 frameNum, PMA_PAGESTATUS newState, PMA_PAGESTATUS newStateMask);
typedef void  (*pmaMapChangePageStateAttrib_t)(void *pMap, NvU64 startFrame, NvU64 pageSize, PMA_PAGESTATUS newState, PMA_PAGESTATUS newStateMask);
typedef void  (*pmaMapChangeBlockStateAttrib_t)(void *pMap, NvU64 frameNum, NvU64 numFrames, PMA_PAGESTATUS newState, PMA_PAGESTATUS newStateMask);
typedef PMA_PAGESTATUS (*pmaMapRead_t)(void *pMap, NvU64 frameNum, NvBool readAttrib);
typedef NV_STATUS (*pmaMapScanContiguous_t)(void *pMap, NvU64 addrBase, NvU64 rangeStart, NvU64 rangeEnd,
                                            NvU64 numPages, NvU64 *freelist, NvU64 pageSize, NvU64 alignment,
                                            NvU64 stride, NvU32 strideStart,
                                            NvU64 *pagesAllocated, NvBool bSkipEvict, NvBool bReverseAlloc);
typedef NV_STATUS (*pmaMapScanDiscontiguous_t)(void *pMap, NvU64 addrBase, NvU64 rangeStart, NvU64 rangeEnd,
                                               NvU64 numPages, NvU64 *freelist, NvU64 pageSize, NvU64 alignment,
                                               NvU64 stride, NvU32 strideStart,
                                               NvU64 *pagesAllocated, NvBool bSkipEvict, NvBool bReverseAlloc);
typedef void (*pmaMapGetSize_t)(void *pMap, NvU64 *pBytesTotal);
typedef void (*pmaMapGetLargestFree_t)(void *pMap, NvU64 *pLargestFree, NvU64 *pLargestFreeBase);
typedef NV_STATUS (*pmaMapScanContiguousNumaEviction_t)(void *pMap, NvU64 addrBase, NvLength actualSize,
                                                        NvU64 pageSize, NvU64 *evictStart, NvU64 *evictEnd);
typedef NvU64 (*pmaMapGetEvictingFrames_t)(void *pMap);
typedef void (*pmaMapSetEvictingFrames_t)(void *pMap, NvU64 frameEvictionsInProcess);

struct _PMA_MAP_INFO
{
    NvU32                       mode;
    pmaMapInit_t                pmaMapInit;
    pmaMapDestroy_t             pmaMapDestroy;
    pmaMapChangeStateAttrib_t      pmaMapChangeStateAttrib;
    pmaMapChangePageStateAttrib_t  pmaMapChangePageStateAttrib;
    pmaMapChangeBlockStateAttrib_t pmaMapChangeBlockStateAttrib;
    pmaMapRead_t                pmaMapRead;
    pmaMapScanContiguous_t      pmaMapScanContiguous;
    pmaMapScanDiscontiguous_t   pmaMapScanDiscontiguous;
    pmaMapGetSize_t             pmaMapGetSize;
    pmaMapGetLargestFree_t      pmaMapGetLargestFree;
    pmaMapScanContiguousNumaEviction_t pmaMapScanContiguousNumaEviction;
    pmaMapGetEvictingFrames_t  pmaMapGetEvictingFrames;
    pmaMapSetEvictingFrames_t  pmaMapSetEvictingFrames;
};

struct _PMA
{
    PORT_SPINLOCK           *pPmaLock;                          // PMA-wide lock
    PORT_MUTEX              *pEvictionCallbacksLock;            // Eviction callback registration lock

    // Only used when free scrub-on-free feature is turned on
    PORT_RWLOCK             *pScrubberValidLock;                // A reader-writer lock to protect the scrubber valid bit
    PORT_MUTEX              *pAllocLock;                        // Used to protect page stealing in the allocation path

    // Region related states
    NvU32                   regSize;                            // Actual size of regions array
    void *                  pRegions[PMA_REGION_SIZE];          // All the region maps stored as opaque pointers
    NvU32                   *pSortedFastFirst;                  // Pre-sorted array of region IDs
    PMA_REGION_DESCRIPTOR   *pRegDescriptors [PMA_REGION_SIZE]; // Stores the descriptions of each region
    PMA_MAP_INFO            *pMapInfo;                          // The pluggable layer for managing scanning

    // Allocation related states
    void *                  evictCtxPtr;                        // Opaque context pointer for eviction callback
    pmaEvictPagesCb_t       evictPagesCb;                       // Discontiguous eviction callback
    pmaEvictRangeCb_t       evictRangeCb;                       // Contiguous eviction callback
    NvU64                   frameAllocDemand;                   // Frame count of allocations in-process
    NvBool                  bForcePersistence;                  // Force all allocations to persist across suspend/resume
    PMA_STATS               pmaStats;                           // PMA statistics used for client heuristics

    // Scrubber related states
    NvSPtr                  initScrubbing;                      // If the init scrubber has finished in this PMA
    NvBool                  bScrubOnFree;                       // If "scrub on free" is enabled for this PMA object
    NvSPtr                  scrubberValid;                      // If scrubber object is valid, using atomic variable to prevent races
    OBJMEMSCRUB            *pScrubObj;                          // Object to store the FreeScrub header

    // NUMA states
    NvBool                  bNuma;                              // If we are allocating for a NUMA system
    NvBool                  nodeOnlined;                        // If node is onlined
    NvS32                   numaNodeId;                         // Current Node ID, set at initialization. -1 means invalid
    NvU64                   coherentCpuFbBase;                  // Used to calculate FB offset from bus address
    NvU64                   coherentCpuFbSize;                  // Used for error checking only
    NvU32                   numaReclaimSkipThreshold;           // percent value below which __GFP_RECLAIM will not be used.
    NvBool                  bNumaAutoOnline;                    // If NUMA memory is auto-onlined

    // Blacklist related states
    PMA_BLACKLIST_CHUNK    *pBlacklistChunks;                   // Tracking for blacklist pages
    NvU32                   blacklistCount;                     // Number of blacklist pages
    NvBool                  bClientManagedBlacklist;            // Blacklisted pages in PMA that will be taken over by Client

    // RUSD Callback
    pmaUpdateStatsCb_t      pStatsUpdateCb;                     // RUSD update free pages
    void                   *pStatsUpdateCtx;                    // Context for RUSD update
};

#ifdef __cplusplus
}
#endif

#endif // PHYS_MEM_ALLOCATOR_PRIVATE_H
