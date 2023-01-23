/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl30f1.finn
//

#include "ctrl/ctrlxxxx.h"
/* NV30_GSYNC_CTRL control commands and parameters */

#define NV30F1_CTRL_CMD(cat,idx)             NVXXXX_CTRL_CMD(0x30F1, NV30F1_CTRL_##cat, idx)

/* Command categories (6bits) */
#define NV30F1_CTRL_RESERVED (0x00)
#define NV30F1_CTRL_GSYNC    (0x01)

/*
 * NV30F1_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV30F1_CTRL_CMD_NULL (0x30f10000) /* finn: Evaluated from "(FINN_NV30_GSYNC_RESERVED_INTERFACE_ID << 8) | 0x0" */





/* NV30F1_CTRL_GSYNC 
 *
 * Gsync board access/control functionality.
 *
 */

/*
 * NV30F1_CTRL_CMD_GSYNC_GET_VERSION
 *
 * This command will get the current gsync api version info.
 *
 * version
 *   The api's major version. Does not change often.
 * 
 * revision
 *   The api's minor version.
 *   Bumped with each change, no matter how minor.
 *
 * Possible status values returned are:
 *   NV_OK
 *
 */
#define NV30F1_CTRL_CMD_GSYNC_GET_VERSION (0x30f10101) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_GET_VERSION_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_GET_VERSION_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV30F1_CTRL_GSYNC_GET_VERSION_PARAMS {
    NvU32 version;
    NvU32 revision;
} NV30F1_CTRL_GSYNC_GET_VERSION_PARAMS;

#define NV30F1_CTRL_GSYNC_API_VER                1
#define NV30F1_CTRL_GSYNC_API_REV                0

/*
 * NV30F1_CTRL_GSYNC api
 *
 * The gsync interface provides access to gsync devices in the system.
 *
 * There are  commands:
 *   NV30F1_CTRL_CMD_GSYNC_GET_STATUS_SIGNALS 
 *     Status on input sync signals.
 *   NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_PARAMS
 *     Get gsync parameters.
 *   NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_PARAMS
 *     Get gsync parameters.
 *   NV30F1_CTRL_CMD_GSYNC_GET_INFO_CAPS
 *     Get basic info about the device and its connected displays
 *   NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_SYNC
 *     Enable frame sync.
 *   NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_UNSYNC
 *     Disable frame sync.
 *   NV30F1_CTRL_CMD_GSYNC_GET_STATUS
 *     Get status info relevant for the control panel
 *   NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_TESTING
 *     Test signal enabling/disabling
 *   NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_WATCHDOG
 *     Control the gsync watchdog
 *   NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_INTERLACE_MODE
 *     Set the interlace mode
 * 
 */


/*
 * NV30F1_CTRL_CMD_GSYNC_GET_STATUS_SIGNALS
 *
 * This command returns information associated with incoming signals to the
 * gsync device.
 *
 * RJ45
 *   This parameter contains the signal information for each of the two RJ45
 *   ports on the gsync device. A value of ~0 indicates that a signal is 
 *   detected, but no rate information is available. Anything else is a rate 
 *   in units of 10e-4 Hz.
 * house
 *   This parameter contains the signal information for the house sync signal
 *   (i.e. the bnc port). A value of 0 means that no signal is present. A value
 *   of ~0 indicates that a signal is detected, but no rate information is 
 *   available. Anything else is a rate in units of 10e-4 Hz.
 * rate
 *   A mask representing signals for which we would like rate information (if 
 *   available).
 *
 * Possible status values returned are
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   
 */

#define NV30F1_CTRL_CMD_GSYNC_GET_STATUS_SIGNALS (0x30f10102) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_PARAMS {
    NvU32 RJ45[2];
    NvU32 house;
    NvU32 rate;
} NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_PARAMS;

/*
 * rate values 
 *
 */

#define NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_RJ45_0 (0x00000001)
#define NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_RJ45_1 (0x00000002)
#define NV30F1_CTRL_GSYNC_GET_SIGNALS_HOUSE         (0x00000004)


/*
 * NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_PARAMS
 * NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_PARAMS
 *
 * These commands respectively return and set state related to the operation
 * of the gsync device.
 *
 * which
 *   This value is a mask set by the client representing which parameters are 
 *   to be updated. In the case of a GET command, these parameters will
 *   be used to change the state of the hardware. For both a GET
 *   command and a SET command, the hardware state will be returned by
 *   the RM in the respective parameter. All other parameters are untouched.
 * syncPolarity
 *   This parameter specifies which edge of the house sync signal to sync with.
 * videoMode
 *   This parameter specifies which video mode to use to decode the house sync
 *   signal.
 * nSync
 *   This parameter specifies the number of pulses to wait between frame lock
 *   signal generation. 0 indicates that every incomming pulse should result in
 *   a frame lock sync pulse being generated (i.e. the input and output rate 
 *   matches).
 * syncSkew
 *   This parameter specifies the time delay between the frame sync signal and
 *   the GPUs signal in units of 0.977 us. Maximum value for SyncSkew is defined
 *   in respective header files of gsync boards. e.g. For P2060 board value
 *   is defined in drivers/resman/kernel/inc/dacp2060.h
 * syncStartDelay
 *   In master mode, the amount of time to wait before generating the first
 *   sync pulse in units of 7.81 us, max 512 ms (i.e 65535 units).
 * useHouseSync
 *   When a house sync signal is detected, this parameter indicates that it 
 *   should be used as the reference to generate the frame sync signal.
 *   
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_GENERIC
 */

#define NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_PARAMS    (0x30f10103) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_PARAMS    (0x30f10104) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS {
    NvU32 which;
    NvU32 syncPolarity;
    NvU32 syncVideoMode;
    NvU32 nSync;
    NvU32 syncSkew;
    NvU32 syncStartDelay;
    NvU32 useHouseSync;
} NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS;
#define NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS_MESSAGE_ID (0x4U)

typedef NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS;

/*
 * which values
 *
 */

#define NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_POLARITY              0x0001
#define NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE                 0x0002
#define NV30F1_CTRL_GSYNC_SET_CONTROL_NSYNC                      0x0004
#define NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_SKEW                  0x0008
#define NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_START_DELAY           0x0010
#define NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_USE_HOUSE             0x0020

#define NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_POLARITY              NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_POLARITY
#define NV30F1_CTRL_GSYNC_GET_CONTROL_VIDEO_MODE                 NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE
#define NV30F1_CTRL_GSYNC_GET_CONTROL_NSYNC                      NV30F1_CTRL_GSYNC_SET_CONTROL_NSYNC
#define NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_SKEW                  NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_SKEW
#define NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_START_DELAY           NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_START_DELAY
#define NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_USE_HOUSE             NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_USE_HOUSE

