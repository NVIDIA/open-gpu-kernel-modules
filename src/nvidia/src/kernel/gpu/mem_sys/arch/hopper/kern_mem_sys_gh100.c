/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nvtypes.h"
#include "os/os.h"
#include "kernel/gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/bus/kern_bus.h"
#include "kernel/gpu/intr/intr.h"
#include "nverror.h"

#include "published/hopper/gh100/dev_fb.h"
#include "published/hopper/gh100/dev_ltc.h"
#include "published/hopper/gh100/dev_fbpa.h"
#include "published/hopper/gh100/dev_vm.h"
#include "published/hopper/gh100/pri_nv_xal_ep.h"
#include "published/hopper/gh100/dev_nv_xal_addendum.h"
#include "published/hopper/gh100/dev_nv_xpl.h"
#include "published/hopper/gh100/dev_xtl_ep_pri.h"
#include "published/hopper/gh100/hwproject.h"
#include "published/ampere/ga100/dev_fb.h"

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
        NV_ASSERT(kmemsysAssertFbAckTimeoutPending_HAL(pGpu, pKernelMemorySystem) == NV_FALSE);
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

/*!
 * @brief Add GPU memory as a NUMA node.
 *
 * Add GPU memory as a NUMA node to the OS kernel in platforms where
 * GPU is coherently connected to the CPU.
 *
 * @param[in]  pGPU                OBJGPU pointer
 * @param[in]  pKernelMemorySystem KernelMemorySystem pointer
 * @param[in]  swizzId             swizzId of the MIG GPU instance, 0 for full GPU instance/non-MIG.
 * @param[in]  offset              start offset of the GPU instance within FB
 * @param[in]  size                size of the GPU instance
 * @param[out] numaNodeId          NUMA node id corresponding to the added @swizzId partition memory
 *                                 when NV_OK is returned.
 *
 * @returns NV_OK if all is okay.  Otherwise an error-specific value.
 *
 */
NV_STATUS
kmemsysNumaAddMemory_GH100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32               swizzId,
    NvU64               offset,
    NvU64               size,
    NvS32              *numaNodeId
)
{
    NV_STATUS status;
    NvU64     memblockSize   = 0;
    NvU32     lNumaNodeId;

    NV_ASSERT_OK_OR_RETURN(osNumaMemblockSize(&memblockSize));
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED(size, memblockSize), NV_ERR_INVALID_STATE);

    if (pKernelMemorySystem->memPartitionNumaInfo[swizzId].bInUse)
    {
        NV_PRINTF(LEVEL_ERROR, "Memory partition: %u is already in use!\n", swizzId);
        return NV_ERR_IN_USE;
    }

    status = osNumaAddGpuMemory(pGpu->pOsGpuInfo, offset, size, &lNumaNodeId);
    if (status == NV_OK)
    {
        pKernelMemorySystem->memPartitionNumaInfo[swizzId].bInUse = NV_TRUE;
        pKernelMemorySystem->memPartitionNumaInfo[swizzId].offset = offset;
        pKernelMemorySystem->memPartitionNumaInfo[swizzId].size = size;
        pKernelMemorySystem->memPartitionNumaInfo[swizzId].numaNodeId = lNumaNodeId;
        *numaNodeId = lNumaNodeId;

        pKernelMemorySystem->bNumaNodesAdded = NV_TRUE;

        NV_PRINTF(LEVEL_INFO, "Memory partition: %u added successfully!"
                  " numa id: %u offset: 0x%llx size: 0x%llx\n",
                  swizzId, lNumaNodeId, offset, size);
    }

    return status;
}

/*!
 * @brief Remove a particular MIG GPU instance GPU memory from OS kernel.
 *
 * Remove GPU memory from the OS kernel that is earlier added as a NUMA node
 * to the kernel in platforms where GPU is coherently connected to the CPU.
 *
 * @param[in]  pGPU                OBJGPU pointer
 * @param[in]  pKernelMemorySystem KernelMemorySystem pointer
 * @param[in]  swizzId             swizzId of the MIG GPU instance, 0 for full partition/non-MIG.
 */
