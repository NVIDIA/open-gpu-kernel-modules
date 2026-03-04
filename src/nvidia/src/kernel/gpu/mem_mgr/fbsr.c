/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "os/os.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/mem_mgr/fbsr.h"
#include "class/cl906f.h"
#include "nvrm_registry.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/bus/kern_bus.h"
#include "rmapi/client.h"
#include "vgpu/rpc_headers.h"

NV_STATUS
fbsrObjectInit_IMPL(OBJFBSR *pFbsr, NvU32 type)
{
    pFbsr->type   = type;
    pFbsr->bValid = NV_FALSE;
    pFbsr->bInitialized = NV_FALSE;
    pFbsr->pRegionRecords = NULL;
    pFbsr->numRegions = 0;

    return NV_OK;
}

/*!
 * @brief Reserves the system memory for DMA type FBSR.
 *
 * @param[in] pGpu  OBJGPU pointer
 * @param[in] pFbsr OBJFBSR pointer
 * @param[in] Size  Reserved system memory size
 *
 * @returns NV_OK on success, error otherwise.
 */
NV_STATUS
fbsrReserveSysMemoryForPowerMgmt_IMPL
(
    OBJGPU     *pGpu,
    OBJFBSR    *pFbsr,
    NvU64       Size
)
{
    NV_STATUS status;

    if (pFbsr->type != FBSR_TYPE_DMA)
        return NV_ERR_GENERIC;

    status = memdescCreate(&pFbsr->pSysReservedMemDesc, pGpu,
                           Size, 0, NV_FALSE,
                           ADDR_SYSMEM, NV_MEMORY_UNCACHED,
                           MEMDESC_FLAGS_NONE);
    if (status != NV_OK)
        return status;

    memdescTagAlloc(status, 
            NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_18, pFbsr->pSysReservedMemDesc);
    if (status != NV_OK)
    {
        memdescDestroy(pFbsr->pSysReservedMemDesc);
        pFbsr->pSysReservedMemDesc = NULL;
    }

    return status;
}

/*!
 * @brief Free the reserved system memory for DMA type FBSR.
 *
 * @param[in] pFbsr OBJFBSR pointer
 *
 * @returns None
 */
void
fbsrFreeReservedSysMemoryForPowerMgmt_IMPL(OBJFBSR *pFbsr)
{
    if (pFbsr->pSysReservedMemDesc != NULL)
    {
        memdescFree(pFbsr->pSysReservedMemDesc);
        memdescDestroy(pFbsr->pSysReservedMemDesc);
        pFbsr->pSysReservedMemDesc = NULL;
    }
}
