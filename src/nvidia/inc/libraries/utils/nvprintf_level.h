/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NVPRINTF_LEVEL_H
#define NVPRINTF_LEVEL_H

/// @defgroup NV_PRINTF_LEVELS Printf verbosity levels
/// @{
/// @brief Prints at this level are discarded
#define LEVEL_SILENT   0x0
/// @brief Verbose debug logging level        (e.g. signaling function entry)
#define LEVEL_INFO     0x1
/// @brief Standard debug logging level       (e.g. Illegal ctrcall call)
#define LEVEL_NOTICE   0x2
/// @brief Warning logging level              (e.g. feature not supported)
#define LEVEL_WARNING  0x3
/// @brief Error logging level                (e.g. resource allocation failed)
#define LEVEL_ERROR    0x4
/// @brief Recoverable HW error               (e.g. RC events)
#define LEVEL_HW_ERROR 0x5
/// @brief Unrecoverable error                (e.g. Bus timeout)
#define LEVEL_FATAL    0x6
/// @}

#define NV_LEVEL_MAX   LEVEL_FATAL

/**
 * @def NV_PRINTF_LEVEL_ENABLED(level)
 * @brief This macro evaluates to 1 if prints of a given level will be compiled.
 *
 * By default, it is available on all builds that allow strings
 */
#ifndef NV_PRINTF_LEVEL_ENABLED
#define NV_PRINTF_LEVEL_ENABLED(level)  ((level) >= NV_PRINTF_LEVEL)
#endif

// Values for NV_LOG_SPECIAL to specify how log entries are encoded.
#define RM_GSP_LOG_SPECIAL_NONE 0x0
#define RM_GSP_LOG_SPECIAL_ASSERT_FAILED 0x1
#define RM_GSP_LOG_SPECIAL_ASSERT_OK_FAILED 0x2
#define RM_GSP_LOG_SPECIAL_CHECK_FAILED 0x3
#define RM_GSP_LOG_SPECIAL_CHECK_OK_FAILED 0x4

#endif // NVPRINTF_LEVEL_H
