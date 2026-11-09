/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file
 * @brief This module contains the gpu control interfaces for the
 *        subdevice (NV20_SUBDEVICE_0) class. Subdevice-level control calls
 *        are directed unicast to the associated GPU.
 */

#include "core/core.h"
#include "core/locks.h"
#include "diagnostics/journal.h"
#include "diagnostics/tracer.h"
#include "gpu/gpu.h"
#include "gpu/subdevice/subdevice.h"
#include "rmapi/client.h"
#include "rmapi/rs_utils.h"
#include "mem_mgr/mem.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "gpu/gsp/gsp_trace_rats_macro.h"
#include "ctrl/ctrl2080/ctrl2080event.h"
#include "rmapi/event_buffer.h"
#include "diagnostics/nocat_event_list.h"
#include "diagnostics/op_event_log.h"
#include "gpu/gpu_op_event.h"
//
// EVENT RM SubDevice Controls
//
NV_STATUS
subdeviceCtrlCmdEventSetTrigger_IMPL
(
    Subdevice *pSubdevice
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_SW, NULL, 0, 0, 0);

    return NV_OK;
}

//
// subdeviceCtrlCmdEventSetTriggerFifo
//
// Used to signal Vulkan timeline semaphores from the CPU.
//
NV_STATUS
subdeviceCtrlCmdEventSetTriggerFifo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS *pTriggerFifoParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    engineNonStallIntrNotifyEvent(pGpu, RM_ENGINE_TYPE_HOST,
                                  pTriggerFifoParams->hEvent);

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdEventSetNotification_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams
)
{
    OBJGPU  *pGpu    = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS status = NV_OK;

    // NV01_EVENT must have been plugged into this subdevice
    if (inotifyGetNotificationList(staticCast(pSubdevice, INotifier)) == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "cmd 0x%x: no event list\n", NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION);
        return NV_ERR_INVALID_STATE;
    }

    if (pSetEventParams->event >= NV2080_NOTIFIERS_MAXCOUNT)
    {
        NV_PRINTF(LEVEL_INFO, "bad event 0x%x\n", pSetEventParams->event);
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pSetEventParams->event == NV2080_NOTIFIERS_TIMER)
    {
        NV_PRINTF(LEVEL_INFO, "wrong control call for timer event\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (IS_FW_CLIENT(pGpu))
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
            pRmApi->Control(pRmApi, RES_GET_CLIENT_HANDLE(pSubdevice),
                            RES_GET_HANDLE(pSubdevice),
                            NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION,
                            pSetEventParams,
                            sizeof *pSetEventParams));
    }

    switch (pSetEventParams->action)
    {
        case NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE:
        case NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT:
        {
            // must be in disabled state to transition to an active state
            if (pSubdevice->notifyActions[pSetEventParams->event] != NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE)
            {
                status = NV_ERR_INVALID_STATE;
                break;
            }

            pSubdevice->notifyActions[pSetEventParams->event] = pSetEventParams->action;
            break;
        }

        case NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE:
        {
            pSubdevice->notifyActions[pSetEventParams->event] = pSetEventParams->action;
            break;
        }
        default:
        {
            status = NV_ERR_INVALID_ARGUMENT;
            break;
        }
    }

    return status;
}

