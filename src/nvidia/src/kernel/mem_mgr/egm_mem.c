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

#include "mem_mgr/egm_mem.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/mem_utils.h"
#include "os/os.h"
#include "deprecated/rmapi_deprecated.h"
#include "vgpu/rpc.h"

#include "class/cl0042.h" // NV_MEMORY_EXTENDED_USER

NV_STATUS
egmmemConstruct_IMPL
(
    ExtendedGpuMemory            *pExtendedGpuMemory,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    Memory                      *pMemory            = staticCast(pExtendedGpuMemory, Memory);
    OBJGPU                      *pGpu               = pMemory->pGpu;
    MemoryManager               *pMemoryManager     = GPU_GET_MEMORY_MANAGER(pGpu);
    RmClient                    *pRmClient          = dynamicCast(pCallContext->pClient, RmClient);
    NvHandle                     hClient            = pCallContext->pClient->hClient;
    NvHandle                     hParent            = pCallContext->pResourceRef->pParentRef->hResource;
    NV_MEMORY_ALLOCATION_PARAMS *pAllocData         = pParams->pAllocParams;
    MEMORY_ALLOCATION_REQUEST    allocRequest       = {0};
    MEMORY_ALLOCATION_REQUEST   *pAllocRequest      = &allocRequest;
    RsResourceRef               *pResourceRef       = pCallContext->pResourceRef;
    NV_STATUS                    rmStatus           = NV_OK;
    FB_ALLOC_INFO               *pFbAllocInfo       = NULL;
    FB_ALLOC_PAGE_FORMAT        *pFbAllocPageFormat = NULL;
    RM_ATTR_PAGE_SIZE            pageSizeAttr;
    MEMORY_DESCRIPTOR *pMemDesc;
    HWRESOURCE_INFO hwResource;
    NvU64 sizeOut;
    NvU64 offsetOut;
    NvU32 flags;
    NvU32 gpuCacheAttrib;
    NvU32 Cache;


    // Copy-construction has already been done by the base Memory class
    if (RS_IS_COPY_CTOR(pParams))
        return NV_OK;

    NV_ASSERT_OR_RETURN(pRmClient != NULL, NV_ERR_INVALID_CLIENT);
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, egmmemValidateParams(pGpu, pRmClient, pAllocData));

    //
    // For non self-hosted case, we mimic vidmem as EGM and is only used for
    // ARCH/MODS verification
    //
    if (!gpuIsSelfHosted(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    stdmemDumpInputAllocParams(pAllocData, pCallContext);

    NV_PRINTF(LEVEL_ERROR, "EGM Allocation requested\n");

    pAllocRequest->classNum = NV_MEMORY_EXTENDED_USER;
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

    // Allocate and initialize FB_ALLOC_INFO
    pFbAllocInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
    NV_ASSERT_TRUE_OR_GOTO(rmStatus, pFbAllocInfo != NULL, NV_ERR_NO_MEMORY, free_params_and_return);

    pFbAllocPageFormat = portMemAllocNonPaged(sizeof(FB_ALLOC_PAGE_FORMAT));
    NV_ASSERT_TRUE_OR_GOTO(rmStatus, pFbAllocPageFormat != NULL, NV_ERR_NO_MEMORY, free_params_and_return);

    portMemSet(pFbAllocInfo, 0, sizeof(FB_ALLOC_INFO));
    portMemSet(pFbAllocPageFormat, 0, sizeof(FB_ALLOC_PAGE_FORMAT));
    pFbAllocInfo->pageFormat = pFbAllocPageFormat;

    memUtilsInitFBAllocInfo(pAllocRequest->pUserParams, pFbAllocInfo,
                            pAllocRequest->hClient, pAllocRequest->hParent);

    NV_ASSERT_OK_OR_GOTO(rmStatus,
        memmgrAllocResources(pGpu, pMemoryManager, pAllocRequest, pFbAllocInfo),
        free_params_and_return);

    NV_ASSERT_OK_OR_GOTO(rmStatus,
        egmmemAllocResources(pGpu, pMemoryManager, pAllocRequest, pFbAllocInfo),
        free_params_and_return);

    NV_ASSERT(pAllocRequest->pMemDesc);
    pMemDesc = pAllocRequest->pMemDesc;

    offsetOut = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
    sizeOut = pMemDesc->Size;
    pAllocData->limit = sizeOut - 1;

    memSetSysmemCacheAttrib(pGpu, pAllocData, &Cache, &gpuCacheAttrib);

    flags = DRF_DEF(OS02, _FLAGS, _LOCATION, _PCI) |
            DRF_DEF(OS02, _FLAGS, _MAPPING, _NO_MAP) |
            DRF_NUM(OS02, _FLAGS, _COHERENCY, DRF_VAL(OS32, _ATTR, _COHERENCY, pAllocData->attr));

    NV_ASSERT(memdescGetAddressSpace(pMemDesc) == ADDR_EGM);
    memdescSetCpuCacheAttrib(pMemDesc, Cache);

    if (pCallContext->secInfo.privLevel < RS_PRIV_LEVEL_KERNEL)
        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_KERNEL_MODE, NV_FALSE);

    memdescSetFlag(pMemDesc, MEMDESC_FLAGS_SYSMEM_OWNED_BY_CLIENT, NV_TRUE);

    memdescSetGpuCacheAttrib(pMemDesc, gpuCacheAttrib);


    pageSizeAttr = dmaNvos32ToPageSizeAttr(pAllocData->attr, pAllocData->attr2);
    NV_ASSERT_OK_OR_GOTO(rmStatus, memmgrSetMemDescPageSize_HAL(pGpu, GPU_GET_MEMORY_MANAGER(pGpu), pMemDesc,
                                            AT_GPU, pageSizeAttr),
                         mem_construct_failed);

    memdescTagAlloc(rmStatus, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_45, 
                    pMemDesc);
    NV_ASSERT_OK_OR_GOTO(rmStatus, rmStatus, mem_construct_failed);

    NV_ASSERT_OK_OR_GOTO(rmStatus, memConstructCommon(pMemory,
                                                      pAllocRequest->classNum,
                                                      flags, pMemDesc, 0,
                                                      NULL, pAllocData->attr,
                                                      pAllocData->attr2, 0, 0,
                                                      pAllocData->tag, &hwResource),
                                                      mem_construct_failed);
    if (IS_VIRTUAL(pGpu))
    {
        NvU32 os02Flags;
        NvU32 os32Flags = pAllocData->flags;

        //
        // Calculate os02flags as VGPU plugin allocates sysmem with legacy
        // RmAllocMemory API
        //
        NV_ASSERT_OK_OR_GOTO(rmStatus, RmDeprecatedConvertOs32ToOs02Flags(pAllocData->attr,
                                                                          pAllocData->attr2,
                                                                          os32Flags,
                                                                          &os02Flags),
                                                                          convert_flags_failed);

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
        pMemory->bRpcAlloc = NV_TRUE;
    }

    pAllocData->size = sizeOut;
    pAllocData->offset = offsetOut;

    stdmemDumpOutputAllocParams(pAllocData);

    goto free_params_and_return;

convert_flags_failed:
    memDestructCommon(pMemory);

mem_construct_failed:
    memdescFree(pMemDesc);
    memdescDestroy(pMemDesc);

free_params_and_return:
    portMemFree(pFbAllocPageFormat);
    portMemFree(pFbAllocInfo);

    return rmStatus;
}

