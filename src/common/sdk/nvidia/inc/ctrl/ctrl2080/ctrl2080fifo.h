/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl2080/ctrl2080fifo.finn
//

#include "nvcfg_sdk.h"
#include "ctrl/ctrl2080/ctrl2080base.h"

/*
 * NV2080_CTRL_CMD_SET_GPFIFO
 *
 * This command set the GPFIFO offset and number of entries for a channel
 * after it has been allocated. The channel must be idle and not pending,
 * otherwise ERROR_IN_USE will be returned.
 *
 *   hChannel
 *     The handle to the channel.
 *   base
 *     The base of the GPFIFO in the channel ctxdma.
 *   numEntries
 *     The number of entries in the GPFIFO.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_DEVICE
 *   NV_ERR_INVALID_CHANNEL
 *   NV_ERR_STATE_IN_USE
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV2080_CTRL_CMD_SET_GPFIFO (0x20801102) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_CMD_SET_GPFIFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_SET_GPFIFO_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_CMD_SET_GPFIFO_PARAMS {
    NvHandle hChannel;
    NV_DECLARE_ALIGNED(NvU64 base, 8);
    NvU32    numEntries;
} NV2080_CTRL_CMD_SET_GPFIFO_PARAMS;

/*
 * NV2080_CTRL_FIFO_BIND_CHANNEL
 *
 * This structure is used to describe a channel that is to have
 * it's bindable engines bound to those of other channels.
 *
 * hClient
 *  This structure member contains the handle of the client object
 *  that owns the channel object specified by hChannel.
 *
 * hChannel
 *  This structure member contains the channel handle of the channel
 *  object.
 */

typedef struct NV2080_CTRL_FIFO_BIND_CHANNEL {
    NvHandle hClient;
    NvHandle hChannel;
} NV2080_CTRL_FIFO_BIND_CHANNEL;

/*
 * NV2080_CTRL_CMD_FIFO_BIND_ENGINES
 *
 * This control call is now deprecated.
 * This command can be used to bind different video engines on G8X from separate
 * channels together for operations such as idling.  The set of bindable engines
 * includes the NV2080_ENGINE_TYPE_BSP, NV2080_ENGINE_TYPE_VP and
 * NV2080_ENGINE_TYPE_PPP engines.
 *
 * bindChannelCount
 *  This parameter specifies the number of channels to bind together.  This
 *  parameter cannot exceed NV2080_CTRL_FIFO_BIND_ENGINES_MAX_CHANNELS.
 *
 * bindChannels
 *  The parameter specifies the array of channels to bind together.  The first
 *  bindChannelCount entries are used in the bind channel operation.
 *
 * Possible status values returned are:
 *  NV_OK
 *  NV_ERR_INVALID_DEVICE
 *  NV_ERR_INVALID_CHANNEL
 *  NV_ERR_INVALID_ARGUMENT
 *  NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_FIFO_BIND_ENGINES_MAX_CHANNELS (16)

#define NV2080_CTRL_FIFO_BIND_ENGINES_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV2080_CTRL_FIFO_BIND_ENGINES_PARAMS {
    NvU32                         bindChannelCount;
    NV2080_CTRL_FIFO_BIND_CHANNEL bindChannels[NV2080_CTRL_FIFO_BIND_ENGINES_MAX_CHANNELS];
} NV2080_CTRL_FIFO_BIND_ENGINES_PARAMS;

#define NV2080_CTRL_CMD_FIFO_BIND_ENGINES          (0x20801103) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_BIND_ENGINES_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES
 *
 * This command is used for a client to setup specialized custom operational
 * properties that may be specific to an environment, or properties that
 * should be set generally but are not for reasons of backward compatibility
 * with previous chip generations
 *
 *  flags
 *   This field specifies the operational properties to be applied
 *
 * Possible return status values returned are
 *   NV_OK
 *   NV_ERR_INVALID_CHANNEL
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES (0x20801104) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_PARAMS {
    NvU32 flags;
} NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_PARAMS;

#define NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_FLAGS_ERROR_ON_STUCK_SEMAPHORE                 0:0
#define NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_FLAGS_ERROR_ON_STUCK_SEMAPHORE_FALSE (0x00000000)
#define NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_FLAGS_ERROR_ON_STUCK_SEMAPHORE_TRUE  (0x00000001)

/*
 * NV2080_CTRL_CMD_FIFO_GET_PHYSICAL_CHANNEL_COUNT
 *
 * This command returns the maximum number of physical channels available for
 * allocation on the current GPU.  This may be less than or equal to the total
 * number of channels supported by the current hardware.
 *
 * physChannelCount
 *   This output parameter contains the maximum physical channel count.
 *
 *   physChannelCountInUse
 *     This output parameter contains the number of physical channels in use
 *
 * Possible return status values returned are
 *   NV_OK
 *
 */
#define NV2080_CTRL_CMD_FIFO_GET_PHYSICAL_CHANNEL_COUNT                                 (0x20801108) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS {
    NvU32 physChannelCount;
    NvU32 physChannelCountInUse;
} NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS;

