/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief UTIL module generic crossplatform implementation
 */

#ifndef _NVPORT_UTIL_GENERIC_H_
#define _NVPORT_UTIL_GENERIC_H_

PORT_UTIL_INLINE NvBool
portUtilCheckOverlap
(
    const NvU8 *pData0,
    NvLength len0,
    const NvU8 *pData1,
    NvLength len1
)
{
    return (pData0 >= pData1 && pData0 < (pData1 + len1)) ||
           (pData1 >= pData0 && pData1 < (pData0 + len0));
}

PORT_UTIL_INLINE NvBool
portUtilCheckAlignment
(
    const void *address,
    NvU32 align
)
{
    if (!portUtilIsPowerOfTwo(align))
        return NV_FALSE;

    return ((NvUPtr)address & (align-1)) == 0;
}

PORT_UTIL_INLINE NvBool
portUtilIsPowerOfTwo
(
    NvU64 num
)
{
    return (num & (num-1)) == 0;
}

/*
 * This function is designed to be able to make unaligned access
 * (but might be slower because of the byte by byte access)
 */
PORT_UTIL_INLINE void
portUtilWriteLittleEndian16
(
    void *pBuf,
    NvU16 value
)
{
    *((NvU8*)pBuf + 1) = (NvU8)(value >> 8);
    *((NvU8*)pBuf + 0) = (NvU8)(value);
}

/*
 * This function is designed to be able to make unaligned access
 * (but might be slower because of the byte by byte access)
 */
PORT_UTIL_INLINE void
portUtilWriteLittleEndian32
(
    void *pBuf,
    NvU32 value
)
{
    *((NvU8*)pBuf + 3) = (NvU8)(value >> 24);
    *((NvU8*)pBuf + 2) = (NvU8)(value >> 16);
    *((NvU8*)pBuf + 1) = (NvU8)(value >> 8);
    *((NvU8*)pBuf + 0) = (NvU8)(value);
}

/*
 * This function is designed to be able to make unaligned access
 * (but might be slower because of the byte by byte access)
 */
PORT_UTIL_INLINE void
portUtilWriteLittleEndian64
(
    void *pBuf,
    NvU64 value
)
{
    *((NvU8*)pBuf + 7) = (NvU8)(value >> 56);
    *((NvU8*)pBuf + 6) = (NvU8)(value >> 48);
    *((NvU8*)pBuf + 5) = (NvU8)(value >> 40);
    *((NvU8*)pBuf + 4) = (NvU8)(value >> 32);
    *((NvU8*)pBuf + 3) = (NvU8)(value >> 24);
    *((NvU8*)pBuf + 2) = (NvU8)(value >> 16);
    *((NvU8*)pBuf + 1) = (NvU8)(value >> 8);
    *((NvU8*)pBuf + 0) = (NvU8)(value);
}

/*
 * This function is designed to be able to make unaligned access
 * (but might be slower because of the byte by byte access)
 */
PORT_UTIL_INLINE void
portUtilWriteBigEndian16
(
    void *pBuf,
    NvU16 value
)
{
    *((NvU8*)pBuf + 0) = (NvU8)(value >> 8);
    *((NvU8*)pBuf + 1) = (NvU8)(value);
}

/*
 * This function is designed to be able to make unaligned access
 * (but might be slower because of the byte by byte access)
 */
PORT_UTIL_INLINE void
portUtilWriteBigEndian32
(
    void *pBuf,
    NvU32 value
)
{
    *((NvU8*)pBuf + 0) = (NvU8)(value >> 24);
    *((NvU8*)pBuf + 1) = (NvU8)(value >> 16);
    *((NvU8*)pBuf + 2) = (NvU8)(value >> 8);
    *((NvU8*)pBuf + 3) = (NvU8)(value);
}

/*
 * This function is designed to be able to make unaligned access
 * (but might be slower because of the byte by byte access)
 */
