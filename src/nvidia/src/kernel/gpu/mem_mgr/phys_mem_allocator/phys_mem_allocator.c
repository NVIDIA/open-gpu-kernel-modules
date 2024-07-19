/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file
 * @brief The PMA implementation file.
 * This file implements the PMA object and the public interfaces.
 *
 * @bug
 *  1. SLI broadcast -- Not implemented
 */

#include "gpu/mem_mgr/phys_mem_allocator/phys_mem_allocator.h"
#include "gpu/mem_mgr/phys_mem_allocator/phys_mem_allocator_util.h"
#include "gpu/mem_mgr/phys_mem_allocator/numa.h"
#include "gpu/mem_mgr/mem_scrub.h"
#include "utils/nvprintf.h"
#include "utils/nvassert.h"

#if !defined(SRT_BUILD)
// These files are not found on SRT builds
#include "os/os.h"
#define PMA_DEBUG 1
#else
// disable PMA debug prints on SRTs to not bloat the logs
#define PMA_DEBUG 0
NV_STATUS pmaNumaAllocate
(
    PMA                    *pPma,
    NvLength                allocationCount,
    NvU64                   pageSize,
    PMA_ALLOCATION_OPTIONS *allocationOptions,
    NvU64                  *pPages
)
{
    return NV_ERR_GENERIC;
}

void pmaNumaFreeInternal
(
    PMA   *pPma,
    NvU64 *pPages,
    NvU64  pageCount,
    NvU64  size,
    NvU32  flag
)
{
    return;
}
void pmaNumaSetReclaimSkipThreshold(PMA *pPma, NvU32 data)
{
    return;
}
#endif

typedef NV_STATUS (*scanFunc)(void *, NvU64, NvU64, NvU64, NvU64, NvU64*, NvU64, NvU64, NvU64, NvU32, NvU64*, NvBool, NvBool);

static void
_pmaRollback
(
    PMA           *pPma,
    NvU64         *pPages,
    NvU32          failCount,
    NvU32          failFrame,
    NvU64          pageSize,
    PMA_PAGESTATUS oldState
)
{
    NvU32 framesPerPage, regId, i, j;
    NvU64 frameNum, addrBase;

    framesPerPage = (NvU32)(pageSize >> PMA_PAGE_SHIFT);
    if (failCount != 0)
    {
        for(i = 0; i < failCount; i++)
        {
            regId = findRegionID(pPma, pPages[i]);
            addrBase = pPma->pRegDescriptors[regId]->base;
            frameNum = PMA_ADDR2FRAME(pPages[i], addrBase);

            for (j = 0; j < framesPerPage; j++)
            {
                pPma->pMapInfo->pmaMapChangeStateAttrib(pPma->pRegions[regId], (frameNum + j), oldState, STATE_MASK);
            }
        }
        pPma->pStatsUpdateCb(pPma->pStatsUpdateCtx, pPma->pmaStats.numFreeFrames);
    }

    if (failFrame != 0)
    {
        // might fail half-way through
        regId = findRegionID(pPma, pPages[failCount]);
        addrBase = pPma->pRegDescriptors[regId]->base;
        frameNum = PMA_ADDR2FRAME(pPages[failCount], addrBase);
        for(i = 0; i < failFrame; i++)
        {
            pPma->pMapInfo->pmaMapChangeStateAttrib(pPma->pRegions[regId], (frameNum + i), oldState, STATE_MASK);
        }
        pPma->pStatsUpdateCb(pPma->pStatsUpdateCtx, pPma->pmaStats.numFreeFrames);
    }
}

static inline void
_pmaDefaultStatsCallback
(
    void *pCtx,
    NvU64 freeFrames
)
{
    return;
}

///////////////////////////////////////////////////////////////////////////////
//
// Public interfaces
//

