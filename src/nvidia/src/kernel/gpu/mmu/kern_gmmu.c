/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

 /******************************************************************************
*
*       Kernel GMMU module header
*       Defines and structures used on CPU RM for the GMMU object.
*
******************************************************************************/

#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/bif/kernel_bif.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/nvlink/kernel_nvlink.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "vgpu/vgpu_events.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "os/os.h"
#include "rmapi/rmapi.h"
#include "gpu/gpu.h"
#include "nvRmReg.h"
#include "vgpu/rpc.h"
#include "kernel/gpu/intr/engine_idx.h"


static void _kgmmuInitRegistryOverrides(OBJGPU *pGpu, KernelGmmu *pKernelGmmu);

/*!
 * KERNEL_GMMU constructor
 *
 * @param[in]  pGpu
 * @param[in]  pKernelGmmu
 * @param[in]  engDesc       Engine descriptor
 *
 * @return NV_OK on success, pertinent error code on failure.
 */
NV_STATUS
kgmmuConstructEngine_IMPL(OBJGPU *pGpu, KernelGmmu *pKernelGmmu, ENGDESCRIPTOR engDesc)
{
    NvU32  v;

    kgmmuDetermineMaxVASize_HAL(pGpu, pKernelGmmu);

    if (gpuIsCacheOnlyModeEnabled(pGpu))
    {
        pKernelGmmu->bHugePageSupported      = NV_FALSE;
        pKernelGmmu->bPageSize512mbSupported = NV_FALSE;
    }

    // Allocate and init MMU format families.
    kgmmuFmtInitPdeApertures_HAL(pKernelGmmu, pKernelGmmu->pdeApertures);
    kgmmuFmtInitPteApertures_HAL(pKernelGmmu, pKernelGmmu->pteApertures);

    for (v = 0; v < GMMU_FMT_MAX_VERSION_COUNT; ++v)
    {
        const NvU32 ver = g_gmmuFmtVersions[v];
        if (kgmmuFmtIsVersionSupported_HAL(pKernelGmmu, ver))
        {
            GMMU_FMT_FAMILY *pFam = NULL;

            // Alloc version struct.
            pFam = portMemAllocNonPaged(sizeof(*pFam));
            NV_ASSERT_OR_RETURN((pFam != NULL), NV_ERR_NO_MEMORY);
            portMemSet(pFam, 0, sizeof(*pFam));
            pKernelGmmu->pFmtFamilies[v] = pFam;

            // Init PDE/PTE formats.
            kgmmuFmtInitPdeMulti_HAL(pKernelGmmu, &pFam->pdeMulti, ver, pKernelGmmu->pdeApertures);
            kgmmuFmtInitPde_HAL(pKernelGmmu, &pFam->pde, ver, pKernelGmmu->pdeApertures);
            kgmmuFmtInitPte_HAL(pKernelGmmu, &pFam->pte, ver, pKernelGmmu->pteApertures,
                gpuIsUnifiedMemorySpaceEnabled(pGpu));

            kgmmuFmtInitPteComptagLine_HAL(pKernelGmmu, &pFam->pte, ver);
        }
        else
        {
            pKernelGmmu->pFmtFamilies[v] = NULL;
        }
    }

    NV_ASSERT_OK_OR_RETURN(kgmmuFmtInit(pKernelGmmu));

    portMemSet(&pKernelGmmu->mmuFaultBuffer, 0, sizeof(pKernelGmmu->mmuFaultBuffer));

    // Default placement for PDEs is in vidmem.
    pKernelGmmu->PDEAperture = ADDR_FBMEM;
    pKernelGmmu->PDEAttr = NV_MEMORY_WRITECOMBINED;
    pKernelGmmu->PDEBAR1Aperture = ADDR_FBMEM;
    pKernelGmmu->PDEBAR1Attr = NV_MEMORY_WRITECOMBINED;

    // Default placement for PTEs is in vidmem.
    pKernelGmmu->PTEAperture = ADDR_FBMEM;
    pKernelGmmu->PTEAttr = NV_MEMORY_WRITECOMBINED;
    pKernelGmmu->PTEBAR1Aperture = ADDR_FBMEM;
    pKernelGmmu->PTEBAR1Attr = NV_MEMORY_WRITECOMBINED;

    _kgmmuInitRegistryOverrides(pGpu, pKernelGmmu);

    return NV_OK;
}

static NV_STATUS
_kgmmuInitStaticInfo
(
    OBJGPU *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *pStaticInfo;
    NV_STATUS status;

    //
    // On vGPU, all hardware management is done by the host except for full SR-IOV.
    // Thus, only do any further HW initialization on the host.
    //
    if (!(IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
          (IS_VIRTUAL_WITH_SRIOV(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu))))
    {
        // Init HAL specific features.
        NV_ASSERT_OK_OR_RETURN(kgmmuFmtFamiliesInit_HAL(pGpu, pKernelGmmu));
    }

    pStaticInfo = portMemAllocNonPaged(sizeof(*pStaticInfo));
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pStaticInfo != NULL, NV_ERR_INSUFFICIENT_RESOURCES);
    portMemSet(pStaticInfo, 0, sizeof(*pStaticInfo));

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        kgmmuInitStaticInfo_HAL(pGpu, pKernelGmmu, pStaticInfo),
        fail);

    pKernelGmmu->pStaticInfo = pStaticInfo;

fail:
    if (status != NV_OK)
    {
        portMemFree(pStaticInfo);
    }

    return status;
}

/*
 * Initialize the Kernel GMMU state.
 *
 * @param      pGpu
 * @param      pKernelGmmu
 */
NV_STATUS kgmmuStateInitLocked_IMPL
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    NV_STATUS  status;

    if (pKernelBif != NULL)
    {
        // This value shouldn't change after initialization, so cache it now
        pKernelGmmu->sysmemBaseAddress = pKernelBif->dmaWindowStartAddress;
    }

    status = _kgmmuInitStaticInfo(pGpu, pKernelGmmu);

    return status;
}

static NV_STATUS
_kgmmuCreateGlobalVASpace
(
    POBJGPU  pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32 flags
)
{
    NvU32       constructFlags = VASPACE_FLAGS_NONE;
    POBJVASPACE pGlobalVAS     = NULL;
    NV_STATUS   rmStatus;
    POBJGPUGRP  pGpuGrp        = NULL;

    // Bail out early on sleep/suspend cases
    if (flags & GPU_STATE_FLAGS_PRESERVING)
        return NV_OK;
    if (!gpumgrIsParentGPU(pGpu))
        return NV_OK;

    //
    // We create the device vaspace at this point. Assemble the flags needed
    // for construction.
    //

    // Allow PTE in SYS
    constructFlags |= VASPACE_FLAGS_RETRY_PTE_ALLOC_IN_SYS;

    constructFlags |= VASPACE_FLAGS_DEFAULT_PARAMS;
    constructFlags |= VASPACE_FLAGS_DEFAULT_SIZE;
    constructFlags |= DRF_DEF(_VASPACE, _FLAGS, _BIG_PAGE_SIZE, _DEFAULT);

    pGpuGrp = gpumgrGetGpuGrpFromGpu(pGpu);
    NV_ASSERT_OR_RETURN(pGpuGrp != NULL, NV_ERR_INVALID_DATA);

    rmStatus = gpugrpCreateGlobalVASpace(pGpuGrp, pGpu,
                                         FERMI_VASPACE_A,
                                         0, 0,
                                         constructFlags,
                                         &pGlobalVAS);
    NV_ASSERT_OR_RETURN((NV_OK == rmStatus), rmStatus);

    return NV_OK;
}

static NV_STATUS
_kgmmuDestroyGlobalVASpace
(
    POBJGPU  pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32 flags
)
{
    POBJGPUGRP pGpuGrp = NULL;

    if (flags & GPU_STATE_FLAGS_PRESERVING)
        return NV_OK;

    pGpuGrp = gpumgrGetGpuGrpFromGpu(pGpu);
    return gpugrpDestroyGlobalVASpace(pGpuGrp, pGpu);
}

/*
 *  Helper function to enable ComputePeerMode
 */
NV_STATUS
kgmmuEnableComputePeerAddressing_IMPL
(
    OBJGPU *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32 flags
)
{
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    NV_STATUS status = NV_OK;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvBool bComputePeerMode = NV_FALSE;

    if (pSys->getProperty(pSys, PDB_PROP_SYS_NVSWITCH_IS_PRESENT) ||
        kbusIsFlaSupported(pKernelBus))
    {
        bComputePeerMode = NV_TRUE;
    }

    if (bComputePeerMode)
    {
        status = kgmmuEnableNvlinkComputePeerAddressing_HAL(pKernelGmmu);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                        "Failed to enable GMMU property compute addressing for GPU %x , status:%x\n",
                        pGpu->gpuInstance, status);
            return status;
        }

        status = pRmApi->Control(pRmApi,
                                pGpu->hInternalClient,
                                pGpu->hInternalSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_NVLINK_ENABLE_COMPUTE_PEER_ADDR,
                                NULL, 0);
    }
    return status;
}

/*
 *  State Post Load
 */
