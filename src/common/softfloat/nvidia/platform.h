/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef nvidia_softfloat_platform_h
#define nvidia_softfloat_platform_h 1

#include "nvtypes.h"

/*
 * Build softfloat for little endian CPUs: all NVIDIA target platforms are
 * little endian.
 */
#define LITTLEENDIAN 1

/*
 * "INLINE" is used by softfloat like this:
 *
 *  INLINE uint32_t softfloat_foo(...)
 *  {
 *      ...
 *  }
 */
#define INLINE static NV_INLINE

#if !defined(nvAssert)
#define nvAssert(x)
#endif

/*
 * softfloat will use THREAD_LOCAL to tag variables that should be per-thread;
 * it could be set to, e.g., gcc's "__thread" keyword.  If THREAD_LOCAL is left
 * undefined, these variables will default to being ordinary global variables.
 */
#undef THREAD_LOCAL

#endif /* nvidia_softfloat_platform_h */
