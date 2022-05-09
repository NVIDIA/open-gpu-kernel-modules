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
* Module: dp_guid.h                                                         *
*    GUID struct and builder class                                          *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_GUID_H
#define INCLUDED_DP_GUID_H

#include "dp_internal.h"
#include "dp_timer.h"

namespace DisplayPort
{
#define DPCD_GUID_SIZE  16

    struct GUID
    {
        NvU8 data[DPCD_GUID_SIZE];

        GUID()
        {
            dpMemZero(&data, sizeof(data));
        }

        bool isGuidZero()
        {
            for (unsigned i = 0 ; i < DPCD_GUID_SIZE; i++)
                if (data[i])
                    return false;

            return true;
        }

        bool operator == (const GUID & other) const
        {
            for (unsigned i = 0 ; i < DPCD_GUID_SIZE; i++)
                if (data[i] != other.data[i])
                    return false;

            return true;
        }

        bool operator != (const GUID & other) const
        {
            return !((*this) == other);
        }

        void copyFrom(const NvU8 * buffer)
        {
            dpMemCopy(&this->data[0], buffer, sizeof data);
        }

        // XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
        // Two Xs per byte, plus four dashes and a NUL byte.
        typedef char StringBuffer[DPCD_GUID_SIZE*2 + 5];
        char * toString(StringBuffer & buffer) const
        {
            char *p = &buffer[0];

            for (unsigned i = 0; i < DPCD_GUID_SIZE; i++) {
                dpByteToHexChar(p, data[i]);
                p += 2;
                if (i == 3 || i == 5 || i == 7 || i == 9)
                    *p++ = '-';
            }

            *p++ = '\0';

            DP_ASSERT(p == buffer + sizeof(buffer));

            return buffer;
        }
    };

    class GUIDBuilder
    {
        NvU32   salt;
        NvU32   previousRandom;
        Timer * source;


        //
        // Linear congruential random number generator
        // Seed values chosen from numerical methods
        //
        NvU32 random();

    public:
        GUIDBuilder(Timer * source, NvU32 salt);
        
        void makeGuid(GUID & guid);
    };
}

#endif //INCLUDED_DP_GUID_H
