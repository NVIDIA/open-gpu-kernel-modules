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
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "ctrl/ctrl2080/ctrl2080fb.h"

#include "published/maxwell/gm107/dev_fb.h"
#include "published/maxwell/gm107/dev_bus.h"
#include "published/maxwell/gm107/dev_flush.h"
#include "published/maxwell/gm107/dev_fifo.h"

// Based on busFlushSingle_GM107

NV_STATUS
kmemsysDoCacheOp_GM107
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32               reg,
    NvU32               regValue,
    NvU32               pollMask,
    PRMTIMEOUT          pTimeout
)
{
    NV_STATUS        rmStatus = NV_OK;
    NvU32            cnt = 0;
    NV_STATUS        timeoutStatus = NV_OK;
    NvU32            regValueRead = 0;

    if (IS_VIRTUAL(pGpu))
    {
        switch (reg)
        {
            case NV_UFLUSH_L2_PEERMEM_INVALIDATE:
            case NV_UFLUSH_L2_SYSMEM_INVALIDATE:
                break;
            case NV_UFLUSH_L2_FLUSH_DIRTY:
                return NV_OK;
            default:
                return NV_ERR_NOT_SUPPORTED;
        }
    }

    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
    {
        //
        // When the GPU is lost we cannot expect to successfully do cache
        // maintenance (see Bug 1557278).
        //
        return rmStatus;
    }

    // We don't want this breakpoint when a debug build is being used by special test
    // equipment (e.g. ATE) that expects to hit this situation.  Bug 672073
#ifdef DEBUG
    if (!(API_GPU_IN_RESET_SANITY_CHECK(pGpu)) && !IS_VIRTUAL(pGpu))
    {
        NV_ASSERT(GPU_REG_RD32(pGpu, NV_UFLUSH_FB_FLUSH) == 0);
        NV_ASSERT(kmemsysReadL2SysmemInvalidateReg_HAL(pGpu, pKernelMemorySystem) == 0);
        NV_ASSERT(kmemsysReadL2PeermemInvalidateReg_HAL(pGpu, pKernelMemorySystem) == 0);
        NV_ASSERT(GPU_REG_RD32(pGpu, NV_UFLUSH_L2_CLEAN_COMPTAGS) == 0);
        NV_ASSERT(GPU_REG_RD32(pGpu, NV_UFLUSH_L2_FLUSH_DIRTY) == 0);
    }
#endif // DEBUG

    switch (reg)
    {
        case NV_UFLUSH_L2_PEERMEM_INVALIDATE:
            kmemsysWriteL2PeermemInvalidateReg_HAL(pGpu, pKernelMemorySystem, regValue);
            break;
        case NV_UFLUSH_L2_SYSMEM_INVALIDATE:
            kmemsysWriteL2SysmemInvalidateReg_HAL(pGpu, pKernelMemorySystem, regValue);
            break;
        default:
            GPU_REG_WR32(pGpu, reg, regValue);
    }

    while(1)
    {
        switch (reg)
        {
            case NV_UFLUSH_L2_PEERMEM_INVALIDATE:
                regValueRead = kmemsysReadL2PeermemInvalidateReg_HAL(pGpu, pKernelMemorySystem);
                break;
            case NV_UFLUSH_L2_SYSMEM_INVALIDATE:
                regValueRead = kmemsysReadL2SysmemInvalidateReg_HAL(pGpu, pKernelMemorySystem);
                break;
            default:
                regValueRead = GPU_REG_RD32(pGpu, reg);
        }

        if (regValueRead & pollMask)
        {
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
        else
            break;
    }

#ifdef DEBUG
    if (cnt > 1 && !IS_VIRTUAL(pGpu))
    {
        NvU32 intr0 = 0;
        intr0 = GPU_REG_RD32(pGpu, NV_PBUS_INTR_0);
        NV_ASSERT(DRF_VAL(_PBUS, _INTR_0, _FB_ACK_TIMEOUT, intr0) != NV_PBUS_INTR_0_FB_ACK_TIMEOUT_PENDING);
    }
#endif // DEBUG

    return rmStatus;
}

void
kmemsysWriteL2SysmemInvalidateReg_GM107
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32               value
)
{
    GPU_REG_WR32(pGpu, NV_UFLUSH_L2_SYSMEM_INVALIDATE, value);
}

NvU32
kmemsysReadL2SysmemInvalidateReg_GM107
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    return GPU_REG_RD32(pGpu, NV_UFLUSH_L2_SYSMEM_INVALIDATE);
}

void
kmemsysWriteL2PeermemInvalidateReg_GM107
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32               value
)
{
    GPU_REG_WR32(pGpu, NV_UFLUSH_L2_PEERMEM_INVALIDATE, value);
}

NvU32
kmemsysReadL2PeermemInvalidateReg_GM107
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    return GPU_REG_RD32(pGpu, NV_UFLUSH_L2_PEERMEM_INVALIDATE);
}

