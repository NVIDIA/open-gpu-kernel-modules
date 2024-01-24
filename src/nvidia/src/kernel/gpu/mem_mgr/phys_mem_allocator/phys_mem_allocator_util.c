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

#include "gpu/mem_mgr/phys_mem_allocator/phys_mem_allocator_util.h"
#include "gpu/mem_mgr/phys_mem_allocator/phys_mem_allocator.h"
#include "gpu/mem_mgr/mem_scrub.h"
#include "utils/nvprintf.h"
#include "utils/nvassert.h"

#if !defined(SRT_BUILD)
// These files are not found on SRT builds
#include "os/os.h"
#else
static NvU64 osGetPageRefcount(NvU64 sysPagePhysAddr)
{
    return 0;
}

static NvU64 osCountTailPages(NvU64 sysPagePhysAddr)
{
    return 0;
}

static void osAllocReleasePage(NvU64 sysPagePhysAddr, NvU32 pageCount)
{
    return;
}

static NV_STATUS osOfflinePageAtAddress(NvU64 address)
{
    return NV_ERR_GENERIC;
}

static NvU8 osGetPageShift(void)
{
    return 0;
}

NV_STATUS scrubCheck(OBJMEMSCRUB *pScrubber, PSCRUB_NODE *ppList, NvU64 *size)
{
    return NV_ERR_GENERIC;
}

NV_STATUS scrubSubmitPages(OBJMEMSCRUB *pScrubber, NvU64 chunkSize, NvU64* pages,
                           NvU64 pageCount, PSCRUB_NODE *ppList, NvU64 *size)
{
    return NV_ERR_GENERIC;
}

NV_STATUS scrubWaitPages(OBJMEMSCRUB *pScrubber, NvU64 chunkSize, NvU64* pages, NvU32 pageCount)
{
    return NV_ERR_GENERIC;
}

NV_STATUS scrubCheckAndWaitForSize (OBJMEMSCRUB *pScrubber, NvU64 numPages,
                                    NvU64 pageSize, PSCRUB_NODE *ppList, NvU64 *pSize)
{
    return NV_ERR_GENERIC;
}
#endif

// Local helpers
NvU32
findRegionID(PMA *pPma, NvU64 address)
{
    NvU32 i;

    for (i = 0; i < pPma->regSize; i++)
    {
        NvU64 start, limit;
        start = pPma->pRegDescriptors[i]->base;
        limit = pPma->pRegDescriptors[i]->limit;
        if (address >= start && address <= limit)
        {
            return i;
        }
    }

    // Should never get here
    NV_ASSERT(0);
    return 0;
}


void
pmaPrintBlockStatus(PMA_PAGESTATUS blockStatus)
{
    // Use DBG_PRINTF so as not to prepend "NVRM:" everytime, as NV_PRINTF does
    if ((blockStatus & STATE_MASK) == STATE_FREE) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "STATE_FREE         ");
    }
    else if ((blockStatus & STATE_MASK) == STATE_UNPIN) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "STATE_UNPIN  ");
    }
    else if ((blockStatus & STATE_MASK) == STATE_PIN) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "STATE_PIN    ");
    }
    else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "UNKNOWN STATE");
    }

    if (blockStatus & ATTRIB_PERSISTENT) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, " | ATTRIB_PERSISTENT");
    }
    else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "             ");
    }

    if (blockStatus & ATTRIB_SCRUBBING) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, " | ATTRIB_SCRUBBING ");
    }
    else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "             ");
    }

    if (blockStatus & ATTRIB_EVICTING) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, " | ATTRIB_EVICTING  ");
    }
    else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "             ");
    }

    if (blockStatus & ATTRIB_BLACKLIST) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, " | ATTRIB_BLACKLIST ");
    }
    else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "             ");
    }

    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "\n");
}

void
pmaRegionPrint(PMA *pPma, PMA_REGION_DESCRIPTOR *pRegion, void *pMap)
{
    NvU32 i;
    PMA_PAGESTATUS currStatus, blockStatus = STATE_FREE;
    NvU64 addrBase, addrLimit, numFrames, blockStart = 0;

    NV_ASSERT(pRegion != NULL);
    NV_ASSERT(pMap != NULL);

    (void)blockStart; //Silence the compiler

    addrBase = pRegion->base;
    addrLimit = pRegion->limit;
    numFrames = (addrLimit - addrBase + 1) >> PMA_PAGE_SHIFT;

    NV_PRINTF(LEVEL_INFO, "Region: 0x%llx..0x%llx\n", addrBase, addrLimit);
    NV_PRINTF(LEVEL_INFO, "Total frames: 0x%llx\n", numFrames);

    for (i = 0; i < numFrames; i++)
    {
        currStatus = pPma->pMapInfo->pmaMapRead(pMap, i, NV_TRUE);
        if (i == 0)
        {
            blockStatus = currStatus;
            blockStart  = i;
        }

        if (blockStatus != currStatus)
        {
            NV_PRINTF(LEVEL_INFO, "%8llx..%8x: ", blockStart, i-1);
            pmaPrintBlockStatus(blockStatus);

            blockStatus = currStatus;
            blockStart  = i;
        }
    }
    NV_PRINTF(LEVEL_INFO, "%8llx..%8x: ", blockStart, i-1);
    pmaPrintBlockStatus(blockStatus);
}

NvBool
pmaStateCheck(PMA *pPma)
{
    NvU32 size, i;
    PMA_REGION_DESCRIPTOR *pState;
    void *pMap = NULL;

    if (pPma == NULL) return NV_FALSE;

    size = pPma->regSize;
    if (size >= PMA_REGION_SIZE) return NV_FALSE;

    if (pPma->bNuma)
    {
        if (!pPma->nodeOnlined)
        {
            NV_PRINTF(LEVEL_INFO, "Warning: NUMA state not onlined.\n");
            return NV_TRUE;
        }
        else if (pPma->numaNodeId == PMA_NUMA_NO_NODE)
        {
            NV_PRINTF(LEVEL_INFO, "NUMA node ID invalid.\n");
            return NV_FALSE;
        }
    }

    for (i = 0; i < size; i++)
    {
        pMap = pPma->pRegions[i];
        pState = pPma->pRegDescriptors[i];

        if (pMap == NULL || pState == NULL) return NV_FALSE;
    }

    return NV_TRUE;
}

