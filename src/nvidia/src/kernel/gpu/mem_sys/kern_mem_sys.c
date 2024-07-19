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

#include "os/os.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "vgpu/vgpu_events.h"
#include "gpu/rpc/objrpc.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/bus/kern_bus.h"
#include "os/os.h"
#include "platform/sli/sli.h"
#include "nvrm_registry.h"
#include "gpu/gsp/gsp_static_config.h"

static void
kmemsysInitRegistryOverrides
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU32 data32;

    //
    // Bug 1032432. Check regkey for FB pull
    //
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_L2_CLEAN_FB_PULL, &data32) == NV_OK)
    {
        if (data32 == NV_REG_STR_RM_L2_CLEAN_FB_PULL_DISABLED)
            pKernelMemorySystem->bL2CleanFbPull = NV_FALSE;
    }

    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_OVERRIDE_TO_GMK, &data32) == NV_OK) && 
        (data32 != NV_REG_STR_RM_OVERRIDE_TO_GMK_DISABLED))
    {
        pKernelMemorySystem->overrideToGMK = data32;
    }

}

NV_STATUS
kmemsysConstructEngine_IMPL
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    ENGDESCRIPTOR       engDesc
)
{
    pKernelMemorySystem->memPartitionNumaInfo = NULL;

    kmemsysInitRegistryOverrides(pGpu, pKernelMemorySystem);

    if (IS_GSP_CLIENT(pGpu))
    {
        // Setting up the sysmem flush buffer needs to be done very early in some cases
        // as it's required for the GPU to perform a system flush. One such case is
        // resetting GPU FALCONs and in particular resetting the PMU as part of VBIOS
        // init.
        NV_ASSERT_OK_OR_RETURN(kmemsysInitFlushSysmemBuffer_HAL(pGpu, pKernelMemorySystem));

    }

    return NV_OK;
}

NV_STATUS
kmemsysStatePreInitLocked_IMPL
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NV_STATUS status = NV_OK;
    MEMORY_SYSTEM_STATIC_CONFIG *pStaticConfig;

    pStaticConfig =
        portMemAllocNonPaged(sizeof(*pStaticConfig));
    NV_ASSERT_TRUE_OR_GOTO(status,
        (pStaticConfig != NULL),
        NV_ERR_INSUFFICIENT_RESOURCES,
        exit);
    portMemSet(pStaticConfig, 0, sizeof(*pStaticConfig));
    //
    // Assign the pointer immediately after allocating to make sure it will
    // always be freed
    //
    pKernelMemorySystem->pStaticConfig = pStaticConfig;

    NV_ASSERT_OK_OR_GOTO(status,
        kmemsysInitStaticConfig_HAL(
            pGpu, pKernelMemorySystem, pStaticConfig),
        exit);

exit:
    if (status != NV_OK)
    {
        portMemFree((void *)pKernelMemorySystem->pStaticConfig);
        pKernelMemorySystem->pStaticConfig = NULL;
    }

    return status;
}

/*
 * Initialize the Kernel Memory System state.
 *
 * @param[in]  pGpu pointer to the GPU instance.
 * @param[in]  pKernelMemorySystem pointer to the kernel side KernelMemorySystem instance.
 *
 * @return NV_OK upon success.
 */
