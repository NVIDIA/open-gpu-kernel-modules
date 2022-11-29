/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * @file  nvdm_payload_cmd_response.h
 * @brief Header file containing the definition for the
 *        NVDM_PAYLOAD_COMMAND_RESPONSE structure, which
 *        is required for SMBPBI-specific payloads and needs
 *        to be visible by bothe the PMU and the driver sides.
 */

#ifndef _NVDM_PAYLOAD_CMD_RESPONSE_H
#define _NVDM_PAYLOAD_CMD_RESPONSE_H

#pragma pack(1)
typedef struct
{
    NvU32 taskId;
    NvU32 commandNvdmType;
    NvU32 errorCode;
} NVDM_PAYLOAD_COMMAND_RESPONSE;
#pragma pack()

#endif // _NVDM_PAYLOAD_CMD_RESPONSE_H
