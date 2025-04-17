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

#include "core/core.h"
#include "gpu/gpu.h"
#include "os/os.h"
#include "kernel/gpu/mem_sys/kern_mem_sys.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "ctrl/ctrl2080/ctrl2080fb.h"

#include "published/ampere/ga100/dev_fb.h"
#include "published/ampere/ga100/dev_fuse.h"
#include "published/ampere/ga100/hwproject.h"

/*!
 * @brief Write the sysmemFlushBuffer val into the NV_PFB_NISO_FLUSH_SYSMEM_ADDR register
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in[ pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns void
 */
void
kmemsysProgramSysmemFlushBuffer_GA100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU64 alignedSysmemFlushBufferAddr = 0x0;
    NvU32 alignedSysmemFlushBufferAddrHi = 0x0;

    NV_ASSERT(pKernelMemorySystem->sysmemFlushBuffer != 0);

    alignedSysmemFlushBufferAddr = pKernelMemorySystem->sysmemFlushBuffer >> kmemsysGetFlushSysmemBufferAddrShift_HAL(pGpu, pKernelMemorySystem);
    alignedSysmemFlushBufferAddrHi = DRF_VAL(_PFB, _NISO_FLUSH_SYSMEM_ADDR_HI, _ADR_63_40,
                                            NvU64_HI32(alignedSysmemFlushBufferAddr));

    NV_ASSERT((alignedSysmemFlushBufferAddrHi & (~NV_PFB_NISO_FLUSH_SYSMEM_ADDR_HI_MASK)) == 0);

    alignedSysmemFlushBufferAddrHi &= NV_PFB_NISO_FLUSH_SYSMEM_ADDR_HI_MASK;

    GPU_FLD_WR_DRF_NUM(pGpu, _PFB, _NISO_FLUSH_SYSMEM_ADDR_HI, _ADR_63_40,
                    alignedSysmemFlushBufferAddrHi);
    GPU_FLD_WR_DRF_NUM(pGpu, _PFB, _NISO_FLUSH_SYSMEM_ADDR, _ADR_39_08,
                    NvU64_LO32(alignedSysmemFlushBufferAddr));
}

/*
 * @brief Initialize the sysmem flush buffer
 *
 * Setting up the sysmem flush buffer needs to be done very early in some cases
 * as it's required for the GPU to perform a system flush. One such case is
 * resetting GPU FALCONs and in particular resetting the PMU as part of VBIOS
 * init.
 *
 * @returns NV_OK if all is okay.  Otherwise an error-specific value.
 */
NV_STATUS
kmemsysInitFlushSysmemBuffer_GA100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NV_STATUS status;

    //
    // In case of suspend/resume, the buffer might be already allocated, but
    // the HW still needs to be programmed below.
    //
    if (pKernelMemorySystem->pSysmemFlushBufferMemDesc == NULL)
    {
        NvU64 flags = MEMDESC_FLAGS_NONE;

            flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;
        //
        // Sysmem flush buffer
        // The sysmembar flush does a zero byte read of sysmem if there was a
        // sysmem write since the last flush. The actual memory does have
        // to be valid and allocated at all times because an actual read may
        // be issued.
        //
        status = memdescCreate(&pKernelMemorySystem->pSysmemFlushBufferMemDesc,
                               pGpu, RM_PAGE_SIZE,
                               1 << kmemsysGetFlushSysmemBufferAddrShift_HAL(pGpu, pKernelMemorySystem),
                               NV_TRUE,
                               ADDR_SYSMEM,
                               NV_MEMORY_UNCACHED,
                               flags);
        if (status != NV_OK)
            return status;

        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_139, 
                        pKernelMemorySystem->pSysmemFlushBufferMemDesc);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not allocate sysmem flush buffer: %x\n", status);
            DBG_BREAKPOINT();
            return status;
        }

        pKernelMemorySystem->sysmemFlushBuffer = memdescGetPhysAddr(pKernelMemorySystem->pSysmemFlushBufferMemDesc, AT_GPU, 0);
    }

    kmemsysProgramSysmemFlushBuffer_HAL(pGpu, pKernelMemorySystem);
    return NV_OK;
}

/*!
 * @brief Validate the sysmemFlushBuffer val and assert
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in[ pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns void
 */
void
kmemsysAssertSysmemFlushBufferValid_GA100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NV_ASSERT((GPU_REG_RD_DRF(pGpu, _PFB, _NISO_FLUSH_SYSMEM_ADDR, _ADR_39_08) != 0)
               || (GPU_REG_RD_DRF(pGpu, _PFB,  _NISO_FLUSH_SYSMEM_ADDR_HI, _ADR_63_40) != 0));
}

/*!
 * @brief  Read MIG Memory CFG register
 */
