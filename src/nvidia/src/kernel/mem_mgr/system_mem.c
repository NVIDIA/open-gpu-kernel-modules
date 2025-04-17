/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "mem_mgr/system_mem.h"
#include "vgpu/rpc.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "core/locks.h"
#include "os/os.h"
#include "rmapi/client.h"
#include "deprecated/rmapi_deprecated.h"
#include "gpu/mem_mgr/mem_utils.h"
#include "core/system.h"
#include "ctrl/ctrl0000/ctrl0000gpu.h"

#include "gpu/mem_sys/kern_mem_sys.h"

#include "kernel/gpu/rc/kernel_rc.h"
#include "Nvcm.h"

#include "class/cl003e.h" // NV01_MEMORY_SYSTEM

/*!
 * sysmemConstruct
 *
 * @brief
 *     This routine provides common allocation services used by the
 *     following heap allocation functions:
 *       NVOS32_FUNCTION_ALLOC_SIZE
 *       NVOS32_FUNCTION_ALLOC_SIZE_RANGE
 *       NVOS32_FUNCTION_ALLOC_TILED_PITCH_HEIGHT
 *
 * @param[in]  pSystemMemory     Pointer to SystemMemory object
 * @param[in]  pCallContext      Pointer to the current CALL_CONTEXT.
 * @param[in]  pParams           Pointer to the alloc params
 *
 * @return 'NV_OK'
 *     Operation completed successfully.
 * @return 'NV_ERR_NO_MEMORY'
 *     There is not enough available memory to satisfy allocation request.
 * @return 'NV_ERR_INSUFFICIENT_RESOURCES'
 *     Not enough available resources to satisfy allocation request.
 */
