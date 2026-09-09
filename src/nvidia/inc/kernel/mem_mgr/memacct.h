 /*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef MEMORY_ACCOUNTING_H
#define MEMORY_ACCOUNTING_H

#include "core/core.h"
#include "os/os.h"
#include "rmapi/client.h"

//
// Opaque type representing a single "charge" of memory. which is essentially
// a voucher that allows you to make a memory allocation.
// The allocation may still fail for various reasons, this simply certifies that
// you are within your allotted quotas.
//
typedef struct MemoryCharge MemoryCharge;

NV_STATUS memacctTryCharge(RmClient *pRmClient, NvU32 gpuId, NvLength size, MemoryCharge **ppCharge);
void memacctIncrementChargeRefCount(MemoryCharge *pCharge);
void memacctReleaseCharge(MemoryCharge *pCharge);

NV_STATUS memacctSetLimits(ClientGroupID cligrp, NvU32 gpuId, NvLength softlimit, NvLength hardlimit);
NV_STATUS memacctGetLimits(ClientGroupID cligrp, NvU32 gpuId, NvLength *pSoftLimit, NvLength *pHardLimit, NvLength *pCurrent);

// Initialize the accounting for a given pGpu
NV_STATUS memacctInitGpuInfo(OBJGPU *pGpu);
void memacctRemoveGpu(OBJGPU *pGpu);
NvCgroupImpl memacctActiveImplementation(void);

#endif