NV_STATUS
kmemsysReadMIGMemoryCfg_GA100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS params = {0};

    NV_ASSERT_OK_OR_RETURN(
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_KMEMSYS_GET_MIG_MEMORY_CONFIG,
                        &params,
                        sizeof(params)));

    pKernelMemorySystem->memBoundaryCfgTable.memBoundaryCfgA = params.memBoundaryCfgA;
    pKernelMemorySystem->memBoundaryCfgTable.memBoundaryCfgB = params.memBoundaryCfgB;
    pKernelMemorySystem->memBoundaryCfgTable.memBoundaryCfgC = params.memBoundaryCfgC;

    return NV_OK;
}

/*!
 * @brief  Read MIG Memory partition table
 */
NV_STATUS
kmemsysInitMIGMemoryPartitionTable_GA100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    if (!pKernelMemorySystem->bDisablePlcForCertainOffsetsBug3046774)
        return NV_OK;

    NV_ASSERT_OK_OR_RETURN(
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE,
                        &pKernelMemorySystem->migMemoryPartitionTable,
                        sizeof(pKernelMemorySystem->migMemoryPartitionTable)));

    return NV_OK;
}

/*!
 * @brief   Function to map swizzId to mem range given total range in Fb
 */
static NV_STATUS
_kmemsysSwizzIdToFbMemRange_GA100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32 swizzId,
    NvU64 vmmuSegmentSize,
    NV_RANGE totalRange,
    NV_RANGE *pAddrRange
)
{
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kmemsysSwizzIdToMIGMemRange(pGpu, pKernelMemorySystem, swizzId, totalRange, pAddrRange));

    //
    // If pAddrRange->lo is vmmuSegment aligned, then alignedUp
    // by one segment else simply align it. We need to make sure we have
    // atleast 1 VMMU segment delta between consecutive segments
    //
    if (pAddrRange->lo != 0)
    {
        pAddrRange->lo = NV_IS_ALIGNED64(pAddrRange->lo, vmmuSegmentSize) ?
                        pAddrRange->lo + vmmuSegmentSize :
                        NV_ALIGN_UP64(pAddrRange->lo, vmmuSegmentSize);
    }

    return NV_OK;
}

/*!
 * @brief   Function to map swizzId to VMMU Segments
 */
NV_STATUS
kmemsysSwizzIdToVmmuSegmentsRange_GA100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32 swizzId,
    NvU32 vmmuSegmentSize,
    NvU32 totalVmmuSegments
)
{
    //
    // This parameter represents the number of boundaries drawn when a
    // specific GPU instance type is created
    //
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU32          numBoundaries = 0;
    NvU32          partitionDivFactor = 1;
    NV_RANGE       addrRange = NV_RANGE_EMPTY;
    NV_RANGE       partitionableMemoryRange = memmgrGetMIGPartitionableMemoryRange(pGpu, pMemoryManager);
    NvU64          startingVmmuSegment;
    NvU64          memSizeInVmmuSegment;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _kmemsysSwizzIdToFbMemRange_GA100(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, partitionableMemoryRange, &addrRange));

    switch (swizzId)
    {
        case 0:
        {
            numBoundaries = 0;
            partitionDivFactor = 1;
            break;
        }
        case 1:
        case 2:
        {
            numBoundaries = 1;
            partitionDivFactor = 2;
            break;
        }
        case 3:
        case 4:
        case 5:
        case 6:
        {
            numBoundaries = 3;
            partitionDivFactor = 4;
            break;
        }
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        {
            numBoundaries = 7;
            partitionDivFactor = 8;
            break;
        }
    }

    startingVmmuSegment = addrRange.lo / vmmuSegmentSize;
    memSizeInVmmuSegment = (totalVmmuSegments - numBoundaries) / partitionDivFactor;

    NV_ASSERT_OK_OR_RETURN(
        kmemsysInitMIGGPUInstanceMemConfigForSwizzId(pGpu, pKernelMemorySystem, swizzId, startingVmmuSegment, memSizeInVmmuSegment));

    return NV_OK;
}

