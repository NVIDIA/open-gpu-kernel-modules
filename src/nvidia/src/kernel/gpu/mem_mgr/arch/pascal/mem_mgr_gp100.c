/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "platform/sli/sli.h"

#include "published/pascal/gp100/dev_mmu.h"
#include "class/cl906f.h"   // GF100_CHANNEL_GPFIFO
#include "class/clc0b5.h"   // PASCAL_DMA_COPY_A

/*!
 * @brief Determine the kind of compressed PTE for a given allocation for color.
 *
 * @param[in]   pFbAllocInfo FB_ALLOC_INFO pointer
 *
 * @returns     PTE Kind.
 */
NvU32
memmgrChooseKindCompressC_GP100
(
    OBJGPU                 *pGpu,
    MemoryManager          *pMemoryManager,
    FB_ALLOC_PAGE_FORMAT   *pFbAllocPageFormat
)
{
    extern NvU32 memmgrChooseKindCompressC_GM107(OBJGPU *pGpu, MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *pFbAllocPageFormat);
    NvU32  kind         = NV_MMU_PTE_KIND_PITCH;
    NvU32  attrdepth    = DRF_VAL(OS32, _ATTR, _DEPTH, pFbAllocPageFormat->attr);
    NvU32  aasamples    = DRF_VAL(OS32, _ATTR, _AA_SAMPLES, pFbAllocPageFormat->attr);

    if ((attrdepth == NVOS32_ATTR_DEPTH_32) &&
         ((aasamples == NVOS32_ATTR_AA_SAMPLES_4) ||
          (aasamples == NVOS32_ATTR_AA_SAMPLES_4_ROTATED) ||
          (aasamples == NVOS32_ATTR_AA_SAMPLES_4_VIRTUAL_8) ||
          (aasamples == NVOS32_ATTR_AA_SAMPLES_4_VIRTUAL_16)))
    {
        kind = NV_MMU_PTE_KIND_C32_MS4_4CBRA;
    }
    else if ((attrdepth == NVOS32_ATTR_DEPTH_64) &&
              ((aasamples == NVOS32_ATTR_AA_SAMPLES_4) ||
              (aasamples == NVOS32_ATTR_AA_SAMPLES_4_ROTATED) ||
              (aasamples == NVOS32_ATTR_AA_SAMPLES_4_VIRTUAL_8) ||
              (aasamples == NVOS32_ATTR_AA_SAMPLES_4_VIRTUAL_16)))
    {
        kind = NV_MMU_PTE_KIND_C64_MS4_4CBRA;
    }
    else
    {
        kind = memmgrChooseKindCompressC_GM107(pGpu, pMemoryManager, pFbAllocPageFormat);
    }

    return kind;

}

void
memmgrHandleSizeOverrides_GP100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    // If the fbOverrideSizeMb is set, insert a reserved region to "remove" the memory
    if (pMemoryManager->Ram.fbTotalMemSizeMb > pMemoryManager->Ram.fbOverrideSizeMb)
    {
        FB_REGION_DESCRIPTOR newRegion = {0};
        NvU32 newRegionIndex;
        NvU64 memDiff = (pMemoryManager->Ram.fbTotalMemSizeMb - pMemoryManager->Ram.fbOverrideSizeMb) << 20;
        //
        // overrideheapmax till scrub end is marked as reserved and unusable
        //
        NvU64 regionLimit = pMemoryManager->Ram.fbRegion[0].limit;
        NvU64 regionBase;

        // Ensure that regionLimit is 64KB aligned - necessary for PMA
        regionLimit = NV_ALIGN_UP(regionLimit, 0x10000) - 1;

        //
        // If there is an overridden heap max already, then reserve everything
        // above that. Otherwise, just go with where it would already land
        //
        regionBase = NV_MIN(pMemoryManager->overrideHeapMax, regionLimit - memDiff) + 1;

        newRegion.base = regionBase;
        newRegion.limit = regionLimit;
        newRegion.rsvdSize = 0;
        newRegion.bRsvdRegion = NV_TRUE;
        newRegion.performance = 0;
        newRegion.bSupportCompressed = NV_FALSE;
        newRegion.bSupportISO = NV_FALSE;
        newRegion.bProtected = NV_FALSE;
        newRegion.bInternalHeap = NV_FALSE;

        newRegionIndex = memmgrInsertFbRegion(pGpu, pMemoryManager, &newRegion);

        pMemoryManager->overrideHeapMax = pMemoryManager->Ram.fbRegion[newRegionIndex].base - 1;
    }
}

