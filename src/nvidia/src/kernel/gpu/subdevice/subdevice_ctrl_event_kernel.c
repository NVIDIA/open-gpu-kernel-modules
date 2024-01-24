/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

    if (IS_GSP_CLIENT(pGpu))
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

            if (pSetEventParams->event == NV2080_NOTIFIERS_FIFO_EVENT_MTHD)
            {
                pGpu->activeFifoEventMthdNotifiers++;
            }

            pSubdevice->notifyActions[pSetEventParams->event] = pSetEventParams->action;
            break;
        }

        case NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE:
        {
            if ((pSetEventParams->event == NV2080_NOTIFIERS_FIFO_EVENT_MTHD) &&
                (pSubdevice->notifyActions[pSetEventParams->event] != NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE))
            {
                NV_ASSERT(pGpu->activeFifoEventMthdNotifiers);
                pGpu->activeFifoEventMthdNotifiers--;
            }

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
    pMemory->vgpuNsIntr.pVgpuVfioRef = NULL;
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

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance));

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

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    NV_ASSERT_OK_OR_RETURN(serverutilGetResourceRefWithType(hClient,
                                                            pParams->hEventBuffer,
                                                            classId(EventBuffer),
                                                            &pEventBufferRef));
    status = gspTraceAddBindpoint(pGpu,
                               pClient,
                               pEventBufferRef,
                               hNotifier,
                               pParams->tracepointMask,
                               pParams->gspLoggingBufferSize,
                               pParams->gspLoggingBufferWatermark);
#endif
    return status;
}
