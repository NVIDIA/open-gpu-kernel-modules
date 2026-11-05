/*
 * SPDX-FileCopyrightText: Copyright (c) 2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "mem_mgr/no_device_mem.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "os/os.h"

NV_STATUS
nodevicememConstruct_IMPL
(
    NoDeviceMemory *pNoDeviceMemory,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS                    status;
    MEMORY_DESCRIPTOR           *pMemDesc = NULL;
    NV_MEMORY_ALLOCATION_PARAMS *pAllocParams = pParams->pAllocParams;
    Memory                      *pMemory = staticCast(pNoDeviceMemory, Memory);
    NvU32                        cpuCacheAttrib;
    NvU32                        coherency;
    NvU32                        protection;
    NvU32                        attr = 0;
    NvU32                        attr2 = 0;

    coherency = DRF_VAL(OS32, _ATTR, _COHERENCY, pAllocParams->attr);
    attr = FLD_SET_DRF_NUM(OS32, _ATTR, _COHERENCY, coherency, attr);
    switch (coherency)
    {
        case NVOS32_ATTR_COHERENCY_CACHED:
        case NVOS32_ATTR_COHERENCY_WRITE_THROUGH:
        case NVOS32_ATTR_COHERENCY_WRITE_PROTECT:
        case NVOS32_ATTR_COHERENCY_WRITE_BACK:
            cpuCacheAttrib = NV_MEMORY_CACHED;
            break;
        case NVOS32_ATTR_COHERENCY_UNCACHED:
            cpuCacheAttrib = NV_MEMORY_UNCACHED;
            break;
        case NVOS32_ATTR_COHERENCY_WRITE_COMBINE:
            // Intentional fall-through
        default:
            cpuCacheAttrib = NV_MEMORY_WRITECOMBINED;
            break;
    }

    status = memdescCreate(&pMemDesc, NULL, pAllocParams->size, 0, NV_MEMORY_CONTIGUOUS,
                           ADDR_SYSMEM, cpuCacheAttrib, MEMDESC_FLAGS_CPU_ONLY);
    if (status != NV_OK)
        return status;

    status = osAllocPages(pMemDesc);
    if (status != NV_OK)
        goto cleanup;
    pMemDesc->Allocated = 1;

    protection = DRF_VAL(OS32, _ATTR2, _PROTECTION_USER, pAllocParams->attr2);
    attr2 = FLD_SET_DRF_NUM(OS32, _ATTR2, _PROTECTION_USER, protection, attr2);
    if (protection == NVOS32_ATTR2_PROTECTION_USER_READ_ONLY)
        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_USER_READ_ONLY, NV_TRUE);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_INFO,
        memSetGpuCacheSnoop(NULL, attr, pMemDesc), cleanup);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_INFO,
        memConstructCommon(pMemory, pCallContext->pResourceRef->externalClassId, pAllocParams->flags, pMemDesc, 0,
                           NULL, attr, attr2, 0, 0,
                           0, NULL),
        cleanup);

    return NV_OK;

cleanup:
    if (!pMemory->bConstructed)
    {
        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);
    }
    return status;
}