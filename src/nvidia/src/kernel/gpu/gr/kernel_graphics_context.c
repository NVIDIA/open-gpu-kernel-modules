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

#define NVOC_KERNEL_GRAPHICS_CONTEXT_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/gr/kernel_graphics_context.h"
#include "kernel/gpu/gr/kernel_graphics_manager.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/fifo/kernel_channel_group_api.h"
#include "kernel/mem_mgr/gpu_vaspace.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/mem_sys/kern_mem_sys.h"
#include "kernel/core/locks.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "vgpu/rpc.h"
#include "gpu/device/device.h"
#include "kernel/gpu/subdevice/subdevice.h"
#include "kernel/virtualization/hypervisor/hypervisor.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "gpu/mmu/kern_gmmu.h"
#include "platform/sli/sli.h"
#include "rmapi/client.h"

/*!
 * @brief Retrieve the context object from a KernelChannel.
 *
 * @return NV_ERR_OBJECT_NOT_FOUND if context object is missing
 */
NV_STATUS
kgrctxFromKernelChannel_IMPL
(
    KernelChannel *pKernelChannel,
    KernelGraphicsContext **ppKernelGraphicsContext
)
{
    RsResourceRef *pResourceRef;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);

    if (pKernelChannel->hKernelGraphicsContext != NV01_NULL_OBJECT)
    {
        NV_ASSERT_OK_OR_RETURN(
            clientGetResourceRefByType(RES_GET_CLIENT(pKernelChannel),
                                       pKernelChannel->hKernelGraphicsContext,
                                       classId(KernelGraphicsContext),
                                       &pResourceRef));
    }
    else if ((pKernelChannel->pKernelChannelGroupApi != NULL) &&
             (pKernelChannel->pKernelChannelGroupApi->hKernelGraphicsContext != NV01_NULL_OBJECT))
    {
        NV_ASSERT_OK_OR_RETURN(
            clientGetResourceRefByType(RES_GET_CLIENT(pKernelChannel),
                                       pKernelChannel->pKernelChannelGroupApi->hKernelGraphicsContext,
                                       classId(KernelGraphicsContext),
                                       &pResourceRef));
    }
    else
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    *ppKernelGraphicsContext = dynamicCast(pResourceRef->pResource, KernelGraphicsContext);
    NV_ASSERT_OR_RETURN(*ppKernelGraphicsContext != NULL, NV_ERR_INVALID_STATE);

    return NV_OK;
}

/*!
 * @brief Retrieve the context object from a ChannelGroup.
 *
 * @return NV_ERR_OBJECT_NOT_FOUND if context object is missing
 */
NV_STATUS
kgrctxFromKernelChannelGroupApi_IMPL
(
    KernelChannelGroupApi  *pKernelChannelGroupApi,
    KernelGraphicsContext **ppKernelGraphicsContext
)
{
    RsResourceRef *pResourceRef;

    NV_ASSERT_OR_RETURN(pKernelChannelGroupApi != NULL, NV_ERR_INVALID_ARGUMENT);

    if (pKernelChannelGroupApi->hKernelGraphicsContext == NV01_NULL_OBJECT)
        return NV_ERR_OBJECT_NOT_FOUND;

    NV_ASSERT_OK_OR_RETURN(
        clientGetResourceRefByType(RES_GET_CLIENT(pKernelChannelGroupApi),
                                   pKernelChannelGroupApi->hKernelGraphicsContext,
                                   classId(KernelGraphicsContext),
                                   &pResourceRef));

    *ppKernelGraphicsContext = dynamicCast(pResourceRef->pResource, KernelGraphicsContext);
    NV_ASSERT_OR_RETURN(*ppKernelGraphicsContext != NULL, NV_ERR_INVALID_STATE);

    return NV_OK;
}

/**
 * @brief Handle NV0090 ctrl call forwarding. The current control call is
 *        dispatched to the KernelGraphicsContext object provided.
 */
NV_STATUS kgrctxCtrlHandle
(
    CALL_CONTEXT *pCallContext,
    NvHandle hKernelGraphicsContext
)
{
    RsResourceRef *pResourceRef;

    NV_ASSERT_OK_OR_RETURN(
        clientGetResourceRefByType(pCallContext->pClient,
                                   hKernelGraphicsContext,
                                   classId(KernelGraphicsContext),
                                   &pResourceRef));

    return resControl(pResourceRef->pResource, pCallContext, pCallContext->pControlParams);
}

/**
 * @brief  Translate global ctx buffer enum to external NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID
 */
NV_STATUS
kgrctxGetGlobalContextBufferExternalId_IMPL
(
    GR_GLOBALCTX_BUFFER id,
    NvU32 *pExternalId
)
{
    NV_ASSERT_OR_RETURN(pExternalId != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NV_ENUM_IS(GR_GLOBALCTX_BUFFER, id), NV_ERR_INVALID_ARGUMENT);

    switch (id)
    {
        case GR_GLOBALCTX_BUFFER_BUNDLE_CB:
            *pExternalId = NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_BUFFER_BUNDLE_CB;
            break;
        case GR_GLOBALCTX_BUFFER_PAGEPOOL:
            *pExternalId = NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PAGEPOOL;
            break;
        case GR_GLOBALCTX_BUFFER_ATTRIBUTE_CB:
            *pExternalId = NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_ATTRIBUTE_CB;
            break;
        case GR_GLOBALCTX_BUFFER_RTV_CB:
            *pExternalId = NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_RTV_CB_GLOBAL;
            break;
        case GR_GLOBALCTX_BUFFER_GFXP_POOL:
            *pExternalId = NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GFXP_POOL;
            break;
        case GR_GLOBALCTX_BUFFER_GFXP_CTRL_BLK:
            *pExternalId = NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GFXP_CTRL_BLK;
            break;
        case GR_GLOBALCTX_BUFFER_FECS_EVENT:
            *pExternalId = NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_FECS_EVENT;
            break;
        case GR_GLOBALCTX_BUFFER_UNRESTRICTED_PRIV_ACCESS_MAP:
            *pExternalId = NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_UNRESTRICTED_PRIV_ACCESS_MAP;
            break;
        case GR_GLOBALCTX_BUFFER_PRIV_ACCESS_MAP:
            *pExternalId = NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PRIV_ACCESS_MAP;
            break;
        case GR_GLOBAL_BUFFER_GLOBAL_PRIV_ACCESS_MAP:
            // TODO this is not valid, this is not a context buffer
            *pExternalId = NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GLOBAL_PRIV_ACCESS_MAP;
            break;
        //
        // No default case: Compiler will enforce that this switch is updated if
        // new global ctx buffers are added
        //
    }

    return NV_OK;
}

/**
 * @brief  Translate NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID to global ctx buffer enum
 */
NV_STATUS
kgrctxGetGlobalContextBufferInternalId_IMPL
(
    NvU32 externalId,
    GR_GLOBALCTX_BUFFER *pInternalId
)
{
    NV_ASSERT_OR_RETURN(pInternalId != NULL, NV_ERR_INVALID_ARGUMENT);

    switch (externalId)
    {
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_MAIN:
            // fall through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PM:
            // fall through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PATCH:
            return NV_ERR_INVALID_ARGUMENT;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_BUFFER_BUNDLE_CB:
            *pInternalId = GR_GLOBALCTX_BUFFER_BUNDLE_CB;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PAGEPOOL:
            *pInternalId = GR_GLOBALCTX_BUFFER_PAGEPOOL;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_ATTRIBUTE_CB:
            *pInternalId = GR_GLOBALCTX_BUFFER_ATTRIBUTE_CB;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_RTV_CB_GLOBAL:
            *pInternalId = GR_GLOBALCTX_BUFFER_RTV_CB;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GFXP_POOL:
            *pInternalId = GR_GLOBALCTX_BUFFER_GFXP_POOL;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GFXP_CTRL_BLK:
            *pInternalId = GR_GLOBALCTX_BUFFER_GFXP_CTRL_BLK;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_FECS_EVENT:
            *pInternalId = GR_GLOBALCTX_BUFFER_FECS_EVENT;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PRIV_ACCESS_MAP:
            *pInternalId = GR_GLOBALCTX_BUFFER_PRIV_ACCESS_MAP;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_UNRESTRICTED_PRIV_ACCESS_MAP:
            *pInternalId = GR_GLOBALCTX_BUFFER_UNRESTRICTED_PRIV_ACCESS_MAP;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GLOBAL_PRIV_ACCESS_MAP:
            *pInternalId = GR_GLOBAL_BUFFER_GLOBAL_PRIV_ACCESS_MAP;
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

/*! Translate GR_CTX_BUFFER to NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID */
NV_STATUS
kgrctxCtxBufferToFifoEngineId_IMPL
(
    GR_CTX_BUFFER buffer,
    NvU32 *pFifoEngineId
)
{
    NV_ASSERT_OR_RETURN(NV_ENUM_IS(GR_CTX_BUFFER, buffer), NV_ERR_INVALID_ARGUMENT);

    switch (buffer)
    {
        case GR_CTX_BUFFER_MAIN:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS;
            break;
        case GR_CTX_BUFFER_ZCULL:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_ZCULL;
            break;
        case GR_CTX_BUFFER_PM:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PM;
            break;
        case GR_CTX_BUFFER_PREEMPT:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PREEMPT;
            break;
        case GR_CTX_BUFFER_SPILL:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_SPILL;
            break;
        case GR_CTX_BUFFER_BETA_CB:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_BETACB;
            break;
        case GR_CTX_BUFFER_PAGEPOOL:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PAGEPOOL;
            break;
        case GR_CTX_BUFFER_RTV_CB:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_RTV;
            break;
        case GR_CTX_BUFFER_PATCH:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PATCH;
            break;
        case GR_CTX_BUFFER_SETUP:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_SETUP;
            break;
        // No default case - Compiler enforces switch update if enum is changed
    }

    return NV_OK;
}

/**
 * @brief Translate global ctx buffer enum to NV0080 FIFO engine context properties index
 */
NV_STATUS
kgrctxGlobalCtxBufferToFifoEngineId_IMPL
(
    GR_GLOBALCTX_BUFFER buffId,
    NvU32 *pFifoEngineId
)
{
    NV_ASSERT_OR_RETURN(pFifoEngineId != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NV_ENUM_IS(GR_GLOBALCTX_BUFFER, buffId), NV_ERR_INVALID_ARGUMENT);

    switch (buffId)
    {
        case GR_GLOBALCTX_BUFFER_BUNDLE_CB:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_BUNDLE_CB;
            break;
        case GR_GLOBALCTX_BUFFER_PAGEPOOL:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PAGEPOOL_GLOBAL;
            break;
        case GR_GLOBALCTX_BUFFER_ATTRIBUTE_CB:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_ATTRIBUTE_CB;
            break;
        case GR_GLOBALCTX_BUFFER_RTV_CB:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_RTV_CB_GLOBAL;
            break;
        case GR_GLOBALCTX_BUFFER_GFXP_POOL:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_GFXP_POOL;
            break;
        case GR_GLOBALCTX_BUFFER_GFXP_CTRL_BLK:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_GFXP_CTRL_BLK;
            break;
        case GR_GLOBALCTX_BUFFER_FECS_EVENT:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_FECS_EVENT;
            break;
        case GR_GLOBALCTX_BUFFER_PRIV_ACCESS_MAP:
            // fall-through
        case GR_GLOBALCTX_BUFFER_UNRESTRICTED_PRIV_ACCESS_MAP:
            // fall-through
        case GR_GLOBAL_BUFFER_GLOBAL_PRIV_ACCESS_MAP:
            *pFifoEngineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PRIV_ACCESS_MAP;
            break;
        // No default case - compiler enforces update if enum changes
    }

    return NV_OK;
}

NV_STATUS
kgrctxGetGidInfoInPlace_IMPL
(
    OBJGPU *pGpu,
    NvU8 *pUuidBuffer,
    NvU32 uuidBufferSize,
    NvU32 flags
)
{
    NvU8 *pUuid;
    NvU32 uuidLength;
    NV_STATUS status;

    // on success, allocates memory for uuid
    status = gpuGetGidInfo(pGpu, &pUuid, &uuidLength, flags);
    if (status != NV_OK)
    {
        return status;
    }

    if (uuidLength == uuidBufferSize)
    {
        portMemCopy(pUuidBuffer, uuidBufferSize, pUuid, uuidLength);
    }

    portMemFree(pUuid);

    if (uuidLength != uuidBufferSize)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

NV_STATUS
kgrctxFillCtxBufferInfo_IMPL
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32 externalId,
    NvBool bBufferGlobal,
    NV2080_CTRL_GR_CTX_BUFFER_INFO *pCtxBufferInfo
)
{
    NvU64 pageSize;
    NV_STATUS status;

    MEMORY_DESCRIPTOR *pRootMemDesc = memdescGetRootMemDesc(pMemDesc, NULL);

    pCtxBufferInfo->bufferHandle = NV_PTR_TO_NvP64(pMemDesc);
    pCtxBufferInfo->bufferType = externalId;
    pCtxBufferInfo->bIsContigous = memdescGetContiguity(pMemDesc, AT_GPU);
    pCtxBufferInfo->aperture = memdescGetAddressSpace(pMemDesc);
    pCtxBufferInfo->pageCount = pMemDesc->PageCount;
    pCtxBufferInfo->kind = memdescGetPteKindForGpu(pMemDesc, pMemDesc->pGpu);

    {
        NvU64 physAddr;
        GMMU_APERTURE aperture = kgmmuGetExternalAllocAperture(pCtxBufferInfo->aperture);

        memdescGetPhysAddrsForGpu(pMemDesc, pMemDesc->pGpu,
                                  AT_GPU, 0, 0, 1,
                                  &physAddr);

        pCtxBufferInfo->physAddr =
            kgmmuEncodePhysAddr(GPU_GET_KERNEL_GMMU(pMemDesc->pGpu), aperture, physAddr,
                                NVLINK_INVALID_FABRIC_ADDR);
    }

    pageSize = memdescGetPageSize(pMemDesc, AT_GPU);
    if (pageSize == 0)
    {
        status = memmgrSetMemDescPageSize_HAL(pMemDesc->pGpu,
                                              GPU_GET_MEMORY_MANAGER(pMemDesc->pGpu),
                                              pMemDesc,
                                              AT_GPU,
                                              RM_ATTR_PAGE_SIZE_DEFAULT);
        if (status != NV_OK)
            return status;

        pageSize = memdescGetPageSize(pMemDesc, AT_GPU);
        NV_ASSERT(pageSize != 0);
    }

    //
    // Alignment is used to adjust the mapping VA. Hence, we need to make sure
    // that at least pageSize to make mapping calculation work correctly.
    //
    pCtxBufferInfo->alignment = (pMemDesc->Alignment != 0) ?
        NV_ALIGN_UP(pMemDesc->Alignment, pageSize) : pageSize;

    pCtxBufferInfo->size = pMemDesc->ActualSize;
    pCtxBufferInfo->pageSize = pageSize;

    pCtxBufferInfo->bGlobalBuffer = bBufferGlobal;
    pCtxBufferInfo->bLocalBuffer = !bBufferGlobal;
    pCtxBufferInfo->bDeviceDescendant = pRootMemDesc->pGpu != NULL;

    if (pCtxBufferInfo->bDeviceDescendant)
    {
        status = kgrctxGetGidInfoInPlace(pMemDesc->pGpu, pCtxBufferInfo->uuid, sizeof(pCtxBufferInfo->uuid),
                                         DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1) |
                                         DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _BINARY));
        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

/*!
 * @brief Construct dupable kernel graphics context.
 */
NV_STATUS
kgrctxConstruct_IMPL
(
    KernelGraphicsContext *pKernelGraphicsContext,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS status = NV_OK;
    RsShared *pShared;

    if (RS_IS_COPY_CTOR(pParams))
        return kgrctxCopyConstruct_IMPL(pKernelGraphicsContext, pCallContext, pParams);

    NV_ASSERT_OK_OR_GOTO(status,
        serverAllocShare(&g_resServ, classInfo(KernelGraphicsContextShared), &pShared),
        cleanup);

    pKernelGraphicsContext->pShared = dynamicCast(pShared, KernelGraphicsContextShared);
    NV_ASSERT_OK_OR_GOTO(status,
        shrkgrctxInit(GPU_RES_GET_GPU(pKernelGraphicsContext),
                      pKernelGraphicsContext->pShared,
                      pKernelGraphicsContext),
        cleanup);

cleanup:
    if (status != NV_OK)
    {
        if (pKernelGraphicsContext->pShared != NULL)
            serverFreeShare(&g_resServ, pShared);

    }

    return status;
}

/*!
 * @brief Copy Construct dupable kernel graphics context.
 */
NV_STATUS
kgrctxCopyConstruct_IMPL
(
    KernelGraphicsContext *pKernelGraphicsContextDst,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    RsResourceRef *pSrcRef = pParams->pSrcRef;
    KernelGraphicsContext *pKernelGraphicsContextSrc =
        dynamicCast(pSrcRef->pResource, KernelGraphicsContext);

    pKernelGraphicsContextDst->pShared = pKernelGraphicsContextSrc->pShared;
    serverRefShare(&g_resServ, staticCast(pKernelGraphicsContextDst->pShared, RsShared));

    return NV_OK;
}

NvHandle
kgrctxGetInternalObjectHandle_IMPL(KernelGraphicsContext *pKernelGraphicsContext)
{
    return NV01_NULL_OBJECT;
}

/*!
 * @brief Destruct dupable kernel graphics context.
 */
void
kgrctxDestruct_IMPL
(
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    RsShared *pShared = staticCast(pKernelGraphicsContext->pShared, RsShared);
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelGraphicsContext);

    if (serverGetShareRefCount(&g_resServ, pShared) == 1)
    {
        shrkgrctxTeardown_IMPL(pGpu, pKernelGraphicsContext->pShared, pKernelGraphicsContext);
    }

    serverFreeShare(&g_resServ, pShared);
}

/*!
 * @brief Retrieve unicast context state
 */
NV_STATUS kgrctxGetUnicast_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphicsContextUnicast **ppKernelGraphicsContextUnicast
)
{
    *ppKernelGraphicsContextUnicast = NULL;
    NV_ASSERT_OR_RETURN(pKernelGraphicsContext->pShared != NULL, NV_ERR_INVALID_STATE);
    *ppKernelGraphicsContextUnicast = &pKernelGraphicsContext->pShared->kernelGraphicsContextUnicast;
    return NV_OK;
}

