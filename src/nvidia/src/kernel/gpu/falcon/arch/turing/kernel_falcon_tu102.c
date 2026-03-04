/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/falcon/kernel_falcon_core_dump.h"
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
 * Returns true if the RISC-V core is selected.
 */
NvBool
kflcnIsRiscvSelected_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    //
    // There is no explicit core select on TU10x/GA100. Use the "is the RISC-V core active" check,
    // even though:
    //  (a) this can theoretically race with the ucode switching into a Falcon mode
    //  (b) this returns a false negative if the RISC-V core has halted
    // since there is no need to support either of these cases on TU10x/GA100.
    //
    return kflcnIsRiscvActive_HAL(pGpu, pKernelFlcn);
}

/*!
 * Reset falcon using secure reset.
 * This leaves the falcon in falcon mode after reset.
 */
NV_STATUS
kflcnReset_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    NV_STATUS status;
    NV_ASSERT_OK_OR_RETURN(kflcnPreResetWait_HAL(pGpu, pKernelFlcn));
    NV_ASSERT_OK(kflcnResetHw(pGpu, pKernelFlcn));
    status = kflcnWaitForResetToFinish_HAL(pGpu, pKernelFlcn);
    NV_ASSERT_OR_RETURN((status == NV_OK) || (status == NV_ERR_GPU_IN_FULLCHIP_RESET), status);
    if (status == NV_ERR_GPU_IN_FULLCHIP_RESET)
        return status;
    kflcnSwitchToFalcon_HAL(pGpu, pKernelFlcn);
    kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_RM,
                      pGpu->chipId0);
    return NV_OK;
}

/*!
 * Reset falcon using secure reset, ready to run riscv.
 */
NV_STATUS
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
    NV_ASSERT_OK_OR_RETURN(kflcnReset_TU102(pGpu, pKernelFlcn));
    kflcnSetRiscvMode(pKernelFlcn, NV_TRUE);
    return NV_OK;
}

/*!
 * Switch the core to FALCON mode.
 */
void
kflcnSwitchToFalcon_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    //
    // Turing and GA100 do not have an explicit core switch,
    // so simply update the software state tracking the expected mode.
    //
    kflcnSetRiscvMode(pKernelFlcn, NV_FALSE);
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
 * Read the IRQ status of the Falcon.
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
    return (kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_IRQSTAT) &
            kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_IRQMASK) &
            kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_IRQDEST));
}

/*!
 * Read the IRQ status of the Falcon in RISC-V mode.
 *
 * @return IRQ status mask
 */
NvU32
kflcnRiscvReadIntrStatus_TU102
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

/*!
 * Function to read the ICD_CMD register.
 */
NvU32 kflcnIcdReadCmdReg_TU102
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
NvU64 kflcnRiscvIcdReadRdata_TU102
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
void kflcnRiscvIcdWriteAddress_TU102
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
void kflcnIcdWriteCmdReg_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 value
)
{
    kflcnRiscvRegWrite_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_ICD_CMD, value);
}

static NvBool
s_riscvIsIcdNotBusy
(
    OBJGPU *pGpu,
    void *pVoid
)
{
    KernelFalcon *pKernelFlcn = reinterpretCast(pVoid, KernelFalcon *);
    NvU32 reg;
    reg = kflcnIcdReadCmdReg_HAL(pGpu, pKernelFlcn);

    return FLD_TEST_DRF(_PRISCV_RISCV, _ICD_CMD, _BUSY, _FALSE, reg);
}

static NV_STATUS
s_riscvIcdGetValue
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU64 *pValue
)
{
    // Wait for ICD to become idle before reading out value.
    NV_STATUS status = kflcnRiscvIcdWaitForIdle_HAL(pGpu, pKernelFlcn);
    if (status == NV_OK)
    {
        *pValue = kflcnRiscvIcdReadRdata_HAL(pGpu, pKernelFlcn);
    }
    else if (status == NV_ERR_INVALID_STATE)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    else
    {
        return NV_ERR_INVALID_STATE;
    }

    return NV_OK;
}

/*!
 * Function to wait for the ICD to become idle.
 *
 * @param[in]  pGpu    OBJGPU pointer
 * @param[in]  pKernelFlcn   KernelFalcon object pointer
 *
 * @return 'NV_OK' if idle and no error
 *         'NV_ERR_INVALID_STATE' if idle and error; typically bad command.
 *         'NV_ERR_TIMEOUT' if busy and timed out. This usually indicates
 *         a fatal error, eg. core has hung or GPU is off the bus.
 */
