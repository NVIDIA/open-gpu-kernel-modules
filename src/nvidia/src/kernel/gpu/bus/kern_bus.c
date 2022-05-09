/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "mem_mgr/vaspace.h"
#include "mem_mgr/io_vaspace.h"
#include "mem_mgr/gpu_vaspace.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/bus/kern_bus.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/mem_sys/kern_mem_sys.h"
#include "platform/chipset/chipset.h"
#include "rmapi/client.h"

#include "gpu/subdevice/subdevice.h"
#include "gpu/gsp/gsp_static_config.h"
#include "vgpu/rpc.h"

#include "nvRmReg.h"

static NV_STATUS kbusInitRegistryOverrides(OBJGPU *pGpu, KernelBus *pKernelBus);

NV_STATUS
kbusConstructEngine_IMPL(OBJGPU *pGpu, KernelBus *pKernelBus, ENGDESCRIPTOR engDesc)
{
    NV_STATUS  status;

    if (IsAMPEREorBetter(pGpu) && pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM))
    {
        pKernelBus->bBar1PhysicalModeEnabled = NV_TRUE;
    }

    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        pKernelBus->bUsePhysicalBar2InitPagetable = NV_TRUE;
    }

    // allocate HAL private info block
    status = kbusConstructHal_HAL(pGpu, pKernelBus);
    if (status != NV_OK)
        return status;

    kbusInitRegistryOverrides(pGpu, pKernelBus);

    kbusInitPciBars_HAL(pKernelBus);

    // Special handle for VGPU.  WAR for bug 3458057, bug 3458029
    if (IS_VIRTUAL(pGpu))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kbusInitBarsSize_HAL(pGpu, pKernelBus));
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kbusInitBarsBaseInfo_HAL(pKernelBus));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kbusSetBarsApertureSize_HAL(pGpu, pKernelBus, GPU_GFID_PF));

    return NV_OK;
}

/*!
 * @brief Initialize all registry overrides for this object
 *
 * @param[in]      pGpu
 * @param[in,out]  pKernelBus
 */
static NV_STATUS
kbusInitRegistryOverrides(OBJGPU *pGpu, KernelBus *pKernelBus)
{
    NvU32 data32;

    switch (DRF_VAL(_REG_STR_RM, _INST_LOC, _BAR_PTE, pGpu->instLocOverrides))
    {
        default:
        case NV_REG_STR_RM_INST_LOC_BAR_PTE_DEFAULT:
            // Do not override on default..
            break;
        case NV_REG_STR_RM_INST_LOC_BAR_PTE_VID:
            pKernelBus->PTEBAR2Aperture = ADDR_FBMEM;
            pKernelBus->PTEBAR2Attr = NV_MEMORY_WRITECOMBINED;
            break;

        case NV_REG_STR_RM_INST_LOC_BAR_PTE_COH:
            if (gpuIsBarPteInSysmemSupported(pGpu) || !gpuIsRegUsesGlobalSurfaceOverridesEnabled(pGpu))
            {
                pKernelBus->PTEBAR2Aperture = ADDR_SYSMEM;
                pKernelBus->PTEBAR2Attr = NV_MEMORY_CACHED;
            }
            else
            {
                //
                // BAR PTEs in sysmem is not supported on all hardware.
                // HW bug 415430. Once fixed, this property will be set on supported GPUs.
                // On unsupported GPUs where the GlobalSurfaceOverrides regkey is used, show a warning and don't override.
                //
                NV_PRINTF(LEVEL_WARNING,
                          "BAR PTEs not supported in sysmem. Ignoring global override request.\n");
            }
            break;

        case NV_REG_STR_RM_INST_LOC_BAR_PTE_NCOH:
            if (gpuIsBarPteInSysmemSupported(pGpu) || !gpuIsRegUsesGlobalSurfaceOverridesEnabled(pGpu))
            {
                pKernelBus->PTEBAR2Aperture = ADDR_SYSMEM;
                pKernelBus->PTEBAR2Attr = NV_MEMORY_UNCACHED;
            }
            else
            {
                // BAR PTEs in sysmem is not supported on current hardware. See above.
                NV_PRINTF(LEVEL_WARNING,
                          "BAR PTEs not supported in sysmem. Ignoring global override request.\n");
            }
            break;
    }

    NV_PRINTF(LEVEL_INFO, "Using aperture %d for BAR2 PTEs\n",
              pKernelBus->PTEBAR2Aperture);

    switch (DRF_VAL(_REG_STR_RM, _INST_LOC, _BAR_PDE, pGpu->instLocOverrides))
    {
        default:
        case NV_REG_STR_RM_INST_LOC_BAR_PDE_DEFAULT:
            // Do not override on default.
            break;
        case NV_REG_STR_RM_INST_LOC_BAR_PDE_VID:
            pKernelBus->PDEBAR2Aperture = ADDR_FBMEM;
            pKernelBus->PDEBAR2Attr = NV_MEMORY_WRITECOMBINED;
            break;

        case NV_REG_STR_RM_INST_LOC_BAR_PDE_COH:
            if (gpuIsBarPteInSysmemSupported(pGpu) || !gpuIsRegUsesGlobalSurfaceOverridesEnabled(pGpu))
            {
                pKernelBus->PDEBAR2Aperture = ADDR_SYSMEM;
                pKernelBus->PDEBAR2Attr = NV_MEMORY_CACHED;
            }
            else
            {
                // BAR PDEs in sysmem is not supported on all hardware. See above.
                NV_PRINTF(LEVEL_WARNING,
                          "BAR PDEs not supported in sysmem. Ignoring global override request.\n");
            }
            break;

        case NV_REG_STR_RM_INST_LOC_BAR_PDE_NCOH:
            if (gpuIsBarPteInSysmemSupported(pGpu) || !gpuIsRegUsesGlobalSurfaceOverridesEnabled(pGpu))
            {
                pKernelBus->PDEBAR2Aperture = ADDR_SYSMEM;
                pKernelBus->PDEBAR2Attr = NV_MEMORY_UNCACHED;
            }
            else
            {
                // BAR PDEs in sysmem is not supported on all hardware. See above.
                NV_PRINTF(LEVEL_WARNING,
                          "BAR PDEs not supported in sysmem. Ignoring global override request.\n");
            }
            break;
    }

    if (RMCFG_FEATURE_PLATFORM_WINDOWS_LDDM && !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_TCC_MODE))
    {
        //
        // Aligns to unlinked SLI: Volta and up
        // Next: Plan for all GPUs after validation
        //
        pKernelBus->bP2pMailboxClientAllocated =
            pKernelBus->bP2pMailboxClientAllocatedBug3466714VoltaAndUp;
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_P2P_MAILBOX_CLIENT_ALLOCATED, &data32) == NV_OK)
    {
        pKernelBus->bP2pMailboxClientAllocated = !!data32;
    }

    return NV_OK;
}

