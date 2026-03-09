/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_desc.h"

#include "published/blackwell/gb100/dev_nv_xal_ep_zb.h"
#include "published/blackwell/gb100/dev_ltc_zb.h"
#include "published/blackwell/gb100/dev_hshub.h"
#include "published/blackwell/gb100/dev_hshub_zb.h"
#include "published/blackwell/gb100/dev_fuse_zb.h"
#include "published/blackwell/gb100/hwproject.h"

#include "published/blackwell/gb100/dev_fb.h"
#include "published/blackwell/gb100/dev_gc6_island.h"
#include "published/blackwell/gb100/dev_gc6_island_addendum.h"

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
    IoAperture *pHshub0IoAperture = NULL;
    NV_STATUS status;

    status = objCreate(&pHshub0IoAperture, pGpu, IoAperture,
                       pGpu->pIOApertures[DEVICE_INDEX_GPU], NULL, 0, 0,
                       NULL, 0, NV_HSHUB0_PRIV_BASE, DRF_SIZE(NV_PFB_HSHUB_ZB));
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
 * @param[in] pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns NV_STATUS - NV_OK if sysmemFlushBuffer is valid otherwise NV_ERR_INVALID_STATE
 */
NV_STATUS
kmemsysAssertSysmemFlushBufferValid_GB100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU32       regHshubEgPcieFlushSysmemAddrValHi = 0;
    NvU32       regHshubEgPcieFlushSysmemAddrValLo = 0;
    IoAperture *pHshub0IoAperture = kmemsysInitHshub0Aperture_HAL(pGpu, pKernelMemorySystem);
    NV_STATUS   status = NV_OK;

    NV_ASSERT_OR_RETURN(pHshub0IoAperture != NULL, NV_ERR_INVALID_POINTER);

    regHshubEgPcieFlushSysmemAddrValLo = REG_RD32(pHshub0IoAperture,
                                               NV_PFB_HSHUB_ZB_EG_PCIE_FLUSH_SYSMEM_ADDR_LO);
    regHshubEgPcieFlushSysmemAddrValHi = REG_RD32(pHshub0IoAperture,
                                               NV_PFB_HSHUB_ZB_EG_PCIE_FLUSH_SYSMEM_ADDR_HI);

    if (regHshubEgPcieFlushSysmemAddrValLo == 0 && regHshubEgPcieFlushSysmemAddrValHi == 0)
    {
        status = NV_ERR_INVALID_STATE;
    }
    
    kmemsysDestroyHshub0Aperture_HAL(pGpu, pKernelMemorySystem, pHshub0IoAperture);
    
    return status;
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
    NV_ASSERT((alignedSysmemFlushBufferAddrHi & (~NV_PFB_HSHUB_ZB_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR_MASK)) == 0);

    alignedSysmemFlushBufferAddrHi &= NV_PFB_HSHUB_ZB_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR_MASK;

    regValHi = DRF_NUM(_PFB_HSHUB_ZB, _PCIE_FLUSH_SYSMEM_ADDR_HI, _ADR, alignedSysmemFlushBufferAddrHi);
    regValLo = DRF_NUM(_PFB_HSHUB_ZB, _PCIE_FLUSH_SYSMEM_ADDR_LO, _ADR, alignedSysmemFlushBufferAddr);

    REG_WR32(pHshub0IoAperture, NV_PFB_HSHUB_ZB_PCIE_FLUSH_SYSMEM_ADDR_HI, ((NvU32)regValHi));
    REG_WR32(pHshub0IoAperture, NV_PFB_HSHUB_ZB_PCIE_FLUSH_SYSMEM_ADDR_LO, ((NvU32)regValLo));

    // See bug 4503681 comment 47
    REG_WR32(pHshub0IoAperture, NV_PFB_HSHUB_ZB_EG_PCIE_FLUSH_SYSMEM_ADDR_HI, ((NvU32)regValHi));
    REG_WR32(pHshub0IoAperture, NV_PFB_HSHUB_ZB_EG_PCIE_FLUSH_SYSMEM_ADDR_LO, ((NvU32)regValLo));

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
    return portUtilCountTrailingZeros32(NV_PFB_HSHUB_ZB_PCIE_FLUSH_SYSMEM_ADDR_LO_ADR_MASK);
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
    IoAperture *pXalAperture = kbusGetXalAperture_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), XAL_BASE);
    NvU32 intr0 = REG_RD32(pXalAperture, NV_XAL_EP_ZB_INTR_0);
    return DRF_VAL(_XAL_EP_ZB, _INTR_0, _TRS_TIMEOUT, intr0) == NV_XAL_EP_ZB_INTR_0_TRS_TIMEOUT_PENDING;
#else
    return NV_FALSE;
#endif
}

/*!
 * @brief Extract FB offset from LOCAL_MEMORY_RANGE register value
 */
static inline NvU64 _kmemsysGetFbOffsetFromLocalMemoryRangeRegVal_GB100(NvU32 regVal)
{
    NvU32 lowerRangeMag   = DRF_VAL(_PFB, _PRI_MMU_LOCAL_MEMORY_RANGE, _LOWER_MAG, regVal);
    NvU32 lowerRangeScale = DRF_VAL(_PFB, _PRI_MMU_LOCAL_MEMORY_RANGE, _LOWER_SCALE, regVal);
    return ((NvU64) lowerRangeMag << (lowerRangeScale + 20));
}

/*!
 * @brief Read HDM top address from VBIOS (or 0 if not supported)
 */
NV_STATUS
kmemsysReadHdmTopFromVbios_GB100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU64 *pHdmTopOut
)
{
    /*
     * On GB100, some VBIOS versions emulate an HDM top by setting
     * LOCAL_MEMORY_RANGE to a smaller value after reset, then restoring
     * the true LOCAL_MEMORY_RANGE value after FSP boot commands. VBIOS then
     * stores the emulated HDM top value in a secure scratch register.
     *
     * Compare secure scratch vs. local memory range to determine if emulated
     * HDM top is present.
     */

    NvU64 localMemoryRange = _kmemsysGetFbOffsetFromLocalMemoryRangeRegVal_GB100(
        GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_LOCAL_MEMORY_RANGE));

    NvU64 scratchHdmTop = _kmemsysGetFbOffsetFromLocalMemoryRangeRegVal_GB100(
        GPU_REG_RD32(pGpu, NV_PGC6_BSI_SECURE_SCRATCH_MMU_LOCAL_MEMORY_RANGE));

    if (localMemoryRange != scratchHdmTop)
    {
        *pHdmTopOut = scratchHdmTop;
    }
    else
    {
        *pHdmTopOut = 0;
    }

    return NV_OK;
}

NvBool
kmemsysCheckReadoutEccEnablement_GB100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU32 fuse = GPU_REG_RD32(pGpu, gpuGetPrimaryFuseBaseAddr_HAL(pGpu) + NV_FUSE_ZB_FEATURE_READOUT);
    return FLD_TEST_DRF(_FUSE_ZB, _FEATURE_READOUT, _ECC_DRAM, _ENABLED, fuse);
}

NvU32
kmemsysGetL2EccDedCountRegAddr_GB100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32               fbpa,
    NvU32               subp
)
{
    return (NV_LTC_PRI_BASE + NV_PLTC_LTS0_L2_CACHE_ECC_UNCORRECTED_ERR_COUNT +
            (fbpa * NV_LTC_PRI_STRIDE) + (subp * NV_LTS_PRI_STRIDE));
}
