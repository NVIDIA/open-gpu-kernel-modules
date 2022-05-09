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

/*
 * nvmisc.h
 */
#ifndef __NV_MISC_H
#define __NV_MISC_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include "nvtypes.h"

#if !defined(NVIDIA_UNDEF_LEGACY_BIT_MACROS)
//
// Miscellaneous macros useful for bit field manipulations
//
// STUPID HACK FOR CL 19434692.  Will revert when fix CL is delivered bfm -> chips_a.
#ifndef BIT
#define BIT(b)                  (1U<<(b))
#endif
#ifndef BIT32
#define BIT32(b)                ((NvU32)1U<<(b))
#endif
#ifndef BIT64
#define BIT64(b)                ((NvU64)1U<<(b))
#endif

#endif

//
// It is recommended to use the following bit macros to avoid macro name
// collisions with other src code bases.
//
#ifndef NVBIT
#define NVBIT(b)                  (1U<<(b))
#endif
#ifndef NVBIT_TYPE
#define NVBIT_TYPE(b, t)          (((t)1U)<<(b))
#endif
#ifndef NVBIT32
#define NVBIT32(b)                NVBIT_TYPE(b, NvU32)
#endif
#ifndef NVBIT64
#define NVBIT64(b)                NVBIT_TYPE(b, NvU64)
#endif

// Helper macro's for 32 bit bitmasks
#define NV_BITMASK32_ELEMENT_SIZE            (sizeof(NvU32) << 3)
#define NV_BITMASK32_IDX(chId)               (((chId) & ~(0x1F)) >> 5)  
#define NV_BITMASK32_OFFSET(chId)            ((chId) & (0x1F))
#define NV_BITMASK32_SET(pChannelMask, chId) \
        (pChannelMask)[NV_BITMASK32_IDX(chId)] |= NVBIT(NV_BITMASK32_OFFSET(chId))
#define NV_BITMASK32_GET(pChannelMask, chId) \
        ((pChannelMask)[NV_BITMASK32_IDX(chId)] & NVBIT(NV_BITMASK32_OFFSET(chId)))


// Index of the 'on' bit (assuming that there is only one).
// Even if multiple bits are 'on', result is in range of 0-31.
#define BIT_IDX_32(n)                            \
   (((((n) & 0xFFFF0000U) != 0U) ? 0x10U: 0U) |  \
    ((((n) & 0xFF00FF00U) != 0U) ? 0x08U: 0U) |  \
    ((((n) & 0xF0F0F0F0U) != 0U) ? 0x04U: 0U) |  \
    ((((n) & 0xCCCCCCCCU) != 0U) ? 0x02U: 0U) |  \
    ((((n) & 0xAAAAAAAAU) != 0U) ? 0x01U: 0U) )

// Index of the 'on' bit (assuming that there is only one).
// Even if multiple bits are 'on', result is in range of 0-63.
#define BIT_IDX_64(n)                                       \
   (((((n) & 0xFFFFFFFF00000000ULL) != 0U) ? 0x20U: 0U) |   \
    ((((n) & 0xFFFF0000FFFF0000ULL) != 0U) ? 0x10U: 0U) |   \
    ((((n) & 0xFF00FF00FF00FF00ULL) != 0U) ? 0x08U: 0U) |   \
    ((((n) & 0xF0F0F0F0F0F0F0F0ULL) != 0U) ? 0x04U: 0U) |   \
    ((((n) & 0xCCCCCCCCCCCCCCCCULL) != 0U) ? 0x02U: 0U) |   \
    ((((n) & 0xAAAAAAAAAAAAAAAAULL) != 0U) ? 0x01U: 0U) )

/*!
 * DRF MACRO README:
 *
 * Glossary:
 *      DRF: Device, Register, Field
 *      FLD: Field
 *      REF: Reference
 *
 * #define NV_DEVICE_OMEGA_REGISTER_ALPHA                   0xDEADBEEF
 * #define NV_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_GAMMA             27:0
 * #define NV_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA             31:28
 * #define NV_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA_ZERO   0x00000000
 * #define NV_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA_ONE    0x00000001
 * #define NV_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA_TWO    0x00000002
 * #define NV_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA_THREE  0x00000003
 * #define NV_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA_FOUR   0x00000004
 * #define NV_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA_FIVE   0x00000005
 * #define NV_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA_SIX    0x00000006
 * #define NV_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA_SEVEN  0x00000007
 *
 *
 * Device = _DEVICE_OMEGA
 *   This is the common "base" that a group of registers in a manual share
 *
 * Register = _REGISTER_ALPHA
 *   Register for a given block of defines is the common root for one or more fields and constants
 *
 * Field(s) = _FIELD_GAMMA, _FIELD_ZETA
 *   These are the bit ranges for a given field within the register
 *   Fields are not required to have defined constant values (enumerations)
 *
 * Constant(s) = _ZERO, _ONE, _TWO, ...
 *   These are named values (enums) a field can contain; the width of the constants should not be larger than the field width
 *
 * MACROS:
 *
 * DRF_SHIFT:
 *      Bit index of the lower bound of a field
 *      DRF_SHIFT(NV_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA) == 28
 *
 * DRF_SHIFT_RT:
 *      Bit index of the higher bound of a field
 *      DRF_SHIFT_RT(NV_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA) == 31
 *
 * DRF_MASK:
 *      Produces a mask of 1-s equal to the width of a field
 *      DRF_MASK(NV_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA) == 0xF (four 1s starting at bit 0)
 *
 * DRF_SHIFTMASK:
 *      Produces a mask of 1s equal to the width of a field at the location of the field
 *      DRF_SHIFTMASK(NV_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA) == 0xF0000000
 *
 * DRF_DEF:
 *      Shifts a field constant's value to the correct field offset
 *      DRF_DEF(_DEVICE_OMEGA, _REGISTER_ALPHA, _FIELD_ZETA, _THREE) == 0x30000000
 *
 * DRF_NUM:
 *      Shifts a number to the location of a particular field
 *      DRF_NUM(_DEVICE_OMEGA, _REGISTER_ALPHA, _FIELD_ZETA, 3) == 0x30000000
 *      NOTE: If the value passed in is wider than the field, the value's high bits will be truncated
 *
 * DRF_SIZE:
 *      Provides the width of the field in bits
 *      DRF_SIZE(NV_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA) == 4
 *
 * DRF_VAL:
 *      Provides the value of an input within the field specified
 *      DRF_VAL(_DEVICE_OMEGA, _REGISTER_ALPHA, _FIELD_ZETA, 0xABCD1234) == 0xA
 *      This is sort of like the inverse of DRF_NUM
 *
 * DRF_IDX...:
 *      These macros are similar to the above but for fields that accept an index argumment
 *
 * FLD_SET_DRF:
 *      Set the field bits in a given value with the given field constant
 *      NvU32 x = 0x00001234;
 *      x = FLD_SET_DRF(_DEVICE_OMEGA, _REGISTER_ALPHA, _FIELD_ZETA, _THREE, x);
 *      x == 0x30001234;
 *
 * FLD_SET_DRF_NUM:
 *      Same as FLD_SET_DRF but instead of using a field constant a literal/variable is passed in
 *      NvU32 x = 0x00001234;
 *      x = FLD_SET_DRF_NUM(_DEVICE_OMEGA, _REGISTER_ALPHA, _FIELD_ZETA, 0xF, x);
 *      x == 0xF0001234;
 *
 * FLD_IDX...:
 *      These macros are similar to the above but for fields that accept an index argumment
 *
 * FLD_TEST_DRF:
 *      Test if location specified by drf in 'v' has the same value as NV_drfc
 *      FLD_TEST_DRF(_DEVICE_OMEGA, _REGISTER_ALPHA, _FIELD_ZETA, _THREE, 0x3000ABCD) == NV_TRUE
 *
 * FLD_TEST_DRF_NUM:
 *      Test if locations specified by drf in 'v' have the same value as n
 *      FLD_TEST_DRF_NUM(_DEVICE_OMEGA, _REGISTER_ALPHA, _FIELD_ZETA, 0x3, 0x3000ABCD) == NV_TRUE
 *
 * REF_DEF:
 *      Like DRF_DEF but maintains full symbol name (use in cases where "NV" is not prefixed to the field)
 *      REF_DEF(SOME_OTHER_PREFIX_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA, _THREE) == 0x30000000
 *
 * REF_VAL:
 *      Like DRF_VAL but maintains full symbol name (use in cases where "NV" is not prefixed to the field)
 *      REF_VAL(SOME_OTHER_PREFIX_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA, 0xABCD1234) == 0xA
 *
 * REF_NUM:
 *      Like DRF_NUM but maintains full symbol name (use in cases where "NV" is not prefixed to the field)
 *      REF_NUM(SOME_OTHER_PREFIX_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA, 0xA) == 0xA00000000
 *
 * FLD_SET_REF_NUM:
 *      Like FLD_SET_DRF_NUM but maintains full symbol name (use in cases where "NV" is not prefixed to the field)
 *      NvU32 x = 0x00001234;
 *      x = FLD_SET_REF_NUM(SOME_OTHER_PREFIX_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA, 0xF, x);
 *      x == 0xF0001234;
 *
 * FLD_TEST_REF:
 *      Like FLD_TEST_DRF but maintains full symbol name (use in cases where "NV" is not prefixed to the field)
 *      FLD_TEST_REF(SOME_OTHER_PREFIX_DEVICE_OMEGA_REGISTER_ALPHA_FIELD_ZETA, _THREE, 0x3000ABCD) == NV_TRUE
 *
 * Other macros:
 *      There a plethora of other macros below that extend the above (notably Multi-Word (MW), 64-bit, and some
 *      reg read/write variations). I hope these are self explanatory. If you have a need to use them, you
 *      probably have some knowledge of how they work.
 */

