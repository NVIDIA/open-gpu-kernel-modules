/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_printf.h                                                       *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_PRINTF_H
#define INCLUDED_DP_PRINTF_H

#define DPLIB_LOG_BUFFER_SIZE 512

typedef enum
{
    DP_SILENT,
    DP_INFO,
    DP_NOTICE,
    DP_WARNING,
    DP_ERROR,
    DP_HW_ERROR,
    DP_FATAL,
} DP_LOG_LEVEL;

#if defined(_DEBUG) || defined(DEBUG)
#define DP_PRINTF(severity, format, ...) dpPrintf(severity, format, ##__VA_ARGS__)
#else
#define DP_PRINTF(severity, format, ...)
#endif // _DEBUG || DEBUG

#endif // INCLUDED_DP_PRINTF_H
