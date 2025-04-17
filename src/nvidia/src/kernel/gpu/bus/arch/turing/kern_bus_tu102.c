/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/bus/kern_bus.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "vgpu/vgpu_events.h"
#include "gpu/fifo/kernel_fifo.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "nvrm_registry.h"

#include "published/turing/tu102/dev_bus.h"
#include "published/turing/tu102/dev_vm.h"
#include "published/turing/tu102/dev_mmu.h"

/*!
 * @brief Returns the first available peer Id excluding the nvlink peerIds
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns NvU32 first free peer Id
 */
NvU32
kbusGetUnusedPciePeerId_TU102
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NvU32         nvlinkIdMask  = 0;
    NvU32         peerId;

    if ((pKernelNvlink != NULL) &&
        (pKernelNvlink->getProperty(pKernelNvlink,
                        PDB_PROP_KNVLINK_WAR_BUG_3471679_PEERID_FILTERING)))
    {
        //
        // Get the mask of NvLink peerIds, to exclude them from the
        // peerIds PCIE P2P is using.
        // Pre-Ampere GPUs use a static peerId assignment reserved in
        // busGetNvlinkP2PPeerId_GP100() and we need to make sure the
        // PCIE and nvLink P2P assignments do not collide.
        // Make this Windows + Turing only until bug 3471679 is fixed.
        //
        nvlinkIdMask = knvlinkGetUniquePeerIdMask_HAL(pGpu, pKernelNvlink);
    }

    for (peerId = 0; peerId < pKernelBus->numPeers; peerId++)
    {
        if ((pKernelBus->p2pPcie.busPeer[peerId].refCount == 0) &&
            (!pKernelBus->p2pPcie.busPeer[peerId].bReserved) &&
            ((BIT(peerId) & nvlinkIdMask) == 0))
        {
            return peerId;
        }
    }
    return BUS_INVALID_PEER;
}

/*!
 * Previous bind sequence would just do a sysmembar after a flush.
 * Now the flushes don't guarantee anything for the BIND itself.
 * Just that previous reads/writes are complete.
 * We need to use the BIND_STATUS register now.
 * New procedure:
 * - Write NV_PBUS_BLOCK_(BAR1|BAR2|IFB)
 * - Poll NV_PBUS_BIND_STATUS to make sure the BIND completed.
 */
NV_STATUS
kbusBindBar2_TU102
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    BAR2_MODE  bar2Mode
)
{
    NvU32             gfid;
    RMTIMEOUT         timeout;
    NvU32             temp;
    NvU32             value;
    NvU32             instBlkAperture = 0;
    NvU64             instBlkAddr     = 0;
    NV_STATUS         status          = NV_OK;
    NvBool            bIsModePhysical;
    MEMORY_DESCRIPTOR *pMemDesc;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    if (pKernelBus->bar2[gfid].bBootstrap &&
        (NULL != pKernelBus->bar2[gfid].pInstBlkMemDescForBootstrap) &&
        kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
    {
        pMemDesc = pKernelBus->bar2[gfid].pInstBlkMemDescForBootstrap;
    }
    else
    {
        pMemDesc = pKernelBus->bar2[gfid].pInstBlkMemDesc;
    }

    //
    // Bind BAR2 to virtual. Carefully.  We have not initialized PTEs yet. We will first
    // map the BAR2 PTEs into BAR2. This allows us to use the BAR2 interface to invalidate
    // the rest of the BAR2 PTEs.  WC memory writes are faster than single BAR0 writes
    // and this matters for RTL sim and emulation. DEBUG_CYA = OFF keeps the VBIOS
    // aperture in physical addressing.
    //
    bIsModePhysical = (BAR2_MODE_PHYSICAL == bar2Mode);

    if (!bIsModePhysical)
    {
        instBlkAperture = kgmmuGetHwPteApertureFromMemdesc(GPU_GET_KERNEL_GMMU(pGpu), pMemDesc);
        instBlkAddr     = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
    }

    value = ((bIsModePhysical ? DRF_DEF(_PBUS, _BAR2_BLOCK, _TARGET, _VID_MEM) :
                                DRF_NUM(_PBUS, _BAR2_BLOCK, _TARGET, instBlkAperture)) |
             (bIsModePhysical ? DRF_DEF(_PBUS, _BAR2_BLOCK, _MODE, _PHYSICAL) :
                                DRF_DEF(_PBUS, _BAR2_BLOCK, _MODE, _VIRTUAL)) |
             (bIsModePhysical ? DRF_NUM(_PBUS, _BAR2_BLOCK, _PTR, 0x0) :
                                DRF_NUM(_PBUS, _BAR2_BLOCK, _PTR,
                                        NvU64_LO32(instBlkAddr >> GF100_BUS_INSTANCEBLOCK_SHIFT))) |
             DRF_DEF(_PBUS, _BAR2_BLOCK, _DEBUG_CYA, _OFF));

    {
        GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_BAR2_BLOCK, value);
    }


    osFlushCpuWriteCombineBuffer();

    // Skip the wait if we are in the reset path (GPU most likely in a bad state)
    if (!IS_VIRTUAL(pGpu) && API_GPU_IN_RESET_SANITY_CHECK(pGpu))
    {
        return status;
    }

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    do
    {
        //
        // To avoid deadlocks and non-deterministic virtual address
        // translation behavior, after writing BAR2_BLOCK to bind BAR2 to a
        // virtual address space, SW must ensure that the bind has completed
        // prior to issuing any further BAR2 requests by polling for both
        // NV_PBUS_BIND_STATUS_BAR2_PENDING to return to EMPTY and
        // NV_PBUS_BIND_STATUS_BAR2_OUTSTANDING to return to FALSE.
        //
        // BAR2_PENDING indicates a Bar2 bind is waiting to be sent.
        // BAR2_OUTSTANDING indicates a Bar2 bind is outstanding to FB.
        //
        {
            temp = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_BIND_STATUS);
        }
        if (FLD_TEST_DRF(_PBUS, _BIND_STATUS, _BAR2_PENDING, _EMPTY, temp) &&
            FLD_TEST_DRF(_PBUS, _BIND_STATUS, _BAR2_OUTSTANDING, _FALSE, temp))
        {
            status = NV_OK;
            break;
        }

        if (status == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "timed out waiting for bar2 binding to complete\n");
            DBG_BREAKPOINT();
            break;
        }

        status = gpuCheckTimeout(pGpu, &timeout);
        osSpinLoop();
    } while (1);

    return status;
}

