/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "mem_mgr/video_mem.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/mem_mgr/heap.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/mem_utils.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "vgpu/rpc.h"
#include "core/locks.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "diagnostics/gpu_acct.h"
#include "gpu/device/device.h"
#include "Nvcm.h"
#include "gpu/bus/third_party_p2p.h"
#include "gpu/bus/kern_bus.h"
#include "platform/sli/sli.h"

#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER

/*!
 * _vidmemQueryAlignment
 *
 * @brief
 *     Returns the size and alignment for this allocation.
 *
 * @param[in]  pRVHCP      Pointer to RmVidHeapControlParams data
 * @param[in]  pAllocData  Pointer to VIDHEAP_ALLOC_DATA
 * @param[out] pSize       The size aligned to the HW/requested alignment
 * @param[out] pAlign      The alignment required for this allocation.

 * @returns
 *      NV_OK Operation is successful.
 */
static NV_STATUS
_vidmemQueryAlignment
(
    MEMORY_ALLOCATION_REQUEST *pAllocRequest,
    NvU64                     *pSize,
    NvU64                     *pAlign
)
{
    NV_MEMORY_ALLOCATION_PARAMS *pAllocData     = pAllocRequest->pUserParams;
    OBJGPU                      *pGpu           = pAllocRequest->pGpu;
    MemoryManager               *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS                    rmStatus       = NV_OK;
    NvU64                        size           = pAllocData->size;
    NvU64                        pageSize       = 0;
    NvU64                        align          = 0;
    NvU32                        retAttr        = pAllocData->attr;
    NvU32                        retAttr2       = pAllocData->attr2;

    NV_ASSERT_OR_RETURN((NULL != pSize) && (NULL != pAlign),
                      NV_ERR_INVALID_ARGUMENT);

    if ((pAllocData->flags & NVOS32_ALLOC_FLAGS_ALIGNMENT_HINT) ||
        (pAllocData->flags & NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE))
        align = pAllocData->alignment;
    else
        align = RM_PAGE_SIZE;

    // Fetch RM page size
    pageSize = memmgrDeterminePageSize(pMemoryManager, pAllocRequest->hClient, size, pAllocData->format,
                                       pAllocData->flags, &retAttr, &retAttr2);

    if (pageSize == 0)
    {
        rmStatus = NV_ERR_INVALID_STATE;
        NV_ASSERT_OK_FAILED("memmgrDeterminePageSize", rmStatus);
        return rmStatus;
    }

    // Fetch memory alignment
    NV_ASSERT_OK_OR_RETURN(memmgrAllocDetermineAlignment_HAL(pGpu, pMemoryManager,
                    &size, &align, 0, pAllocData->flags, retAttr, retAttr2, 0));

    *pSize = size;
    *pAlign = align + 1;

    return rmStatus;
}

/*!
 * _vidmemPmaAllocate
 *
 * @brief
 *     Allocates memory on vidmem through PMA.
 *
 * @param[in] pHeap         Pointer to Heap object
 * @param[in] pAllocRequest Pointer to the MEMORY_ALLOCATION_REQUEST.
 *
 * @returns
 *      NV_OK    Operation is successful
 *      NV_ERR_* Error code in case of errors.
 */
static NV_STATUS
_vidmemPmaAllocate
(
    Heap                      *pHeap,
    MEMORY_ALLOCATION_REQUEST *pAllocRequest
)
{
    NV_MEMORY_ALLOCATION_PARAMS *pAllocData     = pAllocRequest->pUserParams;
    OBJGPU                      *pGpu           = pAllocRequest->pGpu;
    MemoryManager               *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    PMA                         *pPma           = &pHeap->pmaObject;
    NvU64                        size           = 0;
    NvU32                        pageCount      = 0;
    NvU32                        pmaInfoSize;
    NvU64                        pageSize;
    NV_STATUS                    status;
    NvU64                        sizeAlign    = 0;
    PMA_ALLOCATION_OPTIONS       allocOptions = {0};
    NvBool                       bContig;
    NvU32                        subdevInst   = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    NvU32                        pmaConfig    = PMA_QUERY_NUMA_ENABLED;

    status = pmaQueryConfigs(pPma, &pmaConfig);
    NV_ASSERT(status == NV_OK);

    //
    // In NUMA platforms, contig memory is allocated using page order from
    // kernel and that could lead to memory wastage when the size is not
    // naturally aligned to page order. Prefer non-contig when clients
    // are okay with NON_CONTIG.
    //
    if ((status == NV_OK) && (pmaConfig & PMA_QUERY_NUMA_ENABLED))
    {
        bContig =
            !FLD_TEST_DRF(OS32, _ATTR, _PHYSICALITY,
                                _ALLOW_NONCONTIGUOUS, pAllocData->attr) &&
            !FLD_TEST_DRF(OS32, _ATTR, _PHYSICALITY,
                          _NONCONTIGUOUS, pAllocData->attr);
    }
    else
    {
        bContig = !FLD_TEST_DRF(OS32, _ATTR,
                                _PHYSICALITY, _NONCONTIGUOUS,
                                pAllocData->attr);
    }

    NV_PRINTF(LEVEL_INFO, "PMA input\n");
    NV_PRINTF(LEVEL_INFO, "          Owner: 0x%x\n", pAllocData->owner);
    NV_PRINTF(LEVEL_INFO, "        hMemory: 0x%x\n", pAllocRequest->hMemory);
    NV_PRINTF(LEVEL_INFO, "           Type: 0x%x\n", pAllocData->type);
    NV_PRINTF(LEVEL_INFO, "          Flags: 0x%x\n", pAllocData->flags);
    NV_PRINTF(LEVEL_INFO, "          Begin: 0x%08llx\n", pAllocData->rangeLo);
    NV_PRINTF(LEVEL_INFO, "            End: 0x%08llx\n", pAllocData->rangeHi);
    NV_PRINTF(LEVEL_INFO, "         Height: 0x%x\n", pAllocData->height);
    NV_PRINTF(LEVEL_INFO, "          Width: 0x%x\n", pAllocData->width);
    NV_PRINTF(LEVEL_INFO, "          Pitch: 0x%x\n", pAllocData->pitch);
    NV_PRINTF(LEVEL_INFO, "           Size: 0x%08llx\n", pAllocData->size);
    NV_PRINTF(LEVEL_INFO, "      Alignment: 0x%08llx\n",
              pAllocData->alignment);
    NV_PRINTF(LEVEL_INFO, "         Offset: 0x%08llx\n", pAllocData->offset);
    NV_PRINTF(LEVEL_INFO, "           Attr: 0x%x\n", pAllocData->attr);
    NV_PRINTF(LEVEL_INFO, "          Attr2: 0x%x\n", pAllocData->attr2);
    NV_PRINTF(LEVEL_INFO, "         Format: 0x%x\n", pAllocData->format);
    NV_PRINTF(LEVEL_INFO, "      ComprCovg: 0x%x\n", pAllocData->comprCovg);
    NV_PRINTF(LEVEL_INFO, "      ZCullCovg: 0x%x\n", pAllocData->zcullCovg);
    NV_PRINTF(LEVEL_INFO, "     CtagOffset: 0x%x\n", pAllocData->ctagOffset);
    NV_PRINTF(LEVEL_INFO, "       hVASpace: 0x%x\n", pAllocData->hVASpace);

    // Get the page size returned by RM.
    pageSize = stdmemQueryPageSize(pMemoryManager, pAllocRequest->hClient, pAllocData);
    NV_ASSERT_OR_RETURN(pageSize != 0, NV_ERR_INVALID_STATE);

    if (pageSize == RM_PAGE_SIZE)
    {
        //
        // TODO Remove this after the suballocator is in place
        // Minimum granularity of PMA is 64K.
        //
        pageSize = RM_PAGE_SIZE_64K;
    }

    // Get the alignment returned by RM without actually allocating.
    status = _vidmemQueryAlignment(pAllocRequest, &size, &sizeAlign);

    //
    // Pass the turn blacklist off flag to PMA Allocation API
    // No need for FB check since PMA only manages FB
    // Bug:2451834, gpuCheckPageRetirementSupport should not be called outside
    // RM lock.
    //
    if (FLD_TEST_DRF(OS32, _ATTR2, _BLACKLIST, _OFF, pAllocData->attr2))
    {
        allocOptions.flags |= PMA_ALLOCATE_TURN_BLACKLIST_OFF;
    }

    NV_ASSERT_OR_RETURN(NV_OK == status, status);

    // RM allocations are always pinned.
    allocOptions.flags |= PMA_ALLOCATE_PINNED;

    if (pAllocData->flags & NVOS32_ALLOC_FLAGS_PERSISTENT_VIDMEM)
    {
        allocOptions.flags |= PMA_ALLOCATE_PERSISTENT;
    }

    // Check for VPR region.
    if (pAllocData->flags & NVOS32_ALLOC_FLAGS_PROTECTED)
    {
        allocOptions.flags |= PMA_ALLOCATE_PROTECTED_REGION;
    }

    // Check memory alloc direction.
    if (pAllocData->flags & NVOS32_ALLOC_FLAGS_FORCE_REVERSE_ALLOC)
    {
        allocOptions.flags |= PMA_ALLOCATE_REVERSE_ALLOC;
    }

    // Fixed address allocations.
    if (pAllocData->flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE)
    {
        NvU64 offsetAlign = NV_MAX(sizeAlign, pageSize);

        allocOptions.flags |= PMA_ALLOCATE_SPECIFY_ADDRESS_RANGE;
        allocOptions.physBegin = NV_ALIGN_DOWN(pAllocData->offset, offsetAlign);
        allocOptions.physEnd   = NV_ALIGN_UP(allocOptions.physBegin + size, offsetAlign) - 1;
        size = allocOptions.physEnd - allocOptions.physBegin + 1;
    }

    // Range based allocations.
    if (pAllocData->flags & NVOS32_ALLOC_FLAGS_USE_BEGIN_END)
    {
        allocOptions.flags |= PMA_ALLOCATE_SPECIFY_ADDRESS_RANGE;
        allocOptions.physBegin = NV_ALIGN_UP(pAllocData->rangeLo, ((NvU64)pageSize));
        allocOptions.physEnd   = NV_ALIGN_DOWN(pAllocData->rangeHi, ((NvU64)pageSize));
        allocOptions.physEnd = (allocOptions.physEnd > 0) ?
                                          allocOptions.physEnd - 1 : 0;
        NV_ASSERT_OR_RETURN(allocOptions.physBegin <= allocOptions.physEnd,
                         NV_ERR_INVALID_ARGUMENT);
    }

    // Skip scrubber, used only by scrubber construction path
    if (pAllocData->internalflags & NVOS32_ALLOC_INTERNAL_FLAGS_SKIP_SCRUB)
    {
        allocOptions.flags |= PMA_ALLOCATE_NO_ZERO;
    }

    // Pass along client requested alignment
    allocOptions.flags |= PMA_ALLOCATE_FORCE_ALIGNMENT;
    allocOptions.alignment = NV_MAX(sizeAlign, pageSize);

    // Get the number of pages to be allocated by PMA
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (NV_DIV_AND_CEIL(size, pageSize) <= NV_U32_MAX),
        NV_ERR_NO_MEMORY);
    pageCount = (NvU32) NV_DIV_AND_CEIL(size, pageSize);