/*!
 * @brief Query the details of MMU faults caused by this context
 */
NV_STATUS
kgrctxLookupMmuFaultInfo_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    NV83DE_CTRL_DEBUG_READ_MMU_FAULT_INFO_PARAMS *pParams
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    const NvU32 size = NV_ARRAY_ELEMENTS(pKernelGraphicsContextUnicast->mmuFault.mmuFaultInfoList);
    NvU32 i;

    NV_ASSERT_OK_OR_RETURN(
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

    NV_ASSERT_OR_RETURN(pKernelGraphicsContextUnicast->mmuFault.head < size, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsContextUnicast->mmuFault.tail < size, NV_ERR_INVALID_STATE);

    pParams->count = 0;
    for (i = pKernelGraphicsContextUnicast->mmuFault.tail;
         i != pKernelGraphicsContextUnicast->mmuFault.head;
         i = (i + 1) % size)
    {
        pParams->mmuFaultInfoList[pParams->count] = pKernelGraphicsContextUnicast->mmuFault.mmuFaultInfoList[i];
        pParams->count++;
    }

    return NV_OK;
}

/*!
 * @brief Query the details of MMU faults caused by this context
 */
NV_STATUS
kgrctxLookupMmuFault_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    NV83DE_MMU_FAULT_INFO *pMmuFaultInfo
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;

    NV_ASSERT_OK_OR_RETURN(
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

    *pMmuFaultInfo = pKernelGraphicsContextUnicast->mmuFault.mmuFaultInfo;
    return NV_OK;
}

/*!
 * @brief clear the details of MMU faults caused by this context
 */
NV_STATUS
kgrctxClearMmuFault_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;

    NV_ASSERT_OK_OR_RETURN(
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

    pKernelGraphicsContextUnicast->mmuFault.mmuFaultInfo.valid = NV_FALSE;
    pKernelGraphicsContextUnicast->mmuFault.mmuFaultInfo.faultInfo = 0;
    return NV_OK;
}

/*!
 * @brief Record the details of an MMU fault caused by this context
 */
void
kgrctxRecordMmuFault_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    NvU32 mmuFaultInfo,
    NvU64 mmuFaultAddress,
    NvU32 mmuFaultType,
    NvU32 mmuFaultAccessType
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NV_STATUS status;

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast),
        return;);

    pKernelGraphicsContextUnicast->mmuFault.mmuFaultInfo.valid = NV_TRUE;
    pKernelGraphicsContextUnicast->mmuFault.mmuFaultInfo.faultInfo = mmuFaultInfo;

    {
        const NvU32 size = NV_ARRAY_ELEMENTS(pKernelGraphicsContextUnicast->mmuFault.mmuFaultInfoList);

        NV_ASSERT_OR_RETURN_VOID(pKernelGraphicsContextUnicast->mmuFault.head < size);
        NV_ASSERT_OR_RETURN_VOID(pKernelGraphicsContextUnicast->mmuFault.tail < size);

        pKernelGraphicsContextUnicast->mmuFault.mmuFaultInfoList[pKernelGraphicsContextUnicast->mmuFault.head].faultAddress = mmuFaultAddress;
        pKernelGraphicsContextUnicast->mmuFault.mmuFaultInfoList[pKernelGraphicsContextUnicast->mmuFault.head].faultType = mmuFaultType;
        pKernelGraphicsContextUnicast->mmuFault.mmuFaultInfoList[pKernelGraphicsContextUnicast->mmuFault.head].accessType = mmuFaultAccessType;
        pKernelGraphicsContextUnicast->mmuFault.head = (pKernelGraphicsContextUnicast->mmuFault.head + 1) % size;
        if (pKernelGraphicsContextUnicast->mmuFault.head == pKernelGraphicsContextUnicast->mmuFault.tail)
            pKernelGraphicsContextUnicast->mmuFault.tail = (pKernelGraphicsContextUnicast->mmuFault.tail + 1) % size;
    }
}

/*!
 * @brief getter for active debugger interator
 */
KernelSMDebuggerSessionListIter
kgrctxGetDebuggerSessionIter_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext
) 
{
    return listIterAll(&pKernelGraphicsContext->pShared->activeDebuggers);
}

/*! add active debugger session */
NvBool
kgrctxRegisterKernelSMDebuggerSession_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelSMDebuggerSession *pKernelSMDebuggerSession
)
{
    return listAppendValue(&pKernelGraphicsContext->pShared->activeDebuggers, &pKernelSMDebuggerSession) != NULL;
}

/*! remove active debugger session */
void
kgrctxDeregisterKernelSMDebuggerSession_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelSMDebuggerSession *pKernelSMDebuggerSession
)
{
    listRemoveFirstByValue(&pKernelGraphicsContext->pShared->activeDebuggers, &pKernelSMDebuggerSession);
}

/*!
 * @brief Determine whether channels in this context are associated with GR engine
 *
 * @returns NV_TRUE if passed channel is allocated on GR, and is on GR runlist
 */
NvBool
kgrctxIsValid_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelChannel *pKernelChannel
)
{
    NvU32 runlistId;
    RM_ENGINE_TYPE engineType;

    // TODO remove pKernelChannel from params

    if (RM_ENGINE_TYPE_IS_VALID(kchannelGetEngineType(pKernelChannel)) &&
        !RM_ENGINE_TYPE_IS_GR(kchannelGetEngineType(pKernelChannel)))
    {
        return NV_FALSE;
    }

    NV_CHECK_OR_RETURN(LEVEL_INFO, kchannelIsRunlistSet(pGpu, pKernelChannel), NV_FALSE);

    runlistId = kchannelGetRunlistId(pKernelChannel);
    NV_ASSERT_OK(
        kfifoEngineInfoXlate_HAL(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
                                 ENGINE_INFO_TYPE_RUNLIST, runlistId,
                                 ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32 *) &engineType));

    NV_CHECK_OR_RETURN(LEVEL_INFO, RM_ENGINE_TYPE_IS_GR(engineType), NV_FALSE);

    return NV_TRUE;
}

/*!
 * @brief returns if the main context buffer has been allocated
 *
 * @return NV_TRUE if allocated, NV_FALSE otherwise
 */
NvBool
kgrctxIsMainContextAllocated_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    MEMORY_DESCRIPTOR *pMemDesc;
    return (kgrctxGetMainContextBuffer(pGpu, pKernelGraphicsContext, &pMemDesc) == NV_OK) &&
           (pMemDesc != NULL);
}

/*! Retrieve the memdesc containing the main ctx buffer */
NV_STATUS kgrctxGetMainContextBuffer_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    MEMORY_DESCRIPTOR **ppMemDesc
)
{
    ENGINE_CTX_DESCRIPTOR *pEngCtxDesc;
    NvU32 subDevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    RsResourceRef *pParentRef = RES_GET_REF(pKernelGraphicsContext)->pParentRef;
    KernelChannelGroupApi *pKernelChannelGroupApi;
    KernelChannelGroup *pKernelChannelGroup;

    *ppMemDesc = NULL;
    pKernelChannelGroupApi = dynamicCast(pParentRef->pResource, KernelChannelGroupApi);
    pKernelChannelGroup = pKernelChannelGroupApi->pKernelChannelGroup;
    pEngCtxDesc = pKernelChannelGroup->ppEngCtxDesc[subDevInst];

    if (pEngCtxDesc != NULL)
        *ppMemDesc = pEngCtxDesc->pMemDesc;

    return NV_OK;
}

/*!
 * @brief Retrieve information about the context buffers
 */
NV_STATUS
kgrctxGetCtxBuffers_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    NvU32 gfid,
    NvU32 bufferCount,
    MEMORY_DESCRIPTOR **ppBuffers,
    NvU32 *pCtxBufferType,
    NvU32 *pBufferCountOut,
    NvU32 *pFirstGlobalBuffer
)
{
    GR_GLOBALCTX_BUFFERS *pGlobalCtxBuffers = kgraphicsGetGlobalCtxBuffers(pGpu, pKernelGraphics, gfid);
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    MEMORY_DESCRIPTOR *pGrCtxBufferMemDesc;
    NvU32 bufferCountOut = 0;
    NvU32 i;

    NV_ASSERT_OK_OR_RETURN(
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

    // Get local context buffer memdesc.
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrctxGetMainContextBuffer(pGpu, pKernelGraphicsContext, &pGrCtxBufferMemDesc));

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                       pGrCtxBufferMemDesc != NULL,
                       NV_ERR_INVALID_OBJECT);

    NV_CHECK_OR_RETURN(LEVEL_INFO, bufferCountOut < bufferCount, NV_ERR_INVALID_ARGUMENT);
    pCtxBufferType[bufferCountOut] = NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_MAIN;
    ppBuffers[bufferCountOut++] = pGrCtxBufferMemDesc;

    // Get context patch buffer memdesc.
    NV_CHECK_OR_RETURN(LEVEL_SILENT,
                       pKernelGraphicsContextUnicast->ctxPatchBuffer.pMemDesc != NULL,
                       NV_ERR_INVALID_STATE);

    NV_CHECK_OR_RETURN(LEVEL_INFO, bufferCountOut < bufferCount, NV_ERR_INVALID_ARGUMENT);
    pCtxBufferType[bufferCountOut] = NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PATCH;
    ppBuffers[bufferCountOut++] = pKernelGraphicsContextUnicast->ctxPatchBuffer.pMemDesc;

    // Add PM ctxsw buffer if it's allocated.
    if (pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc != NULL)
    {
        NV_CHECK_OR_RETURN(LEVEL_INFO, bufferCountOut < bufferCount, NV_ERR_INVALID_ARGUMENT);
        pCtxBufferType[bufferCountOut] = NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PM;
        ppBuffers[bufferCountOut++] = pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc;
    }

    if (pFirstGlobalBuffer != NULL)
    {
        *pFirstGlobalBuffer = bufferCountOut;
    }

    // Add global buffers.
    for (i = 0; i < GR_GLOBALCTX_BUFFER_COUNT; i++)
    {
        MEMORY_DESCRIPTOR *pMemDesc = pGlobalCtxBuffers->memDesc[i];
        if (pMemDesc != NULL)
        {
            NV_CHECK_OR_RETURN(LEVEL_INFO, bufferCountOut < bufferCount, NV_ERR_INVALID_ARGUMENT);
            NV_ASSERT_OK(
                kgrctxGetGlobalContextBufferExternalId(i, &pCtxBufferType[bufferCountOut]));
            ppBuffers[bufferCountOut++] = pMemDesc;
        }
    }

    *pBufferCountOut = bufferCountOut;

    return NV_OK;
}

/*
 * @brief Get maximum context buffer count including global and local buffers
 *
 * @param[in]  pGpu
 * @param[in]  pKernelGraphicsContext
 * @param[in]  pKernelGraphics
 * @param[out] pBufferCount Max context buffer count
 */
NV_STATUS
kgrctxGetBufferCount_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    NvU32 *pBufferCount
)
{
    NvU32 gfid;
    NvBool bCallingContextPlugin;
    GR_GLOBALCTX_BUFFERS *pGlobalCtxBuffers;
    NvU32 i;

    *pBufferCount = 0;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));
    NV_ASSERT_OK_OR_RETURN(vgpuIsCallingContextPlugin(pGpu, &bCallingContextPlugin));

    pGlobalCtxBuffers = kgraphicsGetGlobalCtxBuffers(pGpu, pKernelGraphics, gfid);

    if (bCallingContextPlugin)
    {
        gfid = GPU_GFID_PF;
    }

    for (i = 0; i < GR_GLOBALCTX_BUFFER_COUNT; i++)
    {
        if (pGlobalCtxBuffers->memDesc[i] != NULL)
            (*pBufferCount)++;
    }

    // Increment by 3 to include local ctx buffer, patch context buffer and PM Ctxsw buffer
    *pBufferCount += 3;

    return NV_OK;
}

/*
 * @brief Get context buffer info like size, alignment for global and
 *        local buffers
 *
 * @param[in]  pGpu
 * @param[in]  pKernelGraphicsContext
 * @param[in]  pKernelGraphics
 * @param[in]  gfid
 * @param[in]  bufferMaxCount   Amount of space provided in pCtxBufferInfo
 * @param[out] bufferCount      Number of buffers described
 * @param[out] pCtxBufferInfo   Structure to fill buffer information
 *
 * @return NV_OK                    successfully probed all buffers
 * @return NV_ERR_INVALID_ARGUMENT  not enough space provided for buffers queried
 */
