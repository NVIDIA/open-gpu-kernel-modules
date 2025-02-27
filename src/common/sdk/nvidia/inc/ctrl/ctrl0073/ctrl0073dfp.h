/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0073/ctrl0073dfp.finn
//

#include "ctrl/ctrl0073/ctrl0073base.h"
#include "ctrl/ctrl0073/ctrl0073common.h"

#include "nvcfg_sdk.h"

/* NV04_DISPLAY_COMMON dfp-display-specific control commands and parameters */

/*
 * NV0073_CTRL_CMD_DFP_GET_INFO
 *
 * This command can be used to determine the associated display type for
 * the specified displayId.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the display for which the dfp
 *     caps should be returned.  The display ID must be a dfp display
 *     as determined with the NV0073_CTRL_CMD_SPECIFIC_GET_TYPE command.
 *     If more than one displayId bit is set or the displayId is not a dfp,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   flags
 *     This parameter returns the information specific to this dfp.  Here are
 *     the possible fields:
 *       NV0073_CTRL_DFP_FLAGS_SIGNAL
 *         This specifies the type of signal used for this dfp.
 *       NV0073_CTRL_DFP_FLAGS_LANES
 *         This specifies whether the board supports 1, 2, or 4 lanes
 *         for DISPLAYPORT signals.
 *       NV0073_CTRL_DFP_FLAGS_LIMIT
 *         Some GPUs were not qualified to run internal TMDS except at 60 HZ
 *         refresh rates.  So, if LIMIT_60HZ_RR is set, then the client must
 *         make sure to only allow 60 HZ refresh rate modes to the OS/User.
 *       NV0073_CTRL_DFP_FLAGS_SLI_SCALER
 *         While running in SLI, if SLI_SCALER_DISABLE is set, the GPU cannot
 *         scale any resolutions.  So, the output timing must match the
 *         memory footprint.
 *       NV0073_CTRL_DFP_FLAGS_HDMI_CAPABLE
 *         This specifies whether the DFP displayId is capable of
 *         transmitting HDMI.
 *       NV0073_CTRL_DFP_FLAGS_RANGE_LIMITED_CAPABLE
 *         This specifies whether the displayId is capable of sending a
 *         limited color range out from the board.
 *       NV0073_CTRL_DFP_FLAGS_RANGE_AUTO_CAPABLE
 *         This specifies whether the displayId is capable of auto-configuring
 *         the color range.
 *       NV0073_CTRL_DFP_FLAGS_FORMAT_YCBCR422_CAPABLE
 *         This specifies whether the displayId is capable of sending the
 *         YCBCR422 color format out from the board.
 *       NV0073_CTRL_DFP_FLAGS_FORMAT_YCBCR444_CAPABLE
 *         This specifies whether the displayId is capable of sending
 *         YCBCR444 color format out from the board.
 *       NV0073_CTRL_DFP_FLAGS_TYPE_C_TO_DP_CONNECTOR
 *         This specifies whether the displayId is a DP connector routed to an USB-TYPE-C port.
 *       NV0073_CTRL_DFP_FLAGS_DP_LINK_BANDWIDTH
 *         This specifies max link rate supported by the displayId, if the DFP is
 *         display port.
 *       NV0073_CTRL_DFP_FLAGS_HDMI_ALLOWED
 *         This specifies whether the DFP displayId is allowed to transmit HDMI
 *         based on the VBIOS settings.
 *       NV0073_CTRL_DFP_FLAGS_EMBEDDED_DISPLAYPORT
 *         This specifies whether the DFP displayId is actually an embedded display
 *         port based on VBIOS connector information AND ASSR cap.
 *       NV0073_CTRL_DFP_FLAGS_DP_LINK_CONSTRAINT
 *         This specifies whether the DFP displayId must be trained to RBR mode
 *         (if it is using DP protocol) whenever possible.
 *       NV0073_CTRL_DFP_FLAGS_LINK
 *         This specifies whether the board supports single or dual links
 *         for TMDS, LVDS, and SDI signals.
 *       NV0073_CTRL_DFP_FLAGS_DP_POST_CURSOR2_DISABLED
 *         This specifies if PostCursor2 is disabled in the VBIOS
 *       NV0073_CTRL_DFP_FLAGS_DSI_DEVICE_ID
 *         This indicates whether this SOR uses DSI-A, DSI-B or both (ganged mode).
 *       NV0073_CTRL_DFP_FLAGS_DYNAMIC_MUX_CAPABLE
 *         This indicates whether this DFP supports DDS (NV dynamic display mux).
 *   UHBRSupportedByDfp
 *     Bitmask to specify the UHBR link rates supported by this dfp.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_DFP_GET_INFO (0x731140U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_DFP_GET_INFO_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DFP_GET_INFO_PARAMS_MESSAGE_ID (0x40U)

typedef struct NV0073_CTRL_DFP_GET_INFO_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 flags;
    NvU32 UHBRSupportedByDfp;
} NV0073_CTRL_DFP_GET_INFO_PARAMS;

/* valid display types */
#define NV0073_CTRL_DFP_FLAGS_SIGNAL                                       2:0
#define NV0073_CTRL_DFP_FLAGS_SIGNAL_TMDS                       (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_SIGNAL_LVDS                       (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_SIGNAL_SDI                        (0x00000002U)
#define NV0073_CTRL_DFP_FLAGS_SIGNAL_DISPLAYPORT                (0x00000003U)
#define NV0073_CTRL_DFP_FLAGS_SIGNAL_DSI                        (0x00000004U)
#define NV0073_CTRL_DFP_FLAGS_SIGNAL_WRBK                       (0x00000005U)
#define NV0073_CTRL_DFP_FLAGS_LANE                                         5:3
#define NV0073_CTRL_DFP_FLAGS_LANE_NONE                         (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_LANE_SINGLE                       (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_LANE_DUAL                         (0x00000002U)
#define NV0073_CTRL_DFP_FLAGS_LANE_QUAD                         (0x00000003U)
#define NV0073_CTRL_DFP_FLAGS_LANE_OCT                          (0x00000004U)
#define NV0073_CTRL_DFP_FLAGS_LIMIT                                        6:6
#define NV0073_CTRL_DFP_FLAGS_LIMIT_DISABLE                     (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_LIMIT_60HZ_RR                     (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_SLI_SCALER                                   7:7
#define NV0073_CTRL_DFP_FLAGS_SLI_SCALER_NORMAL                 (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_SLI_SCALER_DISABLE                (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_HDMI_CAPABLE                                 8:8
#define NV0073_CTRL_DFP_FLAGS_HDMI_CAPABLE_FALSE                (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_HDMI_CAPABLE_TRUE                 (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_RANGE_LIMITED_CAPABLE                        9:9
#define NV0073_CTRL_DFP_FLAGS_RANGE_LIMITED_CAPABLE_FALSE       (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_RANGE_LIMITED_CAPABLE_TRUE        (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_RANGE_AUTO_CAPABLE                         10:10
#define NV0073_CTRL_DFP_FLAGS_RANGE_AUTO_CAPABLE_FALSE          (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_RANGE_AUTO_CAPABLE_TRUE           (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_FORMAT_YCBCR422_CAPABLE                    11:11
#define NV0073_CTRL_DFP_FLAGS_FORMAT_YCBCR422_CAPABLE_FALSE     (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_FORMAT_YCBCR422_CAPABLE_TRUE      (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_FORMAT_YCBCR444_CAPABLE                    12:12
#define NV0073_CTRL_DFP_FLAGS_FORMAT_YCBCR444_CAPABLE_FALSE     (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_FORMAT_YCBCR444_CAPABLE_TRUE      (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_TYPE_C_TO_DP_CONNECTOR                     13:13
#define NV0073_CTRL_DFP_FLAGS_TYPE_C_TO_DP_CONNECTOR_FALSE      (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_TYPE_C_TO_DP_CONNECTOR_TRUE       (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_HDMI_ALLOWED                               14:14
#define NV0073_CTRL_DFP_FLAGS_HDMI_ALLOWED_FALSE                (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_HDMI_ALLOWED_TRUE                 (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_EMBEDDED_DISPLAYPORT                       15:15
#define NV0073_CTRL_DFP_FLAGS_EMBEDDED_DISPLAYPORT_FALSE        (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_EMBEDDED_DISPLAYPORT_TRUE         (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_DP_LINK_CONSTRAINT                         16:16
#define NV0073_CTRL_DFP_FLAGS_DP_LINK_CONSTRAINT_NONE           (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_DP_LINK_CONSTRAINT_PREFER_RBR     (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_DP_LINK_BW                                 19:17
#define NV0073_CTRL_DFP_FLAGS_DP_LINK_BW_1_62GBPS               (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_DP_LINK_BW_2_70GBPS               (0x00000002U)
#define NV0073_CTRL_DFP_FLAGS_DP_LINK_BW_5_40GBPS               (0x00000003U)
#define NV0073_CTRL_DFP_FLAGS_DP_LINK_BW_8_10GBPS               (0x00000004U)
#define NV0073_CTRL_DFP_FLAGS_LINK                                       21:20
#define NV0073_CTRL_DFP_FLAGS_LINK_NONE                         (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_LINK_SINGLE                       (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_LINK_DUAL                         (0x00000002U)
#define NV0073_CTRL_DFP_FLAGS_DP_FORCE_RM_EDID                           22:22
#define NV0073_CTRL_DFP_FLAGS_DP_FORCE_RM_EDID_FALSE            (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_DP_FORCE_RM_EDID_TRUE             (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_DSI_DEVICE_ID                              24:23
#define NV0073_CTRL_DFP_FLAGS_DSI_DEVICE_ID_DSI_NONE            (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_DSI_DEVICE_ID_DSI_A               (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_DSI_DEVICE_ID_DSI_B               (0x00000002U)
#define NV0073_CTRL_DFP_FLAGS_DSI_DEVICE_ID_DSI_GANGED          (0x00000003U)
#define NV0073_CTRL_DFP_FLAGS_DP_POST_CURSOR2_DISABLED                   25:25
#define NV0073_CTRL_DFP_FLAGS_DP_POST_CURSOR2_DISABLED_FALSE    (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_DP_POST_CURSOR2_DISABLED_TRUE     (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS_DP_PHY_REPEATER_COUNT                      29:26
#define NV0073_CTRL_DFP_FLAGS_DYNAMIC_MUX_CAPABLE                        30:30
#define NV0073_CTRL_DFP_FLAGS_DYNAMIC_MUX_CAPABLE_FALSE         (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS_DYNAMIC_MUX_CAPABLE_TRUE          (0x00000001U)