/*
 * NV2080_CTRL_FIFO_INFO
 *
 * This structure represents a single 32bit fifo engine value.  Clients
 * request a particular FIFO engine value by specifying a unique fifo
 * information index.
 *
 * Legal fifo information index values are:
 *   NV2080_CTRL_FIFO_INFO_INDEX_INSTANCE_TOTAL
 *     This index can be used to request the amount of instance space
 *     in kilobytes reserved by the fifo engine.
 *   NV2080_CTRL_FIFO_INFO_INDEX_MAX_CHANNEL_GROUPS
 *     This index can be used to query the maximum number of channel groups
 *     that can be allocated on the GPU.
 *   NV2080_CTRL_FIFO_INFO_INDEX_MAX_CHANNELS_PER_GROUP
 *     This index can be used to query the maximum number of channels that can
 *    be allocated in a single channel group.
 *   NV2080_CTRL_FIFO_INFO_INDEX_MAX_SUBCONTEXT_PER_GROUP
 *     This index can be used to query the maximum number of subcontext that can
 *     be allocated in a single channel group.
 *   NV2080_CTRL_FIFO_INFO_INDEX_BAR1_USERD_START_OFFSET
 *     This index can be used to query the starting offset of the RM
 *     pre-allocated USERD range in BAR1. This index query is honored only
 *     on Legacy-vGPU host RM.
 *   NV2080_CTRL_FIFO_INFO_INDEX_DEFAULT_CHANNEL_TIMESLICE
 *     This index can be used to query the default timeslice value
 *     (microseconds) used for a channel or channel group.
 *   NV2080_CTRL_FIFO_INFO_INDEX_CHANNEL_GROUPS_IN_USE
 *     This index can be used to query the number of channel groups that are
 *     already allocated on the GPU.
 *   NV2080_CTRL_FIFO_INFO_INDEX_IS_PER_RUNLIST_CHANNEL_RAM_SUPPORTED
 *     This index can be used to check if per runlist channel ram is supported, and
 *     to query the supported number of channels per runlist.
 *   NV2080_CTRL_FIFO_INFO_INDEX_MAX_CHANNEL_GROUPS_PER_ENGINE
 *     This index can be used to get max channel groups supported per engine/runlist.
 *   NV2080_CTRL_FIFO_INFO_INDEX_CHANNEL_GROUPS_IN_USE_PER_ENGINE
 *     This index can be used too get channel groups currently in use per engine/runlist.
 *   NV2080_CTRL_FIFO_INFO_INDEX_MAX_LOWER_SUBCONTEXT
 *     This index can be used to query the maximum "lower" subcontext index
 *     allocated under NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_ASYNC_PREFER_LOWER.
 *     Note: Includes subcontext ID 0, which will be allocated last in ASYNC allocation mode.
 *
 */
typedef NVXXXX_CTRL_XXX_INFO NV2080_CTRL_FIFO_INFO;

/* valid fifo info index values */
#define NV2080_CTRL_FIFO_INFO_INDEX_INSTANCE_TOTAL                       (0x000000000)
#define NV2080_CTRL_FIFO_INFO_INDEX_MAX_CHANNEL_GROUPS                   (0x000000001)
#define NV2080_CTRL_FIFO_INFO_INDEX_MAX_CHANNELS_PER_GROUP               (0x000000002)
#define NV2080_CTRL_FIFO_INFO_INDEX_MAX_SUBCONTEXT_PER_GROUP             (0x000000003)
#define NV2080_CTRL_FIFO_INFO_INDEX_BAR1_USERD_START_OFFSET              (0x000000004)
#define NV2080_CTRL_FIFO_INFO_INDEX_DEFAULT_CHANNEL_TIMESLICE            (0x000000005)
#define NV2080_CTRL_FIFO_INFO_INDEX_CHANNEL_GROUPS_IN_USE                (0x000000006)
#define NV2080_CTRL_FIFO_INFO_INDEX_IS_PER_RUNLIST_CHANNEL_RAM_SUPPORTED (0x000000007)
#define NV2080_CTRL_FIFO_INFO_INDEX_MAX_CHANNEL_GROUPS_PER_ENGINE        (0x000000008)
#define NV2080_CTRL_FIFO_INFO_INDEX_CHANNEL_GROUPS_IN_USE_PER_ENGINE     (0x000000009)
#define NV2080_CTRL_FIFO_INFO_INDEX_MAX_LOWER_SUBCONTEXT                 (0x00000000a)


/* set INDEX_MAX to greatest possible index value */
#define NV2080_CTRL_FIFO_INFO_INDEX_MAX                                  NV2080_CTRL_FIFO_INFO_INDEX_MAX_LOWER_SUBCONTEXT

#define NV2080_CTRL_FIFO_GET_INFO_USERD_OFFSET_SHIFT                     (12)

/*
 * NV2080_CTRL_CMD_FIFO_GET_INFO
 *
 * This command returns fifo engine information for the associated GPU.
 * Requests to retrieve fifo information use an array of one or more
 * NV2080_CTRL_FIFO_INFO structures.
 *
 *   fifoInfoTblSize
 *     This field specifies the number of valid entries in the fifoInfoList
 *     array.  This value cannot exceed NV2080_CTRL_FIFO_GET_INFO_MAX_ENTRIES.
 *   fifoInfoTbl
 *     This parameter contains the client's fifo info table into
 *     which the fifo info values will be transferred by the RM.
 *     The fifo info table is an array of NV2080_CTRL_FIFO_INFO structures.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FIFO_GET_INFO                                    (0x20801109) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_GET_INFO_PARAMS_MESSAGE_ID" */

/* maximum number of NV2080_CTRL_FIFO_INFO entries per request */
#define NV2080_CTRL_FIFO_GET_INFO_MAX_ENTRIES                            (256)

#define NV2080_CTRL_FIFO_GET_INFO_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV2080_CTRL_FIFO_GET_INFO_PARAMS {
    NvU32                 fifoInfoTblSize;
    /*
     * C form:
     * NV2080_CTRL_FIFO_INFO fifoInfoTbl[NV2080_CTRL_FIFO_GET_INFO_MAX_ENTRIES];
     */
    NV2080_CTRL_FIFO_INFO fifoInfoTbl[NV2080_CTRL_FIFO_GET_INFO_MAX_ENTRIES];
    NvU32                 engineType;
} NV2080_CTRL_FIFO_GET_INFO_PARAMS;



/*
 * NV2080_CTRL_FIFO_CHANNEL_PREEMPTIVE_REMOVAL
 *
 * This command removes the specified channel from the associated GPU's runlist
 * and then initiates RC recovery.  If the channel is active it will first be preempted.
 *   hChannel
 *     The handle to the channel to be preempted.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_CHANNEL
 */
