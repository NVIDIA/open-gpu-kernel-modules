/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: class/cl30f1.finn
//

#include "cl30f1_notification.h"

/* class NV30_GSYNC */
#define NV30_GSYNC                       (0x000030F1)

#define NV30F1_GSYNC_CONNECTOR_ONE       (0)
#define NV30F1_GSYNC_CONNECTOR_TWO       (1)
#define NV30F1_GSYNC_CONNECTOR_THREE     (2)
#define NV30F1_GSYNC_CONNECTOR_FOUR      (3)

#define NV30F1_GSYNC_CONNECTOR_PRIMARY   NV30F1_GSYNC_CONNECTOR_ONE
#define NV30F1_GSYNC_CONNECTOR_SECONDARY NV30F1_GSYNC_CONNECTOR_TWO

#define NV30F1_GSYNC_CONNECTOR_COUNT     (4)

/* NvRmAlloc parameters */
#define NV30F1_MAX_GSYNCS                (0x0000004)

#define NV30F1_ALLOC_PARAMETERS_MESSAGE_ID (0x30f1U)

typedef struct NV30F1_ALLOC_PARAMETERS {
    NvU32 gsyncInstance;
} NV30F1_ALLOC_PARAMETERS;

