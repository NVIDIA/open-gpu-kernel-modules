/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      class/cl00f1.finn
//

#define NV_IMEX_SESSION (0xf1U) /* finn: Evaluated from "NV00F1_ALLOCATION_PARAMETERS_MESSAGE_ID" */

#define NV00F1_ALLOC_FLAGS_DEFAULT             0

//
// Disables channel recovery on IMEX shutdown. This can lead to security
// issues. The flag is intended to be used for testing purpose only.
//
#define NV00F1_ALLOC_DISABLE_CHANNEL_RECOVERY  NVBIT(0)

#define NV00F1_ALLOCATION_PARAMETERS_MESSAGE_ID (0x00f1U)

typedef struct NV00F1_ALLOCATION_PARAMETERS {
    //
    // capDescriptor is a file descriptor for UNIX RM clients, but a void
    // pointer for windows RM clients.
    //
    // capDescriptor is transparent to RM clients i.e. RM's user-mode shim
    // populates this field on behalf of clients.
    //
    NV_DECLARE_ALIGNED(NvU64 capDescriptor, 8);

    // See NV00F1_ALLOC_FLAGS_xxx
    NvU32 flags;

    // OS event handle created with NvRmAllocOsEvent().
    NV_DECLARE_ALIGNED(NvP64 pOsEvent, 8);

    // Node ID which represent this OS/RM instance
    NvU16 nodeId;
} NV00F1_ALLOCATION_PARAMETERS;

