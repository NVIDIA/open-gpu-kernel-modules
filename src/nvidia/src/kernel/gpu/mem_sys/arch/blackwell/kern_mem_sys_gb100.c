/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/mem_desc.h"

#include "published/blackwell/gb100/pri_nv_xal_ep.h"

#include "published/blackwell/gb100/dev_top.h"
#include "published/blackwell/gb100/dev_hshub_base.h"

/*!
 * @brief Function used to return the HSHUB0 IoAperture
 *        Used by sysmem flush buffer code, since it gets called
 *        before HSHUB IO apertures are constructed by HSHUB class object
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in[ pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns IoAperture used to write to HSHUB0 PRI's
 */
IoAperture*
kmemsysInitHshub0Aperture_GB100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU64 hshub0PriBaseAddress;
    IoAperture *pHshub0IoAperture = NULL;
    NV_STATUS status;

// TODO Remove this hardcoded value by fixing bug 4313915
#define NV_PFB_HSHUB0      0x00870fff:0x00870000
    hshub0PriBaseAddress = DRF_BASE(NV_PFB_HSHUB0);
    status = objCreate(&pHshub0IoAperture, pGpu, IoAperture,
                         pGpu->pIOApertures[DEVICE_INDEX_GPU], NULL, 0, 0,
                         NULL, 0, hshub0PriBaseAddress, DRF_SIZE(NV_PFB_HSHUB));
    if (status != NV_OK)
    {
        return NULL;
    }

    return pHshub0IoAperture;
}

/*!
 * @brief Function used to destroy the HSHUB0 IoAperture
 *        Used by sysmem flush buffer code, since it gets called
 *        before HSHUB IO apertures are constructed by HSHUB class object
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in[ pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns void
 */
void
kmemsysDestroyHshub0Aperture_GB100
(
    OBJGPU             *pGpu, 
    KernelMemorySystem *pKernelMemorySystem, 
    IoAperture         *pIoAperture
)
{
    objDelete(pIoAperture);
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
kmemsysAssertSysmemFlushBufferValid_GB100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU32       regHshubPcieFlushSysmemAddrValHi = 0;
    NvU32       regHshubPcieFlushSysmemAddrValLo = 0;
    NvU32       regHshubEgPcieFlushSysmemAddrValHi = 0;
    NvU32       regHshubEgPcieFlushSysmemAddrValLo = 0;
    IoAperture *pHshub0IoAperture = kmemsysInitHshub0Aperture_HAL(pGpu, pKernelMemorySystem);

    NV_ASSERT_OR_RETURN_VOID(pHshub0IoAperture != NULL);

    regHshubPcieFlushSysmemAddrValLo = REG_RD32(pHshub0IoAperture,
                                             NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_LO);
    regHshubPcieFlushSysmemAddrValHi = REG_RD32(pHshub0IoAperture,
                                             NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_HI);

    NV_ASSERT((regHshubPcieFlushSysmemAddrValLo != 0) || (regHshubPcieFlushSysmemAddrValHi != 0));

    regHshubEgPcieFlushSysmemAddrValLo = REG_RD32(pHshub0IoAperture,
                                               NV_PFB_HSHUB_EG_PCIE_FLUSH_SYSMEM_ADDR_LO);
    regHshubEgPcieFlushSysmemAddrValHi = REG_RD32(pHshub0IoAperture,
                                               NV_PFB_HSHUB_EG_PCIE_FLUSH_SYSMEM_ADDR_HI);

    NV_ASSERT((regHshubEgPcieFlushSysmemAddrValLo != 0) || (regHshubEgPcieFlushSysmemAddrValHi != 0));

    //
    // In addition to a non-zero address, both NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_<> and 
    // NV_PFB_HSHUB_EG_PCIE_FLUSH_SYSMEM_ADDR_<> must program same value.
    //
    NV_ASSERT((regHshubPcieFlushSysmemAddrValLo == regHshubEgPcieFlushSysmemAddrValLo) &&
        (regHshubPcieFlushSysmemAddrValHi == regHshubEgPcieFlushSysmemAddrValHi));

    kmemsysDestroyHshub0Aperture_HAL(pGpu, pKernelMemorySystem, pHshub0IoAperture);
}

/*!
 * @brief Write the sysmemFlushBuffer val into the NV_PFB_FBHUB_BASE_PCIE_FLUSH_SYSMEM_ADDR register
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in[ pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns void
 */
void
kmemsysProgramSysmemFlushBuffer_GB100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU32       alignedSysmemFlushBufferAddr   = 0x0;
    NvU32       alignedSysmemFlushBufferAddrHi = 0x0;
    NvU32       regValHi;
    NvU32       regValLo;
    IoAperture *pHshub0IoAperture = kmemsysInitHshub0Aperture_HAL(pGpu, pKernelMemorySystem);

    NV_ASSERT_OR_RETURN_VOID(pHshub0IoAperture != NULL);
    NV_ASSERT(pKernelMemorySystem->sysmemFlushBuffer != 0);

    // alignedSysmemFlushBufferAddr will have the lower 32 bits of the buffer address
    alignedSysmemFlushBufferAddr = NvU64_LO32(pKernelMemorySystem->sysmemFlushBuffer);

    // alignedSysmemFlushBufferAddrHi will have the upper 32 bits of the buffer address
    alignedSysmemFlushBufferAddrHi = NvU64_HI32(pKernelMemorySystem->sysmemFlushBuffer);

    // Assert when Sysmem Flush buffer has more than 52-bit address
    NV_ASSERT((alignedSysmemFlushBufferAddrHi & (~NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR_MASK)) == 0);

    alignedSysmemFlushBufferAddrHi &= NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR_MASK;

    regValHi = DRF_NUM(_PFB, _HSHUB_PCIE_FLUSH_SYSMEM_ADDR_HI, _ADR, alignedSysmemFlushBufferAddrHi);
    regValLo = DRF_NUM(_PFB, _HSHUB_PCIE_FLUSH_SYSMEM_ADDR_LO, _ADR, alignedSysmemFlushBufferAddr);

    REG_WR32(pHshub0IoAperture, NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_HI, ((NvU32)regValHi));
    REG_WR32(pHshub0IoAperture, NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_LO, ((NvU32)regValLo));

    // See bug 4503681 comment 47
    REG_WR32(pHshub0IoAperture, NV_PFB_HSHUB_EG_PCIE_FLUSH_SYSMEM_ADDR_HI, ((NvU32)regValHi));
    REG_WR32(pHshub0IoAperture, NV_PFB_HSHUB_EG_PCIE_FLUSH_SYSMEM_ADDR_LO, ((NvU32)regValLo));

    kmemsysDestroyHshub0Aperture_HAL(pGpu, pKernelMemorySystem, pHshub0IoAperture);
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
kmemsysGetFlushSysmemBufferAddrShift_GB100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    return portUtilCountTrailingZeros32(NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_LO_ADR_MASK);
}

/*
 * @brief   Function to check if an FB ACK timeout occured. Used only for Debug.
 */
NvBool
kmemsysAssertFbAckTimeoutPending_GB100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
#ifdef DEBUG
    NvU32 intr0 = GPU_REG_RD32(pGpu, NV_XAL_EP_INTR_0);
    return DRF_VAL(_XAL_EP, _INTR_0, _TRS_TIMEOUT, intr0) == NV_XAL_EP_INTR_0_TRS_TIMEOUT_PENDING;
#else
    return NV_FALSE;
#endif
}
