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

//
// NV_OPERATIONAL_EVENT_BUFFER_BIND (0x90d0) resource + the op-event
// producer state that an EventBuffer carries to support it.
//

#include "diagnostics/op_event_buffer_bind.h"
#include "rmapi/event_buffer.h"
#include "rmapi/event_api.h"      // EVENTNOTIFICATION, PEVENTNOTIFICATION
#include "rmapi/client.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"
#include "class/cl90cd.h"
#include "class/cl90d0.h"
#include "nvos.h"                 // NV01_EVENT_KERNEL_CALLBACK_EX
#include "eventbufferproducer.h"  // eventBufferSetMaintainRecordCount
#include "utils/nvassert.h"
#include "nvmisc.h"
#include "nvport/nvport.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu_uuid.h"

#include "diagnostics/op_event_log.h"   // opEventLog singleton + opevtlog* registry API

/* -------------------------- Operational Event Buffer Producer State -------------------------- */

MAKE_INTRUSIVE_LIST(OpEventBufferBindList, OpEventBufferBind, bindListNode);

// Opaque in op_event_buffer_bind.h.
struct OpEventBufferProducerState
{
    EventBufferProducerState  base;            // MUST be first field
    EventBuffer              *pBuf;            // back-pointer (weak)
    NvU64                     bindIdMask[4];   // 256 bind IDs, per-buffer
    OpEventBufferBindList     bindList;        // binds targeting pBuf
    EVENTNOTIFICATION        *pListenerEntry;  // optional listener entry; NULL by default
    NvHandle                  hListenerClient;
    NvHandle                  hListenerEventBuffer;
};

static inline OpEventBufferProducerState *
_oebpsFromBase(EventBufferProducerState *pBase)
{
    return (OpEventBufferProducerState *)pBase;
}

static void _oebpsDestroyCb(EventBuffer *pBuf, EventBufferProducerState *pBase);

static NvBool
_opevtbufbindIsValidLogLevel(NvU8 logLevel)
{
    switch (logLevel)
    {
        case NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_ALL:
        case NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_TELEMETRY:
        case NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_DIAGNOSTIC:
        case NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_NOTICE:
        case NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_WARNING:
        case NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR:
            return NV_TRUE;
        default:
            return NV_FALSE;
    }
}

static NvBool
_opevtbufbindIsValidSeverity(NvU8 severity)
{
    switch (severity)
    {
        case NV_OPERATIONAL_EVENT_SEVERITY_ALL:
        case NV_OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL:
        case NV_OPERATIONAL_EVENT_SEVERITY_CORRECTED:
        case NV_OPERATIONAL_EVENT_SEVERITY_RECOVERABLE:
        case NV_OPERATIONAL_EVENT_SEVERITY_FATAL:
            return NV_TRUE;
        default:
            return NV_FALSE;
    }
}

static NvBool
_oebpsEmpty(OpEventBufferProducerState *pState)
{
    return (listCount(&pState->bindList) == 0) && (pState->pListenerEntry == NULL);
}

static NV_STATUS
_oebpsAllocBindId(OpEventBufferProducerState *pState, NvU8 *pBindId)
{
    NvU32 word;

    NV_ASSERT_OR_RETURN(pState  != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pBindId != NULL, NV_ERR_INVALID_POINTER);

    for (word = 0; word < NV_ARRAY_ELEMENTS(pState->bindIdMask); word++)
    {
        NvU64 m = pState->bindIdMask[word];
        if (m == NV_U64_MAX)
            continue;

        NvU32 bit = portUtilCountTrailingZeros64(~m);
        pState->bindIdMask[word] |= NVBIT64(bit);
        *pBindId = (NvU8)(word * 64U + bit);
        return NV_OK;
    }

    return NV_ERR_INSUFFICIENT_RESOURCES;
}

static void
_oebpsFreeBindId(OpEventBufferProducerState *pState, NvU8 bindId)
{
    NV_ASSERT_OR_RETURN_VOID(pState != NULL);

    NvU32 word = (NvU32)bindId / 64U;
    NvU32 bit  = (NvU32)bindId % 64U;

    NV_ASSERT_OR_RETURN_VOID(word < NV_ARRAY_ELEMENTS(pState->bindIdMask));
    pState->bindIdMask[word] &= ~NVBIT64(bit);
}

