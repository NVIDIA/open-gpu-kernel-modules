/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/fermi_dma.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/bus/kern_bus.h"
#include "containers/eheap_old.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "platform/sli/sli.h"
#include "nvrm_registry.h"

#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/disp/inst_mem/disp_inst_mem.h"

#include "rmifrif.h"

#include "published/maxwell/gm107/dev_mmu.h"
#include "published/maxwell/gm107/dev_ram.h"

#include "vgpu/rpc.h"
#include "vgpu/vgpu_events.h"
#include "nvdevid.h"

//
// statics
//
static NV_STATUS memmgrComputeAndSetVgaDisplayMemoryBase_GM107(OBJGPU *, NvU64);

static NvU32
memmgrGetParentGpuZbcSurfaces
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    return GPU_GET_MEMORY_MANAGER(gpumgrGetParentGPU(pGpu))->zbcSurfaces;
}

static NvU32
memmgrIncParentGpuZbcSurfaces
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    return ++(GPU_GET_MEMORY_MANAGER(gpumgrGetParentGPU(pGpu))->zbcSurfaces);
}

static NvU32
memmgrDecParentGpuZbcSurfaces
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    return --(GPU_GET_MEMORY_MANAGER(gpumgrGetParentGPU(pGpu))->zbcSurfaces);
}

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
        case RM_ATTR_PAGE_SIZE_256GB:
            NV_ASSERT_OR_RETURN(kgmmuIsPageSize256gbSupported(pKernelGmmu),
                                NV_ERR_INVALID_ARGUMENT);
            hwAlignment = NV_MAX(hwAlignment, RM_PAGE_SIZE_256G - 1);
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
    NvHandle hClient,
    NvHandle hDevice,
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
            hClient,
            hDevice,
            NV0080_CTRL_CMD_INTERNAL_MEMSYS_SET_ZBC_REFERENCED,
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
    NvU32               comprAttr, zcullAttr, type;
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
    zcullAttr     = DRF_VAL(OS32, _ATTR, _ZCULL, pFbAllocInfo->pageFormat->attr);
    format        = DRF_VAL(OS32, _ATTR, _FORMAT, pFbAllocInfo->pageFormat->attr);
    cacheAttr     = DRF_VAL(OS32, _ATTR2, _GPU_CACHEABLE, pFbAllocInfo->pageFormat->attr2);
    type          = pFbAllocInfo->pageFormat->type;
    addrSpace     = memmgrAllocGetAddrSpace(pMemoryManager, pFbAllocInfo->pageFormat->flags, retAttr);

    bAlignPhase = !!(pFbAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_SKIP_RESOURCE_ALLOC);

    //
    // errorcheck specified attributes
    // NOTE: With the new macro, the distinction between 32-bit colour
    // compression and Z compression is in the value of 'type' - DEPTH or IMAGE
    // So the caller is urged to verify integrity.
    //
    if (
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
            memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_ZBC, pFbAllocInfo->pageFormat->kind) &&
            !(pFbAllocInfo->pageFormat->flags & NVOS32_ALLOC_FLAGS_VIRTUAL))
        {
            retAttr2 = FLD_SET_DRF(OS32, _ATTR2, _ZBC, _PREFER_ZBC, retAttr2);
            if (!bAlignPhase)
            {
                NvU32 zbcSurfaces = memmgrIncParentGpuZbcSurfaces(pGpu, pMemoryManager);

                NV_PRINTF(LEVEL_INFO,
                          "zbcSurfaces = 0x%x, hwResId = 0x%x\n",
                          zbcSurfaces, pFbAllocInfo->hwResId);

                if (zbcSurfaces == 1)
                    memmgrSetZbcReferenced(pGpu, pFbAllocInfo->hClient, pFbAllocInfo->hDevice, NV_TRUE);
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

    kmemsysFreeComprResources_HAL(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu), commitResId);

    if (FLD_TEST_DRF(OS32, _ATTR2, _ZBC_SKIP_ZBCREFCOUNT, _NO, pFbAllocInfo->pageFormat->attr2) &&
        memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_ZBC, pFbAllocInfo->format))
    {
        NvU32 zbcSurfaces = memmgrGetParentGpuZbcSurfaces(pGpu, pMemoryManager);

        //
        // For vGPU, ZBC is used by the guest application. So for vGPU use case zbcsurface can
        // be 0. Hence this ASSERT is not relevant for the VGPU HOST.
        // For the long term fix, we will have to save the flags that are set at the time of
        // object allocation and then while Free we will have to add assert based on the flags.
        //
        if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VGPU))
            NV_ASSERT(zbcSurfaces !=0 );

        if (zbcSurfaces != 0)
        {
            zbcSurfaces = memmgrDecParentGpuZbcSurfaces(pGpu, pMemoryManager);

            if (zbcSurfaces == 0)
                memmgrSetZbcReferenced(pGpu, pFbAllocInfo->hClient, pFbAllocInfo->hDevice, NV_FALSE);
        }

        NV_PRINTF(LEVEL_INFO,
                  "[1] hwResId = 0x%x, offset = 0x%llx, size = 0x%llx\n",
                  pFbAllocInfo->hwResId, pFbAllocInfo->offset,
                  pFbAllocInfo->size);

        NV_PRINTF(LEVEL_INFO, "[2] zbcSurfaces = 0x%x\n", zbcSurfaces);
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

        NV_ASSERT_OK_OR_RETURN(
            subdeviceGetByInstance(pClient,
                                   RES_GET_HANDLE(pDevice),
                                   gpumgrGetSubDeviceInstanceFromGpu(pGpu),
                                   &pSubdevice));

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

    if (!kbusIsBar1Disabled(pKernelBus))
    {
        pBar1VAS = kbusGetBar1VASpace_HAL(pGpu, pKernelBus);
        NV_ASSERT_OR_RETURN(pBar1VAS != NULL, NV_ERR_INVALID_STATE);
        pVASHeap = vaspaceGetHeap(pBar1VAS);

        NV_ASSERT_OK_OR_RETURN(kbusGetBar1VARangeForDevice(pGpu, pKernelBus, pDevice, &bar1VARange));
        bar1Info->bar1Size = (NvU32)(rangeLength(bar1VARange) / 1024);
        bar1Info->bankSwizzleAlignment = vaspaceGetBigPageSize(pBar1VAS);

        bar1Info->bar1AvailSize = 0;
        bar1Info->bar1MaxContigAvailSize = 0;

        if (!kbusIsStaticBar1Enabled(pGpu, pKernelBus))
        {
            // normal non-static BAR1 case
            if (pVASHeap != NULL)
            {
                pVASHeap->eheapInfoForRange(pVASHeap, bar1VARange, NULL, &largestFreeSize, NULL, &freeSize);
                bar1Info->bar1AvailSize = (NvU32)(freeSize / 1024);
                bar1Info->bar1MaxContigAvailSize = (NvU32)(largestFreeSize / 1024);
            }
        }
        else
        {
            //
            // Actual BAR1 usage isn't interesting in static BAR1 because all the
            // client BAR1 is already mapped. Also BAR1 >= client FB size.
            // So free BAR1 that can stil be used is reported as
            // bar1Size - FB in-use size
            //
            NV2080_CTRL_FB_GET_INFO_V2_PARAMS fbInfoParams = {0};
            Subdevice *pSubdevice;
            RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

            NvU32 fbInUse;

            NV_ASSERT_OK_OR_RETURN(
                subdeviceGetByInstance(pClient,
                                       RES_GET_HANDLE(pDevice),
                                       gpumgrGetSubDeviceInstanceFromGpu(pGpu),
                                       &pSubdevice));

            fbInfoParams.fbInfoList[0].index = NV2080_CTRL_FB_INFO_INDEX_HEAP_SIZE;
            fbInfoParams.fbInfoList[1].index = NV2080_CTRL_FB_INFO_INDEX_HEAP_FREE;

            fbInfoParams.fbInfoListSize = 2;

            NV_ASSERT_OK_OR_RETURN(
                pRmApi->Control(pRmApi, pClient->hClient, RES_GET_HANDLE(pSubdevice),
                                NV2080_CTRL_CMD_FB_GET_INFO_V2,
                                &fbInfoParams,
                                sizeof(fbInfoParams)));

            fbInUse = fbInfoParams.fbInfoList[0].data -
                      fbInfoParams.fbInfoList[1].data;

            bar1Info->bar1AvailSize = bar1Info->bar1Size - fbInUse;

            // Bug 4087553: we're requeted to return 0 for now
            bar1Info->bar1MaxContigAvailSize = 0;
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

NvU64
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

    return rsvdSize;
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
    if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL(pGpu))
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

    if (!IS_GSP_CLIENT(pGpu))
    {
        NV_STATUS              status;
        KernelMemorySystem    *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
        KernelBus             *pKernelBus          = GPU_GET_KERNEL_BUS(pGpu);
        NvU32                  allocFlags          = MEMDESC_FLAGS_NONE;
        const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
            kmemsysGetStaticConfig(pGpu, pKernelMemorySystem);

        // check for allocating VPR memory
        if (pKernelBus->bInstProtectedMem)
            allocFlags |= MEMDESC_ALLOC_FLAGS_PROTECTED;

        if ((status = memdescCreate(&pKernelBus->bar1[GPU_GFID_PF].pInstBlkMemDesc, pGpu,
                                    GF100_BUS_INSTANCEBLOCK_SIZE,
                                    GF100_BUS_INSTANCEBLOCK_SIZE,
                                    NV_TRUE,
                                    pKernelBus->InstBlkAperture,
                                    pKernelBus->InstBlkAttr,
                                    allocFlags)) != NV_OK)
        {
            return status;
        }

        if ((memdescGetAddressSpace(pKernelBus->bar1[GPU_GFID_PF].pInstBlkMemDesc) == ADDR_SYSMEM) &&
            (gpuIsInstanceMemoryAlwaysCached(pGpu)))
        {
            memdescSetGpuCacheAttrib(pKernelBus->bar1[GPU_GFID_PF].pInstBlkMemDesc, NV_MEMORY_CACHED);
        }

        if ((status = memdescCreate(&pKernelBus->bar2[GPU_GFID_PF].pInstBlkMemDesc, pGpu,
                                    GF100_BUS_INSTANCEBLOCK_SIZE,
                                    GF100_BUS_INSTANCEBLOCK_SIZE,
                                    NV_TRUE,
                                    pKernelBus->InstBlkAperture,
                                    pKernelBus->InstBlkAttr,
                                    allocFlags)) != NV_OK)
        {
            return status;
        }

        if ((memdescGetAddressSpace(pKernelBus->bar2[GPU_GFID_PF].pInstBlkMemDesc) == ADDR_SYSMEM) &&
            (gpuIsInstanceMemoryAlwaysCached(pGpu)))
        {
            memdescSetGpuCacheAttrib(pKernelBus->bar2[GPU_GFID_PF].pInstBlkMemDesc, NV_MEMORY_CACHED);
        }

        switch (pKernelBus->InstBlkAperture)
        {
            default:
            case ADDR_FBMEM:
                pKernelBus->bar1[GPU_GFID_PF].instBlockBase += pMemoryManager->rsvdMemoryBase;
                memdescDescribe(pKernelBus->bar1[GPU_GFID_PF].pInstBlkMemDesc,
                                pKernelBus->InstBlkAperture,
                                pKernelBus->bar1[GPU_GFID_PF].instBlockBase,
                                GF100_BUS_INSTANCEBLOCK_SIZE);
                pKernelBus->bar2[GPU_GFID_PF].instBlockBase += pMemoryManager->rsvdMemoryBase;
                memdescDescribe(pKernelBus->bar2[GPU_GFID_PF].pInstBlkMemDesc,
                                pKernelBus->InstBlkAperture,
                                pKernelBus->bar2[GPU_GFID_PF].instBlockBase,
                                GF100_BUS_INSTANCEBLOCK_SIZE);

                // Pre-fill cache to prevent FB read accesses if in cache only mode and not doing one time pre-fill
                if (gpuIsCacheOnlyModeEnabled(pGpu) &&
                    !pMemorySystemConfig->bL2PreFill)
                {
                    kmemsysPreFillCacheOnlyMemory_HAL(pGpu, pKernelMemorySystem, pKernelBus->bar1[GPU_GFID_PF].instBlockBase, GF100_BUS_INSTANCEBLOCK_SIZE);
                    kmemsysPreFillCacheOnlyMemory_HAL(pGpu, pKernelMemorySystem, pKernelBus->bar2[GPU_GFID_PF].instBlockBase, GF100_BUS_INSTANCEBLOCK_SIZE);
                }
                break;

            case ADDR_SYSMEM:
                memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_135, 
                                (pKernelBus->bar1[GPU_GFID_PF].pInstBlkMemDesc));
                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "couldn't allocate BAR1 instblk in sysmem\n");
                    return status;
                }
                pKernelBus->bar1[GPU_GFID_PF].instBlockBase = memdescGetPhysAddr(pKernelBus->bar1[GPU_GFID_PF].pInstBlkMemDesc, AT_GPU, 0);
                memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_136, 
                                (pKernelBus->bar2[GPU_GFID_PF].pInstBlkMemDesc));
                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "couldn't allocate BAR2 instblk in sysmem\n");
                    return status;
                }
                pKernelBus->bar2[GPU_GFID_PF].instBlockBase = memdescGetPhysAddr(pKernelBus->bar2[GPU_GFID_PF].pInstBlkMemDesc, AT_GPU, 0);
                break;
        }
    }

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

    NV_PRINTF(LEVEL_INFO, "Final reserved memory size = 0x%llx\n", pMemoryManager->rsvdMemorySize);

    if (!IS_VIRTUAL(pGpu))
    {
        //
        // Reserved memory must fit in BAR0 window - well compression backing is after this.
        // Does not matter for GSP itself as BAR0 is not used.
        //
        NV_ASSERT_OR_RETURN(RMCFG_FEATURE_PLATFORM_GSP ||
            memmgrCheckReservedMemorySize_HAL(pGpu, pMemoryManager) == NV_OK, NV_ERR_INSUFFICIENT_RESOURCES);
    }

    NV_PRINTF(LEVEL_INFO, "RESERVED Memory size: 0x%llx\n", pMemoryManager->rsvdMemorySize);

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
    if (gpuIsCacheOnlyModeEnabled(pGpu) || !pMemoryManager->bReservedMemAtBottom)
    {
        const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
            kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

        if (gpuIsCacheOnlyModeEnabled(pGpu))
        {
            rsvdTopOfMem = pMemorySystemConfig->l2CacheSize;
        }
        else
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
        pMemoryManager->rsvdMemorySize = rsvdTopOfMem - pMemoryManager->rsvdMemoryBase;

        // make sure we didn't just blindly truncate that...
        NV_ASSERT(0 == NvU64_HI32(rsvdTopOfMem - pMemoryManager->rsvdMemoryBase));
    }
    // Reserved memory located at bottom of FB, base this at start of FB
    else
    {
        tmpAddr = 0;
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

        if (RMCFG_FEATURE_PLATFORM_GSP)
        {
            rsvdFbRegion.bPreserveOnSuspend = NV_TRUE;
        }
        else
        {
            // Reserved region is explicitly saved as before/after Bar2 PTE region.
            rsvdFbRegion.bLostOnSuspend = NV_TRUE;
        }

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
        NvU64 sysmemPageSize = RMCFG_FEATURE_PLATFORM_UNIX ? osGetPageSize() : pMemoryManager->sysmemPageSize;
        RmPhysAddr physAddr = memdescGetPte(pMemDesc, addressTranslation, 0);
        switch (pageSizeAttr)
        {
            case RM_ATTR_PAGE_SIZE_INVALID:
                NV_PRINTF(LEVEL_ERROR, "invalid page size attr\n");
                return NV_ERR_INVALID_ARGUMENT;
            case RM_ATTR_PAGE_SIZE_DEFAULT:
                newPageSize = _memmgrGetOptimalSysmemPageSize(physAddr,
                        pMemDesc, kgmmuGetBigPageSize_HAL(pKernelGmmu),
                        sysmemPageSize);
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
            case RM_ATTR_PAGE_SIZE_256GB:
                NV_ASSERT_OR_RETURN(kgmmuIsPageSize256gbSupported(pKernelGmmu),
                                  NV_ERR_NOT_SUPPORTED);
                // If forcing the 256GB page size the underlying memory must be aligned
                NV_ASSERT_OR_RETURN(0 == (physAddr & (RM_PAGE_SIZE_256G - 1)), NV_ERR_INVALID_OFFSET);
                newPageSize = RM_PAGE_SIZE_256G;
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
            case RM_ATTR_PAGE_SIZE_256GB:
                NV_ASSERT_OR_RETURN(kgmmuIsPageSize256gbSupported(pKernelGmmu),
                                  NV_ERR_NOT_SUPPORTED);
                // If forcing the 256GB page size the underlying memory must be aligned
                NV_ASSERT_OR_RETURN(0 == (physAddr & (RM_PAGE_SIZE_256G - 1)), NV_ERR_INVALID_OFFSET);
                newPageSize = RM_PAGE_SIZE_256G;
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
    //
    // Temporary WAR to override WDDM S/R buffer for specific skus
    // Bug 5327051
    //
    static const NvU16 gb20x_devid[] = { 0x2B8C };
    NvU32  pciDeviceID = DRF_VAL(_PCI, _DEVID, _DEVICE, pGpu->idInfo.PCIDeviceID);
    NvBool overrideFbsrRsvdBufferSize = NV_FALSE;

    for (NvU32 i = 0; i < NV_ARRAY_ELEMENTS(gb20x_devid); i++)
    {
        if (pciDeviceID == gb20x_devid[i])
        {
            overrideFbsrRsvdBufferSize = NV_TRUE;
            break;
        }
    }

    if (((pMemoryManager->Ram.fbTotalMemSizeMb >> 10) >= 31) || IS_GSP_CLIENT(pGpu))
    {
        //
        // We need to reserve more memory for S/R if
        // 1. FB size is >= 31GB  Bug Id: 2468357
        // 2. Or GSP is enabled  Bug Id: 4312881
        //
        return 512 * 1024 * 1024;
    }
    else if (overrideFbsrRsvdBufferSize)
    {
        // Bug 5327051: WAR to override WDDM S/R buffer for specific skus
        return 300 * 1024 * 1024;
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
NvU64
memmgrGetAddrSpaceSizeMB_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NV_ASSERT(pMemoryManager->Ram.fbAddrSpaceSizeMb != 0);

    return pMemoryManager->Ram.fbAddrSpaceSizeMb;
}

//
// Get fb ram size (usable and mappable).
//
NvU64
memmgrGetUsableMemSizeMB_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NV_ASSERT(pMemoryManager->Ram.fbAddrSpaceSizeMb != 0);

    // we shouldn't ever need this, but...
    NV_ASSERT(0 == NvU64_HI32(pMemoryManager->Ram.fbUsableMemSize >> 20));
    return (pMemoryManager->Ram.fbUsableMemSize >> 20);
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

    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

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
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    BLACKLIST_ADDRESS  *pBlAddrs;
    NvU32               idx;
    NV_STATUS           status;
    NvU32               count;

    count = kmemsysGetMaximumBlacklistPages(pGpu, pKernelMemorySystem);
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

NvU32
memmgrGetFBEndReserveSizeEstimate_GM107
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    const NvU32 ESTIMATED_RESERVE_FB = 0x200000;

    return ESTIMATED_RESERVE_FB;
}

/*!
 *  @brief Calculate the reserved memory requirement
 *         per FB region for mixed type/density
 *
 *  @param[out] rsvdFastSize   generic reserved RM memory needed in fast region
 *  @param[out] rsvdSlowSize   generic reserved RM memory needed in slow region
 *  @param[out] rsvdISOSize    ISO-specific reserved RM memory needed
 *
 *  @returns void
 *
 */
void
memmgrCalcReservedFbSpaceHal_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU64         *rsvdFastSize,
    NvU64         *rsvdSlowSize,
    NvU64         *rsvdISOSize
)
{
    KernelGmmu            *pKernelGmmu   = GPU_GET_KERNEL_GMMU(pGpu);
    KernelFifo            *pKernelFifo   = GPU_GET_KERNEL_FIFO(pGpu);
    KernelGraphics        *pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, 0);
    NvU64                  smallPagePte  = 0;
    NvU64                  bigPagePte    = 0;
    NvU32                  attribBufferSize;
    NvU64                  maxContextSize = 0;
    NvU64                  userdReservedSize = 0;
    NvU64                  runlistEntriesReservedSize = 0;
    NvU64                  mmuFaultBufferSize = 0;
    NvU64                  faultMethodBufferSize = 0;
    NV_STATUS              status = NV_OK;

    // Initialize reserved block logging data structure
    NV_FB_RSVD_BLOCK_LOG_INIT(pMemoryManager);

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        //
        // 4MB of reserved memory for vGPU.
        // Mainly to satisfy KMD memory allocations.
        //
        *rsvdFastSize = 4 * 1024 * 1024;
        *rsvdSlowSize = 0;
        *rsvdISOSize = 0;
        return;
    }

