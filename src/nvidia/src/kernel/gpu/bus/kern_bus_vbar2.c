/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief Common Virtual BAR2 support. Because of this we cannot
 *        include any chip specific headers.
 */

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "mem_mgr/io_vaspace.h"
#include "vgpu/vgpu_events.h"
#include "containers/eheap_old.h"

// Prototypes for static functions
static NV_STATUS _kbusConstructVirtualBar2Heaps(KernelBus *pKernelBus, NvU32 gfid);
static NV_STATUS _kbusConstructVirtualBar2Lists(KernelBus *pKernelBus, NvU32 gfid);
static void _kbusDestructVirtualBar2Heaps(KernelBus *pKernelBus, NvU32 gfid);
static void _kbusDestructVirtualBar2Lists(KernelBus *pKernelBus, NvU32 gfid);
static void _freeRmApertureMap_VBAR2(OBJGPU *, KernelBus *, VirtualBar2MapEntry *, NvU32 flags);
static MemDescDestroyCallBack _kbusReleaseRmAperture_wrapper;

/*!
 * Internal function to allocate various bar2 heaps
 * @returns NV_STATUS
 *
 * @note This function handles the memory cleanup for heaps for failure paths.
 */
static NV_STATUS _kbusConstructVirtualBar2Heaps(KernelBus *pKernelBus, NvU32 gfid)
{
    NV_STATUS status = NV_OK;

    //
    // Setup eheap for RM bar2 space management.
    //
    // The number of maximum eheap region descriptors needed is 2 times the number
    // of cached BAR2 mappings, as the worst case is (alloc,free,alloc,free, etc.)
    // in VA space
    //

    NV_ASSERT_OK_OR_GOTO(status,
        kbusConstructVirtualBar2CpuVisibleHeap_HAL(pKernelBus, gfid), cleanup);

    NV_ASSERT_OK_OR_GOTO(status,
        kbusConstructVirtualBar2CpuInvisibleHeap_HAL(pKernelBus, gfid), cleanup);

cleanup:
    if (NV_OK != status)
        _kbusDestructVirtualBar2Heaps(pKernelBus, gfid);
    return status;
}

/*!
 * Internal function to allocate various bar2 lists
 * @returns NV_STATUS
 *
 * @note This function handles the memory cleanup for failure paths.
 */
static NV_STATUS _kbusConstructVirtualBar2Lists(KernelBus *pKernelBus, NvU32 gfid)
{
    //
    // TODO: This if() will go away when kbusConstructVirtualBar2 is moved back to kbusConstruct
    // from kbusStatePreInit().
    //
    if (pKernelBus->virtualBar2[gfid].pMapListMemory == NULL)
    {
        NvU32 i;

        // Pre-alloc the mapping list used for bar2 allocations
        listInitIntrusive(&pKernelBus->virtualBar2[gfid].freeMapList);
        listInitIntrusive(&pKernelBus->virtualBar2[gfid].cachedMapList);
        listInitIntrusive(&pKernelBus->virtualBar2[gfid].usedMapList);

        pKernelBus->virtualBar2[gfid].pMapListMemory = portMemAllocNonPaged(
                               sizeof(VirtualBar2MapEntry) * BUS_BAR2_MAX_MAPPINGS);
        if (pKernelBus->virtualBar2[gfid].pMapListMemory == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "Unable to alloc bar2 mapping list!\n");
            DBG_BREAKPOINT();
            _kbusDestructVirtualBar2Lists(pKernelBus, gfid);
            return NV_ERR_NO_MEMORY;
        }
        portMemSet(pKernelBus->virtualBar2[gfid].pMapListMemory, 0, sizeof(VirtualBar2MapEntry) * BUS_BAR2_MAX_MAPPINGS);

        // Initialize the free mapping list
        for (i = 0; i < BUS_BAR2_MAX_MAPPINGS; i++)
        {
            listAppendExisting(&pKernelBus->virtualBar2[gfid].freeMapList, &(pKernelBus->virtualBar2[gfid].pMapListMemory[i]));
        }
    }
    return NV_OK;
}

/*!
 * Initialize common virtual BAR2 data structures.
 *
 *  @param[in]   pGpu
 *  @param[in]   pKernelBus
 *  @param[in]   gfid
 *
 *  @returns None
 */
NV_STATUS
kbusConstructVirtualBar2_VBAR2(OBJGPU *pGpu, KernelBus *pKernelBus, NvU32 gfid)
{
    NV_STATUS    status = NV_OK;

    //
    // TODO: Enable these when cpu invisible heap gets separated from bar2 virtual heap
    // construction and virtual bar2 heap construction moves under kbusConstruct.
    //
//  NV_ASSERT_OR_RETURN(NULL == pKernelBus->virtualBar2.pMapListMemory, NV_ERR_INVALID_STATE);
//  NV_ASSERT_OR_RETURN(NULL == pKernelBus->virtualBar2.pVASpaceHeap, NV_ERR_INVALID_STATE);
//  NV_ASSERT_OR_RETURN(NULL == pKernelBus->virtualBar2.pVASpaceHiddenHeap, NV_ERR_INVALID_STATE);

    //
    // GSP-RM and VF in SRIOV heavy mode don't use the cpuVisible BAR2,
    // so no need to construct the BAR2 lists
    //
    if (!RMCFG_FEATURE_PLATFORM_GSP && IS_GFID_PF(gfid))
    {
        // Construct the various lists needed by BAR2
        status = _kbusConstructVirtualBar2Lists(pKernelBus, gfid);
        NV_ASSERT_OR_RETURN(NV_OK == status, status);
    }

    // Construct various eheaps needed by BAR2
    status = _kbusConstructVirtualBar2Heaps(pKernelBus, gfid);
    NV_ASSERT_OR_RETURN(NV_OK == status, status);

    // Default to 4KB alignment
    pKernelBus->virtualBar2[gfid].vAlignment = RM_PAGE_SIZE;

    // Used for issuing TLB invalidates
    pKernelBus->virtualBar2[gfid].flags      = VASPACE_FLAGS_BAR | VASPACE_FLAGS_BAR_BAR2;
    pKernelBus->virtualBar2[gfid].pPDB       = NULL;

#if (NV_PRINTF_ENABLED)
    pKernelBus->virtualBar2[gfid].mapCount = 0;
    pKernelBus->virtualBar2[gfid].cacheHit = 0;
    pKernelBus->virtualBar2[gfid].evictions = 0;
#endif

    return status;
}

