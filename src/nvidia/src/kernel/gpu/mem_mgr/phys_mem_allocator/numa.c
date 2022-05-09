/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *
 * @brief Implementation for the NUMA interfaces, used by parent module PMA only.
 * This file interfaces with the RM Linux layer which interfaces with the
 * Linux kernel.
 */

#include "gpu/mem_mgr/phys_mem_allocator/numa.h"
#include "gpu/mem_mgr/phys_mem_allocator/phys_mem_allocator_util.h"
#include "gpu/mem_mgr/mem_scrub.h"
#include "utils/nvprintf.h"
#include "utils/nvassert.h"
#include "os/os.h"

//
// Local helper functions and declarations
//

//TODO merge or nuke these functions
static NV_STATUS _pmaNumaAvailableEvictablePage(PMA *pPma, NvS32 *validRegionList);
static NV_STATUS _pmaNumaAvailableEvictableRange(PMA *pPma, NvS32 *validRegionList,
    NvLength actualSize, NvU64 pageSize, NvU64 *evictStart, NvU64 *evictEnd);
static NV_STATUS _pmaNumaAllocateRange(PMA *pPma, NvU32 numaNodeId, NvLength actualSize,
    NvU64 pageSize, NvU64 *pPages, NvBool bScrubOnAlloc, NvBool allowEvict, NvS32 *validRegionList,
    NvU64 *allocatedCount);
static NV_STATUS _pmaNumaAllocatePages (PMA *pPma, NvU32 numaNodeId, NvU32 pageSize,
    NvLength allocationCount, NvU64 *pPages, NvBool bScrubOnAlloc, NvBool allowEvict, NvS32 *validRegionList,
    NvU64 *allocatedPages);

/*!
 * @brief Check if there is at least one evictable page from UVM.
 */
static NV_STATUS _pmaNumaAvailableEvictablePage
(
    PMA     *pPma,
    NvS32   *validRegionList
)
{
    NvU32           regionIdx;
    PMA_PAGESTATUS  frameState;
    void           *pMap   = NULL;
    NV_STATUS       status = NV_ERR_NO_MEMORY;

    for (regionIdx = 0; regionIdx < pPma->regSize; regionIdx++)
    {
        NvU32 regId, frameNum;
        NvU64 totalFrames;

        regId = (NvU32)validRegionList[regionIdx];

        if (validRegionList[regionIdx] == -1)
            continue;

        pMap = pPma->pRegions[regId];
        pPma->pMapInfo->pmaMapGetSize(pMap, &totalFrames);
        totalFrames >>= PMA_PAGE_SHIFT;

        for (frameNum = 0; frameNum < totalFrames; frameNum++)
        {
            frameState = pPma->pMapInfo->pmaMapRead(pMap, frameNum, NV_TRUE);
            if ((frameState & STATE_MASK) == STATE_UNPIN)
            {
                status = NV_OK;
                break;
            }
        }

        if (status == NV_OK)
            break;
    }

    if (status == NV_OK)
        NV_PRINTF(LEVEL_INFO, "Evictable frame: FOUND\n");
    else
        NV_PRINTF(LEVEL_INFO, "Evictable frame: NOT FOUND\n");

    return status;
}

/*!
 * @brief  Check if there is a contiguous range of
 *         evictable frame with UVM and get the start
 *         and end address if there is
 * In NUMA, OS manages memory and PMA will only track allocated memory in ALLOC_PIN
 * and ALLOC_UNPIN state. FREE memory is managed by OS and cannot be tracked by PMA
 * and hence PMA cannot consider FREE memory for eviction and can only consider frames
 * in known state to PMA or eviction. ALLOC_PIN cannot be evicted and hence only ALLOC_UNPIN
 * can be evictable.
 */
