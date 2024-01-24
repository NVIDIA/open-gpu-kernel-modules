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
// Source file:      ctrl/ctrl0080/ctrl0080fifo.finn
//

#include "ctrl/ctrl0080/ctrl0080base.h"

/* NV01_DEVICE_XX/NV03_DEVICE fifo control commands and parameters */

/**
 * NV0080_CTRL_FIFO_GET_CAPS
 *
 * This command returns the set of FIFO engine capabilities for the device
 * in the form of an array of unsigned bytes.  FIFO capabilities
 * include supported features and required workarounds for the FIFO
 * engine(s) within the device, each represented by a byte offset into the
 * table and a bit position within that byte.
 *
 *   capsTblSize
 *     This parameter specifies the size in bytes of the caps table.
 *     This value should be set to NV0080_CTRL_FIFO_CAPS_TBL_SIZE.
 *   capsTbl
 *     This parameter specifies a pointer to the client's caps table buffer
 *     into which the framebuffer caps bits will be transferred by the RM.
 *     The caps table is an array of unsigned bytes.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0080_CTRL_CMD_FIFO_GET_CAPS (0x801701) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FIFO_INTERFACE_ID << 8) | NV0080_CTRL_FIFO_GET_CAPS_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_FIFO_GET_CAPS_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0080_CTRL_FIFO_GET_CAPS_PARAMS {
    NvU32 capsTblSize;
    NV_DECLARE_ALIGNED(NvP64 capsTbl, 8);
} NV0080_CTRL_FIFO_GET_CAPS_PARAMS;

/* extract cap bit setting from tbl */
#define NV0080_CTRL_FIFO_GET_CAP(tbl,c)            (((NvU8)tbl[(1?c)]) & (0?c))

/* caps format is byte_index:bit_mask */
#define NV0080_CTRL_FIFO_CAPS_SUPPORT_SCHED_EVENT                    0:0x01
#define NV0080_CTRL_FIFO_CAPS_SUPPORT_PCI_PB                         0:0x02
#define NV0080_CTRL_FIFO_CAPS_SUPPORT_VID_PB                         0:0x04
#define NV0080_CTRL_FIFO_CAPS_USERD_IN_SYSMEM                        0:0x40
/* do not use pipelined PTE BLITs to update PTEs:  call the RM */
#define NV0080_CTRL_FIFO_CAPS_NO_PIPELINED_PTE_BLIT                  0:0x80
#define NV0080_CTRL_FIFO_CAPS_GPU_MAP_CHANNEL                        1:0x01
#define NV0080_CTRL_FIFO_CAPS_BUFFEREDMODE_SCHEDULING                1:0x02 // Deprecated
#define NV0080_CTRL_FIFO_CAPS_WFI_BUG_898467                         1:0x08 // Deprecated
#define NV0080_CTRL_FIFO_CAPS_HAS_HOST_LB_OVERFLOW_BUG_1667921       1:0x10
/*
 * To indicate Volta subcontext support with multiple VA spaces in a TSG.
 * We are not using "subcontext" tag for the property, since we also use
 * subcontext to represent pre-VOlta SCG feature, which only allows a single
 * VA space in a TSG.
 */
#define NV0080_CTRL_FIFO_CAPS_MULTI_VAS_PER_CHANGRP                  1:0x20


#define NV0080_CTRL_FIFO_CAPS_SUPPORT_WDDM_INTERLEAVING              1:0x40

/* size in bytes of fifo caps table */
#define NV0080_CTRL_FIFO_CAPS_TBL_SIZE 2

/*
 * NV0080_CTRL_CMD_FIFO_START_SELECTED_CHANNELS
 *
 * This command allows the caller to request that a set of channels
 * be added to the runlist.
 *
 *   fifoStartChannelListSize
 *     Size of the fifoStartChannelList.  The units are in entries, not
 *     bytes.
 *   fifoStartChannelList
 *     This will be a list of NV0080_CTRL_FIFO_CHANNEL data structures, 
 *     one for each channel that is to be started.
 *   channelHandle
 *     deprecated
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

/*
 *   hChannel
 *     This is the handle to the channel that is scheduled to be started.
 */
typedef struct NV0080_CTRL_FIFO_CHANNEL {
    NvHandle hChannel;
} NV0080_CTRL_FIFO_CHANNEL;

#define NV0080_CTRL_CMD_FIFO_START_SELECTED_CHANNELS (0x801705) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FIFO_INTERFACE_ID << 8) | NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS {
    NvU32    fifoStartChannelListCount;
    NvHandle channelHandle[8];
    NV_DECLARE_ALIGNED(NvP64 fifoStartChannelList, 8);
} NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS;

