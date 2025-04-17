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

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/device/device.h"
#include "mem_mgr/vaspace.h"
#include "mem_mgr/io_vaspace.h"
#include "mem_mgr/gpu_vaspace.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/bus/kern_bus.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/mem_sys/kern_mem_sys.h"
#include "platform/chipset/chipset.h"
#include "rmapi/client.h"
#include "platform/sli/sli.h"
#include "nvdevid.h"
#include "containers/eheap_old.h"
#include "gpu/bus/p2p_api.h"

#include "gpu/gsp/gsp_static_config.h"
#include "vgpu/rpc.h"

#include "nvrm_registry.h"

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

    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_FORCE_STATIC_BAR1, &data32) == NV_OK) &&
        data32 < NV_REG_STR_RM_FORCE_STATIC_BAR1_MAX)
    {
        pKernelBus->staticBar1ForceType = data32;
        NV_PRINTF(LEVEL_WARNING, "Forcing static BAR1 to type %d.\n", data32);
    }
    else
    {
        pKernelBus->staticBar1ForceType = NV_REG_STR_RM_FORCE_STATIC_BAR1_DEFAULT;
    }

    //
    // NV_REG_STR_RM_GPUDIRECT_RDMA_FORCE_SPA regkey
    // is only used on coherent systems with BAR1 enabled.
    //
    pKernelBus->bGrdmaForceSpa = NV_REG_STR_RM_GPUDIRECT_RDMA_FORCE_SPA_DEFAULT;

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GPUDIRECT_RDMA_FORCE_SPA, &data32) == NV_OK)
    {
        if (data32 == NV_REG_STR_RM_GPUDIRECT_RDMA_FORCE_SPA_YES)
        {
            pKernelBus->bGrdmaForceSpa = NV_TRUE;
        }
    }

    if (RMCFG_FEATURE_PLATFORM_WINDOWS && !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_TCC_MODE))
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

    if (osReadRegistryDword(pGpu, NV_REG_STR_RESTORE_BAR1_SIZE_BUG_3249028_WAR, &data32) == NV_OK)
    {
        pKernelBus->setProperty(pKernelBus, PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR, !!data32);
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_FORCE_BAR_ACCESS_ON_HCC, &data32) == NV_OK &&
        data32 == NV_REG_STR_RM_FORCE_BAR_ACCESS_ON_HCC_YES  && gpuIsCCDevToolsModeEnabled(pGpu))
    {
        pKernelBus->bForceBarAccessOnHcc = NV_TRUE;
    }

    return NV_OK;
}

/**
 * @brief  Gets the BAR1 VA range for a device
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 * @param[in] pDevice               Device pointer
 * @param[out] pBar1VARange         BAR1 VA range
 */

