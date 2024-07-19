/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu_user_shared_data.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/heap.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/mem_utils.h"
#include "gpu/mem_mgr/ce_utils.h"
#include "mem_mgr/video_mem.h"
#include "gpu/mem_mgr/fbsr.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/bif/kernel_bif.h"
#include "core/locks.h"
#include "vgpu/vgpu_util.h"
#include "virtualization/kernel_vgpu_mgr.h"
#include "vgpu/rpc.h"
#include "core/thread_state.h"
#include "nvrm_registry.h"
#include "gpu/fsp/kern_fsp.h"
#include "gpu/pmu/kern_pmu.h"
#include "gpu/mem_mgr/phys_mem_allocator/numa.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/rmapi/rs_utils.h"
#include "rmapi/rmapi_utils.h"
#include "mmu/gmmu_fmt.h"
#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER
#include "class/cl503c.h"
#include "class/cl906f.h" // GF100_CHANNEL_GPFIFO
#include "os/os.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/conf_compute/conf_compute.h"
#include "platform/sli/sli.h"
#include "virtualization/hypervisor/hypervisor.h"

#include "class/cl0050.h"
#include "containers/eheap_old.h"

static NV_STATUS _memmgrCreateFBSR(MemoryManager *pMemoryManager, NvU32);
static NV_STATUS _memmgrCreateChildObjects(MemoryManager *pMemoryManager);
static void _memmgrInitRegistryOverridesAtConstruct(OBJGPU *pGpu, MemoryManager *pMemoryManager);
static void _memmgrInitRegistryOverrides(OBJGPU *pGpu, MemoryManager *pMemoryManager);
static NV_STATUS _memmgrInitMIGMemoryPartitionHeap(OBJGPU *pGpu, MemoryManager *pMemoryManager,
                                                   NvU32 swizzId, NV_RANGE *pAddrRange,
                                                   Heap **ppMemoryPartitionHeap);
static NV_STATUS _memmgrAllocInternalClientObjects(OBJGPU *pGpu,
                                                   MemoryManager *pMemoryManager);
static void _memmgrFreeInternalClientObjects(MemoryManager *pMemoryManager);
static void _memmgrInitRUSDHeapSize(OBJGPU *pGpu, MemoryManager *pMemoryManager);

#define MEMUTILS_CHANNEL_GPFIFO_SIZE                  (NV906F_GP_ENTRY__SIZE * MEMUTILS_NUM_GPFIFIO_ENTRIES)

NV_STATUS
memmgrConstructEngine_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    ENGDESCRIPTOR  engDesc
)
{
    NV_STATUS rmStatus;

    pMemoryManager->overrideInitHeapMin = 0;
    pMemoryManager->overrideHeapMax     = ~0ULL;
    pMemoryManager->Ram.fbOverrideSizeMb = ~0ULL;

    // Create the children
    rmStatus = _memmgrCreateChildObjects(pMemoryManager);
    if (rmStatus != NV_OK)
        return rmStatus;

    pMemoryManager->MIGMemoryPartitioningInfo.hClient = NV01_NULL_OBJECT;
    pMemoryManager->MIGMemoryPartitioningInfo.hDevice = NV01_NULL_OBJECT;
    pMemoryManager->MIGMemoryPartitioningInfo.hSubdevice = NV01_NULL_OBJECT;
    pMemoryManager->MIGMemoryPartitioningInfo.partitionableMemoryRange = NV_RANGE_EMPTY;

    _memmgrInitRegistryOverridesAtConstruct(pGpu, pMemoryManager);

    return NV_OK;
}

void
memmgrDestruct_IMPL
(
    MemoryManager *pMemoryManager
)
{
    NvU32 i;

    for (i = 0; i < NUM_FBSR_TYPES; i++)
    {
        objDelete(pMemoryManager->pFbsr[i]);
        pMemoryManager->pFbsr[i] = NULL;
    }

    objDelete(pMemoryManager->pHeap);
    pMemoryManager->pHeap = NULL;

    pMemoryManager->MIGMemoryPartitioningInfo.partitionableMemoryRange = NV_RANGE_EMPTY;
}

/*!
 * @brief   Initializes registry overrides in @ref MemoryManager that need to be
 *          ready by the end of @ref memmgrConstructEngine_IMPL.
 *
 * @param[in]       pGpu
 * @param[in,out]   pMemoryManager
 */
static void
_memmgrInitRegistryOverridesAtConstruct
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
}

static void
_memmgrInitRegistryOverrides(OBJGPU *pGpu, MemoryManager *pMemoryManager)
{
    NvU32 data32;

    // Check for ram size override.
    if ((osReadRegistryDword(pGpu, NV_REG_STR_OVERRIDE_FB_SIZE, &data32) == NV_OK) &&
        (data32 != 0))
    {
        NV_PRINTF(LEVEL_WARNING, "Regkey %s = %dM\n",
                  NV_REG_STR_OVERRIDE_FB_SIZE, data32);
        // Used to override heap sizing at create
        pMemoryManager->Ram.fbOverrideSizeMb = data32;
    }
    else
    {
        pMemoryManager->Ram.fbOverrideSizeMb = ~0ULL;
    }

    //
    // Scrub on Free is enabled by default for GK110+
    // The reg key will be used to disable the scrub on free
    //
    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_DISABLE_SCRUB_ON_FREE,
                             &data32) == NV_OK) && data32)
    {
        pMemoryManager->bScrubOnFreeEnabled = NV_FALSE;
    }

    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_DISABLE_FAST_SCRUBBER,
                             &data32) == NV_OK) && data32)
    {
        pMemoryManager->bFastScrubberEnabled = NV_FALSE;
    }

    if (NV_OK == osReadRegistryDword(pGpu, NV_REG_STR_RM_SYSMEM_PAGE_SIZE, &data32))
    {
        switch (data32)
        {
            case RM_PAGE_SIZE:
            case RM_PAGE_SIZE_64K:
            case RM_PAGE_SIZE_HUGE:
            case RM_PAGE_SIZE_512M:
                break;
            default:
                NV_ASSERT(0);
                NV_PRINTF(LEVEL_ERROR,
                          "Sysmem page size 0x%x not supported! Defaulting to 4KB\n",
                          data32);
                data32 = RM_PAGE_SIZE;
        }
        pMemoryManager->sysmemPageSize = data32;
    }
    else
    {
        pMemoryManager->sysmemPageSize = RM_PAGE_SIZE;

    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_ALLOW_SYSMEM_LARGE_PAGES, &data32) == NV_OK)
    {
        pMemoryManager->bAllowSysmemHugePages = data32 ? NV_TRUE : NV_FALSE;
    }
    else
    {
        pMemoryManager->bAllowSysmemHugePages = NV_FALSE;
    }

    // This key should not be used on physical (GSP) RM.
    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        // Allow user to increase size of RM reserved heap via a regkey
        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_INCREASE_RSVD_MEMORY_SIZE_MB,
                                &data32) == NV_OK)
        {
            pMemoryManager->rsvdMemorySizeIncrement = (NvU64)data32 << 20;
            NV_PRINTF(LEVEL_ERROR,
                      "User specified increase in reserved size = %d MBs\n",
                      data32);
        }
    }

    if (osReadRegistryDword(pGpu,
                            NV_REG_STR_RM_DISABLE_NONCONTIGUOUS_ALLOCATION,
                            &data32) == NV_OK)
    {
        if (data32 == NV_REG_STR_RM_DISABLE_NONCONTIGUOUS_ALLOCATION_TRUE)
        {
            pMemoryManager->bAllowNoncontiguousAllocation = NV_FALSE;
        }
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_FBSR_PAGED_DMA, &data32) == NV_OK)
    {
        pMemoryManager->bEnableFbsrPagedDma = !!data32;
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_FBSR_FILE_MODE, &data32) == NV_OK)
    {
        if (data32 && RMCFG_FEATURE_PLATFORM_UNIX)
        {
            pMemoryManager->bEnableFbsrFileMode = NV_TRUE;
        }
    }

    //
    // Override PMA enable.  PDB_PROP_FB_PMA_ENABLED is reconciled with
    // PDB_PROP_FB_PLATFORM_PMA_SUPPORT to decide whether to enable PMA.
    //
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_ENABLE_PMA, &data32) == NV_OK)
    {
        if (data32 == NV_REG_STR_RM_ENABLE_PMA_YES)
        {
            pMemoryManager->bPmaEnabled = NV_TRUE;
        }
        else
        {
            pMemoryManager->bPmaEnabled = NV_FALSE;
        }
    }

    if (RMCFG_FEATURE_PLATFORM_WINDOWS && !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_TCC_MODE))
    {
        pMemoryManager->bFbsrWddmModeEnabled = NV_TRUE;
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_FBSR_WDDM_MODE, &data32) == NV_OK)
    {
        pMemoryManager->bFbsrWddmModeEnabled = !!data32;
    }

    //
    // Override PMA managed client page tables.
    // NOTE: This is WAR for bug #s 1946145 and 1971628.
    // This should be removed as part of heap removal and PMA refactor.
    //
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_ENABLE_PMA_MANAGED_PTABLES,
                            &data32) == NV_OK)
    {
        if (data32 == NV_REG_STR_RM_ENABLE_PMA_MANAGED_PTABLES_NO)
        {
            memmgrSetClientPageTablesPmaManaged(pMemoryManager, NV_FALSE);
        }
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_DISABLE_GLOBAL_CE_UTILS, &data32) == NV_OK &&
        data32 == NV_REG_STR_DISABLE_GLOBAL_CE_UTILS_YES)
    {
        pMemoryManager->bDisableGlobalCeUtils = NV_TRUE;
    }

    pMemoryManager->bCePhysicalVidmemAccessNotSupported = gpuIsSelfHosted(pGpu);
}

NV_STATUS
memmgrStatePreInitLocked_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL(pGpu))
    {
        //
        // Temporary hack to get OpenRM working without breaking SLI
        // After fixing CORERM-4078, memmgrInitFbRegions() call should be removed from memsysStateInitLocked()
        // and only left here
        //
        NV_ASSERT_OK_OR_RETURN(memmgrInitFbRegions(pGpu, pMemoryManager));
    }

    // Determine the size of reserved memory
    NV_ASSERT_OK_OR_RETURN(memmgrPreInitReservedMemory_HAL(pGpu, pMemoryManager));

    return NV_OK;
}

static NV_STATUS
memmgrTestCeUtils
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    MEMORY_DESCRIPTOR *pVidMemDesc   = NULL;
    MEMORY_DESCRIPTOR *pSysMemDesc   = NULL;
    TRANSFER_SURFACE   vidSurface    = {0};
    TRANSFER_SURFACE   sysSurface    = {0};
    NvU32              vidmemData    = 0xAABBCCDD;
    NvU32              sysmemData    = 0x11223345;
    NV_STATUS          status;

    NV_ASSERT_OR_RETURN(pMemoryManager->pCeUtils != NULL, NV_ERR_INVALID_STATE);

    if (pMemoryManager->pCeUtils->pLiteKernelChannel != NULL)
    {
        //
        // BUG 4167899: Temporarily skip test in case of lite mode
        // It sometimes fails when called from acrGatherWprInformation_GM200()
        // However, ACR is initialized without issues
        //
        return NV_OK;
    }

    NV_ASSERT_OK_OR_GOTO(status,
        memdescCreate(&pVidMemDesc, pGpu, sizeof vidmemData, RM_PAGE_SIZE, NV_TRUE, ADDR_FBMEM,
                      NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE),
        failed);
    memdescTagAlloc(status,
                    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_19, pVidMemDesc);
    NV_ASSERT_OK_OR_GOTO(status, status, failed);
    vidSurface.pMemDesc = pVidMemDesc;

    NV_ASSERT_OK_OR_GOTO(status,
        memdescCreate(&pSysMemDesc, pGpu, sizeof sysmemData, 0, NV_TRUE,
                      RMCFG_FEATURE_PLATFORM_GSP ? ADDR_FBMEM : ADDR_SYSMEM,
                      NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE),
        failed);
    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_138,
                    pSysMemDesc);
    NV_ASSERT_OK_OR_GOTO(status, status, failed);
    sysSurface.pMemDesc = pSysMemDesc;

    NV_ASSERT_OK_OR_GOTO(status, memmgrMemWrite(pMemoryManager, &vidSurface, &vidmemData, sizeof vidmemData, TRANSFER_FLAGS_NONE),      failed);
    NV_ASSERT_OK_OR_GOTO(status, memmgrMemWrite(pMemoryManager, &sysSurface, &sysmemData, sizeof sysmemData, TRANSFER_FLAGS_NONE),      failed);
    NV_ASSERT_OK_OR_GOTO(status, memmgrMemCopy (pMemoryManager, &sysSurface, &vidSurface, sizeof vidmemData, TRANSFER_FLAGS_PREFER_CE), failed);
    NV_ASSERT_OK_OR_GOTO(status, memmgrMemRead (pMemoryManager, &sysSurface, &sysmemData, sizeof sysmemData, TRANSFER_FLAGS_NONE),      failed);
    NV_ASSERT_TRUE_OR_GOTO(status, sysmemData == vidmemData, NV_ERR_INVALID_STATE, failed);

failed:
    memdescFree(pVidMemDesc);
    memdescDestroy(pVidMemDesc);
    memdescFree(pSysMemDesc);
    memdescDestroy(pSysMemDesc);

    return status;
}

NV_STATUS
memmgrInitInternalChannels_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NV_ASSERT_OK_OR_RETURN(memmgrScrubHandlePostSchedulingEnable_HAL(pGpu, pMemoryManager));

    if (pMemoryManager->bDisableGlobalCeUtils ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB) ||
        gpuIsCacheOnlyModeEnabled(pGpu) ||
        (IS_VIRTUAL(pGpu) && !IS_VIRTUAL_WITH_FULL_SRIOV(pGpu)) ||
        IS_SIMULATION(pGpu) ||
        IsDFPGA(pGpu))
    {
        NV_PRINTF(LEVEL_INFO, "Skipping global CeUtils creation (unsupported platform)\n");

        return NV_OK;
    }

    if (hypervisorIsVgxHyper() || (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && !IS_VIRTUAL(pGpu)) ||
        IS_MIG_ENABLED(pGpu) ||
        gpuIsCCorApmFeatureEnabled(pGpu) ||
        IsSLIEnabled(pGpu) ||
        gpuIsSelfHosted(pGpu) ||
        NVCPU_IS_PPC64LE)
    {
        // BUG 4167899: Temporarily skip CeUtils creation on platforms where it fails
        NV_PRINTF(LEVEL_INFO, "Skipping global CeUtils creation\n");

        return NV_OK;
    }

    NV_PRINTF(LEVEL_INFO, "Initializing global CeUtils instance\n");

    NV_ASSERT_OK_OR_RETURN(memmgrInitCeUtils(pMemoryManager, NV_FALSE));

    return NV_OK;
}

NV_STATUS
memmgrDestroyInternalChannels_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NV_PRINTF(LEVEL_INFO, "Destroying global CeUtils instance\n");

    memmgrDestroyCeUtils(pMemoryManager);

    NV_ASSERT_OK_OR_RETURN(memmgrScrubHandlePreSchedulingDisable_HAL(pGpu, pMemoryManager));

    return NV_OK;
}

static NV_STATUS
memmgrPostSchedulingEnableHandler
(
    OBJGPU *pGpu,
    void   *pUnusedData
)
{
    return memmgrInitInternalChannels(pGpu, GPU_GET_MEMORY_MANAGER(pGpu));
}

static NV_STATUS
memmgrPreSchedulingDisableHandler
(
    OBJGPU *pGpu,
    void   *pUnusedData
)
{
    return memmgrDestroyInternalChannels(pGpu, GPU_GET_MEMORY_MANAGER(pGpu));
}

