/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "mem_mgr/mem.h"

#include "mem_mgr/fla_mem.h"


#include "platform/chipset/chipset.h"

#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/disp/disp_objs.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "os/os.h"
#include "core/locks.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "vgpu/rpc.h"
#include "platform/sli/sli.h"
#include "deprecated/rmapi_deprecated.h"

#include "class/cl0041.h" // NV04_MEMORY
#include "class/cl003e.h" // NV01_MEMORY_SYSTEM
#include "class/cl0071.h" // NV01_MEMORY_SYSTEM_OS_DESCRIPTOR

NV_STATUS
memConstruct_IMPL
(
    Memory                       *pMemory,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    RsResourceRef *pResourceRef = pCallContext->pResourceRef;
    RsResourceRef *pParentRef = pResourceRef->pParentRef;

    //
    // Common initialization used for both normal construction & copy
    // constructor
    //

    // NULL if parent isn't a device
    pMemory->pDevice = dynamicCast(pParentRef->pResource, Device);

    // NULL if parent isn't a subdevice
    pMemory->pSubDevice = dynamicCast(pParentRef->pResource, Subdevice);

    // If parent subdevice, grandparent must be a device
    if (pMemory->pSubDevice)
    {
        RsResourceRef *pGrandParentRef = pParentRef->pParentRef;

        pMemory->pDevice = dynamicCast(pGrandParentRef->pResource, Device);

        if (pMemory->pDevice == NULL)
            return NV_ERR_INVALID_OBJECT_HANDLE;
    }

    // If child of device, we have a pGpu
    if (pMemory->pDevice)
    {
        // NOTE: pGpu and pDevice be NULL for NoDeviceMemory
        pMemory->pGpu = CliGetGpuFromContext(pResourceRef, &pMemory->bBcResource);

        NV_ASSERT_OR_RETURN(pMemory->pGpu != NULL, NV_ERR_INVALID_ARGUMENT);

        // Set thread BC state
        gpuSetThreadBcState(pMemory->pGpu, pMemory->bBcResource);
    }

    if (RS_IS_COPY_CTOR(pParams))
    {
        //
        // Copy constructor path (NvRmDupObject)
        //
        return memCopyConstruct_IMPL(pMemory, pCallContext, pParams);
    }
    else
    {
        //
        // Default constructor path (NvRmAlloc)
        //
    }

    return NV_OK;
}

NV_STATUS
memGetMapAddrSpace_IMPL
(
    Memory *pMemory,
    CALL_CONTEXT *pCallContext,
    NvU32 mapFlags,
    NV_ADDRESS_SPACE *pAddrSpace
)
{
    NV_ADDRESS_SPACE   addrSpace;
    OBJGPU            *pGpu = pMemory->pGpu;
    NvBool             bBcResource = pMemory->bBcResource;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;

    if (pGpu == NULL)
        return NV_ERR_INVALID_OBJECT;

    gpuSetThreadBcState(pGpu, bBcResource);

    pMemDesc = memdescGetMemDescFromGpu(pMemory->pMemDesc, pGpu);

    NV_ASSERT_OK_OR_RETURN(rmapiGetEffectiveAddrSpace(pGpu, pMemDesc, mapFlags, &addrSpace));

    if (addrSpace == ADDR_SYSMEM)
    {
        if (memdescGetFlag(pMemDesc, MEMDESC_FLAGS_BAR0_REFLECT))
        {
            addrSpace = ADDR_REGMEM;
        }
        else if (memdescGetFlag(pMemDesc, MEMDESC_FLAGS_BAR1_REFLECT))
        {
            addrSpace = ADDR_FBMEM;
        }
    }

    if (pAddrSpace)
        *pAddrSpace = addrSpace;

    return NV_OK;
}

void
memDestruct_IMPL
(
    Memory *pMemory
)
{
    OBJGPU             *pGpu            = pMemory->pGpu;
    NvHandle            hClient         = RES_GET_CLIENT_HANDLE(pMemory);
    NvHandle            hParent         = RES_GET_PARENT_HANDLE(pMemory);
    NvHandle            hMemory         = RES_GET_HANDLE(pMemory);
    NV_STATUS           status          = NV_OK;

    //
    // The default destructor is used when memConstructCommon() is called by
    // the subclass but not memDestructCommon().
    //
    if (pMemory->bConstructed && pMemory->pMemDesc != NULL)
    {
        // Remove the system memory reference from the client
        memDestructCommon(pMemory);
        memdescFree(pMemory->pMemDesc);
        memdescDestroy(pMemory->pMemDesc);
    }

    // if the allocation is RPC-ed, free using RPC
    if (pMemory->bRpcAlloc && (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu)))
    {
        NV_RM_RPC_FREE(pGpu, hClient, hParent, hMemory, status);
        NV_ASSERT((status == NV_OK) || (status == NV_ERR_GPU_IN_FULLCHIP_RESET));
    }
}