/* 
 * syncPolarity values
 *
 */

#define NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_POLARITY_RISING_EDGE  0
#define NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_POLARITY_FALLING_EDGE 1
#define NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_POLARITY_BOTH_EDGES   2

/*
 * syncVideoMode values
 * Video_Mode_Composite is valid for P2060 only.
 *
 */

#define NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_NONE            0
#define NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_TTL             1
#define NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_NTSCPALSECAM    2
#define NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_HDTV            3

#define NV30F1_CTRL_GSYNC_GET_CONTROL_VIDEO_MODE_NONE            NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_NONE
#define NV30F1_CTRL_GSYNC_GET_CONTROL_VIDEO_MODE_TTL             NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_TTL
#define NV30F1_CTRL_GSYNC_GET_CONTROL_VIDEO_MODE_NTSCPALSECAM    NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_NTSCPALSECAM
#define NV30F1_CTRL_GSYNC_GET_CONTROL_VIDEO_MODE_HDTV            NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_HDTV
#define NV30F1_CTRL_GSYNC_GET_CONTROL_VIDEO_MODE_COMPOSITE       4

/*
 * NV30F1_CTRL_CMD_GSYNC_GET_CAPS
 *
 * This command returns the capabilities of this gsync device.
 *
 * revId
 *  This parameter is set by the RM to indicate the combined 
 *  FPGA revision (low 4 bits) and board ID (high 4 bits).
 * 
 * boardId
 *  This parameter is set by the RM to indicate the board ID,
 *  allowing disambiguation of P2060 and so forth.
 *
 * minRevRequired
 *  This parameter is set by the RM to indicate the minimum
 *  Qsync FPGA revision required for a specific CHIP Familiy
 *
 * isFirmwareRevMismatch
 * This parameter is set to TRUE by RM when the Qsync Firmware
 * Revision is incompatibled with the connected GPU chip family.
 *
 * revision
 *  This parameter is set by the RM to indicate the device revision,
 *  also known as major version.
 *
 * extendedRevision
 *  This parameter is set by the RM to indicate the device extended
 *  revision, also known as minor version.
 *
 * capFlags
 *  This parameter is set by the RM to indicate capabilities of
 *  the board, preventing the client from needing to keep track
 *  of the feature lists supported by each revision of each board.
 *
 * maxSyncSkew
 *  This parameter returns that maximum units of sync skew the 
 *  board supports. The value prgrammed into the board has to be
 *  between 0 and maxSyncSkew, inclusive. The value of each unit
 *  can be learned from the syncSkewResolution parameter.
 * 
 * syncSkewResolution
 *  This parameter returns the number of nanoseconds that one unit
 *  of sync skew corresponds to.
 *
 * maxStartDelay
 *  This parameter returns that maximum units of sync start delay
 *  the board supports. The value prgrammed into the board has to be
 *  between 0 and maxStartDelay, inclusive. The value of each unit
 *  can be learned from the startDelayResolution parameter.
 * 
 * startDelayResolution
 *  This parameter returns the number of nanoseconds that one unit
 *  of sync start delay corresponds to.
 *
 * maxSyncInterval
 *  This parameter returns the maximum duration of house sync interval
 *  between frame lock sync cycle that the board supports. The value
 *  programmed into the board has to be between 0 and maxSyncInterval,
 *  inclusive.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV30F1_CTRL_CMD_GSYNC_GET_CAPS                           (0x30f10105) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS {
    NvU32  revId;
    NvU32  boardId;
    NvU32  minRevRequired;
    NvBool isFirmwareRevMismatch;
    NvU32  revision;
    NvU32  extendedRevision;
    NvU32  capFlags;
    NvU32  maxSyncSkew;
    NvU32  syncSkewResolution;
    NvU32  maxStartDelay;
    NvU32  startDelayResolution;
    NvU32  maxSyncInterval;
} NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS;

#define NV30F1_CTRL_GSYNC_GET_CAPS_BOARD_ID_P2060                         (0x00002060)
#define NV30F1_CTRL_GSYNC_GET_CAPS_BOARD_ID_P2061                         (0x00002061)

#define NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_FREQ_ACCURACY_2DPS           (0x00000001)
#define NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_FREQ_ACCURACY_3DPS           (0x00000002)
#define NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_FREQ_ACCURACY_4DPS           (0x00000004)

#define NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_NEED_MASTER_BARRIER_WAR      (0x00000010)

#define NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_SYNC_LOCK_EVENT              (0x10000000)
#define NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_HOUSE_SYNC_EVENT             (0x20000000)
#define NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_FRAME_COUNT_EVENT            (0x40000000)

#define NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_ONLY_PRIMARY_CONNECTOR_EVENT (0x01000000)
#define NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_ALL_CONNECTOR_EVENT          (0x02000000)

// For P2060, clients can only request for video modes at BNC connector
// e.g. NO HS, TTL and Composite etc.
#define NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_ONLY_GET_VIDEO_MODE          (0x00100000)

/*
 * NV30F1_CTRL_CMD_GET_GSYNC_GPU_TOPOLOGY
 * 
 * This command returns the list of GPU IDs connected with the associated
 * gsync device.
 *
 * gpus
 *   This array is set by RM to contain the gpu connection information
 *   for gpus attached to the gsync device. Valid entries are contiguous,
 *   beginning with the first entry in the list. The elements of this array contain
 *   the following fields:
 *   gpuId
 *     This field contains the ID of the connected GPU. If the entry in the 
 *     table is invalid, this fields contains  NV30F1_CTRL_GPU_INVALID_ID.
 *   connector
 *     This field indicates which connector on the device the GPU is connected
 *     to (i.e. the primary or secondary connector), if any.
 *   proxyGpuId
 *     If the 'connector' field indicates that the GPU is not connected to
 *     a G-Sync device directly, then this field contains the ID of the
 *     GPU that acts as a proxy, i.e. the GPU to which this GPU should be
 *     a RasterLock slave.
 * connectorCount
 *    This parameter indicates the number of GPU connectors available on
 *    the gsync device.  The connector count of the gsync device may be
 *    less than NV30F1_CTRL_MAX_GPUS_PER_GSYNC.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV30F1_CTRL_CMD_GET_GSYNC_GPU_TOPOLOGY                            (0x30f10106) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_MAX_GPUS_PER_GSYNC                                    4
#define NV30F1_CTRL_GPU_INVALID_ID                                        (0xffffffff)

#define NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS {
    struct {
        NvU32 gpuId;
        NvU32 connector;
        NvU32 proxyGpuId;
    } gpus[NV30F1_CTRL_MAX_GPUS_PER_GSYNC];
    NvU32 connectorCount;
} NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS;

/*
 * connector values
 *
 */
