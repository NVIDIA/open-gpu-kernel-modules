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
* Module: dp_timeout.h                                                      *
*    Local timeout management                                               *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_TIMEOUT_H
#define INCLUDED_DP_TIMEOUT_H

#include "dp_timer.h"

namespace DisplayPort
{
    //
    //  Timeout management
    //
    class Timeout : virtual public Object
    {
        Timer * timer;
        NvU64 timeoutTime;    // What time to trigger the timeout at
                            
    public:

        Timeout(Timer * _timer, int timeoutMilliseconds)
            : timer(_timer), timeoutTime(_timer->getTimeUs() + timeoutMilliseconds*1000 + 1 /* counter could be about to roll */)
        {
        }

        NvS64 remainingUs()
        {
            NvS64 remaining = (NvS64)(timeoutTime - timer->getTimeUs());
            
            // Rollover check
            if (remaining < 0)
            {
                remaining = 0;
            }

            DP_ASSERT(remaining < ((NvS64)1000000*3600) && "Timeout remaining over an hour");
            
            return remaining;
        }

        bool valid()
        {
            return remainingUs() > 0;
        }
    };
}

#endif //INCLUDED_DP_TIMEOUT_H