NV_STATUS
pmaCheckRangeAgainstRegionDesc
(
    PMA   *pPma,
    NvU64  base,
    NvU64  size
)
{
    PMA_REGION_DESCRIPTOR *pRegionDesc;
    NvU32 regId = findRegionID(pPma, base);
    pRegionDesc = pPma->pRegDescriptors[regId];

    if ((base < pRegionDesc->base) ||
        ((base + size - 1) > pRegionDesc->limit))
    {
        return NV_ERR_INVALID_STATE;
    }

    return NV_OK;
}

void
pmaSetBlockStateAttribUnderPmaLock
(
    PMA           *pPma,
    NvU64          base,
    NvU64          size,
    PMA_PAGESTATUS pmaState,
    PMA_PAGESTATUS pmaStateWriteMask
)
{
    NvU64 numFrames, baseFrame, i;
    NvS32 regId;
    void *pMap;

    NV_ASSERT(pPma != NULL);
    NV_ASSERT(NV_IS_ALIGNED(base, PMA_GRANULARITY));
    NV_ASSERT(NV_IS_ALIGNED(size, PMA_GRANULARITY));

    regId = findRegionID(pPma, base); // assume same id for base+size TODO check this
    pMap = pPma->pRegions[regId];

    numFrames = size >> PMA_PAGE_SHIFT;
    baseFrame = (base - pPma->pRegDescriptors[regId]->base) >> PMA_PAGE_SHIFT;

    // Ensure accessing the frame data would not go out of bound in lower level
    NV_ASSERT_OR_RETURN_VOID((base + size - 1) <= pPma->pRegDescriptors[regId]->limit);

    for (i = 0; i < numFrames; i++)
    {
        pPma->pMapInfo->pmaMapChangeStateAttrib(pMap, (baseFrame + i), pmaState, pmaStateWriteMask);
    }
}

void
pmaSetBlockStateAttrib
(
    PMA           *pPma,
    NvU64          base,
    NvU64          size,
    PMA_PAGESTATUS pmaState,
    PMA_PAGESTATUS pmaStateWriteMask
)
{
    NV_ASSERT(pPma != NULL);

    portSyncSpinlockAcquire(pPma->pPmaLock);

    pmaSetBlockStateAttribUnderPmaLock(pPma, base, size, pmaState, pmaStateWriteMask);

    portSyncSpinlockRelease(pPma->pPmaLock);
}

// This must be called with the PMA lock held!
void
pmaStatsUpdateState
(
    NvU64 *pNumFree,
    NvU64 numPages,
    PMA_PAGESTATUS oldState,
    PMA_PAGESTATUS newState
)
{
    NV_ASSERT(pNumFree != NULL);

    oldState &= STATE_MASK;
    newState &= STATE_MASK;

    if ((oldState == STATE_FREE) && (newState != STATE_FREE))
    {
        (*pNumFree) -= numPages;
      //  NV_PRINTF(LEVEL_INFO, "Decrease to 0x%llx \n", *pNumFree);
    }
    else if ((oldState != STATE_FREE) && (newState == STATE_FREE))
    {
        (*pNumFree) += numPages;
      //  NV_PRINTF(LEVEL_INFO, "Increase to 0x%llx \n", *pNumFree);
    }
}

NvBool pmaIsEvictionPending(PMA *pPma)
{
    NvU32 i;
    void *pMap = NULL;

    for (i = 0; i < pPma->regSize; ++i)
    {
        pMap = pPma->pRegions[i];
        if (pPma->pMapInfo->pmaMapGetEvictingFrames(pMap) > 0)
            return NV_TRUE;
    }

    return NV_FALSE;
}

void pmaOsSchedule(void)
{
    // TODO Move osSchedule() to nvport?
#if !defined(SRT_BUILD)
    osSchedule();
#endif
}

/*!
 * @brief Handle eviction results from UVM and free the reuse pages to
 * OS if eviction failed half-way.
 * If eviction was successful, we have to double check the refcount and
 * decide if it's ok to reuse the pages for this eviction.
 * See bug 2019754.
 */
