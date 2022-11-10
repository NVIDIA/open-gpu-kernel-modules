/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl5070/ctrl5070rg.finn
//

#include "ctrl/ctrl5070/ctrl5070base.h"



/*
 * NV5070_CTRL_CMD_GET_RG_STATUS
 *
 * This 'get' command returns the status of raster generator
 *
 *      head
 *          The head for which RG status is desired.
 *
 *      scanLocked
 *          Whether or not RG is scan (raster or frame) locked.
 *      flipLocked
 *          Whether or not RG is flip locked.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC
 */
#define NV5070_CTRL_CMD_GET_RG_STATUS                (0x50700202) /* finn: Evaluated from "(FINN_NV50_DISPLAY_RG_INTERFACE_ID << 8) | NV5070_CTRL_CMD_GET_RG_STATUS_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_GET_RG_STATUS_SCANLOCKED_NO  (0x00000000)
#define NV5070_CTRL_CMD_GET_RG_STATUS_SCANLOCKED_YES (0x00000001)

#define NV5070_CTRL_CMD_GET_RG_STATUS_FLIPLOCKED_NO  (0x00000000)
#define NV5070_CTRL_CMD_GET_RG_STATUS_FLIPLOCKED_YES (0x00000001)

#define NV5070_CTRL_CMD_GET_RG_STATUS_STALLED_NO     (0x00000000)
#define NV5070_CTRL_CMD_GET_RG_STATUS_STALLED_YES    (0x00000001)

#define NV5070_CTRL_CMD_GET_RG_STATUS_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV5070_CTRL_CMD_GET_RG_STATUS_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       head;

    NvU32                       scanLocked;           // [OUT]
    NvU32                       flipLocked;           // [OUT]
    NvU32                       rgStalled;
} NV5070_CTRL_CMD_GET_RG_STATUS_PARAMS;

/*
 * NV5070_CTRL_CMD_UNDERFLOW_PARAMS
 *
 * This structure contains data for
 * NV5070_CTRL_CMD_SET_RG_UNDERFLOW_PROP and
 * NV5070_CTRL_CMD_GET_RG_UNDERFLOW_PROP CTRL calls
 *
 *   head
 *     The head for which RG underflow properties needed to be set/get.
 *     Valid values for this parameter are 0 to NV5070_CTRL_CMD_MAX_HEADS.
 *   enable
 *     _SET_RG_: Whether to enable or disable RG underflow reporting.
 *     _GET_RG_: Whether or not RG underflow reporting is enabled.
 *   underflow
 *     _SET_RG_: Clear underflow (CLEAR_UNDERFLOW_YES) or leave it alone
 *               (CLEAR_UNDERFLOW_NO).
 *               Note: The GET_RG function automatically clears the underflow.
 *               It is recommended that GET_RG be used to clear any initial
 *               underflows, and that the "underflow" field be set to
 *               CLEAR_UNDERFLOW_NO in any SET_RG calls.  This field may be
 *               deprecated in the future, for SET_RG calls.
 *     _GET_RG_: UNDERFLOWED_YES if an RG underflow occurred since the most
 *               recent prior call to to NV5070_CTRL_CMD_GET_RG_STATUS.
 *   epfifoUnderflow
 *     _SET_RG_: Not used.
 *     _GET_RG_: EPFIFO_UNDERFLOWED_YES if an EPFIFO underflow occurred since
 *               the most recent prior call to NV5070_CTRL_CMD_GET_RG_STATUS.
 *   mode
 *     _SET_RG_: What mode to use when underflow occurs. This is
 *               independent from enable field. This is always active.
 *     _GET_RG_: What mode is used when underflow occurs. This is
 *               independent from enable field. This is always active.
 */
typedef struct NV5070_CTRL_CMD_UNDERFLOW_PARAMS {
    NvU32 head;
    NvU32 enable;
    NvU32 underflow;
    NvU32 epfifoUnderflow;
    NvU32 mode;
} NV5070_CTRL_CMD_UNDERFLOW_PARAMS;

