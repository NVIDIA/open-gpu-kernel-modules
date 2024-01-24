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

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl906f.finn
//




/* GF100_GPFIFO control commands and parameters */

#include "ctrl/ctrlxxxx.h"
#define NV906F_CTRL_CMD(cat,idx)  \
    NVXXXX_CTRL_CMD(0x906F, NV906F_CTRL_##cat, idx)

/* GF100_GPFIFO command categories (6bits) */
#define NV906F_CTRL_RESERVED (0x00)
#define NV906F_CTRL_GPFIFO   (0x01)
#define NV906F_CTRL_EVENT    (0x02)


/*
 * NV906F_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV906F_CTRL_CMD_NULL (0x906f0000) /* finn: Evaluated from "(FINN_GF100_CHANNEL_GPFIFO_RESERVED_INTERFACE_ID << 8) | 0x0" */






/*
 * NV906F_CTRL_GET_CLASS_ENGINEID
 *
 * Takes an object handle as input and returns
 * the Class and Engine that this object uses.
 *
 * hObject
 *   Handle to an object created. For example a
 *   handle to object of type FERMI_A created by
 *   the client. This is supplied by the client
 *   of this call.
 *
 * classEngineID
 *   A concatenation of class and engineid
 *   that the object with handle hObject
 *   belongs to. This is returned by RM. The internal
 *   format of this data structure is opaque to clients.
 *
 * classID
 *   ClassID for object represented by hObject
 *
 * engineID
 *   EngineID for object represented by hObject
 *
 * Possible status values returned are:
 *   NV_OK
 *   If the call was successful.
 *
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   No object of handle hObject was found.
 */
#define NV906F_CTRL_GET_CLASS_ENGINEID (0x906f0101) /* finn: Evaluated from "(FINN_GF100_CHANNEL_GPFIFO_GPFIFO_INTERFACE_ID << 8) | NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS_MESSAGE_ID" */

#define NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS {
    NvHandle hObject;
    NvU32    classEngineID;
    NvU32    classID;
    NvU32    engineID;
} NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS;

/*
 * NV906F_CTRL_RESET_CHANNEL
 *
 * This command resets the channel corresponding to specified engine and also
 * resets the specified engine.
 *
 * Takes an engine ID as input.
 *
 * engineID
 *   This parameter specifies the engine to be reset.  See the description of the
 *   NV2080_ENGINE_TYPE values in cl2080.h for more information.
 * subdeviceInstance
 *   This parameter specifies the subdevice to be reset when in SLI.
 * resetReason
 *   Specifies reason to reset a channel.
 *
 * bIsRcPending
 *   Specifies if an RC is pending on the channel
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV906F_CTRL_CMD_RESET_CHANNEL_REASON_DEFAULT             0
#define NV906F_CTRL_CMD_RESET_CHANNEL_REASON_VERIF               1
#define NV906F_CTRL_CMD_RESET_CHANNEL_REASON_MMU_FLT             2
#define NV906F_CTRL_CMD_RESET_CHANNEL_REASON_ENUM_MAX            3
/*
 * Internal values for NV906F_CTRL_CMD_RESET_REASON. External values will be
 * checked and enforced to be < NV906F_CTRL_CMD_RESET_CHANNEL_REASON_ENUM_MAX
 */
#define NV906F_CTRL_CMD_INTERNAL_RESET_CHANNEL_REASON_FAKE_ERROR (0x4) /* finn: Evaluated from "NV906F_CTRL_CMD_RESET_CHANNEL_REASON_ENUM_MAX + 1" */


#define NV906F_CTRL_CMD_RESET_CHANNEL                            (0x906f0102) /* finn: Evaluated from "((FINN_GF100_CHANNEL_GPFIFO_GPFIFO_INTERFACE_ID << 8) | NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS_MESSAGE_ID)" */

#define NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS {
    NvU32  engineID;
    NvU32  subdeviceInstance;
    NvU32  resetReason;
    NvBool bIsRcPending;
} NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS;

/*
 * NV906F_CTRL_CMD_EVENT_SET_NOTIFICATION
 *
 * This command sets event notification state for the associated channel.
 * This command requires that an instance of NV01_EVENT has been previously
 * bound to the associated channel object.
 *
 *   event
 *     This parameter specifies the type of event to which the specified
 *     action is to be applied.  This parameter must specify a valid
 *     NV906F_NOTIFIERS value (see cl906f.h for more details) and should
 *     not exceed one less NV906F_NOTIFIERS_MAXCOUNT.
 *   action
 *     This parameter specifies the desired event notification action.
 *     Valid notification actions include:
 *       NV906F_CTRL_SET_EVENT_NOTIFICATION_ACTION_DISABLE
 *         This action disables event notification for the specified
 *         event for the associated channel object.
 *       NV906F_CTRL_SET_EVENT_NOTIFICATION_ACTION_SINGLE
 *         This action enables single-shot event notification for the
 *         specified event for the associated channel object.
 *       NV906F_CTRL_SET_EVENT_NOTIFICATION_ACTION_REPEAT
 *         This action enables repeated event notification for the specified
 *         event for the associated channel object.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV906F_CTRL_CMD_EVENT_SET_NOTIFICATION (0x906f0203) /* finn: Evaluated from "(FINN_GF100_CHANNEL_GPFIFO_EVENT_INTERFACE_ID << 8) | NV906F_CTRL_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID" */