static NV_STATUS
_pmaCleanupNumaReusePages
(
    PMA         *pPma,
    NvU64        evictStart,
    NvU64        numFrames,
    NvBool       bEvictionSucceeded
)
{
    NvU32 regId;
    NvU64 sysPhysAddr = 0, sysPagePhysAddr = 0;
    NvU64 frameNum, addrBase, i;
    PMA_PAGESTATUS currentStatus;
    NvBool bRaisedRefcount = NV_FALSE;

    regId       = findRegionID(pPma, evictStart);
    addrBase    = pPma->pRegDescriptors[regId]->base;
    frameNum    = PMA_ADDR2FRAME(evictStart, addrBase);
    sysPhysAddr = evictStart + pPma->coherentCpuFbBase;

    if (bEvictionSucceeded == NV_TRUE)
    {
        //
        // If eviction from UVM succeeded, we double check the refcount and
        // update whether we should reuse these pages or not. If refcount is
        // greater than the appropriate number (1 for non-compound pages; for
        // compound pages, refcount should be equal to the number of pages
        // in this compound page), that means someone called get_user_pages
        // on those pages and we need to fail this eviction.
        //
        for (i = 0; i < numFrames; i++)
        {
            sysPagePhysAddr = sysPhysAddr + (i << PMA_PAGE_SHIFT);

            if (osGetPageRefcount(sysPagePhysAddr) > osCountTailPages(sysPagePhysAddr))
            {
                bRaisedRefcount = NV_TRUE;
                break;
            }
        }
    }

    if (!bEvictionSucceeded || bRaisedRefcount)
    {
        //
        // Eviction Failed. Need to clean up.
        // Since we set the NUMA_REUSE bit when we decide to reuse the pages,
        // we know exactly which pages to free both to OS and in PMA bitmap.
        //
        NvU8 osPageShift = osGetPageShift();

        NV_ASSERT_OR_RETURN(PMA_PAGE_SHIFT >= osPageShift, NV_ERR_INVALID_STATE);

        for (i = 0; i < numFrames; i++)
        {
            currentStatus = pPma->pMapInfo->pmaMapRead(pPma->pRegions[regId], (frameNum + i), NV_TRUE);
            sysPagePhysAddr = sysPhysAddr + (i << PMA_PAGE_SHIFT);

            if (currentStatus & ATTRIB_NUMA_REUSE)
            {
                osAllocReleasePage(sysPagePhysAddr, 1 << (PMA_PAGE_SHIFT - osPageShift));
                pPma->pMapInfo->pmaMapChangeStateAttrib(pPma->pRegions[regId], (frameNum + i),
                                                        STATE_FREE, (STATE_MASK | ATTRIB_NUMA_REUSE));
            }
        }

        return NV_ERR_NO_MEMORY;
    }

    return NV_OK;
}


/*!
 * @brief Eviction for contiguous allocation always evicts the full
 *  range to be allocated and the pmaMapScanContiguous()
 *  function sets the address to start eviction at as the first
 *  entry in the array of pages.
 */
NV_STATUS
_pmaEvictContiguous
(
    PMA              *pPma,
    void             *pMap,
    NvU64             evictStart,
    NvU64             evictEnd,
    MEMORY_PROTECTION prot
)
{
    NV_STATUS status;
    NvU64 numFramesToEvict;
    NvU64 evictSize;
    NvU64 frameEvictionsInProcess = pPma->pMapInfo->pmaMapGetEvictingFrames(pMap);
    NvBool pmaNumaEnabled = pPma->bNuma;

    evictSize = evictEnd - evictStart + 1;
    numFramesToEvict = evictSize >> PMA_PAGE_SHIFT;
    frameEvictionsInProcess += numFramesToEvict;
    pPma->pMapInfo->pmaMapSetEvictingFrames(pMap, frameEvictionsInProcess);

    pmaSetBlockStateAttribUnderPmaLock(pPma, evictStart, evictSize, ATTRIB_EVICTING, ATTRIB_EVICTING);

    // Release PMA lock before calling into UVM for eviction.
    portSyncSpinlockRelease(pPma->pPmaLock);

    if (pPma->bScrubOnFree)
    {
        PSCRUB_NODE pPmaScrubList = NULL;
        portSyncMutexRelease(pPma->pAllocLock);

        status = pPma->evictRangeCb(pPma->evictCtxPtr, evictStart, evictEnd, prot);

        portSyncMutexAcquire(pPma->pAllocLock);

        NV_PRINTF(LEVEL_INFO, "evictRangeCb returned with status %llx\n", (NvU64)status);

        if (status != NV_OK)
        {
            goto evict_cleanup;
        }
        // For NUMA we will scrub only what's being evicted and returned to client.
        if (pmaNumaEnabled)
        {
            //
            // The evicting contiguous range is marked as ATTRIB_EVICTING
            // and hence there will be no page stealing.
            //
            NvU64 count;

            if ((status = scrubSubmitPages(pPma->pScrubObj, (NvU32)evictSize, &evictStart,
                                           1, &pPmaScrubList, &count)) != NV_OK)
            {
                status = NV_ERR_INSUFFICIENT_RESOURCES;
                goto scrub_exit;
            }

            if (count > 0)
                _pmaClearScrubBit(pPma, pPmaScrubList, count);
        }

        if ((status = _pmaCheckScrubbedPages(pPma, evictSize, &evictStart, 1)) != NV_OK)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto scrub_exit; // just incase someone adds anything below.
        }

scrub_exit:
        portMemFree(pPmaScrubList);

        if (!pmaNumaEnabled &&
            (status == NV_ERR_INSUFFICIENT_RESOURCES))
        {
            NV_PRINTF(LEVEL_INFO, "ERROR: scrubber OOM!\n");
            goto exit; // fix this later, never exit early violating lock semantics
        }
    }
    else
    {
        status = pPma->evictRangeCb(pPma->evictCtxPtr, evictStart, evictEnd, prot);
        NV_PRINTF(LEVEL_INFO, "evictRangeCb returned with status %llx\n", (NvU64)status);
    }

evict_cleanup:
    // Reacquire PMA lock after returning from UVM and scrubber.
    portSyncSpinlockAcquire(pPma->pPmaLock);

    //
    // When we are in NUMA mode, we need to double check the NUMA_REUSE page attribute
    // to possibly return these pages to OS.
    //
    if (pmaNumaEnabled)
    {
        status = _pmaCleanupNumaReusePages(pPma, evictStart, numFramesToEvict, (status == NV_OK));
    }

    pmaSetBlockStateAttribUnderPmaLock(pPma, evictStart, evictSize, 0, ATTRIB_EVICTING | ATTRIB_NUMA_REUSE);

    frameEvictionsInProcess = pPma->pMapInfo->pmaMapGetEvictingFrames(pMap);
    NV_ASSERT(frameEvictionsInProcess >= numFramesToEvict);
    pPma->pMapInfo->pmaMapSetEvictingFrames(pMap, (frameEvictionsInProcess - numFramesToEvict));

exit:
    return status;
}

