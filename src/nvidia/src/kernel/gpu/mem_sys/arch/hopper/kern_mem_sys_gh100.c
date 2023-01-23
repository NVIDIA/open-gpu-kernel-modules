/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/bus/kern_bus.h"

#include "published/hopper/gh100/dev_fb.h"
#include "published/hopper/gh100/dev_vm.h"
#include "published/hopper/gh100/pri_nv_xal_ep.h"
#include "published/hopper/gh100/dev_nv_xal_addendum.h"

NV_STATUS
kmemsysDoCacheOp_GH100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32               reg,
    NvU32               regValue,
    NvU32               pollMask,
    PRMTIMEOUT          pTimeout
)
{
    NV_STATUS  rmStatus = NV_OK;
    NvU32      cnt = 0;
    NV_STATUS  timeoutStatus = NV_OK;
    NvU32      tokenRangeMask = 0;
    NvU32      startToken = 0;
    NvU32      completedToken = 0;
    NvBool     bMemopBusy = NV_TRUE;

    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
    {
        //
        // When the GPU is lost we cannot expect to successfully do cache
        // maintenance (see Bug 1557278).
        //
        return rmStatus;
    }

    switch (reg)
    {
        case NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY:
            tokenRangeMask = DRF_MASK(NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY_TOKEN);

            startToken = GPU_REG_RD32(pGpu, NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY);
            startToken = DRF_VAL( _XAL_EP_UFLUSH, _L2_FLUSH_DIRTY, _TOKEN, startToken);
            break;

        case NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS:
            tokenRangeMask = DRF_MASK(NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS_TOKEN);

            startToken = GPU_REG_RD32(pGpu, NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS);
            startToken = DRF_VAL( _XAL_EP_UFLUSH, _L2_CLEAN_COMPTAGS, _TOKEN, startToken);
            break;

        case NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_SYSMEM_INVALIDATE:
            tokenRangeMask = DRF_MASK(NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_SYSMEM_INVALIDATE_TOKEN);

            startToken = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_SYSMEM_INVALIDATE);
            startToken = DRF_VAL( _VIRTUAL_FUNCTION, _PRIV_FUNC_L2_SYSMEM_INVALIDATE, _TOKEN, startToken);
            break;

        case NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_PEERMEM_INVALIDATE:
            tokenRangeMask = DRF_MASK(NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_PEERMEM_INVALIDATE_TOKEN);

            startToken = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_PEERMEM_INVALIDATE);
            startToken = DRF_VAL( _VIRTUAL_FUNCTION, _PRIV_FUNC_L2_PEERMEM_INVALIDATE, _TOKEN, startToken);
            break;

        default:
            return NV_ERR_NOT_SUPPORTED;
    }

    while(1)
    {
        switch (reg)
        {
            case NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY:
                completedToken = GPU_REG_RD32(pGpu, NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY_COMPLETED);

                bMemopBusy = FLD_TEST_DRF(_XAL_EP_UFLUSH, _L2_FLUSH_DIRTY_COMPLETED, _STATUS, _BUSY, completedToken)
                             ? NV_TRUE : NV_FALSE;

                completedToken = DRF_VAL( _XAL_EP_UFLUSH, _L2_FLUSH_DIRTY_COMPLETED, _TOKEN, completedToken);
                break;

            case NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS:
                completedToken = GPU_REG_RD32(pGpu, NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS_COMPLETED);

                bMemopBusy = FLD_TEST_DRF(_XAL_EP_UFLUSH, _L2_CLEAN_COMPTAGS_COMPLETED, _STATUS, _BUSY, completedToken)
                             ? NV_TRUE : NV_FALSE;

                completedToken = DRF_VAL( _XAL_EP_UFLUSH, _L2_CLEAN_COMPTAGS_COMPLETED, _TOKEN, completedToken);
                break;

            case NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_SYSMEM_INVALIDATE:
                completedToken = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_SYSMEM_INVALIDATE_COMPLETED);

                bMemopBusy = FLD_TEST_DRF( _VIRTUAL_FUNCTION, _PRIV_FUNC_L2_SYSMEM_INVALIDATE_COMPLETED, _STATUS, _BUSY, completedToken)
                             ? NV_TRUE : NV_FALSE;

                completedToken = DRF_VAL( _VIRTUAL_FUNCTION, _PRIV_FUNC_L2_SYSMEM_INVALIDATE_COMPLETED, _TOKEN, completedToken);
                break;

            case NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_PEERMEM_INVALIDATE:
                completedToken = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_PEERMEM_INVALIDATE_COMPLETED);

                bMemopBusy = FLD_TEST_DRF( _VIRTUAL_FUNCTION, _PRIV_FUNC_L2_PEERMEM_INVALIDATE_COMPLETED, _STATUS, _BUSY, completedToken)
                             ? NV_TRUE : NV_FALSE;

                completedToken = DRF_VAL( _VIRTUAL_FUNCTION, _PRIV_FUNC_L2_PEERMEM_INVALIDATE_COMPLETED, _TOKEN, completedToken);
                break;
        }

        if (bMemopBusy == NV_FALSE)
        {
            break;
        }

        //
        // When completedToken > startToken(including the wrapping around case), due to the nature
        // of unsigned number, the value of "(startToken - completeToken) & tokenRangeMask" will be
        // at top of token range which will be bigger than NV_XAL_EP_MEMOP_MAX_OUTSTANDING. So it
        // will break out from the loop.
        //
        // The loop will wait only when completedToken in the range of
        // [startToken-NV_XAL_EP_MEMOP_MAX_OUTSTANDING, startToken].
        //
        if (((startToken - completedToken) & tokenRangeMask) > NV_XAL_EP_MEMOP_MAX_OUTSTANDING)
        {
            break;
        }

        if (timeoutStatus == NV_ERR_TIMEOUT)
        {
            //
            // This should not timeout, except for a HW bug.  Famous last words.
            // On !DEBUG we just keep trucking, it's the best we can do.
            //
            NV_PRINTF(LEVEL_ERROR,
                      "- timeout error waiting for reg 0x%x update cnt=%d\n",
                      reg, cnt);
            rmStatus = NV_ERR_TIMEOUT;
            DBG_BREAKPOINT();
            break;
        }
        else if ( API_GPU_IN_RESET_SANITY_CHECK(pGpu) ||
                 !API_GPU_ATTACHED_SANITY_CHECK(pGpu))
        {
            //
            // The GPU is in full chip reset, or has fallen off the bus
            // Just return
            //
            return NV_OK;
        }

        timeoutStatus = gpuCheckTimeout(pGpu, pTimeout);
        osSpinLoop();
        cnt++;
    }

