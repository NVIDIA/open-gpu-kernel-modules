/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* DisplayPort********************************\
*                                                                           *
* Module: dp_buffer.cpp                                                     *
*    Resizable byte buffer and stream operations                            *
*                                                                           *
\***************************************************************************/

#include "dp_internal.h"
#include "dp_buffer.h"

using namespace DisplayPort;

void DisplayPort::swapBuffers(Buffer & left, Buffer & right)
{
    NvU8        *tmpData        = left.data;
    unsigned    tmpLength       = left.length;
    unsigned    tmpCapacity     = left.capacity;
    bool        tmpErrorState   = left.errorState;

    left.data       = right.data;
    left.length     = right.length;
    left.capacity   = right.capacity;
    left.errorState = right.errorState;

    right.data      = tmpData;
    right.length    = tmpLength;
    right.capacity  = tmpCapacity;
    right.errorState= tmpErrorState;
}


bool Stream::seek(unsigned where)
{
    //
    //  Allow seek to any position in the file INCLUDING
    //   the first byte past the end of the file.
    //
    if (where > this->parent->length)
    {
        return false;
    }

    this->byteOffset = where;

    return true;
}

bool Stream::read(NvU8 * buffer, unsigned size)
{
    unsigned stopReadAt = this->byteOffset + size;

    if (stopReadAt > this->parent->length)
    {
        return false;
    }

    dpMemCopy(buffer, this->parent->data + this->byteOffset, size);
    this->byteOffset = stopReadAt;

    return true;
}

bool Buffer::resize(unsigned stopWriteAt)
{
    bool mustIncrease = stopWriteAt > this->capacity;

    if (mustIncrease || (stopWriteAt * 4 < this->capacity) )
    {
        unsigned newCapacity;
        NvU8 * newBuffer;

        newCapacity = 32;

        while (newCapacity <= stopWriteAt)
        {
            newCapacity *= 2;
        }

        if (newCapacity == this->capacity) {
            this->length = stopWriteAt;
            return true;
        }

        newBuffer = (NvU8 *)dpMalloc(sizeof(NvU8) * newCapacity);

        if (!newBuffer)
        {
            if (mustIncrease)
            {
                if (this->data)
                {
                    dpFree(this->data);
                }

                this->errorState = true;
                this->data = 0;
                this->capacity = 0;
                this->length = 0;
            }
            else
                newCapacity = this->capacity;

            return false;
        }

        if (this->data)
        {
            dpMemCopy(newBuffer, this->data, DP_MIN(newCapacity, this->length));
            dpFree(this->data);
        }

        this->data = newBuffer;
        this->capacity = newCapacity;

    }

    this->length = stopWriteAt;
    return true;
}

void Buffer::memZero()
{
    if (this->data)
        dpMemZero(this->data, this->length);
}

bool Stream::write(NvU8 * buffer, unsigned size)
{
    unsigned stopWriteAt = this->byteOffset + size;

    if (stopWriteAt > this->parent->length)
    {
        this->parent->resize(stopWriteAt);
    }

    if (isError())
        return false;

    dpMemCopy( this->parent->data + this->byteOffset, buffer, size);
    this->byteOffset = stopWriteAt;
    this->parent->length = DP_MAX(this->parent->length, stopWriteAt);

    return true;
}

unsigned  Stream::remaining()
{
    return this->parent->length - this->byteOffset;
}

unsigned  Stream::offset()
{
    return this->byteOffset;
}

Buffer::~Buffer()
{
    reset();
}

void Buffer::reset()
{
    if (this->data)
    {
        dpFree(this->data);
    }

    length = 0;
    capacity = 0;
    data = 0;
    errorState = false;
}

bool Buffer::isError() const
{
    return this->errorState;
}


Stream::Stream(Buffer * buffer)
 : parent(buffer), byteOffset(0)
{
}

bool Stream::isError() const
{
    return this->parent->errorState;
}

Buffer::Buffer()
   : data(0), length(0), capacity(0),  errorState(false)
{
}

Buffer::Buffer(NvU8 * src, unsigned size)
   : data(0), length(0), capacity(0),  errorState(false)
{
    if (src && size && resize(size) && data)
        dpMemCopy(data, src, size);
}

Buffer::Buffer(const Buffer & other)
   : data(0), length(0), capacity(0), errorState(false)
{
    if (other.isError())
    {
        errorState = true;
    }
    else
    {
        if (resize(other.getLength()) && other.getData())
            dpMemCopy(getData(), other.getData(), getLength());
    }
}

Buffer & Buffer::operator =  (const Buffer & other)
{
    if (other.isError())
    {
        errorState = true;
    }
    else
    {
        if (resize(other.getLength()))
            dpMemCopy(getData(), other.getData(), getLength());
    }
    return *this;
}


bool Buffer::operator== (const Buffer  & other) const
{
    if (length != other.length)
        return false;

    for (unsigned i = 0; i < length; i++)
    {
        if (data[i] != other.data[i])
            return false;

    }

    return true;
}