retry_alloc:
    // Evaluate the size of the PMA_ALLOC_INFO struct.
    if (bContig)
    {
        allocOptions.flags |= PMA_ALLOCATE_CONTIGUOUS;
        pmaInfoSize = sizeof(PMA_ALLOC_INFO);
    }
    else
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR,
            portSafeMulU32((pageCount - 1), (sizeof(NvU64)), &pmaInfoSize),
            NV_ERR_NO_MEMORY);
        NV_CHECK_OR_RETURN(LEVEL_ERROR,
            portSafeAddU32(pmaInfoSize, (sizeof(PMA_ALLOC_INFO)), &pmaInfoSize),
            NV_ERR_NO_MEMORY);
    }

    // Alloc the tracking structure and store the values in it.
    pAllocRequest->pPmaAllocInfo[subdevInst] = portMemAllocNonPaged(pmaInfoSize);
    NV_ASSERT_OR_RETURN(NULL != pAllocRequest->pPmaAllocInfo[subdevInst], NV_ERR_NO_MEMORY);
    portMemSet(pAllocRequest->pPmaAllocInfo[subdevInst], 0, pmaInfoSize);

    pAllocRequest->pPmaAllocInfo[subdevInst]->pageSize  = pageSize;
    pAllocRequest->pPmaAllocInfo[subdevInst]->pageCount = pageCount;
    pAllocRequest->pPmaAllocInfo[subdevInst]->allocSize = ((NvU64) pageCount) * pageSize;
    pAllocRequest->pPmaAllocInfo[subdevInst]->bContig   = bContig;
    pAllocRequest->pPmaAllocInfo[subdevInst]->refCount  = 1;
    pAllocRequest->pPmaAllocInfo[subdevInst]->flags     = allocOptions.flags;

    NV_PRINTF(LEVEL_INFO, "\nNVRM:  Size requested: 0x%llx bytes\n", size);
    NV_PRINTF(LEVEL_INFO, "       PageSize: 0x%llx bytes\n", pageSize);
    NV_PRINTF(LEVEL_INFO, "      PageCount: 0x%x\n", pageCount);
    NV_PRINTF(LEVEL_INFO, "    Actual Size: 0x%llx\n",
              pAllocRequest->pPmaAllocInfo[subdevInst]->allocSize);
    NV_PRINTF(LEVEL_INFO, "     Contiguous:  %s\n", bContig ? "YES" : "NO");

    // Get the allocation from PMA.
    status = pmaAllocatePages(pPma, pageCount, pageSize, &allocOptions,
                              pAllocRequest->pPmaAllocInfo[subdevInst]->pageArray);
    if (NV_OK != status)
    {
        portMemFree(pAllocRequest->pPmaAllocInfo[subdevInst]);
        pAllocRequest->pPmaAllocInfo[subdevInst] = NULL;

        if (bContig)
        {
            if (FLD_TEST_DRF(OS32, _ATTR, _PHYSICALITY, _ALLOW_NONCONTIGUOUS, pAllocData->attr) ||
                (FLD_TEST_DRF(OS32, _ATTR, _PHYSICALITY, _DEFAULT, pAllocData->attr) &&
                   pHeap->getProperty(pHeap, PDB_PROP_HEAP_NONCONTIG_ALLOC_BY_DEFAULT)))
            {
                bContig = NV_FALSE;
                allocOptions.flags &= ~PMA_ALLOCATE_CONTIGUOUS;
                NV_PRINTF(LEVEL_INFO,
                          "pmaAllocatePages failed -- retrying as noncontiguous\n");
                goto retry_alloc;
            }
        }

        NV_PRINTF(LEVEL_WARNING, "pmaAllocatePages failed (%x)\n", status);
    }
    else
    {
        pAllocData->attr = (bContig ?
            FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS,    pAllocData->attr) :
            FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY, _NONCONTIGUOUS, pAllocData->attr));
    }

    return status;
}

