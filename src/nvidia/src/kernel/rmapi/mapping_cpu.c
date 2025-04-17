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
#include "core/locks.h"
#include "core/thread_state.h"
#include "os/os.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/generic_engine.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "mem_mgr/fla_mem.h"

#include "class/cl0000.h" // NV01_NULL_OBJECT

#include "resserv/rs_server.h"
#include "resserv/rs_client.h"
#include "resserv/rs_resource.h"

#include "class/cl0071.h" // NV01_MEMORY_SYSTEM_OS_DESCRIPTOR
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/bus/kern_bus.h"

#include "rmapi/rs_utils.h"
#include "rmapi/mapping_list.h"
#include "entry_points.h"

static void RmUnmapBusAperture (OBJGPU *, NvP64, NvU64, NvBool, NvP64);

#include "gpu/conf_compute/conf_compute.h"

typedef struct RS_CPU_MAP_PARAMS RmMapParams;
typedef struct RS_CPU_UNMAP_PARAMS RmUnmapParams;

NV_STATUS
rmapiMapGpuCommon
(
    RsResource *pResource,
    CALL_CONTEXT *pCallContext,
    RsCpuMapping *pCpuMapping,
    OBJGPU *pGpu,
    NvU32 regionOffset,
    NvU32 regionSize
)
{
    NV_STATUS rmStatus;
    RmClient *pClient = dynamicCast(pCallContext->pClient, RmClient);
    NvU64 offset;

    // Validate the offset and limit passed in.
    if (pCpuMapping->offset >= regionSize)
        return NV_ERR_INVALID_BASE;
    if (pCpuMapping->length == 0)
        return NV_ERR_INVALID_LIMIT;
    if ((pCpuMapping->offset + pCpuMapping->length > regionSize) ||
        !portSafeAddU64(pCpuMapping->offset, pCpuMapping->length, &offset))
        return NV_ERR_INVALID_LIMIT;

    if (!portSafeAddU64((NvU64)regionOffset, pCpuMapping->offset, &offset))
        return NV_ERR_INVALID_OFFSET;

    // Create a mapping of BAR0
    rmStatus = osMapGPU(pGpu,
                        rmclientGetCachedPrivilege(pClient),
                        offset,
                        pCpuMapping->length,
                        pCpuMapping->pPrivate->protect,
                        &pCpuMapping->pLinearAddress,
                        &pCpuMapping->pPrivate->pPriv);
    return rmStatus;
}



NV_STATUS
rmapiGetEffectiveAddrSpace
(
    OBJGPU *pGpu,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32 mapFlags,
    NV_ADDRESS_SPACE *pAddrSpace
)
{
    return kbusGetEffectiveAddressSpace_HAL(pGpu, pMemDesc, mapFlags, pAddrSpace);
}

// Asserts to check caching type matches across sdk and nv_memory_types
ct_assert(NVOS33_FLAGS_CACHING_TYPE_CACHED        == NV_MEMORY_CACHED);
ct_assert(NVOS33_FLAGS_CACHING_TYPE_UNCACHED      == NV_MEMORY_UNCACHED);
ct_assert(NVOS33_FLAGS_CACHING_TYPE_WRITECOMBINED == NV_MEMORY_WRITECOMBINED);
ct_assert(NVOS33_FLAGS_CACHING_TYPE_WRITEBACK     == NV_MEMORY_WRITEBACK);
ct_assert(NVOS33_FLAGS_CACHING_TYPE_DEFAULT       == NV_MEMORY_DEFAULT);
ct_assert(NVOS33_FLAGS_CACHING_TYPE_UNCACHED_WEAK == NV_MEMORY_UNCACHED_WEAK);