NV_STATUS
memCreateMemDesc_IMPL
(
    OBJGPU             *pGpu,
    MEMORY_DESCRIPTOR **ppMemDesc,
    NV_ADDRESS_SPACE    addrSpace,
    NvU64               FBOffset,
    NvU64               length,
    NvU32               attr,
    NvU32               attr2
)
{
    NV_STATUS          status = NV_OK;
    NvU32              CpuCacheAttrib, gpuCacheAttrib;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;

    *ppMemDesc = NULL;

    if (addrSpace == ADDR_SYSMEM)
        NV_ASSERT_OR_RETURN(FLD_TEST_DRF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS, attr), NV_ERR_INVALID_ARGUMENT);

    // setup the CpuCacheAttrib as well.. (if the caller doesn't specify anything it will be 0=UNCACHED)
    switch (DRF_VAL(OS32, _ATTR, _COHERENCY, attr))
    {
        case NVOS32_ATTR_COHERENCY_UNCACHED:
            CpuCacheAttrib = NV_MEMORY_UNCACHED;
            break;
        case NVOS32_ATTR_COHERENCY_WRITE_COMBINE:
            CpuCacheAttrib = NV_MEMORY_WRITECOMBINED;
            break;
        case NVOS32_ATTR_COHERENCY_CACHED:
        case NVOS32_ATTR_COHERENCY_WRITE_THROUGH:
        case NVOS32_ATTR_COHERENCY_WRITE_PROTECT:
        case NVOS32_ATTR_COHERENCY_WRITE_BACK:
            CpuCacheAttrib = NV_MEMORY_CACHED;
            break;
        default:
            NV_ASSERT(0);
            CpuCacheAttrib = NV_MEMORY_UNCACHED;
            break;
    }

    gpuCacheAttrib = FLD_TEST_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _YES, attr2) ? NV_MEMORY_CACHED : NV_MEMORY_UNCACHED;

    // Create and fill in a memory descriptor
    status = memdescCreate(&pMemDesc, pGpu, length, 0, NV_TRUE, addrSpace,
        CpuCacheAttrib,
        MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE_FB_BC_ONLY(pGpu, addrSpace));
    if (status == NV_OK)
    {
        if (memdescHasSubDeviceMemDescs(pMemDesc))
        {
            MEMORY_DESCRIPTOR *pMemDescNext = pMemDesc->_pNext;
            while (pMemDescNext)
            {
                memdescDescribe(pMemDescNext, addrSpace, FBOffset, length);
                memdescSetGpuCacheAttrib(pMemDescNext, gpuCacheAttrib);
                pMemDescNext = pMemDescNext->_pNext;
            }
        }
        else
        {
            memdescDescribe(pMemDesc, addrSpace, FBOffset, length);
            memdescSetGpuCacheAttrib(pMemDesc, gpuCacheAttrib);
        }

        *ppMemDesc = pMemDesc;
    }

    return status;
}

NV_STATUS
memCreateKernelMapping_IMPL
(
    Memory  *pMemory,
    NvU32    Protect,
    NvBool   bClear
)
{
    NV_STATUS status;

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO, memIsReady(pMemory, NV_FALSE));

    if (pMemory->KernelVAddr == NvP64_NULL)
    {
        if (memdescGetAddressSpace(pMemory->pMemDesc) != ADDR_SYSMEM)
        {
            return NV_ERR_NOT_SUPPORTED;
        }

        status = memdescMap(pMemory->pMemDesc, 0, pMemory->Length, NV_TRUE,
            Protect, &pMemory->KernelVAddr, &pMemory->KernelMapPriv);

        if (status != NV_OK)
        {
            pMemory->KernelVAddr = NvP64_NULL;
            pMemory->KernelMapPriv = NvP64_NULL;
            return status;
        }

        memdescSetKernelMapping(pMemory->pMemDesc, pMemory->KernelVAddr);
        memdescSetKernelMappingPriv(pMemory->pMemDesc, pMemory->KernelMapPriv);

        if (bClear)
        {
            portMemSet(NvP64_VALUE(pMemory->KernelVAddr), 0, pMemory->Length);
        }
    }

    return NV_OK;
}

RM_ATTR_PAGE_SIZE
dmaNvos32ToPageSizeAttr
(
    NvU32 attr,
    NvU32 attr2
)
{
    switch (DRF_VAL(OS32, _ATTR, _PAGE_SIZE, attr))
    {
        case NVOS32_ATTR_PAGE_SIZE_DEFAULT:
            return RM_ATTR_PAGE_SIZE_DEFAULT;
        case NVOS32_ATTR_PAGE_SIZE_4KB:
            return RM_ATTR_PAGE_SIZE_4KB;
        case NVOS32_ATTR_PAGE_SIZE_BIG:
            return RM_ATTR_PAGE_SIZE_BIG;
        case NVOS32_ATTR_PAGE_SIZE_HUGE:
            switch (DRF_VAL(OS32, _ATTR2, _PAGE_SIZE_HUGE, attr2))
            {
                case NVOS32_ATTR2_PAGE_SIZE_HUGE_DEFAULT:
                case NVOS32_ATTR2_PAGE_SIZE_HUGE_2MB:
                    return RM_ATTR_PAGE_SIZE_HUGE;
                case NVOS32_ATTR2_PAGE_SIZE_HUGE_512MB:
                    return RM_ATTR_PAGE_SIZE_512MB;
                case NVOS32_ATTR2_PAGE_SIZE_HUGE_256GB:
                    return RM_ATTR_PAGE_SIZE_256GB;
            }
            break;
    }

    NV_ASSERT_FAILED("Invalid attr and attr2 page size arguments");
    return RM_ATTR_PAGE_SIZE_DEFAULT;
}