#define NV0073_CTRL_DFP_FLAGS2_DP_UHBR_SUPPORTED_10_0GBPS                  0:0
#define NV0073_CTRL_DFP_FLAGS2_DP_UHBR_SUPPORTED_10_0GBPS_FALSE (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS2_DP_UHBR_SUPPORTED_10_0GBPS_TRUE  (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS2_DP_UHBR_SUPPORTED_13_5GBPS                  1:1
#define NV0073_CTRL_DFP_FLAGS2_DP_UHBR_SUPPORTED_13_5GBPS_FALSE (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS2_DP_UHBR_SUPPORTED_13_5GBPS_TRUE  (0x00000001U)
#define NV0073_CTRL_DFP_FLAGS2_DP_UHBR_SUPPORTED_20_0GBPS                  2:2
#define NV0073_CTRL_DFP_FLAGS2_DP_UHBR_SUPPORTED_20_0GBPS_FALSE (0x00000000U)
#define NV0073_CTRL_DFP_FLAGS2_DP_UHBR_SUPPORTED_20_0GBPS_TRUE  (0x00000001U)



/*
 * NV0073_CTRL_CMD_DFP_GET_DP2TMDS_DONGLE_INFO
 *
 * This command can be used to determine information about dongles attached
 * to a displayport connection.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the dfp display which owns the
 *     panel power to adjust.  The display ID must be a dfp display
 *     as determined with the NV0073_CTRL_CMD_SPECIFIC_GET_TYPE command.
 *     If more than one displayId bit is set or the displayId is not a dfp,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   flags
 *     This parameter provide state information about the dongle attachments.
 *       NV0073_CTRL_DFP_GET_DP2TMDS_DONGLE_INFO_FLAGS_CAPABLE
 *         Specifies if the connection is capable of a dongle.  This field
 *         returns false in all cases of signal types except for those capable
 *         of outputting TMDS.  Even then the if a gpio is not defined, the
 *         the a statement of false will also be returned.
 *       NV0073_CTRL_DFP_GET_DP2TMDS_DONGLE_INFO_FLAGS_ATTACHED
 *         When attached, this value specifies that a dongle is detected and
 *         attached.  The client should read the _TYPE field to determine
 *         if it is a dp2hdmi or dp2dvi dongle.
 *      NV0073_CTRL_DFP_GET_DP2TMDS_DONGLE_INFO_FLAGS_TYPE
 *         _DP2DVI: no response to i2cAddr 0x80 per DP interop guidelines.
 *                  clients MUST avoid outputting HDMI even if capable.
 *         _DP2HDMI: dongle responds to i2cAddr 0x80 per DP interop guidelines.
 *                   client is allowed to output HDMI when possible.
 *         _LFH_DVI: DMS59-DVI breakout dongle is in use.
 *         _LFH_VGA: DMS59-VGA breakout dongle is in use.
 *      NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_FLAGS_DP2TMDS_DONGLE_TYPE
 *         _1: Max TMDS Clock rate is 165 MHz for both DVI and HDMI.
 *         _2: Max TMDS Clock rate will be specified in the dongle
 *              address space at device address 0x80.
 *              DVI  is up to 165 MHz
 *              HDMI is up to 300 MHz
 *              There are type 2 devices that support beyond 600 MHz
 *              though not defined in the spec.
 *   maxTmdsClkRateHz
 *     This defines the max TMDS clock rate for dual mode adaptor in Hz.
 */
#define NV0073_CTRL_CMD_DFP_GET_DISPLAYPORT_DONGLE_INFO (0x731142U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_PARAMS_MESSAGE_ID (0x42U)

typedef struct NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 flags;
    NvU32 maxTmdsClkRateHz;
} NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_PARAMS;

#define  NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_FLAGS_CAPABLE                  0:0
#define NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_FLAGS_CAPABLE_FALSE         (0x00000000U)
#define NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_FLAGS_CAPABLE_TRUE          (0x00000001U)
#define  NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_FLAGS_ATTACHED                 1:1
#define NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_FLAGS_ATTACHED_FALSE        (0x00000000U)
#define NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_FLAGS_ATTACHED_TRUE         (0x00000001U)
#define  NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_FLAGS_TYPE                     7:4
#define NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_FLAGS_TYPE_DP2DVI           (0x00000000U)
#define NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_FLAGS_TYPE_DP2HDMI          (0x00000001U)
#define NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_FLAGS_TYPE_LFH_DVI          (0x00000002U)
#define NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_FLAGS_TYPE_LFH_VGA          (0x00000003U)
#define  NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_FLAGS_DP2TMDS_DONGLE_TYPE      8:8
#define NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_FLAGS_DP2TMDS_DONGLE_TYPE_1 (0x00000000U)
#define NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_FLAGS_DP2TMDS_DONGLE_TYPE_2 (0x00000001U)



