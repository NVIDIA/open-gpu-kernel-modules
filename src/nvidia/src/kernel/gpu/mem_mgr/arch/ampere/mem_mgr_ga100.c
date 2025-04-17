/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "platform/sli/sli.h"

#include "nvRmReg.h"

#include "kernel/gpu/intr/intr.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "vgpu/vgpu_events.h"
#include "nvdevid.h"

#include "published/ampere/ga100/dev_mmu.h"
#include "published/ampere/ga100/dev_fb.h"

#include "kernel/virtualization/common_vgpu_mgr.h"

#define NV_CBC_MAX_SIZE_BUG_2509894_WAR   ((3 * NVBIT64(30)) / 2) // 1.5GBs

/*!
 * @brief This function will return the Kind that should be used by surfaces which
 *        maps the FLA object
 *
 * @param[in/out] pPteKind
 *
 * @returns NV_OK
 */
NV_STATUS
memmgrGetFlaKind_GA100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU32         *pPteKind
)
{
    *pPteKind = NV_MMU_PTE_KIND_SMSKED_MESSAGE;
    return NV_OK;
}

/*!
 * @brief Determine Alignment for a surface, if the surface is compressible with
 * the reg key enabled, set the hwAlignment to 256KB
 * else fall back to pre-Ampere way
 *
 * returns NV_STATUS
 */
NV_STATUS
memmgrAllocDetermineAlignment_GA100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU64         *pMemSize,
    NvU64         *pAlign,
    NvU64          alignPad,
    NvU32          allocFlags,
    NvU32          retAttr,
    NvU32          retAttr2,
    NvU64          hwAlignment
)
{
    const MEMORY_SYSTEM_STATIC_CONFIG    *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    // set the alignment to 256K if property is enabled and its a compressed surface
    if (pMemorySystemConfig->bUseOneToFourComptagLineAllocation &&
        !FLD_TEST_DRF(OS32, _ATTR, _COMPR, _NONE, retAttr))
    {
        hwAlignment = pMemorySystemConfig->comprPageSize - 1;
    }

    return memmgrAllocDetermineAlignment_GM107(pGpu, pMemoryManager, pMemSize, pAlign, alignPad,
                                               allocFlags, retAttr, retAttr2, hwAlignment);
}

/**
 * @brief Override Scrubber related PDB properties based on regkeys and platform configs
 */
void
memmgrScrubRegistryOverrides_GA100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    //Fix me: Fork the same function for GA10x.

    //
    // Disabling the SCRUB_ON_FREE property for all the platforms except Windows TCC Mode.
    // Disabling in Non-TCC windows because the OS manages FB
    // Disabling for Simulation Platforms, since slower in simulation
    // Disabling in DFPGA, since they skip the Host Load
    // Disabling for vGPU (host), since the plugin has scrubbing support
    // Disabling for legacy VGPU (guest), blocked on bug #1929798
    // Disabling for SLI for now, until the bug # 1790190 is fixed.
    // Disabling for GSP-RM ucode, since scrubbing is done from CPU-side kernel RM.
    // Enabling virtual scrubbing mode for SRIOV-HEAVY mode.
    //

    if ((RMCFG_FEATURE_PLATFORM_WINDOWS && !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_TCC_MODE)) ||
         IS_SIMULATION(pGpu) || IsDFPGA(pGpu) ||
         pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VGPU) ||
         IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
         RMCFG_FEATURE_PLATFORM_GSP ||
         pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) ||
         IsSLIEnabled(pGpu))
    {
        pMemoryManager->bScrubOnFreeEnabled = NV_FALSE;
    }

    //
    // CE virtual writes are used in the following cases
    // 1. When SR-IOV heavy is in use on GA100
    // 2. When APM is enabled on GA100.
    //
    if (pMemoryManager->bScrubOnFreeEnabled &&
        ((IS_VIRTUAL_WITH_SRIOV(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)) ||
        gpuIsApmFeatureEnabled(pGpu)))
    {
        pMemoryManager->bUseVasForCeMemoryOps = NV_TRUE;
    }
}