/*!
 * vidmemPmaFree
 *
 * @brief
 *     Frees the memory allocated by PMA
 *
 * @param[in] pGpu           Pointer to OBJGPU
 * @param[in] pHeap          Pointer to Heap object
 * @param[in] pPmaAllocInfo  Pointer to the PMA allocation tracking structure
 * @param[in] flag           Flags to modify PMA behavior
 *
 * @returns NONE
 */
void
vidmemPmaFree
(
    OBJGPU         *pGpu,
    Heap           *pHeap,
    PMA_ALLOC_INFO *pPmaAllocInfo,
    NvU32           flags
)
{
    PMA   *pPma  = &pHeap->pmaObject;
    NvU32 pmaFreeFlags = flags;

    NV_ASSERT_OR_RETURN_VOID(NULL != pPmaAllocInfo);

    // Decrement the refcount and free only in case of zero references.
    pPmaAllocInfo->refCount--;
    if (pPmaAllocInfo->refCount != 0)
    {
        return;
    }

    //
    // Skip the scrubber if the memory is allocated with scrubber skipped.
    // The only use case is scrubber internal allocations.
    //
    if (pPmaAllocInfo->flags & PMA_ALLOCATE_NO_ZERO)
    {
        pmaFreeFlags |= PMA_FREE_SKIP_SCRUB;
    }

    if (pPmaAllocInfo->bContig)
    {
        pmaFreePages(pPma, pPmaAllocInfo->pageArray, 1,
                     pPmaAllocInfo->allocSize, pmaFreeFlags);
    }
    else
    {
        pmaFreePages(pPma, pPmaAllocInfo->pageArray,
                     pPmaAllocInfo->pageCount,
                     pPmaAllocInfo->pageSize, pmaFreeFlags);
    }
    portMemFree(pPmaAllocInfo);
    pPmaAllocInfo = NULL;
}

Heap*
vidmemGetHeap
(
    OBJGPU  *pGpu,
    Device  *pDevice,
    NvBool   bSubheap,
    NvBool   bForceGlobalHeap
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS      status         = NV_OK;

    if (bSubheap)
    {
        Heap *pHeap = memmgrGetDeviceSuballocator(pMemoryManager, bSubheap);

        NV_CHECK_OR_RETURN(LEVEL_ERROR, pHeap != NULL && pHeap->heapType == HEAP_TYPE_PHYS_MEM_SUBALLOCATOR, NULL);
        return pHeap;
    }

    if (IS_MIG_IN_USE(pGpu) && !bForceGlobalHeap)
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        Heap *pMemoryPartitionHeap = NULL;

        status = kmigmgrGetMemoryPartitionHeapFromDevice(pGpu, pKernelMIGManager, pDevice,
                                                         &pMemoryPartitionHeap);
        if (status == NV_OK)
        {
            if (pMemoryPartitionHeap != NULL)
                return pMemoryPartitionHeap;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                "failed to get memory partition heap for hClient = 0x%x, hDevice = 0x%x\n",
                RES_GET_CLIENT_HANDLE(pDevice), RES_GET_HANDLE(pDevice));
            return NULL;
        }
    }

    return MEMORY_MANAGER_GET_HEAP(pMemoryManager);
}

static NV_STATUS
vidmemCopyConstruct
(
    VideoMemory                     *pVideoMemory,
    CALL_CONTEXT                    *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL    *pParams
)
{
    Memory    *pMemorySrc            = dynamicCast(pParams->pSrcRef->pResource, Memory);
    OBJGPU    *pGpu                  = pMemorySrc->pGpu;
    NV_STATUS  status;

    NV_ASSERT_OR_RETURN(!memdescGetCustomHeap(pMemorySrc->pMemDesc), NV_ERR_INVALID_ARGUMENT);

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
        MEMORY_DESCRIPTOR *pSrcMemDesc = memdescGetMemDescFromGpu(pMemorySrc->pMemDesc, pGpu);
        status = heapReference(pGpu, pSrcMemDesc->pHeap, pMemorySrc->HeapOwner,
                               pSrcMemDesc);
        NV_ASSERT(status == NV_OK);
    SLI_LOOP_END

    return NV_OK;
}

/*!
 * vidmemConstruct
 *
 * @brief
 *     This routine provides common allocation services used by the
 *     following heap allocation functions:
 *       NVOS32_FUNCTION_ALLOC_SIZE
 *       NVOS32_FUNCTION_ALLOC_SIZE_RANGE
 *       NVOS32_FUNCTION_ALLOC_TILED_PITCH_HEIGHT
 *
 * @param[in]  pVideoMemory      Pointer to VideoMemory object
 * @param[in]  pCallContext      Pointer to the current CALL_CONTEXT.
 * @param[in]  pParams           Pointer to the alloc params
 *
 * @return 'NV_OK'
 *     Operation completed successfully.
 * @return 'NV_ERR_NO_MEMORY'
 *     There is not enough available memory to satisfy allocation request.
 * @return 'NV_ERR_NOT_SUPPORTED'
 *     Operation not supported on broken FB.
 * @return 'NV_ERR_INSUFFICIENT_RESOURCES'
 *     Not enough available resources to satisfy allocation request.
 */
