/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
*
*       Display Instance Memory Module
*       This file contains functions managing display on CPU RM
*
******************************************************************************/

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "gpu_mgr/gpu_mgr.h"
#include "gpu/disp/inst_mem/disp_inst_mem.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/context_dma.h"
#include "disp/v03_00/dev_disp.h"

/*!
 * @brief Get display instance memory and hash table size
 *
 * @param[in]  pGpu
 * @param[in]  PInstMem
 * @param[out] pTotalInstMemSize pointer to instance memory size
 * @param[out] pHashTableSize    pointer to hash table size
 *
 * @return void
 */
void
instmemGetSize_v03_00
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    NvU32                 *pTotalInstMemSize,
    NvU32                 *pHashTableSize
)
{
    if (pTotalInstMemSize != NULL)
    {
        *pTotalInstMemSize = (NV_UDISP_HASH_LIMIT - NV_UDISP_HASH_BASE + 1) +
                             (NV_UDISP_OBJ_MEM_LIMIT - NV_UDISP_OBJ_MEM_BASE + 1);
    }

    if (pHashTableSize != NULL)
    {
        *pHashTableSize = (NV_UDISP_HASH_LIMIT - NV_UDISP_HASH_BASE + 1);
    }
}

NvU32
instmemGetHashTableBaseAddr_v03_00
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem
)
{
    return NV_UDISP_HASH_BASE;
}

/*! Check if the instance memory pointer is valid */
NvBool
instmemIsValid_v03_00
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    NvU32                  offset
)
{
    return (((offset << 5) < NV_UDISP_OBJ_MEM_LIMIT) &&
            ((offset << 5) > NV_UDISP_HASH_LIMIT));
}

NV_STATUS
instmemHashFunc_v03_00
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    NvHandle               hClient,
    NvHandle               hContextDma,
    NvU32                  dispChannelNum,
    NvU32                 *pResult
)
{
    NV_ASSERT_OR_RETURN(pResult, NV_ERR_INVALID_ARGUMENT);


     //
     // The hash function for display will be:
     //    hContextDma[9:0]
     //  ^ hContextDma[19:10]
     //  ^ hContextDma[29:20]
     //  ^ {hClient[7:0], hContextDma[31:30]}
     //  ^ {dispChannelNum[3:0], hClient[13:8]}
     //  ^ {7'h00, dispChannelNum[6:4]}
     //
    *pResult = ((hContextDma >> 0)  & 0x3FF)                       ^
               ((hContextDma >> 10)  & 0x3FF)                      ^
               ((hContextDma >> 20) & 0x3FF)                       ^
               (((hClient & 0xFF) << 2) | (hContextDma >> 30))      ^
               (((dispChannelNum & 0xF) << 6) | ((hClient >> 8) & 0x3F))^
               ((dispChannelNum >> 4) & 0x7);

    return NV_OK;
}

/*! Generate hash table data */
NvU32
instmemGenerateHashTableData_v03_00
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    NvU32                  hClient,
    NvU32                  offset,
    NvU32                  dispChannelNum
)
{
    return (SF_NUM(_UDISP, _HASH_TBL_CLIENT_ID, hClient) |
            SF_NUM(_UDISP, _HASH_TBL_INSTANCE,  offset)  |
            SF_NUM(_UDISP, _HASH_TBL_CHN,       dispChannelNum));
}

