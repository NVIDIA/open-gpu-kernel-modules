/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file: ctrl/ctrl0073/ctrl0073base.finn
//

#include "ctrl/ctrlxxxx.h"
/* NV04_DISPLAY_COMMON control commands and parameters */

#define NV0073_CTRL_CMD(cat,idx)                NVXXXX_CTRL_CMD(0x0073, NV0073_CTRL_##cat, idx)

/* NV04_DISPLAY_COMMON command categories (6bits) */
#define NV0073_CTRL_RESERVED (0x00U)
#define NV0073_CTRL_SYSTEM   (0x01U)
#define NV0073_CTRL_SPECIFIC (0x02U)
#define NV0073_CTRL_EVENT    (0x03U)
#define NV0073_CTRL_INTERNAL (0x04U)
#define NV0073_CTRL_DFP      (0x11U)
#define NV0073_CTRL_DP       (0x13U)
#define NV0073_CTRL_SVP      (0x14U)
#define NV0073_CTRL_DPU      (0x15U)
#define NV0073_CTRL_PSR      (0x16U)
#define NV0073_CTRL_STEREO   (0x17U)

/*
 * NV0073_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV0073_CTRL_CMD_NULL (0x730000U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_RESERVED_INTERFACE_ID << 8) | 0x0" */

/* _ctrl0073base_h_ */
