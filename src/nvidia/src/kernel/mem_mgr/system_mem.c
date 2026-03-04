/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/mem_mgr.h"
#include "core/locks.h"
#include "os/os.h"
#include "rmapi/client.h"
#include "deprecated/rmapi_deprecated.h"
#include "gpu/mem_mgr/mem_utils.h"
#include "core/system.h"
#include "ctrl/ctrl0000/ctrl0000gpu.h"

#include "gpu/mem_mgr/sysmem_scrub.h"

#include "gpu/mem_sys/kern_mem_sys.h"

#include "kernel/gpu/rc/kernel_rc.h"
#include "Nvcm.h"

#include "class/cl003e.h" // NV01_MEMORY_SYSTEM

static NvU64
_sysmemGetNextSmallestPageSize
(
    OBJGPU *pGpu,
    NvU32  *pAttr,
    NvU32  *pAttr2
)
{
    NvU64 pageSize;
    NvU64 nextPageSize;

    switch (DRF_VAL(OS32, _ATTR, _PAGE_SIZE, *pAttr))
    {
        case NVOS32_ATTR_PAGE_SIZE_4KB:
            pageSize = RM_PAGE_SIZE;
            break;
        case NVOS32_ATTR_PAGE_SIZE_BIG:
            pageSize = RM_PAGE_SIZE_64K;
            break;
        case NVOS32_ATTR_PAGE_SIZE_HUGE:
            switch (DRF_VAL(OS32, _ATTR2, _PAGE_SIZE_HUGE, *pAttr2))
            {
                case NVOS32_ATTR2_PAGE_SIZE_HUGE_DEFAULT:
                case NVOS32_ATTR2_PAGE_SIZE_HUGE_2MB:
                    pageSize = RM_PAGE_SIZE_2M;
                    break;
                case NVOS32_ATTR2_PAGE_SIZE_HUGE_512MB:
                    pageSize = RM_PAGE_SIZE_512M;
                    break;
                case NVOS32_ATTR2_PAGE_SIZE_HUGE_256GB:
                    pageSize = RM_PAGE_SIZE_256G;
                    break;
                default:
                    NV_ASSERT_OR_RETURN(0, 0);
            }
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Invalid page size attribute: 0x%x\n", DRF_VAL(OS32, _ATTR, _PAGE_SIZE, *pAttr));
            return 0ULL;
    }

    // Reached smallest page size. Return 0 to stop retrying.
    if (pageSize == osGetPageSize())
    {
        return 0ULL;
    }

    switch (pageSize)
    {
        case RM_PAGE_SIZE_64K:
        case RM_PAGE_SIZE_128K:
            nextPageSize = RM_PAGE_SIZE;
            *pAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _4KB, *pAttr);
            break;
        case RM_PAGE_SIZE_2M:
            nextPageSize = RM_PAGE_SIZE_64K;
            *pAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _BIG, *pAttr);
            break;
        case RM_PAGE_SIZE_512M:
            nextPageSize = RM_PAGE_SIZE_2M;
            *pAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _HUGE, *pAttr);
            *pAttr2 = FLD_SET_DRF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _2MB, *pAttr2);
            break;
        case RM_PAGE_SIZE_256G:
            nextPageSize = RM_PAGE_SIZE_512M;
            *pAttr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _HUGE, *pAttr);
            *pAttr2 = FLD_SET_DRF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _512MB, *pAttr2);
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Invalid page size: 0x%llx\n", pageSize);
            return 0ULL;
    }

    return nextPageSize;
}

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
    HWRESOURCE_INFO hwResource = {0};
    RsResourceRef *pResourceRef = pCallContext->pResourceRef;
    NV_STATUS rmStatus = NV_OK;
    NvHandle hClient = pCallContext->pClient->hClient;
    RmClient *pRmClient = dynamicCast(pCallContext->pClient, RmClient);
    NvHandle hParent = pCallContext->pResourceRef->pParentRef->hResource;
    NvU64 sizeOut;
    NvU64 offsetOut;
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU32 flags;
    RM_ATTR_PAGE_SIZE pageSizeAttr;
    NvBool bRetry = NV_FALSE;

    NV_ASSERT_OR_RETURN(pRmClient != NULL, NV_ERR_INVALID_CLIENT);

    if (RMCFG_FEATURE_PLATFORM_GSP && !pGpu->pGpuArch->bGpuArchIsZeroFb)
    {
        NV_ASSERT_FAILED("System memory can't be allocated on GSP without 0FB");
        return NV_ERR_NOT_SUPPORTED;
    }

    // Copy-construction has already been done by the base Memory class
    if (RS_IS_COPY_CTOR(pParams))
    {
        Memory            *pMemorySrc   = dynamicCast(pParams->pSrcRef->pResource, Memory);
        MEMORY_DESCRIPTOR *pMemDesc     = pMemorySrc->pMemDesc;
        NV_STATUS          status       = NV_ERR_INVALID_ARGUMENT;

        if (memdescGetCustomHeap(pMemDesc) == MEMDESC_CUSTOM_HEAP_SCANOUT_CARVEOUT) {
            status = memmgrDuplicateFromScanoutCarveoutRegion(pGpu,
                                                              GPU_GET_MEMORY_MANAGER(pGpu),
                                                              pMemDesc);
            return status;
        }
        return NV_OK;
    }

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
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }
    }

    //
    // Enable retrying the allocation only if the page size is default.
    // Each new attempt decreases the page size to the next supported RM value
    // until the minimum OS granularity is reached.
    //
    if (FLD_TEST_DRF(OS32, _ATTR, _PAGE_SIZE, _DEFAULT, pAllocData->attr) &&
        (GPU_GET_MEMORY_MANAGER(pGpu)->bSysmemPageSizeDefaultAllowLargePages))
    {
        bRetry = NV_TRUE;
    }

    do
    {

        NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
            sysmemInitAllocRequest(pGpu, pSystemMemory, pAllocRequest));

        // Memdesc should be allocated by now.
        NV_ASSERT_OR_RETURN(pAllocRequest->pMemDesc, NV_ERR_INVALID_STATE);
        pMemDesc = pAllocRequest->pMemDesc;

        // Copy final heap size back to client struct
        sizeOut = pMemDesc->Size;
        pAllocData->limit = sizeOut - 1;

        memSetSysmemCacheAttrib(pGpu, pAllocData, pMemDesc);

        // GPU cache snooping is a property of fully coherent platforms.
        NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
            memSetGpuCacheSnoop(pGpu, pAllocData->attr, pMemDesc));

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

        if (FLD_TEST_DRF(OS32, _ATTR2, _FIXED_NUMA_NODE_ID, _YES, pAllocData->attr2))
        {

            if (memdescGetFlag(pMemDesc, MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY))
            {
                NV_PRINTF(LEVEL_ERROR, "Cannot specify NUMA node in unprotected memory.\n");
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto failed_destroy_memdesc;
            }

            if ((pGpu->cpuNumaNodeId != NV0000_CTRL_NO_NUMA_NODE) &&
                (pAllocData->numaNode != pGpu->cpuNumaNodeId))
            {
                NV_PRINTF(LEVEL_ERROR, "NUMA node mismatch. Requested node: %u CPU node: %u\n",
                        pAllocData->numaNode, pGpu->cpuNumaNodeId);
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto failed_destroy_memdesc;
            }

            memdescSetNumaNode(pMemDesc, pAllocData->numaNode);
        }

        pageSizeAttr = dmaNvos32ToPageSizeAttr(pAllocData->attr, pAllocData->attr2);
        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_INFO,
            memmgrSetMemDescPageSize_HAL(pGpu, GPU_GET_MEMORY_MANAGER(pGpu), pMemDesc,
                                        AT_GPU, pageSizeAttr),
            failed_destroy_memdesc);

    if (memdescGetFlag(pMemDesc, MEMDESC_FLAGS_ALLOC_FROM_SCANOUT_CARVEOUT))
    {
        NvU32 heapFlag  = NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN;

        rmStatus = memmgrAllocScanoutCarveoutRegionResources(GPU_GET_MEMORY_MANAGER(pGpu),
                                                             pAllocData,
                                                             pAllocRequest->hClient,
                                                             &heapFlag,
                                                             pMemDesc);
        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR, rmStatus, failed_destroy_memdesc);

        sizeOut = pMemDesc->Size;
        pAllocData->limit = sizeOut - 1;
    }

        memdescTagAlloc(rmStatus, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_132, pMemDesc);
        if (rmStatus != NV_OK)
        {
            if (bRetry)
            {
                NvU64 pageSize;
                pageSize = _sysmemGetNextSmallestPageSize(pGpu, &pAllocData->attr, &pAllocData->attr2);
                if (pageSize == 0)
                {
                    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR, rmStatus, failed_destroy_memdesc);
                }
                NV_PRINTF(LEVEL_INFO, "Sysmem alloc failed, retrying with page size 0x%llx.\n", pageSize);

                // Freeing memdescs to avoid leaks on retry.
                memdescFree(pMemDesc);
                memdescDestroy(pMemDesc);
                pAllocRequest->pMemDesc = pMemDesc = NULL;
            }
            else
            {
                NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR, rmStatus, failed_free_scanout_carveout);
            }
        }
        else
        {
            // Got a valid allocation, set retry to false.
            bRetry = NV_FALSE;
        }
    } while (bRetry);

    offsetOut = 0;


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

    NV_ASSERT_OR_ELSE(!FLD_TEST_DRF(OS32, _ATTR2, _SMMU_ON_GPU, _ENABLE, pAllocData->attr2),
        rmStatus = NV_ERR_NOT_SUPPORTED; goto failed_free_memdesc);

    if ((FLD_TEST_DRF(OS32, _ATTR, _PAGE_SIZE, _BIG, pAllocData->attr) ||
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
            if (osGetPageSize() > RM_PAGE_SIZE)
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

        NV_CHECK_OR_ELSE(LEVEL_ERROR, bLargePageNonContigAllocSupported,
            rmStatus = NV_ERR_NOT_SUPPORTED; goto failed_free_memdesc);
    }

    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_INFO,
        memConstructCommon(pMemory, pAllocRequest->classNum, flags, pMemDesc, 0,
                           NULL, pAllocData->attr, pAllocData->attr2, 0, 0,
                           pAllocData->tag, &hwResource),
        failed_free_memdesc);

    //
    // We need to force a kernel mapping of system memory-backed notifiers
    // allocated in this path.
    //
    if (pAllocData->type == NVOS32_TYPE_NOTIFIER)
    {
        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
            memCreateKernelMapping(pMemory, NV_PROTECT_READ_WRITE, NV_FALSE),
            failed_destruct_common);
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
        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_INFO,
            RmDeprecatedConvertOs32ToOs02Flags(pAllocData->attr,
                                               pAllocData->attr2,
                                               os32Flags,
                                               &os02Flags),
            failed_destruct_common);


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

        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR, rmStatus, failed_destruct_common);

        pMemory->bRpcAlloc = NV_TRUE;
    }

    pAllocData->size = sizeOut;
    pAllocData->offset = offsetOut;

    stdmemDumpOutputAllocParams(pAllocData);

    return rmStatus;