/*!
 * @brief Eviction for a non-contiguous range will allow the UVM driver to pick
 * and evict the specific pages being evicted. The UVM driver is required to hand
 * back pages to PMA in STATE_PIN state to prevent page stealing.
 */
NV_STATUS
_pmaEvictPages
(
    PMA              *pPma,
    void             *pMap,
    NvU64            *evictPages,
    NvU64             evictPageCount,
    NvU64            *allocPages,
    NvU64             allocPageCount,
    NvU64             pageSize,
    NvU64             physBegin,
    NvU64             physEnd,
    MEMORY_PROTECTION prot
)
{
    NvU64 i;
    NV_STATUS status;
    NvU64 numFramesToEvict = evictPageCount * (pageSize >> PMA_PAGE_SHIFT);
    NvU64 frameEvictionsInProcess = pPma->pMapInfo->pmaMapGetEvictingFrames(pMap);
    NvBool pmaNumaEnabled = pPma->bNuma;

    frameEvictionsInProcess += numFramesToEvict;
    pPma->pMapInfo->pmaMapSetEvictingFrames(pMap, frameEvictionsInProcess);

    //
    // Pin all the already allocated pages before unlocking the PMA
    // lock to prevent them from being allocated while eviction is
    // happening.
    //
    for (i = 0; i < allocPageCount; i++)
        pmaSetBlockStateAttribUnderPmaLock(pPma, allocPages[i], pageSize, STATE_PIN, STATE_PIN);

    // Release PMA lock before calling into UVM for eviction.
    portSyncSpinlockRelease(pPma->pPmaLock);

    if (pPma->bScrubOnFree)
    {
        PSCRUB_NODE pPmaScrubList = NULL;
        NvU64 count = 0;

        portSyncMutexRelease(pPma->pAllocLock);
        status = pPma->evictPagesCb(pPma->evictCtxPtr, pageSize, evictPages,
                            (NvU32)evictPageCount, physBegin, physEnd, prot);
        portSyncMutexAcquire(pPma->pAllocLock);

        NV_PRINTF(LEVEL_INFO, "evictPagesCb returned with status %llx\n", (NvU64)status);

        if (status != NV_OK)
        {
            goto evict_cleanup;
        }

        // Don't need to mark ATTRIB_SCRUBBING to protect the pages because they are already pinned
        status = scrubSubmitPages(pPma->pScrubObj, pageSize, evictPages,
                                  (NvU32)evictPageCount, &pPmaScrubList, &count);
        NV_ASSERT_OR_GOTO((status == NV_OK), scrub_exit);

        if (count > 0)
            _pmaClearScrubBit(pPma, pPmaScrubList, count);

        // Wait for our scrubbing to complete
       status = _pmaCheckScrubbedPages(pPma, pageSize, evictPages, (NvU32)evictPageCount);
scrub_exit:
       // Free the actual list, although allocated by objscrub
        portMemFree(pPmaScrubList);

        if ((status != NV_OK) && !pmaNumaEnabled)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES; // Caller expects this status.
            NV_PRINTF(LEVEL_ERROR, "ERROR: scrubber OOM!\n");
            NV_ASSERT_OK_OR_RETURN(status);
       }
    }
    else
    {
        status = pPma->evictPagesCb(pPma->evictCtxPtr, pageSize, evictPages,
                                (NvU32)evictPageCount, physBegin, physEnd, prot);
        NV_PRINTF(LEVEL_INFO, "evictPagesCb returned with status %llx\n", (NvU64)status);
    }

evict_cleanup:
    // Reacquire PMA lock after returning from UVM.
    portSyncSpinlockAcquire(pPma->pPmaLock);

    // Unpin the allocations now that we reacquired the PMA lock.
    for (i = 0; i < allocPageCount; i++)
        pmaSetBlockStateAttribUnderPmaLock(pPma, allocPages[i], pageSize, 0, STATE_PIN);

    frameEvictionsInProcess = pPma->pMapInfo->pmaMapGetEvictingFrames(pMap);
    NV_ASSERT(frameEvictionsInProcess >= numFramesToEvict);
    pPma->pMapInfo->pmaMapSetEvictingFrames(pMap, (frameEvictionsInProcess - numFramesToEvict));

    return status;
}

