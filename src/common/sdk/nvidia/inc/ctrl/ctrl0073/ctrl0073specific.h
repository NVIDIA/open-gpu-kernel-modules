/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0073/ctrl0073specific.finn
//

#include "ctrl/ctrl0073/ctrl0073base.h"

#include "ctrl/ctrlxxxx.h"
/* NV04_DISPLAY_COMMON display-specific control commands and parameters */

/*
 * NV0073_CTRL_CMD_SPECIFIC_GET_TYPE
 *
 * This command can be used to determine the associated display type for
 * the specified displayId.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should  be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the display for which the display
 *     type is to be returned. Only one display may be indicated in this
 *     parameter.
 *   displayType
 *     This parameter returns the display type associated with the specified
 *     displayId parameter.  Valid displayType values are:
 *       NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_CRT
 *       NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP
 *       NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_TV
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SPECIFIC_GET_TYPE (0x730240U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_TYPE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_GET_TYPE_PARAMS_MESSAGE_ID (0x40U)

typedef struct NV0073_CTRL_SPECIFIC_GET_TYPE_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 displayType;
} NV0073_CTRL_SPECIFIC_GET_TYPE_PARAMS;

/* valid display types */
#define NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_UNKNOWN    (0x00000000U)
#define NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_CRT        (0x00000001U)
#define NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP        (0x00000002U)
#define NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_TV         (0x00000003U)

/*
 * NV0073_CTRL_CMD_SPECIFIC_GET_EDID_V2
 *
 * This command can be used to request the EDID for the specified displayId.
 *
 *   [in] subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the total
 *     number of subdevices within the parent device.  This parameter should
 *     be set to zero for default behavior.
 *   [in] displayId
 *     This parameter specifies the display to read the EDID. The display ID
 *     must specify a display with a positive connect state as determined
 *     with the NV0073_CTRL_CMD_SYSTEM_GET_CONNECT_STATE command.  Only one
 *     display may be indicated in this parameter.  If a more than one
 *     display Id is used, the RM will return NV_ERR_INVALID_ARGUMENT.
 *   [out] bufferSize
 *     This parameter returns the number of bytes copied into edidBuffer after
 *     performing the requested EDID operations.
 *   [out] edidBuffer
 *     The array of EDIDs that RM will fill after the requested operations. If
 *     the size of the array is not large enough to hold the number of bytes to
 *     be copied, NV_ERR_INVALID_ARGUMENT will be returned.
 *   [in] flags
 *     This parameter defines the specific operations that will be performed
 *     in reading the EDID.
 *     Here are the current defined fields:
 *       NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_COPY_CACHE
 *         A client uses this field to indicate whether to return the cached
 *         copy of the EDID or to use DDC to read the EDID from the display.
 *         Possible values are:
 *           NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_COPY_CACHE_NO
 *             The RM will use DDC to grab the EDID.
 *           NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_COPY_CACHE_YES
 *             The RM will copy the last EDID found into the clients
 *             buffer.  No DDC will be performed.
 *       NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_READ_MODE
 *         A client uses this field to indicate whether to read from
 *         the HW and return the EDID w/o any patching
 *         Possible values are:
 *           NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_READ_MODE_COOKED
 *             Use the _COPY_CACHE policy
 *           NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_READ_MODE_RAW
 *             Perform the read and return an unadulterated EDID.
 *       NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_DISPMUX_READ_MODE
 *         A client uses this field to indicate whether to read EDID
 *         from SBIOS using ACPI sub function for display dynamic switching
 *         feature. This flag should only be set on internal display with
 *         dynamic switching feature enabled.
 *         Possible values are:
 *           NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_DISPMUX_READ_MODE_ACPI
 *             RM reads the EDID from SBIOS and returns the raw EDID provided
 *             by SBIOS.
 *           NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_DISPMUX_READ_MODE_DEFAULT
 *             EDID is read based on rest of the 'flags' that are passed to
 *             this function.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SPECIFIC_GET_EDID_V2         (0x730245U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_EDID_V2_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_GET_EDID_MAX_EDID_BYTES 2048U

#define NV0073_CTRL_SPECIFIC_GET_EDID_V2_PARAMS_MESSAGE_ID (0x45U)

typedef struct NV0073_CTRL_SPECIFIC_GET_EDID_V2_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 bufferSize;
    NvU32 flags;
    NvU8  edidBuffer[NV0073_CTRL_SPECIFIC_GET_EDID_MAX_EDID_BYTES];
} NV0073_CTRL_SPECIFIC_GET_EDID_V2_PARAMS;

#define NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_COPY_CACHE                           0:0
#define NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_COPY_CACHE_NO             0x00000000U
#define NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_COPY_CACHE_YES            0x00000001U

#define NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_READ_MODE                            1:1
#define NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_READ_MODE_COOKED          0x00000000U
#define NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_READ_MODE_RAW             0x00000001U

#define NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_DISPMUX_READ_MODE                    3:2
#define NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_DISPMUX_READ_MODE_DEFAULT 0x00000000U
#define NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_DISPMUX_READ_MODE_ACPI    0x00000001U

/*
 * NV0073_CTRL_CMD_SPECIFIC_SET_EDID_V2
 *
 * This command can be used to set or remove a complete EDID for the
 * specified displayId.  Once the EDID is set, any requests
 * to read the EDID or use DDC detection will always use a cached copy of
 * the EDID.  That is, the EDID becomes static until disabled by calling
 * this same function with edidBuffer.  Note, that DDC based
 * detection will always pass for any displayId that has set an EDID. Also,
 * this path will not store any value across reboots.  If an EDID needs to
 * remain set after a reboot, RM clients must call this function again.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the total
 *     number of subdevices within the parent device.  This parameter should
 *     be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the display to store or the EDID.  Only one
 *     display may be indicated in this parameter.  If more than one displayId
 *     is used, the RM will return NV_ERR_INVALID_ARGUMENT.
 *     If the displayId does not use DDC and hence would not have an EDID,
 *     then the RM could also return NV_ERR_INVALID_ARGUMENT.
 *   bufferSize
 *     This parameter specifies the size of the EDID buffer pointed to by
 *     pEdidBuffer.  If the EDID write contains more bytes than bufferSize,
 *     the RM will extend the bufferSize of the EDID inside the RM to match.
 *     Note a bufferSize of 0 would mean no bytes will be copied, but set the
 *     current cached EDID as static.
 *   edidBuffer
 *     This parameter specifies the EDID buffer that the RM will copy into
 *     the RM buffer. If the EDID buffer is empty, the RM will remove any
 *     previous set EDID and allow further detection and EDID reads to use DDC.
 *     The RM will not check to see if the EDID is valid here or not.
 *     The client should validate the EDID if needed before calling this function.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *
 */

#define NV0073_CTRL_CMD_SPECIFIC_SET_EDID_V2                          (0x730246U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_SET_EDID_V2_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_SET_EDID_V2_PARAMS_MESSAGE_ID (0x46U)

typedef struct NV0073_CTRL_SPECIFIC_SET_EDID_V2_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 bufferSize;
    NvU8  edidBuffer[NV0073_CTRL_SPECIFIC_GET_EDID_MAX_EDID_BYTES];
} NV0073_CTRL_SPECIFIC_SET_EDID_V2_PARAMS;

/*
 * NV0073_CTRL_CMD_SPECIFIC_FAKE_DEVICE
 *
 * This Control Cmd is for providing the Faking device(s) support from RM.
 * This command serves as the entry point for all interaction of RM with
 * user mode component of the any internal [test] tool. The Faking framework
 * in RM will be activated only after the usermode app sends in a proper ENABLE
 * cmd first. Any attempt to issue other cmds while the faking code has not
 * been enabled will result in RM err _INVALID_DATA.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the total
 *     number of subdevices within the parent device.  This parameter should
 *     be set to zero for default behavior.
 *   cmd
 *     This field will carry the command to be executed by the framework.
 *     This includes Enabling/Disabling the test framework and faking devices
 *     like CRT/DVI/TV.
 *   data
 *     This field is to carry the data required for executing the cmd.
 *     Except for Enable and Disable, the other faking device commands will
 *     require the device mask of the device to be faked/removed.
 *   tvType
 *     This field specifies a specific TV type while faking a TV.
 *     Possible values are:
 *       NV0073_FAKE_DEVICE_TV_NONE
 *       NV0073_FAKE_DEVICE_TV_SVIDEO
 *       NV0073_FAKE_DEVICE_TV_COMPOSITE
 *       NV0073_FAKE_DEVICE_TV_COMPONENT
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_DATA
 *
 */

#define NV0073_CTRL_CMD_SPECIFIC_FAKE_DEVICE (0x730243U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_CMD_SPECIFIC_FAKE_DEVICE_PARAMS_MESSAGE_ID" */

/* valid fake device TV connector types */
#define NV0073_FAKE_DEVICE_TV_NONE           (0U)
#define NV0073_FAKE_DEVICE_TV_SVIDEO         (1U)
#define NV0073_FAKE_DEVICE_TV_COMPOSITE      (2U)
#define NV0073_FAKE_DEVICE_TV_COMPONENT      (3U)

#define NV0073_CTRL_CMD_SPECIFIC_FAKE_DEVICE_PARAMS_MESSAGE_ID (0x43U)

typedef struct NV0073_CTRL_CMD_SPECIFIC_FAKE_DEVICE_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 cmd;
    NvU32 data;
    NvU32 tvType;
} NV0073_CTRL_CMD_SPECIFIC_FAKE_DEVICE_PARAMS;

/* Faking Support commands */
/* some random value to enable/disable test code */
#define NV0073_FAKE_DEVICE_SUPPORT_ENABLE         0x11faU
#define NV0073_FAKE_DEVICE_SUPPORT_DISABLE        0x99ceU
#define NV0073_FAKE_DEVICE_SUPPORT_ATTACH_DEVICES 0x100U
#define NV0073_FAKE_DEVICE_SUPPORT_REMOVE_DEVICES 0x101U

/*
 * NV0073_CTRL_CMD_SPECIFIC_GET_I2C_PORTID
 *
 * This command returns the I2C portID for the specified display device.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the display for which information is to be
 *     returned.  Only one display may be indicated in this parameter.
 *     If more than one displayId is used a failing status of
 *     NV_ERR_INVALID_ARGUMENT will be returned.
 *   commPortId
 *      This parameter returns the I2C communication port ID of the
 *      display device indicated by the displayId parameter.
 *   ddcPortId
 *      This parameter returns the I2C DDC port ID of the display device
 *      indicated by the displayId parameter.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0073_CTRL_CMD_SPECIFIC_GET_I2C_PORTID   (0x730211U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS_MESSAGE_ID (0x11U)

typedef struct NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 commPortId;
    NvU32 ddcPortId;
} NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS;

#define NV0073_CTRL_SPECIFIC_I2C_PORT_NONE          (0x0U)

/*
 * NV0073_CTRL_CMD_SPECIFIC_GET_CONNECTOR_DATA
 *
 * This command can be used to get display connector data.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the display for which information is to be
 *     returned.  Only one display may be indicated in this parameter.
 *     If more than one displayId is used a failing status of
 *     NV_ERR_INVALID_ARGUMENT will be returned.
 *   DDCPartners
 *     This parameter specifies an NV0073_DISPLAY_MASK value describing
 *     the set of displays that share the same DDC line as displayId.  This
 *     parameter will always be returned even if we also return the
 *     NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_FLAGS_PRESENT_NO flag.
 *   flags
 *     This parameter specifies optional flags to be used while retrieving
 *     the connector data for a given displayId.
 *     Legal values for this parameter include:
 *       NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_FLAGS_PRESENT
 *         This flag describes whether the connector data is present
 *         inside the firmware.
 *   count
 *     This parameter returns the number of connectors associated with
 *     the displayId argument.  This value indicates the number of
*      valid entries returned in the data parameter.
 *   data
 *     This parameter returns an array of structures containing the connector
 *     data associated with each connector for the given displayId argument.
 *     The count field specifies how many entries in this array are returned.
 *     Each entry in the array contains the following members:
 *       index
 *         This value is the index associated with the given connector. If
 *         two displayIds share the same index, then they share the same
 *         connector.
 *       type
 *         This value defines the type of connector associated with the
 *         displayId argument.
 *       location
 *         This value provides a possible means to determine the relative
 *         location of the connector in association to other connectors.
 *         For desktop boards, a value of zero defines the south most
 *         connector (the connector closest to the bus slot into which
 *         the board is inserted).
 *   platform
 *     This value defines the type of system with which to associate the
 *     location of each connector.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *
 */

#define NV0073_CTRL_CMD_SPECIFIC_GET_CONNECTOR_DATA (0x730250U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_CONNECTOR_DATA_PARAMS_MESSAGE_ID" */

/* maximum number of connectors */
#define NV0073_CTRL_MAX_CONNECTORS                  4U

#define NV0073_CTRL_SPECIFIC_GET_CONNECTOR_DATA_PARAMS_MESSAGE_ID (0x50U)

typedef struct NV0073_CTRL_SPECIFIC_GET_CONNECTOR_DATA_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 flags;
    NvU32 DDCPartners;
    NvU32 count;
    struct {
        NvU32 index;
        NvU32 type;
        NvU32 location;
    } data[NV0073_CTRL_MAX_CONNECTORS];
    NvU32 platform;
} NV0073_CTRL_SPECIFIC_GET_CONNECTOR_DATA_PARAMS;

/* defines for the flags field */
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_FLAGS_PRESENT            0:0
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_FLAGS_PRESENT_NO                           0x00000000U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_FLAGS_PRESENT_YES                          0x00000001U

/* defines for the data[].type field */
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_VGA_15_PIN                            0x00000000U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_TV_COMPOSITE                          0x00000010U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_TV_SVIDEO                             0x00000011U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_TV_HDTV_COMPONENT                     0x00000013U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_TV_SCART                              0x00000014U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_TV_COMPOSITE_SCART_OVER_EIAJ4120_BLUE 0x00000016U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_TV_HDTV_EIAJ4120                      0x00000017U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_PC_POD_HDTV_YPRPB                     0x00000018U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_PC_POD_SVIDEO                         0x00000019U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_PC_POD_COMPOSITE                      0x0000001AU
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DVI_I_TV_SVIDEO                       0x00000020U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DVI_I_TV_COMPOSITE                    0x00000021U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DVI_I                                 0x00000030U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DVI_D                                 0x00000031U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_ADC                                   0x00000032U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_LFH_DVI_I_1                           0x00000038U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_LFH_DVI_I_2                           0x00000039U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_SPWG                                  0x00000040U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_OEM                                   0x00000041U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DP_EXT                                0x00000046U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DP_INT                                0x00000047U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DP_MINI_EXT                           0x00000048U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DP_SERIALIZER                         0x00000049U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_HDMI_A                                0x00000061U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_HDMI_C_MINI                           0x00000063U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_LFH_DP_1                              0x00000064U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_LFH_DP_2                              0x00000065U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_VIRTUAL_WFD                           0x00000070U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_USB_C                                 0x00000071U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DSI                                   0x00000072U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_STEREO_3PIN_DIN                       0x00000073U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_USB_C_UHBR                            0x00000074U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_UNKNOWN                               0xFFFFFFFFU

/* defines for the platform field */
#define NV0073_CTRL_SPECIFIC_CONNECTOR_PLATFORM_DEFAULT_ADD_IN_CARD                    0x00000000U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_PLATFORM_TWO_PLATE_ADD_IN_CARD                  0x00000001U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_PLATFORM_CONFIGURABLE                           0x00000002U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_PLATFORM_DESKTOP_FULL_DP                        0x00000007U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_PLATFORM_MOBILE_ADD_IN_CARD                     0x00000008U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_PLATFORM_MXM                                    0x00000009U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_PLATFORM_MOBILE_BACK                            0x00000010U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_PLATFORM_MOBILE_BACK_LEFT                       0x00000011U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_PLATFORM_MOBILE_BACK_DOCK                       0x00000018U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_PLATFORM_CRUSH_DEFAULT                          0x00000020U
#define NV0073_CTRL_SPECIFIC_CONNECTOR_PLATFORM_UNKNOWN                                0xFFFFFFFFU