//
// Unlink and free the listener entry; revert bMaintainRecordCount (set
// by listener installers). No-op if no listener installed.
//
static void
_oebpsTearDownListener(OpEventBufferProducerState *pState)
{
    EventBuffer *pBuf = pState->pBuf;
    NV_STATUS    status;

    if (pState->pListenerEntry == NULL)
        return;

    status = unregisterEventNotificationWithData(&pBuf->pListeners,
                 pState->hListenerClient,
                 pState->hListenerEventBuffer,
                 pState->hListenerEventBuffer,
                 NV_TRUE,
                 pBuf->producerInfo.notificationHandle);
    NV_ASSERT_OK(status);

    pState->hListenerClient = 0;
    pState->hListenerEventBuffer = 0;

    pState->pListenerEntry = NULL;

    eventBufferSetMaintainRecordCount(&pBuf->producerInfo, NV_FALSE);
}

static void
_oebpsTearDownListenerIfNoBinds(OpEventBufferProducerState *pState)
{
    if (listCount(&pState->bindList) == 0)
        _oebpsTearDownListener(pState);
}

// Get the producer state on pBuf, lazily allocating on first call.
// Registers the embedded base with EventBuffer so destruct callbacks fire.
// Returns NULL on OOM.
static OpEventBufferProducerState *
_opEventBufferProducerStateGet(EventBuffer *pBuf)
{
    OpEventBufferProducerState *pState;

    NV_ASSERT_OR_RETURN(pBuf != NULL, NULL);

    if (pBuf->pProducerState != NULL)
    {
        // By invariant (single producer per buffer), this is ours.
        return _oebpsFromBase(pBuf->pProducerState);
    }

    pState = portMemAllocNonPaged(sizeof(*pState));
    if (pState == NULL)
        return NULL;

    portMemSet(pState, 0, sizeof(*pState));
    pState->pBuf = pBuf;
    listInitIntrusive(&pState->bindList);

    // Register the embedded base with the target buffer.
    pState->base.destroy = _oebpsDestroyCb;

    if (eventbufferRegisterProducerState(pBuf, &pState->base) != NV_OK)
    {
        portMemFree(pState);
        return NULL;
    }

    return pState;
}

// Tear down producer state: sever bind back-pointers, unlink any
// listener, clear pBuf->pProducerState, and free.
static void
_opEventBufferProducerStateDestroy(OpEventBufferProducerState *pState)
{
    OpEventBufferBindListIter it;

    NV_ASSERT_OR_RETURN_VOID(pState != NULL);

    //
    // Detach all bindpoints for this buffer from the OpEventLog before
    // we null bind back-pointers. After this call the
    // producer push iterates over the log's bind list without seeing
    // our bindpoints, so the bind resources can safely destruct later
    // without racing a delivery.
    //
    if (opEventLog != NULL && pState->pBuf != NULL)
        opevtlogDetachBindsForEventBuffer(opEventLog, pState->pBuf);

    //
    // Sever every bind's back-pointer to this buffer. Each bind's
    // destructor sees pTargetEventBuffer == NULL and skips its
    // bindId-free path on a dying buffer.
    //
    it = listIterAll(&pState->bindList);
    while (listIterNext(&it))
        it.pValue->pTargetEventBuffer = NULL;
    listDestroy(&pState->bindList);

    _oebpsTearDownListener(pState);

    if (pState->pBuf != NULL)
        pState->pBuf->pProducerState = NULL;

    portMemFree(pState);
}

// Trampoline from EventBuffer's destruct callback to the typed entry point.
static void
_oebpsDestroyCb(EventBuffer *pBuf, EventBufferProducerState *pBase)
{
    _opEventBufferProducerStateDestroy(_oebpsFromBase(pBase));
}

/* --------------- OpEventBufferBind (NV_OPERATIONAL_EVENT_BUFFER_BIND, 0x90d0) --------------- */