NV_STATUS kmemsysStateInitLocked_IMPL
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NV_STATUS status = NV_OK;
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    NV_ASSERT_OK_OR_GOTO(status, kmemsysEnsureSysmemFlushBufferInitialized(pGpu, pKernelMemorySystem), fail);

    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    pKernelMemorySystem->memPartitionNumaInfo = portMemAllocNonPaged(sizeof(MEM_PARTITION_NUMA_INFO) * KMIGMGR_MAX_GPU_SWIZZID);
    if (pKernelMemorySystem->memPartitionNumaInfo == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate memory for numa information.\n");
        status = NV_ERR_NO_MEMORY;
        NV_ASSERT_OR_GOTO(0, fail);
    }
    portMemSet(pKernelMemorySystem->memPartitionNumaInfo, 0, sizeof(MEM_PARTITION_NUMA_INFO) * KMIGMGR_MAX_GPU_SWIZZID);

    if (gpuIsSelfHosted(pGpu) &&
        (pKernelBif != NULL) && pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_C2C_LINK_UP))
    {
        //
        // memsysSetupCoherentCpuLink should be done only for the self hosted
        // configuration(SHH) where the coherent C2C link connects host CPU(TH500) and GPU
        // and not in the externally hosted(EHH) case where host CPU(say x86) is connected
        // to GPU through PCIe and C2C only connects the TH500 (for EGM memory) and GPU.
        // The gpuIsSelfHosted(pGpu) check here is to distinguish between the SHH
        // and EHH configuration as C2C link is up in both of these cases.
        //

        if (IS_GSP_CLIENT(pGpu))
        {
            GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);

            if (pGSCI->bAtsSupported)
            {
                NV_PRINTF(LEVEL_INFO, "ATS supported\n");

                pGpu->setProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED, NV_TRUE);
            }
        }
        if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL_WITH_SRIOV(pGpu))
        {
            //
            // PDB_PROP_GPU_C2C_SYSMEM is already set in physical-RM but not in
            // in Kernel-RM/Guest-RM where it is actually consumed. setting PDB_PROP_GPU_C2C_SYSMEM
            // in Kernel-RM/Guest-RM when the platform is self-hosted and the C2C links are up, which
            // indicate the C2C is connected to CPU and Physical-RM would have set up the HSHUB
            // to route sysmem through C2C.
            //
            pGpu->setProperty(pGpu, PDB_PROP_GPU_C2C_SYSMEM, NV_TRUE);
        }

        //
        // kmemesysSetupCoherentCpuLink should not be called from physical RM as
        // it is intended to be called on kernel side to update
        // KernelMemorySystem for C2C, NUMA functionality.
        //
        NV_ASSERT_OK_OR_GOTO(status, kmemsysSetupCoherentCpuLink(pGpu, pKernelMemorySystem, NV_FALSE), fail);

        if (gpuIsSelfHosted(pGpu) && pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
        {
            //Disable BAR1 only for Hopper as Blackwell+ supports simultaneous BAR1 access to FB.
            if (IsHOPPER(pGpu))
            {
                pKernelBus->bBar1Disabled = NV_TRUE;
            }
            // In self-hosted systems Disable BAR2 when C2C is up.
            pKernelBus->bCpuVisibleBar2Disabled = NV_TRUE;
        }
    }

    {
        KernelGmmu   *pKernelGmmu   = GPU_GET_KERNEL_GMMU(pGpu);

        //
        // Ask GMMU to set the large page size after we have initialized
        // memory and before we initialize BAR2.
        //
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgmmuCheckAndDecideBigPageSize_HAL(pGpu, pKernelGmmu));
    }

fail:
    return status;
}

NV_STATUS
kmemsysStatePreLoad_IMPL
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32 flags
)
{
    //
    // Program the sysmem flush buffer address and assert that the register contents are valid.
    // The HAL wiring is such that a given RM build will only do one or the other (e.g., RM offloaded
    // to ucode won't program the register itself but will assert that its contents are valid).
    //
    kmemsysProgramSysmemFlushBuffer_HAL(pGpu, pKernelMemorySystem);
    kmemsysAssertSysmemFlushBufferValid_HAL(pGpu, pKernelMemorySystem);

    // Self Hosted GPUs should have its memory onlined by now.
    if (gpuIsSelfHosted(pGpu) &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING) &&
        osNumaOnliningEnabled(pGpu->pOsGpuInfo) &&
        !pKernelMemorySystem->bNumaNodesAdded)
    {
        //
        // TODO: Bug 1945658: Deferred error checking from stateInit so that stateDestroy
        // gets called. Refer kmemsysNumaAddMemory_HAL call site for further
        // details.
        //
        return NV_ERR_INVALID_STATE;
    }

    return NV_OK;
}

NV_STATUS
kmemsysStatePostLoad_IMPL
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32 flags
)
{
    if (IS_SILICON(pGpu) &&
        osNumaOnliningEnabled(pGpu->pOsGpuInfo))
    {
        NV_ASSERT_OR_RETURN(pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED),
                            NV_ERR_INVALID_STATE);
    }

    if (IS_SILICON(pGpu) &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED))
    {
        NV_STATUS status = kmemsysSetupAllAtsPeers_HAL(pGpu, pKernelMemorySystem);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "ATS peer setup failed.\n");
            return status;
        }
    }

    return NV_OK;
}

NV_STATUS
kmemsysStatePreUnload_IMPL
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32 flags
)
{
    if (IS_SILICON(pGpu) &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED))
    {
        kmemsysRemoveAllAtsPeers_HAL(pGpu, pKernelMemorySystem);
    }
    return NV_OK;
}

/*
 * Release the state accumulated in StateInit.
 * @param[in]  pGpu pointer to the GPU instance.
 * @param[in]  pKernelMemorySystem pointer to the kernel side KernelMemorySystem instance.
 */
void kmemsysStateDestroy_IMPL
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{

    // Teardown of Coherent Cpu Link is not required on Physical RM
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    if (pKernelBif && pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_C2C_LINK_UP) &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
    {
        kmemsysTeardownCoherentCpuLink(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu), NV_FALSE);
    }

    portMemFree((void *)pKernelMemorySystem->pStaticConfig);
}

/*!
 * Returns MemorySystem settings that are static after GPU state init/load is
 * finished.
 */
const MEMORY_SYSTEM_STATIC_CONFIG *
kmemsysGetStaticConfig_IMPL
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    // check if state Init has not completed.
    NV_ASSERT_OR_ELSE(pKernelMemorySystem != NULL, return NULL);

    return pKernelMemorySystem->pStaticConfig;
}