NV_STATUS
egmmemValidateParams
(
    OBJGPU                      *pGpu,
    RmClient                    *pRmClient,
    NV_MEMORY_ALLOCATION_PARAMS *pAllocData
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, stdmemValidateParams(pGpu, pRmClient, pAllocData));

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                       FLD_TEST_DRF(OS32, _ATTR2, _USE_EGM, _TRUE, pAllocData->attr2),
                       NV_ERR_INVALID_ARGUMENT);

    // Make sure EGM memory is not requested if local EGM is not supported
    if (!memmgrIsLocalEgmEnabled(pMemoryManager))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Allocation requested from EGM when local EGM is not supported\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // For Self-Hosted platforms which support EGM, EGM pool is the same as sysmem pool
    // Make sure _USE_EGM attribute is set only for sysmem allocations for SHH
    if (gpuIsSelfHosted(pGpu))
    {
        if (!FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _PCI, pAllocData->attr))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "NVOS32_ATTR2_USE_EGM can be set to true only when NVOS32_ATTR_LOCATION_PCI is set for SHH\n");
            return NV_ERR_INVALID_ARGUMENT;
        }
    }
    else
    {
        // Make sure _USE_EGM attribute is set only for NVOS32_ATTR_LOCATION_VIDMEM for non-SHH platforms
        if (!FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _VIDMEM, pAllocData->attr))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "NVOS32_ATTR2_USE_EGM can be set to true only when NVOS32_ATTR_LOCATION_VIDMEM is set\n");
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    return NV_OK;
}

NV_STATUS
egmmemAllocResources
(
    OBJGPU                      *pGpu,
    MemoryManager               *pMemoryManager,
    MEMORY_ALLOCATION_REQUEST   *pAllocRequest,
    FB_ALLOC_INFO               *pFbAllocInfo
)
{
    NV_STATUS                    status          = NV_OK;
    NV_MEMORY_ALLOCATION_PARAMS *pVidHeapAlloc   = pAllocRequest->pUserParams;
    MEMORY_DESCRIPTOR           *pMemDesc        = NULL;
    NvBool                       bAllocedMemDesc = NV_FALSE;
    NvBool                       bContig         = FLD_TEST_DRF(OS32, _ATTR, _PHYSICALITY,
                                                                _CONTIGUOUS, pVidHeapAlloc->attr);

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
                                                                  ADDR_EGM, bContig, &bAllocedMemDesc), failed);

    // get possibly updated surface attributes
    pVidHeapAlloc->attr = pFbAllocInfo->retAttr;
    pVidHeapAlloc->attr2 = pFbAllocInfo->retAttr2;

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