NV_STATUS
sysmemConstruct_IMPL
(
    SystemMemory *pSystemMemory,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    Memory *pMemory = staticCast(pSystemMemory, Memory);
    NV_MEMORY_ALLOCATION_PARAMS *pAllocData = pParams->pAllocParams;
    MEMORY_ALLOCATION_REQUEST allocRequest = {0};
    MEMORY_ALLOCATION_REQUEST *pAllocRequest = &allocRequest;
    OBJGPU *pGpu = pMemory->pGpu;
    HWRESOURCE_INFO hwResource;
    RsResourceRef *pResourceRef = pCallContext->pResourceRef;
    NvU32 gpuCacheAttrib;
    NV_STATUS rmStatus = NV_OK;
    NvHandle hClient = pCallContext->pClient->hClient;
    RmClient *pRmClient = dynamicCast(pCallContext->pClient, RmClient);
    NvHandle hParent = pCallContext->pResourceRef->pParentRef->hResource;
    NvU64 sizeOut;
    NvU64 offsetOut;
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU32 Cache;
    NvU32 flags;
    RM_ATTR_PAGE_SIZE pageSizeAttr;

    NV_ASSERT_OR_RETURN(pRmClient != NULL, NV_ERR_INVALID_CLIENT);

    // Copy-construction has already been done by the base Memory class
    if (RS_IS_COPY_CTOR(pParams))
        return NV_OK;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, stdmemValidateParams(pGpu, pRmClient, pAllocData));
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                       DRF_VAL(OS32, _ATTR, _LOCATION, pAllocData->attr) != NVOS32_ATTR_LOCATION_VIDMEM &&
                           !(pAllocData->flags & NVOS32_ALLOC_FLAGS_VIRTUAL),
                       NV_ERR_INVALID_ARGUMENT);

    stdmemDumpInputAllocParams(pAllocData, pCallContext);

    // send it through the regular allocator even though it is for sysmem
    pAllocRequest->classNum = NV01_MEMORY_SYSTEM;
    pAllocRequest->pUserParams = pAllocData;
    pAllocRequest->hMemory = pResourceRef->hResource;
    pAllocRequest->hClient = hClient;
    pAllocRequest->hParent = hParent;
    pAllocRequest->pGpu = pGpu;
    pAllocRequest->internalflags = NVOS32_ALLOC_INTERNAL_FLAGS_CLIENTALLOC;
    pAllocRequest->pHwResource = &hwResource;

    // Unsure if we need to keep separate copies, but keeping old behavior for now.
    sizeOut = pAllocData->size;
    offsetOut = pAllocData->offset;

    {
        //
        // If using thwap to generate an allocation failure here, fail the
        // alloc right away.
        //
        KernelRc *pKernelRc = GPU_GET_KERNEL_RC(pGpu);
        if (pKernelRc != NULL &&
            !krcTestAllowAlloc(pGpu, pKernelRc,
                               NV_ROBUST_CHANNEL_ALLOCFAIL_HEAP))
        {
            rmStatus = NV_ERR_INSUFFICIENT_RESOURCES;
            goto failed;
        }
    }

    rmStatus = sysmemInitAllocRequest(pGpu, pSystemMemory, pAllocRequest);

    if (rmStatus != NV_OK)
        goto failed;

    NV_ASSERT(pAllocRequest->pMemDesc);
    pMemDesc = pAllocRequest->pMemDesc;

    // Copy final heap size/offset back to client struct
    //
    // What should we return ?. System or the Device physical address.
    // Return the Device physical address for now.
    // May change with the heap refactoring !.
    //
    // System and Device physical address can be got using the nv0041CtrlCmdGetSurfacePhysAttr ctrl call
    offsetOut = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
    sizeOut = pMemDesc->Size;
    pAllocData->limit = sizeOut - 1;

    memSetSysmemCacheAttrib(pGpu, pAllocData, &Cache, &gpuCacheAttrib);

    ct_assert(NVOS32_ATTR_COHERENCY_UNCACHED      == NVOS02_FLAGS_COHERENCY_UNCACHED);
    ct_assert(NVOS32_ATTR_COHERENCY_CACHED        == NVOS02_FLAGS_COHERENCY_CACHED);
    ct_assert(NVOS32_ATTR_COHERENCY_WRITE_COMBINE == NVOS02_FLAGS_COHERENCY_WRITE_COMBINE);
    ct_assert(NVOS32_ATTR_COHERENCY_WRITE_THROUGH == NVOS02_FLAGS_COHERENCY_WRITE_THROUGH);
    ct_assert(NVOS32_ATTR_COHERENCY_WRITE_PROTECT == NVOS02_FLAGS_COHERENCY_WRITE_PROTECT);
    ct_assert(NVOS32_ATTR_COHERENCY_WRITE_BACK    == NVOS02_FLAGS_COHERENCY_WRITE_BACK);

    flags = DRF_DEF(OS02, _FLAGS, _LOCATION, _PCI) |
            DRF_DEF(OS02, _FLAGS, _MAPPING, _NO_MAP) |
            DRF_NUM(OS02, _FLAGS, _COHERENCY, DRF_VAL(OS32, _ATTR, _COHERENCY, pAllocData->attr));

    NV_ASSERT(memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM);
    memdescSetCpuCacheAttrib(pMemDesc, Cache);

    if (pCallContext->secInfo.privLevel < RS_PRIV_LEVEL_KERNEL)
        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_KERNEL_MODE, NV_FALSE);

    if (pAllocData->flags & NVOS32_ALLOC_FLAGS_TURBO_CIPHER_ENCRYPTED)
        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_ENCRYPTED, NV_TRUE);

    if (FLD_TEST_DRF(OS32, _ATTR2, _NISO_DISPLAY, _YES, pAllocData->attr2))
        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_MEMORY_TYPE_DISPLAY_NISO, NV_TRUE);

    memdescSetFlag(pMemDesc, MEMDESC_FLAGS_SYSMEM_OWNED_BY_CLIENT, NV_TRUE);

    if ((sysGetStaticConfig(SYS_GET_INSTANCE()))->bOsCCEnabled &&
        gpuIsCCorApmFeatureEnabled(pGpu) &&
        FLD_TEST_DRF(OS32, _ATTR2, _MEMORY_PROTECTION, _UNPROTECTED,
                     pAllocData->attr2))
        {
            memdescSetFlag(pMemDesc, MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY,
                           NV_TRUE);
        }

    memdescSetGpuCacheAttrib(pMemDesc, gpuCacheAttrib);

    if (FLD_TEST_DRF(OS32, _ATTR2, _FIXED_NUMA_NODE_ID, _YES, pAllocData->attr2))
    {

        if (memdescGetFlag(pMemDesc, MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY))
        {
            NV_PRINTF(LEVEL_ERROR, "Cannot specify NUMA node in unprotected memory.\n");
            memdescDestroy(pMemDesc);
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            goto failed;
        }

        if ((pGpu->cpuNumaNodeId != NV0000_CTRL_NO_NUMA_NODE) && 
            (pAllocData->numaNode != pGpu->cpuNumaNodeId))
        {
            NV_PRINTF(LEVEL_ERROR, "NUMA node mismatch. Requested node: %u CPU node: %u\n",
                      pAllocData->numaNode, pGpu->cpuNumaNodeId);
            memdescDestroy(pMemDesc);
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            goto failed;
        }

        memdescSetNumaNode(pMemDesc, pAllocData->numaNode);
    }

    pageSizeAttr = dmaNvos32ToPageSizeAttr(pAllocData->attr, pAllocData->attr2);
    rmStatus = memmgrSetMemDescPageSize_HAL(pGpu, GPU_GET_MEMORY_MANAGER(pGpu), pMemDesc,
                                            AT_GPU, pageSizeAttr);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to set memdesc page size\n");
        memdescDestroy(pMemDesc);
        goto failed;
    }

    memdescTagAlloc(rmStatus, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_132, 
                    pMemDesc);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "*** Cannot allocate sysmem through fb heap\n");
        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);
        goto failed;
    }

    if (FLD_TEST_DRF(OS32, _ATTR2, _NISO_DISPLAY, _YES, pAllocData->attr2))
    {
        KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
        NvU64 physAddrStart;
        NvU64 physAddrEnd;

        NV_ASSERT_OR_RETURN(pKernelDisplay != NULL, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(pKernelDisplay->pStaticInfo != NULL, NV_ERR_INVALID_STATE);

        if (pKernelDisplay->pStaticInfo->bFbRemapperEnabled)
        {
            KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

            physAddrStart = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
            physAddrEnd = memdescGetPhysAddr(pMemDesc, AT_GPU, pAllocData->limit);

            rmStatus = kmemsysCheckDisplayRemapperRange_HAL(pGpu, pKernelMemorySystem, physAddrStart, physAddrEnd);

            if (rmStatus != NV_OK)
            {
                memdescFree(pMemDesc);
                memdescDestroy(pMemDesc);
                goto failed;
            }
        }
    }

    // ClientDB can set the pagesize for memdesc.
    // With GPU SMMU mapping, this needs to be set on the SMMU memdesc.
    // So SMMU allocation should happen before memConstructCommon()
    // Eventually SMMU allocation will be part of memdescAlloc().

    // An SMMU mapping will be added to SYSMEM allocations in the following cases:
    // 1. BIG page allocations with non-contiguous SYSMEM in Tegra.
    // 2. RM clients forcing SMMU mapping via flags.
    //    GPU Arch verification with VPR is one such usecase.
    //
    // fbAlloc_GF100() will set the page size attribute to BIG for these cases.

    if (FLD_TEST_DRF(OS32, _ATTR2, _SMMU_ON_GPU, _ENABLE, pAllocData->attr2))
    {
        NV_PRINTF(LEVEL_ERROR, "SMMU mapping allocation is not supported.\n");
        NV_ASSERT(0);
        rmStatus = NV_ERR_NOT_SUPPORTED;

        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);
        goto failed;
    }
    else if ((FLD_TEST_DRF(OS32, _ATTR, _PAGE_SIZE, _BIG, pAllocData->attr) ||
              FLD_TEST_DRF(OS32, _ATTR, _PAGE_SIZE, _HUGE, pAllocData->attr)) &&
              FLD_TEST_DRF(OS32, _ATTR, _PHYSICALITY, _NONCONTIGUOUS, pAllocData->attr))
    {
        NvBool bLargePageNonContigAllocSupported;

        //
        // The below checks verify whether a non-contig large page size sysmem request is supported by
        // the platform. In the MODS case we can only support large page size sysmem requests
        // if MODS supplied the sysmem page size regkey that is >4K.
        // This check could be performed better, but avoiding a change in MODS behavior for now.
        //
        if (RMCFG_FEATURE_PLATFORM_UNIX)
        {
            bLargePageNonContigAllocSupported = NV_TRUE;
        }
        else if (RMCFG_FEATURE_MODS_FEATURES)
        {
            StandardMemory *pStdMemory = staticCast(pSystemMemory, StandardMemory);

            if (stdmemGetSysmemPageSize_HAL(pGpu, pStdMemory) > RM_PAGE_SIZE)
            {
                bLargePageNonContigAllocSupported = NV_TRUE;
            }
            else
            {
                bLargePageNonContigAllocSupported = NV_FALSE;
            }
        }
        else
        {
            bLargePageNonContigAllocSupported = NV_FALSE;
        }

        if (!bLargePageNonContigAllocSupported)
        {
            NV_PRINTF(LEVEL_ERROR,
                "Non-contiguous allocation not supported where requested page size is larger than sysmem page size.\n");
            rmStatus = NV_ERR_NOT_SUPPORTED;

            memdescFree(pMemDesc);
            memdescDestroy(pMemDesc);
            goto failed;
        }
    }

    rmStatus = memConstructCommon(pMemory, pAllocRequest->classNum, flags, pMemDesc, 0,
                                  NULL, pAllocData->attr, pAllocData->attr2, 0, 0,
                                  pAllocData->tag, &hwResource);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "*** Cannot add symem through fb heap to client db\n");
        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);
        goto failed;
    }

    //
    // We need to force a kernel mapping of system memory-backed notifiers
    // allocated in this path.
    //
    if (pAllocData->type == NVOS32_TYPE_NOTIFIER)
    {
        rmStatus = memCreateKernelMapping(pMemory, NV_PROTECT_READ_WRITE, NV_FALSE);
        if (rmStatus != NV_OK)
        {
            memDestructCommon(pMemory);
            memdescFree(pMemDesc);
            memdescDestroy(pMemDesc);
            goto failed;
        }
    }

    if (IS_VIRTUAL(pGpu))
    {
        NvU32 os02Flags;
        NvU32 os32Flags = pAllocData->flags;

        // NVOS32_TYPE_NOTIFIER notifier indicates kernel mapping in this path
        if (pAllocData->type == NVOS32_TYPE_NOTIFIER)
            os32Flags |= NVOS32_ALLOC_FLAGS_KERNEL_MAPPING_MAP;

        //
        // Calculate os02flags as VGPU plugin allocates sysmem with legacy
        // RmAllocMemory API
        //
        rmStatus = RmDeprecatedConvertOs32ToOs02Flags(pAllocData->attr,
                                                      pAllocData->attr2,
                                                      os32Flags,
                                                      &os02Flags);

        if (rmStatus == NV_OK)
        {
            //
            // vGPU:
            //
            // Since vGPU does all real hardware management in the
            // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
            // do an RPC to the host to do the hardware update.
            //
            NV_RM_RPC_ALLOC_MEMORY(pGpu,
                                   hClient,
                                   hParent,
                                   pAllocRequest->hMemory,
                                   pAllocRequest->classNum,
                                   os02Flags,
                                   pMemDesc,
                                   rmStatus);
        }

        if (rmStatus != NV_OK)
        {
            memDestructCommon(pMemory);
            memdescFree(pMemDesc);
            memdescDestroy(pMemDesc);
            goto failed;
        }

        pMemory->bRpcAlloc = NV_TRUE;
    }

    pAllocData->size = sizeOut;
    pAllocData->offset = offsetOut;

    stdmemDumpOutputAllocParams(pAllocData);