void
kmemsysDestruct_IMPL
(
    KernelMemorySystem *pKernelMemorySystem
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pKernelMemorySystem);

    //
    // kmemsysNumaRemoveAllMemory_HAL() is called here in Destruct instead of
    // Destroy to guarantee that NUMA memory is removed. This goes against
    // Init/Destroy symmetry, but it is necessary because kmemsysStateDestroy
    // may not be called for all cases when kmemsysStateInit was called
    // (e.g., when kmemsys or another engine afterwards fails Init).
    //
    // If NUMA memory is not removed, then all subsequent attempts to add NUMA
    // memory will fail, which will cause failures in future RM init attempts.
    //
    if (pKernelMemorySystem->memPartitionNumaInfo != NULL)
    {
        if (pKernelMemorySystem->bNumaNodesAdded == NV_TRUE)
        {
            kmemsysNumaRemoveAllMemory_HAL(pGpu, pKernelMemorySystem);
        }
        portMemFree(pKernelMemorySystem->memPartitionNumaInfo);
    }

    pKernelMemorySystem->sysmemFlushBuffer = 0;
    memdescFree(pKernelMemorySystem->pSysmemFlushBufferMemDesc);
    memdescDestroy(pKernelMemorySystem->pSysmemFlushBufferMemDesc);
    pKernelMemorySystem->pSysmemFlushBufferMemDesc = NULL;

    portMemSet(pKernelMemorySystem->gpuInstanceMemConfig, 0, sizeof(pKernelMemorySystem->gpuInstanceMemConfig));
}

NV_STATUS
kmemsysAllocComprResources_KERNEL
(
    OBJGPU               *pGpu,
    KernelMemorySystem   *pKernelMemorySystem,
    FB_ALLOC_INFO        *pFbAllocInfo,
    NvU64                 origSize,
    NvU32                 kindChosen,
    NvU32                *pRetAttr,
    NvU32                 retAttr2
)
{
    MemoryManager                     *pMemoryManager      = GPU_GET_MEMORY_MANAGER(pGpu);
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig = kmemsysGetStaticConfig(pGpu, pKernelMemorySystem);
    NvU32                              gfid;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    NV_ASSERT_OR_RETURN(pMemorySystemConfig->bOneToOneComptagLineAllocation || pMemorySystemConfig->bUseRawModeComptaglineAllocation,
        NV_ERR_INVALID_STATE);

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        !FLD_TEST_DRF(OS32, _ALLOC, _COMPTAG_OFFSET_USAGE, _FIXED, pFbAllocInfo->ctagOffset),
        NV_ERR_INVALID_ARGUMENT);

    // Failing the allocation if scrub on free is disabled
    if (!memmgrIsScrubOnFreeEnabled(pMemoryManager))
    {
        if (!(IS_SIMULATION(pGpu) || IsDFPGA(pGpu) || (IS_EMULATION(pGpu) && RMCFG_FEATURE_PLATFORM_MODS)
            ||(RMCFG_FEATURE_PLATFORM_WINDOWS && !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_TCC_MODE))
            ||hypervisorIsVgxHyper()
            ||IS_GFID_VF(gfid)
            ||(IsSLIEnabled(pGpu) && !(RMCFG_FEATURE_PLATFORM_WINDOWS &&
                                     !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_TCC_MODE))))
           )
        {
            NV_PRINTF(LEVEL_ERROR, "Compressible surfaces cannot be allocated on a system, "
                    "where scrub on free is disabled\n");
            return NV_ERR_INVALID_STATE;
        }
    }
    else if (pMemorySystemConfig->bOneToOneComptagLineAllocation)
    {
        NV_ASSERT_OR_RETURN(memmgrUseVasForCeMemoryOps(pMemoryManager), NV_ERR_INVALID_STATE);
    }

    FB_SET_HWRESID_CTAGID_FERMI(pFbAllocInfo->hwResId, FB_HWRESID_CTAGID_VAL_FERMI(-1));
    *pRetAttr = FLD_SET_DRF(OS32, _ATTR, _COMPR, _REQUIRED, *pRetAttr);
    return NV_OK;
}

/*!
 * @brief Initializes static config data from the Physical side.
 * @param[in]  pGpu pointer to the GPU instance.
 * @param[in]  pKernelMemorySystem pointer to the kernel side KernelMemorySystem instance.
 * @param[out] pConfig pointer to the static config init on Physical driver.
 *
 * @return NV_OK upon success.
 *         NV_ERR* otherwise.
 */
NV_STATUS
kmemsysInitStaticConfig_KERNEL
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    MEMORY_SYSTEM_STATIC_CONFIG *pConfig
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    return pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                           NV2080_CTRL_CMD_INTERNAL_MEMSYS_GET_STATIC_CONFIG,
                           pConfig, sizeof(*pConfig));
}

