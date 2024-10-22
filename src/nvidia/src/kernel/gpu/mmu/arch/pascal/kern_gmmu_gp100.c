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
#include "gpu/gpu.h"

#include "gpu/mem_mgr/fermi_dma.h"

#include "published/pascal/gp100/dev_fb.h"
#include "published/pascal/gp100/dev_ram.h"
#include "published/pascal/gp100/dev_fault.h"

/*!
 * Check if a specific GMMU format version is supported.
 */
NvBool
kgmmuFmtIsVersionSupported_GP10X(KernelGmmu *pKernelGmmu, NvU32 version)
{
    return (version == GMMU_FMT_VERSION_2);
}

/*!
 * @brief Get the size of PDB allocation for 5-level page table formats
 *
 * Because GMMU can prefetch uninitialized PDB entries and cause XVE to hang,
 * we need to allocate all entries of the PDB regardless of vaLimit.
 *
 * @param[in] pKernelGmmu
 * @param[in] pLevelFmt   Level format of the PDB. Must be root level.
 * @param[in] vaLimit     VA limit that needs to be covered
 *
 */
NvU32
kgmmuGetPDBAllocSize_GP100
(
    KernelGmmu          *pKernelGmmu,
    const MMU_FMT_LEVEL *pLevelFmt,
    NvU64                vaLimit
)
{
    return mmuFmtLevelSize(pLevelFmt);
}

/*!
 * @brief This gets the offset and data for vaLimit
 *
 * @param[in]  pKernelGmmu
 * @param[in]  pVAS            OBJVASPACE pointer
 * @param[in]  subctxId        subctxId value
 * @param[in]  pParams         Pointer to the structure containing parameters passed by the engine
 * @param[out] pOffset         Pointer to offset of NV_RAMIN_ADR_LIMIT_LO:NV_RAMIN_ADR_LIMIT_HI pair
 * @param[out] pData           Pointer to value to write
 *
 * @returns NV_STATUS
 */
NV_STATUS
kgmmuInstBlkVaLimitGet_GP100
(
    KernelGmmu           *pKernelGmmu,
    OBJVASPACE           *pVAS,
    NvU32                 subctxId,
    INST_BLK_INIT_PARAMS *pParams,
    NvU32                *pOffset,
    NvU64                *pData
)
{
    ct_assert(SF_OFFSET(NV_RAMIN_ADR_LIMIT_HI) == SF_OFFSET(NV_RAMIN_ADR_LIMIT_LO) + 4);
    ct_assert((SF_OFFSET(NV_RAMIN_ADR_LIMIT_LO) & 7) == 0);

    NvU64     vaLimit;
    NvU64     vaMask = ((NvU64)SF_SHIFTMASK(NV_RAMIN_ADR_LIMIT_HI) << 32) |
                        SF_SHIFTMASK(NV_RAMIN_ADR_LIMIT_LO);

    // Only legacy pdb is valid in instance block.
    NV_ASSERT(subctxId == FIFO_PDB_IDX_BASE);
    NV_ASSERT_OR_RETURN(pVAS, NV_ERR_INVALID_ARGUMENT);

    vaLimit  = vaspaceGetVaLimit(pVAS) - pParams->uvmKernelPrivRegion;
    *pData   = (vaLimit & vaMask) | 0xfff;
    *pOffset = SF_OFFSET(NV_RAMIN_ADR_LIMIT_LO);

    return NV_OK;
}

/*!
 * @brief This gets the offsets and data for the PDB limit
 *
 * @param[in] pGpu
 * @param[in] pKernelGmmu
 * @param[in] pVAS            OBJVASPACE pointer
 * @param[in] pParams         Pointer to the structure containing parameters passed by the engine
 * @param[in] subctxId        subctxId value
 * @param[out] pOffsetLo      Pointer to low offset
 * @param[out] pDataLo        Pointer to data written at above offset
 * @param[out] pOffsetHi      Pointer to high offset
 * @param[out] pDataHi        Pointer to data written at above offset
 *
 * @returns
 */
