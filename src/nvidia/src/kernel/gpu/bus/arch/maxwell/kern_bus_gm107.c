/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/conf_compute/conf_compute.h"
#include "gpu/gpu.h"
#include "gpu/device/device.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/bif/kernel_bif.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "mem_mgr/vaspace.h"
#include "mem_mgr/gpu_vaspace.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "core/system.h"
#include "mem_mgr/virt_mem_mgr.h"
#include "rmapi/rs_utils.h"
#include "vgpu/rpc.h"
#include "nvrm_registry.h"
#include "platform/sli/sli.h"
#include "containers/eheap_old.h"

#include "gpu/mem_mgr/fermi_dma.h"

#include "published/maxwell/gm107/dev_ram.h"
#include "published/maxwell/gm107/dev_bus.h"
#include "published/maxwell/gm107/dev_mmu.h"

#include "class/cl90f1.h"  // FERMI_VASPACE_A

#define MAXWELL_BAR2_VBIOS_OFFSET           (BUS_BAR2_RM_APERTURE_MB<<20)

//
// forwards
//
static NV_STATUS _kbusInitP2P_GM107(OBJGPU *, KernelBus *);
static NV_STATUS _kbusDestroyP2P_GM107(OBJGPU *, KernelBus *);
static void _kbusLinkP2P_GM107(OBJGPU *, KernelBus *);

static NvU32 _kbusGetSizeOfBar2PageDir_GM107(NvU64 vaBase, NvU64 vaLimit, NvU64 vaPerEntry, NvU32 entrySize);

NV_STATUS _kbusMapAperture_GM107(OBJGPU *, KernelBus *, PMEMORY_DESCRIPTOR, OBJVASPACE *, NvU64, NvU64 *,
                                 NvU64 *, NvU32 mapFlags, Device *pDevice);
NV_STATUS _kbusUnmapAperture_GM107(OBJGPU *, KernelBus *, OBJVASPACE *, PMEMORY_DESCRIPTOR, NvU64);
MEMORY_DESCRIPTOR* kbusCreateStagingMemdesc(OBJGPU *pGpu);

// This is the peer number assignment for SLI with
// 8 GPUs. The peer ID's should be symmetrical
static const NvU32 peerNumberTable_GM107[8][8] =
{
    {0, 0, 1, 2, 3, 4, 5, 6},
    {0, 0, 2, 3, 4, 5, 6, 7},
    {1, 2, 0, 4, 5, 6, 7, 0},
    {2, 3, 4, 0, 6, 7, 0, 1},
    {3, 4, 5, 6, 0, 0, 1, 2},
    {4, 5, 6, 7, 0, 0, 2, 3},
    {5, 6, 7, 0, 1, 2, 0, 4},
    {6, 7, 0, 1, 2, 3, 4, 0}
};

// Helper function to create a staging buffer memdesc with a size of one page
MEMORY_DESCRIPTOR*
kbusCreateStagingMemdesc(OBJGPU *pGpu)
{
    return NULL;
}

NV_STATUS
kbusConstructHal_GM107(OBJGPU *pGpu, KernelBus *pKernelBus)
{

    NV_PRINTF(LEVEL_INFO, "Entered \n");

    pKernelBus->p2pPcie.writeMailboxBar1Addr = PCIE_P2P_INVALID_WRITE_MAILBOX_ADDR;

    pKernelBus->bar2[GPU_GFID_PF].pdeBase   = 0xdeadbeef;
    pKernelBus->bar2[GPU_GFID_PF].pteBase   = 0xdeadbeef;

    pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleBase = 0;
    pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleLimit = 0;

    pKernelBus->virtualBar2[GPU_GFID_PF].pVASpaceHeap = NULL;
    pKernelBus->virtualBar2[GPU_GFID_PF].pMapListMemory = NULL;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))
    {
        pKernelBus->bFbFlushDisabled = NV_TRUE;
    }

    //
    // Conditions to disable CPU pointer for flushing VBAR2:
    // 1. If inst_in_sys is passed in (regkey setting)
    // 2. If FB flushing is disabled (brokenFB or regkey setting)
    // 3. If we are on GSP firmware
    //
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM) ||
        kbusIsFbFlushDisabled(pKernelBus) ||
        RMCFG_FEATURE_PLATFORM_GSP)
    {
        pKernelBus->bReadCpuPointerToFlush = NV_FALSE;
    }

    // indicate that Bar2 is not initialized yet
    pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping = NULL;

    pKernelBus->numPeers = P2P_MAX_NUM_PEERS;

    //
    // Default apertures for BAR2 PTEs/PDEsr
    //
    pKernelBus->PTEBAR2Aperture = ADDR_FBMEM;
    pKernelBus->PTEBAR2Attr = NV_MEMORY_WRITECOMBINED;
    pKernelBus->PDEBAR2Aperture = ADDR_FBMEM;
    pKernelBus->PDEBAR2Attr = NV_MEMORY_WRITECOMBINED;

    return NV_OK;
}

NV_STATUS
kbusStatePreInitLocked_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NV_PRINTF(LEVEL_INFO, "gpu:%d\n", pGpu->gpuInstance);

    if (FLD_TEST_DRF(_REG_STR_RM, _INST_VPR, _INSTBLK, _TRUE, pGpu->instVprOverrides))
    {
        pKernelBus->bInstProtectedMem = NV_TRUE;
    }

    // kbusInitBarsSize_HAL for VGPU is called in early phase
    if (!IS_VIRTUAL(pGpu))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kbusInitBarsSize_HAL(pGpu, pKernelBus));
    }

    kbusDetermineBar1Force64KBMapping(pKernelBus);

    kbusDetermineBar1ApertureLength(pKernelBus, GPU_GFID_PF);

    if (NV_OK != kbusConstructVirtualBar2_HAL(pGpu, pKernelBus, GPU_GFID_PF))
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    return NV_OK;
}

/*!
 * Stub eheap free for address reuse case below. This allows us to not if the code.
 */
static NV_STATUS nullEHeapFree(OBJEHEAP *thisHeap, NvU64 offset)
{
    return NV_OK;
}

/*!
 * @brief program the default BAR0 window based on the mode we are running at.
 */
static void
kbusSetupDefaultBar0Window
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));
    NvU64 offsetBar0;

    NV_ASSERT_OR_RETURN_VOID(!KBUS_BAR0_PRAMIN_DISABLED(pGpu));
    //
    // Initialize BAR0 window to the last 1MB of FB. Since this is where it
    // should already be positioned by the vbios, this should not be an issue.
    // Do NOT ever move this BAR0 window away from the last 1MB since it's
    // shared with the vbios
    //
    if (pMemoryManager->bReservedMemAtBottom)
    {
        offsetBar0 = memmgrGetRsvdMemoryBase(pMemoryManager);
    }
    else
    {
        offsetBar0 = (pMemoryManager->Ram.fbAddrSpaceSizeMb << 20) - DRF_SIZE(NV_PRAMIN);
    }

    //
    // However, when running in L2 cache only mode, there is typically
    // less than 1MB of L2 cache, so just position the BAR0 either at
    // the start of FB or at the end of L2 depending on how big the
    // window is compared to the size of L2.  We want to make sure that
    // the window overlaps reserved memory.
    //
    if (gpuIsCacheOnlyModeEnabled(pGpu) ||
        !(pMemorySystemConfig->bFbpaPresent))
    {
        if (pMemorySystemConfig->l2CacheSize < DRF_SIZE(NV_PRAMIN))
        {
            // L2 Cache size is < BAR0 window size so just set it offset to 0
            offsetBar0 = 0;
        }
        else
        {
            //
            // L2 Cache size is > BAR0 window, so position it at the end of L2 to
            // make sure it overlaps reserved memory, which is at the end of L2
            //
            offsetBar0 = pMemorySystemConfig->l2CacheSize - DRF_SIZE(NV_PRAMIN);
        }
    }

    if (!IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        DEVICE_MAPPING *pDeviceMapping = gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0);
        pKernelBus->pUncachedBar0Window = (NvU8*) &pDeviceMapping->gpuNvAddr->Reg008[DRF_BASE(NV_PRAMIN)];
        pKernelBus->pDefaultBar0Pointer = pKernelBus->pUncachedBar0Window;
        pKernelBus->physicalBar0WindowSize = DRF_SIZE(NV_PRAMIN);

        kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, offsetBar0);

    }
}

/*!
 * @brief  kbusStateInit routine for Kernel RM functionality.
 */
NV_STATUS
kbusStateInitLockedKernel_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    KernelBif        *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    VirtMemAllocator *pDma       = GPU_GET_DMA(pGpu);
    NvU32             data;

    if ((pKernelBif != NULL) && (!pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_READS_DISABLED) ||
                                 !pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_WRITES_DISABLED)))
    {
        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_MAP_P2P_PEER_ID, &data) == NV_OK)
        {
            pKernelBus->p2pMapSpecifyId = NV_TRUE;
            pKernelBus->p2pMapPeerId = data;
        }
        else
        {
            pKernelBus->p2pMapSpecifyId = NV_FALSE;
        }

        if (gpumgrGetGpuLinkCount(pGpu->gpuInstance) > 0)
        {
            if (!kbusIsP2pInitialized(pKernelBus) &&
                !kbusIsP2pMailboxClientAllocated(pKernelBus))
            {
                _kbusInitP2P_GM107(pGpu, pKernelBus);
            }
        }
    }

    if (!KBUS_BAR0_PRAMIN_DISABLED(pGpu))
    {
        kbusSetupDefaultBar0Window(pGpu, pKernelBus);

        if (gpuIsCCFeatureEnabled(pGpu))
        {
            // Fake sparse table creation for KERNEL RM uses BAR0 window.
            // Fake sparse entries can be used in BAR VAS setup.
            // So create fake sparse tables after BAR0 setup but before BAR2 setup.
            NV_ASSERT_OK_OR_RETURN(kgmmuCreateFakeSparseTables_HAL(pGpu, GPU_GET_KERNEL_GMMU(pGpu)));
        }
    }

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        vgpuGspTeardownBuffers(pGpu);

        if (pKernelBus->bIsBar2SetupInPhysicalMode)
        {
            kbusTeardownBar2CpuAperture_HAL(pGpu, pKernelBus, GPU_GFID_PF);
            pKernelBus->bIsBar2SetupInPhysicalMode= NV_FALSE;
        }
    }

    //
    // Initialize BAR2 before initializing BAR1.  That way, we can use BAR2
    // rather than BAR0 to set up the BAR1 page table.  This is faster because
    // BAR2 can be write-combined
    //
    if (IS_GSP_CLIENT(pGpu) && kbusIsCpuVisibleBar2Disabled(pKernelBus))
    {
        NV_PRINTF(LEVEL_INFO, "For GSP client with C2C enabled, skip BAR2 init\n");
    }
    else
    {
        NV_ASSERT_OK_OR_RETURN(kbusInitBar2_HAL(pGpu, pKernelBus, GPU_GFID_PF));
    }

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        vgpuGspSetupBuffers(pGpu);
    }

    if (IS_VIRTUAL(pGpu))
    {
        VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

        pKernelBus->bFlaSupported = pVSI->bFlaSupported;

        NV_PRINTF(LEVEL_INFO, "Enabling FLA Support in Guest RM: %x, flabase: %llx, flaSize: %llx \n",
                  pVSI->bFlaSupported, pVSI->flaInfo.base, pVSI->flaInfo.size);

        // if FLA supported enable FLA VAS
        if (kbusIsFlaSupported(pKernelBus))
        {
            NV_STATUS status = kbusCheckFlaSupportedAndInit_HAL(pGpu, pKernelBus, pVSI->flaInfo.base, pVSI->flaInfo.size);
            NV_ASSERT(status == NV_OK);
        }
    }

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
    {
        // Verify that BAR2 and the MMU actually works
        NV_ASSERT_OK_OR_RETURN(kbusVerifyBar2_HAL(pGpu, pKernelBus, NULL, NULL, 0, 0));
    }

    //
    // For "unsupported" mmu invalidate skipping mode, we align virtual BAR2
    // to avoid false TLB fills and disallow address reuse.
    //
    if (pDma->bDmaMmuInvalidateDisable)
    {
        pKernelBus->virtualBar2[GPU_GFID_PF].vAlignment = 16 * RM_PAGE_SIZE;
        pKernelBus->virtualBar2[GPU_GFID_PF].pVASpaceHeap->eheapFree = nullEHeapFree;
    }

    return NV_OK;
}

NV_STATUS
kbusStateInitLocked_IMPL(OBJGPU *pGpu, KernelBus *pKernelBus)
{
    // Nothing to be done in guest for the paravirtualization case.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return NV_OK;
    }

    //
    // Block BAR2 whenever HCC is enabled irrespective of prod and devtools mode
    // unless user has used regkey to force BAR access on devtools mode
    //
    if (IS_GSP_CLIENT(pGpu) && gpuIsCCFeatureEnabled(pGpu) &&
        !pKernelBus->bForceBarAccessOnHcc)
    {
        pKernelBus->bBarAccessBlocked = NV_TRUE;
        pKernelBus->bBar2TestSkipped = NV_TRUE;
        NV_PRINTF(LEVEL_INFO, "BARs will be blocked for CC\n");
    }
    else
    {
        pKernelBus->bBarAccessBlocked = NV_FALSE;
    }

    // Verify GSP-DMA functionality before it is used for bootstrapping BAR2
    NV_ASSERT_OK_OR_RETURN(
        memmgrVerifyGspDmaOps(pGpu, GPU_GET_MEMORY_MANAGER(pGpu)));

    if (kbusIsBar1Disabled(pKernelBus))
    {
        pKernelBus->pciBarSizes[BUS_BAR_1] = 0;
        pKernelBus->bar1[GPU_GFID_PF].physAddr = 0;
        pKernelBus->bar1[GPU_GFID_PF].apertureLength  = 0;
    }

    if (kbusIsCpuVisibleBar2Disabled(pKernelBus))
    {
        NV_PRINTF(LEVEL_INFO, "C2C is being used, so disable CPU visible BAR2 now before they are setup\n");

        pKernelBus->pciBarSizes[BUS_BAR_2] = 0;

        pKernelBus->bar2[GPU_GFID_PF].physAddr = 0;

        pKernelBus->bar2[GPU_GFID_PF].rmApertureLimit = 0;

        //
        // For C2C case, there is no CPU visible BAR2. So CPU visible BAR2
        // window is removed. In MONOLITHIC RM, where CPU visible and invisible
        // BAR2 windows are contiguous, just shift CPU invisible BAR2 down to
        // CPU invisible BAR2 base (0).
        // Only need to handle PF, since VF BAR1/2 is disabled for C2C case.
        //
        if (!RMCFG_FEATURE_PLATFORM_GSP) // contiguous BAR2
        {
            NV_ASSERT(pKernelBus->bar2[GPU_GFID_PF].cpuVisibleBase == 0);
            NV_ASSERT(pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleLimit >= pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleBase);
            if (pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleBase == (pKernelBus->bar2[GPU_GFID_PF].cpuVisibleLimit + 1))
            {
                NV_PRINTF(LEVEL_INFO, "Contiguous range, update BAR2 cpuInvisibleBase: 0x%llX to 0, and cpuInvisibleLimit: 0x%llX to 0x%llX.\n",
                      pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleBase,
                      pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleLimit,
                      pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleLimit - pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleBase);
                pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleLimit -= pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleBase;
                pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleBase = pKernelBus->bar2[GPU_GFID_PF].cpuVisibleBase;
            }
            else
            {
                NV_PRINTF(LEVEL_INFO, "Discontiguous range, retaining BAR2 cpuInvisibleBase: 0x%llX, and cpuInvisibleLimit: 0x%llX.\n",
                      pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleBase,
                      pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleLimit);
            }
        }
        NV_PRINTF(LEVEL_INFO, "Setting cpuVisibleLimit: 0x%llX to 0\n", pKernelBus->bar2[GPU_GFID_PF].cpuVisibleLimit);
        pKernelBus->bar2[GPU_GFID_PF].cpuVisibleLimit = 0;
        pKernelBus->bUsePhysicalBar2InitPagetable = NV_FALSE;
    }

    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        if (gpuIsCCFeatureEnabled(pGpu))
        {
            // Fake sparse entries can be used in BAR VAS setup.
            // So create fake sparse tables before BAR2 setup.
            NV_ASSERT_OK_OR_RETURN(kgmmuCreateFakeSparseTables_HAL(pGpu, GPU_GET_KERNEL_GMMU(pGpu)));
        }
        NV_ASSERT_OK_OR_RETURN(kbusInitBar2_HAL(pGpu, pKernelBus, GPU_GFID_PF));
    }

    NV_ASSERT_OK_OR_RETURN(kbusStateInitLockedKernel_HAL(pGpu, pKernelBus));

    NV_ASSERT_OK_OR_RETURN(kbusStateInitLockedPhysical_HAL(pGpu, pKernelBus));

    if (pKernelBus->bar1[GPU_GFID_PF].pInstBlkMemDesc != NULL)
    {
        // Initialize the BAR1 instance block to zero
        NV_ASSERT_OK_OR_RETURN(memmgrMemDescMemSet(GPU_GET_MEMORY_MANAGER(pGpu),
                               pKernelBus->bar1[GPU_GFID_PF].pInstBlkMemDesc,
                               0, TRANSFER_FLAGS_NONE));
    }

    return NV_OK;
}

/**
 * @brief  Setup BAR2 during hibernate resume
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 * @param[in] flags
 */

NV_STATUS
kbusStatePreLoad_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      flags
)
{
    if (flags & GPU_STATE_FLAGS_PRESERVING)
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

        // FB address space may not be available on Tegra (see fbInitFbRegions)
        if (pMemoryManager->Ram.fbAddrSpaceSizeMb != 0)
        {
            if (!KBUS_BAR0_PRAMIN_DISABLED(pGpu))
            {
                // Bind the BAR0 window to its default location
                // note: we can't move the window for all intents and purposes since VBIOS
                //       will also use the window at arbitrary locations (eg during an SMI event
                NvU64 offsetBar0 = (pMemoryManager->Ram.fbAddrSpaceSizeMb << 20) - DRF_SIZE(NV_PRAMIN);
                kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, offsetBar0);
            }
        }
        else
        {
            NV_ASSERT(IsTEGRA(pGpu));
        }

        if (!IsMAXWELL(pGpu))
        {
            // Bug 4351702 WAR: restore BAR2 after FBSR on Maxwell
            NV_ASSERT_OK_OR_RETURN(kbusRestoreBar2_HAL(pKernelBus, flags));
        }
    }

    return NV_OK;
}

NV_STATUS
kbusStateLoad_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      flags
)
{
    if (IS_VIRTUAL(pGpu) && (flags & GPU_STATE_FLAGS_PRESERVING))
    {
        NV_ASSERT_OK_OR_RETURN(kbusBar1InstBlkBind_HAL(pGpu, pKernelBus));
    }

    if (!IS_GPU_GC6_STATE_EXITING(pGpu))
    {
        if (!IS_VIRTUAL_WITH_FULL_SRIOV(pGpu))
        {
            if (flags & GPU_STATE_FLAGS_PM_TRANSITION)
            {
                //
                // Restore the BAR1 size if we are coming out of suspend or hibernate/hybrid sleep,
                // and the SBIOS did not restore it properly.
                // Help for customers who did not update their SBIOS.
                //
                NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                    kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_HAL(pGpu, pKernelBus));
            }
            else
            {
                // Cache the BAR1 size at driver load
                kbusCacheBAR1ResizeSize_WAR_BUG_3249028_HAL(pGpu, pKernelBus);
            }
        }
    }

    if ((flags & GPU_STATE_FLAGS_PRESERVING) && IsMAXWELL(pGpu))
    {
        // Bug 4351702 WAR: restore BAR2 after FBSR on Maxwell
        NV_ASSERT_OK_OR_RETURN(kbusRestoreBar2_HAL(pKernelBus, flags));
    }

    return NV_OK;
}

NV_STATUS
kbusRestoreBar2_GM107
(
    KernelBus *pKernelBus,
    NvU32      flags
)
{
    OBJGPU    *pGpu = ENG_GET_GPU(pKernelBus);

    NV_ASSERT_OR_RETURN(flags & GPU_STATE_FLAGS_PRESERVING, NV_ERR_INVALID_STATE);

    if (!(flags & GPU_STATE_FLAGS_GC6_TRANSITION))
    {
        if (!RMCFG_FEATURE_PLATFORM_GSP && !kbusIsCpuVisibleBar2Disabled(pKernelBus))
        {
            // Get the CPU mapping.
            NV_ASSERT_OK_OR_RETURN(kbusSetupBar2CpuAperture_HAL(pGpu, pKernelBus, GPU_GFID_PF));
        }

        if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
        {
            // Use BAR2 in physical mode to write the PDEs.
            NV_ASSERT_OK_OR_RETURN(kbusBindBar2_HAL(pGpu, pKernelBus, BAR2_MODE_PHYSICAL));
            NV_ASSERT_OK_OR_RETURN(kbusCommitBar2PDEs_HAL(pGpu, pKernelBus));
        }
        else
        {
            //
            // Only write PDEs here
            // PTEs for visible range are lost/cleared, as monolithic needs BAR2 to set up FBSR CE channel
            // PTEs for invisible range are restored
            //
            MMU_WALK            *pBar2Walk     = kbusGetBar2GmmuWalker_HAL(pKernelBus);
            const GMMU_FMT      *pBar2GmmuFmt  = kbusGetBar2GmmuFmt_HAL(pKernelBus);
            const MMU_FMT_LEVEL *pLevelFmt     = NULL;
            MMU_WALK_USER_CTX    userCtx       = {0};
            NvU64                origVidOffset = 0;

            // Check that Bar2 Page Dir starts at or after bar0 window vid offset
            NV_ASSERT_OK_OR_RETURN(kbusSetupBar0WindowBeforeBar2Bootstrap_HAL(pGpu, pKernelBus, &origVidOffset));

            // Setup walk user context.
            userCtx.pGpu = pGpu;
            NV_ASSERT_OK_OR_RETURN(mmuWalkSetUserCtx(pBar2Walk, &userCtx));

            // We want to lock the small page table
            pLevelFmt = mmuFmtFindLevelWithPageShift(pBar2GmmuFmt->pRoot,
                                                     RM_PAGE_SHIFT);

            if (pKernelBus->bar2[GPU_GFID_PF].cpuVisibleLimit != 0)
                NV_ASSERT_OK_OR_RETURN(mmuWalkCommitPDEs(pBar2Walk, pLevelFmt, pKernelBus->bar2[GPU_GFID_PF].cpuVisibleBase, pKernelBus->bar2[GPU_GFID_PF].cpuVisibleLimit));

            if (pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleLimit != 0)
                NV_ASSERT_OK_OR_RETURN(mmuWalkCommitPDEs(pBar2Walk, pLevelFmt, pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleBase, pKernelBus->bar2[GPU_GFID_PF].cpuInvisibleLimit));

            if (pKernelBus->bar2[GPU_GFID_PF].cpuVisibleLimit != 0)
            {
                NV_ASSERT_OK_OR_RETURN(mmuWalkSparsify(pBar2Walk, pKernelBus->bar2[GPU_GFID_PF].cpuVisibleBase,
                                        pKernelBus->bar2[GPU_GFID_PF].cpuVisibleLimit, NV_FALSE));
            }

            kbusRestoreBar0WindowAfterBar2Bootstrap_HAL(pGpu, pKernelBus, origVidOffset);
        }
    }

    NV_ASSERT_OK_OR_RETURN(kbusCommitBar2_HAL(pGpu, pKernelBus, flags));

    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        if (!(flags & GPU_STATE_FLAGS_GC6_TRANSITION))
        {
            RMTIMEOUT timeout;

            //
            // Temporary WAR for Bug 3737096
            // Wait for BAR0/BAR2 to settle
            //
            gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);
            do
            {
                NV_STATUS status = kbusVerifyBar2_HAL(pGpu, pKernelBus, NULL, NULL, 0, 0);

                if (status == NV_OK)
                    break;
                else if (status == NV_ERR_TIMEOUT)
                {
                    NV_PRINTF(LEVEL_ERROR, "kbusVerifyBar2_HAL() keeps failing.\n");
                    DBG_BREAKPOINT();
                    break;
                }

                status = gpuCheckTimeout(pGpu, &timeout);
                osSpinLoop();
            } while (1);
        }

        if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING) &&
            !(IS_GPU_GC6_STATE_EXITING(pGpu) && pKernelBus->bSkipBar2TestOnGc6Exit))
        {
            //
            // If we are exiting GC6 and the SKIP_BAR2_TEST_GC6 is set for the
            // chip, then don't verify BAR2. The time taken to verify causes a
            // a hit on the GC6 exit times, so this verif only feature does not
            // come for free.
            //

            // Verify that BAR2 and the MMU actually works
            NV_ASSERT_OK_OR_RETURN(kbusVerifyBar2_HAL(pGpu, pKernelBus, NULL, NULL, 0, 0));

            // Fail to set this status meaning kbusVerifyBar2_HAL() failed
        }
    }

    return NV_OK;
}

NV_STATUS
kbusStatePostLoad_GM107
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus,
    NvU32 flags
)
{
    NV_STATUS  status     = NV_OK;
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    if ( ! IS_GPU_GC6_STATE_EXITING(pGpu) &&
         !kbusIsBar1Disabled(pKernelBus))
    {
        // Bar1 is created once per Gpu on each Gpu call to kbusStatePostLoad_GM107
        if ((status = kbusInitBar1_HAL(pGpu, pKernelBus, GPU_GFID_PF)) != NV_OK)
        {
            return status;
        }
    }

    // Call _kbusLinkP2P_GM107 only in case of Linked SLI and Unlinked SLI. Bug 4182245
    if ((pKernelBif != NULL)
        &&
        // RM managed P2P or restoring the HW state for OS resume
        (!kbusIsP2pMailboxClientAllocated(pKernelBus) ||
         (flags & GPU_STATE_FLAGS_PM_TRANSITION))
        &&
        (!pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_READS_DISABLED) ||
         !pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_WRITES_DISABLED))
        &&
        (IsSLIEnabled(pGpu) || IsUnlinkedSLIEnabled(pGpu)))
    {
        _kbusLinkP2P_GM107(pGpu, pKernelBus);
    }

    // Cache BAR1 SPA for GPUDirect RDMA use-cases
    if (pKernelBus->bGrdmaForceSpa)
    {
        NV_ASSERT_OK_OR_RETURN(kbusGetPFBar1Spa_HAL(pGpu, pKernelBus,
                                                    &pKernelBus->grdmaBar1Spa));
    }

    kbusUpdateRusdStatistics(pGpu);

    return status;
}

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
kbusTeardownMailbox_GM107
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus
)
{
    NvU32 bar1Block;

    // set bar1 mode to physical and vidmem so we don't accidentally corrupt sysmem
    bar1Block = GPU_REG_RD32(pGpu, NV_PBUS_BAR1_BLOCK);
    bar1Block = FLD_SET_DRF(_PBUS, _BAR1_BLOCK, _MODE, _PHYSICAL, bar1Block);
    bar1Block = FLD_SET_DRF(_PBUS, _BAR1_BLOCK, _TARGET, _VID_MEM, bar1Block);

    //
    // override the aperture to sysmem if FB is not present.
    //
    if ((pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB)) && RMCFG_FEATURE_MODS_FEATURES)
        bar1Block = FLD_SET_DRF(_PBUS, _BAR1_BLOCK, _TARGET, _SYS_MEM_NONCOHERENT, bar1Block);

    GPU_REG_WR32(pGpu, NV_PBUS_BAR1_BLOCK, bar1Block);
}

