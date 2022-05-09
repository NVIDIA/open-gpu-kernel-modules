/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _INFOROM_TYPES_H_
#define _INFOROM_TYPES_H_

/*!
 * @file  types.h
 * @brief Common types and definitions used by InfoROM objects
 */

typedef NvS32 inforom_S008;
typedef NvU32 inforom_U004;
typedef NvU32 inforom_U008;
typedef NvU32 inforom_U016;
typedef NvU32 inforom_U024;
typedef NvU32 inforom_U032;
typedef NvU64 inforom_U064;
typedef NvU8  inforom_X008;

#define INFOROM_FMT_S08     's'
#define INFOROM_FMT_U04     'n'
#define INFOROM_FMT_U08     'b'
#define INFOROM_FMT_U16     'w'
#define INFOROM_FMT_U24     't'
#define INFOROM_FMT_U32     'd'
#define INFOROM_FMT_U64     'q'
#define INFOROM_FMT_BINARY  'x'

// Helper macros for generating repeating format sequences
#define INFOROM_FMT_REP02(fmt) fmt fmt
#define INFOROM_FMT_REP04(fmt) INFOROM_FMT_REP02(fmt) INFOROM_FMT_REP02(fmt)
#define INFOROM_FMT_REP08(fmt) INFOROM_FMT_REP04(fmt) INFOROM_FMT_REP04(fmt)
#define INFOROM_FMT_REP16(fmt) INFOROM_FMT_REP08(fmt) INFOROM_FMT_REP08(fmt)
#define INFOROM_FMT_REP32(fmt) INFOROM_FMT_REP16(fmt) INFOROM_FMT_REP16(fmt)
#define INFOROM_FMT_REP64(fmt) INFOROM_FMT_REP32(fmt) INFOROM_FMT_REP32(fmt)
#define INFOROM_FMT_REP128(fmt) INFOROM_FMT_REP64(fmt) INFOROM_FMT_REP64(fmt)
#define INFOROM_FMT_REP256(fmt) INFOROM_FMT_REP128(fmt) INFOROM_FMT_REP128(fmt)
#define INFOROM_FMT_REP512(fmt) INFOROM_FMT_REP256(fmt) INFOROM_FMT_REP256(fmt)

#define INFOROM_OBJECT_SUBVERSION_SUPPORTS_NVSWITCH(sv) \
    ((((sv) & 0xF0) == 0) || (((sv) & 0xF0) == 0x20))

#define INFOROM_OBJECT_HEADER_V1_00_SIZE_OFFSET                             0x05
#define INFOROM_OBJECT_HEADER_V1_00_CHECKSUM_OFFSET                         0x07
#define INFOROM_OBJECT_HEADER_V1_00_PACKED_SIZE                                8

typedef struct INFOROM_OBJECT_HEADER_V1_00
{
    inforom_S008 type[3];
    inforom_U008 version;
    inforom_U008 subversion;
    inforom_U016 size;
    inforom_U008 checksum;
} INFOROM_OBJECT_HEADER_V1_00;

#define INFOROM_OBJECT_HEADER_V1_00_FMT "3s2bwb"

#endif // _INFOROM_TYPES_H_
