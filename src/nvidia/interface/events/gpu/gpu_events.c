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

#include "events/gpu/gpu_events.h"

#include "cper/gpu_cper.h"
#if !defined(SRT_BUILD)
#include "gpu/gpu.h"
#endif
#include "nvoc/event.h"
#include "nvport/nvport.h"
#include "utils/nvassert.h"

#define GPU_EVENTS_PROTOBUF_SUPPORTED 0

/* ----------------------------- GPU Operational Event Categories ----------------------------- */

// GPU_OPERATIONAL_EVENT_CATEGORY_TIMEOUT
static NV_STATUS _gpuTimeoutSerializeToEventBufferOpEventCtx
(
    EventContextHeader *pContextHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
            pContextHeader,
            pOutput,
            outputSize,
            pInfo,
            GPU_OPERATIONAL_EVENT_CTX_TYPE_GPU_TIMEOUT_DATA,
            GPU_OPERATIONAL_EVENT_CTX_GPU_TIMEOUT_DATA_VERSION);
}

static NV_STATUS _gpuTimeoutSerializeToProtobuf
(
    EventContextHeader *pContextHeader,
    PRB_ENCODER         *pPrbEnc,
    NvU32              *pContextType,
    NvU32              *pDataFormatVersion
)
{
    NV_ASSERT_OR_RETURN(pContextHeader != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pPrbEnc != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pContextType != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pDataFormatVersion != NULL, NV_ERR_INVALID_POINTER);

    *pContextType = GPU_OPERATIONAL_EVENT_CTX_TYPE_GPU_TIMEOUT_DATA;
    *pDataFormatVersion = GPU_OPERATIONAL_EVENT_CTX_GPU_TIMEOUT_DATA_VERSION;

#if GPU_EVENTS_PROTOBUF_SUPPORTED
    GpuTimeoutEventCtx *pContext = (GpuTimeoutEventCtx *)pContextHeader;
    NV_STATUS           status;

    status = prbEncNestedStart(
        pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_CONTEXT_GPU_TIMEOUT);
    if (status != NV_OK)
        return status;

    status = prbEncAddUInt64(
        pPrbEnc, GPUEVENTS_GPUTIMEOUTCONTEXT_TIMEOUT_NS,
        pContext->data.timeoutNs);
    if (status != NV_OK)
        return status;

    status = prbEncAddUInt64(
        pPrbEnc, GPUEVENTS_GPUTIMEOUTCONTEXT_ELAPSED_NS,
        pContext->data.elapsedNs);
    if (status != NV_OK)
        return status;

    status = prbEncAddString(
        pPrbEnc, GPUEVENTS_GPUTIMEOUTCONTEXT_WAIT_TARGET,
        pContext->data.waitTarget);
    if (status != NV_OK)
        return status;

    return prbEncNestedEnd(pPrbEnc);
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

static const EventContextOutputAdapters g_gpuTimeoutOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _gpuTimeoutSerializeToEventBufferOpEventCtx,
    .serializeToProtobuf = _gpuTimeoutSerializeToProtobuf,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = NULL,
};

NV_STATUS gputimeoutConstruct_IMPL
(
    GpuTimeout      *pEvent,
    EventBus        *pEventBus,
    const char      *pModuleSignature,
    NvU32            eventCode,
    NvU32            severity,
    NvU32            attributes,
    NvU32            scope,
    NvU64            timeoutNs,
    NvU64            elapsedNs,
    const char      *pWaitTarget,
    NvU32            logLevel
)
{
    pEvent->context.header.pOutputAdapters = &g_gpuTimeoutOutputAdapters;
    pEvent->context.data.timeoutNs = timeoutNs;
    pEvent->context.data.elapsedNs = elapsedNs;
    pEvent->context.header.dataSize = (NvU32)
        GPU_OPERATIONAL_EVENT_CTX_GPU_TIMEOUT_DATA_SIZE(
            portStringCopy(pEvent->context.data.waitTarget,
                           sizeof(pEvent->context.data.waitTarget),
                           (pWaitTarget != NULL) ? pWaitTarget : "",
                           sizeof(pEvent->context.data.waitTarget))
        );
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    return NV_OK;
}