NV_STATUS
memConstructCommon_IMPL
(
    Memory             *pMemory,
    NvU32               categoryClassId,
    NvU32               flags,
    MEMORY_DESCRIPTOR  *pMemDesc,
    NvU32               heapOwner,
    Heap               *pHeap,
    NvU32               attr,
    NvU32               attr2,
    NvU32               Pitch,
    NvU32               type,
    NvU32               tag,
    HWRESOURCE_INFO    *pHwResource
)
{
    OBJGPU            *pGpu           = NULL;
    NV_STATUS          status         = NV_OK;
    NvHandle           hParent        = RES_GET_PARENT_HANDLE(pMemory);
    NvHandle           hMemory        = RES_GET_HANDLE(pMemory);

    if (pMemDesc == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    // initialize the memory description
    pMemory->categoryClassId = categoryClassId;
    pMemory->pMemDesc        = pMemDesc;
    pMemory->Length          = pMemDesc->Size;
    pMemory->RefCount        = 1;
    pMemory->HeapOwner       = heapOwner;
    pMemory->pHeap           = pHeap;
    pMemory->Attr            = attr;
    pMemory->Attr2           = attr2;
    pMemory->Pitch           = Pitch;
    pMemory->Type            = type;
    pMemory->Flags           = flags;
    pMemory->tag             = tag;
    pMemory->isMemDescOwner  = NV_TRUE;
    pMemory->bRpcAlloc       = NV_FALSE;

    // We are finished if this instance is device-less
    if (pMemory->pDevice == NULL)
    {
        goto done;
    }

    if (pMemDesc->pGpu == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    // Memory has hw resources associated with it that need to be tracked.
    if (pHwResource != NULL)
    {
        pMemory->pHwResource = portMemAllocNonPaged(sizeof(HWRESOURCE_INFO));
        if (pMemory->pHwResource != NULL)
        {
            *pMemory->pHwResource = *pHwResource;       // struct copy
            pMemory->pHwResource->refCount = 1;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unable to allocate HWRESOURCE_INFO tracking structure\n");
            status = NV_ERR_NO_MEMORY;
            goto done;
        }
    }

    NV_ASSERT(status == NV_OK);

    //
    // Apply attr and flags to the memory descriptor. Ideally all should
    // be handled before we get here.
    //

    // Check whether encryption should be enabled
    if (flags & NVOS32_ALLOC_FLAGS_TURBO_CIPHER_ENCRYPTED)
    {
         pGpu = pMemDesc->pGpu;
         SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
         memdescSetFlag(memdescGetMemDescFromGpu(pMemDesc, pGpu), MEMDESC_FLAGS_ENCRYPTED, NV_TRUE);
         SLI_LOOP_END
    }

    if (FLD_TEST_DRF(OS32, _ATTR2, _PROTECTION_USER, _READ_ONLY, attr2))
    {
         pGpu = pMemDesc->pGpu;
         SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
         memdescSetFlag(memdescGetMemDescFromGpu(pMemDesc, pGpu), MEMDESC_FLAGS_USER_READ_ONLY, NV_TRUE);
         SLI_LOOP_END
    }

    if (FLD_TEST_DRF(OS32, _ATTR2, _PROTECTION_DEVICE, _READ_ONLY, attr2))
    {
         pGpu = pMemDesc->pGpu;
         SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
         memdescSetFlag(memdescGetMemDescFromGpu(pMemDesc, pGpu), MEMDESC_FLAGS_DEVICE_READ_ONLY, NV_TRUE);
         SLI_LOOP_END
    }

    // setup GpuP2PCacheAttrib
    switch (DRF_VAL(OS32, _ATTR2, _P2P_GPU_CACHEABLE, attr2))
    {
        case NVOS32_ATTR2_P2P_GPU_CACHEABLE_YES:
            pGpu = pMemDesc->pGpu;
            SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
            memdescSetGpuP2PCacheAttrib(memdescGetMemDescFromGpu(pMemDesc, pGpu), NV_MEMORY_CACHED);
            SLI_LOOP_END
            break;
        default:
            NV_ASSERT(0);
            /*FALLSTHRU*/
        case NVOS32_ATTR2_P2P_GPU_CACHEABLE_NO:
        case NVOS32_ATTR2_P2P_GPU_CACHEABLE_DEFAULT:
            pGpu = pMemDesc->pGpu;
            SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
            memdescSetGpuP2PCacheAttrib(memdescGetMemDescFromGpu(pMemDesc, pGpu), NV_MEMORY_UNCACHED);
            SLI_LOOP_END
            break;
    }

    //
    // Page size may be specified at allocation.  This if for fermi family
    // chips and is a nop for previous generations. At this point the hal call
    // to set the page size should never fail as the memory was just allocated.
    //
    if (pMemDesc->pGpu)
    {
        pGpu = pMemDesc->pGpu;
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)

        RM_ATTR_PAGE_SIZE pageSizeAttr = dmaNvos32ToPageSizeAttr(attr, attr2);
        status = memmgrSetMemDescPageSize_HAL(pGpu, GPU_GET_MEMORY_MANAGER(pGpu), memdescGetMemDescFromGpu(pMemDesc, pGpu),
                                              AT_GPU, pageSizeAttr);
        if (status != NV_OK)
        {
            SLI_LOOP_BREAK;
        }
        SLI_LOOP_END

        if (status != NV_OK)
        {
            goto done;
        }
    }

    pMemory->Node.keyStart = RES_GET_HANDLE(pMemory);
    pMemory->Node.keyEnd = RES_GET_HANDLE(pMemory);
    pMemory->Node.Data = pMemory;

    status = btreeInsert(&pMemory->Node, &pMemory->pDevice->DevMemoryTable);
    if (status != NV_OK)
        goto done;

    // Make GSP-RM aware of the memory descriptor so it can be used there
    if (FLD_TEST_DRF(OS32, _ATTR2, _REGISTER_MEMDESC_TO_PHYS_RM, _TRUE, attr2))
    {
        status = memRegisterWithGsp(pGpu, RES_GET_CLIENT(pMemory), hParent, hMemory);
        if (status != NV_OK)
            goto done;
    }

    // Initialize the circular list item for tracking dup/sharing of pMemDesc
    pMemory->dupListItem.pNext = pMemory->dupListItem.pPrev = pMemory;

done:
    if (status != NV_OK)
    {
        if (pMemory->pHwResource != NULL)
        {
            portMemFree(pMemory->pHwResource);
        }
    }
    else
    {
        pMemory->bConstructed = NV_TRUE;
    }

    return status;
}

NV_STATUS
memRegisterWithGsp_IMPL
(
    OBJGPU *pGpu,
    RsClient *pClient,
    NvHandle hParent,
    NvHandle hMemory
)
{
    NV_STATUS          status     = NV_OK;
    Memory            *pMemory    = NULL;
    RsResourceRef     *pMemoryRef = NULL;
    MEMORY_DESCRIPTOR *pMemDesc   = NULL;
    NvU32              hClass;

    // Nothing to do without GSP
    if (!IS_GSP_CLIENT(pGpu))
    {
        return NV_OK;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, clientGetResourceRef(pClient, hMemory, &pMemoryRef));

    pMemory = dynamicCast(pMemoryRef->pResource, Memory);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pMemory != NULL, NV_ERR_INVALID_OBJECT);

    pMemDesc = pMemory->pMemDesc;

    // Check: memory already registered
    if (pMemory->bRegisteredWithGsp)
    {
        return NV_OK;
    }

    // Check:  no subdevice memDescs
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                       !memdescHasSubDeviceMemDescs(pMemDesc),
                       NV_ERR_INVALID_STATE);

    // Check: SYSMEM or FBMEM only
    if (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM)
        hClass = NV01_MEMORY_LIST_FBMEM;
    else if  (memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM)
        hClass = NV01_MEMORY_LIST_SYSTEM;
    else
        return NV_ERR_INVALID_STATE;

    NvU32 os02Flags = 0;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          RmDeprecatedConvertOs32ToOs02Flags(pMemory->Attr,
                                                             pMemory->Attr2,
                                                             pMemory->Flags,
                                                             &os02Flags));
    NV_RM_RPC_ALLOC_MEMORY(pGpu,
                           pClient->hClient,
                           hParent,
                           hMemory,
                           hClass,
                           os02Flags,
                           pMemDesc,
                           status);

    if (status == NV_OK)
    {
        // Mark memory as registered in GSP
        pMemory->bRegisteredWithGsp = NV_TRUE;
    }

    return status;
}

