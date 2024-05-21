/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/heap.h"

/*!
 * @brief: Find FB region associated with a given FB offset
 *
 * @param[in] fbOffset  Start offset of FB block
 * @param[in] fbLimit   End offset of FB block
 *
 * @return FB_REGION_DESCRIPTOR if found
 *         NULL if not found
 */

PFB_REGION_DESCRIPTOR
memmgrLookupFbRegionByOffset_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    RmPhysAddr     fbOffset,
    RmPhysAddr     fbLimit
)
{
    NvU32       i = 0;
    PFB_REGION_DESCRIPTOR pFbRegion = NULL;

    // Find the region in which the candidate block resides
    while (i < pMemoryManager->Ram.numFBRegions)
    {
        pFbRegion = &pMemoryManager->Ram.fbRegion[i];
        // Does the block resides entirely within this region?  If so, then we are done searching.
        //++ Too restrictive for some platforms
//        if ((fbOffset >= pFbRegion->base) &&
//            (fbLimit <= pFbRegion->limit))
        // Does the block resides at least partially within this region?  If so, then we are done searching.
        if ((fbOffset >= pFbRegion->base) &&
            (fbOffset <= pFbRegion->limit))
        {
            if (fbLimit > pFbRegion->limit)
            {
                NV_PRINTF(LEVEL_WARNING, "STRADDLING REGION!\n");
            }
            return(pFbRegion);
        }
        i++;
    }

    return (NULL);
}

/*!
 * Regenerate FB region allocation priority list
 */
void
memmgrRegenerateFbRegionPriority_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NvU32   i, j, temp;

    // Re-build a list of allocatable regions, sorted by preference (highest to lowest)
    pMemoryManager->Ram.numFBRegionPriority = 0;

    for (i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
    {
        if (!pMemoryManager->Ram.fbRegion[i].bRsvdRegion)
        {
            pMemoryManager->Ram.fbRegionPriority[pMemoryManager->Ram.numFBRegionPriority] = i;
            pMemoryManager->Ram.numFBRegionPriority++;
        }
    }
    NV_ASSERT( pMemoryManager->Ram.numFBRegionPriority > 0 );
    if (pMemoryManager->Ram.numFBRegionPriority > 1)
    {
        for (i = 0; i < pMemoryManager->Ram.numFBRegionPriority - 1; i++)
        {
            for (j = i + 1; j < pMemoryManager->Ram.numFBRegionPriority; j++)
            {
                if (pMemoryManager->Ram.fbRegion[pMemoryManager->Ram.fbRegionPriority[i]].performance < pMemoryManager->Ram.fbRegion[pMemoryManager->Ram.fbRegionPriority[j]].performance)
                {
                    temp = pMemoryManager->Ram.fbRegionPriority[i];
                    pMemoryManager->Ram.fbRegionPriority[i] = pMemoryManager->Ram.fbRegionPriority[j];
                    pMemoryManager->Ram.fbRegionPriority[j] = temp;
                }
            }
        }
    }
}

/*!
 * @brief: Shifts the FB region IDs
 *
 * @param[in] regionId  FB Region ID from which we want to shift the FB regions
 *
 */
static void
_memmgrShiftFbRegions
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU32          regionId
)
{
    NvU32  fbRegion;

    NV_ASSERT(pMemoryManager->Ram.numFBRegions < MAX_FB_REGIONS);

    for (fbRegion=pMemoryManager->Ram.numFBRegions; fbRegion>regionId; fbRegion--)
    {
        pMemoryManager->Ram.fbRegion[fbRegion] = pMemoryManager->Ram.fbRegion[fbRegion-1];
    }

    pMemoryManager->Ram.numFBRegions++;
}

/*!
 * @brief: Insert a region into FbRegions[]
 *
 * @param[in] pRegion   Descriptor of New region
 *
 * @return new FbRegion ID
 */
