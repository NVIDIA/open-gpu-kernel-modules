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

#include "core/core.h"
#include "gpu/gpu.h"

#include <class/cl00fc.h>      // FABRIC_VASPACE_A
#include "gpu/bus/kern_bus.h"
#include "gpu/bus/p2p_api.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/mem_mgr/virt_mem_allocator_common.h"
#include "mem_mgr/fabric_vaspace.h"
#include "mem_mgr/virt_mem_mgr.h"
#include "vgpu/rpc.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "os/os.h"
#include "containers/eheap_old.h"

#include "mem_mgr/mem_multicast_fabric.h"

#include "gpu/gpu_fabric_probe.h"
#include "published/hopper/gh100/dev_ram.h"
#include "published/hopper/gh100/pri_nv_xal_ep.h"
#include "published/hopper/gh100/pri_nv_xal_ep_p2p.h"
#include "published/hopper/gh100/dev_xtl_ep_pcfg_gpu.h"
#include "published/hopper/gh100/dev_vm.h"
#include "published/hopper/gh100/dev_mmu.h"
#include "ctrl/ctrl2080/ctrl2080fla.h" // NV2080_CTRL_CMD_FLA_SETUP_INSTANCE_MEM_BLOCK

#include "nvrm_registry.h"

 // Defines for P2P
#define HOPPER_MAX_WRITE_MAILBOX_ADDR(pGpu)                                         \
    ((PCIE_P2P_WRITE_MAILBOX_SIZE << kbusGetP2PWriteMailboxAddressSize_HAL(pGpu)) - \
     PCIE_P2P_WRITE_MAILBOX_SIZE)

// RM reserved memory region is mapped separately as it is not added to the kernel
#define COHERENT_CPU_MAPPING_RM_RESV_REGION             COHERENT_CPU_MAPPING_REGION_1
#define COHERENT_CPU_MAPPING_RM_KERNEL_ONLINED_REGION   COHERENT_CPU_MAPPING_REGION_0

/*!
 * @brief Tear Down BAR1 Mailbox
 *
 * Destroys Bar1 VA Space.
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelBus KernelBus pointer
 *
 * @returns void
 */
void
kbusTeardownMailbox_GH100
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus
)
{
    NvU32 bar1Block = 0;

    // set bar1 mode to physical and vidmem so we don't accidentally corrupt sysmem
    bar1Block = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR);
    bar1Block = FLD_SET_DRF(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR1_BLOCK_LOW_ADDR, _MODE, _PHYSICAL, bar1Block);
    bar1Block = FLD_SET_DRF(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR1_BLOCK_LOW_ADDR, _TARGET, _VID_MEM, bar1Block);

    //
    // override the aperture to sysmem if FB is not present.
    //
    if ((pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB)) && RMCFG_FEATURE_MODS_FEATURES)
    {
        bar1Block = FLD_SET_DRF(
                        _VIRTUAL_FUNCTION_PRIV_FUNC, _BAR1_BLOCK_LOW_ADDR, _TARGET, _SYS_MEM_NONCOHERENT, bar1Block);
    }

    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR, bar1Block);
}

/*!
 * @brief Gets the P2P write mailbox address size (NV_XAL_EP_P2P_WMBOX_ADDR_ADDR)
 *
 * @returns P2P write mailbox address size (NV_XAL_EP_P2P_WMBOX_ADDR_ADDR)
 */
NvU32
kbusGetP2PWriteMailboxAddressSize_GH100(OBJGPU *pGpu)
{
    return DRF_SIZE(NV_XAL_EP_P2P_WMBOX_ADDR_ADDR);
}

/*!
 * @brief Writes NV_XAL_EP_BAR0_WINDOW_BASE
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 * @param[in] base       base address to write
 *
 * @returns NV_OK
 */
NV_STATUS
kbusWriteBAR0WindowBase_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      base
)
{
    GPU_FLD_WR_DRF_NUM(pGpu, _XAL_EP, _BAR0_WINDOW, _BASE, base);
    return NV_OK;
}

/*!
 * @brief Reads NV_XAL_EP_BAR0_WINDOW_BASE
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns Contents of NV_XAL_EP_BAR0_WINDOW_BASE
 */
NvU32
kbusReadBAR0WindowBase_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    return GPU_REG_RD_DRF(pGpu, _XAL_EP, _BAR0_WINDOW, _BASE);
}

/*!
 * @brief Validates that the given base fits within the width of the window base
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 * @param[in] base       base offset to validate
 *
 * @returns Whether given base fits within the width of the window base.
 */
NvBool
kbusValidateBAR0WindowBase_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      base
)
{
    return base <= DRF_MASK(NV_XAL_EP_BAR0_WINDOW_BASE);
}

NV_STATUS
kbusSetBAR0WindowVidOffset_GH100
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus,
    NvU64        vidOffset
)
{
    if (KBUS_BAR0_PRAMIN_DISABLED(pGpu))
    {
        NV_ASSERT_FAILED("kbusSetBAR0WindowVidOffset_HAL call in coherent path\n");
        return NV_ERR_INVALID_STATE;
    }

    NV_ASSERT((vidOffset & 0xffff)==0);
    NV_ASSERT(kbusValidateBAR0WindowBase_HAL(pGpu, pKernelBus, vidOffset >> NV_XAL_EP_BAR0_WINDOW_BASE_SHIFT));

    //
    // RM initialises cachedBar0WindowVidOffset with 0. Refresh its value with
    // current NV_XAL_EP_BAR0_WINDOW_BASE.
    //
    if (pKernelBus->cachedBar0WindowVidOffset == 0)
    {
        pKernelBus->cachedBar0WindowVidOffset = ((NvU64) kbusReadBAR0WindowBase_HAL(pGpu, pKernelBus))
            << NV_XAL_EP_BAR0_WINDOW_BASE_SHIFT;
    }

    // Update only if the new offset is different from the cached value
    if (pKernelBus->cachedBar0WindowVidOffset != vidOffset)
    {
        NV_PRINTF(LEVEL_INFO,
                  "mapping BAR0_WINDOW to VID:%x'%08x\n",
                  NvU64_HI32(vidOffset), NvU64_LO32(vidOffset));

        // _BAR0_WINDOW_TARGET field is removed. It's always VIDMEM
        kbusWriteBAR0WindowBase_HAL(pGpu, pKernelBus, NvU64_LO32(vidOffset >> 16));

        pKernelBus->cachedBar0WindowVidOffset = vidOffset;
    }

    return (NV_OK);
}

NvU64
kbusGetBAR0WindowVidOffset_GH100
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus
)
{
    NvU64 vidOffset;

    //
    // RM initialises cachedBar0WindowVidOffset with 0. Refresh its value with
    // current NV_XAL_EP_BAR0_WINDOW_BASE.
    //
    if (pKernelBus->cachedBar0WindowVidOffset == 0)
    {
        pKernelBus->cachedBar0WindowVidOffset = ((NvU64) kbusReadBAR0WindowBase_HAL(pGpu, pKernelBus))
            << NV_XAL_EP_BAR0_WINDOW_BASE_SHIFT;
    }

    vidOffset = pKernelBus->cachedBar0WindowVidOffset;

    return (vidOffset);
}

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
kbusVerifyBar2_GH100
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus,
    PMEMORY_DESCRIPTOR pMemDescIn,
    NvU8        *pCpuPtrIn,
    NvU64        offset,
    NvU64        size
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
        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_95, 
                        (&memDesc));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not allocate vidmem to test bar2 with\n");
            DBG_BREAKPOINT();
            return NV_ERR_GENERIC;
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

    // ==========================================================
    // Does the BAR0 window work?

    NV_PRINTF_COND(IS_EMULATION(pGpu), LEVEL_ERROR, LEVEL_INFO, "Testing BAR0 window...\n");

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

    NV_PRINTF_COND(IS_EMULATION(pGpu), LEVEL_ERROR, LEVEL_INFO, "Bar0 window tests successfully\n");

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
        MEM_WR32(pOffset + index, SAMPLEDATA);
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
        NV_PRINTF_COND(IS_EMULATION(pGpu), LEVEL_ERROR, LEVEL_INFO, "BAR2 virtual test passes\n");
    }

    return status;
}

/*!
 * @brief Tear down BAR2 CPU aperture
 *
 * 1. Release BAR2 GPU vaspace mappings.
 * 2. Release BAR2 CPU mapping.
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 * @param[in] gfid
 *
 * @returns NV_OK on success.
 */
NV_STATUS
kbusTeardownBar2CpuAperture_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid
)
{
    // Nothing to be done in guest in the paravirtualization case.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) || IS_GFID_VF(gfid))
    {
        return NV_OK;
    }

    if (pKernelBus->virtualBar2[gfid].pPageLevels != NULL &&
        pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc != NULL)
    {
        memmgrMemDescEndTransfer(GPU_GET_MEMORY_MANAGER(pGpu),
                        pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc,
                        TRANSFER_FLAGS_NONE);
        pKernelBus->virtualBar2[gfid].pPageLevels = NULL;
    }

    kbusDestroyCpuPointerForBusFlush_HAL(pGpu, pKernelBus);

    kbusFlushVirtualBar2_HAL(pGpu, pKernelBus, NV_FALSE, gfid);

    if (pKernelBus->virtualBar2[gfid].pCpuMapping != NULL)
    {
        osUnmapPciMemoryKernelOld(pGpu, (void*)pKernelBus->virtualBar2[gfid].pCpuMapping);
        // Mark the BAR as un-initialized so that a later call
        // to initbar2 can succeed.
        pKernelBus->virtualBar2[gfid].pCpuMapping = NULL;
    }

    //
    // make sure that the bar2 mode is physical so that the vesa extended
    // linear framebuffer works after driver unload.  Clear other bits to force
    // vid.
    //
    // if BROKEN_FB, merely rewriting this to 0 (as it already was) causes
    // FBACKTIMEOUT -- don't do it (Bug 594539)
    //
    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB))
    {
        GPU_VREG_FLD_WR_DRF_DEF(pGpu, _VIRTUAL_FUNCTION_PRIV_FUNC, _BAR2_BLOCK_LOW_ADDR, _MODE, _PHYSICAL);
        // bug 1738008: temporary fix to unblock -inst_in_sys argument
        // we tried to correct bar2 unbind sequence but didn't fix the real issue
        // will fix this soon 4/8/16
        GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR);
    }

    return NV_OK;
}

