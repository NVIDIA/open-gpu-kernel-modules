/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
/* _ctrl0073common_h_ */