NV_STATUS
kbusGetBar1VARangeForDevice_IMPL(OBJGPU *pGpu, KernelBus *pKernelBus, Device *pDevice, NV_RANGE *pBar1VARange)
{
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    OBJVASPACE       *pBar1VAS          = kbusGetBar1VASpace_HAL(pGpu, pKernelBus);
    RmClient         *pRmClient;

    NV_ASSERT_OR_RETURN(pBar1VAS != NULL, NV_ERR_INVALID_STATE);

   *pBar1VARange = rangeMake(vaspaceGetVaStart(pBar1VAS), vaspaceGetVaLimit(pBar1VAS));

    pRmClient = dynamicCast(RES_GET_CLIENT(pDevice), RmClient);
    NV_ASSERT_OR_RETURN(pRmClient != NULL, NV_ERR_INVALID_CLIENT);

    if ((pKernelMIGManager != NULL) && kmigmgrIsMIGMemPartitioningEnabled(pGpu, pKernelMIGManager) &&
        !rmclientIsCapable(pRmClient, NV_RM_CAP_SYS_SMC_MONITOR) &&
        !kmigmgrIsDeviceUsingDeviceProfiling(pGpu, pKernelMIGManager, pDevice))
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
        KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
        MIG_INSTANCE_REF ref;

       *pBar1VARange = memmgrGetMIGPartitionableBAR1Range(pGpu, pMemoryManager);

        NV_ASSERT_OK_OR_RETURN(kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                   pDevice, &ref));
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

    if (pLocalGpu != pRemoteGpu)
    {
        status = memdescMapIommu(pMemDesc, pRemoteGpu->busInfo.iovaspaceId);
        if (status != NV_OK)
        {
            memdescDestroy(pMemDesc);
        }
        NV_ASSERT_OR_RETURN(status == NV_OK, ~0ULL);
    }

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

    memdescFree(pKernelBus->bar1[GPU_GFID_PF].pInstBlkMemDesc);
    memdescDestroy(pKernelBus->bar1[GPU_GFID_PF].pInstBlkMemDesc);
    pKernelBus->bar1[GPU_GFID_PF].pInstBlkMemDesc = NULL;
    memdescFree(pKernelBus->bar2[GPU_GFID_PF].pInstBlkMemDesc);
    memdescDestroy(pKernelBus->bar2[GPU_GFID_PF].pInstBlkMemDesc);
    pKernelBus->bar2[GPU_GFID_PF].pInstBlkMemDesc = NULL;

    //
    // We need to clean-up the memory resources for BAR2 as late as possible,
    // and after all memory descriptors have been reclaimed.
    //
    kbusDestructVirtualBar2_HAL(pGpu, pKernelBus, NV_TRUE, GPU_GFID_PF);

    // Unmap BAR2 mapping that was retained for RPC
    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) &&
        IS_VIRTUAL_WITH_SRIOV(pGpu) &&
        pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping != NULL)
    {
        // vgpuDestructObject should have been run
        OBJVGPU *pVGpu = GPU_GET_VGPU(pGpu);
        NV_ASSERT(pVGpu == NULL);

        osUnmapPciMemoryKernelOld(pGpu, pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping);
    }

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
    if (!KBUS_BAR0_PRAMIN_DISABLED(pGpu) &&
        !kbusIsBarAccessBlocked(pKernelBus) &&
        !(flags & GPU_STATE_FLAGS_GC6_TRANSITION))
    {
        // we will initialize bar2 to the default big page size of the system
        NV_ASSERT_OK_OR_RETURN(kbusInitVirtualBar2_HAL(pGpu, pKernelBus));
        NV_ASSERT_OK_OR_RETURN(kbusSetupCpuPointerForBusFlush_HAL(pGpu, pKernelBus));
    }
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
kbusGetVaLimitForBar2_FWCLIENT
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
 * @brief Calculates the BAR2 VA limit (in Byte units)
 * Can be safely called only after kbusSetBarsApertureSize_HAL is executed.
 */