NV_STATUS
memmgrStateInitLocked_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NV_STATUS status = NV_OK;
    NvU32     i;
    NvBool    bDynamicPageOffliningDisable = NV_FALSE;

    NV_ASSERT_OK_OR_RETURN(memmgrInitReservedMemory_HAL(pGpu, pMemoryManager, pMemoryManager->Ram.fbAddrSpaceSizeMb << 20));

    _memmgrInitRegistryOverrides(pGpu, pMemoryManager);

    //
    // Enable dynamic page blacklisting at this point before we call CreateHeap
    // since it internally calls heapGetBlacklistPages which depends on this property
    //
    if (!bDynamicPageOffliningDisable)
        memmgrEnableDynamicPageOfflining_HAL(pGpu, pMemoryManager);

    memmgrScrubRegistryOverrides_HAL(pGpu, pMemoryManager);

    memmgrScrubInit_HAL(pGpu, pMemoryManager);
    NV_ASSERT_OK_OR_RETURN(kfifoAddSchedulingHandler(pGpu,
                GPU_GET_KERNEL_FIFO(pGpu),
                memmgrPostSchedulingEnableHandler, NULL,
                memmgrPreSchedulingDisableHandler, NULL));

    //
    // Allocate framebuffer heap.  All memory must be allocated from here to keep the world
    // consistent (N.B. the heap size has been reduced by the amount of instance memory).
    //
    status = memmgrCreateHeap(pMemoryManager);
    if (status != NV_OK)
    {
        return status;
    }

    //
    // Just set up the memory pool now (basic init stuff). Actual physical
    // frames are *NOT* added to the pool at this stage.
    //
    status = memmgrPageLevelPoolsCreate(pGpu, pMemoryManager);
    if (status != NV_OK)
    {
        return status;
    }

    // RMCONFIG: only if FBSR engine is enabled
    if (RMCFG_MODULE_FBSR)
    {
        //
        // If a configuration is not supported, do not initialize
        // the corresponding fbsr engine.
        //
        if (pMemoryManager->bFbsrWddmModeEnabled)
        {
            pMemoryManager->fbsrStartMode = FBSR_TYPE_WDDM_FAST_DMA_DEFERRED_NONPAGED;
        }
        else if (pMemoryManager->bEnableFbsrPagedDma)
        {
            pMemoryManager->fbsrStartMode = FBSR_TYPE_PAGED_DMA;
        }
        else if (pMemoryManager->bEnableFbsrFileMode)
        {
            pMemoryManager->fbsrStartMode = FBSR_TYPE_FILE;
        }
        else
        {
            pMemoryManager->fbsrStartMode = FBSR_TYPE_PERSISTENT;
        }

        for (i = pMemoryManager->fbsrStartMode; i < NUM_FBSR_TYPES; i++)
        {
            if (!pMemoryManager->bPersistentStandbyBuffer &&
                (i == FBSR_TYPE_PERSISTENT))
            {
                continue;
            }

            if (pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) &&
                (i == FBSR_TYPE_PAGED_DMA || i == FBSR_TYPE_DMA))
            {
                continue;
            }

            status = fbsrInit_HAL(pGpu, pMemoryManager->pFbsr[i]);

            //
            // If one fbsr scheme failed, proceed to initializing the other
            // fallback options.
            //
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_WARNING,
                          "fbsrInit failed for supported type %d suspend-resume scheme\n",
                          i);
                continue;
            }
        }
    }

    status = gpuCreateRusdMemory_HAL(pGpu);
    if (status != NV_OK)
    {
        return status;
    }

    if (memmgrIsPmaInitialized(pMemoryManager))
    {
        _memmgrInitRUSDHeapSize(pGpu, pMemoryManager);
    }

    status = _memmgrAllocInternalClientObjects(pGpu, pMemoryManager);
    if (status != NV_OK)
    {
        //
        // TODO: Bug 3482892: Need a way to roll back StateInit
        //       steps in case of a failure
        // WAR for now is to cleanup with memmgrStateDestroy().
        //
        memmgrStateDestroy(pGpu, pMemoryManager);
        return status;
    }

    return NV_OK;
}

NV_STATUS
memmgrVerifyGspDmaOps_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NV_STATUS status = NV_OK;
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU8 *pTestBuffer;
    NvU32 testData = 0xdeadbeef;
    TRANSFER_SURFACE surf = {0};

    //
    // Return early if CPU access to CPR vidmem is allowed as GSP DMA
    // is not needed in this case
    //
    if (!kbusIsBarAccessBlocked(pKernelBus))
        return NV_OK;

    pTestBuffer = portMemAllocNonPaged(4096);
    NV_ASSERT_OR_RETURN(pTestBuffer != NULL, NV_ERR_INSUFFICIENT_RESOURCES);

    portMemSet(pTestBuffer, 0, 4096);

    status = memdescCreate(&pMemDesc, pGpu, RM_PAGE_SIZE, RM_PAGE_SIZE,
                           NV_TRUE, ADDR_FBMEM, NV_MEMORY_UNCACHED, 0);
    NV_ASSERT_OR_RETURN(status == NV_OK, status);

    memdescTagAlloc(status,
                    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_20, pMemDesc);
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    surf.pMemDesc = pMemDesc;
    surf.offset = sizeof(NvU32); // Choosing a random offset

    // Write test data to FB using GSP
    status = memmgrMemWrite(pMemoryManager, &surf, &testData, sizeof(NvU32),
                            TRANSFER_FLAGS_NONE);
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    // Read the same location using GSP and confirm that GSP read is also working fine
    status = memmgrMemRead(pMemoryManager, &surf, pTestBuffer, sizeof(NvU32),
                           TRANSFER_FLAGS_NONE);
    NV_ASSERT_OR_GOTO(status == NV_OK, failed);

    if (((NvU32*)pTestBuffer)[0] != testData)
    {
        NV_PRINTF(LEVEL_ERROR, "####################################################\n");
        NV_PRINTF(LEVEL_ERROR, "    Read back of data using GSP shows mismatch\n");
        NV_PRINTF(LEVEL_ERROR, "    Test data: 0x%x Read Data: 0x%x\n", testData, ((NvU32*)pTestBuffer)[0]);
        NV_PRINTF(LEVEL_ERROR, "####################################################\n");
        status = NV_ERR_INVALID_STATE;
        NV_ASSERT_OR_GOTO(status == NV_OK, failed);
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "####################################################\n");
        NV_PRINTF(LEVEL_INFO, "    Read back of data using GSP confirms write\n");
        NV_PRINTF(LEVEL_INFO, "####################################################\n");
    }

failed:
    memdescFree(pMemDesc);
    memdescDestroy(pMemDesc);
    portMemFree(pTestBuffer);

    return status;
}

NV_STATUS
memmgrStateLoad_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    NvU32 flags
)
{
    // If fbOverrideSizeMb is set, finish setting up the FB parameters now that state init has finished
    memmgrFinishHandleSizeOverrides_HAL(pGpu, pMemoryManager);

    if ((flags & GPU_STATE_FLAGS_PRESERVING) &&
        !(flags & GPU_STATE_FLAGS_GC6_TRANSITION))
    {
        //
        // Only do initialization scrubs (i.e. RM reserved region) on
        // non-GC6 transitions since GC6 cycles leave FB powered.
        //
        memmgrScrubInit_HAL(pGpu, pMemoryManager);
    }

    // Dump FB regions
    memmgrDumpFbRegions(pGpu, pMemoryManager);

    return NV_OK;
}

NV_STATUS
memmgrStatePreUnload_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    NvU32 flags
)
{
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NV_ASSERT((flags & GPU_STATE_FLAGS_PRESERVING) || pMemoryManager->zbcSurfaces == 0);

    if ((flags & GPU_STATE_FLAGS_PRESERVING))
    {
        //
        // fifo won't send a PreSchedulingDisable callback on StateUnload
        // destroy the channel manually, so that a CeUtils lite instance can be created for FBSR
        //
        memmgrDestroyCeUtils(pMemoryManager);
    }

    if (memmgrIsPmaEnabled(pMemoryManager) &&
        memmgrIsPmaSupportedOnPlatform(pMemoryManager) &&
        osNumaOnliningEnabled(pGpu->pOsGpuInfo) &&
        pKernelMemorySystem->memPartitionNumaInfo[0].bInUse)
    {
        pmaNumaOfflined(&pMemoryManager->pHeap->pmaObject);
    }

    return NV_OK;
}

NV_STATUS
memmgrStateUnload_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    NvU32 flags
)
{
    if ((flags & GPU_STATE_FLAGS_PRESERVING) &&
        !(flags & GPU_STATE_FLAGS_GC6_TRANSITION))
    {
        //
        // Initialiation scrubs only happen during StateLoad on non-GC6
        // transitions.
        //
        memmgrScrubDestroy_HAL(pGpu, pMemoryManager);
    }

    return NV_OK;
}

void
memmgrStateDestroy_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    Heap *pHeap = MEMORY_MANAGER_GET_HEAP(pMemoryManager);
    NvU32 i;

    _memmgrFreeInternalClientObjects(pMemoryManager);

    gpuDestroyRusdMemory(pGpu);

    // Destroys the SW state of the page level pools
    memmgrPageLevelPoolsDestroy(pGpu, pMemoryManager);

    // Destroy the heap entirely, and all associated structures
    if (pHeap)
    {
        kmemsysPreHeapDestruct_HAL(pGpu, pKernelMemorySystem);

        objDelete(pHeap);
        pMemoryManager->pHeap = NULL;
    }

    // RMCONFIG: only if FBSR engine is enabled
    if (RMCFG_MODULE_FBSR)
    {
        //
        // Cleanup fbsrReservedRanges
        // GSP_HEAP range is allocated, every other range is described
        //
        if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE] != NULL)
            memdescDestroy(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE]);

        if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE] != NULL)
            memdescDestroy(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE]);

        if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP] != NULL)
        {
            memdescFree(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP]);
            memdescDestroy(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP]);
        }

        if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_NON_WPR] != NULL)
            memdescDestroy(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_NON_WPR]);

        if (pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_VGA_WORKSPACE] != NULL)
            memdescDestroy(pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_VGA_WORKSPACE]);

        pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE] = NULL;
        pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE]  = NULL;
        pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_HEAP]       = NULL;
        pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_GSP_NON_WPR]    = NULL;
        pMemoryManager->fbsrReservedRanges[FBSR_RESERVED_INST_MEMORY_VGA_WORKSPACE]  = NULL;

        for (i = 0; i < NUM_FBSR_TYPES; i++)
        {
            fbsrDestroy_HAL(pGpu, pMemoryManager->pFbsr[i]);
        }
    }

    if (memmgrIsLocalEgmEnabled(pMemoryManager))
    {
        if (!IS_VIRTUAL_WITH_SRIOV(pGpu))
        {
            kbusUnreserveP2PPeerIds_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), NVBIT(pMemoryManager->localEgmPeerId));
        }
        pMemoryManager->localEgmPeerId = BUS_INVALID_PEER;
        pMemoryManager->bLocalEgmEnabled = NV_FALSE;
    }

    kfifoRemoveSchedulingHandler(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
        memmgrPostSchedulingEnableHandler, NULL,
        memmgrPreSchedulingDisableHandler, NULL);
    memmgrScrubDestroy_HAL(pGpu, pMemoryManager);
}

static NV_STATUS
_memmgrCreateChildObjects
(
    MemoryManager *pMemoryManager
)
{
    NV_STATUS status = NV_OK;

    // RMCONFIG: only if FBSR engine is enabled
    if (RMCFG_MODULE_FBSR)
    {
        NvU32 i;

        // Create FBSR object for every type RM supports.
        for (i = 0; i < NUM_FBSR_TYPES; i++)
        {
            status = _memmgrCreateFBSR(pMemoryManager, i);
            if (status != NV_OK)
            {
                return status;
            }
        }
    }

    return status;
}

NV_STATUS
memmgrCreateHeap_IMPL
(
    MemoryManager *pMemoryManager
)
{
    Heap               *newHeap;
    OBJGPU             *pGpu                = ENG_GET_GPU(pMemoryManager);
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NvU64               rsvdSize;
    NvU64               size;
    NV_STATUS           status              = NV_OK;

    // If we're using FB regions then rsvd memory is already marked as a reserved region
    if ((pMemoryManager->Ram.numFBRegions == 0) || (IS_VIRTUAL_WITH_SRIOV(pGpu)))
    {
        if (pMemoryManager->bReservedMemAtBottom)
        {
            // rsvd memory is already accounted for in heapStart
            rsvdSize = 0;
        }
        else
        {
            rsvdSize = pMemoryManager->rsvdMemorySize;
        }
    }
    else
        rsvdSize = 0;

    // for vGPU, add extra FB tax incurred by host RM to reserved size
    rsvdSize += memmgrGetFbTaxSize_HAL(pGpu, pMemoryManager);

    //
    // Fix up region descriptions to match with any FB override size
    //
    memmgrHandleSizeOverrides_HAL(pGpu, pMemoryManager);

    //
    // Calculate the FB heap size as the address space size, then deduct any reserved memory
    //
    size = pMemoryManager->Ram.fbAddrSpaceSizeMb << 20;
    size -= NV_MIN(size, rsvdSize);

    if((size != 0) || (pMemoryManager->bScanoutSysmem))
    {
        status = objCreate(&newHeap, pMemoryManager, Heap);
        if (status != NV_OK)
        {
            return status;
        }

        pMemoryManager->pHeap = newHeap;

        if (memmgrIsPmaEnabled(pMemoryManager) &&
            memmgrIsPmaSupportedOnPlatform(pMemoryManager))
        {
            portMemSet(&pMemoryManager->pHeap->pmaObject, 0, sizeof(pMemoryManager->pHeap->pmaObject));
            status = memmgrPmaInitialize(pGpu, pMemoryManager, &pMemoryManager->pHeap->pmaObject);
            NV_ASSERT_OR_RETURN(status == NV_OK, status);
        }

        status = heapInit(pGpu, newHeap,
                          pMemoryManager->heapStartOffset,
                          size - pMemoryManager->heapStartOffset, HEAP_TYPE_RM_GLOBAL, GPU_GFID_PF, NULL);
        NV_ASSERT_OK_OR_RETURN(status);

        if ((memmgrIsPmaInitialized(pMemoryManager)) && (pMemoryManager->pHeap->bHasFbRegions))
        {
            status = memmgrPmaRegisterRegions(pGpu, pMemoryManager, pMemoryManager->pHeap,
                                              &pMemoryManager->pHeap->pmaObject);
            NV_ASSERT_OR_RETURN(status == NV_OK, status);
        }

        NV_ASSERT_OK_OR_RETURN(memmgrValidateFBEndReservation_HAL(pGpu, pMemoryManager));

        NV_ASSERT_OK_OR_RETURN(memmgrReserveMemoryForFakeWPR_HAL(pGpu, pMemoryManager));

        NV_ASSERT_OK_OR_RETURN(memmgrReserveMemoryForPmu_HAL(pGpu, pMemoryManager));

        // Reserve vidmem for FSP usage, including FRTS, WPR2
        status = memmgrReserveMemoryForFsp(pGpu, pMemoryManager);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to reserve vidmem for WPR and FRTS.\n");
            return status;
        }

        if (!IsSLIEnabled(pGpu))
        {
            // Do the actual blacklisting of pages from the heap
            if (newHeap->blackListAddresses.count != 0)
            {
                status = heapBlackListPages(pGpu, newHeap);

                if (status != NV_OK)
                {
                    // Warn and continue
                    NV_PRINTF(LEVEL_WARNING, "Error 0x%x creating blacklist\n",
                              status);
                }
            }
        }

        kmemsysPostHeapCreate_HAL(pGpu, pKernelMemorySystem);
    }

    return status;
}

/*
 * @brief Gets per-device suballocator. If it is not available, get shared heap.
 *
 * @param[in] pMemoryManager MemoryManager pointer
 */
Heap *
memmgrGetDeviceSuballocator_IMPL
(
    MemoryManager *pMemoryManager,
    NvBool         bForceSubheap
)
{

    if (!bForceSubheap)
    {
        // If no suballocator found, use heap
        return MEMORY_MANAGER_GET_HEAP(pMemoryManager);
    }

    return NULL;
}

static NV_STATUS
_memmgrCreateFBSR
(
    MemoryManager *pMemoryManager,
    NvU32          type
)
{
    OBJFBSR *pFbsr;
    NV_STATUS status;

    status = objCreate(&pFbsr, pMemoryManager, OBJFBSR);
    if (status != NV_OK)
    {
        return status;
    }

    NV_ASSERT(pFbsr);
    pMemoryManager->pFbsr[type] = pFbsr;

    fbsrObjectInit(pFbsr, type);

    return NV_OK;
}

