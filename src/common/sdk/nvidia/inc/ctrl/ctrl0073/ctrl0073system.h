/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0073/ctrl0073system.finn
//

#include "nvlimits.h"
#include "ctrl/ctrl0073/ctrl0073base.h"

/* NV04_DISPLAY_COMMON system-level control commands and parameters */

/* extract cap bit setting from tbl */
#define NV0073_CTRL_SYSTEM_GET_CAP(tbl,c)         (((NvU8)tbl[(1?c)]) & (0?c))

/* Caps format is byte_index:bit_mask.
 * Important: keep the number of bytes needed for these fields in sync with
 * NV0073_CTRL_SYSTEM_CAPS_TBL_SIZE
 */
#define NV0073_CTRL_SYSTEM_CAPS_AA_FOS_GAMMA_COMP_SUPPORTED        0:0x01
#define NV0073_CTRL_SYSTEM_CAPS_TV_LOWRES_BUG_85919                0:0x02
#define NV0073_CTRL_SYSTEM_CAPS_DFP_GPU_SCALING_BUG_154102         0:0x04
#define NV0073_CTRL_SYSTEM_CAPS_SLI_INTERLACED_MODE_BUG_235218     0:0x08 // Deprecated
#define NV0073_CTRL_SYSTEM_CAPS_STEREO_DIN_AVAILABLE               0:0x10
#define NV0073_CTRL_SYSTEM_CAPS_OFFSET_PCLK_DFP_FOR_EMI_BUG_443891 0:0x20
#define NV0073_CTRL_SYSTEM_CAPS_GET_DMI_SCANLINE_SUPPORTED         0:0x40
/*
 * Indicates support for HDCP Key Selection Vector (KSV) list and System
 * Renewability Message (SRM) validation
*/
#define NV0073_CTRL_SYSTEM_CAPS_KSV_SRM_VALIDATION_SUPPORTED       0:0x80

#define NV0073_CTRL_SYSTEM_CAPS_SINGLE_HEAD_MST_SUPPORTED          1:0x01
#define NV0073_CTRL_SYSTEM_CAPS_SINGLE_HEAD_DUAL_SST_SUPPORTED     1:0x02
#define NV0073_CTRL_SYSTEM_CAPS_HDMI_2_0_SUPPORTED                 1:0x04
#define NV0073_CTRL_SYSTEM_CAPS_CROSS_BAR_SUPPORTED                1:0x08
#define NV0073_CTRL_SYSTEM_CAPS_RASTER_LOCK_NEEDS_MIO_POWER        1:0x10
/*
 * Indicates that modesets where no heads are increasing resource requirements,
 * or no heads are decreasing resource requirements, can be done glitchlessly.
 */
#define NV0073_CTRL_SYSTEM_CAPS_GLITCHLESS_MODESET_SUPPORTED       1:0x20
/* Indicates the SW ACR is enabled for HDMI 2.1 due to Bug 3275257. */
#define NV0073_CTRL_SYSTEM_CAPS_HDMI21_SW_ACR_BUG_3275257          1:0x40

/* Size in bytes of display caps table. Keep in sync with # of fields above. */
#define NV0073_CTRL_SYSTEM_CAPS_TBL_SIZE   2U

/*
 * NV0073_CTRL_CMD_SYSTEM_GET_CAPS_V2
 *
 * This command returns the set of display capabilities for the parent device
 * in the form of an array of unsigned bytes.  Display capabilities
 * include supported features and required workarounds for the display
 * engine(s) within the device, each represented by a byte offset into the
 * table and a bit position within that byte.  The set of display capabilities
 * will be normalized across all GPUs within the device (a feature capability
 * will be set only if it's supported on all GPUs while a required workaround
 * capability will be set if any of the GPUs require it).
 *
 *   [out] capsTbl
 *     The display caps bits will be transferred by the RM into this array of
 *     unsigned bytes.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV0073_CTRL_CMD_SYSTEM_GET_CAPS_V2 (0x730101U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS_MESSAGE_ID (0x01U)

typedef struct NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS {
    NvU8 capsTbl[NV0073_CTRL_SYSTEM_CAPS_TBL_SIZE];
} NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_GET_NUM_HEADS
 *
 * This commands returns the number of heads supported by the specified
 * subdevice and available for use by displays.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   flags
 *     This parameter specifies optional flags to be used to while retrieving
 *     the number of heads.
 *     Possible valid flags are:
 *       NV0073_CTRL_SYSTEM_GET_NUM_HEADS_CLIENT
 *         This flag is used to request the number of heads that are
 *         currently in use by an NV client using a user display class
 *         instance (see NV15_VIDEO_LUT_CURSOR_DAC for an examle).  If this
 *         flag is disabled then the total number of heads supported is
 *         returned.
 *   numHeads
 *     This parameter returns the number of usable heads for the specified
 *     subdevice.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SYSTEM_GET_NUM_HEADS (0x730102U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_NUM_HEADS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_NUM_HEADS_PARAMS_MESSAGE_ID (0x02U)

typedef struct NV0073_CTRL_SYSTEM_GET_NUM_HEADS_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 flags;
    NvU32 numHeads;
} NV0073_CTRL_SYSTEM_GET_NUM_HEADS_PARAMS;

/* valid get num heads flags */
#define NV0073_CTRL_SYSTEM_GET_NUM_HEADS_FLAGS_CLIENT              0:0
#define NV0073_CTRL_SYSTEM_GET_NUM_HEADS_FLAGS_CLIENT_DISABLE (0x00000000U)
#define NV0073_CTRL_SYSTEM_GET_NUM_HEADS_FLAGS_CLIENT_ENABLE  (0x00000001U)