failed:
    return rmStatus;
}

NV_STATUS
sysmemCtrlCmdGetSurfaceNumPhysPages_IMPL
(
    SystemMemory *pSystemMemory,
    NV003E_CTRL_GET_SURFACE_NUM_PHYS_PAGES_PARAMS *pParams
)
{
    Memory *pMemory = staticCast(pSystemMemory, Memory);
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(memdescGetAddressSpace(pMemory->pMemDesc) == ADDR_SYSMEM, NV_ERR_NOT_SUPPORTED);

    status = osGetNumMemoryPages(pMemory->pMemDesc,
                                 &pParams->numPages);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get sysmem pages\n");
    }

    return status;
}

NV_STATUS
sysmemCtrlCmdGetSurfacePhysPages_IMPL
(
    SystemMemory *pSystemMemory,
    NV003E_CTRL_GET_SURFACE_PHYS_PAGES_PARAMS *pParams
)
{
    Memory *pMemory = staticCast(pSystemMemory, Memory);
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(memdescGetAddressSpace(pMemory->pMemDesc) == ADDR_SYSMEM, NV_ERR_NOT_SUPPORTED);

    status = osGetMemoryPages(pMemory->pMemDesc,
                              NvP64_VALUE(pParams->pPages),
                              &pParams->numPages);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get sysmem pages\n");
    }

    return status;
}

