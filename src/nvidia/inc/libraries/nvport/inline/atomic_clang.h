/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief Atomic functions implementations using clang compiler intrinsics
 */

#ifndef _NVPORT_ATOMIC_CLANG_H_
#define _NVPORT_ATOMIC_CLANG_H_


#if !(defined(__clang__))
#error "Unsupported compiler: This file can only be compiled by clang"
#endif


PORT_INLINE void
portAtomicMemoryFenceLoad(void)
{
    __c11_atomic_thread_fence(__ATOMIC_SEQ_CST);
}
PORT_INLINE void
portAtomicMemoryFenceStore(void)
{
    __c11_atomic_thread_fence(__ATOMIC_SEQ_CST);
}
PORT_INLINE void
portAtomicMemoryFenceFull(void)
{
    __c11_atomic_thread_fence(__ATOMIC_SEQ_CST);
}
PORT_INLINE void
portAtomicTimerBarrier(void)
{
#if NVCPU_IS_FAMILY_ARM
    __asm__ __volatile__ ("ISB" : : : "memory");
#elif NVCPU_IS_PPC || NVCPU_IS_PPC64LE
    __asm__ __volatile__ ("isync" : : : "memory");
#elif NVCPU_IS_X86 || NVCPU_IS_X86_64
    __asm__ __volatile__ ("lfence" : : : "memory");
#elif NVCPU_IS_RISCV64
    __asm__ __volatile__ ("fence.i" : : : "memory");
#else
#error "portAtomicTimerBarrier implementation not found"
#endif
}

#if PORT_COMPILER_HAS_INTRINSIC_ATOMICS && !defined(NV_MODS) && !NVOS_IS_LIBOS

PORT_ATOMIC_INLINE void
portAtomicInit(void)
{

}

PORT_ATOMIC_INLINE NvS32
portAtomicAddS32
(
    volatile NvS32 *pVal,
    NvS32 val
)
{
    return __c11_atomic_fetch_add((_Atomic NvS32 *)pVal, val, __ATOMIC_SEQ_CST) + val;
}

PORT_ATOMIC_INLINE NvS32
portAtomicSubS32
(
    volatile NvS32 *pVal,
    NvS32 val
)
{
    return __c11_atomic_fetch_sub((_Atomic NvS32 *)pVal,
                                  val, __ATOMIC_SEQ_CST) - val;
}

PORT_ATOMIC_INLINE void
portAtomicSetS32
(
    volatile NvS32 *pVal,
    NvS32 val
)
{
    __c11_atomic_store((_Atomic NvS32 *)pVal, val, __ATOMIC_SEQ_CST);
}

PORT_ATOMIC_INLINE NvBool
portAtomicCompareAndSwapS32
(
    volatile NvS32 *pVal,
    NvS32 newVal,
    NvS32 oldVal
)
{
    NvS32 tmp = oldVal; // Needed so the compiler can still inline this function
    return __c11_atomic_compare_exchange_strong((_Atomic NvS32 *)pVal,
                                                &tmp,
                                                newVal,
                                                __ATOMIC_SEQ_CST,
                                                __ATOMIC_SEQ_CST);
}

PORT_ATOMIC_INLINE NvS32
portAtomicIncrementS32
(
    volatile NvS32 *pVal
)
{
    return portAtomicAddS32(pVal, 1);
}

PORT_ATOMIC_INLINE NvS32
portAtomicDecrementS32
(
    volatile NvS32 *pVal
)
{
    return portAtomicSubS32(pVal, 1);
}

PORT_ATOMIC_INLINE NvS32
portAtomicXorS32
(
    volatile NvS32 *pVal,
    NvS32 val
)
{
    return __c11_atomic_fetch_xor((_Atomic NvS32 *)pVal,
                                  val, __ATOMIC_SEQ_CST) ^ val;
}

PORT_ATOMIC_INLINE NvS32
portAtomicOrS32
(
    volatile NvS32 *pVal,
    NvS32 val
)
{
    return __c11_atomic_fetch_or((_Atomic NvS32 *)pVal,
                                 val, __ATOMIC_SEQ_CST) | val;
}