NvU64 kbusGetVaLimitForBar2_IMPL
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NvU64 limit = 0;
    NvU32 gfid;

    NV_ASSERT_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK, 0);

    //
    // Return zero from the guest in the paravirtualization case or
    // if guest is running in SRIOV heavy mode.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return 0;
    }

    kbusCalcCpuInvisibleBar2Range_HAL(pGpu, pKernelBus, gfid);

    //
    // we are just accounting here for possibility that
    // we are on pre_PASCAL and so we set the va limit
    // to account only for the cpuVisible Aperture
    //
    if (pKernelBus->bar2[gfid].cpuInvisibleLimit > pKernelBus->bar2[gfid].cpuInvisibleBase)
    {
        limit = pKernelBus->bar2[gfid].cpuInvisibleLimit;
    }
    else
    {
        limit = pKernelBus->bar2[gfid].cpuVisibleLimit;
    }
    NV_PRINTF(LEVEL_INFO, "va limit: 0x%llx\n", limit);
    //
    // pKernelBus->bar2.vaLimit is set by this function.
    // Assert to ensure that this value doesn't get changed.
    //
    NV_ASSERT(pKernelBus->bar2[gfid].vaLimit == 0 || pKernelBus->bar2[gfid].vaLimit == limit);

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
    NvU32                rootSize  = mmuFmtLevelSize(pRootFmt);
    MMU_WALK_USER_CTX    userCtx   = {0};
    GspStaticConfigInfo *pGSCI     = GPU_GET_GSP_STATIC_INFO(pGpu);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
         memdescCreate(&pMemDesc, pGpu, rootSize, RM_PAGE_SIZE, NV_TRUE, ADDR_FBMEM,
                       kgmmuGetPTEAttr(pKernelGmmu), MEMDESC_FLAGS_NONE));

    memdescDescribe(pMemDesc, ADDR_FBMEM, pGSCI->bar1PdeBase, rootSize);
    memdescSetPageSize(pMemDesc, VAS_ADDRESS_TRANSLATION(pKernelBus->bar1[GPU_GFID_PF].pVAS), RM_PAGE_SIZE);

    NV_ASSERT_OK_OR_GOTO(status,
        gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, NULL, &userCtx),
        done);

    //
    // Modify the CPU-RM's walker state with the new backing memory.
    // This is intended to replace CPU-RM's PDB by GSP-RM's PDB.
    //
    status = mmuWalkModifyLevelInstance(pGpuState->pWalk,
                                        pRootFmt,
                                        vaspaceGetVaStart(pKernelBus->bar1[GPU_GFID_PF].pVAS),
                                        (MMU_WALK_MEMDESC*)pMemDesc,
                                        rootSize,
                                        NV_TRUE,
                                        NV_TRUE,
                                        NV_FALSE);
    gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);

done:
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to modify CPU-RM's BAR1 PDB to GSP-RM's BAR1 PDB.\n");
        memdescDestroy(pMemDesc);
    }
    else
    {
        gvaspaceInvalidateTlb(pGVAS, pGpu, PTE_DOWNGRADE);
    }

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
    NV_STATUS            status   = NV_OK;
    PMEMORY_DESCRIPTOR   pMemDesc;
    GspStaticConfigInfo *pGSCI    = GPU_GET_GSP_STATIC_INFO(pGpu);
    const MMU_FMT_LEVEL *pRootFmt = pKernelBus->bar2[GPU_GFID_PF].pFmt->pRoot;
    NvU64                entryValue;
    MEMORY_DESCRIPTOR   *pOldPdb;

    pOldPdb = pKernelBus->virtualBar2[GPU_GFID_PF].pPDB;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        memdescCreate(&pMemDesc, pGpu, pKernelBus->bar2[GPU_GFID_PF].pageDirSize, RM_PAGE_SIZE, NV_TRUE,
                      ADDR_FBMEM, pKernelBus->PDEBAR2Attr, MEMDESC_FLAGS_NONE));

    memdescDescribe(pMemDesc, ADDR_FBMEM, pGSCI->bar2PdeBase, pKernelBus->bar2[GPU_GFID_PF].pageDirSize);

    // Update CPU-RM's SW cache of PDB to GSP-RM's PDB address
    pKernelBus->virtualBar2[GPU_GFID_PF].pPDB = pMemDesc;

    //
    // BAR2 page table is not yet working at this point, so retrieving the
    // PDE3[0] of BAR2 page table via BAR0_WINDOW or GSP-DMA (in case BARs
    // are blocked)
    //
    if (kbusIsBarAccessBlocked(pKernelBus))
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
        TRANSFER_SURFACE surf = {0};

        surf.pMemDesc = pOldPdb;
        surf.offset = 0;

        NV_ASSERT_OK_OR_RETURN(
            memmgrMemRead(pMemoryManager, &surf, &entryValue,
                          pRootFmt->entrySize, TRANSFER_FLAGS_NONE));
    }
    else
    {
        entryValue = GPU_REG_RD32(pGpu, (NvU32)pKernelBus->bar2[GPU_GFID_PF].bar2OffsetInBar0Window) |
                 ((NvU64)GPU_REG_RD32(pGpu, (NvU32)pKernelBus->bar2[GPU_GFID_PF].bar2OffsetInBar0Window + 4) << 32);
    }

    //
    // Provide the PDE3[0] value to GSP-RM so that GSP-RM can merge CPU-RM's
    // page table to GSP-RM's page table
    //
    NV_RM_RPC_UPDATE_BAR_PDE(pGpu, NV_RPC_UPDATE_PDE_BAR_2, entryValue, pRootFmt->virtAddrBitLo, status);

    return NV_OK;
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
    NvBool bExplicitCacheFlushRequired;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    portMemSet(tempCaps, 0, NV0080_CTRL_HOST_CAPS_TBL_SIZE);

    /*! DMAs to/from cached memory need to have the cache flushed explicitly */
    bExplicitCacheFlushRequired = NVCPU_IS_ARM &&
                                  (RMCFG_FEATURE_PLATFORM_UNIX || RMCFG_FEATURE_PLATFORM_MODS_UNIX);
    if (bExplicitCacheFlushRequired ||
        (!pCl->getProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT)))
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_HOST_CAPS, _EXPLICIT_CACHE_FLUSH_REQD);

    if ((pCl->FHBBusInfo.vendorID == PCI_VENDOR_ID_NVIDIA) &&
        ((pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_BR04_PRESENT)) ||
         ((pCl->FHBBusInfo.deviceID >= NVIDIA_C73_CPU_PCI_0_DEVICE_ID_SLI2) &&
          (pCl->FHBBusInfo.deviceID <= NVIDIA_C73_CPU_PCI_0_DEVICE_ID_RESERVED_3))))
    {
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_HOST_CAPS, _CPU_WRITE_WAR_BUG_420495);
    }

    // If we don't have existing caps with which to reconcile, then just return
    if (!bCapsInitialized)
    {
        portMemCopy(pHostCaps, NV0080_CTRL_HOST_CAPS_TBL_SIZE, tempCaps, NV0080_CTRL_HOST_CAPS_TBL_SIZE);
        return;
    }

    RMCTRL_OR_CAP(pHostCaps, tempCaps, temp,
                  NV0080_CTRL_HOST_CAPS, _CPU_WRITE_WAR_BUG_420495);

    return;
}

