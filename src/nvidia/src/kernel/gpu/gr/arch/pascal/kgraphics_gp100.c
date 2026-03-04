/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/prelude.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/intr/engine_idx.h"
#include "kernel/gpu/bus/kern_bus.h"
#include "nvrm_registry.h"

#include "published/pascal/gp100/dev_ctxsw_prog.h"
#include "published/pascal/gp100/dev_graphics_nobundle.h"

#include "ctrl/ctrl0080/ctrl0080fifo.h"

NvU32
kgraphicsGetFecsTraceRdOffset_GP100
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    NV_ASSERT(!IS_VIRTUAL(pGpu));

    NvU32 data = GPU_REG_RD32(pGpu, NV_PGRAPH_PRI_FECS_FALCON_MAILBOX1);

    return DRF_VAL(_CTXSW, _TIMESTAMP_BUFFER, _RD_WR_POINTER, data);
}

void
kgraphicsSetFecsTraceRdOffset_GP100
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 rdOffset
)
{
    NV_ASSERT(!IS_VIRTUAL(pGpu));

    NvU32 data = 0;

    data = FLD_SET_DRF_NUM(_CTXSW, _TIMESTAMP_BUFFER, _RD_WR_POINTER, rdOffset, data);

    if (kgraphicsIsCtxswLoggingEnabled(pGpu, pKernelGraphics))
        data = FLD_SET_DRF(_CTXSW, _TIMESTAMP_BUFFER, _MAILBOX1_TRACE_FEATURE, _ENABLED, data);

    GPU_REG_WR32(pGpu, NV_PGRAPH_PRI_FECS_FALCON_MAILBOX1, data);
}

void
kgraphicsSetFecsTraceWrOffset_GP100
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 wrOffset
)
{
    NV_ASSERT(!IS_VIRTUAL(pGpu));

    GPU_REG_WR32(pGpu, NV_PGRAPH_PRI_FECS_FALCON_MAILBOX0, wrOffset);
}

void
kgraphicsSetFecsTraceHwEnable_GP100
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvBool bEnable
)
{
    NvU32 data = 0;
    NvU32 rdOffset = kgraphicsGetFecsTraceRdOffset_HAL(pGpu, pKernelGraphics);

    NV_ASSERT(!IS_VIRTUAL(pGpu));

    data = FLD_SET_DRF_NUM(_CTXSW, _TIMESTAMP_BUFFER, _RD_WR_POINTER, rdOffset, data);

    if (bEnable)
        data = FLD_SET_DRF(_CTXSW, _TIMESTAMP_BUFFER, _MAILBOX1_TRACE_FEATURE, _ENABLED, data);

    GPU_REG_WR32(pGpu, NV_PGRAPH_PRI_FECS_FALCON_MAILBOX1, data);
    kgraphicsSetCtxswLoggingEnabled(pGpu, pKernelGraphics, bEnable);
}

void
kgraphicsInitFecsRegistryOverrides_GP100
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    NvU32 data;

    // init the FECS buffer attributes before allocating buffer
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CTXSW_LOG, &data) == NV_OK)
    {
        NvBool bIntrFallback = NV_FALSE;
        NvBool bIntr = NV_FALSE;
        NvBool bLog = NV_FALSE;

        switch (data)
        {
            case NV_REG_STR_RM_CTXSW_LOG_ENABLE_INTR_APC:
                bIntrFallback = NV_TRUE;
                // Intentional fall-through
            case NV_REG_STR_RM_CTXSW_LOG_ENABLE_INTR:
                bIntr = NV_TRUE;
                // Intentional fall-through
            case NV_REG_STR_RM_CTXSW_LOG_ENABLE:
                bLog = NV_TRUE;
                break;
            default:
                break;
        }

        kgraphicsSetBottomHalfCtxswLoggingEnabled(pGpu, pKernelGraphics, bIntrFallback);
        kgraphicsSetIntrDrivenCtxswLoggingEnabled(pGpu, pKernelGraphics, bIntr);
        kgraphicsSetCtxswLoggingSupported(pGpu, pKernelGraphics, bLog);
    }

    //
    // CTXSW logging is not supported when HCC prod settings are enabled.
    // However, the same is supported when HCC is enabled in devtools mode
    //
    if (gpuIsCCFeatureEnabled(pGpu) && !gpuIsCCDevToolsModeEnabled(pGpu))
    {
        kgraphicsSetCtxswLoggingSupported(pGpu, pKernelGraphics, NV_FALSE);
    }

    fecsSetRecordsPerIntr(pGpu, pKernelGraphics, NV_REG_STR_RM_CTXSW_LOG_RECORDS_PER_INTR_DEFAULT);
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CTXSW_LOG_RECORDS_PER_INTR, &data) == NV_OK)
    {
        fecsSetRecordsPerIntr(pGpu, pKernelGraphics, data);
    }
}