/*
 * NV0073_CTRL_CMD_DFP_SET_ELD_AUDIO_CAPS
 *
 * This command is used to inform hardware the receiver's audio capabilities
 * using the new EDID Like Data (ELD) memory structure.  The ELD memory
 * structure is read by the audio driver by issuing the ELD Data command verb.
 * This mechanism is used for passing sink device' audio EDID information
 * from graphics software to audio software.  ELD contents will contain a
 * subset of the sink device's EDID information.
 * The client should inform hardware at initial boot, a modeset, and whenever
 * a hotplug event occurs.
 *
 *   displayId
 *     This parameter indicates the digital display device's
 *     mask. This comes as input to this command.
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be directed.
 *     This parameter must specify a value between zero and the total number
 *     of subdevices within the parent device.  This parameter should be set
 *     to zero for default behavior.
 *   numELDSize
 *     This parameter specifies how many bytes of data RM should write to the
 *     ELD buffer.  Section 7.3.3.36 of the ECN specifies that the ELD buffer
 *     size of zero based.  HDAudio driver will then use this information to
 *     determine how many bytes of the ELD buffer the HDAudio should read.
 *     The maximum size of the buffer is 96 bytes.
 *   bufferELD
 *     This buffer contains data as defined in the ECR HDMI ELD memory structure.
 *     Refer to the ELD Memory Structure Specification for more details.
 *     The format should be:
 *       - Header block is fixed at 4 bytes
 *         The header block contains the ELD version and baseline ELD len as
 *         well as some reserved fields.
 *       - Baseline block for audio descriptors is 76 bytes
 *         (15 SAD; each SAD=3 bytes requiring 45 bytes with 31 bytes to spare)
 *         As well as some other bits used to denote the CEA version,
 *         the speaker allocation data, monitor name, connector type, and
 *         hdcp capabilities.
 *       - Vendor specific block of 16 bytes
 *   maxFreqSupported
 *     Supply the maximum frequency supported for the overall audio caps.
 *     This value should match CEA861-X defines for sample freq.
 *   ctrl:
 *     NV0073_CTRL_DFP_SET_ELD_AUDIO_CAPS_CTRL_PD:
 *         Specifies the presence detect of the receiver.  On a hotplug
 *         or modeset client should set this bit to TRUE.
 *     NV0073_CTRL_DFP_SET_ELD_AUDIO_CAPS_CTRL_ELDV:
 *         Specifies whether the ELD buffer contents are valid.
 *         An intrinsic unsolicited response (UR) is generated whenever
 *         the ELDV bit changes in value and the PD=1. When _PD=1(hotplug),
 *         RM will set the ELDV bit after ELD buffer contents are written.
 *         If _ELDV bit is set to false such as during a unplug, then the
 *         contents of the ELD buffer will be cleared.
 *   deviceEntry:
 *     The deviceEntry number from which the SF should accept packets.
 *     _NONE if disabling audio.
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_DFP_SET_ELD_AUDIO_CAPS                         (0x731144U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_DFP_SET_ELD_AUDIO_CAP_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_ELD_BUFFER                      96U

#define NV0073_CTRL_DFP_SET_ELD_AUDIO_CAP_PARAMS_MESSAGE_ID (0x44U)

typedef struct NV0073_CTRL_DFP_SET_ELD_AUDIO_CAP_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 numELDSize;
    NvU8  bufferELD[NV0073_CTRL_DFP_ELD_AUDIO_CAPS_ELD_BUFFER];
    NvU32 maxFreqSupported;
    NvU32 ctrl;
    NvU32 deviceEntry;
} NV0073_CTRL_DFP_SET_ELD_AUDIO_CAP_PARAMS;

#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_MAX_FREQ_SUPPORTED_0320KHZ (0x00000001U)
#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_MAX_FREQ_SUPPORTED_0441KHZ (0x00000002U)
#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_MAX_FREQ_SUPPORTED_0480KHZ (0x00000003U)
#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_MAX_FREQ_SUPPORTED_0882KHZ (0x00000004U)
#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_MAX_FREQ_SUPPORTED_0960KHZ (0x00000005U)
#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_MAX_FREQ_SUPPORTED_1764KHZ (0x00000006U)
#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_MAX_FREQ_SUPPORTED_1920KHZ (0x00000007U)

#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_CTRL_PD                                     0:0
#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_CTRL_PD_FALSE              (0x00000000U)
#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_CTRL_PD_TRUE               (0x00000001U)
#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_CTRL_ELDV                                   1:1
#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_CTRL_ELDV_FALSE            (0x00000000U)
#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_CTRL_ELDV_TRUE             (0x00000001U)

#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_DEVICE_ENTRY_0             (0x00000000U)
#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_DEVICE_ENTRY_1             (0x00000001U)
#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_DEVICE_ENTRY_2             (0x00000002U)
#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_DEVICE_ENTRY_3             (0x00000003U)
#define NV0073_CTRL_DFP_ELD_AUDIO_CAPS_DEVICE_ENTRY_NONE          (0x00000007U)



/*
 * NV0073_CTRL_CMD_DFP_GET_SPREAD_SPECTRUM_STATUS
 *
 * This command is used to get spread spectrum status for a display device.
 *
 * displayId
 *    Display ID for which the spread spectrum status is needed.
 * checkRMSsState
 *    Default is to check in Vbios. This flag lets this control call to check in register.
 * status
 *    Return status value.
 */

#define NV0073_CTRL_CMD_DFP_GET_SPREAD_SPECTRUM (0x73114cU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_DFP_GET_SPREAD_SPECTRUM_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DFP_GET_SPREAD_SPECTRUM_PARAMS_MESSAGE_ID (0x4CU)

typedef struct NV0073_CTRL_DFP_GET_SPREAD_SPECTRUM_PARAMS {
    NvU32  displayId;
    NvBool enabled;
} NV0073_CTRL_DFP_GET_SPREAD_SPECTRUM_PARAMS;

/*
 * NV0073_CTRL_CMD_DFP_UPDATE_DYNAMIC_DFP_CACHE
 *
 * Update the Dynamic DFP with Bcaps read from remote display.
 * Also updates hdcpFlags, gpu hdcp capable flags in DFP.
 * If bResetDfp is true, all the flags are reset before making changes.
 *
 *   Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_ARGUMENT
 *     NV_ERR_NOT_SUPPORTED
 */

#define NV0073_CTRL_CMD_DFP_UPDATE_DYNAMIC_DFP_CACHE (0x73114eU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_PARAMS_MESSAGE_ID (0x4EU)

typedef struct NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  headIndex;
    NvU8   bcaps;
    NvU8   bksv[5];
    NvU32  hdcpFlags;
    NvBool bHdcpCapable;
    NvBool bResetDfp;
    NvU8   updateMask;
} NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_PARAMS;

#define NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_MASK_BCAPS 0x01U
#define NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_MASK_BKSV  0x02U
#define NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_MASK_FLAGS 0x03U

/*
 * NV0073_CTRL_CMD_DFP_SET_AUDIO_ENABLE
 *
 * This command sets the audio enable state of the DFP.  When disabled,
 * no audio stream packets or audio timestamp packets will be sent.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the display for which the dfp
 *     audio should be enabled or disabled.  The display ID must be a dfp display.
 *     If the displayId is not a dfp, this call will return
 *     NV_ERR_INVALID_ARGUMENT.
 *   enable
 *     This parameter specifies whether to enable (NV_TRUE) or disable (NV_FALSE)
 *     audio to the display.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *
 *
 */
#define NV0073_CTRL_CMD_DFP_SET_AUDIO_ENABLE                (0x731150U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_DFP_SET_AUDIO_ENABLE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DFP_SET_AUDIO_ENABLE_PARAMS_MESSAGE_ID (0x50U)

typedef struct NV0073_CTRL_DFP_SET_AUDIO_ENABLE_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvBool enable;
} NV0073_CTRL_DFP_SET_AUDIO_ENABLE_PARAMS;


/*
 * NV0073_CTRL_DFP_ASSIGN_SOR_LINKCONFIG
 *
 * This variable specifies default/primary/secondary sor sublinks to be configured.
 * These access modes are:
 *
 *  NV0073_CTRL_DFP_ASSIGN_SOR_FORCE_NONE
 *    Default link config
 *  NV0073_CTRL_DFP_ASSIGN_SOR_FORCE_PRIMARY_SOR_LINK
 *    Primary sor sublink to be configured
 *  NV0073_CTRL_DFP_ASSIGN_SOR_FORCE_SECONDARY_SOR_LINK
 *    Secondary sor sublink to be configured
 */
typedef NvU32 NV0073_CTRL_DFP_ASSIGN_SOR_LINKCONFIG;

#define NV0073_CTRL_DFP_ASSIGN_SOR_FORCE_NONE               (0x0U)
#define NV0073_CTRL_DFP_ASSIGN_SOR_FORCE_PRIMARY_SOR_LINK   (0x1U)
#define NV0073_CTRL_DFP_ASSIGN_SOR_FORCE_SECONDARY_SOR_LINK (0x2U)

