/*
 * SPDX-FileCopyrightText: Copyright (c) 1998,2015,2016 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef __NV_CPU_INTRINSICS_H_
#define __NV_CPU_INTRINSICS_H_

#include <stdint.h>
#include "cpuopsys.h"
#include "nvtypes.h"

/////////////////////////////////////
// Page size
/////////////////////////////////////

#if defined(NV_UNIX) && !defined(NV_CPU_INTRINSICS_KERNEL)
// Page size is dynamic on all Unix systems
#include <unistd.h>
#define __NV_MEM_PAGE_SIZE_BYTES getpagesize()
#else
// And is static for all other known architectures.
#define __NV_MEM_PAGE_SIZE_BYTES 4096
#endif // defined(NV_UNIX)

#define __NV_MEM_PAGE_SIZE_MASK (__NV_MEM_PAGE_SIZE_BYTES - 1)

#define __NV_PAGE_PAD(x) \
    (((x) + __NV_MEM_PAGE_SIZE_MASK) & ~(__NV_MEM_PAGE_SIZE_MASK))

/////////////////////////////////////
// Cache line size
/////////////////////////////////////

#if defined(NVCPU_PPC)
#define __NV_CACHE_LINE_BYTES  32
#else
#define __NV_CACHE_LINE_BYTES  64
#endif

/////////////////////////////////////
// Spin loop hint
/////////////////////////////////////

#if defined(NVCPU_X86_64)

// PAUSE (aka REP NOP) opcode is low-power on x86_64
#if defined(NV_GNU_INLINE_ASM)
#define NV_SPIN_LOOP_HINT() {                                           \
    asm(".byte 0xf3\n\t"                                                \
        ".byte 0x90\n\t");                                              \
}
#else
#define NV_SPIN_LOOP_HINT() _mm_pause()
#endif

#elif defined(NVCPU_X86)

// PAUSE (aka REP NOP) opcode is low-power on P4's
#if defined(NV_GNU_INLINE_ASM)
#define NV_SPIN_LOOP_HINT() {                                           \
    asm(".byte 0xf3\n\t"                                                \
        ".byte 0x90\n\t");                                              \
}
#else
#define NV_SPIN_LOOP_HINT() _mm_pause()
#endif

#elif defined(NVCPU_PPC)

#define NV_PPC_CACHE_LINE_SIZE_IN_BYTES    32
#define NV_PPC_CACHE_LINE_SIZE_IN_U32S      8

// Not implemented yet
#define NV_SPIN_LOOP_HINT()

#elif defined(NVCPU_FAMILY_ARM) || defined(NVCPU_PPC64LE) || defined(NVCPU_RISCV64)

// Not implemented yet
#define NV_SPIN_LOOP_HINT()

#else
#error Unknown CPU type
#endif

/////////////////////////////////////
// Atomic operations
/////////////////////////////////////

#if defined(__GNUC__) || defined(__clang__)

// Include stdbool.h to pick up a definition of false to use with the
// __atomic_* intrinsics below.
#if !defined(__cplusplus)
#include <stdbool.h>
#endif // !defined(__cplusplus)

// Sets a 32-bit variable to the specified value as an atomic operation.
// The function returns the initial value of the destination memory location.
static NV_FORCEINLINE int __NVatomicExchange(volatile int *location, int value)
{
    return __sync_lock_test_and_set(location, value);
}

// Sets a pointer variable to the specified value as an atomic operation.
// The function returns the initial value of the destination memory location.
static NV_FORCEINLINE void* __NVatomicExchangePointer(void * volatile *location, void *value)
{
    return __sync_lock_test_and_set(location, value);
}

// Performs an atomic compare-and-exchange operation on the specified values. The function compares two
// specified 32-bit values and exchanges with another 32-bit value based on the outcome of the comparison.
// The function returns the initial value of the destination memory location.
static NV_FORCEINLINE int __NVatomicCompareExchange(int volatile *location, int newValue, int oldValue)
{
    return __sync_val_compare_and_swap(location, oldValue, newValue);
}

// Performs an atomic compare-and-exchange operation on the specified values. The function compares two
// specified 64-bit values and exchanges with another 64-bit value based on the outcome of the comparison.
// The function returns the initial value of the destination memory location.
static NV_FORCEINLINE NvS64 __NVatomicCompareExchange64(NvS64 volatile *location, NvS64 newValue, NvS64 oldValue)
{
#if NVCPU_IS_ARM && !defined(__clang__)
    // GCC doesn't provided an ARMv7 64-bit sync-and-swap intrinsic, so define
    // one using inline assembly.
    NvU32 oldValLow = NvU64_LO32(oldValue);
    NvU32 oldValHigh = NvU64_HI32(oldValue);
    NvU32 newValLow = NvU64_LO32(newValue);
    NvU32 newValHigh = NvU64_HI32(newValue);
    NvU32 outValLow;
    NvU32 outValHigh;
    NvU32 res;

    // The ldrexd and strexd instructions require use of an adjacent even/odd
    // pair of registers.  GCC supports quad-word register operands and
    // modifiers to enable assignment of 64-bit values to two suitable 32-bit
    // registers, but Clang does not.  To work around this, explicitly request
    // some suitable registers in the clobber list and manually shift the
    // necessary data in/out of them as needed.
    __asm__ __volatile__ (
         "1:  ldrexd r2, r3, [%[loc]]\n"
         "    mov %[res], #0\n"
         "    mov %[outLo], r2\n"
         "    mov %[outHi], r3\n"
         "    mov r2, %[newLo]\n"
         "    mov r3, %[newHi]\n"
         "    teq %[outLo], %[oldLo]\n"
         "    itt eq\n"
         "    teqeq %[outHi], %[oldHi]\n"
         "    strexdeq %[res], r2, r3, [%[loc]]\n"
         "    teq %[res], #0\n"
         "    bne 1b\n"
         // Outputs
         : [res] "=&r" (res),
           [outLo] "=&r" (outValLow), [outHi] "=&r" (outValHigh),
           "+Qo" (*location)
         // Inputs
         : [loc] "r" (location),
           [oldLo] "r" (oldValLow), [oldHi] "r" (oldValHigh),
           [newLo] "r" (newValLow), [newHi] "r" (newValHigh)
         // Clobbers
         : "memory", "cc", "r2", "r3");

    __asm__ __volatile__ ("dmb" ::: "memory");

    return (NvS64)(((NvU64)outValHigh << 32llu) | (NvU64)outValLow);
#else
    __atomic_compare_exchange_n(location, &oldValue, newValue, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return oldValue;
#endif
}

// Performs an atomic compare-and-exchange operation on the specified values. The function compares two
// specified pointer values and exchanges with another pointer value based on the outcome of the comparison.
// The function returns the initial value of the destination memory location.
static NV_FORCEINLINE void* __NVatomicCompareExchangePointer(void * volatile *location, void *newValue, void *oldValue)
{
    return __sync_val_compare_and_swap(location, oldValue, newValue);
}

// Increments (increases by one) the value of the specified 32-bit variable as an atomic operation.
// The function returns the resulting incremented value.
static NV_FORCEINLINE int __NVatomicIncrement(int volatile *location)
{
    return __sync_add_and_fetch(location, 1);
}

// Decrements (decreases by one) the value of the specified 32-bit variable as an atomic operation.
// The function returns the resulting decremented value.
static NV_FORCEINLINE int __NVatomicDecrement(int volatile *location)
{
    return __sync_sub_and_fetch(location, 1);
}

// Adds the values of the specified 32-bit variables as an atomic operation.
// The function returns the resulting added value.
static NV_FORCEINLINE int __NVatomicExchangeAdd(int volatile *location, int value)
{
    return __sync_add_and_fetch(location, value);
}

#ifdef NV_CPU_QUERY_LSE_CAPS
/*
 * Embedding hand coded instructions only for the inc/dec calls. These are the ones that
 * get called very often. The __NVatomicCompareExchange() and other calls for example,
 * are called only at init time, and a few times at most. So, keeping this hand-coding
 * minimal, and to only the most used ones.
 *
 * Disassembly for reference:
 *  b820003e  ldadd   w0, w30, [x1]
 *  0b1e0000  add     w0, w0, w30
 *
 * x16, x17, x30 are added to the clobber list since there could be veneers that maybe
 * generated.
 */