#define NV2080_CTRL_CMD_FIFO_CHANNEL_PREEMPTIVE_REMOVAL (0x2080110a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_CHANNEL_PREEMPTIVE_REMOVAL_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_CHANNEL_PREEMPTIVE_REMOVAL_PARAMS_MESSAGE_ID (0xAU)

typedef struct NV2080_CTRL_FIFO_CHANNEL_PREEMPTIVE_REMOVAL_PARAMS {
    NvHandle hChannel;
} NV2080_CTRL_FIFO_CHANNEL_PREEMPTIVE_REMOVAL_PARAMS;

/*
 * NV2080_CTRL_CMD_FIFO_DISABLE_CHANNELS
 *
 * This command will disable or enable scheduling of channels described in the
 * list provided. Whether or not the channels are also preempted off the GPU
 * can be controlled by bOnlyDisableScheduling. By default channels are preempted
 * off the GPU.
 *
 *  bDisable
 *      This value determines whether to disable or
 *      enable the set of channels.
 *  numChannels
 *      The number of channels to be stopped.
 *  bOnlyDisableScheduling
 *      When false and bDisable=NV_TRUE,the call will ensure none of the listed
 *      channels are running in hardware and will not run until a call with
 *      bDisable=NV_FALSE is made. When true and bDisable=NV_TRUE, the control
 *      call will ensure that none of the listed channels can be scheduled on the
 *      GPU until a call with bDisable=NV_FALSE is made, but will not remove any
 *      of the listed channels from hardware if they are currently running. When
 *      bDisable=NV_FALSE this field is ignored.
 *  bRewindGpPut
 *      If a channel is being disabled and bRewindGpPut=NV_TRUE, the channel's RAMFC
 *      will be updated so that GP_PUT is reset to the value of GP_GET.
 *  hClientList
 *      An array of NvU32 listing the client handles
 *  hChannelList
 *      An array of NvU32 listing the channel handles
 *      to be stopped.
 *  pRunlistPreemptEvent
 *      KEVENT handle for Async HW runlist preemption (unused on preMaxwell)
 *      When NULL, will revert to synchronous preemption with spinloop
 *
 * Possible status values returned are:
 *    NV_OK
 *    NVOS_INVALID_STATE
 */

#define NV2080_CTRL_CMD_FIFO_DISABLE_CHANNELS         (0x2080110b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_DISABLE_CHANNELS_MAX_ENTRIES (64)

#define NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS_MESSAGE_ID (0xBU)

typedef struct NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS {
    NvBool   bDisable;
    NvU32    numChannels;
    NvBool   bOnlyDisableScheduling;
    NvBool   bRewindGpPut;
    NV_DECLARE_ALIGNED(NvP64 pRunlistPreemptEvent, 8);
    // C form:  NvHandle hClientList[NV2080_CTRL_FIFO_DISABLE_CHANNELS_MAX_ENTRIES]
    NvHandle hClientList[NV2080_CTRL_FIFO_DISABLE_CHANNELS_MAX_ENTRIES];
    // C form:  NvHandle hChannelList[NV2080_CTRL_FIFO_DISABLE_CHANNELS_MAX_ENTRIES]
    NvHandle hChannelList[NV2080_CTRL_FIFO_DISABLE_CHANNELS_MAX_ENTRIES];
} NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS;

#define NV2080_CTRL_FIFO_DISABLE_CHANNEL_FALSE         (0x00000000)
#define NV2080_CTRL_FIFO_DISABLE_CHANNEL_TRUE          (0x00000001)
#define NV2080_CTRL_FIFO_ONLY_DISABLE_SCHEDULING_FALSE (0x00000000)
#define NV2080_CTRL_FIFO_ONLY_DISABLE_SCHEDULING_TRUE  (0x00000001)

