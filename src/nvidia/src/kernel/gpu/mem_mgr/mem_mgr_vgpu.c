/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "vgpu/vgpu_events.h"

/*!
 *  @@brief get FB tax size
 *
 *  @returns fb tax size
 */
NvU64
memmgrGetFbTaxSize_VF
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NvU64 ret = 0;
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    if (pVSI)
        ret = pVSI->fbTaxLength;

    return ret;
}

/*!
 *  @brief Fill in FB region information.
 */
NV_STATUS
memmgrInitBaseFbRegions_VF
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    if (pVSI->fbRegionInfoParams.numFBRegions == 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid number of FB regions (%d)\n",
                  pVSI->fbRegionInfoParams.numFBRegions);
        NV_ASSERT(0);
        return NV_ERR_INVALID_PARAMETER;
    }

    if (pVSI->fbRegionInfoParams.numFBRegions > 1)
    {
        // mixed density FB mem is not yet supported on vGPU
        NV_ASSERT(0);
        NV_PRINTF(LEVEL_ERROR,
                  "Mixed density FB regions not supported (%d)\n",
                  pVSI->fbRegionInfoParams.numFBRegions);

        return NV_ERR_INVALID_PARAMETER;
    }

    pMemoryManager->Ram.fbRegion[0].base = 0;
    pMemoryManager->Ram.fbRegion[0].limit = pVSI->fbLength - 1;
    pMemoryManager->Ram.fbRegion[0].rsvdSize = 0;
    pMemoryManager->Ram.fbRegion[0].bRsvdRegion = NV_FALSE;
    pMemoryManager->Ram.fbRegion[0].performance =  pVSI->fbRegionInfoParams.fbRegion[0].performance;
    pMemoryManager->Ram.fbRegion[0].bSupportCompressed = pVSI->fbRegionInfoParams.fbRegion[0].supportCompressed;
    pMemoryManager->Ram.fbRegion[0].bSupportISO =  pVSI->fbRegionInfoParams.fbRegion[0].supportISO;
    pMemoryManager->Ram.fbRegion[0].bProtected = pVSI->fbRegionInfoParams.fbRegion[0].bProtected;

    pMemoryManager->Ram.fbUsableMemSize = pMemoryManager->Ram.fbRegion[0].limit - pMemoryManager->Ram.fbRegion[0].base + 1;

    NV_PRINTF(LEVEL_INFO,
              "FB Region 0 : %x'%08x - %x'%08x perf = %d rsvd = %s ISO = %s internal = %s\n",
              NvU64_HI32(pMemoryManager->Ram.fbRegion[0].base),
              NvU64_LO32(pMemoryManager->Ram.fbRegion[0].base),
              NvU64_HI32(pMemoryManager->Ram.fbRegion[0].limit),
              NvU64_LO32(pMemoryManager->Ram.fbRegion[0].limit),
              pMemoryManager->Ram.fbRegion[0].performance,
              pMemoryManager->Ram.fbRegion[0].bRsvdRegion ? "NV_TRUE" : "NV_FALSE",
              pMemoryManager->Ram.fbRegion[0].bSupportISO ? "NV_TRUE" : "NV_FALSE",
              pMemoryManager->Ram.fbRegion[0].bInternalHeap ? "NV_TRUE" : "NV_FALSE");

    pMemoryManager->Ram.numFBRegions      = pVSI->fbRegionInfoParams.numFBRegions;
    pMemoryManager->Ram.fbTotalMemSizeMb  = (pMemoryManager->Ram.fbUsableMemSize) >> 20;
    pMemoryManager->Ram.fbAddrSpaceSizeMb = (pMemoryManager->Ram.fbRegion[pMemoryManager->Ram.numFBRegions - 1].limit + 1) >> 20;

    NV_PRINTF(LEVEL_INFO,
              "FB Reserved Memory = %x'%08x FB Usable Memory = %x'%08x FB Address "
              "Space (MB) = %x'%08x\n",
              NvU64_HI32(pMemoryManager->Ram.reservedMemSize),
              NvU64_LO32(pMemoryManager->Ram.reservedMemSize),
              NvU64_HI32(pMemoryManager->Ram.fbUsableMemSize),
              NvU64_LO32(pMemoryManager->Ram.fbUsableMemSize),
              NvU64_HI32(pMemoryManager->Ram.fbAddrSpaceSizeMb),
              NvU64_LO32(pMemoryManager->Ram.fbAddrSpaceSizeMb));

    return NV_OK;
}

/*!
 *  @brief Get the required heap space for GR module
 */
NvU32
memmgrGetGrHeapReservationSize_VF
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
    NV_ASSERT_OR_RETURN((pVSI != NULL), 0);

    return pVSI->ctxBuffInfo.engineContextBuffersInfo[0].engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_ATTRIBUTE_CB].size;
}
