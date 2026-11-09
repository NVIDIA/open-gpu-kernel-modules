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
#include "nvmisc.h"
#include "nvctassert.h"
#include "utils/nvassert.h"
#include "libraries/cper/gpu_cper.h"
#include "class/cl90d0.h"
#include "events/event_defs.h"
#include "events/gpu/gpu_events.h"

#define GPU_OP_EVENT_PROTOBUF_SUPPORTED 0

#if !defined(SRT_BUILD) && RMCFG_FEATURE_GSP_CLIENT_RM
#include "events/gpu/bus/bus_events.h"
#include "events/gpu/mem_sys/mem_sys_events.h"
#include "events/gpu/fsp/fsp_events.h"
#include "events/gpu/ras/ras_events.h"
#endif

ct_assert(sizeof(OPERATIONAL_EVENT_GROUP_ATTRIBUTES) == sizeof(NvU8));
ct_assert(DRF_SHIFTMASK(OPERATIONAL_EVENT_GROUP_ATTR_RESERVED) <= NV_U8_MAX);

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
        // Physical RM reads PDI from the GPU, so retain the UUID but leave PDI
        // zero after disconnect.
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_CONNECTED))
        {
            (void)gpuGetPdi_HAL(pGpu, &pEvent->pdi);
        }
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

/* ------------------------------ Simulated event injection (test) ----------------------------- */

//
// Injection is a CPU-RM (host) test facility driven by the kernel subdevice
// control. Gate it like the op-event import path (GSP_CLIENT_RM) so it is not
// compiled into firmware images (GSP/DCE), where the concrete event classes'
// NVOC objCreate symbols are not linked.
//
#if !defined(SRT_BUILD) && RMCFG_FEATURE_GSP_CLIENT_RM

typedef NV_STATUS (*GpuOpEventDeserializer)(
    OBJGPU *pGpu,
    const NV_GPU_OPERATIONAL_EVENT_RECORD *pRecord,
    const NvU8 *pContextData,
    NvU32 contextDataSize);

typedef struct
{
    char moduleSignature[OPERATIONAL_EVENT_MODULE_SIGNATURE_LEN];
    NvU16 moduleEventCode;
    GpuOpEventDeserializer pDeserialize;
} GpuOpEventDeserializerEntry;

static NvU32
_gpuOpEventGetSeverityMask
(
    NvU8 severity
)
{
    switch (severity)
    {
        case OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL:
            return GPU_OP_EVENT_SEVERITY_MASK_INFORMATIONAL;
        case OPERATIONAL_EVENT_SEVERITY_CORRECTED:
            return GPU_OP_EVENT_SEVERITY_MASK_CORRECTED;
        case OPERATIONAL_EVENT_SEVERITY_RECOVERABLE:
            return GPU_OP_EVENT_SEVERITY_MASK_RECOVERABLE;
        case OPERATIONAL_EVENT_SEVERITY_FATAL:
            return GPU_OP_EVENT_SEVERITY_MASK_FATAL;
        default:
            return 0U;
    }
}

static NvBool
_gpuOpEventIsValidReportingSource
(
    NvU8 reportingSource
)
{
    switch (reportingSource)
    {
        case GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK:
        case GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT:
        case GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_FW_FAULT:
        case GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_TIMEOUT:
        case GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_DRIVER_CLIENT:
        case GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_UNKNOWN:
            return NV_TRUE;
        default:
            return NV_FALSE;
    }
}