/*!
 * Allocate and construct the cpu-visible bar2 heap
 *
 * @param   pKernelBus
 * @param   bfid
 *
 * @return  NV_OK or bubble up the returned error code from the callee
 */
NV_STATUS
kbusConstructVirtualBar2CpuVisibleHeap_VBAR2
(
    KernelBus  *pKernelBus,
    NvU32       gfid
)
{
    if (IS_GFID_VF(gfid))
    {
        return NV_OK;
    }

    //
    // TODO: This if() will go away when kbusConstructVirtualBar2 is moved back to kbusConstruct
    // from kbusStatePreInit().
    //
    if (pKernelBus->virtualBar2[gfid].pVASpaceHeap == NULL)
    {
        pKernelBus->virtualBar2[gfid].pVASpaceHeap = portMemAllocNonPaged(sizeof(OBJEHEAP));
        if (pKernelBus->virtualBar2[gfid].pVASpaceHeap == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "Unable to alloc bar2 eheap!\n");
            DBG_BREAKPOINT();
            return NV_ERR_NO_MEMORY;
        }
        constructObjEHeap(pKernelBus->virtualBar2[gfid].pVASpaceHeap,
            (pKernelBus->bar2[gfid].cpuVisibleBase),
            (pKernelBus->bar2[gfid].cpuVisibleLimit + 1),
            0,
            BUS_BAR2_MAX_MAPPINGS * 2);
    }
    return NV_OK;
}

/*!
 * Allocate and construct the cpu-invisible bar2 heap
 *
 * @param   pKernelBus
 * @param   gfid
 *
 * @return  NV_OK or bubble up the returned error code from the callee
 */
NV_STATUS
kbusConstructVirtualBar2CpuInvisibleHeap_VBAR2
(
    KernelBus  *pKernelBus,
    NvU32       gfid
)
{
    //
    // TODO: Move the cpu invisible Heap construction out of BAR2 construction and into kbusPreInit
    // so that virtual BAR2 can be constructed during kbusConstruct
    //
    // Setup eheap for Hidden bar2 space management only if Invisible region is required
    // Hidden heap doesn't require any pre-allocated memory structs.
    //
    if (pKernelBus->virtualBar2[gfid].pVASpaceHiddenHeap == NULL)
    {
        if (pKernelBus->bar2[gfid].cpuInvisibleLimit > pKernelBus->bar2[gfid].cpuInvisibleBase)
        {
            pKernelBus->virtualBar2[gfid].pVASpaceHiddenHeap = portMemAllocNonPaged(sizeof(OBJEHEAP));
            if (pKernelBus->virtualBar2[gfid].pVASpaceHiddenHeap == NULL)
            {
                NV_PRINTF(LEVEL_ERROR, "Unable to alloc hidden bar2 eheap!\n");
                DBG_BREAKPOINT();
                return NV_ERR_NO_MEMORY;
            }
            constructObjEHeap(pKernelBus->virtualBar2[gfid].pVASpaceHiddenHeap,
                              pKernelBus->bar2[gfid].cpuInvisibleBase,
                              (pKernelBus->bar2[gfid].cpuInvisibleLimit + 1), 0, 0);
        }
    }
    return NV_OK;
}

/*!
 * Internal function to destroy all heap objects under bar2
 * @returns void
 */
static void _kbusDestructVirtualBar2Heaps(KernelBus *pKernelBus, NvU32 gfid)
{
    if (NULL != pKernelBus->virtualBar2[gfid].pVASpaceHiddenHeap)
    {
        pKernelBus->virtualBar2[gfid].pVASpaceHiddenHeap->eheapDestruct(pKernelBus->virtualBar2[gfid].pVASpaceHiddenHeap);
        portMemFree(pKernelBus->virtualBar2[gfid].pVASpaceHiddenHeap);
        pKernelBus->virtualBar2[gfid].pVASpaceHiddenHeap = NULL;
    }

    if (NULL != pKernelBus->virtualBar2[gfid].pVASpaceHeap)
    {
        pKernelBus->virtualBar2[gfid].pVASpaceHeap->eheapDestruct(pKernelBus->virtualBar2[gfid].pVASpaceHeap);
        portMemFree(pKernelBus->virtualBar2[gfid].pVASpaceHeap);
        pKernelBus->virtualBar2[gfid].pVASpaceHeap = NULL;
    }
}

/*!
 * Internal function to clean up various mapping lists
 * @returns void
 */
static void _kbusDestructVirtualBar2Lists(KernelBus *pKernelBus, NvU32 gfid)
{
    listDestroy(&pKernelBus->virtualBar2[gfid].freeMapList);
    listDestroy(&pKernelBus->virtualBar2[gfid].cachedMapList);
    listDestroy(&pKernelBus->virtualBar2[gfid].usedMapList);

    portMemFree(pKernelBus->virtualBar2[gfid].pMapListMemory);
    pKernelBus->virtualBar2[gfid].pMapListMemory = NULL;
}

/*!
 * Clean-up and free virtual BAR2 SW resources
 *
 *  @param[in]   pGpu
 *  @param[in]   pKernelBus
 *  @param[in]   shutdown    True if shutting down
 *  @param[in]   gfid
 *
 *  @returns void
 */