/*!
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
kmemsysInitFlushSysmemBuffer_GM107
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NV_STATUS              status;
    NvBool                bTryAgain = NV_FALSE;

    //
    // In case of suspend/resume, the buffer might be already allocated, but
    // the HW still needs to be programmed below.
    //
    if (pKernelMemorySystem->pSysmemFlushBufferMemDesc == NULL)
    {
        const NvU32 flushBufferDmaAddressSize = 40;
        RmPhysAddr dmaWindowStartAddr = gpuGetDmaStartAddress(pGpu);
        RmPhysAddr dmaWindowEndAddr = dmaWindowStartAddr +
            (1ULL << flushBufferDmaAddressSize) - 1;

        //
        // Sysmem flush buffer
        // The sysmembar flush does a zero byte read of sysmem if there was a
        // sysmem write since the last flush. The actual memory does have
        // to be valid and allocated at all times because an actual read may
        // be issued (observed on e.g. GF108).
        //

        //
        // First, try to allocate a 32-bit addressable DMA memory without
        // lowering the DMA address size.
        // This is currently implemented for Linux where the MEMDESC_FLAGS_ALLOC_32BIT_ADDRESSABLE
        // will allocate 32-bit memory by using GFP_DMA32 flag.
        //
        status = memdescCreate(&pKernelMemorySystem->pSysmemFlushBufferMemDesc,
                               pGpu, RM_PAGE_SIZE,
                               (1 << kmemsysGetFlushSysmemBufferAddrShift_HAL(pGpu, pKernelMemorySystem)),
                               NV_TRUE,
                               ADDR_SYSMEM,
                               NV_MEMORY_UNCACHED,
                               MEMDESC_FLAGS_ALLOC_32BIT_ADDRESSABLE);
        if (status != NV_OK)
            return status;

        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_140, 
                    pKernelMemorySystem->pSysmemFlushBufferMemDesc);

        //
        // Check if the memory allocation failed (probably due to no available
        // memory under 4GB).
        //
        if (status != NV_OK)
            bTryAgain = NV_TRUE;
        else
        {
            //
            // Check if the DMA address returned is not within 40-bit boundary
            // (possible on non-Linux platforms).
            //
            pKernelMemorySystem->sysmemFlushBuffer = memdescGetPhysAddr(pKernelMemorySystem->pSysmemFlushBufferMemDesc, AT_GPU, 0);
            if (pKernelMemorySystem->sysmemFlushBuffer < dmaWindowStartAddr ||
                pKernelMemorySystem->sysmemFlushBuffer + RM_PAGE_SIZE - 1 > dmaWindowEndAddr)
                bTryAgain = NV_TRUE;
        }

        //
        // If above checks are satisfied, do the actual sysmem flush buffer setup.
        // If not, try again with the WAR to temporarily lower the DMA address size.
        //
        if (!bTryAgain)
        {
            GPU_FLD_WR_DRF_NUM(pGpu, _PFB, _NISO_FLUSH_SYSMEM_ADDR, _ADR_39_08,
                               NvU64_LO32(pKernelMemorySystem->sysmemFlushBuffer >> kmemsysGetFlushSysmemBufferAddrShift_HAL(pGpu, pKernelMemorySystem)));

            return NV_OK;
        }

        memdescFree(pKernelMemorySystem->pSysmemFlushBufferMemDesc);
        memdescDestroy(pKernelMemorySystem->pSysmemFlushBufferMemDesc);

        status = memdescCreate(&pKernelMemorySystem->pSysmemFlushBufferMemDesc,
                               pGpu, RM_PAGE_SIZE,
                               (1 << kmemsysGetFlushSysmemBufferAddrShift_HAL(pGpu, pKernelMemorySystem)),
                               NV_TRUE,
                               ADDR_SYSMEM,
                               NV_MEMORY_UNCACHED,
                               MEMDESC_FLAGS_NONE);
        if (status != NV_OK)
            return status;
        //
        // Temporarily lower the DMA address size
        //
        // This is admittedly hacky and only safe during GPU initialization,
        // before other drivers like UVM (at least on Linux), can start
        // requesting its own DMA mappings for the same device.
        //
        // If DMA address size modification ever becomes needed in more places,
        // making it a part of the memdesc APIs would be cleaner.
        //
        if (gpuGetPhysAddrWidth_HAL(pGpu, ADDR_SYSMEM) > flushBufferDmaAddressSize)
        {
            memdescOverridePhysicalAddressWidthWindowsWAR(pGpu, pKernelMemorySystem->pSysmemFlushBufferMemDesc, flushBufferDmaAddressSize);
            osDmaSetAddressSize(pGpu->pOsGpuInfo, flushBufferDmaAddressSize);
        }

        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_141, 
                        pKernelMemorySystem->pSysmemFlushBufferMemDesc);

        // Restore it back to what HW supports
        if (gpuGetPhysAddrWidth_HAL(pGpu, ADDR_SYSMEM) > flushBufferDmaAddressSize)
        {
            osDmaSetAddressSize(pGpu->pOsGpuInfo, gpuGetPhysAddrWidth_HAL(pGpu, ADDR_SYSMEM));
        }

        if (status == NV_OK)
        {
            pKernelMemorySystem->sysmemFlushBuffer = memdescGetPhysAddr(pKernelMemorySystem->pSysmemFlushBufferMemDesc, AT_GPU, 0);
        }
        else if (status == NV_ERR_INVALID_ADDRESS)
        {
            if (NVCPU_IS_PPC64LE && dmaWindowStartAddr != 0)
            {
                pKernelMemorySystem->sysmemFlushBuffer = dmaWindowStartAddr;
            }
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not allocate sysmem flush buffer: %x\n", status);
            DBG_BREAKPOINT();
            return status;
        }

        // Manually redo the memdesc addressability check for the reduced address size
        if (pKernelMemorySystem->sysmemFlushBuffer < dmaWindowStartAddr ||
            pKernelMemorySystem->sysmemFlushBuffer + RM_PAGE_SIZE - 1 > dmaWindowEndAddr)
        {
            NvBool bMakeItFatal = NV_TRUE;
            NV_PRINTF(LEVEL_ERROR,
                      "GPU 0x%x: Allocated sysmem flush buffer not addressable 0x%llx\n",
                      pGpu->gpuId, pKernelMemorySystem->sysmemFlushBuffer);

            if (IS_FMODEL(pGpu) || IS_RTLSIM(pGpu) || IS_EMULATION(pGpu))
            {
                bMakeItFatal = NV_FALSE;
            }

            //
            // MODS on DGX-2 is hitting this. Make it non-fatal for now with
            // the proper WAR implementation tracked in bug 2403630.
            //

            //
            // Windows on greater than 2 TB systems is hitting this. Making it
            // non-fatal till a proper WAR is implemented. Bug 2423129 had
            // this issue.
            //
            if (RMCFG_FEATURE_PLATFORM_WINDOWS)
            {
                bMakeItFatal = NV_FALSE;
            }

            if (bMakeItFatal)
            {
                return NV_ERR_NO_MEMORY;
            }
        }
    }

    NV_ASSERT(pKernelMemorySystem->sysmemFlushBuffer != 0);
    GPU_FLD_WR_DRF_NUM(pGpu, _PFB, _NISO_FLUSH_SYSMEM_ADDR, _ADR_39_08,
                       NvU64_LO32(pKernelMemorySystem->sysmemFlushBuffer >> kmemsysGetFlushSysmemBufferAddrShift_HAL(pGpu, pKernelMemorySystem)));

    return NV_OK;
}

/*!
 * @brief Write the sysmemFlushBuffer val into the NV_PFB_NISO_FLUSH_SYSMEM_ADDR register
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in[ pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns void
 */