// GPU_OPERATIONAL_EVENT_CATEGORY_MEMORY_INTEGRITY_ERROR
NV_STATUS gpumemintegrityerrConstruct_IMPL
(
    GpuMemoryIntegrityError *pEvent,
    EventBus                *pEventBus,
    const char              *pModuleSignature,
    NvU32                    eventCode,
    NvU32                    severity,
    NvU32                    attributes,
    NvU32                    scope,
    NvU32                    reportingSource,
    NvU32                    logLevel
)
{
    return NV_OK;
}

// GPU_OPERATIONAL_EVENT_CATEGORY_INTERCONNECT_ERROR
NV_STATUS gpuinterconnecterrConstruct_IMPL
(
    GpuInterconnectError *pEvent,
    EventBus             *pEventBus,
    const char           *pModuleSignature,
    NvU32                 eventCode,
    NvU32                 severity,
    NvU32                 attributes,
    NvU32                 scope,
    NvU32                 reportingSource,
    NvU32                 logLevel
)
{
    return NV_OK;
}

// GPU_OPERATIONAL_EVENT_CATEGORY_FIRMWARE_FAULT
NV_STATUS gpufwfaultConstruct_IMPL
(
    GpuFirmwareFault *pEvent,
    EventBus         *pEventBus,
    const char       *pModuleSignature,
    NvU32             eventCode,
    NvU32             severity,
    NvU32             attributes,
    NvU32             scope,
    NvU32             logLevel
)
{
    return NV_OK;
}

// GPU_OPERATIONAL_EVENT_CATEGORY_RESOURCE_EXHAUSTED
NV_STATUS gpuresexhConstruct_IMPL
(
    GpuResourceExhausted *pEvent,
    EventBus             *pEventBus,
    const char           *pModuleSignature,
    NvU32                 eventCode,
    NvU32                 severity,
    NvU32                 attributes,
    NvU32                 scope
)
{
    return NV_OK;
}

// GPU_OPERATIONAL_EVENT_CATEGORY_UNCLASSIFIED_ERROR
NV_STATUS gpuunclassifiederrConstruct_IMPL
(
    GpuUnclassifiedError *pEvent,
    EventBus             *pEventBus,
    const char           *pModuleSignature,
    NvU32                 eventCode,
    NvU32                 severity,
    NvU32                 attributes,
    NvU32                 scope
)
{
    return NV_OK;
}

// GPU_OPERATIONAL_EVENT_CATEGORY_INITIALIZATION
NV_STATUS gpuinitevtConstruct_IMPL
(
    GpuInitializationEvent *pEvent,
    EventBus               *pEventBus,
    const char             *pModuleSignature,
    NvU32                   eventCode,
    NvU32                   severity,
    NvU32                   attributes,
    NvU32                   scope,
    NvU32                   logLevel
)
{
    return NV_OK;
}

// GPU_OPERATIONAL_EVENT_CATEGORY_RESOURCE_RETIREMENT
NV_STATUS gpuresretConstruct_IMPL
(
    GpuResourceRetirement *pEvent,
    EventBus              *pEventBus,
    const char            *pModuleSignature,
    NvU32                  eventCode,
    NvU32                  severity,
    NvU32                  attributes,
    NvU32                  scope,
    NvU32                  reportingSource
)
{
    return NV_OK;
}

// GPU_OPERATIONAL_EVENT_CATEGORY_STATE_CHANGED
NV_STATUS gpustatechangedConstruct_IMPL
(
    GpuStateChanged *pEvent,
    EventBus        *pEventBus,
    const char      *pModuleSignature,
    NvU32            eventCode,
    NvU32            severity,
    NvU32            attributes,
    NvU32            scope,
    NvU32            reportingSource,
    NvU32            logLevel
)
{
    return NV_OK;
}

