/*
 * SPDX-FileCopyrightText: Copyright (c) 1997-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef __NV_CTASSERT_H
#define __NV_CTASSERT_H

/*****************************************************************************/

/* Compile Time assert
 * -------------------
 * Use ct_assert(b) instead of assert(b) whenever the condition 'b' is constant,
 * i.e. when 'b' can be determined at compile time.
 *
 * e.g.: check array size:
 *       ct_assert(__GL_ARRAYSIZE(arrayName) == constArraySize);
 * e.g.: check struct size alignment:
 *       ct_assert(sizeof(struct xy) % 64 == 0);
 *
 * When available, standard C or C++ language constructs are used:
 * - ISO C++11 defines the static_assert keyword
 * - ISO C11 defines the _Static_assert keyword
 *
 * Note that recent versions of Clang support _Static_assert in all compiler modes
 * - not just C11 mode - so we test for that in addition to checking explicitly for
 * C11 and C++11 support.
 *
 * Those new language standards aren't available on all supported platforms; an
 * alternate method which involves array declarations is employed in that case,
 * described below.
 *
 * In C, there is a restriction where ct_assert() can be placed:
 * It can be placed wherever a variable declaration can be placed, i.e.:
 * - either anywhere at file scope
 * - or inside a function at the beginning of any {} block; it may be mixed
 *   with variable declarations.
 *   e.g.:
 *   void function()
 *   {
 *       ct_assert(...);     <-- ok    \
 *       int a;                         |
 *       ct_assert(...);     <-- ok     | declaration section
 *       int b;                         |
 *       ct_assert(...);     <-- ok    /
 *
 *       a = 0;                        -- first statement
 *
 *       int c;              <-- error
 *       ct_assert(...);     <-- error
 *
 *       {ct_assert(...);}   <-- ok (uses its own block for ct_assert())
 *   }
 *
 * In CPP, there is no such restriction, i.e. it can be placed at file scope
 * or anywhere inside a function or namespace or class (i.e., wherever
 * a variable declaration may be placed).
 *
 * For C code, the mechanism of this ct_assert() is to declare a prototype
 * of a function (e.g. compile_time_assertion_failed_in_line_555, if current
 * line number is 555), which gets an array as argument:
 * (1) the size of this array is +1, if b != 0 (ok)
 * (2) the size of this array is -1, if b == 0 (error)
 *
 * In case (2) the compiler throws an error.
 * e.g. msvc compiler:
 *      error C2118: negative subscript or subscript is too large
 * e.g. gcc 2.95.3:
 *      size of array '_compile_time_assertion_failed_in_line_555' is negative
 *
 * In case the condition 'b' is not constant, the msvc compiler throws
 * an error:
 *      error C2057: expected constant expression
 * In this case the run time assert() must be used.
 *
 * For C++ code, we use a different technique because the function prototype
 * declaration can have function linkage conflicts.  If a single compilation
 * unit has ct_assert() statements on the same line number in two different
 * files, we would have:
 *
 *   compile_time_assertion_failed_in_line_777(...);    from xxx.cpp
 *   compile_time_assertion_failed_in_line_777(...);    from xxx.h
 *
 * That is valid C++.  But if either declaration were in an extern "C" block,
 * the same function would be declared with two different linkage types and an
 * error would ensue.
 *
 * Instead, ct_assert() for C++ simply declares an array typedef.  As in the C
 * version, we will get a compilation error if a typedef with a negative size
 * is specified.  Line numbers are not needed because C++ allows redundant
 * typedefs as long as they are all defined the same way.  But we tack them on
 * anyway in case the typedef name is reported in compiler errors.  C does not
 * permit redundant typedefs, so this version should not be used in true C
 * code.  It can be used in extern "C" blocks of C++ code, however.  As with
 * the C version, MSVC will throw a "negative subscript" or "expected constant
 * expression" error if the expression asserted is false or non-constant.
 *
 * Notes:
 * - This ct_assert() does *not* generate any code or variable.
 *   Therefore there is no need to define it away for RELEASE builds.
 * - The integration of the current source file number (__LINE__) ...
 *   ... would be required in C++ to allow multiple use inside the same
 *       class/namespace (if we used the C-style expansion), because the id
 *       must be unique.
 *   ... is nice to have in C or C++ if the compiler's error message contains
 *       the id (this is not the case for msvc)
 * - Using three nested macros instead of only one is necessary to get the id
 *       compile_time_assertion_failed_in_line_555
 *   instead of
 *       compile_time_assertion_failed_in_line___LINE__
 */

