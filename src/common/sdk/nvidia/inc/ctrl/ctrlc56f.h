/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlc56f.finn
//




/* AMPERE_CHANNEL_GPFIFO_A control commands and parameters */

#include "ctrl/ctrlxxxx.h"
#include "ctrl/ctrl906f.h"          /* C36F is partially derived from 906F */
#include "ctrl/ctrla06f.h"          /* C36F is partially derived from a06F */
#include "ctrl/ctrlc36f.h" // This control call interface is an ALIAS of C36F

#include "cc_drv.h"



#define NVC56F_CTRL_CMD(cat,idx)  \
    NVXXXX_CTRL_CMD(0xC36F, NVC56F_CTRL_##cat, idx)

/* AMPERE_CHANNEL_GPFIFO_A command categories (6bits) */
#define NVC56F_CTRL_RESERVED (0x00)
#define NVC56F_CTRL_GPFIFO   (0x01)
#define NVC56F_CTRL_EVENT    (0x02)

/*
 * NVC56F_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned is: NV_OK
*/
#define NVC56F_CTRL_CMD_NULL (NVC36F_CTRL_CMD_NULL)






/*
 * NVC56F_CTRL_GET_CLASS_ENGINEID
 *
 * Please see description of NV906F_CTRL_GET_CLASS_ENGINEID for more information.
 *
 */
#define NVC56F_CTRL_GET_CLASS_ENGINEID (NVC36F_CTRL_GET_CLASS_ENGINEID)

typedef NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS NVC56F_CTRL_GET_CLASS_ENGINEID_PARAMS;

/*
 * NVC56F_CTRL_RESET_CHANNEL
 *
 * Please see description of NV906F_CTRL_CMD_RESET_CHANNEL for more information.
 *
 */
#define NVC56F_CTRL_CMD_RESET_CHANNEL (NVC36F_CTRL_CMD_RESET_CHANNEL)

typedef NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS NVC56F_CTRL_CMD_RESET_CHANNEL_PARAMS;

/*
 * NVC56F_CTRL_CMD_GPFIFO_SCHEDULE
 *
 * Please see description of NVA06F_CTRL_CMD_GPFIFO_SCHEDULE for more information.
 *
 */
#define NVC56F_CTRL_CMD_GPFIFO_SCHEDULE (NVC36F_CTRL_CMD_GPFIFO_SCHEDULE)

typedef NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS NVC56F_CTRL_GPFIFO_SCHEDULE_PARAMS;

/*
 * NVC56F_CTRL_CMD_BIND
 *
 * Please see description of NVA06F_CTRL_CMD_BIND for more information.
 */
#define NVC56F_CTRL_CMD_BIND (NVC36F_CTRL_CMD_BIND)

typedef NVA06F_CTRL_BIND_PARAMS NVC56F_CTRL_BIND_PARAMS;

/*
 * NVC56F_CTRL_CMD_EVENT_SET_NOTIFICATION
 *
 * Please see description of NVA06F_CTRL_CMD_EVENT_SET_NOTIFICATION for more information.
*/
#define NVC56F_CTRL_CMD_EVENT_SET_NOTIFICATION (NVC36F_CTRL_CMD_EVENT_SET_NOTIFICATION)

typedef NVA06F_CTRL_EVENT_SET_NOTIFICATION_PARAMS NVC56F_CTRL_EVENT_SET_NOTIFICATION_PARAMS;

/* valid action values */
#define NVC56F_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE NVA06F_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE
#define NVC56F_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE  NVA06F_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE
#define NVC56F_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT  NVA06F_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT

/*
 * NVC56F_CTRL_CMD_EVENT_SET_TRIGGER
 *
 * Please see description of NVA06F_CTRL_CMD_EVENT_SET_TRIGGER for more information.
 */
#define NVC56F_CTRL_CMD_EVENT_SET_TRIGGER                 (NVC36F_CTRL_CMD_EVENT_SET_TRIGGER)





/*
 * NVC56F_CTRL_CMD_GET_MMU_FAULT_INFO
 *
 * Please see description of NV906F_CTRL_CMD_GET_MMU_FAULT_INFO for more information.
 *
 */
#define NVC56F_CTRL_CMD_GET_MMU_FAULT_INFO (NVC36F_CTRL_CMD_GET_MMU_FAULT_INFO)

typedef NV906F_CTRL_GET_MMU_FAULT_INFO_PARAMS NVC56F_CTRL_GET_MMU_FAULT_INFO_PARAMS;

/*
 * NVC56F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN
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

#define NVC56F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN (NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN)

typedef struct NVC56F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS {
    NvU32 workSubmitToken;
} NVC56F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS;

/*
 * NVC56F_CTRL_CMD_GET_KMB
 *
 *    This command returns the Key Material Bundle (KMB) for the current channel.
 *
 *    kmb [OUT]           The KMB for the channel.
 *
 *    Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_OBJECT_HANDLE
 *     NV_ERR_INVALID_ARGUMENT
 *     NV_ERR_NOT_SUPPORTED
 */
#define NVC56F_CTRL_CMD_GET_KMB (0xc56f010b) /* finn: Evaluated from "(FINN_AMPERE_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVC56F_CTRL_CMD_GET_KMB_PARAMS_MESSAGE_ID" */

#define NVC56F_CTRL_CMD_GET_KMB_PARAMS_MESSAGE_ID (0xBU)

typedef struct NVC56F_CTRL_CMD_GET_KMB_PARAMS {
    CC_KMB kmb;
} NVC56F_CTRL_CMD_GET_KMB_PARAMS;

/*
 * NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV
 *
 *    This command rotates the IVs for secure channels.
 *
 *    rotateIvType [IN]    Which IVs to rotate.
 *    updatedKmb [OUT]     Updated KMB after the IV rotation.
 *
 *    Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_OBJECT_HANDLE
 *     NV_ERR_INVALID_ARGUMENT
 *     NV_ERR_NOT_SUPPORTED
 */
#define NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV (0xc56f010c) /* finn: Evaluated from "(FINN_AMPERE_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS_MESSAGE_ID" */

#define NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS_MESSAGE_ID (0xCU)

typedef struct NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS {
    ROTATE_IV_TYPE rotateIvType;
    CC_KMB         updatedKmb;
} NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS;



/* _ctrlc56f.h_ */

