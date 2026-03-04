/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#if !defined(NV_IOCTL_LOCKLESS_DIAG)
#define NV_IOCTL_LOCKLESS_DIAG

#include <nvtypes.h>
#include "ctrl/ctrl0000/ctrl0000nvd.h"

typedef struct
{
    NvU32 cmd; // in
    NvU32 status; // out
    union // in/out
    {
        NV0000_CTRL_NVD_GET_NVLOG_INFO_PARAMS getNvlogInfo;
        NV0000_CTRL_NVD_GET_NVLOG_BUFFER_INFO_PARAMS getNvlogBufferInfo;
        NV0000_CTRL_NVD_GET_NVLOG_PARAMS getNvlog;
    } params;
} NV_LOCKLESS_DIAGNOSTIC_PARAMS;

#endif