NV_STATUS
vidmemConstruct_IMPL
(
    VideoMemory                  *pVideoMemory,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    Memory                      *pMemory               = staticCast(pVideoMemory, Memory);
    NV_MEMORY_ALLOCATION_PARAMS *pAllocData            = pParams->pAllocParams;
    NvHandle                     hClient               = pCallContext->pClient->hClient;
    NvHandle                     hParent               = pCallContext->pResourceRef->pParentRef->hResource;
    MEMORY_ALLOCATION_REQUEST    allocRequest          = {0};
    MEMORY_ALLOCATION_REQUEST   *pAllocRequest         = &allocRequest;
    OBJGPU                      *pGpu                  = pMemory->pGpu;
    MemoryManager               *pMemoryManager        = GPU_GET_MEMORY_MANAGER(pGpu);
    Heap                        *pHeap;
    NvBool                       bSubheap              = NV_FALSE;
    NvBool                       bRsvdHeap             = NV_FALSE;
    MEMORY_DESCRIPTOR           *pTopLevelMemDesc      = NULL;
    MEMORY_DESCRIPTOR           *pTempMemDesc          = NULL;
    HWRESOURCE_INFO              hwResource;
    RsClient                    *pRsClient             = pCallContext->pClient;
    RsResourceRef               *pResourceRef          = pCallContext->pResourceRef;
    RsResourceRef               *pDeviceRef;
    Device                      *pDevice;
    NvU32                        gpuCacheAttrib;
    NvBool                       bIsPmaAlloc           = NV_FALSE;
    NvU64                        sizeOut;
    NvU64                        offsetOut;
    NvU64                        offsetOutTemp;
    NvBool                       bLockAcquired         = NV_FALSE;
    NvU32                        attr                  = 0;
    NvU32                        attr2                 = 0;
    FB_ALLOC_INFO               *pFbAllocInfo          = NULL;
    FB_ALLOC_PAGE_FORMAT        *pFbAllocPageFormat    = NULL;
    NV_STATUS                    rmStatus              = NV_OK;
    KernelBus                   *pKernelBus            = GPU_GET_KERNEL_BUS(pGpu);
    NvBool                       bUpdatePteKind        = NV_FALSE;

    NV_ASSERT_OK_OR_RETURN(
        refFindAncestorOfType(pResourceRef, classId(Device), &pDeviceRef));

    pDevice = dynamicCast(pDeviceRef->pResource, Device);

    if (RS_IS_COPY_CTOR(pParams))
    {
        if (!rmDeviceGpuLockIsOwner(pGpu->gpuInstance) && !rmGpuLockIsOwner())
        {
            NV_ASSERT_OK_OR_GOTO(rmStatus,
                                 rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_MEM),
                                 done);

            bLockAcquired = NV_TRUE;
        }

        rmStatus = vidmemCopyConstruct(pVideoMemory, pCallContext, pParams);
        goto done;
    }

    if (!FLD_TEST_DRF(OS32, _ATTR, _COMPR, _NONE, pAllocData->attr) &&
        kbusIsStaticBar1Enabled(pGpu, pKernelBus))
    {
        if (!FLD_TEST_DRF(OS32, _ATTR, _PAGE_SIZE, _HUGE, pAllocData->attr))
        {
            // Override the attr to use 2MB page size
            pAllocData->attr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _HUGE, pAllocData->attr);
            pAllocData->attr2 = FLD_SET_DRF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _DEFAULT, pAllocData->attr2);

            NV_PRINTF(LEVEL_INFO,
                      "Overrode the page size to 2MB on this compressed vidmem for the static bar1\n");
        }

        bUpdatePteKind = NV_TRUE;
    }

    if (FLD_TEST_DRF(OS32, _ATTR, _PHYSICALITY, _DEFAULT, pAllocData->attr))
    {
        pAllocData->attr =
            FLD_SET_DRF_NUM(OS32, _ATTR, _PHYSICALITY,
                            pDevice->defaultVidmemPhysicalityOverride,
                            pAllocData->attr);
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_WARNING, stdmemValidateParams(pGpu, hClient, pAllocData));
    NV_CHECK_OR_RETURN(LEVEL_WARNING,
                       DRF_VAL(OS32, _ATTR, _LOCATION, pAllocData->attr) == NVOS32_ATTR_LOCATION_VIDMEM &&
                           !(pAllocData->flags & NVOS32_ALLOC_FLAGS_VIRTUAL),
                       NV_ERR_INVALID_ARGUMENT);

    stdmemDumpInputAllocParams(pAllocData, pCallContext);

    if (pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL)
    {
        bRsvdHeap = FLD_TEST_DRF(OS32, _ATTR, _ALLOCATE_FROM_RESERVED_HEAP, _YES, pAllocData->attr);
    }

    bSubheap = FLD_TEST_DRF(OS32, _ATTR2, _ALLOCATE_FROM_SUBHEAP, _YES, pAllocData->attr2);
    pHeap = vidmemGetHeap(pGpu, pDevice, bSubheap, bRsvdHeap);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pHeap != NULL, NV_ERR_INVALID_STATE);

    attr  = pAllocData->attr;
    attr2 = pAllocData->attr2;

    if (gpuIsCCorApmFeatureEnabled(pGpu) &&
        !FLD_TEST_DRF(OS32, _ATTR2, _MEMORY_PROTECTION, _UNPROTECTED, pAllocData->attr2))
    {
        pAllocData->flags |= NVOS32_ALLOC_FLAGS_PROTECTED;
    }
    else if (gpuIsCCFeatureEnabled(pGpu) &&
             FLD_TEST_DRF(OS32, _ATTR2, _MEMORY_PROTECTION, _UNPROTECTED, pAllocData->attr2))
    {
        // CC-TODO: Remove this once non-CPR regions are created
        NV_PRINTF(LEVEL_ERROR, "Non-CPR region not yet created\n");
        NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_ARGUMENT);
    }
    else if (!gpuIsCCorApmFeatureEnabled(pGpu) &&
             FLD_TEST_DRF(OS32, _ATTR2, _MEMORY_PROTECTION, _PROTECTED, pAllocData->attr2))
    {
        NV_PRINTF(LEVEL_ERROR, "Protected memory not enabled but PROTECTED flag is set by client");
        return NV_ERR_INVALID_ARGUMENT;
    }

    pAllocRequest->classNum = NV01_MEMORY_LOCAL_USER;
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

    bIsPmaAlloc = memmgrIsPmaInitialized(pMemoryManager) &&
                  !bSubheap &&
                  !bRsvdHeap &&
                  !(pAllocData->flags & NVOS32_ALLOC_FLAGS_WPR1) &&
                  !(pAllocData->flags & NVOS32_ALLOC_FLAGS_WPR2) &&
                  (!(pAllocData->flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE) ||
                      heapIsPmaManaged(pGpu, pHeap, pAllocData->offset, pAllocData->offset+pAllocData->size-1));

    // Scrub-on-free is not supported by heap. Make sure clients don't get unscrubbed allocations
    NV_CHECK_OR_RETURN(LEVEL_WARNING,
        !memmgrIsScrubOnFreeEnabled(pMemoryManager) || bIsPmaAlloc || bSubheap || bRsvdHeap,
        NV_ERR_INVALID_STATE);

    // Get the allocation from PMA if enabled.
    if (bIsPmaAlloc)
    {
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
        pAllocRequest->pGpu = pGpu;
        rmStatus = _vidmemPmaAllocate(vidmemGetHeap(pGpu, pDevice, NV_FALSE, NV_FALSE), pAllocRequest);
        if (NV_OK != rmStatus)
            SLI_LOOP_GOTO(done);
        SLI_LOOP_END;
    }

    if (RMCFG_FEATURE_RM_BASIC_LOCK_MODEL)
    {
        //
        // Can't move locking up as PMA locks need to be taken first.
        // Acquire the lock *only after* PMA is done allocating.
        //
        if (!rmDeviceGpuLockIsOwner(pGpu->gpuInstance) && !rmGpuLockIsOwner())
        {
            rmStatus = rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE,
                                               RM_LOCK_MODULES_MEM);
            NV_ASSERT_OR_GOTO(NV_OK == rmStatus, done);

            bLockAcquired = NV_TRUE;
        }
        else
        {
            NV_ASSERT(0);
        }
    }

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
            goto done;
        }
    }

    // Don't allow FB allocations if FB is broken unless it is a virtual allocation or running in L2 cache only mode
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) &&
        !gpuIsCacheOnlyModeEnabled(pGpu))
    {
        NV_ASSERT_FAILED("Video memory requested despite BROKEN FB");
        rmStatus = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    if (gpuIsDebuggerActive_HAL(pGpu))
    {
        // Bug 643431 - WAR for GR WFI timeouts when debugger is active
        rmStatus = NV_ERR_BUSY_RETRY;
        goto done;
    }

    pFbAllocInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
    NV_ASSERT_TRUE_OR_GOTO(rmStatus, pFbAllocInfo != NULL, NV_ERR_NO_MEMORY, done);

    pFbAllocPageFormat = portMemAllocNonPaged(sizeof(FB_ALLOC_PAGE_FORMAT));
    NV_ASSERT_TRUE_OR_GOTO(rmStatus, pFbAllocPageFormat != NULL, NV_ERR_NO_MEMORY, done);

    // Call heapAlloc to get memory.
    if (gpumgrGetBcEnabledStatus(pGpu))
    {
        MEMORY_DESCRIPTOR *pPrev = NULL;

        // VGPU won't run in SLI. So no need to set subheap and bRsvdHeap flags in memdesc.
        NV_ASSERT(!bSubheap);
        NV_ASSERT(!bRsvdHeap);

        // Create dummy top level memdesc
        rmStatus = memdescCreate(&pTopLevelMemDesc, pGpu, RM_PAGE_SIZE, 0,
            NV_TRUE,
            ADDR_FBMEM,
            NV_MEMORY_UNCACHED,
            MEMDESC_FLAGS_DUMMY_TOPLEVEL);
        if (rmStatus != NV_OK)
            goto done;
        pPrev = pTopLevelMemDesc;
        pTopLevelMemDesc->_subDeviceAllocCount = gpumgrGetSubDeviceCountFromGpu(pGpu); // very important to have this here
        pTopLevelMemDesc->_flags |=  MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE;

        offsetOutTemp = ~((NvU64)0);
        offsetOut = 0;
        sizeOut = 0;
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
        {
            // Call heapAlloc to get memory.
            pAllocRequest->pMemDesc = NULL; // heapAlloc_IMPL needs a NULL pMemdesc in order for it to be allocated,

            portMemSet(pFbAllocInfo, 0, sizeof(FB_ALLOC_INFO));
            portMemSet(pFbAllocPageFormat, 0, sizeof(FB_ALLOC_PAGE_FORMAT));
            pFbAllocInfo->pageFormat = pFbAllocPageFormat;

            memUtilsInitFBAllocInfo(pAllocRequest->pUserParams, pFbAllocInfo, hClient, hParent);

            rmStatus = memmgrAllocResources(pGpu, pMemoryManager, pAllocRequest, pFbAllocInfo);
            if (rmStatus != NV_OK)
                SLI_LOOP_GOTO(done);

            rmStatus = vidmemAllocResources(pGpu, pMemoryManager, pAllocRequest, pFbAllocInfo,
                                            vidmemGetHeap(pGpu, pDevice, NV_FALSE, NV_FALSE));
            if (rmStatus != NV_OK)
                SLI_LOOP_GOTO(done);

            NV_ASSERT(pAllocRequest->pMemDesc);

            //
            // Spoof the flags contiguity, size and alignment of heapAlloc'ed subdev memdesc
            // to dummy top level memdesc we created
            //
            pTopLevelMemDesc->Alignment  = pAllocRequest->pMemDesc->Alignment;
            pTopLevelMemDesc->_flags     = pAllocRequest->pMemDesc->_flags | MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE | MEMDESC_FLAGS_DUMMY_TOPLEVEL;
            pTopLevelMemDesc->Size       = pAllocRequest->pMemDesc->Size;
            pTopLevelMemDesc->ActualSize = pAllocRequest->pMemDesc->ActualSize;
            pTopLevelMemDesc->_pageSize  = pAllocRequest->pMemDesc->_pageSize;
            pTopLevelMemDesc->pHeap      = pAllocRequest->pMemDesc->pHeap;

            // add pAllocData->pMemDesc for subdev to linked list
            pPrev->_pNext         = pAllocRequest->pMemDesc;
            pPrev                 = pAllocRequest->pMemDesc;

            //
            // After Bugs 1967134, 1511574, 1448340, 1761278, 1993033 are implemented, remove the code below and
            // always set offsetOut = ~0 for the broadcast case.
            // Then remove the interface to remove the physical offset.
            //
            if (offsetOutTemp == ~((NvU64)0)) // 1st
                offsetOut = offsetOutTemp = memdescGetPhysAddr(pAllocRequest->pMemDesc, AT_GPU, 0);
            else if (offsetOut != ~((NvU64)0))
            {
                offsetOutTemp = memdescGetPhysAddr(pAllocRequest->pMemDesc, AT_GPU, 0);
                if (offsetOut != offsetOutTemp)
                {
                    offsetOut = ~((NvU64)0);
                }
            }

            NV_ASSERT(!sizeOut || pAllocRequest->pMemDesc->Size == sizeOut);
            sizeOut = pAllocRequest->pMemDesc->Size;
        }
        SLI_LOOP_END;

        pTempMemDesc = memdescGetMemDescFromGpu(pTopLevelMemDesc, pGpu);
    }
    else
    {

        portMemSet(pFbAllocInfo, 0, sizeof(FB_ALLOC_INFO));
        portMemSet(pFbAllocPageFormat, 0, sizeof(FB_ALLOC_PAGE_FORMAT));
        pFbAllocInfo->pageFormat = pFbAllocPageFormat;

        memUtilsInitFBAllocInfo(pAllocRequest->pUserParams, pFbAllocInfo, hClient, hParent);

        rmStatus = memmgrAllocResources(pGpu, pMemoryManager, pAllocRequest, pFbAllocInfo);
        if (rmStatus != NV_OK)
            goto done;

        rmStatus = vidmemAllocResources(pGpu, pMemoryManager, pAllocRequest, pFbAllocInfo, pHeap);
        if (rmStatus != NV_OK)
            goto done;

        NV_ASSERT(pAllocRequest->pMemDesc);

        pTempMemDesc = pTopLevelMemDesc = pAllocRequest->pMemDesc;
        offsetOut = memdescGetPhysAddr(pTempMemDesc, AT_GPU, 0);
        sizeOut   = pTempMemDesc->Size;

        if (bSubheap)
            memdescSetFlag(pTempMemDesc, MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE, NV_TRUE);
    }

    pAllocData->limit = sizeOut - 1;

    if (bIsPmaAlloc)
    {
        // Cache the PMA_ALLOC_INFO structure.
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
        memdescGetMemDescFromGpu(pTopLevelMemDesc, pGpu)->pPmaAllocInfo = pAllocRequest->pPmaAllocInfo[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];
        SLI_LOOP_END;
    }

    //
    // Set the unprotected flag in memdesc. Some control calls will use
    // this flag to determine if this memory lies in the protected or
    // unprotected region and use that to gather statistics like total
    // protected and unprotected memory usage by different clients, etc
    //
    if (gpuIsCCorApmFeatureEnabled(pGpu) &&
        FLD_TEST_DRF(OS32, _ATTR2, _MEMORY_PROTECTION, _UNPROTECTED, pAllocData->attr2))
    {
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
        memdescSetFlag(memdescGetMemDescFromGpu(pTopLevelMemDesc, pGpu),
                       MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY, NV_TRUE);
        SLI_LOOP_END;
    }

    //
    // Video memory is always locally transparently cached.  It does not require
    // any cache managment.  Marked cached unconditionally.  Non-coherent peer
    // caching is handled with an override at mapping time.
    //
    if (DRF_VAL(OS32, _ATTR2, _GPU_CACHEABLE, pAllocData->attr2) ==
        NVOS32_ATTR2_GPU_CACHEABLE_DEFAULT)
    {
        pAllocData->attr2 = FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _YES,
                                        pAllocData->attr2);
    }
    gpuCacheAttrib = NV_MEMORY_CACHED;

    // ClientDB can set the pagesize for memdesc.
    // With GPU SMMU mapping, this needs to be set on the SMMU memdesc.
    // So SMMU allocation should happen before memConstructCommon()
    // Eventaully SMMU allocation will be part of memdescAlloc().

    //
    // There are a few cases where the heap will return an existing
    // memdesc.  Only update attributes if it is new.
    //
    // @todo attr tracking should move into heapAlloc
    //
    if (pTempMemDesc->RefCount == 1)
    {
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY);
        memdescSetGpuCacheAttrib(memdescGetMemDescFromGpu(pTopLevelMemDesc, pGpu), gpuCacheAttrib);
        SLI_LOOP_END;


        // An SMMU mapping will be added to FB allocations in the following cases:
        // 1. RM clients forcing SMMU mapping via flags
        //    GPU Arch verification with VPR is one such usecase.

        if (FLD_TEST_DRF(OS32, _ATTR2, _SMMU_ON_GPU, _ENABLE, pAllocData->attr2))
        {
            NV_ASSERT_FAILED("SMMU mapping allocation is not supported for ARMv7");
            rmStatus = NV_ERR_NOT_SUPPORTED;

            memdescFree(pTopLevelMemDesc);
            memdescDestroy(pTopLevelMemDesc);
            goto done;
        }
    }

    rmStatus = memConstructCommon(pMemory, pAllocRequest->classNum, pAllocData->flags,
                                  pTopLevelMemDesc, pAllocData->owner, pHeap, pAllocData->attr,
                                  pAllocData->attr2, 0, pAllocData->type,
                                  pAllocData->tag, &hwResource);
    if (rmStatus != NV_OK)
    {
        memdescFree(pTopLevelMemDesc);
        memdescDestroy(pTopLevelMemDesc);

        goto done;
    }
    NV_ASSERT(pMemory->pMemDesc);
    NV_ASSERT(pMemory->pHeap);

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to do the hardware update.
    //
    // XXX: This is a hack for now. No Hw resources are assumed to be used in the call.
    // The host is only requested to make an alias to the allocated heap.

    //
    // Heap alloc may allocate non-contiguous pages when it is not able to
    // find contiguous pages. Replace this field before passing to RPC.
    //
    attr = (pAllocData->attr &  DRF_SHIFTMASK(NVOS32_ATTR_PHYSICALITY)) |
           (attr             & ~DRF_SHIFTMASK(NVOS32_ATTR_PHYSICALITY));

    if (!IS_GSP_CLIENT(pGpu))
    {
        NV_RM_RPC_ALLOC_VIDMEM(pGpu,
                               hClient,
                               hParent,
                               pAllocRequest->hMemory,
                               pTopLevelMemDesc,
                               sizeOut,
                               attr,
                               attr2,
                               pAllocData->type,
                               pAllocData->flags,
                               pAllocData->height,
                               pAllocData->width,
                               pAllocData->format,
                               pAllocData->comprCovg,
                               pAllocData->zcullCovg,
                               pAllocData->alignment,
                               pAllocData->pitch,
                               pAllocData->ctagOffset,
                               rmStatus);

        if (rmStatus != NV_OK)
        {
            memDestructCommon(pMemory);
            memdescFree(pTopLevelMemDesc);
            memdescDestroy(pTopLevelMemDesc);
            pTopLevelMemDesc = NULL;
            goto done;
        }

        pMemory->bRpcAlloc = NV_TRUE;
    }

    if (RMCFG_MODULE_GPUACCT)
    {
        OBJGPU *pGpu = pMemory->pGpu;
        OBJSYS *pSys = SYS_GET_INSTANCE();
        GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(pSys);
        RmClient *pClient = dynamicCast(pRsClient, RmClient);
        NvU64 fbUsage;
        NV2080_CTRL_GPU_PID_INFO_DATA pidInfoData;
        NV2080_CTRL_SMC_SUBSCRIPTION_INFO smcInfo;

        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ACCOUNTING_ON) &&
            (pMemory->pSubDevice == NULL) &&  // Skipping for subdevice memory allocations. Was this intentional?
            (pCallContext->secInfo.privLevel < RS_PRIV_LEVEL_KERNEL))
        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            NvBool bSmcGpuPartitioningEnabled = IS_MIG_IN_USE(pGpu);
            MIG_INSTANCE_REF partitionRef = kmigmgrMakeNoMIGReference();
            NvBool bGlobalInfo = NV_TRUE;
            smcInfo.computeInstanceId = PARTITIONID_INVALID;
            smcInfo.gpuInstanceId = PARTITIONID_INVALID;
            //
            // With SMC GPU partitioning enabled, get associated partition ref and
            // only account for partitionLocal usages
            //
            if (bSmcGpuPartitioningEnabled)
            {
                NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                                    kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                                    pDevice, &partitionRef),
                                    done);
                bGlobalInfo = NV_FALSE;
            }
            portMemSet(&pidInfoData, 0, sizeof(NV2080_CTRL_GPU_PID_INFO_DATA));

            gpuFindClientInfoWithPidIterator(pGpu, pClient->ProcID,
                                             pClient->SubProcessID,
                                             classId(Memory),
                                             &pidInfoData,
                                             &smcInfo,
                                             &partitionRef,
                                             bGlobalInfo);

            // Only account for memory owned by the process.
            fbUsage = pidInfoData.vidMemUsage.memPrivate +
                pidInfoData.vidMemUsage.memSharedOwned;

            gpuacctUpdateProcPeakFbUsage(pGpuAcct, pGpu->gpuInstance,
                pClient->ProcID, pClient->SubProcessID,fbUsage);
        }
    }

    if (bUpdatePteKind)
    {
        rmStatus = kbusUpdateStaticBar1VAMapping_HAL(pGpu, pKernelBus,
                         pMemory->pMemDesc, 0,
                         memdescGetSize(pMemory->pMemDesc), NV_FALSE);

        if (rmStatus != NV_OK)
        {
            if (pMemory->bRpcAlloc)
            {
                NV_STATUS status = NV_OK;
                NV_RM_RPC_FREE(pGpu, hClient, hParent,
                               pAllocRequest->hMemory, status);
                NV_ASSERT(status == NV_OK);
            }
            memDestructCommon(pMemory);
            memdescFree(pTopLevelMemDesc);
            memdescDestroy(pTopLevelMemDesc);
            pTopLevelMemDesc = NULL;
            goto done;
        }

        memdescSetFlag(pMemory->pMemDesc,
                       MEMDESC_FLAGS_RESTORE_PTE_KIND_ON_FREE, NV_TRUE);
    }

    pAllocData->size = sizeOut;
    pAllocData->offset = offsetOut;

    stdmemDumpOutputAllocParams(pAllocData);

