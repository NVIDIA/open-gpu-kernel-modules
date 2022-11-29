/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl0073/ctrl0073stereo.finn
//

#include "ctrl/ctrl0073/ctrl0073base.h"



/*
 * NV0073_CTRL_CMD_STEREO_DONGLE_SET_TIMINGS
 *
 * Sets new video mode timings
 * E.g. from display driver on mode set
 *
 * Parameters:
 * [IN]  subDeviceInstance - This parameter specifies the subdevice instance 
 *        within the NV04_DISPLAY_COMMON parent device to which the operation 
 *        should be directed.  This parameter must specify a value between 
 *        zero and the total number of subdevices within the parent device.  
 *        This parameter should be set to zero for default behavior.
 * [IN]  head      - head to be passed to stereoDongleControl 
 * [IN]  timings   - new timings to be set
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED    - stereo is not initialized on the GPU
 */
#define NV0073_CTRL_CMD_STEREO_DONGLE_SET_TIMINGS (0x731703U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_STEREO_INTERFACE_ID << 8) | NV0073_CTRL_STEREO_DONGLE_SET_TIMINGS_PARAMS_MESSAGE_ID" */

typedef struct NV0073_CTRL_STEREO_VIDEO_MODE_TIMINGS {
    NvU32 PixelClock;
    NvU16 TotalWidth;
    NvU16 VisibleImageWidth;
    NvU16 HorizontalBlankStart;
    NvU16 HorizontalBlankWidth;
    NvU16 HorizontalSyncStart;
    NvU16 HorizontalSyncWidth;
    NvU16 TotalHeight;
    NvU16 VisibleImageHeight;
    NvU16 VerticalBlankStart;
    NvU16 VerticalBlankHeight;
    NvU16 VerticalSyncStart;
    NvU16 VerticalSyncHeight;
    NvU16 InterlacedMode;
    NvU16 DoubleScanMode;

    NvU16 MonitorVendorId;
    NvU16 MonitorProductId;
} NV0073_CTRL_STEREO_VIDEO_MODE_TIMINGS;

#define NV0073_CTRL_STEREO_DONGLE_SET_TIMINGS_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV0073_CTRL_STEREO_DONGLE_SET_TIMINGS_PARAMS {
    NvU32                                 subDeviceInstance;
    NvU32                                 head;
    NV0073_CTRL_STEREO_VIDEO_MODE_TIMINGS timings;
} NV0073_CTRL_STEREO_DONGLE_SET_TIMINGS_PARAMS;

/*
 * NV0073_CTRL_CMD_STEREO_DONGLE_ACTIVATE
 *
 * stereoDongleActivate wrapper / NV_STEREO_DONGLE_ACTIVATE_DATA_ACTIVE_YES
 * Updates sbios of 3D stereo state active
 *
 * Parameters:
 * [IN]  subDeviceInstance - This parameter specifies the subdevice instance 
 *        within the NV04_DISPLAY_COMMON parent device to which the operation 
 *        should be directed.  This parameter must specify a value between 
 *        zero and the total number of subdevices within the parent device.  
 *        This parameter should be set to zero for default behavior.
 * [IN]  head                   - head to be passed to stereoDongleActivate
 * [IN]  bSDA                   - enable stereo on DDC SDA
 * [IN]  bWorkStation           - is workstation stereo?
 * [IN]  bDLP                   - is checkerboard DLP Stereo?
 * [IN]  IRPower                - IR power value
 * [IN]  flywheel               - FlyWheel value
 * [IN]  bRegIgnore             - use reg?
 * [IN]  bI2cEmitter            - Sets NV_STEREO_DONGLE_ACTVATE_DATA_I2C_EMITTER_YES and pStereo->bAegisDT
 * [IN]  bForcedSupported       - Sets NV_STEREO_DONGLE_FORCED_SUPPORTED_YES and pStereo->GPIOControlledDongle
 * [IN]  bInfoFrame             - Aegis DT with DP InfoFrame
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT    - if (head > OBJ_MAX_HEADS)
 *   NV_ERR_NOT_SUPPORTED       - stereo is not initialized on the GPU
 */
#define NV0073_CTRL_CMD_STEREO_DONGLE_ACTIVATE (0x731704U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_STEREO_INTERFACE_ID << 8) | NV0073_CTRL_STEREO_DONGLE_ACTIVATE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_STEREO_DONGLE_ACTIVATE_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV0073_CTRL_STEREO_DONGLE_ACTIVATE_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  head;
    NvBool bSDA;
    NvBool bWorkStation;
    NvBool bDLP;
    NvU8   IRPower;
    NvU8   flywheel;
    NvBool bRegIgnore;
    NvBool bI2cEmitter;
    NvBool bForcedSupported;
    NvBool bInfoFrame;
} NV0073_CTRL_STEREO_DONGLE_ACTIVATE_PARAMS;

/*
 * NV0073_CTRL_CMD_STEREO_DONGLE_DEACTIVATE
 *
 * stereoDongleActivate wrapper / NV_STEREO_DONGLE_ACTIVATE_DATA_ACTIVE_NO
 *
 * If active count<=0 then no 3D app is running which indicates 
 * that we have really deactivated the stereo, updates sbios of 3D stereo state NOT ACTIVE.
 *
 * Parameters:
 * [IN]  subDeviceInstance - This parameter specifies the subdevice instance 
 *        within the NV04_DISPLAY_COMMON parent device to which the operation 
 *        should be directed.  This parameter must specify a value between 
 *        zero and the total number of subdevices within the parent device.  
 *        This parameter should be set to zero for default behavior.
 * [IN]  head                   - head to be passed to stereoDongleActivate
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT    - if (head > OBJ_MAX_HEADS)
 *   NV_ERR_NOT_SUPPORTED       - stereo is not initialized on the GPU
 */
#define NV0073_CTRL_CMD_STEREO_DONGLE_DEACTIVATE (0x731705U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_STEREO_INTERFACE_ID << 8) | NV0073_CTRL_STEREO_DONGLE_DEACTIVATE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_STEREO_DONGLE_DEACTIVATE_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV0073_CTRL_STEREO_DONGLE_DEACTIVATE_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 head;
} NV0073_CTRL_STEREO_DONGLE_DEACTIVATE_PARAMS;



/* _ctrl0073stereo_h_ */