/**
 * @brief  Gets the BAR1 VA range for a client
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 * @param[in] hClient               Client handle
 * @param[out] pBar1VARange         BAR1 VA range
 */

NV_STATUS
kbusGetBar1VARangeForClient_IMPL(OBJGPU *pGpu, KernelBus *pKernelBus, NvHandle hClient, NV_RANGE *pBar1VARange)
{
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    OBJVASPACE       *pBar1VAS          = kbusGetBar1VASpace_HAL(pGpu, pKernelBus);

    NV_ASSERT_OR_RETURN(pBar1VAS != NULL, NV_ERR_INVALID_STATE);

   *pBar1VARange = rangeMake(vaspaceGetVaStart(pBar1VAS), vaspaceGetVaLimit(pBar1VAS));

    if ((pKernelMIGManager != NULL) && kmigmgrIsMIGMemPartitioningEnabled(pGpu, pKernelMIGManager) &&
        !rmclientIsCapableByHandle(hClient, NV_RM_CAP_SYS_SMC_MONITOR) &&
        !kmigmgrIsClientUsingDeviceProfiling(pGpu, pKernelMIGManager, hClient))
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
        KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
        MIG_INSTANCE_REF ref;

       *pBar1VARange = memmgrGetMIGPartitionableBAR1Range(pGpu, pMemoryManager);

        NV_ASSERT_OK_OR_RETURN(kmigmgrGetInstanceRefFromClient(pGpu, pKernelMIGManager,
                                   hClient, &ref));
        NV_ASSERT_OK_OR_RETURN(kmemsysSwizzIdToMIGMemRange(pGpu, pKernelMemorySystem, ref.pKernelMIGGpuInstance->swizzId,
                                   *pBar1VARange, pBar1VARange));
    }
    return NV_OK;
}

RmPhysAddr
kbusSetupPeerBarAccess_IMPL
(
    OBJGPU *pLocalGpu,
    OBJGPU *pRemoteGpu,
    RmPhysAddr base,
    NvU64 size,
    PMEMORY_DESCRIPTOR *ppMemDesc
)
{
    NV_STATUS          status;
    MEMORY_DESCRIPTOR *pMemDesc = *ppMemDesc;
    IOVAMAPPING       *pIovaMapping;

    NV_ASSERT_OR_RETURN(((base & RM_PAGE_MASK) == 0), ~0ULL);

    if (pMemDesc == NULL)
    {
        status = memdescCreate(&pMemDesc, pLocalGpu, size, 0, NV_TRUE,
                               ADDR_SYSMEM, NV_MEMORY_UNCACHED,
                               MEMDESC_FLAGS_SKIP_RESOURCE_COMPUTE);
        NV_ASSERT_OR_RETURN(status == NV_OK, ~0ULL);

        memdescDescribe(pMemDesc, ADDR_SYSMEM, base, size);
    }
    else
    {
        NV_ASSERT_OR_RETURN(
            (memdescGetPhysAddr(pMemDesc, AT_GPU, 0) == base) &&
            (memdescGetSize(pMemDesc) == size), ~0ULL);
    }

    //
    // Even if IOMMU-remapping fails (which it shouldn't), try to continue
    // using the CPU physical address. In most cases, this is still sufficient.
    //
    status = memdescMapIommu(pMemDesc, pRemoteGpu->busInfo.iovaspaceId);
    NV_ASSERT(status == NV_OK);

    pIovaMapping = memdescGetIommuMap(pMemDesc, pRemoteGpu->busInfo.iovaspaceId);

    *ppMemDesc = pMemDesc;

    if (pIovaMapping == NULL)
    {
        NV_PRINTF(LEVEL_INFO,
                  "no IOVA mapping found for pre-existing P2P domain memdesc\n");
        return memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
    }

    return pIovaMapping->iovaArray[0];
}

/*!
 *  @brief Get the bus flush aperture flag for the NV_ADDRESS_SPACE
 *         For use with the kbusFlush_HAL() api
 *
 *  @param[in]  addrSpace         NV_ADDRESS_SPACE
 *
 *  @returns bush flush aperture flag
 */
NvU32 kbusGetFlushAperture_IMPL(KernelBus *pKernelBus, NV_ADDRESS_SPACE addrSpace)
{
    return (addrSpace == ADDR_FBMEM) ? BUS_FLUSH_VIDEO_MEMORY : BUS_FLUSH_SYSTEM_MEMORY;
}