NvU64 kbusGetCpuInvisibleBar2BaseAdjust_TU102
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    const GMMU_FMT* pFmt = NULL;

    NV_ASSERT_OR_RETURN(pKernelGmmu != NULL, 0);
    pFmt = kgmmuFmtGetLatestSupportedFormat(pGpu, pKernelGmmu);
    NV_ASSERT_OR_RETURN(pFmt != NULL, 0);

    //
    // In RM-offload scenario, Kernel RM and Physical RM use their own GPU VA space respectively.
    //
    // The expectation is that the Physical RM base starts from the second PD entry of the topmost PD.
    //
    return mmuFmtEntryIndexVirtAddrLo(pFmt->pRoot, 0, 1 /* PD0[1] */);
}

/*!
 * Previous bind sequence would just do a sysmembar after a flush.
 * Now the flushes don't guarantee anything for the BIND itself.
 * Just that previous reads/writes are complete.
 * We need to use the BIND_STATUS register now.
 * New procedure:
 * - Write NV_PBUS_BLOCK_(BAR1|BAR2|IFB)
 * - Poll NV_PBUS_BIND_STATUS to make sure the BIND completed.
 */
NV_STATUS
kbusBar1InstBlkBind_TU102
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    KernelGmmu       *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NvU32             gfid;
    NvU32             target;
    NvU32             temp;
    NvU32             ptr;
    RMTIMEOUT         timeout;
    NV_STATUS         status = NV_OK;
    NvBool            bIsModePhysical = NV_FALSE;
    NvBool            bBrokenFb = NV_FALSE;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    // Nothing to be done in guest in the paravirtualization case.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_OK;
    }

    switch (kgmmuGetMemAperture(pKernelGmmu, pKernelBus->bar1[gfid].pInstBlkMemDesc))
    {
        case GMMU_APERTURE_VIDEO:
            target = NV_PBUS_BAR1_BLOCK_TARGET_VID_MEM;
            break;
        case GMMU_APERTURE_SYS_COH:
            target = NV_PBUS_BAR1_BLOCK_TARGET_SYS_MEM_COHERENT;
            break;
        case GMMU_APERTURE_SYS_NONCOH:
            target = NV_PBUS_BAR1_BLOCK_TARGET_SYS_MEM_NONCOHERENT;
            break;
        default:
            NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_STATE);
    }
    ptr = NvU64_LO32(pKernelBus->bar1[gfid].instBlockBase >> GF100_BUS_INSTANCEBLOCK_SHIFT);

    bIsModePhysical = kbusIsBar1PhysicalModeEnabled(pKernelBus);
    bBrokenFb = pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB);

    {
        GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_BAR1_BLOCK,
                (((bIsModePhysical && !bBrokenFb) ?
                                    DRF_DEF(_PBUS, _BAR1_BLOCK, _TARGET, _VID_MEM) :
                                    DRF_NUM(_PBUS, _BAR1_BLOCK, _TARGET, target)) |
                 (bIsModePhysical ? DRF_DEF(_PBUS, _BAR1_BLOCK, _MODE, _PHYSICAL) :
                                    DRF_DEF(_PBUS, _BAR1_BLOCK, _MODE, _VIRTUAL)) |
                 (bIsModePhysical ? DRF_NUM(_PBUS, _BAR1_BLOCK, _PTR, 0x0) :
                                    DRF_NUM(_PBUS, _BAR1_BLOCK, _PTR, ptr))));
    }

    osFlushCpuWriteCombineBuffer();

    // Skip the wait if we are in the reset path (GPU most likely in a bad state)
    if (!IS_VIRTUAL(pGpu) && API_GPU_IN_RESET_SANITY_CHECK(pGpu))
    {
        return status;
    }

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    do
    {
        //
        // To avoid deadlocks and non-deterministic virtual address
        // translation behavior, after writing BAR1_BLOCK to bind BAR1 to a
        // virtual address space, SW must ensure that the bind has completed
        // prior to issuing any further BAR1 requests by polling for both
        // NV_PBUS_BIND_STATUS_BAR1_PENDING to return to EMPTY and
        // NV_PBUS_BIND_STATUS_BAR1_OUTSTANDING to return to FALSE.
        //
        // BAR1_PENDING indicates a Bar1 bind is waiting to be sent.
        // BAR1_OUTSTANDING indicates a Bar1 bind is outstanding to FB.
        //
        {
            temp = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_BIND_STATUS);
        }
        if (FLD_TEST_DRF(_PBUS, _BIND_STATUS, _BAR1_PENDING, _EMPTY, temp) &&
            FLD_TEST_DRF(_PBUS, _BIND_STATUS, _BAR1_OUTSTANDING, _FALSE, temp))
        {
            status = NV_OK;
            break;
        }

        if (status == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "timed out waiting for bar1 binding to complete\n");
            DBG_BREAKPOINT();
            break;
        }
        status = gpuCheckTimeout(pGpu, &timeout);
        osSpinLoop();
    } while (1);

    return status;
}

