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

// This file implements the timer callback mechanism for the DisplayPort
// library.

#include "nvkms-types.h"

#include "dp/nvdp-timer.h"
#include "nvdp-timer.hpp"

namespace nvkmsDisplayPort {
    Timer::Callback::Callback(DisplayPort::List *pList,
                              NVDevEvoPtr pDevEvo,
                              DisplayPort::RawTimer::Callback *dpCallback,
                              int ms)
        : dpCallback(dpCallback),
          ref_ptr(pDevEvo->ref_ptr),
          handle(nvkms_alloc_timer(onTimerFired, this, 0, ms * 1000)),
          expireTimeUs(nvkms_get_usec() + ms * 1000)
    {
        if (!allocFailed()) {
            pList->insertFront(this);
            nvkms_inc_ref(ref_ptr);
        }
    }

    Timer::Callback::~Callback()
    {
        nvkms_free_timer(handle);
    }

    bool Timer::Callback::allocFailed() const
    {
        return handle == NULL;
    }

    bool Timer::Callback::isExpired(NvU64 timeNowUs) const
    {
        return timeNowUs >= expireTimeUs;
    }

    void Timer::Callback::onTimerFired(void *data, NvU32 dataU32)
    {
        Timer::Callback *cb = static_cast<Timer::Callback*>(data);
        cb->onTimerFired();
    }

    void Timer::Callback::onTimerFired()
    {
        if (nvkms_dec_ref(ref_ptr)) {
            dpCallback->expired();
        }
        delete this;
    }

    void Timer::Callback::fireIfExpired(NvU64 timeNowUs)
    {
        if (isExpired(timeNowUs)) {
            onTimerFired();
        }
    }

    Timer::Timer(NVDevEvoPtr pDevEvo)
        : pDevEvo(pDevEvo)
    {
    }

    void Timer::queueCallback(DisplayPort::RawTimer::Callback *dpCallback, int ms)
    {
        Callback *cb = new Callback(&timerList, pDevEvo, dpCallback, ms);
        nvAssert(cb && !cb->allocFailed());
        if (!cb || cb->allocFailed()) {
            delete cb;
            return;
        }
    }

    NvU64 Timer::getTimeUs()
    {
        return nvkms_get_usec();
    }

    void Timer::sleep(int ms)
    {
        nvkms_usleep(ms * 1000);
    }

    void Timer::fireExpiredTimers()
    {
        const NvU64 timeNowUs = getTimeUs();
        DisplayPort::ListElement *pElem = timerList.begin();
        DisplayPort::ListElement *pNext;

        while (pElem != timerList.end()) {
            Callback *cb = static_cast<Callback*>(pElem);
            pNext = pElem->next;

            cb->fireIfExpired(timeNowUs);

            pElem = pNext;
        }
    }

}; // namespace nvkmsDisplayPort

NVDPLibTimerPtr nvDPAllocTimer(NVDevEvoPtr pDevEvo)
{
    NVDPLibTimerPtr pTimer = new _nv_dplibtimer(pDevEvo);
    return pTimer;
}

void nvDPFreeTimer(NVDPLibTimerPtr pTimer)
{
    delete pTimer;
}

void nvDPFireExpiredTimers(NVDevEvoPtr pDevEvo)
{
    pDevEvo->dpTimer->rawTimer.fireExpiredTimers();
}
