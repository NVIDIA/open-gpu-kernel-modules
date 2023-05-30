/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl90cc/ctrl90cchwpm.finn
//

#include "ctrl/ctrl90cc/ctrl90ccbase.h"

/* GF100_PROFILER HWPM control commands and parameters */

/*
 * NV90CC_CTRL_CMD_HWPM_RESERVE
 *
 * This command attempts to reserve the perfmon for use by the calling client.
 * If this object was allocated as a child of a subdevice, then the
 * reservation will be global among all contexts on that subdevice. If this
 * object was allocated as a child of a channel group or a channel, then the
 * reservation will only be for the hardware context of that channel group or
 * channel.
 *
 * If the global reservation is held on a subdevice by another client, then
 * this command will fail, regardless of the parent class.
 *
 * If one or more per-context reservations are held by other clients, then
 * this command will fail if the parent object is a subdevice or another
 * client already holds the perfmon reservation for the parent context.
 *
 * This command will return NV_ERR_STATE_IN_USE for all of the failure
 * cases described above. A return status of NV_OK guarantees
 * that the client holds the perfmon reservation of the appropriate scope.
 *
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_STATE_IN_USE
 */
#define NV90CC_CTRL_CMD_HWPM_RESERVE              (0x90cc0101) /* finn: Evaluated from "(FINN_GF100_PROFILER_HWPM_INTERFACE_ID << 8) | 0x1" */

/*
 * NV90CC_CTRL_CMD_HWPM_RELEASE
 *
 * This command releases an existing reservation of the perfmon for the
 * calling client. If the calling client does not currently have the perfmon
 * reservation as acquired by NV90CC_CTRL_CMD_PROFILER_RESERVE_HWPM, this
 * command will return NV_ERR_INVALID_REQUEST.
 *
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_REQUEST
 */
#define NV90CC_CTRL_CMD_HWPM_RELEASE              (0x90cc0102) /* finn: Evaluated from "(FINN_GF100_PROFILER_HWPM_INTERFACE_ID << 8) | 0x2" */

/*
 * NV90CC_CTRL_CMD_HWPM_GET_RESERVATION_INFO
 *
 * This command returns information about any outstanding perfmon
 * reservations. If this object was allocated as a child of a subdevice, then
 * this command will return information about all reservations on the
 * subdevice (global or per-context). If this object was allocated as a child
 * of a channel group or channel, then this command will only return
 * information about the per-context reservation for that context or the
 * global reservation, if one exists.
 *
 *   reservationCount
 *     This parameter returns the number of outstanding perfmon reservations
 *     in the applicable scope. If the value of the bGlobal parameter is
 *     NV_TRUE, then this parameter will have a value of 1. If this object was
 *     allocated as a child of a channel group or channel, then this parameter
 *     will have a value of either 0 or 1. If this object was allocated as a
 *     child of a subdevice and the bGlobal parameter is NV_FALSE, then this
 *     parameter will return the number of per-context reservations on the
 *     subdevice.
 *   pid
 *     This parameter returns the PID of a process that holds a reservation in
 *     the applicable scope. If the value of the bGlobal parameter is NV_TRUE,
 *     then this parameter will be the PID of the process holding the global
 *     perfmon reservation on the subdevice. Otherwise, if the value of
 *     reservationCount is greater than 0, the value of this parameter will be
 *     the PID of one of the process that holds the per-context lock in the
 *     applicable scope. If the value of the reservationCount parameter is 0,
 *     the value of this parameter is undefined.
 *   bGlobal
 *     This parameter returns whether the outstanding perfmon reservation held
 *     by any client is global or per-context. If the value of this parameter
 *     is NV_TRUE, then the value of the reservationCount parameter should be 1
 *     and the value of the pid parameter should be the pid of the process
 *     that holds the global perfmon reservation. The value of this parameter
 *     will be NV_FALSE when there is no global perfmon reservation.
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 */
#define NV90CC_CTRL_CMD_HWPM_GET_RESERVATION_INFO (0x90cc0103) /* finn: Evaluated from "(FINN_GF100_PROFILER_HWPM_INTERFACE_ID << 8) | NV90CC_CTRL_HWPM_GET_RESERVATION_INFO_PARAMS_MESSAGE_ID" */

#define NV90CC_CTRL_HWPM_GET_RESERVATION_INFO_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV90CC_CTRL_HWPM_GET_RESERVATION_INFO_PARAMS {
    NvU32  reservationCount;
    NvU32  pid;
    NvBool bGlobal;
} NV90CC_CTRL_HWPM_GET_RESERVATION_INFO_PARAMS;



/* _ctrl90cchwpm_h_ */