// tegra mobile uses nvmisc_macros.h and can't access nvmisc.h... and sometimes both get included.
#ifndef _NVMISC_MACROS_H
// Use Coverity Annotation to mark issues as false positives/ignore when using single bit defines.
#define DRF_ISBIT(bitval,drf)                \
        ( /* coverity[identical_branches] */ \
          (bitval != 0) ? drf )
#define DEVICE_BASE(d)          (0?d)  // what's up with this name? totally non-parallel to the macros below
#define DEVICE_EXTENT(d)        (1?d)  // what's up with this name? totally non-parallel to the macros below
#ifdef NV_MISRA_COMPLIANCE_REQUIRED
#ifdef MISRA_14_3
#define DRF_BASE(drf)           (drf##_LOW_FIELD)
#define DRF_EXTENT(drf)         (drf##_HIGH_FIELD)
#define DRF_SHIFT(drf)          ((drf##_LOW_FIELD) % 32U)
#define DRF_SHIFT_RT(drf)       ((drf##_HIGH_FIELD) % 32U)
#define DRF_MASK(drf)           (0xFFFFFFFFU >> (31U - ((drf##_HIGH_FIELD) % 32U) + ((drf##_LOW_FIELD) % 32U)))
#else
#define DRF_BASE(drf)           (NV_FALSE?drf)  // much better
#define DRF_EXTENT(drf)         (NV_TRUE?drf)  // much better
#define DRF_SHIFT(drf)          (((NvU32)DRF_BASE(drf)) % 32U)
#define DRF_SHIFT_RT(drf)       (((NvU32)DRF_EXTENT(drf)) % 32U)
#define DRF_MASK(drf)           (0xFFFFFFFFU>>(31U - DRF_SHIFT_RT(drf) + DRF_SHIFT(drf)))
#endif
#define DRF_DEF(d,r,f,c)        (((NvU32)(NV ## d ## r ## f ## c))<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_NUM(d,r,f,n)        ((((NvU32)(n))&DRF_MASK(NV ## d ## r ## f))<<DRF_SHIFT(NV ## d ## r ## f))
#else
#define DRF_BASE(drf)           (0?drf)  // much better
#define DRF_EXTENT(drf)         (1?drf)  // much better
#define DRF_SHIFT(drf)          ((DRF_ISBIT(0,drf)) % 32)
#define DRF_SHIFT_RT(drf)       ((DRF_ISBIT(1,drf)) % 32)
#define DRF_MASK(drf)           (0xFFFFFFFFU>>(31-((DRF_ISBIT(1,drf)) % 32)+((DRF_ISBIT(0,drf)) % 32)))
#define DRF_DEF(d,r,f,c)        ((NV ## d ## r ## f ## c)<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_NUM(d,r,f,n)        (((n)&DRF_MASK(NV ## d ## r ## f))<<DRF_SHIFT(NV ## d ## r ## f))
#endif
#define DRF_SHIFTMASK(drf)      (DRF_MASK(drf)<<(DRF_SHIFT(drf)))
#define DRF_SIZE(drf)           (DRF_EXTENT(drf)-DRF_BASE(drf)+1U)

#define DRF_VAL(d,r,f,v)        (((v)>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))
#endif

// Signed version of DRF_VAL, which takes care of extending sign bit.
#define DRF_VAL_SIGNED(d,r,f,v)         (((DRF_VAL(d,r,f,(v)) ^ (NVBIT(DRF_SIZE(NV ## d ## r ## f)-1U)))) - (NVBIT(DRF_SIZE(NV ## d ## r ## f)-1U)))
#define DRF_IDX_DEF(d,r,f,i,c)          ((NV ## d ## r ## f ## c)<<DRF_SHIFT(NV##d##r##f(i)))
#define DRF_IDX_OFFSET_DEF(d,r,f,i,o,c) ((NV ## d ## r ## f ## c)<<DRF_SHIFT(NV##d##r##f(i,o)))
#define DRF_IDX_NUM(d,r,f,i,n)          (((n)&DRF_MASK(NV##d##r##f(i)))<<DRF_SHIFT(NV##d##r##f(i)))
#define DRF_IDX_VAL(d,r,f,i,v)          (((v)>>DRF_SHIFT(NV##d##r##f(i)))&DRF_MASK(NV##d##r##f(i)))
#define DRF_IDX_OFFSET_VAL(d,r,f,i,o,v) (((v)>>DRF_SHIFT(NV##d##r##f(i,o)))&DRF_MASK(NV##d##r##f(i,o)))
// Fractional version of DRF_VAL which reads Fx.y fixed point number (x.y)*z
#define DRF_VAL_FRAC(d,r,x,y,v,z)       ((DRF_VAL(d,r,x,(v))*z) + ((DRF_VAL(d,r,y,v)*z) / (1<<DRF_SIZE(NV##d##r##y))))