/*!
 * @brief Returns BAR1 VA Size that needs to be reserved for BSOD Surface
 *
 *
 * @returns BAR1 VA Size that needs to be reserved
 */
NvU64
kbusGetBar1ResvdVA_TU102
(
    KernelBus *pKernelBus
)
{
    //
    // Turing+ supports 8K displays which needs at least 127MB of BAR1 VA with
    // 64KB/Big page size (used by KMD). Hence, doubling the prev size of 64MB
    //
    return NVBIT64(27); //128MB
}


/**
 * @brief Check if the GPU supports static BAR1
 *
 * @param pGpu
 * @param pKernelBus
 * @param gfid
 *
 * @return NV_OK if it supports static BAR1
 *         NV_ERR_NOT_SUPPORTED if not supported. This is an expected state
 *         NV_ERR_INVALID_REGISTRY_KEY if insufficient BAR1 for the FB, but
 *                             the regkeys specified are still attempting to
 *                             force it
 */
NV_STATUS
kbusIsStaticBar1Supported_TU102
(
    OBJGPU     *pGpu,
    KernelBus  *pKernelBus,
    NvU32       gfid,
    NvU64       consoleSize,
    NvU64       mailboxSize
)
{
    NvU64 bar1Size = kbusGetPciBarSize(pKernelBus, 1);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    //
    // Use fbAddrSpaceSizeMb in this function to make sure there's
    // enough space for the any possible allocations in the RM
    // reserved region to be mapped through the dynamic section of
    // BAR1
    //
    NvU64 fbSize            = pMemoryManager->Ram.fbAddrSpaceSizeMb << 20;
    NvU64 fbSizeAligned     = RM_ALIGN_UP(fbSize, RM_PAGE_SIZE_2M);
    NvU64 bar1VASize        = pKernelBus->bar1[gfid].mappableLength;
    NvU64 bar1VASizeAligned = RM_ALIGN_DOWN(bar1VASize, RM_PAGE_SIZE_2M);

    //
    // GPU BAR1 supports the SYSMEM mapping for the bar1 doorbell, RM needs
    // to make sure that BAR1 VA has 128KB space left for such cases after all
    // FB statically mapped in BAR1. Bug 3869651 #14.
    // 64KB for doorbell
    // 64KB for MMIO priv
    //
    NvU64 doorbellAndMmioPrivSize = 32 * RM_PAGE_SIZE;

    if (gfid != 0)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if ((fbSize == 0) || (bar1Size == 0))
        return NV_ERR_NOT_SUPPORTED;

    switch (pKernelBus->staticBar1ForceType)
    {
        case NV_REG_STR_RM_FORCE_STATIC_BAR1_DISABLE:
            return NV_ERR_NOT_SUPPORTED;
        case NV_REG_STR_RM_FORCE_STATIC_BAR1_ENABLE:
            {
                //
                // Only check for at least client-visible FB size on the assumption the user
                // really wants to enable static BAR1 regardless of the auto checks
                //
                NvU64 bar1MapSize =
                    RM_ALIGN_UP(memmgrGetClientFbAddrSpaceSize(pGpu, pMemoryManager),
                                RM_PAGE_SIZE_2M);

                if (bar1VASizeAligned < bar1MapSize)
                {
                    NV_PRINTF(LEVEL_ERROR, "BAR1 size %lld is not large enough to map FB size"
                                           "%lld to force static BAR1\n",
                                            bar1VASizeAligned, bar1MapSize);
                    DBG_BREAKPOINT();

                    return NV_ERR_INVALID_REGISTRY_KEY;
                }

                return NV_OK;
            }
        case NV_REG_STR_RM_FORCE_STATIC_BAR1_AUTO:
            {
                //
                // Auto-enable if there is enough space to map
                // + all of FB once
                // + enough to reserve enough space for userD BAR mappings
                //       for every channel in the system
                //       (whether userD is allocated by RM or by the user)
                // + doorbell mapping mapped once globally (64KB)
                // + mmio priv mapped once globally (64KB)
                // + console reservation
                // + mailbox BAR1 as fallback P2P until it is disabled
                //
                NvU32 userdSize = 0;
                NvU32 numChannels = kfifoGetMaxChannelsInSystem(pGpu, pKernelFifo);
                NvU64 requiredAutoBar1Size = fbSizeAligned;

                kfifoGetUserdSizeAlign_HAL(pKernelFifo, &userdSize, NULL);

                userdSize *= numChannels;

                requiredAutoBar1Size += userdSize;
                requiredAutoBar1Size += doorbellAndMmioPrivSize;
                requiredAutoBar1Size += consoleSize;
                requiredAutoBar1Size += mailboxSize;

                if (bar1VASizeAligned >= requiredAutoBar1Size)
                {
                    NV_PRINTF(LEVEL_ERROR, "Enabling static BAR1 automatically!\n");
                    return NV_OK;
                }
                else
                {
                    return NV_ERR_NOT_SUPPORTED;
                }
            }
        default:
            return NV_ERR_INVALID_REGISTRY_KEY;
    }
}

