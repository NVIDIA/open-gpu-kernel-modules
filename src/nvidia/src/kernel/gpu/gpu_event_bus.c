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

#include "rmconfig.h"

#include "core/locks.h"
#include "diagnostics/op_event_log.h"
#include "gpu/gpu.h"
#include "gpu/gpu_op_event.h"
#include "gpu_mgr/gpu_mgr.h"
#include "nvoc/prelude.h"
#include "nvoc/event.h"
#include "nvoc/event_bus.h"
#include "nvoc/event_group.h"
#include "nvport/nvport.h"
#include "os/os.h"
#include "utils/nvassert.h"
#include "utils/nvprintf.h"

static NvBool
_gpuEventBusGpuLockIsOwner
(
    OBJGPU *pGpu
)
{
    GPU_MASK gpuMask = 0;

    if (pGpu == NULL)
        return NV_FALSE;

    return rmGpuGroupLockIsOwner(pGpu->gpuInstance,
                                 GPU_LOCK_GRP_SUBDEVICE,
                                 &gpuMask);
}

static NvBool
_gpuEventBusIsValidTracePrefix
(
    GPU_OPERATIONAL_EVENT_ORIGINATOR originator
)
{
    switch (originator)
    {
        case GPU_OPERATIONAL_EVENT_ORIGINATOR_PF_GSP_FW:
        case GPU_OPERATIONAL_EVENT_ORIGINATOR_VF_GSP_FW:
        case GPU_OPERATIONAL_EVENT_ORIGINATOR_PF_DRIVER:
        case GPU_OPERATIONAL_EVENT_ORIGINATOR_VF_DRIVER:
            return NV_TRUE;
        case GPU_OPERATIONAL_EVENT_ORIGINATOR_NONE:
        default:
            return NV_FALSE;
    }
}

static void
_gpuEventBusInitEventGroup
(
    EventBus   *pEventBus,
    EventGroup *pEventGroup,
    Event      *pEvent
)
{
    GpuOperationalEvent *pGpuEvent;
    NvU8 traceIdPrefix = 0;

    NV_ASSERT_OR_RETURN_VOID(pEventGroup != NULL);
    NV_ASSERT_OR_RETURN_VOID(pEvent != NULL);

    //
    // For GpuOperationalEvents, the originator prefix is needed to identify the origin of the
    // trace in GOE output. Other event types wouldn't need this yet, and can simply use the local
    // sequence ID as the trace ID.
    //
    pGpuEvent = dynamicCast(pEvent, GpuOperationalEvent);
    if (pGpuEvent != NULL)
    {
        NV_ASSERT_OR_RETURN_VOID(_gpuEventBusIsValidTracePrefix(pGpuEvent->originator));
        traceIdPrefix = pGpuEvent->originator;
    }

    eventgroupInitTraceId(pEventGroup, traceIdPrefix);
}

static void
_gpuEventBusExportEventGroup
(
    EventBus   *pEventBus,
    EventGroup *pEventGroup
)
{
    OBJGPU *pGpu = (OBJGPU *)eventbusGetContext(pEventBus);
    NV_STATUS status;

    NV_ASSERT_OR_RETURN_VOID(pEventGroup != NULL);
    NV_ASSERT_OR_RETURN_VOID(_gpuEventBusGpuLockIsOwner(pGpu));

    if (opEventLog == NULL)
        return;

    status = opevtlogAppendEventGroup(opEventLog, pEventGroup);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "op-event log append failed on GPU %u: 0x%x\n",
                  pGpu->gpuInstance, status);
    }
}

static void
_gpuEventBusFlushWorkItem
(
    NvU32  gpuInstance,
    void  *pArg
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);

    PORT_UNREFERENCED_VARIABLE(pArg);

    NV_ASSERT_OR_RETURN_VOID(pGpu != NULL);
    NV_ASSERT_OR_RETURN_VOID(_gpuEventBusGpuLockIsOwner(pGpu));

    if (pGpu->pEventBus != NULL)
        eventbusFlush(pGpu->pEventBus);

    //
    // Keep the pending bit set for the whole flush. If handlers publish more
    // runnable GPU events during dispatch, eventbusFlush() will drain them
    // before returning. The GPU lock asserted above prevents a publisher on
    // another thread from appending work between the final drain pass and this
    // clear.
    //
    pGpu->bEventBusFlushWorkItemPending = NV_FALSE;
}

