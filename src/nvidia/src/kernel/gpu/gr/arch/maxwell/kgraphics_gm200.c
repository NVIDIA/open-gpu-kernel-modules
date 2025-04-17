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

#include "gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"

#include "ctrl/ctrl0080/ctrl0080fifo.h"

/*!
 * @brief Allocate common buffers that are required by the graphics context
 */
NV_STATUS
kgraphicsAllocGrGlobalCtxBuffers_GM200
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 gfid,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    MEMORY_DESCRIPTOR           **ppMemDesc;
    GR_GLOBALCTX_BUFFERS         *pCtxBuffers;
    GR_BUFFER_ATTR               *pCtxAttr;
    NvU64                         cbAllocFlags;
    NvBool                        bPhysicallyContiguous;
    NvU64                         flags = MEMDESC_FLAGS_NONE;
    NvU32                         circularBufferSize;
    NvU32                         circularBufferAlign;
    NvU32                         pagepoolBufferSize;
    NvU32                         pagepoolBufferAlign;
    NvU32                         attribBufferSize;
    NvU32                         attribBufferAlign;
    NvU32                         privMapBufferSize;
    NvU32                         privMapBufferAlign;
    NvU32                         unresPrivMapBufferSize;
    NvU32                         unresPrivMapBufferAlign;
    MemoryManager                *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    CTX_BUF_POOL_INFO            *pCtxBufPool;
    const KGRAPHICS_STATIC_INFO  *pKernelGraphicsStaticInfo;
	NV_STATUS                     status;

    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pContextBuffersInfo != NULL, NV_ERR_INVALID_STATE);

    circularBufferSize =
        pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_BUNDLE_CB].size;
    circularBufferAlign =
        pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_BUNDLE_CB].alignment;

    pagepoolBufferSize =
        pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PAGEPOOL_GLOBAL].size;
    pagepoolBufferAlign =
        pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PAGEPOOL_GLOBAL].alignment;

    attribBufferSize =
        pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_ATTRIBUTE_CB].size;
    attribBufferAlign =
        pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_ATTRIBUTE_CB].alignment;

    privMapBufferSize =
        pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PRIV_ACCESS_MAP].size;
    privMapBufferAlign =
        pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PRIV_ACCESS_MAP].alignment;

    unresPrivMapBufferSize =
        pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PRIV_ACCESS_MAP].size;
    unresPrivMapBufferAlign =
        pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PRIV_ACCESS_MAP].alignment;

    // Setup the Circular Buffer DB
    cbAllocFlags = MEMDESC_FLAGS_LOST_ON_SUSPEND;

    if (kgraphicsShouldSetContextBuffersGPUPrivileged(pGpu, pKernelGraphics))
    {
        cbAllocFlags |= MEMDESC_FLAGS_GPU_PRIVILEGED;
    }

    pCtxBufPool = NULL;
    if (pKernelGraphicsContext != NULL)
    {
        KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

        pCtxBuffers = &pKernelGraphicsContextUnicast->localCtxBuffer;
        pCtxAttr = pKernelGraphics->globalCtxBuffersInfo.localCtxAttr;

        //
        // if we already have local buffers allocated, return as we may
        // get called multiple times per-channel
        //
        if (pCtxBuffers->bAllocated)
            return NV_OK;

        // check for allocating local buffers in VPR memory (don't want for global memory)
        if (
            pKernelGraphicsContextUnicast->bVprChannel)
            cbAllocFlags |= MEMDESC_ALLOC_FLAGS_PROTECTED;

        // If allocated per channel, ensure allocations goes into Suballocator if available
        cbAllocFlags |= MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;
    }
    else
    {
        pCtxBuffers = &pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers[gfid];
        pCtxAttr = pKernelGraphics->globalCtxBuffersInfo.globalCtxAttr;
        NV_ASSERT_OK_OR_RETURN(
            ctxBufPoolGetGlobalPool(pGpu, CTX_BUF_ID_GR_GLOBAL,
                                    RM_ENGINE_TYPE_GR(pKernelGraphics->instance),
                                    &pCtxBufPool));
    }

    // Handle VF - must use VF attributes and flags for both global and local buffers
    if (IS_GFID_VF(gfid))
    {
        pCtxAttr = pKernelGraphics->globalCtxBuffersInfo.vfGlobalCtxAttr;

        cbAllocFlags |= MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;
        flags |= MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;
    }

    // Don't use context buffer pool for VF allocations managed by host RM.
    if (ctxBufPoolIsSupported(pGpu) && (pCtxBufPool != NULL))
    {
        cbAllocFlags |= MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL;
        flags |= MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL;
    }

    // Circular Buffer
    if (circularBufferSize > 0)
    {
        ppMemDesc = &pCtxBuffers->memDesc[GR_GLOBALCTX_BUFFER_BUNDLE_CB];
        bPhysicallyContiguous = pCtxAttr[GR_GLOBALCTX_BUFFER_BUNDLE_CB].pAllocList == ADDRLIST_FBMEM_ONLY;

        if (pMemoryManager->bug64kPage5123775War) {
            circularBufferSize = RM_ALIGN_UP(circularBufferSize, 0x10000);
        }

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            memdescCreate(ppMemDesc, pGpu,
                          circularBufferSize,
                          circularBufferAlign,
                          bPhysicallyContiguous,
                          ADDR_UNKNOWN,
                          pCtxAttr[GR_GLOBALCTX_BUFFER_BUNDLE_CB].cpuAttr,
                          cbAllocFlags | MEMDESC_FLAGS_GPU_PRIVILEGED | MEMDESC_FLAGS_HIGH_PRIORITY));

        memdescSetGpuCacheAttrib(*ppMemDesc, NV_MEMORY_CACHED);
        if ((cbAllocFlags & MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL) != 0)
        {
            memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, *ppMemDesc, AT_GPU, RM_ATTR_PAGE_SIZE_4KB);
            NV_ASSERT_OK_OR_RETURN(memdescSetCtxBufPool(*ppMemDesc, pCtxBufPool));
        }

        memdescTagAllocList(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_CIRCULAR_BUFFER, *ppMemDesc, pCtxAttr[GR_GLOBALCTX_BUFFER_BUNDLE_CB].pAllocList);
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);
    }

    // Page Pool
    if (pagepoolBufferSize > 0)
    {
        ppMemDesc = &pCtxBuffers->memDesc[GR_GLOBALCTX_BUFFER_PAGEPOOL];
        bPhysicallyContiguous = pCtxAttr[GR_GLOBALCTX_BUFFER_PAGEPOOL].pAllocList == ADDRLIST_FBMEM_ONLY;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            memdescCreate(ppMemDesc, pGpu,
                          pagepoolBufferSize,
                          pagepoolBufferAlign,
                          bPhysicallyContiguous,
                          ADDR_UNKNOWN,
                          pCtxAttr[GR_GLOBALCTX_BUFFER_PAGEPOOL].cpuAttr,
                          cbAllocFlags | MEMDESC_FLAGS_GPU_PRIVILEGED));

        memdescSetGpuCacheAttrib(*ppMemDesc, NV_MEMORY_CACHED);
        if ((cbAllocFlags & MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL) != 0)
        {
            memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, *ppMemDesc, AT_GPU, RM_ATTR_PAGE_SIZE_4KB);
            NV_ASSERT_OK_OR_RETURN(memdescSetCtxBufPool(*ppMemDesc, pCtxBufPool));
        }

        memdescTagAllocList(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_PAGE_POOL, *ppMemDesc, pCtxAttr[GR_GLOBALCTX_BUFFER_PAGEPOOL].pAllocList);
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);
    }

    // Attribute Buffer
    if (attribBufferSize > 0)
    {
        ppMemDesc = &pCtxBuffers->memDesc[GR_GLOBALCTX_BUFFER_ATTRIBUTE_CB];
        bPhysicallyContiguous = pCtxAttr[GR_GLOBALCTX_BUFFER_ATTRIBUTE_CB].pAllocList == ADDRLIST_FBMEM_ONLY;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            memdescCreate(ppMemDesc, pGpu,
                          attribBufferSize,
                          attribBufferAlign,
                          bPhysicallyContiguous,
                          ADDR_UNKNOWN,
                          pCtxAttr[GR_GLOBALCTX_BUFFER_ATTRIBUTE_CB].cpuAttr,
                          cbAllocFlags | MEMDESC_FLAGS_HIGH_PRIORITY));

        memdescSetGpuCacheAttrib(*ppMemDesc, NV_MEMORY_CACHED);
        if ((cbAllocFlags & MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL) != 0)
        {
            memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, *ppMemDesc, AT_GPU, RM_ATTR_PAGE_SIZE_4KB);
            NV_ASSERT_OK_OR_RETURN(memdescSetCtxBufPool(*ppMemDesc, pCtxBufPool));
        }


        memdescTagAllocList(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_ATTR_BUFFER, *ppMemDesc, pCtxAttr[GR_GLOBALCTX_BUFFER_ATTRIBUTE_CB].pAllocList);
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);

        memdescSetName(pGpu, *ppMemDesc, NV_RM_SURF_NAME_GR_CIRCULAR_BUFFER, NULL);
    }

    // we do not want/need a priv access map allocated per-channel, so skip allocating
    if (pKernelGraphicsContext == NULL)
    {
        if (kgraphicsDoesUcodeSupportPrivAccessMap(pGpu, pKernelGraphics))
        {
            NvBool bIsContiguous = kgraphicsShouldForceMainCtxContiguity_HAL(pGpu, pKernelGraphics) && gpuIsClientRmAllocatedCtxBufferEnabled(pGpu);

            // PRIV access map
            if (privMapBufferSize > 0)
            {
                ppMemDesc = &pCtxBuffers->memDesc[GR_GLOBALCTX_BUFFER_PRIV_ACCESS_MAP];

                NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                    memdescCreate(ppMemDesc, pGpu,
                                  privMapBufferSize,
                                  privMapBufferAlign,
                                  bIsContiguous,
                                  ADDR_UNKNOWN,
                                  pCtxAttr[GR_GLOBALCTX_BUFFER_PRIV_ACCESS_MAP].cpuAttr,
                                  flags));
                if (kgraphicsIsOverrideContextBuffersToGpuCached(pGpu, pKernelGraphics))
                    memdescSetGpuCacheAttrib(*ppMemDesc, NV_MEMORY_CACHED);

                if ((flags & MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL) != 0)
                {
                    //
                    // Force page size to 4KB, we can change this later when RM
                    // access method support 64k pages
                    //
                    memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, *ppMemDesc, AT_GPU, RM_ATTR_PAGE_SIZE_4KB);
                    NV_ASSERT_OK_OR_RETURN(memdescSetCtxBufPool(*ppMemDesc, pCtxBufPool));
                }
                memdescTagAllocList(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_ACCESS_MAP, *ppMemDesc, pCtxAttr[GR_GLOBALCTX_BUFFER_PRIV_ACCESS_MAP].pAllocList);
                NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);
            }

            //
            // vGPU does not support unrestricted priv access map buffer. Hence, avoid
            // allocating it on vGPU configuration.
            //
            if ((unresPrivMapBufferSize > 0) && kgraphicsIsUnrestrictedAccessMapSupported_HAL(pGpu, pKernelGraphics))
            {
                // Unrestricted PRIV access map
                ppMemDesc = &pCtxBuffers->memDesc[GR_GLOBALCTX_BUFFER_UNRESTRICTED_PRIV_ACCESS_MAP];

                NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                    memdescCreate(ppMemDesc, pGpu,
                                  unresPrivMapBufferSize,
                                  unresPrivMapBufferAlign,
                                  bIsContiguous,
                                  ADDR_UNKNOWN,
                                  pCtxAttr[GR_GLOBALCTX_BUFFER_UNRESTRICTED_PRIV_ACCESS_MAP].cpuAttr,
                                  flags));

                if (kgraphicsIsOverrideContextBuffersToGpuCached(pGpu, pKernelGraphics))
                    memdescSetGpuCacheAttrib(*ppMemDesc, NV_MEMORY_CACHED);

                if ((flags & MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL) != 0)
                {
                    //
                    // Force page size to 4KB, we can change this later when RM
                    // access method support 64k pages
                    //
                    memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, *ppMemDesc, AT_GPU, RM_ATTR_PAGE_SIZE_4KB);
                    NV_ASSERT_OK_OR_RETURN(memdescSetCtxBufPool(*ppMemDesc, pCtxBufPool));
                }

                memdescTagAllocList(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_ACCESS_MAP, *ppMemDesc,
                                     pCtxAttr[GR_GLOBALCTX_BUFFER_UNRESTRICTED_PRIV_ACCESS_MAP].pAllocList);
                NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);
            }
        }
    }

    pCtxBuffers->bAllocated = NV_TRUE;

    return NV_OK;
}

