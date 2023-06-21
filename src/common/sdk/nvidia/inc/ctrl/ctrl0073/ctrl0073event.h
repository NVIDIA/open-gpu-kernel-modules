/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl0073/ctrl0073event.finn
//

#include "ctrl/ctrl0073/ctrl0073base.h"

/* NV04_DISPLAY_COMMON event-related control commands and parameters */

/*
 * NV0073_CTRL_CMD_EVENT_SET_NOTIFICATION
 *
 * This command sets event notification state for the associated display
 * object.  This command requires that an instance of NV01_EVENT has been
 * previously bound to the associated display object.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   hEvent
 *     This parameter specifies the handle of the NV01_EVENT instance
 *     to be bound to the given subDeviceInstance.
 *   event
 *     This parameter specifies the type of event to which the specified
 *     action is to be applied.  This parameter must specify a valid
 *     NV0073_NOTIFIERS value (see cl0073.h for more details) and should
 *     not exceed one less NV0073_NOTIFIERS_MAXCOUNT.
 *   action
 *     This parameter specifies the desired event notification action.
 *     Valid notification actions include:
 *       NV0073_CTRL_SET_EVENT_NOTIFICATION_DISABLE
 *         This action disables event notification for the specified
 *         event for the associated subdevice object.
 *       NV0073_CTRL_SET_EVENT_NOTIFICATION_SINGLE
 *         This action enables single-shot event notification for the
 *         specified event for the associated subdevice object.
 *       NV0073_CTRL_SET_EVENT_NOTIFICATION_REPEAT
 *         This action enables repeated event notification for the specified
 *         event for the associated system controller object.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV0073_CTRL_CMD_EVENT_SET_NOTIFICATION (0x730301U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_EVENT_INTERFACE_ID << 8) | NV0073_CTRL_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0073_CTRL_EVENT_SET_NOTIFICATION_PARAMS {
    NvU32    subDeviceInstance;
    NvHandle hEvent;
    NvU32    event;
    NvU32    action;
} NV0073_CTRL_EVENT_SET_NOTIFICATION_PARAMS;

/* valid action values */
#define NV0073_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE (0x00000000U)
#define NV0073_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE  (0x00000001U)
#define NV0073_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT  (0x00000002U)

/*
 * NV0073_CTRL_CMD_EVENT_SET_NOTIFIER_MEMORY
 *
 *     hMemory
 *       This parameter specifies the handle of the memory object
 *       that identifies the memory address translation for this
 *       subdevice instance's notification(s).  The beginning of the
 *       translation points to an array of notification data structures.
 *       The size of the translation must be at least large enough to hold the
 *       maximum number of notification data structures identified by
 *       the NV0073_MAX_NOTIFIERS value.
 *       Legal argument values must be instances of the following classes:
 *         NV01_NULL
 *         NV04_MEMORY
 *       When hMemory specifies the NV01_NULL_OBJECT value then any existing
 *       memory translation connection is cleared.  There must not be any
 *       pending notifications when this command is issued.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV0073_CTRL_CMD_EVENT_SET_MEMORY_NOTIFIES         (0x730303U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_EVENT_INTERFACE_ID << 8) | NV0073_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV0073_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS {
    NvU32    subDeviceInstance;
    NvHandle hMemory;
} NV0073_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS;

#define NV0073_EVENT_MEMORY_NOTIFIES_STATUS_NOTIFIED 0U
#define NV0073_EVENT_MEMORY_NOTIFIES_STATUS_PENDING  1U
#define NV0073_EVENT_MEMORY_NOTIFIES_STATUS_ERROR    2U

/* _ctrl0073event_h_ */