/*!
 * Read and validate MMU Lock registers.
 */
NV_STATUS
memmgrReadMmuLock_GA100
(
    OBJGPU   *pGpu,
    MemoryManager *pMemoryManager,
    NvBool    *pbIsValid,
    NvU64     *pMmuLockLo,
    NvU64     *pMmuLockHi
)
{
    NvU32 plm = 0;
    NvU32 tmp = 0;


    *pbIsValid = NV_FALSE;
    *pMmuLockLo = 0;
    *pMmuLockHi = 0;

    // Ensure RM can read MMU_LOCKED region
    plm = GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_LOCK_ADDR_LO__PRIV_LEVEL_MASK);

    if (!FLD_TEST_DRF(_PFB_PRI, _MMU_LOCK_CFG_PRIV_LEVEL_MASK, _READ_PROTECTION_LEVEL0, _ENABLE, plm))
    {
        NV_PRINTF(LEVEL_ERROR, "MMU_LOCK read permission disabled, PLM val 0x%0x\n",
                         plm);
        NV_ASSERT(0);
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    // Read MEM lock values
    tmp  = DRF_VAL(_PFB, _PRI_MMU_LOCK_ADDR_LO, _VAL, GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_LOCK_ADDR_LO));
    *pMmuLockLo  = ((NvU64)tmp) << NV_PFB_PRI_MMU_LOCK_ADDR_LO_ALIGNMENT;

    tmp  = DRF_VAL(_PFB, _PRI_MMU_LOCK_ADDR_HI, _VAL, GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_LOCK_ADDR_HI));
    *pMmuLockHi  = ((NvU64)tmp) << NV_PFB_PRI_MMU_LOCK_ADDR_HI_ALIGNMENT;

    // Check for validity
    if (*pMmuLockHi >= *pMmuLockLo)
        *pbIsValid = NV_TRUE;

    return NV_OK;
}

/*
 * Bug 2974274
 * As stated in the bug, row remapper takes up few MBs at end of FB but LOCAL_MEMORY_RANGE register
 * rounds it down by nearest 1GB boundary. Since RM uses LOCAL_MEMORY_RANGE to figure out the total
 * FB size, this results in almost 1GB of loss in usable FB size and this function along with WARs
 * in VBIOS & ACR solves the issue.
 * VBIOS rounds up the size to nearest 1GB boundary and locks down (MMU_LOCK) the difference between the usable
 * and rounded up size. This function ensures the difference is blocked from RM allocation.
 *
 */
NV_STATUS
memmgrBlockMemLockedMemory_GA100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    FB_REGION_DESCRIPTOR blockedFbRegion;
    NvU64                memLockLo            = 0;
    NvU64                memLockHi            = 0;
    NvU64                size                 = 0;
    NvBool               bIsMmuLockValid      = NV_FALSE;

    NV_ASSERT_OK_OR_RETURN(memmgrReadMmuLock_HAL(pGpu, pMemoryManager, &bIsMmuLockValid, &memLockLo, &memLockHi));

    // MMU_LOCK is not set in OLD Vbios that programs 1GB less in LOCAL_MEMORY_RANGE
    if (!bIsMmuLockValid)
    {
        return NV_OK;
    }

    memLockHi  = NV_ALIGN_UP(memLockHi, 0x10000) - 1; // Align up to cover till the last byte

    // Check if memLockHi equals FB_TOP, if not MMU_LOCK is set in unexpected range
    if (((memLockHi + 1) >> 20) != pMemoryManager->Ram.fbTotalMemSizeMb)
    {
        return NV_ERR_INVALID_STATE;
    }

    // Add a new region that will be blocked for any usage.
    portMemSet(&blockedFbRegion, 0, sizeof(blockedFbRegion));
    size = RM_PAGE_ALIGN_UP(((memLockHi - memLockLo) + 1));

    blockedFbRegion.base = memLockLo;
    blockedFbRegion.limit = memLockHi;
    blockedFbRegion.rsvdSize = 0;
    blockedFbRegion.bRsvdRegion = NV_TRUE;
    blockedFbRegion.performance = 0;
    blockedFbRegion.bSupportCompressed = NV_FALSE;
    blockedFbRegion.bSupportISO = NV_FALSE;
    blockedFbRegion.bProtected = NV_FALSE;
    blockedFbRegion.bInternalHeap = NV_FALSE;
    blockedFbRegion.bLostOnSuspend = NV_TRUE;

    memmgrInsertFbRegion(pGpu, pMemoryManager, &blockedFbRegion);

    pMemoryManager->Ram.fbUsableMemSize -= size;

    NV_PRINTF(LEVEL_INFO, "Blocked  Start: 0x%0llx End: 0x%0llx Size: 0x%0llx\n",
                       memLockLo, memLockHi, size);
    return NV_OK;
}

