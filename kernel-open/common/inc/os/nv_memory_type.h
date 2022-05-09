/*
 * SPDX-FileCopyrightText: Copyright (c) 2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NV_MEMORY_TYPE_H
#define NV_MEMORY_TYPE_H

#define NV_MEMORY_NONCONTIGUOUS      0
#define NV_MEMORY_CONTIGUOUS         1

#define NV_MEMORY_CACHED             0
#define NV_MEMORY_UNCACHED           1
#define NV_MEMORY_WRITECOMBINED      2
#define NV_MEMORY_WRITEBACK          5
#define NV_MEMORY_DEFAULT            6
#define NV_MEMORY_UNCACHED_WEAK      7

#define NV_PROTECT_READABLE          1
#define NV_PROTECT_WRITEABLE         2
#define NV_PROTECT_READ_WRITE       (NV_PROTECT_READABLE | NV_PROTECT_WRITEABLE)

#endif /* NV_MEMORY_TYPE_H */
