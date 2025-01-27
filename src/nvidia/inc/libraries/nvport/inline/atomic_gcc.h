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
 * @brief Atomic functions implementations using gcc compiler intrinsics
 */

#ifndef _NVPORT_ATOMIC_GCC_H_
#define _NVPORT_ATOMIC_GCC_H_

#if !NVOS_IS_LIBOS

PORT_INLINE void
portAtomicMemoryFenceStore(void)
{
#if NVCPU_IS_FAMILY_ARM
    __asm__ __volatile__ ("DMB ST" : : : "memory");
#elif NVCPU_IS_PPC || NVCPU_IS_PPC64LE
    __asm__ __volatile__ ("sync" : : : "memory");
#elif NVCPU_IS_X86 || NVCPU_IS_X86_64
    __asm__ __volatile__ ("sfence" : : : "memory");
#elif NVCPU_IS_RISCV64
    __asm__ __volatile__ ("fence" : : : "memory");
#else
#error "portAtomicMemoryFenceStore implementation not found"
#endif
}
PORT_INLINE void
portAtomicMemoryFenceLoad(void)
{
#if NVCPU_IS_FAMILY_ARM
    __asm__ __volatile__ ("DMB SY" : : : "memory");
#elif NVCPU_IS_PPC || NVCPU_IS_PPC64LE
    __asm__ __volatile__ ("sync" : : : "memory");
#elif NVCPU_IS_X86 || NVCPU_IS_X86_64
    __asm__ __volatile__ ("lfence" : : : "memory");
#elif NVCPU_IS_RISCV64
    __asm__ __volatile__ ("fence" : : : "memory");
#else
#error "portAtomicMemoryFenceLoad implementation not found"
#endif
}
PORT_INLINE void
portAtomicMemoryFenceFull(void)
{
#if NVCPU_IS_FAMILY_ARM
    __asm__ __volatile__ ("DMB SY" : : : "memory");
#elif NVCPU_IS_PPC || NVCPU_IS_PPC64LE
    __asm__ __volatile__ ("sync" : : : "memory");
#elif NVCPU_IS_X86 || NVCPU_IS_X86_64
    __asm__ __volatile__ ("mfence" : : : "memory");
#elif NVCPU_IS_RISCV64
    __asm__ __volatile__ ("fence" : : : "memory");
#else
#error "portAtomicMemoryFenceFull implementation not found"
#endif
}

#else

#include "libos_interface.h"
PORT_INLINE void portAtomicMemoryFenceStore(void)
{
    __asm__ __volatile__ ("fence" : : : "memory");
   libosInterfaceSysopFlush();

}

PORT_INLINE void portAtomicMemoryFenceLoad(void)
{
    __asm__ __volatile__ ("fence" : : : "memory");
   libosInterfaceSysopFlush();

}

PORT_INLINE void portAtomicMemoryFenceFull(void)
{
    __asm__ __volatile__ ("fence" : : : "memory");
    libosInterfaceSysopFlush();
}

#endif //!NVOS_IS_LIBOS

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
    return __sync_add_and_fetch(pVal, val);
}

PORT_ATOMIC_INLINE NvS32
portAtomicSubS32
(
    volatile NvS32 *pVal,
    NvS32 val
)
{
    return __sync_sub_and_fetch(pVal, val);
}

PORT_ATOMIC_INLINE void
portAtomicSetS32
(
    volatile NvS32 *pVal,
    NvS32 val
)
{
    while (!__sync_bool_compare_and_swap(pVal, *pVal, val));
}

PORT_ATOMIC_INLINE NvBool
portAtomicCompareAndSwapS32
(
    volatile NvS32 *pVal,
    NvS32 newVal,
    NvS32 oldVal
)
{
    return __sync_bool_compare_and_swap(pVal, oldVal, newVal);
}

PORT_ATOMIC_INLINE NvS32
portAtomicIncrementS32
(
    volatile NvS32 *pVal
)
{
    return __sync_add_and_fetch(pVal, 1);
}

PORT_ATOMIC_INLINE NvS32
portAtomicDecrementS32
(
    volatile NvS32 *pVal
)
{
    return __sync_sub_and_fetch(pVal, 1);
}

PORT_ATOMIC_INLINE NvS32
portAtomicXorS32
(
    volatile NvS32 *pVal,
    NvS32 val
)
{
    return __sync_xor_and_fetch(pVal, val);
}

PORT_ATOMIC_INLINE NvS32
portAtomicOrS32
(
    volatile NvS32 *pVal,
    NvS32 val
)
{
    return __sync_or_and_fetch(pVal, val);
}

PORT_ATOMIC_INLINE NvS32
portAtomicAndS32
(
    volatile NvS32 *pVal,
    NvS32 val
)
{
    return __sync_and_and_fetch(pVal, val);
}


PORT_ATOMIC_INLINE NvU32
portAtomicAddU32
(
    volatile NvU32 *pVal,
    NvU32 val
)
{
    return __sync_add_and_fetch(pVal, val);
}

PORT_ATOMIC_INLINE NvU32
portAtomicSubU32
(
    volatile NvU32 *pVal,
    NvU32 val
)
{
    return __sync_sub_and_fetch(pVal, val);
}

PORT_ATOMIC_INLINE void
portAtomicSetU32
(
    volatile NvU32 *pVal,
    NvU32 val
)
{
    while (!__sync_bool_compare_and_swap(pVal, *pVal, val));
}

