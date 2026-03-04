/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES
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

#ifndef FSP_CAPS_QUERY_RPC_H
#define FSP_CAPS_QUERY_RPC_H

#include "fsp/nvdm_payload_cmd_response.h"

#pragma pack(1)

 /*!
  * @biief Capability query payload command to FSP
  */
typedef struct
{
    NvU8 subMessageId;
} FSP_CAPS_QUERY_RPC_PAYLOAD_PARAMS;

/*!
  * @brief Capability query response payload for FSP capability query
  */
typedef struct
{
    NvU8 nvdmType;
    NVDM_PAYLOAD_COMMAND_RESPONSE cmdResponse;
    NvU8 rspPayload[4];
} FSP_CAPS_QUERY_RESPONSE_PAYLOAD_PARAMS;

#pragma pack()

#endif // FSP_CAPS_QUERY_RPC_H
