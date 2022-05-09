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

#include "gpu/sec2/kernel_sec2.h"

#include "core/core.h"
#include "gpu/eng_desc.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/gpu.h"

NV_STATUS
ksec2ConstructEngine_IMPL
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2,
    ENGDESCRIPTOR engDesc
)
{
    ksec2ConfigureFalcon_HAL(pGpu, pKernelSec2);
    return NV_OK;
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
