/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NVTYPES_INCLUDED
#define NVTYPES_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "cpuopsys.h"

#ifndef NVTYPES_USE_STDINT
#define NVTYPES_USE_STDINT 0
#endif

#if NVTYPES_USE_STDINT
#ifdef __cplusplus
#include <cstdint>
#include <cinttypes>
#else
#include <stdint.h>
#include <inttypes.h>
#endif // __cplusplus
#endif // NVTYPES_USE_STDINT

#ifndef __cplusplus
// Header includes to make sure wchar_t is defined for C-file compilation
// (C++ is not affected as it is a fundamental type there)
// _MSC_VER is a hack to avoid  failures for old setup of UEFI builds which are
//  currently set to msvc100 but do not properly set the include paths
#endif // __cplusplus

#if defined(MAKE_NV64TYPES_8BYTES_ALIGNED) && defined(__i386__)
// ensure or force 8-bytes alignment of NV 64-bit types
#define OPTIONAL_ALIGN8_ATTR __attribute__((aligned(8)))
#else
// nothing needed
#define OPTIONAL_ALIGN8_ATTR
#endif // MAKE_NV64TYPES_8BYTES_ALIGNED && i386

 /***************************************************************************\
|*                                 Typedefs                                  *|
 \***************************************************************************/

#ifdef NV_MISRA_COMPLIANCE_REQUIRED
//Typedefs for MISRA COMPLIANCE
typedef unsigned long long   UInt64;
typedef   signed long long    Int64;
typedef unsigned int         UInt32;
typedef   signed int          Int32;
typedef unsigned short       UInt16;
typedef   signed short        Int16;
typedef unsigned char        UInt8 ;
typedef   signed char         Int8 ;

typedef          void          Void;
typedef          float    float32_t;
typedef          double   float64_t;
#endif


// Floating point types
#ifdef NV_MISRA_COMPLIANCE_REQUIRED
typedef float32_t          NvF32;                      /* IEEE Single Precision (S1E8M23)         */
typedef float64_t          NvF64 OPTIONAL_ALIGN8_ATTR; /* IEEE Double Precision (S1E11M52)        */
#else
typedef float              NvF32;                      /* IEEE Single Precision (S1E8M23)         */
typedef double             NvF64 OPTIONAL_ALIGN8_ATTR; /* IEEE Double Precision (S1E11M52)        */
#endif


// 8-bit: 'char' is the only 8-bit in the C89 standard and after.
#if NVTYPES_USE_STDINT
typedef uint8_t            NvV8; /* "void": enumerated or multiple fields    */
typedef uint8_t            NvU8; /* 0 to 255                                 */
typedef  int8_t            NvS8; /* -128 to 127                              */
#else
#ifdef NV_MISRA_COMPLIANCE_REQUIRED
typedef UInt8              NvV8; /* "void": enumerated or multiple fields    */
typedef UInt8              NvU8; /* 0 to 255                                 */
typedef  Int8              NvS8; /* -128 to 127                              */
#else
typedef unsigned char      NvV8; /* "void": enumerated or multiple fields    */
typedef unsigned char      NvU8; /* 0 to 255                                 */
typedef   signed char      NvS8; /* -128 to 127                              */
#endif
#endif // NVTYPES_USE_STDINT


#if NVTYPES_USE_STDINT
typedef uint16_t           NvV16; /* "void": enumerated or multiple fields   */
typedef uint16_t           NvU16; /* 0 to 65535                              */
typedef  int16_t           NvS16; /* -32768 to 32767                         */
#else
// 16-bit: If the compiler tells us what we can use, then use it.
#ifdef __INT16_TYPE__
typedef unsigned __INT16_TYPE__ NvV16; /* "void": enumerated or multiple fields */
typedef unsigned __INT16_TYPE__ NvU16; /* 0 to 65535                            */
typedef   signed __INT16_TYPE__ NvS16; /* -32768 to 32767                       */

