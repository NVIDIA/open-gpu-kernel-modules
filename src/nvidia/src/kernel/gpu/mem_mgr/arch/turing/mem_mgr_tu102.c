/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/mem_desc.h"

#include "virtualization/hypervisor/hypervisor.h"
#include "vgpu/vgpu_events.h"

#include "published/turing/tu102/dev_mmu.h"
#include "published/turing/tu102/kind_macros.h"
#include "published/turing/tu102/dev_fb_addendum.h"
#include "nvRmReg.h"

/*!
 * @brief Determine the kind of uncompressed PTE for a given allocation.
 *
 * @param[in] pFbAllocPageFormat FB_ALLOC_PAGE_FORMAT pointer
 *
 * @returns   PTE kind.
 */
NvU32
memmgrChooseKindZ_TU102
(
    OBJGPU                 *pGpu,
    MemoryManager          *pMemoryManager,
    FB_ALLOC_PAGE_FORMAT   *pFbAllocPageFormat
)
{
    NvU32 kind      = NV_MMU_PTE_KIND_INVALID;
    NvU32 zformat   = DRF_VAL(OS32, _ATTR, _Z_TYPE, pFbAllocPageFormat->attr);
    NvU32 zspacking = DRF_VAL(OS32, _ATTR, _ZS_PACKING, pFbAllocPageFormat->attr);
    NvU32 depth     = DRF_VAL(OS32, _ATTR, _DEPTH, pFbAllocPageFormat->attr);

    if (zspacking == NVOS32_ATTR_ZS_PACKING_S8 && depth == NVOS32_ATTR_DEPTH_8)
    {
        kind = NV_MMU_PTE_KIND_S8;
    }
    else
    {
        if (zformat == NVOS32_ATTR_Z_TYPE_FIXED)
        {
            switch (zspacking)
            {
                case NVOS32_ATTR_ZS_PACKING_Z16:
                {
                    kind = NV_MMU_PTE_KIND_Z16;
                    break;
                }
                case NVOS32_ATTR_ZS_PACKING_S8Z24:
                case NVOS32_ATTR_ZS_PACKING_X8Z24:
                {
                    kind = NV_MMU_PTE_KIND_S8Z24;
                    break;
                }
                case NVOS32_ATTR_ZS_PACKING_Z24S8:
                case NVOS32_ATTR_ZS_PACKING_Z24X8:
                {
                    kind = NV_MMU_PTE_KIND_Z24S8;
                    break;
                }
            }
        }
        else if (zformat == NVOS32_ATTR_Z_TYPE_FLOAT)
        {
            switch (zspacking)
            {
                case NVOS32_ATTR_ZS_PACKING_Z32:
                {
                    kind = NV_MMU_PTE_KIND_GENERIC_MEMORY;
                    break;
                }
                case NVOS32_ATTR_ZS_PACKING_Z32_X24S8:
                {
                    kind = NV_MMU_PTE_KIND_ZF32_X24S8;
                    break;
                }
            }
        }
    }

    return kind;

}

/*!
 * @brief Determine the kind of compressed PTE with PLC disabled for a given allocation.
 *
 * @param[in] pFbAllocPageFormat FB_ALLOC_PAGE_FORMAT pointer
 *
 * @returns   PTE kind.
 */