//
// Map memory entry points.
//
NV_STATUS
memMap_IMPL
(
    Memory *pMemory,
    CALL_CONTEXT *pCallContext,
    RS_CPU_MAP_PARAMS *pMapParams,
    RsCpuMapping *pCpuMapping
)
{
    OBJGPU *pGpu = NULL;
    KernelBus *pKernelBus = NULL;
    MemoryManager *pMemoryManager = NULL;
    KernelMemorySystem *pKernelMemorySystem = NULL;
    RmClient *pClient;
    RsResourceRef *pContextRef;
    RsResourceRef *pMemoryRef;
    Memory *pMemoryInfo; // TODO: rename this field. pMemoryInfo is the legacy name.
                         // Name should be clear on how pMemoryInfo different from pMemory
    MEMORY_DESCRIPTOR *pMemDesc;
    NvP64 priv = NvP64_NULL;
    NV_STATUS rmStatus = NV_OK;
    NV_ADDRESS_SPACE effectiveAddrSpace;
    NvBool bBroadcast;
    NvU64 mapLimit;
    NvBool bIsSysmem = NV_FALSE;
    NvBool bSkipSizeCheck = (DRF_VAL(OS33, _FLAGS, _SKIP_SIZE_CHECK, pMapParams->flags) ==
                             NVOS33_FLAGS_SKIP_SIZE_CHECK_ENABLE);

    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(pMapParams->pLockInfo != NULL, NV_ERR_INVALID_ARGUMENT);
    pContextRef = pMapParams->pLockInfo->pContextRef;
    if (pContextRef != NULL)
    {
        NV_ASSERT_OK_OR_RETURN(gpuGetByRef(pContextRef, &bBroadcast, &pGpu));
        gpuSetThreadBcState(pGpu, bBroadcast);

        pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
        pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
        pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    }

    pClient = serverutilGetClientUnderLock(pMapParams->hClient);
    NV_ASSERT_OR_ELSE(pClient != NULL, return NV_ERR_INVALID_CLIENT);
    NV_ASSERT_OK_OR_RETURN(clientGetResourceRef(staticCast(pClient, RsClient),
                pMapParams->hMemory, &pMemoryRef));

    pMemoryInfo = dynamicCast(pMemoryRef->pResource, Memory);
    NV_ASSERT_OR_RETURN(pMemoryInfo != NULL, NV_ERR_NOT_SUPPORTED);
    pMemDesc = pMemoryInfo->pMemDesc;

    if ((pMemoryInfo->categoryClassId == NV01_MEMORY_SYSTEM_OS_DESCRIPTOR) &&
        !(memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM &&
          RMCFG_FEATURE_PLATFORM_MODS))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // PROTECTED memory is memory which is hidden from the CPU and used for
    // storing protected content.  The CPU is not allowed to read it, but is
    // allowed to write it in order to initialize memory allocated within the
    // PROTECTED region.
    //
    // CPU to directly access protected memory is allowed on MODS
    //
    // The check below is for VPR and should be skipped for Hopper CC
    if ((pGpu != NULL) && !gpuIsCCFeatureEnabled(pGpu))
    {
        if ((pMemoryInfo->Flags & NVOS32_ALLOC_FLAGS_PROTECTED) &&
            (pMapParams->protect != NV_PROTECT_WRITEABLE) &&
            ! RMCFG_FEATURE_PLATFORM_MODS)
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    if ((pGpu != NULL) && gpuIsCCFeatureEnabled(pGpu) &&
        (pMemoryInfo->Flags & NVOS32_ALLOC_FLAGS_PROTECTED))
    {
        ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);
        //
        // If neither BAR1 nor PCIE as a whole is trusted, fail the mapping
        // for allocations in CPR region. Mapping should still succeed for
        // allocations in non-CPR region
        // Deny BAR1 access to CPU-RM by default irrespective of prod or devtools
        // mode. Some mappings made by CPU-RM may be allowed to go thorough in
        // devtools mode.
        // However, allow the mapping to go through on platforms where GSP-DMA
        // is not present e.g. MODS. User may have also set a regkey to force
        // BAR accesses.
        //
        if (((pCC != NULL) && !pCC->ccStaticInfo.bIsBar1Trusted &&
            !pCC->ccStaticInfo.bIsPcieTrusted) ||
            (IS_GSP_CLIENT(pGpu) && pMapParams->bKernel && !pKernelBus->bForceBarAccessOnHcc &&
             FLD_TEST_DRF(OS33, _FLAGS, _ALLOW_MAPPING_ON_HCC, _NO, pMapParams->flags)))
        {
            NV_PRINTF(LEVEL_ERROR, "BAR1 mapping to CPR vidmem not supported\n");
            NV_ASSERT(0);
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    if (!pMapParams->bKernel &&
        FLD_TEST_DRF(OS32, _ATTR2, _PROTECTION_USER, _READ_ONLY, pMemoryInfo->Attr2) &&
        (pMapParams->protect != NV_PROTECT_READABLE))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Validate the offset and limit passed in.
    if (pMapParams->offset >= pMemoryInfo->Length)
    {
        return NV_ERR_INVALID_BASE;
    }
    if (pMapParams->length == 0)
    {
        return NV_ERR_INVALID_LIMIT;
    }

    if (bSkipSizeCheck && (pCallContext->secInfo.privLevel < RS_PRIV_LEVEL_KERNEL))
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    //
    // See bug #140807 and #150889 - we need to pad memory mappings to past their
    // actual allocation size (to PAGE_SIZE+1) because of a buggy ms function so
    // skip the allocation size sanity check so the map operation still succeeds.
    //
    if (!portSafeAddU64(pMapParams->offset, pMapParams->length, &mapLimit) ||
        (!bSkipSizeCheck && (mapLimit > pMemoryInfo->Length)))
    {
        return NV_ERR_INVALID_LIMIT;
    }

    if (pGpu != NULL)
    {
        NV_ASSERT_OK_OR_RETURN(rmapiGetEffectiveAddrSpace(pGpu, memdescGetMemDescFromGpu(pMemDesc, pGpu), pMapParams->flags, &effectiveAddrSpace));
    }
    else
    {
        effectiveAddrSpace = ADDR_SYSMEM;
    }

    bIsSysmem = (effectiveAddrSpace == ADDR_SYSMEM) || (effectiveAddrSpace == ADDR_EGM);

    if (dynamicCast(pMemoryInfo, FlaMemory) != NULL)
    {
        NV_PRINTF(LEVEL_WARNING, "CPU mapping to FLA memory not allowed\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    //  NVLINK2 ATS: Coherent NVLINK mappings may be returned if the client
    //    doesn't specifically request PCI-E and if the surface is pitch.
    //
    if ((pGpu != NULL) && pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING) &&
        (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM))
    {
        NV_ASSERT(pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED));
        if ((memdescGetPteKind(pMemDesc) ==
            memmgrGetHwPteKindFromSwPteKind_HAL(pGpu, pMemoryManager, RM_DEFAULT_PTE_KIND)) && // pitch
            (!memdescGetFlag(memdescGetMemDescFromGpu(pMemDesc, pGpu), MEMDESC_FLAGS_ENCRYPTED)))
        {
            if (pMapParams->bKernel)
            {
                rmStatus = kbusMapCoherentCpuMapping_HAL(pGpu,
                                                         pKernelBus,
                                                         pMemDesc,
                                                         pMapParams->offset,
                                                         pMapParams->length,
                                                         pMapParams->protect,
                                                         pMapParams->ppCpuVirtAddr,
                                                         &priv);
                if (rmStatus != NV_OK)
                    return rmStatus;
            }
            else
            {

                //
                // Allocating mapping for user mode client
                // NOTE: This function intentionally leaves priv uninitialized.
                //       It simply copies the busAddress [argument 2] into ppCpuVirtAddr.
                //       During the FD mapping cleanup for bug 1784955, it is expected that
                //       this function will transition to storing the mapping parameters onto
                //       the FD.  Also note: All mapping parameters are ignored (!).
                //
                //   TODO: refactor this to either use the new osMapMemoryArea interface or unify with kernel in osMapSystemMemory

                rmStatus = NV_OK;
                *((NvU64*) pMapParams->ppCpuVirtAddr) = ((NvU64) pKernelMemorySystem->coherentCpuFbBase) +
                    ((NvU64) memdescGetPhysAddr(pMemDesc, AT_CPU, pMapParams->offset));
            }

            NV_PRINTF(LEVEL_INFO,
                      "NVLINK mapping allocated: AtsBase=0x%llx, _pteArray[0]=0x%llx, mappedCpuAddr=0x%llx, length=%d\n",
                      (NvU64)pKernelMemorySystem->coherentCpuFbBase,
                      (NvU64)((NvUPtr)pMemDesc->_pteArray[0]),
                      (*((NvU64 *)(pMapParams->ppCpuVirtAddr))),
                      (int)pMapParams->length);

            rmStatus = CliUpdateDeviceMemoryMapping(pCpuMapping,
                                                    pMapParams->bKernel,
                                                    priv,
                                                    *(pMapParams->ppCpuVirtAddr),
                                                    pMapParams->length,
                                                    0,
                                                    0,
                                                    pMapParams->flags);
            pCpuMapping->pPrivate->pGpu = pGpu;

            if (rmStatus != NV_OK)
                return rmStatus;

        }
        else
        {
            //
            // RM should fail gracefully when clients map FB in the Coherent link path with special KIND.
            // There is no GMMU in the Coherent link path, only regular KIND(GMK) is supported and other special
            // KIND(s) (like encrypted, compressed etc.) are not supported.
            //
            NV_PRINTF(LEVEL_ERROR, "Need BAR mapping on coherent link! FAIL!!\n");
            return NV_ERR_NOT_SUPPORTED;
        }
    }
    else if (effectiveAddrSpace == ADDR_FBMEM)
    {
        RmPhysAddr fbAddr = 0;
        NvBool bcState = NV_FALSE;
        NvU64 gpuVirtAddr = 0;
        NvU64 gpuMapLength = 0;
        MemoryArea memArea;
        NvBool bUseMemArea = NV_FALSE;

        //
        // MEMDESC_FLAGS_MAP_SYSCOH_OVER_BAR1 indicates a special mapping type of HW registers,
        // so map it as device memory (uncached).
        //
        NvU32 cachingType = NV_MEMORY_WRITECOMBINED;
        if (pMemDesc != NULL && !memdescHasSubDeviceMemDescs(pMemDesc))
        {
            cachingType = memdescGetFlag(pMemDesc, MEMDESC_FLAGS_MAP_SYSCOH_OVER_BAR1) ?
                          NV_MEMORY_UNCACHED : NV_MEMORY_WRITECOMBINED;
        }

        if (!kbusIsBar1PhysicalModeEnabled(pKernelBus))
        {
            //
            // For Multi-Board, the BC area has a NULL address range.  So we have
            // to bring in the master.
            //
            bcState = gpumgrGetBcEnabledStatus(pGpu);
            if (bcState)
            {
                pGpu = gpumgrGetParentGPU(pGpu);
                gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);
            }

            //
            // Allocate a GPU virtual address space for the video memory region
            // for those GPUs that support it.
            //

            gpuMapLength = pMapParams->length;

            //
            // If client ask for Direct mapping , we cannot do much here but just
            // simulate as it is non encrypted surface.
            // It is currently totaly for testing purpose.
            //
            NV_ASSERT(pGpu->busInfo.gpuPhysFbAddr);

            {
                Device *pDevice = NULL;

                // Below, we only map one GPU's address for CPU access, so we can use UNICAST here
                NvU32 busMapFbFlags = BUS_MAP_FB_FLAGS_MAP_UNICAST;
#if defined(NV_UNIX)
                busMapFbFlags |= pMapParams->bKernel ? 0 : BUS_MAP_FB_FLAGS_ALLOW_DISCONTIG;
#endif
                if(DRF_VAL(OS33, _FLAGS, _MAPPING, pMapParams->flags) == NVOS33_FLAGS_MAPPING_DIRECT)
                {
                    busMapFbFlags |= BUS_MAP_FB_FLAGS_DISABLE_ENCRYPTION;
                }

                switch (pMapParams->protect)
                {
                    case NV_PROTECT_READABLE:
                        busMapFbFlags |= BUS_MAP_FB_FLAGS_READ_ONLY;
                        break;
                    case NV_PROTECT_WRITEABLE:
                        busMapFbFlags |= BUS_MAP_FB_FLAGS_WRITE_ONLY;
                        break;
                }

                pMemDesc = memdescGetMemDescFromGpu(pMemDesc, pGpu);

                // WAR for Bug 3564398, need to allocate doorbell for windows differently
                if (RMCFG_FEATURE_PLATFORM_WINDOWS &&
                    memdescGetFlag(pMemDesc, MEMDESC_FLAGS_MAP_SYSCOH_OVER_BAR1))
                {
                    busMapFbFlags |= BUS_MAP_FB_FLAGS_MAP_DOWNWARDS;
                }

                (void) deviceGetByHandle(staticCast(pClient, RsClient),
                                         pMapParams->hDevice, &pDevice);
                bUseMemArea = NV_TRUE;
                rmStatus = kbusMapFbAperture_HAL(pGpu, pKernelBus,
                                                   pMemDesc, mrangeMake(pMapParams->offset, gpuMapLength),
                                                   &memArea, busMapFbFlags, pDevice);

                if (rmStatus == NV_OK)
                {
                    gpuVirtAddr = memArea.pRanges[0].start;
                }


            }

            if (rmStatus != NV_OK)
                goto _rmMapMemory_busFail;
        }
        else
        {
            NV_ASSERT_OR_RETURN(memdescGetContiguity(pMemDesc, AT_GPU),
                   NV_ERR_NOT_SUPPORTED);

            fbAddr = gpumgrGetGpuPhysFbAddr(pGpu) + memdescGetPte(pMemDesc, AT_GPU, 0) +
                     memdescGetPteAdjust(pMemDesc) + pMapParams->offset;
        }

        if (pMapParams->bKernel)
        {
            rmStatus = osMapPciMemoryKernel64(pGpu,
                                              (kbusIsBar1PhysicalModeEnabled(pKernelBus)?
                                              fbAddr: gpumgrGetGpuPhysFbAddr(pGpu) + gpuVirtAddr),
                                              pMapParams->length,
                                              pMapParams->protect,
                                              pMapParams->ppCpuVirtAddr,
                                              cachingType);
        }
        else if(!bUseMemArea)
        {
            rmStatus = osMapPciMemoryUser(pGpu->pOsGpuInfo,
                                          (kbusIsBar1PhysicalModeEnabled(pKernelBus)?
                                          fbAddr: gpumgrGetGpuPhysFbAddr(pGpu) + gpuVirtAddr),
                                          pMapParams->length,
                                          pMapParams->protect,
                                          pMapParams->ppCpuVirtAddr,
                                          &priv,
                                          cachingType);
        }
        else
        {
            NvU64 idx;
            NvU64 barAddr = gpumgrGetGpuPhysFbAddr(pGpu);

            for (idx = 0; idx < memArea.numRanges; idx++)
            {
                memArea.pRanges[idx].start += barAddr;
            }

            rmStatus = osMapPciMemoryAreaUser(pGpu->pOsGpuInfo,
                                          memArea,
                                          pMapParams->protect,
                                          cachingType,
                                          pMapParams->ppCpuVirtAddr,
                                          &priv);

            for (idx = 0; idx < memArea.numRanges; idx++)
            {
                memArea.pRanges[idx].start -= barAddr;
            }

        }

        //
        // It's possible that NVOS33_FLAGS_MAPPING is set to NVOS33_FLAGS_MAPPING_DIRECT
        // at this point--set it to REFLECTED to indicate that we aren't using
        // direct mapping.
        //
        pMapParams->flags = FLD_SET_DRF(OS33, _FLAGS, _MAPPING, _REFLECTED, pMapParams->flags);
        pMapParams->flags = FLD_SET_DRF_NUM(OS33, _FLAGS, _CACHING_TYPE, cachingType, pMapParams->flags);

        if (rmStatus != NV_OK)
            goto _rmMapMemory_pciFail;

        rmStatus = CliUpdateDeviceMemoryMapping(pCpuMapping,
                                                pMapParams->bKernel,
                                                priv,
                                                *(pMapParams->ppCpuVirtAddr),
                                                pMapParams->length,
                                                kbusIsBar1PhysicalModeEnabled(pKernelBus)
                                                    ? (NvU64)0
                                                    : gpuVirtAddr,
                                                kbusIsBar1PhysicalModeEnabled(pKernelBus)
                                                    ? (NvU64)0
                                                    : gpuMapLength,
                                                pMapParams->flags);

        if (bUseMemArea)
        {
            pCpuMapping->pPrivate->memArea = memArea;
        }
        pCpuMapping->pPrivate->pGpu = pGpu;

        if (rmStatus != NV_OK)
        {
            RmUnmapBusAperture(pGpu,
                               *(pMapParams->ppCpuVirtAddr),
                               pMapParams->length,
                               pMapParams->bKernel,
                               priv);
    _rmMapMemory_pciFail:
            if (!kbusIsBar1PhysicalModeEnabled(pKernelBus))
            {
                kbusUnmapFbAperture_HAL(pGpu,
                                          pKernelBus,
                                          pMemDesc,
                                          memArea,
                                          BUS_MAP_FB_FLAGS_MAP_UNICAST);
    _rmMapMemory_busFail:
                gpumgrSetBcEnabledStatus(pGpu, bcState);
            }
        }
    }
    else
    if (bIsSysmem)
    {
        // A client can specify not to map memory by default when
        // calling into RmAllocMemory. In those cases, we don't have
        // a mapping yet, so go ahead and map it for the client now.
        rmStatus = memdescMap(pMemDesc,
                              pMapParams->offset,
                              pMapParams->length,
                              pMapParams->bKernel,
                              pMapParams->protect,
                              pMapParams->ppCpuVirtAddr,
                              &priv);

        // Associate this mapping with the client
        if (rmStatus == NV_OK && *(pMapParams->ppCpuVirtAddr))
        {
            pMapParams->flags = FLD_SET_DRF(OS33, _FLAGS, _MAPPING, _DIRECT, pMapParams->flags);
            rmStatus = CliUpdateMemoryMappingInfo(pCpuMapping,
                                                  pMapParams->bKernel,
                                                  *(pMapParams->ppCpuVirtAddr),
                                                  priv,
                                                  pMapParams->length,
                                                  pMapParams->flags);
            pCpuMapping->pPrivate->pGpu = pGpu;
        }
    }
    else if (effectiveAddrSpace == ADDR_VIRTUAL)
    {
        rmStatus = NV_ERR_NOT_SUPPORTED;
    }
    else if (effectiveAddrSpace == ADDR_REGMEM)
    {
        RS_PRIV_LEVEL privLevel;

        privLevel = rmclientGetCachedPrivilege(pClient);
        if (!rmclientIsAdmin(pClient, privLevel) &&
            !memdescGetFlag(pMemDesc, MEMDESC_FLAGS_SKIP_REGMEM_PRIV_CHECK))
        {
            return NV_ERR_PROTECTION_FAULT;
        }

        if (DRF_VAL(OS33, _FLAGS, _MEM_SPACE, pMapParams->flags) == NVOS33_FLAGS_MEM_SPACE_USER)
        {
            privLevel = RS_PRIV_LEVEL_USER;
        }

        // Create a mapping of BAR0
        rmStatus = osMapGPU(pGpu,
                            privLevel,
                            pMapParams->offset + pMemDesc-> _pteArray[0],
                            pMapParams->length,
                            pMapParams->protect,
                            pMapParams->ppCpuVirtAddr,
                            &priv);
        if (rmStatus != NV_OK)
            return rmStatus;

        // Save off the mapping
        rmStatus = CliUpdateDeviceMemoryMapping(pCpuMapping,
                                                pMapParams->bKernel,
                                                priv,
                                                *(pMapParams->ppCpuVirtAddr),
                                                pMapParams->length,
                                                0, // gpu virtual addr
                                                0, // gpu map length
                                                pMapParams->flags);
        pCpuMapping->pPrivate->pGpu = pGpu;

        if (rmStatus != NV_OK)
        {
            osUnmapGPU(pGpu->pOsGpuInfo,
                       privLevel,
                       *(pMapParams->ppCpuVirtAddr),
                       pMapParams->length,
                       priv);
            return rmStatus;
        }
    }
    else
    {
        return NV_ERR_INVALID_CLASS;
    }

    if (rmStatus == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO,
                  "%s created. CPU Virtual Address: " NvP64_fmt "\n",
                  FLD_TEST_DRF(OS33, _FLAGS, _MAPPING, _DIRECT, pMapParams->flags) ? "Direct mapping" : "Mapping",
                  *(pMapParams->ppCpuVirtAddr));
    }

    return rmStatus;
}

