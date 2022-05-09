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
 * @brief Atomic module public interface
 */

#ifndef _NVPORT_H_
#error "This file cannot be included directly. Include nvport.h instead."
#endif

#ifndef _NVPORT_ATOMIC_H_
#define _NVPORT_ATOMIC_H_
/**
 * @defgroup NVPORT_ATOMIC Atomic operations
 *
 * @brief This module contains atomic operations
 *
 * @note that mixing atomic and non-atomic modifications to the same memory
 * location can have undefined behavior that varies from platform to platform.
 * You are better off not trying it.
 *
 * @note All atomic operations actually impose at least a compiler memory
 * barrier - either just on the variable manipulated, or on all globally
 * accessible variables. This is just a consequence of the current
 * implementations, and should not be relied on. If you need a memory barrier,
 * use @ref portAtomicMemFenceFull.
 *
 * @{
 */

/** See @ref PORT_UTIL_INLINE */
#ifndef PORT_ATOMIC_INLINE
#if PORT_COMPILER_HAS_INTRINSIC_ATOMICS && !defined(NV_MODS)
#define PORT_ATOMIC_INLINE PORT_INLINE
#if NVCPU_IS_64_BITS
#define PORT_ATOMIC64_INLINE PORT_INLINE
#else
#define PORT_ATOMIC64_INLINE
#endif
#else
#define PORT_ATOMIC_INLINE
#define PORT_ATOMIC64_INLINE
#endif
#endif

/**
 * @name Core Functions
 * @{
 */

/**
 * @brief Atomic addition on a signed 32b integer
 *
 * This function is the atomic equivalent to:
 *
 * ~~~{.c}
 * *pVal += val;
 * return *pVal;
 * ~~~
 *
 * @return the new value of `*pVal`
 */
PORT_ATOMIC_INLINE NvS32 portAtomicAddS32(volatile NvS32 *pVal, NvS32 val);
/// @brief Unsigned version of @ref portAtomicAddS32
PORT_ATOMIC_INLINE NvU32 portAtomicAddU32(volatile NvU32 *pVal, NvU32 val);

/**
 * @brief Atomic subtraction on a signed 32b integer
 *
 * This function is the atomic equivalent to:
 *
 * ~~~{.c}
 * *pVal -= val;
 * return *pVal;
 * ~~~
 *
 * @return the new value of `*pVal`
 */
PORT_ATOMIC_INLINE NvS32 portAtomicSubS32(volatile NvS32 *pVal, NvS32 val);
/// @brief Unsigned version of @ref portAtomicSubS32
PORT_ATOMIC_INLINE NvU32 portAtomicSubU32(volatile NvU32 *pVal, NvU32 val);

/**
 * @brief Atomic set a signed 32b integer to the specified value
 *
 * This function is the atomic equivalent to:
 *
 * ~~~{.c}
 * *pVal = val;
 * ~~~
 *
 * Once complete `val` will be visible in the location pointed to by `pVal` by all
 * threads on all processors.
 *
 * @note On some platforms this operation is different from other atomic
 * operations with respect to memory ordering.  The best that can be guaranteed
 * for this operation that it will behave as an acquire barrier.  This
 * means that operations occurring after this one in program order are
 * guaranteed to not occur until the atomic operation is complete.  It also
 * means that it does not guarantee that previous stores are visible, or that
 * previous loads have been satisfied.
 *
 */
PORT_ATOMIC_INLINE void  portAtomicSetS32(volatile NvS32 *pVal, NvS32 val);
/// @brief Unsigned version of @ref portAtomicSetS32
PORT_ATOMIC_INLINE void  portAtomicSetU32(volatile NvU32 *pVal, NvU32 val);

/**
 * @brief Atomic compare-and-swap on a signed 32b integer.
 *
 * A compare and swap is an atomic operation that reads a memory location,
 * compares it to `oldVal` and if they are equal sets the memory location to
 * `newVal`.
 *
 * This function is the atomic equivalent to:
 *
 * ~~~{.c}
 * if (*pVal == oldVal)
 * {
 *     *pVal = newVal;
 *     return NV_TRUE;
 * }
 * return NV_FALSE;
 * ~~~
 *
 * @return NV_TRUE if the operation modified the target of `pVal`, NV_FALSE otherwise
 *
 */
PORT_ATOMIC_INLINE NvBool portAtomicCompareAndSwapS32(volatile NvS32 *pVal, NvS32 newVal, NvS32 oldVal);
/// @brief Unsigned version of @ref portAtomicCompareAndSwapS32
PORT_ATOMIC_INLINE NvBool portAtomicCompareAndSwapU32(volatile NvU32 *pVal, NvU32 newVal, NvU32 oldVal);