static void
_memmgrFreeInternalClientObjects
(
    MemoryManager *pMemoryManager
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (pMemoryManager->hThirdPartyP2P != 0)
    {
        pRmApi->Free(pRmApi, pMemoryManager->hClient,
                     pMemoryManager->hThirdPartyP2P);
        pMemoryManager->hThirdPartyP2P = 0;
    }

    if (pMemoryManager->hClient != 0)
    {
        rmapiutilFreeClientAndDeviceHandles(pRmApi,
                                            &pMemoryManager->hClient,
                                            &pMemoryManager->hDevice,
                                            &pMemoryManager->hSubdevice);
    }
}

static NV_STATUS
_memmgrAllocInternalClientObjects
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NV_STATUS status;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    status = rmapiutilAllocClientAndDeviceHandles(pRmApi, pGpu,
                                                  &pMemoryManager->hClient,
                                                  &pMemoryManager->hDevice,
                                                  &pMemoryManager->hSubdevice);
    if (status != NV_OK)
    {
        goto failed;
    }

    {
        NV503C_ALLOC_PARAMETERS params;
        NvHandle hThirdPartyP2P = 0;

        NV_ASSERT_OK_OR_GOTO(status, serverutilGenResourceHandle(pMemoryManager->hClient,
                                                                 &hThirdPartyP2P),
                             failed);

        portMemSet(&params, 0, sizeof(params));
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
        {
            params.flags = NV503C_ALLOC_PARAMETERS_FLAGS_TYPE_NVLINK;
        }
        else
        {
            params.flags = NV503C_ALLOC_PARAMETERS_FLAGS_TYPE_BAR1;
        }
        status = pRmApi->AllocWithHandle(pRmApi,
                                         pMemoryManager->hClient,
                                         pMemoryManager->hSubdevice,
                                         hThirdPartyP2P,
                                         NV50_THIRD_PARTY_P2P,
                                         &params,
                                         sizeof(params));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING, "Error creating internal ThirdPartyP2P object: %x\n",
                      status);
            pMemoryManager->hThirdPartyP2P = 0;
        }
        else
        {
            pMemoryManager->hThirdPartyP2P = hThirdPartyP2P;
        }

    }

    return NV_OK;

failed:
    _memmgrFreeInternalClientObjects(pMemoryManager);

    return status;
}

/*!
 * @brief Determine size of FB RAM which is used for RM internal allocations
 *        and PMA.
 *
 * @param[out] pFbUsedSize  FB used memory size
 *
 * @returns NV_OK
 */
NV_STATUS
memmgrGetUsedRamSize_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU64         *pFbUsedSize
)
{
    Heap   *pHeap = GPU_GET_HEAP(pGpu);
    NvU64   heapFreeSpace, heapTotalSpace, pmaFreeSpace;

    //
    // Determine free memory in FB and substract with total FB memory.
    // If PMA is initialized, then use the free memory size in PMA and
    // heap otherwise only use heap free memory for calculation.
    //
    heapGetFree(pHeap, &heapFreeSpace);
    heapGetSize(pHeap, &heapTotalSpace);
    if (memmgrIsPmaInitialized(pMemoryManager))
    {
        pmaGetFreeMemory(&pHeap->pmaObject, &pmaFreeSpace);
        *pFbUsedSize = heapTotalSpace - heapFreeSpace - pmaFreeSpace;
    }
    else
    {
        *pFbUsedSize = heapTotalSpace - heapFreeSpace;
    }

    //
    // GSP's WPR region has its own save/restore mechanism and does not need
    // to be accounted for in total FB size used - which is needed to find out
    // how much SYSMEM needs to be allocated to save all FB memory
    //
    if (IS_GSP_CLIENT(pGpu))
    {
        KernelGsp *pKernelGsp       = GPU_GET_KERNEL_GSP(pGpu);
        NvU64      gspWprRegionSize = pKernelGsp->pWprMeta->gspFwWprEnd - pKernelGsp->pWprMeta->gspFwWprStart;

        *pFbUsedSize = *pFbUsedSize - gspWprRegionSize;

        NV2080_CTRL_INTERNAL_GPU_GET_GSP_RM_FREE_HEAP_PARAMS params = {0};
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV_STATUS status;

        status = pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_GPU_GET_GSP_RM_FREE_HEAP,
                                 &params,
                                 sizeof(params));

        if (status == NV_OK)
        {
            *pFbUsedSize = *pFbUsedSize - params.freeHeapSize;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,"Failed to get free heap size of GSP-RM\n");
        }
    }

    return NV_OK;
}

NV_STATUS
memmgrAllocHwResources_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    FB_ALLOC_INFO *pFbAllocInfo
)
{
    MemoryManager  *pMemoryManagerLoop;
    FB_ALLOC_INFO  *pTempInfo = NULL;
    NvU32           skipFlag  = (pFbAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_SKIP_RESOURCE_ALLOC);
    NV_STATUS       rmStatus  = NV_OK;

    pTempInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
    if (pTempInfo == NULL)
    {
        NV_ASSERT(0);
        return NV_ERR_NO_MEMORY;
    }

    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // we can skip the resource allocation step.
    //

    if (IS_VIRTUAL(pGpu) && !vgpuIsGuestManagedHwAlloc(pGpu))
    {
        pFbAllocInfo->pageFormat->flags |= NVOS32_ALLOC_FLAGS_SKIP_RESOURCE_ALLOC;
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
    {
        NV_STATUS tempStatus;
        *pTempInfo = *pFbAllocInfo;    // struct copy

        pMemoryManagerLoop = GPU_GET_MEMORY_MANAGER(pGpu);

        tempStatus = memmgrAllocHal_HAL(pGpu, pMemoryManagerLoop, pTempInfo);
        // be sure to return an intermediate error
        if (NV_OK == rmStatus)
            rmStatus = tempStatus;
    }
    SLI_LOOP_END

    *pFbAllocInfo = *pTempInfo;    // struct copy
    portMemFree(pTempInfo);

    pFbAllocInfo->pageFormat->flags &= ~NVOS32_ALLOC_FLAGS_SKIP_RESOURCE_ALLOC;
    pFbAllocInfo->pageFormat->flags |= skipFlag;

    return rmStatus;
}

NV_STATUS
memmgrFreeHwResources_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    FB_ALLOC_INFO *pFbAllocInfo
)
{
    MemoryManager  *pMemoryManagerLoop;
    NV_STATUS       rmStatus = NV_OK;
    RMTIMEOUT       timeout;
    FB_ALLOC_INFO  *pTempInfo = NULL;

    pTempInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
    if (pTempInfo == NULL)
    {
        NV_ASSERT(0);
        return NV_ERR_NO_MEMORY;
    }

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

    if (IS_VIRTUAL(pGpu) && !vgpuIsGuestManagedHwAlloc(pGpu))
    {
        pFbAllocInfo->pageFormat->flags |= NVOS32_ALLOC_FLAGS_SKIP_RESOURCE_ALLOC;
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
    {
        NV_STATUS tempStatus;
        pMemoryManagerLoop = GPU_GET_MEMORY_MANAGER(pGpu);

        *pTempInfo = *pFbAllocInfo;

        tempStatus = memmgrFreeHal_HAL(pGpu, pMemoryManagerLoop, pTempInfo, &timeout);
        // be sure to return an intermediate error
        if (NV_OK == rmStatus)
            rmStatus = tempStatus;

    }
    SLI_LOOP_END

    *pFbAllocInfo = *pTempInfo;
    portMemFree(pTempInfo);

    return rmStatus;
}

NvBool
memmgrLargePageSupported_IMPL
(
    MemoryManager    *pMemoryManager,
    NV_ADDRESS_SPACE  addrSpace
)
{
    NvBool isSupported = NV_FALSE;

    if (addrSpace == ADDR_FBMEM || addrSpace == ADDR_VIRTUAL)
    {
        isSupported = NV_TRUE;
    }
    else if (addrSpace == ADDR_SYSMEM)
    {
        isSupported = (pMemoryManager->sysmemPageSize != RM_PAGE_SIZE);
    }
    else
    {
        NV_ASSERT(0);
    }

    return isSupported;
}

NvBool
memmgrComprSupported_IMPL
(
    MemoryManager    *pMemoryManager,
    NV_ADDRESS_SPACE  addrSpace
)
{
    OBJGPU *pGpu        = ENG_GET_GPU(pMemoryManager);
    NvBool  isSupported = NV_FALSE;

    if (GPU_GET_KERNEL_GMMU(pGpu) != NULL)
    {
        if (memmgrLargePageSupported(pMemoryManager, addrSpace) ||
            pMemoryManager->bSmallPageCompression)
        {
            if (addrSpace == ADDR_FBMEM || addrSpace == ADDR_VIRTUAL)
            {
                isSupported = NV_TRUE;
            }
            else if (addrSpace == ADDR_SYSMEM)
            {
                // Compression is allowed on vidmem or unified aperture (vidmem/sysmem is same w.r.t HW)
                isSupported = (gpuIsUnifiedMemorySpaceEnabled(pGpu) &&
                               pMemoryManager->bSysmemCompressionSupportDef);
                NV_PRINTF(LEVEL_INFO, "isSupported=%s\n",
                          isSupported ? "NV_TRUE" : "NV_FALSE");
            }
            else
            {
                NV_ASSERT(0);
            }
        }
    }

    return isSupported;
}

NV_ADDRESS_SPACE
memmgrAllocGetAddrSpace_IMPL
(
    MemoryManager *pMemoryManager,
    NvU32          flags,
    NvU32          attr
)
{
   NV_ADDRESS_SPACE addrSpace = ADDR_UNKNOWN;

   if (flags & NVOS32_ALLOC_FLAGS_VIRTUAL)
   {
       addrSpace = ADDR_VIRTUAL;
   }
   else if (FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _VIDMEM, attr))
   {
       addrSpace = ADDR_FBMEM;
   }
   else
   {
        // In case location is SYSMEM or ANY, allocate in vidmem if protected flag is set.
        if (flags & NVOS32_ALLOC_FLAGS_PROTECTED)
        {
            addrSpace = ADDR_FBMEM;
        }
        else
        {
            addrSpace = ADDR_SYSMEM;
        }
   }

   return addrSpace;
}

NvU64
memmgrGetMappableRamSizeMb_IMPL(MemoryManager *pMemoryManager)
{
    return pMemoryManager->Ram.mapRamSizeMb;
}
//
// ZBC clear create/destroy routines.
//

NV_STATUS
memmgrFillMemdescForPhysAttr_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    PMEMORY_DESCRIPTOR pMemDesc,
    ADDRESS_TRANSLATION addressTranslation,
    NvU64 *pOffset,
    NvU32 *pMemAperture,
    NvU32 *pMemKind,
    NvU32 *pZCullId,
    NvU32 *pGpuCacheAttr,
    NvU32 *pGpuP2PCacheAttr,
    NvU64 *contigSegmentSize
)
{
    NvU64 surfOffset = *pOffset, surfBase, surfLimit;
    NvU32 zcbitmap;

    surfBase  = memdescGetPhysAddr(pMemDesc, addressTranslation, 0);
    surfLimit = surfBase + pMemDesc->Size - 1;
    *pMemKind = memdescGetPteKind(pMemDesc);

    *pOffset  = memdescGetPhysAddr(pMemDesc, addressTranslation, surfOffset);

    if (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM )
        *pMemAperture = NV0041_CTRL_CMD_GET_SURFACE_PHYS_ATTR_APERTURE_VIDMEM;
    else if (memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM)
        *pMemAperture = NV0041_CTRL_CMD_GET_SURFACE_PHYS_ATTR_APERTURE_SYSMEM;
    else if (memdescGetAddressSpace(pMemDesc) == ADDR_EGM)
        *pMemAperture = NV0041_CTRL_CMD_GET_SURFACE_PHYS_ATTR_APERTURE_SYSMEM;
    else if (memdescGetAddressSpace(pMemDesc) == ADDR_VIRTUAL )
    {
        //
        // XXX we could theoretically find whatever phys mem object is plugged
        // in at surfOffset w/in the virt object... that'd mean scanning
        // pMemory->DmaMappingList
        //
        return NV_ERR_NOT_SUPPORTED;
    }
    else
        return NV_ERR_GENERIC;

    if (memdescGetGpuCacheAttrib(pMemDesc) == NV_MEMORY_CACHED)
    {
        *pGpuCacheAttr = NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED;
    }
    else if (memdescGetGpuCacheAttrib(pMemDesc) == NV_MEMORY_UNCACHED)
    {
        *pGpuCacheAttr = NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_UNCACHED;
    }
    else
    {
        *pGpuCacheAttr = NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED_UNKNOWN;
    }

    if (memdescGetGpuP2PCacheAttrib(pMemDesc) == NV_MEMORY_CACHED)
    {
        *pGpuP2PCacheAttr = NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED;
    }
    else if (memdescGetGpuP2PCacheAttrib(pMemDesc) == NV_MEMORY_UNCACHED)
    {
        *pGpuP2PCacheAttr = NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_UNCACHED;
    }
    else
    {
        *pGpuP2PCacheAttr = NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED_UNKNOWN;
    }

    zcbitmap = FB_HWRESID_ZCULL_VAL_FERMI(memdescGetHwResId(pMemDesc)); //bitmap form... need a scalar
    for ( *pZCullId = 0;  zcbitmap; zcbitmap >>= 1, *pZCullId += 1) {;;;}
    *pZCullId -= 1; // side effect if there is no zcull id of setting ~0

    *contigSegmentSize = surfLimit - (surfBase + surfOffset) + 1;

    if ( !memdescGetContiguity(pMemDesc, addressTranslation))
    {
        // XXX overly conservative.  we could scan the PTEs to find out if more pages are contig.
        NvU64 surfOffsetLimitSame4KBPage = (4*1024)*((surfBase + surfOffset)/(4*1024)) + (4*1024) - 1;
        if ( surfLimit >= surfOffsetLimitSame4KBPage )
            *contigSegmentSize = surfOffsetLimitSame4KBPage - (surfBase + surfOffset) + 1;
    }

    return NV_OK;
}