NV_STATUS
memUnmap_IMPL
(
    Memory *pMemory,
    CALL_CONTEXT *pCallContext,
    RsCpuMapping *pCpuMapping
)
{
    RmClient           *pClient             = dynamicCast(pCallContext->pClient, RmClient);
    OBJGPU             *pGpu                = pCpuMapping->pPrivate->pGpu;
    MEMORY_DESCRIPTOR  *pMemDesc            = pMemory->pMemDesc;

    KernelBus          *pKernelBus          = NULL;
    MemoryManager      *pMemoryManager      = NULL;

    if (pGpu != NULL)
    {
        pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
        pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    }

    if (FLD_TEST_DRF(OS33, _FLAGS, _OS_DESCRIPTOR, _ENABLE, pCpuMapping->flags))
    {
        // Nothing more to do
    }
    else if ((pGpu != NULL) && pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING) &&
             (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM))
    {
        NV_ASSERT(pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED));
        NV_ASSERT((memdescGetPteKind(pMemDesc) ==
                   memmgrGetHwPteKindFromSwPteKind_HAL(pGpu, pMemoryManager, RM_DEFAULT_PTE_KIND)) && // pitch
                  (!memdescGetFlag(pMemDesc, MEMDESC_FLAGS_ENCRYPTED)));

        if (pCpuMapping->pPrivate->bKernel)
        {
            kbusUnmapCoherentCpuMapping_HAL(pGpu, pKernelBus, pMemDesc,
                                            pCpuMapping->pLinearAddress,
                                            pCpuMapping->pPrivate->pPriv);
        }

        NV_PRINTF(LEVEL_INFO,
                  "Unmapping from NVLINK handle = 0x%x, addr= 0x%llx\n",
                  RES_GET_HANDLE(pMemory), (NvU64)pCpuMapping->pLinearAddress);

        //
        // No BAR aperture mapping to delete.
        // No kernel mapping to remove
        // User-space will call munmap to eliminate PTE mappings
        //
    }
    // System Memory case
    else if ((pGpu == NULL) || (((memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM) ||
                                  (memdescGetAddressSpace(pMemDesc) == ADDR_EGM)) &&
                                 FLD_TEST_DRF(OS33, _FLAGS, _MAPPING, _DIRECT, pCpuMapping->flags)))
    {
        if (FLD_TEST_DRF(OS33, _FLAGS, _MAPPING, _DIRECT, pCpuMapping->flags))
        {
            memdescUnmap(pMemDesc,
                         pCpuMapping->pPrivate->bKernel,
                         pCpuMapping->processId,
                         pCpuMapping->pLinearAddress,
                         pCpuMapping->pPrivate->pPriv);
        }
    }
    else if ((memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM) ||
             ((memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM) &&
              FLD_TEST_DRF(OS33, _FLAGS, _MAPPING, _REFLECTED, pCpuMapping->flags)))
    {
        RmUnmapBusAperture(pGpu,
                           pCpuMapping->pLinearAddress,
                           pCpuMapping->length,
                           pCpuMapping->pPrivate->bKernel,
                           pCpuMapping->pPrivate->pPriv);

        if (!kbusIsBar1PhysicalModeEnabled(pKernelBus))
        {
            {
                kbusUnmapFbAperture_HAL(pGpu, pKernelBus,
                                          pMemory->pMemDesc,
                                          pCpuMapping->pPrivate->memArea,
                                          BUS_MAP_FB_FLAGS_MAP_UNICAST);
            }
        }
    }
    else if (memdescGetAddressSpace(pMemDesc) == ADDR_VIRTUAL)
    {
        NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_STATE);
    }
    else if (memdescGetAddressSpace(pMemDesc) == ADDR_REGMEM)
    {
        osUnmapGPU(pGpu->pOsGpuInfo,
                   rmclientGetCachedPrivilege(pClient),
                   pCpuMapping->pLinearAddress,
                   pCpuMapping->length,
                   pCpuMapping->pPrivate->pPriv);
    }
    return NV_OK;
}

