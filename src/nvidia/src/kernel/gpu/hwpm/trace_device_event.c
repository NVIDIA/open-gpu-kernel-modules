/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
 *
 *   File: trace_device_event.c
 *
 *   Description:
 *       Upon successful allocation of this class, a client is granted
 *       permission to collect device trace event counters.
 *
 *****************************************************************************/

#define NVOC_TRACE_DEVICE_EVENT_H_PRIVATE_ACCESS_ALLOWED



#include "core/core.h"
#include "core/system.h"
#include "gpu/gpu.h"
#include "class/clcdcd.h"
#include "kernel/gpu/hwpm/trace_device_event.h"
#include "os/os.h"
#include "rmapi/client.h"

NV_STATUS
traceDeviceEventConstruct_IMPL
(
    TraceDeviceEvent *traceDeviceEvent,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NVCDCD_ALLOC_PARAMETERS *pUserParams = pParams->pAllocParams;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    RmClient *pRmClient = dynamicCast(pCallContext->pClient, RmClient);
    NV_STATUS status;

    osRmCapInitDescriptor(&traceDeviceEvent->dupedCapDescriptor);

    status = osRmCapAcquire(pSys->pOsRmCaps,
                            NV_RM_CAP_SYS_TRACE_DEVICE,
                            pUserParams->capDescriptor,
                            &traceDeviceEvent->dupedCapDescriptor);

    //
    // On platforms where capability isn't implemented,
    // enforce the admin-only check.
    //
    if (status == NV_ERR_NOT_SUPPORTED)
    {
        if (!rmclientIsAdmin(pRmClient, pCallContext->secInfo.privLevel))
        {
            NV_PRINTF(LEVEL_ERROR, "insufficient permissions\n");
            return NV_ERR_INSUFFICIENT_PERMISSIONS;
        }
    }

    return status;
}

void
traceDeviceEventDestruct_IMPL
(
    TraceDeviceEvent *traceDeviceEvent
)
{
    osRmCapRelease(traceDeviceEvent->dupedCapDescriptor);
}


NvBool
traceDeviceEventCanCopy_IMPL
(
    TraceDeviceEvent *traceDeviceEvent
)
{
    //
    // Returning NV_FALSE here prevents duping of this object,
    // since nv-caps doesn't support duping yet, please revisit duping if needed in the future.
    //
    return NV_FALSE;
}

