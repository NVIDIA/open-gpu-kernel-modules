/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "mem_mgr_internal.h"
#include "mem_mgr/syncpoint_mem.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "os/os.h"
#include "rmapi/client.h"

#include "class/cl00c3.h" // NV01_MEMORY_SYNCPOINT

NV_STATUS
syncpointConstruct_IMPL
(
    SyncpointMemory              *pSyncpointMemory,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pRmAllocParams
)
{
    Memory            *pMemory = staticCast(pSyncpointMemory, Memory);
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    NvP64 physAddr = NvP64_NULL;
    NvU64 syncPointBase = 0;
    NvU64 limit = 0;
    NvU32 offset = 0;
    NV_MEMORY_SYNCPOINT_ALLOCATION_PARAMS *pAllocParams = pRmAllocParams->pAllocParams;
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = pMemory->pGpu;

    // OS get sync-point aperture address.
    status = osGetSyncpointAperture(pGpu->pOsGpuInfo, pAllocParams->syncpointId, &syncPointBase, &limit, &offset);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to get syncpoint aperture %x\n", status);
        return status;
    }

    physAddr = (NvP64)(syncPointBase + offset);

    NvU32 os02Flags =
        DRF_DEF(OS02, _FLAGS, _MAPPING, _NO_MAP)
        | DRF_DEF(OS02, _FLAGS, _PHYSICALITY, _CONTIGUOUS)
        | DRF_DEF(OS02, _FLAGS, _ALLOC_TYPE_SYNCPOINT, _APERTURE)
        | DRF_DEF(OS02, _FLAGS, _ALLOC_NISO_DISPLAY, _YES);

    status = osCreateMemFromOsDescriptor(pGpu,
                                         physAddr,
                                         pCallContext->pClient->hClient,
                                         os02Flags,
                                         &limit,
                                         &pMemDesc,
                                         NVOS32_DESCRIPTOR_TYPE_OS_PHYS_ADDR,
                                         RS_PRIV_LEVEL_KERNEL); // Physical address is obtained using osGetSyncpointAperture, Overriding the privlevel here to KERNEL.

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to import syncpoint memory %x\n", status);
        return status;
    }

    status = memConstructCommon(pMemory,
                                NV01_MEMORY_SYNCPOINT,
                                0,  // pUserParams->flags
                                pMemDesc,
                                0,
                                NULL,
                                0, // pUserParams->attr
                                0, // pUserParams->attr2
                                0,
                                0,
                                NVOS32_MEM_TAG_NONE,
                                NULL);


    // failure case
    if (status != NV_OK)
    {
        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);
    }

    return status;
}

NvBool
syncpointCanCopy_IMPL
(
    SyncpointMemory *pSyncpointMemory
)
{
    return NV_TRUE;
}