/*
 * NV2080_CTRL_FIFO_MEM_INFO
 *
 * This structure describes the details of a block of memory. It consists
 * of the following fields
 *
 * aperture
 *   One of the NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_* values
 * base
 *   Physical base address of the memory
 * size
 *   Size in bytes of the memory
*/
typedef struct NV2080_CTRL_FIFO_MEM_INFO {
    NvU32 aperture;
    NV_DECLARE_ALIGNED(NvU64 base, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
} NV2080_CTRL_FIFO_MEM_INFO;

/*
 * NV2080_CTRL_FIFO_CHANNEL_MEM_INFO
 *
 * This structure describes the details of the instance memory, ramfc
 * and method buffers a channel. It consists of the following fields
 *
 *   inst
 *     Structure describing the details of instance memory
 *   ramfc
 *     Structure describing the details of ramfc
 *   methodBuf
 *     Array of structures describing the details of method buffers
 *   methodBufCount
 *     Number of method buffers(one per runqueue)
 */

// max runqueues
#define NV2080_CTRL_FIFO_GET_CHANNEL_MEM_INFO_MAX_COUNT 0x2

typedef struct NV2080_CTRL_FIFO_CHANNEL_MEM_INFO {
    NV_DECLARE_ALIGNED(NV2080_CTRL_FIFO_MEM_INFO inst, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_FIFO_MEM_INFO ramfc, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_FIFO_MEM_INFO methodBuf[NV2080_CTRL_FIFO_GET_CHANNEL_MEM_INFO_MAX_COUNT], 8);
    NvU32 methodBufCount;
} NV2080_CTRL_FIFO_CHANNEL_MEM_INFO;

/*
 * NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM
 *
 * This command returns the memory aperture, physical base address and the
 * size of each of the instance memory, cache1 and ramfc of a channel.
 *
 *   hChannel
 *     The handle to the channel for which the memory information is desired.
 *   chMemInfo
 *     A NV2080_CTRL_FIFO_CHANNEL_MEM_INFO structure
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_CHANNEL
*/

#define NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO (0x2080110c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS_MESSAGE_ID (0xCU)

typedef struct NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS {
    NvHandle hChannel;
    NV_DECLARE_ALIGNED(NV2080_CTRL_FIFO_CHANNEL_MEM_INFO chMemInfo, 8);
} NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS;

#define NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_INVALID     0x00000000
#define NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_VIDMEM      0x00000001
#define NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_SYSMEM_COH  0x00000002
#define NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_SYSMEM_NCOH 0x00000003

/*
 * NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION
 *
 *    This command determines the location (vidmem/sysmem)
 *    and attribute (cached/uncached/write combined) of memory where USERD is located.
 *
 *   aperture
 *     One of the NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_APERTURE_* values.
 *
 *   attribute
 *     One of the NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_ATTRIBUTE_* values.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_DEVICE
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_POINTER
*/

#define NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION                   (0x2080110d) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS_MESSAGE_ID (0xDU)

typedef struct NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS {
    NvU32 aperture;
    NvU32 attribute;
} NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS;

// support for CPU coherent vidmem (VIDMEM_NVILINK_COH) is not yet available in RM

#define NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_APERTURE_VIDMEM         0x00000000
#define NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_APERTURE_SYSMEM         0x00000001

#define NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_ATTRIBUTE_CACHED        0x00000000
#define NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_ATTRIBUTE_UNCACHED      0X00000001
#define NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_ATTRIBUTE_WRITECOMBINED 0X00000002


#define NV2080_CTRL_CMD_VGPU_SCHEDULER_POLICY_UNKNOWN                   0
#define NV2080_CTRL_CMD_VGPU_SCHEDULER_POLICY_OTHER                     1
#define NV2080_CTRL_CMD_VGPU_SCHEDULER_POLICY_BEST_EFFORT               2
#define NV2080_CTRL_CMD_VGPU_SCHEDULER_POLICY_EQUAL_SHARE               3
#define NV2080_CTRL_CMD_VGPU_SCHEDULER_POLICY_FIXED_SHARE               4

// Count of the supported vGPU scheduler policies
#define NV2080_CTRL_CMD_SUPPORTED_VGPU_SCHEDULER_POLICY_COUNT           3

#define NV2080_CTRL_CMD_VGPU_SCHEDULER_ARR_DEFAULT                      0
#define NV2080_CTRL_CMD_VGPU_SCHEDULER_ARR_DISABLE                      1
#define NV2080_CTRL_CMD_VGPU_SCHEDULER_ARR_ENABLE                       2

/*
 * NV2080_CTRL_CMD_FIFO_OBJSCHED_SW_GET_LOG
 *
 * This command returns the OBJSCHED_SW log enties.
 *
 *   engineId
 *     This field specifies the NV2080_ENGINE_TYPE_* engine whose SW runlist log
 *     entries are to be fetched.
 *
 *   count
 *     This field returns the count of log entries fetched.
 *
 *   entry
 *     The array of SW runlist log entries.
 *
 *       timestampNs
 *         Timestamp in ns when this SW runlist was preeempted.
 *
 *       timeRunTotalNs
 *         Total time in ns this SW runlist has run as compared to others.
 *
 *       timeRunNs
 *         Time in ns this SW runlist ran before preemption.
 *
 *       swrlId
 *         SW runlist Id.
 *
 *   schedPolicy
 *     This field returns the runlist scheduling policy. It specifies the
 *     NV2080_CTRL_CMD_VGPU_SCHEDULER_POLICY_* scheduling policy.
 *
 *   arrEnabled
 *     This field returns if Adaptive round robin scheduler
 *     is enabled/disabled.
 *
 *   arrAvgFactor
 *     This field returns the average factor to be used in compensating the timeslice
 *     for Adaptive scheduler mode.
 *
 *  targetTimesliceNs
 *      This field returns the target timeslice duration in ns for each SW runlist
 *      as configured by the user or the default value otherwise.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
*/

#define NV2080_CTRL_CMD_FIFO_OBJSCHED_SW_GET_LOG                        (0x2080110e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_OBJSCHED_SW_GET_LOG_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_OBJSCHED_SW_COUNT                              32
#define NV2080_CTRL_FIFO_OBJSCHED_SW_NCOUNTERS                          8
#define NV2080_CTRL_FIFO_OBJSCHED_SW_GET_LOG_ENTRIES                    200

#define NV2080_CTRL_FIFO_OBJSCHED_SW_GET_LOG_PARAMS_MESSAGE_ID (0xEU)

typedef struct NV2080_CTRL_FIFO_OBJSCHED_SW_GET_LOG_PARAMS {
    NvU32 engineId;
    NvU32 count;
    struct {
        NV_DECLARE_ALIGNED(NvU64 timestampNs, 8);
        NV_DECLARE_ALIGNED(NvS64 timeRunTotalNs, 8);
        NvU32 timeRunNs;
        NvU32 swrlId;
        NvU32 targetTimeSlice;
        NV_DECLARE_ALIGNED(NvU64 cumulativePreemptionTime, 8);
        NV_DECLARE_ALIGNED(NvU64 counters[NV2080_CTRL_FIFO_OBJSCHED_SW_NCOUNTERS], 8);
    } entry[NV2080_CTRL_FIFO_OBJSCHED_SW_GET_LOG_ENTRIES];
    NvU32 schedPolicy;
    NvU32 arrEnabled;
    NvU32 arrAvgFactor;
    NvU32 targetTimesliceNs;
} NV2080_CTRL_FIFO_OBJSCHED_SW_GET_LOG_PARAMS;



/*
 *  NV2080_CTRL_CMD_FIFO_GET_DEVICE_INFO_TABLE
 *
 *  This command retrieves entries from the SW encoded GPU device info table
 *  from Host RM.
 *
 *  Parameters:
 *
 *    baseIndex [in]
 *      The starting index to read from the devinfo table. Must be a multiple of
 *      MAX_ENTRIES.
 *
 *    entries [out]
 *      A buffer to store up to MAX_ENTRIES entries of the devinfo table.
 *
 *    numEntries [out]
 *      Number of populated entries in the provided buffer.
 *
 *    bMore [out]
 *      A boolean flag indicating whether more valid entries are available to be
 *      read. A value of NV_TRUE indicates that a further call to this control
 *      with baseIndex incremented by MAX_ENTRIES will yield further valid data.
 *
 *  Possible status values returned are:
 *    NV_OK
 */
#define NV2080_CTRL_CMD_FIFO_GET_DEVICE_INFO_TABLE                 (0x20801112) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_MAX_DEVICES         256
#define NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_MAX_ENTRIES         32
#define NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_ENGINE_DATA_TYPES   16
#define NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_ENGINE_MAX_PBDMA    2
#define NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_ENGINE_MAX_NAME_LEN 16

/*
 * NV2080_CTRL_FIFO_DEVICE_ENTRY
 *
 * This structure contains the engine, engine name and
 * push buffers information of FIFO device entry. It consists of the following fields
 *
 *   engineData
 *     Type of the engine
 *   pbdmaIds
 *     List of pbdma ids associated with engine
 *   pbdmaFaultIds
 *     List of pbdma fault ids associated with engine
 *   numPbdmas
 *     Number of pbdmas
 *   engineName
 *     Name of the engine
 */
typedef struct NV2080_CTRL_FIFO_DEVICE_ENTRY {
    NvU32 engineData[NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_ENGINE_DATA_TYPES];
    NvU32 pbdmaIds[NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_ENGINE_MAX_PBDMA];
    NvU32 pbdmaFaultIds[NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_ENGINE_MAX_PBDMA];
    NvU32 numPbdmas;
    char  engineName[NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_ENGINE_MAX_NAME_LEN];
} NV2080_CTRL_FIFO_DEVICE_ENTRY;

#define NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS_MESSAGE_ID (0x12U)

typedef struct NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS {
    NvU32                         baseIndex;
    NvU32                         numEntries;
    NvBool                        bMore;
    // C form: NV2080_CTRL_FIFO_DEVICE_ENTRY entries[NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_MAX_ENTRIES];
    NV2080_CTRL_FIFO_DEVICE_ENTRY entries[NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_MAX_ENTRIES];
} NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS;

/*
 *  NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT
 *
 *  This command clears the ENGINE or PBDMA FAULTED bit and reschedules the faulted channel
 *  by ringing channel's doorbell
 *
 *  Parameters:
 *
 *    engineType [in]
 *      The NV2080_ENGINE_TYPE of the engine to which the faulted
 *      channel is bound. This may be a logical id for guest RM in
 *      case of SMC.
 *
 *    vChid [in]
 *      Virtual channel ID on which the fault occurred
 *
 *    faultType [in]
 *      Whether fault was triggered by engine (_ENGINE_FAULTED) or PBDMA (_PBDMA_FAULTED)
 *      The value specified must be one of the NV2080_CTRL_FIFO_CLEAR_FAULTED_BIT_FAULT_TYPE_* values
 *
 *  Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_STATE
 *    NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT               (0x20801113) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_CLEAR_FAULTED_BIT_FAULT_TYPE_ENGINE 0x00000001
#define NV2080_CTRL_FIFO_CLEAR_FAULTED_BIT_FAULT_TYPE_PBDMA  0x00000002

#define NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT_PARAMS_MESSAGE_ID (0x13U)

typedef struct NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT_PARAMS {
    NvU32 engineType;
    NvU32 vChid;
    NvU32 faultType;
} NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT_PARAMS;



/*
 * NV2080_CTRL_CMD_FIFO_RUNLIST_SET_SCHED_POLICY
 *
 * Allows clients to set the global scheduling policy for all runlists
 * associated to the given subdevice.
 *
 * Currently, this is only supported for HW runlists.
 *
 * Since this is a global setting, only privileged clients will be allowed to
 * set it. Regular clients will get NV_ERR_INSUFFICIENT_PERMISSIONS error.
 *
 * Once a certain scheduling policy is set, that policy cannot be changed to a
 * different one unless all clients which set it have either restored the policy
 * (using the corresponding restore flag) or died. Clients trying to set a
 * policy while a different one is locked by another client will get a
 * NV_ERR_INVALID_STATE error.
 *
 * The same client can set a scheduling policy and later change to another one
 * only when no other clients have set the same policy. Such sequence will be
 * equivalent to restoring the policy in between.
 *
 * For instance, the following sequence:
 *
 *      1. Set policy A
 *      2. Set policy B
 *
 * is equivalent to:
 *
 *      1. Set policy A
 *      2. Restore policy
 *      3. Set policy B
 *
 * Parameters:
 *
 *   flags
 *     This field specifies the operational properties to be applied:
 *
 *      - NV2080_CTRL_CMD_FIFO_RUNLIST_SET_SCHED_POLICY_FLAGS_RESTORE_FALSE
 *          Try to set the provided 'schedPolicy' scheduling policy. If the
 *          operation succeeds, other clients will be prevented from setting a
 *          different scheduling policy until all clients using it have either
 *          restored it or died.
 *
 *      - NV2080_CTRL_CMD_FIFO_RUNLIST_SET_SCHED_POLICY_FLAGS_RESTORE_TRUE
 *          Let the scheduler know the client no longer requires the current
 *          scheduling policy. This may or may not actually change the
 *          scheduling policy, depending on how many other clients are also
 *          using the current policy.
 *
 *          The 'schedPolicy' parameter is ignored when this flag is set.
 *
 *   schedPolicy
 *     One of:
 *
 *      - NV2080_CTRL_FIFO_RUNLIST_SCHED_POLICY_DEFAULT
 *          Set the default scheduling policy and prevent other clients from
 *          changing it.
 *
 *      - NV2080_CTRL_FIFO_RUNLIST_SCHED_POLICY_CHANNEL_INTERLEAVED
 *          This scheduling policy will make channels to be scheduled according
 *          to their interleave level. See NVA06C_CTRL_CMD_SET_INTERLEAVE_LEVEL
 *          description for more details.
 *      - NV2080_CTRL_FIFO_RUNLIST_SCHED_POLICY_CHANNEL_INTERLEAVED_WDDM
 *          This scheduling policy will make channels to be scheduled according
 *          to their interleave level per WDDM policy.
 *          See NVA06C_CTRL_CMD_SET_INTERLEAVE_LEVEL description for more details.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_DEVICE
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FIFO_RUNLIST_SET_SCHED_POLICY                     (0x20801115) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_RUNLIST_SET_SCHED_POLICY_PARAMS_MESSAGE_ID" */

/* schedPolicy values */
#define NV2080_CTRL_FIFO_RUNLIST_SCHED_POLICY_DEFAULT                     0x0
#define NV2080_CTRL_FIFO_RUNLIST_SCHED_POLICY_CHANNEL_INTERLEAVED         0x1
#define NV2080_CTRL_FIFO_RUNLIST_SCHED_POLICY_CHANNEL_INTERLEAVED_WDDM    0x2

/* SET_SCHED_POLICY flags */
#define NV2080_CTRL_CMD_FIFO_RUNLIST_SET_SCHED_POLICY_FLAGS_RESTORE        0:0
#define NV2080_CTRL_CMD_FIFO_RUNLIST_SET_SCHED_POLICY_FLAGS_RESTORE_FALSE (0x00000000)
#define NV2080_CTRL_CMD_FIFO_RUNLIST_SET_SCHED_POLICY_FLAGS_RESTORE_TRUE  (0x00000001)

#define NV2080_CTRL_FIFO_RUNLIST_SET_SCHED_POLICY_PARAMS_MESSAGE_ID (0x15U)

typedef struct NV2080_CTRL_FIFO_RUNLIST_SET_SCHED_POLICY_PARAMS {
    NvU32 flags;
    NvU32 schedPolicy;
} NV2080_CTRL_FIFO_RUNLIST_SET_SCHED_POLICY_PARAMS;

/*
 *  NV2080_CTRL_CMD_FIFO_UPDATE_CHANNEL_INFO
 *
 *  This command updates the channel info params for an existing channel
 *
 *  Can be a deferred Api. The control call can be used for migrating a
 *
 *  channel to a new userd and gpfifo
 *
 *  Parameters:
 *     [in] hClient  - Client handle
 *     [in] hChannel - Channel handle
 *     [in] hUserdMemory  - UserD handle
 *     [in] gpFifoEntries - Number of Gpfifo Entries
 *     [in] gpFifoOffset  - Gpfifo Virtual Offset
 *     [in] userdOffset   - UserD offset
 *
 *
 *  Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_STATE
 *    NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FIFO_UPDATE_CHANNEL_INFO (0x20801116) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS_MESSAGE_ID (0x16U)

typedef struct NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS {
    NvHandle hClient;
    NvHandle hChannel;
    NvHandle hUserdMemory;
    NvU32    gpFifoEntries;
    NV_DECLARE_ALIGNED(NvU64 gpFifoOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 userdOffset, 8);
} NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_FIFO_DISABLE_USERMODE_CHANNELS
 *
 * This command will disable or enable scheduling of all usermode channels.
 *
 *  bDisable
 *      This value determines whether to disable or enable the usermode channels.
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FIFO_DISABLE_USERMODE_CHANNELS (0x20801117) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS_MESSAGE_ID (0x17U)

typedef struct NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS {
    NvBool bDisable;
} NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS;

/*
 * NV2080_CTRL_CMD_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB
 *
 * When a VF subcontext is marked as a zombie, host RM points its PDB to a dummy
 * page allocated by guest RM in GPA space. This command provides the parameters
 * of the guest RMs memory descriptor to be able to create a corresponding
 * memory descriptor on the host RM. Host RM uses this to program the PDB of a
 * zombie subcontext.
 *
 *  Parameters:
 *  Input parameters to describe the memory descriptor
 *     [in] base
 *     [in] size
 *     [in] addressSpace
 *     [in] cacheAttrib
 */
#define NV2080_CTRL_CMD_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB (0x20801118) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS_MESSAGE_ID (0x18U)

typedef struct NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 base, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NvU32 addressSpace;
    NvU32 cacheAttrib;
} NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS;