/*!
 * @brief Setup static Bar1 mapping.
 *
 * @param[in]   pGpu                GPU pointer
 * @param[in]   pKernelBus          Kernel bus pointer
 * @param[in]   gfid                The GFID
 *
 * @returns NV_OK on success, or rm_status from called functions on failure.
 */
NV_STATUS
kbusEnableStaticBar1Mapping_TU102
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus,
    NvU32 gfid
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    MEMORY_DESCRIPTOR *pDmaMemDesc = NULL;
    NV_STATUS status = NV_OK;
    NvU64 bar1Offset = 0;
    NvU64 bar1MapSize;
    NvU64 bar1BusAddr;

    //
    // But use memmgrGetClientFbAddrSpaceSize
    // in this function to only map the client-visible FB
    //

    // align to 2MB page size
    bar1MapSize = RM_ALIGN_UP(memmgrGetClientFbAddrSpaceSize(pGpu, pMemoryManager),
                              RM_PAGE_SIZE_2M);

    //
    // The static mapping is not backed by an allocated physical FB.
    // Here RM describes the memory for the static mapping.
    //
    NV_ASSERT_OK_OR_RETURN(memdescCreate(&pMemDesc, pGpu, bar1MapSize, 0,
                                         NV_MEMORY_CONTIGUOUS, ADDR_FBMEM,
                                         NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE));

    memdescDescribe(pMemDesc, ADDR_FBMEM, 0, bar1MapSize);

    // Set to use RM_PAGE_SIZE_HUGE, 2MB
    memdescSetPageSize(pMemDesc, AT_GPU, RM_PAGE_SIZE_HUGE);

    pKernelBus->staticBar1DefaultKind = NV_MMU_PTE_KIND_GENERIC_MEMORY;

    // Setup GMK PTE type for this memory
    memdescSetPteKind(pMemDesc, pKernelBus->staticBar1DefaultKind);

    // Deploy the static mapping.
    NV_ASSERT_OK_OR_GOTO(status,
        kbusMapFbApertureSingle(pGpu, pKernelBus, pMemDesc, 0,
            &bar1Offset, &bar1MapSize,
            BUS_MAP_FB_FLAGS_MAP_UNICAST,
            NV01_NULL_OBJECT),
        cleanup_mem);

    // Get the system physical address of the Bar1
    bar1BusAddr = gpumgrGetGpuPhysFbAddr(pGpu) + bar1Offset;

    //
    // Create a memory descriptor to describe a SYSMEM target of the GPU
    // BAR1 region. This memDesc will be used for P2P DMA related mapping.
    //
    NV_ASSERT_OK_OR_GOTO(status,
                         memdescCreate(&pDmaMemDesc,
                                       pGpu,
                                       bar1MapSize,
                                       0,
                                       NV_MEMORY_CONTIGUOUS,
                                       ADDR_SYSMEM,
                                       NV_MEMORY_UNCACHED,
                                       MEMDESC_FLAGS_NONE),
                        cleanup_bus_map);

    memdescDescribe(pDmaMemDesc, ADDR_SYSMEM, bar1BusAddr, bar1MapSize);

    pKernelBus->bar1[gfid].bStaticBar1Enabled = NV_TRUE;
    pKernelBus->bar1[gfid].staticBar1.pVidMemDesc = pMemDesc;
    pKernelBus->bar1[gfid].staticBar1.pDmaMemDesc = pDmaMemDesc;
    pKernelBus->bar1[gfid].staticBar1.startOffset = bar1Offset;
    pKernelBus->bar1[gfid].staticBar1.size = bar1MapSize;

    NV_PRINTF(LEVEL_INFO, "Static bar1 mapped offset 0x%llx size 0x%llx\n",
                           bar1Offset, bar1MapSize);

    return NV_OK;

cleanup_bus_map:
    NV_ASSERT_OK(kbusUnmapFbApertureSingle(pGpu, pKernelBus,
                                           pMemDesc, bar1Offset, bar1MapSize,
                                           BUS_MAP_FB_FLAGS_MAP_UNICAST));