#define NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_ONE                                           1
#define NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_TWO                                           2
#define NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_THREE                                         3
#define NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_FOUR                                          4

#define NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_NONE                                          0
#define NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PRIMARY                                       NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_ONE
#define NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_SECONDARY                                     NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_TWO




/*
 * NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_SYNC
 *
 * This command enables frame sync on displays.
 *
 * gpuId
 *   The parameter is set by the client to indicate the gpuId on which
 *   frame lock will be enabled.
 * master
 *   This parameter is set by the client to specify whether this/these 
 *   displays should be set as the master or as slaves. If this is a GET
 *   and displays is not 0, this will be set by the RM to indicate if
 *   the display can be the master.
 * displays
 *   This is a device mask set by the client to indicate which display(s)
 *   are to be synched. Note that only one display may be set as master.
 *   If this is a GET, this set by the client to indicate which display
 *   is to be queried. If the display cannot be synched to this device, 
 *   the RM will overwrite the mask with a 0.
 * validateExternal
 *   This parameter is set by the client to tell the RM to validate the
 *   presence of an external sync source when enabling a master.
 * refresh
 *   This parameter is set by the client to indicate the desired refresh rate
 *   The value is in 0.0001 Hertz (i.e. it has been multiplied by 10000).
 * configFlags
 *   contains flags for specific options. So far only 
 *   NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_CONFIG_FLAGS_KEEP_MASTER_SWAPBARRIER_DISABLED
 *   is supported which allows the caller to prevent the rm code to automatically
 *   enable the swapbarrier on framelock masters on fpga revisions <= 5.
 * 
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 * 
 */
#define NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_SYNC                                           (0x30f10110) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_SYNC                                           (0x30f10111) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_PARAMS_MESSAGE_ID" */

// If set the swapbarrier is not enable automatically when enablign a framelock master on fpga revs <= 5.
#define NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_CONFIG_FLAGS_KEEP_MASTER_SWAPBARRIER_DISABLED (0x00000001)

#define NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS_MESSAGE_ID (0x10U)

typedef struct NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS {
    NvU32 gpuId;
    NvU32 master;
    NvU32 displays;
    NvU32 validateExternal;
    NvU32 refresh;
    NvU32 configFlags;
} NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS;
#define NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_PARAMS_MESSAGE_ID (0x11U)

typedef NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_PARAMS;

/* 
 * NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_UNSYNC
 *
 * This command disables frame sync on displays
 *
 * gpuId
 *   The parameter is set by the client to indicate the gpuId on which
 *   frame lock will be disabled.
 * master
 *   This parameter is set by the client to specify whether this/these
 *   display(s) to be unset is a master/are slaves.
 * displays
 *   This is a device mask set by the client to indicate which display(s)
 *   are to be unsynched.
 * retainMaster
 *   Retain the designation as master, but unsync the displays. 
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_UNSYNC (0x30f10112) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS_MESSAGE_ID (0x12U)

typedef struct NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS {
    NvU32 gpuId;
    NvU32 master;
    NvU32 displays;
    NvU32 retainMaster;
} NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS;

/*
 * NV30F1_CTRL_CMD_GSYNC_GET_STATUS_SYNC
 *
 * This command gets the sync state for the gpus attached to the
 * framelock device. Note that the frame lock device only has
 * knowledge of sync status at the gpu level, not the display
 * device level.
 *
 * gpuId
 *   The parameter is set by the client to indicate which gpuId is to be
 *   queried.
 * bTiming
 *   This parameter is set by the RM to indicate that timing on the GPU is
 *   in sync with the master sync signal.
 * bStereoSync 
 *   This parameter is set by the RM to indicate whether the phase of the
 *   timing signal coming from the GPU is the same as the phase of the 
 *   master sync signal.
 * bSyncReady
 *   This parameter is set by the RM to indicate if a sync signal has
 *   been detected.
 * 
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 *
 */

#define NV30F1_CTRL_CMD_GSYNC_GET_STATUS_SYNC (0x30f10113) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS_MESSAGE_ID (0x13U)

typedef struct NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS {
    NvU32 gpuId;
    NvU32 bTiming;
    NvU32 bStereoSync;
    NvU32 bSyncReady;
} NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS;


/*
 * NV30F1_CTRL_CMD_GSYNC_GET_STATUS
 *
 * This command gets status information for the frame lock device 
 * relevant to a control panel.
 *
 * which
 *   This is a mask set by the client describing which of the other 
 *   parameters we should collect status information for.
 * bLeadingEdge
 *   This parameter is set by the RM to indicate that the gsync device is
 *   set to sync to the leading edge of a house sync signal. Note that
 *   this does not mean that house sync is the signal source.
 * bFallingEdge
 *   This parameter is set by the RM to indicate that the gsync device is
 *   set to sync to the falling edge of a house sync signal. Note that
 *   this does not mean that house sync is the signal source.
 * syncDelay
 *   This parameter is set by the RM to indicate the sync delay in 
 *   microseconds,
 * refresh
 *   This parameter is set by the RM to indicate the rate of frame sync pulse in
 *   0.0001 Hertz (i.e. it has been multiplied by 10000). This is not the refresh
 *   rate of display device. This is same as incoming house sync rate if
 *   framelocked to an external house sync signal. Otherwise, this is same
 *   as the refresh rate of the master display device.
 * houseSyncIncomming
 *   This parameter is set by the RM to indicate the rate of an incomming 
 *   house sync signal in 0.0001 Hertz (i.e. it has been multiplied by 10000).
 * syncInterval
 *   This parameter is set by the RM to indicate the number of incoming 
 *   sync pulses to wait before the generation of the frame sync pulse.
 * bSyncReady
 *   This paramater is set by the RM to indicate if a sync signal has 
 *   been detected (this parameter is also available from the 
 *   NV30F1_CTRL_CMD_GSYNC_GET_STATUS_SYNC method).
 * bSwapReady
 *   This paramater is set by the RM to indicate if the hardware is 
 *   ready to swap. 
 * bHouseSync
 *   This parameter is set by the RM to indicate that a house sync signal
 *   should be used as the source signal if it is available.
 * bPort0Input
 *   This parameter is set by the RM to indicate that RJ45 port 0 is 
 *   configured as an input.
 * bPort1Input
 *   This parameter is set by the RM to indicate that RJ45 port 1 is
 *   configured as an input
 * bPort0Ehternet
 *   This parameter is set by the RM to indicate that RJ45 port 0 has
 *   been connected to an ethernet hub (this is not the right thing to do).
 * bPort1Ehternet
 *   This parameter is set by the RM to indicate that RJ45 port 1 has
 *   been connected to an ethernet hub (this is not the right thing to do).
 * universalFrameCount
 *   This parameter is set by the RM to indicate the value of the 
 *   Universal frame counter.
 * bInternalSlave
 *   This parameter is set by the RM to indicate that a p2061 has been
 *   configured as internal slave.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV30F1_CTRL_CMD_GSYNC_GET_STATUS (0x30f10114) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS_MESSAGE_ID (0x14U)

typedef struct NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS {
    NvU32 which;
    NvU32 bLeadingEdge;
    NvU32 bFallingEdge;
    NvU32 syncDelay;
    NvU32 refresh;
    NvU32 houseSyncIncoming;
    NvU32 syncInterval;
    NvU32 bSyncReady;
    NvU32 bSwapReady;
    NvU32 bHouseSync;
    NvU32 bPort0Input;
    NvU32 bPort1Input;
    NvU32 bPort0Ethernet;
    NvU32 bPort1Ethernet;
    NvU32 universalFrameCount;
    NvU32 bInternalSlave;
} NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS;

/*
 * which values
 *
 */