void
kbusDestructVirtualBar2_VBAR2(OBJGPU *pGpu, KernelBus *pKernelBus, NvBool shutdown, NvU32 gfid)
{
    kbusFlushVirtualBar2_HAL(pGpu, pKernelBus, shutdown, gfid);
    _kbusDestructVirtualBar2Lists(pKernelBus, gfid);
    _kbusDestructVirtualBar2Heaps(pKernelBus, gfid);

    if (IS_GFID_PF(gfid))
    {
        NV_PRINTF(LEVEL_INFO,
                  "MapCount: %d Bar2 Hits: %d Evictions: %d\n",
                  pKernelBus->virtualBar2[gfid].mapCount,
                  pKernelBus->virtualBar2[gfid].cacheHit,
                  pKernelBus->virtualBar2[gfid].evictions);
    }
}

/*!
 * Clean-up virtual cache structures.
 *
 * Verify that there are no leaked or unreleased mappings.
 *
 * When shutting down the RM we should not have any outstanding memory descriptors
 * remaining in BAR2, so allow an error check for this.
 *
 * When suspending we only need to release them as a memory descriptor may live across
 * a resume, but we don't want to save the BAR2 mappings as BAR2 is destroyed and
 * rebuilt on resume. We use this call directly on suspend as we don't need to reclaim
 * data structures, just flush the cached mappings.
 *
 *  @param[in]   pGpu
 *  @param[in]   pKernelBus
 *  @param[in]   True if shutting down
 *
 *  @returns None
 */
void
kbusFlushVirtualBar2_VBAR2(OBJGPU *pGpu, KernelBus *pKernelBus, NvBool shutdown, NvU32 gfid)
{
    if (IS_GFID_VF(gfid))
    {
        return;
    }

    //
    // There should be no there are no active BAR2 mappings on shutdown. Failure indicates
    // there is a missing unmap BAR2 call somewhere in RM.
    //
    NV_ASSERT(listCount(&pKernelBus->virtualBar2[gfid].usedMapList) == 0);

    // There should be no unreleased mappings at shutdown
    NV_ASSERT(!shutdown || (listCount(&pKernelBus->virtualBar2[gfid].cachedMapList) == 0));

    // Release memory descriptors we still have cached
    while (listCount(&pKernelBus->virtualBar2[gfid].cachedMapList))
    {
        VirtualBar2MapEntry *pMap = listHead(&pKernelBus->virtualBar2[gfid].cachedMapList);

        NV_ASSERT(pMap->pMemDesc != NULL);

        _freeRmApertureMap_VBAR2(pGpu, pKernelBus, pMap,
                UPDATE_RM_APERTURE_FLAGS_INVALIDATE | UPDATE_RM_APERTURE_FLAGS_DISCARD);
    }
}

/*!
 * @brief one-time init of BAR2 Virtual Memory Manager.
 *
 *  Sets up CPU pointer to the page tables at the top of FB.
 *
 *  @param[in] pGpu
 *  @param[in] pKernelBus
 *
 *  @returns NV_OK on success, relevant error code otherwise
 */
NV_STATUS
kbusInitVirtualBar2_VBAR2
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NvU32              gfid;
    MEMORY_DESCRIPTOR *pMemDesc;
    NV_STATUS          status     = NV_OK;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    pMemDesc = pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc;

    if (kbusIsBarAccessBlocked(pKernelBus))
    {
        return NV_OK;
    }

    if ((pMemDesc != NULL) &&
        (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM))
    {
        // Get a CPU pointer to BAR2 page tables
        pKernelBus->virtualBar2[gfid].pPageLevels =
            memmgrMemDescBeginTransfer(GPU_GET_MEMORY_MANAGER(pGpu),
                                       pMemDesc,
                                       TRANSFER_FLAGS_PERSISTENT_CPU_MAPPING);
        NV_ASSERT_OR_RETURN(pKernelBus->virtualBar2[gfid].pPageLevels,
                          NV_ERR_INSUFFICIENT_RESOURCES);
    }
    else
    {
        //
        // In SYSMEM, page level instances are allocated one at a time. It is
        // not guaranteed that they are contiguous. Thus, SYSMEM page level
        // instances are dynamically mapped-in via memmap as needed instead of
        // having one static mapping.
        //
        NV_ASSERT(pMemDesc == NULL);
        pKernelBus->virtualBar2[gfid].pPageLevels = NULL;
    }

    return status;
}

/*!
 * @brief Sets up CPU pointer to the temporary page tables setup at
 *        the bottom of FB.
 *
 *  Sets up CPU pointer to the temporary page tables at the bottom of FB.
 *
 *  @param[in] pGpu
 *  @param[in] pKernelBus
 *
 *  @returns NV_OK on success, relevant error code otherwise
 */
NV_STATUS
kbusPreInitVirtualBar2_VBAR2
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NvU32              gfid;
    MEMORY_DESCRIPTOR *pMemDesc;
    NV_STATUS          status     = NV_OK;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    pMemDesc = pKernelBus->virtualBar2[gfid].pPageLevelsMemDescForBootstrap;

    if (kbusIsBarAccessBlocked(pKernelBus))
    {
        return NV_OK;
    }

    if ((pMemDesc != NULL) &&
        (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM))
    {
        // Get a fast CPU pointer to BAR2 page tables (either direct or BAR2).
        pKernelBus->virtualBar2[gfid].pPageLevelsForBootstrap = kbusMapRmAperture_HAL(pGpu,
                                                                                pMemDesc);
        NV_ASSERT_OR_RETURN(pKernelBus->virtualBar2[gfid].pPageLevelsForBootstrap,
                          NV_ERR_INSUFFICIENT_RESOURCES);
    }

    return status;
}

/*!
 *  @brief Helper routine to clean-up a unreferenced mapping
 *
 *  Mapping will be moved from the cached list to the free list.
 *
 *  @param[in]  pGpu
 *  @param[in]  pKernelBus
 *  @param[in]  pMap          Mapping to delete
 *  @param[in]  flags         Flags for kbusUpdateRmAperture_HAL
 *
 *  @returns None
 */
