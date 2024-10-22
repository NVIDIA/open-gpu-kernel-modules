/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NVFIXEDTYPES_INCLUDED
#define NVFIXEDTYPES_INCLUDED

#include "nvtypes.h"

/*!
 * Fixed-point master data types.
 *
 * These are master-types represent the total number of bits contained within
 * the FXP type.  All FXP types below should be based on one of these master
 * types.
 */
typedef NvS16                                                         NvSFXP16;
typedef NvS32                                                         NvSFXP32;
typedef NvS64                                                         NvSFXP64;
typedef NvU16                                                         NvUFXP16;
typedef NvU32                                                         NvUFXP32;
typedef NvU64                                                         NvUFXP64;


/*!
 * Fixed-point data types.
 *
 * These are all integer types with precision indicated in the naming of the
 * form: Nv<sign>FXP<num_bits_above_radix>_<num bits below radix>.  The actual
 * size of the data type is calculated as num_bits_above_radix +
 * num_bit_below_radix.
 *
 * All of these FXP types should be based on one of the master types above.
 */
typedef NvSFXP16                                                    NvSFXP11_5;
typedef NvSFXP16                                                    NvSFXP4_12;
typedef NvSFXP16                                                     NvSFXP8_8;
typedef NvSFXP32                                                    NvSFXP8_24;
typedef NvSFXP32                                                   NvSFXP10_22;
typedef NvSFXP32                                                   NvSFXP16_16;
typedef NvSFXP32                                                   NvSFXP18_14;
typedef NvSFXP32                                                   NvSFXP20_12;
typedef NvSFXP32                                                    NvSFXP24_8;
typedef NvSFXP32                                                    NvSFXP27_5;
typedef NvSFXP32                                                    NvSFXP28_4;
typedef NvSFXP32                                                    NvSFXP29_3;
typedef NvSFXP32                                                    NvSFXP31_1;
typedef NvSFXP64                                                   NvSFXP52_12;

typedef NvUFXP16                                                    NvUFXP0_16;
typedef NvUFXP16                                                    NvUFXP4_12;
typedef NvUFXP16                                                     NvUFXP8_8;
typedef NvUFXP32                                                    NvUFXP3_29;
typedef NvUFXP32                                                    NvUFXP4_28;
typedef NvUFXP32                                                    NvUFXP7_25;
typedef NvUFXP32                                                    NvUFXP8_24;
typedef NvUFXP32                                                    NvUFXP9_23;
typedef NvUFXP32                                                   NvUFXP10_22;
typedef NvUFXP32                                                   NvUFXP15_17;
typedef NvUFXP32                                                   NvUFXP16_16;
typedef NvUFXP32                                                   NvUFXP18_14;
typedef NvUFXP32                                                   NvUFXP20_12;
typedef NvUFXP32                                                    NvUFXP24_8;
typedef NvUFXP32                                                    NvUFXP25_7;
typedef NvUFXP32                                                    NvUFXP26_6;
typedef NvUFXP32                                                    NvUFXP28_4;

typedef NvUFXP64                                                   NvUFXP37_27;
typedef NvUFXP64                                                   NvUFXP40_24;
typedef NvUFXP64                                                   NvUFXP48_16;
typedef NvUFXP64                                                   NvUFXP52_12;
typedef NvUFXP64                                                   NvUFXP54_10;
typedef NvUFXP64                                                    NvUFXP60_4;

/*!
 * Utility macros used in converting between signed integers and fixed-point
 * notation.
 *
 * - COMMON - These are used by both signed and unsigned.
 */
#define NV_TYPES_FXP_INTEGER(x, y)                              ((x)+(y)-1):(y)
#define NV_TYPES_FXP_FRACTIONAL(x, y)                                 ((y)-1):0
#define NV_TYPES_FXP_FRACTIONAL_MSB(x, y)                       ((y)-1):((y)-1)
#define NV_TYPES_FXP_FRACTIONAL_MSB_ONE                              0x00000001
#define NV_TYPES_FXP_FRACTIONAL_MSB_ZERO                             0x00000000
#define NV_TYPES_FXP_ZERO                                                   (0)

/*!
 * - UNSIGNED - These are only used for unsigned.
 */
#define NV_TYPES_UFXP_INTEGER_MAX(x, y)                      (~(NVBIT((y))-1U))
#define NV_TYPES_UFXP_INTEGER_MIN(x, y)                                    (0U)

/*!
 * - SIGNED - These are only used for signed.
 */