#define NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_POLARITY         (0x00000001)
#define NV30F1_CTRL_GSYNC_GET_STATUS_LEADING_EDGE          (0x00000001)
#define NV30F1_CTRL_GSYNC_GET_STATUS_FALLING_EDGE          (0x00000001)
#define NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_DELAY            (0x00000002)
#define NV30F1_CTRL_GSYNC_GET_STATUS_REFRESH               (0x00000004)
#define NV30F1_CTRL_GSYNC_GET_STATUS_HOUSE_SYNC_INCOMING   (0x00000008)
#define NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_INTERVAL         (0x00000010)
#define NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_READY            (0x00000020)
#define NV30F1_CTRL_GSYNC_GET_STATUS_SWAP_READY            (0x00000040)
#define NV30F1_CTRL_GSYNC_GET_STATUS_TIMING                (0x00000080)
#define NV30F1_CTRL_GSYNC_GET_STATUS_STEREO_SYNC           (0x00000100)
#define NV30F1_CTRL_GSYNC_GET_STATUS_HOUSE_SYNC            (0x00000200)
#define NV30F1_CTRL_GSYNC_GET_STATUS_PORT_INPUT            (0x00000400)
#define NV30F1_CTRL_GSYNC_GET_STATUS_PORT0_INPUT           (0x00000400)
#define NV30F1_CTRL_GSYNC_GET_STATUS_PORT1_INPUT           (0x00000400)
#define NV30F1_CTRL_GSYNC_GET_STATUS_PORT_ETHERNET         (0x00000800)
#define NV30F1_CTRL_GSYNC_GET_STATUS_PORT0_ETHERNET        (0x00000800)
#define NV30F1_CTRL_GSYNC_GET_STATUS_PORT1_ETHERNET        (0x00000800)
#define NV30F1_CTRL_GSYNC_GET_STATUS_UNIVERSAL_FRAME_COUNT (0x00001000)
#define NV30F1_CTRL_GSYNC_GET_STATUS_INTERNAL_SLAVE        (0x00002000)

/*
 * NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_TESTING
 *
 * This command controls the test signal on the gsync device.
 *
 * bEmitTestSignal
 *   This parameter is set by the client to emit or stop emitting the test
 *   signal.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_TESTING          (0x30f10120) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_GET_CONTROL_TESTING_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_TESTING          (0x30f10121) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS_MESSAGE_ID (0x21U)

typedef struct NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS {
    NvU32 bEmitTestSignal;
} NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS;
#define NV30F1_CTRL_GSYNC_GET_CONTROL_TESTING_PARAMS_MESSAGE_ID (0x20U)

typedef NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS NV30F1_CTRL_GSYNC_GET_CONTROL_TESTING_PARAMS;

/*
 * NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_WATCHDOG
 *
 * This command enables and disables the gsync watchdog
 *
 * enable
 *   This parameter is set by the client to enable or disable the
 *   gsync watchdog.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_WATCHDOG (0x30f10130) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS_MESSAGE_ID (0x30U)

typedef struct NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS {
    NvU32 enable;
} NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS;


/*
 * NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_INTERLACE_MODE
 *
 * This command enables or disables interlace mode.
 *
 * enable
 *   This parameter is set by the client to enable or disable
 *   interlace mode
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_INTERLACE_MODE (0x30f10140) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_GET_CONTROL_INTERLACE_MODE_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_INTERLACE_MODE (0x30f10141) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS_MESSAGE_ID (0x41U)

typedef struct NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS {
    NvU32 enable;
} NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS;
#define NV30F1_CTRL_GSYNC_GET_CONTROL_INTERLACE_MODE_PARAMS_MESSAGE_ID (0x40U)

typedef NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS NV30F1_CTRL_GSYNC_GET_CONTROL_INTERLACE_MODE_PARAMS;

/*
 *
 * NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_SWAP_BARRIER
 * NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_SWAP_BARRIER
 *
 * These commands enables or disables the swap barrier
 * connection between a GPU and the rest of the gsync
 * network
 *
 * gpuId
 *   The parameter is set by the client to indicate which gpuId is to be
 *   queried.
 * enable
 *   In a set command, this parameter is set by the client to 
 *   indicate if the barrier should be enabled (i.e. connected 
 *   to the rest of the network) or disabled (disconnected).
 *   In both a set and a get command, if successful, the RM
 *   uses this parameter to return the current (i.e. post-set)
 *   value.
 *   
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_SWAP_BARRIER (0x30f10150) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_BARRIER_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_SWAP_BARRIER (0x30f10151) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS_MESSAGE_ID (0x51U)

typedef struct NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS {
    NvU32  gpuId;
    NvBool enable;
} NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS;
#define NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_BARRIER_PARAMS_MESSAGE_ID (0x50U)

typedef NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_BARRIER_PARAMS;

/*
 * NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW
 *
 * This command allow clients to obtain time period for which SwapLock window will
 * remain HIGH for GSYNC III (P2060) i.e. TswapRdyHi. RM clients will use this value
 * for programming SWAP_LOCKOUT_START on all heads of GPU connected to P2060.
 *
 * tSwapRdyHi
 *   RM will return swap lock window High time period in this variable. By default
 *   tSwapRdyHi is 250 micro seconds. RM also provide regkey to change this value.
 *   tSwapRdyHi also used by RM to configure value of LSR_MIN_TIME while programming
 *   swap barrier.
 *   Client should consider tSwapRdyHi only for Gsync III (P2060) network.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 */