NV_STATUS
kbusStatePreUnload_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      flags
)
{
    if (!((flags & GPU_STATE_FLAGS_PRESERVING) || IS_GPU_GC6_STATE_ENTERING(pGpu)))
    {
        //
        // Get rid of the bars if this is not PM. There were/are issues with user-mode
        // OGL on XP not knowing that the system has enter suspend and so continuing to
        // run (and issue APIs, touch bar1 resources, whatever). Therefore we cannot
        // teardown bar1 path when entering suspend.
        //
        kbusDestroyBar1_HAL(pGpu, pKernelBus, GPU_GFID_PF);
        kbusTeardownMailbox_HAL(pGpu, pKernelBus);
    }

    return NV_OK;
}

NV_STATUS
kbusStatePostUnload_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      flags
)
{
    if ((flags & GPU_STATE_FLAGS_PRESERVING) && !(flags & GPU_STATE_FLAGS_GC6_TRANSITION))
    {
        NV_ASSERT_OK(kbusTeardownBar2CpuAperture_HAL(pGpu, pKernelBus, GPU_GFID_PF));
    }

    return NV_OK;
}

NV_STATUS
kbusStateUnload_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      flags
)
{
    NV_STATUS          status     = NV_OK;
    KernelBif         *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    if (IS_VIRTUAL(pGpu) && !(flags & GPU_STATE_FLAGS_PRESERVING))
        return NV_OK;

    // Call kbusUnlinkP2P_HAL only in case of Linked SLI and Unliked SLI. Bug 4182245
    if ((pKernelBif != NULL)
        &&
        (!pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_READS_DISABLED) ||
         !pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_WRITES_DISABLED))
        &&
        // RM managed P2P or unconfiguring HW P2P for OS suspend/hibernate
        (!kbusIsP2pMailboxClientAllocated(pKernelBus) ||
         (flags & GPU_STATE_FLAGS_PM_TRANSITION))
        && (IsSLIEnabled(pGpu) || IsUnlinkedSLIEnabled(pGpu)))
    {
        kbusUnlinkP2P_HAL(pGpu, pKernelBus);
    }

    if (!(flags & GPU_STATE_FLAGS_PRESERVING))
    {
        // Clear write mailbox data window info.
        pKernelBus->p2pPcie.writeMailboxBar1Addr  = PCIE_P2P_INVALID_WRITE_MAILBOX_ADDR;
        pKernelBus->p2pPcie.writeMailboxTotalSize = 0;
    }

    pKernelBus->cachedBar0WindowVidOffset = 0x0;

    return status;
}

/*!
 * @brief Whether we need to init the mailbox BAR1 mappings
 */
static NvBool
_kbusRequiresP2PMailboxBar1_GM107
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus,
    NvU32        gfid
)
{
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    return (pKernelBif->pcieP2PType != NV_REG_STR_RM_PCIEP2P_TYPE_BAR1)
           &&
           (!pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_READS_DISABLED) ||
            !pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_WRITES_DISABLED))
           &&
           IS_GFID_PF(gfid)
           &&
           !kbusIsP2pMailboxClientAllocated(pKernelBus);
}

/*!
 * @brief Init BAR1.
 *
 *  - Inits FERMI BUS HALINFO Bar1 structure
 *  - Sets up BAR1 address space
 *  - The function is skipped during GC6 cycle.  It can update page table in
 *    VIDMEM/SYSMEM but all register access should be avoid in the function
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns NV_OK on success, or rm_status from called functions on failure.
 */
NV_STATUS
kbusInitBar1_GM107(OBJGPU *pGpu, KernelBus *pKernelBus, NvU32 gfid)
{
    OBJEHEAP         *pVASpaceHeap              = NULL;
    NV_STATUS         rmStatus                  = NV_OK;
    NvU64             apertureVirtAddr, apertureVirtLength;
    NvU64             vaRangeMax;
    NvU32             vaflags;
    NvU64             vaSpaceBigPageSize        = 0;
    OBJSYS           *pSys                      = SYS_GET_INSTANCE();
    OBJVMM           *pVmm                      = SYS_GET_VMM(pSys);
    NvU32             gpuMask                   = 0;
    NvBool            bSmoothTransitionEnabled  = ((pGpu->uefiScanoutSurfaceSizeInMB != 0) &&
                                                   RMCFG_FEATURE_PLATFORM_WINDOWS);
    NvU64             consoleSize     = 0;
    NvBool            bStaticBar1Supported;

    vaRangeMax = pKernelBus->bar1[gfid].apertureLength - 1;

    //
    // In sleep-resume, BAR1 is not destroyed - but we would have to rebind the BAR1.
    // Since that's done already. Exit early from here.
    //
    if (pKernelBus->bar1[gfid].pVAS != NULL)
    {
        return rmStatus;
    }

    if (IsT234(pGpu) && pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))
        return NV_OK;

    //
    // For BAR address spaces, leave a valid PTE pointed to page 0.
    // According to page 196 of revision 2.1 of the PCI spec, prefetchable
    // memory must have no side effects on reads, return all bytes on reads
    // regardless of byte enables, and host bridges can merge processor
    // writes without errors.
    //
    // Setting this is done by a combination of two steps. Sparsify the VAS
    // to prevent faults during CPU access and set FULL_PTE.
    //
    // For front door simulation and mods emulation however this leads to an
    // excessive amount of time updating BAR1 PTEs.  So for mods in simulation
    // and emulation we don't set the FULL_PTE flag.  The VMA code will only
    // validate the used parts of the PDE in this case, but will make sure to
    // leave one unused scratch page at the end of the valid range.
    //
    vaflags = VASPACE_FLAGS_BAR | VASPACE_FLAGS_BAR_BAR1;
    vaflags |= VASPACE_FLAGS_ALLOW_ZERO_ADDRESS; // BAR1 requires a zero VAS base.
    vaflags |= VASPACE_FLAGS_ENABLE_VMM;

#if defined(DEVELOP) || defined(DEBUG) || RMCFG_FEATURE_MODS_FEATURES
    {
        NvU32 data32 = 0;
        //
        // The BAR1 page size can be only configured for mods verification.
        // for mods only we will override the default bar1 big page size if this regkey is set.
        // This is the mods plan for testing interop between clients with multiple
        // big page sizes.
        //
        if (osReadRegistryDword(pGpu,
                    NV_REG_STR_RM_SET_BAR1_ADDRESS_SPACE_BIG_PAGE_SIZE, &data32) == NV_OK)
        {
            KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
            if (kgmmuIsPerVaspaceBigPageEn(pKernelGmmu))
            {
                vaSpaceBigPageSize = data32;
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR,
                            "Arch doesnt support BAR1 Big page Override- Using defaults\n");
                NV_ASSERT(0);
                vaSpaceBigPageSize = 0;
            }
        }
    }
#endif // defined(DEVELOP) || defined(DEBUG) || RMCFG_FEATURE_MODS_FEATURES

    switch (vaSpaceBigPageSize)
    {
        case FERMI_BIG_PAGESIZE_64K:
            vaflags |= DRF_DEF(_VASPACE, _FLAGS, _BIG_PAGE_SIZE, _64K);
            break;
        case FERMI_BIG_PAGESIZE_128K:
            vaflags |= DRF_DEF(_VASPACE, _FLAGS, _BIG_PAGE_SIZE, _128K);
            break;
        default:
            vaflags |= DRF_DEF(_VASPACE, _FLAGS, _BIG_PAGE_SIZE, _DEFAULT);
            break;
    }

    //
    // kbusIsP2pMailboxClientAllocated:
    //     The client allocates the mailbox area
    //     It is not safe to disable smooth transition from RM as it assumed to be enabled in KMD
    //
    if (kbusIsP2pMailboxClientAllocated(pKernelBus))
    {
        // KMD requires smooth transition to have a reverse BAR1 VA space
        if (bSmoothTransitionEnabled)
            vaflags |= VASPACE_FLAGS_REVERSE;
    }
    else
    {
        //
        // Smooth transition is enabled
        //     Bug# 3208922: For BAR1 range > 4gig on notebooks.
        //     For BAR1 range less than 4gig, otherwise
        //
        if (bSmoothTransitionEnabled && (IsMobile(pGpu) || (vaRangeMax < NV_U32_MAX)))
        {
            //
            // If UEFI scanoutsurface size is configured to be non-zero,
            // we are going to move all BAR1 vaspace requests to not
            // conflict the UEFI scanout surface at offset 0 to the higher
            // address range.
            //
            // P2P mailbox registers are 34 bit wide and hence can only address
            // first 16 GiG of BAR1 due to the limited address width. Hence,
            // they cannot be moved to the top of the BAR1 always.
            //
            // We are restricting this feature only to those SKUs which
            // has BAR1 aperture within 4gig range, because this feature is
            // notebook only, and the expectation is the BAR1 va range will
            // not be that huge. Once BAR1 va range crosses 4gig (eventhough smaller?
            // than 16 gig), we may have to revisit p2p mailbox and expand it to
            // full fb range - as there will be new features such as dynamic BAR1.
            //
            // Choosing the smallest 4gig range for now.
            //
            vaflags |= VASPACE_FLAGS_REVERSE;
        }
        else
        {
            bSmoothTransitionEnabled = NV_FALSE;
            pGpu->uefiScanoutSurfaceSizeInMB = 0;
        }
    }

    if (IS_GFID_VF(gfid))
    {
        vaflags |= VASPACE_FLAGS_ALLOW_PAGES_IN_PHYS_MEM_SUBALLOCATOR;
    }

    gpuMask = NVBIT(pGpu->gpuInstance);

    rmStatus = vmmCreateVaspace(pVmm, FERMI_VASPACE_A, 0, gpuMask,
                                0, vaRangeMax, 0, 0, NULL,
                                vaflags, &pKernelBus->bar1[gfid].pVAS);
    if (NV_OK != rmStatus)
    {
        NV_PRINTF(LEVEL_ERROR,
                    "Could not construct BAR1 VA space object.\n");
        pKernelBus->bar1[gfid].pVAS = NULL;
        DBG_BREAKPOINT();
        return rmStatus;
    }


    // Restrict normal BAR1 alloc to be within the aperture
    pVASpaceHeap = vaspaceGetHeap(pKernelBus->bar1[gfid].pVAS);

    //
    // Reduce BAR1 VA space by FERMI_SMALL_PAGESIZE for host overfetch bug
    // WAR (Bug 529932/525381). (FERMI_SMALL_PAGESIZE is sufficient to
    // avoid big pagesize allocations at the end of BAR1 VA space.)
    //
    vaRangeMax -= FERMI_SMALL_PAGESIZE;
    rmStatus = pVASpaceHeap->eheapSetAllocRange(pVASpaceHeap,
                                                0, vaRangeMax);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                    "Unable to set BAR1 alloc range to aperture size!\n");
        goto kbusInitBar1_failed;
    }

    // Stash the mappable length after applying WARs
    pKernelBus->bar1[gfid].mappableLength = vaRangeMax;

    //
    // Make sure the aperture length we are using not larger than the maximum length available.
    // Usually, bar1.apertureLength should be equal to the return value of kbusGetPciBarSize, however,
    // in L2 cache only mode, the aperture length being used may have been overridden to a smaller size,
    // so take that into account in the assert.
    //
    NV_ASSERT(pKernelBus->bar1[gfid].apertureLength <= kbusGetPciBarSize(pKernelBus, 1));

    //
    // If we need to preserve a console mapping at the start of BAR1, we
    // need to allocate the VA space before anything else gets allocated.
    //
    // This must come before enabling the static BAR1 mapping for the same reason
    // The consoleSize is also used for below kbusIsStaticBar1Supported_HAL check.
    //
    if (IS_GFID_PF(gfid) &&
        (kbusIsPreserveBar1ConsoleEnabled(pKernelBus) || bSmoothTransitionEnabled))
    {
        MemoryManager     *pMemoryManager  = GPU_GET_MEMORY_MANAGER(pGpu);
        NvU64              bar1VAOffset    = 0;
        NvU64              fbPhysOffset    = 0;
        PMEMORY_DESCRIPTOR pConsoleMemDesc = NULL;
        MEMORY_DESCRIPTOR  memdesc;

        if (bSmoothTransitionEnabled)
        {
            //
            // Smooth transition - The physical fb offset 0 to uefiScanoutSurfaceSize(InMB) should be identity mapped.
            // The lower FB region at offset 0 is owned by PMA and OS in wddm and hence RM will not reserve the physical
            // FB memory but only describe it.
            //
            pConsoleMemDesc = &memdesc;
            memdescCreateExisting(pConsoleMemDesc, pGpu, pGpu->uefiScanoutSurfaceSizeInMB * 1024 * 1024, ADDR_FBMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE);
            memdescDescribe(pConsoleMemDesc, ADDR_FBMEM, 0, pGpu->uefiScanoutSurfaceSizeInMB * 1024 * 1024);
            pConsoleMemDesc->_pageSize = RM_PAGE_SIZE;
        }
        else if (kbusIsPreserveBar1ConsoleEnabled(pKernelBus))
        {
            pConsoleMemDesc = memmgrGetReservedConsoleMemDesc(pGpu, pMemoryManager);
        }

        if (pConsoleMemDesc)
        {
            consoleSize = memdescGetSize(pConsoleMemDesc);

            NV_PRINTF(LEVEL_INFO,
                        "preserving console BAR1 mapping (0x%llx)\n",
                        consoleSize);

            rmStatus = kbusMapFbApertureSingle(pGpu, pKernelBus, pConsoleMemDesc, fbPhysOffset,
                                               &bar1VAOffset, &consoleSize,
                                               BUS_MAP_FB_FLAGS_MAP_UNICAST | BUS_MAP_FB_FLAGS_MAP_OFFSET_FIXED,
                                               NULL);
            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                            "cannot preserve console mapping in BAR1 (0x%llx, 0x%x)\n",
                            consoleSize, rmStatus);
                goto kbusInitBar1_failed;
            }

            //
            // The reserved console is assumed by the console-driving code to
            // be at offset 0 of BAR1; anything else will break it.
            // NOTE: Since BUS_MAP_FB_FLAGS_MAP_OFFSET_FIXED is passed we should never
            // get here. But this is legacy code leaving it here.
            //
            if (bar1VAOffset != 0)
            {
                NV_PRINTF(LEVEL_ERROR,
                            "expected console @ BAR1 offset 0 (0x%llx, 0x%x)\n",
                            bar1VAOffset, rmStatus);
                DBG_BREAKPOINT();
                kbusUnmapFbApertureSingle(pGpu, pKernelBus, pConsoleMemDesc,
                                          bar1VAOffset, consoleSize,
                                          BUS_MAP_FB_FLAGS_MAP_UNICAST | BUS_MAP_FB_FLAGS_PRE_INIT);
                goto kbusInitBar1_failed;
            }

            pKernelBus->bBar1ConsolePreserved = NV_TRUE;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                        "no console memdesc available to preserve\n");
            DBG_BREAKPOINT();
            goto kbusInitBar1_failed;
        }
    }

    //
    // Reserve space for max number of peers for the mailbox p2p  regardless of SLI config
    //
    // This must come before enabling the static BAR1 mapping to reserve the mailbox at
    // low addresses because mailbox registers only support a limited address space,
    // e.g. pre-Hopper only supports sub-8GB memory range - bug 4750016 comment 7.
    // And it sets up pKernelBus->p2pPcie.writeMailboxTotalSize used for below
    // kbusIsStaticBar1Supported_HAL check.
    //
    if (_kbusRequiresP2PMailboxBar1_GM107(pGpu, pKernelBus, gfid))
    {
        rmStatus = kbusAllocP2PMailboxBar1_HAL(pGpu, pKernelBus, gfid, vaRangeMax);

        if (NV_OK != rmStatus)
        {
            goto kbusInitBar1_failed;
        }
    }

    rmStatus = kbusIsStaticBar1Supported_HAL(pGpu, pKernelBus, gfid,
                   consoleSize, pKernelBus->p2pPcie.writeMailboxTotalSize);

    if (rmStatus == NV_OK)
    {
        bStaticBar1Supported = NV_TRUE;
    }
    else if (rmStatus == NV_ERR_NOT_SUPPORTED)
    {
        bStaticBar1Supported = NV_FALSE;
    }
    else
    {
        goto kbusInitBar1_failed;
    }

    rmStatus = NV_OK;

    if (bStaticBar1Supported)
    {
        // Enable the static BAR1 mapping for the BAR1 P2P
        NV_ASSERT_OK_OR_GOTO(rmStatus,
                             kbusEnableStaticBar1Mapping_HAL(pGpu, pKernelBus, gfid),
                             kbusInitBar1_failed);
    }

    //
    // BAR1 vaspace is sparsified during vaspace creation
    // and hence pdb is allocated during vaspace and destroyed
    // when BAR1 is destroyed. During power-save restore cycle
    // BAR1 is not destroyed, but only the instance memory is
    // unbound and put in physical mode and rebound upon restore.
    // Hence pdb of BAR1 is static and can be updated here during
    // initialization instead of previously in mmu update pdb.
    //
    rmStatus = kbusBar1InstBlkVasUpdate_HAL(pGpu, pKernelBus);

    if (NV_OK != rmStatus)
    {
        goto kbusInitBar1_failed;
    }

    kbusPatchBar1Pdb_HAL(pGpu, pKernelBus);

    apertureVirtAddr   = pKernelBus->p2pPcie.writeMailboxBar1Addr;
    apertureVirtLength = pKernelBus->p2pPcie.writeMailboxTotalSize;

    //
    // Copy the mailbox setup to other GPUs
    //
    // This SLI_LOOP is only necessary because _kbusLinkP2P_GM107 is called
    // after each call to kbusInitBar1_GM107 in the function busStatePostLoad_GM107.
    // _kbusLinkP2P_GM107 requires that the writeMailboxAddr of every GPU be set, but
    // that can only happen after kbusInitbar1_GM107 is called on every GPU. In the
    // future, if we can separate the function that kbusInitBar1_GM107 is called in
    // and the function that _kbusLinkP2P_GM107 is called in. Then, all of the
    // kbusInitBar1_GM107 calls can finish and create writeMailboxes, and we can
    // remove this SLI_LOOP.
    //
    if (gpumgrIsParentGPU(pGpu) &&
        !kbusIsP2pMailboxClientAllocated(pKernelBus))
    {
        SLI_LOOP_START(SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
        {
            pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
            pKernelBus->p2pPcie.writeMailboxBar1Addr  = apertureVirtAddr;
            pKernelBus->p2pPcie.writeMailboxTotalSize = apertureVirtLength;
        }
        SLI_LOOP_END
        pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    }

kbusInitBar1_failed:
    if (rmStatus != NV_OK)
    {
        kbusDestroyBar1_HAL(pGpu, pKernelBus, gfid);
    }

    if (!bSmoothTransitionEnabled || (rmStatus != NV_OK))
    {
        pGpu->uefiScanoutSurfaceSizeInMB = 0;
    }

    return rmStatus;
}

void
kbusUnmapPreservedConsole_GM107
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus,
    NvU32        gfid
)
{
    if (pKernelBus->bBar1ConsolePreserved && IS_GFID_PF(gfid))
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
        PMEMORY_DESCRIPTOR pConsoleMemDesc =
            memmgrGetReservedConsoleMemDesc(pGpu, pMemoryManager);

        if (pConsoleMemDesc != NULL)
        {
            NvU64 consoleSize = memdescGetSize(pConsoleMemDesc);

            kbusUnmapFbApertureSingle(pGpu, pKernelBus, pConsoleMemDesc,
                                      0, consoleSize, BUS_MAP_FB_FLAGS_MAP_UNICAST | BUS_MAP_FB_FLAGS_PRE_INIT);
        }
        else if (pGpu->uefiScanoutSurfaceSizeInMB)
        {
            vaspaceFree(pKernelBus->bar1[gfid].pVAS, 0);
        }
        else
        {
            NV_ASSERT(pConsoleMemDesc != NULL);
        }

        pKernelBus->bBar1ConsolePreserved = NV_FALSE;
    }
}

/*!
 * @brief Destroy BAR1
 *
 * Destroys Bar1 VA Space. BAR1 vaspace is not destroyed during
 * Power save.
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns NV_OK always.
 */
NV_STATUS
kbusDestroyBar1_GM107
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus,
    NvU32        gfid
)
{
    NV_STATUS   status = NV_OK;
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJVMM     *pVmm = SYS_GET_VMM(pSys);

    if (pKernelBus->bar1[gfid].pVAS != NULL)
    {

        // Remove the P2P write mailbox alloc, if it exists
        if ((pKernelBus->p2pPcie.writeMailboxBar1Addr != PCIE_P2P_INVALID_WRITE_MAILBOX_ADDR) &&
            IS_GFID_PF(gfid))
        {
            if (!kbusIsP2pMailboxClientAllocated(pKernelBus))
                vaspaceFree(pKernelBus->bar1[gfid].pVAS, pKernelBus->p2pPcie.writeMailboxBar1Addr);
            pKernelBus->p2pPcie.writeMailboxBar1Addr  = PCIE_P2P_INVALID_WRITE_MAILBOX_ADDR;
            pKernelBus->p2pPcie.writeMailboxTotalSize = 0;
        }

        // Remove the preserved BAR1 console mapping, if it exists
        kbusUnmapPreservedConsole(pGpu, pKernelBus, gfid);

        if (pKernelBus->bar1[gfid].bStaticBar1Enabled)
        {
            kbusDisableStaticBar1Mapping_HAL(pGpu, pKernelBus, gfid);
        }

        vmmDestroyVaspace(pVmm, pKernelBus->bar1[gfid].pVAS);

        pKernelBus->bar1[gfid].pVAS = NULL;
    }

    if (IS_GFID_VF(gfid) && (pKernelBus->bar1[gfid].pInstBlkMemDesc != NULL))
    {
        memdescFree(pKernelBus->bar1[gfid].pInstBlkMemDesc);
        memdescDestroy(pKernelBus->bar1[gfid].pInstBlkMemDesc);
        pKernelBus->bar1[gfid].pInstBlkMemDesc = NULL;
    }

    return status;
}

/*!
 * @brief Initialize BAR2
 *
 * 1. Setup Bar2 VA Space.
 * 2. Setup Bar2 in HW.
 * 3. Host over fetch WAR.
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 * @param[in] gfid          GFID for VF
 *
 * @returns NV_OK on success.
 */
NV_STATUS
kbusInitBar2_GM107
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus,
    NvU32        gfid
)
{
    NV_STATUS  status     = NV_OK;

    //
    // Nothing to be done in guest in the paravirtualization case or
    // if guest is running in SRIOV heavy mode.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_OK;
    }

    if (!kbusIsCpuVisibleBar2Disabled(pKernelBus))
    {
        status = kbusSetupBar2CpuAperture_HAL(pGpu, pKernelBus, gfid);
        NV_ASSERT_OR_GOTO(status == NV_OK,  cleanup);
    }

    status = kbusSetupBar2GpuVaSpace_HAL(pGpu, pKernelBus, gfid);
    NV_ASSERT_OR_GOTO(status == NV_OK,  cleanup);

    if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
    {
        status = kbusSetupBar2PageTablesAtTopOfFb_HAL(pGpu, pKernelBus, gfid);
        NV_ASSERT_OR_GOTO(status == NV_OK, cleanup);
    }

    status = kbusCommitBar2_HAL(pGpu, pKernelBus, GPU_STATE_DEFAULT);
    NV_ASSERT_OR_GOTO(status == NV_OK,  cleanup);

    if (IS_GFID_PF(gfid))
    {
        pKernelBus->bIsBar2Initialized = NV_TRUE;
    }

cleanup:
    if (status != NV_OK)
    {
        kbusDestroyBar2_HAL(pGpu, pKernelBus, gfid);
    }

    return status;
}

/*!
 * @brief Destroy BAR2
 *
 * 1. Tear down BAR2 Cpu Aperture.
 * 2. Destroy Bar2 Gpu VA Space.
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns NV_OK on success.
 */
NV_STATUS
kbusDestroyBar2_GM107(OBJGPU *pGpu, KernelBus *pKernelBus, NvU32 gfid)
{
    NV_STATUS status = NV_OK;

    // Tear down buffers to avoid leaking BAR2 mappings
    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        vgpuGspTeardownBuffers(pGpu);
    }

    //
    // Nothing to be done in guest in the paravirtualization case or
    // if guest is running in SRIOV heavy mode.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) || IS_VIRTUAL_WITH_HEAVY_SRIOV(pGpu))
    {
        return NV_OK;
    }

    if (kbusTeardownBar2CpuAperture_HAL(pGpu, pKernelBus, gfid) != NV_OK)
    {
        status = NV_ERR_GENERIC;
    }

    if (kbusTeardownBar2GpuVaSpace_HAL(pGpu, pKernelBus, gfid) != NV_OK)
    {
        status = NV_ERR_GENERIC;
    }

    if (IS_GFID_PF(gfid))
    {
        pKernelBus->bIsBar2Initialized = NV_FALSE;
    }

    //
    // In cache only mode, do a video memory flush after unbinding BARS to
    // make sure that during capture, we don't get stuck waiting on L2.
    // This could probably just be done all the time, but currently limiting
    // to cache only mode.
    //
    if (gpuIsCacheOnlyModeEnabled(pGpu) &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB))
    {
        //
        // Send 2 explicit sysmembars. Bug 557124
        //
        kbusSendSysmembar(pGpu, pKernelBus);
        kbusSendSysmembar(pGpu, pKernelBus);
    }

    //
    // Bring up BAR2 and RPC buffers again to allow RPC.
    // On VGPU-GSP guest we have to retain the BAR2 mapping for RPC to GSP until the driver is
    // unloaded. Therefore unmapping BAR2 is deferred until we send UNLOADING_GUEST_DRIVER RPC
    // event.
    //
    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
        {
            kbusBar2BootStrapInPhysicalMode_HAL(pGpu, pKernelBus);
            NV_ASSERT(IS_GFID_PF(gfid));
        }

        vgpuGspSetupBuffers(pGpu);
    }

    return status;
}

/*!
 * @brief Setup BAR2 aperture for CPU access
 *
 * 1. Acquire BAR2 CPU mapping.
 * 2. Initialize BAR2 GPU vaspace.
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns NV_OK on success.
 */