NV_STATUS
kgrctxGetCtxBufferInfo_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    NvU32 gfid,
    NvU32 bufferMaxCount,
    NvU32 *pBufferCount,
    NV2080_CTRL_GR_CTX_BUFFER_INFO *pCtxBufferInfo
)
{
    MEMORY_DESCRIPTOR *pMemDescArray[3 + GR_GLOBALCTX_BUFFER_COUNT];
    NvU32 bufferExternalId[3 + GR_GLOBALCTX_BUFFER_COUNT];
    NvU32 memdescCount;
    NvU32 firstGlobalBuffer;
    NvU32 i;
    NvU32 j;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrctxGetCtxBuffers(pGpu, pKernelGraphicsContext, pKernelGraphics,
                            gfid,
                            NV_ARRAY_ELEMENTS(pMemDescArray),
                            pMemDescArray,
                            bufferExternalId,
                            &memdescCount,
                            &firstGlobalBuffer));

    if (bufferMaxCount < memdescCount)
        return NV_ERR_INVALID_ARGUMENT;

    for (i = 0; i < memdescCount; i++)
    {
        NvBool bGlobalBuffer = (i >= firstGlobalBuffer);

        NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
            kgrctxFillCtxBufferInfo(pMemDescArray[i],
                                    bufferExternalId[i],
                                    bGlobalBuffer,
                                    &pCtxBufferInfo[i]));
    }

    //
    // Sort the buffer info in descending order using alignment so that the
    // VA range calculation can figure out the optimal VA range size.
    //
    for (i = 0; i < memdescCount; i++)
    {
        for (j = 0; j < memdescCount - 1; j++)
        {
            if (pCtxBufferInfo[j].alignment < pCtxBufferInfo[j + 1].alignment)
            {
                NV2080_CTRL_GR_CTX_BUFFER_INFO tmp;

                portMemCopy(&tmp, sizeof(tmp), &pCtxBufferInfo[j], sizeof(tmp));
                portMemCopy(&pCtxBufferInfo[j], sizeof(tmp), &pCtxBufferInfo[j + 1], sizeof(tmp));
                portMemCopy(&pCtxBufferInfo[j + 1], sizeof(tmp), &tmp, sizeof(tmp));
            }
        }
    }

    *pBufferCount = memdescCount;

    return NV_OK;
}

/*
 * @brief Return physical addresses of context buffer starting from its 'firstPage' page.
 *        The function is intended to be called repeatedly while advancing
 *        'firstPage' parameter until all pages of the buffer are queried.
 *        This condition is indicated by *pNoMorePage == NV_TRUE.
 *        If requested buffer is contiguous, address of first page is returned
 *        always.
 *
 * @param[in]  pGpu
 * @param[in]  pKernelGraphicsContext
 * @param[in]  pKernelGraphics
 * @param[in]  gfid
 * @param[in]  bufferType       Requested ctx buffer type
 * @param[in]  firstPage        First page of the buffer to be queried
 * @param[out] pPhysAddrs       Array to be filled with page addresses
 * @param[in]  addrsSize        Number of elements of pPhysAddrs
 * @param[out] pNumPages        Number of page addresses returned
 * @param[out] pbNoMorePages    End of buffer reached
 *
 * @return NV_OK                    successfully obtained requested addresses
 * @return NV_ERR_INVALID_ARGUMENT  pMemDesc does not specify a context buffer
 */
NV_STATUS
kgrctxGetCtxBufferPtes_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    NvU32 gfid,
    NvU32 bufferType,
    NvU32 firstPage,
    NvU64 *pPhysAddrs,
    NvU32 addrsSize,
    NvU32 *pNumPages,
    NvBool *pbNoMorePages
)
{
    NvU64 bufferSize;
    NvU64 pageSize;
    NvU32 numPages;
    NvU32 bufferCount;
    NvU32 i;
    MEMORY_DESCRIPTOR *pMemDescArray[3 + GR_GLOBALCTX_BUFFER_COUNT];
    NvU32 bufferExternalId[3 + GR_GLOBALCTX_BUFFER_COUNT];

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrctxGetCtxBuffers(pGpu, pKernelGraphicsContext, pKernelGraphics,
                            gfid,
                            NV_ARRAY_ELEMENTS(pMemDescArray),
                            pMemDescArray,
                            bufferExternalId,
                            &bufferCount,
                            NULL));

    for (i = 0; i < bufferCount; i++)
    {
        if (bufferExternalId[i] == bufferType)
            break;
    }

    if (addrsSize == 0)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_CHECK_OR_RETURN(LEVEL_ERROR, i != bufferCount, NV_ERR_INVALID_ARGUMENT);

    bufferSize = memdescGetSize(pMemDescArray[i]);
    pageSize = memdescGetPageSize(pMemDescArray[i], AT_GPU);

    if (pageSize == 0)
    {
        return NV_ERR_INVALID_STATE;
    }

    numPages = NV_ROUNDUP(bufferSize, pageSize) / pageSize;

    if (firstPage >= numPages)
    {
        numPages = 0;
        *pbNoMorePages = NV_TRUE;
        goto done;
    }

    if (memdescGetContiguity(pMemDescArray[i], AT_GPU))
    {
        firstPage = 0;
        numPages = 1;
        *pbNoMorePages = NV_TRUE;
    }
    else
    {
        numPages -= firstPage;
        *pbNoMorePages = (numPages <= addrsSize);
        numPages = NV_MIN(numPages, addrsSize);
    }

    if (numPages > 0)
    {
        memdescGetPhysAddrs(pMemDescArray[i],
                            AT_GPU,
                            firstPage * pageSize,
                            pageSize,
                            numPages,
                            pPhysAddrs);
    }

done:
    *pNumPages = numPages;

    return NV_OK;
}

/*!
 * This function does following things
 * 1. Allocate main GR context buffer
 * 2. Sets up GR context memory descriptor by calling kchannelSetEngineContextMemDesc
 */
NV_STATUS
kgrctxAllocMainCtxBuffer_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    KernelChannel *pKernelChannel
)
{
    MEMORY_DESCRIPTOR            *pGrCtxBufferMemDesc = NULL;
    NvU32                         ctxSize;
    MemoryManager                *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NvU64                         allocFlags = MEMDESC_FLAGS_GPU_PRIVILEGED;
    CTX_BUF_POOL_INFO            *pCtxBufPool = NULL;
    NvBool                        bIsContiguous = kgraphicsShouldForceMainCtxContiguity_HAL(pGpu, pKernelGraphics);
    const GR_BUFFER_ATTR         *pAttr = kgraphicsGetContextBufferAttr(pGpu, pKernelGraphics, GR_CTX_BUFFER_MAIN);

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    NV_ASSERT_OK_OR_RETURN(
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

    //
    // Allocate space for per-subcontext headers in the context buffer.
    // With subcontext, per-subcontext context header is programmed in the channel instance block.
    // Per-subcontext headers can be seperate from the context buffer.
    // For the initial phase, we allocate them at the end of the context buffers for easier tracking.
    // This will waste some memory (256 KB), if the number of subcontexts are sparse.
    // Will cleanup this up later to be on-demand.
    //
    // We also need to report the updated context size to KMD for virtual context.
    // Bug 1764102 tracks the VC support.
    //
    NV_ASSERT_OK_OR_RETURN(
        kgraphicsGetMainCtxBufferSize(pGpu, pKernelGraphics, NV_TRUE, &ctxSize));

    if (ctxBufPoolIsSupported(pGpu) &&
        pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->pCtxBufPool != NULL)
    {
        allocFlags |= MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL;
        pCtxBufPool = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->pCtxBufPool;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        memdescCreate(&pGrCtxBufferMemDesc, pGpu, ctxSize,
                      RM_PAGE_SIZE, bIsContiguous, ADDR_UNKNOWN,
                      pAttr->cpuAttr,
                      allocFlags | MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE));

    if (kgraphicsIsOverrideContextBuffersToGpuCached(pGpu, pKernelGraphics))
        memdescSetGpuCacheAttrib(pGrCtxBufferMemDesc, NV_MEMORY_CACHED);

    //
    // Force page size to 4KB, we can change this later when RM access method
    // support 64k pages
    //
    NV_ASSERT_OK_OR_RETURN(
        memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, pGrCtxBufferMemDesc, AT_GPU, RM_ATTR_PAGE_SIZE_4KB));

    NV_ASSERT_OK_OR_RETURN(memdescSetCtxBufPool(pGrCtxBufferMemDesc, pCtxBufPool));

    NV_STATUS status;
    memdescTagAllocList(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_CONTEXT_BUFFER, pGrCtxBufferMemDesc, pAttr->pAllocList);
    NV_ASSERT_OK_OR_RETURN(status);

    NV_ASSERT_OK_OR_RETURN(
        kchannelSetEngineContextMemDesc(pGpu, pKernelChannel,
                                        ENG_GR(kgraphicsGetInstance(pGpu, pKernelGraphics)),
                                        pGrCtxBufferMemDesc));
    pKernelGraphicsContextUnicast->pMainCtxBuffer = pGrCtxBufferMemDesc;
    return NV_OK;
}

/*!
 * @brief Allocate and setup the GR ctx patch buffer
 */
NV_STATUS
kgrctxAllocPatchBuffer_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    KernelChannel *pKernelChannel
)
{
    KernelGraphicsContextUnicast  *pKernelGraphicsContextUnicast;
    const KGRAPHICS_STATIC_INFO   *pStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NvU32                          status = NV_OK;
    MemoryManager                 *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    MEMORY_DESCRIPTOR            **ppMemDesc;
    NvU64                          size;
    NvU64                          flags = MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;
    CTX_BUF_POOL_INFO             *pCtxBufPool;
    const GR_BUFFER_ATTR          *pAttr = kgraphicsGetContextBufferAttr(pGpu, pKernelGraphics, GR_CTX_BUFFER_PATCH);

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    NV_ASSERT_OR_RETURN(pStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pStaticInfo->pContextBuffersInfo != NULL, NV_ERR_INVALID_STATE);

    NV_ASSERT_OK_OR_RETURN(
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

    pCtxBufPool = NULL;
    if (ctxBufPoolIsSupported(pGpu) &&
        (pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->pCtxBufPool) != NULL)
    {
        flags |= MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL;
        pCtxBufPool = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->pCtxBufPool;
    }

    ppMemDesc = &pKernelGraphicsContextUnicast->ctxPatchBuffer.pMemDesc;
    size = pStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PATCH].size;
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        memdescCreate(ppMemDesc, pGpu, size, RM_PAGE_SIZE, NV_TRUE,
                      ADDR_UNKNOWN,
                      pAttr->cpuAttr,
                      flags));

    if (kgraphicsIsOverrideContextBuffersToGpuCached(pGpu, pKernelGraphics))
        memdescSetGpuCacheAttrib(*ppMemDesc, NV_MEMORY_CACHED);

    //
    // Force page size to 4KB we can change this later when RM access method
    // support 64k pages
    //
    memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, *ppMemDesc, AT_GPU, RM_ATTR_PAGE_SIZE_4KB);
    NV_ASSERT_OK_OR_GOTO(status,
        memdescSetCtxBufPool(*ppMemDesc, pCtxBufPool),
        failed);

    memdescTagAllocList(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_117, 
                        (*ppMemDesc), pAttr->pAllocList);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        status,
        failed);

    return NV_OK;

failed:
    memdescFree(*ppMemDesc);
    memdescDestroy(*ppMemDesc);
    *ppMemDesc = NULL;
    return status;
}

/*!
 * @brief Allocate the local ctx PM buffer
 */
NV_STATUS
kgrctxAllocPmBuffer_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    KernelChannel *pKernelChannel
)
{
    KernelGraphicsContextUnicast    *pKernelGraphicsContextUnicast;
    const KGRAPHICS_STATIC_INFO     *pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NvU32                            size;
    NV_STATUS                        status = NV_OK;
    NvU64                            flags = MEMDESC_FLAGS_GPU_PRIVILEGED;
    MemoryManager                   *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    CTX_BUF_POOL_INFO               *pCtxBufPool;
    MEMORY_DESCRIPTOR              **ppMemDesc;
    const GR_BUFFER_ATTR            *pAttr = kgraphicsGetContextBufferAttr(pGpu, pKernelGraphics, GR_CTX_BUFFER_PM);

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    NV_ASSERT_OK_OR_RETURN(
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pContextBuffersInfo != NULL, NV_ERR_INVALID_STATE);
    size = pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PM].size;
    if (size == 0)
        return NV_ERR_INVALID_STATE;

    pCtxBufPool = NULL;
    if (ctxBufPoolIsSupported(pGpu) &&
        pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->pCtxBufPool != NULL)
    {
        flags |= MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL;
        pCtxBufPool = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->pCtxBufPool;
    }

    //
    // For SRIOV Heavy, the PM ctxsw buffer allocation will be redirected to
    // host RM subheap. Subheap is used by host RM to allocate memory
    // on behalf of the guest(VF) context.
    //
    if (gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        flags |= MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;
    }

    ppMemDesc = &pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc;
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        memdescCreate(ppMemDesc, pGpu,
                      size,
                      RM_PAGE_SIZE,
                      NV_TRUE,
                      ADDR_UNKNOWN,
                      pAttr->cpuAttr,
                      flags));

    if (kgraphicsIsOverrideContextBuffersToGpuCached(pGpu, pKernelGraphics))
        memdescSetGpuCacheAttrib(*ppMemDesc, NV_MEMORY_CACHED);

    //
    // Force page size to 4KB we can change this later when RM access method
    // support 64k pages
    //
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, *ppMemDesc, AT_GPU, RM_ATTR_PAGE_SIZE_4KB),
        error);

    NV_ASSERT_OK_OR_GOTO(status, memdescSetCtxBufPool(*ppMemDesc, pCtxBufPool), error);

    memdescTagAllocList(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_118, 
                        (*ppMemDesc), pAttr->pAllocList);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        status,
        error);

    return NV_OK;

error:
    memdescFree(*ppMemDesc);
    memdescDestroy(*ppMemDesc);
    *ppMemDesc = NULL;

    return status;
}

/*!
 * This function allocates and maps various GR buffers.
 */
NV_STATUS
kgrctxAllocCtxBuffers_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    KernelGraphicsObject *pKernelGraphicsObject
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    ChannelDescendant            *pChannelDescendant = staticCast(pKernelGraphicsObject, ChannelDescendant);
    NV_STATUS                     status = NV_OK;
    NvU32                         classNum = pChannelDescendant->resourceDesc.externalClassId;
    NvU32                         objType;
    NvU32                         gfid = kchannelGetGfid(pChannelDescendant->pKernelChannel);
    KernelChannel                *pKernelChannel = pChannelDescendant->pKernelChannel;
    GR_GLOBALCTX_BUFFERS         *pGlobalCtxBuffers = kgraphicsGetGlobalCtxBuffers(pGpu, pKernelGraphics, gfid);

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    NV_ASSERT_OR_RETURN(kgrobjGetKernelGraphicsContext(pGpu, pKernelGraphicsObject) != NULL, NV_ERR_INVALID_STATE);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrctxGetUnicast(pGpu,
                         kgrobjGetKernelGraphicsContext(pGpu, pKernelGraphicsObject),
                         &pKernelGraphicsContextUnicast));

    // Allocate the GR ctx buffer if required
    if (!kgrctxIsMainContextAllocated(pGpu, pKernelGraphicsContext) &&
        !kchannelIsCtxBufferAllocSkipped(pChannelDescendant->pKernelChannel))
    {
        if (pKernelGraphicsContextUnicast->pMainCtxBuffer == NULL)
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kgrctxAllocMainCtxBuffer(pGpu, pKernelGraphicsContext, pKernelGraphics, pKernelChannel));
        }
        else
        {
            // Ctx buffer was uninstalled in FIFO but not freed. Reinstall it
            NV_ASSERT_OK_OR_RETURN(
                kchannelSetEngineContextMemDesc(pGpu, pKernelChannel,
                                                ENG_GR(kgraphicsGetInstance(pGpu, pKernelGraphics)),
                                                pKernelGraphicsContextUnicast->pMainCtxBuffer));
        }
    }

    kgrmgrGetGrObjectType(classNum, &objType);

    //
    // for each channel, we need to allocate the context patch buffer, this memory region
    // will be used to override settings in the context after it is restored, things
    // like the global ctx buffer addresses, etc.
    //
    if (pKernelGraphicsContextUnicast->ctxPatchBuffer.pMemDesc == NULL)
    {
        NV_ASSERT_OK_OR_RETURN(
            kgrctxAllocPatchBuffer(pGpu, pKernelGraphicsContext, pKernelGraphics, pKernelChannel));
    }

    // Pre-allocate the PM ctxsw buffer, if required
    if (kgrctxShouldPreAllocPmBuffer_HAL(pGpu, pKernelGraphicsContext, pChannelDescendant->pKernelChannel))
    {
        NV_ASSERT_OK_OR_RETURN(
            kgrctxAllocPmBuffer(pGpu, kgrobjGetKernelGraphicsContext(pGpu, pKernelGraphicsObject),
                                pKernelGraphics,
                                pChannelDescendant->pKernelChannel));
    }

    //
    // Allocate Ctx Buffers that are local to this channel if required
    // and they have yet to be allocated.
    //
    if ((pKernelGraphicsContextUnicast->bVprChannel
        ) && !pKernelGraphicsContextUnicast->localCtxBuffer.bAllocated)
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgraphicsAllocGrGlobalCtxBuffers_HAL(pGpu, pKernelGraphics, gfid,
                                                 kgrobjGetKernelGraphicsContext(pGpu, pKernelGraphicsObject)));
    }

    // Allocate global context for this gfid if they haven't been already
    if (!pGlobalCtxBuffers->bAllocated)
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgraphicsAllocGrGlobalCtxBuffers_HAL(pGpu, pKernelGraphics, gfid, NULL));
    }

    return status;
}

