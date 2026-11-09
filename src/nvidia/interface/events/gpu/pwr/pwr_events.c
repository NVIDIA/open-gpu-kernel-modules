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

#include "events/gpu/pwr/pwr_events.h"

#include "diagnostics/op_event.h"
#include "gpu/gpu.h"
#include "nverror.h"
#include "nvport/nvport.h"

// Xid message strings written to the OS log (dmesg) sink by the adapters below.
#define PSHC_REENGAGED_XID_MESSAGE_FMT \
    "PSHC reengaged\n"
#define PSHC_DISENGAGED_SLOWDOWN_XID_MESSAGE_FMT \
    "PSHC disengaged due to slowdown event - 0x%X\n"
#define PSHC_DISENGAGED_TIMEOUT_XID_MESSAGE_FMT \
    "PSHC disengaged due to timeout event\n"
#define PSHC_DISENGAGED_ZERO_LIFETIME_XID_MESSAGE_FMT \
    "PSHC disengaged due to zero lifetime\n"
#define PSHC_DISENGAGED_UNKNOWN_XID_MESSAGE_FMT \
    "PSHC disengaged due to unknown reason (0x%X)\n"
#define PSHC_LOW_LIFETIME_XID_MESSAGE_FMT \
    "PSHC lifetime is lower than the threshold of %u %%\n"
#define PSHC_ZERO_LIFETIME_XID_MESSAGE_FMT \
    "PSHC lifetime is zero. PSHC will be disabled\n"

// Shared Xid-context output adapters (legacy Xid carried as a GPU_LEGACY_XID context).
static const EventContextOutputAdapters g_pshcXidOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuXidEventCtxSerializeToEventBufferGoeCtx,
    .serializeToCperEventCtx = gpuXidEventCtxSerializeToCperCtx,
    .emitToOsLogXid = NULL,
};

/* ------------------------------- PSHC Reengaged (Xid 162) ------------------------------- */
static void _pshcReengagedEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    (void)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ISINK_REENGAGED, PSHC_REENGAGED_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_pshcReengagedOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _pshcReengagedEmitOsLogXid,
};

NV_STATUS pshcreengagedConstruct_IMPL
(
    PshcReengaged *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.header.pOutputAdapters = &g_pshcReengagedOutputAdapters;
    pEvent->context.header.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid162.header.pOutputAdapters = &g_pshcXidOutputAdapters;
    pEvent->xid162.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid162.data.message,
                      sizeof(pEvent->xid162.data.message),
                      PSHC_REENGAGED_XID_MESSAGE_FMT) + 1);
    pEvent->xid162.data.xidCode = ISINK_REENGAGED;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid162.header);

    return NV_OK;
}

/* ------------------------------- PSHC Disengaged (Xid 163) ------------------------------ */
static void _pshcDisengagedEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    PshcDisengagedCtx *pContext = (PshcDisengagedCtx *)pContextHeader;

    switch (pContext->reason)
    {
        case PSHC_DISENGAGE_REASON_SLOWDOWN:
            OP_EVENT_XID_LOG(pGpu, bSimulated, ISINK_DISENGAGED,
                             PSHC_DISENGAGED_SLOWDOWN_XID_MESSAGE_FMT, pContext->slowdownMask);
            break;
        case PSHC_DISENGAGE_REASON_TIMEOUT:
            OP_EVENT_XID_LOG(pGpu, bSimulated, ISINK_DISENGAGED, PSHC_DISENGAGED_TIMEOUT_XID_MESSAGE_FMT);
            break;
        case PSHC_DISENGAGE_REASON_ZERO_LIFETIME:
            OP_EVENT_XID_LOG(pGpu, bSimulated, ISINK_DISENGAGED, PSHC_DISENGAGED_ZERO_LIFETIME_XID_MESSAGE_FMT);
            break;
        default:
            OP_EVENT_XID_LOG(pGpu, bSimulated, ISINK_DISENGAGED,
                             PSHC_DISENGAGED_UNKNOWN_XID_MESSAGE_FMT, pContext->reason);
            break;
    }
}

static const EventContextOutputAdapters g_pshcDisengagedOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _pshcDisengagedEmitOsLogXid,
};