void
kbusDestruct_IMPL(KernelBus *pKernelBus)
{
    OBJGPU *pGpu = ENG_GET_GPU(pKernelBus);

    //
    // We need to clean-up the memory resources for BAR2 as late as possible,
    // and after all memory descriptors have been reclaimed.
    //
    kbusDestructVirtualBar2_HAL(pGpu, pKernelBus, NV_TRUE, GPU_GFID_PF);

    return;
}

/*! Send sysmembar to all sub-devices */
NV_STATUS
kbusSendSysmembar_IMPL
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus
)
{
    NV_STATUS   status  = NV_OK;

    // Nothing to be done in guest in the paravirtualization case.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return NV_OK;
    }

    if (kbusIsFbFlushDisabled(pKernelBus))
    {
        // Eliminate FB flushes, but keep mmu invalidates
        NV_PRINTF(LEVEL_INFO, "disable_fb_flush flag, skipping flush.\n");
        return NV_OK;
    }

    // Wait for the flush to flow through
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY);
        pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
        if (kbusSendSysmembarSingle_HAL(pGpu, pKernelBus) == NV_ERR_TIMEOUT)
        {
            status = NV_ERR_TIMEOUT;
        }
    SLI_LOOP_END;
    pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    return status;
}

/**
 * @brief Send sysmembar to a single sub-devices
 *        Trigger RPC to Physical RM.
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 */
NV_STATUS
kbusSendSysmembarSingle_KERNEL
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS  status;

    status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_BUS_FLUSH_WITH_SYSMEMBAR,
                                NULL, 0);

    return status;
}

/*!
 * @brief Commit BAR2
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 * @param[in] flags         GPU state flag (not used by Kernel RM)
 *
 * @returns NV_OK on success.
 */
NV_STATUS
kbusCommitBar2_KERNEL
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      flags
)
{
    // we will initialize bar2 to the default big page size of the system
    NV_ASSERT_OK_OR_RETURN(kbusInitVirtualBar2_HAL(pGpu, pKernelBus));
    NV_ASSERT_OK_OR_RETURN(kbusSetupCpuPointerForBusFlush_HAL(pGpu, pKernelBus));

    return NV_OK;
}

/*! Get pci bar size in BYTE */
NvU64
kbusGetPciBarSize_IMPL(KernelBus *pKernelBus, NvU32 index)
{
    if (index >= pKernelBus->totalPciBars)
    {
        NV_PRINTF(LEVEL_ERROR, "bad index 0x%x\n", index);
        return 0;
    }

    return pKernelBus->pciBarSizes[index];
}

RmPhysAddr
kbusGetPciBarOffset_IMPL(KernelBus *pKernelBus, NvU32 index)
{
    RmPhysAddr offset = 0x0;

    if (index < pKernelBus->totalPciBars)
    {
        offset = pKernelBus->pciBars[index];
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "bad index 0x%x\n", index);
    }

    return offset;
}

/**
 * @brief Determine bBar1Force64KBMapping base on regkey and bar1 size
 *   Determine if 64KB mappings need to be forced based on total BAR1 size.
 *   Default threshold is 256MB unless overridden by regkey
 *   Force 64KB for SKUs with BAR1 size <= 256MB
 *
 * @param[in] pKernelBus
 */
void
kbusDetermineBar1Force64KBMapping_IMPL
(
    KernelBus *pKernelBus
)
{
    OBJGPU*   pGpu = ENG_GET_GPU(pKernelBus);
    NvU32     data;

    pKernelBus->bBar1Force64KBMapping = NV_TRUE;

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_64KB_BAR1_MAPPINGS,
                            &data) == NV_OK)
    {
        if (data == NV_REG_STR_RM_64KB_BAR1_MAPPINGS_DISABLED)
        {
            pKernelBus->bBar1Force64KBMapping = NV_FALSE;
        }
    }
    else
    {
        NvU32   bar1SizeMB;
        bar1SizeMB = (NvU32)(kbusGetPciBarSize(pKernelBus, 1) >> 20);

        if (bar1SizeMB > 256)
        {
            pKernelBus->bBar1Force64KBMapping = NV_FALSE;
        }
    }
}


/**
 * @brief Determine bar1[gfid].apertureLength base on regkey and bar1 size
 *
 * @param[in] pKernelBus
 * @param[in] gfid
 */
void
kbusDetermineBar1ApertureLength_IMPL
(
    KernelBus *pKernelBus,
    NvU32      gfid
)
{
    OBJGPU   *pGpu = ENG_GET_GPU(pKernelBus);
    NvU32     data32;

    if (IS_GFID_VF(gfid))
    {
        pKernelBus->bar1[gfid].apertureLength = pGpu->sriovState.vfBarSize[1];
    }
    else
    {
        pKernelBus->bar1[gfid].apertureLength = kbusGetPciBarSize(pKernelBus, 1);
    }

    // We can shrink BAR1 using this reg key but cannot grow it.
    if (((NV_OK == osReadRegistryDword(pGpu,
                        NV_REG_STR_RM_BAR1_APERTURE_SIZE_MB, &data32))) &&
            data32 && (((NvU64)data32 << 20) < pKernelBus->bar1[gfid].apertureLength))
    {
        // Set BAR1 aperture length based on the override
        pKernelBus->bar1[gfid].apertureLength = (NvU64) data32 << 20;
    }

}

/*!
 * @brief Initialize pciBarSizes[], set pKernelBus->bPciBarSizesValid
 *        Trigger an internal RMAPI to get the data from Physical RM.
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 */
NV_STATUS
kbusInitBarsSize_KERNEL
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS params;
    NvU32 i;

    NV_ASSERT( ! pKernelBus->bPciBarSizesValid);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_BUS_GET_PCI_BAR_INFO,
                        &params, sizeof(params)));

    for (i = 0; i< params.pciBarCount; i++)
    {
        pKernelBus->pciBarSizes[i] = params.pciBarInfo[i].barSizeBytes;
    }

    pKernelBus->bPciBarSizesValid = NV_TRUE;

    return NV_OK;
}