NV_STATUS
kbusSetupBar2CpuAperture_GM107(OBJGPU *pGpu, KernelBus *pKernelBus, NvU32 gfid)
{
    NV_STATUS         status  = NV_OK;

    // Nothing to be done in guest in the paravirtualization case.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) || IS_GFID_VF(gfid) || IS_VIRTUAL_WITH_HEAVY_SRIOV(pGpu))
    {
        return NV_OK;
    }

    if (pKernelBus->virtualBar2[gfid].pCpuMapping != NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "BAR2 already initialized!\n");
        return NV_ERR_GENERIC;
    }

    if (0 == pKernelBus->bar2[gfid].pteBase)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "BAR2 pteBase not initialized by fbPreInit_FERMI!\n");
        DBG_BREAKPOINT();
        return NV_ERR_GENERIC;
    }

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) &&
        !gpuIsCacheOnlyModeEnabled(pGpu))
    {
        pKernelBus->virtualBar2[gfid].pCpuMapping = NULL;
        return NV_OK;
    }

    NV_ASSERT_OR_RETURN(pKernelBus->bar2[gfid].physAddr != 0, NV_ERR_INVALID_STATE);

    //
    // Map bar2 space -- only map the space we use in the RM.  Some 32b OSes are *cramped*
    // for kernel virtual addresses.
    //
    if (NV_OK != osMapPciMemoryKernelOld(pGpu, pKernelBus->bar2[gfid].physAddr,
                                            (pKernelBus->bar2[gfid].rmApertureLimit + 1),
                                            NV_PROTECT_READ_WRITE,
                                            (void**)&(pKernelBus->virtualBar2[gfid].pCpuMapping),
                                            NV_MEMORY_WRITECOMBINED))
    {
        NV_PRINTF(LEVEL_ERROR, "- Unable to map bar2!\n");
        DBG_BREAKPOINT();
        return NV_ERR_GENERIC;
    }

    NV_PRINTF_COND(IS_EMULATION(pGpu), LEVEL_NOTICE, LEVEL_INFO,
                    "BAR0 Base Cpu Mapping @ 0x%p and BAR2 Base Cpu Mapping @ 0x%p\n",
                    pGpu->deviceMappings[0].gpuNvAddr->Reg032,
                    pKernelBus->virtualBar2[gfid].pCpuMapping);

    if (status != NV_OK)
    {
        kbusTeardownBar2CpuAperture_HAL(pGpu, pKernelBus, gfid);
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
kbusTeardownBar2CpuAperture_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid
)
{
    // Nothing to be done in guest in the paravirtualization case.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) || IS_VIRTUAL_WITH_HEAVY_SRIOV(pGpu) || IS_GFID_VF(gfid))
    {
        return NV_OK;
    }

    if (pKernelBus->virtualBar2[gfid].pPageLevels)
    {
        memmgrMemDescEndTransfer(GPU_GET_MEMORY_MANAGER(pGpu),
                        pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc,
                        TRANSFER_FLAGS_NONE);
        pKernelBus->virtualBar2[gfid].pPageLevels = NULL;
    }

    kbusDestroyCpuPointerForBusFlush_HAL(pGpu, pKernelBus);

    kbusFlushVirtualBar2_HAL(pGpu, pKernelBus, NV_FALSE, gfid);

    if (pKernelBus->virtualBar2[gfid].pCpuMapping)
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
        GPU_FLD_WR_DRF_DEF(pGpu, _PBUS, _BAR2_BLOCK, _MODE, _PHYSICAL);
        // bug 1738008: temporary fix to unblock -inst_in_sys argument
        // we tried to correct bar2 unbind sequence but didn't fix the real issue
        // will fix this soon 4/8/16
        GPU_REG_RD32(pGpu, NV_PBUS_BAR2_BLOCK);
    }

    return NV_OK;
}

/*!
 * @brief Setup BAR2 GPU vaspace
 *
 * 1. Allocate & initialize BAR2 GPU vaspace page directories & tables.
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns NV_OK on success.
 */
NV_STATUS
kbusSetupBar2GpuVaSpace_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid
)
{
    NV_STATUS               status             = NV_OK;
    MemoryManager          *pMemoryManager     = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelGmmu             *pKernelGmmu        = GPU_GET_KERNEL_GMMU(pGpu);
    MMU_WALK               *pWalk              = NULL;
    MMU_WALK_FLAGS          walkFlags          = {0};
    MMU_WALK_USER_CTX       userCtx            = {0};
    const MMU_FMT_LEVEL    *pLevelFmt          = NULL;
    NvU64                   origVidOffset      = 0;
    OBJEHEAP               *pVASpaceHeap;
    MEMORY_DESCRIPTOR      *pPageLevelsMemDesc = NULL;
    NvU32                   allocSize;
    NvBool                  bUseStagingBuffer = NV_FALSE;

    //
    // Nothing to be done in guest in the paravirtualization case or if
    // if guest is running in SRIOV heavy mode.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_OK;
    }

    if (IS_GFID_VF(gfid))
    {
        //
        // VF BAR2 instance block cannot by in PF sysmem as the latter
        // is not mapped into VF's IOMMU domain
        //
        NV_ASSERT_OR_RETURN(pKernelBus->InstBlkAperture == ADDR_FBMEM, NV_ERR_INVALID_ARGUMENT);

        if ((status = memdescCreate(&pKernelBus->bar2[gfid].pInstBlkMemDesc,
                                    pGpu,
                                    GF100_BUS_INSTANCEBLOCK_SIZE,
                                    GF100_BUS_INSTANCEBLOCK_SIZE,
                                    NV_TRUE,
                                    pKernelBus->InstBlkAperture,
                                    pKernelBus->InstBlkAttr,
                                    MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE)) != NV_OK)
        {
            NV_ASSERT_OR_RETURN(status == NV_OK, status);
        }

        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_63,
                        pKernelBus->bar2[gfid].pInstBlkMemDesc);
        NV_ASSERT_OR_RETURN(status == NV_OK, status);

        pKernelBus->bar2[gfid].instBlockBase =
                     memdescGetPhysAddr(pKernelBus->bar2[gfid].pInstBlkMemDesc,
                                    AT_GPU, 0);
    }
    // Add the reserved memory base, converting from relative to absolute addresses.
    else
    {
        if (ADDR_FBMEM == pKernelBus->PDEBAR2Aperture)
            pKernelBus->bar2[gfid].pdeBase += memmgrGetRsvdMemoryBase(pMemoryManager);
        if (ADDR_FBMEM == pKernelBus->PTEBAR2Aperture)
            pKernelBus->bar2[gfid].pteBase += memmgrGetRsvdMemoryBase(pMemoryManager);
    }

    if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus) &&
        ((pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB)) ||
        ((ADDR_FBMEM == pKernelBus->PDEBAR2Aperture) &&
        (ADDR_FBMEM == pKernelBus->PTEBAR2Aperture))))
    {
        pKernelBus->bar2[gfid].bBootstrap = NV_TRUE;
        //
        // Issue a bind in physical mode so that we may bootstrap the BAR2
        // tables and write the instance block using BAR2 PA mode.
        //
        status = kbusBindBar2_HAL(pGpu, pKernelBus, BAR2_MODE_PHYSICAL);
        NV_ASSERT_OR_RETURN(NV_OK == status, status);

        status = kbusSetupBar2PageTablesAtBottomOfFb_HAL(pGpu, pKernelBus, gfid);
        NV_ASSERT_OR_RETURN(NV_OK == status, status);
    }
    else if (IS_GFID_PF(gfid) && !KBUS_BAR0_PRAMIN_DISABLED(pGpu))
    {
        // Setup BAR0 window for page directory/table updates during BAR2 bootstrap
        status = kbusSetupBar0WindowBeforeBar2Bootstrap_HAL(pGpu, pKernelBus, &origVidOffset);
        NV_ASSERT_OR_RETURN(NV_OK == status, status);
    }

    // Get Bar2 VA limit.
    pKernelBus->bar2[gfid].vaLimit = kbusGetVaLimitForBar2_HAL(pGpu, pKernelBus);

    //
    // Reduce BAR2 VA space by FERMI_SMALL_PAGESIZE for host overfetch
    // bug WAR (Bug 529932/525381); the last BAR2 page will remain
    // mapped to the scratch page.
    //
    pVASpaceHeap = pKernelBus->virtualBar2[gfid].pVASpaceHeap;

    if (pVASpaceHeap != NULL)
    {
        if (pVASpaceHeap->eheapSetAllocRange(pVASpaceHeap, pKernelBus->bar2[gfid].rmApertureBase,
            pKernelBus->bar2[gfid].rmApertureLimit - FERMI_SMALL_PAGESIZE) != NV_OK)
        {
            DBG_BREAKPOINT();
        }
    }

    allocSize = kbusGetSizeOfBar2PageDirs_HAL(pGpu, pKernelBus) +
                kbusGetSizeOfBar2PageTables_HAL(pGpu, pKernelBus);

    if (pKernelBus->PDEBAR2Aperture == ADDR_FBMEM)
    {
        //
        // The page directories and page tables should all be within
        // the same type of memory.
        //
        NV_ASSERT_OR_GOTO(pKernelBus->PDEBAR2Aperture == pKernelBus->PTEBAR2Aperture,
                         cleanup);

        status = memdescCreate(&pPageLevelsMemDesc, pGpu,
                               allocSize,
                               RM_PAGE_SIZE,
                               NV_TRUE,
                               pKernelBus->PDEBAR2Aperture,
                               pKernelBus->PDEBAR2Attr,
                               MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE);
        NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);

        if (IS_GFID_VF(gfid))
        {
            memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_64,
                            pPageLevelsMemDesc);
            NV_ASSERT_OR_GOTO(status == NV_OK, cleanup);

            pKernelBus->bar2[gfid].pdeBase = memdescGetPhysAddr(pPageLevelsMemDesc,
                                                                AT_GPU, 0);

            pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc = pPageLevelsMemDesc;

            pKernelBus->bar2[gfid].pteBase = pKernelBus->bar2[gfid].pdeBase +
                                             kbusGetSizeOfBar2PageDirs_HAL(pGpu, pKernelBus);

            pKernelBus->bar2[gfid].pteBase = NV_ROUNDUP(pKernelBus->bar2[gfid].pteBase, RM_PAGE_SIZE);

            pKernelBus->virtualBar2[gfid].pPageLevels = kbusMapRmAperture_HAL(pGpu,
                                                                        pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc);
            NV_ASSERT_OR_RETURN(pKernelBus->virtualBar2[gfid].pPageLevels,
                          NV_ERR_INSUFFICIENT_RESOURCES);
        }
    }

    //
    // Setup a memdesc that covers all of BAR2's page levels.
    //
    // The following is based on _bar2WalkCBLevelAlloc().
    //
    if (IS_GFID_PF(gfid))
    {
        switch (pKernelBus->PDEBAR2Aperture)
        {
            default:
            case ADDR_FBMEM:
                if (pPageLevelsMemDesc != NULL)
                {
                    memdescDescribe(pPageLevelsMemDesc,
                                    pKernelBus->PDEBAR2Aperture,
                                    pKernelBus->bar2[gfid].pdeBase,
                                    allocSize);
                }
                break;

            case ADDR_SYSMEM:
                //
                // In SYSMEM, page level instances are allocated one at a time. It is
                // not guaranteed that they are contiguous. Thus, SYSMEM page level
                // instances are dynamically mapped-in via memmap as needed instead of
                // having one static mapping.
                //
                pPageLevelsMemDesc = NULL;
                break;
        }
        pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc = pPageLevelsMemDesc;

        if (KBUS_BAR0_PRAMIN_DISABLED(pGpu))
        {
            NV_ASSERT_OK_OR_GOTO(status, kbusInitVirtualBar2_HAL(pGpu, pKernelBus), cleanup);
        }
    }

    // Get the MMU format for BAR2.
    pKernelBus->bar2[gfid].pFmt = kgmmuFmtGet(pKernelGmmu, GMMU_FMT_VERSION_DEFAULT, 0);
    NV_ASSERT_OR_GOTO(NULL != pKernelBus->bar2[gfid].pFmt, cleanup);

    //
    // Initialize/allocate walker staging buffer only if PTEs in FBMEM
    // and we are currently bootstrapping BAR2.
    //
    if (pKernelBus->bar2[gfid].pWalkStagingBuffer == NULL &&
        pKernelBus->PTEBAR2Aperture == ADDR_FBMEM &&
        pKernelBus->bar2[gfid].bBootstrap &&
        !RMCFG_FEATURE_PLATFORM_GSP)
    {
        bUseStagingBuffer = NV_TRUE;
        pKernelBus->bar2[gfid].pWalkStagingBuffer = kbusCreateStagingMemdesc(pGpu);
    }

    // Create the MMU_WALKER state
    status = mmuWalkCreate(pKernelBus->bar2[gfid].pFmt->pRoot,
                           NULL,
                           &g_bar2WalkCallbacks,
                           walkFlags,
                           &pWalk,
                           (struct MMU_WALK_MEMDESC *) pKernelBus->bar2[gfid].pWalkStagingBuffer);
    NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
    pKernelBus->bar2[gfid].pWalk = pWalk;

    // We want to lock the small page table
    pLevelFmt = mmuFmtFindLevelWithPageShift(pKernelBus->bar2[gfid].pFmt->pRoot,
                                             RM_PAGE_SHIFT);

    // Setup walk user context.
    userCtx.pGpu = pGpu;
    userCtx.gfid = gfid;

    NV_ASSERT_OR_RETURN(pWalk != NULL, NV_ERR_INVALID_STATE);

    // Pre-reserve and init 4K tables through BAR0 window (bBootstrap) mode.
    NV_ASSERT_OK_OR_RETURN(mmuWalkSetUserCtx(pWalk, &userCtx));

    if (pKernelBus->bar2[gfid].cpuVisibleLimit != 0)
    {
        status = mmuWalkReserveEntries(pWalk, pLevelFmt, pKernelBus->bar2[gfid].cpuVisibleBase,
                                       pKernelBus->bar2[gfid].cpuVisibleLimit, NV_FALSE);
        NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);

        pKernelBus->bar2[gfid].cpuVisiblePgTblSize = pKernelBus->bar2[gfid].pageTblInit * pKernelBus->bar2[gfid].pageTblSize;

        status = mmuWalkSparsify(pWalk,
                                 pKernelBus->bar2[gfid].cpuVisibleBase,
                                 pKernelBus->bar2[gfid].cpuVisibleLimit,
                                 bUseStagingBuffer);
        NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
    }

    if (pKernelBus->bar2[gfid].cpuInvisibleLimit != 0)
    {
        status = mmuWalkReserveEntries(pWalk, pLevelFmt, pKernelBus->bar2[gfid].cpuInvisibleBase,
                                       pKernelBus->bar2[gfid].cpuInvisibleLimit, NV_FALSE);
        NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
        status = mmuWalkSparsify(pWalk,
                                 pKernelBus->bar2[gfid].cpuInvisibleBase,
                                 pKernelBus->bar2[gfid].cpuInvisibleLimit,
                                 bUseStagingBuffer);
        NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
    }

    NV_PRINTF(LEVEL_INFO, "(BAR2 0x%llx, PDB 0x%llx): vaLimit = 0x%llx\n",
              pKernelBus->bar2[gfid].instBlockBase, pKernelBus->bar2[gfid].pdeBase,
              pKernelBus->bar2[gfid].vaLimit);

    if (NULL != pKernelBus->bar2[gfid].pPDEMemDescForBootstrap)
    {
        memdescSetPageSize(pKernelBus->bar2[gfid].pPDEMemDescForBootstrap, AT_GPU,
                       FERMI_SMALL_PAGESIZE);
        pKernelBus->virtualBar2[gfid].pPDB = pKernelBus->bar2[gfid].pPDEMemDescForBootstrap;
    }
    else
    {
        memdescSetPageSize(pKernelBus->bar2[gfid].pPDEMemDesc, AT_GPU,
                       FERMI_SMALL_PAGESIZE);
        pKernelBus->virtualBar2[gfid].pPDB = pKernelBus->bar2[gfid].pPDEMemDesc;
    }

    if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus) &&
        (pKernelBus->virtualBar2[gfid].pPageLevelsMemDescForBootstrap != NULL))
    {
        //
        // This should be done before BAR2 is switched to virtual mode
        // because we can have flushes coming in when we start to use vitual BAR2.
        //
        status = kbusSetupCpuPointerForBusFlush_HAL(pGpu, pKernelBus);
        NV_ASSERT_OR_GOTO(status == NV_OK, cleanup);

        if (pKernelBus->virtualBar2[gfid].pPageLevelsForBootstrap == NULL)
        {
            status = kbusPreInitVirtualBar2_HAL(pGpu, pKernelBus);
            NV_ASSERT_OR_GOTO(status == NV_OK, cleanup);
        }

        // Bind the temporary instance block w/ the temporary PDB.
        status = kbusBindBar2_HAL(pGpu, pKernelBus, BAR2_MODE_VIRTUAL);
        NV_ASSERT_OR_GOTO(status == NV_OK, cleanup);
    }

    kbusPatchBar2Pdb_HAL(pGpu, pKernelBus);

cleanup:

    if (IS_GFID_VF(gfid) &&
        (pKernelBus->virtualBar2[gfid].pPageLevels != NULL) &&
        (pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc != NULL))
    {
        kbusUnmapRmAperture_HAL(pGpu,
                                pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc,
                                &pKernelBus->virtualBar2[gfid].pPageLevels, NV_TRUE);
        pKernelBus->virtualBar2[gfid].pPageLevels = NULL;
    }

    if (pWalk != NULL)
    {
        mmuWalkSetUserCtx(pWalk, NULL);
    }

    if (!kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus) && IS_GFID_PF(gfid) &&
        !KBUS_BAR0_PRAMIN_DISABLED(pGpu))
    {
        // Restore BAR0 window after BAR2 bootstrap
        kbusRestoreBar0WindowAfterBar2Bootstrap_HAL(pGpu, pKernelBus, origVidOffset);
    }

    if (status != NV_OK)
    {
        if (kbusTeardownBar2GpuVaSpace_HAL(pGpu, pKernelBus, gfid) != NV_OK)
        {
            DBG_BREAKPOINT();
        }
    }

    if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
    {
        pKernelBus->bar2[gfid].bBootstrap = NV_FALSE;
    }

    return status;
}

/*!
 * @brief Destroy BAR2 GPU vaspace
 *
 * 1. Free BAR2 GPU vaspace page directories & tables.
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns NV_OK on success.
 */
NV_STATUS
kbusTeardownBar2GpuVaSpace_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid
)
{
    NV_STATUS         status  = NV_OK;

    //
    // Nothing to be done in the guest in the paravirtualization case or if
    // guest is running SRIOV heavy mode.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_OK;
    }

    if (NULL != pKernelBus->bar2[gfid].pWalk)
    {
        const MMU_FMT_LEVEL *pLevelFmt     = NULL;
        MMU_WALK_USER_CTX    userCtx       = {0};
        NvU64                origVidOffset = 0;

        pLevelFmt = mmuFmtFindLevelWithPageShift(pKernelBus->bar2[gfid].pFmt->pRoot, RM_PAGE_SHIFT);

        userCtx.pGpu = pGpu;

        NV_ASSERT_OK_OR_RETURN(mmuWalkSetUserCtx(pKernelBus->bar2[gfid].pWalk, &userCtx));

        if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus) ||
            IS_GFID_VF(gfid) ||
            KBUS_BAR0_PRAMIN_DISABLED(pGpu))
        {
            mmuWalkLevelInstancesForceFree(pKernelBus->bar2[gfid].pWalk);
        }
        else
        {
            status = kbusSetupBar0WindowBeforeBar2Bootstrap_HAL(pGpu, pKernelBus, &origVidOffset);
            NV_ASSERT_OR_RETURN(NV_OK == status, status);

            if (pKernelBus->bar2[gfid].cpuVisibleLimit != 0)
            {
                status = mmuWalkUnmap(pKernelBus->bar2[gfid].pWalk, pKernelBus->bar2[gfid].cpuVisibleBase, pKernelBus->bar2[gfid].cpuVisibleLimit);
                NV_ASSERT(NV_OK == status);
                mmuWalkReleaseEntries(pKernelBus->bar2[gfid].pWalk, pLevelFmt, pKernelBus->bar2[gfid].cpuVisibleBase, pKernelBus->bar2[gfid].cpuVisibleLimit);
            }

            if (pKernelBus->bar2[gfid].cpuInvisibleLimit != 0)
            {
                status = mmuWalkUnmap(pKernelBus->bar2[gfid].pWalk, pKernelBus->bar2[gfid].cpuInvisibleBase, pKernelBus->bar2[gfid].cpuInvisibleLimit);
                NV_ASSERT(NV_OK == status);
                mmuWalkReleaseEntries(pKernelBus->bar2[gfid].pWalk, pLevelFmt, pKernelBus->bar2[gfid].cpuInvisibleBase, pKernelBus->bar2[gfid].cpuInvisibleLimit);
            }

            kbusRestoreBar0WindowAfterBar2Bootstrap_HAL(pGpu, pKernelBus, origVidOffset);
        }

        NV_ASSERT_OK_OR_RETURN(mmuWalkSetUserCtx(pKernelBus->bar2[gfid].pWalk, NULL));

        if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
        {
            // Free the temporary instance block and page level instances.
            kbusTeardownBar2PageTablesAtBottomOfFb_HAL(pGpu, pKernelBus, gfid);
            kbusTeardownBar2InstBlkAtBottomOfFb_HAL(pGpu, pKernelBus, gfid);
        }

        mmuWalkDestroy(pKernelBus->bar2[gfid].pWalk);
        pKernelBus->bar2[gfid].pWalk                    = NULL;
        pKernelBus->bar2[gfid].pPDEMemDesc              = NULL;
        pKernelBus->bar2[gfid].pPDEMemDescForBootstrap  = NULL;
        pKernelBus->virtualBar2[gfid].pPTEMemDesc       = NULL;

        // Free staging buffer
        memdescFree(pKernelBus->bar2[gfid].pWalkStagingBuffer);
        memdescDestroy(pKernelBus->bar2[gfid].pWalkStagingBuffer);
        pKernelBus->bar2[gfid].pWalkStagingBuffer = NULL;

        if (IS_GFID_VF(gfid) && (pKernelBus->virtualBar2[gfid].pPageLevels != NULL))
        {
            kbusUnmapRmAperture_HAL(pGpu,
                                    pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc,
                                    &pKernelBus->virtualBar2[gfid].pPageLevels,
                                    NV_TRUE);
            pKernelBus->virtualBar2[gfid].pPageLevels = NULL;
        }

        // Free the overall page levels memdesc.
        if (pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc != NULL)
        {
            memdescFree(pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc);
            memdescDestroy(pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc);
            pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc = NULL;
        }

        if (IS_GSP_CLIENT(pGpu))
        {
            //
            // Normally virtualBar2.pPDB (which equals to the memDesc
            // of BAR2 root directory) gets freed when BAR2 page table got
            // destroyed. But in RM-offload, virtualBar2.pPDB in CPU-RM
            // is patched to GSP-RM's address, thus it won't be freed when
            // destroying BAR2 page table. So we need to explicitly free it
            // at destruct time.
            //
            if (pKernelBus->virtualBar2[gfid].pPDB != NULL)
            {
                memdescFree(pKernelBus->virtualBar2[gfid].pPDB);
                memdescDestroy(pKernelBus->virtualBar2[gfid].pPDB);
                pKernelBus->virtualBar2[gfid].pPDB = NULL;
            }

            //
            // No more need for CPU-RM's page table, thus requesting GSP-RM to
            // delete the PDE3[0] value from GSP-RM's page table (by wrinting 0
            // to GSP-RM's PDE3[0].
            //
            NV_RM_RPC_UPDATE_BAR_PDE(pGpu, NV_RPC_UPDATE_PDE_BAR_2, 0, pKernelBus->bar2[gfid].pFmt->pRoot->virtAddrBitLo, status);
        }

        if (IS_GFID_VF(gfid) && (pKernelBus->bar2[gfid].pInstBlkMemDesc != NULL))
        {
            memdescFree(pKernelBus->bar2[gfid].pInstBlkMemDesc);
            memdescDestroy(pKernelBus->bar2[gfid].pInstBlkMemDesc);
            pKernelBus->bar2[gfid].pInstBlkMemDesc = NULL;
        }
    }

    return status;
}

/*!
 * @brief Setup BAR0 window for BAR2 setup
 *
 * We point the BAR0 window to the start of the BAR2 page directory
 *
 * @param[in]  pGpu
 * @param[in]  pKernelBus
 * @param[out] pOrigVidOffset Location to Save the original BAR0 window offset
 *
 * @returns NV_OK on success.
 */
NV_STATUS
kbusSetupBar0WindowBeforeBar2Bootstrap_GM107
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus,
    NvU64       *pOrigVidOffset
)
{
    NV_STATUS         status  = NV_OK;

    if (kbusIsBarAccessBlocked(pKernelBus))
        return NV_OK;

    NV_ASSERT_OR_RETURN(!KBUS_BAR0_PRAMIN_DISABLED(pGpu),
                        NV_ERR_INVALID_STATE);
    // Check that Bar2 Page Dir starts at or after bar0 window vid offset
    if (ADDR_FBMEM == pKernelBus->PDEBAR2Aperture ||
        ADDR_FBMEM == pKernelBus->PTEBAR2Aperture)
    {
        // Right now, PDE needs to be in FBMEM for BAR0 window to work.
        NV_ASSERT_OR_RETURN(ADDR_FBMEM == pKernelBus->PDEBAR2Aperture, NV_ERR_NOT_SUPPORTED);

        // Save original BAR0 window base (restored in cleanup).
        *pOrigVidOffset = kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);

        // Set BAR0 window base to memory region reserved for BAR2 page level instances.
        status = kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus,
                                                pKernelBus->bar2[GPU_GFID_PF].pdeBase & ~0xffffULL);
        NV_ASSERT_OR_RETURN(NV_OK == status, status);

        // Get BAR0 window offset to be used for BAR2 init.
        pKernelBus->bar2[GPU_GFID_PF].bar2OffsetInBar0Window =
            (pKernelBus->bar2[GPU_GFID_PF].pdeBase - kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus)) +
             NV_PRAMIN_DATA008(0);
    }

    pKernelBus->bar2[GPU_GFID_PF].bBootstrap = NV_TRUE;

    return NV_OK;
}

/*!
 * @brief Restore BAR0 window after BAR2 setup
 *
 * Restore the BAR0 window to the original offset
 *
 * @param[in]  pGpu
 * @param[in]  pKernelBus
 * @param[in]  origVidOffset  Location to restore the original BAR0 window offset
 *
 * @returns NV_OK on success.
 */
void
kbusRestoreBar0WindowAfterBar2Bootstrap_GM107
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus,
    NvU64        origVidOffset
)
{
    if (kbusIsBarAccessBlocked(pKernelBus))
        return;

    NV_ASSERT_OR_RETURN_VOID(!KBUS_BAR0_PRAMIN_DISABLED(pGpu));

    NV_ASSERT(pKernelBus->bar2[GPU_GFID_PF].bBootstrap);
    pKernelBus->bar2[GPU_GFID_PF].bBootstrap = NV_FALSE;

    if (ADDR_FBMEM == pKernelBus->PDEBAR2Aperture ||
        ADDR_FBMEM == pKernelBus->PTEBAR2Aperture)
    {
        NV_STATUS status;
        status = kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, origVidOffset);
        NV_ASSERT(NV_OK == status);
        pKernelBus->bar2[GPU_GFID_PF].bar2OffsetInBar0Window = 0;
    }
}

/*!
 * Defines the data needed to iterate over the last level during map VA op.
 * Note: Used only in the new VMM code path.
 */
struct MMU_MAP_ITERATOR
{
    /*!
     * @copydoc GMMU_FMT
     */
    const GMMU_FMT *pFmt;

    /*!
     * Physical aperture of the pages.
     */
    GMMU_APERTURE aperture;

    /*!
     * Physical pages to map. Always points to 4K-sized pages.
     */
    DMA_PAGE_ARRAY *pPageArray;

    /*!
     * The index of pPageArray that needs to be mapped.
     */
    NvU32 currIdx;

    /*!
     * Physical address of the last page mapped.
     */
    NvU64 physAddr;

    /*!
     * Template used to initialize PTEs. Contains values that do not change
     * across one map operation.
     */
    GMMU_ENTRY_VALUE pteTemplate;

    /*!
     * The PTE physical address field to use based on the PTE aperture.
     */
    const GMMU_FIELD_ADDRESS *pAddrField;
};

static void
_busWalkCBMapNextEntries_UpdatePhysAddr
(
    OBJGPU           *pGpu,
    GMMU_ENTRY_VALUE *pEntryValue,
    MMU_MAP_ITERATOR *pIter,
    const NvU64       pageSize
)
{
    // Update the PTE with the physical address.
    if (pIter->currIdx < pIter->pPageArray->count)
    {
        pIter->physAddr = dmaPageArrayGetPhysAddr(pIter->pPageArray,
                                                  pIter->currIdx);
        pIter->physAddr = NV_ALIGN_DOWN64(pIter->physAddr, pageSize);
    }
    else
    {
        //
        // As BAR2 page tables are physically contiguous, physAddr can be
        // incremented.
        //
        // Should not be the first page (currIdx == 0) being mapped.
        //
        NV_ASSERT_OR_RETURN_VOID((pIter->pPageArray->count == 1) &&
                               (pIter->currIdx > 0));
        pIter->physAddr += pageSize;
    }

    gmmuFieldSetAddress(pIter->pAddrField,
        kgmmuEncodePhysAddr(GPU_GET_KERNEL_GMMU(pGpu),
            pIter->aperture, pIter->physAddr, NVLINK_INVALID_FABRIC_ADDR),
        pEntryValue->v8);

    //
    // pPageArray deals in 4K-pages. Increment by the ratio of mapping page
    // size to 4K.
    //
    pIter->currIdx += (NvU32)(pageSize / RM_PAGE_SIZE);
}