// Map common buffer to a channel's context helper function
NV_STATUS
kgrctxMapGlobalCtxBuffer_IMPL
(
    OBJGPU                *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics        *pKernelGraphics,
    NvU32                  gfid,
    OBJVASPACE            *pVAS,
    GR_GLOBALCTX_BUFFER    buffId,
    NvBool                 bIsReadOnly
)
{
    GR_GLOBALCTX_BUFFERS *pGlobalCtxBuffers = kgraphicsGetGlobalCtxBuffers(pGpu, pKernelGraphics, gfid);
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NV_STATUS status = NV_OK;
    NvU64 vaddr;
    MEMORY_DESCRIPTOR *pMemDesc;

    NV_ASSERT_OK_OR_RETURN(
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

    // if we have local buffers per-channel allocated, use them, otherwise use the global buffers
    pMemDesc = pKernelGraphicsContextUnicast->localCtxBuffer.memDesc[buffId];
    if (pMemDesc == NULL)
    {
        pMemDesc = pGlobalCtxBuffers->memDesc[buffId];
    }

    if (pMemDesc == NULL)
    {
        const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;
        NvU32 fifoEngineId;
        NvU32 buffSize;

        pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
        NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pContextBuffersInfo != NULL, NV_ERR_INVALID_STATE);
        NV_ASSERT_OK_OR_RETURN(kgrctxGlobalCtxBufferToFifoEngineId(buffId, &fifoEngineId));
        buffSize = pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[fifoEngineId].size;

        if (buffSize == 0)
        {
            NV_PRINTF(LEVEL_INFO,
                      "Could not map %s Buffer as buffer is not supported\n",
                      NV_ENUM_TO_STRING(GR_GLOBALCTX_BUFFER, buffId));
            return NV_OK;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not map %s Buffer, no memory allocated for it!\n",
                      NV_ENUM_TO_STRING(GR_GLOBALCTX_BUFFER, buffId));
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    // Unconditionally call map for refcounting
    NV_ASSERT_OK_OR_ELSE(status,
        kgraphicsMapCtxBuffer(pGpu, pKernelGraphics, pMemDesc, pVAS,
                              &pKernelGraphicsContextUnicast->globalCtxBufferVaList[buffId],
                              kgraphicsIsGlobalCtxBufferSizeAligned(pGpu, pKernelGraphics, buffId),
                              bIsReadOnly),
        NV_PRINTF(LEVEL_ERROR, "%s Buffer could not be mapped\n",
                  NV_ENUM_TO_STRING(GR_GLOBALCTX_BUFFER, buffId));
        return status; );

    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
        vaListFindVa(&pKernelGraphicsContextUnicast->globalCtxBufferVaList[buffId], pVAS, &vaddr));

    NV_PRINTF(LEVEL_INFO,
              "GPU:%d %s Buffer PA @ 0x%llx VA @ 0x%llx of Size 0x%llx\n",
              pGpu->gpuInstance, NV_ENUM_TO_STRING(GR_GLOBALCTX_BUFFER, buffId),
              memdescGetPhysAddr(memdescGetMemDescFromGpu(pMemDesc, pGpu), AT_GPU, 0),
              vaddr, pMemDesc->Size);

    return status;
}

//
// Map common buffers to a channel's context
//
NV_STATUS
kgrctxMapGlobalCtxBuffers_IMPL
(
    OBJGPU                *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics        *pKernelGraphics,
    NvU32                  gfid,
    KernelChannel         *pKernelChannel
)
{
    GR_GLOBALCTX_BUFFERS *pGlobalCtxBuffers = kgraphicsGetGlobalCtxBuffers(pGpu, pKernelGraphics, gfid);
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;

    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    NV_ASSERT_OK_OR_RETURN(
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

    // if global ctx buffers were never allocated then bail out early
    if (!pKernelGraphicsContextUnicast->localCtxBuffer.bAllocated &&
        !pGlobalCtxBuffers->bAllocated)
    {
         return NV_ERR_INVALID_STATE;
    }

    kgrctxMapGlobalCtxBuffer(pGpu,
                             pKernelGraphicsContext,
                             pKernelGraphics,
                             gfid,
                             pKernelChannel->pVAS,
                             GR_GLOBALCTX_BUFFER_BUNDLE_CB,
                             NV_FALSE);
    kgrctxMapGlobalCtxBuffer(pGpu,
                             pKernelGraphicsContext,
                             pKernelGraphics,
                             gfid,
                             pKernelChannel->pVAS,
                             GR_GLOBALCTX_BUFFER_ATTRIBUTE_CB,
                             NV_FALSE);
    kgrctxMapGlobalCtxBuffer(pGpu,
                             pKernelGraphicsContext,
                             pKernelGraphics,
                             gfid,
                             pKernelChannel->pVAS,
                             GR_GLOBALCTX_BUFFER_PAGEPOOL,
                             NV_FALSE);

    if (kgraphicsIsRtvCbSupported(pGpu, pKernelGraphics))
    {
        kgrctxMapGlobalCtxBuffer(pGpu,
                                 pKernelGraphicsContext,
                                 pKernelGraphics,
                                 gfid,
                                 pKernelChannel->pVAS,
                                 GR_GLOBALCTX_BUFFER_RTV_CB,
                                 NV_FALSE);
     }

    return NV_OK;
}

/*!
 * @brief This function allocates and maps various GR buffers.
 */
NV_STATUS
kgrctxMapCtxBuffers_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    KernelGraphicsObject *pKernelGraphicsObject
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    ChannelDescendant            *pChannelDescendant = staticCast(pKernelGraphicsObject, ChannelDescendant);
    KernelChannel                *pKernelChannel = pChannelDescendant->pKernelChannel;
    NvU32                         classNum = pChannelDescendant->resourceDesc.externalClassId;
    OBJGVASPACE                  *pGVAS;
    NvU32                         objType;
    NvU32                         gfid = kchannelGetGfid(pKernelChannel);
    NvBool                        bAcquire3d = NV_FALSE;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    NV_ASSERT_OK_OR_RETURN(
        kgrctxGetUnicast(pGpu, kgrobjGetKernelGraphicsContext(pGpu, pKernelGraphicsObject), &pKernelGraphicsContextUnicast));

    pGVAS = dynamicCast(pKernelChannel->pVAS, OBJGVASPACE);
    if (gvaspaceIsExternallyOwned(pGVAS))
        return NV_OK;

    //
    // in kernel RM we skip the context buffer mapping as part of
    // fifoSetUpChannelDma. fifoSetUpChannelDma is not enabled in kernel RM.
    // So it's possible for the main ctx buffer to be unmapped by this point.
    //
    if (!kchannelIsCtxBufferAllocSkipped(pKernelChannel))
    {
        ENGINE_CTX_DESCRIPTOR *pEngCtx;
        MEMORY_DESCRIPTOR *pMemDesc;
        NvU32 subdevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

        pEngCtx = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->ppEngCtxDesc[subdevInst];

        NV_ASSERT_OR_RETURN(pEngCtx != NULL, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(pEngCtx->pMemDesc != NULL, NV_ERR_INVALID_STATE);
        pMemDesc = pEngCtx->pMemDesc;

        NV_ASSERT_OK_OR_RETURN(
            kgraphicsMapCtxBuffer(pGpu, pKernelGraphics, pMemDesc, pKernelChannel->pVAS,
                                  &pEngCtx->vaList, NV_FALSE, NV_FALSE));
    }

    kgrmgrGetGrObjectType(classNum, &objType);

    NV_ASSERT_OK_OR_RETURN(
        kgraphicsMapCtxBuffer(pGpu, pKernelGraphics, pKernelGraphicsContextUnicast->ctxPatchBuffer.pMemDesc, pKernelChannel->pVAS,
                              &pKernelGraphicsContextUnicast->ctxPatchBuffer.vAddrList, NV_FALSE, NV_FALSE));

    if (pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc != NULL)
    {
        NV_ASSERT_OK_OR_RETURN(
            kgraphicsMapCtxBuffer(pGpu, pKernelGraphics, pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc, pKernelChannel->pVAS,
                                  &pKernelGraphicsContextUnicast->pmCtxswBuffer.vAddrList, NV_FALSE, NV_FALSE));
    }

    if (kgraphicsDoesUcodeSupportPrivAccessMap(pGpu, pKernelGraphics))
    {
        kgrctxMapGlobalCtxBuffer(pGpu,
                                 pKernelGraphicsContext,
                                 pKernelGraphics,
                                 gfid,
                                 pKernelChannel->pVAS,
                                 kgrctxGetRegisterAccessMapId_HAL(pGpu, pKernelGraphicsContext, pChannelDescendant->pKernelChannel),
                                 NV_TRUE);
    }

    //
    // Condition for acquiring 3d context buffer mappings.
    // For non-TSG & legacy TSG mode, always map
    // For subcontext TSG case, only map on 2D/3D allocations
    //
    if (pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->bLegacyMode)
    {
        bAcquire3d = NV_TRUE;
    }
    else
    {
        bAcquire3d = ((objType == GR_OBJECT_TYPE_2D) || (objType == GR_OBJECT_TYPE_3D));
    }

    if (bAcquire3d)
    {
        kgrctxMapGlobalCtxBuffers(pGpu, pKernelGraphicsContext, pKernelGraphics, gfid, pKernelChannel);
    }

    return NV_OK;
}

/*!
 * @brief Set parameters for promoting the PA of a ctx buffer to physical RM.
 */
NV_STATUS
kgrctxPrepareInitializeCtxBuffer_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    KernelChannel *pKernelChannel,
    NvU32 externalId,
    NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ENTRY *pEntry,
    NvBool *pbAddEntry
)
{
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NvU32 physAttr;

    *pbAddEntry = NV_FALSE;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);

    NV_ASSERT_OK_OR_RETURN(
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

    switch (externalId)
    {
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_MAIN:
        {
            ENGINE_CTX_DESCRIPTOR *pEngCtx;
            NvU32 subdevInst;
            if (pKernelGraphicsContextUnicast->bKGrMainCtxBufferInitialized)
                return NV_OK;

            // Do not try to init a KMD virtual context buffer
            if (kchannelIsCtxBufferAllocSkipped(pKernelChannel))
                return NV_OK;

            subdevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
            pEngCtx = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->ppEngCtxDesc[subdevInst];

            NV_ASSERT_OR_RETURN(pEngCtx != NULL, NV_ERR_INVALID_STATE);
            NV_ASSERT_OR_RETURN(pEngCtx->pMemDesc != NULL, NV_ERR_INVALID_STATE);
            pMemDesc = pEngCtx->pMemDesc;
            break;
        }
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PM:
            if (pKernelGraphicsContextUnicast->bKGrPmCtxBufferInitialized)
                return NV_OK;

            if (pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc == NULL)
                return NV_OK;

            pMemDesc = pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PATCH:
            if (pKernelGraphicsContextUnicast->bKGrPatchCtxBufferInitialized)
                return NV_OK;

            NV_ASSERT(pKernelGraphicsContextUnicast->ctxPatchBuffer.pMemDesc != NULL);
            pMemDesc = pKernelGraphicsContextUnicast->ctxPatchBuffer.pMemDesc;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_BUFFER_BUNDLE_CB:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PAGEPOOL:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_ATTRIBUTE_CB:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_RTV_CB_GLOBAL:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GFXP_POOL:
            // No initialization from kernel RM
            return NV_OK;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GFXP_CTRL_BLK:
        {
            GR_GLOBALCTX_BUFFER internalId;
            NvU32 gfid = kchannelGetGfid(pKernelChannel);
            GR_GLOBALCTX_BUFFERS *pCtxBuffers;

            NV_ASSERT_OK_OR_RETURN(
                kgrctxGetGlobalContextBufferInternalId(externalId, &internalId));

            if (pKernelGraphicsContextUnicast->localCtxBuffer.bAllocated)
            {
                pCtxBuffers = &pKernelGraphicsContextUnicast->localCtxBuffer;
            }
            else
            {
                pCtxBuffers = kgraphicsGetGlobalCtxBuffers(pGpu, pKernelGraphics, gfid);
            }

            if (pCtxBuffers->bInitialized[internalId])
                return NV_OK;

            pMemDesc = pCtxBuffers->memDesc[internalId];
            break;
        }
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_FECS_EVENT:
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PRIV_ACCESS_MAP:
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_UNRESTRICTED_PRIV_ACCESS_MAP:
        {
            GR_GLOBALCTX_BUFFER internalId;
            NvU32 gfid = kchannelGetGfid(pKernelChannel);
            GR_GLOBALCTX_BUFFERS *pCtxBuffers;

            NV_ASSERT_OK_OR_RETURN(
                kgrctxGetGlobalContextBufferInternalId(externalId, &internalId));

            pCtxBuffers = kgraphicsGetGlobalCtxBuffers(pGpu, pKernelGraphics, gfid);

            if (pCtxBuffers->bInitialized[internalId])
                return NV_OK;

            pMemDesc = pCtxBuffers->memDesc[internalId];

            break;
        }
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GLOBAL_PRIV_ACCESS_MAP:
            // No initialization from kernel RM
            return NV_OK;
        default:
            NV_ASSERT_OR_RETURN(!"Unrecognized promote ctx enum", NV_ERR_INVALID_ARGUMENT);
    }

    // If these buffers are not supported or not allocated, no need to init
    if (pMemDesc == NULL)
        return NV_OK;

    physAttr = 0x0;
    switch (memdescGetAddressSpace(pMemDesc))
    {
        case ADDR_FBMEM:
            physAttr = FLD_SET_DRF(2080, _CTRL_GPU_INITIALIZE_CTX,
                       _APERTURE, _VIDMEM, physAttr);
            break;

        case ADDR_SYSMEM:
            if (memdescGetCpuCacheAttrib(pMemDesc) == NV_MEMORY_CACHED)
            {
                physAttr = FLD_SET_DRF(2080, _CTRL_GPU_INITIALIZE_CTX,
                           _APERTURE, _COH_SYS, physAttr);
            }
            else if (memdescGetCpuCacheAttrib(pMemDesc) == NV_MEMORY_UNCACHED)
            {
                physAttr = FLD_SET_DRF(2080, _CTRL_GPU_INITIALIZE_CTX,
                           _APERTURE, _NCOH_SYS, physAttr);
            }
            else
            {
                return NV_ERR_INVALID_STATE;
            }
            break;

        default:
            return NV_ERR_INVALID_STATE;
    }

    physAttr = FLD_SET_DRF(2080, _CTRL_GPU_INITIALIZE_CTX, _GPU_CACHEABLE, _NO, physAttr);

    pEntry->gpuPhysAddr = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
    pEntry->size        = pMemDesc->Size;
    pEntry->physAttr    = physAttr;
    pEntry->bufferId    = externalId;
    pEntry->bInitialize = NV_TRUE;
    pEntry->bNonmapped  = NV_TRUE;

    *pbAddEntry = NV_TRUE;

    return NV_OK;
}