/*! Write the Context DMA to display instance memory */
NV_STATUS
instmemCommitContextDma_v03_00
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    ContextDma            *pContextDma
)
{
    MEMORY_DESCRIPTOR  *pMemDesc        = memdescGetMemDescFromGpu(pContextDma->pMemDesc, pGpu);
    MemoryManager      *pMemoryManager  = GPU_GET_MEMORY_MANAGER(pGpu);
    RmPhysAddr          FrameAddr, Limit;
    RmPhysAddr          FrameAddr256Align;
    RmPhysAddr          LimitAlign;
    NvU32               ctxDMAFlag;
    NvU32               instoffset;
    NvU8               *pInstMemCpuVA;
    NvBool              bIsSurfaceBl    = NV_FALSE;
    TRANSFER_SURFACE    dest = {0};

    // This function must be called in unicast.
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    instoffset = pContextDma->Instance[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] << 4;
    NV_ASSERT_OR_RETURN(instoffset, NV_ERR_INVALID_OBJECT);

    FrameAddr = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
    Limit     = FrameAddr + pContextDma->Limit;

    //
    // Set surface format
    //
    switch (DRF_VAL(OS03, _FLAGS, _PTE_KIND, pContextDma->Flags))
    {
    case NVOS03_FLAGS_PTE_KIND_BL:
        bIsSurfaceBl = NV_TRUE;
        break;
    case NVOS03_FLAGS_PTE_KIND_PITCH:
        bIsSurfaceBl = NV_FALSE;
        break;
    case NVOS03_FLAGS_PTE_KIND_NONE:
        {
            NvU32 const kind = memdescGetPteKindForGpu(pMemDesc, pGpu);

            // Cannot bind a Z surface to display.  Bug 439965.
            if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_Z, kind))
                return NV_ERR_INVALID_ARGUMENT;

            bIsSurfaceBl = memmgrIsSurfaceBlockLinear_HAL(pMemoryManager,
                                                          pContextDma->pMemory,
                                                          kind);
        }
        break;
    default:
        NV_PRINTF(LEVEL_ERROR, "Unexpected PTE_KIND value\n");
        return NV_ERR_INVALID_STATE;
    }

    ctxDMAFlag = 0;

    if (bIsSurfaceBl)
    {
        ctxDMAFlag |= SF_DEF(_DMA, _KIND, _BLOCKLINEAR);
    }
    else
    {
        ctxDMAFlag |= SF_DEF(_DMA, _KIND, _PITCH);
    }

    if (pContextDma->bReadOnly)
    {
        ctxDMAFlag |= SF_DEF(_DMA, _ACCESS, _READ_ONLY);
    }
    else
    {
        ctxDMAFlag |= SF_DEF(_DMA, _ACCESS, _READ_AND_WRITE);
    }

    switch (memdescGetAddressSpace(pMemDesc))
    {
        case ADDR_SYSMEM:
        case ADDR_REGMEM:
            // SOC Display always need _PHYSICAL_NVM flag to be set as display is not over PCI
            if (pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY) ||
                pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_SOC_SDM))
            {
                ctxDMAFlag |= SF_DEF(_DMA, _TARGET_NODE, _PHYSICAL_NVM);
            }
            else
            {
                if (pContextDma->CacheSnoop)
                    ctxDMAFlag |= SF_DEF(_DMA, _TARGET_NODE, _PHYSICAL_PCI_COHERENT);
                else
                    ctxDMAFlag |= SF_DEF(_DMA, _TARGET_NODE, _PHYSICAL_PCI);
            }
            break;
        case ADDR_FBMEM:
            ctxDMAFlag |= SF_DEF(_DMA, _TARGET_NODE, _PHYSICAL_NVM);
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Invalid address space: %d\n",
                      memdescGetAddressSpace(pMemDesc));
            DBG_BREAKPOINT();
            return NV_ERR_INVALID_ARGUMENT;
    }

    dest.pMemDesc = pInstMem->pInstMemDesc;
    dest.offset = instoffset;

    pInstMemCpuVA = memmgrMemBeginTransfer(pMemoryManager, &dest, NV_DMA_SIZE,
                        TRANSFER_FLAGS_SHADOW_ALLOC);
    if (pInstMemCpuVA == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    MEM_WR32(pInstMemCpuVA + SF_OFFSET(NV_DMA_TARGET_NODE), ctxDMAFlag);    // word 0

    // Address in disp ctxdma is 256B aligned
    FrameAddr256Align = FrameAddr >> 8;
    MEM_WR32(pInstMemCpuVA + SF_OFFSET(NV_DMA_ADDRESS_BASE_LO),             // word 1
             NvU64_LO32(FrameAddr256Align));
    MEM_WR32(pInstMemCpuVA + SF_OFFSET(NV_DMA_ADDRESS_BASE_HI),             // word 2
             NvU64_HI32(FrameAddr256Align));

    LimitAlign = Limit >> 8;
    MEM_WR32(pInstMemCpuVA + SF_OFFSET(NV_DMA_ADDRESS_LIMIT_LO),            // word 3
             NvU64_LO32(LimitAlign));
    MEM_WR32(pInstMemCpuVA + SF_OFFSET(NV_DMA_ADDRESS_LIMIT_HI),            // word 4
             NvU64_HI32(LimitAlign));

    memmgrMemEndTransfer(pMemoryManager, &dest, NV_DMA_SIZE,
        TRANSFER_FLAGS_SHADOW_ALLOC);

    return NV_OK;
}