#if defined(__clang__)
# ifndef __has_extension
#  define __has_extension __has_feature // Compatibility with Clang pre-3.0 compilers.
# endif
# define CLANG_C_STATIC_ASSERT __has_extension(c_static_assert)
#else
# define CLANG_C_STATIC_ASSERT 0
#endif

// Adding this macro to fix MISRA 2012 rule 20.12
#define NV_CTASSERT_STRINGIFY_MACRO(b) #b

#if (defined(__cplusplus) && __cplusplus >= 201103L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201103L)
 // ISO C++11 defines the static_assert keyword
# define ct_assert(b)  static_assert((b), "Compile time assertion failed: " NV_CTASSERT_STRINGIFY_MACRO(b))
# define ct_assert_i(b,line)  static_assert((b), "Compile time assertion failed: " NV_CTASSERT_STRINGIFY_MACRO(b)NV_CTASSERT_STRINGIFY_MACRO(line))
#elif !defined(NVOC) && ((defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) || CLANG_C_STATIC_ASSERT)
 // ISO C11 defines the _Static_assert keyword
# define ct_assert(b)  _Static_assert((b), "Compile time assertion failed: " NV_CTASSERT_STRINGIFY_MACRO(b))
# define ct_assert_i(b,line)  _Static_assert((b), "Compile time assertion failed: " NV_CTASSERT_STRINGIFY_MACRO(b)NV_CTASSERT_STRINGIFY_MACRO(line))
#else
 // For compilers which don't support ISO C11 or C++11, we fall back to an
 // array (type) declaration
# define ct_assert(b)         ct_assert_i(b,__LINE__)
# define ct_assert_i(b,line)  ct_assert_ii(b,line)
# ifdef __cplusplus
#  define ct_assert_ii(b,line) typedef char compile_time_assertion_failed_in_line_##line[(b)?1:-1]
# else
 /*
  * The use of a function prototype "void compile_time_assertion_failed_in_line_##line(..)
  * above violates MISRA-C 2012 Rule 8.6 since the rule disallows a function
  * declaration without a definition. To fix the MISRA rule, the cplusplus style
  * 'typdef char compile_time_assertion_failed_in_line_##line'
  * is acceptable, but doesn't work for typical C code since there can be duplicate
  * line numbers leading to duplicate typedefs which C doesn't allow.
  *
  * The following macro uses the predefined macro __COUNTER__ to create unique
  * typedefs that fixes the MISRA violations. However, not all C compilers support
  * that macro and even for compilers that support it, the underlying code makes
  * use of variably modified identifiers in ct_assert that makes the use of this
  * unviable.
  *
  * For now restrict the use of MACRO only on
  * i)  GCC 4.3.0 and above that supports __COUNTER__ macro
  * ii) Specifically the Falcon port of the compiler since the use of variably
  * modified identifiers have been removed on those projects
  *
  * TBD: Enable the macro on MSVC and CLANG pending
  */
#  if defined(__GNUC__) && ((__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) >= 40300) && defined(GCC_FALCON)
#   define ct_assert_ii(b,line)        ct_assert_iii(b,line,__COUNTER__)
#   define ct_assert_iii(b,line,cntr)  ct_assert_cntr(b,line,cntr)
#   define ct_assert_cntr(b,line,cntr) typedef char cnt##cntr##_compile_time_assertion_failed_in_line_##line[(b)?1:-1] __attribute__((unused))
#  else
#   define ct_assert_ii(b,line) void compile_time_assertion_failed_in_line_##line(int _compile_time_assertion_failed_in_line_##line[(b) ? 1 : -1])
#  endif
# endif
#endif

#endif // __NV_CTASSERT_H