//
// Region selector
// Given specific PMA_ALLOCATE_* requirements, generate a list of possible intersecting regions
// Invalid regionList IDs set to -1
//
NV_STATUS
pmaSelector
(
    PMA                     *pPma,
    PMA_ALLOCATION_OPTIONS  *allocationOptions,
    NvS32                   *regionList
)
{
    // regSize never decreases + registered states don't change, so lock-free
    NvU32     i;
    NvU32     flags = allocationOptions->flags;
    NvU32     regionCount = 0;
    NV_STATUS status = NV_OK;

    NV_ASSERT(regionList != NULL);
    NV_ASSERT(allocationOptions != NULL);

    for (i=0; i < pPma->regSize; i++)
    {
        if (flags & PMA_ALLOCATE_SPECIFY_REGION_ID)
        {
            if (i != allocationOptions->regionId)
            {
                // Skip: wrong region ID
                continue;
            }
        }

        if (!!(flags & PMA_ALLOCATE_PROTECTED_REGION) ^
            (pPma->pRegDescriptors[i]->bProtected))
        {
            // Don't allow unprotected allocations in protected region
            // OR protected allocations in unprotected region.
            continue;
        }

        if (flags & PMA_ALLOCATE_SPECIFY_ADDRESS_RANGE)
        {
            NvU64 regionBegin, regionEnd;
            NvU64 rangeBegin, rangeEnd;
            PMA_REGION_DESCRIPTOR *regionDes;

            rangeBegin = allocationOptions->physBegin;
            rangeEnd = allocationOptions->physEnd;

            regionDes = pPma->pRegDescriptors[i];
            regionBegin = regionDes->base;
            regionEnd = regionDes->limit;

            if ((rangeEnd < regionBegin) || (rangeBegin > regionEnd))
            {
                // Skip: Requested range doesn't intersect region
                continue;
            }
        }

        if (flags & PMA_ALLOCATE_SPECIFY_MINIMUM_SPEED)
        {
            if (pPma->pRegDescriptors[i]->performance < allocationOptions->minimumSpeed)
            {
                // Skip: Region perf less than minimum threshold
                continue;
            }
        }

        if (regionCount > 0)
        {
            NvU32 j = regionCount;

            if (flags & PMA_ALLOCATE_REVERSE_ALLOC)
            {
                // Find insertion point (highest memory address to lowest)
                while ((j > 0) &&
                    (pPma->pRegDescriptors[i]->limit > pPma->pRegDescriptors[regionList[j-1]]->limit))
                {
                    regionList[j] = regionList[j-1];
                    j--;
                }
            }
            else if (flags & PMA_ALLOCATE_PREFER_SLOWEST)
            {
                // Find insertion point (slowest to fastest)
                while ((j > 0) &&
                    (pPma->pRegDescriptors[i]->performance < pPma->pRegDescriptors[regionList[j-1]]->performance))
                {
                    regionList[j] = regionList[j-1];
                    j--;
                }
            }
            else
            {
                // Find insertion point (fastest to slowest)
                while ((j > 0) &&
                    (pPma->pRegDescriptors[i]->performance > pPma->pRegDescriptors[regionList[j-1]]->performance))
                {
                    regionList[j] = regionList[j-1];
                    j--;
                }
            }

            // Insert in sorted order
            regionList[j] = i;
            regionCount++;
        }
        else
        {
            regionList[regionCount++] = i;
        }
    }

    // Invalidate the unused slots
    for (i = regionCount; i < pPma->regSize; i++)
    {
        regionList[i] = -1;
    }

    if (regionCount == 0)
    {
        status = NV_ERR_NO_MEMORY;
    }

    return status;
}

/*!
 * @brief This function will get a list of base+size and then goes in and
 * clear the scrubbing bit on any pages in these ranges. It is only called
 * when we know something is done scrubbing.
 *
 * @param[in] pPmaScrubList     The list of ranges that need to be cleared
 * @param[in] count             Length of the list
 *
 * Note:
 *  - This call takes the PMA lock! Do not call this with PMA lock held.
 */
void
_pmaClearScrubBit
(
    PMA         *pPma,
    PSCRUB_NODE pPmaScrubList,
    NvU64       count
)
{
    NvU32 i;
    NvU64 base;
    NvU64 size;

    NV_ASSERT(count > 0);
    portSyncSpinlockAcquire(pPma->pPmaLock);

    for (i = 0; i < count; i++)
    {
        base = pPmaScrubList[i].base;
        size = pPmaScrubList[i].size;
        NV_ASSERT(size > 0);
        pmaSetBlockStateAttribUnderPmaLock(pPma, base, size, 0, ATTRIB_SCRUBBING);
    }
    portSyncSpinlockRelease(pPma->pPmaLock);
}

/*!
 * @brief This function will optionally wait for scrubbing to be finished for a
 * list of pages, then check the scrubber status and clear the ATTRIB_SCRUBBING
 * page attribute on any pages that have completed scrubbing
 *
 * @param[in] chunkSize The size of each page being waited on
 * @param[in] pPages    The list of pages being waited on
 * @param[in] pageCount The number of pages we are waiting for
 *                      If pageCount == 0, then we don't wait for any pages
 *
 * Locking:
 * - This needs to be called without the PMA lock!
 * - This call will take the PMA lock internally to modify page attributes.
 */
NV_STATUS
_pmaCheckScrubbedPages
(
    PMA     *pPma,
    NvU64   chunkSize,
    NvU64   *pPages,
    NvU32   pageCount
)
{
    PSCRUB_NODE pPmaScrubList = NULL;
    NvU64 count = 0;
    NV_STATUS status = NV_OK;

    // If the caller wants to wait for something, we wait first before checking
    if (pageCount != 0)
    {
        if ((status = scrubWaitPages(pPma->pScrubObj, chunkSize, pPages, pageCount)) != NV_OK)
            return status;
    }

    status = scrubCheck(pPma->pScrubObj, &pPmaScrubList, &count);
    NV_ASSERT_OR_GOTO((status == NV_OK), exit);

    // This call takes the PMA lock!
    if (count > 0)
        _pmaClearScrubBit(pPma, pPmaScrubList, count);

exit:
    // Free the actual list, although allocated by objscrub
    portMemFree(pPmaScrubList);

    return status;
}


NV_STATUS
_pmaPredictOutOfMemory
(
    PMA                    *pPma,
    NvLength                allocationCount,
    NvU64                   pageSize,
    PMA_ALLOCATION_OPTIONS *allocationOptions
)
{
    NvU32 alignFlag, partialFlag;
    NvU64 alignment;
    NvU64 free2mbPages = 0;
    NvU64 bytesFree    = 0;

    alignFlag   = !!((allocationOptions->flags) & PMA_ALLOCATE_FORCE_ALIGNMENT);
    partialFlag = !!((allocationOptions->flags) & PMA_ALLOCATE_ALLOW_PARTIAL);
    alignment   = allocationOptions->alignment;

    if ((alignFlag && (alignment == _PMA_2MB)) || pageSize == _PMA_2MB)
    {
        if (allocationOptions->flags & PMA_ALLOCATE_PROTECTED_REGION)
        {
            free2mbPages = pPma->pmaStats.numFree2mbPagesProtected;
        }
        else
        {
            free2mbPages = pPma->pmaStats.numFree2mbPages -
                           pPma->pmaStats.numFree2mbPagesProtected;
        }

        // If we have at least one page free, don't fail a partial allocation
        if (partialFlag && (free2mbPages > 0))
        {
            return NV_OK;
        }

        if (free2mbPages < allocationCount)
        {
            return NV_ERR_NO_MEMORY;
        }
    }

    // Do a quick check and exit early if we are in OOM case
    if (allocationOptions->flags & PMA_ALLOCATE_PROTECTED_REGION)
    {
        bytesFree = pPma->pmaStats.numFreeFramesProtected << PMA_PAGE_SHIFT;
    }
    else
    {
        bytesFree = (pPma->pmaStats.numFreeFrames -
                     pPma->pmaStats.numFreeFramesProtected) << PMA_PAGE_SHIFT;
    }

    // If we have at least one page free, don't fail a partial allocation
    if (partialFlag && (bytesFree >= pageSize))
    {
        return NV_OK;
    }

    if (bytesFree < (pageSize * allocationCount))
    {
        return NV_ERR_NO_MEMORY;
    }

    return NV_OK;
}