// Resource cleanup on failure
failed_destruct_common:
    memDestructCommon(pMemory);
failed_free_memdesc:
    memdescFree(pMemDesc);
failed_free_scanout_carveout:
    if (memdescGetFlag(pMemDesc, MEMDESC_FLAGS_ALLOC_FROM_SCANOUT_CARVEOUT)) {
        memmgrFreeScanoutCarveoutRegionResources(GPU_GET_MEMORY_MANAGER(pGpu),
                                                 memdescGetPte(pMemDesc, AT_CPU, 0));
    }
failed_destroy_memdesc:
    memdescDestroy(pMemDesc);

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
sysmemInitAllocRequest_SOC
(
    OBJGPU *pGpu,
    SystemMemory *pSystemMemory,
    MEMORY_ALLOCATION_REQUEST *pAllocRequest
)
{
    NV_MEMORY_ALLOCATION_PARAMS *pAllocParams = pAllocRequest->pUserParams;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    NvBool bAllocedMemDesc = NV_FALSE;
    NvBool bContig   = NV_TRUE;
    NvU32 localAttr  = pAllocParams->attr;
    NvU32 localAttr2 = pAllocParams->attr2;
    NvU64 pageSize   = 0;
    NV_STATUS status = NV_OK;

    // Check for valid size.
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pAllocParams->size != 0, NV_ERR_INVALID_ARGUMENT);

    // Ensure a valid allocation pAllocParams->type was passed in
    NV_CHECK_OR_RETURN(LEVEL_ERROR,(pAllocParams->type < NVOS32_NUM_MEM_TYPES), NV_ERR_INVALID_ARGUMENT);

    // If vidmem not requested explicitly, decide on the physical location.
    if (FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _PCI, localAttr) ||
        FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _ANY, localAttr))
    {

        localAttr = FLD_SET_DRF(OS32, _ATTR, _LOCATION,
                                _PCI, localAttr);
    }

    if (pAllocParams->flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE)
    {
        NV_PRINTF(LEVEL_ERROR,
                    "Fixed allocation on sysmem not allowed.\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto failed;
    }

    CLEAR_HAL_ATTR(localAttr)
    CLEAR_HAL_ATTR2(localAttr2)

    bContig = FLD_TEST_DRF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS, localAttr);

    // Allocate a memory descriptor if needed.
    if (pAllocRequest->pMemDesc == NULL)
    {
        NV_ASSERT_OK_OR_GOTO(status,
            memdescCreate(&pAllocRequest->pMemDesc, pGpu, pAllocParams->size, 0,
                bContig, ADDR_SYSMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_SKIP_RESOURCE_COMPUTE),
            failed);
        bAllocedMemDesc = NV_TRUE;
    }

    pMemDesc = pAllocRequest->pMemDesc;

    // Set attributes tracked by the memdesc
    memdescSetPteKind(pMemDesc, pAllocParams->format);
    memdescSetHwResId(pMemDesc, 0); // hwResId is 0.

    // update contiguity attribute to reflect memdesc
    if (memdescGetContiguity(pAllocRequest->pMemDesc, AT_GPU))
    {
        localAttr = FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY,
                                _CONTIGUOUS, localAttr);
    }
    else
    {
        localAttr = FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY,
                                _NONCONTIGUOUS, localAttr);
    }

    // Select the sysmem alloc page size.
    pageSize = memmgrDeterminePageSize(GPU_GET_MEMORY_MANAGER(pGpu),
                                       pAllocRequest->hClient,
                                       pAllocParams->size,
                                       pAllocParams->format,
                                       pAllocParams->flags,
                                       &localAttr, &localAttr2);
    if (!IsAMODEL(pGpu) && pageSize == 0)
    {
        status = NV_ERR_INVALID_STATE;
        NV_PRINTF(LEVEL_ERROR, "memmgrDeterminePageSize failed, status: 0x%x\n", status);
        goto failed;
    }

    pAllocParams->attr = localAttr;
    pAllocParams->attr2 = localAttr2;
    pAllocParams->offset = ~0ULL;

    return NV_OK;

