/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//
// Disable warnings when constant expressions are always true/false, and
// some signed/unsigned mismatch. To get a common implementation for all safe
// functions, we need to rely on these. There is no undefined behavior here.
//
#if PORT_COMPILER_IS_MSVC
#pragma warning( disable : 4296)
#elif PORT_COMPILER_IS_GCC
// GCC 4.6+ needed for GCC diagnostic
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
// Allow unknown pragmas to ignore unrecognized -W flags.
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wtautological-constant-out-of-range-compare"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wtype-limits"
#else
//
// On older GCCs we declare this as a system header, which tells the compiler
// to ignore all warnings in it (this has no effect on the primary source file)
//
#pragma GCC system_header
#endif
#elif PORT_COMPILER_IS_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-constant-out-of-range-compare"
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wtype-limits"
#endif

#define PORT_SAFE_OP(a, b, pRes, _op_, _US_)                   \
   ((sizeof(a) == 1) ? portSafe##_op_##_US_##8 (a, b, pRes) :  \
    (sizeof(a) == 2) ? portSafe##_op_##_US_##16(a, b, pRes) :  \
    (sizeof(a) == 4) ? portSafe##_op_##_US_##32(a, b, pRes) :  \
    (sizeof(a) == 8) ? portSafe##_op_##_US_##64(a, b, pRes) :  \
                       NV_FALSE)

#define PORT_SAFE_ADD_U(a, b, pRes) PORT_SAFE_OP(a, b, pRes, Add, U)
#define PORT_SAFE_SUB_U(a, b, pRes) PORT_SAFE_OP(a, b, pRes, Sub, U)
#define PORT_SAFE_MUL_U(a, b, pRes) PORT_SAFE_OP(a, b, pRes, Mul, U)
#define PORT_SAFE_DIV_U(a, b, pRes) PORT_SAFE_OP(a, b, pRes, Div, U)

#define PORT_SAFE_ADD_S(a, b, pRes) PORT_SAFE_OP(a, b, pRes, Add, S)
#define PORT_SAFE_SUB_S(a, b, pRes) PORT_SAFE_OP(a, b, pRes, Sub, S)
#define PORT_SAFE_MUL_S(a, b, pRes) PORT_SAFE_OP(a, b, pRes, Mul, S)
#define PORT_SAFE_DIV_S(a, b, pRes) PORT_SAFE_OP(a, b, pRes, Div, S)

#define PORT_SAFE_ADD(a, b, pRes) PORT_SAFE_ADD_U(a, b, pRes)
#define PORT_SAFE_SUB(a, b, pRes) PORT_SAFE_SUB_U(a, b, pRes)
#define PORT_SAFE_MUL(a, b, pRes) PORT_SAFE_MUL_U(a, b, pRes)
#define PORT_SAFE_DIV(a, b, pRes) PORT_SAFE_DIV_U(a, b, pRes)

////////////////////////////////////////////////////////////////////////////////

#define PORT_EXPAND(X) X
#define PORT_SAFE_MAX(t) PORT_EXPAND(NV_##t##_MAX)
#define PORT_SAFE_MIN(t) PORT_EXPAND(NV_##t##_MIN)

// These constants should really be in nvtypes.h
#if !defined (NV_UPtr_MAX)
#if defined(NV_64_BITS)
#define NV_UPtr_MAX     NV_U64_MAX
#define NV_Length_MAX   NV_U64_MAX
#else
#define NV_UPtr_MAX     NV_U32_MAX
#define NV_Length_MAX   NV_U32_MAX
#endif
#define NV_UPtr_MIN     0
#define NV_Length_MIN   0
#endif

#define PORT_WILL_OVERFLOW_UADD(a, b) ((a + b) < a)
#define PORT_WILL_OVERFLOW_USUB(a, b) (b > a)
#define PORT_WILL_OVERFLOW_UMUL(a, b, r) (a != 0 && b != (r/a))

/** @note Signed overflow is Undefined Behavior, which means we have to detect
 * it before it actually happens. We can't do (a+b) unless we are sure it won't
 * overflow.
 */
