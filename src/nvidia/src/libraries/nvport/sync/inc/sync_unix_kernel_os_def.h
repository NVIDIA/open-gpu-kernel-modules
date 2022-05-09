/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief SYNC unix kernel struct implementations
 *
 */

#ifndef _NVPORT_SYNC_UNIX_KERNEL_DEF_H_
#define _NVPORT_SYNC_UNIX_KERNEL_DEF_H_

#include "os-interface.h"

struct PORT_SPINLOCK
{
    void *lock;
    NvU64 oldIrql;
    PORT_MEM_ALLOCATOR *pAllocator;
};

struct PORT_MUTEX
{
    void *mutex;
    PORT_MEM_ALLOCATOR *pAllocator;
};

struct PORT_SEMAPHORE
{
    void *sem;
    PORT_MEM_ALLOCATOR *pAllocator;
};

#endif