/*!
 * @brief Remove P2P mapping to a given peer GPU
 *
 * @param[in]   pGpu0         (Local)
 * @param[in]   pKernelBus0   (Local)
 * @param[in]   pGpu1         (Remote)
 * @param[in]   peerIdx
 *
 * return NV_OK on success
 */
void
kbusDestroyMailbox_IMPL
(
    OBJGPU      *pGpu0,
    KernelBus   *pKernelBus0,
    OBJGPU      *pGpu1,
    NvU32        peerIdx
)
{
    RM_API *pRmApi  = GPU_GET_PHYSICAL_RMAPI(pGpu0);
    NvBool  bNeedWarBug999673 = kbusNeedWarForBug999673_HAL(pGpu0, pKernelBus0, pGpu1) ||
                                kbusNeedWarForBug999673_HAL(pGpu1, GPU_GET_KERNEL_BUS(pGpu1), pGpu0);
    NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS busParams   = {0};
    NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS  hshubParams = {0};
    NV_STATUS status;

    kbusDestroyPeerAccess_HAL(pGpu0, pKernelBus0, peerIdx);

    busParams.peerIdx           = peerIdx;
    busParams.bNeedWarBug999673 = bNeedWarBug999673;
    status = pRmApi->Control(pRmApi, pGpu0->hInternalClient, pGpu0->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_BUS_DESTROY_P2P_MAILBOX,
                             &busParams, sizeof(busParams));
    NV_ASSERT(status == NV_OK);

    // Create a peer mask for each peer to program their respective peer_connection_cfg registers
    hshubParams.invalidatePeerMask = NVBIT32(peerIdx);
    // Program connection_cfg registers
    status = pRmApi->Control(pRmApi, pGpu0->hInternalClient, pGpu0->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_HSHUB_PEER_CONN_CONFIG,
                             &hshubParams, sizeof(hshubParams));
    NV_ASSERT(status == NV_OK);
}

NvU8 *
kbusCpuOffsetInBar2WindowGet_IMPL
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_ASSERT_OR_RETURN(NULL != pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping, NULL);
    NV_ASSERT_OR_RETURN(ADDR_FBMEM == pMemDesc->_addressSpace, NULL);

    return (NvU8 *)(pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping +
                    memdescGetPhysAddr(pMemDesc, AT_GPU, 0));
}

/*!
 * Calculates the BAR2 VA limit (in Byte units) which usually means the
 * cpuVisible area limit in CPU-RM.  Can be safely called only after
 * kbusSetBarsApertureSize_HAL is executed.
 *
 * @param   pGpu
 * @param   pKernelBus
 *
 * @return VA limit of BAR2
 */
NvU64
kbusGetVaLimitForBar2_KERNEL
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NvU64 limit = pKernelBus->bar2[GPU_GFID_PF].cpuVisibleLimit;

    NV_PRINTF(LEVEL_INFO, "va limit: 0x%llx\n", limit);

    //
    // pKernelBus->bar2.vaLimit is set by this function.
    // Assert to ensure that this value doesn't get changed.
    //
    NV_ASSERT(pKernelBus->bar2[GPU_GFID_PF].vaLimit == 0 || pKernelBus->bar2[GPU_GFID_PF].vaLimit == limit);

    return limit;
}

/*!
 * Patch CPU-RM's SW cache of BAR1 PDB to GSP-RM's BAR1 PDB so that CPU-RM can
 * do TLB invalidation to correct VA space.
 *
 * @param   pGpu
 * @param   pKernelBus
 *
 * @return NV_OK if PDB is updated successfully
 *         Or bubble up the error code returned by the callees
 */
NV_STATUS
kbusPatchBar1Pdb_GSPCLIENT
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus
)
{
    NV_STATUS            status    = NV_OK;
    OBJGVASPACE         *pGVAS     = dynamicCast(pKernelBus->bar1[GPU_GFID_PF].pVAS, OBJGVASPACE);
    KernelGmmu          *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    MEMORY_DESCRIPTOR   *pMemDesc  = NULL;
    GVAS_GPU_STATE      *pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    const MMU_FMT_LEVEL *pRootFmt  = pGpuState->pFmt->pRoot;
    NvU32                rootSize  = pRootFmt->entrySize;
    MMU_WALK_USER_CTX    userCtx   = {0};
    GspStaticConfigInfo *pGSCI     = GPU_GET_GSP_STATIC_INFO(pGpu);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
         memdescCreate(&pMemDesc, pGpu, rootSize, RM_PAGE_SIZE, NV_TRUE, ADDR_FBMEM,
                       kgmmuGetPTEAttr(pKernelGmmu), MEMDESC_FLAGS_NONE));

    memdescDescribe(pMemDesc, ADDR_FBMEM, pGSCI->bar1PdeBase, rootSize);
    memdescSetPageSize(pMemDesc, VAS_ADDRESS_TRANSLATION(pKernelBus->bar1[GPU_GFID_PF].pVAS), RM_PAGE_SIZE);

    gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx);

    //
    // Modify the CPU-RM's walker state with the new backing memory.
    // This is intended to replace CPU-RM's PDB by GSP-RM's PDB.
    //
    status = mmuWalkModifyLevelInstance(pGpuState->pWalk,
                                        pRootFmt,
                                        vaspaceGetVaStart(pKernelBus->bar1[GPU_GFID_PF].pVAS),
                                        (MMU_WALK_MEMDESC*)pMemDesc,
                                        mmuFmtLevelSize(pRootFmt),
                                        NV_TRUE,
                                        NV_TRUE,
                                        NV_FALSE);
    gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
    if (NV_OK != status)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to modify CPU-RM's BAR1 PDB to GSP-RM's BAR1 PDB.\n");
        return status;
    }

    gvaspaceInvalidateTlb(pGVAS, pGpu, PTE_DOWNGRADE);

    return status;
}