// The minimal standard for C89 and after
#else       // __INT16_TYPE__
#ifdef NV_MISRA_COMPLIANCE_REQUIRED
typedef UInt16             NvV16; /* "void": enumerated or multiple fields   */
typedef UInt16             NvU16; /* 0 to 65535                              */
typedef  Int16             NvS16; /* -32768 to 32767                         */
#else
typedef unsigned short     NvV16; /* "void": enumerated or multiple fields   */
typedef unsigned short     NvU16; /* 0 to 65535                              */
typedef   signed short     NvS16; /* -32768 to 32767                         */
#endif
#endif      // __INT16_TYPE__
#endif // NVTYPES_USE_STDINT

// wchar type (fixed size types consistent across Linux/Windows boundaries)
#if defined(NV_HAS_WCHAR_T_TYPEDEF)
 typedef wchar_t NvWchar;
#else
 typedef NvV16   NvWchar;
#endif

// Macro to build an NvU32 from four bytes, listed from msb to lsb
#define NvU32_BUILD(a, b, c, d) (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))

#if NVTYPES_USE_STDINT
typedef uint32_t           NvV32; /* "void": enumerated or multiple fields   */
typedef uint32_t           NvU32; /* 0 to 4294967295                         */
typedef  int32_t           NvS32; /* -2147483648 to 2147483647               */
#else
// 32-bit: If the compiler tells us what we can use, then use it.
#ifdef __INT32_TYPE__
typedef unsigned __INT32_TYPE__ NvV32; /* "void": enumerated or multiple fields */
typedef unsigned __INT32_TYPE__ NvU32; /* 0 to 4294967295                       */
typedef   signed __INT32_TYPE__ NvS32; /* -2147483648 to 2147483647             */

// Older compilers
#else       // __INT32_TYPE__

// For historical reasons, NvU32/NvV32 are defined to different base intrinsic
// types than NvS32 on some platforms.
// Mainly for 64-bit linux, where long is 64 bits and win9x, where int is 16 bit.
#if (defined(NV_UNIX) || defined(vxworks) || defined(NV_WINDOWS_CE) ||  \
     defined(__arm) || defined(__IAR_SYSTEMS_ICC__) || defined(NV_QNX) || \
     defined(NV_INTEGRITY) || defined(NV_MODS) || \
     defined(__GNUC__) || defined(__clang__) || defined(NV_MACINTOSH_64)) && \
    (!defined(NV_MACINTOSH) || defined(NV_MACINTOSH_64))
#ifdef NV_MISRA_COMPLIANCE_REQUIRED
typedef UInt32             NvV32; /* "void": enumerated or multiple fields   */
typedef UInt32             NvU32; /* 0 to 4294967295                         */
#else
typedef unsigned int       NvV32; /* "void": enumerated or multiple fields   */
typedef unsigned int       NvU32; /* 0 to 4294967295                         */
#endif

// The minimal standard for C89 and after
#else       // (defined(NV_UNIX) || defined(vxworks) || ...
typedef unsigned long      NvV32; /* "void": enumerated or multiple fields   */
typedef unsigned long      NvU32; /* 0 to 4294967295                         */
#endif      // (defined(NV_UNIX) || defined(vxworks) || ...

// Mac OS 32-bit still needs this
#if defined(NV_MACINTOSH) && !defined(NV_MACINTOSH_64)
typedef   signed long      NvS32; /* -2147483648 to 2147483647               */
#else
#ifdef NV_MISRA_COMPLIANCE_REQUIRED
typedef   Int32            NvS32; /* -2147483648 to 2147483647               */
#else
typedef   signed int       NvS32; /* -2147483648 to 2147483647               */
#endif
#endif      // defined(NV_MACINTOSH) && !defined(NV_MACINTOSH_64)
#endif      // __INT32_TYPE__
#endif // NVTYPES_USE_STDINT