#define NV5070_CTRL_CMD_UNDERFLOW_PROP_ENABLED_NO             (0x00000000)
#define NV5070_CTRL_CMD_UNDERFLOW_PROP_ENABLED_YES            (0x00000001)
#define NV5070_CTRL_CMD_UNDERFLOW_PROP_UNDERFLOWED_NO         (0x00000000)
#define NV5070_CTRL_CMD_UNDERFLOW_PROP_UNDERFLOWED_YES        (0x00000001)
#define NV5070_CTRL_CMD_UNDERFLOW_PROP_EPFIFO_UNDERFLOWED_NO  (0x00000000)
#define NV5070_CTRL_CMD_UNDERFLOW_PROP_EPFIFO_UNDERFLOWED_YES (0x00000001)
#define NV5070_CTRL_CMD_UNDERFLOW_PROP_MODE_REPEAT            (0x00000000)
#define NV5070_CTRL_CMD_UNDERFLOW_PROP_MODE_RED               (0x00000001)
#define NV5070_CTRL_CMD_UNDERFLOW_PROP_ENABLE_NO              (0x00000000)
#define NV5070_CTRL_CMD_UNDERFLOW_PROP_ENABLE_YES             (0x00000001)
#define NV5070_CTRL_CMD_UNDERFLOW_PROP_CLEAR_UNDERFLOW_NO     (0x00000000)
#define NV5070_CTRL_CMD_UNDERFLOW_PROP_CLEAR_UNDERFLOW_YES    (0x00000001)

/*
 * NV5070_CTRL_CMD_GET_RG_UNDERFLOW_PROP
 *
 * This command returns the underflow reporting parameters inside
 * NV5070_CTRL_CMD_UNDERFLOW_PARAMS structure
 *
 *   underflowParams
 *     Contains data for underflow logging.
 *     Check NV5070_CTRL_CMD_UNDERFLOW_PARAMS structure.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC
 */
#define NV5070_CTRL_CMD_GET_RG_UNDERFLOW_PROP                 (0x50700203) /* finn: Evaluated from "(FINN_NV50_DISPLAY_RG_INTERFACE_ID << 8) | NV5070_CTRL_CMD_GET_RG_UNDERFLOW_PROP_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_GET_RG_UNDERFLOW_PROP_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV5070_CTRL_CMD_GET_RG_UNDERFLOW_PROP_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS      base;
    NV5070_CTRL_CMD_UNDERFLOW_PARAMS underflowParams;
} NV5070_CTRL_CMD_GET_RG_UNDERFLOW_PROP_PARAMS;


/*
 * NV5070_CTRL_CMD_SET_RG_UNDERFLOW_PROP
 *
 * This command sets up the underflow parameters using
 * NV5070_CTRL_CMD_UNDERFLOW_PARAMS structure
 *
 *   underflowParams
 *     Contains data for underflow logging.
 *     Check NV5070_CTRL_CMD_UNDERFLOW_PARAMS structure.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC
 */
#define NV5070_CTRL_CMD_SET_RG_UNDERFLOW_PROP (0x50700204) /* finn: Evaluated from "(FINN_NV50_DISPLAY_RG_INTERFACE_ID << 8) | NV5070_CTRL_CMD_SET_RG_UNDERFLOW_PROP_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_SET_RG_UNDERFLOW_PROP_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV5070_CTRL_CMD_SET_RG_UNDERFLOW_PROP_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS      base;
    NV5070_CTRL_CMD_UNDERFLOW_PARAMS underflowParams;
} NV5070_CTRL_CMD_SET_RG_UNDERFLOW_PROP_PARAMS;


/*
 * NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP
 *
 * This command gets the timing parameters associated with the lockout period.
 *
 *      head
 *          The head for which RG fliplock properties are desired.
 *
 *      maxSwapLockoutSkew
 *          The maximum possible skew between the swap lockout signals for all
 *          heads which are fliplocked to this head.
 *
 *      swapLockoutStart
 *          Determines the start of the start lockout period, expressed as the
 *          number of lines before the end of the frame. The minimum allowed
 *          value is 1.

 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC
 */
#define NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP (0x50700205) /* finn: Evaluated from "(FINN_NV50_DISPLAY_RG_INTERFACE_ID << 8) | NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP_MAX_SWAP_LOCKOUT_SKEW          9:0

#define NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP_SWAP_LOCKOUT_START            15:0

#define NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       head;

    NvU32                       maxSwapLockoutSkew;
    NvU32                       swapLockoutStart;
} NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP_PARAMS;

/*
 * NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP
 *
 * This command sets the timing parameters associated with the lockout period.
 *
 *      head
 *          The head for which RG fliplock properties are desired.
 *
 *      maxSwapLockoutSkew
 *          The maximum possible skew between the swap lockout signals for all
 *          heads which are fliplocked to this head.
 *
 *      swapLockoutStart
 *          Determines the start of the start lockout period, expressed as the
 *          number of lines before the end of the frame. The minimum allowed
 *          value is 1.

 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC
 */
