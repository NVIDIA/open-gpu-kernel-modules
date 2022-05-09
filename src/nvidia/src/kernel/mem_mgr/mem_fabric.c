/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *       This file contains the functions managing the memory fabric
 *
 *****************************************************************************/

#include "core/core.h"
#include "core/locks.h"
#include "rmapi/resource.h"
#include "rmapi/rs_utils.h"
#include "mem_mgr_internal.h"
#include "mem_mgr/mem_fabric.h"
#include "mem_mgr/fabric_vaspace.h"
#include "mem_mgr/mem.h"
#include "mem_mgr/vaspace.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/mem_utils.h"
#include "gpu/mem_mgr/heap.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "os/os.h"
#include "compute/fabric.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/gpu.h"
#include "class/cl00f8.h"
#include "Nvcm.h"
#include "vgpu/rpc.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/bus/p2p_api.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"

#include "published/ampere/ga100/dev_mmu.h"

typedef struct
{
    //
    // TODO: Only sticky non-partial mappings are supported currently, so all
    // the fabric addrs are mapped to the single vidmem memory object. However,
    // when partial mappings are supported, we will need a per-fabric memdesc
    // tree to track the mappings for multiple vidmem memory objects.
    //
    NvHandle hDupedVidmem;
} FABRIC_MEMDESC_DATA;

static NvU32
_memoryfabricMemDescGetNumAddr
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    OBJGPU *pGpu     = pMemDesc->pGpu;
    NvU32   pageSize = 0;

    // Get the page size from the memory descriptor.
    pageSize = memdescGetPageSize(pMemDesc,
                            VAS_ADDRESS_TRANSLATION(pGpu->pFabricVAS));

    // Get the number of addresses associated with this memory descriptor.
    if (memdescGetContiguity(pMemDesc,
                            VAS_ADDRESS_TRANSLATION(pGpu->pFabricVAS)))
    {
        // For contiguous allocation, there is just one entry _pteArray[0].
        return 1;
    }

    // For discontiguous allocations, numAddr is total size / page size.
    return (memdescGetSize(pMemDesc) / pageSize);
}

static void
_memoryfabricMemDescDestroyCallback
(
    OBJGPU            *pGpu,
    void              *pObject,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    RM_API *pRmApi   = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    FABRIC_VASPACE *pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);

    FABRIC_MEMDESC_DATA *pMemdescData =
                            (FABRIC_MEMDESC_DATA *)memdescGetMemData(pMemDesc);

    RmPhysAddr *pteArray = memdescGetPteArrayForGpu(pMemDesc, pGpu,
                                    VAS_ADDRESS_TRANSLATION(pGpu->pFabricVAS));

    NvU32 numAddr = _memoryfabricMemDescGetNumAddr(pMemDesc);

    // Get the page size from the memory descriptor.
    NvU32 pageSize = memdescGetPageSize(pMemDesc,
                                    VAS_ADDRESS_TRANSLATION(pGpu->pFabricVAS));

    // Remove the fabric memory allocations from the map.
    fabricvaspaceVaToGpaMapRemove(pFabricVAS, pteArray[0]);

    if (!pFabricVAS->bRpcAlloc)
    {
        //
        // Call fabricvaspaceBatchFree to free the FLA allocations.
        // _pteArray in memdesc is RM_PAGE_SIZE whereas page size for memory fabric
        // allocations is either 2MB or 512MB. Pass stride accordingly.
        //
        fabricvaspaceBatchFree(pFabricVAS, pteArray, numAddr, (pageSize >> RM_PAGE_SHIFT));
    }

    // Destroy the duped physical video memory handle.
    if ((pMemdescData != NULL) && (pMemdescData->hDupedVidmem != 0))
    {
        NV_ASSERT(pRmApi->Free(pRmApi, pFabricVAS->hClient,
                               pMemdescData->hDupedVidmem) == NV_OK);

        portMemFree(pMemDesc->_pMemData);
    }

    portMemFree(pObject);
}