/*!
 * @brief Set parameters for promoting the VA of a ctx buffer to physical RM.
 */
NV_STATUS
kgrctxPreparePromoteCtxBuffer_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelChannel *pKernelChannel,
    NvU32 externalId,
    NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ENTRY *pEntry,
    NvBool *pbAddEntry
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    VA_LIST *pVaList = NULL;
    NvU64 vaddr;
    NvU64 refCount;
    OBJGVASPACE *pGVAS = dynamicCast(pKernelChannel->pVAS, OBJGVASPACE);
    NV_STATUS status;

    NV_ASSERT_OK_OR_RETURN(
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

    *pbAddEntry = NV_FALSE;

    // RM is not responsible for promoting the buffers when UVM is enabled
    if (gvaspaceIsExternallyOwned(pGVAS))
        return NV_OK;

    switch (externalId)
    {
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_MAIN:
        {
            ENGINE_CTX_DESCRIPTOR *pEngCtx;
            NvU32 subdevInst;

            // Do not try to promote a KMD virtual context buffer
            if (kchannelIsCtxBufferAllocSkipped(pKernelChannel))
                return NV_OK;

            subdevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
            pEngCtx = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->ppEngCtxDesc[subdevInst];

            NV_ASSERT_OR_RETURN(pEngCtx != NULL, NV_ERR_INVALID_STATE);
            pVaList = &pEngCtx->vaList;
            break;
        }
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PM:
            pVaList = &pKernelGraphicsContextUnicast->pmCtxswBuffer.vAddrList;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PATCH:
            pVaList = &pKernelGraphicsContextUnicast->ctxPatchBuffer.vAddrList;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_BUFFER_BUNDLE_CB:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PAGEPOOL:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_ATTRIBUTE_CB:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_RTV_CB_GLOBAL:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GFXP_POOL:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GFXP_CTRL_BLK:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_FECS_EVENT:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PRIV_ACCESS_MAP:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_UNRESTRICTED_PRIV_ACCESS_MAP:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GLOBAL_PRIV_ACCESS_MAP:
        {
            GR_GLOBALCTX_BUFFER internalId;

            NV_ASSERT_OK_OR_RETURN(
                kgrctxGetGlobalContextBufferInternalId(externalId, &internalId));

            pVaList = &pKernelGraphicsContextUnicast->globalCtxBufferVaList[internalId];
            break;
        }
        default:
            NV_ASSERT_OR_RETURN(!"Unrecognized promote ctx enum", NV_ERR_INVALID_ARGUMENT);
    }

    // If the buffer isn't currently mapped or was already promoted, nothing to do
    status = vaListGetRefCount(pVaList, pKernelChannel->pVAS, &refCount);
    if ((status != NV_OK) || (refCount > 1))
        return NV_OK;

    NV_ASSERT_OK_OR_RETURN(vaListFindVa(pVaList, pKernelChannel->pVAS, &vaddr));

    pEntry->bufferId    = externalId;
    pEntry->gpuVirtAddr = vaddr;
    pEntry->bNonmapped  = NV_FALSE;

    *pbAddEntry = NV_TRUE;
    return NV_OK;
}

/*! Mark the context buffer as initialized, prevent further calls to init */
void
kgrctxMarkCtxBufferInitialized_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    KernelChannel *pKernelChannel,
    NvU32 externalId
)
{
    NV_STATUS status;
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu,
                         pKernelGraphicsContext,
                         &pKernelGraphicsContextUnicast),
        return;);

    switch (externalId)
    {
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_MAIN:
            pKernelGraphicsContextUnicast->bKGrMainCtxBufferInitialized = NV_TRUE;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PM:
            pKernelGraphicsContextUnicast->bKGrPmCtxBufferInitialized = NV_TRUE;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PATCH:
            pKernelGraphicsContextUnicast->bKGrPatchCtxBufferInitialized = NV_TRUE;
            break;
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_BUFFER_BUNDLE_CB:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PAGEPOOL:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_ATTRIBUTE_CB:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_RTV_CB_GLOBAL:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GFXP_POOL:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GFXP_CTRL_BLK:
        {
            // If "local" global ctx buffers are allocated, check those first
            if (pKernelGraphicsContextUnicast->localCtxBuffer.bAllocated)
            {
                GR_GLOBALCTX_BUFFER internalId;
                GR_GLOBALCTX_BUFFERS *pKCtxBuffers;

                NV_ASSERT_OK_OR_ELSE(status,
                    kgrctxGetGlobalContextBufferInternalId(externalId, &internalId),
                    return;);

                pKCtxBuffers = &pKernelGraphicsContextUnicast->localCtxBuffer;
                pKCtxBuffers->bInitialized[internalId] = NV_TRUE;
                break;
            }
            // fall-through
        }
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_FECS_EVENT:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PRIV_ACCESS_MAP:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_UNRESTRICTED_PRIV_ACCESS_MAP:
            // fall-through
        case NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GLOBAL_PRIV_ACCESS_MAP:
        {
            GR_GLOBALCTX_BUFFER internalId;
            NvU32 gfid = kchannelGetGfid(pKernelChannel);
            GR_GLOBALCTX_BUFFERS *pKCtxBuffers;

            NV_ASSERT_OK_OR_ELSE(status,
                kgrctxGetGlobalContextBufferInternalId(externalId, &internalId),
                return;);

            // TODO XXX Make context buffers available from KGRCTX alone
            pKCtxBuffers = kgraphicsGetGlobalCtxBuffers(pGpu, pKernelGraphics, gfid);

            pKCtxBuffers->bInitialized[internalId] = NV_TRUE;
            break;
        }
        default:
            NV_ASSERT(!"Unrecognized promote ctx enum");
    }
}

/*! Non-UVM late bind PM ctx buffer */
NV_STATUS
kgrctxSetupDeferredPmBuffer_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    KernelChannel *pKernelChannel
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    OBJGVASPACE *pGVAS = dynamicCast(pKernelChannel->pVAS, OBJGVASPACE);
    NV_STATUS status = NV_OK;
    Device *pDevice = GPU_RES_GET_DEVICE(pKernelChannel);
    Subdevice *pSubdevice;

    NV_ASSERT_OK_OR_RETURN(
        subdeviceGetByInstance(
            RES_GET_CLIENT(pKernelChannel),
            RES_GET_HANDLE(pDevice),
            gpumgrGetSubDeviceInstanceFromGpu(pGpu),
            &pSubdevice));

    GPU_RES_SET_THREAD_BC_STATE(pSubdevice);

    NV_ASSERT_OK_OR_RETURN(
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

    if (gvaspaceIsExternallyOwned(pGVAS))
    {
        //
        // The PM Ctxsw buffer is now pre-allocated along with the other context buffers
        // This is done as a WAR for the issue tracked under bug 1760699
        // Reason: With the enablement of UVM8 by default, the UVM channel registration relies on the
        // buffers being allocated at channel allocation time and are not going to work for a buffer
        // created later with an rm ctrl
        //
        NV_ASSERT_OR_RETURN(pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc != NULL, NV_ERR_INVALID_STATE);
        return NV_OK;
    }
    else if (pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc != NULL)
    {
        return NV_OK;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrctxAllocPmBuffer(pGpu, pKernelGraphicsContext, pKernelGraphics, pKernelChannel));

    //
    // !!!
    // From this point on, use `goto failed` to exit
    // !!!
    //

    {
        RS_ORDERED_ITERATOR it;
        RsResourceRef *pScopeRef = RES_GET_REF(pKernelChannel);

        // Iterate over all channels in this TSG and map the new buffer
        if (!pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->bAllocatedByRm &&
            kgraphicsIsPerSubcontextContextHeaderSupported(pGpu, pKernelGraphics))
        {
            pScopeRef = RES_GET_REF(pKernelChannel->pKernelChannelGroupApi);
        }

        it = kchannelGetIter(RES_GET_CLIENT(pKernelChannel), pScopeRef);
        while (clientRefOrderedIterNext(it.pClient, &it))
        {
            NvU64 vaddr;
            KernelChannel *pLoopKernelChannel = dynamicCast(it.pResourceRef->pResource, KernelChannel);

            NV_ASSERT_OR_ELSE(pLoopKernelChannel != NULL,
                              status = NV_ERR_INVALID_STATE;
                              goto failed;);

            if ((vaListFindVa(&pKernelGraphicsContextUnicast->ctxPatchBuffer.vAddrList, pLoopKernelChannel->pVAS, &vaddr) == NV_OK) &&
                (vaListFindVa(&pKernelGraphicsContextUnicast->pmCtxswBuffer.vAddrList, pLoopKernelChannel->pVAS, &vaddr) != NV_OK))
            {
                NvU64 refCount;
                NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS params;
                RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
                NvBool bInitialize;
                NvBool bPromote;

                NV_ASSERT_OK_OR_GOTO(status,
                    kgraphicsMapCtxBuffer(pGpu, pKernelGraphics,
                                          pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc,
                                          pLoopKernelChannel->pVAS,
                                          &pKernelGraphicsContextUnicast->pmCtxswBuffer.vAddrList,
                                          NV_FALSE,
                                          NV_FALSE),
                    failed);

                // Promote the vaddr for each unique VAS
                portMemSet(&params, 0, sizeof(params));

                // Setup parameters to initialize the PA if necessary
                NV_ASSERT_OK_OR_GOTO(status,
                    kgrctxPrepareInitializeCtxBuffer(pGpu,
                                                     pKernelGraphicsContext,
                                                     pKernelGraphics,
                                                     pKernelChannel,
                                                     NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PM,
                                                     &params.promoteEntry[0],
                                                     &bInitialize),
                    failed);

                // Setup parameters to promote the VA if necessary
                NV_ASSERT_OK_OR_GOTO(status,
                    kgrctxPreparePromoteCtxBuffer(pGpu,
                                                  pKernelGraphicsContext,
                                                  pKernelChannel,
                                                  NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PM,
                                                  &params.promoteEntry[0],
                                                  &bPromote),
                    failed);

                NV_ASSERT_OR_ELSE(bInitialize || bPromote,
                                  status = NV_ERR_INVALID_STATE;
                                  goto failed;);

                params.engineType  = NV2080_ENGINE_TYPE_GR(kgraphicsGetInstance(pGpu, pKernelGraphics));
                params.hChanClient = RES_GET_CLIENT_HANDLE(pKernelChannel);
                params.hObject     = RES_GET_HANDLE(pKernelChannel);
                params.entryCount  = 1;

                NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                    pRmApi->Control(pRmApi,
                                    RES_GET_CLIENT_HANDLE(pSubdevice),
                                    RES_GET_HANDLE(pSubdevice),
                                    NV2080_CTRL_CMD_GPU_PROMOTE_CTX,
                                    &params,
                                    sizeof(params)),
                    failed);

                //
                // If we successfully promoted the PA, flip a flag to ensure we don't
                // try to promote it again. The VA_LIST should already track this for
                // VA, but we can't rely on it for PA due to UVM.
                //
                if (params.promoteEntry[0].bInitialize)
                {
                    kgrctxMarkCtxBufferInitialized(pGpu,
                                                   pKernelGraphicsContext,
                                                   pKernelGraphics,
                                                   pKernelChannel,
                                                   params.promoteEntry[0].bufferId);
                }

                // Update the refcount for this buffer now that we've promoted it
                NV_ASSERT_OK_OR_GOTO(status,
                    vaListGetRefCount(&pKernelGraphicsContextUnicast->ctxPatchBuffer.vAddrList, pLoopKernelChannel->pVAS, &refCount),
                    failed);
                NV_ASSERT_OK_OR_GOTO(status,
                    vaListSetRefCount(&pKernelGraphicsContextUnicast->pmCtxswBuffer.vAddrList, pLoopKernelChannel->pVAS, refCount),
                    failed);
            }
        }
    }

    return NV_OK;

failed:

    if (kgraphicsIsPerSubcontextContextHeaderSupported(pGpu, pKernelGraphics))
    {
        RS_ORDERED_ITERATOR it;
        RsResourceRef *pScopeRef = RES_GET_REF(pKernelChannel);

        // Iterate over all channels in this TSG and try to unmap the PM ctx buffer
        if (!pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->bAllocatedByRm &&
            kgraphicsIsPerSubcontextContextHeaderSupported(pGpu, pKernelGraphics))
        {
            pScopeRef = RES_GET_REF(pKernelChannel->pKernelChannelGroupApi);
        }

        it = kchannelGetIter(RES_GET_CLIENT(pKernelChannel), pScopeRef);
        while (clientRefOrderedIterNext(it.pClient, &it))
        {
            NvU64 vaddr;
            KernelChannel *pLoopKernelChannel = dynamicCast(it.pResourceRef->pResource, KernelChannel);
            if (pLoopKernelChannel == NULL)
                continue;

            while (vaListFindVa(&pKernelGraphicsContextUnicast->pmCtxswBuffer.vAddrList, pLoopKernelChannel->pVAS, &vaddr) == NV_OK)
            {
                kgraphicsUnmapCtxBuffer(pGpu, pKernelGraphics,
                                        pLoopKernelChannel->pVAS,
                                        &pKernelGraphicsContextUnicast->pmCtxswBuffer.vAddrList);
            }
        }
    }

    kgrctxFreePmBuffer(pGpu, pKernelGraphicsContext);

    return status;
}

/**
 * @brief unmap the memory for the pm context buffer associated with a channel
 */
void
kgrctxUnmapCtxPmBuffer_IMPL
(
    OBJGPU                *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics        *pKernelGraphics,
    OBJVASPACE            *pVAS
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NV_STATUS status;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast),
        return;);

    kgraphicsUnmapCtxBuffer(pGpu, pKernelGraphics, pVAS, &pKernelGraphicsContextUnicast->pmCtxswBuffer.vAddrList);
}

/**
 * @brief unmap the memory for the zcull context buffer
 */
void
kgrctxUnmapCtxZcullBuffer_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    OBJVASPACE *pVAS
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    MEMORY_DESCRIPTOR *pMemDesc;
    VA_LIST *pVaList;
    NvU64 vaddr;
    NV_STATUS status;

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast),
        return;);

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    pMemDesc = pKernelGraphicsContextUnicast->zcullCtxswBuffer.pMemDesc;
    pVaList = &pKernelGraphicsContextUnicast->zcullCtxswBuffer.vAddrList;
    if (pMemDesc != NULL)
    {
        //
        // This func assumes that the buffer was not allocated per subdevice,
        // and will leak any mappings performed by the secondaries.
        //
        NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));

        if (vaListFindVa(pVaList, pVAS, &vaddr) == NV_OK)
        {
            dmaUnmapBuffer_HAL(pGpu, GPU_GET_DMA(pGpu), pVAS, vaddr);
            vaListRemoveVa(pVaList, pVAS);
        }
    }
    else if (vaListFindVa(pVaList, pVAS, &vaddr) == NV_OK)
    {
        // Zcull buffer mapped by client. Remove the VA here
        vaListRemoveVa(pVaList, pVAS);
    }
}

