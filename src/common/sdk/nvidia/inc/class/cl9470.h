/*
 * Copyright (c) 1993-2022, NVIDIA CORPORATION. All rights reserved.
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
// Source file:      class/cl9470.finn
//

#define NV9470_DISPLAY (0x9470U) /* finn: Evaluated from "NV9470_ALLOCATION_PARAMETERS_MESSAGE_ID" */

#define NV9470_ALLOCATION_PARAMETERS_MESSAGE_ID (0x9470U)

typedef struct NV9470_ALLOCATION_PARAMETERS {
    NvU32 numHeads; // Number of HEADs in this chip/display
    NvU32 numDacs;  // Number of DACs in this chip/display
    NvU32 numSors;  // Number of SORs in this chip/display
    NvU32 numPiors; // Number of PIORs in this chip/display
} NV9470_ALLOCATION_PARAMETERS;
