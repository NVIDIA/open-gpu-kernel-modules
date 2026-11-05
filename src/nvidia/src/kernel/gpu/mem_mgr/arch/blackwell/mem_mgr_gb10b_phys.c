/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "core/system.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/carveout_mgr.h"
#include "os/os.h"
#include "os/nv_memory_type.h"
#include "mem_mgr/system_mem.h"
#include "gpu/mem_mgr/mem_mgr.h"

static CarveoutMgr *
_memmgrGetCarveoutMgr(void)
{
    return SYS_GET_CARVEOUT_MGR(SYS_GET_INSTANCE());
}

/**
 * @brief Initializes FB regions
 *
 * This function is responsible for setting up a carveout if one is required.
 * This is a different type of carveout from what has been done on MCP in the
 * past.  The main difference is that even though it will be treated as "video
 * memory", all addresses will be fully resolved system memory addresses.  This
 * is done by setting up a reserved region of FB from 0 to the sysmem base of
 * carveout as described in the picture below.
 *
 * dGPU Video Memory
 * +-----------------------------|
 * |        Vidmem               |
 * +-----------------------------|
 *
 * @returns OK if success, ERROR otherwise
 */
NV_STATUS
memmgrInitZeroFbRegionsHal_GB10B
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NV_STATUS status =  NV_OK;

    // Simulation won't have carveout support
    if (IS_SIMULATION(pGpu))
    {
        NV_PRINTF(LEVEL_INFO, "Carveout support not available on simulation\n");
        return NV_OK;
    }

    status = memmgrCreateScanoutCarveoutHeap(pGpu, pMemoryManager);

    //
    // Check if platform has carveout support configured, if not configured,
    // return early with NV_OK. In this case, all allocations will happen
    // in the system memory itself.
    //
    if (status == NV_ERR_NOT_SUPPORTED)
    {
        NV_PRINTF(LEVEL_INFO, "Carveout supported not configured on this platform\n");
        return NV_OK;
    }

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to create scanout carveout heap.\n");
        return status;
    }

    return status;
}

NV_STATUS
memmgrCreateScanoutCarveoutHeap_GB10B
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    CarveoutMgr *pCarveoutMgr = _memmgrGetCarveoutMgr();
    NV2080_CTRL_FB_GET_CARVEOUT_REGION_INFO_PARAMS params;
    NvU64 base = 0;
    NvU64 size = 0;
    NvU32 i;
    NV_STATUS status;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pCarveoutMgr != NULL, NV_ERR_INVALID_STATE);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pMemoryManager != NULL, NV_ERR_INVALID_ARGUMENT);

    status = osGetCarveoutInfo(&base, &size);

    // If DT carveout information is not present, fall back to FRM.
    if (status == NV_ERR_NOT_SUPPORTED)
    {
        portMemSet(&params, 0x0, sizeof(params));
        status = memmgrGetCarveoutRegionInfo(pGpu, pMemoryManager, &params);
        if (status == NV_OK)
        {
            for (i = 0; i < params.numCarveoutRegions; i++)
            {
                if (params.carveoutRegion[i].carveoutType ==
                    NV2080_CTRL_FB_GET_CARVEOUT_REGION_CARVEOUT_TYPE_DISPLAY_FRM)
                {
                    base = params.carveoutRegion[i].base;
                    size = params.carveoutRegion[i].size;
                    break;
                }
            }
        }

        if ((status != NV_OK) || (size == 0))
        {
            NV_PRINTF(LEVEL_INFO,
                      "did not get scanout carveout info for GPU instance %u\n",
                      gpuGetInstance(pGpu));
            pGpu->setProperty(pGpu, PDB_PROP_GPU_ALLOC_ISO_SYS_MEM_FROM_CARVEOUT, NV_FALSE);
            return NV_OK;
        }
    }
    else if (status != NV_OK || size == 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to get scanout carveout info from device tree for "
                  "GPU instance %u, status=0x%x, size=0x%llx\n",
                  gpuGetInstance(pGpu), status, size);
        pGpu->setProperty(pGpu, PDB_PROP_GPU_ALLOC_ISO_SYS_MEM_FROM_CARVEOUT, NV_FALSE);
        return status == NV_OK ? NV_ERR_INVALID_DATA : status;
    }

    status = carveoutmgrAcquireHeap(pCarveoutMgr, pGpu, base, size);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to acquire shared scanout carveout heap.\n");
        return status;
    }

    NV_PRINTF(LEVEL_INFO, "Created scanout carveout heap with base address=0x%llx and size=%llx\n", base, size);

    return NV_OK;
}