//
// 64 Bit Versions
//
#define DRF_SHIFT64(drf)                ((DRF_ISBIT(0,drf)) % 64)
#define DRF_MASK64(drf)                 (NV_U64_MAX>>(63-((DRF_ISBIT(1,drf)) % 64)+((DRF_ISBIT(0,drf)) % 64)))
#define DRF_SHIFTMASK64(drf)            (DRF_MASK64(drf)<<(DRF_SHIFT64(drf)))

#define DRF_DEF64(d,r,f,c)              (((NvU64)(NV ## d ## r ## f ## c))<<DRF_SHIFT64(NV ## d ## r ## f))
#define DRF_NUM64(d,r,f,n)              ((((NvU64)(n))&DRF_MASK64(NV ## d ## r ## f))<<DRF_SHIFT64(NV ## d ## r ## f))
#define DRF_VAL64(d,r,f,v)              ((((NvU64)(v))>>DRF_SHIFT64(NV ## d ## r ## f))&DRF_MASK64(NV ## d ## r ## f))

#define DRF_VAL_SIGNED64(d,r,f,v)       (((DRF_VAL64(d,r,f,(v)) ^ (NVBIT64(DRF_SIZE(NV ## d ## r ## f)-1)))) - (NVBIT64(DRF_SIZE(NV ## d ## r ## f)-1)))
#define DRF_IDX_DEF64(d,r,f,i,c)        (((NvU64)(NV ## d ## r ## f ## c))<<DRF_SHIFT64(NV##d##r##f(i)))
#define DRF_IDX_OFFSET_DEF64(d,r,f,i,o,c) ((NvU64)(NV ## d ## r ## f ## c)<<DRF_SHIFT64(NV##d##r##f(i,o)))
#define DRF_IDX_NUM64(d,r,f,i,n)        ((((NvU64)(n))&DRF_MASK64(NV##d##r##f(i)))<<DRF_SHIFT64(NV##d##r##f(i)))
#define DRF_IDX_VAL64(d,r,f,i,v)        ((((NvU64)(v))>>DRF_SHIFT64(NV##d##r##f(i)))&DRF_MASK64(NV##d##r##f(i)))
#define DRF_IDX_OFFSET_VAL64(d,r,f,i,o,v) (((NvU64)(v)>>DRF_SHIFT64(NV##d##r##f(i,o)))&DRF_MASK64(NV##d##r##f(i,o)))

#define FLD_SET_DRF64(d,r,f,c,v)        (((NvU64)(v) & ~DRF_SHIFTMASK64(NV##d##r##f)) | DRF_DEF64(d,r,f,c))
#define FLD_SET_DRF_NUM64(d,r,f,n,v)    ((((NvU64)(v)) & ~DRF_SHIFTMASK64(NV##d##r##f)) | DRF_NUM64(d,r,f,n))
#define FLD_IDX_SET_DRF64(d,r,f,i,c,v)  (((NvU64)(v) & ~DRF_SHIFTMASK64(NV##d##r##f(i))) | DRF_IDX_DEF64(d,r,f,i,c))
#define FLD_IDX_OFFSET_SET_DRF64(d,r,f,i,o,c,v) (((NvU64)(v) & ~DRF_SHIFTMASK64(NV##d##r##f(i,o))) | DRF_IDX_OFFSET_DEF64(d,r,f,i,o,c))
#define FLD_IDX_SET_DRF_DEF64(d,r,f,i,c,v) (((NvU64)(v) & ~DRF_SHIFTMASK64(NV##d##r##f(i))) | DRF_IDX_DEF64(d,r,f,i,c))
#define FLD_IDX_SET_DRF_NUM64(d,r,f,i,n,v) (((NvU64)(v) & ~DRF_SHIFTMASK64(NV##d##r##f(i))) | DRF_IDX_NUM64(d,r,f,i,n))
#define FLD_SET_DRF_IDX64(d,r,f,c,i,v)  (((NvU64)(v) & ~DRF_SHIFTMASK64(NV##d##r##f)) | DRF_DEF64(d,r,f,c(i)))

#define FLD_TEST_DRF64(d,r,f,c,v)       (DRF_VAL64(d, r, f, (v)) == NV##d##r##f##c)
#define FLD_TEST_DRF_AND64(d,r,f,c,v)   (DRF_VAL64(d, r, f, (v)) & NV##d##r##f##c)
#define FLD_TEST_DRF_NUM64(d,r,f,n,v)   (DRF_VAL64(d, r, f, (v)) == (n))
#define FLD_IDX_TEST_DRF64(d,r,f,i,c,v) (DRF_IDX_VAL64(d, r, f, i, (v)) == NV##d##r##f##c)
#define FLD_IDX_OFFSET_TEST_DRF64(d,r,f,i,o,c,v) (DRF_IDX_OFFSET_VAL64(d, r, f, i, o, (v)) == NV##d##r##f##c)

#define REF_DEF64(drf,d)            (((drf ## d)&DRF_MASK64(drf))<<DRF_SHIFT64(drf))
#define REF_VAL64(drf,v)            (((NvU64)(v)>>DRF_SHIFT64(drf))&DRF_MASK64(drf))
#if defined(NV_MISRA_COMPLIANCE_REQUIRED) && defined(MISRA_14_3)
#define REF_NUM64(drf,n)            (((NvU64)(n)&(0xFFFFFFFFFFFFFFFFU>>(63U-((drf##_HIGH_FIELD) % 63U)+((drf##_LOW_FIELD) % 63U)))) << ((drf##_LOW_FIELD) % 63U))
#else
#define REF_NUM64(drf,n)            (((NvU64)(n)&DRF_MASK64(drf))<<DRF_SHIFT64(drf))
#endif
#define FLD_TEST_REF64(drf,c,v)     (REF_VAL64(drf, v) == drf##c)
#define FLD_TEST_REF_AND64(drf,c,v) (REF_VAL64(drf, v) & drf##c)
#define FLD_SET_REF_NUM64(drf,n,v)  (((NvU64)(v) & ~DRF_SHIFTMASK64(drf)) | REF_NUM64(drf,n))

//
// 32 Bit Versions
//