NV_STATUS
kbusAllocP2PMailboxBar1_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid,
    NvU64      vaRangeMax
)
{
    OBJGPU           *pParentGpu;
    NvU64             vaAllocMax;
    NV_STATUS         status = NV_OK;

    VAS_ALLOC_FLAGS flags = {0};

    pParentGpu  = gpumgrGetParentGPU(pGpu);

    if (!gpumgrIsParentGPU(pGpu))
    {
        flags.bFixedAddressAllocate = NV_TRUE;
        pKernelBus->p2pPcie.writeMailboxBar1Addr = GPU_GET_KERNEL_BUS(pParentGpu)->p2pPcie.writeMailboxBar1Addr;
    }

    pKernelBus->p2pPcie.writeMailboxTotalSize =
        PCIE_P2P_WRITE_MAILBOX_SIZE * P2P_MAX_NUM_PEERS;
    vaAllocMax = NV_MIN(vaRangeMax,
        HOPPER_MAX_WRITE_MAILBOX_ADDR(pGpu) + PCIE_P2P_WRITE_MAILBOX_SIZE - 1);

    status = vaspaceAlloc(pKernelBus->bar1[gfid].pVAS,
                          pKernelBus->p2pPcie.writeMailboxTotalSize,
                          PCIE_P2P_WRITE_MAILBOX_SIZE,
                          0, vaAllocMax,
                          0,
                          flags,
                          &pKernelBus->p2pPcie.writeMailboxBar1Addr);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "cannot allocate vaspace for P2P write mailboxes (0x%x)\n",
                  status);
        goto kbusAllocP2PMailboxBar1_failed;
    }

    NV_ASSERT(GPU_GET_KERNEL_BUS(pParentGpu)->p2pPcie.writeMailboxBar1Addr == pKernelBus->p2pPcie.writeMailboxBar1Addr);

    NV_PRINTF(LEVEL_INFO,
              "[GPU%u] P2P write mailboxes allocated at BAR1 addr = 0x%llx\n",
              gpuGetInstance(pGpu), pKernelBus->p2pPcie.writeMailboxBar1Addr);

kbusAllocP2PMailboxBar1_failed:
    if (status != NV_OK)
    {
        pKernelBus->p2pPcie.writeMailboxBar1Addr  = PCIE_P2P_INVALID_WRITE_MAILBOX_ADDR;
        pKernelBus->p2pPcie.writeMailboxTotalSize = 0;
    }

    return status;
}

//
// Returns the P2P mailbox attributes such as:
// - pMailboxAreaSize: total size
// - pMailboxAlignmentBits: aligment in number of bits
// - pMailboxMaxOffset: max supported offset
//
void
kbusGetP2PMailboxAttributes_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32*     pMailboxAreaSize,
    NvU32*     pMailboxAlignmentSize,
    NvU32*     pMailboxBar1MaxOffset64KB
)
{
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    // Initialize null values by default
    if (pMailboxAreaSize != NULL)
    {
        *pMailboxAreaSize = 0;
    }
    if (pMailboxAlignmentSize != NULL)
    {
        *pMailboxAlignmentSize = 0;
    }
    if (pMailboxBar1MaxOffset64KB != NULL)
    {
        *pMailboxBar1MaxOffset64KB = 0;
    }

    if (pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_READS_DISABLED) &&
        pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_WRITES_DISABLED))
    {
        // Return null values
        return;
    }

    // Retrieve attributes
    if (pMailboxAreaSize != NULL)
    {
        *pMailboxAreaSize = PCIE_P2P_WRITE_MAILBOX_SIZE * P2P_MAX_NUM_PEERS;
    }

    if (pMailboxAlignmentSize != NULL)
    {
        // Write mailbox data window needs to be 64KB aligned.
        *pMailboxAlignmentSize = 0x10000;
    }

    if (pMailboxBar1MaxOffset64KB != NULL)
    {
        *pMailboxBar1MaxOffset64KB =
            NvU64_LO32(
                (HOPPER_MAX_WRITE_MAILBOX_ADDR(pGpu) + PCIE_P2P_WRITE_MAILBOX_SIZE) >> 16
            );
    }

    return;
}

RmPhysAddr
kbusSetupP2PDomainAccess_GH100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    PMEMORY_DESCRIPTOR *ppP2PDomMemDesc
)
{
    return kbusSetupPeerBarAccess(pGpu0, pGpu1,
                pGpu0->busInfo.gpuPhysAddr + DRF_BASE(NV_XAL_EP_P2P),
                DRF_SIZE(NV_XAL_EP_P2P), ppP2PDomMemDesc);
}

NV_STATUS
kbusFlushPcieForBar0Doorbell_GH100
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus
)
{
    return kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_VIDEO_MEMORY);
}

/*!
 * @brief Create a P2P mapping to a given peer GPU
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   pKernelBus1    (remote GPU)
 * @param[out]  peer0  Peer ID (local to remote)
 * @param[out]  peer1  Peer ID (remote to local)
 * @param[in]   attributes Sepcial attributes for the mapping
 *
 * return NV_OK on success
 */
NV_STATUS
kbusCreateP2PMapping_GH100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1,
    NvU32     *peer0,
    NvU32     *peer1,
    NvU32      attributes
)
{
    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _C2C, attributes))
    {
        return kbusCreateP2PMappingForC2C_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
    }

    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _NVLINK, attributes) ||
        FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _NVLINK_INDIRECT, attributes))
    {
        return kbusCreateP2PMappingForNvlink_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
    }

    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _PCIE_BAR1, attributes))
    {
        return kbusCreateP2PMappingForBar1P2P_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, attributes);
    }

    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _PCIE, attributes))
    {
        return kbusCreateP2PMappingForMailbox_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
    }

    NV_PRINTF(LEVEL_ERROR, "P2P type %d is not supported\n", DRF_VAL(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, attributes));

    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Remove the P2P mapping to a given peer GPU
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   pKernelBus1    (remote GPU)
 * @param[out]  peer0  Peer ID (local to remote)
 * @param[out]  peer1  Peer ID (remote to local)
 * @param[in]   attributes Sepcial attributes for the mapping
 *
 * return NV_OK on success
 */
NV_STATUS
kbusRemoveP2PMapping_GH100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1,
    NvU32      peer0,
    NvU32      peer1,
    NvU32      attributes
)
{
    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _C2C, attributes))
    {
        return kbusRemoveP2PMappingForC2C_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
    }

    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _NVLINK, attributes) ||
        FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _NVLINK_INDIRECT, attributes))
    {
        return kbusRemoveP2PMappingForNvlink_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
    }

    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _PCIE_BAR1, attributes))
    {
        return kbusRemoveP2PMappingForBar1P2P_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, attributes);
    }

    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _PCIE, attributes))
    {
        return kbusRemoveP2PMappingForMailbox_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
    }

    NV_PRINTF(LEVEL_ERROR, "P2P type %d is not supported\n", DRF_VAL(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, attributes));

    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Returns the peer number from pGpu (Local) to pGpuPeer
 *
 * @param[in] pGpu          Local
 * @param[in] pKernelBus    Local
 * @param[in] pGpuPeer      Remote
 *
 * @returns NvU32 bus peer number
 */
NvU32
kbusGetPeerId_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    OBJGPU    *pGpuPeer
)
{
    NvU32   gpuPeerInst = gpuGetInstance(pGpuPeer);
    NvU32   peerId      = pKernelBus->c2cPeerInfo.busC2CPeerNumberMask[gpuPeerInst];

    // Fall back to Nvlink
    if (peerId == 0)
    {
        NV_PRINTF(LEVEL_INFO,
                  "C2C P2P not set up between GPU%u and GPU%u, checking for Nvlink...\n",
                  gpuGetInstance(pGpu), gpuPeerInst);
        return kbusGetPeerId_GP100(pGpu, pKernelBus, pGpuPeer);
    }

    LOWESTBITIDX_32(peerId);
    return peerId;
}

/**
 * @brief      Returns if the given peerId is a valid for a given GPU
 *
 * @param[in]  pGpu
 * @param[in]  pKernelBus
 * @param[in]  peerId       The peer identifier
 *
 * @return     return NV_OK is valid
 */
NV_STATUS
kbusIsPeerIdValid_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      peerId
)
{
    NV_ASSERT_OR_RETURN(peerId < P2P_MAX_NUM_PEERS, NV_ERR_INVALID_INDEX);
    if (pKernelBus->c2cPeerInfo.busC2CPeerNumberMask[gpuGetInstance(pGpu)] & NVBIT(peerId))
        return NV_OK;
    return kbusIsPeerIdValid_GP100(pGpu, pKernelBus, peerId);
}

/*!
 * @brief Create C2C mappings for FB memory
 * When this is called, we should not have any BAR1/BAR2 mappings
 *
 * @param[in] pGpu                  OBJGPU pointer
 * @param[in] pKernelBus            Kernel bus pointer
 * @param[in] numaOnlineMemorySize  Size of FB memory to online in
 *                                  kernel as a NUMA node
 * @param[in] bFlush                Flush CPU cache or not
 *
 * @return 'NV_OK' if successful, an RM error code otherwise.
 */
