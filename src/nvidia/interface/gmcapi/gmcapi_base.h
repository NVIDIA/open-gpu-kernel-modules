/*
 * SPDX-FileCopyrightText: Copyright (c) 2025-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GMCAPI_BASE_H
#define GMCAPI_BASE_H

typedef struct NV_ABI_STABLE GMCAPI_HEADER
{
    NvU32 command;  // see GMCAPI_HEADER_COMMAND_*
    NvU32 size;     // Size of payload, request or response
    NvU64 sequence; // Unique identifier, response same as for request
    union
    {
        struct
        {
            NvU32 max_response_size;
        } request;
        struct
        {
            NV_STATUS status;
        } response;
    };
    NvU32 reserved[5]; // Must be set to zero
    NvU8  data[];
} GMCAPI_HEADER;

#define GMCAPI_HEADER_COMMAND_ID_MASK              0xFFFFFFu
#define GMCAPI_HEADER_COMMAND_ID(cmd)              ((cmd) & GMCAPI_HEADER_COMMAND_ID_MASK)

#define GMCAPI_HEADER_COMMAND_FLAGS_MASK           0xFF000000u
#define GMCAPI_HEADER_COMMAND_FLAGS(cmd)           ((cmd) & GMCAPI_HEADER_COMMAND_FLAGS_MASK)
// Bits in the FLAGS field that are not currently defined as valid flags.
#define GMCAPI_HEADER_COMMAND_FLAGS_INVALID_MASK   \
    (GMCAPI_HEADER_COMMAND_FLAGS_MASK & ~GMCAPI_HEADER_COMMAND_FLAGS_RESPONSE)

enum NV_ABI_STABLE GMCAPI_HEADER_COMMAND_FLAGS
{
    // This is a response message, not a request.
    GMCAPI_HEADER_COMMAND_FLAGS_RESPONSE = 0x01000000u,
};


enum NV_ABI_STABLE GMCAPI_FLAGS
{
    //
    // Callers shall not wait on the response from this API.
    // RPC infrastructure will not send any response back, but may log failures.
    //
    GMCAPI_FLAGS_FIRE_AND_FORGET                       = 0x00000001,

    GMCAPI_FLAGS_NONE                                  = 0
};

enum NV_ABI_STABLE GMCAPI_COMMANDS
{
    //
    // Numeric IDs <= 0xFFFF are reserved.
    // This ensures no collisions with the 'function' constant of the VGPU style RPC
    //
    GMCAPI_CMD_RESERVED_LAST = 0xFFFF,

#define GMCAPI(numeric_id, symbolic_id, handler_func, input_type, output_type, flags, ...) \
    GMCAPI_CMD_ ## symbolic_id = numeric_id,

#include "gmcapi/gmcapi_table.h"

    GMCAPI_CMD_INVALID = 0xFFFFFFFF
};

#endif // GMCAPI_BASE_H
