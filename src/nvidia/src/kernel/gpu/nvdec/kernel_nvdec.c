/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/nvdec/kernel_nvdec.h"

#include "core/core.h"
#include "gpu/eng_desc.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/gpu.h"

NV_STATUS
knvdecConstructEngine_IMPL
(
    OBJGPU *pGpu,
    KernelNvdec *pKernelNvdec,
    ENGDESCRIPTOR engDesc
)
{
    if (knvdecIsEngineDisabled_HAL(pGpu, pKernelNvdec))
    {
        // fail construction of KernelNvdec if NVDEC0 engine is missing in HW
        return NV_ERR_NOT_SUPPORTED;
    }

    knvdecConfigureFalcon_HAL(pGpu, pKernelNvdec);
    return NV_OK;
}

void knvdecRegisterIntrService_IMPL(OBJGPU *pGpu, KernelNvdec *pKernelNvdec, IntrServiceRecord pRecords[MC_ENGINE_IDX_MAX])
{
    KernelFalcon *pKernelFalcon = staticCast(pKernelNvdec, KernelFalcon);
    NV_ASSERT_OR_RETURN_VOID(pKernelFalcon);

    NV_PRINTF(LEVEL_INFO, "physEngDesc 0x%x\n", pKernelFalcon->physEngDesc);

    NV_ASSERT(pKernelFalcon->physEngDesc == ENG_NVDEC(0));

    NvU32 mcIdx = MC_ENGINE_IDX_NVDEC0;

    NV_ASSERT(pRecords[mcIdx].pNotificationService == NULL);
    pRecords[mcIdx].bFifoWaiveNotify = NV_FALSE;
    pRecords[mcIdx].pNotificationService = staticCast(pKernelNvdec, IntrService);
}

NV_STATUS knvdecServiceNotificationInterrupt_IMPL(OBJGPU *pGpu, KernelNvdec *pKernelNvdec, IntrServiceServiceNotificationInterruptArguments *pParams)
{
    NvU32 idxMc = pParams->engineIdx;
    NvU32 idx2080 = NV2080_ENGINE_TYPE_NVDEC0;

    NV_PRINTF(LEVEL_INFO, "nonstall intr for MC 0x%x\n", idxMc);

    NV_ASSERT_OR_RETURN(idxMc == MC_ENGINE_IDX_NVDEC0, NV_ERR_INVALID_STATE);

    knvdecNonstallIntrCheckAndClear_HAL(pGpu, pKernelNvdec, pParams->pThreadState);

    // Wake up channels waiting on this event
    engineNonStallIntrNotify(pGpu, idx2080);

    return NV_OK;
}