NV_STATUS
rmapiValidateKernelMapping
(
    RS_PRIV_LEVEL privLevel,
    NvU32 flags,
    NvBool *pbKernel
)
{
    NvBool bKernel;
    NV_STATUS status = NV_OK;
    if (privLevel < RS_PRIV_LEVEL_KERNEL)
    {
        // only kernel clients should be specifying the user mapping flags
        if (DRF_VAL(OS33, _FLAGS, _MEM_SPACE, flags) == NVOS33_FLAGS_MEM_SPACE_USER)
            status = NV_ERR_INVALID_FLAGS;
        bKernel = NV_FALSE;
    }
    else
    {
        //
        // Kernel clients can only use the persistent flag if they are
        // doing a user mapping.
        //
        bKernel = (DRF_VAL(OS33, _FLAGS, _MEM_SPACE, flags) == NVOS33_FLAGS_MEM_SPACE_CLIENT);
    }

    // OS descriptor will already be mapped
    if (FLD_TEST_DRF(OS33, _FLAGS, _OS_DESCRIPTOR, _ENABLE, flags))
        status = NV_ERR_INVALID_FLAGS;

    if (pbKernel != NULL)
        *pbKernel = bKernel;

    return status;
}

NV_STATUS
serverMap_Prologue
(
    RsServer *pServer, RS_CPU_MAP_PARAMS *pMapParams
)
{
    NV_STATUS           rmStatus;
    RmClient           *pClient;
    RsResourceRef      *pMemoryRef;
    NvHandle            hClient = pMapParams->hClient;
    NvHandle            hParent = hClient;
    NvHandle            hSubDevice = NV01_NULL_OBJECT;
    NvBool              bClientAlloc = (hClient == pMapParams->hDevice);
    NvU32               flags = pMapParams->flags;
    RS_PRIV_LEVEL       privLevel;

    // Persistent sysmem mapping support is no longer supported
    if (DRF_VAL(OS33, _FLAGS, _PERSISTENT, flags) == NVOS33_FLAGS_PERSISTENT_ENABLE)
        return NV_ERR_INVALID_FLAGS;

    // Populate Resource Server information
    pClient = serverutilGetClientUnderLock(hClient);
    NV_ASSERT_OR_ELSE(pClient != NULL, return NV_ERR_INVALID_CLIENT);

    // Validate hClient
    privLevel = rmclientGetCachedPrivilege(pClient);

    // RS-TODO: Assert if this fails after all objects are converted
    NV_ASSERT_OK_OR_RETURN(clientGetResourceRef(staticCast(pClient, RsClient),
                pMapParams->hMemory, &pMemoryRef));

    if (pMemoryRef->pParentRef != NULL)
        hParent = pMemoryRef->pParentRef->hResource;

    // check if we have a user or kernel RM client
    rmStatus = rmapiValidateKernelMapping(privLevel, flags, &pMapParams->bKernel);
    if (rmStatus != NV_OK)
        return rmStatus;

    //
    // First check to see if it is a standard device or the BC region of
    // a MC adapter.
    //
    pMapParams->pLockInfo->flags |= RM_LOCK_FLAGS_NO_GPUS_LOCK;
    if (!bClientAlloc)
    {
        NV_ASSERT_OR_RETURN(hParent != hClient, NV_ERR_INVALID_OBJECT_PARENT);

        RsResourceRef *pContextRef;
        rmStatus = clientGetResourceRef(staticCast(pClient, RsClient),
                pMapParams->hDevice, &pContextRef);

        if (rmStatus != NV_OK)
            return rmStatus;

        if (pContextRef->internalClassId == classId(Device))
        {
        }
        else if (pContextRef->internalClassId == classId(Subdevice))
        {
            hSubDevice = pMapParams->hDevice;
            pMapParams->hDevice = pContextRef->pParentRef->hResource;
        }
        else
        {
            return NV_ERR_INVALID_OBJECT_PARENT;
        }

        pMapParams->pLockInfo->flags |= RM_LOCK_FLAGS_GPU_GROUP_LOCK;
        pMapParams->pLockInfo->pContextRef = pContextRef;
    }
    else
    {
        NV_ASSERT_OR_RETURN(hParent == hClient, NV_ERR_INVALID_OBJECT_PARENT);
    }

    pMapParams->hContext = (hSubDevice != NV01_NULL_OBJECT)
                      ? hSubDevice
                      : pMapParams->hDevice;


    // convert from OS33 flags to RM's memory protection flags
    switch (DRF_VAL(OS33, _FLAGS, _ACCESS, flags))
    {
        case NVOS33_FLAGS_ACCESS_READ_WRITE:
            pMapParams->protect = NV_PROTECT_READ_WRITE;
            break;
        case NVOS33_FLAGS_ACCESS_READ_ONLY:
            pMapParams->protect = NV_PROTECT_READABLE;
            break;
        case NVOS33_FLAGS_ACCESS_WRITE_ONLY:
            pMapParams->protect = NV_PROTECT_WRITEABLE;
            break;
        default:
            return NV_ERR_INVALID_FLAGS;
    }

    return NV_OK;
}