static void
_freeRmApertureMap_VBAR2
(
    OBJGPU              *pGpu,
    KernelBus           *pKernelBus,
    VirtualBar2MapEntry *pMap,
    NvU32                flags
)
{
    OBJEHEAP     *pVASpaceHeap = pKernelBus->virtualBar2[GPU_GFID_PF].pVASpaceHeap;
    EMEMBLOCK    *pBlockFree;
    NvU64         vAddr, vAddrSize;

    listRemove(&pKernelBus->virtualBar2[GPU_GFID_PF].cachedMapList, pMap);

    if (pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping)
    {
        pBlockFree = pVASpaceHeap->eheapGetBlock(pVASpaceHeap, pMap->vAddr, NV_FALSE);

        if (pBlockFree != NULL)
        {
            vAddr = pBlockFree->begin;
            vAddrSize = pBlockFree->end - vAddr + 1;

            kbusUpdateRmAperture_HAL(pGpu, pKernelBus,
                    pMap->pMemDesc, vAddr, vAddrSize, flags);
        }
    }

    pVASpaceHeap->eheapFree(pVASpaceHeap, pMap->vAddr);

    memdescRemoveDestroyCallback(pMap->pMemDesc, &pMap->memDescCallback);
    pMap->pMemDesc = NULL;

    listPrependExisting(&pKernelBus->virtualBar2[GPU_GFID_PF].freeMapList, pMap);
}

/*!
 * Second level of the RmAperture support for when a mapping is going to be in BAR2.
 *
 * Multiple mappings of a single MEMORY_DESCRIPTOR is now refernced counted in
 * the memory descriptor code.
 *
 * If this requests needs to update PTEs, call kbusUpdateRmAperture().
 *
 * It operates on a single GPU.  SLI is handled above this call.
 *
 *  @param[in]  pGpu
 *  @param[in]  pKernelBus
 *  @param[in]  pMemDesc The memory descriptor being mapped
 *
 *  @returns A CPU pointer to the memory
 */