/*
 * NV2080_CTRL_CMD_FIFO_GET_ALLOCATED_CHANNELS
 *
 * Get's a bitmask of allocated channels. No guarantees are made about
 * synchronization. A channel returned as allocated by this ctrl cmd might have
 * already been destructed.
 *
 * Parameters:
 * [in]     runlistId
 * [in,out] bitMask   A 1 bit indicates that a channel with this index/id is
 *                    allocated. This field is a multiple of 32 bits and each 32
 *                    bit group must be accessed as a platform 32 bit int to
 *                    correctly map channel IDs.
 *
 */
#define NV2080_CTRL_CMD_FIFO_GET_ALLOCATED_CHANNELS          (0x20801119) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_GET_ALLOCATED_CHANNELS_PARAMS_MESSAGE_ID" */

/*
 * The maximum number than can be returned by
 * NV2080_CTRL_CMD_INTERNAL_FIFO_GET_NUM_CHANNELS
 */
#define NV2080_CTRL_FIFO_GET_ALLOCATED_CHANNELS_MAX_CHANNELS 4096

#define NV2080_CTRL_FIFO_GET_ALLOCATED_CHANNELS_PARAMS_MESSAGE_ID (0x19U)

typedef struct NV2080_CTRL_FIFO_GET_ALLOCATED_CHANNELS_PARAMS {
    NvU32 runlistId;
    NvU32 bitMask[NV2080_CTRL_FIFO_GET_ALLOCATED_CHANNELS_MAX_CHANNELS / 32];
} NV2080_CTRL_FIFO_GET_ALLOCATED_CHANNELS_PARAMS;