NvBool
kmemsysIsPagePLCable_GA100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU64               physAddr,
    NvU64               pageSize
)
{
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig = kmemsysGetStaticConfig(pGpu, pKernelMemorySystem);
    NvU64 topAddr       = ((pKernelMemorySystem->memBoundaryCfgTable.memBoundaryCfgB + pKernelMemorySystem->memBoundaryCfgTable.memBoundaryCfgA) *
                          pMemorySystemConfig->ltcCount * pMemorySystemConfig->ltsPerLtcCount) >> 4;
    NvU64 bottomAddr    = (pKernelMemorySystem->memBoundaryCfgTable.memBoundaryCfgA * pMemorySystemConfig->ltcCount *
                          pMemorySystemConfig->ltsPerLtcCount) >> 4;
    NvU64 secureTopAddr = pKernelMemorySystem->memBoundaryCfgTable.memBoundaryCfgC;
    NvU64 revBottomAddr = pKernelMemorySystem->memBoundaryCfgTable.memBoundaryCfgA << 5;
    NvBool bPageSize2M  = (pageSize == (2 << 20));
    NvU64 partitionedMemorySize = ((topAddr - bottomAddr) << 16);
    NvU32 blackBlockOffset = 2048 - (NvU32)((revBottomAddr >> 1) % 2048);
    NvU32 swizzId = 0;

    if (pMemorySystemConfig->ltsPerLtcCount * pMemorySystemConfig->ltcCount != 80)
        return NV_TRUE;

     if (!((pMemorySystemConfig->ltsPerLtcCount != 4) || ((pMemorySystemConfig->ltcCount % 4) != 0) || (pMemorySystemConfig->ltcCount < 4)) &&
         !(topAddr >= secureTopAddr) &&
         !(physAddr >= (secureTopAddr << 16)) &&
         !((physAddr < (bottomAddr << 16)) || (physAddr >= (topAddr << 16))))
     {
         NvU32 partition_id = (NvU32)((physAddr - (bottomAddr << 16)) / (partitionedMemorySize / 8));
         NV_ASSERT_OR_RETURN(partition_id < 8, NV_TRUE);
         swizzId = pKernelMemorySystem->migMemoryPartitionTable.data[partition_id];
     }


    switch (swizzId)
    {
    case 0:
        if (!bPageSize2M)
        {
            return ((physAddr % (160 * 256 * 1024)) >= (3 * 256 * 1024));
        }
        else
        {
            NvBool noPLC = NV_FALSE;

            for (NvU64 addr = physAddr; (addr < physAddr + (2 * 1024 * 1024)); addr += (256 * 1024))
            {
                noPLC |= ((addr % (160 * 256 * 1024)) < (3 * 256 * 1024));
            }

            return !noPLC;
        }
    case 1:
    case 2:
    {
        NvU64 rebasePA = physAddr - (bottomAddr << 16) - ((partitionedMemorySize / 2) * (swizzId - 1)) +
                         160 * 64 * 1024 - (blackBlockOffset * 10 / 128) * (128 * 1024);

        if (!bPageSize2M)
        {
            return ((rebasePA % (160 * 128 * 1024)) >= (4 * 128 * 1024));
        }
        else
        {
            NvBool noPLC = NV_FALSE;

            for (NvU64 addr = rebasePA; (addr < rebasePA + (2 * 1024 * 1024)); addr += (128 * 1024))
            {
                noPLC |= ((addr % (160 * 128 * 1024)) < (4 * 128 * 1024));
            }

            return !noPLC;
        }
    }
    case 3:
    case 4:
    case 5:
    case 6:
    {
        NvU64 rebasePA = physAddr - (bottomAddr << 16) - ((partitionedMemorySize / 4) * (swizzId - 3)) +
                         160 * 64 * 1024 - (blackBlockOffset * 10 / 128) * (64 * 1024);

        if (!bPageSize2M)
        {
            return ((rebasePA % (160 * 64 * 1024)) >= (4 * 64 * 1024));
        }
        else
        {
            NvBool noPLC = NV_FALSE;

            for (NvU64 addr = rebasePA; (addr < rebasePA + (2 * 1024 * 1024)); addr += (64 * 1024))
            {
                noPLC |= ((addr % (160 * 64 * 1024)) < (4 * 64 * 1024));
            }

            return !noPLC;
        }
    }
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    {
        NvU64 rebasePA = physAddr - (bottomAddr << 16) - ((partitionedMemorySize / 8) * (swizzId - 7)) +
                         80 * 64 * 1024 - (blackBlockOffset * 10 / 256) * (64 * 1024);

        if (!bPageSize2M)
        {
            return ((rebasePA % (80 * 64 * 1024)) >= (3 * 64 * 1024));
        }
        else
        {
            NvBool noPLC = NV_FALSE;

            for (NvU64 addr = rebasePA; (addr < rebasePA + (2 * 1024 * 1024)); addr += (64 * 1024))
            {
                noPLC |= ((addr % (80 * 64 * 1024)) < (3 * 64 * 1024));
            }

            return !noPLC;
        }
    }
    default:
        return NV_TRUE;
    }
}

NvU16
kmemsysGetMaximumBlacklistPages_GA100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    return NV2080_CTRL_FB_DYNAMIC_BLACKLIST_MAX_PAGES;
}

NvU32
kmemsysGetMaxFbpas_GA100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    return NV_SCAL_LITTER_NUM_FBPAS;
}

NvBool
kmemsysCheckReadoutEccEnablement_GA100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU32 fuse = GPU_REG_RD32(pGpu, NV_FUSE_FEATURE_READOUT);
    return FLD_TEST_DRF(_FUSE, _FEATURE_READOUT, _ECC_DRAM, _ENABLED, fuse);
}