#ifdef NV_MISRA_COMPLIANCE_REQUIRED
#define FLD_SET_DRF(d,r,f,c,v)                  (((NvU32)(v) & ~DRF_SHIFTMASK(NV##d##r##f)) | DRF_DEF(d,r,f,c))
#define FLD_SET_DRF_NUM(d,r,f,n,v)              (((NvU32)(v) & ~DRF_SHIFTMASK(NV##d##r##f)) | DRF_NUM(d,r,f,n))
#define FLD_IDX_SET_DRF(d,r,f,i,c,v)            (((NvU32)(v) & ~DRF_SHIFTMASK(NV##d##r##f(i))) | DRF_IDX_DEF(d,r,f,i,c))
#define FLD_IDX_OFFSET_SET_DRF(d,r,f,i,o,c,v)   (((NvU32)(v) & ~DRF_SHIFTMASK(NV##d##r##f(i,o))) | DRF_IDX_OFFSET_DEF(d,r,f,i,o,c))
#define FLD_IDX_SET_DRF_DEF(d,r,f,i,c,v)        (((NvU32)(v) & ~DRF_SHIFTMASK(NV##d##r##f(i))) | DRF_IDX_DEF(d,r,f,i,c))
#define FLD_IDX_SET_DRF_NUM(d,r,f,i,n,v)        (((NvU32)(v) & ~DRF_SHIFTMASK(NV##d##r##f(i))) | DRF_IDX_NUM(d,r,f,i,n))
#define FLD_SET_DRF_IDX(d,r,f,c,i,v)            (((NvU32)(v) & ~DRF_SHIFTMASK(NV##d##r##f)) | DRF_DEF(d,r,f,c(i)))

#define FLD_TEST_DRF(d,r,f,c,v)                 ((DRF_VAL(d, r, f, (v)) == (NvU32)(NV##d##r##f##c)))
#define FLD_TEST_DRF_AND(d,r,f,c,v)             ((DRF_VAL(d, r, f, (v)) & (NvU32)(NV##d##r##f##c)) != 0U)
#define FLD_TEST_DRF_NUM(d,r,f,n,v)             ((DRF_VAL(d, r, f, (v)) == (NvU32)(n)))
#define FLD_IDX_TEST_DRF(d,r,f,i,c,v)           ((DRF_IDX_VAL(d, r, f, i, (v)) == (NvU32)(NV##d##r##f##c)))
#define FLD_IDX_OFFSET_TEST_DRF(d,r,f,i,o,c,v)  ((DRF_IDX_OFFSET_VAL(d, r, f, i, o, (v)) == (NvU32)(NV##d##r##f##c)))
#else
#define FLD_SET_DRF(d,r,f,c,v)                  (((v) & ~DRF_SHIFTMASK(NV##d##r##f)) | DRF_DEF(d,r,f,c))
#define FLD_SET_DRF_NUM(d,r,f,n,v)              (((v) & ~DRF_SHIFTMASK(NV##d##r##f)) | DRF_NUM(d,r,f,n))
#define FLD_IDX_SET_DRF(d,r,f,i,c,v)            (((v) & ~DRF_SHIFTMASK(NV##d##r##f(i))) | DRF_IDX_DEF(d,r,f,i,c))
#define FLD_IDX_OFFSET_SET_DRF(d,r,f,i,o,c,v)   (((v) & ~DRF_SHIFTMASK(NV##d##r##f(i,o))) | DRF_IDX_OFFSET_DEF(d,r,f,i,o,c))
#define FLD_IDX_SET_DRF_DEF(d,r,f,i,c,v)        (((v) & ~DRF_SHIFTMASK(NV##d##r##f(i))) | DRF_IDX_DEF(d,r,f,i,c))
#define FLD_IDX_SET_DRF_NUM(d,r,f,i,n,v)        (((v) & ~DRF_SHIFTMASK(NV##d##r##f(i))) | DRF_IDX_NUM(d,r,f,i,n))
#define FLD_SET_DRF_IDX(d,r,f,c,i,v)            (((v) & ~DRF_SHIFTMASK(NV##d##r##f)) | DRF_DEF(d,r,f,c(i)))

#define FLD_TEST_DRF(d,r,f,c,v)                 ((DRF_VAL(d, r, f, (v)) == NV##d##r##f##c))
#define FLD_TEST_DRF_AND(d,r,f,c,v)             ((DRF_VAL(d, r, f, (v)) & NV##d##r##f##c))
#define FLD_TEST_DRF_NUM(d,r,f,n,v)             ((DRF_VAL(d, r, f, (v)) == (n)))
#define FLD_IDX_TEST_DRF(d,r,f,i,c,v)           ((DRF_IDX_VAL(d, r, f, i, (v)) == NV##d##r##f##c))
#define FLD_IDX_OFFSET_TEST_DRF(d,r,f,i,o,c,v)  ((DRF_IDX_OFFSET_VAL(d, r, f, i, o, (v)) == NV##d##r##f##c))
#endif

#define REF_DEF(drf,d)            (((drf ## d)&DRF_MASK(drf))<<DRF_SHIFT(drf))
#define REF_VAL(drf,v)            (((v)>>DRF_SHIFT(drf))&DRF_MASK(drf))
#if defined(NV_MISRA_COMPLIANCE_REQUIRED) && defined(MISRA_14_3)
#define REF_NUM(drf,n)            (((n)&(0xFFFFFFFFU>>(31U-((drf##_HIGH_FIELD) % 32U)+((drf##_LOW_FIELD) % 32U)))) << ((drf##_LOW_FIELD) % 32U))
#else
#define REF_NUM(drf,n)            (((n)&DRF_MASK(drf))<<DRF_SHIFT(drf))
#endif
#define FLD_TEST_REF(drf,c,v)     (REF_VAL(drf, (v)) == drf##c)
#define FLD_TEST_REF_AND(drf,c,v) (REF_VAL(drf, (v)) &  drf##c)
#define FLD_SET_REF_NUM(drf,n,v)  (((v) & ~DRF_SHIFTMASK(drf)) | REF_NUM(drf,n))

#define CR_DRF_DEF(d,r,f,c)     ((CR ## d ## r ## f ## c)<<DRF_SHIFT(CR ## d ## r ## f))
#define CR_DRF_NUM(d,r,f,n)     (((n)&DRF_MASK(CR ## d ## r ## f))<<DRF_SHIFT(CR ## d ## r ## f))
#define CR_DRF_VAL(d,r,f,v)     (((v)>>DRF_SHIFT(CR ## d ## r ## f))&DRF_MASK(CR ## d ## r ## f))

