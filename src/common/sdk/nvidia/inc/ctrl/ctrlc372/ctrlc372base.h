/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlc372/ctrlc372base.finn
//

#include "ctrl/ctrlxxxx.h"
/* NVC372_DISPLAY_SW control commands and parameters */

#define NVC372_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0XC372, NVC372_CTRL_##cat, idx)

/* NVC372_DISPLAY_SW command categories (6 bits) */
#define NVC372_CTRL_RESERVED (0x00)
#define NVC372_CTRL_CHNCTL   (0x01)
#define NVC372_CTRL_VERIF    (0x02)

/*
 * NVC372_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVC372_CTRL_CMD_NULL (0xc3720000) /* finn: Evaluated from "(FINN_NVC372_DISPLAY_SW_RESERVED_INTERFACE_ID << 8) | 0x0" */


// This struct must be the first member of all
// C372 control calls
typedef struct NVC372_CTRL_CMD_BASE_PARAMS {
    NvU32 subdeviceIndex;
} NVC372_CTRL_CMD_BASE_PARAMS;

/* _ctrlc372base_h_ */
