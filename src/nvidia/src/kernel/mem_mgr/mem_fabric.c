/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/device/device.h"
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
#include "kernel/gpu/gpu_fabric_probe.h"
#include "ctrl/ctrl0041.h"

static NvU32
_memoryfabricMemDescGetNumAddr
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    OBJGPU *pGpu     = pMemDesc->pGpu;
    NvU64   pageSize = 0;

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

static NV_STATUS
_memoryfabricValidatePhysMem
(
    RsClient           *pRsClient,
    NvHandle            hPhysMem,
    OBJGPU             *pOwnerGpu,
    MEMORY_DESCRIPTOR **ppPhysMemDesc,
    Memory            **ppPhysMemory
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pOwnerGpu);
    MEMORY_DESCRIPTOR *pPhysMemDesc;
    NvU64 physPageSize;
    NV_STATUS status;
    Memory *pMemory;

    status = memGetByHandle(pRsClient, hPhysMem, &pMemory);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid object handle passed\n");
        return status;
    }

    pPhysMemDesc = pMemory->pMemDesc;

    if ((pOwnerGpu != pPhysMemDesc->pGpu) ||
        !memmgrIsMemDescSupportedByFla_HAL(pOwnerGpu,
                                           pMemoryManager,
                                           pPhysMemDesc))
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid physmem handle passed\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    physPageSize = memdescGetPageSize(pPhysMemDesc, AT_GPU);
    if (
        (physPageSize != RM_PAGE_SIZE_HUGE) &&
        (physPageSize != RM_PAGE_SIZE_512M))
    {
        NV_PRINTF(LEVEL_ERROR, "Physmem page size should be 2MB\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    *ppPhysMemDesc = pPhysMemDesc;

    if (ppPhysMemory != NULL)
        *ppPhysMemory = pMemory;

    return NV_OK;
}

static NV_STATUS
_memoryFabricDetachMem
(
    MemoryFabric        *pMemoryFabric,
    NvU64                offset,
    NvBool               bRemoveInterMapping
)
{
    NV_STATUS status;
    FABRIC_ATTCH_MEM_INFO_NODE *pAttachMemInfoNode;
    NODE *pNode = NULL;
    FABRIC_VASPACE *pFabricVAS;
    Memory *pMemory = staticCast(pMemoryFabric, Memory);
    MEMORY_DESCRIPTOR *pFabricMemDesc = pMemory->pMemDesc;
    MEMORY_DESCRIPTOR *pPhysMemDesc;
    FABRIC_MEMDESC_DATA *pMemdescData;

    pMemdescData = (FABRIC_MEMDESC_DATA *)memdescGetMemData(pFabricMemDesc);

    status = btreeSearch(offset, &pNode, pMemdescData->pAttachMemInfoTree);
    if (status != NV_OK)
        return status;

    pAttachMemInfoNode = (FABRIC_ATTCH_MEM_INFO_NODE *)pNode->Data;
    pPhysMemDesc = pAttachMemInfoNode->pPhysMemDesc;
    pFabricVAS = dynamicCast(pPhysMemDesc->pGpu->pFabricVAS, FABRIC_VASPACE);

    fabricvaspaceUnmapPhysMemdesc(pFabricVAS, pFabricMemDesc, offset,
                                  pPhysMemDesc,
                                  pAttachMemInfoNode->physMapLength);

    if (bRemoveInterMapping)
    {
        refRemoveInterMapping(RES_GET_REF(pMemoryFabric),
                              pAttachMemInfoNode->pInterMapping);
    }

    btreeUnlink(&pAttachMemInfoNode->node, &pMemdescData->pAttachMemInfoTree);

    portMemFree(pAttachMemInfoNode);

    return NV_OK;
}

static NV_STATUS
_memoryFabricAttachMem
(
    MemoryFabric                *pMemoryFabric,
    NV00F8_CTRL_ATTACH_MEM_INFO *pAttachInfo
)
{
    NV_STATUS status;
    Memory *pMemory = staticCast(pMemoryFabric, Memory);
    OBJGPU *pGpu = pMemory->pGpu;
    MEMORY_DESCRIPTOR *pPhysMemDesc;
    MEMORY_DESCRIPTOR *pFabricMemDesc = pMemory->pMemDesc;
    FABRIC_MEMDESC_DATA *pMemdescData;
    FABRIC_VASPACE *pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
    FABRIC_ATTCH_MEM_INFO_NODE *pNode;
    RsInterMapping *pInterMapping;
    Memory *pPhysMemory;

    pMemdescData = (FABRIC_MEMDESC_DATA *)memdescGetMemData(pFabricMemDesc);

    if (!(pMemdescData->allocFlags & NV00F8_ALLOC_FLAGS_FLEXIBLE_FLA))
    {
        NV_PRINTF(LEVEL_ERROR, "Unsupported fabric memory type\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    if (gpuIsCCFeatureEnabled(pGpu) && (!gpuIsCCMultiGpuProtectedPcieModeEnabled(pGpu) && !gpuIsCCMultiGpuNvleModeEnabled(pGpu)))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unsupported when Confidential Computing is enabled in SPT\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    status = _memoryfabricValidatePhysMem(RES_GET_CLIENT(pMemory), pAttachInfo->hMemory,
                                          pGpu, &pPhysMemDesc, &pPhysMemory);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);

    status = refAddInterMapping(RES_GET_REF(pMemoryFabric),
                                RES_GET_REF(pPhysMemory),
                                RES_GET_REF(pMemoryFabric)->pParentRef,
                                &pInterMapping);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to setup inter mapping\n");
        return status;
    }

    status = fabricvaspaceMapPhysMemdesc(pFabricVAS,
                                         pFabricMemDesc,
                                         pAttachInfo->offset,
                                         pPhysMemDesc,
                                         pAttachInfo->mapOffset,
                                         pAttachInfo->mapLength,
                                         0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to map FLA\n");
        goto freeInterMapping;
    }

    pNode = portMemAllocNonPaged(sizeof(*pNode));
    if (pNode == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto unmapVas;
    }

    portMemSet(pNode, 0, sizeof(*pNode));

    pNode->node.keyStart = pAttachInfo->offset;
    pNode->node.keyEnd   = pAttachInfo->offset;
    pNode->physMapOffset = pAttachInfo->mapOffset;
    pNode->physMapLength = pAttachInfo->mapLength;
    pNode->pPhysMemDesc  = pPhysMemDesc;
    pNode->pInterMapping = pInterMapping;
    pNode->node.Data     = pNode;

    pInterMapping->dmaOffset = pAttachInfo->offset;
    pInterMapping->pMemDesc = pPhysMemDesc;

    // No partial unmap supported
    pInterMapping->size = 0;

    status = btreeInsert(&pNode->node, &pMemdescData->pAttachMemInfoTree);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to track attach mem info\n");
        goto freeNode;
    }

    return NV_OK;

