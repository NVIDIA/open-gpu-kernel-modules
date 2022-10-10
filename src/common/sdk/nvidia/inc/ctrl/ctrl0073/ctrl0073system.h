/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl0073/ctrl0073system.finn
//

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
#define NV0073_CTRL_CMD_SYSTEM_GET_CAPS_V2 (0x730138U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS_MESSAGE_ID (0x38U)

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

#define NV0073_CTRL_SYSTEM_GET_NUM_HEADS_PARAMS_MESSAGE_ID (0x2U)

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
#define NV0073_CTRL_CMD_SYSTEM_GET_SCANLINE           (0x730108U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_SYSTEM_GET_SCANLINE_RIGHT_EYE 0x00000000U
#define NV0073_CTRL_CMD_SYSTEM_GET_SCANLINE_LEFT_EYE  0x00000001U

#define NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS_MESSAGE_ID (0x8U)

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
 *   verticalBlankCounter
 *     This parameter returns the vblank counter value for the specified
 *     head. If the display mode is not valid or vblank not active then
 *     the verticalBlankCounter value is undefined.
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SYSTEM_GET_VBLANK_COUNTER (0x730109U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 head;
    NvU32 verticalBlankCounter;
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
#define NV0073_CTRL_CMD_SYSTEM_GET_VBLANK_ENABLE (0x73010aU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_VBLANK_ENABLE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_VBLANK_ENABLE_PARAMS_MESSAGE_ID (0xAU)

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
#define NV0073_CTRL_CMD_SYSTEM_GET_SUPPORTED (0x730120U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS_MESSAGE_ID (0x20U)

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
#define NV0073_CTRL_CMD_SYSTEM_GET_CONNECT_STATE (0x730122U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_PARAMS_MESSAGE_ID (0x22U)

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


#define NV0073_CTRL_CMD_SYSTEM_GET_HOTPLUG_CONFIG                  (0x730123U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS_MESSAGE_ID (0x23U)

typedef struct NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 flags;
    NvU32 hotplugEventMask;
    NvU32 hotplugPollable;
    NvU32 hotplugInterruptible;
    NvU32 hotplugAlwaysAttached;
} NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS;



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
#define NV0073_CTRL_CMD_SYSTEM_GET_HEAD_ROUTING_MAP           (0x730125U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_HEAD_ROUTING_MAP_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_HEAD_ROUTING_MAP_PARAMS_MESSAGE_ID (0x25U)

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
#define NV0073_CTRL_CMD_SYSTEM_GET_ACTIVE                (0x730126U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS_MESSAGE_ID (0x26U)

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
#define NV0073_CTRL_CMD_SYSTEM_GET_INTERNAL_DISPLAYS (0x73015bU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS_MESSAGE_ID (0x5BU)

typedef struct NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 internalDisplaysMask;
    NvU32 availableInternalDisplaysMask;
} NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS;



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
#define NV0073_CTRL_CMD_SYSTEM_GET_BOOT_DISPLAYS                         (0x730166U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS_MESSAGE_ID (0x66U)

typedef struct NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 bootDisplayMask;
} NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS;



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

#define NV0073_CTRL_CMD_SYSTEM_GET_HOTPLUG_UNPLUG_STATE (0x73017bU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | 0x7B" */

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

#define NV0073_CTRL_CMD_SYSTEM_CLEAR_ELV_BLOCK                       (0x73017dU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_CLEAR_ELV_BLOCK_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_CLEAR_ELV_BLOCK_PARAMS_MESSAGE_ID (0x7DU)

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

#define NV0073_CTRL_CMD_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR (0x73017eU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR_PARAMS_MESSAGE_ID (0x7EU)

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
#define NV0073_CTRL_CMD_SYSTEM_CONFIG_VRR_PSTATE_SWITCH (0x730184U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS_MESSAGE_ID (0x84U)

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

#define NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX (0x730190U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX_PARAMS_MESSAGE_ID (0x90U)

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

#define NV0073_CTRL_CMD_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH (0x730196U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS_MESSAGE_ID (0x96U)

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

#define NV0073_CTRL_CMD_SYSTEM_GET_HOTPLUG_EVENT_CONFIG  (0x730197U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | 0x97" */

/*
 * NV0073_CTRL_CMD_SYSTEM_SET_HOTPLUG_EVENT_CONFIG
 *
 * This command sets the hotplug event configuration.
 *
 * See @ref NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS for documentation on
 * the parameters.
 */

#define NV0073_CTRL_CMD_SYSTEM_SET_HOTPLUG_EVENT_CONFIG  (0x730198U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | 0x98" */



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
#define NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS (0x73019bU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS_PARAMS_MESSAGE_ID (0x9BU)

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
#define NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT (0x73019cU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SYSTEM_INTERFACE_ID << 8) | NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS_MESSAGE_ID (0x9CU)

typedef struct NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS {
    NvU32  subDeviceInstance;
    NvBool bIsSidebandI2cSupported;
} NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS;

/* _ctrl0073system_h_ */