/*
 * NV0073_CTRL_DFP_ASSIGN_SOR_INFO
 *
 * This structure describes info about assigned SOR
 *
 *   displayMask
 *     The displayMask for the SOR corresponding to its HW routings
 *   sorType
 *     This parameter specifies the SOR type
 *          Here are the current defined fields:
 *          NV0073_CTRL_DFP_SOR_TYPE_NONE
 *              Unallocated SOR
 *          NV0073_CTRL_DFP_SOR_TYPE_2H1OR_PRIMARY
 *              Primary SOR for 2H1OR stream
 *          NV0073_CTRL_DFP_SOR_TYPE_2H1OR_SECONDARY
 *              Secondary SOR for 2H1OR stream
 *          NV0073_CTRL_DFP_SOR_TYPE_SINGLE
 *              Default Single SOR
 * Note - sorType should only be referred to identify 2H1OR Primary and Secondary SOR
 *
 */

typedef struct NV0073_CTRL_DFP_ASSIGN_SOR_INFO {
    NvU32 displayMask;
    NvU32 sorType;
} NV0073_CTRL_DFP_ASSIGN_SOR_INFO;

#define NV0073_CTRL_DFP_SOR_TYPE_NONE            (0x00000000U)
#define NV0073_CTRL_DFP_SOR_TYPE_SINGLE          (0x00000001U)
#define NV0073_CTRL_DFP_SOR_TYPE_2H1OR_PRIMARY   (0x00000002U)
#define NV0073_CTRL_DFP_SOR_TYPE_2H1OR_SECONDARY (0x00000003U)

/*
 *  NV0073_CTRL_CMD_DFP_ASSIGN_SOR
 *
 *  This command is used by the clients to assign SOR to DFP for CROSS-BAR
 *  when the default SOR-DFP routing that comes from vbios is not considered.
 *  SOR shall be assigned to a DFP at the runtime. This call should be called
 *  before a modeset is done on any dfp display and also before LinkTraining for DP displays.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     DisplayId of the primary display for which SOR is to be assigned. However, if
 *     displayId is 0 then RM shall return the XBAR config it has stored in it's structures.
 *   sorExcludeMask
 *     sorMask of the SORs which should not be used for assignment. If this is 0,
 *     then SW is free to allocate any available SOR.
 *   slaveDisplayId
 *      displayId of the slave device in case of dualSST mode. This ctrl call will
 *      allocate SORs to both slave and the master if slaveDisplayId is set.
 *   forceSublinkConfig
 *      forces RM to configure primary or secondary sor sublink on the given diaplayId.
 *      If not set, then RM will do the default configurations.
 *   bIs2Head1Or
 *      Specifies that SOR allocation is required for 2 head 1 OR. This will allocate
 *      2 SOR for same displayId - one Master and one Slave. Slave SOR would be disconnected
 *      from any padlink and get feedback clock from Master SOR's padlink.
 *   sorAssignList[NV0073_CTRL_CMD_DFP_ASSIGN_SOR_MAX_SORS]
 *       returns the displayMask for all SORs corresponding to their HW routings.
 *   sorAssignListWithTag[NV0073_CTRL_CMD_DFP_ASSIGN_SOR_MAX_SORS]
 *       returns the displayMask for all SORs corresponding to their HW routings along with
 *       SOR type to identify 2H1OR Primary and Secondary SORs. SOR type would be identified by
 *       NV0073_CTRL_DFP_SOR_TYPE. sorAssignList would look as below -
 *       sorAssignListWithTag[] = { DisplayMask, SOR Type
 *                                 {0x100,       SECONDARY_SOR}
 *                                 {0x200,       SINGLE_SOR}
 *                                 {0x100,       PRIMARY_SOR}
 *                                 {0,           NONE}}
 *                                }
 *       Here, for display id = 0x100, SOR2 is Primary and SOR0 is Secondary.
 *       Note - sorAssignList parameter would be removed after Bug 200749158 is resolved
 *   reservedSorMask
 *       returns the sorMask reserved for the internal panels.
 *   flags
 *       Other detail settings.
 *           _AUDIO_OPTIMAL: Client requests trying to get audio SOR if possible.
 *                           If there's no audio capable SOR and OD is HDMI/DP,
 *                           RM will fail the control call.
 *           _AUDIO_DEFAULT: RM does not check audio-capability of SOR.
 *
 *           _ACTIVE_SOR_NOT_AUDIO_CAPABLE_YES : RM returns Active SOR which is not Audio capable.
 *           _ACTIVE_SOR_NOT_AUDIO_CAPABLE_NO  : RM is not returning 'Active non-audio capable SOR'.
 *
 *  Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */


#define NV0073_CTRL_CMD_DFP_ASSIGN_SOR           (0x731152U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DFP_ASSIGN_SOR_MAX_SORS  4U

#define NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS_MESSAGE_ID (0x52U)

typedef struct NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS {
    NvU32                                 subDeviceInstance;
    NvU32                                 displayId;
    NvU8                                  sorExcludeMask;
    NvU32                                 slaveDisplayId;
    NV0073_CTRL_DFP_ASSIGN_SOR_LINKCONFIG forceSublinkConfig;
    NvBool                                bIs2Head1Or;
    NvU32                                 sorAssignList[NV0073_CTRL_CMD_DFP_ASSIGN_SOR_MAX_SORS];
    NV0073_CTRL_DFP_ASSIGN_SOR_INFO       sorAssignListWithTag[NV0073_CTRL_CMD_DFP_ASSIGN_SOR_MAX_SORS];
    NvU8                                  reservedSorMask;
    NvU32                                 flags;
} NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS;

#define NV0073_CTRL_DFP_ASSIGN_SOR_FLAGS_AUDIO                                      0:0
#define NV0073_CTRL_DFP_ASSIGN_SOR_FLAGS_AUDIO_OPTIMAL                    (0x00000001U)
#define NV0073_CTRL_DFP_ASSIGN_SOR_FLAGS_AUDIO_DEFAULT                    (0x00000000U)
#define NV0073_CTRL_DFP_ASSIGN_SOR_FLAGS_ACTIVE_SOR_NOT_AUDIO_CAPABLE               1:1
#define NV0073_CTRL_DFP_ASSIGN_SOR_FLAGS_ACTIVE_SOR_NOT_AUDIO_CAPABLE_NO  (0x00000000U)
#define NV0073_CTRL_DFP_ASSIGN_SOR_FLAGS_ACTIVE_SOR_NOT_AUDIO_CAPABLE_YES (0x00000001U)

/*
*  NV0073_CTRL_CMD_DFP_GET_PADLINK_MASK
*
*  This command will only be used by chipTB tests to get the padlinks corresponding
*  to the given displayId. RM gets this information from vbios. This control call is
*  only for verif purpose.
*
*   subDeviceInstance
*     This parameter specifies the subdevice instance within the
*     NV04_DISPLAY_COMMON parent device to which the operation should be
*     directed. This parameter must specify a value between zero and the
*     total number of subdevices within the parent device.  This parameter
*     should be set to zero for default behavior.
*   displayId
*     DisplayId of the display for which the client needs analog link Mask
*   padlinkMask
*     analogLinkMask for the given displayId. This value returned is 0xffffffff if
*     the given displayId is invalid else RM returns the corresponding padlinkMask.
*   NV_OK
*   NV_ERR_INVALID_ARGUMENT
*   NV_ERR_NOT_SUPPORTED
*/


#define NV0073_CTRL_CMD_DFP_GET_PADLINK_MASK                              (0x731153U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_DFP_GET_PADLINK_MASK_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DFP_GET_PADLINK_MASK_PARAMS_MESSAGE_ID (0x53U)

typedef struct NV0073_CTRL_DFP_GET_PADLINK_MASK_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 padlinkMask;
} NV0073_CTRL_DFP_GET_PADLINK_MASK_PARAMS;

/*
 * NV0073_CTRL_CMD_DFP_LCD_GPIO_FUNC_TYPE
 *      This enum defines the functions that are supported for which a
 *      corresponding GPIO pin number could be retrieved
 *      Values copied from objgpio.h GPIO_FUNC_TYPE_LCD_*. Please keep the
 *      values in sync between the 2 files
 */