cleanup_mem:
    NV_PRINTF(LEVEL_ERROR, "Failed to create the static bar1 mapping offset"
                           "0x%llx size 0x%llx\n", bar1Offset, bar1MapSize);

    pKernelBus->bar1[gfid].bStaticBar1Enabled = NV_FALSE;
    pKernelBus->bar1[gfid].staticBar1.pVidMemDesc = NULL;
    pKernelBus->bar1[gfid].staticBar1.pDmaMemDesc = NULL;

    memdescDestroy(pDmaMemDesc);
    memdescDestroy(pMemDesc);

    return status;
}

/*!
 * @brief tear down static Bar1 mapping.
 *
 * @param[in]   pGpu                GPU pointer
 * @param[in]   pKernelBus          Kernel bus pointer
 * @param[in]   gfid                The GFID
 *
 * @returns NV_OK on success, or rm_status from called functions on failure.
 */
void
kbusDisableStaticBar1Mapping_TU102
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus,
    NvU32 gfid
)
{
    if (!pKernelBus->bar1[gfid].bStaticBar1Enabled)
        return;

    pKernelBus->bar1[gfid].bStaticBar1Enabled = NV_FALSE;

    NV_ASSERT_OK(
        kbusUnmapFbApertureSingle(pGpu, pKernelBus,
            pKernelBus->bar1[gfid].staticBar1.pVidMemDesc,
            pKernelBus->bar1[gfid].staticBar1.startOffset,
            pKernelBus->bar1[gfid].staticBar1.size,
            BUS_MAP_FB_FLAGS_MAP_UNICAST));

    memdescDestroy(pKernelBus->bar1[gfid].staticBar1.pVidMemDesc);
    pKernelBus->bar1[gfid].staticBar1.pVidMemDesc = NULL;

    memdescDestroy(pKernelBus->bar1[gfid].staticBar1.pDmaMemDesc);
    pKernelBus->bar1[gfid].staticBar1.pDmaMemDesc = NULL;
}

/*!
 * @brief To update the StaticBar1 PTE kind for the specified memory.
 *
 *        The staticbar1 only support GMK and the compressed kind PTE.
 *        By default, the bar1 is statically mapped with GMK at boot when the
 *        static bar1 is enabled.
 *
 *        When to map a uncompressed kind memory, RM just return the static
 *        bar1 address which is mapped to the specified memory.
 *
 *        When to map a non-GMK kind memory of page size >=2MB, RM must call this
 *        function to change the static mapped bar1 range to the specified memory
 *        from GMK to the non-GMK kind. And RM needs to call this function to
 *        change it back to GMK from the compressed kind after this mapping is
 *        released.
 * 
 *        If a non-GMK mapping is page size <=2MB, it will be placed in the
 *        dynamic region instead.
 * 
 *        If an allocation has different DMA mapping flags, currently it will
 *        also be placed in the dynmaic region instead by the logic in
 *        kbusIncreaseStaticBar1Refcount_TU102. This can be relaxed later
 *        as needed by that function with no changes to this function by
 *        passing in the appropriate dmaMapFlags to this function.
 *        
 * kbusIncreaseStaticBar1Refcount_TU102
 *
 * @param[in]   pGpu            GPU pointer
 * @param[in]   pKernelBus      Kernel bus pointer
 * @param[in]   pMemDesc        The memory to update
 * @param[in]   offset          The offset of the memory to update
 * @param[in]   length          The length of the memory to update
 * @param[in]   bRelease        Call to release the mapping
 * @param[in]   gfid            The GFID
 *
 * return NV_OK on success
 */
