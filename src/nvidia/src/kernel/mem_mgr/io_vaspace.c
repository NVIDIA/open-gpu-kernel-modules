/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/***************************** HW State Routines ***************************\
*                                                                           *
*         IOMMU Virtual Address Space Function Definitions.                 *
*                                                                           *
\***************************************************************************/

#include "mem_mgr/io_vaspace.h"
#include "class/cl00f2.h"  // IO_VASPACE_A
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "gpu_mgr/gpu_mgr.h"
#include "os/os.h"
#include "core/system.h"
#include "mem_mgr/virt_mem_mgr.h"
#include "gpu/mmu/kern_gmmu.h"


NV_STATUS
iovaspaceConstruct__IMPL
(
    OBJIOVASPACE *pIOVAS,
    NvU32         classId,
    NvU32         vaspaceId,
    NvU64         vaStart,
    NvU64         vaLimit,
    NvU64         vaStartInternal,
    NvU64         vaLimitInternal,
    NvU32         flags
)
{
    NV_ASSERT_OR_RETURN(IO_VASPACE_A == classId, NV_ERR_INVALID_ARGUMENT);
    pIOVAS->mappingCount = 0;
    return NV_OK;
}

void
iovaspaceDestruct_IMPL(OBJIOVASPACE *pIOVAS)
{
    OBJVASPACE *pVAS = staticCast(pIOVAS, OBJVASPACE);

    if (pIOVAS->mappingCount != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "%lld left-over mappings in IOVAS 0x%x\n",
                  pIOVAS->mappingCount, pVAS->vaspaceId);
        DBG_BREAKPOINT();
    }
}

NV_STATUS
iovaspaceAlloc_IMPL
(
    OBJIOVASPACE    *pIOVAS,
    NvU64            size,
    NvU64            align,
    NvU64            rangeLo,
    NvU64            rangeHi,
    NvU64            pageSizeLockMask,
    VAS_ALLOC_FLAGS  flags,
    NvU64           *pAddr
)
{
    NV_STATUS   status = NV_OK;

    // TBD implement iommu specific stuff
    return status;
}

NV_STATUS
iovaspaceFree_IMPL
(
    OBJIOVASPACE *pIOVAS,
    NvU64         vAddr
)
{
    NV_STATUS   status = NV_OK;

    // TBD implement iommu specific stuff
    return status;
}

NV_STATUS
iovaspaceApplyDefaultAlignment_IMPL
(
    OBJIOVASPACE        *pIOVAS,
    const FB_ALLOC_INFO *pAllocInfo,
    NvU64               *pAlign,
    NvU64               *pSize,
    NvU64               *pPageSizeLockMask
)
{
    RM_ATTR_PAGE_SIZE   pageSizeAttr;
    NvU64               maxPageSize = RM_PAGE_SIZE;

    pageSizeAttr = dmaNvos32ToPageSizeAttr(pAllocInfo->pageFormat->attr, pAllocInfo->pageFormat->attr2);
    switch(pageSizeAttr)
    {
        case RM_ATTR_PAGE_SIZE_DEFAULT:
        case RM_ATTR_PAGE_SIZE_4KB:
            *pAlign = NV_MAX(*pAlign, maxPageSize);
            *pSize = RM_ALIGN_UP(*pSize, maxPageSize);
            return NV_OK;
        default:
            break;
    }

    {
        OBJVASPACE         *pVAS;
        OBJGPU             *pGpu;
        KernelGmmu         *pKernelGmmu;
        //
        // This is reached when the page size is BIG or greater. We should only
        // reach here for T124 or later chips which have GPU, or for standalone GPU
        // chips, and require the GPU's big page size here, not the IOMMU's big page
        // as it doesn't have big page.
        //
        pVAS = staticCast(pIOVAS, OBJVASPACE);
        pGpu = gpumgrGetGpu(gpumgrGetDefaultPrimaryGpu(pVAS->gpuMask));
        pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
        NV_ASSERT_OR_RETURN(NULL != pKernelGmmu, NV_ERR_INVALID_POINTER);

        switch (pageSizeAttr)
        {
            case RM_ATTR_PAGE_SIZE_BIG:
                maxPageSize = kgmmuGetMaxBigPageSize_HAL(pKernelGmmu);
                break;
            case NVOS32_ATTR_PAGE_SIZE_HUGE:
                NV_ASSERT_OR_RETURN(kgmmuIsHugePageSupported(pKernelGmmu),
                                NV_ERR_INVALID_ARGUMENT);
                maxPageSize = RM_PAGE_SIZE_HUGE;
                break;
            case RM_ATTR_PAGE_SIZE_512MB:
                NV_ASSERT_OR_RETURN(kgmmuIsPageSize512mbSupported(pKernelGmmu),
                                NV_ERR_INVALID_ARGUMENT);
                maxPageSize = RM_PAGE_SIZE_512M;
                break;
            case RM_ATTR_PAGE_SIZE_256GB:
            default:
                break;
        }

        //
        // Offset and size must be aligned to maximum potential map page size.
        // NOTE: Ignoring NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE but still using
        //       requested alignment as a lower bound.
        //
        *pAlign = NV_MAX(*pAlign, maxPageSize);
        *pSize = RM_ALIGN_UP(*pSize, maxPageSize);
    }
    return NV_OK;
}