NV_STATUS _pmaNumaAvailableEvictableRange
(
    PMA      *pPma,
    NvS32    *validRegionList,
    NvLength  actualSize,
    NvU64     pageSize,
    NvU64    *evictStart,
    NvU64    *evictEnd
)
{
    void     *pMap  = NULL;
    NvU32     regionIdx;
    NV_STATUS status = NV_ERR_NO_MEMORY;

    if ((evictStart == NULL) || (evictEnd == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    *evictStart = 0;
    *evictEnd   = 0;

    for (regionIdx = 0; regionIdx < pPma->regSize; regionIdx++)
    {
        NvU64 addrBase;
        NvU32 regId;

        if (validRegionList[regionIdx] == -1)
            continue;

        regId = (NvU32)validRegionList[regionIdx];
        pMap  = pPma->pRegions[regId];
        addrBase = pPma->pRegDescriptors[regId]->base;


        if ((status = pPma->pMapInfo->pmaMapScanContiguousNumaEviction(pMap, addrBase, actualSize,
                                                                       pageSize, evictStart, evictEnd)) == NV_OK)
        {
            break;
        }
    }

    return status;
}

/*!
 * Check if the number of free frames is below the skip threshold percentage of total.
 * @return NV_TRUE  free frame count is below threshold.
 *         NV_FALSE otherwise.
 */
static NvBool _pmaCheckFreeFramesToSkipReclaim(PMA *pPma)
{
    return (100 * pPma->pmaStats.numFreeFrames < 
             (pPma->pmaStats.num2mbPages * (_PMA_2MB >> PMA_PAGE_SHIFT) * pPma->numaReclaimSkipThreshold));
}

/*!
 * @brief  Allocate contiguous memory for Numa
 *
 */
NV_STATUS _pmaNumaAllocateRange
(
    PMA     *pPma,
    NvU32    numaNodeId,
    NvLength actualSize,
    NvU64    pageSize,
    NvU64   *pPages,
    NvBool   bScrubOnAlloc,
    NvBool   allowEvict,
    NvS32   *validRegionList,
    NvU64   *allocatedCount
)
{
    NV_STATUS   status = NV_ERR_NO_MEMORY;
    NvU64  sysPhysAddr = 0, gpaPhysAddr = 0, evictStart = 0, evictEnd = 0;
    NvU32 flags = OS_ALLOC_PAGES_NODE_NONE;
    *allocatedCount    = 0;

    // check if numFreeFrames(64KB) are below a certain % of PMA managed memory(indicated by num2mbPages).
    if (_pmaCheckFreeFramesToSkipReclaim(pPma))
    {
        flags = OS_ALLOC_PAGES_NODE_SKIP_RECLAIM;
    }

    portSyncSpinlockRelease(pPma->pPmaLock);

    // Try to allocate contiguous allocation of actualSize from OS. Do not force RECLAIM
    status = osAllocPagesNode((int)numaNodeId, (NvLength)actualSize, flags, &sysPhysAddr);

    if (status == NV_OK)
    {
        NvU32 j;
        // j=0 head page is already refcounted  at allocation
        for (j = 1; j < (actualSize >> PMA_PAGE_SHIFT); j++)
        {
            osAllocAcquirePage(sysPhysAddr + (j << PMA_PAGE_SHIFT));
        }

        gpaPhysAddr = sysPhysAddr - pPma->coherentCpuFbBase;
        NV_ASSERT(gpaPhysAddr < pPma->coherentCpuFbBase);
        *allocatedCount = 1;

        if (bScrubOnAlloc)
        {
            PSCRUB_NODE pPmaScrubList = NULL;
            NvU64 count;

            if ((status = scrubSubmitPages(pPma->pScrubObj, (NvU32)actualSize, &gpaPhysAddr,
                                           1, &pPmaScrubList, &count)) != NV_OK)
            {
                status = NV_ERR_INSUFFICIENT_RESOURCES;
                goto scrub_exit;
            }

            if (count > 0)
                _pmaClearScrubBit(pPma, pPmaScrubList, count);

            if ((status = _pmaCheckScrubbedPages(pPma, actualSize, &gpaPhysAddr, 1)) != NV_OK)
            {
                status = NV_ERR_INSUFFICIENT_RESOURCES;
            }

scrub_exit:
            portMemFree(pPmaScrubList);

            if (status == NV_ERR_INSUFFICIENT_RESOURCES)
            {
                NV_PRINTF(LEVEL_ERROR, "ERROR: scrubber OOM!\n");
            }
        }

        portSyncSpinlockAcquire(pPma->pPmaLock);
        goto allocated;
    }

    portSyncSpinlockAcquire(pPma->pPmaLock);

    NV_PRINTF(LEVEL_INFO, "Allocate from OS failed for allocation size = %lld!\n",
                               (NvU64) actualSize);


    if (allowEvict)
    {
        // Check if UVM has evictable contiguous allocations of actualSize
        status = _pmaNumaAvailableEvictableRange(pPma, validRegionList,
                                             actualSize, pageSize,
                                             &evictStart, &evictEnd);
    }

    if ((status == NV_OK) && (evictEnd - evictStart + 1) >=  actualSize)
    {
        void *pMap = NULL;
        NvU32 regId;

        NV_ASSERT((evictEnd - evictStart + 1) ==  actualSize);
        status = NV_ERR_NO_MEMORY;
        regId = findRegionID(pPma, evictStart);
        pMap  = pPma->pRegions[regId];

        if (pMap != NULL)
        {
            //
            // Call UVM to evict the contiguous allocation and evict the rest to OS
            // UVM will call into PMA to free this contiguous range along with any excesses.
            // PMA will release only the excess allocation to OS in the free routine.
            // i.e., region evictStart to evictEnd is marked as 'ATTRIB_EVICTING' and will not
            // be returned to OS.
            //
            status = _pmaEvictContiguous(pPma, pMap, evictStart, evictEnd);

            if (status == NV_ERR_NO_MEMORY)
            {
                NV_PRINTF(LEVEL_INFO, "Eviction Failed = %llx to %llx!\n", evictStart, evictEnd);
            }
            else
            {
                NV_PRINTF(LEVEL_INFO, "Eviction succeeded = %llx to %llx Scrub status 0x%x!\n",
                                      evictStart, evictEnd, status);
                gpaPhysAddr =  evictStart;
                *allocatedCount = 1;
            }
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "pMap NULL cannot perform eviction\n");
        }
    }


allocated:

    // GPA needs to be acquired by shifting by the ATS aperture base address
    pPages[0] = gpaPhysAddr;

    return status;
}

/*!
 * @brief  Allocate discontiguous pages for Numa
 *
 */
static NV_STATUS _pmaNumaAllocatePages
(
    PMA     *pPma,
    NvU32    numaNodeId,
    NvU32    pageSize,
    NvLength allocationCount,
    NvU64   *pPages,
    NvBool   bScrubOnAlloc,
    NvBool   allowEvict,
    NvS32   *validRegionList,
    NvU64   *allocatedPages
)
{
    NV_STATUS status = NV_ERR_NO_MEMORY;
    NvU64     sysPhysAddr;
    NvU64     i = 0, j = 0;
    NvU32 flags = OS_ALLOC_PAGES_NODE_NONE;

    NV_ASSERT(allocationCount);

    // check if numFreeFrames are below certain % of PMA managed memory.
    if (_pmaCheckFreeFramesToSkipReclaim(pPma))
    {
        flags = OS_ALLOC_PAGES_NODE_SKIP_RECLAIM;
    }

    portSyncSpinlockRelease(pPma->pPmaLock);

    for (; i < allocationCount; i++)
    {
        status = osAllocPagesNode((int)numaNodeId, (NvLength) pageSize, flags, &sysPhysAddr);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "Alloc from OS failed for i= %lld allocationCount = %lld pageSize = %lld!\n",
                                   i, (NvU64) allocationCount, (NvU64) pageSize);
            break;
        }

        // GPA needs to be acquired by shifting by the ATS aperture base address
        NV_ASSERT(sysPhysAddr >= pPma->coherentCpuFbBase);
        pPages[i] = sysPhysAddr - pPma->coherentCpuFbBase;

        // Skip the head page at offset 0 (j=0) as it is refcounted at allocation
        for (j = 1; j < (pageSize >> PMA_PAGE_SHIFT); j++)
        {
            osAllocAcquirePage(sysPhysAddr + (j << PMA_PAGE_SHIFT));
        }
    }

    if (bScrubOnAlloc)
    {
        PSCRUB_NODE pPmaScrubList = NULL;
        NvU64 count;

        if ((status = scrubSubmitPages(pPma->pScrubObj, pageSize, pPages,
                                       i, &pPmaScrubList, &count)) != NV_OK)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto scrub_exit;
        }

        if (count > 0)
            _pmaClearScrubBit(pPma, pPmaScrubList, count);

        if ((status = _pmaCheckScrubbedPages(pPma, pageSize, pPages, (NvU32)i)) != NV_OK)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
        }

