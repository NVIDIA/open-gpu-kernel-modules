/*
 * SPDX-FileCopyrightText: Copyright (c) 2002-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef BIOSTYPES_H_FILE
#define BIOSTYPES_H_FILE


/**************************************************************************************************************
*
*   Description:
*       Definitions of BIOS types defines used in parsing bios structures.
*
**************************************************************************************************************/

// We convert the bios data structures before they are used.  They are read from bios,
//   then each field is expanded into a 32 bit field into host byte order.
//   Each data structure has a format string below that describes its packed
//   layout in the rom.
//
// The format string looks like:
//
//    count defaults to 1
//    item is one of
//          b  (byte)
//          w  2 byte word
//          d  4 byte word
//
//  Eg: "1b3d"
//
//   means 1 byte field followed by 3 dword (32 bit) fields
//

#define bios_U008  NvU32
#define bios_U016  NvU32
#define bios_U032  NvU32
#define bios_S008  NvS32
#define bios_S016  NvS32
#define bios_S032  NvS32

#endif // BIOSTYPES_H_FILE