#if NVTYPES_USE_STDINT
typedef uint64_t           NvU64 OPTIONAL_ALIGN8_ATTR; /* 0 to 18446744073709551615                      */
typedef  int64_t           NvS64 OPTIONAL_ALIGN8_ATTR; /* -9223372036854775808 to 9223372036854775807    */

#define NvU64_fmtX PRIX64
#define NvU64_fmtx PRIx64
#define NvU64_fmtu PRIu64
#define NvU64_fmto PRIo64
#define NvS64_fmtd PRId64
#define NvS64_fmti PRIi64
#else
// 64-bit types for compilers that support them, plus some obsolete variants
#if defined(__GNUC__) || defined(__clang__) || defined(__arm) || \
    defined(__IAR_SYSTEMS_ICC__) || defined(__ghs__) || defined(_WIN64) || \
    defined(__SUNPRO_C) || defined(__SUNPRO_CC) || defined (__xlC__)
#ifdef NV_MISRA_COMPLIANCE_REQUIRED
typedef UInt64             NvU64 OPTIONAL_ALIGN8_ATTR; /* 0 to 18446744073709551615                      */
typedef  Int64             NvS64 OPTIONAL_ALIGN8_ATTR; /* -9223372036854775808 to 9223372036854775807    */
#else
typedef unsigned long long NvU64 OPTIONAL_ALIGN8_ATTR; /* 0 to 18446744073709551615                      */
typedef          long long NvS64 OPTIONAL_ALIGN8_ATTR; /* -9223372036854775808 to 9223372036854775807    */
#endif

#define NvU64_fmtX "llX"
#define NvU64_fmtx "llx"
#define NvU64_fmtu "llu"
#define NvU64_fmto "llo"
#define NvS64_fmtd "lld"
#define NvS64_fmti "lli"

// Microsoft since 2003 -- https://msdn.microsoft.com/en-us/library/29dh1w7z.aspx
#else
typedef unsigned __int64   NvU64 OPTIONAL_ALIGN8_ATTR; /* 0 to 18446744073709551615                      */
typedef          __int64   NvS64 OPTIONAL_ALIGN8_ATTR; /* -9223372036854775808 to 9223372036854775807    */

#define NvU64_fmtX "I64X"
#define NvU64_fmtx "I64x"
#define NvU64_fmtu "I64u"
#define NvU64_fmto "I64o"
#define NvS64_fmtd "I64d"
#define NvS64_fmti "I64i"

#endif
#endif // NVTYPES_USE_STDINT

#ifdef NV_TYPESAFE_HANDLES
/*
 * Can't use opaque pointer as clients might be compiled with mismatched
 * pointer sizes. TYPESAFE check will eventually be removed once all clients
 * have transistioned safely to NvHandle.
 * The plan is to then eventually scale up the handle to be 64-bits.
 */
typedef struct
{
    NvU32 val;
} NvHandle;
#else
/*
 * For compatibility with modules that haven't moved typesafe handles.
 */
typedef NvU32 NvHandle;
#endif // NV_TYPESAFE_HANDLES

/* Boolean type */
typedef NvU8 NvBool;
#define NV_TRUE           ((NvBool)(0 == 0))
#define NV_FALSE          ((NvBool)(0 != 0))

/* Tristate type: NV_TRISTATE_FALSE, NV_TRISTATE_TRUE, NV_TRISTATE_INDETERMINATE */
typedef NvU8 NvTristate;
#define NV_TRISTATE_FALSE           ((NvTristate) 0)
#define NV_TRISTATE_TRUE            ((NvTristate) 1)
#define NV_TRISTATE_INDETERMINATE   ((NvTristate) 2)