#define NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW (0x30f10153) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS_MESSAGE_ID (0x53U)

typedef struct NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS {
    NvU32 tSwapRdyHi;
} NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS;


/*
 * NV30F1_CTRL_CMD_GSYNC_GET_OPTIMIZED_TIMING
 *
 * This command allows the client to obtain suggested
 * adjustments to vertical and horizontal timing values
 * that will improve the ability of gsync to lock.
 *
 * gpuId
 *   This parameter is set by the client to indicate the 
 *   gpuId of the GPU to which the display to be optimized
 *   is attached.
 * display
 *   This parameter is not used by RM currently.
 *   Clients can ignore this parameter.  Note that this
 *   parameter will be removed in future.
 * output
 *   This parameter is set by the client to indicate the 
 *   output resource type of the display to be optimized.
 *   For example, CRTs use DAC output, while DFPs use SOR
 *   (Serial Output Resource) type.
 * protocol
 *   This parameter is set by the client to indicate the 
 *   data protocol of output resource.  For DAC displays,
 *   the format of the standard mode most closely matching
 *   the desired mode is used.  For SOR display devices,
 *   the LVDS/TMDS/etc format is the protocol.
 * structure
 *   This parameter is set by the client to indicate the 
 *   raster structure of the mode, either progressive or
 *   interlaced.  Diagrams of the raster structures are
 *   provided below.
 * adjust
 *   This parameter is set by the client to specify which
 *   of the timing values, other than hTotal and vTotal,
 *   may be adjusted during optimization.
 *   If the client does not obtain instructions from the
 *   user about where adjustments should be applied, safe
 *   default values for progressive/interlaced modes are
 *   provided below.
 * hTotal
 *   This parameter is set by the client to specify the
 *   initial Horizontal Pixel Total, from which the
 *   RM will begin optimizing.  The RM also uses the 
 *   parameter to return the optimized value.
 * vTotal
 *   This parameter is set by the client to specify the
 *   initial Vertical Pixel Total, from which the
 *   RM will begin optimizing.  The RM also uses the 
 *   parameter to return the optimized value.
 * hBlankStart
 *   This parameter is set by the client to specify the
 *   initial Horizontal Blanking Start, from which the
 *   RM will begin optimizing.  The RM also uses the 
 *   parameter to return the optimized value.
 * vBlankStart
 *   This parameter is set by the client to specify the
 *   initial Vertical Blanking Start, from which the
 *   RM will begin optimizing.  The RM also uses the 
 *   parameter to return the optimized value.
 * hBlankEnd
 *   This parameter is set by the client to specify the
 *   initial Horizontal Blanking End, from which the
 *   RM will begin optimizing.  The RM also uses the 
 *   parameter to return the optimized value.
 * vBlankEnd
 *   This parameter is set by the client to specify the
 *   initial Vertical Blanking End, from which the
 *   RM will begin optimizing.  The RM also uses the 
 *   parameter to return the optimized value.
 * vInterlacedBlankStart
 *   This parameter is set by the client to specify the
 *   initial Interlaced Vertical Blanking Start, from 
 *   which the RM will begin optimizing.  The RM will
 *   ignore this parameter for non-interlaced modes, as
 *   it has no meaning in those modes.  In modes where
 *   it is meaningful, the RM also uses the parameter
 *   to return the optimized value. 
 * vInterlacedBlankEnd
 *   This parameter is set by the client to specify the
 *   initial Interlaced Vertical Blanking End, from 
 *   which the RM will begin optimizing.  The RM will
 *   ignore this parameter for non-interlaced modes, as
 *   it has no meaning in those modes.  In modes where
 *   it is meaningful, the RM also uses the parameter 
 *   to return the optimized value.
 * hSyncEnd
 *   This parameter is set by the client to specify the
 *   initial Horizontal Raster Sync End, from which the
 *   RM will begin optimizing.  The RM also uses the 
 *   parameter to return the optimized value.
 * vSyncEnd
 *   This parameter is set by the client to specify the
 *   initial Vertical Raster Sync End, from which the
 *   RM will begin optimizing.  The RM also uses the 
 *   parameter to return the optimized value.
 * hDeltaStep
 *   This parameter is set by the client to specify the
 *   increments by which the Horizontal Pixel Total may
 *   be adjusted by the RM, during optimization.
 *   If the client does not obtain a custom value for  
 *   this parameter from the user, setting all four of
 *   hDeltaStep, vDeltaStep, hDeltaMax, and vDeltaMax
 *   to zero will result in a safe default for all four.
 * vDeltaStep
 *   This parameter is set by the client to specify the
 *   increments by which the vertical timings of each 
 *   frame (in interlaced modes, each field) may be 
 *   adjusted by the RM, during optimization.
 *   In interlaced modes, the adjustments to vTotal,
 *   vInterlacedBlankStart, and vInterlacedBlankEnd may
 *   be in increments of vDeltaStep or twice vDeltaStep, 
 *   depending on where adjustments are made.
 *   In progressive modes, the adjustment to the vTotal
 *   will simply be in increments of vDeltaStep.
 *   If the client does not obtain a custom value for  
 *   this parameter from the user, setting all four of
 *   hDeltaStep, vDeltaStep, hDeltaMax, and vDeltaMax
 *   to zero will result in a safe default for all four.
 * hDeltaMax
 *   This parameter is set by the client to specify the
 *   maximum amount that the Horizontal Pixel Total may
 *   be adjusted by the RM, during optimization.
 *   If the client does not obtain a custom value for  
 *   this parameter from the user, setting all four of
 *   hDeltaStep, vDeltaStep, hDeltaMax, and vDeltaMax
 *   to zero will result in a safe default for all four.
 * vDeltaMax
 *   This parameter is set by the client to specify the
 *   maximum amount that vertical timings of each frame
 *   (in interlaced modes, each field) may be adjusted
 *   by the RM, during optimization.
 *   In interlaced modes, the adjustments to vTotal,
 *   vInterlacedBlankStart, and vInterlacedBlankEnd may
 *   be up to twice vDeltaMax.
 *   In progressive modes, the adjustment to the vTotal
 *   may simply be up to vDeltaMax.
 *   If the client does not obtain a custom value for  
 *   this parameter from the user, setting all four of
 *   hDeltaStep, vDeltaStep, hDeltaMax, and vDeltaMax
 *   to zero will result in a safe default for all four.
 * refreshX10K
 *   This parameter is set by the client to specify the
 *   desired refresh rate, multiplied by 10000.  This 
 *   allows refresh rate to be set in units of 0.0001 Hz. 
 *   For example, a 59.94 Hz rate would be set as 599400.
 *   The client can alternatively specify a the 
 *   pixelClockHz parameter (if the passed in refreshX10K
 *   parameter is set to 0, the pixelClockHz parameter
 *   will be used).
 * pixelClockHz
 *   This parameter is set by the client to specify the
 *   desired pixel clock frequency in units of Hz. The 
 *   client can alternatively specify the refreshX10K parameter.
 *   This parameter is returned by the RM to report the
 *   optimal pixel clock to use with the adjusted mode, 
 *   in units of Hz. 
 *
 * Progressive Raster Structure
 *
 *                 hSyncEnd                            hTotal
 *             0   |   hBlankEnd              hBlankStart   |
 *             |   |   |                                |   |    vSync vBlank
 *          0--+--------------------------------------------+     +-+     | 
 *             |     Sync                                   |       |     | 
 *   vSyncEnd--|   +----------------------------------------+     +-+     | 
 *             |   |     Back Porch                         |     |       |
 *  vBlankEnd--|   |   +--------------------------------+   |     |     +-+ 
 *             |   |   |     Active Area                |   |     |     | 
 *             |   |   |   +------------------------+   |   |     |     | 
 *             |   |   |   |                        |   |   |     |     | 
 *             | S | B | A |                        | A | F |     |     | 
 *             | y | a | c |                        | c | r |     |     | 
 *             | n | c | t |                        | t | o |     |     | 
 *             | c | k | i |                        | i | n |     |     | 
 *             |   |   | v |                        | v | t |     |     | 
 *             |   | P | e |     Output Viewport    | e |   |     |     | 
 *             |   | o |   |                        |   | P |     |     | 
 *             |   | r | A |                        | A | o |     |     | 
 *             |   | c | r |                        | r | r |     |     | 
 *             |   | h | e |                        | e | c |     |     | 
 *             |   |   | a |                        | a | h |     |     | 
 *             |   |   |   |                        |   |   |     |     | 
 *             |   |   |   +------------------------+   |   |     |     | 
 *             |   |   |     Active Area                |   |     |     | 
 * vBlankStart-|   |   +--------------------------------+   |     |     +-+ 
 *             |   |     Front Porch                        |     |       | 
 *     vTotal--+---+----------------------------------------+     +-+     | 
 *              ___
 *             /   \________________________________________/ hSync
 *             ________                                  ____
 *                     \________________________________/     hBlank
 *
 *
 *
 * Interlaced Raster Structure
 *
 *                 hSyncEnd                            hTotal
 *             0   |   hBlankEnd              hBlankStart   |
 *             |   |   |                                |   |    vSync vBlank
 *          0--+--------------------------------------------+     +-+     | 
 *             |     Sync                                   |       |     | 
 *   vSyncEnd--|   +----------------------------------------+     +-+     | 
 *             |   |     Back Porch                         |     |       |
 *  vBlankEnd--|   |   +--------------------------------+   |     |     +-+ 
 *             |   |   |     Active Area                |   |     |     | 
 *             |   |   |   +------------------------+   |   |     |     | 
 *             |   |   |   |                        |   |   |     |     | 
 *             | S | B | A |                        | A | F |     |     | 
 *             | y | a | c |                        | c | r |     |     | 
 *             | n | c | t |                        | t | o |     |     | 
 *             | c | k | i |                        | i | n |     |     | 
 *             |   |   | v |                        | v | t |     |     | 
 *             |   | P | e |     Output Viewport    | e |   |     |     | 
 *             |   | o |   |                        |   | P |     |     | 
 *             |   | r | A |                        | A | o |     |     | 
 *             |   | c | r |                        | r | r |     |     | 
 *             |   | h | e |                        | e | c |     |     | 
 *             |   |   | a |                        | a | h |     |     | 
 *             |   |   |   |                        |   |   |     |     | 
 *             |   |   |   +------------------------+   |   |     |     | 
 *             |   |   |     Active Area                |   |     |     | 
 * vBlankStart-|   |   +--------------------------------+   |     |     +-+ 
 *             |   |                                        |     |       | 
 *             |   |     Front Porch   +--------------------+     |       | 
 *             |   |                   |                    |     +-+     | 
 *             |   +-------------------+                    |       |     | 
 *             |                                            |       |     | 
 *             |     Sync              +--------------------+       |     | 
 *             |                       |                    |     +-+     | 
 *             |   +-------------------+                    |     |       |
 * vInterlaced |   |     Back Porch                         |     |       | 
 *   BlankEnd--|   |   +--------------------------------+   |     |     +-+ 
 *             |   |   |     Active Area                |   |     |     | 
 *             |   |   |   +------------------------+   |   |     |     | 
 *             |   |   |   |                        |   |   |     |     | 
 *             | S | B | A |                        | A | F |     |     | 
 *             | y | a | c |                        | c | r |     |     | 
 *             | n | c | t |                        | t | o |     |     | 
 *             | c | k | i |                        | i | n |     |     | 
 *             |   |   | v |                        | v | t |     |     | 
 *             |   | P | e |     Output Viewport    | e |   |     |     | 
 *             |   | o |   |                        |   | P |     |     | 
 *             |   | r | A |                        | A | o |     |     | 
 *             |   | c | r |                        | r | r |     |     | 
 *             |   | h | e |                        | e | c |     |     | 
 *             |   |   | a |                        | a | h |     |     | 
 *             |   |   |   |                        |   |   |     |     | 
 *             |   |   |   +------------------------+   |   |     |     | 
 * vInterlaced |   |   |     Active Area                |   |     |     | 
 *  BlankStart-|   |   +--------------------------------+   |     |     +-+
 *             |   |     Front Porch                        |     |       |
 *     vTotal--+---+----------------------------------------+     +-+     |
 *              ___
 *             /   \________________________________________/ hSync
 *             ________                                  ____
 *                     \________________________________/     hBlank
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 *
*/

