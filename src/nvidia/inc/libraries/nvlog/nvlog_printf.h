/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**
 * NvLog call that logs prints.
 *
 * This is the traditional NvLog component. When enabled, it will also activate
 * preprocessing of all source files to detect calls to NVLOG_PRINTF, and
 * generate a database to be used for decoding.
 *
 * This file just defines the macros used by NV_PRINTF and others clients
 */

#ifndef NVLOG_PRINTF_H
#define NVLOG_PRINTF_H

#include "utils/nvprintf_level.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NVLOG_ENABLED
/// @brief If zero, most of NvLog will be compiled out
#define NVLOG_ENABLED 0
#endif

#ifndef NVLOG_LEVEL
/// @brief Level below which all prints will be compiled out.
#define NVLOG_LEVEL LEVEL_NOTICE
#endif

/// @brief Maximum number of arguments to NVLOG_PRINTF
#define NVLOG_MAX_ARGS 20

/**
 * @brief Log this printf in NvLog internal binary buffers
 *
 * These calls are picked up by the NvLog parser, and are replaced with custom
 * calls from the generated header. See @page nvlog-parser for details.
 *
 * @param tag    - An identifier to help with offline filtering. Doesn't need to
 *                 be defined anywhere.
 * @param route  - 8bit mask of buffers the print will be routed to.
 *                 Use NVLOG_BUFFER_XXX constants
 * @param level  - Level at which to print. Calls with level < NVLOG_LEVEL will
 *                 be compiled out.
 * @param format - printf-like format string
 * @param ...    - printf arguments
 */
#define NVLOG_PRINTF(tag, route, level, format, ...) _NVLOG_PRINTF(tag, route, level, format, __VA_ARGS__)

#define NVLOG_BUFFER_NULL      0x01
#define NVLOG_BUFFER_RM        0x02
#define NVLOG_BUFFER_RM_BOOT   0x04
#define NVLOG_BUFFER_ETW       0x08
#define NVLOG_BUFFER_KMD_BOOT  0x10
#define NVLOG_BUFFER_KMD       0x20
#define NVLOG_BUFFER_ERROR     0x40
#define NVLOG_BUFFER_DD        0x80

#define NVLOG_ROUTE_RM  (NVLOG_BUFFER_RM | NVLOG_BUFFER_RM_BOOT | NVLOG_BUFFER_ETW)
#define NVLOG_ROUTE_KMD (NVLOG_BUFFER_KMD | NVLOG_BUFFER_KMD_BOOT | NVLOG_BUFFER_ETW)
#define NVLOG_ROUTE_DD  (NVLOG_BUFFER_DD | NVLOG_BUFFER_KMD_BOOT | NVLOG_BUFFER_ETW)

#include "nvlog/internal/nvlog_printf_internal.h"

#ifdef __cplusplus
} //extern "C"
#endif

#endif // NVLOG_PRINTF_H
