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

/*!
 * @file
 * @brief  Memory descriptor handling utility routines.
 */

#include "gpu/mem_mgr/mem_desc.h"

#include "gpu/bif/kernel_bif.h"

#include "os/os.h"

#include "gpu_mgr/gpu_mgr.h"
#include "core/locks.h"
#include "mem_mgr/io_vaspace.h"
#include "mem_mgr/virt_mem_mgr.h"
#include "core/system.h"
#include "vgpu/vgpu_util.h"
#include "platform/sli/sli.h"
#include "resserv/rs_client.h"

#include "gpu/mem_mgr/virt_mem_allocator.h"

#include "rmconfig.h"
#include "vgpu/rpc.h"
#include "mem_mgr/mem.h"

#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/mem_utils.h"

#include "gpu/mem_mgr/heap.h"

#include "gpu/mem_sys/kern_mem_sys.h"
#include "mem_mgr/video_mem.h"

#include "mem_mgr/ctx_buf_pool.h"

#include "nvrm_registry.h" // For memdescOverrideInstLoc*()

#include "rmapi/rmapi.h"
#include "rmapi/rs_utils.h"
#include "class/cl0071.h" // NV01_MEMORY_SYSTEM_OS_DESCRIPTOR

#include "gpu/bus/kern_bus.h"

// Structure for keeping track of BAR1 mappings
typedef struct
{
    NvU64 FbAperture;
    NvU64 FbApertureLen;
    NvP64 pPriv;
} FB_MAPPING_INFO;

//
// Common address space lists
//
const NV_ADDRESS_SPACE ADDRLIST_FBMEM_PREFERRED[] = {ADDR_FBMEM, ADDR_SYSMEM, ADDR_UNKNOWN};
const NV_ADDRESS_SPACE ADDRLIST_SYSMEM_PREFERRED[] = {ADDR_SYSMEM, ADDR_FBMEM, ADDR_UNKNOWN};
const NV_ADDRESS_SPACE ADDRLIST_FBMEM_ONLY[] = {ADDR_FBMEM, ADDR_UNKNOWN};
const NV_ADDRESS_SPACE ADDRLIST_SYSMEM_ONLY[] = {ADDR_SYSMEM, ADDR_UNKNOWN};

// XXX These could probably encode the whole list in the u32 bits.
NvU32 memdescAddrSpaceListToU32(const NV_ADDRESS_SPACE *addrlist)
{
    if (addrlist == ADDRLIST_FBMEM_PREFERRED)
        return 1;
    else if (addrlist == ADDRLIST_SYSMEM_PREFERRED)
        return 2;
    else if (addrlist == ADDRLIST_FBMEM_ONLY)
        return 3;
    else if (addrlist == ADDRLIST_SYSMEM_ONLY)
        return 4;
    else
        return 0;
}

const NV_ADDRESS_SPACE *memdescU32ToAddrSpaceList(NvU32 index)
{
    switch (index)
    {
        case 1: return ADDRLIST_FBMEM_PREFERRED;
        case 2: return ADDRLIST_SYSMEM_PREFERRED;
        case 3: return ADDRLIST_FBMEM_ONLY;
        case 4: return ADDRLIST_SYSMEM_ONLY;
        default:
            return NULL;
    }
}

/*
 * @brief Setting a MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE has to initialize
 *        pHeap and bUsingSubAllocator flags
 */
static NV_STATUS _memdescSetSubAllocatorFlag
(
    OBJGPU *pGpu,
    PMEMORY_DESCRIPTOR pMemDesc,
    NvBool  bSet
)
{
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);

    if (!bSet)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unsetting MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE not supported\n");
        NV_ASSERT(0);
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_ASSERT(!(pMemDesc->_flags & MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL));

    // Set flag forcing the allocation to fall into suballocator
    pMemDesc->_flags |= MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;

    {
        Heap *pHeap = pMemDesc->pHeap;
        NvBool bForceSubheap = NV_FALSE;

        NV_ASSERT(pHeap == NULL || pHeap->heapType == HEAP_TYPE_PHYS_MEM_SUBALLOCATOR);

        if (pMemDesc->_flags & MEMDESC_FLAGS_FORCE_ALLOC_FROM_SUBHEAP)
        {
            bForceSubheap = NV_TRUE;
        }

        if (pHeap == NULL)
            pHeap = memmgrGetDeviceSuballocator(GPU_GET_MEMORY_MANAGER(pGpu), bForceSubheap);

        NV_ASSERT_OR_RETURN(pHeap != NULL, NV_ERR_INVALID_STATE);

        if (pHeap->heapType == HEAP_TYPE_PHYS_MEM_SUBALLOCATOR)
        {
            NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &pMemDesc->gfid));
            pMemDesc->bUsingSuballocator = NV_TRUE;
        }
    }

    return NV_OK;
}

/*!
 * @brief Initializing GFID for guest allocated memdescs
 */
static NV_STATUS _memdescSetGuestAllocatedFlag
(
    OBJGPU *pGpu,
    PMEMORY_DESCRIPTOR pMemDesc,
    NvBool  bSet
)
{
// for VGPU header scrubbing in Open Orin package

    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);

    if (!bSet)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unsetting MEMDESC_FLAGS_GUEST_ALLOCATED not supported\n");
        NV_ASSERT(0);
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &pMemDesc->gfid));
    pMemDesc->_flags |= MEMDESC_FLAGS_GUEST_ALLOCATED;

    return NV_OK;
}

/*!
 *  @brief Allocate and initialize a new empty memory descriptor
 *
 *  Allocate a new memory descriptor. This allocates the memory descriptor
 *  only. memdescAlloc or memdescDescribe are later used to allocate or associate
 *  memory to the memory descriptor.
 *
 *  This routine takes size and the physical contiguous of the future allocation
 *  in order to size the PTE array for non-contiguous requests.
 *
 *  memdescDestroy should be called to free a memory descriptor.
 *
 *  If MEMDESC_FLAGS_PRE_ALLOCATED is specified, use the memory descriptor
 *  supplied by the client instead of allocating a new one.
 *
 *  @param[out]  ppMemDesc              Return pointer to new memory descriptor
 *  @param[in]   pGpu
 *  @param[in]   Size                   Size of memory descriptor in bytes.
 *  @param[in]   PhysicallyContiguous   Need physical contig or can it be scattered?
 *  @param[in]   AddressSpace           NV_ADDRESS_SPACE requested
 *  @param[in]   CpuCacheAttrib         CPU cacheability requested
 *  @param[in]   Flags                  MEMDESC_FLAGS_*
 *
 *  @returns NV_OK on success
 */
NV_STATUS
memdescCreate
(
    MEMORY_DESCRIPTOR **ppMemDesc,
    OBJGPU *pGpu,
    NvU64 Size,
    NvU64 Alignment,
    NvBool PhysicallyContiguous,
    NV_ADDRESS_SPACE AddressSpace,
    NvU32 CpuCacheAttrib,
    NvU64 Flags
)
{
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU64              allocSize, MdSize, PageCount;
    NvU32              gpuCacheAttrib = NV_MEMORY_UNCACHED;
    NV_STATUS          status         = NV_OK;


    allocSize = Size;

    //
    // this memdesc may have gotten forced to sysmem if no carveout,
    // but for VPR it needs to be in vidmem, so check and re-direct here,
    // unless running with zero-FB
    //
    if ((AddressSpace != ADDR_UNKNOWN) &&
        (Flags & MEMDESC_ALLOC_FLAGS_PROTECTED) &&
        (!pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) ||
         gpuIsCacheOnlyModeEnabled(pGpu)))
    {
        AddressSpace = ADDR_FBMEM;
    }

    if (pGpu != NULL)
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

        if (((AddressSpace == ADDR_SYSMEM) || (AddressSpace == ADDR_UNKNOWN)) &&
            !(Flags & MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL))
        {
            if (pMemoryManager && pMemoryManager->sysmemPageSize)
            {
                allocSize = RM_ALIGN_UP(allocSize, pMemoryManager->sysmemPageSize);
                if (allocSize < Size)
                {
                    return NV_ERR_INVALID_ARGUMENT;
                }
            }
        }

        if (RMCFG_FEATURE_PLATFORM_MODS || IsT194(pGpu) || IsT234(pGpu))
        {
            if ( (AddressSpace == ADDR_FBMEM) &&
                !(Flags & MEMDESC_ALLOC_FLAGS_PROTECTED) &&
                memmgrGetUsableMemSizeMB_HAL(pGpu, pMemoryManager) == 0 &&
                gpuIsUnifiedMemorySpaceEnabled(pGpu))
            {
                // On Tegra, force sysmem if carveout and SMMU are not available
                AddressSpace = ADDR_SYSMEM;
                if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM))
                {
                    CpuCacheAttrib = pGpu->instCacheOverride;
                }
            }

            //
            // Support for aligned contiguous SYSMEM allocations.
            //
            if ((AddressSpace == ADDR_SYSMEM || AddressSpace == ADDR_UNKNOWN) &&
                PhysicallyContiguous && (Alignment > RM_PAGE_SIZE))
            {
                if (!portSafeAddU64(allocSize, (Alignment - RM_PAGE_SIZE), &allocSize))
                {
                    return NV_ERR_INVALID_ARGUMENT;
                }
            }
        }
    }

    //
    //
    // Note that we allocate one extra PTE, since we don't know what the PteAdjust
    // is yet; if the PteAdjust is zero, we simply won't use it.  This is in the
    // MEMORY_DESCRIPTOR structure definition.
    //
    // RM_PAGE_SIZE is 4k and RM_PAGE_SHIFT is 12, so shift operation can be
    // modified from ((allocSize + RM_PAGE_SIZE-1) >> RM_PAGE_SHIFT) to below as
    // (4k >> 12 = 1). This modification helps us to avoid overflow of variable
    // allocSize, in case caller of this function passes highest value of NvU64.
    //
    // If allocSize is passed as 0, PageCount should be returned as 0.
    //
    if (allocSize == 0)
    {
        PageCount = 0;
    }
    else
    {
        PageCount = ((allocSize - 1) >> RM_PAGE_SHIFT) + 1;
    }

    if (PhysicallyContiguous)
    {
        MdSize = sizeof(MEMORY_DESCRIPTOR);
    }
    else
    {
        MdSize = sizeof(MEMORY_DESCRIPTOR) +
            (sizeof(RmPhysAddr) * PageCount);
        NV_ASSERT(MdSize <= 0xffffffffULL);
        if (MdSize > 0xffffffffULL)
            return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    if (Flags & MEMDESC_FLAGS_PAGED_SYSMEM)
    {
        // The flag MEMDESC_FLAGS_PAGED_SYSMEM is only for Windows
        return NV_ERR_NOT_SUPPORTED;
    }

    if (Flags & MEMDESC_FLAGS_PRE_ALLOCATED)
    {
        // Only fixed sized memDesc can be supported
        if (PhysicallyContiguous == NV_FALSE)
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        NV_ASSERT_OR_RETURN(*ppMemDesc, NV_ERR_NOT_SUPPORTED);

        pMemDesc = *ppMemDesc;
    }
    else
    {
        pMemDesc = portMemAllocNonPaged((NvU32)MdSize);
        if (pMemDesc == NULL)
        {
            return NV_ERR_NO_MEMORY;
        }
    }

    portMemSet(pMemDesc, 0, (NvU32)MdSize);

    // Fill in initial non-zero parameters
    pMemDesc->pGpu                 = pGpu;
    pMemDesc->Size                 = Size;
    pMemDesc->PageCount            = PageCount;
    pMemDesc->ActualSize           = allocSize;
    pMemDesc->_addressSpace        = AddressSpace;
    pMemDesc->RefCount             = 1;
    pMemDesc->DupCount             = 1;
    pMemDesc->_subDeviceAllocCount = 1;
    pMemDesc->_flags               = Flags;
    pMemDesc->_gpuCacheAttrib      = gpuCacheAttrib;
    pMemDesc->_gpuP2PCacheAttrib   = NV_MEMORY_UNCACHED;
    pMemDesc->Alignment            = Alignment;
    pMemDesc->gfid                 = GPU_GFID_PF;
    pMemDesc->bUsingSuballocator   = NV_FALSE;
    pMemDesc->bDeferredFree        = NV_FALSE;
    pMemDesc->numaNode             = NV0000_CTRL_NO_NUMA_NODE;

    pMemDesc->libosRegionHandle    = 0;
    pMemDesc->baseVirtualAddress   = 0;

    // parameter to determine page granularity
    pMemDesc->pageArrayGranularity = RM_PAGE_SIZE;

    memdescSetCpuCacheAttrib(pMemDesc, CpuCacheAttrib);

    // Set any additional flags
    pMemDesc->_flags               |= MEMDESC_FLAGS_KERNEL_MODE;
    if (PhysicallyContiguous)
        pMemDesc->_flags |= MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS;
    else
        pMemDesc->_flags &= ~MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS;

    // OBJHEAP may not be created at this time and pMemDesc->pHeap may be NULL after this if-else
    if (Flags & MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL)
    {
        pMemDesc->_flags |= MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL;
        pMemDesc->_flags &= ~MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;
    }
    else if (Flags & MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE)
    {
        NV_ASSERT_OK_OR_GOTO(status, _memdescSetSubAllocatorFlag(pGpu, pMemDesc, NV_TRUE), failed);
    }

    // In case of guest allocated memory, just initialize GFID
    if (Flags & MEMDESC_FLAGS_GUEST_ALLOCATED)
    {
        NV_ASSERT_OK_OR_GOTO(status, _memdescSetGuestAllocatedFlag(pGpu, pMemDesc, NV_TRUE), failed);
    }

failed:
    if (status != NV_OK)
    {
        if (!(Flags & MEMDESC_FLAGS_PRE_ALLOCATED))
        {
            portMemFree(pMemDesc);
        }
    }
    else
    {
        *ppMemDesc = pMemDesc;
    }

    return status;
}

/*!
 *  @brief Initialize an caller allocated memory descriptor
 *
 *  Helper to make it easier to get the memDesc **, and typically used
 *  with memdescDescribe.
 *
 *  Only can be used for physically contiguous regions with a fixed
 *  size PTE array.
 *
 *  memdescDestroy should be called to free a memory descriptor.
 *
 *  If MEMDESC_FLAGS_PRE_ALLOCATED is specified, use the memory descriptor
 *  supplied by the client instead of allocating a new one.
 *
 *  @param[out]  pMemDesc               Return pointer to new memory descriptor
 *  @param[in]   pGpu
 *  @param[in]   Size                   Size of memory descriptor in bytes
 *  @param[in]   AddressSpace           NV_ADDRESS_SPACE requested
 *  @param[in]   CpuCacheAttrib         CPU cacheability requested
 *  @param[in]   Flags                  MEMDESC_FLAGS_*
 *
 *  @returns void with no malloc there should be no failure cases
 */
void
memdescCreateExisting
(
    MEMORY_DESCRIPTOR *pMemDesc,
    OBJGPU *pGpu,
    NvU64 Size,
    NV_ADDRESS_SPACE AddressSpace,
    NvU32 CpuCacheAttrib,
    NvU64 Flags
)
{
    NV_STATUS status;
    status = memdescCreate(&pMemDesc, pGpu, Size, 0, NV_TRUE, AddressSpace,
                           CpuCacheAttrib,
                           Flags | MEMDESC_FLAGS_PRE_ALLOCATED | MEMDESC_FLAGS_SKIP_RESOURCE_COMPUTE);
    NV_ASSERT(status == NV_OK);
}


/*!
 * Increment ref count
 */
void memdescAddRef
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_ASSERT(pMemDesc != NULL);
    ++(pMemDesc->RefCount);
}

/*!
 * Decrement ref count
 */
void memdescRemoveRef
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_ASSERT_OR_RETURN_VOID(pMemDesc != NULL);
    --(pMemDesc->RefCount);
}

//
// Destroy all IOMMU mappings under this memdesc, including child
// mappings for root memdescs.
//
// TODO: merge the new IOMMU paths with the SMMU paths (see bug 1625121).
//
static void
_memdescFreeIommuMappings(PMEMORY_DESCRIPTOR pMemDesc)
{
#if (RMCFG_FEATURE_PLATFORM_UNIX || RMCFG_FEATURE_PLATFORM_MODS) && !NVCPU_IS_ARM
    PIOVAMAPPING pIovaMapping = pMemDesc->_pIommuMappings;

    if (!pIovaMapping)
        return;

    if (memdescIsSubMemoryMemDesc(pMemDesc))
    {
        iovaMappingDestroy(pIovaMapping);
        return;
    }

    while (pIovaMapping)
    {
        PIOVAMAPPING pTmpIovaMapping = pIovaMapping->pNext;
        iovaMappingDestroy(pIovaMapping);
        pIovaMapping = pTmpIovaMapping;
    }

    pMemDesc->_pIommuMappings = NULL;
#endif
}

/*!
 *  Destroy a memory descriptor if last reference is released
 *
 *  If the memory descriptor is down to one reference, we need
 *  to check with the bus code check if that reference needs
 *  to be reclaimed.
 *
 *  @param[in] pMemDesc Memory descriptor to be destroyed
 *
 *  @returns None
 */
void
memdescDestroy
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    // Allow null frees
    if (!pMemDesc)
    {
        return;
    }

    memdescRemoveRef(pMemDesc);

    // if still more references are there for pMemDesc (pMemDesc->RefCount != 0), then bail out.

    if (pMemDesc->RefCount == 0)
    {
        MEM_DESC_DESTROY_CALLBACK *pCb = memdescGetDestroyCallbackList(pMemDesc);
        MEM_DESC_DESTROY_CALLBACK *pNext;

        if (pMemDesc->_flags & MEMDESC_FLAGS_RESTORE_PTE_KIND_ON_FREE)
        {
            NV_ASSERT_OK(kbusUpdateStaticBar1VAMapping_HAL(pMemDesc->pGpu,
                             GPU_GET_KERNEL_BUS(pMemDesc->pGpu), pMemDesc,
                             0, memdescGetSize(pMemDesc), NV_TRUE));
        }

        if (pMemDesc->_flags & MEMDESC_FLAGS_DUMMY_TOPLEVEL)
        {
            // When called from RmFreeFrameBuffer() and memdescFree could not do it because it is unallocated.
            pMemDesc->_pNext = NULL;
            pMemDesc->_subDeviceAllocCount = 1;
        }

        NV_ASSERT(pMemDesc->childDescriptorCnt == 0);
        NV_ASSERT(pMemDesc->_addressSpace == ADDR_FBMEM || pMemDesc->pHeap == NULL);

        //
        // If there is private memdata, use the CB to free
        //
        if (pMemDesc->_pMemData && pMemDesc->_pMemDataReleaseCallback)
        {
            pMemDesc->_pMemDataReleaseCallback(pMemDesc);
        }

        if (pMemDesc->bDeferredFree)
        {
            memdescFree(pMemDesc);
        }
        else if (pMemDesc->Allocated != 0)
        {
            //
            // The caller forgot to free the actual memory before destroying the memdesc.
            // Please fix this by calling memdescFree().
            // To prevent memory leaks, we explicitly free here until its fixed elsewhere.
            //
            NV_PRINTF(LEVEL_ERROR, "Destroying unfreed memory %p\n", pMemDesc);
            NV_PRINTF(LEVEL_ERROR, "Please call memdescFree()\n");
            memdescFree(pMemDesc);
            NV_ASSERT(!pMemDesc->Allocated);
        }

        if (memdescGetStandbyBuffer(pMemDesc))
        {
            memdescFree(memdescGetStandbyBuffer(pMemDesc));
            memdescDestroy(memdescGetStandbyBuffer(pMemDesc));
            memdescSetStandbyBuffer(pMemDesc, NULL);
        }

        //
        // Submemory descriptors will be destroyed without going through a free
        // path, so we need to make sure that we remove the IOMMU submapping
        // here. For root descriptors, we should already have removed all the
        // associated IOVA mappings.
        //
        // However, for memory descriptors that weren't allocated by the RM,
        // (e.g., were created from a user allocation), we won't go through a
        // free path at all. In this case, mappings for other GPUs may still be
        // attached to this root memory descriptor, so release them now.
        //
        _memdescFreeIommuMappings(pMemDesc);

        // Notify all interested parties of destruction
        while (pCb)
        {
            pNext = pCb->pNext;
            pCb->destroyCallback(pMemDesc->pGpu, pCb->pObject, pMemDesc);
            // pCb is now invalid
            pCb = pNext;
        }

        portMemFree(pMemDesc->pPteSpaMappings);
        pMemDesc->pPteSpaMappings = NULL;
        portMemFree(pMemDesc->pSubMemDescList);
        pMemDesc->pSubMemDescList = NULL;

        if (pMemDesc->pPteEgmMappings != NULL)
        {
            portMemFree(pMemDesc->pPteEgmMappings);
            pMemDesc->pPteEgmMappings = NULL;
        }

        if (pMemDesc->_pParentDescriptor)
        {
            if ((pMemDesc->_flags & MEMDESC_FLAGS_PRE_ALLOCATED) == 0)
                pMemDesc->_pParentDescriptor->childDescriptorCnt--;
            memdescDestroy(pMemDesc->_pParentDescriptor);
            pMemDesc->_pParentDescriptor = NULL;
        }

        // Verify memdesc is not top
        NV_ASSERT(memdescHasSubDeviceMemDescs(pMemDesc) == NV_FALSE);

        if ((pMemDesc->_flags & MEMDESC_FLAGS_PRE_ALLOCATED) == 0)
        {
            portMemFree(pMemDesc);
        }
    }
}

