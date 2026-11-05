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

#include "events/gpu/gsp/gsp_events.h"

#include "diagnostics/op_event.h"
#include "gpu/gpu.h"
#include "gpu/gsp/kernel_gsp.h"
#include "nvport/nvport.h"
#include "nverror.h"
#include "utils/nvassert.h"

#define GSP_EVENTS_PROTOBUF_SUPPORTED 0

#define GSP_RPC_TIMEOUT_XID_MESSAGE_FMT \
    "Timeout after %llus of waiting for RPC response from GPU%u GSP! Expected function %u (%s) sequence %u (0x%llx 0x%llx)."

#define GSP_POISON_XID_MESSAGE_FMT "Poison error in GSP."

static NV_STATUS _gspRpcTimeoutSerializeToEventBufferOpEventCtx
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
            GSP_EVENT_CTX_TYPE_RPC_TIMEOUT_DATA,
            GSP_RPC_TIMEOUT_DATA_VERSION);
}

static NV_STATUS _gspRpcTimeoutSerializeToProtobuf
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

    *pContextType = GSP_EVENT_CTX_TYPE_RPC_TIMEOUT_DATA;
    *pDataFormatVersion = GSP_RPC_TIMEOUT_DATA_VERSION;

#if GSP_EVENTS_PROTOBUF_SUPPORTED
    GspRpcTimeoutCtx *pContext = (GspRpcTimeoutCtx *)pContextHeader;
    NV_STATUS         status;

    status = prbEncNestedStart(
        pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_CONTEXT_GSP_RPC_TIMEOUT);
    if (status != NV_OK)
        return status;

    status = prbEncAddUInt32(
        pPrbEnc, GSPEVENTS_GSPRPCTIMEOUTCONTEXT_EXPECTED_FUNCTION,
        pContext->data.expectedFunc);
    if (status != NV_OK)
        return status;

    status = prbEncAddUInt32(
        pPrbEnc, GSPEVENTS_GSPRPCTIMEOUTCONTEXT_EXPECTED_SEQUENCE,
        pContext->data.expectedSequence);
    if (status != NV_OK)
        return status;

    status = prbEncAddUInt32(
        pPrbEnc, GSPEVENTS_GSPRPCTIMEOUTCONTEXT_GPU_INSTANCE,
        pContext->data.gpuInstance);
    if (status != NV_OK)
        return status;

    status = prbEncAddUInt64(
        pPrbEnc, GSPEVENTS_GSPRPCTIMEOUTCONTEXT_HISTORY_DATA_0,
        pContext->data.historyData0);
    if (status != NV_OK)
        return status;

    status = prbEncAddUInt64(
        pPrbEnc, GSPEVENTS_GSPRPCTIMEOUTCONTEXT_HISTORY_DATA_1,
        pContext->data.historyData1);
    if (status != NV_OK)
        return status;

    status = prbEncAddUInt64(
        pPrbEnc, GSPEVENTS_GSPRPCTIMEOUTCONTEXT_WAITED_SECONDS,
        pContext->data.waitedSeconds);
    if (status != NV_OK)
        return status;

    status = prbEncAddString(
        pPrbEnc, GSPEVENTS_GSPRPCTIMEOUTCONTEXT_RPC_NAME,
        pContext->data.rpcName);
    if (status != NV_OK)
        return status;

    return prbEncNestedEnd(pPrbEnc);
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

static void _rpcTimeoutDataEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    GspRpcTimeoutCtx *pContext = (GspRpcTimeoutCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, GSP_RPC_TIMEOUT, GSP_RPC_TIMEOUT_XID_MESSAGE_FMT,
                 pContext->data.waitedSeconds,
                 pContext->data.gpuInstance,
                 pContext->data.expectedFunc,
                 pContext->data.rpcName,
                 pContext->data.expectedSequence,
                 pContext->data.historyData0,
                 pContext->data.historyData1);
}

static const EventContextOutputAdapters g_gspRpcTimeoutCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _gspRpcTimeoutSerializeToEventBufferOpEventCtx,
    .serializeToProtobuf = _gspRpcTimeoutSerializeToProtobuf,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _rpcTimeoutDataEmitOsLogXid,
};

//
// The GspRpcTimeout context emits the Xid 119 to OS logs, because it has the data context
// required to do so directly, so emitToOsLogXid is not implemented for GpuXidEventCtx.
// The serializers for this context exist to produce the legacy Xid compatibility contexts in
// CPER and EventBuffer output using a pre-rendered string built at event construction time.
//
static const EventContextOutputAdapters g_xidOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuXidEventCtxSerializeToEventBufferGoeCtx,
    .serializeToProtobuf = gpuXidEventCtxSerializeToProtobuf,
    .serializeToCperEventCtx = gpuXidEventCtxSerializeToCperCtx,
    .emitToOsLogXid = NULL,
};

static void _gspFirmwareFaultEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    GpuXidEventCtx *pContext = (GpuXidEventCtx *)pContextHeader;

    // CrashCat emits Xid 120 for real faults before publishing this event.
    if (!bSimulated)
        return;

    OP_EVENT_XID_LOG(pGpu, bSimulated, pContext->data.xidCode,
                     "%s", pContext->data.message);
}