/* Macros to extract the low and high parts of a 64-bit unsigned integer */
/* Also designed to work if someone happens to pass in a 32-bit integer */
#ifdef NV_MISRA_COMPLIANCE_REQUIRED
#define NvU64_HI32(n)     ((NvU32)((((NvU64)(n)) >> 32) & 0xffffffffU))
#define NvU64_LO32(n)     ((NvU32)(( (NvU64)(n))        & 0xffffffffU))
#else
#define NvU64_HI32(n)     ((NvU32)((((NvU64)(n)) >> 32) & 0xffffffff))
#define NvU64_LO32(n)     ((NvU32)(( (NvU64)(n))        & 0xffffffff))
#endif
#define NvU40_HI32(n)     ((NvU32)((((NvU64)(n)) >>  8) & 0xffffffffU))
#define NvU40_HI24of32(n) ((NvU32)(  (NvU64)(n)         & 0xffffff00U))

/* Macros to get the MSB and LSB of a 32 bit unsigned number */
#define NvU32_HI16(n)     ((NvU16)((((NvU32)(n)) >> 16) & 0xffffU))
#define NvU32_LO16(n)     ((NvU16)(( (NvU32)(n))        & 0xffffU))

 /***************************************************************************\
|*                                                                           *|
|*  64 bit type definitions for use in interface structures.                 *|
|*                                                                           *|
 \***************************************************************************/

#if defined(NV_64_BITS)

typedef void*              NvP64; /* 64 bit void pointer                     */
typedef NvU64             NvUPtr; /* pointer sized unsigned int              */
typedef NvS64             NvSPtr; /* pointer sized signed int                */
typedef NvU64           NvLength; /* length to agree with sizeof             */

#define NvP64_VALUE(n)        (n)
#define NvP64_fmt "%p"

#define KERNEL_POINTER_FROM_NvP64(p,v) ((p)(v))
#define NvP64_PLUS_OFFSET(p,o) (NvP64)((NvU64)(p) + (NvU64)(o))

#define NvUPtr_fmtX NvU64_fmtX
#define NvUPtr_fmtx NvU64_fmtx
#define NvUPtr_fmtu NvU64_fmtu
#define NvUPtr_fmto NvU64_fmto
#define NvSPtr_fmtd NvS64_fmtd
#define NvSPtr_fmti NvS64_fmti

#else

typedef NvU64              NvP64; /* 64 bit void pointer                     */
typedef NvU32             NvUPtr; /* pointer sized unsigned int              */
typedef NvS32             NvSPtr; /* pointer sized signed int                */
typedef NvU32           NvLength; /* length to agree with sizeof             */

#define NvP64_VALUE(n)        ((void *)(NvUPtr)(n))
#define NvP64_fmt "0x%llx"

#define KERNEL_POINTER_FROM_NvP64(p,v) ((p)(NvUPtr)(v))
#define NvP64_PLUS_OFFSET(p,o) ((p) + (NvU64)(o))

#define NvUPtr_fmtX "X"
#define NvUPtr_fmtx "x"
#define NvUPtr_fmtu "u"
#define NvUPtr_fmto "o"
#define NvSPtr_fmtd "d"
#define NvSPtr_fmti "i"

#endif

#define NvP64_NULL       (NvP64)0

/*!
 * Helper macro to pack an @ref NvU64_ALIGN32 structure from a @ref NvU64.
 *
 * @param[out] pDst   Pointer to NvU64_ALIGN32 structure to pack
 * @param[in]  pSrc   Pointer to NvU64 with which to pack
 */
#define NvU64_ALIGN32_PACK(pDst, pSrc)                                         \
do {                                                                           \
    (pDst)->lo = NvU64_LO32(*(pSrc));                                          \
    (pDst)->hi = NvU64_HI32(*(pSrc));                                          \
} while (NV_FALSE)

/*!
 * Helper macro to unpack a @ref NvU64_ALIGN32 structure into a @ref NvU64.
 *
 * @param[out] pDst   Pointer to NvU64 in which to unpack
 * @param[in]  pSrc   Pointer to NvU64_ALIGN32 structure from which to unpack
 */
