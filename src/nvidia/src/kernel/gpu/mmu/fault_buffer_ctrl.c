/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED

#include "core/core.h"

#include "gpu/mmu/kern_gmmu.h"
#include "rmapi/control.h"

#include "ctrl/ctrlb069.h"
#include "gpu/mmu/mmu_fault_buffer.h"

NV_STATUS
faultbufCtrlCmdFaultbufferGetSize_IMPL
(
    MmuFaultBuffer *pMmuFaultBuffer,
    NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS *pGetParams
)
{
    OBJGPU  *pGpu  = GPU_RES_GET_GPU(pMmuFaultBuffer);
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

    pGetParams->faultBufferSize =
        pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hwFaultBuffers[REPLAYABLE_FAULT_BUFFER].faultBufferSize;

    return NV_OK;
}

NV_STATUS
faultbufCtrlCmdFaultbufferGetRegisterMappings_IMPL
(
    MmuFaultBuffer *pMmuFaultBuffer,
    NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS *pParams
)
{
    OBJGPU  *pGpu  = GPU_RES_GET_GPU(pMmuFaultBuffer);
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

    return kgmmuGetFaultRegisterMappings_HAL(pGpu, pKernelGmmu, pParams->faultBufferType,
                                            &pParams->pFaultBufferGet,
                                            &pParams->pFaultBufferPut,
                                            &pParams->pFaultBufferInfo,
                                            &pParams->pPmcIntr,
                                            &pParams->pPmcIntrEnSet,
                                            &pParams->pPmcIntrEnClear,
                                            &pParams->replayableFaultMask,
                                            &pParams->pPrefetchCtrl);
}