NV_STATUS
gpuOpEventValidateMetadata
(
    const NV_GPU_OPERATIONAL_EVENT_RECORD *pRecord,
    const GpuOpEventExpectedMetadata *pExpected
)
{
    NvU32 severityMask;

    NV_ASSERT_OR_RETURN(pRecord != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pExpected != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pExpected->pModuleSignature != NULL,
                        NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(
        (pExpected->allowedSeverityMask != 0) &&
        ((pExpected->allowedSeverityMask &
          ~GPU_OP_EVENT_SEVERITY_MASK_ALL) == 0),
        NV_ERR_INVALID_ARGUMENT);

    if ((pRecord->event.moduleEventCode != pExpected->moduleEventCode) ||
        (pRecord->event.eventCategory != pExpected->eventCategory) ||
        (pRecord->event.attributes != pExpected->attributes) ||
        (pRecord->scope != pExpected->scope) ||
        (pRecord->logLevel != pExpected->logLevel) ||
        (portStringCompare(pRecord->event.moduleSignature,
                           pExpected->pModuleSignature,
                           sizeof(pRecord->event.moduleSignature)) != 0))
    {
        return NV_ERR_INVALID_DATA;
    }

    severityMask = _gpuOpEventGetSeverityMask(pRecord->event.severity);
    if ((severityMask == 0U) ||
        ((pExpected->allowedSeverityMask & severityMask) == 0))
    {
        return NV_ERR_INVALID_DATA;
    }

    if (pExpected->bReportingSourceVariable)
    {
        if (!_gpuOpEventIsValidReportingSource(pRecord->reportingSource))
            return NV_ERR_INVALID_DATA;
    }
    else if (pRecord->reportingSource != pExpected->reportingSource)
    {
        return NV_ERR_INVALID_DATA;
    }

    return NV_OK;
}

//
// The registry is deliberately data, not control flow. Event-specific parsing
// stays beside the event constructor in its owning module; adding an injectable
// event requires one registry row and one module deserializer, not another arm
// in a central constructor switch.
//
static const GpuOpEventDeserializerEntry g_gpuOpEventDeserializers[] =
{
#define GPU_EVENT_DESERIALIZER(moduleSignature, moduleEventCode, deserialize) \
    { moduleSignature, moduleEventCode, deserialize },
#include "events/gpu/gpu_event_deserializer_entries.h"
#undef GPU_EVENT_DESERIALIZER
};

static NV_STATUS
_gpuOpEventValidateContexts
(
    const NV_GPU_OPERATIONAL_EVENT_RECORD *pRecord,
    const NvU8 *pContextData
)
{
    NvU32 offset = 0;

    if ((pRecord->event.totalContextSize != 0U) && (pContextData == NULL))
        return NV_ERR_INVALID_ARGUMENT;

    for (NvU32 i = 0; i < pRecord->event.numContexts; i++)
    {
        NV_OPERATIONAL_EVENT_CONTEXT context;
        NvU32 contextSize;
        NvU32 paddedSize;

        if (offset > pRecord->event.totalContextSize ||
            sizeof(context) > pRecord->event.totalContextSize - offset)
            return NV_ERR_INVALID_DATA;

        portMemCopy(&context, sizeof(context), pContextData + offset, sizeof(context));

        if (context.dataSize > NV_U32_MAX - (NvU32)sizeof(context))
            return NV_ERR_INVALID_DATA;
        contextSize = (NvU32)sizeof(context) + context.dataSize;

        if (contextSize > pRecord->event.totalContextSize - offset)
            return NV_ERR_INVALID_DATA;
        if (contextSize > NV_U32_MAX - (NV_EVENT_VARDATA_GRANULARITY - 1U))
            return NV_ERR_INVALID_DATA;

        paddedSize = NV_ALIGN_UP(contextSize, NV_EVENT_VARDATA_GRANULARITY);
        if (paddedSize > pRecord->event.totalContextSize - offset)
            return NV_ERR_INVALID_DATA;

        offset += paddedSize;
    }

    return (offset == pRecord->event.totalContextSize) ? NV_OK : NV_ERR_INVALID_DATA;
}

NV_STATUS
gpuOpEventFindContext
(
    const NV_GPU_OPERATIONAL_EVENT_RECORD *pRecord,
    const NvU8 *pContextData,
    NvU32 contextDataSize,
    NvU16 contextType,
    NvU16 dataFormatVersion,
    const NvU8 **ppPayload,
    NvU32 *pPayloadSize
)
{
    const NvU8 *pMatch = NULL;
    NvU32 matchSize = 0;
    NvU32 offset = 0;

    NV_ASSERT_OR_RETURN(pRecord != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(ppPayload != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pPayloadSize != NULL, NV_ERR_INVALID_ARGUMENT);
    if (contextDataSize != pRecord->event.totalContextSize)
        return NV_ERR_INVALID_DATA;
    if ((contextDataSize != 0U) && (pContextData == NULL))
        return NV_ERR_INVALID_ARGUMENT;
    if (_gpuOpEventValidateContexts(pRecord, pContextData) != NV_OK)
        return NV_ERR_INVALID_DATA;

    for (NvU32 i = 0; i < pRecord->event.numContexts; i++)
    {
        NV_OPERATIONAL_EVENT_CONTEXT context;
        NvU32 contextSize;

        portMemCopy(&context, sizeof(context), pContextData + offset, sizeof(context));
        contextSize = (NvU32)sizeof(context) + context.dataSize;

        if (context.contextType == contextType)
        {
            if ((context.dataFormatVersion != dataFormatVersion) ||
                (pMatch != NULL))
            {
                return NV_ERR_INVALID_DATA;
            }

            pMatch = pContextData + offset + sizeof(context);
            matchSize = context.dataSize;
        }

        offset += NV_ALIGN_UP(contextSize, NV_EVENT_VARDATA_GRANULARITY);
    }

    if (pMatch == NULL)
        return NV_ERR_INVALID_DATA;

    *ppPayload = pMatch;
    *pPayloadSize = matchSize;
    return NV_OK;
}

NV_STATUS
gpuOpEventInjectSimulated
(
    OBJGPU *pGpu,
    const NvU8 *pRecordData,
    NvU32 recordSize
)
{
    NV_GPU_OPERATIONAL_EVENT_RECORD record;
    const NvU8 *pContextData;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pRecordData != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pGpu->pEventBus != NULL, NV_ERR_INVALID_STATE);

    if (recordSize < sizeof(record))
        return NV_ERR_INVALID_ARGUMENT;

    portMemCopy(&record, sizeof(record), pRecordData, sizeof(record));
    pContextData = pRecordData + sizeof(record);

    if ((record.event.header.type != NV_EVENT_BUFFER_RECORD_TYPE_OPERATIONAL_EVENT_GOE) ||
        (record.event.header.subtype != NV_OPERATIONAL_EVENT_RECORD_SUBTYPE_GOE_V1))
        return NV_ERR_INVALID_DATA;

    if ((record.event.groupSize == 0U) ||
        (record.event.groupIndex >= record.event.groupSize))
        return NV_ERR_INVALID_DATA;

    if ((record.event.groupSize != 1U) || (record.event.groupIndex != 0U))
        return NV_ERR_NOT_SUPPORTED;

    if (record.event.totalContextSize != recordSize - sizeof(record))
        return NV_ERR_INVALID_DATA;

    status = _gpuOpEventValidateContexts(&record, pContextData);
    if (status != NV_OK)
        return status;

    // Deserializers select and validate only contexts needed by the concrete
    // constructor. Other contexts remain untrusted and are regenerated by the
    // constructor or target-side subscribers rather than copied into the event.
    for (NvU32 i = 0; i < NV_ARRAY_ELEMENTS(g_gpuOpEventDeserializers); i++)
    {
        const GpuOpEventDeserializerEntry *pEntry = &g_gpuOpEventDeserializers[i];

        if ((record.event.moduleEventCode == pEntry->moduleEventCode) &&
            (portStringCompare(record.event.moduleSignature, pEntry->moduleSignature,
                               sizeof(record.event.moduleSignature)) == 0))
        {
            return pEntry->pDeserialize(pGpu, &record, pContextData,
                                        record.event.totalContextSize);
        }
    }

    return NV_ERR_NOT_SUPPORTED;
}

#endif // !defined(SRT_BUILD) && RMCFG_FEATURE_GSP_CLIENT_RM

/* ------------------------- Generic Event Context Serializer Helpers ------------------------- */

NV_STATUS gpuEventCtxSerializeToEventBufferGoeOpaqueCtx
(
    EventContextHeader *pCtxHeader,
    NV_GPU_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(pCtxHeader, pOutput, outputSize,
                                                       pInfo,
                                                       OPERATIONAL_EVENT_CTX_TYPE_OPAQUE, 0);
}

NV_STATUS gpuEventCtxSerializeToEventBufferGoeCtxType
(
    EventContextHeader *pCtxHeader,
    NV_GPU_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo,
    GPU_OPERATIONAL_EVENT_CTX_TYPE ctxType,
    NvU16 dataFormatVersion
)
{
    NvU32 totalSize;

    NV_ASSERT_OR_RETURN(pInfo != NULL, NV_ERR_INVALID_POINTER);

    if (pCtxHeader->dataSize > NV_U32_MAX - sizeof(*pOutput))
        return NV_ERR_INVALID_ARGUMENT;

    totalSize = sizeof(*pOutput) + pCtxHeader->dataSize;
    pInfo->wireSize = totalSize;
    pInfo->contextType = ctxType;
    pInfo->dataFormatVersion = dataFormatVersion;
    pInfo->dataSize = pCtxHeader->dataSize;

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
    EventBufferOpEventCtxInfo *pInfo
)
{
    GpuXidEventCtx *pXidCtx = (GpuXidEventCtx *)pCtxHeader;
    GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID *pOutputXidContext;
    NvU32 messageSize;
    NvU32 payloadSize;
    NvU32 totalSize;
    const NvU32 maxMessageLen = GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_MAX_MSG_LEN;

    NV_ASSERT_OR_RETURN(pInfo != NULL, NV_ERR_INVALID_POINTER);

    messageSize = (NvU32)portStringLengthSafe(pXidCtx->data.message, maxMessageLen) + 1;
    payloadSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(messageSize);
    totalSize = sizeof(*pOutputCtx) + payloadSize;
    pInfo->wireSize = totalSize;
    pInfo->contextType = GPU_OPERATIONAL_EVENT_CTX_TYPE_GPU_LEGACY_XID;
    pInfo->dataFormatVersion = GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_VERSION;
    pInfo->dataSize = payloadSize;

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

NV_STATUS gpuXidEventCtxSerializeToProtobuf
(
    EventContextHeader *pCtxHeader,
    PRB_ENCODER         *pPrbEnc,
    NvU32              *pContextType,
    NvU32              *pDataFormatVersion
)
{
    NV_ASSERT_OR_RETURN(pCtxHeader != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pPrbEnc != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pContextType != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pDataFormatVersion != NULL, NV_ERR_INVALID_POINTER);

    *pContextType = GPU_OPERATIONAL_EVENT_CTX_TYPE_GPU_LEGACY_XID;
    *pDataFormatVersion = GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_VERSION;

#if GPU_OP_EVENT_PROTOBUF_SUPPORTED
    GpuXidEventCtx *pXidCtx = (GpuXidEventCtx *)pCtxHeader;
    NV_STATUS       status;

    status = prbEncNestedStart(
        pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_CONTEXT_GPU_LEGACY_XID);
    if (status != NV_OK)
        return status;

    status = prbEncAddUInt32(
        pPrbEnc, GPUEVENTS_GPULEGACYXIDCONTEXT_XID_CODE,
        pXidCtx->data.xidCode);
    if (status != NV_OK)
        return status;

    status = prbEncAddString(
        pPrbEnc, GPUEVENTS_GPULEGACYXIDCONTEXT_MESSAGE,
        pXidCtx->data.message);
    if (status != NV_OK)
        return status;

    return prbEncNestedEnd(pPrbEnc);
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
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