/*!
 * @brief Allocate common local/global buffers that are required by the graphics context for GfxP Pool
 *
 * @param[in] pGpu
 * @param[in] pKernelGraphics
 * @param[in] gfid                   host or guest gfid
 * @param[in] pKernelGraphicsContext graphics context - if valid allocate local
 */
NV_STATUS
kgraphicsAllocGrGlobalCtxBuffers_GP100
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 gfid,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    extern NV_STATUS kgraphicsAllocGrGlobalCtxBuffers_GM200(OBJGPU *pGpu, KernelGraphics *pKernelGraphics, NvU32 gfid, KernelGraphicsContext *pKernelGraphicsContext);
    GR_GLOBALCTX_BUFFERS         *pCtxBuffers;
    NV_STATUS                     status;
    CTX_BUF_POOL_INFO            *pCtxBufPool;
    const KGRAPHICS_STATIC_INFO  *pKernelGraphicsStaticInfo;

    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    pCtxBufPool = NULL;
    if (pKernelGraphicsContext != NULL)
    {
        KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

        pCtxBuffers = &pKernelGraphicsContextUnicast->localCtxBuffer;

        //
        // if we already have local buffers allocated, return as we may get
        // called multiple times per-channel
        //
        if (pCtxBuffers->bAllocated)
             return NV_OK;

    }
    else
    {
        pCtxBuffers = &pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers[gfid];
        NV_ASSERT_OK_OR_RETURN(ctxBufPoolGetGlobalPool(pGpu, CTX_BUF_ID_GR_GLOBAL,
            RM_ENGINE_TYPE_GR(pKernelGraphics->instance), &pCtxBufPool));
    }

    pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pContextBuffersInfo != NULL, NV_ERR_INVALID_STATE);

    status = kgraphicsAllocGrGlobalCtxBuffers_GM200(pGpu, pKernelGraphics, gfid, pKernelGraphicsContext);

    return status;
}