/*!
 *  @brief Function that frees subdevice memory descriptors. If there are no
 *  subdevice memory descriptors function just simply resets memdesc structure.
 *  Top level memory descriptor is not destroyed.
 *
 *  @param[in,out] pMemDesc  Top level memory descriptor.
 *
 *  @returns None
 */
static void
_memSubDeviceFreeAndDestroy
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    MEMORY_DESCRIPTOR *pSubDevMemDesc = pMemDesc->_pNext;
    MEMORY_DESCRIPTOR *pNextMemDesc;
    OBJGPU            *pGpu           = pMemDesc->pGpu;
    NvBool             bBcState;

    // No subdevice memdescs
    if (pSubDevMemDesc == NULL || pGpu == NULL)
    {
        return;
    }

    bBcState       = gpumgrGetBcEnabledStatus(pGpu);
    gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);

    do
    {
        pNextMemDesc = pSubDevMemDesc->_pNext;
        pSubDevMemDesc->_pNext = NULL;
        memdescFree(pSubDevMemDesc);
        memdescDestroy(pSubDevMemDesc);
        pSubDevMemDesc = pNextMemDesc;
    } while (pSubDevMemDesc != NULL);

    gpumgrSetBcEnabledStatus(pGpu, bBcState);
}

/*!
 *  @brief Lower memdesc allocation layer for the special case of allocation
           in the VPR region when MODS is managing it.
 *
 *  @param[in] pMemDesc  Memory descriptor to allocate
 *
 *  @returns NV_OK on successful allocation.
 *           NV_ERR_NOT_SUPPORTED if not supported
 */
static NV_STATUS
_memdescAllocVprRegion
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 *  @brief Allocate and populate the EGM array based off of the already
 *         populated _pteArray of the memdesc
 *
 *  @param[in] pMemDesc Memory descriptor to allocate EGM array in
 *
 *  @returns NV_OK on successful allocation. NV_ERR if not.
 */
static NV_INLINE NV_STATUS
_memdescAllocEgmArray
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    //
    // Since we allocate an extra PTE, we need to account for that in case we
    // need it for the EGM array. This is why there is a '+ 1' for pageCount.
    //
    NvU64 i;
    NvU64 pageCount = pMemDesc->PageCount + 1;

    //
    // Get the root memory descriptor's memory manager to be able to get the
    // EGM base of that GPU, instead of the mapping GPU in the case of this
    // array being used in a submemdesc. The submemdesc should always have the
    // mapping of the root since it's a submemdesc derived from the root, and
    // not based on the mapping GPU.
    //
    MEMORY_DESCRIPTOR *pRootMemDesc   = memdescGetRootMemDesc(pMemDesc, NULL);
    MemoryManager     *pMemoryManager = GPU_GET_MEMORY_MANAGER(pRootMemDesc->pGpu);

    if (pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS)
    {
        pageCount = 1;
    }

    if (pMemDesc->pPteEgmMappings == NULL)
    {
        pMemDesc->pPteEgmMappings = portMemAllocNonPaged(sizeof(RmPhysAddr) * pageCount);
    }

    NV_ASSERT_OR_RETURN(pMemDesc->pPteEgmMappings != NULL, NV_ERR_NO_MEMORY);

    for (i = 0; i < pageCount; i++)
    {
        pMemDesc->pPteEgmMappings[i] = pMemDesc->_pteArray[i] -
                                       pMemoryManager->localEgmBasePhysAddr;
    }

    return NV_OK;
}

/*!
 *  @brief Lower memdesc allocation layer. Provides underlying allocation
 *  functionality.
 *
 *  @param[in,out] pMemDesc  Memory descriptor to allocate
 *
 *  @returns NV_OK on successful allocation. Various NV_ERR_GENERIC codes otherwise.
 */
static NV_STATUS
_memdescAllocInternal
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    OBJGPU                      *pGpu               = pMemDesc->pGpu;
    NV_STATUS                    status             = NV_OK;
    FB_ALLOC_INFO               *pFbAllocInfo       = NULL;
    FB_ALLOC_PAGE_FORMAT        *pFbAllocPageFormat = NULL;

    if (pMemDesc->Allocated)
    {
        NV_ASSERT(!pMemDesc->Allocated);
        return NV_ERR_INVALID_OBJECT_BUFFER;
    }

    // Special case of an allocation request in MODS managed VPR region.
    status = _memdescAllocVprRegion(pMemDesc);
    if (status != NV_ERR_NOT_SUPPORTED)
        goto done;

    switch (pMemDesc->_addressSpace)
    {
        case ADDR_EGM:
        case ADDR_SYSMEM:
            // System memory can be obtained from osAllocPages
            status = osAllocPages(pMemDesc);
            if (status != NV_OK)
            {
                goto done;
            }

            if (memdescIsEgm(pMemDesc))
            {
                NV_ASSERT_OK_OR_GOTO(status,
                                     _memdescAllocEgmArray(pMemDesc),
                                     done);
            }

            //
            // The pages have been allocated, so mark the descriptor as
            // allocated. The IOMMU-mapping code needs the memdesc to be
            // allocated in order to create the mapping.
            //
            pMemDesc->Allocated = 1;

            //
            // TODO: merge new IOMMU paths with the SMMU paths below (see bug
            //       1625121). For now they are parallel, and only one will be
            //       used.
            //
            if (!memdescGetFlag(pMemDesc, MEMDESC_FLAGS_CPU_ONLY) &&
                !memdescIsEgm(pMemDesc) &&
                !memdescGetFlag(pMemDesc, MEMDESC_FLAGS_SKIP_IOMMU_MAPPING))
            {
                status = memdescMapIommu(pMemDesc, pGpu->busInfo.iovaspaceId);
                if (status != NV_OK)
                {
                    pMemDesc->Allocated = 0;
                    osFreePages(pMemDesc);
                    goto done;
                }
            }

            if (pMemDesc->_flags & MEMDESC_FLAGS_PROVIDE_IOMMU_MAP)
            {
                NV_PRINTF(LEVEL_ERROR, "SMMU mapping allocation is not supported for ARMv7.\n");
                NV_ASSERT(0);
                status = NV_ERR_NOT_SUPPORTED;
                goto done;
            }
            else if ((pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS) &&
                     RMCFG_FEATURE_PLATFORM_MODS)
            {
                if (pMemDesc->Alignment > RM_PAGE_SIZE)
                {
                    RmPhysAddr addr = memdescGetPhysAddr(pMemDesc, AT_CPU, 0);
                    NvU64      offset;

                    NV_ASSERT((addr & (RM_PAGE_SIZE - 1)) == 0);

                    NV_ASSERT((pMemDesc->Alignment & (pMemDesc->Alignment - 1)) == 0);
                    offset = addr & (pMemDesc->Alignment - 1);

                    if (offset)
                    {
                        NV_ASSERT((pMemDesc->PageCount * pMemDesc->pageArrayGranularity - pMemDesc->Size) >= offset);
                        NV_ASSERT(pMemDesc->PteAdjust == 0);
                        pMemDesc->PteAdjust += NvU64_LO32(pMemDesc->Alignment - offset);
                    }
                }
            }

            break;
        case ADDR_FBMEM:
        {
            Heap *pHeap = pMemDesc->pHeap;

            if (RMCFG_FEATURE_PMA &&
                (pMemDesc->_flags & MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL))
            {
                CTX_BUF_POOL_INFO *pCtxBufPool = NULL;
                pCtxBufPool = memdescGetCtxBufPool(pMemDesc);
                NV_ASSERT_TRUE_OR_GOTO(status, pCtxBufPool != NULL, NV_ERR_INVALID_STATE, done);

                // If pool is setup then allocate from pool
                NV_ASSERT_OK_OR_GOTO(status, ctxBufPoolAllocate(pCtxBufPool, pMemDesc), done);
            }
            else
            {
                // XXX Hack!
                MEMORY_ALLOCATION_REQUEST allocRequest = {0};
                NV_MEMORY_ALLOCATION_PARAMS allocData = {0};
                MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
                NvU64 requestedSize = pMemDesc->Size;

                allocRequest.pUserParams = &allocData;

                // Don't allow FB allocations if FB is broken unless running in L2 cache only mode
                if (pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) &&
                    !gpuIsCacheOnlyModeEnabled(pGpu))
                {
                    DBG_BREAKPOINT();
                    status = NV_ERR_BROKEN_FB;
                    goto done;
                }

                allocData.owner = HEAP_OWNER_RM_CHANNEL_CTX_BUFFER;
                allocData.type = NVOS32_TYPE_IMAGE;
                allocData.flags = NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE;

                // remove the "grows_down" flag when bReservedMemAtBottom is set so as to move RM memory to the bottom.
                if (!pMemoryManager->bReservedMemAtBottom)
                {
                    allocData.flags |= NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN;
                }

                // Allocate in high priority memory?
                if (pMemDesc->_flags & MEMDESC_FLAGS_HIGH_PRIORITY)
                {
                    allocData.attr2 |= DRF_DEF(OS32, _ATTR2, _PRIORITY, _HIGH);
                }
                else if (pMemDesc->_flags & MEMDESC_FLAGS_LOW_PRIORITY)
                {
                    allocData.attr2 |= DRF_DEF(OS32, _ATTR2, _PRIORITY, _LOW);
                }

                allocData.size = pMemDesc->Size;
                allocData.alignment = pMemDesc->Alignment;
                allocRequest.pMemDesc = pMemDesc;

                if (pMemDesc->_pageSize == RM_PAGE_SIZE)
                {
                    allocData.attr |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _4KB);
                }
                else if (pMemDesc->_pageSize == RM_PAGE_SIZE_64K ||
                     pMemDesc->_pageSize == RM_PAGE_SIZE_128K)
                {
                    allocData.attr |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _BIG);
                }

                allocData.flags |= pMemDesc->Alignment ?
                    NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE :
                    NVOS32_ALLOC_FLAGS_FORCE_ALIGN_HOST_PAGE;

                if (pMemDesc->_flags & MEMDESC_FLAGS_FIXED_ADDRESS_ALLOCATE)
                {
                    allocData.flags |= NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
                    allocData.offset = pMemDesc->_pteArray[0];
                }

                if (pMemDesc->_gpuCacheAttrib == NV_MEMORY_CACHED)
                {
                    allocData.attr2 |= DRF_DEF(OS32, _ATTR2, _GPU_CACHEABLE, _YES);
                }
                else
                {
                    // Force internal allocations to uncached unless explicitly requested.
                    allocData.attr2 |= DRF_DEF(OS32, _ATTR2, _GPU_CACHEABLE, _NO);
                }

                allocData.attr2 = FLD_SET_DRF(OS32, _ATTR2, _INTERNAL, _YES, allocData.attr2);

                if (pMemDesc->_flags & MEMDESC_ALLOC_FLAGS_PROTECTED)
                {
                    allocData.flags |= NVOS32_ALLOC_FLAGS_PROTECTED;
                }

                //
                // Assume all RM internal allocations to go into protected (CPR)
                // video memory unless specified otherwise explicitly
                //
                if (gpuIsCCFeatureEnabled(pGpu))
                {
                    if (pMemDesc->_flags & MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY)
                    {
                        //
                        // CC-TODO: Remove this check after non-CPR region is
                        // created. Not sure if RM will ever need to use non-CPR
                        // region for itself
                        //
                        NV_PRINTF(LEVEL_ERROR, "Non-CPR region still not created\n");
                        NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_ARGUMENT);
                    }
                    else
                    {
                        allocData.flags |= NVOS32_ALLOC_FLAGS_PROTECTED;
                    }
                }

                allocData.attr |= DRF_DEF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS);

                pFbAllocInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
                NV_ASSERT_TRUE_OR_GOTO(status, pFbAllocInfo != NULL, NV_ERR_NO_MEMORY, done);

                pFbAllocPageFormat = portMemAllocNonPaged(sizeof(FB_ALLOC_PAGE_FORMAT));
                NV_ASSERT_TRUE_OR_GOTO(status, pFbAllocPageFormat != NULL, NV_ERR_NO_MEMORY, done);

                portMemSet(pFbAllocInfo, 0, sizeof(FB_ALLOC_INFO));
                portMemSet(pFbAllocPageFormat, 0, sizeof(FB_ALLOC_PAGE_FORMAT));
                pFbAllocInfo->pageFormat = pFbAllocPageFormat;

                memUtilsInitFBAllocInfo(&allocData, pFbAllocInfo, 0, 0); // Client/device N/A

                status = memmgrAllocResources(pGpu, pMemoryManager, &allocRequest, pFbAllocInfo);
                if (status != NV_OK)
                    goto done;

                status = vidmemAllocResources(pGpu, pMemoryManager, &allocRequest, pFbAllocInfo, pHeap);
                if (status != NV_OK)
                    goto done;

                pMemDesc->Alignment = allocData.alignment;

                // Update MemDesc GPU cacheability with results of allocation
                if (DRF_VAL(OS32, _ATTR2, _GPU_CACHEABLE, allocData.attr2) == NVOS32_ATTR2_GPU_CACHEABLE_YES)
                {
                    pMemDesc->_gpuCacheAttrib = NV_MEMORY_CACHED;
                }
                else
                {
                    pMemDesc->_gpuCacheAttrib = NV_MEMORY_UNCACHED;
                }

                //
                // Adjust size to the requested size, not the heap rounded size.  A number of callers
                // depend on this. In the future we will have the PageCount be accurate.
                //
                pMemDesc->Size = requestedSize;
                pMemDesc->PageCount = ((pMemDesc->Size + pMemDesc->PteAdjust + pMemDesc->pageArrayGranularity - 1) >>
                                        BIT_IDX_32(pMemDesc->pageArrayGranularity));
            }
            // We now have the memory
            pMemDesc->Allocated = 1;

            // If the allocation succeeds and if its PhysMemSubAlloc, increment the refCount
            if ((status == NV_OK) && (pHeap != NULL) &&
                (pHeap->heapType == HEAP_TYPE_PHYS_MEM_SUBALLOCATOR))
            {
                heapAddRef(pHeap);
            }
            break;
        }

        default:
            // Don't know how to do any other types of memory yet
            DBG_BREAKPOINT();
            status = NV_ERR_GENERIC;
            goto done;
    }

done:
    if (status == NV_OK)
    {
        memdescPrintMemdesc(pMemDesc, NV_TRUE, MAKE_NV_PRINTF_STR("memdesc allocated"));
    }
    else if (pMemDesc->pPteEgmMappings != NULL)
    {
        portMemFree(pMemDesc->pPteEgmMappings);
        pMemDesc->pPteEgmMappings = NULL;
    }
    portMemFree(pFbAllocPageFormat);
    portMemFree(pFbAllocInfo);

    return status;
}

/*!
 *  @brief Upper memdesc allocation layer. Provides support for per-subdevice
 *  sysmem buffers and lockless sysmem allocation.
 *
 *  @param[in,out] pMemDesc  Memory descriptor to allocate
 *
 *  @returns NV_OK on successful allocation. Various NV_ERR_GENERIC codes otherwise.
 */