NvU32
memmgrInsertFbRegion_IMPL
(
    OBJGPU                 *pGpu,
    MemoryManager          *pMemoryManager,
    PFB_REGION_DESCRIPTOR   pInsertRegion
)
{
    NvU32   insertRegion = 0;
    PFB_REGION_DESCRIPTOR pFbRegion;
    //
    // Consider that we have 4 Fb Regions
    //  +----------------------------- +
    //  | FB0  |  FB1  |  FB2  |  FB3  |
    //  +----------------------------- +
    // Find out the which region Insert region belongs to
    //
    for(insertRegion = 0; insertRegion < pMemoryManager->Ram.numFBRegions;insertRegion++)
    {
        pFbRegion = &pMemoryManager->Ram.fbRegion[insertRegion];

        // Find out whether insert region's Base and Limit lies within current FB Region
        if(pFbRegion->base <= pInsertRegion->base &&
            pFbRegion->limit >= pInsertRegion->limit)
        {
            break;
        }
    }

    if (insertRegion >= pMemoryManager->Ram.numFBRegions)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "New Region does not belong to any existing FB Regions\n");
        NV_ASSERT(0);
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "New Region belongs to FB Region 0x%x\n",
                  insertRegion);
    }


    pFbRegion = &pMemoryManager->Ram.fbRegion[insertRegion];

    //
    // Consider that we have 4 FB regions and for the sake of example we will
    // work on region 2
    //     0       1     2      3
    //  +--------------------------+
    //  | FB0  |  FB1  | FB2 | FB3 |
    //  +--------------------------+
    //

    // Case 1: Insert Region occupies all space in the FB region
    if ( pFbRegion->base == pInsertRegion->base &&
          pFbRegion->limit == pInsertRegion->limit)
    {
        //     0       1     2      3
        //  +--------------------------+
        //  | FB0  |  FB1  | NEW | FB3 |
        //  +--------------------------+
        //
        pMemoryManager->Ram.fbRegion[insertRegion] = *pInsertRegion;
    }
    //
    // Case 2: Insert Region's base address is same as FB Region.
    //          split the Fb Region into 2 parts and assign first part to new region
    //          in Below case, region 2 will get split
    //
    else if (pFbRegion->base == pInsertRegion->base &&
                pFbRegion->limit > pInsertRegion->limit)
    {
        //
        //     0       1     2      3       4
        //  +-----------------------------------+
        //  | FB0  |  FB1  | New | FB2' |  FB3  |
        //  +-----------------------------------+
        //
        _memmgrShiftFbRegions(pGpu, pMemoryManager, insertRegion);
        pMemoryManager->Ram.fbRegion[insertRegion] = *pInsertRegion;
        pMemoryManager->Ram.fbRegion[insertRegion+1].base = pInsertRegion->limit + 1;
    }
    //
    // Case 3: Insert Region's limit is same as FB Region
    //          split the region into 2 part and assign lower part of FB region to new Region
    //
    else if (pFbRegion-> base < pInsertRegion->base &&
              pFbRegion->limit == pInsertRegion->limit)
    {
        //
        //     0       1     2      3       4
        //  +-----------------------------------+
        //  | FB0  |  FB1  | FB2' | NEW |  FB3  |
        //  +-----------------------------------+
        //
        _memmgrShiftFbRegions(pGpu, pMemoryManager, insertRegion);
        insertRegion++;
        pMemoryManager->Ram.fbRegion[insertRegion] = *pInsertRegion;
        pMemoryManager->Ram.fbRegion[insertRegion-1].limit = pInsertRegion->base - 1;
    }
    //
    // Case 4: Insert region lies in between of FB region
    //          split the FB region in 3 parts and assign the middle part to insert region
    //
    else
    {
        //
        //     0       1     2      3       4        5
        //  +-------------------------------------------+
        //  | FB0  |  FB1  | FB2' | NEW |  FB2''  | FB3 |
        //  +-------------------------------------------+
        //
        _memmgrShiftFbRegions(pGpu, pMemoryManager, insertRegion);
        insertRegion++;
        pMemoryManager->Ram.fbRegion[insertRegion] = *pInsertRegion;

        _memmgrShiftFbRegions(pGpu, pMemoryManager, insertRegion);
        pMemoryManager->Ram.fbRegion[insertRegion+1] = pMemoryManager->Ram.fbRegion[insertRegion-1];
        pMemoryManager->Ram.fbRegion[insertRegion+1].base = pInsertRegion->limit + 1;
        pMemoryManager->Ram.fbRegion[insertRegion-1].limit = pInsertRegion->base - 1;
    }

    // Invalidate allocation priority list and regenerate it
    memmgrRegenerateFbRegionPriority(pGpu, pMemoryManager);

    return insertRegion;
}

/*
 *  @brief: Splits the fb region such that PMA regions,
 *          Rm internal reserve region and unusable regions
 *          are all separate regions
 */