/*!
 *  Returns the max context size
 *
 *  @returns NvU64
 */
NvU64
memmgrGetMaxContextSize_GA100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NvU64  size = memmgrGetMaxContextSize_TU102(pGpu, pMemoryManager);

    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        if (!gpuIsClientRmAllocatedCtxBufferEnabled(pGpu))
        {
            //
            // When ctx buffer management is in GSP-RM, GSP-RM needs extra
            // 100 MBs to meet max CUDA context allocation requirement
            //
            size += 100 * 1024 * 1024;
        }
    }

    //
    // See bug 200619860. We are running out of memory during allocation
    // of GR buffers. Since GR buffers are not allocated inside guest RM
    // we are skipping reservation there
    //
    if (RMCFG_FEATURE_PLATFORM_WINDOWS &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_TCC_MODE))
    {
        size += 32 * 1024 * 1024;
    }

    return size;
}

void
memmgrGetDisablePlcKind_GA100
(
    MemoryManager *pMemoryManager,
    NvU32          *pKind
)
{
    if (pKind != NULL)
    {
        *pKind = NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE_DISABLE_PLC;
    }
}

/*!
 * @brief This function sets the PDB property to enable/disable dynamic page offlining
 */
void
memmgrEnableDynamicPageOfflining_GA100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    if (gpuIsGlobalPoisonFuseEnabled(pGpu))
    {
       pMemoryManager->bEnableDynamicPageOfflining = NV_TRUE;
    }

    return;
}

/*!
 *  @brief Get blacklist page details.
 *
 *  @param[in]      pGpu            OBJGPU
 *  @param[in]      pMemoryManager  MemoryManager
 *  @param[out]     pBlAddrs        BLACKLIST_ADDRESSES where count is taken
 *                                  as input and the addressed and count is
 *                                  returned.
 *  @param[in/out]  pCount          Takes size of pBlAddrs as input and returns
 *                                  the number of populated addresses in
 *                                  pBlAddrs.
   @returns NV_STATUS
 *
 */
NV_STATUS
memmgrGetBlackListPages_GA100
(
    OBJGPU              *pGpu,
    MemoryManager       *pMemoryManager,
    BLACKLIST_ADDRESS   *pBlAddrs,
    NvU32               *pCount
)
{
    NvU32               baseIndex        = 0;
    NV_STATUS           status           = NV_OK;
    NvU32               idx              = 0;
    NvU32               entryIdx         = 0;

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_ALLOW_PAGE_RETIREMENT) ||
             !gpuCheckPageRetirementSupport_HAL(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // Read the inforom for a list of pages to blacklist.
    // SLI support requires investigation to ensure
    // identical heaps on both devices (bug 756971).
    //
    if (IsSLIEnabled(pGpu) && !gpuIsEccPageRetirementWithSliAllowed(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS *pBlParams =
        portMemAllocNonPaged(sizeof(*pBlParams));
    if (pBlParams == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    while (baseIndex < NV2080_CTRL_FB_DYNAMIC_BLACKLIST_MAX_PAGES)
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        portMemSet(pBlParams, 0, sizeof(*pBlParams));

        pBlParams->baseIndex = baseIndex;

        status = pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_FB_GET_DYNAMIC_OFFLINED_PAGES,
                                 pBlParams,
                                 sizeof(*pBlParams));
        if(NV_OK != status)
        {
            if (NV_ERR_NOT_SUPPORTED == status ||
                NV_ERR_OBJECT_NOT_FOUND == status)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "No blacklisted pages\n");
            }
            else
            {
                NV_ASSERT(0);
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to read black list addresses\n");
            }
            break;
        }

        for (idx = 0; idx < pBlParams->validEntries; idx++)
        {

            if (entryIdx >= *pCount)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                goto done;
            }
            pBlAddrs[entryIdx].address = pBlParams->offlined[idx].pageNumber << RM_PAGE_SHIFT;
            pBlAddrs[entryIdx].type = pBlParams->offlined[idx].source;
            entryIdx++;
         }

        if (!pBlParams->bMore) {
            break;
        }

        baseIndex += NV2080_CTRL_FB_DYNAMIC_BLACKLIST_MAX_ENTRIES;
    }