static NvU8 *
kbusMapBar2ApertureCached_VBAR2
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32              flags
)
{
    VirtualBar2MapEntry   *pMapNew;
    OBJEHEAP              *pVASpaceHeap = NULL;
    NvU64                  vAddr        = 0;
    NvU32                  allocFlags   = 0;
    NvU64                  allocSize    = 0;
    NvBool                 bEvictNeeded = NV_FALSE;
    VirtualBar2MapListIter it;

    NV_ASSERT(pMemDesc->pGpu == pGpu);

#if NV_PRINTF_ENABLED
    pKernelBus->virtualBar2[GPU_GFID_PF].mapCount++;
#endif

    //
    // Reject a illegal memdesc.  Mappings that are too big will fail when
    // they can't find space in the eheap.
    //
    NV_ASSERT_OR_RETURN((pMemDesc->Size != 0) && (pMemDesc->PageCount != 0), NULL);

    NV_ASSERT_OR_RETURN(pKernelBus->virtualBar2[GPU_GFID_PF].pVASpaceHeap != NULL, NULL);

    //
    // Check the cached list for a recently used mapping
    //
    it = listIterAll(&pKernelBus->virtualBar2[GPU_GFID_PF].cachedMapList);
    while (listIterNext(&it))
    {
        VirtualBar2MapEntry *pMap = it.pValue;

        NV_ASSERT(pMap->pMemDesc);

        if (pMap->pMemDesc == pMemDesc)
        {
            // Move the mapping from the cached list to the used list
            listRemove(&pKernelBus->virtualBar2[GPU_GFID_PF].cachedMapList, pMap);
            listPrependExisting(&pKernelBus->virtualBar2[GPU_GFID_PF].usedMapList, pMap);

#if NV_PRINTF_ENABLED
            pKernelBus->virtualBar2[GPU_GFID_PF].cacheHit++;
#endif
            return pMap->pRtnPtr;
        }
    }

    //
    // We didn't find an existing mapping. If there are no free mappings
    // list entries available, bail here
    //
    if ((listCount(&pKernelBus->virtualBar2[GPU_GFID_PF].freeMapList) == 0) &&
        (listCount(&pKernelBus->virtualBar2[GPU_GFID_PF].cachedMapList) == 0))
    {
        NV_PRINTF(LEVEL_ERROR, "No free bar2 mapping struct left!\n");
        DBG_BREAKPOINT();
        return NULL;
    }

    //
    // Pack persistent mappings at the end of BAR2 space to avoid
    // fragmentation.
    //
    if (flags & TRANSFER_FLAGS_PERSISTENT_CPU_MAPPING)
    {
        allocFlags |= NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN;
    }

    //
    // Allocate VA SPACE
    //
    pVASpaceHeap = pKernelBus->virtualBar2[GPU_GFID_PF].pVASpaceHeap;
    allocSize = GET_SIZE_FROM_PAGE_AND_COUNT(pMemDesc->PageCount, pMemDesc->pageArrayGranularity);
    bEvictNeeded =
        (NV_OK != pVASpaceHeap->eheapAlloc(pVASpaceHeap, VAS_EHEAP_OWNER_NVRM,
                                           &allocFlags, &vAddr, &allocSize,
                                           pKernelBus->virtualBar2[GPU_GFID_PF].vAlignment,
                                           pKernelBus->virtualBar2[GPU_GFID_PF].vAlignment,
                                           NULL, NULL, NULL));

    if (bEvictNeeded)
    {
        //
        // Is a single mapping big enough to fit the new request? If so, lets evict it.
        // Search in reverse to find the oldest mapping.
        //
        VirtualBar2MapEntry *pMap;

        for (pMap = listTail(&pKernelBus->virtualBar2[GPU_GFID_PF].cachedMapList);
             pMap != NULL;
             pMap = listPrev(&pKernelBus->virtualBar2[GPU_GFID_PF].cachedMapList, pMap))
        {
            NV_ASSERT(pMap->pMemDesc != NULL);
            if ((GET_SIZE_FROM_PAGE_AND_COUNT(pMap->pMemDesc->PageCount, pMap->pMemDesc->pageArrayGranularity)) >=
                (GET_SIZE_FROM_PAGE_AND_COUNT(pMemDesc->PageCount, pMemDesc->pageArrayGranularity)))
            {
#if NV_PRINTF_ENABLED
                pKernelBus->virtualBar2[GPU_GFID_PF].evictions++;
#endif
                _freeRmApertureMap_VBAR2(pGpu, pKernelBus, pMap,
                        UPDATE_RM_APERTURE_FLAGS_INVALIDATE | UPDATE_RM_APERTURE_FLAGS_DISCARD);
                bEvictNeeded = NV_FALSE;
                break;
            }
        }

        //
        // If no single allocation has enough room, free all cached mappings and
        // hope we get enough contiguous VASpace.
        //
        if (bEvictNeeded)
        {
            while (listCount(&pKernelBus->virtualBar2[GPU_GFID_PF].cachedMapList))
            {
                VirtualBar2MapEntry *pMap = listHead(&pKernelBus->virtualBar2[GPU_GFID_PF].cachedMapList);

                NV_ASSERT(pMap->pMemDesc != NULL);

#if NV_PRINTF_ENABLED
                pKernelBus->virtualBar2[GPU_GFID_PF].evictions++;
#endif
                _freeRmApertureMap_VBAR2(pGpu, pKernelBus, pMap,
                        UPDATE_RM_APERTURE_FLAGS_INVALIDATE | UPDATE_RM_APERTURE_FLAGS_DISCARD);
            }
        }

        // try to reallocate BAR2|CPU space via the eheap
        if ( NV_OK != pVASpaceHeap->eheapAlloc(pVASpaceHeap, VAS_EHEAP_OWNER_NVRM, &allocFlags, &vAddr,
                                               &allocSize,
                                               pKernelBus->virtualBar2[GPU_GFID_PF].vAlignment,
                                               pKernelBus->virtualBar2[GPU_GFID_PF].vAlignment,
                                               NULL, NULL, NULL) )
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Not enough contiguous BAR2 VA space left allocSize %llx!\n",
                      allocSize);
            return NULL;
        }
    }

    //
    // Allocate pMap - evict oldest (last) cached entry if no free entries
    //
    if (listCount(&pKernelBus->virtualBar2[GPU_GFID_PF].freeMapList) == 0)
    {
#if NV_PRINTF_ENABLED
        pKernelBus->virtualBar2[GPU_GFID_PF].evictions++;
#endif
        _freeRmApertureMap_VBAR2(pGpu, pKernelBus,
                        listTail(&pKernelBus->virtualBar2[GPU_GFID_PF].cachedMapList),
                        UPDATE_RM_APERTURE_FLAGS_INVALIDATE | UPDATE_RM_APERTURE_FLAGS_DISCARD);
    }
    pMapNew = listHead(&pKernelBus->virtualBar2[GPU_GFID_PF].freeMapList);

    listRemove(&pKernelBus->virtualBar2[GPU_GFID_PF].freeMapList, pMapNew);

    // Update the page tables
    if (pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping == NULL ||
        (NV_OK != kbusUpdateRmAperture_HAL(pGpu, pKernelBus, pMemDesc, vAddr,
            pMemDesc->PageCount * pMemDesc->pageArrayGranularity,
            UPDATE_RM_APERTURE_FLAGS_INVALIDATE)))
    {
        pVASpaceHeap->eheapFree(pVASpaceHeap, vAddr);
        listPrependExisting(&pKernelBus->virtualBar2[GPU_GFID_PF].freeMapList, pMapNew);
        return NULL;
    }

    // Fill in the new mapping data
    pMapNew->pRtnPtr   = pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping + NvU64_LO32(vAddr) + pMemDesc->PteAdjust; // CPU pointer
    pMapNew->pMemDesc  = pMemDesc;
    pMapNew->vAddr     = vAddr;

    // Request notification when this memDesc is destroyed
    pMapNew->memDescCallback.destroyCallback = &_kbusReleaseRmAperture_wrapper;
    pMapNew->memDescCallback.pObject = (void *)pKernelBus;
    memdescAddDestroyCallback(pMemDesc, &pMapNew->memDescCallback);

    listPrependExisting(&pKernelBus->virtualBar2[GPU_GFID_PF].usedMapList, pMapNew);

    return pMapNew->pRtnPtr;
}

/*!
 * This is a wrapper function to trigger kbusReleaseRmAperture_HAL().
 * This must be kept compat with MemDescDestroyCallBack.
 */
static void
_kbusReleaseRmAperture_wrapper
(
    OBJGPU *pGpu,
    void   *pObject,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    KernelBus *pKernelBus = reinterpretCast(pObject, KernelBus*);
    kbusReleaseRmAperture_HAL(ENG_GET_GPU(pKernelBus), pKernelBus, pMemDesc);
}

/*!
 * Second level of the RmAperture support for when a mapping is going to be in BAR2.
 * We don't update PTEs here unless SPARSIFY flag is passed, just leave the mapping
 * cached and move on.  This is faster and we may get to reuse them later.
 *
 * This is common code shared by all chips after NV50
 *
 *  @param[out]    pGpu
 *  @param[in]     pKernelBus
 *  @param[in]     pMemDesc     Memory descriptor to unmap
 *  @param[in]     flags        TRANSFER_FLAGS
 *  @returns None
 */
