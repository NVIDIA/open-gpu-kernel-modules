/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0073/ctrl0073common.finn
//



/*
 *   DSC caps -
 *      bDscSupported
 *          If GPU supports DSC or not
 *
 *      encoderColorFormatMask
 *          Mask of all color formats for which DSC
 *          encoding is supported by GPU
 *
 *      lineBufferSizeKB
 *          Size of line buffer.
 *
 *      rateBufferSizeKB
 *          Size of rate buffer per slice.
 *
 *      bitsPerPixelPrecision
 *          Bits per pixel precision for DSC e.g. 1/16, 1/8, 1/4, 1/2, 1bpp
 *
 *      maxNumHztSlices
 *          Maximum number of horizontal slices supported by DSC encoder
 *
 *      lineBufferBitDepth
 *          Bit depth used by the GPU to store the reconstructed pixels within
 *          the line buffer
 */
#define NV0073_CTRL_CMD_DSC_CAP_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0073_CTRL_CMD_DSC_CAP_PARAMS {
    NvBool bDscSupported;
    NvU32  encoderColorFormatMask;
    NvU32  lineBufferSizeKB;
    NvU32  rateBufferSizeKB;
    NvU32  bitsPerPixelPrecision;
    NvU32  maxNumHztSlices;
    NvU32  lineBufferBitDepth;
} NV0073_CTRL_CMD_DSC_CAP_PARAMS;

/*
 * NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior
 *   cmd
 *     This parameter is an input to this command.
 *     Here are the current defined fields:
 *       NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD_CMD_POWER
 *         Set to specify what operation to run.
 *           NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD_CMD_POWER_UP
 *             Request to power up pad.
 *           NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD_CMD_POWER_DOWN
 *             Request to power down the pad.
 *   linkBw
 *     This parameter is used to pass in the link bandwidth required to run the
 *     power up sequence. Refer enum DM_FRL_LINK_RATE_GBPS for valid values.
 *   laneCount
 *     This parameter is used to pass the lanecount.
 *   sorIndex
 *     This parameter is used to pass the SOR index.
 *   padlinkIndex
 *     This parameter is used to pass the padlink index for primary link.
 *     Please refer enum DFPPADLINK for valid index values for Link A~F.
 */

#define NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD (0x730502U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_COMMON_INTERFACE_ID << 8) | NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 cmd;
    NvU32 linkBw;
    NvU32 laneCount;
    NvU32 sorIndex;
    NvU32 padlinkIndex;
} NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD_PARAMS;

#define NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD_CMD_POWER                        0:0
#define NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD_CMD_POWER_UP   (0x00000000U)
#define NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD_CMD_POWER_DOWN (0x00000001U)

/* _ctrl0073common_h_ */