#define NV30F1_CTRL_CMD_GSYNC_GET_OPTIMIZED_TIMING (0x30f10160) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS_MESSAGE_ID (0x60U)

typedef struct NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS {
    NvU32 gpuId;
    NvU32 display;
    NvU32 output;
    NvU32 protocol;
    NvU32 structure;
    NvU32 adjust;
    NvU32 hDeltaStep;
    NvU32 hDeltaMax;
    NvU32 vDeltaStep;
    NvU32 vDeltaMax;
    NvU32 hSyncEnd;
    NvU32 hBlankEnd;
    NvU32 hBlankStart;
    NvU32 hTotal;
    NvU32 vSyncEnd;
    NvU32 vBlankEnd;
    NvU32 vBlankStart;
    NvU32 vInterlacedBlankEnd;
    NvU32 vInterlacedBlankStart;
    NvU32 vTotal;
    NvU32 refreshX10K;
    NvU32 pixelClockHz;
} NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS;

/* output values */
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_OUTPUT_DAC        (0x00000001)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_OUTPUT_SOR        (0x00000004)

/* protocol values for DAC displays (e.g. CRTs) */
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_DAC_RGB_CRT       (0x00000000)

/* protocol values for SOR displays (e.g. DFPs) */
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_SOR_LVDS_CUSTOM   (0x00000000)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_SOR_SINGLE_TMDS_A (0x00000001)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_SOR_SINGLE_TMDS_B (0x00000002)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_SOR_DUAL_TMDS     (0x00000005)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_SOR_DP_A          (0x00000008)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_SOR_DP_B          (0x00000009)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_SOR_HDMI_FRL      (0x0000000C)

