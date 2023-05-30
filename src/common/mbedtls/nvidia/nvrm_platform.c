/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

// File provides RM implementations of platform utilities needed by MbedTLS.

#include "mbedtls/config.h"
#include "os/os.h"
#include "stddef.h"

void
mbedtls_platform_zeroize
(
    void   *buf,
    size_t  len
)
{
    portMemSet(buf, 0, (NvLength)len);
}

void *
mbedtls_calloc
(
    size_t n,
    size_t size
)
{
    NvLength  bytes;
    void     *ptr;

    bytes = (NvLength)(n * size);
    ptr   = portMemAllocNonPaged(bytes);
    if (ptr == NULL)
    {
        return ptr;
    }

    portMemSet(ptr, 0, bytes);

    return ptr;
}

void
mbedtls_free
(
    void *ptr
)
{
    // portMemFree handles NULL.
    portMemFree(ptr);
}

//
// MbedTLS requires snprintf function pointer - since we don't currently need,
// define empty function to be called when snprintf is needed.
// 
int nvrmSnPrintf
(
    char       *s,
    size_t      n,
    const char *format,
    ...
)
{
    return 0;
}

int (*mbedtls_snprintf)(char *s, size_t n, const char *format, ...) = nvrmSnPrintf;
