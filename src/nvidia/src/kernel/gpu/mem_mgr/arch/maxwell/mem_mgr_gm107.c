/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/gpu.h"
#include "os/os.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/heap.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/bus/kern_bus.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

#include "gpu/bif/kernel_bif.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/disp/inst_mem/disp_inst_mem.h"

#include "rmifrif.h"

#include "published/maxwell/gm107/dev_mmu.h"
#include "published/maxwell/gm107/dev_ram.h"

#include "vgpu/rpc.h"
#include "vgpu/vgpu_events.h"

//
// statics
//
static NV_STATUS memmgrComputeAndSetVgaDisplayMemoryBase_GM107(OBJGPU *, NvU64);

NvU32
memmgrChooseKindCompressC_GM107
(
    OBJGPU                 *pGpu,
    MemoryManager          *pMemoryManager,
    FB_ALLOC_PAGE_FORMAT   *pFbAllocPageFormat)
{
    NvU32  kind         = NV_MMU_PTE_KIND_PITCH;
    NvU32  attrdepth    = DRF_VAL(OS32, _ATTR, _DEPTH, pFbAllocPageFormat->attr);
    NvU32  aasamples    = DRF_VAL(OS32, _ATTR, _AA_SAMPLES, pFbAllocPageFormat->attr);
    NvBool prefer_zbc   = !FLD_TEST_DRF(OS32, _ATTR2, _ZBC, _PREFER_NO_ZBC, pFbAllocPageFormat->attr2);

    switch (attrdepth)
    {
        case NVOS32_ATTR_DEPTH_UNKNOWN:
        case NVOS32_ATTR_DEPTH_8:
        case NVOS32_ATTR_DEPTH_16:
            kind = NV_MMU_PTE_KIND_GENERIC_16BX2;
            break;
        case NVOS32_ATTR_DEPTH_32:
            switch (aasamples)
            {
                case NVOS32_ATTR_AA_SAMPLES_1:
                    kind = NV_MMU_PTE_KIND_C32_2CRA;
                    break;
                case NVOS32_ATTR_AA_SAMPLES_2:
                    kind = memmgrChooseKindCompressCForMS2_HAL(pGpu, pMemoryManager, attrdepth);
                    break;
                case NVOS32_ATTR_AA_SAMPLES_4:
                case NVOS32_ATTR_AA_SAMPLES_4_ROTATED:
                case NVOS32_ATTR_AA_SAMPLES_4_VIRTUAL_8:
                case NVOS32_ATTR_AA_SAMPLES_4_VIRTUAL_16:
                    kind = prefer_zbc? NV_MMU_PTE_KIND_C32_MS4_2CBR : NV_MMU_PTE_KIND_C32_MS4_2BRA;
                    break;
                case NVOS32_ATTR_AA_SAMPLES_8:
                case NVOS32_ATTR_AA_SAMPLES_16:
                case NVOS32_ATTR_AA_SAMPLES_8_VIRTUAL_16:
                case NVOS32_ATTR_AA_SAMPLES_8_VIRTUAL_32:
                    kind = NV_MMU_PTE_KIND_C32_MS8_MS16_2CRA;
                    break;
            }
            break;
        case NVOS32_ATTR_DEPTH_64:
            switch (aasamples)
            {
                case NVOS32_ATTR_AA_SAMPLES_1:
                    kind = NV_MMU_PTE_KIND_C64_2CRA;
                    break;
                case NVOS32_ATTR_AA_SAMPLES_2:
                    kind = memmgrChooseKindCompressCForMS2_HAL(pGpu, pMemoryManager, attrdepth);
                    break;
                case NVOS32_ATTR_AA_SAMPLES_4:
                case NVOS32_ATTR_AA_SAMPLES_4_ROTATED:
                case NVOS32_ATTR_AA_SAMPLES_4_VIRTUAL_8:
                case NVOS32_ATTR_AA_SAMPLES_4_VIRTUAL_16:
                    kind = prefer_zbc? NV_MMU_PTE_KIND_C64_MS4_2CBR : NV_MMU_PTE_KIND_C64_MS4_2BRA;
                    break;
                case NVOS32_ATTR_AA_SAMPLES_8:
                case NVOS32_ATTR_AA_SAMPLES_16:
                case NVOS32_ATTR_AA_SAMPLES_8_VIRTUAL_16:
                case NVOS32_ATTR_AA_SAMPLES_8_VIRTUAL_32:
                    kind = NV_MMU_PTE_KIND_C64_MS8_MS16_2CRA;
                    break;
            }
            break;
        case NVOS32_ATTR_DEPTH_128:
            switch (aasamples)
            {
                case NVOS32_ATTR_AA_SAMPLES_1:
                    kind = NV_MMU_PTE_KIND_C128_2CR;
                    break;
                case NVOS32_ATTR_AA_SAMPLES_2:
                    kind = NV_MMU_PTE_KIND_C128_MS2_2CR;
                    break;
                case NVOS32_ATTR_AA_SAMPLES_4:
                case NVOS32_ATTR_AA_SAMPLES_4_ROTATED:
                case NVOS32_ATTR_AA_SAMPLES_4_VIRTUAL_8:
                case NVOS32_ATTR_AA_SAMPLES_4_VIRTUAL_16:
                    kind = NV_MMU_PTE_KIND_C128_MS4_2CR;
                    break;
                case NVOS32_ATTR_AA_SAMPLES_8:
                case NVOS32_ATTR_AA_SAMPLES_16:
                case NVOS32_ATTR_AA_SAMPLES_8_VIRTUAL_16:
                case NVOS32_ATTR_AA_SAMPLES_8_VIRTUAL_32:
                    kind = NV_MMU_PTE_KIND_C128_MS8_MS16_2CR;
                    break;
            }
            break;
    }

    return kind;
}

