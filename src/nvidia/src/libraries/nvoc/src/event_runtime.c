/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * This file is part of the NVOC runtime.
 */

#define NVOC_EVENT_H_PRIVATE_ACCESS_ALLOWED

#include "nvport/nvport.h"
#include "nvoc/prelude.h"
#include "nvoc/event.h"
#include "nvoc/event_group.h"
#include "nvoc/event_bus.h"
#include "utils/nvassert.h"

NV_STATUS eventConstruct_IMPL(Event *pEvent, EventBus *pEventBus)
{
    //
    // Only pEvent is checked for NULL here. pSource and pEventBus are
    // intentionally allowed to be NULL: NVOC test fixtures construct
    // Event-derived classes purely to exercise class-hierarchy / RTTI
    // machinery (e.g. EventTestObject in eventTest.h passes NULL/NULL/0)
    // with no intent to ever publish. The publish path catches a NULL bus
    // when the event would actually be dispatched.
    //
    NV_ASSERT_OR_RETURN(pEvent != NULL, NV_ERR_INVALID_POINTER);

    // Store the source object, event bus, and creation flags
    pEvent->pEventBus = pEventBus;

    // Group membership is established later, when the event is added to a group.
    // Initialize explicitly rather than relying on the allocator zeroing memory.
    pEvent->pEventGroup = NULL;

    pEvent->bPaused = NV_FALSE;
    pEvent->bDispatched = NV_FALSE;

    return NV_OK;
}

void eventPause_IMPL(Event *pEvent)
{
    NV_ASSERT_OR_RETURN_VOID(pEvent != NULL);
    NV_ASSERT_OR_RETURN_VOID(pEvent->pEventGroup != NULL);

    // Don't pause if already paused
    if (pEvent->bPaused)
    {
        return;
    }

    NV_ASSERT_OR_RETURN_VOID(!eventbusIsBlockingMode(pEvent->pEventBus));

    // Set our own paused state
    pEvent->bPaused = NV_TRUE;

    // Notify the event group that this event has been paused
    eventgroupEventPaused(pEvent->pEventGroup);
}

void eventResume_IMPL(Event *pEvent)
{
    NV_ASSERT_OR_RETURN_VOID(pEvent != NULL);
    NV_ASSERT_OR_RETURN_VOID(pEvent->pEventGroup != NULL);

    // Don't resume if not paused
    if (!pEvent->bPaused)
    {
        return;
    }

    // Clear our own paused state
    pEvent->bPaused = NV_FALSE;

    // Notify the event group that this event has been resumed
    eventgroupEventResumed(pEvent->pEventGroup);
}

NvBool eventIsPaused_IMPL(Event *pEvent)
{
    NV_ASSERT_OR_RETURN(pEvent != NULL, NV_FALSE);
    return pEvent->bPaused;
}