static void
_memUnregisterFromGsp
(
    Memory *pMemory,
    RsClient *pClient,
    NvHandle hParent,
    NvHandle hMemory
)
{
    NV_STATUS status = NV_OK;

    // Nothing to do without GSP
    if ((pMemory->pGpu == NULL) ||
        !IS_GSP_CLIENT(pMemory->pGpu))
    {
        return;
    }

    // Nothing to do if memory is not registered to GSP
    if (!pMemory->bRegisteredWithGsp)
    {
        return;
    }

    NV_RM_RPC_FREE(pMemory->pGpu,
                   pClient->hClient,
                   hParent,
                   hMemory,
                   status);

    if (status == NV_OK)
    {
        // Mark memory as not registered in GSP
        pMemory->bRegisteredWithGsp = NV_FALSE;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to unregister hMemory 0x%08x from GSP, status 0x%08x\n",
                  hMemory, status);
    }
}

static NvBool
_memCheckHostVgpuDeviceExists
(
    OBJGPU *pGpu
)
{
    NV_STATUS status;

    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice = NULL;

    NV_ASSERT_OK_OR_ELSE(status, vgpuGetCallingContextKernelHostVgpuDevice(pGpu, &pKernelHostVgpuDevice), return NV_FALSE);

    return (pKernelHostVgpuDevice != NULL);
}

