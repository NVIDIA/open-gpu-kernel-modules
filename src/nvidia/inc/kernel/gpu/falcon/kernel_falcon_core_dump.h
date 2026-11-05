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

#ifndef KERNEL_FALCON_CORE_DUMP_H
#define KERNEL_FALCON_CORE_DUMP_H

#include "nvtypes.h"

#define __RISCV_MAX_TRACE_ENTRIES 64
#define KERNEL_FALCON_RISCV_NUM_RSTAT_REGS 8
#define KERNEL_FALCON_RISCV_NUM_CORE_REGS 32
#define __RISCV_MAX_UNWIND_DEPTH 32
#define KERNEL_FALCON_RISCV_MAX_CSR_DUMP_ENTRIES 8

typedef struct CoreDumpRegs
{
    NvU32 riscvPc;
    NvU32 riscvCpuctl;
    NvU32 riscvIrqmask;
    NvU32 riscvIrqdest;
    NvU32 riscvIrqdeleg;
    NvU32 falconMailbox[2];
    NvU32 falconIrqstat;
    NvU32 falconIrqmode;
    NvU32 fbifInstblk;
    NvU32 fbifCtl;
    NvU32 fbifThrottle;
    NvU32 fbifAchkBlk[2];
    NvU32 fbifAchkCtl[2];
    NvU32 fbifCg1;
    // Ampere and above
    NvU32 riscvPrivErrStat;
    NvU32 riscvPrivErrInfo;
    NvU32 riscvPrivErrAddrH;
    NvU32 riscvPrivErrAddrL;
    NvU32 riscvHubErrStat;
    NvU32 tracePCEntries;
    NvU64 tracePC[__RISCV_MAX_TRACE_ENTRIES];
} CoreDumpRegs;

typedef struct DestructiveCoreDumpRegs
{
    NvBool bCoreBooted;
    NvU32 rstatSuccessCount;
    NvU32 rstatSuccessMask;
    NvU64 rstat[KERNEL_FALCON_RISCV_NUM_RSTAT_REGS];
    NvBool bHaltSucceeded;
    NvBool bPcValid;
    NvBool bPcFallback;
    NvU64 pc;
    NvU32 gprValidMask;
    NvU64 gpr[KERNEL_FALCON_RISCV_NUM_CORE_REGS];
    NvU32 csrReadCount;
    NvBool bCsrTruncated;
    NvU32 csrCount;
    struct
    {
        NvU32 index;
        NvU64 value;
    } csr[KERNEL_FALCON_RISCV_MAX_CSR_DUMP_ENTRIES];
    NvU32 unwindDepth;
    NvU64 unwindPC[__RISCV_MAX_UNWIND_DEPTH];
} DestructiveCoreDumpRegs;

#endif
