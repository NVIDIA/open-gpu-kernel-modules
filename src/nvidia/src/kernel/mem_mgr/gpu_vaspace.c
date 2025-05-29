/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/***************************** HW State Routines ***************************\
*                                                                           *
*         GPU Virtual Address Space Function Definitions.                   *
*                                                                           *
\***************************************************************************/

#include "gpu/mmu/kern_gmmu.h"
#include "mem_mgr/gpu_vaspace.h"
#include "mem_mgr/fabric_vaspace.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "os/os.h"
#include "containers/eheap_old.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/bus/kern_bus.h"
#include "mmu/mmu_walk.h"
#include "lib/base_utils.h"
#include "class/cl90f1.h"  // FERMI_VASPACE_A
#include "ctrl/ctrl90f1.h"  // FERMI_VASPACE_A
#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER
#include "vgpu/rpc.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/device/device.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/rmapi/mapping_list.h"
#include "gpu/subdevice/subdevice.h"
#include "core/locks.h"
#include "mem_mgr/pool_alloc.h"
#include "deprecated/rmapi_deprecated.h"
#include "rmapi/rs_utils.h"
#include "gpu/mem_mgr/vaspace_api.h"
#include "platform/sli/sli.h"

#include "nvmisc.h"



#define GMMU_PD0_VADDR_BIT_LO                        21

static const NvU64 pageSizes[] = {
    RM_PAGE_SIZE,
    RM_PAGE_SIZE_64K,
    RM_PAGE_SIZE_HUGE,
    RM_PAGE_SIZE_512M
    , RM_PAGE_SIZE_256G
};

static const NvU32 pageSizeCount = NV_ARRAY_ELEMENTS(pageSizes);

static NV_STATUS
_gvaspaceGpuStateConstruct
(
    OBJGVASPACE    *pGVAS,
    OBJGPU         *pGpu,
    GVAS_GPU_STATE *pGpuState,
    const NvU64     reqBigPageSize,
    const NvU64     vaStart,
    const NvU64     vaLimit,
    const NvU64     vaStartInternal,
    const NvU64     vaLimitInternal,
    const NvU32     flags,
    const NvBool    bFirst,
    NvU64          *pFullPdeCoverage,
    NvU32          *pPartialPdeExpMax
);

static void
_gvaspaceGpuStateDestruct
(
    OBJGVASPACE    *pGVAS,
    OBJGPU         *pGpu,
    GVAS_GPU_STATE *pGpuState
);

static NV_STATUS
_gvaspaceReserveTopForGrowth
(
    OBJGVASPACE    *pGVAS
);

static NV_STATUS
_gvaspaceReserveRange
(
    OBJGVASPACE *pGVAS,
    NvU64 rangeLo,
    NvU64 rangeHi
);

static NV_STATUS
_gvaspacePinLazyPageTables
(
    OBJGVASPACE       *pGVAS,
    OBJGPU            *pGpu,
    const NvU64        va
);

static NV_STATUS
_gvaspaceFreeVASBlock
(
    OBJEHEAP  *pHeap,
    void      *pEnv,
    EMEMBLOCK *pMemBlock,
    NvU32     *pContinue,
    NvU32     *pInvalCursor
);

static NV_STATUS
_gvaspaceMappingInsert
(
    OBJGVASPACE        *pGVAS,
    OBJGPU             *pGpu,
    GVAS_BLOCK         *pVASBlock,
    const NvU64         vaLo,
    const NvU64         vaHi,
    const VAS_MAP_FLAGS flags
);

static NV_STATUS
_gvaspaceMappingRemove
(
    OBJGVASPACE       *pGVAS,
    OBJGPU            *pGpu,
    GVAS_BLOCK        *pVASBlock,
    const NvU64        vaLo,
    const NvU64        vaHi
);

static void
_gvaspaceAddPartialPtRange
(
    OBJGVASPACE       *pGVAS,
    const NvU64        va
);

static NV_STATUS
_gvaspaceSetExternalPageDirBase
(
    OBJGVASPACE       *pGVAS,
    OBJGPU            *pGpu,
    MEMORY_DESCRIPTOR *pMemDesc
);

static NV_STATUS
_gvaspaceReservePageTableEntries
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu,
    const NvU64  vaLo,
    const NvU64  vaHi,
    const NvU64  pageSizeMask
);

static NV_STATUS
_gvaspaceReleasePageTableEntries
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu,
    const NvU64  vaLo,
    const NvU64  vaHi,
    const NvU64  pageSizeMask
);

static NV_STATUS
_gvaspaceReleaseUnreservedPTEs
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu,
    const NvU64  vaLo,
    const NvU64  vaHi,
    const MMU_FMT_LEVEL *pLevelFmt
);

static NV_STATUS
_gvaspaceCopyServerRmReservedPdesToServerRm
(
    NvHandle                                              hClient,
    NvHandle                                              hVASpace,
    OBJGPU                                               *pGpu,
    NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pPdeCopyParams
);

static void
_gvaspaceForceFreePageLevelInstances
(
    OBJGVASPACE    *pGVAS,
    OBJGPU         *pGpu,
    GVAS_GPU_STATE *pGpuState
);

static NV_STATUS
_gvaspacePopulatePDEentries
(
    OBJGVASPACE    *pGVAS,
    OBJGPU         *pGpu,
    NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pPdeCopyParams
);

static NV_STATUS
_gvaspaceBar1VaSpaceConstructFW
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu
)
{
    NV_STATUS status = NV_OK;

    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        return NV_OK;
    }

    status = gvaspacePinRootPageDir(pGVAS, pGpu);
    NV_ASSERT_OR_RETURN(NV_OK == status, status);

    return status;
}

static NV_STATUS
_gvaspaceBar1VaSpaceConstructClient
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu
)
{
    NV_STATUS         status  = NV_OK;
    OBJVASPACE       *pVAS    = staticCast(pGVAS, OBJVASPACE);
    MMU_WALK_USER_CTX userCtx = {0};

    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        NV_ASSERT_OK_OR_RETURN(gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx));
        status = mmuWalkSparsify(userCtx.pGpuState->pWalk, vaspaceGetVaStart(pVAS),
                                 vaspaceGetVaLimit(pVAS), NV_FALSE);

        gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
    }

    return status;
}

static NV_STATUS
_gvaspaceBar1VaSpaceConstruct
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu
)
{
    NV_STATUS status = NV_OK;

    status = _gvaspaceBar1VaSpaceConstructFW(pGVAS, pGpu);
    NV_ASSERT_OR_RETURN(status == NV_OK, status);

    status = _gvaspaceBar1VaSpaceConstructClient(pGVAS, pGpu);
    NV_ASSERT_OR_RETURN(status == NV_OK, status);

    return status;
}

static NV_STATUS
_gvaspaceReserveVaForServerRm
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu
)
{
    NV_STATUS   status = NV_OK;
    OBJVASPACE *pVAS   = staticCast(pGVAS, OBJVASPACE);

    // Reserve everything below vaStartServerRMOwned as non allocable by server RM.
    if (pVAS->vasStart < pGVAS->vaStartServerRMOwned)
    {
        status = _gvaspaceReserveRange(pGVAS, pVAS->vasStart,
                                       pGVAS->vaStartServerRMOwned - 1);
        NV_ASSERT_OR_RETURN(status == NV_OK, status);
    }

    // Reserve everything above vaLimitServerRMOwned as non allocable by server RM.
    if (pGVAS->vaLimitServerRMOwned < pGVAS->vaLimitInternal)
    {
        status = _gvaspaceReserveRange(pGVAS, pGVAS->vaLimitServerRMOwned + 1,
                                       pGVAS->vaLimitInternal);
        NV_ASSERT_OR_RETURN(status == NV_OK, status);
    }

    return status;
}

static NV_STATUS
_gvaspaceReserveVaForClientRm
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu
)
{
    NV_STATUS   status = NV_OK;
    OBJVASPACE *pVAS   = staticCast(pGVAS, OBJVASPACE);

    //
    // Client RM needs to hold the GPU lock for any GPU it wants to RPC to.
    // We don't actually know which locks we potentially hold here, so use
    // SAFE_LOCK_UPGRADE.
    //
    GPU_MASK gpuMask = pVAS->gpuMask;
    status = rmGpuGroupLockAcquire(0, GPU_LOCK_GRP_MASK,
                GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE, RM_LOCK_MODULES_MEM, &gpuMask);

    // If we get NOTHING_TO_DO, we already have the needed locks, so don't free them
    if (status == NV_WARN_NOTHING_TO_DO)
        gpuMask = 0;
    else if (status != NV_OK)
        return status;

    //
    // Reserve everything in the range [vaStartServerRMOwned, vaLimitServerRMOwned]
    // as non allocable by client RM. This range is reserved for server RM.
    //
    status = _gvaspaceReserveRange(pGVAS, pGVAS->vaStartServerRMOwned,
                                   pGVAS->vaLimitServerRMOwned);
    NV_ASSERT_OR_GOTO(status == NV_OK, done);

    // Loop over each GPU associated with VAS.
    FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
    {
        MMU_WALK_USER_CTX userCtx  = {0};
        status = gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx);
        if (status != NV_OK)
        {
            break;
        }
        else
        {
            //
            // We're pinning only till PD0 for now to conserve memory. We don't know
            // how much memory will be eventually consumed by leaf page tables.
            //
            const MMU_FMT_LEVEL *pLevelFmt =
                   mmuFmtFindLevelWithPageShift(userCtx.pGpuState->pFmt->pRoot, GMMU_PD0_VADDR_BIT_LO);
            status = mmuWalkReserveEntries(userCtx.pGpuState->pWalk,
                                           pLevelFmt,
                                           pGVAS->vaStartServerRMOwned,
                                           pGVAS->vaLimitServerRMOwned,
                                           NV_TRUE);

            gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);

            if (status != NV_OK)
            {
                break;
            }
        }

        status = gvaspaceCopyServerRmReservedPdesToServerRm(pGVAS, pGpu);
        if (status != NV_OK)
        {
            break;
        }
    }
    FOR_EACH_GPU_IN_MASK_UC_END

done:
    if (gpuMask != 0)
    {
        rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);
    }
    return status;
}

NV_STATUS
gvaspaceReserveSplitVaSpace_IMPL
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu
)
{
    NV_STATUS status    = NV_OK;
    NvBool    bClientRm = NV_FALSE;
    NvBool    bServerRm = NV_FALSE;
    NvU32     gfid;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    if (IS_VIRTUAL_WITH_SRIOV(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        bClientRm = NV_TRUE;
    }
    else if (IS_GFID_VF(gfid))
    {
        bServerRm = NV_TRUE;
    }

    if (bServerRm || bClientRm)
    {
        OBJVASPACE *pVAS = staticCast(pGVAS, OBJVASPACE);

        pGVAS->vaStartServerRMOwned = NV_MIN(pGVAS->vaLimitInternal -
                                         SPLIT_VAS_SERVER_RM_MANAGED_VA_SIZE + 1,
                                         SPLIT_VAS_SERVER_RM_MANAGED_VA_START);

        if (pVAS->vasStart > pGVAS->vaStartServerRMOwned)
        {
            pGVAS->vaStartServerRMOwned = pVAS->vasStart + SPLIT_VAS_SERVER_RM_MANAGED_VA_START;
        }

        pGVAS->vaLimitServerRMOwned = pGVAS->vaStartServerRMOwned +
                                      SPLIT_VAS_SERVER_RM_MANAGED_VA_SIZE - 1;

        // Base and limit + 1 should be aligned to 2MB.
        if (!NV_IS_ALIGNED(pGVAS->vaStartServerRMOwned, NVBIT64(GMMU_PD0_VADDR_BIT_LO)))
        {
            NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_ARGUMENT);
        }

        if (!NV_IS_ALIGNED(pGVAS->vaLimitServerRMOwned + 1, NVBIT64(GMMU_PD0_VADDR_BIT_LO)))
        {
            NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_ARGUMENT);
        }

        // Validate limit.
        if (pGVAS->vaLimitServerRMOwned > pGVAS->vaLimitInternal)
        {
            NV_PRINTF(LEVEL_ERROR, "vaLimitServerRMOwned (0x%llx)"
                      "> vaLimitInternal (0x%llx)\n",
                       pGVAS->vaLimitServerRMOwned, pGVAS->vaLimitInternal);
            NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_ARGUMENT);
        }

        //
        // If we are running inside server on behalf of a client, server RM can assign VA
        // only inside the range [vaStartServerRMOwned, vaLimitServerRMOwned].
        //
        if (bServerRm)
        {
            status = _gvaspaceReserveVaForServerRm(pGVAS, pGpu);
        }
        else if (bClientRm)
        {
            status = _gvaspaceReserveVaForClientRm(pGVAS, pGpu);
        }
    }
    return status;
}

NV_STATUS
gvaspaceConstruct__IMPL
(
    OBJGVASPACE *pGVAS,
    NvU32        classId,
    NvU32        vaspaceId,
    NvU64        vaStart,
    NvU64        vaLimit,
    NvU64        vaStartInternal,
    NvU64        vaLimitInternal,
    NvU32        flags
)
{
    OBJVASPACE      *pVAS  = staticCast(pGVAS, OBJVASPACE);
    OBJGPU          *pGpu  = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));
    NvU64            reqBigPageSize;
    NV_STATUS        status = NV_OK;
    GVAS_GPU_STATE  *pGpuState;
    NvU32            highestBitIdx;
    NvU64            fullPdeCoverage = 0;
    NvU32            partialPdeExpMax = 0;
    NvBool           bFirst = NV_TRUE;
    NvBool           bCallingContextPlugin;

    NV_ASSERT_OR_RETURN(FERMI_VASPACE_A == classId, NV_ERR_INVALID_ARGUMENT);

    // Save off flags.
    pGVAS->flags = flags;

    // Save off UVM mirroring flag.
    if (flags & VASPACE_FLAGS_SET_MIRRORED)
    {
        NV_ASSERT_OR_RETURN(!(pGVAS->flags & VASPACE_FLAGS_BAR), NV_ERR_ILLEGAL_ACTION);
        NV_ASSERT_OR_RETURN(!(pGVAS->flags & VASPACE_FLAGS_IS_EXTERNALLY_OWNED), NV_ERR_INVALID_ARGUMENT);
        pGVAS->bIsMirrored = NV_TRUE;
    }

    if (flags & VASPACE_FLAGS_ENABLE_FAULTING)
    {
        // All channels in this address space will have faulting enabled.
       pGVAS->bIsFaultCapable = NV_TRUE;
    }
    if (flags & VASPACE_FLAGS_IS_EXTERNALLY_OWNED)
    {
        // This address space is managed by the UVM driver.
       pGVAS->bIsExternallyOwned = NV_TRUE;
    }
    if (flags & VASPACE_FLAGS_ENABLE_ATS)
    {
        pGVAS->bIsAtsEnabled = NV_TRUE;
        NV_PRINTF(LEVEL_INFO, "ATS Enabled VaSpace\n");
        //
        // Initialize with invalid PASID value for sanity checking later during
        // PASID programming in HW.
        // For non-MODS case, PASID is programmed via control call
        // NV0080_CTRL_DMA_SET_PAGE_DIRECTORY
        //
        pGVAS->processAddrSpaceId = NV_U32_MAX;
    }

    if (flags & VASPACE_FLAGS_FLA)
    {
        pGVAS->flags |= VASPACE_FLAGS_INVALIDATE_SCOPE_NVLINK_TLB;
    }

    // Determine requested big page size based on flags.
    switch (DRF_VAL(_VASPACE, _FLAGS, _BIG_PAGE_SIZE, flags))
    {
        case NV_VASPACE_FLAGS_BIG_PAGE_SIZE_64K:
            reqBigPageSize = RM_PAGE_SIZE_64K;
            break;
        case NV_VASPACE_FLAGS_BIG_PAGE_SIZE_128K:
            reqBigPageSize = RM_PAGE_SIZE_128K;
            break;
        case NV_VASPACE_FLAGS_BIG_PAGE_SIZE_DEFAULT:
            reqBigPageSize = 0; // Let GMMU pick based on format.
            break;
        default:
            NV_ASSERT_OR_RETURN(0, NV_ERR_NOT_SUPPORTED);
            break;
    }

    // Create per-GPU state array.
    highestBitIdx = pVAS->gpuMask;
    HIGHESTBITIDX_32(highestBitIdx);
    pGVAS->pGpuStates = portMemAllocNonPaged(sizeof(*pGVAS->pGpuStates) * (highestBitIdx + 1));
    NV_ASSERT_OR_RETURN(NULL != pGVAS->pGpuStates, NV_ERR_NO_MEMORY);
    portMemSet(pGVAS->pGpuStates, 0, sizeof(*pGVAS->pGpuStates) * (highestBitIdx + 1));

    // Initialize channel group map
    mapInit(&pGVAS->chanGrpMap, portMemAllocatorGetGlobalNonPaged());

    // Loop over each GPU associated with VAS.
    FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
    {
        pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
        status = _gvaspaceGpuStateConstruct(pGVAS, pGpu, pGpuState, reqBigPageSize,
                                            vaStart, vaLimit,  vaStartInternal,
                                            vaLimitInternal, flags,
                                            bFirst,
                                            &fullPdeCoverage, &partialPdeExpMax);
        if (NV_OK != status)
        {
            break;
        }
        bFirst = NV_FALSE;
    }
    FOR_EACH_GPU_IN_MASK_UC_END
    if (NV_OK != status)
    {
        goto catch;
    }

    // Validate limit.
    NV_ASSERT_OR_RETURN(pVAS->vasStart <= pVAS->vasLimit, NV_ERR_INVALID_ARGUMENT);
    // External limit is applied to the HW, so must be at least the internal limit.
    NV_ASSERT_OR_RETURN(pVAS->vasLimit >= pGVAS->vaLimitInternal, NV_ERR_INVALID_ARGUMENT);

    // Create virtual address heap (BC state).
    pGVAS->pHeap = portMemAllocNonPaged(sizeof(*pGVAS->pHeap));
    if (pGVAS->pHeap == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        NV_ASSERT_OR_GOTO(NULL != pGVAS->pHeap, catch);
    }

    constructObjEHeap(pGVAS->pHeap, pVAS->vasStart, pGVAS->vaLimitMax + 1,
                      sizeof(GVAS_BLOCK), 0);

    if (gpuIsSplitVasManagementServerClientRmEnabled(pGpu) &&
        !(pGVAS->flags & VASPACE_FLAGS_BAR) &&
        !(pGVAS->flags & VASPACE_FLAGS_FLA) &&
        !(pGVAS->flags & VASPACE_FLAGS_PMU) &&
        !(pGVAS->flags & VASPACE_FLAGS_HDA) &&
        !(pGVAS->flags & VASPACE_FLAGS_HWPM) &&
        !(pGVAS->flags & VASPACE_FLAGS_PERFMON) &&
        !(pGVAS->flags & VASPACE_FLAGS_DISABLE_SPLIT_VAS))
    {
        NV_ASSERT_OK_OR_GOTO(status, vgpuIsCallingContextPlugin(pGpu, &bCallingContextPlugin), catch);
        if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) || !bCallingContextPlugin)
        {
            status = gvaspaceReserveSplitVaSpace(pGVAS, pGpu);
            NV_ASSERT_OR_GOTO(NV_OK == status, catch);
        }
    }

    // Reserve VA block between current limit and max limit for later growth.
    if (flags & VASPACE_FLAGS_RESTRICTED_RM_INTERNAL_VALIMITS)
    {
        // MAC could overcommit VA, so let the entire va range include RM internal and Client VA be available.
        // Reserve only the varange outside the vaspace.
        // i.e., vaStart to vaStartInternal - 1 (enforce 32 bit client VA), vaStartInternal - vaLimitInternal (RM internal VA)
        // vaLimitInternal+1 - vasLimit (client VA)

        // By default allocations will be routed within RM internal va range.
        pGVAS->bRMInternalRestrictedVaRange = NV_TRUE;

        if (pVAS->vasLimit != pGVAS->vaLimitMax)
            status = _gvaspaceReserveRange(pGVAS, pVAS->vasLimit + 1, pGVAS->vaLimitMax);
    }
    else
    {
        status = _gvaspaceReserveTopForGrowth(pGVAS);
    }
    NV_ASSERT_OR_GOTO(NV_OK == status, catch);

    // Reserve VA holes for partial page tables if requested and supported.
    if ((flags & VASPACE_FLAGS_MINIMIZE_PTETABLE_SIZE) && (partialPdeExpMax > 0))
    {
        const NvU64         partialSize        = fullPdeCoverage >> partialPdeExpMax;
        const NvU64         pdeAlignedVasStart = NV_ALIGN_DOWN64(pVAS->vasStart, fullPdeCoverage);
        const NvU64         pdeAlignedVasLimit = NV_ALIGN_UP64(pGVAS->vaLimitInternal + 1, fullPdeCoverage) - 1;
        const NvU64         pdeAlignedVasSize  = pdeAlignedVasLimit - pdeAlignedVasStart + 1;
        const NvU64         maxRangeSize       = NV_ALIGN_DOWN64(pdeAlignedVasSize / 4, fullPdeCoverage);
        NvU32               i;

        NV_ASSERT_OR_RETURN(!(flags & VASPACE_FLAGS_RESTRICTED_RM_INTERNAL_VALIMITS), NV_ERR_ILLEGAL_ACTION);

        pGpu = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));

        //
        // Pick a size for partial page table ranges.
        //
        // This optimization is required for WHQL MaxContexts on pre-Pascal.
        // In this scenario each (minimal) context allocates at least one
        // small page table.
        // Each full small page table requires 256KB a piece
        // (assuming 128KB big page size).
        // With 100 contexts, this would require 100 * 256KB = 25MB of
        // reserved FB memory.
        // While system memory fallback is enabled it is not reliable.
        // OS makes no guarantees for such large contiguous sysmem
        // allocations.
        //
        // The optimization uses a heuristic based on two competing constraints:
        // 1. Need to cover enough small allocations that page table memory is
        //    not wasted incrementally (especially for small applications).
        // 2. Need to leave enough contiguous VA to satisfy large requests.
        //
        // There are drawbacks in both directions, so we pick a simple policy.
        // We statically partition the VA space into areas where partial
        // page tables will be used and areas that will use full page tables.
        // We pick the partitioning size to be the smaller of 256MB and
        // 1/4th of the VAS heap size to satisfy the above two constraints.
        //
        pGVAS->partialPtVaRangeSize = NV_MIN(NVBIT64(28), maxRangeSize);

        //
        // We also need to pick where to place the partial page table VA ranges.
        // We use a static heuristic: initial VA allocations usually land
        // at the beginning (normal) and end (grow down) of the VA heap.
        // Grow down is an important case since KMD reserves push buffers and other
        // special allocations at the end of the heap.
        //
        // There is also the complication that virtual address within 32-bits
        // are optimal for some UMDs and chips - e.g. UMD can restrict and detect 32-bit
        // addresses and compile shaders dynamically with more efficient instructions.
        // For these configurations we also allocate partial ranges above and
        // below the 4GB offset to catch allocations with 32-bit restricted ranges.
        // The range just above 32-bits catches unrestricted allocations
        // which are moved above 32-bits to stay out of the way and conserve
        // the 32-bit space.
        //
        // If the application uses a large amount of the VA it will eventually use
        // the middle of the heap, but at that point incremental page table waste
        // is amortized (low overall overhead).
        //
        // An alternative approach is to pick the partial PDEs dynamically,
        // for example the first N PDEs used.
        // However this significantly complicates VA heap allocation,
        // especially for grow down requests (think about it).
        // The original RM VAS code used this approach, but it was
        // proved to cause stuttering in allocation-heavy apps due to the
        // complex "reject PDE" loops that were required (see Bug 1551532).
        //
        // Another alternative considered was to dynamically grow
        // partial page tables - e.g. migrate from 1/8th to 1/4th as the upper VA
        // is allocated. This would remove the need for static heuristics and
        // place no restriction on VA heap allocation (great!), BUT:
        //
        // 1. WDDMv1 allows paging (mapping with CE) to take place concurrently
        //    with respect to allocation (page table pinning),
        //    so migration is not possible without the pager being able
        //    to synchronize dependencies (WDDMv2). Darn.
        // 2. Even if it were possible, if page tables were migrated through
        //    BAR2 the read performance during the copy would be dreadful.
        //    RM would need internal CE support (e.g. leverage ECC scrubber)
        //    for this to be feasible.
        //
        // Hence, we are using these static heuristics.
        //

        // Bottom of heap.
        _gvaspaceAddPartialPtRange(pGVAS, pdeAlignedVasStart);

        // Handle 1GB offset. See usage of KMD MINIMUM_GPU_VIRTUAL_ADDRESS.
        if ((pdeAlignedVasLimit + 1) > NVBIT64(30))
        {
            _gvaspaceAddPartialPtRange(pGVAS, NVBIT64(30));
        }

        VirtMemAllocator *pDma = GPU_GET_DMA(pGpu);
        // Handle 32-bit restricted pointer ranges.
        if (((pdeAlignedVasLimit + 1) > NVBIT64(32)) &&
            (pDma->bDmaEnforce32BitPointer))
        {
            // Top of 32-bit range.
            _gvaspaceAddPartialPtRange(pGVAS,
                NVBIT64(32) - pGVAS->partialPtVaRangeSize);

            // Bottom of range above 32-bits.
            _gvaspaceAddPartialPtRange(pGVAS, NVBIT64(32));
        }
        // Top of heap.
        _gvaspaceAddPartialPtRange(pGVAS,
            pdeAlignedVasLimit - pGVAS->partialPtVaRangeSize + 1);

        // Reserve the VA holes at the end of each partial PDE.
        for (i = 0; i < pGVAS->numPartialPtRanges; ++i)
        {
            NvU64 off;
            for (off = 0; off < pGVAS->partialPtVaRangeSize; off += fullPdeCoverage)
            {
                EMEMBLOCK  *pBlock;
                NvU32       allocFlags = NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
                NvU64       allocOffset;
                NvU64       allocSize;
                const NvU64 currPdeStart = pGVAS->partialPtVaRangeBase[i] + off;
                const NvU64 nextPdeStart = currPdeStart + fullPdeCoverage;

                // Clamp to VAS start and limit.
                allocOffset = NV_MAX(pVAS->vasStart, currPdeStart + partialSize);

                // Only reserve the hole if the VA limit extends past the partial size.
                if (allocOffset <= pGVAS->vaLimitInternal)
                {
                    allocSize = NV_MIN(pGVAS->vaLimitInternal + 1, nextPdeStart) - allocOffset;

                    status = pGVAS->pHeap->eheapAlloc(pGVAS->pHeap, VAS_EHEAP_OWNER_RSVD,
                                                      &allocFlags, &allocOffset, &allocSize,
                                                      1, 1, &pBlock, NULL, NULL);
                    NV_ASSERT_OR_GOTO(NV_OK == status, catch);
                }
            }
        }
    }

    // Sparsify entire VAS for BAR1
    if (pGVAS->flags & VASPACE_FLAGS_BAR_BAR1)
    {
        // Loop over each GPU associated with VAS.
        FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
        {
            status = _gvaspaceBar1VaSpaceConstruct(pGVAS, pGpu);
            NV_ASSERT(NV_OK == status);
        }
        FOR_EACH_GPU_IN_MASK_UC_END
    }