NV_STATUS
iovaspaceIncAllocRefCnt_IMPL
(
    OBJIOVASPACE *pIOVAS,
    NvU64        vAddr
)
{
    NV_STATUS   status       = NV_OK;

    // TBD: Implement iommu specific stuff
    return status;
}

NV_STATUS
iovaspaceGetVasInfo_IMPL
(
    OBJIOVASPACE                                         *pIOVAS,
    struct NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS  *pParams
)
{
    return NV_OK;
}

NvU64
iovaspaceGetVaStart_IMPL(OBJIOVASPACE *pIOVAS)
{
    // TODO: query OS layer, this could also be set in ctor, not virtual?
    return 0;
}

NvU64
iovaspaceGetVaLimit_IMPL(OBJIOVASPACE *pIOVAS)
{
    // TODO: query OS layer, this could also be set in ctor, not virtual?
    return NVBIT64(32) - 1;
}

#if (RMCFG_FEATURE_PLATFORM_UNIX || RMCFG_FEATURE_MODS_FEATURES) && !NVCPU_IS_ARM
static PIOVAMAPPING
_iovaspaceCreateMappingDataFromMemDesc
(
    PMEMORY_DESCRIPTOR pMemDesc
)
{
    PIOVAMAPPING pIovaMapping = NULL;
    NvU64 mappingDataSize = 0;

    mappingDataSize = sizeof(IOVAMAPPING);
    if (!memdescGetContiguity(pMemDesc, AT_CPU))
    {
        mappingDataSize += sizeof(RmPhysAddr) *
            (NvU64_LO32(pMemDesc->PageCount) - 1);
    }

    //
    // The portMemAllocNonPaged() and portMemSet() interfaces work with 32-bit sizes,
    // so make sure we don't exceed that here.
    //
    if (NvU64_HI32(mappingDataSize) != 0UL)
    {
        NV_PRINTF(LEVEL_ERROR, "too much memory to map! (0x%llx bytes)\n",
                  mappingDataSize);
        DBG_BREAKPOINT();
        return NULL;
    }

    pIovaMapping = portMemAllocNonPaged(NvU64_LO32(mappingDataSize));
    if (pIovaMapping == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed to allocate 0x%x bytes for IOVA mapping metadata\n",
                  NvU64_LO32(mappingDataSize));
        return NULL;
    }

    portMemSet((void *)pIovaMapping, 0, NvU64_LO32(mappingDataSize));

    pIovaMapping->pPhysMemDesc = pMemDesc;

    return pIovaMapping;
}