/*!
 * @brief Internal function to intermittently free the blacklisted pages in the
 * range of allocation request. This will enable PMA to allow OS to manage those
 * blacklisted pages after being allocated.
 *
 * @param[in] pPma       PMA Object
 * @param[in] regId      PMA Region ID , where the allocation falls into
 * @param[in] rangeBegin Start address for the allocation range
 * @param[in] rangeSize  Size of the allocation region
 *
 * Locking:
 * - This needs to be called with the PMA lock!
 */

void
_pmaFreeBlacklistPages
(
    PMA   *pPma,
    NvU32  regId,
    NvU64  rangeBegin,
    NvU64  rangeSize
)
{
    NvU32 blacklistCount = 0;
    NvU32 chunk;
    NvU64 alignedBlacklistAddr;
    NvBool bClientManagedBlacklist = NV_FALSE;
    PMA_BLACKLIST_CHUNK *pBlacklistChunks, *pBlacklistChunk;

    pmaQueryBlacklistInfo(pPma, &blacklistCount, &bClientManagedBlacklist, &pBlacklistChunks);

    if(blacklistCount == 0)
        return; // return early, nothing to do.

    for (chunk = 0; chunk < blacklistCount; chunk++)
    {
        pBlacklistChunk = &pBlacklistChunks[chunk];
        if (pBlacklistChunk->bIsValid && (pBlacklistChunk->physOffset >= rangeBegin &&
               pBlacklistChunk->physOffset <= (rangeBegin + rangeSize - 1)))
        {
            //
            // Clear the blacklist attribute of the pages
            // Since physOffset here is the absolute address, make sure we align it to 64k
            //
            alignedBlacklistAddr = NV_ALIGN_DOWN64(pBlacklistChunk->physOffset, PMA_GRANULARITY);
            pmaSetBlockStateAttribUnderPmaLock(pPma, alignedBlacklistAddr, PMA_GRANULARITY, 0, ATTRIB_BLACKLIST);
            pBlacklistChunk->bIsValid = NV_FALSE;
            bClientManagedBlacklist = NV_TRUE;
        }
    }

    pmaSetClientManagedBlacklist(pPma, bClientManagedBlacklist);

    return;
}

/*!
 * @brief Internal function to reallocate blacklisted pages in the
 * range of allocation request.This is called, when the allocation requesting
 * blacklisting OFF fails or when the allocation with blacklisting OFF gets free-d.
 *
 * @param[in] pPma       PMA Object
 * @param[in] regId      PMA Region ID , where the allocation falls into
 * @param[in] rangeBegin Start address for the allocation range
 * @param[in] rangeSize  Size of the allocation region
 *
 * Locking:
 * - This needs to be called with the PMA lock!
 */

void _pmaReallocBlacklistPages
(
    PMA  *pPma,
    NvU32 regId,
    NvU64 rangeBegin,
    NvU64 rangeSize
)
{
    NvU32 blacklistCount = 0;
    NvU32 chunk;
    NvU64 alignedBlacklistAddr;
    NvU32 reallocatedBlacklistCount = 0;

    NvBool bClientManagedBlacklist = NV_FALSE;
    PMA_BLACKLIST_CHUNK *pBlacklistChunks, *pBlacklistChunk;
    pmaQueryBlacklistInfo(pPma, &blacklistCount, &bClientManagedBlacklist, &pBlacklistChunks);

    if (blacklistCount == 0 || !bClientManagedBlacklist)
    {
        return;
    }

    // Assert if scrub on free is enabled for client managed blacklist
    NV_ASSERT(pPma->bScrubOnFree == NV_FALSE);

    for (chunk = 0; chunk < blacklistCount; chunk++)
    {
        pBlacklistChunk = &pBlacklistChunks[chunk];
        if (!pBlacklistChunk->bIsValid &&
               (pBlacklistChunk->physOffset >= rangeBegin &&
                pBlacklistChunk->physOffset <= (rangeBegin + rangeSize -1)))
        {
            // Since physOffset here is the absolute address, make sure we align it to 64k
            alignedBlacklistAddr = NV_ALIGN_DOWN64(pBlacklistChunk->physOffset, PMA_GRANULARITY);
            pmaSetBlockStateAttribUnderPmaLock(pPma, alignedBlacklistAddr, PMA_GRANULARITY, ATTRIB_BLACKLIST, ATTRIB_BLACKLIST);
            pBlacklistChunk->bIsValid = NV_TRUE;
        }
        reallocatedBlacklistCount = (pBlacklistChunk->bIsValid == NV_TRUE) ? reallocatedBlacklistCount+1:
                  reallocatedBlacklistCount;
    }

    // Reset the flag if client handed over the blacklisted pages in their region to RM.
    if (chunk == reallocatedBlacklistCount)
    {
        pmaSetClientManagedBlacklist(pPma, NV_FALSE);
    }
}

