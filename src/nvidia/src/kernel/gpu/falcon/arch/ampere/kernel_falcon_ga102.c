/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * Provides the implementation for all GA102+ specific KernelFalcon
 * interfaces.
 */

#include "gpu/falcon/kernel_falcon.h"
#include "os/os.h"

#include "published/ampere/ga102/dev_falcon_v4.h"
#include "published/ampere/ga102/dev_falcon_v4_addendum.h"
#include "published/ampere/ga102/dev_riscv_pri.h"


#define PRE_RESET_PRE_SILICON_TIMEOUT_US    300000
#define PRE_RESET_TIMEOUT_US                150


/*!
 * Function to check if RISCV is active
 */
NvBool
kflcnIsRiscvActive_GA10X
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    NvU32 val = kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_CPUCTL);

    return FLD_TEST_DRF(_PRISCV, _RISCV_CPUCTL, _ACTIVE_STAT, _ACTIVE, val);
}

/*!
 * Reset falcon using secure reset, ready to run riscv.
 */
void
kflcnResetIntoRiscv_GA102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    NV_ASSERT_OR_RETURN_VOID(kflcnPreResetWait_HAL(pGpu, pKernelFlcn) == NV_OK);
    NV_ASSERT_OK(kflcnResetHw(pGpu, pKernelFlcn));
    kflcnWaitForResetToFinish_HAL(pGpu, pKernelFlcn);
    kflcnRiscvProgramBcr_HAL(pGpu, pKernelFlcn, NV_TRUE);
}

/*!
 * Program BCR register of RISCV
 *
 * @param[in] pGpu            OBJGPU pointer
 * @param[in] pKernelFlcn     KernelFalcon pointer
 * @param[in] bBRFetch        BR_FETCH field value of BCR register
 */
void
kflcnRiscvProgramBcr_GA102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvBool bBRFetch
)
{
    NvU32 bcr;

    bcr = DRF_DEF(_PRISCV_RISCV, _BCR_CTRL, _CORE_SELECT, _RISCV)   |
          DRF_DEF(_PRISCV_RISCV, _BCR_CTRL, _VALID,       _TRUE)    |
          DRF_NUM(_PRISCV_RISCV, _BCR_CTRL, _BRFETCH,     bBRFetch);

    kflcnRiscvRegWrite_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_BCR_CTRL, bcr);
}

/*!
 * Switch the core to FALCON.  Releases priv lockdown.
 * Should not be called while in reset.  See bug 200586493.
 */
void kflcnSwitchToFalcon_GA10X
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    NvU32     bcrCtrl;
    RMTIMEOUT timeout;
    NV_STATUS status = NV_OK;

    // If RISC-V is not enabled, then core must already be in Falcon
    if (!kflcnIsRiscvCpuEnabled_HAL(pGpu, pKernelFlcn))
        return;

    bcrCtrl = kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_BCR_CTRL);

    if (FLD_TEST_DRF(_PRISCV_RISCV, _BCR_CTRL, _CORE_SELECT, _FALCON, bcrCtrl))
        return;

    kflcnRiscvRegWrite_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_BCR_CTRL,
                           DRF_DEF(_PRISCV_RISCV, _BCR_CTRL, _CORE_SELECT, _FALCON));

    // Wait for Peregrine to report VALID, indicating that the core switch is successful
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    for (;;)
    {
        bcrCtrl = kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_BCR_CTRL);
        if (FLD_TEST_DRF(_PRISCV_RISCV, _BCR_CTRL, _VALID, _TRUE, bcrCtrl))
            break;

        if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
            status = NV_ERR_GPU_IN_FULLCHIP_RESET;
        else if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
            status = NV_ERR_GPU_IS_LOST;
        else
            status = gpuCheckTimeout(pGpu, &timeout);

        if (status != NV_OK)
            break;
    }

    if (status != NV_OK)
    {
        NV_ASSERT_OK_FAILED("Failed to switch core to Falcon mode", status);
    }
}

/*!
 * Pre-Reset sequence for Falcon/RiscV core.
 *
 * Read RESET_READY bit of HWCFG2 register.
 * Bug 3419321: This sometimes may not get set by HW, so use time out.
 */
NV_STATUS
kflcnPreResetWait_GA10X
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    NvU32 hwcfg2;
    RMTIMEOUT timeout;
    NvU32 flags = (GPU_TIMEOUT_FLAGS_TMR |
                   GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE |
                   GPU_TIMEOUT_FLAGS_BYPASS_JOURNAL_LOG);

    if (!IS_SILICON(pGpu) && !IS_EMULATION(pGpu))
    {
        return NV_OK;
    }

    hwcfg2 = kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_HWCFG2);

    if (IS_SILICON(pGpu))
    {
        gpuSetTimeout(pGpu, PRE_RESET_TIMEOUT_US, &timeout, flags);
    }
    else if (IS_EMULATION(pGpu))
    {
        gpuSetTimeout(pGpu, PRE_RESET_PRE_SILICON_TIMEOUT_US, &timeout, flags);
    }

    while (!FLD_TEST_DRF(_PFALCON, _FALCON_HWCFG2, _RESET_READY, _TRUE, hwcfg2))
    {
        if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
        {
            break;
        }

        osSpinLoop();

        hwcfg2 = kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_HWCFG2);
    }

    return NV_OK;
}

/*!
 * Wait for Falcon memory scrubbing to finish.
 *
 * Receives a Gpu pointer and a void pointer that must be to a KernelFalcon
 * object, to facilitate use with gpuTimeoutCondWait.
 *
 * @param pGpu                OBJGPU pointer
 * @param pVoid               KernelFalcon pointer
 */
static NvBool
_kflcnWaitForScrubbingToFinish(OBJGPU *pGpu, void *pVoid)
{
    NvBool bResult = NV_FALSE;
    NvU32 dmaCtrl = 0;
    KernelFalcon *pKernelFlcn = reinterpretCast(pVoid, KernelFalcon *);

    dmaCtrl = kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_HWCFG2);

    if (FLD_TEST_DRF(_PFALCON, _FALCON_HWCFG2, _MEM_SCRUBBING, _DONE, dmaCtrl))
    {
        bResult = NV_TRUE;
    }

    return bResult;
}

NV_STATUS
kflcnWaitForResetToFinish_GA102(OBJGPU *pGpu, KernelFalcon *pKernelFlcn)
{
    // Skip the wait if we are in the reset path
    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
        return NV_ERR_GPU_IN_FULLCHIP_RESET;

    //
    // We could potentially bypass the polling if we are going to read from IMEM or DMEM.
    // But waiting ensures we avoid pri timouts.  See bug 623410.
    //
    return gpuTimeoutCondWait(pGpu, _kflcnWaitForScrubbingToFinish, pKernelFlcn, NULL);
}

/*!
 * Read the IRQ status of the RISCV Falcon.
 *
 * @return IRQ status mask
 */
NvU32
kflcnReadIntrStatus_GA102
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

