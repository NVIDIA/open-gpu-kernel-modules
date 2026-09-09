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

#include "events/gpu/mem_sys/mem_sys_events.h"

#include "diagnostics/op_event.h"
#include "gpu/gpu.h"
#include "nverror.h"
#include "nvport/nvport.h"

#define MEM_SYS_TIMEOUT_XID_MESSAGE_FMT "GPU memory subsystem timeout (type %u) detected"

static void _memSysTimeoutEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu
)
{
    MemSysTimeoutCtx *pContext = (MemSysTimeoutCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, MEMORY_SUBSYSTEM_ERROR, MEM_SYS_TIMEOUT_XID_MESSAGE_FMT,
                 pContext->maintOpError);
}

static const EventContextOutputAdapters g_memSysTimeoutOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _memSysTimeoutEmitOsLogXid,
};

static const EventContextOutputAdapters g_memSysXidOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuXidEventCtxSerializeToEventBufferGoeCtx,
    .serializeToCperEventCtx = gpuXidEventCtxSerializeToCperCtx,
    .emitToOsLogXid = NULL,
};

NV_STATUS memsystoConstruct_IMPL
(
    MemSysTimeout *pEvent,
    EventBus *pEventBus,
    NvU32 severity,
    NvU32 maintOpError
)
{
    pEvent->context.header.pOutputAdapters = &g_memSysTimeoutOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.maintOpError);
    pEvent->context.maintOpError = maintOpError;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid175.header.pOutputAdapters = &g_memSysXidOutputAdapters;
    pEvent->xid175.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid175.data.message,
                      sizeof(pEvent->xid175.data.message),
                      MEM_SYS_TIMEOUT_XID_MESSAGE_FMT,
                      maintOpError) + 1);
    pEvent->xid175.data.xidCode = MEMORY_SUBSYSTEM_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid175.header);

    return NV_OK;
}

