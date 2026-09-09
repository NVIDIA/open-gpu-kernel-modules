/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu_op_event.h"

#if !defined(SRT_BUILD)
#include "gpu/gpu.h"
#endif
#include "nvoc/event.h"
#include "nvoc/event_bus.h"
#include "nvport/nvport.h"
#include "utils/nvassert.h"
#include "libraries/cper/gpu_cper.h"
#include "class/cl90d0.h"

static inline GPU_OPERATIONAL_EVENT_ORIGINATOR _gpuopevtOriginator(void)
{
    return GPU_OPERATIONAL_EVENT_ORIGINATOR_PF_DRIVER;
}

NV_STATUS
gpuopevtConstruct_IMPL
(
    GpuOperationalEvent *pEvent,
    EventBus            *pEventBus,
    NvU32                category,
    const char          *pModuleSignature,
    NvU32                eventCode,
    NvU32                severity,
    NvU32                attributes,
    NvU32                scope,
    NvU32                reportingSource,
    NvU32                logLevel
)
{
    pEvent->scope = (GPU_OPERATIONAL_EVENT_SCOPE)scope;
    pEvent->originator = _gpuopevtOriginator();
    pEvent->reportingSource = (GPU_OPERATIONAL_EVENT_REPORTING_SOURCE)reportingSource;
    portMemSet(pEvent->deviceUuid, 0, sizeof(pEvent->deviceUuid));
    pEvent->pdi = 0;

    // These runtime parameters can be set later by calling gpuopevtInit()
    pEvent->moduleInstance = 0;
    pEvent->chipletId = 0;
    pEvent->migAttribution = GPU_OPERATIONAL_EVENT_MIG_ATTRIBUTION_NONE;

#if !defined(SRT_BUILD)
    OBJGPU *pGpu = (OBJGPU *)eventbusGetContext(pEventBus);
    if ((pGpu != NULL) && pGpu->gpuUuid.isInitialized)
    {
        portMemCopy(pEvent->deviceUuid, sizeof(pEvent->deviceUuid),
                    pGpu->gpuUuid.uuid, sizeof(pEvent->deviceUuid));
        (void)gpuGetPdi_HAL(pGpu, &pEvent->pdi);
    }
#endif

    return NV_OK;
}

void gpuopevtInit_IMPL(GpuOperationalEvent *pEvent, const GpuOperationalEventParams *pParams)
{
    pEvent->moduleInstance = pParams->moduleInstance;
    pEvent->chipletId = pParams->chipletId;
    pEvent->migAttribution = pParams->migAttribution;
}

/* ------------------------- Generic Event Context Serializer Helpers ------------------------- */

NV_STATUS gpuEventCtxSerializeToEventBufferGoeOpaqueCtx
(
    EventContextHeader *pCtxHeader,
    NV_GPU_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    NvU32 *pBytesWritten
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(pCtxHeader, pOutput, outputSize,
                                                       pBytesWritten,
                                                       OPERATIONAL_EVENT_CTX_TYPE_OPAQUE, 0);
}

NV_STATUS gpuEventCtxSerializeToEventBufferGoeCtxType
(
    EventContextHeader *pCtxHeader,
    NV_GPU_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    NvU32 *pBytesWritten,
    GPU_OPERATIONAL_EVENT_CTX_TYPE ctxType,
    NvU16 dataFormatVersion
)
{
    NvU32 totalSize;

    if (pCtxHeader->dataSize > NV_U32_MAX - sizeof(*pOutput))
        return NV_ERR_INVALID_ARGUMENT;

    totalSize = sizeof(*pOutput) + pCtxHeader->dataSize;
    *pBytesWritten = totalSize;

    if (pOutput == NULL)
    {
        NV_ASSERT_OR_RETURN(outputSize == 0, NV_ERR_INVALID_ARGUMENT);
        return NV_OK;
    }

    if (outputSize < totalSize)
        return NV_ERR_BUFFER_TOO_SMALL;

    pOutput->contextType = ctxType;
    pOutput->dataFormatVersion = dataFormatVersion;
    pOutput->dataSize = pCtxHeader->dataSize;

    if (pCtxHeader->dataSize != 0)
        portMemCopy((NvU8 *)(pOutput + 1), pCtxHeader->dataSize,
                    (NvU8 *)(pCtxHeader + 1), pCtxHeader->dataSize);

    return NV_OK;
}

NV_STATUS gpuEventCtxSerializeToCperOpaqueCtx
(
    EventContextHeader *pCtxHeader,
    NV_CPER_NV_EVENT_SECTION_STATE *pState
)
{
    // Nothing to add for 0-sized contexts
    if (pCtxHeader->dataSize == 0)
        return NV_OK;

    return cperNvidiaEventAddOpaqueContext(pState, (NvU8 *)(pCtxHeader + 1), pCtxHeader->dataSize);
}

/* --------------------------- Xid Event Context Serializer Helpers --------------------------- */

// Serializes a GpuXidEventCtx to a NV_GPU_OPERATIONAL_EVENT_CONTEXT of type GPU_LEGACY_XID
NV_STATUS gpuXidEventCtxSerializeToEventBufferGoeCtx
(
    EventContextHeader *pCtxHeader,
    NV_GPU_OPERATIONAL_EVENT_CONTEXT *pOutputCtx,
    NvU32 outputSize,
    NvU32 *pBytesWritten
)
{
    GpuXidEventCtx *pXidCtx = (GpuXidEventCtx *)pCtxHeader;
    GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID *pOutputXidContext;
    NvU32 messageSize;
    NvU32 payloadSize;
    NvU32 totalSize;
    const NvU32 maxMessageLen = GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_MAX_MSG_LEN;

    messageSize = (NvU32)portStringLengthSafe(pXidCtx->data.message, maxMessageLen) + 1;
    payloadSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(messageSize);
    totalSize = sizeof(*pOutputCtx) + payloadSize;
    *pBytesWritten = totalSize;

    if (pOutputCtx == NULL)
    {
        NV_CHECK_OR_RETURN(LEVEL_WARNING, outputSize == 0, NV_ERR_INVALID_ARGUMENT);
        return NV_OK;
    }

    if (outputSize < totalSize)
        return NV_ERR_BUFFER_TOO_SMALL;

    pOutputCtx->contextType = GPU_OPERATIONAL_EVENT_CTX_TYPE_GPU_LEGACY_XID;
    pOutputCtx->dataFormatVersion = GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_VERSION;
    pOutputCtx->dataSize = payloadSize;

    pOutputXidContext = (GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID *)(pOutputCtx + 1);
    pOutputXidContext->xidCode = pXidCtx->data.xidCode;
    portStringCopy(pOutputXidContext->message, messageSize,
                   pXidCtx->data.message, messageSize);

    return NV_OK;
}

// Serializes a GpuXidEventCtx to an NVIDIA GPU-CPER section by appending a GPU_LEGACY_XID context
NV_STATUS gpuXidEventCtxSerializeToCperCtx
(
    EventContextHeader *pCtxHeader,
    NV_CPER_NV_EVENT_SECTION_STATE *pState
)
{
    GpuXidEventCtx *pXidCtx = (GpuXidEventCtx *)pCtxHeader;
    return cperNvidiaEventAddGpuLegacyXidContext(pState,
                                                 pXidCtx->data.xidCode,
                                                 pXidCtx->data.message);
}

