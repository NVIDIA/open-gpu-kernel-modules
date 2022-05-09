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

#include "kernel/gpu/fifo/usermode_api.h"
#include "kernel/gpu/fifo/kernel_fifo.h"

#include "class/cl003e.h" // NV01_MEMORY_SYSTEM
#include "class/cl003f.h" // NV01_MEMORY_LOCAL_PRIVILEGED

NV_STATUS
usrmodeConstructHal_GV100
(
    UserModeApi                  *pUserModeApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    Memory *pMemory                             = staticCast(pUserModeApi, Memory);
    NV_STATUS status                            = NV_OK;
    MEMORY_DESCRIPTOR *pMemDesc                 = NULL;
    OBJGPU *pGpu                                = pMemory->pGpu;
    KernelFifo *pKernelFifo                     = GPU_GET_KERNEL_FIFO(pGpu);
    NvU64 offset                                = 0;
    NvU32 size                                  = 0;
    NvU32 attr                                  = 0;
    NvU32 attr2                                 = 0;

    attr = FLD_SET_DRF(OS32, _ATTR,  _PHYSICALITY, _CONTIGUOUS, attr);
    attr = FLD_SET_DRF(OS32, _ATTR,  _COHERENCY, _CACHED, attr);

    attr2 = FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _NO, attr2 );

    NV_ASSERT_OK_OR_RETURN(kfifoGetUsermodeMapInfo_HAL(pGpu, pKernelFifo, &offset, &size));

    status = memCreateMemDesc(pGpu, &pMemDesc, ADDR_REGMEM,
                              offset, size, attr, attr2);

    memdescSetFlag(pMemDesc, MEMDESC_FLAGS_SKIP_REGMEM_PRIV_CHECK, NV_TRUE);

    status = memConstructCommon(pMemory, NV01_MEMORY_LOCAL_PRIVILEGED,
                    0, pMemDesc, 0, NULL, 0, 0, 0, 0, NVOS32_MEM_TAG_NONE, NULL);

    return status;
}