NV_STATUS
memdescAlloc
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    OBJGPU             *pGpu        = pMemDesc->pGpu;
    NV_STATUS           status      = NV_OK;
    NvBool              bcState     = NV_FALSE;
    OBJSYS             *pSys        = SYS_GET_INSTANCE();
    NvBool              reAcquire;
    NvU32               gpuMask     = 0;

    NV_ASSERT_OR_RETURN(!pMemDesc->Allocated, NV_ERR_INVALID_OBJECT_BUFFER);

    switch (pMemDesc->_addressSpace)
    {
        case ADDR_SYSMEM:
        case ADDR_EGM:
            // Can't alloc sysmem on GSP firmware.
            if (RMCFG_FEATURE_PLATFORM_GSP && !memdescGetFlag(pMemDesc, MEMDESC_FLAGS_GUEST_ALLOCATED))
            {
                //
                // TO DO: Make this an error once existing allocations are cleaned up.
                // After that pHeap selection can be moved to memdescAllocInternal()
                //
                NV_PRINTF(LEVEL_WARNING,
                          "WARNING sysmem alloc on GSP firmware\n");
                pMemDesc->_addressSpace = ADDR_FBMEM;
                pMemDesc->pHeap = GPU_GET_HEAP(pGpu);
            }
            //
            // If AMD SEV is enabled but CC or APM is not enabled on the GPU,
            // all RM and client allocations must to to unprotected sysmem.
            // So, we override any unprotected/protected flag set by either RM
            // or client.
            // If APM is enabled and RM is allocating sysmem for its internal use
            // use such memory has to be unprotected as protected sysmem is not
            // accessible to GPU
            //
            if ((sysGetStaticConfig(pSys))->bOsCCEnabled)
            {
                if (!gpuIsCCorApmFeatureEnabled(pGpu) ||
                    (gpuIsApmFeatureEnabled(pGpu) &&
                     !memdescGetFlag(pMemDesc, MEMDESC_FLAGS_SYSMEM_OWNED_BY_CLIENT)))
                {
                    memdescSetFlag(pMemDesc,
                        MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY, NV_TRUE);
                }
            }
            else
            {
                //
                // This flag has no meaning on non-SEV systems. So, unset it. The
                // OS layer currently honours this flag irrespective of whether
                // SEV is enabled or not
                //
                memdescSetFlag(pMemDesc,
                        MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY, NV_FALSE);
            }

            break;
        case ADDR_FBMEM:
        {
            //
            // When APM is enabled, all RM internal vidmem allocations go to
            // unprotected memory. There is an underlying assumption that
            // memdescAlloc won't be directly called in the client vidmem alloc
            // codepath. Note that memdescAlloc still gets called in the client
            // sysmem alloc codepath. See CONFCOMP-529
            //
            if (gpuIsApmFeatureEnabled(pGpu))
            {
                memdescSetFlag(pMemDesc,
                    MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY, NV_TRUE);
            }
            // If FB is broken then don't allow the allocation, unless running in L2 cache only mode
            if (pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) &&
                !gpuIsCacheOnlyModeEnabled(pGpu))
            {
                status = NV_ERR_BROKEN_FB;
                NV_PRINTF(LEVEL_ERROR, "Unsupported FB bound allocation on broken FB(0FB) platform\n");
                DBG_BREAKPOINT();
            }

            NV_ASSERT(pMemDesc->pHeap == NULL);
            // Set the pHeap based on who owns this allocation
            if (pMemDesc->_flags & MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL)
            {
                //
                // pHeap is not required in memdesc for ctx buf pools because each ctx buf
                // pool is tied to PMA and this pools is cached inside memdesc.
                //
                CTX_BUF_POOL_INFO *pCtxBufPool = memdescGetCtxBufPool(pMemDesc);
                NV_ASSERT_OR_RETURN(pCtxBufPool != NULL, NV_ERR_INVALID_STATE);
            }
            else if (pMemDesc->_flags & MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE)
            {
                NvBool bForceSubheap = NV_FALSE;

                if (pMemDesc->_flags & MEMDESC_FLAGS_FORCE_ALLOC_FROM_SUBHEAP)
                {
                    bForceSubheap = NV_TRUE;
                }

                pMemDesc->pHeap = memmgrGetDeviceSuballocator(GPU_GET_MEMORY_MANAGER(pGpu), bForceSubheap);
            }
            else if (GPU_GET_MEMORY_MANAGER(pGpu) != NULL &&
                RMCFG_MODULE_HEAP &&
                pMemDesc->_addressSpace == ADDR_FBMEM)
            {
                pMemDesc->pHeap = GPU_GET_HEAP(pGpu);
            }

            break;
        }
        default:
            // Don't know how to do any other types of memory yet
            DBG_BREAKPOINT();
            return NV_ERR_GENERIC;
    }

    if (status != NV_OK)
    {
        return status;
    }

    if (gpumgrGetBcEnabledStatus(pGpu))
    {
        // Broadcast memdescAlloc call with flag set to allocate per subdevice.
        if (pMemDesc->_flags & MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE)
        {
            NvU32               i;
            MEMORY_DESCRIPTOR   *pSubDevMemDesc = pMemDesc;
            MEMORY_DESCRIPTOR   *pPrev = pMemDesc;
            OBJGPU             *pGpuChild;

            pMemDesc->_subDeviceAllocCount = NumSubDevices(pGpu);

            for (i = 0; i < pMemDesc->_subDeviceAllocCount; i++)
            {
                // Get pGpu for this subdeviceinst
                pGpuChild = gpumgrGetGpuFromSubDeviceInst(gpuGetDeviceInstance(pGpu), i);
                if (NULL == pGpuChild)
                {
                    NV_ASSERT(0);
                    status = NV_ERR_OBJECT_NOT_FOUND;
                    goto subdeviceAlloc_failed;
                }

                //
                // We are accessing the fields of the top level desc here directly without using the
                // accessor routines on purpose.
                //
                status = memdescCreate(&pSubDevMemDesc, pGpuChild, pMemDesc->Size, pMemDesc->Alignment,
                                       !!(pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS),
                                       pMemDesc->_addressSpace,
                                       pMemDesc->_cpuCacheAttrib,
                                       pMemDesc->_flags & ~MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE);

                if (status != NV_OK)
                {
                    NV_ASSERT(0);
                    goto subdeviceAlloc_failed;
                }

                pSubDevMemDesc->_gpuCacheAttrib   = pMemDesc->_gpuCacheAttrib;
                pSubDevMemDesc->_pageSize = pMemDesc->_pageSize;

                // Force broadcast state to false when allocating a subdevice memdesc
                gpumgrSetBcEnabledStatus(pGpuChild, NV_FALSE);

                status = memdescAlloc(pSubDevMemDesc);

                if (pMemDesc->_addressSpace == ADDR_FBMEM)
                {
                    //
                    // The top level memdesc could have flags that don't reflect contiguity which
                    // is set after memdescAlloc.
                    //
                    pMemDesc->Alignment            = pSubDevMemDesc->Alignment;
                    pMemDesc->_flags               = pSubDevMemDesc->_flags | MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE;
                    pMemDesc->ActualSize           = pSubDevMemDesc->ActualSize;
                }

                // Restore broadcast state to true after allocating a subdevice memdesc
                gpumgrSetBcEnabledStatus(pGpuChild, NV_TRUE);

                if (status != NV_OK)
                {
                    memdescDestroy(pSubDevMemDesc);
                    NV_ASSERT(0);
                    goto subdeviceAlloc_failed;
                }

                // Check for similarity in allocations for previous allocated subdev with current allocated subdev.
                // If subdev0 ~ subdev1 && subdev1~subdev2 then subdev0 ~ subdev2 and so on...Thus can check symmetry across all subdev allocations
                if (i > 0)
                {
                    NV_ASSERT(pPrev->Size == pSubDevMemDesc->Size);
                    NV_ASSERT(pPrev->PteAdjust == pSubDevMemDesc->PteAdjust);
                    NV_ASSERT(pPrev->_addressSpace == pSubDevMemDesc->_addressSpace);
                    NV_ASSERT(pPrev->_flags == pSubDevMemDesc->_flags);
                    NV_ASSERT(pPrev->_pteKind == pSubDevMemDesc->_pteKind);
                    NV_ASSERT(pPrev->_pteKindCompressed == pSubDevMemDesc->_pteKindCompressed);
                    NV_ASSERT(pPrev->pHeap != pSubDevMemDesc->pHeap);
                }

                pPrev->_pNext = pSubDevMemDesc;
                pPrev        = pSubDevMemDesc;
            }
            pMemDesc->Allocated = 1;
            return NV_OK;
        }
        else if (pMemDesc->_addressSpace == ADDR_FBMEM)
        {
            // Broadcast memdescAlloc call on vidmem *without* flag set to allocate per subdevice
            NV_ASSERT(0);
        }
    }

    // Unicast memdescAlloc call but with flag set to allocate per subdevice.
    NV_ASSERT(!((pMemDesc->_flags & MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE) && !gpumgrGetBcEnabledStatus(pGpu)));

    reAcquire = NV_FALSE;
    bcState = NV_FALSE;

    if ((pMemDesc->_flags & MEMDESC_FLAGS_LOCKLESS_SYSMEM_ALLOC) && (pMemDesc->_addressSpace != ADDR_FBMEM))
    {
        bcState = gpumgrGetBcEnabledStatus(pGpu);
        if (RMCFG_FEATURE_RM_BASIC_LOCK_MODEL)
        {
            //
            // There is no equivalent routine for osCondReleaseRmSema in
            // the new basic lock model.

            //
            // However, we can't drop the RM system semaphore in this
            // path because on non-windows platforms (i.e. MODS) it
            // has undesirable consequences.  So for now we must
            // bracket this section with a reference to the feature
            // flag until we can rework this interface.
            //
            //
            // Check to make sure we own the lock and that we are
            // not at elevated IRQL; this models the behavior
            // of osCondReleaseRmSema.
            //
            if (!osIsRaisedIRQL() &&
                (rmGpuGroupLockIsOwner(pGpu->gpuInstance, GPU_LOCK_GRP_DEVICE, &gpuMask) ||
                 rmGpuGroupLockIsOwner(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE, &gpuMask)))
            {
                //
                // Release all owned gpu locks rather than just the
                // device-related locks because the caller may be holding more
                // than the required device locks. All currently owned
                // locks will be re-acquired before returning.
                //
                // This prevents potential GPU locking violations (e.g., if the
                // caller is holding all the gpu locks but only releases the
                // first of two device locks, then attempting to re-acquire
                // the first device lock will be a locking violation with
                // respect to the second device lock.)
                //
                gpuMask = rmGpuLocksGetOwnedMask();
                rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);
                reAcquire = NV_TRUE;
            }
        }
        else
        {
            reAcquire = osCondReleaseRmSema(pSys->pSema);
        }
    }

    // Actually allocate the memory
    NV_CHECK_OK(status, LEVEL_ERROR, _memdescAllocInternal(pMemDesc));

    if (status != NV_OK)
    {
        pMemDesc->pHeap = NULL;
    }

    if (reAcquire)
    {
        if (osAcquireRmSema(pSys->pSema) != NV_OK)
        {
            DBG_BREAKPOINT();

        }

        if (rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_MASK,
                                  GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_MEM,
                                  &gpuMask) != NV_OK)
        {
            DBG_BREAKPOINT();
        }
        // Releasing the semaphore allows another thread to enter RM and
        // modify broadcast state. We need to set it back (see bug 368643)
        gpumgrSetBcEnabledStatus(pGpu, bcState);
    }

    return status;

subdeviceAlloc_failed:
    _memSubDeviceFreeAndDestroy(pMemDesc);
    pMemDesc->_subDeviceAllocCount = 1;
    pMemDesc->_pNext               = NULL;
    return status;
}

/*!
 *  Allocate memory from one of the possible locations specified in pList.
 *
 *  @param[in,out] pMemDesc      Memory descriptor to allocate
 *  @param[in]     pList         List of NV_ADDRESS_SPACE values. Terminated
 *                               by an ADDR_UNKNOWN entry.
 *
 *  @returns NV_OK on successful allocation. Various NV_ERR_GENERIC codes otherwise.
 */
NV_STATUS
memdescAllocList
(
    MEMORY_DESCRIPTOR *pMemDesc,
    const NV_ADDRESS_SPACE *pList
)
{
    NV_STATUS status = NV_ERR_INVALID_ARGUMENT;
    NvU32 i = 0;

    if (!pList)
    {
        return status;
    }

    //
    // this memdesc may have gotten forced to sysmem if no carveout,
    // but for VPR it needs to be in vidmem, so check and re-direct here
    //
    if (pMemDesc->_flags & MEMDESC_ALLOC_FLAGS_PROTECTED)
    {
        OBJGPU *pGpu = pMemDesc->pGpu;

        // Only force to vidmem if not running with zero-FB.
        if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) ||
            gpuIsCacheOnlyModeEnabled(pGpu))
        {
            pList = ADDRLIST_FBMEM_ONLY;
        }
    }

    while (pList[i] != ADDR_UNKNOWN)
    {
        pMemDesc->_addressSpace = pList[i];
        status = memdescAlloc(pMemDesc);

        if (status == NV_OK)
        {
            return status;
        }

        i++;
    }

    return status;
}

/*!
 *  @brief Lower memdesc free layer. Provides underlying free
 *  functionality.
 *
 *  @param[in,out] pMemDesc Memory descriptor to free
 *
 *  @returns None
 */
static void
_memdescFreeInternal
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    MEM_DESC_DESTROY_CALLBACK *pCb, *pNext;
    NvU64 oldSize;

    // Allow null frees
    if (!pMemDesc)
    {
        return;
    }

    pCb = memdescGetDestroyCallbackList(pMemDesc);

    // Notify all interested parties of destruction
    while (pCb)
    {
        pNext = pCb->pNext;
        pCb->destroyCallback(pMemDesc->pGpu, pCb->pObject, pMemDesc);
        // pCb is now invalid
        pCb = pNext;
    }

    if (memdescHasSubDeviceMemDescs(pMemDesc))
        return;

    memdescPrintMemdesc(pMemDesc, NV_FALSE, MAKE_NV_PRINTF_STR("memdesc being freed"));

    // Bail our early in case this memdesc describes a MODS managed VPR region.
    if (memdescGetFlag(pMemDesc, MEMDESC_FLAGS_VPR_REGION_CLIENT_MANAGED))
        return;

    switch (pMemDesc->_addressSpace)
    {
        case ADDR_SYSMEM:
        case ADDR_EGM:
            // invalidate if memory is cached in FB L2 cache.
            if (pMemDesc->_gpuCacheAttrib == NV_MEMORY_CACHED)
            {
                OBJGPU *pGpu = pMemDesc->pGpu;

                //
                // If this memdesc managed to outlive its pGpu getting detached,
                // we're plenty off the rails already, but avoid using the pGpu
                // and carry on as best we can
                //
                if (gpumgrIsGpuPointerValid(pGpu))
                {
                    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
                    {
                        KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
                        NV_ASSERT_OK(kmemsysCacheOp_HAL(pGpu, pKernelMemorySystem, pMemDesc,
                                                                  FB_CACHE_SYSTEM_MEMORY,
                                                                  FB_CACHE_INVALIDATE));
                    }
                    SLI_LOOP_END
                }
                else
                {
                    NV_ASSERT_FAILED("Sysmemdesc outlived its attached pGpu");
                }
            }

            oldSize             = pMemDesc->Size;
            pMemDesc->Size      = pMemDesc->ActualSize;
            pMemDesc->PageCount = ((pMemDesc->ActualSize + pMemDesc->pageArrayGranularity - 1) >> BIT_IDX_64(pMemDesc->pageArrayGranularity));

            osFreePages(pMemDesc);

            pMemDesc->Size      = oldSize;
            pMemDesc->PageCount = ((oldSize + pMemDesc->pageArrayGranularity - 1) >> BIT_IDX_64(pMemDesc->pageArrayGranularity));

            break;

        case ADDR_FBMEM:
        {
            Heap *pHeap = pMemDesc->pHeap;
            NV_STATUS status = NV_OK;
            OBJGPU *pGpu = pMemDesc->pGpu;

            if (RMCFG_FEATURE_PMA &&
                (pMemDesc->_flags & MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL))
            {
                CTX_BUF_POOL_INFO *pCtxBufPool = memdescGetCtxBufPool(pMemDesc);
                if (pCtxBufPool == NULL)
                {
                    DBG_BREAKPOINT();
                    NV_PRINTF(LEVEL_ERROR, "ctx buf pool not found\n");
                    return;
                }
                NV_STATUS status = ctxBufPoolFree(pCtxBufPool, pMemDesc);
                if (status != NV_OK)
                {
                    DBG_BREAKPOINT();
                    NV_PRINTF(LEVEL_ERROR, "Failed to free memdesc from context buffer pool\n");
                }
            }
            else
            {
                MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

                NV_ASSERT(pHeap != NULL);

                if (!pHeap)
                    return;

                status = memmgrFree(pGpu, pMemoryManager, pHeap, 0x0, 0x0, 0x0,
                                    HEAP_OWNER_RM_CHANNEL_CTX_BUFFER,
                                    pMemDesc);
                NV_ASSERT(status == NV_OK);
            }

            // If this heap is being used to manage PMSA memory, reduce the refcount accordingly
            if ((status == NV_OK) && (pHeap != NULL) &&
                (pHeap->heapType == HEAP_TYPE_PHYS_MEM_SUBALLOCATOR))
            {
                heapRemoveRef(pHeap);
            }
            break;
        }

        default:
            // Don't know how to do any other types of memory yet
            DBG_BREAKPOINT();
    }
}

/*!
 *  @brief Upper memdesc free layer. Provides support for per-subdevice
 *  sysmem buffers and lockless sysmem allocation. Because of SLI and subdevice
 *  submem allocations (refer to submem chart) support, if memory has never
 *  been allocated function will just unlink subdevice structure and destroy
 *  subdevice descriptors.
 *
 *  @param[in,out] pMemDesc Memory descriptor to free
 *
 *  @returns None
 */
void
memdescFree
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    // Allow null frees
    if (!pMemDesc)
    {
        return;
    }


    if (memdescIsSubMemoryMemDesc(pMemDesc))
    {
        NV_ASSERT(!pMemDesc->_pInternalMapping);

        if (pMemDesc->_addressSpace == ADDR_SYSMEM)
        {
            // The memdesc is being freed so destroy all of its IOMMU mappings.
            _memdescFreeIommuMappings(pMemDesc);
        }

        if (pMemDesc->_addressSpace != ADDR_FBMEM &&
            pMemDesc->_addressSpace != ADDR_SYSMEM &&
            pMemDesc->_addressSpace != ADDR_EGM)
        {
            return;
        }

        _memSubDeviceFreeAndDestroy(pMemDesc);
    }
    else
    {
        //
        // In case RM attempts to free memory that has more than 1 refcount, the free is deferred until refcount reaches 0
        //
        // Bug 3307574 RM crashes when client's specify sysmem UserD location.
        // RM attempts to peek at the client allocated UserD when waiting for a channel to go idle.
        //
        if (pMemDesc->RefCount > 1 && pMemDesc->Allocated == 1)
        {
            pMemDesc->bDeferredFree = NV_TRUE;
            return;
        }

        if (!pMemDesc->Allocated)
        {
            return;
        }
        pMemDesc->Allocated--;
        if (0 != pMemDesc->Allocated)
        {
            return;
        }

        // If standbyBuffer memory was allocated then free it
        if (pMemDesc->_pStandbyBuffer)
        {
            memdescFree(pMemDesc->_pStandbyBuffer);
            memdescDestroy(pMemDesc->_pStandbyBuffer);
            pMemDesc->_pStandbyBuffer = NULL;
        }

        NV_ASSERT(!pMemDesc->_pInternalMapping);

        if (pMemDesc->_addressSpace == ADDR_SYSMEM)
        {
            // The memdesc is being freed so destroy all of its IOMMU mappings.
            _memdescFreeIommuMappings(pMemDesc);
        }

        if (pMemDesc->_addressSpace != ADDR_FBMEM &&
            pMemDesc->_addressSpace != ADDR_EGM &&
            pMemDesc->_addressSpace != ADDR_SYSMEM)
        {
            return;
        }

        _memSubDeviceFreeAndDestroy(pMemDesc);

        _memdescFreeInternal(pMemDesc);
    }

    // Reset tracking state
    pMemDesc->_pNext = NULL;
    pMemDesc->_subDeviceAllocCount = 1;

    //
    // Reset tracking state of parent
    // Why it is needed:
    // When a submemory toplevel memdesc with subdevices is freed,
    // the subdecice memdescs and their parent are destroyed or their
    // refcount decreased.
    // When the parent subdevice descriptors are destroyed, their
    // top level descriptor is left alone and has a dangling
    // _pNext pointer
    //
    if ((pMemDesc->_pParentDescriptor != NULL) &&
        (memdescHasSubDeviceMemDescs(pMemDesc->_pParentDescriptor)) &&
        (pMemDesc->_pParentDescriptor->RefCount == 1))
    {
        pMemDesc->_pParentDescriptor->_pNext = NULL;
        pMemDesc->_pParentDescriptor->_subDeviceAllocCount = 1;
    }
}

/*!
 *  @brief Lock the paged virtual memory descripted by the memory descriptor
 *
 *  @param[in] pMemDesc Memory descriptor to lock
 *
 *  @returns NV_OK on successful allocation. Various NV_ERR_GENERIC codes otherwise.
 */
NV_STATUS
memdescLock
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{

    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    if (!(pMemDesc->_flags & MEMDESC_FLAGS_PAGED_SYSMEM))
    {
        return NV_ERR_ILLEGAL_ACTION;
    }

    return osLockMem(pMemDesc);
}

/*!
 *  @brief Unlock the paged virtual memory descripted by the memory descriptor
 *
 *  @param[in] pMemDesc Memory descriptor to unlock
 *
 *  @returns NV_OK on successful allocation. Various NV_ERR_GENERIC codes otherwise.
 */
NV_STATUS
memdescUnlock
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    if (!(pMemDesc->_flags & MEMDESC_FLAGS_PAGED_SYSMEM))
    {
        return NV_ERR_ILLEGAL_ACTION;
    }

    return osUnlockMem(pMemDesc);
}

/*!
 *  @brief Get a CPU mapping to the memory described by a memory descriptor
 *
 *  This is for memory descriptors used by RM clients, not by the RM itself.
 *  For internal mappings the busMapRmAperture() hal routines are used.
 *
 *  @param[in]  pMemDesc    Memory descriptor to map
 *  @param[in]  Offset      Offset into memory descriptor to start map
 *  @param[in]  Size        Size of mapping
 *  @param[in]  Kernel      Kernel or user address space
 *  @param[in]  Protect     NV_PROTECT_*
 *  @param[out] pAddress    Return address
 *  @param[out] pPriv       Return cookie to be passed back to memdescUnmap
 *
 *  @returns NV_STATUS
 */

NV_STATUS
memdescMapOld
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU64 Offset,
    NvU64 Size,
    NvBool Kernel,
    NvU32 Protect,
    void **pAddress,
    void **pPriv
)
{
    NvP64 pAddressP64 = NV_PTR_TO_NvP64(*pAddress);
    NvP64 pPrivP64 = NV_PTR_TO_NvP64(*pPriv);
    NV_STATUS status;

#if !defined(NV_64_BITS)
    NV_ASSERT(Kernel);
#endif

    status = memdescMap(pMemDesc,
                        Offset,
                        Size,
                        Kernel,
                        Protect,
                        &pAddressP64,
                        &pPrivP64);

    *pAddress = NvP64_VALUE(pAddressP64);
    *pPriv = NvP64_VALUE(pPrivP64);

    return status;
}

