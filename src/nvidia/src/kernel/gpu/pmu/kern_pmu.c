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

/****************************************************************************
*
*   Kernel Pmu Module
*   This file contains functions managing PMU core on CPU RM
*
****************************************************************************/

#include "rmconfig.h"
#include "gpu/pmu/kern_pmu.h"
#include "gpu/fsp/kern_fsp.h"
#include "gpu/mem_mgr/mem_mgr.h"

#include "nvrm_registry.h"
#include "kernel/os/os.h"

#include "ctrl/ctrl0080/ctrl0080gpu.h"

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
}

void
kpmuDestruct_IMPL(KernelPmu *pKernelPmu)
{
    OBJGPU *pGpu = ENG_GET_GPU(pKernelPmu);

    kpmuFreeLibosLoggingStructures(pGpu, pKernelPmu);
}

NvU32
kpmuReservedMemorySizeGet_IMPL
(
    KernelPmu *pKernelPmu
)
{
    return NV_ALIGN_UP64(
            kpmuReservedMemoryBackingStoreSizeGet(pKernelPmu) +
                kpmuReservedMemorySurfacesSizeGet(pKernelPmu) +
                kpmuReservedMemoryMiscSizeGet(pKernelPmu),
            KPMU_RESERVED_MEMORY_ALIGNMENT);
}

NvU32
kpmuReservedMemoryBackingStoreSizeGet_IMPL
(
    KernelPmu *pKernelPmu
)
{
    if (kpmuGetIsSelfInit(pKernelPmu))
    {
        //
        // MMINTS-TODO: cross-reference with this when reserving memory
        // in pmu_20.c
        //
        return 0x800000;
    }

    return 0U;
}

NvU32
kpmuReservedMemorySurfacesSizeGet_IMPL
(
    KernelPmu *pKernelPmu
)
{
    if (kpmuGetIsSelfInit(pKernelPmu))
    {
        return PMU_RESERVED_MEMORY_SURFACES_SIZE;
    }

    return 0U;
}

NvU32
kpmuReservedMemoryMiscSizeGet_IMPL
(
    KernelPmu *pKernelPmu
)
{
    if (kpmuGetIsSelfInit(pKernelPmu))
    {
        return (4U * (1U << 10U));
    }

    return 0U;
}

NvU64 kpmuReservedMemoryOffsetGet_IMPL
(
    OBJGPU *pGpu,
    KernelPmu *pKernelPmu
)
{
    if (kpmuGetIsSelfInit(pKernelPmu))
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
        const NvU64 fbTotalMemSize = (pMemoryManager->Ram.fbTotalMemSizeMb << 20U);

        KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);

        if ((pKernelFsp != NULL) && !pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_FRTS_VIDMEM))
        {
            // This value should have been populated already
            NV_ASSERT(pKernelFsp->pCotPayload->frtsVidmemOffset > 0U);

            //
            // Note: frtsVidmemOffset is an offset of the end of FRTS from the
            // end of FB. If FRTS is enabled and placed in FB (in WPR2) by FSP,
            // we want to directly use its offset to place PMU, since the offset
            // calculation is not always straightforward.
            // Note that we ensure, in kfspPrepareBootCommands_HAL, that we have
            // enough space to place PMU after FRTS, so this FRTS offset should
            // definitely have that accounted for.
            //
            // Note that we have to add back in the ExtraReservedMemorySize
            // because the FSP allocation may extend beyond where the
            // frtsVidmemOffset claims it stops.
            //
            const NvU64 pmuRsvdOffset = fbTotalMemSize -
                pKernelFsp->pCotPayload->frtsVidmemOffset +
                kfspGetExtraReservedMemorySize_HAL(pGpu, pKernelFsp);
            return pmuRsvdOffset;
        }
        else
        {
            const NvU64 pmuRsvdOffset = fbTotalMemSize -
                memmgrGetFBEndReserveSizeEstimate_HAL(pGpu, pMemoryManager) -
                kpmuReservedMemorySizeGet(pKernelPmu);
            return pmuRsvdOffset;
        }
    }
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
                     "NVRISCV", NULL, 0, NULL);

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
