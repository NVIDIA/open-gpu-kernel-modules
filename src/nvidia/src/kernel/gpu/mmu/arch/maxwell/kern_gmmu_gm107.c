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

#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/mmu/kern_gmmu.h"
#include "gpu/bus/kern_bus.h"
#include "vgpu/rpc.h"

#include "published/maxwell/gm107/dev_fb.h"
#include "published/maxwell/gm107/dev_mmu.h"

/*!
 * Returns the big page size in bytes
 *
 * @param[in]  pKernelGmmu    KernelGmmu object pointer
 *
 * @returns NvU32
 */
NvU64
kgmmuGetBigPageSize_GM107(KernelGmmu *pKernelGmmu)
{
    return pKernelGmmu->defaultBigPageSize;
}

NV_STATUS
kgmmuCommitInvalidateTlbTest_GM107
(
    OBJGPU                              *pGpu,
    KernelGmmu                          *pKernelGmmu,
    COMMIT_TLB_INVALIDATE_TEST_PARAMS   *pTestParams
)
{
    TLB_INVALIDATE_PARAMS params;
    NvU32 regVal = 0;

    if (pTestParams->invalidateAll != NV_TRUE)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    portMemSet(&params, 0, sizeof(TLB_INVALIDATE_PARAMS));
    params.gfid = pTestParams->gfid;
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &params.timeout,
                  GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE |
                  GPU_TIMEOUT_FLAGS_DEFAULT | GPU_TIMEOUT_FLAGS_BYPASS_CPU_YIELD);

    NV_ASSERT_OK_OR_RETURN(kgmmuCheckPendingInvalidates_HAL(pGpu, pKernelGmmu,
                                                            &params.timeout));

    // Invalidate all VA and PDB
    regVal = DRF_DEF(_PFB_PRI, _MMU_INVALIDATE, _ALL_VA, _TRUE) |
             DRF_DEF(_PFB_PRI, _MMU_INVALIDATE, _ALL_PDB, _TRUE) |
             DRF_DEF(_PFB_PRI, _MMU_INVALIDATE, _TRIGGER, _TRUE);

    params.regVal = regVal;

    return kgmmuCommitTlbInvalidate_HAL(pGpu, pKernelGmmu, &params);
}

/*!
 * @brief Unicast GMMU TLB invalidate.
 *
 * Sequence:
 * 1. Acquire mutex for PMU ELPG (exclusive PRI_FIFO access).
 * 2. Wait for available PRI_FIFO space.
 * 3. Commit invalidate.
 * 4. Wait for PRI_FIFO to drain.
 * 5. Release mutex for PMU ELPG.
 *
 * @param[in] pGpu
 * @param[in] pKernelGmmu
 * @param[in] pRootPageDir       Memory descriptor of the PDB
 * @param[in] bHubOnly           Only HUB TLB needs to be invalidated
 * @param[in] update_type        PTE_DOWNGRADE if coherency is required following
 *                               invalidate. Unused in FERMI.
 * @param[in] gfid               GFID of the VF whose mappings are to be
 *                               invalidated. Unused on pre Turing
 * @param[in] invalidation_scope invalidation scope to choose whether to invalidate
 *                               Link TLB or Non-Link TLBs or ALL TLBs
 */
