/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl2080/ctrl2080hshub.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"

/*
 * NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK
 *
 * This command get active HSHUB masks.
 *
 *   hshubNcisocMask
 *     NCISOC enabled active HSHUBs
 *   hshubNvlMask
 *     NVLINK capable active HSHUBs.
 */

#define NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK_PARAMS {
    NvU32 hshubNcisocMask;
    NvU32 hshubNvlMask;
} NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK_PARAMS;

#define NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK (0x20804101) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_HSHUB_INTERFACE_ID << 8) | NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK_PARAMS_MESSAGE_ID" */

/* _ctrl2080hshub_h_ */
