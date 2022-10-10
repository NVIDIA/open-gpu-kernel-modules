/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl2080/ctrl2080unix.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"

/* NV20_SUBDEVICE_XX OS control commands and parameters */

/*
 * NV2080_CTRL_CMD_OS_UNIX_GC6_BLOCKER_REFCNT
 *
 * This command increases or decreases the value of the per-GPU GC6 blocker
 * refCount used by Linux kernel clients to prevent the GPU from entering GC6.
 *
 * When the refCount is non-zero, the GPU cannot enter GC6. When the refCount
 * transitions from zero to non-zero as a result of this command, the GPU will
 * automatically come out of GC6.
 *
 * action   Whether to increment or decrement the value of the refCount.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_OS_UNIX_GC6_BLOCKER_REFCNT (0x20803d01) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_OS_UNIX_INTERFACE_ID << 8) | NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS {
    NvU32 action;
} NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS;

// Possible values for action
#define NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_INC   (0x00000001)
#define NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_DEC   (0x00000002)

/*
 * NV2080_CTRL_CMD_OS_UNIX_ALLOW_DISALLOW_GCOFF
 *
 * RM by default allows GCOFF but when the X driver disallows to enter in GCOFF
 * then this rmcontrol sets flag as NV_FALSE and if it allows to enter in GCOFF
 * then the flag is set as NV_TRUE.
 *
 * action   Whether to allow or disallow the user mode clients to enter in GCOFF.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_OS_UNIX_ALLOW_DISALLOW_GCOFF (0x20803d02) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_OS_UNIX_INTERFACE_ID << 8) | NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS {
    NvU32 action;
} NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS;

// Possible values for action
#define NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_ALLOW    (0x00000001)
#define NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_DISALLOW (0x00000002)

/*
 * NV2080_CTRL_CMD_OS_UNIX_AUDIO_DYNAMIC_POWER
 *
 * GPU can have integrated HDA (High Definition Audio) controller which
 * can be in active or suspended state during dynamic power management.
 * This command will perform HDA controller wakeup (if bEnter is false) or
 * suspend (if bEnter is true).
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_OS_UNIX_AUDIO_DYNAMIC_POWER       (0x20803d03) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_OS_UNIX_INTERFACE_ID << 8) | NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS {
    NvBool bEnter;
} NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS;

/*
 * NV2080_CTRL_CMD_OS_UNIX_INSTALL_PROFILER_HOOKS
 *
 * Initialize cyclestats HWPM support in the kernel. This will set up a callback
 * event for the channel indicated by hNotifierResource. This callback will execute
 * perf register read / write commands enqueued in the shared buffer indicated by
 * hNotifyDataMemory. Only one client may use HWPM functionality at a time.
 *
 * Additionally, if perfmonIdCount is greater than zero, mode-e HWPM streaming into
 * the buffer indicated by hSnapshotMemory will be initialized (but not turned on).
 * Data will be copied into the provided buffer every 10ms, or whenever a
 * NV2080_CTRL_CMD_OS_UNIX_FLUSH_SNAPSHOT_BUFFER command is issued.
 */
#define NV2080_CTRL_CMD_OS_UNIX_INSTALL_PROFILER_HOOKS (0x20803d04) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_OS_UNIX_INTERFACE_ID << 8) | NV2080_CTRL_OS_UNIX_INSTALL_PROFILER_HOOKS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_OS_UNIX_INSTALL_PROFILER_HOOKS_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV2080_CTRL_OS_UNIX_INSTALL_PROFILER_HOOKS_PARAMS {
    NvHandle hNotifierResource;
    NvU32    notifyDataSize;
    NvHandle hNotifyDataMemory;
    NvU32    perfmonIdCount;
    NvU32    snapshotBufferSize;
    NvHandle hSnapshotMemory;
} NV2080_CTRL_OS_UNIX_INSTALL_PROFILER_HOOKS_PARAMS;

/*
 * NV2080_CTRL_CMD_OS_UNIX_FLUSH_SNAPSHOT_BUFFER
 *
 * Immediately copies any pending mode-e HWPM data into the previously
 * installed snapshot buffer instead of waiting for the timer.
 */
#define NV2080_CTRL_CMD_OS_UNIX_FLUSH_SNAPSHOT_BUFFER     (0x20803d05) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_OS_UNIX_INTERFACE_ID << 8) | 0x5" */

/*
 * NV2080_CTRL_CMD_OS_UNIX_STOP_PROFILER
 *
 * Stop the timer responsible for copying mode-e HWPM data to the snapshot buffer.
 * The snapshot buffer must not be freed by the client before this command is issued.
 */
#define NV2080_CTRL_CMD_OS_UNIX_STOP_PROFILER             (0x20803d06) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_OS_UNIX_INTERFACE_ID << 8) | 0x6" */

/*
 * NV2080_CTRL_CMD_OS_UNIX_VIDMEM_PERSISTENCE_STATUS
 *
 * This command will be used by clients to check if the GPU video memory will
 * be persistent during system suspend/resume cycle.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_OS_UNIX_VIDMEM_PERSISTENCE_STATUS (0x20803d07) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_OS_UNIX_INTERFACE_ID << 8) | NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS {
    NvBool bVidmemPersistent;
} NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS;

/*
 * NV2080_CTRL_CMD_OS_UNIX_UPDATE_TGP_STATUS
 *
 * This command will be used by clients to set restore TGP flag which will
 * help to restore TGP limits when clients are destroyed.
 *
 * Possible status values returned are:
 * NV_OK
 */
#define NV2080_CTRL_CMD_OS_UNIX_UPDATE_TGP_STATUS (0x20803d08) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_OS_UNIX_INTERFACE_ID << 8) | NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS {
    NvBool bUpdateTGP;
} NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS;
/* _ctrl2080unix_h_ */
