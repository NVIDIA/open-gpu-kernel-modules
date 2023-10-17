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
#include "kernel/gpu/mem_mgr/mem_mgr.h"

#include "ctrl/ctrl0080/ctrl0080fifo.h"

/*!
 * @brief Allocate common local/global buffers that are required by the graphics context
 *
 * @param[in] pGpu
 * @param[in] pKernelGraphics
 * @param[in] gfid                   host or guest gfid
 * @param[in] pKernelGraphicsContext context pointer - if valid allocate local
 */
NV_STATUS
kgraphicsAllocGrGlobalCtxBuffers_TU102
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 gfid,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    extern NV_STATUS kgraphicsAllocGrGlobalCtxBuffers_GP100(OBJGPU *pGpu, KernelGraphics *pKernelGraphics, NvU32 gfid, KernelGraphicsContext *pKernelGraphicsContext);
    MEMORY_DESCRIPTOR           **ppMemDesc;
    GR_GLOBALCTX_BUFFERS         *pCtxBuffers;
    GR_BUFFER_ATTR               *pCtxAttr;
    NvU64                         allocFlags = MEMDESC_FLAGS_NONE;
    NvBool                        bIsFbBroken = NV_FALSE;
    NvU32                         rtvcbBufferSize;
    NvU32                         rtvcbBufferAlign;
    NV_STATUS                     status;
    const KGRAPHICS_STATIC_INFO  *pKernelGraphicsStaticInfo;
    CTX_BUF_POOL_INFO            *pCtxBufPool = NULL;

    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) ||
       (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM)))
    {
        bIsFbBroken = NV_TRUE;
    }

    // Setup the Circular Buffer DB
    allocFlags = MEMDESC_FLAGS_LOST_ON_SUSPEND;

    if (kgraphicsShouldSetContextBuffersGPUPrivileged(pGpu, pKernelGraphics))
    {
        allocFlags |= MEMDESC_FLAGS_GPU_PRIVILEGED;
    }

    if (pKernelGraphicsContext != NULL)
    {
        KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

        pCtxBuffers = &pKernelGraphicsContextUnicast->localCtxBuffer;
        pCtxAttr = pKernelGraphics->globalCtxBuffersInfo.localCtxAttr;

        //
        // if we already have local buffers allocated, return as we may get
        // called multiple times per-channel
        //
        if (pCtxBuffers->bAllocated)
            return NV_OK;

        // check for allocating local buffers in VPR memory (don't want for global memory)
        if (
            pKernelGraphicsContextUnicast->bVprChannel)
            allocFlags |= MEMDESC_ALLOC_FLAGS_PROTECTED;

        // If allocated per channel, ensure allocations goes into Suballocator if available
        allocFlags |= MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;
    }
    else
    {
        pCtxBuffers = &pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers[gfid];
        pCtxAttr = pKernelGraphics->globalCtxBuffersInfo.globalCtxAttr;
        NV_ASSERT_OK_OR_RETURN(ctxBufPoolGetGlobalPool(pGpu, CTX_BUF_ID_GR_GLOBAL,
            RM_ENGINE_TYPE_GR(pKernelGraphics->instance), &pCtxBufPool));
    }

    // Don't use context buffer pool for VF allocations managed by host RM.
    if (ctxBufPoolIsSupported(pGpu) && (pCtxBufPool != NULL))
    {
        allocFlags |= MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL;
    }

    if (IS_GFID_VF(gfid))
    {
        pCtxAttr = pKernelGraphics->globalCtxBuffersInfo.vfGlobalCtxAttr;
        allocFlags |= MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;
    }

    pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pContextBuffersInfo != NULL, NV_ERR_INVALID_STATE);

    rtvcbBufferSize =
        pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_RTV_CB_GLOBAL].size;
    rtvcbBufferAlign =
        pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_RTV_CB_GLOBAL].alignment;

    if (rtvcbBufferSize > 0)
    {
        ppMemDesc = &pCtxBuffers->memDesc[GR_GLOBALCTX_BUFFER_RTV_CB];
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            memdescCreate(ppMemDesc, pGpu,
                          rtvcbBufferSize,
                          rtvcbBufferAlign,
                          !bIsFbBroken,
                          ADDR_UNKNOWN,
                          pCtxAttr[GR_GLOBALCTX_BUFFER_RTV_CB].cpuAttr,
                          allocFlags));

        memdescSetGpuCacheAttrib(*ppMemDesc, NV_MEMORY_CACHED);
        if ((allocFlags & MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL) != 0)
        {
            MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

            memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, *ppMemDesc, AT_GPU, RM_ATTR_PAGE_SIZE_4KB);
            NV_ASSERT_OK_OR_RETURN(memdescSetCtxBufPool(*ppMemDesc, pCtxBufPool));
        }
        memdescTagAllocList(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_113, 
                    (*ppMemDesc), pCtxAttr[GR_GLOBALCTX_BUFFER_RTV_CB].pAllocList);
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);
    }
    status = kgraphicsAllocGrGlobalCtxBuffers_GP100(pGpu, pKernelGraphics, gfid, pKernelGraphicsContext);

    return status;
}
