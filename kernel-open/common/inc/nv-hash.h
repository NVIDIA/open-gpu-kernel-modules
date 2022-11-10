/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-22 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef __NV_HASH_H__
#define __NV_HASH_H__

#include "conftest.h"
#include "nv-list-helpers.h"
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/hash.h>

#if defined(NV_LINUX_STRINGHASH_H_PRESENT)
#include <linux/stringhash.h>       /* full_name_hash() */
#else
#include <linux/dcache.h>
#endif

#if (NV_FULL_NAME_HASH_ARGUMENT_COUNT == 3)
#define nv_string_hash(_str) full_name_hash(NULL, _str, strlen(_str))
#else
#define nv_string_hash(_str) full_name_hash(_str, strlen(_str))
#endif

/**
 * This naive hashtable was introduced by commit d9b482c8ba19 (v3.7, 2012-10-31).
 * To support older kernels import necessary functionality from
 * <linux/hashtable.h>.
 */

#define NV_HASH_SIZE(name) (ARRAY_SIZE(name))
#define NV_HASH_BITS(name) ilog2(NV_HASH_SIZE(name))

/* Use hash_32 when possible to allow for fast 32bit hashing in 64bit kernels. */
#define NV_HASH_MIN(val, bits) \
    (sizeof(val) <= 4 ? hash_32(val, bits) : hash_long(val, bits))

#define NV_DECLARE_HASHTABLE(name, bits) \
    struct hlist_head name[1 << (bits)]

static inline void _nv_hash_init(struct hlist_head *ht, unsigned int sz)
{
    unsigned int i;

    for (i = 0; i < sz; i++)
    {
        INIT_HLIST_HEAD(&ht[i]);
    }
}

/**
 * nv_hash_init - initialize a hash table
 * @hashtable: hashtable to be initialized
 */
#define nv_hash_init(hashtable) _nv_hash_init(hashtable, NV_HASH_SIZE(hashtable))

/**
 * nv_hash_add - add an object to a hashtable
 * @hashtable: hashtable to add to
 * @node: the &struct hlist_node of the object to be added
 * @key: the key of the object to be added
 */
#define nv_hash_add(hashtable, node, key) \
    hlist_add_head(node, &hashtable[NV_HASH_MIN(key, NV_HASH_BITS(hashtable))])

/**
 * nv_hash_for_each_possible - iterate over all possible objects hashing to the
 * same bucket
 * @name: hashtable to iterate
 * @obj: the type * to use as a loop cursor for each entry
 * @member: the name of the hlist_node within the struct
 * @key: the key of the objects to iterate over
 */
#define nv_hash_for_each_possible(name, obj, member, key) \
    hlist_for_each_entry(obj, &name[NV_HASH_MIN(key, NV_HASH_BITS(name))], member)

#endif // __NV_HASH_H__