NV_STATUS
sysmemInitAllocRequest_HMM
(
    OBJGPU *pGpu,
    SystemMemory *pSystemMemory,
    MEMORY_ALLOCATION_REQUEST *pAllocRequest
)
{
    MemoryManager               *pMemoryManager     = GPU_GET_MEMORY_MANAGER(pGpu);
    FB_ALLOC_INFO               *pFbAllocInfo       = NULL;
    FB_ALLOC_PAGE_FORMAT        *pFbAllocPageFormat = NULL;
    NV_STATUS                    status             = NV_OK;

    pFbAllocInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
    NV_ASSERT_TRUE_OR_GOTO(status, pFbAllocInfo != NULL, NV_ERR_NO_MEMORY, done);

    pFbAllocPageFormat = portMemAllocNonPaged(sizeof(FB_ALLOC_PAGE_FORMAT));
    NV_ASSERT_TRUE_OR_GOTO(status, pFbAllocPageFormat != NULL, NV_ERR_NO_MEMORY, done);

    portMemSet(pFbAllocInfo, 0, sizeof(FB_ALLOC_INFO));
    portMemSet(pFbAllocPageFormat, 0, sizeof(FB_ALLOC_PAGE_FORMAT));
    pFbAllocInfo->pageFormat = pFbAllocPageFormat;

    memUtilsInitFBAllocInfo(pAllocRequest->pUserParams, pFbAllocInfo, pAllocRequest->hClient, pAllocRequest->hParent);

    NV_ASSERT_OK_OR_GOTO(status,
        memmgrAllocResources(pGpu, pMemoryManager, pAllocRequest, pFbAllocInfo),
        done);

    NV_ASSERT_OK_OR_GOTO(status,
        sysmemAllocResources(pGpu, pMemoryManager, pAllocRequest, pFbAllocInfo, pSystemMemory),
        done);

done:
    portMemFree(pFbAllocPageFormat);
    portMemFree(pFbAllocInfo);

    return status;
}

