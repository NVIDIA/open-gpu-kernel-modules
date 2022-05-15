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

/******************************* DisplayPort*********************************\
*                                                                           *
* Module: dp_crc.cpp                                                        *
*    CRC Algorithms for the messaging subsystem.                            *
*                                                                           *
\***************************************************************************/
#include "dp_internal.h"
#include "dp_bitstream.h"
#include "dp_crc.h"
using namespace DisplayPort;

//
//  DP CRC for transactions headers
//  
unsigned DisplayPort::dpCalculateHeaderCRC(BitStreamReader * reader)
{
    unsigned remainder = 0;
    unsigned bit, i;

    while (reader->read(&bit, 1))
    {
        remainder <<= 1;
        remainder |= bit; 
        if ((remainder & 0x10) == 0x10)
        {
            remainder ^= 0x13;
        }
    }

    for (i = 4; i != 0; i--)
    {
        remainder <<= 1;
        if ((remainder & 0x10) != 0)
        {
            remainder ^= 0x13;
        }
    }

    return remainder & 0xF;
}

//
//  DP CRC for body
//
unsigned DisplayPort::dpCalculateBodyCRC(BitStreamReader * reader)
{
    unsigned remainder = 0;
    unsigned bit, i;

    while (reader->read(&bit, 1))
    {
        remainder <<= 1;
        remainder |= bit; 
        if ((remainder & 0x100) == 0x100)
        {
            remainder ^= 0xD5;
        }
    }

    for (i = 8; i != 0; i--)
    {
        remainder <<= 1;
        if ((remainder & 0x100) != 0)
        {
            remainder ^= 0xD5;
        }
    }

    return remainder & 0xFF;
}
