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
 * @brief Util module public interface
 */

#ifndef _NVPORT_H_
#error "This file cannot be included directly. Include nvport.h instead."
#endif

#ifndef _NVPORT_UTIL_H_
#define _NVPORT_UTIL_H_

/**
 * @defgroup NVPORT_UTIL Utilities module
 *
 * @brief This module contains utility functions used by other modules.
 *
 * Generic implementation for all functions is in util-generic.h
 *
 * @{
 */

/**
 * @def PORT_UTIL_INLINE
 *
 * @note There are ways to declare a function without qualifiers, and then
 * redefine it as static/extern inline, but none work across all compilers that
 * we use. The easiest solution is to just specify the qualifiers upon function
 * declaration. We assume all these will be inline, but that can be changed
 * through the makefile when adding non-inline implementations:
 * MODULE_DEFINES += PORT_UTIL_INLINE
 * MODULE_SOURCES += util-impl.c
 */
#ifndef PORT_UTIL_INLINE
#define PORT_UTIL_INLINE PORT_INLINE
#endif

#if NVOS_IS_LIBOS
#include "nvport/inline/util_libos.h"
#endif

/**
 * @name Core Functions
 * @{
 */

/**
 * @brief Returns true if the two buffers overlap.
 *
 * Buffer length is specified in len0 and len1 params.
 */
PORT_UTIL_INLINE NvBool portUtilCheckOverlap(const NvU8 *pData0, NvLength len0, const NvU8 *pData1, NvLength len1);

/**
 * @brief Returns true if address is aligned to align bytes
 *
 * If align is not a power of two, it will return false.
 */
PORT_UTIL_INLINE NvBool portUtilCheckAlignment(const void *address, NvU32 align);

/**
 * @brief Returns true if num is a power of two.
 */
PORT_UTIL_INLINE NvBool portUtilIsPowerOfTwo(NvU64 num);

/**
 * @brief Write the 16bit number to pBuf in Little Endian
 */
PORT_UTIL_INLINE void portUtilWriteLittleEndian16(void *pBuf, NvU16 value);

/**
 * @brief Write the 32bit number to pBuf in Little Endian
 */
PORT_UTIL_INLINE void portUtilWriteLittleEndian32(void *pBuf, NvU32 value);

/**
 * @brief Write the 64bit number to pBuf in Little Endian
 */
PORT_UTIL_INLINE void portUtilWriteLittleEndian64(void *pBuf, NvU64 value);

/**
 * @brief Write the 16bit number to pBuf in Big Endian
 */
PORT_UTIL_INLINE void portUtilWriteBigEndian16(void *pBuf, NvU16 value);

/**
 * @brief Write the 32bit number to pBuf in Big Endian
 */
PORT_UTIL_INLINE void portUtilWriteBigEndian32(void *pBuf, NvU32 value);

/**
 * @brief Write the 64bit number to pBuf in Big Endian
 */
PORT_UTIL_INLINE void portUtilWriteBigEndian64(void *pBuf, NvU64 value);

/**
 * @brief Efficient spinloop body that doesn't waste power.
 *
 * This function will spin for a very short time, then return, so it should be
 * called as:
 *
 * ~~~{.c}
 * while (bShouldSpin)
 *     portUtilSpin();
 * ~~~
 */
static NV_FORCEINLINE void portUtilSpin(void);

/**
 * @brief Returns true if the caller is currently in interrupt context.
 *
 * Interrupt context here means:
 * - Unix: Interrupts are masked
 * - Windows: IRQL > DISPATCH_LEVEL
 */
NvBool portUtilIsInterruptContext(void);

/**
 * @def  portUtilGetReturnAddress()
 * Returns the current function's return address.
 */

/**
 * @def  portUtilGetIPAddress()
 * Returns the current IP address.
 */
NV_NOINLINE NvUPtr portUtilGetIPAddress(void);

