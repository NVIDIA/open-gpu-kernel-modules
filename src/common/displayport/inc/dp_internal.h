/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_internal.h                                                     *
*    RM stubs to allow unit testing.                                        *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_INTERNAL_H
#define INCLUDED_DP_INTERNAL_H

//
//  Clients should not include this file
//  This file provides the private malloc implementation.
//

#include <nvtypes.h>
#include <stddef.h>      // size_t

#include "dp_object.h"
#include "dp_ringbuffer.h"

static inline void dpByteToHexChar(char *output, NvU8 c)
{
    char dig = (c>>4) & 0xF;
    output[0] = dig < 10 ? dig + '0' : dig + 'A' - 10;
    dig = c & 0xF;
    output[1] = dig < 10 ? dig + '0' : dig + 'A' - 10;
}

static inline void dpHexDump(char * output, unsigned outSize, NvU8 * buffer, unsigned size)
{
    char * tail = output;       
    if (outSize < size * 3 + 1)
        return;

    for (unsigned i = 0; i < size; i++)
    {
        dpByteToHexChar(tail, buffer[i]);
        tail += 2;
        *tail++ = ' ';
    }
    *tail = 0;
}

namespace DisplayPort
{
    template <class T>
    inline void swap_args(T & left, T & right)
    {
        T temp = left;
        left = right;
        right = temp;
    }

    inline NvU64 divide_ceil(NvU64 a, NvU64 b)
    {
        return (a + b - 1) / b;
    }

    inline NvU64 divide_floor(NvU64 a, NvU64 b)
    {
        return a / b;
    }

    inline NvU64 axb_div_c_64(NvU64 a, NvU64 b, NvU64 c)
    {
        // NvU64 arithmetic to keep precision and avoid floats
        // a*b/c = (a/c)*b + ((a%c)*b + c/2)/c
        return ((a/c)*b + ((a%c)*b + c/2)/c);
    }
}

#define DP_MIN(x,y) ((x)<(y)?(x):(y))
#define DP_MAX(x,y) ((x)<(y)?(y):(x))

//
//  Macro to suppress unused local variable
//
template <class T> void dp_used(const T & /*x*/) {}
#define DP_USED(x) dp_used(x)


//
//  Basic debug logging facility
//

#if NV_DP_ASSERT_ENABLED
#define DP_LOG(x)                                                           \
    do                                                                      \
    {                                                                       \
        dpPrint x;                                                          \
        addDpLogRecord x;                                                   \
    }while (false)

#define DP_ASSERT(x)                                                        \
    if (!(x))                                                               \
    {                                                                       \
        addDpAssertRecord();                                                \
        dpAssert(#x, __FILE__, __FUNCTION__, __LINE__);                     \
        dpDebugBreakpoint();                                                \
    }
#else

#define DP_LOG(x)

#define DP_ASSERT(x)                                                        \
    {                                                                       \
        DP_USED(x);                                                         \
        if (!(x))                                                           \
        {                                                                   \
            addDpAssertRecord();                                            \
        }                                                                   \
    }
#endif

#endif //INCLUDED_DP_INTERNAL_H