NV_STATUS
memmgrAllocDetermineAlignment_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU64         *pMemSize,
    NvU64         *pAlign,
    NvU64          alignPad,
    NvU32          allocFlags,
    NvU32          retAttr,
    NvU32          retAttr2,
    NvU64          hwAlignment
)
{
    KernelGmmu  *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    switch (dmaNvos32ToPageSizeAttr(retAttr, retAttr2))
    {
        case RM_ATTR_PAGE_SIZE_4KB:
            hwAlignment = NV_MAX(hwAlignment, RM_PAGE_SIZE - 1);
            break;
        case RM_ATTR_PAGE_SIZE_BIG:
            // we will always align to the biggest page size
            hwAlignment = NV_MAX(hwAlignment, kgmmuGetMaxBigPageSize_HAL(pKernelGmmu) - 1);
            break;
        case RM_ATTR_PAGE_SIZE_HUGE:
            NV_ASSERT_OR_RETURN(kgmmuIsHugePageSupported(pKernelGmmu),
                              NV_ERR_INVALID_ARGUMENT);
            hwAlignment = NV_MAX(hwAlignment, RM_PAGE_SIZE_HUGE - 1);
            break;
        case RM_ATTR_PAGE_SIZE_512MB:
            NV_ASSERT_OR_RETURN(kgmmuIsPageSize512mbSupported(pKernelGmmu),
                              NV_ERR_INVALID_ARGUMENT);
            hwAlignment = NV_MAX(hwAlignment, RM_PAGE_SIZE_512M - 1);
            break;
        case RM_ATTR_PAGE_SIZE_DEFAULT:
        case RM_ATTR_PAGE_SIZE_INVALID:
            NV_PRINTF(LEVEL_ERROR, "- invalid page size specified\n");
            return NV_ERR_INVALID_ARGUMENT;
    }

    if (!FLD_TEST_DRF(OS32, _ATTR, _COMPR, _NONE, retAttr))
    {
        if (FLD_TEST_DRF(OS32, _ATTR, _PAGE_SIZE, _4KB, retAttr) &&
            !(allocFlags & NVOS32_ALLOC_FLAGS_VIRTUAL) &&
            pMemoryManager->bSmallPageCompression)
        {
            //
            // No offset alignment requirement for 4KB compression.
            // The size should be aligned to compression pagesize.
            //
            NvU64 comprPageSize = pMemorySystemConfig->comprPageSize;
            *pMemSize = ((*pMemSize + alignPad + comprPageSize - 1) / comprPageSize) * comprPageSize;
        }
        else
        {
            // Both size and  offset should be aligned to compression pagesize.
            hwAlignment = NV_MAX(hwAlignment, pMemorySystemConfig->comprPageSize - 1);

            if (FLD_TEST_DRF(OS32, _ATTR, _PAGE_SIZE, _4KB, retAttr) &&
                !pMemoryManager->bSmallPageCompression)
            {
                NV_PRINTF(LEVEL_INFO,
                          "Compression requested on small page size mappings\n");
            }
        }
    }

    // a non-zero alignment means it's a requested alignment. Ensure the requested
    // alignment is still aligned to the hw requirements
    if ((*pAlign) &&
        (((*pAlign > hwAlignment) && !(*pAlign % (hwAlignment+1))) ||   // align is >=1 multiple of hwAlignment
         ((*pAlign <= hwAlignment+1) && !((hwAlignment+1) % *pAlign)))) // hwAlignment is a >= mulitple of align
    {
        if ( *pAlign <= hwAlignment+1 )
        {
            *pAlign = hwAlignment + 1;
        }

        (*pAlign)--;  // convert to (alignment-1) (not really a "mask")
        // calculate the new size based on hw alignment
        *pMemSize = ((*pMemSize + alignPad + hwAlignment) / (hwAlignment+1)) * (hwAlignment+1);
        hwAlignment = *pAlign; // this aligns the offset to the requested alignment
    }
    else
    {
        // if this alignment was a force or fail, fail it here
        if (*pAlign != 0 && (allocFlags & NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE))
        {
            *pAlign = 0;
            return (NV_ERR_INVALID_ARGUMENT);
        }

        //
        // By default round to the hw alignment.  It is important to pad to the page size
        // on fermi for all allocations as we cannot mix page size on the same physical
        // memory due to page swizzle.
        //
        *pAlign = hwAlignment;
        *pMemSize = ((*pMemSize + alignPad + hwAlignment) / (hwAlignment+1)) * (hwAlignment+1);
    }

    return NV_OK;
}

static void
memmgrSetZbcReferenced
(
    OBJGPU *pGpu,
    NvBool  bZbcSurfacesExist
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS params = {0};

    // Allocations are RPCed to host, so they are counted there
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
        return;

    params.bZbcSurfacesExist = bZbcSurfacesExist;
    NV_ASSERT_OK(
        pRmApi->Control(
            pRmApi,
            pGpu->hInternalClient,
            pGpu->hInternalSubdevice,
            NV2080_CTRL_CMD_INTERNAL_MEMSYS_SET_ZBC_REFERENCED,
            &params,
            sizeof(params)));
}