#if defined(NV_UNIX)
    if (IS_VIRTUAL_WITH_SRIOV(pGpu) && pMemoryManager->Ram.fbTotalMemSizeMb <= 1024)
    {
        //
        // 88MB of reserved memory for vGPU guest with lower FB size(1GB)
        // in full SRIOV mode. On lower vGPU profiles, available FB memory
        // is already very less. To compensate for that, reducing the guest
        // reserved FB memory.
        //
        *rsvdFastSize = 88 * 1024 * 1024;
        *rsvdSlowSize = 0;
        *rsvdISOSize = 0;
        return;
    }
#endif

    {
        *rsvdFastSize = 0;

        // Allow reservation up to half of usable FB size
        if (pMemoryManager->rsvdMemorySizeIncrement > (pMemoryManager->Ram.fbUsableMemSize / 2))
        {
            pMemoryManager->rsvdMemorySizeIncrement = pMemoryManager->Ram.fbUsableMemSize / 2;
            NV_PRINTF(LEVEL_ERROR,
                      "RM can only increase reserved heap by 0x%llx bytes\n",
                      pMemoryManager->rsvdMemorySizeIncrement);
        }
        NV_PRINTF(LEVEL_INFO, "RT::: incrementing the reserved size by: %llx\n",
                   pMemoryManager->rsvdMemorySizeIncrement);
        *rsvdSlowSize = pMemoryManager->rsvdMemorySizeIncrement;
        *rsvdISOSize = 0;
    }
    // Temporary workaround to increase the heap size for NVBUG 4997009
    if(IsGB20XorBetter(pGpu) && (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && !IS_VIRTUAL(pGpu)))
    {
        // increase by 150 MB
        *rsvdSlowSize += (150 << 20);
    }

    if (RMCFG_FEATURE_PLATFORM_WINDOWS && pMemoryManager->bBug2301372IncreaseRmReserveMemoryWar)
    {
        *rsvdFastSize += 30 * 1024 * 1024;
    }

    attribBufferSize = memmgrGetGrHeapReservationSize_HAL(pGpu, pMemoryManager);

    // Fast: Attribute buffer
    NV_FB_RSVD_BLOCK_LOG_ENTRY_ADD(status, pMemoryManager, NV_FB_ALLOC_RM_INTERNAL_OWNER_ATTR_BUFFER,
                         attribBufferSize);
    *rsvdFastSize += attribBufferSize;

    // Fast: Circular buffer & fudge
    NV_FB_RSVD_BLOCK_LOG_ENTRY_ADD(status, pMemoryManager, NV_FB_ALLOC_RM_INTERNAL_OWNER_CIRCULAR_BUFFER,
                        1 *1024 *1024);
    *rsvdFastSize += 1 *1024 *1024;

    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        // smallPagePte = FBSize /4k * 8 (Small page PTE for whole FB)
        smallPagePte = NV_ROUNDUP((pMemoryManager->Ram.fbUsableMemSize / FERMI_SMALL_PAGESIZE) * 8, RM_PAGE_SIZE);

        // bigPagePte = FBSize /bigPageSize * 8 (Big page PTE for whole FB)
        bigPagePte = NV_ROUNDUP((pMemoryManager->Ram.fbUsableMemSize / (kgmmuGetMaxBigPageSize_HAL(pKernelGmmu))) * 8,
                                     RM_PAGE_SIZE);

        NV_FB_RSVD_BLOCK_LOG_ENTRY_ADD(status, pMemoryManager, NV_FB_ALLOC_RM_INTERNAL_OWNER_PAGE_PTE,
                    (smallPagePte + bigPagePte));
    }

    userdReservedSize = memmgrGetUserdReservedFbSpace_HAL(pGpu, pMemoryManager);
    NV_FB_RSVD_BLOCK_LOG_ENTRY_ADD(status, pMemoryManager, NV_FB_ALLOC_RM_INTERNAL_OWNER_USERD_BUFFER,
                         userdReservedSize);

    runlistEntriesReservedSize = memmgrGetRunlistEntriesReservedFbSpace_HAL(pGpu, pMemoryManager);
    NV_FB_RSVD_BLOCK_LOG_ENTRY_ADD(status, pMemoryManager, NV_FB_ALLOC_RM_INTERNAL_OWNER_RUNLIST_ENTRIES,
                         runlistEntriesReservedSize);

    maxContextSize = memmgrGetMaxContextSize_HAL(pGpu, pMemoryManager);
    NV_FB_RSVD_BLOCK_LOG_ENTRY_ADD(status, pMemoryManager, NV_FB_ALLOC_RM_INTERNAL_OWNER_CONTEXT_BUFFER,
                         maxContextSize);
    *rsvdSlowSize +=
             userdReservedSize +  // Kepler USERD
             runlistEntriesReservedSize +   // Kepler Runlist entries
             smallPagePte +       // small page Pte
             bigPagePte +         // big page pte
             maxContextSize;

    // Reserve FB for UVM on WDDM
    memmgrCalcReservedFbSpaceForUVM_HAL(pGpu, pMemoryManager, rsvdSlowSize);

    // Reserve FB for MMU fault buffers
    mmuFaultBufferSize = kgmmuGetFaultBufferReservedFbSpaceSize(pGpu, pKernelGmmu);
    NV_FB_RSVD_BLOCK_LOG_ENTRY_ADD(status, pMemoryManager, NV_FB_ALLOC_RM_INTERNAL_OWNER_MMU_FAULT_BUFFER,
                         mmuFaultBufferSize);
    *rsvdSlowSize += mmuFaultBufferSize;

    // Reserve FB for Fault method buffers
    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        faultMethodBufferSize = kfifoCalcTotalSizeOfFaultMethodBuffers_HAL(pGpu, pKernelFifo, NV_TRUE);
        NV_FB_RSVD_BLOCK_LOG_ENTRY_ADD(status, pMemoryManager, NV_FB_ALLOC_RM_INTERNAL_OWNER_FAULT_METHOD,
                        faultMethodBufferSize);
        *rsvdSlowSize += faultMethodBufferSize;
    }

    // The access map is fairly large (512KB) so we account for it specifically
    if (kgraphicsDoesUcodeSupportPrivAccessMap(pGpu, pKernelGraphics))
    {
        *rsvdSlowSize += pGpu->userRegisterAccessMapSize;
        NV_FB_RSVD_BLOCK_LOG_ENTRY_ADD(status, pMemoryManager, NV_FB_ALLOC_RM_INTERNAL_OWNER_ACCESS_MAP,
                        pGpu->userRegisterAccessMapSize);
    }

    if (*rsvdFastSize + *rsvdSlowSize > pMemoryManager->Ram.fbUsableMemSize / 2)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Before capping: rsvdFastSize = 0x%llx bytes rsvdSlowSize = 0x%llx "
                  "bytes Usable FB = 0x%llx bytes\n", *rsvdFastSize,
                  *rsvdSlowSize, pMemoryManager->Ram.fbUsableMemSize);
        if (pMemoryManager->rsvdMemorySizeIncrement > 0)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Fail the rsvd memory capping in case of user specified increase = %llx bytes\n",
                      pMemoryManager->rsvdMemorySizeIncrement);
            *rsvdFastSize = 0;
            *rsvdSlowSize = 0;
            NV_ASSERT(0);
            return;
        }
        // Scale down fast and slow proportionally
        *rsvdFastSize = *rsvdFastSize * pMemoryManager->Ram.fbUsableMemSize / 2
            / (*rsvdFastSize + *rsvdSlowSize);
        *rsvdSlowSize = pMemoryManager->Ram.fbUsableMemSize / 2 - *rsvdFastSize;
        NV_PRINTF(LEVEL_ERROR,
                  "After capping: rsvdFastSize = 0x%llx bytes rsvdSlowSize = 0x%llx bytes\n",
                  *rsvdFastSize, *rsvdSlowSize);
    }

    if (!pMemoryManager->bPreferSlowRegion)
    {
        *rsvdFastSize = *rsvdFastSize + *rsvdSlowSize;
        *rsvdSlowSize = 0;
    }

    //
    // Memory should be blocked off with 64K granularity.  This makes PMA and
    // and VA space management more efficient.
    //
    *rsvdFastSize = NV_ROUNDUP(*rsvdFastSize, RM_PAGE_SIZE_64K);
    *rsvdSlowSize = NV_ROUNDUP(*rsvdSlowSize, RM_PAGE_SIZE_64K);
    *rsvdISOSize  = NV_ROUNDUP(*rsvdISOSize, RM_PAGE_SIZE_64K);

    // If any of the reservation logging fails then print error message
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING, "Error logging the FB reservation entries\n");
    }
}

