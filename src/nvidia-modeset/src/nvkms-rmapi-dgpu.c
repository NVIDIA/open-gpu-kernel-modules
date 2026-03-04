/*
 * SPDX-FileCopyrightText: Copyright (c) 2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nv-kernel-rmapi-ops.h"
#include "nvidia-modeset-os-interface.h"

#include "nvkms-rmapi.h"
#include "nv_assert.h"

NvU32 nvRmApiAlloc(
    NvU32 hClient,
    NvU32 hParent,
    NvU32 hObject,
    NvU32 hClass,
    void *pAllocParams)
{
    nvidia_kernel_rmapi_ops_t ops = { 0 };

    ops.op = NV04_ALLOC;

    ops.params.alloc.hRoot         = hClient;
    ops.params.alloc.hObjectParent = hParent;
    ops.params.alloc.hObjectNew    = hObject;
    ops.params.alloc.hClass        = hClass;
    ops.params.alloc.pAllocParms   = NV_PTR_TO_NvP64(pAllocParams);

    nvkms_call_rm(&ops);

    return ops.params.alloc.status;
}

NvU32 nvRmApiAllocMemory64(
    NvU32 hClient,
    NvU32 hParent,
    NvU32 hMemory,
    NvU32 hClass,
    NvU32 flags,
    void **ppAddress,
    NvU64 *pLimit)
{
    nvidia_kernel_rmapi_ops_t ops = { 0 };

    ops.op = NV01_ALLOC_MEMORY;

    ops.params.allocMemory64.hRoot         = hClient;
    ops.params.allocMemory64.hObjectParent = hParent;
    ops.params.allocMemory64.hObjectNew    = hMemory;
    ops.params.allocMemory64.hClass        = hClass;
    ops.params.allocMemory64.flags         = flags;
    ops.params.allocMemory64.pMemory       = NV_PTR_TO_NvP64(*ppAddress);
    ops.params.allocMemory64.limit         = *pLimit;

    nvkms_call_rm(&ops);

    *pLimit    = ops.params.allocMemory64.limit;
    *ppAddress = NvP64_VALUE(ops.params.allocMemory64.pMemory);

    return ops.params.allocMemory64.status;
}

NvU32 nvRmApiControl(
    NvU32 hClient,
    NvU32 hObject,
    NvU32 cmd,
    void *pParams,
    NvU32 paramsSize)
{
    nvidia_kernel_rmapi_ops_t ops = { 0 };

    ops.op = NV04_CONTROL;

    ops.params.control.hClient    = hClient;
    ops.params.control.hObject    = hObject;
    ops.params.control.cmd        = cmd;
    ops.params.control.params     = NV_PTR_TO_NvP64(pParams);
    ops.params.control.paramsSize = paramsSize;

    nvkms_call_rm(&ops);

    return ops.params.control.status;
}

NvU32 nvRmApiDupObject2(
    NvU32 hClient,
    NvU32 hParent,
    NvU32 *hObjectDest,
    NvU32 hClientSrc,
    NvU32 hObjectSrc,
    NvU32 flags)
{
    nvidia_kernel_rmapi_ops_t ops = { 0 };

    ops.op = NV04_DUP_OBJECT;

    ops.params.dupObject.hClient    = hClient;
    ops.params.dupObject.hParent    = hParent;
    ops.params.dupObject.hObject    = *hObjectDest;
    ops.params.dupObject.hClientSrc = hClientSrc;
    ops.params.dupObject.hObjectSrc = hObjectSrc;
    ops.params.dupObject.flags      = flags;

    nvkms_call_rm(&ops);

    *hObjectDest = ops.params.dupObject.hObject;

    return ops.params.dupObject.status;
}

NvU32 nvRmApiDupObject(
    NvU32 hClient,
    NvU32 hParent,
    NvU32 hObjectDest,
    NvU32 hClientSrc,
    NvU32 hObjectSrc,
    NvU32 flags)
{
    NvU32 hObjectLocal = hObjectDest;
    NvU32 ret = nvRmApiDupObject2(hClient,
                                  hParent,
                                  &hObjectLocal,
                                  hClientSrc,
                                  hObjectSrc,
                                  flags);

    nvAssert(hObjectLocal == hObjectDest);

    return ret;
}

NvU32 nvRmApiFree(
    NvU32 hClient,
    NvU32 hParent,
    NvU32 hObject)
{
    nvidia_kernel_rmapi_ops_t ops = { 0 };

    ops.op = NV01_FREE;

    ops.params.free.hRoot         = hClient;
    ops.params.free.hObjectParent = hParent;
    ops.params.free.hObjectOld    = hObject;

    nvkms_call_rm(&ops);

    return ops.params.free.status;
}

NvU32 nvRmApiVidHeapControl(
    void *pVidHeapControlParams)
{
    nvidia_kernel_rmapi_ops_t ops = { 0 };
    NVOS32_PARAMETERS *pParams = pVidHeapControlParams;

    ops.op = NV04_VID_HEAP_CONTROL;

    ops.params.pVidHeapControl = pParams;

    nvkms_call_rm(&ops);

    return pParams->status;
}

NvU32 nvRmApiMapMemory(
    NvU32 hClient,
    NvU32 hDevice,
    NvU32 hMemory,
    NvU64 offset,
    NvU64 length,
    void **ppLinearAddress,
    NvU32 flags)
{
    nvidia_kernel_rmapi_ops_t ops = { 0 };

    ops.op = NV04_MAP_MEMORY;

    ops.params.mapMemory.hClient = hClient;
    ops.params.mapMemory.hDevice = hDevice;
    ops.params.mapMemory.hMemory = hMemory;
    ops.params.mapMemory.offset  = offset;
    ops.params.mapMemory.length  = length;
    ops.params.mapMemory.flags   = flags;

    nvkms_call_rm(&ops);

    *ppLinearAddress = NvP64_VALUE(ops.params.mapMemory.pLinearAddress);

    return ops.params.mapMemory.status;
}

NvU32 nvRmApiUnmapMemory(
    NvU32 hClient,
    NvU32 hDevice,
    NvU32 hMemory,
    const void *pLinearAddress,
    NvU32 flags)
{
    nvidia_kernel_rmapi_ops_t ops = { 0 };

    ops.op = NV04_UNMAP_MEMORY;

    ops.params.unmapMemory.hClient        = hClient;
    ops.params.unmapMemory.hDevice        = hDevice;
    ops.params.unmapMemory.hMemory        = hMemory;
    ops.params.unmapMemory.pLinearAddress = NV_PTR_TO_NvP64(pLinearAddress);
    ops.params.unmapMemory.flags          = flags;

    nvkms_call_rm(&ops);

    return ops.params.unmapMemory.status;
}

NvU32 nvRmApiMapMemoryDma(
    NvU32 hClient,
    NvU32 hDevice,
    NvU32 hDma,
    NvU32 hMemory,
    NvU64 offset,
    NvU64 length,
    NvU32 flags,
    NvU64 *pDmaOffset)
{
    nvidia_kernel_rmapi_ops_t ops = { 0 };

    ops.op = NV04_MAP_MEMORY_DMA;

    ops.params.mapMemoryDma.hClient   = hClient;
    ops.params.mapMemoryDma.hDevice   = hDevice;
    ops.params.mapMemoryDma.hDma      = hDma;
    ops.params.mapMemoryDma.hMemory   = hMemory;
    ops.params.mapMemoryDma.offset    = offset;
    ops.params.mapMemoryDma.length    = length;
    ops.params.mapMemoryDma.flags     = flags;
    ops.params.mapMemoryDma.dmaOffset = *pDmaOffset;

    nvkms_call_rm(&ops);

    *pDmaOffset = ops.params.mapMemoryDma.dmaOffset;

    return ops.params.mapMemoryDma.status;
}

NvU32 nvRmApiUnmapMemoryDma(
    NvU32 hClient,
    NvU32 hDevice,
    NvU32 hDma,
    NvU32 hMemory,
    NvU32 flags,
    NvU64 dmaOffset)
{
    nvidia_kernel_rmapi_ops_t ops = { 0 };

    ops.op = NV04_UNMAP_MEMORY_DMA;

    ops.params.unmapMemoryDma.hClient   = hClient;
    ops.params.unmapMemoryDma.hDevice   = hDevice;
    ops.params.unmapMemoryDma.hDma      = hDma;
    ops.params.unmapMemoryDma.hMemory   = hMemory;
    ops.params.unmapMemoryDma.flags     = flags;
    ops.params.unmapMemoryDma.dmaOffset = dmaOffset;

    nvkms_call_rm(&ops);

    return ops.params.unmapMemoryDma.status;
}
