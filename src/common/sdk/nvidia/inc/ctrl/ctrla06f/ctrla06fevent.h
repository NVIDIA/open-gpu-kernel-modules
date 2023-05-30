/*
 * SPDX-FileCopyrightText: Copyright (c) 2007-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrla06f/ctrla06fevent.finn
//

#include "ctrl/ctrla06f/ctrla06fbase.h"

/*
 * NVA06F_CTRL_CMD_EVENT_SET_NOTIFICATION
 *
 * This command sets event notification state for the associated channel.
 * This command requires that an instance of NV01_EVENT has been previously
 * bound to the associated channel object.
 *
 *   event
 *     This parameter specifies the type of event to which the specified
 *     action is to be applied.  This parameter must specify a valid
 *     NVA06F_NOTIFIERS value (see cla06f.h for more details) and should
 *     not exceed one less NVA06F_NOTIFIERS_MAXCOUNT.
 *   action
 *     This parameter specifies the desired event notification action.
 *     Valid notification actions include:
 *       NVA06F_CTRL_SET_EVENT_NOTIFICATION_ACTION_DISABLE
 *         This action disables event notification for the specified
 *         event for the associated channel object.
 *       NVA06F_CTRL_SET_EVENT_NOTIFICATION_ACTION_SINGLE
 *         This action enables single-shot event notification for the
 *         specified event for the associated channel object.
 *       NVA06F_CTRL_SET_EVENT_NOTIFICATION_ACTION_REPEAT
 *         This action enables repeated event notification for the specified
 *         event for the associated channel object.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NVA06F_CTRL_CMD_EVENT_SET_NOTIFICATION (0xa06f0205) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GPFIFO_A_EVENT_INTERFACE_ID << 8) | NVA06F_CTRL_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID" */

#define NVA06F_CTRL_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID (0x5U)

typedef struct NVA06F_CTRL_EVENT_SET_NOTIFICATION_PARAMS {
    NvU32 event;
    NvU32 action;
} NVA06F_CTRL_EVENT_SET_NOTIFICATION_PARAMS;

/* valid action values */
#define NVA06F_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE (0x00000000)
#define NVA06F_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE  (0x00000001)
#define NVA06F_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT  (0x00000002)

/*
 * NVA06F_CTRL_CMD_EVENT_SET_TRIGGER
 *
 * This command triggers a software event for the associated channel.
 * This command accepts no parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVA06F_CTRL_CMD_EVENT_SET_TRIGGER                 (0xa06f0206) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GPFIFO_A_EVENT_INTERFACE_ID << 8) | 0x6" */


/* _ctrla06fevent_h_ */