static void
_memDestructCommonWithDevice
(
    Memory *pMemory
)
{
    NvHandle               hMemory = RES_GET_HANDLE(pMemory);
    OBJGPU                *pGpu = pMemory->pGpu;
    Device                *pDevice = pMemory->pDevice;
    RsResourceRef         *pDeviceRef = RES_GET_REF(pDevice);
    NvHandle               hDevice = RES_GET_HANDLE(pDevice);
    Subdevice             *pSubDeviceInfo;
    DispCommon            *pDispCommon;
    RsClient              *pRsClient = RES_GET_CLIENT(pMemory);
    NV_STATUS              status;
    RS_ITERATOR            subDevIt;
    FB_ALLOC_INFO         *pFbAllocInfo       = NULL;
    FB_ALLOC_PAGE_FORMAT  *pFbAllocPageFormat = NULL;

    gpuSetThreadBcState(pGpu, pMemory->bBcResource);

    subDevIt = clientRefIter(pRsClient, pDeviceRef, classId(Subdevice), RS_ITERATE_CHILDREN, NV_TRUE);
    while (clientRefIterNext(pRsClient, &subDevIt))
    {
         pSubDeviceInfo = dynamicCast(subDevIt.pResourceRef->pResource, Subdevice);

         if (hMemory == pSubDeviceInfo->hNotifierMemory)
         {
             pSubDeviceInfo->hNotifierMemory = NV01_NULL_OBJECT;
             pSubDeviceInfo->pNotifierMemory = NULL;
         }
    }

    dispcmnGetByDevice(pRsClient, hDevice, &pDispCommon);

    //
    // Release any FB HW resources
    //
    if (pMemory->pHwResource)
    {
        if (--pMemory->pHwResource->refCount == 0)
        {
            MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
            NvBool bHostVgpuDeviceExists = _memCheckHostVgpuDeviceExists(pGpu);

            if ((pMemory->categoryClassId == NV01_MEMORY_SYSTEM && memmgrComprSupported(pMemoryManager, ADDR_SYSMEM)) ||
                (bHostVgpuDeviceExists && (pMemory->pHwResource->isGuestAllocated)))
            {
                pFbAllocInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
                if (pFbAllocInfo == NULL)
                {
                    NV_ASSERT(0);
                    status = NV_ERR_NO_MEMORY;
                    goto done;
                }

                pFbAllocPageFormat = portMemAllocNonPaged(sizeof(FB_ALLOC_PAGE_FORMAT));
                if (pFbAllocPageFormat == NULL) {
                    NV_ASSERT(0);
                    status = NV_ERR_NO_MEMORY;
                    goto done;
                }

                portMemSet(pFbAllocInfo, 0, sizeof(FB_ALLOC_INFO));
                portMemSet(pFbAllocPageFormat, 0, sizeof(FB_ALLOC_PAGE_FORMAT));
                pFbAllocInfo->pageFormat = pFbAllocPageFormat;

                pFbAllocInfo->pageFormat->type = pMemory->Type;
                pFbAllocInfo->pageFormat->attr = pMemory->Attr;
                pFbAllocInfo->pageFormat->attr2 = pMemory->Attr2;
                pFbAllocInfo->hwResId = memdescGetHwResId(pMemory->pMemDesc);
                pFbAllocInfo->size = pMemory->Length;
                pFbAllocInfo->format = memdescGetPteKind(pMemory->pMemDesc);
                pFbAllocInfo->hClient = pRsClient->hClient;
                pFbAllocInfo->hDevice = hDevice;

                //
                // Note that while freeing duped memory under a device, the
                // device may not be the memory owning device. Hence, always use
                // memory owning device (pMemDesc->pGpu) to free HW resources.
                //
                status = memmgrFreeHwResources(pMemory->pMemDesc->pGpu, pMemoryManager, pFbAllocInfo);
                NV_ASSERT(status == NV_OK);
            }
            portMemFree(pMemory->pHwResource);
        }
    }

    NV_ASSERT_OK_OR_GOTO(status, btreeUnlink(&pMemory->Node, &pDevice->DevMemoryTable), done);

    pMemory->pMemDesc->DupCount--;

    // Choose the new owner
    if (pMemory->isMemDescOwner)
    {
        (pMemory->dupListItem.pNext)->isMemDescOwner = NV_TRUE;
    }
    // Remove from circular list tracking dup/sharing of pMemDesc
    pMemory->dupListItem.pPrev->dupListItem.pNext = pMemory->dupListItem.pNext;
    pMemory->dupListItem.pNext->dupListItem.pPrev = pMemory->dupListItem.pPrev;
    pMemory->dupListItem.pNext = pMemory->dupListItem.pPrev = NULL;

    pMemory->bConstructed = NV_FALSE;

done:
    portMemFree(pFbAllocPageFormat);
    portMemFree(pFbAllocInfo);

    // The unmap call(s) above may have changed the broadcast state so restore it here
    gpuSetThreadBcState(pGpu, pMemory->bBcResource);
}

void
memDestructCommon_IMPL
(
    Memory *pMemory
)
{
    RsResourceRef      *pResourceRef    = RES_GET_REF(pMemory);
    RsResourceRef      *pParentRef      = pResourceRef->pParentRef;
    RsClient           *pClient         = RES_GET_CLIENT(pMemory);
    NvHandle            hParent         = pParentRef->hResource;
    NvHandle            hMemory         = RES_GET_HANDLE(pMemory);

    if (!pMemory->bConstructed)
        return;

    _memUnregisterFromGsp(pMemory, pClient, hParent, hMemory);

    // Do device specific teardown if we have a device
    if (pMemory->pDevice != NULL)
    {
        _memDestructCommonWithDevice(pMemory);
    }
    else
    {
        pMemory->bConstructed = NV_FALSE;
    }

    if (pMemory->KernelVAddr != NvP64_NULL)
    {
        memdescUnmap(pMemory->pMemDesc, NV_TRUE, osGetCurrentProcess(),
                     pMemory->KernelVAddr, pMemory->KernelMapPriv);
        pMemory->KernelVAddr = NvP64_NULL;
        pMemory->KernelMapPriv = NvP64_NULL;
    }
}

NV_STATUS
memGetByHandleAndDevice_IMPL
(
    RsClient   *pClient,
    NvHandle    hMemory,
    NvHandle    hDevice,
    Memory    **ppMemory
)
{
    NV_STATUS status;

    status = memGetByHandle(pClient, hMemory, ppMemory);
    if (status != NV_OK)
        return status;

    if (hDevice != RES_GET_HANDLE((*ppMemory)->pDevice))
    {
        *ppMemory = NULL;
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    return NV_OK;
}

NV_STATUS
memGetByHandle_IMPL
(
    RsClient  *pClient,
    NvHandle   hMemory,
    Memory   **ppMemory
)
{
    RsResourceRef  *pResourceRef;
    NV_STATUS       status;

    *ppMemory = NULL;

    status = clientGetResourceRef(pClient, hMemory, &pResourceRef);
    if (status != NV_OK)
        return status;

    *ppMemory = dynamicCast(pResourceRef->pResource, Memory);

    if (*ppMemory == NULL)
        return NV_ERR_INVALID_OBJECT_HANDLE;

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO, memIsReady(*ppMemory, NV_FALSE));

    return NV_OK;
}

