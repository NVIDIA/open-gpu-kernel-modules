/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvtypes.h"

#ifndef _clb0b5sw_h_
#define _clb0b5sw_h_

#ifdef __cplusplus
extern "C" {
#endif

/* This file is *not* auto-generated. */

//
// Using VERSION_0 will cause the API to interpret 
// engineType as a CE engine instance.  This allows
// for backward compatibility with 85B5sw and 90B5sw.
//
#define NVB0B5_ALLOCATION_PARAMETERS_VERSION_0  0

//
// Using VERSION_1 will cause the API to interpret 
// engineType as an NV2080_ENGINE_TYPE ordinal.
//
#define NVB0B5_ALLOCATION_PARAMETERS_VERSION_1  1

typedef struct
{
    NvU32    version;
    NvU32    engineType;
} NVB0B5_ALLOCATION_PARAMETERS;

#ifdef __cplusplus
};     /* extern "C" */
#endif
#endif // _clb0b5sw_h_