NvU32
memmgrChooseKindCompressZ_TU102
(
    OBJGPU                 *pGpu,
    MemoryManager          *pMemoryManager,
    FB_ALLOC_PAGE_FORMAT   *pFbAllocPageFormat
)
{
    NvU32 kind      = NV_MMU_PTE_KIND_INVALID;
    NvU32 zformat   = DRF_VAL(OS32, _ATTR, _Z_TYPE, pFbAllocPageFormat->attr);
    NvU32 zspacking = DRF_VAL(OS32, _ATTR, _ZS_PACKING, pFbAllocPageFormat->attr);
    NvU32 depth     = DRF_VAL(OS32, _ATTR, _DEPTH, pFbAllocPageFormat->attr);

    kind = NV_MMU_PTE_KIND_INVALID;

    if (zspacking == NVOS32_ATTR_ZS_PACKING_S8 && depth == NVOS32_ATTR_DEPTH_8)
    {
        kind = NV_MMU_PTE_KIND_S8_COMPRESSIBLE_DISABLE_PLC;
    }
    else
    {
        if (zformat == NVOS32_ATTR_Z_TYPE_FIXED)
        {
            switch (zspacking)
            {
                case NVOS32_ATTR_ZS_PACKING_Z16:
                {
                    kind = NV_MMU_PTE_KIND_Z16_COMPRESSIBLE_DISABLE_PLC;
                    break;
                }
                case NVOS32_ATTR_ZS_PACKING_S8Z24:
                case NVOS32_ATTR_ZS_PACKING_X8Z24:
                {
                    kind = NV_MMU_PTE_KIND_S8Z24_COMPRESSIBLE_DISABLE_PLC;
                    break;
                }
                case NVOS32_ATTR_ZS_PACKING_Z24S8:
                case NVOS32_ATTR_ZS_PACKING_Z24X8:
                {
                    kind = NV_MMU_PTE_KIND_Z24S8_COMPRESSIBLE_DISABLE_PLC;
                    break;
                }
            }
        }
        else if (zformat == NVOS32_ATTR_Z_TYPE_FLOAT)
        {
            switch (zspacking)
            {
                case NVOS32_ATTR_ZS_PACKING_Z32:
                {
                    kind = NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE_DISABLE_PLC;
                    break;
                }
                case NVOS32_ATTR_ZS_PACKING_Z32_X24S8:
                {
                    kind = NV_MMU_PTE_KIND_ZF32_X24S8_COMPRESSIBLE_DISABLE_PLC;
                    break;
                }
            }
        }
    }

    return kind;
}

/*!
 * @brief Choose kind for a surface
 *
 * @param[in] pFbAllocPageFormat User Alloc Requirements
 * @param[in] comprAttr      Compression attribute
 *
 * @returns compression kind
 */