NV_STATUS
kgmmuInstBlkPageDirBaseGet_GP100
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu,
    OBJVASPACE           *pVAS,
    INST_BLK_INIT_PARAMS *pParams,
    NvU32                 subctxId,
    NvU32                *pOffsetLo,
    NvU32                *pDataLo,
    NvU32                *pOffsetHi,
    NvU32                *pDataHi
)
{

    NV_ASSERT(subctxId == FIFO_PDB_IDX_BASE);

    if (pVAS != NULL)
    {
        KernelGmmu         *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
        OBJGVASPACE        *pGVAS       = dynamicCast(pVAS, OBJGVASPACE);
        const GMMU_FMT     *pFmt        = gvaspaceGetGmmuFmt(pGVAS, pGpu);
        PMEMORY_DESCRIPTOR pPDB;
        RmPhysAddr         physAdd;
        NvU32              aperture;
        NvU32              addrLo;
        NvU64              bigPageSize = vaspaceGetBigPageSize(pVAS);

        pPDB = (pParams->bIsClientAdmin) ? vaspaceGetKernelPageDirBase(pVAS, pGpu) :
            vaspaceGetPageDirBase(pVAS, pGpu);
        NV_ASSERT_OR_RETURN(pPDB != NULL, NV_ERR_INVALID_STATE);

        physAdd  = memdescGetPhysAddr(pPDB, AT_GPU, 0);
        aperture = kgmmuGetHwPteApertureFromMemdesc(pKernelGmmu, pPDB);
        addrLo   = NvU64_LO32(physAdd >> NV_RAMIN_BASE_SHIFT);

        *pOffsetLo = SF_OFFSET(NV_RAMIN_PAGE_DIR_BASE_LO);
        *pDataLo   = SF_NUM(_RAMIN_PAGE_DIR_BASE, _TARGET, aperture) |
                        ((pParams->bIsFaultReplayable) ?
                            SF_DEF(_RAMIN_PAGE_DIR_BASE, _FAULT_REPLAY_TEX, _ENABLED) |
                            SF_DEF(_RAMIN_PAGE_DIR_BASE, _FAULT_REPLAY_GCC, _ENABLED) :
                            SF_DEF(_RAMIN_PAGE_DIR_BASE, _FAULT_REPLAY_TEX, _DISABLED) |
                            SF_DEF(_RAMIN_PAGE_DIR_BASE, _FAULT_REPLAY_GCC, _DISABLED)) |
                        ((NULL != pFmt) && (GMMU_FMT_VERSION_2 == pFmt->version) ?
                            SF_DEF(_RAMIN, _USE_NEW_PT_FORMAT, _TRUE) :
                            SF_DEF(_RAMIN, _USE_NEW_PT_FORMAT, _FALSE)) |
                        ((bigPageSize == FERMI_BIG_PAGESIZE_64K) ?
                            SF_DEF(_RAMIN, _BIG_PAGE_SIZE, _64KB) :
                            SF_DEF(_RAMIN, _BIG_PAGE_SIZE, _128KB)) |
                        SF_NUM(_RAMIN_PAGE_DIR_BASE, _VOL, memdescGetVolatility(pPDB)) |
                        SF_NUM(_RAMIN_PAGE_DIR_BASE, _LO, addrLo);

        *pOffsetHi = SF_OFFSET(NV_RAMIN_PAGE_DIR_BASE_HI);
        *pDataHi   = SF_NUM(_RAMIN_PAGE_DIR_BASE, _HI, NvU64_HI32(physAdd));
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "A channel must have a pVAS if it does not support TSG sub context!\n");
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_PARAMETER;
    }

    return NV_OK;
}


/**
 * @brief Converts a MMU fault type (NV_PFAULT_FAULT_TYPE_*) into a string.
 *
 * @param[in] faultType NV_PFAULT_FAULT_TYPE_*
 *
 * @returns a string (always non-null)
 */
const char *
kgmmuGetFaultTypeString_GP100(KernelGmmu *pGmmu, NvU32 faultType)
{
    switch (faultType)
    {
        case NV_PFAULT_FAULT_TYPE_PDE:
            return "FAULT_PDE";
        case NV_PFAULT_FAULT_TYPE_PDE_SIZE:
            return "FAULT_PDE_SIZE";
        case NV_PFAULT_FAULT_TYPE_PTE:
            return "FAULT_PTE";
        case NV_PFAULT_FAULT_TYPE_VA_LIMIT_VIOLATION:
            return "FAULT_VA_LIMIT_VIOLATION";
        case NV_PFAULT_FAULT_TYPE_UNBOUND_INST_BLOCK:
            return "FAULT_UNBOUND_INST_BLOCK";
        case NV_PFAULT_FAULT_TYPE_PRIV_VIOLATION:
            return "FAULT_PRIV_VIOLATION";
        case NV_PFAULT_FAULT_TYPE_RO_VIOLATION:
            return "FAULT_RO_VIOLATION";
        case NV_PFAULT_FAULT_TYPE_PITCH_MASK_VIOLATION:
            return "FAULT_PITCH_MASK_VIOLATION";
        case NV_PFAULT_FAULT_TYPE_WORK_CREATION:
            return "FAULT_WORK_CREATION";
        case NV_PFAULT_FAULT_TYPE_UNSUPPORTED_APERTURE:
            return "FAULT_UNSUPPORTED_APERTURE";
        case NV_PFAULT_FAULT_TYPE_COMPRESSION_FAILURE:
            return "FAULT_COMPRESSION_FAILURE";
        case NV_PFAULT_FAULT_TYPE_UNSUPPORTED_KIND:
            return "FAULT_INFO_TYPE_UNSUPPORTED_KIND";
        case NV_PFAULT_FAULT_TYPE_REGION_VIOLATION:
            return "FAULT_INFO_TYPE_REGION_VIOLATION";
        case NV_PFAULT_FAULT_TYPE_POISONED:
            return "FAULT_INFO_TYPE_POISONED";
        case NV_PFAULT_FAULT_TYPE_ATOMIC_VIOLATION:
            return "FAULT_INFO_TYPE_ATOMIC_VIOLATION";
        default:
            return "UNRECOGNIZED_FAULT";
    }
}

