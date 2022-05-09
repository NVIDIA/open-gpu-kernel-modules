/*
 * SPDX-FileCopyrightText: Copyright (c) 2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NV_ASSERT_H__
#define __NV_ASSERT_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * nvAssert() has three possible configurations: __COVERITY__, DEBUG,
 * and non-DEBUG.  In DEBUG builds, the includer should provide an
 * implementation of nvDebugAssert().
 */

#if defined(__COVERITY__)
  /*
   * Coverity assert handling -- basically inform coverity that the
   * condition is verified independently and coverity can assume that
   * it is true.
   */
  void __coverity_panic__(void);

  #define nvAssert(exp)           \
    do {                          \
        if (exp) {                \
        } else {                  \
            __coverity_panic__(); \
        }                         \
    } while (0)

#elif defined(DEBUG)

  void nvDebugAssert(const char *expString, const char *filenameString,
                     const char *funcString, const unsigned int lineNumber);

  /*
   * Assert that (exp) is TRUE.  We use 'if (exp) { } else { fail }'
   * instead of 'if (!(exp)) { fail }' to cause warnings when people
   * accidentally write nvAssert(foo = bar) instead of nvAssert(foo ==
   * bar).
   */
  #define nvAssert(exp)                                            \
    do {                                                           \
        if (exp) {                                                 \
        } else {                                                   \
            nvDebugAssert(#exp, __FILE__, __FUNCTION__, __LINE__); \
        }                                                          \
    } while (0)

#else

  #define nvAssert(exp) {}

#endif

#ifdef __cplusplus
};
#endif

#endif /* __NV_ASSERT_H__ */