catch:
    if (status != NV_OK)
    {
        gvaspaceDestruct_IMPL(pGVAS);
    }

    return status;
}

static void
_gvaspaceBar1VaSpaceDestructFW
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu
)
{
    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        return;
    }

    gvaspaceUnpinRootPageDir(pGVAS, pGpu);
}

static NV_STATUS
_gvaspaceBar1VaSpaceDestructClient
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu
)
{
    NV_STATUS         status  = NV_OK;
    OBJVASPACE       *pVAS    = staticCast(pGVAS, OBJVASPACE);
    MMU_WALK_USER_CTX userCtx = {0};

    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {

        NV_ASSERT_OK_OR_RETURN(gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx));

        status = mmuWalkUnmap(userCtx.pGpuState->pWalk, vaspaceGetVaStart(pVAS), vaspaceGetVaLimit(pVAS));

        gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
    }

    return status;
}

static NV_STATUS
_gvaspaceBar1VaSpaceDestruct
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu
)
{
    NV_STATUS status = NV_OK;

    _gvaspaceBar1VaSpaceDestructFW(pGVAS, pGpu);

    status = _gvaspaceBar1VaSpaceDestructClient(pGVAS, pGpu);
    NV_ASSERT_OR_RETURN(status == NV_OK, status);

    return status;
}

static NV_STATUS
_gvaspaceFlaVaspaceDestruct
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu
)
{
    NV_STATUS status = NV_OK;
    MMU_WALK_USER_CTX userCtx = {0};
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    OBJVASPACE *pVAS = staticCast(pGVAS, OBJVASPACE);

    gvaspaceUnpinRootPageDir(pGVAS, pGpu);

    if (gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx) == NV_OK)
    {
        NV_ASSERT_OK_OR_RETURN(mmuWalkUnmap(userCtx.pGpuState->pWalk, vaspaceGetVaStart(pVAS), vaspaceGetVaLimit(pVAS)));
        gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
    }
    else
    {
        NV_PRINTF(LEVEL_WARNING,
            "Failed to acquire walk user context\n");
    }

    NV_PRINTF(LEVEL_INFO, "Releasing legacy FLA VASPACE, gpu: %x \n",
            pGpu->gpuInstance);

    pKernelBus->flaInfo.hFlaVASpace = NV01_NULL_OBJECT;
    pKernelBus->flaInfo.pFlaVAS = NULL;

    return status;
}

static NV_STATUS
_gvaspaceReleaseVaForServerRm
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu
)
{
    NV_STATUS         status   = NV_OK;
    MMU_WALK_USER_CTX userCtx  = {0};
    NV_ASSERT_OK_OR_RETURN(gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx));

    const MMU_FMT_LEVEL *pLevelFmt =
           mmuFmtFindLevelWithPageShift(userCtx.pGpuState->pFmt->pRoot, GMMU_PD0_VADDR_BIT_LO);
    status = mmuWalkReleaseEntries(userCtx.pGpuState->pWalk,
                                   pLevelFmt,
                                   pGVAS->vaStartServerRMOwned,
                                   pGVAS->vaLimitServerRMOwned);

    gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);

    return status;
}

void
gvaspaceDestruct_IMPL(OBJGVASPACE *pGVAS)
{
    // Destroy BC state.
    if (NULL != pGVAS->pHeap)
    {
        pGVAS->pHeap->eheapTraverse(pGVAS->pHeap,
                                    pGVAS,
                                    _gvaspaceFreeVASBlock,
                                    1 /*forwards*/);
        pGVAS->pHeap->eheapDestruct(pGVAS->pHeap);
        portMemFree(pGVAS->pHeap);
        pGVAS->pHeap = NULL;
    }

    // Destroy channel group map
    if (mapCount(&pGVAS->chanGrpMap))
    {
        NV_ASSERT(0);
        NV_PRINTF(LEVEL_ERROR,
                  "GVAS is still used by some channel group(s)\n");
    }
    mapDestroy(&pGVAS->chanGrpMap);

    // Destroy per-GPU state.
    if (NULL != pGVAS->pGpuStates)
    {
        OBJVASPACE      *pVAS = staticCast(pGVAS, OBJVASPACE);
        OBJGPU          *pGpu = NULL;
        GVAS_GPU_STATE  *pGpuState;
        NV_STATUS        status;

        FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
        {
            // Unsparsify entire VAS for BAR1.
            if (pGVAS->flags & VASPACE_FLAGS_BAR_BAR1)
            {
                status = _gvaspaceBar1VaSpaceDestruct(pGVAS, pGpu);
                NV_ASSERT(NV_OK == status);
            }

            if (pGVAS->flags & VASPACE_FLAGS_FLA)
            {
                status = _gvaspaceFlaVaspaceDestruct(pGVAS, pGpu);
                NV_ASSERT(NV_OK == status);
            }
        }
        FOR_EACH_GPU_IN_MASK_UC_END

        // Release the PDEs for the server owned portion of the VA range
        FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
        {
            NvBool bClientRm = (IS_VIRTUAL_WITH_SRIOV(pGpu) || IS_GSP_CLIENT(pGpu));

            if (bClientRm && (0 != pGVAS->vaStartServerRMOwned))
            {
                NV_ASSERT(NV_OK == _gvaspaceReleaseVaForServerRm(pGVAS, pGpu));
            }
        }
        FOR_EACH_GPU_IN_MASK_UC_END

        FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
        {
            pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
            _gvaspaceGpuStateDestruct(pGVAS, pGpu, pGpuState);
        }
        FOR_EACH_GPU_IN_MASK_UC_END

        //
        // Release the Big Page Table (BPT) caches *only* after all page level
        // updates have been completed on all the GPUs in SLI. Destroying the
        // cache on one GPU with unreleased BPT instances on another GPU can
        // cause memory leaks in a SLI scenario. This is because in SLI, a GPU
        // can share a BPT instance from another GPU's cache.
        //
        FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
        {
            pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
            gmmuMemDescCacheFree(pGpuState);
        }
        FOR_EACH_GPU_IN_MASK_UC_END

        portMemFree(pGVAS->pGpuStates);
        pGVAS->pGpuStates = NULL;
    }
}

/*!
 * Add a region of VA reserved for partial page tables.
 */
static void
_gvaspaceAddPartialPtRange
(
    OBJGVASPACE *pGVAS,
    const NvU64  va
)
{
    NV_ASSERT_OR_RETURN_VOID(pGVAS->numPartialPtRanges <
                          GVAS_MAX_PARTIAL_PAGE_TABLE_RANGES);

    // Only add the range if it is first range or above the previous range.
    if ((0 == pGVAS->numPartialPtRanges) ||
        (va >= (pGVAS->partialPtVaRangeBase[pGVAS->numPartialPtRanges - 1] +
                pGVAS->partialPtVaRangeSize)))
    {
        pGVAS->partialPtVaRangeBase[pGVAS->numPartialPtRanges] = va;
        pGVAS->numPartialPtRanges++;
    }
}

/*!
 * Construct unicast GPU state associated with a VAS and reconcile
 * differences between GMMU settings (currently must be homogenous).
 */
static NV_STATUS
_gvaspaceGpuStateConstruct
(
    OBJGVASPACE    *pGVAS,
    OBJGPU         *pGpu,
    GVAS_GPU_STATE *pGpuState,
    const NvU64     reqBigPageSize,
    const NvU64     vaStart,
    const NvU64     vaLimit,
    const NvU64     vaStartInternal,
    const NvU64     vaLimitInternal,
    const NvU32     flags,
    const NvBool    bFirst,
    NvU64          *pFullPdeCoverage,
    NvU32          *pPartialPdeExpMax
)
{
    OBJVASPACE          *pVAS  = staticCast(pGVAS, OBJVASPACE);
    KernelGmmu          *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NvU64                pageSizeMask;
    NvU64                vaStartMin;
    NvU64                vaLimitMax;
    NvU64                bigPageSize;
    NvU64                compPageSize;
    NvU64                extManagedAlign;
    NvU64                vaLimitExt;
    const GMMU_FMT      *pFmt;
    const MMU_FMT_LEVEL *pBigPT;
    MMU_WALK_FLAGS       walkFlags = {0};
    NvU64                fullPdeCoverage;
    NvU32                partialPdeExpMax = 0;
    NvU64                vaStartInt = 0;
    NvU64                vaLimitInt = 0;
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    // Must be in UC.
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    if (RMCFG_FEATURE_PMA &&
       (flags & VASPACE_FLAGS_PTETABLE_PMA_MANAGED))
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RsResourceRef *pDeviceRef = pCallContext->pResourceRef;
        Device *pDevice;

        NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

        pDeviceRef = pCallContext->pResourceRef;
        if (pDeviceRef->internalClassId != classId(Device))
        {
            NV_ASSERT_OK_OR_RETURN(refFindAncestorOfType(pDeviceRef, classId(Device), &pDeviceRef));
        }

        pDevice = dynamicCast(pDeviceRef->pResource, Device);
        NV_ASSERT_OR_RETURN(pDevice != NULL, NV_ERR_INVALID_STATE);

        NV_ASSERT_OK_OR_RETURN(
           memmgrPageLevelPoolsGetInfo(pGpu, pMemoryManager, pDevice, &pGpuState->pPageTableMemPool));
    }

    // Get GMMU format for this GPU.
    pFmt = kgmmuFmtGet(pKernelGmmu, GMMU_FMT_VERSION_DEFAULT, reqBigPageSize);
    NV_ASSERT_OR_RETURN(NULL != pFmt, NV_ERR_NOT_SUPPORTED);
    pGpuState->pFmt = pFmt;

    // UVM mirroring works only with pre-Pascal format.
    if (flags & VASPACE_FLAGS_SET_MIRRORED)
    {
        NV_ASSERT_OR_RETURN(GMMU_FMT_VERSION_1 == pFmt->version, NV_ERR_ILLEGAL_ACTION);
    }

    // Determine GPU's page size settings.
    pageSizeMask = mmuFmtAllPageSizes(pFmt->pRoot);
    bigPageSize = pageSizeMask & (RM_PAGE_SIZE_64K | RM_PAGE_SIZE_128K);

    compPageSize = pMemorySystemConfig->comprPageSize;

    // Determine externally managed VA alignment from big page table coverage.
    pBigPT = mmuFmtFindLevelWithPageShift(pFmt->pRoot, nvLogBase2(bigPageSize));
    NV_ASSERT_OR_RETURN(NULL != pBigPT, NV_ERR_INVALID_ARGUMENT);
    extManagedAlign = NVBIT64(pBigPT->virtAddrBitHi + 1);

    // Determine partial page table parameters.
    fullPdeCoverage = mmuFmtLevelVirtAddrMask(pBigPT) + 1;
    if (nvFieldIsValid32(&pFmt->pPdeMulti->fldSizeRecipExp))
    {
        partialPdeExpMax = pFmt->pPdeMulti->fldSizeRecipExp.maskPos >>
                           pFmt->pPdeMulti->fldSizeRecipExp.shift;
    }

    // set VA start address to non-zero reserved VA space base.
    vaStartMin = gvaspaceGetReservedVaspaceBase(pGVAS, pGpu);

    vaLimitMax = NVBIT64(pFmt->pRoot->virtAddrBitHi + 1) - 1;

    // Calculate the desired internal and external VAS limits.
    if (0 == vaLimit)
    {
        // Default: allow maximum VAS limit.
        vaLimitExt = vaLimitMax;
    }
    else
    {
        // Otherwise ensure requested limit does not exeed max HW limit.
        NV_CHECK_OR_RETURN(LEVEL_ERROR, vaLimit <= vaLimitMax, NV_ERR_INVALID_ARGUMENT);

        vaLimitExt = vaLimit;
    }

    if (flags & VASPACE_FLAGS_RESTRICTED_RM_INTERNAL_VALIMITS)
    {
        NV_ASSERT_OR_RETURN(vaLimitInternal <= vaLimitMax, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(vaStartInternal <= vaLimitInternal, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(vaStartInternal >= vaStartMin, NV_ERR_INVALID_ARGUMENT);

        vaStartInt = vaStartInternal;
        vaLimitInt = vaLimitInternal;
    }
    else
    {
        vaStartInt = vaStart;
        vaLimitInt = vaLimitExt;
    }

    //
    // Shared management external limit is aligned to root PDE coverage.
    // This allows KMD/OS to hook external PDEs beneath an RM-allocated root.
    //
    if (flags & VASPACE_FLAGS_SHARED_MANAGEMENT)
    {
        vaLimitExt = NV_ALIGN_UP64(vaLimitExt + 1, mmuFmtLevelPageSize(pFmt->pRoot)) - 1;
    }

    // First GPU sets the precedent.
    if (bFirst)
    {
        pGVAS->bigPageSize  = bigPageSize;
        pGVAS->compPageSize = compPageSize;
        pGVAS->extManagedAlign = extManagedAlign;

        //
        // Determine VAS start and limit.
        // vaStart of 0 is allowed if explicitly requested (e.g. BAR1).
        //
        if ((0 == vaStart) && !(flags & VASPACE_FLAGS_ALLOW_ZERO_ADDRESS))
        {
            pVAS->vasStart = vaStartMin;
        }
        else
        {
            pVAS->vasStart = vaStart;
        }

        if (vaStartInt == 0)
        {
            vaStartInt = pVAS->vasStart;
        }

        pGVAS->vaStartInternal = vaStartInt;
        pGVAS->vaLimitInternal = vaLimitInt;

        pVAS->vasLimit         = vaLimitExt;
        pGVAS->vaLimitInternal = vaLimitInt;
        pGVAS->vaLimitMax      = vaLimitMax;
        *pFullPdeCoverage      = fullPdeCoverage;
        *pPartialPdeExpMax     = partialPdeExpMax;
    }
    // Remaining must either match or take best-fit.
    else
    {
        NV_ASSERT_OR_RETURN(bigPageSize == pGVAS->bigPageSize, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(compPageSize == pGVAS->compPageSize, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(extManagedAlign == pGVAS->extManagedAlign, NV_ERR_INVALID_ARGUMENT);
        if ((0 == vaStart) && !(flags & VASPACE_FLAGS_ALLOW_ZERO_ADDRESS))
        {
            pVAS->vasStart = NV_MAX(pVAS->vasStart, vaStartMin);
        }
        pVAS->vasLimit         = NV_MIN(pVAS->vasLimit,         vaLimitExt);
        pGVAS->vaStartInternal = NV_MAX(pGVAS->vaStartInternal, vaStartInt);
        pGVAS->vaLimitInternal = NV_MIN(pGVAS->vaLimitInternal, vaLimitInt);
        pGVAS->vaLimitMax      = NV_MIN(pGVAS->vaLimitMax,      vaLimitMax);
        NV_ASSERT_OR_RETURN(*pFullPdeCoverage  == fullPdeCoverage,  NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(*pPartialPdeExpMax == partialPdeExpMax, NV_ERR_INVALID_ARGUMENT);
    }

    //
    // Create MMU walker library state.
    // Set ats flag to enable related functionality/functionalities in MMU walker
    // e.g. NV4K state for 64K PTEs
    //
    walkFlags.bAtsEnabled = gvaspaceIsAtsEnabled(pGVAS);
    NV_ASSERT_OK_OR_RETURN(
        mmuWalkCreate(pFmt->pRoot, NULL,
                      &g_gmmuWalkCallbacks,
                      walkFlags,
                      &pGpuState->pWalk,
                      NULL));

    listInit(&pGpuState->reservedPageTableEntries,
             portMemAllocatorGetGlobalNonPaged());

    listInitIntrusive(&pGpuState->unpackedMemDescList);

    return NV_OK;
}

/*!
 * Destruct unicast GPU state associated with a VAS.
 */
static void
_gvaspaceGpuStateDestruct
(
    OBJGVASPACE    *pGVAS,
    OBJGPU         *pGpu,
    GVAS_GPU_STATE *pGpuState
)
{
    NV_ASSERT_OR_RETURN_VOID(!gpumgrGetBcEnabledStatus(pGpu));
    if (NULL != pGpuState->pRootInternal)
    {
        // Cleanup if client didn't call UnsetPageDir.
        NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS params = {0};
        NV_STATUS                                   status;
        status = gvaspaceExternalRootDirRevoke(pGVAS, pGpu, &params);
        NV_ASSERT(NV_OK == status);
    }

    //
    // Force free all page level instances. This can come in
    // handy on systems that support surprise removal.
    //
    _gvaspaceForceFreePageLevelInstances(pGVAS, pGpu, pGpuState);

    mmuWalkDestroy(pGpuState->pWalk);

    if (pGpuState->pPageTableMemPool != NULL)
        rmMemPoolRelease(pGpuState->pPageTableMemPool, pGVAS->flags);

    pGpuState->pWalk = NULL;
    NV_ASSERT(NULL == pGpuState->pMirroredRoot);

    NV_ASSERT(NULL == listHead(&pGpuState->reservedPageTableEntries));
    listDestroy(&pGpuState->reservedPageTableEntries);
}

static void
_gvaspaceCleanupFlaDummyPagesForFlaRange
(
    OBJGVASPACE    *pGVAS,
    OBJGPU         *pGpu,
    GVAS_GPU_STATE *pGpuState
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    if (pGpuState->flaDummyPage.hMemory != NV01_NULL_OBJECT)
    {
        portMemSet(&pGpuState->flaDummyPage.pte, 0, sizeof(pGpuState->flaDummyPage.pte));
        pRmApi->Free(pRmApi, pKernelBus->flaInfo.hClient, pGpuState->flaDummyPage.hMemory);
        pGpuState->flaDummyPage.hMemory =  NV01_NULL_OBJECT;
    }
}

static NV_STATUS
_gvaspaceAllocateFlaDummyPagesForFlaRange
(
    OBJGVASPACE    *pGVAS,
    OBJGPU         *pGpu,
    GVAS_GPU_STATE *pGpuState
)
{
    NV_STATUS                   status;
    KernelGmmu                 *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    KernelBus                  *pKernelBus  = GPU_GET_KERNEL_BUS(pGpu);
    const GMMU_FMT_FAMILY      *pFam = kgmmuFmtGetFamily(pKernelGmmu, pGpuState->pFmt->version);
    NvU64                       addr;
    NvBool                      bAcquireLock = NV_FALSE;
    RM_API                     *pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams;
    RsClient                   *pClient;
    Memory                     *pMemory;

    if (!kbusIsFlaDummyPageEnabled(pKernelBus))
        return NV_OK;

    portMemSet(&memAllocParams, 0, sizeof(memAllocParams));
    memAllocParams.owner     = VAS_EHEAP_OWNER_NVRM;
    memAllocParams.size      = RM_PAGE_SIZE_64K;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.attr      = DRF_DEF(OS32, _ATTR, _LOCATION, _VIDMEM) |
                               DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _BIG) |
                               DRF_DEF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS);
    memAllocParams.flags     = NVOS32_ALLOC_FLAGS_PERSISTENT_VIDMEM |
                               NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE;
    memAllocParams.alignment = RM_PAGE_SIZE_64K;

    NV_ASSERT_OK_OR_GOTO(status,
       serverutilGenResourceHandle(pKernelBus->flaInfo.hClient, &pGpuState->flaDummyPage.hMemory),
       cleanup);

    //
    // Allocate memory using vidHeapControl
    //
    // vidHeapControl calls should happen outside GPU locks
    // This is a PMA requirement as memory allocation calls may invoke eviction
    // which UVM could get stuck behind GPU lock
    //
    if (rmDeviceGpuLockIsOwner(pGpu->gpuInstance) || rmGpuLockIsOwner())
    {
        rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
        bAcquireLock = NV_TRUE;
        pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
    }

    status = pRmApi->AllocWithHandle(pRmApi, pKernelBus->flaInfo.hClient, pKernelBus->flaInfo.hSubDevice,
                                    pGpuState->flaDummyPage.hMemory, NV01_MEMORY_LOCAL_USER,
                                    &memAllocParams, sizeof(memAllocParams));

    if (bAcquireLock)
    {
        // Reacquire the GPU locks
        if (rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_MEM) != NV_OK)
        {
            NV_ASSERT(0);
            status = NV_ERR_GENERIC;
            goto cleanup;
        }
        bAcquireLock = NV_FALSE;
    }

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to allocate dummy page for FLA, status: %x\n", status);
        goto cleanup;
    }

    NV_ASSERT_OK_OR_GOTO(status,
                         serverGetClientUnderLock(&g_resServ, pKernelBus->flaInfo.hClient, &pClient),
                         cleanup);

    NV_ASSERT_OK_OR_GOTO(status,
                         memGetByHandle(pClient, pGpuState->flaDummyPage.hMemory, &pMemory),
                         cleanup);

    // prefill the big pte
    const GMMU_APERTURE pgAperture = kgmmuGetMemAperture(pKernelGmmu, pMemory->pMemDesc);

    nvFieldSetBool(&pFam->pte.fldValid, NV_TRUE, pGpuState->flaDummyPage.pte.v8);
    nvFieldSetBool(&pFam->pte.fldVolatile, memdescGetVolatility(pMemory->pMemDesc),
                    pGpuState->flaDummyPage.pte.v8);
    gmmuFieldSetAperture(&pFam->pte.fldAperture, pgAperture,
                            pGpuState->flaDummyPage.pte.v8);

    addr = kgmmuEncodePhysAddr(pKernelGmmu, pgAperture,
                       memdescGetPhysAddr(pMemory->pMemDesc, AT_GPU, 0),
                       NVLINK_INVALID_FABRIC_ADDR);

    gmmuFieldSetAddress(gmmuFmtPtePhysAddrFld(&pFam->pte, pgAperture), addr, pGpuState->flaDummyPage.pte.v8);

    return NV_OK;

cleanup:
    _gvaspaceCleanupFlaDummyPagesForFlaRange(pGVAS, pGpu, pGpuState);
    return status;
}