#define NvU64_ALIGN32_UNPACK(pDst, pSrc)                                       \
do {                                                                           \
    (*(pDst)) = NvU64_ALIGN32_VAL(pSrc);                                       \
} while (NV_FALSE)

/*!
 * Helper macro to unpack a @ref NvU64_ALIGN32 structure as a @ref NvU64.
 *
 * @param[in]  pSrc   Pointer to NvU64_ALIGN32 structure to unpack
 */
#define NvU64_ALIGN32_VAL(pSrc)                                                \
    ((NvU64) ((NvU64)((pSrc)->lo) | (((NvU64)(pSrc)->hi) << 32U)))

/*!
 * Helper macro to check whether the 32 bit aligned 64 bit number is zero.
 *
 * @param[in]  _pU64   Pointer to NvU64_ALIGN32 structure.
 *
 * @return
 *  NV_TRUE     _pU64 is zero.
 *  NV_FALSE    otherwise.
 */
#define NvU64_ALIGN32_IS_ZERO(_pU64)                                          \
    (((_pU64)->lo == 0U) && ((_pU64)->hi == 0U))

/*!
 * Helper macro to sub two 32 aligned 64 bit numbers on 64 bit processor.
 *
 * @param[in]       pSrc1   Pointer to NvU64_ALIGN32 source 1 structure.
 * @param[in]       pSrc2   Pointer to NvU64_ALIGN32 source 2 structure.
 * @param[in/out]   pDst    Pointer to NvU64_ALIGN32 dest. structure.
 */
#define NvU64_ALIGN32_ADD(pDst, pSrc1, pSrc2)                                 \
do {                                                                          \
    NvU64 __dst, __src1, __scr2;                                              \
                                                                              \
    NvU64_ALIGN32_UNPACK(&__src1, (pSrc1));                                   \
    NvU64_ALIGN32_UNPACK(&__scr2, (pSrc2));                                   \
    __dst = __src1 + __scr2;                                                  \
    NvU64_ALIGN32_PACK((pDst), &__dst);                                       \
} while (NV_FALSE)

/*!
 * Helper macro to sub two 32 aligned 64 bit numbers on 64 bit processor.
 *
 * @param[in]       pSrc1   Pointer to NvU64_ALIGN32 source 1 structure.
 * @param[in]       pSrc2   Pointer to NvU64_ALIGN32 source 2 structure.
 * @param[in/out]   pDst    Pointer to NvU64_ALIGN32 dest. structure.
 */
#define NvU64_ALIGN32_SUB(pDst, pSrc1, pSrc2)                                  \
do {                                                                           \
    NvU64 __dst, __src1, __scr2;                                               \
                                                                               \
    NvU64_ALIGN32_UNPACK(&__src1, (pSrc1));                                    \
    NvU64_ALIGN32_UNPACK(&__scr2, (pSrc2));                                    \
    __dst = __src1 - __scr2;                                                   \
    NvU64_ALIGN32_PACK((pDst), &__dst);                                        \
} while (NV_FALSE)

/*!
 * Structure for representing 32 bit aligned NvU64 (64-bit unsigned integer)
 * structures. This structure must be used because the 32 bit processor and
 * 64 bit processor compilers will pack/align NvU64 differently.
 *
 * One use case is RM being 64 bit proc whereas PMU being 32 bit proc, this
 * alignment difference will result in corrupted transactions between the RM
 * and PMU.
 *
 * See the @ref NvU64_ALIGN32_PACK and @ref NvU64_ALIGN32_UNPACK macros for
 * packing and unpacking these structures.
 *
 * @note The intention of this structure is to provide a datatype which will
 *       packed/aligned consistently and efficiently across all platforms.
 *       We don't want to use "NV_DECLARE_ALIGNED(NvU64, 8)" because that
 *       leads to memory waste on our 32-bit uprocessors (e.g. FALCONs) where
 *       DMEM efficiency is vital.
 */