PORT_ATOMIC_INLINE NvBool
portAtomicCompareAndSwapU32
(
    volatile NvU32 *pVal,
    NvU32 newVal,
    NvU32 oldVal
)
{
    return __sync_bool_compare_and_swap(pVal, oldVal, newVal);
}

PORT_ATOMIC_INLINE NvU32
portAtomicIncrementU32
(
    volatile NvU32 *pVal
)
{
    return __sync_add_and_fetch(pVal, 1);
}

PORT_ATOMIC_INLINE NvU32
portAtomicDecrementU32
(
    volatile NvU32 *pVal
)
{
    return __sync_sub_and_fetch(pVal, 1);
}

PORT_ATOMIC_INLINE NvU32
portAtomicXorU32
(
    volatile NvU32 *pVal,
    NvU32 val
)
{
    return __sync_xor_and_fetch(pVal, val);
}

PORT_ATOMIC_INLINE NvU32
portAtomicOrU32
(
    volatile NvU32 *pVal,
    NvU32 val
)
{
    return __sync_or_and_fetch(pVal, val);
}

PORT_ATOMIC_INLINE NvU32
portAtomicAndU32
(
    volatile NvU32 *pVal,
    NvU32 val
)
{
    return __sync_and_and_fetch(pVal, val);
}



#if defined(NV_64_BITS)

PORT_ATOMIC64_INLINE NvS64
portAtomicExAddS64
(
    volatile NvS64 *pVal,
    NvS64 val
)
{
    return __sync_add_and_fetch(pVal, val);
}

PORT_ATOMIC64_INLINE NvS64
portAtomicExSubS64
(
    volatile NvS64 *pVal,
    NvS64 val
)
{
    return __sync_sub_and_fetch(pVal, val);
}

PORT_ATOMIC64_INLINE void
portAtomicExSetS64
(
    volatile NvS64 *pVal,
    NvS64 val
)
{
    while (!__sync_bool_compare_and_swap(pVal, *pVal, val));
}

PORT_ATOMIC64_INLINE NvBool
portAtomicExCompareAndSwapS64
(
    volatile NvS64 *pVal,
    NvS64 newVal,
    NvS64 oldVal
)
{
    return __sync_bool_compare_and_swap(pVal, oldVal, newVal);
}

PORT_ATOMIC64_INLINE NvS64
portAtomicExIncrementS64
(
    volatile NvS64 *pVal
)
{
    return __sync_add_and_fetch(pVal, 1);
}

PORT_ATOMIC64_INLINE NvS64
portAtomicExDecrementS64
(
    volatile NvS64 *pVal
)
{
    return __sync_sub_and_fetch(pVal, 1);
}

PORT_ATOMIC64_INLINE NvS64
portAtomicExXorS64
(
    volatile NvS64 *pVal,
    NvS64 val
)
{
    return __sync_xor_and_fetch(pVal, val);
}

PORT_ATOMIC64_INLINE NvS64
portAtomicExOrS64
(
    volatile NvS64 *pVal,
    NvS64 val
)
{
    return __sync_or_and_fetch(pVal, val);
}

PORT_ATOMIC64_INLINE NvS64
portAtomicExAndS64
(
    volatile NvS64 *pVal,
    NvS64 val
)
{
    return __sync_and_and_fetch(pVal, val);
}

PORT_ATOMIC64_INLINE NvU64
portAtomicExAddU64
(
    volatile NvU64 *pVal,
    NvU64 val
)
{
    return __sync_add_and_fetch(pVal, val);
}

PORT_ATOMIC64_INLINE NvU64
portAtomicExSubU64
(
    volatile NvU64 *pVal,
    NvU64 val
)
{
    return __sync_sub_and_fetch(pVal, val);
}

PORT_ATOMIC64_INLINE void
portAtomicExSetU64
(
    volatile NvU64 *pVal,
    NvU64 val
)
{
    while (!__sync_bool_compare_and_swap(pVal, *pVal, val));
}

PORT_ATOMIC64_INLINE NvBool
portAtomicExCompareAndSwapU64
(
    volatile NvU64 *pVal,
    NvU64 newVal,
    NvU64 oldVal
)
{
    return __sync_bool_compare_and_swap(pVal, oldVal, newVal);
}

PORT_ATOMIC64_INLINE NvU64
portAtomicExIncrementU64
(
    volatile NvU64 *pVal
)
{
    return __sync_add_and_fetch(pVal, 1);
}

PORT_ATOMIC64_INLINE NvU64
portAtomicExDecrementU64
(
    volatile NvU64 *pVal
)
{
    return __sync_sub_and_fetch(pVal, 1);
}

PORT_ATOMIC64_INLINE NvU64
portAtomicExXorU64
(
    volatile NvU64 *pVal,
    NvU64 val
)
{
    return __sync_xor_and_fetch(pVal, val);
}

PORT_ATOMIC64_INLINE NvU64
portAtomicExOrU64
(
    volatile NvU64 *pVal,
    NvU64 val
)
{
    return __sync_or_and_fetch(pVal, val);
}

PORT_ATOMIC64_INLINE NvU64
portAtomicExAndU64
(
    volatile NvU64 *pVal,
    NvU64 val
)
{
    return __sync_and_and_fetch(pVal, val);
}

#endif // NV_64_BITS

#endif // PORT_COMPILER_HAS_INTRINSIC_ATOMICS && !defined(NV_MODS) && !NVOS_IS_LIBOS
#endif // _NVPORT_ATOMIC_GCC_H_
