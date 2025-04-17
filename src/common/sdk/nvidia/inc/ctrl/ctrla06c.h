/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrla06c.finn
//




/*
 * KEPLER_CHANNEL_GROUP_A control commands and parameters
 *
 * A channel group is a collection of channels which are executed in a specific
 * order by hardware.  Hardware will not move from one channel in the group
 * until it is finished executing all its work or gives up control to the next
 * channel.
 *
 * Channels are added to the group by allocating them as a child of the
 * KEPLER_CHANNEL_GROUP_A object.
 *
 * A channel in a channel group may be restricted with respect to what objects
 * can be allocated on it and what APIs are allowed to operate on the channel.
 * For more details on specifics of how groups behave see the Software and
 * Hardware documetation for the specific architecture in which you are
 * interested.
 *
 */

#include "ctrl/ctrlxxxx.h"
#include "ctrl/ctrla06f/ctrla06fgpfifo.h"   /* A06C is partially derived from A06F */

#include "ctrl/ctrl2080/ctrl2080internal.h" /* For NV2080_CTRL_INTERNAL_MEMDESC_INFO */
#include "ctrl/ctrlc36f.h"                  /* For NVC36F_CTRL_CMD_GPFIFO_FAULT_METHOD_BUFFER_MAX_RUNQUEUES */

#define NVA06C_CTRL_CMD(cat,idx)  \
    NVXXXX_CTRL_CMD(0xA06C, NVA06C_CTRL_##cat, idx)

/* KEPLER_CHANNEL_GROUP_A command categories (6bits) */
#define NVA06C_CTRL_RESERVED (0x00)
#define NVA06C_CTRL_GPFIFO   (0x01)
#define NVA06C_CTRL_INTERNAL (0x02)

/*
 * NVA06C_CTRL_CMD_NULL
 *
 * Please see description of NV906D_CTRL_CMD_NULL.
 *
 */
#define NVA06C_CTRL_CMD_NULL (0xa06c0000) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GROUP_A_RESERVED_INTERFACE_ID << 8) | 0x0" */





/*
 * NVA06C_CTRL_CMD_GPFIFO_SCHEDULE
 *
 * This command schedules a channel group in hardware. This command should be
 * called after objects have been allocated on the channel or a call to
 * NVA06C_CTRL_CMD_BIND has been made.
 *
 * See NVA06F_CTRL_CMD_GPFIFO_SCHEDULE for parameter definitions.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_OPERATION
 *
 */
#define NVA06C_CTRL_CMD_GPFIFO_SCHEDULE (0xa06c0101) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GROUP_A_GPFIFO_INTERFACE_ID << 8) | NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS_MESSAGE_ID" */

#define NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS_MESSAGE_ID (0x1U)

typedef NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS;

/*
 * NVA06C_CTRL_CMD_BIND
 *
 * This command uses the given engine to configure the group for scheduling.
 * It alleviates the need to call NVA06C_CTRL_CMD_GPFIFO_SCHEDULE after objects
 * have been allocated.  However, it requires that the caller know which engine
 * they want to be able to execute in the group.  Once this has been called
 * only objects that can be allocated on the specified engine or other engines
 * allowed to coexist in the group will be allowed.  See
 * NV2080_CTRL_CMD_GPU_GET_ENGINE_PARTNERLIST to determine which engines can
 * share a parent.
 *
 * See NVA06F_CTRL_CMD_BIND for parameter definitions.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NVA06C_CTRL_CMD_BIND (0xa06c0102) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GROUP_A_GPFIFO_INTERFACE_ID << 8) | NVA06C_CTRL_BIND_PARAMS_MESSAGE_ID" */

#define NVA06C_CTRL_BIND_PARAMS_MESSAGE_ID (0x2U)

typedef NVA06F_CTRL_BIND_PARAMS NVA06C_CTRL_BIND_PARAMS;

/*
 * NVA06C_CTRL_CMD_SET_TIMESLICE
 *
 * This command modifies the timeslice for a channel group.  Hardware may not
 * support all possible values, so the input will be rounded down to a valid
 * hardware value.
 *
 * This takes effect immediately, meaning the channel may be forced off of the
 * GPU so the change can be made.
 *
 * timesliceUs
 *   This parameter is an input containing the desired timeslice in microseconds
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NVA06C_CTRL_CMD_SET_TIMESLICE (0xa06c0103) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GROUP_A_GPFIFO_INTERFACE_ID << 8) | NVA06C_CTRL_SET_TIMESLICE_PARAMS_MESSAGE_ID" */

typedef struct NVA06C_CTRL_TIMESLICE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 timesliceUs, 8);
} NVA06C_CTRL_TIMESLICE_PARAMS;