// Multi-word (MW) field manipulations.  For multi-word structures (e.g., Fermi SPH),
// fields may have bit numbers beyond 32.  To avoid errors using "classic" multi-word macros,
// all the field extents are defined as "MW(X)".  For example, MW(127:96) means
// the field is in bits 0-31 of word number 3 of the structure.
//
// DRF_VAL_MW() macro is meant to be used for native endian 32-bit aligned 32-bit word data,
// not for byte stream data.
//
// DRF_VAL_BS() macro is for byte stream data used in fbQueryBIOS_XXX().
//
#define DRF_EXPAND_MW(drf)         drf                          // used to turn "MW(a:b)" into "a:b"
#define DRF_PICK_MW(drf,v)         ((v)? DRF_EXPAND_##drf)      // picks low or high bits
#define DRF_WORD_MW(drf)           (DRF_PICK_MW(drf,0)/32)      // which word in a multi-word array
#define DRF_BASE_MW(drf)           (DRF_PICK_MW(drf,0)%32)      // which start bit in the selected word?
#define DRF_EXTENT_MW(drf)         (DRF_PICK_MW(drf,1)%32)      // which end bit in the selected word
#define DRF_SHIFT_MW(drf)          (DRF_PICK_MW(drf,0)%32)
#define DRF_MASK_MW(drf)           (0xFFFFFFFFU>>((31-(DRF_EXTENT_MW(drf))+(DRF_BASE_MW(drf)))%32))
#define DRF_SHIFTMASK_MW(drf)      ((DRF_MASK_MW(drf))<<(DRF_SHIFT_MW(drf)))
#define DRF_SIZE_MW(drf)           (DRF_EXTENT_MW(drf)-DRF_BASE_MW(drf)+1)

