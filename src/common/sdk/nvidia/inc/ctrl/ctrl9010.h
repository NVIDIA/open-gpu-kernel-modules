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
// Source file:      ctrl/ctrl9010.finn
//

#include "ctrl/ctrlxxxx.h"
#define NV9010_CTRL_CMD(cat,idx)  \
    NVXXXX_CTRL_CMD(0x9010, NV9010_CTRL_##cat, idx)

/* Command categories (6bits) */
#define NV9010_CTRL_RESERVED        (0x00)
#define NV9010_CTRL_VBLANK_CALLBACK (0x01)


/*
 * NV9010_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV9010_CTRL_CMD_NULL        (0x90100000) /* finn: Evaluated from "(FINN_NV9010_VBLANK_CALLBACK_RESERVED_INTERFACE_ID << 8) | 0x0" */



/*
 * NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION
 *
 * This command is used to enable and disable vblank notifications. This 
 * is specially intended for cases where the RM client is calling from a
 * high IRQL context, where other mechanisms to toggle vblank notification
 * (such as freeing and reallocating the NV9010_VBLANK_CALLBACK object)
 * would not be suitable. As this is being invoked at the high IRQL, 
 * locking can be bypassed, if the NVOS54_FLAGS_LOCK_BYPASS flag is set on 
 * the control call.Here the OS will take care of the synchronization. 
 * The Windows Display Driver for Cobalt requires this, for example.
 *
 *    bSetVBlankNotifyEnable
 *       This parameter tell whether to enable or disable the Vblank notification
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION (0x90100101) /* finn: Evaluated from "(FINN_NV9010_VBLANK_CALLBACK_INTERFACE_ID << 8) | NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION_PARAMS_MESSAGE_ID" */

#define NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION_PARAMS_MESSAGE_ID (0x01U)

typedef struct NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION_PARAMS {
    NvBool bSetVBlankNotifyEnable;
} NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION_PARAMS;

/* _ctrl9010_h_ */
