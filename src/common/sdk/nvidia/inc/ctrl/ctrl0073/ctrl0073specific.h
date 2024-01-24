/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#define NV0073_FAKE_DEVICE_SUPPORT_ENABLE                       0x11faU
#define NV0073_FAKE_DEVICE_SUPPORT_DISABLE                      0x99ceU
#define NV0073_FAKE_DEVICE_SUPPORT_ATTACH_DEVICES               0x100U
#define NV0073_FAKE_DEVICE_SUPPORT_REMOVE_DEVICES               0x101U



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

#define NV0073_CTRL_CMD_SPECIFIC_GET_I2C_PORTID                        (0x730211U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS_MESSAGE_ID (0x11U)

typedef struct NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 commPortId;
    NvU32 ddcPortId;
} NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS;

#define NV0073_CTRL_SPECIFIC_I2C_PORT_NONE            (0x0U)




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

#define NV0073_CTRL_CMD_SPECIFIC_GET_CONNECTOR_DATA   (0x730250U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_CONNECTOR_DATA_PARAMS_MESSAGE_ID" */

/* maximum number of connectors */
#define NV0073_CTRL_MAX_CONNECTORS                    4U

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
    NvBool bLtSkipped;
} NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_LINK_CONFIG_PARAMS;

#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE                                           2:0
#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_NONE                (0x00000000U)
#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_3LANES_3G           (0x00000001U)
#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_3LANES_6G           (0x00000002U)
#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_6G           (0x00000003U)
#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_8G           (0x00000004U)
#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_10G          (0x00000005U)
#define NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_12G          (0x00000006U)



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
} NV0073_CTRL_SPECIFIC_DISPLAY_BRIGHTNESS_LTM_PARAMS;

#define NV0073_CTRL_SPECIFIC_GET_DISPLAY_BRIGHTNESS_LTM_PARAMS_MESSAGE_ID (0xAFU)

typedef NV0073_CTRL_SPECIFIC_DISPLAY_BRIGHTNESS_LTM_PARAMS NV0073_CTRL_SPECIFIC_GET_DISPLAY_BRIGHTNESS_LTM_PARAMS;

#define NV0073_CTRL_CMD_SPECIFIC_GET_DISPLAY_BRIGHTNESS_LTM (0x7302afU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_GET_DISPLAY_BRIGHTNESS_LTM_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_SPECIFIC_SET_DISPLAY_BRIGHTNESS_LTM_PARAMS_MESSAGE_ID (0xB0U)

typedef NV0073_CTRL_SPECIFIC_DISPLAY_BRIGHTNESS_LTM_PARAMS NV0073_CTRL_SPECIFIC_SET_DISPLAY_BRIGHTNESS_LTM_PARAMS;

#define NV0073_CTRL_CMD_SPECIFIC_SET_DISPLAY_BRIGHTNESS_LTM (0x7302b0U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_SPECIFIC_INTERFACE_ID << 8) | NV0073_CTRL_SPECIFIC_SET_DISPLAY_BRIGHTNESS_LTM_PARAMS_MESSAGE_ID" */

/* _ctrl0073specific_h_ */