void
kmemsysProgramSysmemFlushBuffer_GM107
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    //
    // Q: Why are we writing this twice, both in fbInit and fbLoad?
    // A: fbLoad is preceded by busLoad which can do sysmem writes.
    // Writing in fbInit solves the load order guessing problem.
    //
    // Q: Why not just in fbInit?
    // A: Because on power management resume, this value should be restored too.
    //
    GPU_FLD_WR_DRF_NUM(pGpu, _PFB, _NISO_FLUSH_SYSMEM_ADDR, _ADR_39_08,
                       NvU64_LO32(pKernelMemorySystem->sysmemFlushBuffer >> kmemsysGetFlushSysmemBufferAddrShift_HAL(pGpu, pKernelMemorySystem)));
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
kmemsysAssertSysmemFlushBufferValid_GM107
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NV_ASSERT(GPU_REG_RD_DRF(pGpu, _PFB, _NISO_FLUSH_SYSMEM_ADDR, _ADR_39_08) != 0);
}

NvU32
kmemsysGetFlushSysmemBufferAddrShift_GM107
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    return NV_PFB_NISO_FLUSH_SYSMEM_ADDR_SHIFT;
}

NvU16
kmemsysGetMaximumBlacklistPages_GM107
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    return NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES;
}

/*!
 * @brief  Do any operations to get ready for a XVE sw reset.
 *
 * Set the PFIFO_FB_IFACE to DISABLE
 *
 * @return NV_OK
 */
NV_STATUS
kmemsysPrepareForXVEReset_GM107
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    GPU_REG_WR32(pGpu, NV_PFIFO_FB_IFACE,
                 DRF_DEF(_PFIFO, _FB_IFACE, _CONTROL, _DISABLE) |
                 DRF_DEF(_PFIFO, _FB_IFACE, _STATUS, _DISABLED));

    return NV_OK;
}
