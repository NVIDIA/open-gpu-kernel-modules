/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl503c_h_
#define _cl503c_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

#define   NV50_THIRD_PARTY_P2P                          (0x0000503c)

/* NvRmAlloc parameters */
typedef struct {
    NvU32 flags;
} NV503C_ALLOC_PARAMETERS;

#define NV503C_ALLOC_PARAMETERS_FLAGS_TYPE                      1:0
#define NV503C_ALLOC_PARAMETERS_FLAGS_TYPE_PROPRIETARY  (0x00000000)
#define NV503C_ALLOC_PARAMETERS_FLAGS_TYPE_BAR1         (0x00000001)
#define NV503C_ALLOC_PARAMETERS_FLAGS_TYPE_NVLINK       (0x00000002)

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl503c_h_ */