typedef enum NV0073_CTRL_CMD_DFP_LCD_GPIO_FUNC_TYPE {
    // GPIO types of LCD GPIO functions common to all internal panels
    NV0073_CTRL_CMD_DFP_LCD_GPIO_FUNC_TYPE_LCD_BACKLIGHT = 268435456,
    NV0073_CTRL_CMD_DFP_LCD_GPIO_FUNC_TYPE_LCD_POWER = 285212672,
    NV0073_CTRL_CMD_DFP_LCD_GPIO_FUNC_TYPE_LCD_POWER_OK = 301989888,
    NV0073_CTRL_CMD_DFP_LCD_GPIO_FUNC_TYPE_LCD_SELF_TEST = 318767104,
    NV0073_CTRL_CMD_DFP_LCD_GPIO_FUNC_TYPE_LCD_LAMP_STATUS = 335544320,
    NV0073_CTRL_CMD_DFP_LCD_GPIO_FUNC_TYPE_LCD_BRIGHTNESS = 352321536,
} NV0073_CTRL_CMD_DFP_LCD_GPIO_FUNC_TYPE;

/*
 * NV0073_CTRL_CMD_DFP_GET_LCD_GPIO_PIN_NUM
 *
 * This command can be used to get the GPIO pin number that corresponds to one
 * of the LCD functions
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the dfp display.
 *     If more than one displayId bit is set or the displayId is not a dfp,
 *     this call will return NVOS_STATUS_ERROR_INVALID_ARGUMENT.
 *   funcType
 *      The LDC function for which the GPIO pin number is needed
 *   lcdGpioPinNum
 *     The GPIO pin number that corresponds to the LCD function.
 *
*/
#define NV0073_CTRL_CMD_DFP_GET_LCD_GPIO_PIN_NUM (0x731154U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_DFP_GET_LCD_GPIO_PIN_NUM_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DFP_GET_LCD_GPIO_PIN_NUM_PARAMS_MESSAGE_ID (0x54U)

typedef struct NV0073_CTRL_DFP_GET_LCD_GPIO_PIN_NUM_PARAMS {
    NvU32                                  subDeviceInstance;
    NvU32                                  displayId;
    NV0073_CTRL_CMD_DFP_LCD_GPIO_FUNC_TYPE funcType;
    NvU32                                  lcdGpioPinNum;
} NV0073_CTRL_DFP_GET_LCD_GPIO_PIN_NUM_PARAMS;



/*
 *  NV0073_CTRL_CMD_DFP_CONFIG_TWO_HEAD_ONE_OR
 *
 *  This command is used for configuration of 2 head 1 OR.
 *
 *   subDeviceInstance
 *      This parameter specifies the subdevice instance within the
 *      NV04_DISPLAY_COMMON parent device to which the operation should be
 *      directed. This parameter must specify a value between zero and the
 *      total number of subdevices within the parent device.  This parameter
 *      should be set to zero for default behavior.
 *   displayId
 *      Display Id of the panel for which Two Head One OR is going to be used
 *   bEnable
 *      Enable/Disable 2 Head 1 OR
 *   masterSorIdx
 *      Master SOR Index which will send pixels to panel
 *   slaveSorIdx
 *      Slave SOR Index which will take feedback clock from Master SOR's
 *      padlink
 *  Possible status values returned are:
 *   NVOS_STATUS_SUCCESS
 *   NVOS_STATUS_ERROR_INVALID_ARGUMENT
 *   NVOS_STATUS_ERROR_NOT_SUPPORTED
 */


#define NV0073_CTRL_CMD_DFP_CONFIG_TWO_HEAD_ONE_OR (0x731156U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_DFP_CONFIG_TWO_HEAD_ONE_OR_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DFP_CONFIG_TWO_HEAD_ONE_OR_PARAMS_MESSAGE_ID (0x56U)

typedef struct NV0073_CTRL_DFP_CONFIG_TWO_HEAD_ONE_OR_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvBool bEnable;
    NvU32  masterSorIdx;
    NvU32  slaveSorIdx;
} NV0073_CTRL_DFP_CONFIG_TWO_HEAD_ONE_OR_PARAMS;

/*
 *  NV0073_CTRL_CMD_DFP_DSC_CRC_CONTROL
 *
 *  This command is used to enable/disable CRC on the GPU or query the registers
 *  related to it
 *
 *   subDeviceInstance
 *      This parameter specifies the subdevice instance within the
 *      NV04_DISPLAY_COMMON parent device to which the operation should be
 *      directed. This parameter must specify a value between zero and the
 *      total number of subdevices within the parent device.  This parameter
 *      should be set to zero for default behavior.
 *   headIndex
 *      index of the head
 *   cmd
 *      specifying if setup or querying is done
 *   bEnable
 *      enable or disable CRC on the GPU
 *   gpuCrc0
 *      0-indexed CRC register of the GPU
 *   gpuCrc1
 *      1-indexed CRC register of the GPU
 *   gpuCrc0
 *      2-indexed CRC register of the GPU
 *  Possible status values returned are:
 *   NVOS_STATUS_SUCCESS
 *   NVOS_STATUS_ERROR_NOT_SUPPORTED
 */


#define NV0073_CTRL_CMD_DFP_DSC_CRC_CONTROL (0x731157U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_DFP_DSC_CRC_CONTROL_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DFP_DSC_CRC_CONTROL_PARAMS_MESSAGE_ID (0x57U)

typedef struct NV0073_CTRL_DFP_DSC_CRC_CONTROL_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  headIndex;
    NvU32  cmd;
    NvBool bEnable;
    NvU16  gpuCrc0;
    NvU16  gpuCrc1;
    NvU16  gpuCrc2;
} NV0073_CTRL_DFP_DSC_CRC_CONTROL_PARAMS;

#define NV0073_CTRL_DP_CRC_CONTROL_CMD                                     0:0
#define NV0073_CTRL_DP_CRC_CONTROL_CMD_SETUP (0x00000000U)
#define NV0073_CTRL_DP_CRC_CONTROL_CMD_QUERY (0x00000001U)

/*
 * NV0073_CTRL_CMD_DFP_INIT_MUX_DATA
 *
 * This control call is used to configure the display MUX related data
 * for the given display device. Clients to RM are expected to call this
 * control call to initialize the data related to MUX before any MUX related
 * operations such mux switch or PSR entry/ exit are performed.
 *
 *   subDeviceInstance (in)
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *   displayId (in)
 *     ID of the display device for which the mux state has to be initialized
 *   manfId (in)
 *     Specifies the manufacturer ID of panel obtained from the EDID. This
 *     parameter is expected to be non-zero only in case of internal panel.
 *   productId (in)
 *     Specifies the product ID of panel obtained from the EDID. This
 *     parameter is expected to be non-zero only in case of internal panel.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV0073_CTRL_CMD_DFP_INIT_MUX_DATA    (0x731158U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DFP_INIT_MUX_DATA_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DFP_INIT_MUX_DATA_PARAMS_MESSAGE_ID (0x58U)

typedef struct NV0073_CTRL_CMD_DFP_INIT_MUX_DATA_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU16 manfId;
    NvU16 productId;
} NV0073_CTRL_CMD_DFP_INIT_MUX_DATA_PARAMS;



/*
 * NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX
 *
 * This command is used to switch the dynamic display mux between
 * integrated GPU and discrete GPU.
 *
 *   subDeviceInstance (in)
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *   displayId (in)
 *     ID of the display device for which the display MUX has to be switched
 *   flags (in)
 *     Flags indicating the action to be performed. Here are the possible
 *     valid values-
 *       NV0073_CTRL_DFP_DISP_MUX_SWITCH_IGPU_TO_DGPU
 *         When set mux is switched from integrated to discrete GPU.
 *       NV0073_CTRL_DFP_DISP_MUX_SWITCH_DGPU_TO_IGPU
 *         When set mux is switched from discrete to integrated GPU.
 *       NV0073_CTRL_DFP_DISP_MUX_SWITCH_SKIP_SIDEBAND_ACCESS
 *         Set to true for PSR panels as we skip sideband access.
 *   auxSettleDelay (in)
 *     Time, in milliseconds, necessary for AUX channel to settle and become
 *     accessible after a mux switch. Set to zero to use the default delay.
 *   muxSwitchLatencyMs (out)
 *     mux switch latency stats in milli-seconds
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX                   (0x731160U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS_MESSAGE_ID (0x60U)

typedef struct NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 flags;
    NvU32 auxSettleDelay;
    NvU32 muxSwitchLatencyMs;
} NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS;

/* valid flags*/
#define NV0073_CTRL_DFP_DISP_MUX_SWITCH                            0:0
#define NV0073_CTRL_DFP_DISP_MUX_SWITCH_IGPU_TO_DGPU               0x00000000
#define NV0073_CTRL_DFP_DISP_MUX_SWITCH_DGPU_TO_IGPU               0x00000001
#define NV0073_CTRL_DFP_DISP_MUX_SWITCH_SKIP_SIDEBAND_ACCESS       1:1
#define NV0073_CTRL_DFP_DISP_MUX_SWITCH_SKIP_SIDEBAND_ACCESS_YES   0x00000001
#define NV0073_CTRL_DFP_DISP_MUX_SWITCH_SKIP_SIDEBAND_ACCESS_NO    0x00000000