static NV_STATUS
_memoryfabricMapPhysicalMemory
(
    NvU64             *vAddr,
    NvU32              numAddr,
    NvU64              allocSize,
    NvU32              pageSize,
    MEMORY_DESCRIPTOR *pVidMemDesc,
    NvU64              offset,
    NvBool             bReadOnly
)
{
    OBJGPU                  *pGpu           = pVidMemDesc->pGpu;
    VirtMemAllocator        *pDma           = GPU_GET_DMA(pGpu);
    MemoryManager           *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS                status         = NV_OK;
    DMA_PAGE_ARRAY           pageArray;
    NvU64                    mapLength;
    NvU32                    kind;
    COMPR_INFO               comprInfo;
    NvU32                    vidmemPteArraySize;
    RmPhysAddr               addr;
    NvU32                    i;
    FABRIC_VASPACE          *pFabricVAS;
    NvU32                    mapFlags = DMA_UPDATE_VASPACE_FLAGS_UPDATE_ALL |
                                        DMA_UPDATE_VASPACE_FLAGS_SKIP_4K_PTE_CHECK;

    NV_ASSERT_OR_RETURN(vAddr != NULL,       NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVidMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);

    mapFlags |= bReadOnly ? DMA_UPDATE_VASPACE_FLAGS_READ_ONLY : 0;

    pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);

    if (pFabricVAS->bRpcAlloc)
        return NV_OK;

    // Get compression attributes for the vidmem memdesc.
    status = memmgrGetKindComprFromMemDesc(pMemoryManager, pVidMemDesc, offset, &kind, &comprInfo);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to get the compression attributes for the vidmem memdesc\n");
        return status;
    }

    portMemSet(&pageArray, 0, sizeof(DMA_PAGE_ARRAY));

    // Get the vidmem pteArray size.
    vidmemPteArraySize = memdescGetPteArraySize(pVidMemDesc, AT_GPU);

    // Get the fabric addr range to map.
    mapLength       = (numAddr == 1 ? allocSize : pageSize);
    pageArray.count = (vidmemPteArraySize == 1 ? 1 : (mapLength / RM_PAGE_SIZE));

    for (i = 0; i < numAddr; i++)
    {
        if (pageArray.count == 1)
        {
            addr = pVidMemDesc->_pteArray[0] + offset;
            pageArray.pData = &addr;
        }
        else
        {
            pageArray.pData = &pVidMemDesc->_pteArray[offset / RM_PAGE_SIZE];
        }

        // Map the memory fabric object at the given physical video memory offset.
        status = dmaUpdateVASpace_HAL(pGpu, pDma, pFabricVAS->pGVAS, pVidMemDesc,
                                      NULL, vAddr[i], vAddr[i] + mapLength - 1,
                                      mapFlags, &pageArray, 0, &comprInfo, 0,
                                      NV_MMU_PTE_VALID_TRUE,
                                      NV_MMU_PTE_APERTURE_VIDEO_MEMORY,
                                      BUS_INVALID_PEER, NVLINK_INVALID_FABRIC_ADDR,
                                      DMA_DEFER_TLB_INVALIDATE, NV_FALSE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to map fabric addrs starting at 0x%llx\n", vAddr[i]);
            return status;
        }

        offset = offset + mapLength;
    }

    fabricvaspaceInvalidateTlb(pFabricVAS, pVidMemDesc->pGpu, PTE_UPGRADE);

    return NV_OK;
}

static void
_memoryfabricFreeFabricVa_VGPU
(
    OBJGPU                       *pGpu,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS status = NV_OK;
    NV_RM_RPC_FREE(pGpu, pParams->hClient,
                   pParams->hParent, pParams->hResource, status);
    NV_ASSERT(status == NV_OK);
}

static void
_memoryfabricFreeFabricVa
(
    FABRIC_VASPACE               *pFabricVAS,
    OBJGPU                       *pGpu,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams,
    NvU64                        *pAddr,
    NvU32                         numAddr
)
{
    if (pFabricVAS->bRpcAlloc)
    {
        _memoryfabricFreeFabricVa_VGPU(pGpu, pParams);
    }
    else
    {
        fabricvaspaceBatchFree(pFabricVAS, pAddr, numAddr, 1);
    }
}