NV_STATUS
memdescMap
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU64 Offset,
    NvU64 Size,
    NvBool Kernel,
    NvU32 Protect,
    NvP64 *pAddress,
    NvP64 *pPriv
)
{
    NV_STATUS status     = NV_OK;
    NvU64     rootOffset = 0;

    NV_ASSERT_OR_RETURN(((Offset + Size) <= memdescGetSize(pMemDesc)), NV_ERR_INVALID_ARGUMENT);

    pMemDesc = memdescGetRootMemDesc(pMemDesc, &rootOffset);
    Offset  += rootOffset;

    if (pMemDesc->PteAdjust &&
        (pMemDesc->Alignment > RM_PAGE_SIZE) &&
        (pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS) &&
        RMCFG_FEATURE_PLATFORM_MODS)
    {
        Offset += pMemDesc->PteAdjust;
    }

    NV_ASSERT_OR_RETURN(!memdescHasSubDeviceMemDescs(pMemDesc), NV_ERR_INVALID_OBJECT_BUFFER);

    switch (pMemDesc->_addressSpace)
    {
        case ADDR_SYSMEM:
        case ADDR_EGM:
        {
            status = osMapSystemMemory(pMemDesc, Offset, Size,
                                       Kernel, Protect, pAddress, pPriv);
            if (status != NV_OK)
            {
                return status;
            }
            break;
        }

        case ADDR_FBMEM:
        {
            OBJGPU          *pGpu                   = pMemDesc->pGpu;
            NvU32            mode                   = NV_MEMORY_WRITECOMBINED;
            KernelBus       *pKernelBus;
            FB_MAPPING_INFO *pMapping;
            RmPhysAddr       bar1PhysAddr;
            NvBool           bCoherentCpuMapping;

            NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);

            pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
            bCoherentCpuMapping = pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING);

            // Need struct to keep track of the info for this mapping
            pMapping = portMemAllocNonPaged(sizeof(FB_MAPPING_INFO));
            if (pMapping == NULL)
            {
                return NV_ERR_NO_MEMORY;
            }

            if (bCoherentCpuMapping)
            {
                if (Kernel)
                {
                    status = kbusMapCoherentCpuMapping_HAL(pGpu,
                                                           pKernelBus,
                                                           pMemDesc,
                                                           Offset,
                                                           Size,
                                                           Protect,
                                                           pAddress,
                                                           &pMapping->pPriv);
                }
                else
                {
                    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
                    NvU64              fbOffset             = pMemDesc->_pteArray[0] +
                                                              pMemDesc->PteAdjust + Offset;
                    bar1PhysAddr = pKernelMemorySystem->coherentCpuFbBase + fbOffset;
                    mode = NV_MEMORY_CACHED;

                    status = osMapPciMemoryUser(pGpu->pOsGpuInfo, bar1PhysAddr,
                                                Size, Protect, pAddress,
                                                &pMapping->pPriv,
                                                mode);
                }


                if (status != NV_OK)
                {
                    portMemFree(pMapping);
                    return status;
                }

                NV_PRINTF(LEVEL_INFO, "Allocating coherent link mapping. VA: %p PA: 0x%llx size: 0x%llx\n",
                          NvP64_VALUE(*pAddress),
                          memdescGetPhysAddr(pMemDesc, AT_GPU, Offset), Size);

                *pPriv = NV_PTR_TO_NvP64(pMapping);
                break;
            }

            // Determine where in BAR1 the mapping will go
            pMapping->FbApertureLen = Size;
            status = kbusMapFbApertureSingle(pGpu, pKernelBus,
                                             pMemDesc, Offset,
                                             &pMapping->FbAperture,
                                             &pMapping->FbApertureLen,
                                             BUS_MAP_FB_FLAGS_MAP_UNICAST,
                                             NULL);
            if (status != NV_OK)
            {
                portMemFree(pMapping);
                return status;
            }

            bar1PhysAddr = gpumgrGetGpuPhysFbAddr(pGpu) + pMapping->FbAperture;
            mode = NV_MEMORY_WRITECOMBINED;

            // Create the mapping
            if (Kernel)
            {
                status = osMapPciMemoryKernel64(pGpu, bar1PhysAddr,
                                                Size, Protect, pAddress,
                                                mode);
            }
            else
            {
                status = osMapPciMemoryUser(pGpu->pOsGpuInfo, bar1PhysAddr,
                                            Size, Protect, pAddress,
                                            &pMapping->pPriv,
                                            mode);
            }

            if (status != NV_OK)
            {
                if (!bCoherentCpuMapping)
                {
                    kbusUnmapFbApertureSingle(pGpu, pKernelBus, pMemDesc,
                                              pMapping->FbAperture,
                                              pMapping->FbApertureLen,
                                              BUS_MAP_FB_FLAGS_MAP_UNICAST);
                }
                portMemFree(pMapping);
                return status;
            }

            *pPriv = NV_PTR_TO_NvP64(pMapping);
            break;
        }

        default:
            // Don't know how to do any other types of memory yet
            DBG_BREAKPOINT();
            return NV_ERR_GENERIC;
    }
    return NV_OK;
}
void
memdescUnmapOld
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvBool Kernel,
    NvU32 ProcessId,
    void *Address,
    void *Priv
)
{
    memdescUnmap(pMemDesc,
                 Kernel,
                 ProcessId,
                 NV_PTR_TO_NvP64(Address),
                 NV_PTR_TO_NvP64(Priv));
}

/*!
 *  @brief Remove a mapping for the memory descriptor, reversing memdescMap
 *
 *  @param[in]   pMemDesc       Memory descriptor to unmap
 *  @param[in]   Kernel         Kernel or user address space
 *  @param[in]   ProcessId      Process ID if user space
 *  @param[in]   Address        Mapped address
 *  @param[in]   Priv           Return priv cookie from memdescMap
 *
 *  @returns None
 */
void
memdescUnmap
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvBool Kernel,
    NvU32 ProcessId,
    NvP64 Address,
    NvP64 Priv
)
{
    // Allow null unmaps
    if (!Address)
        return;

    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));


    // find first allocated parent descriptor
    while (!pMemDesc->Allocated && pMemDesc->_pParentDescriptor)
    {
        pMemDesc = pMemDesc->_pParentDescriptor;
    }

    switch (pMemDesc->_addressSpace)
    {
        case ADDR_SYSMEM:
        case ADDR_EGM:
        {
            osUnmapSystemMemory(pMemDesc, Kernel, ProcessId, Address, Priv);
            break;
        }

        case ADDR_FBMEM:
        {
            OBJGPU          *pGpu                = pMemDesc->pGpu;
            KernelBus       *pKernelBus          = GPU_GET_KERNEL_BUS(pGpu);
            FB_MAPPING_INFO *pMapping            = (FB_MAPPING_INFO *)NvP64_VALUE(Priv);
            NvBool           bCoherentCpuMapping = pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING);
            NvU64            Size                = pMapping->FbApertureLen;

            NV_ASSERT(!(pMemDesc->_flags & MEMDESC_FLAGS_CPU_ONLY));

            if (bCoherentCpuMapping)
            {
                if (Kernel)
                {
                    kbusUnmapCoherentCpuMapping_HAL(pGpu, pKernelBus, pMemDesc, Address, pMapping->pPriv);
                }
                else
                {
                    osUnmapPciMemoryUser(pGpu->pOsGpuInfo, Address, Size,
                                         pMapping->pPriv);
                }

                portMemFree(pMapping);
                break;
            }

            kbusUnmapFbApertureSingle(pGpu, pKernelBus, pMemDesc,
                                      pMapping->FbAperture,
                                      Size,
                                      BUS_MAP_FB_FLAGS_MAP_UNICAST);
            if (Kernel)
            {
                osUnmapPciMemoryKernel64(pGpu, Address);
            }
            else
            {
                osUnmapPciMemoryUser(pGpu->pOsGpuInfo, Address, Size,
                                     pMapping->pPriv);
            }

            portMemFree(pMapping);
            break;
        }

        default:
            // Don't know how to do any other types of memory yet
            DBG_BREAKPOINT();
    }
}

typedef enum
{
    MEMDESC_MAP_INTERNAL_TYPE_GSP,            // On GSP, use a pre-existing mapping
    MEMDESC_MAP_INTERNAL_TYPE_COHERENT_FBMEM, // For NVLINK, use a pre-existing mapping for fbmem
    MEMDESC_MAP_INTERNAL_TYPE_BAR2,           // Use BAR2 (fbmem or reflected sysmem)
    MEMDESC_MAP_INTERNAL_TYPE_SYSMEM_DIRECT,  // Use OS to map sysmem
} MEMDESC_MAP_INTERNAL_TYPE;

static MEMDESC_MAP_INTERNAL_TYPE
memdescGetMapInternalType
(
    OBJGPU            *pGpu,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        return MEMDESC_MAP_INTERNAL_TYPE_GSP;
    }
    else if (pMemDesc->_addressSpace == ADDR_FBMEM &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
    {
        // Temporary hack to keep the same behavior on GV100F (dVOLTA & DFPGA)
        if (IsDFPGA(pGpu))
            return MEMDESC_MAP_INTERNAL_TYPE_BAR2;

        return MEMDESC_MAP_INTERNAL_TYPE_COHERENT_FBMEM;
    }
    else
    {
        KernelBus *pKernelBus      = GPU_GET_KERNEL_BUS(pGpu);
        NvBool     bUseDirectMap   = NV_FALSE;
        NV_STATUS  status;

        status = kbusUseDirectSysmemMap_HAL(pGpu, pKernelBus, pMemDesc, &bUseDirectMap);
        NV_ASSERT_OR_RETURN(status == NV_OK, MEMDESC_MAP_INTERNAL_TYPE_BAR2);

        return bUseDirectMap ? MEMDESC_MAP_INTERNAL_TYPE_SYSMEM_DIRECT : MEMDESC_MAP_INTERNAL_TYPE_BAR2;
    }

    return MEMDESC_MAP_INTERNAL_TYPE_SYSMEM_DIRECT;
}

void
memdescFlushGpuCaches
(
    OBJGPU            *pGpu,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

    if (memdescGetGpuCacheAttrib(pMemDesc) == NV_MEMORY_CACHED)
    {
        //
        // Only the non-coherent memory path is available, so writeback GPU L2
        // invalidate the GPU L2
        //
        kmemsysCacheOp_HAL(pGpu, pKernelMemorySystem, pMemDesc, FB_CACHE_MEM_UNDEFINED, FB_CACHE_EVICT);
    }
}

void
memdescFlushCpuCaches
(
    OBJGPU            *pGpu,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    // Flush WC to get the data written to this mapping out to memory
    osFlushCpuWriteCombineBuffer();

    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    // Special care is needed on SOC, where the GPU cannot snoop the CPU L2
    if ((pKernelBif != NULL)                     &&
        !kbifIsSnoopDmaCapable(pGpu, pKernelBif) &&
        (memdescGetCpuCacheAttrib(pMemDesc) == NV_MEMORY_CACHED))
    {
        // Flush CPU L2 so that the GPU will see any changes the CPU made
        osFlushCpuCache();
    }
}

/*
 * @brief map memory descriptor for internal access
 *
 * flags - subset of TRANSFER_FLAGS_
 */
void*
memdescMapInternal
(
    OBJGPU            *pGpu,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32              flags
)
{
    MEMDESC_MAP_INTERNAL_TYPE  mapType;
    NV_STATUS                  status;

    NV_ASSERT_OR_RETURN(pMemDesc != NULL, NULL);

    if (pMemDesc->_addressSpace == ADDR_FBMEM)
    {
        pMemDesc = memdescGetMemDescFromGpu(pMemDesc, pGpu);
    }

    mapType = memdescGetMapInternalType(pGpu, pMemDesc);

    // We need to flush & invalidate GPU L2 cache only for directed BAR mappings.
    // Reflected BAR mappings will access memory via GPU, and hence go through GPU L2 cache.
    if (mapType == MEMDESC_MAP_INTERNAL_TYPE_SYSMEM_DIRECT)
        memdescFlushGpuCaches(pGpu, pMemDesc);

    if (pMemDesc->_pInternalMapping != NULL)
    {
        NV_ASSERT(pMemDesc->_internalMappingRefCount);

        // Existing BAR2 mapping may be invalid due to GPU reset
        if (mapType == MEMDESC_MAP_INTERNAL_TYPE_BAR2)
        {
            pMemDesc->_pInternalMapping = kbusValidateBar2ApertureMapping_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), pMemDesc,
                                                                              pMemDesc->_pInternalMapping);
            NV_CHECK_OR_RETURN(LEVEL_ERROR, pMemDesc->_pInternalMapping != NULL, NULL);
        }

        pMemDesc->_internalMappingRefCount++;
        return pMemDesc->_pInternalMapping;
    }

    switch (mapType)
    {
        case MEMDESC_MAP_INTERNAL_TYPE_GSP:
            NV_CHECK_OR_RETURN(LEVEL_ERROR, pMemDesc->_pInternalMapping != NULL, NULL);
            break;
        case MEMDESC_MAP_INTERNAL_TYPE_SYSMEM_DIRECT:
        {
            status = memdescMapOld(pMemDesc, 0, pMemDesc->Size, NV_TRUE, NV_PROTECT_READ_WRITE,
                                   &pMemDesc->_pInternalMapping, &pMemDesc->_pInternalMappingPriv);
            NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, NULL);
            break;
        }
        case MEMDESC_MAP_INTERNAL_TYPE_COHERENT_FBMEM:
        {
            NV_ASSERT(pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED));
            status = kbusMapCoherentCpuMapping_HAL(pGpu,
                                                   GPU_GET_KERNEL_BUS(pGpu),
                                                   pMemDesc,
                                                   0,
                                                   memdescGetSize(pMemDesc),
                                                   NV_PROTECT_READ_WRITE,
                                                   &pMemDesc->_pInternalMapping,
                                                   &pMemDesc->_pInternalMappingPriv);
            NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, NULL);
            break;
        }
        case MEMDESC_MAP_INTERNAL_TYPE_BAR2:
            pMemDesc->_pInternalMapping = kbusMapBar2Aperture_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), pMemDesc, flags);
            NV_CHECK_OR_RETURN(LEVEL_INFO, pMemDesc->_pInternalMapping != NULL, NULL);
            break;

        default:
            DBG_BREAKPOINT();
    }

    pMemDesc->_internalMappingRefCount = 1;
    return pMemDesc->_pInternalMapping;
}

void memdescUnmapInternal
(
    OBJGPU            *pGpu,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32              flags
)
{
    MEMDESC_MAP_INTERNAL_TYPE  mapType;

    NV_ASSERT_OR_RETURN_VOID(pMemDesc != NULL);
    NV_ASSERT_OR_RETURN_VOID(pMemDesc->_pInternalMapping != NULL && pMemDesc->_internalMappingRefCount != 0);

    if (pMemDesc->_addressSpace == ADDR_FBMEM)
    {
        pMemDesc = memdescGetMemDescFromGpu(pMemDesc, pGpu);
    }

    mapType = memdescGetMapInternalType(pGpu, pMemDesc);

    if (mapType == MEMDESC_MAP_INTERNAL_TYPE_SYSMEM_DIRECT || mapType == MEMDESC_MAP_INTERNAL_TYPE_BAR2)
    {
        memdescFlushCpuCaches(pGpu, pMemDesc);
    }

    if (--pMemDesc->_internalMappingRefCount == 0)
    {
        switch (mapType)
        {
            case MEMDESC_MAP_INTERNAL_TYPE_GSP:
                break;
            case MEMDESC_MAP_INTERNAL_TYPE_SYSMEM_DIRECT:
                memdescUnmapOld(pMemDesc, NV_TRUE, 0,
                                pMemDesc->_pInternalMapping, pMemDesc->_pInternalMappingPriv);
                break;

            case MEMDESC_MAP_INTERNAL_TYPE_COHERENT_FBMEM:
            {
                kbusUnmapCoherentCpuMapping_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), pMemDesc,
                                                pMemDesc->_pInternalMapping, pMemDesc->_pInternalMappingPriv);
                break;
            }
            case MEMDESC_MAP_INTERNAL_TYPE_BAR2:
            {
                NvU8 *p = (NvU8 *)pMemDesc->_pInternalMapping;

                kbusUnmapBar2ApertureWithFlags_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), pMemDesc, &p, flags);
                break;
            }

            default:
                DBG_BREAKPOINT();
        }

        pMemDesc->_pInternalMapping        = NULL;
        pMemDesc->_pInternalMappingPriv    = NULL;
        pMemDesc->_internalMappingRefCount = 0;
    }

    // Flush for direct mappings too to keep the behavior
    if (((flags & TRANSFER_FLAGS_DEFER_FLUSH) == 0) &&
        (mapType == MEMDESC_MAP_INTERNAL_TYPE_SYSMEM_DIRECT || mapType == MEMDESC_MAP_INTERNAL_TYPE_BAR2))
    {
        kbusFlush_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu),
                      kbusGetFlushAperture(GPU_GET_KERNEL_BUS(pGpu), memdescGetAddressSpace(pMemDesc)));
    }
}

/*!
 *  Describe an existing region of memory in a memory descriptor
 *
 *  Memory must be physically contiguous.
 *
 *  The memory descriptor must be initialized with
 *  memdescCreate*(), typically memdescCreateExisting()
 *  prior to calling memdescDescribe.
 *
 *  memdescDescribe() now only updates the fields needed in the call.
 *
 *  @param[out]  pMemDesc       Memory descriptor to fill
 *  @param[in]   AddressSpace   Address space of memory
 *  @param[in]   Base           Physical address of region
 *  @param[in]   Size           Size of region
 *
 *  @returns None
 */
void
memdescDescribe
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NV_ADDRESS_SPACE AddressSpace,
    RmPhysAddr Base,
    NvU64 Size
)
{
    // Some sanity checks to see if we went through MemCreate*() first
    NV_ASSERT((pMemDesc->RefCount == 1) &&
              (memdescGetDestroyCallbackList(pMemDesc) == NULL) &&
              (pMemDesc->PteAdjust == 0));

    NV_ASSERT(pMemDesc->_pIommuMappings == NULL);
    NV_ASSERT(pMemDesc->Allocated == 0);
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));

    //
    // Check if the base address accounts for the DMA window start address
    // (always in the high, unaddressable bits of the address) and add it
    // if necessary. On most platforms, the DMA window start address will
    // simply be 0.
    //
    // This is most likely to happen in cases where the Base address is
    // read directly from a register or MMU entry, which does not already
    // account for the DMA window.
    //
    if (pMemDesc->pGpu == NULL)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "unable to check Base 0x%016llx for DMA window\n", Base);
    }
    else if (AddressSpace == ADDR_SYSMEM)
    {
        OBJGPU *pGpu = pMemDesc->pGpu;
        if (pGpu)
        {
            KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
            NvU32 physAddrWidth = gpuGetPhysAddrWidth_HAL(pGpu, ADDR_SYSMEM);
            if ((Base & ~(NVBIT64(physAddrWidth) - 1)) == 0)
            {
                Base += pKernelBif->dmaWindowStartAddress;
            }
        }
    }

    if (pMemDesc->Alignment != 0)
    {
        NV_ASSERT(NV_FLOOR_TO_QUANTA(Base, pMemDesc->Alignment) == Base);
    }

    pMemDesc->Size                 = Size;
    pMemDesc->ActualSize           = NV_ROUNDUP(Size, pMemDesc->pageArrayGranularity);
    pMemDesc->_flags              |= MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS;
    pMemDesc->_addressSpace        = AddressSpace;
    pMemDesc->_pteArray[0]         = Base & ~RM_PAGE_MASK;
    pMemDesc->_subDeviceAllocCount = 1;
    pMemDesc->PteAdjust            = NvU64_LO32(Base) & RM_PAGE_MASK;
    pMemDesc->PageCount            = ((Size + pMemDesc->PteAdjust + RM_PAGE_SIZE - 1) >> RM_PAGE_SHIFT);
    pMemDesc->_pParentDescriptor   = NULL;
    pMemDesc->childDescriptorCnt   = 0;
}

/*!
 * Static helper called from memdescFillPages.
 * When dynamic granularity memdescs are enabled. We only need to copy over the pages
 * without worrying about converting them to 4K.
 *
 *  @param[in]   pMemDesc       Memory descriptor to fill
 *  @param[in]   pageIndex      Index into memory descriptor to fill from
 *  @param[in]   pPages         Array of physical addresses
 *  @param[in]   pageCount      Number of entries in pPages
 *  @param[in]   pageSize       Size of each page in pPages
 *
 *  @returns None
 */
