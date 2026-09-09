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

/*
 * nvintrin.h
 */
#ifndef __NV_INTRIN_H
#define __NV_INTRIN_H

#include "nvtypes.h"

#if defined(nvMemoryLoadFence)
// do nothing
#else

/*!
 * This function is a load fence that prevents architectural and compiler reordering of loads.
 *
 * @note This function is a no-op on platforms that do not support memory fences.
 */
static NV_FORCEINLINE void
nvMemoryLoadFence(void)
{
#if   defined(__GNUC__) || defined(__clang__)

#if NVCPU_IS_FAMILY_X86
    __asm__ __volatile__ ("lfence" : : : "memory");
#elif defined(NVCPU_AARCH64)
    __asm__ __volatile__ ("dmb ld" : : : "memory");
#elif NVCPU_IS_FAMILY_ARM
    // We still use 32-bit arm on some processors we still support.
    __asm__ __volatile__ ("dmb sy" : : : "memory");
#elif NVCPU_IS_FAMILY_RISCV
    __asm__ __volatile__ ("fence r, r" : : : "memory");
#else
#error "unsupported compiler"
#endif

#else
#error "unsupported compiler"
#endif
}
#endif // defined(NVOC)

#endif // __NV_INTRIN_H
