/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlc86f.finn
//



#include "nvcfg_sdk.h"



/* HOPPER_CHANNEL_GPFIFO_A control commands and parameters */

#include "ctrl/ctrlxxxx.h"
#include "ctrl/ctrl906f.h"          /* C36F is partially derived from 906F */
#include "ctrl/ctrla06f.h"          /* C36F is partially derived from a06F */
#include "ctrl/ctrlc36f.h" // This control call interface is an ALIAS of C36F

#define NVC86F_CTRL_CMD(cat,idx)  \
    NVXXXX_CTRL_CMD(0xC36F, NVC86F_CTRL_##cat, idx)

/* HOPPER_CHANNEL_GPFIFO_A command categories (6bits) */
#define NVC86F_CTRL_RESERVED (0x00)
#define NVC86F_CTRL_GPFIFO   (0x01)
#define NVC86F_CTRL_EVENT    (0x02)

/*
 * NVC86F_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned is: NV_OK
*/
#define NVC86F_CTRL_CMD_NULL (NVC36F_CTRL_CMD_NULL)






/*
 * NVC86F_CTRL_GET_CLASS_ENGINEID
 *
 * Please see description of NV906F_CTRL_GET_CLASS_ENGINEID for more information.
 *
 */
#define NVC86F_CTRL_GET_CLASS_ENGINEID (NVC36F_CTRL_GET_CLASS_ENGINEID)

typedef NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS NVC86F_CTRL_GET_CLASS_ENGINEID_PARAMS;

/*
 * NVC86F_CTRL_RESET_CHANNEL
 *
 * Please see description of NV906F_CTRL_CMD_RESET_CHANNEL for more information.
 *
 */
#define NVC86F_CTRL_CMD_RESET_CHANNEL (NVC36F_CTRL_CMD_RESET_CHANNEL)

typedef NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS NVC86F_CTRL_CMD_RESET_CHANNEL_PARAMS;

/*
 * NVC86F_CTRL_CMD_GPFIFO_SCHEDULE
 *
 * Please see description of NVA06F_CTRL_CMD_GPFIFO_SCHEDULE for more information.
 *
 */
#define NVC86F_CTRL_CMD_GPFIFO_SCHEDULE (NVC36F_CTRL_CMD_GPFIFO_SCHEDULE)

typedef NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS NVC86F_CTRL_GPFIFO_SCHEDULE_PARAMS;

/*
 * NVC86F_CTRL_CMD_BIND
 *
 * Please see description of NVA06F_CTRL_CMD_BIND for more information.
 */
#define NVC86F_CTRL_CMD_BIND (NVC36F_CTRL_CMD_BIND)

typedef NVA06F_CTRL_BIND_PARAMS NVC86F_CTRL_BIND_PARAMS;

/*
 * NVC86F_CTRL_CMD_EVENT_SET_NOTIFICATION
 *
 * Please see description of NVA06F_CTRL_CMD_EVENT_SET_NOTIFICATION for more information.
*/
#define NVC86F_CTRL_CMD_EVENT_SET_NOTIFICATION (NVC36F_CTRL_CMD_EVENT_SET_NOTIFICATION)

typedef NVA06F_CTRL_EVENT_SET_NOTIFICATION_PARAMS NVC86F_CTRL_EVENT_SET_NOTIFICATION_PARAMS;

/* valid action values */
#define NVC86F_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE NVA06F_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE
#define NVC86F_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE  NVA06F_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE
#define NVC86F_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT  NVA06F_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT

/*
 * NVC86F_CTRL_CMD_EVENT_SET_TRIGGER
 *
 * Please see description of NVA06F_CTRL_CMD_EVENT_SET_TRIGGER for more information.
 */
#define NVC86F_CTRL_CMD_EVENT_SET_TRIGGER                 (NVC36F_CTRL_CMD_EVENT_SET_TRIGGER)





/*
 * NVC86F_CTRL_CMD_GET_MMU_FAULT_INFO
 *
 * Please see description of NV906F_CTRL_CMD_GET_MMU_FAULT_INFO for more information.
 *
 */
#define NVC86F_CTRL_CMD_GET_MMU_FAULT_INFO (NVC36F_CTRL_CMD_GET_MMU_FAULT_INFO)

typedef NV906F_CTRL_GET_MMU_FAULT_INFO_PARAMS NVC86F_CTRL_GET_MMU_FAULT_INFO_PARAMS;

/*
 * NVC86F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN
 *
 *    This command returns an opaque work submit token to the caller which
 *    can be used to write to doorbell register to finish submitting work.
 *
 *    workSubmitToken       The 32-bit work submit token
 *
 *    Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_OBJECT_HANDLE
 *     NV_ERR_INVALID_ARGUMENT
 *
 */

#define NVC86F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN (NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN)

typedef struct NVC86F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS {
    NvU32 workSubmitToken;
} NVC86F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS;

/* _ctrlC86F.h_ */