/*
 * NV2080_CTRL_CMD_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION
 *
 * This command will disable and preempt channels described in the
 * list provided and mark them ready for key rotation.
 * hClient <-> hChannel pairs should use the same index in the arrays.
 *
 *  numChannels
 *      The number of valid entries in hChannelList array.
 *  hClientList
 *      An array of NvHandle listing the client handles
 *  hChannelList
 *      An array of NvHandle listing the channel handles
 *      to be stopped.
 *  bEnableAfterKeyRotation
 *      This determines if channel is enabled by RM after it completes key rotation.
 * Possible status values returned are:
 *    NV_OK
 *    NVOS_INVALID_STATE
 */
#define NV2080_CTRL_CMD_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION         (0x2080111a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_MAX_ENTRIES (64)

#define NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_PARAMS_MESSAGE_ID (0x1AU)

typedef struct NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_PARAMS {
    NvU32    numChannels;
    NvHandle hClientList[NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_MAX_ENTRIES];
    NvHandle hChannelList[NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_MAX_ENTRIES];
    NvBool   bEnableAfterKeyRotation;
} NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_PARAMS;

/*
 * NV2080_CTRL_CMD_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_V2
 *
 * This command does the same thing as @ref NV2080_CTRL_CMD_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION.
 * The difference is that it doesn't take a list of clients and instead all channels belong
 * to the client on which this control call is made.
 *
 *  numChannels
 *      The number of valid entries in hChannelList array.
 *  hChannelList
 *      An array of NvHandle listing the channel handles
 *      to be stopped.
 *  bEnableAfterKeyRotation
 *      This determines if channel is enabled by RM after it completes key rotation.
 * Possible status values returned are:
 *    NV_OK
 *    NVOS_INVALID_STATE
 */