NV_STATUS
gvaspaceAlloc_IMPL
(
    OBJGVASPACE     *pGVAS,
    NvU64            size,
    NvU64            align,
    NvU64            rangeLo,
    NvU64            rangeHi,
    NvU64            pageSizeLockMask,
    VAS_ALLOC_FLAGS  flags,
    NvU64           *pAddr
)
{
    OBJVASPACE *pVAS       = staticCast(pGVAS, OBJVASPACE);
    OBJGPU     *pGpu       = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));
    KernelBus  *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU32       eheapFlags = 0;
    NV_STATUS   status     = NV_OK;
    OBJEHEAP   *pHeap      = pGVAS->pHeap;
    EMEMBLOCK  *pMemBlock;
    GVAS_BLOCK *pVASBlock;
    NvU64       origRangeLo = pHeap->rangeLo;
    NvU64       origRangeHi = pHeap->rangeHi;

    if (pGVAS->bIsExternallyOwned)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Cannot reserve VA on an externally owned VASPACE\n");

        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // TODO: To be removed after pKernelBus->flaInfo.pFlaVAS is removed.
    // In case of FLA vaspace, check that fabric vaspace is not in use.
    //
    if ((pVAS == pKernelBus->flaInfo.pFlaVAS) && (pGpu->pFabricVAS != NULL))
    {
        FABRIC_VASPACE *pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);

        if (gvaspaceIsInUse(dynamicCast(pFabricVAS->pGVAS, OBJGVASPACE)))
        {
            NV_PRINTF(LEVEL_ERROR, "FabricVAS and FlaVAS cannot be used simultaneously! "
                      "FlaVAS Alloc failed\n");
            return NV_ERR_INVALID_OPERATION;
        }
    }

    // Clip the input range to the original range.
    rangeLo = NV_MAX(rangeLo, origRangeLo);
    rangeHi = NV_MIN(rangeHi, origRangeHi);

    // Check RM's internal allocation is only used.
    if (gvaspaceIsInternalVaRestricted(pGVAS))
    {
        if (!flags.bClientAllocation) // ignore the 32bit restriction here.
        {
            rangeLo = pGVAS->vaStartInternal;
            rangeHi = pGVAS->vaLimitInternal;
        }
        else
        {
            // Fixed address range Check

            // Does not interfere with RM internal VA range.
            if (flags.bFixedAddressRange &&
                 ((rangeLo >= pGVAS->vaStartInternal && rangeLo <= pGVAS->vaLimitInternal) || \
                 (rangeHi <= pGVAS->vaLimitInternal && rangeHi >= pGVAS->vaStartInternal)))
            {
                return NV_ERR_INVALID_PARAMETER;
            }

            // Flexible address range

            // Place above RM va internal as much as possible
            if (!flags.bFixedAddressRange && !(rangeHi < pGVAS->vaStartInternal || rangeLo > pGVAS->vaLimitInternal))
            {
                if ((rangeHi > pGVAS->vaLimitInternal) && (rangeHi - pGVAS->vaLimitInternal) >= size)
                {
                    rangeLo = pGVAS->vaLimitInternal + 1;
                }
                else if (rangeLo < pGVAS->vaStartInternal && pGVAS->vaStartInternal - rangeLo  >= size)
                {
                    rangeHi = pGVAS->vaStartInternal - 1;
                }
                else
                {
                    return NV_ERR_INSUFFICIENT_RESOURCES;
                }
                // else do nothing as the ranges are disjoint
            }
        }
    }

    //
    // If this address space is marked as mirrored, then we will
    // cap user allocations to be under the top PDE.
    // If the allocations are privileged, then we will restrict the
    // allocations to the top PDE.
    //
    if (pGVAS->bIsMirrored)
    {
        if (flags.bPrivileged)
        {
            //
            // This is a kernel allocation so restrict the Allocations to
            // the topmost PDE.
            //
            rangeLo = NV_MAX(rangeLo, pGVAS->vaLimitInternal -
                                      UVM_KERNEL_PRIVILEGED_REGION_LENGTH + 1);
            rangeHi = NV_MIN(rangeHi, pGVAS->vaLimitInternal);

            // Verify the allocation range is within UVM_PRIVILEGED_REGION
            NV_ASSERT_OR_RETURN(rangeLo >= UVM_KERNEL_PRIVILEGED_REGION_START,
                              NV_ERR_OUT_OF_RANGE);
            NV_ASSERT_OR_RETURN(rangeHi < UVM_KERNEL_PRIVILEGED_REGION_START +
                                        UVM_KERNEL_PRIVILEGED_REGION_LENGTH,
                              NV_ERR_OUT_OF_RANGE);
        }
        else
        {
            //
            // This is a user space allocation. Restrict allocation from the last PDB
            // because that is privileged
            // vaRangeLo can still be based on the users override. We will return an error
            // if the user requested for an address in the last PDE range
            //
            rangeHi = NV_MIN(rangeHi, pGVAS->vaLimitInternal -
                                      UVM_KERNEL_PRIVILEGED_REGION_LENGTH);

            // Verify range is not in the priviledged region.
            NV_ASSERT_OR_RETURN(rangeHi < UVM_KERNEL_PRIVILEGED_REGION_START,
                              NV_ERR_OUT_OF_RANGE);
        }
    }

    //
    // Sanity check the range before applying to eheap since
    // eheapSetAllocRange auto-clips (silencing potential range bugs).
    //

    // This first check isn't an assert to avoid log spam from trying to map an
    // allocation too large for the provided (or locally modified) range.
    NV_CHECK_OR_RETURN(LEVEL_NOTICE, size <= (rangeHi - rangeLo + 1), NV_ERR_INVALID_ARGUMENT);

    // Everything else is expected to adhere to the assertion conditions.
    NV_ASSERT_OR_RETURN(origRangeLo <= rangeLo,          NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(rangeLo <= rangeHi,              NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(rangeHi <= origRangeHi,          NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OK_OR_RETURN(pHeap->eheapSetAllocRange(pHeap, rangeLo, rangeHi));
    // !!! All return paths after this point must "goto catch" to restore. !!!

    // Honor reverse flag for non-BAR VA spaces.
    if (flags.bReverse || (pGVAS->flags & VASPACE_FLAGS_REVERSE))
    {
        eheapFlags |= NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN;
    }

    if (flags.bFixedAddressAllocate)
    {
        eheapFlags |= NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
    }

    // Attempt to allocate VA space of the size and alignment requested.
    if (NV_OK != pHeap->eheapAlloc(pHeap, VAS_EHEAP_OWNER_NVRM, &eheapFlags,
                                   pAddr, &size, align, 1,
                                   &pMemBlock, NULL, NULL))
    {
        status = NV_ERR_NO_MEMORY;
        goto catch;
    }
    pVASBlock = (PGVAS_BLOCK)pMemBlock->pData;

    // Save flags for VA initialization
    pVASBlock->flags = flags;
    pVASBlock->pageSizeLockMask = pageSizeLockMask;

    if (flags.bExternallyManaged)
    {
        pVASBlock->management = VA_MANAGEMENT_PDES_ONLY;
    }

    //
    // VA reserved as sparse is sparsified immediately, changing its
    // unmapped state from "invalid" to "zero."
    //
    if (flags.bSparse || (pGVAS->flags & VASPACE_FLAGS_BAR_BAR1))
    {
        // Loop over each GPU associated with VAS.
        FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
        {
            KernelBus  *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
            MMU_WALK_USER_CTX userCtx = {0};

            // Sparsify the VA range.
            status = gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, pVASBlock, &userCtx);
            NV_ASSERT(status == NV_OK);

            if (status == NV_OK)
            {
                status = mmuWalkSparsify(userCtx.pGpuState->pWalk, *pAddr,
                                         *pAddr + size - 1, NV_FALSE);
                gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
            }
            if (NV_OK != status)
            {
                DBG_BREAKPOINT();
                break;
            }

            // Invalidate TLB to apply new sparse state.
            kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_VIDEO_MEMORY  |
                                            BUS_FLUSH_SYSTEM_MEMORY);
            gvaspaceInvalidateTlb(pGVAS, pGpu, PTE_UPGRADE);
        }
        FOR_EACH_GPU_IN_MASK_UC_END
        if (NV_OK != status)
        {
            FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
            {
                KernelBus  *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
                MMU_WALK_USER_CTX userCtx = {0};

                // Unsparsify the VA range.
                NV_STATUS acquireStatus = gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, pVASBlock, &userCtx);
                NV_ASSERT(acquireStatus == NV_OK);
                if (acquireStatus == NV_OK)
                {
                    // Not checking the returns status
                    mmuWalkUnmap(userCtx.pGpuState->pWalk,
                              pMemBlock->begin, pMemBlock->end);
                    gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
                }

                // Invalidate TLB to apply new sparse state.
                kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_VIDEO_MEMORY  |
                                         BUS_FLUSH_SYSTEM_MEMORY);
                gvaspaceInvalidateTlb(pGVAS, pGpu, PTE_UPGRADE);
            }
            FOR_EACH_GPU_IN_MASK_UC_END

            goto catch;
        }
    }
    // Pin page tables upfront for non-lazy, non-external VA reservations.
    else if (!(flags.bLazy || flags.bExternallyManaged) &&
             (0 != pVASBlock->pageSizeLockMask))
    {
        // Loop over each GPU associated with VAS.
        FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
        {
            NvU32             pageShift;
            MMU_WALK_USER_CTX userCtx = {0};

            status = gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, pVASBlock, &userCtx);
            NV_ASSERT_OR_ELSE(status == NV_OK, break);


            if (pGVAS->flags & VASPACE_FLAGS_FLA)
            {
                // currently FLA VASpace is associated with only GPU.
                NV_ASSERT(ONEBITSET(pVAS->gpuMask));
                status = _gvaspaceAllocateFlaDummyPagesForFlaRange(pGVAS, pGpu, userCtx.pGpuState);
            }
            // Loop over each page size requested by client.
            FOR_EACH_INDEX_IN_MASK(64, pageShift, pVASBlock->pageSizeLockMask)
            {
                // Pre-reserve page level instances in the VA range.
                const MMU_FMT_LEVEL *pLevelFmt =
                    mmuFmtFindLevelWithPageShift(userCtx.pGpuState->pFmt->pRoot, pageShift);
                status = mmuWalkReserveEntries(userCtx.pGpuState->pWalk, pLevelFmt,
                                               *pAddr, *pAddr + size - 1, NV_TRUE);
                if (NV_OK != status)
                {
                    DBG_BREAKPOINT();
                    break;
                }
            }
            FOR_EACH_INDEX_IN_MASK_END
            gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);


            if (NV_OK != status)
            {
                break;
            }
        }
        FOR_EACH_GPU_IN_MASK_UC_END
        if (NV_OK != status)
        {
            // Unpin page tables for each GPU associated with VAS.
            FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
            {
                NvU32             pageShift;
                MMU_WALK_USER_CTX userCtx = {0};
                NV_STATUS acquireStatus = gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, pVASBlock, &userCtx);
                NV_ASSERT_OR_ELSE(acquireStatus == NV_OK, continue);

                if (pGVAS->flags & VASPACE_FLAGS_FLA)
                {
                    _gvaspaceCleanupFlaDummyPagesForFlaRange(pGVAS, pGpu, userCtx.pGpuState);
                }
                // Loop over each page size requested by client during VA reservation.
                FOR_EACH_INDEX_IN_MASK(64, pageShift, pVASBlock->pageSizeLockMask)
                {
                    // Release page level instances in the VA range.
                    const MMU_FMT_LEVEL *pLevelFmt =
                        mmuFmtFindLevelWithPageShift(userCtx.pGpuState->pFmt->pRoot, pageShift);
                   // Not checking the returns status
                   mmuWalkReleaseEntries(userCtx.pGpuState->pWalk, pLevelFmt,
                                         pMemBlock->begin, pMemBlock->end);
                }
                FOR_EACH_INDEX_IN_MASK_END
                gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
            }
            FOR_EACH_GPU_IN_MASK_UC_END
            goto catch;
        }
    }

catch:
    pHeap->eheapSetAllocRange(pHeap, origRangeLo, origRangeHi);
    return status;
}

static NV_STATUS
_gvaspaceInternalFree
(
    OBJGVASPACE  *pGVAS,
    NvU64         vAddr,
    EMEMBLOCK    *pMemBlock
)
{
    PGVAS_BLOCK   pVASBlock;
    GVAS_MAPPING *pMapNode;
    OBJVASPACE   *pVAS   = staticCast(pGVAS, OBJVASPACE);
    OBJGPU       *pGpu   = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));
    NV_STATUS     status = NV_OK;

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    pVASBlock = (PGVAS_BLOCK)pMemBlock->pData;

    if (pMemBlock->refCount > 1)
    {
        pMemBlock->refCount--;
        return NV_OK;
    }

    // Before unmapping any CPU visible surfaces, make sure any CPU writes are flushed to L2.
    if (pGVAS->flags & VASPACE_FLAGS_BAR)
    {
        // Loop over each GPU associated with VAS.
        FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
        {
            KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
            kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_VIDEO_MEMORY  |
                                            BUS_FLUSH_SYSTEM_MEMORY);
        }
        FOR_EACH_GPU_IN_MASK_UC_END
    }

    //
    // Unmap any leaked mappings.
    //
    btreeEnumStart(0, (NODE**)&pMapNode, &pVASBlock->pMapTree->node);
    while (NULL != pMapNode)
    {
        FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pMapNode->gpuMask)
        {
            gvaspaceUnmap(pGVAS, pGpu, pMapNode->node.keyStart, pMapNode->node.keyEnd);
        }
        FOR_EACH_GPU_IN_MASK_UC_END

        btreeEnumStart(0, (NODE**)&pMapNode, &pVASBlock->pMapTree->node);
    }

    // Unpin page tables for each GPU associated with VAS.
    FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
    {
        GVAS_GPU_STATE *pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
        NvU32           pageShift;

        NV_ASSERT(NULL != pGpuState);

        if (NULL != pGpuState)
        {
            FOR_EACH_INDEX_IN_MASK(64, pageShift, pVASBlock->pageSizeLockMask)
            {
                // Release page level instances in the VA range.
                const MMU_FMT_LEVEL *pLevelFmt =
                    mmuFmtFindLevelWithPageShift(pGpuState->pFmt->pRoot,
                                                 pageShift);

                status = _gvaspaceReleaseUnreservedPTEs(pGVAS, pGpu,
                                                        pMemBlock->begin,
                                                        pMemBlock->end,
                                                        pLevelFmt);
                NV_ASSERT(NV_OK == status);
            }
            FOR_EACH_INDEX_IN_MASK_END
        }
    }
    FOR_EACH_GPU_IN_MASK_UC_END

    if (!pVASBlock->flags.bSkipTlbInvalidateOnFree)
    {
        // Invalidate TLB on each GPU associated with VAS.
        FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
        {
            KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
            kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_VIDEO_MEMORY  |
                                            BUS_FLUSH_SYSTEM_MEMORY);
            gvaspaceInvalidateTlb(pGVAS, pGpu, PTE_DOWNGRADE);
        }
        FOR_EACH_GPU_IN_MASK_UC_END
    }

    pGVAS->pHeap->eheapFree(pGVAS->pHeap, pMemBlock->begin);

    return NV_OK;
}

NV_STATUS
gvaspaceFree_IMPL
(
    OBJGVASPACE  *pGVAS,
    NvU64         vAddr
)
{
    EMEMBLOCK *pMemBlock = pGVAS->pHeap->eheapGetBlock(pGVAS->pHeap, vAddr, 0);
    NV_ASSERT_OR_RETURN(NULL != pMemBlock, NV_ERR_INVALID_ARGUMENT);

    return _gvaspaceInternalFree(pGVAS, vAddr, pMemBlock);
}

NV_STATUS
gvaspaceApplyDefaultAlignment_IMPL
(
    OBJGVASPACE         *pGVAS,
    const FB_ALLOC_INFO *pAllocInfo,
    NvU64               *pAlign,
    NvU64               *pSize,
    NvU64               *pPageSizeLockMask
)
{
    OBJVASPACE *pVAS = staticCast(pGVAS, OBJVASPACE);
    OBJGPU     *pGpu = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));
    NvU64       bigPageSize  = pGVAS->bigPageSize;
    NvU64       maxPageSize  = RM_PAGE_SIZE;
    NvU64       compPageSize = pGVAS->compPageSize;
    NvU64       pageSizeMask = 0;

    //
    // In L2 cache only mode, force the page size to 4K in order to
    // conserve memory, otherwise we end up wasting a lot of memory
    // aligning allocations to the big page size
    //
    if (gpuIsCacheOnlyModeEnabled(pGpu))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Overriding page size to 4k in Cache only Mode\n");
        pageSizeMask |= RM_PAGE_SIZE;
    }
    else
    {
        KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

        // Determine the page size to align to based on user hint.
        switch (dmaNvos32ToPageSizeAttr(pAllocInfo->pageFormat->attr, pAllocInfo->pageFormat->attr2))
        {
            case RM_ATTR_PAGE_SIZE_4KB:
                pageSizeMask |= RM_PAGE_SIZE;
                break;
            case RM_ATTR_PAGE_SIZE_DEFAULT:
                pageSizeMask |= RM_PAGE_SIZE;
                pageSizeMask |= bigPageSize;
                maxPageSize   = bigPageSize;

                if (FLD_TEST_DRF(OS32, _ATTR, _PAGE_SIZE, _HUGE, pAllocInfo->retAttr))
                {
                    NV_ASSERT_OR_RETURN(kgmmuIsHugePageSupported(pKernelGmmu),
                                        NV_ERR_NOT_SUPPORTED);
                    pageSizeMask |= RM_PAGE_SIZE_HUGE;
                    maxPageSize   = RM_PAGE_SIZE_HUGE;
                }
                break;
            case RM_ATTR_PAGE_SIZE_BIG:
                pageSizeMask |= bigPageSize;
                maxPageSize   = bigPageSize;
                break;
            case RM_ATTR_PAGE_SIZE_HUGE:
                NV_ASSERT_OR_RETURN(kgmmuIsHugePageSupported(pKernelGmmu),
                                    NV_ERR_NOT_SUPPORTED);
                pageSizeMask |= RM_PAGE_SIZE_HUGE;
                maxPageSize   = RM_PAGE_SIZE_HUGE;
                break;
            case RM_ATTR_PAGE_SIZE_512MB:
                NV_ASSERT_OR_RETURN(kgmmuIsPageSize512mbSupported(pKernelGmmu),
                                  NV_ERR_NOT_SUPPORTED);
                pageSizeMask |= RM_PAGE_SIZE_512M;
                maxPageSize   = RM_PAGE_SIZE_512M;
                break;
            case RM_ATTR_PAGE_SIZE_256GB:
                NV_ASSERT_OR_RETURN(kgmmuIsPageSize256gbSupported(pKernelGmmu),
                                  NV_ERR_NOT_SUPPORTED);
                pageSizeMask |= RM_PAGE_SIZE_256G;
                maxPageSize   = RM_PAGE_SIZE_256G;
                break;
            case RM_ATTR_PAGE_SIZE_INVALID:
                NV_PRINTF(LEVEL_ERROR, "Invalid page size attr\n");
                return NV_ERR_INVALID_ARGUMENT;
        }
    }

    // Save page sizes that will have page tables pinned (either upfront or lazily).
    *pPageSizeLockMask |= pageSizeMask;

    // Size must be aligned to maximum potential map page size.
    *pSize = RM_ALIGN_UP(*pSize, maxPageSize);

    //
    // Offset must be aligned to maximum potential map page size and
    // compression page size.
    //
    // However, the client may force alignment if it is known the VA range will
    // not be mapped to compressed physical memory.
    // The forced alignment better be aligned to the mapping page size,
    // but this isn't enforced until map time.
    //
    if (!(pAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE))
    {
        *pAlign = NV_MAX(*pAlign, NV_MAX(maxPageSize, compPageSize));
    }

    // Offset and size must be aligned to PDE stride for external management.
    if (pAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_EXTERNALLY_MANAGED)
    {
            *pAlign =      NV_MAX(*pAlign, pGVAS->extManagedAlign);
            *pSize  = RM_ALIGN_UP(*pSize,  pGVAS->extManagedAlign);
    }

    return NV_OK;
}

NV_STATUS
gvaspaceIncAllocRefCnt_IMPL
(
    OBJGVASPACE *pGVAS,
    NvU64        vAddr
)
{
    EMEMBLOCK  *pVASpaceBlock;

    pVASpaceBlock = pGVAS->pHeap->eheapGetBlock(pGVAS->pHeap, vAddr, 0);
    if (NULL == pVASpaceBlock)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pVASpaceBlock->refCount++;

    return NV_OK;
}

OBJEHEAP *
gvaspaceGetHeap_IMPL(OBJGVASPACE *pGVAS)
{
    return pGVAS->pHeap;
}

NvU64
gvaspaceGetMapPageSize_IMPL
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu,
    EMEMBLOCK   *pMemBlock
)
{
    GVAS_GPU_STATE      *pGpuState    = gvaspaceGetGpuState(pGVAS, pGpu);
    const MMU_FMT_LEVEL *pRootFmtLvl  = pGpuState->pFmt->pRoot;
    const NvU64          pageSizeMask = mmuFmtAllPageSizes(pRootFmtLvl);
    NvU32                i;

    for (i = 0; i < 64; ++i)
    {
        if (pageSizeMask & NVBIT64(i))
        {
            const MMU_FMT_LEVEL *pTargetFmt = NULL;
            MEMORY_DESCRIPTOR   *pMemDesc   = NULL;
            NvU32                memSize    = 0;

            pTargetFmt = mmuFmtFindLevelWithPageShift(pRootFmtLvl, i);
            mmuWalkGetPageLevelInfo(pGpuState->pWalk, pTargetFmt, pMemBlock->begin,
                                    (const MMU_WALK_MEMDESC**)&pMemDesc, &memSize);
            if (NULL != pMemDesc)
            {
                return NVBIT64(i);
            }
        }
    }

    NV_ASSERT(0);
    return 0;
}

NvU64
gvaspaceGetBigPageSize_IMPL(OBJGVASPACE *pGVAS)
{
    return pGVAS->bigPageSize;
}

NvBool
gvaspaceIsMirrored_IMPL(OBJGVASPACE *pGVAS)
{
    return pGVAS->bIsMirrored;
}

NvBool
gvaspaceIsFaultCapable_IMPL(OBJGVASPACE *pGVAS)
{
    return pGVAS->bIsFaultCapable;
}

NvBool
gvaspaceIsExternallyOwned_IMPL(OBJGVASPACE *pGVAS)
{
    return pGVAS->bIsExternallyOwned;
}

NvBool
gvaspaceIsAtsEnabled_IMPL(OBJGVASPACE *pGVAS)
{
    NvBool bAtsEnabled = pGVAS->bIsAtsEnabled;

    // ATS is supported with MIG memory partitioning only when VA Space has it enabled.
    if (bAtsEnabled)
    {
        OBJVASPACE *pVAS = staticCast(pGVAS, OBJVASPACE);
        OBJGPU *pGpu     = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));
        KernelMIGManager *pKernelMIGManager = (pGpu != NULL) ? GPU_GET_KERNEL_MIG_MANAGER(pGpu) : NULL;

        if ((pKernelMIGManager != NULL) && kmigmgrIsMIGMemPartitioningEnabled(pGpu, pKernelMIGManager))
        {
            return gpuIsAtsSupportedWithSmcMemPartitioning_HAL(pGpu);
        }
    }

    return bAtsEnabled;
}

NV_STATUS
gvaspaceGetPasid_IMPL(OBJGVASPACE *pGVAS, NvU32 *pPasid)
{
    NV_ASSERT_OR_RETURN(pPasid != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_PRINTF(LEVEL_INFO, "ATS enabled: %u PASID: %u\n",
              pGVAS->bIsAtsEnabled, pGVAS->processAddrSpaceId);

    NV_ASSERT_OR_RETURN(pGVAS->bIsAtsEnabled, NV_ERR_INVALID_STATE);
    if (pGVAS->processAddrSpaceId == NV_U32_MAX)
    {
        return NV_ERR_NOT_READY;
    }

    *pPasid = pGVAS->processAddrSpaceId;
    return NV_OK;
}

NvU32
gvaspaceGetFlags_IMPL(OBJGVASPACE *pGVAS)
{
    return pGVAS->flags;
}

MEMORY_DESCRIPTOR*
gvaspaceGetPageDirBase_IMPL(OBJGVASPACE *pGVAS, OBJGPU *pGpu)
{
    GVAS_GPU_STATE    *pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    MEMORY_DESCRIPTOR *pRootMem  = NULL;
    NvU32              rootSize  = 0;

    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NULL);


    if (pGVAS->bIsExternallyOwned)
    {
        return pGVAS->pExternalPDB;
    }

    mmuWalkGetPageLevelInfo(pGpuState->pWalk, pGpuState->pFmt->pRoot, 0,
                            (const MMU_WALK_MEMDESC**)&pRootMem, &rootSize);
    return pRootMem;
}

MEMORY_DESCRIPTOR*
gvaspaceGetKernelPageDirBase_IMPL(OBJGVASPACE *pGVAS, OBJGPU *pGpu)
{
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NULL);

    GVAS_GPU_STATE *pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    return (MEMORY_DESCRIPTOR*)pGpuState->pMirroredRoot;
}

const GMMU_FMT *
gvaspaceGetGmmuFmt_IMPL(OBJGVASPACE *pGVAS, OBJGPU *pGpu)
{
    GVAS_GPU_STATE *pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    NV_ASSERT_OR_RETURN(NULL != pGpuState, NULL);
    return pGpuState->pFmt;
}

GVAS_GPU_STATE *
gvaspaceGetGpuState_IMPL(OBJGVASPACE *pGVAS, OBJGPU *pGpu)
{
    OBJVASPACE *pVAS = staticCast(pGVAS, OBJVASPACE);
    NV_ASSERT_OR_RETURN(NULL != pGVAS->pGpuStates, NULL);
    NV_ASSERT_OR_RETURN(pVAS->gpuMask & NVBIT32(pGpu->gpuInstance), NULL);
    return pGVAS->pGpuStates + nvMaskPos32(pVAS->gpuMask, pGpu->gpuInstance);
}

NV_STATUS
gvaspacePinRootPageDir_IMPL(OBJGVASPACE *pGVAS, OBJGPU *pGpu)
{
    MMU_WALK_USER_CTX    userCtx = {0};
    const MMU_FMT_LEVEL *pLevelFmt;
    NV_STATUS            status;
    NvU64                rootPdeCoverage;
    NvU64                vaLo;
    NvU64                vaHi;

    if (NULL == pGVAS->pGpuStates)
    {
        // TODO: VMM must be enabled - remove once default.
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_ASSERT_OK_OR_RETURN(gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx));

    // Determine aligned range to pin.
    pLevelFmt = userCtx.pGpuState->pFmt->pRoot;
    rootPdeCoverage = mmuFmtLevelPageSize(pLevelFmt);
    vaLo = NV_ALIGN_DOWN64(gvaspaceGetVaStart(pGVAS),   rootPdeCoverage);
    vaHi = NV_ALIGN_UP64(gvaspaceGetVaLimit(pGVAS) + 1, rootPdeCoverage) - 1;

    // Alloc and bind root level instance.
    status = mmuWalkReserveEntries(userCtx.pGpuState->pWalk,
                                   pLevelFmt, vaLo, vaHi, NV_TRUE);
    NV_ASSERT(NV_OK == status);

    gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
    return status;
}