/* structure values */
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_STRUCTURE_PROGRESSIVE      (0x00000000)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_STRUCTURE_INTERLACED       (0x00000001)

/* adjust values */
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_ADJUST_H_FRONT_PORCH       (0x00000001)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_ADJUST_V_FRONT_PORCH       (0x00000002)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_ADJUST_H_ACTIVE_AREA       (0x00000004)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_ADJUST_V_ACTIVE_AREA       (0x00000008)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_ADJUST_H_BACK_PORCH        (0x00000010)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_ADJUST_V_BACK_PORCH        (0x00000020)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_ADJUST_H_RASTER_SYNC       (0x00000040)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_ADJUST_V_RASTER_SYNC       (0x00000080)

#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_ADJUST_DEFAULT_CRT         (0x00000030)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_ADJUST_DEFAULT_DFP         (0x00000020)

/* DeltaStep and DeltaMax values to trigger default settings */
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_H_DELTA_STEP_USE_DEFAULTS  (0x00000000)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_H_DELTA_MAX_USE_DEFAULTS   (0x00000000)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_V_DELTA_STEP_USE_DEFAULTS  (0x00000000)
#define NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_V_DELTA_MAX_USE_DEFAULTS   (0x00000000)

/*
 * NV30F1_CTRL_CMD_GSYNC_SET_EVENT_NOTIFICATION
 *
 * This command sets event notification state for the associated Gsync
 * object.  This command requires that an instance of NV01_EVENT has
 * been previously bound to the associated Gsync object.  
 * 
 * If one or more of the "smart event notification" options are set in the
 * action parameter, multiple sequential events of the same type will only 
 * trigger one notification.  After that, only an event of a different type
 * will trigger a new notification.
 *
 *   action
 *     This member specifies the desired event notification action.
 *     Valid notification actions include:
 *     NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_DISABLE
 *       This action disables event notification for the associated
 *       Gsync object.
 *     NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_SYNC_LOSS
 *       This action enables smart event notification for the
 *       associated Gsync object, for "sync loss" events.
 *     NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_SYNC_GAIN
 *       This action enables smart event notification for the
 *       associated Gsync object, for "sync gained" events.
 *     NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_STEREO_LOSS
 *       This action enables smart event notification for the
 *       associated Gsync object, for "stereo lost" events.
 *     NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_STEREO_GAIN
 *       This action enables smart event notification for the
 *       associated Gsync object, for "stereo gained" events.
 *     NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_HOUSE_GAIN
 *       This action enables smart event notification for the
 *       associated Gsync object, for "house sync (BNC) plug in" events.
 *     NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_HOUSE_LOSS
 *       This action enables smart event notification for the
 *       associated Gsync object, for "house sync (BNC) plug out" events.
 *     NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_RJ45_GAIN
 *       This action enables smart event notification for the
 *       associated Gsync object, for "ethernet (RJ45) plug in" events.
 *     NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_RJ45_LOSS
 *       This action enables smart event notification for the
 *       associated Gsync object, for "ethernet (RJ45) plug out" events.
 *     NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_COUNT_MATCH
 *       This action enables smart event notification for the
 *       associated Gsync object, for "frame counter match" events.
 *     NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_ALL
 *       This action enables smart event notification for the
 *       associated Gsync object, for any type of event.
 * 
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV30F1_CTRL_CMD_GSYNC_SET_EVENT_NOTIFICATION                      (0x30f10170) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_PARAMS_MESSAGE_ID (0x70U)

typedef struct NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_PARAMS {
    NvU32 action;
} NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_PARAMS;

/* valid action values */

#define NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_DISABLE           (0x00000000)
#define NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_SYNC_LOSS   (0x00000001)
#define NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_SYNC_GAIN   (0x00000002)
#define NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_STEREO_LOSS (0x00000004)
#define NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_STEREO_GAIN (0x00000008)
#define NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_HOUSE_GAIN  (0x00000010)
#define NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_HOUSE_LOSS  (0x00000020)
#define NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_RJ45_GAIN   (0x00000040)
#define NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_RJ45_LOSS   (0x00000080)
#define NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_COUNT_MATCH (0x00000100)
#define NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_ALL         (0x000001FF)

#define NV30F1_CTRL_GSYNC_EVENT_TYPES                                     9

/*
 * NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE
 * NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE
 *
 * These commands can be used to get/set the stereo lock assistance mode of
 * the GSYNC device. This is supported by GSYNC III device only.
 * 1] In this mode the GSYNC recreates the hidden VS either by ->
 *    (a) using local stereo edge if stereo is toggle or
 *    (b) counting lines and generate the missing VS.
 * 2] Master GSYNC card recreates the stereo and passes it along to
 *    the slave GSYNC cards.
 * 3] Slave GSYNC cards generates the stereo raster sync structure to
 *    synchronize the GPU.
 * 4] For stereo sync status reporting, under this mode, the GSYNC automatically
 *    reports stereo lock whenever it gets the master stereo signal. The
 *    assumption is local stereo will be in synced with the new structure.
 * 5] If the slave GSYNC card does not observed master stereo for any reason,
 *    (a) it clears the stereo sync bit and
 *    (b) it generates its own version of stereo and sync the GPU.
 *
 * Parameters:
 *   gpuId
 *     This parameter is set by the client to indicate the gpuId on which
 *     the stereo lock mode should be enabled/disabled.
 *
 *   enable
 *     In SET query, this parameter is set by the client to indicate whether
 *     RM should enable or disable stereo lock mode for GPU specified in gpuId.
 *     1 and 0 indicates enable and disable stereo lock mode respectively. In
 *     GET query, RM will set this parameter to 1 or 0 depending on StereoLock
 *     mode is enabled or not respectively for specified GPU.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE                (0x30f10172) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE                (0x30f10173) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS_MESSAGE_ID (0x72U)

typedef struct NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS {
    NvU32 gpuId;
    NvU32 enable;
} NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS;
#define NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE_PARAMS_MESSAGE_ID (0x73U)

typedef NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE_PARAMS;

/*
 * NV30F1_CTRL_CMD_GSYNC_READ_REGISTER
 *
 * This command is used to read raw i2c registers from the gsync device, via
 * the given GPU (registers on the same gsync device may have different values
 * depending on which GPU is used to do the read).
 *
 * This may only be used by a privileged client.
 *
 * Parameters:
 *   gpuId
 *     This parameter is set by the client to specify which GPU to use to
 *     perform the read.
 *
 *   reg
 *     This parameter is set by the client to specify which i2c register to
 *     read.
 *
 *   data
 *     This parameter is written by the RM and returned to the client upon a
 *     successful read.
 *
 *  Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 */