static void
kbusUnmapBar2ApertureCached_VBAR2
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32              flags
)
{
    VirtualBar2MapListIter it;

    it = listIterAll(&pKernelBus->virtualBar2[GPU_GFID_PF].usedMapList);
    while (listIterNext(&it))
    {
        VirtualBar2MapEntry *pMap = it.pValue;

        if (pMap->pMemDesc == pMemDesc)
        {
            //
            // Remove from used list and move to the end start of the cached list.
            // Remapping of recent buffers is common.
            //
            listRemove(&pKernelBus->virtualBar2[GPU_GFID_PF].usedMapList, pMap);
            listPrependExisting(&pKernelBus->virtualBar2[GPU_GFID_PF].cachedMapList, pMap);

            if (flags & TRANSFER_FLAGS_DESTROY_MAPPING)
            {
                _freeRmApertureMap_VBAR2(pGpu, pKernelBus, pMap,
                                    UPDATE_RM_APERTURE_FLAGS_INVALIDATE | UPDATE_RM_APERTURE_FLAGS_SPARSIFY);
            }

            return;
        }
    }

    // Whoops, we didn't find the mapping region - something's wrong!
    NV_PRINTF(LEVEL_ERROR, "can't find mapping struct!\n");
    DBG_BREAKPOINT();
}

/*!
 * @brief Rubber-stamp scratch mapping as valid
 */
NvU8 *
kbusValidateBar2ApertureMapping_SCRATCH
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU8              *pCpu
)
{
    return pCpu;
}

/*!
 * @brief validate existing BAR2 mapping is still valid vs GPU reset
 *
 * @returns Existing or updated scratch buffer pointer
 */
NvU8 *
kbusValidateBar2ApertureMapping_VBAR2
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU8              *pCpu
)
{
    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu) &&
        !memdescGetFlag(pMemDesc, MEMDESC_FLAGS_GPU_IN_RESET))
    {
        //
        // Release existing mapping and replace it with a new mapping.
        //
        // The callee is responsbile for updating the pointer
        // after it is validated.  We cannot handle fixing stale
        // pointers to allocated before a GPU reset here.
        //
        kbusUnmapBar2ApertureWithFlags_HAL(pGpu, pKernelBus, pMemDesc, &pCpu,
                                           TRANSFER_FLAGS_NONE);
        return kbusMapBar2Aperture_HAL(pGpu, pKernelBus, pMemDesc,
                                           TRANSFER_FLAGS_NONE);
    }

    return pCpu;
}

/*!
 * @brief validate existing BAR2 mapping is still valid vs GPU reset
 *
 * @returns Existing or updated scratch buffer pointer
 */
NvU8 *
kbusValidateBar2ApertureMapping_VBAR2_SRIOV
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU8              *pCpu
)
{
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) || gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
        return kbusValidateBar2ApertureMapping_SCRATCH(pGpu, pKernelBus, pMemDesc, pCpu);

    return kbusValidateBar2ApertureMapping_VBAR2(pGpu, pKernelBus, pMemDesc, pCpu);
}

NvBool
kbusBar2IsReady_SCRATCH
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus
)
{
    // The scratch implementation is just an allocation, so it's always ready
    return NV_TRUE;
}

/*!
 * @brief Fake BAR2 map API to a scratch buffer.
 *
 * Use for old VGPU w/o SRIOV guard cases, and when we are recovering from TDR.
 */
NvU8 *
kbusMapBar2Aperture_SCRATCH
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32              flags
)
{
    if (pMemDesc->Size >= NV_U32_MAX)
    {
        return NULL;
    }

    return portMemAllocNonPaged(pMemDesc->Size);
}

NvBool
kbusBar2IsReady_VBAR2
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus
)
{
    //
    // The CPU mapping is the last part of BAR2 to be initialized, so we can
    // check it here to see if BAR2 is ready
    //
    return (pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping != NULL);
}

/*!
 * Dynamically map memory either a virtual BAR2 or with a directly CPU
 * mapping. This is the HAL entry point.
 *
 * This is common code shared by all chips after NV50
 *
 *  @param[in]   pGpu
 *  @param[in]   pKernelBus
 *  @param[in]   pMemDesc  Map this memory descriptor
 *  @param[in]   flags     Subset of TRANSFER_FLAGS
 *
 *  @returns Master CPU pointer and an SLI set of CPU pointers
 *
 *  @todo When using BAR2 this routine could not fail, but now with direct maps it can.
 */
NvU8 *
kbusMapBar2Aperture_VBAR2
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32              flags
)
{
    //
    // Fail the mapping when BAR2 access to CPR vidmem is blocked (for HCC)
    // It is however legal to allow non-CPR vidmem to be mapped to BAR2
    // Certain mapping requests which arrive with a specific flag set are allowed
    // to go through only in HCC devtools mode.
    //
    if (kbusIsBarAccessBlocked(pKernelBus) &&
       (!gpuIsCCDevToolsModeEnabled(pGpu) || !(flags & TRANSFER_FLAGS_PREFER_PROCESSOR)) &&
       !memdescGetFlag(pMemDesc, MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY))
    {
        os_dump_stack();
        NV_PRINTF(LEVEL_ERROR, "Cannot map/unmap CPR vidmem into/from BAR2\n");
        return NULL;
    }

    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
    {
        //
        // If the gpu is no longer in a state where any gpu access is allowed,
        // create some dummy system memory and return the pointer to the
        // caller.  All of the caller operations should now become nops.  Only
        // reads of this data might cause problems.
        //
        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_GPU_IN_RESET, NV_TRUE);
        return kbusMapBar2Aperture_SCRATCH(pGpu, pKernelBus, pMemDesc, flags);
    }

#if 0 // Useful for finding leaks
    NV_PRINTF(LEVEL_ERROR,
                "memDesc %p from function %p\n",
                pMemDesc,  __builtin_return_address(0));