void
kgmmuInvalidateTlb_GM107
(
    OBJGPU              *pGpu,
    KernelGmmu          *pKernelGmmu,
    MEMORY_DESCRIPTOR   *pRootPageDir,
    NvU32                vaspaceFlags,
    VAS_PTE_UPDATE_TYPE  update_type,
    NvU32                gfid,
    NvU32                invalidation_scope
)
{
    NV_STATUS             status         = NV_OK;
    TLB_INVALIDATE_PARAMS params;
    NvU32                 flushCount     = 0;
    NvBool                bDoVgpuRpc     = NV_FALSE;
    OBJVGPU              *pVgpu          = NULL;

    //
    // Bail out early if
    // 1. The GPU is in reset path.
    // 2. We are running inside a guest in paravirtualization config.
    //
    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu) ||
        IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return;
    }

    // Clear struct before use.
    portMemSet(&params, 0, sizeof(TLB_INVALIDATE_PARAMS));

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    // Skip BAR invalidates if regkey is set for RTL/FMOD.
    if ((vaspaceFlags & VASPACE_FLAGS_BAR)
        && kgmmuIsIgnoreHubTlbInvalidate(pKernelGmmu))
    {
        NV_PRINTF(LEVEL_INFO,
                  "disable_mmu_invalidate flag, skipping hub invalidate.\n");
        return;
    }

    pVgpu = GPU_GET_VGPU(pGpu);
    if (pVgpu && pVgpu->bGspBuffersInitialized)
    {
        VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

        if  (pVSI &&
             FLD_TEST_DRF(A080, _CTRL_CMD_VGPU_GET_CONFIG,
                          _PARAMS_VGPU_DEV_CAPS_VF_INVALIDATE_TLB_TRAP_ENABLED,
                          _TRUE, pVSI->vgpuConfig.vgpuDeviceCapsBits))
            bDoVgpuRpc = NV_TRUE;
    }

    if (!bDoVgpuRpc)
    {
        //
        // Originally the flag is 0, but to WAR bug 2909388, add flag
        // GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE to bypass using threadStateCheckTimeout,
        // GPU_TIMEOUT_FLAGS_BYPASS_CPU_YIELD to not wait inside timeout with mutex held.
        //
        gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &params.timeout,
                      GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE |
                      GPU_TIMEOUT_FLAGS_DEFAULT | GPU_TIMEOUT_FLAGS_BYPASS_CPU_YIELD);

        //
        // 2. Wait until we can issue an invalidate. On pre-Turing, wait for space
        // in the PRI FIFO. On Turing, check if an invalidate is already in progress.
        //
        // Set the GFID.
        params.gfid = gfid;

        status = kgmmuCheckPendingInvalidates_HAL(pGpu, pKernelGmmu, &params.timeout);
        if (status != NV_OK)
        {
           return;
        }
    }

    // Trigger an invalidate.
    params.regVal = FLD_SET_DRF(_PFB_PRI, _MMU_INVALIDATE, _TRIGGER, _TRUE, params.regVal);

    // Not using range-based invalidate.
    params.regVal = FLD_SET_DRF(_PFB_PRI, _MMU_INVALIDATE, _ALL_VA, _TRUE, params.regVal);

    if (NULL != pRootPageDir)
    {
        // Invalidatating only one VAS.
        params.regVal = FLD_SET_DRF(_PFB_PRI, _MMU_INVALIDATE, _ALL_PDB, _FALSE, params.regVal);

        // Setup PDB of VAS to invalidate.
        if (memdescGetAddressSpace(pRootPageDir) == ADDR_FBMEM)
        {
            params.pdbAperture = NV_PFB_PRI_MMU_INVALIDATE_PDB_APERTURE_VID_MEM;
        }
        else if (memdescGetAddressSpace(pRootPageDir) == ADDR_SYSMEM)
        {
            params.pdbAperture = NV_PFB_PRI_MMU_INVALIDATE_PDB_APERTURE_SYS_MEM;
        }
        else
        {
            return;
        }

        params.pdbAddress = memdescGetPhysAddr(memdescGetMemDescFromGpu(pRootPageDir, pGpu), AT_GPU, 0);
    }
    else
    {
        // Invalidate *ALL* address spaces.
        params.regVal = FLD_SET_DRF(_PFB_PRI, _MMU_INVALIDATE, _ALL_PDB, _TRUE, params.regVal);
        // Override invalidation scope.
        invalidation_scope = NV_GMMU_INVAL_SCOPE_ALL_TLBS;
    }

    // For host VAS (e.g. BAR) we do not have to invalidate GR.
    if (vaspaceFlags & VASPACE_FLAGS_BAR)
    {
        params.regVal = FLD_SET_DRF(_PFB_PRI, _MMU_INVALIDATE, _HUBTLB_ONLY, _TRUE, params.regVal);
    }

    // Perform membarWAR for non-BAR2 pte downgrades.
    if (!(vaspaceFlags & VASPACE_FLAGS_BAR_BAR2) && (PTE_DOWNGRADE == update_type))
    {
        flushCount = kgmmuSetTlbInvalidateMembarWarParameters_HAL(pGpu, pKernelGmmu, &params);
    }

    status = kgmmuSetTlbInvalidationScope_HAL(pGpu, pKernelGmmu, invalidation_scope, &params);
    if (!(status == NV_OK || status == NV_ERR_NOT_SUPPORTED))
        return;

    if (bDoVgpuRpc)
    {
        NV_RM_RPC_INVALIDATE_TLB(pGpu, status, params.pdbAddress, params.regVal);
        if (status != NV_OK)
        {
            return;
        }
    }
    else
    {
        // 3 and 4. Commit the invalidate and wait for invalidate to complete.
        status = kgmmuCommitTlbInvalidate_HAL(pGpu, pKernelGmmu, &params);
        if (status != NV_OK)
        {
            return;
        }
    }

    while (flushCount--)
    {
        if (kbusSendSysmembar(pGpu, GPU_GET_KERNEL_BUS(pGpu)) == NV_ERR_TIMEOUT)
        {
            break;
        }
    }
}

