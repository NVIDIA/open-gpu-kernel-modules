/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"

#include "kernel/gpu/fifo/kernel_channel_group.h"

/**
 * @brief Gets the default runlist id to use for TSGs allocated with no engines on them.
 *
 * @param[in] pGpu
 * @param[in] pKernelChannelGroup      - TSG to retrieve default runlist id for
 */
NvU32
kchangrpGetDefaultRunlist_GM107
(
    OBJGPU             *pGpu,
    KernelChannelGroup *pKernelChannelGroup
)
{
    KernelFifo   *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32         runlistId = INVALID_RUNLIST_ID;
    ENGDESCRIPTOR engDesc   = ENG_GR(0);

    if (NV2080_ENGINE_TYPE_IS_VALID(pKernelChannelGroup->engineType))
    {
        // if translation fails, default is GR0
        NV_ASSERT_OK(kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                              ENGINE_INFO_TYPE_NV2080,
                                              pKernelChannelGroup->engineType,
                                              ENGINE_INFO_TYPE_ENG_DESC,
                                              &engDesc));
    }

    // if translation fails, defualt is INVALID_RUNLIST_ID
    NV_ASSERT_OK(
        kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                 ENGINE_INFO_TYPE_ENG_DESC, engDesc,
                                 ENGINE_INFO_TYPE_RUNLIST, &runlistId));

    return runlistId;
}