#define NV_TYPES_SFXP_INTEGER_SIGN(x, y)                ((x)+(y)-1):((x)+(y)-1)
#define NV_TYPES_SFXP_INTEGER_SIGN_NEGATIVE                          0x00000001
#define NV_TYPES_SFXP_INTEGER_SIGN_POSITIVE                          0x00000000
#define NV_TYPES_SFXP_S32_SIGN_EXTENSION(x, y)                           31:(x)
#define NV_TYPES_SFXP_S32_SIGN_EXTENSION_POSITIVE(x, y)              0x00000000
#define NV_TYPES_SFXP_S32_SIGN_EXTENSION_NEGATIVE(x, y)      (NVBIT(32-(x))-1U)
#define NV_TYPES_SFXP_INTEGER_MAX(x, y)                         (NVBIT((x))-1U)
#define NV_TYPES_SFXP_INTEGER_MIN(x, y)                      (~(NVBIT((x))-1U))
#define NV_TYPES_SFXP_S64_SIGN_EXTENSION(x, y)                           63:(x)
#define NV_TYPES_SFXP_S64_SIGN_EXTENSION_POSITIVE(x, y)      0x0000000000000000
#define NV_TYPES_SFXP_S64_SIGN_EXTENSION_NEGATIVE(x, y)    (NVBIT64(64-(x))-1U)
#define NV_TYPES_SFXP_S64_INTEGER_MAX(x, y)                 (NVBIT64((x)-1)-1U)
#define NV_TYPES_SFXP_S64_INTEGER_MIN(x, y)              (~(NVBIT64((x)-1)-1U))

/*!
 * Conversion macros used for converting between integer and fixed point
 * representations.  Both signed and unsigned variants.
 *
 * Warning:
 * Note that most of the macros below can overflow if applied on values that can
 * not fit the destination type.  It's caller responsibility to ensure that such
 * situations will not occur.
 *
 * Some conversions perform some commonly performed tasks other than just
 * bit-shifting:
 *
 * - _SCALED:
 *   For integer -> fixed-point we add handling divisors to represent
 *   non-integer values.
 *
 * - _ROUNDED:
 *   For fixed-point -> integer we add rounding to integer values.
 */

// 32-bit Unsigned FXP:
#define NV_TYPES_U32_TO_UFXP_X_Y(x, y, integer)                               \
    ((NvUFXP##x##_##y) (((NvU32) (integer)) <<                                \
                        DRF_SHIFT(NV_TYPES_FXP_INTEGER((x), (y)))))

#define NV_TYPES_U32_TO_UFXP_X_Y_SCALED(x, y, integer, scale)                 \
    ((NvUFXP##x##_##y) ((((((NvU32) (integer)) <<                             \
                        DRF_SHIFT(NV_TYPES_FXP_INTEGER((x), (y))))) /         \
                            (scale)) +                                        \
                        ((((((NvU32) (integer)) <<                            \
                            DRF_SHIFT(NV_TYPES_FXP_INTEGER((x), (y)))) %      \
                                (scale)) > ((scale) >> 1)) ? 1U : 0U)))

