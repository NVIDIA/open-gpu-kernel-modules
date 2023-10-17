/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2016,2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

 /***************************************************************************\
|*                                                                           *|
|*                        NV Display Common Types                            *|
|*                                                                           *|
|*  <nvdisptypes.h>  defines the common display types.                       *|
|*                                                                           *|
 \***************************************************************************/

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      nvdisptypes.finn
//




#include "nvtypes.h"



typedef enum NV_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP {
    NV_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_DEFAULT = 0,
    NV_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_16_422 = 1,
    NV_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_18_444 = 2,
    NV_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_20_422 = 3,
    NV_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_24_422 = 4,
    NV_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_24_444 = 5,
    NV_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_30_444 = 6,
    NV_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_32_422 = 7,
    NV_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_36_444 = 8,
    NV_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_48_444 = 9,
} NV_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP;



typedef NvU32 NV_DISP_LOCK_PIN;

#define NV_DISP_LOCK_PIN_0           0x0
#define NV_DISP_LOCK_PIN_1           0x1
#define NV_DISP_LOCK_PIN_2           0x2
#define NV_DISP_LOCK_PIN_3           0x3
#define NV_DISP_LOCK_PIN_4           0x4
#define NV_DISP_LOCK_PIN_5           0x5
#define NV_DISP_LOCK_PIN_6           0x6
#define NV_DISP_LOCK_PIN_7           0x7
#define NV_DISP_LOCK_PIN_8           0x8
#define NV_DISP_LOCK_PIN_9           0x9
#define NV_DISP_LOCK_PIN_A           0xA
#define NV_DISP_LOCK_PIN_B           0xB
#define NV_DISP_LOCK_PIN_C           0xC
#define NV_DISP_LOCK_PIN_D           0xD
#define NV_DISP_LOCK_PIN_E           0xE
#define NV_DISP_LOCK_PIN_F           0xF

// Value used solely for HW initialization
#define NV_DISP_LOCK_PIN_UNSPECIFIED 0x10



typedef NvU32 NV_DISP_LOCK_MODE;

#define NV_DISP_LOCK_MODE_NO_LOCK     0x0
#define NV_DISP_LOCK_MODE_FRAME_LOCK  0x1
#define NV_DISP_LOCK_MODE_RASTER_LOCK 0x3