NV_STATUS
memmgrDestroyScanoutCarveoutHeap_GB10B
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    CarveoutMgr *pCarveoutMgr = _memmgrGetCarveoutMgr();

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pCarveoutMgr != NULL, NV_ERR_INVALID_STATE);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pMemoryManager != NULL, NV_ERR_INVALID_ARGUMENT);

    carveoutmgrReleaseHeap(pCarveoutMgr, pGpu);

    return NV_OK;
}

/**
 * @brief Allocates memory from scanout carveout region
 *
 * This function request a free range (base address and size) first from
 * scanout carveout heap and update a memory descriptor for that.
 * The memory is already reserved, so we can set the physical address with
 * memdescSetPte().
 *
 * @returns OK if success, ERROR otherwise
 */
NV_STATUS
memmgrAllocScanoutCarveoutRegionResources_GB10B
(
    MemoryManager *pMemoryManager,
    NV_MEMORY_ALLOCATION_PARAMS *pAllocData,
    NvU32 heapOwner,
    NvU32 *pHeapFlag,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    CarveoutMgr *pCarveoutMgr = _memmgrGetCarveoutMgr();
    OBJGPU *pGpu;
    NvU64 alignment;
    NvU64 offset;
    NvU64 size;
    NvU64 carveoutBase;
    NvU64 carveoutSize;
    NvU64 rangeLo;
    NvU64 rangeHi;
    NV_STATUS status = NV_OK;

    NV_CHECK_OR_RETURN(LEVEL_INFO, pCarveoutMgr != NULL, NV_ERR_INVALID_STATE);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pMemoryManager != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pAllocData != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pHeapFlag != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);

    pGpu = ENG_GET_GPU(pMemoryManager);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pGpu != NULL, NV_ERR_INVALID_STATE);

    NV_ASSERT(memdescGetFlag(pMemDesc, MEMDESC_FLAGS_ALLOC_FROM_SCANOUT_CARVEOUT));

    alignment = pAllocData->alignment ? pAllocData->alignment : RM_PAGE_SIZE;
    offset = pAllocData->offset;
    size = pMemDesc->Size;

    status = carveoutmgrGetHeapInfo(pCarveoutMgr, &carveoutBase, &carveoutSize);
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                       (carveoutSize != 0) &&
                       (carveoutBase <= (NV_U64_MAX - (carveoutSize - 1))),
                       NV_ERR_INVALID_STATE);

    rangeLo = carveoutBase;
    rangeHi = carveoutBase + carveoutSize - 1;

    status = carveoutmgrAllocHeapBlock(pCarveoutMgr, pGpu, heapOwner,
                                       pHeapFlag, &offset, &size, alignment,
                                       rangeLo, rangeHi);
    if (status != NV_OK) {
        NV_PRINTF(LEVEL_ERROR, "EheapAlloc returns error 0x%08x\n", status);
        return status;
    }

    memdescSetPte(pMemDesc, AT_CPU, 0, offset);
    memdescSetContiguity(pMemDesc, AT_CPU, NV_TRUE);
    pMemDesc->Size = size;

    return NV_OK;
}

/**
 * @brief Allocates memory from scanout carveout region
 *
 * This function request a free range (base address and size) first from
 * scanout carveout heap and setup a memory descriptor for that.
 * The memory is already reserved, so we can set the physical address with
 * memdescSetPte().
 *
 * @returns OK if success, ERROR otherwise
 */