static NV_STATUS
_iovaspaceCreateSubmapping
(
    OBJIOVASPACE *pIOVAS,
    PMEMORY_DESCRIPTOR pPhysMemDesc
)
{
    NvU64 rootOffset;
    NV_STATUS status = NV_OK;
    OBJVASPACE *pVAS = staticCast(pIOVAS, OBJVASPACE);
    PMEMORY_DESCRIPTOR pRootMemDesc = memdescGetRootMemDesc(pPhysMemDesc, &rootOffset);
    PIOVAMAPPING pRootIovaMapping;
    PIOVAMAPPING pSubMapping = NULL;

    NV_ASSERT(pRootMemDesc != pPhysMemDesc);

    //
    // A submapping requires the root mapping to be there, acquire a reference
    // on it so that it sticks around for at least as long as the submapping.
    // The reference is released when the submapping is destroyed.
    //
    status = iovaspaceAcquireMapping(pIOVAS, pRootMemDesc);
    if (status != NV_OK)
        return status;

    //
    // The root mapping has been just successfully acquired so it has to be
    // there.
    //
    pRootIovaMapping = memdescGetIommuMap(pRootMemDesc, pVAS->vaspaceId);
    NV_ASSERT(pRootIovaMapping != NULL);

    //
    // Since this is a submemory descriptor, we need to account for the
    // PteAdjust as well, which is included in rootOffset. We don't want to
    // account for it in the iovaArray because it is not accounted for in the
    // memdesc's PTE array. This should result in a 4K-aligned root offset.
    //
    rootOffset -= pPhysMemDesc->PteAdjust;
    NV_ASSERT((rootOffset & RM_PAGE_MASK) == 0);

    //
    // For submemory descriptors, there are two possibilities:
    //  (1) The root descriptor already has an IOVA mapping for the entire
    //      allocation in this IOVA space, in which case we just need a subset
    //      of that.
    //  (2) The root descriptor does not have an IOVA mapping for any of the
    //      allocation in this IOVA space, in which case we need to create one
    //      first.
    //

    pSubMapping = _iovaspaceCreateMappingDataFromMemDesc(pPhysMemDesc);
    if (pSubMapping == NULL)
    {
        iovaspaceReleaseMapping(pIOVAS, pRootIovaMapping);
        return NV_ERR_NO_MEMORY;
    }

    pSubMapping->refcount = 1;
    pSubMapping->iovaspaceId = pRootIovaMapping->iovaspaceId;
    pSubMapping->link.pParent = pRootIovaMapping;

    pSubMapping->pNext = pRootIovaMapping->link.pChildren;
    pRootIovaMapping->link.pChildren = pSubMapping;

    //
    // We need to copy over the corresponding entries from the root IOVA
    // mapping before we assign it to the physical memdesc. The root offset
    // determines where in the root mapping we need to start.
    //
    if (memdescGetContiguity(pPhysMemDesc, AT_CPU))
    {
        pSubMapping->iovaArray[0] = pRootIovaMapping->iovaArray[0] + rootOffset;
    }
    else
    {
        NvU64 i, j;
        NV_ASSERT(((rootOffset >> RM_PAGE_SHIFT) + pPhysMemDesc->PageCount) <=
                    pRootMemDesc->PageCount);
        for (i = (rootOffset >> RM_PAGE_SHIFT), j = 0;
             j < pPhysMemDesc->PageCount && i < pRootMemDesc->PageCount; i++, j++)
        {
            pSubMapping->iovaArray[j] = pRootIovaMapping->iovaArray[i];
        }
    }

    memdescAddIommuMap(pPhysMemDesc, pSubMapping);

    ++pIOVAS->mappingCount;

    return NV_OK;
}