failed:
    if (bAllocedMemDesc)
    {
        memdescDestroy(pAllocRequest->pMemDesc);
        pAllocRequest->pMemDesc = NULL;
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

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        memmgrAllocResources(pGpu, pMemoryManager, pAllocRequest, pFbAllocInfo),
        done);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
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
            (osGetPageSize() == RM_PAGE_SIZE))
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

void sysmemDestruct_IMPL(SystemMemory *pSystemMemory)
{
    Memory             *pMemory        = staticCast(pSystemMemory, Memory);
    MEMORY_DESCRIPTOR  *pMemDesc       = pMemory->pMemDesc;
    OBJGPU             *pGpu           = pMemory->pMemDesc->pGpu;
    MemoryManager      *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    // TODO: fix broken dup behavior
    if (memdescGetFlag(pMemDesc, MEMDESC_FLAGS_ALLOC_FROM_SCANOUT_CARVEOUT))
    {
        memmgrFreeScanoutCarveoutRegionResources(GPU_GET_MEMORY_MANAGER(pGpu),
                                                 memdescGetPte(pMemDesc, AT_CPU, 0));
        return;
    }

    if (pMemDesc->DupCount > 1)
        return;

    if (pMemory->pHwResource != NULL && pMemory->pHwResource->hwResId != 0)
    {
        NV_ASSERT_OR_RETURN_VOID(pMemoryManager->pSysmemScrubber != NULL);
        sysmemscrubScrubAndFree(pMemoryManager->pSysmemScrubber, pMemDesc);
    }
}