/*!
 * Patch CPU-RM's SW cache of BAR2 PDB to GSP-RM's BAR2 PDB so that CPU-RM can
 * do TLB invalidation to correct VA space.
 *
 * For the BAR2 support in RM-offload model, CPU-RM owns the VA range under
 * PDE3[0] and GSP-RM owns the VA range under PDE3[1]. GSP-RM and CPU-RM
 * establish their own BAR2 page tables respectively. After CPU-RM establishes
 * its own table, it passes its PDE3[0] value to GSP-RM, then GSP-RM will fill
 * this value to PDE3[0] of GSP-RM's table (only GSP-RM's BAR2 table will be
 * bound to HW) so that HW sees single BAR2 page table for both GSP-RM and
 * CPU-RM.
 *
 * @param   pGpu
 * @param   pKernelBus
 *
 * @return NV_OK if PDB is updated successfully
 *         Or bubble up the error code returned by the callees
 */
NV_STATUS
kbusPatchBar2Pdb_GSPCLIENT
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus
)
{
    NV_STATUS            status = NV_OK;
    PMEMORY_DESCRIPTOR   pMemDesc;
    GspStaticConfigInfo *pGSCI  = GPU_GET_GSP_STATIC_INFO(pGpu);
    NvU64                entryValue;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        memdescCreate(&pMemDesc, pGpu, pKernelBus->bar2[GPU_GFID_PF].pageDirSize, RM_PAGE_SIZE, NV_TRUE,
                      ADDR_FBMEM, pKernelBus->PDEBAR2Attr, MEMDESC_FLAGS_NONE));

    memdescDescribe(pMemDesc, ADDR_FBMEM, pGSCI->bar2PdeBase, pKernelBus->bar2[GPU_GFID_PF].pageDirSize);

    // Update CPU-RM's SW cache of PDB to GSP-RM's PDB address
    pKernelBus->virtualBar2[GPU_GFID_PF].pPDB = pMemDesc;

    //
    // BAR2 page table is not yet working at this point, so retrieving the
    // PDE3[0] of BAR2 page table via BAR0_WINDOW
    //
    entryValue = GPU_REG_RD32(pGpu, (NvU32)pKernelBus->bar2[GPU_GFID_PF].bar2OffsetInBar0Window) |
                 ((NvU64)GPU_REG_RD32(pGpu, (NvU32)pKernelBus->bar2[GPU_GFID_PF].bar2OffsetInBar0Window + 4) << 32);

    //
    // Provide the PDE3[0] value to GSP-RM so that GSP-RM can merge CPU-RM's
    // page table to GSP-RM's page table
    //
    NV_RM_RPC_UPDATE_BAR_PDE(pGpu, NV_RPC_UPDATE_PDE_BAR_2, entryValue, pKernelBus->bar2[GPU_GFID_PF].pFmt->pRoot->virtAddrBitLo, status);

    return NV_OK;
}

/*!
 * @brief Helper function to trigger RPC to Physical RM to unbind FLA VASpace
 *
 * @param[in]  pGpu
 * @param[in]  pKernelBus
 *
 * @return NV_OK if successful
 */
NV_STATUS
kbusSetupUnbindFla_KERNEL
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS params = { 0 };

    if (!pKernelBus->flaInfo.bFlaBind)
        return NV_OK;

    params.flaAction = NV2080_CTRL_FLA_ACTION_UNBIND;

    NV_RM_RPC_CONTROL(pGpu, pKernelBus->flaInfo.hClient,
                      pKernelBus->flaInfo.hSubDevice,
                      NV2080_CTRL_CMD_FLA_SETUP_INSTANCE_MEM_BLOCK,
                      &params, sizeof(params), status);

    pKernelBus->flaInfo.bFlaBind = NV_FALSE;
    pKernelBus->bFlaEnabled      = NV_FALSE;

    return status;
}

/*!
 * @brief Helper function to extract information from FLA data structure and
 *        to trigger RPC to Physical RM to BIND FLA VASpace
 *
 * @param[in]  pGpu
 * @param[in]  pKernelBus
 * @param[in]  gfid     GFID
 *
 * @return NV_OK if successful
 */
NV_STATUS
kbusSetupBindFla_KERNEL
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid
)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS params = {0};

    if (!gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        MEMORY_DESCRIPTOR  *pMemDesc;
        RmPhysAddr          imbPhysAddr;
        NvU32               addrSpace;

        pMemDesc     = pKernelBus->flaInfo.pInstblkMemDesc;
        imbPhysAddr  = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
        addrSpace    = memdescGetAddressSpace(pMemDesc);
        NV2080_CTRL_FLA_ADDRSPACE paramAddrSpace = NV2080_CTRL_FLA_ADDRSPACE_FBMEM;

        switch(addrSpace)
        {
            case ADDR_FBMEM:
                paramAddrSpace = NV2080_CTRL_FLA_ADDRSPACE_FBMEM;
                break;
            case ADDR_SYSMEM:
                paramAddrSpace = NV2080_CTRL_FLA_ADDRSPACE_SYSMEM;
                break;
        }
        params.imbPhysAddr = imbPhysAddr;
        params.addrSpace   = paramAddrSpace;
    }
    params.flaAction   = NV2080_CTRL_FLA_ACTION_BIND;
    NV_RM_RPC_CONTROL(pGpu, pKernelBus->flaInfo.hClient,
                        pKernelBus->flaInfo.hSubDevice,
                        NV2080_CTRL_CMD_FLA_SETUP_INSTANCE_MEM_BLOCK,
                        &params, sizeof(params), status);
    // Since FLA state is tracked in the Guest, Guest RM needs to set it here
    pKernelBus->flaInfo.bFlaBind = NV_TRUE;
    pKernelBus->bFlaEnabled      = NV_TRUE;

    return status;
}