void
gvaspaceUnpinRootPageDir_IMPL(OBJGVASPACE *pGVAS, OBJGPU *pGpu)
{
    MMU_WALK_USER_CTX    userCtx = {0};
    const MMU_FMT_LEVEL *pLevelFmt;
    NV_STATUS            status;
    NvU64                rootPdeCoverage;
    NvU64                vaLo;
    NvU64                vaHi;

    if (NULL == pGVAS->pGpuStates)
    {
        // TODO: VMM must be enabled - remove once default.
        return;
    }

    if (gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx) != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING,
            "Failed to acquire walk user context\n");
        return;
    }

    // Determine aligned range to unpin.
    pLevelFmt = userCtx.pGpuState->pFmt->pRoot;
    rootPdeCoverage = mmuFmtLevelPageSize(pLevelFmt);
    vaLo = NV_ALIGN_DOWN64(gvaspaceGetVaStart(pGVAS),   rootPdeCoverage);
    vaHi = NV_ALIGN_UP64(gvaspaceGetVaLimit(pGVAS) + 1, rootPdeCoverage) - 1;

    // Unreserve root level instance (won't free it if there are still mappings).
    status = mmuWalkReleaseEntries(userCtx.pGpuState->pWalk,
                                   pLevelFmt, vaLo, vaHi);
    NV_ASSERT_OK(status);
    gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
}

NV_STATUS
gvaspaceMap_IMPL
(
    OBJGVASPACE          *pGVAS,
    OBJGPU               *pGpu,
    const NvU64           vaLo,
    const NvU64           vaHi,
    const MMU_MAP_TARGET *pTarget,
    const VAS_MAP_FLAGS   flags
)
{
    NV_STATUS         status    = NV_OK;
    EMEMBLOCK        *pMemBlock = NULL;
    GVAS_BLOCK       *pVASBlock = NULL;
    NvU64             pageSize  = mmuFmtLevelPageSize(pTarget->pLevelFmt);
    MMU_WALK_USER_CTX userCtx   = {0};

    // Enforce unicast.
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    // Check VA alignment.
    NV_ASSERT_OR_RETURN(0 == (vaLo       & (pageSize - 1)), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(0 == ((vaHi + 1) & (pageSize - 1)), NV_ERR_INVALID_ARGUMENT);

    //
    // Register the mapping unless remapping an existing mapping.
    // Remapping an existing mapping is used in two cases:
    // 1. [MODS-only]    Release/reacquire compression for verif.
    // 2. [Windows-only] BAR1 force clobber for BSOD during bugcheck.
    //
    if (!flags.bRemap)
    {
        // Get VA block.
        pMemBlock = pGVAS->pHeap->eheapGetBlock(pGVAS->pHeap, vaLo, 0);
        NV_ASSERT_OR_RETURN(NULL != pMemBlock, NV_ERR_INVALID_ARGUMENT);
        pVASBlock = (GVAS_BLOCK*)pMemBlock->pData;

        // Check VA containment.
        NV_ASSERT_OR_RETURN(vaHi <= pMemBlock->end, NV_ERR_INVALID_ARGUMENT);

        // Insert range into VAS block mapping tree.
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, _gvaspaceMappingInsert(pGVAS,
                              pGpu, pVASBlock, vaLo, vaHi, flags));
    }

    // Call MMU walker to map.
    NV_ASSERT_OK_OR_GOTO(status,
        gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, pVASBlock, &userCtx),
        catch);

    status = mmuWalkMap(userCtx.pGpuState->pWalk, vaLo, vaHi, pTarget);
    gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
    NV_ASSERT_OR_GOTO(NV_OK == status, catch);

catch:
    if (NV_OK != status && (!flags.bRemap))
    {
        _gvaspaceMappingRemove(pGVAS, pGpu, pVASBlock, vaLo, vaHi);
    }
    return status;
}

void
gvaspaceUnmap_IMPL
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu,
    const NvU64  vaLo,
    const NvU64  vaHi
)
{
    NV_STATUS         status    = NV_OK;
    EMEMBLOCK        *pMemBlock = NULL;
    GVAS_BLOCK       *pVASBlock = NULL;
    MMU_WALK_USER_CTX userCtx   = {0};

    // Enforce unicast.
    NV_ASSERT_OR_RETURN_VOID(!gpumgrGetBcEnabledStatus(pGpu));

    // Get VA block.
    pMemBlock = pGVAS->pHeap->eheapGetBlock(pGVAS->pHeap, vaLo, 0);
    NV_ASSERT_OR_RETURN_VOID(NULL != pMemBlock);
    pVASBlock = (GVAS_BLOCK*)pMemBlock->pData;

    // Unregister the mapping
    status = _gvaspaceMappingRemove(pGVAS, pGpu, pVASBlock, vaLo, vaHi);
    NV_ASSERT_OR_RETURN_VOID(NV_OK == status);

    NV_ASSERT_OR_RETURN_VOID(gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, pVASBlock, &userCtx) == NV_OK);

    if (pVASBlock->flags.bSparse || (pGVAS->flags & VASPACE_FLAGS_BAR_BAR1)
        ||((pMemBlock->refCount >1) && (pGVAS->flags & VASPACE_FLAGS_FLA))
       )
    {
        // Return back to Sparse if that was the original state of this allocation.
        NV_ASSERT_OK(mmuWalkSparsify(userCtx.pGpuState->pWalk, vaLo, vaHi, NV_FALSE));
    }
    else
    {
        // Plain old unmap
        NV_ASSERT_OK(mmuWalkUnmap(userCtx.pGpuState->pWalk, vaLo, vaHi));
    }
    gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
}

void
gvaspaceInvalidateTlb_IMPL
(
    OBJGVASPACE         *pGVAS,
    OBJGPU              *pGpu,
    VAS_PTE_UPDATE_TYPE  update_type
)
{
    OBJVASPACE *pVAS = staticCast(pGVAS, OBJVASPACE);
    NvU32      gfid  = GPU_GFID_PF;

    NV_ASSERT_OR_RETURN_VOID(!gpumgrGetBcEnabledStatus(pGpu));
    NV_ASSERT_OR_RETURN_VOID(0 != (NVBIT(pGpu->gpuInstance) & pVAS->gpuMask));

    GVAS_GPU_STATE    *pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    MEMORY_DESCRIPTOR *pRootMem  = NULL;
    NvU32              rootSize  = 0;
    NvU32              invalidation_scope = NV_GMMU_INVAL_SCOPE_ALL_TLBS;
    NvBool             bCallingContextPlugin;

    NV_ASSERT_OR_RETURN_VOID(vgpuIsCallingContextPlugin(pGpu, &bCallingContextPlugin) == NV_OK);
    if (!bCallingContextPlugin)
    {
        NV_ASSERT_OR_RETURN_VOID(vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK);
    }

    if (pGVAS->flags & VASPACE_FLAGS_INVALIDATE_SCOPE_NVLINK_TLB)
    {
        invalidation_scope = NV_GMMU_INVAL_SCOPE_LINK_TLBS;
    }
    else
    {
        invalidation_scope = NV_GMMU_INVAL_SCOPE_NON_LINK_TLBS;
    }

    mmuWalkGetPageLevelInfo(pGpuState->pWalk, pGpuState->pFmt->pRoot, 0,
                            (const MMU_WALK_MEMDESC**)&pRootMem, &rootSize);
    if (pRootMem != NULL)
    {
        KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
        kgmmuInvalidateTlb_HAL(pGpu, pKernelGmmu, pRootMem,
                              pGVAS->flags,
                              update_type, gfid,
                              invalidation_scope);

        if (pGVAS->bIsMirrored)
        {
            kgmmuInvalidateTlb_HAL(pGpu, pKernelGmmu,
                                  (MEMORY_DESCRIPTOR*)pGpuState->pMirroredRoot,
                                  pGVAS->flags,
                                  update_type, gfid,
                                  invalidation_scope);
        }
    }
}

NV_STATUS
gvaspaceGetVasInfo_IMPL
(
    OBJGVASPACE                                   *pGVAS,
    NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS  *pParams
)
{
    OBJVASPACE          *pVAS  = staticCast(pGVAS, OBJVASPACE);
    OBJGPU              *pGpu  = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));
    KernelGmmu          *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    const MMU_FMT_LEVEL *pBigPageTable   = NULL;
    const MMU_FMT_LEVEL *pSmallPageTable = NULL;
    const GMMU_FMT      *pFmt            = gvaspaceGetGpuState(pGVAS, pGpu)->pFmt;

    NV_ASSERT_OR_RETURN(NULL != pParams, NV_ERR_INVALID_PARAM_STRUCT);

    // Retrive the number of VA bits for this format.
    pParams->vaBitCount = pFmt->pRoot->virtAddrBitHi + 1;

    // Check if the page sizes are supported
    pSmallPageTable = mmuFmtFindLevelWithPageShift(pFmt->pRoot, RM_PAGE_SHIFT);
    NV_ASSERT_OR_RETURN(pSmallPageTable, NV_ERR_INVALID_EVENT);

    pBigPageTable = mmuFmtFindLevelWithPageShift(pFmt->pRoot, nvLogBase2(pGVAS->bigPageSize));
    NV_ASSERT_OR_RETURN(pBigPageTable, NV_ERR_INVALID_EVENT);
    pParams->bigPageSize = pGVAS->bigPageSize;

    pParams->supportedPageSizeMask = RM_PAGE_SIZE | pParams->bigPageSize;

    if (kgmmuIsHugePageSupported(pKernelGmmu))
        pParams->supportedPageSizeMask |= RM_PAGE_SIZE_HUGE;

    if (kgmmuIsPageSize512mbSupported(pKernelGmmu))
        pParams->supportedPageSizeMask |= RM_PAGE_SIZE_512M;

    if (kgmmuIsPageSize256gbSupported(pKernelGmmu))
        pParams->supportedPageSizeMask |= RM_PAGE_SIZE_256G;

    // Dual Page Table is supported for all Fermi-and-later chips
    pParams->dualPageTableSupported = (NvU32)NV_TRUE;

    // Big Page Table caps

    // VA bits covered by a PDE (for Big Page Table), in a terminal Page Directory.
    pParams->pdeCoverageBitCount = pBigPageTable->virtAddrBitHi + 1;
    // Physical size of Page Table in bytes
    pParams->pageTableBigFormat.pageTableSize = mmuFmtLevelSize(pBigPageTable);
    // VA extent of a Big Page Table
    pParams->pageTableBigFormat.pageTableCoverage =
        (NvU32)mmuFmtLevelVirtAddrMask(pBigPageTable) + 1;

    // Small Page Table caps, similar to Big Page Table caps
    //<! TODO: num4KPageTableFormats should change after partial PT support is added.
    pParams->num4KPageTableFormats = 1;
    pParams->pageTable4KFormat[0].pageTableSize = mmuFmtLevelSize(pSmallPageTable);
    pParams->pageTable4KFormat[0].pageTableCoverage =
        (NvU32)mmuFmtLevelVirtAddrMask(pSmallPageTable) + 1;

    pParams->idealVRAMPageSize = pParams->bigPageSize;

    pParams->vaRangeLo = vaspaceGetVaStart(pVAS);

    return NV_OK;
}

NV_STATUS
gvaspaceGetPageTableInfo_IMPL
(
    OBJGVASPACE                           *pGVAS,
    NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS   *pParams
)
{
    OBJVASPACE              *pVAS = staticCast(pGVAS, OBJVASPACE);
    OBJGPU                  *pGpu = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));
    GVAS_GPU_STATE          *pGpuState;
    MMU_WALK                *pWalk;
    const MMU_FMT_LEVEL     *pRootFmt;
    PMEMORY_DESCRIPTOR       pRootMem = NULL;
    NvU32                    rootSize = 0;
    NvU32                    pteBlockIdx = 0;
    NvU32                    i;
    NvBool                   bOrigBcState;
    NV_STATUS                rmStatus;

    NV_ASSERT_OR_RETURN(NULL != pParams, NV_ERR_INVALID_PARAM_STRUCT);

    // Pick a specific sub-device if requested.
    if (0 != pParams->subDeviceId)
    {
        pGpu = gpumgrGetGpuFromSubDeviceInst(gpuGetDeviceInstance(pGpu),
                                             pParams->subDeviceId - 1);
        NV_ASSERT_OR_RETURN(NULL != pGpu, NV_ERR_INVALID_ARGUMENT);
    }

    // Page tables are pinned and queried in UC. Force GPU to unicast.
    bOrigBcState = gpumgrGetBcEnabledStatus(pGpu);
    gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);

    pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    pWalk     = pGpuState->pWalk;
    pRootFmt  = pGpuState->pFmt->pRoot;

    // Pin lazy page tables for WDDMv1 KMD.
    rmStatus = _gvaspacePinLazyPageTables(pGVAS, pGpu, pParams->gpuAddr);

    gpumgrSetBcEnabledStatus(pGpu, bOrigBcState);

    NV_ASSERT_OR_RETURN((NV_OK == rmStatus), rmStatus);

    for (i = 0; i < pageSizeCount; i++)
    {
        PMEMORY_DESCRIPTOR                  pMemDesc  = NULL;
        NvU32                               memSize   = 0;
        NV0080_CTRL_DMA_PDE_INFO_PTE_BLOCK *pPteBlock = NULL;
        NvU64                               pageSize;
        const MMU_FMT_LEVEL                *pLevelFmt;
        const MMU_FMT_LEVEL                *pParentFmt;
        NvU32                               subLevel;


        pageSize = (VAS_PAGESIZE_IDX_BIG == i) ? pGVAS->bigPageSize : pageSizes[i];
        pLevelFmt = mmuFmtFindLevelWithPageShift(pRootFmt, BIT_IDX_64(pageSize));
        if (NULL == pLevelFmt)
        {
            continue;
        }

        pParentFmt = mmuFmtFindLevelParent(pRootFmt, pLevelFmt, &subLevel);
        NV_ASSERT_OR_RETURN(NULL != pParentFmt, NV_ERR_INVALID_ARGUMENT);

        NV_ASSERT_OK_OR_RETURN(
            mmuWalkGetPageLevelInfo(pWalk, pLevelFmt, pParams->gpuAddr,
                                    (const MMU_WALK_MEMDESC**)&pMemDesc, &memSize));
        if (NULL == pMemDesc)
            continue;

        // These only need to be calculated once, but we need the parent level format.
        if (0 == pteBlockIdx)
        {
            // The base VA of the PDE
            pParams->pdeVirtAddr = mmuFmtLevelVirtAddrLo(pLevelFmt, pParams->gpuAddr);

            // Number of bytes occupied by one PDE
            pParams->pdeEntrySize = pParentFmt->entrySize;
        }

        NV_ASSERT_OR_RETURN(pteBlockIdx < NV0080_CTRL_DMA_PDE_INFO_PTE_BLOCKS, NV_ERR_INVALID_STATE);
        pPteBlock = &pParams->pteBlocks[pteBlockIdx++];

        // Page size supported by this page table
        pPteBlock->pageSize       = pageSize;

        // Phys addr of the Page Table
        pPteBlock->ptePhysAddr    = memdescGetPhysAddr(pMemDesc, VAS_ADDRESS_TRANSLATION(pVAS), 0);

        // Number of bytes occupied by one PTE
        pPteBlock->pteEntrySize   = pLevelFmt->entrySize;

        // VA extent of one PDE, i.e. of one entire Page Table.
        pPteBlock->pdeVASpaceSize = (NvU32)mmuFmtLevelVirtAddrMask(pLevelFmt) + 1;

        // Caching attributes
        pPteBlock->pteCacheAttrib = memdescGetCpuCacheAttrib(pMemDesc);

        // Addr space of the Page Table
        switch (memdescGetAddressSpace(pMemDesc))
        {
            case ADDR_FBMEM:
                pPteBlock->pteAddrSpace =
                    NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PTE_ADDR_SPACE_VIDEO_MEMORY;
                break;
            case ADDR_SYSMEM:
                if (memdescGetCpuCacheAttrib(pMemDesc) == NV_MEMORY_CACHED)
                {
                    pPteBlock->pteAddrSpace =
                        NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PTE_ADDR_SPACE_SYSTEM_COHERENT_MEMORY;
                }
                else
                {
                    pPteBlock->pteAddrSpace =
                        NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PTE_ADDR_SPACE_SYSTEM_NON_COHERENT_MEMORY;
                }
                break;
            default:
                NV_ASSERT(0);
                return NV_ERR_INVALID_STATE;
        }
    }

    // Addr of the root Page Dir
    NV_ASSERT_OK_OR_RETURN(
        mmuWalkGetPageLevelInfo(pWalk, pRootFmt, 0,
                                (const MMU_WALK_MEMDESC**)&pRootMem, &rootSize));
    if (NULL == pRootMem)
    {
        return NV_ERR_INVALID_STATE;
    }
    pParams->pdbAddr = memdescGetPhysAddr(pRootMem, VAS_ADDRESS_TRANSLATION(pVAS), 0);

    // Addr Space of the Page Dir.
    switch (memdescGetAddressSpace(pRootMem))
    {
        case ADDR_FBMEM:
            pParams->pdeAddrSpace =
                NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PDE_ADDR_SPACE_VIDEO_MEMORY;
            break;
        case ADDR_SYSMEM:
            if (memdescGetCpuCacheAttrib(pRootMem) == NV_MEMORY_CACHED)
            {
                pParams->pdeAddrSpace =
                    NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PDE_ADDR_SPACE_SYSTEM_COHERENT_MEMORY;
            }
            else
            {
                pParams->pdeAddrSpace =
                     NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS_PDE_ADDR_SPACE_SYSTEM_NON_COHERENT_MEMORY;
            }
            break;
        default:
            NV_ASSERT(0);
            return NV_ERR_INVALID_STATE;
    }

    return NV_OK;
}

NV_STATUS
gvaspaceGetPteInfo_IMPL
(
    OBJGVASPACE        *pGVAS,
    OBJGPU             *pGpu,
    NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams,
    RmPhysAddr         *pPhysAddr
)
{
    KernelGmmu     *pKernelGmmu;
    MemoryManager  *pMemoryManager;
    GVAS_GPU_STATE *pGpuState;
    MMU_WALK       *pWalk;
    const GMMU_FMT *pFmt;
    NV_STATUS      status = NV_OK;
    NvU32          i;
    NvU32          pteBlockIndex = 0;
    const MMU_FMT_LEVEL *pRootFmt;
    TRANSFER_SURFACE surf = {0};

    NV_ASSERT_OR_RETURN(NULL != pParams, NV_ERR_INVALID_PARAM_STRUCT);

    // Pick a specific sub-device if requested.
    if (0 != pParams->subDeviceId)
    {
        pGpu = gpumgrGetGpuFromSubDeviceInst(gpuGetDeviceInstance(pGpu),
                                             pParams->subDeviceId - 1);
        NV_ASSERT_OR_RETURN(NULL != pGpu, NV_ERR_INVALID_ARGUMENT);
    }
    pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    pWalk     = pGpuState->pWalk;
    pFmt      = pGpuState->pFmt;
    pRootFmt  = pFmt->pRoot;
    pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    //
    // We will try all available page sizes for valid allocation a the give VA.
    // Will flag error if mutiple valid allocations exist.
    //

    for (i = 0; i < pageSizeCount; i++)
    {
        NV0080_CTRL_DMA_PTE_INFO_PTE_BLOCK *pPteBlock = NULL;
        const GMMU_FIELD_ADDRESS           *pAddrField;
        const MMU_FMT_LEVEL                *pLevelFmt     = NULL;
        PMEMORY_DESCRIPTOR                  pMemDesc      = NULL;
        NvU32                               memSize       = 0;
        NvU32                               pteIndex;
        GMMU_ENTRY_VALUE                    pte       = {{0}};
        NvU64                               pageSize;

        pageSize = (VAS_PAGESIZE_IDX_BIG == i) ? pGVAS->bigPageSize : pageSizes[i];
        pLevelFmt = mmuFmtFindLevelWithPageShift(pRootFmt, BIT_IDX_64(pageSize));
        if (NULL == pLevelFmt)
        {
            continue;
        }

        NV_ASSERT_OK_OR_RETURN(
            mmuWalkGetPageLevelInfo(pWalk, pLevelFmt, pParams->gpuAddr,
                                    (const MMU_WALK_MEMDESC**)&pMemDesc, &memSize));
        if (NULL == pMemDesc)
        {
            // Skip if not allocated.
            continue;
        }

        pteIndex = mmuFmtVirtAddrToEntryIndex(pLevelFmt, pParams->gpuAddr);

        // Read the PTE
        surf.pMemDesc = pMemDesc;
        surf.offset = pteIndex * pLevelFmt->entrySize;

        NV_ASSERT_OK_OR_RETURN(memmgrMemRead(pMemoryManager, &surf, pte.v8,
                                             pLevelFmt->entrySize,
                                             TRANSFER_FLAGS_DEFER_FLUSH));

        NV_ASSERT_OR_RETURN(pteBlockIndex < NV0080_CTRL_DMA_PDE_INFO_PTE_BLOCKS, NV_ERR_INVALID_STATE);
        pPteBlock = &pParams->pteBlocks[pteBlockIndex++];

        // Page size supported by this page table
        pPteBlock->pageSize = pageSize;

        // Number of bytes occupied by one PTE
        pPteBlock->pteEntrySize = pLevelFmt->entrySize;

        kgmmuExtractPteInfo(pKernelGmmu, &pte, pPteBlock, pFmt, pLevelFmt);

        //
        // Get phys addr encoded in the PTE, but only
        // if requested and the PTE is valid. Only one page size
        // PTE should be valid at a time.
        //
        if ((NULL != pPhysAddr) &&
            nvFieldGetBool(&pFmt->pPte->fldValid, pte.v8))
        {
            pAddrField = gmmuFmtPtePhysAddrFld(pFmt->pPte,
                gmmuFieldGetAperture(&pFmt->pPte->fldAperture, pte.v8));
            *pPhysAddr = (RmPhysAddr)gmmuFieldGetAddress(pAddrField, pte.v8);
        }
    }

    //
    // We don't support pre-initializing paga tables on VMM.
    // Page tables are allocated on-demand during mapping.
    // So we assert if this ctrl call is called with request to init page tables (skipVASpaceInit ==  NV_FALSE),
    // and the page tables are not already allocated.
    //
    NV_CHECK_OR_RETURN(LEVEL_ERROR, (pteBlockIndex > 0) || pParams->skipVASpaceInit, NV_ERR_INVALID_REQUEST);

    return status;
}

static inline
NvBool isPteDowngrade(KernelGmmu *pKernelGmmu, const GMMU_FMT *pFmt, NvU32 pteInputFlags, GMMU_ENTRY_VALUE curPte)
{
    NvBool bReadOnly      = NV_FALSE;
    NvBool curPteReadOnly = NV_FALSE;

    NvBool bPteInvalid = (!FLD_TEST_DRF(0080, _CTRL_DMA_PTE_INFO_PARAMS_FLAGS,
                            _VALID, _TRUE, pteInputFlags)
                            && nvFieldGetBool(&pFmt->pPte->fldValid, curPte.v8));

    if (pFmt->version == GMMU_FMT_VERSION_3)
    {
        NvU32 ptePcfHw = 0;
        NvU32 ptePcfSw = 0;

        ptePcfHw = nvFieldGet32(&pFmt->pPte->fldPtePcf, curPte.v8);
        NV_ASSERT_OR_RETURN((kgmmuTranslatePtePcfFromHw_HAL(pKernelGmmu, ptePcfHw, nvFieldGetBool(&pFmt->pPte->fldValid, curPte.v8),
                                                           &ptePcfSw) == NV_OK), NV_ERR_INVALID_ARGUMENT);
        curPteReadOnly = ptePcfSw & (1 << SW_MMU_PCF_RO_IDX);
    }
    else
    {
        curPteReadOnly = nvFieldGetBool(&pFmt->pPte->fldReadOnly, curPte.v8);
    }
    bReadOnly = (!FLD_TEST_DRF(0080, _CTRL_DMA_PTE_INFO_PARAMS_FLAGS,
                     _READ_ONLY, _TRUE, pteInputFlags)
                     && !curPteReadOnly);

    return (bPteInvalid || bReadOnly);
}