/*
 * NV0073_CTRL_CMD_SYSTEM_GET_SCANLINE
 *
 * This command returns the current RG scanline of the specified head on the
 * specified subdevice. To get the DMI scanline on supported chips, use
 * NV0073_CTRL_CMD_SYSTEM_GET_DMI_SCANLINE
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   head
 *     This parameter specifies the head for which the active display
 *     should be retrieved.  This value must be between zero and the
 *     maximum number of heads supported by the subdevice.
 *   currentScanline
 *     This parameter returns the current RG scanline value for the specified
 *     head.  If the head does not have a valid mode enabled then a scanline
 *     value of 0xffffffff is returned.
 *   bStereoEyeSupported (out)
 *     This parameter specifies whether stereoEye reporting is supported (this
 *     is hw dependent). Note that this value doesn't actually reflect whether
 *     given head is really in stereo mode.
 *   stereoEye (out)
 *     If supported (ie bStereoEyeSupported is TRUE), this parameter returns
 *     either NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS_RIGHT_EYE or
 *     NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS_LEFT_EYE, reflecting the
 *     stereo eye that is currently scanned out. Although this value typically
 *     changes at the beginning of vblank, the exact guarantee isn't more
 *     accurate than "somewhere in vblank".
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SYSTEM_GET_SCANLINE           (0x730104U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_SYSTEM_GET_SCANLINE_RIGHT_EYE 0x00000000U
#define NV0073_CTRL_CMD_SYSTEM_GET_SCANLINE_LEFT_EYE  0x00000001U

#define NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS_MESSAGE_ID (0x04U)

typedef struct NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  head;
    NvU32  currentScanline;
    NvBool bStereoEyeSupported;
    NvU32  stereoEye;
} NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_GET_VBLANK_COUNTER
 *
 * This command returns the current VBlank counter of the specified head on the
 * specified subdevice.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   head
 *     This parameter specifies the head for which the vblank counter
 *     should be retrieved.  This value must be between zero and the
 *     maximum number of heads supported by the subdevice.
 *   lowLatencyHint
 *     RM maintains several different vblank counts.  When this parameter is
 *     NV_TRUE, the command may return the low latency count.
 *   verticalBlankCounter
 *     This parameter returns the vblank counter value for the specified
 *     head. If the display mode is not valid or vblank not active then
 *     the verticalBlankCounter value is undefined.
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SYSTEM_GET_VBLANK_COUNTER (0x730105U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS_MESSAGE_ID (0x05U)

typedef struct NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  head;
    NvBool lowLatencyHint;
    NvU32  verticalBlankCounter;
} NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_GET_VBLANK_ENABLE
 *
 * This command returns the current VBlank enable status for the specified
 * head.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   head
 *     This parameter specifies the head for which the vblank status
 *     should be retrieved.  This value must be between zero and the
 *     maximum number of heads supported by the subdevice.
 *   bEnabled
 *     This parameter returns the vblank enable status for the specified head.
 *     A value of NV_FALSE indicates that vblank interrupts are not currently
 *     enabled while a value of NV_TRUE indicates that vblank are currently
 *     enabled.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SYSTEM_GET_VBLANK_ENABLE (0x730106U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_VBLANK_ENABLE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_VBLANK_ENABLE_PARAMS_MESSAGE_ID (0x06U)

typedef struct NV0073_CTRL_SYSTEM_GET_VBLANK_ENABLE_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  head;
    NvBool bEnabled;
} NV0073_CTRL_SYSTEM_GET_VBLANK_ENABLE_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_GET_SUPPORTED
 *
 * This command returns the set of supported display IDs for the specified
 * subdevice in the form of a 32bit display mask.  State from internal
 * display connectivity tables is used to determine the set of possible
 * display connections for the GPU.  The presence of a display in the
 * display mask only indicates the display is supported.  The connectivity
 * status of the display should be determined using the
 * NV0073_CTRL_CMD_SYSTEM_GET_CONNECT_STATE command.  The displayMask
 * value returned by NV0073_CTRL_CMD_SYSTEM_GET_SUPPORTED is static
 * and will remain consistent across boots of the system.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayMask
 *     This parameter returns a NV0073_DISPLAY_MASK value describing the set
 *     of displays supported by the subdevice.  An enabled bit in displayMask
 *     indicates the support of a display device with that displayId.
 *   displayMaskDDC
 *     This parameter returns a NV0073_DISPLAY_MASK value, indicating the
 *     subset of displayMask that supports DDC.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SYSTEM_GET_SUPPORTED (0x730107U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS_MESSAGE_ID (0x07U)

typedef struct NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayMask;
    NvU32 displayMaskDDC;
} NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_GET_CONNECT_STATE
 *
 * This command can be used to check the presence of a mask of display
 * devices on the specified subdevice.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   flags
 *     This parameter specifies optional flags to be used while retrieving
 *     the connection state information.
 *     Here are the current defined fields:
 *       NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_METHOD
 *         A client uses this field to indicate what method it wishes the
 *         system to use when determining the presence of attached displays.
 *         Possible values are:
 *            NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_METHOD_DEFAULT
 *              The system decides what method to use.
 *            NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_METHOD_CACHED
 *              Return the last full detection state for the display mask.
 *                   safety.)
 *            NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_METHOD_ECONODDC
 *              Ping the DDC address of the given display mask to check for
 *              a connected device. This is a lightweight method to check
 *              for a present device.
 *       NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_DDC
 *         A client uses this field to indicate whether to allow DDC during
 *         this detection or to not use it.
 *         Possible values are:
 *            NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_DDC_DEFAULT
 *              The system will use DDC as needed for each display.
 *            NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_DDC_DISABLE
 *              The system will not use DDC for any display. If DDC is
 *              disabled, this detection state will not be cached.
 *       NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_LOAD
 *         A client uses this field to indicate whether to detect loads
 *         during this detection or to not use it.
 *         Possible values are:
 *            NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_LOAD_DEFAULT
 *              The system will use load detection as needed for each display.
 *            NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_LOAD_DISABLE
 *              The system will not use load detection for any display. If
 *              load detection is disabled, this detection state will not
 *              be cached.
 *   displayMask
 *     This parameter specifies an NV0073_DISPLAY_MASK value describing
 *     the set of displays for which connectivity status is to be checked.
 *     If a display is present then the corresponding bit in the display
 *     mask is left enabled.  If the display is not present then the
 *     corresponding bit in the display mask is disabled.  Upon return this
 *     parameter contains the subset of displays in the mask that are
 *     connected.
 *
 *     If displayMask includes bit(s) that correspond to a TV encoder, the
 *     result will be simply 'yes' or 'no' without any indication of which
 *     connector(s) are actually attached.  For fine-grained TV attachment
 *     detection, please see NV0073_CTRL_CMD_TV_GET_ATTACHMENT_STATUS.
 *   retryTimeMs
 *     This parameter is an output to this command.  In case of
 *     NVOS_STATUS_ERROR_RETRY return status, this parameter returns the time
 *     duration in milli-seconds after which client should retry this command.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NVOS_STATUS_ERROR_RETRY
 */
#define NV0073_CTRL_CMD_SYSTEM_GET_CONNECT_STATE (0x730108U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_PARAMS_MESSAGE_ID (0x08U)

typedef struct NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 flags;
    NvU32 displayMask;
    NvU32 retryTimeMs;
} NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_PARAMS;

/* valid get connect state flags */
#define NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_METHOD                  1:0
#define NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_METHOD_DEFAULT  (0x00000000U)
#define NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_METHOD_CACHED   (0x00000001U)
#define NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_METHOD_ECONODDC (0x00000002U)
#define NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_DDC                     4:4
#define NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_DDC_DEFAULT     (0x00000000U)
#define NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_DDC_DISABLE     (0x00000001U)
#define NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_LOAD                    5:5
#define NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_LOAD_DEFAULT    (0x00000000U)
#define NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_LOAD_DISABLE    (0x00000001U)
#define NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_VBLANK                  6:6
#define NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_VBLANK_DEFAULT  (0x00000000U)
#define NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_FLAGS_VBLANK_SAFE     (0x00000001U)


/*
 * NV0073_CTRL_CMD_SYSTEM_GET_HOTPLUG_CONFIG
 *
 * This command can be used to retrieve dynamic hotplug state information that
 * are currently recorded by the RM. This information can be used by the client
 * to determine which displays to detect after a hotplug event occurs.  Or if
 * the client knows that this device generates a hot plug/unplug signal on all
 * connectors, then this can be used to cull displays from detection.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   flags
 *     This parameter specifies optional flags to be used while retrieving
 *     or changing the hotplug configuration.
 *       No flags are currently defined.
 *   hotplugEventMask
 *     For _GET_HOTPLUG_CONFIG, this returns which connectors the client
 *     has asked for notifications for, when a hotplug event is detected.
 *     Events can only be provided for connectors whose displayID is set
 *     by the system in the hotplugInterruptible field.
 *   hotplugPollable
 *     For _GET_HOTPLUG_CONFIG, this returns which connectors are pollable
 *     in some non-destructive fashion.
 *   hotplugInterruptible
 *     For _GET_HOTPLUG_CONFIG, this returns which connectors are capable
 *     of generating interrupts.
 *
 *     This display mask specifies an NV0073_DISPLAY_MASK value describing
 *     the set of displays that have seen a hotplug or hotunplug event
 *     sometime after the last valid EDID read.  If the device never has
 *     a valid EDID read, then it will always be listed here.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */


#define NV0073_CTRL_CMD_SYSTEM_GET_HOTPLUG_CONFIG                  (0x730109U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS_MESSAGE_ID (0x09U)

typedef struct NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 flags;
    NvU32 hotplugEventMask;
    NvU32 hotplugPollable;
    NvU32 hotplugInterruptible;
    NvU32 hotplugAlwaysAttached;
} NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_GET_HOTPLUG_STATE
 *
 * This command can be used to retrieve dynamic hotplug state information that
 * are currently recorded by the RM. This information can be used by the client
 * to determine which displays to detect after a hotplug event occurs.  Or if
 * the client knows that this device generates a hot plug/unplug signal on all
 * connectors, then this can be used to cull displays from detection.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   flags
 *     This parameter specifies optional flags to be used while retrieving
 *     the hotplug state information.
 *     Here are the current defined fields:
 *       NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_FLAGS_LID
 *         A client uses this field to determine the lid state.
 *         Possible values are:
 *            NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_FLAGS_LID_OPEN
 *              The lid is open.
 *            NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_FLAGS_LID_CLOSED
 *              The lid is closed.  The client should remove devices as
 *              reported inside the
 *              NV0073_CTRL_SYSTEM_GET_CONNECT_POLICY_PARAMS.lidClosedMask.
 *   hotplugAfterEdidMask
 *     This display mask specifies an NV0073_DISPLAY_MASK value describing
 *     the set of displays that have seen a hotplug or hotunplug event
 *     sometime after the last valid EDID read.  If the device never has
 *     a valid EDID read, then it will always be listed here.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */


#define NV0073_CTRL_CMD_SYSTEM_GET_HOTPLUG_STATE (0x73010aU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_PARAMS_MESSAGE_ID (0x0AU)

typedef struct NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 flags;
    NvU32 hotplugAfterEdidMask;
} NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_PARAMS;

/* valid get hoplug state flags */
#define NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_FLAGS_LID                   0:0
#define NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_FLAGS_LID_OPEN   (0x00000000U)
#define NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_FLAGS_LID_CLOSED (0x00000001U)

