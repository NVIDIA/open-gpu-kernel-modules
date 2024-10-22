/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/fifo/kernel_fifo.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/uvm/uvm.h"
#include "os/os.h"

#include "published/volta/gv100/dev_ram.h"

// @ref busMigrateBarMapping_GV100 to see how FB region is organized
#define COHERENT_CPU_MAPPING_WPR            COHERENT_CPU_MAPPING_REGION_0

/*!
 * @brief Sets up a memdesc and a CPU pointer to the bottom
 *        of FB that will be used for issuing reads in order
 *        to flush pending writes to FB.
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns NV_OK on success
 */
NV_STATUS
kbusSetupCpuPointerForBusFlush_GV100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NV_STATUS status = NV_OK;

    // Nothing to be done in paravirtualized guest (or CC) or if we don't want to do CPU reads for flushing.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        !kbusIsReadCpuPointerToFlushEnabled(pKernelBus))
    {
        return NV_OK;
    }

    NV_ASSERT_OR_RETURN(!kbusIsBarAccessBlocked(pKernelBus), NV_ERR_INVALID_STATE);

    status = memdescCreate(&pKernelBus->pFlushMemDesc, pGpu,
                           RM_PAGE_SIZE,
                           RM_PAGE_SIZE,
                           NV_TRUE,
                           ADDR_FBMEM,
                           NV_MEMORY_UNCACHED,
                           MEMDESC_FLAGS_LOST_ON_SUSPEND);
    NV_ASSERT_OR_GOTO(status == NV_OK, cleanup);

    // Allocate memory from reserved heap for flush
    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_54,
                    pKernelBus->pFlushMemDesc);
    NV_ASSERT_OR_GOTO(status == NV_OK, cleanup);

    //
    // Please note this is a long-lived BAR2 mapping by design.
    // The mapping is used for flushing all future vidmem writes on BAR2.
    //
    pKernelBus->pReadToFlush = memdescMapInternal(pGpu,
                                                  pKernelBus->pFlushMemDesc,
                                                  TRANSFER_FLAGS_PERSISTENT_CPU_MAPPING);
    if (pKernelBus->pReadToFlush == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        NV_ASSERT_OR_GOTO(pKernelBus->pReadToFlush != NULL, cleanup);
    }

    return NV_OK;
cleanup:
    kbusDestroyCpuPointerForBusFlush_HAL(pGpu, pKernelBus);
    return status;
}

/*!
 * @brief Destroys the memdesc and frees the CPU pointer to the bottom of
 *        FB that was used for issuing reads in order to trigger bus flushes.
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns
 */
void
kbusDestroyCpuPointerForBusFlush_GV100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    if (pKernelBus->pReadToFlush != NULL)
    {
        memdescUnmapInternal(pGpu,
                             pKernelBus->pFlushMemDesc,
                             TRANSFER_FLAGS_DEFER_FLUSH);
        pKernelBus->pReadToFlush = NULL;
    }

    memdescFree(pKernelBus->pFlushMemDesc);
    memdescDestroy(pKernelBus->pFlushMemDesc);
    pKernelBus->pFlushMemDesc = NULL;
}

/**
 * Helper function to map coherent cpu mapping.
 *
 * @param[in]  pGpu       Pointer to GPU
 * @param[in]  pKernelBus Kernel bus pointer
 * @param[in]  pMemDesc   Pointer to memdesc that is to be mapped.
 * @param[in]  offset     Offset from base address given in memdesc
 * @param[in]  length     Length of memory to map
 * @param[in]  protect    Protection flags
 * @param[out] ppAddress  Virtual address of mapping
 * @param[out] ppPriv     Private data to be retained for unmapping
 *
 * @return NV_OK or errors if failed to map
 */
NV_STATUS
kbusMapCoherentCpuMapping_GV100
(
    OBJGPU                *pGpu,
    KernelBus             *pKernelBus,
    MEMORY_DESCRIPTOR     *pMemDesc,
    NvU64                  offset,
    NvU64                  length,
    NvU32                  protect,
    NvP64                 *ppAddress,
    NvP64                 *ppPriv
)
{
    RmPhysAddr startAddr = memdescGetPhysAddr(pMemDesc, FORCE_VMMU_TRANSLATION(pMemDesc, AT_GPU), offset);
    NvU64      size = memdescGetSize(pMemDesc);
    RmPhysAddr endAddr = startAddr + size - 1;
    RmPhysAddr rangeStart = 0;
    RmPhysAddr rangeEnd = 0;
    RmPhysAddr regionOffset = 0;
    NvU32 i = 0;

    NV_ASSERT_OR_RETURN(memdescGetContiguity(pMemDesc, AT_GPU), NV_ERR_NOT_SUPPORTED);

    for (i = COHERENT_CPU_MAPPING_REGION_0; i < pKernelBus->coherentCpuMapping.nrMapping; ++i)
    {
        // Check if requested mem in the mappings.
        rangeStart = pKernelBus->coherentCpuMapping.physAddr[i];
        rangeEnd = pKernelBus->coherentCpuMapping.physAddr[i] + pKernelBus->coherentCpuMapping.size[i] - 1 ;
        regionOffset = 0;

        if (rangeStart <= startAddr && endAddr <= rangeEnd)
        {
            NV_ASSERT_OR_RETURN(
                pKernelBus->coherentCpuMapping.pCpuMapping[i] != NvP64_NULL, NV_ERR_INVALID_STATE);

            // Get the offset of the region
            regionOffset = startAddr - pKernelBus->coherentCpuMapping.physAddr[i];
            pKernelBus->coherentCpuMapping.refcnt[i]++;
            *ppAddress = (NvU8 *)NvP64_VALUE(
                ((NvUPtr)pKernelBus->coherentCpuMapping.pCpuMapping[i] +
                 (NvUPtr)regionOffset));
            return NV_OK;
        }
    }

    NV_ASSERT_FAILED("No mappings found");
    return NV_ERR_INVALID_ARGUMENT;
}

