/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "ctrl/ctrl2080/ctrl2080gpu.h"

NV_STATUS
memmgrCreateUnprotectedVidmemCarveout_GH100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    
    if (!gpuIsCCMultiGpuProtectedPcieModeEnabled(pGpu))
        return NV_OK;
    
    NV_ASSERT_OR_RETURN(
        pMemoryManager->unprotectedVidmemSize <= pMemoryManager->Ram.fbUsableMemSize,
            NV_ERR_INVALID_ARGUMENT);

    if (pMemoryManager->unprotectedVidmemSize > 0)
    {
        FB_REGION_DESCRIPTOR fbRegion;
        
        portMemSet(&fbRegion, 0, sizeof(fbRegion));

        fbRegion.base = 0;
        fbRegion.limit = fbRegion.base + 
             NV_ALIGN_UP64(pMemoryManager->unprotectedVidmemSize, 
                 NV2080_CTRL_GPU_VMMU_SEGMENT_SIZE_512MB) - 1;
        fbRegion.bRsvdRegion = NV_FALSE;
        fbRegion.bInternalHeap = NV_FALSE;
        fbRegion.bProtected = NV_FALSE;
        
        memmgrInsertFbRegion(pGpu, pMemoryManager, &fbRegion, NULL);
        
        NV_PRINTF(LEVEL_INFO, "Non CPR carveout Start: 0x%llx End: 0x%llx\n",
            fbRegion.base, fbRegion.limit);
    }
              
    return NV_OK;
}

NV_STATUS
memmgrCommitUnprotectedVidmemSegments_GH100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NvU32 i;
    NvU32 numUnprotectedRegions = 0;
    
    if (!gpuIsCCMultiGpuProtectedPcieModeEnabled(pGpu) || 
        pMemoryManager->unprotectedVidmemSize == 0)
    {
        return NV_OK;
    }
    
    for (i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
    {
        FB_REGION_DESCRIPTOR *pFbRegion = &pMemoryManager->Ram.fbRegion[i];
        if (pFbRegion->bProtected == NV_FALSE)
        {
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
            NV2080_CTRL_FB_PROGRAM_SEGMENT_PROTECTION_BITMASK_PARAMS params;
            
            portMemSet(&params, 0, sizeof(params));
            
            params.startAddress = pFbRegion->base;
            params.length = pFbRegion->limit - pFbRegion->base + 1;
            params.bMarkUnprotected = NV_TRUE;
             
            NV_ASSERT_OK_OR_RETURN(
                pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                NV2080_CTRL_CMD_FB_PROGRAM_SEGMENT_PROTECTION_BITMASK,
                                &params, sizeof(params)));
            numUnprotectedRegions++;
        }
    }
    
    // Don't expect more than 1 unprotected region
    NV_ASSERT_OR_RETURN(numUnprotectedRegions == 1, NV_ERR_INVALID_ARGUMENT);
    
    return NV_OK;
}