NvU32
kbusConvertBusMapFlagsToDmaFlags(KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU32 busMapFlags)
{
    NvU32 dmaFlags = DRF_DEF(OS46, _FLAGS, _DMA_UNICAST_REUSE_ALLOC, _FALSE);

    if (busMapFlags & BUS_MAP_FB_FLAGS_MAP_OFFSET_FIXED)
    {
        dmaFlags = FLD_SET_DRF(OS46, _FLAGS, _DMA_OFFSET_FIXED, _TRUE, dmaFlags);
    }

    if (memdescGetCpuCacheAttrib(pMemDesc) == NV_MEMORY_CACHED)
    {
        dmaFlags = FLD_SET_DRF(OS46, _FLAGS, _CACHE_SNOOP, _ENABLE, dmaFlags);
    }

    if (busMapFlags & BUS_MAP_FB_FLAGS_MAP_DOWNWARDS)
    {
        dmaFlags = FLD_SET_DRF(OS46, _FLAGS, _DMA_OFFSET_GROWS, _DOWN, dmaFlags);
    }

    // Disable the encryption if DIRECT mapping is requested, currently it is just for testing purpose
    if (busMapFlags & BUS_MAP_FB_FLAGS_DISABLE_ENCRYPTION)
    {
        dmaFlags = FLD_SET_DRF(OS46, _FLAGS, _DISABLE_ENCRYPTION, _TRUE, dmaFlags);
    }

    NV_ASSERT(!((busMapFlags & BUS_MAP_FB_FLAGS_READ_ONLY) &&
                (busMapFlags & BUS_MAP_FB_FLAGS_WRITE_ONLY)));
    if (busMapFlags & BUS_MAP_FB_FLAGS_READ_ONLY)
    {
        dmaFlags = FLD_SET_DRF(OS46, _FLAGS, _ACCESS, _READ_ONLY, dmaFlags);
    }
    else if (busMapFlags & BUS_MAP_FB_FLAGS_WRITE_ONLY)
    {
        dmaFlags = FLD_SET_DRF(OS46, _FLAGS, _ACCESS, _WRITE_ONLY, dmaFlags);
    }

    return dmaFlags;
}