/**
 * @brief unmap the memory for the preemption context buffers
 */
void
kgrctxUnmapCtxPreemptionBuffers_IMPL
(
    OBJGPU                *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics        *pKernelGraphics,
    OBJVASPACE            *pVAS
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NV_STATUS status;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast),
        return;);

    kgraphicsUnmapCtxBuffer(pGpu, pKernelGraphics, pVAS, &pKernelGraphicsContextUnicast->preemptCtxswBuffer.vAddrList);

    kgraphicsUnmapCtxBuffer(pGpu, pKernelGraphics, pVAS, &pKernelGraphicsContextUnicast->spillCtxswBuffer.vAddrList);

    kgraphicsUnmapCtxBuffer(pGpu, pKernelGraphics, pVAS, &pKernelGraphicsContextUnicast->betaCBCtxswBuffer.vAddrList);

    kgraphicsUnmapCtxBuffer(pGpu, pKernelGraphics, pVAS, &pKernelGraphicsContextUnicast->pagepoolCtxswBuffer.vAddrList);

    kgraphicsUnmapCtxBuffer(pGpu, pKernelGraphics, pVAS, &pKernelGraphicsContextUnicast->rtvCbCtxswBuffer.vAddrList);
	
    if (kgraphicsGetPeFiroBufferEnabled(pGpu, pKernelGraphics))
        kgraphicsUnmapCtxBuffer(pGpu, pKernelGraphics, pVAS, &pKernelGraphicsContextUnicast->setupCtxswBuffer.vAddrList);
}

/**
 * @brief Unmap associated ctx buffers (main, patch, global buffers etc).
 *
 * This function is called on every channel free and therefore can't assume any
 * graphics objects actually exist on the given channel.
 * TODO: Bug 3164256
 * This function also unmaps and frees zcull and preempt buffers. Ideally we want to decouple
 * unmap and free for all buffers and move free of buffers to kgrctxFreeAssociatedCtxBuffers
 */
void
kgrctxUnmapAssociatedCtxBuffers_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    KernelChannel *pKernelChannel
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NvBool bRelease3d  = NV_FALSE;
    NvU32 gfid = kchannelGetGfid(pKernelChannel);
    NvU32 status;
    GR_GLOBALCTX_BUFFER registerMapBufferId = kgrctxGetRegisterAccessMapId_HAL(pGpu, pKernelGraphicsContext, pKernelChannel);

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu,
                         pKernelGraphicsContext,
                         &pKernelGraphicsContextUnicast),
        return;);

    //
    // Unmap 3D context buffers on the last 3D channel free
    //
    // Condition for unmapping 3d context buffer mappings:
    // For non-TSG & legacy TSG mode, always unmap 3d context buffers.
    // For subcontext TSG case, unmap on the last graphics channel free.
    //
    if (pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->bLegacyMode)
    {
        bRelease3d = pKernelGraphicsContextUnicast->channelObjects == 0;
    }
    else
    {
        bRelease3d = ((pKernelGraphicsContextUnicast->objectCounts[GR_OBJECT_TYPE_3D] == 0) &&
            (pKernelGraphicsContextUnicast->objectCounts[GR_OBJECT_TYPE_2D] == 0));
    }

    if (bRelease3d)
    {
        // Unmap Circular buffer from the current channel's address space
        kgrctxUnmapGlobalCtxBuffers(pGpu, pKernelGraphicsContext, pKernelGraphics, pKernelChannel->pVAS, gfid);
    }

    //
    // If multiple channels sharing the same VAS exist, it is possible both
    // channels could be using these mappings, and we must wait for both
    // channels to be detached before we remove them.
    //
    if ((!pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->bAllocatedByRm) &&
        (pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->pChanList != NULL))
    {
        CHANNEL_NODE *pChanNode;
        CHANNEL_LIST *pChanList;

        pChanList = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->pChanList;

        for (pChanNode = pChanList->pHead; pChanNode; pChanNode = pChanNode->pNext)
        {
            // Skip the channel we are looking to unmap
            if (kchannelGetDebugTag(pKernelChannel) == kchannelGetDebugTag(pChanNode->pKernelChannel))
                continue;

            NV_CHECK_OR_RETURN_VOID(LEVEL_SILENT, pChanNode->pKernelChannel->pVAS != pKernelChannel->pVAS);
        }
    }

    // Only unmap once the last channel using this VAS has gone
    if (kgraphicsGetGlobalCtxBuffers(pGpu, pKernelGraphics, gfid)->memDesc[GR_GLOBALCTX_BUFFER_FECS_EVENT] != NULL)
    {
        kgrctxUnmapGlobalCtxBuffer(pGpu, pKernelGraphicsContext, pKernelGraphics, pKernelChannel->pVAS, GR_GLOBALCTX_BUFFER_FECS_EVENT);
    }

    if (pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc != NULL)
    {
        kgrctxUnmapCtxPmBuffer(pGpu, pKernelGraphicsContext, pKernelGraphics, pKernelChannel->pVAS);
    }

    if (kgraphicsDoesUcodeSupportPrivAccessMap(pGpu, pKernelGraphics))
    {
        kgrctxUnmapGlobalCtxBuffer(pGpu,
                                   pKernelGraphicsContext,
                                   pKernelGraphics,
                                   pKernelChannel->pVAS,
                                   registerMapBufferId);
    }

    if (pKernelGraphicsContextUnicast->ctxPatchBuffer.pMemDesc != NULL)
    {
        //
        // Delay freeing the patch buffer until the last channel free.
        // these buffers are accessed even after the last GR object is freed.
        //
        kgraphicsUnmapCtxBuffer(pGpu, pKernelGraphics, pKernelChannel->pVAS, &pKernelGraphicsContextUnicast->ctxPatchBuffer.vAddrList);
    }

    kgrctxUnmapCtxZcullBuffer(pGpu, pKernelGraphicsContext, pKernelGraphics, pKernelChannel->pVAS);

    // Release all preemption buffers if they were allocated
    kgrctxUnmapCtxPreemptionBuffers(pGpu, pKernelGraphicsContext, pKernelGraphics, pKernelChannel->pVAS);

    //
    // Cleanup lingering main ctx buffer mappings for this VAS
    // TODO fix main ctx buffer refcounting
    // TODO move setEngineContextMemDesc to FreeMainCtxBuffer, move this loop
    //      inside UnmapMainCtxBuffer
    //
    if (pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->ppEngCtxDesc[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] != NULL)
    {
        NvU32 subdevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
        VA_LIST *pVAddrList = &pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->ppEngCtxDesc[subdevInst]->vaList;
        NvU64 vaddr;

        while (vaListFindVa(pVAddrList, pKernelChannel->pVAS, &vaddr) == NV_OK)
            kgraphicsUnmapCtxBuffer(pGpu, pKernelGraphics, pKernelChannel->pVAS, pVAddrList);
    }

    //
    // When sharing contexts across channels we need to defer this until all
    // objects have been freed.
    //
    NV_CHECK_OR_RETURN_VOID(LEVEL_SILENT,
        pKernelGraphicsContextUnicast->channelObjects == 0);

    kgrctxUnmapMainCtxBuffer(pGpu, pKernelGraphicsContext, pKernelGraphics, pKernelChannel);
}

/*!
 * @brief is this object responsible for any cleanup tasks i.e. buffer unmapping?
 */
NvBool kgrctxShouldCleanup_KERNEL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    return gpuIsClientRmAllocatedCtxBufferEnabled(pGpu);
};

/*!
 * This function returns whether PM ctxsw buffer should be pre-allocated or not.
 */
NvBool kgrctxShouldPreAllocPmBuffer_PF
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelChannel *pKernelChannel
)
{
    OBJGVASPACE                  *pGVAS = NULL;
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));
    NV_ASSERT_OR_RETURN(NULL != pKernelChannel, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsContext != NULL, NV_ERR_INVALID_STATE);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrctxGetUnicast(pGpu,
                         pKernelGraphicsContext,
                         &pKernelGraphicsContextUnicast));

    pGVAS = dynamicCast(pKernelChannel->pVAS, OBJGVASPACE);

    // Do not allocate the buffer, if already allocated
    if (pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc != NULL)
    {
        return NV_FALSE;
    }

    //
    // The PM Ctxsw buffer is now pre-allocated along with the other context buffers
    // This is done as a WAR for the issue tracked under bug 1760699
    // Reason: With the enablement of UVM8 by default, the UVM channel registration relies on the
    // buffers being allocated at channel allocation time and are not going to work for a buffer
    // created later with an rm ctrl
    //
    return gvaspaceIsExternallyOwned(pGVAS);
}

/*!
 * This function returns whether PM ctxsw buffer should be pre-allocated
 * or not.
 */
NvBool
kgrctxShouldPreAllocPmBuffer_VF
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelChannel *pKernelChannel
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast = NULL;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));
    NV_ASSERT_OR_RETURN(NULL != pKernelChannel, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsContext != NULL, NV_ERR_INVALID_STATE);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrctxGetUnicast(pGpu,
                         pKernelGraphicsContext,
                         &pKernelGraphicsContextUnicast));

    // Do not allocate the buffer, if already allocated
    if (pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc != NULL)
    {
        return NV_FALSE;
    }

    //
    // Pre-allocate the PM Ctxsw buffer along with the other context buffers
    // in below scenarios:
    // 1. For externally owned VA spaces. This is done as a WAR for the issue
    //    tracked under bug 1760699.
    //    Reason: With the enablement of UVM8 by default, the UVM channel registration relies on the
    //    buffers being allocated at channel allocation time and are not going to work for a buffer
    //    created later with an rm ctrl.
    // 2. For full SRIOV vGPU guests with Profiling capability enabled
    //
    if (IS_VIRTUAL_WITH_SRIOV(pGpu) && !gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
        NvBool bProfilingEnabledVgpuGuest = NV_FALSE;

        if (pVSI != NULL)
        {
            bProfilingEnabledVgpuGuest = pVSI->vgpuStaticProperties.bProfilingTracingEnabled;
            if (bProfilingEnabledVgpuGuest)
            {
                return NV_TRUE;
            }
        }
    }

    return kgrctxShouldPreAllocPmBuffer_PF(pGpu, pKernelGraphicsContext, pKernelChannel);
}

/*!
 * @brief should this layer manage the ctx buffers?
 */
NvBool
kgrctxShouldManageCtxBuffers_KERNEL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    NvU32 gfid
)
{
    return gpuIsClientRmAllocatedCtxBufferEnabled(pGpu) && !IS_GFID_VF(gfid);
}

/*!
 * @brief should this layer manage the ctx buffers?
 *    If client RM is managing the ctx buffers but the channel is a plugin
 *    channel, we should still manage them.
 */
NvBool
kgrctxShouldManageCtxBuffers_PHYSICAL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    NvU32 gfid
)
{
    return !gpuIsClientRmAllocatedCtxBufferEnabled(pGpu) || (gpuIsSriovEnabled(pGpu) && IS_GFID_PF(gfid));
}

/**
 * @brief Unmaps buffers associated with this context.
 */
void kgrctxUnmapBuffers_KERNEL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast,
    KernelChannel *pKernelChannel
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NV_STATUS status;
    NV2080_CTRL_GR_ROUTE_INFO grRouteInfo;
    KernelGraphics *pKernelGraphics;
    Device *pDevice = GPU_RES_GET_DEVICE(pKernelGraphicsContext);
    NvHandle hParent = RES_GET_PARENT_HANDLE(pKernelGraphicsContext);

    portMemSet(&grRouteInfo, 0, sizeof(grRouteInfo));
    kgrmgrCtrlSetChannelHandle(hParent, &grRouteInfo);
    NV_ASSERT_OK_OR_ELSE(status,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &grRouteInfo, &pKernelGraphics),
        return; );

    kgrctxUnmapAssociatedCtxBuffers(pGpu, pKernelGraphicsContext, pKernelGraphics, pKernelChannel);
}

/*!
 * @brief Unmap main GR ctx buffer
 *
 * @param[in]  pGpu
 * @param[in]  pKernelGraphicsContext
 * @param[in]  pKernelGraphics
 * @param[in]  pKernelChannel
 */
void
kgrctxUnmapMainCtxBuffer_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    KernelChannel *pKernelChannel
)
{
    MEMORY_DESCRIPTOR *pCtxBufferMemDesc;
    NV_STATUS status;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    status = kchangrpGetEngineContextMemDesc(pGpu, pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup, &pCtxBufferMemDesc);
    if ((status == NV_OK) && (pCtxBufferMemDesc != NULL))
    {
        // TODO remove Channel, ENG_GR dependencies
        kchannelUnmapEngineCtxBuf(pGpu, pKernelChannel, ENG_GR(kgraphicsGetInstance(pGpu, pKernelGraphics)));
        NV_ASSERT_OR_RETURN_VOID(
            kchannelSetEngineContextMemDesc(pGpu, pKernelChannel, ENG_GR(kgraphicsGetInstance(pGpu, pKernelGraphics)), NULL) == NV_OK);
    }
}

/*!
 * @brief Unmap all global ctx buffers from this context
 */
void
kgrctxUnmapGlobalCtxBuffers_IMPL
(
    OBJGPU                *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics        *pKernelGraphics,
    OBJVASPACE            *pVAS,
    NvU32                  gfid
)
{
    GR_GLOBALCTX_BUFFERS *pGlobalCtxBuffers = kgraphicsGetGlobalCtxBuffers(pGpu, pKernelGraphics, gfid);
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NV_STATUS status;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast),
        return;);

    // if global ctx buffers were never allocated then bail out early
    if (!pKernelGraphicsContextUnicast->localCtxBuffer.bAllocated &&
        !pGlobalCtxBuffers->bAllocated)
    {
         return;
    }

    kgrctxUnmapGlobalCtxBuffer(pGpu, pKernelGraphicsContext, pKernelGraphics, pVAS, GR_GLOBALCTX_BUFFER_BUNDLE_CB);
    kgrctxUnmapGlobalCtxBuffer(pGpu, pKernelGraphicsContext, pKernelGraphics, pVAS, GR_GLOBALCTX_BUFFER_ATTRIBUTE_CB);
    kgrctxUnmapGlobalCtxBuffer(pGpu, pKernelGraphicsContext, pKernelGraphics, pVAS, GR_GLOBALCTX_BUFFER_PAGEPOOL);

    if (kgraphicsIsRtvCbSupported(pGpu, pKernelGraphics))
    {
        kgrctxUnmapGlobalCtxBuffer(pGpu, pKernelGraphicsContext, pKernelGraphics, pVAS, GR_GLOBALCTX_BUFFER_RTV_CB);
    }

}

/*!
 * @brief Unmap specified global ctx buffer from the given VAS
 */
void
kgrctxUnmapGlobalCtxBuffer_IMPL
(
    OBJGPU                *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics        *pKernelGraphics,
    OBJVASPACE            *pVAS,
    GR_GLOBALCTX_BUFFER    buffId
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NV_STATUS status;
    NvU64 vaddr;

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast),
        return;);

    status = vaListFindVa(&pKernelGraphicsContextUnicast->globalCtxBufferVaList[buffId], pVAS, &vaddr);
    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Unmapping %s from VA @ 0x%llx\n",
                  NV_ENUM_TO_STRING(GR_GLOBALCTX_BUFFER, buffId), vaddr);

        kgraphicsUnmapCtxBuffer(pGpu, pKernelGraphics, pVAS, &pKernelGraphicsContextUnicast->globalCtxBufferVaList[buffId]);
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "Buffer for %s already unmapped\n",
                  NV_ENUM_TO_STRING(GR_GLOBALCTX_BUFFER, buffId));
    }
}