static NV_STATUS
_opEventBufferInstallPublicNotify
(
    OpEventBufferProducerState *pState,
    EventBuffer                *pEventBuffer,
    RsClient                   *pEventClient,
    NvHandle                    hEventBuffer
)
{
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pState != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pEventBuffer != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pEventClient != NULL, NV_ERR_INVALID_ARGUMENT);

    if (pEventBuffer->producerInfo.notificationHandle == NvP64_NULL)
        return NV_OK;

    if (pState->pListenerEntry != NULL)
        return NV_OK;

    status = registerEventNotification(&pEventBuffer->pListeners,
                 pEventClient,
                 hEventBuffer,
                 hEventBuffer,
                 NV01_EVENT_WITHOUT_EVENT_DATA,
                 NV_EVENT_BUFFER_BIND,
                 pEventBuffer->producerInfo.notificationHandle,
                 NV_FALSE);
    if (status != NV_OK)
        return status;

    pState->pListenerEntry = pEventBuffer->pListeners;
    pState->hListenerClient = pEventClient->hClient;
    pState->hListenerEventBuffer = hEventBuffer;

    // eventbufferAddNotify uses this for live ring threshold checks.
    eventBufferSetMaintainRecordCount(&pEventBuffer->producerInfo, NV_TRUE);

    return NV_OK;
}

