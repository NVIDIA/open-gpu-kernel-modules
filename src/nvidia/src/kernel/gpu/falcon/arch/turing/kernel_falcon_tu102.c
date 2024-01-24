/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * Provides the implementation for all TU102+ specific KernelFalcon interfaces.
 */

#include "gpu/falcon/kernel_falcon.h"
#include "os/os.h"

#include "published/turing/tu102/dev_riscv_pri.h"
#include "published/turing/tu102/dev_falcon_v4.h"
#include "published/turing/tu102/dev_fbif_v4.h"

/*!
 * Read a Falcon register.
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pKernelFlcn   KernelFalcon pointer
 * @param[in]   offset        Offset into the Falcon register space.
 *
 * @returns The value of the register.
 */
NvU32
kflcnRegRead_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 offset
)
{
    return REG_INST_DEVIDX_RD32_EX(pGpu, DEVICE_INDEX_GPU, 0,
               pKernelFlcn->registerBase + offset, NULL);
}

/*!
 * Write a Falcon register.
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pKernelFlcn   KernelFalcon pointer
 * @param[in]   offset        Offset into the Falcon register space.
 * @param[in]   data          Data to write to the register.
 */
void
kflcnRegWrite_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 offset,
    NvU32 data
)
{
    REG_INST_DEVIDX_WR32_EX(pGpu, DEVICE_INDEX_GPU, 0,
        pKernelFlcn->registerBase + offset, data, NULL);
}

/*!
 * Read a RISCV register.
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pKernelFlcn   KernelFalcon pointer
 * @param[in]   offset        Offset into the RISCV register space.
 *
 * @returns The value of the register.
 */
NvU32
kflcnRiscvRegRead_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 offset
)
{
    return REG_INST_DEVIDX_RD32_EX(pGpu, DEVICE_INDEX_GPU, 0,
               pKernelFlcn->riscvRegisterBase + offset, NULL);
}

/*!
 * Write a RISCV register.
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pKernelFlcn   KernelFalcon pointer
 * @param[in]   offset        Offset into the RISCV register space.
 * @param[in]   data          Data to write to the register.
 */
void
kflcnRiscvRegWrite_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 offset,
    NvU32 data
)
{
    REG_INST_DEVIDX_WR32_EX(pGpu, DEVICE_INDEX_GPU, 0,
        pKernelFlcn->riscvRegisterBase + offset, data, NULL);
}

/*!
 * Check the existence of RISCV CPU.
 */
NvBool
kflcnIsRiscvCpuEnabled_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    NvU32 reg = kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_HWCFG2);

    return FLD_TEST_DRF(_PFALCON, _FALCON_HWCFG2, _RISCV, _ENABLE, reg);
}

/*!
 * Function to check if RISCV is active.
 */
NvBool
kflcnIsRiscvActive_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    NvU32 val;

    val = kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_CORE_SWITCH_RISCV_STATUS);

    return FLD_TEST_DRF(_PRISCV, _RISCV_CORE_SWITCH_RISCV_STATUS, _ACTIVE_STAT, _ACTIVE, val);
}

/*!
 * Reset falcon using secure reset.
 * This leaves the falcon in falcon mode after reset.
 */
void
kflcnReset_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    NV_ASSERT_OR_RETURN_VOID(kflcnPreResetWait_HAL(pGpu, pKernelFlcn) == NV_OK);
    NV_ASSERT_OK(kflcnResetHw(pGpu, pKernelFlcn));
    kflcnWaitForResetToFinish_HAL(pGpu, pKernelFlcn);
    kflcnSwitchToFalcon_HAL(pGpu, pKernelFlcn);
    kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_RM,
                      pGpu->chipId0);
}

/*!
 * Reset falcon using secure reset, ready to run riscv.
 */
void
kflcnResetIntoRiscv_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    //
    // Turing and GA100 do not have an explicit core switch,
    // the core will be ready to run riscv after reset.
    //
    kflcnReset_TU102(pGpu, pKernelFlcn);
}

/*!
 * Start a Falcon CPU.
 */
void
kflcnStartCpu_TU102(OBJGPU *pGpu, KernelFalcon *pKernelFlcn)
{
    if (FLD_TEST_DRF(_PFALCON, _FALCON_CPUCTL, _ALIAS_EN, _TRUE,
                     kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_CPUCTL)))
    {
        kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_CPUCTL_ALIAS,
                          DRF_DEF(_PFALCON, _FALCON_CPUCTL_ALIAS, _STARTCPU, _TRUE));
    }
    else
    {
        kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_CPUCTL,
                          DRF_DEF(_PFALCON, _FALCON_CPUCTL, _STARTCPU, _TRUE));
    }
}