/**
 * @brief Atomic increments of a signed 32b integer.
 *
 * Adds one to the memory location pointed to by the parameter and returns the
 * resulting value.
 *
 * This function is the atomic equivalent to:
 *
 * ~~~{.c}
 * ++(*pVal);
 * return *pVal;
 * ~~~
 *
 * @return the new value of `*pVal`
 *
 */
PORT_ATOMIC_INLINE NvS32 portAtomicIncrementS32(volatile NvS32 *pVal);
/// @brief Unsigned version of @ref portAtomicIncrementS32
PORT_ATOMIC_INLINE NvU32 portAtomicIncrementU32(volatile NvU32 *pVal);

/**
 * @brief Atomic decrements of a signed 32b integer.
 *
 * Subtracts one to the memory location pointed to by the parameter and returns
 * the resulting value.
 *
 * This function is the atomic equivalent to:
 *
 * ~~~{.c}
 * --(*pVal);
 * return *pVal;
 * ~~~
 *
 * @return the new value of `*pVal`
 */
PORT_ATOMIC_INLINE NvS32 portAtomicDecrementS32(volatile NvS32 *pVal);
/// @brief Unsigned version of @ref portAtomicDecrementS32
PORT_ATOMIC_INLINE NvU32 portAtomicDecrementU32(volatile NvU32 *pVal);


/**
 * @brief Atomic bitwise XOR on a signed 32b integer.
 *
 * This function is the atomic equivalent to:
 *
 * ~~~{.c}
 * *pVal ^= val;
 * return *pVal;
 * ~~~
 *
 * @return the new value of `*pVal`
 */
PORT_ATOMIC_INLINE NvS32 portAtomicXorS32(volatile NvS32 *pVal, NvS32 val);
/// @brief Unsigned version of @ref portAtomicXorS32
PORT_ATOMIC_INLINE NvU32 portAtomicXorU32(volatile NvU32 *pVal, NvU32 val);

/**
 * @brief Atomic bitwise OR on a signed 32b integer.
 *
 * This function is the atomic equivalent to:
 *
 * ~~~{.c}
 * *pVal |= val;
 * return *pVal;
 * ~~~
 *
 * @return the new value of `*pVal`
 */
PORT_ATOMIC_INLINE NvS32 portAtomicOrS32(volatile NvS32 *pVal, NvS32 val);
/// @brief Unsigned version of @ref portAtomicOrS32
PORT_ATOMIC_INLINE NvU32 portAtomicOrU32(volatile NvU32 *pVal, NvU32 val);

/**
 * @brief Atomic bitwise AND on a signed 32b integer.
 *
 * This function is the atomic equivalent to:
 *
 * ~~~{.c}
 * *pVal &= val;
 * return *pVal;
 * ~~~
 *
 * @return the new value of `*pVal`
 */
PORT_ATOMIC_INLINE NvS32 portAtomicAndS32(volatile NvS32 *pVal, NvS32 val);
/// @brief Unsigned version of @ref portAtomicAndS32
PORT_ATOMIC_INLINE NvU32 portAtomicAndU32(volatile NvU32 *pVal, NvU32 val);



/**
 * @name Memory Barrier functions
 * @note Memory fence functions must be marked inline, so the compiler knows
 * about the barrier and doesn't reorder instructions around the call. Thus,
 * this is PORT_INLINE and not PORT_ATOMIC_INLINE. (Force inline not necessary)
 *
 * @note A given platform is allowed to implement the load/store barriers as
 * full barriers instead, if the former isn't supported. Thus, you should only
 * use @ref portAtomicMemoryFenceLoad and @ref portAtomicMemoryFenceStore for
 * possible performance bonus over @ref portAtomicMemoryFenceFull. Don't write
 * code that relies on those being load/store only barriers.
 *
 * @{
 */

/**
 * @brief Creates a full HW and compiler memory barrier.
 *
 * A memory fence (memory barrier) imposes a sequential ordering on access to
 * all globally accessible variables. That means that all accesses found before
 * the fence will finish before any of those after it.
 */
PORT_INLINE void portAtomicMemoryFenceFull(void);
/**
 * @brief Creates a HW and compiler load memory barrier.
 *
 * A load memory fence (memory barrier) imposes a sequential ordering on all
 * loads to globally accessible variables. All loads found before the barrier
 * will happen before any loads found after it. A load barrier has no effect on
 * store operations.
 */
