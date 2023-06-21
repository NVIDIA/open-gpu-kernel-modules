/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      class/clb2cc.finn
//

#include "clb0cc.h"

#define MAXWELL_PROFILER_DEVICE (0xb2ccU) /* finn: Evaluated from "NVB2CC_ALLOC_PARAMETERS_MESSAGE_ID" */

/*
 * Creating the MAXWELL_PROFILER_DEVICE object:
 * - The profiler object is instantiated as a child of subdevice.
 */
#define NVB2CC_ALLOC_PARAMETERS_MESSAGE_ID (0xb2ccU)

typedef struct NVB2CC_ALLOC_PARAMETERS {
    /*
     * This parameter specifies the handle of the client that owns the context
     * specified by hContextTarget. This can set it to 0 where a context
     * specific operation is not needed. For context level operations see:
     * @ref NVB0CC_CTRL_CMD_RESERVE_HWPM_LEGACY, @ref NVB0CC_CTRL_CMD_RESERVE_PM_AREA_SMPC,
     * @ref NVB0CC_CTRL_CMD_ALLOC_PMA_STREAM.
     */
    NvHandle hClientTarget;

    /*
     * This parameter specifies the handle of the BC channel (or BC channel
     * group) object instance to which context-specific operations are to be
     * directed. If hClientTarget is set to 0 then this parameter is ignored.
     */
    NvHandle hContextTarget;
} NVB2CC_ALLOC_PARAMETERS;

