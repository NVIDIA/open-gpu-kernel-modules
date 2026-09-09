/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

NV_STATUS
memmgrHandleSizeOverrides_GP100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NvU32  i;
    NvU64 fbOverrideSize = pMemoryManager->Ram.fbOverrideSizeMb << 20;

    if (pMemoryManager->Ram.fbOverrideSizeMb == NV_U64_MAX)
        return NV_OK;

    //
    // PMU and fake WPR regions are hard-coded and allocated from heap
    // Until this is fixed, we can't make heap respect the override
    // Only apply the restriction to client-allocatable regions
    //
    for (i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
    {
        FB_REGION_DESCRIPTOR newRegion = {0};
        FB_REGION_DESCRIPTOR *pFbRegion = &pMemoryManager->Ram.fbRegion[i];

        if (pFbRegion->limit < fbOverrideSize)
            continue;

        if (pFbRegion->bRsvdRegion || pFbRegion->bInternalHeap)
            continue;

        newRegion.base = NV_MAX(pFbRegion->base, fbOverrideSize);
        newRegion.limit = pFbRegion->limit;
        newRegion.rsvdSize = newRegion.limit - newRegion.base + 1;
        newRegion.bRsvdRegion = NV_TRUE;
        newRegion.performance = 0;
        newRegion.bSupportCompressed = NV_FALSE;
        newRegion.bSupportISO = NV_FALSE;
        newRegion.bProtected = NV_FALSE;
        newRegion.bInternalHeap = NV_FALSE;

        NV_ASSERT_OK_OR_RETURN(memmgrInsertFbRegion(pGpu, pMemoryManager, &newRegion, NULL));
        pMemoryManager->Ram.fbUsableMemSize -= newRegion.rsvdSize;
        pMemoryManager->Ram.reservedMemSize += newRegion.rsvdSize;
    }

    return NV_OK;
}
