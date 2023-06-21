/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlc06f.finn
//




/* PASCAL_CHANNEL_GPFIFO_A control commands and parameters */

#include "ctrl/ctrlxxxx.h"
#include "ctrl/ctrla06f.h"          /* C06F is partially derived from A06F */
#include "ctrl/ctrl906f.h"          /* C06F is partially derived from 906F */
#define NVC06F_CTRL_CMD(cat,idx)  \
    NVXXXX_CTRL_CMD(0xC06F, NVC06F_CTRL_##cat, idx)

/* PASCAL_CHANNEL_GPFIFO_B command categories (6bits) */
#define NVC06F_CTRL_RESERVED (0x00)
#define NVC06F_CTRL_GPFIFO   (0x01)
#define NVC06F_CTRL_EVENT    (0x02)

/*
 * NVC06F_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVC06F_CTRL_CMD_NULL (0xc06f0000) /* finn: Evaluated from "(FINN_PASCAL_CHANNEL_GPFIFO_A_RESERVED_INTERFACE_ID << 8) | 0x0" */





/*
 * NVC06F_CTRL_GET_CLASS_ENGINEID
 *
 * Please see description of NV906F_CTRL_GET_CLASS_ENGINEID for more information.
 *   
 */
#define NVC06F_CTRL_GET_CLASS_ENGINEID (0xc06f0101) /* finn: Evaluated from "(FINN_PASCAL_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVC06F_CTRL_GET_CLASS_ENGINEID_PARAMS_MESSAGE_ID" */

#define NVC06F_CTRL_GET_CLASS_ENGINEID_PARAMS_MESSAGE_ID (0x1U)

typedef NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS NVC06F_CTRL_GET_CLASS_ENGINEID_PARAMS;

/*
 * NVC06F_CTRL_RESET_CHANNEL
 *
 * Please see description of NVA06F_CTRL_RESET_CHANNEL for more information. 
 *   
 */
#define NVC06F_CTRL_CMD_RESET_CHANNEL (0xc06f0102) /* finn: Evaluated from "(FINN_PASCAL_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVC06F_CTRL_CMD_RESET_CHANNEL_PARAMS_MESSAGE_ID" */

#define NVC06F_CTRL_CMD_RESET_CHANNEL_PARAMS_MESSAGE_ID (0x2U)

typedef NVA06F_CTRL_CMD_RESET_CHANNEL_PARAMS NVC06F_CTRL_CMD_RESET_CHANNEL_PARAMS;

/*
 * NVC06F_CTRL_CMD_GPFIFO_SCHEDULE
 *
 * Please see description of NVA06F_CTRL_CMD_GPFIFO_SCHEDULE for more information.
 *
 */
#define NVC06F_CTRL_CMD_GPFIFO_SCHEDULE (0xc06f0103) /* finn: Evaluated from "(FINN_PASCAL_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVC06F_CTRL_GPFIFO_SCHEDULE_PARAMS_MESSAGE_ID" */

#define NVC06F_CTRL_GPFIFO_SCHEDULE_PARAMS_MESSAGE_ID (0x3U)

typedef NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS NVC06F_CTRL_GPFIFO_SCHEDULE_PARAMS;

/*
 * NVC06F_CTRL_CMD_BIND
 *
 * Please see description of NVA06F_CTRL_CMD_BIND for more information.
 */
#define NVC06F_CTRL_CMD_BIND (0xc06f0104) /* finn: Evaluated from "(FINN_PASCAL_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVC06F_CTRL_BIND_PARAMS_MESSAGE_ID" */

#define NVC06F_CTRL_BIND_PARAMS_MESSAGE_ID (0x4U)

typedef NVA06F_CTRL_BIND_PARAMS NVC06F_CTRL_BIND_PARAMS;

/*
 * NVC06F_CTRL_CMD_EVENT_SET_NOTIFICATION
 *
 * Please see description of NVA06F_CTRL_CMD_EVENT_SET_NOTIFICATION for more information.
*/
#define NVC06F_CTRL_CMD_EVENT_SET_NOTIFICATION (0xc06f0205) /* finn: Evaluated from "(FINN_PASCAL_CHANNEL_GPFIFO_A_EVENT_INTERFACE_ID << 8) | NVC06F_CTRL_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID" */

#define NVC06F_CTRL_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID (0x5U)

typedef NVA06F_CTRL_EVENT_SET_NOTIFICATION_PARAMS NVC06F_CTRL_EVENT_SET_NOTIFICATION_PARAMS;

/* valid action values */
#define NVC06F_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE NVA06F_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE
#define NVC06F_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE  NVA06F_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE
#define NVC06F_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT  NVA06F_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT

/*
 * NVC06F_CTRL_CMD_EVENT_SET_TRIGGER
 *
 * Please see description of NVA06F_CTRL_CMD_EVENT_SET_TRIGGER for more information.
 */
#define NVC06F_CTRL_CMD_EVENT_SET_TRIGGER                 (0xc06f0206) /* finn: Evaluated from "(FINN_PASCAL_CHANNEL_GPFIFO_A_EVENT_INTERFACE_ID << 8) | 0x6" */





/*
 * NVC06F_CTRL_CMD_GET_MMU_FAULT_INFO
 *
 * Please see description of NV906F_CTRL_CMD_GET_MMU_FAULT_INFO for more information.
 *   
 */
#define NVC06F_CTRL_CMD_GET_MMU_FAULT_INFO (0xc06f0107) /* finn: Evaluated from "(FINN_PASCAL_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVC06F_CTRL_GET_MMU_FAULT_INFO_PARAMS_MESSAGE_ID" */

#define NVC06F_CTRL_GET_MMU_FAULT_INFO_PARAMS_MESSAGE_ID (0x7U)

typedef NV906F_CTRL_GET_MMU_FAULT_INFO_PARAMS NVC06F_CTRL_GET_MMU_FAULT_INFO_PARAMS;

/* _ctrlc06f.h_ */
