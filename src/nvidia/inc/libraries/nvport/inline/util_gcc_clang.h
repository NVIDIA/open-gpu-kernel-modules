/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**
 * @file
 * @brief Util functions implementations using gcc and clang compiler intrinsics
 */

#ifndef _NVPORT_UTIL_GCC_CLANG_H_
#define _NVPORT_UTIL_GCC_CLANG_H_

//
// Disabling portUtilExGetStackTrace_SUPPORTED on all clients because the
//  implementation is unsafe and generates warnings on new build compilers.
//
// From https://gcc.gnu.org/onlinedocs/gcc/Return-Address.html :
//   Calling this function with a nonzero argument can have unpredictable effects,
//   including crashing the calling program. As a result, calls that are considered
//   unsafe are diagnosed when the -Wframe-address option is in effect. Such calls
//   should only be made in debugging situations.
//
// If this feature is desirable, please replace the body of portUtilExGetStackTrace()
//   with implementations that tie into native stacktrace reporting infrastructure
//   of the platforms nvport runs on.
//
#define portUtilExGetStackTrace_SUPPORTED 0
#define portUtilExGetStackTrace(_level) ((NvUPtr)0)

#define portUtilGetReturnAddress() (NvUPtr)__builtin_return_address(0)

#if NVCPU_IS_X86 || NVCPU_IS_X86_64
#define NVPORT_DUMMY_LOOP() \
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");\
                                   \
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");\
                                   \
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");\
                                   \
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");\
    __asm__ __volatile__ ("pause");
#else
#define NVPORT_DUMMY_LOOP() \
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");\
                                   \
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");\
                                   \
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");\
                                   \
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");\
    __asm__ __volatile__ ("nop");
#endif

#if (__GNUC__ < 4) || (NVCPU_IS_ARM) || (NVCPU_IS_X86 && PORT_IS_KERNEL_BUILD) || (NVCPU_IS_RISCV64)
#define PORT_UTIL_CLZ_CTX_NOT_DEFINED 1
#else
PORT_UTIL_INLINE NvU32 portUtilCountLeadingZeros64(NvU64 n)
{
    if (n == 0)
        return 64;

    return __builtin_clzll(n);
}
PORT_UTIL_INLINE NvU32 portUtilCountLeadingZeros32(NvU32 n)
{
    if (n == 0)
        return 32;

    return __builtin_clz(n);
}


PORT_UTIL_INLINE NvU32 portUtilCountTrailingZeros64(NvU64 n)
{
    if (n == 0)
        return 64;

    return __builtin_ctzll(n);
}
PORT_UTIL_INLINE NvU32 portUtilCountTrailingZeros32(NvU32 n)
{
    if (n == 0)
        return 32;

    return __builtin_ctz(n);
}

#endif


#if NVCPU_IS_FAMILY_X86 && !defined(NV_MODS)
PORT_UTIL_INLINE NvU64 portUtilExReadTimestampCounter()
{
    NvU32 lo;
    NvU32 hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (lo | ((NvU64)hi << 32));
}
#define portUtilExReadTimestampCounter_SUPPORTED 1

#elif NVCPU_IS_AARCH64 && !defined(NV_MODS)
PORT_UTIL_INLINE NvU64 portUtilExReadTimestampCounter()
{
    NvU64 ts = 0;
    __asm__ __volatile__ ("mrs %0, cntvct_el0" : "=r" (ts));
    return ts;
}
#define portUtilExReadTimestampCounter_SUPPORTED 1

#elif NVCPU_IS_PPC64LE && !defined(NV_MODS)
PORT_UTIL_INLINE NvU64 portUtilExReadTimestampCounter()
{
    NvU64 ts;
    __asm__ __volatile__ ("mfspr %0,268" : "=r"(ts));
    return ts;
}
#define portUtilExReadTimestampCounter_SUPPORTED 1

#elif NVCPU_IS_PPC && !defined(NV_MODS)
PORT_UTIL_INLINE NvU64 portUtilExReadTimestampCounter()
{
    NvU32 lo, hi, tmp;
    __asm__ __volatile__ (
      "0:\n"
      "mftbu %0\n"
      "mftbl %1\n"
      "mftbu %2\n"
      "cmpw %0, %2\n"
      "bne- 0b"
      : "=r" (hi), "=r" (lo), "=r" (tmp) );
    return  ((hi << 32) | lo);
}
#define portUtilExReadTimestampCounter_SUPPORTED 1

#else
#define portUtilExReadTimestampCounter_SUPPORTED 0
#endif

#endif // _NVPORT_UTIL_GCC_CLANG_H_