#ifdef DEBUG
    if (cnt > 1)
    {
        NvU32 intr0 = 0;
        intr0 = GPU_REG_RD32(pGpu, NV_XAL_EP_INTR_0);
        NV_ASSERT(DRF_VAL(_XAL_EP, _INTR_0, _FB_ACK_TIMEOUT, intr0) != NV_XAL_EP_INTR_0_FB_ACK_TIMEOUT_PENDING);
    }
#endif // DEBUG

    return rmStatus;
}

/**
 * @brief L2 Cache management OPs
 *
 * GK20A/T124 have a writeback L2 cache, so the cache ops are slightly
 * different than those of Fermi/Kepler.  Specifically, we can write back dirty
 * lines to system memory.
 *
 * @param pMemDesc
 * @param targetMem
 * @param cacheOp
 */
NV_STATUS
kmemsysCacheOp_GH100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    PMEMORY_DESCRIPTOR  pMemDesc,
    FB_CACHE_MEMTYPE    targetMem,
    FB_CACHE_OP         cacheOp
)
{
    NV_STATUS status  = NV_OK;
    RMTIMEOUT timeout = {0, };
    NvU32     reg = 0;

    if ((targetMem == FB_CACHE_MEM_UNDEFINED) && pMemDesc)
    {
        targetMem = (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM) ?
                        FB_CACHE_VIDEO_MEMORY : FB_CACHE_SYSTEM_MEMORY;
    }

    if ((cacheOp == FB_CACHE_OP_UNDEFINED) || (targetMem == FB_CACHE_MEM_UNDEFINED))
    {
        NV_PRINTF(LEVEL_ERROR, "called with null %s\n",
                  cacheOp ? "cache operation" : "memory target");
        DBG_BREAKPOINT();
        return status;  // return NV_OK
    }

    // For GK20A, an explicit sysmembar flush is needed before L2 cache flush operation.
    // Refer GK20A LTC IAS (section 5.5)
    kbusSendSysmembarSingle_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu));

    // Wait for the flush to flow through
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

    switch (targetMem)
    {
        case FB_CACHE_SYSTEM_MEMORY:
            if (cacheOp == FB_CACHE_INVALIDATE)
            {
                NV_PRINTF(LEVEL_INFO,
                          "Invalidate not supported, promoting to an evict (writeback + "
                          "invalidate clean lines).\n");
                cacheOp = FB_CACHE_EVICT;
            }

            if (cacheOp == FB_CACHE_WRITEBACK || cacheOp == FB_CACHE_EVICT)
            {
                reg =  NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY;
                status = kmemsysDoCacheOp_HAL(pGpu, pKernelMemorySystem, reg, 0, 0, &timeout);
            }

            if (cacheOp == FB_CACHE_EVICT)
            {
                //
                // Ideally we should use NV_UFLUSH_L2_INVALIDATE_CLEAN_LINES.
                // But NV_UFLUSH_L2_INVALIDATE_CLEAN_LINES is not defined on desktop GPUs.
                // NV_UFLUSH_L2_SYSMEM_INVALIDATE is same as NV_UFLUSH_L2_INVALIDATE_CLEAN_LINES, and is defined in all chips.
                // For Hopper, it is changed to NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_SYSMEM_INVALIDATE
                //
                reg =  NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_SYSMEM_INVALIDATE;
                status = kmemsysDoCacheOp_HAL(pGpu, pKernelMemorySystem, reg, 0, 0, &timeout);
            }
            break;

        case FB_CACHE_VIDEO_MEMORY:
            if (cacheOp == FB_CACHE_EVICT)
            {
                NvU32 flags = NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_ALL |
                              NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_CLEAN;
                status = kmemsysSendL2InvalidateEvict(pGpu, pKernelMemorySystem, flags);
            }
            else if (cacheOp == FB_CACHE_INVALIDATE)
            {
                NvU32 flags = NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_ALL;
                status = kmemsysSendL2InvalidateEvict(pGpu, pKernelMemorySystem, flags);
            }
            else
            {
                status = NV_ERR_NOT_SUPPORTED;
            }
            break;

        case FB_CACHE_PEER_MEMORY:
            //
            // for GF100 - for sysmem cache only invalidate operation is supported
            // evict = writeback+invalidate is reduced to invalidate for GF100
            //
            if((cacheOp != FB_CACHE_INVALIDATE) && (cacheOp != FB_CACHE_EVICT))
            {
                return NV_OK;
            }

            reg =  NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_PEERMEM_INVALIDATE;
            status = kmemsysDoCacheOp_HAL(pGpu, pKernelMemorySystem, reg, 0, 0, &timeout);
            break;

        case FB_CACHE_COMPTAG_MEMORY:
            if(cacheOp != FB_CACHE_WRITEBACK && cacheOp != FB_CACHE_EVICT)
            {
                return NV_OK;
            }

            //
            // Beware of HW bug 545242. Graphics should be idle and flushed here
            // or comp tag cache could be corrupted.  When mods uses this call
            // during verif, this should already be the case.
            //
            reg =  NV_XAL_EP_UFLUSH_L2_CLEAN_COMPTAGS;
            status = kmemsysDoCacheOp_HAL(pGpu, pKernelMemorySystem, reg, 0, 0, &timeout);
            break;

        case FB_CACHE_DIRTY:
            if(cacheOp != FB_CACHE_WRITEBACK && cacheOp != FB_CACHE_EVICT)
            {
                return NV_OK;
            }

            reg =  NV_XAL_EP_UFLUSH_L2_FLUSH_DIRTY;
            status = kmemsysDoCacheOp_HAL(pGpu, pKernelMemorySystem, reg, 0, 0, &timeout);
            break;

        case FB_CACHE_DIRTY_ALL:
            if(cacheOp != FB_CACHE_EVICT)
            {
                return NV_OK;
            }

            status = kmemsysSendFlushL2AllRamsAndCaches(pGpu, pKernelMemorySystem);
            break;

        default:
            // return OK for other memory targets
            status = NV_OK;
            break;
    }

    return status;
}