static NV_STATUS
_memoryfabricAllocFabricVa_VGPU
(
    OBJGPU                       *pGpu,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams,
    NV00F8_ALLOCATION_PARAMETERS *pAllocParams,
    NvU64                       **ppAddr,
    NvU32                        *pNumAddr
)
{
    NV00F8_CTRL_DESCRIBE_PARAMS *pDescribeParams = NULL;
    NvU32  i = 0;
    NV_STATUS status = NV_OK;
    NvU32 idx = 0;
    NvU64 *pAddr  = NULL;

    NV_RM_RPC_ALLOC_OBJECT(pGpu, pParams->hClient,
                           pParams->hParent,
                           pParams->hResource,
                           pParams->externalClassId,
                           pAllocParams,
                           status);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Alloc NV_MEMORY_FABRIC RPC failed, status: %x\n",
                    status);
        return status;
    }

    pDescribeParams = portMemAllocNonPaged(sizeof(*pDescribeParams));
    if (pDescribeParams == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto cleanup;
    }

    portMemSet(pDescribeParams, 0, sizeof(*pDescribeParams));

    do
    {
        pDescribeParams->offset = idx;
        NV_RM_RPC_CONTROL(pGpu, pParams->hClient,
                          pParams->hResource,
                          NV00F8_CTRL_CMD_DESCRIBE,
                          pDescribeParams,
                          sizeof(*pDescribeParams), status);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "CTRL_CMD_DESCRIBE failed, status: 0x%x, "
                        "numPfns: 0x%x, totalPfns: 0x%llx, readSoFar: 0x%x \n",
                        status, pDescribeParams->numPfns, pDescribeParams->totalPfns, idx);
            goto cleanup;
        }

        if (pAddr == NULL)
        {
            pAddr = portMemAllocNonPaged(sizeof(NvU64) * pDescribeParams->totalPfns);
            if (pAddr == NULL)
            {
                status = NV_ERR_NO_MEMORY;
                goto cleanup;
            }
        }

        for (i=0; i < pDescribeParams->numPfns; i++)
        {
            pAddr[idx + i] = (NvU64)((NvU64)pDescribeParams->pfnArray[i] << RM_PAGE_SHIFT_HUGE);
        }

        idx += pDescribeParams->numPfns;
    } while (idx < pDescribeParams->totalPfns);

    portMemFree(pDescribeParams);

    *ppAddr   = pAddr;
    *pNumAddr = idx;

    return status;

cleanup:
    portMemFree(pAddr);
    portMemFree(pDescribeParams);

    _memoryfabricFreeFabricVa_VGPU(pGpu, pParams);

    return status;
}

static NV_STATUS
_memoryfabricAllocFabricVa
(
    FABRIC_VASPACE               *pFabricVAS,
    OBJGPU                       *pGpu,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams,
    NV00F8_ALLOCATION_PARAMETERS *pAllocParams,
    VAS_ALLOC_FLAGS               flags,
    NvU64                       **ppAddr,
    NvU32                        *pNumAddr
)
{
    OBJVASPACE *pOBJVASPACE = staticCast(pFabricVAS, OBJVASPACE);

    if (pFabricVAS->bRpcAlloc)
    {
        return _memoryfabricAllocFabricVa_VGPU(pGpu, pParams,
                                               pAllocParams, ppAddr,
                                               pNumAddr);
    }
    else
    {
        return fabricvaspaceAllocNonContiguous(pFabricVAS,
                                               pAllocParams->allocSize,
                                               pAllocParams->alignment,
                                               vaspaceGetVaStart(pOBJVASPACE),
                                               vaspaceGetVaLimit(pOBJVASPACE),
                                               pAllocParams->pageSize, flags,
                                               ppAddr, pNumAddr);
    }
}

