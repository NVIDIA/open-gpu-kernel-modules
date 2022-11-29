/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "gpu/gpu.h"
#include "gpu/disp/kern_disp.h"

/*!
 * @brief - Compute the value LSR_MIN_TIME to be set for swap barrier
 *
 * @param[in]  pGpu                GPU object pointer
 * @param[in]  pKernelDisplay      KernelDisplay pointer
 * @param[in]  head                head number
 * @param[in]  swapRdyHiLsrMinTime effective time in micro seconds for which
 *                                 SWAPRDY will be asserted.
 * @param[out] computedLsrMinTime  computed LsrMinTime to be set.
 */
NV_STATUS
kdispComputeLsrMinTimeValue_v02_07
(
    OBJGPU  *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32    head,
    NvU32    swapRdyHiLsrMinTime,
    NvU32   *computedLsrMinTime
)
{
    //
    // For Pascal and onwards LSR_MIN_TIME has been moved to static clock
    //in ns from dispclock. Thus just send the swapRdyHiTime in ns.
    //
    *computedLsrMinTime = swapRdyHiLsrMinTime * 1000;
    return NV_OK;
}