static void
_memdescFillPagesAtNativeGranularity
(
    MEMORY_DESCRIPTOR   *pMemDesc,
    NvU32                pageIndex,
    NvU64               *pPages,
    NvU32                pageCount,
    NvU64                pageSize
)
{
    NV_STATUS status;

    NV_ASSERT(pageIndex + pageCount < pMemDesc->PageCount);

    status = memdescSetPageArrayGranularity(pMemDesc, pageSize);
    if (status != NV_OK)
    {
        return;
    }

    for (NvU32 i = 0; i < pageCount; i++)
    {
        pMemDesc->_pteArray[pageIndex + i] = pPages[i];
    }

    pMemDesc->ActualSize = pageCount * pageSize;
}

/*!
 * Fill the PTE array of a memory descriptor with an array of addresses
 * returned by pmaAllocatePages().
 *
 *  Memory must be physically discontiguous. For the contiguous case
 *  memdescDescribe() is more apt.
 *
 *  The memory descriptor must be initialized with memdescCreate*(),
 *  typically memdescCreateExisting() prior to calling
 *  memdescFillPages().
 *
 *  @param[in]   pMemDesc       Memory descriptor to fill
 *  @param[in]   pageIndex      Index into memory descriptor to fill from
 *  @param[in]   pPages         Array of physical addresses
 *  @param[in]   pageCount      Number of entries in pPages
 *  @param[in]   pageSize       Size of each page in pPages
 *
 *  @returns None
 */
void
memdescFillPages
(
    MEMORY_DESCRIPTOR   *pMemDesc,
    NvU32                pageIndex,
    NvU64               *pPages,
    NvU32                pageCount,
    NvU64                pageSize
)
{
    OBJGPU *pGpu = gpumgrGetSomeGpu();
    NvU32 i, j, k;
    NvU32 numChunks4k = pageSize / RM_PAGE_SIZE;
    NvU32 offset4k = numChunks4k * pageIndex;
    NvU32 pageCount4k = numChunks4k * pageCount;
    NvU32 result4k, limit4k;
    NvU64 addr;

    NV_ASSERT(pMemDesc != NULL);

    if (GPU_GET_MEMORY_MANAGER(pGpu)->bEnableDynamicGranularityPageArrays)
    {
        _memdescFillPagesAtNativeGranularity(pMemDesc, pageIndex, pPages, pageCount, pageSize);
        return;
    }

    NV_ASSERT(offset4k < pMemDesc->PageCount);
    NV_ASSERT(portSafeAddU32(offset4k, pageCount4k, &result4k));

    //
    // There is a possibility that the pMemDesc was created using 4K aligned
    // allocSize, but the actual memory allocator could align up the allocation
    // size based on its supported pageSize, (e.g. PMA supports 64K pages). In
    // that case, pageCount4k would be greater than pMemdesc->pageCount. We
    // limit pageCount4k to stay within pMemdesc->pageCount in that case.
    //
    if (result4k > pMemDesc->PageCount)
        pageCount4k = pMemDesc->PageCount - offset4k;

    NV_ASSERT(pageSize > 0);
    NV_ASSERT(0 == (pageSize & (RM_PAGE_SIZE - 1)));
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));

    // Fill _pteArray array using numChunks4k as a stride
    for (i = 0, j = offset4k; i < pageCount; i++, j += numChunks4k)
    {
        pMemDesc->_pteArray[j] = addr = pPages[i];

        // Fill _pteArray at 4K granularity
        limit4k = NV_MIN(j + numChunks4k, pageCount4k);

        addr += RM_PAGE_SIZE;
        for (k = j + 1; k < limit4k; k++, addr += RM_PAGE_SIZE)
            pMemDesc->_pteArray[k] = addr;
    }
}

/*!
 *  @brief Acquire exclusive use for memdesc for RM.
 *
 *  @param[inout] pMemDesc Memory descriptor
 *
 *  @returns Boolean indicating whether we successfully acquired the memdesc for exclusive use
 */
NvBool
memdescAcquireRmExclusiveUse
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pMemDesc->_pParentDescriptor == NULL &&
                                    !pMemDesc->bRmExclusiveUse &&
                                    pMemDesc->DupCount == 1,
                       NV_FALSE);

    pMemDesc->bRmExclusiveUse = NV_TRUE;
    return NV_TRUE;
}

//
// SubMemory per subdevice chart: (MD - Memory Descriptor, SD - subdevice)
//
// If we try to create submemory of descriptor which has subdevices:
//
//   [Top level MD]
//       ^      |
//       |      +--------> [     Subdevice 0 MD    ] --------> [Subdevice 1 MD]
//       |                       ^                               ^
//       |                       |                               |
//  [SubMemory top level MD]     |                               |
//             |                 |                               |
//             +--------> [Subdevice 0 SubMemory MD] --------> [Subdevice 1 SubMemory MD]
//
// Top Level MD             : parent of SubMemoryTopLevelMD; has subdescriptors
//                            for two subdevices
// SubMemory top level MD   : has pointer to parent memory descriptor; has two
//                            subdevice MDs
// Subdevice 0 MD           : subdevice MD of topLevelMD and parent of SD0
//                            submemory descriptor; has pointer to next in the
//                            list of subdevice MDs
// Subdevice 0 SubMemory MD : submemory of subdevice 0 MD; has pointer to
//                            parent, subdevice 0 MD and to next in list of
//                            submemory subdevice memory descriptors
//



/*!
 *  @brief Create a new memory descriptor that is a subset of pMemDesc. If
 *  pMemDesc has subdevice memory descriptors subMemory will be created for all
 *  subdevices and new memory descriptor will be top level for them (ASCII art)
 *
 *  @param[out]  ppMemDescNew   New memory descriptor
 *  @param[in]   pMemDesc       Original memory descriptor
 *  @param[in]   pGpu           The GPU that this memory will be mapped to
 *  @param[in]   Offset         Sub memory descriptor starts at pMemdesc+Offset
 *  @param[in]   Size           For Size bytes
 *
 *  @returns None
 */
NV_STATUS
memdescCreateSubMem
(
    MEMORY_DESCRIPTOR **ppMemDescNew,
    MEMORY_DESCRIPTOR *pMemDesc,
    OBJGPU *pGpu,
    NvU64 Offset,
    NvU64 Size
)
{
    NV_STATUS status;
    MEMORY_DESCRIPTOR *pMemDescNew;
    NvU32 subDevInst;
    NvU64 tmpSize = Size;
    MEMORY_DESCRIPTOR *pLast;
    MEMORY_DESCRIPTOR *pNew;
    OBJGPU *pGpuChild;
    const NvU64 pageArrayGranularity = pMemDesc->pageArrayGranularity;
    const NvU64 pageArrayGranularityMask = pMemDesc->pageArrayGranularity - 1;
    const NvU32 pageArrayGranularityShift = BIT_IDX_64(pMemDesc->pageArrayGranularity);

    // Default to the original memdesc's GPU if none is specified
    if (pGpu == NULL)
    {
        pGpu = pMemDesc->pGpu;
    }

    // Allocation size should be adjusted for the memory descriptor _pageSize.
    // Also note that the first 4k page may not be at _pageSize boundary so at
    // the time of the mapping, we maybe overmapping at the beginning or end of
    // the descriptor. To fix it in the right way, memory descriptor needs to
    // be further cleaned. Do not round to page size if client specifies so.
    if (!(pMemDesc->_flags & MEMDESC_FLAGS_PAGE_SIZE_ALIGN_IGNORE) &&
          pMemDesc->_pageSize != 0)
    {
        PMEMORY_DESCRIPTOR pTempMemDesc = pMemDesc;
        NvU64              pageOffset;

        if (memdescHasSubDeviceMemDescs(pMemDesc))
        {
            NV_ASSERT(pGpu);
            pTempMemDesc = memdescGetMemDescFromGpu(pMemDesc, pGpu);
        }

        pageOffset = memdescGetPhysAddr(pTempMemDesc, AT_CPU, Offset) &
                     (pTempMemDesc->_pageSize - 1);

        // Check for integer overflow
        if (!portSafeAddU64(pageOffset, Size, &tmpSize))
        {
            return NV_ERR_INVALID_ARGUMENT;
        }

        tmpSize = RM_ALIGN_UP(pageOffset + Size, pTempMemDesc->_pageSize);

        // Check for integer overflow
        if (tmpSize < pageOffset + Size)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    // Allocate the new MEMORY_DESCRIPTOR
    status = memdescCreate(&pMemDescNew, pGpu, tmpSize, 0,
                           !!(pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS),
                           pMemDesc->_addressSpace,
                           pMemDesc->_cpuCacheAttrib,
                           ((pMemDesc->_flags & ~MEMDESC_FLAGS_PRE_ALLOCATED) | MEMDESC_FLAGS_SKIP_RESOURCE_COMPUTE));

    if (status != NV_OK)
    {
        return status;
    }

    // Fill in various fields as best we can; XXX this can get sort of sketchy
    // in places, which should be all the more motivation to rip some of these
    // fields out of the MEMORY_DESCRIPTOR.
    if (pMemDesc->_flags & MEMDESC_FLAGS_KERNEL_MODE)
        pMemDescNew->_flags |= MEMDESC_FLAGS_KERNEL_MODE;
    else
        pMemDescNew->_flags &= ~MEMDESC_FLAGS_KERNEL_MODE;

    //
    // This flag used to indicate the memdesc needs to restore pte kind
    // when it is freed. It should only not be set for any sub memDesc.
    //
    pMemDescNew->_flags &= ~MEMDESC_FLAGS_RESTORE_PTE_KIND_ON_FREE;

    pMemDescNew->Size = Size;
    pMemDescNew->_pteKind = pMemDesc->_pteKind;
    pMemDescNew->_hwResId = pMemDesc->_hwResId;
    if (pMemDesc->_flags & MEMDESC_FLAGS_ENCRYPTED)
        pMemDescNew->_flags |= MEMDESC_FLAGS_ENCRYPTED;
    else
        pMemDescNew->_flags &= ~MEMDESC_FLAGS_ENCRYPTED;
    pMemDescNew->_pageSize   = pMemDesc->_pageSize;
    pMemDescNew->pageArrayGranularity = pageArrayGranularity;
    pMemDescNew->_gpuCacheAttrib = pMemDesc->_gpuCacheAttrib;
    pMemDescNew->_gpuP2PCacheAttrib  = pMemDesc->_gpuP2PCacheAttrib;
    pMemDescNew->gfid                = pMemDesc->gfid;
    pMemDescNew->bUsingSuballocator  = pMemDesc->bUsingSuballocator;
    pMemDescNew->_pParentDescriptor  = pMemDesc;
    pMemDesc->childDescriptorCnt++;
    pMemDescNew->bRmExclusiveUse = pMemDesc->bRmExclusiveUse;
    pMemDescNew->numaNode        = pMemDesc->numaNode;

    pMemDescNew->subMemOffset        = Offset;

    // increase refCount of parent descriptor
    memdescAddRef(pMemDesc);

    // Fill in the PteArray and PteAdjust
    if ((pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS) ||
        (pMemDesc->PageCount == 1))
    {
        // Compute the base address, then fill it in
        RmPhysAddr Base = pMemDesc->_pteArray[0] + pMemDesc->PteAdjust + Offset;
        pMemDescNew->_pteArray[0] = Base & ~pageArrayGranularityMask;
        pMemDescNew->PteAdjust   = NvU64_LO32(Base) & pageArrayGranularityMask;

        if (memdescIsEgm(pMemDesc))
        {
            NV_ASSERT_OK_OR_GOTO(status,
                                 _memdescAllocEgmArray(pMemDescNew),
                                 fail);
        }
    }
    else
    {
        // More complicated...
        RmPhysAddr Adjust;
        NvU32 PageIndex, i;

        // We start this many bytes into the memory alloc
        Adjust = pMemDesc->PteAdjust + Offset;

        // Break it down into pages (PageIndex) and bytes (PteAdjust)
        PageIndex = (NvU32)(Adjust >> pageArrayGranularityShift);
        pMemDescNew->PteAdjust = NvU64_LO32(Adjust) & pageArrayGranularityMask;

        // Fill in the PTEs; remember to copy the extra PTE, in case we need it
        if (pMemDesc->PageCount)
        {
            for (i = 0; i < pMemDescNew->PageCount+1; i++)
            {
                NvU32 j = i + PageIndex;
                if (j < pMemDesc->PageCount)
                {
                    pMemDescNew->_pteArray[i] = pMemDesc->_pteArray[j];
                }
                else
                {
                    //
                    // This case can happen with page size greater than 4KB.
                    // Since pages are always tracked at 4KB granularity the
                    // subset description may overflow the parent memdesc.
                    //
                    // In this case the best we can do is describe the contiguous
                    // memory after the last 4KB page in the sub-memdesc.
                    //
                    // TODO: Tracking memdesc pages at native page size would
                    //       remove the need for several hacks, including this one.
                    //
                    NV_ASSERT(i > 0);
                    pMemDescNew->_pteArray[i] = pMemDescNew->_pteArray[i - 1] + pMemDescNew->pageArrayGranularity;
                }
            }

            if (memdescIsEgm(pMemDesc))
            {
                NV_ASSERT_OK_OR_GOTO(status,
                                     _memdescAllocEgmArray(pMemDescNew),
                                     fail);
            }
        }
    }

    if ((pMemDesc->_addressSpace == ADDR_SYSMEM) &&
        !memdescIsEgm(pMemDesc) &&
        !memdescGetFlag(memdescGetMemDescFromGpu(pMemDesc, pGpu), MEMDESC_FLAGS_CPU_ONLY) &&
        !memdescGetFlag(memdescGetMemDescFromGpu(pMemDesc, pGpu), MEMDESC_FLAGS_MAP_SYSCOH_OVER_BAR1) &&
        !memdescGetFlag(memdescGetMemDescFromGpu(pMemDesc, pGpu), MEMDESC_FLAGS_SKIP_IOMMU_MAPPING))
    {
        //
        // For different IOVA spaces, the IOMMU mapping will often not be a
        // subrange of the original mapping.
        //
        // Request the submapping to be associated with the submemdesc.
        //
        // TODO: merge the new IOMMU paths with the SMMU path above (see bug
        //       1625121).
        //
        status = memdescMapIommu(pMemDescNew, pGpu->busInfo.iovaspaceId);
        if (status != NV_OK)
        {
            memdescDestroy(pMemDescNew);
            return status;
        }
    }

    // Support for SLI submemory per-subdevice allocations (refer to chart)
    if (memdescHasSubDeviceMemDescs(pMemDesc))
    {
        NvBool bBcState = gpumgrGetBcEnabledStatus(pGpu);

        if (gpumgrGetBcEnabledStatus(pGpu) && (pMemDesc->_addressSpace == ADDR_FBMEM))
        {
            NV_ASSERT(!!(pMemDesc->_flags & MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE));
            gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);
        }
        pLast = pMemDescNew;

        pMemDescNew->_subDeviceAllocCount = pMemDesc->_subDeviceAllocCount;

        for (subDevInst = 0; subDevInst < pMemDesc->_subDeviceAllocCount; subDevInst++)
        {
            pGpuChild = gpumgrGetGpuFromSubDeviceInst(gpuGetDeviceInstance(pGpu), subDevInst);
            status = memdescCreateSubMem(&pNew, memdescGetMemDescFromGpu(pMemDesc, pGpuChild), pGpuChild, Offset, Size);

            if (status != NV_OK)
            {
                while (NULL != pMemDescNew)
                {
                    pNew = pMemDescNew;
                    pMemDescNew = pMemDescNew->_pNext;
                    memdescDestroy(pNew);
                }
                return status;
            }

            pLast->_pNext = pNew;
            pLast = pNew;
        }

        gpumgrSetBcEnabledStatus(pGpu, bBcState);
    }

    *ppMemDescNew = pMemDescNew;

    return NV_OK;

fail:
    memdescDestroy(pMemDescNew);
    return status;
}

/*!
 * Given a memdesc, this checks if the allocated memory falls under subheap or in GPA address space
 */
static NvBool
_memIsSriovMappingsEnabled
(
    PMEMORY_DESCRIPTOR   pMemDesc
)
{
    return gpuIsSriovEnabled(pMemDesc->pGpu) &&
           (((pMemDesc->_flags & MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE) && pMemDesc->bUsingSuballocator) ||
            (pMemDesc->_flags & MEMDESC_FLAGS_GUEST_ALLOCATED));
}

/*!
 * Fills pGpaEntries with numEntries GPAs from pMemDesc->_pteArray starting at
 * the given starting index. For physically contiguous memdescs, fills with
 * RM_PAGE_SIZE strides.
 */
static void
_memdescFillGpaEntriesForSpaTranslation
(
    PMEMORY_DESCRIPTOR   pMemDesc,
    RmPhysAddr          *pGpaEntries,
    NvU32                start,
    NvU32                numEntries
)
{
    if (pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS)
    {
        NvU32 i;

        for (i = 0; i < numEntries; i++)
        {
            pGpaEntries[i] = pMemDesc->_pteArray[0] + (((RmPhysAddr) (start + i)) * pMemDesc->pageArrayGranularity);
        }
    }
    else
    {
        portMemCopy(&pGpaEntries[0], numEntries * sizeof(pGpaEntries[0]),
                    &pMemDesc->_pteArray[start], numEntries * sizeof(pGpaEntries[0]));
    }
}

/*!
 * This function translates GPA -> SPA for a given memdesc and updates pPteSpaMappings with list of SPA addresses.
 * If memdesc is contiguous and if the translated SPA count > 1, this function fails for now.
 */
NV_STATUS
_memdescUpdateSpaArray
(
    PMEMORY_DESCRIPTOR   pMemDesc
)
{
    NV_STATUS   status   = NV_OK;
    RM_API     *pRmApi   = GPU_GET_PHYSICAL_RMAPI(pMemDesc->pGpu);
    NvU32       allocCnt;
    NvU32       i;
    NV2080_CTRL_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES_PARAMS *pParams = NULL;

    if ((pMemDesc->pPteSpaMappings) || (!pMemDesc->PageCount))
    {
        status = NV_OK;
        goto _memUpdateSpArray_exit;
    }

    allocCnt = memdescGetPteArraySize(pMemDesc, AT_PA);

    // Allocate the array to hold pages up to PageCount
    pMemDesc->pPteSpaMappings = portMemAllocNonPaged(sizeof(RmPhysAddr) * allocCnt);
    if (pMemDesc->pPteSpaMappings == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto _memUpdateSpArray_exit;
    }

    pParams = portMemAllocStackOrHeap(sizeof(*pParams));
    if (pParams == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto _memUpdateSpArray_exit;
    }
    portMemSet(pParams, 0, sizeof(*pParams));

    pParams->gfid = pMemDesc->gfid;

    for (i = 0; i < allocCnt; i += NV2080_CTRL_INTERNAL_VMMU_MAX_SPA_FOR_GPA_ENTRIES)
    {
        NvU32 numEntries = NV_MIN(allocCnt - i, NV2080_CTRL_INTERNAL_VMMU_MAX_SPA_FOR_GPA_ENTRIES);
        pParams->numEntries = numEntries;

        _memdescFillGpaEntriesForSpaTranslation(pMemDesc, &pParams->gpaEntries[0],
                                                i, numEntries);

        status = pRmApi->Control(pRmApi,
                                 pMemDesc->pGpu->hInternalClient,
                                 pMemDesc->pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES,
                                 pParams,
                                 sizeof(*pParams));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Getting SPA for GPA failed: GFID=%u, GPA=0x%llx\n",
                      pMemDesc->gfid, pMemDesc->_pteArray[i]);
            goto _memUpdateSpArray_exit;
        }

        portMemCopy(&pMemDesc->pPteSpaMappings[i], numEntries * sizeof(pParams->spaEntries[0]),
                    &pParams->spaEntries[0], numEntries * sizeof(pParams->spaEntries[0]));
    }

_memUpdateSpArray_exit:
    if (status != NV_OK)
    {
        portMemFree(pMemDesc->pPteSpaMappings);
        pMemDesc->pPteSpaMappings = NULL;
    }
    portMemFreeStackOrHeap(pParams);

    return status;
}