NvU32
memmgrChooseKind_TU102
(
    OBJGPU                 *pGpu,
    MemoryManager          *pMemoryManager,
    FB_ALLOC_PAGE_FORMAT   *pFbAllocPageFormat,
    NvU32                   comprAttr,
    NvU32                  *pKind
)
{
    NvU32     kind            = NV_MMU_PTE_KIND_INVALID;
    NvBool    bRmToChooseKind = NV_TRUE;
    NV_STATUS status          = NV_OK;
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_EXTERNAL_HEAP_CONTROL) &&
        !(pFbAllocPageFormat->flags & NVOS32_ALLOC_FLAGS_SKIP_RESOURCE_ALLOC) &&
        !hypervisorIsVgxHyper())
    {
        bRmToChooseKind = NV_FALSE;
        kind = pFbAllocPageFormat->kind;

        if ( comprAttr == NVOS32_ATTR_COMPR_NONE &&
            memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, kind))
        {
            //
            // If client passes a compressible PTE kind but sets
            // NVOS32_ATTR_COMPR_NONE, then RM will need to choose
            // an uncompressible kind for the client, instead of
            // using the kind from the client.
            //
            NV_PRINTF(LEVEL_ERROR,
                      "Client sets a compressible PTE kind 0x%x, while sets "
                      "NVOS32_ATTR_COMPR_NONE. RM will ignore the PTE kind from client and "
                      "choose an uncompressible kind instead.\n", kind);
            bRmToChooseKind = NV_TRUE;
        }
    }

    if (bRmToChooseKind)
    {
        kind = NV_MMU_PTE_KIND_INVALID;

        {
            switch (pFbAllocPageFormat->type)
            {
                case NVOS32_TYPE_IMAGE:
                case NVOS32_TYPE_TEXTURE:
                case NVOS32_TYPE_VIDEO:
                case NVOS32_TYPE_CURSOR:
                case NVOS32_TYPE_DMA:
                case NVOS32_TYPE_PRIMARY:
                case NVOS32_TYPE_UNUSED:
                case NVOS32_TYPE_SHADER_PROGRAM:
                case NVOS32_TYPE_OWNER_RM:
                case NVOS32_TYPE_NOTIFIER:
                case NVOS32_TYPE_RESERVED:
                case NVOS32_TYPE_PMA:
                {
                    if (comprAttr == NVOS32_ATTR_COMPR_NONE || pMemorySystemConfig->bDisableCompbitBacking)
                    {
                        kind = NV_MMU_PTE_KIND_GENERIC_MEMORY;
                    }
                    else if (comprAttr == NVOS32_ATTR_COMPR_DISABLE_PLC_ANY ||
                             pMemorySystemConfig->bDisablePostL2Compression)
                    {
                        kind = NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE_DISABLE_PLC;
                    }
                    else
                    {
                        kind = NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE;
                    }
                    break;
                }
                case NVOS32_TYPE_DEPTH:
                case NVOS32_TYPE_STENCIL:
                {
                    if (comprAttr == NVOS32_ATTR_COMPR_NONE)
                    {
                        kind = memmgrChooseKindZ_HAL(pGpu, pMemoryManager, pFbAllocPageFormat);
                    }
                    else
                    {
                        kind = memmgrChooseKindCompressZ_HAL(pGpu, pMemoryManager, pFbAllocPageFormat);
                    }
                    break;
                }
            }
        }
    }

    *pKind = kind;

    if (kind == NV_MMU_PTE_KIND_INVALID)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to set a kind, dumping attributes:comprAttr = 0x%x, type = "
                  "0x%x, attr = 0x%x\n", comprAttr, pFbAllocPageFormat->type,
                  pFbAllocPageFormat->attr);
    }

    return status;
}

/*
 * @brief Return an uncompressible kind for the given kind.  There are two
 * modes of operation, one for ReleaseReacquire and the full fledged
 * mode.
 * @param[in] NvU32   - PTE kind
 * @param[in] NvBool  - ReleaseReacquire / full-fledge mode
 *
 * @returns NvU32 - Uncompressed kind for the compressed PTE kind type, or it will Assert
 */
NvU32
memmgrGetUncompressedKind_TU102
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU32          kind,
    NvBool         bReleaseReacquire
)
{
    //
    // This check is to return GENERIC_MEMORY if mods want to bypass uncompression
    // But it doesn't work for S8 surfaces, so returning the uncompressed kind for
    // that only
    //
    if (bReleaseReacquire && kind != NV_MMU_PTE_KIND_S8_COMPRESSIBLE_DISABLE_PLC)
    {
        return NV_MMU_PTE_KIND_GENERIC_MEMORY;
    }

    switch (kind)
    {
        case NV_MMU_PTE_KIND_GENERIC_MEMORY:
        case NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE:
        case NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE_DISABLE_PLC:
            return NV_MMU_PTE_KIND_GENERIC_MEMORY;
        case NV_MMU_PTE_KIND_S8:
        case NV_MMU_PTE_KIND_S8_COMPRESSIBLE_DISABLE_PLC:
            return NV_MMU_PTE_KIND_S8;
        case NV_MMU_PTE_KIND_Z16:
        case NV_MMU_PTE_KIND_Z16_COMPRESSIBLE_DISABLE_PLC:
            return NV_MMU_PTE_KIND_Z16;
        case NV_MMU_PTE_KIND_S8Z24:
        case NV_MMU_PTE_KIND_S8Z24_COMPRESSIBLE_DISABLE_PLC:
            return NV_MMU_PTE_KIND_S8Z24;
        case NV_MMU_PTE_KIND_Z24S8:
        case NV_MMU_PTE_KIND_Z24S8_COMPRESSIBLE_DISABLE_PLC:
            return NV_MMU_PTE_KIND_Z24S8;
        case NV_MMU_PTE_KIND_ZF32_X24S8:
        case NV_MMU_PTE_KIND_ZF32_X24S8_COMPRESSIBLE_DISABLE_PLC:
            return NV_MMU_PTE_KIND_ZF32_X24S8;
        case NV_MMU_PTE_KIND_PITCH:
            return NV_MMU_PTE_KIND_PITCH;
        default:
        {
            NV_PRINTF(LEVEL_ERROR, "Unknown kind 0x%x.\n", kind);
            DBG_BREAKPOINT();

            return NV_MMU_PTE_KIND_INVALID;
        }
    }
}