NV_STATUS
serverUnmap_Prologue
(
    RsServer *pServer,
    RS_CPU_UNMAP_PARAMS *pUnmapParams
)
{
    OBJGPU *pGpu = NULL;
    NV_STATUS rmStatus;
    RmClient *pClient;
    RsResourceRef *pMemoryRef;
    NvHandle hClient = pUnmapParams->hClient;
    NvHandle hParent = hClient;
    NvHandle hMemory = pUnmapParams->hMemory;
    NvBool bClientAlloc = (pUnmapParams->hDevice == pUnmapParams->hClient);
    NvBool bKernel;
    NvBool bBroadcast;
    NvU32 ProcessId = pUnmapParams->processId;
    RS_PRIV_LEVEL privLevel;
    void *pProcessHandle = NULL;

    // Populate Resource Server information
    pClient = serverutilGetClientUnderLock(hClient);
    NV_ASSERT_OR_ELSE(pClient != NULL, return NV_ERR_INVALID_CLIENT);

    // check if we have a user or kernel RM client
    privLevel = rmclientGetCachedPrivilege(pClient);

    // RS-TODO: Assert if this fails after all objects are converted
    NV_ASSERT_OK_OR_RETURN(clientGetResourceRef(staticCast(pClient, RsClient),
                hMemory, &pMemoryRef));

    if (pMemoryRef->pParentRef != NULL)
        hParent = pMemoryRef->pParentRef->hResource;

    //
    // First check to see if it is a standard device or the BC region of
    // a MC adapter.
    //
    pUnmapParams->pLockInfo->flags |= RM_LOCK_FLAGS_NO_GPUS_LOCK;
    if (!bClientAlloc)
    {
        NV_ASSERT_OR_RETURN(hParent != hClient, NV_ERR_INVALID_OBJECT_PARENT);

        RsResourceRef *pContextRef;
        rmStatus = clientGetResourceRef(staticCast(pClient, RsClient),
                pUnmapParams->hDevice, &pContextRef);

        if (rmStatus != NV_OK)
            return rmStatus;

        if (pContextRef->internalClassId == classId(Subdevice))
        {
            pUnmapParams->hDevice = pContextRef->pParentRef->hResource;
        }
        else if (pContextRef->internalClassId != classId(Device))
        {
            return NV_ERR_INVALID_OBJECT_PARENT;
        }

        pUnmapParams->pLockInfo->flags |= RM_LOCK_FLAGS_GPU_GROUP_LOCK;
        pUnmapParams->pLockInfo->pContextRef = pContextRef;
        NV_ASSERT_OK_OR_RETURN(gpuGetByRef(pUnmapParams->pLockInfo->pContextRef, &bBroadcast, &pGpu));
        gpuSetThreadBcState(pGpu, bBroadcast);
    }
    else
    {
        NV_ASSERT_OR_RETURN(hParent == hClient, NV_ERR_INVALID_OBJECT_PARENT);
    }

    // Decide what sort of mapping it is, user or kernel
    if (privLevel < RS_PRIV_LEVEL_KERNEL)
    {
        bKernel = NV_FALSE;
    }
    else
    {
        bKernel = (DRF_VAL(OS33, _FLAGS, _MEM_SPACE, pUnmapParams->flags) == NVOS33_FLAGS_MEM_SPACE_CLIENT);
    }

    //
    // If it's a user mapping, and we're not currently in the same process that
    // it's mapped into, then attempt to attach to the other process first.
    //
    if (!bKernel && (ProcessId != osGetCurrentProcess()))
    {
        rmStatus = osAttachToProcess(&pProcessHandle, ProcessId);
        if (rmStatus != NV_OK)
        {
            if (pUnmapParams->bTeardown)
                pProcessHandle = NULL;
            else
                return rmStatus;
        }

        pUnmapParams->pProcessHandle = pProcessHandle;
    }

    // Don't do any filtering if this is a tear-down path
    if (pUnmapParams->bTeardown)
    {
        pUnmapParams->fnFilter = NULL;
        return NV_OK;
    }


    pUnmapParams->fnFilter = bKernel
        ? serverutilMappingFilterKernel
        : serverutilMappingFilterCurrentUserProc;

    return NV_OK;
}

