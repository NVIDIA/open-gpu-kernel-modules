/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
  * @brief Standard utility macros for some more advanced CPP operations
  */

#ifndef _NV_UTILS_MACRO_H_
#define _NV_UTILS_MACRO_H_

/**
 * @defgroup NV_UTILS_MACRO Standard utility Macros
 *
 * @brief Implements commonly used macros for advanced CPP operations
 *
 * @{
 */

/**
 * @brief Expands all arguments
 */
#define NV_EXPAND(...) __VA_ARGS__
/**
 * @brief Discards all arguments
 */
#define NV_DISCARD(...)

/**
 * @brief Fully expands the given argument, then stringifies it.
 */
#define NV_STRINGIFY(s) _NV_STRINGIFY(s)
/**
 * @brief Fully expands both arguments, then concatenates them.
 */
#define NV_CONCATENATE(a, b) _NV_CONCATENATE(a, b)

/**
 * @brief Returns a number literal corresponding to the number of arguments.
 *
 * NV_NUM_ARGS(x)     expands to 1
 * NV_NUM_ARGS(x,y,z) expands to 3
 *
 * @warning Due to differences in standards, it is impossible to make this
 * consistently return 0 when called without arguments. Thus, the behavior of
 * NV_NUM_ARGS() is undefined, and shouldn't be counted on.
 * If you do decide to use it: It usually returns 0, except when -std=c++11.
 *
 * @note Works for a maximum of 16 arguments
 */
#define NV_NUM_ARGS(...) _NV_NUM_ARGS(unused, ##__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define _NV_NUM_ARGS(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, N, ...) N

/**
 * @brief Performs an operation on each of its arguments, except first
 *
 * @param what - Function or function-like macro that takes exactly one param.
 *               This will be called for other args: what(arg1), what(arg2), ...
 *
 * NV_FOREACH_ARG(foo, a, b, c) expands to: foo(a), foo(b), foo(c)
 * #define INC(x) (x+1)
 * NV_FOREACH_ARG(INC,0,1,2,3,4) expands to: (0+1), (1+1), (2+1), (3+1), (4+1)
 *
 * @note Works for a maximum of 16 arguments, not counting 'what' param
 */
#define NV_FOREACH_ARG(what, ...) \
    NV_CONCATENATE(_NV_FOREACH_ARG_, NV_NUM_ARGS(__VA_ARGS__)) (what, __VA_ARGS__)

/**
 * @brief Similar NV_FOREACH_ARG, but without comma in the expanded result
 *
 * @param what - Function or function-like macro that takes exactly one param.
 *               This will be called for other args: what(arg1) what(arg2) ...
 *
 * NV_FOREACH_ARG(foo, a, b, c) expands to: foo(a) foo(b) foo(c)
 * #define OR(x)              | (x)
 * #define FLAGS(...)         (0 NV_FOREACH_ARG_NOCOMMA(OR, __VA_ARGS__))
 * FLAGS(flag1, flag2, flag3) expands to: 0 | (flag1) | (flag2) | (flag3)
 *
 * @note Works for a maximum of 16 arguments, not counting 'what' param
 */
#define NV_FOREACH_ARG_NOCOMMA(what, ...) \
    NV_CONCATENATE(_NV_FOREACH_ARG_NOCOMMA_, NV_NUM_ARGS(__VA_ARGS__)) (what, __VA_ARGS__)


/**
 * @brief Compile time evaluate a condition
 *
 * - If 'cond' evaluates to 1 at compile time, macro expands to 'then'
 * - If 'cond' evaluates to 0 at compile time, macro expands to nothing
 * - If 'cond' is undefined or evaluates to anything else, report a build error
 */
#define NV_STATIC_IF(cond, then) \
    NV_EXPAND(NV_CONCATENATE(NV_STATIC_IF_, NV_EXPAND(cond))) (then)


/**
 * @brief Similar to @ref NV_STATIC_IF except condition is reversed
 *
 * - If 'cond' evaluates to 0 at compile time, macro expands to 'then'
 * - If 'cond' evaluates to 1 at compile time, macro expands to nothing
 * - If 'cond' is undefined or evaluates to anything else, report a build error
 */
#define NV_STATIC_IFNOT(cond, then) \
    NV_EXPAND(NV_CONCATENATE(NV_STATIC_IFNOT_, NV_EXPAND(cond))) (then)


/**
 * @brief Similar to @ref NV_STATIC_IF except with both THEN and ELSE branches
 *
 * - If 'cond' evaluates to 1 at compile time, macro expands to 'then'
 * - If 'cond' evaluates to 0 at compile time, macro expands to 'els'
 * - If 'cond' is undefined or evaluates to anything else, report a build error
 */