freeNode:
    portMemFree(pNode);

unmapVas:
    fabricvaspaceUnmapPhysMemdesc(pFabricVAS, pFabricMemDesc,
                                  pAttachInfo->offset,
                                  pPhysMemDesc, pAttachInfo->mapLength);

freeInterMapping:
    refRemoveInterMapping(RES_GET_REF(pMemoryFabric), pInterMapping);

    return status;
}

static void
_memoryfabricMemDescDestroyCallback
(
    OBJGPU            *pGpu,
    void              *pObject,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    RM_API              *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    FABRIC_VASPACE      *pFabricVAS;
    RmPhysAddr          *pteArray;
    FABRIC_MEMDESC_DATA *pMemdescData;
    NvU32                numAddr;
    NvU64                pageSize;
    NODE                *pNode;

    NV_ASSERT_OR_RETURN_VOID(pGpu->pFabricVAS != NULL);

    pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
    pMemdescData = (FABRIC_MEMDESC_DATA *)memdescGetMemData(pMemDesc);
    pteArray = memdescGetPteArrayForGpu(pMemDesc, pGpu,
                                    VAS_ADDRESS_TRANSLATION(pGpu->pFabricVAS));
    numAddr = _memoryfabricMemDescGetNumAddr(pMemDesc);
    // Get the page size from the memory descriptor.
    pageSize = memdescGetPageSize(pMemDesc,
                                    VAS_ADDRESS_TRANSLATION(pGpu->pFabricVAS));

    // Remove the fabric memory allocations from the map.
    fabricvaspaceVaToGpaMapRemove(pFabricVAS, pteArray[0]);

    // Every attached memory should have been unmapped by now..
    btreeEnumStart(0, &pNode, pMemdescData->pAttachMemInfoTree);
    NV_ASSERT(pNode == NULL);

    if (!pFabricVAS->bRpcAlloc)
    {
        //
        // Call fabricvaspaceBatchFree to free the FLA allocations.
        // _pteArray in memdesc is RM_PAGE_SIZE whereas page size for memory
        // fabric allocations is either 2MB or 512MB. Pass stride accordingly.
        //
        fabricvaspaceBatchFree(pFabricVAS, pteArray, numAddr,
                               (pageSize >> RM_PAGE_SHIFT));
    }

    if (pMemdescData != NULL)
    {
        if (pMemdescData->hDupedPhysMem != 0)
        {
            NV_ASSERT(pRmApi->Free(pRmApi, pFabricVAS->hClient,
                                   pMemdescData->hDupedPhysMem) == NV_OK);
        }

        portMemFree(pMemDesc->_pMemData);
        memdescSetMemData(pMemDesc, NULL, NULL);
    }

    portMemFree(pObject);
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
                           sizeof(*pAllocParams),
                           status);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Alloc NV_MEMORY_FABRIC RPC failed, status: %x\n",
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
                      status, pDescribeParams->numPfns,
                      pDescribeParams->totalPfns, idx);
            goto cleanup;
        }

        if (pAddr == NULL)
        {
            pAddr = portMemAllocNonPaged(sizeof(NvU64) *
                                         pDescribeParams->totalPfns);
            if (pAddr == NULL)
            {
                status = NV_ERR_NO_MEMORY;
                goto cleanup;
            }
        }

        for (i=0; i < pDescribeParams->numPfns; i++)
        {
            pAddr[idx + i] =
                (NvU64)((NvU64)pDescribeParams->pfnArray[i] << RM_PAGE_SHIFT_HUGE);
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
                                               fabricvaspaceGetUCFlaStart(pFabricVAS),
                                               fabricvaspaceGetUCFlaLimit(pFabricVAS),
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
    Memory *pMemory = staticCast(pMemoryFabric, Memory);
    OBJGPU *pGpu = pMemory->pGpu;
    FABRIC_VASPACE *pFabricVAS  = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
    NV00F8_ALLOCATION_PARAMETERS *pAllocParams   = pParams->pAllocParams;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    MEMORY_DESCRIPTOR *pPhysMemDesc = NULL;
    NV_STATUS status = NV_OK;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    FABRIC_MEMDESC_DATA *pMemdescData = NULL;
    MEM_DESC_DESTROY_CALLBACK *pCallback = NULL;
    VAS_ALLOC_FLAGS flags = {0};
    NvU64   *pAddr = NULL;
    NvU32    numAddr = 0;
    NvU32    pteKind = 0;
    NvBool   bReadOnly = NV_FALSE;
    NvHandle hPhysMem;
    NvBool   bFlexible = NV_FALSE;
    NvU32    mapFlags = 0;

    if (RS_IS_COPY_CTOR(pParams))
    {
        return memoryfabricCopyConstruct_IMPL(pMemoryFabric,
                                              pCallContext,
                                              pParams);
    }

    hPhysMem  = pAllocParams->map.hVidMem;

    // Check if fabric vaspace is valid.
    if (pFabricVAS == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Fabric vaspace object not available\n");

        return NV_ERR_NOT_SUPPORTED;
    }

    // initialize Fabric VAS Unicast range if not already setup
    if (fabricvaspaceGetUCFlaLimit(pFabricVAS) == 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "UC FLA ranges should be initialized by this time!\n");
        return NV_ERR_INVALID_STATE;
    }

    if (
        (pAllocParams->pageSize != NV_MEMORY_FABRIC_PAGE_SIZE_256G) &&
        (pAllocParams->pageSize != NV_MEMORY_FABRIC_PAGE_SIZE_512M) &&
        (pAllocParams->pageSize != NV_MEMORY_FABRIC_PAGE_SIZE_2M))
    {
        NV_PRINTF(LEVEL_ERROR, "Unsupported pageSize: 0x%llx\n",
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

    bFlexible = !!(pAllocParams->allocFlags & NV00F8_ALLOC_FLAGS_FLEXIBLE_FLA);

    if (pAllocParams->allocFlags & NV00F8_ALLOC_FLAGS_READ_ONLY)
    {
#if !defined(DEVELOP) && !defined(DEBUG) && !RMCFG_FEATURE_MODS_FEATURES
        NV_PRINTF(LEVEL_ERROR,
                  "RO mappings are only supported on non-release builds\n");

        return NV_ERR_NOT_SUPPORTED;
#else
        bReadOnly = NV_TRUE;
#endif
    }

    if (bFlexible && (hPhysMem != 0))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Physmem can't be provided during flexible object alloc\n");

        return NV_ERR_NOT_SUPPORTED;
    }
    else if (!bFlexible)
    {
        status = _memoryfabricValidatePhysMem(pCallContext->pClient, hPhysMem,
                                              pGpu, &pPhysMemDesc, NULL);
        if (status != NV_OK)
            return status;
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
                  "RangeLo: 0x%llx, RangeHi: 0x%llx, page size: 0x%llx\n",
                  status, pAllocParams->allocSize,
                  fabricvaspaceGetUCFlaStart(pFabricVAS),
                  fabricvaspaceGetUCFlaLimit(pFabricVAS),
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

    //
    // In case of flexible mappings, we don't support:
    //
    // 1. Caching attributes of physical memory
    // 2. FLA to GPA tracking to allow FLA object to be mapped as local memory
    //
    if (hPhysMem != 0)
    {
        // Dup the physical memory handle and cache it in memfabric memdesc.
        status = pRmApi->DupObject(pRmApi, pFabricVAS->hClient,
                                   pFabricVAS->hDevice,
                                   &pMemdescData->hDupedPhysMem,
                                   pCallContext->pClient->hClient,
                                   hPhysMem, 0);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to dup physmem handle\n");
            goto freeCallback;
        }

        NV0041_CTRL_SURFACE_INFO surfaceInfo[2] = {0};
        NV0041_CTRL_GET_SURFACE_INFO_PARAMS surfaceInfoParam = {0};

        surfaceInfo[0].index = NV0041_CTRL_SURFACE_INFO_INDEX_ADDR_SPACE_TYPE;
        surfaceInfo[1].index = NV0041_CTRL_SURFACE_INFO_INDEX_ATTRS;
        surfaceInfoParam.surfaceInfoListSize = 2;
        surfaceInfoParam.surfaceInfoList = NvP64_VALUE(&surfaceInfo);

        status = pRmApi->Control(pRmApi,
                                 pFabricVAS->hClient,
                                 pMemdescData->hDupedPhysMem,
                                 NV0041_CTRL_CMD_GET_SURFACE_INFO,
                                 &surfaceInfoParam,
                                 sizeof(surfaceInfoParam));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to query physmem info\n");
            goto freeDupedMem;
        }

        pMemdescData->physAttrs.addressSpace = surfaceInfo[0].data;
        //
        // TODO: Bug 4322867: use NV0041_CTRL_SURFACE_INFO_ATTRS_COMPR
        // instead of NV0041_CTRL_SURFACE_INFO_INDEX_COMPR_COVERAGE.
        // NV0041_CTRL_SURFACE_INFO_INDEX_COMPR_COVERAGE is buggy and
        // will be removed soon.
        //
        pMemdescData->physAttrs.compressionCoverage =
            (surfaceInfo[1].data & NV0041_CTRL_SURFACE_INFO_ATTRS_COMPR) ?  0x1 : 0x0;

        mapFlags |= bReadOnly ? FABRIC_VASPACE_MAP_FLAGS_READ_ONLY : 0;

        //
        // Sticky FLA object should be mapped completely, so pass
        // pAllocParams->allocSize.as mapLength.
        //
        status = fabricvaspaceMapPhysMemdesc(pFabricVAS,
                                             pMemDesc, 0,
                                             pPhysMemDesc,
                                             pAllocParams->map.offset,
                                             pAllocParams->allocSize,
                                             mapFlags);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "Failed to map FLA at the given physmem offset\n");
            goto freeDupedMem;
        }

        //
        // No need to unmap on failure. Unmap happens implicitly when fabric VA
        // would be freed.
        //
        status = fabricvaspaceVaToGpaMapInsert(pFabricVAS, pAddr[0],
                                               pPhysMemDesc,
                                               pAllocParams->map.offset);
        if (status != NV_OK)
            goto freeDupedMem;
    }

    pMemdescData->allocFlags = pAllocParams->allocFlags;
    pMemory->bRpcAlloc = pFabricVAS->bRpcAlloc;

    portMemFree(pAddr);

    return NV_OK;