/*!
 * @brief   Function to map swizzId to mem size given total mem
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMemorySystem
 * @param[IN]   swizzId
 * @param[IN]   totalRange          total memory range
 * @param[OUT]  pPartitionSizeFlag  Flag stating partition memory size
 * @param[OUT]  pSizeInBytes        Memory size in bytes supported by partition
 */
NV_STATUS
kmemsysSwizzIdToMIGMemSize_IMPL
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32 swizzId,
    NV_RANGE totalRange,
    NvU32 *pPartitionSizeFlag,
    NvU64 *pSizeInBytes
)
{
    //
    // To handle the straddling issue we always consider memory for different
    // swizzIds as addition of minimum sized segements allowed in partitioning
    //
    NvU64 memSize = rangeLength(totalRange) / KMIGMGR_MAX_GPU_INSTANCES;

    switch (swizzId)
    {
        case 0:
        {
            *pSizeInBytes = memSize * KMIGMGR_MAX_GPU_INSTANCES;
            *pPartitionSizeFlag = DRF_DEF(2080_CTRL_GPU, _PARTITION_FLAG, _MEMORY_SIZE, _FULL);
            break;
        }

        case 1:
        case 2:
        {
            *pSizeInBytes = (memSize * (KMIGMGR_MAX_GPU_INSTANCES / 2));
            *pPartitionSizeFlag = DRF_DEF(2080_CTRL_GPU, _PARTITION_FLAG, _MEMORY_SIZE, _HALF);
            break;
        }

        case 3:
        case 4:
        case 5:
        case 6:
        {
            *pSizeInBytes = (memSize * (KMIGMGR_MAX_GPU_INSTANCES / 4));
            *pPartitionSizeFlag = DRF_DEF(2080_CTRL_GPU, _PARTITION_FLAG, _MEMORY_SIZE, _QUARTER);
            break;
        }

        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        {
            *pSizeInBytes = memSize;
            *pPartitionSizeFlag = DRF_DEF(2080_CTRL_GPU, _PARTITION_FLAG, _MEMORY_SIZE, _EIGHTH);
            break;
        }

        default:
        {
            NV_PRINTF(LEVEL_ERROR, "Unsupported SwizzId %d\n", swizzId);
            DBG_BREAKPOINT();
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    if ((*pSizeInBytes == 0) &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB) &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB))
    {
        NV_PRINTF(LEVEL_ERROR, "Insufficient memory\n");
        DBG_BREAKPOINT();
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }
    return NV_OK;
}

/*!
 * @brief   Function to map swizzId to mem range given total range
 */
NV_STATUS
kmemsysSwizzIdToMIGMemRange_IMPL
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32 swizzId,
    NV_RANGE totalRange,
    NV_RANGE *pAddrRange
)
{
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NV_STATUS rmStatus = NV_OK;
    NvU32 memSizeFlag = 0;
    NvU32 minSwizzId = 0;
    NvU64 unalignedStartAddr = 0;
    NvU64 memSize = 0;
    NV_RANGE swizzIdRange = NV_RANGE_EMPTY;

    NV_ASSERT_OR_RETURN(!rangeIsEmpty(totalRange), NV_ERR_INVALID_ARGUMENT);

    // Get SwizzId to size mapping
    NV_ASSERT_OK_OR_RETURN(
        kmemsysSwizzIdToMIGMemSize(pGpu, pKernelMemorySystem, swizzId, totalRange, &memSizeFlag, &memSize));

    swizzIdRange = kmigmgrMemSizeFlagToSwizzIdRange_HAL(pGpu, pKernelMIGManager, memSizeFlag);
    NV_ASSERT_OR_RETURN(!rangeIsEmpty(swizzIdRange), NV_ERR_INVALID_ARGUMENT);

    minSwizzId = swizzIdRange.lo;

    unalignedStartAddr = (totalRange.lo + (memSize * (swizzId - minSwizzId)));
    *pAddrRange = rangeMake(unalignedStartAddr, unalignedStartAddr + memSize - 1);

    return rmStatus;
}

/*!
 * @brief   Function to return GPU instance memory address range
 */