PORT_UTIL_INLINE void
portUtilWriteBigEndian64
(
    void *pBuf,
    NvU64 value
)
{
    *((NvU8*)pBuf + 0) = (NvU8)(value >> 56);
    *((NvU8*)pBuf + 1) = (NvU8)(value >> 48);
    *((NvU8*)pBuf + 2) = (NvU8)(value >> 40);
    *((NvU8*)pBuf + 3) = (NvU8)(value >> 32);
    *((NvU8*)pBuf + 4) = (NvU8)(value >> 24);
    *((NvU8*)pBuf + 5) = (NvU8)(value >> 16);
    *((NvU8*)pBuf + 6) = (NvU8)(value >> 8);
    *((NvU8*)pBuf + 7) = (NvU8)(value);
}

#if PORT_COMPILER_IS_GCC || PORT_COMPILER_IS_CLANG
#include "nvport/inline/util_gcc_clang.h"
#elif PORT_COMPILER_IS_MSVC
#include "nvport/inline/util_msvc.h"
#else
#error "Unsupported compiler"
#endif // switch

#ifdef PORT_UTIL_CLZ_CTX_NOT_DEFINED
PORT_UTIL_INLINE NvU32 portUtilCountLeadingZeros64(NvU64 n)
{
    NvU32 y;

    if (n == 0)
        return 64;

    for (y = 0; !(n & 0x8000000000000000LL); y++)
        n <<= 1;

    return y;
}
PORT_UTIL_INLINE NvU32 portUtilCountLeadingZeros32(NvU32 n)
{
    NvU32 y;

    if (n == 0)
        return 32;

    for (y = 0; !(n & 0x80000000); y++)
        n <<= 1;

    return y;
}

PORT_UTIL_INLINE NvU32 portUtilCountTrailingZeros64(NvU64 n)
{
    NvU32 bz, b5, b4, b3, b2, b1, b0;
    NvU64 y;

    y = n & (~n + 1);
    bz = y ? 0 : 1;
    b5 = (y & 0x00000000FFFFFFFFLL) ? 0 : 32;
    b4 = (y & 0x0000FFFF0000FFFFLL) ? 0 : 16;
    b3 = (y & 0x00FF00FF00FF00FFLL) ? 0 : 8;
    b2 = (y & 0x0F0F0F0F0F0F0F0FLL) ? 0 : 4;
    b1 = (y & 0x3333333333333333LL) ? 0 : 2;
    b0 = (y & 0x5555555555555555LL) ? 0 : 1;

    return (bz + b5 + b4 + b3 + b2 + b1 + b0);
}
PORT_UTIL_INLINE NvU32 portUtilCountTrailingZeros32(NvU32 n)
{
    NvU32 bz, b4, b3, b2, b1, b0;
    NvU32 y;

    y = n & (~n + 1);
    bz = y ? 0 : 1;
    b4 = (y & 0x0000FFFF) ? 0 : 16;
    b3 = (y & 0x00FF00FF) ? 0 : 8;
    b2 = (y & 0x0F0F0F0F) ? 0 : 4;
    b1 = (y & 0x33333333) ? 0 : 2;
    b0 = (y & 0x55555555) ? 0 : 1;

    return (bz + b4 + b3 + b2 + b1 + b0);
}
#endif

static NV_FORCEINLINE void
portUtilSpin(void)
{
    NvU32 idx;
    for (idx = 0; idx < 100; idx++)
    {
        NVPORT_DUMMY_LOOP();
    }
}

#if NVCPU_IS_FAMILY_X86 && !defined(NV_MODS) && PORT_IS_MODULE_SUPPORTED(atomic)
static NV_FORCEINLINE NvU64
portUtilExReadTimestampCounterSerialized(void)
{
    NvU64 val;

    portAtomicMemoryFenceLoad();
    val = portUtilExReadTimestampCounter();
    portAtomicMemoryFenceLoad();

    return val;
}
#define portUtilExReadTimestampCounterSerialized_SUPPORTED 1
#else
#define portUtilExReadTimestampCounterSerialized_SUPPORTED 0
#endif

#endif // _NVPORT_UTIL_GENERIC_H_