#define NV2080_CTRL_CMD_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_V2 (0x2080111b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_V2_PARAMS_MESSAGE_ID (0x1BU)

typedef struct NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_V2_PARAMS {
    NvU32    numChannels;
    NvHandle hChannelList[NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_MAX_ENTRIES];
    NvBool   bEnableAfterKeyRotation;
} NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_V2_PARAMS;



/*
 * NV2080_CTRL_CMD_FIFO_OBJSCHED_GET_STATE
 *
 * This command returns the vGPU schedular state.
 *
 *   engineId
 *     This field specifies the NV2080_ENGINE_TYPE_* engine whose SW runlist log
 *     entries are to be fetched.
 *
 *   schedPolicy
 *     This field returns the runlist scheduling policy. It specifies the
 *     NV2080_CTRL_CMD_VGPU_SCHEDULER_POLICY_* scheduling policy.
 *
 *   arrEnabled
 *     This field returns if Adaptive round robin scheduler
 *     is enabled/disabled.
 *
 *  targetTimesliceNs
 *      This field returns the target timeslice duration in ns for each SW runlist
 *      as configured by the user or the default value otherwise.
 *
 *   arrAvgFactor
 *     This field returns the average factor to be used in compensating the timeslice
 *     for Adaptive scheduler mode.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV2080_CTRL_CMD_FIFO_OBJSCHED_GET_STATE (0x20801120) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_OBJSCHED_GET_STATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_OBJSCHED_GET_STATE_PARAMS_MESSAGE_ID (0x20U)

typedef struct NV2080_CTRL_FIFO_OBJSCHED_GET_STATE_PARAMS {
    NvU32 engineId;
    NvU32 schedPolicy;
    NvU32 arrEnabled;
    NvU32 targetTimesliceNs;
    NvU32 arrAvgFactor;
} NV2080_CTRL_FIFO_OBJSCHED_GET_STATE_PARAMS;

/*
 * NV2080_CTRL_CMD_FIFO_OBJSCHED_SET_STATE
 *
 * This command set the vGPU schedular state.
 *
 *  engineId
 *     This field specifies the NV2080_ENGINE_TYPE_* engine.
 *
 *  schedPolicy
 *    This field sets the runlist scheduling policy. It specifies the
 *    NV2080_CTRL_CMD_VGPU_SCHEDULER_POLICY_* scheduling policy.
 *
 *  enableArr
 *    This field sets the Adaptive round robin scheduler
 *    is enabled/disabled.
 *
 *  timesliceTargetNs
 *    This field sets the time slice target time in ns.
 *
 *  frequencyForARR
 *    This field sets the scheduling frequency for Adaptive round robin scheduler mode.
 *
 *  avgFactorForARR
 *    This field sets the average factor to be used in compensating the timeslice
 *    for Adaptive scheduler mode.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_DEVICE
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_PARAM_STRUCT
 */

#define NV2080_CTRL_CMD_FIFO_OBJSCHED_SET_STATE (0x20801121) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_OBJSCHED_SET_STATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_OBJSCHED_SET_STATE_PARAMS_MESSAGE_ID (0x21U)

typedef struct NV2080_CTRL_FIFO_OBJSCHED_SET_STATE_PARAMS {
    NvU32 engineId;
    NvU32 schedPolicy;
    NvU32 enableArr;
    NvU32 timesliceTargetNs;
    NvU32 frequencyForARR;
    NvU32 avgFactorForARR;
} NV2080_CTRL_FIFO_OBJSCHED_SET_STATE_PARAMS;