NV_STATUS kgmmuStatePostLoad_IMPL
(
    OBJGPU *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32 flags
)
{
    NV_STATUS status = NV_OK;

    status = _kgmmuCreateGlobalVASpace(pGpu, pKernelGmmu, flags);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                    "Failed to create GVASpace, status:%x\n",
                    status);
        return status;
    }

    status = kgmmuEnableComputePeerAddressing(pGpu, pKernelGmmu, flags);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                    "Failed to enable compute peer addressing, status:%x\n",
                    status);
        return status;
    }

    return status;
}

/*
 *  State Pre Unload
 */
NV_STATUS
kgmmuStatePreUnload_IMPL
(
    OBJGPU *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32 flags
)
{
    NV_STATUS status = NV_OK;

    status = _kgmmuDestroyGlobalVASpace(pGpu, pKernelGmmu, flags);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                    "Failed to destory GVASpace, status:%x\n",
                    status);
        return status;
    }
    return status;
}

/*!
 * KernelGmmu destructor
 *
 * @param[in]  pKernelGmmu KernelGmmu object pointer
 */
void
kgmmuDestruct_IMPL(KernelGmmu *pKernelGmmu)
{
    NvU32       v;
    NvU32       b;

    // Free per big page size format and format-family storage.
    for (v = 0; v < GMMU_FMT_MAX_VERSION_COUNT; ++v)
    {
        if (NULL != pKernelGmmu->pFmtFamilies[v])
        {
            for (b = 0; b < GMMU_FMT_MAX_BIG_PAGE_SIZES; ++b)
            {
                portMemFree(pKernelGmmu->pFmtFamilies[v]->pFmts[b]);
                pKernelGmmu->pFmtFamilies[v]->pFmts[b] = NULL;
            }
            portMemFree(pKernelGmmu->pFmtFamilies[v]);
        }
    }
}

void
kgmmuStateDestroy_IMPL(OBJGPU *pGpu, KernelGmmu *pKernelGmmu)
{
    if (NULL != pKernelGmmu->pStaticInfo)
    {
        portMemFree((void *)pKernelGmmu->pStaticInfo);
        pKernelGmmu->pStaticInfo = NULL;
    }
    if (NULL != pKernelGmmu->pWarSmallPageTable)
    {
        memdescFree(pKernelGmmu->pWarSmallPageTable);
        memdescDestroy(pKernelGmmu->pWarSmallPageTable);
        pKernelGmmu->pWarSmallPageTable = NULL;
    }
    if (NULL != pKernelGmmu->pWarPageDirectory0)
    {
        memdescFree(pKernelGmmu->pWarPageDirectory0);
        memdescDestroy(pKernelGmmu->pWarPageDirectory0);
        pKernelGmmu->pWarPageDirectory0 = NULL;
    }
}

/*!
 * Initializes KERN_GMMU state based on registry key overrides
 *
 * @param[in]  pGpu
 * @param[in]  pKernelGmmu
 */
static void
_kgmmuInitRegistryOverrides(OBJGPU *pGpu, KernelGmmu *pKernelGmmu)
{
    NvU32 data;

    memdescOverrideInstLoc(DRF_VAL(_REG_STR_RM, _INST_LOC, _PDE, pGpu->instLocOverrides),
                           "GMMU PDE",
                           &pKernelGmmu->PDEAperture,
                           &pKernelGmmu->PDEAttr);
    memdescOverrideInstLoc(DRF_VAL(_REG_STR_RM, _INST_LOC, _BAR_PDE, pGpu->instLocOverrides),
                           "BAR1 PDE",
                           &pKernelGmmu->PDEBAR1Aperture,
                           &pKernelGmmu->PDEBAR1Attr);
    memdescOverrideInstLoc(DRF_VAL(_REG_STR_RM, _INST_LOC, _PTE, pGpu->instLocOverrides),
                           "GMMU PTE",
                           &pKernelGmmu->PTEAperture,
                           &pKernelGmmu->PTEAttr);
    memdescOverrideInstLoc(DRF_VAL(_REG_STR_RM, _INST_LOC, _BAR_PTE, pGpu->instLocOverrides),
                           "BAR1 PTE",
                           &pKernelGmmu->PTEBAR1Aperture,
                           &pKernelGmmu->PTEBAR1Attr);

    //
    // Check if we want to disable big page size per address space
    //
    pKernelGmmu->bEnablePerVaspaceBigPage = IsGM20X(pGpu);
    if (NV_OK == osReadRegistryDword(pGpu,
                   NV_REG_STR_RM_DISABLE_BIG_PAGE_PER_ADDRESS_SPACE, &data))
    {
        pKernelGmmu->bEnablePerVaspaceBigPage = !data;
    }

    if (NV_OK == osReadRegistryDword(pGpu,
                   NV_REG_STR_FERMI_BIG_PAGE_SIZE, &data))
    {
        if (pGpu->optimizeUseCaseOverride !=
            NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_DEFAULT)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "The %s regkey cannot be used with the %s regkey!\n",
                      NV_REG_STR_FERMI_BIG_PAGE_SIZE,
                      NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX);
            return;
        }
        else
        {
            switch (data)
            {
                case NV_REG_STR_FERMI_BIG_PAGE_SIZE_64KB:
                case NV_REG_STR_FERMI_BIG_PAGE_SIZE_128KB:
                    pKernelGmmu->overrideBigPageSize = data;
                    break;
                default:
                    break;
            }
        }
    }
    else if (pGpu->optimizeUseCaseOverride !=
             NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_DEFAULT)
    {
        switch (pGpu->optimizeUseCaseOverride)
        {
            case NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_SPARSE_TEX:
                pKernelGmmu->overrideBigPageSize = RM_PAGE_SIZE_64K;
                break;
            case NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_COMPUTE:
                pKernelGmmu->overrideBigPageSize = RM_PAGE_SIZE_128K;
                break;
            default:
                break;
        }
    }

    // Check if HW fault buffer is disabled
    if (NV_OK == osReadRegistryDword(pGpu,
                                     NV_REG_STR_RM_DISABLE_HW_FAULT_BUFFER, &data))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Overriding HW Fault buffer state to 0x%x due to regkey!\n",
                  data);
        pKernelGmmu->setProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED, data);
    }

}

GMMU_APERTURE
kgmmuGetMemAperture_IMPL
(
    KernelGmmu        *pKernelGmmu,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    switch (memdescGetAddressSpace(pMemDesc))
    {
        case ADDR_FBMEM:
            return GMMU_APERTURE_VIDEO;
        case ADDR_SYSMEM:
            if (NV_MEMORY_CACHED == memdescGetCpuCacheAttrib(pMemDesc))
            {
                return GMMU_APERTURE_SYS_COH;
            }
            return GMMU_APERTURE_SYS_NONCOH;
        default:
            NV_ASSERT(0);
            return GMMU_APERTURE_INVALID;
    }
}

/*!
 * Initialize GMMU format structures dependent on big page size.
 */
NV_STATUS
kgmmuFmtInit_IMPL(KernelGmmu *pKernelGmmu)
{
    NvU32       v;
    NvU32       b;

    // Allocate and init MMU formats for the supported big page sizes.
    for (v = 0; v < GMMU_FMT_MAX_VERSION_COUNT; ++v)
    {
        const NvU32      ver  = g_gmmuFmtVersions[v];
        GMMU_FMT_FAMILY *pFam = pKernelGmmu->pFmtFamilies[v];
        if (NULL != pFam)
        {
            for (b = 0; b < GMMU_FMT_MAX_BIG_PAGE_SIZES; ++b)
            {
                const NvU32 bigPageShift = g_gmmuFmtBigPageShifts[b];

                // Allocate +1 level for the last dual-level.
                const NvU32 numLevels = GMMU_FMT_MAX_LEVELS + 1;
                const NvU32 size = sizeof(GMMU_FMT) + sizeof(MMU_FMT_LEVEL) * numLevels;
                MMU_FMT_LEVEL *pLvls;

                // Allocate format and levels in one chunk.
                pFam->pFmts[b] = portMemAllocNonPaged(size);
                NV_ASSERT_OR_RETURN((pFam->pFmts[b] != NULL), NV_ERR_NO_MEMORY);
                portMemSet(pFam->pFmts[b], 0, size);

                // Levels stored contiguously after the format struct.
                pLvls = (MMU_FMT_LEVEL *)(pFam->pFmts[b] + 1);

                // Common init.
                pFam->pFmts[b]->version    = ver;
                pFam->pFmts[b]->pRoot      = pLvls;
                pFam->pFmts[b]->pPdeMulti  = &pFam->pdeMulti;
                pFam->pFmts[b]->pPde       = &pFam->pde;
                pFam->pFmts[b]->pPte       = &pFam->pte;

                kgmmuFmtInitLevels_HAL(pKernelGmmu, pLvls, numLevels, ver, bigPageShift);
                kgmmuFmtInitCaps_HAL(pKernelGmmu, pFam->pFmts[b]);
            }
        }
    }

    return NV_OK;
}

/*!
 * Retrieve GMMU format family based on version.
 */