NV_STATUS
kflcnRiscvIcdWaitForIdle_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    NvU32 icdCmd;
    RMTIMEOUT timeout;

    gpuSetTimeout(pGpu, 125*1000, &timeout, GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE); // Wait up to 125ms
    if (gpuTimeoutCondWait(pGpu, s_riscvIsIcdNotBusy, pKernelFlcn, &timeout) != NV_OK)
    {
        return NV_ERR_TIMEOUT;
    }

    icdCmd = kflcnIcdReadCmdReg_HAL(pGpu, pKernelFlcn);

    if (FLD_TEST_DRF(_PRISCV_RISCV, _ICD_CMD, _ERROR, _TRUE, icdCmd))
    {
        return NV_ERR_INVALID_STATE;
    }

    return NV_OK;
}

/*!
 * Function to tell RISCV ICD to read RISCV virtual addresses.
 *
 * @param[in]  pGpu    OBJGPU pointer
 * @param[in]  pKernelFlcn   KernelFalcon object pointer
 * @param[in]  address Address of memory to read.
 * @param[in]  size    Size of access (1-8 bytes, pow2)
 * @param[out] pValue  register value
 *
 * @return 'NV_OK' if register value was read
 *         'NV_ERR_INVALID_STATE' if core is not booted or didn't halt.
 *         'NV_ERR_INVALID_ARGUMENT' if size is invalid
 */
NV_STATUS
kflcnRiscvIcdReadMem_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU64 address,
    NvU64 size,
    NvU64 *pValue
)
{
    NvU32 icdCmd;

    // Only pow2 sizes are allowed
    if ((size != 1) && (size != 2) && (size != 4) && (size != 8))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    if ((address & (size - 1))) // Addresses must be aligned to the size. This is a RISCV architecture design decision.
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    NvU32 size_shift = 0;
    while (size != 1)
    {
        size = size >> 1;
        size_shift++;
    }

    if (kflcnIsRiscvActive_HAL(pGpu, pKernelFlcn) &&
        (kflcnRiscvIcdWaitForIdle_HAL(pGpu, pKernelFlcn) != NV_ERR_TIMEOUT))
    {
        icdCmd = DRF_DEF(_PRISCV_RISCV, _ICD_CMD, _OPC, _RDM);
        icdCmd = FLD_SET_DRF_NUM(_PRISCV_RISCV, _ICD_CMD, _SZ, size_shift, icdCmd);
        icdCmd = FLD_SET_DRF_NUM(_PRISCV_RISCV, _ICD_CMD, _PARM, 1, icdCmd);

        kflcnRiscvIcdWriteAddress_HAL(pGpu, pKernelFlcn, address);

        kflcnIcdWriteCmdReg_HAL(pGpu, pKernelFlcn, icdCmd);
    }
    else
    {
        // RISCV core was not booted, or ICD failed to execute command.
        return NV_ERR_INVALID_STATE;
    }

    return s_riscvIcdGetValue(pGpu, pKernelFlcn, pValue);
}

/*!
 * Function to tell RISCV ICD to read RISCV register.
 *
 * @param[in]  pGpu    OBJGPU pointer
 * @param[in]  pKernelFlcn   KernelFalcon object pointer
 * @param[in]  reg     which register to read. Valid: 0-31 (0 is x0, so it is skipped)
 * @param[out] pValue  register value
 *
 * @return 'NV_OK' if register value was read
 *         'NV_ERR_INVALID_STATE' if core is not booted or didn't halt.
 *         'NV_ERR_INVALID_ARGUMENT' if register is invalid.
 */
NV_STATUS
kflcnRiscvIcdReadReg_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 reg,
    NvU64 *pValue
)
{
    NvU32 icdCmd;

    // x0..x31 are valid RISCV register values.
    if (reg >= 32)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (reg == 0)
    {
        *pValue = 0;
        return NV_OK;
    }

    if (kflcnIsRiscvActive_HAL(pGpu, pKernelFlcn) &&
        (kflcnRiscvIcdWaitForIdle_HAL(pGpu, pKernelFlcn) != NV_ERR_TIMEOUT))
    {
        icdCmd = DRF_DEF(_PRISCV_RISCV, _ICD_CMD, _OPC, _RREG);
        icdCmd = FLD_SET_DRF_NUM(_PRISCV_RISCV, _ICD_CMD, _IDX, reg, icdCmd);

        kflcnIcdWriteCmdReg_HAL(pGpu, pKernelFlcn, icdCmd);
    }
    else
    {
        // RISCV core was not booted, or ICD failed to execute command.
        return NV_ERR_INVALID_STATE;
    }

    return s_riscvIcdGetValue(pGpu, pKernelFlcn, pValue);
}

