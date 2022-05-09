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
* Module: dp_guid.cpp                                                       *
*                                                                           *
\***************************************************************************/
#include "dp_internal.h"
#include "dp_guid.h"
#include "dp_buffer.h"

using namespace DisplayPort;

//
// Linear congruential random number generator
// Seed values chosen from numerical methods
//
NvU32 GUIDBuilder::random()
{
    previousRandom = static_cast<NvU32>(( ((NvU64)1664525 * previousRandom + 1013904223) & 0xFFFFFFFF) & 0xFF);
    return previousRandom;
}


GUIDBuilder::GUIDBuilder(Timer * source, NvU32 salt)
    : salt(salt), source(source)
{
    previousRandom = static_cast<NvU32>(( source->getTimeUs() & 0xFFFFFFFF) & 0xFF);
}

void GUIDBuilder::makeGuid(GUID & guid)
{
    NvU64 currentTimer = source->getTimeUs();
    guid.data[0] = static_cast<NvU8>(( salt >> 24) & 0xFF);
    guid.data[1] = static_cast<NvU8>(( salt >> 16) & 0xFF);
    guid.data[2] = static_cast<NvU8>(( salt >> 8) & 0xFF);
    guid.data[3] = static_cast<NvU8>(( salt) & 0xFF);

    guid.data[4] = static_cast<NvU8>(( currentTimer >> 56) & 0xFF);
    guid.data[5] = static_cast<NvU8>(( currentTimer >> 48) & 0xFF);
    guid.data[6] = static_cast<NvU8>(( currentTimer >> 40) & 0xFF);
    guid.data[7] = static_cast<NvU8>(( currentTimer >> 32) & 0xFF);
    guid.data[8] = static_cast<NvU8>(( currentTimer >> 24) & 0xFF);
    guid.data[9] = static_cast<NvU8>(( currentTimer >> 16) & 0xFF);
    guid.data[10] = static_cast<NvU8>(( currentTimer >> 8) & 0xFF);
    guid.data[11] = static_cast<NvU8>(( currentTimer) & 0xFF);

    unsigned rnd = random();
    guid.data[12] = static_cast<NvU8>(( rnd >> 24) & 0xFF);
    guid.data[13] = static_cast<NvU8>(( rnd >> 16) & 0xFF);
    guid.data[14] = static_cast<NvU8>(( rnd >> 8) & 0xFF);
    guid.data[15] = static_cast<NvU8>(( rnd) & 0xFF);
 
    //
    //  Spin until we get a new timer counter
    //  This guarantees a monotonitically increased counter
    //
    while (source->getTimeUs() == currentTimer)
        ;
}
