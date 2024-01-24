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

#include "gpu/sec2/kernel_sec2.h"

#include "core/core.h"
#include "gpu/eng_desc.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/gpu.h"
#include "gpu/fifo/kernel_fifo.h"
#include "rmapi/event.h"

NV_STATUS
ksec2ConstructEngine_IMPL
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2,
    ENGDESCRIPTOR engDesc
)
{
    return ksec2ConfigureFalcon_HAL(pGpu, pKernelSec2);
}

void
ksec2Destruct_IMPL
(
    KernelSec2 *pKernelSec2
)
{
    portMemFree((void * /* const_cast */) pKernelSec2->pGenericBlUcodeDesc);
    pKernelSec2->pGenericBlUcodeDesc = NULL;

    portMemFree((void * /* const_cast */) pKernelSec2->pGenericBlUcodeImg);
    pKernelSec2->pGenericBlUcodeImg = NULL;
}

void
ksec2RegisterIntrService_IMPL
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2,
    IntrServiceRecord pRecords[MC_ENGINE_IDX_MAX]
)
{
    KernelFalcon *pKernelFalcon = staticCast(pKernelSec2, KernelFalcon);
    NV_ASSERT_OR_RETURN_VOID(pKernelFalcon);

    // Register to handle nonstalling interrupts
    NV_ASSERT_OR_RETURN_VOID(pKernelFalcon->physEngDesc != ENG_INVALID);

    NvU32 mcIdx = MC_ENGINE_IDX_SEC2;

    NV_PRINTF(LEVEL_INFO, "Registering 0x%x/0x%x to handle SEC2 nonstall intr\n", pKernelFalcon->physEngDesc, mcIdx);

    NV_ASSERT(pRecords[mcIdx].pNotificationService == NULL);
    pRecords[mcIdx].bFifoWaiveNotify = NV_FALSE;
    pRecords[mcIdx].pNotificationService = staticCast(pKernelSec2, IntrService);

}

NV_STATUS
ksec2ServiceNotificationInterrupt_IMPL
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2,
    IntrServiceServiceNotificationInterruptArguments *pParams
)
{
    NV_PRINTF(LEVEL_INFO, "servicing nonstall intr for SEC2 engine\n");

    // Wake up channels waiting on this event
    engineNonStallIntrNotify(pGpu, RM_ENGINE_TYPE_SEC2);
    return NV_OK;
}