#define NVA06C_CTRL_SET_TIMESLICE_PARAMS_MESSAGE_ID (0x3U)

typedef NVA06C_CTRL_TIMESLICE_PARAMS NVA06C_CTRL_SET_TIMESLICE_PARAMS;

/*
 * NVA06C_CTRL_CMD_GET_TIMESLICE
 *
 * This command returns the timeslice for a channel group.  If
 * NVA06C_CTRL_CMD_SET_TIMESLICE has been used this call will return the exact
 * value passed to that function, which may not be exactly what is programmed
 * to hardware (see NVA06C_CTRL_CMD_SET_TIMESLICE for more details).
 *
 * timesliceUs
 *   This parameter is an output containing current timeslice in microseconds.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NVA06C_CTRL_CMD_GET_TIMESLICE (0xa06c0104) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GROUP_A_GPFIFO_INTERFACE_ID << 8) | NVA06C_CTRL_GET_TIMESLICE_PARAMS_MESSAGE_ID" */

#define NVA06C_CTRL_GET_TIMESLICE_PARAMS_MESSAGE_ID (0x4U)

typedef NVA06C_CTRL_TIMESLICE_PARAMS NVA06C_CTRL_GET_TIMESLICE_PARAMS;

/*
 * NVA06C_CTRL_CMD_PREEMPT
 *
 * This command preempts a channel group. It optionally waits for the preempt to
 * complete before returning.
 *
 * bWait
 *   If bWait evaluates to NV_TRUE then this control call waits till the preempt
 *   completes, else it returns immediately after issuing the preempt.  Calling
 *   this control call multiple times with bWait = NV_FALSE without waiting for
 *   the preempt to complete by some other means can lead to undefined results.
 * bManualTimeout
 *   If bManualTimer evaluates to NV_TRUE then timeoutUs will be used as the
 *   timeout value.
 * timeoutUs
 *   The desired timeout in microseconds. Only used if bManualTimeout evaluates
 *   to NV_TRUE. The maximum value of timeoutUs is
 *   NVA06C_CTRL_CMD_PREEMPT_MAX_MANUAL_TIMEOUT_US.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 *
 */
#define NVA06C_CTRL_CMD_PREEMPT (0xa06c0105) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GROUP_A_GPFIFO_INTERFACE_ID << 8) | NVA06C_CTRL_PREEMPT_PARAMS_MESSAGE_ID" */

#define NVA06C_CTRL_PREEMPT_PARAMS_MESSAGE_ID (0x5U)

typedef struct NVA06C_CTRL_PREEMPT_PARAMS {
    NvBool bWait;
    NvBool bManualTimeout;
    NvU32  timeoutUs;
} NVA06C_CTRL_PREEMPT_PARAMS;

#define NVA06C_CTRL_CMD_PREEMPT_MAX_MANUAL_TIMEOUT_US (1000000) // 1 second

/*
 * NVA06C_CTRL_CMD_GET_INFO
 *
 * This command returns information about the channel group.
 *
 * tsgID
 *   Output parameter containing the hardware TSG ID for this channel group.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_OBJECT
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NVA06C_CTRL_CMD_GET_INFO                      (0xa06c0106) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GROUP_A_GPFIFO_INTERFACE_ID << 8) | NVA06C_CTRL_GET_INFO_PARAMS_MESSAGE_ID" */

#define NVA06C_CTRL_GET_INFO_PARAMS_MESSAGE_ID (0x6U)

typedef struct NVA06C_CTRL_GET_INFO_PARAMS {
    NvU32 tsgID;
} NVA06C_CTRL_GET_INFO_PARAMS;