/*!
 * Function to tell RISCV ICD to read RISCV CSR.
 *
 * @param[in]  pGpu    OBJGPU pointer
 * @param[in]  pKernelFlcn   KernelFalcon object pointer
 * @param[in]  csr     which CSR register to read. Valid: 0-4095
 * @param[out] pValue  CSR register value
 *
 * @return 'NV_OK' if CSR value was read
 *         'NV_ERR_INVALID_STATE' if core is not booted or didn't halt.
 *         'NV_ERR_INVALID_ARGUMENT' if CSR is invalid.
 */
NV_STATUS
kflcnRiscvIcdRcsr_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 csr,
    NvU64 *pValue
)
{
    NvU32 icdCmd;

    // CSR must be between 0 and 4095, inclusive, as this is part of the RISCV spec.
    if (csr >= 4096)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (kflcnIsRiscvActive_HAL(pGpu, pKernelFlcn) &&
        (kflcnRiscvIcdWaitForIdle_HAL(pGpu, pKernelFlcn) != NV_ERR_TIMEOUT))
    {
        icdCmd = DRF_DEF(_PRISCV_RISCV, _ICD_CMD, _OPC, _RCSR);
        icdCmd = FLD_SET_DRF_NUM(_PRISCV_RISCV, _ICD_CMD, _PARM, csr, icdCmd);

        kflcnIcdWriteCmdReg_HAL(pGpu, pKernelFlcn, icdCmd);
    }
    else
    {
        // RISCV core was not booted, or ICD failed to read CSR.
        return NV_ERR_INVALID_STATE;
    }

    return s_riscvIcdGetValue(pGpu, pKernelFlcn, pValue);
}

/*!
 * Function to tell RISCV ICD to read RSTAT register.
 *
 * @param[in]  pGpu    OBJGPU pointer
 * @param[in]  pKernelFlcn   KernelFalcon object pointer
 * @param[in]  index   which RSTAT register to read. Valid: 0 3 4
 * @param[out] pValue  RSTAT register value
 *
 * @return 'NV_OK' if RSTAT value was read
 *         'NV_ERR_INVALID_STATE' if core is not booted or didn't halt.
 *         'NV_ERR_INVALID_ARGUMENT' if invalid RSTAT register was specified.
 */
NV_STATUS
kflcnRiscvIcdRstat_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 index,
    NvU64 *pValue
)
{
    NvU32 icdCmd;

    if (kflcnIsRiscvActive_HAL(pGpu, pKernelFlcn) &&
        (kflcnRiscvIcdWaitForIdle_HAL(pGpu, pKernelFlcn) != NV_ERR_TIMEOUT))
    {
        icdCmd = DRF_DEF(_PRISCV_RISCV, _ICD_CMD, _OPC, _RSTAT);
        icdCmd = FLD_SET_DRF_NUM(_PRISCV_RISCV, _ICD_CMD, _IDX, index, icdCmd);

        kflcnIcdWriteCmdReg_HAL(pGpu, pKernelFlcn, icdCmd);
    }
    else
    {
        // RISCV core was not booted, or ICD misbehaved.
        return NV_ERR_INVALID_STATE;
    }

    return s_riscvIcdGetValue(pGpu, pKernelFlcn, pValue);
}

/*!
 * Function to tell RISCV ICD to read PC.
 *
 * @param[in]  pGpu    OBJGPU pointer
 * @param[in]  pKernelFlcn   KernelFalcon object pointer
 * @param[out] pValue  PC value
 *
 * @return 'NV_OK' if RSTAT value was read
 *         'NV_ERR_INVALID_STATE' if core is not booted or didn't halt.
 *         'NV_ERR_INVALID_ARGUMENT' should not happen.
 */
NV_STATUS
kflcnRiscvIcdRpc_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU64 *pValue
)
{
    NvU32 icdCmd;

    if (kflcnIsRiscvActive_HAL(pGpu, pKernelFlcn) &&
        (kflcnRiscvIcdWaitForIdle_HAL(pGpu, pKernelFlcn) != NV_ERR_TIMEOUT))
    {
        icdCmd = DRF_DEF(_PRISCV_RISCV, _ICD_CMD, _OPC, _RPC);
        kflcnIcdWriteCmdReg_HAL(pGpu, pKernelFlcn, icdCmd);
    }
    else
    {
        // RISCV core was not booted, or ICD failed to retrieve PC.
        return NV_ERR_INVALID_STATE;
    }

    return s_riscvIcdGetValue(pGpu, pKernelFlcn, pValue);
}

/*!
 * Function to tell RISCV core to enter ICD mode.
 *
 * @param[in]  pGpu    OBJGPU pointer
 * @param[in]  pKernelFlcn   KernelFalcon object pointer
 *
 * @return 'NV_OK' if core has entered ICD
 *         'NV_ERR_INVALID_STATE' if core is not booted.
 *         'NV_ERR_TIMEOUT' if core did not successfully halt.
 */