freeDupedMem:
    // Free the duped physmem handle.
    NV_ASSERT(pRmApi->Free(pRmApi, pFabricVAS->hClient,
                           pMemdescData->hDupedPhysMem) == NV_OK);

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
    FABRIC_MEMDESC_DATA *pMemdescData;

    if (pMemory->pMemDesc == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    pMemdescData = (FABRIC_MEMDESC_DATA *)memdescGetMemData(pMemory->pMemDesc);

    pParams->size = memdescGetSize(pMemory->pMemDesc);
    pParams->pageSize = memdescGetPageSize(pMemory->pMemDesc, AT_GPU);
    pParams->allocFlags = pMemdescData->allocFlags;
    pParams->physAttrs = pMemdescData->physAttrs;

    return NV_OK;
}

NV_STATUS
memoryfabricCtrlCmdDescribe_IMPL
(
    MemoryFabric                  *pMemoryFabric,
    NV00F8_CTRL_DESCRIBE_PARAMS   *pParams
)
{
    NV_STATUS status;
    Memory *pMemory = staticCast(pMemoryFabric, Memory);
    NvU64  *pFabricArray;
    NvU64   offset;
    NvU64   pageSize;
    NvU32   i;
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    FABRIC_MEMDESC_DATA *pMemdescData;
    OBJGPU *pGpu;

    RmClient *pRmClient = dynamicCast(pCallContext->pClient, RmClient);
    NV_ASSERT_OR_RETURN(pRmClient != NULL, NV_ERR_INVALID_CLIENT);

    if (
        !rmclientIsCapableOrAdmin(pRmClient, NV_RM_CAP_SYS_FABRIC_IMEX_MGMT, pCallContext->secInfo.privLevel)
    )
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (pMemory->pMemDesc == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    pGpu = pMemory->pMemDesc->pGpu;
    pMemdescData = (FABRIC_MEMDESC_DATA *)memdescGetMemData(pMemory->pMemDesc);

    pParams->memFlags = pMemdescData->allocFlags;
    pParams->physAttrs = pMemdescData->physAttrs;

    pParams->attrs.pageSize = memdescGetPageSize(pMemory->pMemDesc, AT_GPU);
    pParams->attrs.kind = memdescGetPteKind(pMemory->pMemDesc);
    pParams->attrs.size = memdescGetSize(pMemory->pMemDesc);

    if (gpuFabricProbeIsSupported(pGpu))
    {
        status = gpuFabricProbeGetFabricCliqueId(pGpu->pGpuFabricProbeInfoKernel,
                                                 &pParams->attrs.cliqueId);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "unable to query cliqueId 0x%x\n", status);
            return status;
        }
    }
    else
    {
        pParams->attrs.cliqueId = 0;
    }

    pageSize = memdescGetPageSize(pMemory->pMemDesc, AT_GPU);

    if (memdescGetContiguity(pMemory->pMemDesc, AT_GPU))
        pParams->totalPfns = 1;
    else
        pParams->totalPfns = memdescGetSize(pMemory->pMemDesc) / pageSize;

    if (pParams->offset >= pParams->totalPfns)
    {
        NV_PRINTF(LEVEL_ERROR, "offset: 0x%llx is out of range: 0x%llx\n",
                  pParams->offset, pParams->totalPfns);
        return NV_ERR_OUT_OF_RANGE;
    }

    pParams->numPfns = NV_MIN(pParams->totalPfns - pParams->offset,
                              NV00F8_CTRL_DESCRIBE_PFN_ARRAY_SIZE);

    pFabricArray = portMemAllocNonPaged(sizeof(NvU64) * pParams->numPfns);

    if (pFabricArray == NULL)
        return NV_ERR_NO_MEMORY;

    offset = pParams->offset * pageSize;
    memdescGetPhysAddrsForGpu(pMemory->pMemDesc, pGpu, AT_GPU, offset,
                              pageSize, pParams->numPfns, pFabricArray);

    for (i = 0; i < pParams->numPfns; i++)
    {
        pParams->pfnArray[i] = (NvU32)(pFabricArray[i] >> RM_PAGE_SHIFT_HUGE);
    }

    portMemFree(pFabricArray);

    return NV_OK;
}