NV_STATUS
kgraphicsAllocGlobalCtxBuffers_GP100
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 gfid
)
{
    CTX_BUF_POOL_INFO *pCtxBufPool = NULL;
    NvU64 allocFlags = 0;
    NvU32 fecsBufferSize = 0;
    NvU32 fecsBufferAlign = 0x0;
    GR_GLOBALCTX_BUFFERS *pCtxBuffers;
    GR_BUFFER_ATTR *pCtxAttr;
    NV_STATUS status;

    // SKIP FECS buffer allocation for Virtual context
    if (IS_GFID_VF(gfid))
    {
        return NV_OK;
    }

    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    pCtxBuffers = &pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers[gfid];
    pCtxAttr = pKernelGraphics->globalCtxBuffersInfo.globalCtxAttr;
    NV_ASSERT_OK_OR_RETURN(
        ctxBufPoolGetGlobalPool(pGpu,
                                CTX_BUF_ID_GR_GLOBAL,
                                RM_ENGINE_TYPE_GR(pKernelGraphics->instance),
                                &pCtxBufPool));

    if (pCtxBufPool != NULL)
    {
        allocFlags |= MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL;
    }

    // TODO: Need this check for vGPU configs without SRIOV support?
    if (!IS_VIRTUAL(pGpu) || IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        NvU32 engineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_FECS_EVENT;
        const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);

        NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
        fecsBufferSize = pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[engineId].size;
        fecsBufferAlign = pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[engineId].alignment;
    }

    // Allocate the FECS buffer if the right regkey (RmCtxswLog) is enabled
    if ((fecsBufferSize > 0) &&
        kgraphicsIsCtxswLoggingSupported(pGpu, pKernelGraphics))
    {
        NvBool bIsFbBroken;
        MEMORY_DESCRIPTOR **ppMemDesc = &pCtxBuffers->memDesc[GR_GLOBALCTX_BUFFER_FECS_EVENT];

        bIsFbBroken = pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) ||
                      pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM);

        if (bIsFbBroken)
        {
            pCtxAttr[GR_GLOBALCTX_BUFFER_FECS_EVENT].pAllocList = ADDRLIST_SYSMEM_ONLY;
            pCtxAttr[GR_GLOBALCTX_BUFFER_FECS_EVENT].cpuAttr = NV_MEMORY_UNCACHED;
        }

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            memdescCreate(ppMemDesc, pGpu,
                          fecsBufferSize,   // size
                          fecsBufferAlign,  // alignment
                          NV_TRUE,          // physically contiguous
                          ADDR_UNKNOWN,
                          pCtxAttr[GR_GLOBALCTX_BUFFER_FECS_EVENT].cpuAttr,
                          allocFlags | MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS | MEMDESC_FLAGS_GPU_PRIVILEGED));

        if (kgraphicsIsOverrideContextBuffersToGpuCached(pGpu, pKernelGraphics) || (*ppMemDesc)->_addressSpace == ADDR_FBMEM)
            memdescSetGpuCacheAttrib(*ppMemDesc, NV_MEMORY_CACHED);

        if ((allocFlags & MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL) != 0)
        {
            MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

            memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, *ppMemDesc, AT_GPU, RM_ATTR_PAGE_SIZE_4KB);
            NV_ASSERT_OK_OR_RETURN(memdescSetCtxBufPool(*ppMemDesc, pCtxBufPool));
        }

        memdescTagAllocList(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_108,
                    (*ppMemDesc), (pCtxAttr[GR_GLOBALCTX_BUFFER_FECS_EVENT].pAllocList));
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);
    }

    return NV_OK;
}

/**
 * @brief Services the GRn_FECS_LOG interrupts.
 *
 * @returns Zero, because KernelGraphics opts out of stuck interrupt detection.
 */
NvU32
kgraphicsServiceInterrupt_GP100
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    IntrServiceServiceInterruptArguments *pParams
)
{
    NvU32 grIdx = pKernelGraphics->instance;
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);

    NV_ASSERT_OR_RETURN(pParams != NULL, 0);
    NV_ASSERT_OR_RETURN(pParams->engineIdx == MC_ENGINE_IDX_GRn_FECS_LOG(grIdx), 0);

    // if MIG is disabled and an access has been made to any GR engine ID > 0, fail
    if (!IS_MIG_IN_USE(pGpu) && (grIdx != 0))
    {
        NV_ASSERT_FAILED("GR[1-7]_FECS_LOG is not supported if MIG is disabled!");
        return 0;
    }

    if ((fecsGetCtxswLogConsumerCount(pGpu, pKernelGraphicsManager) > 0) &&
        (kgraphicsIsIntrDrivenCtxswLoggingEnabled(pGpu, pKernelGraphics)))
    {
        if (fecsClearIntrPendingIfPending(pGpu, pKernelGraphics))
        {
            nvEventBufferFecsCallback(pGpu, (void*)pKernelGraphics);
        }
    }
    return 0;
}

/**
 * @brief Clears the stall interrupt leaf vector and return whether to call ServiceStall.
 * @details Normally there's no need to override this function; however,
 *          the FECS_LOG engine idxs do not have real interrupt vectors to clear.
 *          This implementation just tells INTR to continue with servicing.
 *
 * @returns NV_TRUE indicating the interrupt should be serviced.
 */
NvBool
kgraphicsClearInterrupt_GP100
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    IntrServiceClearInterruptArguments *pParams
)
{
    NvU32 grIdx = pKernelGraphics->instance;

    NV_ASSERT_OR_RETURN(pParams != NULL, NV_FALSE);
    NV_ASSERT_OR_RETURN(pParams->engineIdx == MC_ENGINE_IDX_GRn_FECS_LOG(grIdx), 0);

    return NV_TRUE;
}