//
// Update user alloc request parameter according to memory
// type and (possibly) reserve hw resources.
//
NV_STATUS
memmgrAllocHal_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    FB_ALLOC_INFO *pFbAllocInfo
)
{
    KernelMemorySystem *pKernelMemorySystem   = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NV_STATUS           status                = NV_OK;
    NvU32               comprAttr, tiledAttr, zcullAttr, type;
    NvU32               cacheAttr;
    NvU32               format, kind, bAlignPhase;
    NvU32               retAttr               = pFbAllocInfo->retAttr;
    NvU32               retAttr2              = pFbAllocInfo->retAttr2;
    NV_ADDRESS_SPACE    addrSpace;
    NvBool              bComprWar            = NV_FALSE;
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, pKernelMemorySystem);

    // get the specified attribute values
    comprAttr     = DRF_VAL(OS32, _ATTR, _COMPR, pFbAllocInfo->pageFormat->attr);
    tiledAttr     = DRF_VAL(OS32, _ATTR, _TILED, pFbAllocInfo->pageFormat->attr);
    zcullAttr     = DRF_VAL(OS32, _ATTR, _ZCULL, pFbAllocInfo->pageFormat->attr);
    format        = DRF_VAL(OS32, _ATTR, _FORMAT, pFbAllocInfo->pageFormat->attr);
    cacheAttr     = DRF_VAL(OS32, _ATTR2, _GPU_CACHEABLE, pFbAllocInfo->pageFormat->attr2);
    type          = pFbAllocInfo->pageFormat->type;
    addrSpace     = memmgrAllocGetAddrSpace(pMemoryManager, pFbAllocInfo->pageFormat->flags, retAttr);

    if ( NVOS32_ATTR_LOCATION_AGP == DRF_VAL(OS32, _ATTR, _LOCATION, pFbAllocInfo->pageFormat->attr) )
        return NV_ERR_NOT_SUPPORTED; // only local vid & pci (sysmem) supported

    bAlignPhase = !!(pFbAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_SKIP_RESOURCE_ALLOC);

    //
    // errorcheck specified attributes
    // NOTE: With the new macro, the distinction between 32-bit colour
    // compression and Z compression is in the value of 'type' - DEPTH or IMAGE
    // So the caller is urged to verify integrity.
    //
    if (
        // Tiling is not supported in nv50+
        (tiledAttr == NVOS32_ATTR_TILED_REQUIRED) ||
        (tiledAttr == NVOS32_ATTR_TILED_DEFERRED) ||
        // check the value of compression attribute
        // attributes verification for compressed surfaces
        !(memmgrVerifyComprAttrs_HAL(pMemoryManager, type, format, comprAttr)) ||
        // depth buffer attribute verification
        !(memmgrVerifyDepthSurfaceAttrs_HAL(pMemoryManager, type, format))
        || (zcullAttr == NVOS32_ATTR_ZCULL_REQUIRED) || (zcullAttr == NVOS32_ATTR_ZCULL_SHARED)
        )
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Fermi does not support tiling
    retAttr = FLD_SET_DRF(OS32, _ATTR, _TILED, _NONE, retAttr);

    if (cacheAttr == NVOS32_ATTR2_GPU_CACHEABLE_DEFAULT)
    {
        //
        // The GPU cache is not sysmem coherent. Caching sysmem in GPU cache requires
        // clients to issue GPU cache invalidates to maintain coherency.
        //
        if (addrSpace == ADDR_SYSMEM)
        {
            retAttr2 = FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _NO, retAttr2);
        }
        else
        {
            retAttr2 = FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _YES, retAttr2);
        }
    }

    if (!FLD_TEST_DRF(OS32, _ATTR, _COMPR, _NONE, retAttr))
    {
        if (pMemorySystemConfig->bDisableCompbitBacking)
        {
            NV_PRINTF(LEVEL_INFO, "compression disabled due to regkey\n");
            retAttr = FLD_SET_DRF(OS32, _ATTR, _COMPR, _NONE, retAttr);
        }
        else if (!memmgrComprSupported(pMemoryManager, addrSpace))
        {
            if (FLD_TEST_DRF(OS32, _ATTR, _COMPR, _REQUIRED, retAttr))
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Compression not supported for this configuration.\n");
                return NV_ERR_NOT_SUPPORTED;
            }
            else
            {
                retAttr = FLD_SET_DRF(OS32, _ATTR, _COMPR, _NONE, retAttr);
            }
        }
    }

    {
        status = memmgrChooseKind_HAL(pGpu, pMemoryManager, pFbAllocInfo->pageFormat,
                 DRF_VAL(OS32, _ATTR, _COMPR, retAttr), &kind);
        if (status != NV_OK)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    pFbAllocInfo->pageFormat->kind = kind;

    //
    // See Bug 351429: It should not be an error to specify an uncompressible kind
    // via -pte_kind[CZ] should be sufficient even if -compress[CZ] not specified
    //
    if(!memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, kind))
    {
        retAttr = FLD_SET_DRF(OS32, _ATTR, _COMPR, _NONE, retAttr);
    }

    // Ideally compression should only be enabled on big/huge page mapping
    if (FLD_TEST_DRF(OS32, _ATTR, _PAGE_SIZE, _4KB, retAttr) &&
        !FLD_TEST_DRF(OS32, _ATTR, _COMPR, _NONE, retAttr)   &&
        !pMemoryManager->bSmallPageCompression)
    {
        if (FLD_TEST_DRF(OS32, _ATTR, _COMPR, _REQUIRED, retAttr))
        {
            // We allow in MODS due to test requirement.
            if (!RMCFG_FEATURE_PLATFORM_MODS)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "ERROR: Compression requested for small page allocation.\n");
                return NV_ERR_NOT_SUPPORTED;
            }
        }
        else
        {
            if (RMCFG_FEATURE_PLATFORM_MODS)
            {
                retAttr = FLD_SET_DRF(OS32, _ATTR, _COMPR, _NONE, retAttr);
                if(memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, kind))
                {
                    kind = memmgrGetUncompressedKind_HAL(pGpu, pMemoryManager, kind, NV_FALSE);
                    pFbAllocInfo->pageFormat->kind = kind;
                }
            }
            else
            {
                bComprWar = NV_TRUE;
            }
        }
    }

    // Allocate zcull before we save the pitch and size
    pFbAllocInfo->hwResId = 0;

    //
    // Attempt to allocate ctags to go with this allocation.
    // Note: The way things work we're actually allocating ctags for a region
    // which hasn't been allocated yet.  We only know the size the region will be.
    // Later we'll get a call to bind (fbsetallocparams).  But, this fbsetallocparams
    // call isn't late enough!  We need a contextdma to actually bind with!
    // So we have to keep track of the allocation by creating a marker for it and using
    // the hwresid to invoke it later :(
    //
    if (!FLD_TEST_DRF(OS32, _ATTR, _COMPR, _NONE, retAttr))
    {
        if (!bAlignPhase && !bComprWar &&
            !(pFbAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_VIRTUAL))
        {
            status = kmemsysAllocComprResources_HAL(pGpu, pKernelMemorySystem, pFbAllocInfo,
                                                    pFbAllocInfo->origSize, 1, &retAttr, retAttr2);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "memsysAllocComprResources failed\n");

                return status;
            }
        }
    }
    //
    // !!WARNING!!!
    //
    // This flag is introduced as a temporary WAR to enable color compression
    // without ZBC. RM will skip refcounting the ZBC table when this flag is set.
    // PTE Kind could still support ZBC (there are sometimes no non-zbc equivalent)
    // Hence UMD has to disable zbc for the app by masking all the zbc slots.
    // It's a temporary WAR until we implement per process zbc slot management.
    //
    if (FLD_TEST_DRF(OS32, _ATTR2, _ZBC_SKIP_ZBCREFCOUNT, _NO, pFbAllocInfo->pageFormat->attr2))
    {
        if (
            !IS_MIG_ENABLED(pGpu) &&
            memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_ZBC, pFbAllocInfo->pageFormat->kind) &&
            !(pFbAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_VIRTUAL))
        {
            retAttr2 = FLD_SET_DRF(OS32, _ATTR2, _ZBC, _PREFER_ZBC, retAttr2);
            if (!bAlignPhase)
            {
                pMemoryManager->zbcSurfaces++;
                NV_PRINTF(LEVEL_INFO,
                          "zbcSurfaces = 0x%x, hwResId = 0x%x\n",
                          pMemoryManager->zbcSurfaces, pFbAllocInfo->hwResId);

                if (pMemoryManager->zbcSurfaces == 1)
                    memmgrSetZbcReferenced(pGpu, NV_TRUE);
            }
        }
        else
        {
            retAttr2 = FLD_SET_DRF(OS32, _ATTR2, _ZBC, _PREFER_NO_ZBC, retAttr2);
        }
    }
    else
    {
        NV_ASSERT(FLD_TEST_DRF(OS32, _ATTR2, _ZBC, _PREFER_NO_ZBC, retAttr2));
    }

    pFbAllocInfo->format = kind;
    pFbAllocInfo->retAttr = retAttr;
    pFbAllocInfo->retAttr2 = retAttr2;

    return (NV_OK);
}

NV_STATUS
memmgrSetAllocParameters_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    FB_ALLOC_INFO *pFbAllocInfo
)
{

    return NV_OK;
}

//
// Release tile back to the free pool.
//
NV_STATUS
memmgrFreeHal_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    FB_ALLOC_INFO *pFbAllocInfo,
    PRMTIMEOUT     pTimeout
)
{
    NvU32 commitResId = pFbAllocInfo->hwResId;

    if (pFbAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_SKIP_RESOURCE_ALLOC)
    {
        // for vGPU, we set this flag in memmgrAllocHwResources
        return NV_OK;
    }

    // We might want to move this check to higher-level
    if (IS_MIG_ENABLED(pGpu))
    {
        // In SMC mode, we do not program ZCULL or ZBC
        return NV_OK;
    }

    kmemsysFreeComprResources_HAL(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu), commitResId);

    if (FLD_TEST_DRF(OS32, _ATTR2, _ZBC_SKIP_ZBCREFCOUNT, _NO, pFbAllocInfo->pageFormat->attr2) &&
        memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_ZBC, pFbAllocInfo->format))
    {
        NV_ASSERT(pMemoryManager->zbcSurfaces !=0 );
        if (pMemoryManager->zbcSurfaces != 0)
        {
            pMemoryManager->zbcSurfaces--;

            if (pMemoryManager->zbcSurfaces == 0)
                memmgrSetZbcReferenced(pGpu, NV_FALSE);
        }

        NV_PRINTF(LEVEL_INFO,
                  "[1] hwResId = 0x%x, offset = 0x%llx, size = 0x%llx\n",
                  pFbAllocInfo->hwResId, pFbAllocInfo->offset,
                  pFbAllocInfo->size);

        NV_PRINTF(LEVEL_INFO, "[2] zbcSurfaces = 0x%x\n",
                  pMemoryManager->zbcSurfaces);
    }

    return NV_OK;
}