/*!
 * Check if a specific GMMU format version is supported.
 */
NvBool
kgmmuFmtIsVersionSupported_GM10X(KernelGmmu *pKernelGmmu, NvU32 version)
{
    return (version == GMMU_FMT_VERSION_1);
}

void
kgmmuDetermineMaxVASize_GM107
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    NvU32       v;
    NvU32       maxFmtVersionSupported = 0;

    for (v = 0; v < GMMU_FMT_MAX_VERSION_COUNT; ++v)
    {
        const NvU32 ver = g_gmmuFmtVersions[v];
        if (kgmmuFmtIsVersionSupported_HAL(pKernelGmmu, ver))
        {
            maxFmtVersionSupported = maxFmtVersionSupported < ver ? ver : maxFmtVersionSupported;
        }
    }

    switch (maxFmtVersionSupported)
    {
        case GMMU_FMT_VERSION_1:
            pKernelGmmu->maxVASize = 1ULL << 40;
        break;
        case GMMU_FMT_VERSION_2:
            pKernelGmmu->maxVASize = 1ULL << 49;
        break;
        case GMMU_FMT_VERSION_3:
            pKernelGmmu->maxVASize = 1ULL << 57;
        break;
        default:
            pKernelGmmu->maxVASize = 1ULL << 40;
    }
}

/*
 * @brief Checks the system memory address against the PA capabilities of the
 *        GMMU.
 */
void
kgmmuEncodeSysmemAddrs_GM107
(
    KernelGmmu *pKernelGmmu,
    NvU64      *pAddresses,
    NvU64       count
)
{
    OBJGPU     *pGpu    = ENG_GET_GPU(pKernelGmmu);
    const NvU32 paWidth = gpuGetPhysAddrWidth_HAL(pGpu, ADDR_SYSMEM);
    NvU64 i;

    for (i = 0; i < count; ++i)
    {
        NvU64 address = pAddresses[i];

        NV_ASSERT(address <= (NVBIT64(paWidth) - 1));
    }
}

/*!
 * @brief This function returns the largest page size
 * that is supported by the system.
 * On GM2xx, big page size is specific to VASpace and is not decided
 * globally (provided the property is set).
 * In such a case, all FB allocations need to be aligned to this page
 * size so that this surface can be mapped to both 64k and 128k page sizes.
 *
 * @param[in] pKernelGmmu
 *
 * @returns    The size of a large page in bytes
 */
NvU64
kgmmuGetMaxBigPageSize_GM107(KernelGmmu *pKernelGmmu)
{
    if (!kgmmuIsPerVaspaceBigPageEn(pKernelGmmu))
        return kgmmuGetBigPageSize_HAL(pKernelGmmu);

    return RM_PAGE_SIZE_128K;
}

NvU8
kgmmuGetHwPteApertureFromMemdesc_GM107
(
    KernelGmmu        *pGmmu,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_ADDRESS_SPACE addrSpace = memdescGetAddressSpace(pMemDesc);
    NvU8             aperture  = 0;

    switch (addrSpace)
    {
        case ADDR_SYSMEM:
            if (memdescGetCpuCacheAttrib(pMemDesc) == NV_MEMORY_CACHED)
            {
                aperture = NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY;
            }
            else
            {
                aperture = NV_MMU_PTE_APERTURE_SYSTEM_NON_COHERENT_MEMORY;
            }
            break;
        case ADDR_FABRIC_V2:
        case ADDR_FABRIC_MC:
        case ADDR_EGM:
            aperture = NV_MMU_PTE_APERTURE_PEER_MEMORY;
            break;
        case ADDR_FBMEM:
            aperture = NV_MMU_PTE_APERTURE_VIDEO_MEMORY;
            break;
        default:
            //
            // If we reach here, then the addrSpace is not valid
            // and we should assert here.
            //
            NV_ASSERT(0);
    }
    return aperture;
}
