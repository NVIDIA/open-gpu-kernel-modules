/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef __NV_LIST_HELPERS_H__
#define __NV_LIST_HELPERS_H__

#include <linux/list.h>
#include "conftest.h"

/* list_last_entry_or_null() doesn't actually exist in the kernel */
#if !defined(list_last_entry_or_null)
    #define list_last_entry_or_null(ptr, type, member) \
        (!list_empty(ptr) ? list_last_entry(ptr, type, member) : NULL)
#endif

#if !defined(NV_LIST_IS_FIRST_PRESENT)
    static inline int list_is_first(const struct list_head *list,
                                    const struct list_head *head)
    {
        return list->prev == head;
    }
#endif

#endif // __NV_LIST_HELPERS_H__