NV_STATUS
memmgrGetSurfacePhysAttr_GM107
(
    OBJGPU           *pGpu,
    MemoryManager    *pMemoryManager,
    Memory           *pMemory,
    NvU64            *pOffset,
    NvU32            *pMemAperture,
    NvU32            *pMemKind,
    NvU32            *pComprOffset,
    NvU32            *pComprKind,
    NvU32            *pLineMin,
    NvU32            *pLineMax,
    NvU32            *pZCullId,
    NvU32            *pGpuCacheAttr,
    NvU32            *pGpuP2PCacheAttr,
    NvU64            *contigSegmentSize
)
{
    NV_STATUS                   rmStatus;
    PMEMORY_DESCRIPTOR          pMemDesc      = memdescGetMemDescFromGpu(pMemory->pMemDesc, pGpu);
    COMPR_INFO                  comprInfo;
    NvU32                       unused;

    NV_ASSERT(pMemDesc);

    rmStatus = memmgrFillMemdescForPhysAttr(pGpu, pMemoryManager, pMemDesc, AT_GPU, pOffset, pMemAperture,
                                            pMemKind, pZCullId, pGpuCacheAttr, pGpuP2PCacheAttr,
                                            contigSegmentSize);
    if (NV_OK != rmStatus)
    {
        return rmStatus;
    }

    if ((!memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, *pMemKind)) ||
         !FB_HWRESID_CTAGID_VAL_FERMI(memdescGetHwResId(pMemDesc)))
    {
        *pComprKind = 0;
        return NV_OK;
    }
    // vGPU: pPrivate->pCompTags is not
    // currently initialized in the guest RM
    // vGPU does not use compression tags yet.
    // GSPTODO: sort out ctags
    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        *pComprOffset = 0x0;
        *pLineMin = 0x0;
        *pLineMax = 0x0;
        return NV_OK;
    }

    rmStatus = memmgrGetKindComprFromMemDesc(pMemoryManager, pMemDesc, 0, &unused, &comprInfo);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "dmaGetKidnCompr failed: %x\n", rmStatus);
        return rmStatus;
    }

    if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, comprInfo.kind))
    {
        *pLineMin = comprInfo.compTagLineMin;
        *pLineMax = comprInfo.compPageIndexHi - comprInfo.compPageIndexLo + comprInfo.compTagLineMin;
        *pComprOffset = comprInfo.compPageIndexLo;
        *pComprKind = 1;
    }
    else
    {
        // No coverage at all (stripped by release/reacquire or invalid hw res).
        *pLineMin = ~0;
        *pLineMax = ~0;
        *pComprKind = 0;
    }

    return NV_OK;
}

NV_STATUS
memmgrGetBAR1InfoForDevice_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    Device        *pDevice,
    PGETBAR1INFO   bar1Info
)
{
    KernelBus     *pKernelBus   = GPU_GET_KERNEL_BUS(pGpu);
    NvU64          largestFreeSize;
    NvU64          freeSize;
    OBJVASPACE    *pBar1VAS;
    OBJEHEAP      *pVASHeap;
    NV_RANGE       bar1VARange = NV_RANGE_EMPTY;
    RsClient      *pClient = RES_GET_CLIENT(pDevice);

    /*
     * For legacy vGPU and SRIOV heavy, get BAR1 information from vGPU plugin.
     */
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        NV_STATUS status = NV_OK;
        NV2080_CTRL_FB_GET_INFO_V2_PARAMS fbInfoParams = {0};
        Subdevice *pSubdevice;

        NV_ASSERT_OK_OR_RETURN(subdeviceGetByGpu(pClient, pGpu, &pSubdevice));

        fbInfoParams.fbInfoList[0].index = NV2080_CTRL_FB_INFO_INDEX_BAR1_SIZE;
        fbInfoParams.fbInfoList[1].index = NV2080_CTRL_FB_INFO_INDEX_BAR1_AVAIL_SIZE;
        fbInfoParams.fbInfoList[2].index = NV2080_CTRL_FB_INFO_INDEX_BAR1_MAX_CONTIGUOUS_AVAIL_SIZE;
        fbInfoParams.fbInfoList[3].index = NV2080_CTRL_FB_INFO_INDEX_BANK_SWIZZLE_ALIGNMENT;

        fbInfoParams.fbInfoListSize = 4;

        NV_RM_RPC_CONTROL(pGpu, pClient->hClient, RES_GET_HANDLE(pSubdevice),
                          NV2080_CTRL_CMD_FB_GET_INFO_V2,
                          &fbInfoParams, sizeof(fbInfoParams),
                          status);
        if (status == NV_OK) {
            bar1Info->bar1Size               = fbInfoParams.fbInfoList[0].data;
            bar1Info->bar1AvailSize          = fbInfoParams.fbInfoList[1].data;
            bar1Info->bar1MaxContigAvailSize = fbInfoParams.fbInfoList[2].data;
            bar1Info->bankSwizzleAlignment   = fbInfoParams.fbInfoList[3].data;
        }
        return status;
    }

    if (!KBUS_CPU_VISIBLE_BAR12_DISABLED(pGpu))
    {
        pBar1VAS = kbusGetBar1VASpace_HAL(pGpu, pKernelBus);
        NV_ASSERT_OR_RETURN(pBar1VAS != NULL, NV_ERR_INVALID_STATE);
        pVASHeap = vaspaceGetHeap(pBar1VAS);

        NV_ASSERT_OK_OR_RETURN(kbusGetBar1VARangeForClient(pGpu, pKernelBus, pClient->hClient, &bar1VARange));
        bar1Info->bar1Size = (NvU32)(rangeLength(bar1VARange) / 1024);
        bar1Info->bankSwizzleAlignment = vaspaceGetBigPageSize(pBar1VAS);

        bar1Info->bar1AvailSize = 0;

        if (pVASHeap != NULL)
        {
            pVASHeap->eheapInfoForRange(pVASHeap, bar1VARange, NULL, &largestFreeSize, NULL, &freeSize);
            bar1Info->bar1AvailSize = (NvU32)(freeSize / 1024);
            bar1Info->bar1MaxContigAvailSize = (NvU32)(largestFreeSize / 1024);
        }
    }
    else
    {
        // When coherent C2C path is enabled, BAR1 is disabled
        bar1Info->bar1Size = 0;
        bar1Info->bar1AvailSize = 0;
        bar1Info->bar1MaxContigAvailSize = 0;
        bar1Info->bankSwizzleAlignment = 0;
    }
    return NV_OK;
}

NvU32
memmgrGetReservedHeapSizeMb_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NvU32  i;
    NvU64  rsvdSize = 0;

    // Display and tally the results to make sure the numbers add up.
    for (i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
    {
        rsvdSize += pMemoryManager->Ram.fbRegion[i].rsvdSize;

        if (pMemoryManager->Ram.fbRegion[i].rsvdSize > 0)
        {
            NV_PRINTF(LEVEL_INFO, "FB region #%d:rsvdSize=%d\n", i,
                      NvU64_LO32(pMemoryManager->Ram.fbRegion[i].rsvdSize));
        }
    }

    rsvdSize = rsvdSize / (1024 * 1024);   // convert byte to MB

    return (NvU64_LO32(rsvdSize));
}

/*!
 * @brief Set up additional RM reserved memory space for physical carveout.
 */
static void
memmgrStateInitReservedMemory
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    if (IS_GSP_CLIENT(pGpu))
        return;

}

/*!
 * @brief Correct RM reserved memory addresses by adding region base to them.
 *        Before this point, all reserved memory addresses started at 0.
 */
static NV_STATUS
memmgrStateInitAdjustReservedMemory
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{

    return NV_OK;
}

/*!
 * @brief Checks if the reserved memory size passed fits in the bar0 window
 *
 * @param[in]   rsvdMemorySizeBytes   The value to check against the bar0 size
 */
NV_STATUS
memmgrCheckReservedMemorySize_GK104
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NV_ASSERT_OR_RETURN(pMemoryManager->rsvdMemorySize < DRF_SIZE(NV_PRAMIN), NV_ERR_INSUFFICIENT_RESOURCES);

    return NV_OK;
}