/*
 * NV0073_CTRL_CMD_SYSTEM_GET_HDCP_REPEATER_INFO
 *
 * This command is used to get HDCP repeater information. From the
 * repeater device this call returns KsvList,BStatus and VPrime. If the
 * device is implemented internally, the client call supply a Cksv and Cn
 * And in turn following parameters are returned: MPrime, Dksv which are used
 * for upstream authentication. In addition to this the flag bAuthRequired
 * shall be set to indicate that upstream authentication is required along with
 * comparing the KsvList with SRM. On the other hand, if the device is an
 * external implementation MPrime and Dksv values shall be ignored and the flag
 * bAuthRequired will not be set indicating upstream authentication not
 * required and just the comparison of KsvList with upstream SRM is required.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.  This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter indicates the digital display device's
 *     displayId. This comes as input to this command.
 *   bRepeaterPending
 *     The repeater pending flag as the output to this command.
 *     The flag returned indicating whether repeater is ready or not
 *     TRUE if repeater is not ready and FALSE otherwise.
 *   Cn
 *     This parameter is the input value Cn a 64 bit random number
 *     to be provided to this command. Cn value is Upstream protocol's
 *     exchange random number.This value must be written by software
 *     before the KSV is written if the transmitter device follows the HDCP
 *     upstream protocol. If the transmitter supports a proprietary method of
 *     authenticating the repeater device, Cn can be ignored.
 *   Cksv
 *     This parameter is the input value Cksv (a unique identifier) of 40 bit
 *     size to be provided to this command. This input value shall contain 20
 *     ones and 20 zeros in accordance with the HDCP specification. This value
 *     must be written by software before the KSV is written if the transmitter
 *     device follows the HDCP upstream protocol. If the transmitter supports
 *     a proprietary method of authenticating the repeater device, Cksv can be
 *     ignored.
 *   actualKsvSize
 *     The actual KSV list size(in bytes) returned back as output while reading
 *     KSV list.
 *   ksvList
 *     In case downstream device is repeater then this will give the list of
 *     KSV's of all downstream devices attached to the repeater. It differs
 *     from actualKsvSize because this allocates maximum allowed size.
 *     If downstream device is receiver then this array will contain all zeros.
 *   BStatus
 *     The BSTATUS value as output by this command.This value's bit fields
 *     contains information returned by repeater device such as total number of
 *     downstream devices attached to the repeater excluding HDCP repeater,
 *     value for the depth indicating number of connection levels through
 *     connection topology, this value also gives information about maximum
 *     cascaded and devices exceeded (127).
 *   VPrime
 *      The VPRIME value returned as output by this command from the repeater
 *      device. This value should be used to compare the verification value
 *      during the HDCP upstream protocol using SHA1 in accordance with the
 *      upstream protocol. This value can be ignored if bAuthRequired is not
 *      set indicating the verification is done by the transmitter device.
 *   bAuthrequired
 *     The authentication flag as the output to this command.
 *     The Flag returned indicating whether authentication is required or not
 *     TRUE if authentication required and FALSE otherwise.
 *   MPrime
 *     The MPRIME value returned as output by this command.
 *     MPrime shall be decrypted by the client and used in the SHA-1
 *     computation of V during upstream authentication. This value can be
 *     ignored if bAuthRequired is not set indicating the verification is
 *     done by the transmitter device.
 *   Dksv
 *     This parameter is the output value DKSV of 40 bit size.
 *     As per the HDCP specification this value should contain 20 ones and
 *     20 zeros.This value can be ignored if bAuthRequired is not set
 *     indicating the verification is done by the transmitter device.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_REP_KSV_SIZE                                                       (5U)
#define NV0073_CTRL_DKSV_SIZE                                                          (5U)
#define NV0073_CTRL_MPRIME_SIZE                                                        (0x2U) /* finn: Evaluated from "(8 / 4)" */
#define NV0073_CTRL_CN_SIZE                                                            (0x2U) /* finn: Evaluated from "(8 / 4)" */
#define NV0073_CTRL_CKSV_SIZE                                                          (5U)
#define NV0073_CTRL_VPRIME_SIZE                                                        (0x5U) /* finn: Evaluated from "(20 / 4)" */

#define NV0073_CTRL_MAX_HDCP_REPEATER_COUNT                                            (0x27bU) /* finn: Evaluated from "(NV0073_CTRL_REP_KSV_SIZE * 127)" */

#define NV0073_CTRL_CMD_SPECIFIC_GET_HDCP_REPEATER_INFO                                (0x730260U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_HDCP_REPEATER_INFO_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_GET_HDCP_REPEATER_INFO_PARAMS_MESSAGE_ID (0x60U)

typedef struct NV0073_CTRL_SPECIFIC_GET_HDCP_REPEATER_INFO_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU8  bRepeaterPending;
    NvU32 Cn[NV0073_CTRL_CN_SIZE];
    NvU8  Cksv[NV0073_CTRL_CKSV_SIZE];
    NvU32 actualKsvSize;
    NvU8  ksvList[NV0073_CTRL_MAX_HDCP_REPEATER_COUNT];
    NvU16 BStatus;
    NvU32 VPrime[NV0073_CTRL_VPRIME_SIZE];
    NvU8  bAuthrequired;
    NvU32 MPrime[NV0073_CTRL_MPRIME_SIZE];
    NvU8  Dksv[NV0073_CTRL_DKSV_SIZE];
} NV0073_CTRL_SPECIFIC_GET_HDCP_REPEATER_INFO_PARAMS;

