/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/gsp/gsp_static_config.h"
#include "vgpu/vgpu_events.h"
#include <ctrl/ctrl2080/ctrl2080fb.h>
#include "gpu/mem_mgr/fermi_dma.h"
#include "nvoc/prelude.h"

/*!
 * @brief Initialize FB regions from static info obtained from GSP FW. Also,
 *        initialize region table related RAM fields.
 */
NV_STATUS
memmgrInitBaseFbRegions_FWCLIENT
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS *pFbRegionInfoParams;
    NV2080_CTRL_CMD_FB_GET_FB_REGION_FB_REGION_INFO *pFbRegionInfo;
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
    NvU64 bias;
    NvU32 i;

    // sanity checks
    if (pGSCI == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Missing static info.\n");

        return NV_ERR_INVALID_STATE;
    }

    pFbRegionInfoParams = &pGSCI->fbRegionInfoParams;
    if (pFbRegionInfoParams->numFBRegions == 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Missing FB region table in GSP Init arguments.\n");

        return NV_ERR_INVALID_PARAMETER;
    }

    if (pFbRegionInfoParams->numFBRegions > MAX_FB_REGIONS)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Static info struct has more FB regions (%u) than FB supports (%u).\n",
                  pFbRegionInfoParams->numFBRegions, MAX_FB_REGIONS);

        return NV_ERR_INVALID_PARAMETER;
    }

    pMemoryManager->Ram.reservedMemSize = 0;
    pMemoryManager->Ram.fbUsableMemSize = 0;

    // Copy FB regions from static info structure
    for (i = 0; i < pFbRegionInfoParams->numFBRegions; i++)
    {
        pFbRegionInfo = &pFbRegionInfoParams->fbRegion[i];
        pMemoryManager->Ram.fbRegion[i].base               = pFbRegionInfo->base;
        pMemoryManager->Ram.fbRegion[i].limit              = pFbRegionInfo->limit;
        pMemoryManager->Ram.fbRegion[i].bProtected         = pFbRegionInfo->bProtected;
        pMemoryManager->Ram.fbRegion[i].bInternalHeap      = NV_FALSE;
        pMemoryManager->Ram.fbRegion[i].performance        = pFbRegionInfo->performance;
        pMemoryManager->Ram.fbRegion[i].bSupportCompressed = pFbRegionInfo->supportCompressed;
        pMemoryManager->Ram.fbRegion[i].bSupportISO        = pFbRegionInfo->supportISO;
        pMemoryManager->Ram.fbRegion[i].rsvdSize           = pFbRegionInfo->reserved;

        if (pFbRegionInfo->reserved)
        {
            pMemoryManager->Ram.fbRegion[i].bRsvdRegion = NV_TRUE;
            pMemoryManager->Ram.reservedMemSize += pMemoryManager->Ram.fbRegion[i].rsvdSize;
        }
        else
        {
            pMemoryManager->Ram.fbRegion[i].bRsvdRegion = NV_FALSE;
            pMemoryManager->Ram.fbUsableMemSize += (pMemoryManager->Ram.fbRegion[i].limit -
                                            pMemoryManager->Ram.fbRegion[i].base + 1);
        }
    }
    pMemoryManager->Ram.numFBRegions = pFbRegionInfoParams->numFBRegions;

    // Round up to the closest megabyte.
    bias = (1 << 20) - 1;
    //
    // fbTotalMemSizeMb was set to fbUsableMemSize. However, in RM-offload,
    // GSP-RM reserves some FB regions for its own usage, thus fbUsableMemSize
    // won't represent the exact FB size. Instead, we are taking the FB size
    // from the static info provided by GSP-RM.
    //
    pMemoryManager->Ram.fbTotalMemSizeMb  = (pGSCI->fb_length + bias) >> 20;
    pMemoryManager->Ram.fbAddrSpaceSizeMb =
        (pMemoryManager->Ram.fbRegion[pFbRegionInfoParams->numFBRegions - 1].limit + bias) >> 20;

    NV_ASSERT(pMemoryManager->Ram.fbAddrSpaceSizeMb >= pMemoryManager->Ram.fbTotalMemSizeMb);

    // Dump some stats, region table is dumped in memsysStateLoad
    NV_PRINTF(LEVEL_INFO, "FB Memory from Static info:\n");
    NV_PRINTF(LEVEL_INFO, "Reserved Memory=0x%llx, Usable Memory=0x%llx\n",
              pMemoryManager->Ram.reservedMemSize, pMemoryManager->Ram.fbUsableMemSize);
    NV_PRINTF(LEVEL_INFO, "fbTotalMemSizeMb=0x%llx, fbAddrSpaceSizeMb=0x%llx\n",
              pMemoryManager->Ram.fbTotalMemSizeMb, pMemoryManager->Ram.fbAddrSpaceSizeMb);

    return NV_OK;
}