done:
    if (bSubheap && pTempMemDesc != NULL && rmStatus != NV_OK)
        heapRemoveRef(pHeap);

    portMemFree(pFbAllocPageFormat);
    portMemFree(pFbAllocInfo);

    if (bLockAcquired)
    {
        // UNLOCK: release GPUs lock
        rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
    }

    if (bIsPmaAlloc && NV_OK != rmStatus)
    {
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)

        if (pAllocRequest->pPmaAllocInfo[gpumgrGetSubDeviceInstanceFromGpu(pGpu)])
            vidmemPmaFree(pGpu, vidmemGetHeap(pGpu, pDevice, NV_FALSE, NV_FALSE),
                          pAllocRequest->pPmaAllocInfo[gpumgrGetSubDeviceInstanceFromGpu(pGpu)], 0);
        SLI_LOOP_END;
    }

    return rmStatus;
}

void
vidmemDestruct_IMPL
(
    VideoMemory        *pVideoMemory
)
{
    Memory             *pMemory        = staticCast(pVideoMemory, Memory);
    OBJGPU             *pGpu           = pMemory->pGpu;
    MEMORY_DESCRIPTOR  *pMemDesc       = pMemory->pMemDesc;

    // Free any association of the memory with existing third-party p2p object
    CliUnregisterMemoryFromThirdPartyP2P(pMemory);

    memDestructCommon(pMemory);

    // free the video memory based on how it was alloced ... a non-zero
    // heapOwner indicates it was heapAlloc-ed.
    if (!memdescGetCustomHeap(pMemDesc))
    {
        MemoryManager      *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
        NvHandle            hClient        = RES_GET_CLIENT_HANDLE(pVideoMemory);
        NvHandle            hParent        = RES_GET_PARENT_HANDLE(pVideoMemory);
        NvU32               heapOwner      = pMemory->HeapOwner;
        NV_STATUS           status;

        NV_PRINTF(LEVEL_INFO, "Function: FREE\n");
        NV_PRINTF(LEVEL_INFO, "   Owner: 0x%x\n", heapOwner);
        NV_PRINTF(LEVEL_INFO, " hMemory: 0x%x\n", RES_GET_HANDLE(pVideoMemory));

        //
        // memHandle (and the block's size/type) is returned, but not
        // needed ... the caller already has the correct handle to pass
        // to memDestructCommon
        //
        if (gpumgrGetBcEnabledStatus(pGpu) &&
            (memdescGetAddressSpace(memdescGetMemDescFromGpu(pMemDesc, pGpu)) == ADDR_FBMEM))
        {
            MEMORY_DESCRIPTOR *pNextMemDesc = NULL, *pSubdevMemDesc = NULL;
            pSubdevMemDesc = pMemDesc->_pNext;

            NV_ASSERT(pMemDesc->_subDeviceAllocCount > 1);
            NV_ASSERT(!IS_MIG_IN_USE(pGpu));

            SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY);
            if (pSubdevMemDesc == NULL)
            {
                NV_ASSERT(0);
                SLI_LOOP_BREAK;
            }
            // Unlink per-gpu memdesc for SLI client allocations before freeing heap
            pNextMemDesc = pSubdevMemDesc->_pNext;

            status = memmgrFree(pGpu,
                                pMemoryManager,
                                pSubdevMemDesc->pHeap,
                                hClient,
                                hParent, // device or subdevice
                                0,
                                heapOwner,
                                pSubdevMemDesc);
            NV_ASSERT(status == NV_OK);

            pSubdevMemDesc = pNextMemDesc;
            SLI_LOOP_END;

            memdescFree(pMemDesc);
            memdescDestroy(pMemDesc);
        }
        else
        {
            Heap *pHeap = pMemDesc->pHeap;

            NV_ASSERT(pMemDesc->_subDeviceAllocCount == 1);
            status = memmgrFree(pGpu,
                                pMemoryManager,
                                pHeap,
                                hClient,
                                hParent, // device or subdevice
                                0,
                                heapOwner,
                                pMemDesc);
            NV_ASSERT(status == NV_OK);

        }
    }
}

