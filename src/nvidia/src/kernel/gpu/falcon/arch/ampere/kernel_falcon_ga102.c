/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/falcon/kernel_falcon_core_dump.h"
#include "os/os.h"

#include "published/ampere/ga102/dev_falcon_v4.h"
#include "published/ampere/ga102/dev_falcon_v4_addendum.h"
#include "published/ampere/ga102/dev_riscv_pri.h"
#include "published/ampere/ga102/dev_fbif_v4.h"


#define PRE_RESET_PRE_SILICON_TIMEOUT_US    300000
#define PRE_RESET_TIMEOUT_US                150


/*!
 * Function to check if RISCV is active
 */
NvBool
kflcnIsRiscvActive_GA102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    NvU32 val = kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_CPUCTL);

    return FLD_TEST_DRF(_PRISCV, _RISCV_CPUCTL, _ACTIVE_STAT, _ACTIVE, val);
}

/*!
 * Returns true if the RISC-V core is selected
 */
NvBool
kflcnIsRiscvSelected_GA102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFalcon
)
{
    const NvU32 privErrVal = 0xbadf0000;
    const NvU32 privErrMask = 0xffff0000;
    NvU32 val = kflcnRiscvRegRead_HAL(pGpu, pKernelFalcon, NV_PRISCV_RISCV_BCR_CTRL);

    //
    // If NV_PRISCV_RISCV_BCR_CTRL is locked out from reads (e.g., by PLM), assume the RISC-V core
    // is in use. Nearly all ucodes set the RISCV_BCR PLM to allow RO for all sources.
    //
    return (FLD_TEST_DRF(_PRISCV, _RISCV_BCR_CTRL, _CORE_SELECT, _RISCV, val) ||
            ((val & privErrMask) == privErrVal));
}

/*!
 * Reset falcon using secure reset, ready to run riscv.
 */
NV_STATUS
kflcnResetIntoRiscv_GA102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    NV_ASSERT_OK_OR_RETURN(kflcnPreResetWait_HAL(pGpu, pKernelFlcn));
    NV_ASSERT_OK(kflcnResetHw(pGpu, pKernelFlcn));
    NV_ASSERT_OK_OR_RETURN(kflcnWaitForResetToFinish_HAL(pGpu, pKernelFlcn));
    kflcnRiscvProgramBcr_HAL(pGpu, pKernelFlcn, NV_TRUE);
    kflcnSetRiscvMode(pKernelFlcn, NV_TRUE);
    return NV_OK;
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
void kflcnSwitchToFalcon_GA102
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
    else
    {
        kflcnSetRiscvMode(pKernelFlcn, NV_FALSE);
    }
}

/*!
 * Pre-Reset sequence for Falcon/RiscV core.
 *
 * Read RESET_READY bit of HWCFG2 register.
 * Bug 3419321: This sometimes may not get set by HW, so use time out.
 */
NV_STATUS
kflcnPreResetWait_GA102
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
 * Wait for RISC-V to halt.
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelFlcn    KernelFalcon pointer
 * @param[in]  timeoutUs      Timeout value
 *
 * @returns NV_ERR_TIMEOUT if RISC-V fails to halt.
 */
NV_STATUS
kflcnWaitForHaltRiscv_GA102
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

    while (!FLD_TEST_DRF_NUM(_PRISCV, _RISCV, _CPUCTL_HALTED, 0x1,
                             kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_CPUCTL)))
    {
        status = gpuCheckTimeout(pGpu, &timeout);
        if (status == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR, "Timeout waiting for RISC-V to halt\n");
            DBG_BREAKPOINT();
            break;
        }
        osSpinLoop();
    }

    return status;
}

/*!
 * Read the IRQ status of the Falcon in RISC-V mode.
 *
 * @return IRQ status mask
 */
NvU32
kflcnRiscvReadIntrStatus_GA102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    return (kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_IRQSTAT) &
            kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_IRQMASK) &
            kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_IRQDEST));
}

/*!
 * Function to read the ICD_CMD register.
 */
NvU32 kflcnIcdReadCmdReg_GA102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    return kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_ICD_CMD);
}

/*!
 * Function to read the ICD_RDATA register pair.
 */
NvU64 kflcnRiscvIcdReadRdata_GA102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    return (((NvU64)kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_ICD_RDATA1)) << 32) |
        kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_ICD_RDATA0);
}

/*!
 * Function to write the ICD_ADDR register pair.
 */
void kflcnRiscvIcdWriteAddress_GA102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU64 address
)
{
    kflcnRiscvRegWrite_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_ICD_ADDR1, address >> 32);
    kflcnRiscvRegWrite_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_ICD_ADDR0, (NvU32) address);
}

