/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_timer.cpp                                                      *
*                                                                           *
\***************************************************************************/
#include "dp_internal.h"
#include "dp_timer.h"
using namespace DisplayPort;

void Timer::expired()
{
    fire(false);
}

//  Take care, this function is re-entrant.
//   Consider that sleep() is effectively a call to fire().
//   Clients may sleep in response to a timer callback.
unsigned Timer::fire(bool fromSleep) // returns min time to next item to be fired
{
    restart:

    NvU64 now = getTimeUs();
    NvU64 nearest = (NvU64)-1;
    for (PendingCallback * i = (PendingCallback*)pending.begin(); i!=pending.end(); )
    {
        if (fromSleep && !i->executeInSleep) {
            i = (PendingCallback*)i->next;
            continue;
        }

        if (now >= i->timestamp)
        {
            const void * context = i->context;
            TimerCallback * target = i->target;
            delete i;
            if (target)
                target->expired(context);           // Take care, the client may have made
                                                    // a recursive call to fire in here.
                                                    // Easy solution: Restart at front of list.
                                                    //    current time may have also changed
                                                    //    drastically from a nested sleep
            goto restart;
        }
        else
        {
            if (i->timestamp < nearest)
                nearest = i->timestamp;
            i = (PendingCallback*)i->next;
        }
    }
    unsigned minleft = (unsigned)((nearest - now + 999)/ 1000);
    return minleft;
}

void Timer::_pump(unsigned milliseconds, bool fromSleep) 
{
    do
    {
        unsigned amt = fire(fromSleep);
        if (amt >= milliseconds) {
            raw->sleep(milliseconds);
            return;
        }
        raw->sleep(amt);
        milliseconds-=amt;
    } while(milliseconds);
}

//
//  Queue a timer callback.
//      Unless the dont-execute-in-sleep flag is set
//
void Timer::queueCallback(Timer::TimerCallback * target, const  void * context, unsigned milliseconds, bool executeInSleep) 
{
    NvU64 now = getTimeUs();
    PendingCallback * callback = new PendingCallback();
    if (callback == NULL)
    {
        DP_LOG(("DP> %s: Failed to allocate callback",
                    __FUNCTION__));
        return;
    }
    callback->target = target;
    callback->context = context;
    callback->timestamp = now + milliseconds * 1000;
    callback->executeInSleep = executeInSleep;
    pending.insertBack(callback);
    raw->queueCallback(this, milliseconds);
}

NvU64 Timer::getTimeUs() 
{
    return raw->getTimeUs();
}

// Sleep a number of milliseconds.
//   timer callbacks will be serviced!
void Timer::sleep(unsigned milliseconds)
{
    _pump(milliseconds, true);
}

void Timer::cancelCallbacks(Timer::TimerCallback * to) 
{
    if (!to)
        return;
    for (PendingCallback * i = (PendingCallback*)pending.begin(); i!=pending.end(); i = (PendingCallback *)i->next)
        if (i->target == to)
            i->target = 0;
}

void Timer::cancelCallback(Timer::TimerCallback * to, const void * context) 
{
    if (!to)
        return;
    for (PendingCallback * i = (PendingCallback *)pending.begin(); i!=pending.end(); i = (PendingCallback*)i->next)
        if (i->target == to && i->context == context) 
            i->target = 0;
}

// Queue callbacks in order.
void Timer::queueCallbackInOrder(Timer::TimerCallback * target, const  void * context, unsigned milliseconds, bool executeInSleep) 
{
    NvU64 now = getTimeUs();
    PendingCallback * callback = new PendingCallback();
    callback->target = target;
    callback->context = context;
    callback->timestamp = now + milliseconds * 1000;
    callback->executeInSleep = executeInSleep;

    //Figure out where to insert the current callback
    Timer::PendingCallback* i;

    for (i = (PendingCallback*)pending.begin(); i != pending.end();)
    {
        // only for the given context.
        if(i->context == context)
        {
            if(i->timestamp > callback->timestamp)
                break;
        }
        i = (PendingCallback*) i->next;
    }
    if (i == pending.end())
    {
        pending.insertBack(callback);
    }
    else 
    {
        pending.insertBefore(i, callback);
    }

    raw->queueCallback(this, milliseconds);
}

void Timer::cancelAllCallbacks()
{
    for (PendingCallback * i = (PendingCallback*)pending.begin(); i!=pending.end(); i = (PendingCallback *)i->next)
        i->target = 0;
}

void Timer::cancelCallbacksWithoutContext(const  void * context)
{
    for (PendingCallback * i = (PendingCallback*)pending.begin(); i!=pending.end(); i = (PendingCallback *)i->next)
        if(i->context != context)
            i->target = 0;
}

bool Timer::checkCallbacksOfSameContext(const void * context)
{
    for (PendingCallback * i = (PendingCallback*)pending.begin(); i!=pending.end(); i = (PendingCallback *)i->next)
        if(i->context == context)
            return true;

    return false;
}
