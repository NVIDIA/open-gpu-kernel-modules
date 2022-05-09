/*******************************************************************************
    Copyright (c) 2015-2019 NVIDIA Corporation

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
#include "uvm_api.h"
#include "uvm_thread_context.h"
#include "uvm_va_space.h"
#include "uvm_kvmalloc.h"
#include "uvm_test.h"


static NvU64 timed_udelay(NvU64 delay_us)
{
    NvU64 start = NV_GETTIME();

    udelay(delay_us);

    return NV_GETTIME() - start;
}

static NvU64 timed_udelay_entry(NvU64 delay_us)
{
    UVM_ENTRY_RET(timed_udelay(delay_us));
}

// Measure the overhead of wrapping entry functions i.e. the overhead of
// adding and removing a thread context.
NV_STATUS uvm_test_thread_context_perf(UVM_TEST_THREAD_CONTEXT_PERF_PARAMS *params, struct file *filp)
{
    NvU64 start;
    NvU32 i;
    uvm_thread_context_t *thread_context;
    uvm_thread_context_wrapper_t thread_context_wrapper_backup;
    const NvU64 delay_us = params->delay_us;
    NvU64 total_delay_ns = 0;

    if (params->iterations == 0)
        return NV_ERR_INVALID_ARGUMENT;

    TEST_CHECK_RET(uvm_thread_context_present());
    TEST_CHECK_RET(uvm_thread_context_wrapper_is_used());

    thread_context = uvm_thread_context();
    uvm_thread_context_save(&thread_context_wrapper_backup.context);

    // Remove the current thread context, forcing UVM_ENTRY_VOID to register a
    // new thread context instead of reusing the existing one.
    uvm_thread_context_remove(thread_context);
    TEST_CHECK_RET(!uvm_thread_context_present());

    start = NV_GETTIME();

    for (i = 0; i < params->iterations; i++) {
        if (delay_us > 0)
            total_delay_ns += timed_udelay_entry(delay_us);
        else
            UVM_ENTRY_VOID();
    }

    // Report average iteration time
    params->ns = ((NV_GETTIME() - start - total_delay_ns) / params->iterations);

    TEST_CHECK_RET(uvm_thread_context_add(thread_context));
    TEST_CHECK_RET(uvm_thread_context_present());

    uvm_thread_context_restore(&thread_context_wrapper_backup.context);

    return NV_OK;
}

static uvm_thread_context_t *inner_thread_context(void)
{
    UVM_ENTRY_RET(uvm_thread_context());
}

NV_STATUS uvm_test_thread_context_sanity(UVM_TEST_THREAD_CONTEXT_SANITY_PARAMS *params, struct file *filp)
{
    NvU32 i;
    uvm_thread_context_t *thread_context, *nested_thread_context;
    uvm_thread_context_wrapper_t thread_context_wrapper_backup;

    if (params->iterations == 0)
        return NV_ERR_INVALID_ARGUMENT;

    TEST_CHECK_RET(uvm_thread_context_present());
    TEST_CHECK_RET(uvm_thread_context_wrapper_is_used());

    thread_context = uvm_thread_context();

    // Nested entry points do not add new thread contexts. Instead, they reuse
    // the top-most thread context.
    nested_thread_context = inner_thread_context();
    TEST_CHECK_RET(nested_thread_context == thread_context);

    uvm_thread_context_save(&thread_context_wrapper_backup.context);

    uvm_thread_context_remove(thread_context);
    TEST_CHECK_RET(!uvm_thread_context_present());

    // The removal of a thread context is expected to be the last operation on
    // every UVM module entry point. This test breaks that expectation, so
    // proceed carefully and avoid invoking any function that assumes the
    // presence of the thread context.

    // The loop is used to detect concurrency errors when multiple threads add
    // or remove their contexts to/from global data structures.
    for (i = 0; i < params->iterations; i++) {
        UVM_ENTRY_VOID();
        TEST_CHECK_RET(!uvm_thread_context_present());
    }

    // The restored thread context may be added to an array or tree location
    // different from the original one.
    TEST_CHECK_RET(uvm_thread_context_add(thread_context));

    uvm_thread_context_restore(&thread_context_wrapper_backup.context);

    TEST_CHECK_RET(uvm_thread_context_present());

    return NV_OK;
}
