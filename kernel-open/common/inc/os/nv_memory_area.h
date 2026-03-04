/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NV_MEMORY_AREA_H
#define NV_MEMORY_AREA_H

typedef struct MemoryRange
{
    NvU64 start;
    NvU64 size;
} MemoryRange;

typedef struct MemoryArea
{
    MemoryRange *pRanges;
    NvU64 numRanges;
} MemoryArea;

static inline NvU64 memareaSize(MemoryArea memArea)
{
    NvU64 size = 0;
    NvU64 idx = 0;
    for (idx = 0; idx < memArea.numRanges; idx++)
    {
        size += memArea.pRanges[idx].size;
    }
    return size;
}

static inline MemoryRange
mrangeMake
(
    NvU64 start,
    NvU64 size
)
{
    MemoryRange range;
    range.start = start;
    range.size = size;
    return range;
}

static inline NvU64
mrangeLimit
(
    MemoryRange a
)
{
    return a.start + a.size;
}

static inline NvBool
mrangeIntersects
(
    MemoryRange a,
    MemoryRange b
)
{
    return ((a.start >= b.start) && (a.start < mrangeLimit(b))) ||
        ((b.start >= a.start) && (b.start < mrangeLimit(a)));
}

static inline NvBool
mrangeContains
(
    MemoryRange outer,
    MemoryRange inner
)
{
    return (inner.start >= outer.start) && (mrangeLimit(inner) <= mrangeLimit(outer));
}

static inline MemoryRange
mrangeOffset
(
    MemoryRange range,
    NvU64 amt
)
{
    range.start += amt;
    return range;
}

#endif /* NV_MEMORY_AREA_H */