NV_STATUS
gvaspaceSetPteInfo_IMPL
(
    OBJGVASPACE                         *pGVAS,
    OBJGPU                              *pGpu,
    NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams
)
{
    OBJVASPACE              *pVAS = staticCast(pGVAS, OBJVASPACE);
    MemoryManager           *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU8                     i;
    NV_STATUS                status = NV_OK;
    NvBool                   bDowngrade = NV_FALSE;
    TRANSFER_SURFACE         surf = {0};

    NV_ASSERT_OR_RETURN(NULL != pParams, NV_ERR_INVALID_PARAM_STRUCT);

    // Loop over each GPU associated with the VAS.
    FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
    {
        GVAS_GPU_STATE      *pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
        MMU_WALK            *pWalk     = pGpuState->pWalk;
        const GMMU_FMT      *pFmt      = pGpuState->pFmt;
        const MMU_FMT_LEVEL *pRootFmt  = pFmt->pRoot;
        bDowngrade = NV_FALSE;

        // Skip the GPU if the caller requested a different specific sub-device.
        if ((0 != pParams->subDeviceId) &&
            (pGpu->subdeviceInstance != (pParams->subDeviceId - 1)))
        {
            goto catchGpu;
        }

        for (i = 0; i < NV0080_CTRL_DMA_SET_PTE_INFO_PTE_BLOCKS; i++)
        {
            NV0080_CTRL_DMA_PTE_INFO_PTE_BLOCK *pPteBlock = NULL;
            const MMU_FMT_LEVEL                *pLevelFmt;
            PMEMORY_DESCRIPTOR                  pMemDesc  = NULL;
            NvU32                               memSize   = 0;
            GMMU_ENTRY_VALUE                    pte       = {{0}};
            NvBool                              bValid;
            NvBool                              bEncrypted;
            NvBool                              bReadOnly;
            NvBool                              bVolatile = NV_FALSE;
            NvU32                               aperture;
            NvU32                               pteIndex;
            NvU64                               surfOffset;

            //
            // Ignore the index if a page size of 0 is specified. This is a valid
            // check as the caller may send down 0 page size for indxes
            // which are not of interest.
            //
            if (0 == pParams->pteBlocks[i].pageSize)
                continue;

            //
            // Continue the loop if we see an unsupported page size.
            // Ideally we should assert, but we're emulating the behavior of the old
            // API @ref dmaSetPteInfo_GF100 here.
            //
            if (!(pGVAS->bigPageSize == pParams->pteBlocks[i].pageSize ||
                  RM_PAGE_SIZE_HUGE == pParams->pteBlocks[i].pageSize ||
                  RM_PAGE_SIZE_512M == pParams->pteBlocks[i].pageSize ||
                  RM_PAGE_SIZE_256G == pParams->pteBlocks[i].pageSize ||
                  RM_PAGE_SIZE == pParams->pteBlocks[i].pageSize))
            {
                continue;
            }

            // Query the Page Tables.
            pLevelFmt = mmuFmtFindLevelWithPageShift(pRootFmt,
                                                     BIT_IDX_64(pParams->pteBlocks[i].pageSize));
            if (NULL == pLevelFmt)
            {
                status = NV_ERR_INVALID_STATE;
                NV_ASSERT_OR_GOTO(0, catchGpu);
            }
            status = mmuWalkGetPageLevelInfo(pWalk, pLevelFmt, pParams->gpuAddr,
                                             (const MMU_WALK_MEMDESC**)&pMemDesc, &memSize);
            NV_ASSERT_OR_GOTO(NV_OK == status, catchGpu);

            // Can't set PTE for an unallocated VA.
            if (NULL == pMemDesc)
            {
                status = NV_ERR_INVALID_ADDRESS;
                NV_ASSERT_OR_GOTO(0, catchGpu);
            }

            pPteBlock  = &pParams->pteBlocks[i];

            // Read the PTE
            pteIndex = mmuFmtVirtAddrToEntryIndex(pLevelFmt, pParams->gpuAddr);

            surf.pMemDesc = pMemDesc;
            surf.offset = pteIndex * pLevelFmt->entrySize;

            NV_ASSERT_OK_OR_GOTO(status,
                memmgrMemRead(pMemoryManager, &surf, pte.v8,
                              pLevelFmt->entrySize,
                              TRANSFER_FLAGS_NONE),
                catchGpu);

            //
            // If any of these entries are being downgraded, we need to perform
            // a full flush.
            //
            bDowngrade = isPteDowngrade(GPU_GET_KERNEL_GMMU(pGpu), pFmt, pPteBlock->pteFlags, pte);


            // Insert the PTE fields from pParams

            // Valid
            bValid = FLD_TEST_DRF(0080, _CTRL_DMA_PTE_INFO_PARAMS_FLAGS,
                                       _VALID, _TRUE, pPteBlock->pteFlags);

            // Aperture
            aperture = DRF_VAL(0080_CTRL, _DMA_PTE_INFO,
                                         _PARAMS_FLAGS_APERTURE, pPteBlock->pteFlags);

            // Encryption
            bEncrypted = FLD_TEST_DRF(0080, _CTRL_DMA_PTE_INFO_PARAMS_FLAGS,
                                           _ENCRYPTED, _TRUE, pPteBlock->pteFlags);

            // Read Only
            bReadOnly = FLD_TEST_DRF(0080, _CTRL_DMA_PTE_INFO_PARAMS_FLAGS,
                                           _READ_ONLY, _TRUE, pPteBlock->pteFlags);

            if (!FLD_TEST_DRF(0080, _CTRL_DMA_PTE_INFO_PARAMS_FLAGS,
                    _GPU_CACHED, _NOT_SUPPORTED, pPteBlock->pteFlags))
            {
                bVolatile = FLD_TEST_DRF(0080, _CTRL_DMA_PTE_INFO_PARAMS_FLAGS,
                    _GPU_CACHED, _FALSE, pPteBlock->pteFlags);
            }

            if (pFmt->version == GMMU_FMT_VERSION_3)
            {
                NvU32 ptePcfHw  = 0;
                NvU32 ptePcfSw  = 0;

                if (bValid)
                {
                    nvFieldSetBool(&pFmt->pPte->fldValid, NV_TRUE, pte.v8);
                    nvFieldSet32(&pFmt->pPte->fldAperture._enum.desc, aperture, pte.v8);
                    nvFieldSet32(&pFmt->pPte->fldKind, pPteBlock->kind, pte.v8);
                    ptePcfSw |= bVolatile ? (1 << SW_MMU_PCF_UNCACHED_IDX) : 0;
                    if (bReadOnly)
                    {
                        ptePcfSw |= 1 << SW_MMU_PCF_RO_IDX;
                        ptePcfSw |= 1 << SW_MMU_PCF_NOATOMIC_IDX;
                    }
                    ptePcfSw |= (1 << SW_MMU_PCF_REGULAR_IDX);
                    ptePcfSw |= (1 << SW_MMU_PCF_ACE_IDX);
                }
                else
                {
                    ptePcfSw |= (1 << SW_MMU_PCF_INVALID_IDX);
                }
                NV_ASSERT_OR_RETURN((kgmmuTranslatePtePcfFromSw_HAL(GPU_GET_KERNEL_GMMU(pGpu), ptePcfSw, &ptePcfHw) == NV_OK),
                                     NV_ERR_INVALID_ARGUMENT);
                nvFieldSet32(&pFmt->pPte->fldPtePcf, ptePcfHw, pte.v8);
            }
            else
            {
                nvFieldSetBool(&pFmt->pPte->fldValid, bValid, pte.v8);
                if (bValid)
                {
                    nvFieldSet32(&pFmt->pPte->fldAperture._enum.desc, aperture, pte.v8);
                    nvFieldSetBool(&pFmt->pPte->fldEncrypted, bEncrypted, pte.v8);
                    nvFieldSetBool(&pFmt->pPte->fldReadOnly, bReadOnly, pte.v8);
                    nvFieldSetBool(&pFmt->pPte->fldVolatile, bVolatile, pte.v8);

                    //
                    // gmmuFmtInitPteCompTags assumes that lower/upper half of CTL is
                    // determined by the surfOffset passed in.
                    // We calculate the surface offset here based on the pteIndex to
                    // match the pre-GM20X behavior of selecting half based on VA alignment.
                    //
                    // compPageIndexLo/Hi are also set to always allow compression on
                    // the page index we are overriding. The 2MB page require 0x20 comptags to be
                    // contiguous, so set the endPage limit proprerly as well.
                    //
                    if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, pPteBlock->kind))
                    {
                        COMPR_INFO                comprInfo;
                        const GMMU_FIELD_ADDRESS *pAddrField = gmmuFmtPtePhysAddrFld(pFmt->pPte,
                                                                                     gmmuFieldGetAperture(&pFmt->pPte->fldAperture, pte.v8));
                        RmPhysAddr                physAddr = (RmPhysAddr)gmmuFieldGetAddress(pAddrField, pte.v8);

                        surfOffset = pteIndex * pPteBlock->pageSize;
                        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, memmgrFillComprInfo(pGpu, pMemoryManager, pPteBlock->pageSize, 1, pPteBlock->kind,
                                                                               surfOffset, pPteBlock->comptagLine, &comprInfo));
                        kgmmuFieldSetKindCompTags(GPU_GET_KERNEL_GMMU(pGpu), pFmt, pLevelFmt, &comprInfo, physAddr, surfOffset, pteIndex, pte.v8);
                    }
                    else
                    {
                        nvFieldSet32(&pFmt->pPte->fldKind, pPteBlock->kind, pte.v8);
                    }
                }
            }

            // Copy back the overwritten values to the actual PTE memory
            NV_ASSERT_OK_OR_GOTO(status,
                memmgrMemWrite(pMemoryManager, &surf, pte.v8,
                               pLevelFmt->entrySize,
                               TRANSFER_FLAGS_NONE),
                catchGpu);

        }

        // Invalidate TLB
        gvaspaceInvalidateTlb(pGVAS, pGpu, bDowngrade ? PTE_DOWNGRADE : PTE_UPGRADE);

catchGpu:
        if (NV_OK != status)
        {
            break;
        }
    }
    FOR_EACH_GPU_IN_MASK_UC_END

    return status;
}

static void
_gmmuWalkCBFillEntries_SkipExternal
(
    MMU_WALK_USER_CTX         *pUserCtx,
    const MMU_FMT_LEVEL       *pLevelFmt,
    const MMU_WALK_MEMDESC    *pLevelMem,
    const NvU32                entryIndexLo,
    const NvU32                entryIndexHi,
    const MMU_WALK_FILL_STATE  fillState,
    NvU32                     *pProgress
)
{
    OBJGVASPACE *pGVAS = pUserCtx->pGVAS;
    OBJVASPACE  *pVAS  = staticCast(pGVAS, OBJVASPACE);

    // Clamp index range to RM-internal entries.
    const NvU32 entryIndexLoClamp = NV_MAX(entryIndexLo,
                    mmuFmtVirtAddrToEntryIndex(pLevelFmt, pVAS->vasStart));
    const NvU32 entryIndexHiClamp = NV_MIN(entryIndexHi,
                    mmuFmtVirtAddrToEntryIndex(pLevelFmt, pGVAS->vaLimitInternal));

    // Clamp may negate range.
    if (entryIndexHiClamp >= entryIndexLoClamp)
    {
        // Reuse normal fill callback.
        g_gmmuWalkCallbacks.FillEntries(pUserCtx, pLevelFmt, pLevelMem,
                                        entryIndexLoClamp, entryIndexHiClamp,
                                        fillState, pProgress);
        NV_ASSERT_OR_RETURN_VOID(*pProgress == (entryIndexHiClamp - entryIndexLoClamp + 1));
    }

    // Report full range complete on success.
    *pProgress = entryIndexHi - entryIndexLo + 1;
}

static void
_gmmuWalkCBCopyEntries_SkipExternal
(
    MMU_WALK_USER_CTX         *pUserCtx,
    const MMU_FMT_LEVEL       *pLevelFmt,
    const MMU_WALK_MEMDESC    *pSrcMem,
    const MMU_WALK_MEMDESC    *pDstMem,
    const NvU32                entryIndexLo,
    const NvU32                entryIndexHi,
    NvU32                     *pProgress
)
{
    OBJGVASPACE *pGVAS = pUserCtx->pGVAS;
    OBJVASPACE  *pVAS  = staticCast(pGVAS, OBJVASPACE);

    // Clamp index range to RM-internal entries.
    const NvU32 entryIndexLoClamp = NV_MAX(entryIndexLo,
                    mmuFmtVirtAddrToEntryIndex(pLevelFmt, pVAS->vasStart));
    const NvU32 entryIndexHiClamp = NV_MIN(entryIndexHi,
                    mmuFmtVirtAddrToEntryIndex(pLevelFmt, pGVAS->vaLimitInternal));

    // Clamp may negate range.
    if (entryIndexHiClamp >= entryIndexLoClamp)
    {
        // Reuse normal copy callback.
        g_gmmuWalkCallbacks.CopyEntries(pUserCtx, pLevelFmt, pSrcMem, pDstMem,
                                        entryIndexLoClamp, entryIndexHiClamp,
                                        pProgress);
        NV_ASSERT_OR_RETURN_VOID(*pProgress == (entryIndexHiClamp - entryIndexLoClamp + 1));
    }

    // Report full range complete.
    *pProgress = entryIndexHi - entryIndexLo + 1;
}

//
// TODO: hClient is added temporarily and will be removed after RS_PRIV_LEVEL is used
// in osCreateMemFromOsDescriptor. See bug #2107861.
//
NV_STATUS
gvaspaceExternalRootDirCommit_IMPL
(
    OBJGVASPACE                               *pGVAS,
    NvHandle                                   hClient,
    OBJGPU                                    *pGpu,
    NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS *pParams
)
{
    OBJVASPACE               *pVAS               = staticCast(pGVAS, OBJVASPACE);
    MEMORY_DESCRIPTOR        *pRootMemNew        = NULL;
    NvU64                     rootSizeNew;
    NV_STATUS                 status;
    NV_ADDRESS_SPACE          aperture;
    NvU32                     cpuCacheAttr;
    NvU64                     vaLimitOld;
    NvU64                     vaLimitNew;
    NvU32                     attr;
    NvU32                     attr2;
    NvU32                     os02Flags          = 0;
    const MMU_WALK_CALLBACKS *pCb                = NULL;
    MMU_WALK_CALLBACKS        callbacks;
    MMU_WALK_USER_CTX         userCtx            = {0};
    GVAS_GPU_STATE           *pGpuState          = gvaspaceGetGpuState(pGVAS, pGpu);
    const NvBool              bAllChannels       = FLD_TEST_DRF(0080_CTRL_DMA_SET_PAGE_DIRECTORY,
                                                _FLAGS, _ALL_CHANNELS, _TRUE, pParams->flags);
    const NvBool              bFirstCommit       = (NULL == pGpuState->pRootInternal);
    const NvBool              bIgnoreChannelBusy = FLD_TEST_DRF(0080_CTRL_DMA_SET_PAGE_DIRECTORY,
                                                _FLAGS, _IGNORE_CHANNEL_BUSY, _TRUE, pParams->flags);
    const NvU64               rootPdeCoverage    = mmuFmtLevelPageSize(pGpuState->pFmt->pRoot);
    NvU64                     vaInternalLo       = NV_ALIGN_DOWN64(gvaspaceGetVaStart(pGVAS), rootPdeCoverage);
    NvU64                     vaInternalHi       = NV_ALIGN_UP64(pGVAS->vaLimitInternal + 1, rootPdeCoverage) - 1;
    NvU32                     gfid;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    //
    // For external VAS, create subcontext only after SetPageDirectory() call is made.
    // This will ensure that new PDB will be updated in all channels subcontext array.
    // bug 1805222 comment 11 for more details.
    //
    if (vaspaceIsExternallyOwned(pVAS))
    {
        MEMORY_DESCRIPTOR *pPDB = vaspaceGetPageDirBase(pVAS, pGpu);
        NV_ASSERT_OR_RETURN(pPDB == NULL, NV_ERR_INVALID_STATE);
    }

    switch (DRF_VAL(0080_CTRL_DMA_SET_PAGE_DIRECTORY, _FLAGS, _APERTURE, pParams->flags))
    {
        case NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_APERTURE_VIDMEM:
            aperture     = ADDR_FBMEM;
            cpuCacheAttr = NV_MEMORY_UNCACHED;
            break;
        case NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_APERTURE_SYSMEM_COH:
            aperture     = ADDR_SYSMEM;
            cpuCacheAttr = NV_MEMORY_CACHED;
            break;
        case NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS_APERTURE_SYSMEM_NONCOH:
            aperture     = ADDR_SYSMEM;
            cpuCacheAttr = NV_MEMORY_UNCACHED;
            break;
        default:
            NV_ASSERT_OR_RETURN(!"invalid aperture", NV_ERR_INVALID_ARGUMENT);
    }

    NV_ASSERT_OR_RETURN(!pGVAS->bIsMirrored, NV_ERR_NOT_SUPPORTED);
    // Ensure new page directory is not smaller than RM-managed region of the VA heap.
    vaLimitOld = pVAS->vasLimit;
    vaLimitNew = mmuFmtEntryIndexVirtAddrHi(pGpuState->pFmt->pRoot, 0, pParams->numEntries - 1);

    NV_ASSERT_OR_RETURN(vaLimitNew >= pGVAS->vaLimitInternal, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(vaLimitNew <= pGVAS->vaLimitMax,      NV_ERR_INVALID_ARGUMENT);

    // We have to truncate this later so we check for overflow here
    NV_ASSERT_OR_RETURN((NvU64)pParams->numEntries * (NvU64)pGpuState->pFmt->pRoot->entrySize <= NV_U32_MAX,
                      NV_ERR_INVALID_ARGUMENT);

    // Describe the new page directory.
    rootSizeNew = (NvU64)pParams->numEntries * (NvU64)pGpuState->pFmt->pRoot->entrySize;

    if (pGVAS->bIsAtsEnabled)
    {
        NV_PRINTF(LEVEL_INFO, "PASID: %u\n", pParams->pasid);
        pGVAS->processAddrSpaceId = pParams->pasid;
    }

    NV_ASSERT_OR_RETURN((pGVAS->flags & VASPACE_FLAGS_SHARED_MANAGEMENT) || vaspaceIsExternallyOwned(pVAS),
                     NV_ERR_NOT_SUPPORTED);

    // When placing top-level PDE in sysmem, must import the memory to RM
    if ((aperture == ADDR_SYSMEM) && !RMCFG_FEATURE_PLATFORM_GSP)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, IS_GFID_PF(gfid), NV_ERR_INVALID_ARGUMENT);

        //
        // If the page tables are in sysmem, we need to explicitly state this is
        // allocated outside RM and need to register with OS layer so that RM
        // can map the memory later in the busMapRmAperture code path.
        //
        attr  = DRF_DEF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS);
        attr2 = 0;

        status = RmDeprecatedConvertOs32ToOs02Flags(attr, attr2, 0, &os02Flags);
        NV_ASSERT_OR_GOTO(NV_OK == status, catch);

        status = osCreateMemFromOsDescriptor(pGpu, (NvP64)pParams->physAddress,
                                             hClient, os02Flags,
                                             &rootSizeNew, &pRootMemNew,
                                             NVOS32_DESCRIPTOR_TYPE_OS_PHYS_ADDR,
                                             RS_PRIV_LEVEL_KERNEL);
        NV_ASSERT_OR_GOTO(NV_OK == status, catch);
    }
    else if (!IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        NvU32 flags = MEMDESC_FLAGS_NONE;

        if (IS_GFID_VF(gfid))
        {
            // In SRIOV-heavy PDB address is GPA
            NV_CHECK_OR_RETURN(LEVEL_ERROR, aperture == ADDR_FBMEM, NV_ERR_INVALID_ARGUMENT);
            flags |= MEMDESC_FLAGS_GUEST_ALLOCATED;
        }

        // TODO: PDB alignment
        status = memdescCreate(&pRootMemNew, pGpu, (NvU32)rootSizeNew, RM_PAGE_SIZE, NV_TRUE, aperture,
                           cpuCacheAttr, flags);
        NV_ASSERT_OR_GOTO(NV_OK == status, catch);
        memdescDescribe(pRootMemNew, aperture, pParams->physAddress, (NvU32)rootSizeNew);
        memdescSetPageSize(pRootMemNew, VAS_ADDRESS_TRANSLATION(pVAS), RM_PAGE_SIZE);
    }

    //
    // In this case, we don't want to actually migrate Page Table, just perform
    // the above mapping, assuming it was required
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
        return NV_OK;

    if (vaspaceIsExternallyOwned(pVAS))
    {
        //
        // For externally owned vaspace we will associate a PDB that is provided
        // by the owner of the vaspace. THis is different from the "shared management"
        // scenario because, in the externally owned case RM will not allocate any page tables
        // or VA for this address space. This is a way to make sure RM programs the correct PDB
        // when clients use this address space to create a channel.
        // TODO: Make externally owned vaspace a separate vaspace class.
        //
        status = _gvaspaceSetExternalPageDirBase(pGVAS, pGpu, pRootMemNew);
        return status;
    }

    // Acquire MMU walker user context (always released below in catch label).
    NV_ASSERT_OK_OR_GOTO(status,
        gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx),
        catch);
    if (!bAllChannels)
    {
        // Specify single channel ID for which to update PDB if required by caller.
        userCtx.pChID = &pParams->chId;
    }

    //
    // If RM-internal page directory has not been saved yet, this is
    // the first external page directory committed.
    //
    if (bFirstCommit)
    {
        NvU32 rootSizeOld;

        //
        // Lock-down the root entries of the RM-internal VA range.
        // This forces the internal root page directory to be allocated if it
        // is not already.
        //
        status = mmuWalkReserveEntries(pGpuState->pWalk, pGpuState->pFmt->pRoot,
                                       vaInternalLo, vaInternalHi, NV_TRUE);
        NV_ASSERT_OR_GOTO(NV_OK == status, catch);

        // Save a reference to the RM-internal root for later revoke.
        mmuWalkGetPageLevelInfo(pGpuState->pWalk, pGpuState->pFmt->pRoot, 0,
                                (const MMU_WALK_MEMDESC**)&pGpuState->pRootInternal,
                                &rootSizeOld);
        NV_ASSERT(NULL != pGpuState->pRootInternal);

        // TODO: Proper refcount with memdesc cleanup - inverse of memdescFree/memdescDestroy.
        ++pGpuState->pRootInternal->RefCount;
        if (pGpuState->pRootInternal->Allocated > 0)
        {
            ++pGpuState->pRootInternal->Allocated;
        }
    }

    //
    // Invalidate MMU to kick out any entries associated with old PDB.
    // Because we're copying the PTE entry, we need to ensure all accesses
    // referring to the old entry are flushed, so we use PTE_DOWNGRADE here.
    //
    gvaspaceInvalidateTlb(pGVAS, pGpu, PTE_DOWNGRADE);

    //
    // Override callbacks for migration.
    // Copy and Fill callbacks are replaced to skip external entries.
    //
    pCb = mmuWalkGetCallbacks(pGpuState->pWalk);
    callbacks = *pCb;
    callbacks.CopyEntries = _gmmuWalkCBCopyEntries_SkipExternal;
    callbacks.FillEntries = _gmmuWalkCBFillEntries_SkipExternal;
    mmuWalkSetCallbacks(pGpuState->pWalk, &callbacks);

    // Track latest limit for PDB commit.
    pVAS->vasLimit = vaLimitNew;

    // Migrate root to the new memory.
    status = mmuWalkMigrateLevelInstance(pGpuState->pWalk, pGpuState->pFmt->pRoot, 0,
                                         (MMU_WALK_MEMDESC *)pRootMemNew, (NvU32)rootSizeNew,
                                         bIgnoreChannelBusy);
    NV_ASSERT_OR_GOTO(NV_OK == status, catch);

catch:
    // Restore walker callbacks.
    if (NULL != pCb)
    {
        mmuWalkSetCallbacks(pGpuState->pWalk, pCb);
    }
    // Rollback on failure.
    if (NV_OK != status)
    {
        pVAS->vasLimit = vaLimitOld;
        if (bFirstCommit)
        {
            if (NULL != pGpuState->pRootInternal)
            {
                memdescFree(pGpuState->pRootInternal);
                memdescDestroy(pGpuState->pRootInternal);
                pGpuState->pRootInternal = NULL;

                mmuWalkReleaseEntries(pGpuState->pWalk, pGpuState->pFmt->pRoot,
                                      vaInternalLo, vaInternalHi);
            }
        }
        memdescDestroy(pRootMemNew);
        pRootMemNew = NULL;
    }
    if (userCtx.pGpuState != NULL)
    {
        // Release MMU walker user context.
        gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
    }

    return status;
}

NV_STATUS
gvaspaceExternalRootDirRevoke_IMPL
(
    OBJGVASPACE                                 *pGVAS,
    OBJGPU                                      *pGpu,
    NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS *pParams
)
{
    OBJVASPACE               *pVAS        = staticCast(pGVAS, OBJVASPACE);
    NV_STATUS                 status      = NV_OK;
    MEMORY_DESCRIPTOR        *pRootMemNew = NULL;
    NvU32                     rootSizeNew;
    const MMU_WALK_CALLBACKS *pCb = NULL;
    MMU_WALK_CALLBACKS        callbacks;
    MMU_WALK_USER_CTX         userCtx = {0};
    GVAS_GPU_STATE           *pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    const NvU64               rootPdeCoverage = mmuFmtLevelPageSize(pGpuState->pFmt->pRoot);
    const NvU64               vaInternalLo = NV_ALIGN_DOWN64(pVAS->vasStart,           rootPdeCoverage);
    const NvU64               vaInternalHi = NV_ALIGN_UP64(pGVAS->vaLimitInternal + 1, rootPdeCoverage) - 1;

    //
    // Due to virtual without SRIOV design the page table update would take place in the host.
    // However, the guest is responsible for creating the IOMMU mapping for UVM to use.
    // This block makes sure we clean up the memdesc and the IOMMU mapping.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        MEMORY_DESCRIPTOR *pMemDesc = NULL;
        pMemDesc = vaspaceGetPageDirBase(pVAS, pGpu);

        //
        // memdescFree calls the destruct callback to free the created IOMMU mapping.
        // This only occurs in case the PDB is in sysmem. Otherwise this call
        // will NOP.
        //
        memdescFree(pMemDesc);

        // Free the RM memory used to hold the memdesc struct.
        memdescDestroy(pMemDesc);

        return status;
    }

    if (vaspaceIsExternallyOwned(pVAS))
    {
        MEMORY_DESCRIPTOR *pExternalPDB = NULL;

        // get the PDB
        pExternalPDB = vaspaceGetPageDirBase(pVAS, pGpu);
        memdescDestroy(pExternalPDB);
        pExternalPDB = NULL;
        status = _gvaspaceSetExternalPageDirBase(pGVAS, pGpu, pExternalPDB);
        return status;
    }

    NV_ASSERT_OR_RETURN(!pGVAS->bIsMirrored, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(NULL != pGpuState->pRootInternal, NV_ERR_INVALID_STATE);

    pRootMemNew = pGpuState->pRootInternal;
    rootSizeNew = (NvU32)pRootMemNew->Size;

    //
    // Invalidate MMU to kick out old entries before changing PDBs.
    // Because we're copying the PTE entry, we need to ensure all accesses
    // referring to the old entry are flushed, so we use PTE_DOWNGRADE here.
    //
    gvaspaceInvalidateTlb(pGVAS, pGpu, PTE_DOWNGRADE);

    // Acquire walker user context.
    NV_ASSERT_OK_OR_RETURN(gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx));

    //
    // Override callbacks for migration.
    // Copy and Fill callbacks are replaced to skip external entries.
    //
    pCb = mmuWalkGetCallbacks(pGpuState->pWalk);
    callbacks = *pCb;
    callbacks.CopyEntries = _gmmuWalkCBCopyEntries_SkipExternal;
    callbacks.FillEntries = _gmmuWalkCBFillEntries_SkipExternal;
    mmuWalkSetCallbacks(pGpuState->pWalk, &callbacks);

    // Restore limit for PDB commit.
    pVAS->vasLimit = pGVAS->vaLimitInternal;

    // Migrate root to the new memory.
    status = mmuWalkMigrateLevelInstance(pGpuState->pWalk, pGpuState->pFmt->pRoot, 0,
                                         (MMU_WALK_MEMDESC *)pRootMemNew, rootSizeNew, NV_FALSE);
    NV_ASSERT(NV_OK == status);

    // RM-internal root ownership has transferred back to walker.
    pGpuState->pRootInternal = NULL;

    // Release locked-down internal root entries.
    status = mmuWalkReleaseEntries(pGpuState->pWalk, pGpuState->pFmt->pRoot,
                                   vaInternalLo, vaInternalHi);
    NV_ASSERT(NV_OK == status);

    // Restore callbacks.
    mmuWalkSetCallbacks(pGpuState->pWalk, pCb);

    // Release walker user context.
    gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);

    // No possible response to failure - above asserts are best we can do.
    return NV_OK;
}

