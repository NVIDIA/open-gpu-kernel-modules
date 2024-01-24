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
* Module: dp_timer.h                                                        *
*    Local timer interface                                                  *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_TIMER_H
#define INCLUDED_DP_TIMER_H

#include "dp_list.h"

namespace DisplayPort
{
    //
    //  RawTimer
    //      This API is expected to be implemented by the
    //      library client.
    //
    class RawTimer : virtual public Object
    {
    public:
        struct Callback : virtual public Object
        {
            virtual void expired() = 0;
        };
        virtual void queueCallback(Callback * callback, int milliseconds) = 0;
        virtual NvU64 getTimeUs() = 0;
        virtual void sleep(int milliseconds) = 0;
    };


    //
    //  Timer
    //
    class Timer : public RawTimer::Callback
    {
    public:
        struct TimerCallback
        {
            virtual void expired(const  void * context) = 0;
        };

    private:
        RawTimer * raw;
        List       pending;
        struct PendingCallback : ListElement
        {
            TimerCallback *    target;
            const void *       context;
            NvU64              timestamp; // in usec
            bool               executeInSleep;

        };

        virtual void expired();
        unsigned fire(bool fromSleep);

        void _pump(unsigned milliseconds, bool fromSleep);
    public:
        Timer(RawTimer * raw) : raw(raw) {}
        virtual ~Timer() {}

        //
        //  Queue a timer callback.
        //      Unless the dont-execute-in-sleep flag is
        //
        void queueCallback(Timer::TimerCallback * target, const  void * context, unsigned milliseconds, bool executeInSleep = true);
        NvU64 getTimeUs();
        void sleep(unsigned milliseconds);
        void cancelCallbacks(Timer::TimerCallback * to);

        void cancelCallback(Timer::TimerCallback * to, const void * context);
        void queueCallbackInOrder(Timer::TimerCallback * target, const  void * context, unsigned milliseconds, bool executeInSleep);
        void cancelCallbacksWithoutContext(const  void * context);
        void cancelAllCallbacks();
        bool checkCallbacksOfSameContext(const void * context);
    };
}

#endif //INCLUDED_DP_TIMER_H
