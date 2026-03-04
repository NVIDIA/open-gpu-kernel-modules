/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
*
*   Description:
*       This module contains Nv04Control support for heap allocations
*       represented by NV04_MEMORY class instantiations.
*
******************************************************************************/

#include "core/core.h"
#include "os/os.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "platform/platform.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "platform/sli/sli.h"

#include "gpu/mem_mgr/mem_desc.h"
#include "rmapi/client_resource.h"
#include "rmapi/control.h"
#include "gpu_mgr/gpu_mgr.h"
#include "rmapi/rs_utils.h"
#include "gpu/subdevice/subdevice.h"
#include "vgpu/rpc.h"

#include "ctrl/ctrl0041.h"

static NV_STATUS
_memmgrGetSurfaceComprInfo
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32             *pComprOffset,
    NvU32             *pComprKind,
    NvU32             *pLineMin,
    NvU32             *pLineMax
)
{
    OBJGPU *pGpu = pMemDesc->pGpu;

    if (pGpu == NULL || IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        //
        // vGPU-legacy: pPrivate->pCompTags is not initialized in the guest RM
        // SRIOV:       same handling as raw mode
        // GPU=NULL:    devicelless sysmem, can't have comptags
        //
        *pComprKind = 0;
        *pComprOffset = 0;
        *pLineMin = 0;
        *pLineMax = 0;
        return NV_OK;
    }

    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    COMPR_INFO comprInfo;
    NvU32 unused;

    NV_ASSERT_OK_OR_RETURN(memmgrGetKindComprFromMemDesc(pMemoryManager, pMemDesc, 0, &unused, &comprInfo));

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
        *pComprKind = 0;
        *pComprOffset = 0;
        *pLineMin = ~0;
        *pLineMax = ~0;
    }

        return NV_OK;
}

NV_STATUS
memCtrlCmdGetSurfaceCompressionCoverageLvm_IMPL
(
    Memory *pMemory,
    NV0041_CTRL_GET_SURFACE_COMPRESSION_COVERAGE_PARAMS *pParams
)
{
    MEMORY_DESCRIPTOR *pMemDesc = pMemory->pMemDesc;
    NvU32 unused;

    if (pParams->hSubDevice)
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        Subdevice *pSubDevice;
        NV_STATUS status;

        // Alloc operation in unicast mode
        status = subdeviceGetByHandle(pCallContext->pClient,
                pParams->hSubDevice, &pSubDevice);

        if (status != NV_OK)
            return status;

        GPU_RES_SET_THREAD_BC_STATE(pSubDevice);

        pMemDesc = memdescGetMemDescFromGpu(pMemDesc, GPU_RES_GET_GPU(pSubDevice));
    }

    return _memmgrGetSurfaceComprInfo(pMemory->pMemDesc, &unused, &pParams->format, &pParams->lineMin, &pParams->lineMax);
}

