/*******************************************************************************
    Copyright (c) 2016-2023 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#include "uvm_perf_events.h"
#include "uvm_va_space.h"
#include "uvm_test.h"

// Global variable used to check that callbacks are correctly executed
static int test_data;

static void callback_inc_1(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data)
{
    ++test_data;
}

static void callback_inc_2(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data)
{
    test_data += 2;
}

static NV_STATUS test_events(uvm_va_space_t *va_space)
{
    NV_STATUS status;
    uvm_perf_event_data_t event_data;

    test_data = 0;
    memset(&event_data, 0, sizeof(event_data));

    // Use CPU id to avoid triggering the GPU stats update code
    event_data.fault.proc_id = UVM_ID_CPU;

    // Register a callback for page fault
    status = uvm_perf_register_event_callback(&va_space->perf_events, UVM_PERF_EVENT_FAULT, callback_inc_1);
    TEST_CHECK_GOTO(status == NV_OK, done);

    // Register a callback for page fault
    status = uvm_perf_register_event_callback(&va_space->perf_events, UVM_PERF_EVENT_FAULT, callback_inc_2);
    TEST_CHECK_GOTO(status == NV_OK, done);

    // va_space read lock is required for page fault event notification
    uvm_va_space_down_read(va_space);

    // Notify (fake) page fault. The two registered callbacks for this event
    // increment the value of test_value
    uvm_perf_event_notify(&va_space->perf_events, UVM_PERF_EVENT_FAULT, &event_data);

    uvm_va_space_up_read(va_space);

    // test_data was initialized to zero. It should have been incremented by 1
    // and 2, respectively in the callbacks
    TEST_CHECK_GOTO(test_data == 3, done);

done:
    // Unregister all callbacks
    uvm_perf_unregister_event_callback(&va_space->perf_events, UVM_PERF_EVENT_FAULT, callback_inc_1);
    uvm_perf_unregister_event_callback(&va_space->perf_events, UVM_PERF_EVENT_FAULT, callback_inc_2);

    return status;
}

NV_STATUS uvm_test_perf_events_sanity(UVM_TEST_PERF_EVENTS_SANITY_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_va_space_t *va_space;

    va_space = uvm_va_space_get(filp);

    status = test_events(va_space);
    if (status != NV_OK)
        goto done;

done:
    return status;
}