void
memmgrRegionSetupCommon_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    FB_REGION_DESCRIPTOR    rsvdFbRegion;
    NvU32                   i;
    Heap                   *pHeap     = GPU_GET_HEAP(pGpu);
    NvU64                   heapBase  = pHeap->base;
    NvU64                   heapEnd   = pHeap->total - 1;
    NvU64                   fbTax     = memmgrGetFbTaxSize_HAL(pGpu, pMemoryManager);

    // TODO: Remove this check and enable on baremetal as well.
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        for (i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
        {
            //
            // Chop off anything that doesnt belong to the Heap object until
            // we figure out why vGPU initializes objheap smaller.
            //
            if  (pMemoryManager->Ram.fbRegion[i].base < heapBase)
            {
                portMemSet(&rsvdFbRegion, 0, sizeof(rsvdFbRegion));
                rsvdFbRegion.limit              = heapBase - 1;
                rsvdFbRegion.base               = pMemoryManager->Ram.fbRegion[i].base;
                rsvdFbRegion.rsvdSize           = 0;

                // Should never be true for internal heap
                rsvdFbRegion.bRsvdRegion        = NV_TRUE;
                rsvdFbRegion.performance        = pMemoryManager->Ram.fbRegion[i].performance;
                rsvdFbRegion.bSupportCompressed = pMemoryManager->Ram.fbRegion[i].bSupportCompressed;
                rsvdFbRegion.bSupportISO        = pMemoryManager->Ram.fbRegion[i].bSupportISO;
                rsvdFbRegion.bProtected         = pMemoryManager->Ram.fbRegion[i].bProtected;
                rsvdFbRegion.bInternalHeap      = NV_FALSE;

                i = memmgrInsertFbRegion(pGpu, pMemoryManager, &rsvdFbRegion);
                continue;
            }

            if  (pMemoryManager->Ram.fbRegion[i].limit > heapEnd)
            {
                portMemSet(&rsvdFbRegion, 0, sizeof(rsvdFbRegion));
                rsvdFbRegion.limit              = pMemoryManager->Ram.fbRegion[i].limit;
                rsvdFbRegion.base               = heapEnd + 1;
                rsvdFbRegion.rsvdSize           = 0;

                // Should never be true for internal heap
                rsvdFbRegion.bRsvdRegion        = NV_TRUE;
                rsvdFbRegion.performance        = pMemoryManager->Ram.fbRegion[i].performance;
                rsvdFbRegion.bSupportCompressed = pMemoryManager->Ram.fbRegion[i].bSupportCompressed;
                rsvdFbRegion.bSupportISO        = pMemoryManager->Ram.fbRegion[i].bSupportISO;
                rsvdFbRegion.bProtected         = pMemoryManager->Ram.fbRegion[i].bProtected;
                rsvdFbRegion.bInternalHeap      = NV_FALSE;

                i = memmgrInsertFbRegion(pGpu, pMemoryManager, &rsvdFbRegion);
            }
        }

        // Create a separate region for FB tax as the last FB region, it's not accessible to VF
        if (fbTax)
        {
            i = pMemoryManager->Ram.numFBRegions - 1;

            portMemSet(&rsvdFbRegion, 0, sizeof(rsvdFbRegion));
            rsvdFbRegion.limit              = pMemoryManager->Ram.fbRegion[i].limit;
            rsvdFbRegion.base               = pMemoryManager->Ram.fbRegion[i].limit - fbTax + 1;
            rsvdFbRegion.rsvdSize           = 0;

            // Should never be true for internal heap
            rsvdFbRegion.bRsvdRegion        = NV_TRUE;
            rsvdFbRegion.performance        = pMemoryManager->Ram.fbRegion[i].performance;
            rsvdFbRegion.bSupportCompressed = pMemoryManager->Ram.fbRegion[i].bSupportCompressed;
            rsvdFbRegion.bSupportISO        = pMemoryManager->Ram.fbRegion[i].bSupportISO;
            rsvdFbRegion.bProtected         = pMemoryManager->Ram.fbRegion[i].bProtected;
            rsvdFbRegion.bInternalHeap      = NV_FALSE;

            // Not required to be saved on hibernation, mark it as lost on suspend
            rsvdFbRegion.bLostOnSuspend     = NV_TRUE;

            i = memmgrInsertFbRegion(pGpu, pMemoryManager, &rsvdFbRegion);
        }
    }

    //
    // We really don't want to calculate this before we have complete
    // information from grCalculateGlobalCtxBufferSize, but we need to
    // setup the internal region
    //

    for (i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
    {
        KernelMemorySystem *pKernelMemorySystem  = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

        //
        // In NUMA systems unreserved FB memory Block is onlined to the
        // kernel after aligning to memblock size. If we have leftover
        // memory after the alignment this memory will just be unused.
        // Hence adding this memory to the reserved heap to avoid
        // assigning unused memory later to PMA and to keep both the
        // NUMA size and the PMA size same.
        //
        if (osNumaOnliningEnabled(pGpu->pOsGpuInfo) &&
            (pMemoryManager->Ram.fbRegion[i].base == KMEMSYS_FB_NUMA_ONLINE_BASE))
        {
            NvU64 unusedBlockSize = 0;
            NvU64 memblockSize = 0;
            NvU64 regionSize = pMemoryManager->Ram.fbRegion[i].limit - pMemoryManager->Ram.fbRegion[i].base + 1;
            NvU64 usableBlockSize = regionSize - pMemoryManager->Ram.fbRegion[i].rsvdSize;

            //
            // If numaOnlineSize is 0, this indicates that the memory is not onlined yet and hence align
            // the memoryblock with memblock size. (Currently this case gets executed in P9+GV100 config).
            //
            if (pKernelMemorySystem->numaOnlineSize == 0)
            {
                NV_ASSERT_OR_RETURN_VOID(osNumaMemblockSize(&memblockSize) == NV_OK);
                unusedBlockSize = usableBlockSize - KMEMSYS_FB_NUMA_ONLINE_SIZE(usableBlockSize, memblockSize);
            }
            else
            {
                //
                // If usableBlockSize is less than numaOnlineSize this indicates that a part
                // of RM reserved memory is onlined to the kernel. In this case skip creating
                // internal heap region.
                //
                NV_ASSERT_OR_RETURN_VOID(usableBlockSize >= pKernelMemorySystem->numaOnlineSize);

                unusedBlockSize = usableBlockSize - pKernelMemorySystem->numaOnlineSize;
            }
            pMemoryManager->Ram.fbRegion[i].rsvdSize += unusedBlockSize;
        }

        //
        // if the region has an RM reserved block and is not already reserved, subdivide it.
        //
        if ((pMemoryManager->Ram.fbRegion[i].rsvdSize > 0) &&
            (pMemoryManager->Ram.fbRegion[i].rsvdSize <= pMemoryManager->Ram.fbRegion[i].limit - pMemoryManager->Ram.fbRegion[i].base + 1) &&
            (pMemoryManager->Ram.fbRegion[i].bRsvdRegion == NV_FALSE))
        {
            portMemSet(&rsvdFbRegion, 0, sizeof(rsvdFbRegion));

            rsvdFbRegion.limit              = pMemoryManager->Ram.fbRegion[i].limit;
            rsvdFbRegion.base               = rsvdFbRegion.limit - pMemoryManager->Ram.fbRegion[i].rsvdSize + 1;
            rsvdFbRegion.rsvdSize           = pMemoryManager->Ram.fbRegion[i].rsvdSize;
            pMemoryManager->Ram.fbRegion[i].rsvdSize   = 0;
            // Should never be true for internal heap
            rsvdFbRegion.bRsvdRegion        = pMemoryManager->Ram.fbRegion[i].bRsvdRegion;
            rsvdFbRegion.performance        = pMemoryManager->Ram.fbRegion[i].performance;
            rsvdFbRegion.bSupportCompressed = pMemoryManager->Ram.fbRegion[i].bSupportCompressed;
            rsvdFbRegion.bSupportISO        = pMemoryManager->Ram.fbRegion[i].bSupportISO;
            rsvdFbRegion.bProtected         = pMemoryManager->Ram.fbRegion[i].bProtected;
            rsvdFbRegion.bInternalHeap      = NV_TRUE;

            i = memmgrInsertFbRegion(pGpu, pMemoryManager, &rsvdFbRegion);
        }
    }

    //
    // If FB size is being overriden, PMA should only own
    // memory below overrideHeapMax.
    // Also note on Pascal&&+ with FB override which below
    // code takes care of:
    // We can no longer can rely on RM reserve region being
    // in the pre-scrubbed region after FB size is restricted.
    // Non-prescrubbed region could overlap with vpr region.
    // Until fbstate init completes, RM cannot distinguish VPR
    // region, hence we have a WAR to prevent RM internal
    // allocations from falling into the VPR region by routing it
    // outside the fb override zone essentially to the prescrubbed
    // region. See fbHandleSizeOverrides_GP100. Till fbstate init
    // completes objheap will force all internal allocations to the
    // region outside the 'boot scrub'. To allow for it the region
    // should be in objheap - but not in pma.
    // Since we mark everything above overrideHeapMax as internal heap
    // the WAR will still be valid.
    //
    for (i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
    {
        //
        // Scan for the region that is above the overrideheapmax
        // that is not already reserved and allow objheap to manage it.
        //
        if ((pMemoryManager->Ram.fbRegion[i].bRsvdRegion == NV_FALSE) &&
            (pMemoryManager->overrideHeapMax < pMemoryManager->Ram.fbRegion[i].limit))
        {
            // Entire region is above the heap max
            if ((pMemoryManager->overrideHeapMax <  pMemoryManager->Ram.fbRegion[i].base))
            {
                pMemoryManager->Ram.fbRegion[i].bInternalHeap = NV_TRUE;
            }
            else // straddling create a separate region
            {
                portMemSet(&rsvdFbRegion, 0, sizeof(rsvdFbRegion));
                rsvdFbRegion.base  = pMemoryManager->overrideHeapMax + 1;
                rsvdFbRegion.limit = pMemoryManager->Ram.fbRegion[i].limit;
                rsvdFbRegion.bInternalHeap      = NV_TRUE;
                rsvdFbRegion.bRsvdRegion        = NV_FALSE;
                rsvdFbRegion.performance        = pMemoryManager->Ram.fbRegion[i].performance;
                rsvdFbRegion.bSupportCompressed = pMemoryManager->Ram.fbRegion[i].bSupportCompressed;
                rsvdFbRegion.bSupportISO        = pMemoryManager->Ram.fbRegion[i].bSupportISO;
                rsvdFbRegion.bProtected         = pMemoryManager->Ram.fbRegion[i].bProtected;
                i = memmgrInsertFbRegion(pGpu, pMemoryManager, &rsvdFbRegion);
            }
        }
    }
}

/*
 *  @brief: Prepares the fb region for PMA such that PMA regions,
 *          Rm internal reserve region and unusable regions
 *          are all separate regions. In order to do that RM has
 *          to calculate the reserved region size a little earlier.
 */
void
memmgrRegionSetupForPma_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    memmgrCalcReservedFbSpace(pGpu, pMemoryManager);
    memmgrRegionSetupCommon(pGpu, pMemoryManager);
}