#define NV0080_CTRL_FIFO_ENGINE_ID_GRAPHICS                                               (0x00000000)
#define NV0080_CTRL_FIFO_ENGINE_ID_MPEG                                                   (0x00000001)
#define NV0080_CTRL_FIFO_ENGINE_ID_MOTION_ESTIMATION                                      (0x00000002)
#define NV0080_CTRL_FIFO_ENGINE_ID_VIDEO                                                  (0x00000003)
#define NV0080_CTRL_FIFO_ENGINE_ID_BITSTREAM                                              (0x00000004)
#define NV0080_CTRL_FIFO_ENGINE_ID_ENCRYPTION                                             (0x00000005)
#define NV0080_CTRL_FIFO_ENGINE_ID_FGT                                                    (0x00000006)

/*
 * NV0080_CTRL_CMD_FIFO_GET_ENGINE_CONTEXT_PROPERTIES
 *
 * This command is used to provide the caller with the alignment and size
 * of the context save region for an engine
 *
 *   engineId
 *     This parameter is an input parameter specifying the engineId for which
 *     the alignment/size is requested.
 *   alignment
 *     This parameter is an output parameter which will be filled in with the
 *     minimum alignment requirement.
 *   size
 *     This parameter is an output parameter which will be filled in with the
 *     minimum size of the context save region for the engine.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0080_CTRL_CMD_FIFO_GET_ENGINE_CONTEXT_PROPERTIES                                (0x801707) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FIFO_INTERFACE_ID << 8) | NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID                          4:0
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS                 (0x00000000)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_VLD                      (0x00000001)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_VIDEO                    (0x00000002)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_MPEG                     (0x00000003)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_CAPTURE                  (0x00000004)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_DISPLAY                  (0x00000005)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_ENCRYPTION               (0x00000006)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_POSTPROCESS              (0x00000007)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_ZCULL           (0x00000008)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PM              (0x00000009)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_COMPUTE_PREEMPT          (0x0000000a)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PREEMPT         (0x0000000b)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_SPILL           (0x0000000c)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PAGEPOOL        (0x0000000d)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_BETACB          (0x0000000e)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_RTV             (0x0000000f)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PATCH           (0x00000010)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_BUNDLE_CB       (0x00000011)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PAGEPOOL_GLOBAL (0x00000012)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_ATTRIBUTE_CB    (0x00000013)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_RTV_CB_GLOBAL   (0x00000014)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_GFXP_POOL       (0x00000015)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_GFXP_CTRL_BLK   (0x00000016)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_FECS_EVENT      (0x00000017)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PRIV_ACCESS_MAP (0x00000018)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_SETUP           (0x00000019)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_COUNT                    (0x0000001a)
#define NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS {
    NvU32 engineId;
    NvU32 alignment;
    NvU32 size;
} NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS;

/*
 * NV0080_CTRL_CMD_FIFO_RUNLIST_GROUP_CHANNELS      <Deprecated since Fermi+>
 *
 * This command allows the caller to group two sets of channels.  A channel 
 * set includes one or more channels.  After grouping, the grouped channel IDs 
 * are set to next to each other in the runlist.  This command can be used 
 * several times to group more than two channels. 
 *
 * Using a NV0080_CTRL_CMD_FIFO_RUNLIST_DIVIDE_TIMESLICE after 
 * NV0080_CTRL_CMD_FIFO_RUNLIST_GROUP_CHANNELS is the general usage.  A 
 * NV0080_CTRL_CMD_FIFO_RUNLIST_GROUP_CHANNELS after a 
 * NV0080_CTRL_CMD_FIFO_RUNLIST_DIVIDE_TIMESLICE for a channel handle is not 
 * allowed.
 *
 * NV0080_CTRL_FIFO_RUNLIST_GROUP_MAX_CHANNELS defines the max channels in a 
 * group.
 *
 *   hChannel1
 *     This parameter specifies the handle of the channel that belongs to the 
 *     base set of channels.
 *   hChannel2
 *     This parameter specifies the handle of the channel that belongs to the 
 *     additional set of channels.

 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_DEVICE
 *   NV_ERR_INVALID_CHANNEL
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0080_CTRL_CMD_FIFO_RUNLIST_GROUP_CHANNELS (0x801709) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FIFO_INTERFACE_ID << 8) | 0x9" */