NV_STATUS
kmemsysGetMIGGPUInstanceMemInfo_IMPL
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32 swizzId,
    NV_RANGE *pAddrRange
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU64 vmmuSegmentSize;
    NvU64 startAddr;
    NvU64 endAddr;
    NvU64 partitionSize;

    NV_ASSERT_OR_RETURN(pAddrRange != NULL, NV_ERR_INVALID_ARGUMENT);
    *pAddrRange = NV_RANGE_EMPTY;
    NV_ASSERT_OR_RETURN(swizzId < KMIGMGR_MAX_GPU_SWIZZID, NV_ERR_INVALID_STATE);

    // Not supported in vGPU or ZERO_FB configs
    NV_CHECK_OR_RETURN(LEVEL_SILENT,
                       !(IS_VIRTUAL(pGpu) || (pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))),
                       NV_OK);

    //
    // VMMU not supported in AMODEL. Use legacy swizz-ID calculation instead of relying on vMMU segments
    // to calculate address range
    //
    if (IsAMODEL(pGpu))
    {
        NV_RANGE partitionableMemoryRange = memmgrGetMIGPartitionableMemoryRange(pGpu, pMemoryManager);
        return kmemsysSwizzIdToMIGMemRange(pGpu, pKernelMemorySystem, swizzId, partitionableMemoryRange, pAddrRange);
    }

    // Get the VMMU segment size
    vmmuSegmentSize = gpuGetVmmuSegmentSize(pGpu);
    NV_ASSERT_OR_RETURN((vmmuSegmentSize != 0), NV_ERR_INVALID_STATE);

    startAddr = pKernelMemorySystem->gpuInstanceMemConfig[swizzId].startingVmmuSegment * vmmuSegmentSize;
    partitionSize = pKernelMemorySystem->gpuInstanceMemConfig[swizzId].memSizeInVmmuSegment * vmmuSegmentSize;

    if (osNumaOnliningEnabled(pGpu->pOsGpuInfo))
    {
        NvU64 memblockSize;
        NvU64 alignedStartAddr;

        NV_ASSERT_OK_OR_RETURN(osNumaMemblockSize(&memblockSize));

        //
        // Align the partition start address and size to memblock size
        // Some FB memory is wasted here if it is not already aligned.
        //
        alignedStartAddr = NV_ALIGN_UP64(startAddr, memblockSize);

        if(pKernelMemorySystem->bNumaMigPartitionSizeEnumerated)
        {
            partitionSize = pKernelMemorySystem->numaMigPartitionSize[swizzId];
        }
        else
        {
            partitionSize -= (alignedStartAddr - startAddr);
        }

        partitionSize = NV_ALIGN_DOWN64(partitionSize, memblockSize);
        startAddr = alignedStartAddr;
    }

    endAddr = startAddr + partitionSize - 1;

    *pAddrRange = rangeMake(startAddr, endAddr);

    return NV_OK;
}

/**
 * @brief Modifies numaMigPartitionSize array such that memory size of
          all the mig partitions with swizzId between startSwizzId and
          endSwizzId is assigned the minimum value among all partition's
          memory size.
 *
 * @param[IN]      pKernelMemorySystem
 * @param[IN]      startSwizzId
 * @param[IN]      endSwizzId
 *
 */
static void
_kmemsysSetNumaMigPartitionSizeSubArrayToMinimumValue
(
    KernelMemorySystem *pKernelMemorySystem,
    NvU64 startSwizzId,
    NvU64 endSwizzId
)
{
    NvU64 minPartitionSize = pKernelMemorySystem->numaMigPartitionSize[startSwizzId];
    NvU64 index;

    for (index = startSwizzId; index <= endSwizzId; index++)
    {
        if(pKernelMemorySystem->numaMigPartitionSize[index] < minPartitionSize)
            minPartitionSize = pKernelMemorySystem->numaMigPartitionSize[index];
    }

    for (index = startSwizzId; index <= endSwizzId; index++)
    {
        pKernelMemorySystem->numaMigPartitionSize[index] = minPartitionSize;
    }
}

/*!
 * @brief   Function to populate static GPU instance memory config which will be
 *          utilized for GPU instance memory query and memory allocation
 */
