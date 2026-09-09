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

static void _busC2cContainmentEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu
)
{
    BusC2cContainmentCtx *pContext = (BusC2cContainmentCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, GPU_CONTAINMENT_ERROR,
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
    .serializeToCperEventCtx = gpuXidEventCtxSerializeToCperCtx,
    .emitToOsLogXid = NULL,
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

