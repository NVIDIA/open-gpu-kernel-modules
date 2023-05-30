/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl208f/ctrl208fevent.finn
//

#include "ctrl/ctrl208f/ctrl208fbase.h"

/* NV20_SUBDEVICE_DIAG event-related control commands and parameters */

/*
 * NV208F_CTRL_CMD_EVENT_SET_NOTIFICATION
 *
 * This command sets event notification state for the associated subdevice.
 * This command requires that an instance of NV01_EVENT has been previously
 * bound to the associated subdevice object.
 *
 *   event
 *     This parameter specifies the type of event to which the specified
 *     action is to be applied.  This parameter must specify a valid
 *     NV208F_NOTIFIERS value (see cl208f.h for more details) and should
 *     not exceed one less NV208F_NOTIFIERS_MAXCOUNT.
 *   action
 *     This parameter specifies the desired event notification action.
 *     Valid notification actions include:
 *       NV208F_CTRL_SET_EVENT_NOTIFICATION_DISABLE
 *         This action disables event notification for the specified
 *         event for the associated subdevice object.
 *       NV208F_CTRL_SET_EVENT_NOTIFICATION_SINGLE
 *         This action enables single-shot event notification for the
 *         specified event for the associated subdevice object.
 *       NV208F_CTRL_SET_EVENT_NOTIFICATION_REPEAT
 *         This action enables repeated event notification for the specified
 *         event for the associated system controller object.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV208F_CTRL_CMD_EVENT_SET_NOTIFICATION (0x208f1001) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_EVENT_INTERFACE_ID << 8) | NV208F_CTRL_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV208F_CTRL_EVENT_SET_NOTIFICATION_PARAMS {
    NvU32 event;
    NvU32 action;
} NV208F_CTRL_EVENT_SET_NOTIFICATION_PARAMS;

/* valid action values */
#define NV208F_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE (0x00000000)
#define NV208F_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE  (0x00000001)
#define NV208F_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT  (0x00000002)

/*
 * NV208F_CTRL_CMD_EVENT_SET_TRIGGER
 *
 * This command triggers a software event for the associated subdevice.
 * This command accepts no parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV208F_CTRL_CMD_EVENT_SET_TRIGGER                 (0x208f1002) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_EVENT_INTERFACE_ID << 8) | 0x2" */

/* _ctrl208fevent_h_ */