/*!
 * @brief Set up CPU RM reserved memory space for physical carveout.
 */
NV_STATUS
memmgrPreInitReservedMemory_FWCLIENT
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    KernelBus     *pKernelBus     = GPU_GET_KERNEL_BUS(pGpu);
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NvU64          tmpAddr        = 0;

    if (pKernelDisplay)
    {
        // TODO: Determine the correct size of display instance memory
        // via instmemGetSize_HAL(), as well as other parameters.
        // I.e. refactor and leverage the code performing these tasks
        // in memmgrPreInitReservedMemory_GM107() today.
        tmpAddr += 0x10000;
    }

    if (gpuIsSelfHosted(pGpu))
    {
        //
        // Reserve space for the test buffer used in coherent link test
        // that is run early when memory allocation is not ready yet.
        //
        // if Self-Hosted is running in PCIe mode then this space will
        // will not be used, this should not cause any issue
        //
        pKernelBus->coherentLinkTestBufferBase = tmpAddr;
        tmpAddr += BUS_COHERENT_LINK_TEST_BUFFER_SIZE;
    }

    //
    // This has to be the very *last* thing in reserved memory as it
    // will may grow past the 1MB reserved memory window.  We cannot
    // size it until memsysStateInitLockedHal_GK104.
    //
    memmgrReserveBar2BackingStore(pGpu, pMemoryManager, &tmpAddr);

    NV_ASSERT(NvU64_LO32(tmpAddr) == tmpAddr);
    pMemoryManager->rsvdMemorySize = NvU64_LO32(tmpAddr);

    return NV_OK;
}

/*!
 *  @brief Calculate the FB reserved memory requirement.
 *
 *  @param[out] rsvdFastSize   generic reserved RM memory needed in fast region
 *  @param[out] rsvdSlowSize   generic reserved RM memory needed in slow region
 *  @param[out] rsvdISOSize    ISO-specific reserved RM memory needed
 */
void
memmgrCalcReservedFbSpaceHal_FWCLIENT
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU64         *rsvdFastSize,
    NvU64         *rsvdSlowSize,
    NvU64         *rsvdISOSize
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NvU64       rsvdSizeBytes;
    NvU64       smallPagePte = 0;
    NvU64       bigPagePte = 0;

    //
    // Minimum reserved memory for driver internal memdescAlloc() calls.
    // DO NOT increase this hard-coded memory to account for more reserved
    // FB memory, instead add individual calculations below.
    //
    rsvdSizeBytes = 5 * 1024 * 1024;

    // Add in USERD reservation
    rsvdSizeBytes += memmgrGetUserdReservedFbSpace_HAL(pGpu, pMemoryManager);

    // Reserve FB for Fault method buffers
    rsvdSizeBytes += kfifoCalcTotalSizeOfFaultMethodBuffers_HAL(pGpu, pKernelFifo, NV_TRUE);

    // smallPagePte = FBSize /4k * 8 (Small page PTE for whole FB)
    smallPagePte = NV_ROUNDUP((pMemoryManager->Ram.fbUsableMemSize / FERMI_SMALL_PAGESIZE) * 8, RM_PAGE_SIZE);

    // bigPagePte = FBSize /bigPageSize * 8 (Big page PTE for whole FB)
    bigPagePte = NV_ROUNDUP((pMemoryManager->Ram.fbUsableMemSize/ (kgmmuGetMaxBigPageSize_HAL(pKernelGmmu))) * 8,
                            RM_PAGE_SIZE);

    rsvdSizeBytes += smallPagePte;
    rsvdSizeBytes += bigPagePte;

    if (gpuIsClientRmAllocatedCtxBufferEnabled(pGpu))
    {
        rsvdSizeBytes += memmgrGetMaxContextSize_HAL(pGpu, pMemoryManager);
    }

    // Add in NV_REG_STR_RM_INCREASE_RSVD_MEMORY_SIZE_MB if applicable
    if (pMemoryManager->rsvdMemorySizeIncrement != 0)
    {
        // Allow reservation up to half of usable FB size
        if (pMemoryManager->rsvdMemorySizeIncrement > (pMemoryManager->Ram.fbUsableMemSize / 2))
        {
            pMemoryManager->rsvdMemorySizeIncrement = pMemoryManager->Ram.fbUsableMemSize / 2;
            NV_PRINTF(LEVEL_ERROR,
                      "RM can only increase reserved heap by 0x%llx bytes\n",
                      pMemoryManager->rsvdMemorySizeIncrement);
        }
        rsvdSizeBytes += pMemoryManager->rsvdMemorySizeIncrement;
    }

    rsvdSizeBytes = NV_ROUNDUP(rsvdSizeBytes, RM_PAGE_SIZE_64K);

    // mixed memory type/density only existed in pre-Pascal chips
    *rsvdFastSize = rsvdSizeBytes;
    *rsvdSlowSize = 0;
    *rsvdISOSize  = 0;
}