NV_STATUS
memmgrAllocFromScanoutCarveoutRegion_GB10B
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    NvU32 owner,
    NV_MEMORY_ALLOCATION_PARAMS *pVidHeapAlloc,
    NvU32 *pHeapFlag,
    PMEMORY_DESCRIPTOR *ppMemDesc
)
{
    MemorySystem        *pMemorySystem;
    MEMORY_DESCRIPTOR   *pMemDesc = NULL;
    NvU64                allocatedOffset;
    NvU64                size;
    NV_STATUS            status;

    NV_CHECK_OR_RETURN(LEVEL_INFO, pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pVidHeapAlloc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pHeapFlag != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, ppMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);

    pMemorySystem = GPU_GET_MEMORY_SYSTEM(pGpu);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pMemorySystem != NULL, NV_ERR_INVALID_STATE);

    size = pVidHeapAlloc->size;

    status = memdescCreate(&pMemDesc, pGpu, size, 0, NV_MEMORY_CONTIGUOUS,
                           ADDR_SYSMEM, NV_MEMORY_UNCACHED,
                           MEMDESC_FLAGS_ALLOC_FROM_SCANOUT_CARVEOUT);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "memdescCreate returns error 0x%08x\n", status);
        DBG_BREAKPOINT();
        return status;
    }

    memSetSysmemCacheAttrib(pGpu, pVidHeapAlloc, pMemDesc);

    status = memmgrAllocScanoutCarveoutRegionResources(pMemoryManager,
                                                       pVidHeapAlloc,
                                                       owner,
                                                       pHeapFlag,
                                                       pMemDesc);
    if (status != NV_OK) {
        DBG_BREAKPOINT();
        memdescDestroy(pMemDesc);
        return status;
    }

    // Save the heap-assigned address before memdescAlloc can alter PTE state.
    allocatedOffset = memdescGetPte(pMemDesc, AT_CPU, 0);
    status = memdescAlloc(pMemDesc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "memdescAlloc returns error 0x%08x\n", status);
        DBG_BREAKPOINT();

        //
        // This descriptor is not yet visible to callers and has one reference.
        // Its destruction also releases any partially allocated backing and
        // IOMMU mappings. Keep the eheap range reserved until that teardown is
        // complete.
        //
        memdescDestroy(pMemDesc);
        memmgrFreeScanoutCarveoutRegionResources(pMemoryManager, allocatedOffset);
        return status;
    }

    *ppMemDesc = pMemDesc;
    pVidHeapAlloc->offset = memdescGetPte(pMemDesc, AT_CPU, 0);
    pVidHeapAlloc->size = pMemDesc->Size;
    memdescSetCustomHeap(pMemDesc, MEMDESC_CUSTOM_HEAP_SCANOUT_CARVEOUT);

    return NV_OK;
}

/**
 * @brief Frees memory from scanout carveout region
 *
 * This function performs free operation for the memory allocated at given base
 * offset from scanout carveout region.
 *
 * This rollback helper is used before a memdesc association is published.
 * Published associations must use memmgrFreeFromScanoutCarveoutRegion so the
 * eheap and memdesc references are released together.
 *
 * @returns void
 */
void
memmgrFreeScanoutCarveoutRegionResources_GB10B
(
    MemoryManager *pMemoryManager,
    NvU64 base
)
{
    CarveoutMgr *pCarveoutMgr = _memmgrGetCarveoutMgr();
    NV_STATUS status;

    NV_CHECK_OR_RETURN_VOID(LEVEL_INFO, pCarveoutMgr != NULL);
    NV_CHECK_OR_RETURN_VOID(LEVEL_INFO, pMemoryManager != NULL);

    status = carveoutmgrFreeHeapBlock(pCarveoutMgr, base);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to release scanout carveout rollback block at "
                  "0x%llx, status=0x%x\n",
                  base, status);
        NV_ASSERT(status == NV_OK);
    }
}

/**
 * @brief Frees memory from scanout carveout region
 *
 * This function performs free operation for the memory allocated from scanout
 * carveout region. The system-level manager owns the association release, so
 * the originating GPU may already be detached when cleanup reaches it.
 *
 * @returns void
 */
