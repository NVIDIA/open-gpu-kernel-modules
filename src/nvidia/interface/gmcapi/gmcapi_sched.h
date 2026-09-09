/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GMCAPI_SCHED_H
#define GMCAPI_SCHED_H

/*
 * Ephemeral state keys for GMCAPI_CMD_SCHED_CONTROL
 *
 * These keys set state that persists across subsequent commands within a single
 * GMCAPI_CMD_SCHED_CONTROL call.  The index field of these keys must be zero.
 * The default values are reset at the beginning of each GMCAPI_CMD_SCHED_CONTROL
 * call.  All defaults are zero except NVGMC_SCHED_SET_TIMEOUT_US which defaults
 * to an internal default value.
 *
 */

/* 32-bit state keys */
#define NVGMC_SCHED_SET_RUNLIST_ID                                      0x0002
#define NVGMC_SCHED_SET_TSG_ID                                          0x0003

#define NVGMC_SCHED_SET_CASID                                           0x0004

#define NVGMC_SCHED_SET_INST_MEM_TARGET                                 0x0005
#define NVGMC_SCHED_SET_INST_MEM_TARGET_APERTURE                           7:0
#define NVGMC_SCHED_SET_INST_MEM_TARGET_APERTURE_VID_MEM                0x0000
#define NVGMC_SCHED_SET_INST_MEM_TARGET_APERTURE_SYS_MEM_COHERENT       0x0002
#define NVGMC_SCHED_SET_INST_MEM_TARGET_APERTURE_SYS_MEM_NON_COHERENT   0x0003

#define NVGMC_SCHED_SET_USERD_MEM_TARGET                                0x0006
#define NVGMC_SCHED_SET_USERD_MEM_TARGET_APERTURE                          7:0
#define NVGMC_SCHED_SET_USERD_MEM_TARGET_APERTURE_VID_MEM               0x0000
#define NVGMC_SCHED_SET_USERD_MEM_TARGET_APERTURE_SYS_MEM_COHERENT      0x0002
#define NVGMC_SCHED_SET_USERD_MEM_TARGET_APERTURE_SYS_MEM_NON_COHERENT  0x0003

#define NVGMC_SCHED_SET_VEID                                            0x0007
#define NVGMC_SCHED_SET_RUNQUEUE_ID                                     0x0008

/*
 * NVGMC_SCHED_SET_TIMEOUT_US
 *
 * Overrides the timeout value (in microseconds) used by subsequent commands
 * that perform HW waits. The default value is initialized from
 * an internal default value before each GMC_CMD_SCHED_CONTROL call.
 *
 * Applicable commands:
 *   NVGMC_SCHED_TSG_PREEMPT
 *   NVGMC_SCHED_RUNLIST_PREEMPT
 *   NVGMC_SCHED_RUNLIST_RUN
 *
 */
#define NVGMC_SCHED_SET_TIMEOUT_US                                      0x0009

/* 64-bit state keys */
#define NVGMC_SCHED_SET_INST_MEM_ADDRESS                                0x1005
#define NVGMC_SCHED_SET_USERD_MEM_ADDRESS                               0x1006

/* "Command" keys for GMC_CMD_SCHED_CONTROL
 *
 * For commands that target a specific channel, runlist, or TSG, the key's
 * index field carries the ID of that object.  The payload is reserved for
 * command options (if any).  Commands with no defined options must have their
 * payload set to 0.
 *
 * Values are ordered to roughly follow the expected bringup/teardown lifecycle:
 * bringup:  BIND -> ENABLE -> ADD -> COMMIT
 * teardown: DISABLE -> REMOVE -> COMMIT -> UNBIND
 *
 * All reserved fields for commands must be set to 0.
 *
 */

/*
 * NVGMC_SCHED_CHANNEL_BIND
 *
 * Writes the instance block binding into channel RAM for the given channel.
 * This must be called before CHANNEL_ENABLE or CHANNEL_ADD.
 *
 * Key index: channel ID
 *
 * This command uses the following state keys:
 *
 * NVGMC_SCHED_SET_RUNLIST_ID
 * NVGMC_SCHED_SET_INST_MEM_ADDRESS
 * NVGMC_SCHED_SET_INST_MEM_TARGET
 *
 */
#define NVGMC_SCHED_CHANNEL_BIND                                        0x0101
#define NVGMC_SCHED_CHANNEL_BIND_RESERVED                                 31:0

/*
 * NVGMC_SCHED_CHANNEL_ENABLE
 *
 * Key index: channel ID
 *
 * This command is used to set the enable flag for a channel in channel RAM for
 * a given runlist.  This makes use of the following state keys:
 *
 * NVGMC_SCHED_SET_RUNLIST_ID
 *
 */
#define NVGMC_SCHED_CHANNEL_ENABLE                                      0x0102
#define NVGMC_SCHED_CHANNEL_ENABLE_RESERVED                               31:0