NV_STATUS
memGetByHandleAndGroupedGpu_IMPL
(
    RsClient  *pClient,
    NvHandle   hMemory,
    OBJGPU    *pGpu,
    Memory   **ppMemory
)
{
    Memory      *pMemory;
    NV_STATUS    status;
    Device      *pDevice;

    status = memGetByHandle(pClient, hMemory, &pMemory);
    if (status != NV_OK)
        return status;

    pDevice = pMemory->pDevice;

    if ((pDevice == NULL) ||
        (gpumgrGetParentGPU(pGpu) != GPU_RES_GET_GPU(pDevice)))
    {
        *ppMemory = NULL;
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    *ppMemory = pMemory;
    return NV_OK;
}

NV_STATUS
memIsReady_IMPL
(
    Memory *pMemory,
    NvBool  bCopyConstructorContext
)
{
    if (pMemory->pMemDesc == NULL)
        return NV_ERR_INVALID_OBJECT;

    return NV_OK;
}

NV_STATUS
memControl_IMPL
(
    Memory                         *pMemory,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    RmCtrlParams *pRmCtrlParams = pParams->pLegacyParams;

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO, memIsReady(pMemory, NV_FALSE));

    if (!pMemory->pGpu)
        return NV_ERR_INVALID_OBJECT_PARENT;

    if (REF_VAL(NVXXXX_CTRL_CMD_CLASS, pParams->cmd) == NV04_MEMORY)
    {
        //
        // Tegra SOC import memory usecase uses NV01_MEMORY_SYSTEM_OS_DESCRIPTOR class for
        // RM resource server registration of memory, RM can return the physical memory attributes
        // for these imported buffers.
        //
        if ((pMemory->categoryClassId == NV01_MEMORY_SYSTEM_OS_DESCRIPTOR) &&
            (pParams->cmd != NV0041_CTRL_CMD_GET_SURFACE_PHYS_ATTR))
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    pRmCtrlParams->pGpu = pMemory->pGpu;

    gpuSetThreadBcState(pMemory->pGpu, pMemory->bBcResource);

    return resControl_IMPL(staticCast(pMemory, RsResource), pCallContext, pParams);
}

NV_STATUS
memCopyConstruct_IMPL
(
    Memory *pMemory,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    RsClient          *pDstClient = pCallContext->pClient;
    RsClient          *pSrcClient = pParams->pSrcClient;
    RsResourceRef     *pDstRef = pCallContext->pResourceRef;
    RsResourceRef     *pSrcRef = pParams->pSrcRef;
    Memory            *pMemorySrc = dynamicCast(pSrcRef->pResource, Memory);
    Memory            *pMemoryDst = pMemory;
    OBJGPU            *pSrcGpu  = NULL;
    OBJGPU            *pDstGpu  = NULL;
    NV_STATUS          status = NV_OK;
    NvBool             bReleaseGpuLock = NV_FALSE;
    Device            *pSrcDevice = NULL;
    Device            *pDstDevice = NULL;
    Subdevice         *pSrcSubDevice = NULL;
    Subdevice         *pDstSubDevice = NULL;
    RsResourceRef     *pSrcParentRef = pSrcRef->pParentRef;
    RsResourceRef     *pDstParentRef = pDstRef->pParentRef;

    NV_ASSERT_OR_RETURN(pSrcParentRef != NULL, NV_ERR_INVALID_OBJECT_PARENT);
    NV_ASSERT_OR_RETURN(pDstParentRef != NULL, NV_ERR_INVALID_OBJECT_PARENT);
    NV_ASSERT_OR_RETURN(pMemorySrc != NULL, NV_ERR_INVALID_OBJECT_HANDLE);

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO, memIsReady(pMemorySrc, NV_TRUE));

    //
    // Must return early when parent is Client.
    // This copy constructor is very device-specific so it is up
    // to the device-less Memory subclasses to define their own dup behavior.
    //
    if (RES_GET_CLIENT_HANDLE(pMemoryDst) == RES_GET_PARENT_HANDLE(pMemoryDst))
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR,
                RES_GET_CLIENT_HANDLE(pMemorySrc) == RES_GET_PARENT_HANDLE(pMemorySrc),
                NV_ERR_INVALID_OBJECT_PARENT);
        return NV_OK;
    }

    pSrcGpu  = pMemorySrc->pGpu;
    pDstGpu  = pMemoryDst->pGpu;
    pSrcDevice = pMemorySrc->pDevice;
    pDstDevice = pMemoryDst->pDevice;
    pSrcSubDevice = pMemorySrc->pSubDevice;
    pDstSubDevice = pMemoryDst->pSubDevice;

    // Only children of device are supported
    NV_ASSERT_OR_RETURN(pSrcDevice != NULL, NV_ERR_INVALID_OBJECT_PARENT);
    NV_ASSERT_OR_RETURN(pDstDevice != NULL, NV_ERR_INVALID_OBJECT_PARENT);

    if (!!pSrcSubDevice != !!pDstSubDevice)
    {
        NV_PRINTF(LEVEL_INFO, "Parent type mismatch between Src and Dst objects"
                               "Both should be either device or subDevice\n");
        return NV_ERR_INVALID_OBJECT_PARENT;
    }

    // RS-TODO: This should use pMemorySrc->bBcResource when adding full support for subdevice duping
    gpuSetThreadBcState(pSrcGpu, NV_TRUE);

    if (!rmGpuLockIsOwner() &&
        !(rmDeviceGpuLockIsOwner(pSrcGpu->gpuInstance) &&
          rmDeviceGpuLockIsOwner(pDstGpu->gpuInstance)))
    {
        // LOCK: acquire GPUs lock
        if ((status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_MEM)) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to acquire GPU locks, error 0x%x\n", status);
            return status;
        }

        bReleaseGpuLock = NV_TRUE;
    }

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                        memCheckCopyPermissions(pMemorySrc, pDstGpu, pDstDevice), done);

    // Initialize Memory
    pMemoryDst->categoryClassId = pMemorySrc->categoryClassId;
    pMemoryDst->Length          = pMemorySrc->Length;
    pMemoryDst->HeapOwner       = pMemorySrc->HeapOwner;
    pMemoryDst->pHeap           = pMemorySrc->pHeap;
    pMemoryDst->pMemDesc        = pMemorySrc->pMemDesc;
    pMemoryDst->KernelVAddr     = NvP64_NULL;
    pMemoryDst->KernelMapPriv   = NvP64_NULL;
    pMemoryDst->Attr            = pMemorySrc->Attr;
    pMemoryDst->Attr2           = pMemorySrc->Attr2;
    pMemoryDst->Pitch           = pMemorySrc->Pitch;
    pMemoryDst->Type            = pMemorySrc->Type;
    pMemoryDst->Flags           = pMemorySrc->Flags;
    pMemoryDst->tag             = pMemorySrc->tag;
    pMemoryDst->pHwResource     = pMemorySrc->pHwResource;
    pMemoryDst->isMemDescOwner  = NV_FALSE;
    pMemoryDst->bRpcAlloc       = pMemorySrc->bRpcAlloc;

    // Link in the new device memory mapping
    pMemoryDst->Node.keyStart   = RES_GET_HANDLE(pMemoryDst);
    pMemoryDst->Node.keyEnd     = RES_GET_HANDLE(pMemoryDst);
    pMemoryDst->Node.Data       = pMemoryDst;

    status = btreeInsert(&pMemoryDst->Node, &pDstDevice->DevMemoryTable);
    if (status != NV_OK)
        goto done;

    {
        OBJGPU *pGpu = pDstGpu; // Need pGpu for SLI loop

        gpuSetThreadBcState(pDstGpu, NV_TRUE);
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
        if (memdescGetPageSize(memdescGetMemDescFromGpu(pMemoryDst->pMemDesc, pGpu), AT_GPU) == 0)
        {
            status = memmgrSetMemDescPageSize_HAL(pGpu, GPU_GET_MEMORY_MANAGER(pGpu),
                                                  memdescGetMemDescFromGpu(pMemoryDst->pMemDesc, pGpu),
                                                  AT_GPU, RM_ATTR_PAGE_SIZE_DEFAULT);
            NV_ASSERT(status == NV_OK);
        }
        SLI_LOOP_END
    }

    //
    // ref-count increments for shared structs after all places where we
    // could return early.
    //
    if (pMemoryDst->pHwResource != NULL)
        pMemoryDst->pHwResource->refCount++;

    memdescAddRef(pMemoryDst->pMemDesc);
    pMemoryDst->pMemDesc->DupCount++;
    if (pMemoryDst->pMemDesc->Allocated)
        pMemoryDst->pMemDesc->Allocated++;

    // Insert pMemoryDst after pMemorySrc in circular list to track dup/sharing of pMemDesc
    pMemoryDst->dupListItem.pNext = pMemorySrc->dupListItem.pNext;
    pMemoryDst->dupListItem.pPrev = pMemorySrc;
    pMemorySrc->dupListItem.pNext = pMemoryDst;
    pMemoryDst->dupListItem.pNext->dupListItem.pPrev = pMemoryDst;