void
memmgrFreeFromScanoutCarveoutRegion_GB10B
(
    OBJGPU             *pGpu,
    MemoryManager      *pMemoryManager,
    PMEMORY_DESCRIPTOR pMemDesc
)
{
    CarveoutMgr *pCarveoutMgr = _memmgrGetCarveoutMgr();

    NV_CHECK_OR_RETURN_VOID(LEVEL_INFO, pMemDesc != NULL);
    if (pCarveoutMgr == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Missing carveout manager while freeing scanout memory\n");
        NV_ASSERT(pCarveoutMgr != NULL);
        pMemDesc->pHeap = NULL;
        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);
        return;
    }

    (void)pGpu;
    (void)pMemoryManager;
    carveoutmgrReleaseMemDesc(pCarveoutMgr, pMemDesc);
}

/**
 * @brief Duplicate memory descriptor allocated from scanout carveout region.
 *
 * This function gets the heap block for the scanout carveout memory descriptor
 * and increase the reference count.
 *
 * @returns OK if success, ERROR otherwise
 */
NV_STATUS
memmgrDuplicateFromScanoutCarveoutRegion_GB10B
(
    OBJGPU             *pGpu,
    MemoryManager      *pMemoryManager,
    PMEMORY_DESCRIPTOR pMemDesc
)
{
    CarveoutMgr *pCarveoutMgr = _memmgrGetCarveoutMgr();
    NvU64 base;

    NV_CHECK_OR_RETURN(LEVEL_INFO, pMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pCarveoutMgr != NULL, NV_ERR_INVALID_STATE);

    base = memdescGetPte(pMemDesc, AT_CPU, 0);
    return carveoutmgrDuplicateHeapBlock(pCarveoutMgr, pGpu, base);
}

/**
 * @brief Return the carveout region information.
 *
 * @returns OK if success, ERROR otherwise
 */
NV_STATUS
memmgrGetCarveoutRegionInfo_KERNEL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NV2080_CTRL_FB_GET_CARVEOUT_REGION_INFO_PARAMS *pParams
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    return pRmApi->Control(pRmApi,
                           pGpu->hInternalClient,
                           pGpu->hInternalSubdevice,
                           NV2080_CTRL_CMD_FB_GET_CARVEOUT_REGION_INFO,
                           pParams,
                           sizeof(*pParams));
}

/*!
 * Allocate console region in CPU-RM from UEFI carveout region.
 */
NV_STATUS
memmgrAllocateConsoleRegion_GB10B
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    NvU32 i;
    NV_STATUS status;
    NV2080_CTRL_FB_GET_CARVEOUT_REGION_INFO_PARAMS infoParams;

    portMemSet(&infoParams, 0, sizeof(infoParams));
    status = memmgrGetCarveoutRegionInfo(pGpu, pMemoryManager, &infoParams);
    if (status != NV_OK)
    {
        return status;
    }

    // Once the console is reserved, we don't expect to reserve it again
    NV_ASSERT_OR_RETURN(pMemoryManager->pReservedConsoleMemDesc == NULL,
                        NV_ERR_STATE_IN_USE);

    for (i = 0; i < infoParams.numCarveoutRegions; i++)
    {
        const NV2080_CTRL_FB_GET_CARVEOUT_REGION_INFO *pRegion =
            &infoParams.carveoutRegion[i];

        if (pRegion->carveoutType ==
            NV2080_CTRL_FB_GET_CARVEOUT_REGION_CARVEOUT_TYPE_UEFI)
        {

            status = memdescCreate(&pMemoryManager->pReservedConsoleMemDesc,
                                   pGpu,
                                   pRegion->size,
                                   RM_PAGE_SIZE_64K,
                                   NV_TRUE,
                                   ADDR_SYSMEM,
                                   NV_MEMORY_UNCACHED,
                                   MEMDESC_FLAGS_SKIP_RESOURCE_COMPUTE |
                                   MEMDESC_FLAGS_ALLOC_FROM_UEFI_CARVEOUT);
            if (status != NV_OK)
            {
                return status;
            }

            memdescDescribe(pMemoryManager->pReservedConsoleMemDesc,
                            ADDR_SYSMEM,
                            pRegion->base,
                            pRegion->size);
            memdescSetPageSize(pMemoryManager->pReservedConsoleMemDesc,
                               AT_CPU,
                               RM_PAGE_SIZE);

            NV_PRINTF(LEVEL_INFO, "Allocating console region of size: %llx, at base : %llx \n ",
                      pRegion->size, pRegion->base);
            break;
        }
    }

    return NV_OK;
}
