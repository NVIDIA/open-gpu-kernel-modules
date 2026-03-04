/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      alloc/alloc_access_counter_buffer.finn
//

#include "class/clc365.h"


/*
 * NV_ACCESS_COUNTER_NOTIFY_BUFFER_ALLOC_PARAMS
 *
 * Allocation params for ACCESS_COUNTER_NOTIFY_BUFFER.
 * This class provides an interface to access counter buffers.

 *    accessCounterIndex [IN]
 *       This parameter specifies index of access counter to which the object should be assigned
 *
 */
#define NV_ACCESS_COUNTER_NOTIFY_BUFFER_ALLOC_PARAMS_MESSAGE_ID (0xC365U)

typedef struct NV_ACCESS_COUNTER_NOTIFY_BUFFER_ALLOC_PARAMS {
    NvU32 accessCounterIndex;
} NV_ACCESS_COUNTER_NOTIFY_BUFFER_ALLOC_PARAMS;