static void
_gpuEventBusReportRunnableEvent
(
    EventBus *pEventBus,
    Event    *pEvent
)
{
    OBJGPU *pGpu = (OBJGPU *)eventbusGetContext(pEventBus);
    NV_STATUS status;

    PORT_UNREFERENCED_VARIABLE(pEvent);

    if (pGpu == NULL)
        return;

    NV_ASSERT_OR_RETURN_VOID(_gpuEventBusGpuLockIsOwner(pGpu));

    if (pGpu->bEventBusFlushWorkItemPending)
        return;

    pGpu->bEventBusFlushWorkItemPending = NV_TRUE;

    status = osQueueWorkItem(pGpu,
                             _gpuEventBusFlushWorkItem,
                             NULL,
                             (OsQueueWorkItemFlags){
                                 .bLockGpuGroupSubdevice = NV_TRUE});
    if (status != NV_OK)
    {
        pGpu->bEventBusFlushWorkItemPending = NV_FALSE;
        NV_PRINTF(LEVEL_WARNING,
                  "GPU event bus flush work item queue failed on GPU %u: 0x%x\n",
                  pGpu->gpuInstance,
                  status);
    }
}

static void
_gpuEventBusReportDroppedEvent
(
    EventBus  *pEventBus,
    Event     *pEvent,
    NV_STATUS  status
)
{
    OBJGPU *pGpu = (OBJGPU *)eventbusGetContext(pEventBus);

    PORT_UNREFERENCED_VARIABLE(pEvent);

    NV_PRINTF(LEVEL_WARNING,
              "GPU event dropped on GPU %u: status 0x%x\n",
              (pGpu != NULL) ? pGpu->gpuInstance : NV_U32_MAX,
              status);
}

NV_STATUS
gpuEventBusConstruct_IMPL
(
    OBJGPU *pGpu
)
{
    EVENT_BUS_CALLBACKS callbacks;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_POINTER);

    if (pGpu->pEventBus != NULL)
    {
        objEventBus(pGpu) = pGpu->pEventBus;
        return NV_OK;
    }

    portMemSet(&callbacks, 0, sizeof(callbacks));
    callbacks.exportEventGroup = _gpuEventBusExportEventGroup;
    callbacks.initEventGroup = _gpuEventBusInitEventGroup;
    callbacks.reportRunnableEvent = _gpuEventBusReportRunnableEvent;
    callbacks.reportDroppedEvent = _gpuEventBusReportDroppedEvent;

    status = objCreate(&pGpu->pEventBus,
                       pGpu,
                       EventBus,
                       (EVENT_BUS_CONTEXT *)pGpu,
                       &callbacks);
    NV_ASSERT_OK_OR_RETURN(status);

    objEventBus(pGpu) = pGpu->pEventBus;

    return NV_OK;
}

void
gpuEventBusEnterBlockingMode_IMPL
(
    OBJGPU *pGpu
)
{
    NV_ASSERT_OR_RETURN_VOID(pGpu != NULL);

    if (pGpu->pEventBus == NULL)
        return;

    NV_ASSERT_OR_RETURN_VOID(_gpuEventBusGpuLockIsOwner(pGpu));

    eventbusEnterBlockingMode(pGpu->pEventBus);
    eventbusFlush(pGpu->pEventBus);
}

void
gpuEventBusDestruct_IMPL
(
    OBJGPU *pGpu
)
{
    NV_ASSERT_OR_RETURN_VOID(pGpu != NULL);

    if (pGpu->pEventBus == NULL)
        return;

    objDelete(pGpu->pEventBus);
    pGpu->pEventBus = NULL;
    objEventBus(pGpu) = NULL;
}