/*!
 * @brief Checks whether an engine is available or not.
 *
 * The 'engine' is an engine descriptor
 * This function is different from busProbeRegister in a sense that it doesn't
 * rely on timeouts after a read of a register in the reg space for engine.
 * Instead, it
 *  - Return TRUE for all engines which are must present in GPU.
 *  - Get information about CE, MSENC, NVJPG and OFA engines from plugin.
 *  - Rest engines are determined from HAL creation data.
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelBus KernelBus pointer
 * @param[in] engDesc    ENGDESCRIPTOR pointer used to check Engine presence
 *
 * @returns NV_TRUE if engine is available.
 *          NV_FALSE if engine is not available or floorswept.
 *
 */
NvBool
kbusCheckEngine_KERNEL
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus,
    ENGDESCRIPTOR engDesc
)
{
    NvU64    engineList;
    NvBool   bSupported;

    if (!RMCFG_FEATURE_VIRTUALIZATION && !RMCFG_FEATURE_GSP_CLIENT_RM)
        return NV_TRUE;

    {
        GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
        if (pGSCI == NULL)
        {
            return NV_FALSE;
        }
        engineList = pGSCI->engineCaps;
    }

    switch (engDesc)
    {
        case ENG_LSFM:
        case ENG_PMU:
        case ENG_CLK:
        case ENG_ACR:
        case ENG_DISP:
            return NV_FALSE;
        //
        // This function is used in two environments:
        // (a) vGPU where display is not yet supported.
        // (b) RM offload (Kernel RM) where display is supported.
        //
        case ENG_KERNEL_DISPLAY:
            if (IS_GSP_CLIENT(pGpu))
                return NV_TRUE;
            else
                return NV_FALSE;

        case ENG_BIF:
        case ENG_KERNEL_BIF:
        case ENG_MC:
        case ENG_KERNEL_MC:
        case ENG_PRIV_RING:
        case ENG_SW_INTR:
        case ENG_TMR:
        case ENG_DMA:
        case ENG_BUS:
        case ENG_GR(0):
        case ENG_CIPHER:
        case ENG_INTR:
        case ENG_GPULOG:
        case ENG_GPUMON:
        case ENG_FIFO:
            return NV_TRUE;

        case ENG_CE(0):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_COPY0))) ? NV_TRUE: NV_FALSE);
        case ENG_CE(1):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_COPY1))) ? NV_TRUE: NV_FALSE);
        case ENG_CE(2):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_COPY2))) ? NV_TRUE: NV_FALSE);
        case ENG_CE(3):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_COPY3))) ? NV_TRUE: NV_FALSE);
        case ENG_CE(4):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_COPY4))) ? NV_TRUE: NV_FALSE);
        case ENG_CE(5):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_COPY5))) ? NV_TRUE: NV_FALSE);
        case ENG_CE(6):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_COPY6))) ? NV_TRUE: NV_FALSE);
        case ENG_CE(7):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_COPY7))) ? NV_TRUE: NV_FALSE);
        case ENG_CE(8):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_COPY8))) ? NV_TRUE: NV_FALSE);
        case ENG_CE(9):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_COPY9))) ? NV_TRUE: NV_FALSE);
        case ENG_MSENC(0):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_NVENC0))) ? NV_TRUE: NV_FALSE);
        case ENG_MSENC(1):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_NVENC1))) ? NV_TRUE: NV_FALSE);
        case ENG_MSENC(2):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_NVENC2))) ? NV_TRUE: NV_FALSE);
        case ENG_SEC2:
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_SEC2))) ? NV_TRUE: NV_FALSE);
        case ENG_NVDEC(0):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_NVDEC0))) ? NV_TRUE: NV_FALSE);
        case ENG_NVDEC(1):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_NVDEC1))) ? NV_TRUE: NV_FALSE);
        case ENG_NVDEC(2):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_NVDEC2))) ? NV_TRUE: NV_FALSE);
        case ENG_OFA:
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_OFA))) ? NV_TRUE: NV_FALSE);
        case ENG_NVDEC(3):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_NVDEC3))) ? NV_TRUE: NV_FALSE);
        case ENG_NVDEC(4):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_NVDEC4))) ? NV_TRUE: NV_FALSE);
        case ENG_NVJPEG(0):
                return ((engineList & (NVBIT64(NV2080_ENGINE_TYPE_NVJPEG0))) ? NV_TRUE: NV_FALSE);
        case ENG_GR(1):
        case ENG_GR(2):
        case ENG_GR(3):
        case ENG_GR(4):
        case ENG_GR(5):
        case ENG_GR(6):
        case ENG_GR(7):
        {
            KernelFifo *pKernelFifo  = GPU_GET_KERNEL_FIFO(pGpu);

            NV_ASSERT_OR_RETURN(pKernelFifo != NULL, NV_FALSE);

            if (kfifoCheckEngine_HAL(pGpu, pKernelFifo, engDesc, &bSupported) == NV_OK)
                return bSupported;
            else
                return NV_FALSE;
        }

        case ENG_INVALID:
            NV_PRINTF(LEVEL_ERROR,
                      "Query for ENG_INVALID considered erroneous: %d\n",
                      engDesc);
            return NV_TRUE;
        //
        // Check if engine descriptor is supported by current GPU.
        // Callee must not send engine descriptor which are not on
        // HAL lists of GPU. So Add ASSERT there.
        //
        default:
            bSupported = gpuIsEngDescSupported(pGpu, engDesc);

            if (!bSupported)
            {
                NV_PRINTF(LEVEL_ERROR, "Unable to check engine ID: %d\n",
                          engDesc);
                NV_ASSERT(bSupported);
            }
            return bSupported;
    }
}