PORT_ATOMIC_INLINE NvS32
portAtomicAndS32
(
    volatile NvS32 *pVal,
    NvS32 val
)
{
    return __c11_atomic_fetch_and((_Atomic NvS32 *)pVal,
                                 val, __ATOMIC_SEQ_CST) & val;
}


PORT_ATOMIC_INLINE NvU32
portAtomicAddU32
(
    volatile NvU32 *pVal,
    NvU32 val
)
{
    return __c11_atomic_fetch_add((_Atomic NvU32 *)pVal, val, __ATOMIC_SEQ_CST) + val;
}

PORT_ATOMIC_INLINE NvU32
portAtomicSubU32
(
    volatile NvU32 *pVal,
    NvU32 val
)
{
    return __c11_atomic_fetch_sub((_Atomic NvU32 *)pVal,
                                  val, __ATOMIC_SEQ_CST) - val;
}

PORT_ATOMIC_INLINE void
portAtomicSetU32
(
    volatile NvU32 *pVal,
    NvU32 val
)
{
    __c11_atomic_store((_Atomic NvU32 *)pVal, val, __ATOMIC_SEQ_CST);
}

PORT_ATOMIC_INLINE NvBool
portAtomicCompareAndSwapU32
(
    volatile NvU32 *pVal,
    NvU32 newVal,
    NvU32 oldVal
)
{
    NvU32 tmp = oldVal; // Needed so the compiler can still inline this function
    return __c11_atomic_compare_exchange_strong((_Atomic NvU32 *)pVal,
                                                &tmp,
                                                newVal,
                                                __ATOMIC_SEQ_CST,
                                                __ATOMIC_SEQ_CST);
}

PORT_ATOMIC_INLINE NvU32
portAtomicIncrementU32
(
    volatile NvU32 *pVal
)
{
    return portAtomicAddU32(pVal, 1);
}

PORT_ATOMIC_INLINE NvU32
portAtomicDecrementU32
(
    volatile NvU32 *pVal
)
{
    return portAtomicSubU32(pVal, 1);
}

PORT_ATOMIC_INLINE NvU32
portAtomicXorU32
(
    volatile NvU32 *pVal,
    NvU32 val
)
{
    return __c11_atomic_fetch_xor((_Atomic NvU32 *)pVal,
                                  val, __ATOMIC_SEQ_CST) ^ val;
}

PORT_ATOMIC_INLINE NvU32
portAtomicOrU32
(
    volatile NvU32 *pVal,
    NvU32 val
)
{
    return __c11_atomic_fetch_or((_Atomic NvU32 *)pVal,
                                 val, __ATOMIC_SEQ_CST) | val;
}

PORT_ATOMIC_INLINE NvU32
portAtomicAndU32
(
    volatile NvU32 *pVal,
    NvU32 val
)
{
    return __c11_atomic_fetch_and((_Atomic NvU32 *)pVal,
                                 val, __ATOMIC_SEQ_CST) & val;
}


#if NVCPU_IS_64_BITS

PORT_ATOMIC64_INLINE NvS64
portAtomicExAddS64
(
    volatile NvS64 *pVal,
    NvS64 val
)
{
    return __c11_atomic_fetch_add((_Atomic NvS64 *)pVal,
                                  val, __ATOMIC_SEQ_CST) + val;
}

PORT_ATOMIC64_INLINE NvS64
portAtomicExSubS64
(
    volatile NvS64 *pVal,
    NvS64 val
)
{
    return __c11_atomic_fetch_sub((_Atomic NvS64 *)pVal,
                                  val, __ATOMIC_SEQ_CST) - val;
}

PORT_ATOMIC64_INLINE void
portAtomicExSetS64
(
    volatile NvS64 *pVal,
    NvS64 val
)
{
    __c11_atomic_store((_Atomic NvS64 *)pVal, val, __ATOMIC_SEQ_CST);
}

PORT_ATOMIC64_INLINE NvBool
portAtomicExCompareAndSwapS64
(
    volatile NvS64 *pVal, NvS64 newVal, NvS64 oldVal
)
{
    NvS64 tmp = oldVal; // Needed so the compiler can still inline this function
    return __c11_atomic_compare_exchange_strong((_Atomic NvS64 *)pVal,
                                                &tmp,
                                                newVal,
                                                __ATOMIC_SEQ_CST,
                                                __ATOMIC_SEQ_CST);
}

