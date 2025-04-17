
#ifndef _G_KERN_HWPM_POWER_NVOC_H_
#define _G_KERN_HWPM_POWER_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef KERN_HWPM_POWER_H
#define KERN_HWPM_POWER_H

#include "nvtypes.h"

typedef struct HWPM_POWER_REQUEST_FEATURES_PARAMS
{
    NvU32 globalStatus;
    NvU32 controlMask;
    NvU32 statusMask;
} HWPM_POWER_REQUEST_FEATURES_PARAMS;

/* Valid fields for the controlMask and statusMask parameters */
#define HWPM_POWER_FEATURE_MASK_ELCG                                1:0
#define HWPM_POWER_FEATURE_MASK_BLCG                                3:2
#define HWPM_POWER_FEATURE_MASK_SLCG                                5:4

/*
 * The following are temporary fields for the controlMask and statusMask
 * parameters. They are required to reference count their respective features
 * until the existing RM controls can be safely updated, and the definitions
 * for these features will be removed soon after that.
 */
#define HWPM_POWER_FEATURE_MASK_ELPG                                7:6
#define HWPM_POWER_FEATURE_MASK_IDLE_SLOWDOWN                       9:8

/* Possible values for fields in the statusMask parameter */
#define HWPM_POWER_FEATURE_REQUEST_FULFILLED                        (0x00000000)
#define HWPM_POWER_FEATURE_REQUEST_REJECTED                         (0x00000001)
#define HWPM_POWER_FEATURE_REQUEST_NOT_SUPPORTED                    (0x00000002)
#define HWPM_POWER_FEATURE_REQUEST_FAILED                           (0x00000003)

#endif // KERN_HWPM_POWER_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_HWPM_POWER_NVOC_H_
