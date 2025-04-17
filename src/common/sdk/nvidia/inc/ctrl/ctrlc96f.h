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

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrlc96f.finn
//



#include "nvcfg_sdk.h"



/* BLACKWELL_CHANNEL_GPFIFO_A control commands and parameters */

#include "ctrl/ctrlxxxx.h"
#include "ctrl/ctrlc36f.h" // This control call interface is an ALIAS of C36F

#define NVC96F_CTRL_CMD(cat,idx)  \
    NVXXXX_CTRL_CMD(0xC36F, NVC96F_CTRL_##cat, idx)

/* BLACKWELL_CHANNEL_GPFIFO_A command categories (6bits) */
#define NVC96F_CTRL_RESERVED (0x00)
#define NVC96F_CTRL_GPFIFO   (0x01)

/*
 * NVC96F_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned is: NV_OK
*/
#define NVC96F_CTRL_CMD_NULL (NVC36F_CTRL_CMD_NULL)