const GMMU_FMT_FAMILY *
kgmmuFmtGetFamily_IMPL(KernelGmmu *pKernelGmmu, NvU32 version)
{
    NvU32       v;

    // Find a matching format.
    for (v = GMMU_FMT_MAX_VERSION_COUNT; v > 0; --v)
    {
        if (0 == version)
        {
            // Pick newest default version if none requested.
            if (NULL != pKernelGmmu->pFmtFamilies[v - 1])
            {
                return pKernelGmmu->pFmtFamilies[v - 1];
            }
        }
        else if (g_gmmuFmtVersions[v - 1] == version)
        {
            return pKernelGmmu->pFmtFamilies[v - 1];
        }
    }

    return NULL;
}

/*!
 * Returns GMMU settings that are static after GPU state init/load is
 * finished.
 */
const NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *
kgmmuGetStaticInfo_IMPL
(
    OBJGPU *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    // check if state Init has not completed.
    NV_ASSERT_OR_ELSE(pKernelGmmu != NULL, return NULL);

    return pKernelGmmu->pStaticInfo;
}

/*!
 * @brief Initializes static info data from the Physical side.
 *
 * @param      pGpu
 * @param      pKernelGmmu
 * @param[out] pStaticInfo pointer to the static info init on Physical driver.
 */
NV_STATUS
kgmmuInitStaticInfo_KERNEL
(
    OBJGPU *pGpu,
    KernelGmmu *pKernelGmmu,
    NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *pStaticInfo
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS status;

    status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_GMMU_GET_STATIC_INFO,
                                pStaticInfo, sizeof(*pStaticInfo));

    return status;
}

/*!
 * Retrieve GMMU format based on version and big page size.
 */
const GMMU_FMT *
kgmmuFmtGet_IMPL(KernelGmmu *pKernelGmmu, NvU32 version, NvU64 bigPageSize)
{
    const GMMU_FMT_FAMILY *pFmtFamily  = kgmmuFmtGetFamily(pKernelGmmu, version);

    if (NULL != pFmtFamily)
    {
        NvU32 b;

        // Pick default big page size if none requested.
        if (0 == bigPageSize)
        {
            //
            // Retrieve Big Page Size. If it is not yet set, set it to 64K.
            // Useful when this method is invoked before big page size is set.
            //
            if (0 == (bigPageSize = kgmmuGetBigPageSize_HAL(pKernelGmmu)))
                bigPageSize = NVBIT64(16);
        }

        // Find a matching format.
        for (b = 0; b < GMMU_FMT_MAX_BIG_PAGE_SIZES; ++b)
        {
            if (NVBIT64(g_gmmuFmtBigPageShifts[b]) == bigPageSize)
            {
                return pFmtFamily->pFmts[b];
            }
        }
    }

    return NULL;
}

/*!
 * Check if a big page size is supported.
 */
NvBool
kgmmuFmtIsBigPageSizeSupported_IMPL(KernelGmmu *pKernelGmmu, NvU64 bigPageSize)
{
    if (kgmmuIsPerVaspaceBigPageEn(pKernelGmmu))
    {
        return NV_TRUE;
    }
    return kgmmuGetBigPageSize_HAL(pKernelGmmu) == bigPageSize;
}

/*!
 * @bried Returns the latest supported MMU fmt.
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelGmmu   KernelGmmu pointer
 *
 * @returns const GMMU_FMT*
 */
const GMMU_FMT*
kgmmuFmtGetLatestSupportedFormat_IMPL(OBJGPU *pGpu, KernelGmmu *pKernelGmmu)
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

    return kgmmuFmtGet(pKernelGmmu, maxFmtVersionSupported, 0);
}

/*!
 * @brief Used for calculating total memory required for page tables
          required for translating a given VA range.
 *
 * @param     pGpu
 * @param     pKernelGmmu
 * @param[in] pFmt              Pointer to GMMU format
 * @param[in] vaBase            Start VA
 * @param[in] vaLimit           End VA
 * @param[in] pageSizeLockMask  Mask of page sizes locked down at VA reservation
 *
 * @returns total size of page tables.
 */