/*!
 * @brief Write the sysmemFlushBuffer val into the NV_PFB_FBHUB_PCIE_FLUSH_SYSMEM_ADDR register
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in[ pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns void
 */
void
kmemsysProgramSysmemFlushBuffer_GH100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU32                  alignedSysmemFlushBufferAddr   = 0x0;
    NvU32                  alignedSysmemFlushBufferAddrHi = 0x0;

    NV_ASSERT(pKernelMemorySystem->sysmemFlushBuffer != 0);

    // alignedSysmemFlushBufferAddr will have the lower 32 bits of the buffer address
    alignedSysmemFlushBufferAddr = NvU64_LO32(pKernelMemorySystem->sysmemFlushBuffer);

    // alignedSysmemFlushBufferAddrHi will have the upper 32 bits of the buffer address
    alignedSysmemFlushBufferAddrHi = NvU64_HI32(pKernelMemorySystem->sysmemFlushBuffer);

    // Assert when Sysmem Flush buffer has more than 52-bit address
    NV_ASSERT((alignedSysmemFlushBufferAddrHi & (~NV_PFB_FBHUB_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR_MASK)) == 0);

    alignedSysmemFlushBufferAddrHi &= NV_PFB_FBHUB_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR_MASK;

    GPU_FLD_WR_DRF_NUM(pGpu, _PFB, _FBHUB_PCIE_FLUSH_SYSMEM_ADDR_HI, _ADR, alignedSysmemFlushBufferAddrHi);
    GPU_FLD_WR_DRF_NUM(pGpu, _PFB, _FBHUB_PCIE_FLUSH_SYSMEM_ADDR_LO, _ADR, alignedSysmemFlushBufferAddr);
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
kmemsysAssertSysmemFlushBufferValid_GH100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NV_ASSERT((GPU_REG_RD_DRF(pGpu, _PFB, _FBHUB_PCIE_FLUSH_SYSMEM_ADDR_LO, _ADR) != 0) ||
              (GPU_REG_RD_DRF(pGpu, _PFB, _FBHUB_PCIE_FLUSH_SYSMEM_ADDR_HI, _ADR) != 0));
}

