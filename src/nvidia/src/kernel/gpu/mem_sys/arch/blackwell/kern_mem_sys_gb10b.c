/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/mem_sys/kern_mem_sys.h"

#include "published/blackwell/gb10b/dev_fbhub_zb.h"
#include "published/blackwell/gb10b/dev_top_zb.h"
#include "published/blackwell/gb10b/hwproject.h"

/*!
 * @brief Write the sysmemFlushBuffer val into the NV_FBHUB_ZB_PCIE_FLUSH_SYSMEM_ADDR register
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in[ pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns void
 */
void
kmemsysProgramSysmemFlushBuffer_GB10B
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU32                  alignedSysmemFlushBufferAddr   = 0x0;
    NvU32                  alignedSysmemFlushBufferAddrHi = 0x0;
    NvU32                  addr;

    NV_ASSERT(pKernelMemorySystem->sysmemFlushBuffer != 0);

    // alignedSysmemFlushBufferAddr will have the lower 32 bits of the buffer address
    alignedSysmemFlushBufferAddr = NvU64_LO32(pKernelMemorySystem->sysmemFlushBuffer);

    // alignedSysmemFlushBufferAddrHi will have the upper 32 bits of the buffer address
    alignedSysmemFlushBufferAddrHi = NvU64_HI32(pKernelMemorySystem->sysmemFlushBuffer);

    // Assert when Sysmem Flush buffer has more than 52-bit address
    NV_ASSERT((alignedSysmemFlushBufferAddrHi & (~NV_FBHUB_ZB_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR_MASK)) == 0);

    alignedSysmemFlushBufferAddrHi &= NV_FBHUB_ZB_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR_MASK;
    addr = NV_FBHUB0_PRI_BASE + NV_FBHUB_ZB_PCIE_FLUSH_SYSMEM_ADDR_HI;
    GPU_REG_WR32(pGpu, addr,
                 FLD_SET_DRF_NUM(_FBHUB_ZB, _PCIE_FLUSH_SYSMEM_ADDR_HI, _ADR,
                                 alignedSysmemFlushBufferAddrHi,
                                 GPU_REG_RD32(pGpu, addr)));
    
    addr = NV_FBHUB0_PRI_BASE + NV_FBHUB_ZB_PCIE_FLUSH_SYSMEM_ADDR_LO;
    GPU_REG_WR32(pGpu, addr,
                 FLD_SET_DRF_NUM(_FBHUB_ZB, _PCIE_FLUSH_SYSMEM_ADDR_LO, _ADR,
                                 alignedSysmemFlushBufferAddr,
                                 GPU_REG_RD32(pGpu, addr)));
}

/*!
 * @brief Validate the sysmemFlushBuffer val and assert
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in[ pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns void
 */
NV_STATUS
kmemsysAssertSysmemFlushBufferValid_GB10B
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NV_STATUS status;
    const DEVICE_INFO_ENTRY *pFbhubEntry;
    status = gpuGetOneDeviceEntry(pGpu,
                                  NV_PTOP_ZB_DEVICE_INFO_DEV_TYPE_ENUM_FBHUB,
                                  DEVICE_INFO_DIELET_INSTANCE_ANY,
                                  0,
                                  DEVICE_INFO_DIE_LOCAL_INSTANCE_ID_ANY,
                                  &pFbhubEntry);
    NV_ASSERT_OK_OR_RETURN(status);
    NvU32 regPfbFbhub0PcieFlushSysmemAddrValLo = DRF_VAL(_FBHUB_ZB, _PCIE_FLUSH_SYSMEM_ADDR_LO, _ADR,
                                                 GPU_REG_RD32(pGpu,
                                                    pFbhubEntry->devicePriBase + NV_FBHUB_ZB_PCIE_FLUSH_SYSMEM_ADDR_LO));
    NvU32 regPfbFbhub0PcieFlushSysmemAddrValHi = DRF_VAL(_FBHUB_ZB, _PCIE_FLUSH_SYSMEM_ADDR_HI, _ADR,
                                                 GPU_REG_RD32(pGpu,
                                                    pFbhubEntry->devicePriBase + NV_FBHUB_ZB_PCIE_FLUSH_SYSMEM_ADDR_HI));

    if (regPfbFbhub0PcieFlushSysmemAddrValLo == 0 && regPfbFbhub0PcieFlushSysmemAddrValHi == 0)
    {
        return NV_ERR_INVALID_STATE;
    }

    return NV_OK;
}

/*!
 * @brief Gets the address shift for the sysmem flush buffer address.
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in[ pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns NvU32 - The number of bits the address needs to be shifted by
 */
NvU32
kmemsysGetFlushSysmemBufferAddrShift_GB10B
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    return portUtilCountTrailingZeros32(NV_FBHUB_ZB_PCIE_FLUSH_SYSMEM_ADDR_LO_ADR_MASK);
}