#define NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP                            (0x50700206) /* finn: Evaluated from "(FINN_NV50_DISPLAY_RG_INTERFACE_ID << 8) | NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_MAX_SWAP_LOCKOUT_SKEW      9:0
#define NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_MAX_SWAP_LOCKOUT_SKEW_INIT (0x00000000)

#define NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_SWAP_LOCKOUT_START         15:0
#define NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_SWAP_LOCKOUT_START_INIT    (0x00000000)

#define NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       head;

    NvU32                       maxSwapLockoutSkew;
    NvU32                       swapLockoutStart;
} NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_PARAMS;

/*
 * NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN
 *
 * This command returns which lockpin has been connected for the specified
 * subdevice in the current SLI and/or framelock configuration.
 *
 *      head
 *          The head for which the locking is associated with
 *
 *      masterScanLock
 *          Indicate the connection status and pin number of master scanlock
 *
 *      slaveScanLock
 *          Indicate the connection status and pin number of slave scanlock
 *
 *      flipLock
 *          Indicate the connection status and pin number of fliplock
 *
 *      stereoLock
 *          Indicate the connection status and pin number of stereo lock
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 */
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN                                (0x50700207) /* finn: Evaluated from "(FINN_NV50_DISPLAY_RG_INTERFACE_ID << 8) | NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_MASTER_SCAN_LOCK_CONNECTED     0:0
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_MASTER_SCAN_LOCK_CONNECTED_NO  (0x00000000)
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_MASTER_SCAN_LOCK_CONNECTED_YES (0x00000001)
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_MASTER_SCAN_LOCK_PIN           3:1

#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_SLAVE_SCAN_LOCK_CONNECTED      0:0
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_SLAVE_SCAN_LOCK_CONNECTED_NO   (0x00000000)
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_SLAVE_SCAN_LOCK_CONNECTED_YES  (0x00000001)
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_SLAVE_SCAN_LOCK_PIN            3:1

#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_FLIP_LOCK_CONNECTED       0:0
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_FLIP_LOCK_CONNECTED_NO         (0x00000000)
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_FLIP_LOCK_CONNECTED_YES        (0x00000001)
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_FLIP_LOCK_PIN             3:1

#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STEREO_LOCK_CONNECTED     0:0
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STEREO_LOCK_CONNECTED_NO       (0x00000000)
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STEREO_LOCK_CONNECTED_YES      (0x00000001)
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STEREO_LOCK_PIN           3:1

#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       head;

    NvU32                       masterScanLock;
    NvU32                       slaveScanLock;
    NvU32                       flipLock;
    NvU32                       stereoLock;
} NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_PARAMS;

/*
 * NV5070_CTRL_CMD_SET_VIDEO_STATUS
 *
 * This command is used to set the current video playback status for use
 * by the Display Power Saving (nvDPS) feature.  The playback status is
 * used to maximize power savings by altering the DFP refresh rate used for
 * video playback.
 *
 *   displayId
 *     This parameter specifies the ID of the video playback display.
 *     Only one display may be indicated in this parameter.
 *   clientId
 *     This parameter specifies the opaque client ID associated with
 *     the video playback application.
 *   mode
 *     This parameter specifies the video playback mode.  Valid values
 *     for this parameter include:
 *       NV5070_CTRL_DFP_SET_VIDEO_STATUS_MODE_NON_FULLSCREEN
 *         This value indicates that there is either no video playback or
 *         that video playback is windowed.
 *       NV5070_CTRL_DFP_SET_VIDEO_STATUS_MODE_FULLSCREEN
 *         This value indicates that video playback is fullscreen.
 *       NV5070_CTRL_DFP_SET_VIDEO_STATUS_MODE_D3D
 *         This value indicates that there is a D3D app started.
 *   frameRate
 *     The parameter indicates the current video playback frame rate.
 *     The value is a 32 bit unsigned fixed point number, 24 bit unsigned
 *     integer (bits 31:7), and 8 fraction bits (bits 7:0), measured in
 *     number of frames per second.
 *     A value of 0 indicates that video playback is stopped or not playing.
 *   frameRateAlarmUpperLimit
 *     The parameter indicates the upper limit which will can be tolerated in
 *     notifying frame rate change. If the frame rate changed but is still
 *     below the limit. The newer frame rate doesn't have to be set till it's
 *     over the limit.
 *     The value is a 32 bit unsigned fixed point number, 24 bit unsigned
 *     integer (bits 31:7), and 8 fraction bits (bits 7:0), measured in
 *     number of frames per second.
 *     A value of 0 indicates no tolerance of frame rate notifying. Instant
 *     frame rate has to be set when it has changed.
 *   frameRateAlarmLowerLimit
 *     The parameter indicates the lower limit which will can be tolerated in
 *     notifying frame rate change. If the frame rate changed but is still
 *     above the limit. The newer frame rate doesn't have to be set till it's
 *     below the limit.
 *     The value is a 32 bit unsigned fixed point number, 24 bit unsigned
 *     integer (bits 31:7), and 8 fraction bits (bits 7:0), measured in
 *     number of frames per second.
 *     A value of 0 indicates no tolerance of frame rate notifying. Instant
 *     frame rate has to be set when it has changed.
 *
 *     The frameRateAlarm limit values can be used by the video client to
 *     indicate the the range in which frame rate changes do not require
 *     notification (i.e. frame rates outside these limits will result in
 *     notification).
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV5070_CTRL_CMD_SET_VIDEO_STATUS (0x50700209) /* finn: Evaluated from "(FINN_NV50_DISPLAY_RG_INTERFACE_ID << 8) | NV5070_CTRL_DFP_SET_VIDEO_STATUS_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_DFP_SET_VIDEO_STATUS_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV5070_CTRL_DFP_SET_VIDEO_STATUS_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;

    NvU32                       displayId;
    NvU32                       clientId;
    NvU32                       mode;
    NvU32                       frameRate;
    NvU32                       frameRateAlarmUpperLimit;
    NvU32                       frameRateAlarmLowerLimit;
} NV5070_CTRL_DFP_SET_VIDEO_STATUS_PARAMS;

/* valid mode flags */
#define NV5070_CTRL_DFP_SET_VIDEO_STATUS_MODE_NON_FULLSCREEN                              (0x00000000)
#define NV5070_CTRL_DFP_SET_VIDEO_STATUS_MODE_FULLSCREEEN                                 (0x00000001)
#define NV5070_CTRL_DFP_SET_VIDEO_STATUS_MODE_D3D                                         (0x00000002)