NvU32
memmgrGetCompressedKind_TU102
(
    MemoryManager *pMemoryManager,
    NvU32          kind,
    NvBool         bDisablePlc
)
{
    switch (kind)
    {
        case NV_MMU_PTE_KIND_GENERIC_MEMORY:
        case NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE:
        case NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE_DISABLE_PLC:
            return bDisablePlc ? NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE_DISABLE_PLC
                               : NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE;
        case NV_MMU_PTE_KIND_S8:
        case NV_MMU_PTE_KIND_S8_COMPRESSIBLE_DISABLE_PLC:
            return NV_MMU_PTE_KIND_S8_COMPRESSIBLE_DISABLE_PLC;
        case NV_MMU_PTE_KIND_Z16:
        case NV_MMU_PTE_KIND_Z16_COMPRESSIBLE_DISABLE_PLC:
            return NV_MMU_PTE_KIND_Z16_COMPRESSIBLE_DISABLE_PLC;
        case NV_MMU_PTE_KIND_S8Z24:
        case NV_MMU_PTE_KIND_S8Z24_COMPRESSIBLE_DISABLE_PLC:
            return NV_MMU_PTE_KIND_S8Z24_COMPRESSIBLE_DISABLE_PLC;
        case NV_MMU_PTE_KIND_Z24S8:
        case NV_MMU_PTE_KIND_Z24S8_COMPRESSIBLE_DISABLE_PLC:
            return NV_MMU_PTE_KIND_Z24S8_COMPRESSIBLE_DISABLE_PLC;
        case NV_MMU_PTE_KIND_ZF32_X24S8:
        case NV_MMU_PTE_KIND_ZF32_X24S8_COMPRESSIBLE_DISABLE_PLC:
            return NV_MMU_PTE_KIND_ZF32_X24S8_COMPRESSIBLE_DISABLE_PLC;
        default:
        {
            NV_PRINTF(LEVEL_ERROR, "Unknown kind 0x%x.\n", kind);
            return NV_MMU_PTE_KIND_INVALID;
        }
    }
}

/*!
 * @brief Get the proper NV_MMU_PTE_KIND_SMSKED_MESSAGE kind.
 *
 * @returns NV_MMU_PTE_KIND_SMSKED_MESSAGE.
 */
NvU32
memmgrGetMessageKind_TU102
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    return NV_MMU_PTE_KIND_SMSKED_MESSAGE;
}

/*!
 * @brief Returns default PTE kind for no memory handle.
 *
 * @return The default PTE kind.
 */
NvU32
memmgrGetDefaultPteKindForNoHandle_TU102
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    return NV_MMU_PTE_KIND_GENERIC_MEMORY;
}

/*!
 * @brief Returns whether the kind can be compressible or not.
 *
 * @param[in] kind  NvU32  Kind value
 *
 * @returns NV_FALSE, if the kind is not compressible
 *          NV_TRUE, if the kind is compressible
 */
NvBool
memmgrIsKindCompressible_TU102
(
    MemoryManager *pMemoryManager,
    NvU32          kind
)
{
    return memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, kind);
}