/*!
 * @brief Free main GR ctx buffer
 */
void
kgrctxFreeMainCtxBuffer_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NV_STATUS status;

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast),
        return;);

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));
    if (pKernelGraphicsContextUnicast->pMainCtxBuffer != NULL)
    {
        memdescFree(pKernelGraphicsContextUnicast->pMainCtxBuffer);
        memdescDestroy(pKernelGraphicsContextUnicast->pMainCtxBuffer);
        pKernelGraphicsContextUnicast->pMainCtxBuffer = NULL;
        pKernelGraphicsContextUnicast->bKGrMainCtxBufferInitialized = NV_FALSE;
    }
}

/**
 * @brief free the memory for the zcull context buffer
 */
void
kgrctxFreeZcullBuffer_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    MEMORY_DESCRIPTOR *pMemDesc;
    NV_STATUS status;

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast),
        return;);

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    pMemDesc = pKernelGraphicsContextUnicast->zcullCtxswBuffer.pMemDesc;
    if (pMemDesc == NULL)
    {
        NV_PRINTF(LEVEL_INFO,
                  "call to free zcull ctx buffer not RM managed, skipped!\n");
    }

    // buffer can be shared and refcounted -- released on final free
    memdescFree(pMemDesc);
    memdescDestroy(pMemDesc);
    pKernelGraphicsContextUnicast->zcullCtxswBuffer.pMemDesc = NULL;
}

/**
 * @brief free the memory for the preemption context buffers
 */
void
kgrctxFreeCtxPreemptionBuffers_IMPL
(
    OBJGPU                *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NV_STATUS status;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast),
        return;);

    memdescFree(pKernelGraphicsContextUnicast->preemptCtxswBuffer.pMemDesc);
    memdescDestroy(pKernelGraphicsContextUnicast->preemptCtxswBuffer.pMemDesc);
    pKernelGraphicsContextUnicast->preemptCtxswBuffer.pMemDesc = NULL;

    memdescFree(pKernelGraphicsContextUnicast->spillCtxswBuffer.pMemDesc);
    memdescDestroy(pKernelGraphicsContextUnicast->spillCtxswBuffer.pMemDesc);
    pKernelGraphicsContextUnicast->spillCtxswBuffer.pMemDesc = NULL;

    memdescFree(pKernelGraphicsContextUnicast->betaCBCtxswBuffer.pMemDesc);
    memdescDestroy(pKernelGraphicsContextUnicast->betaCBCtxswBuffer.pMemDesc);
    pKernelGraphicsContextUnicast->betaCBCtxswBuffer.pMemDesc = NULL;

    memdescFree(pKernelGraphicsContextUnicast->pagepoolCtxswBuffer.pMemDesc);
    memdescDestroy(pKernelGraphicsContextUnicast->pagepoolCtxswBuffer.pMemDesc);
    pKernelGraphicsContextUnicast->pagepoolCtxswBuffer.pMemDesc = NULL;

    memdescFree(pKernelGraphicsContextUnicast->rtvCbCtxswBuffer.pMemDesc);
    memdescDestroy(pKernelGraphicsContextUnicast->rtvCbCtxswBuffer.pMemDesc);
    pKernelGraphicsContextUnicast->rtvCbCtxswBuffer.pMemDesc = NULL;

    if (pKernelGraphicsContextUnicast->setupCtxswBuffer.pMemDesc != NULL)
    {
        memdescFree(pKernelGraphicsContextUnicast->setupCtxswBuffer.pMemDesc);
        memdescDestroy(pKernelGraphicsContextUnicast->setupCtxswBuffer.pMemDesc);
        pKernelGraphicsContextUnicast->setupCtxswBuffer.pMemDesc = NULL;
    }
}

/*!
 * @brief Free patch GR ctx buffer
 */
void
kgrctxFreePatchBuffer_IMPL
(
    OBJGPU                *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NV_STATUS status;

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu,
                         pKernelGraphicsContext,
                         &pKernelGraphicsContextUnicast),
        return;);

    if (pKernelGraphicsContextUnicast->ctxPatchBuffer.pMemDesc == NULL)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Attempt to free null ctx patch buffer pointer, skipped!\n");
        return;
    }

    memdescFree(pKernelGraphicsContextUnicast->ctxPatchBuffer.pMemDesc);
    memdescDestroy(pKernelGraphicsContextUnicast->ctxPatchBuffer.pMemDesc);

    pKernelGraphicsContextUnicast->ctxPatchBuffer.pMemDesc = NULL;
    pKernelGraphicsContextUnicast->bKGrPatchCtxBufferInitialized = NV_FALSE;
}

/**
 * @brief Free the memory for the pm context buffer associated with a channel
 */
void
kgrctxFreePmBuffer_IMPL
(
    OBJGPU                *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    MEMORY_DESCRIPTOR *pMemDesc;
    NV_STATUS status;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu,
                         pKernelGraphicsContext,
                         &pKernelGraphicsContextUnicast),
        return;);

    pMemDesc = pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc;
    if (pMemDesc != NULL)
    {
        //
        // This func assumes that the buffer was not allocated per subdevice,
        // and will leak any mappings performed by the secondaries.
        //
        NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));

        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,
                  "Attempt to free null pm ctx buffer pointer??\n");
    }

    pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc = NULL;
    pKernelGraphicsContextUnicast->bKGrPmCtxBufferInitialized = NV_FALSE;
}

/*!
 * @brief Free "local" global context buffers
 */
void kgrctxFreeLocalGlobalCtxBuffers_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    MEMORY_DESCRIPTOR *pMemDesc;
    GR_GLOBALCTX_BUFFERS *pCtxBuffers;
    GR_GLOBALCTX_BUFFER buff;
    NV_STATUS status;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast),
        return;);

    pCtxBuffers = &pKernelGraphicsContextUnicast->localCtxBuffer;

    // no ctx buffers allocated, so get out early
    if (!pCtxBuffers->bAllocated)
        return;

    FOR_EACH_IN_ENUM(GR_GLOBALCTX_BUFFER, buff)
    {
        pMemDesc = pCtxBuffers->memDesc[buff];
        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);
        pCtxBuffers->memDesc[buff] = NULL;
    }
    FOR_EACH_IN_ENUM_END;

    pCtxBuffers->bAllocated = NV_FALSE;

    // make sure all L2 cache lines using CB buffers are clear after we free them
    NV_ASSERT_OK(kmemsysCacheOp_HAL(pGpu, pKernelMemorySystem, NULL, FB_CACHE_VIDEO_MEMORY, FB_CACHE_EVICT));
}

/**
 * @brief Free all associated ctx buffers (main, patch, PM and private global buffers for cases like VPR).
 *
 * This function is called on every channel free and thefore can't assume any
 * graphics objects actually exist on the given channel.
 */
void kgrctxFreeAssociatedCtxBuffers_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NV_STATUS status;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu,
                         pKernelGraphicsContext,
                         &pKernelGraphicsContextUnicast),
        return;);

    //
    // When sharing contexts across channels we need to defer this until all
    // objects have been freed.
    //
    NV_CHECK_OR_RETURN_VOID(LEVEL_SILENT, pKernelGraphicsContextUnicast->channelObjects == 0);

    // if we have a context patch buffer, this will free it up
    kgrctxFreePatchBuffer(pGpu, pKernelGraphicsContext);

    // if we have a PM ctxsw buffer, this will free it up
    kgrctxFreePmBuffer(pGpu, pKernelGraphicsContext);

    // if we have a zcull buffer, this will free it up
    kgrctxFreeZcullBuffer(pGpu, pKernelGraphicsContext);

    // If we have preemption buffers, this will free them up
    kgrctxFreeCtxPreemptionBuffers(pGpu, pKernelGraphicsContext);

    // Release all common buffers used as part of the gr context.
    kgrctxFreeLocalGlobalCtxBuffers(pGpu, pKernelGraphicsContext);

    kgrctxFreeMainCtxBuffer(pGpu, pKernelGraphicsContext);
}

/*!
 * This function unmaps various GR buffers.
 */
NV_STATUS
kgrctxUnmapCtxBuffers_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphicsObject *pKernelGraphicsObject,
    KernelGraphics *pKernelGraphics,
    NvBool bDestructor
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    ChannelDescendant            *pChannelDescendant = staticCast(pKernelGraphicsObject, ChannelDescendant);
    KernelChannel                *pKernelChannel = pChannelDescendant->pKernelChannel;
    NvU32                         classNum = pChannelDescendant->resourceDesc.externalClassId;
    NvBool                        bRelease3d = NV_FALSE;
    NvU32                         objType;
    NvU32                         gfid;
    NvU64                         refCount;
    GR_GLOBALCTX_BUFFER           registerMapBufferId = kgrctxGetRegisterAccessMapId_HAL(pGpu, pKernelGraphicsContext, pChannelDescendant->pKernelChannel);

    NV_PRINTF(LEVEL_INFO, "gpu:%d isBC=%d\n", pGpu->gpuInstance,
              gpumgrGetBcEnabledStatus(pGpu));

    gfid = kchannelGetGfid(pKernelChannel);

    NV_ASSERT_OK_OR_RETURN(
        kgrctxGetUnicast(pGpu,
                         kgrobjGetKernelGraphicsContext(pGpu, pKernelGraphicsObject),
                         &pKernelGraphicsContextUnicast));

    kgrmgrGetGrObjectType(classNum, &objType);

    if (!bDestructor)
    {
        //
        // If we are cleaning up from the constructor, then we know the main ctx
        // buffer is not being used yet, and no other context buffers need to be
        // cleaned up, so we can return early here. If we are coming from the
        // destructor, the context buffers are not freed until channel
        // destruction.
        // TODO move this up one stack frame
        //
        kgrctxUnmapMainCtxBuffer(pGpu, pKernelGraphicsContext, pKernelGraphics, pChannelDescendant->pKernelChannel);
        kgrctxFreeMainCtxBuffer(pGpu, pKernelGraphicsContext);
        return NV_OK;
    }

    if ((pKernelGraphicsContextUnicast->channelObjects != 0) &&
         (pKernelGraphicsContextUnicast->ctxPatchBuffer.pMemDesc != NULL))
    {
        //
        // Delay freeing the patch buffer until the last channel free.
        // these buffers are accessed even after the last GR object is freed.
        //
        kgraphicsUnmapCtxBuffer(pGpu, pKernelGraphics, pKernelChannel->pVAS, &pKernelGraphicsContextUnicast->ctxPatchBuffer.vAddrList);
    }

    // Defer releasing mapping if this would cause the buffer to be unmapped
    if ((pKernelGraphicsContextUnicast->pmCtxswBuffer.pMemDesc != NULL) &&
        (vaListGetRefCount(&pKernelGraphicsContextUnicast->pmCtxswBuffer.vAddrList, pKernelChannel->pVAS, &refCount) == NV_OK) &&
        (refCount > 1))
    {
        kgrctxUnmapCtxPmBuffer(pGpu, pKernelGraphicsContext, pKernelGraphics, pKernelChannel->pVAS);
    }

    // Defer releasing mapping if this would cause the buffer to be unmapped
    if (kgraphicsDoesUcodeSupportPrivAccessMap(pGpu, pKernelGraphics) &&
        (vaListGetRefCount(&pKernelGraphicsContextUnicast->globalCtxBufferVaList[registerMapBufferId], pKernelChannel->pVAS, &refCount) == NV_OK) &&
        (refCount > 1))
    {
        kgrctxUnmapGlobalCtxBuffer(pGpu,
                                   pKernelGraphicsContext,
                                   pKernelGraphics,
                                   pKernelChannel->pVAS,
                                   registerMapBufferId);
    }

    // Defer releasing mapping if this would cause the buffer to be unmapped
    if ((kgraphicsGetGlobalCtxBuffers(pGpu, pKernelGraphics, gfid)->memDesc[GR_GLOBALCTX_BUFFER_FECS_EVENT] != NULL) &&
        (vaListGetRefCount(&pKernelGraphicsContextUnicast->globalCtxBufferVaList[GR_GLOBALCTX_BUFFER_FECS_EVENT], pKernelChannel->pVAS, &refCount) == NV_OK) &&
        (refCount > 1))
    {
        kgrctxUnmapGlobalCtxBuffer(pGpu, pKernelGraphicsContext, pKernelGraphics, pKernelChannel->pVAS, GR_GLOBALCTX_BUFFER_FECS_EVENT);
    }

    //
    // Condition for releasing 3d context buffer mappings:
    // For non-TSG & legacy TSG mode, always unmap as long as its not the last
    // object in TSG
    // If it is the last object, then unmap during channel free as these buffers
    // are accessed even after the last 3D object is freed.
    // For subcontext TSG case, only unmap on 2D/3D object free as long its not
    // the last 2D/3D object
    // For compute object with subcontext we never map so we don't need to call
    // unmap.
    // If we free on last object then buffer gets actually unmapped (as opposed
    // to decrement in refcount) and we start hitting page faults
    //
    if (pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->bLegacyMode)
    {
        bRelease3d = (pKernelGraphicsContextUnicast->channelObjects > 0);
    }
    else
    {
        NvBool bIs3dBuffer = ((objType == GR_OBJECT_TYPE_2D) || (objType == GR_OBJECT_TYPE_3D));
        bRelease3d = (bIs3dBuffer && ((pKernelGraphicsContextUnicast->objectCounts[GR_OBJECT_TYPE_2D] > 0) ||
            (pKernelGraphicsContextUnicast->objectCounts[GR_OBJECT_TYPE_3D] > 0)));
    }

    if (bRelease3d)
    {
        kgrctxUnmapGlobalCtxBuffers(pGpu, pKernelGraphicsContext, pKernelGraphics, pKernelChannel->pVAS, gfid);
    }

    return NV_OK;
}

/*!
 * Function to increment the GR channel object count
 */
void
kgrctxIncObjectCount_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    NvU32 classNum
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NvU32 objType;
    NV_STATUS status;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    kgrmgrGetGrObjectType(classNum, &objType);

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast),
        return;);

    switch (objType)
    {
        case GR_OBJECT_TYPE_COMPUTE:
            pKernelGraphicsContextUnicast->objectCounts[GR_OBJECT_TYPE_COMPUTE]++;
            gpuChangeComputeModeRefCount(pGpu, NV_GPU_COMPUTE_REFCOUNT_COMMAND_INCREMENT);
            break;

        case GR_OBJECT_TYPE_3D:
            pKernelGraphicsContextUnicast->objectCounts[GR_OBJECT_TYPE_3D]++;
            break;

        case GR_OBJECT_TYPE_2D:
            pKernelGraphicsContextUnicast->objectCounts[GR_OBJECT_TYPE_2D]++;
            break;

        case GR_OBJECT_TYPE_MEM:
            pKernelGraphicsContextUnicast->objectCounts[GR_OBJECT_TYPE_MEM]++;
            break;

        default:
            NV_PRINTF(LEVEL_ERROR, "Unrecognized graphics class 0x%x\n",
                      classNum);
            DBG_BREAKPOINT();
            return;
    }

    pKernelGraphicsContextUnicast->channelObjects++;
    NV_PRINTF(LEVEL_INFO,
              "Class 0x%x allocated.  %d objects allocated\n",
              classNum,
              pKernelGraphicsContextUnicast->channelObjects);
}

/*!
 * Function to decrement the GR channel object count of the given class
 */
