/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      class/clb1cc.finn
//

#include "clb0cc.h"

#define MAXWELL_PROFILER_CONTEXT (0xb1ccU) /* finn: Evaluated from "NVB1CC_ALLOC_PARAMETERS_MESSAGE_ID" */

/*
 * Creating the MAXWELL_PROFILER_CONTEXT object:
 * - The profiler object is instantiated as a child of either a bc channel
 *   group or bc channel.
 */
#define NVB1CC_ALLOC_PARAMETERS_MESSAGE_ID (0xb1ccU)

typedef struct NVB1CC_ALLOC_PARAMETERS {
    /*
     * Handle of a specific subdevice of a broadcast device.
     */
    NvHandle hSubDevice;
} NVB1CC_ALLOC_PARAMETERS;