NV_STATUS
kmemsysPopulateMIGGPUInstanceMemConfig_KERNEL
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_RANGE partitionableMemoryRange = memmgrGetMIGPartitionableMemoryRange(pGpu, pMemoryManager);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvU64 vmmuSegmentSize;
    NvU64 totalVmmuSegments;
    NvU64 alignedStartAddr;
    NvU64 alignedEndAddr;
    NvU32 swizzId;

    // Not needed in vGPU or zero_fb configs
    NV_CHECK_OR_RETURN(LEVEL_SILENT,
                       !(IS_VIRTUAL(pGpu) || (pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))),
                       NV_OK);

    // Nothing to do if MIG is not supported
    NV_CHECK_OR_RETURN(LEVEL_SILENT, kmigmgrIsMIGSupported(pGpu, pKernelMIGManager), NV_OK);

    // Get the VMMU segment size
    vmmuSegmentSize = gpuGetVmmuSegmentSize(pGpu);
    NV_ASSERT_OR_RETURN((vmmuSegmentSize != 0), NV_ERR_INVALID_STATE);

    alignedStartAddr = partitionableMemoryRange.lo;
    alignedEndAddr = partitionableMemoryRange.hi;
    if (alignedStartAddr != 0)
    {
        alignedStartAddr = NV_IS_ALIGNED64(alignedStartAddr, vmmuSegmentSize) ?
                           alignedStartAddr + vmmuSegmentSize :
                           NV_ALIGN_UP64(alignedStartAddr, vmmuSegmentSize);
    }

    if (NV_IS_ALIGNED64(alignedEndAddr + 1, vmmuSegmentSize))
    {
        alignedEndAddr = alignedEndAddr - vmmuSegmentSize;
    }

    totalVmmuSegments = (alignedEndAddr - alignedStartAddr + 1) / vmmuSegmentSize;
    for (swizzId = 0; swizzId < KMIGMGR_MAX_GPU_SWIZZID; swizzId++)
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kmemsysSwizzIdToVmmuSegmentsRange_HAL(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, totalVmmuSegments));
    }

    if (osNumaOnliningEnabled(pGpu->pOsGpuInfo))
    {
        NV_RANGE addrRange = NV_RANGE_EMPTY;
        NvU32 memSize;

        for(swizzId = 0; swizzId < KMIGMGR_MAX_GPU_SWIZZID; swizzId++)
        {
            kmemsysGetMIGGPUInstanceMemInfo(pGpu, pKernelMemorySystem, swizzId, &addrRange);
            pKernelMemorySystem->numaMigPartitionSize[swizzId] = addrRange.hi - addrRange.lo + 1;
        }

        //
        // In GH180 for all the swizzId's for a given memory profile (FULL, HALF, QUARTER 
        // and EIGHTH partitions) might not be same. Modify numaMigPartitionSize array
        // for the partition size to be constant for a given profile. BUG 4284299.
        //
        for (memSize = NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_FULL; memSize < NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE__SIZE; memSize++)
        {
            NV_RANGE swizzRange = kmigmgrMemSizeFlagToSwizzIdRange(pGpu, pKernelMIGManager, 
                                      DRF_NUM(2080_CTRL_GPU, _PARTITION_FLAG, _MEMORY_SIZE, memSize));
            _kmemsysSetNumaMigPartitionSizeSubArrayToMinimumValue(pKernelMemorySystem, swizzRange.lo, swizzRange.hi);
        }
        pKernelMemorySystem->bNumaMigPartitionSizeEnumerated = NV_TRUE;
    }
    return NV_OK;
}

/*!
 * @brief Gets GPU instance memory configuration based on swizzId
 */
NV_STATUS
kmemsysGetMIGGPUInstanceMemConfigFromSwizzId_IMPL
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32 swizzId,
    const MIG_GPU_INSTANCE_MEMORY_CONFIG **ppGPUInstanceMemConfig
)
{
    NV_ASSERT_OR_RETURN(swizzId < KMIGMGR_MAX_GPU_SWIZZID, NV_ERR_INVALID_ARGUMENT);

    if (IS_VIRTUAL(pGpu))
    {
        // VMMU Segment details are populated on Host and not Guest.
        return NV_ERR_NOT_SUPPORTED;
    }

    // MODS makes a control call to describe GPU instances before this is populated. Return invalid data anyways
    NV_ASSERT_OR_RETURN(pKernelMemorySystem->gpuInstanceMemConfig[swizzId].bInitialized, NV_ERR_INVALID_STATE);

    *ppGPUInstanceMemConfig = &pKernelMemorySystem->gpuInstanceMemConfig[swizzId];
    return NV_OK;
}

/*!
 * @brief Set GPU Instance memory config information and mark initialized
 */
NV_STATUS
kmemsysInitMIGGPUInstanceMemConfigForSwizzId_IMPL
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32 swizzId,
    NvU64 startingVmmuSegment,
    NvU64 memSizeInVmmuSegment
)
{
    NV_ASSERT_OR_RETURN(swizzId < KMIGMGR_MAX_GPU_SWIZZID, NV_ERR_INVALID_ARGUMENT);

    pKernelMemorySystem->gpuInstanceMemConfig[swizzId].startingVmmuSegment = startingVmmuSegment;
    pKernelMemorySystem->gpuInstanceMemConfig[swizzId].memSizeInVmmuSegment = memSizeInVmmuSegment;
    pKernelMemorySystem->gpuInstanceMemConfig[swizzId].bInitialized = NV_TRUE;

    NV_PRINTF(LEVEL_INFO,
        "GPU Instance Mem Config for swizzId = 0x%x : MemStartSegment = 0x%llx, MemSizeInSegments = 0x%llx\n",
        swizzId,
        pKernelMemorySystem->gpuInstanceMemConfig[swizzId].startingVmmuSegment,
        pKernelMemorySystem->gpuInstanceMemConfig[swizzId].memSizeInVmmuSegment);

    return NV_OK;
}

/*!
 * @brief Ensure that the sysmem flush sysmem buffer has been initialized
 *
 * Setting up the sysmem flush buffer needs to be done very early in some cases
 * as it's required for the GPU to perform a system flush. One such case is
 * resetting GPU FALCONs and in particular resetting the PMU as part of VBIOS
 * init.
 *
 * @returns NV_OK if the sysmem flush buffer has been initialized.
 */
NV_STATUS
kmemsysEnsureSysmemFlushBufferInitialized_IMPL
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    if (IS_VIRTUAL(pGpu)                                       ||
        IS_GSP_CLIENT(pGpu)                                    ||
        RMCFG_FEATURE_PLATFORM_GSP)
    {
        return NV_OK;
    }

    return kmemsysInitFlushSysmemBuffer_HAL(pGpu, pKernelMemorySystem);
}

