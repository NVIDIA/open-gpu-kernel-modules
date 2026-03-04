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

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "gpu/bus/kern_bus.h"
#include "gpu_mgr/gpu_mgr.h"
#include "rmapi/mapping_list.h"
#include "published/hopper/gh100/dev_mmu.h"

/*!
 * @brief Setup mappings needed for a BAR1 P2P surface
 *        For BAR1 P2P surface mapping, RM needs to create an internal BAR1 mapping on the target GPU, and
 *        create a ADDR_SYSMEM mapping from the source GPU VA space to this newly created BAR1 region.
 *
 * @param[in]      pGpu                 OBJGPU pointer
 * @param[in]      pDma                 VirtMemAllocator pointer
 * @param[in/out]  params               BAR1 P2P mapping parameters
 *
 * @returns
 *      NV_OK if it creates the BAR1 P2P surface.
 */
NV_STATUS
dmaAllocBar1P2PMapping_GH100
(
    OBJGPU *pGpu,
    VirtMemAllocator *pDma,
    DMA_BAR1P2P_MAPPING_PRARAMS *params
)
{
    MEMORY_DESCRIPTOR *pBar1P2PVirtMemDesc = NULL;           // The peer GPU BAR1 region mapped for BAR1 P2P
    MEMORY_DESCRIPTOR *pBar1P2PPhysMemDesc = NULL;           // The peer GPU vidmem sub region
    NvU64 phyAddr = 0;                                       // BAR1 offset of SPA addresses
    NvU64 bar1PhyAddr = 0;                                   // BAR1 aperture SPA addresses
    NvU64 bar1ApertureLen = 0;                               // BAR1 aperture mapped lengths
    OBJGPU *pPeerGpu = NULL;
    KernelBus *pPeerKernelBus = NULL;
    NV_STATUS status = NV_OK;

    if (params == NULL ||
        params->pVas == NULL ||
        params->pPeerGpu == NULL ||
        params->pPeerMemDesc == NULL ||
        params->pDmaMappingInfo == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pPeerGpu = params->pPeerGpu;
    pPeerKernelBus = GPU_GET_KERNEL_BUS(pPeerGpu);

    //
    // Step 1:
    // Create sub memory descriptor from the memory source (on target GPU) that need to map.
    //
    status = memdescCreateSubMem(&pBar1P2PPhysMemDesc, params->pPeerMemDesc, pPeerGpu, params->offset, params->length);
    if (status != NV_OK)
        return status;

    bar1ApertureLen = params->length;

    //
    // Step 2:
    // Create BAR1 mapping with MAP_BAR1_P2P flag on the target GPU.
    //
    status = kbusMapFbApertureSingle(pPeerGpu, pPeerKernelBus,
                                     pBar1P2PPhysMemDesc,
                                     0,
                                     &phyAddr,
                                     &bar1ApertureLen,
                                     BUS_MAP_FB_FLAGS_MAP_UNICAST,
                                     NULL);
    if (status != NV_OK)
        goto cleanup;

    bar1PhyAddr = gpumgrGetGpuPhysFbAddr(pPeerGpu) + phyAddr;

    NV_PRINTF(LEVEL_INFO, "bar1p2p surface mapped at bar1PhyAddr 0x%llx, len 0x%llx\n",
                           bar1PhyAddr, bar1ApertureLen);

    //
    // Step 3:
    // Create pGpu memory descriptor to describe the target BAR1 P2P region.
    // Note: This sysmem mapping to IO device memory needs to be UNCACHED, non-coherent
    //
    status = memdescCreate(&pBar1P2PVirtMemDesc,
                           pPeerGpu,
                           bar1ApertureLen,
                           0,
                           NV_MEMORY_CONTIGUOUS,
                           ADDR_SYSMEM,
                           NV_MEMORY_UNCACHED,
                           MEMDESC_FLAGS_NONE);
    if (status != NV_OK)
        goto cleanup;

    // Copy the page size from the target/peer vidmem
    memdescSetPageSize(pBar1P2PVirtMemDesc, VAS_ADDRESS_TRANSLATION(params->pVas),
                       memdescGetPageSize(pBar1P2PPhysMemDesc, VAS_ADDRESS_TRANSLATION(params->pVas)));

    // Setup GMK PTE type for this memory
    memdescSetPteKind(pBar1P2PVirtMemDesc, NV_MMU_PTE_KIND_GENERIC_MEMORY);

    // Set the memory address and memory type as a SYSMEM
    memdescDescribe(pBar1P2PVirtMemDesc, ADDR_SYSMEM, (RmPhysAddr)bar1PhyAddr, bar1ApertureLen);

    params->pDmaMappingInfo->pBar1P2PVirtMemDesc = pBar1P2PVirtMemDesc;
    params->pDmaMappingInfo->pBar1P2PPhysMemDesc = pBar1P2PPhysMemDesc;

    // Save the MemDesc to be the newly created sysmem in the peer BAR1 aperture
    params->pMemDescOut = pBar1P2PVirtMemDesc;

    // The sysmem MemDesc already added the requested offset. Set the offset to 0
    params->offsetOut = 0;

    // Override Mapping flags with PHYSICAL and no snoop
    params->flagsOut = params->flags;
    params->flagsOut = FLD_SET_DRF(OS46, _FLAGS, _PAGE_KIND, _PHYSICAL, params->flagsOut);
    params->flagsOut = FLD_SET_DRF(OS46, _FLAGS, _CACHE_SNOOP, _DISABLE, params->flagsOut);
    // It is a normal SYSMEM mapping from source GPU VA to the target GPU BAR1 region
    params->flagsOut = FLD_SET_DRF(OS46, _FLAGS, _P2P_ENABLE, _NO, params->flagsOut);

    // Update the flags in the pDmaMappingInfo
    params->pDmaMappingInfo->Flags = params->flagsOut;

    return NV_OK;;

cleanup:
    if (phyAddr != 0)
    {
        kbusUnmapFbApertureSingle(pPeerGpu, pPeerKernelBus,
                                  params->pPeerMemDesc,
                                  phyAddr,
                                  bar1ApertureLen,
                                  BUS_MAP_FB_FLAGS_MAP_UNICAST);
    }

    memdescDestroy(pBar1P2PPhysMemDesc);

    memdescDestroy(pBar1P2PVirtMemDesc);

    NV_PRINTF(LEVEL_ERROR, "Failed to create bar1p2p mapping\n");

    return status;
}

/*!
 * @brief Free the resources for a BAR1 P2P surface
 *
 * @param[in]  pDmaMappingInfo      The Dma mapping info structure.
 *
 */
void
dmaFreeBar1P2PMapping_GH100
(
    VirtMemAllocator *pDma,
    CLI_DMA_MAPPING_INFO *pDmaMappingInfo
)
{
    if (pDmaMappingInfo == NULL)
        return;

    if (pDmaMappingInfo->pBar1P2PPhysMemDesc != NULL &&
        pDmaMappingInfo->pBar1P2PVirtMemDesc != NULL)
    {
        OBJGPU *pPeerGpu = pDmaMappingInfo->pBar1P2PPhysMemDesc->pGpu;
        KernelBus *pPeerKernelBus = GPU_GET_KERNEL_BUS(pPeerGpu);
        NvU64   bar1PhysAddr = memdescGetPhysAddr(pDmaMappingInfo->pBar1P2PVirtMemDesc, AT_CPU, 0);
        NvU64   bar1MapSize  = memdescGetSize(pDmaMappingInfo->pBar1P2PVirtMemDesc);

        // Unmap the BAR1 mapping
        kbusUnmapFbApertureSingle(pPeerGpu, pPeerKernelBus,
                                  pDmaMappingInfo->pBar1P2PPhysMemDesc,
                                  bar1PhysAddr,
                                  bar1MapSize,
                                  BUS_MAP_FB_FLAGS_MAP_UNICAST);

        NV_PRINTF(LEVEL_INFO, "bar1p2p surface UN-mapped at 0x%llx + 0x%llx\n", bar1PhysAddr, bar1MapSize);

        // Destroy the source memory descriptor
        memdescDestroy(pDmaMappingInfo->pBar1P2PPhysMemDesc);

        memdescDestroy(pDmaMappingInfo->pBar1P2PVirtMemDesc);
    }
}