NV_STATUS
kbusMapFbApertureSingle_IMPL
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU64 offset,
    NvU64 *pAperOffset,
    NvU64 *pLength,
    NvU32 flags,
    Device *pDevice
)
{
    MemoryArea memArea;
    MemoryRange memRange;
    memArea.numRanges = 1;
    memArea.pRanges = &memRange;
    memRange.start = *pAperOffset;
    NV_ASSERT_OK_OR_RETURN(kbusMapFbAperture_HAL(pGpu, pKernelBus, pMemDesc, mrangeMake(offset, *pLength), &memArea,
        flags | BUS_MAP_FB_FLAGS_UNMANAGED_MEM_AREA, pDevice));
    *pLength = memRange.size;
    *pAperOffset = memRange.start;
    return NV_OK;
}
    
NV_STATUS
kbusUnmapFbApertureSingle_IMPL
(
    OBJGPU *pGpu,
    KernelBus *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU64 aperOffset,
    NvU64 length,
    NvU32 flags
)
{
    MemoryArea memArea;
    MemoryRange memRange = mrangeMake(aperOffset, length);

    memArea.numRanges = 1;
    memArea.pRanges = &memRange;

    return kbusUnmapFbAperture_HAL(pGpu, pKernelBus, pMemDesc, memArea, flags | BUS_MAP_FB_FLAGS_UNMANAGED_MEM_AREA);
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

    if (IS_VIRTUAL(pGpu))
    {
        VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

        NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

        pBusInfo->data = pVSI->busGetInfoV2.busInfoList[pBusInfo->index].data;
    }
    else
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV2080_CTRL_BUS_GET_INFO_V2_PARAMS busGetInfoParams = {0};

        busGetInfoParams.busInfoList[0] = *pBusInfo;
        busGetInfoParams.busInfoListSize = 1;

        status = pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_BUS_GET_INFO_V2,
                                 &busGetInfoParams,
                                 sizeof(busGetInfoParams));

        pBusInfo->data = busGetInfoParams.busInfoList[0].data;
    }

    return status;
}

/*!
 * @brief Returns the Nvlink peer ID from pGpu0 to pGpu1
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   pKernelBus1    (remote GPU)
 * @param[out]  nvlinkPeer     NvU32 pointer
 *
 * return NV_OK on success
 */
NV_STATUS
kbusGetNvlinkP2PPeerId_VGPU
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1,
    NvU32     *nvlinkPeer,
    NvU32      flags
)
{
    NvBool bEgmPeer = FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _REMOTE_EGM, _YES, flags);
    NvU32 status = NV_OK;

    if (bEgmPeer)
    {
        *nvlinkPeer = kbusGetEgmPeerId_HAL(pGpu0, pKernelBus0, pGpu1);
    }
    else
    {
        *nvlinkPeer = kbusGetPeerId_HAL(pGpu0, pKernelBus0, pGpu1);
    }

    if (*nvlinkPeer != BUS_INVALID_PEER)
    {
        return NV_OK;
    }

    *nvlinkPeer = kbusGetUnusedPeerId_HAL(pGpu0, pKernelBus0);

    // If could not find a free peer ID, return error
    if (*nvlinkPeer == BUS_INVALID_PEER)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "GPU%d: peerID not available for NVLink P2P\n",
                  pGpu0->gpuInstance);
        return NV_ERR_GENERIC;
    }
    // Reserve the peer ID for NVLink use
    status = kbusReserveP2PPeerIds_HAL(pGpu0, pKernelBus0, NVBIT(*nvlinkPeer));

    if (status == NV_OK)
    {
        pKernelBus0->p2p.bEgmPeer[*nvlinkPeer] = bEgmPeer;
    }

    return status;
}