/*!
 * @brief Internal function to lookup if the current frame is blacklisted already
 * If so, we will return NV_TRUE, otherwise NV_FALSE.
 *
 * @param[in] pPma       PMA Object
 * @param[in] regId      PMA Region ID , where the allocation falls into
 * @param[in] frameNum   Frame Number which needs to be checked.
 *
 * Locking:
 * - This needs to be called with the PMA lock!
 */
NvBool
_pmaLookupBlacklistFrame
(
    PMA   *pPma,
    NvU32  regId,
    NvU64  frameNum
)
{
    NvU32 blacklistCount;
    NvU64 addrBase;
    NvU32 chunk;
    NvU64 cliManagedBlackFrame = 0;

    NvBool bClientManagedBlacklist = NV_FALSE;
    PMA_BLACKLIST_CHUNK *pBlacklistChunks, *pBlacklistChunk;

    pmaQueryBlacklistInfo(pPma, &blacklistCount, &bClientManagedBlacklist, &pBlacklistChunks);

    if (blacklistCount == 0 || !bClientManagedBlacklist)
        return NV_FALSE;

    addrBase = pPma->pRegDescriptors[regId]->base;
    for (chunk = 0; chunk < blacklistCount; chunk++)
    {
        pBlacklistChunk = &pBlacklistChunks[chunk];
        if (pBlacklistChunk->bIsValid)
        {
            // calculate the frame addr
            cliManagedBlackFrame = PMA_ADDR2FRAME(pBlacklistChunk->physOffset, addrBase);
            if (cliManagedBlackFrame == frameNum)
            {
                return NV_TRUE;
            }
        }
    }
    return NV_FALSE;
}

/*!
 * @brief Returns a list of PMA-managed blocks with the specified state and
 *        attributes.
 *
 * @param[in]     pPma          PMA pointer
 * @param[in/out] ppList        Pointer to list of segments having specified
 *                              state and attributes
 * @param[in]     pageStatus    PMA page state and attribute
 *
 * @return
 *      NV_OK                   Success
 *      NV_ERR_NO_MEMORY        Failure to allocate list
 */
NV_STATUS
pmaBuildList
(
    PMA             *pPma,
    PRANGELISTTYPE  *ppList,
    PMA_PAGESTATUS   pageStatus
)
{
    NvU32 regionIdx, frameNum;
    NvU64 addrBase, addrLimit, numFrames;
    NvU64 blockStart = 0, blockEnd = 0;
    NvBool bBlockValid;
    PMA_PAGESTATUS pageState;
    PRANGELISTTYPE pRangeCurr, pRangeList = NULL;
    NV_STATUS status = NV_OK;
    void *pMap = NULL;

    for (regionIdx = 0; regionIdx < pPma->regSize; regionIdx++)
    {
        pMap = pPma->pRegions[regionIdx];
        addrBase = pPma->pRegDescriptors[regionIdx]->base;
        addrLimit = pPma->pRegDescriptors[regionIdx]->limit;
        numFrames = (addrLimit - addrBase + 1) >> PMA_PAGE_SHIFT;
        bBlockValid = NV_FALSE;

        for (frameNum = 0; frameNum < numFrames; frameNum++)
        {
            pageState = pPma->pMapInfo->pmaMapRead(pMap, frameNum, NV_TRUE);
            if (pageState & pageStatus)
            {
                if (bBlockValid)
                {
                    // Block start already found.  Find the end
                    blockEnd = frameNum;
                }
                else
                {
                    // Block start found.  Now find the end
                    blockStart = frameNum;
                    blockEnd = frameNum;
                    bBlockValid = NV_TRUE;
                }
            }
            else if (bBlockValid)
            {
                // Block found having required PMA page state. Store it in the list
                pRangeCurr = (PRANGELISTTYPE) portMemAllocNonPaged(sizeof(RANGELISTTYPE));
                if (pRangeCurr)
                {
                    pRangeCurr->base  = addrBase + blockStart * PMA_GRANULARITY;
                    pRangeCurr->limit = addrBase + blockEnd * PMA_GRANULARITY + PMA_GRANULARITY - 1;
                    pRangeCurr->pNext = pRangeList;
                    pRangeList = pRangeCurr;
                }
                else
                {
                    // Allocation failed
                    pmaFreeList(pPma, &pRangeList);
                    pRangeList = NULL;
                    status = NV_ERR_NO_MEMORY;
                    break;
                }

                bBlockValid = NV_FALSE;
            }
        }

        // No point checking further if we are already out of memory
        if (status == NV_ERR_NO_MEMORY)
            break;

        // Check if last frame was part of a block.
        if (bBlockValid)
        {
            // Block found having required PMA page state. Store it in the list
            pRangeCurr = (PRANGELISTTYPE) portMemAllocNonPaged(sizeof(RANGELISTTYPE));
            if (pRangeCurr)
            {
                pRangeCurr->base  = addrBase + blockStart * PMA_GRANULARITY;
                pRangeCurr->limit = addrBase + blockEnd * PMA_GRANULARITY + PMA_GRANULARITY - 1;
                pRangeCurr->pNext = pRangeList;
                pRangeList = pRangeCurr;
            }
            else
            {
                // Allocation failed
                pmaFreeList(pPma, &pRangeList);
                pRangeList = NULL;
                status = NV_ERR_NO_MEMORY;
                break;
            }
        }
    }

    *ppList = pRangeList;

    return status;
}

/*!
 * @brief Frees previously generated list of PMA-managed blocks with
 *        function pmaBuildList()
 *
 * @param[in]     pPma      PMA pointer
 * @param[in/out] ppList    Pointer to list of PMA segments
 *
 * @return
 *      None
 */
void
pmaFreeList
(
    PMA             *pPma,
    PRANGELISTTYPE  *ppList
)
{
    PRANGELISTTYPE pRangeCurr = *ppList;
    PRANGELISTTYPE pRangeNext;

    while (pRangeCurr)
    {
        pRangeNext = pRangeCurr->pNext;;
        portMemFree(pRangeCurr);
        pRangeCurr = pRangeNext;
    }

    *ppList = NULL;
}