NV_STATUS
subdeviceCtrlCmdEventSetMemoryNotifies_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *pSetMemoryNotifiesParams
)
{
    Memory   *pMemory;
    RsClient *pClient = RES_GET_CLIENT(pSubdevice);
    NvU32     i;

    // ensure there's no pending notifications if there is an existing notification buffer
    if (pSubdevice->hNotifierMemory != NV01_NULL_OBJECT)
    {
        for (i = 0; i < NV2080_NOTIFIERS_MAXCOUNT; i++)
        {
            if (pSubdevice->notifyActions[i] != NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE)
            {
                return NV_ERR_STATE_IN_USE;
            }
        }
    }

    if (pSetMemoryNotifiesParams->hMemory == NV01_NULL_OBJECT)
    {
        pSubdevice->hNotifierMemory = pSetMemoryNotifiesParams->hMemory;
        pSubdevice->pNotifierMemory = NULL;
        return NV_OK;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT,
        memGetByHandle(pClient, pSetMemoryNotifiesParams->hMemory, &pMemory));

    if (pMemory->pDevice != pSubdevice->pDevice)
    {
        NV_PRINTF(LEVEL_INFO, "subdeviceCtrlCmdEventSetMemoryNotifies: rejecting memory not associated with this device\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pMemory->pMemDesc->Size < sizeof(NvNotification) * NV2080_NOTIFIERS_MAXCOUNT)
    {
        return NV_ERR_INVALID_LIMIT;
    }

    pSubdevice->hNotifierMemory = pSetMemoryNotifiesParams->hMemory;
    pSubdevice->pNotifierMemory = pMemory;

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdEventSetSemaphoreMemory_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS *pSetSemMemoryParams
)
{
    RsClient *pClient = RES_GET_CLIENT(pSubdevice);
    Memory   *pMemory;
    NvU32     i;

    if (pSubdevice->hSemMemory != NV01_NULL_OBJECT)
    {
        for (i = 0; i < NV2080_NOTIFIERS_MAXCOUNT; i++)
        {
            if (pSubdevice->notifyActions[i] != NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE)
            {
                return NV_ERR_STATE_IN_USE;
            }
        }
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT,
        memGetByHandle(pClient, pSetSemMemoryParams->hSemMemory, &pMemory));

    if (pSetSemMemoryParams->semOffset >= pMemory->pMemDesc->Size)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pSubdevice->hSemMemory = pSetSemMemoryParams->hSemMemory;
    pMemory->vgpuNsIntr.nsSemOffset = pSetSemMemoryParams->semOffset;

    pMemory->vgpuNsIntr.nsSemValue = 0;
    pMemory->vgpuNsIntr.guestMSIAddr = 0;
    pMemory->vgpuNsIntr.guestMSIData = 0;
    pMemory->vgpuNsIntr.guestDomainId = 0;
    pMemory->vgpuNsIntr.isSemaMemValidationEnabled = NV_TRUE;

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdEventSetSemaMemValidation_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS *pSetSemaMemValidationParams
)
{
    Memory   *pMemory;
    RsClient *pClient = RES_GET_CLIENT(pSubdevice);
    NvU32    *pSemValue;
    NV_STATUS rmStatus = NV_ERR_INVALID_OBJECT_HANDLE;

    rmStatus = memGetByHandle(pClient, pSetSemaMemValidationParams->hSemMemory, &pMemory);

    if (rmStatus == NV_OK)
    {
        pSemValue = (NvU32 *)NvP64_VALUE(memdescGetKernelMapping(pMemory->pMemDesc));

        if (pSemValue == NULL)
        {
            return NV_ERR_INVALID_ADDRESS;
        }

        portMemSet(pSemValue, 0, RM_PAGE_SIZE);
        pMemory->vgpuNsIntr.nsSemValue = 0;
        pMemory->vgpuNsIntr.isSemaMemValidationEnabled = pSetSemaMemValidationParams->isSemaMemValidationEnabled;
    }

    return rmStatus;
}

NV_STATUS
subdeviceCtrlCmdEventVideoBindEvtbuf_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_PARAMS *pParams
)
{
    NV_STATUS status;
    RsClient *pClient = RES_GET_CLIENT(pSubdevice);
    RsResourceRef *pEventBufferRef = NULL;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
    NvHandle hNotifier = RES_GET_HANDLE(pSubdevice);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    NV_ASSERT_OK_OR_RETURN(serverutilGetResourceRefWithType(hClient,
                                                            pParams->hEventBuffer,
                                                            classId(EventBuffer),
                                                            &pEventBufferRef));

    status = videoAddBindpoint(pGpu,
                               pClient,
                               pEventBufferRef,
                               hNotifier,
                               pParams->bAllUsers,
                               pParams->levelOfDetail,
                               pParams->eventFilter);
    return status;
}


NV_STATUS
subdeviceCtrlCmdEventNocatBindEvtbuf_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_EVENT_NOCAT_BIND_EVTBUF_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    RsClient *pClient = RES_GET_CLIENT(pSubdevice);
    RsResourceRef *pEventBufferRef = NULL;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
    NvHandle hNotifier = RES_GET_HANDLE(pSubdevice);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    NV_ASSERT_OK_OR_RETURN(serverutilGetResourceRefWithType(hClient,
                                                            pParams->hEventBuffer,
                                                            classId(EventBuffer),
                                                            &pEventBufferRef));

    status = nocatAddBindpoint(pGpu,
                               pClient,
                               pEventBufferRef,
                               hNotifier);

    return status;
}

