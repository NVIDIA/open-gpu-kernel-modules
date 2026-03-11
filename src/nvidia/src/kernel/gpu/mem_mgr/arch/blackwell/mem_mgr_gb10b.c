/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/gsp/kernel_gsp.h"

NV_STATUS
memmgrSc7SrInitGsp_GB10B
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    KernelGsp   *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    RM_API      *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_FBSR_INIT_PARAMS params;

    params.hClient    = pMemoryManager->hClient;
    params.hSysMem    = NV01_NULL_OBJECT;
    params.bEnteringGcoffState = NV_FALSE;
    params.sysmemAddrOfSuspendResumeData = memdescGetPhysAddr(pKernelGsp->pSRMetaDescriptor, AT_GPU, 0);

    // Send S/R init information to GSP, SR Meta data descriptor address
    // is the only valid information passed for 0FB chips.
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                           pGpu->hInternalClient,
                           pGpu->hInternalSubdevice,
                           NV2080_CTRL_CMD_INTERNAL_FBSR_INIT,
                           &params,
                           sizeof(params)));
    return NV_OK;
}