NV_STATUS pshcdisengagedConstruct_IMPL
(
    PshcDisengaged *pEvent,
    EventBus *pEventBus,
    NvU32 reason,
    NvU32 slowdownMask
)
{
    pEvent->context.header.pOutputAdapters = &g_pshcDisengagedOutputAdapters;
    pEvent->context.reason = reason;
    pEvent->context.slowdownMask = slowdownMask;
    pEvent->context.header.dataSize = sizeof(pEvent->context.reason) +
                                      sizeof(pEvent->context.slowdownMask);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid163.header.pOutputAdapters = &g_pshcXidOutputAdapters;
    switch (reason)
    {
        case PSHC_DISENGAGE_REASON_SLOWDOWN:
            pEvent->xid163.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
                nvDbgSnprintf(pEvent->xid163.data.message,
                              sizeof(pEvent->xid163.data.message),
                              PSHC_DISENGAGED_SLOWDOWN_XID_MESSAGE_FMT, slowdownMask) + 1);
            break;
        case PSHC_DISENGAGE_REASON_TIMEOUT:
            pEvent->xid163.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
                nvDbgSnprintf(pEvent->xid163.data.message,
                              sizeof(pEvent->xid163.data.message),
                              PSHC_DISENGAGED_TIMEOUT_XID_MESSAGE_FMT) + 1);
            break;
        case PSHC_DISENGAGE_REASON_ZERO_LIFETIME:
            pEvent->xid163.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
                nvDbgSnprintf(pEvent->xid163.data.message,
                              sizeof(pEvent->xid163.data.message),
                              PSHC_DISENGAGED_ZERO_LIFETIME_XID_MESSAGE_FMT) + 1);
            break;
        default:
            pEvent->xid163.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
                nvDbgSnprintf(pEvent->xid163.data.message,
                              sizeof(pEvent->xid163.data.message),
                              PSHC_DISENGAGED_UNKNOWN_XID_MESSAGE_FMT, reason) + 1);
            break;
    }
    pEvent->xid163.data.xidCode = ISINK_DISENGAGED;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid163.header);

    return NV_OK;
}

/* ------------------------------ PSHC Low Lifetime (Xid 164) ----------------------------- */
static void _pshcLowLifetimeEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    PshcLowLifetimeCtx *pContext = (PshcLowLifetimeCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ISINK_LOW_LIFETIME,
                     PSHC_LOW_LIFETIME_XID_MESSAGE_FMT, pContext->thresholdPct);
}

static const EventContextOutputAdapters g_pshcLowLifetimeOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _pshcLowLifetimeEmitOsLogXid,
};

NV_STATUS pshclowlifetimeConstruct_IMPL
(
    PshcLowLifetime *pEvent,
    EventBus *pEventBus,
    NvU32 thresholdPct
)
{
    pEvent->context.header.pOutputAdapters = &g_pshcLowLifetimeOutputAdapters;
    pEvent->context.thresholdPct = thresholdPct;
    pEvent->context.header.dataSize = sizeof(pEvent->context.thresholdPct);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid164.header.pOutputAdapters = &g_pshcXidOutputAdapters;
    pEvent->xid164.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid164.data.message,
                      sizeof(pEvent->xid164.data.message),
                      PSHC_LOW_LIFETIME_XID_MESSAGE_FMT, pEvent->context.thresholdPct) + 1);
    pEvent->xid164.data.xidCode = ISINK_LOW_LIFETIME;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid164.header);

    return NV_OK;
}

/* ------------------------------ PSHC Zero Lifetime (Xid 165) ---------------------------- */
static void _pshcZeroLifetimeEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    (void)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ISINK_ZERO_LIFETIME, PSHC_ZERO_LIFETIME_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_pshcZeroLifetimeOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _pshcZeroLifetimeEmitOsLogXid,
};

NV_STATUS pshczerolifetimeConstruct_IMPL
(
    PshcZeroLifetime *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.header.pOutputAdapters = &g_pshcZeroLifetimeOutputAdapters;
    pEvent->context.header.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid165.header.pOutputAdapters = &g_pshcXidOutputAdapters;
    pEvent->xid165.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid165.data.message,
                      sizeof(pEvent->xid165.data.message),
                      PSHC_ZERO_LIFETIME_XID_MESSAGE_FMT) + 1);
    pEvent->xid165.data.xidCode = ISINK_ZERO_LIFETIME;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid165.header);

    return NV_OK;
}