NvU64
memmgrDeterminePageSize_IMPL
(
    MemoryManager *pMemoryManager,
    NvHandle       hClient,
    NvU64          memSize,
    NvU32          memFormat,
    NvU32          pageFormatFlags,
    NvU32         *pRetAttr,
    NvU32         *pRetAttr2
)
{
    OBJGPU           *pGpu  = ENG_GET_GPU(pMemoryManager);
    KernelGmmu       *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NV_ADDRESS_SPACE  addrSpace;
    NvBool            bIsBigPageSupported;
    RM_ATTR_PAGE_SIZE pageSizeAttr;
    NvU64             pageSize = 0;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY) || (pKernelGmmu == NULL))
    {
        pageSize = RM_PAGE_SIZE;
    }
    // Sanity check the arguments.
    else if (pRetAttr == NULL || pRetAttr2 == NULL)
    {
        NV_ASSERT_OR_RETURN(0, 0);
    }
    else
    {
        addrSpace = memmgrAllocGetAddrSpace(pMemoryManager, pageFormatFlags, *pRetAttr);

        //
        // Bug 4270864: Temp hack until sysmem supports higher order allocations.
        // We allow EGM to get allocated at higher page size.
        //
        if (memmgrIsLocalEgmEnabled(pMemoryManager) &&
            addrSpace == ADDR_SYSMEM &&
            FLD_TEST_DRF(OS32, _ATTR2, _FIXED_NUMA_NODE_ID, _YES, *pRetAttr2) &&
            //
            // Bug 4270868: MODS has test cases which pass FIXED_NUMA_NODE_ID,
            // but invalid node_id. Will remove once MODS tests get fixed.
            //
            !RMCFG_FEATURE_MODS_FEATURES)
        {
            bIsBigPageSupported = NV_TRUE;
        }
        else
        {
            bIsBigPageSupported = memmgrLargePageSupported(pMemoryManager, addrSpace);
        }
        pageSizeAttr = dmaNvos32ToPageSizeAttr(*pRetAttr, *pRetAttr2);

        //
        // Precedence in page size selection
        // 1. CACHE_ONLY mode                                                   -> SMALL
        // 2. !BigPageSupport (Sysmem && GpuSmmuOff )                           -> SMALL
        // 3. Client page size override                                         -> Use override
        // 4. HugePageSupported && size >= HugePageSize                         -> HUGE
        // 5. Block-linear || size >= minSizeForBigPage || hClient || GpuSmmuOn -> BIG
        // 6. none of the above                                                 -> SMALL
        //
        // On Tegra, we don't have a carveout/FB in production. So, we're
        // not guaranteed to get BIG page sized or contiguous allocations
        // from OS. But we need BIG page sized allocations for efficient Big GPU
        // operation. We use the SMMU unit within the Tegra Memory Contoller (MC),
        // to construct BIG pages from the 4KB small page allocations from OS.
        // SMMU will linearize the discontiguous 4KB allocations into what will
        // appear to the GPU as a large contiguous physical allocation.
        //
        // RM will eventually decide whether a SYSMEM allocation needs BIG page
        // via GPU SMMU mapping. Right now, we give an option for RM clients to
        // force it, via the SMMU_ON_GPU attribute.
        //
        if (gpuIsCacheOnlyModeEnabled(pGpu))
        {
            pageSize = RM_PAGE_SIZE;
        }
        else if (!bIsBigPageSupported)
        {
            if (RM_ATTR_PAGE_SIZE_BIG == pageSizeAttr ||
                RM_ATTR_PAGE_SIZE_HUGE == pageSizeAttr ||
                RM_ATTR_PAGE_SIZE_256GB == pageSizeAttr ||
                RM_ATTR_PAGE_SIZE_512MB == pageSizeAttr)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Big/Huge/512MB/256GB page size not supported in sysmem.\n");

                NV_ASSERT_OR_RETURN(0, 0);
            }
            else
            {
                pageSize = RM_PAGE_SIZE;
            }
        }
        else
        {
            switch (pageSizeAttr)
            {
                case RM_ATTR_PAGE_SIZE_INVALID:
                    NV_PRINTF(LEVEL_ERROR, "invalid page size attr\n");
                    NV_ASSERT_OR_RETURN(0, 0);

                case RM_ATTR_PAGE_SIZE_DEFAULT:
                {
                    NvBool bUseDefaultHugePagesize = NV_TRUE;
                    // WDDMV2 Windows it expect default page size to be 4K /64KB /128KB
                    if (bUseDefaultHugePagesize &&
                        kgmmuIsHugePageSupported(pKernelGmmu) &&
                        (memSize >= RM_PAGE_SIZE_HUGE) && (addrSpace != ADDR_SYSMEM ||
                        pMemoryManager->sysmemPageSize == RM_PAGE_SIZE_HUGE))
                    {
                        pageSize = RM_PAGE_SIZE_HUGE;
                        break;
                    }
                    else if ((memFormat != NVOS32_ATTR_FORMAT_PITCH) ||
                             (memSize >= kgmmuGetMinBigPageSize(pKernelGmmu)) || hClient ||
                             FLD_TEST_DRF(OS32, _ATTR2, _SMMU_ON_GPU, _ENABLE, *pRetAttr2))
                    {
                        pageSize = kgmmuGetMaxBigPageSize_HAL(pKernelGmmu);
                        break;
                    }

                    pageSize = RM_PAGE_SIZE;
                    break;
                }

                case RM_ATTR_PAGE_SIZE_4KB:
                    pageSize = RM_PAGE_SIZE;
                    break;

                case RM_ATTR_PAGE_SIZE_BIG:
                    pageSize = kgmmuGetMaxBigPageSize_HAL(pKernelGmmu);
                    break;

                case RM_ATTR_PAGE_SIZE_HUGE:
                    if (kgmmuIsHugePageSupported(pKernelGmmu))
                    {
                        pageSize = RM_PAGE_SIZE_HUGE;
                    }
                    else
                    {
                        NV_ASSERT_OR_RETURN(0, 0);
                    }
                    break;

                case RM_ATTR_PAGE_SIZE_512MB:
                    if (kgmmuIsPageSize512mbSupported(pKernelGmmu))
                    {
                        pageSize = RM_PAGE_SIZE_512M;
                    }
                    else
                    {
                        NV_ASSERT_OR_RETURN(0, 0);
                    }
                    break;

                case RM_ATTR_PAGE_SIZE_256GB:
                    if (kgmmuIsPageSize256gbSupported(pKernelGmmu))
                    {
                        pageSize = RM_PAGE_SIZE_256G;
                    }
                    else
                    {
                        NV_ASSERT_OR_RETURN(0, 0);
                    }
                    break;
                default:
                    NV_ASSERT(0);
            }
        }
    }

    switch (pageSize)
    {
        case RM_PAGE_SIZE:
            *pRetAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _4KB, *pRetAttr);
            break;

        case RM_PAGE_SIZE_64K:
        case RM_PAGE_SIZE_128K:
            *pRetAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _BIG, *pRetAttr);
            break;

        case RM_PAGE_SIZE_HUGE:
            *pRetAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _HUGE, *pRetAttr);
            *pRetAttr2 = FLD_SET_DRF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _2MB, *pRetAttr2);
            break;

        case RM_PAGE_SIZE_512M:
            *pRetAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _HUGE, *pRetAttr);
            *pRetAttr2 = FLD_SET_DRF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _512MB,  *pRetAttr2);
            break;

        case RM_PAGE_SIZE_256G:
            *pRetAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _HUGE, *pRetAttr);
            *pRetAttr2 = FLD_SET_DRF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _256GB,  *pRetAttr2);
            break;
        default:
            NV_ASSERT(0);
    }

    return pageSize;
}

/*!
 * Identify if platform's current configuration supports PMA
 */
NV_STATUS
memmgrSetPlatformPmaSupport_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    //
    // KMD in WDDM mode will not support pma managed client page tables as
    // in both cases client / OS manges it.
    //
    if (RMCFG_FEATURE_PLATFORM_WINDOWS && !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_TCC_MODE))
    {
        memmgrSetClientPageTablesPmaManaged(pMemoryManager, NV_FALSE);
    }

    //
    // FB management should use PMA on Unix/Linux/Mods/Windows
    //
    if (RMCFG_FEATURE_PLATFORM_UNIX
        || RMCFG_FEATURE_PLATFORM_MODS
        || RMCFG_FEATURE_PLATFORM_WINDOWS)
    {
        pMemoryManager->bPmaSupportedOnPlatform = NV_TRUE;
    }

    //
    // PMA memory management is not currently supported in non SRIOV VGPU environment.
    // The RPC mechanism needs to be expanded to distinguish allocation types.
    // Bug #1735412
    //
    // TODO : Remove these constraints.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        pMemoryManager->bPmaSupportedOnPlatform = NV_FALSE;
    }

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VGPU))
    {
        if (pMemoryManager->bVgpuPmaSupport)
        {
            memmgrSetClientPageTablesPmaManaged(pMemoryManager, NV_FALSE);
        }
        else
        {
            pMemoryManager->bPmaSupportedOnPlatform = NV_FALSE;
        }
    }
    return (NV_OK);
}

/*!
 * Allocate console region in CPU-RM based on region table passed from Physical RM
 */
NV_STATUS
memmgrAllocateConsoleRegion_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    FB_REGION_DESCRIPTOR *pConsoleFbRegion
)
{

    NV_STATUS status     = NV_OK;
    NvU32     consoleRegionId = 0x0;
    NvU64     regionSize;

    if (pMemoryManager->Ram.ReservedConsoleDispMemSize > 0)
    {
        pMemoryManager->Ram.fbRegion[consoleRegionId].bLostOnSuspend = NV_FALSE;
        pMemoryManager->Ram.fbRegion[consoleRegionId].bPreserveOnSuspend = NV_TRUE;

        pConsoleFbRegion->base = pMemoryManager->Ram.fbRegion[consoleRegionId].base;
        pConsoleFbRegion->limit = pMemoryManager->Ram.fbRegion[consoleRegionId].limit;

        regionSize = pConsoleFbRegion->limit - pConsoleFbRegion->base + 1;

        // Once the console is reserved, we don't expect to reserve it again
        NV_ASSERT_OR_RETURN(pMemoryManager->Ram.pReservedConsoleMemDesc == NULL,
                        NV_ERR_STATE_IN_USE);

        status = memdescCreate(&pMemoryManager->Ram.pReservedConsoleMemDesc, pGpu,
                            regionSize, RM_PAGE_SIZE_64K, NV_TRUE, ADDR_FBMEM,
                            NV_MEMORY_UNCACHED,
                            MEMDESC_FLAGS_SKIP_RESOURCE_COMPUTE);
        if (status != NV_OK)
        {
            pConsoleFbRegion->base = pConsoleFbRegion->limit = 0;
            return status;
        }

        memdescDescribe(pMemoryManager->Ram.pReservedConsoleMemDesc, ADDR_FBMEM,
                        pConsoleFbRegion->base, regionSize);
        memdescSetPageSize(pMemoryManager->Ram.pReservedConsoleMemDesc,
                    AT_GPU, RM_PAGE_SIZE);


        NV_PRINTF(LEVEL_INFO, "Allocating console region of size: %llx, at base : %llx \n ",
                        regionSize, pConsoleFbRegion->base);
    }

    return status;
}

void
memmgrReleaseConsoleRegion_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    memdescDestroy(pMemoryManager->Ram.pReservedConsoleMemDesc);
    pMemoryManager->Ram.pReservedConsoleMemDesc = NULL;
}

PMEMORY_DESCRIPTOR
memmgrGetReservedConsoleMemDesc_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    return pMemoryManager->Ram.pReservedConsoleMemDesc;
}

/*!
 * Reserve FB for allocating BAR2 Page Dirs and Page Tables
 */
void
memmgrReserveBar2BackingStore_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU64         *pAddr
)
{
    NvU64             tmpAddr = *pAddr;
    KernelBus        *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    NvU32 pageDirsSize = kbusGetSizeOfBar2PageDirs_HAL(pGpu, pKernelBus);
    NvU32 pageTblsSize = kbusGetSizeOfBar2PageTables_HAL(pGpu, pKernelBus);

    // Reserve space for BAR2 Page Dirs
    if (pKernelBus->PDEBAR2Aperture == ADDR_FBMEM)
    {
        tmpAddr = NV_ROUNDUP(tmpAddr, RM_PAGE_SIZE);
        pKernelBus->bar2[GPU_GFID_PF].pdeBase  = tmpAddr;
        tmpAddr += pageDirsSize;
    }

    // Reserve space for BAR2 Page Tables
    if (pKernelBus->PTEBAR2Aperture == ADDR_FBMEM)
    {
        tmpAddr = NV_ROUNDUP(tmpAddr, RM_PAGE_SIZE);
        pKernelBus->bar2[GPU_GFID_PF].pteBase = tmpAddr;
        tmpAddr += pageTblsSize;
    }

    NV_PRINTF(LEVEL_INFO, "Reserve space for bar2 Page dirs offset = 0x%llx size = 0x%x\n",
        pKernelBus->bar2[GPU_GFID_PF].pdeBase, pageDirsSize);

    NV_PRINTF(LEVEL_INFO, "Reserve space for bar2 Page tables offset = 0x%llx size = 0x%x\n",
        pKernelBus->bar2[GPU_GFID_PF].pteBase, pageTblsSize);

    *pAddr = NV_ROUNDUP(tmpAddr, RM_PAGE_SIZE);
}

/*!
 *  Calculate the Vista reserved memory requirement per FB region for mixed type/density
 */
void
memmgrCalcReservedFbSpace_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NvU64   rsvdFastSize  = 0;
    NvU64   rsvdSlowSize  = 0;
    NvU64   rsvdISOSize   = 0;
    NvU32   i;
    NvU32   idxISORegion  = 0;
    NvU32   idxFastRegion = 0;
    NvU32   idxSlowRegion = 0;
    NvBool  bAllocProtected = NV_FALSE;

    bAllocProtected = gpuIsCCFeatureEnabled(pGpu);

    //
    // This is a hack solely for Vista (on Vista the OS controls the majority of heap).
    // Linux and Mac don't have reserved memory and doesn't use this function.
    //
    // On Vista, Fermi's instance memory is not reserved by RM anymore.
    // KMD has to reserve enough instance memory for driver private data.
    // This function does the calculation of needed space.  See bug 642233.
    // While it returns the result in Mb, the calculation is made with byte
    //

    // If we have no usable memory then we can't reserve any.
    if (!pMemoryManager->Ram.fbUsableMemSize)
        return;

    memmgrCalcReservedFbSpaceHal_HAL(pGpu, pMemoryManager, &rsvdFastSize, &rsvdSlowSize, &rsvdISOSize);

    // If we have regions defined, fill in the per-segment reserved memory requirement
    if (pMemoryManager->Ram.numFBRegions > 0)
    {
        NvBool bFastAssigned = NV_FALSE;
        NvBool bSlowAssigned = NV_FALSE;
        NvBool bIsoAssigned = NV_FALSE;

        //
        // Find the fastest, slowest, and ISO regions.
        //
        for (i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
        {
            FB_REGION_DESCRIPTOR *pFbRegion = &pMemoryManager->Ram.fbRegion[i];
            NvU64 regionSize = (pFbRegion->limit - pFbRegion->base +1);

            // Check only non-reserved regions (which are typically unpopulated blackholes in address space)
            if ((!pFbRegion->bRsvdRegion) &&
                (bAllocProtected || !pFbRegion->bProtected)  &&
                (regionSize >= (rsvdFastSize + rsvdSlowSize + rsvdISOSize)))
            {
                // Find the fastest region
                if (!bFastAssigned
                        || (pFbRegion->performance > pMemoryManager->Ram.fbRegion[idxFastRegion].performance)
                        || pMemoryManager->Ram.fbRegion[idxFastRegion].bRsvdRegion
                        || (!bAllocProtected && pMemoryManager->Ram.fbRegion[idxFastRegion].bProtected))
                {
                    idxFastRegion = i;
                    bFastAssigned = NV_TRUE;
                }
                // Find the slowest region
                if (!bSlowAssigned
                        || (pFbRegion->performance < pMemoryManager->Ram.fbRegion[idxSlowRegion].performance)
                        || pMemoryManager->Ram.fbRegion[idxSlowRegion].bRsvdRegion
                        || (!bAllocProtected && pMemoryManager->Ram.fbRegion[idxSlowRegion].bProtected))
                {
                    idxSlowRegion = i;
                    bSlowAssigned = NV_TRUE;
                }
                 // Find the fastest ISO region
                if (pFbRegion->bSupportISO)
                {
                    if (!bIsoAssigned ||
                        (!pMemoryManager->Ram.fbRegion[idxISORegion].bSupportISO) ||
                        (pFbRegion->performance > pMemoryManager->Ram.fbRegion[idxISORegion].performance)
                        || (!bAllocProtected && pMemoryManager->Ram.fbRegion[idxISORegion].bProtected))
                    {
                        idxISORegion = i;
                        bIsoAssigned = NV_TRUE;
                    }
                }
            }
        }

        NV_ASSERT(bFastAssigned && bSlowAssigned && bIsoAssigned);

        // There should *ALWAYS* be a region that supports ISO, even if we have no display
        NV_ASSERT(pMemoryManager->Ram.fbRegion[idxISORegion].bSupportISO);

        // There should *ALWAYS* be a non-reserved region that is faster than reserved and supports ISO
        NV_ASSERT(!pMemoryManager->Ram.fbRegion[idxISORegion].bRsvdRegion);
        NV_ASSERT(!pMemoryManager->Ram.fbRegion[idxFastRegion].bRsvdRegion);
        NV_ASSERT(!pMemoryManager->Ram.fbRegion[idxSlowRegion].bRsvdRegion);

        //
        // Whenever Hopper CC is enabled, it is mandatory to put allocations
        // like page tables, CBC and fault buffers in CPR region. Cannot put
        // reserved memory in protected region in non CC cases
        //
        if (!bAllocProtected)
        {
            NV_ASSERT(!pMemoryManager->Ram.fbRegion[idxISORegion].bProtected);
            NV_ASSERT(!pMemoryManager->Ram.fbRegion[idxFastRegion].bProtected);
            NV_ASSERT(!pMemoryManager->Ram.fbRegion[idxSlowRegion].bProtected);
        }

        //
        // Vista expects to be able to VidHeapControl allocate a cursor in ISO
        //
        // For mixed density reserved memory should be split between "fast" and
        // "slow" memory. Fast memory should also support ISO.  The policy to
        // prefer "slow" vs "fast" memory is platform dependent.
        //
        pMemoryManager->Ram.fbRegion[idxISORegion].rsvdSize += rsvdISOSize;
        pMemoryManager->Ram.fbRegion[idxSlowRegion].rsvdSize += rsvdSlowSize;
        pMemoryManager->Ram.fbRegion[idxFastRegion].rsvdSize += rsvdFastSize;
    }
}