NvU64
kgmmuGetSizeOfPageTables_IMPL
(
    OBJGPU         *pGpu,
    KernelGmmu     *pKernelGmmu,
    const GMMU_FMT *pFmt,
    NvU64           vaBase,
    NvU64           vaLimit,
    NvU64           pageSizeLockMask
)
{
    const MMU_FMT_LEVEL *pPgTbl         = NULL;
    NvU64                pgTblSize      = 0;
    NvU64                numPgTblsCeil;
    NvU64                numPgTblsFloor;
    NvU64                numEntries;
    NvU32                pageShift;

    // Loop over all page table sizes in mask
    FOR_EACH_INDEX_IN_MASK(64, pageShift, pageSizeLockMask)
    {
        pPgTbl = mmuFmtFindLevelWithPageShift(pFmt->pRoot, pageShift);

        //
        // Do not consider page directories. They are handled by
        // @ref kgmmuGetSizeOfPageDirs.
        //
        if (!pPgTbl->bPageTable || (pPgTbl->numSubLevels != 0))
        {
            continue;
        }

        numPgTblsCeil  = NV_DIV_AND_CEIL(vaLimit, NVBIT64(pPgTbl->virtAddrBitHi + 1)) -
                         (vaBase / NVBIT64(pPgTbl->virtAddrBitHi + 1)) + 1;
        numPgTblsFloor = vaLimit / NVBIT64(pPgTbl->virtAddrBitHi + 1);

        // If full page tables are not used, allocate only as much as needed.
        if (numPgTblsFloor == 0)
        {
            numEntries = mmuFmtVirtAddrToEntryIndex(pPgTbl, vaLimit) -
                         mmuFmtVirtAddrToEntryIndex(pPgTbl, vaBase) + 1;
            pgTblSize  += numEntries * pPgTbl->entrySize;
        }
        else
        {
            pgTblSize += numPgTblsCeil * mmuFmtLevelSize(pPgTbl);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END

    return pgTblSize;
}

/*!
 * @brief Used for calculating total memory required for page directories
          required for translating  a given VA range.
 *
 * @param       pGpu
 * @param       pKernelGmmu
 * @param[in]   pFmt      Pointer to GMMU format
 * @param[in]   vaBase    Start VA
 * @param[in]   vaLimit   End VA
 *
 * @returns total size of page directories
 */
NvU64
kgmmuGetSizeOfPageDirs_IMPL
(
    OBJGPU         *pGpu,
    KernelGmmu     *pKernelGmmu,
    const GMMU_FMT *pFmt,
    NvU64           vaBase,
    NvU64           vaLimit,
    NvU64           pageSizeLockMask
)
{
    const MMU_FMT_LEVEL *pLevel = NULL;
    NvU64                size   = 0;
    NvU16                i;

    NV_ASSERT_OR_RETURN(pFmt != NULL, 0);

    pLevel = pFmt->pRoot;

    //
    // Retain only the lowest set bit
    //
    // If the lowest set bit corresponds to a leaf page table (4K or 64K), we"ll
    // calculate memory for all upper level page directories and if the set bit
    // corresponds to an upper level page directory we"ll factor in all levels
    // from the root upto that level.
    //
    pageSizeLockMask = pageSizeLockMask & -((NvS64)pageSizeLockMask);

    // Accumulate size for all Page Directories.
    for (i = 0; i < GMMU_FMT_MAX_LEVELS - 1; i++)
    {
        NvU64 vaPerEntry = mmuFmtEntryVirtAddrMask(pLevel) + 1;
        NvU64 numEntries = NV_DIV_AND_CEIL(vaLimit, vaPerEntry) -
                           (vaBase / vaPerEntry) + 1;
        NvU64 levelSize  = numEntries * pLevel->entrySize;
        levelSize        = NV_ROUNDUP(levelSize, RM_PAGE_SIZE);

        // Stop accumulating size once we are beyond the specified level.
        if (mmuFmtLevelPageSize(pLevel) < pageSizeLockMask)
        {
            break;
        }

        size += levelSize;

        // If there's one sublevel choose that.
        if (pLevel->numSubLevels == 1)
        {
            pLevel = &(pLevel->subLevels[0]);
        }
        else
        {
            // Choose the 4K page size sublevel.
            pLevel = &(pLevel->subLevels[1]);
        }
        NV_ASSERT_OR_RETURN(pLevel != NULL, 0);

        // Stop accumulating size if we've exhausted all Page Dirs.
        if (pLevel->bPageTable && (pLevel->numSubLevels == 0))
        {
            break;
        }
    }

    return size;
}

/*
 * Fill comptag field in PTE.
 */
void kgmmuFieldSetKindCompTags_IMPL
(
    KernelGmmu          *pGmmu,
    const GMMU_FMT      *pFmt,
    const MMU_FMT_LEVEL *pLevel,
    const COMPR_INFO    *pCompr,
    NvU64                physAddr,
    NvU64                surfOffset,
    NvU32                pteIndex,
    NvU8                *pEntries
)
{
    OBJGPU                            *pGpu                = ENG_GET_GPU(pGmmu);
    GMMU_COMPR_INFO                    comprInfo           = {0};

    comprInfo.compressedKind        = pCompr->kind;
    comprInfo.compPageShift         = pCompr->compPageShift;

    if (memmgrIsKind_HAL(GPU_GET_MEMORY_MANAGER(pGpu), FB_IS_KIND_COMPRESSIBLE, pCompr->kind))
    {
        const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
            kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

        if (pCompr->bPhysBasedComptags)
        {
            NvBool bCallingContextPlugin;

            NV_ASSERT(pMemorySystemConfig->bOneToOneComptagLineAllocation || pMemorySystemConfig->bUseRawModeComptaglineAllocation);

            NV_ASSERT_OR_RETURN_VOID(vgpuIsCallingContextPlugin(pGpu, &bCallingContextPlugin) == NV_OK);
            if (IS_VIRTUAL_WITH_SRIOV(pGpu) || bCallingContextPlugin ||
                pMemorySystemConfig->bUseRawModeComptaglineAllocation)
            {
                // In raw mode or when SR-IOV is enabled, HW handles compression tags
                comprInfo.compTagLineMin = 1;
            }
            else
            {
                comprInfo.compTagLineMin = memmgrDetermineComptag_HAL(pGpu, GPU_GET_MEMORY_MANAGER(pGpu), physAddr);
            }

            comprInfo.compPageIndexLo = surfOffset >> pCompr->compPageShift;
            comprInfo.compPageIndexHi = (surfOffset + mmuFmtLevelPageSize(pLevel) - 1) >> pCompr->compPageShift;
            comprInfo.compTagLineMultiplier = 1;
        }
        else
        {
            comprInfo.compPageIndexLo       = pCompr->compPageIndexLo;
            comprInfo.compPageIndexHi       = pCompr->compPageIndexHi;
            comprInfo.compTagLineMin        = pCompr->compTagLineMin;
            comprInfo.compTagLineMultiplier = pCompr->compTagLineMultiplier;
        }
    }

    gmmuFmtInitPteCompTags(pFmt, pLevel, &comprInfo, surfOffset, pteIndex, 1, pEntries);
}

NV_STATUS
kgmmuFaultBufferGetAddressSpace_IMPL
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu,
    NvU32                 index,
    NvU32                *pFaultBufferAddrSpace,
    NvU32                *pFaultBufferAttr
)
{
    NvU32 faultBufferAddrSpace = ADDR_UNKNOWN;
    NvU32 faultBufferAttr = 0;

    NV_ASSERT_OR_RETURN((index < NUM_FAULT_BUFFERS), NV_ERR_INVALID_ARGUMENT);

    if (index == NON_REPLAYABLE_FAULT_BUFFER)
    {
        faultBufferAddrSpace = ADDR_SYSMEM;
        faultBufferAttr      = NV_MEMORY_CACHED;
        memdescOverrideInstLoc(DRF_VAL(_REG_STR_RM, _INST_LOC_3, _UVM_FAULT_BUFFER_NONREPLAYABLE, pGpu->instLocOverrides3),
                               "UVM non-replayable fault", &faultBufferAddrSpace, &faultBufferAttr);
    }
    else if (index == REPLAYABLE_FAULT_BUFFER)
    {
        faultBufferAddrSpace = ADDR_SYSMEM;
        faultBufferAttr      = NV_MEMORY_CACHED;
        memdescOverrideInstLoc(DRF_VAL(_REG_STR_RM, _INST_LOC_4, _UVM_FAULT_BUFFER_REPLAYABLE, pGpu->instLocOverrides4),
                               "UVM replayable fault", &faultBufferAddrSpace, &faultBufferAttr);
    }

    if (pFaultBufferAddrSpace != NULL)
    {
        *pFaultBufferAddrSpace = faultBufferAddrSpace;
    }

    if (pFaultBufferAttr != NULL)
    {
        *pFaultBufferAttr = faultBufferAttr;
    }

    return NV_OK;
}

NV_STATUS
kgmmuFaultBufferCreateMemDesc_IMPL
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu,
    NvU32                 index,
    NvU32                 faultBufferSize,
    NvU64                 memDescFlags,
    MEMORY_DESCRIPTOR   **ppMemDesc
)
{
    NV_STATUS status;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    NvU32 faultBufferAddrSpace = ADDR_UNKNOWN;
    NvU32 faultBufferAttr = 0;
    NvBool isContiguous = NV_FALSE;

    NV_ASSERT_OR_RETURN((index < NUM_FAULT_BUFFERS), NV_ERR_INVALID_ARGUMENT);

    status = kgmmuFaultBufferGetAddressSpace(pGpu, pKernelGmmu, index,
                                             &faultBufferAddrSpace, &faultBufferAttr);
    if (status != NV_OK)
    {
        return status;
    }

    if (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        // Allocate contiguous fault buffers for SR-IOV Heavy
        isContiguous = NV_TRUE;
    }

    status = memdescCreate(&pMemDesc, pGpu,
                           RM_PAGE_ALIGN_UP(faultBufferSize), 0, isContiguous,
                           faultBufferAddrSpace, faultBufferAttr,
                           (memDescFlags | MEMDESC_FLAGS_LOST_ON_SUSPEND));
    if (status != NV_OK)
    {
        return status;
    }

    //
    // GPU doesn't read faultbuffer memory, so if faultBuffers are in sysmem, ensure that GpuCacheAttr
    // is set to UNCACHED as having a vol bit set in PTEs will ensure HUB uses L2Bypass mode and it will
    // save extra cycles to cache in L2 while MMU will write fault packets.
    //
    if (faultBufferAddrSpace == ADDR_SYSMEM &&
        pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_SYSMEM_FAULT_BUFFER_GPU_UNCACHED))
    {
        memdescSetGpuCacheAttrib(pMemDesc, NV_MEMORY_UNCACHED);
    }

    *ppMemDesc = pMemDesc;

    return NV_OK;
}

NV_STATUS
kgmmuFaultBufferUnregister_IMPL
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu,
    NvU32                 index
)
{
    struct HW_FAULT_BUFFER *pFaultBuffer;
    MEMORY_DESCRIPTOR      *pMemDesc;

    pFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hwFaultBuffers[index];
    pMemDesc = pFaultBuffer->pFaultBufferMemDesc;

    pFaultBuffer->faultBufferSize = 0;
    pFaultBuffer->pFaultBufferMemDesc = NULL;

    memdescDestroy(pMemDesc);

    return NV_OK;
}

NV_STATUS
kgmmuFaultBufferAlloc_IMPL
(
    OBJGPU         *pGpu,
    KernelGmmu     *pKernelGmmu,
    NvU32           index,
    NvU32           faultBufferSize
)
{
    NV_STATUS status;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    struct HW_FAULT_BUFFER *pFaultBuffer;
    const char *name = (index == REPLAYABLE_FAULT_BUFFER ? NV_RM_SURF_NAME_REPLAYABLE_FAULT_BUFFER : NV_RM_SURF_NAME_NONREPLAYABLE_FAULT_BUFFER);

    NV_ASSERT_OR_RETURN((index < NUM_FAULT_BUFFERS), NV_ERR_INVALID_ARGUMENT);

    if (pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED))
        return NV_OK;

    pFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hwFaultBuffers[index];

    status = kgmmuFaultBufferCreateMemDesc(pGpu, pKernelGmmu, index, faultBufferSize,
                                           MEMDESC_FLAGS_NONE, &pMemDesc);
    if (status != NV_OK)
    {
        return status;
    }

    status = memdescAlloc(pMemDesc);
    if (status != NV_OK)
    {
        memdescDestroy(pMemDesc);
        return status;
    }

    memdescSetName(pGpu, pMemDesc, name, NULL);

    pFaultBuffer->faultBufferSize = faultBufferSize;
    pFaultBuffer->pFaultBufferMemDesc = pMemDesc;

    return status;
}

NV_STATUS
kgmmuFaultBufferFree_IMPL
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu,
    NvU32                 index
)
{
    struct HW_FAULT_BUFFER *pFaultBuffer;

    NV_ASSERT_OR_RETURN((index < NUM_FAULT_BUFFERS), NV_ERR_INVALID_ARGUMENT);

    if (pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED))
        return NV_OK;

    pFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hwFaultBuffers[index];

    memdescFree(pFaultBuffer->pFaultBufferMemDesc);

    kgmmuFaultBufferUnregister(pGpu, pKernelGmmu, index);

    return NV_OK;
}