NV_STATUS
kbusCreateCoherentCpuMapping_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU64     numaOnlineMemorySize,
    NvBool    bFlush
)
{
    MemoryManager      *pMemoryManager             = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelMemorySystem *pKernelMemorySystem        = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NV_STATUS           status                     = NV_OK;
    KernelBif          *pKernelBif                 = GPU_GET_KERNEL_BIF(pGpu);
    NvP64               pCpuMapping                = NvP64_NULL;
    NvU64               fbSize;
    NvU64               busAddrStart[COHERENT_CPU_MAPPING_TOTAL_REGIONS];
    NvU64               busAddrSize[COHERENT_CPU_MAPPING_TOTAL_REGIONS];
    NvU32               i;
    NvU64               memblockSize;
    NvU32               cachingMode[COHERENT_CPU_MAPPING_TOTAL_REGIONS];

    NV_ASSERT_OR_RETURN(gpuIsSelfHosted(pGpu) && pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_C2C_LINK_UP), NV_ERR_INVALID_STATE);

    // Assert no BAR1/BAR2 mappings
    NV_ASSERT_OR_RETURN(kbusGetBar1VASpace_HAL(pGpu, pKernelBus) == NULL,
                        NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(listCount(&pKernelBus->virtualBar2[GPU_GFID_PF].usedMapList) == 0,
                        NV_ERR_INVALID_STATE);

    fbSize = (pMemoryManager->Ram.fbTotalMemSizeMb << 20);

    NV_ASSERT_OK_OR_RETURN(osNumaMemblockSize(&memblockSize));

    pKernelBus->coherentCpuMapping.nrMapping = 2;

    pKernelBus->coherentCpuMapping.physAddr[COHERENT_CPU_MAPPING_REGION_0] = pMemoryManager->Ram.fbRegion[0].base;
    pKernelBus->coherentCpuMapping.size[COHERENT_CPU_MAPPING_REGION_0] = numaOnlineMemorySize;
    cachingMode[COHERENT_CPU_MAPPING_REGION_0] = NV_MEMORY_CACHED;

    pKernelBus->coherentCpuMapping.physAddr[COHERENT_CPU_MAPPING_RM_RESV_REGION] =
        pKernelBus->coherentCpuMapping.physAddr[COHERENT_CPU_MAPPING_REGION_0] +
        pKernelBus->coherentCpuMapping.size[COHERENT_CPU_MAPPING_REGION_0];
    pKernelBus->coherentCpuMapping.size[COHERENT_CPU_MAPPING_RM_RESV_REGION] =
        fbSize - pKernelBus->coherentCpuMapping.size[COHERENT_CPU_MAPPING_REGION_0];

    for (i = COHERENT_CPU_MAPPING_REGION_0; i < pKernelBus->coherentCpuMapping.nrMapping; ++i)
    {
        busAddrStart[i] = pKernelMemorySystem->coherentCpuFbBase +
            pKernelBus->coherentCpuMapping.physAddr[i];
        busAddrSize[i]  = pKernelBus->coherentCpuMapping.size[i];
    }

    if (pKernelMemorySystem->bBug3656943WAR)
    {
        if (IS_PASSTHRU(pGpu))
        {
            //
            // For passthrough case, reserved memory guest physical address
            // comes from BAR1 address.
            //
            busAddrStart[COHERENT_CPU_MAPPING_RM_RESV_REGION] =
                pKernelMemorySystem->coherentRsvdFbBase;
        }

        //
        // RM reserved region should be mapped as Normal Non-cacheable as a SW WAR
        // for the bug 3656943. NV_MEMORY_WRITECOMBINED translates to linux
        // kernel ioremap_wc which actually uses the normal non-cacheable type
        // PROT_NORMAL_NC
        //
        cachingMode[COHERENT_CPU_MAPPING_RM_RESV_REGION] = NV_MEMORY_WRITECOMBINED;
    }
    else
    {
        cachingMode[COHERENT_CPU_MAPPING_RM_RESV_REGION] = NV_MEMORY_CACHED;
    }

    for (i = COHERENT_CPU_MAPPING_REGION_0; i < pKernelBus->coherentCpuMapping.nrMapping; ++i)
    {

        // In SHH, CPU uses coherent C2C link to access GPU memory and hence it can be accessed cached.
        status = osMapPciMemoryKernel64(pGpu,
                                        (NvUPtr)busAddrStart[i],
                                        (NvU64)busAddrSize[i],
                                        NV_PROTECT_READ_WRITE,
                                        &(pCpuMapping),
                                        cachingMode[i]);

        NV_ASSERT_OR_RETURN(status == NV_OK, NV_ERR_GENERIC);

        pKernelBus->coherentCpuMapping.pCpuMapping[i] = (NvP64)pCpuMapping;
        NV_PRINTF(LEVEL_INFO, "coherent link mapping. i: %d base: 0x%llx size: 0x%llx \n",
                  i, busAddrStart[i], busAddrSize[i]);

        NV_ASSERT_OR_RETURN(bFlush == NV_FALSE, NV_ERR_NOT_SUPPORTED);

        // Counts the number of outstanding mappings in FB.
        pKernelBus->coherentCpuMapping.refcnt[i] = 0;
    }

    pKernelBus->coherentCpuMapping.bCoherentCpuMapping  = NV_TRUE;

    NV_PRINTF(LEVEL_INFO, "Enabling CPU->C2C->FBMEM path\n");

    return status;
}

/*!
 * @brief Sanity test coherent link between CPU and GPU.
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelBus Kernel bus pointer
 *
 * @returns NV_OK on success.
 */
NV_STATUS
kbusVerifyCoherentLink_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NvU64             size             = 0x100;
    MEMORY_DESCRIPTOR *pMemDesc        = NULL;
    NvU8              *pOffset         = NULL;
    const NvU32       sampleData       = 0x12345678;
    NV_STATUS         status           = NV_OK;
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NvU32             index            = 0;
    NvU32             flagsClean       = 0;
    MEMORY_DESCRIPTOR memDesc;

    // Skip the test if 0FB configuration is used.
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM))
    {
        NV_PRINTF_COND(IS_EMULATION(pGpu), LEVEL_ERROR, LEVEL_INFO, "Skipping Coherent link test\n");
        return NV_OK;
    }

    memdescCreateExisting(&memDesc, pGpu, size, ADDR_FBMEM, NV_MEMORY_CACHED, MEMDESC_FLAGS_NONE);

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_95, 
                    (&memDesc));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Could not allocate vidmem to test coherent link with\n");
        DBG_BREAKPOINT();
        status = NV_ERR_NO_MEMORY;
        goto busVerifyCoherentLink_failed;
    }

    pOffset = kbusMapRmAperture_HAL(pGpu, &memDesc);
    if (pOffset == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto busVerifyCoherentLink_failed;
    }
    pMemDesc = &memDesc;

    NV_PRINTF_COND(IS_EMULATION(pGpu), LEVEL_ERROR, LEVEL_INFO,
                   "Coherent link test buffer PA: 0x%llx\n",
                   memdescGetPhysAddr(pMemDesc, AT_CPU, 0));

    for(index = 0; index < size; index += 4)
    {
        MEM_WR32(pOffset + index, sampleData);
    }

    // Ensure the writes are flushed out of the CPU caches.
    osFlushGpuCoherentCpuCacheRange(pGpu->pOsGpuInfo, (NvUPtr)pOffset, size);

    // L2 invalidate registers are not present in VF BAR0. PF driver should have tested this on host so
    // skipping this on SRIOV guest
    if (!IS_VIRTUAL(pGpu))
    {
        flagsClean = NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_ALL |
                     NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_CLEAN;
        if (kmemsysIsL2CleanFbPull(pKernelMemorySystem))
        {
            flagsClean |= NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_WAIT_FB_PULL;
        }

        status = kmemsysSendL2InvalidateEvict(pGpu, pKernelMemorySystem, flagsClean);
        if (NV_OK != status)
        {
            NV_PRINTF(LEVEL_ERROR, "L2 evict failed\n");
            goto busVerifyCoherentLink_failed;
        }
    }

    for(index = 0; index < size; index += 4)
    {
        NvU32 readbackData = MEM_RD32(pOffset + index);

        if (readbackData != sampleData)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Coherent Link test readback VA = 0x%llx returned garbage 0x%x\n",
                      (NvUPtr)(pOffset + index), readbackData);

            DBG_BREAKPOINT_REASON(NV_ERR_MEMORY_ERROR);
            status = NV_ERR_GENERIC;
        }
    }

busVerifyCoherentLink_failed:
    if (pOffset != NULL)
    {
        kbusUnmapRmAperture_HAL(pGpu, pMemDesc, &pOffset, NV_TRUE);
    }
    memdescFree(pMemDesc);
    memdescDestroy(pMemDesc);

    if (status == NV_OK)
    {
        NV_PRINTF_COND(IS_EMULATION(pGpu), LEVEL_ERROR, LEVEL_INFO, "Coherent link test passes\n");
    }

    return status;

}

/**
 * Determine if memory described in memdesc is within a specific fb region
 *
 * @param[in] pKernelBus    Kernel bus pointer
 * @param[in] pMemDesc      Pointer to memdesc describing memory range
 * @param[in] offset        Offset from base address to check
 * @param[in] region        Fb region to test against
 *
 * @return  Whether or not described memory range is within fb region.
 */