/*!
 * Init channel size
 *
 * @param[in]  pChannel       OBJCHANNEL pointer
 * @param[in]  numCopyBlocks  Number of copies that should fit in the push buffer
 *
 * @returns NV_STATUS
 */
void
memmgrMemUtilsSetupChannelBufferSizes_IMPL
(
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel,
    NvU32          numCopyBlocks
)
{
    // set channel specific sizes
    pChannel->channelPbSize            = numCopyBlocks * MEMUTILS_SIZE_PER_BLOCK_INBYTES;
    pChannel->channelNotifierSize      = MEMUTILS_CHANNEL_NOTIFIER_SIZE;
    pChannel->channelNumGpFifioEntries = MEMUTILS_NUM_GPFIFIO_ENTRIES;
    pChannel->methodSizePerBlock       = MEMUTILS_SIZE_PER_BLOCK_INBYTES;
    pChannel->channelSize              = pChannel->channelPbSize + MEMUTILS_CHANNEL_GPFIFO_SIZE + MEMUTILS_CHANNEL_SEMAPHORE_SIZE;
    pChannel->semaOffset               = pChannel->channelPbSize + MEMUTILS_CHANNEL_GPFIFO_SIZE;
    pChannel->finishPayloadOffset      = pChannel->semaOffset + 4;
}

NV_STATUS memmgrFree_IMPL
(
    OBJGPU             *pGpu,
    MemoryManager      *pMemoryManager,
    Heap               *pHeap,
    NvHandle            hClient,
    NvHandle            hDevice,
    NvHandle            hVASpace,
    NvU32               owner,
    MEMORY_DESCRIPTOR  *pMemDesc
)
{
    NvU64       offsetAlign;
    NV_STATUS   status;
    NvU32       pmaFreeFlag       = 0;

    // IRQL TEST:  must be running at equivalent of passive-level
    IRQL_ASSERT_AND_RETURN(!osIsRaisedIRQL());

    if (pMemDesc == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    offsetAlign = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);

    if (owner == NVOS32_BLOCK_TYPE_FREE)
        return NV_ERR_INVALID_ARGUMENT;

    // Virtual heap allocs are tagged vitual and always own the memdesc
    if (memdescGetAddressSpace(pMemDesc) == ADDR_VIRTUAL)
    {
        OBJVASPACE  *pVAS = NULL;
        RsClient    *pClient;

        status = serverGetClientUnderLock(&g_resServ, hClient, &pClient);
        if (status != NV_OK)
            return status;

        status = vaspaceGetByHandleOrDeviceDefault(pClient, hDevice, hVASpace, &pVAS);
        if (status != NV_OK)
            return status;

        status = vaspaceFree(pVAS, offsetAlign);
        memdescDestroy(pMemDesc);
        return status;
    }

    // Free up the memory allocated by PMA.
    if (pMemDesc->pPmaAllocInfo)
    {
        FB_ALLOC_INFO        *pFbAllocInfo       = NULL;
        FB_ALLOC_PAGE_FORMAT *pFbAllocPageFormat = NULL;
        OBJGPU               *pMemdescOwnerGpu   = NULL;

        //
        // A memdesc can be duped under a peer device. In that case, before
        // freeing FB make sure the GPU which owns the memdesc is available.
        // Otherwise, just assert, destroy the memdesc and return NV_OK to
        // make sure rest of the clean up happens correctly as we are on
        // destroy path.
        // Note this is just a WAR till ressrv bring in cleanup of dup objects
        // on GPU tear down.
        // RS-TODO: Nuke this check once the cleanup is implemented.
        //
        if (pGpu != pMemDesc->pGpu)
        {
            if (!gpumgrIsGpuPointerValid(pMemDesc->pGpu))
            {
                //
                // This should never happen. GPU tear down should always clear
                // the duped memory list after resource server implements it.
                // For now just assert!
                //
                NV_ASSERT(0);
                memdescDestroy(pMemDesc);
                goto pma_free_exit;
            }
        }

        pMemdescOwnerGpu = pMemDesc->pGpu;

        //
        // Similar to the above WAR, if portMem alocations fail for any reason,
        // just assert and return NV_OK to ensure that the rest of the clean up
        // happens correctly.
        //
        pFbAllocInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
        if (pFbAllocInfo == NULL)
        {
            NV_ASSERT(0);
            goto pma_free_exit;
        }

        pFbAllocPageFormat = portMemAllocNonPaged(sizeof(FB_ALLOC_PAGE_FORMAT));
        if (pFbAllocPageFormat == NULL) {
            NV_ASSERT(0);
            goto pma_free_exit;
        }

        portMemSet(pFbAllocInfo, 0, sizeof(FB_ALLOC_INFO));
        portMemSet(pFbAllocPageFormat, 0, sizeof(FB_ALLOC_PAGE_FORMAT));
        pFbAllocInfo->hClient = hClient;
        pFbAllocInfo->hDevice = hDevice;
        pFbAllocInfo->pageFormat = pFbAllocPageFormat;

        //
        // Do not release any HW resources associated with this allocation
        // until the last reference to the allocation is freed. Passing
        // hwresid = 0 and format = pitch to memmgrFreeHwResources will ensure
        // that no comptags/zcull/zbc resources are freed.
        //
        if (pMemDesc->RefCount == 1)
        {
            pFbAllocInfo->hwResId = memdescGetHwResId(pMemDesc);
            pFbAllocInfo->format  = memdescGetPteKind(pMemDesc);
        }
        else
        {
            pFbAllocInfo->hwResId = 0;
            pFbAllocInfo->format = 0;
        }
        pFbAllocInfo->offset  = offsetAlign;
        pFbAllocInfo->size    = pMemDesc->Size;

        // Free any HW resources allocated.
        memmgrFreeHwResources(pMemdescOwnerGpu,
                GPU_GET_MEMORY_MANAGER(pMemdescOwnerGpu), pFbAllocInfo);

        if (pMemDesc->pPmaAllocInfo != NULL)
        {
            // Disabling scrub on free for non compressible surfaces
            if (RMCFG_FEATURE_PLATFORM_MODS &&
                !memmgrIsKind_HAL(GPU_GET_MEMORY_MANAGER(pMemdescOwnerGpu),
                                  FB_IS_KIND_COMPRESSIBLE,
                                  memdescGetPteKind(pMemDesc)))
            {
                pmaFreeFlag = PMA_FREE_SKIP_SCRUB;
            }

            vidmemPmaFree(pMemdescOwnerGpu, pHeap, pMemDesc->pPmaAllocInfo, pmaFreeFlag);
            NV_PRINTF(LEVEL_INFO, "Freeing PMA allocation\n");
        }

pma_free_exit:
        portMemFree(pFbAllocInfo);
        portMemFree(pFbAllocPageFormat);
        memdescDestroy(pMemDesc);

        return NV_OK;
    }

    return heapFree(pGpu, pHeap, hClient, hDevice, owner, pMemDesc);
}

NV_STATUS
memmgrSetPartitionableMem_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS params = {0};
    Heap *pHeap = GPU_GET_HEAP(pGpu);
    NvU64 bottomRsvdSize = 0;
    NvU64 topRsvdSize = 0;
    NvU32 bottomRegionIdx = 0xFFFF;
    NvU32 topRegionIdx = 0xFFFF;
    NvU32 i;
    NvU64 size;
    NvU64 base;
    NvU64 offset;
    NvU64 freeMem;

    //
    // Find out the first and the last region for which internal heap or
    // bRsvdRegion is true. In Ampere we should never have more than two
    // discontigous RM reserved region
    // To-Do - Bug 2301972 - Make sure that reserved memory is aligned to VMMU
    // segments
    //
    for (i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
    {
        if (pMemoryManager->Ram.fbRegion[i].bInternalHeap ||
            pMemoryManager->Ram.fbRegion[i].bRsvdRegion)
        {
            NvU64 rsvdSize = (pMemoryManager->Ram.fbRegion[i].limit -
                              pMemoryManager->Ram.fbRegion[i].base + 1);

            // Check if this is bottom reserved region
            if (pMemoryManager->Ram.fbRegion[i].base == 0)
            {
                bottomRegionIdx = i;
                bottomRsvdSize += rsvdSize;
            }
            else if (i > 0 && (pMemoryManager->Ram.fbRegion[i-1].bInternalHeap ||
                              pMemoryManager->Ram.fbRegion[i-1].bRsvdRegion) &&
                    (pMemoryManager->Ram.fbRegion[i].base == pMemoryManager->Ram.fbRegion[i - 1].limit + 1))
            {
                // See if this is the contigous region with previous discovery
                if (bottomRegionIdx == (i - 1))
                {
                    // Contigous bottom region
                    bottomRsvdSize += rsvdSize;
                }
                else
                {
                    // Contigous top region
                    topRsvdSize += rsvdSize;
                }
            }
            else
            {
                //
                // Make sure we don't have discontigous reserved regions as
                // they are not supported by HW also and we need to support
                // these by using blacklisting mechanism.
                //
                if (topRegionIdx != 0xFFFF)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "More than two discontigous rsvd regions found. "
                              "Rsvd region base - 0x%llx, Rsvd region Size - 0x%llx\n",
                              pMemoryManager->Ram.fbRegion[i].base, rsvdSize);
                    NV_ASSERT(0);
                    return NV_ERR_INVALID_STATE;
                }

                topRegionIdx = i;
                topRsvdSize += rsvdSize;
            }
        }
    }

    //
    // Sanity check against the biggest available memory chunk. Pick the smallest
    // of biggest available memory chunk or calculated total - reserved memory as
    // in vGPU we are still using OBJHEAP and there are some allocations which
    // happens at the top of the heap before we program this register
    //
    if (!memmgrIsPmaInitialized(pMemoryManager))
    {
        NvU64 bytesTotal;
        const NvU64 vgpuHeapWarSize = 256 *1024 * 1024;
        NV_ASSERT_OK_OR_RETURN(heapInfo(pHeap, &freeMem, &bytesTotal, &base,
                                        &offset, &size));

        //
        // offset is the starting address of biggest empty block whose size is
        // returned and we care about the base of largest empty block
        //
        base = offset;

        //
        // WAR - Bug-2383259 - TilL PMA is not enabled in vGPU-Host
        // we need to delay reserve some memory at the top to full fill lazy
        // allocations like FECS and GPCCS uCode. Leave 256MB at the top for
        // such lazy allocations
        //
        if (size > vgpuHeapWarSize)
        {
            size -= vgpuHeapWarSize;
        }
    }
    else
    {
        PMA_REGION_DESCRIPTOR *pFirstPmaRegionDesc = NULL;
        NvU32 numPmaRegions;
        NvU32 pmaConfig = PMA_QUERY_NUMA_ONLINED;

        NV_ASSERT_OK_OR_RETURN(pmaGetRegionInfo(&pHeap->pmaObject,
            &numPmaRegions, &pFirstPmaRegionDesc));

        base = pFirstPmaRegionDesc->base;
        pmaGetFreeMemory(&pHeap->pmaObject, &freeMem);
        pmaGetTotalMemory(&pHeap->pmaObject, &size);

        NV_ASSERT_OK(pmaQueryConfigs(&pHeap->pmaObject, &pmaConfig));

        //
        // MIG won't be used alongside APM and hence the check below is of no use
        // Even if we enable the check for APM the check will fail given that after
        // enabling "scrub on free" using virtual CE writes, memory gets consumed by
        // page tables backing the scrubber channel virtual mappings and hence the
        // calculation below no longer holds good
        // In case of HCC, structures like PB, GPFIFO and USERD for scrubber and golden
        // channels are required to be in CPR vidmem. This changes the calculation below
        // We can ignore this for the non-MIG case.
        //
        // When FB memory is onlined as NUMA node, kernel can directly alloc FB memory
        // and hence free memory can not be expected to be same as total memory.
        //
        if ((!gpuIsCCorApmFeatureEnabled(pGpu) || IS_MIG_ENABLED(pGpu)) &&
            !(pmaConfig & PMA_QUERY_NUMA_ONLINED))
        {
            NvU64 maxUsedPmaSize = 2 * RM_PAGE_SIZE_128K;
            //
            // PMA should be completely free at this point, otherwise we risk
            // not setting the right partitionable range (pmaGetLargestFree's
            // offset argument is not implemented as of this writing, so we
            // only get the base address of the region that contains it). There
            // is a known allocation from the top-level scrubber/CeUtils channel that
            // is expected to be no larger than 128K. Issue a warning for any
            // other uses.
            //
            if ((size > maxUsedPmaSize) &&
                (freeMem < (size - maxUsedPmaSize)))
            {
                NV_PRINTF(LEVEL_ERROR,
                    "Assumption that PMA is empty (after accounting for the top-level scrubber and CeUtils) is not met!\n");
                NV_PRINTF(LEVEL_ERROR,
                    "    free space = 0x%llx bytes, total space = 0x%llx bytes\n",
                    freeMem, size);
                NV_ASSERT_OR_RETURN(freeMem >= (size - maxUsedPmaSize),
                                    NV_ERR_INVALID_STATE);
            }
        }
    }

    if (size == 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "No partitionable memory. MIG memory partitioning can't be enabled.\n");
        return NV_OK;
    }

    if (base != bottomRsvdSize)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Partitionable memory start - 0x%llx not aligned with RM reserved "
                  "region base-end - 0x%llx\n", base, bottomRsvdSize);
        return NV_ERR_INVALID_STATE;
    }

    params.partitionableMemSize = size;
    params.bottomRsvdSize = bottomRsvdSize;
    params.topRsvdSize = topRsvdSize;

    // Call physical MemorySystem to align and program the partitionable range
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        pRmApi->Control(pRmApi,
            pGpu->hInternalClient,
            pGpu->hInternalSubdevice,
            NV2080_CTRL_CMD_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM,
            &params,
            sizeof(params)));

    pMemoryManager->MIGMemoryPartitioningInfo.partitionableMemoryRange =
        rangeMake(params.partitionableStartAddr, params.partitionableEndAddr);

    //
    // Make sure the created range is a valid range.
    // rangeIsEmpty checks lo > hi, which should be good enough to catch
    // inverted range case.
    //
    NV_ASSERT_OR_RETURN(!rangeIsEmpty(pMemoryManager->MIGMemoryPartitioningInfo.partitionableMemoryRange),
                        NV_ERR_INVALID_STATE);

    if (!kbusIsBar1Disabled(GPU_GET_KERNEL_BUS(pGpu)))
    {
        NV_ASSERT_OK_OR_RETURN(memmgrSetMIGPartitionableBAR1Range(pGpu, pMemoryManager));
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

        //
        // The Physical RM initializes its AMAPLIB context via
        // memsysSetPartitionableMem_HAL(). The GSP Client RM has a separate
        // AMAPLIB context that must also be initialized.
        //
        kmemsysReadMIGMemoryCfg_HAL(pGpu, pKernelMemorySystem);
    }

    return NV_OK;
}

NV_STATUS
memmgrFillComprInfo_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU64          pageSize,
    NvU32          pageCount,
    NvU32          kind,
    NvU64          surfOffset,
    NvU32          compTagStartOffset,
    COMPR_INFO    *pComprInfo
)
{
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    portMemSet(pComprInfo, 0, sizeof(*pComprInfo));

    pComprInfo->kind = kind;

    if (!memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, kind))
        return NV_OK;

    // TODO: We will have to support compression on vGPU HOST for AC
    NV_ASSERT(compTagStartOffset != ~(NvU32)0);

    pComprInfo->compPageShift = pMemorySystemConfig->comprPageShift;
    pComprInfo->compTagLineMin = compTagStartOffset;
    pComprInfo->compPageIndexLo = (NvU32)(surfOffset >> pComprInfo->compPageShift);
    pComprInfo->compPageIndexHi = (NvU32)((surfOffset + pageSize * pageCount - 1) >> pComprInfo->compPageShift);
    pComprInfo->compTagLineMultiplier = 1;

    return NV_OK;
}