/*
 * NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS
 *
 * This command is used to perform all the operations that need to be
 * performed before a mux switch is started.
 *
 *   subDeviceInstance (in)
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *   displayId (in)
 *     ID of the display device for which the pre mux switch operations have
 *     to be performed.
 *   flags (in)
 *     Flags indicating the action to be performed. Here are the possible
 *     valid values -
 *       NV0073_CTRL_DFP_DISP_MUX_FLAGS_SWITCH_TYPE_IGPU_TO_DGPU
 *         Indicates a switch from i to d is initiated
 *       NV0073_CTRL_DFP_DISP_MUX_FLAGS_SWITCH_TYPE_DGPU_TO_IGPU
 *         Indicates a switch from d to i is initiated
 *       NV0073_CTRL_DFP_DISP_MUX_FLAGS_SR_ENTER_SKIP_NO
 *         When set RM will execute the PSR enter sequence. By default RM will
 *         not skip SR enter sequence
 *       NV0073_CTRL_DFP_DISP_MUX_FLAGS_SR_ENTER_SKIP_YES
 *         When set RM will skip the PSR enter sequence
 *   iGpuBrightness (in)
 *     iGPU brightness value (scale 0~100) before switching mux from I2D.
 *     This is used to match brightness after switching mux to dGPU
 *   preOpsLatencyMs (out)
 *     premux switch operations latency stats in milli-seconds. This includes -
 *      - disabling SOR sequencer and enable BL GPIO control
 *      - toggling LCD VDD, BL EN and PWM MUX GPIOs
 *      - PSR entry, if not skipped
 *   psrEntryLatencyMs (out)
 *     psr entry latency stats in milli-seconds
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS (0x731161U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS_PARAMS_MESSAGE_ID (0x61U)

typedef struct NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 flags;
    NvU32 iGpuBrightness;
    NvU32 preOpsLatencyMs;
    NvU32 psrEntryLatencyMs;
} NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS_PARAMS;

/* valid flags*/
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_SWITCH_TYPE                   0:0
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_SWITCH_TYPE_IGPU_TO_DGPU             0x00000000U
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_SWITCH_TYPE_DGPU_TO_IGPU             0x00000001U
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_SR_ENTER_SKIP                 1:1
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_SR_ENTER_SKIP_NO                     0x00000000U
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_SR_ENTER_SKIP_YES                    0x00000001U
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_MUX_SWITCH_IGPU_POWER_TIMING  2:2
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_MUX_SWITCH_IGPU_POWER_TIMING_KNOWN   0x00000000
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_MUX_SWITCH_IGPU_POWER_TIMING_UNKNOWN 0x00000001
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_SKIP_BACKLIGHT_ENABLE         3:3
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_SKIP_BACKLIGHT_ENABLE_NO             0x00000000U
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_SKIP_BACKLIGHT_ENABLE_YES            0x00000001U

#define NV0073_CTRL_DISP_MUX_BACKLIGHT_BRIGHTNESS_MIN                       0U
#define NV0073_CTRL_DISP_MUX_BACKLIGHT_BRIGHTNESS_MAX                       100U

/*
 * NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS
 *
 * This command is used to perform all the operations that need to be
 * performed after a successful mux switch is completed.
 *
 *   subDeviceInstance (in)
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *   displayId (in)
 *     ID of the display device for which the post mux switch operations have
 *     to be performed.
 *   flags (in)
 *     Flags indicating the action to be performed. Here are the possible
 *     valid values -
 *       NV0073_CTRL_DFP_DISP_MUX_FLAGS_SWITCH_TYPE_IGPU_TO_DGPU
 *         Indicates a switch from i to d is initiated
 *       NV0073_CTRL_DFP_DISP_MUX_FLAGS_SWITCH_TYPE_DGPU_TO_IGPU
 *         Indicates a switch from d to i is initiated
 *       NV0073_CTRL_DFP_DISP_MUX_FLAGS_SR_EXIT_SKIP_NO
 *         When set RM will execute the PSR exit sequence. By default RM will
 *         not skip SR exit sequence
 *       NV0073_CTRL_DFP_DISP_MUX_FLAGS_SR_EXIT_SKIP_YES
 *         When set RM will skip the PSR exit sequence
 *       NV0073_CTRL_DFP_DISP_MUX_FLAGS_MUX_SWITCH_IGPU_POWER_TIMING_KNOWN
 *         Indicates mux switches where we know when igpu powers up
 *       NV0073_CTRL_DFP_DISP_MUX_FLAGS_MUX_SWITCH_IGPU_POWER_TIMING_UNKNOWN
 *         Indicates mux switches where we don't know when igpu powers up
 *   postOpsLatencyMs (out)
 *     postmux switch operations latency stats in milli-seconds. This includes -
  *     - restoring SOR sequencer and BL GPIO control
 *      - toggling LCD VDD, BL EN and PWM MUX GPIOs
 *      - PSR exit, if not skipped
 *   psrExitLatencyMs (out)
 *     psr exit latency stats in milli-seconds
 *   psrExitTransitionToInactiveLatencyMs (out)
 *     psr exit latency stats in milli-seconds, from state 2 (SR active) to state 4 (transition to inactive)
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_TIMEOUT in case of SR exit failure
 */

#define NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS                    (0x731162U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS_PARAMS_MESSAGE_ID (0x62U)

typedef struct NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 flags;
    NvU32 postOpsLatencyMs;
    NvU32 psrExitLatencyMs;
    NvU32 psrExitTransitionToInactiveLatencyMs;
} NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS_PARAMS;

/* valid flags*/
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_SWITCH_TYPE                   0:0
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_SR_EXIT_SKIP                  1:1
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_SR_EXIT_SKIP_NO  0x00000000U
#define NV0073_CTRL_DFP_DISP_MUX_FLAGS_SR_EXIT_SKIP_YES 0x00000001U

/*
 * NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS
 *
 * This command is used to query the display mux status for the given
 * display device
 *
 *   subDeviceInstance (in)
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *   displayId (in)
 *     ID of the display device for which the post mux switch operations have
 *     to be performed.
 *   muxStatus (out)
 *     status indicating the current state of the mux.
 *     valid values -
 *       NV0073_CTRL_DFP_DISP_MUX_STATE_INTEGRATED_GPU
 *         Indicates that the MUX is currently switched to integrated GPU.
 *       NV0073_CTRL_DFP_DISP_MUX_STATE_DISCRETE_GPU
 *         Indicates that the MUX is currently switched to discrete GPU.
 *       NV0073_CTRL_DFP_DISP_MUX_MODE_DISCRETE_ONLY
 *         Indicates that the MUX mode is set to discrete mode, where all displays
 *         are driven by discrete GPU.
 *       NV0073_CTRL_DFP_DISP_MUX_MODE_INTEGRATED_ONLY
 *         Indicates that the MUX mode is set to integrated mode, where all
 *         displays are driven by Integrated GPU.
 *       NV0073_CTRL_DFP_DISP_MUX_MODE_HYBRID
 *         Indicates that the MUX mode is set to hybrid, where internal panel is
 *         driven by integrated GPU, while external displays might be driven by
 *         discrete GPU.
 *       NV0073_CTRL_DFP_DISP_MUX_MODE_DYNAMIC
 *         Indicates that the MUX mode is dynamic. It is only in this mode, the
 *         display MUX can be toggled between discrete and hybrid dynamically.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS         (0x731163U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS_MESSAGE_ID (0x63U)

typedef struct NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 muxStatus;
} NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS;

/* valid flags */
#define NV0073_CTRL_DFP_DISP_MUX_STATE                        1:0
#define NV0073_CTRL_DFP_DISP_MUX_STATE_INVALID                  0x00000000U
#define NV0073_CTRL_DFP_DISP_MUX_STATE_INTEGRATED_GPU           0x00000001U
#define NV0073_CTRL_DFP_DISP_MUX_STATE_DISCRETE_GPU             0x00000002U
#define NV0073_CTRL_DFP_DISP_MUX_MODE                         4:2
#define NV0073_CTRL_DFP_DISP_MUX_MODE_INVALID                   0x00000000U
#define NV0073_CTRL_DFP_DISP_MUX_MODE_INTEGRATED_ONLY           0x00000001U
#define NV0073_CTRL_DFP_DISP_MUX_MODE_DISCRETE_ONLY             0x00000002U
#define NV0073_CTRL_DFP_DISP_MUX_MODE_HYBRID                    0x00000003U
#define NV0073_CTRL_DFP_DISP_MUX_MODE_DYNAMIC                   0x00000004U



