/*
 * SPDX-FileCopyrightText: Copyright (c) 2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _clc637_h_
#define _clc637_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

#define AMPERE_SMC_PARTITION_REF    (0x0000c637)

//
// This swizzId can be used by root clients like tools for device level
// profiling
//
#define NVC637_DEVICE_PROFILING_SWIZZID (0xFFFFFFFE)

//
// TODO: Deprecate NVC637_DEVICE_LEVEL_SWIZZID once all the clients are moved to
//       NVC637_DEVICE_PROFILING_SWIZZID
//
#define NVC637_DEVICE_LEVEL_SWIZZID NVC637_DEVICE_PROFILING_SWIZZID

/* NvRmAlloc parameters */
typedef struct {
    //
    // capDescriptor is a file descriptor for unix RM clients, but a void
    // pointer for windows RM clients.
    //
    // capDescriptor is transparent to RM clients i.e. RM's user-mode shim
    // populates this field on behalf of clients.
    //
    NV_DECLARE_ALIGNED(NvU64 capDescriptor, 8);

    NvU32 swizzId;
} NVC637_ALLOCATION_PARAMETERS;

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _clc637_h_ */