NV_STATUS
kgmmuFaultBufferReplayableAllocate_IMPL
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu,
    NvHandle              hClient,
    NvHandle              hObject
)
{
    NV_STATUS               status;
    struct HW_FAULT_BUFFER *pFaultBuffer;
    NvU32                   faultBufferSize;
    NvU32                   numBufferPages;
    const NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *pStaticInfo = kgmmuGetStaticInfo(pGpu, pKernelGmmu);

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED))
    {
        return NV_OK;
    }

    pFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hwFaultBuffers[REPLAYABLE_FAULT_BUFFER];
    if (pFaultBuffer->pFaultBufferMemDesc != NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    faultBufferSize = pStaticInfo->replayableFaultBufferSize;

    status = kgmmuFaultBufferAlloc(pGpu, pKernelGmmu,
                                   REPLAYABLE_FAULT_BUFFER,
                                   faultBufferSize);
    if (status != NV_OK)
    {
        return status;
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS *pParams;

        pParams = portMemAllocNonPaged(sizeof(*pParams));
        if (pParams == NULL)
        {
            kgmmuFaultBufferFree(pGpu, pKernelGmmu, REPLAYABLE_FAULT_BUFFER);
            return NV_ERR_NO_MEMORY;
        }
        portMemSet(pParams, 0, sizeof(*pParams));

        numBufferPages = RM_PAGE_ALIGN_UP(faultBufferSize) / RM_PAGE_SIZE;
        if (numBufferPages > NV_ARRAY_ELEMENTS(pParams->faultBufferPteArray))
        {
            portMemFree(pParams);
            kgmmuFaultBufferFree(pGpu, pKernelGmmu, REPLAYABLE_FAULT_BUFFER);
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        memdescGetPhysAddrs(pFaultBuffer->pFaultBufferMemDesc,
                            AT_GPU, 0, RM_PAGE_SIZE,
                            numBufferPages, pParams->faultBufferPteArray);

        pParams->hClient            = hClient;
        pParams->hObject            = hObject;
        pParams->faultBufferSize    = faultBufferSize;

        status = pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_GMMU_REGISTER_FAULT_BUFFER,
                                 pParams, sizeof(*pParams));

        portMemFree(pParams);
        if (status != NV_OK)
        {
            kgmmuFaultBufferFree(pGpu, pKernelGmmu, REPLAYABLE_FAULT_BUFFER);
            return status;
        }
    }

    pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hFaultBufferClient = hClient;
    pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hFaultBufferObject = hObject;

    return NV_OK;
}

NV_STATUS
kgmmuFaultBufferReplayableDestroy_IMPL
(
    OBJGPU      *pGpu,
    KernelGmmu  *pKernelGmmu
)
{
    NV_STATUS               status = NV_OK;
    struct HW_FAULT_BUFFER *pFaultBuffer;

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED))
    {
        return NV_OK;
    }

    pFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hwFaultBuffers[REPLAYABLE_FAULT_BUFFER];
    if (pFaultBuffer->pFaultBufferMemDesc == NULL)
    {
        return NV_OK;
    }

    pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hFaultBufferClient = 0;
    pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hFaultBufferObject = 0;

    if (IS_GSP_CLIENT(pGpu))
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        status = pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_GMMU_UNREGISTER_FAULT_BUFFER,
                                 NULL, 0);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unregistering Replayable Fault buffer failed (status=0x%08x), proceeding...\n",
                      status);
        }
    }

    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        status = kgmmuFaultBufferUnregister(pGpu, pKernelGmmu, REPLAYABLE_FAULT_BUFFER);
    }
    else
    {
        status = kgmmuFaultBufferFree(pGpu, pKernelGmmu, REPLAYABLE_FAULT_BUFFER);
    }

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Destroying Replayable Fault buffer failed (status=0x%08x), proceeding...\n",
                  status);
    }

    return NV_OK;
}

/*!
 * @brief: Encodes peer addresses to support NVSwitch systems.
 *
 * This function prepends the fabricBaseAddress to a physical address in order
 * to generate a unique peer address from the global fabric address space.
 *
 * @param[in] pAddresses        : Array of physical addresses to be encoded.
 * @param[in] fabricBaseAddress : Unique fabric base address.
 * @param[in] count             : Count if physical addresses.
 */
static void
_kgmmuEncodePeerAddrs
(
    NvU64              *pAddresses,
    NvU64               fabricBaseAddress,
    NvU64               count
)
{
    NvU64 i;

    //
    // If there is no fabric address, it should be a NOP. Note, this acts as an
    // early complete path for other PEER addressing.
    //
    if (fabricBaseAddress == NVLINK_INVALID_FABRIC_ADDR)
    {
        return;
    }

    for (i = 0; i < count; i++)
    {
        pAddresses[i] = fabricBaseAddress + pAddresses[i];
    }
}

void
kgmmuEncodePhysAddrs_IMPL
(
    KernelGmmu         *pKernelGmmu,
    const GMMU_APERTURE aperture,
    NvU64              *pAddresses,
    NvU64               fabricBaseAddress,
    NvU64               count
)
{
    NV_ASSERT(aperture != GMMU_APERTURE_INVALID);

    if (aperture == GMMU_APERTURE_SYS_COH ||
        aperture == GMMU_APERTURE_SYS_NONCOH)
    {
        kgmmuEncodeSysmemAddrs_HAL(pKernelGmmu, pAddresses, count);
    }
    else if (aperture == GMMU_APERTURE_PEER)
    {
        _kgmmuEncodePeerAddrs(pAddresses, fabricBaseAddress, count);
    }
    else
    {
        return;
    }
}

NvU64
kgmmuEncodePhysAddr_IMPL
(
    KernelGmmu         *pKernelGmmu,
    const GMMU_APERTURE aperture,
    NvU64               physAddr,
    NvU64               fabricBaseAddress
)
{
    kgmmuEncodePhysAddrs(pKernelGmmu, aperture, &physAddr, fabricBaseAddress, 1);
    return physAddr;
}

static void
_kgmmuClientShadowBufferQueueCopyData
(
    NvLength      msgSize,
    NvLength      opIdx,
    QueueContext *pCtx,
    void         *pData,
    NvLength      count,
    NvBool        bCopyIn
)
{
    NvLength size;
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuffer = pCtx->pData;
    NvU8 *pQueueData, *pClientData = pData;
    void *pDst, *pSrc;

    if (count == 0)
        return;

    size = count * msgSize;
    pQueueData = KERNEL_POINTER_FROM_NvP64(NvU8 *, pClientShadowFaultBuffer->pBufferAddress);
    pQueueData = pQueueData + (opIdx * msgSize);

    pDst = bCopyIn ? pQueueData : pClientData;
    pSrc = bCopyIn ? pClientData : pQueueData;
    portMemCopy(pDst, size, pSrc, size);
}

static NV_STATUS
_kgmmuClientShadowFaultBufferQueueAllocate
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    NV_STATUS status;
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuffer;
    MEMORY_DESCRIPTOR *pQueueMemDesc;

    pClientShadowFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].clientShadowFaultBuffer;

    status = memdescCreate(&pQueueMemDesc, pGpu,
                           sizeof(GMMU_SHADOW_FAULT_BUF), RM_PAGE_SIZE,
                           NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED,
                           MEMDESC_FLAGS_NONE);
    if (status != NV_OK)
    {
        return status;
    }

    status = memdescAlloc(pQueueMemDesc);
    if (status != NV_OK)
    {
        memdescDestroy(pQueueMemDesc);
        return status;
    }

    status = memdescMap(pQueueMemDesc, 0,
                        memdescGetSize(pQueueMemDesc),
                        NV_TRUE, NV_PROTECT_READ_WRITE,
                        &pClientShadowFaultBuffer->pQueueAddress,
                        &pClientShadowFaultBuffer->pQueuePriv);
    if (status != NV_OK)
    {
        memdescFree(pQueueMemDesc);
        memdescDestroy(pQueueMemDesc);
        return status;
    }

    pClientShadowFaultBuffer->queueContext.pCopyData = _kgmmuClientShadowBufferQueueCopyData;
    pClientShadowFaultBuffer->queueContext.pData = pClientShadowFaultBuffer;
    pClientShadowFaultBuffer->pQueueMemDesc = pQueueMemDesc;

    return NV_OK;
}

void
kgmmuClientShadowFaultBufferQueueDestroy_IMPL
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvBool      bFreeQueue
)
{
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuffer;
    MEMORY_DESCRIPTOR *pQueueMemDesc;

    pClientShadowFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].clientShadowFaultBuffer;

    pQueueMemDesc = pClientShadowFaultBuffer->pQueueMemDesc;

    pClientShadowFaultBuffer->pQueueMemDesc = NULL;
    pClientShadowFaultBuffer->pQueueAddress = NvP64_NULL;
    pClientShadowFaultBuffer->pQueuePriv = NvP64_NULL;

    if (bFreeQueue)
    {
        memdescFree(pQueueMemDesc);
    }
    memdescDestroy(pQueueMemDesc);
}

static NV_STATUS
_kgmmuClientShadowFaultBufferPagesAllocate
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32       shadowFaultBufferSize
)
{
    NV_STATUS status;
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuffer;
    MEMORY_DESCRIPTOR *pMemDesc;

    pClientShadowFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].clientShadowFaultBuffer;

    shadowFaultBufferSize = RM_PAGE_ALIGN_UP(shadowFaultBufferSize);

    status = memdescCreate(&pMemDesc, pGpu,
                           shadowFaultBufferSize, RM_PAGE_SIZE,
                           NV_FALSE, ADDR_SYSMEM, NV_MEMORY_CACHED,
                           MEMDESC_FLAGS_NONE);
    if (status != NV_OK)
    {
        return status;
    }

    status = memdescAlloc(pMemDesc);
    if (status != NV_OK)
    {
        memdescDestroy(pMemDesc);
        return status;
    }

    status = memdescMap(pMemDesc, 0,
                        memdescGetSize(pMemDesc),
                        NV_TRUE, NV_PROTECT_READ_WRITE,
                        &pClientShadowFaultBuffer->pBufferAddress,
                        &pClientShadowFaultBuffer->pBufferPriv);
    if (status != NV_OK)
    {
        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);
        return status;
    }

    pClientShadowFaultBuffer->pBufferMemDesc = pMemDesc;

    return NV_OK;
}