static const EventContextOutputAdapters g_gspFirmwareFaultXidOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuXidEventCtxSerializeToEventBufferGoeCtx,
    .serializeToProtobuf = gpuXidEventCtxSerializeToProtobuf,
    .serializeToCperEventCtx = gpuXidEventCtxSerializeToCperCtx,
    .emitToOsLogXid = _gspFirmwareFaultEmitOsLogXid,
};

static void _gspPoisonEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    OP_EVENT_XID_LOG(pGpu, bSimulated, UNRECOVERABLE_ECC_ERROR_ESCAPE, GSP_POISON_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_gspPoisonOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _gspPoisonEmitOsLogXid,
};

NV_STATUS gsprpctoConstruct_IMPL
(
    GspRpcTimeout *pEvent,
    EventBus *pEventBus,
    NvU32 severity,
    NvU64 timeoutNs,
    NvU64 elapsedNs,
    NvU32 gpuInstance,
    NvU32 expectedFunc,
    const char *pRpcName,
    NvU32 expectedSequence,
    NvU64 historyData0,
    NvU64 historyData1,
    NvU64 waitedSeconds
)
{
    // Populate and add context
    pEvent->context.header.pOutputAdapters = &g_gspRpcTimeoutCtxOutputAdapters;
    pEvent->context.header.dataSize = (NvU32)GSP_RPC_TIMEOUT_DATA_SIZE(
        portStringCopy(pEvent->context.data.rpcName,
                       sizeof(pEvent->context.data.rpcName),
                       (pRpcName != NULL) ? pRpcName : "unknown",
                       sizeof(pEvent->context.data.rpcName)));
    pEvent->context.data.gpuInstance = gpuInstance;
    pEvent->context.data.expectedFunc = expectedFunc;
    pEvent->context.data.expectedSequence = expectedSequence;
    pEvent->context.data.historyData0 = historyData0;
    pEvent->context.data.historyData1 = historyData1;
    pEvent->context.data.waitedSeconds = waitedSeconds;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Populate and add xid119
    pEvent->xid119.header.pOutputAdapters = &g_xidOutputAdapters;
    pEvent->xid119.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid119.data.message,
                      sizeof(pEvent->xid119.data.message),
                      GSP_RPC_TIMEOUT_XID_MESSAGE_FMT,
                      pEvent->context.data.waitedSeconds,
                      pEvent->context.data.gpuInstance,
                      pEvent->context.data.expectedFunc,
                      pEvent->context.data.rpcName,
                      pEvent->context.data.expectedSequence,
                      pEvent->context.data.historyData0,
                      pEvent->context.data.historyData1) + 1);
    pEvent->xid119.data.xidCode = GSP_RPC_TIMEOUT;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid119.header);

    return NV_OK;
}

static void _gspHeartbeatTimeoutEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    GspHeartbeatTimeoutCtx *pContext = (GspHeartbeatTimeoutCtx *)pContextHeader;
    OP_EVENT_XID_LOG(pGpu, bSimulated, GSP_ERROR, "%s Heartbeat Timeout detected", pContext->pHeartbeatSource);
}

static const EventContextOutputAdapters g_gspHeartbeatTimeoutOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = NULL, // No additional data to serialize
    .serializeToCperEventCtx = NULL, // No additional data to serialize
    .emitToOsLogXid = _gspHeartbeatTimeoutEmitOsLogXid,
};

NV_STATUS gsphbtoConstruct_IMPL
(
    GspHeartbeatTimeout *pEvent,
    EventBus *pEventBus,
    NvU32 eventCode,
    const char *pHeartbeatSource,
    NvU32 severity,
    NvU64 timeoutNs,
    NvU64 elapsedNs
)
{
    pEvent->context.header.pOutputAdapters = &g_gspHeartbeatTimeoutOutputAdapters;
    pEvent->context.header.dataSize = (NvU32)sizeof(pEvent->context.pHeartbeatSource);
    pEvent->context.pHeartbeatSource = pHeartbeatSource;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid120.header.pOutputAdapters = &g_xidOutputAdapters;
    pEvent->xid120.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid120.data.message,
                      sizeof(pEvent->xid120.data.message),
                      "%s Heartbeat Timeout detected",
                      pHeartbeatSource) + 1);
    pEvent->xid120.data.xidCode = GSP_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid120.header);

    return NV_OK;
}

NV_STATUS gspfwfaultConstruct_IMPL
(
    GspFirmwareFault *pEvent,
    EventBus *pEventBus
)
{
    // No GspFirmwareFaultCtx yet, to avoid duplicate Xid 120 emission.

    pEvent->xid120.header.pOutputAdapters = &g_gspFirmwareFaultXidOutputAdapters;
    pEvent->xid120.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid120.data.message,
                      sizeof(pEvent->xid120.data.message),
                      "GSP firmware fault detected") + 1);
    pEvent->xid120.data.xidCode = GSP_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid120.header);
    return NV_OK;
}

NV_STATUS gsppoisonConstruct_IMPL
(
    GspPoison *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.header.pOutputAdapters = &g_gspPoisonOutputAdapters;
    pEvent->context.header.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid140.header.pOutputAdapters = &g_xidOutputAdapters;
    pEvent->xid140.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid140.data.message,
                      sizeof(pEvent->xid140.data.message),
                      GSP_POISON_XID_MESSAGE_FMT) + 1);
    pEvent->xid140.data.xidCode = UNRECOVERABLE_ECC_ERROR_ESCAPE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid140.header);

    return NV_OK;
}

