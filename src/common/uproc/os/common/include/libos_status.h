/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef LIBOS_STATUS_H
#define LIBOS_STATUS_H

/**
 *  @brief Status codes for Libos kernel syscalls and standard daemons.
 * 
 */
// __packed__ forces this enum to use a minimal underlying type to represent this enum
// It is required for LibOS 3.x syscalls, otherwise it wouldn't fit into registers
// Note: Windows does not support this feature
typedef enum
__attribute__ ((__packed__))
{
  LibosOk                    = 0u,

  LibosErrorArgument         = 1u,
  LibosErrorAccess           = 2u,
  LibosErrorTimeout          = 3u,
  LibosErrorIncomplete       = 4u,
  LibosErrorFailed           = 5u,
  LibosErrorOutOfMemory      = 6u,
  LibosErrorSpoofed          = 7u,
  LibosErrorPortLost         = 8u,
  LibosErrorExhaustedHandles = 9u,
  LibosErrorShuttleReset     = 10u,
  LibosErrorNotDeviceMapped  = 11u,
  LibosErrorUnavailable      = 12u,
} LibosStatus;

#define LIBOS_OK LibosOk

#define LIBOS_ERROR_INVALID_ARGUMENT LibosErrorArgument
#define LIBOS_ERROR_INVALID_ACCESS   LibosErrorAccess
#define LIBOS_ERROR_TIMEOUT          LibosErrorTimeout
#define LIBOS_ERROR_INCOMPLETE       LibosErrorIncomplete
#define LIBOS_ERROR_OPERATION_FAILED LibosErrorFailed

#endif