/*
 * NVGMC_SCHED_CHANNEL_ADD
 *
 * Key index: channel ID
 *
 * Uses the state set by the following keys to add a channel to a runlist:
 *
 * NVGMC_SCHED_SET_RUNLIST_ID
 * NVGMC_SCHED_SET_TSG_ID
 * NVGMC_SCHED_SET_CASID
 * NVGMC_SCHED_SET_VEID
 * NVGMC_SCHED_SET_RUNQUEUE_ID
 * NVGMC_SCHED_SET_INST_MEM_ADDRESS
 * NVGMC_SCHED_SET_USERD_MEM_ADDRESS
 * NVGMC_SCHED_SET_INST_MEM_TARGET
 * NVGMC_SCHED_SET_USERD_MEM_TARGET
 *
 */
#define NVGMC_SCHED_CHANNEL_ADD                                         0x0103
#define NVGMC_SCHED_CHANNEL_ADD_RESERVED                                  31:0

/*
 * NVGMC_SCHED_RUNLIST_COMMIT
 *
 * Key index: runlist ID
 *
 * This command will commit all prior channel add/remove operations and submit the
 * runlist to the hardware.
 *
 */
#define NVGMC_SCHED_RUNLIST_COMMIT                                      0x0104
#define NVGMC_SCHED_RUNLIST_COMMIT_RESERVED                               31:0

/*
 * NVGMC_SCHED_RUNLIST_ENGINE_SCHEDULING
 *
 * Key index: runlist ID
 *
 * This controls whether or not the HW scheduler will dispatch new work from
 * the current runlist to the engine(s) it controls.
 *
 * This does NOT prevent the runlist processing hardware from reading more
 * data from the runlist.  Note that waiting for this to complete does not
 * necessarily guarantee that the engine is fully idle.
 *
 */
#define NVGMC_SCHED_RUNLIST_ENGINE_SCHEDULING                           0x0105
#define NVGMC_SCHED_RUNLIST_ENGINE_SCHEDULING_ENABLE                       0:0
#define NVGMC_SCHED_RUNLIST_ENGINE_SCHEDULING_RESERVED                    31:1

/*
 * NVGMC_SCHED_RUNLIST_RUN
 *
 * Key index: runlist ID
 *
 * Controls whether the current runlist or an empty/NULL runlist is submitted to HW.
 * When RUN_ENABLE=1, the current runlist is submitted.
 * When RUN_ENABLE=0, an empty runlist is submitted, halting all execution.
 *
 * Optional state keys:
 *
 * NVGMC_SCHED_SET_TIMEOUT_US
 *
 */
#define NVGMC_SCHED_RUNLIST_RUN                                         0x0106
#define NVGMC_SCHED_RUNLIST_RUN_ENABLE                                     0:0
#define NVGMC_SCHED_RUNLIST_RUN_RESERVED                                  31:1

/*
 * NVGMC_SCHED_RUNLIST_PREEMPT
 *
 * Key index: runlist ID
 *
 * This command is used to preempt/restart a specific runlist.
 *
 * Optional state keys:
 *
 * NVGMC_SCHED_SET_TIMEOUT_US
 *
 */
#define NVGMC_SCHED_RUNLIST_PREEMPT                                     0x0107
#define NVGMC_SCHED_RUNLIST_PREEMPT_WAIT                                   0:0
#define NVGMC_SCHED_RUNLIST_PREEMPT_RESERVED                              31:1

/*
 * NVGMC_SCHED_TSG_PREEMPT
 *
 * Key index: TSG ID
 *
 * This command is used to preempt/restart a specific TSG within a runlist.
 * This makes use of the following state keys:
 *
 * NVGMC_SCHED_SET_RUNLIST_ID
 *
 * Optional state keys:
 *
 * NVGMC_SCHED_SET_TIMEOUT_US
 *
 */
#define NVGMC_SCHED_TSG_PREEMPT                                         0x0108
#define NVGMC_SCHED_TSG_PREEMPT_WAIT                                       0:0
#define NVGMC_SCHED_TSG_PREEMPT_RESERVED                                  31:1

/*
 * NVGMC_SCHED_CHANNEL_DISABLE
 *
 * Key index: channel ID
 *
 * This command is used to clear the enable flag for a channel in channel RAM for
 * a given runlist.
 *
 * Required state keys:
 *
 * NVGMC_SCHED_SET_RUNLIST_ID
 *
 */
#define NVGMC_SCHED_CHANNEL_DISABLE                                     0x0109
#define NVGMC_SCHED_CHANNEL_DISABLE_RESERVED                              31:0

/*
 * NVGMC_SCHED_CHANNEL_REMOVE
 *
 * Key index: channel ID
 *
 * This command is used to remove a channel from a runlist.
 *
 * Required state keys:
 *
 * NVGMC_SCHED_SET_RUNLIST_ID
 *
 */
#define NVGMC_SCHED_CHANNEL_REMOVE                                      0x010A
#define NVGMC_SCHED_CHANNEL_REMOVE_RESERVED                               31:0

/*
 * NVGMC_SCHED_CHANNEL_UNBIND
 *
 * Key index: channel ID
 *
 * Clears the instance block binding from channel RAM. This must be
 * done only after the channel has been removed from the runlist and
 * the runlist has been committed to HW.
 *
 * Required state keys:
 *
 * NVGMC_SCHED_SET_RUNLIST_ID
 *
 */
#define NVGMC_SCHED_CHANNEL_UNBIND                                      0x010B
#define NVGMC_SCHED_CHANNEL_UNBIND_RESERVED                               31:0

#endif /* GMCAPI_SCHED_H */