void
kmemsysNumaRemoveMemory_GH100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32               swizzId
)
{
    if (pKernelMemorySystem->memPartitionNumaInfo[swizzId].bInUse == NV_FALSE)
    {
        return;
    }

    osNumaRemoveGpuMemory(pGpu->pOsGpuInfo,
                          pKernelMemorySystem->memPartitionNumaInfo[swizzId].offset,
                          pKernelMemorySystem->memPartitionNumaInfo[swizzId].size,
                          pKernelMemorySystem->memPartitionNumaInfo[swizzId].numaNodeId);
    pKernelMemorySystem->memPartitionNumaInfo[swizzId].bInUse = NV_FALSE;
    pKernelMemorySystem->memPartitionNumaInfo[swizzId].offset = 0;
    pKernelMemorySystem->memPartitionNumaInfo[swizzId].size = 0;
    pKernelMemorySystem->memPartitionNumaInfo[swizzId].numaNodeId = NV_U32_MAX;

    NV_PRINTF(LEVEL_INFO, "NVRM: memory partition: %u removed successfully!\n",
              swizzId);
    return;
}

/*!
 * @brief Remove all GPU memory from OS kernel.
 *
 * Remove all MIG GPU instances GPU memory from the OS kernel that is earlier added
 * as a NUMA node  to the kernel in platforms where GPU is coherently
 * connected to the CPU.
 *
 * @param[in]  pGPU                OBJGPU pointer
 * @param[in]  pKernelMemorySystem KernelMemorySystem pointer
 *
 */
void
kmemsysNumaRemoveAllMemory_GH100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU32 swizzId;

    for (swizzId = 0; swizzId < KMIGMGR_MAX_GPU_SWIZZID; swizzId++)
    {
        kmemsysNumaRemoveMemory_HAL(pGpu, pKernelMemorySystem, swizzId);
    }

    return;
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
    memSizeInVmmuSegment = (pStaticInfo->pSwizzIdFbMemPageRanges->fbMemPageRanges[swizzId].hi -
                            pStaticInfo->pSwizzIdFbMemPageRanges->fbMemPageRanges[swizzId].lo + 1);

    if (memSizeInVmmuSegment > totalVmmuSegments)
    {
        //
        // SwizzID-0 should cover only partitionable range, however for AMAP,
        // there is no difference between swizzID-0 and no MIG which can result in
        // AMAP returning an additional vmmuSegment for swizzID-0
        //
        NV_ASSERT_OR_RETURN((swizzId == 0), NV_ERR_INVALID_STATE);
    }

    NV_ASSERT_OK_OR_RETURN(
        kmemsysInitMIGGPUInstanceMemConfigForSwizzId(pGpu, pKernelMemorySystem, swizzId, startingVmmuSegment, memSizeInVmmuSegment));

    return NV_OK;
}

/*
 * @brief   Function to check if an FB ACK timeout occured. Used only for Debug.
 */
NvBool
kmemsysAssertFbAckTimeoutPending_GH100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
#ifdef DEBUG
    NvU32 intr0 = 0;
    intr0 = GPU_REG_RD32(pGpu, NV_XAL_EP_INTR_0);
    return DRF_VAL(_XAL_EP, _INTR_0, _FB_ACK_TIMEOUT, intr0) == NV_XAL_EP_INTR_0_FB_ACK_TIMEOUT_PENDING;
#else
    return NV_FALSE;
#endif
}


/*!
 * Utility function used to read registers and ignore PRI errors
 */
static NvU32
_kmemsysReadRegAndMaskPriError
(
    OBJGPU *pGpu,
    NvU32 regAddr
)
{
    NvU32 regVal;

    regVal = osGpuReadReg032(pGpu, regAddr);
    if ((regVal & GPU_READ_PRI_ERROR_MASK) == GPU_READ_PRI_ERROR_CODE)
    {
        return 0;
    }

    return regVal;
}
/*
 * @brief Function that checks if ECC error occurred by reading various count
 * registers/interrupt registers. This function is not floorsweeping-aware so
 * PRI errors are ignored
 */