/*!
 * @brief Get Ctag offset from FB_ALLOC_INFO
 *
 * @param[in] pFbAllocInfo     pointer to FB_ALLOC_INFO
 *
 * @returns a valid NvU32 (CTAG OFFSET) stored in the FB_ALLOC_INFO structure by RM
 *
 */
NvU32
memmgrGetCtagOffsetFromParams_TU102
(
    OBJGPU          *pGpu,
    MemoryManager   *pMemoryManager,
    FB_ALLOC_INFO   *pFbAllocInfo
)
{
    // Ensure max possible size of ctagOffset fits in comptagline
    ct_assert(DRF_VAL(OS32, _ALLOC, _COMPTAG_OFFSET_START, NV_U32_MAX) <
              NVBIT32(GPU_DRF_WIDTH(NV_MMU_PTE_COMPTAGLINE)));

    return DRF_VAL(OS32, _ALLOC, _COMPTAG_OFFSET_START, pFbAllocInfo->ctagOffset);
}

/*!
 * @brief Set Ctag offset in FB_ALLOC_INFO structure from the ctag offset input
 *
 * @param[in/out] pFbAllocInfo     pointer to FB_ALLOC_INFO
 * @param[in] comptagOffset    comptag offset that needs to be stored in FB_ALLOC_INFO
 *
 *
 */
void
memmgrSetCtagOffsetInParams_TU102
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    FB_ALLOC_INFO *pFbAllocInfo,
    NvU32          comptagOffset
)
{
    if (comptagOffset < (NVBIT32(GPU_DRF_WIDTH(NV_MMU_PTE_COMPTAGLINE))))
    {
        pFbAllocInfo->ctagOffset =  FLD_SET_DRF_NUM(OS32, _ALLOC, _COMPTAG_OFFSET_START, comptagOffset,
                                    pFbAllocInfo->ctagOffset);
        NV_PRINTF(LEVEL_INFO, "Setting ctag offset before allocating: %x\n",
                  pFbAllocInfo->ctagOffset);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "- comptagline offset is outside the bounds, offset: %x, limit:%x.\n",
                  comptagOffset, NVBIT32(GPU_DRF_WIDTH(NV_MMU_PTE_COMPTAGLINE)));
    }
}

/*!
 * @brief Determine comptag offset from physical address
 *
 * @param[out]physAddr         physical address for the surface
 *
 * @returns comptag offset from physical address of the surface
 */
NvU32
memmgrDetermineComptag_TU102
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    RmPhysAddr     physAddr
)
{
    return 1 + NvU64_LO32(DRF_VAL64(_MMU, _PTE_COMPTAGLINE, _BITS_FROM_SPA, physAddr));
}

/*!
 * @brief This function converts RM_DEFAULT_PTE_KIND to the chip specific kind
 *
 *@param[in]   pteKind        PTE Kind
 *
 * @returns the HW PTE Kind
 *
 */
NvU32
memmgrGetHwPteKindFromSwPteKind_TU102
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU32          pteKind
)
{
    if (pteKind == RM_DEFAULT_PTE_KIND)
        return NV_MMU_PTE_KIND_GENERIC_MEMORY;
    else
        return pteKind;
}

/*!
 * @brief This function converts from chip specific kind to RM_DEFAULT_PTE_KIND
 *
 * @param[in]   pteKind        PTE Kind
 *
 * @returns the Chip Specific Kind/SW PTE KIND
 *
 */
NvU32
memmgrGetSwPteKindFromHwPteKind_TU102
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU32          pteKind
)
{
    if (pteKind == NV_MMU_PTE_KIND_GENERIC_MEMORY)
        return RM_DEFAULT_PTE_KIND;
    else
        return pteKind;
}

/*!
 * @brief This function returns Kind to be used for Scrub on free
 *
 * @param[out]   pteKind        PTE Kind to be returned
 *
 * @returns void
 *
 */