/**
 * @brief     Check if the static bar1 is enabled
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 */
NvBool
kbusIsStaticBar1Enabled_IMPL
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NvU32 gfid;

    return ((vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK) &&
            pKernelBus->bar1[gfid].bStaticBar1Enabled);
}

/**
 * @brief     Check for any P2P references in to remote GPUs
 *            which are still have a P2P api object alive.
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 */
NV_STATUS
kbusIsGpuP2pAlive_IMPL
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    return (pKernelBus->totalP2pObjectsAliveRefCount > 0);
}

/**
 * @brief     Gets BAR0 size in bytes for VF
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 */
NvU64 kbusGetVfBar0SizeBytes_IMPL
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    // Bar 0 size for VF is always 16MB
    return 16llu << 20;
}

/**
 * @brief Update BAR1 size and availability in RUSD
 *
 * @param[in] pGpu
 */
NV_STATUS
kbusUpdateRusdStatistics_IMPL
(
    OBJGPU *pGpu
)
{
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    OBJVASPACE *pBar1VAS;
    OBJEHEAP *pVASHeap;
    RUSD_BAR1_MEMORY_INFO *pSharedData;
    NvU64 bar1Size = 0;
    NvU64 bar1AvailSize = 0;
    NV_RANGE bar1VARange = NV_RANGE_EMPTY;
    NvBool bZeroRusd = kbusIsBar1Disabled(pKernelBus);
    
    if (kbusIsStaticBar1Enabled(pGpu, pKernelBus))
    {
        // PMA owns BAR1 info, so don't  update.
        return NV_OK;
    }

    bZeroRusd = bZeroRusd || IS_MIG_ENABLED(pGpu);

    if (!bZeroRusd)
    {
        pBar1VAS = kbusGetBar1VASpace_HAL(pGpu, pKernelBus);
        NV_ASSERT_OR_RETURN(pBar1VAS != NULL, NV_ERR_INVALID_STATE);
        pVASHeap = vaspaceGetHeap(pBar1VAS);
        bar1VARange = rangeMake(vaspaceGetVaStart(pBar1VAS), vaspaceGetVaLimit(pBar1VAS));

        bar1Size = (NvU32)(rangeLength(bar1VARange) / 1024);

        if (pVASHeap != NULL)
        {
            NvU64 freeSize = 0;

            pVASHeap->eheapInfoForRange(pVASHeap, bar1VARange, NULL, NULL, NULL, &freeSize);
            bar1AvailSize = (NvU32)(freeSize / 1024);
        }
    }

    // Minimize critical section, write data once we have it.
    pSharedData = gpushareddataWriteStart(pGpu, bar1MemoryInfo);
    MEM_WR32(&pSharedData->bar1Size, bar1Size);
    MEM_WR32(&pSharedData->bar1AvailSize, bar1AvailSize);
    gpushareddataWriteFinish(pGpu, bar1MemoryInfo);

    return NV_OK;
}

NV_STATUS
kbusGetGpuFbPhysAddressForRdma_IMPL
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvBool     bForcePcie,
    NvU64     *pPhysAddr
)
{
    if((bForcePcie) &&
       (!pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING)))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // For forced PCIe mappings on coherent systems, use SPA by default instead of GPA
    // if the RmGpuDirectRdmaForceSPA regkey is set.
    // This is a stop-gap measure until hypervisor ensures GPA==SPA.
    //
    if (bForcePcie && pKernelBus->bGrdmaForceSpa)
    {
        *pPhysAddr = pKernelBus->grdmaBar1Spa;
    }
    else
    {
        *pPhysAddr = gpumgrGetGpuPhysFbAddr(pGpu);
    }

    return NV_OK;
}

NV_STATUS kbusGetEffectiveAddressSpace_SOC(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc, NvU32 mapFlags,
                                       NV_ADDRESS_SPACE *pAddrSpace)
{
    if (pAddrSpace != NULL)
        *pAddrSpace = memdescGetAddressSpace(pMemDesc);

    return NV_OK;
}