/*!
 * Dump FB region table for debugging purposes.
 */
void
memmgrDumpFbRegions_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NvU32  i;

    NV_PRINTF(LEVEL_INFO, "FB region table: numFBRegions = %u.\n",
              pMemoryManager->Ram.numFBRegions);

    for (i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
    {
        NV_PRINTF(LEVEL_INFO, "FB region %u - Base=0x%llx, Limit=0x%llx, RsvdSize=0x%llx\n",
                  i,
                  pMemoryManager->Ram.fbRegion[i].base,
                  pMemoryManager->Ram.fbRegion[i].limit,
                  pMemoryManager->Ram.fbRegion[i].rsvdSize);
        NV_PRINTF(LEVEL_INFO, "FB region %u - Reserved=%d, InternalHeap=%d, Compressed=%d, ISO=%d, Protected=%d, "
                                              "Performance=%u, LostOnSuspend=%d, PreserveOnSuspend=%d\n",
                  i,
                  pMemoryManager->Ram.fbRegion[i].bRsvdRegion,
                  pMemoryManager->Ram.fbRegion[i].bInternalHeap,
                  pMemoryManager->Ram.fbRegion[i].bSupportCompressed,
                  pMemoryManager->Ram.fbRegion[i].bSupportISO,
                  pMemoryManager->Ram.fbRegion[i].bProtected,
                  pMemoryManager->Ram.fbRegion[i].performance,
                  pMemoryManager->Ram.fbRegion[i].bLostOnSuspend,
                  pMemoryManager->Ram.fbRegion[i].bPreserveOnSuspend);
    }
}

/*!
 *  @Clear FB sizing & regions
 *
 *  Variables initialized:
 *    pMemoryManager->Ram.fbAddrSpaceSizeMb   - Size of FB address space
 *    pMemoryManager->Ram.reservedMemSize     - Size of FB reserved region
 *    pMemoryManager->Ram.numFBRegions        - size of FB region list (can be 0)
 *    pMemoryManager->Ram.numFBRegionPriority - size of priority list
 *
 *  @returns NV_OK
 *
 */
void
memmgrClearFbRegions_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    pMemoryManager->Ram.fbAddrSpaceSizeMb = 0;
    pMemoryManager->Ram.numFBRegions = 0;
    pMemoryManager->Ram.numFBRegionPriority = 0;
    pMemoryManager->Ram.reservedMemSize = 0;
}