NV_STATUS
vidmemAllocResources
(
    OBJGPU                      *pGpu,
    MemoryManager               *pMemoryManager,
    MEMORY_ALLOCATION_REQUEST   *pAllocRequest,
    FB_ALLOC_INFO               *pFbAllocInfo,
    Heap                        *pHeap
)
{
    NV_STATUS                    status               = NV_OK;
    KernelMemorySystem          *pKernelMemorySystem  = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NV_MEMORY_ALLOCATION_PARAMS *pVidHeapAlloc        = pAllocRequest->pUserParams;
    NvU64                        requestedSize        = pVidHeapAlloc->size;
    HWRESOURCE_INFO             *pHwResource          = NULL;
    MEMORY_DESCRIPTOR           *pMemDesc             = NULL;
    NvBool                       bAllocedMemDesc      = NV_FALSE;
    NvBool                       bAllocedMemory       = NV_FALSE;
    NvBool                       bNoncontigAllowed    = NV_FALSE;
    NvBool                       bNoncontigAllocation = NV_FALSE;
    NvHandle                     hVASpace             = pVidHeapAlloc->hVASpace;
    NvBool                       bIsPmaOwned          = NV_FALSE;
    NvU32                        subdeviceInst        = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    NvBool                       bContig              = NV_TRUE;

    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, pKernelMemorySystem);

    // Most vidmem allocations external to RM get routed to PMA.
    bIsPmaOwned = (pAllocRequest->pPmaAllocInfo[subdeviceInst] != NULL);

    // check if pMemorySystemConfig is not yet initialized on Offload client.
    if (pMemorySystemConfig == NULL)
    {
        status = NV_ERR_INVALID_STATE;
        NV_ASSERT(0);
        goto failed;
    }

    if (pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_VIRTUAL_ONLY)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Virtual-only flag used with physical allocation\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto failed;
    }
    //
    // In NUMA systems, the memory allocation comes from kernel
    // and kernel doesn't support fixed address allocation.
    //
    if ((pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE) &&
        bIsPmaOwned &&
        osNumaOnliningEnabled(pGpu->pOsGpuInfo))
    {
        NV_PRINTF(LEVEL_WARNING,
                  "NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE for PMA cannot be "
                  "accommodated for NUMA systems\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto failed;
    }
    if (FLD_TEST_DRF(OS32, _ATTR2, _32BIT_POINTER, _ENABLE, pVidHeapAlloc->attr2))
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Virtual-only 32-bit pointer attr used with physical allocation\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto failed;
    }
    if (hVASpace != 0)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "VA space handle used with physical allocation\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto failed;
    }

    // Prior to this change, heap was silently ignoring non-contig Vidmem allocation requests.
    // With this change to allow non-contig vidmem allocation, I was getting a DVS Extended Sanity failures & regression on Windows.
    // It seems Windows is making some allocations with non-contig flag, but was expecting contig allocation.
    // So enable the non-contig path only for verif platforms.
    //
    bContig = FLD_TEST_DRF(OS32, _ATTR, _PHYSICALITY, _DEFAULT, pVidHeapAlloc->attr) ?
        !pHeap->getProperty(pHeap, PDB_PROP_HEAP_NONCONTIG_ALLOC_BY_DEFAULT) :
        !FLD_TEST_DRF(OS32, _ATTR, _PHYSICALITY, _NONCONTIGUOUS, pVidHeapAlloc->attr);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_WARNING, memUtilsAllocMemDesc(pGpu, pAllocRequest, pFbAllocInfo, &pMemDesc, pHeap,
                                                                  ADDR_FBMEM, bContig, &bAllocedMemDesc), failed);