static NvBool
_kbusMemoryIsInFbRegion
(
    KernelBus           *pKernelBus,
    MEMORY_DESCRIPTOR   *pMemDesc,
    NvU64                offset,
    NvU8                 region
)
{
    RmPhysAddr startAddr = memdescGetPhysAddr(pMemDesc, FORCE_VMMU_TRANSLATION(pMemDesc, AT_GPU), offset);
    RmPhysAddr rangeStart;
    RmPhysAddr rangeEnd;

    NV_ASSERT_OR_RETURN(region < pKernelBus->coherentCpuMapping.nrMapping, NV_FALSE);

    rangeStart = pKernelBus->coherentCpuMapping.physAddr[region];
    rangeEnd = pKernelBus->coherentCpuMapping.physAddr[region] +
               pKernelBus->coherentCpuMapping.size[region] - 1;

    return (rangeStart <= startAddr) && (startAddr <= rangeEnd);
}

/**
 * Helper function to map coherent cpu mapping.
 *
 * @param[in]  pGpu       Pointer to GPU
 * @param[in]  pKernelBus Kernel bus pointer
 * @param[in]  pMemDesc   Pointer to memdesc that is to be mapped
 * @param[in]  offset     Offset from base address given in memdesc
 * @param[in]  length     Length of memory to map
 * @param[in]  protect    Protection flags
 * @param[out] ppAddress  Virtual address of mapping
 * @param[out] ppPriv     Private data to be retained for unmapping
 *
 * @return NV_OK or errors if failed to map
 */
NV_STATUS
kbusMapCoherentCpuMapping_GH100
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
    NvU8       regionIndex  = COHERENT_CPU_MAPPING_RM_KERNEL_ONLINED_REGION;
    RmPhysAddr regionOffset = 0;
    RmPhysAddr startAddr    = memdescGetPhysAddr(pMemDesc, FORCE_VMMU_TRANSLATION(pMemDesc, AT_GPU), offset);
    NvU8       i            = 0;

    //
    // VGPU does not online memory, yet has multiple regions, so we need to use
    // static mapping for the "onlined" region.
    //
    if (!hypervisorIsVgxHyper())
    {
        if (_kbusMemoryIsInFbRegion(pKernelBus, pMemDesc, offset, COHERENT_CPU_MAPPING_RM_KERNEL_ONLINED_REGION))
        {
            return osMapSystemMemory(pMemDesc, offset, length, NV_TRUE, protect, ppAddress, ppPriv);
        }
        regionIndex = COHERENT_CPU_MAPPING_RM_RESV_REGION;
    }

    NV_ASSERT_OR_RETURN(memdescGetContiguity(pMemDesc, AT_CPU), NV_ERR_NOT_SUPPORTED);
    for (i = regionIndex; i < pKernelBus->coherentCpuMapping.nrMapping; ++i)
    {
        if (_kbusMemoryIsInFbRegion(pKernelBus, pMemDesc, offset, i))
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
kbusUnmapCoherentCpuMapping_GH100
(
    OBJGPU              *pGpu,
    KernelBus           *pKernelBus,
    PMEMORY_DESCRIPTOR   pMemDesc,
    NvP64                pAddress,
    NvP64                pPriv
)
{
    NvU8 regionIndex = COHERENT_CPU_MAPPING_RM_KERNEL_ONLINED_REGION;
    NvU8 i           = 0;

    //
    // VGPU does not online memory, yet has multiple regions, so we need to use
    // static mapping for the "onlined" region.
    //
    if (!hypervisorIsVgxHyper())
    {
        if (_kbusMemoryIsInFbRegion(pKernelBus, pMemDesc, 0, COHERENT_CPU_MAPPING_RM_KERNEL_ONLINED_REGION))
        {
            kbusFlush_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), BUS_FLUSH_VIDEO_MEMORY);
            osUnmapSystemMemory(pMemDesc, NV_TRUE, 0, pAddress, pPriv);
            kbusFlush_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), BUS_FLUSH_VIDEO_MEMORY);
            return;
        }
        regionIndex = COHERENT_CPU_MAPPING_RM_RESV_REGION;
    }

    NV_ASSERT_OR_RETURN_VOID(memdescGetContiguity(pMemDesc, AT_CPU));
    for (i = regionIndex; i < pKernelBus->coherentCpuMapping.nrMapping; ++i)
    {
        if (_kbusMemoryIsInFbRegion(pKernelBus, pMemDesc, 0, i))
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

/*!
 * @brief check if it can support BAR1 P2P between the specified GPUs
 *        At the point this function is called, the system do not support C2C and
 *        NVLINK P2P and the BAR1 P2P is the preferred option.
 *
 * @param[in]   pGpu0         (local GPU)
 * @param[in]   pKernelBus0   (local GPU)
 * @param[in]   pGpu1         (remote GPU)
 * @param[in]   pKernelBus1   (remote GPU)
 *
 * return NV_TRUE if it supports BAR1 P2P between the specified GPUs
 */
NvBool
kbusIsPcieBar1P2PMappingSupported_GH100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1
)
{
    NvU32   gpuInst0 = gpuGetInstance(pGpu0);
    NvU32   gpuInst1 = gpuGetInstance(pGpu1);

    // Not loopback support
    if (pGpu0 == pGpu1)
    {
        return NV_FALSE;
    }

    // Both of GPUs need to have the static bar1 enabled
    if (!kbusIsStaticBar1Enabled(pGpu0, pKernelBus0) ||
        !kbusIsStaticBar1Enabled(pGpu1, pKernelBus1))
    {
        return NV_FALSE;
    }

    //
    // RM only supports one type of PCIE P2P protocol, either BAR1 P2P or mailbox P2P, between
    // two GPUs at a time. For more info on this topic, please check bug 3274549 comment 10
    //
    // Check if there is p2p mailbox connection between the GPUs.
    //
    if ((pKernelBus0->p2pPcie.peerNumberMask[gpuInst1] != 0) ||
        (pKernelBus1->p2pPcie.peerNumberMask[gpuInst0] != 0))
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

/*!
 *  @brief Remove source GPU IOMMU mapping for the peer GPU
 *
 *  @param[in]  pSrcGpu             The source GPU
 *  @param[in]  pSrcKernelBus       The source Kernel Bus
 *  @param[in]  pPeerGpu            The peer GPU
 *
 *  @returns void
 */
static void
_kbusRemoveStaticBar1IOMMUMapping
(
    OBJGPU    *pSrcGpu,
    KernelBus *pSrcKernelBus,
    OBJGPU    *pPeerGpu,
    KernelBus *pPeerKernelBus
)
{
    NvU32 peerGfid;

    NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                            vgpuGetCallingContextGfid(pPeerGpu, &peerGfid) == NV_OK);

    NV_ASSERT_OR_RETURN_VOID(pPeerKernelBus->bar1[peerGfid].staticBar1.pDmaMemDesc != NULL);

    memdescUnmapIommu(pPeerKernelBus->bar1[peerGfid].staticBar1.pDmaMemDesc,
                      pSrcGpu->busInfo.iovaspaceId);
}

/*!
 *  @brief Remove GPU IOMMU mapping between the pair of GPUs
 *
 *  @param[in]  pGpu0
 *  @param[in]  pKernelBus0
 *  @param[in]  pGpu1
 *  @param[in]  pKernelBus0
 *
 *  @returns void
 */
static void
_kbusRemoveStaticBar1IOMMUMappingForGpuPair
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1
)
{
    _kbusRemoveStaticBar1IOMMUMapping(pGpu0, pKernelBus0, pGpu1, pKernelBus1);
    _kbusRemoveStaticBar1IOMMUMapping(pGpu1, pKernelBus1, pGpu0, pKernelBus0);
}

/*!
 *  @brief Create source GPU IOMMU mapping for the peer GPU
 *
 *  @param[in]  pSrcGpu             The source GPU
 *  @param[in]  pSrcKernelBus       The source Kernel Bus
 *  @param[in]  pPeerGpu            The peer GPU
 *  @param[in]  pPeerKernelBus      The peer Kernel Bus
 *
 *  @returns NV_OK on success
 */
static NV_STATUS
_kbusCreateStaticBar1IOMMUMapping
(
    OBJGPU    *pSrcGpu,
    KernelBus *pSrcKernelBus,
    OBJGPU    *pPeerGpu,
    KernelBus *pPeerKernelBus
)
{
    NvU32 peerGpuGfid;
    MEMORY_DESCRIPTOR *pPeerDmaMemDesc = NULL;
    RmPhysAddr peerDmaAddr;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pPeerGpu, &peerGpuGfid));

    pPeerDmaMemDesc = pPeerKernelBus->bar1[peerGpuGfid].staticBar1.pDmaMemDesc;

    NV_ASSERT_OR_RETURN(pPeerDmaMemDesc != NULL, NV_ERR_INVALID_STATE);

    // Create the source GPU IOMMU mapping on the peer static bar1
    NV_ASSERT_OK_OR_RETURN(memdescMapIommu(pPeerDmaMemDesc,
                                           pSrcGpu->busInfo.iovaspaceId));

    // To get the peer DMA address of the memory for the GPU was mapped to
    memdescGetPhysAddrsForGpu(pPeerDmaMemDesc, pSrcGpu,
                              AT_GPU, 0, 0, 1, &peerDmaAddr);

    // Check the if it is aligned to max RM_PAGE_SIZE 512M.
    if (!NV_IS_ALIGNED64(peerDmaAddr, RM_PAGE_SIZE_512M))
    {
        NV_PRINTF(LEVEL_ERROR, "The peer DMA address 0x%llx is not aligned at 0x%llx\n",
                               peerDmaAddr, RM_PAGE_SIZE_512M);

        memdescUnmapIommu(pPeerDmaMemDesc, pSrcGpu->busInfo.iovaspaceId);

        return NV_ERR_INVALID_ADDRESS;
    }

    return NV_OK;
}

