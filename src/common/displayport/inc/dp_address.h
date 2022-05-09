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
* Module: dp_address.h                                                      *
*    Basic class for AUX Address                                            *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_ADDRESS_H
#define INCLUDED_DP_ADDRESS_H

#include "dp_internal.h"

namespace DisplayPort
{
    class Address
    {
    public:
        enum
        {
            maxHops = 15,       // update DP_MAX_ADDRESS_HOPS when changed (in displayportCommon.h)
            maxHopsHDCP = 7,
            maxPortCount = 15
        };

        Address()
        {
            clear();
        }

        Address(unsigned hop0)
        {
            clear();
            hop[hops++] = hop0;
        }

        Address(unsigned hop0, unsigned hop1)
        {
            clear();
            hop[hops++] = hop0;
            hop[hops++] = hop1;
        }

        Address(const Address & other)
        {
            clear();
            for(unsigned i = 0; i < other.size(); i++)
            {
                append(other[i]);
            }
        }

        void clear() 
        {
            hops = 0;
            for (unsigned i = 0; i < maxHops; i++)
            {
                hop[i] = 0;
            }
        }

        Address parent() const
        {
            DP_ASSERT(hops != 0);
            Address addr = *this;
            addr.hops --;
            return addr;
        }

        unsigned tail() const
        {
            if (hops == 0)
            {
                DP_ASSERT(hops != 0);
                return 0;
            }
            return hop[hops-1];
        }

        void append(unsigned port)
        {
            if (hops >= maxHops)
            {
                DP_ASSERT(0);
                return;
            }
            hop[hops++] = port;
        }

        void prepend(unsigned port)
        {
            if (hops >= maxHops)
            {
                DP_ASSERT(0);
                return;
            }
            hops++;
            for (unsigned i = hops - 1; i > 0; i--)
                hop[i] = hop[i-1];
            hop[0] = port;
        }

        void pop()
        {
            if (hops == 0)
            {
                DP_ASSERT(0);
                return;
            }
            hops--;
        }

        // Just to keep clear copy
        Address & operator = (const Address & other)
        {
            clear();
            for(unsigned i = 0; i < other.size(); i++)
            {
                append(other[i]);
            }

            return *this;
        }

        bool operator == (const Address & other) const
        {
            if (other.size() != size())
                return false;

            for (unsigned i = 0; i < hops; i++)
                if (other[i] != (*this)[i])
                    return false;

            return true;
        }

        //
        //  Sort by size first, then "alphabetically" (lexicographical see wikipedia)
        //
        bool operator > (const Address & other) const
        {
            if (size() > other.size())
                return true;
            else if (size() < other.size())
                return false;

            for (unsigned i = 0; i < hops; i++)
            {
                if ((*this)[i] > other[i])
                    return true;
                else if ((*this)[i] < other[i])
                    return false;
            }

            return false;
        }

        //
        //  Sort by size first, then "alphabetically" (lexicographical see wikipedia)
        //
        bool operator < (const Address & other) const
        {
            if (size() < other.size())
                return true;
            else if (size() > other.size())
                return false;

            for (unsigned i = 0; i < hops; i++)
            {
                if ((*this)[i] < other[i])
                    return true;
                else if ((*this)[i] > other[i])
                    return false;
            }

            return false;
        }

        bool operator >= (const Address & other) const
        {
            return !((*this) < other);
        }

        bool operator <= (const Address & other) const
        {
            return !((*this) > other);
        }

        bool operator != (const Address & other) const
        {
            return !((*this) == other);
        }

        unsigned size() const
        {
            return hops;
        }

        unsigned & operator [](unsigned index)
        {
            DP_ASSERT(index < hops);
            return hop[index];
        }

        const unsigned & operator [](unsigned index) const
        {
            DP_ASSERT(index < hops);
            return hop[index];
        }

        bool under(const Address & root) const
        {
            if (size() < root.size())
                return false;

            for (unsigned i = 0; i < root.size(); i++)
                if ((*this)[i] != root[i])
                    return false;

            return true;
        }

        typedef char StringBuffer[maxHops*3+1];
        char * toString(StringBuffer & buffer, bool removeLeadingZero = false) const
        {
            char * p = &buffer[0];
            int hopsWritten = 0;
            for (unsigned i = 0; i < hops; i++)
            {
                if (i == 0 && hop[0] == 0 && removeLeadingZero)
                    continue;
                if (hopsWritten > 0)
                    *p++ = '.';
                if (hop[i] >= 10)
                    *p++ = (char)(hop[i] / 10 +'0');
                *p++ = (char)(hop[i] % 10 + '0');
                hopsWritten++;
            }

            *p++= 0;
            return (char *)&buffer[0];
        }

        // Large enough to fit 4 hops into every NvU32
        typedef NvU32 NvU32Buffer[(maxHops-1)/4+1 < 4 ? 4 : (maxHops-1)/4+1];
        NvU32 * toNvU32Buffer(NvU32Buffer & buffer) const
        {
            for (unsigned i = 0; i < hops; i++)
            {
                buffer[i/4] |= ((NvU8) hop[i]) << (i % 4) * 8;
            }

            return (NvU32 *)&buffer[0];
        }

    private:
        unsigned hop[maxHops];
        unsigned hops;
    };
}

#endif //INCLUDED_DP_ADDRESS_H