static NV_FORCEINLINE int __NVatomicIncrement_LSE(int volatile *location)
{
    register int w0 asm ("w0") = 1;
    register volatile int *x1 asm ("x1") = location;

    asm volatile
    (
        ".inst 0xb820003e \n"
        "add   w0, w0, w30"
        : "+r" (w0), "+r" (x1)
        : "r" (x1)
        : "x16", "x17", "x30", "memory"
    );

    return w0;
}

static NV_FORCEINLINE int __NVatomicDecrement_LSE(int volatile *location)
{
    register int w0 asm ("w0") = (int32_t)-1;
    register volatile int *x1 asm ("x1") = location;

    asm volatile
    (
        ".inst 0xb820003e \n"
        "add   w0, w0, w30"
        : "+r" (w0), "+r" (x1)
        : "r" (x1)
        : "x16", "x17", "x30", "memory"
    );

    return w0;
}
#endif

#else

#error undefined architecture

#endif

/////////////////////////////////////
// Bit scan operations
/////////////////////////////////////

// __NV_clz(mask) provides a generic count-leading-zeros. If "mask" is 0, then the return value is undefined.
// __NV_ctz(mask) provides a generic count-trailing-zeros. If "mask" is 0, then the return value is undefined.
//
// __NVbsfFirst(), __NVbsfNext(), __NVbsrFirst() and __NVbsrNext() are helper functions to implement
// generic bit scan operations over a 32 bit mask long the following template:
//
//     for (__NVbsfFirst(&index, &mask, maskInit); mask; __NVbsfNext(&index, &mask)) { ... }
//
// These operations are implemented using gcc/MSVC builtins/intrinsics. A test program to verify the correct
// functionality of these routines is available at //sw/pvt/ddadap/bitscantest.c
//
// The scan process provides the next valid "index". In __NVbsfNext() the bit corresponding to the passed in
// (1 << index) will be masked out.
//
// bsf scan from the lsb to the msb, while bsr scans from the msb to the lsb.
//
// The use of inlines and defines below is dictated by insufficiencies of MSVC ...