void
serverUnmap_Epilogue
(
    RsServer *pServer,
    RS_CPU_UNMAP_PARAMS *pUnmapParams
)
{
    // do we need to detach?
    if (pUnmapParams->pProcessHandle != NULL)
    {
        osDetachFromProcess(pUnmapParams->pProcessHandle);
        pUnmapParams->pProcessHandle = NULL;
    }
}

void RmUnmapBusAperture
(
    OBJGPU *pGpu,
    NvP64   pCpuVirtualAddress,
    NvU64   length,
    NvBool  bKernel,
    NvP64   pPrivateData
)
{
    if (bKernel)
    {
        osUnmapPciMemoryKernel64(pGpu, pCpuVirtualAddress);
    }
    else
    {
        osUnmapPciMemoryUser(pGpu->pOsGpuInfo, pCpuVirtualAddress, length, pPrivateData);
    }
}

NV_STATUS
rmapiMapToCpu
(
    RM_API   *pRmApi,
    NvHandle  hClient,
    NvHandle  hDevice,
    NvHandle  hMemory,
    NvU64     offset,
    NvU64     length,
    void    **ppCpuVirtAddr,
    NvU32     flags
)
{
    NvP64     pCpuVirtAddrNvP64 = NvP64_NULL;
    NV_STATUS status;

    if (!pRmApi->bHasDefaultSecInfo)
        return NV_ERR_NOT_SUPPORTED;

    status = pRmApi->MapToCpuWithSecInfo(pRmApi, hClient, hDevice, hMemory, offset, length,
                                          &pCpuVirtAddrNvP64, flags, &pRmApi->defaultSecInfo);

    if (ppCpuVirtAddr)
        *ppCpuVirtAddr = NvP64_VALUE(pCpuVirtAddrNvP64);

    return status;
}