NV_STATUS
memmgrGetKindComprForGpu_KERNEL
(
    MemoryManager      *pMemoryManager,
    MEMORY_DESCRIPTOR  *pMemDesc,
    OBJGPU             *pMappingGpu,
    NvU64               offset,
    NvU32              *pKind,
    COMPR_INFO         *pComprInfo
)
{
    NvU32               ctagId = FB_HWRESID_CTAGID_VAL_FERMI(memdescGetHwResId(pMemDesc));
    NvU32               kind   = memdescGetPteKindForGpu(pMemDesc, pMappingGpu);
    const MEMORY_SYSTEM_STATIC_CONFIG *pMappingMemSysConfig =
        kmemsysGetStaticConfig(pMappingGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pMappingGpu));

    // Compression is not supported on memory not backed by a GPU
    if (pMemDesc->pGpu != NULL && memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, kind) &&
        (ctagId == 0 || ctagId == FB_HWRESID_CTAGID_VAL_FERMI(-1)))
    {
        portMemSet(pComprInfo, 0, sizeof(*pComprInfo));

        pComprInfo->kind = kind;
        pComprInfo->compPageShift = pMappingMemSysConfig->comprPageShift;
        pComprInfo->bPhysBasedComptags = NV_TRUE;
        pComprInfo->compTagLineMin = 1;
    }
    else
    {
        if (ctagId == FB_HWRESID_CTAGID_VAL_FERMI(0xcdcdcdcd))
        {
            portMemSet(pComprInfo, 0, sizeof(*pComprInfo));

            pComprInfo->kind = memmgrGetUncompressedKind_HAL(pMappingGpu, pMemoryManager, kind, NV_TRUE);
        }
        else
        {
            memmgrFillComprInfoUncompressed(pMemoryManager, kind, pComprInfo);
        }
    }

    *pKind = pComprInfo->kind;

    return NV_OK;
}

NV_STATUS
memmgrGetKindComprFromMemDesc_IMPL
(
    MemoryManager     *pMemoryManager,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU64              offset,
    NvU32             *kind,
    COMPR_INFO        *pComprInfo
)
{
    return memmgrGetKindComprForGpu_HAL(pMemoryManager, pMemDesc, pMemDesc->pGpu,
                                        offset, kind, pComprInfo);
}

void
memmgrSetMIGPartitionableMemoryRange_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    NV_RANGE range
)
{
    pMemoryManager->MIGMemoryPartitioningInfo.partitionableMemoryRange = range;
}

NV_RANGE
memmgrGetMIGPartitionableMemoryRange_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    return pMemoryManager->MIGMemoryPartitioningInfo.partitionableMemoryRange;
}

/*
 * @brief Sets total partitionable BAR1
 */
NV_STATUS
memmgrSetMIGPartitionableBAR1Range_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    KernelBus  *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    OBJVASPACE *pBar1VAS   = kbusGetBar1VASpace_HAL(pGpu, pKernelBus);
    OBJEHEAP   *pVASHeap;
    NvU64 largestFreeOffset = 0;
    NvU64 largestFreeSize = 0;
    NvU64 partitionableBar1Start;
    NvU64 partitionableBar1End;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))
        return NV_OK;

    NV_ASSERT_OR_RETURN(pBar1VAS != NULL, NV_ERR_INVALID_STATE);
    pVASHeap = vaspaceGetHeap(pBar1VAS);

    // Get partitionable BAR1 range
    pVASHeap->eheapInfo(pVASHeap, NULL, NULL, &largestFreeOffset, &largestFreeSize, NULL, NULL);

    //
    // We are not considering alignment here because VA space is reserved/allocated in chunks of pages
    // so largestFreeOffset should be already aligned.
    //
    partitionableBar1Start = largestFreeOffset;
    partitionableBar1End = largestFreeOffset + largestFreeSize - 1;
    NV_ASSERT_OR_RETURN(partitionableBar1Start >= vaspaceGetVaStart(pBar1VAS), NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(partitionableBar1End <= vaspaceGetVaLimit(pBar1VAS), NV_ERR_INVALID_STATE);

    pMemoryManager->MIGMemoryPartitioningInfo.partitionableBar1Range = rangeMake(partitionableBar1Start, partitionableBar1End);
    return NV_OK;
}

NV_RANGE
memmgrGetMIGPartitionableBAR1Range_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    return pMemoryManager->MIGMemoryPartitioningInfo.partitionableBar1Range;
}

NV_STATUS
memmgrAllocMIGGPUInstanceMemory_VF
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU32          swizzId,
    NvHandle      *phMemory,
    NV_RANGE      *pAddrRange,
    Heap         **ppMemoryPartitionHeap
)
{
    // For vGpu we have a static memory allocation
    *phMemory = NV01_NULL_OBJECT;
    *pAddrRange = pMemoryManager->MIGMemoryPartitioningInfo.partitionableMemoryRange;
    *ppMemoryPartitionHeap = GPU_GET_HEAP(pGpu);

    return NV_OK;
}

// Function to allocate memory for a GPU instance
NV_STATUS
memmgrAllocMIGGPUInstanceMemory_PF
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU32          swizzId,
    NvHandle      *phMemory,
    NV_RANGE      *pAddrRange,
    Heap         **ppMemoryPartitionHeap
)
{
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NV_STATUS rmStatus = NV_OK;
    NvHandle hMemory = 0;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvBool bNumaEnabled = osNumaOnliningEnabled(pGpu->pOsGpuInfo);

    NV_ASSERT_OR_RETURN(pKernelMIGManager != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OK_OR_RETURN(kmemsysGetMIGGPUInstanceMemInfo(pGpu, pKernelMemorySystem, swizzId, pAddrRange));

    //
    // Only allocate memory for non swizzID-0 GPU instances as swizzID-0 owns full
    // gpu and there is no need to pre-reserve memory for that and non
    // coherent systems. In coherent NUMA systems, NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE
    // is not supported and the memory comes from the MIG partition memory
    // NUMA node.
    //
    if (kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, swizzId))
    {
        if(bNumaEnabled)
        {
            NvS32 numaNodeId;
            NvU64 partitionBaseAddr = pAddrRange->lo;
            NvU64 partitionSize = rangeLength(*pAddrRange);

            if (kmigmgrGetSwizzIdInUseMask(pGpu, pKernelMIGManager) == 0x0)
            {
                // Remove swizz Id 0 / baremetal GPU memory NUMA node
                pmaNumaOfflined(&GPU_GET_HEAP(pGpu)->pmaObject);
                kmemsysNumaRemoveMemory_HAL(pGpu, pKernelMemorySystem, 0);
            }

            //
            // The memory gets removed in memmgrFreeMIGGPUInstanceMemory if
            // there is any failure after adding the memory.
            //
            NV_ASSERT_OK_OR_RETURN(kmemsysNumaAddMemory_HAL(pGpu,
                                                            pKernelMemorySystem,
                                                            swizzId,
                                                            partitionBaseAddr,
                                                            partitionSize,
                                                            &numaNodeId));
        }
        else
        {
            //
            // Allocate memory using vidHeapControl
            //
            // vidHeapControl calls should happen outside GPU locks
            // This is a PMA requirement as memory allocation calls may invoke eviction
            // which UVM could get stuck behind GPU lock
            // See Bug 1735851-#24
            //
            rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

            // Allocate gpfifo entries
            NV_MEMORY_ALLOCATION_PARAMS memAllocParams;
            portMemSet(&memAllocParams, 0, sizeof(NV_MEMORY_ALLOCATION_PARAMS));
            memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
            memAllocParams.type      = NVOS32_TYPE_IMAGE;
            memAllocParams.size      = rangeLength(*pAddrRange);
            memAllocParams.attr      = DRF_DEF(OS32, _ATTR, _LOCATION, _VIDMEM);
            memAllocParams.attr     |= DRF_DEF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS);
            memAllocParams.attr     |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _DEFAULT);
            memAllocParams.attr2     = DRF_DEF(OS32, _ATTR2, _PAGE_OFFLINING, _OFF); // free the offlined pages
            memAllocParams.flags    |= NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
            memAllocParams.rangeLo   = 0;
            memAllocParams.rangeHi   = 0;
            memAllocParams.offset    = pAddrRange->lo; // Offset needed if fixed address allocation
            memAllocParams.hVASpace  = 0; // Physical allocation
            memAllocParams.internalflags = NVOS32_ALLOC_INTERNAL_FLAGS_SKIP_SCRUB;

            rmStatus = pRmApi->Alloc(pRmApi,
                                     pMemoryManager->MIGMemoryPartitioningInfo.hClient,
                                     pMemoryManager->MIGMemoryPartitioningInfo.hSubdevice,
                                     &hMemory,
                                     NV01_MEMORY_LOCAL_USER,
                                     &memAllocParams,
                                     sizeof(memAllocParams));

            // Reaquire the GPU locks
            if (rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_MEM) != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "failed to grab RM-Lock\n");
                DBG_BREAKPOINT();
                rmStatus = NV_ERR_GENERIC;
                goto cleanup;
            }

            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Unable to allocate physical memory for GPU instance.\n");
                return rmStatus;
            }
        }
    }
    rmStatus = _memmgrInitMIGMemoryPartitionHeap(pGpu, pMemoryManager, swizzId, pAddrRange, ppMemoryPartitionHeap);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to initialize memory partition heap\n");
        goto cleanup;
    }

    NV_PRINTF(LEVEL_INFO,
              "Allocated memory partition heap for swizzId - %d with StartAddr - 0x%llx, endAddr - 0x%llx.\n",
              swizzId, pAddrRange->lo, pAddrRange->hi);

    *phMemory = hMemory;
    return rmStatus;

cleanup:
    pRmApi->Free(pRmApi, pMemoryManager->MIGMemoryPartitioningInfo.hClient, hMemory);

    return rmStatus;
}

// Function to initialize heap for managing MIG partition memory
static NV_STATUS
_memmgrInitMIGMemoryPartitionHeap
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU32          swizzId,
    NV_RANGE      *pAddrRange,
    Heap         **ppMemoryPartitionHeap
)
{
    NV_STATUS status = NV_OK;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    Heap *pMemoryPartitionHeap = NULL;
    NvBool bNumaEnabled = osNumaOnliningEnabled(pGpu->pOsGpuInfo);
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NvU64 partitionBaseAddr = pAddrRange->lo;
    NvU64 partitionSize = rangeLength(*pAddrRange);

    // Use default heap for swizzID-0 as we don't prereserve memory for swizzID-0
    NV_ASSERT_OR_RETURN(pKernelMIGManager != NULL, NV_ERR_INVALID_STATE);
    if (!kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, swizzId))
    {
        *ppMemoryPartitionHeap = pMemoryManager->pHeap;
        return NV_OK;
    }
    else
    {
        *ppMemoryPartitionHeap  = NULL;
    }

    NV_ASSERT_OK_OR_GOTO(
        status,
        objCreate(ppMemoryPartitionHeap, pMemoryManager, Heap),
        fail);

    pMemoryPartitionHeap = *ppMemoryPartitionHeap;

    if (memmgrIsPmaEnabled(pMemoryManager) &&
        memmgrIsPmaSupportedOnPlatform(pMemoryManager))
    {
        portMemSet(&pMemoryPartitionHeap->pmaObject, 0, sizeof(pMemoryPartitionHeap->pmaObject));
        NV_ASSERT_OK_OR_GOTO(
            status,
            memmgrPmaInitialize(pGpu, pMemoryManager, &pMemoryPartitionHeap->pmaObject),
            fail);

        if (bNumaEnabled)
        {
            NV_ASSERT_OR_GOTO(pKernelMemorySystem->memPartitionNumaInfo[swizzId].bInUse, fail);
            partitionBaseAddr = pKernelMemorySystem->memPartitionNumaInfo[swizzId].offset;
            partitionSize = pKernelMemorySystem->memPartitionNumaInfo[swizzId].size;

            //
            // The base and size passed here is the FB base and size and
            // not the partition's. pmaNumaOnlined requires the FB base and
            // size to convert between FB local address and SPA.
            // memmgrPmaRegisterRegions is where the partition's base and size
            // is reported to PMA.
            //
            NV_ASSERT_OK_OR_GOTO(
                status,
                pmaNumaOnlined(&pMemoryPartitionHeap->pmaObject,
                               pKernelMemorySystem->memPartitionNumaInfo[swizzId].numaNodeId,
                               pKernelMemorySystem->coherentCpuFbBase,
                               pKernelMemorySystem->numaOnlineSize),
                               fail);
        }
    }

    NV_ASSERT_OK_OR_GOTO(
        status,
        heapInit(pGpu, pMemoryPartitionHeap, partitionBaseAddr,
                 partitionSize,
                 HEAP_TYPE_PARTITION_LOCAL,
                 GPU_GFID_PF,
                 NULL),
        fail);

    if (memmgrIsPmaInitialized(pMemoryManager) &&
        (pMemoryPartitionHeap->bHasFbRegions))
    {
        NV_ASSERT_OK_OR_GOTO(
            status,
            memmgrPmaRegisterRegions(pGpu, pMemoryManager, pMemoryPartitionHeap,
                                     &pMemoryPartitionHeap->pmaObject),
            fail);
    }

    if (!IsSLIEnabled(pGpu))
    {
        // Do the actual blacklisting of pages from the heap
        if (pMemoryPartitionHeap->blackListAddresses.count != 0)
        {
            status = heapBlackListPages(pGpu, pMemoryPartitionHeap);

            if (status != NV_OK)
            {
                // Warn and continue
                NV_PRINTF(LEVEL_WARNING, "Error 0x%x creating blacklist\n",
                          status);
            }
        }
    }

    return NV_OK;

fail:

    if (pMemoryPartitionHeap != NULL)
    {
        objDelete(pMemoryPartitionHeap);
        *ppMemoryPartitionHeap = NULL;
    }

    return status;
}

// Function to free GPU instance memory
NV_STATUS
memmgrFreeMIGGPUInstanceMemory_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    NvU32 swizzId,
    NvHandle hMemory,
    Heap **ppMemoryPartitionHeap
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NvBool bNumaEnabled = osNumaOnliningEnabled(pGpu->pOsGpuInfo);

    NV_ASSERT_OR_RETURN(pKernelMIGManager != NULL, NV_ERR_INVALID_STATE);

    // Nothing to do for swizzId 0 as we neither allocate memory nor allocate new heap object
    if (!kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, swizzId))
        return NV_OK;

    objDelete(*ppMemoryPartitionHeap);
    *ppMemoryPartitionHeap = NULL;

    if (bNumaEnabled)
    {
        kmemsysNumaRemoveMemory_HAL(pGpu, pKernelMemorySystem, swizzId);

        if (kmigmgrGetSwizzIdInUseMask(pGpu, pKernelMIGManager) == 0x0)
        {
            NvS32 numaNodeId;

            // Add back the baremetal GPU memory NUMA node.
            NV_ASSERT_OK_OR_RETURN(kmemsysNumaAddMemory_HAL(pGpu,
                                     pKernelMemorySystem,
                                     0,
                                     pKernelMemorySystem->numaOnlineBase,
                                     pKernelMemorySystem->numaOnlineSize,
                                     &numaNodeId));
            // Baremetal NUMA node id should be same as pGpu->numaNodeId
            NV_ASSERT_OR_RETURN(numaNodeId == pGpu->numaNodeId, NV_ERR_INVALID_STATE);
            NV_ASSERT_OK_OR_RETURN(pmaNumaOnlined(&GPU_GET_HEAP(pGpu)->pmaObject,
                                                  pGpu->numaNodeId,
                                                  pKernelMemorySystem->coherentCpuFbBase,
                                                  pKernelMemorySystem->numaOnlineSize));
        }
    }

    // Free allocated memory
    if (!bNumaEnabled && (hMemory != NV01_NULL_OBJECT))
    {
        pRmApi->Free(pRmApi, pMemoryManager->MIGMemoryPartitioningInfo.hClient, hMemory);
    }
    return NV_OK;
}

void memmgrComprInfoDisableCompression_IMPL
(
    MemoryManager *pMemoryManager,
    COMPR_INFO    *pComprInfo
)
{
    memmgrFillComprInfoUncompressed(pMemoryManager, pComprInfo->kind, pComprInfo);
}

void memmgrFillComprInfoUncompressed_IMPL
(
    MemoryManager *pMemoryManager,
    NvU32 kind,
    COMPR_INFO *pComprInfo
)
{
    if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, kind))
        kind = memmgrGetUncompressedKind_HAL(ENG_GET_GPU(pMemoryManager), pMemoryManager, kind, NV_FALSE);

    portMemSet(pComprInfo, 0, sizeof(*pComprInfo));
    pComprInfo->kind = kind;
}

/*!
 * @brief   Creates the SW state of the page level pools.
 *
 * @param   pGpu
 * @param   pMemoryManager
 *
 * @returns On success, returns NV_OK.
 *          On failure, returns error code.
 */
