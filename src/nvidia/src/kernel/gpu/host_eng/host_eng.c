/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/prelude.h"
#include "kernel/gpu/host_eng/host_eng.h"
#include "core/hal.h"
#include "os/os.h"

/*!
 * @brief generic host engine halt
 */
NV_STATUS
hostengHaltEngine_IMPL
(
    OBJGPU     *pGpu,
    OBJHOSTENG *pHosteng
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief generic host engine error reset initialization
 */
NV_STATUS
hostengHaltAndReset_IMPL
(
    OBJGPU     *pGpu,
    OBJHOSTENG *pHosteng,
    RMTIMEOUT  *pTimeout
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief generic host engine reset
 */
NV_STATUS
hostengReset_IMPL
(
    OBJGPU          *pGpu,
    OBJHOSTENG      *pHosteng,
    NvBool           bReload,
    KernelChannel  *pKernelChannel,
    KernelChannel **ppCurrentKernelChannel
)
{
    return NV_ERR_NOT_SUPPORTED;
}
