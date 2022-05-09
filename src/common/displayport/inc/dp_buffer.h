/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_buffer.h                                                       *
*    Resizable byte buffer and stream classes                               *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_BUFFER_H
#define INCLUDED_DP_BUFFER_H

#include "dp_internal.h"

namespace DisplayPort
{
    class Buffer
    {
    public:
        NvU8        *data;         // Data buffer
        unsigned    length;        // bytes used 
        unsigned    capacity;      // size of allocation
        bool        errorState;    // did we lose a malloc in there?
    public: 
        //
        //  Write will only fail if we're unable to reallocate the buffer.  In this case
        //  the buffer will be reset to its empty state. 
        //
        const NvU8 * getData() const { return data; }
        NvU8 * getData() { return data; }
        bool resize(unsigned newSize);
        void memZero();
        void reset();
        unsigned getLength() const { return length; }
        
        // Is in error state? This happens if malloc fails.  Error state is 
        // held until reset is called.
        bool isError() const;

        Buffer(const Buffer & other);
        Buffer(NvU8 * data, unsigned size);
        Buffer & operator = (const Buffer & other);
        Buffer();
        ~Buffer();

        void swap(Buffer & other) {
            swap_args(other.data, data);
            swap_args(other.length, length);
            swap_args(other.capacity, capacity);
            swap_args(other.errorState, errorState);
        }
        
        bool operator== (const Buffer  & other) const;
    };

    class Stream
    {
    protected:
        Buffer * parent; 
        unsigned  byteOffset;
    public:
        Stream(Buffer * buffer);
        bool seek(unsigned where);
        bool read(NvU8 * buffer, unsigned size);
        bool write(NvU8 * buffer, unsigned size);
        
        // returns error state of buffer
        bool isError() const;
        unsigned  remaining();
        unsigned  offset();
    };
    
    void swapBuffers(Buffer & left, Buffer & right);
}

#endif //INCLUDED_DP_BUFFER_H