/*!
 *  @brief To create IOMMU mapping between the pair of GPUs
 *
 *  @param[in]  pGpu0
 *  @param[in]  pKernelBus0
 *  @param[in]  pGpu1
 *  @param[in]  pKernelBus0
 *
 *  @returns NV_OK on success
 */
static NV_STATUS
_kbusCreateStaticBar1IOMMUMappingForGpuPair
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1
)
{
    NvU32 gpuInst0 = gpuGetInstance(pGpu0);
    NvU32 gpuInst1 = gpuGetInstance(pGpu1);
    NV_STATUS status;

    // Create GPU0 IOMMU mapping to GPU1 BAR1
    status = _kbusCreateStaticBar1IOMMUMapping(pGpu0, pKernelBus0, pGpu1, pKernelBus1);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "IOMMU mapping failed from GPU%u to GPU%u\n",
                  gpuInst0, gpuInst1);
        return status;
    }

    // Create GPU1 IOMMU mapping to GPU0 BAR1
    status = _kbusCreateStaticBar1IOMMUMapping(pGpu1, pKernelBus1, pGpu0, pKernelBus0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "IOMMU mapping failed from GPU%u to GPU%u\n",
                  gpuInst1, gpuInst0);

        // Remove the previous created IOMMU mapping
        _kbusRemoveStaticBar1IOMMUMapping(pGpu0, pKernelBus0, pGpu1, pKernelBus1);
    }

    return status;
}

/*!
 *  @brief To get the DMA information from the source GPU to the peer GPU
 *
 *  @param[in]  pSrcGpu             The source GPU
 *  @param[in]  pPeerGpu            The peer GPU
 *  @param[in]  pPeerKernelBus      The peer Kernel Bus
 *  @param[out] pDmaAddress         The start DMA address for the source GPU
 *                                  to access the peer GPU
 *  @param[out] pDmaSize            The size of the DMA transfer range
 *
 *  @returns NV_OK on success
 */
NV_STATUS kbusGetBar1P2PDmaInfo_GH100
(
    OBJGPU      *pSrcGpu,
    OBJGPU      *pPeerGpu,
    KernelBus   *pPeerKernelBus,
    NvU64       *pDmaAddress,
    NvU64       *pDmaSize
)
{
    NvU32 peerGfid;
    MEMORY_DESCRIPTOR *pPeerDmaMemDesc;

    NV_ASSERT_OR_RETURN((pDmaAddress != NULL) && (pDmaSize != NULL),
                        NV_ERR_INVALID_ARGUMENT);

    // Set the default value
    *pDmaAddress = NV_U64_MAX;
    *pDmaSize = 0;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pPeerGpu, &peerGfid));

    pPeerDmaMemDesc = pPeerKernelBus->bar1[peerGfid].staticBar1.pDmaMemDesc;
    NV_ASSERT_OR_RETURN(pPeerDmaMemDesc != NULL, NV_ERR_NOT_SUPPORTED);

    // Get the peer GPU DMA address for the source GPU
    memdescGetPhysAddrsForGpu(pPeerDmaMemDesc, pSrcGpu,
                              AT_GPU, 0, 0, 1, pDmaAddress);

    *pDmaSize = memdescGetSize(pPeerDmaMemDesc);

    return NV_OK;
}

/*!
 * @brief check if there is BAR1 P2P mapping between given GPUs
 *
 * @param[in]   pGpu0         (local GPU)
 * @param[in]   pKernelBus0   (local GPU)
 * @param[in]   pGpu1         (remote GPU)
 * @param[in]   pKernelBus1   (remote GPU)
 *
 * return NV_TRUE if the P2P is using BAR1
 */
NvBool
kbusHasPcieBar1P2PMapping_GH100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1
)
{
    return ((pKernelBus0->p2pPcieBar1.busBar1PeerRefcount[gpuGetInstance(pGpu1)] != 0) &&
            (pKernelBus1->p2pPcieBar1.busBar1PeerRefcount[gpuGetInstance(pGpu0)] != 0));
}

/*!
 * @brief Create a Bar1 P2P mapping to given GPUs
 *
 * @param[in]   pGpu0         (local GPU)
 * @param[in]   pKernelBus0   (local GPU)
 * @param[in]   pGpu1         (remote GPU)
 * @param[in]   pKernelBus1   (remote GPU)
 * @param[in]   attributes    attributes to control the mapping
 *
 * return NV_OK on success
 *        NV_ERR_NOT_SUPPORTED if it fails
 */
NV_STATUS
kbusCreateP2PMappingForBar1P2P_GH100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1,
    NvU32      attributes
)
{
    NvU32 gpuInst0 = gpuGetInstance(pGpu0);
    NvU32 gpuInst1 = gpuGetInstance(pGpu1);
    NV_STATUS status = NV_OK;

    if (IS_VIRTUAL(pGpu0) || IS_VIRTUAL(pGpu1))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (!kbusIsPcieBar1P2PMappingSupported_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // Only create IOMMU mapping between the pair of GPUs at the first time.
    if ((pKernelBus0->p2pPcieBar1.busBar1PeerRefcount[gpuInst1] == 0) &&
        (pKernelBus1->p2pPcieBar1.busBar1PeerRefcount[gpuInst0] == 0))
    {
        NV_ASSERT_OK_OR_RETURN(_kbusCreateStaticBar1IOMMUMappingForGpuPair(pGpu0, pKernelBus0,
                                                                           pGpu1, pKernelBus1));
    }

    pKernelBus0->p2pPcieBar1.busBar1PeerRefcount[gpuInst1]++;
    pKernelBus1->p2pPcieBar1.busBar1PeerRefcount[gpuInst0]++;

    NV_PRINTF(LEVEL_INFO, "added PCIe BAR1 P2P mapping between GPU%u and GPU%u\n",
              gpuInst0, gpuInst1);

    return status;
}

/*!
 * @brief remove a Bar1 P2P mapping to given GPUs
 *
 * @param[in]   pGpu0         (local GPU)
 * @param[in]   pKernelBus0   (local GPU)
 * @param[in]   pGpu1         (remote GPU)
 * @param[in]   pKernelBus1   (remote GPU)
 * @param[in]   attributes    attributes of the P2P
 *
 * return NV_OK on success
 */
NV_STATUS
kbusRemoveP2PMappingForBar1P2P_GH100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1,
    NvU32      attributes
)
{
    NvU32 gpuInst0, gpuInst1;

    if (IS_VIRTUAL(pGpu0) || IS_VIRTUAL(pGpu1))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    gpuInst0 = gpuGetInstance(pGpu0);
    gpuInst1 = gpuGetInstance(pGpu1);

    if ((pKernelBus0->p2pPcieBar1.busBar1PeerRefcount[gpuInst1] == 0) ||
        (pKernelBus1->p2pPcieBar1.busBar1PeerRefcount[gpuInst0] == 0))
    {
        return NV_ERR_INVALID_STATE;
    }

    pKernelBus0->p2pPcieBar1.busBar1PeerRefcount[gpuInst1]--;
    pKernelBus1->p2pPcieBar1.busBar1PeerRefcount[gpuInst0]--;

    // Only remove the IOMMU mapping between the pair of GPUs when it is the last mapping.
    if ((pKernelBus0->p2pPcieBar1.busBar1PeerRefcount[gpuInst1] == 0) &&
        (pKernelBus1->p2pPcieBar1.busBar1PeerRefcount[gpuInst0] == 0))
    {
        _kbusRemoveStaticBar1IOMMUMappingForGpuPair(pGpu0, pKernelBus0, pGpu1, pKernelBus1);
    }

    NV_PRINTF(LEVEL_INFO, "removed PCIe BAR1 P2P mapping between GPU%u and GPU%u\n",
                          gpuInst0, gpuInst1);

    return NV_OK;
}

/*!
 * @brief Returns the C2C peer ID from pGpu0 to pGpu1 after
 *        reserving it if peer mapping doesn't exist already
 *        for the GPU pair
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   pKernelBus1    (remote GPU)
 * @param[out]  c2cPeer        NvU32  pointer contains the peer ID to use
 *                             for local GPU to remote GPU when return value
 *                             is NV_OK
 *
 * return NV_OK on success
 */
static NV_STATUS
_kbusGetC2CP2PPeerId
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1,
    NvU32     *c2cPeer
)
{
    NV_STATUS  status       = NV_OK;
    return status;
}

/*!
 * @brief Create C2C mapping to a given peer GPU
 *
 *
 * @param[in]   pGpu0         (Local)
 * @param[in]   pKernelBus0   (Local)
 * @param[in]   pGpu1         (Remote)
 * @param[in]   peerId  Peer ID to use for local GPU to
 *              remote GPU mapping.
 *
 * return NV_OK on success
 */
static NV_STATUS
_kbusCreateC2CPeerMapping
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    NvU32      peerId
)
{
    NvU32      gpuInstance = gpuGetInstance(pGpu1);
    RM_API    *pRmApi      = GPU_GET_PHYSICAL_RMAPI(pGpu0);
    NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS params = {0};
    NV_STATUS  status = NV_OK;

    //
    // Increment the mapping refcount per peerID - since there is a new mapping that
    // will use this peerID
    //
    pKernelBus0->c2cPeerInfo.busC2CMappingRefcountPerPeerId[peerId]++;

    // Set the peer IDs in the corresponding peer number masks
    pKernelBus0->c2cPeerInfo.busC2CPeerNumberMask[gpuInstance] |= NVBIT(peerId);

    params.peerId = peerId;
    status = pRmApi->Control(pRmApi,
                             pGpu0->hInternalClient,
                             pGpu0->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING,
                             &params,
                             sizeof(NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS));
    NV_ASSERT(status == NV_OK);

    return status;
}