/**
 * Call into Resource Server to register and execute a CPU mapping operation.
 *
 * Resource Server will:
 *    1. Callback into RM (serverMap_Prologue) to set up mapping parameters, mapping context object,
 *       and locking requirements
 *    2. Take locks (if required)
 *    3. Allocate and register a RsCpuMapping book-keeping entry on the target object's RsResourceRef
 *    4. Call the target object's mapping virtual function (xxxMap_IMPL, defined in RM)
 *    5. Setup back-references to the mapping context object (if required.) This mapping will automatically
 *       be unmapped if either the target object or mapping context object are freed.
 *    6. Release any locks taken
 */
NV_STATUS
rmapiMapToCpuWithSecInfoV2
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemory,
    NvU64              offset,
    NvU64              length,
    NvP64             *ppCpuVirtAddr,
    NvU32             *flags,
    API_SECURITY_INFO *pSecInfo
)
{
    NV_STATUS  status;
    RM_API_CONTEXT rmApiContext = {0};
    RmMapParams rmMapParams;
    RS_LOCK_INFO lockInfo;

    NV_PRINTF(LEVEL_INFO,
              "Nv04MapMemory: client:0x%x device:0x%x memory:0x%x\n", hClient,
              hDevice, hMemory);
    NV_PRINTF(LEVEL_INFO,
              "Nv04MapMemory:  offset: %llx length: %llx flags:0x%x\n",
              offset, length, *flags);

    status = rmapiPrologue(pRmApi, &rmApiContext);
    if (status != NV_OK)
        return status;

    NV_PRINTF(LEVEL_INFO, "MMU_PROFILER Nv04MapMemory 0x%x\n", *flags);

    portMemSet(&lockInfo, 0, sizeof(lockInfo));
    status = rmapiInitLockInfo(pRmApi, hClient, NV01_NULL_OBJECT, &lockInfo);
    if (status != NV_OK)
    {
        rmapiEpilogue(pRmApi, &rmApiContext);
        return status;
    }

    //
    // In the RTD3 case, the API lock isn't taken since it can be initiated
    // from another thread that holds the API lock and because we now hold
    // the GPU lock.
    //
    if (rmapiInRtd3PmPath())
    {
        lockInfo.flags |= RM_LOCK_FLAGS_NO_API_LOCK;
        lockInfo.state &= ~RM_LOCK_STATES_API_LOCK_ACQUIRED;
    }

    LOCK_METER_DATA(MAPMEM, flags, 0, 0);

    // clear params for good measure
    portMemSet(&rmMapParams, 0, sizeof (rmMapParams));

    // load user args
    rmMapParams.hClient = hClient;
    rmMapParams.hDevice = hDevice;
    rmMapParams.hMemory = hMemory;
    rmMapParams.offset = offset;
    rmMapParams.length = length;
    rmMapParams.ppCpuVirtAddr = ppCpuVirtAddr;
    rmMapParams.flags = *flags;
    rmMapParams.pLockInfo = &lockInfo;
    rmMapParams.pSecInfo = pSecInfo;

    status = serverMap(&g_resServ, rmMapParams.hClient, rmMapParams.hMemory, &rmMapParams);

    rmapiEpilogue(pRmApi, &rmApiContext);

    *flags = rmMapParams.flags;

    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Nv04MapMemory: complete\n");
        NV_PRINTF(LEVEL_INFO,
                  "Nv04MapMemory:  *ppCpuVirtAddr:" NvP64_fmt "\n",
                  *ppCpuVirtAddr);
    }
    else
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Nv04MapMemory: map failed; status: %s (0x%08x)\n",
                  nvstatusToString(status), status);
    }

    return status;
}

NV_STATUS
rmapiMapToCpuWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemory,
    NvU64              offset,
    NvU64              length,
    NvP64             *ppCpuVirtAddr,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
)
{
    return rmapiMapToCpuWithSecInfoV2(pRmApi, hClient,
        hDevice, hMemory, offset, length, ppCpuVirtAddr,
        &flags, pSecInfo);
}

