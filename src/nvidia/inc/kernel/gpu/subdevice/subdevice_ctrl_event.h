/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

    RMCTRL_EXPORT(NV2080_CTRL_CMD_EVENT_SET_TRIGGER,
                  RMCTRL_FLAGS(NON_PRIVILEGED))
    NV_STATUS subdeviceCtrlCmdEventSetTrigger(Subdevice *pSubdevice);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_EVENT_SET_TRIGGER_FIFO,
                  RMCTRL_FLAGS(NO_GPUS_LOCK, NON_PRIVILEGED))
    NV_STATUS subdeviceCtrlCmdEventSetTriggerFifo(Subdevice *pSubdevice,
                                NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS *pTriggerFifoParams);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION,
                  RMCTRL_FLAGS(NON_PRIVILEGED, API_LOCK_READONLY, GPU_LOCK_DEVICE_ONLY))
    NV_STATUS subdeviceCtrlCmdEventSetNotification(Subdevice *pSubdevice,
                                NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_EVENT_SET_MEMORY_NOTIFIES,
                  RMCTRL_FLAGS(NON_PRIVILEGED))
    NV_STATUS subdeviceCtrlCmdEventSetMemoryNotifies(Subdevice *pSubdevice,
                                NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *pSetMemoryNotifiesParams);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_EVENT_SET_SEMAPHORE_MEMORY,
                  RMCTRL_FLAGS(NON_PRIVILEGED))
    NV_STATUS subdeviceCtrlCmdEventSetSemaphoreMemory(Subdevice *pSubdevice,
                                NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS *pSetSemMemoryParams);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_EVENT_SET_SEMA_MEM_VALIDATION,
                  RMCTRL_FLAGS(NON_PRIVILEGED))
    NV_STATUS subdeviceCtrlCmdEventSetSemaMemValidation(Subdevice *pSubdevice,
                                NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS *pSetSemaMemValidationParams);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_EVENT_VIDEO_BIND_EVTBUF,
                  RMCTRL_FLAGS(NON_PRIVILEGED, API_LOCK_READONLY, GPU_LOCK_DEVICE_ONLY))
    NV_STATUS subdeviceCtrlCmdEventVideoBindEvtbuf(Subdevice *pSubdevice,
                                NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_PARAMS *pBindParams);

