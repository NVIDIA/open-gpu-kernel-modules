/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "gpu/disp/kern_disp.h"
#include "gpu/disp/disp_objs.h"
#include "class/cl5070.h"
#include "mem_mgr/mem.h"
#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"

NV_STATUS
dispobjCtrlCmdEventSetTrigger_IMPL
(
    DispObject *pDispObject
)
{
    OBJGPU        *pGpu = DISPAPI_GET_GPU(pDispObject);
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);

    kdispNotifyEvent(pGpu, pKernelDisplay, NV5070_NOTIFIERS_SW, NULL, 0, 0, 0);

    return NV_OK;
}

NV_STATUS
dispobjCtrlCmdEventSetMemoryNotifies_IMPL
(
    DispObject *pDispObject,
    NV5070_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *pSetMemoryNotifiesParams
)
{
    OBJGPU        *pGpu = DISPAPI_GET_GPU(pDispObject);
    DisplayApi    *pDisplayApi = staticCast(pDispObject, DisplayApi);
    RsClient      *pClient = RES_GET_CLIENT(pDispObject);
    Memory        *pMemory;
    NvU32         *pNotifyActions, i;

    // error check subDeviceInstance
    if (pSetMemoryNotifiesParams->subDeviceInstance >= gpumgrGetSubDeviceCountFromGpu(pGpu))
    {
        NV_PRINTF(LEVEL_INFO, "bad subDeviceInstance 0x%x\n",
                  pSetMemoryNotifiesParams->subDeviceInstance);
        return NV_ERR_INVALID_ARGUMENT;
    }

    pNotifyActions = pDisplayApi->pNotifyActions[pSetMemoryNotifiesParams->subDeviceInstance];

    // ensure there's no pending notifications
    for (i = 0; i < pDisplayApi->numNotifiers; i++)
    {
        if (pNotifyActions[i] != NV5070_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE)
        {
            return NV_ERR_STATE_IN_USE;
        }
    }

    if (pSetMemoryNotifiesParams->hMemory == NV01_NULL_OBJECT)
    {
        pDisplayApi->hNotifierMemory = pSetMemoryNotifiesParams->hMemory;
        pDisplayApi->pNotifierMemory = NULL;
        return NV_OK;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT,
        memGetByHandle(pClient, pSetMemoryNotifiesParams->hMemory, &pMemory));

    if (pMemory->pMemDesc->Size < sizeof(NvNotification) * pDisplayApi->numNotifiers)
    {
        return NV_ERR_INVALID_LIMIT;
    }

    pDisplayApi->hNotifierMemory = pSetMemoryNotifiesParams->hMemory;
    pDisplayApi->pNotifierMemory = pMemory;

    return NV_OK;
}
