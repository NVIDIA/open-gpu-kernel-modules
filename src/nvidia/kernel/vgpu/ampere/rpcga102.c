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

/*!
 * @file    rpcga102.c
 * @brief   Ampere HAL routines 
 */

#include "nvtypes.h"
#include "nv_sriov_defines.h"
#include "gpu/gpu.h"
#include "gpu/gpu_access.h"
#include "published/ampere/ga102/dev_vm.h"

void rpcVgpuGspWriteScratchRegister_GA102(OBJGPU *pGpu, NvU64 scratchRegVal)
{
    // Write the scratch register
    GPU_VREG_WR32(pGpu, 
        NV_VIRTUAL_FUNCTION_PRIV_MAILBOX_SCRATCH(NV_VF_SCRATCH_REGISTER_GUEST_RPC_HI),
        NvU64_HI32(scratchRegVal));
    GPU_VREG_WR32(pGpu, 
        NV_VIRTUAL_FUNCTION_PRIV_MAILBOX_SCRATCH(NV_VF_SCRATCH_REGISTER_GUEST_RPC_LO),
        NvU64_LO32(scratchRegVal));
}

void rpcVgpuGspRingDoorbell_GA102(OBJGPU *pGpu, NvU32 doorbellToken)
{
    // Ring the setup doorbell to send the request
    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_DOORBELL, doorbellToken);
}

