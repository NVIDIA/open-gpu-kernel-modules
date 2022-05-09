/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NV_SOFTFLOAT_H__
#define __NV_SOFTFLOAT_H__

/*
 * This header file provides utility code built on top of the softfloat floating
 * point emulation library.
 */

#include "softfloat.h"
#include "nvtypes.h"
#include "platform.h"

/*
 * float32_t stores the bit pattern for a 32-bit single-precision IEEE floating
 * point value in a structure containing an uint32_t:
 *
 *   typedef struct { uint32_t v; } float32_t;
 *
 * In some cases, clients pass in a 32-bit single-precision IEEE floating
 * point value in an NvU32.
 *
 * Define functions to change the "view" between an NvU32 and a float32_t.
 */
INLINE float32_t NvU32viewAsF32(NvU32 u)
{
    float32_t f = { .v = u };
    return f;
}

INLINE NvU32 F32viewAsNvU32(float32_t f)
{
    return f.v;
}

/*
 * Convert the value of a float32_t to an NvU16.
 *
 * The conversion requires several steps:
 *
 * - Clamp the float32_t value to the [0,NV_U16_MAX] range of NvU16.
 *
 * - Use softfloat to convert the float32_t to ui32, with appropriate rounding.
 *
 * - Due to the clamping and rounding above, the value in the ui32 should be in
 *   the range of NvU16 and can be safely returned as NvU16.
 */
INLINE NvU16 F32toNvU16(float32_t f)
{
    const float32_t minF32 = NvU32viewAsF32(0);
    const float32_t maxF32 = ui32_to_f32(NV_U16_MAX);
    NvU32 u;

    /* clamp to zero: f = (f < minF32) ? minF32 : f */
    f = f32_lt(f, minF32) ? minF32 : f;

    /* clamp to NV_U16_MAX: f = (maxF32 < f) ? maxF32 : f */
    f = f32_lt(maxF32, f) ? maxF32 : f;

    /*
     * The "_r_minMag" in "f32_to_ui32_r_minMag" means round "to minimum
     * magnitude" (i.e., round towards zero).
     *
     * The "exact = FALSE" argument means do not raise the inexact exception
     * flag, even if the conversion is inexact.
     *
     * For more on f32_to_ui32_r_minMag() semantics, see
     * drivers/common/softfloat/doc/SoftFloat.html
     */
    u = f32_to_ui32_r_minMag(f, NV_FALSE /* exact */);
    nvAssert(u <= NV_U16_MAX);

    return (NvU16) u;
}

/*
 * Perform the following with float32_t: (a * b) + (c * d) + e
 */
INLINE float32_t F32_AxB_plus_CxD_plus_E(
    float32_t a,
    float32_t b,
    float32_t c,
    float32_t d,
    float32_t e)
{
    const float32_t tmpA = f32_mul(a, b);
    const float32_t tmpB = f32_mul(c, d);
    const float32_t tmpC = f32_add(tmpA, tmpB);

    return f32_add(tmpC, e);
}

/*
 * Perform the following with float32_t: (a * b) - (c * d)
 */
INLINE float32_t F32_AxB_minus_CxD(
    float32_t a,
    float32_t b,
    float32_t c,
    float32_t d)
{
    const float32_t tmpA = f32_mul(a, b);
    const float32_t tmpB = f32_mul(c, d);

    return f32_sub(tmpA, tmpB);
}

/*
 * Perform the following with float64_t: a * -1
 */
INLINE float64_t F64_negate(float64_t a)
{
    const float64_t negOneF64 = i32_to_f64(-1);
    return f64_mul(negOneF64, a);
}

INLINE float16_t nvUnormToFp16(NvU16 unorm, float32_t maxf)
{
    const float32_t unormf = ui32_to_f32(unorm);
    const float32_t normf = f32_div(unormf, maxf);

    return f32_to_f16(normf);
}

INLINE float16_t nvUnorm10ToFp16(NvU16 unorm10)
{
    const float32_t maxf = NvU32viewAsF32(0x44800000U); // 1024.0f
    return nvUnormToFp16(unorm10, maxf);
}

INLINE float32_t f32_min(float32_t a, float32_t b)
{
    return (f32_lt(a, b)) ? a : b;
}

INLINE float32_t f32_max(float32_t a, float32_t b)
{
    return (f32_lt(a, b)) ? b : a;
}

#endif /* __NV_SOFTFLOAT_H__ */
