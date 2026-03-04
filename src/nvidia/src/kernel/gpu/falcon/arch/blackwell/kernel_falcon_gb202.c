/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/falcon/kernel_falcon_core_dump.h"

#include "published/blackwell/gb202/dev_riscv_pri.h"

void
kflcnDumpCoreRegs_GB202(OBJGPU *pGpu, KernelFalcon *pKernelFlcn, CoreDumpRegs *pCore)
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