/*
 * SPDX-FileCopyrightText: Copyright (c) 2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NV_COMMON_UTILS_H__
#define __NV_COMMON_UTILS_H__

#include "nvtypes.h"
#include "nvmisc.h"

#if !defined(TRUE)
#define TRUE NV_TRUE
#endif

#if !defined(FALSE)
#define FALSE NV_FALSE
#endif

#define NV_IS_UNSIGNED(x) ((__typeof__(x))-1 > 0)

/* Get the length of a statically-sized array. */
#define ARRAY_LEN(_arr) (sizeof(_arr) / sizeof(_arr[0]))

#define NV_INVALID_HEAD         0xFFFFFFFF

#define NV_INVALID_CONNECTOR_PHYSICAL_INFORMATION (~0)

#if !defined(NV_MIN)
# define NV_MIN(a,b) (((a)<(b))?(a):(b))
#endif

#define NV_MIN3(a,b,c) NV_MIN(NV_MIN(a, b), c)
#define NV_MIN4(a,b,c,d) NV_MIN3(NV_MIN(a,b),c,d)

#if !defined(NV_MAX)
# define NV_MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define NV_MAX3(a,b,c) NV_MAX(NV_MAX(a, b), c)
#define NV_MAX4(a,b,c,d) NV_MAX3(NV_MAX(a,b),c,d)

static inline int NV_LIMIT_VAL_TO_MIN_MAX(int val, int min, int max)
{
    if (val < min) {
        return min;
    }
    if (val > max) {
        return max;
    }
    return val;
}

#define NV_ROUNDUP_DIV(x,y) ((x) / (y) + (((x) % (y)) ? 1 : 0))

/*
 * Macros used for computing palette entries:
 *
 * NV_UNDER_REPLICATE(val, source_size, result_size) expands a value
 * of source_size bits into a value of target_size bits by shifting
 * the source value into the high bits and replicating the high bits
 * of the value into the low bits of the result.
 *
 * PALETTE_DEPTH_SHIFT(val, w) maps a colormap entry for a component
 * that has w bits to an appropriate entry in a LUT of 256 entries.
 */
static inline unsigned int NV_UNDER_REPLICATE(unsigned short val,
                                              int source_size,
                                              int result_size)
{
    return (val << (result_size - source_size)) |
        (val >> ((source_size << 1) - result_size));
}


static inline unsigned short PALETTE_DEPTH_SHIFT(unsigned short val, int depth)
{
    return NV_UNDER_REPLICATE(val, depth, 8);
}

/*
 *  Use __builtin_ffs where it is supported, or provide an equivalent
 *  implementation for platforms like riscv where it is not.
 */
#if defined(__GNUC__) && !NVCPU_IS_RISCV64
static inline int nv_ffs(int x)
{
    return __builtin_ffs(x);
}
#else
static inline int nv_ffs(int x)
{
    if (x == 0)
        return 0;

    LOWESTBITIDX_32(x);

    return 1 + x;
}
#endif

#endif /* __NV_COMMON_UTILS_H__ */