NV_STATUS
opevtbufbindConstruct_IMPL
(
    OpEventBufferBind *pResource,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_OPERATIONAL_EVENT_BUFFER_BIND_ALLOC_PARAMETERS *pAllocParams =
        (NV_OPERATIONAL_EVENT_BUFFER_BIND_ALLOC_PARAMETERS *)pParams->pAllocParams;
    RsClient                   *pClient         = pCallContext->pClient;
    RsResourceRef              *pEventBufferRef = NULL;
    EventBuffer                *pEventBuffer    = NULL;
    OpEventBufferProducerState *pState          = NULL;
    NV_STATUS                   status;
    NvU8                        assignedBindId  = 0;
    NvU32                       i;

    NV_ASSERT_OR_RETURN(pAllocParams != NULL, NV_ERR_INVALID_ARGUMENT);

    //
    // Reject capability descriptors — CAP_SYS_ADMIN gate this CL, no
    // descriptor validation yet.
    //
    for (i = 0; i < NV_OPERATIONAL_EVENT_BUFFER_CAP_SLOTS; i++)
    {
        if (pAllocParams->capDescriptors[i] != 0)
            return NV_ERR_NOT_SUPPORTED;
    }

    // Validate record format. INVALID and out-of-range values rejected.
    if (pAllocParams->recordFormat != NV_OPERATIONAL_EVENT_BUFFER_RECORD_FORMAT_GOE &&
        pAllocParams->recordFormat != NV_OPERATIONAL_EVENT_BUFFER_RECORD_FORMAT_CPER)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Validate scope. DEVICE supported; MIG scopes reserved.
    if (pAllocParams->scope != NV_OPERATIONAL_EVENT_BUFFER_SCOPE_DEVICE)
        return NV_ERR_NOT_SUPPORTED;

    if (!_opevtbufbindIsValidLogLevel(pAllocParams->minLogLevel) ||
        !_opevtbufbindIsValidSeverity(pAllocParams->minSeverity))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Replay is not implemented yet; only AFTER_BIND is supported.
    if (pAllocParams->startingCursor !=
        NV_OPERATIONAL_EVENT_BUFFER_STARTING_CURSOR_AFTER_BIND)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // Weak target pointer: EventBuffer destroy severs this bind's back-pointer.
    status = clientGetResourceRef(pClient, pAllocParams->hEventBuffer,
                                  &pEventBufferRef);
    if (status != NV_OK)
        return status;

    pEventBuffer = dynamicCast(pEventBufferRef->pResource, EventBuffer);
    if (pEventBuffer == NULL)
        return NV_ERR_INVALID_OBJECT;

    //
    // Op-event delivery must not overwrite unconsumed records, so the
    // target buffer must be KEEP_OLDEST. Switching the buffer to
    // KEEP_NEWEST while binds exist is blocked by opevtlogTrySetKeepNewest
    // in event_buffer.c.
    //
    if (pEventBuffer->producerInfo.isKeepNewest)
        return NV_ERR_NOT_SUPPORTED;

    // The buffer must fit the largest op-event record type.
    if (pEventBuffer->producerInfo.recordBuffer.recordSize <
        NV_MAX(sizeof(NV_GPU_OPERATIONAL_EVENT_RECORD),
               sizeof(NV_OPERATIONAL_EVENT_CPER_RECORD)))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // CPER records always need vardata; GOE records do not.
    if (pAllocParams->recordFormat == NV_OPERATIONAL_EVENT_BUFFER_RECORD_FORMAT_CPER &&
        pEventBuffer->producerInfo.vardataBuffer.bufferSize == 0)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // Operational-event buffers represent a system-level log. Bind targets are
    // selected by resourceUuid, not by EventBuffer subdevice ownership.
    //
    // The GSP import buffer still carries hSubDevice because NV_EVENT_BUFFER
    // requires it for non-deviceless external sysmem backing.
    //
    if (pEventBuffer->hSubDevice != 0)
        return NV_ERR_INVALID_ARGUMENT;

    // Validate that resourceUuid names an existing DEVICE-scope GPU.
    {
        OBJGPU    *pGpu;

        ct_assert(NV_UUID_LEN == sizeof(pAllocParams->resourceUuid));
        pGpu = gpumgrGetGpuFromUuid(pAllocParams->resourceUuid,
                                    DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1) |
                                    DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _BINARY));
        if (pGpu == NULL)
            return NV_ERR_GPU_UUID_NOT_FOUND;
    }

    pState = _opEventBufferProducerStateGet(pEventBuffer);
    if (pState == NULL)
        return NV_ERR_NO_MEMORY;

    status = _oebpsAllocBindId(pState, &assignedBindId);
    if (status != NV_OK)
    {
        // We may have just created the state; tear it down if nothing
        // else is keeping it alive.
        if (_oebpsEmpty(pState))
            _opEventBufferProducerStateDestroy(pState);
        return status;
    }

    pResource->pTargetEventBuffer = pEventBuffer;
    pResource->recordFormat       = pAllocParams->recordFormat;
    pResource->bindId             = assignedBindId;
    pResource->scope              = pAllocParams->scope;
    pResource->minLogLevel        = pAllocParams->minLogLevel;
    pResource->minSeverity        = pAllocParams->minSeverity;
    portMemCopy(pResource->resourceUuid, sizeof(pResource->resourceUuid),
                pAllocParams->resourceUuid, sizeof(pResource->resourceUuid));

    // Register the bind so EventBuffer destroy can sever the weak target.
    listAppendExisting(&pState->bindList, pResource);

    status = _opEventBufferInstallPublicNotify(pState, pEventBuffer,
                                               pClient,
                                               pAllocParams->hEventBuffer);
    if (status != NV_OK)
    {
        listRemove(&pState->bindList, pResource);
        _oebpsFreeBindId(pState, assignedBindId);
        if (_oebpsEmpty(pState))
            _opEventBufferProducerStateDestroy(pState);
        return status;
    }

    //
    // Kernel-RM: register the bindpoint with the host OpEventLog so
    // producer pushes deliver to this bind. opevtlogRegisterBind
    // serializes against opevtlogTrySetKeepNewest under pPushMutex, so
    // a concurrent KEEP_NEWEST transition can't race a bind register.
    // opEventLog can be NULL during early init / late teardown -- in
    // either window there are no producers pushing, so the bind exists
    // at the substrate level but isn't reachable for delivery until the
    // log comes up (matches the pre-existing single-log behavior).
    //
    if (opEventLog != NULL)
    {
        status = opevtlogRegisterBind(opEventLog, pResource);
        if (status != NV_OK)
        {
            listRemove(&pState->bindList, pResource);
            _oebpsTearDownListenerIfNoBinds(pState);
            _oebpsFreeBindId(pState, assignedBindId);
            if (_oebpsEmpty(pState))
                _opEventBufferProducerStateDestroy(pState);
            return status;
        }
    }

    pAllocParams->bindId = assignedBindId;
    return NV_OK;
}

void
opevtbufbindDestruct_IMPL
(
    OpEventBufferBind *pResource
)
{
    //
    // pTargetEventBuffer == NULL: EventBuffer freed first; its destroy
    // callback already severed our back-pointer. Nothing to do.
    //
    if (pResource->pTargetEventBuffer != NULL)
    {
        EventBuffer                *pBuffer = pResource->pTargetEventBuffer;
        OpEventBufferProducerState *pState  = _oebpsFromBase(pBuffer->pProducerState);

        NV_ASSERT_OR_RETURN_VOID(pState != NULL);

        // Stop the producer push from delivering to this bind.
        if (opEventLog != NULL)
            opevtlogUnregisterBind(opEventLog, pResource);

        listRemove(&pState->bindList, pResource);
        _oebpsFreeBindId(pState, pResource->bindId);

        _oebpsTearDownListenerIfNoBinds(pState);

        // Tear down only when nothing else keeps the state alive.
        if (_oebpsEmpty(pState))
            _opEventBufferProducerStateDestroy(pState);
    }
}
