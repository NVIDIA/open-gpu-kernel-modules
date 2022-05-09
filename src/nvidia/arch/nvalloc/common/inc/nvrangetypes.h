/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file  nvrangetypes.h
 * @brief Range types and operator macros
 * @note  #include a header to define NvUxx and NvSxx before sourcing this file.
 */

#ifndef _NVRANGETYPES_H_
#define _NVRANGETYPES_H_


//
// Define range types by convention
//
#define __NV_DEFINE_RANGE_TYPE(T)       \
typedef struct NvRange ## T             \
{                                       \
    Nv ## T min;                        \
    Nv ## T max;                        \
} NvRange ## T;


__NV_DEFINE_RANGE_TYPE(U64)         // NvRangeU64
__NV_DEFINE_RANGE_TYPE(S64)         // NvRangeS64
__NV_DEFINE_RANGE_TYPE(U32)         // NvRangeU32
__NV_DEFINE_RANGE_TYPE(S32)         // NvRangeS32
__NV_DEFINE_RANGE_TYPE(U16)         // NvRangeU16
__NV_DEFINE_RANGE_TYPE(S16)         // NvRangeS16
__NV_DEFINE_RANGE_TYPE(U8)          // NvRangeU8
__NV_DEFINE_RANGE_TYPE(S8)          // NvRangeS8


//
// Operator macros
//
// Macros are named xxx_RANGE (rather than xxx_RANGEU32, etc.) since they work
// properly on ranges with any number of bits, signed or unsigned.
//

#define NV_EQUAL_RANGE(r1, r2)          ((r1).min == (r2).min && (r1).max == (r2).max)
#define NV_EMPTY_INCLUSIVE_RANGE(r)     ((r).min     > (r).max)
#define NV_EMPTY_EXCLUSIVE_RANGE(r)     ((r).min + 1 > (r).max - 1)
#define NV_WITHIN_INCLUSIVE_RANGE(r, x) ((r).min <= (x) && (x) <= (r).max)
#define NV_WITHIN_EXCLUSIVE_RANGE(r, x) ((r).min <  (x) && (x) <  (r).max)
#define NV_IS_SUBSET_RANGE(r1, r2)      ((r1).min >= (r2).min && (r2).max >= (r1).max)
#define NV_IS_SUPERSET_RANGE(r1, r2)    ((r1).min <= (r2).min && (r2).max <= (r1).max)
#define NV_CENTER_OF_RANGE(r)           ((r).min / 2 + ((r).max + 1) / 2)   // Avoid overflow and rounding anomalies.
#define NV_IS_OVERLAPPING_RANGE(r1, r2)             \
    (NV_WITHIN_INCLUSIVE_RANGE((r1), (r2).min) || \
     NV_WITHIN_INCLUSIVE_RANGE((r1), (r2).max))

#define NV_DISTANCE_FROM_RANGE(r, x)          ((x) <  (r).min? (r).min - (x): ((x) >  (r).max? (x) - (r).max:  0))
#define NV_VALUE_WITHIN_INCLUSIVE_RANGE(r, x) ((x) <  (r).min? (r).min      : ((x) >  (r).max? (r).max      : (x)))
#define NV_VALUE_WITHIN_EXCLUSIVE_RANGE(r, x) ((x) <= (r).min? (r).min + 1  : ((x) >= (r).max? (r).max - 1  : (x)))

#define NV_INIT_RANGE(r, x, y)                      \
do                                                  \
{                                                   \
    (r).min = (x);                                  \
    (r).max = (y);                                  \
} while(0)

#define NV_ASSIGN_DELTA_RANGE(r, x, d)              \
do                                                  \
{                                                   \
    (r).min = (x) - (d);                            \
    (r).max = (x) + (d);                            \
} while(0)

#define NV_ASSIGN_INTERSECTION_RANGE(r1, r2)        \
do                                                  \
{                                                   \
    if ((r1).min < (r2).min)                        \
        (r1).min = (r2).min;                        \
    if ((r1).max > (r2).max)                        \
        (r1).max = (r2).max;                        \
} while(0)

#define NV_ASSIGN_UNION_RANGE(r1, r2)               \
do                                                  \
{                                                   \
    if ((r1).min > (r2).min)                        \
        (r1).min = (r2).min;                        \
    if ((r1).max < (r2).max)                        \
        (r1).max = (r2).max;                        \
} while(0)

#define NV_MULTIPLY_RANGE(r, x)                     \
do                                                  \
{                                                   \
    (r).min *= (x);                                 \
    (r).max *= (x);                                 \
} while(0)

#define NV_DIVIDE_FLOOR_RANGE(r, x)                 \
do                                                  \
{                                                   \
    (r).min /= (x);                                 \
    (r).max /= (x);                                 \
} while(0)

#define NV_DIVIDE_CEILING_RANGE(r, x)               \
do                                                  \
{                                                   \
    (r).min = ((r).min + (x) - 1) / (x);            \
    (r).max = ((r).max + (x) - 1) / (x);            \
} while(0)

#define NV_DIVIDE_ROUND_RANGE(r, x)                 \
do                                                  \
{                                                   \
    (r).min = ((r).min + (x) / 2) / (x);            \
    (r).max = ((r).max + (x) / 2) / (x);            \
} while(0)

#define NV_DIVIDE_WIDE_RANGE(r, x)                  \
do                                                  \
{                                                   \
    (r).min /= (x);                                 \
    (r).max = ((r).max + (x) - 1) / (x);            \
} while(0)

#define NV_DIVIDE_NARROW_RANGE(r, x)                \
do                                                  \
{                                                   \
    (r).min = ((r).min + (x) - 1) / (x);            \
    (r).max /= (x);                                 \
} while(0)

#define NV_VALUE_WITHIN_INCLUSIVE_RANGE(r, x)       \
    ((x) <  (r).min? (r).min      : ((x) >  (r).max? (r).max      : (x)))

#define NV_WITHIN_INCLUSIVE_RANGE(r, x)             \
    ((r).min <= (x) && (x) <= (r).max)

#define NV_DISTANCE_FROM_RANGE(r, x)                \
    ((x) <  (r).min? (r).min - (x): ((x) >  (r).max? (x) - (r).max:  0))

#endif // _NVRANGETYPES_H_

