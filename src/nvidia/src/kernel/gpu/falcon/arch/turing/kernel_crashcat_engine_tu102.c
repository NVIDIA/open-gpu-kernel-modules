/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERNEL_CRASHCAT_ENGINE_H_PRIVATE_ACCESS_ALLOWED
#include "gpu/falcon/kernel_crashcat_engine.h"
#include "gpu/gpu.h"
#include "utils/nvprintf.h"

#include "published/turing/tu102/dev_falcon_v4.h"

void kcrashcatEngineReadDmem_TU102
(
    KernelCrashCatEngine *pKernelCrashCatEng,
    NvU32 offset,
    NvU32 size,
    void *pBuf
)
{
    // This implementation only supports 32-bit-aligned accesses
    NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, (offset & (sizeof(NvU32) - 1)) == 0);
    NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, (size & (sizeof(NvU32) - 1)) == 0);

    NvU8 port = pKernelCrashCatEng->dmemPort;
    NvU32 dmemc = kcrashcatEngineMaskDmemAddr(pKernelCrashCatEng->pGpu, pKernelCrashCatEng, offset);
    dmemc = FLD_SET_DRF(_PFALCON, _FALCON_DMEMC, _AINCR, _TRUE, dmemc);
    kcrashcatEngineRegWrite(pKernelCrashCatEng->pGpu, pKernelCrashCatEng,
                            NV_PFALCON_FALCON_DMEMC(port), dmemc);

    NvU32 *pWordBuf = (NvU32 *)pBuf;
    for (NvU32 i = 0; i < (size >> 2); i++)
        pWordBuf[i] = kcrashcatEngineRegRead(pKernelCrashCatEng->pGpu, pKernelCrashCatEng,
                                             NV_PFALCON_FALCON_DMEMD(port));
}

NvU32 kcrashcatEngineGetWFL0Offset_TU102(KernelCrashCatEngine *pKernelCrashCatEng)
{
    return NV_PFALCON_FALCON_DEBUGINFO;
}

const NvU32 *kcrashcatEngineGetScratchOffsets_TU102
(
    KernelCrashCatEngine *pKernelCrashCatEng,
    NV_CRASHCAT_SCRATCH_GROUP_ID scratchId
)
{
    NV_CRASHCAT_DEF_SCRATCH_GROUP_V1_REGMAP_TABLE(scratchOffsetTable);

    if (scratchId <= NV_CRASHCAT_SCRATCH_GROUP_ID_LAST)
        return scratchOffsetTable[scratchId];

    NV_PRINTF(LEVEL_ERROR, "unknown CrashCat scratch ID %u\n", scratchId);

    return NULL;
}