typedef struct NV0080_CTRL_FIFO_RUNLIST_GROUP_CHANNELS_PARAM {
    NvHandle hChannel1;
    NvHandle hChannel2;
} NV0080_CTRL_FIFO_RUNLIST_GROUP_CHANNELS_PARAM;

#define NV0080_CTRL_FIFO_RUNLIST_GROUP_MAX_CHANNELS   (8)

/*
 * NV0080_CTRL_CMD_FIFO_RUNLIST_DIVIDE_TIMESLICE        <Deprecated since Fermi+>
 *
 * This command allows the caller to divide the timeslice (DMA_TIMESLICE) of a 
 * channel between the channels in the group in which the channel resides.  
 * After applying this command, a timeslice divided channel (group) has a
 * short timeslice and repeats more than once in the runlist.  The total
 * available execution time is not changed.
 *
 * Using this command after NV0080_CTRL_CMD_FIFO_RUNLIST_GROUP_CHANNELS is the 
 * general usage.  A NV0080_CTRL_CMD_FIFO_RUNLIST_GROUP_CHANNELS after a 
 * NV0080_CTRL_CMD_FIFO_RUNLIST_DIVIDE_TIMESLICE for a channel handle is not 
 * allowed.
 *
 *   hChannel
 *     This parameter specifies the handle of the channel for the channel
 *     group to which the divided timeslice operation will apply.
 *   tsDivisor
 *     This parameter specifies the timeslice divisor value.  This value
 *     should not exceed NV0080_CTRL_FIFO_RUNLIST_MAX_TIMESLICE_DIVISOR
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_DEVICE
 *   NV_ERR_INVALID_CHANNEL
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INSUFFICIENT_RESOURCES
 */
#define NV0080_CTRL_CMD_FIFO_RUNLIST_DIVIDE_TIMESLICE (0x80170b) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FIFO_INTERFACE_ID << 8) | 0xB" */

typedef struct NV0080_CTRL_FIFO_RUNLIST_DIVIDE_TIMESLICE_PARAM {
    NvHandle hChannel;
    NvU32    tsDivisor;
} NV0080_CTRL_FIFO_RUNLIST_DIVIDE_TIMESLICE_PARAM;

#define NV0080_CTRL_FIFO_RUNLIST_MAX_TIMESLICE_DIVISOR (12)

/*
 * NV0080_CTRL_CMD_FIFO_PREEMPT_RUNLIST                 <Deprecated since Fermi+>
 *
 * This command preepmts the engine represented by the specified runlist.
 * 
 *   hRunlist
 *     This parameter specifies the per engine runlist handle. This
 *     parameter is being retained to maintain backwards compatibility
 *     with clients that have not transitioned over to using runlists
 *     on a per subdevice basis.
 *
 *   engineID
 *     This parameter specifies the engine to be preempted. Engine defines
 *     can be found in cl2080.h. 
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0080_CTRL_CMD_FIFO_PREEMPT_RUNLIST           (0x80170c) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FIFO_INTERFACE_ID << 8) | 0xC" */

typedef struct NV0080_CTRL_FIFO_PREEMPT_RUNLIST_PARAMS {
    NvHandle hRunlist;
    NvU32    engineID;
} NV0080_CTRL_FIFO_PREEMPT_RUNLIST_PARAMS;


/*
 * NV0080_CTRL_CMD_FIFO_GET_CHANNELLIST
 *
 * Takes a list of hChannels as input and returns the
 * corresponding Channel IDs that they corresponding to
 * on hw.
 * 
 *   numChannels
 *     Size of input hChannellist
 *   pChannelHandleList
 *     List of input channel handles
 *   pChannelList
 *     List of Channel ID's corresponding to the
 *     each entry in the hChannelList.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0080_CTRL_CMD_FIFO_GET_CHANNELLIST (0x80170d) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FIFO_INTERFACE_ID << 8) | NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS_MESSAGE_ID (0xDU)

typedef struct NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS {
    NvU32 numChannels;
    NV_DECLARE_ALIGNED(NvP64 pChannelHandleList, 8);
    NV_DECLARE_ALIGNED(NvP64 pChannelList, 8);
} NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS;


/*
 * NV0080_CTRL_CMD_FIFO_GET_LATENCY_BUFFER_SIZE
 *
 *  This control call is used to return the number of gp methods(gpsize) and push buffer methods(pbsize)
 *  allocated to each engine.
 *
 *engineID
 *  The engine ID which is an input
 *
 *gpEntries
 *  number of gp entries
 *
 *pbEntries
 *  number of pb entries (in units of 32B rows)
 *
 */