/*
 * @brief   Function to map swizzId to VMMU Segments
 */
NV_STATUS
kmemsysSwizzIdToVmmuSegmentsRange_GH100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32 swizzId,
    NvU32 vmmuSegmentSize,
    NvU32 totalVmmuSegments
)
{
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    const KERNEL_MIG_MANAGER_STATIC_INFO *pStaticInfo = kmigmgrGetStaticInfo(pGpu, pKernelMIGManager);
    NvU64 startingVmmuSegment;
    NvU64 memSizeInVmmuSegment;

    NV_ASSERT_OR_RETURN(swizzId < KMIGMGR_MAX_GPU_SWIZZID, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pStaticInfo->pSwizzIdFbMemPageRanges != NULL, NV_ERR_INVALID_STATE);

    startingVmmuSegment = pStaticInfo->pSwizzIdFbMemPageRanges->fbMemPageRanges[swizzId].lo;
    memSizeInVmmuSegment = pStaticInfo->pSwizzIdFbMemPageRanges->fbMemPageRanges[swizzId].hi -
                           pStaticInfo->pSwizzIdFbMemPageRanges->fbMemPageRanges[swizzId].lo;
    NV_ASSERT_OR_RETURN((memSizeInVmmuSegment <= totalVmmuSegments), NV_ERR_INVALID_STATE);

    NV_ASSERT_OK_OR_RETURN(
        kmemsysInitMIGGPUInstanceMemConfigForSwizzId(pGpu, pKernelMemorySystem, swizzId, startingVmmuSegment, memSizeInVmmuSegment));

    return NV_OK;
}