NV_STATUS
memmgrPageLevelPoolsCreate_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NV_STATUS status = NV_OK;

    if (RMCFG_FEATURE_PMA &&
        memmgrIsPmaInitialized(pMemoryManager) &&
        memmgrAreClientPageTablesPmaManaged(pMemoryManager))
    {
        Heap           *pHeap       = GPU_GET_HEAP(pGpu);
        KernelGmmu     *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
        const GMMU_FMT *pFmt        = NULL;

        pFmt = kgmmuFmtGet(pKernelGmmu, GMMU_FMT_VERSION_DEFAULT, 0);
        NV_ASSERT_OR_RETURN(NULL != pFmt, NV_ERR_INVALID_ARGUMENT);

        status = rmMemPoolSetup((void *)&pHeap->pmaObject, &pMemoryManager->pPageLevelReserve,
                                    (pFmt->version == GMMU_FMT_VERSION_1) ? POOL_CONFIG_GMMU_FMT_1 : POOL_CONFIG_GMMU_FMT_2);

        NV_ASSERT(NV_OK == status);

        //
        // Allocate the pool in CPR in case of Confidential Compute
        // When Hopper Confidential Compute is enabled, page tables
        // cannot be in non-CPR region
        //
        if (gpuIsCCFeatureEnabled(pGpu) && (status == NV_OK))
        {
            rmMemPoolAllocateProtectedMemory(pMemoryManager->pPageLevelReserve, NV_TRUE);
        }
    }
    return status;
}

/*!
 * @brief   Destroys the SW state of the page level pools.
 *
 * @param   pGpu
 * @param   pMemoryManager
 *
 * @returns
 */
void
memmgrPageLevelPoolsDestroy_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    if (RMCFG_FEATURE_PMA &&
        memmgrIsPmaInitialized(pMemoryManager) &&
        memmgrAreClientPageTablesPmaManaged(pMemoryManager))
    {
        rmMemPoolDestroy(pMemoryManager->pPageLevelReserve);
        pMemoryManager->pPageLevelReserve = NULL;
    }
}

/*!
 * @brief   Gets page level pool to use
 *
 * @param       pGpu
 * @param       pMemoryManager
 * @param[in]   hClient         client handle
 * @param[out]  ppMemPoolInfo   page level pool
 *
 * @returns On success, returns NV_OK.
 *          On failure, returns error code.
 */
NV_STATUS
memmgrPageLevelPoolsGetInfo_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    Device        *pDevice,
    RM_POOL_ALLOC_MEM_RESERVE_INFO **ppMemPoolInfo
)
{
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvBool bMemPartitioningEnabled = (pKernelMIGManager != NULL) && kmigmgrIsMIGMemPartitioningEnabled(pGpu, pKernelMIGManager);
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemPool = NULL;
    NV_ASSERT_OR_RETURN(ppMemPoolInfo != NULL, NV_ERR_INVALID_ARGUMENT);

    if (!memmgrIsPmaInitialized(pMemoryManager) ||
        !memmgrAreClientPageTablesPmaManaged(pMemoryManager))
    {
        return NV_ERR_INVALID_STATE;
    }

    // If memory partitioning is enabled, then use per-partition pool allocator
    if (bMemPartitioningEnabled)
    {
        MIG_INSTANCE_REF ref;
        NV_ASSERT_OK_OR_RETURN(
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref));
        pMemPool = ref.pKernelMIGGpuInstance->pPageTableMemPool;
    }
    else
    {
        pMemPool = pMemoryManager->pPageLevelReserve;
    }
    NV_ASSERT_OR_RETURN(pMemPool != NULL, NV_ERR_INVALID_STATE);

    *ppMemPoolInfo = pMemPool;
    return NV_OK;
}

static inline void
_memmgrPmaStatsUpdateCb
(
    void *pCtx,
    NvU64 freeFrames
)
{
    OBJGPU *pGpu = (OBJGPU *) pCtx;
    RUSD_PMA_MEMORY_INFO *pSharedData;

    NV_ASSERT_OR_RETURN_VOID(pGpu != NULL);

    pSharedData = gpushareddataWriteStart(pGpu, pmaMemoryInfo);
    pSharedData->freePmaMemory = freeFrames << PMA_PAGE_SHIFT;
    gpushareddataWriteFinish(pGpu, pmaMemoryInfo);
}

static void
_memmgrInitRUSDHeapSize
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    RUSD_PMA_MEMORY_INFO  *pSharedData;
    KernelMemorySystem  *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NvU64                bytesTotal = 0;
    PMA                 *pPma;

    NV_ASSERT_OR_RETURN_VOID(memmgrIsPmaInitialized(pMemoryManager));

    pPma = &pMemoryManager->pHeap->pmaObject;
    pmaGetTotalMemory(pPma, &bytesTotal);
    bytesTotal -= ((NvU64)pKernelMemorySystem->fbOverrideStartKb << 10);

    pSharedData = gpushareddataWriteStart(pGpu, pmaMemoryInfo);
    pSharedData->totalPmaMemory = bytesTotal;
    gpushareddataWriteFinish(pGpu, pmaMemoryInfo);
}

/*!
 * @brief Initialize the PMA object
 *
 * @param       pGpu
 * @param       pMemoryManager
 * @param[in]   pPma         Pointer to the PMA object to init
 *
 * @returns On success, returns NV_OK.
 *          On failure, returns error code.
 */
NV_STATUS
memmgrPmaInitialize_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    PMA           *pPma
)
{
    NvU32 pmaInitFlags = PMA_INIT_NONE;
    NV_STATUS status = NV_OK;
    NvBool bNumaEnabled = osNumaOnliningEnabled(pGpu->pOsGpuInfo);

    NV_ASSERT(memmgrIsPmaEnabled(pMemoryManager) &&
              memmgrIsPmaSupportedOnPlatform(pMemoryManager));

    if (memmgrIsPmaForcePersistence(pMemoryManager))
    {
        pmaInitFlags |= PMA_INIT_FORCE_PERSISTENCE;
    }

    if (memmgrIsScrubOnFreeEnabled(pMemoryManager))
    {
        pmaInitFlags |= PMA_INIT_SCRUB_ON_FREE;
    }

    // Disable client page table management on SLI.
    if (IsSLIEnabled(pGpu))
    {
        memmgrSetClientPageTablesPmaManaged(pMemoryManager, NV_FALSE);
    }

    if (bNumaEnabled)
    {
        NV_PRINTF(LEVEL_INFO, "Initializing PMA with NUMA flag.\n");
        pmaInitFlags |= PMA_INIT_NUMA;

        if (gpuIsSelfHosted(pGpu))
        {
            NV_PRINTF(LEVEL_INFO, "Initializing PMA with NUMA_AUTO_ONLINE flag.\n");
            pmaInitFlags |= PMA_INIT_NUMA_AUTO_ONLINE;
        }
    }

    status = pmaInitialize(pPma, pmaInitFlags);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to initialize PMA!\n");
        return status;
    }

    pmaRegisterUpdateStatsCb(pPma, _memmgrPmaStatsUpdateCb, pGpu);

    if (bNumaEnabled)
    {
        KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
        NvU32 numaSkipReclaimVal = NV_REG_STR_RM_NUMA_ALLOC_SKIP_RECLAIM_PERCENTAGE_DEFAULT;

        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_NUMA_ALLOC_SKIP_RECLAIM_PERCENTAGE, &numaSkipReclaimVal) == NV_OK)
        {
            if (numaSkipReclaimVal > NV_REG_STR_RM_NUMA_ALLOC_SKIP_RECLAIM_PERCENTAGE_MAX)
            {
                numaSkipReclaimVal = NV_REG_STR_RM_NUMA_ALLOC_SKIP_RECLAIM_PERCENTAGE_MAX;
            }
        }
        pmaNumaSetReclaimSkipThreshold(pPma, numaSkipReclaimVal);

        // Full FB memory is added and onlined already
        if (pKernelMemorySystem->memPartitionNumaInfo[0].bInUse)
        {
            NV_ASSERT_OK_OR_RETURN(pmaNumaOnlined(pPma, pGpu->numaNodeId,
                                                  pKernelMemorySystem->coherentCpuFbBase,
                                                  pKernelMemorySystem->numaOnlineSize));
        }
    }

    return NV_OK;
}

NV_STATUS
memmgrInitFbRegions_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NV_ASSERT_OR_RETURN(pMemoryManager->Ram.numFBRegions == 0, NV_ERR_INVALID_STATE);

    // Don't setup regions if FB is broken and we aren't using L2 cache as "FB".
    if ((pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) &&
         !gpuIsCacheOnlyModeEnabled(pGpu)))
    {
        //
        // Bug 594534: Don't read/write in the FBIO/FBPA space when FB is broken.
        // Indicate 32MB FB Memory instead, which is a bit of a hack since zero
        // would be more accurate, but zero breaks things.
        //

        // When ZeroFB + L2Cache mode is enabled, we'll set fbAddrSpaceSizeMb
        // appropriately in memmgrInitBaseFbRegions_HAL.
        if (!gpuIsCacheOnlyModeEnabled(pGpu))
        {
            pMemoryManager->Ram.mapRamSizeMb = pMemoryManager->Ram.fbAddrSpaceSizeMb = 32;
            NV_PRINTF(LEVEL_ERROR,
                      "Bug 594534: HACK: Report 32MB of framebuffer instead of reading registers.\n");

        }

        return NV_OK;
    }

    NV_ASSERT_OK_OR_RETURN(memmgrInitBaseFbRegions_HAL(pGpu, pMemoryManager));

    NV_ASSERT_OK_OR_RETURN(memmgrInitFbRegionsHal_HAL(pGpu, pMemoryManager));

    //
    // Build a list of regions sorted by allocation priority
    // (highest to lowest). Used for allocations using ObjHeap.
    //
    memmgrRegenerateFbRegionPriority(pGpu, pMemoryManager);

    if (RMCFG_FEATURE_PLATFORM_WINDOWS)
    {
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_EXTERNAL_HEAP_CONTROL))
        {
            // KMD in WDDM mode
            if (pMemoryManager->bMixedDensityFbp)
            {
                //
                // For mixed memory on LDDM platforms, when we are using kernel-managed
                // heap (not TCC mode), we want to prefer allocating in slow memory to conserve
                // fast memory for applications.
                //
                pMemoryManager->bPreferSlowRegion = NV_TRUE;
            }
        }
    }

    NV_ASSERT_OK_OR_RETURN(memmgrSetPlatformPmaSupport(pGpu, pMemoryManager));

    return NV_OK;
}

/*!
 * @brief Register regions to the PMA object
 *
 * @param       pGpu
 * @param       pMemoryManager
 * @param[in]   pPma         Pointer to the PMA object to register with
 *
 * @returns On success, returns NV_OK.
 *          On failure, returns error code.
 */
NV_STATUS
memmgrPmaRegisterRegions_IMPL
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    Heap          *pHeap,
    PMA           *pPma
)
{
    HEAP_TYPE_INTERNAL heapType = pHeap->heapType;
    PMA_REGION_DESCRIPTOR pmaRegion;
    NvU32 pmaRegionIdx = 0;
    NvU32 i;
    PMA_BLACKLIST_ADDRESS *pBlacklistPages = NULL;
    NvU32 blRegionCount = 0;
    NvU32 blPageIndex;
    NvU32 blackListCount;
    NvU64 base, size;
    NV_STATUS status = NV_OK;
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

    blackListCount = pHeap->blackListAddresses.count;
    base = pHeap->base;
    size = pHeap->total;

    //
    // If there are blacklisted pages, prepare a staging buffer to pass the
    // per-region blacklisted pages to PMA
    //
    if (blackListCount > 0)
    {
        pBlacklistPages = portMemAllocNonPaged(
                            sizeof(PMA_BLACKLIST_ADDRESS) * blackListCount);
        if (pBlacklistPages == NULL)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not allocate memory for blackList!\n");
            status = NV_ERR_NO_MEMORY;
            goto _pmaInitFailed;
        }
    }

    for (i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
    {
        //
        // Skip all regions that are completely outside the heap boundry
        // OR marked as internal(used for internal RM allocations)
        // OR marked as reserved(used for console, display, link training buffer etc.)
        //
        if ((pMemoryManager->Ram.fbRegion[i].limit < base ||
             pMemoryManager->Ram.fbRegion[i].base >= (base + size)) ||
            (pMemoryManager->Ram.fbRegion[i].bInternalHeap) ||
            (pMemoryManager->Ram.fbRegion[i].bRsvdRegion))
        {
            continue;
        }

        NV_PRINTF(LEVEL_INFO,
                  "PMA: Register FB region[%d] %llx..%llx EXTERNAL\n", i,
                  pMemoryManager->Ram.fbRegion[i].base, pMemoryManager->Ram.fbRegion[i].limit);

        pmaRegion.base              = pMemoryManager->Ram.fbRegion[i].base;
        pmaRegion.limit             = pMemoryManager->Ram.fbRegion[i].limit;

        // Check if the base of managed memory is not based at FB region base.
        if (pmaRegion.base < base)
        {
            pmaRegion.base = base;
        }

        // check if limit of managed memory is less than FB region limit
        if (pmaRegion.limit >= (base + size))
        {
            pmaRegion.limit = base + size - 1;
        }

        pmaRegion.performance        = pMemoryManager->Ram.fbRegion[i].performance;
        pmaRegion.bSupportCompressed = pMemoryManager->Ram.fbRegion[i].bSupportCompressed;
        pmaRegion.bSupportISO        = pMemoryManager->Ram.fbRegion[i].bSupportISO;
        pmaRegion.bProtected         = pMemoryManager->Ram.fbRegion[i].bProtected;

        //
        // Now we know the region, find if it has any blacklisted pages
        // TODO: Try to coalesce to unique 64K pages
        //
        blRegionCount = 0;
        if (pBlacklistPages != NULL)
        {
            for (blPageIndex = 0; blPageIndex < blackListCount; blPageIndex++)
            {
                if ((pHeap->blackListAddresses.data[blPageIndex].address
                            != NV2080_CTRL_FB_OFFLINED_PAGES_INVALID_ADDRESS) &&
                    (pHeap->blackListAddresses.data[blPageIndex].address >= pmaRegion.base) &&
                    (pHeap->blackListAddresses.data[blPageIndex].address <= pmaRegion.limit))
                {
                    // Collect the region's blacklisted pages
                    pBlacklistPages[blRegionCount].physOffset = pHeap->blackListAddresses.data[blPageIndex].address;

                    pBlacklistPages[blRegionCount].bIsDynamic =
                            ((pHeap->blackListAddresses.data[blPageIndex].type ==
                                NV2080_CTRL_FB_OFFLINED_PAGES_SOURCE_DPR_MULTIPLE_SBE) ||
                            (pHeap->blackListAddresses.data[blPageIndex].type ==
                                NV2080_CTRL_FB_OFFLINED_PAGES_SOURCE_DPR_DBE));

                    blRegionCount++;
                }
            }
        }

        NV_PRINTF(LEVEL_INFO,
                  "Register FB region %llx..%llx of size %llx with PMA\n",
                  pmaRegion.base, pmaRegion.limit,
                  pmaRegion.limit - pmaRegion.base + 1);
        //
        // Register the region for PMA management, and note if asynchronous
        // scrubbing is enabled.  Synchronous scrubbing is done before
        // heap/PMA is initialized, but asynchronously scrubbed pages will
        // need to be unmarked once they are scrubbed.
        //
        status = pmaRegisterRegion(pPma, pmaRegionIdx,
                    memmgrEccScrubInProgress_HAL(pGpu, pMemoryManager),
                    &pmaRegion, blRegionCount,
                    ((blRegionCount==0) ? NULL : pBlacklistPages));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to register FB region %llx..%llx with PMA\n",
                      pmaRegion.base, pmaRegion.limit);
            DBG_BREAKPOINT();
            goto _pmaInitFailed;
        }
        pmaRegionIdx++;
    }

    //
    // bug #200354346, make sure the RM reserved region(s) are
    // scrubbed during the region creation itself. Top Down scrubber,
    // skips the RM reserved region(s) because the assumption is, they
    // are pre-scrubbed.
    //
    if (heapType != HEAP_TYPE_PARTITION_LOCAL)
        memmgrScrubInternalRegions_HAL(pGpu, pMemoryManager);