PORT_INLINE void portAtomicMemoryFenceLoad(void);
/**
 * @brief Creates a HW and compiler store memory barrier.
 *
 * A store memory fence (memory barrier) imposes a sequential ordering on all
 * stores to globally accessible variables. All stores found before the barrier
 * will happen before any stores found after it. A store barrier has no effect
 * on load operations.
 */
PORT_INLINE void portAtomicMemoryFenceStore(void);
/// @} End memory barrier functions

/// @} End core functions

/**
 * @name Extended Functions
 * @{
 */

#ifndef PORT_ATOMIC_64_BIT_SUPPORTED
/// @note We support 64bit atomics on all 64bit systems (and some 32bit)
#define PORT_ATOMIC_64_BIT_SUPPORTED NVCPU_IS_64_BITS
#endif

#define portAtomicExAddS64_SUPPORTED            PORT_ATOMIC_64_BIT_SUPPORTED
#define portAtomicExSubS64_SUPPORTED            PORT_ATOMIC_64_BIT_SUPPORTED
#define portAtomicExSetS64_SUPPORTED            PORT_ATOMIC_64_BIT_SUPPORTED
#define portAtomicExCompareAndSwapS64_SUPPORTED PORT_ATOMIC_64_BIT_SUPPORTED
#define portAtomicExIncrementS64_SUPPORTED      PORT_ATOMIC_64_BIT_SUPPORTED
#define portAtomicExDecrementS64_SUPPORTED      PORT_ATOMIC_64_BIT_SUPPORTED
#define portAtomicExXorS64_SUPPORTED            PORT_ATOMIC_64_BIT_SUPPORTED
#define portAtomicExOrS64_SUPPORTED             PORT_ATOMIC_64_BIT_SUPPORTED
#define portAtomicExAndS64_SUPPORTED            PORT_ATOMIC_64_BIT_SUPPORTED

#if PORT_ATOMIC_64_BIT_SUPPORTED
/**
 * @brief Like @ref portAtomicAddS32, except operating on 64bit integers
 */
PORT_ATOMIC64_INLINE NvS64 portAtomicExAddS64(volatile NvS64 *pVal, NvS64 val);
/// @brief Unsigned version of @ref portAtomicExAddS64
PORT_ATOMIC64_INLINE NvU64 portAtomicExAddU64(volatile NvU64 *pVal, NvU64 val);
/**
 * @brief Like @ref portAtomicSubS32, except operating on 64bit integers
 */
PORT_ATOMIC64_INLINE NvS64 portAtomicExSubS64(volatile NvS64 *pVal, NvS64 val);
/// @brief Unsigned version of @ref portAtomicExSubS64
PORT_ATOMIC64_INLINE NvU64 portAtomicExSubU64(volatile NvU64 *pVal, NvU64 val);
/**
 * @brief Like @ref portAtomicSetS32, except operating on 64bit integers
 */
PORT_ATOMIC64_INLINE void  portAtomicExSetS64(volatile NvS64 *pVal, NvS64 val);
/// @brief Unsigned version of @ref portAtomicExSetS64
PORT_ATOMIC64_INLINE void  portAtomicExSetU64(volatile NvU64 *pVal, NvU64 val);
/**
 * @brief Like @ref portAtomicCompareAndSwapS32, except operating on 64bit integers
 */
PORT_ATOMIC64_INLINE NvBool portAtomicExCompareAndSwapS64(volatile NvS64 *pVal, NvS64 newVal, NvS64 oldVal);
/// @brief Unsigned version of @ref portAtomicExCompareAndSwapS64
PORT_ATOMIC64_INLINE NvBool portAtomicExCompareAndSwapU64(volatile NvU64 *pVal, NvU64 newVal, NvU64 oldVal);
/**
 * @brief Like @ref portAtomicIncrementS32, except operating on 64bit integers
 */
PORT_ATOMIC64_INLINE NvS64 portAtomicExIncrementS64(volatile NvS64 *pVal);
/// @brief Unsigned version of @ref portAtomicExIncrementS64
PORT_ATOMIC64_INLINE NvU64 portAtomicExIncrementU64(volatile NvU64 *pVal);
/**
 * @brief Like @ref portAtomicDecrementS32, except operating on 64bit integers
 */
PORT_ATOMIC64_INLINE NvS64 portAtomicExDecrementS64(volatile NvS64 *pVal);
/// @brief Unsigned version of @ref portAtomicExDecrementS64
PORT_ATOMIC64_INLINE NvU64 portAtomicExDecrementU64(volatile NvU64 *pVal);
/**
 * @brief Like @ref portAtomicXorS32, except operating on 64bit integers
 */
