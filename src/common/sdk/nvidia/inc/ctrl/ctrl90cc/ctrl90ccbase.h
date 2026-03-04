/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl90cc/ctrl90ccbase.finn
//

#include "ctrl/ctrlxxxx.h"
/* GF100_PROFILER control commands and parameters */

#define NV90CC_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0x90CC, NV90CC_CTRL_##cat, idx)

/* GF100_PROFILER command categories (6 bits) */
#define NV90CC_CTRL_RESERVED (0x00)
#define NV90CC_CTRL_HWPM     (0x01)


#define NV90CC_CTRL_POWER    (0x03)

/*
 * NV90CC_CTRL_CMD_NULL
 *
 *    This command does nothing.
 *    This command does not take any parameters.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV90CC_CTRL_CMD_NULL (0x90cc0000) /* finn: Evaluated from "(FINN_GF100_PROFILER_RESERVED_INTERFACE_ID << 8) | 0x0" */

/* _ctrl90ccbase_h_ */