NV_STATUS
memoryfabricCtrlAttachMem_IMPL
(
    MemoryFabric                  *pMemoryFabric,
    NV00F8_CTRL_ATTACH_MEM_PARAMS *pParams
)
{
    NvU32 i;
    NV_STATUS status;

    pParams->numAttached = 0;

    if (pParams->flags != 0)
        return NV_ERR_INVALID_ARGUMENT;

    if ((pParams->numMemInfos == 0) ||
        (pParams->numMemInfos > NV00F8_MAX_ATTACHABLE_MEM_INFOS))
        return NV_ERR_INVALID_ARGUMENT;

    for (i = 0; i < pParams->numMemInfos; i++)
    {
        status = _memoryFabricAttachMem(pMemoryFabric, &pParams->memInfos[i]);
        if (status != NV_OK)
            return status;

        pParams->numAttached++;
    }

    return NV_OK;
}

NV_STATUS
memoryfabricCtrlDetachMem_IMPL
(
    MemoryFabric                  *pMemoryFabric,
    NV00F8_CTRL_DETACH_MEM_PARAMS *pParams
)
{
    NvU32 i;
    NV_STATUS status;

    pParams->numDetached = 0;

    if (pParams->flags != 0)
        return NV_ERR_INVALID_ARGUMENT;

    if ((pParams->numOffsets == 0) ||
         pParams->numOffsets > NV00F8_MAX_DETACHABLE_OFFSETS)
        return NV_ERR_INVALID_ARGUMENT;

    for (i = 0; i < pParams->numOffsets; i++)
    {
        // Remove inter mapping on explicit detach by the control call.
        status = _memoryFabricDetachMem(pMemoryFabric, pParams->offsets[i],
                                        NV_TRUE);
        if (status != NV_OK)
            return status;

        pParams->numDetached++;
    }

    return NV_OK;
}

