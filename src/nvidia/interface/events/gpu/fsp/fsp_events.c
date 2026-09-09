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

#include "events/gpu/fsp/fsp_events.h"

#include "cper/gpu_cper.h"
#include "diagnostics/op_event.h"
#include "gpu/gpu.h"
#include "gpu/fsp/kern_fsp.h"
#include "nvmisc.h"
#include "nvport/nvport.h"
#include "nverror.h"

#define FSP_BOOT_TIMEOUT_XID_MESSAGE_FMT \
    "Error status 0x%x while polling for FSP boot complete, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x"
#define FSP_FUSE_ERROR_XID_MESSAGE_FMT \
    "FSP fuse error check has failed. Status = 0x%x."

static NV_STATUS _fspBootTimeoutSerializeToEventBufferOpEventCtx
(
    EventContextHeader *pContextHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    NvU32 *pBytesWritten
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
            pContextHeader,
            pOutput,
            outputSize,
            pBytesWritten,
            FSP_EVENT_CTX_TYPE_BOOT_TIMEOUT_DATA,
            FSP_BOOT_TIMEOUT_DATA_VERSION);
}

static NV_STATUS _fspFuseErrorSerializeToEventBufferOpEventCtx
(
    EventContextHeader *pContextHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    NvU32 *pBytesWritten
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
            pContextHeader,
            pOutput,
            outputSize,
            pBytesWritten,
            FSP_EVENT_CTX_TYPE_FUSE_ERROR_DATA,
            FSP_FUSE_ERROR_DATA_VERSION);
}

static void _fspBootTimeoutEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu
)
{
    FspBootTimeoutCtx *pContext = (FspBootTimeoutCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, GPU_INIT_ERROR, FSP_BOOT_TIMEOUT_XID_MESSAGE_FMT,
                 pContext->data.errorStatus,
                 pContext->data.fspBootComplete,
                 pContext->data.scratch0,
                 pContext->data.scratch1,
                 pContext->data.scratch2,
                 pContext->data.scratch3);
}

static void _fspFuseErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu
)
{
    FspFuseErrorCtx *pContext = (FspFuseErrorCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, GPU_INIT_ERROR, FSP_FUSE_ERROR_XID_MESSAGE_FMT,
                 pContext->data.fuseStatus);
}

static const EventContextOutputAdapters g_fspBootTimeoutOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _fspBootTimeoutSerializeToEventBufferOpEventCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _fspBootTimeoutEmitOsLogXid,
};

static const EventContextOutputAdapters g_fspFuseErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _fspFuseErrorSerializeToEventBufferOpEventCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _fspFuseErrorEmitOsLogXid,
};

//
// The FSP event contexts emit Xid 143 to OS logs, because they have the data context
// required to do so directly, so emitToOsLogXid is not implemented for GpuXidEventCtx.
// The serializers for this context exist to produce the legacy Xid compatibility contexts in
// CPER and EventBuffer output using a pre-rendered string built at event construction time.
//
static const EventContextOutputAdapters g_xid143OutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuXidEventCtxSerializeToEventBufferGoeCtx,
    .serializeToCperEventCtx = gpuXidEventCtxSerializeToCperCtx,
    .emitToOsLogXid = NULL,
};

NV_STATUS fspboottoConstruct_IMPL
(
    FspBootTimeout *pEvent,
    EventBus *pEventBus,
    NvU64 timeoutNs,
    NvU64 elapsedNs,
    NV_STATUS errorStatus,
    NvU32 fspBootComplete,
    NvU32 scratch0,
    NvU32 scratch1,
    NvU32 scratch2,
    NvU32 scratch3
)
{
    // Populate and add context
    pEvent->context.header.pOutputAdapters = &g_fspBootTimeoutOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.data);
    pEvent->context.data.errorStatus = errorStatus;
    pEvent->context.data.fspBootComplete = fspBootComplete;
    pEvent->context.data.scratch0 = scratch0;
    pEvent->context.data.scratch1 = scratch1;
    pEvent->context.data.scratch2 = scratch2;
    pEvent->context.data.scratch3 = scratch3;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Populate and add xid143
    pEvent->xid143.header.pOutputAdapters = &g_xid143OutputAdapters;
    pEvent->xid143.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid143.data.message,
                      sizeof(pEvent->xid143.data.message),
                      FSP_BOOT_TIMEOUT_XID_MESSAGE_FMT,
                      pEvent->context.data.errorStatus,
                      pEvent->context.data.fspBootComplete,
                      pEvent->context.data.scratch0,
                      pEvent->context.data.scratch1,
                      pEvent->context.data.scratch2,
                      pEvent->context.data.scratch3) + 1);
    pEvent->xid143.data.xidCode = GPU_INIT_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid143.header);

    return NV_OK;
}

NV_STATUS fspfuseerrConstruct_IMPL
(
    FspFuseError *pEvent,
    EventBus *pEventBus,
    NvU32 fuseStatus
)
{
    // Populate and add context
    pEvent->context.header.pOutputAdapters = &g_fspFuseErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.data);
    pEvent->context.data.fuseStatus = fuseStatus;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Populate and add xid143
    pEvent->xid143.header.pOutputAdapters = &g_xid143OutputAdapters;
    pEvent->xid143.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid143.data.message,
                      sizeof(pEvent->xid143.data.message),
                      FSP_FUSE_ERROR_XID_MESSAGE_FMT,
                      pEvent->context.data.fuseStatus) + 1);
    pEvent->xid143.data.xidCode = GPU_INIT_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid143.header);

    return NV_OK;
}