NV_STATUS
memoryfabricConstruct_IMPL
(
    MemoryFabric                 *pMemoryFabric,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    Memory                       *pMemory        = staticCast(pMemoryFabric, Memory);
    OBJGPU                       *pGpu           = pMemory->pGpu;
    FABRIC_VASPACE               *pFabricVAS     = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
    NV00F8_ALLOCATION_PARAMETERS *pAllocParams   = pParams->pAllocParams;
    RM_API                       *pRmApi         = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    MemoryManager                *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    RsResourceRef                *pVidmemRef     = NULL;
    MEMORY_DESCRIPTOR            *pVidMemDesc    = NULL;
    NV_STATUS                     status         = NV_OK;
    MEMORY_DESCRIPTOR            *pMemDesc       = NULL;
    FABRIC_MEMDESC_DATA          *pMemdescData   = NULL;
    MEM_DESC_DESTROY_CALLBACK    *pCallback      = NULL;
    VAS_ALLOC_FLAGS               flags          = {0};
    NvU64                        *pAddr          = NULL;
    NvU32                         numAddr        = 0;
    NvU32                         pteKind        = 0;
    NvBool                        bReadOnly      = NV_FALSE;

    if (RS_IS_COPY_CTOR(pParams))
    {
        return memoryfabricCopyConstruct_IMPL(pMemoryFabric,
                                              pCallContext,
                                              pParams);
    }

    // Only page size 512MB and 2MB supported.
    if ((pAllocParams->pageSize != NV_MEMORY_FABRIC_PAGE_SIZE_512M) &&
        (pAllocParams->pageSize != NV_MEMORY_FABRIC_PAGE_SIZE_2M))
    {
        NV_PRINTF(LEVEL_ERROR, "Unsupported pageSize: 0x%x\n",
                  pAllocParams->pageSize);

        return NV_ERR_INVALID_ARGUMENT;
    }

    // Alignment should be pageSize aligned.
    if (!NV_IS_ALIGNED64(pAllocParams->alignment, pAllocParams->pageSize))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Alignment should be pageSize aligned\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    // AllocSize should be page size aligned.
    if (!NV_IS_ALIGNED64(pAllocParams->allocSize, pAllocParams->pageSize))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "AllocSize should be pageSize aligned\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    // We don't support flexible mappings yet.
    if (pAllocParams->allocFlags & NV00F8_ALLOC_FLAGS_FLEXIBLE_FLA)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Only sticky mappings are supported\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pAllocParams->allocFlags & NV00F8_ALLOC_FLAGS_READ_ONLY)
    {
#if !defined(DEVELOP) && !defined(DEBUG) && !defined(NV_MODS)
        NV_PRINTF(LEVEL_ERROR,
                  "RO mappings are only supported on non-release builds\n");

        return NV_ERR_NOT_SUPPORTED;
#else
        bReadOnly = NV_TRUE;
#endif
    }

    // For sticky mappings, physical video memory handle is needed.
    if (pAllocParams->map.hVidMem == 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Physical vidmem handle needed for sticky mappings\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    status = serverutilGetResourceRef(pCallContext->pClient->hClient,
                                      pAllocParams->map.hVidMem, &pVidmemRef);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to get resource in resserv for vidmem handle\n");
        return status;
    }

    pVidMemDesc = (dynamicCast(pVidmemRef->pResource, Memory))->pMemDesc;

    if ((memdescGetAddressSpace(pVidMemDesc) != ADDR_FBMEM) ||
        (pGpu != pVidMemDesc->pGpu))
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid physical vidmem handle passed\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    if (memdescGetPageSize(pVidMemDesc, AT_GPU) != NV_MEMORY_FABRIC_PAGE_SIZE_2M)
    {
        NV_PRINTF(LEVEL_ERROR, "Physical vidmem page size should be 2MB\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((pAllocParams->map.offset >= pVidMemDesc->Size) ||
        !NV_IS_ALIGNED64(pAllocParams->map.offset, NV_MEMORY_FABRIC_PAGE_SIZE_2M))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Invalid offset passed for the physical vidmem handle\n");

        return NV_ERR_INVALID_OFFSET;
    }

    // hVidmem should be big enough to cover allocSize, starting from offset.
    if (pAllocParams->allocSize >
        (memdescGetSize(pVidMemDesc) - pAllocParams->map.offset))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Insufficient physical video memory to map the requested "
                  "memory fabric allocation\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    // Check if fabric vaspace is valid.
    if (pFabricVAS == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Fabric vaspace object not available\n");

        return NV_ERR_NOT_SUPPORTED;
    }

    // Set the vaspace alloc flags.
    flags.bSkipTlbInvalidateOnFree = NV_TRUE;

    flags.bForceContig = !!(pAllocParams->allocFlags &
                            NV00F8_ALLOC_FLAGS_FORCE_CONTIGUOUS);

    flags.bForceNonContig = !!(pAllocParams->allocFlags &
                               NV00F8_ALLOC_FLAGS_FORCE_NONCONTIGUOUS);

    status = _memoryfabricAllocFabricVa(pFabricVAS, pGpu,
                                        pParams, pAllocParams,
                                        flags, &pAddr, &numAddr);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "VA Space alloc failed! Status Code: 0x%x Size: 0x%llx "
                  "RangeLo: 0x%llx, RangeHi: 0x%llx, page size: 0x%x\n",
                  status, pAllocParams->allocSize,
                  vaspaceGetVaStart(pGpu->pFabricVAS),
                  vaspaceGetVaLimit(pGpu->pFabricVAS),
                  pAllocParams->pageSize);

        return status;
    }

    // Create a memdesc to associate with the above allocation.
    status = memdescCreate(&pMemDesc, pGpu, pAllocParams->allocSize,
                           0, (numAddr == 1), ADDR_FABRIC_V2, NV_MEMORY_UNCACHED,
                           MEMDESC_FLAGS_NONE);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate memory descriptor\n");
        goto freeVaspace;
    }

    // Associate the memdesc with the above FLA allocation.
    if (numAddr == 1)
    {
        // For contiguous allocation, call memdescSetPte to set _pteArray[0].
        memdescSetPte(pMemDesc, VAS_ADDRESS_TRANSLATION(pGpu->pFabricVAS),
                      0, pAddr[0]);
    }
    else
    {
        // For discontiguous allocations, call memdescFillPages to fill ptes.
        memdescFillPages(pMemDesc, 0, pAddr, numAddr, pAllocParams->pageSize);
    }

    // Set the memdesc _pageSize.
    memdescSetPageSize(pMemDesc, VAS_ADDRESS_TRANSLATION(pGpu->pFabricVAS),
                       pAllocParams->pageSize);

    status = memConstructCommon(pMemory, NV_MEMORY_FABRIC, 0, pMemDesc, 0, NULL,
                                0, 0, 0, 0, NVOS32_MEM_TAG_NONE, NULL);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "MemoryFabric memConstructCommon failed\n");
        goto freeMemdesc;
    }

    status = memmgrGetFlaKind_HAL(pGpu, pMemoryManager, &pteKind);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error getting kind attr for fabric memory\n");
        goto freeMemCommon;
    }

    // Set PTE kind attribute for fabric memory.
    memdescSetPteKind(pMemory->pMemDesc, pteKind);
    memdescSetGpuCacheAttrib(pMemory->pMemDesc, NV_MEMORY_UNCACHED);

    // Allocate memory for memory fabric memdesc private data.
    pMemdescData = portMemAllocNonPaged(sizeof(FABRIC_MEMDESC_DATA));
    if (pMemdescData == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto freeMemCommon;
    }
    portMemSet(pMemdescData, 0, sizeof(FABRIC_MEMDESC_DATA));

    // Associate the memdesc data release callback function.
    memdescSetMemData(pMemDesc, (void *)pMemdescData, NULL);

    // Allocate memory for the memory descriptor destroy callback.
    pCallback = portMemAllocNonPaged(sizeof(MEM_DESC_DESTROY_CALLBACK));
    if (pCallback == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto freeMemdescMemData;
    }
    portMemSet(pCallback, 0, sizeof(MEM_DESC_DESTROY_CALLBACK));

    // Associate the memdescDestroy callback function.
    pCallback->pObject         = (void *)pCallback;
    pCallback->destroyCallback =
                   (MemDescDestroyCallBack*) &_memoryfabricMemDescDestroyCallback;

    memdescAddDestroyCallback(pMemDesc, pCallback);

    // Dup the physical video memory handle and cache it in memfabric memdesc.
    status = pRmApi->DupObject(pRmApi, pFabricVAS->hClient, pFabricVAS->hDevice,
                        &pMemdescData->hDupedVidmem, pCallContext->pClient->hClient,
                        pAllocParams->map.hVidMem, 0);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to dup physical video memory handle\n");
        goto freeCallback;
    }

    status = fabricvaspaceVaToGpaMapInsert(pFabricVAS, pAddr[0], pVidMemDesc,
                                           pAllocParams->map.offset);
    if (status != NV_OK)
        goto freeDupedMem;

    // Map the memory fabric object at the given physical video memory offset.
    status = _memoryfabricMapPhysicalMemory(pAddr, numAddr, pAllocParams->allocSize,
                                            pAllocParams->pageSize, pVidMemDesc,
                                            pAllocParams->map.offset, bReadOnly);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                "Failed to map FLA at the given physical vidmem offset\n");
        goto memFabricRemoveVaToGpaMap;
    }

    pMemoryFabric->flags = pAllocParams->allocFlags;
    pMemory->bRpcAlloc = pFabricVAS->bRpcAlloc;

    portMemFree(pAddr);

    return NV_OK;