NV_STATUS
sysmemAllocResources
(
    OBJGPU                      *pGpu,
    MemoryManager               *pMemoryManager,
    MEMORY_ALLOCATION_REQUEST   *pAllocRequest,
    FB_ALLOC_INFO               *pFbAllocInfo,
    SystemMemory                *pSystemMemory
)
{
    NV_STATUS                    status          = NV_OK;
    NV_MEMORY_ALLOCATION_PARAMS *pVidHeapAlloc   = pAllocRequest->pUserParams;
    MEMORY_DESCRIPTOR           *pMemDesc        = NULL;
    NvBool                       bAllocedMemDesc = NV_FALSE;
    NvBool                       bContig         = FLD_TEST_DRF(OS32, _ATTR, _PHYSICALITY,
                                                                _CONTIGUOUS, pVidHeapAlloc->attr);
    //
    // BUG 3506666
    // While replaying a trace, it is possible for the playback OS to have a smaller page size
    // than the capture OS so if we're running a replay where the requested page size is larger,
    // assume this is a contiguous piece of memory, if contiguity is not specified.
    //
    if (RMCFG_FEATURE_PLATFORM_MODS && FLD_TEST_DRF(OS32, _ATTR, _PHYSICALITY, _DEFAULT, pVidHeapAlloc->attr))
    {
        if ((FLD_TEST_DRF(OS32, _ATTR, _PAGE_SIZE, _BIG, pVidHeapAlloc->attr) ||
             FLD_TEST_DRF(OS32, _ATTR, _PAGE_SIZE, _HUGE, pVidHeapAlloc->attr)) &&
            (stdmemGetSysmemPageSize_HAL(pGpu, staticCast(pSystemMemory, StandardMemory)) == RM_PAGE_SIZE))
        {
            bContig = NV_TRUE;
        }
    }

    //
    // Check for virtual-only parameters used on physical allocs.
    //
    if (pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_VIRTUAL_ONLY)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Virtual-only flag used with physical allocation\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto failed;
    }
    if (FLD_TEST_DRF(OS32, _ATTR2, _32BIT_POINTER, _ENABLE, pVidHeapAlloc->attr2))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Virtual-only 32-bit pointer attr used with physical allocation\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto failed;
    }
    if (pVidHeapAlloc->hVASpace != 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "VA space handle used with physical allocation\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto failed;
    }

    NV_ASSERT(!(pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_WPR1) && !(pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_WPR2));

    if (pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Expected fixed address allocation\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto failed;
    }

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, memUtilsAllocMemDesc(pGpu, pAllocRequest, pFbAllocInfo, &pMemDesc, NULL,
                                                                  ADDR_SYSMEM, bContig, &bAllocedMemDesc), failed);

    // get possibly updated surface attributes
    pVidHeapAlloc->attr = pFbAllocInfo->retAttr;
    pVidHeapAlloc->attr2 = pFbAllocInfo->retAttr2;

    // update contiguity attribute to reflect memdesc
    if (memdescGetContiguity(pAllocRequest->pMemDesc, AT_GPU))
    {
        pVidHeapAlloc->attr = FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY,
                                          _CONTIGUOUS,
                                          pVidHeapAlloc->attr);
    }
    else
    {
        pVidHeapAlloc->attr = FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY,
                                          _NONCONTIGUOUS,
                                          pVidHeapAlloc->attr);
    }

    pVidHeapAlloc->offset = pFbAllocInfo->offset;

    if (pAllocRequest->pHwResource != NULL)
    {
        pAllocRequest->pHwResource->attr       = pFbAllocInfo->retAttr;
        pAllocRequest->pHwResource->attr2      = pFbAllocInfo->retAttr2;
        pAllocRequest->pHwResource->hwResId    = pFbAllocInfo->hwResId;
        pAllocRequest->pHwResource->comprCovg  = pFbAllocInfo->comprCovg;
        pAllocRequest->pHwResource->ctagOffset = pFbAllocInfo->ctagOffset;
        pAllocRequest->pHwResource->hwResId    = pFbAllocInfo->hwResId;
    }

    return NV_OK;

failed:
    memmgrFreeHwResources(pGpu, pMemoryManager, pFbAllocInfo);

    if (bAllocedMemDesc)
    {
        memdescDestroy(pAllocRequest->pMemDesc);
        pAllocRequest->pMemDesc = NULL;
    }

    return status;
}