/*!
 * Implementation of @ref MmuWalkCBMapNextEntries for BAR2
 */
static void
_kbusWalkCBMapNextEntries_RmAperture
(
    MMU_WALK_USER_CTX        *pUserCtx,
    const MMU_MAP_TARGET     *pTarget,
    const MMU_WALK_MEMDESC   *pLevelMem,
    const NvU32               entryIndexLo,
    const NvU32               entryIndexHi,
    NvU32                    *pProgress
)
{
    OBJGPU              *pGpu        = pUserCtx->pGpu;
    KernelBus           *pKernelBus  = GPU_GET_KERNEL_BUS(pGpu);
    MMU_MAP_ITERATOR    *pIter       = pTarget->pIter;
    const MMU_FMT_LEVEL *pLevelFmt   = pTarget->pLevelFmt;
    NvU8                *pMap        = NULL;
    void                *pPriv       = NULL;
    MEMORY_DESCRIPTOR   *pMemDesc    = (MEMORY_DESCRIPTOR*)pLevelMem;
    const NvU64          pageSize    = mmuFmtLevelPageSize(pLevelFmt);
    NV_STATUS            status      = NV_OK;
    GMMU_ENTRY_VALUE     entryValue;
    NvU32                entryIndex;
    NvU32                entryOffset;
    NvU32                sizeInDWord = 0;
    NvU64                entry = 0;
    NvU32                gfid = pUserCtx->gfid;

    NV_PRINTF(LEVEL_INFO, "[GPU%u]: PA 0x%llX, Entries 0x%X-0x%X\n",
              pUserCtx->pGpu->gpuInstance,
              memdescGetPhysAddr(pMemDesc, AT_GPU, 0), entryIndexLo,
              entryIndexHi);

    //
    // Initialize the PTE with the template. The template contains the values
    // that do not change across PTEs for this map operation.
    //
    portMemCopy(entryValue.v8, sizeof(pIter->pteTemplate), pIter->pteTemplate.v8, sizeof(pIter->pteTemplate));

    if (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM)
    {
        if (kbusIsBarAccessBlocked(pKernelBus))
        {
            MemoryManager    *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
            TRANSFER_SURFACE surf = {0};
            NvU32            sizeOfEntries;
            NvU32            index;

            NV_ASSERT_OR_RETURN_VOID(pKernelBus->virtualBar2[gfid].pPageLevels == NULL);

            surf.pMemDesc = pMemDesc;
            surf.offset = entryIndexLo * pLevelFmt->entrySize;

            sizeOfEntries = (entryIndexHi - entryIndexLo + 1) * pLevelFmt->entrySize;

            pMap = memmgrMemBeginTransfer(pMemoryManager, &surf, sizeOfEntries,
                                          TRANSFER_FLAGS_SHADOW_ALLOC);

            for (entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++)
            {
                // Update the PTE with the physical address.
                _busWalkCBMapNextEntries_UpdatePhysAddr(pGpu,
                                                        &entryValue,
                                                        pIter,
                                                        pageSize);

                index = (entryIndex - entryIndexLo) * pLevelFmt->entrySize;
                portMemCopy(&pMap[index], pLevelFmt->entrySize,
                            entryValue.v8, pLevelFmt->entrySize);
            }

            memmgrMemEndTransfer(pMemoryManager, &surf, sizeOfEntries,
                                 TRANSFER_FLAGS_SHADOW_ALLOC);
        }
        else if (pKernelBus->virtualBar2[gfid].pPageLevels != NULL)
        {
            //
            // Determine the start of the desired page level offset from
            // CPU mapping to the start of the BAR2 VAS page levels.
            //
            if (pKernelBus->bar2[gfid].bMigrating)
            {
                // In the migration phase. HW is using the page tables at bottom of FB.
                NV_ASSERT_OR_RETURN_VOID(NULL != pKernelBus->virtualBar2[gfid].pPageLevelsForBootstrap);
                pMap = memdescGetPhysAddr(pMemDesc, AT_GPU, 0) -
                                      pKernelBus->bar2[gfid].pdeBaseForBootstrap +
                                      pKernelBus->virtualBar2[gfid].pPageLevelsForBootstrap;
            }
            else
            {
                // Migration is done. HW is using the page tables at top of FB.
                pMap = memdescGetPhysAddr(pMemDesc, AT_GPU, 0) -
                                      pKernelBus->bar2[gfid].pdeBase +
                                      pKernelBus->virtualBar2[gfid].pPageLevels;
            }

            for (entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++)
            {
                // Update the PTE with the physical address.
                _busWalkCBMapNextEntries_UpdatePhysAddr(pGpu,
                                                       &entryValue,
                                                        pIter,
                                                        pageSize);

                entryOffset = entryIndex * pLevelFmt->entrySize;

                // Commit to memory.
                portMemCopy(pMap + entryOffset, pLevelFmt->entrySize, entryValue.v8, pLevelFmt->entrySize);
            }
        }
        else if (pKernelBus->bar2[gfid].bBootstrap)
        {

            for ( entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++ )
            {
                // Update the PTE with the physical address.
                _busWalkCBMapNextEntries_UpdatePhysAddr(pGpu,
                                                        &entryValue,
                                                        pIter,
                                                        pageSize);

                entryOffset = entryIndex * pLevelFmt->entrySize;

                if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
                {
                    pMap = kbusCpuOffsetInBar2WindowGet(pGpu, pKernelBus, pMemDesc);
                    NV_ASSERT_OR_RETURN_VOID(NULL != pMap);
                    portMemCopy(pMap + entryOffset,
                                pLevelFmt->entrySize, entryValue.v8,
                                pLevelFmt->entrySize);
                }
                else
                {
                    // Use BAR0 or nvlink if available
                    NvU64 entryStart = memdescGetPhysAddr(pMemDesc, FORCE_VMMU_TRANSLATION(pMemDesc, AT_GPU), entryOffset);

                    if (KBUS_BAR0_PRAMIN_DISABLED(pGpu))
                    {
                        NvU8 *pMapping = NULL;
                        NvP64 pPriv    = NULL;

                        status = kbusMapCoherentCpuMapping_HAL(pGpu,
                                                               pKernelBus,
                                                               pMemDesc,
                                                               entryOffset,
                                                               pLevelFmt->entrySize,
                                                               NV_PROTECT_READ_WRITE,
                                                               (NvP64 *)&pMapping,
                                                               &pPriv);

                        NV_ASSERT_OR_RETURN_VOID(status == NV_OK);
                        NV_ASSERT_OR_RETURN_VOID(pMapping != NULL);
                        portMemCopy(pMapping,
                                    pLevelFmt->entrySize, entryValue.v8,
                                    pLevelFmt->entrySize);
                        kbusUnmapCoherentCpuMapping_HAL(pGpu, pKernelBus, pMemDesc, pMapping, pPriv);
                    }
                    else
                    {
                        sizeInDWord = (NvU32)NV_CEIL(pLevelFmt->entrySize, sizeof(NvU32));
                        NvU32 i;
                        for (i = 0; i < sizeInDWord; i++)
                        {
                            // BAR0 write.
                            status = kbusMemAccessBar0Window_HAL(pGpu, pKernelBus,
                                                  (entryStart + (sizeof(NvU32) * i)),
                                                  &entryValue.v32[i],
                                                  sizeof(NvU32),
                                                  NV_FALSE,
                                                  ADDR_FBMEM);
                            NV_ASSERT_OR_RETURN_VOID(NV_OK == status);
                        }
                    }

                    entry = entryStart;
                }
            }
            //
            // Use PRAMIN flush to make sure that BAR0 writes has reached the memory
            //
            if (pKernelBus->bar2[gfid].bBootstrap &&
                !kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus) &&
                !RMCFG_FEATURE_PLATFORM_GSP)
            {
                NvU32 data = 0;
                NvU32 i;
                for (i = 0; i < sizeInDWord; i++)
                {
                    NV_ASSERT_OR_RETURN_VOID(kbusMemAccessBar0Window_HAL(pGpu, pKernelBus,
                                                        (entry + (sizeof(NvU32) * i)), &data, sizeof(NvU32),
                                                        NV_TRUE, ADDR_FBMEM) == NV_OK);
                }
            }
        }
        else
        {
            //
            // We are migrating and old page tables are currently active. So, PTE
            // updates should be made in the old page tables at the bottom of FB.
            //
            NV_ASSERT_OR_RETURN_VOID(pKernelBus->bar2[gfid].bMigrating);
            NV_ASSERT_OR_RETURN_VOID(NULL == pKernelBus->virtualBar2[gfid].pPageLevels);
            NV_ASSERT_OR_RETURN_VOID(NULL != pKernelBus->virtualBar2[gfid].pPageLevelsForBootstrap);

            pMap = memdescGetPhysAddr(pMemDesc, AT_GPU, 0) -
                                  pKernelBus->bar2[gfid].pdeBaseForBootstrap +
                                  pKernelBus->virtualBar2[gfid].pPageLevelsForBootstrap;

            for (entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++)
            {
                // Update the PTE with the physical address.
                _busWalkCBMapNextEntries_UpdatePhysAddr(pGpu,
                                                        &entryValue,
                                                        pIter,
                                                        pageSize);

                entryOffset = entryIndex * pLevelFmt->entrySize;

                // Commit to memory.
                portMemCopy(pMap + entryOffset, pLevelFmt->entrySize, entryValue.v8, pLevelFmt->entrySize);
            }
        }
    }
    else
    {
        NV_ASSERT(memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM &&
                  pKernelBus->virtualBar2[gfid].pPageLevels == NULL);

        // Plain old memmap.
        status = memdescMapOld(pMemDesc, 0,
                               pMemDesc->Size,
                               NV_TRUE, // kernel,
                               NV_PROTECT_READ_WRITE,
                               (void **)&pMap,
                               &pPriv);
        NV_ASSERT_OR_RETURN_VOID(NV_OK == status);

        for ( entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++ )
        {
            // Update the PTE with the physical address.
            _busWalkCBMapNextEntries_UpdatePhysAddr(pGpu,
                                                    &entryValue,
                                                    pIter,
                                                    pageSize);

            entryOffset = entryIndex * pLevelFmt->entrySize;

            // Memory-mapped write.
            portMemCopy(pMap + entryOffset,
                        pLevelFmt->entrySize,
                        entryValue.v8,
                        pLevelFmt->entrySize);
        }

        memdescUnmapOld(pMemDesc, 1, 0, pMap, pPriv);
    }

    *pProgress = entryIndexHi - entryIndexLo + 1;
}

/*!
 *  @brief Third level of RmAperture support. This routine writes BAR2 PTEs.
 *
 *  @param[in]   pGpu
 *  @param[in]   pKernelBus
 *  @param[in]   pMemDesc    The memory area to copy from.
 *  @param[in]   vaddr       Offset into bar2 to program
 *  @param[in]   vaSize      Amount of VA to write (can be greater than pMemDesc size)
 *  @param[in]   flags       Defined by UPDATE_RM_APERTURE_FLAGS_*
 *
 *  @returns NV_OK on success, failure in some bootstrapping cases.
 */
NV_STATUS
kbusUpdateRmAperture_GM107
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus,
    PMEMORY_DESCRIPTOR pMemDesc,
    NvU64        vaddr,
    NvU64        vaSize,
    NvU32        flags
)
{
    KernelGmmu         *pKernelGmmu  = GPU_GET_KERNEL_GMMU(pGpu);
    PMEMORY_DESCRIPTOR  pSubDevMemDesc;
    NV_STATUS           status      = NV_OK;
    NvBool              bInvalidate = !!(flags & UPDATE_RM_APERTURE_FLAGS_INVALIDATE);
    NvBool              bDiscard    = !!(flags & UPDATE_RM_APERTURE_FLAGS_DISCARD);
    NvBool              bSparsify   = !!(flags & UPDATE_RM_APERTURE_FLAGS_SPARSIFY);
    MMU_MAP_TARGET      mapTarget = {0};
    MMU_MAP_ITERATOR    mapIter   = {0};
    MMU_WALK_USER_CTX   userCtx   = {0};
    DMA_PAGE_ARRAY      pageArray;
    NvU64               origVidOffset = 0;
    NvU64               vaLo;
    NvU64               vaHi;
    NvU32               gfid;
    const NvU64         pageSize  = FERMI_SMALL_PAGESIZE;
    const GMMU_FMT     *pFmt;
    ADDRESS_TRANSLATION addressTranslation;
    NvBool              bCallingContextPlugin;

    //
    // In case of SR-IOV heavy, host RM must update VF BAR2 page tables
    // only for CPU invisible range. VF BAR2's CPU visible range is not
    // in use on host RM.
    //
    if (!(flags & UPDATE_RM_APERTURE_FLAGS_CPU_INVISIBLE_RANGE))
    {
        gfid = GPU_GFID_PF;
    }
    else
    {
        NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));
        NV_ASSERT_OK_OR_RETURN(vgpuIsCallingContextPlugin(pGpu, &bCallingContextPlugin));
        if (bCallingContextPlugin)
        {
            gfid = GPU_GFID_PF;
        }
    }

    // Math below requires page-sized va.
    if (vaSize == 0 || vaSize & RM_PAGE_MASK)
    {
        NV_PRINTF(LEVEL_ERROR, "unsupported VA size (0x%llx)\n", vaSize);
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Currently don't do anything at unmap.
    if (bDiscard && !bSparsify)
        return NV_OK;

    vaLo = NV_ALIGN_DOWN64(vaddr, pageSize);
    vaHi = NV_ALIGN_UP64(vaddr + vaSize, pageSize) - 1;
    pSubDevMemDesc = memdescGetMemDescFromGpu(pMemDesc, pGpu);
    //
    // In case of SR-IOV heavy, host RM updates VF BAR2, so
    // if the update is for VF BAR2 (IS_GFID_PF(gfid) is false),
    // use GPA, otherwise use SPA.
    //
    if (IS_GFID_PF(gfid))
    {
        addressTranslation = FORCE_VMMU_TRANSLATION(pSubDevMemDesc, AT_GPU);
    }
    else
    {
        addressTranslation = AT_GPU;
    }

    dmaPageArrayInitFromMemDesc(&pageArray, pSubDevMemDesc, addressTranslation);
    userCtx.pGpu = pGpu;
    userCtx.gfid = gfid;
    NV_ASSERT_OK_OR_RETURN(mmuWalkSetUserCtx(pKernelBus->bar2[gfid].pWalk, &userCtx));

    if (bSparsify)
    {
        NV_PRINTF(LEVEL_INFO,
                  "mmuWalkSparsify pwalk=%p, vaLo=%llx, vaHi = %llx\n",
                  pKernelBus->bar2[gfid].pWalk, vaLo, vaHi);

        status = mmuWalkSparsify(pKernelBus->bar2[gfid].pWalk, vaLo, vaHi, NV_FALSE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "mmuWalkSparsify status=%x pwalk=%p, vaLo=%llx, vaHi = %llx\n",
                      status, pKernelBus->bar2[gfid].pWalk, vaLo, vaHi);
        }
    }
    else
    {
        pFmt = pKernelBus->bar2[gfid].pFmt;

        NV_CHECK_OR_RETURN(LEVEL_ERROR, pFmt != NULL, NV_ERR_INVALID_ARGUMENT);

        // MMU_MAP_CTX
        mapTarget.pLevelFmt      = mmuFmtFindLevelWithPageShift(pFmt->pRoot,
                                                                BIT_IDX_64(pageSize));
        mapTarget.pIter          = &mapIter;
        mapTarget.MapNextEntries = _kbusWalkCBMapNextEntries_RmAperture;

        // MMU_MAP_ITER
        mapIter.pFmt       = pFmt;
        mapIter.aperture   = kgmmuGetMemAperture(pKernelGmmu, pMemDesc);
        mapIter.pPageArray = &pageArray;

        //
        // Setup a template PTE with those values that will not change across
        // PTEs during mapping.
        //
        nvFieldSetBool(&pFmt->pPte->fldValid, NV_TRUE, mapIter.pteTemplate.v8);
        if (pFmt->version == GMMU_FMT_VERSION_3)
        {
            NvU32 ptePcfHw  = 0;
            NvU32 ptePcfSw  = 0;

            if (memdescGetVolatility(pMemDesc))
            {
                ptePcfSw = 1 << SW_MMU_PCF_UNCACHED_IDX;
            }
            else
            {
                //
                // For internal DMA mappings to sysmem, we should always use
                // GPU-uncached because RM won't invalidate L2 upon unmap
                //
                NV_ASSERT(mapIter.aperture == GMMU_APERTURE_VIDEO);
            }

            ptePcfSw |= (1 << SW_MMU_PCF_REGULAR_IDX);

            NV_ASSERT_OR_RETURN(kgmmuTranslatePtePcfFromSw_HAL(pKernelGmmu, ptePcfSw,
                &ptePcfHw) == NV_OK, NV_ERR_INVALID_ARGUMENT);
            nvFieldSet32(&pFmt->pPte->fldPtePcf, ptePcfHw, mapIter.pteTemplate.v8);
        }
        else
        {
            nvFieldSetBool(&pFmt->pPte->fldVolatile, memdescGetVolatility(pMemDesc), mapIter.pteTemplate.v8);
        }

        gmmuFieldSetAperture(&pFmt->pPte->fldAperture,
                             mapIter.aperture,
                             mapIter.pteTemplate.v8);

        //
        // Determine the PTE physical address field to use based on the PTE
        // aperture. Physical addresses themselves will get added to the PTE
        // during mapping.
        //
        mapIter.pAddrField =
            gmmuFmtPtePhysAddrFld(pFmt->pPte,
                                  gmmuFieldGetAperture(
                                      &pFmt->pPte->fldAperture,
                                      mapIter.pteTemplate.v8));


        // Write PTE kind.
        nvFieldSet32(&pFmt->pPte->fldKind, memdescGetPteKind(pMemDesc),
                     mapIter.pteTemplate.v8);

        //
        // We haven't yet self-mapped the BAR2 page tables.
        // This call is to do the same.
        // So keep BAR2 in bootstrap mode to allow BAR0 window updates.
        //
        if ((ADDR_FBMEM == pKernelBus->PDEBAR2Aperture ||
             ADDR_FBMEM == pKernelBus->PTEBAR2Aperture ||
             pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB)) &&
             !kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus) &&
             pKernelBus->virtualBar2[gfid].pPageLevels == NULL && IS_GFID_PF(gfid))
        {
            status = kbusSetupBar0WindowBeforeBar2Bootstrap_HAL(pGpu, pKernelBus, &origVidOffset);
            NV_ASSERT_OR_RETURN(NV_OK == status, status);
        }
        status = mmuWalkMap(pKernelBus->bar2[gfid].pWalk, vaLo, vaHi, &mapTarget);
        NV_ASSERT(NV_OK == status);
    }

    NV_ASSERT_OK_OR_RETURN(mmuWalkSetUserCtx(pKernelBus->bar2[gfid].pWalk, NULL));

    if (pKernelBus->bar2[gfid].bBootstrap &&
        !kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
    {
        kbusRestoreBar0WindowAfterBar2Bootstrap_HAL(pGpu, pKernelBus, origVidOffset);
    }

    //
    // Synchronize BAR2 address space to memory and then invalidate TLB
    // to invalidate any cached PTEs.
    //
    if (bInvalidate)
    {
        osFlushCpuWriteCombineBuffer();

        if (pKernelBus->pReadToFlush != NULL)
        {
            NvU32 flushFlag = kbusGetFlushAperture(pKernelBus,
                                                   memdescGetAddressSpace(pKernelBus->virtualBar2[gfid].pPTEMemDesc));
            kbusFlush_HAL(pGpu, pKernelBus, flushFlag);
        }
        else
        {
            kbusSendSysmembar(pGpu, pKernelBus);
        }

        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
        pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
        pKernelBus  = GPU_GET_KERNEL_BUS(pGpu);
        kgmmuInvalidateTlb_HAL(pGpu, pKernelGmmu,
                              pKernelBus->virtualBar2[gfid].pPDB,
                              pKernelBus->virtualBar2[gfid].flags,
                              PTE_DOWNGRADE, 0,
                              NV_GMMU_INVAL_SCOPE_NON_LINK_TLBS);
        SLI_LOOP_END
        pKernelBus  = GPU_GET_KERNEL_BUS(pGpu);
    }

    return status;
}

/**
 * @brief This function is used to return the BAR1 VA space.
 *        BAR1 VA space per-GPU, no longer shared
 */
OBJVASPACE *kbusGetBar1VASpace_GM107(OBJGPU *pGpu, KernelBus *pKernelBus)
{
    NvU32             gfid;
    NvBool            bCallingContextPlugin;

    NV_ASSERT_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK, NULL);
    NV_ASSERT_OR_RETURN(vgpuIsCallingContextPlugin(pGpu, &bCallingContextPlugin) == NV_OK, NULL);
    if (bCallingContextPlugin || !gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        gfid = GPU_GFID_PF;
    }

    return pKernelBus->bar1[gfid].pVAS;
}

MAKE_VECTOR(MemoryRangeVector, MemoryRange);

//
// Note: When static BAR1 is enabled, this function may be called without
// RMAPI and GPU locks being acquired since we don't expect the
// GPU state or the memdesc state to change since we're just fetching
// immutable BAR1 addressses for the handle/memdesc.
//
NV_STATUS
kbusMapFbAperture_GM107
(
    OBJGPU     *pGpu,
    KernelBus  *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    MemoryRange mapRange,
    MemoryArea *pMemArea,
    NvU32       flags,
    Device     *pDevice
)
{
    NvBool           bBcState = gpumgrGetBcEnabledStatus(pGpu);
    OBJVASPACE      *pVAS = kbusGetBar1VASpace_HAL(pGpu, pKernelBus);
    NV_STATUS        rmStatus   = NV_OK;
    MemoryRangeVector vect;
    NvU64 totalMapped = 0;
    NvU64 curMappingSize = mapRange.size;
    //
    // pageSize is used to ensure that all chunks are aligned to the OS's page size.
    // Only used in this function's algorithm, not internally by DMA code
    //
    NvU64 pageSize = osGetPageSize();

    // BUS_MAP_FB_FLAGS_MAP_OFFSET_FIXED effectively implies we have a preallocated memArea.
    flags |= (flags & BUS_MAP_FB_FLAGS_MAP_OFFSET_FIXED) ? BUS_MAP_FB_FLAGS_UNMANAGED_MEM_AREA : 0;

    NV_ASSERT((flags & BUS_MAP_FB_FLAGS_FERMI_INVALID) == 0);
    NV_ASSERT_OR_RETURN(pVAS != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(!(flags & BUS_MAP_FB_FLAGS_MAP_OFFSET_FIXED) || pMemArea->numRanges == 1,
        NV_ERR_INVALID_ARGUMENT);

    //
    // Try to get a static BAR1 mapping. If static BAR1 is not enabled
    // or the allocation goes in the dynamic range, we will get NV_ERR_NOT_SUPPORTED
    // and fall through to the dynamic map (also used for initial static BAR1 setup itself)
    //
    rmStatus = kbusGetStaticFbAperture_HAL(pGpu, pKernelBus, pMemDesc,
                                           mapRange, pMemArea, flags);

    if (rmStatus == NV_OK)
    {
        // We succeeded in getting a static aperture mapping exit (no cleanup required yet)
        return rmStatus;
    }
    else if (rmStatus != NV_ERR_NOT_SUPPORTED)
    {
        NV_PRINTF(LEVEL_ERROR, "static BAR1 reported error\n");
        return rmStatus;
    }

    // If not static BAR1, we must have the GPU lock to create a new mapping.
    NV_ASSERT_OR_RETURN(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)),
                        NV_ERR_INVALID_LOCK_STATE);

    rmStatus = NV_OK;

    // Set BC to enabled in UC flag not passed
    gpumgrSetBcEnabledStatus(pGpu, IsSLIEnabled(pGpu) && ((flags & BUS_MAP_FB_FLAGS_MAP_UNICAST) == 0) &&
        ((flags & BUS_MAP_FB_FLAGS_PRE_INIT) == 0));

    vectInit(&vect, portMemAllocatorGetGlobalNonPaged(), 0);

    if(flags & BUS_MAP_FB_FLAGS_UNMANAGED_MEM_AREA)
    {
        pMemArea->numRanges = 1;
        pMemArea->pRanges[0].size = mapRange.size;
        rmStatus = _kbusMapAperture_GM107(pGpu, pKernelBus, pMemDesc, pVAS, mapRange.start,
            &pMemArea->pRanges[0].start, &pMemArea->pRanges[0].size, flags, pDevice);
        goto cleanup;
    }

    while (totalMapped < mapRange.size)
    {
        MemoryRange memRange;
        NvU64 cap = vectCapacity(&vect);

        // Round out the ends with potentially non-aligned size
        curMappingSize = NV_MIN(curMappingSize, mapRange.size - totalMapped);
        memRange = mrangeMake(0, curMappingSize);

        if (cap == vectCount(&vect))
        {
            // Preallocate capacity to get rid of complicated error paths
            NvU64 nextCap = (cap == 0) ? 8 : (cap * 2);
            NV_ASSERT_OK_OR_GOTO(rmStatus, vectReserve(&vect, nextCap), err);
        }

        rmStatus = _kbusMapAperture_GM107(pGpu, pKernelBus, pMemDesc, pVAS, mapRange.start + totalMapped,
            &memRange.start, &memRange.size, flags, pDevice);

        if (rmStatus != NV_OK)
        {
            NV_CHECK_OR_GOTO(LEVEL_INFO, curMappingSize > pageSize, err);
            NV_CHECK_OR_GOTO(LEVEL_INFO, (flags & BUS_MAP_FB_FLAGS_ALLOW_DISCONTIG) &&
                pKernelBus->bBar1DiscontigEnabled, err);
            curMappingSize = NV_ALIGN_UP64(curMappingSize >> 1, pageSize);
            continue;
        }

        totalMapped += curMappingSize;
        vectAppend(&vect,  &memRange); // Guaranteed to succeed
    }

    pMemArea->numRanges = vectCount(&vect);
    pMemArea->pRanges = portMemAllocNonPaged(sizeof(MemoryRange) * pMemArea->numRanges);
    NV_ASSERT_TRUE_OR_GOTO(rmStatus, pMemArea->pRanges != NULL, NV_ERR_NO_MEMORY, err);
    portMemCopy(pMemArea->pRanges, sizeof(MemoryRange) * pMemArea->numRanges,
        vectAt(&vect, 0), sizeof(MemoryRange) * pMemArea->numRanges);

    goto cleanup;

err:
    while (!vectIsEmpty(&vect))
    {
        NvU64 idx = vectCount(&vect) - 1llu;
        _kbusUnmapAperture_GM107(pGpu, pKernelBus, pVAS, pMemDesc,
            vectAt(&vect, idx)->start);
        vectRemove(&vect, idx);
    }

cleanup:
    vectDestroy(&vect);
    gpumgrSetBcEnabledStatus(pGpu, bBcState);
    kbusUpdateRusdStatistics(pGpu);
    return rmStatus;
}

