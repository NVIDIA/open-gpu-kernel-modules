/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _NV_UTILS_NV_RANGE_H_
#define _NV_UTILS_NV_RANGE_H_

#include "nvtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \anchor NV_RANGE_1
 * @defgroup NV_RANGE NV_RANGE
 *
 * @brief Range is a sequence of unsigned 64 bit integers, represented by its
 *        lower and upper bounds, inclusive.
 *
 * @details Range is a general purpose data structure utility.
 *          It consist of two fields, lower and upper bound.
 *          It is assumed that both lower and upper bounds are \b inclusive.
 *          Range with lower bound greater than the upper bound is considered to
 *          be an empty range.
 *
 * @note If a range is declared like <I> Range r = {0x0, 0x2} </I> it consist
 *       of elements <I> 0x0, 0x1 and 0x2 </I>, i.e. <B> Range = [lo, hi] ! </B>
 *
 *    > There are 4 possible options
 *    > -# (lo, hi)  lo+1 .. hi-1
 *    > -# [lo, hi)  lo   .. hi-1
 *    > -# (lo, hi]  lo+1 .. hi
 *    > -# [lo, hi]  lo   .. hi
 *
 * Notice that only option 4 is capable of describing a full range.
 * Full range would be 0x0..NvU64_MAX, where
 * NvU64_MAX = 0xFFFFFFFFFFFFFFFF.
 *
 * @{
 */
typedef struct NV_RANGE NV_RANGE;
struct NV_RANGE
{
    /** Lower bound of the range, where range includes the lower bound.*/
    NvU64 lo;
    /** Upper bound of the range, where range includes the upper bound.*/
    NvU64 hi;
};

static const NV_RANGE NV_RANGE_EMPTY = {1, 0};

/**
 * @brief Checks if range is empty, i.e. range.lo > range.hi
 *
 * @returns NV_TRUE if range is empty, NV_FALSE otherwise.
 */
static NV_INLINE NvBool rangeIsEmpty(NV_RANGE range)
{
    return range.lo > range.hi;
}

/**
 * @brief Calculate range length in bytes.
 *
 * @warning If the range is max, i.e. from 0 to NvU64_MAX, calling this
 *          function would result in overflow since range length is calculated
 *          as hi-lo+1.
 *
 * @par Example:
 * @snippet nv_range-test.cpp rangeLengthExample
 */
static NV_INLINE NvU64 rangeLength(NV_RANGE range)
{
    return rangeIsEmpty(range) ? 0 : range.hi - range.lo + 1;
}

/**
 * @brief Creates a range
 *
 * @details This is useful since on some compilers the following code won't
 *          work: `rangeLength({0, 100})`.
 *          However, `rangeLength(rangeMake(0, 100))` will always work.
 *
 * @Returns Range of elements from and including \a lo to and
 *          including \a hi, i.e. <B> [lo, hi] </B>
 */
static NV_INLINE NV_RANGE rangeMake(NvU64 lo, NvU64 hi)
{
    NV_RANGE rng = {lo, hi};
    return rng;
}

/**
 * @brief Check if the two given ranges are equal.
 */
static NV_INLINE NvBool rangeEquals(NV_RANGE range1, NV_RANGE range2)
{
    if (rangeIsEmpty(range1) && rangeIsEmpty(range2))
    {
        return NV_TRUE;
    }

    return (range1.lo == range2.lo) && (range1.hi == range2.hi);
}

/**
 * @brief Check if \a range1 contains \a range2.
 *
 * @param[in] range1 Container.
 * @param[in] range2 Containee.
 *
 * @par Example:
 * @snippet nv_range-test.cpp rangeContainsExample
 */
static NV_INLINE NvBool rangeContains(NV_RANGE range1, NV_RANGE range2)
{
    return (range1.lo <= range2.lo) &&(range1.hi >= range2.hi);
}

/**
 * @brief Checks if intersection of two ranges is not an empty range.
 *
 * @par Example:
 * @snippet nv_range-test.cpp rangeOverlapExample
 */
static NV_INLINE NvBool rangeOverlaps(NV_RANGE range1, NV_RANGE range2)
{
    return (range1.lo <= range2.lo && range2.lo <= range1.hi)
           ||
           (range1.lo <= range2.hi && range2.hi <= range1.hi)
           ||
           (range2.lo <= range1.lo && range1.lo <= range2.hi)
           ||
           (range2.lo <= range1.hi && range1.hi <= range2.hi);
}

/**
 * @brief Returns a range representing an intersection between two given ranges.
 *
 * @par Example:
 * @snippet nv_range-test.cpp rangeOverlapExample
 */
static NV_INLINE NV_RANGE rangeIntersection(NV_RANGE range1, NV_RANGE range2)
{
    NV_RANGE intersect;

    if (rangeIsEmpty(range1) || rangeIsEmpty(range2))
    {
        return NV_RANGE_EMPTY;
    }

    intersect.lo = range1.lo < range2.lo ? range2.lo : range1.lo;
    intersect.hi = range1.hi > range2.hi ? range2.hi : range1.hi;

    return intersect;
}

/**
 * @brief Compares two ranges.
 * @returns 0  - \a range1's lower bound is equal to \a range2's lower bound,
 *         <0 - \a range1's lower bound is less than \a range2's lower bound,
 *         >0 - \a range2's lower bound is greater than \a range2's lower bound.
 *
 * @warning If function returns 0 that does not mean that ranges are equal,
 *          just that their lower bounds are equal!
 *
 * @par Example:
 * @snippet nv_range-test.cpp rangeCompareExample
 */
static NV_INLINE NvS32 rangeCompare(NV_RANGE range1, NV_RANGE range2)
{
    if (rangeIsEmpty(range1) && rangeIsEmpty(range2))
    {
        return 0;
    }

    return range1.lo >= range2.lo ? (range1.lo == range2.lo ? 0 : 1) : -1;
}

/**
 * @brief Merge two ranges into one.
 *
 * @returns Merged range. If two ranges have no intersection
 *          the returned range will be empty.
 *
 * @note Empty range is range with lo > hi.
 *
 * @par Example:
 * @snippet nv_range-test.cpp rangeMergeExample
 */
static NV_INLINE NV_RANGE rangeMerge(NV_RANGE range1, NV_RANGE range2)
{
    NV_RANGE merged = NV_RANGE_EMPTY;

    if (rangeIsEmpty(range1) || rangeIsEmpty(range2) || !rangeOverlaps(range1, range2))
    {
        return merged;
    }

    merged.lo = range1.lo;
    merged.hi = range1.hi;

    if (range2.lo < merged.lo)
    {
        merged.lo = range2.lo;
    }
    if (range2.hi > merged.hi)
    {
        merged.hi = range2.hi;
    }

    return merged;
}

/**
 * @brief Checks if \a range1 borders with \a range2, i.e. \a range1.lo ==
 *         \a range2.hi+1 or \a range2.lo == \a range1.hi+1
 *
 * @note [a,b] borders with [b+1,c] where a < b < c
 *
 */
static NV_INLINE NvBool rangeBorders(NV_RANGE range1, NV_RANGE range2)
{
    if (rangeIsEmpty(range1) || rangeIsEmpty(range2))
    {
        return NV_FALSE;
    }

    return (range1.hi + 1 == range2.lo) || (range2.hi + 1 == range1.lo);
}

/**
 * @brief Splits \a pBigRange
 *
 * @param[in] pBigRange             Pointer to starting range.
 * @param[in] rangeToSplit          Range to split the first range over.
 * @param[in] pSecondPartAfterSplit Second range after split.
 *
 * @par Example:
 * @snippet nv_range-test.cpp rangeSplitExample
 */
static NV_INLINE NV_STATUS rangeSplit(NV_RANGE *pBigRange,
                         NV_RANGE rangeToSplit, NV_RANGE *pSecondPartAfterSplit)
{
    if (rangeIsEmpty(*pBigRange) || rangeIsEmpty(rangeToSplit) ||
        !rangeContains(*pBigRange, rangeToSplit))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pSecondPartAfterSplit->hi = pBigRange->hi;
    pBigRange->hi = rangeToSplit.lo;
    pSecondPartAfterSplit->lo = rangeToSplit.hi + 1;

    return NV_OK;
}

#ifdef __cplusplus
}
#endif
///@}
///  NV_UTILS_RANGE
#endif