void
kgrctxDecObjectCount_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    NvU32 classNum
)
{
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
    NvU32 objType;
    NvU32 countIdx;
    NV_STATUS status;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    kgrmgrGetGrObjectType(classNum, &objType);

    NV_ASSERT_OK_OR_ELSE(status,
        kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast),
        return;);

    NV_ASSERT_OR_ELSE(pKernelGraphicsContextUnicast->channelObjects != 0,
                      NV_PRINTF(LEVEL_ERROR, "No active GR objects to free for Class 0x%x\n", classNum));

    //
    // Handle 2D and Compute class reference counting
    //
    switch (objType)
    {
        case GR_OBJECT_TYPE_COMPUTE:
            gpuChangeComputeModeRefCount(pGpu, NV_GPU_COMPUTE_REFCOUNT_COMMAND_DECREMENT);
            countIdx = GR_OBJECT_TYPE_COMPUTE;
            break;

        case GR_OBJECT_TYPE_3D:
            countIdx = GR_OBJECT_TYPE_3D;
            break;

        case GR_OBJECT_TYPE_2D:
            countIdx = GR_OBJECT_TYPE_2D;
            break;

        case GR_OBJECT_TYPE_MEM:
            countIdx = GR_OBJECT_TYPE_MEM;
            break;

        default:
            NV_PRINTF(LEVEL_ERROR, "Unrecognized graphics class 0x%x\n",
                      classNum);
            DBG_BREAKPOINT();
            return;
    }

    NV_ASSERT_OR_RETURN_VOID(pKernelGraphicsContextUnicast->objectCounts[countIdx] > 0);
    pKernelGraphicsContextUnicast->objectCounts[countIdx]--;
    pKernelGraphicsContextUnicast->channelObjects--;
}

/*!
 * @brief Return the register access buffer used for a given Channel's permission
 *
 * The base register access map comes from the architecture team.  The user version
 * of the buffer removes access to PM regisers.
 *
 * The policy currently depends on administrator access on the system, except on
 * one VGPU configuration.
 */
GR_GLOBALCTX_BUFFER
kgrctxGetRegisterAccessMapId_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelChannel *pKernelChannel
)
{
    // Using cached privilege because this function is called at a raised IRQL.
    if (kchannelCheckIsAdmin(pKernelChannel)
        && !hypervisorIsVgxHyper() && IS_GFID_PF(kchannelGetGfid(pKernelChannel)))
    {
        return GR_GLOBALCTX_BUFFER_UNRESTRICTED_PRIV_ACCESS_MAP;
    }

    return GR_GLOBALCTX_BUFFER_PRIV_ACCESS_MAP;
}

NV_STATUS
kgrctxCtrlGetTpcPartitionMode_IMPL
(
    KernelGraphicsContext *pKernelGraphicsContext,
    NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelGraphicsContext);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (IS_GSP_CLIENT(pGpu))
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        return pRmApi->Control(pRmApi,
                               pRmCtrlParams->hClient,
                               pRmCtrlParams->hObject,
                               pRmCtrlParams->cmd,
                               pRmCtrlParams->pParams,
                               pRmCtrlParams->paramsSize);
    }

    return gpuresInternalControlForward_IMPL(staticCast(pKernelGraphicsContext, GpuResource),
                                             NV0090_CTRL_CMD_INTERNAL_GET_TPC_PARTITION_MODE,
                                             pParams,
                                             sizeof(*pParams));
}

NV_STATUS
kgrctxCtrlSetTpcPartitionMode_IMPL
(
    KernelGraphicsContext *pKernelGraphicsContext,
    NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelGraphicsContext);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (IS_GSP_CLIENT(pGpu))
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        return pRmApi->Control(pRmApi,
                               pRmCtrlParams->hClient,
                               pRmCtrlParams->hObject,
                               pRmCtrlParams->cmd,
                               pRmCtrlParams->pParams,
                               pRmCtrlParams->paramsSize);
    }

    return gpuresInternalControlForward_IMPL(staticCast(pKernelGraphicsContext, GpuResource),
                                             NV0090_CTRL_CMD_INTERNAL_SET_TPC_PARTITION_MODE,
                                             pParams,
                                             sizeof(*pParams));
}

NV_STATUS
kgrctxCtrlSetLgSectorPromotion_IMPL
(
    KernelGraphicsContext* pKernelGraphicsContext,
    NV0090_CTRL_SET_LG_SECTOR_PROMOTION_PARAMS* pParams
)
{
    OBJGPU* pGpu = GPU_RES_GET_GPU(pKernelGraphicsContext);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (IS_GSP_CLIENT(pGpu))
    {
        RM_API* pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        CALL_CONTEXT* pCallContext = resservGetTlsCallContext();
        RmCtrlParams* pRmCtrlParams = pCallContext->pControlParams;

        return pRmApi->Control(pRmApi,
            pRmCtrlParams->hClient,
            pRmCtrlParams->hObject,
            pRmCtrlParams->cmd,
            pRmCtrlParams->pParams,
            pRmCtrlParams->paramsSize);
    }

    return gpuresInternalControlForward_IMPL(staticCast(pKernelGraphicsContext, GpuResource),
        NV0090_CTRL_CMD_INTERNAL_SET_LG_SECTOR_PROMOTION,
        pParams,
        sizeof(*pParams));
}

NV_STATUS
kgrctxCtrlGetMMUDebugMode_IMPL
(
    KernelGraphicsContext *pKernelGraphicsContext,
    NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelGraphicsContext);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (IS_GSP_CLIENT(pGpu))
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        return pRmApi->Control(pRmApi,
                               pRmCtrlParams->hClient,
                               pRmCtrlParams->hObject,
                               pRmCtrlParams->cmd,
                               pRmCtrlParams->pParams,
                               pRmCtrlParams->paramsSize);
    }

    return gpuresInternalControlForward_IMPL(staticCast(pKernelGraphicsContext, GpuResource),
                                             NV0090_CTRL_CMD_INTERNAL_GET_MMU_DEBUG_MODE,
                                             pParams,
                                             sizeof(*pParams));
}

NV_STATUS
kgrctxCtrlProgramVidmemPromote_IMPL
(
    KernelGraphicsContext *pKernelGraphicsContext,
    NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelGraphicsContext);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        NV_STATUS status = NV_OK;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);

        return status;
    }

    return gpuresInternalControlForward_IMPL(staticCast(pKernelGraphicsContext, GpuResource),
                                             NV0090_CTRL_CMD_INTERNAL_PROGRAM_VIDMEM_PROMOTE,
                                             pParams,
                                             sizeof(*pParams));
}

/*!
 * @brief Construct shared kernel graphics context. (Does nothing)
 */
NV_STATUS
shrkgrctxConstruct_IMPL(KernelGraphicsContextShared *pKernelGraphicsContextShared)
{
    return NV_OK;
}

/*!
 * @brief Construct shared kernel graphics context
 *
 * @param[in]  pGpu
 * @param[in]  pKernelGraphicsContextShared
 * @param[in]  pKernelGraphicsContext
 */
NV_STATUS
shrkgrctxInit_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContextShared *pKernelGraphicsContextShared,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    NV_STATUS status = NV_OK;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
        KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
        KernelGraphics *pKernelGraphics;
        NV2080_CTRL_GR_ROUTE_INFO grRouteInfo;
        Device *pDevice = GPU_RES_GET_DEVICE(pKernelGraphicsContext);
        NvHandle hParent = RES_GET_PARENT_HANDLE(pKernelGraphicsContext);

        portMemSet(&grRouteInfo, 0, sizeof(grRouteInfo));
        kgrmgrCtrlSetChannelHandle(hParent, &grRouteInfo);

        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_ERROR,
            kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &grRouteInfo, &pKernelGraphics));

        if (status != NV_OK)
            SLI_LOOP_BREAK;

        if (kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast) == NV_OK)
        {
            NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_ERROR,
                shrkgrctxConstructUnicast(pGpu, pKernelGraphicsContextShared, pKernelGraphicsContext, pKernelGraphics, pKernelGraphicsContextUnicast));

            if (status != NV_OK)
                SLI_LOOP_BREAK;
        }
    }
    SLI_LOOP_END;

    listInit(&pKernelGraphicsContextShared->activeDebuggers, portMemAllocatorGetGlobalNonPaged());

    if (status != NV_OK)
        shrkgrctxTeardown_IMPL(pGpu, pKernelGraphicsContextShared, pKernelGraphicsContext);

    return status;
}

/*!
 * @brief Construct unicast state for this context
 *
 * @param[in]  pGpu
 * @param[in]  pKernelGraphicsContextShared
 * @param[in]  pKernelGraphicsContext
 * @param[in]  pKernelGraphics
 * @param[in]  pKernelGraphicsContextUnicast
 */
NV_STATUS
shrkgrctxConstructUnicast_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContextShared *pKernelGraphicsContextShared,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphics *pKernelGraphics,
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast
)
{
    NvU32 i;

    for (i = 0; i < GR_GLOBALCTX_BUFFER_COUNT; i++)
        NV_ASSERT_OK_OR_RETURN(vaListInit(&pKernelGraphicsContextUnicast->globalCtxBufferVaList[i]));

    NV_ASSERT_OK_OR_RETURN(vaListInit(&pKernelGraphicsContextUnicast->ctxPatchBuffer.vAddrList));
    NV_ASSERT_OK_OR_RETURN(vaListInit(&pKernelGraphicsContextUnicast->pmCtxswBuffer.vAddrList));
    NV_ASSERT_OK_OR_RETURN(vaListInit(&pKernelGraphicsContextUnicast->zcullCtxswBuffer.vAddrList));

    NV_ASSERT_OK_OR_RETURN(vaListInit(&pKernelGraphicsContextUnicast->preemptCtxswBuffer.vAddrList));
    NV_ASSERT_OK_OR_RETURN(vaListInit(&pKernelGraphicsContextUnicast->spillCtxswBuffer.vAddrList));
    NV_ASSERT_OK_OR_RETURN(vaListInit(&pKernelGraphicsContextUnicast->betaCBCtxswBuffer.vAddrList));
    NV_ASSERT_OK_OR_RETURN(vaListInit(&pKernelGraphicsContextUnicast->pagepoolCtxswBuffer.vAddrList));
    NV_ASSERT_OK_OR_RETURN(vaListInit(&pKernelGraphicsContextUnicast->rtvCbCtxswBuffer.vAddrList));
    if (kgraphicsGetPeFiroBufferEnabled(pGpu, pKernelGraphics))
        NV_ASSERT_OK_OR_RETURN(vaListInit(&pKernelGraphicsContextUnicast->setupCtxswBuffer.vAddrList));

    pKernelGraphicsContextUnicast->bSupportsPerSubctxHeader =
        kgraphicsIsPerSubcontextContextHeaderSupported(pGpu, pKernelGraphics);

    return NV_OK;
}

/*!
 * @brief Destruct shared kernel graphics context.
 */
void
shrkgrctxDestruct_IMPL(KernelGraphicsContextShared *pKernelGraphicsContextShared)
{
    NV_ASSERT(listCount(&pKernelGraphicsContextShared->activeDebuggers) == 0);
    listDestroy(&pKernelGraphicsContextShared->activeDebuggers);
}

/*!
 * @brief Destroy the shared context state
 */
void
shrkgrctxTeardown_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContextShared *pKernelGraphicsContextShared,
    KernelGraphicsContext *pKernelGraphicsContext
)
{

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;

        if (kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast) == NV_OK)
            shrkgrctxDestructUnicast(pGpu, pKernelGraphicsContextShared, pKernelGraphicsContext, pKernelGraphicsContextUnicast);
    }
    SLI_LOOP_END;

    portMemSet(&pKernelGraphicsContext->pShared->kernelGraphicsContextUnicast, 0x0, sizeof(pKernelGraphicsContext->pShared->kernelGraphicsContextUnicast));
}

/*!
 * Destroy the unicast context state
 */
void
shrkgrctxDestructUnicast_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContextShared *pKernelGraphicsContextShared,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast
)
{
    GR_GLOBALCTX_BUFFER buff;

    if (kgrctxShouldCleanup_HAL(pGpu, pKernelGraphicsContext))
        kgrctxFreeAssociatedCtxBuffers(pGpu, pKernelGraphicsContext);

    FOR_EACH_IN_ENUM(GR_GLOBALCTX_BUFFER, buff)
    {
        vaListDestroy(&pKernelGraphicsContextUnicast->globalCtxBufferVaList[buff]);
    }
    FOR_EACH_IN_ENUM_END;
    vaListDestroy(&pKernelGraphicsContextUnicast->ctxPatchBuffer.vAddrList);
    vaListDestroy(&pKernelGraphicsContextUnicast->pmCtxswBuffer.vAddrList);
    vaListDestroy(&pKernelGraphicsContextUnicast->zcullCtxswBuffer.vAddrList);

    vaListDestroy(&pKernelGraphicsContextUnicast->preemptCtxswBuffer.vAddrList);
    vaListDestroy(&pKernelGraphicsContextUnicast->spillCtxswBuffer.vAddrList);
    vaListDestroy(&pKernelGraphicsContextUnicast->betaCBCtxswBuffer.vAddrList);
    vaListDestroy(&pKernelGraphicsContextUnicast->pagepoolCtxswBuffer.vAddrList);
    vaListDestroy(&pKernelGraphicsContextUnicast->rtvCbCtxswBuffer.vAddrList);
    vaListDestroy(&pKernelGraphicsContextUnicast->setupCtxswBuffer.vAddrList);
}

/*!
 * @brief Perform cleanup tasks run on channel removal from context
 */
void shrkgrctxDetach_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsContextShared *pKernelGraphicsContextShared,
    KernelGraphicsContext *pKernelGraphicsContext,
    KernelChannel *pKernelChannel
)
{
    RM_ENGINE_TYPE rmEngineType = kchannelGetEngineType(pKernelChannel);

    // pre-Ampere chips can have NULL engine types. Find the engine type based on runlistId if set
    if (!RM_ENGINE_TYPE_IS_VALID(rmEngineType))
    {
        if (kchannelIsRunlistSet(pGpu, pKernelChannel))
        {
            KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

            NV_ASSERT_OK(
                kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                         ENGINE_INFO_TYPE_RUNLIST, kchannelGetRunlistId(pKernelChannel),
                                         ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32 *)&rmEngineType));
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "Channel destroyed but never bound, scheduled, or had a descendant object created\n");
        }
    }

    if (RM_ENGINE_TYPE_IS_GR(rmEngineType))
    {
        OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelChannel);
        KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
        Device *pDevice = GPU_RES_GET_DEVICE(pKernelChannel);
        NV2080_CTRL_GR_ROUTE_INFO grRouteInfo = {0};
        KernelGraphics *pKernelGraphics;
        NV_STATUS status;

        kgrmgrCtrlSetChannelHandle(RES_GET_HANDLE(pKernelChannel), &grRouteInfo);
        NV_ASSERT_OK_OR_ELSE(status,
            kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &grRouteInfo, &pKernelGraphics),
            return;);

        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
        {
            // Last chance to process FECS ctxsw log before channel goes away
            nvEventBufferFecsCallback(pGpu, pKernelGraphics);
        }
        SLI_LOOP_END
    }

    //
    // If PDB_PROP_GPU_CLIENT_RM_ALLOCATED_CTX_BUFFER is set, then kernel RM
    // is responsible for unmapping the context buffers, otherwise it is left to
    // physical RM.
    //
    if (!kgrctxShouldCleanup(pGpu, pKernelGraphicsContext))
        return;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;

        if (kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast) == NV_OK)
            kgrctxUnmapBuffers_HAL(pGpu, pKernelGraphicsContext, pKernelGraphicsContextUnicast, pKernelChannel);
    }
    SLI_LOOP_END;
}