NV_STATUS
kbusUnmapFbAperture_GM107
(
    OBJGPU     *pGpu,
    KernelBus  *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    MemoryArea memArea,
    NvU32       flags
)
{
    NV_STATUS       rmStatus    = NV_OK;
    NvBool          bBcState    = gpumgrGetBcEnabledStatus(pGpu);
    OBJVASPACE     *pVAS        = kbusGetBar1VASpace_HAL(pGpu, pKernelBus);;
    NvU64           idx;

    NV_ASSERT_OR_RETURN(pVAS != NULL, NV_ERR_INVALID_STATE);

    rmStatus = kbusDecreaseStaticBar1Refcount_HAL(pGpu, pKernelBus, pMemDesc, &memArea);

    //
    // If ok, then static BAR1 code has successfully decreased ref count
    // If not, then it's a dynamic mapping we need to continue unmapping
    //
    if (rmStatus == NV_OK)
    {
        goto done;
    }
    else if (rmStatus != NV_ERR_NOT_SUPPORTED)
    {
        NV_PRINTF(LEVEL_ERROR, "static BAR1 reported error\n");
        goto done;
    }

    // Set BC to enabled if UC flag not passed
    gpumgrSetBcEnabledStatus(pGpu, IsSLIEnabled(pGpu) && ((flags & BUS_MAP_FB_FLAGS_MAP_UNICAST) == 0) &&
        ((flags & BUS_MAP_FB_FLAGS_PRE_INIT) == 0));

    // TODO: investigate whether the tegra wbinvd flush is really necessary, seems only useful for SYSMEM_COH
    memdescFlushCpuCaches(pGpu, pMemDesc);

    rmStatus = NV_OK;


    for (idx = 0; idx < memArea.numRanges; idx++)
    {
        NV_STATUS curStatus = _kbusUnmapAperture_GM107(pGpu, pKernelBus, pVAS, pMemDesc, memArea.pRanges[idx].start & (~RM_PAGE_MASK));
        if (rmStatus == NV_OK)
        {
            rmStatus = curStatus;
        }
    }

    gpumgrSetBcEnabledStatus(pGpu, bBcState);

done:
    kbusUpdateRusdStatistics(pGpu);

    if (!(flags & BUS_MAP_FB_FLAGS_UNMANAGED_MEM_AREA))
    {
        portMemFree(memArea.pRanges);
    }

    return rmStatus;
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
kbusFlushSingle_GM107
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus,
    NvU32        flags
)
{
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

    if ((flags & BUS_FLUSH_VIDEO_MEMORY) && !kbusIsFbFlushDisabled(pKernelBus))
    {
        //
        // Note: sysmembar should be flushing PCIE as one of its steps,
        // but this behavior may change on later chips.
        // It's also overkill here because it also flushes GPU's egress traffic.
        //
        return kbusSendSysmembarSingle_HAL(pGpu, pKernelBus);
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

/*!
 * @brief Properly flush written PDEs, PTEs, or other
 * instance memory data or context buffers. See bug 152868
 *
 * NOTE: Must call kbusFlush BEFORE any calls to busInvalidate
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 * @param[in] flags     NvU32 flags to indicate flush behavior
 *
 */
NV_STATUS
kbusFlush_GM107(OBJGPU *pGpu, KernelBus *pKernelBus, NvU32 flags)
{
    NV_STATUS           status  = NV_OK;

    // Nothing to be done in guest in the paravirtualization case.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return NV_OK;
    }

    // Wait for the flush to flow through
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY);
        pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
        if (kbusFlushSingle_HAL(pGpu, pKernelBus, flags) == NV_ERR_TIMEOUT)
        {
            status = NV_ERR_TIMEOUT;
        }
    SLI_LOOP_END;
    pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    return status;
}

//
// _kbusMapAperture_GM107
// Helper function: Given offset and range, alloc VA address space and update it.
//
NV_STATUS
_kbusMapAperture_GM107
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    OBJVASPACE        *pVAS,
    NvU64              offset,
    NvU64             *pAperOffset,
    NvU64             *pLength,
    NvU32              mapFlags,
    Device            *pDevice
)
{
    NV_STATUS           rmStatus = NV_ERR_GENERIC;
    VirtMemAllocator   *pDma;
    NvU32               dmaFlags = kbusConvertBusMapFlagsToDmaFlags(pKernelBus, pMemDesc, mapFlags);
    MEMORY_DESCRIPTOR  *pTempMemDesc;
    NvU32               swizzId = KMIGMGR_SWIZZID_INVALID;

    pDma  = GPU_GET_DMA(pGpu);

    if (IS_MIG_IN_USE(pGpu))
    {
        if (pDevice != NULL)
        {
            MIG_INSTANCE_REF ref;
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

            NV_ASSERT_OR_RETURN(pDevice != NULL, NV_ERR_INVALID_ARGUMENT);
            NV_ASSERT_OK_OR_RETURN(kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                       pDevice, &ref));
            swizzId = ref.pKernelMIGGpuInstance->swizzId;
        }
        else if (pMemDesc->pHeap != NULL)
        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            KERNEL_MIG_GPU_INSTANCE *pCurrKernelMIGGPUInstance = NULL;
            KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance = NULL;

            FOR_EACH_VALID_GPU_INSTANCE(pGpu, pKernelMIGManager, pCurrKernelMIGGPUInstance)
            {
                if (pCurrKernelMIGGPUInstance->pMemoryPartitionHeap == pMemDesc->pHeap)
                {
                    pKernelMIGGPUInstance = pCurrKernelMIGGPUInstance;
                    break;
                }
            }
            FOR_EACH_VALID_GPU_INSTANCE_END();

            NV_ASSERT_OR_RETURN(pKernelMIGGPUInstance != NULL, NV_ERR_INVALID_STATE);
            swizzId = pKernelMIGGPUInstance->swizzId;
        }
    }

    rmStatus = memdescCreateSubMem(&pTempMemDesc, pMemDesc, pGpu, offset, *pLength);
    if (NV_OK == rmStatus)
    {
        rmStatus = dmaAllocMapping_HAL(pGpu, pDma, pVAS, pTempMemDesc, pAperOffset, dmaFlags, NULL, swizzId);
        memdescFree(pTempMemDesc);
        memdescDestroy(pTempMemDesc);
    }

    return rmStatus;
}

//
// _kbusUnmapAperture_GM107
// Helper function: Given offset and range, free VA address space.
//
NV_STATUS
_kbusUnmapAperture_GM107
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    OBJVASPACE        *pVAS,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU64              aperOffset
)
{
    NV_STATUS           rmStatus = NV_OK;
    VirtMemAllocator   *pDma = GPU_GET_DMA(pGpu);

    rmStatus = dmaFreeMapping_HAL(pGpu, pDma, pVAS, aperOffset, pMemDesc, 0, NULL);

    return rmStatus;
}

NV_STATUS
_kbusInitP2P_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBusUnused
)
{
    NV_STATUS status = NV_OK;
    KernelBus *pLocalKernelBus;
    KernelBus *pRemoteKernelBus;

    NvU32 deviceInstance, gpuMask;
    OBJGPU *pLocalGpu, *pRemoteGpu;
    NvU32 localGpuInstance, remoteGpuInstance;
    NvU32 localPeerIndex, remotePeerIndex, localPeerCount, remotePeerCount;
    NvU32 numSubdevices;

    deviceInstance = gpuGetDeviceInstance(pGpu);
    gpuMask = gpumgrGetDeviceGpuMask(deviceInstance);
    numSubdevices = gpumgrGetSubDeviceCount(gpuMask);

    if ((numSubdevices < 1) || (numSubdevices > P2P_MAX_NUM_PEERS))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Fermi only supports P2P with up to 8 subdevices in SLI configuration.\n");
        return NV_ERR_GENERIC;
    }

    // Link all the GPUs.
    localGpuInstance = 0;
    localPeerIndex = 0;
    localPeerCount = 0;

    while ((pLocalGpu = gpumgrGetNextGpu(gpuMask, &localGpuInstance)) != NULL)
    {
        pLocalKernelBus = GPU_GET_KERNEL_BUS(pLocalGpu);

        remoteGpuInstance = localGpuInstance;
        remotePeerIndex = localPeerIndex + 1;
        remotePeerCount = 0;

        while ((pRemoteGpu = gpumgrGetNextGpu(gpuMask, &remoteGpuInstance)) != NULL)
        {
            NvU32 locPeerId;
            NvU32 remPeerId;

            NV_ASSERT(localPeerIndex != remotePeerIndex);
            NV_ASSERT((localPeerCount < P2P_MAX_NUM_PEERS) &&
                      (remotePeerCount < P2P_MAX_NUM_PEERS));

            pRemoteKernelBus  = GPU_GET_KERNEL_BUS(pRemoteGpu);

            locPeerId = kbusGetPeerIdFromTable_HAL(pLocalGpu, pLocalKernelBus,
                                                  localPeerIndex, remotePeerIndex);
            remPeerId = kbusGetPeerIdFromTable_HAL(pRemoteGpu, pRemoteKernelBus,
                                                  remotePeerIndex, localPeerIndex);

            NV_ASSERT((locPeerId < P2P_MAX_NUM_PEERS) &&
                      (remPeerId < P2P_MAX_NUM_PEERS));

            pLocalKernelBus->p2pPcie.peerNumberMask[pRemoteGpu->gpuInstance] |=
                NVBIT(locPeerId);
            pRemoteKernelBus->p2pPcie.peerNumberMask[pLocalGpu->gpuInstance] |=
                NVBIT(remPeerId);

            pLocalKernelBus->p2pPcie.busPeer[locPeerId].refCount++;
            pLocalKernelBus->p2pPcie.busPeer[locPeerId].remotePeerId = remPeerId;
            pRemoteKernelBus->p2pPcie.busPeer[remPeerId].refCount++;
            pRemoteKernelBus->p2pPcie.busPeer[remPeerId].remotePeerId = locPeerId;

            remotePeerIndex++;
            remotePeerCount++;
        }

        pLocalKernelBus->bP2pInitialized = NV_TRUE;

        localPeerIndex++;
        localPeerCount++;
    }

    return status;
}

NV_STATUS
_kbusDestroyP2P_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NV_STATUS status = NV_OK;

    OBJGPU *pRemoteGpu;
    KernelBus *pRemoteKernelBus;
    NvU32 i;


    // Clear all peer numbers.
    for (i = 0; i < NV_MAX_DEVICES; i++)
    {
        if (pKernelBus->p2pPcie.peerNumberMask[i] != 0)
        {
            NvU32 locPeerId, remPeerId, gpuInst;

            pRemoteGpu = gpumgrGetGpu(i);
            NV_ASSERT_OR_RETURN(pRemoteGpu != NULL, NV_ERR_INVALID_STATE);
            pRemoteKernelBus = GPU_GET_KERNEL_BUS(pRemoteGpu);
            locPeerId = kbusGetPeerId_HAL(pGpu, pKernelBus, pRemoteGpu);
            remPeerId = kbusGetPeerId_HAL(pRemoteGpu, pRemoteKernelBus, pGpu);

            NV_ASSERT_OR_RETURN(locPeerId < P2P_MAX_NUM_PEERS,
                              NV_ERR_INVALID_STATE);
            NV_ASSERT_OR_RETURN(remPeerId < P2P_MAX_NUM_PEERS,
                              NV_ERR_INVALID_STATE);
            NV_ASSERT_OR_RETURN(pRemoteKernelBus->p2pPcie.busPeer[remPeerId].remotePeerId == locPeerId,
                              NV_ERR_INVALID_STATE);

            pKernelBus->p2pPcie.busPeer[locPeerId].refCount--;
            pRemoteKernelBus->p2pPcie.busPeer[remPeerId].refCount--;

            gpuInst = gpuGetInstance(pGpu);
            pKernelBus->p2pPcie.peerNumberMask[i] &= ~NVBIT(locPeerId);
            pRemoteKernelBus->p2pPcie.peerNumberMask[gpuInst] &= ~NVBIT(remPeerId);

            // That should have been the only peer ID associated with the remote
            NV_ASSERT(pKernelBus->p2pPcie.peerNumberMask[i] == 0);
            NV_ASSERT(pRemoteKernelBus->p2pPcie.peerNumberMask[gpuInst] == 0);
        }

        // Clear NVlink related data structures as well.
        if (kbusGetNvlinkPeerNumberMask_HAL(pGpu, pKernelBus, i) != 0)
        {
            NvU32 locPeerId, remPeerId, gpuInst;

            pRemoteGpu = gpumgrGetGpu(i);
            NV_ASSERT_OR_RETURN(pRemoteGpu != NULL, NV_ERR_INVALID_STATE);
            pRemoteKernelBus = GPU_GET_KERNEL_BUS(pRemoteGpu);
            locPeerId = kbusGetPeerId_HAL(pGpu, pKernelBus, pRemoteGpu);
            remPeerId = kbusGetPeerId_HAL(pRemoteGpu, pRemoteKernelBus, pGpu);
            gpuInst = gpuGetInstance(pGpu);

            NV_ASSERT_OR_RETURN(locPeerId < P2P_MAX_NUM_PEERS,
                              NV_ERR_INVALID_STATE);
            NV_ASSERT_OR_RETURN(remPeerId < P2P_MAX_NUM_PEERS,
                              NV_ERR_INVALID_STATE);

            pKernelBus->p2p.busNvlinkMappingRefcountPerGpu[i]--;
            pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerGpu[gpuInst]--;
            pKernelBus->p2p.busNvlinkPeerNumberMask[i] &= ~NVBIT(locPeerId);
            pRemoteKernelBus->p2p.busNvlinkPeerNumberMask[gpuInst] &= ~NVBIT(remPeerId);
            pKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[locPeerId]--;
            pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[remPeerId]--;
        }
    }

    for (i = 0; i < P2P_MAX_NUM_PEERS; ++i)
    {
        if (pKernelBus->p2pPcie.busPeer[i].refCount)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "non-zero peer refcount(%d) on GPU 0x%x peer %d\n",
                      pKernelBus->p2pPcie.busPeer[i].refCount, pGpu->gpuInstance, i);
        }
        pKernelBus->p2pPcie.busPeer[i].refCount = 0;
    }

    pKernelBus->bP2pInitialized = NV_FALSE;

    return status;
}


//
// Link P2P for all GPUs
//
void
_kbusLinkP2P_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    OBJGPU     *pRemoteGpu;
    NV_STATUS   status;
    NvU32       i;

    for ( i = 0; i < NV_MAX_DEVICES; ++i)
    {
        if ((pKernelBus->p2pPcie.peerNumberMask[i] != 0) ||
            (kbusGetNvlinkPeerNumberMask_HAL(pGpu, pKernelBus, i) != 0))
        {
            pRemoteGpu = gpumgrGetGpu(i);
            NV_ASSERT(pRemoteGpu != NULL);

            //
            // If there is a loopback mapping pRemoteGpu will return !fullPower
            // since we are currently in the process of resuming it.
            // Therefore, we special case it and restore the mapping anyways.
            //
            if (gpuIsGpuFullPower(pRemoteGpu) ||
                    pRemoteGpu == pGpu)
            {
                KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
                KernelNvlink *pRemoteKernelNvlink = GPU_GET_KERNEL_NVLINK(pRemoteGpu);
                NvU32 locPeerId = kbusGetPeerId_HAL(pGpu, pKernelBus, pRemoteGpu);
                NvU32 remPeerId = kbusGetPeerId_HAL(pRemoteGpu, GPU_GET_KERNEL_BUS(pRemoteGpu), pGpu);

                NV_ASSERT(locPeerId < P2P_MAX_NUM_PEERS);
                NV_ASSERT(remPeerId < P2P_MAX_NUM_PEERS);
                NV_ASSERT(pKernelBus->p2pPcie.busPeer[locPeerId].remotePeerId == remPeerId);

                if ((pKernelNvlink != NULL) && (pRemoteKernelNvlink != NULL) &&
                    (knvlinkGetP2pConnectionStatus(pGpu, pKernelNvlink, pRemoteGpu) == NV_OK))
                {
                    //
                    // These variables should only be updated for RM Managed P2P.
                    // And only once during RmInit, not during resume as while
                    // going to S3/S4, these variables are not cleared.
                    //
                    if (!kbusIsP2pMailboxClientAllocated(pKernelBus) &&
                        !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH))
                    {
                        KernelBus *pRemoteKernelBus = GPU_GET_KERNEL_BUS(pRemoteGpu);

                        pKernelBus->p2p.busNvlinkPeerNumberMask[pRemoteGpu->gpuInstance] |=
                            NVBIT(locPeerId);
                        pRemoteKernelBus->p2p.busNvlinkPeerNumberMask[pGpu->gpuInstance] |=
                            NVBIT(remPeerId);
                        pKernelBus->p2p.busNvlinkMappingRefcountPerGpu[pRemoteGpu->gpuInstance]++;
                        pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerGpu[pGpu->gpuInstance]++;
                        pKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[locPeerId]++;
                        pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[remPeerId]++;
                    }

                    // Train the links to ACTIVE
                    if ((knvlinkTrainP2pLinksToActive(pGpu, pRemoteGpu, pKernelNvlink)) != NV_OK)
                    {
                        NV_ASSERT(0);
                    }

                    // Use NVLINK if available
                    knvlinkSetupPeerMapping_HAL(pGpu, pKernelNvlink, pRemoteGpu, locPeerId);
                    knvlinkSetupPeerMapping_HAL(pRemoteGpu, pRemoteKernelNvlink, pGpu, remPeerId);
                }
                else
                {
                    RM_API *pRmApi;
                    NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS params;

                    //
                    // Fall back to PCIe otherwise
                    // We only expect one PCIE peer ID per remote GPU for SLI
                    //
                    NV_ASSERT(nvPopCount32(pKernelBus->p2pPcie.peerNumberMask[i]) == 1);

                    kbusSetupMailboxes_HAL(pGpu, pKernelBus,
                                           pRemoteGpu, GPU_GET_KERNEL_BUS(pRemoteGpu),
                                           locPeerId, remPeerId);
                    kbusSetupMailboxes_HAL(pRemoteGpu, GPU_GET_KERNEL_BUS(pRemoteGpu),
                                           pGpu, pKernelBus,
                                           remPeerId, locPeerId);
                    // Program the registers
                    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
                    portMemSet(&params, 0, sizeof(params));
                    params.programPciePeerMask = NVBIT32(locPeerId);
                    status = pRmApi->Control(pRmApi,
                                             pGpu->hInternalClient,
                                             pGpu->hInternalSubdevice,
                                             NV2080_CTRL_CMD_INTERNAL_HSHUB_PEER_CONN_CONFIG,
                                             &params,
                                             sizeof(params));
                    if (status != NV_OK)
                    {
                        NV_PRINTF(LEVEL_ERROR, "Error in programming the local PEER_CONNECTION_CFG registers\n");
                    }
                    pRmApi = GPU_GET_PHYSICAL_RMAPI(pRemoteGpu);
                    portMemSet(&params, 0, sizeof(params));
                    params.programPciePeerMask = NVBIT32(remPeerId);
                    status = pRmApi->Control(pRmApi,
                                             pRemoteGpu->hInternalClient,
                                             pRemoteGpu->hInternalSubdevice,
                                             NV2080_CTRL_CMD_INTERNAL_HSHUB_PEER_CONN_CONFIG,
                                             &params,
                                             sizeof(params));
                    if (status != NV_OK)
                    {
                        NV_PRINTF(LEVEL_ERROR, "Error in programming the remote PEER_CONNECTION_CFG registers\n");
                    }
                }
            }
        }
    }
}

static NV_STATUS
kbusSendMemsysDisableNvlinkPeers
(
    OBJGPU    *pGpu
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
        return NV_OK;

    return pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                           NV2080_CTRL_CMD_INTERNAL_MEMSYS_DISABLE_NVLINK_PEERS,
                           NULL, 0);
}

//
// Unlink P2P for all GPUs
//
void
kbusUnlinkP2P_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    KernelBus *pRemoteKernelBus;
    OBJGPU *pRemoteGpu;
    NvU32 i;

    for ( i = 0; i < NV_MAX_DEVICES; ++i)
    {
        if ((pKernelBus->p2pPcie.peerNumberMask[i] != 0) ||
            (kbusGetNvlinkPeerNumberMask_HAL(pGpu, pKernelBus, i) != 0))
        {
            pRemoteGpu = gpumgrGetGpu(i);
            if (pRemoteGpu == NULL)
            {
                //
                // There is a P2P mapping involving an unloaded GPU
                // Has NV50_P2P been properly freed ?
                //
                NV_PRINTF(LEVEL_ERROR, "There is a P2P mapping involving an unloaded GPU\n");
                continue;
            }

            pRemoteKernelBus = GPU_GET_KERNEL_BUS(pRemoteGpu);

            if (gpuIsGpuFullPower(pRemoteGpu) &&
                kbusIsP2pInitialized(pRemoteKernelBus))
            {
                //
                // NVLINK mappings are static and cannot be torn down, but make
                // sure we tear down any PCIe P2P mappings created.
                //
                if (pKernelBus->p2pPcie.peerNumberMask[i] != 0)
                {
                    NvU32 locPeerId = kbusGetPeerId_HAL(pGpu, pKernelBus, pRemoteGpu);
                    NvU32 remPeerId = kbusGetPeerId_HAL(pRemoteGpu, pRemoteKernelBus, pGpu);

                    // We only expect one PCIE peer ID per remote GPU for SLI
                    NV_ASSERT(nvPopCount32(pKernelBus->p2pPcie.peerNumberMask[i]) == 1);

                    NV_ASSERT(locPeerId < P2P_MAX_NUM_PEERS);
                    NV_ASSERT(remPeerId < P2P_MAX_NUM_PEERS);
                    NV_ASSERT(pKernelBus->p2pPcie.busPeer[locPeerId].remotePeerId == remPeerId);

                    kbusDestroyMailbox(pGpu, pKernelBus, pRemoteGpu, locPeerId);
                    kbusDestroyMailbox(pRemoteGpu, pRemoteKernelBus, pGpu, remPeerId);
                }

                //
                // Instead just disable the NVLINK peers
                //
                NV_ASSERT_OK(kbusSendMemsysDisableNvlinkPeers(pGpu));
                NV_ASSERT_OK(kbusSendMemsysDisableNvlinkPeers(pRemoteGpu));
            }
        }
    }
}

/*!
 * @brief  Calculates the memory needed for allocating BAR2 Page Tables.
 *
 * Size calculation is optimized  for @ref GMMU_FMT_VER_1 due to
 * large % overhead of full Page Table size over the size
 * actually needed for BAR2. UVM replayable fault buffer size is
 * also accomodated in this calculation.
 *
 * @return Size in Bytes, needed for BAR2 Page Tables.
 */
NvU32
kbusGetSizeOfBar2PageTables_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    KernelGmmu          *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    const GMMU_FMT      *pFmt  = NULL;
    NvU64                vaLimit;
    NvU32                numPgTblsCeil;
    NvU32                numPgTblsFloor;
    NvU32                pgTblSize;
    NvU32                numEntries;
    NvU64                vaPerEntry;
    const MMU_FMT_LEVEL *pPgTbl = NULL;
    NvU32                gfid;
    NvU32                cpuVisibleApertureSize = 0;
    NvU32                cpuInisibleApertureSize = 0;

    NV_ASSERT_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK, 0);

    // Return 0 from the guest in the paravirtualization case.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return 0;
    }

    // Get the @ref GMMU_FMT for this chip
    pFmt = kgmmuFmtGet(pKernelGmmu, GMMU_FMT_VERSION_DEFAULT, 0);
    NV_ASSERT_OR_RETURN(NULL != pFmt, 0);

    // Get 4K page size Page Table
    pPgTbl = mmuFmtFindLevelWithPageShift(pFmt->pRoot, RM_PAGE_SHIFT);

    if (pKernelBus->bar2[gfid].cpuVisibleLimit != 0)
        cpuVisibleApertureSize  = pKernelBus->bar2[gfid].cpuVisibleLimit - pKernelBus->bar2[gfid].cpuVisibleBase + 1;
    if (pKernelBus->bar2[gfid].cpuInvisibleLimit != 0)
        cpuInisibleApertureSize = pKernelBus->bar2[gfid].cpuInvisibleLimit - pKernelBus->bar2[gfid].cpuInvisibleBase + 1;

    vaLimit = cpuVisibleApertureSize + cpuInisibleApertureSize;


    numPgTblsCeil  = (NvU32)(NV_CEIL(vaLimit, NVBIT64(pPgTbl->virtAddrBitHi + 1)));
    numPgTblsFloor = (NvU32)vaLimit / NVBIT64(pPgTbl->virtAddrBitHi + 1);

    //
    // Let's optimize the space caculation on GMMU_FMT_VER_1
    // if the Page Table is not fully used.
    //
    if (0 == numPgTblsFloor)
    {
        vaPerEntry = mmuFmtEntryVirtAddrMask(pPgTbl) + 1;
        numEntries = (NvU32)(NV_CEIL(vaLimit, vaPerEntry));
        pgTblSize  = numEntries * pPgTbl->entrySize;
        pKernelBus->bar2[gfid].pageTblSize = pgTblSize;
    }
    else
    {
        pKernelBus->bar2[gfid].pageTblSize = mmuFmtLevelSize(pPgTbl);
        pgTblSize = numPgTblsCeil * pKernelBus->bar2[gfid].pageTblSize;
    }

    pKernelBus->bar2[gfid].numPageTbls = numPgTblsCeil;

    return pgTblSize;
}

void
kbusStateDestroy_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    KernelBif           *pKernelBif     = GPU_GET_KERNEL_BIF(pGpu);
    MemoryManager       *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelGmmu          *pKernelGmmu    = GPU_GET_KERNEL_GMMU(pGpu);
    NvU64                offsetBar0;

    (void)kbusDestroyBar2_HAL(pGpu, pKernelBus, GPU_GFID_PF);

    // Bind the BAR0 window to its default location
    // note: we can't move the window for all intents and purposes since VBIOS
    //       will also use the window at arbitrary locations (eg during an SMI event
    if (pMemoryManager->Ram.fbAddrSpaceSizeMb)
    {
        if (!KBUS_BAR0_PRAMIN_DISABLED(pGpu))
        {
            offsetBar0 = (pMemoryManager->Ram.fbAddrSpaceSizeMb << 20) - DRF_SIZE(NV_PRAMIN);
            (void)kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, offsetBar0);
        }
    }

    // Unmap BAR0 Writecombined Window
    if(pKernelBus->pWriteCombinedBar0Window != NULL)
    {
        osUnmapPciMemoryKernelOld(pGpu, (void*)pKernelBus->pWriteCombinedBar0Window);
        pKernelBus->pWriteCombinedBar0Window = NULL;
        pKernelBus->pDefaultBar0Pointer = pKernelBus->pUncachedBar0Window;
    }

    NV_PRINTF(LEVEL_INFO, "FLA Supported: %x \n", kbusIsFlaSupported(pKernelBus));

    // clean up FLA here
    // if FLA supported & enabled FLA VAS
    if (IS_VIRTUAL(pGpu) && kbusIsFlaSupported(pKernelBus))
    {
        NV_PRINTF(LEVEL_INFO, "Trying to destroy FLA VAS\n");
        kbusDestroyFla_HAL(pGpu, pKernelBus);
    }
    //
    // clean up private info block
    //


    // Call _kbusDestroyP2P_GM107 only in case of Linked SLI and Unlinked SLI. Bug 4182245
    if ((pKernelBif != NULL) && ((!pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_READS_DISABLED) ||
                                  !pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_WRITES_DISABLED)) &&
                                 (kbusIsP2pInitialized(pKernelBus))) &&
                                 (IsSLIEnabled(pGpu) || IsUnlinkedSLIEnabled(pGpu)))
    {
        (void)_kbusDestroyP2P_GM107(pGpu, pKernelBus);
    }

    if (pKernelGmmu->pFakeSparseBuffer != NULL)
    {
        memdescFree(pKernelGmmu->pFakeSparseBuffer);
        memdescDestroy(pKernelGmmu->pFakeSparseBuffer);
        pKernelGmmu->pFakeSparseBuffer = NULL;
    }
}