void
kmemsysCheckEccCounts_GH100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU32 dramCount = 0;
    NvU32 mmuCount = 0;
    NvU32 ltcCount = 0;
    NvU32 pcieCount = 0;
    NvU32 regVal;
    for (NvU32 i = 0; i < NV_SCAL_LITTER_NUM_FBPAS; i++)
    {
        for (NvU32 j = 0; j < NV_PFB_FBPA_0_ECC_DED_COUNT__SIZE_1; j++)
        {
            // DRAM count read
            dramCount += _kmemsysReadRegAndMaskPriError(pGpu, NV_PFB_FBPA_0_ECC_DED_COUNT(j) + (i * NV_FBPA_PRI_STRIDE));

            // LTC count read
            regVal = _kmemsysReadRegAndMaskPriError(pGpu, NV_PLTCG_LTC0_LTS0_L2_CACHE_ECC_UNCORRECTED_ERR_COUNT +
                    (i * NV_LTC_PRI_STRIDE) + (j * NV_LTS_PRI_STRIDE));
            ltcCount += DRF_VAL(_PLTCG_LTC0_LTS0, _L2_CACHE_ECC, _UNCORRECTED_ERR_COUNT_UNIQUE, regVal);
        }
    }

    // L2TLB
    regVal = _kmemsysReadRegAndMaskPriError(pGpu, NV_PFB_PRI_MMU_L2TLB_ECC_UNCORRECTED_ERR_COUNT);
    mmuCount += DRF_VAL(_PFB_PRI_MMU, _L2TLB_ECC, _UNCORRECTED_ERR_COUNT_UNIQUE, regVal);

    // HUBTLB
    regVal = _kmemsysReadRegAndMaskPriError(pGpu, NV_PFB_PRI_MMU_HUBTLB_ECC_UNCORRECTED_ERR_COUNT);
    mmuCount += DRF_VAL(_PFB_PRI_MMU, _HUBTLB_ECC, _UNCORRECTED_ERR_COUNT_UNIQUE, regVal);

    // FILLUNIT
    regVal = _kmemsysReadRegAndMaskPriError(pGpu, NV_PFB_PRI_MMU_FILLUNIT_ECC_UNCORRECTED_ERR_COUNT);
    mmuCount += DRF_VAL(_PFB_PRI_MMU, _FILLUNIT_ECC, _UNCORRECTED_ERR_COUNT_UNIQUE, regVal);

    // PCIE RBUF
    regVal = _kmemsysReadRegAndMaskPriError(pGpu, NV_XPL_BASE_ADDRESS + NV_XPL_DL_ERR_COUNT_RBUF);
    pcieCount += DRF_VAL(_XPL_DL, _ERR_COUNT_RBUF, _UNCORR_ERR, regVal);

    // PCIE SEQ_LUT
    regVal = _kmemsysReadRegAndMaskPriError(pGpu, NV_XPL_BASE_ADDRESS + NV_XPL_DL_ERR_COUNT_SEQ_LUT);
    pcieCount += DRF_VAL(_XPL_DL, _ERR_COUNT_SEQ_LUT, _UNCORR_ERR, regVal);

    // PCIE RE ORDER
    regVal = _kmemsysReadRegAndMaskPriError(pGpu, NV_XAL_EP_REORDER_ECC_UNCORRECTED_ERR_COUNT);
    pcieCount += DRF_VAL(_XAL_EP, _REORDER_ECC, _UNCORRECTED_ERR_COUNT_UNIQUE, regVal);

    // PCIE P2PREQ
    regVal = _kmemsysReadRegAndMaskPriError(pGpu, NV_XAL_EP_P2PREQ_ECC_UNCORRECTED_ERR_COUNT);
    pcieCount += DRF_VAL(_XAL_EP, _P2PREQ_ECC, _UNCORRECTED_ERR_COUNT_UNIQUE, regVal);

    // PCIE XTL
    regVal = _kmemsysReadRegAndMaskPriError(pGpu, NV_XTL_BASE_ADDRESS + NV_XTL_EP_PRI_DED_ERROR_STATUS);
    if (regVal != 0)
    {
        pcieCount += 1;
    }

    // PCIE XTL
    regVal = _kmemsysReadRegAndMaskPriError(pGpu, NV_XTL_BASE_ADDRESS + NV_XTL_EP_PRI_RAM_ERROR_INTR_STATUS);
    if (regVal != 0)
    {
        pcieCount += 1;
    }

    // If counts > 0 or if poison interrupt pending, ECC error has occurred.
    if (((dramCount + ltcCount + mmuCount + pcieCount) != 0) ||
        intrIsVectorPending_HAL(pGpu, GPU_GET_INTR(pGpu), NV_PFB_FBHUB_POISON_INTR_VECTOR_HW_INIT, NULL))
    {
        nvErrorLog_va((void *)pGpu, UNRECOVERABLE_ECC_ERROR_ESCAPE,
                      "An uncorrectable ECC error detected "
                      "(possible firmware handling failure) "
                      "DRAM:%d, LTC:%d, MMU:%d, PCIE:%d", dramCount, ltcCount, mmuCount, pcieCount);
    }
}