#define NV906F_CTRL_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV906F_CTRL_EVENT_SET_NOTIFICATION_PARAMS {
    NvU32 event;
    NvU32 action;
} NV906F_CTRL_EVENT_SET_NOTIFICATION_PARAMS;

/* valid action values */
#define NV906F_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE (0x00000000)
#define NV906F_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE  (0x00000001)
#define NV906F_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT  (0x00000002)

/*
 * NV906F_CTRL_CMD_GET_DEFER_RC_STATE
 *
 * If SM Debugger is attached then on a MMU fault, RM defers the RC error
 * recovery and keeps a flag indicating that RC is deferred. This command
 * checks whether or not deferred RC is pending in RM for the associated
 * channel.
 *
 *   bDeferRCPending
 *     The output are TRUE and FALSE.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV906F_CTRL_CMD_GET_DEFER_RC_STATE (0x906f0105) /* finn: Evaluated from "(FINN_GF100_CHANNEL_GPFIFO_GPFIFO_INTERFACE_ID << 8) | NV906F_CTRL_CMD_GET_DEFER_RC_STATE_PARAMS_MESSAGE_ID" */

#define NV906F_CTRL_CMD_GET_DEFER_RC_STATE_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV906F_CTRL_CMD_GET_DEFER_RC_STATE_PARAMS {
    NvBool bDeferRCPending;
} NV906F_CTRL_CMD_GET_DEFER_RC_STATE_PARAMS;

#define NV906F_CTRL_CMD_GET_MMU_FAULT_INFO                         (0x906f0106) /* finn: Evaluated from "(FINN_GF100_CHANNEL_GPFIFO_GPFIFO_INTERFACE_ID << 8) | NV906F_CTRL_GET_MMU_FAULT_INFO_PARAMS_MESSAGE_ID" */

/*
 * Shader types supported by MMU fault info
 * The types before compute shader refer to NV9097_SET_PIPELINE_SHADER_TYPE
 */
#define NV906F_CTRL_MMU_FAULT_SHADER_TYPE_VERTEX_CULL_BEFORE_FETCH 0x00000000
#define NV906F_CTRL_MMU_FAULT_SHADER_TYPE_VERTEX                   0x00000001
#define NV906F_CTRL_MMU_FAULT_SHADER_TYPE_TESSELLATION_INIT        0x00000002
#define NV906F_CTRL_MMU_FAULT_SHADER_TYPE_TESSELLATION             0x00000003
#define NV906F_CTRL_MMU_FAULT_SHADER_TYPE_GEOMETRY                 0x00000004
#define NV906F_CTRL_MMU_FAULT_SHADER_TYPE_PIXEL                    0x00000005
#define NV906F_CTRL_MMU_FAULT_SHADER_TYPE_COMPUTE                  0x00000006
#define NV906F_CTRL_MMU_FAULT_SHADER_TYPES                         7

/*
 * NV906F_CTRL_CMD_GET_MMU_FAULT_INFO
 *
 * This command returns MMU fault information for a given channel. The MMU
 * fault information will be cleared once this command is executed.
 *
 *   addrHi - [out]
 *      Upper 32 bits of faulting address
 *   addrLo [out]
 *      Lower 32 bits of faulting address
 *   faultType [out]
 *      MMU fault type. Please see NV_PFIFO_INTR_MMU_FAULT_INFO_TYPE_* in
 *      dev_fifo.h for details about MMU fault type.
 *   faultString [out]
 *      String indicating the MMU fault type
 *   shaderProgramVA [out]
 *      an array of shader program virtual addresses to indicate faulted shaders in the pipeline
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV906F_CTRL_MMU_FAULT_STRING_LEN                           32
#define NV906F_CTRL_GET_MMU_FAULT_INFO_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV906F_CTRL_GET_MMU_FAULT_INFO_PARAMS {
    NvU32 addrHi;
    NvU32 addrLo;
    NvU32 faultType;
    char  faultString[NV906F_CTRL_MMU_FAULT_STRING_LEN];
    NV_DECLARE_ALIGNED(NvU64 shaderProgramVA[NV906F_CTRL_MMU_FAULT_SHADER_TYPES], 8);
} NV906F_CTRL_GET_MMU_FAULT_INFO_PARAMS;


/* _ctrl906f.h_ */
