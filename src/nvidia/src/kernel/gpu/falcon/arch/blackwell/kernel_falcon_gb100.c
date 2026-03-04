/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * Provides the implementation for all GB100+ specific KernelFalcon
 * interfaces.
 */

#include "kernel/gpu/gpu.h"
#include "gpu/falcon/kernel_falcon.h"

#include "published/blackwell/gb100/dev_falcon_v4.h"
#include "published/blackwell/gb100/dev_riscv_pri.h"

/*!
 * Helper function to get IRQSTAT ECC bit
 *
 * @return FALCON_IRQSTAT_EXT_EXTIRQ7 bit set
 */
NvU32
kflcnGetEccInterruptMask_GB100
(
    OBJGPU       *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    return REF_DEF(NV_PFALCON_FALCON_IRQSTAT_EXT_EXTIRQ7, _TRUE);
}

/*!
 * Read and return the value in NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT
 *
 * @return NV_OK on success
 *         NV_ERR_INVALID_READ if the register read returns a PRI error
 */
NV_STATUS
kflcnGetFatalHwErrorStatus_GB100
(
    OBJGPU       *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32        *pErrorStatus
)
{
    NvU32 errorStatus;

    NV_ASSERT_OR_RETURN(pErrorStatus != NULL, NV_ERR_INVALID_ARGUMENT);

    errorStatus = kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT);

    if ((errorStatus & GPU_READ_PRI_ERROR_MASK) == GPU_READ_PRI_ERROR_CODE)
    {
        NV_PRINTF(LEVEL_ERROR, "Cannot read NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT (0x%x)\n", errorStatus);
        return NV_ERR_INVALID_READ;
    }

    *pErrorStatus = errorStatus;
    return NV_OK;
}

/*!
 * Return a string for each field in NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT
 */
const char *
kflcnFatalHwErrorCodeToString_GB100
(
    OBJGPU       *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32         errorCode,
    NvBool        bNvPrintfStr
)
{
    NV_ASSERT_OR_RETURN(ONEBITSET(errorCode), bNvPrintfStr ? MAKE_NV_PRINTF_STR("Unknown") : "Unknown");

#define RISCV_FAULT_CONTAINMENT_STR_CASE(drf, str)                  \
    case DRF_BASE(NV_PRISCV_RISCV_FAULT_CONTAINMENT_SRCSTAT_##drf): \
        return bNvPrintfStr ? MAKE_NV_PRINTF_STR(str) : str

    switch (BIT_IDX_32(errorCode))
    {
        RISCV_FAULT_CONTAINMENT_STR_CASE(GLOBAL_MEM, "FB Poison");
        RISCV_FAULT_CONTAINMENT_STR_CASE(ROM, "BROM ECC");
        RISCV_FAULT_CONTAINMENT_STR_CASE(ITCM, "ITCM ECC");
        RISCV_FAULT_CONTAINMENT_STR_CASE(DTCM, "DTCM ECC");
        RISCV_FAULT_CONTAINMENT_STR_CASE(ICACHE, "ICACHE ECC");
        RISCV_FAULT_CONTAINMENT_STR_CASE(DCACHE, "DCACHE ECC");
        RISCV_FAULT_CONTAINMENT_STR_CASE(RVCORE, "RISCV Delayed Lockstep");
        RISCV_FAULT_CONTAINMENT_STR_CASE(REG, "TKE Register ECC");
        RISCV_FAULT_CONTAINMENT_STR_CASE(SE_LOGIC, "SE Logic");
        RISCV_FAULT_CONTAINMENT_STR_CASE(SE_KSLT, "SE Keyslot ECC");
        RISCV_FAULT_CONTAINMENT_STR_CASE(TKE, "TKE Watchdog Timeout");
        RISCV_FAULT_CONTAINMENT_STR_CASE(FBIF, "FBIF ECC");
        RISCV_FAULT_CONTAINMENT_STR_CASE(MPURAM, "MPU RAM ECC");
        RISCV_FAULT_CONTAINMENT_STR_CASE(ENGINE_IN0, "Engine Fault 0");
        RISCV_FAULT_CONTAINMENT_STR_CASE(ENGINE_IN1, "Engine Fault 1");
        RISCV_FAULT_CONTAINMENT_STR_CASE(ENGINE_IN2, "Engine Fault 2");
        RISCV_FAULT_CONTAINMENT_STR_CASE(ENGINE_IN3, "Engine Fault 3");
        RISCV_FAULT_CONTAINMENT_STR_CASE(ENGINE_IN4, "Engine Fault 4");
        RISCV_FAULT_CONTAINMENT_STR_CASE(ENGINE_IN5, "Engine Fault 5");
        RISCV_FAULT_CONTAINMENT_STR_CASE(ENGINE_IN6, "Engine Fault 6");
        RISCV_FAULT_CONTAINMENT_STR_CASE(ENGINE_IN7, "Engine Fault 7");
        default:
            return (bNvPrintfStr ? MAKE_NV_PRINTF_STR("Unknown") : "Unknown");
    }

#undef RISCV_FAULT_CONTAINMENT_STR_CASE
}
