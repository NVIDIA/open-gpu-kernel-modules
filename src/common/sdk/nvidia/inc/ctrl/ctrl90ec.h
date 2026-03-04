/*
 * SPDX-FileCopyrightText: Copyright (c) 2011-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl90ec.finn
//

#include "ctrl/ctrlxxxx.h"
/* GK104 HDACODEC control commands and parameters */

#define NV90EC_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0x90EC, NV90EC_CTRL_##cat, idx)

/* NV04_DISPLAY_COMMON command categories (6bits) */
#define NV90EC_CTRL_RESERVED (0x00)
#define NV90EC_CTRL_HDACODEC (0x01)

/*
 * NV90EC_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV90EC_CTRL_CMD_NULL (0x90ec0000) /* finn: Evaluated from "(FINN_GF100_HDACODEC_RESERVED_INTERFACE_ID << 8) | 0x0" */





/* 
 * NV90EC_CTRL_CMD_HDACODEC_SET_CP_READY_ENABLE
 *
 * This command sets the CP_READY bit. It basically informs RM whether
 * the DD has worked upon the HDCP request requested by the Audio driver 
 * or not. DD asks RM to enable CP_READY bit (by setting CpReadyEnable to NV_TRUE)
 * once it is done honouring/dishonouring the request.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be 
 *     directed. This parameter must specify a value between zero and the  
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the display for which the cp ready
 *     bit should be enabled.  The display ID must a dfp display.
 *     If the displayId is not a dfp, this call will return 
 *     NV_ERR_INVALID_ARGUMENT.
 *   CpReadyEnable
 *     This parameter specifies whether to enable (NV_TRUE) or not. If CpReady
 *     is enabled then AudioCodec can send more HDCP requests.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 * 
 * 
 */
#define NV90EC_CTRL_CMD_HDACODEC_SET_CP_READY_ENABLE (0x90ec0101) /* finn: Evaluated from "(FINN_GF100_HDACODEC_HDACODEC_INTERFACE_ID << 8) | NV90EC_CTRL_CMD_HDACODEC_SET_CP_READY_ENABLE_PARAMS_MESSAGE_ID" */

#define NV90EC_CTRL_CMD_HDACODEC_SET_CP_READY_ENABLE_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV90EC_CTRL_CMD_HDACODEC_SET_CP_READY_ENABLE_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvBool bCpReadyEnable;
} NV90EC_CTRL_CMD_HDACODEC_SET_CP_READY_ENABLE_PARAMS;

/* 
 * NV90EC_CTRL_CMD_HDACODEC_NOTIFY_AUDIO_EVENT
 *
 * This command notifies Audio of any events to audio
 * like notification of PD bit being set.
 *
 *   audioEvent
 *     This parameter specifies the event type.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 * 
 * 
 */
#define NV90EC_CTRL_CMD_HDACODEC_NOTIFY_AUDIO_EVENT (0x90ec0102) /* finn: Evaluated from "(FINN_GF100_HDACODEC_HDACODEC_INTERFACE_ID << 8) | NV90EC_CTRL_HDACODEC_NOTIFY_AUDIO_EVENT_PARAMS_MESSAGE_ID" */

#define NV90EC_CTRL_HDACODEC_NOTIFY_AUDIO_EVENT_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV90EC_CTRL_HDACODEC_NOTIFY_AUDIO_EVENT_PARAMS {
    NvU32 audioEvent;
} NV90EC_CTRL_HDACODEC_NOTIFY_AUDIO_EVENT_PARAMS;

/*
 * This command notifies audio driver that PD bit is set by DD, by writing to scratch register
 */
#define NV90EC_CTRL_HDACODEC_AUDIOEVENT_PD_BIT_SET (0x00000001)

/* _ctrl90ec_h_ */