/*
 * NVA06C_CTRL_CMD_SET_INTERLEAVE_LEVEL
 *
 * Used to change the target TSG's interleave level in the runlist. The
 * interleave level indicates how often channels in the TSG appear in the
 * runlist according to the following policy:
 *
 *       - LOW:    Appear once
 *       - MEDIUM: If L > 0, appear L times
 *                 Else, appear once
 *       - HIGH:   If L > 0, appear (M + 1) * L times
 *                 Else if M > 0, appear M times
 *                 Else, appear once
 *  where
 *       - L = # of LOW interleave level TSGs
 *       - M = # of MEDIUM interleave level TSGs
 *
 * For safety reasons, setting this property requires PRIVILEGED user level.
 *
 * tsgInterleaveLevel
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
#define NVA06C_CTRL_CMD_SET_INTERLEAVE_LEVEL (0xa06c0107) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GROUP_A_GPFIFO_INTERFACE_ID << 8) | NVA06C_CTRL_SET_INTERLEAVE_LEVEL_PARAMS_MESSAGE_ID" */

typedef struct NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS {
    NvU32 tsgInterleaveLevel;
} NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS;

#define NVA06C_CTRL_INTERLEAVE_LEVEL_LOW    (0x00000000)
#define NVA06C_CTRL_INTERLEAVE_LEVEL_MEDIUM (0x00000001)
#define NVA06C_CTRL_INTERLEAVE_LEVEL_HIGH   (0x00000002)

#define NVA06C_CTRL_SET_INTERLEAVE_LEVEL_PARAMS_MESSAGE_ID (0x7U)

typedef NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS NVA06C_CTRL_SET_INTERLEAVE_LEVEL_PARAMS;

/*
 * NVA06C_CTRL_CMD_GET_INTERLEAVE_LEVEL
 *
 * Returns the target TSG's interleave level.
 *
 * tsgInterleaveLevel
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
#define NVA06C_CTRL_CMD_GET_INTERLEAVE_LEVEL (0xa06c0108) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GROUP_A_GPFIFO_INTERFACE_ID << 8) | NVA06C_CTRL_GET_INTERLEAVE_LEVEL_PARAMS_MESSAGE_ID" */

#define NVA06C_CTRL_GET_INTERLEAVE_LEVEL_PARAMS_MESSAGE_ID (0x8U)

typedef NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS NVA06C_CTRL_GET_INTERLEAVE_LEVEL_PARAMS;

/*
 * NVA06C_CTRL_CMD_PROGRAM_VIDMEM_PROMOTE
 * This ctrl call is deprecated in favor of NV0090_CTRL_CMD_PROGRAM_VIDMEM_PROMOTE.
 * @see NV0090_CTRL_CMD_PROGRAM_VIDMEM_PROMOTE
 */
#define NVA06C_CTRL_CMD_PROGRAM_VIDMEM_PROMOTE (0xa06c0109) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GROUP_A_GPFIFO_INTERFACE_ID << 8) | NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS_MESSAGE_ID" */

typedef enum NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_SIZE {
    NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_SIZE_NONE = 0,
    NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_SIZE_64B = 1,
    NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_SIZE_128B = 2,
} NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_SIZE;

typedef struct NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_FIELD {
    NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_SIZE size;
} NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_FIELD;

#define NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS_MESSAGE_ID (0x9U)

typedef struct NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS {
    NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_FIELD l1;
    NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_FIELD t1;
} NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS;

/*!
 * NVA06C_CTRL_CMD_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS
 *
 * This control call is used to
 * - set the fault method buffer addresses in vGpu host / GSP after the fault
 *   method buffers are allocated by vGpu guest / GSP_CLIENT during channel
 *   group allocation
 * - destroy the method buffer memory descriptors during channel group free
 *
 * methodBufferMemdesc [input]
 *   Details about the memory allocated for method buffer for each runqueue.
 *   If the size of the memory region is zero, the descriptor will be destroyed.
 *
 * bar2Addr [input]
 *   CPU invisible BAR2 address that the method buffer is mapped to for each
 *   runqueue
 *
 * numValidEntries [input]
 *   The number of valid entries upto
 *   NVC36F_CTRL_CMD_GPFIFO_FAULT_METHOD_BUFFER_MAX_RUNQUEUES actually filled in
 *   the control call parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NVA06C_CTRL_CMD_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS           (0xa06c010a) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GROUP_A_GPFIFO_INTERFACE_ID << 8 | NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS_MESSAGE_ID)" */