memFabricRemoveVaToGpaMap:
    fabricvaspaceVaToGpaMapRemove(pFabricVAS, pAddr[0]);

freeDupedMem:
    // Free the duped vidmem handle.
    NV_ASSERT(pRmApi->Free(pRmApi, pFabricVAS->hClient,
                           pMemdescData->hDupedVidmem) == NV_OK);

freeCallback:
    // Destroy the memdesc destroy callback.
    memdescRemoveDestroyCallback(pMemDesc, pCallback);
    portMemFree(pCallback);
    pCallback = NULL;

freeMemdescMemData:
    // Free the memory fabric memdesc private data.
    portMemFree(pMemdescData);
    pMemdescData = NULL;

freeMemCommon:
    memDestructCommon(pMemory);

freeMemdesc:
    // Destroy the created memory descriptor.
    memdescDestroy(pMemDesc);
    pMemDesc = NULL;

freeVaspace:
    _memoryfabricFreeFabricVa(pFabricVAS, pGpu,
                              pParams, pAddr, numAddr);

    // Free memory allocated for vaspace allocations.
    portMemFree(pAddr);

    return status;
}

void
memoryfabricDestruct_IMPL
(
    MemoryFabric *pMemoryFabric
)
{
    return;
}

NvBool
memoryfabricCanCopy_IMPL
(
    MemoryFabric *pMemoryFabric
)
{
    return NV_TRUE;
}