NV_STATUS
memmgrFinishHandleSizeOverrides_GP100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NV_STATUS rmStatus = NV_OK;

    if (pMemoryManager->overrideInitHeapMin > 0)
    {
        //
        // We want all the memory above the overrideHeapMax to be inaccessible,
        // so make everything above the MAX now reserved
        //
        NvU32 i;
        for (i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
        {
            if (pMemoryManager->Ram.fbRegion[i].limit > pMemoryManager->overrideHeapMax)
            {
                if (pMemoryManager->Ram.fbRegion[i].base >= pMemoryManager->overrideHeapMax + 1)
                {
                    // If the region is completely above the max, just mark it internal
                    pMemoryManager->Ram.fbRegion[i].bRsvdRegion = NV_TRUE;
                }
                else if (!pMemoryManager->Ram.fbRegion[i].bRsvdRegion)
                {
                    //
                    // Otherwise, if the region is straddling and not already reserved,
                    // split it into one reserved and one non-reserved region
                    //
                    FB_REGION_DESCRIPTOR newRegion = {0};
                    newRegion.base = pMemoryManager->overrideHeapMax + 1;
                    newRegion.limit = pMemoryManager->Ram.fbRegion[i].limit;
                    newRegion.rsvdSize = 0;
                    newRegion.bRsvdRegion = NV_TRUE;
                    newRegion.performance = 0;
                    newRegion.bSupportCompressed = NV_FALSE;
                    newRegion.bSupportISO = NV_FALSE;
                    newRegion.bProtected = NV_FALSE;
                    newRegion.bInternalHeap = NV_FALSE;
                    i = memmgrInsertFbRegion(pGpu, pMemoryManager, &newRegion);
                }
            }
        }

        //
        // Scrubbing should be finished before the next allocation, so this can
        // safely be reset.
        //
        pMemoryManager->overrideInitHeapMin = 0;
    }

    return rmStatus;
}

/*!
 *  Returns the max context size
 *
 *  @returns NvU64
 */
NvU64
memmgrGetMaxContextSize_GP100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    extern NvU64 memmgrGetMaxContextSize_GM200(OBJGPU *pGpu, MemoryManager *pMemoryManager);

    NvU64 size = memmgrGetMaxContextSize_GM200(pGpu, pMemoryManager);

    //
    // This function's original purpose was to estimate how much heap memory RM
    // needs to keep in reserve from Windows LDDM driver to pass WHQL MaxContexts
    // test.  This estimation is done after heap init before KMD allocates a
    // kernel-managed chunk.
    // UVM & PMA similarly require RM to estimate how much heap memory RM needs
    // to reserve for page tables, contexts, etc.  This estimation is used during
    // heap init to divide the FB into internal heap and external PMA managed
    // spaces.
    // Update for Pascal+ chips: on WDDMv2 KMD manages the reserve by locking down
    // lowest level PDEs at RM device creation time (=process creation) via
    // NV90F1_CTRL_CMD_VASPACE_RESERVE_ENTRIES rmControl call. Thus RM has to allocate
    // the low level PTs for the entire reserve which is 4Gb (range 4Gb-8Gb).
    // When PD0 is locked down and RM PD1 entries are valid, KMD can simply copy them
    // at the setRootPageTable ddi call and don't restore at the unsetRootPT time.
    // Because of the above reservation RM has to create quite a few 4k page tables and
    // this results in extra ~28k consumption per default DX device (with default 2 contexts).
    // On Kepler and Maxwell, the up-to-date wddm2 driver supports up to ~400 processes.
    // On Pascal, with the same amount of reserve, we can only have ~200 processes.
    // Hence we need to increase the RM physical reserve size for MMUv2 enabled chips
    // to have supported process count on parity with previous chips.
    // If any changes to RM reserve are introduced, for testing it with multi-process scenarios,
    // a new kmdtest (CreateNProcesses) should be used.


    if (RMCFG_FEATURE_PLATFORM_WINDOWS)
    {
        // Only needs increase in single GPU case as 400 process requirement is satisfied on SLI with the additional SLI reserve
        if (!IsSLIEnabled(pGpu) && pGpu->getProperty(pGpu, PDB_PROP_GPU_EXTERNAL_HEAP_CONTROL))
        {
            // KMD in WDDM mode
        }
    }

    return size;
}
