/*******************************************************************************
    Copyright (c) 2017-2024 NVIDIA Corporation

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

#ifndef __UVM_GPU_ACCESS_COUNTERS_H__
#define __UVM_GPU_ACCESS_COUNTERS_H__

#include "uvm_common.h"
#include "uvm_forward_decl.h"
#include "uvm_test_ioctl.h"

NV_STATUS uvm_parent_gpu_init_access_counters(uvm_parent_gpu_t *parent_gpu);
void uvm_parent_gpu_deinit_access_counters(uvm_parent_gpu_t *parent_gpu);
bool uvm_parent_gpu_access_counters_pending(uvm_parent_gpu_t *parent_gpu);

void uvm_parent_gpu_service_access_counters(uvm_parent_gpu_t *parent_gpu);

void uvm_parent_gpu_access_counter_buffer_flush(uvm_parent_gpu_t *parent_gpu);

// Ignore or unignore access counters notifications. Ignoring means that the
// bottom half is a no-op which just leaves notifications in the HW buffer
// without being serviced and without inspecting any SW state.
//
// To avoid interrupt storms, access counter interrupts will be disabled while
// ignored. Access counter bottom halves may still be scheduled in the top half
// when other interrupts arrive and the top half sees that there are also
// pending access counter notifications.
//
// When uningoring, the interrupt conditions will be re-evaluated to trigger
// processing of buffered notifications, if any exist.
void uvm_parent_gpu_access_counters_set_ignore(uvm_parent_gpu_t *parent_gpu, bool do_ignore);

// Return whether the VA space has access counter migrations enabled. The
// caller must ensure that the VA space cannot go away.
bool uvm_va_space_has_access_counter_migrations(uvm_va_space_t *va_space);

// Global perf initialization/cleanup functions
NV_STATUS uvm_perf_access_counters_init(void);
void uvm_perf_access_counters_exit(void);

// VA space Initialization/cleanup functions. See comments in
// uvm_perf_heuristics.h
NV_STATUS uvm_perf_access_counters_load(uvm_va_space_t *va_space);
void uvm_perf_access_counters_unload(uvm_va_space_t *va_space);

// Check whether access counters should be enabled when the given GPU is
// registered on any VA space.
bool uvm_parent_gpu_access_counters_required(const uvm_parent_gpu_t *parent_gpu);

// Functions used to enable/disable access counters on a GPU in the given VA
// space.
//
// A per-GPU reference counter tracks the number of VA spaces in which access
// counters are currently enabled. The hardware notifications and interrupts on
// the GPU are enabled the first time any VA space invokes
// uvm_gpu_access_counters_enable, and disabled when the last VA space invokes
// uvm_parent_gpu_access_counters_disable().
//
// Locking: the VA space lock must not be held by the caller since these
// functions may take the access counters ISR lock.
NV_STATUS uvm_gpu_access_counters_enable(uvm_gpu_t *gpu, uvm_va_space_t *va_space);
void uvm_parent_gpu_access_counters_disable(uvm_parent_gpu_t *parent_gpu, uvm_va_space_t *va_space);

NV_STATUS uvm_test_access_counters_enabled_by_default(UVM_TEST_ACCESS_COUNTERS_ENABLED_BY_DEFAULT_PARAMS *params,
                                                      struct file *filp);
NV_STATUS uvm_test_reconfigure_access_counters(UVM_TEST_RECONFIGURE_ACCESS_COUNTERS_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_reset_access_counters(UVM_TEST_RESET_ACCESS_COUNTERS_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_set_ignore_access_counters(UVM_TEST_SET_IGNORE_ACCESS_COUNTERS_PARAMS *params, struct file *filp);

#endif // __UVM_GPU_ACCESS_COUNTERS_H__
