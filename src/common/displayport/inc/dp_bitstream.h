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
* Module: dp_bitstream.h                                                    *
*    This is an implementation of the big endian bit stream                 *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_BITSTREAM_H
#define INCLUDED_DP_BITSTREAM_H

#include "dp_buffer.h"

namespace DisplayPort
{
    //
    //  Bitstream reader interface
    //    - reads a packed stream of bits in Big Endian format
    //    - handles alignment, buffering, and buffer bounds checking
    //
    class BitStreamReader
    {
        Buffer * sourceBuffer;
        unsigned  bitsOffset;
        unsigned  bitsEnd;

    public:
        // Read 1-32 bits from the stream into *value.  Returns true on success
        bool read(unsigned * value, unsigned bits);

        // Read 1-32 bits from stream.  Returns 'default' on failure.
        unsigned  readOrDefault(unsigned bits, unsigned defaultValue);

        // Skip bits until we're aligned to the power of two alignment
        bool align(unsigned align);

        unsigned  offset();
        Buffer *  buffer();
        BitStreamReader(Buffer * buffer, unsigned bitsOffset, unsigned bitsCount);
    };

    //
    //  Bitstream writer interface
    //
    class BitStreamWriter
    {
        Buffer * targetBuffer;
        unsigned  bitsOffset;
    public:
        //
        // Create a bitstream writer at a specific bit offset
        // into an already existing buffer
        //
        BitStreamWriter(Buffer * buffer, unsigned bitsOffset = 0);   

        //
        //  Write n bits to the buffer in big endian format.
        //  No buffering is performed.
        //
        bool write(unsigned value, unsigned bits);

        //
        // Emit zero's until the offset is divisible by align.
        //  CAVEAT: align must be a power of 2 (eg 8)
        //
        bool align(unsigned align);

        //
        //  Get current offset and buffer target
        //  
        unsigned  offset();
        Buffer *  buffer();
    };
}

#endif //INCLUDED_DP_BITSTREAM_H
