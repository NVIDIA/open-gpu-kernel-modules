/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/rm_page_size.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "mem_mgr/fabric_vaspace.h"
#include "mem_mgr/vaspace.h"
#include "os/os.h"

#include "published/rubin/gr100/hwproject.h"

NvBool
memmgrComprMappingSupported_GR100
(
    MemoryManager    *pMemoryManager,
    NV_ADDRESS_SPACE  addrSpace
)
{
    OBJGPU             *pGpu                = ENG_GET_GPU(pMemoryManager);
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, pKernelMemorySystem);

    if (pMemoryManager->bSkipCompressionCheck)
    {
        return NV_TRUE;
    }

    if (pMemorySystemConfig->bDisableCompbitBacking)
    {
        return NV_FALSE;
    }

    return memmgrComprSupported(pMemoryManager, addrSpace);
}

NvBool
memmgrIsFlaSysmemSupported_GR100(OBJGPU *pGpu, MemoryManager *pMemoryManager)
{
    if (pMemoryManager->bForceEnableFlaSysmem)
    {
        return NV_TRUE;
    }

    return kmemsysIsNonPasidAtsSupported_HAL(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));
}

NvU8
memmgrGetLocalizedOffset_GR100
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    return NV_LOCALIZATION_MODE_BIT_IN_ADDRESS_OFFSET;
}

NV_STATUS
memmgrInitSysmemFlaWindowForUvm_GR100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU64          flaBase,
    NvU64          flaSize
)
{
    NV_STATUS          status;
    MEMORY_DESCRIPTOR *pPhysMemDesc = NULL;
    MEMORY_DESCRIPTOR *pFabricMemDesc = NULL;
    FABRIC_VASPACE    *pFabricVAS;
    NvU64              dmaBase = 0;
    NvU64              dmaSize = 0;
    NvBool             bDmaIdentity = NV_FALSE;
    NvU64              dmaBaseAligned = 0;
    NvU64              dmaSizeAligned = 0;
    NvU64              dmaOffset = 0;
    NvBool             bOsWindowInited = NV_FALSE;
    NvBool             bFlaVasAllocated = NV_FALSE;
    NvBool             bFlaVasInited = NV_FALSE;
    VAS_ALLOC_FLAGS    vasFlags = { 0 };

    if (pMemoryManager->sysmemFlaWindow.bSupported)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "FLA sysmem window already initialized, base=0x%llx size=0x%llx\n",
                  pMemoryManager->sysmemFlaWindow.flaBase, pMemoryManager->sysmemFlaWindow.flaSize);
        return NV_ERR_IN_USE;
    }

    NV_CHECK_OR_RETURN(LEVEL_ERROR, flaSize != 0,
                       NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, NV_IS_ALIGNED64(flaBase, RM_PAGE_SIZE_256G),
                       NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, NV_IS_ALIGNED64(flaSize, RM_PAGE_SIZE_256G),
                       NV_ERR_INVALID_ARGUMENT);

    pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pFabricVAS != NULL, NV_ERR_INVALID_STATE);

    // Get DMA range for sysmem
    status = osInitSysmemWindowForFabricAccess(pGpu,
                                               RM_PAGE_SIZE_256G,
                                               &dmaBase,
                                               &dmaSize,
                                               &bDmaIdentity);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to DMA map sysmem for FLA: 0x%x\n", status);
        return status;
    }
    bOsWindowInited = NV_TRUE;

    if (dmaSize == 0)
    {
        NV_PRINTF(LEVEL_ERROR, "OS returned zero-length sysmem DMA window\n");
        status = NV_ERR_INVALID_STATE;
        goto cleanup;
    }

    //
    // In SMMU identity mode, DMA addresses equal physical addresses
    // and we can freely align-down the base and align-up the size.
    // FLA PTEs will simply cover a few extra pages at either end.
    //
    // In dynamic IOVA mode, we request a 256G-aligned base and size
    // via the OS helper and if it didn't, we fail the feature
    // rather than manual alignment of the range.
    //
    if (!bDmaIdentity && !NV_IS_ALIGNED64(dmaBase, RM_PAGE_SIZE_256G))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Dynamic IOVA base 0x%llx not 256G-aligned, FLA for sysmem is not supported\n",
                  dmaBase);
        status = NV_ERR_NOT_SUPPORTED;
        goto cleanup;
    }

    dmaBaseAligned = NV_ALIGN_DOWN64(dmaBase, RM_PAGE_SIZE_256G);
    dmaOffset = dmaBase - dmaBaseAligned;
    dmaSizeAligned = NV_ALIGN_UP64(dmaSize + dmaOffset, RM_PAGE_SIZE_256G);

    if (dmaSizeAligned > flaSize)
    {
        //
        // If the expected DMA mapping size is not covered fully
        // by the FLA range from GFM, we fail the feature.
        //
        NV_PRINTF(LEVEL_ERROR,
                  "FLA range insufficient for local sysmem, DMA size: 0x%llx, FLA size: 0x%llx\n",
                  dmaSizeAligned, flaSize);
        status = NV_ERR_NOT_SUPPORTED;
        goto cleanup;
    }
    else if (dmaSizeAligned < flaSize)
    {
        //
        // At this point, both flaSize and dmaSizeAligned are expected to be 256G aligned.
        // If DMA range is smaller than FLA range(by 256G-sized pages),
        // adjust the FLA size to only what we need.
        //
        flaSize = dmaSizeAligned;
    }

    // Create phys(IOVA) memdesc covering the (aligned)DMA window
    status = memdescCreate(&pPhysMemDesc, pGpu, flaSize, 0, NV_TRUE,
                           ADDR_SYSMEM, NV_MEMORY_CACHED, MEMDESC_FLAGS_NONE);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Phys memdesc creation failed for sysmem: 0x%x\n", status);
        goto cleanup;
    }

    memdescDescribe(pPhysMemDesc, ADDR_SYSMEM, dmaBaseAligned, dmaSizeAligned);
    memdescSetPageSize(pPhysMemDesc, AT_GPU, RM_PAGE_SIZE_256G);

    //
    // Initialize sysmem FLA range here with the updated flaSize which
    // reflects the size of sysmem we need to cover.
    //
    status = fabricvaspaceInitSysmemFlaRange(pFabricVAS, pGpu, flaBase, flaSize);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to initialize sysmem FLA range in fabric VAS, error 0x%x\n",
                  status);
        goto cleanup;
    }
    bFlaVasInited = NV_TRUE;

    // Reserve the fixed FLA VA range in the fabric VAS eheap
    status = fabricvaspaceAllocFixed(pFabricVAS,
                                     RM_PAGE_SIZE_256G,
                                     RM_PAGE_SIZE_256G,
                                     vasFlags,
                                     flaBase,
                                     flaSize);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to alloc FLA VA for sysmem at FLA range: 0x%llx-0x%llx, error 0x%x\n",
                  flaBase, flaBase + flaSize, status);
        goto cleanup;
    }
    bFlaVasAllocated = NV_TRUE;

    // Create fabric memdesc at the reserved FLA VA
    status = memdescCreate(&pFabricMemDesc, pGpu, flaSize, 0, NV_TRUE,
                           ADDR_FABRIC_V2, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Fabric memdesc creation failed for sysmem: 0x%x\n", status);
        goto cleanup;
    }

    memdescSetPte(pFabricMemDesc, AT_GPU, 0, flaBase);
    memdescSetPageSize(pFabricMemDesc, AT_GPU, RM_PAGE_SIZE_256G);

    // Program the GPU FLA PTEs
    status = fabricvaspaceMapPhysMemdesc(pFabricVAS, pFabricMemDesc, 0,
                                         pPhysMemDesc, 0, flaSize, 0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to map FLA to IOVA memdesc for sysmem, error: 0x%x\n",
                  status);
        goto cleanup;
    }

    // Save the state for UVM use
    pMemoryManager->sysmemFlaWindow.dmaBase        = dmaBase;
    pMemoryManager->sysmemFlaWindow.dmaSize        = dmaSize;
    pMemoryManager->sysmemFlaWindow.dmaOffset      = dmaOffset;
    pMemoryManager->sysmemFlaWindow.bDmaIdentity   = bDmaIdentity;
    pMemoryManager->sysmemFlaWindow.flaBase        = flaBase;
    pMemoryManager->sysmemFlaWindow.flaSize        = flaSize;
    pMemoryManager->sysmemFlaWindow.pPhysMemDesc   = pPhysMemDesc;
    pMemoryManager->sysmemFlaWindow.pFabricMemDesc = pFabricMemDesc;
    pMemoryManager->sysmemFlaWindow.bSupported     = NV_TRUE;

    NV_PRINTF(LEVEL_INFO,
              "FLA sysmem window setup: flaBase=0x%llx flaSize=0x%llx "
              "dmaOffset=0x%llx identity=%d\n",
              flaBase, flaSize, dmaOffset, bDmaIdentity);

    return NV_OK;