NV_STATUS
gvaspaceResize_IMPL
(
    OBJGVASPACE                              *pGVAS,
    NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_PARAMS *pParams
)
{
    OBJGPU         *pGpu = NULL;
    OBJVASPACE     *pVAS = staticCast(pGVAS, OBJVASPACE);
    NvU64           vaLimitNew;
    NV_STATUS       status = NV_OK;

    NV_ASSERT_OR_RETURN(!pGVAS->bIsMirrored, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(!(pGVAS->flags & VASPACE_FLAGS_SHARED_MANAGEMENT),
                     NV_ERR_NOT_SUPPORTED);

    // Calculate and check new VA range size (limit + 1).
    if (NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_MAX == pParams->vaSpaceSize)
    {
        vaLimitNew = pGVAS->vaLimitMax;
    }
    else
    {
        vaLimitNew = pVAS->vasStart + pParams->vaSpaceSize - 1;
    }

    // Abort early if not changing the size.
    if (vaLimitNew == pVAS->vasLimit)
    {
        goto done;
    }

    // Shrinking VAS space is not currently supported.
    NV_ASSERT_OR_RETURN(vaLimitNew >= pVAS->vasLimit,    NV_ERR_INVALID_LIMIT);
    NV_ASSERT_OR_RETURN(vaLimitNew <= pGVAS->vaLimitMax, NV_ERR_INVALID_LIMIT);

    if (gvaspaceIsInternalVaRestricted(pGVAS))
    {
        // This is not supported because, VASPACE_SHARED_MANAGEMENT supported
        // clients use Set/Revoke Root Page Dir to expand/shrink their VAs.
        // I do not find documented use case for this.
        NV_PRINTF(LEVEL_ERROR, "doesn't support clientVA expansion\n");
        NV_ASSERT(0);
    }


    // Commit new limit.
    pVAS->vasLimit         = vaLimitNew;
    pGVAS->vaLimitInternal = vaLimitNew;

    // Shrink the top VA region reserved for growth.
    pGVAS->pHeap->eheapFree(pGVAS->pHeap, pGVAS->vaLimitMax);
    NV_ASSERT_OK_OR_RETURN(_gvaspaceReserveTopForGrowth(pGVAS));

    // Loop over each GPU associated with the VAS.
    FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, pVAS->gpuMask)
    {
        GVAS_GPU_STATE           *pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
        MMU_WALK_USER_CTX         userCtx  = {0};
        const MMU_WALK_CALLBACKS *pCb      = mmuWalkGetCallbacks(pGpuState->pWalk);
        const MMU_FMT_LEVEL      *pRootFmt = pGpuState->pFmt->pRoot;
        MMU_WALK_MEMDESC         *pRootMem = NULL;
        NvU32                     rootSize = 0;
        NvBool                    bChanged = NV_FALSE;

        // If root has not been allocated yet it will use the new limit later.
        if (NULL == gvaspaceGetPageDirBase(pGVAS, pGpu))
        {
            goto doneGpu;
        }

        // Acquire walker context.
        NV_ASSERT_OK_OR_GOTO(status,
            gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx),
            doneGpu);

        status = mmuWalkGetPageLevelInfo(pGpuState->pWalk, pRootFmt, 0,
                                         (const MMU_WALK_MEMDESC**)&pRootMem, &rootSize);
        NV_ASSERT_OR_GOTO(NV_OK == status, doneGpu);

        //
        // Allocate new root manually. If realloc is not needed, LevelAlloc()
        // will retain the existing PDB.
        //
        status = pCb->LevelAlloc(&userCtx, pRootFmt, 0, vaLimitNew, NV_TRUE,
                                 &pRootMem, &rootSize, &bChanged);
        NV_ASSERT_OR_GOTO(NV_OK == status, doneGpu);

        // Migrate to new root (if reallocated), updating PDB and VA limit accordingly.
        if (bChanged)
        {
            //
            // Invalidate MMU to kick out any entries associated with old PDB.
            // Because we're copying the PTE entry, we need to ensure all accesses
            // referring to the old entry are flushed, so we use PTE_DOWNGRADE here.
            //
            gvaspaceInvalidateTlb(pGVAS, pGpu, PTE_DOWNGRADE);

            status = mmuWalkMigrateLevelInstance(pGpuState->pWalk, pRootFmt, 0,
                                                 pRootMem, rootSize, NV_FALSE);
            NV_ASSERT_OR_GOTO(NV_OK == status, doneGpu);
        }
        else
        {
            NvBool bDone;

            // Trigger an UpdatePdb to commit the new vaLimit to HW.
            bDone = pCb->UpdatePdb(&userCtx, pRootFmt, (const MMU_WALK_MEMDESC *)pRootMem,
                                   NV_FALSE);
            if (!bDone)
            {
                status = NV_ERR_INVALID_STATE;
                NV_ASSERT_OR_GOTO(bDone, doneGpu);
            }
        }

doneGpu:
        if ((NV_OK != status) && (NULL != pRootMem))
        {
            pCb->LevelFree(&userCtx, pRootFmt, 0, pRootMem);
        }
        // Release walker context.
        if (NULL != userCtx.pGpu)
        {
            gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
        }
        if (NV_OK != status)
        {
            break;
        }
    }
    FOR_EACH_GPU_IN_MASK_UC_END

done:
    if (NV_OK == status)
    {
        // On success, return usable VA space size.
        pParams->vaSpaceSize = pGVAS->vaLimitInternal - pVAS->vasStart + 1;
    }

    return status;
}

struct MMU_MAP_ITERATOR
{
    GMMU_ENTRY_VALUE entry;
};

static void
_gmmuWalkCBMapSingleEntry
(
    MMU_WALK_USER_CTX        *pUserCtx,
    const MMU_MAP_TARGET     *pTarget,
    const MMU_WALK_MEMDESC   *pLevelMem,
    const NvU32               entryIndexLo,
    const NvU32               entryIndexHi,
    NvU32                    *pProgress
)
{
    OBJGPU              *pGpu           = pUserCtx->pGpu;
    MemoryManager       *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    MMU_MAP_ITERATOR    *pIter          = pTarget->pIter;
    MEMORY_DESCRIPTOR   *pMemDesc       = (MEMORY_DESCRIPTOR*)pLevelMem;
    TRANSFER_SURFACE     surf           = {0};

    NV_PRINTF(LEVEL_INFO, "[GPU%u]: PA 0x%llX, Entries 0x%X-0x%X\n",
              pUserCtx->pGpu->gpuInstance,
              memdescGetPhysAddr(pMemDesc, AT_GPU, 0), entryIndexLo,
              entryIndexHi);

    NV_ASSERT_OR_RETURN_VOID(entryIndexLo == entryIndexHi);

    surf.pMemDesc = pMemDesc;
    surf.offset = entryIndexLo * pTarget->pLevelFmt->entrySize;

    NV_ASSERT_OR_RETURN_VOID(memmgrMemWrite(pMemoryManager, &surf,
                                            pIter->entry.v8,
                                            pTarget->pLevelFmt->entrySize,
                                            TRANSFER_FLAGS_NONE) == NV_OK);

    *pProgress = entryIndexHi - entryIndexLo + 1;
}

NV_STATUS
gvaspaceUpdatePde2_IMPL
(
    OBJGVASPACE                         *pGVAS,
    OBJGPU                              *pGpu,
    NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *pParams
)
{
    MMU_WALK_USER_CTX    userCtx   = {0};
    MMU_MAP_TARGET       mapTarget = {0};
    MMU_MAP_ITERATOR     mapIter;
    NvU32                numValidPTs = 0;
    GVAS_GPU_STATE      *pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    const GMMU_FMT      *pFmt      = pGpuState->pFmt;
    const MMU_FMT_LEVEL *pPTBig;
    const MMU_FMT_LEVEL *pPT4KB;
    NvU32                bigIdx;
    NvU32                ptIdx;
    const NvBool         bSparse = FLD_TEST_DRF(0080_CTRL_DMA_UPDATE_PDE_2, _FLAGS,
                                                _SPARSE, _TRUE, pParams->flags);
    NvU8                *pPdeBuffer = KERNEL_POINTER_FROM_NvP64(NvU8*, pParams->pPdeBuffer);

    portMemSet(&mapIter, 0, sizeof(mapIter));

    // Lookup leaf page table formats.
    pPTBig = mmuFmtFindLevelWithPageShift(pFmt->pRoot,
                                          BIT_IDX_64(gvaspaceGetBigPageSize(pGVAS)));
    pPT4KB = mmuFmtFindLevelWithPageShift(pFmt->pRoot, 12);

    NV_ASSERT_OR_RETURN(NULL != pPTBig, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(NULL != pPT4KB, NV_ERR_NOT_SUPPORTED);

    // Setup map target.
    mapTarget.pLevelFmt      = mmuFmtFindLevelParent(pFmt->pRoot, pPTBig, &bigIdx);
    mapTarget.pIter          = &mapIter;
    mapTarget.MapNextEntries = _gmmuWalkCBMapSingleEntry;

    NV_ASSERT_OR_RETURN(NULL != mapTarget.pLevelFmt,            NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(2 == mapTarget.pLevelFmt->numSubLevels, NV_ERR_NOT_SUPPORTED);

    // Setup PDE value.
    for (ptIdx = 0; ptIdx < NV0080_CTRL_DMA_UPDATE_PDE_2_PT_IDX__SIZE; ++ptIdx)
    {
        NV0080_CTRL_DMA_UPDATE_PDE_2_PAGE_TABLE_PARAMS *pPtParams = &pParams->ptParams[ptIdx];
        const GMMU_FMT_PDE                             *pPdeFmt;
        GMMU_APERTURE                                   aperture;

        // Select PDE format.
        switch (ptIdx)
        {
            case NV0080_CTRL_DMA_UPDATE_PDE_2_PT_IDX_SMALL:
                pPdeFmt = gmmuFmtGetPde(pFmt, mapTarget.pLevelFmt, !bigIdx);
                break;
            case NV0080_CTRL_DMA_UPDATE_PDE_2_PT_IDX_BIG:
                pPdeFmt = gmmuFmtGetPde(pFmt, mapTarget.pLevelFmt, bigIdx);
                break;
            default:
                NV_ASSERT_OR_RETURN(0, NV_ERR_NOT_SUPPORTED);
        }

        // Translate aperture field.
        switch (pPtParams->aperture)
        {
            case NV0080_CTRL_DMA_UPDATE_PDE_2_PT_APERTURE_INVALID:
                aperture = GMMU_APERTURE_INVALID;
                break;
            case NV0080_CTRL_DMA_UPDATE_PDE_2_PT_APERTURE_VIDEO_MEMORY:
                aperture = GMMU_APERTURE_VIDEO;
                break;
            case NV0080_CTRL_DMA_UPDATE_PDE_2_PT_APERTURE_SYSTEM_COHERENT_MEMORY:
                aperture = GMMU_APERTURE_SYS_COH;
                break;
            case NV0080_CTRL_DMA_UPDATE_PDE_2_PT_APERTURE_SYSTEM_NON_COHERENT_MEMORY:
                aperture = GMMU_APERTURE_SYS_NONCOH;
                break;
            default:
                NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_ARGUMENT);
        }
        gmmuFieldSetAperture(&pPdeFmt->fldAperture, aperture,
                             mapIter.entry.v8);

        if (GMMU_APERTURE_INVALID != aperture)
        {
            KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
            MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

            gmmuFieldSetAddress(gmmuFmtPdePhysAddrFld(pPdeFmt, aperture),
                kgmmuEncodePhysAddr(pKernelGmmu, aperture, pPtParams->physAddr,
                    NVLINK_INVALID_FABRIC_ADDR),
                mapIter.entry.v8);

            if (pFmt->version == GMMU_FMT_VERSION_3)
            {
                NvU32                 pdePcfHw = 0;
                NvU32                 pdePcfSw = 0;
                PMEMORY_DESCRIPTOR    pMemDesc = NULL;
                NvU32                 memSize  = 0;
                GMMU_ENTRY_VALUE      pde      = {{0}};
                GMMU_APERTURE         currAperture;
                TRANSFER_SURFACE      surf     = {0};

                NV_ASSERT_OK_OR_RETURN(
                    mmuWalkGetPageLevelInfo(pGpuState->pWalk, mapTarget.pLevelFmt,
                        (pParams->pdeIndex * mmuFmtLevelPageSize(mapTarget.pLevelFmt)),
                            (const MMU_WALK_MEMDESC**)&pMemDesc, &memSize));

                surf.pMemDesc = pMemDesc;
                surf.offset = pParams->pdeIndex * mapTarget.pLevelFmt->entrySize;

                NV_ASSERT_OK_OR_RETURN(memmgrMemRead(pMemoryManager, &surf,
                                                     pde.v8,
                                                     mapTarget.pLevelFmt->entrySize,
                                                     TRANSFER_FLAGS_DEFER_FLUSH));

                pdePcfHw = nvFieldGet32(&pPdeFmt->fldPdePcf, pde.v8);
                currAperture = gmmuFieldGetAperture(&pPdeFmt->fldAperture, pde.v8);

                if (currAperture != GMMU_APERTURE_INVALID)
                {
                    NV_ASSERT_OR_RETURN(
                       (kgmmuTranslatePdePcfFromHw_HAL(pKernelGmmu, pdePcfHw, currAperture, &pdePcfSw) == NV_OK), NV_ERR_INVALID_ARGUMENT);
                    pdePcfSw |= 1 << SW_MMU_PCF_UNCACHED_IDX;
                }
                else
                {
                    pdePcfSw = 1 << SW_MMU_PCF_UNCACHED_IDX;
                }
                NV_ASSERT_OR_RETURN(
                    (kgmmuTranslatePdePcfFromSw_HAL(pKernelGmmu, pdePcfSw, &pdePcfHw) == NV_OK), NV_ERR_INVALID_ARGUMENT);
                nvFieldSet32(&pPdeFmt->fldPdePcf, pdePcfHw, mapIter.entry.v8);
            }
            else
            {
                nvFieldSetBool(&pPdeFmt->fldVolatile, NV_TRUE, mapIter.entry.v8);
            }
            numValidPTs++;
        }
    }

    if (0 == numValidPTs)
    {
        // Check for sparse flags and invalid PDEs
        if (bSparse)
        {
            KernelGmmu            *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
            const GMMU_FMT_FAMILY *pFam        = kgmmuFmtGetFamily(pKernelGmmu, pFmt->version);
            mapIter.entry = pFam->sparsePdeMulti;
        }
    }
    else
    {
        const NvU32 sizeFracExp =
            DRF_VAL(0080_CTRL_DMA_UPDATE_PDE_2, _FLAGS, _PDE_SIZE, pParams->flags);

        NV_ASSERT_OR_RETURN(!bSparse, NV_ERR_INVALID_ARGUMENT);

        // Translate PDE_SIZE field.
        if (sizeFracExp > 0)
        {
            NV_ASSERT_OR_RETURN(nvFieldIsValid32(&pFmt->pPdeMulti->fldSizeRecipExp),
                             NV_ERR_INVALID_ARGUMENT);
            nvFieldSet32(&pFmt->pPdeMulti->fldSizeRecipExp, sizeFracExp, mapIter.entry.v8);
        }
    }

    if (NULL != pPdeBuffer)
    {
        // Copy value to user supplied buffer if provided.
        portMemCopy(pPdeBuffer, mapTarget.pLevelFmt->entrySize,
                    mapIter.entry.v8, mapTarget.pLevelFmt->entrySize);
    }
    else
    {
        OBJVASPACE  *pVAS = staticCast(pGVAS, OBJVASPACE);
        const NvU64  vaLo = pParams->pdeIndex * mmuFmtLevelPageSize(mapTarget.pLevelFmt);
        const NvU64  vaHi = vaLo + mmuFmtEntryVirtAddrMask(mapTarget.pLevelFmt);
        NV_STATUS    status;

        NV_ASSERT_OR_RETURN(vaLo >= pVAS->vasStart,         NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(vaHi <= pGVAS->vaLimitInternal, NV_ERR_INVALID_ARGUMENT);

        // Call walker to map the PDE.
        NV_ASSERT_OK_OR_RETURN(gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx));

        status = mmuWalkMap(userCtx.pGpuState->pWalk, vaLo, vaHi, &mapTarget);
        gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);

        NV_ASSERT_OR_RETURN(NV_OK == status, status);

        // Flush TLB if requested.
        if (FLD_TEST_DRF(0080_CTRL_DMA_UPDATE_PDE_2, _FLAGS, _FLUSH_PDE_CACHE, _TRUE,
                         pParams->flags))
        {
            gvaspaceInvalidateTlb(pGVAS, pGpu, PTE_UPGRADE);
        }
    }

    return NV_OK;
}

NV_STATUS
gvaspaceWalkUserCtxAcquire_IMPL
(
    OBJGVASPACE       *pGVAS,
    OBJGPU            *pGpu,
    const GVAS_BLOCK  *pVASBlock,
    MMU_WALK_USER_CTX *pUserCtx
)
{
    GVAS_GPU_STATE *pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);

    if(pGpuState == NULL || pGpuState->pWalk == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    // Must be UC.
    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    // If current context is non-NULL, a previous release was missed.
    NV_ASSERT_OR_RETURN(NULL == mmuWalkGetUserCtx(pGpuState->pWalk), NV_ERR_INVALID_STATE);

    pUserCtx->pGVAS     = pGVAS;
    pUserCtx->pGpu      = pGpu;
    pUserCtx->pGpuState = pGpuState;
    pUserCtx->pBlock    = pVASBlock;

    NV_ASSERT_OK_OR_RETURN(mmuWalkSetUserCtx(pUserCtx->pGpuState->pWalk, pUserCtx));
    return NV_OK;
}

void
gvaspaceWalkUserCtxRelease_IMPL
(
    OBJGVASPACE       *pGVAS,
    MMU_WALK_USER_CTX *pUserCtx
)
{
    // If current context doesn't match, must have nested acquires (not allowed).
    NV_ASSERT_OR_RETURN_VOID(pUserCtx->pGpuState);
    NV_ASSERT_OR_RETURN_VOID(pUserCtx->pGpuState->pWalk);
    NV_ASSERT(pUserCtx == mmuWalkGetUserCtx(pUserCtx->pGpuState->pWalk));
    NV_ASSERT_OK(mmuWalkSetUserCtx(pUserCtx->pGpuState->pWalk, NULL));
}

NV_STATUS
gvaspaceIncChanGrpRefCnt_IMPL
(
    OBJGVASPACE        *pGVAS,
    KernelChannelGroup *pKernelChannelGroup
)
{
    NvU32 *pChanGrpRefCnt;

    NV_ASSERT_OR_RETURN(pGVAS != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pKernelChannelGroup != NULL, NV_ERR_INVALID_ARGUMENT);

    pChanGrpRefCnt = mapFind(&pGVAS->chanGrpMap,
                             (NvU64)NV_PTR_TO_NvP64(pKernelChannelGroup));

    if (pChanGrpRefCnt != NULL)
    {
        (*pChanGrpRefCnt)++;
    }
    else
    {
        pChanGrpRefCnt = mapInsertNew(&pGVAS->chanGrpMap,
            (NvU64)NV_PTR_TO_NvP64(pKernelChannelGroup));
        NV_ASSERT_OR_RETURN(pChanGrpRefCnt != NULL, NV_ERR_INVALID_STATE);
        NV_PRINTF(LEVEL_INFO, "ChanGrp 0x%x on runlist 0x%x registered.\n",
                  pKernelChannelGroup->grpID, pKernelChannelGroup->runlistId);
        *pChanGrpRefCnt = 1;
    }

    NV_PRINTF(LEVEL_INFO,
              "ChanGrp 0x%x on runlist 0x%x refCnt increased to 0x%x\n",
              pKernelChannelGroup->grpID,
              pKernelChannelGroup->runlistId,
              *pChanGrpRefCnt);

    return NV_OK;
}

NV_STATUS
gvaspaceDecChanGrpRefCnt_IMPL
(
    OBJGVASPACE        *pGVAS,
    KernelChannelGroup *pKernelChannelGroup
)
{
    NvU32 *pChanGrpRefCnt;

    NV_ASSERT_OR_RETURN(pGVAS != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pKernelChannelGroup != NULL, NV_ERR_INVALID_ARGUMENT);

    pChanGrpRefCnt = mapFind(&pGVAS->chanGrpMap,
                             (NvU64)NV_PTR_TO_NvP64(pKernelChannelGroup));

    NV_ASSERT_OR_RETURN(pChanGrpRefCnt != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(*pChanGrpRefCnt != 0, NV_ERR_INVALID_STATE);

    (*pChanGrpRefCnt)--;

    NV_PRINTF(LEVEL_INFO,
              "ChanGrp 0x%x on runlist 0x%x refCnt decreased to 0x%x\n",
              pKernelChannelGroup->grpID,
              pKernelChannelGroup->runlistId,
              *pChanGrpRefCnt);

    if (*pChanGrpRefCnt == 0)
    {
        mapRemove(&pGVAS->chanGrpMap, pChanGrpRefCnt);
        NV_PRINTF(LEVEL_INFO, "ChanGrp 0x%x on runlist 0x%x unregistered.\n",
                  pKernelChannelGroup->grpID, pKernelChannelGroup->runlistId);
    }

    return NV_OK;
}

NvU32
gvaspaceGetChanGrpRefCnt_IMPL
(
    OBJGVASPACE        *pGVAS,
    KernelChannelGroup *pKernelChannelGroup
)
{
    NvU32 refCnt = 0;
    NvU32 *pChanGrpRefCnt;

    NV_ASSERT_OR_RETURN(pGVAS != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pKernelChannelGroup != NULL, NV_ERR_INVALID_ARGUMENT);

    pChanGrpRefCnt = mapFind(&pGVAS->chanGrpMap,
                             (NvU64)NV_PTR_TO_NvP64(pKernelChannelGroup));

    if (pChanGrpRefCnt != NULL)
    {
        refCnt = *pChanGrpRefCnt;
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,
                  "grpID 0x%x on runlistId 0x%x not registered to the VAS\n",
                  pKernelChannelGroup->grpID,
                  pKernelChannelGroup->runlistId);
    }

    return refCnt;
}

NV_STATUS
gvaspaceCheckChanGrpRefCnt_IMPL
(
    OBJGVASPACE        *pGVAS,
    KernelChannelGroup *pKernelChannelGroup
)
{
    // @todo Implement
    return NV_OK;
}

NV_STATUS
gvaspaceUnregisterAllChanGrps_IMPL
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    // TODO: This function should be made Physcial only
    if(IS_GSP_CLIENT(pGpu) || IS_VIRTUAL(pGpu))
    {
        return NV_OK;
    }

    if (!kfifoIsSubcontextSupported(pKernelFifo))
    {
        return NV_OK;
    }

    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