_pmaInitFailed:
    portMemFree(pBlacklistPages);

    if ((status == NV_OK) && (pKernelMemorySystem->fbOverrideStartKb != 0))
    {
        NvU64 allocSize = NV_ALIGN_UP(((NvU64)pKernelMemorySystem->fbOverrideStartKb << 10), PMA_GRANULARITY);
        NvU32 numPages  = (NvU32)(allocSize >> PMA_PAGE_SHIFT);
        PMA_ALLOCATION_OPTIONS allocOptions = {0};

        allocOptions.flags     = PMA_ALLOCATE_CONTIGUOUS;
        allocOptions.flags    |= PMA_ALLOCATE_SPECIFY_ADDRESS_RANGE;
        allocOptions.physBegin = 0;
        allocOptions.physEnd   = allocSize - 1;

        // This is intentionally thrown away
        NvU64 *pPages = NULL;
        pPages = portMemAllocNonPaged(numPages * sizeof(NvU64));
        if (pPages != NULL)
        {
            // Accommodate the regkey override for FB start
            status = pmaAllocatePages(pPma, numPages, _PMA_64KB, &allocOptions, pPages);
            portMemFree(pPages);
        }
    }
    if (status != NV_OK)
    {
        if (memmgrIsPmaInitialized(pMemoryManager))
        {
            if (heapType != HEAP_TYPE_PARTITION_LOCAL)
            {
                memmgrSetPmaInitialized(pMemoryManager, NV_FALSE);
            }
            pmaDestroy(pPma);
        }
    }

    return status;
}

/*!
 * @brief Allocate internal handles for MIG partition memory allocation
 */
NV_STATUS
memmgrAllocMIGMemoryAllocationInternalHandles_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    NV_ASSERT_OR_RETURN(pMemoryManager->MIGMemoryPartitioningInfo.hClient == NV01_NULL_OBJECT, NV_ERR_INVALID_STATE);
    NV_ASSERT_OK_OR_RETURN(
        rmapiutilAllocClientAndDeviceHandles(pRmApi, pGpu,
                                             &pMemoryManager->MIGMemoryPartitioningInfo.hClient,
                                             &pMemoryManager->MIGMemoryPartitioningInfo.hDevice,
                                             &pMemoryManager->MIGMemoryPartitioningInfo.hSubdevice));

    return NV_OK;
}

/*!
 * @brief Free internal handles used to support MIG memory partitioning
 */
void
memmgrFreeMIGMemoryAllocationInternalHandles_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    rmapiutilFreeClientAndDeviceHandles(pRmApi,
                                        &pMemoryManager->MIGMemoryPartitioningInfo.hClient,
                                        &pMemoryManager->MIGMemoryPartitioningInfo.hDevice,
                                        &pMemoryManager->MIGMemoryPartitioningInfo.hSubdevice);
}

/*!
 * @brief Gets free memory (client visible) for all valid GPU instances
 */
void
memmgrGetFreeMemoryForAllMIGGPUInstances_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    NvU64 *pBytes
)
{
    NvU64 val = 0;
    Heap *pHeap = NULL;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance;

    *pBytes = 0;

    FOR_EACH_VALID_GPU_INSTANCE(pGpu, pKernelMIGManager, pKernelMIGGPUInstance)
    {
        NV_ASSERT(pKernelMIGGPUInstance->pMemoryPartitionHeap != NULL);
        pHeap = pKernelMIGGPUInstance->pMemoryPartitionHeap;

        if (memmgrIsPmaInitialized(pMemoryManager))
            pmaGetFreeMemory(&pHeap->pmaObject, &val);
        else
            heapGetFree(pHeap, &val);

        *pBytes += val;
    }
    FOR_EACH_VALID_GPU_INSTANCE_END();
}

/*!
 * @brief Gets total memory for all valid GPU instances
 *
 * @param       pGpu
 * @param       pMemoryManager
 * @param[out]  pBytes          pointer to the total memory
 *
 */
void
memmgrGetTotalMemoryForAllMIGGPUInstances_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    NvU64 *pBytes
)
{
    NvU64 val = 0;
    Heap *pHeap = NULL;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance;

    *pBytes = 0;

    FOR_EACH_VALID_GPU_INSTANCE(pGpu, pKernelMIGManager, pKernelMIGGPUInstance)
    {
        NV_ASSERT(pKernelMIGGPUInstance->pMemoryPartitionHeap != NULL);
        pHeap = pKernelMIGGPUInstance->pMemoryPartitionHeap;

        if (memmgrIsPmaInitialized(pMemoryManager))
            pmaGetTotalMemory(&pHeap->pmaObject, &val);
        else
            heapGetSize(pHeap, &val);

        *pBytes += val;
    }
    FOR_EACH_VALID_GPU_INSTANCE_END();
}

void
memmgrGetTopLevelScrubberStatus_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    NvBool *pbTopLevelScrubberEnabled,
    NvBool *pbTopLevelScrubberConstructed
)
{
    NvBool bTopLevelScrubberEnabled = NV_FALSE;
    NvBool bTopLevelScrubberConstructed = NV_FALSE;
    NvU32 pmaConfigs = PMA_QUERY_SCRUB_ENABLED | PMA_QUERY_SCRUB_VALID;

    if (memmgrIsPmaInitialized(pMemoryManager))
    {
        Heap *pHeap = GPU_GET_HEAP(pGpu);
        NV_ASSERT_OK(pmaQueryConfigs(&pHeap->pmaObject, &pmaConfigs));
        bTopLevelScrubberEnabled = (pmaConfigs & PMA_QUERY_SCRUB_ENABLED) != 0x0;
        bTopLevelScrubberConstructed = (pmaConfigs & PMA_QUERY_SCRUB_VALID) != 0x0;
    }

    if (pbTopLevelScrubberEnabled != NULL)
        *pbTopLevelScrubberEnabled = bTopLevelScrubberEnabled;
    if (pbTopLevelScrubberConstructed != NULL)
        *pbTopLevelScrubberConstructed = bTopLevelScrubberConstructed;
}

/*!
 * @brief       Return the full address range for the partition assigend for the vGPU.
 *
 * @param[in]   pGpu
 * @param[in]   pMemoryManager
 * @param[out]  base           reference to the base address of the partition
 * @param[out]  size           reference to the overall size of the partition
 */
static void
_memmgrGetFullMIGAddrRange
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    NvU64 *base,
    NvU64 *size
)
{
    NvU32 i;
    NvU64 lo, hi;

    *base = 0;
    *size = 0;
    if (pMemoryManager->Ram.numFBRegions == 0)
    {
        return;
    }

    lo = pMemoryManager->Ram.fbRegion[0].base;
    hi = pMemoryManager->Ram.fbRegion[0].limit;

    for (i = 1; i < pMemoryManager->Ram.numFBRegions; i++)
    {
        if (pMemoryManager->Ram.fbRegion[i].base < lo)
        {
            lo = pMemoryManager->Ram.fbRegion[i].base;
        }

        if (pMemoryManager->Ram.fbRegion[i].limit > hi)
        {
            hi = pMemoryManager->Ram.fbRegion[i].limit;
        }
    }

    *base = lo;
    *size = hi - lo + 1;
}

/*!
 * @brief Discover MIG partitionable memory range based on PMA status
 */
NV_STATUS
memmgrDiscoverMIGPartitionableMemoryRange_VF
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    NV_RANGE *pMemoryRange
)
{
    NvU64 size;
    NvU64 base;

    // Set memory information
    if (!memmgrIsPmaInitialized(pMemoryManager))
    {
        Heap *pHeap = GPU_GET_HEAP(pGpu);
        NvU64 freeMem;
        NvU64 bytesTotal;
        NvU64 offset;

        NV_ASSERT_OK_OR_RETURN(heapInfo(pHeap, &freeMem, &bytesTotal, &base,
                                        &offset, &size));

        //
        // offset is the starting address of biggest empty block whose size is
        // returned and we care about the base of largest empty block
        //
        base = offset;
    }
    else
    {
        //
        // In the case of vGPU, pmaGetLargestFree only returns the user-visible
        // PMA region and not the reserved/internal regions that constitute the
        // overall partition size assigned to the vGPU.
        // This is misleading as pMemoryManager->partitionableMemoryRange is expected to
        // represent the actual partition size.
        //
        _memmgrGetFullMIGAddrRange(pGpu, pMemoryManager, &base, &size);
    }

    *pMemoryRange = rangeMake(base, base + size - 1);

    return NV_OK;
}

NV_STATUS
memmgrAllocReservedFBRegionMemdesc_IMPL
(
    OBJGPU                       *pGpu,
    MemoryManager                *pMemoryManager,
    MEMORY_DESCRIPTOR           **ppMemdesc,
    NvU64                         rangeStart,
    NvU64                         allocSize,
    NvU64                         memdescFlags,
    NV_FB_ALLOC_RM_INTERNAL_OWNER allocTag
)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(ppMemdesc != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OK_OR_GOTO(status,
        memdescCreate(ppMemdesc, pGpu, allocSize,
                            RM_PAGE_SIZE, NV_TRUE, ADDR_FBMEM,
                            NV_MEMORY_UNCACHED, memdescFlags),
        memmgrAllocReservedFBRegionMemdesc_IMPL_exit);

    memdescSetPageSize(*ppMemdesc, AT_GPU, RM_PAGE_SIZE);
    memdescDescribe(*ppMemdesc, ADDR_FBMEM, rangeStart, allocSize);
    memdescSetHeapOffset(*ppMemdesc, rangeStart);

    memdescTagAlloc(status, allocTag, *ppMemdesc);
    NV_ASSERT_OK_OR_GOTO(status, status, memmgrAllocReservedFBRegionMemdesc_IMPL_exit);

memmgrAllocReservedFBRegionMemdesc_IMPL_exit:
    if ((status != NV_OK) && (*ppMemdesc != NULL))
    {
        NV_PRINTF(LEVEL_ERROR, "Cannot allocate the memory with range allocation\n");
        memdescDestroy(*ppMemdesc);
        *ppMemdesc = NULL;
    }

    return status;
}


NV_STATUS
memmgrReserveMemoryForFsp_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);

    //
    // If we sent FSP commands to boot ACR, we need to allocate the surfaces
    // used by FSP and ACR as WPR/FRTS here from the reserved heap
    //
    if (pKernelFsp && (!pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_FRTS_VIDMEM) &&
        (pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_BOOT_COMMAND_OK))))
    {

        // For GSP-RM flow, we don't need to allocate WPR since it is handled by CPU
        if (pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_GSP_MODE_GSPRM))
        {
            return NV_OK;
        }

    }
    return NV_OK;
}

NvU64
memmgrGetVgpuHostRmReservedFb_KERNEL
(
    OBJGPU         *pGpu,
    MemoryManager  *pMemoryManager,
    NvU32           vgpuTypeId
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_MEMMGR_GET_VGPU_CONFIG_HOST_RESERVED_FB_PARAMS params = {0};

    params.vgpuTypeId = vgpuTypeId;
    // Send to GSP to get amount of FB reserved for the host
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                           pGpu->hInternalClient,
                                           pGpu->hInternalSubdevice,
                                           NV2080_CTRL_CMD_INTERNAL_MEMMGR_GET_VGPU_CONFIG_HOST_RESERVED_FB,
                                           &params,
                                           sizeof(params)));
    return params.hostReservedFb;
}

/*!
 * @brief   Memory Manager State post load
 *
 * @param[in]       pGpu           GPU pointer
 * @param[in/out]   pMemoryManager MemoryManager pointer
 * @param[in]       flags          State transition flags
 *
 * @returns On success, returns NV_OK.
 *          On failure, returns error code.
 */
NV_STATUS
memmgrStatePostLoad_IMPL
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    NvU32 flags
)
{
    if (memmgrIsLocalEgmSupported(pMemoryManager))
    {
        NvU64 egmPhysAddr, egmSize;
        NvS32 egmNodeId;
        NvU32 data32;
        KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

        pMemoryManager->localEgmNodeId = -1;
        if (gpuIsSelfHosted(pGpu) &&
            pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_C2C_LINK_UP) &&    // EGM can be enabled only in C2C path.
            (osGetEgmInfo(pGpu, &egmPhysAddr, &egmSize, &egmNodeId) == NV_OK) &&
            (egmSize != 0))
        {
            pMemoryManager->localEgmBasePhysAddr = egmPhysAddr;
            pMemoryManager->localEgmSize = egmSize;
            pMemoryManager->localEgmNodeId = egmNodeId;
            //
            // Using fixed Peer ID 7 for local EGM so that vGPU
            // migration doesn't fail because of peer id conflict in
            // the new host system.
            //
            pMemoryManager->localEgmPeerId = 7;
            pMemoryManager->bLocalEgmEnabled = NV_TRUE;
        }

        //
        // regkey can override the production flow values.
        // Note that this could cause an issue with vGPU migration
        // if one host system uses regkey to override the EGM peer id
        // and other host system doesn't.
        //
        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_ENABLE_LOCAL_EGM_PEER_ID, &data32) == NV_OK)
        {
            pMemoryManager->bLocalEgmEnabled = NV_TRUE;
            pMemoryManager->localEgmPeerId = data32;
        }
    }

    //
    // Reserve the peerID used for local EGM so that the peerID isn't
    // resused for other peer Gpus.
    //
    if (memmgrIsLocalEgmEnabled(pMemoryManager))
    {
        if (kbusReserveP2PPeerIds_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), NVBIT(pMemoryManager->localEgmPeerId)) == NV_OK)
        {
            if (!IS_VIRTUAL_WITH_SRIOV(pGpu))
            {
                NV2080_CTRL_INTERNAL_HSHUB_EGM_CONFIG_PARAMS params = { 0 };
                RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
                NV_STATUS status;

                params.egmPeerId = pMemoryManager->localEgmPeerId;

                // Call physical HSHUB to program the EGM PeerId settings.

                status = pRmApi->Control(pRmApi,
                                         pGpu->hInternalClient,
                                         pGpu->hInternalSubdevice,
                                         NV2080_CTRL_CMD_INTERNAL_HSHUB_EGM_CONFIG,
                                         &params,
                                         sizeof(params));
                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR, "HSHUB programming failed for EGM Peer ID: %u\n",
                              pMemoryManager->localEgmPeerId);
                    pMemoryManager->bLocalEgmEnabled = NV_FALSE;
                    pMemoryManager->localEgmPeerId = BUS_INVALID_PEER;
                    return status;
                }
            }
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Peer ID specified for local EGM already in use!\n");
            pMemoryManager->bLocalEgmEnabled = NV_FALSE;
            pMemoryManager->localEgmPeerId = BUS_INVALID_PEER;
            return NV_ERR_INVALID_ARGUMENT;
        }
    }
    else
    {
        pMemoryManager->localEgmPeerId = BUS_INVALID_PEER;
    }
    return NV_OK;
}

NV_STATUS
memmgrInitCeUtils_IMPL
(
    MemoryManager *pMemoryManager,
    NvBool         bFifoLite
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pMemoryManager);
    NV0050_ALLOCATION_PARAMETERS ceUtilsParams = {0};

    NV_ASSERT_OR_RETURN(pMemoryManager->pCeUtils == NULL, NV_ERR_INVALID_STATE);

    if (bFifoLite)
        ceUtilsParams.flags |= DRF_DEF(0050_CEUTILS, _FLAGS, _FIFO_LITE, _TRUE);
    else if(IsTURINGorBetter(pGpu))
        ceUtilsParams.flags |= DRF_DEF(0050_CEUTILS, _FLAGS, _NO_BAR1_USE, _TRUE);

    if (pMemoryManager->bCePhysicalVidmemAccessNotSupported)
        ceUtilsParams.flags |= DRF_DEF(0050_CEUTILS, _FLAGS, _VIRTUAL_MODE, _TRUE);

    NV_ASSERT_OK_OR_RETURN(objCreate(&pMemoryManager->pCeUtils, pMemoryManager, CeUtils, ENG_GET_GPU(pMemoryManager), NULL, &ceUtilsParams));

    NV_STATUS status = memmgrTestCeUtils(pGpu, pMemoryManager);
    NV_ASSERT_OK(status);
    if (status != NV_OK)
    {
        memmgrDestroyCeUtils(pMemoryManager);
    }

    return status;
}

void
memmgrDestroyCeUtils_IMPL
(
    MemoryManager *pMemoryManager
)
{
    objDelete(pMemoryManager->pCeUtils);
    pMemoryManager->pCeUtils = NULL;
}
