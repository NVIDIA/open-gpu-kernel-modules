/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_messageheader.cpp                                              *
*    DP message header parser                                               *
*                                                                           *
\***************************************************************************/

#include "dp_internal.h"
#include "dp_bitstream.h"
#include "dp_crc.h"
#include "dp_messageheader.h"
#include "dp_printf.h"


bool DisplayPort::decodeHeader(BitStreamReader * reader, MessageHeader * header, const Address & address)
{
    unsigned  startOffset = reader->offset();
    int  LCT, i;
    
    //
    //  Read the RAD
    //
    LCT = reader->readOrDefault( 4, 0);
    reader->readOrDefault( 4, 0);

    header->address = address;

    for (i = 0; i < LCT - 1; i++)
    {
        header->address.append(reader->readOrDefault( 4, 0));
    }
    
    reader->align( 8);

    //
    //  Read flags
    //
    header->isBroadcast = !!reader->readOrDefault( 1, 0); 
    header->isPathMessage = !!reader->readOrDefault( 1, 0); 
    header->payloadBytes = reader->readOrDefault( 6, 0) ; 

    header->isTransactionStart = !!reader->readOrDefault( 1, 0); 
    header->isTransactionEnd = !!reader->readOrDefault( 1, 0); 
    reader->readOrDefault( 1, 0);
    header->messageNumber = reader->readOrDefault( 1, 0); 
    

    // Build a bit reader for the slice of header we just processed
    BitStreamReader crcReader(reader->buffer(), startOffset, reader->offset());

    if (reader->readOrDefault( 4, (NvU32)~0) != dpCalculateHeaderCRC(&crcReader))
    {
        // Corrupt packet received
        char buffer[48*3+1];
        dpHexDump(&buffer[0], sizeof(buffer), (NvU8*)reader->buffer() + startOffset, reader->offset() - startOffset);
        DP_PRINTF(DP_ERROR, "DP-MM> Corrupt message transaction.  Expected CRC %d. Message = {%s}",  dpCalculateHeaderCRC(&crcReader), buffer);

        return false;
    }

    header->headerSizeBits = reader->offset() - startOffset;
    return true;
}