NV_STATUS
kgmmuFaultCancelIssueInvalidate_GP100
(
    OBJGPU                 *pGpu,
    KernelGmmu             *pKernelGmmu,
    GMMU_FAULT_CANCEL_INFO *pCancelInfo,
    TLB_INVALIDATE_PARAMS  *pParams,
    NvBool                  bGlobal
)
{
    NvU32 data32 = 0;
    data32 = FLD_SET_DRF_NUM(_PFB, _PRI_MMU_INVALIDATE, _ALL_VA,
        NV_PFB_PRI_MMU_INVALIDATE_ALL_VA_TRUE, data32);

    data32 = FLD_SET_DRF_NUM(_PFB, _PRI_MMU_INVALIDATE, _ALL_PDB,
        NV_PFB_PRI_MMU_INVALIDATE_ALL_PDB_TRUE, data32);

    data32 = FLD_SET_DRF_NUM(_PFB, _PRI_MMU_INVALIDATE, _HUBTLB_ONLY,
        NV_PFB_PRI_MMU_INVALIDATE_HUBTLB_ONLY_FALSE, data32);

    data32 = FLD_SET_DRF_NUM(_PFB, _PRI_MMU_INVALIDATE, _SYS_MEMBAR,
        NV_PFB_PRI_MMU_INVALIDATE_SYS_MEMBAR_FALSE, data32);

    data32 = FLD_SET_DRF_NUM(_PFB, _PRI_MMU_INVALIDATE, _ACK,
        NV_PFB_PRI_MMU_INVALIDATE_ACK_NONE_REQUIRED, data32);

    data32 = FLD_SET_DRF_NUM(_PFB, _PRI_MMU_INVALIDATE, _CACHE_LEVEL,
        NV_PFB_PRI_MMU_INVALIDATE_CACHE_LEVEL_ALL, data32);

    data32 = FLD_SET_DRF_NUM(_PFB, _PRI_MMU_INVALIDATE, _TRIGGER,
        NV_PFB_PRI_MMU_INVALIDATE_TRIGGER_TRUE, data32);

    if (bGlobal)
    {
        data32 = FLD_SET_DRF_NUM(_PFB, _PRI_MMU_INVALIDATE, _REPLAY,
            NV_PFB_PRI_MMU_INVALIDATE_REPLAY_CANCEL_GLOBAL, data32);
    }
    else
    {
        data32 = FLD_SET_DRF_NUM(_PFB, _PRI_MMU_INVALIDATE, _REPLAY,
            NV_PFB_PRI_MMU_INVALIDATE_REPLAY_CANCEL_TARGETED, data32);

        data32 = FLD_SET_DRF_NUM(_PFB, _PRI_MMU_INVALIDATE, _CANCEL_CLIENT_ID,
            pCancelInfo->clientId, data32);

        data32 = FLD_SET_DRF_NUM(_PFB, _PRI_MMU_INVALIDATE, _CANCEL_GPC_ID,
            pCancelInfo->gpcId, data32);

        data32 = FLD_SET_DRF_NUM(_PFB, _PRI_MMU_INVALIDATE, _CANCEL_CLIENT_TYPE,
            NV_PFB_PRI_MMU_INVALIDATE_CANCEL_CLIENT_TYPE_GPC, data32);
    }

    pParams->regVal = data32;

    NV_ASSERT_OK_OR_RETURN(kgmmuCheckPendingInvalidates_HAL(pGpu, pKernelGmmu, &pParams->timeout));

    NV_ASSERT_OK_OR_RETURN(kgmmuCommitTlbInvalidate_HAL(pGpu, pKernelGmmu, pParams));

    return NV_OK;
}

/*!
 * On Pascal+, the big page size is 64KB. The function checks and captures
 * incorrect registry overriding.
 *
 * @param[in] pGpu
 * @param[in] pGmmu
 *
 */
NV_STATUS
kgmmuCheckAndDecideBigPageSize_GP100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    NvU64 bigPageSizeOverride = kgmmuGetBigPageSizeOverride(pKernelGmmu);

    if (bigPageSizeOverride)
    {
        // Pascal+ does not support 128kB page size
        NV_ASSERT_OR_RETURN(bigPageSizeOverride == RM_PAGE_SIZE_64K, NV_ERR_NOT_SUPPORTED);
    }

    return NV_OK;
}