static void
_iovaspaceDestroySubmapping
(
    OBJIOVASPACE *pIOVAS,
    PIOVAMAPPING pIovaMapping
)
{
    PMEMORY_DESCRIPTOR pPhysMemDesc = pIovaMapping->pPhysMemDesc;
    PIOVAMAPPING pRootIovaMapping = pIovaMapping->link.pParent;
    PIOVAMAPPING pTmpIovaMapping = pRootIovaMapping->link.pChildren;

    memdescRemoveIommuMap(pPhysMemDesc, pIovaMapping);

    if (pTmpIovaMapping == pIovaMapping)
    {
        pRootIovaMapping->link.pChildren = pIovaMapping->pNext;
    }
    else
    {
        while (pTmpIovaMapping != NULL && pTmpIovaMapping->pNext != pIovaMapping)
        {
            pTmpIovaMapping = pTmpIovaMapping->pNext;
        }

        if (pTmpIovaMapping != NULL)
        {
            pTmpIovaMapping->pNext = pIovaMapping->pNext;
        }
        else
        {
            // Not found in the root submappings list?
            NV_ASSERT(pTmpIovaMapping != NULL);
        }
    }

    portMemFree(pIovaMapping);
    --pIOVAS->mappingCount;

    //
    // After destroying a submapping, release its reference on the root mapping.
    // The reference was acquired in _iovaspaceCreateSubmapping().
    //
    iovaspaceReleaseMapping(pIOVAS, pRootIovaMapping);
}

static NV_STATUS
_iovaspaceCreateMapping
(
    OBJIOVASPACE *pIOVAS,
    PMEMORY_DESCRIPTOR pPhysMemDesc
)
{
    NV_STATUS status;
    OBJVASPACE *pVAS = staticCast(pIOVAS, OBJVASPACE);
    NV_ADDRESS_SPACE addressSpace;
    PIOVAMAPPING pIovaMapping = NULL;
    OBJGPU *pMappingGpu = NULL;

    //
    // The source memdesc has to be allocated to acquire an I/O VA space
    // mapping, because the OS layer will be setting up a layer of indirection
    // that assumes the PTEs in the memdesc are valid. There is no requirement
    // that it be mapped to the CPU at this point.
    //
    if (pPhysMemDesc == NULL)
    {
        NV_ASSERT(pPhysMemDesc != NULL);
        return NV_ERR_INVALID_ARGUMENT;
    }

    pMappingGpu = gpumgrGetGpuFromId(pVAS->vaspaceId);
    addressSpace = memdescGetAddressSpace(pPhysMemDesc);

    // Only support SYSMEM or indirect peer mappings
    if ((addressSpace != ADDR_SYSMEM) &&
         !gpumgrCheckIndirectPeer(pMappingGpu, pPhysMemDesc->pGpu))
    {
        NV_ASSERT(0);
        return NV_ERR_INVALID_STATE;
    }

    pIovaMapping = _iovaspaceCreateMappingDataFromMemDesc(pPhysMemDesc);
    if (pIovaMapping == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    // Initialize the mapping as an identity mapping for the OS layer
    if (memdescGetContiguity(pPhysMemDesc, AT_CPU))
    {
        pIovaMapping->iovaArray[0] = memdescGetPte(pPhysMemDesc, AT_CPU, 0);
    }
    else
    {
        NvU32 i;
        for (i = 0; i < pPhysMemDesc->PageCount; i++)
        {
            pIovaMapping->iovaArray[i] = memdescGetPte(pPhysMemDesc, AT_CPU, i);
        }
    }

    pIovaMapping->iovaspaceId = pVAS->vaspaceId;
    pIovaMapping->refcount = 1;

    status = osIovaMap(pIovaMapping);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed to map memdesc into I/O VA space 0x%x (status = 0x%x)\n",
                  pVAS->vaspaceId, status);
        goto error;
    }

    memdescAddIommuMap(pPhysMemDesc, pIovaMapping);
    ++pIOVAS->mappingCount;

    return NV_OK;

error:
    portMemFree(pIovaMapping);
    pIovaMapping = NULL;

    return status;
}

NV_STATUS
iovaspaceAcquireMapping_IMPL
(
    OBJIOVASPACE *pIOVAS,
    PMEMORY_DESCRIPTOR pPhysMemDesc
)
{
    OBJVASPACE *pVAS = staticCast(pIOVAS, OBJVASPACE);
    PIOVAMAPPING pIovaMapping = memdescGetIommuMap(pPhysMemDesc, pVAS->vaspaceId);

    if (pIovaMapping)
    {
        // If the mapping is already there, just increment its refcount.
        NV_ASSERT(pIovaMapping->refcount != 0);
        ++pIovaMapping->refcount;
        return NV_OK;
    }

    if (memdescIsSubMemoryMemDesc(pPhysMemDesc))
        return _iovaspaceCreateSubmapping(pIOVAS, pPhysMemDesc);
    else
        return _iovaspaceCreateMapping(pIOVAS, pPhysMemDesc);
}

