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

#define NVOC_KERNEL_GRAPHICS_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/gr/kernel_graphics.h"
#include "nv_sriov_defines.h"

#include "published/blackwell/gb100/dev_ctxsw_prog.h"
#include "published/blackwell/gb100/dev_vm.h"

/*!
 * @brief Set the current trace enable status in hw
 *
 * @param[in]   bEnable                 NV_TRUE to enable, NV_FALSE to disable
 */
void
kgraphicsSetFecsTraceHwEnable_GB100
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvBool bEnable
)
{
    NvU32 data = 0;
    NvU32 rdOffset = kgraphicsGetFecsTraceRdOffset_HAL(pGpu, pKernelGraphics);

    data = FLD_SET_DRF_NUM(_CTXSW, _TIMESTAMP_BUFFER, _RD_WR_POINTER, rdOffset, data);

    if (bEnable)
        data = FLD_SET_DRF(_CTXSW, _TIMESTAMP_BUFFER, _MAILBOX1_TRACE_FEATURE, _ENABLED, data);

    GPU_VREG_WR32(pGpu,
        NV_VIRTUAL_FUNCTION_PRIV_MAILBOX_SCRATCH(NV_VF_SCRATCH_REGISTER_FECS_TRACE_RD_RD_OFFSET), data);

    kgraphicsSetCtxswLoggingEnabled(pGpu, pKernelGraphics, bEnable);
}

/*!
 * @brief Set the current read offset of the FECS trace buffer
 *
 * @param[in]   rdOffset                New read offset
 */
void
kgraphicsSetFecsTraceRdOffset_GB100
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 rdOffset
)
{
    NvU32 data = 0;
    data = FLD_SET_DRF_NUM(_CTXSW, _TIMESTAMP_BUFFER, _RD_WR_POINTER, rdOffset, data);

    if (kgraphicsIsCtxswLoggingEnabled(pGpu, pKernelGraphics))
        data = FLD_SET_DRF(_CTXSW, _TIMESTAMP_BUFFER, _MAILBOX1_TRACE_FEATURE, _ENABLED, data);

    GPU_VREG_WR32(pGpu,
        NV_VIRTUAL_FUNCTION_PRIV_MAILBOX_SCRATCH(NV_VF_SCRATCH_REGISTER_FECS_TRACE_RD_RD_OFFSET), data);
}

/*!
 * @brief Set the current write offset of the FECS trace buffer
 *
 * @param[in]   wrOffset                New write offset
 */
void
kgraphicsSetFecsTraceWrOffset_GB100
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 wrOffset
)
{
    GPU_VREG_WR32(pGpu,
        NV_VIRTUAL_FUNCTION_PRIV_MAILBOX_SCRATCH(NV_VF_SCRATCH_REGISTER_FECS_TRACE_WR_RD_OFFSET), wrOffset);
}