#define NV30F1_CTRL_CMD_GSYNC_READ_REGISTER (0x30f10180) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_READ_REGISTER_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_READ_REGISTER_PARAMS_MESSAGE_ID (0x80U)

typedef struct NV30F1_CTRL_GSYNC_READ_REGISTER_PARAMS {
    NvU32 gpuId;
    NvU8  reg;
    NvU8  data;
} NV30F1_CTRL_GSYNC_READ_REGISTER_PARAMS;

/*
 * NV30F1_CTRL_CMD_GSYNC_WRITE_REGISTER
 *
 * This command is used to write raw i2c registers on the gsync device, via the
 * given GPU (registers on the same gsync device may have different values
 * depending on which GPU is used to do the write).
 *
 * This may only be used by a privileged client.
 *
 * Parameters:
 *   gpuId
 *     This parameter is set by the client to specify which GPU to use to
 *     perform the write.
 *
 *   reg
 *     This parameter is set by the client to specify which i2c register to
 *     write.
 *
 *   data
 *     This parameter is set by the client to specify what data to write to the
 *     given i2c register.
 *
 *  Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 */

#define NV30F1_CTRL_CMD_GSYNC_WRITE_REGISTER (0x30f10181) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_WRITE_REGISTER_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_WRITE_REGISTER_PARAMS_MESSAGE_ID (0x81U)

typedef struct NV30F1_CTRL_GSYNC_WRITE_REGISTER_PARAMS {
    NvU32 gpuId;
    NvU8  reg;
    NvU8  data;
} NV30F1_CTRL_GSYNC_WRITE_REGISTER_PARAMS;



/*
 * NV30F1_CTRL_CMD_GSYNC_SET_LOCAL_SYNC
 *
 * This command enables/disables raster sync on displays i.e. 
 * mosaic groups between gpus.
 *
 * gpuTimingSource
 *   The parameter is set by the client to indicate the gpuId of the
 *   Timing Source gpu for specified mosaic group.
 * gpuTimingSlaves[]
 *   This parameter is set by the client to indicate the gpuIds of the
 *   timing slave gpus for specified mosaic group. It should not contain
 *   more gpuids than slaveGpuCount.
 * slaveGpuCount
 *   This parameter is set by the client to indicate the count of timing
 *   slave gpus under specified group.
 *   Referring to gsync3-P2060, slaveGpuCount can vary from 0x01 to 0x03
 *   as maximum possible connected gpus are four and one gpu must be
 *   timing master for mosaic group.
 * mosaicGroupNumber
 *   This parameter is set by the client to tell the RM to which mosaic
 *   group it should refer.
 *   Referring to gsync3-P2060, mosaicGroupNumber can contain 0x00 or
 *   0x01 as only two mosaic groups are possible.
 * enableMosaic
 *   This parameter is set by the client to indicate RM that whether RM
 *   should enable mosaic or disable mosaic.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 * 
 */
#define NV30F1_CTRL_CMD_GSYNC_SET_LOCAL_SYNC (0x30f10185) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS_MESSAGE_ID (0x85U)

typedef struct NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS {
    NvU32  gpuTimingSource;
    NvU32  gpuTimingSlaves[NV30F1_CTRL_MAX_GPUS_PER_GSYNC];
    NvU32  slaveGpuCount;
    NvU32  mosaicGroupNumber;
    NvBool enableMosaic;
} NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS;

/*
 * NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH
 *
 * This command configure GSYNC registers for pre-flash and post-flash
 * operations. This is currenly used for GSYNC-3 (P2060) only. RM clients
 * has to make sure that they perform both pre-flash and post-flash
 * operations on GSYNC board. Avoiding, post-flash will cause mismatch
 * between RM cached-data and GSYNC register values.
 *
 * Parameters:
 *   gpuId
 *     This parameter is set by the client to indicate the gpuId for which
 *     GSYNC board connected to that GPU will be configured for pre-flash
 *     or post-flash operation depending on preFlash value.
 *
 *   preFlash
 *     This parameter is set by the client to indicate whether RM has to configure
 *     GSYNC registers and SW state for pre-flash or post-flash operation. Values
 *     1 and 0 indicates that RM will configure GSYNC board for pre-flash and
 *     post-flash operations respectively.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH (0x30f10186) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH_PARAMS_MESSAGE_ID" */

#define NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH_PARAMS_MESSAGE_ID (0x86U)

typedef struct NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH_PARAMS {
    NvU32 gpuId;
    NvU32 preFlash;
} NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH_PARAMS;

/*
 * NV30F1_CTRL_CMD_GSYNC_GET_HOUSE_SYNC_MODE
 * NV30F1_CTRL_CMD_GSYNC_SET_HOUSE_SYNC_MODE
 *
 * These two commands gets/sets house sync mode as input or output.
 *
 * Parameters:
 *   houseSyncMode
 *     This parameter indicates whether the house sync mode is input or
 *     output. For GET_HOUSE_SYNC_MODE, the current mode will be written 
 *     by RM and returned to the client; for SET_HOUSE_SYNC_MODE, the client
 *     will write the new mode value to this parameter and pass it to RM
 *     for execution.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_DEVICE
 *   NV_ERR_INVALID_STATE
 *
 */
#define NV30F1_CTRL_CMD_GSYNC_GET_HOUSE_SYNC_MODE (0x30f10187) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | 0x87" */

#define NV30F1_CTRL_CMD_GSYNC_SET_HOUSE_SYNC_MODE (0x30f10188) /* finn: Evaluated from "(FINN_NV30_GSYNC_GSYNC_INTERFACE_ID << 8) | 0x88" */

typedef struct NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS {
    NvU8 houseSyncMode;
} NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS;


#define NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_INPUT  (0x00)
#define NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_OUTPUT (0x01)

/* _ctrl30f1_h_ */
