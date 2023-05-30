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

#include "gpu/gpu_user_shared_data.h"
#include "gpu/gpu.h"
#include "gpu/subdevice/subdevice.h"
#include "os/os.h"
#include "rmapi/client.h"
#include "rmapi/rmapi.h"
#include "class/cl00de.h"
#include "class/cl003e.h" // NV01_MEMORY_SYSTEM

NV_STATUS
gpushareddataConstruct_IMPL
(
    GpuUserSharedData *pData,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS            status   = NV_OK;
    Memory              *pMemory  = staticCast(pData, Memory);

    // pGpu is initialied in the Memory class constructor
    OBJGPU              *pGpu     = pMemory->pGpu;
    MEMORY_DESCRIPTOR   **ppMemDesc = &(pGpu->userSharedData.pMemDesc);

    if (RS_IS_COPY_CTOR(pParams))
    {
        return NV_OK;
    }

    if (*ppMemDesc == NULL)
    {
        // Create a kernel-side mapping for writing the data if one is not already present
        NV_ASSERT_OK_OR_RETURN(memdescCreate(ppMemDesc, pGpu, sizeof(NV00DE_SHARED_DATA), 0, NV_TRUE,
                                    ADDR_SYSMEM, NV_MEMORY_CACHED, MEMDESC_FLAGS_USER_READ_ONLY));

        NV_ASSERT_OK_OR_GOTO(status, memdescAlloc(*ppMemDesc), err);


        NV_ASSERT_OK_OR_GOTO(status,
            memdescMap(*ppMemDesc, 0, (*ppMemDesc)->Size,
                        NV_TRUE, NV_PROTECT_READ_WRITE,
                        &pGpu->userSharedData.pMapBuffer,
                        &pGpu->userSharedData.pMapBufferPriv),
            err);

        portMemSet(pGpu->userSharedData.pMapBuffer, 0, sizeof(NV00DE_SHARED_DATA));

        // Initial write from cached data
        gpuUpdateUserSharedData_KERNEL(pGpu);
    }

    NV_ASSERT_OK_OR_RETURN(memConstructCommon(pMemory,
                NV01_MEMORY_SYSTEM, 0, *ppMemDesc, 0, NULL, 0, 0, 0, 0,
                NVOS32_MEM_TAG_NONE, NULL));
    memdescAddRef(pGpu->userSharedData.pMemDesc);
    return NV_OK;

err:
    memdescFree(*ppMemDesc);
    memdescDestroy(*ppMemDesc);
    *ppMemDesc = NULL;
    return status;
}

NvBool
gpushareddataCanCopy_IMPL(GpuUserSharedData *pData)
{
    return NV_TRUE;
}

NV00DE_SHARED_DATA * gpushareddataWriteStart(OBJGPU *pGpu)
{
    return &pGpu->userSharedData.data;
}

void gpushareddataWriteFinish(OBJGPU *pGpu)
{
    gpuUpdateUserSharedData_HAL(pGpu);
}

void gpuUpdateUserSharedData_KERNEL(OBJGPU *pGpu)
{
    NV00DE_SHARED_DATA *pSharedData = (NV00DE_SHARED_DATA*)(pGpu->userSharedData.pMapBuffer);
    const NvU32 data_offset = sizeof(pSharedData->seq);
    const NvU32 data_size = sizeof(NV00DE_SHARED_DATA) - data_offset;

    if (pSharedData == NULL)
        return;

    portAtomicIncrementU32(&pSharedData->seq);
    portAtomicMemoryFenceStore();

    // Push cached data to mapped buffer
    portMemCopy((NvU8*)pSharedData + data_offset, data_size,
                (NvU8*)&pGpu->userSharedData.data + data_offset, data_size);

    portAtomicMemoryFenceStore();
    portAtomicIncrementU32(&pSharedData->seq);
}