//
//
// Tests BAR2 against BAR0.
// If memDescIn is NULL, a test mem desc is created and map/unmapped.
// If memDescIn is not NULL and provided, this method assumes that it has
// already been alloc'ed and mapping/unmapping is handled outside
// this method.
//
NV_STATUS
kbusVerifyBar2_GM107
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
    NV_ADDRESS_SPACE  oldAddrSpace     = ADDR_FBMEM;
    NvBool            bIsStandaloneTest;
    const NvU32       SAMPLEDATA       = 0xabcdabcd;
    const NvU32       FBSIZETESTED     = 0x10;
    NvU64             bar0TestAddr     = 0;
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NvU32             flagsClean       = 0;

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
        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_65,
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

    // Log error before starting test
    // ==========================================================
    // Does the BAR0 window work?

    NV_PRINTF_COND(IS_EMULATION(pGpu), LEVEL_NOTICE, LEVEL_INFO, "Testing BAR0 window...\n");

    bar0Window = kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);
    oldAddrSpace = DRF_VAL( _PBUS, _BAR0_WINDOW, _TARGET, GPU_REG_RD32(pGpu, NV_PBUS_BAR0_WINDOW));
    bar0TestAddr = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
    GPU_FLD_WR_DRF_NUM(pGpu, _PBUS, _BAR0_WINDOW, _BASE, NvU64_LO32(bar0TestAddr >> 16));
    GPU_FLD_WR_DRF_NUM(pGpu, _PBUS, _BAR0_WINDOW, _TARGET, testAddrSpace);
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

    NV_PRINTF_COND(IS_EMULATION(pGpu), LEVEL_NOTICE, LEVEL_INFO, "Bar0 window tests successfully\n");
    GPU_REG_WR32(pGpu, DRF_BASE(NV_PRAMIN) + NvU64_LO32(bar0TestAddr & 0xffff), testData);
    GPU_FLD_WR_DRF_NUM(pGpu, _PBUS, _BAR0_WINDOW, _BASE, NvU64_LO32(bar0Window >> 16));
    GPU_FLD_WR_DRF_NUM(pGpu, _PBUS, _BAR0_WINDOW, _TARGET, oldAddrSpace);

    if ((testAddrSpace == NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY) ||
        (testAddrSpace == NV_MMU_PTE_APERTURE_SYSTEM_NON_COHERENT_MEMORY))
    {
        // Flush GPU write before proceeding to next test (otherwise it may stomp over following CPU writes)
        kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_VIDEO_MEMORY);
    }
    // ==========================================================


    // ==========================================================
    // Does MMU's translation logic work?
    NV_PRINTF(LEVEL_INFO,
              "MMUTest Writing test data through virtual BAR2 starting at bar2 offset"
              " (%p - %p) = %p and of size 0x%x\n", (NvU8 *)pOffset,
              (NvU8 *)pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping,
              (NvU8 *)(pOffset - pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping),
              testMemorySize);
    NV_PRINTF_COND(IS_EMULATION(pGpu), LEVEL_NOTICE, LEVEL_INFO,
                   "MMUTest The physical address being targetted is 0x%llx\n",
                   testMemoryOffset);
    for(index = 0; index < testMemorySize; index += 4)
    {
        MEM_WR32( pOffset + index, SAMPLEDATA );
    }
    // Flush the bar2 writes
    // A uflush should not be required since a bar0 window read follows after this
    if ((testAddrSpace == NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY) ||
        (testAddrSpace == NV_MMU_PTE_APERTURE_SYSTEM_NON_COHERENT_MEMORY))
    {
        kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_VIDEO_MEMORY);
    }
    osFlushCpuWriteCombineBuffer();

    status = kmemsysSendL2InvalidateEvict(pGpu, pKernelMemorySystem, flagsClean);
    if (NV_OK != status)
    {
        NV_PRINTF(LEVEL_ERROR, "L2 evict failed\n");
        goto kbusVerifyBar2_failed;
    }

    // Readback through the bar0 window
    bar0Window = kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);
    oldAddrSpace = DRF_VAL( _PBUS, _BAR0_WINDOW, _TARGET, GPU_REG_RD32(pGpu, NV_PBUS_BAR0_WINDOW));
    GPU_FLD_WR_DRF_NUM(pGpu, _PBUS, _BAR0_WINDOW, _BASE, NvU64_LO32(testMemoryOffset >> 16));
    GPU_FLD_WR_DRF_NUM(pGpu, _PBUS, _BAR0_WINDOW, _TARGET, testAddrSpace);

    NV_PRINTF(LEVEL_INFO,
              "bar0Window = 0x%llx, testMemoryOffset = 0x%llx, testAddrSpace = %d, "
              "_PBUS_BAR0_WINDOW = 0x%08x\n", bar0Window, testMemoryOffset,
              testAddrSpace, GPU_REG_RD32(pGpu, NV_PBUS_BAR0_WINDOW));

    temp = (DRF_BASE(NV_PRAMIN) + (NvU32)(testMemoryOffset & 0xffff));
    for(index = 0; index < testMemorySize; index += 4)
    {
        NvU32 bar0WindowData = GPU_REG_RD32(pGpu, temp + index);
        if (bar0WindowData != SAMPLEDATA)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "MMUTest BAR0 window offset 0x%x returned garbage 0x%x\n",
                      temp + index, bar0WindowData);
            NV_PRINTF_COND(IS_EMULATION(pGpu), LEVEL_ERROR, LEVEL_INFO,
                           "Setup a trigger for write<bar0 + 0x1700, 0x40> and in the waves search"
                           " the last few bar2 virtual writes mixed with bar0 window reads\n");
            DBG_BREAKPOINT_REASON(NV_ERR_MEMORY_ERROR);
            status = NV_ERR_MEMORY_ERROR;
            goto kbusVerifyBar2_failed;
        }
        // Write through the BAR0 window to be readback through BAR2 later
        GPU_REG_WR32(pGpu, temp + index, SAMPLEDATA + 0x10);
    }

    GPU_FLD_WR_DRF_NUM(pGpu, _PBUS, _BAR0_WINDOW, _BASE, NvU64_LO32(bar0Window >> 16));
    GPU_FLD_WR_DRF_NUM(pGpu, _PBUS, _BAR0_WINDOW, _TARGET, oldAddrSpace);

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
        NV_PRINTF_COND(IS_EMULATION(pGpu), LEVEL_NOTICE, LEVEL_INFO, "BAR2 virtual test passes\n");

    }

    return status;
}

/*!
 * @brief Inits physical address of Bar1 and Bar2 structures
 *
 * @param[in] KernelBus
 */
NV_STATUS
kbusInitBarsBaseInfo_GM107
(
    KernelBus  *pKernelBus
)
{
    pKernelBus->bar1[GPU_GFID_PF].physAddr   = pKernelBus->pciBars[BUS_BAR_1];
    pKernelBus->bar2[GPU_GFID_PF].physAddr   = pKernelBus->pciBars[BUS_BAR_2];

    return NV_OK;
}

/**
 * @brief Set BAR1/BAR2 virtual aperture size and BAR2 CPU visible limit
 *
 * @param pGpu
 * @param pKernelBus
 * @param gfid
 *
 * @return
 */
NV_STATUS kbusSetBarsApertureSize_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid
)
{
    NvU32 data32;
    NvU32 maxRmAddressibleBar2SizeMb = pKernelBus->bIsEntireBar2RegionVirtuallyAddressible ?
                                       BUS_BAR2_APERTURE_MB : BUS_BAR2_RM_APERTURE_MB;

    //
    // Setup BAR1 aperture size only for GFID_VF
    // GFID_PF is done in StateInit phase
    //
    if (IS_GFID_VF(gfid))
    {
        kbusDetermineBar1ApertureLength(pKernelBus, gfid);
    }

    //
    // Setup BAR2 aperture size
    // Check to see if a BAR2 aperture size override has been specified.
    //
    if (((NV_OK == osReadRegistryDword(pGpu, NV_REG_STR_RM_BAR2_APERTURE_SIZE_MB,
                        &data32))) && data32 && data32 <= maxRmAddressibleBar2SizeMb)
    {
        // Set the BAR2 aperture size based on the override
        pKernelBus->bar2[gfid].rmApertureLimit = (data32 << 20) - 1;
        //
        // This shrinks the bar2 page table and has the side-effect of not
        // configuring the upper part of bar2 used for VESA access (because we
        // only apply override if < BUS_BAR2_RM_APERTURE_MB).
        //
        pKernelBus->bar2[gfid].cpuVisibleLimit  = (data32 << 20) - 1;
    }
    else
    {
        //
        // For simulation mods we limit BAR2 size to decrease PTE init time.
        // Backdoor fmodel/RTL could use the standard settings, but want to
        // keep the code path the same for emulation.  With a 8MB BAR2 we do
        // not expect instance memory to evict a cached mapping.
        //
        if ((IS_SIM_MODS(GPU_GET_OS(pGpu)) && IS_SILICON(pGpu) == 0) || (!RMCFG_FEATURE_MODS_FEATURES && IS_SIMULATION(pGpu)))
        {
            // Temporarily increasing the RM aperture size to 16MB - Bug 3317956
            if (gpuIsCCFeatureEnabled(pGpu))
                pKernelBus->bar2[gfid].rmApertureLimit = (BUS_BAR2_RM_APERTURE_MB << 20) - 1;  // 16MB
            else
                pKernelBus->bar2[gfid].rmApertureLimit = ((BUS_BAR2_RM_APERTURE_MB >> 1) << 20) - 1;  // 8MB
            pKernelBus->bar2[gfid].cpuVisibleLimit = pKernelBus->bar2[gfid].rmApertureLimit;        // No VESA space
        }
        else
        {
            pKernelBus->bar2[gfid].cpuVisibleLimit = (BUS_BAR2_APERTURE_MB << 20) - 1;
            pKernelBus->bar2[gfid].rmApertureLimit = (maxRmAddressibleBar2SizeMb << 20) - 1;
        }
    }

    return NV_OK;
}

/*!
 * @brief Calculates the memory needed for allocating a BAR2 Page Dir for a given VA range
 *
 * @param[in] vaPerEntry  The VA span of one entry within the Page Dir
 *                        whose size is needed.
 * @param[in] entrySize   The size of one PDE within the Page Dir of interest.
 *
 * @return RM_PAGE_SIZE aligned size in Bytes, needed for the BAR2 Page Dir.
 */
static NvU32 _kbusGetSizeOfBar2PageDir_GM107
(
    NvU64                vaBase,
    NvU64                vaLimit,
    NvU64                vaPerEntry,
    NvU32                entrySize
)
{
    NvU32              size;
    NvU32              numEntries;
    NvU64              vaBaseAligned;

    NV_ASSERT_OR_RETURN(0 != entrySize, 0);
    NV_ASSERT_OR_RETURN(0 != vaPerEntry, 0);

    //
    // Calculate number of entries needed within this level to represent
    // the entire BAR2 aperture VA range, then align to 4K
    //
    vaBaseAligned = vaBase & ~(vaPerEntry - 1);
    numEntries = (NvU32)NV_CEIL(vaLimit - vaBaseAligned, vaPerEntry);
    size       = numEntries * entrySize;
    size       = NV_ROUNDUP(size, RM_PAGE_SIZE);

    return size;
}

/*!
 * @brief Calculates the memory needed for allocating BAR2 Page Dirs
 *
 * Size calculation considers all Page Levels defined in @ref GMMU_FMT.
 * Assumes Cpu visible region always starts before the invisible region.
 *
 * @return RM_PAGE_SIZE aligned size in Bytes, needed for all BAR2 Page Dirs.
 */
NvU32 kbusGetSizeOfBar2PageDirs_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    KernelGmmu          *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    const GMMU_FMT      *pFmt   = NULL;
    NvU32                size   = 0;
    const MMU_FMT_LEVEL *pLevel = NULL;
    NvU64                bar2VaLimit = kbusGetVaLimitForBar2_HAL(pGpu, pKernelBus);
    NvU16                i;
    NvU32                gfid;
    NvBool               bContiguous;

    NV_ASSERT_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK, 0);

    // Return 0 from the guest in the paravirtualization case.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return 0;
    }

    // Get the @ref GMMU_FMT for this chip
    pFmt = kgmmuFmtGet(pKernelGmmu, GMMU_FMT_VERSION_DEFAULT, 0);
    NV_ASSERT_OR_RETURN(NULL != pFmt, 0);
    pLevel = pFmt->pRoot;

    // Cache the size of the root Page Dir, once.
    pKernelBus->bar2[gfid].pageDirSize = _kbusGetSizeOfBar2PageDir_GM107(pKernelBus->bar2[gfid].cpuVisibleBase,
                                                                         bar2VaLimit,
                                                                         mmuFmtEntryVirtAddrMask(pLevel) + 1,
                                                                         pLevel->entrySize);

    // Accumulate size for all Page Directories.
    pKernelBus->bar2[gfid].numPageDirs = 0;
    bContiguous = (pKernelBus->bar2[gfid].cpuVisibleLimit + 1 == pKernelBus->bar2[gfid].cpuInvisibleBase) ||
                   pKernelBus->bar2[gfid].cpuInvisibleLimit == 0;

    for (i = 0; (i < GMMU_FMT_MAX_LEVELS - 1); i++)
    {
        NvU32 levelSize = 0;
        NvU64 vaPerEntry = mmuFmtEntryVirtAddrMask(pLevel) + 1;

        if (!bContiguous)
        {
            //
            // Avoid double reserving size for page dir when visible and invisible bar2 share the same page directory
            // In this case we treat them as contiguous.
            //
            if ((pKernelBus->bar2[gfid].cpuVisibleLimit  & ~(vaPerEntry - 1)) ==
                (pKernelBus->bar2[gfid].cpuInvisibleBase & ~(vaPerEntry - 1)))
            {
                levelSize += _kbusGetSizeOfBar2PageDir_GM107(pKernelBus->bar2[gfid].cpuVisibleBase,
                                                             bar2VaLimit,
                                                             vaPerEntry,
                                                             pLevel->entrySize);
            }
            else
            {
                levelSize += _kbusGetSizeOfBar2PageDir_GM107(pKernelBus->bar2[gfid].cpuInvisibleBase,
                                                             pKernelBus->bar2[gfid].cpuInvisibleLimit,
                                                             vaPerEntry,
                                                             pLevel->entrySize);

                levelSize += _kbusGetSizeOfBar2PageDir_GM107(pKernelBus->bar2[gfid].cpuVisibleBase,
                                                             pKernelBus->bar2[gfid].cpuVisibleLimit,
                                                             vaPerEntry,
                                                             pLevel->entrySize);
            }
        }
        else
        {
            levelSize = _kbusGetSizeOfBar2PageDir_GM107(pKernelBus->bar2[gfid].cpuVisibleBase,
                                                        bar2VaLimit,
                                                        vaPerEntry,
                                                        pLevel->entrySize);
        }

        // Get the number of directories we need to initialize from the level size.
        pKernelBus->bar2[gfid].numPageDirs += levelSize >> RM_PAGE_SHIFT;
        size += levelSize;

        // If there's one sublevel choose that.
        if (1 == pLevel->numSubLevels)
        {
            pLevel = &(pLevel->subLevels[0]);
        }
        else
        {
            // Choose the 4K page size sublevel.
            pLevel = &(pLevel->subLevels[1]);
        }
        NV_ASSERT_OR_RETURN(NULL != pLevel, 0);

        // Stop accumulating size if we've exhausted all Page Dirs.
        if (pLevel->bPageTable && (0 == pLevel->numSubLevels))
        {
            break;
        }
    }

    return size;
}

NvU64
kbusGetBAR0WindowAddress_GM107
(
    KernelBus *pKernelBus
)
{
    return NV_PRAMIN_DATA008(0);
}

 /*!
 * @brief Returns the first available peer Id
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns NvU32 first free peer Id
 */
NvU32
kbusGetUnusedPeerId_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NvU32 peerId;

    for (peerId = 0; peerId < pKernelBus->numPeers; peerId++)
    {
        if ((pKernelBus->p2pPcie.busPeer[peerId].refCount == 0) &&
            (!pKernelBus->p2pPcie.busPeer[peerId].bReserved))
        {
            return peerId;
        }
    }

    return BUS_INVALID_PEER;
}

/*!
 * @brief Returns the first available PCIE peer Id
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns NvU32 first free peer Id
 */
NvU32
kbusGetUnusedPciePeerId_GM107
(
    OBJGPU* pGpu,
    KernelBus* pKernelBus
)
{
    return kbusGetUnusedPeerId_HAL(pGpu, pKernelBus);
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
kbusGetPeerId_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    OBJGPU    *pGpuPeer
)
{
    NvU32 gpuPeerInst = gpuGetInstance(pGpuPeer);
    NvU32 peerId;

    if (pKernelBus->p2pPcie.peerNumberMask[gpuPeerInst] == 0)
    {
        return BUS_INVALID_PEER;
    }

    peerId = pKernelBus->p2pPcie.peerNumberMask[gpuPeerInst];
    LOWESTBITIDX_32(peerId);

    return peerId;
}

/*!
 * @brief Returns whether or not the given peerId is valid for the given GPU.
 *
 * @returns NV_OK if the peerId corresponds to an active peer mapping
 *          NV_ERR_INVALID_INDEX otherwise
 */
NV_STATUS
kbusIsPeerIdValid_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      peerId
)
{
    NV_ASSERT_OR_RETURN(peerId < P2P_MAX_NUM_PEERS, NV_ERR_INVALID_INDEX);
    if (pKernelBus->p2pPcie.peerNumberMask[gpuGetInstance(pGpu)] & NVBIT(peerId))
        return NV_OK;
    return NV_ERR_INVALID_INDEX;
}

/*!
* @brief Gets the BAR2 GMMU walker object
*
* @param[in] pKernelBus
*
* @returns MMU_WALK *  Pointer to BAR2 MMU walker
*/
MMU_WALK *
kbusGetBar2GmmuWalker_GM107
(
    KernelBus *pKernelBus
)
{
    OBJGPU*   pGpu = ENG_GET_GPU(pKernelBus);
    NvU32     gfid;

    NV_ASSERT_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK, NULL);

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NULL;
    }
    return pKernelBus->bar2[gfid].pWalk;
}

/*!
* @brief Gets the BAR2 GMMU format descriptor
*
* @param[in] pKernelBus
*
* @returns const GMMU_FMT *   Pointer to BAR2 GMMU format
*/
const GMMU_FMT *
kbusGetBar2GmmuFmt_GM107
(
    KernelBus *pKernelBus
)
{
    OBJGPU*   pGpu = ENG_GET_GPU(pKernelBus);
    NvU32     gfid;

    NV_ASSERT_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK, NULL);

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NULL;
    }
    return pKernelBus->bar2[gfid].pFmt;
}

/*!
 *  brief Returns the peer ID corresponding to the peer indexes
 *        from the peer ID table
 *
 * @param[in]  pGpu
 * @param[in]  pKernelBus
 * @param[in]  locPeerIdx Local peer Index
 * @param[in]  remPeerIdx Remote peer Index
 *
 * return NvU32 peerID from the table using given peer indexes
 */
NvU32
kbusGetPeerIdFromTable_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      locPeerIdx,
    NvU32      remPeerIdx
)
{
    if (locPeerIdx >= P2P_MAX_NUM_PEERS ||
        remPeerIdx >= P2P_MAX_NUM_PEERS)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Peer number table doesn't support >%u GPUs\n",
                  P2P_MAX_NUM_PEERS);

        return BUS_INVALID_PEER;
    }

    return peerNumberTable_GM107[locPeerIdx][remPeerIdx];
}

//
// Description: This function fills in an object array describing
// the offsets to and addresses in the PCI bus.
//
void
kbusInitPciBars_GM107
(
    KernelBus *pKernelBus
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pKernelBus);

    pKernelBus->pciBars[0] = pGpu->busInfo.gpuPhysAddr;
    pKernelBus->pciBars[1] = pGpu->busInfo.gpuPhysFbAddr;
    pKernelBus->pciBars[2] = pGpu->busInfo.gpuPhysInstAddr;

    if (! IsAMODEL(pGpu))
    {
        // Classic dGPUs
        pKernelBus->totalPciBars = BUS_NUM_BARS;
        pKernelBus->pciBars[3] = pGpu->busInfo.gpuPhysIoAddr;
    }
    else
    {
        // AMODEL doesn't have IO BAR
        pKernelBus->totalPciBars = 3;
    }
}

NV_STATUS
kbusSetBAR0WindowVidOffset_GM107
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus,
    NvU64        vidOffset
)
{
    NV_ASSERT( (vidOffset & 0xffff)==0 );
    NV_ASSERT( (vidOffset >> 16) <= DRF_MASK(NV_PBUS_BAR0_WINDOW_BASE) );

    // RM initialises cachedBar0WindowVidOffset with 0. Refresh its value with
    // current NV_PBUS_BAR0_WINDOW_BASE.
    if (pKernelBus->cachedBar0WindowVidOffset == 0)
    {
        pKernelBus->cachedBar0WindowVidOffset = ((NvU64) GPU_REG_RD_DRF(pGpu, _PBUS, _BAR0_WINDOW, _BASE)) << 16;
    }

    // Update only if the new offset is different from the cached value
    if (pKernelBus->cachedBar0WindowVidOffset != vidOffset)
    {
        NV_PRINTF(LEVEL_INFO,
                  "mapping BAR0_WINDOW to VID:%x'%08x\n",
                  NvU64_HI32(vidOffset), NvU64_LO32(vidOffset));

        GPU_FLD_WR_DRF_NUM(pGpu, _PBUS, _BAR0_WINDOW, _BASE, NvU64_LO32(vidOffset >> 16));
        GPU_FLD_WR_DRF_DEF(pGpu, _PBUS, _BAR0_WINDOW, _TARGET, _VID_MEM);

        pKernelBus->cachedBar0WindowVidOffset = vidOffset;
    }

    return (NV_OK);
}

NvU64
kbusGetBAR0WindowVidOffset_GM107
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus
)
{
    NvU64 vidOffset;

    // RM initialises cachedBar0WindowVidOffset with 0. Refresh its value with
    // current NV_PBUS_BAR0_WINDOW_BASE.
    if (pKernelBus->cachedBar0WindowVidOffset == 0)
    {
        pKernelBus->cachedBar0WindowVidOffset = ((NvU64) GPU_REG_RD_DRF(pGpu, _PBUS, _BAR0_WINDOW, _BASE)) << 16;
    }

    vidOffset = pKernelBus->cachedBar0WindowVidOffset;

    return (vidOffset);
}

/*!
 * Mem read/write through the bar0 window.
 *
 * This routine is used to re-direct the bar2 accesses which were mapped as
 * type BUSBARMAP_TYPE_BAR through the bar0 window.
 *
 *  @param[in]       pGpu
 *  @param[in]       pKernelBus
 *  @param[in]       physAddr   - physical address of the accessed memory
 *  @param[in]       accessSize - Size of the data to be read/written
 *  @param[in]       bRead      - Read or Write flag
 *  @param[in]       addrSpace  - aperture of the accessed memory
 *  @returns         NV_STATUS
 */
NV_STATUS
kbusMemAccessBar0Window_GM107
(
    OBJGPU                 *pGpu,
    KernelBus              *pKernelBus,
    NvU64                   physAddr,
    void                   *pData,
    NvU64                   accessSize,
    NvBool                  bRead,
    NV_ADDRESS_SPACE        addrSpace
)
{
    NvU64              bar0WindowOffset;
    NvU64              bar0WindowOrig;
    NvBool             bRestoreWindow = NV_FALSE;

    // The following code assumes aperture to be VID_MEM (or that vidmem/sysmem are same).
    NV_ASSERT(gpuIsUnifiedMemorySpaceEnabled(pGpu) || (addrSpace == ADDR_FBMEM));

    if (KBUS_BAR0_PRAMIN_DISABLED(pGpu))
    {
        NV_ASSERT_FAILED("kbusMemAccessBar0Window_HAL call in coherent path\n");
        return NV_ERR_INVALID_STATE;
    }

    bar0WindowOrig   = kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);
    bar0WindowOffset = physAddr - bar0WindowOrig;

    if (bar0WindowOffset + accessSize > DRF_SIZE(NV_PRAMIN))
    {
        kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, (physAddr & ~0xffff));
        bar0WindowOffset = physAddr - kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);
        bRestoreWindow = NV_TRUE;
    }

    if (bRead)
    {
        // Read access
        switch (accessSize)
        {
            case 1:
                *((NvU8  *)pData) = (NvU8)GPU_REG_RD08(pGpu, NV_PRAMIN_DATA008(bar0WindowOffset));
                break;
            case 2:
                *((NvU16 *)pData) = (NvU16)GPU_REG_RD16(pGpu, NV_PRAMIN_DATA008(bar0WindowOffset));
                break;
            case 4:
                *((NvU32 *)pData) = (NvU32)GPU_REG_RD32(pGpu, NV_PRAMIN_DATA008(bar0WindowOffset));
                break;
            case 8:
                // TO DO: Create GPU_REG_RD64
                *((NvU32 *)pData)     = (NvU32)GPU_REG_RD32(pGpu, NV_PRAMIN_DATA008(bar0WindowOffset));
                *((NvU32 *)pData + 1) = (NvU32)GPU_REG_RD32(pGpu, NV_PRAMIN_DATA008(bar0WindowOffset + 4));
                break;

            default:
                NV_ASSERT(0);
                return NV_ERR_GENERIC;
        }
    }
    else
    {
        // Write access
        switch (accessSize)
        {
            case 1:
                GPU_REG_WR08(pGpu, NV_PRAMIN_DATA008(bar0WindowOffset), (NvU8)(*((NvU8 *)pData) & 0xff));
                break;
            case 2:
                GPU_REG_WR16(pGpu, NV_PRAMIN_DATA008(bar0WindowOffset), (NvU16)(*((NvU16 *)pData) & 0xffff));
                break;
            case 4:
                GPU_REG_WR32(pGpu, NV_PRAMIN_DATA008(bar0WindowOffset), *((NvU32 *)pData));
                break;
            case 8:
                // TO DO: Create GPU_REG_WR64
                GPU_REG_WR32(pGpu, NV_PRAMIN_DATA008(bar0WindowOffset),     *((NvU32 *)pData));
                GPU_REG_WR32(pGpu, NV_PRAMIN_DATA008(bar0WindowOffset + 4), *((NvU32 *)pData + 1));
                break;

            default:
                NV_ASSERT(0);
                return NV_ERR_GENERIC;
        }
    }

    //
    // The Bar0 window will be restored after Bar2 bootstrap
    // so check if we can skip restoring the window to avoid
    // these extra register writes to adjust the WINDOW which may
    // cause a timeout failure on some GA10X fmodel environment tests.
    // By skipping the restore function here we ensure the following
    // Bar2 PT writes have the Bar0 window already set up.
    //
    if (bRestoreWindow && !pKernelBus->bar2[GPU_GFID_PF].bBootstrap)
    {
        NV_ASSERT_OK_OR_RETURN(kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, bar0WindowOrig));
    }

    return NV_OK;
}

/*!
 * @brief Function to determine if the mapping can be direct mapped or BAR mapped
 *
 * @param[in]   pGpu
 * @param[in]   pKernelBus
 * @param[in]   pMemDesc    Memory Descriptor pointer
 * @param[in]   mapFlags    Flags used for mapping
 * @param[in]   bDirectSysMappingAllowed boolean to return the result
 *
 * returns NV_OK, since HW supports reflected mappings
 */