//
// kbusGetDeviceCaps
//
// This routine gets cap bits in unicast. If bCapsInitialized is passed as
// NV_FALSE, the caps will be copied into pHostCaps without OR/ANDing. Otherwise,
// the caps bits for the current GPU will be OR/ANDed together with pHostCaps to
// create a single set of caps that accurately represents the functionality of
// the device.
//
void
kbusGetDeviceCaps_IMPL
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU8      *pHostCaps,
    NvBool     bCapsInitialized
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL  *pCl  = SYS_GET_CL(pSys);
    NvU8 tempCaps[NV0080_CTRL_HOST_CAPS_TBL_SIZE], temp;
    NvBool bVirtualP2P;
    NvBool bExplicitCacheFlushRequired;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    portMemSet(tempCaps, 0, NV0080_CTRL_HOST_CAPS_TBL_SIZE);

    /*! On KEPLER+, mailbox protocol based P2P transactions goes through virtual to
     *  physical translation (on request side) */
    bVirtualP2P = IsdMAXWELLorBetter(pGpu);
    if (bVirtualP2P)
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_HOST_CAPS, _VIRTUAL_P2P);

    /*! DMAs to/from cached memory need to have the cache flushed explicitly */
    bExplicitCacheFlushRequired = NVCPU_IS_ARM && 
                                  (RMCFG_FEATURE_PLATFORM_UNIX || RMCFG_FEATURE_PLATFORM_MODS_UNIX);
    if (bExplicitCacheFlushRequired ||
        (!pCl->getProperty(pCL, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT)))
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_HOST_CAPS, _EXPLICIT_CACHE_FLUSH_REQD);

    if ((pCl->FHBBusInfo.vendorID == PCI_VENDOR_ID_NVIDIA) &&
        ((pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_BR04_PRESENT)) ||
         ((pCl->FHBBusInfo.deviceID >= NVIDIA_C73_CPU_PCI_0_DEVICE_ID_SLI2) &&
          (pCl->FHBBusInfo.deviceID <= NVIDIA_C73_CPU_PCI_0_DEVICE_ID_RESERVED_3))))
    {
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_HOST_CAPS, _CPU_WRITE_WAR_BUG_420495);
    }

    // the RM always supports GPU-coherent mappings
    RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_HOST_CAPS, _GPU_COHERENT_MAPPING_SUPPORTED);

    // If we don't have existing caps with which to reconcile, then just return
    if (!bCapsInitialized)
    {
        portMemCopy(pHostCaps, NV0080_CTRL_HOST_CAPS_TBL_SIZE, tempCaps, NV0080_CTRL_HOST_CAPS_TBL_SIZE);
        return;
    }

    // factor in this GPUs caps: all these are feature caps, so use AND
    RMCTRL_AND_CAP(pHostCaps, tempCaps, temp,
                   NV0080_CTRL_HOST_CAPS, _P2P_4_WAY);
    RMCTRL_AND_CAP(pHostCaps, tempCaps, temp,
                   NV0080_CTRL_HOST_CAPS, _P2P_8_WAY);
    RMCTRL_AND_CAP(pHostCaps, tempCaps, temp,
                   NV0080_CTRL_HOST_CAPS, _VIRTUAL_P2P);
    RMCTRL_AND_CAP(pHostCaps, tempCaps, temp,
                   NV0080_CTRL_HOST_CAPS, _GPU_COHERENT_MAPPING_SUPPORTED);

    RMCTRL_OR_CAP(pHostCaps, tempCaps, temp,
                  NV0080_CTRL_HOST_CAPS, _SEMA_ACQUIRE_BUG_105665);
    RMCTRL_OR_CAP(pHostCaps, tempCaps, temp,
                  NV0080_CTRL_HOST_CAPS, _SYS_SEMA_DEADLOCK_BUG_148216);
    RMCTRL_OR_CAP(pHostCaps, tempCaps, temp,
                  NV0080_CTRL_HOST_CAPS, _SLOWSLI);
    RMCTRL_OR_CAP(pHostCaps, tempCaps, temp,
                  NV0080_CTRL_HOST_CAPS, _SEMA_READ_ONLY_BUG);
    RMCTRL_OR_CAP(pHostCaps, tempCaps, temp,
                  NV0080_CTRL_HOST_CAPS, _MEM2MEM_BUG_365782);
    RMCTRL_OR_CAP(pHostCaps, tempCaps, temp,
                  NV0080_CTRL_HOST_CAPS, _LARGE_NONCOH_UPSTR_WRITE_BUG_114871);
    RMCTRL_OR_CAP(pHostCaps, tempCaps, temp,
                  NV0080_CTRL_HOST_CAPS, _LARGE_UPSTREAM_WRITE_BUG_115115);
    RMCTRL_OR_CAP(pHostCaps, tempCaps, temp,
                  NV0080_CTRL_HOST_CAPS, _SEP_VIDMEM_PB_NOTIFIERS_BUG_83923);
    RMCTRL_OR_CAP(pHostCaps, tempCaps, temp,
                  NV0080_CTRL_HOST_CAPS, _P2P_DEADLOCK_BUG_203825);
    RMCTRL_OR_CAP(pHostCaps, tempCaps, temp,
                  NV0080_CTRL_HOST_CAPS, _COMPRESSED_BL_P2P_BUG_257072);
    RMCTRL_OR_CAP(pHostCaps, tempCaps, temp,
                  NV0080_CTRL_HOST_CAPS, _CROSS_BLITS_BUG_270260);
    RMCTRL_OR_CAP(pHostCaps, tempCaps, temp,
                  NV0080_CTRL_HOST_CAPS, _CPU_WRITE_WAR_BUG_420495);
    RMCTRL_OR_CAP(pHostCaps, tempCaps, temp,
                  NV0080_CTRL_HOST_CAPS, _BAR1_READ_DEADLOCK_BUG_511418);

    return;
}