/*
 * NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STATELESS
 *
 * This command returns which set of lockpins needs to be used in order to
 * successfully raster lock two heads on different GPUs together.  The
 * second GPU is not inferred from linked SLI state, if any, and needs to
 * be specified explicitly.
 *
 *   head
 *     The local head to be locked with the peer head.
 *
 *   peer.hDisplay
 *     The handle identifying a display object allocated on another
 *     GPU.  It specifies the peer of interest with a subdevice
 *     index (see below) and needs to be be distinct from the handle
 *     supplied directly to NvRmControl().
 *
 *   peer.subdeviceIndex
 *     The index of the peer subdevice of interest.
 *
 *   peer.head
 *     The peer head to be locked with the local head.
 *
 *   masterScanLockPin
 *   slaveScanLockPin
 *     Returns the master and slave scanlock pins that would need to
 *     be used to lock the specified heads together, if any.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_OBJECT_PARENT
 */
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STATELESS                                (0x5070020a) /* finn: Evaluated from "(FINN_NV50_DISPLAY_RG_INTERFACE_ID << 8) | NV5070_CTRL_GET_RG_CONNECTED_LOCKPIN_STATELESS_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STATELESS_MASTER_SCAN_LOCK_CONNECTED     0:0
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STATELESS_MASTER_SCAN_LOCK_CONNECTED_NO  (0x00000000)
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STATELESS_MASTER_SCAN_LOCK_CONNECTED_YES (0x00000001)
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STATELESS_MASTER_SCAN_LOCK_PIN           2:1

#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STATELESS_SLAVE_SCAN_LOCK_CONNECTED      0:0
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STATELESS_SLAVE_SCAN_LOCK_CONNECTED_NO   (0x00000000)
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STATELESS_SLAVE_SCAN_LOCK_CONNECTED_YES  (0x00000001)
#define NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STATELESS_SLAVE_SCAN_LOCK_PIN            2:1

#define NV5070_CTRL_GET_RG_CONNECTED_LOCKPIN_STATELESS_PARAMS_MESSAGE_ID (0xAU)

typedef struct NV5070_CTRL_GET_RG_CONNECTED_LOCKPIN_STATELESS_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       head;

    struct {
        NvHandle hDisplay;
        NvU32    subdeviceIndex;
        NvU32    head;
    } peer;

    NvU32 masterScanLock;
    NvU32 slaveScanLock;
} NV5070_CTRL_GET_RG_CONNECTED_LOCKPIN_STATELESS_PARAMS;

/*
 * NV5070_CTRL_CMD_GET_PINSET_LOCKPINS
 *
 * Get the lockpins for the specified pinset.
 *
 *   pinset [in]
 *     The pinset whose corresponding lockpin numbers need to be determined
 *     must be specified with this parameter.
 *
 *   scanLockPin [out]
 *     The scanlock lockpin (rasterlock or framelock) index, which can be
 *     either master or slave, is returned in this parameter.
 *
 *   flipLockPin [out]
 *     The fliplock lockpin index, is returned in this parameter.
 *
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV5070_CTRL_CMD_GET_PINSET_LOCKPINS                (0x5070020b) /* finn: Evaluated from "(FINN_NV50_DISPLAY_RG_INTERFACE_ID << 8) | NV5070_CTRL_GET_PINSET_LOCKPINS_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_GET_PINSET_LOCKPINS_SCAN_LOCK_PIN_NONE 0xffffffff

#define NV5070_CTRL_GET_PINSET_LOCKPINS_FLIP_LOCK_PIN_NONE 0xffffffff

#define NV5070_CTRL_GET_PINSET_LOCKPINS_PARAMS_MESSAGE_ID (0xBU)

typedef struct NV5070_CTRL_GET_PINSET_LOCKPINS_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       pinset;
    NvU32                       scanLockPin;
    NvU32                       flipLockPin;
} NV5070_CTRL_GET_PINSET_LOCKPINS_PARAMS;

/*
 * NV5070_CTRL_CMD_GET_RG_SCAN_LINE
 *
 * This 'get' command returns the current scan line value from raster generator 
 *
 *      head
 *          The head for which current scan line number is desired.
 * 
 *      scanLine
 *          Current scan line number.
 *
 *      inVblank
 *          Whether or not in vblank.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC
 */