/*
*  NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING
*
*  This command can be used to get DSI mode timing parameters.
*
*   subDeviceInstance
*     This parameter specifies the subdevice instance within the
*     NV04_DISPLAY_COMMON parent device to which the operation should be
*     directed. This parameter must specify a value between zero and the
*     total number of subdevices within the parent device.  This parameter
*     should be set to zero for default behavior.
*   displayId
*     This parameter specifies the ID of the display on which the DSI
*     info will be set. The display ID must be a DSI-capable display.
*   hActive
*     This parameter specifies the horizontal length of the active pixel
*     data in the raster.
*   vActive
*     This parameter specifies the vertical lines of the active pixel
*     data in the raster.
*   hFrontPorch
*     This parameter specifies the number of horizontal front porch
*     blanking pixels in the raster.
*   vFrontPorch
*     This parameter specifies the numer of lines of the vertical front
*     porch in the raster.
*   hBackPorch
*     This parameter specifies the number of horizontal back porch
*     blanking pixels in the raster.
*   vBackPorch
*     This parameter specifies the numer of lines of the vertical back
*     porch in the raster.
*   hSyncWidth
*     This parameter specifies the number of horizontal sync pixels in
*     the raster.
*   vSyncWidth
*     This parameter specifies the numer of lines of the vertical sync
*     in the raster.
*   bpp
*     This parameter specifies the depth (Bits per Pixel) of the output
*     display stream.
*   refresh
*     This parameter specifies the refresh rate of the panel (in Hz).
*   pclkHz
*     This parameter specifies the pixel clock rate in Hz.
*   numLanes
*     Number of DSI data lanes.
*   dscEnable
*     Flag to indicate if DSC an be enabled, which in turn indicates if
*     panel supports DSC.
*   dscBpp
*     DSC Bits per pixel
*   dscNumSlices
*     Number of slices for DSC.
*   dscDuaDsc
*     Flag to indicate if panel supports DSC streams from two DSI
*     controllers.
*   dscSliceHeight
*     Height of DSC slices.
*   dscBlockPrediction
*     Flag to indicate if DSC Block Prediction needs to be enabled.
*   dscDecoderVersionMajor
*     Major version number of DSC decoder on Panel.
*   dscDecoderVersionMinor
*     Minor version number of DSC decoder on Panel.
*   dscUseCustomPPS
*     Flag to indicate if Panel uses custom PPS values which deviate from standard values.
*   dscCustomPPSData
*     32 bytes of custom PPS data required by Panel.
*   dscEncoderCaps
*     Capabilities of DSC encoder in SoC.
*
*  Possible status values returned are:
*   NV_OK
*   NV_ERR_INVALID_ARGUMENT
*   NV_ERR_NOT_SUPPORTED
*/

#define NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING         (0x731166U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DFP_DSI_CUSTOM_PPS_DATA_COUNT   32U

#define NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING_PARAMS_MESSAGE_ID (0x66U)

typedef struct NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING_PARAMS {
    NvU32                          subDeviceInstance;
    NvU32                          displayId;
    NvU32                          hActive;
    NvU32                          vActive;
    NvU32                          hFrontPorch;
    NvU32                          vFrontPorch;
    NvU32                          hBackPorch;
    NvU32                          vBackPorch;
    NvU32                          hSyncWidth;
    NvU32                          vSyncWidth;
    NvU32                          bpp;
    NvU32                          refresh;
    NvU32                          pclkHz;
    NvU32                          numLanes;
    NvU32                          dscEnable;
    NvU32                          dscBpp;
    NvU32                          dscNumSlices;
    NvU32                          dscDualDsc;
    NvU32                          dscSliceHeight;
    NvU32                          dscBlockPrediction;
    NvU32                          dscDecoderVersionMajor;
    NvU32                          dscDecoderVersionMinor;
    NvBool                         dscUseCustomPPS;
    NvU32                          dscCustomPPSData[NV0073_CTRL_CMD_DFP_DSI_CUSTOM_PPS_DATA_COUNT];
    NV0073_CTRL_CMD_DSC_CAP_PARAMS dscEncoderCaps;
} NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING_PARAMS;



/*
 * NV0073_CTRL_CMD_DFP_GET_FIXED_MODE_TIMING
 *
 * This control call is used to retrieve the display mode timing info that's
 * specified for a given DFP from an offline configuration blob (e.g., Device Tree).
 * This display timing info is intended to replace the timings exposed in a
 * sink's EDID.
 *
 * subDeviceInstance (in)
 *   This parameter specifies the subdevice instance within the
 *   NV04_DISPLAY_COMMON parent device to which the operation should be
 *   directed.
 * displayId (in)
 *   ID of the display device for which the timings should be retrieved.
 * stream (in)
 *   For MST connectors with static topologies (e.g., DP serializers),
 *   this parameter further identifies the video stream for which the
 *   timings should be retrieved.
 * valid (out)
 *   Indicates whether a valid display timing was found for this DFP.
 * hActive (out)
 *   Horizontal active width in pixels
 * hFrontPorch (out)
 *   Horizontal front porch
 * hSyncWidth (out)
 *   Horizontal sync width
 * hBackPorch (out)
 *   Horizontal back porch
 * vActive (out)
 *   Vertical active height in lines
 * vFrontPorch (out)
 *   Vertical front porch
 * vSyncWidth (out)
 *   Vertical sync width
 * vBackPorch (out)
 *   Vertical back porch
 * pclkKHz (out)
 *   Pixel clock frequency in KHz
 * rrx1k (out)
 *   Refresh rate in units of 0.001Hz
* x (out)
*   x offset inside superframe at which this view starts
 * y (out)
*   y offset inside superframe at which this view starts
 * width (out)
*   Horizontal active width in pixels for this view
* height (out)
*   Vertical active height in lines for this view
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0073_CTRL_CMD_DFP_GET_FIXED_MODE_TIMING              (0x731172) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8 | NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS_MESSAGE_ID)" */

#define NV0073_CTRL_DFP_FIXED_MODE_TIMING_MAX_SUPERFRAME_VIEWS 4U

#define NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS_MESSAGE_ID (0x72U)

typedef struct NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvU8   stream;

    NvBool valid;

    NvU16  hActive;
    NvU16  hFrontPorch;
    NvU16  hSyncWidth;
    NvU16  hBackPorch;

    NvU16  vActive;
    NvU16  vFrontPorch;
    NvU16  vSyncWidth;
    NvU16  vBackPorch;

    NvU32  pclkKHz;
    NvU32  rrx1k;

    struct {
        NvU8 numViews;
        struct {
            NvU16 x;
            NvU16 y;
            NvU16 width;
            NvU16 height;
        } view[NV0073_CTRL_DFP_FIXED_MODE_TIMING_MAX_SUPERFRAME_VIEWS];
    } superframeInfo;
} NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS;



