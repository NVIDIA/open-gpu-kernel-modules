/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      class/cl0050.finn
//

#include "nvcfg_sdk.h"

#define NV_CE_UTILS (0x50U) /* finn: Evaluated from "NV0050_ALLOCATION_PARAMETERS_MESSAGE_ID" */

#define NV0050_ALLOCATION_PARAMETERS_MESSAGE_ID (0x0050U)

typedef struct NV0050_ALLOCATION_PARAMETERS {
    NvHandle hVaspace;
    NV_DECLARE_ALIGNED(NvU64 flags, 8);
    NvU32    forceCeId;
} NV0050_ALLOCATION_PARAMETERS;


// Whether the CeUtils will allocate everything with RM client or external client
#define NV0050_CEUTILS_FLAGS_EXTERNAL                0:0
#define NV0050_CEUTILS_FLAGS_EXTERNAL_FALSE     (0x00000000)
#define NV0050_CEUTILS_FLAGS_EXTERNAL_TRUE      (0x00000001)

// Whether CeUtils will use virtual copy
#define NV0050_CEUTILS_FLAGS_VIRTUAL_MODE            1:1
#define NV0050_CEUTILS_FLAGS_VIRTUAL_MODE_FALSE (0x00000000)
#define NV0050_CEUTILS_FLAGS_VIRTUAL_MODE_TRUE  (0x00000001)

// Whether the CeUtils is using fifo lite mode. Has to be internal
#define NV0050_CEUTILS_FLAGS_FIFO_LITE               2:2
#define NV0050_CEUTILS_FLAGS_FIFO_LITE_FALSE    (0x00000000)
#define NV0050_CEUTILS_FLAGS_FIFO_LITE_TRUE     (0x00000001)

// Whether the CeUtils will use BAR1 or BAR2 for data copy
#define NV0050_CEUTILS_FLAGS_NO_BAR1_USE             3:3
#define NV0050_CEUTILS_FLAGS_NO_BAR1_USE_FALSE  (0x00000000)
#define NV0050_CEUTILS_FLAGS_NO_BAR1_USE_TRUE   (0x00000001)

// Force a specific CE engine to be used be setting forceCeId
#define NV0050_CEUTILS_FLAGS_FORCE_CE_ID             4:4
#define NV0050_CEUTILS_FLAGS_FORCE_CE_ID_FALSE  (0x00000000)
#define NV0050_CEUTILS_FLAGS_FORCE_CE_ID_TRUE   (0x00000001)

// Use a CC secure channel
#define NV0050_CEUTILS_FLAGS_CC_SECURE             5:5
#define NV0050_CEUTILS_FLAGS_CC_SECURE_FALSE    (0x00000000)
#define NV0050_CEUTILS_FLAGS_CC_SECURE_TRUE     (0x00000001)