#ifndef NV_DISABLE_NONCONTIG_ALLOC
    bNoncontigAllowed =
        (!bContig || FLD_TEST_DRF(OS32, _ATTR, _PHYSICALITY, _ALLOW_NONCONTIGUOUS, pVidHeapAlloc->attr)) &&
        pMemoryManager->bAllowNoncontiguousAllocation &&
        !FLD_TEST_DRF(OS32, _ATTR, _FORMAT, _SWIZZLED, pVidHeapAlloc->attr);
#endif

    if (bIsPmaOwned)
    {
        pFbAllocInfo->offset = pMemDesc->_pteArray[0];

        if (pMemoryManager->bEnableDynamicGranularityPageArrays == NV_TRUE)
        {
            //
            // set pagearray granularity if dynamic memdesc pagesize is enabled
            // this ensures consistency in calculation of page count
            //
            pMemDesc->pageArrayGranularity = pAllocRequest->pPmaAllocInfo[subdeviceInst]->pageSize;
        }

        if (bContig)
        {
                NV_PRINTF(LEVEL_INFO, "---> PMA Path taken contiguous\n");
                pVidHeapAlloc->attr = FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY,
                                                  _CONTIGUOUS,
                                                  pVidHeapAlloc->attr);

                memdescDescribe(pAllocRequest->pMemDesc, ADDR_FBMEM,
                                pAllocRequest->pPmaAllocInfo[subdeviceInst]->pageArray[0],
                                pFbAllocInfo->adjustedSize);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "---> PMA Path taken discontiguous\n");
            NV_ASSERT(!bContig && bNoncontigAllowed);
            pVidHeapAlloc->attr = FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY,
                                          _NONCONTIGUOUS,
                                          pVidHeapAlloc->attr);
            memdescFillPages(pAllocRequest->pMemDesc, 0,
                         pAllocRequest->pPmaAllocInfo[subdeviceInst]->pageArray,
                         pAllocRequest->pPmaAllocInfo[subdeviceInst]->pageCount,
                         pAllocRequest->pPmaAllocInfo[subdeviceInst]->pageSize);
        }
    }
    else
    {
        OBJHEAP_ALLOC_DATA allocData = { 0 };

        bNoncontigAllocation = !bContig;

        allocData.alignment = pVidHeapAlloc->alignment;
        allocData.allocSize = pFbAllocInfo->size + pFbAllocInfo->pad;

        status = heapAlloc(pGpu,
                           pFbAllocInfo->hClient,
                           pHeap,
                           pAllocRequest,
                           pAllocRequest->hMemory,
                           &allocData,
                           pFbAllocInfo,
                           &pHwResource,
                           &bNoncontigAllocation,
                           bNoncontigAllowed,
                           bAllocedMemDesc);

        // heapAlloc might create a new memdesc for compbit/discontig case
        pMemDesc = pAllocRequest->pMemDesc;

        if (status != NV_OK)
        {
            goto failed;
        }

        bAllocedMemory = NV_TRUE;

        if (pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_PERSISTENT_VIDMEM)
        {
            memdescSetFlag(pMemDesc, MEMDESC_FLAGS_PRESERVE_CONTENT_ON_SUSPEND, NV_TRUE);
        }
    }

    if (!bIsPmaOwned && (pVidHeapAlloc->type != NVOS32_TYPE_PMA))
    {
        NvU64 size, numBlocks;
        NvU32 i;

        //
        // Pre-fill cache to prevent FB read accesses if in cache only mode and not doing one time pre-fill
        // Also, only need to fill the *requested* size and not the actual allocation size
        // These might not work with noncontig allocation since they assume
        // physically contig memory
        //
        if (!bNoncontigAllocation &&
            gpuIsCacheOnlyModeEnabled(pGpu) &&
            !pMemorySystemConfig->bL2PreFill)
        {
            NV_STATUS preFillStatus;

            requestedSize = (requestedSize == 0) ? pVidHeapAlloc->size : requestedSize;
            preFillStatus = kmemsysPreFillCacheOnlyMemory_HAL(pGpu, pKernelMemorySystem,
                    pFbAllocInfo->offset, requestedSize);
            NV_ASSERT(preFillStatus == NV_OK);
        }

        if (memdescGetContiguity(pMemDesc, AT_GPU))
        {
            size = pFbAllocInfo->adjustedSize;
            numBlocks = 1; // One contiguous page
        }
        else
        {
            // Only 4k-sized noncontig pages supported currently
            size = RM_PAGE_SIZE;
            numBlocks = pMemDesc->PageCount;
        }

        for (i = 0; i < numBlocks; i++)
        {
            // Ensures memory is fully initialized
            memmgrScrubMemory_HAL(pGpu, pMemoryManager, memdescGetPte(pMemDesc, AT_GPU, i), size);
        }
    }

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

    // TODO remove once we don't have hwResource in MEM_BLOCK
    if (pHwResource != NULL)
    {
        pHwResource->attr       = pFbAllocInfo->retAttr;
        pHwResource->attr2      = pFbAllocInfo->retAttr2;
        pHwResource->hwResId    = pFbAllocInfo->hwResId;
        pHwResource->comprCovg  = pFbAllocInfo->comprCovg;
        pHwResource->ctagOffset = pFbAllocInfo->ctagOffset;
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
    if (bAllocedMemory)
    {
        memmgrFree(pGpu, pMemoryManager, pHeap,
                   pFbAllocInfo->hClient, pFbAllocInfo->hDevice, 0,
                   pVidHeapAlloc->owner,
                   pMemDesc);
    }
    else
    {
        memmgrFreeHwResources(pGpu, pMemoryManager, pFbAllocInfo);
    }

    if (bAllocedMemDesc)
    {
        memdescDestroy(pAllocRequest->pMemDesc);
        pAllocRequest->pMemDesc = NULL;
    }

    return status;
}