scrub_exit:
        portMemFree(pPmaScrubList);

        if (status == NV_ERR_INSUFFICIENT_RESOURCES)
        {
            NV_PRINTF(LEVEL_ERROR, "ERROR: scrubber OOM!\n");
            portSyncSpinlockAcquire(pPma->pPmaLock);
            goto exit;
        }
    }

    portSyncSpinlockAcquire(pPma->pPmaLock);

    if (( i < allocationCount) && allowEvict)
    {
        NvU32 regionIdx;

        // Check if there is atleast one evictable page
        status = _pmaNumaAvailableEvictablePage(pPma, validRegionList);

        if (status != NV_OK)
        {
            goto exit;
        }

        status = NV_ERR_NO_MEMORY;

        for (regionIdx = 0; regionIdx < pPma->regSize; regionIdx++)
        {
            NvU32 regId;
            NvU64 addrBase, addrLimit;
            void *pMap = NULL;

            if (validRegionList[regionIdx] == -1)
            {
                continue;
            }

            regId = (NvU32)validRegionList[regionIdx];
            pMap  = pPma->pRegions[regId];

            addrBase = pPma->pRegDescriptors[regId]->base;
            addrLimit = pPma->pRegDescriptors[regId]->limit;

            status = _pmaEvictPages(pPma, pMap,
                                    &pPages[i], (NvU32)(allocationCount - i),
                                    &pPages[0], i,
                                    pageSize, addrBase, addrLimit);

            if (status != NV_ERR_NO_MEMORY)
            {
                NV_PRINTF(LEVEL_INFO, "Frames %lld evicted in region %d of total allocationCount %lld Scrub status 0x%x!\n",
                                      i, regionIdx,  (NvU64) allocationCount, status);
                //
                // UVM can over evict, but will call into PMA only to evict the excess.
                // free startAddr + actualSize, (uvmAllocatedSize - actualSize) to OS.
                // Assume no under eviction. Overeviction is taken care of by the free routine.
                //
                i = allocationCount;
                break;
            }

            NV_PRINTF(LEVEL_INFO, "Eviction Failed %d pages !\n", (NvU32) (allocationCount - i));
        }

    }

