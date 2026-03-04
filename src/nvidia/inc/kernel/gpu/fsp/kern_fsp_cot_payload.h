/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef KERN_FSP_COT_PAYLOAD_H
#define KERN_FSP_COT_PAYLOAD_H

#pragma pack(1)
typedef struct
{
    NvU32 constBlob;
    NvU8 msgType;
    NvU16 vendorId;
} MCTP_HEADER;

// Needed to remove unnecessary padding
#pragma pack(1)
typedef struct
{
    NvU16 version;
    NvU16 size;
    NvU64 gspFmcSysmemOffset;
    NvU64 frtsSysmemOffset;
    NvU32 frtsSysmemSize;

    // Note this is an offset from the end of FB
    NvU64 frtsVidmemOffset;
    NvU32 frtsVidmemSize;

    // Authentication related fields
    NvU32 hash384[12];
    NvU32 publicKey[96];
    NvU32 signature[96];

    NvU64 gspBootArgsSysmemOffset;
} NVDM_PAYLOAD_COT;
#pragma pack()

#endif // KERN_FSP_COT_PAYLOAD_H