#define DRF_DEF_MW(d,r,f,c)        ((NV##d##r##f##c) << DRF_SHIFT_MW(NV##d##r##f))
#define DRF_NUM_MW(d,r,f,n)        (((n)&DRF_MASK_MW(NV##d##r##f))<<DRF_SHIFT_MW(NV##d##r##f))
//
// DRF_VAL_MW is the ONLY multi-word macro which supports spanning. No other MW macro supports spanning currently
//
#define DRF_VAL_MW_1WORD(d,r,f,v)       ((((v)[DRF_WORD_MW(NV##d##r##f)])>>DRF_SHIFT_MW(NV##d##r##f))&DRF_MASK_MW(NV##d##r##f))
#define DRF_SPANS(drf)                  ((DRF_PICK_MW(drf,0)/32) != (DRF_PICK_MW(drf,1)/32))
#define DRF_WORD_MW_LOW(drf)            (DRF_PICK_MW(drf,0)/32)
#define DRF_WORD_MW_HIGH(drf)           (DRF_PICK_MW(drf,1)/32)
#define DRF_MASK_MW_LOW(drf)            (0xFFFFFFFFU)
#define DRF_MASK_MW_HIGH(drf)           (0xFFFFFFFFU>>(31-(DRF_EXTENT_MW(drf))))
#define DRF_SHIFT_MW_LOW(drf)           (DRF_PICK_MW(drf,0)%32)
#define DRF_SHIFT_MW_HIGH(drf)          (0)
#define DRF_MERGE_SHIFT(drf)            ((32-((DRF_PICK_MW(drf,0)%32)))%32)
#define DRF_VAL_MW_2WORD(d,r,f,v)       (((((v)[DRF_WORD_MW_LOW(NV##d##r##f)])>>DRF_SHIFT_MW_LOW(NV##d##r##f))&DRF_MASK_MW_LOW(NV##d##r##f)) | \
    (((((v)[DRF_WORD_MW_HIGH(NV##d##r##f)])>>DRF_SHIFT_MW_HIGH(NV##d##r##f))&DRF_MASK_MW_HIGH(NV##d##r##f)) << DRF_MERGE_SHIFT(NV##d##r##f)))
#define DRF_VAL_MW(d,r,f,v)             ( DRF_SPANS(NV##d##r##f) ? DRF_VAL_MW_2WORD(d,r,f,v) : DRF_VAL_MW_1WORD(d,r,f,v) )

#define DRF_IDX_DEF_MW(d,r,f,i,c)  ((NV##d##r##f##c)<<DRF_SHIFT_MW(NV##d##r##f(i)))
#define DRF_IDX_NUM_MW(d,r,f,i,n)  (((n)&DRF_MASK_MW(NV##d##r##f(i)))<<DRF_SHIFT_MW(NV##d##r##f(i)))
#define DRF_IDX_VAL_MW(d,r,f,i,v)  ((((v)[DRF_WORD_MW(NV##d##r##f(i))])>>DRF_SHIFT_MW(NV##d##r##f(i)))&DRF_MASK_MW(NV##d##r##f(i)))

//
// Logically OR all DRF_DEF constants indexed from zero to s (semiinclusive).
// Caution: Target variable v must be pre-initialized.
//
#define FLD_IDX_OR_DRF_DEF(d,r,f,c,s,v)                 \
do                                                      \
{   NvU32 idx;                                          \
    for (idx = 0; idx < (NV ## d ## r ## f ## s); ++idx)\
    {                                                   \
        v |= DRF_IDX_DEF(d,r,f,idx,c);                  \
    }                                                   \
} while(0)


#define FLD_MERGE_MW(drf,n,v)               (((v)[DRF_WORD_MW(drf)] & ~DRF_SHIFTMASK_MW(drf)) | n)
#define FLD_ASSIGN_MW(drf,n,v)              ((v)[DRF_WORD_MW(drf)] = FLD_MERGE_MW(drf, n, v))
#define FLD_IDX_MERGE_MW(drf,i,n,v)         (((v)[DRF_WORD_MW(drf(i))] & ~DRF_SHIFTMASK_MW(drf(i))) | n)
#define FLD_IDX_ASSIGN_MW(drf,i,n,v)        ((v)[DRF_WORD_MW(drf(i))] = FLD_MERGE_MW(drf(i), n, v))

#define FLD_SET_DRF_MW(d,r,f,c,v)              FLD_MERGE_MW(NV##d##r##f, DRF_DEF_MW(d,r,f,c), v)
#define FLD_SET_DRF_NUM_MW(d,r,f,n,v)          FLD_ASSIGN_MW(NV##d##r##f, DRF_NUM_MW(d,r,f,n), v)
#define FLD_SET_DRF_DEF_MW(d,r,f,c,v)          FLD_ASSIGN_MW(NV##d##r##f, DRF_DEF_MW(d,r,f,c), v)
#define FLD_IDX_SET_DRF_MW(d,r,f,i,c,v)        FLD_IDX_MERGE_MW(NV##d##r##f, i, DRF_IDX_DEF_MW(d,r,f,i,c), v)
#define FLD_IDX_SET_DRF_DEF_MW(d,r,f,i,c,v)    FLD_IDX_MERGE_MW(NV##d##r##f, i, DRF_IDX_DEF_MW(d,r,f,i,c), v)
#define FLD_IDX_SET_DRF_NUM_MW(d,r,f,i,n,v)    FLD_IDX_ASSIGN_MW(NV##d##r##f, i, DRF_IDX_NUM_MW(d,r,f,i,n), v)

#define FLD_TEST_DRF_MW(d,r,f,c,v)          ((DRF_VAL_MW(d, r, f, (v)) == NV##d##r##f##c))
#define FLD_TEST_DRF_NUM_MW(d,r,f,n,v)      ((DRF_VAL_MW(d, r, f, (v)) == n))
#define FLD_IDX_TEST_DRF_MW(d,r,f,i,c,v)    ((DRF_IDX_VAL_MW(d, r, f, i, (v)) == NV##d##r##f##c))

#define DRF_VAL_BS(d,r,f,v)                 ( DRF_SPANS(NV##d##r##f) ? DRF_VAL_BS_2WORD(d,r,f,(v)) : DRF_VAL_BS_1WORD(d,r,f,(v)) )

//------------------------------------------------------------------------//
//                                                                        //
// Common defines for engine register reference wrappers                  //
//                                                                        //
// New engine addressing can be created like:                             //
// \#define ENG_REG_PMC(o,d,r)                     NV##d##r               //
// \#define ENG_IDX_REG_CE(o,d,i,r)                CE_MAP(o,r,i)          //
//                                                                        //
// See FB_FBPA* for more examples                                         //
//------------------------------------------------------------------------//

#define ENG_RD_REG(g,o,d,r)             GPU_REG_RD32(g, ENG_REG##d(o,d,r))
#define ENG_WR_REG(g,o,d,r,v)           GPU_REG_WR32(g, ENG_REG##d(o,d,r), (v))
#define ENG_RD_DRF(g,o,d,r,f)           ((GPU_REG_RD32(g, ENG_REG##d(o,d,r))>>GPU_DRF_SHIFT(NV ## d ## r ## f))&GPU_DRF_MASK(NV ## d ## r ## f))
#define ENG_WR_DRF_DEF(g,o,d,r,f,c)     GPU_REG_WR32(g, ENG_REG##d(o,d,r),(GPU_REG_RD32(g,ENG_REG##d(o,d,r))&~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_DEF(d,r,f,c))
#define ENG_WR_DRF_NUM(g,o,d,r,f,n)     GPU_REG_WR32(g, ENG_REG##d(o,d,r),(GPU_REG_RD32(g,ENG_REG##d(o,d,r))&~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_NUM(d,r,f,n))
#define ENG_TEST_DRF_DEF(g,o,d,r,f,c)   (ENG_RD_DRF(g, o, d, r, f) == NV##d##r##f##c)

#define ENG_RD_IDX_DRF(g,o,d,r,f,i)     ((GPU_REG_RD32(g, ENG_REG##d(o,d,r(i)))>>GPU_DRF_SHIFT(NV ## d ## r ## f))&GPU_DRF_MASK(NV ## d ## r ## f))
#define ENG_TEST_IDX_DRF_DEF(g,o,d,r,f,c,i) (ENG_RD_IDX_DRF(g, o, d, r, f, (i)) == NV##d##r##f##c)

#define ENG_IDX_RD_REG(g,o,d,i,r)       GPU_REG_RD32(g, ENG_IDX_REG##d(o,d,i,r))
#define ENG_IDX_WR_REG(g,o,d,i,r,v)     GPU_REG_WR32(g, ENG_IDX_REG##d(o,d,i,r), (v))

#define ENG_IDX_RD_DRF(g,o,d,i,r,f)     ((GPU_REG_RD32(g, ENG_IDX_REG##d(o,d,i,r))>>GPU_DRF_SHIFT(NV ## d ## r ## f))&GPU_DRF_MASK(NV ## d ## r ## f))

//
// DRF_READ_1WORD_BS() and DRF_READ_1WORD_BS_HIGH() do not read beyond the bytes that contain
// the requested value. Reading beyond the actual data causes a page fault panic when the
// immediately following page happened to be protected or not mapped.
//
#define DRF_VAL_BS_1WORD(d,r,f,v)           ((DRF_READ_1WORD_BS(d,r,f,v)>>DRF_SHIFT_MW(NV##d##r##f))&DRF_MASK_MW(NV##d##r##f))
#define DRF_VAL_BS_2WORD(d,r,f,v)           (((DRF_READ_4BYTE_BS(NV##d##r##f,v)>>DRF_SHIFT_MW_LOW(NV##d##r##f))&DRF_MASK_MW_LOW(NV##d##r##f)) | \
    (((DRF_READ_1WORD_BS_HIGH(d,r,f,v)>>DRF_SHIFT_MW_HIGH(NV##d##r##f))&DRF_MASK_MW_HIGH(NV##d##r##f)) << DRF_MERGE_SHIFT(NV##d##r##f)))

#define DRF_READ_1BYTE_BS(drf,v)            ((NvU32)(((const NvU8*)(v))[DRF_WORD_MW(drf)*4]))
#define DRF_READ_2BYTE_BS(drf,v)            (DRF_READ_1BYTE_BS(drf,v)| \
    ((NvU32)(((const NvU8*)(v))[DRF_WORD_MW(drf)*4+1])<<8))
#define DRF_READ_3BYTE_BS(drf,v)            (DRF_READ_2BYTE_BS(drf,v)| \
    ((NvU32)(((const NvU8*)(v))[DRF_WORD_MW(drf)*4+2])<<16))
#define DRF_READ_4BYTE_BS(drf,v)            (DRF_READ_3BYTE_BS(drf,v)| \
    ((NvU32)(((const NvU8*)(v))[DRF_WORD_MW(drf)*4+3])<<24))

#define DRF_READ_1BYTE_BS_HIGH(drf,v)       ((NvU32)(((const NvU8*)(v))[DRF_WORD_MW_HIGH(drf)*4]))
#define DRF_READ_2BYTE_BS_HIGH(drf,v)       (DRF_READ_1BYTE_BS_HIGH(drf,v)| \
    ((NvU32)(((const NvU8*)(v))[DRF_WORD_MW_HIGH(drf)*4+1])<<8))
#define DRF_READ_3BYTE_BS_HIGH(drf,v)       (DRF_READ_2BYTE_BS_HIGH(drf,v)| \
    ((NvU32)(((const NvU8*)(v))[DRF_WORD_MW_HIGH(drf)*4+2])<<16))
#define DRF_READ_4BYTE_BS_HIGH(drf,v)       (DRF_READ_3BYTE_BS_HIGH(drf,v)| \
    ((NvU32)(((const NvU8*)(v))[DRF_WORD_MW_HIGH(drf)*4+3])<<24))

// Calculate 2^n - 1 and avoid shift counter overflow
//
// On Windows amd64, 64 << 64 => 1
//
#define NV_TWO_N_MINUS_ONE(n) (((1ULL<<(n/2))<<((n+1)/2))-1)

#define DRF_READ_1WORD_BS(d,r,f,v) \
    ((DRF_EXTENT_MW(NV##d##r##f)<8)?DRF_READ_1BYTE_BS(NV##d##r##f,(v)): \
    ((DRF_EXTENT_MW(NV##d##r##f)<16)?DRF_READ_2BYTE_BS(NV##d##r##f,(v)): \
    ((DRF_EXTENT_MW(NV##d##r##f)<24)?DRF_READ_3BYTE_BS(NV##d##r##f,(v)): \
    DRF_READ_4BYTE_BS(NV##d##r##f,(v)))))

#define DRF_READ_1WORD_BS_HIGH(d,r,f,v) \
    ((DRF_EXTENT_MW(NV##d##r##f)<8)?DRF_READ_1BYTE_BS_HIGH(NV##d##r##f,(v)): \
    ((DRF_EXTENT_MW(NV##d##r##f)<16)?DRF_READ_2BYTE_BS_HIGH(NV##d##r##f,(v)): \
    ((DRF_EXTENT_MW(NV##d##r##f)<24)?DRF_READ_3BYTE_BS_HIGH(NV##d##r##f,(v)): \
    DRF_READ_4BYTE_BS_HIGH(NV##d##r##f,(v)))))

#define LOWESTBIT(x)            ( (x) &  (((x) - 1U) ^ (x)) )
// Destructive operation on n32
#define HIGHESTBIT(n32)     \
{                           \
    HIGHESTBITIDX_32(n32);  \
    n32 = NVBIT(n32);       \
}
#define ONEBITSET(x)            ( ((x) != 0U) && (((x) & ((x) - 1U)) == 0U) )

// Destructive operation on n32
#define NUMSETBITS_32(n32)                                         \
{                                                                  \
    n32 = n32 - ((n32 >> 1) & 0x55555555);                         \
    n32 = (n32 & 0x33333333) + ((n32 >> 2) & 0x33333333);          \
    n32 = (((n32 + (n32 >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;  \
}

/*!
 * Calculate number of bits set in a 32-bit unsigned integer.
 * Pure typesafe alternative to @ref NUMSETBITS_32.
 */
static NV_FORCEINLINE NvU32
nvPopCount32(const NvU32 x)
{
    NvU32 temp = x;
    temp = temp - ((temp >> 1) & 0x55555555U);
    temp = (temp & 0x33333333U) + ((temp >> 2) & 0x33333333U);
    temp = (((temp + (temp >> 4)) & 0x0F0F0F0FU) * 0x01010101U) >> 24;
    return temp;
}

/*!
 * Calculate number of bits set in a 64-bit unsigned integer.
 */
static NV_FORCEINLINE NvU32
nvPopCount64(const NvU64 x)
{
    NvU64 temp = x;
    temp = temp - ((temp >> 1) & 0x5555555555555555ULL);
    temp = (temp & 0x3333333333333333ULL) + ((temp >> 2) & 0x3333333333333333ULL);
    temp = (temp + (temp >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    temp = (temp * 0x0101010101010101ULL) >> 56;
    return (NvU32)temp;
}

/*!
 * Determine how many bits are set below a bit index within a mask.
 * This assigns a dense ordering to the set bits in the mask.
 *
 * For example the mask 0xCD contains 5 set bits:
 *     nvMaskPos32(0xCD, 0) == 0
 *     nvMaskPos32(0xCD, 2) == 1
 *     nvMaskPos32(0xCD, 3) == 2
 *     nvMaskPos32(0xCD, 6) == 3
 *     nvMaskPos32(0xCD, 7) == 4
 */
static NV_FORCEINLINE NvU32
nvMaskPos32(const NvU32 mask, const NvU32 bitIdx)
{
    return nvPopCount32(mask & (NVBIT32(bitIdx) - 1U));
}

// Destructive operation on n32
#define LOWESTBITIDX_32(n32)         \
{                                    \
    n32 = BIT_IDX_32(LOWESTBIT(n32));\
}

// Destructive operation on n32
#define HIGHESTBITIDX_32(n32)   \
{                               \
    NvU32 count = 0;            \
    while (n32 >>= 1)           \
    {                           \
        count++;                \
    }                           \
    n32 = count;                \
}

// Destructive operation on n32
#define ROUNDUP_POW2(n32) \
{                         \
    n32--;                \
    n32 |= n32 >> 1;      \
    n32 |= n32 >> 2;      \
    n32 |= n32 >> 4;      \
    n32 |= n32 >> 8;      \
    n32 |= n32 >> 16;     \
    n32++;                \
}

/*!
 * Round up a 32-bit unsigned integer to the next power of 2.
 * Pure typesafe alternative to @ref ROUNDUP_POW2.
 *
 * param[in] x must be in range [0, 2^31] to avoid overflow.
 */
static NV_FORCEINLINE NvU32
nvNextPow2_U32(const NvU32 x)
{
    NvU32 y = x;
    y--;
    y |= y >> 1;
    y |= y >> 2;
    y |= y >> 4;
    y |= y >> 8;
    y |= y >> 16;
    y++;
    return y;
}


static NV_FORCEINLINE NvU32
nvPrevPow2_U32(const NvU32 x )
{
    NvU32 y = x;
    y |= (y >> 1);
    y |= (y >> 2);
    y |= (y >> 4);
    y |= (y >> 8);
    y |= (y >> 16);
    return y - (y >> 1);
}

static NV_FORCEINLINE NvU64
nvPrevPow2_U64(const NvU64 x )
{
    NvU64 y = x;
    y |= (y >> 1);
    y |= (y >> 2);
    y |= (y >> 4);
    y |= (y >> 8);
    y |= (y >> 16);
    y |= (y >> 32);
    return y - (y >> 1);
}

// Destructive operation on n64
#define ROUNDUP_POW2_U64(n64) \
{                         \
    n64--;                \
    n64 |= n64 >> 1;      \
    n64 |= n64 >> 2;      \
    n64 |= n64 >> 4;      \
    n64 |= n64 >> 8;      \
    n64 |= n64 >> 16;     \
    n64 |= n64 >> 32;     \
    n64++;                \
}

#define NV_SWAP_U8(a,b) \
{                       \
    NvU8 temp;          \
    temp = a;           \
    a = b;              \
    b = temp;           \
}

#define NV_SWAP_U32(a,b)    \
{                           \
    NvU32 temp;             \
    temp = a;               \
    a = b;                  \
    b = temp;               \
}

/*!
 * @brief   Macros allowing simple iteration over bits set in a given mask.
 *
 * @param[in]       maskWidth   bit-width of the mask (allowed: 8, 16, 32, 64)
 *
 * @param[in,out]   index       lvalue that is used as a bit index in the loop
 *                              (can be declared as any NvU* or NvS* variable)
 * @param[in]       mask        expression, loop will iterate over set bits only
 */
#define FOR_EACH_INDEX_IN_MASK(maskWidth,index,mask)        \
{                                                           \
    NvU##maskWidth lclMsk = (NvU##maskWidth)(mask);         \
    for ((index) = 0U; lclMsk != 0U; (index)++, lclMsk >>= 1U)\
    {                                                       \
        if (((NvU##maskWidth)NVBIT64(0) & lclMsk) == 0U)    \
        {                                                   \
            continue;                                       \
        }
#define FOR_EACH_INDEX_IN_MASK_END                          \
    }                                                       \
}

//
// Size to use when declaring variable-sized arrays
//
#define NV_ANYSIZE_ARRAY                                                      1

//
// Returns ceil(a/b)
//
#define NV_CEIL(a,b) (((a)+(b)-1)/(b))

// Clearer name for NV_CEIL
#ifndef NV_DIV_AND_CEIL
#define NV_DIV_AND_CEIL(a, b) NV_CEIL(a,b)
#endif

#ifndef NV_MIN
#define NV_MIN(a, b)        (((a) < (b)) ? (a) : (b))
#endif

#ifndef NV_MAX
#define NV_MAX(a, b)        (((a) > (b)) ? (a) : (b))
#endif

//
// Returns absolute value of provided integer expression
//
#define NV_ABS(a) ((a)>=0?(a):(-(a)))

//
// Returns 1 if input number is positive, 0 if 0 and -1 if negative. Avoid
// macro parameter as function call which will have side effects.
//
#define NV_SIGN(s) ((NvS8)(((s) > 0) - ((s) < 0)))

//
// Returns 1 if input number is >= 0 or -1 otherwise. This assumes 0 has a
// positive sign.
//
#define NV_ZERO_SIGN(s) ((NvS8)((((s) >= 0) * 2) - 1))

// Returns the offset (in bytes) of 'member' in struct 'type'.
#ifndef NV_OFFSETOF
    #if defined(__GNUC__) && (__GNUC__ > 3)
        #define NV_OFFSETOF(type, member)   ((NvU32)__builtin_offsetof(type, member))
    #else
        #define NV_OFFSETOF(type, member)    ((NvU32)(NvU64)&(((type *)0)->member)) // shouldn't we use PtrToUlong? But will need to include windows header.
    #endif
#endif

//
// Performs a rounded division of b into a (unsigned). For SIGNED version of
// NV_ROUNDED_DIV() macro check the comments in bug 769777.
//
#define NV_UNSIGNED_ROUNDED_DIV(a,b)    (((a) + ((b) / 2U)) / (b))

/*!
 * Performs a ceiling division of b into a (unsigned).  A "ceiling" division is
 * a division is one with rounds up result up if a % b != 0.
 *
 * @param[in] a    Numerator
 * @param[in] b    Denominator
 *
 * @return a / b + a % b != 0 ? 1 : 0.
 */
#define NV_UNSIGNED_DIV_CEIL(a, b)      (((a) + (b - 1)) / (b))

/*!
 * Performs subtraction where a negative difference is raised to zero.
 * Can be used to avoid underflowing an unsigned subtraction.
 *
 * @param[in] a    Minuend
 * @param[in] b    Subtrahend
 *
 * @return a > b ? a - b : 0.
 */
#define NV_SUBTRACT_NO_UNDERFLOW(a, b) ((a)>(b) ? (a)-(b) : 0)

/*!
 * Performs a rounded right-shift of 32-bit unsigned value "a" by "shift" bits.
 * Will round result away from zero.
 *
 * @param[in] a      32-bit unsigned value to shift.
 * @param[in] shift  Number of bits by which to shift.
 *
 * @return  Resulting shifted value rounded away from zero.
 */
#define NV_RIGHT_SHIFT_ROUNDED(a, shift)                                       \
    (((a) >> (shift)) + !!((NVBIT((shift) - 1) & (a)) == NVBIT((shift) - 1)))

//
// Power of 2 alignment.
//    (Will give unexpected results if 'gran' is not a power of 2.)
//
#ifndef NV_ALIGN_DOWN
//
// Notably using v - v + gran ensures gran gets promoted to the same type as v if gran has a smaller type.
// Otherwise, if aligning a NVU64 with NVU32 granularity, the top 4 bytes get zeroed.
//
#define NV_ALIGN_DOWN(v, gran)      ((v) & ~((v) - (v) + (gran) - 1))
#endif

#ifndef NV_ALIGN_UP
//
// Notably using v - v + gran ensures gran gets promoted to the same type as v if gran has a smaller type.
// Otherwise, if aligning a NVU64 with NVU32 granularity, the top 4 bytes get zeroed.
//
#define NV_ALIGN_UP(v, gran)        (((v) + ((gran) - 1)) & ~((v) - (v) + (gran) - 1))
#endif

#ifndef NV_ALIGN_DOWN64
#define NV_ALIGN_DOWN64(v, gran)      ((v) & ~(((NvU64)gran) - 1))
#endif

#ifndef NV_ALIGN_UP64
#define NV_ALIGN_UP64(v, gran)        (((v) + ((gran) - 1)) & ~(((NvU64)gran)-1))
#endif

#ifndef NV_IS_ALIGNED
#define NV_IS_ALIGNED(v, gran)      (0U == ((v) & ((gran) - 1U)))
#endif

#ifndef NV_IS_ALIGNED64
#define NV_IS_ALIGNED64(v, gran)      (0U == ((v) & (((NvU64)gran) - 1U)))
#endif

#ifndef NVMISC_MEMSET
static NV_FORCEINLINE void *NVMISC_MEMSET(void *s, NvU8 c, NvLength n)
{
    NvU8 *b = (NvU8 *) s;
    NvLength i;

    for (i = 0; i < n; i++)
    {
        b[i] = c;
    }

    return s;
}
#endif

#ifndef NVMISC_MEMCPY
static NV_FORCEINLINE void *NVMISC_MEMCPY(void *dest, const void *src, NvLength n)
{
    NvU8 *destByte = (NvU8 *) dest;
    const NvU8 *srcByte = (const NvU8 *) src;
    NvLength i;

    for (i = 0; i < n; i++)
    {
        destByte[i] = srcByte[i];
    }

    return dest;
}
#endif

static NV_FORCEINLINE char *NVMISC_STRNCPY(char *dest, const char *src, NvLength n)
{
    NvLength i;

    for (i = 0; i < n; i++)
    {
        dest[i] = src[i];
        if (src[i] == '\0')
        {
            break;
        }
    }

    for (; i < n; i++)
    {
        dest[i] = '\0';
    }

    return dest;
}

/*!
 * Convert a void* to an NvUPtr. This is used when MISRA forbids us from doing a direct cast.
 *
 * @param[in] ptr      Pointer to be converted
 *
 * @return  Resulting NvUPtr
 */
static NV_FORCEINLINE NvUPtr NV_PTR_TO_NVUPTR(void *ptr)
{
    union
    {
        NvUPtr v;
        void *p;
    } uAddr;

    uAddr.p = ptr;
    return uAddr.v;
}

/*!
 * Convert an NvUPtr to a void*. This is used when MISRA forbids us from doing a direct cast.
 *
 * @param[in] ptr      Pointer to be converted
 *
 * @return  Resulting void *
 */
static NV_FORCEINLINE void *NV_NVUPTR_TO_PTR(NvUPtr address)
{
    union
    {
        NvUPtr v;
        void *p;
    } uAddr;

    uAddr.v = address;
    return uAddr.p;
}

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __NV_MISC_H