/*!
 * @brief Create a C2C P2P mapping to a given peer GPU
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   pKernelBus1    (remote GPU)
 * @param[out]  peer0  Peer ID (local to remote)
 * @param[out]  peer1  Peer ID (remote to local)
 *
 * return NV_OK on success
 */
NV_STATUS
kbusCreateP2PMappingForC2C_GH100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1,
    NvU32     *peer0,
    NvU32     *peer1,
    NvU32      attributes
)
{
    NvU32              gpu0Instance   = gpuGetInstance(pGpu0);
    NvU32              gpu1Instance   = gpuGetInstance(pGpu1);
    NvU32              c2cPeer0;
    NvU32              c2cPeer1;
    NV_STATUS          status;

    if (IS_VIRTUAL(pGpu0) || IS_VIRTUAL(pGpu1))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (peer0 == NULL || peer1 == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((*peer0 != BUS_INVALID_PEER && *peer0 >= P2P_MAX_NUM_PEERS) ||
        (*peer1 != BUS_INVALID_PEER && *peer1 >= P2P_MAX_NUM_PEERS))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    c2cPeer0 = *peer0;
    c2cPeer1 = *peer1;

    // Get the peer ID pGpu0 should use for P2P over C2C to pGpu1
    if ((status = _kbusGetC2CP2PPeerId(pGpu0, pKernelBus0,
                                       pGpu1, pKernelBus1,
                                       &c2cPeer0)) != NV_OK)
    {
        return status;
    }

    // Get the peer ID pGpu1 should use for P2P over C2C to pGpu0
    if ((status = _kbusGetC2CP2PPeerId(pGpu1, pKernelBus1,
                                       pGpu0, pKernelBus0,
                                       &c2cPeer1)) != NV_OK)
    {
        return status;
    }

    if ((c2cPeer0 == BUS_INVALID_PEER) || (c2cPeer1 == BUS_INVALID_PEER))
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to create C2C P2P mapping between GPU%u and GPU%u\n",
                                gpu0Instance, gpu1Instance);

        return NV_ERR_INVALID_REQUEST;
    }

    *peer0 = c2cPeer0;
    *peer1 = c2cPeer1;

    //
    // Does the mapping already exist between the given pair of GPUs using the peerIDs
    // *peer0 and *peer1 respectively ?
    //
    if ((pKernelBus0->c2cPeerInfo.busC2CPeerNumberMask[gpu1Instance] & NVBIT(*peer0)) &&
        (pKernelBus1->c2cPeerInfo.busC2CPeerNumberMask[gpu0Instance] & NVBIT(*peer1)))
    {
        //
        // Increment the mapping refcount per peerID - since there is another usage
        // of a mapping that is using this peerID
        //
        pKernelBus0->c2cPeerInfo.busC2CMappingRefcountPerPeerId[*peer0]++;
        pKernelBus1->c2cPeerInfo.busC2CMappingRefcountPerPeerId[*peer1]++;

        NV_PRINTF(LEVEL_INFO,
                  "- P2P: Peer mapping is already in use for gpu instances %x and %x "
                  "with peer id's %d and %d. Increasing the mapping refcounts for the"
                  " peer IDs to %d and %d respectively.\n",
                  gpu0Instance, gpu1Instance, *peer0, *peer1,
                  pKernelBus0->c2cPeerInfo.busC2CMappingRefcountPerPeerId[*peer0],
                  pKernelBus1->c2cPeerInfo.busC2CMappingRefcountPerPeerId[*peer1]);

        return NV_OK;
    }

    //
    // Reached here implies the mapping between the given pair of GPUs using the peerIDs
    // *peer0 and *peer1 does not exist. Create the mapping
    //

    NV_ASSERT_OK_OR_RETURN(_kbusCreateC2CPeerMapping(pGpu0, pKernelBus0, pGpu1, *peer0));
    NV_ASSERT_OK_OR_RETURN(_kbusCreateC2CPeerMapping(pGpu1, pKernelBus1, pGpu0, *peer1));

    NV_PRINTF(LEVEL_INFO,
              "added C2C P2P mapping between GPU%u (peer %u) and GPU%u (peer %u)\n",
              gpu0Instance, *peer0, gpu1Instance, *peer1);

    return NV_OK;
}

/*!
 * @brief Remove C2C mapping to a given peer GPU
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   peerId         Peer ID for local to remote GPU
 *
 * return NV_OK on success
 */
static NV_STATUS
_kbusRemoveC2CPeerMapping
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    NvU32      peerId
)
{
    NV_STATUS          status          = NV_OK;

    if (IS_VIRTUAL(pGpu0) || IS_VIRTUAL(pGpu1))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    return status;
}

/*!
 * @brief Remove the C2C P2P mapping to a pair of GPU
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   pKernelBus1    (remote GPU)
 * @param[in]   peer0          Peer ID (local to remote)
 * @param[in]   peer1          Peer ID (remote to local)
 *
 * return NV_OK on success
 */
NV_STATUS
kbusRemoveP2PMappingForC2C_GH100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1,
    NvU32      peer0,
    NvU32      peer1,
    NvU32      attributes
)
{
    NV_STATUS          status        = NV_OK;

    // Check if there's C2C mapping
    if (((pKernelBus0->c2cPeerInfo.busC2CPeerNumberMask[pGpu1->gpuInstance] & NVBIT(peer0)) == 0) ||
        ((pKernelBus1->c2cPeerInfo.busC2CPeerNumberMask[pGpu0->gpuInstance] & NVBIT(peer1)) == 0))
    {
        return NV_ERR_INVALID_STATE;
    }

    // C2C mapping exists, remove the C2C mapping
    NV_ASSERT_OK_OR_RETURN(_kbusRemoveC2CPeerMapping(pGpu0, pKernelBus0, pGpu1, peer0));
    NV_ASSERT_OK_OR_RETURN(_kbusRemoveC2CPeerMapping(pGpu1, pKernelBus1, pGpu0, peer1));

    return status;
}

void
kbusWriteP2PWmbTag_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      remote2Local,
    NvU64      p2pWmbTag
)
{
    // See bug 3558208 comment 34 and 50
    GPU_REG_RD32(pGpu, NV_XAL_EP_P2P_WREQMB_L(remote2Local));
    GPU_REG_WR32(pGpu, NV_XAL_EP_P2P_WREQMB_L(remote2Local), NvU64_LO32(p2pWmbTag));
    GPU_REG_WR32(pGpu, NV_XAL_EP_P2P_WREQMB_H(remote2Local), NvU64_HI32(p2pWmbTag));
}

/*!
 * @brief Determine FLA Base and Size for direct-connected and NvSwitch systems.
 *
 * @param[in]  base       VASpace base
 * @param[in]  size       VASpace size
 *
 * @return NV_OK if successful
 */
NV_STATUS
kbusDetermineFlaRangeAndAllocate_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU64      base,
    NvU64      size
)
{
    NV_STATUS      status        = NV_OK;

    OBJSYS *pSys = SYS_GET_INSTANCE();

    if ((pSys->getProperty(pSys, PDB_PROP_SYS_NVSWITCH_IS_PRESENT) ||
         GPU_IS_NVSWITCH_DETECTED(pGpu)) && !gpuFabricProbeIsSupported(pGpu))
    {
        return kbusDetermineFlaRangeAndAllocate_GA100(pGpu, pKernelBus, base, size);
    }

    NV_ASSERT_OK_OR_RETURN(kbusAllocateFlaVaspace_HAL(pGpu, pKernelBus, 0x0, NVBIT64(52)));

    return status;
}

/*!
 * @brief Sets up the Fabric FLA state for the GPU. This function will allocate fabric VASpace,
 *        allocates PDB for fabric VAS, allocates instance block and initialize with
 *        fabric VAS and binds the instance block to HW.
 *
 * @param[in]  base       VASpace base
 * @param[in]  size       VASpace size
 *
 * @return NV_OK if successful
 */