/*
 * NV0073_CTRL_CMD_SYSTEM_GET_HEAD_ROUTING_MAP
 *
 * This command can be used to retrieve the suggested head routing map
 * for the specified display mask.  A head routing map describes the
 * suggested crtc (or head) assignments for each display in the specified
 * mask.
 *
 * Up to MAX_DISPLAYS displays may be specified in the display mask.  Displays
 * are numbered from zero beginning with the lowest bit position set in the
 * mask.  The corresponding head assignment for each of specified displays can
 * then be found in the respective per-device field in the routing map.
 *
 * If a particular display cannot be successfully assigned a position in the
 * head routing map then it is removed from the display mask.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayMask
 *     This parameter specifies the NV0073_DISPLAY_MASK value for which
 *     the head routing map is desired.  Each enabled bit indicates
 *     a display device to include in the routing map. Enabled bits
 *     must represent supported displays as indicated by the
 *     NV0073_CTRL_CMD_SYSTEM_GET_SUPPORTED command.  If a particular
 *     display cannot be included in the routing map then it's corresponding
 *     bit in the displayMask will be disabled.  A return value of 0 in
 *     displayMask indicates that a head routing map could not be constructed
 *     with the given display devices.
 *   oldDisplayMask
 *     This optional parameter specifies a prior display mask to be
 *     used when generating the head routing map to be returned in
 *     headRoutingMap.  Displays set in oldDisplayMask are retained
 *     if possible in the new routing map.
 *   oldHeadRoutingMap
 *     This optional parameter specifies a prior head routing map to be
 *     used when generating the new routing map to be returned in
 *     headRoutingMap.  Head assignments in oldHeadRoutingMap are
 *     retained if possible in the new routing map.
 *   headRoutingMap
 *     This parameter returns the new head routing map.  This parameter
 *     is organized into eight distinct fields, each containing the head
 *     assignment for the corresponding display in display mask.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SYSTEM_GET_HEAD_ROUTING_MAP           (0x73010bU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_HEAD_ROUTING_MAP_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_HEAD_ROUTING_MAP_PARAMS_MESSAGE_ID (0x0BU)

typedef struct NV0073_CTRL_SYSTEM_GET_HEAD_ROUTING_MAP_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayMask;
    NvU32 oldDisplayMask;
    NvU32 oldHeadRoutingMap;
    NvU32 headRoutingMap;
} NV0073_CTRL_SYSTEM_GET_HEAD_ROUTING_MAP_PARAMS;

/* maximum number of allowed displays in a routing map */
#define NV0073_CTRL_SYSTEM_HEAD_ROUTING_MAP_MAX_DISPLAYS (8U)

/* per-display head assignments in a routing map */
#define NV0073_CTRL_SYSTEM_HEAD_ROUTING_MAP_DISPLAY0               3:0
#define NV0073_CTRL_SYSTEM_HEAD_ROUTING_MAP_DISPLAY1               7:4
#define NV0073_CTRL_SYSTEM_HEAD_ROUTING_MAP_DISPLAY2               11:8
#define NV0073_CTRL_SYSTEM_HEAD_ROUTING_MAP_DISPLAY3               15:12
#define NV0073_CTRL_SYSTEM_HEAD_ROUTING_MAP_DISPLAY4               19:16
#define NV0073_CTRL_SYSTEM_HEAD_ROUTING_MAP_DISPLAY5               23:20
#define NV0073_CTRL_SYSTEM_HEAD_ROUTING_MAP_DISPLAY6               27:24
#define NV0073_CTRL_SYSTEM_HEAD_ROUTING_MAP_DISPLAY7               31:28


/*
 * NV0073_CTRL_CMD_SYSTEM_GET_ACTIVE
 *
 * This command returns the active display ID for the specified head
 * on the specified subdevice.  The active display may be established
 * at system boot by low-level software and can then be later modified
 * by an NV client using a user display class instance (see
 * NV15_VIDEO_LUT_CURSOR_DAC).
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   head
 *     This parameter specifies the head for which the active display
 *     should be retrieved.  This value must be between zero and the
 *     maximum number of heads supported by the subdevice.
 *   flags
 *     This parameter specifies optional flags to be used to while retrieving
 *     the active display information.
 *     Possible valid flags are:
 *       NV0073_CTRL_SYSTEM_GET_ACTIVE_FLAGS_CLIENT
 *         This flag is used to limit the search for the active display to
 *         that established by an NV client.  If this flag is not specified,
 *         then any active display is returned (setup at system boot by
 *         low-level software or later by an NV client).
 *   displayId
 *     This parameter returns the displayId of the active display.  A value
 *     of zero indicates no display is active.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SYSTEM_GET_ACTIVE                (0x73010cU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS_MESSAGE_ID (0x0CU)

typedef struct NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 head;
    NvU32 flags;
    NvU32 displayId;
} NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS;

/* valid get active flags */
#define NV0073_CTRL_SYSTEM_GET_ACTIVE_FLAGS_CLIENT                 0:0
#define NV0073_CTRL_SYSTEM_GET_ACTIVE_FLAGS_CLIENT_DISABLE (0x00000000U)
#define NV0073_CTRL_SYSTEM_GET_ACTIVE_FLAGS_CLIENT_ENABLE  (0x00000001U)



/*
 * NV0073_CTRL_SYSTEM_ACPI_ID_MAP
 *
 * This structure defines the mapping between the RM's displayId and the
 * defined ACPI IDs for each display.
 *   displayId
 *     This parameter is a handle to a single display output path from the
 *     GPU pins to the display connector.  Each display ID is defined by one bit.
 *     A zero in this parameter indicates a skip entry.
 *   acpiId
 *     This parameter defines the corresponding ACPI ID of the displayId.
 *   flags
 *     This parameter specifies optional flags that describe the association
 *     between the display ID and the ACPI ID.
 *       NV0073_CTRL_SYSTEM_ACPI_ID_MAP_ORIGIN
 *         This field describes where the ACPI was found.
 *           NV0073_CTRL_SYSTEM_ACPI_ID_MAP_ORIGIN_RM
 *             The ACPI ID was generated by RM code.
 *           NV0073_CTRL_SYSTEM_ACPI_ID_MAP_ORIGIN_DOD
 *             The ACPI ID was found via the ACPI _DOD call.
 *           NV0073_CTRL_SYSTEM_ACPI_ID_MAP_ORIGIN_CLIENT
 *             The ACPI ID was generated by RM Client and sent to RM. Note this
 *             must be set on a NV0073_CTRL_CMD_SYSTEM_SET_ACPI_ID_MAP call.
 *       NV0073_CTRL_SYSTEM_ACPI_ID_MAP_SNAG_UNDOCKED
 *         This flag explains that the ACPI ID is only valid when the system
 *         is undocked.  If this flag is not set, the ACPI ID is valid always.
 *       NV0073_CTRL_SYSTEM_ACPI_ID_MAP_SNAG_DOCKED
 *         This flag explains that the ACPI ID is only valid when the system
 *         is docked.  If this flag is not set, the ACPI ID is valid always.
 *       NV0073_CTRL_SYSTEM_ACPI_ID_MAP_DOD_BIOS_DETECT
 *         This flag is set only if the _DOD returns that the device can be
 *         detected by the system BIOS.  This flag is copied directly from
 *         the ACPI spec.
 *       NV0073_CTRL_SYSTEM_ACPI_ID_MAP_DOD_NON_VGA_OUTPUT
 *         This flag is set only if the _DOD returns that the device is
 *         a non-VGA device whose power is related to the VGA device.
 *         i.e. TV tuner, DVD decoder, Video capture. This flag is copied
 *         directly from the ACPI spec.
 *       NV0073_CTRL_SYSTEM_ACPI_ID_MAP_DOD_MULTIHEAD_ID
 *         This value is set only if the _DOD returns it.  The number
 *         indicates the head output of a multi-head device. This has no
 *         relation to the term, Head, currently used in the RM today.
 *         This is strictly a copy of the value directly from the ACPI spec.
 *       NV0073_CTRL_SYSTEM_ACPI_ID_MAP_DOD_SCHEME
 *         This flag is set only if the _DOD returns that the acpiID follows
 *         the ACPI 3.0 spec.  This flag is copied directly from
 *         the ACPI spec.
 *
 */

typedef struct NV0073_CTRL_SYSTEM_ACPI_ID_MAP_PARAMS {
    NvU32 displayId;
    NvU32 acpiId;
    NvU32 flags;
} NV0073_CTRL_SYSTEM_ACPI_ID_MAP_PARAMS;

#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_ORIGIN                            1:0
#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_ORIGIN_RM                0x00000000U
#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_ORIGIN_DOD               0x00000001U
#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_ORIGIN_CLIENT            0x00000002U

#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_SNAG_UNDOCKED                     2:2
#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_SNAG_UNDOCKED_FALSE      0x00000000U
#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_SNAG_UNDOCKED_TRUE       0x00000001U

#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_SNAG_DOCKED                       3:3
#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_SNAG_DOCKED_FALSE        0x00000000U
#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_SNAG_DOCKED_TRUE         0x00000001U

#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_DOD_BIOS_DETECT                 16:16
#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_DOD_BIOS_DETECT_FALSE    0x00000000U
#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_DOD_BIOS_DETECT_TRUE     0x00000001U

#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_DOD_NON_VGA_OUTPUT              17:17
#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_DOD_NON_VGA_OUTPUT_FALSE 0x00000000U
#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_DOD_NON_VGA_OUTPUT_TRUE  0x00000001U

#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_DOD_MULTIHEAD_ID                20:18

