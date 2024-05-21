/*
 * SPDX-FileCopyrightText: Copyright (c) 2007-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrla06f/ctrla06fgpfifo.finn
//

#include "ctrl/ctrla06f/ctrla06fbase.h"

/*
 * NVA06F_CTRL_CMD_GPFIFO_SCHEDULE
 *
 * This command schedules a channel in hardware. This command should be called
 * after objects have been allocated on the channel or a call to
 * NVA06F_CTRL_CMD_BIND has been made.
 *
 * bEnable
 *   This parameter indicates whether or not the channel should be scheduled in hardware.
 *   When set, the channel will be enabled in addition to being added to the appropriate runlist.
 *   When not set, the channel will be disabled and removed from runlist.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_OPERATION
 *
 */
#define NVA06F_CTRL_CMD_GPFIFO_SCHEDULE (0xa06f0103) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS_MESSAGE_ID" */
#define NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS_MESSAGE_ID (0x3U)

typedef struct NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS {
    NvBool bEnable;
    NvBool bSkipSubmit;
} NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS;

/*
 * NVA06F_CTRL_CMD_BIND
 *
 * This command uses the given engine to configure the channel for scheduling.
 * It alleviates the need to call NVA06F_CTRL_CMD_GPFIFO_SCHEDULE after objects
 * have been allocated.  However, it requires that the caller know which engine
 * they want to be able to execute on the channel.  Once this has been called
 * only objects that can be allocated on the specified engine or other engines
 * allowed to coexist on the channel will be allowed.  See
 * NV2080_CTRL_CMD_GPU_GET_ENGINE_PARTNERLIST to determine which engines can
 * share a parent.
 *
 * engineType
 *   This parameter specifies an NV2080_ENGINE_TYPE value indicating the
 *   engine to which this channel should be bound.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NVA06F_CTRL_CMD_BIND (0xa06f0104) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVA06F_CTRL_BIND_PARAMS_MESSAGE_ID" */

#define NVA06F_CTRL_BIND_PARAMS_MESSAGE_ID (0x4U)

typedef struct NVA06F_CTRL_BIND_PARAMS {
    NvU32 engineType;
} NVA06F_CTRL_BIND_PARAMS;

/*
 * NVA06F_CTRL_CMD_SET_ERROR_NOTIFIER
 *
 * This command sets the channel error notifier of the target channel.
 *   bNotifyEachChannelInTSG
 *     When true, the error notifier will be set on every channel in
 *     the TSG that contains the channel.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVA06F_CTRL_CMD_SET_ERROR_NOTIFIER (0xa06f0108) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVA06F_CTRL_SET_ERROR_NOTIFIER_PARAMS_MESSAGE_ID" */

#define NVA06F_CTRL_SET_ERROR_NOTIFIER_PARAMS_MESSAGE_ID (0x8U)

typedef struct NVA06F_CTRL_SET_ERROR_NOTIFIER_PARAMS {
    NvBool bNotifyEachChannelInTSG;
} NVA06F_CTRL_SET_ERROR_NOTIFIER_PARAMS;

/*
 * NVA06F_CTRL_CMD_SET_INTERLEAVE_LEVEL
 *
 * Symmetric to NVA06C_CTRL_CMD_SET_INTERLEAVE_LEVEL, applied to the individual
 * target channel.
 *
 * When belonging to a TSG, same interleave level will be set to every channel
 * in the TSG.
 *
 * channelInterleaveLevel
 *   Input parameter. One of:
 *       - NVA06C_CTRL_INTERLEAVE_LEVEL_LOW
 *       - NVA06C_CTRL_INTERLEAVE_LEVEL_MEDIUM
 *       - NVA06C_CTRL_INTERLEAVE_LEVEL_HIGH
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 */
#define NVA06F_CTRL_CMD_SET_INTERLEAVE_LEVEL (0xa06f0109) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVA06F_CTRL_SET_INTERLEAVE_LEVEL_PARAMS_MESSAGE_ID" */

typedef struct NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS {
    NvU32 channelInterleaveLevel;
} NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS;

#define NVA06F_CTRL_SET_INTERLEAVE_LEVEL_PARAMS_MESSAGE_ID (0x9U)

typedef NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS NVA06F_CTRL_SET_INTERLEAVE_LEVEL_PARAMS;