static
NV_STATUS
_kbusUpdateStaticBar1VAMapping_TU102
(
    OBJGPU             *pGpu,
    KernelBus          *pKernelBus,
    MEMORY_DESCRIPTOR  *pMemDesc,
    NvU32               dmaMapFlags,
    NvBool              bRelease
)
{
    NV_STATUS           status = NV_OK;
    VirtMemAllocator   *pDma = GPU_GET_DMA(pGpu);
    MemoryManager      *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    OBJVASPACE         *pVAS;
    NvU32               kind;
    NvU64               physAddr;
    NvU64               vaLo;
    NvU64               vaHi;
    NvU64               pageSize;
    COMPR_INFO          comprInfo = {0};
    NvU32               gfid;
    NvU64               mapSize;
    NvU64               mapGranularity;
    NvU64               offset;
    ADDRESS_TRANSLATION addressTranslation;

    NV_ASSERT_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK,
                        NV_ERR_INVALID_STATE);

    NV_ASSERT_OR_RETURN(pMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN(memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM,
                        NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)),
                        NV_ERR_INVALID_LOCK_STATE);

    pVAS = pKernelBus->bar1[gfid].pVAS;
    addressTranslation = VAS_ADDRESS_TRANSLATION(pVAS);
    pageSize = memdescGetPageSize(pMemDesc, addressTranslation);
    mapSize  = memdescGetSize(pMemDesc);

    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(
                        memdescGetPhysAddr(pMemDesc, addressTranslation, 0), pageSize),
                        NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(mapSize, pageSize),
                        NV_ERR_INVALID_ARGUMENT);

    // TODO: simplify for dynamic page granularity
    mapGranularity = memdescGetContiguity(pMemDesc, addressTranslation) ?
                                            mapSize : pageSize;

    NV_ASSERT_OK_OR_RETURN(memmgrGetKindComprFromMemDesc(pMemoryManager,
                               pMemDesc, 0, &kind, &comprInfo));

    // Static BAR1 mapping only support >=2MB page size
    NV_CHECK_OR_RETURN(LEVEL_SILENT,
        (bRelease || pageSize >= RM_PAGE_SIZE_HUGE),
        NV_ERR_NOT_SUPPORTED);

    if (bRelease)
    {
        // update the PTE kind to be the uncompressed kind
        comprInfo.kind = memmgrGetUncompressedKind_HAL(pGpu, pMemoryManager,
                                                       kind, NV_FALSE);
    }

    for (offset = 0; offset < mapSize; offset += mapGranularity)
    {
        // Under static BAR1 mapping, BAR1 VA equal to fb physAddr plus startOffset
        physAddr = memdescGetPhysAddr(pMemDesc, addressTranslation, offset);
        vaLo     = physAddr + pKernelBus->bar1[gfid].staticBar1.startOffset;
        vaHi     = vaLo + mapGranularity - 1;

        //
        // Note: dmaUpdateVASpace_HAL uses lower level flags than NVOS46_FLAGS.
        // Invoking dmaUpdateVASpace_HAL directly here misses all the handling
        // that dmaAllocMapping_HAL does to do additional handling of the 
        // NVOS46_FLAGS into internal dma flags.
        // These flags should influence the dmaMapFlags and not be allowed through
        // this path. When we add such parsing, we can use
        // DMA_UPDATE_VASPACE_FLAGS_UPDATE_ALL here to not have to read back
        // the page tables
        //
        status = dmaUpdateVASpace_HAL(pGpu, pDma, pVAS,
                                      pMemDesc, NULL,
                                      vaLo, vaHi,
                                      DMA_UPDATE_VASPACE_FLAGS_UPDATE_KIND |
                                      DMA_UPDATE_VASPACE_FLAGS_UPDATE_COMPR,
                                      NULL, 0,                            
                                      &comprInfo, 0,                           
                                      NV_MMU_PTE_VALID_TRUE,
                                      NV_MMU_PTE_APERTURE_VIDEO_MEMORY,
                                      BUS_INVALID_PEER,
                                      NVLINK_INVALID_FABRIC_ADDR,
                                      DMA_TLB_INVALIDATE,
                                      NV_FALSE,
                                      pageSize);

    }

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to update static bar1 VA space, error 0x%x.\n",
                  status);

        //
        // Callers expect NV_ERR_NOT_SUPPORTED under normal operation, so
        // don't let it alias
        // 
        if (status == NV_ERR_NOT_SUPPORTED)
        {
            status = NV_ERR_INVALID_STATE;
        }
    }

    return status;
}

/*!
 * @brief Increase the refcount on the staticBar1 mapped
 *        per memdesc or return error if it's already in use
 *        and the mapping should be made in the dynamic region.
 *        Automatically updates compression mapping.
 *
 * @param[in]   pGpu            GPU pointer
 * @param[in]   pKernelBus      Kernel bus pointer
 * @param[in]   pMemDesc        The memory to update
 *
 * return NV_OK on success
 * return NV_ERR_NOT_SUPPORTED if staticBar1 not enabled or the memdesc should
 *                             otherwise go in the dynamic region
 * return other on error
 */
NV_STATUS kbusIncreaseStaticBar1Refcount_TU102
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32 busMapFlags
)
{
    NvU32               requestedKind;
    NvU64               rootOffset;
    MEMORY_DESCRIPTOR  *pRootMemDesc;
    NV_STATUS           status = NV_OK;
    NvU32 requestedDmaFlags;

    NV_CHECK_OR_RETURN(LEVEL_SILENT, kbusIsStaticBar1Enabled(pGpu, pKernelBus),
                        NV_ERR_NOT_SUPPORTED);

    requestedKind = memdescGetPteKind(pMemDesc);
    pRootMemDesc  = memdescGetRootMemDesc(pMemDesc, &rootOffset);

    requestedDmaFlags = kbusConvertBusMapFlagsToDmaFlags(pKernelBus, pMemDesc, busMapFlags);

    //
    // If the mapping kind doesn't match, allow updating it on first reference.
    // If the mapping dmaFlags don't match, don't allow updates at all since
    // static BAR1 does not handle all such updates/offsets/etc.
    //
    if ((pRootMemDesc->staticBar1MappingRefCount != 0 &&
        requestedKind != pRootMemDesc->staticBar1MappingKind) ||
        requestedDmaFlags != pRootMemDesc->staticBar1DmaFlags)
    {
        //
        // The mapping is being used with a different kind
        // or different mapping flags from the default
        //
        return NV_ERR_IN_USE;
    }

    if (requestedKind != pKernelBus->staticBar1DefaultKind)
    {
        status = _kbusUpdateStaticBar1VAMapping_TU102(pGpu, pKernelBus,
                                                     pRootMemDesc,
                                                     requestedDmaFlags,
                                                     NV_FALSE);
    }

    if (status == NV_OK)
    {
        pRootMemDesc->staticBar1MappingKind = requestedKind;
        pRootMemDesc->staticBar1DmaFlags = requestedDmaFlags;
        pRootMemDesc->staticBar1MappingRefCount++;
    }

    return status;
}