gvaspaceGetPageLevelInfo_IMPL
(
    OBJGVASPACE                                    *pGVAS,
    OBJGPU                                         *pGpu,
    NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pParams
)
{
    OBJVASPACE          *pVAS       = staticCast(pGVAS, OBJVASPACE);
    MMU_WALK            *pWalk      = NULL;
    const MMU_FMT_LEVEL *pLevelFmt  = NULL;
    const MMU_FMT_LEVEL *pTargetFmt = NULL;
    NvU32                level      = 0;
    NvU32                sublevel   = 0;
    GVAS_GPU_STATE      *pGpuState;

    if (NULL == pGVAS->pGpuStates)
    {
        // TODO: VMM must be enabled - remove once default.
        return NV_ERR_NOT_SUPPORTED;
    }

    pGpuState  = gvaspaceGetGpuState(pGVAS, pGpu);
    NV_ASSERT_OR_RETURN(NULL != pGpuState, NV_ERR_INVALID_ARGUMENT);

    pWalk      = pGpuState->pWalk;
    pLevelFmt  = pGpuState->pFmt->pRoot;

    pTargetFmt = mmuFmtFindLevelWithPageShift(pLevelFmt, BIT_IDX_64(pParams->pageSize));
    NV_ASSERT_OR_RETURN(NULL != pTargetFmt, NV_ERR_INVALID_ARGUMENT);

    for (level = 0; NULL != pLevelFmt; ++level)
    {
        MEMORY_DESCRIPTOR *pMemDesc = NULL;
        NvU32              memSize  = 0;

        NV_ASSERT_OR_RETURN(level < GMMU_FMT_MAX_LEVELS, NV_ERR_INVALID_STATE);

        NV_ASSERT_OK_OR_RETURN(
            mmuWalkGetPageLevelInfo(pWalk, pLevelFmt, pParams->virtAddress,
                                    (const MMU_WALK_MEMDESC**)&pMemDesc, &memSize));
        if (NULL == pMemDesc)
        {
            break;
        }

        pParams->levels[level].pFmt = (MMU_FMT_LEVEL *) pLevelFmt;
        pParams->levels[level].size = memSize;

        // Copy level formats
        portMemCopy((void *)&(pParams->levels[level].levelFmt), sizeof(MMU_FMT_LEVEL), (void *)pLevelFmt, sizeof(MMU_FMT_LEVEL));

        for (sublevel = 0; (sublevel < MMU_FMT_MAX_SUB_LEVELS) && (sublevel < pLevelFmt->numSubLevels); sublevel++)
        {
            portMemCopy((void *)&(pParams->levels[level].sublevelFmt[sublevel]), sizeof(MMU_FMT_LEVEL), (void *)(pLevelFmt->subLevels + sublevel), sizeof(MMU_FMT_LEVEL));
        }

        pParams->levels[level].physAddress =
            memdescGetPhysAddr(pMemDesc, VAS_ADDRESS_TRANSLATION(pVAS), 0);

        switch (memdescGetAddressSpace(pMemDesc))
        {
            case ADDR_FBMEM:
                pParams->levels[level].aperture = GMMU_APERTURE_VIDEO;
                break;
            case ADDR_SYSMEM:
                if (NV_MEMORY_CACHED == memdescGetCpuCacheAttrib(pMemDesc))
                {
                    pParams->levels[level].aperture = GMMU_APERTURE_SYS_COH;
                }
                else
                {
                    pParams->levels[level].aperture = GMMU_APERTURE_SYS_NONCOH;
                }
                break;
            default:
                NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_STATE);
        }

        pParams->levels[level].entryIndex = mmuFmtVirtAddrToEntryIndex(pLevelFmt, pParams->virtAddress);
        pLevelFmt = mmuFmtGetNextLevel(pLevelFmt, pTargetFmt);
    }

    pParams->numLevels = level;

    return NV_OK;
}

NV_STATUS
gvaspaceCopyServerRmReservedPdesToServerRm_IMPL
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu
)
{
    CALL_CONTEXT *pContext = resservGetTlsCallContext();
    NV_STATUS     status   = NV_OK;

    if ((!IS_VIRTUAL_WITH_SRIOV(pGpu) && !IS_GSP_CLIENT(pGpu)) ||
        (0 == pGVAS->vaStartServerRMOwned))
    {
        return NV_OK;
    }

    NvHandle                                             hClient;
    NvBool                                               bFreeNeeded  = NV_FALSE;
    NvHandle                                             hDevice;
    NvHandle                                             hVASpace;
    OBJGPUGRP                                           *pGpuGrp = gpumgrGetGpuGrpFromGpu(pGpu);

    if (NULL != pContext)
    {
        NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS pdeCopyParams;
        RsResourceRef                                       *pResourceRef = pContext->pResourceRef;
        RsResourceRef                                       *pDeviceRef   = NULL;

        hClient = pContext->pClient->hClient;

        if (pResourceRef->internalClassId == classId(VaSpaceApi))
        {
            hVASpace = pResourceRef->hResource;
        }
        else
        {
            // Can't think of a way we can reach here with a non zero handle.
            hVASpace = 0;
        }

        // Find the device ref.
        if (pResourceRef->internalClassId == classId(Device))
        {
            pDeviceRef = pResourceRef;
        }
        else
        {
            status = refFindAncestorOfType(pResourceRef, classId(Device), &pDeviceRef);
            NV_ASSERT_OR_RETURN(NV_OK == status, status);
        }

        hDevice = pDeviceRef->hResource;

        //
        // VAS handle is 0 for the device vaspace. Trigger an allocation on
        // server RM so that the plugin has a valid handle to the device VAS
        // under this client. This handle will be required by the plugin when
        // we make the RPC later.
        //
        if ((0 == hVASpace) && IS_GSP_CLIENT(pGpu))
        {
            NV_VASPACE_ALLOCATION_PARAMETERS vaParams = {0};

            status = serverutilGenResourceHandle(hClient, &hVASpace);
            NV_ASSERT_OR_RETURN(NV_OK == status, status);

            vaParams.index = NV_VASPACE_ALLOCATION_INDEX_GPU_DEVICE;

            NV_RM_RPC_ALLOC_OBJECT(pGpu,
                                   hClient,
                                   hDevice,
                                   hVASpace,
                                   FERMI_VASPACE_A,
                                   &vaParams,
                                   sizeof(vaParams),
                                   status);

            NV_ASSERT_OR_RETURN(NV_OK == status, status);
            bFreeNeeded = NV_TRUE;
        }

        status = _gvaspacePopulatePDEentries(pGVAS, pGpu, &pdeCopyParams);
        NV_ASSERT_OR_GOTO(NV_OK == status, done);

        //
        // RPC the details of these reserved PDEs to server RM so that server RM can
        // mirror these PDEs in its mmu walker state. Any lower level PDEs/PTEs
        // allocated under these top level PDEs will be modified exclusively by
        // server RM. Client RM won't touch those.
        //
        status = _gvaspaceCopyServerRmReservedPdesToServerRm(hClient, hVASpace, pGpu, &pdeCopyParams);
        NV_ASSERT_OR_GOTO(NV_OK == status, done);
done:
        if (bFreeNeeded)
        {
            NV_STATUS tmpStatus = NV_OK;

            NV_RM_RPC_FREE(pGpu, hClient, hDevice, hVASpace, tmpStatus);
            NV_ASSERT_OR_RETURN(NV_OK == tmpStatus, tmpStatus);
        }
    }
    //check to ensure server reserved PDEs are copied when global va space is created
    else if(!IS_VIRTUAL(pGpu) && pGpuGrp->pGlobalVASpace == dynamicCast(pGVAS, OBJVASPACE))
    {
        NV2080_CTRL_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER_PARAMS globalCopyParams;

        NV_ASSERT_OK_OR_RETURN(_gvaspacePopulatePDEentries(pGVAS, pGpu, &globalCopyParams.PdeCopyParams));

        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                pGpu->hInternalClient,
                                pGpu->hInternalSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER,
                                &globalCopyParams,
                                sizeof(NV2080_CTRL_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER_PARAMS)));
    }

    return status;
}

static NV_STATUS
_gvaspaceControl_Prolog
(
    VaSpaceApi   *pVaspaceApi,
    NvHandle      hSubDevice,
    NvU32         subDeviceId,
    OBJGVASPACE **ppGVAS,
    OBJGPU      **ppGpu
)
{
    OBJVASPACE *pVAS = NULL;

    NV_ASSERT_OK_OR_RETURN(
        vaspaceGetByHandleOrDeviceDefault(RES_GET_CLIENT(pVaspaceApi),
                                          RES_GET_PARENT_HANDLE(pVaspaceApi),
                                          RES_GET_HANDLE(pVaspaceApi),
                                          &pVAS));
    *ppGVAS = dynamicCast(pVAS, OBJGVASPACE);
    NV_ASSERT_OR_RETURN(NULL != *ppGVAS, NV_ERR_INVALID_CLASS);

    if (0 != hSubDevice)
    {
        Subdevice *pSubDevice;

        NV_ASSERT_OK_OR_RETURN(
                subdeviceGetByHandle(RES_GET_CLIENT(pVaspaceApi),
                    hSubDevice, &pSubDevice));

        *ppGpu = GPU_RES_GET_GPU(pSubDevice);

        GPU_RES_SET_THREAD_BC_STATE(pSubDevice);
    }
    else
    {
        *ppGpu = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));
        // Pick a specific sub-device if requested.
        if (0 != subDeviceId)
        {
            *ppGpu = gpumgrGetGpuFromSubDeviceInst(gpuGetDeviceInstance(*ppGpu), subDeviceId - 1);
            NV_ASSERT_OR_RETURN(NULL != *ppGpu, NV_ERR_INVALID_ARGUMENT);
        }
        gpumgrSetBcEnabledStatus(*ppGpu, NV_FALSE);
    }

    return NV_OK;
}


NV_STATUS
vaspaceapiCtrlCmdVaspaceGetGmmuFormat_IMPL
(
    VaSpaceApi                                 *pVaspaceApi,
    NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS *pGmmuFormatParams
)
{
    OBJGVASPACE *pGVAS = NULL;
    OBJGPU      *pGpu  = NULL;

    NV_ASSERT_OK_OR_RETURN(
        _gvaspaceControl_Prolog(pVaspaceApi, pGmmuFormatParams->hSubDevice,
                                pGmmuFormatParams->subDeviceId, &pGVAS, &pGpu));

    pGmmuFormatParams->pFmt = (GMMU_FMT *) gvaspaceGetGmmuFmt_IMPL(pGVAS, pGpu);
    NV_ASSERT_OR_RETURN(NULL != pGmmuFormatParams->pFmt, NV_ERR_INVALID_STATE);

    return NV_OK;
}

NV_STATUS
vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_IMPL
(
    VaSpaceApi                                     *pVaspaceApi,
    NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pPageLevelInfoParams
)
{
    OBJGVASPACE *pGVAS = NULL;
    OBJGPU      *pGpu  = NULL;

    NV_ASSERT_OR_RETURN(pPageLevelInfoParams->flags <=
                NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_FLAG_BAR1, NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OK_OR_RETURN(
            _gvaspaceControl_Prolog(pVaspaceApi, pPageLevelInfoParams->hSubDevice,
                                    pPageLevelInfoParams->subDeviceId, &pGVAS, &pGpu));

    if (NULL == pGVAS->pGpuStates)
    {
        // TODO: VMM must be enabled - remove once default.
        return NV_ERR_NOT_SUPPORTED;
    }

    if (IS_VIRTUAL(pGpu) && !gpuIsSplitVasManagementServerClientRmEnabled(pGpu))
    {
        NV_STATUS status = NV_OK;
        if (!(gvaspaceGetFlags(pGVAS) & VASPACE_FLAGS_FLA))
        {
            CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
            RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;

            NV_RM_RPC_CONTROL(pGpu,
                              pRmCtrlParams->hClient,
                              pRmCtrlParams->hObject,
                              pRmCtrlParams->cmd,
                              pRmCtrlParams->pParams,
                              pRmCtrlParams->paramsSize,
                              status);
            return status;
        }
    }

    if (pPageLevelInfoParams->flags & NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_FLAG_BAR1)
    {
        KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
        RsCpuMapping *pCpuMapping = NULL;
        NvU64 offset;
        // switch the vaspace API pointer to bar1 ptr
        pGVAS = dynamicCast(kbusGetBar1VASpace_HAL(pGpu, pKernelBus), OBJGVASPACE);
        NV_ASSERT_OR_RETURN(pGVAS != NULL, NV_ERR_INVALID_ARGUMENT);
        pCpuMapping = CliFindMappingInClient(RES_GET_CLIENT_HANDLE(pVaspaceApi),
                                             RES_GET_PARENT_HANDLE(pVaspaceApi),
                                             (NvP64)pPageLevelInfoParams->virtAddress);
        offset = pPageLevelInfoParams->virtAddress - (NvU64)pCpuMapping->pLinearAddress;
        NV_ASSERT_OR_RETURN(pCpuMapping->pPrivate->memArea.numRanges == 1, NV_ERR_INVALID_ARGUMENT);
        pPageLevelInfoParams->virtAddress = pCpuMapping->pPrivate->memArea.pRanges[0].start + offset;
    }

    return gvaspaceGetPageLevelInfo(pGVAS, pGpu, pPageLevelInfoParams);
}

#if defined(DEBUG) || defined(DEVELOP) || defined(NV_VERIF_FEATURES)
NV_STATUS
vaspaceapiCtrlCmdVaspaceGetPageLevelInfoVerif_IMPL
(
    VaSpaceApi                                     *pVaspaceApi,
    NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pPageLevelInfoParams
)
{
    return vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_IMPL(pVaspaceApi, pPageLevelInfoParams);
}
#endif

NV_STATUS
vaspaceapiCtrlCmdVaspaceReserveEntries_IMPL
(
    VaSpaceApi                                 *pVaspaceApi,
    NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS *pReserveEntriesParams
)
{
    OBJGVASPACE    *pGVAS  = NULL;
    OBJGPU         *pGpu   = NULL;
    NV_STATUS       status = NV_OK;
    GVAS_GPU_STATE *pGpuState;

    NV_ASSERT_OK_OR_RETURN(
        _gvaspaceControl_Prolog(pVaspaceApi, pReserveEntriesParams->hSubDevice,
                                pReserveEntriesParams->subDeviceId, &pGVAS, &pGpu));

    if (NULL == pGVAS->pGpuStates)
    {
        // TODO: VMM must be enabled - remove once default.
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_ASSERT_OR_RETURN(ONEBITSET(pReserveEntriesParams->pageSize), NV_ERR_INVALID_ARGUMENT);

    if (IS_VIRTUAL(pGpu) && !gpuIsSplitVasManagementServerClientRmEnabled(pGpu))
    {
        if (!(gvaspaceGetFlags(pGVAS) & VASPACE_FLAGS_FLA))
        {
            CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
            RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;

            NV_RM_RPC_CONTROL(pGpu,
                              pRmCtrlParams->hClient,
                              pRmCtrlParams->hObject,
                              pRmCtrlParams->cmd,
                              pRmCtrlParams->pParams,
                              pRmCtrlParams->paramsSize,
                              status);
            return status;
        }
    }

    pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    NV_ASSERT_OR_RETURN(NULL != pGpuState, NV_ERR_INVALID_STATE);

    // Alloc and bind page level instances.
    status = gvaspaceReservePageTableEntries(pGVAS, pGpu, pReserveEntriesParams->virtAddrLo,
                                             pReserveEntriesParams->virtAddrHi,
                                             pReserveEntriesParams->pageSize);
    NV_ASSERT(NV_OK == status);

    return status;
}

NV_STATUS
vaspaceapiCtrlCmdVaspaceReleaseEntries_IMPL
(
    VaSpaceApi                                 *pVaspaceApi,
    NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS *pReleaseEntriesParams
)
{
    OBJGVASPACE *pGVAS  = NULL;
    OBJGPU      *pGpu   = NULL;
    NV_STATUS    status = NV_OK;

    NV_ASSERT_OK_OR_RETURN(
        _gvaspaceControl_Prolog(pVaspaceApi, pReleaseEntriesParams->hSubDevice,
                                pReleaseEntriesParams->subDeviceId, &pGVAS, &pGpu));

    if (NULL == pGVAS->pGpuStates)
    {
        // TODO: VMM must be enabled - remove once default.
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_ASSERT_OR_RETURN(ONEBITSET(pReleaseEntriesParams->pageSize), NV_ERR_INVALID_ARGUMENT);

    if (IS_VIRTUAL(pGpu) && !gpuIsSplitVasManagementServerClientRmEnabled(pGpu))
    {
        if (!(gvaspaceGetFlags(pGVAS) & VASPACE_FLAGS_FLA))
        {
            CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
            RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;

            NV_RM_RPC_CONTROL(pGpu,
                              pRmCtrlParams->hClient,
                              pRmCtrlParams->hObject,
                              pRmCtrlParams->cmd,
                              pRmCtrlParams->pParams,
                              pRmCtrlParams->paramsSize,
                              status);
            return status;
        }
    }

    // Unbind and free page level instances.
    status = gvaspaceReleasePageTableEntries(pGVAS, pGpu, pReleaseEntriesParams->virtAddrLo,
                                             pReleaseEntriesParams->virtAddrHi,
                                             pReleaseEntriesParams->pageSize);
    NV_ASSERT(NV_OK == status);

    return status;
}

NV_STATUS
vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_IMPL
(
    VaSpaceApi                                           *pVaspaceApi,
    NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pCopyServerReservedPdesParams
)
{
    OBJGVASPACE      *pGVAS   = NULL;
    OBJGPU           *pGpu    = NULL;

    NV_ASSERT_OK_OR_RETURN(
        _gvaspaceControl_Prolog(pVaspaceApi, pCopyServerReservedPdesParams->hSubDevice,
                                pCopyServerReservedPdesParams->subDeviceId, &pGVAS, &pGpu));

    if (((pCopyServerReservedPdesParams->virtAddrHi < pCopyServerReservedPdesParams->virtAddrLo) &&
             !(RMCFG_FEATURE_PLATFORM_GSP || IS_GSP_CLIENT(pGpu) ||
                 (IS_VIRTUAL(pGpu) && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu)))) ||
        (pGVAS->vaLimitServerRMOwned == 0))
    {
        // BUG 4580145 WAR: make sure only GSP's context is updated; caller is WAR if (AddrHi < AddrLo)
        return NV_OK;
    }

    return gvaspaceCopyServerReservedPdes(pGVAS, pGpu, pCopyServerReservedPdesParams);
}

NV_STATUS
gvaspaceCopyServerReservedPdes_IMPL
(
    OBJGVASPACE      *pGVAS,
    OBJGPU           *pGpu,
    NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pCopyServerReservedPdesParams
)
{

    OBJVASPACE       *pVAS = NULL;
    KernelGmmu       *pKernelGmmu   = NULL;
    NV_STATUS         status  = NV_OK;
    MMU_WALK_USER_CTX userCtx = {0};
    GVAS_GPU_STATE   *pGpuState;
    NvS32             i;

    pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    pVAS = staticCast(pGVAS, OBJVASPACE);

    if (NULL == pGVAS->pGpuStates)
    {
        // TODO: VMM must be enabled - remove once default.
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_ASSERT_OR_RETURN(pCopyServerReservedPdesParams->numLevelsToCopy <= GMMU_FMT_MAX_LEVELS, NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN(ONEBITSET(pCopyServerReservedPdesParams->pageSize), NV_ERR_INVALID_ARGUMENT);

    if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL(pGpu))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        return status;
    }

    pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    NV_ASSERT_OR_RETURN(NULL != pGpuState, NV_ERR_INVALID_STATE);

    if (pCopyServerReservedPdesParams->virtAddrHi >= pCopyServerReservedPdesParams->virtAddrLo)
    {
        // Alloc and bind page level instances.
        status = gvaspaceReservePageTableEntries(pGVAS, pGpu, pCopyServerReservedPdesParams->virtAddrLo,
                                                 pCopyServerReservedPdesParams->virtAddrHi,
                                                 pCopyServerReservedPdesParams->pageSize);
        NV_ASSERT_OR_RETURN(NV_OK == status, status);
    }

    // Kick out any stale TLB entries.
    gvaspaceInvalidateTlb(pGVAS, pGpu, PTE_DOWNGRADE);

    // Acquire walker context.
    NV_ASSERT_OK_OR_RETURN(gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx));

    for (i = pCopyServerReservedPdesParams->numLevelsToCopy - 1; i >= 0; i--)
    {
        MEMORY_DESCRIPTOR *pMemDescNew;
        NV_ADDRESS_SPACE  aperture;
        NvU64 flags = 0;

        if (!RMCFG_FEATURE_PLATFORM_GSP)
        {
            flags = MEMDESC_FLAGS_GUEST_ALLOCATED;
        }

        switch(pCopyServerReservedPdesParams->levels[i].aperture)
        {
            case GMMU_APERTURE_VIDEO:
                aperture = ADDR_FBMEM;
                break;
            case GMMU_APERTURE_SYS_COH:
            case GMMU_APERTURE_SYS_NONCOH:
                aperture = ADDR_SYSMEM;
                break;
            default:
                NV_ASSERT_OR_GOTO(0, done);
        }

        status = memdescCreate(&pMemDescNew, pGpu,
                               pCopyServerReservedPdesParams->levels[i].size,
                               RM_PAGE_SIZE,
                               NV_TRUE,
                               aperture,
                               kgmmuGetPTEAttr(pKernelGmmu),
                               flags);
        NV_ASSERT_OR_GOTO(NV_OK == status, done);

        // Setup a memdesc in server to describe the client's PDE backing memory
        memdescDescribe(pMemDescNew, aperture, pCopyServerReservedPdesParams->levels[i].physAddress,
                        pCopyServerReservedPdesParams->levels[i].size);
        memdescSetPageSize(pMemDescNew, VAS_ADDRESS_TRANSLATION(pVAS), RM_PAGE_SIZE);

        // Modify the server's walker state with the new backing memory.
        const MMU_FMT_LEVEL *pLevelFmt =
              mmuFmtFindLevelWithPageShift(pGpuState->pFmt->pRoot,
                                    pCopyServerReservedPdesParams->levels[i].pageShift);
        status = mmuWalkModifyLevelInstance(pGpuState->pWalk,
                                            pLevelFmt,
                                            pGVAS->vaStartServerRMOwned,
                                            (MMU_WALK_MEMDESC*)pMemDescNew,
                                            (NvU32)pCopyServerReservedPdesParams->levels[i].size,
                                            NV_FALSE,
                                            NV_FALSE,
                                            NV_FALSE);
        NV_ASSERT_OR_GOTO(NV_OK == status, done);
    }

done:
    gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
    return status;
}

NV_STATUS
vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize_IMPL
(
    VaSpaceApi *pVaspaceApi,
    NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS *pVaspaceGetHostRmManagedSizeParams
)
{
    OBJGVASPACE      *pGVAS   = NULL;
    OBJGPU           *pGpu    = NULL;

    NV_ASSERT_OK_OR_RETURN(
        _gvaspaceControl_Prolog(pVaspaceApi, pVaspaceGetHostRmManagedSizeParams->hSubDevice,
                                pVaspaceGetHostRmManagedSizeParams->subDeviceId, &pGVAS, &pGpu));

    // If GSP or guest-managed VA space are enabled, then RM requires extra VA range
    if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL_WITH_FULL_SRIOV(pGpu))
    {
        pVaspaceGetHostRmManagedSizeParams->requiredVaRange = VA_SIZE_FULL_SRIOV_OR_GSP;
    }
    else
    {
        pVaspaceGetHostRmManagedSizeParams->requiredVaRange = 0;
    }

    return NV_OK;
}

/********************Local routines used in this file alone*******************/

/*!
 * Reserve VA block between current limit and max limit for later growth.
 */
static NV_STATUS
_gvaspaceReserveTopForGrowth
(
    OBJGVASPACE *pGVAS
)
{
    if (pGVAS->vaLimitInternal < pGVAS->vaLimitMax)
    {
        EMEMBLOCK *pBlock;
        NvU32      allocFlags;
        NvU64      allocOffset;
        NvU64      allocSize;

        allocFlags  = NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
        allocOffset = pGVAS->vaLimitInternal + 1;
        allocSize   = pGVAS->vaLimitMax - pGVAS->vaLimitInternal;

        return pGVAS->pHeap->eheapAlloc(pGVAS->pHeap, VAS_EHEAP_OWNER_RSVD,
                                        &allocFlags, &allocOffset, &allocSize,
                                        1, 1, &pBlock, NULL, NULL);
    }
    return NV_OK;
}

/*!
 * Reserve a range of VA from rangeLo to rangeHi for later use.
 */
static NV_STATUS
_gvaspaceReserveRange
(
    OBJGVASPACE *pGVAS,
    NvU64 rangeLo,
    NvU64 rangeHi
)
{
    if (rangeLo <= rangeHi)
    {
        EMEMBLOCK *pBlock;
        NvU32      allocFlags;
        NvU64      allocOffset;
        NvU64      allocSize;

        allocFlags  = NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
        allocOffset = rangeLo;
        allocSize   = rangeHi - rangeLo + 1;

        return pGVAS->pHeap->eheapAlloc(pGVAS->pHeap, VAS_EHEAP_OWNER_RSVD,
                                        &allocFlags, &allocOffset, &allocSize,
                                        1, 1, &pBlock, NULL, NULL);
    }
    return NV_ERR_INVALID_ARGUMENT;
}

/*!
 * Pin the page tables covering external management (leaf PDE) alignment if the
 * provided VA belongs to a lazy VA reservation. This is used for lazy WDDMv1 page tables.
 */
static NV_STATUS
_gvaspacePinLazyPageTables
(
    OBJGVASPACE       *pGVAS,
    OBJGPU            *pGpu,
    const NvU64        va
)
{
    NV_STATUS     status = NV_OK;
    EMEMBLOCK    *pMemBlock;
    PGVAS_BLOCK   pVASBlock;

    // Search for the VA block, abort if not found.
    pMemBlock = pGVAS->pHeap->eheapGetBlock(pGVAS->pHeap, va, 0);
    if (!pMemBlock)
    {
        return NV_OK;
    }
    pVASBlock = (PGVAS_BLOCK)pMemBlock->pData;

    // Only proceed if lazy and we have lock mask info.
    if (pVASBlock->flags.bLazy && (0 != pVASBlock->pageSizeLockMask))
    {
        NvU32             pageShift;
        MMU_WALK_USER_CTX userCtx = {0};
        NvU64             vaLo = NV_ALIGN_DOWN64(va, pGVAS->extManagedAlign);
        NvU64             vaHi = NV_ALIGN_UP64(va + 1, pGVAS->extManagedAlign) - 1;

        // Clamp VA to block limits.
        vaLo = NV_MAX(vaLo, pMemBlock->begin);
        vaHi = NV_MIN(vaHi, pMemBlock->end);

        NV_ASSERT_OK_OR_RETURN(gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, pVASBlock, &userCtx));

        // Loop over each page size requested by client.
        FOR_EACH_INDEX_IN_MASK(64, pageShift, pVASBlock->pageSizeLockMask)
        {
            // Pre-reserve page level instances in the VA range.
            const MMU_FMT_LEVEL *pLevelFmt =
                mmuFmtFindLevelWithPageShift(userCtx.pGpuState->pFmt->pRoot, pageShift);
            status = mmuWalkReserveEntries(userCtx.pGpuState->pWalk, pLevelFmt,
                                           vaLo, vaHi, NV_TRUE);
            if (NV_OK != status)
            {
                DBG_BREAKPOINT();
                break;
            }
        }
        FOR_EACH_INDEX_IN_MASK_END

        gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
    }

    return status;
}

