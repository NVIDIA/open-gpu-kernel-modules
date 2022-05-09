/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "os/os.h"

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

    // Nothing to be done in paravirtualized guest or if we don't want to do CPU reads for flushing.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        !kbusIsReadCpuPointerToFlushEnabled(pKernelBus))
    {
        return NV_OK;
    }

    status = memdescCreate(&pKernelBus->pFlushMemDesc, pGpu,
                           RM_PAGE_SIZE,
                           RM_PAGE_SIZE,
                           NV_TRUE,
                           ADDR_FBMEM,
                           NV_MEMORY_UNCACHED,
                           MEMDESC_FLAGS_LOST_ON_SUSPEND);
    NV_ASSERT_OR_GOTO(status == NV_OK, cleanup);

    // Allocate memory from reserved heap for flush
    status = memdescAlloc(pKernelBus->pFlushMemDesc);
    NV_ASSERT_OR_GOTO(status == NV_OK, cleanup);

    //
    // Please note this is a long-lived BAR2 mapping by design.
    // The mapping is used for flushing all future vidmem writes on BAR2.
    //
    pKernelBus->pReadToFlush = memmgrMemDescBeginTransfer(GPU_GET_MEMORY_MANAGER(pGpu),
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
        memmgrMemDescEndTransfer(GPU_GET_MEMORY_MANAGER(pGpu),
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
 * @param[in] pGpu       Pointer to GPU
 * @param[in] pKernelBus Kernel bus pointer
 * @param[in] pMemDesc   Pointer to memdesc that is to be mapped.
 *
 * @return cpu pointer if success
 *         NULL on other errors
 */
NvU8*
kbusMapCoherentCpuMapping_GV100
(
    OBJGPU                *pGpu,
    KernelBus             *pKernelBus,
    PMEMORY_DESCRIPTOR     pMemDesc
)
{
    RmPhysAddr startAddr = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
    NvU64      size = memdescGetSize(pMemDesc);
    RmPhysAddr endAddr = startAddr + size - 1;
    RmPhysAddr rangeStart = 0;
    RmPhysAddr rangeEnd = 0;
    RmPhysAddr offset = 0;
    NvU32 i = 0;

    for (i = COHERENT_CPU_MAPPING_WPR; i < pKernelBus->coherentCpuMapping.nrMapping; ++i)
    {
        // Check if requested mem in the mappings.
        rangeStart = pKernelBus->coherentCpuMapping.physAddr[i];
        rangeEnd = pKernelBus->coherentCpuMapping.physAddr[i] + pKernelBus->coherentCpuMapping.size[i] - 1;
        offset = 0;

        if (rangeStart <= startAddr && endAddr <= rangeEnd)
        {
            NV_ASSERT_OR_RETURN(
                pKernelBus->coherentCpuMapping.pCpuMapping[i] != NvP64_NULL, NvP64_NULL);

            // Get the offset of the region
            offset = startAddr - pKernelBus->coherentCpuMapping.physAddr[i];
            pKernelBus->coherentCpuMapping.refcnt[i]++;
            return (NvU8 *)NvP64_VALUE(
                ((NvUPtr)pKernelBus->coherentCpuMapping.pCpuMapping[i] +
                 (NvUPtr)offset));
        }
    }

    NV_ASSERT_FAILED("No mappings found");
    return NvP64_NULL;
}

/**
 * Helper function to unmap coherent cpu mapping
 *
 * @param[in] pGpu       Pointer to GPU
 * @param[in] pKernelBus Kernel bus pointer
 * @param[in] pMemDesc   Pointer to memdesc
 *
 * @return void
 */
void
kbusUnmapCoherentCpuMapping_GV100
(
    OBJGPU              *pGpu,
    KernelBus           *pKernelBus,
    PMEMORY_DESCRIPTOR   pMemDesc
)
{
    RmPhysAddr startAddr = pMemDesc->_pteArray[0] + pMemDesc->PteAdjust;
    NvU64      size = memdescGetSize(pMemDesc);
    RmPhysAddr endAddr = startAddr + size - 1;
    NvU32 i = 0;

    NV_ASSERT(pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS);

    for (i = COHERENT_CPU_MAPPING_WPR; i < pKernelBus->coherentCpuMapping.nrMapping; ++i)
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

    for (i = COHERENT_CPU_MAPPING_WPR; i < pKernelBus->coherentCpuMapping.nrMapping; ++i)
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
