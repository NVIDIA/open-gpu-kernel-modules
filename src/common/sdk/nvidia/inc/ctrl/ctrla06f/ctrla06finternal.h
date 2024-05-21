/*
 * SPDX-FileCopyrightText: Copyright (c) 2007-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrla06f/ctrla06finternal.finn
//

#include "ctrl/ctrla06f/ctrla06fbase.h"
#include "ctrl/ctrla06f/ctrla06fgpfifo.h"

/*
 * NVA06F_CTRL_CMD_INTERNAL_STOP_CHANNEL
 *
 * This command is an internal command sent from Kernel RM to Physical RM
 * to stop the channel in hardware
 *
 * Please see description of NVA06F_CTRL_CMD_STOP_CHANNEL for more information.
 *
 */
#define NVA06F_CTRL_CMD_INTERNAL_STOP_CHANNEL (0xa06f0301) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GPFIFO_A_INTERNAL_INTERFACE_ID << 8) | NVA06F_CTRL_INTERNAL_STOP_CHANNEL_PARAMS_MESSAGE_ID" */

#define NVA06F_CTRL_INTERNAL_STOP_CHANNEL_PARAMS_MESSAGE_ID (0x1U)

typedef NVA06F_CTRL_STOP_CHANNEL_PARAMS NVA06F_CTRL_INTERNAL_STOP_CHANNEL_PARAMS;

/*
 * NVA06F_CTRL_CMD_INTERNAL_GPFIFO_SCHEDULE
 *
 * This command is an internal command sent from Kernel RM to Physical RM
 * to schedule the channel in hardware
 *
 * Please see description of NVA06F_CTRL_CMD_GPFIFO_SCHEDULE for more information.
 *
 */
#define NVA06F_CTRL_CMD_INTERNAL_GPFIFO_SCHEDULE (0xa06f0303) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GPFIFO_A_INTERNAL_INTERFACE_ID << 8) | NVA06F_CTRL_INTERNAL_GPFIFO_SCHEDULE_PARAMS_MESSAGE_ID" */

#define NVA06F_CTRL_INTERNAL_GPFIFO_SCHEDULE_PARAMS_MESSAGE_ID (0x3U)

typedef NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS NVA06F_CTRL_INTERNAL_GPFIFO_SCHEDULE_PARAMS;

/* ctrla06finternal_h */