#define NV5070_CTRL_CMD_GET_RG_SCAN_LINE               (0x5070020c) /* finn: Evaluated from "(FINN_NV50_DISPLAY_RG_INTERFACE_ID << 8) | NV5070_CTRL_CMD_GET_RG_SCAN_LINE_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_GET_RG_SCAN_LINE_IN_VBLANK_NO  (0x00000000)
#define NV5070_CTRL_CMD_GET_RG_SCAN_LINE_IN_VBLANK_YES (0x00000001)

#define NV5070_CTRL_CMD_GET_RG_SCAN_LINE_PARAMS_MESSAGE_ID (0xCU)

typedef struct NV5070_CTRL_CMD_GET_RG_SCAN_LINE_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       head;
    NvU32                       scanLine;             // [OUT]
    NvU32                       inVblank;             // [OUT]
} NV5070_CTRL_CMD_GET_RG_SCAN_LINE_PARAMS;

/*
 * NV5070_CTRL_CMD_GET_FRAMELOCK_HEADER_LOCKPINS
 *
 * This command returns FrameLock header lock pin information.
 * Lock pin index returned by this command corresponds to the 
 * evo lock pin number. Example - lock pin index 0 means 
 * LOCKPIN_0. 
 *
 *   frameLockPin [out]
 *     This parameter returns the FrameLock pin index 
 *     connected to FrameLock header.
 *
 *   rasterLockPin [out]
 *     This parameter returns the RasterLock pin index 
 *     connected to FrameLock header.
 *    
 *   flipLockPin [out]
 *     This parameter returns the FlipLock pin index 
 *     connected to FrameLock header.
 *
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED   
 */

#define NV5070_CTRL_CMD_GET_FRAMELOCK_HEADER_LOCKPINS                  (0x5070020d) /* finn: Evaluated from "(FINN_NV50_DISPLAY_RG_INTERFACE_ID << 8) | NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_FRAME_LOCK_PIN_NONE  (0xffffffff)
#define NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_RASTER_LOCK_PIN_NONE (0xffffffff)
#define NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_FLIP_LOCK_PIN_NONE   (0xffffffff)
#define NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_PARAMS_MESSAGE_ID (0xDU)

typedef struct NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       frameLockPin;
    NvU32                       rasterLockPin;
    NvU32                       flipLockPin;
} NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_PARAMS;



/* _ctrl5070rg_h_ */