NV_STATUS
pmaInitialize(PMA *pPma, NvU32 initFlags)
{
    NV_STATUS status = NV_OK;
    PMA_MAP_INFO *pMapInfo;

    if (pPma == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pPma->pPmaLock = NULL;
    pPma->pEvictionCallbacksLock = NULL;

    // Assume portMemInitialize() has been called
    pPma->pPmaLock = (PORT_SPINLOCK *)portMemAllocNonPaged(portSyncSpinlockSize);
    if (pPma->pPmaLock == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    status = portSyncSpinlockInitialize(pPma->pPmaLock);
    if (status != NV_OK)
    {
        portMemFree(pPma->pPmaLock);
        pPma->pPmaLock = NULL;
        goto error;
    }

    pPma->pEvictionCallbacksLock = (PORT_MUTEX *)portMemAllocNonPaged(portSyncMutexSize);
    if (pPma->pEvictionCallbacksLock == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    status = portSyncMutexInitialize(pPma->pEvictionCallbacksLock);
    if (status != NV_OK)
    {
        portMemFree(pPma->pEvictionCallbacksLock);
        pPma->pEvictionCallbacksLock = NULL;
        goto error;
    }

    pPma->pAllocLock = (PORT_MUTEX *)portMemAllocNonPaged(portSyncMutexSize);
    if (pPma->pAllocLock == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    status = portSyncMutexInitialize(pPma->pAllocLock);
    if (status != NV_OK)
    {
        portMemFree(pPma->pAllocLock);
        pPma->pAllocLock = NULL;
        goto error;
    }

    pPma->pScrubberValidLock = (PORT_RWLOCK *)portMemAllocNonPaged(portSyncRwLockSize);
    if (pPma->pScrubberValidLock == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    pMapInfo = (PMA_MAP_INFO *)portMemAllocNonPaged(sizeof(struct _PMA_MAP_INFO));
    if (pMapInfo == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    status = portSyncRwLockInitialize(pPma->pScrubberValidLock);
    if (status != NV_OK)
    {
        portMemFree(pPma->pScrubberValidLock);
        pPma->pScrubberValidLock = NULL;
        goto error;
    }

    //
    // Initialize all the scanning callbacks to lower layer
    // Default use regmap
    //
    pMapInfo->pmaMapInit = pmaRegmapInit;
    pMapInfo->pmaMapDestroy = pmaRegmapDestroy;
    pMapInfo->pmaMapChangeStateAttrib = pmaRegmapChangeStateAttrib;
    pMapInfo->pmaMapChangePageStateAttrib = pmaRegmapChangePageStateAttrib;
    pMapInfo->pmaMapChangeBlockStateAttrib = pmaRegmapChangeBlockStateAttrib;
    pMapInfo->pmaMapRead = pmaRegmapRead;
    pMapInfo->pmaMapScanContiguous = pmaRegmapScanContiguous;
    pMapInfo->pmaMapScanDiscontiguous = pmaRegmapScanDiscontiguous;
    pMapInfo->pmaMapGetSize = pmaRegmapGetSize;
    pMapInfo->pmaMapGetLargestFree = pmaRegmapGetLargestFree;
    pMapInfo->pmaMapScanContiguousNumaEviction = pmaRegMapScanContiguousNumaEviction;
    pMapInfo->pmaMapGetEvictingFrames = pmaRegmapGetEvictingFrames;
    pMapInfo->pmaMapSetEvictingFrames = pmaRegmapSetEvictingFrames;

    if (initFlags != PMA_INIT_NONE)
    {
        pPma->bForcePersistence = !!(initFlags & PMA_INIT_FORCE_PERSISTENCE);

        // If scrubber feature is enable, PMA is not valid until scrubber registration
        if (initFlags & PMA_INIT_SCRUB_ON_FREE)
        {
            portAtomicSetSize(&pPma->scrubberValid, PMA_SCRUBBER_INVALID);
        }
        pPma->bScrubOnFree = !!(initFlags & PMA_INIT_SCRUB_ON_FREE);

        // If running on NUMA system, we cannot allocate from OS until node is onlined
        if (initFlags & PMA_INIT_NUMA)
        {
            pPma->nodeOnlined = NV_FALSE;
        }
        pPma->bNuma = !!(initFlags & PMA_INIT_NUMA);

        pPma->bNumaAutoOnline = !!(initFlags & PMA_INIT_NUMA_AUTO_ONLINE);
    }
    pPma->pMapInfo = pMapInfo;

    pPma->pmaStats.numFreeFrames = 0;
    pPma->pmaStats.num2mbPages = 0;
    pPma->pmaStats.numFree2mbPages = 0;
    pPma->pmaStats.numFreeFramesProtected = 0;
    pPma->pmaStats.num2mbPagesProtected = 0;
    pPma->pmaStats.numFree2mbPagesProtected = 0;
    pPma->regSize = 0;
    portAtomicSetSize(&pPma->initScrubbing, PMA_SCRUB_INITIALIZE);

    pPma->pStatsUpdateCtx = NULL;
    pPma->pStatsUpdateCb = _pmaDefaultStatsCallback;

    // OK not to take lock since it's initialization
    NV_ASSERT(pmaStateCheck(pPma));

    return NV_OK;

error:
    pmaDestroy(pPma);
    return status;
}

NV_STATUS
pmaQueryConfigs(PMA *pPma, NvU32 *pConfig)
{
    NvU32 config = 0;

    if (pPma == NULL || pConfig == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    if (pPma->bScrubOnFree)
    {
        config |= PMA_QUERY_SCRUB_ENABLED;

        portSyncRwLockAcquireRead(pPma->pScrubberValidLock);
        if (pmaPortAtomicGet(&pPma->scrubberValid) == PMA_SCRUBBER_VALID)
        {
            config |= PMA_QUERY_SCRUB_VALID;
        }
        portSyncRwLockReleaseRead(pPma->pScrubberValidLock);
    }
    config |= pPma->bNuma ? (PMA_QUERY_NUMA_ENABLED) : 0;

    portSyncSpinlockAcquire(pPma->pPmaLock);
    config |= pPma->nodeOnlined ? (PMA_QUERY_NUMA_ONLINED) : 0;
    portSyncSpinlockRelease(pPma->pPmaLock);

    // Only expose the states the clients asked for
    *pConfig = (*pConfig) & config;
    return NV_OK;
}

NV_STATUS
pmaRegMemScrub(PMA *pPma, OBJMEMSCRUB *pScrubObj)
{
    NV_ASSERT(pPma && pPma->bScrubOnFree);
    portSyncRwLockAcquireWrite(pPma->pScrubberValidLock);
    pPma->pScrubObj = pScrubObj;
    portAtomicSetSize(&pPma->scrubberValid, PMA_SCRUBBER_VALID);
    portSyncRwLockReleaseWrite(pPma->pScrubberValidLock);

    return NV_OK;
}

void
pmaUnregMemScrub(PMA *pPma)
{
    NV_ASSERT(pPma && pPma->bScrubOnFree);
    portSyncRwLockAcquireWrite(pPma->pScrubberValidLock);
    portAtomicSetSize(&pPma->scrubberValid, PMA_SCRUBBER_INVALID);
    pPma->pScrubObj = NULL;
    portSyncRwLockReleaseWrite(pPma->pScrubberValidLock);
}

NV_STATUS
pmaNumaOnlined(PMA *pPma, NvS32 numaNodeId,
               NvU64 coherentCpuFbBase, NvU64 coherentCpuFbSize)
{
    if ((pPma == NULL) || (!pPma->bNuma) ||
        (numaNodeId == PMA_NUMA_NO_NODE))
    {
        NV_ASSERT(0);
        return NV_ERR_INVALID_STATE;
    }

    portSyncSpinlockAcquire(pPma->pPmaLock);
    pPma->nodeOnlined = NV_TRUE;
    pPma->numaNodeId = numaNodeId;
    pPma->coherentCpuFbBase = coherentCpuFbBase;
    pPma->coherentCpuFbSize = coherentCpuFbSize;
    portSyncSpinlockRelease(pPma->pPmaLock);

    return NV_OK;
}


void
pmaNumaOfflined(PMA *pPma)
{
    if ((pPma == NULL) || (!pPma->bNuma))
    {
        NV_ASSERT(0);
        return;
    }

    portSyncSpinlockAcquire(pPma->pPmaLock);
    pPma->nodeOnlined = NV_FALSE;
    pPma->numaNodeId = PMA_NUMA_NO_NODE;
    portSyncSpinlockRelease(pPma->pPmaLock);
}


void
pmaDestroy(PMA *pPma)
{
    NvU32 i;

    NV_ASSERT(pPma != NULL);

    NV_ASSERT(pmaStateCheck(pPma));

    if (pmaPortAtomicGet(&pPma->initScrubbing) == PMA_SCRUB_IN_PROGRESS)
    {
        pmaScrubComplete(pPma);
    }

    if (pPma->bNuma)
    {
        if (pPma->nodeOnlined != NV_FALSE)
        {
            //
            // Not really an error right now but it will be later, when we are able
            // to offline memory.
            //
            NV_PRINTF(LEVEL_WARNING, "Destroying PMA before node %d is offlined\n",
                                     pPma->numaNodeId);
        }
    }

    for (i = 0; i < pPma->regSize; i++)
    {
        pPma->pMapInfo->pmaMapDestroy((void *)pPma->pRegions[i]);
        portMemFree(pPma->pRegDescriptors[i]);
    }
    pPma->regSize = 0;

    if (pPma->blacklistCount != 0)
    {
        portMemFree(pPma->pBlacklistChunks);
    }

    portMemFree(pPma->pMapInfo);

    if (pPma->pAllocLock != NULL)
    {
        portSyncMutexDestroy(pPma->pAllocLock);
        portMemFree(pPma->pAllocLock);
    }

    if (pPma->pScrubberValidLock != NULL)
    {
        portSyncRwLockDestroy(pPma->pScrubberValidLock);
        portMemFree(pPma->pScrubberValidLock);
    }

    if (pPma->pEvictionCallbacksLock != NULL)
    {
        portSyncMutexDestroy(pPma->pEvictionCallbacksLock);
        portMemFree(pPma->pEvictionCallbacksLock);
    }

    if (pPma->pPmaLock != NULL)
    {
        portSyncSpinlockDestroy(pPma->pPmaLock);
        portMemFree(pPma->pPmaLock);
    }
}


NV_STATUS
pmaRegisterRegion
(
    PMA                   *pPma,
    NvU32                  id,
    NvBool                 bAsyncEccScrub,
    PMA_REGION_DESCRIPTOR *pRegionDesc,
    NvU32                  blacklistCount,
    PPMA_BLACKLIST_ADDRESS pBlacklistPageBase
)
{
    NvU64 numFrames;
    void *pMap;
    NvU64 physBase, physLimit;
    NV_STATUS status = NV_OK;

    if (pPma == NULL || pRegionDesc == NULL || id != pPma->regSize
        || (pBlacklistPageBase == NULL && blacklistCount != 0))
    {
        if (pPma == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "ERROR: NULL PMA object\n");
        }
        else if (id != pPma->regSize)
        {
            NV_PRINTF(LEVEL_ERROR, "ERROR: Non-consecutive region ID %d (should be %d)\n",
                id, pPma->regSize);
        }
        if (pRegionDesc == NULL)
            NV_PRINTF(LEVEL_ERROR, "ERROR: NULL region descriptor\n");
        if (pBlacklistPageBase == NULL && blacklistCount != 0)
            NV_PRINTF(LEVEL_ERROR, "ERROR: Blacklist failure.  List is NULL but count = %d\n",
                blacklistCount);

        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pPma->bNuma)
    {
        NV_PRINTF(LEVEL_WARNING, "WARNING: registering regions on NUMA system.\n");
    }

    physBase = pRegionDesc->base;
    physLimit = pRegionDesc->limit;

    if (!NV_IS_ALIGNED(physBase, PMA_GRANULARITY) ||
        !NV_IS_ALIGNED((physLimit + 1), PMA_GRANULARITY))
    {
        NV_PRINTF(LEVEL_ERROR, "ERROR: Region range %llx..%llx unaligned\n",
            physBase, physLimit);
        // just try to check alignment on 64KB boundaries
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_ASSERT(pmaStateCheck(pPma));

    numFrames = (physLimit - physBase + 1) >> PMA_PAGE_SHIFT;

    pMap = pPma->pMapInfo->pmaMapInit(numFrames, physBase, &pPma->pmaStats,
                                      pRegionDesc->bProtected);
    if (pMap == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    pPma->pRegions[id] = pMap;

    // Deep copy of descriptor
    pPma->pRegDescriptors[id] =
      (PMA_REGION_DESCRIPTOR *) portMemAllocNonPaged(sizeof(PMA_REGION_DESCRIPTOR));
    portMemCopy(pPma->pRegDescriptors[id], sizeof(PMA_REGION_DESCRIPTOR),
        pRegionDesc, sizeof(PMA_REGION_DESCRIPTOR));

    pPma->regSize++;

    if (bAsyncEccScrub)
    {
        //
        // Scrubbing cannot be done before we start. This is to protect against spurious pmaScrubComplete
        // calls from RM
        //
        NV_ASSERT(pmaPortAtomicGet(&pPma->initScrubbing) != PMA_SCRUB_DONE);

        // Mark region as "scrubbing" until background scrubbing completes
        pmaSetBlockStateAttrib(pPma, physBase, physLimit - physBase + 1, ATTRIB_SCRUBBING, ATTRIB_SCRUBBING);

        //
        // This depends on RM initialization order: RM will only call pmaScrubComplete
        // once after all regions are registered and finished scrubbing.
        // The return value cannot be asserted. For example, when we are registering
        // the second region, the old returned value is _IN_PROGRESS and that is expected.
        //
        portAtomicCompareAndSwapSize(&pPma->initScrubbing, PMA_SCRUB_IN_PROGRESS,
                                                           PMA_SCRUB_INITIALIZE);
    }

    status = pmaRegisterBlacklistInfo(pPma, physBase, pBlacklistPageBase, blacklistCount, NV_TRUE);
    if (status != NV_OK)
    {
        pPma->pMapInfo->pmaMapDestroy(pMap);
        portMemFree(pPma->pRegDescriptors[id]);
        return status;
    }

    pPma->pStatsUpdateCb(pPma->pStatsUpdateCtx, pPma->pmaStats.numFreeFrames);

    NV_PRINTF(LEVEL_INFO, "Registered region:\n");
    pmaRegionPrint(pPma, pPma->pRegDescriptors[id], pPma->pRegions[id]);
    NV_PRINTF(LEVEL_INFO, "%d region(s) now registered\n", pPma->regSize);

    return status;
}

NV_STATUS
pmaAllocatePages
(
    PMA                    *pPma,
    NvLength                allocationCount,
    NvU64                   pageSize,
    PMA_ALLOCATION_OPTIONS *allocationOptions,
    NvU64                  *pPages
)
{
    NvS32 regionList[PMA_REGION_SIZE];
    NV_STATUS status, prediction;
    NvU32 flags;
    NvBool evictFlag, contigFlag, persistFlag, alignFlag, pinFlag, rangeFlag, blacklistOffFlag, partialFlag, skipScrubFlag, reverseFlag;

    NvU32 regId, regionIdx;
    NvU64 numPagesAllocatedThisTime, numPagesLeftToAllocate, numPagesAllocatedSoFar;
    NvU64 addrBase, addrLimit;
    NvU64 rangeStart, rangeEnd;
    NvU64 stride = 0;
    NvU32 strideStart = 0;
    NvU64 *curPages;
    NvBool blacklistOffPerRegion[PMA_REGION_SIZE]={NV_FALSE};
    NvU64 blacklistOffAddrStart[PMA_REGION_SIZE] = {0}, blacklistOffRangeSize[PMA_REGION_SIZE] = {0};
    NvBool bScrubOnFree = NV_FALSE;

    void *pMap = NULL;
    scanFunc useFunc;
    PMA_PAGESTATUS pinOption;
    NvU64 alignment = pageSize;
    NvU32 framesPerPage;
    NvU64 numFramesToAllocateTotal;

    //
    // A boolean indicating if we should try to evict. We at most try eviction once per call
    // to pmaAllocatePages.
    //
    NvBool tryEvict = NV_TRUE;
    NvBool tryAlloc = NV_TRUE;

    if (pPma == NULL || pPages == NULL || allocationCount == 0
        || (pageSize != _PMA_64KB && pageSize != _PMA_128KB && pageSize != _PMA_2MB && pageSize != _PMA_512MB)
        || allocationOptions == NULL)
    {
        if (pPma == NULL)
            NV_PRINTF(LEVEL_ERROR, "NULL PMA object\n");
        if (pPages == NULL)
            NV_PRINTF(LEVEL_ERROR, "NULL page list pointer\n");
        if (allocationCount == 0)
            NV_PRINTF(LEVEL_ERROR, "count == 0\n");
        if (pageSize != _PMA_64KB && pageSize != _PMA_128KB && pageSize != _PMA_2MB && pageSize != _PMA_512MB)
            NV_PRINTF(LEVEL_ERROR, "pageSize=0x%llx (not 64K, 128K, 2M, or 512M)\n", pageSize);
        if (allocationOptions == NULL)
            NV_PRINTF(LEVEL_ERROR, "NULL allocationOptions\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    flags = allocationOptions->flags;
    evictFlag   = !(flags & PMA_ALLOCATE_DONT_EVICT);
    contigFlag  = !!(flags & PMA_ALLOCATE_CONTIGUOUS);
    pinFlag     = !!(flags & PMA_ALLOCATE_PINNED);
    rangeFlag   = !!(flags & PMA_ALLOCATE_SPECIFY_ADDRESS_RANGE);
    persistFlag = pPma->bForcePersistence || !!(flags & PMA_ALLOCATE_PERSISTENT);
    alignFlag   = !!(flags & PMA_ALLOCATE_FORCE_ALIGNMENT);
    blacklistOffFlag = !!(flags & PMA_ALLOCATE_TURN_BLACKLIST_OFF);
    partialFlag = !!(flags & PMA_ALLOCATE_ALLOW_PARTIAL);
    skipScrubFlag = !!(flags & PMA_ALLOCATE_NO_ZERO);
    reverseFlag = !!(flags & PMA_ALLOCATE_REVERSE_ALLOC);

    // Fork out new code path for NUMA sub-allocation from OS
    if (pPma->bNuma)
    {
        if (reverseFlag)
        {
            NV_PRINTF(LEVEL_ERROR, "Reverse allocation not supported on NUMA.\n");
            return NV_ERR_INVALID_ARGUMENT;
        }

        return pmaNumaAllocate(pPma, allocationCount, pageSize, allocationOptions, pPages);
    }

    //
    // Scrub on free is enabled for this allocation request if the feature is enabled and the
    // caller does not want to skip scrubber.
    // Caller may want to skip scrubber when it knows the memory is zero'ed or when we are
    // initializing RM structures needed by the scrubber itself.
    //
    bScrubOnFree = pPma->bScrubOnFree && (!skipScrubFlag);

    //
    // PMA only knows the page is zero'ed if PMA scrubbed it.
    // For example, if something else scrubbed the page, called PMA with ALLOCATE_NO_ZERO,
    // the _RESULT_IS_ZERO flag is not set because PMA did not scrub that page.
    //
    allocationOptions->resultFlags = bScrubOnFree ? PMA_ALLOCATE_RESULT_IS_ZERO : 0;

    if (blacklistOffFlag && !contigFlag)
    {
        NV_PRINTF(LEVEL_ERROR, "Blacklist can only be turned off for contiguous allocations\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (bScrubOnFree && blacklistOffFlag)
    {
        NV_PRINTF(LEVEL_ERROR, "Blacklist cannot be turned off when scrub on free is enabled\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (rangeFlag && (!NV_IS_ALIGNED(allocationOptions->physBegin, pageSize)
          || !NV_IS_ALIGNED((allocationOptions->physEnd + 1), pageSize)))
    {
        NV_PRINTF(LEVEL_WARNING,
                "base [0x%llx] or limit [0x%llx] not aligned to page size 0x%llx\n",
                allocationOptions->physBegin,
                allocationOptions->physEnd + 1,
                pageSize);
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // Minimum alignment is requested page size. Alignment granularity is 64K.
    // Alignment must be power of two for PMA math
    //
    if (alignFlag)
    {
        if (!NV_IS_ALIGNED(allocationOptions->alignment, _PMA_64KB) ||
            !portUtilIsPowerOfTwo(allocationOptions->alignment))
        {
            NV_PRINTF(LEVEL_WARNING,
                "alignment [%llx] is not aligned to 64KB or is not power of two.",
                alignment);
            return NV_ERR_INVALID_ARGUMENT;
        }

        alignment = NV_MAX(pageSize, allocationOptions->alignment);
        if (!contigFlag && alignment > pageSize)
        {
            NV_PRINTF(LEVEL_WARNING,
                "alignment [%llx] larger than the pageSize [%llx] not supported for non-contiguous allocs\n",
                alignment, pageSize);
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    framesPerPage  = (NvU32)(pageSize >> PMA_PAGE_SHIFT);
    numFramesToAllocateTotal = framesPerPage * allocationCount;

    pinOption = pinFlag ? STATE_PIN : STATE_UNPIN;
    pinOption |= persistFlag ? ATTRIB_PERSISTENT : 0;

    useFunc = contigFlag ? (pPma->pMapInfo->pmaMapScanContiguous) :
                           (pPma->pMapInfo->pmaMapScanDiscontiguous);

    // No locking required because the states don't change
    status = pmaSelector(pPma, allocationOptions, regionList);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_FATAL, "Region selector failed\n");
        return status;
    }

    if (bScrubOnFree)
    {
        portSyncMutexAcquire(pPma->pAllocLock);
        portSyncRwLockAcquireRead(pPma->pScrubberValidLock);
        if (pmaPortAtomicGet(&pPma->scrubberValid) != PMA_SCRUBBER_VALID)
        {
            NV_PRINTF(LEVEL_WARNING, "PMA object is not valid\n");
            portSyncRwLockReleaseRead(pPma->pScrubberValidLock);
            portSyncMutexRelease(pPma->pAllocLock);
            return NV_ERR_INVALID_STATE;
        }
    }

    tryEvict = evictFlag;

pmaAllocatePages_retry:
    //
    // Retry implies that the PMA lock has been released and will be re-acquired
    // after checking the scrubber so any pages allocated so far are not guaranteed
    // to be there any more. Restart from scratch.
    //
#if PMA_DEBUG
    NV_PRINTF(LEVEL_INFO, "Attempt %s allocation of 0x%llx pages of size 0x%llx "
                          "(0x%x frames per page)\n",
                          contigFlag ? "contiguous" : "discontiguous",
                          (NvU64)allocationCount, pageSize, framesPerPage);
#endif

    // Check if scrubbing is done before allocating each time before we retry
    if (bScrubOnFree)
    {
        if ((status = _pmaCheckScrubbedPages(pPma, 0, NULL, 0)) != NV_OK)
            goto scrub_fatal;
    }

    // Attempting to allocate starts here
    numPagesLeftToAllocate = allocationCount;
    numPagesAllocatedSoFar = 0;
    curPages = pPages;

    portSyncSpinlockAcquire(pPma->pPmaLock);

    NV_ASSERT(pmaStateCheck(pPma));

    prediction = _pmaPredictOutOfMemory(pPma, allocationCount, pageSize, allocationOptions);
    if (!tryEvict && (prediction == NV_ERR_NO_MEMORY))
    {
        NV_PRINTF(LEVEL_INFO, "Returning OOM from prediction path.\n");
        status = NV_ERR_NO_MEMORY;
        goto normal_exit;
    }

    for (regionIdx = 0; regionIdx < pPma->regSize; regionIdx++)
    {
        MEMORY_PROTECTION prot;

        if (regionList[regionIdx] == -1)
        {
            status = NV_ERR_NO_MEMORY;
            goto normal_exit;
        }
        NV_ASSERT(regionList[regionIdx] < PMA_REGION_SIZE);

        regId = (NvU32)regionList[regionIdx];
        pMap  = pPma->pRegions[regId];

        addrBase = pPma->pRegDescriptors[regId]->base;
        addrLimit = pPma->pRegDescriptors[regId]->limit;
        prot = pPma->pRegDescriptors[regId]->bProtected ? MEMORY_PROTECTION_PROTECTED :
                                                          MEMORY_PROTECTION_UNPROTECTED;

        //
        // If the start address of the range is less than the region's base
        // address, start from the base itself.
        //
        rangeStart = rangeFlag ? ((allocationOptions->physBegin >= addrBase) ?
                                  (allocationOptions->physBegin - addrBase) : 0) : 0;
        rangeEnd   = rangeFlag ? ((allocationOptions->physEnd >= addrBase) ?
                                  (allocationOptions->physEnd - addrBase) : 0) : 0;

        if (rangeStart > rangeEnd)
        {
            status = NV_ERR_INVALID_ARGUMENT;
            goto normal_exit;
        }

        //
        // Before continuing with allocation, lets check if we need to turn-off
        // blacklisting. During retry, we don't have to free the blacklisted pages again
        //
        if (blacklistOffFlag && !blacklistOffPerRegion[regId])
        {
            if (allocationOptions->physBegin > addrLimit)
            {
                blacklistOffAddrStart[regId] = 0;
                blacklistOffRangeSize[regId] = 0;
            }
            else
            {
                // if the range falls within the region then free blacklisted pages in the entire region
                blacklistOffAddrStart[regId] = addrBase;
                blacklistOffRangeSize[regId] = (addrLimit - addrBase + 1);
                _pmaFreeBlacklistPages(pPma, regId, blacklistOffAddrStart[regId], blacklistOffRangeSize[regId]);
                blacklistOffPerRegion[regId] = NV_TRUE;
            }
        }

        NV_ASSERT(numPagesLeftToAllocate + numPagesAllocatedSoFar == allocationCount);
        NV_ASSERT(numPagesLeftToAllocate > 0);

        numPagesAllocatedThisTime = 0;
        status = (*useFunc)(pMap, addrBase, rangeStart, rangeEnd, numPagesLeftToAllocate,
            curPages, pageSize, alignment, stride, strideStart, &numPagesAllocatedThisTime, !tryEvict, reverseFlag);

        NV_ASSERT(numPagesAllocatedThisTime <= numPagesLeftToAllocate);

        if (contigFlag)
        {
            // Contiguous allocations are all or nothing
            NV_ASSERT(numPagesAllocatedThisTime == 0 ||
                      numPagesAllocatedThisTime == numPagesLeftToAllocate);
        }

        //
        // Adjust the counts and the pointer within the array of pages for the
        // discontiguous case where only some pages might have been successfully
        // allocated.
        //
        numPagesAllocatedSoFar += numPagesAllocatedThisTime;
        curPages += numPagesAllocatedThisTime;
        numPagesLeftToAllocate -= numPagesAllocatedThisTime;

        if (status == NV_ERR_IN_USE && !tryEvict)
        {
            //
            // If memory is evictable, but eviction is not allowed by the
            // caller, just return the no memory error.
            //
            NV_PRINTF(LEVEL_WARNING, "Memory evictable, but eviction not allowed, returning\n");
            status = NV_ERR_NO_MEMORY;
        }

        if (status == NV_OK)
        {
            NV_ASSERT(numPagesLeftToAllocate == 0);
            NV_ASSERT(numPagesAllocatedSoFar == allocationCount);
            break;
        }
        else if (status == NV_ERR_NO_MEMORY)
        {
            //
            // Print an "out of memory" mssg only after we've scanned through
            // all the regions. Printing an OOM message on per region basis may
            // confuse someone debugging that we've actually run out of memory.
            //
            if ((regionIdx < (pPma->regSize - 1)) && (regionList[regionIdx + 1] == -1))
            {
                NV_PRINTF(LEVEL_ERROR, "Status no_memory\n");
            }
            if (contigFlag)
            {
                // Contiguous allocations are all or nothing.
                NV_ASSERT(numPagesAllocatedThisTime == 0);
            }
        }
        else if (tryEvict)
        {
            NV_PRINTF(LEVEL_INFO, "Status evictable, region before eviction:\n");
            pmaRegionPrint(pPma, pPma->pRegDescriptors[regId], pMap);

            NV_ASSERT(numPagesLeftToAllocate > 0);

            if (contigFlag)
            {
                NV_ASSERT(numPagesLeftToAllocate == allocationCount);
                NV_ASSERT(numPagesAllocatedThisTime == 0);
                NV_ASSERT(numPagesAllocatedSoFar == 0);

                NvU64 evictStart  = *curPages;
                NvU64 evictEnd    = *curPages + (numFramesToAllocateTotal << PMA_PAGE_SHIFT) - 1;

                NV_PRINTF(LEVEL_INFO, "Attempt %s eviction of 0x%llx pages of size 0x%llx, "
                                      "(0x%x frames per page) in the frame range 0x%llx..0x%llx\n",
                                      contigFlag ? "contiguous" : "discontiguous",
                                      numPagesLeftToAllocate,
                                      pageSize,
                                      framesPerPage,
                                      (evictStart - addrBase) >> PMA_PAGE_SHIFT,
                                      (evictEnd - addrBase) >> PMA_PAGE_SHIFT);

                status = _pmaEvictContiguous(pPma, pMap, evictStart, evictEnd, prot);
            }
            else
            {
                // Default to allowing the whole region to be evicted
                NvU64 evictPhysBegin = addrBase;
                NvU64 evictPhysEnd = addrLimit;

                if (rangeFlag)
                {
                    //
                    // And if a specific physical range was requested, intersect
                    // it with the region.
                    //
                    evictPhysBegin = NV_MAX(allocationOptions->physBegin, evictPhysBegin);
                    evictPhysEnd   = NV_MIN(allocationOptions->physEnd, evictPhysEnd);

                    // Regions that would cause the intersection to be empty are skipped.
                    NV_ASSERT(evictPhysBegin <= evictPhysEnd);
                }

                NV_PRINTF(LEVEL_INFO, "Attempt %s eviction of 0x%llx pages of size 0x%llx, "
                                      "(0x%x frames per page), in the frame range 0x%llx..0x%llx\n",
                                      contigFlag ? "contiguous" : "discontiguous",
                                      numPagesLeftToAllocate,
                                      pageSize,
                                      framesPerPage,
                                      (evictPhysBegin - addrBase) >> PMA_PAGE_SHIFT,
                                      (evictPhysEnd - addrBase) >> PMA_PAGE_SHIFT);

                status = _pmaEvictPages(pPma, pMap, curPages, numPagesLeftToAllocate,
                                        pPages, numPagesAllocatedSoFar, pageSize,
                                        evictPhysBegin, evictPhysEnd, prot);
            }

            if (status == NV_OK)
            {
                numPagesAllocatedSoFar = allocationCount;
            }
            else
            {
                NV_PRINTF(LEVEL_INFO, "Eviction/scrubbing failed, region after:\n");
                pmaRegionPrint(pPma, pPma->pRegDescriptors[regId], pMap);
            }

            if (status == NV_ERR_INSUFFICIENT_RESOURCES)
            {
                NV_PRINTF(LEVEL_ERROR, "ERROR: scrubber OOM\n");

                // Scrubber is out of memory
                goto scrub_fatal;
            }
        }
    }

    //
    // if scrubbing is active in the background, release lock and spin until it
    // completes, then re-try.
    //
    if ((status == NV_ERR_NO_MEMORY) &&
        (pmaPortAtomicGet(&pPma->initScrubbing) == PMA_SCRUB_IN_PROGRESS))
    {
        // Release the spinlock before attempting a semaphore acquire.
        portSyncSpinlockRelease(pPma->pPmaLock);

        // Wait until scrubbing is complete.
        while (pmaPortAtomicGet(&pPma->initScrubbing) != PMA_SCRUB_DONE)
        {
            // Deschedule without PMA lock
            pmaOsSchedule();
        }
        NV_PRINTF(LEVEL_INFO, "Retrying after eviction/scrub\n");
        goto pmaAllocatePages_retry;
    }

    if ((status == NV_ERR_NO_MEMORY) && partialFlag && (numPagesAllocatedSoFar > 0))
    {
        //
        // If scrub on free is enabled, continue to scrubWaitForAll if we haven't already,
        // otherwise succeed the partial allocation.
        // If scrub on free is not enabled, there is no waiting to try, so succeed the
        // partial allocation immediately.
        //
        if (!bScrubOnFree  || !tryAlloc)
        {
            NV_PRINTF(LEVEL_INFO, "Succeed partial allocation\n");
            status = NV_OK;
        }
    }

    if (status == NV_ERR_NO_MEMORY && bScrubOnFree)
    {
        PSCRUB_NODE pPmaScrubList = NULL;
        NvU64       count;
        portSyncSpinlockRelease(pPma->pPmaLock);

        NV_PRINTF(LEVEL_INFO, "Waiting for scrubber\n");

        status = scrubCheckAndWaitForSize(pPma->pScrubObj, numPagesLeftToAllocate,
                                          pageSize, &pPmaScrubList, &count);

        if (status == NV_OK)
        {
            if (count > 0)
            {
                _pmaClearScrubBit(pPma, pPmaScrubList, count);
            }

            //
            // Free the actual list, although allocated by objscrub
            // there is no need for failure case handling to free the list,  because the call
            // returns error for 1)memory allocation failure or 2)nothing remaining to scrub.
            //
            portMemFree(pPmaScrubList);
        }

        //
        // Set tryEvict to NV_FALSE because we know UVM already failed eviction and any
        // available memory that comes after we tried eviction will not be counted towards
        // this allocation.
        //
        if (tryAlloc)
        {
            tryAlloc = NV_FALSE;
            tryEvict = NV_FALSE;
            NV_PRINTF(LEVEL_INFO, "Retrying after waiting for scrubber\n");
            goto pmaAllocatePages_retry;
        }

        if (blacklistOffFlag)
        {
            for (regionIdx = 0; regionIdx < pPma->regSize; regionIdx++)
            {
                if (blacklistOffPerRegion[regionIdx] == NV_FALSE)
                    continue;
                _pmaReallocBlacklistPages(pPma, regionIdx, blacklistOffAddrStart[regionIdx], blacklistOffRangeSize[regionIdx]);
            }
        }
        if (bScrubOnFree)
        {
            portSyncRwLockReleaseRead(pPma->pScrubberValidLock);
            portSyncMutexRelease(pPma->pAllocLock);
        }
        NV_PRINTF(LEVEL_INFO, "Returning OOM after waiting for scrubber\n");
        return NV_ERR_NO_MEMORY;
    }

    if (status == NV_OK)
    {
        NvU32 i;

        //
        // Here we need to double check if the scrubber was valid because the contiguous eviction
        // which called pmaFreePages could have had a fatal failure that resulted in some
        // pages being unscrubbed.
        //
        if (bScrubOnFree && (pmaPortAtomicGet(&pPma->scrubberValid) != PMA_SCRUBBER_VALID))
        {
            portSyncSpinlockRelease(pPma->pPmaLock);
            NV_PRINTF(LEVEL_FATAL, "Failing allocation because the scrubber is not valid.\n");
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto scrub_fatal;
        }

        // Commit
        allocationOptions->numPagesAllocated = (NvLength)numPagesAllocatedSoFar;

        if (contigFlag)
        {
            NvU64 frameBase;
            const NvU64 numFramesAllocated = framesPerPage * numPagesAllocatedSoFar;

            regId = findRegionID(pPma, pPages[0]);
            pMap  = pPma->pRegions[regId];
            addrBase = pPma->pRegDescriptors[regId]->base;
            frameBase = PMA_ADDR2FRAME(pPages[0], addrBase);

#if PMA_DEBUG
            NV_PRINTF(LEVEL_INFO, "Successfully allocated frames 0x%llx through 0x%llx\n",
                                  frameBase,
                                  frameBase + numFramesAllocated - 1);
#endif

            pPma->pMapInfo->pmaMapChangeBlockStateAttrib(pMap, frameBase, numPagesAllocatedSoFar * framesPerPage,
                                                         pinOption, MAP_MASK);
            
            pPma->pStatsUpdateCb(pPma->pStatsUpdateCtx, pPma->pmaStats.numFreeFrames);

            if (blacklistOffFlag && blacklistOffPerRegion[regId])
            {
                NvU64 allocatedRegionEnd = PMA_FRAME2ADDR(frameBase + numFramesAllocated - 1, addrBase) + PMA_GRANULARITY - 1;
                NvU64 blacklistOffAddrEnd = blacklistOffAddrStart[regId] + blacklistOffRangeSize[regId] - 1;
                blacklistOffPerRegion[regId] = NV_FALSE;
                _pmaReallocBlacklistPages(pPma, regId, blacklistOffAddrStart[regId],  (pPages[0] - blacklistOffAddrStart[regId] + 1));
                if (allocatedRegionEnd < blacklistOffAddrEnd)
                    _pmaReallocBlacklistPages(pPma, regId, allocatedRegionEnd, (blacklistOffAddrEnd - allocatedRegionEnd));
            }
        }
        else
        {
            NvU64 frameBase = 0;
#if PMA_DEBUG
            NvU64 frameRangeStart   = 0;
            NvU64 nextExpectedFrame = 0;
            NvU32 frameRangeRegId   = 0;

            (void)frameRangeStart;   //Silence the compiler
            (void)nextExpectedFrame;
            (void)frameRangeRegId;

            NV_PRINTF(LEVEL_INFO, "Successfully allocated frames:\n");
#endif

            for (i = 0; i < numPagesAllocatedSoFar; i++)
            {
                regId = findRegionID(pPma, pPages[i]);
                pMap  = pPma->pRegions[regId];
                addrBase = pPma->pRegDescriptors[regId]->base;
                frameBase =  PMA_ADDR2FRAME(pPages[i], addrBase);

#if PMA_DEBUG
                // Print out contiguous frames in the same NV_PRINTF
                if (i == 0)
                {
                    frameRangeStart = frameBase;
                    frameRangeRegId = regId;
                }
                else if ((frameRangeRegId != regId) || (nextExpectedFrame != frameBase))
                {
                    // Break in frame range detected
                    NV_PRINTF(LEVEL_INFO, "0x%llx through 0x%llx region %d \n",
                                          reverseFlag ? nextExpectedFrame + framesPerPage : frameRangeStart,
                                          reverseFlag ? frameRangeStart + framesPerPage - 1 : nextExpectedFrame - 1,
                                          frameRangeRegId);

                    frameRangeStart = frameBase;
                    frameRangeRegId = regId;
                }
                nextExpectedFrame = reverseFlag ? frameBase - framesPerPage : frameBase + framesPerPage;
#endif

                pPma->pMapInfo->pmaMapChangePageStateAttrib(pMap, frameBase,
                                                            pageSize, pinOption, MAP_MASK);

            }

            pPma->pStatsUpdateCb(pPma->pStatsUpdateCtx, pPma->pmaStats.numFreeFrames);

#if PMA_DEBUG
                // Break in frame range detected
                NV_PRINTF(LEVEL_INFO, "0x%llx through 0x%llx region %d \n",
                                      reverseFlag ? nextExpectedFrame + framesPerPage : frameRangeStart,
                                      reverseFlag ? frameRangeStart + framesPerPage - 1 : nextExpectedFrame - 1,
                                      frameRangeRegId);
#endif
        }
    }

normal_exit:
    if (blacklistOffFlag)
    {
        for (regionIdx = 0; regionIdx < pPma->regSize; regionIdx++)
        {
            if (blacklistOffPerRegion[regionIdx] == NV_FALSE)
                continue;
            _pmaReallocBlacklistPages(pPma, regionIdx, blacklistOffAddrStart[regionIdx], blacklistOffRangeSize[regionIdx]);
        }
    }

    portSyncSpinlockRelease(pPma->pPmaLock);
    if (bScrubOnFree)
    {
        portSyncRwLockReleaseRead(pPma->pScrubberValidLock);
        portSyncMutexRelease(pPma->pAllocLock);
    }
    return status;

scrub_fatal:
    if (blacklistOffFlag)
    {
        for (regionIdx = 0; regionIdx < pPma->regSize; regionIdx++)
        {
            if (blacklistOffPerRegion[regionIdx] == NV_FALSE)
                continue;
            _pmaReallocBlacklistPages(pPma, regionIdx, blacklistOffAddrStart[regionIdx], blacklistOffRangeSize[regionIdx]);
        }
    }
    // Note we do not have the PMA lock.
    portAtomicSetSize(&pPma->scrubberValid, PMA_SCRUBBER_INVALID);
    portSyncRwLockReleaseRead(pPma->pScrubberValidLock);
    portSyncMutexRelease(pPma->pAllocLock);
    return status;

}

NV_STATUS
pmaAllocatePagesBroadcast
(
    PMA                   **pPma,
    NvU32                   pmaCount,
    NvLength                allocationCount,
    NvU64                   pageSize,
    PMA_ALLOCATION_OPTIONS *allocationOptions,
    NvU64                  *pPages
)
{

    if (pPma == NULL || pmaCount == 0 || allocationCount == 0
        || (pageSize != _PMA_64KB && pageSize != _PMA_128KB && pageSize != _PMA_2MB && pageSize != _PMA_512MB)
        || pPages == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_ERR_GENERIC;
}

NV_STATUS
pmaPinPages
(
    PMA      *pPma,
    NvU64    *pPages,
    NvLength  pageCount,
    NvU64     pageSize
)
{
    NV_STATUS status = NV_OK;
    NvU32          framesPerPage, regId, i, j;
    NvU64          frameNum, addrBase;
    PMA_PAGESTATUS state;
    framesPerPage  = (NvU32)(pageSize >> PMA_PAGE_SHIFT);

    if (pPma == NULL || pageCount == 0 || pPages == NULL
        || (pageSize != _PMA_64KB && pageSize != _PMA_128KB && pageSize != _PMA_2MB && pageSize != _PMA_512MB))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    portSyncSpinlockAcquire(pPma->pPmaLock);

    for(i = 0; i < pageCount; i++)
    {
        regId = findRegionID(pPma, pPages[i]);
        addrBase = pPma->pRegDescriptors[regId]->base;
        frameNum = PMA_ADDR2FRAME(pPages[i], addrBase);

        for (j = 0; j < framesPerPage; j++)
        {
            state = pPma->pMapInfo->pmaMapRead(pPma->pRegions[regId], (frameNum + j), NV_TRUE);

            //
            // Check for incorrect usage of the API where the caller requests to
            // pin pages that are not allocated unpinned.
            //
            if ((state & STATE_MASK) != STATE_UNPIN)
                status = NV_ERR_INVALID_STATE;

            //
            // Check for pages being evicted. Notably this is expected if the
            // call races with eviction.
            //
            if (state & ATTRIB_EVICTING)
                status = NV_ERR_IN_USE;

            if (status != NV_OK)
            {
                //
                // Don't print the error for the eviction case as that's
                // expected to happen.
                //
                if (status != NV_ERR_IN_USE)
                {
                    NV_PRINTF(LEVEL_ERROR,
                        "Pin failed at page %d frame %d in region %d state %d\n",
                        i, j, regId, state);
                }
                _pmaRollback(pPma, pPages, i, j, pageSize, STATE_UNPIN);
                goto done;
            }
            else
            {
                pPma->pMapInfo->pmaMapChangeStateAttrib(pPma->pRegions[regId], (frameNum + j), STATE_PIN, STATE_MASK);
            }
        }
    }

done:
    portSyncSpinlockRelease(pPma->pPmaLock);

    return status;
}

void
pmaFreePages
(
    PMA   *pPma,
    NvU64 *pPages,
    NvU64  pageCount,
    NvU64  size,
    NvU32  flag
)
{
    // TODO Support free of multiple regions in one call??
    NvU64 i, j, frameNum, framesPerPage, addrBase;
    NvU32 regId;
    NvBool bScrubValid = NV_TRUE;
    NvBool bNeedScrub = pPma->bScrubOnFree && !(flag & PMA_FREE_SKIP_SCRUB);

    NV_ASSERT(pPma != NULL);
    NV_ASSERT(pageCount != 0);
    NV_ASSERT(pPages != NULL);

    if (pageCount != 1)
    {
        NV_ASSERT((size == _PMA_64KB)  ||
                  (size == _PMA_128KB) ||
                  (size == _PMA_2MB)   ||
                  (size == _PMA_512MB));
    }

    // Fork out new code path for NUMA sub-allocation from OS
    if (pPma->bNuma)
    {
        portSyncSpinlockAcquire(pPma->pPmaLock);
        pmaNumaFreeInternal(pPma, pPages, pageCount, size, flag);
        portSyncSpinlockRelease(pPma->pPmaLock);

        return;
    }

    // Check if any scrubbing is done before we actually free
    if (bNeedScrub)
    {
        portSyncRwLockAcquireRead(pPma->pScrubberValidLock);
        if (pmaPortAtomicGet(&pPma->scrubberValid) == PMA_SCRUBBER_VALID)
        {
            if (_pmaCheckScrubbedPages(pPma, 0, NULL, 0) != NV_OK)
            {
                portAtomicSetSize(&pPma->scrubberValid, PMA_SCRUBBER_INVALID);
                portSyncRwLockReleaseRead(pPma->pScrubberValidLock);
                bScrubValid = NV_FALSE;
                NV_PRINTF(LEVEL_WARNING, "Scrubber object is not valid\n");
            }
        }
        else
        {
            // We allow free with invalid scrubber object
            portSyncRwLockReleaseRead(pPma->pScrubberValidLock);
            bScrubValid = NV_FALSE;
            NV_PRINTF(LEVEL_WARNING, "Scrubber object is not valid\n");
        }
    }
    // Only hold Reader lock here if (bScrubValid && bNeedScrub)

    portSyncSpinlockAcquire(pPma->pPmaLock);

    framesPerPage = size >> PMA_PAGE_SHIFT;

    for (i = 0; i < pageCount; i++)
    {
        regId    = findRegionID(pPma, pPages[i]);
        addrBase = pPma->pRegDescriptors[regId]->base;
        frameNum = PMA_ADDR2FRAME(pPages[i], addrBase);

        _pmaReallocBlacklistPages(pPma, regId, pPages[i], pageCount * size);

        for (j = 0; j < framesPerPage; j++)
        {
            PMA_PAGESTATUS newStatus = (bScrubValid && bNeedScrub) ? ATTRIB_SCRUBBING : STATE_FREE;
            PMA_PAGESTATUS exceptedMask = ATTRIB_EVICTING | ATTRIB_BLACKLIST;

            //
            // Reset everything except for the (ATTRIB_EVICTING and ATTRIB_BLACKLIST) state to support memory being freed
            // after being picked for eviction.
            //
            pPma->pMapInfo->pmaMapChangeStateAttrib(pPma->pRegions[regId], (frameNum + j), newStatus, ~(exceptedMask));
        }
    }

    pPma->pStatsUpdateCb(pPma->pStatsUpdateCtx, pPma->pmaStats.numFreeFrames);

    portSyncSpinlockRelease(pPma->pPmaLock);

    // Maybe we need to scrub the page on free
    if (bScrubValid && bNeedScrub)
    {
        PSCRUB_NODE pPmaScrubList = NULL;
        NvU64 count;
        if (scrubSubmitPages(pPma->pScrubObj, size, pPages, pageCount,
                             &pPmaScrubList, &count) == NV_OK)
        {
            if (count > 0)
            {
                _pmaClearScrubBit(pPma, pPmaScrubList, count);
            }
        }
        else
        {
            portAtomicSetSize(&pPma->scrubberValid, PMA_SCRUBBER_INVALID);
        }

        // Free the actual list, although allocated by objscrub
        portMemFree(pPmaScrubList);

        portSyncRwLockReleaseRead(pPma->pScrubberValidLock);
    }
}


void
pmaClearScrubRange
(
    PMA *pPma,
    NvU64 rangeBase,
    NvU64 rangeLimit
)
{
    NvU32 regionIdx;
    NvU64 physBase, physLimit;

    for (regionIdx = 0; regionIdx < pPma->regSize; regionIdx++)
    {
        physBase = pPma->pRegDescriptors[regionIdx]->base;
        physLimit = pPma->pRegDescriptors[regionIdx]->limit;

        if ((physBase >= rangeBase) && (physLimit <= rangeLimit))
        {
            pmaSetBlockStateAttrib(pPma, physBase, physLimit - physBase + 1, 0, ATTRIB_SCRUBBING);
        }
    }

    pPma->pStatsUpdateCb(pPma->pStatsUpdateCtx, pPma->pmaStats.numFreeFrames);
}


NV_STATUS
pmaScrubComplete
(
    PMA *pPma
)
{
    NvU32 regionIdx;
    NvU64 physBase, physLimit;


    if (pPma == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pmaPortAtomicGet(&pPma->initScrubbing) != PMA_SCRUB_IN_PROGRESS)
    {
        return NV_ERR_GENERIC;
    }

    // Clear the scrubbing bit for all regions
    for (regionIdx = 0; regionIdx < pPma->regSize; regionIdx++)
    {
        physBase = pPma->pRegDescriptors[regionIdx]->base;
        physLimit = pPma->pRegDescriptors[regionIdx]->limit;

        pmaSetBlockStateAttrib(pPma, physBase, physLimit - physBase + 1, 0, ATTRIB_SCRUBBING);
    }

    pPma->pStatsUpdateCb(pPma->pStatsUpdateCtx, pPma->pmaStats.numFreeFrames);

    NV_PRINTF(LEVEL_INFO, "Inside\n");
    for (regionIdx = 0; regionIdx < pPma->regSize; regionIdx++)
    {
        pmaRegionPrint(pPma, pPma->pRegDescriptors[regionIdx], pPma->pRegions[regionIdx]);
    }

    portAtomicSetSize(&pPma->initScrubbing, PMA_SCRUB_DONE);


    return NV_OK;
}

void
pmaRegisterUpdateStatsCb
(
    PMA *pPma,
    pmaUpdateStatsCb_t pUpdateCb,
    void *pCtxPtr
)
{
    // Only supported right after init, so we don't bother taking locks.
    pPma->pStatsUpdateCb = pUpdateCb;
    pPma->pStatsUpdateCtx = pCtxPtr;
    pUpdateCb(pCtxPtr, pPma->pmaStats.numFreeFrames);
}

NV_STATUS
pmaRegisterEvictionCb
(
    PMA              *pPma,
    pmaEvictPagesCb_t pEvictPagesCb,
    pmaEvictRangeCb_t pEvictRangeCb,
    void             *ctxPtr
)
{
    NV_STATUS status = NV_OK;

    if (pPma == NULL || pEvictPagesCb == NULL || pEvictRangeCb == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    //
    // Lock the eviction callback mutex to guarantee that all the previously
    // registered callbacks have been flushed before registering new ones.
    //
    portSyncMutexAcquire(pPma->pEvictionCallbacksLock);

    //
    // Take the spin lock to make setting the callbacks atomic with allocations
    // using the callbacks.
    //
    portSyncSpinlockAcquire(pPma->pPmaLock);

    //
    // Both callbacks are always set together to a non-NULL value so just check
    // one of them to make sure they are unset.
    //
    if (pPma->evictPagesCb == NULL)
    {
        pPma->evictPagesCb = pEvictPagesCb;
        pPma->evictRangeCb = pEvictRangeCb;
        pPma->evictCtxPtr  = ctxPtr;
    }
    else
    {
        status = NV_ERR_INVALID_STATE;
    }

    portSyncSpinlockRelease(pPma->pPmaLock);

    portSyncMutexRelease(pPma->pEvictionCallbacksLock);

    return status;
}


void
pmaUnregisterEvictionCb
(
    PMA *pPma
)
{
    NvBool evictionPending;

    if (pPma == NULL)
        return;

    //
    // Lock the eviction callbacks mutex to prevent new callbacks from being
    // registered while the old ones are being unregistered and flushed.
    //
    portSyncMutexAcquire(pPma->pEvictionCallbacksLock);

    //
    // Take the spin lock to make removing the callbacks atomic with allocations
    // using the callbacks.
    //
    portSyncSpinlockAcquire(pPma->pPmaLock);

    // TODO: Assert that no unpinned allocations are left.

    pPma->evictPagesCb = NULL;
    pPma->evictRangeCb = NULL;
    pPma->evictCtxPtr  = NULL;

    evictionPending = pmaIsEvictionPending(pPma);

    portSyncSpinlockRelease(pPma->pPmaLock);

    //
    // Even though no unpinned allocations should be present, there still could
    // be pending eviction callbacks that picked some unpinned pages for
    // eviction before they were freed. Wait for all of them to finish.
    //
    while (evictionPending)
    {
        // TODO: Consider adding a better wait mechanism.
        pmaOsSchedule();

        portSyncSpinlockAcquire(pPma->pPmaLock);

        evictionPending = pmaIsEvictionPending(pPma);

        portSyncSpinlockRelease(pPma->pPmaLock);
    }

    portSyncMutexRelease(pPma->pEvictionCallbacksLock);
}

void
pmaGetFreeMemory
(
    PMA             *pPma,
    NvU64           *pBytesFree
)
{
#if !defined(SRT_BUILD)
    NvU64 val;

    portSyncSpinlockAcquire(pPma->pPmaLock);
    NvBool nodeOnlined = pPma->nodeOnlined;
    portSyncSpinlockRelease(pPma->pPmaLock);

    if (nodeOnlined)
    {
        osGetNumaMemoryUsage(pPma->numaNodeId, pBytesFree, &val);
        return;
    }
    //
    // what to return when bNUMA == NV_TRUE and nodeOnlined==NV_FALSE?
    // TODO : BUG 4199482.
    //
#endif

    portSyncSpinlockAcquire(pPma->pPmaLock);

    *pBytesFree = pPma->pmaStats.numFreeFrames << PMA_PAGE_SHIFT;

    portSyncSpinlockRelease(pPma->pPmaLock);
}

void
pmaGetTotalMemory
(
    PMA             *pPma,
    NvU64           *pBytesTotal
)
{
    void *pMap;
    NvU64 totalBytesInRegion;
    NvU32 i;

    *pBytesTotal = 0;

#if !defined(SRT_BUILD)
    NvU64 val;

    portSyncSpinlockAcquire(pPma->pPmaLock);
    NvBool nodeOnlined = pPma->nodeOnlined;
    portSyncSpinlockRelease(pPma->pPmaLock);

    if (nodeOnlined)
    {
        osGetNumaMemoryUsage(pPma->numaNodeId, &val, pBytesTotal);
        return;
    }
    //
    // what to return when bNUMA == NV_TRUE and nodeOnlined==NV_FALSE?
    // TODO : BUG 4199482.
    //
#endif

    for (i = 0; i < pPma->regSize; i++)
    {
        pMap = pPma->pRegions[i];
        pPma->pMapInfo->pmaMapGetSize(pMap, &totalBytesInRegion);

        *pBytesTotal += totalBytesInRegion;
    }
}

NV_STATUS
pmaGetRegionInfo
(
    PMA                     *pPma,
    NvU32                   *pRegSize,
    PMA_REGION_DESCRIPTOR  **ppRegionDesc
)
{
    if (pPma == NULL || pRegSize == NULL || ppRegionDesc == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    *pRegSize = pPma->regSize;
    *ppRegionDesc = pPma->pRegDescriptors[0];
    return NV_OK;
}

void
pmaGetLargestFree
(
    PMA             *pPma,
    NvU64           *pLargestFree,
    NvU64           *pRegionBase,
    NvU64           *pLargestOffset
)
{
    void *pMap;
    NvU64 largestFreeInRegion;
    NvU32 i;

    *pLargestFree = 0;
    *pRegionBase = 0;

    //
    // FIXME: This field is still not being used by any RM client.
    // Set it to "bad" value for the present time. This should ideally
    // contain the offset of the largest free chunk.
    //
    *pLargestOffset = ~0ULL;

    portSyncSpinlockAcquire(pPma->pPmaLock);

    for (i = 0; i < pPma->regSize; i++)
    {
        pMap = pPma->pRegions[i];
        pPma->pMapInfo->pmaMapGetLargestFree(pMap, &largestFreeInRegion);

        if (*pLargestFree < largestFreeInRegion)
        {
            *pLargestFree = largestFreeInRegion;
            *pRegionBase = pPma->pRegDescriptors[i]->base;
        }
    }

    portSyncSpinlockRelease(pPma->pPmaLock);

    NV_PRINTF(LEVEL_INFO, "Largest Free Bytes = 0x%llx, base = 0x%llx, largestOffset = 0x%llx.\n",
        *pLargestFree, *pRegionBase, *pLargestOffset);
}

/*!
 * @brief Returns a list of PMA allocated blocks which has ATTRIB_PERSISTENT
 *        attribute set. It will be used by FBSR module to save/restore
 *        clients PMA allocations during system suspend/resume.
 *
 * @param[in]     pPma              PMA pointer
 * @param[in/out] ppPersistList     Pointer to list of persistent segments
 *
 * @return
 *      NV_OK                   Success
 *      NV_ERR_NO_MEMORY        Failure to allocate list
 */
NV_STATUS
pmaBuildPersistentList
(
    PMA             *pPma,
    PRANGELISTTYPE  *ppPersistList
)
{
    return pmaBuildList(pPma, ppPersistList, ATTRIB_PERSISTENT);
}

/*!
 * @brief Returns a list of all PMA allocated blocks. For all the PMA
 *        allocated blocks, either STATE_PIN or STATE_UNPIN attribute will
 *        be set. It will be used by FBSR module to save/restore clients
 *        PMA allocations for Unix GC-OFF based power management.
 *
 * @param[in]     pPma      PMA pointer
 * @param[in/out] ppList    Pointer to list of all the PMA allocated blocks.
 *
 * @return
 *      NV_OK                   Success
 *      NV_ERR_NO_MEMORY        Failure to allocate list
 */
NV_STATUS
pmaBuildAllocatedBlocksList
(
    PMA             *pPma,
    PRANGELISTTYPE  *ppList
)
{
    return pmaBuildList(pPma, ppList, STATE_PIN | STATE_UNPIN);
}

/*!
 * @brief Frees previously generated list by function pmaBuildPersistentList().
 *
 * @param[in]       pPma            PMA pointer
 * @param[in/out]   ppPersistList   Pointer to list of persistent segments
 *
 * @return
 *      void
 */
void
pmaFreePersistentList
(
    PMA             *pPma,
    PRANGELISTTYPE  *ppPersistList
)
{
    pmaFreeList(pPma, ppPersistList);
}

/*!
 * @brief Frees previously generated list by function
 *        pmaBuildAllocatedBlocksList().
 *
 * @param[in]     pPma      PMA pointer
 * @param[in/out] ppList    Pointer to list of all the PMA allocated blocks.
 *
 * @return
 *      void
 */
void
pmaFreeAllocatedBlocksList
(
    PMA             *pPma,
    PRANGELISTTYPE  *ppList
)
{
    pmaFreeList(pPma, ppList);
}

/*!
 * @brief Returns client managed blacklisted pages in the PMA region
 *
 * @param[in]  pPma             PMA pointer
 * @param[in]  pChunks          pointer to blacklist addresses in the PMA region
 * @param[in]  pPageSize        pointer to Size of each blacklist page addresses
 * @param[in]  pValidEntries    pointer to valid client managed blacklist pages
 *
 * @return
 *     void
 */
void
pmaGetClientBlacklistedPages
(
    PMA   *pPma,
    NvU64 *pChunks,
    NvU64 *pPageSize,
    NvU32 *pNumChunks
)
{
    NvU32  region = 0;
    NvU32  validEntries = 0;
    NvU32  chunk  = 0;

    NvU32 blacklistCount = 0;
    NvBool bClientManagedBlacklist = NV_FALSE;
    PMA_BLACKLIST_CHUNK *pBlacklistChunks, *pBlacklistChunk;

    for (region = 0; region < pPma->regSize; region++)
    {
        pmaQueryBlacklistInfo(pPma, &blacklistCount,
                              &bClientManagedBlacklist, &pBlacklistChunks);
        if (blacklistCount && bClientManagedBlacklist)
        {
            for (chunk = 0; chunk < blacklistCount; chunk++)
            {
                pBlacklistChunk = &pBlacklistChunks[chunk];
                if (!pBlacklistChunk->bIsValid)
                {
                    pChunks[validEntries++] = pBlacklistChunk->physOffset;
                }
            }
        }
    }

    if (validEntries == 0)
        pChunks = NULL;

    *pPageSize  = _PMA_64KB;
    *pNumChunks = validEntries;
}

/*!
 * @brief Returns the total blacklist size in bytes for
 *        both statically and dynamically blacklisted pages.
 *        pDynamicBlacklistSize and pStaticBlacklistSize are only copied-out if non-NULL.
 *
 * @param[in]  pPma                     PMA pointer
 * @param[in]  pDynamicBlacklistSize    pointer to dynamic blacklist size (bytes)
 * @param[in]  pStaticBlacklistSize     pointer to static blacklist size  (bytes)
 *
 * @return
 *     void
 */
void
pmaGetBlacklistSize
(
    PMA   *pPma,
    NvU32 *pDynamicBlacklistSize,
    NvU32 *pStaticBlacklistSize
)
{
    NvU32 dynamicBlacklistCount = 0;
    NvU32 staticBlacklistCount = 0;
    NvU32 blacklistCount = 0;
    NvU32 region, size;

    PMA_BLACKLIST_CHUNK *pBlacklistChunks, *pChunk;

    for (region = 0; region < pPma->regSize; region++)
    {
        pmaQueryBlacklistInfo(pPma, &blacklistCount,
                              NULL, &pBlacklistChunks);
        for (size = 0; size < blacklistCount; size++)
        {
            pChunk = &pBlacklistChunks[size];

            if (pChunk->bIsDynamic)
                dynamicBlacklistCount++;
            else
                staticBlacklistCount++;
        }
    }

    if (pDynamicBlacklistSize != NULL)
        *pDynamicBlacklistSize = dynamicBlacklistCount << PMA_PAGE_SHIFT;

    if (pStaticBlacklistSize != NULL)
        *pStaticBlacklistSize = staticBlacklistCount << PMA_PAGE_SHIFT;
}

void
pmaClearScrubbedPages
(
    PMA *pPma,
    PSCRUB_NODE pPmaScrubList,
    NvU64 count
)
{
    _pmaClearScrubBit(pPma, pPmaScrubList, count);
}

void pmaPrintMapState
(
    PMA *pPma
)
{
    NvU32 regionIdx;
    for (regionIdx = 0; regionIdx < pPma->regSize; regionIdx++)
    {
        pmaRegionPrint(pPma, pPma->pRegDescriptors[regionIdx], pPma->pRegions[regionIdx]);
    }
}

NV_STATUS
pmaAddToBlacklistTracking
(
    PMA   *pPma,
    NvU64  physAddr
)
{
    PMA_BLACKLIST_ADDRESS blacklistPages = {0};
    NV_STATUS status = NV_OK;
    if (pmaIsBlacklistingAddrUnique(pPma, physAddr))
    {
        blacklistPages.physOffset  = physAddr;
        blacklistPages.bIsDynamic  = NV_TRUE;
        status = pmaRegisterBlacklistInfo(pPma, 0, &blacklistPages, 1, NV_FALSE);
    }
    return status;
}

void
pmaGetTotalProtectedMemory
(
    PMA   *pPma,
    NvU64 *pBytesTotal
)
{
    void *pMap;
    NvU64 totalBytesInRegion;
    NvU32 i;

    *pBytesTotal = 0;

    for (i = 0; i < pPma->regSize; i++)
    {
        if (pPma->pRegDescriptors[i]->bProtected)
        {
            pMap = pPma->pRegions[i];
            pPma->pMapInfo->pmaMapGetSize(pMap, &totalBytesInRegion);
            *pBytesTotal += totalBytesInRegion;
        }
    }
}

void
pmaGetTotalUnprotectedMemory
(
    PMA   *pPma,
    NvU64 *pBytesTotal
)
{
    NvU64 totalBytesInProtectedRegion = 0;
    NvU64 totalBytesOverall = 0;

    *pBytesTotal = 0;

    pmaGetTotalMemory(pPma, &totalBytesOverall);
    pmaGetTotalProtectedMemory(pPma, &totalBytesInProtectedRegion);

    NV_ASSERT_OR_RETURN_VOID(totalBytesOverall >= totalBytesInProtectedRegion);

    *pBytesTotal = totalBytesOverall - totalBytesInProtectedRegion;
}

void
pmaGetFreeProtectedMemory
(
    PMA   *pPma,
    NvU64 *pBytesFree
)
{
    portSyncSpinlockAcquire(pPma->pPmaLock);

    *pBytesFree = (pPma->pmaStats.numFreeFramesProtected) << PMA_PAGE_SHIFT;

    portSyncSpinlockRelease(pPma->pPmaLock);
}

void
pmaGetFreeUnprotectedMemory
(
    PMA   *pPma,
    NvU64 *pBytesFree
)
{
    portSyncSpinlockAcquire(pPma->pPmaLock);

    *pBytesFree = (pPma->pmaStats.numFreeFrames -
                   pPma->pmaStats.numFreeFramesProtected) << PMA_PAGE_SHIFT;

    portSyncSpinlockRelease(pPma->pPmaLock);
}