/*!
 * @brief - This routine initializes the reserved video memory
 * regions specific to GPUs using this HAL entry point; size
 * arguments are in units of bytes.
 *
 * @param[in]  fbSize  The size of video memory
 */
NV_STATUS
memmgrInitReservedMemory_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU64          fbSize
)
{
    NvU64                  tmpAddr          = 0;
    NvU32                  i;
    NvBool                 bRsvdRegionIsValid = NV_FALSE;
    NvU32                  rsvdRegion       = 0;
    NvU64                  rsvdTopOfMem     = 0;
    NvU64                  rsvdAlignment    = 0;
    NvBool                 bMemoryProtectionEnabled = NV_FALSE;
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    if (!IS_VIRTUAL(pGpu) && !IS_GSP_CLIENT(pGpu))
    {
        if (memmgrComputeAndSetVgaDisplayMemoryBase_GM107(pGpu, fbSize) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to compute/set VGA display memory base!\n");
            DBG_BREAKPOINT();
        }
    }

    bMemoryProtectionEnabled = gpuIsCCFeatureEnabled(pGpu);

    memmgrStateInitReservedMemory(pGpu, pMemoryManager);

    // Align reserved memory to 64K granularity
    pMemoryManager->rsvdMemorySize = NV_ALIGN_UP(pMemoryManager->rsvdMemorySize, 0x10000);

    NV_PRINTF(LEVEL_INFO, "Final reserved memory size = 0x%x\n", pMemoryManager->rsvdMemorySize);

    if (!IS_VIRTUAL(pGpu))
    {
        //
        // Reserved memory must fit in BAR0 window - well compression backing is after this.
        // Does not matter for GSP itself as BAR0 is not used.
        //
        NV_ASSERT_OR_RETURN(RMCFG_FEATURE_PLATFORM_GSP ||
            memmgrCheckReservedMemorySize_HAL(pGpu, pMemoryManager) == NV_OK, NV_ERR_INSUFFICIENT_RESOURCES);
    }

    NV_PRINTF(LEVEL_INFO, "RESERVED Memory size: 0x%x\n", pMemoryManager->rsvdMemorySize);

    // ***************************************************************
    // Done sizing reserved memory
    // ***************************************************************

    if (pMemoryManager->Ram.numFBRegions > 0)
    {
        //
        // Find the last region in memory which is not already reserved or
        // protected.  RM's reserved memory will then be carved out of it below
        // (once the final size and address are determined).
        // RM internal data like BAR2 page tables, BAR1/2 instance blocks, etc should
        // always be in protected memory whenever memory protection is enabled using Hopper
        // Confidential Compute. For uses outside Hopper Confidential Compute, RM internal
        // data should always be in unprotected video memory.
        //
        for (i = 0; i < pMemoryManager->Ram.numFBRegions; i++)
        {
            if (pMemoryManager->Ram.fbRegion[i].bRsvdRegion ||
                (bMemoryProtectionEnabled && !pMemoryManager->Ram.fbRegion[i].bProtected) ||
                (!bMemoryProtectionEnabled && pMemoryManager->Ram.fbRegion[i].bProtected))
            {
                continue;
            }

            bRsvdRegionIsValid = NV_TRUE;
            rsvdRegion = i;
        }
    }


    // No need to create a reserved region for vGPU.
    // For vGPU, memory required for the host will be reserved separately.
    //
    if (IS_VIRTUAL(pGpu))
    {
        bRsvdRegionIsValid = NV_FALSE;
    }

    rsvdAlignment = RM_PAGE_SIZE;

    //
    // Generate the FB physical offset of reserved mem.
    //
    // In L2 cache only mode, base this off the size of L2 cache
    // If reserved memory at top of FB, base this off the size of FB
    //
    if (gpuIsCacheOnlyModeEnabled(pGpu) || !pMemorySystemConfig->bReservedMemAtBottom)
    {
        {
            rsvdTopOfMem = pMemoryManager->Ram.fbAddrSpaceSizeMb << 20;

            //
            // We are assuming that subheap is at the end of guest FB. We place
            // the guest RM reserved region at the end of the guest client owned
            // portion of the guest FB (total guest FB minus the subheap). The
            // guest FB is partitioned in the following way (Addresses increasing
            // from left to right).
            //
            //    Region 0                  Region 1                 Region 2
            // [Guest client owned FB] [Guest RM reserved region] [Guest subheap]
            //
            // Guest heap is created only for Region 0.
            //
            if (IS_VIRTUAL_WITH_SRIOV(pGpu))
                rsvdTopOfMem -= memmgrGetFbTaxSize_HAL(pGpu, pMemoryManager);

            if (bRsvdRegionIsValid)
            {
                rsvdTopOfMem = NV_MIN(pMemoryManager->Ram.fbRegion[rsvdRegion].limit + 1, rsvdTopOfMem);
            }
        }
        tmpAddr = rsvdTopOfMem - pMemoryManager->rsvdMemorySize;
        pMemoryManager->rsvdMemoryBase = RM_ALIGN_DOWN(tmpAddr, rsvdAlignment);
        pMemoryManager->rsvdMemorySize = NvU64_LO32(rsvdTopOfMem - pMemoryManager->rsvdMemoryBase);

        // make sure we didn't just blindly truncate that...
        NV_ASSERT(0 == NvU64_HI32(rsvdTopOfMem - pMemoryManager->rsvdMemoryBase));
    }
    // Reserved memory located at bottom of FB, base this at start of FB
    else
    {
        tmpAddr = pMemoryManager->heapStartOffset;
        if (bRsvdRegionIsValid)
        {
            tmpAddr = NV_MAX(pMemoryManager->Ram.fbRegion[rsvdRegion].base, tmpAddr);
        }
        pMemoryManager->rsvdMemoryBase = RM_ALIGN_UP(tmpAddr, rsvdAlignment);
        pMemoryManager->rsvdMemorySize = RM_PAGE_ALIGN_UP(pMemoryManager->rsvdMemorySize);
    }

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB))
    {
        NV_ASSERT(pMemoryManager->Ram.fbUsableMemSize >= pMemoryManager->rsvdMemorySize);
        pMemoryManager->Ram.fbUsableMemSize -= RM_PAGE_ALIGN_UP(pMemoryManager->rsvdMemorySize);
    }

    // Now update the region table to remove rsvd memory
    if (bRsvdRegionIsValid && pMemoryManager->rsvdMemorySize)
    {
        FB_REGION_DESCRIPTOR   rsvdFbRegion;
        portMemSet(&rsvdFbRegion, 0, sizeof(rsvdFbRegion));

        // Add a new region that is a hole for reserved memory
        rsvdFbRegion.bRsvdRegion = NV_TRUE;
        rsvdFbRegion.base = pMemoryManager->rsvdMemoryBase;
        rsvdFbRegion.limit =
            pMemoryManager->rsvdMemoryBase + pMemoryManager->rsvdMemorySize - 1;
        rsvdFbRegion.performance = 0;
        rsvdFbRegion.bSupportCompressed = NV_FALSE;
        rsvdFbRegion.bSupportISO = NV_FALSE;
        rsvdFbRegion.rsvdSize = pMemoryManager->rsvdMemorySize;
        rsvdFbRegion.bProtected = bMemoryProtectionEnabled;
        rsvdFbRegion.bInternalHeap = NV_TRUE;

        memmgrInsertFbRegion(pGpu, pMemoryManager, &rsvdFbRegion);
    }

    // Add above reserved FB region base to reserved memory
    NV_ASSERT_OK_OR_RETURN(memmgrStateInitAdjustReservedMemory(pGpu, pMemoryManager));

    return NV_OK;
}