/*!
 * @brief Decrease the refcount on the staticBar1 mapped
 *        per memdesc or return error if it's unused
 *        or the mapping should freed from the dynamic region
 *        Automatically updates compression mapping.
 *
 * 
 * @param[in]   pGpu            GPU pointer
 * @param[in]   pKernelBus      Kernel bus pointer
 * @param[in]   pMemDesc        The memory to update
 * @param[in]   pMemArea        The Memory area the memdesc is mapped in according to kbus
 *                              It is used to determine whether the mapping is
 *                              in the dynamic BAR1
 *
 * return NV_OK on success
 * return NV_ERR_NOT_SUPPORTED if staticBar1 not enabled or the memdesc should
 *                             otherwise be freed from the dynamic region
 * return other on error
 */
NV_STATUS kbusDecreaseStaticBar1Refcount_TU102
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    MemoryArea *pMemArea
)
{
    NvU32 gfid;
    MEMORY_DESCRIPTOR *pRootMemDesc;

    NV_CHECK_OR_RETURN(LEVEL_SILENT, kbusIsStaticBar1Enabled(pGpu, pKernelBus),
                        NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK,
                        NV_ERR_INVALID_STATE);

    // TODO: investigate whether the tegra wbinvd flush is really necessary, seems only useful for SYSMEM_COH
    memdescFlushCpuCaches(pGpu, pMemDesc);

    pRootMemDesc = memdescGetRootMemDesc(pMemDesc, NULL);

    if (pMemArea != NULL)
    {
        // mappings in the dynamic range will be unmapped in normal unmap code
        MemoryRange staticRange = {
            .start = pKernelBus->bar1[gfid].staticBar1.startOffset,
            .size = pKernelBus->bar1[gfid].staticBar1.size
        };

        NV_CHECK_OR_RETURN(LEVEL_SILENT,
                           mrangeContains(staticRange, pMemArea->pRanges[0]),
                           NV_ERR_NOT_SUPPORTED);
    }
    else
    {
        //
        // pMemArea = NULL is passed in vidmemDestruct to entirely destroy the mapping
        // regardless of range. Mappings in the dynamic range don't get a static mapping,
        // so return early
        // copy constructor may create multiple references, so the refcount can be >= 1,
        // so the below decrement will naturally handle this.
        //
        if (pRootMemDesc->staticBar1MappingRefCount == 0)
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    NV_ASSERT_OR_RETURN(pRootMemDesc->staticBar1MappingRefCount != 0, NV_ERR_INVALID_STATE);

    if (--pRootMemDesc->staticBar1MappingRefCount != 0 ||
        pRootMemDesc->staticBar1MappingKind == pKernelBus->staticBar1DefaultKind)
    {
        return NV_OK;
    }

    //
    // deploying the static mapping just uses BUS_MAP_FB_FLAGS_MAP_UNICAST,
    // so nothing influences the dmaMapFlags (see kbusConvertBusMapFlagsToDmaFlags)
    //
    NV_ASSERT_OK_OR_RETURN(
        _kbusUpdateStaticBar1VAMapping_TU102(pGpu, pKernelBus, pRootMemDesc,
            BUS_MAP_FB_FLAGS_NONE, NV_TRUE));

    return NV_OK;
}

/*!
 * @brief To get FB aperture for the specified memory under the static mapping.
 *
 * @param[in]   pGpu            GPU pointer
 * @param[in]   pKernelBus      Kernel bus pointer
 * @param[in]   pMemDesc        The memory to update
 * @param[in]   mapRange        The range of the memory allocation to map
 * @param[out]  pMemArea        The Memory Area struct of theFb Aperture(BAR1)
 *                              of the mapped vidmem
 * @param[in]   gfid            The GFID
 * @param[in]   flags           Flags from kbusMapFbAperture_GM107 on whether to alloc
 *                              memory area
 *
 * return NV_ERR_NOT_SUPPORTED if static BAR1 is not enabled or this mapping should otherwise
 *                             be mapped in the dynamic region instead
 *        other error if fatal error has occurred and we should bail on the allocation
 *        NV_OK                if static BAR1 mapping is succesfully found and returned
 */
NV_STATUS
kbusGetStaticFbAperture_TU102
(
    OBJGPU     *pGpu,
    KernelBus  *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    MemoryRange mapRange,
    MemoryArea *pMemArea,
    NvU32       busMapFlags
)
{
    NV_STATUS   status;
    NvU64       pageSize;
    OBJVASPACE *pVAS;
    NvU32       gfid;
    NvU64       mapGranularity;
    NvU64       offset;
    NvU64       mapRangeEndPlus1;
    NvU64       numRanges = 0;
    NvU64       i;
    NvBool      bUnmanagedRange   = !!(busMapFlags & BUS_MAP_FB_FLAGS_UNMANAGED_MEM_AREA);
    NvBool      bDiscontigAllowed = !!(busMapFlags & BUS_MAP_FB_FLAGS_ALLOW_DISCONTIG);
    NvBool      bContigDesc;
    ADDRESS_TRANSLATION addressTranslation;
    MemoryRange testMapRange;
    NvBool      bInStaticRegion  = NV_FALSE;
    NvBool      bInDynamicRegion = NV_FALSE;

    NV_CHECK_OR_RETURN(LEVEL_SILENT, kbusIsStaticBar1Enabled(pGpu, pKernelBus),
                        NV_ERR_NOT_SUPPORTED);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM,
                        NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK,
                        NV_ERR_INVALID_STATE);

    NV_ASSERT_OR_RETURN(gfid == 0, NV_ERR_INVALID_STATE);

    NV_ASSERT_OR_RETURN(pMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);

    pVAS = pKernelBus->bar1[gfid].pVAS;
    addressTranslation = VAS_ADDRESS_TRANSLATION(pVAS);
    pageSize = memdescGetPageSize(pMemDesc, addressTranslation);
    bContigDesc = memdescGetContiguity(pMemDesc, addressTranslation);
    mapRangeEndPlus1 = mapRange.start + mapRange.size;
    // TODO: simplify for dynamic page granularity
    mapGranularity = bContigDesc ? mapRange.size : pageSize;

    NV_CHECK_OR_RETURN(LEVEL_INFO,
                       mapRangeEndPlus1 <= memdescGetSize(pMemDesc),
                       NV_ERR_OUT_OF_RANGE);

    //
    // check ranges before modifying any of the values since pMemArea->pRanges[i] is used by the dynamic code
    // if not used here and can't be written until it will be successfully allocated
    //
    for (offset = mapRange.start; offset < mapRangeEndPlus1; numRanges++)
    {
        testMapRange.start = memdescGetPhysAddr(pMemDesc, addressTranslation, offset);
        testMapRange.size  = bContigDesc ? mapGranularity : (NV_MIN(mapRangeEndPlus1, NV_ALIGN_UP(offset + 1, mapGranularity)) - offset);

        if (mrangeLimit(testMapRange) > pKernelBus->bar1[gfid].staticBar1.size)
        {
            bInDynamicRegion = NV_TRUE;
        }
        else
        {
            bInStaticRegion = NV_TRUE;
        }

        offset = bContigDesc ? (offset + mapGranularity) : NV_ALIGN_DOWN(offset + mapGranularity, mapGranularity);
    }

    NV_CHECK_OR_RETURN(LEVEL_INFO, (numRanges == 1) || (bDiscontigAllowed && !bUnmanagedRange),
                        NV_ERR_INVALID_ARGUMENT);

    if (bInDynamicRegion && bInStaticRegion)
    {
        NV_PRINTF(LEVEL_ERROR, "MemDesc spans both static and dynamic region,"
                               "which is unsupported.\n");
        NV_PRINTF(LEVEL_ERROR, "static Bar1 map [0, 0x%llx]\n",
                  pKernelBus->bar1[gfid].staticBar1.size);
        NV_PRINTF(LEVEL_ERROR, "Requested map range 0x%llx to 0x%llx, mapGranularity 0x%llx\n",
                  mapRange.start, mapRangeEndPlus1 - 1, mapGranularity);

        memdescPrintMemdesc(pMemDesc, NV_TRUE, MAKE_NV_PRINTF_STR("Dumping memdesc:"));

        return NV_ERR_INVALID_ARGUMENT;
    }

    if (bInDynamicRegion)
    {
        // goes in dynamic region only
        return NV_ERR_NOT_SUPPORTED;
    }

    status = kbusIncreaseStaticBar1Refcount_HAL(pGpu, pKernelBus, pMemDesc, busMapFlags);

    if (status == NV_ERR_IN_USE || status == NV_ERR_NOT_SUPPORTED)
    {
        //
        // This mapping is already in use or not supported,
        // and we need to make a dynamic mapping instead
        //
        return NV_ERR_NOT_SUPPORTED;
    }
    else if (status != NV_OK)
    {
        return status;
    }

    //
    // When the static bar1 is enabled, the Fb aperture offset is the
    // physical address.
    //
    if (!bUnmanagedRange)
    {
        pMemArea->pRanges = portMemAllocNonPaged(sizeof(MemoryRange) * numRanges);
        NV_CHECK_OR_RETURN(LEVEL_INFO, pMemArea->pRanges != NULL, NV_ERR_NO_MEMORY);
    }

    for (i = 0, offset = mapRange.start; offset < mapRangeEndPlus1; i++)
    {
        pMemArea->pRanges[i].start = memdescGetPhysAddr(pMemDesc, addressTranslation, offset) + pKernelBus->bar1[gfid].staticBar1.startOffset;
        pMemArea->pRanges[i].size  = bContigDesc ? mapGranularity : NV_MIN(mapRangeEndPlus1, NV_ALIGN_UP(offset + 1, mapGranularity)) - offset;

        offset = bContigDesc ? (offset + mapGranularity) : NV_ALIGN_DOWN(offset + mapGranularity, mapGranularity);
    }

    pMemArea->numRanges = numRanges;

    return NV_OK;
}
