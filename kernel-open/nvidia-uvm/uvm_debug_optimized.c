/*******************************************************************************
    Copyright (c) 2015 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

// This file provides simple wrappers that are always built with optimizations
// turned on to WAR issues with functions that don't build correctly otherwise.

#include "uvm_linux.h"

int nv_atomic_xchg(atomic_t *val, int new)
{
    return atomic_xchg(val, new);
}

int nv_atomic_cmpxchg(atomic_t *val, int old, int new)
{
    return atomic_cmpxchg(val, old, new);
}

long nv_atomic_long_cmpxchg(atomic_long_t *val, long old, long new)
{
    return atomic_long_cmpxchg(val, old, new);
}

unsigned long nv_copy_from_user(void *to, const void __user *from, unsigned long n)
{
    return copy_from_user(to, from, n);
}

unsigned long nv_copy_to_user(void __user *to, const void *from, unsigned long n)
{
    return copy_to_user(to, from, n);
}