NV_STATUS
kbusMapFbApertureByHandle_IMPL
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvHandle   hClient,
    NvHandle   hMemory,
    NvU64      offset,
    NvU64      size,
    NvU64     *pBar1Va
)
{
    NV_STATUS status;
    RsClient *pClient = NULL;
    RsResourceRef *pSrcMemoryRef = NULL;
    Memory *pSrcMemory = NULL;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    NvU64 fbApertureOffset = 0;
    NvU64 fbApertureLength = size;

    NV_ASSERT_OK_OR_RETURN(serverGetClientUnderLock(&g_resServ, hClient, &pClient));

    status = clientGetResourceRef(pClient, hMemory, &pSrcMemoryRef);
    if (status != NV_OK)
    {
        return status;
    }

    pSrcMemory = dynamicCast(pSrcMemoryRef->pResource, Memory);
    if (pSrcMemory == NULL)
    {
        return NV_ERR_INVALID_OBJECT;
    }

    pMemDesc = pSrcMemory->pMemDesc;

    if (memdescGetAddressSpace(pMemDesc) != ADDR_FBMEM)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = kbusMapFbAperture_HAL(pGpu, pKernelBus, pMemDesc, offset,
                                   &fbApertureOffset, &fbApertureLength,
                                   BUS_MAP_FB_FLAGS_MAP_UNICAST, hClient);
    if (status != NV_OK)
    {
        return status;
    }

    NV_ASSERT_OR_GOTO(fbApertureLength >= size, failed);

    if ((!NV_IS_ALIGNED64(fbApertureOffset, osGetPageSize())) ||
        (!NV_IS_ALIGNED64(fbApertureLength, osGetPageSize())))
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto failed;
    }

    *pBar1Va = gpumgrGetGpuPhysFbAddr(pGpu) + fbApertureOffset;

    if (!NV_IS_ALIGNED64(*pBar1Va, osGetPageSize()))
    {
        status = NV_ERR_INVALID_ADDRESS;
        goto failed;
    }

    return NV_OK;

failed:
    // Note: fbApertureLength is not used by kbusUnmapFbAperture_HAL(), so it's passed as 0
    kbusUnmapFbAperture_HAL(pGpu, pKernelBus, pMemDesc,
                            fbApertureOffset, 0,
                            BUS_MAP_FB_FLAGS_MAP_UNICAST);

    return status;
}

NV_STATUS
kbusUnmapFbApertureByHandle_IMPL
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvHandle   hClient,
    NvHandle   hMemory,
    NvU64      bar1Va
)
{
    NV_STATUS status;
    RsClient *pClient = NULL;
    RsResourceRef *pSrcMemoryRef = NULL;
    Memory *pSrcMemory = NULL;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;

    NV_ASSERT_OK_OR_RETURN(serverGetClientUnderLock(&g_resServ, hClient, &pClient));

    status = clientGetResourceRef(pClient, hMemory, &pSrcMemoryRef);
    if (status != NV_OK)
    {
        return status;
    }

    pSrcMemory = dynamicCast(pSrcMemoryRef->pResource, Memory);
    if (pSrcMemory == NULL)
    {
        return NV_ERR_INVALID_OBJECT;
    }

    pMemDesc = pSrcMemory->pMemDesc;

    // Note: fbApertureLength is not used by kbusUnmapFbAperture_HAL(), so it's passed as 0
    status = kbusUnmapFbAperture_HAL(pGpu, pKernelBus, pMemDesc,
                                     bar1Va - gpumgrGetGpuPhysFbAddr(pGpu),
                                     0, BUS_MAP_FB_FLAGS_MAP_UNICAST);
    if (status != NV_OK)
    {
        return status;
    }

    return NV_OK;
}

/*!
 * Helper function to determine if the requested GET_BUS_INFO ctrl call needs to be served
 * by GSP/host, then send RPC to GSP/host. Otherwise return directly so that the caller can
 * continue the execution on CPU.
 *
 *  @param[in]       pGpu       OBJGPU pointer
 *  @param[in/out]   pBusInfo   Pointer to NV2080_CTRL_BUS_INFO which specifies the index we want to query
 *
 *  @returns RPC status
 */
NV_STATUS
kbusSendBusInfo_IMPL
(
    OBJGPU               *pGpu,
    KernelBus            *pKernelBus,
    NV2080_CTRL_BUS_INFO *pBusInfo
)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_BUS_GET_INFO_V2_PARAMS busGetInfoParams = {0};

    busGetInfoParams.busInfoList[0] = *pBusInfo;
    busGetInfoParams.busInfoListSize = 1;

    NV_RM_RPC_CONTROL(pGpu,
                      pGpu->hInternalClient,
                      pGpu->hInternalSubdevice,
                      NV2080_CTRL_CMD_BUS_GET_INFO_V2,
                      &busGetInfoParams,
                      sizeof(busGetInfoParams),
                      status);

    pBusInfo->data = busGetInfoParams.busInfoList[0].data;
    return status;
}