/*!
 * @brief Handle sysmem NVLink/C2C, NUMA and ATS functionality
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in] pKernelMemorySystem pointer to the kernel side KernelMemorySystem instance.
 * @param[in] bFlush              Whether the CPU cache of the GPU mapping
 *                                should be flushed
 *
 * @return  NV_OK on success
 */
NV_STATUS
kmemsysSetupCoherentCpuLink_IMPL
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvBool              bFlush
)
{
    KernelBus     *pKernelBus     = GPU_GET_KERNEL_BUS(pGpu);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU64          numaOnlineSize = 0;
    NvU64          fbSize         = (pMemoryManager->Ram.fbTotalMemSizeMb << 20);
    NvU32          data32;
    NvBool         bCpuMapping    = NV_TRUE; // Default enable
    NvS32          numaNodeId     = NV0000_CTRL_NO_NUMA_NODE;
    NvU64          memblockSize   = 0;
    NvU64          rsvdFastSize   = 0;
    NvU64          rsvdSlowSize   = 0;
    NvU64          rsvdISOSize    = 0;
    NvU64          totalRsvdBytes = 0;

    {
        NV_ASSERT_OK_OR_RETURN(kmemsysGetFbNumaInfo_HAL(pGpu, pKernelMemorySystem,
                                                        &pKernelMemorySystem->coherentCpuFbBase,
                                                        &pKernelMemorySystem->coherentRsvdFbBase,
                                                        &numaNodeId));
        if (pKernelMemorySystem->coherentCpuFbBase != 0)
        {
            pKernelMemorySystem->coherentCpuFbEnd = pKernelMemorySystem->coherentCpuFbBase + fbSize;
        }
    }

    if ((osReadRegistryDword(pGpu,
                             NV_REG_STR_OVERRIDE_GPU_NUMA_NODE_ID, &data32)) == NV_OK)
    {
        numaNodeId = (NvS32)data32;
        NV_PRINTF(LEVEL_ERROR, "Override GPU NUMA node ID %d!\n", numaNodeId);
    }

    // Parse regkey here
    if ((osReadRegistryDword(pGpu,
                             NV_REG_STR_RM_FORCE_BAR_PATH, &data32) == NV_OK) &&
        (data32 == 1))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Force disabling NVLINK/C2C mappings through regkey.\n");

        bCpuMapping = NV_FALSE;
    }

    if ((pKernelMemorySystem->coherentCpuFbBase == 0) || !bCpuMapping)
    {
        return NV_OK;
    }

    NV_ASSERT_OK_OR_RETURN(osNumaMemblockSize(&memblockSize));

    memmgrCalcReservedFbSpaceHal_HAL(pGpu, pMemoryManager, &rsvdFastSize, &rsvdSlowSize, &rsvdISOSize);

    //
    // Calculate the size of the memory which can be safely onlined to the
    // kernel after accounting for different reserved memory requirements.
    //
    // Align rsvd memory to 64K granularity.
    // TODO : rsvdMemorySize is not finalized at this point of time in 
    // GH180, currently rsvdMemorySize is not increasing after this
    // point. This needs to be fixed.
    //
    totalRsvdBytes += NV_ALIGN_UP(pMemoryManager->rsvdMemorySize, 0x10000);
    totalRsvdBytes += (rsvdFastSize + rsvdSlowSize + rsvdISOSize);
    totalRsvdBytes += pMemoryManager->Ram.reservedMemSize;

    // For SRIOV guest, take into account FB tax paid on host side for each VF
    // This FB tax is non zero only for SRIOV guest RM environment.
    totalRsvdBytes += memmgrGetFbTaxSize_HAL(pGpu, pMemoryManager);

    //
    // TODO: make sure the onlineable memory is aligned to memblockSize
    // Currently, if we have leftover memory, it'll just be wasted because no
    // one can access it. If FB size itself is memblock size unaligned(because
    // of CBC and row remapper deductions), then the memory wastage is unavoidable.
    //
    numaOnlineSize = KMEMSYS_FB_NUMA_ONLINE_SIZE(fbSize - totalRsvdBytes, memblockSize);

    if (IS_PASSTHRU(pGpu) && pKernelMemorySystem->bBug3656943WAR)
    {
        // For passthrough case, reserved memory size is fixed as 1GB
        NvU64 rsvdSize = 1 * 1024 * 1024 * 1024;

        NV_ASSERT_OR_RETURN(rsvdSize >= totalRsvdBytes, NV_ERR_INVALID_STATE);
        totalRsvdBytes = rsvdSize;
        //
        // Aligning to hardcoded 512MB size as both host and guest need to use
        // the same alignment irrespective of the kernel page size. 512MB size
        // works for both 4K and 64K page size kernels but more memory is
        // wasted being part of non onlined region which can't be avoided
        // per the design.
        //
        numaOnlineSize = KMEMSYS_FB_NUMA_ONLINE_SIZE(fbSize - totalRsvdBytes, 512 * 1024 * 1024);
    }

    NV_PRINTF(LEVEL_INFO,
              "fbSize: 0x%llx NUMA reserved memory size: 0x%llx online memory size: 0x%llx\n",
              fbSize, totalRsvdBytes, numaOnlineSize);

    if (osNumaOnliningEnabled(pGpu->pOsGpuInfo))
    {
        pKernelMemorySystem->numaOnlineBase   = KMEMSYS_FB_NUMA_ONLINE_BASE;
        pKernelMemorySystem->numaOnlineSize   = numaOnlineSize;
        //
        // TODO: Bug 1945658: Soldier through on GPU memory add
        // failure(which is often possible because of missing auto online
        // setting) and instead check for failure on stateLoad.
        // Any failure in StateInit results in gpuStateDestroy not getting called.
        // kgspUnloadRm_IMPL from gpuStateDestroy also doesn't get called leaving
        // GSP in unclean state and requiring GPU reset to recover from that.
        //
        // kmemsysNumaAddMemory_HAL by itself cannot be called from stateLoad
        // because the memory mapping that follows this call site comes from linear
        // kernel virtual address when memory is added to the kernel vs the
        // VMALLOC_START region when memory is not added.
        //
        NV_ASSERT_OK(kmemsysNumaAddMemory_HAL(pGpu, pKernelMemorySystem, 0, 0,
                                              numaOnlineSize, &numaNodeId));
    }
    pGpu->numaNodeId = numaNodeId;

    NV_ASSERT_OK_OR_RETURN(kbusCreateCoherentCpuMapping_HAL(pGpu, pKernelBus, numaOnlineSize, bFlush));

    // Switch the toggle for coherent link mapping only if migration is successful
    pGpu->setProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING, NV_TRUE);

    NV_ASSERT_OK_OR_RETURN(kbusVerifyCoherentLink_HAL(pGpu, pKernelBus));

    return NV_OK;
}