typedef struct
{
    /*!
     * Low 32 bits.
     */
    NvU32 lo;
    /*!
     * High 32 bits.
     */
    NvU32 hi;
} NvU64_ALIGN32;

/* Useful macro to hide required double cast */
#define NV_PTR_TO_NvP64(n) (NvP64)(NvUPtr)(n)
#define NV_SIGN_EXT_PTR_TO_NvP64(p) ((NvP64)(NvS64)(NvSPtr)(p))
#define KERNEL_POINTER_TO_NvP64(p) ((NvP64)(uintptr_t)(p))

 /***************************************************************************\
|*                                                                           *|
|*  Limits for common types.                                                 *|
|*                                                                           *|
 \***************************************************************************/

/* Explanation of the current form of these limits:
 *
 * - Decimal is used, as hex values are by default positive.
 * - Casts are not used, as usage in the preprocessor itself (#if) ends poorly.
 * - The subtraction of 1 for some MIN values is used to get around the fact
 *   that the C syntax actually treats -x as NEGATE(x) instead of a distinct
 *   number.  Since 214748648 isn't a valid positive 32-bit signed value, we
 *   take the largest valid positive signed number, negate it, and subtract 1.
 */
#define NV_S8_MIN       (-128)
#define NV_S8_MAX       (+127)
#define NV_U8_MIN       (0U)
#define NV_U8_MAX       (+255U)
#define NV_S16_MIN      (-32768)
#define NV_S16_MAX      (+32767)
#define NV_U16_MIN      (0U)
#define NV_U16_MAX      (+65535U)
#define NV_S32_MIN      (-2147483647 - 1)
#define NV_S32_MAX      (+2147483647)
#define NV_U32_MIN      (0U)
#define NV_U32_MAX      (+4294967295U)
#define NV_S64_MIN      (-9223372036854775807LL - 1LL)
#define NV_S64_MAX      (+9223372036854775807LL)
#define NV_U64_MIN      (0ULL)
#define NV_U64_MAX      (+18446744073709551615ULL)

/* Aligns fields in structs  so they match up between 32 and 64 bit builds */
#if defined(__GNUC__) || defined(__clang__) || defined(NV_QNX)
#define NV_ALIGN_BYTES(size) __attribute__ ((aligned (size)))
#elif defined(__arm)
#define NV_ALIGN_BYTES(size) __align(ALIGN)
#else
// XXX This is dangerously nonportable!  We really shouldn't provide a default
// version of this that doesn't do anything.
#define NV_ALIGN_BYTES(size)
#endif

// NV_DECLARE_ALIGNED() can be used on all platforms.
// This macro form accounts for the fact that __declspec on Windows is required
// before the variable type,
// and NV_ALIGN_BYTES is required after the variable name.
#if defined(__GNUC__) || defined(__clang__) || defined(NV_QNX)
#define NV_DECLARE_ALIGNED(TYPE_VAR, ALIGN) TYPE_VAR __attribute__ ((aligned (ALIGN)))
#elif defined(__arm)
#define NV_DECLARE_ALIGNED(TYPE_VAR, ALIGN) __align(ALIGN) TYPE_VAR
#endif

 /***************************************************************************\
|*                       Function Declaration Types                          *|
 \***************************************************************************/

