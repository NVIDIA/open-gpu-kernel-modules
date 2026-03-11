/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NVPORT_ATOMIC_C11_H
#define NVPORT_ATOMIC_C11_H

#define PORT_ATOMIC _Atomic

// We only need NVOC to pass _Atomic to generated headers

#if defined(__STRICT_ANSI__)
#error "GNU extensions are required"
#endif

#if PORT_COMPILER_IS_CLANG
// For Android clang
#include "nvport/clang/stdatomic.h"
#else
#include <stdatomic.h>
#endif

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

#define NVPORT_ATOMIC_MEMORY_ORDER_DEFAULT memory_order_seq_cst

// FIXME: use acq/rel? is dmb sy overkill?
#define portAtomicMemoryFenceLoad() atomic_thread_fence(memory_order_seq_cst)
#define portAtomicMemoryFenceStore() atomic_thread_fence(memory_order_seq_cst)
#define portAtomicMemoryFenceFull() atomic_thread_fence(memory_order_seq_cst)

// Helpers to get optional memory_order from VA_ARGS, else use NVPORT_ATOMIC_MEMORY_ORDER_DEFAULT
#define _ATOMIC_CHOOSE_VARIANT(_0, _1, FN, ...) FN
#define _ATOMIC_CHOOSE_VARIANT_2(_0, _1, _2, FN, ...) FN

#define _ATOMIC_SET_DEFAULT(a, v, ...) _ATOMIC_SET_EXPLICIT(a, v, NVPORT_ATOMIC_MEMORY_ORDER_DEFAULT)
#define _ATOMIC_SET_EXPLICIT(a, v, ...) (atomic_store_explicit(a, v, __VA_ARGS__))

#define portAtomicSet(a, v, ...) _ATOMIC_CHOOSE_VARIANT(, ##__VA_ARGS__, _ATOMIC_SET_EXPLICIT, _ATOMIC_SET_DEFAULT)(a, v, __VA_ARGS__)
#define portAtomicSetSize(...) portAtomicSet(__VA_ARGS__)
#define portAtomicSetS32(...) portAtomicSet(__VA_ARGS__)
#define portAtomicSetU32(...) portAtomicSet(__VA_ARGS__)
#define portAtomicExSetS64(...) portAtomicSet(__VA_ARGS__)
#define portAtomicExSetU64(...) portAtomicSet(__VA_ARGS__)

#define _ATOMIC_GET_DEFAULT(a, ...) _ATOMIC_GET_EXPLICIT(a, NVPORT_ATOMIC_MEMORY_ORDER_DEFAULT)
#define _ATOMIC_GET_EXPLICIT(a, ...) (atomic_load_explicit(a, __VA_ARGS__))

#define portAtomicGet(a, ...) _ATOMIC_CHOOSE_VARIANT(, ##__VA_ARGS__, _ATOMIC_GET_EXPLICIT, _ATOMIC_GET_DEFAULT)(a, __VA_ARGS__)
#define portAtomicGetSize(...) portAtomicGet(__VA_ARGS__)

#define _ATOMIC_ADD_DEFAULT(a, b, ...) _ATOMIC_ADD_EXPLICIT(a, b, NVPORT_ATOMIC_MEMORY_ORDER_DEFAULT)
#define _ATOMIC_ADD_EXPLICIT(a, b, ...) ({ __auto_type res = atomic_fetch_add_explicit(a, b, __VA_ARGS__) + b; res; })

#define portAtomicAdd(a, b, ...) _ATOMIC_CHOOSE_VARIANT(, ##__VA_ARGS__, _ATOMIC_ADD_EXPLICIT, _ATOMIC_ADD_DEFAULT)(a, b, __VA_ARGS__)
#define portAtomicAddSize(...) portAtomicAdd(__VA_ARGS__)
#define portAtomicAddS32(...) portAtomicAdd(__VA_ARGS__)
#define portAtomicAddU32(...) portAtomicAdd(__VA_ARGS__)
#define portAtomicExAddS64(...) portAtomicAdd(__VA_ARGS__)
#define portAtomicExAddU64(...) portAtomicAdd(__VA_ARGS__)

#define portAtomicIncrementSize(...) portAtomicIncrement(__VA_ARGS__)
#define portAtomicIncrementS32(a, ...) portAtomicAdd(a, 1, ##__VA_ARGS__)
#define portAtomicIncrementU32(a, ...) portAtomicAdd(a, 1, ##__VA_ARGS__)
#define portAtomicExIncrementS64(a, ...) portAtomicAdd(a, 1, ##__VA_ARGS__)
#define portAtomicExIncrementU64(a, ...) portAtomicAdd(a, 1, ##__VA_ARGS__)

#define _ATOMIC_SUB_DEFAULT(a, b, ...) _ATOMIC_SUB_EXPLICIT(a, b, NVPORT_ATOMIC_MEMORY_ORDER_DEFAULT)
#define _ATOMIC_SUB_EXPLICIT(a, b, ...) ({ __auto_type res = atomic_fetch_sub_explicit(a, b, __VA_ARGS__) - b; res; })

#define portAtomicSub(a, b, ...) _ATOMIC_CHOOSE_VARIANT(, ##__VA_ARGS__, _ATOMIC_SUB_EXPLICIT, _ATOMIC_SUB_DEFAULT)(a, b, __VA_ARGS__)
#define portAtomicSubSize(...) portAtomicSub(__VA_ARGS__)
#define portAtomicSubS32(...) portAtomicSub(__VA_ARGS__)
#define portAtomicSubU32(...) portAtomicSub(__VA_ARGS__)
#define portAtomicExSubS64(...) portAtomicSub(__VA_ARGS__)
#define portAtomicExSubU64(...) portAtomicSub(__VA_ARGS__)

