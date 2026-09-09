/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _EVENT_CTX_DEFS_H_
#define _EVENT_CTX_DEFS_H_

#include "nvtypes.h"

#if defined(EVENT_CTX_VARIABLE_SIZE)
  /*
   * Some Windows builds may include this file, and some Microsoft
   * compilers won't accept a zero size array in this context:
   * "C4200 nonstandard extension used: zero-sized array in struct/union",
   * despite this being a legitimate C99 construct.
   */
    #define EVENT_CTX_FLEXIBLE_ARRAY_MEMBER(type, name, maxSize) type name[];
#else
  #define EVENT_CTX_FLEXIBLE_ARRAY_MEMBER(type, name, maxSize) type name[maxSize];
#endif

/*
 * Common operational event context type identifiers.
 */
typedef NvU16 OPERATIONAL_EVENT_CTX_TYPE;
enum
{
    OPERATIONAL_EVENT_CTX_TYPE_OPAQUE       = 0x0000,
    OPERATIONAL_EVENT_CTX_TYPE_KEY_VALUE_64 = 0x0001,
    OPERATIONAL_EVENT_CTX_TYPE_KEY_VALUE_32 = 0x0002,
    OPERATIONAL_EVENT_CTX_TYPE_VALUES_64    = 0x0003,
    OPERATIONAL_EVENT_CTX_TYPE_VALUES_32    = 0x0004,
};

/*
 * Common operational event context payload element types.
 */
typedef struct OPERATIONAL_EVENT_CTX_KEY_VALUE_64
{
    NvU64 key;
    NvU64 value;
} OPERATIONAL_EVENT_CTX_KEY_VALUE_64;

typedef struct OPERATIONAL_EVENT_CTX_KEY_VALUE_32
{
    NvU32 key;
    NvU32 value;
} OPERATIONAL_EVENT_CTX_KEY_VALUE_32;

#endif // _EVENT_CTX_DEFS_H_
