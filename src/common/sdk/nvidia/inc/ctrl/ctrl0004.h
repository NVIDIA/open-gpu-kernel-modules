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
// Source file:      ctrl/ctrl0004.finn
//

#include "ctrl/ctrlxxxx.h"
/* NV01_TIMER control commands and parameters */

#define NV0004_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0x0004, NV0004_CTRL_##cat, idx)

/* NV01_TIMER command categories (8bits) */
#define NV0004_CTRL_RESERVED (0x00)
#define NV0004_CTRL_TMR      (0x01)

/*
 * NV0004_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV0004_CTRL_CMD_NULL (0x40000) /* finn: Evaluated from "(FINN_NV01_TIMER_RESERVED_INTERFACE_ID << 8) | 0x0" */





/*
 * NV0004_CTRL_CMD_TMR_SET_ALARM_NOTIFY
 *
 * This command can be used to set a PTIMER alarm to trigger at the
 * specified time in the future on the subdevice associated with this
 * NV01_TIMER object instance.
 *
 *   hEvent
 *     This parameter specifies the handle of an NV01_EVENT object instance
 *     that is to be signaled when the alarm triggers.  This NV01_EVENT
 *     object instance must have been allocated with this NV01_TIMER object
 *     instance as its parent.  If this parameter is set to NV01_NULL_OBJECT
 *     then all NV01_EVENT object instances associated with this NV01_TIMER
 *     object instance are signaled.
 *   alarmTimeUsecs
 *     This parameter specifies the relative time in nanoseconds at which
 *     the alarm should trigger.  Note that the accuracy between the alarm
 *     trigger and the subsequent notification to the caller can vary
 *     depending on system conditions.
 *
 * Possible status values returned include:
 *   NVOS_STATUS_SUCCES
 *   NVOS_STATUS_INVALID_PARAM_STRUCT
 *   NVOS_STATUS_INVALID_OBJECT_HANDLE
 */

#define NV0004_CTRL_CMD_TMR_SET_ALARM_NOTIFY (0x40110) /* finn: Evaluated from "(FINN_NV01_TIMER_TMR_INTERFACE_ID << 8) | NV0004_CTRL_TMR_SET_ALARM_NOTIFY_PARAMS_MESSAGE_ID" */

#define NV0004_CTRL_TMR_SET_ALARM_NOTIFY_PARAMS_MESSAGE_ID (0x10U)

typedef struct NV0004_CTRL_TMR_SET_ALARM_NOTIFY_PARAMS {
    NvHandle hEvent;
    NV_DECLARE_ALIGNED(NvU64 alarmTimeNsecs, 8);
} NV0004_CTRL_TMR_SET_ALARM_NOTIFY_PARAMS;

/* _ctrl0004_h_ */