#define portAtomicDecrementS32(a, ...) portAtomicSub(a, 1, ##__VA_ARGS__)
#define portAtomicDecrementU32(a, ...) portAtomicSub(a, 1, ##__VA_ARGS__)
#define portAtomicExDecrementS64(a, ...) portAtomicSub(a, 1, ##__VA_ARGS__)
#define portAtomicExDecrementU64(a, ...) portAtomicSub(a, 1, ##__VA_ARGS__)

#define _ATOMIC_CAS_DEFAULT(pObj, desired, expected, ...) _ATOMIC_CAS_EXPLICIT(pObj, desired, expected, NVPORT_ATOMIC_MEMORY_ORDER_DEFAULT, NVPORT_ATOMIC_MEMORY_ORDER_DEFAULT)
#define _ATOMIC_CAS_EXPLICIT(pObj, desired, expected, ...) ({ __auto_type _lexpected = expected; atomic_compare_exchange_strong_explicit(pObj, &_lexpected, desired, __VA_ARGS__); })

#define portAtomicCompareAndSwap(pObj, desired, expected, ...) _ATOMIC_CHOOSE_VARIANT_2(, ##__VA_ARGS__, _ATOMIC_CAS_EXPLICIT, ERROR, _ATOMIC_CAS_DEFAULT)(pObj, desired, expected, __VA_ARGS__)
#define portAtomicCompareAndSwapSize(...) portAtomicCompareAndSwap(__VA_ARGS__)
#define portAtomicCompareAndSwapS32(...) portAtomicCompareAndSwap(__VA_ARGS__)
#define portAtomicCompareAndSwapU32(...) portAtomicCompareAndSwap(__VA_ARGS__)
#define portAtomicExCompareAndSwapS64(...) portAtomicCompareAndSwap(__VA_ARGS__)
#define portAtomicExCompareAndSwapU64(...) portAtomicCompareAndSwap(__VA_ARGS__)

#define _ATOMIC_XOR_DEFAULT(a, b, ...) _ATOMIC_XOR_EXPLICIT(a, b, NVPORT_ATOMIC_MEMORY_ORDER_DEFAULT)
#define _ATOMIC_XOR_EXPLICIT(a, b, ...) ({ __auto_type res = atomic_fetch_xor_explicit(a, b, __VA_ARGS__) ^ b; res; })

#define portAtomicXor(a, b, ...) _ATOMIC_CHOOSE_VARIANT(, ##__VA_ARGS__, _ATOMIC_XOR_EXPLICIT, _ATOMIC_XOR_DEFAULT)(a, b, __VA_ARGS__)
#define portAtomicXorSize(...) portAtomicXor(__VA_ARGS__)
#define portAtomicXorS32(...) portAtomicXor(__VA_ARGS__)
#define portAtomicXorU32(...) portAtomicXor(__VA_ARGS__)
#define portAtomicExXorS64(...) portAtomicXor(__VA_ARGS__)
#define portAtomicExXorU64(...) portAtomicXor(__VA_ARGS__)

#define _ATOMIC_AND_DEFAULT(a, b, ...) _ATOMIC_AND_EXPLICIT(a, b, NVPORT_ATOMIC_MEMORY_ORDER_DEFAULT)
#define _ATOMIC_AND_EXPLICIT(a, b, ...) ({ __auto_type res = atomic_fetch_and_explicit(a, b, __VA_ARGS__) & b; res; })

#define portAtomicAnd(a, b, ...) _ATOMIC_CHOOSE_VARIANT(, ##__VA_ARGS__, _ATOMIC_AND_EXPLICIT, _ATOMIC_AND_DEFAULT)(a, b, __VA_ARGS__)
#define portAtomicAndSize(...) portAtomicAnd(__VA_ARGS__)
#define portAtomicAndS32(...) portAtomicAnd(__VA_ARGS__)
#define portAtomicAndU32(...) portAtomicAnd(__VA_ARGS__)
#define portAtomicExAndS64(...) portAtomicAnd(__VA_ARGS__)
#define portAtomicExAndU64(...) portAtomicAnd(__VA_ARGS__)

#define _ATOMIC_OR_DEFAULT(a, b, ...) _ATOMIC_OR_EXPLICIT(a, b, NVPORT_ATOMIC_MEMORY_ORDER_DEFAULT)
#define _ATOMIC_OR_EXPLICIT(a, b, ...) ({ __auto_type res = atomic_fetch_or_explicit(a, b, __VA_ARGS__) | b; res; })

#define portAtomicOr(a, b, ...) _ATOMIC_CHOOSE_VARIANT(, ##__VA_ARGS__, _ATOMIC_OR_EXPLICIT, _ATOMIC_OR_DEFAULT)(a, b, __VA_ARGS__)
#define portAtomicOrSize(...) portAtomicOr(__VA_ARGS__)
#define portAtomicOrS32(...) portAtomicOr(__VA_ARGS__)
#define portAtomicOrU32(...) portAtomicOr(__VA_ARGS__)
#define portAtomicExOrS64(...) portAtomicOr(__VA_ARGS__)
#define portAtomicExOrU64(...) portAtomicOr(__VA_ARGS__)

#endif // NVPORT_ATOMIC_C11_H