static void
_iovaspaceDestroyRootMapping
(
    OBJIOVASPACE *pIOVAS,
    PIOVAMAPPING pIovaMapping
)
{
    PMEMORY_DESCRIPTOR pPhysMemDesc = pIovaMapping->pPhysMemDesc;
    PIOVAMAPPING pNextIovaMapping, pTmpIovaMapping;

    //
    // Increment the refcount to guarantee that destroying the last submapping
    // won't end up trying to destroy the root mapping we are already
    // destroying.
    //
    ++pIovaMapping->refcount;

    //
    // Clear out any submappings underneath this mapping, since they will no
    // longer be valid.
    //
    pNextIovaMapping = pIovaMapping->link.pChildren;
    while (pNextIovaMapping != NULL)
    {
        pTmpIovaMapping = pNextIovaMapping->pNext;
        _iovaspaceDestroySubmapping(pIOVAS, pNextIovaMapping);
        pNextIovaMapping = pTmpIovaMapping;
    }

    memdescRemoveIommuMap(pPhysMemDesc, pIovaMapping);

    osIovaUnmap(pIovaMapping);
    portMemFree(pIovaMapping);

    --pIOVAS->mappingCount;
}

void
iovaspaceDestroyMapping_IMPL
(
    OBJIOVASPACE *pIOVAS,
    PIOVAMAPPING pIovaMapping
)
{
    if (memdescIsSubMemoryMemDesc(pIovaMapping->pPhysMemDesc))
        _iovaspaceDestroySubmapping(pIOVAS, pIovaMapping);
    else
        _iovaspaceDestroyRootMapping(pIOVAS, pIovaMapping);
}

void
iovaspaceReleaseMapping_IMPL
(
    OBJIOVASPACE *pIOVAS,
    PIOVAMAPPING pIovaMapping
)
{
    if (pIovaMapping == NULL)
    {
        NV_ASSERT(0);
        return;
    }

    if (pIovaMapping->refcount == 0)
        NV_ASSERT(pIovaMapping->refcount > 0);

    if (--pIovaMapping->refcount != 0)
        return;

    iovaspaceDestroyMapping(pIOVAS, pIovaMapping);
}

OBJIOVASPACE *iovaspaceFromId(NvU32 iovaspaceId)
{
    OBJVASPACE *pVAS;
    OBJVMM     *pVmm = SYS_GET_VMM(SYS_GET_INSTANCE());
    NV_STATUS   status = vmmGetVaspaceFromId(pVmm, iovaspaceId, IO_VASPACE_A, &pVAS);

    if (status != NV_OK)
        return NULL;

    return dynamicCast(pVAS, OBJIOVASPACE);
}

OBJIOVASPACE *iovaspaceFromMapping(PIOVAMAPPING pIovaMapping)
{
    OBJIOVASPACE *pIOVAS = iovaspaceFromId(pIovaMapping->iovaspaceId);

    //
    // The IOVASPACE has to be there as the mapping is referencing it. If it's
    // not, the mapping has been left dangling outlasting the IOVAS it was
    // under.
    //
    NV_ASSERT(pIOVAS != NULL);

    return pIOVAS;
}

void iovaMappingDestroy(PIOVAMAPPING pIovaMapping)
{
    OBJIOVASPACE *pIOVAS = iovaspaceFromMapping(pIovaMapping);

    NV_ASSERT_OR_RETURN_VOID(pIOVAS != NULL);
    iovaspaceDestroyMapping(pIOVAS, pIovaMapping);
}

#endif // (RMCFG_FEATURE_PLATFORM_UNIX || RMCFG_FEATURE_MODS_FEATURES) && !NVCPU_IS_ARM