#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_DOD_SCHEME                      31:31
#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_DOD_SCHEME_VENDOR        0x00000000U
#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_DOD_SCHEME_30            0x00000001U

#define NV0073_CTRL_SYSTEM_ACPI_ID_MAP_MAX_DISPLAYS             (16U)

/*
 * NV0073_CTRL_CMD_SYSTEM_GET_ACPI_ID_MAP
 *
 * This command retrieves the mapping between the RM's displayId and the
 * defined ACPI IDs for each display.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and
 *     the total number of subdevices within the parent device.  It should
 *     be set to zero for default behavior.
 *   NV0073_CTRL_SYSTEM_ACPI_ID_MAP_PARAMS
 *     An array of display ID to ACPI ids with flags for each description.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *     Only returned if subdeviceInstance was not valid.
 */

#define NV0073_CTRL_CMD_SYSTEM_GET_ACPI_ID_MAP                  (0x730115U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_ACPI_ID_MAP_PARAMS_MESSAGE_ID" */


#define NV0073_CTRL_SYSTEM_GET_ACPI_ID_MAP_PARAMS_MESSAGE_ID (0x15U)

typedef struct NV0073_CTRL_SYSTEM_GET_ACPI_ID_MAP_PARAMS {
    NvU32                                 subDeviceInstance;
    NV0073_CTRL_SYSTEM_ACPI_ID_MAP_PARAMS acpiIdMap[NV0073_CTRL_SYSTEM_ACPI_ID_MAP_MAX_DISPLAYS];
} NV0073_CTRL_SYSTEM_GET_ACPI_ID_MAP_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_GET_INTERNAL_DISPLAYS
 *
 * This command returns the set of internal (safe) display IDs for the specified
 * subdevice in the form of a 32bit display mask. Safe means the displays do
 * not require copy protection as they are on the motherboard.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   internalDisplaysMask
 *     This parameter returns a NV0073_DISPLAY_MASK value describing the set
 *     of displays that are internal (safe) and which do not require copy
 *     protection schemes.
 *   availableInternalDisplaysMask
 *     This parameter returns a NV0073_DISPLAY_MASK value describing the set
 *     of displays that are internal and available for use.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SYSTEM_GET_INTERNAL_DISPLAYS (0x730116U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS_MESSAGE_ID (0x16U)

typedef struct NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 internalDisplaysMask;
    NvU32 availableInternalDisplaysMask;
} NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED
 *
 * This command is used to notify RM that all subdevices are ready for ACPI
 * calls. The caller must make sure that the OS is ready to handle the ACPI
 * calls for each ACPI ID. So, this call must be done after the OS has
 * initialized all the display ACPI IDs to this subdevice.
 * Besides, the ACPI spec provides a function for the display drivers to read
 * the EDID directly from the SBIOS for each display's ACPI ID. This function
 * is used to override the EDID found from a I2C or DPAux based transaction.
 * This command will also attempt to call the ACPI _DDC function to read the
 * EDID from the SBIOS for all displayIDs. If an EDID is found from this call,
 * the RM will store that new EDID in the EDID buffer of that OD.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *
 * Possible status values returned are:
 * NV_OK
 * NV_ERR_INVALID_PARAM_STRUCT
 * NV_ERR_NOT_SUPPORTED
 *
 */

#define NV0073_CTRL_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED_PARAMS_MESSAGE_ID (0x17U)

typedef struct NV0073_CTRL_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED_PARAMS {
    NvU32 subDeviceInstance;
} NV0073_CTRL_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED_PARAMS;

#define NV0073_CTRL_CMD_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED (0x730117U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED_PARAMS_MESSAGE_ID" */


/*
 * NV0073_CTRL_SYSTEM_CONNECTOR_INFO
 *
 * This structure describes a single connector table entry.
 *
 *   type
 *     This field specifies the connector type.
 *   displayMask
 *     This field specifies the the displayMask to which the connector belongs.
 *   location
 *     This field specifies the placement of the connector on the platform.
 *   hotplug
 *     This field specifies hotplug capabilities (if any) for the connector.
 */
typedef struct NV0073_CTRL_SYSTEM_CONNECTOR_INFO {
    NvU32 type;
    NvU32 displayMask;
    NvU32 location;
    NvU32 hotplug;
} NV0073_CTRL_SYSTEM_CONNECTOR_INFO;

