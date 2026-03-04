/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/bus/kern_bus.h"

#include "published/maxwell/gm200/dev_flush.h"

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
kmemsysCacheOp_GM200
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    PMEMORY_DESCRIPTOR  pMemDesc,
    FB_CACHE_MEMTYPE    targetMem,
    FB_CACHE_OP         cacheOp
)
{
    NV_STATUS status  = NV_OK;
    RMTIMEOUT timeout;
    NvU32     reg;
    NvU32     regValue;
    NvU32     pollMask;

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
    kbusSendSysmembar(pGpu, GPU_GET_KERNEL_BUS(pGpu));

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
                reg =  NV_UFLUSH_L2_FLUSH_DIRTY;
                regValue = FLD_SET_DRF(_UFLUSH, _L2_FLUSH_DIRTY, _PENDING, _BUSY, 0);
                pollMask = FLD_SET_DRF(_UFLUSH, _L2_FLUSH_DIRTY, _PENDING, _BUSY, 0);
                pollMask = FLD_SET_DRF(_UFLUSH, _L2_FLUSH_DIRTY, _OUTSTANDING, _TRUE, pollMask);
                status = kmemsysDoCacheOp_HAL(pGpu, pKernelMemorySystem, reg, regValue, pollMask, &timeout);
            }

            if (cacheOp == FB_CACHE_EVICT)
            {
                //
                // Ideally we should use NV_UFLUSH_L2_INVALIDATE_CLEAN_LINES.
                // But NV_UFLUSH_L2_INVALIDATE_CLEAN_LINES is not defined on desktop GPUs.
                // NV_UFLUSH_L2_SYSMEM_INVALIDATE is same as NV_UFLUSH_L2_INVALIDATE_CLEAN_LINES, and is defined in all chips.
                //
                reg =  NV_UFLUSH_L2_SYSMEM_INVALIDATE;
                regValue = FLD_SET_DRF(_UFLUSH, _L2_SYSMEM_INVALIDATE, _PENDING, _BUSY, 0);
                pollMask = FLD_SET_DRF(_UFLUSH, _L2_SYSMEM_INVALIDATE, _PENDING, _BUSY, 0);
                pollMask = FLD_SET_DRF(_UFLUSH, _L2_SYSMEM_INVALIDATE, _OUTSTANDING, _TRUE, pollMask);
                status = kmemsysDoCacheOp_HAL(pGpu, pKernelMemorySystem, reg, regValue, pollMask, &timeout);
            }
            break;
        case FB_CACHE_VIDEO_MEMORY:
            // Fbmem cache ops are not supported from VF -- force NV_OK for MODS
            if (IS_VIRTUAL(pGpu))
                return NV_OK;

            if (cacheOp == FB_CACHE_EVICT)
            {
                status = kmemsysSendL2InvalidateEvict(pGpu, pKernelMemorySystem,
                                                      NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_ALL |
                                                      NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_CLEAN);
            }
            else if (cacheOp == FB_CACHE_INVALIDATE)
            {
                status = kmemsysSendL2InvalidateEvict(pGpu, pKernelMemorySystem,
                                                      NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_ALL);
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
                return NV_OK;

            reg =  NV_UFLUSH_L2_PEERMEM_INVALIDATE;
            regValue = FLD_SET_DRF(_UFLUSH, _L2_PEERMEM_INVALIDATE, _PENDING, _BUSY, 0);
            pollMask = FLD_SET_DRF(_UFLUSH, _L2_PEERMEM_INVALIDATE, _PENDING, _BUSY, 0);
            pollMask = FLD_SET_DRF(_UFLUSH, _L2_PEERMEM_INVALIDATE, _OUTSTANDING, _TRUE, pollMask);
            status = kmemsysDoCacheOp_HAL(pGpu, pKernelMemorySystem, reg, regValue, pollMask, &timeout);
            break;
        case FB_CACHE_COMPTAG_MEMORY:
            if(cacheOp != FB_CACHE_WRITEBACK && cacheOp != FB_CACHE_EVICT)
                return NV_OK;

            //
            // Beware of HW bug 545242. Graphics should be idle and flushed here
            // or comp tag cache could be corrupted.  When mods uses this call
            // during verif, this should already be the case.
            //
            reg =  NV_UFLUSH_L2_CLEAN_COMPTAGS;
            regValue = FLD_SET_DRF(_UFLUSH, _L2_CLEAN_COMPTAGS, _PENDING, _BUSY, 0);
            pollMask = FLD_SET_DRF(_UFLUSH, _L2_CLEAN_COMPTAGS, _PENDING, _BUSY, 0);
            pollMask = FLD_SET_DRF(_UFLUSH, _L2_CLEAN_COMPTAGS, _OUTSTANDING, _TRUE, pollMask);
            status = kmemsysDoCacheOp_HAL(pGpu, pKernelMemorySystem, reg, regValue, pollMask, &timeout);
            break;
        case FB_CACHE_DIRTY:
            if(cacheOp != FB_CACHE_WRITEBACK && cacheOp != FB_CACHE_EVICT)
                return NV_OK;

            reg =  NV_UFLUSH_L2_FLUSH_DIRTY;
            regValue = FLD_SET_DRF(_UFLUSH, _L2_FLUSH_DIRTY, _PENDING, _BUSY, 0);
            pollMask = FLD_SET_DRF(_UFLUSH, _L2_FLUSH_DIRTY, _PENDING, _BUSY, 0);
            pollMask = FLD_SET_DRF(_UFLUSH, _L2_FLUSH_DIRTY, _OUTSTANDING, _TRUE, pollMask);
            status = kmemsysDoCacheOp_HAL(pGpu, pKernelMemorySystem, reg, regValue, pollMask, &timeout);
            break;
        case FB_CACHE_DIRTY_ALL:
            if(cacheOp != FB_CACHE_EVICT)
                return NV_OK;

            status = kmemsysSendFlushL2AllRamsAndCaches(pGpu, pKernelMemorySystem);
            break;
        default:
            // return OK for other memory targets
            status = NV_OK;
    }

    return status;
}