#define NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_MAX_RUNQUEUES (NVC36F_CTRL_CMD_GPFIFO_FAULT_METHOD_BUFFER_MAX_RUNQUEUES)

#define NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS_MESSAGE_ID (0xaU)

typedef struct NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_MEMDESC_INFO methodBufferMemdesc[NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_MAX_RUNQUEUES], 8);
    NV_DECLARE_ALIGNED(NvU64 bar2Addr[NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_MAX_RUNQUEUES], 8);
    NvU32 numValidEntries;
} NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS;

/*
 * NVA06C_CTRL_CMD_MAKE_REALTIME
 *
 * Promote the specified TSG to realtime, or demote to non-realtime,
 * depending on the given bRealtime parameter. A realtime TSG will have the
 * highest interleave level when the scheduling policy is CHANNEL_INTERLEAVED,
 * and will also precede any non-realtime channel/TSG in the order channels are
 * added to the corresponding runlist.
 *
 * Whenever a realtime TSG is added to a runlist, all non-realtime channels/TSGs
 * are made preemptible by setting a COMPUTE preemption mode to CTA.
 *
 * Preemption modes for realtime TSGs remain WFI.
 *
 * A client must then issue a NVA06F_CTRL_CMD_RESTART_RUNLIST control call when
 * kicking off work on a realtime TSG to preempt non-realtime channels and
 * restart the runlist such that the realtime TSG will start executing next.
 *
 *   bRealtime
 *     Whether the TSG is to be promoted to/demoted from realtime.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_PARAMETER
 *   NV_ERR_INVALID_OBJECT
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_NOT_SUPPORTED
 */
#define NVA06C_CTRL_CMD_MAKE_REALTIME (0xa06c0110) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GROUP_A_GPFIFO_INTERFACE_ID << 8) | NVA06C_CTRL_MAKE_REALTIME_PARAMS_MESSAGE_ID" */

#define NVA06C_CTRL_MAKE_REALTIME_PARAMS_MESSAGE_ID (0x10U)

typedef struct NVA06C_CTRL_MAKE_REALTIME_PARAMS {
    NvBool bRealtime;
} NVA06C_CTRL_MAKE_REALTIME_PARAMS;





/*
 * NVA06C_CTRL_CMD_INTERNAL_GPFIFO_SCHEDULE
 *
 * This command is an internal command sent from Kernel RM to Physical RM
 * to schedule a channel group in hardware.
 *
 * Please see description of NVA06C_CTRL_CMD_GPFIFO_SCHEDULE for more information.
 *
 */
#define NVA06C_CTRL_CMD_INTERNAL_GPFIFO_SCHEDULE (0xa06c0201) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GROUP_A_INTERNAL_INTERFACE_ID << 8) | NVA06C_CTRL_INTERNAL_GPFIFO_SCHEDULE_PARAMS_MESSAGE_ID" */

#define NVA06C_CTRL_INTERNAL_GPFIFO_SCHEDULE_PARAMS_MESSAGE_ID (0x1U)

typedef NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS NVA06C_CTRL_INTERNAL_GPFIFO_SCHEDULE_PARAMS;

/* 
 * NVA06C_CTRL_CMD_INTERNAL_SET_TIMESLICE
 *
 * This command is an internal command sent from Kernel RM to Physical RM
 * to modify the timeslice for a channel group in hardware.
 *
 * Please see description of NVA06C_CTRL_CMD_SET_TIMESLICE for more information.
 *
 */
#define NVA06C_CTRL_CMD_INTERNAL_SET_TIMESLICE (0xa06c0202) /* finn: Evaluated from "(FINN_KEPLER_CHANNEL_GROUP_A_INTERNAL_INTERFACE_ID << 8) | NVA06C_CTRL_INTERNAL_SET_TIMESLICE_PARAMS_MESSAGE_ID" */

#define NVA06C_CTRL_INTERNAL_SET_TIMESLICE_PARAMS_MESSAGE_ID (0x2U)

typedef NVA06C_CTRL_TIMESLICE_PARAMS NVA06C_CTRL_INTERNAL_SET_TIMESLICE_PARAMS;

/* _ctrla06c.h_ */