#define NV0080_CTRL_CMD_FIFO_GET_LATENCY_BUFFER_SIZE (0x80170e) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FIFO_INTERFACE_ID << 8) | NV0080_CTRL_FIFO_GET_LATENCY_BUFFER_SIZE_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_FIFO_GET_LATENCY_BUFFER_SIZE_PARAMS_MESSAGE_ID (0xEU)

typedef struct NV0080_CTRL_FIFO_GET_LATENCY_BUFFER_SIZE_PARAMS {
    NvU32 engineID;
    NvU32 gpEntries;
    NvU32 pbEntries;
} NV0080_CTRL_FIFO_GET_LATENCY_BUFFER_SIZE_PARAMS;

#define NV0080_CTRL_FIFO_GET_CHANNELLIST_INVALID_CHANNEL (0xffffffff)

/*
 * NV0080_CTRL_CMD_FIFO_SET_CHANNEL_PROPERTIES
 * 
 * This command allows internal properties of the channel
 * to be modified even when the channel is active. Most of these properties 
 * are not meant to be modified during normal runs hence have been
 * kept separate from channel alloc params. It is the
 * responsibility of the underlying hal routine to make
 * sure the channel properties are changed while the channel
 * is *NOT* in a transient state.
 * 
 *   hChannel
 *     The handle to the channel.
 *
 *   property
 *     The channel property to be modified.
 *     NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_xxx provides the entire list
 *     of properties.
 *
 *   value
 *     The new value for the property.
 *     When property = NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_ENGINETIMESLICEINMICROSECONDS
 *          value    = timeslice in microseconds
 *          desc:      Used to change a channel's engine timeslice in microseconds
 *
 *          property = NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PBDMATIMESLICEINMICROSECONDS
 *          value    = timeslice in microseconds
 *          desc:      Used to change a channel's pbdma timeslice in microseconds
 *
 *          property = NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_ENGINETIMESLICEDISABLE
 *          value    is ignored
 *          desc:      Disables a channel from being timesliced out from an engine.
 *                     Other scheduling events like explicit yield, acquire failures will
 *                     switch out the channel though.
 *
 *          property = NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PBDMATIMESLICEDISABLE
 *          value    is ignored
 *          desc:      Disables a channel from being timesliced out from its pbdma.
 *
 *          property = NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_INVALIDATE_PDB_TARGET
 *          value    is ignored
 *          desc:      Override the channel's page directory pointer table with an
 *                     erroneous aperture value. (TODO: make test calls NV_VERIF_FEATURES
 *                     only)(VERIF ONLY)
 *                     
 *          property = NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_RESETENGINECONTEXT
 *          value    = engineID of engine that will have its context pointer reset.
 *                     engineID defines can be found in cl2080.h
 *                     (e.g., NV2080_ENGINE_TYPE_GRAPHICS)
 *          desc:      Override the channel's engine context pointer with a non existent
 *                     buffer forcing it to fault. (VERIF ONLY)
 *                     
 *          property = NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_RESETENGINECONTEXT_NOPREEMPT
 *          value    = engineID of engine that will have its context pointer reset.
 *                     engineID defines can be found in cl2080.h
 *                     (e.g., NV2080_ENGINE_TYPE_GRAPHICS)
 *          desc:      Override the channel's engine context pointer with a non existent
 *                     buffer forcing it to fault. However the channel will not be preempted
 *                     before having its channel state modified.(VERIF ONLY)
 *
 *          property = NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_NOOP
 *          value    is ignored
 *          desc:      does not change any channel state exercises a full channel preempt/
 *                     unbind/bind op. (VERIF ONLY)
 *          
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_CHANNEL
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV0080_CTRL_CMD_FIFO_SET_CHANNEL_PROPERTIES      (0x80170f) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FIFO_INTERFACE_ID << 8) | NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS_MESSAGE_ID (0xFU)

typedef struct NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS {
    NvHandle hChannel;
    NvU32    property;
    NV_DECLARE_ALIGNED(NvU64 value, 8);
} NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS;

#define NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_ENGINETIMESLICEINMICROSECONDS (0x00000000)
#define NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PBDMATIMESLICEINMICROSECONDS  (0x00000001)
#define NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_ENGINETIMESLICEDISABLE        (0x00000002)
#define NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PBDMATIMESLICEDISABLE         (0x00000003)
#define NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_INVALIDATE_PDB_TARGET         (0x00000004)
#define NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_RESETENGINECONTEXT            (0x00000005)
#define NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_NOOP                          (0x00000007)
#define NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_RESETENGINECONTEXT_NOPREEMPT  (0x00000008)



/*
 * NV0080_CTRL_CMD_FIFO_STOP_RUNLIST
 *
 * Stops all processing on the runlist for the given engine.  This is only
 * valid in per-engine round-robin scheduling mode.
 * 
 *   engineID
 *     This parameter specifies the engine to be stopped. Engine defines
 *     can be found in cl2080.h. 
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV0080_CTRL_CMD_FIFO_STOP_RUNLIST (0x801711) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FIFO_INTERFACE_ID << 8) | NV0080_CTRL_FIFO_STOP_RUNLIST_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_FIFO_STOP_RUNLIST_PARAMS_MESSAGE_ID (0x11U)

typedef struct NV0080_CTRL_FIFO_STOP_RUNLIST_PARAMS {
    NvU32 engineID;
} NV0080_CTRL_FIFO_STOP_RUNLIST_PARAMS;

/*
 * NV0080_CTRL_CMD_FIFO_START_RUNLIST
 *
 * Restarts a runlist previously stopped with NV0080_CTRL_CMD_FIFO_STOP_RUNLIST.
 * This is only valid for per-engine round-robin mode.
 * 
 *   engineID
 *     This parameter specifies the engine to be started. Engine defines
 *     can be found in cl2080.h. 
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV0080_CTRL_CMD_FIFO_START_RUNLIST (0x801712) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FIFO_INTERFACE_ID << 8) | NV0080_CTRL_FIFO_START_RUNLIST_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_FIFO_START_RUNLIST_PARAMS_MESSAGE_ID (0x12U)

typedef struct NV0080_CTRL_FIFO_START_RUNLIST_PARAMS {
    NvU32 engineID;
} NV0080_CTRL_FIFO_START_RUNLIST_PARAMS;

/**
 * NV0080_CTRL_FIFO_GET_CAPS_V2
 *
 * This command returns the same set of FIFO engine capabilities for the device
 * as @ref NV0080_CTRL_FIFO_GET_CAPS. The difference is in the structure
 * NV0080_CTRL_FIFO_GET_CAPS_V2_PARAMS, which contains a statically sized array,
 * rather than a caps table pointer and a caps table size in
 * NV0080_CTRL_FIFO_GET_CAPS_PARAMS.
 *
 *   capsTbl
 *     This parameter is an array of the client's caps table buffer.
 *     The framebuffer caps bits will be written by the RM.
 *     The caps table is an array of unsigned bytes.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0080_CTRL_CMD_FIFO_GET_CAPS_V2 (0x801713) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FIFO_INTERFACE_ID << 8) | NV0080_CTRL_FIFO_GET_CAPS_V2_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_FIFO_GET_CAPS_V2_PARAMS_MESSAGE_ID (0x13U)

typedef struct NV0080_CTRL_FIFO_GET_CAPS_V2_PARAMS {
    NvU8 capsTbl[NV0080_CTRL_FIFO_CAPS_TBL_SIZE];
} NV0080_CTRL_FIFO_GET_CAPS_V2_PARAMS;

/**
 * NV0080_CTRL_CMD_FIFO_IDLE_CHANNELS
 *
 * @brief This command idles (deschedules and waits for pending work to complete) channels
 *        belonging to a particular device.
 *
 *   numChannels
 *     Number of channels to idle
 *
 *   hChannels
 *     Array of channel handles to idle
 *
 *   flags
 *     NVOS30_FLAGS that control aspects of how the channel is idled
 *
 *   timeout
 *     GPU timeout in microseconds, for each CHID Manager's idling operation
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_TIMEOUT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_LOCK_STATE
 */
#define NV0080_CTRL_CMD_FIFO_IDLE_CHANNELS              (0x801714) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_FIFO_INTERFACE_ID << 8) | NV0080_CTRL_FIFO_IDLE_CHANNELS_PARAMS_MESSAGE_ID" */
#define NV0080_CTRL_CMD_FIFO_IDLE_CHANNELS_MAX_CHANNELS 4096

#define NV0080_CTRL_FIFO_IDLE_CHANNELS_PARAMS_MESSAGE_ID (0x14U)

typedef struct NV0080_CTRL_FIFO_IDLE_CHANNELS_PARAMS {
    NvU32    numChannels;
    NvHandle hChannels[NV0080_CTRL_CMD_FIFO_IDLE_CHANNELS_MAX_CHANNELS];
    NvU32    flags;
    NvU32    timeout;
} NV0080_CTRL_FIFO_IDLE_CHANNELS_PARAMS;

/* _ctrl0080fifo_h_ */