/**
 * Helper function to unmap coherent cpu mapping
 *
 * @param[in] pGpu       Pointer to GPU
 * @param[in] pKernelBus Kernel bus pointer
 * @param[in] pMemDesc   Pointer to memdesc
 * @param[in] pAddress   Virtual address to unmap
 * @param[in] pPriv      Private data to be passed for unmapping
 *
 * @return void
 */
void
kbusUnmapCoherentCpuMapping_GV100
(
    OBJGPU              *pGpu,
    KernelBus           *pKernelBus,
    MEMORY_DESCRIPTOR   *pMemDesc,
    NvP64                pAddress,
    NvP64                pPriv
)
{
    RmPhysAddr startAddr = memdescGetPhysAddr(pMemDesc, FORCE_VMMU_TRANSLATION(pMemDesc, AT_GPU), 0);
    NvU64      size = memdescGetSize(pMemDesc);
    RmPhysAddr endAddr = startAddr + size - 1;
    NvU32 i = 0;

    NV_ASSERT(pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS);

    for (i = COHERENT_CPU_MAPPING_REGION_0; i < pKernelBus->coherentCpuMapping.nrMapping; ++i)
    {
        RmPhysAddr rangeStart = pKernelBus->coherentCpuMapping.physAddr[i];
        RmPhysAddr rangeEnd = pKernelBus->coherentCpuMapping.physAddr[i] +
            pKernelBus->coherentCpuMapping.size[i] - 1;
        if (rangeStart <= startAddr && endAddr <= rangeEnd)
        {
            NV_ASSERT_OR_RETURN_VOID(pKernelBus->coherentCpuMapping.refcnt[i] != 0);
            pKernelBus->coherentCpuMapping.refcnt[i]--;
            break;
        }
    }

    if (i == pKernelBus->coherentCpuMapping.nrMapping)
    {
        NV_ASSERT_FAILED("No mappings found");
    }

    // Flush the memory since caller writes to the FB
    kbusFlush_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), BUS_FLUSH_VIDEO_MEMORY);

    return;
}
/**
 * Destroy coherent cpu mapping to ACR region.
 *
 * This needs to be done only for P9 and not SHH. In SHH, CPU prefetches
 * to WPR region because of the CPU mapping doesn't result in SW visible error
 * unlike P9.
 *
 * @param[in] pGpu       Pointer to Gpu
 * @param[in] pKernelBus Kernel bus pointer
 *
 * @return void
 */
void kbusTeardownCoherentCpuMappingAcr_GV100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    if (pKernelBus->coherentCpuMapping.bCoherentCpuMapping)
    {
        NV_ASSERT_OR_RETURN_VOID(pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING));
        NV_ASSERT_OR_RETURN_VOID( pKernelBus->coherentCpuMapping.refcnt[COHERENT_CPU_MAPPING_WPR] == 0);

        osFlushGpuCoherentCpuCacheRange(pGpu->pOsGpuInfo,
             (NvUPtr)pKernelBus->coherentCpuMapping.pCpuMapping[COHERENT_CPU_MAPPING_WPR],
             pKernelBus->coherentCpuMapping.size[COHERENT_CPU_MAPPING_WPR]);

        osUnmapPciMemoryKernel64(pGpu,
            pKernelBus->coherentCpuMapping.pCpuMapping[COHERENT_CPU_MAPPING_WPR]);
        pKernelBus->coherentCpuMapping.pCpuMapping[COHERENT_CPU_MAPPING_WPR] = NvP64_NULL;
    }
}

/*!
 * @brief Destroy coherent cpu mapping.
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelBus     Kernel bus pointer
 * @param[in] bFlush         Flush CPU cache or not
 *
 * @return 'NV_OK' if successful, an RM error code otherwise.
 */
