/*
 * SPDX-FileCopyrightText: Copyright (c) 2011-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_object.h                                                       *
*    This is the object from which all other dynamically-allocated objects  *
*    must inherit.                                                          *
*                                                                           *
\***************************************************************************/
 
#ifndef INCLUDED_DP_OBJECT_H
#define INCLUDED_DP_OBJECT_H

#include "nvtypes.h"
#include "stddef.h"
#include "dp_hostimp.h"

static inline void dpMemCopy(void * target, const void * source, size_t len)
{
    NvU8 * t = (NvU8 *)target;
    const NvU8 * s = (const NvU8 *)source;

    while (len--) 
        *t++=*s++;
}

static inline void dpMemZero(void * target, size_t len)
{
    NvU8 * t = (NvU8 *)target;

    while (len--) 
        *t++=0;
}

static inline bool dpMemCmp(void *pvBuf1, void *pvBuf2, size_t size)
{
    NvU8 *pBuf1 = (NvU8 *)pvBuf1;
    NvU8 *pBuf2 = (NvU8 *)pvBuf2;

    if(!pBuf1 || !pBuf2 || !size)
        return false;

    do
    {
        if(*pBuf1++ == *pBuf2++)
            continue;
        else
            break;
    }while(--size);

    if(!size)
        return true;
    else
        return false;
}

namespace DisplayPort
{
    //
    // Any object allocated through "new" must virtually inherit from this type.
    // This guarantees that the memory allocation goes through dpMalloc/dpFree.
    // Leak detection is implemented only on allocations of this type.  Data
    // structures may assume 0 initialization if allocated off the heap.
    //
    // You must use virtual inheritance because objects that inherit from
    // multiple Object-derived classes would otherwise cause ambiguity when
    // someone tries to use new or delete on them.
    //
    struct Object
    {
        virtual ~Object() {}

        void *operator new(size_t sz)
        {
            void * block = dpMalloc(sz);
            if (block)
            {
                dpMemZero(block, sz);
            }
            return block;
        }

        void *operator new[](size_t sz)
        {
            void * block = dpMalloc(sz);
            if (block)
            {
                dpMemZero(block, sz);
            }
            return block;
        }

        void operator delete(void * ptr)
        {
            if (ptr)
            {
                dpFree(ptr);
            }
        }

        void operator delete[](void * ptr)
        {
            if (ptr)
            {
                dpFree(ptr);
            }
        }
    };
}

#endif // INCLUDED_DP_OBJECT_H
