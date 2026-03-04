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
        // CPU-RM is not responsible for saving GSP-RM allocations
        pMemoryManager->Ram.fbRegion[i].bLostOnSuspend     = NV_TRUE;

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