/*
 * NV2080_CTRL_CMD_FIFO_OBJSCHED_GET_CAPS
 *
 * This command returns the vGPU schedular capabilities.
 *
 *   engineId [in]
 *     This field specifies the NV2080_ENGINE_TYPE_* engine whose SW runlist log
 *     entries are to be fetched.
 *
 *   supportedSchedulers [out]
 *     This field returns the supported runlist scheduling policies on the device.
 *     It specifies the NV2080_CTRL_CMD_VGPU_SCHEDULER_POLICY_* scheduling policy.
 *
 *   bIsArrModeSupported [out]
 *     This field returns if Adaptive scheduler mode is enabled/disabled.
 *
 *   maxTimesliceNs [out]
 *      This field returns the maximum time slice value in ns.
 *
 *   minTimesliceNs [out]
 *     This field returns the minimum time slice value in ns.
 *
 *   maxFrequencyForARR [out]
 *     This field returns the maximum scheduling frequency for
 *     Adaptive round robin scheduler mode.
 *
 *   minFrequencyForARR [out]
 *     This field returns the minimum scheduling frequency for
 *     Adaptive round robin scheduler mode.
 *
 *   maxAvgFactorForARR [out]
 *     This field returns the maximum average factor in compensating
 *     the timeslice for Adaptive scheduler mode.
 *
 *   minAvgFactorForARR [out]
 *     This field returns the minimum average factor in compensating
 *     the timeslice for Adaptive scheduler mode.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV2080_CTRL_CMD_FIFO_OBJSCHED_GET_CAPS (0x20801122) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_OBJSCHED_GET_CAPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_OBJSCHED_GET_CAPS_PARAMS_MESSAGE_ID (0x22U)

typedef struct NV2080_CTRL_FIFO_OBJSCHED_GET_CAPS_PARAMS {
    NvU32  engineId;
    NvU32  supportedSchedulers[NV2080_CTRL_CMD_SUPPORTED_VGPU_SCHEDULER_POLICY_COUNT];
    NvBool bIsArrModeSupported;
    NvU32  maxTimesliceNs;
    NvU32  minTimesliceNs;
    NvU32  maxFrequencyForARR;
    NvU32  minFrequencyForARR;
    NvU32  maxAvgFactorForARR;
    NvU32  minAvgFactorForARR;
} NV2080_CTRL_FIFO_OBJSCHED_GET_CAPS_PARAMS;

// Max channels per group is limited by NV_RAMRL_ENTRY_TSG_LENGTH_MAX for the arch.
#define NV2080_CTRL_CMD_FIFO_MAX_CHANNELS_PER_TSG             128

/*
 * NV2080_CTRL_CMD_FIFO_GET_CHANNEL_GROUP_UNIQUE_ID_INFO
 *   hClient
 *      Input parameter
 *      This parameter specifies the client handle associated input channel/TSG
 *   hChannelOrTsg
 *     Input parameter.
 *     This parameter specifies the handle of input channel handle (or channel
 *     group)
 *   tsgId
 *     Output parameter.
 *     This field return the Unique of TSG object if user specified a channel group handle
 *     with hChannelOrTsg.
 *   numChannels
 *     Output parameter.
 *     This field return the number of channels under TSG if user specify a
 *     channel group handle or return 1 if user specify a channel handle.
 *   channelUniqueID
 *     Output parameter.
 *     This array field returns unique Channel ID for each channel.
 *   vasUniqueID
 *     Output parameter.
 *     This array field returns unique IDs of VA Space objects of channels under TSG or channel.
 *   veid
 *     Output parameter.
 *     This array field returns VEID for channels under TSG or channel.
 */
#define NV2080_CTRL_CMD_FIFO_GET_CHANNEL_GROUP_UNIQUE_ID_INFO (0x20801123) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_GET_CHANNEL_GROUP_UNIQUE_ID_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_GET_CHANNEL_GROUP_UNIQUE_ID_INFO_PARAMS_MESSAGE_ID (0x23U)

typedef struct NV2080_CTRL_FIFO_GET_CHANNEL_GROUP_UNIQUE_ID_INFO_PARAMS {
    NvHandle hClient;
    NvHandle hChannelOrTsg;
    NvU32    tsgId;
    NvU32    numChannels;
    NvU32    channelUniqueID[NV2080_CTRL_CMD_FIFO_MAX_CHANNELS_PER_TSG];
    NvU32    vasUniqueID[NV2080_CTRL_CMD_FIFO_MAX_CHANNELS_PER_TSG];
    NvU32    veid[NV2080_CTRL_CMD_FIFO_MAX_CHANNELS_PER_TSG];
} NV2080_CTRL_FIFO_GET_CHANNEL_GROUP_UNIQUE_ID_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_FIFO_QUERY_CHANNEL_UNIQUE_ID
 * This command is used query the CID (channel ID) in batch
 *   hClients
 *      Input parameter
 *      Array of Client handles
 *   hChannels
 *      Input parameter
 *      Array of Channel handles
 *   numChannels
 *      Indicates the number of input client, channel handle pairs.
 *   channelUniqueIDs
 *     Output parameter.
 *     This parameter returns an array of unique Channel IDs for each input pair.
 *     channel handles.
 */
#define NV2080_CTRL_CMD_FIFO_QUERY_CHANNEL_UNIQUE_ID (0x20801124) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FIFO_INTERFACE_ID << 8) | NV2080_CTRL_FIFO_QUERY_CHANNEL_UNIQUE_ID_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FIFO_QUERY_CHANNEL_UNIQUE_ID_PARAMS_MESSAGE_ID (0x24U)

typedef struct NV2080_CTRL_FIFO_QUERY_CHANNEL_UNIQUE_ID_PARAMS {
    NvHandle hClients[NV2080_CTRL_CMD_FIFO_MAX_CHANNELS_PER_TSG];
    NvHandle hChannels[NV2080_CTRL_CMD_FIFO_MAX_CHANNELS_PER_TSG];
    NvU32    numChannels;
    NvU32    channelUniqueIDs[NV2080_CTRL_CMD_FIFO_MAX_CHANNELS_PER_TSG];
} NV2080_CTRL_FIFO_QUERY_CHANNEL_UNIQUE_ID_PARAMS;
/* _ctrl2080fifo_h_ */
