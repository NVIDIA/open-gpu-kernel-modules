/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _GPU_DEVICE_MAPPING_H_
#define _GPU_DEVICE_MAPPING_H_

// Defines the enum type DEVICE_INDEX used for identifying the device type being accessed
typedef enum
{
    DEVICE_INDEX_GPU = 0,
    DEVICE_INDEX_HOST1X,
    DEVICE_INDEX_DISPLAY,
    DEVICE_INDEX_DPAUX,
    DEVICE_INDEX_MC,
    DEVICE_INDEX_CLKRST,
    DEVICE_INDEX_MSS_NVLINK,
    DEVICE_INDEX_HDACODEC,
    DEVICE_INDEX_EMC,
    DEVICE_INDEX_FUSE,
    DEVICE_INDEX_KFUSE,
    DEVICE_INDEX_MIPICAL,
    DEVICE_INDEX_MAX  //Should always be the last entry
} DEVICE_INDEX;

typedef enum
{
    SOC_DEV_MAPPING_DISP = 0,
    SOC_DEV_MAPPING_DPAUX0,
    SOC_DEV_MAPPING_DPAUX1,  // Update NV_MAX_SOC_DPAUX_NUM_DEVICES if adding new DPAUX mappings
    SOC_DEV_MAPPING_HDACODEC,
    SOC_DEV_MAPPING_MIPICAL,
    SOC_DEV_MAPPING_MAX
} SOC_DEV_MAPPING;

#define GPU_MAX_DEVICE_MAPPINGS                   (60)

typedef struct
{
    DEVICE_INDEX deviceIndex;    // DEVICE_INDEX_*
    NvU32        devId;          // NV_DEVID_*
} DEVICE_ID_MAPPING;

#endif // _GPU_DEVICE_MAPPING_H_