/*!
 * Function to write the ICD_CMD register.
 */
void kflcnIcdWriteCmdReg_GA102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 value
)
{
    kflcnRiscvRegWrite_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_ICD_CMD, value);
}

void
kflcnDumpTracepc_GA102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    CoreDumpRegs *pCore
)
{
    NvU64 pc;
    NvU32 ctl;
    NvU32 r, w, size;
    NvU32 entry;
    NvU32 count;

    r = kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_TRACE_RDIDX);
    w = kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_TRACE_WTIDX);

    if (((r & 0xbadf0000) == 0xbadf0000) &&
        ((w & 0xbadf0000) == 0xbadf0000))
    {
        NV_PRINTF(LEVEL_ERROR, "Trace buffer blocked, skipping.\n");
        return;
    }

    size = DRF_VAL(_PRISCV_RISCV, _TRACE_RDIDX, _MAXIDX, r);

    if (size > __RISCV_MAX_TRACE_ENTRIES)
    {
        NV_PRINTF(LEVEL_ERROR, "Trace buffer larger than expected. Bailing!\n");
        return;
    }

    r = DRF_VAL(_PRISCV_RISCV, _TRACE_RDIDX, _RDIDX, r);
    w = DRF_VAL(_PRISCV_RISCV, _TRACE_WTIDX, _WTIDX, w);

    ctl = kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_TRACECTL);

    if ((w == r) && (DRF_VAL(_PRISCV_RISCV, _TRACECTL, _FULL, ctl) == 0))
    {
        count = 0;
    }
    else
    {
        //
        // The number of entries in trace buffer is how far the w (put) pointer
        // is ahead of the r (get) pointer. If this value is negative, add
        // the size of the circular buffer to bring the element count back into range.
        //
        count = w > r ? w - r : w - r + size;
    }

    pCore->tracePCEntries = count;

    if (count)
    {
        for (entry = 0; entry < count; ++entry)
        {
            if (entry > w)
                w += size;
            kflcnRiscvRegWrite_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_TRACE_RDIDX, w - entry);

            pc = ((NvU64)kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_TRACEPC_HI) << 32ull) |
                    kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_TRACEPC_LO);
            pCore->tracePC[entry] = pc;
        }
    }

    // Restore original value
    kflcnRiscvRegWrite_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_TRACE_RDIDX, r);
    return;
}

NV_STATUS kflcnCoreDumpPc_GA102(OBJGPU *pGpu, KernelFalcon *pKernelFlcn, NvU64 *pc)
{
    // 
    // This code originally handled 0xbadfxxxx values and returned failure,
    // however we may want to see badf values so it is now wired to return the read
    // register always. We want to also ensure any automated processing will know to
    // attempt a soft decode of the lower 32 bits as it is not a complete address.
    //
    *pc = 0xfa11bacc00000000ull | (NvU64)kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_RPC);
    return NV_OK;
}

void
kflcnDumpCoreRegs_GA102(OBJGPU *pGpu, KernelFalcon *pKernelFlcn, CoreDumpRegs *pCore)
{
#define __CORE_DUMP_RISCV_REG(x,y) do { pCore->x = kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, (y)); } while (0)
    __CORE_DUMP_RISCV_REG(riscvCpuctl,       NV_PRISCV_RISCV_CPUCTL);
    __CORE_DUMP_RISCV_REG(riscvIrqmask,      NV_PRISCV_RISCV_IRQMASK);
    __CORE_DUMP_RISCV_REG(riscvIrqdest,      NV_PRISCV_RISCV_IRQDEST);

    __CORE_DUMP_RISCV_REG(riscvPc,           NV_PRISCV_RISCV_RPC);
    __CORE_DUMP_RISCV_REG(riscvIrqdeleg,     NV_PRISCV_RISCV_IRQDELEG);
    __CORE_DUMP_RISCV_REG(riscvPrivErrStat,  NV_PRISCV_RISCV_PRIV_ERR_STAT);
    __CORE_DUMP_RISCV_REG(riscvPrivErrInfo,  NV_PRISCV_RISCV_PRIV_ERR_INFO);
    __CORE_DUMP_RISCV_REG(riscvPrivErrAddrH, NV_PRISCV_RISCV_PRIV_ERR_ADDR_HI);
    __CORE_DUMP_RISCV_REG(riscvPrivErrAddrL, NV_PRISCV_RISCV_PRIV_ERR_ADDR);
    __CORE_DUMP_RISCV_REG(riscvHubErrStat,   NV_PRISCV_RISCV_HUB_ERR_STAT);
#undef __CORE_DUMP_RISCV_REG
}