PORT_ATOMIC64_INLINE NvS64
portAtomicExIncrementS64
(
    volatile NvS64 *pVal
)
{
    return portAtomicExAddS64(pVal, 1);
}

PORT_ATOMIC64_INLINE NvS64
portAtomicExDecrementS64
(
    volatile NvS64 *pVal
)
{
    return portAtomicExSubS64(pVal, 1);
}

PORT_ATOMIC64_INLINE NvS64
portAtomicExXorS64
(
    volatile NvS64 *pVal,
    NvS64 val
)
{
    return __c11_atomic_fetch_xor((_Atomic NvS64 *)pVal,
                                 val, __ATOMIC_SEQ_CST) ^ val;
}

PORT_ATOMIC64_INLINE NvS64
portAtomicExOrS64
(
    volatile NvS64 *pVal,
    NvS64 val
)
{
    return __c11_atomic_fetch_or((_Atomic NvS64 *)pVal,
                                 val, __ATOMIC_SEQ_CST) | val;
}

PORT_ATOMIC64_INLINE NvS64
portAtomicExAndS64
(
    volatile NvS64 *pVal,
    NvS64 val
)
{
    return __c11_atomic_fetch_and((_Atomic NvS64 *)pVal,
                                 val, __ATOMIC_SEQ_CST) & val;
}



PORT_ATOMIC64_INLINE NvU64
portAtomicExAddU64
(
    volatile NvU64 *pVal,
    NvU64 val
)
{
    return __c11_atomic_fetch_add((_Atomic NvU64 *)pVal,
                                  val, __ATOMIC_SEQ_CST) + val;
}

PORT_ATOMIC64_INLINE NvU64
portAtomicExSubU64
(
    volatile NvU64 *pVal,
    NvU64 val
)
{
    return __c11_atomic_fetch_sub((_Atomic NvU64 *)pVal,
                                  val, __ATOMIC_SEQ_CST) - val;
}

PORT_ATOMIC64_INLINE void
portAtomicExSetU64
(
    volatile NvU64 *pVal,
    NvU64 val
)
{
    __c11_atomic_store((_Atomic NvU64 *)pVal, val, __ATOMIC_SEQ_CST);
}

PORT_ATOMIC64_INLINE NvBool
portAtomicExCompareAndSwapU64
(
    volatile NvU64 *pVal, NvU64 newVal, NvU64 oldVal
)
{
    NvU64 tmp = oldVal; // Needed so the compiler can still inline this function
    return __c11_atomic_compare_exchange_strong((_Atomic NvU64 *)pVal,
                                                &tmp,
                                                newVal,
                                                __ATOMIC_SEQ_CST,
                                                __ATOMIC_SEQ_CST);
}

PORT_ATOMIC64_INLINE NvU64
portAtomicExIncrementU64
(
    volatile NvU64 *pVal
)
{
    return portAtomicExAddU64(pVal, 1);
}

PORT_ATOMIC64_INLINE NvU64
portAtomicExDecrementU64
(
    volatile NvU64 *pVal
)
{
    return portAtomicExSubU64(pVal, 1);
}

PORT_ATOMIC64_INLINE NvU64
portAtomicExXorU64
(
    volatile NvU64 *pVal,
    NvU64 val
)
{
    return __c11_atomic_fetch_xor((_Atomic NvU64 *)pVal,
                                 val, __ATOMIC_SEQ_CST) ^ val;
}

PORT_ATOMIC64_INLINE NvU64
portAtomicExOrU64
(
    volatile NvU64 *pVal,
    NvU64 val
)
{
    return __c11_atomic_fetch_or((_Atomic NvU64 *)pVal,
                                 val, __ATOMIC_SEQ_CST) | val;
}

PORT_ATOMIC64_INLINE NvU64
portAtomicExAndU64
(
    volatile NvU64 *pVal,
    NvU64 val
)
{
    return __c11_atomic_fetch_and((_Atomic NvU64 *)pVal,
                                 val, __ATOMIC_SEQ_CST) & val;
}


#endif // NVCPU_IS_64_BITS

#endif // PORT_COMPILER_HAS_INTRINSIC_ATOMICS && !defined(NV_MODS) && !NVOS_IS_LIBOS

#endif // _NVPORT_ATOMIC_CLANG_H_
