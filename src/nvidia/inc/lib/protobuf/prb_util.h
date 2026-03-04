/*
 * SPDX-FileCopyrightText: Copyright (c) 2000-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef PRB_UTIL_H
#define PRB_UTIL_H

#include "kernel/diagnostics/journal_structs.h"
#include "kernel/gpu/gpu.h"

#include "lib/protobuf/prb.h"
#include "nvstatus.h"
#include "nvtypes.h"

typedef struct
{
    NvU32 offset;
    NvU32 numRegs;
    NvU32 stride;           // Ignored if numRegs = 1
} PRB_GPU_REG_TABLE;

// Helper macro to help construct PRB_GPU_REG_TABLE entries from indexed registers.
#define PRB_GPU_REG_TBL_ENTRY(r) { r(0), r##__SIZE_1, r(1) - r(0) }

// Helper macro to help construct PRB_GPU_REG_TABLE entries from indexed registers that are offset from a base register
#define PRB_GPU_REG_TBL_ENTRY_OFFSET(r, base) { r(0) + (base), r##__SIZE_1, r(1) - r(0) }

typedef struct
{
    NvU32 offset;
    NvU32 numRegs;
    NvU32 stride;
    NvU32 ilen;
} PRB_GPU_REG_INDEXED_TABLE;

// Helper macro to help construct PRB_GPU_REG_INDEXED_TABLE entries
#define PRB_GRI_TBL_ENTRY(r, n, s) { r(0), n, s, r(1) - r(0) }

// Macro that always returns 1.
//
// Used to mcheck registers and count them at the same time.  Useful for passing
// counts into prbEncGpuRegs, etc.  Note that this macro does NOT assert that
// the registers are adjacent.
//
#define PRB_COUNT_REG(r) (0?(r):1)

//
// Worst case overhead per regs and mem message.
// Includes everything but the actual register to allow calculations
// for packed structures.
//
#define PRB_REGS_MSG_OVERHEAD 13

NV_STATUS prbEncGpuRegs(OBJGPU *pGpu, IoAperture *pAperture, NvU64 offset, NvU32 numEntries, PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc);
NV_STATUS prbEncGpuRegImm(OBJGPU *pGpu, NvU64 offset, NvU32 reg, PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc);
NV_STATUS prbEncGpuRegOffset(OBJGPU *pGpu, IoAperture *pAperture, const NvU32 *pOffset, NvU32 numEntries, PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc);
NV_STATUS prbEncGpuRegSliceOffset(OBJGPU *pGpu, IoAperture *pAperture, const NvU32 *pOffset, NvU32 numEntries, NvU32 base, PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc);
NV_STATUS prbEncGpuRegTbl(OBJGPU *pGpu, IoAperture *pAperture, const PRB_GPU_REG_TABLE *pTbl, NvU32 numEntries, PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc);
NV_STATUS prbEncGpuRegSliceTbl(OBJGPU *pGpu, IoAperture *pAperture, const PRB_GPU_REG_TABLE *pTbl, NvU32 numEntries, NvU32 base, PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc);
NV_STATUS prbEncGpuRegSliceIndexedTbl(OBJGPU *pGpu, IoAperture *pAperture, const PRB_GPU_REG_INDEXED_TABLE *pTbl, NvU32 numEntries, NvU32 base, NvU32 index, PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc);
NV_STATUS prbEncMem(NvU64 offset, NvU32 numRegs, PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc);
NV_STATUS prbEncInstMem(NvU64 physAddr, NvU8 *pVirtAddr, NvU32 numWords, PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc);
NV_STATUS prbEncInstMemImm(NvU64 physAddr, NvU32 data, PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc);
NV_STATUS prbEncPciConfigRegs(OBJGPU *pGpu, NvU64 index, NvU32 numRegs, PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc);
NV_STATUS prbEncPciReadDword(void *handle, NvU64 offset, PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc);
NV_STATUS prbEncGenExData(PRB_ENCODER *pPrbEnc, OBJGPU *pGpu, NvU32 chId, const PRB_FIELD_DESC *fieldDesc);
NV_STATUS prbWrapAndQueue(PRB_ENCODER *pPrbEnc, RMERRORHEADER *pErrorHeader, RMCD_ERROR_BLOCK **);
NV_STATUS prbSetupDclMsg(PRB_ENCODER *pPrbEnc, NvU32 len, const PRB_FIELD_DESC *fieldDesc);

NV_STATUS prbEncUnwindNesting(PRB_ENCODER *pPrbEnc, NvU8 level);
static NV_FORCEINLINE NvU8 prbEncNestingLevel(PRB_ENCODER *pPrbEnc)  { return pPrbEnc->depth; }

#endif // PRB_UTIL_H
