/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl0000/ctrl0000event.finn
//

#include "ctrl/ctrl0000/ctrl0000base.h"

#include "ctrl/ctrlxxxx.h"
#include "class/cl0000.h"
/*
 * NV0000_CTRL_CMD_EVENT_SET_NOTIFICATION
 *
 * This command sets event notification for the system events.
 * 
 *   event
 *     This parameter specifies the type of event to which the specified
 *     action is to be applied. The valid event values can be found in
 *     cl0000.h. 
 *
 *   action
 *     This parameter specifies the desired event notification action.
 *     Valid notification actions include:
 *       NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE
 *         This action disables event notification for the specified
 *         event.
 *       NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE
 *         This action enables single-shot event notification for the
 *         specified event.
 *       NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT
 *         This action enables repeated event notification for the 
 *         specified event.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_CLIENT
 *
 */

#define NV0000_CTRL_CMD_EVENT_SET_NOTIFICATION (0x501) /* finn: Evaluated from "(FINN_NV01_ROOT_EVENT_INTERFACE_ID << 8) | NV0000_CTRL_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0000_CTRL_EVENT_SET_NOTIFICATION_PARAMS {
    NvU32 event;
    NvU32 action;
} NV0000_CTRL_EVENT_SET_NOTIFICATION_PARAMS;

/* valid action values */
#define NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE (0x00000000)
#define NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE  (0x00000001)
#define NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT  (0x00000002)

/*
 * NV0000_CTRL_CMD_GET_SYSTEM_EVENT_STATUS
 *
 * This command returns the status of the specified system event type.
 * See the description of NV01_EVENT for details on registering events.
 *
 *   event
 *     This parameter specifies the event type. Valid event type values
 *     can be found in cl0000.h.
 *   status
 *     This parameter returns the status for a given event type. Valid
 *     status values can be found in cl0000.h.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_CLIENT
 *
 */

#define NV0000_CTRL_CMD_GET_SYSTEM_EVENT_STATUS           (0x502) /* finn: Evaluated from "(FINN_NV01_ROOT_EVENT_INTERFACE_ID << 8) | NV0000_CTRL_GET_SYSTEM_EVENT_STATUS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GET_SYSTEM_EVENT_STATUS_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0000_CTRL_GET_SYSTEM_EVENT_STATUS_PARAMS {
    NvU32 event;
    NvU32 status;
} NV0000_CTRL_GET_SYSTEM_EVENT_STATUS_PARAMS;

/* _ctrl0000event_h_ */