NV_STATUS
kbusIsDirectMappingAllowed_GM107
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32              mapFlags,
    NvBool            *bDirectSysMappingAllowed
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvBool bAllowReflectedMapping = NV_FALSE;
    NvU32  pteKind = memdescGetPteKind(pMemDesc);

    //
    // Bug 2033948: Will remove supporting reflected mapping for Z surfaces in sysmem,
    // as soon as MODS implements Z swizzling. Only for MODS.
    //
    if (pKernelBus->bAllowReflectedMappingAccess &&
        memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_Z, pteKind))
    {
        bAllowReflectedMapping = NV_TRUE;
    }

    *bDirectSysMappingAllowed =
         (!(bAllowReflectedMapping) &&
         (!memdescGetFlag(pMemDesc, MEMDESC_FLAGS_ENCRYPTED))  &&
         (memdescGetGpuCacheAttrib(pMemDesc) != NV_MEMORY_CACHED) &&
         (DRF_VAL(OS33, _FLAGS, _MAPPING, mapFlags) != NVOS33_FLAGS_MAPPING_REFLECTED));

    return NV_OK;
}

/**
 *!
 *  @brief Determine if we should use a direct mapping.
 *
 *  RM tries to pick the most efficient mapping possible.  For frame buffer addresses,
 *  we have no choice, they must be mapped through BAR2.  For system memory we prefer
 *  to use direct mappings on dGPU as reflected transactions can lead the PCIE bus to
 *  deadlock.
 *
 *  The conditions in which we choose to map system memory through BAR2 are:
 *    - Running swap endian and we need BAR2 to do byte swapping
 *    - Allowed by verification BAR2_SYSMEM_ENABLE property
 *    - Memory is GPU cached
 *
 *  Allocated is required for a direct system memory map on some platforms
 *  as extra information is needed to complete the mapping request.
 *  User allocated system memory must be Direct mapped (and NOT reflected mapping).
 *  But, memDesc for user allocated memory may have allocated flag as false.
 *  So, adding check for the same.
 *
 *  RM does not map block linear or compressed buffers.  If those come up
 *  we will have to check for them.
 *
 *  We now allow mapping on Allocated memdescs & as well as submemdescs.
 *  The Parent descriptor check is added to handle some specific cases where
 *  memDesc is not allocated and doesn't have a parent. i.e when GMMU PTEs are
 *  allocated from Reserved Sysmem Heap, we use memdescDescribe() to populate the
 *  PTE memdesc. This happens in WinXP, and needs to be reflected BAR2 mapped.
 *
 *  On Tegra we don't want to go via BAR2 (i.e tunneled via BAR0), since it is
 *  expensive. BUS cache maintenance code will ensure coherency b/w CPU & GPU in
 *  Tegra. We can even have dGPU use this path in future.
 *
 * @param[in]     pGpu
 * @param[in]     pKernelBus
 * @param[in]     pMemDesc           MEMORY_DESCRIPTOR pointer
 * @param[in/out] pbAllowDirectMap   NvBool pointer
 *
 *@returns NV_OK, if supported
 *         NV_ERR_NOT_SUPPORTED, otherwise
 */
NV_STATUS
kbusUseDirectSysmemMap_GM107
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvBool            *pbAllowDirectMap
)
{
    *pbAllowDirectMap = NV_FALSE;

    if((memdescGetAddressSpace(pMemDesc) != ADDR_FBMEM) &&
     (!kbusIsBar2SysmemAccessEnabled(pKernelBus)) &&
     (pMemDesc->Allocated || memdescGetParentDescriptor(pMemDesc) ||
      memdescGetFlag(pMemDesc, MEMDESC_FLAGS_EXT_PAGE_ARRAY_MEM) ||
      memdescGetFlag(pMemDesc, MEMDESC_FLAGS_PEER_IO_MEM)) &&
     ((memdescGetGpuCacheAttrib(pMemDesc) == NV_MEMORY_UNCACHED) || IsTEGRA(pGpu)))
    {
         *pbAllowDirectMap =  NV_TRUE;
    }

    return NV_OK;
}

/*!
 * Update BAR1 instance block VAS state and rebind it to HW.
 */
NV_STATUS
kbusBar1InstBlkVasUpdate_GM107
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus
)
{
    KernelGmmu          *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    MemoryManager       *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    OBJVASPACE          *pBar1VAS = kbusGetBar1VASpace_HAL(pGpu, pKernelBus);
    INST_BLK_INIT_PARAMS params = {0};
    NvU32                gfid;
    NV_STATUS            status = NV_OK;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    // Nothing to be done in the guest in the paravirtualization case.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_OK;
    }

    if (IS_GFID_VF(gfid) &&
        pKernelBus->bar1[gfid].pInstBlkMemDesc == NULL)
    {
        //
        // VF BAR1 instance block cannot by in PF sysmem as the latter
        // is not mapped into VF's IOMMU domain
        //
        NV_ASSERT_OR_RETURN(pKernelBus->InstBlkAperture == ADDR_FBMEM, NV_ERR_INVALID_ARGUMENT);

        if ((status = memdescCreate(&pKernelBus->bar1[gfid].pInstBlkMemDesc,
                                    pGpu,
                                    GF100_BUS_INSTANCEBLOCK_SIZE,
                                    GF100_BUS_INSTANCEBLOCK_SIZE,
                                    NV_TRUE,
                                    pKernelBus->InstBlkAperture,
                                    pKernelBus->InstBlkAttr,
                                    MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE)) != NV_OK)
        {
            NV_ASSERT_OR_RETURN(status == NV_OK, status);
        }

        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_66,
                        pKernelBus->bar1[gfid].pInstBlkMemDesc);
        NV_ASSERT_OR_RETURN(status == NV_OK, status);

        status = memmgrMemDescMemSet(pMemoryManager,
                                     pKernelBus->bar1[gfid].pInstBlkMemDesc,
                                     0,
                                     TRANSFER_FLAGS_NONE);
        NV_ASSERT_OR_RETURN(status == NV_OK, status);

        pKernelBus->bar1[gfid].instBlockBase =
                     memdescGetPhysAddr(pKernelBus->bar1[gfid].pInstBlkMemDesc,
                                    AT_GPU, 0);
    }

    // Initialize the instance block VAS state.
    NV_ASSERT_OK_OR_RETURN(
        kgmmuInstBlkInit(pKernelGmmu, pKernelBus->bar1[gfid].pInstBlkMemDesc, pBar1VAS,
                        FIFO_PDB_IDX_BASE, &params));

    //
    // (Re-)bind instance block so host fetches the new VAS state.
    // Flush to ensure host sees the latest.
    //
    kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_VIDEO_MEMORY);
    kbusSendSysmembar(pGpu, pKernelBus);

    NV_ASSERT_OK_OR_RETURN(kbusBar1InstBlkBind_HAL(pGpu, pKernelBus));

    return NV_OK;
}

/*!
 * Previous bind sequence would just do a sysmembar after a flush.
 * Now the flushes don't guarantee anything for the BIND itself.
 * Just that previous reads/writes are complete.
 * We need to use the BIND_STATUS register now.
 * New procedure:
 * - Write NV_PBUS_BLOCK_(BAR1|BAR2)
 * - Poll NV_PBUS_BIND_STATUS to make sure the BIND completed.
 */
NV_STATUS
kbusBindBar2_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    BAR2_MODE  bar2Mode
)
{
    RMTIMEOUT         timeout;
    NvU32             temp;
    NvU32             value;
    NvU32             instBlkAperture = 0;
    NvU64             instBlkAddr     = 0;
    NV_STATUS         status          = NV_OK;
    NvBool            bIsModePhysical;
    MEMORY_DESCRIPTOR *pMemDesc;

    if (pKernelBus->bar2[GPU_GFID_PF].bBootstrap &&
        (NULL != pKernelBus->bar2[GPU_GFID_PF].pInstBlkMemDescForBootstrap) &&
        kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
    {
        pMemDesc = pKernelBus->bar2[GPU_GFID_PF].pInstBlkMemDescForBootstrap;
    }
    else
    {
        pMemDesc = pKernelBus->bar2[GPU_GFID_PF].pInstBlkMemDesc;
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

    GPU_REG_WR32(pGpu, NV_PBUS_BAR2_BLOCK, value);

    osFlushCpuWriteCombineBuffer();

    // Skip the wait if we are in the reset path (GPU most likely in a bad state)
    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
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
        temp = GPU_REG_RD32(pGpu, NV_PBUS_BIND_STATUS);
        if (FLD_TEST_DRF(_PBUS, _BIND_STATUS, _BAR2_PENDING, _EMPTY, temp) &&
            FLD_TEST_DRF(_PBUS, _BIND_STATUS, _BAR2_OUTSTANDING, _FALSE, temp))
        {
            status = NV_OK;
            break;
        }

        if (NV_ERR_TIMEOUT == status)
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
 * @brief This sets up the instance block for BAR1 and BAR2
 *
 * @param[in] pGpu            OBJGPU pointer
 * @param[in] pKernelBus      KernelBus pointer
 * @param[in] pEngCommon      Engine whose instance mem is being written (Bar1 or Bar2)
 * @param[in] pInstBlkMemDesc NULL if BAR2 aperture not set up yet: BAR2 instance block using BAR0 window
 * @param[in] pPDB            PDB pointer for the instance blocks PDB
 * @param[in] vaLimit         VALimit to set for this instance
 * @param[in] bigPageSize     VASPACE Big page size (relevant for maxwell+)
 *
 * @returns NV_STATUS
*/
NV_STATUS
kbusInitInstBlk_GM107
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    PMEMORY_DESCRIPTOR pInstBlkMemDesc,
    PMEMORY_DESCRIPTOR pPDB,
    NvU64              vaLimit,
    NvU64              bigPageSize,
    OBJVASPACE        *pVAS
)
{
    NvU32              windowOffset = 0;
    NvU8              *pMap;

    if (pInstBlkMemDesc == NULL)
    {
        if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
        {
            return kbusSetupBar2InstBlkAtBottomOfFb_HAL(pGpu, pKernelBus, pPDB, vaLimit, bigPageSize, GPU_GFID_PF);
        }
        // find the offset of the bar ctx block into the BAR0 window
        NvU64 origVidOffset = kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);
        NV_ASSERT_OK_OR_RETURN(kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, pKernelBus->bar2[GPU_GFID_PF].instBlockBase & ~0xffffULL));
        // find the offset of the BAR2 instance block into the BAR0 window
        windowOffset = NvU64_LO32(pKernelBus->bar2[GPU_GFID_PF].instBlockBase - kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus));
        GPU_REG_WR32(pGpu, windowOffset + NV_PRAMIN_DATA008(0) + SF_OFFSET(NV_RAMIN_ADR_LIMIT_LO),
                     NvU64_LO32(vaLimit) | 0xfff);
        GPU_REG_WR32(pGpu, windowOffset + NV_PRAMIN_DATA008(0) + SF_OFFSET(NV_RAMIN_ADR_LIMIT_HI),
                     SF_NUM(_RAMIN_ADR_LIMIT, _HI, NvU64_HI32(vaLimit)));
        GPU_REG_WR32(pGpu, windowOffset + NV_PRAMIN_DATA008(0) + SF_OFFSET(NV_RAMIN_PAGE_DIR_BASE_HI),
                     SF_NUM(_RAMIN_PAGE_DIR_BASE, _HI, NvU64_HI32(memdescGetPhysAddr(pPDB, AT_GPU, 0))));
        GPU_REG_WR32(pGpu, windowOffset + NV_PRAMIN_DATA008(0) + SF_OFFSET(NV_RAMIN_PAGE_DIR_BASE_TARGET),
                     SF_NUM(_RAMIN_PAGE_DIR_BASE, _TARGET, kgmmuGetHwPteApertureFromMemdesc(GPU_GET_KERNEL_GMMU(pGpu), pPDB)) |
                     SF_NUM(_RAMIN_PAGE_DIR_BASE, _VOL, memdescGetVolatility(pPDB)) |
                     SF_NUM(_RAMIN_PAGE_DIR_BASE, _LO, NvU64_LO32(memdescGetPhysAddr(pPDB, AT_GPU, 0) >> PDB_SHIFT_FERMI)));
        NV_ASSERT_OK_OR_RETURN(kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, origVidOffset));
    }
    else
    {
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
        pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
        pMap = kbusMapRmAperture_HAL(pGpu, pInstBlkMemDesc);
        if (pMap == NULL)
        {
            SLI_LOOP_RETURN(NV_ERR_INSUFFICIENT_RESOURCES);
        }
        kbusBar2InstBlkWrite_HAL(pGpu, pKernelBus, pMap, pPDB, vaLimit, bigPageSize);
        kbusUnmapRmAperture_HAL(pGpu, pInstBlkMemDesc, &pMap, NV_TRUE);
        if (pKernelBus->bar2[GPU_GFID_PF].bMigrating)
        {
            //
            // Remove memdesc from cached mappings as the page tables backing
            // this mapping will be discarded and the old VA to PA translation
            // won't be valid.
            //
            kbusReleaseRmAperture_HAL(pGpu, pKernelBus, pInstBlkMemDesc);
        }
        SLI_LOOP_END
        pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    }
    return NV_OK;
}

/*!
 * @brief This sets up the instance block for BAR1 and BAR2
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelBus    KernelBus pointer
 * @param[in] pMap          Opaque pointer in CPU/GPU VASPACE
 * @param[in] pPDB          PDB pointer for BAR2 VAS.
 * @param[in] vaLimit       VALimit to set for this instance
 * @param[in] bigPageSize   VASPACE Big page size (relevant for maxwell+)
 *
 * @returns NV_STATUS
*/
void
kbusBar2InstBlkWrite_GM107
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    NvU8              *pMap,
    PMEMORY_DESCRIPTOR pPDB,
    NvU64              vaLimit,
    NvU64              bigPageSize
)
{
    NV_ASSERT_OR_RETURN_VOID(NULL != pMap);

    MEM_WR32(pMap + SF_OFFSET(NV_RAMIN_ADR_LIMIT_LO), NvU64_LO32(vaLimit) | 0xfff);
    MEM_WR32(pMap + SF_OFFSET(NV_RAMIN_ADR_LIMIT_HI), SF_NUM(_RAMIN_ADR_LIMIT, _HI,
                              NvU64_HI32(vaLimit)));
    MEM_WR32(pMap + SF_OFFSET(NV_RAMIN_PAGE_DIR_BASE_HI),
                    SF_NUM(_RAMIN_PAGE_DIR_BASE, _HI,
                           NvU64_HI32(memdescGetPhysAddr(pPDB, AT_GPU, 0))));
    MEM_WR32(pMap + SF_OFFSET(NV_RAMIN_PAGE_DIR_BASE_TARGET),
                    SF_NUM(_RAMIN_PAGE_DIR_BASE, _TARGET, kgmmuGetHwPteApertureFromMemdesc(GPU_GET_KERNEL_GMMU(pGpu), pPDB)) |
                    SF_NUM(_RAMIN_PAGE_DIR_BASE, _VOL, memdescGetVolatility(pPDB)) |
                    SF_NUM(_RAMIN_PAGE_DIR_BASE, _LO,
                           NvU64_LO32(memdescGetPhysAddr(pPDB, AT_GPU, 0) >> PDB_SHIFT_FERMI)));
}

/*!
 * @brief Sets up starting addresses for BAR2 page directories and tables
 *        within the bottom 32MB of FB for bootstrapping.
 *
 * In physical mode, BAR2 window points to bottom 32MB of FB. CPU
 * mapping is created only for lower 16MB of this window. So, the BAR2
 * level instances should fit within the bottom 16M of FB if we intend to
 * to use CPU mapping for bootstrapping the page level instances.
 *
 * @param[in]  pGpu        OBJGPU pointer
 * @param[in]  pKernelBus  KernelBus pointer
 *
 * @returns NV_OK
 */
NV_STATUS
kbusSetupBar2PageTablesAtBottomOfFb_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid
)
{
    NV_STATUS   status      = NV_OK;

    if ((ADDR_FBMEM == pKernelBus->PDEBAR2Aperture) &&
        (ADDR_FBMEM == pKernelBus->PTEBAR2Aperture))
    {
        KernelGmmu       *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
        NvU64             pageLvlSize;
        NvU64             allocSize;
        NvU64             physAddr;

        NV_ASSERT(pKernelBus->bar2[gfid].bBootstrap);

        pageLvlSize = kbusGetSizeOfBar2PageDirs_HAL(pGpu, pKernelBus) +
                      kbusGetSizeOfBar2PageTables_HAL(pGpu, pKernelBus);

        physAddr = kgmmuGetBigPageSize_HAL(pKernelGmmu) + GF100_BUS_INSTANCEBLOCK_SIZE;

        status = memdescCreate(&pKernelBus->virtualBar2[gfid].pPageLevelsMemDescForBootstrap,
                               pGpu,
                               pageLvlSize,
                               RM_PAGE_SIZE,
                               NV_TRUE,
                               pKernelBus->PDEBAR2Aperture,
                               pKernelBus->PDEBAR2Attr,
                               MEMDESC_FLAGS_NONE);
        NV_ASSERT_OR_RETURN(NV_OK == status, status);

        memdescDescribe(pKernelBus->virtualBar2[gfid].pPageLevelsMemDescForBootstrap,
                        pKernelBus->PDEBAR2Aperture,
                        physAddr,
                        pageLvlSize);

        physAddr = memdescGetPhysAddr(pKernelBus->virtualBar2[gfid].pPageLevelsMemDescForBootstrap,
                                  AT_GPU, 0);
        pKernelBus->bar2[gfid].pdeBaseForBootstrap = physAddr;

        physAddr += kbusGetSizeOfBar2PageDirs_HAL(pGpu, pKernelBus);
        physAddr = NV_ROUNDUP(physAddr, RM_PAGE_SIZE);
        pKernelBus->bar2[gfid].pteBaseForBootstrap = physAddr;

        allocSize = kgmmuGetBigPageSize_HAL(pKernelGmmu) +
                    GF100_BUS_INSTANCEBLOCK_SIZE  +
                    pageLvlSize;

        if (allocSize >= (pKernelBus->bar2[gfid].rmApertureLimit + 1))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Init memory size (0x%x) > BAR2 window mapped to CPU (0x%llx)\n",
                      NvU64_LO32(allocSize), pKernelBus->bar2[gfid].rmApertureLimit + 1);
            memdescDestroy(pKernelBus->virtualBar2[gfid].pPageLevelsMemDescForBootstrap);
            NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_STATE);
        }
    }
    else
    {
        // Ensure that BAR2 page tables and directories are in same aperture.
        NV_ASSERT(pKernelBus->PDEBAR2Aperture == pKernelBus->PTEBAR2Aperture);
    }

    return status;
}

/*!
 * @brief Frees the BAR2 page level instances at the bottom of FB and the
          memory descriptor that was allocated during init to describe the
 *        block of memory containing the page level instances.
 *
 * @param[in]  pGpu        OBJGPU pointer
 * @param[in]  pKernelBus  KernelBus pointer
 *
 * @returns
 */
void
kbusTeardownBar2PageTablesAtBottomOfFb_GM107
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus,
    NvU32 gfid
)
{
    MMU_WALK_USER_CTX userCtx = {0};

    userCtx.pGpu = pGpu;

    if (NULL != pKernelBus->bar2[gfid].pWalkForBootstrap)
    {
        NV_ASSERT_OK(mmuWalkSetUserCtx(pKernelBus->bar2[gfid].pWalkForBootstrap, &userCtx));
        mmuWalkLevelInstancesForceFree(pKernelBus->bar2[gfid].pWalkForBootstrap);
        NV_ASSERT_OK(mmuWalkSetUserCtx(pKernelBus->bar2[gfid].pWalkForBootstrap, NULL));
        mmuWalkDestroy(pKernelBus->bar2[gfid].pWalkForBootstrap);
        pKernelBus->bar2[gfid].pWalkForBootstrap = NULL;

        // Free staging buffer
        memdescFree(pKernelBus->bar2[gfid].pWalkStagingBuffer);
        memdescDestroy(pKernelBus->bar2[gfid].pWalkStagingBuffer);
        pKernelBus->bar2[gfid].pWalkStagingBuffer = NULL;
    }

    memdescDestroy(pKernelBus->virtualBar2[gfid].pPageLevelsMemDescForBootstrap);
    pKernelBus->virtualBar2[gfid].pPageLevelsMemDescForBootstrap = NULL;
}

/*!
 * @brief This sets up a temporary instance block for BAR2 at the bottom of FB
 *        before BAR2 is bootstrapped.
 *
 * @param[in] pGpu         OBJGPU pointer
 * @param[in] pKernelBus   KernelBus pointer
 * @param[in] pPDB         PDB pointer for the instance block's PDB
 * @param[in] vaLimit      VALimit to set for this instance
 * @param[in] bigPageSize  VASPACE Big page size (relevant for maxwell+)
 *
 * @returns NV_STATUS
 */
NV_STATUS
kbusSetupBar2InstBlkAtBottomOfFb_GM107
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    PMEMORY_DESCRIPTOR pPDB,
    NvU64              vaLimit,
    NvU64              bigPageSize,
    NvU32              gfid
)
{
    KernelGmmu       *pKernelGmmu  = GPU_GET_KERNEL_GMMU(pGpu);
    NvU8             *pMap;
    NV_STATUS         status       = NV_OK;

    if (ADDR_FBMEM == pKernelBus->InstBlkAperture)
    {
        NV_ASSERT(pKernelBus->bar2[gfid].bBootstrap);

        if (NULL == pKernelBus->bar2[gfid].pInstBlkMemDescForBootstrap)
        {
            status = memdescCreate(&pKernelBus->bar2[gfid].pInstBlkMemDescForBootstrap,
                                   pGpu,
                                   GF100_BUS_INSTANCEBLOCK_SIZE,
                                   GF100_BUS_INSTANCEBLOCK_SIZE,
                                   NV_TRUE,
                                   pKernelBus->InstBlkAperture,
                                   pKernelBus->InstBlkAttr,
                                   MEMDESC_FLAGS_NONE);
            NV_ASSERT_OR_RETURN(NV_OK == status, status);

            memdescDescribe(pKernelBus->bar2[gfid].pInstBlkMemDescForBootstrap,
                            pKernelBus->InstBlkAperture,
                            kgmmuGetBigPageSize_HAL(pKernelGmmu),
                            GF100_BUS_INSTANCEBLOCK_SIZE);
        }

        pMap = kbusCpuOffsetInBar2WindowGet(pGpu, pKernelBus,
                                            pKernelBus->bar2[gfid].pInstBlkMemDescForBootstrap);
        if (NULL == pMap)
        {
            memdescDestroy(pKernelBus->bar2[gfid].pInstBlkMemDescForBootstrap);
            pKernelBus->bar2[gfid].pInstBlkMemDescForBootstrap = NULL;
            NV_ASSERT_OR_RETURN(NULL != pMap, NV_ERR_INVALID_POINTER);
        }

        // Write the instance block.
        kbusBar2InstBlkWrite_HAL(pGpu, pKernelBus, pMap, pPDB, vaLimit, bigPageSize);
    }

    return status;
}

/*!
 * @brief Frees the temporary instance block after instance block
 *        has moved to top of FB.
 *
 * @param[in]  pGpu        OBJGPU pointer
 * @param[in]  pKernelBus  KernelBus pointer
 *
 * @returns None
 */
void
kbusTeardownBar2InstBlkAtBottomOfFb_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid
)
{
    memdescDestroy(pKernelBus->bar2[gfid].pInstBlkMemDescForBootstrap);
    pKernelBus->bar2[gfid].pInstBlkMemDescForBootstrap = NULL;
}

/*!
 * @brief Sets up the BAR2 page level instances at the top of FB.
 *
 * In physical mode, BAR2 window points to first 32MB of FB. The BAR2
 * window is fixed and cannot be moved around like NV_PRAMIN. So, if we
 * we are to use BAR2 PA mode for bootstrapping the page tables, we have
 * to place the page tables within first 32M of FB. Once the bootstrapping
 * is done, we can use this function to initialize the PTEs at the top of
 * FB.
 *
 * @param[in]  pGpu        OBJGPU pointer
 * @param[in]  pKernelBus  KernelBus pointer
 *
 * @returns NV_OK
 */
NV_STATUS
kbusSetupBar2PageTablesAtTopOfFb_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid
)
{
    MMU_WALK            *pWalk      = NULL;
    const MMU_FMT_LEVEL *pLevelFmt  = NULL;
    MMU_WALK_USER_CTX   userCtx     = {0};
    MMU_WALK_FLAGS      walkFlags   = {0};
    NV_STATUS           status;

    // Do nothing if BAR2 page directories/tables are in sysmem.
    if ((ADDR_FBMEM != pKernelBus->PDEBAR2Aperture) ||
        (ADDR_FBMEM != pKernelBus->PTEBAR2Aperture))
    {
        return NV_OK;
    }

    pKernelBus->bar2[gfid].bMigrating = NV_TRUE;

    // Grab a CPU pointer to the actual BAR2 page tables at the top of FB.
    status = kbusInitVirtualBar2_HAL(pGpu, pKernelBus);
    NV_ASSERT_OR_RETURN(NV_OK == status, status);

    //
    // Initialize/allocate walker staging buffer only if PTEs in FBMEM
    // and we are currently bootstrapping BAR2.
    //
    if (pKernelBus->bar2[gfid].pWalkStagingBuffer == NULL &&
        pKernelBus->PTEBAR2Aperture == ADDR_FBMEM &&
        pKernelBus->bar2[gfid].bBootstrap)
    {
        pKernelBus->bar2[gfid].pWalkStagingBuffer = kbusCreateStagingMemdesc(pGpu);
    }

    // Create a new MMU_WALKER state for the actual page tables.
    status = mmuWalkCreate(pKernelBus->bar2[gfid].pFmt->pRoot,
                           NULL,
                           &g_bar2WalkCallbacks,
                           walkFlags,
                           &pWalk,
                           (struct MMU_WALK_MEMDESC *) pKernelBus->bar2[gfid].pWalkStagingBuffer);
    NV_ASSERT_OR_RETURN(NV_OK == status, status);

    // We want to lock the small page table
    pLevelFmt = mmuFmtFindLevelWithPageShift(pKernelBus->bar2[gfid].pFmt->pRoot,
                                             RM_PAGE_SHIFT);

    // Setup walk user context.
    userCtx.pGpu = pGpu;
    //
    // Pre-reserve and init 4K tables through mappings made using the
    // temporary page tables that were bootstrapped at the bottom of FB.
    //
    NV_ASSERT_OK_OR_GOTO(status, mmuWalkSetUserCtx(pWalk, &userCtx), cleanup);

    if (pKernelBus->bar2[gfid].cpuVisibleLimit != 0)
    {
        status = mmuWalkReserveEntries(pWalk, pLevelFmt, pKernelBus->bar2[gfid].cpuVisibleBase,
                                       pKernelBus->bar2[gfid].cpuVisibleLimit, NV_FALSE);
        NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
        status = mmuWalkSparsify(pWalk, pKernelBus->bar2[gfid].cpuVisibleBase, pKernelBus->bar2[gfid].cpuVisibleLimit, NV_TRUE);
        NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
    }

    if (pKernelBus->bar2[gfid].cpuInvisibleLimit != 0)
    {
        status = mmuWalkReserveEntries(pWalk, pLevelFmt, pKernelBus->bar2[gfid].cpuInvisibleBase,
                                       pKernelBus->bar2[gfid].cpuInvisibleLimit, NV_FALSE);
        NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
        status = mmuWalkSparsify(pWalk, pKernelBus->bar2[gfid].cpuInvisibleBase, pKernelBus->bar2[gfid].cpuInvisibleLimit, NV_TRUE);
        NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
    }

    NV_ASSERT_OK_OR_GOTO(status, mmuWalkSetUserCtx(pWalk, NULL), cleanup);

    NV_PRINTF(LEVEL_INFO, "(BAR2 0x%llx, PDB 0x%llx): vaLimit = 0x%llx\n",
              pKernelBus->bar2[gfid].instBlockBase, pKernelBus->bar2[gfid].pdeBase,
              pKernelBus->bar2[gfid].vaLimit);

    memdescSetPageSize(pKernelBus->bar2[gfid].pPDEMemDesc, AT_GPU,
                       FERMI_SMALL_PAGESIZE);

    // Free the CPU pointer to the temporary BAR2 page tables.
    if (NULL != pKernelBus->virtualBar2[gfid].pPageLevelsForBootstrap)
    {
        kbusUnmapRmAperture_HAL(pGpu,
                                pKernelBus->virtualBar2[gfid].pPageLevelsMemDescForBootstrap,
                                &pKernelBus->virtualBar2[gfid].pPageLevelsForBootstrap,
                                NV_TRUE);
        // Release the memdesc from cached mappings.
        kbusReleaseRmAperture_HAL(pGpu, pKernelBus,
                                  pKernelBus->virtualBar2[gfid].pPageLevelsMemDescForBootstrap);
        pKernelBus->virtualBar2[gfid].pPageLevelsForBootstrap = NULL;
    }

    //
    // Save the MMU walker state that was setup at bootstrap and commit the
    // new walker state.
    //
    pKernelBus->bar2[gfid].pWalkForBootstrap = pKernelBus->bar2[gfid].pWalk;
    pKernelBus->bar2[gfid].pWalk = pWalk;

cleanup:
    pKernelBus->bar2[gfid].bMigrating = NV_FALSE;

    if (NV_OK != status)
    {
        mmuWalkLevelInstancesForceFree(pWalk);
        mmuWalkSetUserCtx(pWalk, NULL);
        mmuWalkDestroy(pWalk);

        // Free staging buffer
        memdescFree(pKernelBus->bar2[gfid].pWalkStagingBuffer);
        memdescDestroy(pKernelBus->bar2[gfid].pWalkStagingBuffer);
        pKernelBus->bar2[gfid].pWalkStagingBuffer = NULL;
    }

    return status;
}

