/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      class/cl0000.finn
//

#include "nvlimits.h"
#include "cl0000_notification.h"

/* object NV01_NULL_OBJECT */
#define NV01_NULL_OBJECT (0x0) /* finn: Evaluated from "NV0000_ALLOC_PARAMETERS_MESSAGE_ID" */

/* obsolete alises */
#define NV1_NULL_OBJECT  NV01_NULL_OBJECT

#define NV01_ROOT        (0x0U) /* finn: Evaluated from "NV0000_ALLOC_PARAMETERS_MESSAGE_ID" */

/* NvAlloc parameteters */
#define NV0000_ALLOC_PARAMETERS_MESSAGE_ID (0x0000U)

typedef struct NV0000_ALLOC_PARAMETERS {
    NvHandle hClient; /* CORERM-2934: hClient must remain the first member until all allocations use these params */
    NvU32    processID;
    char     processName[NV_PROC_NAME_MAX_LENGTH];
    NV_DECLARE_ALIGNED(NvP64 pOsPidInfo, 8);
} NV0000_ALLOC_PARAMETERS;

