/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl208f/ctrl208ffifo.finn
//

#include "ctrl/ctrl208f/ctrl208fbase.h"

/*
 * NV208F_CTRL_CMD_FIFO_CHECK_ENGINE_CONTEXT
 *
 * This command checks whether or not engine context exists for a given
 * engine for the channel with a given channel ID. This API is intended
 * for testing virtual context. For debug only.
 *
 *   hChannel
 *     The handle to the channel.
 *   engine
 *     The engine ID.
 *     Valid values are:
 *        NV2080_ENGINE_TYPE_GRAPHICS
 *   exists
 *     The output are TRUE or FALSE.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_DEVICE
 *   NV_ERR_INVALID_CHANNEL
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV208F_CTRL_CMD_FIFO_CHECK_ENGINE_CONTEXT (0x208f0401) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FIFO_INTERFACE_ID << 8) | NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS {
    NvHandle hChannel;
    NvU32    engine;
    NvBool   exists;
} NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS;

/*
 * NV208F_CTRL_CMD_FIFO_ENABLE_VIRTUAL_CONTEXT
 *
 * This command enables virtual context for a given channel (for all engines).
 * This API is intended for testing virtual context. For debug only.
 *
 *   hChannel
 *     The handle to the channel.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_DEVICE
 *   NV_ERR_INVALID_CHANNEL
 */
#define NV208F_CTRL_CMD_FIFO_ENABLE_VIRTUAL_CONTEXT (0x208f0402) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FIFO_INTERFACE_ID << 8) | NV208F_CTRL_FIFO_ENABLE_VIRTUAL_CONTEXT_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FIFO_ENABLE_VIRTUAL_CONTEXT_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV208F_CTRL_FIFO_ENABLE_VIRTUAL_CONTEXT_PARAMS {
    NvHandle hChannel;
} NV208F_CTRL_FIFO_ENABLE_VIRTUAL_CONTEXT_PARAMS;

/*
 * NV208F_CTRL_CMD_FIFO_GET_CHANNEL_STATE
 *
 * This command returns the fifo channel state for the given channel.
 * This is for testing channel behavior.  For debug only.
 *
 *   hChannel 
 *    The handle to the channel
 *   hClient 
 *    The handle to the client
 *   bound
 *      The channel has been bound to channel RAM
 *   enabled
 *      The channel is able to run.
 *   scheduled
 *      The channel has been scheduled to run.
 *   cpuMap
 *      There is a cpu mapping available to this channel.
 *   contention
 *      The virtual channel is under contention
 *   runlistSet
 *      A runlist has been chosen for this channel
 *   deferRC
 *      An RC error has occurred, but recovery will occur at channel teardown.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_DEVICE
 *   NV_ERR_INVALID_CHANNEL
 */
#define NV208F_CTRL_CMD_FIFO_GET_CHANNEL_STATE (0x208f0403) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FIFO_INTERFACE_ID << 8) | NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS {
    NvHandle hChannel;
    NvHandle hClient;
    NvBool   bBound;
    NvBool   bEnabled;
    NvBool   bScheduled;
    NvBool   bCpuMap;
    NvBool   bContention;
    NvBool   bRunlistSet;
    NvBool   bDeferRC;
} NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS;

/*
 * NV208F_CTRL_CMD_FIFO_GET_CONTIG_RUNLIST_POOL
 *
 * This command returns the location of the pool runlists are allocated from for
 * WPR testing.
 * For debug only.
 *
 * physAddr [out]
 *    Physical address of the pool
 *
 * size [out]
 *    Size in bytes of the pool
 *
 */
#define NV208F_CTRL_CMD_FIFO_GET_CONTIG_RUNLIST_POOL (0x208f0404) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FIFO_INTERFACE_ID << 8) | NV208F_CTRL_FIFO_GET_CONTIG_RUNLIST_POOL_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FIFO_GET_CONTIG_RUNLIST_POOL_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV208F_CTRL_FIFO_GET_CONTIG_RUNLIST_POOL_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 physAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
} NV208F_CTRL_FIFO_GET_CONTIG_RUNLIST_POOL_PARAMS;

/* _ctrl208ffifo_h_ */