// GPU_OPERATIONAL_EVENT_CATEGORY_RESOURCE_THRESHOLD
NV_STATUS gpuresthresholdConstruct_IMPL
(
    GpuResourceThreshold *pEvent,
    EventBus             *pEventBus,
    const char           *pModuleSignature,
    NvU32                 eventCode,
    NvU32                 severity,
    NvU32                 attributes,
    NvU32                 scope,
    NvU32                 reportingSource,
    NvU32                 logLevel
)
{
    return NV_OK;
}

/* ---------------------------------- GPU Operational Events ---------------------------------- */

static NV_STATUS _gpuDriverInitMetadataSerializeToRmapiOpEvent
(
    EventContextHeader *pContextHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pContextHeader,
        pOutput,
        outputSize,
        pInfo,
        GPU_EVENT_CTX_TYPE_GPU_INIT_METADATA,
        GPU_OPERATIONAL_EVENT_CTX_GPU_INIT_METADATA_VERSION);
}

static NV_STATUS _gpuDriverInitMetadataSerializeToCper
(
    EventContextHeader *pContextHeader,
    NV_CPER_NV_EVENT_SECTION_STATE *pState
)
{
    GpuDriverInitMetadataContext *pContext =
        (GpuDriverInitMetadataContext *)pContextHeader;

    return cperNvidiaEventAddGpuInitMetadataContext(pState, &pContext->data);
}

static const EventContextOutputAdapters g_gpuDriverInitMetadataOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _gpuDriverInitMetadataSerializeToRmapiOpEvent,
    .serializeToCperEventCtx = _gpuDriverInitMetadataSerializeToCper,
};

NV_STATUS gpudrvinitConstruct_IMPL
(
    GpuDriverInit *pEvent,
    EventBus      *pEventBus
)
{
    pEvent->context.header.pOutputAdapters = &g_gpuDriverInitMetadataOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.data);
    // context added by event handler

    return NV_OK;
}

#if !defined(SRT_BUILD) && RMCFG_FEATURE_GSP_CLIENT_RM
NV_STATUS gpudrvinitDeserialize
(
    OBJGPU *pGpu,
    const NV_GPU_OPERATIONAL_EVENT_RECORD *pRecord,
    const NvU8 *pContextData,
    NvU32 contextDataSize
)
{
    static const GpuOpEventExpectedMetadata expected =
    {
        .pModuleSignature = "GPU",
        .moduleEventCode = GPU_OP_EVENT_CODE_DRIVER_INIT,
        .eventCategory = GPU_OPERATIONAL_EVENT_CATEGORY_INITIALIZATION,
        .allowedSeverityMask = GPU_OP_EVENT_SEVERITY_MASK_INFORMATIONAL,
        .attributes = (NvU8)REF_DEF(OPERATIONAL_EVENT_ATTR_PRIMARY, _TRUE),
        .scope = GPU_OPERATIONAL_EVENT_SCOPE_DEVICE,
        .reportingSource = GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK,
        .logLevel = GPU_OPERATIONAL_EVENT_LOG_LEVEL_NOTICE,
        .bReportingSourceVariable = NV_FALSE,
    };
    NV_STATUS status;

    if ((pGpu == NULL) || (pRecord == NULL))
        return NV_ERR_INVALID_ARGUMENT;

    status = gpuOpEventValidateMetadata(pRecord, &expected);
    if (status != NV_OK)
        return status;

    // Init metadata is populated by the existing bus handler after emission.
    // Any structurally valid serialized contexts are intentionally ignored;
    // constructor/subscriber-derived contexts are regenerated on the target.
    (void)pContextData;
    (void)contextDataSize;
    eventTryEmitSimulated(GpuDriverInit, pGpu, status);
    return status;
}
#endif