#define NV_STATIC_IFELSE(cond, then, els) \
    NV_STATIC_IF(NV_EXPAND(cond), then) NV_STATIC_IFNOT(NV_EXPAND(cond), els)

/// @}

/// @cond NV_MACROS_IMPLEMENTATION

#define _NV_STRINGIFY(s) #s
#define _NV_CONCATENATE(a, b) a##b

#define NV_STATIC_IF_0(then) NV_DISCARD(then)
#define NV_STATIC_IF_1(then) NV_EXPAND(then)

#define NV_STATIC_IFNOT_0(then) NV_EXPAND(then)
#define NV_STATIC_IFNOT_1(then) NV_DISCARD(then)

// Iterating over empty list is unsupported. Give a semi-readable error.
#define _NV_FOREACH_ARG_0(X) NV_FOREACH_ERROR_argument_list_emtpy

#define _NV_FOREACH_ARG_1(X, _1) \
    X(_1)
#define _NV_FOREACH_ARG_2(X, _1, _2) \
    X(_1), X(_2)
#define _NV_FOREACH_ARG_3(X, _1, _2, _3) \
    X(_1), X(_2), X(_3)
#define _NV_FOREACH_ARG_4(X, _1, _2, _3, _4) \
    X(_1), X(_2), X(_3), X(_4)
#define _NV_FOREACH_ARG_5(X, _1, _2, _3, _4, _5) \
    X(_1), X(_2), X(_3), X(_4), X(_5),
#define _NV_FOREACH_ARG_6(X, _1, _2, _3, _4, _5, _6) \
    X(_1), X(_2), X(_3), X(_4), X(_5), X(_6)
#define _NV_FOREACH_ARG_7(X, _1, _2, _3, _4, _5, _6, _7) \
    X(_1), X(_2), X(_3), X(_4), X(_5), X(_6), X(_7)
#define _NV_FOREACH_ARG_8(X, _1, _2, _3, _4, _5, _6, _7, _8) \
    X(_1), X(_2), X(_3), X(_4), X(_5), X(_6), X(_7), X(_8)
#define _NV_FOREACH_ARG_9(X, _1, _2, _3, _4, _5, _6, _7, _8, _9) \
    X(_1), X(_2), X(_3), X(_4), X(_5), X(_6), X(_7), X(_8), X(_9)
#define _NV_FOREACH_ARG_10(X, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
    X(_1), X(_2), X(_3), X(_4), X(_5), X(_6), X(_7), X(_8), X(_9), X(_10)
#define _NV_FOREACH_ARG_11(X, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) \
    X(_1), X(_2), X(_3), X(_4), X(_5), X(_6), X(_7), X(_8), X(_9), X(_10), X(_11)
#define _NV_FOREACH_ARG_12(X, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) \
    X(_1), X(_2), X(_3), X(_4), X(_5), X(_6), X(_7), X(_8), X(_9), X(_10), X(_11), X(_12)
#define _NV_FOREACH_ARG_13(X, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) \
    X(_1), X(_2), X(_3), X(_4), X(_5), X(_6), X(_7), X(_8), X(_9), X(_10), X(_11), X(_12), X(_13)
#define _NV_FOREACH_ARG_14(X, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) \
    X(_1), X(_2), X(_3), X(_4), X(_5), X(_6), X(_7), X(_8), X(_9), X(_10), X(_11), X(_12), X(_13), X(_14)
#define _NV_FOREACH_ARG_15(X, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) \
    X(_1), X(_2), X(_3), X(_4), X(_5), X(_6), X(_7), X(_8), X(_9), X(_10), X(_11), X(_12), X(_13), X(_14), X(_15)
#define _NV_FOREACH_ARG_16(X, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) \
    X(_1), X(_2), X(_3), X(_4), X(_5), X(_6), X(_7), X(_8), X(_9), X(_10), X(_11), X(_12), X(_13), X(_14), X(_15), X(_16) 

// Iterating over empty list is unsupported. Give a semi-readable error.
#define _NV_FOREACH_ARG_NOCOMMA_0(X) NV_FOREACH_NOCOMMA_ERROR_argument_list_emtpy

#define _NV_FOREACH_ARG_NOCOMMA_1(X, _1) \
    X(_1)
#define _NV_FOREACH_ARG_NOCOMMA_2(X, _1, _2) \
    X(_1) X(_2)