done:
    portMemFree(pBlParams);
    *pCount = entryIdx;

    return status;
}

/*!
 *  @brief Inserts an unprotected segment at the start of FB on GA100
           to prevent VPR from getting allocated here
 *
 *  @returns NV_STATUS
 */
NV_STATUS
memmgrInsertUnprotectedRegionAtBottomOfFb_GA100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU64         *pSize
)
{
    FB_REGION_DESCRIPTOR fbRegion        = {0};
    NvU64                memLockLo       = 0;
    NvU64                memLockHi       = 0;
    NvBool               bIsMmuLockValid = NV_FALSE;
    NvU64                size;

    NV_ASSERT_OK_OR_RETURN(memmgrReadMmuLock_HAL(pGpu, pMemoryManager,
                                                 &bIsMmuLockValid, &memLockLo,
                                                 &memLockHi));

    // MMU_LOCK is not set in OLD Vbios that programs 1GB less in LOCAL_MEMORY_RANGE
    if (!bIsMmuLockValid)
    {
        *pSize = 0;
        return NV_OK;
    }

    memLockHi  = NV_ALIGN_UP(memLockHi, 0x10000) - 1; // Align up to cover till the last byte

    size = RM_PAGE_ALIGN_UP(memLockHi - memLockLo + 1);

    //
    // Bug 2509894: In order to prevent CBC wrap around and clobbering of
    // VPR contents, we move VPR out of the bottom 1.5GB of video memory.
    // For raw mode, HW assumes a max of 384 MBs (for 96GB FB) of CBC. However,
    // on GA100 due to a HW bug this figure comes around ~1GB. By experimentation,
    // we found that 1.5GBs works fine for us.
    //
    size = NV_MAX(size, NV_CBC_MAX_SIZE_BUG_2509894_WAR);

    // SEC2 ucode expects VPR start address to be 1MB aligned
    size = NV_ALIGN_UP(size, 1 * 1024 * 1024);

    fbRegion.base = 0;
    fbRegion.limit = size - 1;
    fbRegion.rsvdSize = 0;
    fbRegion.bRsvdRegion = NV_FALSE;
    fbRegion.performance = 0;
    fbRegion.bSupportCompressed = NV_TRUE;
    fbRegion.bSupportISO = NV_FALSE;
    fbRegion.bProtected = NV_FALSE;
    fbRegion.bInternalHeap = NV_FALSE;

    memmgrInsertFbRegion(pGpu, pMemoryManager, &fbRegion);

    NV_PRINTF(LEVEL_INFO, "Unprotected Block Start: 0x%0llx End: 0x%0llx Size: 0x%0llx\n",
                       memLockLo, memLockHi, size);

    // Return the size
    *pSize = size;

    return NV_OK;
}

NvBool
memmgrIsMemDescSupportedByFla_GA100
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    if ((memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM) || memdescIsEgm(pMemDesc))
    {
        return NV_TRUE;
    }
    return NV_FALSE;
}