/*!
 * @brief Callback to free leaked virtual allocations.
 *
 * @param[in[     pHeap        Heap being traversed.
 * @param[in]     pEnv         Callback environment.
 * @param[in]     pMemBlock    Current heap block (may be free or used).
 * @param[in,out] pContinue    Initially 1, if set to 0 traversal stops.
 * @param[in,out] pInvalCursor Initially 0, must be set to 1 if current block
 *                             is deleted during the callback (to prevent it
 *                             from being used to find next block).
 */
static NV_STATUS
_gvaspaceFreeVASBlock
(
    OBJEHEAP  *pHeap,
    void      *pEnv,
    EMEMBLOCK *pMemBlock,
    NvU32     *pContinue,
    NvU32     *pInvalCursor
)
{
    OBJGVASPACE *pGVAS = pEnv;

    if (NVOS32_BLOCK_TYPE_FREE != pMemBlock->owner && VAS_EHEAP_OWNER_RSVD != pMemBlock->owner)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Virtual allocation leak in range 0x%llX-0x%llX\n",
                  pMemBlock->begin, pMemBlock->end);

        gvaspaceFree(pGVAS, pMemBlock->begin);
        *pInvalCursor = 1;
    }

    return NV_OK;
}

/*!
 * @brief Register a mapping range for a given GPU.
 */
static NV_STATUS
_gvaspaceMappingInsert
(
    OBJGVASPACE        *pGVAS,
    OBJGPU             *pGpu,
    GVAS_BLOCK         *pVASBlock,
    const NvU64         vaLo,
    const NvU64         vaHi,
    const VAS_MAP_FLAGS flags
)
{
    NV_STATUS     status    = NV_OK;
    OBJVASPACE   *pVAS      = staticCast(pGVAS, OBJVASPACE);
    GVAS_MAPPING *pMapNode  = NULL;
    const NvU32   gpuMask   = NVBIT(pGpu->gpuInstance);

    // Ensure this is not a remap.
    NV_ASSERT_OR_RETURN(!flags.bRemap, NV_ERR_INVALID_ARGUMENT);

    // Check if GPU is valid for this VAS.
    NV_ASSERT_OR_RETURN(gpuMask == (pVAS->gpuMask & gpuMask), NV_ERR_INVALID_ARGUMENT);

    // Search for existing mapping.
    status = btreeSearch(vaLo, (NODE**)&pMapNode, &pVASBlock->pMapTree->node);
    if (NV_OK == status)
    {
        // If it already exists, check for consistency.
        NV_CHECK_OR_RETURN(LEVEL_ERROR, 0 == (pMapNode->gpuMask & gpuMask),
                           NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(pMapNode->node.keyStart == vaLo, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(pMapNode->node.keyEnd   == vaHi, NV_ERR_INVALID_ARGUMENT);

        // Commit the new GPU mask to the mapping.
        pMapNode->gpuMask |= gpuMask;
    }
    else
    {
        // If it doesn't exist, allocate new node.
        pMapNode = portMemAllocNonPaged(sizeof(*pMapNode));
        NV_ASSERT_OR_RETURN(NULL != pMapNode, NV_ERR_NO_MEMORY);

        portMemSet(pMapNode, 0, sizeof(*pMapNode));
        pMapNode->node.keyStart = vaLo;
        pMapNode->node.keyEnd   = vaHi;
        pMapNode->gpuMask       = gpuMask;

        // Insert mapping node.
        status = btreeInsert(&pMapNode->node, (NODE**)&pVASBlock->pMapTree);
        NV_ASSERT_OR_GOTO(NV_OK == status, catch);

catch:
        if (NV_OK != status)
        {
            portMemFree(pMapNode);
        }
    }

    return status;
}

/*
 *  @brief Override to an externally owned PDB.
 *  For externally owned vaspace we will associate a PDB that is provided
 *  by the owner of the vaspace. THis is different from the "shared management"
 *  scenario because, in the externally owned case RM will not allocate any page tables
 *  or VA for this address space. This is a way to make sure RM programs the correct PDB
 *  when clients use this address space to create a channel.
 *  TODO: Make externally owned vaspace a separate vaspace class.
*/
static NV_STATUS
_gvaspaceSetExternalPageDirBase
(
    OBJGVASPACE       *pGVAS,
    OBJGPU            *pGpu,
    MEMORY_DESCRIPTOR *pPDB
)
{
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    if (pGVAS->bIsExternallyOwned)
    {
        pGVAS->pExternalPDB = pPDB;
    }
    return NV_OK;
}

/*!
 * @brief Unregister a mapping range for a given GPU.
 */
static NV_STATUS
_gvaspaceMappingRemove
(
    OBJGVASPACE       *pGVAS,
    OBJGPU            *pGpu,
    GVAS_BLOCK        *pVASBlock,
    const NvU64        vaLo,
    const NvU64        vaHi
)
{
    NV_STATUS     status    = NV_OK;
    GVAS_MAPPING *pMapNode  = NULL;
    const NvU32   gpuMask   = NVBIT(pGpu->gpuInstance);
    NvU64 nodeVaLo;
    NvU64 nodeVaHi;
    const VAS_MAP_FLAGS flags = { 0 };
    NvBool bPartialUnmap = NV_FALSE;
    NvBool bLoEntryAdded = NV_FALSE;
    NvBool bHiEntryAdded = NV_FALSE;

    // Search for existing mapping.
    status = btreeSearch(vaLo, (NODE**)&pMapNode, &pVASBlock->pMapTree->node);
    NV_ASSERT_OR_RETURN(NV_OK == status, status);

    // Check for consistency.
    NV_ASSERT_OR_RETURN(gpuMask == (pMapNode->gpuMask & gpuMask), NV_ERR_INVALID_ARGUMENT);
    if (pMapNode->node.keyStart != vaLo ||
        pMapNode->node.keyEnd   != vaHi)
    {
        // check the entire range falls within the existing mapping
        NV_ASSERT_OR_RETURN(pMapNode->node.keyStart <= vaLo &&
                            pMapNode->node.keyEnd >= vaHi, NV_ERR_INVALID_ARGUMENT);
        // Allow partial unmap only when the mapping is not shared
        NV_ASSERT_OR_RETURN(pMapNode->gpuMask == gpuMask,
                            NV_ERR_INVALID_ARGUMENT);
        NV_PRINTF(LEVEL_INFO, "Partial unmap: Removing vaLo: 0x%llx vaHi: 0x%llx.\n",
                  vaLo, vaHi);
        bPartialUnmap = NV_TRUE;
    }

    // Remove GPU from mapping mask.
    pMapNode->gpuMask &= ~gpuMask;

    nodeVaLo = pMapNode->node.keyStart;
    nodeVaHi = pMapNode->node.keyEnd;
    // Remove mapping if unused.
    if (0 == pMapNode->gpuMask)
    {
        btreeUnlink(&pMapNode->node, (NODE**)&pVASBlock->pMapTree);
    }

    if (nodeVaLo < vaLo)
    {
        status = _gvaspaceMappingInsert(pGVAS, pGpu, pVASBlock, nodeVaLo, vaLo - 1, flags);
        NV_ASSERT_OR_GOTO(NV_OK == status, done);
        NV_PRINTF(LEVEL_INFO, "Partial unmap: Inserting partial vaLo: 0x%llx "
                  "vaHi: 0x%llx. status: 0x%x\n", nodeVaLo, vaLo - 1, status);
        bLoEntryAdded = NV_TRUE;
    }
    if (nodeVaHi > vaHi)
    {
        status = _gvaspaceMappingInsert(pGVAS, pGpu, pVASBlock, vaHi + 1, nodeVaHi, flags);
        NV_ASSERT_OR_GOTO(NV_OK == status, done);
        NV_PRINTF(LEVEL_INFO, "Partial unmap: Inserting partial vaLo: 0x%llx "
                  "vaHi: 0x%llx. status: 0x%x\n",
                  vaHi + 1, nodeVaHi, status);
        bHiEntryAdded = NV_TRUE;
    }

done:
    if (bPartialUnmap &&
        (status != NV_OK))
    {
        if (bLoEntryAdded)
        {
            NV_ASSERT_OK(_gvaspaceMappingRemove(pGVAS, pGpu, pVASBlock,
                                                nodeVaLo, vaLo - 1));
        }
        if (bHiEntryAdded)
        {
            NV_ASSERT_OK(_gvaspaceMappingRemove(pGVAS, pGpu, pVASBlock,
                                                vaHi + 1, nodeVaHi));
        }

        // Add the original mapping back
        portMemSet(pMapNode, 0, sizeof(*pMapNode));
        pMapNode->node.keyStart = nodeVaLo;
        pMapNode->node.keyEnd   = nodeVaHi;
        pMapNode->gpuMask       = gpuMask;
        NV_ASSERT_OK(btreeInsert(&pMapNode->node, (NODE**)&pVASBlock->pMapTree));

        return status;
    }

    // Free if unused.
    if (0 == pMapNode->gpuMask)
    {
        portMemFree(pMapNode);
    }
    return status;
}

/*!
 * Returns if the RM va space is restricted
 * True only for MAC GPU Va space
 * @param[in] pGVAS Pointer to the OBJGVASPACE object
 */
NvBool
gvaspaceIsInternalVaRestricted_IMPL(OBJGVASPACE *pGVAS)
{
    return pGVAS->bRMInternalRestrictedVaRange;
}

NV_STATUS
gvaspaceReservePageTableEntries_IMPL
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu,
    const NvU64  vaLo,
    const NvU64  vaHi,
    const NvU64  pageSizeMask
)
{
    GVAS_GPU_STATE *pGpuState;
    VA_RANGE_GPU   *pIter;
    VA_RANGE_GPU    newReservedPageTableEntry;

    pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    NV_ASSERT_OR_RETURN(NULL != pGpuState, NV_ERR_GENERIC);
    pIter = listHead(&pGpuState->reservedPageTableEntries);

    // Move past any entries before this
    while (pIter && pIter->vaLo < vaLo)
        pIter = listNext(&pGpuState->reservedPageTableEntries, pIter);

    // Insert this range and and reserve it
    newReservedPageTableEntry.vaLo = vaLo;
    newReservedPageTableEntry.vaHi = vaHi;
    NV_ASSERT_OR_RETURN(listInsertValue(&pGpuState->reservedPageTableEntries,
                                     pIter, &newReservedPageTableEntry),
                     NV_ERR_GENERIC);
    return _gvaspaceReservePageTableEntries(pGVAS, pGpu, vaLo, vaHi,
                                            pageSizeMask);
}

NV_STATUS
gvaspaceReleasePageTableEntries_IMPL
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu,
    const NvU64  vaLo,
    const NvU64  vaHi,
    const NvU64  pageSizeMask
)
{
    GVAS_GPU_STATE *pGpuState;
    VA_RANGE_GPU   *pIter;

    pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    NV_ASSERT_OR_RETURN(NULL != pGpuState, NV_ERR_GENERIC);
    pIter = listHead(&pGpuState->reservedPageTableEntries);

    while (pIter != NULL)
    {
        if ((pIter->vaLo == vaLo) && (pIter->vaHi == vaHi))
        {
            listRemove(&pGpuState->reservedPageTableEntries, pIter);
            return _gvaspaceReleasePageTableEntries(pGVAS, pGpu, vaLo, vaHi,
                                                    pageSizeMask);
        }

        pIter = listNext(&pGpuState->reservedPageTableEntries, pIter);
    }

    NV_PRINTF(LEVEL_ERROR, "Cannot find the reserved PTE to release.\n");

    return NV_ERR_GENERIC;
}

static NV_STATUS
_gvaspaceReservePageTableEntries
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu,
    const NvU64  vaLo,
    const NvU64  vaHi,
    const NvU64  pageSizeMask
)
{
    NV_STATUS         status = NV_OK;
    NvU32             pageShift;
    MMU_WALK_USER_CTX userCtx = {0};

    NV_ASSERT_OK_OR_RETURN(gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx));

    // Loop over each page size requested by client.
    FOR_EACH_INDEX_IN_MASK(64, pageShift, pageSizeMask)
    {
        // Pre-reserve page level instances in the VA range
        const MMU_FMT_LEVEL *pLevelFmt =
            mmuFmtFindLevelWithPageShift(
                    userCtx.pGpuState->pFmt->pRoot, pageShift);
        status = mmuWalkReserveEntries(userCtx.pGpuState->pWalk,
                                       pLevelFmt, vaLo, vaHi, NV_TRUE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to Reserve Entries.\n");
            break;
        }

        if (pGVAS->flags & VASPACE_FLAGS_BAR_BAR1)
        {
            status = mmuWalkSparsify(userCtx.pGpuState->pWalk, vaLo, vaHi, NV_FALSE);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Failed to sparsify reserved BAR1 page tables.\n");
                break;
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END

    gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);

    return status;
}

static NV_STATUS
_gvaspaceReleasePageTableEntries
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu,
    const NvU64  vaLo,
    const NvU64  vaHi,
    const NvU64  pageSizeMask
)
{
    GVAS_GPU_STATE *pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    NvU32 pageShift;

    NV_ASSERT(NULL != pGpuState);


    // Loop over each page size.
    FOR_EACH_INDEX_IN_MASK(64, pageShift, pageSizeMask)
    {
        // Release page level instances in the VA range.
        const MMU_FMT_LEVEL *pLevelFmt =
            mmuFmtFindLevelWithPageShift(pGpuState->pFmt->pRoot, pageShift);

        NV_ASSERT_OK_OR_RETURN(
                _gvaspaceReleaseUnreservedPTEs(pGVAS, pGpu, vaLo, vaHi,
                                               pLevelFmt));
    }
    FOR_EACH_INDEX_IN_MASK_END

    return NV_OK;
}

static NV_STATUS
_gvaspaceReleaseUnreservedPTEs
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu,
    const NvU64  vaLo,
    const NvU64  vaHi,
    const MMU_FMT_LEVEL *pLevelFmt
)
{
    NV_STATUS   status = NV_OK;

    MMU_WALK_USER_CTX userCtx = {0};
    GVAS_GPU_STATE   *pGpuState;
    VA_RANGE_GPU     *pIter;
    NvU64             piecewiseStart;
    NvU64             piecewiseEnd;

    pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    NV_ASSERT_OR_RETURN(NULL != pGpuState, NV_ERR_GENERIC);
    pIter = listHead(&pGpuState->reservedPageTableEntries);

    NV_ASSERT_OK_OR_RETURN(gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx));

    piecewiseStart = vaLo;
    while (piecewiseStart <= vaHi)
    {
        piecewiseEnd = 0;

        // If there is no reservation, free the whole range.
        if (!pIter)
            piecewiseEnd = vaHi;
        // If there is a reservation on this GPU, free the memory space
        // before its low address.
        else if (pIter->vaLo > piecewiseStart)
            piecewiseEnd = NV_MIN(vaHi,
                                  pIter->vaLo - 1);

        if (piecewiseEnd)
        {
            if (!(pGVAS->flags & VASPACE_FLAGS_BAR_BAR1))
            {
                // Clear out any mappings or sparse state.
                status = mmuWalkUnmap(userCtx.pGpuState->pWalk,
                                      piecewiseStart, piecewiseEnd);
                NV_ASSERT(NV_OK == status);
            }

            // Release page level instances in the VA range.
            status = mmuWalkReleaseEntries(userCtx.pGpuState->pWalk,
                                           pLevelFmt,
                                           piecewiseStart,
                                           piecewiseEnd);
            NV_ASSERT(NV_OK == status);

            piecewiseStart = piecewiseEnd + 1;
        }

        if (pIter)
        {
            /* Skip over the memory space of the reservation */
            piecewiseStart = NV_MAX(piecewiseStart, pIter->vaHi + 1);
            pIter = listNext(&pGpuState->reservedPageTableEntries, pIter);
        }
    }


    if (pGVAS->flags & VASPACE_FLAGS_FLA)
    {
        // free the dummy vas block
        _gvaspaceCleanupFlaDummyPagesForFlaRange(pGVAS, pGpu, userCtx.pGpuState);
    }

    gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);

    return status;
}

static NV_STATUS
_gvaspaceCopyServerRmReservedPdesToServerRm
(
    NvHandle                                              hClient,
    NvHandle                                              hVASpace,
    OBJGPU                                               *pGpu,
    NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pPdeCopyParams
)
{
    NV_STATUS    status = NV_OK;
    RmCtrlParams rmCtrlParams;

    portMemSet(&rmCtrlParams, 0, sizeof(RmCtrlParams));

    rmCtrlParams.hClient    = hClient;
    rmCtrlParams.hObject    = hVASpace;
    rmCtrlParams.cmd        = NV90F1_CTRL_CMD_VASPACE_COPY_SERVER_RESERVED_PDES;
    rmCtrlParams.pParams    = pPdeCopyParams;
    rmCtrlParams.paramsSize = sizeof(NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS);

    NV_RM_RPC_CONTROL(pGpu,
                      rmCtrlParams.hClient,
                      rmCtrlParams.hObject,
                      rmCtrlParams.cmd,
                      rmCtrlParams.pParams,
                      rmCtrlParams.paramsSize,
                      status);
    NV_ASSERT(NV_OK == status);

    return status;
}

static void
_gvaspaceForceFreePageLevelInstances
(
    OBJGVASPACE    *pGVAS,
    OBJGPU         *pGpu,
    GVAS_GPU_STATE *pGpuState
)
{
    VA_RANGE_GPU      *pIter   = NULL;
    MMU_WALK_USER_CTX  userCtx = {0};

    pIter = listHead(&(pGpuState->reservedPageTableEntries));
    while (NULL != pIter)
    {
        VA_RANGE_GPU *pNext;
        pNext = listNext(&(pGpuState->reservedPageTableEntries), pIter);
        listRemove(&(pGpuState->reservedPageTableEntries), pIter);
        pIter = pNext;
    }

    if (gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx) == NV_OK)
    {
        mmuWalkLevelInstancesForceFree(pGpuState->pWalk);
        gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
    }
}

static NV_STATUS
_gvaspacePopulatePDEentries
(
    OBJGVASPACE    *pGVAS,
    OBJGPU         *pGpu,
    NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pPdeCopyParams
)
{
    NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS pdeInfo;
    NvU32                                          i;
    NV_STATUS                                      status = NV_OK;

    portMemSet(&pdeInfo, 0, sizeof(NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS));
    portMemSet(pPdeCopyParams, 0, sizeof(NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS));

    // Populate the input params.
    pdeInfo.pageSize    = NVBIT64(GMMU_PD0_VADDR_BIT_LO);
    pdeInfo.virtAddress = pGVAS->vaStartServerRMOwned;

    // Fetch the details of the PDEs backing server RM's VA range.
    status = gvaspaceGetPageLevelInfo(pGVAS, pGpu, &pdeInfo);
    NV_ASSERT_OR_RETURN(NV_OK == status, status);

    // Populate the input params.
    for (i = 0; i < pdeInfo.numLevels; i++)
    {
        pPdeCopyParams->levels[i].pageShift   = pdeInfo.levels[i].levelFmt.virtAddrBitLo;
        pPdeCopyParams->levels[i].physAddress = pdeInfo.levels[i].physAddress;
        pPdeCopyParams->levels[i].aperture    = pdeInfo.levels[i].aperture;
        pPdeCopyParams->levels[i].size        = pdeInfo.levels[i].size;
    }

    pPdeCopyParams->numLevelsToCopy = pdeInfo.numLevels;
    pPdeCopyParams->subDeviceId     = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    pPdeCopyParams->pageSize        = NVBIT64(GMMU_PD0_VADDR_BIT_LO);
    pPdeCopyParams->virtAddrLo      = pGVAS->vaStartServerRMOwned;
    pPdeCopyParams->virtAddrHi      = pPdeCopyParams->virtAddrLo +
                                        SPLIT_VAS_SERVER_RM_MANAGED_VA_SIZE - 1;

    return status;
}


/*!
 * @brief Reserve mempool for page levels.
 *
 * @param[in] pGpu               OBJGPU pointer
 * @param[in] pClient            RsClient
 * @param[in] pVAS               Virtual address space
 * @param[in] size               Size of the mapping (assumes mapping is page aligned)
 * @param[in] pageSizeLockMask   Mask of page sizes locked down at VA reservation
 */
NV_STATUS
gvaspaceReserveMempool_IMPL
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu,
    Device      *pDevice,
    NvU64        size,
    NvU64        pageSizeLockMask,
    NvU32        flags
)
{
    NvBool          bRetryInSys = !!(pGVAS->flags & VASPACE_FLAGS_RETRY_PTE_ALLOC_IN_SYS);
    GVAS_GPU_STATE *pGpuState;
    KernelGmmu     *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    const GMMU_FMT *pFmt        = kgmmuFmtGet(pKernelGmmu, GMMU_FMT_VERSION_DEFAULT, 0);
    NV_STATUS       status;
    NvU64           poolSize;

    if ((pGVAS->flags & VASPACE_FLAGS_PTETABLE_PMA_MANAGED) == 0)
        return NV_OK;

    pGpuState   = gvaspaceGetGpuState(pGVAS, pGpu);
    if ((pGpuState == NULL) ||
        (pGpuState->pPageTableMemPool == NULL))
        return NV_OK;

    //
    // Always assume worst case of 4K mapping even if client has
    // requested bigger page size. This is to ensure that we have
    // sufficient memory in pools. Some MODS tests query for free
    // framebuffer and allocate the entire available. In such cases
    // we can run into OOM errors during page table allocation when
    // the test tries to map a big surface and the pools are short
    // of memory.
    //
    if (ONEBITSET(pageSizeLockMask))
    {
        //
        // There is a requirement of serial ATS enabled vaspaces to have
        // both small and big page tables allocated at the same time. This
        // is required for the 4K not valid feature. This is irrespective
        // of the actual page size requested by the client.
        //
        if (gvaspaceIsAtsEnabled(pGVAS))
        {
            pageSizeLockMask = RM_PAGE_SIZE | pGVAS->bigPageSize;
        }
        else if (!(flags & VASPACE_RESERVE_FLAGS_ALLOC_UPTO_TARGET_LEVEL_ONLY))
        {
            pageSizeLockMask = RM_PAGE_SIZE;
        }
    }
    else
    {
        NV_ASSERT_OR_RETURN(((pageSizeLockMask & RM_PAGE_SIZE) != 0),
                            NV_ERR_INVALID_ARGUMENT);
    }

    poolSize = kgmmuGetSizeOfPageDirs(pGpu, pKernelGmmu, pFmt, 0, size - 1, pageSizeLockMask) +
               kgmmuGetSizeOfPageTables(pGpu, pKernelGmmu, pFmt, 0, size - 1, pageSizeLockMask);

    status = rmMemPoolReserve(pGpuState->pPageTableMemPool, poolSize, pGVAS->flags);

    if ((status == NV_ERR_NO_MEMORY) && bRetryInSys)
    {
        //
        // It is okay to change the status to NV_OK here since it is understood that
        // we may run out of video memory at some time. The RETRY_PTE_ALLOC_IN_SYS
        // flag ensures that RM retries allocating the page tables in sysmem if such
        // a situation arises. So, running out of video memory here need not be fatal.
        // It may be fatal if allocation in sysmem also fails. In that case RM will
        // return an error from elsewhere.
        //
        status = NV_OK;
    }

    return status;
}

NV_STATUS
gvaspaceGetFreeHeap_IMPL
(
    OBJGVASPACE *pGVAS,
    NvU64       *pFreeSize
)
{
    NV_ASSERT_OR_RETURN(pFreeSize != NULL, NV_ERR_INVALID_ARGUMENT);

    return pGVAS->pHeap->eheapGetFree(pGVAS->pHeap, pFreeSize);
}

NvBool
gvaspaceIsInUse_IMPL
(
    OBJGVASPACE *pGVAS
)
{
    NvU64 freeSize  = 0;
    NvU64 totalSize = 0;

    // Get the free heap size.
    NV_ASSERT(gvaspaceGetFreeHeap(pGVAS, &freeSize) == NV_OK);

    // Get the total heap size for FLA vaspace.
    totalSize = vaspaceGetVaLimit(staticCast(pGVAS, OBJVASPACE)) -
                vaspaceGetVaStart(staticCast(pGVAS, OBJVASPACE)) + 1;

    return (totalSize != freeSize);
}

NV_STATUS
gvaspaceFreeV2_IMPL
(
    OBJGVASPACE *pGVAS,
    NvU64        vAddr,
    NvU64       *pSize
)
{
    EMEMBLOCK *pMemBlock = pGVAS->pHeap->eheapGetBlock(pGVAS->pHeap, vAddr, 0);
    NV_ASSERT_OR_RETURN(NULL != pMemBlock, NV_ERR_INVALID_ARGUMENT);

    *pSize = pMemBlock->end - pMemBlock->begin +1;
    return _gvaspaceInternalFree(pGVAS, vAddr, pMemBlock);
}