void
memmgrGetPteKindForScrubber_TU102
(
    MemoryManager *pMemoryManager,
    NvU32          *pteKind
)
{
    if (pteKind == NULL)
        return;

    *pteKind = NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE_DISABLE_PLC;
}

/*!
 *  Returns the max context size
 *
 *  @returns NvU64
 */
NvU64
memmgrGetMaxContextSize_TU102
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    extern NvU64 memmgrGetMaxContextSize_GV100(OBJGPU *pGpu, MemoryManager *pMemoryManager);

    NvU64  size = memmgrGetMaxContextSize_GV100(pGpu, pMemoryManager);

    if (RMCFG_FEATURE_PLATFORM_MODS)
    {
        if (memmgrIsPmaInitialized(pMemoryManager))
        {
            // Double the context size
            size *= 2;
        }
    }

    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        if (gpuIsClientRmAllocatedCtxBufferEnabled(pGpu))
        {
            //
            // When ctx buffer management is in CPU-RM, GSP-RM needs extra
            // 32 MBs to meet max CUDA context allocation requirement
            // (Including 16MB reservation for possible global ctx buffers
            // allocation)
            //
            size += 32 * 1024 * 1024;
        }
        else
        {
            //
            // When ctx buffer management is in GSP-RM, GSP-RM needs extra
            // 160 MBs to meet max CUDA context allocation requirement
            //
            size += 160 * 1024 * 1024;
        }
    }

    return size;
}


/*!
 * Calculates heap offset based on presence and size of console and CBC regions
 *
 * When GSP is in use, CBC is placed at the beginning of memory instead of the
 * end, which means it must be taken into account when calculating where the
 * heap starts. If the offset is successfully calculated, it is placed in the
 * offset variable and NV_OK is returned
 *
 *  @returns NV_STATUS
 */
NV_STATUS
memmgrCalculateHeapOffsetWithGSP_TU102
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU32         *offset
)
{
    // The heap will be located after the Console and CBC regions if they are
    // present. Zero, one, or both of them may be present. If Console and
    // CBC regions are present, they are guaranteed to be in Regions 0 and 1.
    // If only one is present, then it is guaranteed to be in Region 0. As an
    // extra check, it should be validated that these regions are indeed
    // reserved
    KernelMemorySystem   *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    FB_REGION_DESCRIPTOR *pFbRegion0          = &pMemoryManager->Ram.fbRegion[0];
    FB_REGION_DESCRIPTOR *pFbRegion1          = &pMemoryManager->Ram.fbRegion[1];

    NvBool isConsoleRegionPresent = pMemoryManager->Ram.ReservedConsoleDispMemSize != 0;
    NvBool isCbcRegionPresent     = !pKernelMemorySystem->pStaticConfig->bDisableCompbitBacking;

    // Both regions in use
    if (isCbcRegionPresent && isConsoleRegionPresent &&
        pFbRegion0->bRsvdRegion && pFbRegion1->bRsvdRegion)
    {
        // Use Region 1 limit and safely convert to KB
        *offset = NvU64_LO32((pFbRegion1->limit + 1) >> 10);
        NV_ASSERT_OR_RETURN(((NvU64) *offset << 10ULL) == (pFbRegion1->limit + 1),
                            NV_ERR_INVALID_DATA);
    }

    // One region in use
    else if ((isCbcRegionPresent || isConsoleRegionPresent) &&
             pFbRegion0->bRsvdRegion)
    {
        // Use Region 0 limit and safely convert to KB
        *offset = NvU64_LO32((pFbRegion0->limit + 1) >> 10);
        NV_ASSERT_OR_RETURN(((NvU64) *offset << 10ULL) == (pFbRegion0->limit + 1),
                            NV_ERR_INVALID_DATA);
    }

    // Neither region in use, return NOT_SUPPORTED to fall back to default calculation
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    return NV_OK;
}