#endif

    //
    // Raise warning on encountering Reflected Mapping on setups with sysmem nvlink.
    // On 0 FB systems, Reflected mapping may be used, so don't raise warning for that.
    //
    if ((memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM) &&
        (pGpu->getProperty(pGpu, PDB_PROP_GPU_NVLINK_SYSMEM)) &&
       !(pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB)))
    {
        //
        // Reflected mapping is deprecated and may cause GPU to enter deadlock
        // on certain systems and result into GPU fall off the bus. (B1829446)
        // If you see any hangs after this print, please fix the allocation
        // code in client for the memory tracked by this memDesc to avoid
        // reflected mapping.
        //
        NV_PRINTF(LEVEL_ERROR,
                  "GPU %d: Warning: Reflected Mapping Found: MapType = BAR and "
                  "AddressSpace = SYSMEM.\n", pGpu->gpuInstance);
        NV_ASSERT(0);
    }

    // Call the lower-level routine
    return kbusMapBar2ApertureCached_VBAR2(pGpu, pKernelBus, pMemDesc, flags);
}

NvBool
kbusBar2IsReady_VBAR2_SRIOV
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus
)
{
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) || gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
        return kbusBar2IsReady_SCRATCH(pGpu, pKernelBus);

    return kbusBar2IsReady_VBAR2(pGpu, pKernelBus);
}

/*!
 * @brief SRIOV BAR2 map filter to decide between SRIOV and classic VGPU behavior
 *
 * Turing/GA100 can run in both modes, so we need the dynamic check.
 */
NvU8 *
kbusMapBar2Aperture_VBAR2_SRIOV
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32              flags
)
{
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) || gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
        return kbusMapBar2Aperture_SCRATCH(pGpu, pKernelBus, pMemDesc, flags);

    return kbusMapBar2Aperture_VBAR2(pGpu, pKernelBus, pMemDesc, flags);
}

/*!
 * @brief Fake BAR2 unmap API to a scratch buffer.
 *
 * Use for old VGPU w/o SRIOV guard cases, and when we are recovering from TDR.
 */
void
kbusUnmapBar2ApertureWithFlags_SCRATCH
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU8             **pCpuPtr,
    NvU32              flags
)
{
    portMemFree(*pCpuPtr);
    kbusFlush_HAL(pGpu, pKernelBus, kbusGetFlushAperture(pKernelBus, memdescGetAddressSpace(pMemDesc)));
}

/*!
 * @brief Unmap instance memory, reversing kbusMapRmAperture_VBAR2
 *
 * If a Destroy flag is passed, actually clear the PTE mappings, and don't
 * leave on the cached free list.
 *
 * The value of *pCpuPtr must be the same as the value returned from
 * kbusMapRmAperture_VBAR2 when the original mapping was performed.
 *
 *  @param[in]   pGpu
 *  @param[in]   pKernelBus
 *  @param[in]   pMemDesc  Unmap this memory descriptor
 *  @param[in]   pCpuPtr   CPU VA previously returned by busMapRmAperture_VBAR2
 *  @param[in]   flags     Bitfield of flags to perform various operations
 *
 *  @returns None
 */
void
kbusUnmapBar2ApertureWithFlags_VBAR2
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU8             **pCpuPtr,
    NvU32              flags
)
{
    //
    // Fail the mapping when BAR2 access to CPR vidmem is blocked (for HCC)
    // It is however legal to allow non-CPR vidmem to be mapped to BAR2
    // Certain mapping requests which arrive with a specific flag set are allowed
    // to go through only in HCC devtools mode.
    //
    if (kbusIsBarAccessBlocked(pKernelBus) &&
       (!gpuIsCCDevToolsModeEnabled(pGpu) || !(flags & TRANSFER_FLAGS_PREFER_PROCESSOR)) &&
       !memdescGetFlag(pMemDesc, MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY))
    {
        NV_ASSERT(0);
        NV_PRINTF(LEVEL_ERROR, "Cannot map/unmap CPR vidmem into/from BAR2\n");
        return;
    }

    //
    // Free the dummy data we allocated for handling a reset GPU.
    // Let a map created before the reset go through the normal path
    // to clear out the memory.
    //
    if (memdescGetFlag(pMemDesc, MEMDESC_FLAGS_GPU_IN_RESET))
    {
        kbusUnmapBar2ApertureWithFlags_SCRATCH(pGpu, pKernelBus, pMemDesc, pCpuPtr, flags);
        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_GPU_IN_RESET, NV_FALSE);
        return;
    }

    // Call the lower-level routine
    kbusUnmapBar2ApertureCached_VBAR2(pGpu, pKernelBus, pMemDesc, flags);
}

/*!
 * @brief SRIOV BAR2 unmap filter to decide between SRIOV and classic VGPU behavior
 *
 * Turing/GA100 can run in both modes, so we need the dynamic check.
 */
void
kbusUnmapBar2ApertureWithFlags_VBAR2_SRIOV
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU8             **pCpuPtr,
    NvU32              flags
)
{
    // If SR-IOV is enabled, BAR2 mappings are managed by the guest.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) || gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        kbusUnmapBar2ApertureWithFlags_SCRATCH(pGpu, pKernelBus, pMemDesc, pCpuPtr, flags);
        return;
    }

    kbusUnmapBar2ApertureWithFlags_VBAR2(pGpu, pKernelBus, pMemDesc, pCpuPtr, flags);
}

/*!
 *  Release cached memory descriptor so the memory descriptor can be freed.
 *
 *  This is called from the memdescDestroy/memdescRelease path when ending the
 *  life of a memory descriptor.
 *
 *  We assume this should be on the free list and already unmapped.  If this
 *  doesn't happen it will show up as a leaked mapping when shutting down.  On
 *  debug drivers we check used list to help pinpoint source of a leaked
 *  mapping.
 *
 *  @param[in]   pGpu
 *  @param[in]   pKernelBus
 *  @param[in]   pMemDesc  Map this memory descriptor
 */
