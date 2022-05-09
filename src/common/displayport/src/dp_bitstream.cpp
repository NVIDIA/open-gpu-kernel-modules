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

/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_bitstream.c                                                    *
*    Implementation of Big Endian bit streams.                              *
*                                                                           *
\***************************************************************************/

#include "dp_internal.h"
#include "dp_bitstream.h"

using namespace DisplayPort;
bool BitStreamReader::read(unsigned * value, unsigned bits)
{
    unsigned topbit = (7- (this->bitsOffset & 7));

    if (this->bitsOffset + bits > this->bitsEnd)
    {
        return false;
    }

    //
    //  We're filling the byte down from 'topbit' towards 0.
    //    Can we fit all of the bits starting at topbit before
    //    overflowing to the next byte?
    //
    if (bits <= (topbit+1)) 
    {
        int bottombit = topbit - (bits-1);
        *value = (this->buffer()->data[this->bitsOffset / 8] >> bottombit) & ((1 << bits)-1);

        this->bitsOffset+=bits;
        return true;
    }

    //
    //  We're either reading too many bits or we're straddling
    //  a byte boundary.  Serialize bit by bit.
    //  NOTE: This scenario is entire unlikely.  Don't optimize.
    //

    *value = 0;
    while (bits) 
    {
        unsigned bit;
        if (!read(&bit, 1))
        {
            return false;
        }
        *value = *value * 2 + bit;
        bits--;
    }

    return true;
}

unsigned  BitStreamReader::readOrDefault(unsigned bits, unsigned defaultValue)
{
    unsigned value;

    if (read(&value, bits))
    {
        return value;
    }
    else
    {
        return defaultValue;
    }
}


bool BitStreamReader::align(unsigned align)
{
    // Verify alignment is a power of two 
    if (!(align && ((align & (align - 1)) == 0)))
    {
        DP_ASSERT(0);
    }
    else
    {
        if (this->bitsOffset & (align - 1))
        {
            this->bitsOffset = (this->bitsOffset + align) &~ (align - 1);
        }
    }
    return this->bitsOffset <= this->bitsEnd;
}

bool BitStreamWriter::write(unsigned value, unsigned bits)
{
    DP_ASSERT((value < (1ULL << bits)) && "Value out of range");
    unsigned topbit = (7- (this->bitsOffset & 7));

    if (this->bitsOffset + bits > this->buffer()->length * 8)
    {
        this->buffer()->resize((this->bitsOffset + bits+7)/8);
    }

    //
    //  We're filling the byte down from 'topbit' towards 0.
    //    Can we fit all of the bits starting at topbit before
    //    overflowing to the next byte?
    //
    if (bits <= (topbit+1)) 
    {
        int bottombit = topbit - (bits-1);
        NvU8  clearmask = ((1 << bits)-1) << bottombit;

        this->buffer()->data[this->bitsOffset / 8] = (NvU8)((this->buffer()->data[this->bitsOffset / 8] &~ clearmask) | (value << bottombit));

        this->bitsOffset+=bits;
        return true;
    }

    //
    //  We're either writing too many bits or we're straddling
    //  a byte boundary.  Serialize bit by bit.
    //  NOTE: This scenario is entire unlikely.  Don't optimize.
    //

    while (bits)
    {
        bits --;
        if (!write( (value >> bits) & 1, 1))
        {
            return false;
        }
    }

    return true;
}

bool BitStreamWriter::align(unsigned align)
{
    // Verify alignment is a power of two 
    if (!(align && ((align & (align - 1)) == 0)))
    {
        DP_ASSERT(0);
    }
    else
    {
        if (this->bitsOffset & (align - 1))
            return this->write(0, align - (this->bitsOffset & (align - 1)));
    }

    return true;
}

unsigned BitStreamReader::offset() 
{
    return this->bitsOffset;
}

unsigned BitStreamWriter::offset() 
{
    return this->bitsOffset;
}

Buffer * BitStreamWriter::buffer() 
{
    return this->targetBuffer;
}

Buffer * BitStreamReader::buffer() 
{
    return this->sourceBuffer;
}


BitStreamWriter::BitStreamWriter(Buffer * buffer, unsigned bitsOffset)
{
    this->targetBuffer = buffer;
    this->bitsOffset = bitsOffset;
}


BitStreamReader::BitStreamReader(Buffer * buffer, unsigned bitsOffset, unsigned bitsCount)
{
    this->sourceBuffer = buffer;
    this->bitsOffset = bitsOffset;
    this->bitsEnd = bitsCount + bitsOffset;
}