NV_STATUS
kflcnRiscvIcdHalt_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    NV_STATUS status = NV_OK;
    NvU32 icdCmd;
    NvU8 tries = 9; // This should be set to allow retries for over a second.

    if (kflcnIsRiscvActive_HAL(pGpu, pKernelFlcn))
    {
        do
        {
            icdCmd = DRF_DEF(_PRISCV_RISCV, _ICD_CMD, _OPC, _STOP);
            kflcnIcdWriteCmdReg_HAL(pGpu, pKernelFlcn, icdCmd);
            status = kflcnRiscvIcdWaitForIdle_HAL(pGpu, pKernelFlcn);
            if (tries == 0)
                break;
            tries--;
        }
        while (status != NV_OK);
    }
    else // RISCV core was not booted; die immediately.
    {
        return NV_ERR_INVALID_STATE;
    }

    return status;
}

void
kflcnDumpTracepc_TU102
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

void
kflcnDumpCoreRegs_TU102(OBJGPU *pGpu, KernelFalcon *pKernelFlcn, CoreDumpRegs *pCore)
{
#define __CORE_DUMP_RISCV_REG(x,y) do { pCore->x = kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, (y)); } while (0)
    __CORE_DUMP_RISCV_REG(riscvCpuctl,       NV_PRISCV_RISCV_CPUCTL);
    __CORE_DUMP_RISCV_REG(riscvIrqmask,      NV_PRISCV_RISCV_IRQMASK);
    __CORE_DUMP_RISCV_REG(riscvIrqdest,      NV_PRISCV_RISCV_IRQDEST);
    __CORE_DUMP_RISCV_REG(riscvPrivErrStat,  NV_PRISCV_RISCV_PRIV_ERR_STAT);
    __CORE_DUMP_RISCV_REG(riscvPrivErrInfo,  NV_PRISCV_RISCV_PRIV_ERR_INFO);
    __CORE_DUMP_RISCV_REG(riscvPrivErrAddrL, NV_PRISCV_RISCV_PRIV_ERR_ADDR);
    __CORE_DUMP_RISCV_REG(riscvHubErrStat,   NV_PRISCV_RISCV_HUB_ERR_STAT);
#undef __CORE_DUMP_RISCV_REG
}

void
kflcnDumpPeripheralRegs_TU102(OBJGPU *pGpu, KernelFalcon *pKernelFlcn, CoreDumpRegs *pCore)
{
#define __CORE_DUMP_REG(x,y) do { pCore->x = kflcnRegRead_HAL(pGpu, pKernelFlcn, (y)); } while (0)
    __CORE_DUMP_REG(falconMailbox[0], NV_PFALCON_FALCON_MAILBOX0);
    __CORE_DUMP_REG(falconMailbox[1], NV_PFALCON_FALCON_MAILBOX1);
    __CORE_DUMP_REG(falconIrqstat,    NV_PFALCON_FALCON_IRQSTAT);
    __CORE_DUMP_REG(falconIrqmode,    NV_PFALCON_FALCON_IRQMODE);
#undef __CORE_DUMP_REG

#define __CORE_DUMP_RAW(x,y) do { pCore->x = GPU_REG_RD32(pGpu, (y)); } while (0)
    __CORE_DUMP_RAW(fbifInstblk,           pKernelFlcn->fbifBase + NV_PFALCON_FBIF_INSTBLK);
    __CORE_DUMP_RAW(fbifCtl,               pKernelFlcn->fbifBase + NV_PFALCON_FBIF_CTL);
    __CORE_DUMP_RAW(fbifThrottle,          pKernelFlcn->fbifBase + NV_PFALCON_FBIF_THROTTLE);
    __CORE_DUMP_RAW(fbifAchkBlk[0],        pKernelFlcn->fbifBase + NV_PFALCON_FBIF_ACHK_BLK(0));
    __CORE_DUMP_RAW(fbifAchkBlk[1],        pKernelFlcn->fbifBase + NV_PFALCON_FBIF_ACHK_BLK(1));
    __CORE_DUMP_RAW(fbifAchkCtl[0],        pKernelFlcn->fbifBase + NV_PFALCON_FBIF_ACHK_CTL(0));
    __CORE_DUMP_RAW(fbifAchkCtl[1],        pKernelFlcn->fbifBase + NV_PFALCON_FBIF_ACHK_CTL(1));
    __CORE_DUMP_RAW(fbifCg1,               pKernelFlcn->fbifBase + NV_PFALCON_FBIF_CG1);
#undef __CORE_DUMP_RAW
}

