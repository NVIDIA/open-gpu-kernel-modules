/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlc36f.finn
//




/* VOLTA_CHANNEL_GPFIFO_A control commands and parameters */

#include "ctrl/ctrlxxxx.h"
#define NVC36F_CTRL_CMD(cat,idx)  \
    NVXXXX_CTRL_CMD(0xC36F, NVC36F_CTRL_##cat, idx)

/* VOLTA_CHANNEL_GPFIFO_A command categories (6bits) */
#define NVC36F_CTRL_RESERVED (0x00)
#define NVC36F_CTRL_GPFIFO   (0x01)
#define NVC36F_CTRL_INTERNAL (0x03)

/*
 * NVC36F_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned is: NV_OK
*/
#define NVC36F_CTRL_CMD_NULL (0xc36f0000) /* finn: Evaluated from "(FINN_VOLTA_CHANNEL_GPFIFO_A_RESERVED_INTERFACE_ID << 8) | 0x0" */



/*
 * NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN
 *
 *    This command returns an opaque work submit token to the caller which
 *    can be used to write to doorbell register to finish submitting work. If
 *    the client has provided an error context handle during channel allocation,
 *    and the error context is large enough to write the doorbell token, a
 *    notification at index NV_CHANNELGPFIFO_NOTIFICATION_TYPE_WORK_SUBMIT_TOKEN
 *    will be sent with the work submit token value. This index may be modified
 *    by NVC36F_CTRL_CMD_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX.
 *
 *    workSubmitToken       The 32-bit work submit token
 *
 *    Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_OBJECT_HANDLE
 *     NV_ERR_INVALID_ARGUMENT
 *
 */

#define NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN (0xc36f0108) /* finn: Evaluated from "(FINN_VOLTA_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS_MESSAGE_ID" */

#define NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS_MESSAGE_ID (0x8U)

typedef struct NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS {
    NvU32 workSubmitToken;
} NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS;

/**
 * NVC36F_CTRL_CMD_GPFIFO_UPDATE_FAULT_METHOD_BUFFER
 *
 *    This command updates the HOST CE Fault method buffer
 *    data structure of Virtual channel created for SR-IOV guest. It will also
 *    update the fault method buffer address in the instance block of the channel
 *
 *    bar2Addr[]
 *           Array contains the Virtual BAR2 address mapped by the Guest during channel
 *           creation.
 *    Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_INSUFFICIENT_RESOURCES
 *
 */
#define NVC36F_CTRL_CMD_GPFIFO_FAULT_METHOD_BUFFER_MAX_RUNQUEUES 0x2
#define NVC36F_CTRL_CMD_GPFIFO_UPDATE_FAULT_METHOD_BUFFER        (0xc36f0109) /* finn: Evaluated from "(FINN_VOLTA_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVC36F_CTRL_GPFIFO_UPDATE_FAULT_METHOD_BUFFER_PARAMS_MESSAGE_ID" */

#define NVC36F_CTRL_GPFIFO_UPDATE_FAULT_METHOD_BUFFER_PARAMS_MESSAGE_ID (0x9U)

typedef struct NVC36F_CTRL_GPFIFO_UPDATE_FAULT_METHOD_BUFFER_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 bar2Addr[NVC36F_CTRL_CMD_GPFIFO_FAULT_METHOD_BUFFER_MAX_RUNQUEUES], 8);
} NVC36F_CTRL_GPFIFO_UPDATE_FAULT_METHOD_BUFFER_PARAMS;

/*
 * NVC36F_CTRL_CMD_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX
 *
 *    This command updates the notifier index within the error context notifier
 *    to write the doorbell token to. This interface cannot be used to cause the
 *    token to overwrite the RC notification slot. The notification slot
 *    referred to by the passed index must be within the bounds of the error
 *    context notifier object. In the case that multiple channels share the same
 *    error context, it is not illegal for the client to set the same index for
 *    multiple channels (however it is not recommended).
 *
 *    [IN] index
 *      Notification slot to write the doorbell token. The initial value of this
 *      index is NV_CHANNELGPFIFO_NOTIFICATION_TYPE_WORK_SUBMIT_TOKEN.
 *
 *    Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_OBJECT_HANDLE
 *     NV_ERR_INVALID_ARGUMENT
 */

#define NVC36F_CTRL_CMD_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX (0xc36f010a) /* finn: Evaluated from "(FINN_VOLTA_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS_MESSAGE_ID" */

#define NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS_MESSAGE_ID (0xAU)

typedef struct NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS {
    NvU32 index;
} NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS;



/*
 * NVC36F_CTRL_CMD_INTERNAL_GPFIFO_GET_WORK_SUBMIT_TOKEN
 *
 *    This command is an internal command sent from Kernel RM to Physical RM
 *    to get work submit token
 *
 *    Please see description of NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN for more information.
 *
 */

#define NVC36F_CTRL_CMD_INTERNAL_GPFIFO_GET_WORK_SUBMIT_TOKEN (0xc36f0301) /* finn: Evaluated from "(FINN_VOLTA_CHANNEL_GPFIFO_A_INTERNAL_INTERFACE_ID << 8) | NVC36F_CTRL_INTERNAL_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS_MESSAGE_ID" */

#define NVC36F_CTRL_INTERNAL_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS_MESSAGE_ID (0x1U)

typedef NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS NVC36F_CTRL_INTERNAL_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS;

/* _ctrlc36f.h_ */