NV_STATUS
rmapiMapToCpuWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemory,
    NvU64              offset,
    NvU64              length,
    NvP64             *ppCpuVirtAddr,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS         status;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    status = rmapiMapToCpuWithSecInfoV2(pRmApi, hClient, hDevice, hMemory, offset, length, ppCpuVirtAddr, &flags, pSecInfo);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    return status;
}
NV_STATUS
rmapiMapToCpuWithSecInfoTlsV2
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemory,
    NvU64              offset,
    NvU64              length,
    NvP64             *ppCpuVirtAddr,
    NvU32             *flags,
    API_SECURITY_INFO *pSecInfo
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS         status;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    status = rmapiMapToCpuWithSecInfoV2(pRmApi, hClient, hDevice, hMemory, offset, length, ppCpuVirtAddr, flags, pSecInfo);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    return status;
}

NV_STATUS
rmapiUnmapFromCpu
(
    RM_API   *pRmApi,
    NvHandle  hClient,
    NvHandle  hDevice,
    NvHandle  hMemory,
    void     *pLinearAddress,
    NvU32     flags,
    NvU32     ProcessId
)
{
    if (!pRmApi->bHasDefaultSecInfo)
        return NV_ERR_NOT_SUPPORTED;

    return pRmApi->UnmapFromCpuWithSecInfo(pRmApi, hClient, hDevice, hMemory, NV_PTR_TO_NvP64(pLinearAddress),
                                           flags, ProcessId, &pRmApi->defaultSecInfo);
}

/**
 * Call into Resource Server to execute a CPU unmapping operation.
 *
 * Resource Server will:
 *    1. Callback into RM (serverUnmap_Prologue) to set up unmapping parameters, locking requirements,
 *       and attempt to attach to the mapping's user process (for user mappings only)
 *    2. Take locks (if required)
 *    3. Lookup the mapping
 *    4. Call the target object's unmapping virtual function (xxxUnmap_IMPL, defined in RM)
 *    5. Unregister the mapping from its back-references, and free the mapping
 *    6. Callback into RM (serverUnmap_Epilogue) to detach from the mapping's user process (if required)
 *    7. Release any locks taken
 */
NV_STATUS
rmapiUnmapFromCpuWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemory,
    NvP64              pLinearAddress,
    NvU32              flags,
    NvU32              ProcessId,
    API_SECURITY_INFO *pSecInfo
)
{
    NV_STATUS status;
    RM_API_CONTEXT rmApiContext = {0};
    RmUnmapParams rmUnmapParams;
    RS_LOCK_INFO lockInfo;

    NV_PRINTF(LEVEL_INFO,
              "Nv04UnmapMemory: client:0x%x device:0x%x memory:0x%x pLinearAddr:" NvP64_fmt " flags:0x%x\n",
              hClient, hDevice, hMemory, pLinearAddress, flags);

    status = rmapiPrologue(pRmApi, &rmApiContext);
    if (status != NV_OK)
        return status;

    portMemSet(&lockInfo, 0, sizeof(lockInfo));
    status = rmapiInitLockInfo(pRmApi, hClient, NV01_NULL_OBJECT, &lockInfo);
    if (status != NV_OK)
    {
        rmapiEpilogue(pRmApi, &rmApiContext);
        return NV_OK;
    }

    LOCK_METER_DATA(UNMAPMEM, flags, 0, 0);

    portMemSet(&rmUnmapParams, 0, sizeof (rmUnmapParams));
    rmUnmapParams.hClient = hClient;
    rmUnmapParams.hDevice = hDevice;
    rmUnmapParams.hMemory = hMemory;
    rmUnmapParams.pLinearAddress = pLinearAddress;
    rmUnmapParams.flags = flags;
    rmUnmapParams.processId = ProcessId;
    rmUnmapParams.pLockInfo = &lockInfo;
    rmUnmapParams.pSecInfo = pSecInfo;

    status = serverUnmap(&g_resServ, hClient, hMemory, &rmUnmapParams);

    rmapiEpilogue(pRmApi, &rmApiContext);

    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Nv04UnmapMemory: unmap complete\n");
    }
    else
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Nv04UnmapMemory: unmap failed; status: %s (0x%08x)\n",
                  nvstatusToString(status), status);
    }

    return status;
}

NV_STATUS
rmapiUnmapFromCpuWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemory,
    NvP64              pLinearAddress,
    NvU32              flags,
    NvU32              ProcessId,
    API_SECURITY_INFO *pSecInfo
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS         status;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    status = rmapiUnmapFromCpuWithSecInfo(pRmApi, hClient, hDevice, hMemory, pLinearAddress,
                                          flags, ProcessId, pSecInfo);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    return status;
}

NV_STATUS
serverMapLookupLockFlags
(
    RsServer *pServer,
    RS_LOCK_ENUM lock,
    RS_CPU_MAP_PARAMS *pParams,
    LOCK_ACCESS_TYPE *pAccess
)
{
    NV_ASSERT_OR_RETURN(pAccess != NULL, NV_ERR_INVALID_ARGUMENT);

    *pAccess = (serverSupportsReadOnlyLock(pServer, lock, RS_API_MAP))
        ? LOCK_ACCESS_READ
        : LOCK_ACCESS_WRITE;
    return NV_OK;
}

NV_STATUS
serverUnmapLookupLockFlags
(
    RsServer *pServer,
    RS_LOCK_ENUM lock,
    RS_CPU_UNMAP_PARAMS *pParams,
    LOCK_ACCESS_TYPE *pAccess
)
{
    NV_ASSERT_OR_RETURN(pAccess != NULL, NV_ERR_INVALID_ARGUMENT);

    *pAccess = (serverSupportsReadOnlyLock(pServer, lock, RS_API_UNMAP))
        ? LOCK_ACCESS_READ
        : LOCK_ACCESS_WRITE;
    return NV_OK;
}

NV_STATUS
refAllocCpuMappingPrivate
(
    RS_CPU_MAP_PARAMS *pMapParams,
    RsCpuMapping *pCpuMapping
)
{
    pCpuMapping->pPrivate = portMemAllocNonPaged(sizeof(RS_CPU_MAPPING_PRIVATE));
    if (pCpuMapping->pPrivate == NULL)
        return NV_ERR_NO_MEMORY;

    pCpuMapping->pPrivate->protect = pMapParams->protect;
    pCpuMapping->pPrivate->bKernel = pMapParams->bKernel;

    return NV_OK;
}

void
refFreeCpuMappingPrivate
(
    RsCpuMapping *pCpuMapping
)
{
    portMemFree(pCpuMapping->pPrivate);
}
