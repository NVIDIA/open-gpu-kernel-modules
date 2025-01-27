/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "mem_mgr_internal.h"
#include "mem_mgr/mem_list.h"
#include "deprecated/rmapi_deprecated.h"

#include "mem_mgr/video_mem.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/device/device.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/bus/kern_bus.h"
#include "vgpu/vgpu_events.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "gpu/mem_mgr/heap.h"
#include "os/os.h"
#include "rmapi/client.h"

#include "class/cl003e.h" // NV01_MEMORY_SYSTEM
#include "class/cl84a0.h" // NV01_MEMORY_LIST_XXX
#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER

NV_STATUS
memlistConstruct_IMPL
(
    MemoryList                   *pMemoryList,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_MEMORY_LIST_ALLOCATION_PARAMS *pAllocParams;
    RsResourceRef                    *pResourceRef = pCallContext->pResourceRef;
    NvHandle                          hClient = pCallContext->pClient->hClient;
    NvHandle                          hParent = pResourceRef->pParentRef->hResource;
    NvU32                             externalClassId = pResourceRef->externalClassId;
    Memory                           *pMemory = staticCast(pMemoryList, Memory);
    OBJGPU                           *pGpu = pMemory->pGpu;
    NvBool                            bUserModeArgs = (pCallContext->secInfo.paramLocation == PARAM_LOCATION_USER);
    MEMORY_DESCRIPTOR                *pMemDesc = NULL;
    NV_STATUS                         status = NV_OK;
    NvU64                             memSize;
    NvU32                             i;
    Memory                           *pMemoryInfo = NULL; // TODO: This needs a more descriptive name (MemoryInfo for what?)
    NV_ADDRESS_SPACE                  addressSpace = ADDR_UNKNOWN;
    NvU32                             Cache = 0;
    MEMORY_DESCRIPTOR                *src_pMemDesc = NULL;
    NvU32                             src_hClient;
    NvU32                             src_hParent;
    OBJGPU                           *src_pGpu;
    RmPhysAddr                       *pPteArray = NULL;
    NvBool                            bContig;
    NvU32                             src_hHwResClient;
    NvU32                             src_hHwResDevice;
    NvU32                             src_hHwResHandle;
    NvU32                             result;
    RS_PRIV_LEVEL                     privLevel = pCallContext->secInfo.privLevel;

    // Copy-construction has already been done by the base Memory class
    if (RS_IS_COPY_CTOR(pParams))
        return NV_OK;

    pAllocParams = pParams->pAllocParams;

    //
    // We use memory list support in the GSP/DCE firmware RM so we want
    // to bypass the hypervisor check here.
    //
    if (!hypervisorIsVgxHyper())
    {
        if (IS_VIRTUAL(pGpu) && privLevel >= RS_PRIV_LEVEL_KERNEL)
        {
            goto continue_alloc_object;
        }
        if (IS_GSP_CLIENT(pGpu))
        {
            goto continue_alloc_object;
        }
        return NV_ERR_NOT_SUPPORTED;
    }
continue_alloc_object:

    if (DRF_VAL(OS02, _FLAGS, _COHERENCY, pAllocParams->flagsOs02) == NVOS02_FLAGS_COHERENCY_UNCACHED)
        Cache = NV_MEMORY_UNCACHED;
    else if (DRF_VAL(OS02, _FLAGS, _COHERENCY, pAllocParams->flagsOs02) == NVOS02_FLAGS_COHERENCY_CACHED)
        Cache = NV_MEMORY_CACHED;
    else if (DRF_VAL(OS02, _FLAGS, _COHERENCY, pAllocParams->flagsOs02) == NVOS02_FLAGS_COHERENCY_WRITE_COMBINE)
        Cache = NV_MEMORY_WRITECOMBINED;
    else if (DRF_VAL(OS02, _FLAGS, _COHERENCY, pAllocParams->flagsOs02) == NVOS02_FLAGS_COHERENCY_WRITE_THROUGH)
        Cache = NV_MEMORY_CACHED;
    else if (DRF_VAL(OS02, _FLAGS, _COHERENCY, pAllocParams->flagsOs02) == NVOS02_FLAGS_COHERENCY_WRITE_PROTECT)
        Cache = NV_MEMORY_CACHED;
    else if (DRF_VAL(OS02, _FLAGS, _COHERENCY, pAllocParams->flagsOs02) == NVOS02_FLAGS_COHERENCY_WRITE_BACK)
        Cache = NV_MEMORY_CACHED;

    src_hClient = pAllocParams->hClient;
    src_hParent = pAllocParams->hParent;
    src_hHwResClient = pAllocParams->hHwResClient;
    src_hHwResDevice = pAllocParams->hHwResDevice;
    src_hHwResHandle = pAllocParams->hHwResHandle;

    if (src_hClient == NV01_NULL_OBJECT)
    {
        src_hClient = hClient;
        if (src_hParent != NV01_NULL_OBJECT)
        {
            return NV_ERR_INVALID_OBJECT_PARENT;
        }
        src_hParent = hParent;
        src_pGpu = pGpu;
    }
    else
    {
        RsClient  *pClient;

        status = serverGetClientUnderLock(&g_resServ, src_hClient, &pClient);
        if (status != NV_OK)
            return status;

        status = gpuGetByHandle(pClient, src_hParent, NULL, &src_pGpu);
        if (status != NV_OK)
            return status;

        if (src_pGpu != pGpu)
            return NV_ERR_INVALID_OBJECT_PARENT;
    }

    if (externalClassId == NV01_MEMORY_LIST_OBJECT)
    {
        if (pAllocParams->hObject == NV01_NULL_OBJECT)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }
    else
    {
        if (pAllocParams->hObject != NV01_NULL_OBJECT)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }
    if (pAllocParams->reserved_0 || (pAllocParams->pteAdjust >= RM_PAGE_SIZE))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_ASSERT_OR_RETURN(!(pAllocParams->flags & NVOS32_ALLOC_FLAGS_VIRTUAL), NV_ERR_NOT_SUPPORTED);

    switch (externalClassId)
    {
        case NV01_MEMORY_LIST_SYSTEM:
            addressSpace = ADDR_SYSMEM;
            break;
        case NV01_MEMORY_LIST_FBMEM:
            addressSpace = ADDR_FBMEM;
            if (src_hHwResHandle != 0)
            {
                RsClient *pHwResClient;

                status = serverGetClientUnderLock(&g_resServ, src_hHwResClient, &pHwResClient);
                if (status == NV_OK)
                {
                    status = memGetByHandleAndDevice(pHwResClient, src_hHwResHandle,
                                                     src_hHwResDevice, &pMemoryInfo);
                }
                if (status != NV_OK)
                {
                    src_hHwResHandle = 0;
                }
            }
            break;
        case NV01_MEMORY_LIST_OBJECT:
        {
            RsClient   *pClient;

            pMemoryInfo = NULL;

            status = serverGetClientUnderLock(&g_resServ, src_hClient, &pClient);
            if (status != NV_OK)
            {
                return NV_ERR_INVALID_CLIENT;
            }

            status = memGetByHandle(pClient, pAllocParams->hObject, &pMemoryInfo);
            if (status != NV_OK)
            {
                return status;
            }
            src_pMemDesc = pMemoryInfo->pMemDesc;

            if (src_pMemDesc == NULL)
            {
                return NV_ERR_INVALID_CLASS;
            }

            addressSpace = memdescGetAddressSpace(src_pMemDesc);
            switch (addressSpace)
            {
                case ADDR_SYSMEM:
                case ADDR_FBMEM:
                    break;
                default:
                    return NV_ERR_INVALID_OBJECT;
            }

            if (!memdescGetContiguity(src_pMemDesc, AT_GPU) &&
                ((src_pMemDesc->PageCount << RM_PAGE_SHIFT) < src_pMemDesc->Size))
            {
                return NV_ERR_BUFFER_TOO_SMALL;
            }

            /*
             * get the client device memory info reference which will be used to
             * retrieve the associated hardware resource information (pHwResource)
             * for Win VMs
             */
            if (src_hHwResHandle != 0)
            {
                RsClient *pHwResClient;

                status = serverGetClientUnderLock(&g_resServ, src_hHwResClient, &pHwResClient);
                if (status == NV_OK)
                {
                    status = memGetByHandleAndDevice(pHwResClient, src_hHwResHandle,
                                                     src_hHwResDevice, &pMemoryInfo);
                }
                if (status != NV_OK)
                {
                    src_hHwResHandle = 0;
                }
            }
            break;
        }
        default:
            break;
    }

    bContig = FLD_TEST_DRF(OS02, _FLAGS, _PHYSICALITY, _CONTIGUOUS, pAllocParams->flagsOs02);
    memSize = pAllocParams->limit + 1;

    if (addressSpace == ADDR_SYSMEM)
    {
        NvU32 attr2 = 0;

        NV_CHECK_OR_RETURN(LEVEL_ERROR, src_pMemDesc == NULL, NV_ERR_NOT_SUPPORTED);

        status = memdescCreate(&pMemDesc, pGpu, memSize, 0,
                               bContig, addressSpace, Cache,
                               MEMDESC_FLAGS_SKIP_RESOURCE_COMPUTE);
        if (status != NV_OK)
        {
            return status;
        }

        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_GUEST_ALLOCATED, NV_TRUE);

        pMemDesc->PteAdjust = pAllocParams->pteAdjust;
        memdescSetPteKind(pMemDesc, pAllocParams->format);
        memdescSetGuestId(pMemDesc, pAllocParams->guestId);

        pPteArray = memdescGetPteArray(pMemDesc, AT_GPU);

        if ((pAllocParams->pageCount > pMemDesc->PageCount) ||
            !portSafeMulU32(sizeof(NvU64), pAllocParams->pageCount, &result))
        {
            memdescDestroy(pMemDesc);
            return NV_ERR_INVALID_ARGUMENT;
        }

        // copy in the pages
        status = rmapiParamsCopyIn(NULL,
                                   pPteArray,
                                   pAllocParams->pageNumberList,
                                   result,
                                   bUserModeArgs);
        if (status != NV_OK)
        {
            memdescDestroy(pMemDesc);
            return status;
        }

        if (RMCFG_MODULE_KERNEL_BUS)
        {
            KernelBus  *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

            for (i = 0; i < pKernelBus->totalPciBars; ++i)
            {
                RmPhysAddr barOffset = pKernelBus->pciBars[i];
                NvU64      barSize   = pKernelBus->pciBarSizes[i];

                if ((pPteArray[0] >= barOffset >> RM_PAGE_SHIFT) &&
                    (pPteArray[0] <  (barOffset + barSize) >> RM_PAGE_SHIFT))
                {
                    /*
                     * For SYSMEM address space when creating descriptor for
                     * physical BAR range, mark the descriptor as CPU only. This
                     * access is generally done when NvWatch is enabled and we want
                     * to map physical BAR range in CPU addressable memory and
                     * these addresses are not used for DMA.
                     */
                    memdescSetFlag(pMemDesc, MEMDESC_FLAGS_CPU_ONLY, NV_TRUE);

                    if (i == 0)
                        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_BAR0_REFLECT, NV_TRUE);
                    else if (i == 1)
                        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_BAR1_REFLECT, NV_TRUE);

                    break;
                }
            }
        }

        // reformat the pages to addresses
        for (i = pAllocParams->pageCount; i > 0;)
        {
            i--;
            memdescSetPte(pMemDesc, AT_GPU, i,
                      pPteArray[i] << RM_PAGE_SHIFT);
        }

        // this will fake a memory allocation at
        // the OS driver interface level - and also set pMemDesc->Allocated
        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_46,
                        pMemDesc);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "*** Cannot fake guest sysmem allocation. status =0x%x\n",
                      status);

            memdescDestroy(pMemDesc);
            return status;
        }

        NvU32 os32Flags = 0;
        NvU32 attr = 0;
        NvBool bUseOs02flag = NV_FALSE;

        if (bUseOs02flag == NV_FALSE)
        {
            status = RmDeprecatedConvertOs02ToOs32Flags(pAllocParams->flagsOs02, &attr, &attr2, &os32Flags);
            if (status != NV_OK)
            {
                memdescDestroy(pMemDesc);
                return status;
            }

            status = memConstructCommon(pMemory, NV01_MEMORY_SYSTEM, os32Flags, pMemDesc,
                                    0, NULL, attr, attr2, 0, 0,
                                    NVOS32_MEM_TAG_NONE, (HWRESOURCE_INFO *)NULL);
        }
        else
        {
            if (FLD_TEST_DRF(OS02, _FLAGS, _ALLOC_USER_READ_ONLY, _YES, pAllocParams->flagsOs02))
                attr2 |= DRF_DEF(OS32, _ATTR2, _PROTECTION_USER, _READ_ONLY);

            status = memConstructCommon(pMemory, NV01_MEMORY_SYSTEM, 0, pMemDesc,
                                        0, NULL, 0, attr2, 0, 0, NVOS32_MEM_TAG_NONE, (HWRESOURCE_INFO *)NULL);
        }

        if (status != NV_OK)
        {
            memdescDestroy(pMemDesc);
        }
        //
        // We currently map all allocations that get registered with GSP
        // since there is no mechanism to specify whether the registered
        // allocation is ever accessed by GSP.
        //
        else if (RMCFG_FEATURE_PLATFORM_GSP)
        {
            // GSP only supports contiguous mappings
            if (pAllocParams->pageCount == 1)
            {
                NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                    memCreateKernelMapping(pMemory, NV_PROTECT_READ_WRITE, NV_FALSE));
            }
        }
    }
    else if (addressSpace == ADDR_FBMEM)
    {
        NvU64                              newBase;
        NvU64                              baseOffset          = 0;
        NvU64                              trueLength;
        NvU32                              hwResId             = 0;
        NvU64                              pageSize            = 0;
        RM_ATTR_PAGE_SIZE                  pageSizeAttr;
        FB_ALLOC_INFO                     *pFbAllocInfo        = NULL;
        FB_ALLOC_PAGE_FORMAT              *pFbAllocPageFormat  = NULL;
        HWRESOURCE_INFO                   *pHwResource         = NULL;
        MemoryManager                     *pMemoryManager      = GPU_GET_MEMORY_MANAGER(pGpu);
        KernelGmmu                        *pKernelGmmu         = GPU_GET_KERNEL_GMMU(pGpu);
        Heap                              *pHeap;
        NvBool                             bCallingContextPlugin;
        RsResourceRef                     *pDeviceRef;

        NV_ASSERT_OK_OR_RETURN(
            refFindAncestorOfType(pResourceRef, classId(Device), &pDeviceRef));

        pHeap = vidmemGetHeap(pGpu,
                              dynamicCast(pDeviceRef->pResource, Device),
                              NV_FALSE,
                              NV_FALSE);

        //
        // When guest RM client doesn't subscribe to MIG partition and requests for vidmem allocation
        // vidmemGetHeap() returns NULL for heap. Hence, add below assert for validation.
        //
        NV_ASSERT_OR_RETURN((pHeap != NULL), NV_ERR_INVALID_STATE);

        // Must be of valid type, in FBMEM, one page for contig.
        if ((pAllocParams->type >= NVOS32_NUM_MEM_TYPES) ||
            (bContig && (pAllocParams->pageCount > 1)) ||
            (!FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _VIDMEM, pAllocParams->attr)) ||
            (pAllocParams->flags & (NVOS32_ALLOC_FLAGS_TURBO_CIPHER_ENCRYPTED |
                                 NVOS32_ALLOC_FLAGS_ALIGNMENT_HINT |
                                 NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE |
                                 NVOS32_ALLOC_FLAGS_BANK_FORCE)))
        {
            return NV_ERR_INVALID_ARGUMENT;
        }

        pAllocParams->flags |= (NVOS32_ALLOC_FLAGS_MEMORY_HANDLE_PROVIDED |
                             NVOS32_ALLOC_FLAGS_IGNORE_BANK_PLACEMENT);

        pFbAllocInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
        if (pFbAllocInfo == NULL)
        {
            NV_ASSERT(0);
            status = NV_ERR_NO_MEMORY;
            goto done_fbmem;
        }

        pFbAllocPageFormat = portMemAllocNonPaged(sizeof(FB_ALLOC_PAGE_FORMAT));
        if (pFbAllocPageFormat == NULL) {
            NV_ASSERT(0);
            status = NV_ERR_NO_MEMORY;
            goto done_fbmem;
        }

        portMemSet(pFbAllocInfo, 0, sizeof(FB_ALLOC_INFO));
        portMemSet(pFbAllocPageFormat, 0, sizeof(FB_ALLOC_PAGE_FORMAT));
        pFbAllocInfo->pageFormat = pFbAllocPageFormat;

        if (src_pMemDesc != NULL)
        {
            trueLength = (src_pMemDesc->PteAdjust +
                          src_pMemDesc->Size);
            baseOffset = memdescGetPhysAddr(src_pMemDesc, AT_GPU, 0);
        }
        else
        {
            NvU64 base = 0;
            heapGetSize(pHeap, &trueLength);
            heapGetBase(pHeap, &base);
            trueLength = base + trueLength;
        }

        // pAllocParams->hHwResHandle can be non-zero only for Win VMs and
        // at least one of NVOS32_ATTR_COMPR or NVOS32_ATTR_ZCULL can be
        // set in pAllocParams->attr only for Linux VMs
        NV_ASSERT((pAllocParams->hHwResHandle == 0) ||
                  !(pAllocParams->attr & (DRF_SHIFTMASK(NVOS32_ATTR_COMPR) |
                                       DRF_SHIFTMASK(NVOS32_ATTR_ZCULL))));

        status = memdescCreate(&pMemDesc, pGpu, memSize, 0, bContig, addressSpace,
                               Cache, MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE_FB_BC_ONLY(pGpu, addressSpace));
        if (status != NV_OK)
        {
            goto done_fbmem;
        }
        pPteArray = memdescGetPteArray(pMemDesc, AT_GPU);
        // copy in the pages
        // copy in the pages
        status = rmapiParamsCopyIn(NULL,
                                   pPteArray,
                                   pAllocParams->pageNumberList,
                                   sizeof(NvU64) * pAllocParams->pageCount,
                                   bUserModeArgs);

        if (status != NV_OK)
        {
            goto done_fbmem;
        }

        if (bContig)
        {
            newBase = (pPteArray[0] << RM_PAGE_SHIFT) + pAllocParams->pteAdjust;

            if ((newBase + memSize) > trueLength)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Out of range contig memory at 0x%016llx of size 0x%016llx\n",
                          newBase, memSize);
                status = NV_ERR_INVALID_ARGUMENT;
                goto done_fbmem;
            }
        }

        // reformat the pages to addresses
        for (i = pAllocParams->pageCount; i > 0;)
        {
            i--;
            newBase = (pPteArray[i] << RM_PAGE_SHIFT);
            if ((newBase + RM_PAGE_SIZE) > trueLength)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Out of range page address 0x%016llx\n", newBase);
                status = NV_ERR_BUFFER_TOO_SMALL;
                goto done_fbmem;
            }
            memdescSetPte(pMemDesc, AT_GPU, i, newBase + baseOffset);
        }

        NV_ASSERT_OK_OR_GOTO(status, vgpuIsCallingContextPlugin(pMemDesc->pGpu, &bCallingContextPlugin), done_fbmem);
        if (!bCallingContextPlugin)
            memdescSetFlag(pMemDesc, MEMDESC_FLAGS_GUEST_ALLOCATED, NV_TRUE);

        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_LIST_MEMORY, NV_TRUE);

        if (pAllocParams->attr & (DRF_SHIFTMASK(NVOS32_ATTR_COMPR) |
                               DRF_SHIFTMASK(NVOS32_ATTR_ZCULL)))
        {
            //
            // Request any chip-specific resources for memory of this
            // pAllocParams->type (e.g. tiles). This call may adjust size, pPitch
            // and alignment as necessary.
            //
            pFbAllocInfo->pageFormat->type  = pAllocParams->type;
            pFbAllocInfo->hwResId           = hwResId;
            pFbAllocInfo->pad               = 0;
            pFbAllocInfo->alignPad          = 0;
            pFbAllocInfo->height            = pAllocParams->height;
            pFbAllocInfo->width             = pAllocParams->width;
            pFbAllocInfo->pitch             = pAllocParams->pitch;
            pFbAllocInfo->size              = pAllocParams->size;
            pFbAllocInfo->origSize          = pAllocParams->size;
            pFbAllocInfo->offset            = ~0;
            pFbAllocInfo->pageFormat->flags = pAllocParams->flags;
            pFbAllocInfo->internalflags     = 0;
            pFbAllocInfo->pageFormat->attr  = pAllocParams->attr;
            pFbAllocInfo->retAttr           = pAllocParams->attr;
            pFbAllocInfo->pageFormat->attr2 = pAllocParams->attr2;
            pFbAllocInfo->retAttr2          = pAllocParams->attr2;
            pFbAllocInfo->format            = pAllocParams->format;
            pFbAllocInfo->comprCovg         = pAllocParams->comprcovg;
            pFbAllocInfo->zcullCovg         = 0;
            pFbAllocInfo->ctagOffset        = pAllocParams->ctagOffset;
            pFbAllocInfo->hClient           = hClient;
            pFbAllocInfo->hDevice           = hParent;    /* device */
            pFbAllocInfo->bIsKernelAlloc    = NV_FALSE;

            // only a kernel client can request for a protected allocation
            if (pFbAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_ALLOCATE_KERNEL_PRIVILEGED)
            {
                if (privLevel < RS_PRIV_LEVEL_KERNEL)
                {
                    status = NV_ERR_INSUFFICIENT_PERMISSIONS;
                    NV_PRINTF(LEVEL_ERROR, "only kernel clients may request for a protected allocation\n");
                    goto done_fbmem;
                }
                pFbAllocInfo->bIsKernelAlloc = NV_TRUE;
            }

            if ((pAllocParams->flags & NVOS32_ALLOC_FLAGS_ALIGNMENT_HINT) ||
                (pAllocParams->flags & NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE))
            {
                pFbAllocInfo->align = pAllocParams->align;
            }
            else
            {
                pFbAllocInfo->align = RM_PAGE_SIZE;
            }

            // Fetch RM page size
            pageSize = memmgrDeterminePageSize(pMemoryManager, pFbAllocInfo->hClient, pFbAllocInfo->size,
                                               pFbAllocInfo->format, pFbAllocInfo->pageFormat->flags,
                                               &pFbAllocInfo->retAttr, &pFbAllocInfo->retAttr2);

            if (pageSize == 0)
            {
                status = NV_ERR_INVALID_STATE;
                NV_PRINTF(LEVEL_ERROR, "memmgrDeterminePageSize failed\n");
                goto done_fbmem;
            }

            // Fetch memory alignment
            status = memmgrAllocDetermineAlignment_HAL(pGpu, pMemoryManager, &pFbAllocInfo->size, &pFbAllocInfo->align,
                                                       pFbAllocInfo->alignPad, pFbAllocInfo->pageFormat->flags,
                                                       pFbAllocInfo->retAttr, pFbAllocInfo->retAttr2, 0);

            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "memmgrAllocDetermineAlignment failed\n");
                goto done_fbmem;
            }

            //
            // Call into HAL to reserve any hardware resources for
            // the specified memory type.
            // If the alignment was changed due to a HW limitation, and the
            // flag NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE is set, bad_argument
            // will be passed back from the HAL
            //
            status = memmgrAllocHwResources(pGpu, pMemoryManager, pFbAllocInfo);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "memmgrAllocHwResources failure!\n");
                goto done_fbmem;
            }

            // No need to check format if comptag allocation is not requested
            if (!(pAllocParams->flags & NVOS32_ALLOC_FLAGS_SKIP_RESOURCE_ALLOC))
            {
                NV_ASSERT(pFbAllocInfo->format == pAllocParams->format);
            }

            NV_PRINTF(LEVEL_INFO, "fbAlloc for comptag successful!\n");

            /* Create hwResource from pFbAllocInfo */
            pHwResource = portMemAllocNonPaged(sizeof(HWRESOURCE_INFO));
            if (pHwResource == NULL)
            {
                (void)memmgrFreeHwResources(pGpu, pMemoryManager, pFbAllocInfo);
                status = NV_ERR_NO_MEMORY;
                goto done_fbmem;
            }

            portMemSet(pHwResource, 0x0, sizeof(HWRESOURCE_INFO));
            pHwResource->attr       = pFbAllocInfo->retAttr;
            pHwResource->attr2      = pFbAllocInfo->retAttr2;
            pHwResource->comprCovg  = pFbAllocInfo->comprCovg;
            pHwResource->ctagOffset = pFbAllocInfo->ctagOffset;
            pHwResource->hwResId    = pFbAllocInfo->hwResId;

            NV_PRINTF(LEVEL_INFO, "memmgrAllocHwResources result\n");
            NV_PRINTF(LEVEL_INFO, "                Attr:0x%x\n",
                      pFbAllocInfo->retAttr);
            NV_PRINTF(LEVEL_INFO, "               Attr2:0x%x\n",
                      pFbAllocInfo->retAttr2);
            NV_PRINTF(LEVEL_INFO, "           comprCovg:0x%x\n",
                      pFbAllocInfo->comprCovg);
            NV_PRINTF(LEVEL_INFO, "           zcullCovg:0x%x\n",
                      pFbAllocInfo->zcullCovg);
            NV_PRINTF(LEVEL_INFO, "          ctagOffset:0x%x\n",
                      pFbAllocInfo->ctagOffset);
            NV_PRINTF(LEVEL_INFO, "             hwResId:0x%x\n",
                      pFbAllocInfo->hwResId);

            hwResId = pFbAllocInfo->hwResId;

            //
            // For Linux Guest, we allocate hardware resources on the host through
            // NV01_MEMORY_LIST_OBJECT or NV01_MEMORY_LIST_FBMEM class objects.
            // isGuestAllocated flag when set TRUE indicates that hardware resource
            // is allocated for Linux Guest and we will free it in
            // memDestructCommon.
            //
            pHwResource->isGuestAllocated = NV_TRUE;
        }
        else if (src_hHwResHandle != 0)
        {
            // obtaining hardware resources info for Win VMs
            pHwResource = pMemoryInfo->pHwResource;
            hwResId = pHwResource->hwResId;
        }


        pageSizeAttr = dmaNvos32ToPageSizeAttr(pAllocParams->attr, pAllocParams->attr2);

        switch (pageSizeAttr)
        {
            case RM_ATTR_PAGE_SIZE_4KB:
                memdescSetPageSize(pMemDesc, AT_GPU, RM_PAGE_SIZE);
                break;
            case RM_ATTR_PAGE_SIZE_BIG:
                memdescSetPageSize(pMemDesc, AT_GPU, RM_PAGE_SIZE_64K);
                break;
            case RM_ATTR_PAGE_SIZE_HUGE:
                if (!kgmmuIsHugePageSupported(pKernelGmmu))
                {
                    NV_ASSERT(0);
                    status = NV_ERR_INVALID_ARGUMENT;
                    goto done_fbmem;
                }
                memdescSetPageSize(pMemDesc, AT_GPU, RM_PAGE_SIZE_HUGE);
                break;
            case RM_ATTR_PAGE_SIZE_512MB:
                if (!kgmmuIsPageSize512mbSupported(pKernelGmmu))
                {
                    NV_ASSERT(0);
                    status = NV_ERR_INVALID_ARGUMENT;
                    goto done_fbmem;
                }
                memdescSetPageSize(pMemDesc, AT_GPU, RM_PAGE_SIZE_512M);
                break;
            case RM_ATTR_PAGE_SIZE_DEFAULT:
                NV_PRINTF(LEVEL_INFO, "page size default doesn't have any impact \n");
                break;
            case RM_ATTR_PAGE_SIZE_256GB:
            case RM_ATTR_PAGE_SIZE_INVALID:
                NV_PRINTF(LEVEL_INFO, "unexpected pageSizeAttr = 0x%x\n", pageSizeAttr);
                status = NV_ERR_INVALID_STATE;
                goto done_fbmem;
        }

        status = memConstructCommon(pMemory,
                                    NV01_MEMORY_LOCAL_USER,
                                    0,                       // flags
                                    pMemDesc,
                                    0,                       // heapOwner
                                    pHeap,
                                    pAllocParams->attr,
                                    pAllocParams->attr2,
                                    0,                       // pitch
                                    pAllocParams->type,      // type
                                    NVOS32_MEM_TAG_NONE,
                                    pHwResource);            // pHwResource
        if (status != NV_OK)
        {
            goto done_fbmem;
        }

        if ((pHwResource != NULL) && (src_hHwResHandle == 0))
        {
            portMemFree(pHwResource);
            pHwResource = NULL;
        }

        memdescSetPteKind(pMemory->pMemDesc, pAllocParams->format);
        memdescSetHwResId(pMemory->pMemDesc, hwResId);

done_fbmem:
        if (status != NV_OK)
        {
            memdescDestroy(pMemDesc);

            if (src_hHwResHandle == 0)
            {
                portMemFree(pHwResource);

                /* release hwResId resources */
                portMemSet(pFbAllocInfo, 0, sizeof(FB_ALLOC_INFO));
                portMemSet(pFbAllocPageFormat, 0, sizeof(FB_ALLOC_PAGE_FORMAT));
                pFbAllocInfo->pageFormat       = pFbAllocPageFormat;
                pFbAllocInfo->pageFormat->type = pAllocParams->type;
                pFbAllocInfo->hwResId          = hwResId;
                pFbAllocInfo->size             = memSize;
                pFbAllocInfo->hClient          = hClient;
                pFbAllocInfo->hDevice          = hParent;

                memmgrFreeHwResources(pGpu, pMemoryManager, pFbAllocInfo);
            }
        }

        portMemFree(pFbAllocPageFormat);
        portMemFree(pFbAllocInfo);
    }
    else
    {
        status = NV_ERR_INVALID_CLASS;
    }
    return status;
}

NvBool
memlistCanCopy_IMPL
(
    MemoryList *pMemoryList
)
{
    return NV_TRUE;
}