void
kgmmuClientShadowFaultBufferPagesDestroy_IMPL
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvBool      bFreePages
)
{
    MEMORY_DESCRIPTOR *pMemDesc;
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuffer;
    GMMU_FAULT_BUFFER_PAGE *pBufferPage;
    NvU32 i;

    pClientShadowFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].clientShadowFaultBuffer;
    pMemDesc = pClientShadowFaultBuffer->pBufferMemDesc;

    if (bFreePages)
    {
        memdescUnmap(pMemDesc,
                     NV_TRUE, osGetCurrentProcess(),
                     pClientShadowFaultBuffer->pBufferAddress,
                     pClientShadowFaultBuffer->pBufferPriv);

        memdescFree(pMemDesc);
    }
    else
    {
        for (i = 0; i < pClientShadowFaultBuffer->numBufferPages; i++)
        {
            pBufferPage = &pClientShadowFaultBuffer->pBufferPages[i];

            memdescUnmap(pMemDesc, NV_TRUE, osGetCurrentProcess(),
                         pBufferPage->pAddress, pBufferPage->pPriv);
        }
        portMemFree(pClientShadowFaultBuffer->pBufferPages);
    }
    memdescDestroy(pMemDesc);
}

NV_STATUS
kgmmuClientShadowFaultBufferRegister_IMPL
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    NV_STATUS status;
    struct GMMU_FAULT_BUFFER *pFaultBuffer;
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuffer;
    GMMU_SHADOW_FAULT_BUF *pQueue;
    MEMORY_DESCRIPTOR *pBufferMemDesc;
    RmPhysAddr shadowFaultBufferQueuePhysAddr;
    NvU32 queueCapacity, numBufferPages;
    NvU32 faultBufferSize;
    const NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *pStaticInfo = kgmmuGetStaticInfo(pGpu, pKernelGmmu);

    pFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF];
    pClientShadowFaultBuffer = &pFaultBuffer->clientShadowFaultBuffer;
    faultBufferSize = pStaticInfo->nonReplayableFaultBufferSize;

    pQueue = KERNEL_POINTER_FROM_NvP64(GMMU_SHADOW_FAULT_BUF *,
                                       pClientShadowFaultBuffer->pQueueAddress);
    queueCapacity = faultBufferSize / NVC369_BUF_SIZE;

    status = queueInitNonManaged(pQueue, queueCapacity);
    if (status != NV_OK)
    {
        return status;
    }

    if (!IS_GSP_CLIENT(pGpu))
    {
        portSyncSpinlockAcquire(pFaultBuffer->pShadowFaultBufLock);

        if (pFaultBuffer->pClientShadowFaultBuffer == NULL)
        {
            pFaultBuffer->pClientShadowFaultBuffer = pClientShadowFaultBuffer;
        }
        else
        {
            status = NV_ERR_NOT_SUPPORTED;
        }

        portSyncSpinlockRelease(pFaultBuffer->pShadowFaultBufLock);

        if (status != NV_OK)
        {
            queueDestroy(pQueue);
            return status;
        }
    }
    else
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS *pParams;

        pParams = portMemAllocNonPaged(sizeof(*pParams));
        if (pParams == NULL)
        {
            queueDestroy(pQueue);
            return NV_ERR_NO_MEMORY;
        }
        portMemSet(pParams, 0, sizeof(*pParams));

        pBufferMemDesc = pClientShadowFaultBuffer->pBufferMemDesc;
        numBufferPages = memdescGetSize(pBufferMemDesc) >> RM_PAGE_SHIFT;
        if (numBufferPages > NV_ARRAY_ELEMENTS(pParams->shadowFaultBufferPteArray))
        {
            portMemFree(pParams);
            queueDestroy(pQueue);
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        shadowFaultBufferQueuePhysAddr = memdescGetPhysAddr(pClientShadowFaultBuffer->pQueueMemDesc,
                                                            AT_GPU, 0);

        memdescGetPhysAddrs(pBufferMemDesc,
                            AT_GPU,
                            0, RM_PAGE_SIZE,
                            numBufferPages, pParams->shadowFaultBufferPteArray);

        pParams->shadowFaultBufferQueuePhysAddr = shadowFaultBufferQueuePhysAddr;
        pParams->shadowFaultBufferSize          = faultBufferSize;

        status = pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER,
                                 pParams, sizeof(*pParams));

        portMemFree(pParams);
        if (status != NV_OK)
        {
            queueDestroy(pQueue);
            return status;
        }

        pFaultBuffer->pClientShadowFaultBuffer = pClientShadowFaultBuffer;
    }

    return NV_OK;
}

void
kgmmuClientShadowFaultBufferUnregister_IMPL
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    NV_STATUS status = NV_OK;
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuffer;
    GMMU_SHADOW_FAULT_BUF *pQueue;
    struct GMMU_FAULT_BUFFER *pFaultBuffer;

    pFaultBuffer = &pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF];

    if (!IS_GSP_CLIENT(pGpu))
    {
        portSyncSpinlockAcquire(pFaultBuffer->pShadowFaultBufLock);

        pFaultBuffer->pClientShadowFaultBuffer = NULL;

        portSyncSpinlockRelease(pFaultBuffer->pShadowFaultBufLock);
    }
    else
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        status = pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER,
                                 NULL, 0);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unregistering Replayable Fault buffer failed (status=0x%08x), proceeding...\n",
                      status);
        }

        pFaultBuffer->pClientShadowFaultBuffer = NULL;
    }

    pClientShadowFaultBuffer = &pFaultBuffer->clientShadowFaultBuffer;
    pQueue = KERNEL_POINTER_FROM_NvP64(GMMU_SHADOW_FAULT_BUF *,
                                       pClientShadowFaultBuffer->pQueueAddress);
    queueDestroy(pQueue);
}

/*!
 * @brief Creates shadow fault buffer for client handling of non-replayable
 *        faults in the CPU-RM, and registers it in the GSP-RM.
 *
 * @param[in] pGpu
 * @param[in] pKernelGmmu
 *
 * @returns
 */
NV_STATUS
kgmmuClientShadowFaultBufferNonreplayableAllocate_IMPL
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    NV_STATUS   status;
    const NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *pStaticInfo = kgmmuGetStaticInfo(pGpu, pKernelGmmu);

    ct_assert((RM_PAGE_SIZE % sizeof(struct GMMU_FAULT_PACKET)) == 0);

    NV_ASSERT_OR_RETURN(!pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED), NV_ERR_INVALID_STATE);

    NV_ASSERT_OR_RETURN(pStaticInfo->nonReplayableFaultBufferSize != 0, NV_ERR_INVALID_STATE);

    status = _kgmmuClientShadowFaultBufferQueueAllocate(pGpu, pKernelGmmu);
    if (status != NV_OK)
    {
        return status;
    }

    status = _kgmmuClientShadowFaultBufferPagesAllocate(pGpu, pKernelGmmu,
                                                       pStaticInfo->nonReplayableFaultBufferSize);
    if (status != NV_OK)
    {
        goto destroy_queue_and_exit;
    }

    status = kgmmuClientShadowFaultBufferRegister(pGpu, pKernelGmmu);
    if (status != NV_OK)
    {
        goto destroy_pages_and_exit;
    }

    return NV_OK;

destroy_pages_and_exit:
    kgmmuClientShadowFaultBufferPagesDestroy(pGpu, pKernelGmmu, NV_TRUE);
destroy_queue_and_exit:
    kgmmuClientShadowFaultBufferQueueDestroy(pGpu, pKernelGmmu, NV_TRUE);
    return status;
}

/*!
 * @brief Unregister client shadow fault buffer in the GSP-RM or destroy
 *        it in the CPU-RM.
 *
 * @param[in] pGpu
 * @param[in] pKernelGmmu
 *
 * @returns
 */
NV_STATUS
kgmmuClientShadowFaultBufferNonreplayableDestroy_IMPL
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuffer;
    NvBool bFreeMemory = !RMCFG_FEATURE_PLATFORM_GSP;

    pClientShadowFaultBuffer = pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pClientShadowFaultBuffer;

    if (pClientShadowFaultBuffer != NvP64_NULL)
    {
        kgmmuClientShadowFaultBufferUnregister(pGpu, pKernelGmmu);

        kgmmuClientShadowFaultBufferPagesDestroy(pGpu, pKernelGmmu, bFreeMemory);
        kgmmuClientShadowFaultBufferQueueDestroy(pGpu, pKernelGmmu, bFreeMemory);
    }

    return NV_OK;
}

/*!
 * Returns the minimum allocation size to align to big-page size in bytes
 *
 * @param[in]  pKernelGmmu
 *
 * @return NvU32
 */
