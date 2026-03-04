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
// Source file:      ctrl/ctrl9074.finn
//

#include "ctrl/ctrlxxxx.h"
#define NV9074_CTRL_CMD(cat,idx)  NVXXXX_CTRL_CMD(0x9074, NV9074_CTRL_##cat, idx)

/* Command categories (6bits) */
#define NV9074_CTRL_RESERVED (0x00)
#define NV9074_CTRL_SEM      (0x01)

/*
 * NV9074_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV9074_CTRL_CMD_NULL (0x90740000) /* finn: Evaluated from "(FINN_GF100_TIMED_SEMAPHORE_SW_RESERVED_INTERFACE_ID << 8) | 0x0" */





/*
 * NV9074_CTRL_CMD_FLUSH
 *
 * This command is intended to aid in idling/flushing a channel containing
 * methods invoked against a specific NV9074 object.
 *
 * This control manipulates an NV9074 object's  "flushing" state.
 *
 * Typical usage is:
 * NvRmControl(NV9074_CTRL_CMD_FLUSH, {TRUE, flushDelay});
 * NvRmIdleChannel(channel_containing_the_9074_object);
 * NvRmControl(NV9074_CTRL_CMD_FLUSH, {FALSE, 0});
 *
 * When an NV9074 object is placed into the flushing state, a snaphot of the
 * current timer value is taken, and "maxFlushTime" is added to this. This
 * value is the "flush limit timestamp". Any previously or newly invoked
 * NV9074_SEMAPHORE_SCHED methods and NV9074_CTRL_CMD_RELEASE requests that
 * specify a release timestamp at or after this "flush limit timestamp" will
 * immediately release the specified semaphore, without waiting for the
 * specified timestamp, and write a DONE_FORCED value to the specified notifier.
 */
#define NV9074_CTRL_CMD_FLUSH (0x90740101) /* finn: Evaluated from "(FINN_GF100_TIMED_SEMAPHORE_SW_SEM_INTERFACE_ID << 8) | NV9074_CTRL_CMD_FLUSH_PARAMS_MESSAGE_ID" */

#define NV9074_CTRL_CMD_FLUSH_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV9074_CTRL_CMD_FLUSH_PARAMS {
    NvU32 isFlushing;
    NV_DECLARE_ALIGNED(NvU64 maxFlushTime, 8);
} NV9074_CTRL_CMD_FLUSH_PARAMS;

/*
 * NV9074_CTRL_CMD_GET_TIME
 *
 * Retrieve the current time value.
 */
#define NV9074_CTRL_CMD_GET_TIME (0x90740102) /* finn: Evaluated from "(FINN_GF100_TIMED_SEMAPHORE_SW_SEM_INTERFACE_ID << 8) | NV9074_CTRL_CMD_GET_TIME_PARAMS_MESSAGE_ID" */

#define NV9074_CTRL_CMD_GET_TIME_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV9074_CTRL_CMD_GET_TIME_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 currentTime, 8);
} NV9074_CTRL_CMD_GET_TIME_PARAMS;

/*
 * NV9074_CTRL_CMD_RELEASE
 *
 * This command adds a timed semaphore release request. When the desired time is
 * reached, the semaphore is written with the release value, the notifier is
 * filled with status and timestamp, and optionally an event is sent to all the
 * client waiting on it.
 *
 *   notifierGPUVA
 *     This parameter specifies the GPU VA of the notifier to receive the status
 *     for this particular release.
 *
 *   semaphoreGPUVA
 *     This parameter specifies the GPU VA of the semaphore to release.
 *
 *   waitTimestamp
 *     This parameter specifies the timestamp at which to release the semaphore.
 *
 *   releaseValue
 *     This parameter specifies the semaphore value to release.
 *
 *   releaseFlags
 *     This parameter specifies the flags:
 *       _NOTIFY wake client or not.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ADDRESS
 *   NV_ERR_INVALID_EVENT
 *   NV_ERR_INVALID_STATE
 *
 */
#define NV9074_CTRL_CMD_RELEASE (0x90740103) /* finn: Evaluated from "(FINN_GF100_TIMED_SEMAPHORE_SW_SEM_INTERFACE_ID << 8) | NV9074_CTRL_CMD_RELEASE_PARAMS_MESSAGE_ID" */

#define NV9074_CTRL_CMD_RELEASE_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV9074_CTRL_CMD_RELEASE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 notifierGPUVA, 8);
    NV_DECLARE_ALIGNED(NvU64 semaphoreGPUVA, 8);
    NV_DECLARE_ALIGNED(NvU64 waitTimestamp, 8);
    NvU32 releaseValue;
    NvU32 releaseFlags;
} NV9074_CTRL_CMD_RELEASE_PARAMS;

#define NV9074_CTRL_CMD_RELEASE_FLAGS
#define NV9074_CTRL_CMD_RELEASE_FLAGS_NOTIFY                                 1:0        
#define NV9074_CTRL_CMD_RELEASE_FLAGS_NOTIFY_WRITE_ONLY        (0x00000000)
#define NV9074_CTRL_CMD_RELEASE_FLAGS_NOTIFY_WRITE_THEN_AWAKEN (0x00000001)

/* _ctrl9074.h_ */

