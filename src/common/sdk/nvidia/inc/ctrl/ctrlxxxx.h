/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlxxxx.finn
//



#include "nvtypes.h"

/* definitions shared by all CTRL interfaces */

/* Basic command format:
*   cmd_class       [31:16],
*   cmd_reserved    [15:15],
*   cmd_reserved    [14:14],
*   cmd_category    [13:8],
*   cmd_index       [7:0]
*/

#define NVXXXX_CTRL_CMD_CLASS                                             31:16

#define NVXXXX_CTRL_CMD_CATEGORY                                           13:8
#define NVXXXX_CTRL_CMD_INDEX                                               7:0

/* don't use DRF_NUM - not always available */
#  define NVXXXX_CTRL_CMD(cls,cat,idx)     \
                               (((cls) << 16) | ((0) << 15) | ((0) << 14) \
                               | ((cat) << 8) | ((idx) & 0xFF))
/*
 * NVXXXX_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 * This command is valid for all classes.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVXXXX_CTRL_CMD_NULL              (0x00000000)

#define NVxxxx_CTRL_LEGACY_PRIVILEGED     (0xC0)
#define NVxxxx_CTRL_LEGACY_NON_PRIVILEGED (0x80)

typedef struct NVXXXX_CTRL_XXX_INFO {
    NvU32 index;
    NvU32 data;
} NVXXXX_CTRL_XXX_INFO;