done:

    // If the original allocation was RPCed, also send the Dup.
    if (pMemory->bRpcAlloc && (IS_VIRTUAL(pSrcGpu) || IS_GSP_CLIENT(pSrcGpu)))
    {
        NV_RM_RPC_DUP_OBJECT(pSrcGpu, pDstClient->hClient, pDstParentRef->hResource, pDstRef->hResource,
                             pSrcClient->hClient, pSrcRef->hResource, 0,
                             NV_FALSE, // do not automatically issue RPC_FREE on object free
                             NULL,
                             status);
        NV_ASSERT(status == NV_OK);
    }

    // UNLOCK: release GPUs lock
    if (bReleaseGpuLock)
    {
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    }

    pMemory->bConstructed = (status == NV_OK);
    return status;
}

NV_STATUS
memGetMemInterMapParams_IMPL
(
    Memory *pMemory,
    RMRES_MEM_INTER_MAP_PARAMS *pParams
)
{
    OBJGPU             *pGpu = pParams->pGpu;
    RsResourceRef      *pMemoryRef = pParams->pMemoryRef;

    FlaMemory          *pFlaMemory;

    MEMORY_DESCRIPTOR  *pSrcMemDesc = pMemory->pMemDesc;
    Device             *pDevice;
    Subdevice          *pSubdevice;
    NvBool              bcState = gpumgrGetBcEnabledStatus(pGpu);

    // Don't expect to use default, but safe thing to do is set src=dest
    NvHandle            hMemoryDevice = 0;
    OBJGPU             *pSrcGpu = pGpu;

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO, memIsReady(pMemory, NV_FALSE));

    if (pMemoryRef->pParentRef != NULL)
    {
        pDevice = dynamicCast(pMemoryRef->pParentRef->pResource, Device);
        if (pDevice != NULL)
        {
            pSrcGpu = GPU_RES_GET_GPU(pDevice);
            hMemoryDevice = RES_GET_HANDLE(pDevice);
            GPU_RES_SET_THREAD_BC_STATE(pDevice);
        }
        else
        {
            pSubdevice = dynamicCast(pMemoryRef->pParentRef->pResource, Subdevice);
            if (pSubdevice != NULL)
            {
                pSrcGpu = GPU_RES_GET_GPU(pSubdevice);
                hMemoryDevice = RES_GET_HANDLE(pSubdevice->pDevice);
                GPU_RES_SET_THREAD_BC_STATE(pSubdevice);
            }
        }
    }

    pParams->pSrcGpu = pSrcGpu;
    pParams->hMemoryDevice = hMemoryDevice;

    //
    // Restore pGpu's bcState in case it was overwritten above (i.e.,
    // the case that hMemoryDevice and hBroadcastDevice are the same
    // device, but a unicast mapping was desired).
    //
    gpumgrSetBcEnabledStatus(pGpu, bcState);

    pParams->pSrcMemDesc = pSrcMemDesc;

    pFlaMemory = dynamicCast(pMemoryRef->pResource, FlaMemory);
    if (pFlaMemory != NULL)
    {
        pParams->pSrcGpu      = gpumgrGetGpu(pFlaMemory->peerGpuInst);
        pParams->bFlaMapping  = NV_TRUE;

        NV_PRINTF(LEVEL_INFO, "FLA memory imported as (%s) with exportGpu:%x \n",
                  (pParams->pSrcGpu != pGpu ? " P2P " : " LOOPBACK "),
                   pFlaMemory->peerDeviceInst);
    }

    return NV_OK;
}

