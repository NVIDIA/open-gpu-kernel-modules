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

#ifndef __NV_KREF_H__
#define __NV_KREF_H__

#include <asm/atomic.h>

typedef struct nv_kref
{
    atomic_t refcount;
} nv_kref_t;

static inline void nv_kref_init(nv_kref_t *nv_kref)
{
    atomic_set(&nv_kref->refcount, 1);
}

static inline void nv_kref_get(nv_kref_t *nv_kref)
{
    atomic_inc(&nv_kref->refcount);
}

static inline int nv_kref_put(nv_kref_t *nv_kref,
                              void (*release)(nv_kref_t *nv_kref))
{
    if (atomic_dec_and_test(&nv_kref->refcount))
    {
	release(nv_kref);
	return 1;
    }

    return 0;
}

static inline unsigned int nv_kref_read(const nv_kref_t *nv_kref)
{
    return atomic_read(&nv_kref->refcount);
}

#endif // __NV_KREF_H__