PORT_ATOMIC64_INLINE NvS64 portAtomicExXorS64(volatile NvS64 *pVal, NvS64 val);
/// @brief Unsigned version of @ref portAtomicExXorS64
PORT_ATOMIC64_INLINE NvU64 portAtomicExXorU64(volatile NvU64 *pVal, NvU64 val);
/**
 * @brief Like @ref portAtomicOrS32, except operating on 64bit integers
 */
PORT_ATOMIC64_INLINE NvS64 portAtomicExOrS64(volatile NvS64 *pVal, NvS64 val);
/// @brief Unsigned version of @ref portAtomicExOrS64
PORT_ATOMIC64_INLINE NvU64 portAtomicExOrU64(volatile NvU64 *pVal, NvU64 val);
/**
 * @brief Like @ref portAtomicAndS32, except operating on 64bit integers
 */
PORT_ATOMIC64_INLINE NvS64 portAtomicExAndS64(volatile NvS64 *pVal, NvS64 val);
/// @brief Unsigned version of @ref portAtomicExAndS64
PORT_ATOMIC64_INLINE NvU64 portAtomicExAndU64(volatile NvU64 *pVal, NvU64 val);

#endif // PORT_ATOMIC_64_BIT_SUPPORTED

/// @} End extended functions

/**
 * Platform-specific inline implementations
 */
#if NVOS_IS_LIBOS
#include "nvport/inline/atomic_libos.h"
#endif

#if PORT_COMPILER_IS_GCC
#include "nvport/inline/atomic_gcc.h"
#elif PORT_COMPILER_IS_CLANG
#include "nvport/inline/atomic_clang.h"
#elif PORT_COMPILER_IS_MSVC
#include "nvport/inline/atomic_msvc.h"
#endif


/**
 * @name Utility Functions
 *
 * These are utility functions for performing operations on pointer sized
 * operands. While the 64bit functions are "extended", they should always be
 * present on systems where pointers and NvLength are 64 bits.
 * @{
 */
#if !NVCPU_IS_64_BITS
#define portAtomicAddSize(a,b)              (NvSPtr)portAtomicAddS32((volatile NvSPtr *)a, (NvSPtr)b)
#define portAtomicSubSize(a,b)              (NvSPtr)portAtomicSubS32((volatile NvSPtr *)a, (NvSPtr)b)
#define portAtomicSetSize(a,b)              portAtomicSetS32((volatile NvSPtr *)a, (NvSPtr)b)
#define portAtomicCompareAndSwapSize(a,b,c) portAtomicCompareAndSwapS32((volatile NvSPtr *)a, (NvSPtr)b, (NvSPtr)c)
#define portAtomicIncrementSize(a)          (NvSPtr)portAtomicIncrementS32((volatile NvSPtr *)a)
#define portAtomicDecrementSize(a)          (NvSPtr)portAtomicDecrementS32((volatile NvSPtr *)a)
#define portAtomicXorSize(a,b)              (NvSPtr)portAtomicXorS32((volatile NvSPtr *)a, (NvSPtr)b)
#define portAtomicOrSize(a,b)               (NvSPtr)portAtomicOrS32((volatile NvSPtr *)a, (NvSPtr)b)
#define portAtomicAndSize(a,b)              (NvSPtr)portAtomicAndS32((volatile NvSPtr *)a, (NvSPtr)b)
#else
#define portAtomicAddSize(a,b)              (NvSPtr)portAtomicExAddS64((volatile NvSPtr *)a, (NvSPtr)b)
#define portAtomicSubSize(a,b)              (NvSPtr)portAtomicExSubS64((volatile NvSPtr *)a, (NvSPtr)b)
#define portAtomicSetSize(a,b)              portAtomicExSetS64((volatile NvSPtr *)a, (NvSPtr)b)
#define portAtomicCompareAndSwapSize(a,b,c) portAtomicExCompareAndSwapS64((volatile NvSPtr *)a, (NvSPtr)b, (NvSPtr)c)
#define portAtomicIncrementSize(a)          (NvSPtr)portAtomicExIncrementS64((volatile NvSPtr *)a)
#define portAtomicDecrementSize(a)          (NvSPtr)portAtomicExDecrementS64((volatile NvSPtr *)a)
#define portAtomicXorSize(a,b)              (NvSPtr)portAtomicExXorS64((volatile NvSPtr *)a, (NvSPtr)b)
#define portAtomicOrSize(a,b)               (NvSPtr)portAtomicExOrS64((volatile NvSPtr *)a, (NvSPtr)b)
#define portAtomicAndSize(a,b)              (NvSPtr)portAtomicExAndS64((volatile NvSPtr *)a, (NvSPtr)b)
#endif
/// @}

#endif // _NVPORT_ATOMIC_H_
/// @}