/*
 * NVA06F_CTRL_CMD_GET_INTERLEAVE_LEVEL
 *
 * Returns the target channel's interleave level.
 *
 * channelInterleaveLevel
 *   Output parameter. One of:
 *       - NVA06C_CTRL_INTERLEAVE_LEVEL_LOW
 *       - NVA06C_CTRL_INTERLEAVE_LEVEL_MEDIUM
 *       - NVA06C_CTRL_INTERLEAVE_LEVEL_HIGH
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NVA06F_CTRL_CMD_GET_INTERLEAVE_LEVEL (0xa06f0110) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVA06F_CTRL_GET_INTERLEAVE_LEVEL_PARAMS_MESSAGE_ID" */

#define NVA06F_CTRL_GET_INTERLEAVE_LEVEL_PARAMS_MESSAGE_ID (0x10U)

typedef NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS NVA06F_CTRL_GET_INTERLEAVE_LEVEL_PARAMS;

/*
 * NVA06F_CTRL_CMD_RESTART_RUNLIST
 *
 * This command expires the current timeslice and restarts the runlist the given
 * channel belongs to. This effectively preempts the current channel on the
 * corresponding engine.
 *
 * This is useful for clients to trigger preemption manually and reduce start
 * latency for higher priority channels as they are added first to the runlist
 * if NV0080_CTRL_FIFO_RUNLIST_SCHED_POLICY_CHANNEL_INTERLEAVED policy is
 * configured.
 *
 * This command interacts with the scheduler and may cause certain low priority
 * channels to starve under certain circumstances. Therefore, it is only
 * available to privileged clients.
 *
 *   bForceRestart
 *     Input parameter. If NV_FALSE, the runlist restart will be skipped
 *     whenever the given channel (or its group) is already running on the
 *     corresponding engine.
 *
 *   bBypassWait
 *     Input parameter. If NV_TRUE, the command will return immediately after
 *     issuing the hardware preemption request, without actually waiting for the
 *     context switch to complete.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NVA06F_CTRL_CMD_RESTART_RUNLIST (0xa06f0111) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVA06F_CTRL_RESTART_RUNLIST_PARAMS_MESSAGE_ID" */

#define NVA06F_CTRL_RESTART_RUNLIST_PARAMS_MESSAGE_ID (0x11U)

typedef struct NVA06F_CTRL_RESTART_RUNLIST_PARAMS {
    NvBool bForceRestart;
    NvBool bBypassWait;
} NVA06F_CTRL_RESTART_RUNLIST_PARAMS;

/*
 * NVA06F_CTRL_CMD_STOP_CHANNEL
 *
 * This command is used to stop the channel
 *
 * Stopping the channel here means disabling and unbinding the channel and removing it from runlist.
 * So, if the channel needs to run again, it has to be scheduled, bound and enabled again.
 * If we fail to preempt channel or remove it from runlist, then we RC the channel.
 * Also set an error notifier to notify user space that channel is stopped.
 *
 *   bImmediate
 *     Input parameter. If NV_FALSE, we will wait for default RM timeout
 *     for channel to idle. If NV_TRUE, we don't wait for channel to idle.
 *     If channel is not idle, we forcefully preempt it off the runlist.
 *     If the preempt times out, we will RC the channel.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NVA06F_CTRL_CMD_STOP_CHANNEL (0xa06f0112) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVA06F_CTRL_STOP_CHANNEL_PARAMS_MESSAGE_ID" */

#define NVA06F_CTRL_STOP_CHANNEL_PARAMS_MESSAGE_ID (0x12U)

typedef struct NVA06F_CTRL_STOP_CHANNEL_PARAMS {
    NvBool bImmediate;
} NVA06F_CTRL_STOP_CHANNEL_PARAMS;

/*
 * NVA06F_CTRL_CMD_GET_CONTEXT_ID
 * 
 * This command returns the context ID of a given channel.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVA06F_CTRL_CMD_GET_CONTEXT_ID (0xa06f0113) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVA06F_CTRL_GET_CONTEXT_ID_PARAMS_MESSAGE_ID" */

#define NVA06F_CTRL_GET_CONTEXT_ID_PARAMS_MESSAGE_ID (0x13U)

typedef struct NVA06F_CTRL_GET_CONTEXT_ID_PARAMS {
    NvU32 contextId;
} NVA06F_CTRL_GET_CONTEXT_ID_PARAMS;

/* _ctrla06fgpfifo_h_ */