/**
 * @brief Compares two addresses and apertures and returns if they are equivalent.
 *
 * @param[in] target0
 * @param[in] address0
 * @param[in] target1
 * @param[in] address1
 *
 * @return NV_TRUE if adresses refer to same memory location, NV_FALSE otherwise
 */
NvBool
memmgrComparePhysicalAddresses_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU32          target0,
    NvU64          address0,
    NvU32          target1,
    NvU64          address1
)
{
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    //
    // Sysmem inst blocks can be flipped:
    //   * PDB_PROP_FIFO_BUG_442481_NCOH_INST_BLOCK_DEF: ncoh -> coh
    // For system memory there is no harm in matching both sysmem
    // apertures, it is really only vital that we check between vid
    // and system memory.  Force both to SYS NCOH if system coherent.
    //
    if (target0 == NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY)
        target0 = NV_MMU_PTE_APERTURE_SYSTEM_NON_COHERENT_MEMORY;
    if (target1 == NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY)
        target1 = NV_MMU_PTE_APERTURE_SYSTEM_NON_COHERENT_MEMORY;

    if (target0 == NV_MMU_PTE_APERTURE_SYSTEM_NON_COHERENT_MEMORY)
    {
        //
        // One of the addresses may not account for the DMA window while the
        // other does. Given the nature of the DMA window (its offset must be
        // outside the addressable range of the GPU or 0), there's no danger
        // in trying to account for it here; it can't cause any false
        // positives.
        //
        if (address0 < address1)
            address0 += pKernelBif->dmaWindowStartAddress;
        else if (address1 < address0)
            address1 += pKernelBif->dmaWindowStartAddress;
    }

    return (target0 == target1) && (address0 == address1);
}

/*!
 * @brief - This routine computes the location in memory to
 * relocate VGA display memory to; size arguments are
 * expected in units of bytes.
 *
 * @param[in]  pGpu            GPU object pointer
 * @param[in]  pFb          FB  object pointer
 * @param[in]  vbiosSpaceSize  The current size of the VBIOS space
 * @param[in]  fbSize          The size of video memory
 */
static NV_STATUS
memmgrComputeAndSetVgaDisplayMemoryBase_GM107
(
    OBJGPU *pGpu,
    NvU64   fbSize
)
{

    return NV_OK;
}

/*!
 * @brief: Returns the PTE kind of block linear surfaces
 */
NvU32
memmgrGetPteKindBl_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    return NV_MMU_PTE_KIND_GENERIC_16BX2;
}

/*!
 * @brief: Returns the PTE kind of pitch linear surfaces
 */
NvU32
memmgrGetPteKindPitch_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    return NV_MMU_PTE_KIND_PITCH;
}

#define PAGE_ALIGN_MATCH(value, pageSize) ((value & (pageSize - 1)) == 0)

//
// Try to determine the optimial page size.  See if both the aligment of the
// physical address and the alignment of the allocation size fit one of the
// larger page sizes.
//
static RM_ATTR_PAGE_SIZE
_memmgrGetOptimalSysmemPageSize
(
    RmPhysAddr physAddr,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU64 bigPageSize,
    NvU64 sysmemPageSize
)
{
    NvBool bIsContiguous = memdescGetContiguity(pMemDesc, AT_GPU);

    //
    // Optimization currently only applies to contiguous memory.
    //
    if (bIsContiguous)
    {
        if (PAGE_ALIGN_MATCH(physAddr, RM_PAGE_SIZE_HUGE) &&
            PAGE_ALIGN_MATCH(pMemDesc->Size, RM_PAGE_SIZE_HUGE))
        {
            return RM_PAGE_SIZE_HUGE;
        }

        if (PAGE_ALIGN_MATCH(physAddr, bigPageSize) &&
            PAGE_ALIGN_MATCH(pMemDesc->Size, bigPageSize))
        {
            return bigPageSize;
        }
    }

    return sysmemPageSize;
}

#undef PAGE_ALIGN_MATCH

