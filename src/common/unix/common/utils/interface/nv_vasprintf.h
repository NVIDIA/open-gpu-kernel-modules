/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NV_VASPRINTF_H__
#define __NV_VASPRINTF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stddef.h>

/*
 * nv_vasprintf() depends on nv_vasprintf_{alloc,free,vsnprintf}().
 * Those functions should be implemented by the user of
 * nv_vasprintf().
 */
void *nv_vasprintf_alloc(size_t size);
void nv_vasprintf_free(void *ptr);
int nv_vasprintf_vsnprintf(char *str, size_t size,
                           const char *format, va_list ap);

char* nv_vasprintf(const char *f, va_list ap);

/*
 * NV_VSNPRINTF(): macro that assigns b using nv_vasprintf(); intended to
 * be used by vararg printing functions.
 *
 * This macro allocates memory for b; the caller should free the
 * memory when done.
 */

#define NV_VSNPRINTF(b, f) do {                             \
    va_list ap;                                             \
    va_start(ap, f);                                        \
    (b) = nv_vasprintf(f, ap);                              \
    va_end(ap);                                             \
} while(0)

#ifdef __cplusplus
};
#endif

#endif /* __NV_VASPRINTF_H__ */