NV_STATUS
memCtrlCmdGetSurfaceInfoLvm_IMPL
(
    Memory *pMemory,
    NV0041_CTRL_GET_SURFACE_INFO_PARAMS *pSurfaceInfoParams
)
{
    NV0041_CTRL_SURFACE_INFO *pSurfaceInfos = NvP64_VALUE(pSurfaceInfoParams->surfaceInfoList);
    NV_STATUS           status  = NV_OK;
    NvU32               i;
    NvU32               data    = 0;
    NvU64               size    = 0;

    if ((pSurfaceInfoParams->surfaceInfoListSize == 0) || pSurfaceInfos == NULL)
        return NV_OK;

    // step thru the list
    for (i = 0; i < pSurfaceInfoParams->surfaceInfoListSize; i++)
    {
        status = NV_OK;
        data = 0;

        switch (pSurfaceInfos[i].index)
        {
            case NV0041_CTRL_SURFACE_INFO_INDEX_ATTRS:
            {
                if ((pMemory->pHwResource != NULL) &&
                     pMemory->pHwResource->attr & DRF_DEF(OS32, _ATTR, _COMPR, _REQUIRED))
                    data |= NV0041_CTRL_SURFACE_INFO_ATTRS_COMPR;
                if ((pMemory->pHwResource != NULL) &&
                     pMemory->pHwResource->attr & DRF_DEF(OS32, _ATTR, _ZCULL, _REQUIRED))
                    data |= NV0041_CTRL_SURFACE_INFO_ATTRS_ZCULL;
                break;
            }
            case NV0041_CTRL_SURFACE_INFO_INDEX_COMPR_COVERAGE:
            {
                //
                // adding check for pHwResource, since host managed HW resource
                // gets allocated only when ATTR is set to COMPR_REQUIRED
                //
                if ((pMemory->pHwResource != NULL) &&
                     pMemory->pHwResource->attr &
                    DRF_DEF(OS32, _ATTR, _COMPR, _REQUIRED))
                {
                    NvU64 contigSegmentSize;
                    NvU32 unused;
                    NvU64 zero = 0;

                    status = memdescFillMemdescForPhysAttr(pMemory->pMemDesc, AT_GPU, &zero, &unused,
                                                           &unused, &unused, &unused,
                                                           &contigSegmentSize);
                    if (status == NV_OK)
                    {
                        // report compression coverage in units of 64k
                        data = NvOffset_LO32(contigSegmentSize / 0x10000);
                    }
                }
                break;
            }
            case NV0041_CTRL_SURFACE_INFO_INDEX_PHYS_SIZE_LO:
            {
                // Report the low 32 bits of the size of the physical allocation
                size = memdescGetSize(pMemory->pMemDesc);
                data = NvU64_LO32(size);
                break;
            }
            case NV0041_CTRL_SURFACE_INFO_INDEX_PHYS_SIZE_HI:
            {
                // Report the high 32 bits of the size of the physical allocation
                size = memdescGetSize(pMemory->pMemDesc);
                data = NvU64_HI32(size);
                break;
            }
            case NV0041_CTRL_SURFACE_INFO_INDEX_PHYS_SIZE:
            {
                // Report the scaled size of the physical allocation
                size = (memdescGetSize(pMemory->pMemDesc) / NV0041_CTRL_SURFACE_INFO_PHYS_SIZE_SCALE_FACTOR);
                data = NvOffset_LO32(size);
                NV_ASSERT_OR_RETURN((NvU64)data == size, NV_ERR_OUT_OF_RANGE);
                break;
            }
            case NV0041_CTRL_SURFACE_INFO_INDEX_PHYS_ATTR:
            {
                if (pMemory->pHwResource != NULL)
                    data = pMemory->pHwResource->attr & (DRF_SHIFTMASK(NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_PAGE_SIZE) | DRF_SHIFTMASK(NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_CPU_COHERENCY) | DRF_SHIFTMASK(NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_FORMAT));
                break;
            }
            case NV0041_CTRL_SURFACE_INFO_INDEX_ADDR_SPACE_TYPE:
            {
                // This is equivalent to NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE on the surface.
                NV_ADDRESS_SPACE    addrSpace;

                addrSpace = memdescGetAddressSpace(pMemory->pMemDesc);
                if (addrSpace == ADDR_SYSMEM)
                {
                    if (memdescGetFlag(pMemory->pMemDesc, MEMDESC_FLAGS_BAR0_REFLECT))
                    {
                        addrSpace = ADDR_REGMEM;
                    }
                    else if (memdescGetFlag(pMemory->pMemDesc, MEMDESC_FLAGS_BAR1_REFLECT))
                    {
                        addrSpace = ADDR_FBMEM;
                    }
                }
                switch (addrSpace)
                {
                    case ADDR_SYSMEM:
                    {
                        data = NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_SYSMEM;
                        break;
                    }
                    case ADDR_FBMEM:
                    {
                        data = NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_VIDMEM;
                        break;
                    }
                    case ADDR_REGMEM:
                    {
                        data = NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_REGMEM;
                        break;
                    }
                    default:
                    {
                        data = NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_INVALID;
                        break;
                    }
                }
                break;
            }
            default:
            {
                status = NV_ERR_INVALID_ARGUMENT;
                break;
            }
        }

        // stop processing list at first failure
        if (status != NV_OK)
            break;

        pSurfaceInfos[i].data = data;
    }

    return status;
}