#if defined (__GNUC__) || defined(__clang__)

static NV_FORCEINLINE int __NV_clz(unsigned int mask) {
    return __builtin_clz(mask);
}

static NV_FORCEINLINE int __NV_ctz(unsigned int mask) {
    return __builtin_ctz(mask);
}

static NV_FORCEINLINE int __NV_clzll(unsigned long long mask) {
    return __builtin_clzll(mask);
}

static NV_FORCEINLINE int __NV_ctzll(unsigned long long mask) {
    return __builtin_ctzll(mask);
}

#define __BitScanForward(_pindex, _mask) *((_pindex)) = __NV_ctz((_mask))
#define __BitScanReverse(_pindex, _mask) *((_pindex)) = 31 - __NV_clz((_mask))
#define __BitScanForward64(_pindex, _mask) *((_pindex)) = __NV_ctzll((_mask))
#define __BitScanReverse64(_pindex, _mask) *((_pindex)) = 63 - __NV_clzll((_mask))

#else

#error Unsupported compiler

#endif // MSVC_VER

#ifndef __BitScanForward64
// Implement bit scan forward for 64 bit using 32 bit instructions
static NV_FORCEINLINE void _BitScanForward64on32(unsigned int *index, NvU64 mask)
{
    const unsigned int lowMask = (unsigned int)(mask & 0xFFFFFFFFULL);

    if (lowMask != 0) {
        __BitScanForward(index, lowMask);
    } else {
        const unsigned int highMask = (unsigned int)(mask >> 32);
        __BitScanForward(index, highMask);
        *index += 32;
    }
}

#define __BitScanForward64(_pindex, _mask) _BitScanForward64on32((_pindex), (_mask))
#endif // __BitScanForward64

#ifndef __BitScanReverse64
// Implement bit scan reverse for 64 bit using 32 bit instructions
static NV_FORCEINLINE void _BitScanReverse64on32(unsigned int *index, NvU64 mask)
{
    const unsigned int highMask = (unsigned int)(mask >> 32);

    if (highMask != 0) {
        __BitScanReverse(index, highMask);
        *index += 32;
    } else {
        const unsigned int lowMask = (unsigned int)(mask & 0xFFFFFFFFULL);
        __BitScanReverse(index, lowMask);
    }
}

#define __BitScanReverse64(_pindex, _mask) _BitScanReverse64on32((_pindex), (_mask))
#endif // __BitScanReverse64

static NV_FORCEINLINE void __NVbsfFirst(unsigned int *pindex, unsigned int *pmask, unsigned int maskInit)
{
    *pmask = maskInit;
    __BitScanForward(pindex, maskInit);
}

static NV_FORCEINLINE void __NVbsfNext(unsigned int *pindex, unsigned int *pmask)
{
    unsigned int index, mask;

    index = *pindex;
    mask = *pmask ^ (1ul << index);

    *pmask = mask;
    __BitScanForward(pindex, mask);
}

static NV_FORCEINLINE void __NVbsrFirst(unsigned int *pindex, unsigned int *pmask, unsigned int maskInit)
{
    *pmask = maskInit;
    __BitScanReverse(pindex, maskInit);
}

static NV_FORCEINLINE void __NVbsrNext(unsigned int *pindex, unsigned int *pmask)
{
    unsigned int index, mask;

    index = *pindex;
    mask = *pmask ^ (1ul << index);

    *pmask = mask;
    __BitScanReverse(pindex, mask);
}

// Variations for 64 bit maks
static NV_FORCEINLINE void __NVbsfFirst64(unsigned int *pindex, NvU64 *pmask, NvU64 maskInit)
{
    *pmask = maskInit;
    __BitScanForward64(pindex, maskInit);
}

static NV_FORCEINLINE void __NVbsfNext64(unsigned int *pindex, NvU64 *pmask)
{
    unsigned int index;
    NvU64 mask;

    index = *pindex;
    mask = *pmask ^ (1ULL << index);

    *pmask = mask;
    __BitScanForward64(pindex, mask);
}

static NV_FORCEINLINE void __NVbsrFirst64(unsigned int *pindex, NvU64 *pmask, NvU64 maskInit)
{
    *pmask = maskInit;
    __BitScanReverse64(pindex, maskInit);
}

static NV_FORCEINLINE void __NVbsrNext64(unsigned int *pindex, NvU64 *pmask)
{
    unsigned int index;
    NvU64 mask;

    index = *pindex;
    mask = *pmask ^ (1ULL << index);

    *pmask = mask;
    __BitScanReverse64(pindex, mask);
}

#undef __BitScanForward
#undef __BitScanReverse
#undef __BitScanForward64
#undef __BitScanReverse64

#endif // __NV_CPU_INTRINSICS_H_