/*!
 *  @brief Return the physical addresses of pMemdesc
 *
 *  @param[in]  pMemDesc            Memory descriptor used
 *  @param[in]  pGpu                GPU to return the addresses for
 *  @param[in]  addressTranslation  Address translation identifier
 *  @param[in]  offset              Offset into memory descriptor
 *  @param[in]  stride              How much to advance the offset for each
 *                                  consecutive address
 *  @param[in]  count               How many addresses to retrieve
 *  @param[out] pAddresses          Returned array of addresses
 *
 */
void memdescGetPhysAddrsForGpu(MEMORY_DESCRIPTOR *pMemDesc,
                               OBJGPU *pGpu,
                               ADDRESS_TRANSLATION addressTranslation,
                               NvU64 offset,
                               NvU64 stride,
                               NvU64 count,
                               RmPhysAddr *pAddresses)
{
    //
    // Get the PTE array that we should use for phys addr lookups based on the
    // MMU context. (see bug 1625121)
    //
    NvU64 i;
    NvU64 pageIndex;
    RmPhysAddr *pteArray = memdescGetPteArrayForGpu(pMemDesc, pGpu, addressTranslation);
    const NvBool contiguous = (memdescGetPteArraySize(pMemDesc, addressTranslation) == 1);
    const NvU64 pageArrayGranularityMask = pMemDesc->pageArrayGranularity - 1;
    const NvU32 pageArrayGranularityShift = BIT_IDX_64(pMemDesc->pageArrayGranularity);

    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    offset += pMemDesc->PteAdjust;

    for (i = 0; i < count; ++i)
    {
        if (contiguous)
        {
            pAddresses[i] = pteArray[0] + offset;
        }
        else
        {
            pageIndex = offset >> pageArrayGranularityShift;
            NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, pageIndex < pMemDesc->PageCount);
            pAddresses[i] = pteArray[pageIndex] + (offset & pageArrayGranularityMask);
        }

        offset += stride;
    }
}


/*!
 *  @brief Return the physical addresses of pMemdesc
 *
 *  @param[in]  pMemDesc           Memory descriptor used
 *  @param[in]  addressTranslation Address translation identifier
 *  @param[in]  offset             Offset into memory descriptor
 *  @param[in]  stride             How much to advance the offset for each
 *                                 consecutive address
 *  @param[in]  count              How many addresses to retrieve
 *  @param[out] pAddresses         Returned array of addresses
 *
 */
void memdescGetPhysAddrs(MEMORY_DESCRIPTOR *pMemDesc,
                         ADDRESS_TRANSLATION addressTranslation,
                         NvU64 offset,
                         NvU64 stride,
                         NvU64 count,
                         RmPhysAddr *pAddresses)
{
    memdescGetPhysAddrsForGpu(pMemDesc, pMemDesc->pGpu, addressTranslation, offset, stride, count, pAddresses);
}

/*!
 *  @brief Return the physical address of pMemdesc+Offset
 *
 *  "long description"
 *
 *  @param[in]  pMemDesc           Memory descriptor used
 *  @param[in]  addressTranslation Address translation identifier
 *  @param[in]  offset             Offset into memory descriptor
 *
 *  @returns A physical address
 */
RmPhysAddr
memdescGetPhysAddr
(
    MEMORY_DESCRIPTOR *pMemDesc,
    ADDRESS_TRANSLATION addressTranslation,
    NvU64 offset
)
{
    RmPhysAddr addr;
    memdescGetPhysAddrs(pMemDesc, addressTranslation, offset, 0, 1, &addr);
    return addr;
}

/*!
 *  @brief Return physical address for page specified by PteIndex
 *
 *  @param[in]  pMemDesc           Memory descriptor to use
 *  @param[in]  addressTranslation Address translation identifier
 *  @param[in]  PteIndex           Look up this PteIndex
 *
 *  @returns A physical address
 */
RmPhysAddr
memdescGetPte
(
    PMEMORY_DESCRIPTOR  pMemDesc,
    ADDRESS_TRANSLATION addressTranslation,
    NvU32               PteIndex
)
{
    //
    // Get the PTE array that we should use for phys addr lookups based on the
    // MMU context. (see bug 1625121)
    //
    RmPhysAddr *pteArray = memdescGetPteArray(pMemDesc, addressTranslation);
    RmPhysAddr PhysAddr;

    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));

    if (pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS)
    {
        PhysAddr = pteArray[0] + (PteIndex << RM_PAGE_SHIFT);
    }
    else
    {
        PhysAddr = pteArray[PteIndex];
    }

    return PhysAddr;
}

/*!
 *  @brief Return physical address for page specified by PteIndex
 *
 *  @param[in]  pMemDesc           Memory descriptor to use
 *  @param[in]  addressTranslation Address translation identifier
 *  @param[in]  PteIndex           Look up this PteIndex
 *  @param[in]  PhysAddr           PTE address
 *
 *  @returns None
 */
void
memdescSetPte
(
    PMEMORY_DESCRIPTOR  pMemDesc,
    ADDRESS_TRANSLATION addressTranslation,
    NvU32               PteIndex,
    RmPhysAddr          PhysAddr
)
{
    //
    // Get the PTE array that we should use for phys addr lookups based on the
    // MMU context. (see bug 1625121)
    //
    RmPhysAddr *pteArray = memdescGetPteArray(pMemDesc, addressTranslation);
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));

    if (pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS)
    {
        NV_ASSERT_OR_RETURN_VOID(PteIndex == 0);
    }

    pteArray[PteIndex] = PhysAddr;

    // Free pteArraySpa
    portMemFree(pMemDesc->pPteSpaMappings);
    pMemDesc->pPteSpaMappings = NULL;
}

/*!
 *  @brief Return page array size based on the MMU context
 *         For SRIOV, the host context (AT_PA) will
 *         have discontiguous view of the GPA in SPA space
 *         This is treated similar to discontiguous memdescs
 *
 *  @param[in]  pMemDesc           Memory descriptor to use
 *  @param[in]  addressTranslation Address translation identifier
 *
 *  @returns PageArray
 */
NvU32 memdescGetPteArraySize(MEMORY_DESCRIPTOR *pMemDesc, ADDRESS_TRANSLATION addressTranslation)
{
    // Contiguous allocations in SPA domain can be non-contiguous at vmmusegment granularity.
    // Hence treat SPA domain allocations as non-contiguous by default.
    if (!(pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS) ||
         ((addressTranslation == AT_PA) && (pMemDesc->_addressSpace == ADDR_FBMEM) && _memIsSriovMappingsEnabled(pMemDesc)))
    {
        return NvU64_LO32(pMemDesc->PageCount);
    }
    return 1;
}

/*!
 *  @brief Return page array
 *
 *  @param[in]  pMemDesc           Memory descriptor to use
 *  @param[in]  pGpu               GPU to get the PTE array for.
 *  @param[in]  addressTranslation Address translation identifier
 *
 *  @returns PageArray
 */
RmPhysAddr *
memdescGetPteArrayForGpu
(
    PMEMORY_DESCRIPTOR  pMemDesc,
    OBJGPU             *pGpu,
    ADDRESS_TRANSLATION addressTranslation
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));

    switch (AT_VALUE(addressTranslation))
    {
        //
        // In SRIOV systems, an access from guest has to go through the following translations
        //     GVA -> GPA -> SPA
        //
        // Given HOST manages channel/memory management for guest, there are certain code paths that
        // expects VA -> GPA translations and some may need GPA -> SPA translations. We use addressTranslation
        // to differentiate between these cases.
        // Since GPA -> SPA is very similar to IOMMU xlation and since existing AT_PA is used only in
        // SYSMEM allocations, we decided to reuse AT_PA addressTranslation to fetch GPA -> SPA xlations.
        // In case of non-SRIOV systems, using AT_PA will fall back to AT_GPU or default context.
        //
        // pMemDesc -> _pteArray       tracks GVA -> GPA translations
        // pMemDesc -> pPteSpaMappings tracks GPA -> SPA translations
        //

        case AT_VALUE(AT_PA):
        {
            if (pGpu != NULL)
            {
                if (pMemDesc->_addressSpace == ADDR_FBMEM)
                {
                    if (_memIsSriovMappingsEnabled(pMemDesc))
                    {
                        if (!pMemDesc->pPteSpaMappings)
                            _memdescUpdateSpaArray(pMemDesc);

                        return pMemDesc->pPteSpaMappings;
                    }
                }
            }
        }
        case AT_VALUE(AT_GPU):
        {
            // Imported ADDR_FABRIC_V2 memdescs are device-less.
            if (pGpu != NULL)
            {
                if (memdescIsEgm(pMemDesc) && (pMemDesc->pPteEgmMappings != NULL))
                {
                    return pMemDesc->pPteEgmMappings;
                }

                PIOVAMAPPING pIovaMap = memdescGetIommuMap(pMemDesc,
                                            pGpu->busInfo.iovaspaceId);
                if (pIovaMap != NULL)
                {
                    return pIovaMap->iovaArray;
                }
            }

            //
            // If no IOMMU mapping exists in the default IOVASPACE, fall
            // through and use the physical memory descriptor instead.
            //
        }
        default:
        {
            return pMemDesc->_pteArray;
        }
    }
}



/*!
 *  @brief Convert aperture into a descriptive string.
 *
 *  @param[in]  addressSpace
 *
 *  @returns String
 *
 *  @todo "text"
 */
const char *
memdescGetApertureString
(
    NV_ADDRESS_SPACE addressSpace
)
{
    static NV_PRINTF_STRING_SECTION const char ADDR_FBMEM_STR[]  = "VIDEO MEMORY";
    static NV_PRINTF_STRING_SECTION const char ADDR_SYSMEM_STR[] = "SYSTEM MEMORY";

    if (addressSpace == ADDR_FBMEM)
    {
        return ADDR_FBMEM_STR;
    }

    if (addressSpace == ADDR_SYSMEM)
    {
        return ADDR_SYSMEM_STR;
    }

    return NULL;
}

/*!
 *  @brief Compare two memory descriptors to see if the memory described the same
 *
 *  @param[in]  pMemDescOne
 *  @param[in]  pMemDescTwo
 *
 *  @returns NV_TRUE if the memory descriptors refer to the same memory
 */
NvBool
memdescDescIsEqual
(
    MEMORY_DESCRIPTOR *pMemDescOne,
    MEMORY_DESCRIPTOR *pMemDescTwo
)
{
    if ((pMemDescOne == NULL) || (pMemDescTwo == NULL))
        return NV_FALSE;

    if (pMemDescOne->_addressSpace != pMemDescTwo->_addressSpace)
        return NV_FALSE;

    // All the physical memory views should match.
    if ((memdescGetPhysAddr(pMemDescOne, AT_CPU, 0) != memdescGetPhysAddr(pMemDescTwo, AT_CPU, 0)) ||
        (memdescGetPhysAddr(pMemDescOne, AT_GPU, 0) != memdescGetPhysAddr(pMemDescTwo, AT_GPU, 0)))
        return NV_FALSE;

    if (memdescGetCpuCacheAttrib(pMemDescOne) != memdescGetCpuCacheAttrib(pMemDescTwo))
        return NV_FALSE;

    if (pMemDescOne->Size != pMemDescTwo->Size)
        return NV_FALSE;

    if (pMemDescOne->Alignment != pMemDescTwo->Alignment)
        return NV_FALSE;

    if (pMemDescOne->_pageSize != pMemDescTwo->_pageSize)
        return NV_FALSE;

    return NV_TRUE;
}

/*!
 *  @brief Add callback block to the destroy callback queue
 *
 *  @param[in]  pMemDesc  Memory descriptor to update
 *  @param[in]  pCb       Callee allocated block with callback func/arg
 *
 *  @returns nothing
 */
void
memdescAddDestroyCallback
(
    MEMORY_DESCRIPTOR *pMemDesc,
    MEM_DESC_DESTROY_CALLBACK *pCb
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    pCb->pNext = memdescGetDestroyCallbackList(pMemDesc);
    memdescSetDestroyCallbackList(pMemDesc, pCb);
}

/*!
 *  @brief Remove callback block from the destroy callback queue
 *
 *  @param[in]  pMemDesc  Memory descriptor to update
 *  @param[in]  pRemoveCb Callee allocated block with callback func/arg
 *
 *  @returns nothing
 */
void
memdescRemoveDestroyCallback
(
    MEMORY_DESCRIPTOR *pMemDesc,
    MEM_DESC_DESTROY_CALLBACK *pRemoveCb
)
{
    MEM_DESC_DESTROY_CALLBACK *pCb = memdescGetDestroyCallbackList(pMemDesc);
    MEM_DESC_DESTROY_CALLBACK *pPrev = NULL;

    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    while (pCb)
    {
        if (pCb == pRemoveCb)
        {
            if (pPrev == NULL)
            {
                memdescSetDestroyCallbackList(pMemDesc, pCb->pNext);
            }
            else
            {
                pPrev->pNext = pCb->pNext;
            }
            break;
        }
        pPrev = pCb;
        pCb = pCb->pNext;
    }
}

/*!
 *  @brief Retrieves a subdevice's memory descriptor by subdevice instance
 *
 *  Subdevice memory descriptors are memory descriptors that describe
 *  per-subdevice memory buffers. This functionality is required by our current
 *  SLI programming model as our memdescAlloc() calls are primarily broadcast
 *  operations. A singular memdesc works for video memory as the
 *  heaps are symmetric. However, we run into trouble when dealing with system
 *  memory as both GPUs then share the same address space and symmetric
 *  addressing is no longer possible.
 *
 *  N.B. The rational for exposing this routine is that it keeps SLI-isms out of
 *  most of the RM -- the alternative approach would've been to pass in the
 *  subdevice or a pGpu for all memdesc methods which would require more code
 *  changes solely for SLI. Long term hopefully we can transition to a unicast
 *  allocation model (SLI loops above memdescAlloc()/memdescCreate()) and the
 *  subdevice support in memdesc can (easily) be deleted. This approach also
 *  provides a safety net against misuse, e.g., if we added pGpu to
 *  memdescGetPhysAddr, current code which utilizes that routine outside an SLI loop
 *  would execute cleanly even though it's incorrect.
 *
 *  @param[in]  pMemDesc        Memory descriptor to query
 *  @param[in]  subDeviceInst   SLI subdevice instance (subdevice - 1)
 *
 *  @returns Memory descriptor if one exist for the subdevice.
 *           NULL if none is found.
 */
MEMORY_DESCRIPTOR *
memdescGetMemDescFromSubDeviceInst(MEMORY_DESCRIPTOR *pMemDesc, NvU32 subDeviceInst)
{
    if (!memdescHasSubDeviceMemDescs(pMemDesc))
    {
        return pMemDesc;
    }
    else
    {
        return memdescGetMemDescFromIndex(pMemDesc, subDeviceInst);
    }
}

/*!
 *  @brief Retrieves a subdevice's memory descriptor by GPU object
 *
 *  See memdescGetMemDescFromSubDeviceInst for an explanation of subdevice memory
 *  descriptors
 *
 *  @param[in]  pMemDesc  Memory descriptor to query
 *  @param[in]  pGpu
 *
 *  @returns Memory descriptor if one exist for the GPU.
 *           NULL if none is found.
 */
MEMORY_DESCRIPTOR *
memdescGetMemDescFromGpu(MEMORY_DESCRIPTOR *pMemDesc, OBJGPU *pGpu)
{
    NvU32 subDeviceInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    return memdescGetMemDescFromSubDeviceInst(pMemDesc, subDeviceInst);
}

/*!
 *  @brief Retrieves a subdevice's memory descriptor by memdesc index.
 *
 *  See memdescGetMemDescFromSubDeviceInst for an explanation of subdevice memory
 *  descriptors
 *
 *  @param[in]  pMemDesc  Memory descriptor to query
 *  @param[in]  index     Index into array of memdesc
 *
 *  @returns Memory descriptor if one exist for the GPU.
 *           NULL if none is found.
 */
MEMORY_DESCRIPTOR *
memdescGetMemDescFromIndex(MEMORY_DESCRIPTOR *pMemDesc, NvU32 index)
{
    if (!memdescHasSubDeviceMemDescs(pMemDesc))
    {
        return pMemDesc;
    }
    else
    {
        MEMORY_DESCRIPTOR *pSubDevMemDesc = pMemDesc->_pNext;

        NV_ASSERT(pSubDevMemDesc);

        while (index--)
        {
            pSubDevMemDesc = pSubDevMemDesc->_pNext;

            if (!pSubDevMemDesc)
            {
                NV_ASSERT(0);
                return NULL;
            }
        }

        return pSubDevMemDesc;
    }
}

/*!
 *  @brief Set address for a fixed heap allocation.
 *
 *  Offset must refer to the heap.  A later memdescAlloc() will
 *  force this offset.
 *
 *  @param[in]  pMemDesc  Memory descriptor to update
 *  @param[in]  fbOffset  Offset to refer to
 *
 *  @returns nothing
 */
void
memdescSetHeapOffset
(
    MEMORY_DESCRIPTOR *pMemDesc,
    RmPhysAddr fbOffset
)
{
    NV_ASSERT(pMemDesc->_addressSpace == ADDR_FBMEM);
    NV_ASSERT(pMemDesc->Allocated == NV_FALSE);

    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    pMemDesc->_flags      |= MEMDESC_FLAGS_FIXED_ADDRESS_ALLOCATE;
    pMemDesc->_pteArray[0] = fbOffset;
}

/*!
 *  @brief Set GPU cacheability
 *
 *  A later memdescAlloc() will use this setting.
 *
 *  @param[in]  pMemDesc    Memory descriptor to update
 *  @param[in]  cacheAttrib Set memory to GPU cacheable
 *
 *  @returns nothing
 */
void memdescSetGpuCacheAttrib
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32 cacheAttrib
)
{
    NV_ASSERT(pMemDesc->Allocated == NV_FALSE);

    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    pMemDesc->_gpuCacheAttrib = cacheAttrib;
}

/*!
 *  @brief Get GPU P2P cache attributes
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Current GPU P2P cache attributes
 */
NvU32 memdescGetGpuP2PCacheAttrib
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    return pMemDesc->_gpuP2PCacheAttrib;
}

/*!
 *  @brief Set GPU P2P cacheability
 *
 *  A later memdescAlloc() will use this setting.
 *
 *  @param[in]  pMemDesc    Memory descriptor to update
 *  @param[in]  cacheAttrib Set memory to GPU P2P cacheable
 *
 *  @returns nothing
 */
void memdescSetGpuP2PCacheAttrib
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32 cacheAttrib
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    pMemDesc->_gpuP2PCacheAttrib = cacheAttrib;
}

/*!
 *  @brief Set CPU cacheability
 *
 *  A later memdescAlloc() will use this setting.
 *
 *  @param[in]  pMemDesc    Memory descriptor to update
 *  @param[in]  cacheAttrib Set memory to CPU cacheable
 *
 *  @returns nothing
 */
void memdescSetCpuCacheAttrib
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32 cpuCacheAttrib
)
{
    //
    // When running 64-bit MODS on ARM v8, we need to force all CPU mappings as WC.
    // This seems to be an issue with glibc. See bug 1556221.
    //
    // Ideally, this should have been set based on a Core Logic (CL) property.
    // But chipset initialization will only happen during bifStateInit().
    // RM can makes sysmem CPU mappings before bifStateInit().
    //
    if (RMCFG_FEATURE_PLATFORM_MODS && NVCPU_IS_AARCH64)
    {
        if (cpuCacheAttrib == NV_MEMORY_UNCACHED)
        {
            cpuCacheAttrib = NV_MEMORY_WRITECOMBINED;
        }
    }

    pMemDesc->_cpuCacheAttrib = cpuCacheAttrib;
}

/*!
 *  @brief Print contents of a MEMORY_DESCRIPTOR in a human readable format.
 *
 *  @param[in]  pMemDesc                Memory Descriptor to print
 *  @param[in]  bPrintIndividualPages   Individual pages will also be printed
 *                                      iff they are discontiguous
 *  @param[in]  pPrefixMessage          Message that will be printed before the contents
 *                                      of the Memory Descriptor are printed.
 *
 *  @returns nothing
 */