#define PORT_WILL_OVERFLOW_SADD(a, b, size)            \
    ((b < 0) ? (a < (NV_S##size##_MIN - b)) : (a > (NV_S##size##_MAX - b)))

#define PORT_WILL_OVERFLOW_SSUB(a, b, size)            \
    ((b < 0) ? (a > (NV_S##size##_MAX + b)) : (a < (NV_S##size##_MIN + b)))

#define PORT_MIN_MUL(x, s) ((x < 0) ? (NV_S##s##_MAX / x) : (NV_S##s##_MIN / x))
#define PORT_MAX_MUL(x, s) ((x < 0) ? (NV_S##s##_MIN / x) : (NV_S##s##_MAX / x))
#define PORT_WILL_OVERFLOW_SMUL(a, b, size) \
    (a != 0 && b != 0 && (a > PORT_MAX_MUL(b, size) || a < PORT_MIN_MUL(b, size)))

#define PORT_SAFE_DIV_IMPL(a, b, pRes) \
    ((b == 0) ? NV_FALSE : ((*pRes = a / b), NV_TRUE))

#define PORT_SAFE_Add_IMPL_S(a, b, pRes, n) \
    (PORT_WILL_OVERFLOW_SADD(a, b, n) ? NV_FALSE : ((*pRes = a + b), NV_TRUE))
#define PORT_SAFE_Sub_IMPL_S(a, b, pRes, n) \
    (PORT_WILL_OVERFLOW_SSUB(a, b, n) ? NV_FALSE : ((*pRes = a - b), NV_TRUE))
#define PORT_SAFE_Mul_IMPL_S(a, b, pRes, n) \
    (PORT_WILL_OVERFLOW_SMUL(a, b, n) ? NV_FALSE : ((*pRes = a * b), NV_TRUE))
#define PORT_SAFE_Div_IMPL_S(a, b, pRes, n) PORT_SAFE_DIV_IMPL(a, b, pRes)

#define PORT_SAFE_Add_IMPL_U(a, b, pRes, n) \
    ((*pRes = a + b), ((*pRes < a) ? NV_FALSE : NV_TRUE))
#define PORT_SAFE_Sub_IMPL_U(a, b, pRes, n) \
    ((*pRes = a - b), ((b > a) ? NV_FALSE : NV_TRUE))
#define PORT_SAFE_Mul_IMPL_U(a, b, pRes, n) \
    ((*pRes = a * b), ((a != 0 && b != *pRes/a) ? NV_FALSE : NV_TRUE))
#define PORT_SAFE_Div_IMPL_U(a, b, pRes, n) PORT_SAFE_DIV_IMPL(a, b, pRes)


#define PORT_SAFE_Add_IMPL_ PORT_SAFE_Add_IMPL_U
#define PORT_SAFE_Sub_IMPL_ PORT_SAFE_Sub_IMPL_U
#define PORT_SAFE_Mul_IMPL_ PORT_SAFE_Mul_IMPL_U
#define PORT_SAFE_Div_IMPL_ PORT_SAFE_Div_IMPL_U

#define PORT_SAFE_CAST(a, b, t)                       \
    ((a < PORT_SAFE_MIN(t) || a > PORT_SAFE_MAX(t)) ? \
                                    NV_FALSE : \
                            ((b = (Nv##t) a), NV_TRUE))


#define PORT_SAFE_DEFINE_MATH_FUNC(_op_, _US_, _size_)                         \
    PORT_SAFE_INLINE NvBool                                                    \
    portSafe##_op_##_US_##_size_                                               \
    (                                                                          \
        Nv##_US_##_size_  x,                                                   \
        Nv##_US_##_size_  y,                                                   \
        Nv##_US_##_size_ *pRes                                                 \
    )                                                                          \
    {                                                                          \
        return PORT_EXPAND(PORT_SAFE_##_op_##_IMPL_##_US_)(x, y, pRes, _size_);\
    }


#define PORT_SAFE_DEFINE_CAST_FUNC(_type_from_, _type_to_)                     \
    PORT_SAFE_INLINE NvBool                                                    \
    portSafe##_type_from_##To##_type_to_                                       \
    (                                                                          \
        Nv##_type_from_ data,                                                  \
        Nv##_type_to_  *pResult                                                \
    )                                                                          \
    {                                                                          \
        if (((data<0) && (PORT_SAFE_MIN(_type_to_) == 0 ||                     \
                         PORT_SAFE_MIN(_type_to_) > data))                     \
            || data > PORT_SAFE_MAX(_type_to_))                                \
            return NV_FALSE;                                                   \
        *pResult = (Nv##_type_to_) data;                                       \
        return NV_TRUE;                                                        \
    }



PORT_SAFE_DEFINE_MATH_FUNC(Add, S, 8)
PORT_SAFE_DEFINE_MATH_FUNC(Sub, S, 8)
PORT_SAFE_DEFINE_MATH_FUNC(Mul, S, 8)
PORT_SAFE_DEFINE_MATH_FUNC(Div, S, 8)

PORT_SAFE_DEFINE_MATH_FUNC(Add, S, 16)
PORT_SAFE_DEFINE_MATH_FUNC(Sub, S, 16)
PORT_SAFE_DEFINE_MATH_FUNC(Mul, S, 16)
PORT_SAFE_DEFINE_MATH_FUNC(Div, S, 16)

PORT_SAFE_DEFINE_MATH_FUNC(Add, S, 32)
PORT_SAFE_DEFINE_MATH_FUNC(Sub, S, 32)
PORT_SAFE_DEFINE_MATH_FUNC(Mul, S, 32)
PORT_SAFE_DEFINE_MATH_FUNC(Div, S, 32)

PORT_SAFE_DEFINE_MATH_FUNC(Add, S, 64)
PORT_SAFE_DEFINE_MATH_FUNC(Sub, S, 64)
PORT_SAFE_DEFINE_MATH_FUNC(Mul, S, 64)
PORT_SAFE_DEFINE_MATH_FUNC(Div, S, 64)


PORT_SAFE_DEFINE_MATH_FUNC(Add, U, 8)
PORT_SAFE_DEFINE_MATH_FUNC(Sub, U, 8)
PORT_SAFE_DEFINE_MATH_FUNC(Mul, U, 8)
PORT_SAFE_DEFINE_MATH_FUNC(Div, U, 8)

PORT_SAFE_DEFINE_MATH_FUNC(Add, U, 16)
PORT_SAFE_DEFINE_MATH_FUNC(Sub, U, 16)
PORT_SAFE_DEFINE_MATH_FUNC(Mul, U, 16)
PORT_SAFE_DEFINE_MATH_FUNC(Div, U, 16)

PORT_SAFE_DEFINE_MATH_FUNC(Add, U, 32)
PORT_SAFE_DEFINE_MATH_FUNC(Sub, U, 32)
PORT_SAFE_DEFINE_MATH_FUNC(Mul, U, 32)
PORT_SAFE_DEFINE_MATH_FUNC(Div, U, 32)

PORT_SAFE_DEFINE_MATH_FUNC(Add, U, 64)
PORT_SAFE_DEFINE_MATH_FUNC(Sub, U, 64)
PORT_SAFE_DEFINE_MATH_FUNC(Mul, U, 64)
PORT_SAFE_DEFINE_MATH_FUNC(Div, U, 64)

PORT_SAFE_DEFINE_MATH_FUNC(Add, U, Ptr)
PORT_SAFE_DEFINE_MATH_FUNC(Sub, U, Ptr)
PORT_SAFE_DEFINE_MATH_FUNC(Mul, U, Ptr)
PORT_SAFE_DEFINE_MATH_FUNC(Div, U, Ptr)

PORT_SAFE_DEFINE_MATH_FUNC(Add, , Length)
PORT_SAFE_DEFINE_MATH_FUNC(Sub, , Length)
PORT_SAFE_DEFINE_MATH_FUNC(Mul, , Length)
PORT_SAFE_DEFINE_MATH_FUNC(Div, , Length)


PORT_SAFE_DEFINE_CAST_FUNC(S8, U8)
PORT_SAFE_DEFINE_CAST_FUNC(S8, U16)
PORT_SAFE_DEFINE_CAST_FUNC(S8, U32)
PORT_SAFE_DEFINE_CAST_FUNC(S8, U64)
PORT_SAFE_DEFINE_CAST_FUNC(S8, UPtr)
PORT_SAFE_DEFINE_CAST_FUNC(S8, Length)

PORT_SAFE_DEFINE_CAST_FUNC(S16, S8)
PORT_SAFE_DEFINE_CAST_FUNC(S16, U8)
PORT_SAFE_DEFINE_CAST_FUNC(S16, U16)
PORT_SAFE_DEFINE_CAST_FUNC(S16, U32)
PORT_SAFE_DEFINE_CAST_FUNC(S16, U64)
PORT_SAFE_DEFINE_CAST_FUNC(S16, UPtr)
PORT_SAFE_DEFINE_CAST_FUNC(S16, Length)

PORT_SAFE_DEFINE_CAST_FUNC(S32, S8)
PORT_SAFE_DEFINE_CAST_FUNC(S32, S16)
PORT_SAFE_DEFINE_CAST_FUNC(S32, U8)
PORT_SAFE_DEFINE_CAST_FUNC(S32, U16)
PORT_SAFE_DEFINE_CAST_FUNC(S32, U32)
PORT_SAFE_DEFINE_CAST_FUNC(S32, U64)
PORT_SAFE_DEFINE_CAST_FUNC(S32, UPtr)
PORT_SAFE_DEFINE_CAST_FUNC(S32, Length)

PORT_SAFE_DEFINE_CAST_FUNC(S64, S8)
PORT_SAFE_DEFINE_CAST_FUNC(S64, S16)
PORT_SAFE_DEFINE_CAST_FUNC(S64, S32)
PORT_SAFE_DEFINE_CAST_FUNC(S64, U8)
PORT_SAFE_DEFINE_CAST_FUNC(S64, U16)
PORT_SAFE_DEFINE_CAST_FUNC(S64, U32)
PORT_SAFE_DEFINE_CAST_FUNC(S64, U64)
PORT_SAFE_DEFINE_CAST_FUNC(S64, UPtr)
PORT_SAFE_DEFINE_CAST_FUNC(S64, Length)

PORT_SAFE_DEFINE_CAST_FUNC(U8, S8)

PORT_SAFE_DEFINE_CAST_FUNC(U16, S8)
PORT_SAFE_DEFINE_CAST_FUNC(U16, S16)
PORT_SAFE_DEFINE_CAST_FUNC(U16, U8)

PORT_SAFE_DEFINE_CAST_FUNC(U32, S8)
PORT_SAFE_DEFINE_CAST_FUNC(U32, S16)
PORT_SAFE_DEFINE_CAST_FUNC(U32, S32)
PORT_SAFE_DEFINE_CAST_FUNC(U32, U8)
PORT_SAFE_DEFINE_CAST_FUNC(U32, U16)

PORT_SAFE_DEFINE_CAST_FUNC(U64, S8)
PORT_SAFE_DEFINE_CAST_FUNC(U64, S16)
PORT_SAFE_DEFINE_CAST_FUNC(U64, S32)
PORT_SAFE_DEFINE_CAST_FUNC(U64, S64)
PORT_SAFE_DEFINE_CAST_FUNC(U64, U8)
PORT_SAFE_DEFINE_CAST_FUNC(U64, U16)
PORT_SAFE_DEFINE_CAST_FUNC(U64, U32)
PORT_SAFE_DEFINE_CAST_FUNC(U64, UPtr)
PORT_SAFE_DEFINE_CAST_FUNC(U64, Length)

PORT_SAFE_DEFINE_CAST_FUNC(UPtr, S8)
PORT_SAFE_DEFINE_CAST_FUNC(UPtr, S16)
PORT_SAFE_DEFINE_CAST_FUNC(UPtr, S32)
PORT_SAFE_DEFINE_CAST_FUNC(UPtr, S64)
PORT_SAFE_DEFINE_CAST_FUNC(UPtr, U8)
PORT_SAFE_DEFINE_CAST_FUNC(UPtr, U16)
PORT_SAFE_DEFINE_CAST_FUNC(UPtr, U32)
PORT_SAFE_DEFINE_CAST_FUNC(UPtr, U64)
PORT_SAFE_DEFINE_CAST_FUNC(UPtr, Length)

PORT_SAFE_DEFINE_CAST_FUNC(Length, S8)
PORT_SAFE_DEFINE_CAST_FUNC(Length, S16)
PORT_SAFE_DEFINE_CAST_FUNC(Length, S32)
PORT_SAFE_DEFINE_CAST_FUNC(Length, S64)
PORT_SAFE_DEFINE_CAST_FUNC(Length, U8)
PORT_SAFE_DEFINE_CAST_FUNC(Length, U16)
PORT_SAFE_DEFINE_CAST_FUNC(Length, U32)
PORT_SAFE_DEFINE_CAST_FUNC(Length, U64)
PORT_SAFE_DEFINE_CAST_FUNC(Length, UPtr)


#if PORT_COMPILER_IS_MSVC
#pragma warning( default : 4296)
#elif PORT_COMPILER_IS_GCC && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
#pragma GCC diagnostic pop
#elif PORT_COMPILER_IS_CLANG
#pragma clang diagnostic pop
#endif