/* valid type values */
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_VGA_15_PIN               (0x00000000U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_DVI_A                    (0x00000001U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_POD_VGA_15_PIN           (0x00000002U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_TV_COMPOSITE             (0x00000010U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_TV_SVIDEO                (0x00000011U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_TV_SVIDEO_BO_COMPOSITE   (0x00000012U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_TV_COMPONENT             (0x00000013U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_TV_SCART                 (0x00000014U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_TV_SCART_EIAJ4120        (0x00000014U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_TV_EIAJ4120              (0x00000017U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_PC_POD_HDTV_YPRPB        (0x00000018U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_PC_POD_SVIDEO            (0x00000019U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_PC_POD_COMPOSITE         (0x0000001AU)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_DVI_I_TV_SVIDEO          (0x00000020U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_DVI_I_TV_COMPOSITE       (0x00000021U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_DVI_I_TV_SV_BO_COMPOSITE (0x00000022U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_DVI_I                    (0x00000030U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_DVI_D                    (0x00000031U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_ADC                      (0x00000032U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_LFH_DVI_I_1              (0x00000038U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_LFH_DVI_I_2              (0x00000039U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_LFH_SVIDEO               (0x0000003AU)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_BNC                      (0x0000003CU)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_LVDS_SPWG                (0x00000040U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_LVDS_OEM                 (0x00000041U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_LVDS_SPWG_DET            (0x00000042U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_LVDS_OEM_DET             (0x00000043U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_TVDS_OEM_ATT             (0x00000045U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_VGA_15_PIN_UNDOCKED      (0x00000050U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_VGA_15_PIN_DOCKED        (0x00000051U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_DVI_I_UNDOCKED           (0x00000052U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_DVI_I_DOCKED             (0x00000053U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_DVI_D_UNDOCKED           (0x00000052U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_DVI_D_DOCKED             (0x00000053U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_DP_EXT                   (0x00000056U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_DP_INT                   (0x00000057U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_DP_EXT_UNDOCKED          (0x00000058U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_DP_EXT_DOCKED            (0x00000059U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_3PIN_DIN_STEREO          (0x00000060U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_HDMI_A                   (0x00000061U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_AUDIO_SPDIF              (0x00000062U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_HDMI_C_MINI              (0x00000063U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_LFH_DP_1                 (0x00000064U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_LFH_DP_2                 (0x00000065U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_TYPE_VIRTUAL_WFD              (0x00000070U)

/* valid hotplug values */
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_HOTPLUG_A_SUPPORTED           (0x00000001U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_HOTPLUG_B_SUPPORTED           (0x00000002U)

/*
 * Nv0073_CTRL_CMD_SYSTEM_GET_CONNECTOR_TABLE
 *
 * This command can be used to retrieve display connector information.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   version
 *     This parameter returns the version of the connector table.
 *   platform
 *     This parameter returns the type of platform of the associated subdevice.
 *   connectorTableEntries
 *     This parameter returns the number of valid entries in the connector
 *     table.
 *   connectorTable
 *     This parameter returns the connector information in the form of an
 *     array of NV0073_CTRL_SYSTEM_CONNECTOR_INFO structures.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV0073_CTRL_CMD_SYSTEM_GET_CONNECTOR_TABLE                      (0x73011dU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_CONNECTOR_TABLE_PARAMS_MESSAGE_ID" */

/* maximum number of connector table entries */
#define NV0073_CTRL_SYSTEM_GET_CONNECTOR_TABLE_MAX_ENTRIES              (16U)

#define NV0073_CTRL_SYSTEM_GET_CONNECTOR_TABLE_PARAMS_MESSAGE_ID (0x1DU)

typedef struct NV0073_CTRL_SYSTEM_GET_CONNECTOR_TABLE_PARAMS {
    NvU32                             subDeviceInstance;
    NvU32                             version;
    NvU32                             platform;
    NvU32                             connectorTableEntries;
    /*
     * C form:
     * NV0073_CTRL_SYSTEM_CONNECTOR_INFO connectorTable[NV0073_CTRL_SYSTEM_CONNECTOR_TABLE_MAX_ENTRIES];
     */
    NV0073_CTRL_SYSTEM_CONNECTOR_INFO connectorTable[NV0073_CTRL_SYSTEM_GET_CONNECTOR_TABLE_MAX_ENTRIES];
} NV0073_CTRL_SYSTEM_GET_CONNECTOR_TABLE_PARAMS;

/* valid version values */
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_VERSION_30                     (0x00000030U)

/* valid platform values */
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_PLATFORM_DEFAULT_ADD_IN_CARD   (0x00000000U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_PLATFORM_TWO_PLATE_ADD_IN_CARD (0x00000001U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_PLATFORM_MOBILE_ADD_IN_CARD    (0x00000008U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_PLATFORM_MXM_MODULE            (0x00000009U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_PLATFORM_MOBILE_BACK           (0x00000010U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_PLATFORM_MOBILE_BACK_LEFT      (0x00000011U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_PLATFORM_MOBILE_BACK_DOCK      (0x00000018U)
#define NV0073_CTRL_SYSTEM_CONNECTOR_INFO_PLATFORM_CRUSH_DEFAULT         (0x00000020U)

/*
 * NV0073_CTRL_CMD_SYSTEM_GET_BOOT_DISPLAYS
 *
 * This command returns a mask of boot display IDs.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   bootDisplayMask
 *     This parameter returns the mask of boot display IDs.
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV0073_CTRL_CMD_SYSTEM_GET_BOOT_DISPLAYS                         (0x73011eU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS_MESSAGE_ID (0x1EU)

typedef struct NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 bootDisplayMask;
} NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD
 *
 * This command is used to execute general MXM ACPI methods.
 *
 * method
 *   This parameter identifies the MXM ACPI API to be invoked.
 *   Valid values for this parameter are:
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXMI
 *       This value specifies that the MXMI API is to invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXMS
 *       This value specifies that the MXMS API is to invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXMX
 *       This value specifies that the MXMX API is to invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_GPUON
 *       This value specifies that the Hybrid GPU ON API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_GPUOFF
 *       This value specifies that the Hybrid GPU OFF API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_GPUSTA
 *       This value specifies that the Hybrid GPU STA API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXDS
 *       This value specifies that the Hybrid GPU MXDS API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NVHG_MXMX
 *       This value specifies that the Hybrid GPU MXMX API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DOS
 *       This value specifies that the Hybrid GPU DOS API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_ROM
 *       This value specifies that the Hybrid GPU ROM API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DOD
 *       This value specifies that the Hybrid GPU DOD API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_SUPPORT
 *       This value specifies that the Hybrid GPU DSM subfunction SUPPORT
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HYBRIDCAPS
 *       This value specifies that the Hybrid GPU DSM subfunction SUPPORT
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_POLICYSELECT
 *       This value specifies that the Hybrid GPU DSM subfunction POLICYSELECT
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_POWERCONTROL
 *       This value specifies that the Hybrid GPU DSM subfunction POWERCONTROL
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_PLATPOLICY
 *       This value specifies that the Hybrid GPU DSM subfunction PLATPOLICY
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_DISPLAYSTATUS
 *       This value specifies that the Hybrid GPU DSM subfunction DISPLAYSTATUS
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MDTL
 *       This value specifies that the Hybrid GPU DSM subfunction MDTL
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCSMBLIST
 *       This value specifies that the Hybrid GPU DSM subfunction HCSMBLIST
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCSMBADDR
 *       This value specifies that the Hybrid GPU DSM subfunction HCSMBADDR
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCREADBYTE
 *       This value specifies that the Hybrid GPU DSM subfunction HCREADBYTE
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCSENDBYTE
 *       This value specifies that the Hybrid GPU DSM subfunction HCSENDBYTES
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCGETSTATUS
 *       This value specifies that the Hybrid GPU DSM subfunction HCGETSTATUS
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCTRIGDDC
 *       This value specifies that the Hybrid GPU DSM subfunction HCTRIGDDC
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCGETDDC
 *       This value specifies that the Hybrid GPU DSM subfunction HCGETDDC
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DCS
 *       This value specifies that the Hybrid GPU DCS API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXSS
 *       This value specifies that the DSM MXM subfunction MXSS
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXMI
 *       This value specifies that the DSM MXM subfunction MXMI
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXMS
 *       This value specifies that the DSM MXM subfunction MXMS
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXPP
 *       This value specifies that the DSM MXM subfunction MXPP
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXDP
 *       This value specifies that the DSM MXM subfunction MXDP
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MDTL
 *       This value specifies that the DSM MXM subfunction MDTL
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXCB
 *       This value specifies that the DSM MXM subfunction MXCB
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_CTL_REMAPFUNC
 *       This value specifies the DSM generic remapping should return function
 *       and subfunction when this API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_HYBRIDCAPS
 *       This value specifies that the generic DSM subfunction HYBRIDCAPS
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_POLICYSELECT
 *       This value specifies that the generic DSM subfunction POLICYSELECT
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_PLATPOLICY
 *       This value specifies that the generic DSM subfunction PLATPOLICY
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_DISPLAYSTATUS
 *       This value specifies that the generic DSM subfunction DISPLAYSTATUS
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_MDTL
 *       This value specifies that the generic DSM subfunction MDTL
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_GETOBJBYTYPE
 *       This value specifies that the generic DSM subfunction GETOBJBYTYPE
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_GETALLOBJS
 *       This value specifies that the generic DSM subfunction GETALLOBJS
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_GETEVENTLIST
 *       This value specifies that the generic DSM subfunction GETEVENTLIST
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_GETBACKLIGHT
 *       This value specifies that the generic DSM subfunction GETBACKLIGHT
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_CTL_TESTSUBFUNCENABLED
 *       This value specifies the testIfDsmSubFunctionEnabled test should
 *       be done for the func/subfunction when this API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_CTL_GETSUPPORTEDFUNC
 *       This value specifies the list of supported generic dsm functions
 *       should be returned.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NVOP_OPTIMUSCAPS
 *       This value specifies that the DSM NVOP subfunction OPTIMUSCAPS
 *       API is to be invoked.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NVOP_OPTIMUSFLAG
 *       This value specifies that the DSM NVOP subfunction OPTIMUSFLAG
 *       API is to be invoked. This API will set a Flag in sbios to Indicate
 *       that HD Audio Controller is disable/Enabled from GPU Config space.
 *       This flag will be used by sbios to restore Audio state after resuming
 *       from s3/s4.
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_WMMX_NVOP_GPUON
 *      This value specifies that the WMMX (WMI-ACPI) GPON methods has to be invoked
 *      this call should happen below DPC level from any client.
 * inData
 *   This parameter specifies the method-specific input buffer.  Data is
 *   passed to the specified API using this buffer.  For display related
 *   APIs the associated display mask can be found at a byte offset within
 *   the inData buffer using the following method-specific values:
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXMX_DISP_MASK_OFFSET
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXDS_DISP_MASK_OFFSET
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NVHG_MXMX_DISP_MASK_OFFSET
 *     NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DOS_DISP_MASK_OFFSET
 * inDataSize
 *   This parameter specifies the size of the inData buffer in bytes.
 * outStatus
 *   This parameter returns the status code from the associated ACPI call.
 * outData
 *   This parameter specifies the method-specific output buffer.  Data
 *   is returned by the specified API using this buffer.
 * outDataSize
 *   This parameter specifies the size of the outData buffer in bytes.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0073_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD (0x730120U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS_MESSAGE_ID (0x20U)

typedef struct NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS {
    NvU32 method;
    NV_DECLARE_ALIGNED(NvP64 inData, 8);
    NvU16 inDataSize;
    NvU32 outStatus;
    NV_DECLARE_ALIGNED(NvP64 outData, 8);
    NvU16 outDataSize;
} NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS;


/* valid method parameter values */
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXMX                               (0x00000002U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXMX_DISP_MASK_OFFSET              (0x00000001U)

#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_GPUON                              (0x00000003U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_GPUOFF                             (0x00000004U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_GPUSTA                             (0x00000005U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXDS                               (0x00000006U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NVHG_MXMX                          (0x00000007U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DOS                                (0x00000008U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_ROM                                (0x00000009U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DOD                                (0x0000000aU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_SUPPORT                        (0x0000000bU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HYBRIDCAPS                     (0x0000000cU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_POLICYSELECT                   (0x0000000dU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_POWERCONTROL                   (0x0000000eU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_PLATPOLICY                     (0x0000000fU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_DISPLAYSTATUS                  (0x00000010U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MDTL                           (0x00000011U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCSMBLIST                      (0x00000012U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCSMBADDR                      (0x00000013U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCREADBYTE                     (0x00000014U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCSENDBYTE                     (0x00000015U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCGETSTATUS                    (0x00000016U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCTRIGDDC                      (0x00000017U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCGETDDC                       (0x00000018U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DCS                                (0x00000019U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXSS                       (0x0000001aU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXMI                       (0x0000001bU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXMS                       (0x0000001cU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXPP                       (0x0000001dU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXDP                       (0x0000001eU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MDTL                       (0x0000001fU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXCB                       (0x00000020U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_GETEVENTLIST               (0x00000021U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GETMEMTABLE                    (0x00000022U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GETMEMCFG                      (0x00000023U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GETOBJBYTYPE                   (0x00000024U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GETALLOBJS                     (0x00000025U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_CTL_REMAPFUNC          (0x00000026U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_DISPLAYSTATUS          (0x0000002aU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_MDTL                   (0x0000002bU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_GETOBJBYTYPE           (0x0000002cU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_GETALLOBJS             (0x0000002dU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_GETEVENTLIST           (0x0000002eU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_GETBACKLIGHT           (0x0000002fU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_CTL_TESTSUBFUNCENABLED (0x00000030U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_CTL_GETSUPPORTEDFUNC   (0x00000031U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NVOP_OPTIMUSCAPS               (0x00000032U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_WMMX_NVOP_GPUON                    (0x00000033U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NVOP_OPTIMUSFLAG               (0x00000034U)


#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_GETCALLBACKS           (0x00000036U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NBCI_SUPPORTFUNCS              (0x00000037U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NBCI_PLATCAPS                  (0x00000038U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NBCI_PLATPOLICY                (0x00000039U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_MSTL                   (0x0000003aU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NVGPS_FUNC_SUPPORT             (0x0000003bU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_MXDS                          (0x0000003cU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_MXDM                          (0x0000003dU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_MXID                          (0x0000003eU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_LRST                          (0x0000003fU)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DDC_EDID                           (0x00000040U)

/* valid input buffer offset values */
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXDS_DISP_MASK_OFFSET              (0x00000004U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NVHG_MXMX_DISP_MASK_OFFSET         (0x00000004U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DOS_DISP_MASK_OFFSET               (0x00000004U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_MXDS_DISP_MASK_OFFSET         (0x00000004U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_MXDM_DISP_MASK_OFFSET         (0x00000004U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_MXID_DISP_MASK_OFFSET         (0x00000004U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_LRST_DISP_MASK_OFFSET         (0x00000004U)
#define NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DDC_EDID_DISP_MASK_OFFSET          (0x00000004U)



/*
* NV0073_CTRL_SYSTEM_VRR_DISPLAY_INFO_PARAMS
*
* This command is used to update information about VRR capable monitors
*   subDeviceInstance
*     This parameter specifies the subdevice instance within the
*     NV04_DISPLAY_COMMON parent device to which the operation should be
*     directed.This parameter must specify a value between zero and the
*     total number of subdevices within the parent device.This parameter
*     should be set to zero for default behavior.
*
*   displayId
*     DisplayId of the panel for which client wants to add or remove from VRR
*     capable monitor list
*
*   bAddition
*     When set to NV_TRUE, signifies that the vrr monitor is to be added.
*     When set to NV_FALSE, signifies that the vrr monitor is to be removed.
*
*/
#define NV0073_CTRL_CMD_SYSTEM_VRR_DISPLAY_INFO (0x73012cU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_VRR_DISPLAY_INFO_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_VRR_DISPLAY_INFO_PARAMS_MESSAGE_ID (0x2CU)

typedef struct NV0073_CTRL_SYSTEM_VRR_DISPLAY_INFO_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvBool bAddition;
} NV0073_CTRL_SYSTEM_VRR_DISPLAY_INFO_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_GET_HOTPLUG_UNPLUG_STATE
 *
 * This command can be used to retrieve hotplug and unplug state
 * information that are currently recorded by the RM. This information is
 * used by the client to determine which displays to detect after a
 * hotplug event occurs. Or if  the client knows that this device generates
 * a hot plug/unplug signal on all connectors, then this can be used to call
 * displays from detection. The displayIds on which hotplug/unplug has
 * happened will be reported only ONCE to the client. That is if the call
 * is done multiple times for the same event update, then for consequent
 * calls the display mask will be reported as 0.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   flags
 *     This parameter specifies optional flags to be used while retrieving
 *     the hotplug state information.
 *     Here are the current defined fields:
 *       NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_FLAGS_LID
 *         A client uses this field to determine the lid state.
 *         Possible values are:
 *       NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_FLAGS_LID_OPEN
 *              The lid is open.
 *       NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_FLAGS_LID_CLOSED
 *              The lid is closed.  The client should remove devices a
 *              reported inside the
 *              NV0073_CTRL_SYSTEM_GET_CONNECT_POLICY_PARAMS.lidClosedMask.
 *   hotPlugMask
 *     This display mask specifies an NV0073_DISPLAY_MASK value describing
 *     the set of displays that have seen a hotplug.
 *   hotUnplugMask
 *     This display mask specifies an NV0073_DISPLAY_MASK value describing
 *     the set of displays that have seen a hot unplug
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0073_CTRL_CMD_SYSTEM_GET_HOTPLUG_UNPLUG_STATE (0x73012dU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS_MESSAGE_ID (0x2DU)

typedef struct NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 flags;
    NvU32 hotPlugMask;
    NvU32 hotUnplugMask;
} NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS;

/* valid get hoplug state flags */
#define NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_FLAGS_LID                   0:0
#define NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_FLAGS_LID_OPEN   (0x00000000U)
#define NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_FLAGS_LID_CLOSED (0x00000001U)

/*
 * NV0073_CTRL_CMD_SYSTEM_CLEAR_ELV_BLOCK
 *
 * This command instructs the RM to explicitly clear any
 * ELV block.  Clients should call this before attempting core-channel
 * updates when in VRR one-shot mode.  ELV block mode will be
 * properly restored to its appropriate setting based on the stall-lock
 * in Supervisor3 after the core channel update
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *
 *   displayId
 *     The public ID of the Output Display which is to be used for VRR.
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_INVALID_STATE
 */

#define NV0073_CTRL_CMD_SYSTEM_CLEAR_ELV_BLOCK                       (0x73012eU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_CLEAR_ELV_BLOCK_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_CLEAR_ELV_BLOCK_PARAMS_MESSAGE_ID (0x2EU)

typedef struct NV0073_CTRL_SYSTEM_CLEAR_ELV_BLOCK_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
} NV0073_CTRL_SYSTEM_CLEAR_ELV_BLOCK_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR
 *
 * This command arms the display modeset supervisor to operate in
 * a lightweight mode.  By calling this, the client is implicitly
 * promising not to make any changes in the next modeset that require
 * the full supervisor.  After SV3, the LWSV will disarm and any subsequent
 * modesets will revert to full supervisors.  This must be called separately
 * for every display that will be part of the modeset.
 * It is recommended that the client explicitly disarm the lightweight
 * supervisor after every modeset as null modesets will not trigger the
 * supervisor interrupts and the RM will not be able to disarm automatically
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *
 *   displayId
 *     The public ID of the Output Display which is to be used for VRR.
 *
 *   bArmLWSV
 *     If this is set to NV_TRUE, the RM will arm the lightweight supervisor
 *     for the next modeset.
 *     If this is set to NV_FALSE, the RM will disarm the lightweight supervisor
 *
 *   bVrrState
 *     VRR state to be changed.
 *
 *   vActive
 *      GPU-SRC vertical active value
 *
 *   vfp
 *      GPU-SRC vertical front porch
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_INVALID_STATE
 */

#define NV0073_CTRL_CMD_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR (0x73012fU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR_PARAMS_MESSAGE_ID (0x2FU)

typedef struct NV0073_CTRL_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvBool bArmLWSV;
    NvBool bVrrState;
    NvU32  vActive;
    NvU32  vfp;
} NV0073_CTRL_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR_PARAMS;



/*
* NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS
*
* This command is used to configure pstate switch parameters on VRR monitors
*   subDeviceInstance
*     This parameter specifies the subdevice instance within the
*     NV04_DISPLAY_COMMON parent device to which the operation should be
*     directed.This parameter must specify a value between zero and the
*     total number of subdevices within the parent device.This parameter
*     should be set to zero for default behavior.
*
*   displayId
*     DisplayId of the monitor being vrr configured
*
*   bVrrState
*     When set to NV_TRUE, signifies that the vrr is about to become active.
*     When set to NV_FALSE, signifies that the vrr is about to become suspended.
*
*   bVrrDirty
*     When set to NV_TRUE, indicates that vrr configuration has been changed
*     When set to NV_FALSE, this will indicate transitions from One shot mode to
*     Continuous mode and vice versa
*
*   bVrrEnabled
*     When set to NV_TRUE, indicates that vrr has been enabled, i.e. vBp extended by 2 lines
*
*   maxVblankExtension
*     When VRR is enabled, this is the maximum amount of lines that the vblank can be extended.
*     Only updated when bVrrDirty = true
*
*   internalVRRHeadVblankStretch
*     When VRR is enabled, this is the maximum amount of lines that the vblank can be extended.
*     On NVSR and DD panels . Only updated when bVrrDirty = true
*
*   minVblankExtension
*     When VRR is enabled, this is the minimum amount of lines that should be present in the Vblank. The purpose is to cap the maximum refresh (currently only for HDMI 2.1 VRR compliance)
*/
#define NV0073_CTRL_CMD_SYSTEM_CONFIG_VRR_PSTATE_SWITCH (0x730134U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS_MESSAGE_ID (0x34U)

typedef struct NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvBool bVrrState;
    NvBool bVrrDirty;
    NvBool bVrrEnabled;
    NvU32  maxVblankExtension;
    NvU32  internalVRRHeadVblankStretch;
    NvU32  minVblankExtension;
} NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_INLINE_DISP_INTR_SERVICE_WAR_FOR_VR
 *
 * This command engages the WAR when VR devices are connected,
 * where the Pstate switching can cause delay in Vblank callbacks
 * reported to KMD, by servicing disp interrupts inline and reporting the
 * callbacks to KMD. Without the WAR, there can be stutters during pstate switch.
 * Bug#1778552
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *   bEngageWAR
 *     Indicates if inline disp interrupt serving WAR has to be engaged or
 *     disengaged.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0073_CTRL_CMD_SYSTEM_INLINE_DISP_INTR_SERVICE_WAR_FOR_VR (0x730187U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_INLINE_DISP_INTR_SERVICE_WAR_FOR_VR_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_INLINE_DISP_INTR_SERVICE_WAR_FOR_VR_PARAMS_MESSAGE_ID (0x87U)

typedef struct NV0073_CTRL_SYSTEM_INLINE_DISP_INTR_SERVICE_WAR_FOR_VR_PARAMS {
    NvU32  subDeviceInstance;
    NvBool bEngageWAR;
} NV0073_CTRL_SYSTEM_INLINE_DISP_INTR_SERVICE_WAR_FOR_VR_PARAMS;



/*
 * NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX
 *
 * This command is used to query the display mask of all displays
 * that support dynamic display MUX.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *   displayMask (out)
 *     Mask of all displays that support dynamic display MUX
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX (0x73013dU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX_PARAMS_MESSAGE_ID (0x3DU)

typedef struct NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 muxDisplayMask;
} NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX_PARAMS;



/*
 * NV0073_CTRL_CMD_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH
 *
 * This command allocates a specified amount of ISO memory bandwidth for
 * display.  If the requested amount of bandwidth cannot be allocated (either
 * because it exceeds the total bandwidth available to the system, or because
 * too much bandwidth is already allocated to other clients), the call will
 * fail and NV_ERR_INSUFFICIENT_RESOURCES will be returned.
 *
 * If bandwidth has already been allocated via a prior call, and a new
 * allocation is requested, the new allocation will replace the old one.  (If
 * the new allocation fails, the old allocation remains in effect.)
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *   averageBandwidthKBPS
 *     This parameter specifies the amount of ISO memory bandwidth requested.
 *   floorBandwidthKBPS
 *     This parameter specifies the minimum required (i.e., floor) dramclk
 *     frequency, multiplied by the width of the pipe over which the display
 *     data will travel.  (It is understood that the bandwidth calculated by
 *     multiplying the clock frequency by the pipe width will not be
 *     realistically achievable, due to overhead in the memory subsystem.  The
 *     API will not actually use the bandwidth value, except to reverse the
 *     calculation to get the required dramclk frequency.)
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INSUFFICIENT_RESOURCES
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_GENERIC
 */

#define NV0073_CTRL_CMD_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH (0x730143U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS_MESSAGE_ID (0x43U)

typedef struct NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 averageBandwidthKBPS;
    NvU32 floorBandwidthKBPS;
} NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS;

/*
 * NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS
 *
 * This structure represents the hotplug event config control parameters.
 *
 *   subDeviceInstance
 *     This parameter should specify the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *
 *   deviceMapFilter
 *     This parameter returns (in GET) or should specify (in SET) a device map
 *     indicating device(s) to sense.
 *
 *   hotPluginSense
 *     This parameter returns (in GET) or should specify (in SET) a device map
 *     indicating device(s) plugged in that caused the most recent hotplug
 *     event.
 *
 *   hotUnplugSense
 *     This parameter returns (in GET) or should specify (in SET) a device map
 *     indicating device(s) un plugged that caused the most recent hotplug
 *     event.
 */

typedef struct NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 deviceMapFilter;
    NvU32 hotPluginSense;
    NvU32 hotUnplugSense;
} NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_GET_HOTPLUG_EVENT_CONFIG
 *
 * This command fetches the hotplug event configuration.
 *
 * See @ref NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS for documentation on
 * the parameters.
 */

#define NV0073_CTRL_CMD_SYSTEM_GET_HOTPLUG_EVENT_CONFIG (0x730144U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_HOTPLUG_EVENT_CONFIG_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_HOTPLUG_EVENT_CONFIG_PARAMS_MESSAGE_ID (0x44U)

typedef NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS NV0073_CTRL_SYSTEM_GET_HOTPLUG_EVENT_CONFIG_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_SET_HOTPLUG_EVENT_CONFIG
 *
 * This command sets the hotplug event configuration.
 *
 * See @ref NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS for documentation on
 * the parameters.
 */

#define NV0073_CTRL_CMD_SYSTEM_SET_HOTPLUG_EVENT_CONFIG (0x730145U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_SET_HOTPLUG_EVENT_CONFIG_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_SET_HOTPLUG_EVENT_CONFIG_PARAMS_MESSAGE_ID (0x45U)

typedef NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS NV0073_CTRL_SYSTEM_SET_HOTPLUG_EVENT_CONFIG_PARAMS;



/*
*  NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS
*
*  This command is used to read Core channel, Cursor channel, Window channel, and Head register values and encode these values with ProtoDmp.
*
*  subDeviceInstance (in)
*    This parameter specifies the subdevice instance within the
*    NV04_DISPLAY_COMMON parent device to which the operation should be
*    directed.
*  headMask (in)
*    Head mask representing which register values should be encoded
*  windowMask (in)
*    Window channel mask whose register values should be encoded
*  bRecordCoreChannel (in)
*    Indicates whether or not to encode core channel register values
*  bRecordCursorChannel (in)
*    Indicates whether or not to encode cursor channel register values
*
*  Possible status values returned are:
*    NV_OK
*    NV_ERR_INVALID_ARGUMENT
*    NV_ERR_NOT_SUPPORTED
*/
#define NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS (0x73014aU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS_PARAMS_MESSAGE_ID (0x4AU)

typedef struct NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  headMask;
    NvU32  windowMask;
    NvBool bRecordCoreChannel;
    NvBool bRecordCursorChannel;
} NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT
 *
 * This command is used to query the display mux status for the given
 * display device
 *
 *   subDeviceInstance (in)
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT (0x73014bU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS_MESSAGE_ID (0x4BU)

typedef struct NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS {
    NvU32  subDeviceInstance;
    NvBool bIsSidebandI2cSupported;
} NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_SR_SUPPORT
 *
 * This command is used to query if SIDEBAND SR can be used with the
 * given display device. If PSR API is supported on the system,
 * then sideband SR support is set to false.
 *
 *    subDeviceInstance (in)
 *        This parameter specifies the subdevice instance within the
 *        NV04_DISPLAY_COMMON parent device to which the operation
 *        should be directed.
 *    displayId (in)
 *        This parameter inputs the displayId of the active display. A value
 *        of zero indicates no display is active.
 *    bIsSidebandSrSupported
 *        If it is true, it means that sideband is supported and not PSR API.
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_PARAM_STRUCT
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_NOT_SUPPORTED
 */
#define NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_SR_SUPPORT (0x73014cU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_SR_SUPPORT_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_SR_SUPPORT_PARAMS_MESSAGE_ID (0x4CU)

typedef struct NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_SR_SUPPORT_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvBool bIsSidebandSrSupported;
} NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_SR_SUPPORT_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE
 *
 * This command is used by client like nvkms to set up the VRR specific
 * memory operation in RM such as mapping the client created shared memory
 * into RM and reserving a RGline for processing of self-refresh timeout
 * related calculations.
 *
 * Also the expectation is that the client which calls this command with parameter
 * bEnable = TRUE, should also call this command with bEnable = FALSE on the
 * same head when VRR needs to be disabled.
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_OBJECT_NOT_FOUND
 *    NV_ERR_GENERIC
 */

/*
 * This is the shared structure that will be used to communicate between
 * Physical RM and clients. As of now the access relies on single source of
 * truth operation, i.e. only Physical RM writes into the shared location
 * and client (nvkms) reads from the same location.
 *
 * "dataTimeStamp" field is added to capture the timestamp before and after
 * updating the flip delay related data fields(all fields except "timeout").
 * This timestamp will be used by clients to determine if the data got updated
 * in between by RM while clients were reading it.
 * As of now "timeout" field does not have such protection, as access to
 * this field is only in response to notification from RM.
 */
typedef struct NV0073_CTRL_RM_VRR_SHARED_DATA {
    NvU32  expectedFrameNum;
    NvU32  timeout;
    NV_DECLARE_ALIGNED(NvU64 flipTimeStamp, 8);
    NvBool bCheckFlipTime;
    NvBool bFlipTimeAdjustment;
    NV_DECLARE_ALIGNED(NvU64 dataTimeStamp, 8);
} NV0073_CTRL_RM_VRR_SHARED_DATA;

#define NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE (0x73014dU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS_MESSAGE_ID (0x4DU)

typedef struct NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS {
    NvU32    subDeviceInstance;
    NvBool   bEnable;
    NvU32    head;
    NvU32    height;
    NvU32    maxFrameTime;
    NvU32    minFrameTime;
    NvHandle hMemory;
} NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS;

/*
 * NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL
 *
 * The VBlank Semaphore Control API ("VBlank Sem Control") allows clients to
 * register for a semaphore release to be performed on the specified memory.
 *
 * One or more clients may register a memory allocation + offset by describing a
 * video memory object with _PARAMS::hMemory and an offset within that memory
 * object (_PARAMS::memoryOffset).  Until the hMemory + memoryOffset combination
 * is disabled, during each vblank on the specified heads, RM will interpret the
 * specified memory location as an
 * NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DATA data structure.  Each enabled
 * head will inspect the corresponding
 * NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DATA_ONE_HEAD at
 * NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DATA::head[head].
 *
 * _PARAMS::memoryOffset must be a multiple of 8, so that GPU semaphore releases
 * and GSP can write to 8-byte fields within
 * NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DATA_ONE_HEAD with natural alignment.
 *
 * During vblank, the _CONTROL_DATA_ONE_HEAD::requestCounter field will be read,
 * and the following pseudocode will be performed:
 *
 *   swapInterval      = DRF_VAL(data->flags)
 *   useMinimumGpuTime = DRV_VAL(data->flags)
 *
 *   if (data->requestCounter == prevRequestCounter)
 *       return
 *
 *   if (currentVblankCount < (prevVBlankCount + swapInterval))
 *       return
 *
 *   if (useMinimumGpuTime && (data->minimumGpuTime < currentGpuTime))
 *       return
 *
 *   data->vblankCount    = currentVblankCount
 *   data->releaseGpuTime = currentGpuTime
 *   data->semaphore      = data->requestCounter
 *
 *   prevRequestCounter   = data->requestCounter
 *   previousVblankCount  = currentVblankCount
 *
 * I.e., if the client-described conditions are met, the RM will write
 * _CONTROL_DATA_ONE_HEAD::semaphore to the client-requested 'requestCounter'
 * along with several informational fields (vblankCount, releaseGpuTime).
 *
 * The intent is for clients to use semaphore releases to write:
 *
 *   _CONTROL_DATA_ONE_HEAD::minimumGpuTime (if desired)
 *   _CONTROL_DATA_ONE_HEAD::swapInterval
 *   _CONTROL_DATA_ONE_HEAD::requestCounter
 *
 * and then perform a semaphore acquire on _CONTROL_DATA_ONE_HEAD::semaphore >=
 * requestCounter (using the ACQ_GEQ semaphore operation).  This will block any
 * following methods in the client's channel (e.g., a blit) until the requested
 * conditions are met.  Note the ::requestCounter should be written last,
 * because the change in value of ::requestCounter is what causes RM, during a
 * vblank callback, to inspect the other fields.
 *
 * Additionally, clients should use the CPU (not semaphore releases in their
 * channel) to write the field _CONTROL_DATA_ONE_HEAD::requestCounterAccel at
 * the same time that they enqueue the semaphore release to write to
 * _CONTROL_DATA_ONE_HEAD::requestCounter.  ::requestCounterAccel will be used
 * by resman to "accelerate" the vblank sem control by copying the value from
 * ::requestCounterAccel to ::semaphore.  This will be done when the vblank sem
 * control is disabled, and when a client calls
 * NV0073_CTRL_CMD_SYSTEM_ACCEL_VBLANK_SEM_CONTROLS.  It is important for resman
 * to have access to the value in ::requestCounterAccel, and not just
 * ::requestCounter.  The latter is only the last value released so far by the
 * client's channel (further releases to ::requestCounter may still be inflight,
 * perhaps blocked on pending semaphore acquires).  The former should be the
 * most recent value enqueued in the channel.  This is also why it is important
 * for clients to acquire with ACQ_GEQ (greater-than-or-equal-to), rather than
 * just ACQUIRE.
 *
 * The same hMemory (with difference memoryOffsets) may be used by multiple
 * VBlank Sem Controls.
 *
 * Lastly, the bUseHeadIndexMap field in the enable params is used to tell
 * resman to honor the headIndexMap[] table in the enable params.  Normally, the
 * N-th bit in the enable params headMask corresponds to element N in the
 * _CONTROL_DATA::head[] array.  But, headIndexMap[] allows the N-th bit in
 * headMask to be remapped to a different index in the head[] array.  E.g.,
 *
 *  NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DATA *pData = ...;
 *
 *  FOR_EACH_INDEX_IN_MASK(32, head, pParams->headMask)
 *  {
 *      NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DATA_ONE_HEAD *pDataOneHead;
 *      NvU32 headIndex = pParams->bUseHeadIndexMap ?
 *          pParams->headIndexMap[head] : head;
 *      pDataOneHead = &pData->head[headIndex];
 *      ...
 *  }
 *  FOR_EACH_INDEX_IN_MASK_END;
 * 
 * This remapping is important for nvkms' use of the RMAPI.  To support
 * 2head1or, nvkms may remap head indices between the headMask passed to RM and
 * the array indices in _CONTROL_DATA::head[] as used by OpenGL.
 */

/* Fields within NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DATA_ONE_HEAD::flags */
#define NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_SWAP_INTERVAL          15:0
#define NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_USE_MINIMUM_GPU_TIME   16:16

typedef struct NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DATA_ONE_HEAD {
    NvU32 requestCounterAccel;
    NvU32 requestCounter;
    NvU32 flags;
    NV_DECLARE_ALIGNED(NvU64 minimumGpuTime, 8);

    NvU32 semaphore;
    NV_DECLARE_ALIGNED(NvU64 vblankCount, 8);
    NV_DECLARE_ALIGNED(NvU64 releaseGpuTime, 8);
} NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DATA_ONE_HEAD;

typedef struct NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DATA {
    NV_DECLARE_ALIGNED(NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DATA_ONE_HEAD head[NV_MAX_HEADS], 8);
} NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DATA;

#define NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_ENABLE (0x73014eU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_ENABLE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_ENABLE_PARAMS_MESSAGE_ID (0x4EU)

typedef struct NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_ENABLE_PARAMS {
    NvU32    subDeviceInstance;
    NvU32    headMask;
    NvU8     headIndexMap[NV_MAX_HEADS];
    NvHandle hMemory;
    NV_DECLARE_ALIGNED(NvU64 memoryOffset, 8);
    NvBool   bUseHeadIndexMap;
} NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_ENABLE_PARAMS;

#define NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DISABLE (0x73014fU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DISABLE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DISABLE_PARAMS_MESSAGE_ID (0x4FU)

typedef struct NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DISABLE_PARAMS {
    NvU32    subDeviceInstance;
    NvHandle hMemory;
    NV_DECLARE_ALIGNED(NvU64 memoryOffset, 8);
} NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DISABLE_PARAMS;

/*
 * Accelerate all VBlank Sem Controls on the specified heads.
 *
 * For all enabled vblank sem controls on the specified heads, immediate set
 * their pending ::semaphore fields to the value in their ::requestCounterAccel
 * field.
 */
#define NV0073_CTRL_CMD_SYSTEM_ACCEL_VBLANK_SEM_CONTROLS (0x730150U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_CMD_SYSTEM_ACCEL_VBLANK_SEM_CONTROLS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_SYSTEM_ACCEL_VBLANK_SEM_CONTROLS_PARAMS_MESSAGE_ID (0x50U)

typedef struct NV0073_CTRL_CMD_SYSTEM_ACCEL_VBLANK_SEM_CONTROLS_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 headMask;
} NV0073_CTRL_CMD_SYSTEM_ACCEL_VBLANK_SEM_CONTROLS_PARAMS;

/*
 * Maps the memory allocated in Kernel RM into Physical RM using the
 * memory descriptor information provided.
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_NOT_SUPPORTED
 */

typedef struct NV0073_CTRL_SHARED_MEMDESC_INFO {
    NV_DECLARE_ALIGNED(NvU64 base, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NV_DECLARE_ALIGNED(NvU64 alignment, 8);
    NvU32 addressSpace;
    NvU32 cpuCacheAttrib;
} NV0073_CTRL_SHARED_MEMDESC_INFO;

#define NV0073_CTRL_CMD_SYSTEM_MAP_SHARED_DATA (0x730151U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_CMD_SYSTEM_MAP_SHARED_DATA_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_SYSTEM_MAP_SHARED_DATA_PARAMS_MESSAGE_ID (0x51U)

typedef struct NV0073_CTRL_CMD_SYSTEM_MAP_SHARED_DATA_PARAMS {
    NV_DECLARE_ALIGNED(NV0073_CTRL_SHARED_MEMDESC_INFO memDescInfo, 8);
    NvU32  subDeviceInstance;
    NvBool bMap;
} NV0073_CTRL_CMD_SYSTEM_MAP_SHARED_DATA_PARAMS;

/* _ctrl0073system_h_ */