/*!
 * @brief Teardown sysmem NVLink/C2C NUMA and ATS functionality
 *
 * @param[in] pGpu                 OBJGPU pointer
 * @param[in] pKernelMemorySystem  Kernel Memory System pointer
 * @param[in] bFlush               Whether the CPU cache of the GPU mapping
 *                                 should be flushed
 */
void
kmemsysTeardownCoherentCpuLink_IMPL
(
    OBJGPU            *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvBool             bFlush
)
{
    kbusTeardownCoherentCpuMapping_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), bFlush);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING, NV_FALSE);
}

NV_STATUS
kmemsysSendL2InvalidateEvict_IMPL
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32               flags)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS params = {0};

    params.flags = flags;

    return pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                           NV2080_CTRL_CMD_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT,
                           &params, sizeof(params));
}

NV_STATUS
kmemsysSendFlushL2AllRamsAndCaches_IMPL
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    return pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                           NV2080_CTRL_CMD_INTERNAL_MEMSYS_FLUSH_L2_ALL_RAMS_AND_CACHES,
                           NULL, 0);
}

NV_STATUS
kmemsysGetUsableFbSize_KERNEL
(
    OBJGPU               *pGpu,
    KernelMemorySystem   *pKernelMemorySystem,
    NvU64                *pFbSize
)
{
    return kmemsysReadUsableFbSize_HAL(pGpu, pKernelMemorySystem, pFbSize);
}

NV_STATUS
kmemsysStateLoad_IMPL(OBJGPU *pGpu, KernelMemorySystem *pKernelMemorySystem, NvU32 flags)
{
    NV_STATUS status = NV_OK;

    if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL(pGpu))
    {
        if ((flags & GPU_STATE_FLAGS_PRESERVING) &&
            (flags & GPU_STATE_FLAGS_PM_TRANSITION) &&
            !(flags & GPU_STATE_FLAGS_GC6_TRANSITION))
        {
            MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

            NV_ASSERT_OK_OR_ELSE(status,
                                 memmgrRestorePowerMgmtState(pGpu, pMemoryManager),
                                 memmgrFreeFbsrMemory(pGpu, pMemoryManager));
        }
    }

    return status;
}

NV_STATUS
kmemsysStateUnload_IMPL(OBJGPU *pGpu, KernelMemorySystem *pKernelMemorySystem, NvU32 flags)
{
    NV_STATUS status = NV_OK;

    if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL(pGpu))
    {
        if ((flags & GPU_STATE_FLAGS_PRESERVING) &&
            (flags & GPU_STATE_FLAGS_PM_TRANSITION) &&
            !(flags & GPU_STATE_FLAGS_GC6_TRANSITION))
        {
            MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

            NV_ASSERT_OK_OR_ELSE(status,
                                 memmgrSavePowerMgmtState(pGpu, pMemoryManager),
                                 memmgrFreeFbsrMemory(pGpu, pMemoryManager));
        }
    }

    return status;
}
