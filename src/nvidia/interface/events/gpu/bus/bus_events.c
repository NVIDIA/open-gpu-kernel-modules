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

#include "events/gpu/bus/bus_events.h"

#include "diagnostics/op_event.h"
#include "gpu/gpu.h"
#include "nverror.h"
#include "nvport/nvport.h"

#define BUS_C2C_CONTAINMENT_XID_MESSAGE "C2C containment detected"
#define BUS_GPU_FALLEN_OFF_BUS_XID_MESSAGE "GPU has fallen off the bus."

static void _busC2cContainmentEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    BusC2cContainmentCtx *pContext = (BusC2cContainmentCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, GPU_CONTAINMENT_ERROR,
                 BUS_C2C_CONTAINMENT_XID_MESSAGE ". Containment code 0x%x",
                 pContext->containmentCode);
}

static const EventContextOutputAdapters g_busC2cContainmentOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _busC2cContainmentEmitOsLogXid,
};

static const EventContextOutputAdapters g_busXidOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuXidEventCtxSerializeToEventBufferGoeCtx,
    .serializeToProtobuf = gpuXidEventCtxSerializeToProtobuf,
    .serializeToCperEventCtx = gpuXidEventCtxSerializeToCperCtx,
    .emitToOsLogXid = NULL,
};

static void _busGpuFallenOffBusEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    GpuXidEventCtx *pContext = (GpuXidEventCtx *)pContextHeader;

    // osHandleGpuLost emits Xid 79 synchronously before publishing real events.
    if (!bSimulated)
        return;

    OP_EVENT_XID_LOG(pGpu, bSimulated, pContext->data.xidCode,
                     "%s", pContext->data.message);
}

static const EventContextOutputAdapters g_busGpuFallenOffBusXidOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuXidEventCtxSerializeToEventBufferGoeCtx,
    .serializeToCperEventCtx = gpuXidEventCtxSerializeToCperCtx,
    .emitToOsLogXid = _busGpuFallenOffBusEmitOsLogXid,
};

NV_STATUS busc2ccontnmtConstruct_IMPL
(
    BusC2cContainment *pEvent,
    EventBus *pEventBus,
    NvU32 severity,
    NvU32 containmentCode
)
{
    pEvent->context.header.pOutputAdapters = &g_busC2cContainmentOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.containmentCode);
    pEvent->context.containmentCode = containmentCode;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid179.header.pOutputAdapters = &g_busXidOutputAdapters;
    pEvent->xid179.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid179.data.message,
                      sizeof(pEvent->xid179.data.message),
                      BUS_C2C_CONTAINMENT_XID_MESSAGE ". Containment code 0x%x",
                      containmentCode) + 1);
    pEvent->xid179.data.xidCode = GPU_CONTAINMENT_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid179.header);

    return NV_OK;
}

NV_STATUS busgpufobConstruct_IMPL
(
    BusGpuFallenOffBus *pEvent,
    EventBus *pEventBus
)
{
    pEvent->xid79.header.pOutputAdapters = &g_busGpuFallenOffBusXidOutputAdapters;
    pEvent->xid79.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid79.data.message,
                      sizeof(pEvent->xid79.data.message),
                      BUS_GPU_FALLEN_OFF_BUS_XID_MESSAGE) + 1);
    pEvent->xid79.data.xidCode = ROBUST_CHANNEL_GPU_HAS_FALLEN_OFF_THE_BUS;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid79.header);

    return NV_OK;
}

#if !defined(SRT_BUILD) && RMCFG_FEATURE_GSP_CLIENT_RM
NV_STATUS busgpufobDeserialize
(
    OBJGPU *pGpu,
    const NV_GPU_OPERATIONAL_EVENT_RECORD *pRecord,
    const NvU8 *pContextData,
    NvU32 contextDataSize
)
{
    static const GpuOpEventExpectedMetadata expected =
    {
        .pModuleSignature = "GPU-BUS",
        .moduleEventCode = BUS_OP_EVENT_CODE_GPU_FALLEN_OFF_BUS,
        .eventCategory = GPU_OPERATIONAL_EVENT_CATEGORY_INTERCONNECT_ERROR,
        .allowedSeverityMask = GPU_OP_EVENT_SEVERITY_MASK_FATAL,
        .attributes = (NvU8)REF_DEF(OPERATIONAL_EVENT_ATTR_PRIMARY, _TRUE),
        .scope = GPU_OPERATIONAL_EVENT_SCOPE_DEVICE,
        .reportingSource = GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK,
        .logLevel = GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR,
        .bReportingSourceVariable = NV_FALSE,
    };
    NV_STATUS status;

    if ((pGpu == NULL) || (pRecord == NULL))
        return NV_ERR_INVALID_ARGUMENT;

    status = gpuOpEventValidateMetadata(pRecord, &expected);
    if (status != NV_OK)
        return status;

    (void)pContextData;
    (void)contextDataSize;
    eventTryEmitSimulated(BusGpuFallenOffBus, pGpu, status);
    return status;
}

NV_STATUS busc2ccontnmtDeserialize
(
    OBJGPU *pGpu,
    const NV_GPU_OPERATIONAL_EVENT_RECORD *pRecord,
    const NvU8 *pContextData,
    NvU32 contextDataSize
)
{
    static const GpuOpEventExpectedMetadata expected =
    {
        .pModuleSignature = "GPU-BUS",
        .moduleEventCode = BUS_OP_EVENT_CODE_C2C_CONTAINMENT,
        .eventCategory = GPU_OPERATIONAL_EVENT_CATEGORY_INTERCONNECT_ERROR,
        .allowedSeverityMask =
            GPU_OP_EVENT_SEVERITY_MASK_RECOVERABLE |
            GPU_OP_EVENT_SEVERITY_MASK_FATAL,
        .attributes = (NvU8)REF_DEF(OPERATIONAL_EVENT_ATTR_PRIMARY, _TRUE),
        .scope = GPU_OPERATIONAL_EVENT_SCOPE_DEVICE,
        .reportingSource = GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_TIMEOUT,
        .logLevel = GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR,
        .bReportingSourceVariable = NV_FALSE,
    };
    const NvU8 *pPayload;
    NvU32 payloadSize;
    BUS_C2C_CONTAINMENT_CODE containmentCode;
    NV_STATUS status;

    if ((pGpu == NULL) || (pRecord == NULL))
        return NV_ERR_INVALID_ARGUMENT;

    status = gpuOpEventValidateMetadata(pRecord, &expected);
    if (status != NV_OK)
        return status;

    status = gpuOpEventFindContext(pRecord, pContextData, contextDataSize,
                                   OPERATIONAL_EVENT_CTX_TYPE_OPAQUE, 0,
                                   &pPayload, &payloadSize);
    if (status != NV_OK)
        return status;
    if (payloadSize != sizeof(containmentCode))
        return NV_ERR_INVALID_DATA;

    portMemCopy(&containmentCode, sizeof(containmentCode),
                pPayload, sizeof(containmentCode));
    if ((containmentCode != BUS_C2C_CONTAINMENT_CODE_SW) &&
        (containmentCode != BUS_C2C_CONTAINMENT_CODE_HW))
    {
        return NV_ERR_INVALID_DATA;
    }

    eventTryEmitSimulated(BusC2cContainment, pGpu, status,
                          pRecord->event.severity, containmentCode);
    return status;
}
#endif