/*!
 * Set up RM reserved memory space
 */
NV_STATUS
memmgrPreInitReservedMemory_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    KernelDisplay         *pKernelDisplay      = GPU_GET_KERNEL_DISPLAY(pGpu);
    KernelBus             *pKernelBus          = GPU_GET_KERNEL_BUS(pGpu);
    NvU64                  tmpAddr             = 0;
    NV_STATUS              status              = NV_OK;
    NvU32                  instBlkBarOverride  = 0;

    // ***************************************************************
    // Determine the size of reserved memory & tell the HW where it is
    // Note that the order of these matters for optimum alignment &
    // FB usage. The order must be highest alignment requirement to
    // lowest alignment requirement, with the last item being the
    // vbios image / workspace area
    // ***************************************************************

    if (IS_GSP_CLIENT(pGpu) && pKernelDisplay != NULL)
    {
        // TODO: Determine the correct size of display instance memory
        // via instmemGetSize_HAL(), as well as other parameters.
        // I.e. refactor and leverage the code performing these tasks
        // in memmgrPreInitReservedMemory_GM107() today.
        tmpAddr += 0x10000;
    }

    {
        instBlkBarOverride = DRF_VAL(_REG_STR_RM, _INST_LOC, _INSTBLK, pGpu->instLocOverrides);
    }

    pKernelBus->InstBlkAperture = ADDR_FBMEM;
    pKernelBus->InstBlkAttr = NV_MEMORY_WRITECOMBINED;

    memdescOverrideInstLoc(instBlkBarOverride, "BAR instblk",
                           &pKernelBus->InstBlkAperture,
                           &pKernelBus->InstBlkAttr);

    if (pKernelBus->InstBlkAperture == ADDR_FBMEM)
    {
        // Reserve space for BAR1 and BAR2 instance blocks
        tmpAddr = NV_ROUNDUP(tmpAddr, GF100_BUS_INSTANCEBLOCK_SIZE);
        pKernelBus->bar1[GPU_GFID_PF].instBlockBase = tmpAddr;
        tmpAddr += GF100_BUS_INSTANCEBLOCK_SIZE;

        tmpAddr = NV_ROUNDUP(tmpAddr, GF100_BUS_INSTANCEBLOCK_SIZE);
        pKernelBus->bar2[GPU_GFID_PF].instBlockBase = tmpAddr;
        tmpAddr += GF100_BUS_INSTANCEBLOCK_SIZE;

        NV_PRINTF(LEVEL_INFO, "Reserve space for Bar1 inst block offset = 0x%llx size = 0x%x\n",
            pKernelBus->bar1[GPU_GFID_PF].instBlockBase, GF100_BUS_INSTANCEBLOCK_SIZE);

        NV_PRINTF(LEVEL_INFO, "Reserve space for Bar2 inst block offset = 0x%llx size = 0x%x\n",
            pKernelBus->bar2[GPU_GFID_PF].instBlockBase, GF100_BUS_INSTANCEBLOCK_SIZE);
    }

    //
    // This has to be the very *last* thing in reserved memory as it
    // will may grow past the 1MB reserved memory window.  We cannot
    // size it until memsysStateInitLockedHal_GM107.
    //
    memmgrReserveBar2BackingStore(pGpu, pMemoryManager, &tmpAddr);

    //
    // Store the size of rsvd memory excluding VBIOS space. Size finalized in memmgrStateInitReservedMemory.
    //
    pMemoryManager->rsvdMemorySize = tmpAddr;

    NV_PRINTF(LEVEL_INFO, "Calculated size of reserved memory = 0x%llx. Size finalized in StateInit.\n", pMemoryManager->rsvdMemorySize);

    return status;
}