/*
 * NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_ENABLE
 *
 * This command is used to signal the resource manager that the upcoming mode
 * shall be hdmi vs dvi.  This is required since the resource manager
 * does not read the display edid. The resource manager shall enable hdmi
 * components such as turning on the audio engine for instance.  This should
 * be called prior to every modeset in which the displayId is capable of hdmi.
 *   displayId
 *     This parameter specifies the displayId of HDMI resource to configure.
 *     This comes as input to this command.
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which operation should be directed.
 *     This parameter must specify a value between zero and the total number
 *     of subdevices within the parent device.  This parameter should be set
 *     to zero for default behavior.
 *   enable
 *     This field specifies the legal values:
 *       NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_TRUE
 *       NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_FALSE
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_ENABLE (0x730273U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_PARAMS_MESSAGE_ID (0x73U)

typedef struct NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_PARAMS {
    NvU8  subDeviceInstance;
    NvU32 displayId;
    NvU8  enable;
} NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_PARAMS;

#define NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_FALSE (0x00000000U)
#define NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_TRUE  (0x00000001U)

/*
 * NV0073_CTRL_CMD_SPECIFIC_CTRL_HDMI
 *
 * This command can be used to enable HDMI communication on the associated GPU.
 * This should be called prior to every modeset in which the displayId is capable of HDMI.
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which operation should be directed.
 *     This parameter must specify a value between zero and the total number
 *     of subdevices within the parent device.  This parameter should be set
 *     to zero for default behavior.
 *   displayId
 *     This parameter specifies the displayId of HDMI resource to configure.
 *     This comes as input to this command.
 *   enable
 *     This field specifies the legal values:
 *       NV0073_CTRL_SPECIFIC_CTRL_HDMI_DISABLE
 *       NV0073_CTRL_SPECIFIC_CTRL_HDMI_ENABLE
 *
 *  Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SPECIFIC_CTRL_HDMI         (0x730274U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_CTRL_HDMI_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_CTRL_HDMI_PARAMS_MESSAGE_ID (0x74U)

typedef struct NV0073_CTRL_SPECIFIC_CTRL_HDMI_PARAMS {
    NvU8   subDeviceInstance;
    NvU32  displayId;
    NvBool bEnable;
} NV0073_CTRL_SPECIFIC_CTRL_HDMI_PARAMS;

#define NV0073_CTRL_SPECIFIC_CTRL_HDMI_DISABLE             (0x00000000U)
#define NV0073_CTRL_SPECIFIC_CTRL_HDMI_ENABLE              (0x00000001U)

/*
 * NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM
 *
 * This command is used to signal the resource manager that the audio stream
 * is to be mute off or on.
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which operation should be directed.
 *     This parameter must specify a value between zero and the total number
 *     of subdevices within the parent device.  This parameter should be set
 *     to zero for default behavior.
 *   displayId
 *     This parameter specifies the displayId of HDMI resource to configure.
 *     This comes as input to this command.
 *   mute
 *     This field specifies the legal values:
 *       NV0073_CTRL_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_TRUE
 *       NV0073_CTRL_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_FALSE
 *
 *  Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_PARAM_STRUCT
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_NOT_SUPPORTED
 */
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM (0x730275U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_PARAMS_MESSAGE_ID (0x75U)

typedef struct NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_PARAMS {
    NvU8  subDeviceInstance;
    NvU32 displayId;
    NvU8  mute;
} NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_PARAMS;

#define NV0073_CTRL_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_FALSE (0x00000000U)
#define NV0073_CTRL_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_TRUE  (0x00000001U)

/*
 * NV0073_CTRL_CMD_SPECIFIC_GET_HDCP_STATE
 *
 * This command is used to obtain that state of hdcp for the specified attach
 * point (that being the displayId).
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the display for which information is to be
 *     returned.  Only one display may be indicated in this parameter.
 *     If more than one displayId is used a failing status of
 *     NV_ERR_INVALID_ARGUMENT will be returned.
 *   flags
 *     This parameter specifies the state of the attach point resource.
 *     Here are the current defined fields:
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_ENCRYPTING
 *         This comes as an output to this command.  The attach point
 *         is currently encrypting hdcp content over the attach point
 *         link.  This state of this field is determined by
 *         NV0073_CTRL_SPECIFIC_HDCP_STATE_ENCRYPTING_CACHED.  The
 *         default is to return cached hdcp state.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_ENCRYPTING_CACHED
 *         This comes as an input to this command.  If set to 1, the return
 *         value in NV0073_CTRL_SPECIFIC_HDCP_STATE_ENCRYPTING is based
 *         on the Status Word.  If the uncached hdcp state fails,
 *         such as the case for external hdcp designs that do not support
 *         Upstream Status register, then if the flag
 *         NV0073_CTRL_SPECIFIC_HDCP_STATE_ENCRYPTING_CACHED is set, RM
 *         will unclear  it and return the cached value instead.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_CAPABLE
 *         This comes as an output to this command.
 *         This bit indicates that the attach point resource is capable
 *         of hdcp encryption.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_DUAL_LINK_CAPABLE
 *         This comes as an output to this command.
 *         This bit indicates that the attach point resource is capable
 *         of hdcp encryption in a dual-link configuration.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_DISALLOWED
 *         This bit indicates that the attach point resource should not
 *         have HDCP available even if capable.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_RECEIVER_CAPABLE
 *         This comes as an output to this command.
 *         This bit indicates that the receiver attached to this attach point
 *         resource is capable of hdcp encryption.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_REPEATER_CAPABLE
 *         This comes as an output to this command.
 *         This bit indicates that the receiver attached to this attach point
 *         resource is capable of hdcp repeater operation.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_FP_INTERNAL
 *         This comes as output to this command.
 *         This bit indicates that the associated display is an HDCP-capable
 *         internal panel.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_RECEIVER_CAPABLE
 *         This comes as output to this command.
 *         This bit indicates that the receiver attached to this attach point
 *         resource is capable of hdcp22 encryption.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_REPEATER_CAPABLE
  *        This comes as output to this command.
 *         This bit indicates that the receiver attached to this attach point
 *         resource is capable of hdcp22 repeater operation.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_ENCRYPTING
 *         This comes as an output to this command.  The attach point
 *         is currently encrypting hdcp22 content over the attach point
 *         link.  This state of this field is determined by
 *         NV0073_CTRL_SPECIFIC_HDCP_STATE_ENCRYPTING_CACHED, return hdcp22 uncached
 *         status  by default.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_TYPE1
 *         This comes as an output to this command.  The attach point
 *         is currently encrypting hdcp22 content with stream Type 1 over the
 *         link.  This state of this field is determined by
 *         NV0073_CTRL_SPECIFIC_HDCP_STATE_ENCRYPTING_CACHED, return hdcp22 uncached
 *         status of stream type by default.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_AUTHENTICATED
 *         This comes an output to this command.
 *         This bit indicates if the receiver attached to this attach point
 *         completes authenticaion with source or not. To non DP-MST receiver, the state
 *         should be identical to NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_ENCRYPTING, while
 *         DP MST needs to assign ECF after authenticated.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_HDCP22_CAPABLE
 *         This comes as an output to this command.
 *         This bit indicates that the attach point resource is capable
 *         of hdcp2.2 encryption.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0073_CTRL_CMD_SPECIFIC_GET_HDCP_STATE              (0x730280U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_HDCP_STATE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_GET_HDCP_STATE_PARAMS_MESSAGE_ID (0x80U)

typedef struct NV0073_CTRL_SPECIFIC_GET_HDCP_STATE_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 flags;
} NV0073_CTRL_SPECIFIC_GET_HDCP_STATE_PARAMS;

#define NV0073_CTRL_SPECIFIC_HDCP_STATE_ENCRYPTING                       0:0
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_ENCRYPTING_NO                    (0x00000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_ENCRYPTING_YES                   (0x00000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_ENCRYPTING_CACHED                1:1
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_ENCRYPTING_CACHED_TRUE           (0x00000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_ENCRYPTING_CACHED_FALSE          (0x00000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_ENCRYPTION_INPROGRESS     2:2
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_ENCRYPTION_INPROGRESS_YES (0x00000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_ENCRYPTION_INPROGRESS_NO  (0x00000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_CAPABLE                       4:4
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_CAPABLE_NO                    (0x00000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_CAPABLE_YES                   (0x00000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_DUAL_LINK_CAPABLE             5:5
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_DUAL_LINK_CAPABLE_NO          (0x00000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_DUAL_LINK_CAPABLE_YES         (0x00000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_DISALLOWED                    6:6
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_DISALLOWED_NO                 (0x00000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_DISALLOWED_YES                (0x00000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_RECEIVER_CAPABLE                 8:8
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_RECEIVER_CAPABLE_NO              (0x00000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_RECEIVER_CAPABLE_YES             (0x00000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_REPEATER_CAPABLE                 9:9
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_REPEATER_CAPABLE_NO              (0x00000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_REPEATER_CAPABLE_YES             (0x00000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_FP_INTERNAL                      10:10
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_FP_INTERNAL_FALSE                (0x00000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_FP_INTERNAL_TRUE                 (0x00000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_RECEIVER_CAPABLE          11:11
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_RECEIVER_CAPABLE_NO       (0x00000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_RECEIVER_CAPABLE_YES      (0x00000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_REPEATER_CAPABLE          12:12
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_REPEATER_CAPABLE_NO       (0x00000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_REPEATER_CAPABLE_YES      (0x00000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_ENCRYPTING                13:13
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_ENCRYPTING_NO             (0x00000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_ENCRYPTING_YES            (0x00000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_TYPE1                     14:14
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_TYPE1_NO                  (0x00000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_HDCP22_TYPE1_YES                 (0x00000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_AUTHENTICATED                    15:15
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_AUTHENTICATED_NO                 (0x00000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_AUTHENTICATED_YES                (0x00000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_HDCP22_CAPABLE                16:16
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_HDCP22_CAPABLE_NO             (0x00000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_STATE_AP_HDCP22_CAPABLE_YES            (0x00000001U)

/*
 * NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_INFO
 *
 * This structure describes stStatus information.
 *
 *   displayId
 *     This parameter returns the displayId associated with the
 *     attach point index.
 *   S
 *     Each element contains the attach-point S.  This value's bit
 *     field contains information pertaining to STATUS of each attach point.
 *     The stStatus parameter is broken down as follows:
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_ENCRYPTING
 *         This field specifies that the attach-point is transmitting and
 *         has output encryption enabled.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_REPEATER
 *         This field specifies that the attach-point is transmitting to a
 *         repeater device.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_USER_ACCESSIBLE
 *         This field specifies that the attach-point is transmitting on a
 *         user-accessible external digital port.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_EXTERNALLY_UNPROTECTED
 *         This field specifies that the attach-point is transmitting
 *         externally and is unprotected.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_ATTACH_PORT_INDEX
 *         This field specifies the port/attach-point index.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_NUM_PORTS
 *         This field specifies the number of connectable attach-ports.
 *         The default is 8.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_INTERNAL_PANEL
 *         This field specifies a compliant internal/non-user accessible
 *         port panel without hdcp encryption.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_WIDE_SCOPE
 *         This field specifies _CS is not enough to determine the presence
 *         of non-compliant outputs (this field is always 1).
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_CS_CAPABLE
 *         This field specifies that connection-state is supported.
 *         This field is always 1.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_READZ_CAPABLE
 *         This field specifies that readZ is supported.
 *         This field is always 0.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_DUAL_LINK_EVEN
 *         This field specifies the even half of a dual-link (0x74).
 *         This field *NOT* yet supported.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_DUAL_LINK_ODD
 *         This field specifies the odd half of a dual-link (0x76)
 *         This field *NOT* yet supported.
 */
typedef struct NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_INFO {
    NvU32 displayId;
    NV_DECLARE_ALIGNED(NvU64 S, 8);
} NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_INFO;

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_ENCRYPTING                  0:0
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_REPEATER                    1:1
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_USER_ACCESSIBLE             2:2
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_EXTERNALLY_UNPROTECTED      3:3
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_ATTACH_PORT_INDEX           7:4
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_NUM_PORTS                  11:8
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_INTERNAL_PANEL            12:12
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_WIDE_SCOPE                13:13
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_CS_CAPABLE                14:14
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_READZ_CAPABLE             15:15
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_RESERVED0                 39:16
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_DUAL_LINK_EVEN            40:40
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_S_DUAL_LINK_ODD             41:41


/*
 * NV0073_CTRL_CMD_SPECIFIC_HDCP_CTRL
 *
 * This command is used to do HDCP controls on the specified attach
 * point (that being the displayId).
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the display for which information is to be
 *     returned.  Only one display may be indicated in this parameter.
 *     If more than one displayId is used a failing status of
 *     NV_ERR_INVALID_ARGUMENT will be returned.
 *   err
 *     This parameter specifies provides info regarding the outcome
 *     of this calling control call.  If zero, no errors were found.
 *     Otherwise, this parameter will specify the error detected.
 *     The valid parameter is broken down as follows:
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_UNSUCCESSFUL
 *         If set to _YES, this indicates at least one of the calling
 *         functions failed.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_PENDING
 *         If set to _YES, this indicates at renogiation is not complete and
 *         that the client should check status later.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_BAD_TOKEN_TYPE
 *         If set to _YES, the session ID or KSV was rejected.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_LINK_FAILED
 *         If set to _YES, renogiation could not complete.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_INVALID_PARAMETER
 *         If set to _YES, one or more of the calling parameters was invalid.
 *   cmd
 *     This parameter specifies a bitmask of the legal defined fields.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_NULL
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_RENEGOTIATE
 *         This command forces the specified displayId to renegotiate the
 *         hdcp link.  The client should supply as an input,
 *         cN and cKsv.  On return, bStatus, stStatus and cS is returned.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_DISABLE_AUTHENTICATION
 *         This command forces the specified displayId to off authentication the
 *         hdcp link.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_READ_LINK_STATUS
 *         This command reads the status of the cipher returning a signed
 *         S (ie: kP) and cS for the requested displayId, as well as
 *         the relevant parameters necessary for the client to verify
 *         the information provided.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_VALIDATE_LINK
 *         This command returns the parameters necessary to validiate the
 *         links for the displayId.  The client should supply as input
 *         cN and cKsv.  On return, bStatus, cS, stStatus, aN, numBksvs,
 *         bKsvList, vP, mP, and dKsv are returned.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_QUERY_HEAD_CONFIG
 *         This command enumerates ports attached to a head.
 *         On input, Cn and cKsv should be provided and on return
 *         bSTatus, status, and connection state is returned.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_ABORT_AUTHENTICATION
 *         This command causes the specified AP to abort authentication
 *         protocol after KSV list is read, or during next time it's
 *         renegotiated.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_GET_ALL_FLAGS
 *         This command provides all possible valid device data.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_GET_SPECIFIED_FLAGS
 *         This command provides data specified by flags field set by the
 *         client.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_FORWARD_PENDING_KSVLIST_READY
 *         This command provides client to tell there's pending Hdcp1X KsvList
 *         Ready notification at BStatus read.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_READ_LINK_STATUS_NO_DISPLAY
 *         This command reads the status of the cipher returning a signed
 *         S (ie: kP) and cS and the relevant parameters necessary for the client to
 *         verify upstream.
 *   flags
 *     This parameter specifies a bitmask of the legal defined fields and the
 *       reason AbortAuthentication.
 *     The client shall set the desired fields and on return if valid,
 *     the resource manager will set which flags are actually valid.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BCAPS_PRESENT
 *         IN:  Request hdcp receiver bcaps register state.
 *         OUT: Bcaps parameter contains valid receiver bcaps register data.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BSTATUS_PRESENT
 *         IN:  Request hdcp receiver bstatus register state.
 *         OUT: BStatus parameter contains valid receiver bstatus register data
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BKSV_PRESENT
 *         IN:  Request hdcp receiver key selection vector: BKSV
 *              Driver will read BKSV from receiver and update RM states if
 *              the cmd is _GET_SPECIFIED_FLAGS and _KP_PRESENT is unset.
 *              Otherwise, driver returns cached BKSV.
 *         OUT: Bksv parameter contains valid receiver bksv data.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BKSV_LIST_PRESENT
 *         IN:  Request list of downstream BKSV from repeater
 *         OUT: BksvList parameter contains valid data.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_DKSV_PRESENT
 *         IN:  Request hdcp transmitter key selection vector: DKSV
 *         OUT: Dksv parameter contains valid receiver DKSV data.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_AN_PRESENT
 *         IN:  Request hdcp parameter An
 *         OUT: An parameter contains valid data
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_AKSV_PRESENT
 *         IN:  Request hdcp transmitter downstream key selection vector: AKSV
 *         OUT: Aksv parameter contains valid receiver Aksv data.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_VP_PRESENT
 *         IN:  Request VPrime data
 *         OUT: VPrime parameter contains valid data
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_MP_PRESENT
 *         IN:  Request MPrime data used for repeater authentication
 *         OUT: MPrime parameter contains valid data
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_KP_PRESENT
 *         IN:  Request SPrime data
 *         OUT: SPrime parameter contains valid data
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_IMPLICIT_HEAD
 *         IN:  The head to use if no legal head could be located.
 *              Use NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_IMPLICIT_HEAD_NONE
 *              if no implicit head should be used.
 *         OUT: n/a
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_FORCE_REAUTH
 *         IN:  Request to execute authentication protocol even encryption
 *              enabled.
 *         OUT: n/a
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_RXIDMSG_PENDING
 *         IN:  Request to execute repeater authentication protocol with pending
 *              ID List message information.
 *         OUT: n/a
 *     Reason of AbortAuthentication.
 *     When pass in by client, it indicates the reason why client issue an
 *     Abort.  When return by RM, it indicates the reason of last successful
 *     Abort.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_NONE
 *         Default value
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_UNTRUST
 *         Abort due to Kp mismatch
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_UNRELBL
 *         Abort due to repeated link failure
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_KSV_LEN
 *         Abort due to KSV length
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_KSV_SIG
 *         Abort due to KSV signature
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_SRM_SIG
 *         Abort due to SRM signature
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_SRM_REV
 *         Abort due to SRM revocation
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_NORDY
 *         Abort due to repeater not ready
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_KSVTOP
 *         Abort due to KSV topology error
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_BADBKSV
 *         Abort due to invalid Bksv
 *   linkCount
 *     This parameter specifies how many links are valid.  This is important
 *     when determining AKSV, BKSV, AN, etc... and is an output to this
 *     command.
 *   apIndex
 *     Each element of this parameter specifies the hardware attach-point index
 *     for the requested displayId.  The 2nd element is only valid in the case
 *     where the resource output is capable of dual-link determined when the
 *     linkCount is greater than 1.
 *   cN
 *     This parameter is the input value Cn a 64 bit random number
 *     to be provided to this command. Cn value is Upstream protocol's
 *     exchange random number.This value must be written by software
 *     before the KSV is written if the transmitter device follows the HDCP
 *     upstream protocol.
 *   cKsv
 *     This parameter is the input value Cksv (a unique identifier) of 40 bit
 *     size to be provided to this command. This input value shall contain 20
 *     ones and 20 zeros in accordance with the HDCP specification. This value
 *     must be written by software before the KSV is written if the transmitter
 *     device follows the HDCP upstream protocol.
 *   aN
 *     Each element of this buffer specifies the output value aN,
 *     a 64 bit random number used during hdcp authentication and validating
 *     the upstream link in which case only the first 40 bits are used.
 *     The 2nd element is only valid in the case where the resource output is
 *     capable of dual-link determined when the linkCount is greater than 1.
 *   aKsv
 *     Each element of this buffer specifies output value aKsv of 40 bit size.
 *     As per the HDCP specification this value should contain 20 ones and
 *     20 zeros.
 *     The 2nd element is only valid in the case where the resource output is
 *     capable of dual-link determined when the linkCount is greater than 1.
 *   bStatus
 *     Each element contains the attach-point bStatus data returned by the
 *     repeater/receiver device (if valid).  The bStatus value is an output
 *     by this command. For HDCP on DP, bInfo is the one we should look at.
 *     bInfo is defined exactly the same with bStatus on DVI. Hal is taking
 *     care of the difference here.
 *     This bStatus info is broken down as follows:
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_BSTATUS_DEVICE_COUNT
 *         Specifies the total number of receivers excluding repeater.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_BSTATUS_MAX_DEVICES_EXCEEDED
 *         Specifies a topology error in which greater than 127 devices are
 *         detected in the overall hdcp configuration.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_BSTATUS_REPEATER_DEPTH
 *         Specifies the repeater depth.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_BSTATUS_MAX_CASCADE_EXCEEDED
 *         Specifies a topology error in which greater than 7 levels are
 *         detected in the overall hdcp configuration.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDMI_MODE
 *         Specifies that the hdcp receiver is in HDMI mode.
 *   bCaps
 *     The BCAPS value is an output by this command.  This value's bit fields
 *     contains information returned by receiver device. Bcaps can be used
 *     to determine if receiver is a repeater and when the ksvlist and vprime
 *     data is ready.
 *     The BCAPS is defined different in the spec of HDCP on DP. It's been
 *     split to BCAPS and BSTATUS. Here we'll end a flag to indicate
 *     the client if it's a DP.
 *     The bCaps parameter is broken down as follows based on HDCP spec 1.1:
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_FAST_REAUTHENTICATION
 *         This field when set to 1, specifies the receiver is capable of
 *         receiving(unencrypted) video signal during session
 *         re-authentication.  All HDMI capable receivers shall be capable of
 *         performing the fast authentication even if this bit is not set.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_EESS_1_1
 *         This field when set to 1, specifies this HDCP receiver supports
 *         Enhanced Encryption Status Signaling (EESS), Advance cipher, and
 *         Enhanced Link Verification options.  For the HDMI protocol, EESS
 *         capability is assumed regardless of this bit setting.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_READY_KSV_FIFO
 *         This field when set to 1, specifies this HDCP repeater has built
 *         the list of attached KSVs and computed the verification value V'.
 *         This value is always zero during the computation of V'.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_FAST
 *         This field when set to 1, specifies this device supports 400khz
 *         transfers.  When zero, 100 khz is the maximum transfer rate
 *         supported.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_REPEATER
 *         This field when set to 1, specifies the HDCP receiver supports
 *         downstream connections as permitted by Digital Content
 *         Protection LLC licence.  This bit does not change while the HDCP
 *         receiver is active.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_HDMI_RESERVED
 *         This field is reserved and HDCP receivers not capable of
 *         support HDMI must clear this bit to 0.
 *   stStatus
 *     This parameter specifies the attach point stStatus. See
 *     the description of NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_INFO for
 *     details on stStatus information.
 *   cS
 *     This parameter provides the connection-state for the status of
 *     all port/attach-points on this head.
 *     The connection-state is broken down as follows:
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_ATTACH_POINTS
 *         This field specifies the transmitting attach-points.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_NON_HDCP
 *         This field specifies the transmitting attach-points.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_HEAD_INDEX
 *         This field specifies the index of the head.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_RFUPLANES
 *         This field specifies the RFUPLANES.
 *         This field *NOT* yet supported.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_NUM_ACTIVE_HEADS
 *         This field specifies the number of heads - 1.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_ATTACH_PLANES
 *         This field specifies attach planes.
 *         This field *NOT* yet supported.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_ENCRYPTING
 *         This field is supported on chips with Display IP 0501 and later.
 *       NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_SPAN_MODE
 *         This field specifies dual-display span mode.
 *         This field *NOT* yet supported.
 *   bKsvList
 *     In case downstream device is repeater then this will give the list of
 *     KSV's of all downstream devices attached to the repeater. It differs
 *     from actualKsvSize because this allocates maximum allowed size.
 *     If downstream device is receiver then this array will contain all zeros.
 *   numBksvs
 *     Total number of Bksv from all downstream devices in the bKsvList.
 *     This info can also be obtained via bStatus.
 *   vP
 *     The VPRIME value returned as output by this command from the repeater
 *     device. This value should be used to compare the verification value
 *     during the HDCP upstream protocol using SHA1 in accordance with the
 *     upstream protocol. This value can be ignored if bAuthRequired is not
 *     set indicating the verification is done by the transmitter device.
 *   kP
 *     The KP value is returned as an output by this command.  This
 *     parameter is the signature computed by hardware and the client
 *     should compute the signature to compare this value.
 *     The 2nd element is only valid in the case where the resource output is
 *     capable of dual-link determined when the linkCount is greater than 1.
 *   mP
 *     The MPRIME value returned as output by this command.
 *     MPrime shall be decrypted by the client and used in the SHA-1
 *     computation of V during upstream authentication. This value can be
 *     ignored if bAuthRequired is not set indicating the verification is
 *     done by the transmitter device.
 *   dKsv
 *     Each element of this buffer is the output value DKSV of 40 bit size.
 *     As per the HDCP specification this value should contain 20 ones and
 *     20 zeros.
 *     The 2nd element is only valid in the case where the resource output is
 *     capable of dual-link determined when the linkCount is greater than 1.
 *   streamIndex
 *     Each content stream is assigned an index value bye upstream client.
 *     HDMI: The index must be 0.
 *     DP SST: The index must be 0.
 *     DP MST: Assigned stream index.
 *   streamType
 *     Each content stream is assigned a type value by the upstream content
 *     control function.
 *     0x00: Type 0 content stream. May be transimtted by the HDCP repeater to
 *           all HDCP devices.
 *     0x01: Type 1 content stream. Must not be transmitted by the HDCP
 *           repeater to HDCP 1.x-compliant devices and HDCP 2.0-compliant
 *           repeaters.
 *     0x02-0xFF: reserved for futures use only.
 *   bEnforceType0Hdcp1xDS
 *     If this bit is set, DPU enforces Type0 if it finds Hdcp1x monitor downstream
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0073_CTRL_CMD_SPECIFIC_HDCP_CTRL (0x730282U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_HDCP_CTRL_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_HDCP_LINK_COUNT        (0x0000002U)
#define NV0073_CTRL_HDCP_VPRIME_SIZE       (0x0000014U)
#define NV0073_CTRL_HDCP_MAX_DEVICE_COUNT  (0x00000FFU)
#define NV0073_CTRL_HDCP_KSV_SIZE          (0x0000005U)
#define NV0073_CTRL_HDCP_MAX_NUM_APS       (0x0000010U)

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_PARAMS_MESSAGE_ID (0x82U)

typedef struct NV0073_CTRL_SPECIFIC_HDCP_CTRL_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;

    NvU32  err;

    NvU32  cmd;
    NvU32  flags;
    NvU8   linkCount;

    NV_DECLARE_ALIGNED(NvU64 cN, 8);
    NV_DECLARE_ALIGNED(NvU64 cKsv, 8);

    NvU32  apIndex[NV0073_CTRL_HDCP_LINK_COUNT];

    NV_DECLARE_ALIGNED(NvU64 aN[NV0073_CTRL_HDCP_LINK_COUNT], 8);
    NV_DECLARE_ALIGNED(NvU64 aKsv[NV0073_CTRL_HDCP_LINK_COUNT], 8);

    NvU32  bStatus[NV0073_CTRL_HDCP_MAX_NUM_APS];
    NvU32  bCaps[NV0073_CTRL_HDCP_MAX_NUM_APS];

    NV_DECLARE_ALIGNED(NV0073_CTRL_SPECIFIC_HDCP_CTRL_STATUS_INFO stStatus[NV0073_CTRL_HDCP_MAX_NUM_APS], 8);

    NV_DECLARE_ALIGNED(NvU64 cS, 8);

    NV_DECLARE_ALIGNED(NvU64 bKsv[NV0073_CTRL_HDCP_LINK_COUNT], 8);
    NV_DECLARE_ALIGNED(NvU64 bKsvList[NV0073_CTRL_HDCP_MAX_DEVICE_COUNT], 8);
    NvU32  numBksvs;

    NvU8   vP[NV0073_CTRL_HDCP_VPRIME_SIZE];
    NV_DECLARE_ALIGNED(NvU64 kP[NV0073_CTRL_HDCP_LINK_COUNT], 8);

    NV_DECLARE_ALIGNED(NvU64 mP, 8);
    NV_DECLARE_ALIGNED(NvU64 dKsv[NV0073_CTRL_HDCP_LINK_COUNT], 8);

    NvU32  streamIndex;
    NvU8   streamType;
    NvBool bEnforceType0Hdcp1xDS;

    NvBool bPendingKsvListReady;

    NvBool isHdcpCapable;
    NvBool isHdcpAuthOn;
    NvBool isHdcpRp;
    NvBool isHdcp2X;
    NvBool bMaxCascadeExceeded;
    NvBool bMaxDeviceExceeded;
    NvBool bHdcp1DevDownstream;
    NvBool bHdcp2LegacyDevDownstream;
    NvU8   cascadeDepth;
} NV0073_CTRL_SPECIFIC_HDCP_CTRL_PARAMS;

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_UNSUCCESSFUL             0:0
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_UNSUCCESSFUL_NO               (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_UNSUCCESSFUL_YES              (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_PENDING                  1:1
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_PENDING_NO                    (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_PENDING_YES                   (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_BAD_TOKEN_TYPE           2:2
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_BAD_TOKEN_TYPE_NO             (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_BAD_TOKEN_TYPE_YES            (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_LINK_FAILED              3:3
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_LINK_FAILED_NO                (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_LINK_FAILED_YES               (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_INVALID_PARAMETER        4:4
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_INVALID_PARAMETER_NO          (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_ERR_INVALID_PARAMETER_YES         (0x0000001U)

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD                          31:0
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_NULL                          (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_READ_LINK_STATUS              (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_VALIDATE_LINK                 (0x0000002U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_QUERY_HEAD_CONFIG             (0x0000003U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_RENEGOTIATE                   (0x0000004U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_GET_ALL_FLAGS                 (0x0000005U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_GET_SPECIFIED_FLAGS           (0x0000006U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_ABORT_AUTHENTICATION          (0x0000007U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_DISABLE_AUTHENTICATION        (0x0000008U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_SET_TYPE                      (0x0000009U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_FORWARD_KSVLIST_READY         (0x000000AU)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_READ_LINK_STATUS_NO_DISPLAY   (0x000000BU)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CMD_READ_TOPOLOGY                 (0x000000CU)

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BCAPS_PRESENT          0:0
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BCAPS_PRESENT_NO            (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BCAPS_PRESENT_YES           (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BSTATUS_PRESENT        1:1
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BSTATUS_PRESENT_NO          (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BSTATUS_PRESENT_YES         (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BKSV_PRESENT           2:2
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BKSV_PRESENT_NO             (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BKSV_PRESENT_YES            (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BKSV_S_PRESENT         3:3
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BKSV_S_PRESENT_NO           (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BKSV_S_PRESENT_YES          (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BKSV_LIST_PRESENT      4:4
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BKSV_LIST_PRESENT_NO        (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_BKSV_LIST_PRESENT_YES       (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_AN_PRESENT             5:5
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_AN_PRESENT_NO               (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_AN_PRESENT_YES              (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_AN_S_PRESENT           6:6
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_AN_S_PRESENT_NO             (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_AN_S_PRESENT_YES            (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_AKSV_PRESENT           7:7
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_AKSV_PRESENT_NO             (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_AKSV_PRESENT_YES            (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_AKSV_S_PRESENT         8:8
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_AKSV_S_PRESENT_NO           (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_AKSV_S_PRESENT_YES          (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_DKSV_PRESENT           9:9
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_DKSV_PRESENT_NO             (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_DKSV_PRESENT_YES            (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_DKSV_S_PRESENT         10:10
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_DKSV_S_PRESENT_NO           (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_DKSV_S_PRESENT_YES          (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_MP_PRESENT             11:11
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_MP_PRESENT_NO               (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_MP_PRESENT_YES              (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_VP_PRESENT             12:12
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_VP_PRESENT_NO               (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_VP_PRESENT_YES              (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_CN_PRESENT             13:13
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_CN_PRESENT_NO               (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_CN_PRESENT_YES              (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_CKSV_PRESENT           14:14
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_CKSV_PRESENT_NO             (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_CKSV_PRESENT_YES            (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_KP_PRESENT             15:15
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_KP_PRESENT_NO               (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_KP_PRESENT_YES              (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_KP_S_PRESENT           16:16
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_KP_S_PRESENT_NO             (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_KP_S_PRESENT_YES            (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_STATUS_PRESENT         17:17
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_STATUS_PRESENT_NO           (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_STATUS_PRESENT_YES          (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_CS_PRESENT             18:18
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_CS_PRESENT_NO               (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_CS_PRESENT_YES              (0x0000001U)

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT                  22:19
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_NONE                  (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_UNTRUST               (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_UNRELBL               (0x0000002U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_KSV_LEN               (0x0000003U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_KSV_SIG               (0x0000004U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_SRM_SIG               (0x0000005U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_SRM_REV               (0x0000006U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_NORDY                 (0x0000007U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_KSVTOP                (0x0000008U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_ABORT_BADBKSV               (0x0000009U)

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_IMPLICIT_HEAD          25:23
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_IMPLICIT_HEAD_NONE          (0x0000007U)

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_TYPE_CHANGED           26:26
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_TYPE_CHANGED_NO             (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_TYPE_CHANGED_YES            (0x0000001U)

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_FORCE_REAUTH           27:27
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_FORCE_REAUTH_NO             (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_FORCE_REAUTH_YES            (0x0000001U)

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_RXIDMSG_PENDING        28:28
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_RXIDMSG_PENDING_NO          (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_FLAGS_RXIDMSG_PENDING_YES         (0x0000001U)

/* BCaps definition of HDCP over TMDS */
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_FAST_REAUTHENTICATION  0:0
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_EESS_1_1               1:1
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_FAST                   4:4
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_READY_KSV_FIFO         5:5
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_REPEATER               6:6
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_HDMI_RESERVED          7:7
/* BCaps definition of HDCP over DP */
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_DP_HDCP_CAPABLE        0:0
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_DP_REPEATER            1:1
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_DP_READY_KSV_FIFO      2:2
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_DP_R0_AVAILABLE        3:3
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_DP_LINK_INTEGRITY_FAILURE 4:4

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BSTATUS_DEVICE_COUNT         6:0
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BSTATUS_MAX_DEVICES_EXCEEDED 7:7
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BSTATUS_REPEATER_DEPTH       10:8
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BSTATUS_MAX_CASCADE_EXCEEDED 11:11
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDMI_MODE                    12:12
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_RESERVED_0                   31:13

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_ATTACH_POINTS             15:0
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_NON_HDCP                  16:16
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_HEAD_INDEX                20:17
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_RFUPLANES                 28:21
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_NUM_ACTIVE_HEADS          30:29
// Bit 39-29 are implementation dependent connection state information
// for HDCP22 from gm206 (v02_06) onwards Bit-30 says HDCP22 encryption status.
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_HDCP22_ENCRYPTION         30:30
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_HDCP22_ENCRYPTION_YES          0x00000001U
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_HDCP22_ENCRYPTION_NO           0x00000000U
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_HDCP22_TYPE1              31:31
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_HDCP22_TYPE1_YES               0x00000001U
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_HDCP22_TYPE1_NO                0x00000000U
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_RESERVED_0                39:32
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_ATTACH_PLANES             47:40
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_ENCRYPTING                48:48
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_CS_SPAN_MODE                 49:49

/* This HDCP_MODE definition applies to both DP and TMDS */
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_HDCP_MODE             15:15
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_HDCP_MODE_TMDS              (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_HDCP_MODE_DP                (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_HDCP_VERSION          23:16
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_HDCP_VERSION_1X             (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_BCAPS_HDCP_VERSION_22             (0x0000022U)

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BCAPS_REPEATER            0:0
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BCAPS_REPEATER_NO          (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BCAPS_REPEATER_YES         (0x0000001U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BCAPS_HDCP_CAPABLE        1:1
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BCAPS_HDCP_CAPABLE_NO      (0x0000000U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BCAPS_HDCP_CAPABLE_YES     (0x0000001U)

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BSTATUS_DP_READY                     0:0
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BSTATUS_DP_HPRIME_AVAILABLE          1:1
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BSTATUS_DP_PAIRING_AVAILABLE         2:2
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BSTATUS_DP_REAUTH_REQ                3:3
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BSTATUS_DP_LINK_INTEGRITY_FAILURE    4:4
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BSTATUS_HDCP1_REPEATER_DOWNSTREAM    5:5
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BSTATUS_HDCP2_0_REPEATER_DOWNSTREAM  6:6
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BSTATUS_MAX_CASCADE_EXCEEDED         7:7
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BSTATUS_MAX_DEVS_EXCEEDED            8:8
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BSTATUS_DEVICE_COUNT                 13:9
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_BSTATUS_REPEATER_DEPTH               16:14

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_TYPE_0                     (0x00U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_TYPE_1                     (0x01U)

#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_ENFORCE_TYPE0_HDCP1XDS_NO  (0x00U)
#define NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_ENFORCE_TYPE0_HDCP1XDS_YES (0x01U)



/*
 * NV0073_CTRL_CMD_SPECIFIC_GET_HDCP_DIAGNOSTICS
 *
 * This command is used to obtain diagnostic info, useful when hdcp
 * fails for the specified attach point (that being the displayId).
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the display for which information is to be
 *     returned.  Only one display may be indicated in this parameter.
 *     If more than one displayId is used a failing status of
 *     NV_ERR_INVALID_ARGUMENT will be returned.
 *   flags
 *     This parameter specifies the diagnostics obtained from the attach point
 *     resource.  Here are the current defined fields:
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_ROM_ERROR
 *         The hdcp hardware detected an error with the rom.  Possible
 *         causes are that a rom is not present or if present, the hardware
 *         is not able to access the rom.
 *       NV0073_CTRL_SPECIFIC_HDCP_DIAGNOSTICS_BOND_NOT_ENABLED
 *         The hdcp fuse register has not been set.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_AKSV_INVALID
 *         If the AKSV (key selection vector) of the hardware does not return
 *         20 1s and 0s, this bit will be set.  This is an indication that
 *         the ROM is not programmed correctly and may need to be corrected
 *         by replacing the external hdcp cryptorom.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_BKSV_INVALID
 *         If the BKSV (key selection vector) of the display receiver hardware
 *         does not return 20 1s and 0s, this bit will be set.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_DKSV_INVALID
 *         If the DKSV (key selection vector) of the upstream hdcp hardware
 *         does not return 20 1s and 0s, this bit will be set.
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_DUAL_LINK_INUSE
 *         This bit is set if the attach point is currently outputting dual-link
 *       NV0073_CTRL_SPECIFIC_HDCP_STATE_DOWNSTREAM_CHECKSUM_FAILED
 *         This bit is set if hardware reports that its checksum BIST of its
 *         downstream HDCP keys failed.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDCP_DIAGNOSTICS                        (0x730281U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_HDCP_DIAGNOSTICS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_GET_HDCP_DIAGNOSTICS_PARAMS_MESSAGE_ID (0x81U)

typedef struct NV0073_CTRL_SPECIFIC_GET_HDCP_DIAGNOSTICS_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 flags;
} NV0073_CTRL_SPECIFIC_GET_HDCP_DIAGNOSTICS_PARAMS;

#define NV0073_CTRL_SPECIFIC_HDCP_DIAGNOSTICS_ROM_ERROR                     0:0
#define NV0073_CTRL_SPECIFIC_HDCP_DIAGNOSTICS_BOND_NOT_ENABLED              1:1
#define NV0073_CTRL_SPECIFIC_HDCP_DIAGNOSTICS_AKSV_INVALID                  2:2
#define NV0073_CTRL_SPECIFIC_HDCP_DIAGNOSTICS_BKSV_INVALID                  3:3
#define NV0073_CTRL_SPECIFIC_HDCP_DIAGNOSTICS_DKSV_INVALID                  4:4
#define NV0073_CTRL_SPECIFIC_HDCP_DIAGNOSTICS_DUAL_LINK_INUSE               5:5
#define NV0073_CTRL_SPECIFIC_HDCP_DIAGNOSTICS_DOWNSTREAM_CHECKSUM_FAILED    6:6

/*
 * NV0073_CTRL_SPECIFIC_ACPI_ID_MAPPING
 *
 * This structure defines the mapping between the ACPI ID and the corresponding
 * display ID of a display device
 *
 *     subDeviceInstance
 *       This parameter specifies the subdevice instance within the
 *       NV04_DISPLAY_COMMON parent device to which the operation should be
 *       directed. This parameter must specify a value between zero and the
 *       total number of subdevices within the parent device.  This parameter
 *       should be set to zero for default behavior.
 *     acpiId
 *       The ACPI ID of the display device
 *     displayId
 *       The corresponding display ID
 *     dodIndex
 *       The corresponding DOD index
 */
typedef struct NV0073_CTRL_SPECIFIC_ACPI_ID_MAPPING {
    NvU32 subDeviceInstance;
    NvU32 acpiId;
    NvU32 displayId;
    NvU32 dodIndex;
} NV0073_CTRL_SPECIFIC_ACPI_ID_MAPPING;

/*
 * NV0073_CTRL_CMD_GET_ACPI_DOD_DISPLAY_PORT_ATTACHMENT
 *
 * This call will return the Display Port Attachment value
 * per displayID as defined by Nvidia that is directly
 * associated with the ACPI 3.0 _DOD entry's Display Port
 * Attachment field.  This should help clients map the
 * _DOD ACPI ID to each displayID. Note, that some systems
 * do not have a standard in place for this field.  On those
 * systems, the RM will return NV_ERR_NOT_SUPPORTED.
 *
 * Note that this "Display Port" attachment field has nothing
 * to do with DisplayPort/DP.  It's an unfortunate name inside
 * the ACPI 3.0 spec that coincides with the name of DisplayPort.
 *
 *
 * Possible status values returned are:
 * NV_OK
 * NV_ERR_INVALID_PARAM_STRUCT
 * NV_ERR_NOT_SUPPORTED
 *
*/

#define NV0073_CTRL_GET_ACPI_DOD_DISPLAY_PORT_ATTACHMENT_PARAMS_MESSAGE_ID (0x85U)

typedef struct NV0073_CTRL_GET_ACPI_DOD_DISPLAY_PORT_ATTACHMENT_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 dispPortAttachment;
} NV0073_CTRL_GET_ACPI_DOD_DISPLAY_PORT_ATTACHMENT_PARAMS;


#define NV0073_CTRL_CMD_SPECIFIC_GET_ACPI_DOD_DISPLAY_PORT_ATTACHMENT (0x730285U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_GET_ACPI_DOD_DISPLAY_PORT_ATTACHMENT_PARAMS_MESSAGE_ID" */

// defines for dispPortAttachment
#define NV0073_DISPLAY_PORT_ATTACHMENT_ANALOG                         (0x00000000U)
#define NV0073_DISPLAY_PORT_ATTACHMENT_LVDS                           (0x00000001U)
#define NV0073_DISPLAY_PORT_ATTACHMENT_DP_A                           (0x00000002U)
#define NV0073_DISPLAY_PORT_ATTACHMENT_DP_B                           (0x00000003U)
#define NV0073_DISPLAY_PORT_ATTACHMENT_DP_C                           (0x00000004U)
#define NV0073_DISPLAY_PORT_ATTACHMENT_DP_D                           (0x00000005U)
#define NV0073_DISPLAY_PORT_ATTACHMENT_UNKNOWN                        (0xFFFFFFFFU)

/*
 * NV0073_CTRL_CMD_SPECIFIC_SET_ACPI_ID_MAPPING
 *
 * This call will update the RM data structure which holds the
 * ACPI ID to display ID mapping of the display devices
 *
 * The input parameter is an array of structures of type
 * NV0073_CTRL_SPECIFIC_ACPI_ID_MAPPING
 *
 * If some of the array elements remain unused, the acpiId field of the
 * structure must be set to 0x0000
 *
 * The size of the array is given by
 * NV0073_CTRL_SPECIFIC_MAX_ACPI_DEVICES (defined below)
 *
 * Possible status values returned are:
 * NV_OK
 * NV_ERR_INVALID_PARAM_STRUCT
 *
*/
#define NV0073_CTRL_CMD_SPECIFIC_SET_ACPI_ID_MAPPING                  (0x730284U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_SET_ACPI_ID_MAPPING_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_MAX_ACPI_DEVICES                         16U

#define NV0073_CTRL_SPECIFIC_SET_ACPI_ID_MAPPING_PARAMS_MESSAGE_ID (0x84U)

typedef struct NV0073_CTRL_SPECIFIC_SET_ACPI_ID_MAPPING_PARAMS {
    NV0073_CTRL_SPECIFIC_ACPI_ID_MAPPING mapTable[NV0073_CTRL_SPECIFIC_MAX_ACPI_DEVICES];
} NV0073_CTRL_SPECIFIC_SET_ACPI_ID_MAPPING_PARAMS;

/*
 * NV0073_CTRL_CMD_SPECIFIC_GET_ALL_HEAD_MASK
 *
 * This call will return all head mask.
 *
 *     subDeviceInstance
 *       This parameter specifies the subdevice instance within the
 *       NV04_DISPLAY_COMMON parent device to which the operation should be
 *       directed. This parameter must specify a value between zero and the
 *       total number of subdevices within the parent device. This parameter
 *       should be set to zero for default behavior.
 *     headMask
 *       headMask is the mask of all heads that are usable. For example, if
 *       head 0 and head 2 are present, headMask would be NVBIT(0)|NVBIT(2). This
 *       parameter returns to the client.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV0073_CTRL_CMD_SPECIFIC_GET_ALL_HEAD_MASK (0x730287U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_ALL_HEAD_MASK_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_GET_ALL_HEAD_MASK_PARAMS_MESSAGE_ID (0x87U)

typedef struct NV0073_CTRL_SPECIFIC_GET_ALL_HEAD_MASK_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 headMask;
} NV0073_CTRL_SPECIFIC_GET_ALL_HEAD_MASK_PARAMS;

/*
 * NV0073_CTRL_CMD_SPECIFIC_SET_OD_PACKET
 *
 * This command is used to program the display output packets.
 * Currently it supports DP and HDMI.
 * Common supported packets are AVI infoframes, Audio Infoframes, Gamma
 * Metadata, Vendor Specific infoframes and General Control Packets (GCP).
 * + GCP AVMute Enable should be performed before the start of the modeset.
 * + GCP AVMute Disable should be performed after the end of the modeset.
 *   GCP AVMute should contain HDR + 7 bytes.
 * + AVI infoframes should occur after the modeset but before a GCP AVMute
 *   Disable.  AVI infoframe should contain HDR + 14 bytes
 * + Audio infoframes should occur after the modeset but before a GCP AVMute
 *   Enable.
 *   Audio infoframe should contain HDR + 11 bytes.
 * + Gamma Metadata packets should contain HDR + 28 bytes.
 * + Vendor Specific packets are variable length.
 *   By HDMI 1.4 June 5 2009 spec, payload can be 5 bytes, 6 bytes, 7 bytes or
 *   16 bytes, depending on the packets spec.
 * Unused data bytes should be zero-ed out.
 *
 *   displayID
 *     This parameter specifies the displayID for the display resource to
 *     configure.
 *     This comes as input to this command.
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *     This parameter must specify a value between zero and the total number
 *     of subdevices within the parent device.  This parameter should be set
 *     to zero for default behavior.
 *   transmitControl
 *     This parameter controls how the packet is to be sent by setting the
 *     control bits.
 *     Possible flags are as follows:
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_ENABLE
 *         Set to _ENABLE to start sending the packet at next frame, set to
 *         _DISABLE to stop sending.
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_OTHER_FRAME
 *         Set to _ENABLE to send the packet at other frame, set to _DISABLE to
 *         send at every frame.
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SINGLE_FRAME
 *         Set to _ENABLE to send once next frame, set to _DISABLE to send at
 *         every frame.
 *       Note: A setting to set both _OTHER_FRAME and _SINGLE_FRAME is invalid
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_ON_HBLANK
 *         Set to _ENABLE to send the packet once on next HBLANK, set to
 *         _DISABLE to send on VBLANK.
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_IMMEDIATE
 *         Set to _ENABLE to send the info frame packet as soon as possible.
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_VIDEO_FMT
 *         Set to _SW_CONTROLLED to set HDMI_Video_Format field and 3D_Structure field
 *         from NV_PDISP_SF_HDMI_VSI_SUBPACK0_HIGH_PB4 and PB5, if it is set to _HW_CONTROLLED
 *         then HW will get them based on the state of the setHdmiCtrl method.
 *         Btw this applies only for stereo ovverides.
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SET_STEREO_POLARITY
 *         Set to TRUE to send Vendor specific info frame used for 3D stereo LR sync.
 *         Set PACKET_TYPE=pktType_VendorSpecInfoFrame along with this flag.
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SET_SELF_REFRESH_SETTING
 *         Set to TRUE to send Vendor specific info frame used for Self Refresh panels
 *         Set PACKET_TYPE=pktType_VendorSpecInfoFrame along with this flag.
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_GEN_INFOFRAME_MODE
 *         HW provides support to program 2 generic infoframes per frame for DP with GP10X+.
 *         This flag indicates the INFOFRAME that needs to be programmed.
 *         Set to _INFOFRAME0 if RM should program GENERIC_INFOFRAME
 *         Set to _INFOFRAME1 if RM should program GENERIC_INFOFRAME1
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_RESERVED_LEGACY_MODE
 *         This option is reserved for backward compatibility with
 *         NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_PACKET and
 *         NV0073_CTRL_CMD_DP_SET_PACKET.
 *         It is temporal and will be deprecated soon.
 *   packetSize
 *     packet size of packets in pPacket to send, including header and payload.
 *   targetHead
 *     Specifies the target head number for which SDP needs to be updated.
 *   bUsePsrHeadforSdp
 *     Indicates use targetHead field for setting SDP or infoframe packet instead
 *     of deriving the active head from displayID.
 *   pPacket
 *     pPacket points to the packets to send.
 *     For HDMI 1.1, the maximum allowed bytes is 31.
 *     The packet array includes the 3 bytes of header + data depending on
 *     the type of packet.  For an infoframe, the header bytes refer to type,
 *     version and length respectively.  This comes as input to this command.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SPECIFIC_SET_OD_PACKET (0x730288U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SET_OD_MAX_PACKET_SIZE     36U

#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS_MESSAGE_ID (0x88U)

typedef struct NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvU32  transmitControl;
    NvU32  packetSize;
    NvU32  targetHead;
    NvBool bUsePsrHeadforSdp;
    NvU8   aPacket[NV0073_CTRL_SET_OD_MAX_PACKET_SIZE];
} NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS;

#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_ENABLE                                     0:0
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_ENABLE_NO                      (0x0000000U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_ENABLE_YES                     (0x0000001U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_OTHER_FRAME                                1:1
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_OTHER_FRAME_DISABLE            (0x0000000U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_OTHER_FRAME_ENABLE             (0x0000001U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SINGLE_FRAME                               2:2
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SINGLE_FRAME_DISABLE           (0x0000000U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SINGLE_FRAME_ENABLE            (0x0000001U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_ON_HBLANK                                 3:3
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_ON_HBLANK_DISABLE              (0x0000000U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_ON_HBLANK_ENABLE               (0x0000001U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_IMMEDIATE                                  4:4
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_IMMEDIATE_DISABLE              (0x0000000U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_IMMEDIATE_ENABLE               (0x0000001U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_VIDEO_FMT                                  5:5
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_VIDEO_FMT_SW_CONTROLLED        (0x0000000U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_VIDEO_FMT_HW_CONTROLLED        (0x0000001U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SET_STEREO_POLARITY                        6:6
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SET_STEREO_POLARITY_FALSE      (0x0000000U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SET_STEREO_POLARITY_TRUE       (0x0000001U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SET_SELF_REFRESH_SETTING                   7:7
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SET_SELF_REFRESH_SETTING_FALSE (0x0000000U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SET_SELF_REFRESH_SETTING_TRUE  (0x0000001U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_GEN_INFOFRAME_MODE                         9:8
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_GEN_INFOFRAME_MODE_INFOFRAME0  (0x0000000U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_GEN_INFOFRAME_MODE_INFOFRAME1  (0x0000001U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_RESERVED_LEGACY_MODE                     31:31
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_RESERVED_LEGACY_MODE_NO        (0x0000000U)
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_RESERVED_LEGACY_MODE_YES       (0x0000001U)


/*
 * NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_PARAMS
 *
 * This command is used to enable/disable sending of display output packets.
 * Currently it supports HDMI only.
 * Unused data bytes should be zero-ed out.
 *
 *   displayID
 *     This parameter specifies the displayID for the display output resource to
 *     configure.
 *     This comes as input to this command.
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be directed.
 *     This parameter must specify a value between zero and the total number
 *     of subdevices within the parent device.  This parameter should be set
 *     to zero for default behavior.
 *   type
 *     The client shall specify the type of display output packet. For HDMI, set
 *     this according to HDMI specification 1.4.
 *     This comes as input to this command.
 *   transmitControl
 *     This parameter controls how the packet is to be sent by setting the control
 *     bits.
 *     Possible flags are as follows:
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_ENABLE
 *         Set to _ENABLE to start sending the packet at next frame, set to
 *         _DISABLE to stop sending.
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_OTHER_FRAME
 *         Set to _ENABLE to send the packet at other frame, set to _DISABLE to
 *         send at every frame.
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_SINGLE_FRAME
 *         Set to _ENABLE to send once next frame, set to _DISABLE to send at
 *         every frame.
 *       Note: A setting to set both _OTHER_FRAME and _SINGLE_FRAME is invalid
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_ON_HBLANK
 *         Set to _ENABLE to send the packet once on next HBLANK, set to _DISABLE
 *         to send on VBLANK.
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_VIDEO_FMT
 *         Set to _SW_CONTROLLED to set HDMI_Video_Format field and 3D_Structure field
 *         from NV_PDISP_SF_HDMI_VSI_SUBPACK0_HIGH_PB4 and PB5, if it is set to _HW_CONTROLLED
 *         then HW will get them based on the state of the setHdmiCtrl method.
 *         Btw this applies only for stereo ovverides.
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_SET_STEREO_POLARITY
 *         Set to TRUE to enable Vendor specific info frame used for 3D stereo LR sync
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_SET_SELF_REFRESH_SETTING
 *         Set to TRUE to enable Vendor specific info frame used for Self Refresh panels
 *       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_RESERVED_LEGACY_MODE
 *         This option is reserved for backward compatibility with
 *         NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_PACKET and
 *         NV0073_CTRL_CMD_DP_SET_PACKET.
 *         It is temporal and will be deprecated soon.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SPECIFIC_SET_OD_PACKET_CTRL                                        (0x730289U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_PARAMS_MESSAGE_ID (0x89U)

typedef struct NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 type;
    NvU32 transmitControl;
} NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_PARAMS;

#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_ENABLE                      NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_ENABLE
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_ENABLE_NO                      NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_ENABLE_NO
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_ENABLE_YES                     NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_ENABLE_YES
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_OTHER_FRAME                 NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_OTHER_FRAME
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_OTHER_FRAME_DISABLE            NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_OTHER_FRAME_DISABLE
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_OTHER_FRAME_ENABLE             NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_OTHER_FRAME_ENABLE
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_SINGLE_FRAME                NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SINGLE_FRAME
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_SINGLE_FRAME_DISABLE           NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SINGLE_FRAME_DISABLE
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_SINGLE_FRAME_ENABLE            NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SINGLE_FRAME_ENABLE
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_ON_HBLANK                   NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_ON_HBLANK
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_ON_HBLANK_DISABLE              NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_ON_HBLANK_DISABLE
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_ON_HBLANK_ENABLE               NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_ON_HBLANK_ENABLE
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_VIDEO_FMT                   NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_VIDEO_FMT
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_VIDEO_FMT_SW_CONTROLLED        NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_VIDEO_FMT_SW_CONTROLLED
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_VIDEO_FMT_HW_CONTROLLED        NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_VIDEO_FMT_HW_CONTROLLED
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_SET_STEREO_POLARITY         NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SET_STEREO_POLARITY
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_SET_STEREO_POLARITY_FALSE      NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SET_STEREO_POLARITY_FALSE
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_SET_STEREO_POLARITY_TRUE       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SET_STEREO_POLARITY_TRUE
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_SET_SELF_REFRESH_SETTING         NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SET_SELF_REFRESH_SETTING
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_SET_SELF_REFRESH_SETTING_TRUE  NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SET_SELF_REFRESH_SETTING_FALSE
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_SET_SELF_REFRESH_SETTING_FALSE NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_SET_SELF_REFRESH_SETTING_TRUE
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_RESERVED_LEGACY_MODE        NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_RESERVED_LEGACY_MODE
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_RESERVED_LEGACY_MODE_NO        NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_RESERVED_LEGACY_MODE_NO
#define NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL_RESERVED_LEGACY_MODE_YES       NV0073_CTRL_SPECIFIC_SET_OD_PACKET_TRANSMIT_CONTROL_RESERVED_LEGACY_MODE_YES

/*
 * NV0073_CTRL_CMD_SPECIFIC_GET_PCLK_LIMIT
 *
 * This command returns the maximum supported pixel clock rate that is
 * supported by the specified display device.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the display for which information is to be
 *     returned.  Only one display may be indicated in this parameter.
 *     If more than one displayId is used a failing status of
 *     NV_ERR_INVALID_ARGUMENT will be returned.
 *   pclkLimit
 *     This parameter returns the min of orPclkLimit and vbPclkLimit in KHz.
 *     It may be used for SLI configs that use a video bridge. For non-SLI
 *     configs and bridgeless SLI configs, the client should use orPclkLimit instead.
 *   orPclkLimit
 *     This parameter returns the maximum pixel clock frequency of OR in KHz.
 *   vbPclkLimit
 *     This parameter returns the maximum pixel clock frequency of the
 *     video bridge (SLI) in KHz (or zero if there is no video bridge).
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SPECIFIC_GET_PCLK_LIMIT                                                 (0x73028aU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_PCLK_LIMIT_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_GET_PCLK_LIMIT_PARAMS_MESSAGE_ID (0x8AU)

typedef struct NV0073_CTRL_SPECIFIC_GET_PCLK_LIMIT_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 pclkLimit;
    NvU32 orPclkLimit;
    NvU32 vbPclkLimit;
} NV0073_CTRL_SPECIFIC_GET_PCLK_LIMIT_PARAMS;

/*
 * NV0073_CTRL_CMD_SPECIFIC_OR_GET_INFO
 *
 * This command returns output resource information for the specified display
 * device.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the display for which information is to be
 *     returned.  Only one display may be indicated in this parameter.
 *     If more than one displayId is used a failing status of
 *     NV_ERR_INVALID_ARGUMENT will be returned.
 *   type
 *     This parameter returns the output resource type.  Legal values for
 *     this parameter include:
 *       NV0073_CTRL_SPECIFIC_OR_TYPE_DAC
 *         The output resource is a DAC.
 *       NV0073_CTRL_SPECIFIC_OR_TYPE_SOR
 *         The output resource is a serial output resource.
 *       NV0073_CTRL_SPECIFIC_OR_TYPE_DSI
 *         The output resource is a Display Serial Interface output resource.
 *       NV0073_CTRL_SPECIFIC_OR_TYPE_PIOR
 *         The output resource is a parallel input output resource.
 *   index
 *     This parameter returns the type-specific index of the output
 *     resource associated with the specified displayId.
 *   protocol
 *     This parameter returns the type-specific protocol used by the
 *     output resource.  Legal values for this parameter include:
 *       NV0073_CTRL_SPECIFIC_OR_PROTOCOL_DAC_RGB_CRT
 *       NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_LVDS_CUSTOM
 *       NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_SINGLE_TMDS_A
 *       NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_SINGLE_TMDS_B
 *       NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DUAL_TMDS
 *       NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_A
 *       NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_B
 *       NV0073_CTRL_SPECIFIC_OR_PROTOCOL_PIOR_EXT_TMDS_ENC
 *       NV0073_CTRL_SPECIFIC_OR_PROTOCOL_DSI
 *       NV0073_CTRL_SPECIFIC_OR_PROTOCOL_UNKNOWN
 *   ditherType
 *     This parameter returns the dither type for the output resource.
 *     Legal values for this parameter include:
 *       NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_6_BITS
 *       NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_8_BITS
 *       NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_10_BITS
 *       NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_OFF
 *   ditherAlgo
 *     This parameter returns the dithering algorithm used by the output
 *     resource.  Legal values for this parameter include:
 *       NV0073_CTRL_SPECIFIC_OR_DITHER_ALOGO_DYNAMIC_ERR_ACC
 *       NV0073_CTRL_SPECIFIC_OR_DITHER_ALOGO_STATIC_ERR_ACC
 *       NV0073_CTRL_SPECIFIC_OR_DITHER_ALOGO_DYNAMIC_2X2
 *       NV0073_CTRL_SPECIFIC_OR_DITHER_ALOGO_STATIC_2X2
 *       NV0073_CTRL_SPECIFIC_OR_DITHER_ALOGO_TEMPORAL
 *       NV0073_CTRL_SPECIFIC_OR_DITHER_ALOGO_UNKNOWN
 *   location
 *     This parameter returns the physical location of the output resource.
 *     Legal values for this parameter include:
 *       NV0073_CTRL_SPECIFIC_OR_LOCATION_CHIP
 *       NV0073_CTRL_SPECIFIC_OR_LOCATION_BOARD
 *   rootPortId
 *     This parameter specifies the Root-Port ID for the given display.
 *   dcbIndex
 *     This parameter returns the DCB index of the display device.
 *   vbiosAddress
 *      This parameter is the VBIOS IP address which will have valid value
 *      only if displayId is allocated by VBIOS.
 *   bIsLitByVbios
 *     This parameter specifies that whether displayID allocation was
 *     requested by VBIOS or not.
 *   bIsDispDynamic
 *     Returns NV_TRUE if DisplayID is allocated Dynamically else NV_FALSE
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SPECIFIC_OR_GET_INFO (0x73028bU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS_MESSAGE_ID (0x8BU)

typedef struct NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvU32  index;
    NvU32  type;
    NvU32  protocol;
    NvU32  ditherType;
    NvU32  ditherAlgo;
    NvU32  location;
    NvU32  rootPortId;
    NvU32  dcbIndex;
    NV_DECLARE_ALIGNED(NvU64 vbiosAddress, 8);
    NvBool bIsLitByVbios;
    NvBool bIsDispDynamic;
} NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS;

/* valid type values */
#define NV0073_CTRL_SPECIFIC_OR_TYPE_NONE                   (0x00000000U)
#define NV0073_CTRL_SPECIFIC_OR_TYPE_DAC                    (0x00000001U)
#define NV0073_CTRL_SPECIFIC_OR_TYPE_SOR                    (0x00000002U)
#define NV0073_CTRL_SPECIFIC_OR_TYPE_PIOR                   (0x00000003U)


#define NV0073_CTRL_SPECIFIC_OR_TYPE_DSI                    (0x00000005U)

/* valid DAC protocol values */
#define NV0073_CTRL_SPECIFIC_OR_PROTOCOL_DAC_RGB_CRT        (0x00000000U)



/* valid SOR protocol values */
#define NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_LVDS_CUSTOM    (0x00000000U)
#define NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_SINGLE_TMDS_A  (0x00000001U)
#define NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_SINGLE_TMDS_B  (0x00000002U)
#define NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DUAL_TMDS      (0x00000005U)
#define NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_A           (0x00000008U)
#define NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_B           (0x00000009U)
#define NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DSI            (0x00000010U)

/* valid DSI protocol values */
#define NV0073_CTRL_SPECIFIC_OR_PROTOCOL_DSI                (0x00000011U)

/* valid PIOR protocol values */
#define NV0073_CTRL_SPECIFIC_OR_PROTOCOL_PIOR_EXT_TMDS_ENC  (0x00000000U)

/* valid UNKNOWN protocol value */
#define NV0073_CTRL_SPECIFIC_OR_PROTOCOL_UNKNOWN            (0xFFFFFFFFU)

/* valid ditherType values */
#define NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_6_BITS          (0x00000000U)
#define NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_8_BITS          (0x00000001U)
#define NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_10_BITS         (0x00000002U)
#define NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_OFF             (0x00000003U)

/* valid ditherAlgo values */
#define NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_DYNAMIC_ERR_ACC (0x00000000U)
#define NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_STATIC_ERR_ACC  (0x00000001U)
#define NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_DYNAMIC_2X2     (0x00000002U)
#define NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_STATIC_2X2      (0x00000003U)
#define NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_TEMPORAL        (0x00000004U)
#define NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_UNKNOWN         (0xFFFFFFFFU)

/* valid location values */
#define NV0073_CTRL_SPECIFIC_OR_LOCATION_CHIP               (0x00000000U)
#define NV0073_CTRL_SPECIFIC_OR_LOCATION_BOARD              (0x00000001U)

/*
 * NV0073_CTRL_CMD_SPECIFIC_HDCP_KSVLIST_VALIDATE
 *
 * This Command initiate the KSV validation for the specific device
 * if it is a repeater.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be directed.
 *     This parameter must specify a value between zero and the total number
 *     of subdevices within the parent device.  This parameter should be set
 *     to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the root port device for which KsvList is to be validated
 *   bUseCachedKsvList
 *     The parameter specifies RM to use cachedKsvList in case BCAPS's READY bit not set to read
 *     ksvList.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_NOT_READY
 */
#define NV0073_CTRL_CMD_SPECIFIC_HDCP_KSVLIST_VALIDATE      (0x73028dU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_HDCP_KSVLIST_VALIDATE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_HDCP_KSVLIST_VALIDATE_PARAMS_MESSAGE_ID (0x8DU)

typedef struct NV0073_CTRL_SPECIFIC_HDCP_KSVLIST_VALIDATE_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvBool bUseCachedKsvList;
} NV0073_CTRL_SPECIFIC_HDCP_KSVLIST_VALIDATE_PARAMS;

/*
 * NV0073_CTRL_CMD_SPECIFIC_HDCP_UPDATE
 *
 * This Command updates the display to the proper HDCP state based on
 * whether it has been newly connected or disconnected. This is called
 * during a hotplug event.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be directed.
 *     This parameter must specify a value between zero and the total number
 *     of subdevices within the parent device.  This parameter should be set
 *     to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the root port device to update
 *   bIsConnected
 *     This parameter specifies whether the device has been connected (NV_TRUE)
 *     or disconnected (NV_FALSE).
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0073_CTRL_CMD_SPECIFIC_HDCP_UPDATE (0x73028eU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_HDCP_UPDATE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_HDCP_UPDATE_PARAMS_MESSAGE_ID (0x8EU)

typedef struct NV0073_CTRL_SPECIFIC_HDCP_UPDATE_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvBool bIsConnected;
} NV0073_CTRL_SPECIFIC_HDCP_UPDATE_PARAMS;



/*
 * NV0073_CTRL_CMD_SPECIFIC_GET_BACKLIGHT_BRIGHTNESS
 * NV0073_CTRL_CMD_SPECIFIC_SET_BACKLIGHT_BRIGHTNESS
 *
 * These commands retrieve and set the user backlight brightness for
 * the specified display.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *   displayId
 *     Display for which brightness is to be retrieved or set.
 *   brightness
 *     The backlight brightness in the range [0,100], inclusive.  This
 *     is an input for SET_BACKLIGHT_BRIGHTNESS, and an output for
 *     GET_BACKLIGHT_BRIGHTNESS.
 *   brightnessType
 *     This can take in one of the three parameters:
 *     NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_TYPE_PERCENT100(for percentage brightness with value calibrated to 100 scale),
 *     NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_TYPE_PERCENT1000(for percentage brightness with uncalibrated values),
 *     NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_TYPE_NITS(used when panel supports Nits based)
 *     based on the brightness control method to be used.
 *
 * Possible status values returned include:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0073_CTRL_CMD_SPECIFIC_GET_BACKLIGHT_BRIGHTNESS (0x730291U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_BACKLIGHT_BRIGHTNESS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_BACKLIGHT_BRIGHTNESS_MIN_VALUE        0U
#define NV0073_CTRL_BACKLIGHT_BRIGHTNESS_MAX_VALUE        100U

typedef struct NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvU32  brightness;
    NvBool bUncalibrated;
    NvU8   brightnessType;
} NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS;
#define NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_TYPE_PERCENT100        1
#define NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_TYPE_PERCENT1000       2
#define NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_TYPE_NITS              3

#define NV0073_CTRL_SPECIFIC_GET_BACKLIGHT_BRIGHTNESS_PARAMS_MESSAGE_ID (0x91U)

typedef NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS NV0073_CTRL_SPECIFIC_GET_BACKLIGHT_BRIGHTNESS_PARAMS;

#define NV0073_CTRL_CMD_SPECIFIC_SET_BACKLIGHT_BRIGHTNESS (0x730292U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_SET_BACKLIGHT_BRIGHTNESS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_SET_BACKLIGHT_BRIGHTNESS_PARAMS_MESSAGE_ID (0x92U)

typedef NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS NV0073_CTRL_SPECIFIC_SET_BACKLIGHT_BRIGHTNESS_PARAMS;

/*
 * NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS
 *
 * This command is used to inform RM about the scrambling, clock mode, FRL and
 * DSC caps of the HDMI sink device.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed..
 *   displayID
 *     This parameter specifies the displayID for the display output resource to
 *     configure.
 *   caps
 *     This parameter specifies the sink caps.
 *       GT_340MHZ_CLOCK_SUPPORTED refers to whether sink supports TMDS clock (sorClk) rate greater than 340 MHz
 *       LTE_340MHZ_SCRAMBLING_SUPPORTED refers to whether scrambling is supported for clock rate at or below 340 MHz
 *       SCDC_SUPPORTED refers to whether SCDC access is supported on sink
 *       MAX_FRL_RATE_SUPPORTED refers to the maximum HDMI 2.1 FRL rate supported
 *       DSC_12_SUPPORTED refers to whether VESA DSC v1.2a is supported
 *       DSC_12_MAX_FRL_RATE_SUPPORTED refers to the maximum HDMI 2.1 FRL rate supported when VESA DSC v1.2a is supported
 *
 *
 * Possible status values returned include:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS (0x730293U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_SET_HDMI_SINK_CAPS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_SET_HDMI_SINK_CAPS_PARAMS_MESSAGE_ID (0x93U)

typedef struct NV0073_CTRL_SPECIFIC_SET_HDMI_SINK_CAPS_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 caps;
} NV0073_CTRL_SPECIFIC_SET_HDMI_SINK_CAPS_PARAMS;

#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_GT_340MHZ_CLOCK_SUPPORTED                           0:0
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_GT_340MHZ_CLOCK_SUPPORTED_FALSE       (0x00000000U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_GT_340MHZ_CLOCK_SUPPORTED_TRUE        (0x00000001U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_LTE_340MHZ_SCRAMBLING_SUPPORTED                     1:1
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_LTE_340MHZ_SCRAMBLING_SUPPORTED_FALSE (0x00000000U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_LTE_340MHZ_SCRAMBLING_SUPPORTED_TRUE  (0x00000001U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_SCDC_SUPPORTED                                      2:2
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_SCDC_SUPPORTED_FALSE                  (0x00000000U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_SCDC_SUPPORTED_TRUE                   (0x00000001U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_MAX_FRL_RATE_SUPPORTED                              5:3
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_MAX_FRL_RATE_SUPPORTED_NONE           (0x00000000U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_MAX_FRL_RATE_SUPPORTED_3LANES_3G      (0x00000001U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_MAX_FRL_RATE_SUPPORTED_3LANES_6G      (0x00000002U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_MAX_FRL_RATE_SUPPORTED_4LANES_6G      (0x00000003U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_MAX_FRL_RATE_SUPPORTED_4LANES_8G      (0x00000004U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_MAX_FRL_RATE_SUPPORTED_4LANES_10G     (0x00000005U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_MAX_FRL_RATE_SUPPORTED_4LANES_12G     (0x00000006U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_DSC_12_SUPPORTED                                    6:6
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_DSC_12_SUPPORTED_FALSE                (0x00000000U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_DSC_12_SUPPORTED_TRUE                 (0x00000001U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_DSC_MAX_FRL_RATE_SUPPORTED                          9:7
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_DSC_MAX_FRL_RATE_SUPPORTED_NONE       (0x00000000U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_DSC_MAX_FRL_RATE_SUPPORTED_3LANES_3G  (0x00000001U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_DSC_MAX_FRL_RATE_SUPPORTED_3LANES_6G  (0x00000002U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_DSC_MAX_FRL_RATE_SUPPORTED_4LANES_6G  (0x00000003U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_DSC_MAX_FRL_RATE_SUPPORTED_4LANES_8G  (0x00000004U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_DSC_MAX_FRL_RATE_SUPPORTED_4LANES_10G (0x00000005U)
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_DSC_MAX_FRL_RATE_SUPPORTED_4LANES_12G (0x00000006U)



/*
 * NV0073_CTRL_CMD_SPECIFIC_SET_MONITOR_POWER
 *
 * This command sets monitor power on/off.
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *   displayId
 *     This parameter specifies the displayID for the display output resource to
 *     configure.
 *   powerState
 *     This parameter should be one of the valid
 *     NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_* values.
 *   headIdx
 *     The head id on which power operation needs to be done.
 *   bForceMonitorState
 *     Monitor power state that client wants to force in RM.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_CMD_SPECIFIC_SET_MONITOR_POWER               (0x730295U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_PARAMS_MESSAGE_ID (0x95U)

typedef struct NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvU32  powerState;
    NvU32  headIdx;
    NvBool bForceMonitorState;
} NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_PARAMS;

#define NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_OFF     (0x00000000U)
#define NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_ON      (0x00000001U)



/*
* NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_FRL_CONFIG
*
* This command is used to perform HDMI FRL link training and enable FRL mode for
* the specified displayId. The link configuration will be returned after link
* training success.
*
*   subDeviceInstance
*     This parameter specifies the subdevice instance within the
*     NV04_DISPLAY_COMMON parent device to which the operation should be
*     directed.
*   displayID
*     This parameter specifies the displayID for the display output resource to
*     configure.
*   data
*     This parameter is an input and output to this command.
*     Here are the current defined fields:
*       NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE
*         This field specifies the desired setting for lane count. A client may
*         choose any lane count as long as it does not exceed the capability of
*         HDMI FRL sink as indicated in the sink capability field.
*         The valid values for this field are:
*           NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_NONE
*             For 0 lane configuration, link training is shut down (disable FRL).
*           NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_3LANES_3G
*             For FRL 3-lane configuration and 3 Gbps bandwidth per lane.
*           NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_3LANES_6G
*             For FRL 3-lane configuration and 6 Gbps bandwidth per lane.
*           NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_6G
*             For FRL 4-lane configuration and 6 Gbps bandwidth per lane.
*           NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_8G
*             For FRL 4-lane configuration and 8 Gbps bandwidth per lane.
*           NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_10G
*             For FRL 4-lane configuration and 10 Gbps bandwidth per lane.
*           NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_12G
*             For FRL 4-lane configuration and 12 Gbps bandwidth per lane.
*         On return, the link bandwidth setting is returned which may be
*         different from the requested input setting.
*   bFakeLt
*     This flag as input to this command.
*     It indicates the FRL link training is a fake link training or not.
*     TRUE if the FRL link training is fake and no real sink device attached.
*   bLtSkipped
*     The flag returned indicating whether link training is skipped or not.
*     TRUE if link training is skipped due to the link config is not changed.
*   bLinkAssessmentOnly
*     The flag as input to this command. It indicates that the client is doing
*     link training only for the link assessment, no FRL video transmission is
*     intended.
*
* Possible status values returned include:
* NV_OK -
*   Affter finishing link tranning, NV_OK status will be returned along with
*   the updated link congiration. In case of link training failure, FRL_RATE_NONE
*   will be returned with NV_OK.
* NV_ERR_NOT_SUPPORTED -
*   If the GPU/sink is not capable for HDMI FRL, NV_ERR_NOT_SUPPORTED status
*   will be returned.
* NV_ERR_INVALID_ARGUMENT
*   If any argument is valid for this control call, NV_ERR_INVALID_ARGUMENT
*   status will be returned.
*/
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_FRL_CONFIG (0x73029aU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_LINK_CONFIG_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_LINK_CONFIG_PARAMS_MESSAGE_ID (0x9AU)

typedef struct NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_LINK_CONFIG_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvU32  data;
    NvBool bFakeLt;
    NvBool bDoNotSkipLt;
    NvBool bLtSkipped;
    NvBool bLinkAssessmentOnly;
} NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_LINK_CONFIG_PARAMS;

#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE                                           2:0
#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_NONE       (0x00000000U)
#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_3LANES_3G  (0x00000001U)
#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_3LANES_6G  (0x00000002U)
#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_6G  (0x00000003U)
#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_8G  (0x00000004U)
#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_10G (0x00000005U)
#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_12G (0x00000006U)

/*
* NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_FRL_FLUSH_MODE
*
* This command is used to enable flush mode on given HDMI displayId in
* preparation for FRL link training. The flush mode can be enabled only if HDMI
* display is active and driven by FRL protocol. If display is not active then
* this control command does nothing.
*
*   subDeviceInstance
*     This parameter specifies the subdevice instance within the
*     NV04_DISPLAY_COMMON parent device to which the operation should be
*     directed.
*   displayID
*     This parameter specifies the displayID for the display output resource to
*     configure.
*   bEnable
*     This parameter is an inputto this command.
*
* Possible status values returned include:
* NV_OK
* NV_ERR_INVALID_ARGUMENT
*   If any argument is invalid for this control call, NV_ERR_INVALID_ARGUMENT
*   status will be returned.
*/
#define NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_FRL_FLUSH_MODE  (0x73029bU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_FLUSH_MODE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_FLUSH_MODE_PARAMS_MESSAGE_ID (0x9BU)

typedef struct NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_FLUSH_MODE_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvBool bEnable;
} NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_FLUSH_MODE_PARAMS;



#define NV0073_CTRL_SPECIFIC_MAX_CRC_REGIONS 9U

#define NV0073_CTRL_CMD_SPECIFIC_GET_REGIONAL_CRCS_PARAMS_MESSAGE_ID (0xA0U)

typedef struct NV0073_CTRL_CMD_SPECIFIC_GET_REGIONAL_CRCS_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 regionCrcs[NV0073_CTRL_SPECIFIC_MAX_CRC_REGIONS];
    NvU16 reqRegionCrcMask;
} NV0073_CTRL_CMD_SPECIFIC_GET_REGIONAL_CRCS_PARAMS;

/*
 * NV0073_CTRL_CMD_SPECIFIC_GET_REGIONAL_CRCS
 *
 * This command is used to capture the active viewport region CRCs
 *
 *  [in]subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device. This parameter
 *     should be set to zero for default behavior.
 *
 *  [in]displayId
 *     This parameter specifies the displayId of panel, for which region CRC to be captured
.*
 *  [out]regionCrcs
 *      This field holds the region CRC values to be returned after successful completion of the control command.
 *
 *  [in]reqRegionCrcMask
 *     This parameter specifies BIT mask value of requested CRC regions.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV0073_CTRL_CMD_SPECIFIC_GET_REGIONAL_CRCS      (0x7302a0U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_CMD_SPECIFIC_GET_REGIONAL_CRCS_PARAMS_MESSAGE_ID" */

/*
* NV0073_CTRL_CMD_SPECIFIC_APPLY_EDID_OVERRIDE_V2
*
* Apply EDID override on specific OD.
*
*   subDeviceInstance
*     This parameter specifies the subdevice instance within the
*     NV04_DISPLAY_COMMON parent device to which the operation should be
*     directed.
*   displayId (in)
*     ID of panel on which the operation is to be performed.
*   bufferSize (in)
*     Size of the EDID buffer.
*   edidBuffer (in/out)
*     The buffer which stores the EDID before and after override.
*
* Possible status values returned are:
*   NV_OK
*   NV_ERR_INVALID_PARAMETER
*/
#define NV0073_CTRL_CMD_SPECIFIC_APPLY_EDID_OVERRIDE_V2 (0x7302a1U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_APPLY_EDID_OVERRIDE_V2_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_APPLY_EDID_OVERRIDE_V2_PARAMS_MESSAGE_ID (0xA1U)

typedef struct NV0073_CTRL_SPECIFIC_APPLY_EDID_OVERRIDE_V2_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 bufferSize;
    NvU8  edidBuffer[NV0073_CTRL_SPECIFIC_GET_EDID_MAX_EDID_BYTES];
} NV0073_CTRL_SPECIFIC_APPLY_EDID_OVERRIDE_V2_PARAMS;

/*
 * NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_GPU_CAPS
 *
 * This command is used to get the HDMI FRL caps of GPU side.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *   caps
 *     This parameter specifies the GPU caps.
 *       MAX_FRL_RATE_SUPPORTED refers to the maximum HDMI 2.1 FRL link rate supported
 *
 *
 * Possible status values returned include:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_GPU_CAPS (0x7302a2U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_HDMI_GPU_CAPS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_GET_HDMI_GPU_CAPS_PARAMS_MESSAGE_ID (0xA2U)

typedef struct NV0073_CTRL_SPECIFIC_GET_HDMI_GPU_CAPS_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 caps;
} NV0073_CTRL_SPECIFIC_GET_HDMI_GPU_CAPS_PARAMS;

#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_GPU_CAPS_MAX_FRL_LINK_RATE_SUPPORTED                           2:0
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_GPU_CAPS_MAX_FRL_LINK_RATE_SUPPORTED_NONE       (0x00000000U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_GPU_CAPS_MAX_FRL_LINK_RATE_SUPPORTED_3LANES_3G  (0x00000001U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_GPU_CAPS_MAX_FRL_LINK_RATE_SUPPORTED_3LANES_6G  (0x00000002U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_GPU_CAPS_MAX_FRL_LINK_RATE_SUPPORTED_4LANES_6G  (0x00000003U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_GPU_CAPS_MAX_FRL_LINK_RATE_SUPPORTED_4LANES_8G  (0x00000004U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_GPU_CAPS_MAX_FRL_LINK_RATE_SUPPORTED_4LANES_10G (0x00000005U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_GPU_CAPS_MAX_FRL_LINK_RATE_SUPPORTED_4LANES_12G (0x00000006U)



/*
 * NV0073_CTRL_CMD_SPECIFIC_DISPLAY_CHANGE
 *
 * Notifies the system that a display change is about to begin/end.
 * Also performs the necessary synchronizations for the same.
 *
 * The command takes a NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_PARAMS structure as an
 * argument with appropriate subDeviceInstance.
 *
 *  [in]subDeviceInstance
 *   The sub-device instance
 *  [in]newDevices
 *   Bitmask of devices that are planned on being enabled with the
 *   pending device change. See NV_CFGEX_GET_DEVICES_CONFIGURATION for bit defs.
 *  [in]properties
 *   Bitmask of display attributes for new configuration (none used at the moment).
 *  [in]enable
 *   Parameter to decide between display change start and end. Can take values
 *   NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_START or NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_END.
 * Possible return values:
 *  NV_OK
 *  NV_ERR_INVALID_ARGUMENT
 *
 */

#define NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_PARAMS_MESSAGE_ID (0xA4U)

typedef struct NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_PARAMS {

    NvU32 subDeviceInstance;
    NvU32 newDevices;
    NvU32 properties;
    NvU32 enable;
} NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_PARAMS;

#define NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_END                 (0x00000000U)
#define NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_START               (0x00000001U)

#define NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_PROPERTIES_SPANNING (0x00000001U)

#define NV0073_CTRL_CMD_SPECIFIC_DISPLAY_CHANGE                 (0x7302a4U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_PARAMS_MESSAGE_ID" */



/*
 * NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA
 *
 * This command is used to get the HDMI sink status/caps via Status and Control
 * Data Channel (SCDC).
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *   displayId
 *     This parameter specifies the displayId of HDMI sink.
 *   offset
 *     This parameter specifies the SCDC offset which the read operation
 *     should be used.
 *   data
 *     This field specifies the return data from sink for reading the specified
 *     SCDC offset.
 *
 *
 * Possible status values returned include:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA         (0x7302a6U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_HDMI_SCDC_DATA_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_GET_HDMI_SCDC_DATA_PARAMS_MESSAGE_ID (0xA6U)

typedef struct NV0073_CTRL_SPECIFIC_GET_HDMI_SCDC_DATA_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU8  offset;
    NvU8  data;
} NV0073_CTRL_SPECIFIC_GET_HDMI_SCDC_DATA_PARAMS;

#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET                                       7:0
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_NONE                      (0x00000000U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_SINK_VERSION              (0x00000001U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_SOURCE_VERSION            (0x00000002U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_UPDATE_FLAGS_0            (0x00000010U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_TMDS_CONFIGURATION        (0x00000020U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_SCRAMBLER_STATUS          (0x00000021U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_CONFIGURATION_0           (0x00000030U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_CONFIGURATION_1           (0x00000031U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_SOURCE_TEST_CONFIGURATION (0x00000035U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_STATUS_FLAGS_0            (0x00000040U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_STATUS_FLAGS_1            (0x00000041U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_STATUS_FLAGS_2            (0x00000042U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_CED_0                     (0x00000050U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_CED_1                     (0x00000051U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_CED_2                     (0x00000052U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_CED_3                     (0x00000053U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_CED_4                     (0x00000054U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_CED_5                     (0x00000055U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_CED_6                     (0x00000056U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_CED_7                     (0x00000057U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_CED_8                     (0x00000058U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_RSED_0                    (0x00000059U)
#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_SCDC_DATA_OFFSET_RSED_1                    (0x0000005AU)

/*
 * NV0073_CTRL_CMD_SPECIFIC_IS_DIRECTMODE_DISPLAY
 *
 * This command is used to query whether the specified monitor should be used
 * with directmode.
 *
 *   [in]manufacturerID
 *     This parameter specifies the 16-bit EDID Manufacturer ID.
 *   [in]productID
 *     This parameter specifies the 16-bit EDID Product ID.
 *   [out]bIsDirectmode;
 *     This indicates whether the monitor should be used with directmode.
 * Possible return values:
 *  NV_OK
 */

#define NV0073_CTRL_CMD_SPECIFIC_IS_DIRECTMODE_DISPLAY                               (0x7302a7U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_IS_DIRECTMODE_DISPLAY_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_IS_DIRECTMODE_DISPLAY_PARAMS_MESSAGE_ID (0xA7U)

typedef struct NV0073_CTRL_SPECIFIC_IS_DIRECTMODE_DISPLAY_PARAMS {
    NvU16  manufacturerID;
    NvU16  productID;
    NvBool bIsDirectmode;
} NV0073_CTRL_SPECIFIC_IS_DIRECTMODE_DISPLAY_PARAMS;

/*
 * NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION
 *
 * This command is used to get the HDMI FRL capacity computation result.
 *
 *   [in] cmd
 *     This parameter specifies the command for the HDMI FRL capacity computation.
 *   [in] input
 *     This parameter specifies the input data for the HDMI FRL capacity
 *     computation.
 *   [out] result
 *     This indicates the computation result of HDMI FRL capacity computation.
 *   [in/out] preCalc
 *     This indicates the pre-caculation result of HDMI FRL capacity computation.
 *   [in/out] dsc
 *     This indicates the DSC parameters of HDMI FRL capacity computation.
 * Possible return values:
 *  NV_OK
 */

#define NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION (0x7302a8U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS_MESSAGE_ID" */

typedef struct NV0073_CTRL_FRL_CAPACITY_COMPUTATION_PARAMS {
    NvU32 numLanes;
    NvU32 frlBitRateGbps;
    NvU32 pclk10KHz;
    NvU32 hTotal;
    NvU32 hActive;
    NvU32 bpc;
    NvU32 pixelPacking;
    NvU32 audioType;
    NvU32 numAudioChannels;
    NvU32 audioFreqKHz;

    struct {
        NvU32 bppTargetx16;
        NvU32 hSlices;
        NvU32 sliceWidth;
        NvU32 dscTotalChunkKBytes;
    } compressionInfo;
} NV0073_CTRL_FRL_CAPACITY_COMPUTATION_PARAMS;

typedef struct NV0073_CTRL_FRL_CAPACITY_COMPUTATION_RESULT {
    NvU32  frlRate;
    NvU32  bppTargetx16;
    NvBool engageCompression;
    NvBool isAudioSupported;
    NvBool dataFlowDisparityReqMet;
    NvBool dataFlowMeteringReqMet;
    NvBool isVideoTransportSupported;
    NvU32  triBytesBorrowed;
    NvU32  hcActiveBytes;
    NvU32  hcActiveTriBytes;
    NvU32  hcBlankTriBytes;
    NvU32  tBlankToTTotalX1k;
} NV0073_CTRL_FRL_CAPACITY_COMPUTATION_RESULT;

typedef struct NV0073_CTRL_FRL_PRE_CALC_CONFIG {
    NvU32  vic;
    NvU32  packing;
    NvU32  bpc;
    NvU32  frlRate;
    NvU32  bppX16;
    NvBool bHasPreCalcFRLData;
} NV0073_CTRL_FRL_PRE_CALC_CONFIG;

typedef struct NV0073_CTRL_IS_FRL_DSC_POSSIBLE_PARAMS {
    NvU32  maxSliceCount;
    NvU32  maxSliceWidth;
    NvBool bIsDSCPossible;
} NV0073_CTRL_IS_FRL_DSC_POSSIBLE_PARAMS;

#define NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS_MESSAGE_ID (0xA8U)

typedef struct NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS {
    NvU8                                        cmd;
    NV0073_CTRL_FRL_CAPACITY_COMPUTATION_PARAMS input;
    NV0073_CTRL_FRL_CAPACITY_COMPUTATION_RESULT result;
    NV0073_CTRL_FRL_PRE_CALC_CONFIG             preCalc;
    NV0073_CTRL_IS_FRL_DSC_POSSIBLE_PARAMS      dsc;
} NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS;

#define NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_CMD_NULL                               (0x00000000U)
#define NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_CMD_UNCOMPRESSED_VIDEO                 (0x00000001U)
#define NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_CMD_COMPRESSED_VIDEO                   (0x00000002U)
#define NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_CMD_HAS_PRECAL_FRL_DATA                (0x00000003U)
#define NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_CMD_GET_PRECAL_UNCOMPRESSED_FRL_CONFIG (0x00000004U)
#define NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_CMD_GET_PRECAL_COMPRESSED_FRL_CONFIG   (0x00000005U)
#define NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_CMD_IS_FRL_DSC_POSSIBLE                (0x00000006U)

/*
 * NV0073_CTRL_CMD_SPECIFIC_SET_SHARED_GENERIC_PACKET
 *
 * This command is used to program the display output packets.
 * This generic packets can be used for both HDMI and DP.
 * HW has added 6 new generic packets for each head because some usecases have
 * requirement to send infoframe in particular location (vsync, vblank, loadV).
 *
 * Note: 1. Client first needs to reserve or acquire a free infoframe index
 *          using NV0073_CTRL_CMD_SPECIFIC_ACQUIRE_SHARED_GENERIC_PACKET.
 *       2. Client needs to update the SDP index for head through control call
 *          NV0073_CTRL_CMD_SPECIFIC_SET_SHARED_GENERIC_PACKET
 *       3. Client needs to Release the infoframe index using control call
 *          NV0073_CTRL_CMD_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET
 *
 *   [in]subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *     This parameter must specify a value between zero and the total number
 *     of subdevices within the parent device.  This parameter should be set
 *     to zero for default behavior.
 *   [in]transmitControl
 *     This parameter controls how the packet is to be sent by setting the
 *     control bits.
 *     Possible flags are as follows:
 *       NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_ENABLE
 *         Setting this field to _YES will enable this generic infoframe,
 *         Setting this field to _NO will disable this generic infoframe.
 *       NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_SINGLE
 *        Set to _YES will cause new infoframe to be transmitted exactly once.
 *        Set to _NO will cause new infoframe to be transmitted every frame.
 *       NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_LOC
 *         SDP can be sent in 3 different locations:
 *           VBLANK - new infoframe will be sent at Vblank.
 *           VSYNC - new infoframe will be sent at Vsync.
 *           LOADV - new infoframe will be triggered by LOADV, and sent at Vsync
 *       NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_VSC_SDP_UPDATE_RFB_OVERRIDE
 *         _ENABLE: override DB1 bit1 with existence of loadv (for Panel Self Refresh)
 *         _DISABLE: do not override shared generic infoframe subpacker DB1 bit1.
 *       NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_VSC_SDP_SU_COORDINATES_VALID_OVERRIDE
 *        _ENABLE: override DB1 bit3 with existence of loadv (for Panel Replay)
 *        _DISABLE: do not override shared generic infoframe subpacker DB1 bit3.
 *   [in]packetSize
 *     size of packets in Packet array to send, including header and payload.
 *   [in]infoframeIndex
 *     Specifies the target head number for which SDP needs to be updated.
 *   [in]infoframeIndex
 *     Specifies the index of infoframe.
 *   [in]packet
 *     pPacket points to the packets to send.
 *     For HDMI 1.1, the maximum allowed bytes is 31.
 *     The packet array includes the 3 bytes of header + data depending on
 *     the type of packet.  For an infoframe, the header bytes refer to type,
 *     version and length respectively.  This comes as input to this command.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0073_CTRL_CMD_SPECIFIC_SET_SHARED_GENERIC_PACKET                                            (0x7302a9) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_SET_SHARED_GENERIC_PACKET_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_SET_SHARED_GENERIC_PACKET_PARAMS_MESSAGE_ID (0xA9U)

typedef struct NV0073_CTRL_SPECIFIC_SET_SHARED_GENERIC_PACKET_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 transmitControl;
    NvU32 packetSize;
    NvU32 targetHeadIndex;
    NvU32 infoframeIndex;
    NvU8  packet[NV0073_CTRL_SET_OD_MAX_PACKET_SIZE];
} NV0073_CTRL_SPECIFIC_SET_SHARED_GENERIC_PACKET_PARAMS;

#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_ENABLE                                               0:0
#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_ENABLE_NO                                     (0x0000000)
#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_ENABLE_YES                                    (0x0000001)

#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_SINGLE                                               1:1
#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_SINGLE_NO                                     (0x0000000)
#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_SINGLE_YES                                    (0x0000001)

#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_LOC                                                  5:2
#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_LOC_VBLANK                                    (0x0000000)
#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_LOC_VSYNC                                     (0x0000001)
#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_LOC_LOADV                                     (0x0000002)

#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_VSC_SDP_STATE_OVERRIDE                               6:6
#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_VSC_SDP_STATE_OVERRIDE_DISABLE                (0x0000000)
#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_VSC_SDP_STATE_OVERRIDE_ENABLE                 (0x0000001)

#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_VSC_SDP_UPDATE_RFB_OVERRIDE                          7:7
#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_VSC_SDP_UPDATE_RFB_OVERRIDE_DISABLE           (0x0000000)
#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_VSC_SDP_UPDATE_RFB_OVERRIDE_ENABLE            (0x0000001)

#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_VSC_SDP_SU_COORDINATES_VALID_OVERRIDE                8:8
#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_VSC_SDP_SU_COORDINATES_VALID_OVERRIDE_DISABLE (0x0000000)
#define NV0073_CTRL_SPECIFIC_SHARED_GENERIC_CTRL_VSC_SDP_SU_COORDINATES_VALID_OVERRIDE_ENABLE  (0x0000001)

/*
 * NV0073_CTRL_CMD_SPECIFIC_ACQUIRE_SHARED_GENERIC_PACKET
 *
 * This command is used to reserve the infoframe for head and RM would assign
 * free infoframe index and return the index of infoframe. Later client needs
 * to call control call NV0073_CTRL_CMD_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET
 * to release the index.
 *
 *   [in]subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *     This parameter must specify a value between zero and the total number
 *     of subdevices within the parent device.  This parameter should be set
 *     to zero for default behavior.
 *   [in]targetHeadIndex
 *     target Head for which SDP needs to be sent
 *   [out]infoframeIndex
 *     return Infoframe Index for head.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INSUFFIENT_RESOURCES
 */

#define NV0073_CTRL_CMD_SPECIFIC_ACQUIRE_SHARED_GENERIC_PACKET (0x7302aa) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_ACQUIRE_SHARED_GENERIC_PACKET_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_ACQUIRE_SHARED_GENERIC_PACKET_PARAMS_MESSAGE_ID (0xAAU)

typedef struct NV0073_CTRL_SPECIFIC_ACQUIRE_SHARED_GENERIC_PACKET_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 targetHeadIndex;
    NvU32 infoframeIndex;
} NV0073_CTRL_SPECIFIC_ACQUIRE_SHARED_GENERIC_PACKET_PARAMS;

/*
 * NV0073_CTRL_CMD_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET
 *
 * This command is used to release the infoframe index which was acquired by
 * client.
 *
 *   [in]subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed.
 *     This parameter must specify a value between zero and the total number
 *     of subdevices within the parent device.  This parameter should be set
 *     to zero for default behavior.
 *   [in]targetHeadIndex
 *     Specifies the target head number for which SDP needs to be updated.
 *   [in]infoframeIndex
 *     Infoframe index for the target head
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0073_CTRL_CMD_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET (0x7302ab) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET_PARAMS_MESSAGE_ID (0xABU)

typedef struct NV0073_CTRL_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 targetHeadIndex;
    NvU32 infoframeIndex;
} NV0073_CTRL_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET_PARAMS;

/*
 * NV0073_CTRL_CMD_SPECIFIC_DISP_I2C_READ_WRITE
 *
 * This command is used to do I2C R/W to slave on display i2c instance.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   i2cPort
 *      This parameter specifies the I2C CCB port ID.
 *   i2cSlaveAddress
 *      This parameter specifies the I2C slave address.
 *   readWriteFlag
 *      This parameter specifies whether its read/write operation.
 *   readWriteLen
 *      This parameter specifies the length of the read/write buffer
 *   readBuffer
 *      This parameter reads the data from slave address and copies to this
 *      buffer
 *   writeBuffer
 *      This parameter specifies this buffer data that would be written to
 *      slave address
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0073_CTRL_CMD_SPECIFIC_DISP_I2C_READ_WRITE     (0x7302acU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_DISP_I2C_READ_WRITE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_DISP_I2C_READ_WRITE_BUF_LEN 128U

#define NV0073_CTRL_SPECIFIC_DISP_I2C_READ_WRITE_PARAMS_MESSAGE_ID (0xACU)

typedef struct NV0073_CTRL_SPECIFIC_DISP_I2C_READ_WRITE_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 i2cPort;
    NvU32 i2cSlaveAddress;
    NvU32 readWriteFlag;
    NvU32 readWriteLen;
    NvU8  readBuffer[NV0073_CTRL_SPECIFIC_DISP_I2C_READ_WRITE_BUF_LEN];
    NvU8  writeBuffer[NV0073_CTRL_SPECIFIC_DISP_I2C_READ_WRITE_BUF_LEN];
} NV0073_CTRL_SPECIFIC_DISP_I2C_READ_WRITE_PARAMS;

#define NV0073_CTRL_SPECIFIC_DISP_I2C_READ_MODE                   (0x00000001)
#define NV0073_CTRL_SPECIFIC_DISP_I2C_WRITE_MODE                  (0x00000000)

/*
 * NV0073_CTRL_CMD_GET_VALID_HEAD_WINDOW_ASSIGNMENT
 *
 * This command returns the valid window head assignment mask
 *
 * windowHeadMask [out]
 *   This out parameter is an array which holds the head mask for
 *   each window. The Nth element in the array would be a bitmask
 *   of which heads can possibly drive window N.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_GENERIC
 */
#define NV0073_CTRL_CMD_SPECIFIC_GET_VALID_HEAD_WINDOW_ASSIGNMENT (0x7302ad) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_VALID_HEAD_WINDOW_ASSIGNMENT_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_MAX_WINDOWS                          32U
#define NV0073_CTRL_SPECIFIC_FLEXIBLE_HEAD_WINDOW_ASSIGNMENT      (0xFFU)

#define NV0073_CTRL_SPECIFIC_GET_VALID_HEAD_WINDOW_ASSIGNMENT_PARAMS_MESSAGE_ID (0xADU)

typedef struct NV0073_CTRL_SPECIFIC_GET_VALID_HEAD_WINDOW_ASSIGNMENT_PARAMS {
    NvU32 subDeviceInstance;
    NvU8  windowHeadMask[NV0073_CTRL_SPECIFIC_MAX_WINDOWS];
} NV0073_CTRL_SPECIFIC_GET_VALID_HEAD_WINDOW_ASSIGNMENT_PARAMS;

/*
 * NV0073_CTRL_CMD_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY
 *
 * This command is used to query whether the specified monitor should default
 * to adaptive sync.
 *
 *   [in]manufacturerID
 *     This parameter specifies the 16-bit EDID Manufacturer ID.
 *   [in]productID
 *     This parameter specifies the 16-bit EDID Product ID.
 *   [out]bDefaultAdaptivesync;
 *     This indicates whether the monitor should default to adaptive sync.
 * Possible return values:
 *  NV_OK
 */

#define NV0073_CTRL_CMD_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY (0x7302aeU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY_PARAMS_MESSAGE_ID (0xAEU)

typedef struct NV0073_CTRL_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY_PARAMS {
    NvU16  manufacturerID;
    NvU16  productID;
    NvBool bDefaultAdaptivesync;
} NV0073_CTRL_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY_PARAMS;

/*
 * NV0073_CTRL_CMD_SPECIFIC_GET_DISPLAY_BRIGHTNESS_LTM
 * NV0073_CTRL_CMD_SPECIFIC_SET_DISPLAY_BRIGHTNESS_LTM
 * These commands retrieve and set the brightness level and Local Tone Mapping (LTM) settings for
 * the specified display.
 *
 * subDeviceInstance
 * This parameter specifies the subdevice instance within the
 * NV04_DISPLAY_COMMON parent device to which the operation should be
 * directed.
 *
 * displayId
 * Display for which brightness/LTM settings is to be retrieved or set.
 *
 * brightnessMilliNits
 * the display brightness in the millinits units in the [0,10000000] range, inclusive.
 *
 * transitionTimeMs
 * the transition time for display brightness to transition from current brightness level to the brightnessMilliNits
 *
 * bUncalibrated
 * if true the parameter indicates brightnessMilliNits has to be interpreted as brightness % value, in 0.001% units
 *
 * bAdaptiveBrightness
 * if true the brightnessMilliNits is set by the OS in the response to the ambient light sensor (ALS) input (if present on the system)
 *
 * bBoostRange
 * if true the brightnessMilliNits value is set by the OS in the response to the ambient light sensor (ALS) input (if present on the system)
 * Indicating very bright ambient light environment
 *
 * ambientIlluminance
 * if bAdaptiveBrightness = true, ambientIlluminance represents an ambient illuminance value reported by the ALS sensor, in 0.001 Lux units
 *
 * ambientChromaticityX
 * if bAdaptiveBrightness = true, ambientChromaticityX represents X chromaticity value reported by ALS sensor, in 0.001 units of [0,1] range
 *
 * ambientChromaticityY
 * if bAdaptiveBrightness = true, ambientChromaticityY represents Y chromaticity value reported by ALS sensor, in 0.001 units of [0,1] range
 *
 * bLtmEnable
 * if set to true, enable LTM functionality in GPU display HW. Set to true by default.
 *
 * bEnhanceContrast
 * if bLtmEnable = true, and if set to true, enhance local contrast via LTM regardless of any other contrast enhancement driver policies. Set to false by default.
 *
 * contrastGain
 * if bLtmEnable = true, this is used to how gain we need to apply on the contrast.
 *
 * detailGain
 * if bLtmEnable = true, how much details needs to be boosted is indicated by this parameter.
 *
 * bContentAdaptiveBrightness
 * if bLtmEnable = true, and if set to true, modify display backlight level and adjust pixel values dynamically on per-frame basis to perform content adaptive brightness control to reduce display power. Set to false by default.
 *
 * bDynamicHdrTonemapping
 * if bLtmEnable = true, and if set to true, and output is HDR, enable dynamic per frame HDR tonemapping. Set to false by default.
 *
 * maxDisplayLuminance
 * maximum display luminance
 *
 * luminanceSscalingFactor
 * HDR tone mapping luminance scaling factor
 *
 * Possible status values returned include:
 * NV_OK
 * NV_ERR_NOT_SUPPORTED
 */

typedef struct NV0073_CTRL_SPECIFIC_DISPLAY_BRIGHTNESS_LTM_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvU32  brightnessMilliNits;
    NvU32  transitionTimeMs;
    NvBool bUncalibrated;
    NvBool bAdaptiveBrightness;
    NvBool bBoostRange;
    NvU32  ambientIlluminance;
    NvU32  ambientChromaticityX;
    NvU32  ambientChromaticityY;
    NvBool bEnhanceContrast;
    NvU16  contrastGain;
    NvU16  detailGain;
    NvBool bContentAdaptiveBrightness;
    NvBool bDynamicHdrTonemapping;
    NvU32  maxDisplayLuminance;
    NvU32  luminanceScalingFactor;
} NV0073_CTRL_SPECIFIC_DISPLAY_BRIGHTNESS_LTM_PARAMS;

#define NV0073_CTRL_SPECIFIC_GET_DISPLAY_BRIGHTNESS_LTM_PARAMS_MESSAGE_ID (0xAFU)

typedef NV0073_CTRL_SPECIFIC_DISPLAY_BRIGHTNESS_LTM_PARAMS NV0073_CTRL_SPECIFIC_GET_DISPLAY_BRIGHTNESS_LTM_PARAMS;

#define NV0073_CTRL_CMD_SPECIFIC_GET_DISPLAY_BRIGHTNESS_LTM (0x7302afU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_DISPLAY_BRIGHTNESS_LTM_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_SET_DISPLAY_BRIGHTNESS_LTM_PARAMS_MESSAGE_ID (0xB0U)

typedef NV0073_CTRL_SPECIFIC_DISPLAY_BRIGHTNESS_LTM_PARAMS NV0073_CTRL_SPECIFIC_SET_DISPLAY_BRIGHTNESS_LTM_PARAMS;

#define NV0073_CTRL_CMD_SPECIFIC_SET_DISPLAY_BRIGHTNESS_LTM    (0x7302b0U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_SET_DISPLAY_BRIGHTNESS_LTM_PARAMS_MESSAGE_ID" */

/*
* NV0073_CTRL_CMD_GET_REGISTER_OFFSET_FOR_ULMB_TIMESTAMP
*
* The command returns the offset of the disp registers for sending timestamp
* directly to RISCV, so that clients may map them directly and write to this
* register which will trigger interrupt in RISCV.
*
* Possible status values returned are: 
*   NV_OK
*   NV_ERR_NOT_SUPPORTED
*/

#define NV0073_CTRL_CMD_GET_REGISTER_OFFSET_FOR_ULMB_TIMESTAMP (0x7302b1U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_GET_REGISTER_OFFSET_FOR_ULMB_TIMESTAMP_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_GET_REGISTER_OFFSET_FOR_ULMB_TIMESTAMP_PARAMS_MESSAGE_ID (0xB1U)

typedef struct NV0073_CTRL_GET_REGISTER_OFFSET_FOR_ULMB_TIMESTAMP_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 dispRegisterBase;
} NV0073_CTRL_GET_REGISTER_OFFSET_FOR_ULMB_TIMESTAMP_PARAMS;
/* _ctrl0073specific_h_ */