/*!
 * @brief Update the Context DMA already in display instance memory
 *
 * NOTE: this control call may be called at high IRQL on WDDM.
 */
NV_STATUS
instmemUpdateContextDma_v03_00
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    ContextDma            *pContextDma,
    NvU64                 *pNewAddress,
    NvU64                 *pNewLimit,
    NvHandle               hMemory,
    NvU32                  comprInfo
)
{
    MemoryManager     *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS          status         = NV_OK;
    NvU8              *pInst;
    NvU32              instoffset;
    TRANSFER_SURFACE   dest = {0};

    // Must use comprInfo to specify kind
    NV_CHECK_OR_RETURN(LEVEL_SILENT, hMemory == NV01_NULL_OBJECT, NV_ERR_INVALID_ARGUMENT);

    instoffset = pContextDma->Instance[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] << 4;
    NV_ASSERT(instoffset);

    dest.pMemDesc = pInstMem->pInstMemDesc;
    dest.offset = instoffset;

    pInst = memmgrMemBeginTransfer(pMemoryManager, &dest, NV_DMA_SIZE,
                TRANSFER_FLAGS_SHADOW_ALLOC | TRANSFER_FLAGS_SHADOW_INIT_MEM);
    if (pInst == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    if (pNewAddress != NULL)
    {
        // Address in disp ctxdma is 256B aligned
        NvU64 newAddress256Align = (*pNewAddress) >> 8;
        MEM_WR32(pInst + SF_OFFSET(NV_DMA_ADDRESS_BASE_LO),
                 NvU64_LO32(newAddress256Align));
        MEM_WR32(pInst + SF_OFFSET(NV_DMA_ADDRESS_BASE_HI),
                 NvU64_HI32(newAddress256Align));
    }

    if (pNewLimit != NULL)
    {
        NvU64 newLimitAlign = (*pNewLimit) >> 8;
        MEM_WR32(pInst + SF_OFFSET(NV_DMA_ADDRESS_LIMIT_LO),
                 NvU64_LO32(newLimitAlign));
        MEM_WR32(pInst + SF_OFFSET(NV_DMA_ADDRESS_LIMIT_HI),
                 NvU64_HI32(newLimitAlign));
    }

    if (comprInfo != NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_USE_COMPR_INFO_NONE)
    {
        NvU32 word = MEM_RD32(pInst + SF_OFFSET(NV_DMA_KIND));

        if (comprInfo == NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_USE_COMPR_INFO_FORMAT_BLOCK_LINEAR)
        {
            word = FLD_SF_DEF(_DMA, _KIND, _BLOCKLINEAR, word);
        }
        else
        {
            word = FLD_SF_DEF(_DMA, _KIND, _PITCH, word);
        }

        MEM_WR32(pInst + SF_OFFSET(NV_DMA_KIND), word);
    }

    memmgrMemEndTransfer(pMemoryManager, &dest, NV_DMA_SIZE,
        TRANSFER_FLAGS_SHADOW_ALLOC | TRANSFER_FLAGS_SHADOW_INIT_MEM);

exit:

    return status;
}
