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

#include "core/system.h"
#include "core/locks.h"
#include "os/os.h"
#include "gpu/device/device.h"
#include "rmapi/control.h"
#include "vgpu/rpc.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "vgpu/vgpu_events.h"

#include "vgpu/vgpuapi.h"

NV_STATUS
vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties_IMPL
(
    VgpuApi *pVgpuApi,
    NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES *pParams
)
{
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pVgpuApi);
    NV_STATUS rmStatus = NV_OK;

    //
    // For RM-managed heaps, look up the memory object from the passed-in
    // hMemory and overwrite the offset and surfaceKind fields.  For
    // externally-managed heaps, we just pass through these parameters from the
    // client directly.
    //
    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_EXTERNAL_HEAP_CONTROL))
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
        Memory *pMemory = NULL;

        NvU64   physOffset = 0;
        NvU32   pageKind = 0;
        // unused
        NvU32   _memAperture, _comprOffset, _comprFormat;
        NvU32   _lineMin, _lineMax, _zcullId, _gpuCacheAttr, _gpuP2PCacheAttr;
        NvU64   _contigSegmentSize;

        // Find the allocation from the hMemory
        rmStatus = memGetByHandle(RES_GET_CLIENT(pVgpuApi), pParams->hMemory, &pMemory);
        if (rmStatus != NV_OK)
            return rmStatus;

        // Verify the allocation is on the same GPU as this VGPU object
        if (GPU_RES_GET_GPU(pMemory->pDevice) != pGpu)
            return NV_ERR_INVALID_OBJECT_PARENT;

        // Verify the allocation is in video memory.
        if (memdescGetAddressSpace(pMemory->pMemDesc) != ADDR_FBMEM)
        {
            return NV_ERR_INVALID_OBJECT;
        }

        // Look up the page kind.
        rmStatus = memmgrGetSurfacePhysAttr_HAL(pGpu, pMemoryManager, pMemory,
                                                &physOffset, &_memAperture, &pageKind,
                                                &_comprOffset, &_comprFormat, &_lineMin, &_lineMax,
                                                &_zcullId, &_gpuCacheAttr, &_gpuP2PCacheAttr,
                                                &_contigSegmentSize);
        if (rmStatus != NV_OK)
            return rmStatus;

        physOffset += pParams->offset;
        if (physOffset > 0xffffffffull)
            return NV_ERR_INVALID_OFFSET;

        pParams->offset = NvU64_LO32(physOffset);
        pParams->surfaceKind = pageKind;
    }

    NV_RM_RPC_SET_SURFACE_PROPERTIES(pGpu, RES_GET_CLIENT_HANDLE(pVgpuApi),
                                     pParams, NV_FALSE, rmStatus);

    return rmStatus;
}

NV_STATUS
vgpuapiCtrlCmdVgpuDisplayCleanupSurface_IMPL
(
    VgpuApi *pVgpuApi,
    NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pVgpuApi);
    NV_STATUS rmStatus = NV_OK;

    NV_RM_RPC_CLEANUP_SURFACE(pGpu, pParams, rmStatus);

    return rmStatus;
}

NV_STATUS
vgpuapiCtrlCmdVGpuGetConfig_IMPL
(
    VgpuApi *pVgpuApi,
    NVA080_CTRL_VGPU_GET_CONFIG_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pVgpuApi);
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
    NV_STATUS rmStatus = NV_OK;

    if (pVSI != NULL)
        portMemCopy(pParams, sizeof(pVSI->vgpuConfig), &pVSI->vgpuConfig, sizeof(pVSI->vgpuConfig));

    return rmStatus;
}

