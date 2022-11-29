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
// Source file: ctrl/ctrlc370/ctrlc370base.finn
//

#include "ctrl/ctrlxxxx.h"
/* NVC370_DISPLAY control commands and parameters */

#define NVC370_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0XC370, NVC370_CTRL_##cat, idx)

/* NVC370_DISPLAY command categories (6bits) */
#define NVC370_CTRL_RESERVED (0x00)
#define NVC370_CTRL_CHNCTL   (0x01)
#define NVC370_CTRL_RG       (0x02)
#define NVC370_CTRL_SEQ      (0x03)
#define NVC370_CTRL_OR       (0x04)
#define NVC370_CTRL_INST     (0x05)
#define NVC370_CTRL_VERIF    (0x06)
#define NVC370_CTRL_SYSTEM   (0x07)
#define NVC370_CTRL_EVENT    (0x09)

// This struct must be the first member of all C370 control calls
typedef struct NVC370_CTRL_CMD_BASE_PARAMS {
    NvU32 subdeviceIndex;
} NVC370_CTRL_CMD_BASE_PARAMS;


/*
 * NVC370_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVC370_CTRL_CMD_NULL (0xc3700000) /* finn: Evaluated from "(FINN_NVC370_DISPLAY_RESERVED_INTERFACE_ID << 8) | 0x0" */


/* _ctrlc370base_h_ */