#define NV_TYPES_UFXP_X_Y_TO_U32(x, y, fxp)                                   \
    ((NvU32) (DRF_VAL(_TYPES, _FXP, _INTEGER((x), (y)),                       \
                    ((NvUFXP##x##_##y) (fxp)))))

#define NV_TYPES_UFXP_X_Y_TO_U32_ROUNDED(x, y, fxp)                           \
    (NV_TYPES_UFXP_X_Y_TO_U32(x, y, (fxp)) +                                  \
        (FLD_TEST_DRF_NUM(_TYPES, _FXP, _FRACTIONAL_MSB((x), (y)),            \
            NV_TYPES_FXP_FRACTIONAL_MSB_ONE, ((NvUFXP##x##_##y) (fxp))) ?     \
            1U : 0U))

// 64-bit Unsigned FXP
#define NV_TYPES_U64_TO_UFXP_X_Y(x, y, integer)                               \
    ((NvUFXP##x##_##y) (((NvU64) (integer)) <<                                \
                        DRF_SHIFT64(NV_TYPES_FXP_INTEGER((x), (y)))))

#define NV_TYPES_U64_TO_UFXP_X_Y_SCALED(x, y, integer, scale)                 \
    ((NvUFXP##x##_##y) (((((NvU64) (integer)) <<                              \
                             DRF_SHIFT64(NV_TYPES_FXP_INTEGER((x), (y)))) +   \
                         ((scale) >> 1)) /                                    \
                        (scale)))

#define NV_TYPES_UFXP_X_Y_TO_U64(x, y, fxp)                                   \
    ((NvU64) (DRF_VAL64(_TYPES, _FXP, _INTEGER((x), (y)),                     \
                    ((NvUFXP##x##_##y) (fxp)))))

#define NV_TYPES_UFXP_X_Y_TO_U64_ROUNDED(x, y, fxp)                           \
    (NV_TYPES_UFXP_X_Y_TO_U64(x, y, (fxp)) +                                  \
        (FLD_TEST_DRF_NUM64(_TYPES, _FXP, _FRACTIONAL_MSB((x), (y)),          \
            NV_TYPES_FXP_FRACTIONAL_MSB_ONE, ((NvUFXP##x##_##y) (fxp))) ?     \
            1U : 0U))

//
// 32-bit Signed FXP:
// Some compilers do not support left shift negative values
// so typecast integer to NvU32 instead of NvS32
//
// Note that there is an issue with the rounding in
// NV_TYPES_S32_TO_SFXP_X_Y_SCALED. In particular, when the signs of the
// numerator and denominator don't match, the rounding is done towards positive
// infinity, rather than away from 0. This will need to be fixed in a follow-up
// change.
//
#define NV_TYPES_S32_TO_SFXP_X_Y(x, y, integer)                               \
    ((NvSFXP##x##_##y) (((NvU32) (integer)) <<                                \
                        DRF_SHIFT(NV_TYPES_FXP_INTEGER((x), (y)))))

#define NV_TYPES_S32_TO_SFXP_X_Y_SCALED(x, y, integer, scale)                 \
    ((NvSFXP##x##_##y) (((((NvS32) (integer)) <<                              \
                             DRF_SHIFT(NV_TYPES_FXP_INTEGER((x), (y)))) +     \
                         ((scale) >> 1)) /                                    \
                        (scale)))

#define NV_TYPES_SFXP_X_Y_TO_S32(x, y, fxp)                                   \
    ((NvS32) ((DRF_VAL(_TYPES, _FXP, _INTEGER((x), (y)),                      \
                    ((NvSFXP##x##_##y) (fxp)))) |                             \
              ((DRF_VAL(_TYPES, _SFXP, _INTEGER_SIGN((x), (y)), (fxp)) ==     \
                    NV_TYPES_SFXP_INTEGER_SIGN_NEGATIVE) ?                    \
                DRF_NUM(_TYPES, _SFXP, _S32_SIGN_EXTENSION((x), (y)),         \
                    NV_TYPES_SFXP_S32_SIGN_EXTENSION_NEGATIVE((x), (y))) :    \
                DRF_NUM(_TYPES, _SFXP, _S32_SIGN_EXTENSION((x), (y)),         \
                    NV_TYPES_SFXP_S32_SIGN_EXTENSION_POSITIVE((x), (y))))))

/*!
 * Note: The rounding action for signed numbers should ideally round away from
 *       0 in both the positive and the negative regions.
 *       For positive numbers, we add 1 if the fractional MSb is 1.
 *       For negative numbers, we add -1 (equivalent to subtracting 1) if the
 *       fractional MSb is 1.
 */
#define NV_TYPES_SFXP_X_Y_TO_S32_ROUNDED(x, y, fxp)                           \
    (NV_TYPES_SFXP_X_Y_TO_S32(x, y, (fxp)) +                                  \
        (FLD_TEST_DRF_NUM(_TYPES, _FXP, _FRACTIONAL_MSB((x), (y)),            \
            NV_TYPES_FXP_FRACTIONAL_MSB_ONE, ((NvSFXP##x##_##y) (fxp))) ?     \
                ((DRF_VAL(_TYPES, _SFXP, _INTEGER_SIGN((x), (y)), (fxp)) ==   \
                    NV_TYPES_SFXP_INTEGER_SIGN_POSITIVE) ? 1 : -1) : 0))

#define NV_TYPES_SFXP_X_Y_TO_FLOAT32(x, y, fxp)                               \
    ((NvF32) NV_TYPES_SFXP_X_Y_TO_S32(x, y, (fxp)) +                          \
        ((NvF32) DRF_NUM(_TYPES, _FXP, _FRACTIONAL((x), (y)),                 \
            ((NvSFXP##x##_##y) (fxp))) / (NvF32) (1 << (y))))

//
// 64-bit Signed FXP:
// Some compilers do not support left shift negative values
// so typecast integer to NvU64 instead of NvS64
//
// Note that there is an issue with the rounding in
// NV_TYPES_S64_TO_SFXP_X_Y_SCALED. In particular, when the signs of the
// numerator and denominator don't match, the rounding is done towards positive
// infinity, rather than away from 0. This will need to be fixed in a follow-up
// change.
//
#define NV_TYPES_S64_TO_SFXP_X_Y(x, y, integer)                               \
    ((NvSFXP##x##_##y) (((NvU64) (integer)) <<                                \
                        DRF_SHIFT64(NV_TYPES_FXP_INTEGER((x), (y)))))

#define NV_TYPES_S64_TO_SFXP_X_Y_SCALED(x, y, integer, scale)                 \
    ((NvSFXP##x##_##y) (((((NvS64) (integer)) <<                              \
                             DRF_SHIFT64(NV_TYPES_FXP_INTEGER((x), (y)))) +     \
                         ((scale) >> 1)) /                                    \
                        (scale)))

#define NV_TYPES_SFXP_X_Y_TO_S64(x, y, fxp)                                   \
    ((NvS64) ((DRF_VAL64(_TYPES, _FXP, _INTEGER((x), (y)),                    \
                    ((NvSFXP##x##_##y) (fxp)))) |                             \
              ((DRF_VAL64(_TYPES, _SFXP, _INTEGER_SIGN((x), (y)), (fxp)) ==   \
                    NV_TYPES_SFXP_INTEGER_SIGN_NEGATIVE) ?                    \
                DRF_NUM64(_TYPES, _SFXP, _S64_SIGN_EXTENSION((x), (y)),       \
                    NV_TYPES_SFXP_S64_SIGN_EXTENSION_NEGATIVE((x), (y))) :    \
                DRF_NUM64(_TYPES, _SFXP, _S64_SIGN_EXTENSION((x), (y)),       \
                    NV_TYPES_SFXP_S64_SIGN_EXTENSION_POSITIVE((x), (y))))))

/*!
 * Note: The rounding action for signed numbers should ideally round away from
 *       0 in both the positive and the negative regions.
 *       For positive numbers, we add 1 if the fractional MSb is 1.
 *       For negative numbers, we add -1 (equivalent to subtracting 1) if the
 *       fractional MSb is 1.
 */
#define NV_TYPES_SFXP_X_Y_TO_S64_ROUNDED(x, y, fxp)                           \
    (NV_TYPES_SFXP_X_Y_TO_S64(x, y, (fxp)) +                                  \
        (FLD_TEST_DRF_NUM64(_TYPES, _FXP, _FRACTIONAL_MSB((x), (y)),          \
            NV_TYPES_FXP_FRACTIONAL_MSB_ONE, ((NvSFXP##x##_##y) (fxp))) ?     \
                ((DRF_VAL64(_TYPES, _SFXP, _INTEGER_SIGN((x), (y)), (fxp)) == \
                    NV_TYPES_SFXP_INTEGER_SIGN_POSITIVE) ? 1 : -1) : 0))

/*!
 * Macros representing the single-precision IEEE 754 floating point format for
 * "binary32", also known as "single" and "float".
 *
 * Single precision floating point format wiki [1]
 *
 * _SIGN
 *     Single bit representing the sign of the number.
 * _EXPONENT
 *     Unsigned 8-bit number representing the exponent value by which to scale
 *     the mantissa.
 *     _BIAS - The value by which to offset the exponent to account for sign.
 * _MANTISSA
 *     Explicit 23-bit significand of the value.  When exponent != 0, this is an
 *     implicitly 24-bit number with a leading 1 prepended.  This 24-bit number
 *     can be conceptualized as FXP 9.23.
 *
 * With these definitions, the value of a floating point number can be
 * calculated as:
 *     (-1)^(_SIGN) *
 *         2^(_EXPONENT - _EXPONENT_BIAS) *
 *         (1 + _MANTISSA / (1 << 23))
 */
// [1] : https://en.wikipedia.org/wiki/Single_precision_floating-point_format
#define NV_TYPES_SINGLE_SIGN                                               31:31
#define NV_TYPES_SINGLE_SIGN_POSITIVE                                 0x00000000
#define NV_TYPES_SINGLE_SIGN_NEGATIVE                                 0x00000001
#define NV_TYPES_SINGLE_EXPONENT                                           30:23
#define NV_TYPES_SINGLE_EXPONENT_ZERO                                 0x00000000
#define NV_TYPES_SINGLE_EXPONENT_MAX                                  0x000000FE
#define NV_TYPES_SINGLE_EXPONENT_BIAS                                 0x0000007F
#define NV_TYPES_SINGLE_MANTISSA                                            22:0


/*!
 * Helper macro to return a IEEE 754 single-precision value's mantissa as an
 * unsigned FXP 9.23 value.
 *
 * @param[in] single   IEEE 754 single-precision value to manipulate.
 *
 * @return IEEE 754 single-precision values mantissa represented as an unsigned
 *     FXP 9.23 value.
 */
#define NV_TYPES_SINGLE_MANTISSA_TO_UFXP9_23(single)                           \
    ((NvUFXP9_23)(FLD_TEST_DRF(_TYPES, _SINGLE, _EXPONENT, _ZERO, single) ?    \
                    NV_TYPES_U32_TO_UFXP_X_Y(9, 23, 0) :                       \
                    (NV_TYPES_U32_TO_UFXP_X_Y(9, 23, 1) +                      \
                        DRF_VAL(_TYPES, _SINGLE, _MANTISSA, single))))

/*!
 * Helper macro to return an IEEE 754 single-precision value's exponent,
 * including the bias.
 *
 * @param[in] single   IEEE 754 single-precision value to manipulate.
 *
 * @return Signed exponent value for IEEE 754 single-precision.
 */
#define NV_TYPES_SINGLE_EXPONENT_BIASED(single)                                \
    ((NvS32)(DRF_VAL(_TYPES, _SINGLE, _EXPONENT, single) -                     \
        NV_TYPES_SINGLE_EXPONENT_BIAS))

/*!
 * Helper macro to convert an NvS8 unbiased exponent value to an IEEE 754
 * single-precision value's exponent, by adding the bias.
 * Input exponent can range from -127 to 127 which is stored in the range
 * [0, 254]
 *
 * @param[in] single   IEEE 754 single-precision value to manipulate.
 *
 * @return Biased exponent value for IEEE 754 single-precision.
 */
#define NV_TYPES_NvS32_TO_SINGLE_EXPONENT_BIASED(exponent)                      \
    ((NvU32)((exponent) + NV_TYPES_SINGLE_EXPONENT_BIAS))

/*!
 * NvTemp - temperature data type introduced to avoid bugs in conversion between
 * various existing notations.
 */
typedef NvSFXP24_8              NvTemp;

/*!
 * Macros for NvType <-> Celsius temperature conversion.
 */
#define NV_TYPES_CELSIUS_TO_NV_TEMP(cel)                                      \
                                NV_TYPES_S32_TO_SFXP_X_Y(24,8,(cel))
#define NV_TYPES_NV_TEMP_TO_CELSIUS_TRUNCED(nvt)                              \
                                NV_TYPES_SFXP_X_Y_TO_S32(24,8,(nvt))
#define NV_TYPES_NV_TEMP_TO_CELSIUS_ROUNDED(nvt)                              \
                                NV_TYPES_SFXP_X_Y_TO_S32_ROUNDED(24,8,(nvt))
#define NV_TYPES_NV_TEMP_TO_CELSIUS_FLOAT(nvt)                                \
                                NV_TYPES_SFXP_X_Y_TO_FLOAT32(24,8,(nvt))

/*!
 * Macro for NvType -> number of bits conversion
 */
#define NV_NBITS_IN_TYPE(type) (8 * sizeof(type))

/*!
 * Macro to convert SFXP 11.5 to NvTemp.
 */
#define NV_TYPES_NVSFXP11_5_TO_NV_TEMP(x) ((NvTemp)(x) << 3)

/*!
 * Macro to convert NvTemp to SFXP 11.5.
 */
#define NV_TYPES_NV_TEMP_TO_NVSFXP11_5(x) ((NvSFXP11_5)(x) >> 3)

/*!
 * Macro to convert UFXP 5.3 to NvTemp.
 */
#define NV_TYPES_NVUFXP5_3_TO_NV_TEMP(x) ((NvTemp)(x) << 5)

/*!
 * Macro to convert UFXP11.5 Watts to NvU32 milli-Watts.
 */
#define NV_TYPES_NVUFXP11_5_WATTS_TO_NVU32_MILLI_WATTS(x) ((((NvU32)(x)) * ((NvU32)1000)) >> 5)

#endif /* NVFIXEDTYPES_INCLUDED */