NV_STATUS
memCtrlCmdGetSurfacePhysAttrLvm_IMPL
(
    Memory *pMemory,
    NV0041_CTRL_GET_SURFACE_PHYS_ATTR_PARAMS *pGPAP
)
{
    NvU32 unused;

    NV_ASSERT_OK_OR_RETURN(
        memdescFillMemdescForPhysAttr(pMemory->pMemDesc, AT_GPU, &pGPAP->memOffset, &pGPAP->memAperture,
                                      &pGPAP->memFormat, &pGPAP->gpuCacheAttr, &pGPAP->gpuP2PCacheAttr,
                                      &pGPAP->contigSegmentSize));

    NV_ASSERT_OK_OR_RETURN(
        _memmgrGetSurfaceComprInfo(pMemory->pMemDesc, &pGPAP->comprOffset, &pGPAP->comprFormat, &unused, &unused));

    return NV_OK;
}


NV_STATUS
memCtrlCmdSurfaceFlushGpuCache_IMPL
(
    Memory *pMemory,
    NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_PARAMS *pCacheFlushParams
)
{
    OBJGPU *pGpu = pMemory->pGpu;
    NV_STATUS status = NV_OK;
    FB_CACHE_OP cacheOp = FB_CACHE_OP_UNDEFINED;
    FB_CACHE_MEMTYPE memType = FB_CACHE_MEM_UNDEFINED;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGpu != NULL, NV_ERR_NOT_SUPPORTED);

    NV_PRINTF(LEVEL_INFO, "\n");

    if (FLD_TEST_DRF(0041, _CTRL_SURFACE_FLUSH_GPU_CACHE_FLAGS, _WRITE_BACK,
                _YES, pCacheFlushParams->flags) &&
            FLD_TEST_DRF(0041, _CTRL_SURFACE_FLUSH_GPU_CACHE_FLAGS, _INVALIDATE,
                _YES, pCacheFlushParams->flags))
    {
        cacheOp = FB_CACHE_EVICT;
    }
    else if (FLD_TEST_DRF(0041, _CTRL_SURFACE_FLUSH_GPU_CACHE_FLAGS, _WRITE_BACK,
                _NO, pCacheFlushParams->flags) &&
            FLD_TEST_DRF(0041, _CTRL_SURFACE_FLUSH_GPU_CACHE_FLAGS, _INVALIDATE,
                _YES, pCacheFlushParams->flags))
    {
        cacheOp = FB_CACHE_INVALIDATE;
    }
    else if (FLD_TEST_DRF(0041, _CTRL_SURFACE_FLUSH_GPU_CACHE_FLAGS, _WRITE_BACK,
                _YES, pCacheFlushParams->flags) &&
            FLD_TEST_DRF(0041, _CTRL_SURFACE_FLUSH_GPU_CACHE_FLAGS, _INVALIDATE,
                _NO, pCacheFlushParams->flags))
    {
        cacheOp = FB_CACHE_WRITEBACK;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Must specify at least one of WRITE_BACK or INVALIDATE\n");
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!pMemory->pMemDesc)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Memory descriptor not found for hMemory 0x%x, unable to flush!\n",
                  RES_GET_HANDLE(pMemory));
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_STATE;
    }

    if (memdescGetGpuCacheAttrib(pMemory->pMemDesc) != NV_MEMORY_CACHED)
    {
        NV_PRINTF(LEVEL_ERROR, "Cannot flush an uncached allocation\n");
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_ARGUMENT;
    }

    switch (memdescGetAddressSpace(pMemory->pMemDesc))
    {
        case ADDR_FBMEM:
            memType = FB_CACHE_VIDEO_MEMORY;
            break;
        case ADDR_SYSMEM:
            memType = FB_CACHE_SYSTEM_MEMORY;
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Cannot flush address space 0x%x\n",
                      memdescGetAddressSpace(pMemory->pMemDesc));
            DBG_BREAKPOINT();
            return NV_ERR_INVALID_ARGUMENT;
            break;
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
        status = kmemsysCacheOp_HAL(pGpu, pKernelMemorySystem, pMemory->pMemDesc, memType, cacheOp);
        if (status != NV_OK)
        {
            SLI_LOOP_BREAK;
        }
    }
    SLI_LOOP_END

    return status;
}