NV_STATUS
memGetMemoryMappingDescriptor_IMPL
(
    Memory *pMemory,
    MEMORY_DESCRIPTOR **ppMemDesc
)
{
    NV_CHECK_OK_OR_RETURN(LEVEL_INFO, memIsReady(pMemory, NV_FALSE));
    if (pMemory->pGpu != NULL)
    {
        *ppMemDesc = memdescGetMemDescFromGpu(pMemory->pMemDesc, pMemory->pGpu);
    }
    else
    {
        *ppMemDesc = pMemory->pMemDesc;
    }
    return NV_OK;
}

NV_STATUS
memIsDuplicate_IMPL
(
    Memory   *pMemory,
    NvHandle  hMemory,
    NvBool   *pDuplicate
)
{
    RsClient *pClient = RES_GET_CLIENT(pMemory);
    Memory *pMemory1;

    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT,
                          memIsReady(pMemory, NV_FALSE));

    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT,
                          memGetByHandle(pClient, hMemory, &pMemory1));

    //
    // Do not dereference pMemdesc here. We only take RMAPI RO lock and
    // client lock in this context.
    //

    *pDuplicate = (pMemory->pMemDesc == pMemory1->pMemDesc);

    return NV_OK;
}

void memSetSysmemCacheAttrib_IMPL
(
    OBJGPU                          *pGpu,
    NV_MEMORY_ALLOCATION_PARAMS     *pAllocData,
    NvU32                           *pCpuCacheAttrib,
    NvU32                           *pGpuCacheAttrib
)
{
    //
    // For system memory default to GPU uncached. GPU caching is different from
    // the expected default memory model since it is not coherent.  Clients must
    // understand this and handle any coherency requirements explicitly.
    //
    if (DRF_VAL(OS32, _ATTR2, _GPU_CACHEABLE, pAllocData->attr2) ==
        NVOS32_ATTR2_GPU_CACHEABLE_DEFAULT)
    {
        pAllocData->attr2 = FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _NO,
                                        pAllocData->attr2);
    }

    if (DRF_VAL(OS32, _ATTR2, _GPU_CACHEABLE, pAllocData->attr2) ==
        NVOS32_ATTR2_GPU_CACHEABLE_YES)
    {
        *pGpuCacheAttrib = NV_MEMORY_CACHED;
    }
    else
    {
        *pGpuCacheAttrib = NV_MEMORY_UNCACHED;
    }

    if (DRF_VAL(OS32, _ATTR, _COHERENCY, pAllocData->attr) == NVOS32_ATTR_COHERENCY_UNCACHED)
        *pCpuCacheAttrib = NV_MEMORY_UNCACHED;
    else if (DRF_VAL(OS32, _ATTR, _COHERENCY, pAllocData->attr) == NVOS32_ATTR_COHERENCY_CACHED)
        *pCpuCacheAttrib = NV_MEMORY_CACHED;
    else if (DRF_VAL(OS32, _ATTR, _COHERENCY, pAllocData->attr) == NVOS32_ATTR_COHERENCY_WRITE_COMBINE)
        *pCpuCacheAttrib = NV_MEMORY_WRITECOMBINED;
    else if (DRF_VAL(OS32, _ATTR, _COHERENCY, pAllocData->attr) == NVOS32_ATTR_COHERENCY_WRITE_THROUGH)
        *pCpuCacheAttrib = NV_MEMORY_CACHED;
    else if (DRF_VAL(OS32, _ATTR, _COHERENCY, pAllocData->attr) == NVOS32_ATTR_COHERENCY_WRITE_PROTECT)
        *pCpuCacheAttrib = NV_MEMORY_CACHED;
    else if (DRF_VAL(OS32, _ATTR, _COHERENCY, pAllocData->attr) == NVOS32_ATTR_COHERENCY_WRITE_BACK)
        *pCpuCacheAttrib = NV_MEMORY_CACHED;
    else
        *pCpuCacheAttrib = 0;

    ct_assert(NVOS32_ATTR_COHERENCY_UNCACHED      == NVOS02_FLAGS_COHERENCY_UNCACHED);
    ct_assert(NVOS32_ATTR_COHERENCY_CACHED        == NVOS02_FLAGS_COHERENCY_CACHED);
    ct_assert(NVOS32_ATTR_COHERENCY_WRITE_COMBINE == NVOS02_FLAGS_COHERENCY_WRITE_COMBINE);
    ct_assert(NVOS32_ATTR_COHERENCY_WRITE_THROUGH == NVOS02_FLAGS_COHERENCY_WRITE_THROUGH);
    ct_assert(NVOS32_ATTR_COHERENCY_WRITE_PROTECT == NVOS02_FLAGS_COHERENCY_WRITE_PROTECT);
    ct_assert(NVOS32_ATTR_COHERENCY_WRITE_BACK    == NVOS02_FLAGS_COHERENCY_WRITE_BACK);
}