NV_STATUS
memoryfabricCopyConstruct_IMPL
(
    MemoryFabric                 *pMemoryFabric,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    //
    // Memory fabric object must enforce the source (owner) GPU for duping.
    // However, CUDA and UVM drivers have been using destination (mapping)
    // GPU to dup memory objects in general. The changes involved in the
    // UVM driver would need more time as they are a bit involved. Thus,
    // for now RM is temporarily relaxing this restriction.
    //
    // The duping restriction will be added back once UVM bug 3367020
    // is fixed.
    //

    return NV_OK;
}

NvBool
memoryfabricCanExport_IMPL
(
    MemoryFabric *pMemoryFabric
)
{
    //
    // Check if FLA->PA mappings are present. Only then allow export.
    // FLA->PA mappings are guaranteed for STICKY FLA mappings, which is only
    // what we support currently.
    // TODO: Re-visit this function when support for FLEXIBLE FLA mappings is
    // added.
    //
    return !(pMemoryFabric->flags & NV00F8_ALLOC_FLAGS_FLEXIBLE_FLA);
}

NV_STATUS
memoryfabricControl_IMPL
(
    MemoryFabric                   *pMemoryFabric,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS status;

    if (REF_VAL(NVXXXX_CTRL_CMD_CLASS, pParams->cmd) != NV_MEMORY_FABRIC)
        return NV_ERR_INVALID_ARGUMENT;

    status = resControl_IMPL(staticCast(pMemoryFabric, RsResource),
                             pCallContext, pParams);

    return status;
}

NV_STATUS
memoryfabricCtrlGetInfo_IMPL
(
    MemoryFabric                *pMemoryFabric,
    NV00F8_CTRL_GET_INFO_PARAMS *pParams
)
{
    Memory *pMemory = staticCast(pMemoryFabric, Memory);

    if (pMemory->pMemDesc == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    pParams->size = memdescGetSize(pMemory->pMemDesc);
    pParams->pageSize = memdescGetPageSize(pMemory->pMemDesc, AT_GPU);
    pParams->allocFlags = pMemoryFabric->flags;

    return NV_OK;
}

NV_STATUS
memoryfabricCtrlCmdDescribe_IMPL
(
    MemoryFabric                  *pMemoryFabric,
    NV00F8_CTRL_DESCRIBE_PARAMS   *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}
