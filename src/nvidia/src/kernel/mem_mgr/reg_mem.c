/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "mem_mgr_internal.h"
#include "mem_mgr/reg_mem.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "os/os.h"

#include "class/cl003f.h" // NV01_MEMORY_LOCAL_PRIVILEGED

NV_STATUS
regmemConstruct_IMPL
(
    RegisterMemory               *pRegisterMemory,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    NV_STATUS          status = NV_OK;
    NvU64              trueLength;
    Memory            *pMemory = staticCast(pRegisterMemory, Memory);
    OBJGPU            *pGpu = pMemory->pGpu;

    // Copy-construction has already been done by the base Memory class
    if (RS_IS_COPY_CTOR(pParams))
        return NV_OK;

    trueLength = kbusGetPciBarSize(GPU_GET_KERNEL_BUS(pGpu), 0);

    status = memCreateMemDesc(pMemory->pGpu, &pMemDesc, ADDR_REGMEM, 0,
                              trueLength, 0, 0);
    if (status == NV_OK)
    {
        status = memConstructCommon(pMemory, NV01_MEMORY_LOCAL_PRIVILEGED, 0, pMemDesc, 0, NULL, 0, 0,
                                    0, 0, NVOS32_MEM_TAG_NONE, (HWRESOURCE_INFO *)NULL);
        if (status != NV_OK)
        {
            memdescDestroy(pMemDesc);
        }
    }
    return status;
}

NvBool
regmemCanCopy_IMPL
(
    RegisterMemory *pRegisterMemory
)
{
    return NV_TRUE;
}