NV_STATUS
kbusAllocateFlaVaspace_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU64      base,
    NvU64      size
)
{
    NV_STATUS    status = NV_OK;
    OBJVMM      *pVmm   = SYS_GET_VMM(SYS_GET_INSTANCE());
    KernelGmmu  *pKernelGmmu  = GPU_GET_KERNEL_GMMU(pGpu);
    INST_BLK_INIT_PARAMS pInstblkParams = {0};
    FABRIC_VASPACE *pFabricVAS;
    RM_API   *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(size != 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(!pKernelBus->flaInfo.bFlaAllocated, NV_ERR_INVALID_ARGUMENT);

    pKernelBus->flaInfo.base = base;
    pKernelBus->flaInfo.size = size;

    OBJSYS *pSys = SYS_GET_INSTANCE();

    if ((pSys->getProperty(pSys, PDB_PROP_SYS_NVSWITCH_IS_PRESENT) ||
         GPU_IS_NVSWITCH_DETECTED(pGpu)) && !gpuFabricProbeIsSupported(pGpu))
    {
        return kbusAllocateFlaVaspace_GA100(pGpu, pKernelBus, base, size);
    }

    // TODO: Remove allocating legaccy FLA Vaspace once CUDA removes the dependency
    NV_ASSERT_OK_OR_RETURN(kbusAllocateLegacyFlaVaspace_HAL(pGpu, pKernelBus, base, size));

    // Allocate a FABRIC_VASPACE_A object
    status = vmmCreateVaspace(pVmm, FABRIC_VASPACE_A, pGpu->gpuId, gpumgrGetGpuMask(pGpu),
                              base, base + size - 1, 0, 0, NULL, 0,
                              &pGpu->pFabricVAS);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed allocating fabric vaspace, status=0x%x\n",
                  status);
        goto cleanup;
    }

    // Pin the VASPACE page directory for pFabricVAS before writing the instance block
    status = vaspacePinRootPageDir(pGpu->pFabricVAS, pGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed pinning down fabric vaspace, status=0x%x\n",
                    status);
        goto cleanup;
    }

    // Construct instance block
    status = kbusConstructFlaInstBlk_HAL(pGpu, pKernelBus, GPU_GFID_PF);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                "failed constructing instblk for FLA, status=0x%x\n",
                status);
        goto unpin_rootpagedir;
    }

    pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);

    // Instantiate Inst Blk for pFlaVAS
    status = kgmmuInstBlkInit(pKernelGmmu,
                                pKernelBus->flaInfo.pInstblkMemDesc,
                                pFabricVAS->pGVAS, FIFO_PDB_IDX_BASE,
                                &pInstblkParams);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                "failed instantiating instblk for FLA, status=0x%x\n",
                status);
        goto free_instblk;
    }

    //
    // For SRIOV PF/VF system, always check for P2P allocation to determine whether
    // this function is allowed to bind FLA
    //
    if (gpuIsSriovEnabled(pGpu) || IS_VIRTUAL(pGpu))
    {
        if (gpuCheckIsP2PAllocated_HAL(pGpu))
        {
            status = kbusSetupBindFla(pGpu, pKernelBus, pGpu->sriovState.pP2PInfo->gfid);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "Skipping binding FLA, because no P2P GFID is"
                      " validated yet\n");
        }
    }
    else
    {
        status = kbusSetupBindFla(pGpu, pKernelBus, GPU_GFID_PF);
    }

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed binding instblk for FLA, status=0x%x\n", status);
        goto free_instblk;
    }

    // setup Unicast FLA range in Fabric VAS object
    if (!GPU_IS_NVSWITCH_DETECTED(pGpu))
    {
        size = gpuGetFlaVasSize_HAL(pGpu, NV_FALSE);
        base = pGpu->gpuInstance * size;

        NV_ASSERT_OK_OR_GOTO(status, fabricvaspaceInitUCRange(
                                     dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE), pGpu,
                                     base, size), free_instblk);
    }

    pKernelBus->flaInfo.bFlaAllocated       = NV_TRUE;

    return NV_OK;

free_instblk:
    kbusDestructFlaInstBlk_HAL(pGpu, pKernelBus);

unpin_rootpagedir:
    if (pGpu->pFabricVAS != NULL)
    {
        vaspaceUnpinRootPageDir(pGpu->pFabricVAS, pGpu);
    }

cleanup:
    if (pGpu->pFabricVAS != NULL)
    {
        vmmDestroyVaspace(pVmm, pGpu->pFabricVAS);
        pGpu->pFabricVAS = NULL;
    }

    // TODO: remove this once legacy FLA VAS support is removed.
    pRmApi->Free(pRmApi, pKernelBus->flaInfo.hClient, pKernelBus->flaInfo.hClient);

    pKernelBus->flaInfo.bFlaAllocated = NV_FALSE;

    NV_PRINTF(LEVEL_ERROR, "failed allocating FLA VASpace status=0x%x\n",
              status);

    return status;
}

void
kbusDestroyFla_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    OBJSYS *pSys   = SYS_GET_INSTANCE();
    OBJVMM *pVmm   = SYS_GET_VMM(pSys);
    RM_API   *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (pGpu->pFabricVAS != NULL)
    {
        if (pKernelBus->flaInfo.bFlaBind)
        {
            if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
            {
                kbusSetupUnbindFla_HAL(pGpu, pKernelBus);
            }
        }

        if (pKernelBus->flaInfo.bFlaAllocated)
        {
            vaspaceUnpinRootPageDir(pGpu->pFabricVAS, pGpu);\
            kbusDestructFlaInstBlk_HAL(pGpu, pKernelBus);
            vmmDestroyVaspace(pVmm, pGpu->pFabricVAS);

            pGpu->pFabricVAS = NULL;
            // TODO: Remove this once legacy FLA  VAS support is deprecated
            pRmApi->Free(pRmApi, pKernelBus->flaInfo.hClient, pKernelBus->flaInfo.hClient);
            portMemSet(&pKernelBus->flaInfo, 0, sizeof(pKernelBus->flaInfo));
        }
    }
}

/*!
 * @brief Helper function to extract information from FLA data structure and
 *        to trigger RPC to Physical RM to BIND FLA VASpace
 *
 * @param[in]  gfid     GFID
 *
 * @return NV_OK if successful
 */
NV_STATUS
kbusSetupBindFla_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid
)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS params = {0};
    MEMORY_DESCRIPTOR  *pMemDesc;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    pMemDesc = pKernelBus->flaInfo.pInstblkMemDesc;

    switch( memdescGetAddressSpace(pMemDesc))
    {
        case ADDR_FBMEM:
            params.addrSpace = NV2080_CTRL_FLA_ADDRSPACE_FBMEM;
            break;
        case ADDR_SYSMEM:
            params.addrSpace = NV2080_CTRL_FLA_ADDRSPACE_SYSMEM;
            break;
    }
    params.imbPhysAddr = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
    params.flaAction   = NV2080_CTRL_FLA_ACTION_BIND;

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_FLA_SETUP_INSTANCE_MEM_BLOCK,
                             &params,
                             sizeof(params));

   if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "FLA bind failed, status: %x \n", status);
        return status;
    }

    // Since FLA state is tracked in the Guest, Guest RM needs to set it here
    pKernelBus->flaInfo.bFlaBind = NV_TRUE;
    pKernelBus->bFlaEnabled      = NV_TRUE;

    return status;
}

/*!
 * @brief Helper function to trigger RPC to Physical RM to unbind FLA VASpace
 *
 * @return NV_OK if successful
 */
NV_STATUS
kbusSetupUnbindFla_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS params = { 0 };
    RM_API *pRmApi = IS_GSP_CLIENT(pGpu) ? GPU_GET_PHYSICAL_RMAPI(pGpu)
                                         : rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (!pKernelBus->flaInfo.bFlaBind)
        return NV_OK;

    params.flaAction = NV2080_CTRL_FLA_ACTION_UNBIND;

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_FLA_SETUP_INSTANCE_MEM_BLOCK,
                             &params,
                             sizeof(params));

    pKernelBus->flaInfo.bFlaBind = NV_FALSE;
    pKernelBus->bFlaEnabled      = NV_FALSE;

    return status;
}

NV_STATUS
kbusGetFlaRange_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU64     *ucFlaBase,
    NvU64     *ucFlaSize,
    NvBool     bIsConntectedToNvswitch
)
{
    if (!GPU_IS_NVSWITCH_DETECTED(pGpu))
    {
        *ucFlaSize = gpuGetFlaVasSize_HAL(pGpu, NV_FALSE);
        *ucFlaBase = pGpu->gpuInstance * (*ucFlaSize);
    }
    else
    {
        FABRIC_VASPACE *pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
        NvU64           ucFlaLimit;

        if (pFabricVAS == NULL)
            return NV_ERR_INVALID_STATE;

        ucFlaLimit = fabricvaspaceGetUCFlaLimit(pFabricVAS);
        if (ucFlaLimit == 0)
            return NV_ERR_INVALID_STATE;

        *ucFlaBase = fabricvaspaceGetUCFlaStart(pFabricVAS);
        *ucFlaSize = ucFlaLimit - *ucFlaBase + 1;
    }

    return NV_OK;
}

/*!
 * @brief Returns the EGM peer ID of pRemoteGpu if it was
 *        reserved already.
 *
 * @param[in]  pLocalGpu      local OBJGPU pointer
 * @param[in]  pLocalBus      local OBJBUS pointer
 * @param[in]  pRemoteGpu     remote OBJGPU pointer
 *
 * return NV_OK on success
 *        BUS_INVALID_PEER otherwise
 *
 */
NvU32
kbusGetEgmPeerId_GH100
(
    OBJGPU    *pLocalGpu,
    KernelBus *pLocalKernelBus,
    OBJGPU    *pRemoteGpu
)
{
    NvU32 gpuPeerInst = gpuGetInstance(pRemoteGpu);
    NvU32 peerMask    = pLocalKernelBus->p2p.busNvlinkPeerNumberMask[gpuPeerInst];
    NvU32 peerId;

    if (peerMask == 0)
    {
        NV_PRINTF(LEVEL_INFO,
                  "NVLINK P2P not set up between GPU%u and GPU%u\n",
                  gpuGetInstance(pLocalGpu), gpuPeerInst);
        return BUS_INVALID_PEER;
    }

    FOR_EACH_INDEX_IN_MASK(32, peerId, peerMask)
    {
        if (pLocalKernelBus->p2p.bEgmPeer[peerId])
        {
            return peerId;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return BUS_INVALID_PEER;
}

/*!
* @brief Cache the value of NV_EP_PCFG_GPU_PF_RESIZE_BAR_CTRL_BAR_SIZE
*
* @param[in] pGpu       OBJGPU pointer
* @param[in] pKernelBus KernelBus pointer
*/
void
kbusCacheBAR1ResizeSize_WAR_BUG_3249028_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NvU32 regVal;

    if ((GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_PF_RESIZE_BAR_CAP, &regVal) != NV_OK) ||
        (regVal == 0))
    {
        NV_PRINTF(LEVEL_INFO, "Resizable Bar capability is absent\n");
        pKernelBus->setProperty(pKernelBus, PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR, NV_FALSE);
        return;
    }

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_PF_RESIZE_BAR_CTRL, &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_EP_PCFG_GPU_PF_RESIZE_BAR_CTRL\n");
        pKernelBus->setProperty(pKernelBus, PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR, NV_FALSE);
        return;
    }
    pKernelBus->bar1ResizeSizeIndex = DRF_VAL(_EP_PCFG_GPU, _PF_RESIZE_BAR_CTRL, _BAR_SIZE, regVal);
}


