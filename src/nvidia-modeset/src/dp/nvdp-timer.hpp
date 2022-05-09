/*
 * SPDX-FileCopyrightText: Copyright (c) 2011 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVDP_TIMER_HPP__
#define __NVDP_TIMER_HPP__

#include <nvtypes.h>
#include <dp_timer.h>
#include <dp_list.h>

namespace nvkmsDisplayPort
{

class Timer : public DisplayPort::RawTimer
{
    NVDevEvoPtr pDevEvo;
    DisplayPort::List timerList;

    class Callback : public DisplayPort::ListElement {
        DisplayPort::RawTimer::Callback *dpCallback;
        // ref_ptr to the pDevEvo
        nvkms_ref_ptr *ref_ptr;
        nvkms_timer_handle_t *handle;
        NvU64 expireTimeUs;

        static void onTimerFired(void *data, NvU32 dataU32);
        void onTimerFired();

    public:
        // Construct an NVKMS timer callback.  Since exceptions cannot be used
        // in NVKMS code, callers must call Callback::allocFailed() to query
        // whether the constructor succeeded.
        //
        // Scheduling a callback bumps the refcount on the corresponding
        // pDevEvo, so that a device isn't freed until all pending callbacks
        // have fired.
        Callback(DisplayPort::List *pList,
                 NVDevEvoPtr pDevEvo,
                 DisplayPort::RawTimer::Callback *dpCallback,
                 int ms);
        ~Callback();

        // Returns TRUE if the constructor failed.
        bool allocFailed() const;
        // Returns TRUE if the timer is ready to fire.
        bool isExpired(NvU64 timeNowUs) const;
        // Fire the timer if it's ready.
        // NOTE: If the timer fires, this deletes it.
        void fireIfExpired(NvU64 timeNowUs);
    };
public:
    Timer(NVDevEvoPtr pDevEvo);

    virtual void queueCallback(DisplayPort::RawTimer::Callback *cb, int ms);
    virtual NvU64 getTimeUs();
    virtual void sleep(int ms);

    void fireExpiredTimers();
};

}; // namespace nvkmsDisplayPort

struct _nv_dplibtimer : public DisplayPort::Object {
    nvkmsDisplayPort::Timer rawTimer;
    DisplayPort::Timer timer;

    _nv_dplibtimer(NVDevEvoPtr pDevEvo)
      : rawTimer(pDevEvo), timer(&rawTimer)
    {
    }
};

#endif // __NVDP_TIMER_HPP__
