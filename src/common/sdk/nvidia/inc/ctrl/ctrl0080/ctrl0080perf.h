/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl0080/ctrl0080perf.finn
//

#define NV0080_CTRL_PERF_SLI_GPU_BOOST_SYNC_CONTROL_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV0080_CTRL_PERF_SLI_GPU_BOOST_SYNC_CONTROL_PARAMS {
    NvBool bActivate;
} NV0080_CTRL_PERF_SLI_GPU_BOOST_SYNC_CONTROL_PARAMS;

#define NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS {
    NvBool bCudaLimit;
} NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS;



/* _ctrl0080perf_h_ */