NvU32
kgmmuGetMinBigPageSize_IMPL(KernelGmmu *pKernelGmmu)
{
    //
    // Set the minimum size in the heap that we will round up to a big page instead
    // just 4KB. HW doesn't like 4KB pages in video memory, but SW wants to pack
    // physical memory sometimes.  Typically UMDs that really care about perf use
    // suballocation for larger RM allocations anyway.
    //
    // Promote allocates bigger than half the big page size.
    // (this is a policy change for Big page sizes/VASpace)
    //
    return RM_PAGE_SIZE_64K >> 1;
}

/*!
 * @brief Initializes the init block for an engine
 *
 * @param[in] pKernelGmmu
 * @param[in] pInstBlkDesc    Memory descriptor for the instance block of the engine
 * @param[in] pVAS            OBJVASPACE pointer of the engine
 * @param[in] subctxId        subctxId Value
 * @param[in] pInstBlkParams  Pointer to the structure storing the parameters passed by the caller
 *
 * @returns NV_STATUS
 */
NV_STATUS
kgmmuInstBlkInit_IMPL
(
    KernelGmmu           *pKernelGmmu,
    MEMORY_DESCRIPTOR    *pInstBlkDesc,
    OBJVASPACE           *pVAS,
    NvU32                 subctxId,
    INST_BLK_INIT_PARAMS *pInstBlkParams
)
{
    OBJGPU   *pGpu   = ENG_GET_GPU(pKernelGmmu);
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU8     *pInstBlk;      // CPU VA of instance block.
    NvU64     vaLimitData;
    NvU32     vaLimitOffset;
    NvU32     dirBaseHiOffset;
    NvU32     dirBaseHiData;
    NvU32     dirBaseLoOffset;
    NvU32     dirBaseLoData;
    NvU32     atsOffset;
    NvU32     atsData;
    NV_STATUS status = NV_OK;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    // Get VA limit
    status = kgmmuInstBlkVaLimitGet_HAL(pKernelGmmu, pVAS, subctxId, pInstBlkParams, &vaLimitOffset, &vaLimitData);
    NV_ASSERT_OR_RETURN((status == NV_OK), status);

    // Get page dir base
    NV_ASSERT_OK_OR_RETURN(kgmmuInstBlkPageDirBaseGet_HAL(pGpu, pKernelGmmu,
        pVAS, pInstBlkParams, subctxId,
        &dirBaseLoOffset, &dirBaseLoData, &dirBaseHiOffset, &dirBaseHiData));

    if ((pVAS != NULL) && vaspaceIsAtsEnabled(pVAS))
    {
        // Coherent link ATS parameters are only set on the new VMM path.
        status = kgmmuInstBlkAtsGet_HAL(pKernelGmmu, pVAS, subctxId, &atsOffset, &atsData);
        NV_ASSERT_OR_RETURN((status == NV_OK), status);
    }
    else
    {
        atsOffset = 0;
        atsData = 0;
    }
    // Write the fields out
    pInstBlk = pInstBlkParams->pInstBlk;

    if (pInstBlk != NULL)
    {
        if (vaLimitOffset != 0)
        {
            // TO DO: FMODEL fails with MEM_WR64
            if (IS_SIMULATION(pGpu))
            {
                MEM_WR32(pInstBlk + vaLimitOffset + 0, NvU64_LO32(vaLimitData));
                MEM_WR32(pInstBlk + vaLimitOffset + 4, NvU64_HI32(vaLimitData));
            }
            else
            {
                MEM_WR64(pInstBlk + vaLimitOffset, vaLimitData);
            }
        }

        MEM_WR32(pInstBlk + dirBaseHiOffset, dirBaseHiData);
        MEM_WR32(pInstBlk + dirBaseLoOffset, dirBaseLoData);

        if (atsOffset != 0)
            MEM_WR32(pInstBlk + atsOffset, atsData);
    }
    else
    {
        pInstBlk = kbusMapRmAperture_HAL(pGpu, pInstBlkDesc);
        if (pInstBlk == NULL)
        {
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        if (vaLimitOffset != 0)
        {
            // TO DO: FMODEL fails with MEM_WR64
            if (IS_SIMULATION(pGpu))
            {
                MEM_WR32(pInstBlk + vaLimitOffset + 0, NvU64_LO32(vaLimitData));
                MEM_WR32(pInstBlk + vaLimitOffset + 4, NvU64_HI32(vaLimitData));
            }
            else
            {
                MEM_WR64(pInstBlk + vaLimitOffset, vaLimitData);
            }
        }

        MEM_WR32(pInstBlk + dirBaseHiOffset, dirBaseHiData);
        MEM_WR32(pInstBlk + dirBaseLoOffset, dirBaseLoData);

        if (atsOffset != 0)
            MEM_WR32(pInstBlk + atsOffset, atsData);

        kbusUnmapRmAperture_HAL(pGpu, pInstBlkDesc, &pInstBlk, NV_FALSE);
    }

    if (!pInstBlkParams->bDeferFlush)
    {
        kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_USE_PCIE_READ
                                        | kbusGetFlushAperture(pKernelBus, memdescGetAddressSpace(pInstBlkDesc)));
    }

    return status;
}

GMMU_APERTURE
kgmmuGetExternalAllocAperture_IMPL
(
    NvU32 addressSpace
)
{
    switch (addressSpace)
    {
        case ADDR_FBMEM:
            return GMMU_APERTURE_VIDEO;
        case ADDR_FABRIC_V2:
        case ADDR_FABRIC_MC:
            return GMMU_APERTURE_PEER;
        case ADDR_SYSMEM:
        case ADDR_VIRTUAL:
            return GMMU_APERTURE_SYS_COH;
        default:
            NV_PRINTF(LEVEL_ERROR, "Unexpected addressSpace (%u) when mapping to GMMU_APERTURE.\n",
                      addressSpace);
            NV_ASSERT(0);
            return GMMU_APERTURE_SYS_COH;
    }
}

/*!
 * @brief
 *
 * @param pGpu
 * @param pKernelGmmu
 * @param bOwnedByRm
 */
void
kgmmuAccessCntrChangeIntrOwnership_IMPL
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvBool      bOwnedByRm
)
{
    //
    // Disable the interrupt when RM loses the ownership and enable it back when
    // RM regains it. nvUvmInterfaceOwnAccessCntIntr() will rely on this behavior.
    //
    if (bOwnedByRm)
        pKernelGmmu->uvmSharedIntrRmOwnsMask |= RM_UVM_SHARED_INTR_MASK_HUB_ACCESS_COUNTER_NOTIFY;
    else
        pKernelGmmu->uvmSharedIntrRmOwnsMask &= ~RM_UVM_SHARED_INTR_MASK_HUB_ACCESS_COUNTER_NOTIFY;
}

/**
 * @brief Provides an opportunity to register some IntrService during intrStateInit.
 */
void
kgmmuRegisterIntrService_IMPL
(
    OBJGPU              *pGpu,
    KernelGmmu          *pKernelGmmu,
    IntrServiceRecord   pRecords[MC_ENGINE_IDX_MAX]
)
{
    NvU32 engineIdx;

    static NvU16 engineIdxList[] = {
        MC_ENGINE_IDX_REPLAYABLE_FAULT,
        MC_ENGINE_IDX_REPLAYABLE_FAULT_ERROR,
    };

    for (NvU32 tableIdx = 0; tableIdx < NV_ARRAY_ELEMENTS32(engineIdxList); tableIdx++)
    {
        engineIdx = engineIdxList[tableIdx];
        NV_ASSERT(pRecords[engineIdx].pInterruptService == NULL);
        pRecords[engineIdx].pInterruptService = staticCast(pKernelGmmu, IntrService);
    }
}

/**
 * @brief Service stall interrupts.
 *
 * @returns Zero, or any implementation-chosen nonzero value. If the same nonzero value is returned enough
 *          times the interrupt is considered stuck.
 */
NvU32
kgmmuServiceInterrupt_IMPL
(
    OBJGPU      *pGpu,
    KernelGmmu  *pKernelGmmu,
    IntrServiceServiceInterruptArguments *pParams
)
{
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pParams != NULL, 0);

    switch (pParams->engineIdx)
    {
        case MC_ENGINE_IDX_REPLAYABLE_FAULT:
        {
            NV_STATUS status = kgmmuServiceReplayableFault_HAL(pGpu, pKernelGmmu);
            if (status != NV_OK)
            {
                NV_ASSERT_OK_FAILED("Failed to service replayable MMU fault error",
                    status);
            }
            break;
        }
        case MC_ENGINE_IDX_REPLAYABLE_FAULT_ERROR:
        {
            status = kgmmuReportFaultBufferOverflow_HAL(pGpu, pKernelGmmu);
            if (status != NV_OK)
            {
                NV_ASSERT_OK_FAILED(
                    "Failed to report replayable MMU fault buffer overflow error",
                    status);
            }
            break;
        }
        default:
        {
            NV_ASSERT_FAILED("Invalid engineIdx");
            break;
        }
    }

    return 0;
}