cleanup:
    if (pFabricMemDesc != NULL)
        memdescDestroy(pFabricMemDesc);

    if (bFlaVasAllocated)
        fabricvaspaceBatchFree(pFabricVAS, &flaBase, 1, 1);

    if (bFlaVasInited)
        fabricvaspaceClearSysmemFlaRange(pFabricVAS);

    if (pPhysMemDesc != NULL)
        memdescDestroy(pPhysMemDesc);

    if (bOsWindowInited)
        osDestroySysmemWindowForFabricAccess(pGpu, dmaBase, dmaSize);

    return status;
}

void
memmgrDestroySysmemFlaWindowForUvm_GR100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    FABRIC_VASPACE *pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);

    if (!pMemoryManager->sysmemFlaWindow.bSupported)
        return;

    if ((pFabricVAS != NULL) &&
        (pMemoryManager->sysmemFlaWindow.pFabricMemDesc != NULL))
    {
        fabricvaspaceUnmapPhysMemdesc(pFabricVAS,
                                      pMemoryManager->sysmemFlaWindow.pFabricMemDesc,
                                      0, pMemoryManager->sysmemFlaWindow.flaSize);

        fabricvaspaceBatchFree(pFabricVAS,
                               &pMemoryManager->sysmemFlaWindow.flaBase, 1, 1);

        fabricvaspaceClearSysmemFlaRange(pFabricVAS);
    }

    if (pMemoryManager->sysmemFlaWindow.pFabricMemDesc != NULL)
    {
        memdescDestroy(pMemoryManager->sysmemFlaWindow.pFabricMemDesc);
        pMemoryManager->sysmemFlaWindow.pFabricMemDesc = NULL;
    }

    if (pMemoryManager->sysmemFlaWindow.pPhysMemDesc != NULL)
    {
        memdescDestroy(pMemoryManager->sysmemFlaWindow.pPhysMemDesc);
        pMemoryManager->sysmemFlaWindow.pPhysMemDesc = NULL;
    }

    osDestroySysmemWindowForFabricAccess(pGpu,
                                         pMemoryManager->sysmemFlaWindow.dmaBase,
                                         pMemoryManager->sysmemFlaWindow.dmaSize);

    portMemSet(&pMemoryManager->sysmemFlaWindow, 0,
               sizeof(pMemoryManager->sysmemFlaWindow));
}