#define _NV_FOREACH_ARG_NOCOMMA_3(X, _1, _2, _3) \
    X(_1) X(_2) X(_3)
#define _NV_FOREACH_ARG_NOCOMMA_4(X, _1, _2, _3, _4) \
    X(_1) X(_2) X(_3) X(_4)
#define _NV_FOREACH_ARG_NOCOMMA_5(X, _1, _2, _3, _4, _5) \
    X(_1) X(_2) X(_3) X(_4) X(_5)
#define _NV_FOREACH_ARG_NOCOMMA_6(X, _1, _2, _3, _4, _5, _6) \
    X(_1) X(_2) X(_3) X(_4) X(_5) X(_6)
#define _NV_FOREACH_ARG_NOCOMMA_7(X, _1, _2, _3, _4, _5, _6, _7) \
    X(_1) X(_2) X(_3) X(_4) X(_5) X(_6) X(_7)
#define _NV_FOREACH_ARG_NOCOMMA_8(X, _1, _2, _3, _4, _5, _6, _7, _8) \
    X(_1) X(_2) X(_3) X(_4) X(_5) X(_6) X(_7) X(_8)
#define _NV_FOREACH_ARG_NOCOMMA_9(X, _1, _2, _3, _4, _5, _6, _7, _8, _9) \
    X(_1) X(_2) X(_3) X(_4) X(_5) X(_6) X(_7) X(_8) X(_9)
#define _NV_FOREACH_ARG_NOCOMMA_10(X, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
    X(_1) X(_2) X(_3) X(_4) X(_5) X(_6) X(_7) X(_8) X(_9) X(_10)
#define _NV_FOREACH_ARG_NOCOMMA_11(X, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) \
    X(_1) X(_2) X(_3) X(_4) X(_5) X(_6) X(_7) X(_8) X(_9) X(_10) X(_11)
#define _NV_FOREACH_ARG_NOCOMMA_12(X, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) \
    X(_1) X(_2) X(_3) X(_4) X(_5) X(_6) X(_7) X(_8) X(_9) X(_10) X(_11) X(_12)
#define _NV_FOREACH_ARG_NOCOMMA_13(X, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) \
    X(_1) X(_2) X(_3) X(_4) X(_5) X(_6) X(_7) X(_8) X(_9) X(_10) X(_11) X(_12) X(_13)
#define _NV_FOREACH_ARG_NOCOMMA_14(X, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) \
    X(_1) X(_2) X(_3) X(_4) X(_5) X(_6) X(_7) X(_8) X(_9) X(_10) X(_11) X(_12) X(_13) X(_14)
#define _NV_FOREACH_ARG_NOCOMMA_15(X, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) \
    X(_1) X(_2) X(_3) X(_4) X(_5) X(_6) X(_7) X(_8) X(_9) X(_10) X(_11) X(_12) X(_13) X(_14) X(_15)
#define _NV_FOREACH_ARG_NOCOMMA_16(X, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) \
    X(_1) X(_2) X(_3) X(_4) X(_5) X(_6) X(_7) X(_8) X(_9) X(_10) X(_11) X(_12) X(_13) X(_14) X(_15) X(_16) 

/// @endcond

/// @cond NV_MACROS_COMPILE_TIME_TESTS
//
// What follows are a couple of compile time smoke tests that will let us know 
// if the given compiler does not properly implement these macros.
// These are disabled by default in the interest of compile time.
// 
#if defined(NVMACRO_DO_COMPILETIME_TESTS)
#if NV_NUM_ARGS(a) != 1
#error "[NvMacros CT Test] NV_NUM_ARGS fails when given 1 args"
#endif
#if NV_NUM_ARGS(a,b,c,d) != 4
#error "[NvMacros CT Test] NV_NUM_ARGS fails when given 4 args"
#endif
#if NV_NUM_ARGS(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) != 16
#error "[NvMacros CT Test] NV_NUM_ARGS fails when given 16 args"
#endif

#define _NVMACRO_ADD_TYPE(x) int x
extern void _nvmacro_compiletime_test_func_proto1(NV_FOREACH_ARG(_NVMACRO_ADD_TYPE, aa, bb, cc));

#define _NVMACRO_ADD_TYPES(...) NV_FOREACH_ARG(_NVMACRO_ADD_TYPE, __VA_ARGS__)
extern void _nvmacro_compiletime_test_func_proto2(_NVMACRO_ADD_TYPES(a, b, c));

#endif // NVMACRO_DO_COMPILETIME_TESTS
/// @endcond

#endif // _NV_UTILS_MACRO_H_