NV_STATUS
kbusCommitBar2PDEs_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NV_STATUS           status        = NV_OK;
    const MMU_FMT_LEVEL *pLevelFmt    = NULL;
    const GMMU_FMT      *pBar2GmmuFmt = kbusGetBar2GmmuFmt_HAL(pKernelBus);
    MMU_WALK_USER_CTX   userCtx       = {0};
    MMU_WALK            *pTempWalk    = NULL;
    NvU32                gfid;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    // Do nothing if page directories/tables in sysmem
    if ((ADDR_FBMEM != pKernelBus->PDEBAR2Aperture) ||
        (ADDR_FBMEM != pKernelBus->PTEBAR2Aperture))
    {
        return NV_OK;
    }

    userCtx.pGpu = pGpu;

    // For bootstrapping, use pWalkForBootstrap as the active MMU_WALK state.
    pTempWalk = pKernelBus->bar2[gfid].pWalk;
    pKernelBus->bar2[gfid].pWalk = pKernelBus->bar2[gfid].pWalkForBootstrap;

    // We want to lock the small page table.
    pLevelFmt = mmuFmtFindLevelWithPageShift(pBar2GmmuFmt->pRoot, RM_PAGE_SHIFT);

    // Start the bootstrap.
    pKernelBus->bar2[gfid].bBootstrap = NV_TRUE;

    // Commit PDEs for page level instances at bottom of FB.
    status = mmuWalkSetUserCtx(pKernelBus->bar2[gfid].pWalkForBootstrap, &userCtx);
    NV_ASSERT_OR_RETURN(NV_OK == status, status);
    status = mmuWalkCommitPDEs(pKernelBus->bar2[gfid].pWalkForBootstrap, pLevelFmt, 0,
                               kbusGetVaLimitForBar2_HAL(pGpu, pKernelBus));
    NV_ASSERT_OR_RETURN(NV_OK == status, status);
    status = mmuWalkSetUserCtx(pKernelBus->bar2[gfid].pWalkForBootstrap, NULL);
    NV_ASSERT_OR_RETURN(NV_OK == status, status);

    // Get a CPU pointer to the page tables at the bottom of FB.
    if (NULL == pKernelBus->virtualBar2[gfid].pPageLevelsForBootstrap)
    {
        status = kbusPreInitVirtualBar2_HAL(pGpu, pKernelBus);
        NV_ASSERT_OR_RETURN(NV_OK == status, status);
    }

    // Bind the temporary instance block at the bottom of FB.
    status = kbusBindBar2_HAL(pGpu, pKernelBus, BAR2_MODE_VIRTUAL);
    NV_ASSERT_OR_RETURN(NV_OK == status, status);

    // End of bootstrap.
    pKernelBus->bar2[gfid].bBootstrap = NV_FALSE;

    // Start the migration.
    pKernelBus->bar2[gfid].bMigrating = NV_TRUE;

    //
    // Get a CPU pointer to the page tables at the top of the FB.
    // The PTEs for this mapping will be written in the page tables
    // at the bottom of FB for this operation. We"ll later mirror
    // these PTEs in the new page tables so that this mapping does
    // not become invalid once we have migrated to the new page
    // tables. See @ref busRewritePTEsForExistingMapping_VBAR2.
    //
    if (NULL == pKernelBus->virtualBar2[gfid].pPageLevels)
    {
        status = kbusInitVirtualBar2_HAL(pGpu, pKernelBus);
        NV_ASSERT_OR_RETURN(NV_OK == status, status);
    }

    // Commit PDEs for page level instances at the top of FB.
    status = mmuWalkSetUserCtx(pTempWalk, &userCtx);
    NV_ASSERT_OR_RETURN(NV_OK == status, status);
    status = mmuWalkCommitPDEs(pTempWalk, pLevelFmt, 0,
                               kbusGetVaLimitForBar2_HAL(pGpu, pKernelBus));
    NV_ASSERT_OR_RETURN(NV_OK == status, status);

    status = mmuWalkSparsify(pTempWalk, 0, kbusGetVaLimitForBar2_HAL(pGpu, pKernelBus), NV_TRUE);
    NV_ASSERT_OR_RETURN(NV_OK == status, status);

    status = mmuWalkSetUserCtx(pTempWalk, NULL);
    NV_ASSERT_OR_RETURN(NV_OK == status, status);

    // Free the CPU pointer to the temporary BAR2 page tables.
    if (NULL != pKernelBus->virtualBar2[gfid].pPageLevelsForBootstrap)
    {
        kbusUnmapRmAperture_HAL(pGpu,
                                pKernelBus->virtualBar2[gfid].pPageLevelsMemDescForBootstrap,
                                &pKernelBus->virtualBar2[gfid].pPageLevelsForBootstrap,
                                NV_TRUE);
        //
        // Release the memdesc from cached mappings as we"ll be discarding the old
        // page tables after migration.
        //
        kbusReleaseRmAperture_HAL(pGpu, pKernelBus,
                                  pKernelBus->virtualBar2[gfid].pPageLevelsMemDescForBootstrap);
        pKernelBus->virtualBar2[gfid].pPageLevelsForBootstrap = NULL;
    }

    // End of migration.
    pKernelBus->bar2[gfid].bMigrating = NV_FALSE;

    // Restore the actual walker.
    pKernelBus->bar2[gfid].pWalk = pTempWalk;

    return status;
}

/*!
 * @brief Commit BAR2
 *
 * 1. Commit BAR2 inst blk in HW.
 * 2. Acquire BAR2 aperture.
 *
 * @param[in] pGpu        OBJGPU pointer
 * @param[in] pKernelBus  KernelBus pointer
 * @param[in] flags       GPU state flag
 *
 * @returns NV_OK on success.
 */
NV_STATUS
kbusCommitBar2_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      flags
)
{
    NvU32 gfid;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    if (!(flags & GPU_STATE_FLAGS_GC6_TRANSITION))
    {

        if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus) &&
            pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc != NULL)
        {
            KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

            //
            // Write the PTEs for the self mapping CPU pointer and the pointer
            // to the page used for bus flushes in the new page tables for the
            // pointers to remain valid after we switch to the new page tables.
            //
            NV_ASSERT_OK_OR_RETURN(kbusRewritePTEsForExistingMapping_HAL(pGpu,
                                   pKernelBus, pKernelBus->virtualBar2[gfid].pPageLevelsMemDesc));

            if (pKernelBus->pFlushMemDesc != NULL)
            {
                NV_ASSERT_OK_OR_RETURN(kbusRewritePTEsForExistingMapping_HAL(pGpu,
                                       pKernelBus, pKernelBus->pFlushMemDesc));
            }

            // Kick out any entries associated w/ old PDB.
            osFlushCpuWriteCombineBuffer();
            kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_VIDEO_MEMORY);
            kgmmuInvalidateTlb_HAL(pGpu, pKernelGmmu,
                                   pKernelBus->virtualBar2[gfid].pPDB,
                                   pKernelBus->virtualBar2[gfid].flags,
                                   PTE_DOWNGRADE, 0, NV_GMMU_INVAL_SCOPE_NON_LINK_TLBS);
            // Update the PDB pointer just before binding w/ the new page tables.
            pKernelBus->virtualBar2[gfid].pPDB = pKernelBus->bar2[gfid].pPDEMemDesc;
        }
    }

    NV_ASSERT_OK_OR_RETURN(kbusBindBar2_HAL(pGpu, pKernelBus, BAR2_MODE_VIRTUAL));

    if (!(flags & GPU_STATE_FLAGS_GC6_TRANSITION) &&
        !kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus) &&
        !KBUS_BAR0_PRAMIN_DISABLED(pGpu) &&
        !IS_GFID_VF(gfid))
    {
        // we will initialize bar2 to the default big page size of the system
        NV_ASSERT_OK_OR_RETURN(kbusInitVirtualBar2_HAL(pGpu, pKernelBus));
        NV_ASSERT_OK_OR_RETURN(kbusSetupCpuPointerForBusFlush_HAL(pGpu, pKernelBus));
    }

    return NV_OK;
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
kbusBar1InstBlkBind_GM107
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NvU32       target;
    NvU32       temp;
    NvU32       ptr;
    RMTIMEOUT   timeout;
    NV_STATUS   status = NV_OK;

    // Nothing to be done in guest in the paravirtualization case.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return NV_OK;
    }

    switch (kgmmuGetMemAperture(pKernelGmmu, pKernelBus->bar1[GPU_GFID_PF].pInstBlkMemDesc))
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
    ptr = NvU64_LO32(pKernelBus->bar1[GPU_GFID_PF].instBlockBase >> GF100_BUS_INSTANCEBLOCK_SHIFT);

    GPU_REG_WR32(pGpu, NV_PBUS_BAR1_BLOCK,
            DRF_NUM(_PBUS, _BAR1_BLOCK, _TARGET, target) |
            DRF_DEF(_PBUS, _BAR1_BLOCK, _MODE,   _VIRTUAL) |
            DRF_NUM(_PBUS, _BAR1_BLOCK, _PTR,    ptr));

    osFlushCpuWriteCombineBuffer();

    // Skip the wait if we are in the reset path (GPU most likely in a bad state)
    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
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
        temp = GPU_REG_RD32(pGpu, NV_PBUS_BIND_STATUS);
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
kbusGetBar1ResvdVA_GM107
(
    KernelBus *pKernelBus
)
{
    // Using the old BAR1 reserved VA Size 0f 64MB
    return NVBIT64(26); //64MB
}

/*!
 * Performs a physically addressed memory copy between system memory and video
 * memory via BAR0 window
 *
 * Regions must be contiguous chunks.
 *
 * @param[in]  pGpu         OBJGPU pointer
 * @param[in]  pKernelBus   KernelBus pointer
 * @param[in]  pDstMemDesc  Destination memory descriptor
 * @param[in]  dstOffset    Offset into destination
 * @param[in]  pSrcMemDesc  Source memory descriptor
 * @param[in]  srcOffset    Offset into source
 * @param[in]  size         Number of bytes to copy
 *
 * @returns None
 */
NV_STATUS
kbusMemoryCopy_GM107
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pDstMemDesc,
    NvU64              dstOffset,
    MEMORY_DESCRIPTOR *pSrcMemDesc,
    NvU64              srcOffset,
    NvU64              size
)
{
    NV_STATUS status = NV_OK;
    NvU8 *pSrcMem = NULL;
    NvU8 *pDstMem = NULL;
    void *pSrcPriv = NULL;
    void *pDstPriv = NULL;
    NvU64 bytes;
    NvU64 offset = 0;
    NvU64 bar0Window;
    NvU64 bar0Offset = 0;
    NvU64 bar0WindowSize = DRF_SIZE(NV_PRAMIN);
    NvU32 data;
    NvU32 i;
    NvU64 source;
    NvU64 dest;
    NvU64 srcPA;
    NvU8 *pTmp;

    source = memdescGetPhysAddr(pSrcMemDesc, FORCE_VMMU_TRANSLATION(pSrcMemDesc, AT_GPU), srcOffset);
    dest   = memdescGetPhysAddr(pDstMemDesc, FORCE_VMMU_TRANSLATION(pDstMemDesc, AT_GPU), dstOffset);

    if (memdescGetAddressSpace(pSrcMemDesc) == ADDR_SYSMEM)
    {
        status = memdescMapOld(pSrcMemDesc, srcOffset, size, NV_TRUE /* kernel */,
                               NV_PROTECT_READ_WRITE,
                               (void **)&pSrcMem, &pSrcPriv);
        if (status != NV_OK)
        {
            DBG_BREAKPOINT();
            return status;
        }
    }

    if (memdescGetAddressSpace(pDstMemDesc) == ADDR_SYSMEM)
    {
        status = memdescMapOld(pDstMemDesc, dstOffset, size, NV_TRUE /* kernel */,
                               NV_PROTECT_READ_WRITE,
                               (void **)&pDstMem, &pDstPriv);
        if (status != NV_OK)
        {
            if (pSrcMem != NULL)
                memdescUnmapOld(pSrcMemDesc, 1 /* kernel */, 0, pSrcMem, pSrcPriv);
            DBG_BREAKPOINT();
            return status;
        }
    }

    if ((memdescGetAddressSpace(pSrcMemDesc) == ADDR_FBMEM) &&
        (memdescGetAddressSpace(pDstMemDesc) == ADDR_FBMEM))
    {
        NV_ASSERT_OR_RETURN(!portUtilCheckOverlap((const NvU8*)dest, size,
                                                  (const NvU8*)source, size),
                            NV_ERR_NOT_SUPPORTED);
        bar0Window = kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);

        pTmp = (NvU8 *)portMemAllocNonPaged(bar0WindowSize);
        NV_ASSERT_OR_RETURN(pTmp != NULL, NV_ERR_NO_MEMORY);
        while (size != 0)
        {
            srcPA = source + offset;
            bar0Offset = ALIGN_64K_OFFSET(srcPA);
            kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus,
                                            ALIGN_64K(srcPA));
            bytes = NV_MIN(size, (bar0WindowSize - bar0Offset));
            //
            // TODO: we could use kbusMemoryCopyToPtr_HAL here, but we might end up
            // with code duplication just to get `bytes`.
            //
            for (i = 0; i < bytes; i+= sizeof(NvU32))
            {
                data = GPU_REG_RD32(pGpu,
                        (DRF_BASE(NV_PRAMIN) + (NvU32)bar0Offset + i));
                MEM_WR32((pTmp + i), data);
            }
            status = kbusMemoryCopyFromPtr_HAL(pGpu, pKernelBus, pDstMemDesc,
                    dstOffset + offset, pTmp, bytes);
            if (status != NV_OK)
            {
                if (pTmp != NULL)
                    portMemFree(pTmp);
                DBG_BREAKPOINT();
                return status;
            }
            offset += bytes;
            size -= bytes;
        }
        portMemFree(pTmp);

        //
        // Make sure the writes make it to FB by sending a read after the writes.
        // We use PRAMIN window readback because some versions of the flush HAL
        // will attempt a VBAR2 readback, and kbusMemoryCopy is designed to be usable
        // even before we have VBAR2 set up.
        //
        kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_VIDEO_MEMORY_VIA_PRAMIN_WINDOW);

        // Reset the window to the previous state.
        kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, bar0Window);
    }
    else if ((memdescGetAddressSpace(pSrcMemDesc) == ADDR_SYSMEM) &&
             (memdescGetAddressSpace(pDstMemDesc) == ADDR_SYSMEM))
    {
        portMemCopy(pDstMem, (NvU32)size, pSrcMem, (NvU32)size);
    }
    else
    {
        bar0Window = kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);

        while (size != 0)
        {
            if (memdescGetAddressSpace(pSrcMemDesc) == ADDR_FBMEM)
            {
                NvU64 sourcePA = source + offset;
                bar0Offset = ALIGN_64K_OFFSET(sourcePA);
                kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus,
                                                ALIGN_64K(sourcePA));
            }

            if (memdescGetAddressSpace(pDstMemDesc) == ADDR_FBMEM)
            {
                NvU64 destPA = dest + offset;
                bar0Offset = ALIGN_64K_OFFSET(destPA);
                kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus,
                                                ALIGN_64K(destPA));
            }

            bytes = NV_MIN(size, (bar0WindowSize - bar0Offset));

            for (i = 0; i < bytes; i += sizeof(NvU32))
            {
                if (memdescGetAddressSpace(pSrcMemDesc) == ADDR_SYSMEM)
                {
                    data = MEM_RD32(pSrcMem + offset + i);
                }
                else
                {
                    data = GPU_REG_RD32(pGpu,
                        (DRF_BASE(NV_PRAMIN) + (NvU32)bar0Offset + i));
                }

                if (memdescGetAddressSpace(pDstMemDesc) == ADDR_SYSMEM)
                {
                    MEM_WR32((pDstMem + offset + i), data);
                }
                else
                {
                    GPU_REG_WR32(pGpu,
                        (DRF_BASE(NV_PRAMIN) + (NvU32)bar0Offset + i),
                        data);
                }
            }

            offset += bytes;
            size -= bytes;
        }

        if (memdescGetAddressSpace(pDstMemDesc) == ADDR_FBMEM)
        {
            //
            // Make sure the writes make it to FB by sending a read after the writes.
            // We use PRAMIN window readback because some versions of the flush HAL
            // will attempt a VBAR2 readback, and kbusMemoryCopy is designed to be usable
            // even before we have VBAR2 set up.
            //
            kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_VIDEO_MEMORY_VIA_PRAMIN_WINDOW);
        }

        // Reset the window to the previous state.
        kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, bar0Window);
    }

    if (pSrcMem != NULL)
        memdescUnmapOld(pSrcMemDesc, 1 /* kernel */, 0, pSrcMem, pSrcPriv);
    if (pDstMem != NULL)
        memdescUnmapOld(pDstMemDesc, 1 /* kernel */, 0, pDstMem, pDstPriv);

    return NV_OK;
}

/*!
 * Performs a physically addressed memory copy between system memory and video
 * memory via BAR0 window
 *
 * Regions must be contiguous chunks.
 *
 * @param[in]  pGpu         OBJGPU pointer
 * @param[in]  pKernelBus   KernelBus pointer
 * @param[in]  pDstMemDesc  Destination memory descriptor
 * @param[in]  dstOffset    Offset into destination
 * @param[in]  pSrc         Source VA
 * @param[in]  size         Number of bytes to copy
 *
 * @returns None
 */
NV_STATUS
kbusMemoryCopyFromPtr_GM107
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pDstMemDesc,
    NvU64              dstOffset,
    NvU8              *pSrcMem,
    NvU64              size
)
{
    NV_STATUS status = NV_OK;
    NvU8 *pDstMem = NULL;
    void *pDstPriv = NULL;
    NvU64 bytes;
    NvU64 offset = 0;
    NvU64 bar0Window;
    NvU64 bar0Offset = 0;
    NvU64 bar0WindowSize = DRF_SIZE(NV_PRAMIN);
    NvU32 data;
    NvU32 i;
    NvU64 dest;


    if (memdescGetAddressSpace(pDstMemDesc) == ADDR_SYSMEM)
    {
        status = memdescMapOld(pDstMemDesc, dstOffset, size, NV_TRUE /* kernel */,
                               NV_PROTECT_READ_WRITE,
                               (void **)&pDstMem, &pDstPriv);
        if (status != NV_OK)
        {
            DBG_BREAKPOINT();
            return status;
        }

        portMemCopy(pDstMem, size, pSrcMem, size);

        memdescUnmapOld(pDstMemDesc, 1 /* kernel */, 0, pDstMem, pDstPriv);
    }
    else
    {
        dest = memdescGetPhysAddr(pDstMemDesc, FORCE_VMMU_TRANSLATION(pDstMemDesc, AT_GPU), dstOffset);

        bar0Window = kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);

        while (size != 0)
        {
            if (memdescGetAddressSpace(pDstMemDesc) == ADDR_FBMEM)
            {
                NvU64 destPA = dest + offset;
                bar0Offset = ALIGN_64K_OFFSET(destPA);
                kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus,
                                                ALIGN_64K(destPA));
            }

            bytes = NV_MIN(size, (bar0WindowSize - bar0Offset));

            for (i = 0; i < bytes; i += sizeof(NvU32))
            {
                data = MEM_RD32(pSrcMem + offset + i);

                GPU_REG_WR32(pGpu,
                    (DRF_BASE(NV_PRAMIN) + (NvU32)bar0Offset + i),
                    data);
            }

            offset += bytes;
            size -= bytes;
        }

        //
        // Make sure the writes make it to FB by sending a read after the writes.
        // We use PRAMIN window readback because some versions of the flush HAL
        // will attempt a VBAR2 readback, and kbusMemoryCopy is designed to be usable
        // even before we have VBAR2 set up.
        //
        kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_VIDEO_MEMORY_VIA_PRAMIN_WINDOW);

        // Reset the window to the previous state.
        kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, bar0Window);
    }

    return NV_OK;
}

/*!
 * Performs a physically addressed memory copy between system memory and video
 * memory via BAR0 window
 *
 * Regions must be contiguous chunks.
 *
 * @param[in]  pGpu         OBJGPU pointer
 * @param[in]  pKernelBus   KernelBus pointer
 * @param[in]  pDst         Destination VA
 * @param[in]  pSrcMemDesc  Src memory descriptor
 * @param[in]  srcOffset    Offset into source
 * @param[in]  size         Number of bytes to copy
 *
 * @returns None
 */
NV_STATUS
kbusMemoryCopyToPtr_GM107
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    NvU8              *pDstMem,
    MEMORY_DESCRIPTOR *pSrcMemDesc,
    NvU64              srcOffset,
    NvU64              size
)
{
    NV_STATUS status = NV_OK;
    NvU8 *pSrcMem = NULL;
    void *pSrcPriv = NULL;
    NvU64 bytes;
    NvU64 offset = 0;
    NvU64 bar0Window;
    NvU64 bar0Offset = 0;
    NvU64 bar0WindowSize = DRF_SIZE(NV_PRAMIN);
    NvU32 data;
    NvU32 i;
    NvU64 source;

    if (memdescGetAddressSpace(pSrcMemDesc) == ADDR_SYSMEM)
    {
        status = memdescMapOld(pSrcMemDesc, srcOffset, size, NV_TRUE /* kernel */,
                               NV_PROTECT_READ_WRITE,
                               (void **)&pSrcMem, &pSrcPriv);
        if (status != NV_OK)
        {
            DBG_BREAKPOINT();
            return status;
        }

        portMemCopy(pDstMem, size, pSrcMem, size);

        memdescUnmapOld(pSrcMemDesc, 1 /* kernel */, 0, pSrcMem, pSrcPriv);
    }
    else
    {
        source = memdescGetPhysAddr(pSrcMemDesc, FORCE_VMMU_TRANSLATION(pSrcMemDesc, AT_GPU), srcOffset);

        bar0Window = kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);

        while (size != 0)
        {
            if (memdescGetAddressSpace(pSrcMemDesc) == ADDR_FBMEM)
            {
                NvU64 sourcePA = source + offset;
                bar0Offset = ALIGN_64K_OFFSET(sourcePA);
                kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus,
                                                ALIGN_64K(sourcePA));
            }

            bytes = NV_MIN(size, (bar0WindowSize - bar0Offset));

            for (i = 0; i < bytes; i += sizeof(NvU32))
            {
                data = GPU_REG_RD32(pGpu,
                    (DRF_BASE(NV_PRAMIN) + (NvU32)bar0Offset + i));

                MEM_WR32((pDstMem + offset + i), data);
            }

            offset += bytes;
            size -= bytes;
        }

        // Reset the window to the previous state.
        kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, bar0Window);
    }

    return NV_OK;
}

/*!
 * @brief  Disable P2P operations to get ready for a XVE sw reset.
 *
 * @param[in]  pGpu          GPU object pointer
 * @param[in]  pKernelBus    KernelBus object pointer
 *
 * @return  NV_OK
 */
NV_STATUS
kbusPrepareForXVEReset_GM107
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus
)
{
    NV_STATUS  status     = NV_OK;
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    if ((!pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_READS_DISABLED) ||
         !pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_WRITES_DISABLED))
        &&
        !kbusIsP2pMailboxClientAllocated(pKernelBus))
    {
        kbusUnlinkP2P_HAL(pGpu, pKernelBus);
    }

    return status;
}

NV_STATUS kbusGetEffectiveAddressSpace_GM107(OBJGPU *pGpu,
                                             MEMORY_DESCRIPTOR *pMemDesc, NvU32 mapFlags,
                                             NV_ADDRESS_SPACE *pAddrSpace)
{
    NV_ADDRESS_SPACE addrSpace;
    NvBool bDirectSysMappingAllowed = NV_TRUE;
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    NV_ASSERT_OR_RETURN(pKernelBus != NULL, NV_ERR_INVALID_STATE);
    NV_CHECK_OK_OR_RETURN(LEVEL_INFO, kbusIsDirectMappingAllowed_HAL(pGpu, pKernelBus, pMemDesc, mapFlags, &bDirectSysMappingAllowed));

    //
    // Bug 1482818: Deprecate reflected mappings in production code.
    //  The usage of reflected writes, in addition to causing several deadlock
    //  scenarios involving P2P transfers, are disallowed on NVLINK (along with
    //  reflected reads), and should no longer be used.
    //  The below PDB property should be unset once the remaining usages in MODS
    //  have been culled. (Bug 1780557)
    //
    if ((memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM) &&
        !bDirectSysMappingAllowed &&
        (DRF_VAL(OS33, _FLAGS, _MAPPING, mapFlags) != NVOS33_FLAGS_MAPPING_DIRECT) &&
        !kbusIsReflectedMappingAccessAllowed(pKernelBus))
    {
        NV_ASSERT(0);
        return NV_ERR_NOT_SUPPORTED;
    }

    if (memdescGetFlag(pMemDesc, MEMDESC_FLAGS_MAP_SYSCOH_OVER_BAR1))
    {
        addrSpace = ADDR_FBMEM;
    }
    else if ((memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM) &&
        (bDirectSysMappingAllowed || FLD_TEST_DRF(OS33, _FLAGS, _MAPPING, _DIRECT, mapFlags) ||
        (IS_VIRTUAL_WITH_SRIOV(pGpu) && !IS_FMODEL(pGpu) && !IS_RTLSIM(pGpu))))
    {
        addrSpace = ADDR_SYSMEM;
    }
    else if ((memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM) ||
             ((memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM) && !bDirectSysMappingAllowed))
    {
        addrSpace = ADDR_FBMEM;
    }
    else
    {
        addrSpace = memdescGetAddressSpace(pMemDesc);
    }

    if (pAddrSpace != NULL)
        *pAddrSpace = addrSpace;

    return NV_OK;
}
