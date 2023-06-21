/*
 * Copyright (c) 2020-2022, NVIDIA CORPORATION. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      class/cl00c3.finn
//

#define NV01_MEMORY_SYNCPOINT (0xc3U) /* finn: Evaluated from "NV_MEMORY_SYNCPOINT_ALLOCATION_PARAMS_MESSAGE_ID" */

/*
 * NV_MEMORY_SYNCPOINT_ALLOCATION_PARAMS - Allocation params to create syncpoint
 * through NvRmAlloc.
 */
#define NV_MEMORY_SYNCPOINT_ALLOCATION_PARAMS_MESSAGE_ID (0x00c3U)

typedef struct NV_MEMORY_SYNCPOINT_ALLOCATION_PARAMS {
    NvU32 syncpointId;
} NV_MEMORY_SYNCPOINT_ALLOCATION_PARAMS;

