/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/mem_desc.h"

/*
 * This function returns attributes associated with the memory object
 * at the given offset. 
 */
NV_STATUS
memmgrGetSurfacePhysAttr_GM107
(
    OBJGPU           *pGpu,
    MemoryManager    *pMemoryManager,
    Memory           *pMemory,
    NvU64            *pOffset,
    NvU32            *pMemAperture,
    NvU32            *pMemKind,
    NvU32            *pComprOffset,
    NvU32            *pComprKind,
    NvU32            *pLineMin,
    NvU32            *pLineMax,
    NvU32            *pZCullId,
    NvU32            *pGpuCacheAttr,
    NvU32            *pGpuP2PCacheAttr,
    NvU64            *contigSegmentSize
)
{
    NV_STATUS                   rmStatus;
    PMEMORY_DESCRIPTOR          pMemDesc      = memdescGetMemDescFromGpu(pMemory->pMemDesc, pGpu);
    COMPR_INFO                  comprInfo;
    NvU32                       unused;

    NV_ASSERT(pMemDesc);

   rmStatus = memdescFillMemdescForPhysAttr(pMemDesc, AT_GPU, pOffset, pMemAperture,
                                            pMemKind, pZCullId, pGpuCacheAttr, pGpuP2PCacheAttr,
                                            contigSegmentSize);
    if (NV_OK != rmStatus)
    {
        return rmStatus;
    }

    if ((!memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, *pMemKind)) ||
         !FB_HWRESID_CTAGID_VAL_FERMI(memdescGetHwResId(pMemDesc)))
    {
        *pComprKind = 0;
        return NV_OK;
    }

    // vGPU: pPrivate->pCompTags is not
    // currently initialized in the guest RM
    // vGPU does not use compression tags yet.
    // GSPTODO: sort out ctags
    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        *pComprOffset = 0x0;
        *pLineMin = 0x0;
        *pLineMax = 0x0;
        return NV_OK;
    }

    rmStatus = memmgrGetKindComprFromMemDesc(pMemoryManager, pMemDesc, 0, &unused, &comprInfo);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "dmaGetKidnCompr failed: %x\n", rmStatus);
        return rmStatus;
    }

    if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, comprInfo.kind))
    {
        *pLineMin = comprInfo.compTagLineMin;
        *pLineMax = comprInfo.compPageIndexHi - comprInfo.compPageIndexLo + comprInfo.compTagLineMin;
        *pComprOffset = comprInfo.compPageIndexLo;
        *pComprKind = 1;
    }
    else
    {
        // No coverage at all (stripped by release/reacquire or invalid hw res).
        *pLineMin = ~0;
        *pLineMax = ~0;
        *pComprKind = 0;
    }
        return NV_OK;
}