void memdescPrintMemdesc
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvBool             bPrintIndividualPages,
    const char        *pPrefixMessage
)
{
#if 0
    NvU32 i;

    if ((DBG_RMMSG_CHECK(DBG_LEVEL_INFO) == 0) || (pPrefixMessage == NULL) || (pMemDesc == NULL))
    {
        return;
    }

    NV_PRINTF(LEVEL_INFO,
                "%s Aperture %s starting at 0x%llx and of size 0x%llx\n",
                pPrefixMessage,
                memdescGetApertureString(pMemDesc->_addressSpace),
                memdescGetPhysAddr(pMemDesc, AT_CPU, 0),
                pMemDesc->Size);

    if ((bPrintIndividualPages == NV_TRUE) &&
        (pMemDesc->PageCount > 1) &&
        (!(pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS)))
    {
        for (i = 0; i < pMemDesc->PageCount; i++)
        {
            NV_PRINTF(LEVEL_INFO,
                        "     contains page starting @0x%llx\n",
                        pMemDesc->_pteArray[i]);
        }
    }

    // TODO: merge with SMMU path above (see bug 1625121).
    if (pMemDesc->_pIommuMappings != NULL)
    {
        if (!memdescIsSubMemoryMemDesc(pMemDesc))
        {
            PIOVAMAPPING pIovaMap = pMemDesc->_pIommuMappings;
            while (pIovaMap != NULL)
            {
                NV_PRINTF(LEVEL_INFO,
                    "Has additional IOMMU mapping for IOVA space 0x%x starting @ 0x%llx\n",
                    pIovaMap->iovaspaceId,
                    pIovaMap->iovaArray[0]);
                pIovaMap = pIovaMap->pNext;
            }
        }
        else
        {
            NV_PRINTF(LEVEL_INFO,
                "Has additional IOMMU mapping starting @ 0x%llx\n",
                memdescGetPhysAddr(pMemDesc, AT_PA, 0));
        }
    }
#endif // NV_PRINTF_ENABLED
}

/*!
 *  @brief Return page offset from a MEMORY_DESCRIPTOR for an arbitrary power of two page size
 *
 *  PageAdjust covers the 4KB alignment, but must include bits from the address for big pages.
 *
 *  @param[in]  pMemDesc                Memory Descriptor to print
 *  @param[in]  pageSize                Page size (4096, 64K, 128K, etc)
 *
 *  @returns nothing
 */
NvU64 memdescGetPageOffset
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU64 pageSize
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    return  (pMemDesc->PteAdjust + pMemDesc->_pteArray[0]) & (pageSize-1);
}

/*!
 *  @brief Get PTE kind using GPU
 *
 *  @param[in]  pMemDesc           Memory descriptor pointer
 *  @param[in]  pGpu               GPU to be used get supported kind
 *  @param[in]  addressTranslation Address translation identifier
 *
 *  @returns Current PTE kind value.
 */
NvU32 memdescGetPteKindForGpu
(
    PMEMORY_DESCRIPTOR  pMemDesc,
    OBJGPU             *pGpu
)
{
    return memmgrGetHwPteKindFromSwPteKind_HAL(pGpu, GPU_GET_MEMORY_MANAGER(pGpu), pMemDesc->_pteKind);
}

/*!
 *  @brief Set PTE kind using GPU.
 *
 *  @param[in]  pMemDesc           Memory descriptor pointer
 *  @param[in]  pGpu               GPU to be used set supported kind
 *  @param[in]  addressTranslation Address translation identifier
 *  @param[in]  pteKind            New PTE kind
 *
 *  @returns nothing
 */
void memdescSetPteKindForGpu
(
    PMEMORY_DESCRIPTOR  pMemDesc,
    OBJGPU             *pGpu,
    NvU32               pteKind
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    pMemDesc->_pteKind = memmgrGetSwPteKindFromHwPteKind_HAL(pGpu, GPU_GET_MEMORY_MANAGER(pGpu), pteKind);
    memdescSetFlag(pMemDesc, MEMDESC_FLAGS_SET_KIND, NV_TRUE);
}

/*!
 *  @brief Set PTE kind compressed value.
 *
 *  @param[in]  pMemDesc           Memory descriptor pointer
 *  @param[in]  pteKind            New PTE kind compressed value
 *
 *  @returns nothing
 */
void memdescSetPteKindCompressed
(
    PMEMORY_DESCRIPTOR  pMemDesc,
    NvU32               pteKindCmpr
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    pMemDesc->_pteKindCompressed = pteKindCmpr;
}

/*!
 *  @brief Get PTE kind compressed value.
 *
 *  @param[in]  pMemDesc           Memory descriptor pointer
 *  @param[in]  addressTranslation Address translation identifier
 *
 *  @returns Current PTE kind compressed value.
 */
NvU32 memdescGetPteKindCompressed
(
    PMEMORY_DESCRIPTOR  pMemDesc
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    return pMemDesc->_pteKindCompressed;
}

/*!
 *  @brief Get kernel mapping
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Current kernel mapping
 */
NvP64 memdescGetKernelMapping
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    return pMemDesc->_kernelMapping;
}

/*!
 *  @brief Set kernel mapping
 *
 *  @param[in]  pMemDesc        Memory descriptor pointer
 *  @param[in]  kernelMapping   New kernel mapping
 *
 *  @returns nothing
 */
void memdescSetKernelMapping
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvP64 kernelMapping
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    pMemDesc->_kernelMapping = kernelMapping;
}

/*!
 *  @brief Get privileged kernel mapping
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Current privileged kernel mapping
 */
NvP64 memdescGetKernelMappingPriv
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    return pMemDesc->_kernelMappingPriv;
}

/*!
 *  @brief Set HW resource identifier (HwResId)
 *
 *  @param[in]  pMemDesc            Memory descriptor pointer
 *  @param[in]  kernelMappingPriv   New privileged kernel mapping
 *
 *  @returns nothing
 */
void memdescSetKernelMappingPriv
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvP64 kernelMappingPriv
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    pMemDesc->_kernelMappingPriv = kernelMappingPriv;
}


/*!
 *  @brief Set standby buffer memory descriptor
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Pointer to standby buffer memory descriptor
 */
MEMORY_DESCRIPTOR *memdescGetStandbyBuffer
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    return pMemDesc->_pStandbyBuffer;
}

/*!
 *  @brief Set standby buffer memory descriptor
 *
 *  @param[in]  pMemDesc        Memory descriptor pointer
 *  @param[in]  pStandbyBuffer  Standby buffer memory descriptor pointer
 *
 *  @returns nothing
 */
void memdescSetStandbyBuffer
(
    MEMORY_DESCRIPTOR *pMemDesc,
    MEMORY_DESCRIPTOR *pStandbyBuffer
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    pMemDesc->_pStandbyBuffer = pStandbyBuffer;
}

/*!
 *  @brief Set mem destroy callback list pointer
 *
 *  @param[in]  pMemDesc                Memory descriptor pointer
 *  @param[in]  pMemDestroyCallbackList Memory destroy callback list pointer
 *
 *  @returns nothing
 */
void memdescSetDestroyCallbackList
(
    MEMORY_DESCRIPTOR *pMemDesc,
    MEM_DESC_DESTROY_CALLBACK *pCb
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    pMemDesc->_pMemDestroyCallbackList = pCb;
}

/*!
 *  @brief Get guest ID for specified memory descriptor
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Guest ID value
 */
NvU64 memdescGetGuestId
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    return pMemDesc->_guestId;
}

/*!
 *  @brief Set guest ID for memory descriptor
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *  @param[in]  guestId     New guest ID
 *
 *  @returns nothing
 */
void memdescSetGuestId
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU64 guestId
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    pMemDesc->_guestId = guestId;
}

/*!
 *  @brief Get value of specified flag
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *  @param[in]  flag        MEMDESC_FLAGS_* value
 *
 *  @returns Boolean value of specified flag
 */
NvBool memdescGetFlag
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU64 flag
)
{
    // For checking contiguity, use the memdescGetContiguity() api
    NV_ASSERT(flag != MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS);
    // GPU_IN_RESET is set/got from top level memdesc always.
    if (flag != MEMDESC_FLAGS_GPU_IN_RESET)
    {
        NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    }
    return !!(pMemDesc->_flags & flag);
}

/*!
 *  @brief Set value of specified flag
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *  @param[in]  flag        MEMDESC_FLAGS_* value
 *  @param[in]  bValue      Boolean value of flag
 *
 *  @returns nothing
 */
void memdescSetFlag
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU64 flag,
    NvBool bValue
)
{
    // For setting contiguity, use the memdescSetContiguity() api
    NV_ASSERT(flag != MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS);

    // GPU_IN_RESET is set/got from top level memdesc always.
    if (flag != MEMDESC_FLAGS_GPU_IN_RESET)
    {
        NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    }

    if (flag == MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE)
    {
        NV_ASSERT_OK(_memdescSetSubAllocatorFlag(pMemDesc->pGpu, pMemDesc, bValue));
        return;
    }
    else if (flag == MEMDESC_FLAGS_GUEST_ALLOCATED)
    {
        NV_ASSERT_OK(_memdescSetGuestAllocatedFlag(pMemDesc->pGpu, pMemDesc, bValue));
        return;
    }

    if (bValue)
        pMemDesc->_flags |= flag;
    else
        pMemDesc->_flags &= ~flag;
}

/*!
 *  @brief Return memory descriptor address pointer
 *
 *  The address value is returned by osAllocPages
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Memory descriptor address pointer
 */
NvP64 memdescGetAddress
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    return pMemDesc->_address;
}

/*!
 *  @brief Set memory descriptor address pointer
 *
 *  The address value is returned by osAllocPages
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *  @param[in]  pAddress    Pointer to address information
 *
 *  @returns nothing
 */
void memdescSetAddress
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvP64 pAddress
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    pMemDesc->_address = pAddress;
}

/*!
 *  @brief Get memory descriptor os-specific memory data pointer
 *
 *  The pMemData value is returned by osAllocPages
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Memory data pointer
 */
void *memdescGetMemData
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    return pMemDesc->_pMemData;
}

/*!
 *  @brief Set memory descriptor os-specific memory data pointer
 *
 *  The pMemData value is returned by osAllocPages
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *  @param[in]  pMemData    Pointer to new os-specific memory data
 *  @param[in]  pMemDataReleaseCallback Pointer to CB to be called when memdesc
 *                                      is freed.
 *
 *  @returns nothing
 */
void memdescSetMemData
(
    MEMORY_DESCRIPTOR          *pMemDesc,
    void                       *pMemData,
    MEM_DATA_RELEASE_CALL_BACK *pMemDataReleaseCallback
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    pMemDesc->_pMemData = pMemData;
    pMemDesc->_pMemDataReleaseCallback = pMemDataReleaseCallback;
}

/*!
 *  @brief Return memory descriptor volatile attribute
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Volatile or not
 */
NvBool memdescGetVolatility
(
    PMEMORY_DESCRIPTOR pMemDesc
)
{
    NvBool bVolatile = NV_FALSE;

    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    if (pMemDesc->_addressSpace == ADDR_SYSMEM)
    {
        bVolatile = (memdescGetGpuCacheAttrib(pMemDesc) == NV_MEMORY_UNCACHED);
    }
    else
    {
        NV_ASSERT(pMemDesc->_addressSpace == ADDR_FBMEM);
    }

    return bVolatile;
}

/*!
 *  @brief Quick check whether the memory is contiguous or not
 *
 *  @param[in]  pMemDesc           Memory descriptor used
 *  @param[in]  addressTranslation Address translation identifier
 *
 *  @returns NV_TRUE if contiguous
 */
NvBool memdescGetContiguity(PMEMORY_DESCRIPTOR pMemDesc, ADDRESS_TRANSLATION addressTranslation)
{
    return !!(pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS);
}

/*!
 *  @brief Detailed Check whether the memory is contiguous or not
 *
 *  @param[in]  pMemDesc           Memory descriptor used
 *  @param[in]  addressTranslation Address translation identifier
 *
 *  @returns NV_TRUE if contiguous
 */
NvBool memdescCheckContiguity(PMEMORY_DESCRIPTOR pMemDesc, ADDRESS_TRANSLATION addressTranslation)
{
    NvU32 i;

    if (!(pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS))
    {
        for (i = 0; i < (pMemDesc->PageCount - 1); i++)
        {
            if ((memdescGetPte(pMemDesc, addressTranslation, i) + pMemDesc->pageArrayGranularity) !=
                    memdescGetPte(pMemDesc, addressTranslation, i + 1))
                return NV_FALSE;
        }
    }

    return NV_TRUE;
}

/*!
 *  @brief Set the contiguity of the memory descriptor
 *
 *  @param[in]  pMemDesc           Memory descriptor used
 *  @param[in]  addressTranslation Address translation identifier
 *  @param[in]  isContiguous       Contiguity value
 *
 *  @returns nothing
 */
void memdescSetContiguity(PMEMORY_DESCRIPTOR pMemDesc, ADDRESS_TRANSLATION addressTranslation, NvBool isContiguous)
{
    NV_ASSERT_OR_RETURN_VOID(pMemDesc);

    if (isContiguous)
        pMemDesc->_flags |= MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS;
    else
        pMemDesc->_flags &= ~MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS;
}

/*!
 *  @brief Get the address space of the memory descriptor
 *
 *  @param[in]  pMemDesc           Memory descriptor used
 *  @param[in]  addressTranslation Address translation identifier
 *
 *  @returns addresspace
 */
NV_ADDRESS_SPACE memdescGetAddressSpace(PMEMORY_DESCRIPTOR pMemDesc)
{
    NV_ASSERT_OR_RETURN(pMemDesc != NULL, 0);
    return pMemDesc->_addressSpace;
}

/*!
 *  @brief Get page size
 *
 *  @param[in]  pMemDesc           Memory descriptor pointer
 *  @param[in]  addressTranslation Address translation identifier
 *
 *  @returns Current page size.
 */
NvU64 memdescGetPageSize
(
    PMEMORY_DESCRIPTOR  pMemDesc,
    ADDRESS_TRANSLATION addressTranslation
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    return pMemDesc->_pageSize;
}

/*!
 *  @brief Set page size
 *
 *  @param[in]  pMemDesc           Memory descriptor pointer
 *  @param[in]  addressTranslation Address translation identifier
 *  @param[in]  pteKind            New PTE kind
 *
 *  @returns nothing
 */
void memdescSetPageSize
(
    PMEMORY_DESCRIPTOR  pMemDesc,
    ADDRESS_TRANSLATION addressTranslation,
    NvU64               pageSize
)
{
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));
    pMemDesc->_pageSize = pageSize;
}

/*!
 *  @brief Get the Root memory descriptor.
 *
 *  This can also be used to get the root offset as well.
 *
 *  Root memory descriptor is the top level memory descriptor with no parent,
 *  from which this memory descriptor was derived
 *
 *  @param[in]  pMemDesc     Pointer to memory descriptor.
 *  @param[out] pRootOffset  Pointer to the root offset parameter.
 *
 *  @returns the Root memory descriptor.
 */
PMEMORY_DESCRIPTOR memdescGetRootMemDesc
(
    PMEMORY_DESCRIPTOR  pMemDesc,
    NvU64              *pRootOffset
)
{
    NvU64 offset = 0;

    // Find the top-level parent descriptor
    while (pMemDesc->_pParentDescriptor)
    {
        // Sanity check, None of the child descriptors should be allocated
        NV_ASSERT(!pMemDesc->Allocated);
        offset += pMemDesc->subMemOffset;
        pMemDesc = pMemDesc->_pParentDescriptor;
    }

    if (pRootOffset)
    {
        *pRootOffset = offset;
    }

    return pMemDesc;
}
/*!
 *  @brief Sets the CUSTOM_HEAP flag of MEMDESC.
 *
 *  Since we have ACR region, Memory descriptor can be allocated in ACR region
 *  in that case, we need to set this flag since we are using the custom ACR HEAP
 *
 *  @param[in]  pMemDesc     Pointer to memory descriptor.
 *
 *  @returns void.
 */
void
memdescSetCustomHeap
(
    PMEMORY_DESCRIPTOR  pMemDesc
)
{
    NV_ASSERT(0);
}

/*!
 *  @brief Returns the ACR CUSTOM_HEAP flag.
 *
 *
 *  @param[in]  pMemDesc     Pointer to memory descriptor.
 *
 *  @returns NV_TRUE if flag MEMDESC_FLAGS_CUSTOM_HEAP_ACR is SET.
 */
NvBool
memdescGetCustomHeap
(
    PMEMORY_DESCRIPTOR pMemDesc
)
{
    return NV_FALSE;
}

PIOVAMAPPING memdescGetIommuMap
(
    PMEMORY_DESCRIPTOR pMemDesc,
    NvU32 iovaspaceId
)
{
    PIOVAMAPPING pIommuMap = pMemDesc->_pIommuMappings;
    while (pIommuMap != NULL)
    {
        if (pIommuMap->iovaspaceId == iovaspaceId)
        {
            break;
        }

        pIommuMap = pIommuMap->pNext;
    }

    return pIommuMap;
}

NV_STATUS memdescAddIommuMap
(
    PMEMORY_DESCRIPTOR pMemDesc,
    PIOVAMAPPING pIommuMap
)
{
    NV_ASSERT_OR_RETURN((pMemDesc->_pIommuMappings == NULL) ||
        (!memdescIsSubMemoryMemDesc(pMemDesc)), NV_ERR_INVALID_ARGUMENT);

    //
    // Only root physical memdescs can have multiple IOMMU mappings.
    // Submemdescs can only have one, and the list linkage is used
    // instead to link it as a child of the root IOMMU mapping, so we
    // don't want to overwrite that here.
    //
    if (!memdescIsSubMemoryMemDesc(pMemDesc))
    {
        pIommuMap->pNext = pMemDesc->_pIommuMappings;
    }

    pMemDesc->_pIommuMappings = pIommuMap;

    return NV_OK;
}

void memdescRemoveIommuMap
(
    PMEMORY_DESCRIPTOR pMemDesc,
    PIOVAMAPPING pIommuMap
)
{
    //
    // Only root physical memdescs can have multiple IOMMU mappings.
    // Submemdescs can only have one, and the list linkage is used
    // instead to link it as a child of the root IOMMU mapping, so we
    // don't want to overwrite that here.
    //
    if (!memdescIsSubMemoryMemDesc(pMemDesc))
    {
        PIOVAMAPPING *ppTmpIommuMap = &pMemDesc->_pIommuMappings;
        while ((*ppTmpIommuMap != NULL) && (*ppTmpIommuMap != pIommuMap))
        {
            ppTmpIommuMap = &(*ppTmpIommuMap)->pNext;
        }

        if (*ppTmpIommuMap != NULL)
        {
            *ppTmpIommuMap = pIommuMap->pNext;

        }
        else
        {
            NV_ASSERT(*ppTmpIommuMap != NULL);
        }
    }
    else if (pMemDesc->_pIommuMappings == pIommuMap)
    {
        pMemDesc->_pIommuMappings = NULL;
    }
    else
    {
        //
        // Trying to remove a submemory mapping that doesn't belong to this
        // descriptor?
        //
        NV_ASSERT(pMemDesc->_pIommuMappings == pIommuMap);
    }
}