//
// Set the page size for the memory descriptor. The page size for a piece of memory
// may be set when it is mapped or when it is bound to a display channel. Current
// algorithm is simple. Default is 4KB in system memory (only choice) or large
// pages in video memory if the allocation is larger than the small page size.
//
// Some APIs allow the page size to be specified.  Allow this if the page size is unset,
// other wise error check it against the existing page size.
//
// We depend on fbgf100.c rounding up allocations to 4KB or bigPageSize to have coherent
// mapping sizes.  This does not show up in pMemDesc->Size at this point, so we have
// to trust that nothing is overlapping and cannot do full error checking.
//
// Big and huge pages are supported only in vidmem by default. In order to support
// big/huge pages in sysmem as is required by ATS (on Volta) and a few arch tests on Pascal
// (better TLB hit), we need to set the regkey RMSysmemPageSize equal to the page size.
// See bugs 1700272 and 1622233.
//
// TODO: Due to the page size swizzling, allocations should not physically overlap
//       within their swizzle range.  I am not sure the heap enforces this.
//
// NOTE: Scattered vidmem with big pages not checked right as it is not supported yet
//
NV_STATUS
memmgrSetMemDescPageSize_GM107
(
    OBJGPU             *pGpu,
    MemoryManager      *pMemoryManager,
    MEMORY_DESCRIPTOR  *pMemDesc,
    ADDRESS_TRANSLATION addressTranslation,
    RM_ATTR_PAGE_SIZE   pageSizeAttr
)
{
    NvU64             newPageSize    = RM_PAGE_SIZE;
    KernelGmmu       *pKernelGmmu    = GPU_GET_KERNEL_GMMU(pGpu);
    NV_ADDRESS_SPACE  addrSpace      = memdescGetAddressSpace(pMemDesc);
    NvU64             oldPageSize;

    // This policy is meaningless for virtual memdescs, so abort early.
    if (ADDR_VIRTUAL == addrSpace)
    {
        return NV_OK;
    }

    if (ADDR_SYSMEM == addrSpace)
    {
        RmPhysAddr physAddr = memdescGetPte(pMemDesc, addressTranslation, 0);
        switch (pageSizeAttr)
        {
            case RM_ATTR_PAGE_SIZE_INVALID:
                NV_PRINTF(LEVEL_ERROR, "invalid page size attr\n");
                return NV_ERR_INVALID_ARGUMENT;
            case RM_ATTR_PAGE_SIZE_DEFAULT:
                newPageSize = _memmgrGetOptimalSysmemPageSize(physAddr,
                        pMemDesc, kgmmuGetBigPageSize_HAL(pKernelGmmu),
                        pMemoryManager->sysmemPageSize);
                break;
            case RM_ATTR_PAGE_SIZE_4KB:
                newPageSize = RM_PAGE_SIZE;
                break;
            case RM_ATTR_PAGE_SIZE_BIG:
                newPageSize = kgmmuGetBigPageSize_HAL(pKernelGmmu);
                break;
            case RM_ATTR_PAGE_SIZE_HUGE:
                NV_ASSERT_OR_RETURN(kgmmuIsHugePageSupported(pKernelGmmu),
                                  NV_ERR_NOT_SUPPORTED);
                // If forcing the huge page size the underlying memory must be aligned
                NV_ASSERT_OR_RETURN(0 == (physAddr & (RM_PAGE_SIZE_HUGE - 1)), NV_ERR_INVALID_OFFSET);
                newPageSize = RM_PAGE_SIZE_HUGE;
                break;
            case RM_ATTR_PAGE_SIZE_512MB:
                NV_ASSERT_OR_RETURN(kgmmuIsPageSize512mbSupported(pKernelGmmu),
                                  NV_ERR_NOT_SUPPORTED);
                // If forcing the 512MB page size the underlying memory must be aligned
                NV_ASSERT_OR_RETURN(0 == (physAddr & (RM_PAGE_SIZE_512M - 1)), NV_ERR_INVALID_OFFSET);
                newPageSize = RM_PAGE_SIZE_512M;
                break;
        }
    }
    else if (ADDR_FBMEM == addrSpace)
    {
        RmPhysAddr physAddr = memdescGetPte(pMemDesc, addressTranslation, 0);
        switch (pageSizeAttr)
        {
            case RM_ATTR_PAGE_SIZE_INVALID:
                NV_PRINTF(LEVEL_ERROR, "invalid page size attr\n");
                return NV_ERR_INVALID_ARGUMENT;
            case RM_ATTR_PAGE_SIZE_DEFAULT:
            {
                NvBool bUseDefaultHugePagesize = NV_TRUE;
                // WDDMV2 Windows it expect default page size to be 4K /64KB /128KB
                // Big enough and aligned for huge pages?
                if (bUseDefaultHugePagesize &&
                    kgmmuIsHugePageSupported(pKernelGmmu) &&
                    (pMemDesc->Size >= RM_PAGE_SIZE_HUGE) &&
                    (0 == (physAddr & (RM_PAGE_SIZE_HUGE - 1))))
                {
                    newPageSize = RM_PAGE_SIZE_HUGE;
                }
                // Big enough and aligned for big pages?
                else if (((pMemDesc->Size >= kgmmuGetMinBigPageSize(pKernelGmmu)) ||
                          (memmgrIsKindCompressible_HAL(pMemoryManager, memdescGetPteKind(pMemDesc)))) &&
                         ((physAddr & (kgmmuGetMaxBigPageSize_HAL(pKernelGmmu) - 1)) == 0))
                {
                    newPageSize = kgmmuGetBigPageSize_HAL(pKernelGmmu);
                }
                break;
            }
            case RM_ATTR_PAGE_SIZE_4KB:
                newPageSize = RM_PAGE_SIZE;
                break;
            case RM_ATTR_PAGE_SIZE_BIG:
                newPageSize = kgmmuGetBigPageSize_HAL(pKernelGmmu);
                // If forcing the big page size the underlying memory must be aligned
                NV_ASSERT_OR_RETURN(0 == (physAddr & (newPageSize - 1)), NV_ERR_INVALID_OFFSET);
                break;
            case RM_ATTR_PAGE_SIZE_HUGE:
                NV_ASSERT_OR_RETURN(kgmmuIsHugePageSupported(pKernelGmmu),
                                  NV_ERR_NOT_SUPPORTED);
                // If forcing the huge page size the underlying memory must be aligned
                NV_ASSERT_OR_RETURN(0 == (physAddr & (RM_PAGE_SIZE_HUGE - 1)), NV_ERR_INVALID_OFFSET);
                newPageSize = RM_PAGE_SIZE_HUGE;
                break;
            case RM_ATTR_PAGE_SIZE_512MB:
                NV_ASSERT_OR_RETURN(kgmmuIsPageSize512mbSupported(pKernelGmmu),
                                  NV_ERR_NOT_SUPPORTED);
                // If forcing the 512MB page size the underlying memory must be aligned
                NV_ASSERT_OR_RETURN(0 == (physAddr & (RM_PAGE_SIZE_512M - 1)), NV_ERR_INVALID_OFFSET);
                newPageSize = RM_PAGE_SIZE_512M;
                break;
        }
    }

    // Only update the memory descriptor if it is unset
    oldPageSize = memdescGetPageSize(pMemDesc, addressTranslation);
    if (0 == oldPageSize)
    {
        memdescSetPageSize(pMemDesc, addressTranslation, newPageSize);
    }
    else if (pageSizeAttr != RM_ATTR_PAGE_SIZE_DEFAULT)
    {
        // If this memdesc already has a page size, the override must match
        NV_ASSERT_OR_RETURN(oldPageSize == newPageSize, NV_ERR_INVALID_ARGUMENT);
    }

    return NV_OK;
}

/*!
 *  @brief Calculate the Vista reserved memory requirement
 *         per FB region for UVM to create sysmem mappings for UVM objects.
 *
 *  @param[out] rsvdSlowSize   generic reserved RM memory needed in slow region
 */
void
memmgrCalcReservedFbSpaceForUVM_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU64         *rsvdSlowSize
)
{
    //
    // For WDDM-UVM, reserve space to create identity mapping (deviceVA = devicePA). (Kepler only)
    //
    *rsvdSlowSize +=
        1 * 1024 * 1024;   // 1MB space to map 4K pages of ~512MB sysmem = Pushbuffers(480MB) + SemaphoreVA(8KB) + PDE(512 entries * 8)
}

/*!
 *  @brief Calculate the reserved memory requirement for pre-allocated UserD.
 *
 *  @return Size of UserD reserved memory in bytes.
 */
NvU32
memmgrGetUserdReservedFbSpace_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    return 2 * 1024 * 1024;
}

/*!
 * @brief - This function will return the size reserved for WDDM
 * S/R buffer. RM is returning a bigger size just so that, our S/R buffer
 * consumption will never go beyond that. There is no deterministic way to find this
 * value, so returning a constant here.
 * Note: OS doesn't really allocate any buffer for the size, we will get the real
 * memory only when we try to map during suspend/resume.
 */
NvU64
memmgrGetRsvdSizeForSr_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    if ((pMemoryManager->Ram.fbTotalMemSizeMb >> 10) > 32)
    {
        //
        // For SKUs with more than 32GB FB, need to reserve for more memory for S/R
        // Bug Id:2468357
        //
        return 512 * 1024 * 1024;
    }
    else
    {
        return 256 * 1024 * 1024;
    }
}

NvU32
memmgrGetRunlistEntriesReservedFbSpace_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    // Kepler Runlist: 4096 entries * 8B/entry * 7 engines * 2 runlists/engine = 458K
    return (4096 * 8 * 7 * 2);
}


/*!
 * @brief Override Scrubber related PDB properties based on regkeys and platform configs
 */
void
memmgrScrubRegistryOverrides_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    //
    // Disabling the SCRUB_ON_FREE property for all the platforms except Windows TCC Mode.
    // Disabling in Non-TCC windows because the OS manages FB
    // Disabling in RTLSIM and FMODEL because the feature is slower in simulation platforms
    // Disabling in DFPGA, since they skip the Host Load
    // Disabling in MODS, because scrub on free slows down the MODS run time.
    // Disabling for vGPU (host), since the plugin has scrubbing support
    // Disabling for VGPU (guest), blocked on bug #1929798
    // Disabling for SLI for now, until the bug # 1790190 is fixed.
    // Disabling for GSP-RM ucode, since scrubbing is done from CPU-side kernel RM.
    // Disabling when reg key override is used
    if ((RMCFG_FEATURE_PLATFORM_WINDOWS && !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_TCC_MODE)) ||
         IS_RTLSIM(pGpu) || IS_FMODEL(pGpu) || IsDFPGA(pGpu) ||
         (RMCFG_FEATURE_PLATFORM_MODS && !pMemorySystemConfig->bOneToOneComptagLineAllocation) ||
         pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VGPU) ||
         IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
         RMCFG_FEATURE_PLATFORM_GSP ||
         IsSLIEnabled(pGpu))
    {
        pMemoryManager->bScrubOnFreeEnabled = NV_FALSE;
    }

    if (pMemoryManager->bDisableAsyncScrubforMods)
    {
        // need to force disable the scrub on free in case the wrong set of regkeys are set
        pMemoryManager->bScrubOnFreeEnabled = NV_FALSE;
    }

    if ((IS_VIRTUAL_WITH_SRIOV(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)) ||
        pMemorySystemConfig->bOneToOneComptagLineAllocation)
    {
        pMemoryManager->bUseVasForCeMemoryOps = NV_TRUE;
    }
}

