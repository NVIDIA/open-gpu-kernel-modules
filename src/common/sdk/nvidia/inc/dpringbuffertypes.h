/*
 * SPDX-FileCopyrightText: Copyright (c) 2002-2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 
#ifndef INCLUDED_DPRINGBUFFERTYPES_H
#define INCLUDED_DPRINGBUFFERTYPES_H

#define MAX_MESSAGE_LEN     100
#define MAX_RECORD_COUNT    15

typedef enum _DP_RECORD_TYPE
{
    ASSERT_HIT           = 135,
    LOG_CALL             = 136,
} DP_RECORD_TYPE;

typedef struct _DpAssertHitRecord
{
    NvU64 breakpointAddr;
} DpAssertHitRecord, *PDpAssertHitRecord;

typedef struct _DpLogCallRecord
{
    char    msg[MAX_MESSAGE_LEN];
    NvU64   addr;
} DpLogCallRecord, *PDpLogCallRecord;

typedef union _DpLogRecord
{
    DpAssertHitRecord   dpAssertHitRecord;
    DpLogCallRecord     dpLogCallRecord;
} DpLogRecord, *PDpLogRecord;

typedef enum _DpLogQueryType
{
    DpLogQueryTypeAssert    = 1,
    DpLogQueryTypeCallLog   = 2,
} DpLogQueryType, *PDpLogQueryType;

#endif //INCLUDED_DPRINGBUFFERTYPES_H