// stretching the meaning of "nvtypes", but this seems to least offensive
// place to re-locate these from nvos.h which cannot be included by a number
// of builds that need them

    #if defined(__GNUC__)
        #if (__GNUC__ > 3) || \
            ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1) && (__GNUC_PATCHLEVEL__ >= 1))
            #define NV_NOINLINE __attribute__((__noinline__))
        #endif
    #elif defined(__clang__)
        #if __has_attribute(noinline)
        #define NV_NOINLINE __attribute__((__noinline__))
        #endif
    #elif defined(__arm) && (__ARMCC_VERSION >= 300000)
        #define NV_NOINLINE __attribute__((__noinline__))
    #elif (defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590)) ||\
            (defined(__SUNPRO_CC) && (__SUNPRO_CC >= 0x590))
        #define NV_NOINLINE __attribute__((__noinline__))
    #elif defined (__INTEL_COMPILER)
        #define NV_NOINLINE __attribute__((__noinline__))
    #endif

    #if !defined(NV_NOINLINE)
    #define NV_NOINLINE
    #endif

    /* GreenHills compiler defines __GNUC__, but doesn't support
     * __inline__ keyword. */
    #if defined(__ghs__)
    #define NV_INLINE inline
    #elif defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
    #define NV_INLINE __inline__
    #elif defined (macintosh) || defined(__SUNPRO_C) || defined(__SUNPRO_CC)
    #define NV_INLINE inline
    #elif defined(__arm)
    #define NV_INLINE __inline
    #else
    #define NV_INLINE
    #endif

    /* Don't force inline on DEBUG builds -- it's annoying for debuggers. */
    #if !defined(DEBUG)
        /* GreenHills compiler defines __GNUC__, but doesn't support
         * __attribute__ or __inline__ keyword. */
        #if defined(__ghs__)
            #define NV_FORCEINLINE inline
        #elif defined(__GNUC__)
            // GCC 3.1 and beyond support the always_inline function attribute.
            #if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
            #define NV_FORCEINLINE __attribute__((__always_inline__)) __inline__
            #else
            #define NV_FORCEINLINE __inline__
            #endif
        #elif defined(__clang__)
            #if __has_attribute(always_inline)
            #define NV_FORCEINLINE __attribute__((__always_inline__)) __inline__
            #else
            #define NV_FORCEINLINE __inline__
            #endif
        #elif defined(__arm) && (__ARMCC_VERSION >= 220000)
            // RVDS 2.2 also supports forceinline, but ADS 1.2 does not
            #define NV_FORCEINLINE __forceinline
        #else /* defined(__GNUC__) */
            #define NV_FORCEINLINE NV_INLINE
        #endif
    #else
        #define NV_FORCEINLINE NV_INLINE
    #endif

    #define NV_APIENTRY
    #define NV_FASTCALL
    #define NV_CDECLCALL
    #define NV_STDCALL

    /*
     * The 'warn_unused_result' function attribute prompts GCC to issue a
     * warning if the result of a function tagged with this attribute
     * is ignored by a caller.  In combination with '-Werror', it can be
     * used to enforce result checking in RM code; at this point, this
     * is only done on UNIX.
     */
    #if defined(__GNUC__) && defined(NV_UNIX)
        #if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))
        #define NV_FORCERESULTCHECK __attribute__((__warn_unused_result__))
        #else
        #define NV_FORCERESULTCHECK
        #endif
    #elif defined(__clang__)
        #if __has_attribute(warn_unused_result)
        #define NV_FORCERESULTCHECK __attribute__((__warn_unused_result__))
        #else
        #define NV_FORCERESULTCHECK
        #endif
    #else /* defined(__GNUC__) */
        #define NV_FORCERESULTCHECK
    #endif

    #if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
        #define NV_ATTRIBUTE_UNUSED __attribute__((__unused__))
    #else
        #define NV_ATTRIBUTE_UNUSED
    #endif

    /*
     * Functions decorated with NV_FORMAT_PRINTF(f, a) have a format string at
     * parameter number 'f' and variadic arguments start at parameter number 'a'.
     * (Note that for C++ methods, there is an implicit 'this' parameter so
     * explicit parameters are numbered from 2.)
     */
    #if defined(__GNUC__)
        #define NV_FORMAT_PRINTF(_f, _a) __attribute__((format(printf, _f, _a)))
    #else
        #define NV_FORMAT_PRINTF(_f, _a)
    #endif

#ifdef __cplusplus
}
#endif

#endif /* NVTYPES_INCLUDED */
