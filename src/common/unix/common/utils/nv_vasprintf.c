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

#include "nv_vasprintf.h"

/*
 * nv_vasprintf(): function that returns a string using vsnprintf(); intended to
 * be used by vararg printing functions.  This is supposedly correct
 * for differing semantics of vsnprintf() in different versions of
 * glibc:
 *
 * different semantics of the return value from (v)snprintf:
 *
 * -1 when the buffer is not long enough (glibc < 2.1)
 *
 *   or
 *
 * the length the string would have been if the buffer had been large
 * enough (glibc >= 2.1)
 *
 * This function allocates memory for the returned string; the caller should use
 * free() the memory when done.
 *
 * The includer should implement nv_vasprintf_{alloc,free,vsnprintf}.
 */

#define __NV_VASPRINTF_LEN 64

char* nv_vasprintf(const char *f, va_list ap)
{
    int len, current_len = __NV_VASPRINTF_LEN;
    char *b = (char *)nv_vasprintf_alloc(current_len);

    while (b) {
        va_list tmp_ap;

        va_copy(tmp_ap, ap);
        len = nv_vasprintf_vsnprintf(b, current_len, f, tmp_ap);
        va_end(tmp_ap);

        if ((len > -1) && (len < current_len)) {
            break;
        } else if (len > -1) {
            current_len = len + 1;
        } else {
            current_len += __NV_VASPRINTF_LEN;
        }

        nv_vasprintf_free(b);
        b = (char *)nv_vasprintf_alloc(current_len);
    }

    return b;
}