/*!
 * Disables context requirement of Falcon.
 */
void
kflcnDisableCtxReq_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    NvU32 data = 0;

    data = GPU_REG_RD32(pGpu, pKernelFlcn->fbifBase + NV_PFALCON_FBIF_CTL);
    data = FLD_SET_DRF(_PFALCON, _FBIF_CTL, _ALLOW_PHYS_NO_CTX, _ALLOW, data);

    // Allow physical address without CTX
    GPU_REG_WR32(pGpu, pKernelFlcn->fbifBase + NV_PFALCON_FBIF_CTL, data);

    // Allow issue DMA request without block bind
    kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_DMACTL, 0x0);
}

/*!
 * Checks if Falcon memory scrubbing is finished.
 *
 * @param pGpu   OBJGPU pointer
 * @param pVoid  void pointer to a KernelFalcon instance
 */
static NvBool
_kflcnMemScrubbingFinished
(
    OBJGPU *pGpu,
    void *pVoid
)
{
    NvBool bResult = NV_FALSE;
    NvU32 dmaCtrl = 0;
    KernelFalcon *pKernelFlcn = reinterpretCast(pVoid, KernelFalcon *);

    dmaCtrl = kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_DMACTL);

    if (FLD_TEST_DRF(_PFALCON, _FALCON_DMACTL, _DMEM_SCRUBBING, _DONE, dmaCtrl) &&
        FLD_TEST_DRF(_PFALCON, _FALCON_DMACTL, _IMEM_SCRUBBING, _DONE, dmaCtrl))
    {
        bResult = NV_TRUE;
    }

    return bResult;
}

/*!
 * Wait for Falcon reset to finish.
 */
NV_STATUS
kflcnWaitForResetToFinish_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    // Skip the wait if we are in the GPU reset path
    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
        return NV_ERR_GPU_IN_FULLCHIP_RESET;

    //
    // We could potentially bypass the polling if we are going to read from IMEM or DMEM.
    // But waiting ensures we avoid pri timouts.  See bug 623410.
    //
    return gpuTimeoutCondWait(pGpu, _kflcnMemScrubbingFinished, pKernelFlcn, NULL);
}

/*!
 * Wait for Falcon to halt.
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelFlcn    KernelFalcon pointer
 * @param[in]  timeoutUs      Timeout value
 *
 * @returns NV_ERR_TIMEOUT if falcon fails to halt.
 */
NV_STATUS
kflcnWaitForHalt_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 timeoutUs,
    NvU32 flags
)
{
    NV_STATUS status = NV_OK;
    RMTIMEOUT timeout;

    gpuSetTimeout(pGpu, timeoutUs, &timeout, flags);

    while (!FLD_TEST_DRF(_PFALCON, _FALCON, _CPUCTL_HALTED, _TRUE,
                         kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_CPUCTL)))
    {
        status = gpuCheckTimeout(pGpu, &timeout);
        if (status == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR, "Timeout waiting for Falcon to halt\n");
            DBG_BREAKPOINT();
            break;
        }
        osSpinLoop();
    }

    return status;
}

/*!
 * Read the IRQ status of the RISCV Falcon.
 *
 * @return IRQ status mask
 */
NvU32
kflcnReadIntrStatus_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    return ((kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_IRQSTAT) &
             kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_IRQMASK) &
             kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_IRQDEST)) |
            (kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_IRQSTAT) &
             kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_IRQMASK) &
             kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_IRQDEST)));
}


/*!
 * Mask a IMEM address to have only the BLK and OFFSET bits set.
 *
 * @param[in] addr    IMEM address
 */
NvU32
kflcnMaskImemAddr_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 addr

)
{
    return (addr & (DRF_SHIFTMASK(NV_PFALCON_FALCON_IMEMC_OFFS) |
                    DRF_SHIFTMASK(NV_PFALCON_FALCON_IMEMC_BLK)));
}

/*!
 * Mask a DMEM address to have only the BLK and OFFSET bits set.
 *
 * @param[in] addr    DMEM address
 */
NvU32
kflcnMaskDmemAddr_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 addr
)
{
    return (addr & (DRF_SHIFTMASK(NV_PFALCON_FALCON_DMEMC_OFFS) |
                    DRF_SHIFTMASK(NV_PFALCON_FALCON_DMEMC_BLK)));
}
