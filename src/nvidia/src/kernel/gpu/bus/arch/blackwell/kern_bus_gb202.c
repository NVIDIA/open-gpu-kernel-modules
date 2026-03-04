/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/mem_mgr/mem_mgr.h"

#include "published/blackwell/gb202/dev_mmu.h"
#include "published/blackwell/gb202/dev_ram.h"
#include "published/blackwell/gb202/pri_nv_xal_ep.h"

#include "nvrm_registry.h"

/*!
 * @brief Tests BAR2 against BAR0.
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 * @param[in] pMemDescIn If memDescIn is NULL, a test mem desc is created and map/unmapped.
 *                       If memDescIn is not NULL and provided, this method assumes that it has
 *                       already been alloc'ed and mapping/unmapping is handled outside
 *                       this method.
 * @param[in] offset     offset of the test memory
 * @param[in] size       size of the test memory
 *
 * @returns NV_OK on success.
 */
NV_STATUS
kbusVerifyBar2_GB202
(
    OBJGPU             *pGpu,
    KernelBus          *pKernelBus,
    PMEMORY_DESCRIPTOR  pMemDescIn,
    NvU8               *pCpuPtrIn,
    NvU64               offset,
    NvU64               size
)
{
    MEMORY_DESCRIPTOR memDesc, *pMemDesc = NULL;
    NvU8             *pOffset          = NULL;
    NvU32             index            = 0;
    NvU64             bar0Window       = 0;
    NvU64             testMemoryOffset = 0;
    NvU32             testMemorySize   = 0;
    NV_STATUS         status           = NV_OK;
    NvU32             testData         = 0;
    NvU32             temp             = 0;
    NV_ADDRESS_SPACE  testAddrSpace    = ADDR_FBMEM;
    NvBool            bIsStandaloneTest;
    const NvU32       SAMPLEDATA       = 0xabcdabcd;
    const NvU32       FBSIZETESTED     = 0x10;
    NvU64             bar0TestAddr     = 0;
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NvU32             flagsClean       = 0;
    NvU64             bar2VirtualAddr  = 0;

    NV_ASSERT_OR_RETURN(pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING) == NV_FALSE, NV_ERR_INVALID_STATE);

    //
    // kbusVerifyBar2 will test BAR0 against sysmem on Tegra; otherwise skip
    // the test if inst_in_sys is used
    //
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM) &&
        !IsTEGRA(pGpu))
    {
        return NV_OK;
    }

    // In L2 Cache only mode or FB broken, don't verify Bar2
    if (gpuIsCacheOnlyModeEnabled(pGpu) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) ||
        kbusIsBar2TestSkipped(pKernelBus))
    {
        return NV_OK;
    }

    NV_PRINTF(LEVEL_INFO, "\n");

    flagsClean = NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_ALL |
                 NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_CLEAN;
    if (kmemsysIsL2CleanFbPull(pKernelMemorySystem))
    {
        flagsClean |= NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_WAIT_FB_PULL;
    }

    if (pMemDescIn && pCpuPtrIn)
    {
        if ((size + offset) > pMemDescIn->Size)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "input offset 0x%llx size 0x%llx exceeds surface size 0x%llx\n",
                      offset, size, pMemDescIn->Size);
            DBG_BREAKPOINT();
            return NV_ERR_INVALID_ARGUMENT;
        }
        bIsStandaloneTest = NV_FALSE;
        pOffset = pCpuPtrIn;
        pMemDesc = pMemDescIn;
    }
    else
    {
        offset = 0;
        size = FBSIZETESTED;
        // Allocate some memory to test virtual BAR2 with
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM))
        {
            memdescCreateExisting(&memDesc, pGpu, size, ADDR_SYSMEM, pGpu->instCacheOverride, MEMDESC_FLAGS_NONE);
        }
        else
        {
            memdescCreateExisting(&memDesc, pGpu, size, ADDR_FBMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE);
        }
        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_43,
                    (&memDesc));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not allocate vidmem to test bar2 with\n");
            DBG_BREAKPOINT();
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        bIsStandaloneTest = NV_TRUE;
        pOffset = kbusMapRmAperture_HAL(pGpu, &memDesc);
        if (pOffset == NULL)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto kbusVerifyBar2_failed;
        }
        pMemDesc = &memDesc;
    }
    testMemoryOffset = memdescGetPhysAddr(pMemDesc, AT_GPU, 0) + offset;
    testMemorySize   = NvU64_LO32(size);
    testAddrSpace    = kgmmuGetHwPteApertureFromMemdesc(GPU_GET_KERNEL_GMMU(pGpu), pMemDesc);

    if (testAddrSpace != NV_MMU_PTE_APERTURE_VIDEO_MEMORY)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Test is not supported. NV_XAL_EP_BAR0_WINDOW only supports vidmem\n");
        DBG_BREAKPOINT();
        status = NV_ERR_NOT_SUPPORTED;
        goto kbusVerifyBar2_failed;
    }

    // Log error before starting test, updated to success if test passed

    // ==========================================================
    // Does the BAR0 window work?

    NV_PRINTF((IS_EMULATION(pGpu)) ? LEVEL_ERROR : LEVEL_INFO,
        "Testing BAR0 window...\n");

    bar0Window = kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);
    bar0TestAddr = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);

    kbusWriteBAR0WindowBase_HAL(pGpu, pKernelBus, NvU64_LO32(bar0TestAddr >> 16));

    testData = GPU_REG_RD32(pGpu, DRF_BASE(NV_PRAMIN) + NvU64_LO32(bar0TestAddr & 0xffff));

    GPU_REG_WR32(pGpu, DRF_BASE(NV_PRAMIN) + NvU64_LO32(bar0TestAddr & 0xffff), SAMPLEDATA);

    if (GPU_REG_RD32(pGpu, DRF_BASE(NV_PRAMIN) + NvU64_LO32(bar0TestAddr & 0xffff)) != SAMPLEDATA)
    {
        //
        // Ideally, this should hit the L2 cache and even if memory is bad,
        // unless something in the path up to L2 is messed up, we should not
        // get here.
        //
        NV_PRINTF(LEVEL_ERROR,
            "Pre-L2 invalidate evict: Address 0x%llx programmed through the bar0 "
            "window with value 0x%x did not read back the last write.\n",
            bar0TestAddr, SAMPLEDATA);
        DBG_BREAKPOINT_REASON(NV_ERR_MEMORY_ERROR);
        status = NV_ERR_MEMORY_ERROR;
        goto kbusVerifyBar2_failed;
    }

    //
    // Evict L2 to ensure that the next read doesn't hit L2 and mistakenly
    // assume that the BAR0 window to vidmem works
    //
    status = kmemsysSendL2InvalidateEvict(pGpu, pKernelMemorySystem, flagsClean);
    if (NV_OK != status)
    {
        NV_PRINTF(LEVEL_ERROR, "L2 evict failed\n");
        goto kbusVerifyBar2_failed;
    }

    if (GPU_REG_RD32(pGpu, DRF_BASE(NV_PRAMIN) + NvU64_LO32(bar0TestAddr & 0xffff)) != SAMPLEDATA)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Post-L2 invalidate evict: Address 0x%llx programmed through the bar0 "
            "window with value 0x%x did not read back the last write\n",
            bar0TestAddr, SAMPLEDATA);
        if (IS_EMULATION(pGpu))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Setup a trigger on write<Bar0+0x1700, 0x40> with a 3 quarters post "
                      "trigger capture\n");
            NV_PRINTF(LEVEL_ERROR,
                      "and search for the last bar0 window write not returning the same value"
                      " in a subsequent read\n");
        }
        DBG_BREAKPOINT_REASON(NV_ERR_MEMORY_ERROR);
        status = NV_ERR_MEMORY_ERROR;
        goto kbusVerifyBar2_failed;
    }

    NV_PRINTF((IS_EMULATION(pGpu)) ? LEVEL_ERROR : LEVEL_INFO, "Bar0 window tests successfully\n");

    GPU_REG_WR32(pGpu, DRF_BASE(NV_PRAMIN) + NvU64_LO32(bar0TestAddr & 0xffff), testData);

    kbusWriteBAR0WindowBase_HAL(pGpu, pKernelBus, NvU64_LO32(bar0Window >> 16));

    // ==========================================================
    // Does MMU's translation logic work?

    bar2VirtualAddr = (NvU64)(pOffset - pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping);
    NV_PRINTF(LEVEL_INFO,
              "MMUTest Writing test data through virtual BAR2 starting at bar2 offset"
              " (%p - %p) = %p and of size 0x%x\n", (NvU8 *)pOffset,
              (NvU8 *)pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping,
              (NvU8 *)bar2VirtualAddr, testMemorySize);

    NV_PRINTF_COND(IS_EMULATION(pGpu), LEVEL_ERROR, LEVEL_INFO,
        "MMUTest The physical address being targetted is 0x%llx\n", testMemoryOffset);

    for(index = 0; index < testMemorySize; index += 4)
    {
        MEM_WR32( pOffset + index, SAMPLEDATA );
    }

    // Flush the bar2 writes
    // A uflush should not be required since a bar0 window read follows after this
    osFlushCpuWriteCombineBuffer();

    status = kmemsysSendL2InvalidateEvict(pGpu, pKernelMemorySystem, flagsClean);
    if (NV_OK != status)
    {
        NV_PRINTF(LEVEL_ERROR, "L2 evict failed\n");
        goto kbusVerifyBar2_failed;
    }

    //
    // Read back using the same BAR2 VA. This can make sure the writes have
    // gotten to memory after MMU translation.
    //
    // What it will catch:
    // - FB dead (also caught by BAR0 test above)
    // - MMU translation fails on BAR2
    // - MMU translation works but other parts of memsys having issues
    //
    // It will not verify whether the mapping points to the right physical
    // memory. The BAR0 readback test will do that next.
    //
    for(index = 0; index < testMemorySize; index += 4)
    {
        NvU32 bar2ReadbackData = 0;
        bar2ReadbackData = MEM_RD32(pOffset + index);

        if (bar2ReadbackData != SAMPLEDATA)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "MMUTest BAR2 readback VA = 0x%llx returned garbage 0x%x\n",
                      (bar2VirtualAddr + index), bar2ReadbackData);

            DBG_BREAKPOINT_REASON(NV_ERR_MEMORY_ERROR);
            status = NV_ERR_MEMORY_ERROR;
            goto kbusVerifyBar2_failed;
        }
    }

    // Readback through the bar0 window
    bar0Window = kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);

    kbusWriteBAR0WindowBase_HAL(pGpu, pKernelBus, NvU64_LO32(testMemoryOffset >> 16));

    NV_PRINTF(LEVEL_INFO,
              "bar0Window = 0x%llx, testMemoryOffset = 0x%llx, testAddrSpace = %d, "
              "_XAL_EP_BAR0_WINDOW = 0x%08x\n", bar0Window, testMemoryOffset,
              testAddrSpace, GPU_REG_RD32(pGpu, NV_XAL_EP_BAR0_WINDOW));

    temp = (DRF_BASE(NV_PRAMIN) + (NvU32)(testMemoryOffset & 0xffff));
    for(index = 0; index < testMemorySize; index += 4)
    {
        NvU32 bar0WindowData = GPU_REG_RD32(pGpu, temp + index);
        if (bar0WindowData != SAMPLEDATA)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "MMUTest BAR0 window offset 0x%x returned garbage 0x%x\n",
                      temp + index, bar0WindowData);
            if (IS_EMULATION(pGpu) == NV_TRUE)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Setup a trigger for write<bar0 + 0x1700, 0x40> and in the waves search"
                          " the last few bar2 virtual writes mixed with bar0 window reads\n");
            }
            DBG_BREAKPOINT_REASON(NV_ERR_MEMORY_ERROR);
            status = NV_ERR_MEMORY_ERROR;
            goto kbusVerifyBar2_failed;
        }
        // Write through the BAR0 window to be readback through BAR2 later
        GPU_REG_WR32(pGpu, temp + index, SAMPLEDATA + 0x10);
    }

    kbusWriteBAR0WindowBase_HAL(pGpu, pKernelBus, NvU64_LO32(bar0Window >> 16));

    status = kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_VIDEO_MEMORY);

    // Bail now if we have encountered any error
    if (status != NV_OK)
    {
        goto kbusVerifyBar2_failed;
    }

    status = kmemsysSendL2InvalidateEvict(pGpu, pKernelMemorySystem, flagsClean);
    if (NV_OK != status)
    {
        goto kbusVerifyBar2_failed;
    }

    // Verify BAR2 virtual reads
    for(index = 0; index < testMemorySize; index +=4)
    {
        temp = MEM_RD32(pOffset + index);
        if (temp != (SAMPLEDATA + 0x10))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "MMUTest BAR2 Read of virtual addr 0x%x returned garbage 0x%x\n",
                      (NvU32)(pOffset - pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping) + index,
                      temp);
            DBG_BREAKPOINT_REASON(NV_ERR_MEMORY_ERROR);
            status = NV_ERR_MEMORY_ERROR;
            goto kbusVerifyBar2_failed;
        }
    }

kbusVerifyBar2_failed:

    if (bIsStandaloneTest)
    {
        if (pOffset != NULL)
        {
            kbusUnmapRmAperture_HAL(pGpu, pMemDesc, &pOffset, NV_TRUE);
        }
        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);
    }

    if (status == NV_OK)
    {
        NV_PRINTF(IS_EMULATION(pGpu) ? LEVEL_ERROR : LEVEL_INFO,
                  "BAR2 virtual test passes\n");

    }

    return status;
}