NV_STATUS
memCtrlCmdGetMemPageSize_IMPL
(
    Memory *pMemory,
    NV0041_CTRL_GET_MEM_PAGE_SIZE_PARAMS *pPageSizeParams
)
{
    pPageSizeParams->pageSize = memdescGetPageSize(pMemory->pMemDesc, AT_GPU);

    return NV_OK;
}

NV_STATUS
memCtrlCmdUpdateSurfaceCompression_IMPL
(
    Memory *pMemory,
    NV0041_CTRL_UPDATE_SURFACE_COMPRESSION_PARAMS *pUpdateParams
)
{
    OBJGPU     *pGpu = pMemory->pGpu;
    NV_STATUS   status = NV_OK;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGpu != NULL, NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(pMemory->pHwResource, NV_ERR_INVALID_ARGUMENT);

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

        NV_STATUS tmpStatus = memmgrUpdateSurfaceCompression_HAL(pGpu, pMemoryManager, pMemory, pUpdateParams->bRelease);
        if (tmpStatus != NV_OK)
        {
            status = tmpStatus;
        }
    }
    SLI_LOOP_END

    return status;
}

NV_STATUS
memCtrlCmdSetTag_IMPL
(
    Memory *pMemory,
    NV0041_CTRL_CMD_SET_TAG_PARAMS *pParams
)
{
    pMemory->tag = pParams->tag;

    return NV_OK;
}

NV_STATUS
memCtrlCmdGetTag_IMPL
(
    Memory *pMemory,
    NV0041_CTRL_CMD_GET_TAG_PARAMS *pParams
)
{
    pParams->tag = pMemory->tag;

    return NV_OK;
}

NV_STATUS
memCtrlCmdMapMemoryForGpuAccess_IMPL
(
    Memory *pMemory,
    NV0041_CTRL_MAP_MEMORY_FOR_GPU_ACCESS_PARAMS *pParams
)
{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    MEMORY_DESCRIPTOR *pMemDesc = pMemory->pMemDesc;
    Subdevice *pSubdevice;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, subdeviceGetByHandle(pCallContext->pClient, pParams->hSubdevice, &pSubdevice));
    NV_ASSERT_OK_OR_RETURN(memdescMapIommu(pMemDesc, GPU_RES_GET_GPU(pSubdevice)->busInfo.iovaspaceId));

    memdescGetPhysAddrsForGpu(pMemDesc, GPU_RES_GET_GPU(pSubdevice), AT_GPU, 0, 0, 1, &pParams->address);

    return NV_OK;
}

NV_STATUS
memCtrlCmdUnmapMemoryForGpuAccess_IMPL
(
    Memory *pMemory,
    NV0041_CTRL_UNMAP_MEMORY_FOR_GPU_ACCESS_PARAMS *pParams
)
{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    MEMORY_DESCRIPTOR *pMemDesc = pMemory->pMemDesc;
    Subdevice *pSubdevice;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, subdeviceGetByHandle(pCallContext->pClient, pParams->hSubdevice, &pSubdevice));
    memdescUnmapIommu(pMemDesc, GPU_RES_GET_GPU(pSubdevice)->busInfo.iovaspaceId);

    return NV_OK;
}