NV_STATUS
memoryfabricCtrlGetNumAttachedMem_IMPL
(
    MemoryFabric                            *pMemoryFabric,
    NV00F8_CTRL_GET_NUM_ATTACHED_MEM_PARAMS *pParams
)
{
    Memory *pMemory = staticCast(pMemoryFabric, Memory);
    MEMORY_DESCRIPTOR *pFabricMemDesc = pMemory->pMemDesc;
    FABRIC_MEMDESC_DATA *pMemdescData = \
                    (FABRIC_MEMDESC_DATA *)memdescGetMemData(pFabricMemDesc);
    NODE *pNode = NULL;

    pParams->numMemInfos = 0;

    btreeEnumStart(pParams->offsetStart, &pNode,
                   pMemdescData->pAttachMemInfoTree);

    while ((pNode != NULL) && (pNode->keyStart <= pParams->offsetEnd))
    {
        pParams->numMemInfos++;
        btreeEnumNext(&pNode, pMemdescData->pAttachMemInfoTree);
    }

    return NV_OK;
}

static NV_STATUS
_memoryfabricGetPhysAttrsUsingFabricMemdesc
(
    OBJGPU            *pGpu,
    FABRIC_VASPACE    *pFabricVAS,
    MEMORY_DESCRIPTOR *pFabricMemDesc,
    NvU64              offset,
    NvU64             *pPhysPageSize
)
{
    NV_STATUS status;
    MEMORY_DESCRIPTOR *pPhysMemDesc;
    FABRIC_MEMDESC_DATA *pMemdescData;
    NODE *pNode = NULL;
    FABRIC_ATTCH_MEM_INFO_NODE *pAttachMemInfoNode;

    if ((pFabricMemDesc == NULL) || (pPhysPageSize == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = fabricvaspaceGetGpaMemdesc(pFabricVAS, pFabricMemDesc, pGpu,
                                        &pPhysMemDesc);
    if (status == NV_OK)
    {
        *pPhysPageSize = memdescGetPageSize(pPhysMemDesc, AT_GPU);
        if (pPhysMemDesc != pFabricMemDesc)
            fabricvaspacePutGpaMemdesc(pFabricVAS, pPhysMemDesc);
        return NV_OK;
    }

    if (status != NV_ERR_OBJECT_NOT_FOUND)
        return status;

    // If the object is flexible, check the attach mem info tree.
    pMemdescData = (FABRIC_MEMDESC_DATA *)memdescGetMemData(pFabricMemDesc);
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, btreeSearch(offset, &pNode,
                          pMemdescData->pAttachMemInfoTree));
    pAttachMemInfoNode = (FABRIC_ATTCH_MEM_INFO_NODE*)pNode->Data;

    *pPhysPageSize = memdescGetPageSize(pAttachMemInfoNode->pPhysMemDesc,
                                          AT_GPU);

    return NV_OK;
}

NV_STATUS
memoryfabricCtrlGetPageLevelInfo_IMPL
(
    MemoryFabric                           *pMemoryFabric,
    NV00F8_CTRL_GET_PAGE_LEVEL_INFO_PARAMS *pParams
)
{
    Memory *pMemory = staticCast(pMemoryFabric, Memory);
    OBJGPU *pGpu  = pMemory->pGpu;
    FABRIC_VASPACE *pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
    NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS pageLevelInfoParams = {0};
    MEMORY_DESCRIPTOR *pFabricMemDesc = pMemory->pMemDesc;
    NvU64 fabricAddr;
    NvU64 mappingPageSize;
    NvU64 physPageSize;
    NvU32 i;

    NV_ASSERT_OR_RETURN(pFabricMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          _memoryfabricGetPhysAttrsUsingFabricMemdesc(pGpu,
                          pFabricVAS, pFabricMemDesc, pParams->offset,
                          &physPageSize));

    mappingPageSize = NV_MIN(physPageSize,
                             memdescGetPageSize(pFabricMemDesc, AT_GPU));

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                       NV_IS_ALIGNED64(pParams->offset, mappingPageSize),
                       NV_ERR_INVALID_ARGUMENT);

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                       pParams->offset < memdescGetSize(pFabricMemDesc),
                       NV_ERR_INVALID_ARGUMENT);

    memdescGetPhysAddrsForGpu(pFabricMemDesc, pGpu, AT_GPU, pParams->offset,
                              mappingPageSize, 1, &fabricAddr);

    pageLevelInfoParams.virtAddress = fabricAddr;
    pageLevelInfoParams.pageSize = mappingPageSize;

    NV_ASSERT_OK_OR_RETURN(fabricvaspaceGetPageLevelInfo(pFabricVAS, pGpu,
                           &pageLevelInfoParams));

    pParams->numLevels = pageLevelInfoParams.numLevels;
    for (i = 0; i < pParams->numLevels; i++)
    {
        portMemCopy(&pParams->levels[i], sizeof(pParams->levels[i]),
                    &pageLevelInfoParams.levels[i],
                    sizeof(pageLevelInfoParams.levels[i]));
    }

    return NV_OK;
}

NV_STATUS
memoryfabricUnmapFrom_IMPL
(
    MemoryFabric             *pMemoryFabric,
    RS_RES_UNMAP_FROM_PARAMS *pParams
)
{
    OBJGPU *pGpu = pParams->pGpu;
    NvU32 gpuMask = NVBIT(gpuGetInstance(pGpu));

    NV_ASSERT_OR_RETURN(rmGpuGroupLockIsOwner(0, GPU_LOCK_GRP_MASK, &gpuMask),
                        NV_ERR_INVALID_STATE);

    NV_ASSERT_OK_OR_RETURN(_memoryFabricDetachMem(pMemoryFabric,
                                                  pParams->dmaOffset, NV_FALSE));

    return NV_OK;
}