NV_STATUS
vidmemCheckCopyPermissions_IMPL
(
    VideoMemory        *pVideoMemory,
    OBJGPU             *pDstGpu,
    Device             *pDstDevice
)
{
    Memory           *pMemory               = staticCast(pVideoMemory, Memory);
    OBJGPU           *pSrcGpu               = pMemory->pGpu;
    RsClient         *pSrcClient            = RES_GET_CLIENT(pVideoMemory);
    RsClient         *pDstClient            = RES_GET_CLIENT(pDstDevice);
    KernelMIGManager *pSrcKernelMIGManager  = GPU_GET_KERNEL_MIG_MANAGER(pSrcGpu);
    KernelMIGManager *pDstKernelMIGManager  = GPU_GET_KERNEL_MIG_MANAGER(pDstGpu);
    NvBool            bSrcClientKernel      =
        (rmclientGetCachedPrivilege(dynamicCast(pSrcClient, RmClient)) >=
         RS_PRIV_LEVEL_KERNEL);
    NvBool            bDstClientKernel      =
        (rmclientGetCachedPrivilege(dynamicCast(pDstClient, RmClient)) >=
         RS_PRIV_LEVEL_KERNEL);

    //
    // XXX: In case of MIG memory, duping across GPU instances is not allowed
    // Bug 2815350 - Due to this bug, allow kernel clients to bypass this check
    //
    if (!bDstClientKernel && (IS_MIG_IN_USE(pSrcGpu) || IS_MIG_IN_USE(pDstGpu)))
    {
        //
        // Due to Bug 2815350 we have to take an exception for kernel clients,
        // hence we can't use a direct instanceRef check.
        // Rather than defaulting to heap based checks, keeping the
        // instanceRef check in else as that's what we should only have
        // when Bug 2815350 is fixed.
        // Both clients are kernel - Force subscription check. No exception needed.
        // Only SrcClientKernel - Enforce subscription check in dstClient
        //      DstClientKernel - Ignore any enforcement as kernel clients are
        //                        allowed to dup without any enforcement
        //

        if (bSrcClientKernel)
        {
            // Get memory partition heap from both clients and compare
            Heap *pDstClientHeap = NULL;
            NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
                                  kmigmgrGetMemoryPartitionHeapFromDevice(pDstGpu, pDstKernelMIGManager,
                                                                          pDstDevice, &pDstClientHeap));

            // Make sure memory is coming from same heaps
            if (pDstClientHeap != pMemory->pHeap)
            {
                 NV_PRINTF(LEVEL_WARNING,
                          "Duping outside of GPU instance not allowed with MIG\n");
                return NV_ERR_NOT_SUPPORTED;
            }
        }
        else
        {
            //
            // Due to Bug 2815350 we have to take an exception for kernel clients,
            // hence we can't use a direct instanceRef check
            //
            MIG_INSTANCE_REF srcInstRef;
            MIG_INSTANCE_REF dstInstRef;
            RsResourceRef *pSrcDeviceRef;
            Device *pSrcDevice;

            NV_ASSERT_OK_OR_RETURN(
                refFindAncestorOfType(RES_GET_REF(pMemory), classId(Device), &pSrcDeviceRef));

            pSrcDevice = dynamicCast(pSrcDeviceRef->pResource, Device);

            // Check instance subscription of source and destination clients
            NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
                                  kmigmgrGetInstanceRefFromDevice(pSrcGpu, pSrcKernelMIGManager,
                                                                  pSrcDevice, &srcInstRef));
            NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
                                  kmigmgrGetInstanceRefFromDevice(pDstGpu, pDstKernelMIGManager,
                                                                  pDstDevice, &dstInstRef));

            //
            // Memory duping is allowed accross compute instances. so ignore
            // compute instance differences
            //
            srcInstRef = kmigmgrMakeGIReference(srcInstRef.pKernelMIGGpuInstance);
            dstInstRef = kmigmgrMakeGIReference(dstInstRef.pKernelMIGGpuInstance);
            if (!kmigmgrAreMIGReferencesSame(&srcInstRef, &dstInstRef))
            {
                NV_PRINTF(LEVEL_WARNING,
                          "GPU instance subscription differ between Source and Destination clients\n");
                return NV_ERR_NOT_SUPPORTED;
            }
        }
    }

    return NV_OK;
}