void
kbusTeardownCoherentCpuMapping_GV100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvBool    bFlush
)
{
    NvU32 i = 0;

    if (!pKernelBus->coherentCpuMapping.bCoherentCpuMapping)
        return;

    for (i = COHERENT_CPU_MAPPING_REGION_0; i < pKernelBus->coherentCpuMapping.nrMapping; ++i)
    {
        NV_ASSERT_OR_RETURN_VOID(pKernelBus->coherentCpuMapping.refcnt[i] == 0);

        if (pKernelBus->coherentCpuMapping.pCpuMapping[i] != NvP64_NULL)
        {
            if (bFlush)
            {
                osFlushGpuCoherentCpuCacheRange(pGpu->pOsGpuInfo,
                                                (NvUPtr)pKernelBus->coherentCpuMapping.pCpuMapping[i],
                                                pKernelBus->coherentCpuMapping.size[i]);
            }

            osUnmapPciMemoryKernel64(pGpu, pKernelBus->coherentCpuMapping.pCpuMapping[i]);
            pKernelBus->coherentCpuMapping.pCpuMapping[i] = NvP64_NULL;
        }
    }

    pKernelBus->coherentCpuMapping.bCoherentCpuMapping = NV_FALSE;
}

/*!
 * @brief Lower level FB flush to push pending writes to FB/sysmem
 *
 * NOTE: Must be called inside a SLI loop
 *
 * @param[in]   pGpu
 * @param[in]   KernelBus
 * @param[in]   flags   Flags to indicate aperture and other behaviors
 * @return      NV_OK on success
 *
 */
NV_STATUS
kbusFlushSingle_GV100
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus,
    NvU32        flags
)
{
    NvBool  bCoherentCpuMapping = pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING);

    //
    // Nothing to be done in the guest in the paravirtualization case or
    // if guest is running in SRIOV heavy mode.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_OK;
    }

    if (bCoherentCpuMapping)
    {
        //
        // This function issues an HWSYNC. This is needed for synchronizing read/writes
        // with NVLINK mappings.
        //
        portAtomicMemoryFenceFull();
        return NV_OK;
    }

    if (flags & BUS_FLUSH_SYSTEM_MEMORY)
    {
        portAtomicMemoryFenceFull();
    }

    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu) || API_GPU_IN_RECOVERY_SANITY_CHECK(pGpu) ||
        !API_GPU_ATTACHED_SANITY_CHECK(pGpu))
    {
        //
        // When the GPU is in full chip reset or lost
        // We cannot expect to flush successfully so early return here
        //
        return NV_OK;
    }

    if (kbusIsBarAccessBlocked(pKernelBus))
    {
        // If BAR1/2 has been blocked, there's nothing to flush for vidmem
        return NV_OK;
    }

    if ((flags & BUS_FLUSH_VIDEO_MEMORY) && kbusIsReadCpuPointerToFlushEnabled(pKernelBus))
    {
        //
        // Read the FB address 0 in order to trigger a flush.
        // This will not work with reflected mappings so only enable on VOLTA+
        // Note SRIOV guest does not have access to uflush register.
        //
        NV_ASSERT_OR_RETURN(
            (pKernelBus->pReadToFlush != NULL || pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping != NULL),
            NV_ERR_INVALID_STATE);

        if (pKernelBus->pReadToFlush != NULL)
        {
            (void)MEM_RD32(pKernelBus->pReadToFlush);
        }
        else if (pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping != NULL)
        {
            //
            // pReadToFlush is still not ready for use. So, use pCpuMapping
            // instead which should already be mapped to FB addr 0 as
            // BAR2 is in physical mode right now.
            //
            (void)MEM_RD32(pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping);
        }
    }

    if (flags & BUS_FLUSH_VIDEO_MEMORY_VIA_PRAMIN_WINDOW)
    {
        //
        // Read the PRAMIN pseudo-bar0-register window to flush writes in it to FB.
        // Note that despite being "register" writes, these transactions are snooped
        // at GPU before hitting BAR0 master, and get redirected to BAR1.
        //
        // This flush option REQUIRES a valid window to be mapped for PRAMIN prior
        // to the call! Note that it doesn't have to be the same window that contains
        // prior writes you might want to flush.
        //
        (void)GPU_REG_RD32(pGpu, DRF_BASE(NV_PRAMIN));
    }

    return NV_OK;
}

NvU32
kbusCalcCpuInvisibleBar2ApertureSize_GV100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus)
{
    NvU32       i;
    NvU32       apertureSize = 0;
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    // Return 0 from the guest in the paravirtualization case.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)) ||
        pKernelGmmu == NULL)
    {
        return 0;
    }

    // Add size required for replayable and non-replayable MMU fault buffers.
    for (i = 0; i < NUM_FAULT_BUFFERS; i++)
    {
        NvU32 faultBufferSize;

        faultBufferSize = kgmmuSetAndGetDefaultFaultBufferSize_HAL(pGpu, pKernelGmmu, i, GPU_GFID_PF);
        faultBufferSize = RM_ALIGN_UP(faultBufferSize, RM_PAGE_SIZE);
        apertureSize += faultBufferSize;
    }

    // Add size required for fault method buffers. Each run queue has its own method buffer.
    apertureSize += (kfifoCalcTotalSizeOfFaultMethodBuffers_HAL(pGpu, pKernelFifo, NV_FALSE));
    apertureSize = RM_ALIGN_UP(apertureSize, RM_PAGE_SIZE);

    return apertureSize;
}