NV_STATUS
pmaRegisterBlacklistInfo
(
    PMA                    *pPma,
    NvU64                   physAddrBase,
    PMA_BLACKLIST_ADDRESS  *pBlacklistPageBase,
    NvU32                   blacklistCount,
    NvBool                  bBlacklistFromInforom
)
{
    NvU32 i;
    NvU64 alignedBlacklistAddr;
    PMA_BLACKLIST_CHUNK *pBlacklistChunk = NULL;
    NvU32 nextBlacklistEntry = 0;
    NvU32 blacklistEntryIn = 0;

    if (blacklistCount == 0  || pBlacklistPageBase == NULL)
    {
        return NV_OK;
    }

    if (pPma->pBlacklistChunks == NULL)
    {
        pPma->pBlacklistChunks = (PMA_BLACKLIST_CHUNK *)
            portMemAllocNonPaged( PMA_MAX_BLACKLIST_ENTRIES * sizeof(PMA_BLACKLIST_CHUNK));
        if (pPma->pBlacklistChunks == NULL)
        {
            pPma->blacklistCount = 0;
            NV_PRINTF(LEVEL_ERROR, "ERROR: Insufficient memory to allocate blacklisting tracking structure.\n");
            return NV_ERR_NO_MEMORY;
        }
        portMemSet(pPma->pBlacklistChunks, 0, PMA_MAX_BLACKLIST_ENTRIES * sizeof(PMA_BLACKLIST_CHUNK));
    }

    nextBlacklistEntry = pPma->blacklistCount;

    for (i = nextBlacklistEntry; i < (nextBlacklistEntry + blacklistCount); i++)
    {
        pBlacklistChunk = &pPma->pBlacklistChunks[i];
        pBlacklistChunk->physOffset = pBlacklistPageBase[blacklistEntryIn].physOffset;
        pBlacklistChunk->bIsDynamic = pBlacklistPageBase[blacklistEntryIn].bIsDynamic;

        // Since physOffset here is the absolute address, make sure we align it to 64K
        alignedBlacklistAddr = NV_ALIGN_DOWN64(pBlacklistPageBase[blacklistEntryIn].physOffset, PMA_GRANULARITY);
        pmaSetBlockStateAttrib(pPma, alignedBlacklistAddr, PMA_GRANULARITY, ATTRIB_BLACKLIST, ATTRIB_BLACKLIST);
        pBlacklistChunk->bIsValid = NV_TRUE;

        //
        // In NUMA systems, memory allocation comes directly from kernel, which
        // won't check for ATTRIB_BLACKLIST. So pages need to be blacklisted
        // directly through the kernel.
        //
        // This is only needed for NUMA systems that auto online NUMA memory.
        // Other systems (e.g., P9) already do blacklisting in nvidia-persistenced.
        //
        if (pPma->bNuma && pPma->bNumaAutoOnline)
        {
            //
            // Only blacklist pages from inforom (i.e., during heap/PMA init) need 
            // to be blacklisted with kernel here. The blacklist pages stored in 
            // inforom need to remain blacklisted persistently across GPU resets -
            // kernel won't automatically blacklist these so RM must do it
            // explicitly here.
            //
            // Blacklist pages not from inforom (i.e., from ECC interrupt handling)
            // do not need to be blacklisted with kernel. This is because the ECC
            // interrupt will automatically trigger kernel itself to blacklist the page.
            //
            if (bBlacklistFromInforom)
            {
                NV_STATUS status;

                // Use physOffset without 64K alignment, because kernel may use a different page size.
                NV_PRINTF(LEVEL_INFO,
                          "NUMA enabled - blacklisting page through kernel at address 0x%llx (GPA) 0x%llx (SPA)\n",
                          pBlacklistPageBase[blacklistEntryIn].physOffset,
                          pBlacklistPageBase[blacklistEntryIn].physOffset + pPma->coherentCpuFbBase);

                status = osOfflinePageAtAddress(pBlacklistPageBase[blacklistEntryIn].physOffset + pPma->coherentCpuFbBase);
                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR, "osOfflinePageAtAddress() failed with status: %d\n", status);
                }
            }
        }

        blacklistEntryIn++;
    }

    pPma->blacklistCount += blacklistCount;

    return NV_OK;
}

void
pmaSetClientManagedBlacklist
(
    PMA    *pPma,
    NvBool bClientManagedBlacklist
)
{
    pPma->bClientManagedBlacklist = bClientManagedBlacklist;
}

void
pmaQueryBlacklistInfo
(
    PMA     *pPma,
    NvU32   *pBlacklistCount,
    NvBool  *pbClientManagedBlacklist,
    PMA_BLACKLIST_CHUNK **ppBlacklistChunks
)
{
    if (pBlacklistCount)
    {
        *pBlacklistCount = pPma->blacklistCount;
    }

    if (pbClientManagedBlacklist)
    {
        *pbClientManagedBlacklist = pPma->bClientManagedBlacklist;
    }

    if (ppBlacklistChunks)
    {
        *ppBlacklistChunks = pPma->pBlacklistChunks;
    }
}

NvBool
pmaIsBlacklistingAddrUnique
(
    PMA   *pPma,
    NvU64 physAddr
)
{
    NvU32 count = 0;
    PMA_BLACKLIST_CHUNK *pBlacklistChunk = NULL;
    if (pPma->blacklistCount == 0)
    {
        return NV_TRUE;
    }
    for (count = 0; count < pPma->blacklistCount; count++)
    {
        pBlacklistChunk = &pPma->pBlacklistChunks[count];
        if (pBlacklistChunk->physOffset == physAddr)
        {
            return NV_FALSE;
        }
    }
    return NV_TRUE;
}