//
// The inject buffer holds one fixed NV_GPU_OPERATIONAL_EVENT_RECORD followed by
// its serialized contexts. Injectable single-event records are small (largest
// today is a few hundred bytes), so MAX_RECORD_SIZE is capped to keep this
// test-only control's by-value params compact. Guard against the cap being
// trimmed below even the fixed record prefix; larger records are already
// rejected at runtime (recordSize > sizeof(recordData) and the size checks in
// gpuOpEventInjectSimulated()).
//
ct_assert(sizeof(NV_GPU_OPERATIONAL_EVENT_RECORD) <=
          NV2080_CTRL_EVENT_OPERATIONAL_INJECT_MAX_RECORD_SIZE);

//
// Test-only: emit a simulated GPU operational event onto this GPU's event bus.
// RM_TEST_ONLY_CODE gating is enforced centrally by the RMAPI control path;
// no manual property check here. Returns NV_ERR_NOT_SUPPORTED when the
// GPU_OPERATIONAL_EVENTS feature is compiled out.
//
NV_STATUS
subdeviceCtrlCmdEventOperationalInject_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_EVENT_OPERATIONAL_INJECT_PARAMS *pParams
)
{
    // gpuOpEventInjectSimulated() is CPU-RM only (GSP_CLIENT_RM); match its
    // gating so firmware builds of this control resolve without it.
#if !defined(SRT_BUILD) && RMCFG_FEATURE_GPU_OPERATIONAL_EVENTS && \
    RMCFG_FEATURE_GSP_CLIENT_RM
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(),
                        NV_ERR_INVALID_LOCK_STATE);

    // Match CLEAR_SIMULATED: entries stamped before UUID initialization cannot
    // be matched and removed by the per-GPU cleanup control.
    if (!pGpu->gpuUuid.isInitialized)
        return NV_ERR_INVALID_STATE;

    if (pParams->recordSize > sizeof(pParams->recordData))
        return NV_ERR_INVALID_ARGUMENT;

    return gpuOpEventInjectSimulated(pGpu, pParams->recordData,
                                     pParams->recordSize);
#else
    return NV_ERR_NOT_SUPPORTED;
#endif // !SRT_BUILD && GPU_OPERATIONAL_EVENTS && GSP_CLIENT_RM
}

//
// Test-only: remove retained simulated GPU operational events for this GPU.
// RM_TEST_ONLY_CODE gating is enforced centrally by the RMAPI control path.
//
NV_STATUS
subdeviceCtrlCmdEventOperationalClearSimulated_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_EVENT_OPERATIONAL_CLEAR_SIMULATED_PARAMS *pParams
)
{
    NV_ASSERT_OR_RETURN(pParams != NULL, NV_ERR_INVALID_POINTER);

    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (opEventLog == NULL)
        return NV_ERR_INVALID_STATE;
    if (!pGpu->gpuUuid.isInitialized)
        return NV_ERR_INVALID_STATE;

    return opevtlogClearSimulated(opEventLog, pGpu->gpuUuid.uuid,
                                  &pParams->removedEntryCount);
}

NV_STATUS
subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_EVENT_RATS_GSP_TRACE_BIND_EVTBUF_PARAMS *pParams
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;
#if KERNEL_GSP_TRACING_RATS_ENABLED
    RsClient *pClient = RES_GET_CLIENT(pSubdevice);
    RsResourceRef *pEventBufferRef = NULL;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
    NvHandle hNotifier = RES_GET_HANDLE(pSubdevice);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    NV_ASSERT_OK_OR_RETURN(serverutilGetResourceRefWithType(hClient,
                                                            pParams->hEventBuffer,
                                                            classId(EventBuffer),
                                                            &pEventBufferRef));
    status = gspRatsAddBindpoint(pGpu,
                               pClient,
                               pEventBufferRef,
                               hNotifier,
                               pParams->tracepointMask,
                               pParams->gspLoggingBufferSize,
                               pParams->gspLoggingBufferWatermark,
                               pParams->targetTask);
#endif
    return status;
}