NV_STATUS memdescMapIommu
(
    PMEMORY_DESCRIPTOR pMemDesc,
    NvU32 iovaspaceId
)
{
#if (RMCFG_FEATURE_PLATFORM_UNIX || RMCFG_FEATURE_PLATFORM_MODS) && !NVCPU_IS_ARM
    if (iovaspaceId != NV_IOVA_DOMAIN_NONE)
    {
        NV_ADDRESS_SPACE addrSpace = memdescGetAddressSpace(pMemDesc);
        OBJGPU *pMappingGpu = gpumgrGetGpuFromId(iovaspaceId);
        PMEMORY_DESCRIPTOR pRootMemDesc = memdescGetRootMemDesc(pMemDesc, NULL);
        if ((addrSpace == ADDR_SYSMEM) || gpumgrCheckIndirectPeer(pMappingGpu, pRootMemDesc->pGpu))
        {
            NV_STATUS status;
            OBJIOVASPACE *pIOVAS = iovaspaceFromId(iovaspaceId);
            NV_ASSERT_OR_RETURN(pIOVAS, NV_ERR_OBJECT_NOT_FOUND);

            status = iovaspaceAcquireMapping(pIOVAS, pMemDesc);
            NV_ASSERT_OR_RETURN(status == NV_OK, status);
        }
    }
#endif

    //
    // Verify that the final physical addresses are indeed addressable by the
    // GPU. We only need to do this for internally allocated sysmem (RM owned)
    // as well as externally allocated/mapped sysmem. Note, addresses for peer
    // (P2P mailbox registers) BARs are actually not handled by the GMMU and
    // support a full 64-bit address width, hence validation is not needed.
    //
    if ((pMemDesc->Allocated ||
         memdescGetFlag(pMemDesc, MEMDESC_FLAGS_EXT_PAGE_ARRAY_MEM) ||
         memdescGetFlag(pMemDesc, MEMDESC_FLAGS_PEER_IO_MEM)) &&
        memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM)
    {
        // TODO This should look up the GPU corresponding to the IOVAS instead.
        OBJGPU *pGpu = pMemDesc->pGpu;
        RmPhysAddr dmaWindowStartAddr = gpuGetDmaStartAddress(pGpu);
        RmPhysAddr dmaWindowEndAddr = gpuGetDmaEndAddress_HAL(pGpu);
        RmPhysAddr physAddr;

        if (memdescGetContiguity(pMemDesc, AT_GPU))
        {
            physAddr = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
            if ((physAddr < dmaWindowStartAddr) ||
                (physAddr + pMemDesc->Size - 1 > dmaWindowEndAddr))
            {
                NV_PRINTF(LEVEL_ERROR,
                          "0x%llx-0x%llx is not addressable by GPU 0x%x [0x%llx-0x%llx]\n",
                          physAddr, physAddr + pMemDesc->Size - 1,
                          pGpu->gpuId, dmaWindowStartAddr, dmaWindowEndAddr);
                memdescUnmapIommu(pMemDesc, iovaspaceId);
                return NV_ERR_INVALID_ADDRESS;
            }
        }
        else
        {
            NvU32 i;
            for (i = 0; i < pMemDesc->PageCount; i++)
            {
                physAddr = memdescGetPte(pMemDesc, AT_GPU, i);
                if ((physAddr < dmaWindowStartAddr) ||
                    (physAddr + (pMemDesc->pageArrayGranularity - 1) > dmaWindowEndAddr))
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "0x%llx is not addressable by GPU 0x%x [0x%llx-0x%llx]\n",
                              physAddr, pGpu->gpuId, dmaWindowStartAddr,
                              dmaWindowEndAddr);
                    memdescUnmapIommu(pMemDesc, iovaspaceId);
                    return NV_ERR_INVALID_ADDRESS;
                }
            }
        }
    }

    return NV_OK;
}

void memdescUnmapIommu
(
    PMEMORY_DESCRIPTOR pMemDesc,
    NvU32 iovaspaceId
)
{
#if (RMCFG_FEATURE_PLATFORM_UNIX || RMCFG_FEATURE_PLATFORM_MODS) && !NVCPU_IS_ARM
    PIOVAMAPPING pIovaMapping;
    OBJIOVASPACE *pIOVAS;

    if (iovaspaceId == NV_IOVA_DOMAIN_NONE)
        return;

    pIovaMapping = memdescGetIommuMap(pMemDesc, iovaspaceId);
    NV_ASSERT(pIovaMapping);

    pIOVAS = iovaspaceFromMapping(pIovaMapping);
    iovaspaceReleaseMapping(pIOVAS, pIovaMapping);
#endif
}

void memdescCheckSubDevicePageSizeConsistency
(
    OBJGPU              *pGpu,
    PMEMORY_DESCRIPTOR   pMemDesc,
    OBJVASPACE          *pVAS,
    NvU64                pageSize,
    NvU64                pageOffset
)
{
    NvU64 tempPageSize, tempPageOffset;
    PMEMORY_DESCRIPTOR pTempMemDesc = NULL;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
       pTempMemDesc   = memdescGetMemDescFromGpu(pMemDesc, pGpu);
       tempPageSize   = memdescGetPageSize(pTempMemDesc, VAS_ADDRESS_TRANSLATION(pVAS));
       tempPageOffset = memdescGetPhysAddr(pTempMemDesc, VAS_ADDRESS_TRANSLATION(pVAS), 0) & (tempPageSize - 1);

       // Assert if inconsistent
       NV_ASSERT(pageSize == tempPageSize);
       NV_ASSERT(pageOffset == tempPageOffset);
    SLI_LOOP_END
}

void memdescCheckSubDeviceMemContiguityConsistency
(
    OBJGPU              *pGpu,
    PMEMORY_DESCRIPTOR   pMemDesc,
    OBJVASPACE          *pVAS,
    NvBool               bIsMemContiguous
)
{
    NvBool bTempIsMemContiguous = NV_FALSE;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
       bTempIsMemContiguous = memdescGetContiguity(memdescGetMemDescFromGpu(pMemDesc, pGpu), VAS_ADDRESS_TRANSLATION(pVAS));
       // Assert if inconsistent
       NV_ASSERT(bIsMemContiguous == bTempIsMemContiguous);
    SLI_LOOP_END
}

NV_STATUS memdescCheckSubDeviceKindComprConsistency
(
    OBJGPU             *pGpu,
    MEMORY_DESCRIPTOR  *pMemDesc,
    OBJVASPACE         *pVAS,
    NvU32               kind,
    COMPR_INFO         *pComprInfo
)
{
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        MemoryManager    *MemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
        NvU32             tempKind;
        COMPR_INFO        tempComprInfo = {0};
        NV_STATUS         status;

        status = memmgrGetKindComprFromMemDesc(MemoryManager,
                                               memdescGetMemDescFromGpu(pMemDesc, pGpu),
                                               0,
                                               &tempKind, &tempComprInfo);

        if (NV_OK != status)
            SLI_LOOP_RETURN(status);

        // Assert if inconsistent
        NV_ASSERT(kind == tempKind);
        NV_ASSERT(tempComprInfo.compPageShift         == pComprInfo->compPageShift &&
                  tempComprInfo.kind                  == pComprInfo->kind &&
                  tempComprInfo.compPageIndexLo       == pComprInfo->compPageIndexLo &&
                  tempComprInfo.compPageIndexHi       == pComprInfo->compPageIndexHi &&
                  tempComprInfo.compTagLineMin        == pComprInfo->compTagLineMin &&
                  tempComprInfo.compTagLineMultiplier == pComprInfo->compTagLineMultiplier);
    }
    SLI_LOOP_END

    return NV_OK;
}

/*  @brief Get GPA(guest physical addresses) for given GPU physical addresses.
 *
 *  @param[in]  pGpu         GPU for which GPAs are needed
 *  @param[in]  pageCount    Size of array. Should be 1 for contiguous mappings
 *  @param[in/out] pGpa      Array of GPU PAs to be converted to guest PAs
 *
 *  @returns NV_STATUS
 */
NV_STATUS memdescGetNvLinkGpa
(
    OBJGPU            *pGpu,
    NvU64              pageCount,
    RmPhysAddr        *pGpa
)
{
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

    NV_ASSERT_OR_RETURN(pGpa, NV_ERR_INVALID_ARGUMENT);

    NvU32  pageIndex;
    // For each page, do the GPU PA to GPA conversion
    for (pageIndex = 0; pageIndex < pageCount; pageIndex++)
    {
        RmPhysAddr gpa;
        gpa = pGpa[pageIndex] + pKernelMemorySystem->coherentCpuFbBase;

        NV_ASSERT_OR_RETURN((gpa >= pKernelMemorySystem->coherentCpuFbBase) &&
                            (gpa <= pKernelMemorySystem->coherentCpuFbEnd),
                            NV_ERR_INVALID_ARGUMENT);

        pGpa[pageIndex] = gpa;
    }

    return NV_OK;
}

NV_STATUS
memdescSetCtxBufPool
(
    PMEMORY_DESCRIPTOR pMemDesc,
    CTX_BUF_POOL_INFO *pCtxBufPool
)
{

    NV_ASSERT_OR_RETURN(!pMemDesc->Allocated, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(!memdescHasSubDeviceMemDescs(pMemDesc), NV_ERR_INVALID_ARGUMENT);

    pMemDesc->pCtxBufPool = pCtxBufPool;
    return NV_OK;
}

CTX_BUF_POOL_INFO*
memdescGetCtxBufPool
(
    PMEMORY_DESCRIPTOR pMemDesc
)
{
    NV_ASSERT_OR_RETURN(!memdescHasSubDeviceMemDescs(pMemDesc), NULL);
    return pMemDesc->pCtxBufPool;
}

/*!
 * @brief Override the registry INST_LOC two-bit enum to an aperture (list) + cpu attr.
 *
 * Caller must set initial default values.
 */
void
memdescOverrideInstLocList
(
    NvU32                    instLoc,       // NV_REG_STR_RM_INST_LOC
    const char              *name,
    const NV_ADDRESS_SPACE **ppAllocList,
    NvU32                   *pCpuMappingAttr
)
{
    switch (instLoc)
    {
        case NV_REG_STR_RM_INST_LOC_COH:
            NV_PRINTF(LEVEL_INFO, "using coh system memory for %s\n", name);
            *ppAllocList = ADDRLIST_SYSMEM_ONLY;
            *pCpuMappingAttr = NV_MEMORY_CACHED;
            break;
        case NV_REG_STR_RM_INST_LOC_NCOH:
            NV_PRINTF(LEVEL_INFO, "using ncoh system memory for %s\n", name);
            *ppAllocList = ADDRLIST_SYSMEM_ONLY;
            *pCpuMappingAttr = NV_MEMORY_UNCACHED;
            break;
        case NV_REG_STR_RM_INST_LOC_VID:
            NV_PRINTF(LEVEL_INFO, "using video memory for %s\n", name);
            *ppAllocList = ADDRLIST_FBMEM_ONLY;
            *pCpuMappingAttr = NV_MEMORY_WRITECOMBINED;
            break;
        case NV_REG_STR_RM_INST_LOC_DEFAULT:
        default:
            // Do not update parameters
            break;
    }
}

/*!
 * @brief Override wrapper for callers needed an aperture
 */
void
memdescOverrideInstLoc
(
    NvU32             instLoc,
    const char       *name,
    NV_ADDRESS_SPACE *pAddrSpace,
    NvU32            *pCpuMappingAttr
)
{
    const NV_ADDRESS_SPACE *pAllocList = NULL;

    memdescOverrideInstLocList(instLoc, name, &pAllocList, pCpuMappingAttr);
    if (pAllocList != NULL)
        *pAddrSpace = pAllocList[0];
}
/*!
*  @brief override physical address width
*
*  address width to be override in bits.
*  @param[in]  pGpu
*  @param[in]  pMemDesc  Memory descriptor to update
*  @param[in]  addresswidth  Offset to refer to
*
*  @returns nothing
*/
void
memdescOverridePhysicalAddressWidthWindowsWAR
(
    OBJGPU *pGpu,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32 addressWidth
)
{
    if (RMCFG_FEATURE_PLATFORM_WINDOWS)
    {
        if (addressWidth < gpuGetPhysAddrWidth_HAL(pGpu, ADDR_SYSMEM))
        {
            pMemDesc->_flags |= MEMDESC_FLAGS_OVERRIDE_SYSTEM_ADDRESS_LIMIT;
            pMemDesc->_overridenAddressWidth = addressWidth;
        }
    }
}

void
memdescSetName(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc, const char *name, const char* suffix)
{
    return;
}

NV_STATUS
memdescSendMemDescToGSP(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc, NvHandle *pHandle)
{
    NV_STATUS                         status          = NV_OK;
    RsClient                         *pClient;
    MemoryManager                    *pMemoryManager  = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU32                             flags           = 0;
    NvU32                             index           = 0;
    NvU32                             hClass;
    NvU64                            *pageNumberList  = NULL;
    RM_API                           *pRmApi          = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV_MEMORY_LIST_ALLOCATION_PARAMS  listAllocParams = {0};

    // Nothing to do without GSP
    if (!IS_GSP_CLIENT(pGpu))
    {
        return NV_OK;
    }

    switch (memdescGetAddressSpace(pMemDesc))
    {

        case ADDR_FBMEM:
            hClass = NV01_MEMORY_LIST_FBMEM;
            break;

        case ADDR_SYSMEM:
            hClass = NV01_MEMORY_LIST_SYSTEM;
            break;

        default:
            return NV_ERR_NOT_SUPPORTED;
    }

    // Initialize parameters with pMemDesc information
    listAllocParams.pteAdjust = pMemDesc->PteAdjust;
    listAllocParams.format    = memdescGetPteKind(pMemDesc);
    listAllocParams.size      = pMemDesc->Size;
    listAllocParams.hClient   = NV01_NULL_OBJECT;
    listAllocParams.hParent   = NV01_NULL_OBJECT;
    listAllocParams.hObject   = NV01_NULL_OBJECT;
    listAllocParams.limit     = pMemDesc->Size - 1;
    listAllocParams.flagsOs02 = (DRF_DEF(OS02,_FLAGS,_MAPPING,_NO_MAP) |
                                (flags & DRF_SHIFTMASK(NVOS02_FLAGS_COHERENCY)));

    // Handle pageCount based on pMemDesc contiguity
    if (!memdescGetContiguity(pMemDesc, AT_GPU))
    {
        listAllocParams.flagsOs02 |=  DRF_DEF(OS02,_FLAGS,_PHYSICALITY,_NONCONTIGUOUS);
        listAllocParams.pageCount = pMemDesc->PageCount;
    }
    else
    {
        listAllocParams.pageCount = 1;
    }


    // Initialize pageNumberList
    pageNumberList = portMemAllocNonPaged(sizeof(NvU64) * listAllocParams.pageCount);
    for (index = 0; index < listAllocParams.pageCount; index++)
        pageNumberList[index] = memdescGetPte(pMemDesc, AT_GPU, index) >> RM_PAGE_SHIFT;
    listAllocParams.pageNumberList = pageNumberList;

    // Create MemoryList object
    NV_ASSERT_OK_OR_GOTO(status,
                         pRmApi->Alloc(pRmApi,
                                       pMemoryManager->hClient,
                                       pMemoryManager->hSubdevice,
                                       pHandle,
                                       hClass,
                                       &listAllocParams,
                                       sizeof(listAllocParams)),
                         end);

    NV_ASSERT_OK_OR_GOTO(status,
        serverGetClientUnderLock(&g_resServ, pMemoryManager->hClient, &pClient),
        end);

    // Register MemoryList object to GSP
    NV_ASSERT_OK_OR_GOTO(status,
                         memRegisterWithGsp(pGpu,
                                            pClient,
                                            pMemoryManager->hSubdevice,
                                            *pHandle),
                         end);

end:
    if ((status != NV_OK) && (*pHandle != NV01_NULL_OBJECT))
        pRmApi->Free(pRmApi, pMemoryManager->hClient, *pHandle);

    if (pageNumberList != NULL)
        portMemFree(pageNumberList);

    return status;
}

NV_STATUS
memdescSetPageArrayGranularity
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU64 pageArrayGranularity
)
{
    // Make sure pageArrayGranularity is a power of 2 value.
    NV_ASSERT_OR_RETURN((pageArrayGranularity & (pageArrayGranularity - 1)) == 0, NV_ERR_INVALID_ARGUMENT);

    // Allow setting the same granularity.
    if (pMemDesc->pageArrayGranularity == pageArrayGranularity)
    {
        return NV_OK;
    }

    // Make sure setting the page array happens before the pteArray is populated.
    NV_ASSERT_OR_RETURN(pMemDesc->_pteArray[0] == 0, NV_ERR_INVALID_STATE);

    pMemDesc->pageArrayGranularity = pageArrayGranularity;

    return NV_OK;
}

NV_STATUS
memdescFillMemdescForPhysAttr
(
    MEMORY_DESCRIPTOR *pMemDesc,
    ADDRESS_TRANSLATION addressTranslation,
    NvU64 *pOffset,
    NvU32 *pMemAperture,
    NvU32 *pMemKind,
    NvU32 *pZCullId,
    NvU32 *pGpuCacheAttr,
    NvU32 *pGpuP2PCacheAttr,
    NvU64 *contigSegmentSize
)
{
    NvU64 surfOffset = *pOffset, surfBase, surfLimit;
    NvU32 zcbitmap;

    surfBase  = memdescGetPhysAddr(pMemDesc, addressTranslation, 0);
    surfLimit = surfBase + pMemDesc->Size - 1;
    *pMemKind = memdescGetPteKind(pMemDesc);

    *pOffset  = memdescGetPhysAddr(pMemDesc, addressTranslation, surfOffset);

    if (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM )
        *pMemAperture = NV0041_CTRL_CMD_GET_SURFACE_PHYS_ATTR_APERTURE_VIDMEM;
    else if (memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM)
        *pMemAperture = NV0041_CTRL_CMD_GET_SURFACE_PHYS_ATTR_APERTURE_SYSMEM;
    else if (memdescGetAddressSpace(pMemDesc) == ADDR_EGM)
        *pMemAperture = NV0041_CTRL_CMD_GET_SURFACE_PHYS_ATTR_APERTURE_SYSMEM;
    else if (memdescGetAddressSpace(pMemDesc) == ADDR_VIRTUAL )
    {
        //
        // XXX we could theoretically find whatever phys mem object is plugged
        // in at surfOffset w/in the virt object... that'd mean scanning
        // pMemory->DmaMappingList
        //
        return NV_ERR_NOT_SUPPORTED;
    }
    else
        return NV_ERR_GENERIC;

    if (memdescGetGpuCacheAttrib(pMemDesc) == NV_MEMORY_CACHED)
    {
        *pGpuCacheAttr = NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED;
    }
    else if (memdescGetGpuCacheAttrib(pMemDesc) == NV_MEMORY_UNCACHED)
    {
        *pGpuCacheAttr = NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_UNCACHED;
    }
    else
    {
        *pGpuCacheAttr = NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED_UNKNOWN;
    }

    if (memdescGetGpuP2PCacheAttrib(pMemDesc) == NV_MEMORY_CACHED)
    {
        *pGpuP2PCacheAttr = NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED;
    }
    else if (memdescGetGpuP2PCacheAttrib(pMemDesc) == NV_MEMORY_UNCACHED)
    {
        *pGpuP2PCacheAttr = NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_UNCACHED;
    }
    else
    {
        *pGpuP2PCacheAttr = NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED_UNKNOWN;
    }

    zcbitmap = FB_HWRESID_ZCULL_VAL_FERMI(memdescGetHwResId(pMemDesc)); //bitmap form... need a scalar
    for ( *pZCullId = 0;  zcbitmap; zcbitmap >>= 1, *pZCullId += 1) {;;;}
    *pZCullId -= 1; // side effect if there is no zcull id of setting ~0

    *contigSegmentSize = surfLimit - (surfBase + surfOffset) + 1;

    if ( !memdescGetContiguity(pMemDesc, addressTranslation))
    {
        // XXX overly conservative.  we could scan the PTEs to find out if more pages are contig.
        NvU64 surfOffsetLimitSame4KBPage = (4*1024)*((surfBase + surfOffset)/(4*1024)) + (4*1024) - 1;
        if ( surfLimit >= surfOffsetLimitSame4KBPage )
            *contigSegmentSize = surfOffsetLimitSame4KBPage - (surfBase + surfOffset) + 1;
    }

    return NV_OK;
}

NvBool
memdescIsEgm
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_ADDRESS_SPACE   addrSpace;
    MEMORY_DESCRIPTOR *pRootMemDesc;
    MemoryManager     *pMemoryManager;

    //
    // If memdesc is not device owned, we can't tell if local EGM is enabled
    // due to lack of memory manager.
    //
    if (pMemDesc->pGpu == NULL)
    {
        return NV_FALSE;
    }

    addrSpace = memdescGetAddressSpace(pMemDesc);
    pRootMemDesc = memdescGetRootMemDesc(pMemDesc, NULL);

    if ((pRootMemDesc == NULL) || (pRootMemDesc->pGpu == NULL))
    {
        return NV_FALSE;
    }

    pMemoryManager = GPU_GET_MEMORY_MANAGER(pRootMemDesc->pGpu);
    if (pMemoryManager == NULL)
    {
        return NV_FALSE;
    }

    if ((addrSpace == ADDR_EGM) ||
        (memmgrIsLocalEgmEnabled(pMemoryManager) &&
         (addrSpace == ADDR_SYSMEM) &&
         (memdescGetNumaNode(pMemDesc) == pMemoryManager->localEgmNodeId)))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}