void
kbusReleaseRmAperture_VBAR2
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    VirtualBar2MapListIter it;

    it = listIterAll(&pKernelBus->virtualBar2[GPU_GFID_PF].cachedMapList);
    while (listIterNext(&it))
    {
        VirtualBar2MapEntry *pMap = it.pValue;

        if (pMap->pMemDesc == pMemDesc)
        {
            _freeRmApertureMap_VBAR2(pGpu, pKernelBus, pMap,
                        UPDATE_RM_APERTURE_FLAGS_INVALIDATE | UPDATE_RM_APERTURE_FLAGS_DISCARD);
            return;
        }
    }

#ifdef DEBUG
    it = listIterAll(&pKernelBus->virtualBar2[GPU_GFID_PF].usedMapList);
    while (listIterNext(&it))
    {
        VirtualBar2MapEntry *pMap = it.pValue;

        if (pMap->pMemDesc == pMemDesc)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Leaked mapping detected. Mapping not unmapped before memdescDestroy call.\n");
            DBG_BREAKPOINT();

            // Must be on cached listed to be freed
            listRemove(&pKernelBus->virtualBar2[GPU_GFID_PF].usedMapList, pMap);
            listAppendExisting(&pKernelBus->virtualBar2[GPU_GFID_PF].cachedMapList, pMap);

            _freeRmApertureMap_VBAR2(pGpu, pKernelBus, pMap,
                    UPDATE_RM_APERTURE_FLAGS_INVALIDATE | UPDATE_RM_APERTURE_FLAGS_DISCARD);

            break;
        }
    }
#endif
}

NV_STATUS kbusMapCpuInvisibleBar2Aperture_VBAR2
(
    OBJGPU     *pGpu,
    KernelBus  *pKernelBus,
    PMEMORY_DESCRIPTOR pMemDesc,
    NvU64      *pVaddr,
    NvU64       allocSize,
    NvU32       allocFlags,
    NvU32       gfid
)
{
    OBJEHEAP *pVASpaceHiddenHeap = pKernelBus->virtualBar2[gfid].pVASpaceHiddenHeap;
    NV_STATUS status;

    status = pVASpaceHiddenHeap->eheapAlloc(pVASpaceHiddenHeap, VAS_EHEAP_OWNER_NVRM,
                            &allocFlags, pVaddr, &allocSize,
                            pKernelBus->virtualBar2[gfid].vAlignment,
                            pKernelBus->virtualBar2[gfid].vAlignment,
                            NULL, NULL, NULL);

    if (status != NV_OK)
    {
        goto done;
    }

    if (IS_GFID_VF(gfid) && (pKernelBus->virtualBar2[gfid].pPageLevels == NULL))
    {
        pKernelBus->virtualBar2[gfid].pPageLevels = kbusMapRmAperture_HAL(pGpu,
                                                                    pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc);
        NV_ASSERT_OR_RETURN(pKernelBus->virtualBar2[gfid].pPageLevels,
                          NV_ERR_INSUFFICIENT_RESOURCES);
    }

    status = kbusUpdateRmAperture_HAL(pGpu, pKernelBus, pMemDesc, *pVaddr,
                pMemDesc->PageCount * pMemDesc->pageArrayGranularity, UPDATE_RM_APERTURE_FLAGS_INVALIDATE |
                                           UPDATE_RM_APERTURE_FLAGS_CPU_INVISIBLE_RANGE);

    if (IS_GFID_VF(gfid) && (pKernelBus->virtualBar2[gfid].pPageLevels != NULL))
    {
        kbusUnmapRmAperture_HAL(pGpu,
                                pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc,
                                &pKernelBus->virtualBar2[gfid].pPageLevels, NV_TRUE);
        pKernelBus->virtualBar2[gfid].pPageLevels = NULL;
    }

    if (status != NV_OK)
    {
        pVASpaceHiddenHeap->eheapFree(pVASpaceHiddenHeap, *pVaddr);
        *pVaddr = 0;
    }

done:
    return status;
}

void kbusUnmapCpuInvisibleBar2Aperture_VBAR2
(
    OBJGPU     *pGpu,
    KernelBus  *pKernelBus,
    PMEMORY_DESCRIPTOR pMemDesc,
    NvU64       vAddr,
    NvU32       gfid
)
{
    OBJEHEAP *pVASpaceHiddenHeap = pKernelBus->virtualBar2[gfid].pVASpaceHiddenHeap;

    if (!pVASpaceHiddenHeap)
    {
        return;
    }

    pVASpaceHiddenHeap->eheapFree(pVASpaceHiddenHeap, vAddr);
}

/*
 * @brief This function simply rewrites the PTEs for an already
 *        existing mapping cached in the usedMapList.
 *
 * This is currently used for updating the PTEs in the BAR2 page
 * tables at the top of FB after bootstrapping is done. The PTEs
 * for this mapping may be already existing in the page tables at
 * the bottom of FB. But those PTEs will be discarded once migration
 * to the page tables at the top of FB is done. So, before switching
 * to the new page tables, we should be rewrite the PTEs so that the
 * cached mapping does not become invalid. The *only* use case currently
 * is the CPU pointer to the new page tables at the top of FB.
 *
 * @param[in] pGpu        OBJGPU pointer
 * @param[in] pKernelBus  KernelBus pointer
 * @param[in] pMemDesc    MEMORY_DESCRIPTOR pointer.
 *
 * @return NV_OK if operation is OK
 *         Error otherwise.
 */
NV_STATUS
kbusRewritePTEsForExistingMapping_VBAR2
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    PMEMORY_DESCRIPTOR pMemDesc
)
{
    VirtualBar2MapListIter it;

    it = listIterAll(&pKernelBus->virtualBar2[GPU_GFID_PF].usedMapList);
    while (listIterNext(&it))
    {
        VirtualBar2MapEntry *pMap = it.pValue;

        if (pMap->pMemDesc == pMemDesc)
        {
            return kbusUpdateRmAperture_HAL(pGpu, pKernelBus, pMemDesc, pMap->vAddr,
                                            pMemDesc->Size, 0);
        }
    }
    return NV_ERR_INVALID_OPERATION;
}