/*
 * @brief  Function that clears ECC error count registers.
 */
NV_STATUS
kmemsysClearEccCounts_GH100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU32 regVal = 0;
    RMTIMEOUT timeout;
    NV_STATUS status = NV_OK;

    gpuClearFbhubPoisonIntrForBug2924523_HAL(pGpu);

    for (NvU32 i = 0; i < NV_SCAL_LITTER_NUM_FBPAS; i++)
    {
        for (NvU32 j = 0; j < NV_PFB_FBPA_0_ECC_DED_COUNT__SIZE_1; j++)
        {
            osGpuWriteReg032(pGpu, NV_PFB_FBPA_0_ECC_DED_COUNT(j) + (i * NV_FBPA_PRI_STRIDE), 0);
            osGpuWriteReg032(pGpu, NV_PLTCG_LTC0_LTS0_L2_CACHE_ECC_UNCORRECTED_ERR_COUNT + (i * NV_LTC_PRI_STRIDE) + (j * NV_LTS_PRI_STRIDE), 0);
        }
    }

    // Reset MMU counts
    osGpuWriteReg032(pGpu, NV_PFB_PRI_MMU_L2TLB_ECC_UNCORRECTED_ERR_COUNT, 0);
    osGpuWriteReg032(pGpu, NV_PFB_PRI_MMU_HUBTLB_ECC_UNCORRECTED_ERR_COUNT, 0);
    osGpuWriteReg032(pGpu, NV_PFB_PRI_MMU_FILLUNIT_ECC_UNCORRECTED_ERR_COUNT, 0);

    // Reset XAL-EP counts
    osGpuWriteReg032(pGpu, NV_XAL_EP_REORDER_ECC_UNCORRECTED_ERR_COUNT, 0);
    osGpuWriteReg032(pGpu, NV_XAL_EP_P2PREQ_ECC_UNCORRECTED_ERR_COUNT, 0);

    // Reset XTL-EP status registers
    osGpuWriteReg032(pGpu, NV_XTL_BASE_ADDRESS + NV_XTL_EP_PRI_DED_ERROR_STATUS, ~0);
    osGpuWriteReg032(pGpu, NV_XTL_BASE_ADDRESS + NV_XTL_EP_PRI_RAM_ERROR_INTR_STATUS, ~0);

    // Reset XPL-EP error counters
    regVal = DRF_DEF(_XPL, _DL_ERR_RESET, _RBUF_UNCORR_ERR_COUNT, _PENDING) |
             DRF_DEF(_XPL, _DL_ERR_RESET, _SEQ_LUT_UNCORR_ERR_COUNT, _PENDING);
    osGpuWriteReg032(pGpu, NV_XPL_BASE_ADDRESS + NV_XPL_DL_ERR_RESET, regVal);

    // Wait for the error counter reset to complete
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    for (;;)
    {
        status = gpuCheckTimeout(pGpu, &timeout);

        regVal = osGpuReadReg032(pGpu, NV_XPL_BASE_ADDRESS + NV_XPL_DL_ERR_RESET);

        if (FLD_TEST_DRF(_XPL, _DL_ERR_RESET, _RBUF_UNCORR_ERR_COUNT, _DONE, regVal) &&
            FLD_TEST_DRF(_XPL, _DL_ERR_RESET, _SEQ_LUT_UNCORR_ERR_COUNT, _DONE, regVal))
            break;

        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}