/*!
 *  @Get the top of memory in MB
 *
 * Calculate the actual physical address space size of FB, without
 * regard for overrides or caps.
 *
 *  @returns the physical address space size of FB, which is greater
 *      than or equal to the populated FB memory size
 */
NvU32
memmgrGetAddrSpaceSizeMB_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NV_ASSERT(pMemoryManager->Ram.fbAddrSpaceSizeMb != 0);

    return NvU64_LO32(pMemoryManager->Ram.fbAddrSpaceSizeMb);
}

//
// Get fb ram size (usable and mappable).
//
NvU32
memmgrGetUsableMemSizeMB_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NV_ASSERT(pMemoryManager->Ram.fbAddrSpaceSizeMb != 0);

    // we shouldn't ever need this, but...
    NV_ASSERT(0 == NvU64_HI32(pMemoryManager->Ram.fbUsableMemSize >> 20));
    return NvU64_LO32(pMemoryManager->Ram.fbUsableMemSize >> 20);
}

#define _MAX_COVG (100*NVOS32_ALLOC_COMPR_COVG_SCALE)

//
// memmgrGetBankPlacementData
//
NV_STATUS
memmgrGetBankPlacementData_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU32         *pPlacementStrategy
)
{
    // set up bank placement arrays
    pPlacementStrategy[BANK_PLACEMENT_IMAGE]  = ((0)
                                                | BANK_MEM_GROW_UP
                                                | MEM_GROW_UP
                                                | 0xFFFFFF00);
    pPlacementStrategy[BANK_PLACEMENT_DEPTH]  = ((0)
                                                | BANK_MEM_GROW_DOWN
                                                | MEM_GROW_DOWN
                                                | 0xFFFFFF00);
    pPlacementStrategy[BANK_PLACEMENT_TEX_OVERLAY_FONT]  = ((0)
                                                | BANK_MEM_GROW_DOWN
                                                | MEM_GROW_UP
                                                | 0xFFFFFF00);
    pPlacementStrategy[BANK_PLACEMENT_OTHER]  = ((0)
                                                | BANK_MEM_GROW_DOWN
                                                | MEM_GROW_DOWN
                                                | 0xFFFFFF00);
    return (NV_OK);
}

//
// memmgrDirtyForPmTest
// Developed soley for testing suspend/resume path. Goal here is before
// resuming the GPU, we want to dirty the entire FB to verify whether
// RM has saved and restored all the critical data structures and states
// during suspend/resume. Called using the RMCTRL NV208F_CTRL_CMD_SUSPEND_RESUME_QUICK.
// WARNING: This function uses BAR0 window (which is always physical)
//          to dirty the FB contents. Upon exit of this call FB ram is dirty and
//          cannot be used, unless all the data needed is restored during resume.
//
void
memmgrDirtyForPmTest_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvBool         partialDirty
)
{
}

/*!
 * @Return an invalid placeholder FB offset.  Should be 128KB aligned for Fermi chips.
 *
 * @returns offset
 */
RmPhysAddr
memmgrGetInvalidOffset_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    return 0xdead000000000000ull;
}

/*!
 *  @brief Get blacklist page details.
 *
 *  @param[in]      pGpu            OBJGPU
 *  @param[in]      pMemoryManager  MemoryManager
 *  @param[out]     pBlAddrs        BLACKLIST_ADDRESSES where count is taken
 *                                  as input and the addressed and count is
 *                                  returned.
 *  @param[in/out]  pCount          Takes size of pBlAddrs as input and returns
 *                                  the number of populated addresses in
 *                                  pBlAddrs.
   @returns NV_STATUS
 *
 */
NV_STATUS
memmgrGetBlackListPages_GM107
(
    OBJGPU              *pGpu,
    MemoryManager       *pMemoryManager,
    BLACKLIST_ADDRESS   *pBlAddrs,
    NvU32               *pCount
)
{
    RM_API    *pRmApi;
    NV_STATUS status = NV_OK;
    NvU32     idx;
    NvU32     entryIdx = 0;
    NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS *pParams;
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_ALLOW_PAGE_RETIREMENT) ||
             !gpuCheckPageRetirementSupport_HAL(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // Read the inforom for a list of pages to blacklist.
    // SLI support requires investigation to ensure
    // identical heaps on both devices (bug 756971).
    //
    if (IsSLIEnabled(pGpu) && !gpuIsEccPageRetirementWithSliAllowed(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pParams = portMemAllocStackOrHeap(sizeof(*pParams));
    if (pParams == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    portMemSet(pParams, 0, sizeof(*pParams));

    pRmApi = IS_GSP_CLIENT(pGpu) ? GPU_GET_PHYSICAL_RMAPI(pGpu) :
                                    rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_FB_GET_OFFLINED_PAGES,
                             pParams,
                             sizeof(*pParams));

    if (status == NV_OK)
    {
        for (idx = 0; idx < pParams->validEntries; idx++)
        {
            if (entryIdx >= *pCount)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                goto done;
            }
            if (pMemorySystemConfig->bEnabledEccFBPA)
            {
                pBlAddrs[entryIdx].address =
                    pParams->offlined[idx].pageAddressWithEccOn << RM_PAGE_SHIFT;
            }
            else
            {
                pBlAddrs[entryIdx].address =
                    pParams->offlined[idx].pageAddressWithEccOff << RM_PAGE_SHIFT;
            }
            pBlAddrs[entryIdx].type = pParams->offlined[idx].source;
            entryIdx++;
        }
    }
    else if (NV_ERR_NOT_SUPPORTED == status)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Offlining pages not supported\n");
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to read offlined addresses\n");
    }

done:
    *pCount = entryIdx;

    portMemFreeStackOrHeap(pParams);

    return status;
}

//
// Get the blackList pages and notify heap
//
NV_STATUS
memmgrGetBlackListPagesForHeap_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    Heap          *pHeap
)
{
    BLACKLIST_ADDRESS  *pBlAddrs;
    NvU32               idx;
    NV_STATUS           status;
    NvU32               count;

    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    count = pMemorySystemConfig->maximumBlacklistPages;
    pBlAddrs = portMemAllocNonPaged(sizeof(BLACKLIST_ADDRESS) * count);
    if (pBlAddrs == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = memmgrGetBlackListPages_HAL(pGpu, pMemoryManager, pBlAddrs, &count);
    NV_ASSERT(status != NV_ERR_BUFFER_TOO_SMALL);

    if (status == NV_OK)
    {
        for (idx = 0; idx < count; idx++)
        {

            status = heapAddPageToBlackList(pGpu, pHeap,
                                            pBlAddrs[idx].address >> RM_PAGE_SHIFT,
                                            pBlAddrs[idx].type);

            if (NV_OK != status)
            {
                NV_PRINTF(LEVEL_ERROR, "No more space in blacklist, status: %x!\n", status);
                NV_ASSERT(0);
                break;
            }
        }
    }

    portMemFree(pBlAddrs);

    // Failure to read offlined pages from host is not fatal
    return NV_OK;
}