/*!
 * @brief Extract the PTE FIELDS from the PTE and
 * set the corresponding flags/fields in pParams.
 *
 * @param[in]  pKernelGmmu
 * @param[in]  pPte        Pointer to the PTE contents
 * @param[out] pPteInfo    Pointer to the PTE info structure
 * @param[in]  pFmt        NV0080_CTRL_DMA_PTE_INFO_PTE_BLOCK pointer to cmd params
 * @param[in]  pLevelFmt   Format of the level
 *
 *
 * @returns none
 */
void
kgmmuExtractPteInfo_IMPL
(
    KernelGmmu                          *pKernelGmmu,
    GMMU_ENTRY_VALUE                    *pPte,
    NV0080_CTRL_DMA_PTE_INFO_PTE_BLOCK  *pPteInfo,
    const GMMU_FMT                      *pFmt,
    const MMU_FMT_LEVEL                 *pLevelFmt
)
{
    OBJGPU             *pGpu = ENG_GET_GPU(pKernelGmmu);
    MemoryManager      *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    const GMMU_FMT_PTE *pFmtPte = pFmt->pPte;
    NvBool              bPteValid;

    bPteValid = nvFieldGetBool(&pFmtPte->fldValid, pPte->v8);

    pPteInfo->pteFlags = FLD_SET_DRF_NUM(0080_CTRL, _DMA_PTE_INFO, _PARAMS_FLAGS_VALID,
        bPteValid, pPteInfo->pteFlags);

    if (pFmtPte->version != GMMU_FMT_VERSION_3)
    {
        pPteInfo->pteFlags = FLD_SET_DRF_NUM(0080_CTRL, _DMA_PTE_INFO, _PARAMS_FLAGS_ENCRYPTED,
            nvFieldGetBool(&pFmtPte->fldEncrypted, pPte->v8), pPteInfo->pteFlags);
    }

    switch (gmmuFieldGetAperture(&pFmtPte->fldAperture, pPte->v8))
    {
        case GMMU_APERTURE_VIDEO:
            pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO, _PARAMS_FLAGS_APERTURE,
                _VIDEO_MEMORY, pPteInfo->pteFlags);
            break;
        case GMMU_APERTURE_PEER:
            pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO, _PARAMS_FLAGS_APERTURE,
                _PEER_MEMORY, pPteInfo->pteFlags);
            break;
        case GMMU_APERTURE_SYS_COH:
            pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO, _PARAMS_FLAGS_APERTURE,
                _SYSTEM_COHERENT_MEMORY, pPteInfo->pteFlags);
            break;
        case GMMU_APERTURE_SYS_NONCOH:
            pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO, _PARAMS_FLAGS_APERTURE,
                _SYSTEM_NON_COHERENT_MEMORY, pPteInfo->pteFlags);
            break;
        case GMMU_APERTURE_INVALID:
        default:
            NV_ASSERT(0);
            break;
    }

    if (pFmtPte->version == GMMU_FMT_VERSION_3)
    {
        KernelGmmu  *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
        NvU32        ptePcfHw;
        NvU32        ptePcfSw = 0;

        // In Version 3, parse the PCF bits and return those
        ptePcfHw = nvFieldGet32(&pFmtPte->fldPtePcf, pPte->v8);
        NV_ASSERT(kgmmuTranslatePtePcfFromHw_HAL(pKernelGmmu, ptePcfHw, bPteValid, &ptePcfSw) == NV_OK);

        // Valid 2MB PTEs follow the same format as 64K and 4K PTEs
        if (bPteValid)
        {
            if (!(ptePcfSw & (1 << SW_MMU_PCF_UNCACHED_IDX)))
            {
                pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO,
                        _PARAMS_FLAGS_GPU_CACHED, _TRUE, pPteInfo->pteFlags);
            }
            if (ptePcfSw & (1 << SW_MMU_PCF_RO_IDX))
            {
                pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO,
                        _PARAMS_FLAGS_READ_ONLY, _TRUE, pPteInfo->pteFlags);
            }
            if (ptePcfSw & (1 << SW_MMU_PCF_NOATOMIC_IDX))
            {
                pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO,
                        _PARAMS_FLAGS_ATOMIC, _DISABLE, pPteInfo->pteFlags);
            }
            if (ptePcfSw & (1 << SW_MMU_PCF_REGULAR_IDX))
            {
                pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO,
                        _PARAMS_FLAGS_PRIVILEGED, _FALSE, pPteInfo->pteFlags);
            }
            if (ptePcfSw & (1 << SW_MMU_PCF_ACE_IDX))
            {
                pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO,
                        _PARAMS_FLAGS_ACCESS_COUNTING, _ENABLE, pPteInfo->pteFlags);
            }
        }
        else
        {
            if (pLevelFmt->numSubLevels == 0)
            {
                if (ptePcfSw & (1 << SW_MMU_PCF_SPARSE_IDX))
                {
                    pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO,
                            _PARAMS_FLAGS_GPU_CACHED, _FALSE, pPteInfo->pteFlags);
                }
                else
                {
                    pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO,
                            _PARAMS_FLAGS_GPU_CACHED, _TRUE, pPteInfo->pteFlags);
                }
            }
            else
            {
                NvU32  pdePcfHw = 0;
                NvU32  pdePcfSw = 0;

                pdePcfHw = nvFieldGet32(&pFmt->pPde->fldPdePcf, pPte->v8);
                NV_ASSERT(kgmmuTranslatePdePcfFromHw_HAL(pKernelGmmu, pdePcfHw, GMMU_APERTURE_INVALID, &pdePcfSw) == NV_OK);
                if (pdePcfSw & (1 << SW_MMU_PCF_SPARSE_IDX))
                {
                    pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO,
                            _PARAMS_FLAGS_GPU_CACHED, _FALSE, pPteInfo->pteFlags);
                }
                else
                {
                    pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO,
                            _PARAMS_FLAGS_GPU_CACHED, _TRUE, pPteInfo->pteFlags);
                }

            }
        }
    }
    else
    {
        pPteInfo->pteFlags = FLD_SET_DRF_NUM(0080_CTRL, _DMA_PTE_INFO, _PARAMS_FLAGS_GPU_CACHED,
            !nvFieldGetBool(&pFmtPte->fldVolatile, pPte->v8), pPteInfo->pteFlags);

        if (nvFieldIsValid32(&pFmtPte->fldReadDisable.desc) &&
            nvFieldIsValid32(&pFmtPte->fldWriteDisable.desc))
        {
            if (nvFieldGetBool(&pFmtPte->fldWriteDisable, pPte->v8))
            {
                pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO,
                        _PARAMS_FLAGS_SHADER_ACCESS, _READ_ONLY, pPteInfo->pteFlags);
            }
            else if (nvFieldGetBool(&pFmtPte->fldReadDisable, pPte->v8))
            {
                pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO,
                        _PARAMS_FLAGS_SHADER_ACCESS, _WRITE_ONLY, pPteInfo->pteFlags);
            }
            else
            {
                pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO,
                        _PARAMS_FLAGS_SHADER_ACCESS, _READ_WRITE, pPteInfo->pteFlags);
            }
        }
        else
        {
            pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO, _PARAMS_FLAGS_SHADER_ACCESS,
                _NOT_SUPPORTED, pPteInfo->pteFlags);
        }

        pPteInfo->pteFlags = FLD_SET_DRF_NUM(0080_CTRL, _DMA_PTE_INFO, _PARAMS_FLAGS_READ_ONLY,
            nvFieldGetBool(&pFmtPte->fldReadOnly, pPte->v8), pPteInfo->pteFlags);

        // Get comptagline
        pPteInfo->comptagLine = nvFieldGet32(&pFmtPte->fldCompTagLine, pPte->v8);
    }

    // Get kind
    pPteInfo->kind = nvFieldGet32(&pFmtPte->fldKind, pPte->v8);

    //
    // Decode the comptags value from kind.  GF100 only supports 2 bits per rop tile,
    // but future chips will use the other layouts.
    //
    if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE_1, pPteInfo->kind))
    {
        pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO, _PARAMS_FLAGS_COMPTAGS, _1, pPteInfo->pteFlags);
    }
    else if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE_2, pPteInfo->kind))
    {
        pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO, _PARAMS_FLAGS_COMPTAGS, _2, pPteInfo->pteFlags);
    }
    else if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE_4, pPteInfo->kind))
    {
        pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO, _PARAMS_FLAGS_COMPTAGS, _4, pPteInfo->pteFlags);
    }
    else
    {
        pPteInfo->pteFlags = FLD_SET_DRF(0080_CTRL, _DMA_PTE_INFO, _PARAMS_FLAGS_COMPTAGS, _NONE, pPteInfo->pteFlags);
    }
}

NvS32*
kgmmuGetFatalFaultIntrPendingState_IMPL
(
    KernelGmmu *pKernelGmmu,
    NvU8 gfid
)
{
    return &pKernelGmmu->mmuFaultBuffer[gfid].fatalFaultIntrPending;
}

struct HW_FAULT_BUFFER*
kgmmuGetHwFaultBufferPtr_IMPL
(
    KernelGmmu *pKernelGmmu,
    NvU8 gfid,
    NvU8 faultBufferIndex
)
{
    return &pKernelGmmu->mmuFaultBuffer[gfid].hwFaultBuffers[faultBufferIndex];
}