/*!
* @brief  Restore the value of NV_EP_PCFG_GPU_PF_RESIZE_BAR_CTRL if different from the
* cached value. Windows has a strict requirement that the PCIE config has to stay the
* same across power transitions. Early SBIOS implementing resize BAR do not restore properly
* the value of NV_EP_PCFG_GPU_PF_RESIZE_BAR_CTRL_BAR_SIZE. The reason of this WAR is to not
* crash the systems that have not beed updated - yet.
*
* @param[in] pGpu       OBJGPU pointer
* @param[in] pKernelBus KernelBus pointer
*
* @returns   NV_OK
*/
NV_STATUS
kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NvU32 regVal;
    NvU32 bar1ResizeSizeIndex;

    if (!pKernelBus->getProperty(pKernelBus, PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR))
    {
        return NV_OK;
    }

    NV_ASSERT_OR_RETURN(pKernelBus->bar1ResizeSizeIndex >= NV_EP_PCFG_GPU_PF_RESIZE_BAR_CTRL_BAR_SIZE_MIN,
        NV_ERR_INVALID_DATA);

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_PF_RESIZE_BAR_CTRL, &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_EP_PCFG_GPU_PF_RESIZE_BAR_CTRL\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    bar1ResizeSizeIndex = DRF_VAL(_EP_PCFG_GPU, _PF_RESIZE_BAR_CTRL, _BAR_SIZE, regVal);

    if (bar1ResizeSizeIndex == pKernelBus->bar1ResizeSizeIndex)
    {
        // BAR1 size match. Nothing to do
        return NV_OK;
    }

    // BAR1 size changed. Warn and update
    NV_PRINTF(LEVEL_WARNING, "BAR1 size mismatch: current: 0x%x, expected: 0x%x\n",
        bar1ResizeSizeIndex, pKernelBus->bar1ResizeSizeIndex);
    NV_PRINTF(LEVEL_WARNING, "Most likely SBIOS did not restore the BAR1 size\n");
    NV_PRINTF(LEVEL_WARNING, "Please update your SBIOS!\n");

    regVal = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _PF_RESIZE_BAR_CTRL, _BAR_SIZE, pKernelBus->bar1ResizeSizeIndex, regVal);
    GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_EP_PCFG_GPU_PF_RESIZE_BAR_CTRL, regVal);

    return NV_OK;
}

/*!
 * @brief Bind BAR2
 *
 * @param[in] pGpu        OBJGPU pointer
 * @param[in] pKernelBus  KernelBus pointer
 * @param[in] bar2Mode    BAR2 binding mode
 *
 * @returns NV_OK on success.
 */
NV_STATUS
kbusBindBar2_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    BAR2_MODE  bar2Mode
)
{
    NvU32             gfid;
    RMTIMEOUT         timeout;
    NvU32             temp;
    NvU32             valueLowAddr;
    NvU32             valueHighAddr;
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
    // and this matters for RTL sim and emulation.
    //
    bIsModePhysical = (BAR2_MODE_PHYSICAL == bar2Mode);

    if (!bIsModePhysical)
    {
        instBlkAperture = kgmmuGetHwPteApertureFromMemdesc(GPU_GET_KERNEL_GMMU(pGpu), pMemDesc);
        instBlkAddr     = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
    }

    valueLowAddr =
            ((bIsModePhysical ? DRF_DEF(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR2_BLOCK_LOW_ADDR, _TARGET, _VID_MEM) :
                                DRF_NUM(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR2_BLOCK_LOW_ADDR, _TARGET, instBlkAperture)) |
             (bIsModePhysical ? DRF_DEF(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR2_BLOCK_LOW_ADDR, _MODE, _PHYSICAL) :
                                DRF_DEF(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR2_BLOCK_LOW_ADDR, _MODE, _VIRTUAL)) |
             (bIsModePhysical ? DRF_NUM(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR2_BLOCK_LOW_ADDR, _PTR, 0x0) :
                                DRF_NUM(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR2_BLOCK_LOW_ADDR, _PTR,
                                NvU64_LO32(instBlkAddr >> NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_PTR_SHIFT))));

    valueHighAddr = NvU64_HI32(instBlkAddr);

    //
    // For BAR1 and BAR2 binds, NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR should be written
    // first followed with the HIGH. Write to the HIGH register triggers the bind.
    //
    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR, valueLowAddr);
    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_HIGH_ADDR, valueHighAddr);

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
        // NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_PENDING to return to EMPTY and
        // NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_OUTSTANDING to return to NV_FALSE.
        //
        // BAR2_PENDING indicates a Bar2 bind is waiting to be sent.
        // BAR2_OUTSTANDING indicates a Bar2 bind is outstanding to FB.
        //
        temp = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR);

        if (FLD_TEST_DRF(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR2_BLOCK_LOW_ADDR, _BAR2_PENDING, _EMPTY, temp) &&
            FLD_TEST_DRF(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR2_BLOCK_LOW_ADDR, _BAR2_OUTSTANDING, _FALSE, temp))
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

/*!
 * @brief Bind BAR1 instance block
 *
 * @param[in] pGpu        OBJGPU pointer
 * @param[in] pKernelBus  KernelBus pointer
 *
 * @returns NV_OK on success.
 */
NV_STATUS
kbusBar1InstBlkBind_GH100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    KernelGmmu       *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NvU32             gfid;
    NvU32             target;
    NvU32             temp;
    NvU32             ptrLow;
    NvU32             ptrHigh;
    RMTIMEOUT         timeout;
    NV_STATUS         status = NV_OK;
    NvU32             blockMode = 0;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    // Nothing to be done in guest in the paravirtualization case.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return NV_OK;
    }

    switch (kgmmuGetMemAperture(pKernelGmmu, pKernelBus->bar1[gfid].pInstBlkMemDesc))
    {
        case GMMU_APERTURE_VIDEO:
            target = NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_TARGET_VID_MEM;
            break;
        case GMMU_APERTURE_SYS_COH:
            target = NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_TARGET_SYS_MEM_COHERENT;
            break;
        case GMMU_APERTURE_SYS_NONCOH:
            target = NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_TARGET_SYS_MEM_NONCOHERENT;
            break;
        default:
            NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_STATE);
    }

    //
    // BAR1 address is 4K-aligned, we do not need to specify the lower 12 bit (11:0) of address.
    // Thus we are storing the upper 32-bit of the 64-bit address pointer as BAR1_BLOCK_HIGH_ADDR[31:0],
    // and the lower 20-bit of the 64-bit pointer as BAR1_BLOCK_LOW_ADDR[31:12].
    //
    ptrLow = NvU64_LO32(pKernelBus->bar1[gfid].instBlockBase >> NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_PTR_SHIFT);
    ptrHigh = NvU64_HI32(pKernelBus->bar1[gfid].instBlockBase);

    if (kbusIsBar1PhysicalModeEnabled(pKernelBus))
    {
        blockMode = DRF_DEF(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR1_BLOCK_LOW_ADDR, _MODE, _PHYSICAL);
    }
    else
    {
        blockMode = DRF_DEF(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR1_BLOCK_LOW_ADDR, _MODE, _VIRTUAL);
    }

    //
    // For BAR1 and BAR2 binds, NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR should be written
    // first followed with the HIGH. Write to the HIGH register triggers the bind.
    //
    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR,
            DRF_NUM(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR1_BLOCK_LOW_ADDR, _TARGET, target) | blockMode |
            DRF_NUM(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR1_BLOCK_LOW_ADDR, _PTR, ptrLow));

    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_HIGH_ADDR,
            DRF_NUM(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR1_BLOCK_HIGH_ADDR, _PTR, ptrHigh));

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
        // NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_PENDING to return to EMPTY and
        // NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_OUTSTANDING to return to NV_FALSE.
        //
        // BAR1_PENDING indicates a Bar1 bind is waiting to be sent.
        // BAR1_OUTSTANDING indicates a Bar1 bind is outstanding to FB.
        //
        temp = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR);

        if (FLD_TEST_DRF(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR1_BLOCK_LOW_ADDR, _BAR1_PENDING, _EMPTY, temp) &&
            FLD_TEST_DRF(_VIRTUAL_FUNCTION_PRIV_FUNC, _BAR1_BLOCK_LOW_ADDR, _BAR1_OUTSTANDING, _FALSE, temp))
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

NvU32
kbusGetEccCounts_GH100
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus
)
{
    NvU32 regVal;
    NvU32 count = 0;

    // PCIE RE ORDER
    regVal = GPU_REG_RD32(pGpu, NV_XAL_EP_REORDER_ECC_UNCORRECTED_ERR_COUNT);
    count += DRF_VAL(_XAL_EP, _REORDER_ECC, _UNCORRECTED_ERR_COUNT_UNIQUE, regVal);

    // PCIE P2PREQ
    regVal = GPU_REG_RD32(pGpu, NV_XAL_EP_P2PREQ_ECC_UNCORRECTED_ERR_COUNT);
    count += DRF_VAL(_XAL_EP, _P2PREQ_ECC, _UNCORRECTED_ERR_COUNT_UNIQUE, regVal);

    return count;
}