/*
 * NV0073_CTRL_DFP_ENTER_DISPLAY_POWER_GATING_DIAGNOSTIC_DATA
 *
 * This structure describes diagnostic information about display power
 * gating entry sequence
 *
 *   totalRmEntryLatencyUs
 *     Duration in microseconds that RM took to service 'Enter
 *     Display Power Gating' command. This includes time for all
 *     steps that RM performs as part of display power gating entry
 *     sequence including the below parameters.
 *   hwOkToGateLatencyUs
 *     Duration in microseconds that HW took to assert ok_to_gate.
 *     Only valid when displayId is not equal to 0xFFFFFFFF
 *   carApiLatencyUs
 *     Duration in microseconds that CAR (Clock and Reset) block took to
 *     service 'Enter Display Power Gating' command
 *
 */

typedef struct NV0073_CTRL_DFP_ENTER_DISPLAY_POWER_GATING_DIAGNOSTIC_DATA {
    NvU32 totalRmEntryLatencyUs;
    NvU32 hwOkToGateLatencyUs;
    NvU32 carEntryApiLatencyUs;
} NV0073_CTRL_DFP_ENTER_DISPLAY_POWER_GATING_DIAGNOSTIC_DATA;

/*
 *  NV0073_CTRL_CMD_DFP_ENTER_DISPLAY_POWER_GATING
 *
 * This command can be used to enter display power gating with an option to
 * save-restore settings for the specified displayId.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device. This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the display for which the state
 *     needs to be saved-restored during exit of Display Power Gating.
 *     The display ID must be a dfp display as determined with the
 *     NV0073_CTRL_CMD_SPECIFIC_GET_TYPE command. If more than one
 *     displayId bit is set or the displayId is not a dfp, this call will
 *     return NV_ERR_INVALID_ARGUMENT. For the case where no save-restore
 *     is needed, displayId should be set to 0xFFFFFFFF.
 *   flags
 *     This parameter specifies special request from client for RM(for future use)
 *   diagnosticData
 *     This parameter provides diagnostic information about display power
 *     gating entry sequence
 *
 * Possible status values returned are:
 *   NV_OK - If Display Power Gating Entry was successful
 *   NV_ERR_GENERIC - If Display Power Gating Entry failed
 *   NV_ERR_INVALID_ARGUMENT - If incorrect parameters are sent
 */
#define NV0073_CTRL_CMD_DFP_ENTER_DISPLAY_POWER_GATING (0x731174U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DFP_ENTER_DISPLAY_POWER_GATING_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DFP_ENTER_DISPLAY_POWER_GATING_PARAMS_MESSAGE_ID (0x74U)

typedef struct NV0073_CTRL_CMD_DFP_ENTER_DISPLAY_POWER_GATING_PARAMS {
    NvU32                                                      subDeviceInstance;
    NvU32                                                      displayId;
    NvU32                                                      flag;
    NV0073_CTRL_DFP_ENTER_DISPLAY_POWER_GATING_DIAGNOSTIC_DATA diagnosticData;
} NV0073_CTRL_CMD_DFP_ENTER_DISPLAY_POWER_GATING_PARAMS;

#define NV0073_CTRL_DFP_ENTER_DISPLAY_POWER_GATING_FLAGS_RESERVED       31:0
#define NV0073_CTRL_DFP_ENTER_DISPLAY_POWER_GATING_FLAGS_RESERVED_INIT (0x00000000U)

/*
 * NV0073_CTRL_DFP_EXIT_DISPLAY_POWER_GATING_DIAGNOSTIC_DATA
 *
 * This structure describes diagnostic information about display power
 * gating exit sequence
 *
 *   totalRmExitLatencyUs
 *     Duration in microseconds that RM took to service 'Exit
 *     Display Power Gating' command. This includes time for all
 *     steps that RM performs as part of display power gating exit
 *     sequence including the below parameters.
 *   riscvBootupLatencyUs
 *     Duration in microseconds that LTM RISCV took to bootup.
 *   carExitApiLatencyUs
 *     Duration in microseconds that CAR (Clock and Reset) block took
 *     to service 'Exit Display Power Gating' command
 *
 */
typedef struct NV0073_CTRL_DFP_EXIT_DISPLAY_POWER_GATING_DIAGNOSTIC_DATA {
    NvU32 totalRmExitLatencyUs;
    NvU32 riscvBootupLatencyUs;
    NvU32 carExitApiLatencyUs;
} NV0073_CTRL_DFP_EXIT_DISPLAY_POWER_GATING_DIAGNOSTIC_DATA;

/*
 * NV0073_CTRL_CMD_DFP_EXIT_DISPLAY_POWER_GATING
 *
 * This command can be used to exit display power gating. If preceding
 * NV0073_CTRL_CMD_DFP_ENTER_DISPLAY_POWER_GATING command requested for
 * save-restore of settings for a particular displayId then this command
 * will restore settings for that displayId.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device. This parameter
 *     should be set to zero for default behavior.
 *   flags
 *     This parameter specifies special request from client for RM(for future use)
 *   diagnosticData
 *     This parameter provides diagnostic information about display power
 *     gating exit sequence
 *
 * Possible status values returned are:
 *   NV_OK - When Display Power Gating Exit was successful
 *   NV_ERR_GENERIC - When Display Power Gating Exit failed
 *   NV_ERR_INVALID_ARGUMENT - When incorrect parameters are sent
 */

#define NV0073_CTRL_CMD_DFP_EXIT_DISPLAY_POWER_GATING (0x731175U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DFP_EXIT_DISPLAY_POWER_GATING_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DFP_EXIT_DISPLAY_POWER_GATING_PARAMS_MESSAGE_ID (0x75U)

typedef struct NV0073_CTRL_CMD_DFP_EXIT_DISPLAY_POWER_GATING_PARAMS {
    NvU32                                                     subDeviceInstance;
    NvU32                                                     flag;
    NV0073_CTRL_DFP_EXIT_DISPLAY_POWER_GATING_DIAGNOSTIC_DATA diagnosticData;
} NV0073_CTRL_CMD_DFP_EXIT_DISPLAY_POWER_GATING_PARAMS;

#define NV0073_CTRL_DFP_EXIT_DISPLAY_POWER_GATING_FLAGS_RESERVED       31:0
#define NV0073_CTRL_DFP_EXIT_DISPLAY_POWER_GATING_FLAGS_RESERVED_INIT (0x00000000U)

/*
 * NV0073_CTRL_CMD_DFP_EDP_DRIVER_UNLOAD
 *
 * This command is called when we want to inform RM of driver
 * unload.
 *
 *    subDeviceInstance (in)
 *        This parameter specifies the subdevice instance within the
 *        NV04_DISPLAY_COMMON parent device to which the operation
 *        should be directed.
 *    displayId (in)
 *        This parameter inputs the displayId of the active display. A value
 *        of zero indicates no display is active.
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_PARAM_STRUCT
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_NOT_SUPPORTED
 */
#define NV0073_CTRL_CMD_DFP_EDP_DRIVER_UNLOAD                         (0x731176U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_DFP_EDP_DRIVER_UNLOAD_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DFP_EDP_DRIVER_UNLOAD_PARAMS_MESSAGE_ID (0x76U)

typedef struct NV0073_CTRL_DFP_EDP_DRIVER_UNLOAD_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
} NV0073_CTRL_DFP_EDP_DRIVER_UNLOAD_PARAMS;



/*
 * NV0073_CTRL_CMD_DFP_SET_FORCE_BLACK_PIXELS
 *
 * This command is used to force black pixels from postcomp.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *
 *   displayId
 *     DisplayId of the connected display.
 *
 *   bForceBlackPixels
 *     To enable or disable black pixel generation.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_NOT_SUPPORTED
 *
 */
#define NV0073_CTRL_CMD_DFP_SET_FORCE_BLACK_PIXELS (0x731179U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DFP_INTERFACE_ID << 8) | NV0073_CTRL_DFP_SET_FORCE_BLACK_PIXELS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DFP_SET_FORCE_BLACK_PIXELS_PARAMS_MESSAGE_ID (0x79U)

typedef struct NV0073_CTRL_DFP_SET_FORCE_BLACK_PIXELS_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvU32  head;
    NvBool bForceBlack;
} NV0073_CTRL_DFP_SET_FORCE_BLACK_PIXELS_PARAMS;

/* _ctrl0073dfp_h_ */
