/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/****************************************************************************
*
*   Kernel Pmu Module
*   This file contains functions managing PMU core on CPU RM
*
****************************************************************************/

#include "gpu/pmu/kern_pmu.h"
#include "gpu/mem_mgr/mem_mgr.h"

NV_STATUS
kpmuConstructEngine_IMPL(OBJGPU *pGpu, KernelPmu *pKernelPmu, ENGDESCRIPTOR engDesc)
{

    return kpmuInitLibosLoggingStructures(pGpu, pKernelPmu);
}

void
kpmuStateDestroy_IMPL
(
    OBJGPU *pGpu,
    KernelPmu *pKernelPmu
)
{
    if (pKernelPmu->pPmuRsvdMemdesc != NULL)
    {
        memdescFree(pKernelPmu->pPmuRsvdMemdesc);
        memdescDestroy(pKernelPmu->pPmuRsvdMemdesc);
        pKernelPmu->pPmuRsvdMemdesc = NULL;
    }
}

void
kpmuDestruct_IMPL(KernelPmu *pKernelPmu)
{
    OBJGPU *pGpu = ENG_GET_GPU(pKernelPmu);

    kpmuFreeLibosLoggingStructures(pGpu, pKernelPmu);
}

NvU32 kpmuReservedMemorySizeGet_IMPL
(
    KernelPmu *pKernelPmu
)
{
    return 0U;
}

NvU64 kpmuReservedMemoryOffsetGet_IMPL
(
    OBJGPU *pGpu,
    KernelPmu *pKernelPmu
)
{
    return 0U;
}


/*!
 * Init libos PMU logging
 */
NV_STATUS
kpmuInitLibosLoggingStructures_IMPL
(
    OBJGPU *pGpu,
    KernelPmu *pKernelPmu
)
{
    NV_STATUS nvStatus = NV_OK;

    if (!IS_GSP_CLIENT(pGpu))
    {
        return NV_OK;
    }

    NV_ASSERT_OR_RETURN(pKernelPmu->pPrintBuf == NULL, NV_ERR_INVALID_STATE);

    pKernelPmu->pLogElf = NULL;

    // Allocate print buffer
    pKernelPmu->printBufSize = PMU_LOG_BUFFER_MAX_SIZE;
    pKernelPmu->pPrintBuf = portMemAllocNonPaged(pKernelPmu->printBufSize);

    // Create PMU log
    libosLogCreateEx(&pKernelPmu->logDecode, "PMU");

    // Add PMU log buffer (use a fake "task name" - NVRISCV)
    libosLogAddLogEx(&pKernelPmu->logDecode, pKernelPmu->pPrintBuf, pKernelPmu->printBufSize,
                     pGpu->gpuInstance, (gpuGetChipArch(pGpu) >> GPU_ARCH_SHIFT), gpuGetChipImpl(pGpu),
                     "NVRISCV", NULL);

    // Finish PMU log init (setting the lossless-print flag and resolve-pointers flag)
    libosLogInitEx(&pKernelPmu->logDecode, pKernelPmu->pLogElf, NV_TRUE, NV_TRUE, NV_TRUE, pKernelPmu->logElfSize);

    if (nvStatus != NV_OK)
        kpmuFreeLibosLoggingStructures(pGpu, pKernelPmu);
    return nvStatus;
}

/*!
 * Free libos PMU logging
 */
void
kpmuFreeLibosLoggingStructures_IMPL
(
    OBJGPU *pGpu,
    KernelPmu *pKernelPmu
)
{
    if (!IS_GSP_CLIENT(pGpu))
    {
        return;
    }

    // Destroy PMU log
    libosLogDestroy(&pKernelPmu->logDecode);
    portMemFree(pKernelPmu->pPrintBuf);
    pKernelPmu->pPrintBuf = NULL;
    portMemFree(pKernelPmu->pLogElf);
    pKernelPmu->pLogElf = NULL;
}

/*!
 * Log a PMU libos log buffer coming from GSP-RM
 */
void
kpmuLogBuf_IMPL
(
    OBJGPU *pGpu,
    KernelPmu *pKernelPmu,
    NvU8 *pBuf,
    NvU32 bufSize
)
{
    NV_ASSERT_OR_RETURN_VOID(pKernelPmu->pPrintBuf != NULL);

    portMemCopy(pKernelPmu->pPrintBuf, pKernelPmu->printBufSize,
                pBuf, bufSize);
    libosExtractLogs(&pKernelPmu->logDecode, NV_FALSE);
}

NV_STATUS kpmuStateInitLocked_IMPL
(
    OBJGPU *pGpu,
    KernelPmu *pKernelPmu
)
{

    return NV_OK;
}