/**
 * @brief Returns number of leading zeros - starting from MSB;
 *
 * Examples:
 * portUtilCountLeadingZeros64(0) == 64
 * portUtilCountLeadingZeros64(1) == 63
 * portUtilCountLeadingZeros64(2) == 62
 * portUtilCountLeadingZeros64(0xFFFFFFFFFFFFFF00) == 0
 */
PORT_UTIL_INLINE NvU32 portUtilCountLeadingZeros64(NvU64 n);

/**
 * @brief Like @ref portUtilCountLeadingZeros64 but operating on 32bit ints
 */
PORT_UTIL_INLINE NvU32 portUtilCountLeadingZeros32(NvU32 n);

/**
 * @brief Returns number of trailing zeros - starting from LSB;
 *
 * Examples:
 * portUtilCountTrailingZeros64(0) == 64
 * portUtilCountTrailingZeros64(1) == 0
 * portUtilCountTrailingZeros64(2) == 1
 * portUtilCountTrailingZeros64(0xFFFFFFFFFFFFFF00) == 8
 */
PORT_UTIL_INLINE NvU32 portUtilCountTrailingZeros64(NvU64 n);

/**
 * @brief Like @ref portUtilCountTrailingZeros64 but operating on 32bit ints
 */
PORT_UTIL_INLINE NvU32 portUtilCountTrailingZeros32(NvU32 n);

/// @} End core functions

#include <stddef.h> /* NULL */

/**
 * @name Extended Functions
 * @{
 */

/**
 * @brief Returns a return address up the stack of the current function.
 *
 * @param level The number of levels up the stack to go.
 *               level == 0 - Gives the current IP.
 *               level == 1 - The current function's return address, same as
 *                            @ref portUtilGetReturnAddress
 */
NV_NOINLINE NvUPtr portUtilExGetStackTrace(NvU32 level);

#define portUtilExSpinNs_SUPPORTED PORT_IS_MODULE_SUPPORTED(time)
#define portUtilExDelayMs_SUPPORTED PORT_IS_MODULE_SUPPORTED(time)

/**
 * @brief Spin for the given amount of nanoseconds.
 *
 * Utilizes @ref portUtilSpin to spin for the given duration, without putting
 * the thread to sleep.
 */
void portUtilExSpinNs(NvU32 nanoseconds);

/**
 * @brief Delay the thread execution for the given duration in milliseconds.
 *
 * Unlike @ref portUtilSpinNs, this function may put the thread to sleep.
 */
void portUtilExDelayMs(NvU32 milliseconds);

#if (NVCPU_IS_FAMILY_X86 || NVCPU_IS_PPC64LE || NVCPU_IS_PPC || NVCPU_IS_AARCH64) && !defined(NV_MODS)
/**
 * @brief Gets the Time stamp counter.
 *
 * @note This function is not serialized, and can be reorder by cpu or compiler.
 * @note On Intel "pre-Nehalem multi-core" cpu and all multi-socket cpu, time
 * may not be synced on all the cores and this function may return timestamps
 * that are not monotonically increasing.
 * @note On some old Intel cpus (P3/P4), timestamp counter was not incremented
 * at a fixed clock rate, but Intel fixed this with "invariant TSC" in late P4+
 * chips.
 */
PORT_UTIL_INLINE NvU64 portUtilExReadTimestampCounter(void);
#endif

#if NVCPU_IS_FAMILY_X86 && !defined(NV_MODS) && PORT_IS_MODULE_SUPPORTED(atomic)
/**
 * @brief Gets the Time stamp counter.
 *
 * Unlike @ref portUtilExReadTimestampCounter, this function serializes
 * the reading of time stamp counter to prevent both compiler and cpu
 * reordering.
 * @note Other than serialization issue, this function has same issues as
 * @ref portUtilExReadTimestampCounter.
 */
static NV_FORCEINLINE NvU64 portUtilExReadTimestampCounterSerialized(void);
#endif
/// @} End extended functions

#include "nvport/inline/util_generic.h"
#include "nvport/inline/util_valist.h"

#endif // _NVPORT_UTIL_H_
/// @}