exit:
    *allocatedPages = i;

    return status;
}


NV_STATUS pmaNumaAllocate
(
    PMA                    *pPma,
    NvLength                allocationCount,
    NvU32                   pageSize,
    PMA_ALLOCATION_OPTIONS *allocationOptions,
    NvU64                  *pPages
)
{
    NvU32    i;
    NV_STATUS  status    = NV_OK;
    NvU32    numaNodeId  = pPma->numaNodeId;
    NvS32    regionList[PMA_REGION_SIZE];
    NvU32    flags       = allocationOptions->flags;
    NvLength allocSize   = 0;
    NvLength allocCount  = 0;
    NvU32    contigFlag  = !!(flags & PMA_ALLOCATE_CONTIGUOUS);
    // As per bug #2444368, kernel scrubbing is too slow. Use the GPU scrubber instead
    NvBool bScrubOnAlloc = !(flags & PMA_ALLOCATE_NO_ZERO);
    NvBool    allowEvict = !(flags & PMA_ALLOCATE_DONT_EVICT);
    NvBool   partialFlag = !!(flags & PMA_ALLOCATE_ALLOW_PARTIAL);

    NvU64    finalAllocatedCount = 0;

    if (!pPma->bNuma)
    {
        NV_PRINTF(LEVEL_FATAL, "Cannot allocate from NUMA node %d on a non-NUMA system.\n",
                                numaNodeId);
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pageSize > _PMA_2MB)
    {
        NV_PRINTF(LEVEL_FATAL, "Cannot allocate with more than 2MB contiguity.\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pPma->nodeOnlined != NV_TRUE)
    {
        NV_PRINTF(LEVEL_INFO, "Cannot allocate from NUMA node %d before it is onlined.\n",
                               numaNodeId);
        return NV_ERR_INVALID_STATE;
    }

    if (contigFlag)
    {
        if (((NvU64)allocationCount) * ((NvU64) pageSize) > NV_U32_MAX)
        {
            NV_PRINTF(LEVEL_FATAL, "Cannot allocate more than 4GB contiguous memory in one call.\n");
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    // We are not changing the state. Can be outside the lock perhaps
    NV_CHECK_OK_OR_RETURN(LEVEL_FATAL, pmaSelector(pPma, allocationOptions, regionList));

    if (pPma->bScrubOnFree)
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
    else
    {
        //
        // Scrub-on-free feature is OFF, therefore we cannot do scrub-on-alloc
        // either because it uses the same HW
        //
        bScrubOnAlloc = NV_FALSE;
    }

    //
    // In the NUMA path, scrub on free does not provide enough safety guarantees
    // because pages are released to the kernel and they can be reused by other
    // processes. Therefore, we can only guarantee that the returned pages are
    // zero if scrub on alloc is used.
    //
    allocationOptions->resultFlags = (bScrubOnAlloc)? PMA_ALLOCATE_RESULT_IS_ZERO : 0;

    portSyncSpinlockAcquire(pPma->pPmaLock);

    if (contigFlag)
    {
        allocCount = 1;
        allocSize  = allocationCount * pageSize;
        status     = _pmaNumaAllocateRange(pPma, numaNodeId, allocSize, pageSize, pPages, bScrubOnAlloc, allowEvict, regionList, &finalAllocatedCount);
    }
    else
    {
        allocCount = allocationCount;
        allocSize  = pageSize;
        status     = _pmaNumaAllocatePages(pPma, numaNodeId, (NvU32) allocSize, allocCount, pPages, bScrubOnAlloc, allowEvict, regionList, &finalAllocatedCount);
    }

    if ((status == NV_ERR_NO_MEMORY) && partialFlag && (finalAllocatedCount > 0))
    {
        status = NV_OK;
    }

    if (status == NV_OK)
    {
        NvU32  regId;
        void  *pMap = NULL;
        NvU64  regAddrBase;
        NvU64  frameOffset;
        NvU64  frameCount = 0;
        PMA_PAGESTATUS curStatus = STATE_FREE;
        PMA_PAGESTATUS allocOption = !!(flags & PMA_ALLOCATE_PINNED) ?
                                        STATE_PIN : STATE_UNPIN;

        NV_PRINTF(LEVEL_INFO, "SUCCESS allocCount %lld, allocsize %lld eviction? %s pinned ? %s contig? %s\n",
                              (NvU64) allocCount,(NvU64) allocSize, (flags & PMA_ALLOCATE_DONT_EVICT) ?  "NOTALLOWED" : "ALLOWED",
                               !!(flags & PMA_ALLOCATE_PINNED) ? "PINNED" : "UNPINNED", contigFlag ? "CONTIG":"DISCONTIG");

        for (i = 0; i < finalAllocatedCount; i++)
        {
            NvU32 j;

            regId = findRegionID(pPma, pPages[i]);
            pMap  = pPma->pRegions[regId];
            regAddrBase = pPma->pRegDescriptors[regId]->base;
            frameCount  = allocSize >> PMA_PAGE_SHIFT;

            for (j = 0; j < frameCount; j++)
            {
                frameOffset = PMA_ADDR2FRAME(pPages[i], regAddrBase) + j;

                curStatus = pPma->pMapInfo->pmaMapRead(pMap, frameOffset, NV_TRUE);

                if (curStatus & ATTRIB_EVICTING)
                {
                    status = NV_ERR_NO_MEMORY;
                    break;
                }
                pPma->pMapInfo->pmaMapChangeStateAttrib(pMap, frameOffset, allocOption, NV_TRUE);
            }
            if (status != NV_OK)
                break;
        }

        if (status == NV_OK)
        {
            allocationOptions->numPagesAllocated = (NvLength)finalAllocatedCount;
        }
    }


    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "FAILED allocCount %lld, allocsize %lld eviction? %s pinned ? %s contig? %s\n",
                              (NvU64) allocCount, (NvU64) allocSize, (flags & PMA_ALLOCATE_DONT_EVICT) ?  "NOTALLOWED" : "ALLOWED",
                              !!(flags & PMA_ALLOCATE_PINNED) ? "PINNED" : "UNPINNED", contigFlag ? "CONTIG":"DISCONTIG");
        //
        // Free the entire allocation if scrubbing failed or if we had allocated evicting allocations.
        // Evicting allocation will be handled in the pmaEvictContiguous
        //
        if (finalAllocatedCount > 0)
            pmaNumaFreeInternal(pPma, pPages, finalAllocatedCount, pageSize, 0);

        status = NV_ERR_NO_MEMORY;
    }

    portSyncSpinlockRelease(pPma->pPmaLock);

    if (pPma->bScrubOnFree)
    {
        portSyncRwLockReleaseRead(pPma->pScrubberValidLock);
        portSyncMutexRelease(pPma->pAllocLock);
    }

    return status;
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
    NvU64 i, j;

    NV_PRINTF(LEVEL_INFO, "Freeing pPage[0] = %llx pageCount %lld\n", pPages[0], pageCount);

    for (i = 0; i < pageCount; i++)
    {
        NvU32 regId;
        NvU64 addrBase;
        NvU64 sysPhysAddr = 0;
        NvU64 frameNum;
        NvU64 framesPerPage;

        // Shift the GPA to acquire the bus address (SPA)
        NV_ASSERT(pPages[i] < pPma->coherentCpuFbSize);

        regId    = findRegionID(pPma, pPages[i]);
        addrBase = pPma->pRegDescriptors[regId]->base;
        frameNum = PMA_ADDR2FRAME(pPages[i], addrBase);
        framesPerPage = size >> PMA_PAGE_SHIFT;
        sysPhysAddr   = pPages[i] + pPma->coherentCpuFbBase;

        for (j = 0; j < framesPerPage; j++)
        {
            PMA_PAGESTATUS newStatus = STATE_FREE;
            PMA_PAGESTATUS currentStatus;
            NvU64 sysPagePhysAddr = 0;
            currentStatus = pPma->pMapInfo->pmaMapRead(pPma->pRegions[regId], (frameNum + j), NV_TRUE);

            //
            // When the pages are marked for evicting, we will skip free the page to OS
            // in order to reuse the page.
            //
            if (currentStatus & ATTRIB_EVICTING)
            {
                //
                // Evicting allocations are returned to new client and will be freed later.
                // We set the ATTRIB_NUMA_REUSE bit here just in case eviction fails later and we
                // need to release the page to OS in the allocation path.
                //
                if (currentStatus & STATE_UNPIN)
                {
                    pPma->pMapInfo->pmaMapChangeStateAttribEx(pPma->pRegions[regId], (frameNum + j),
                                                              ATTRIB_NUMA_REUSE, ATTRIB_NUMA_REUSE);
                }
                continue;
            }
            sysPagePhysAddr = sysPhysAddr + (j << PMA_PAGE_SHIFT);
            osAllocReleasePage(sysPagePhysAddr);
            pPma->pMapInfo->pmaMapChangeStateAttribEx(pPma->pRegions[regId], (frameNum + j), newStatus, ~ATTRIB_EVICTING);
        }
    }
}

void pmaNumaSetReclaimSkipThreshold(PMA *pPma, NvU32 skipReclaimPercent)
{
    portSyncSpinlockAcquire(pPma->pPmaLock);
    pPma->numaReclaimSkipThreshold = skipReclaimPercent;
    portSyncSpinlockRelease(pPma->pPmaLock);
}
