/*******************************************************************************
    Copyright (c) 2025-2026 NVIDIA Corporation

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

#ifndef __UVM_PERF_DYNAMIC_THRESHOLD_H__
#define __UVM_PERF_DYNAMIC_THRESHOLD_H__

#include "uvm_linux.h"
#include "uvm_forward_decl.h"

// Validate the module parameters.
// Return NV_ERR_INVALID_ARGUMENT if enabled with an invalid target rate band.
NV_STATUS uvm_perf_dynamic_threshold_init(void);

// Create the threshold adjustment state for a GPU.
// Return NV_OK if disabled or ineligible, or NV_ERR_NO_MEMORY on allocation
// failure.
NV_STATUS uvm_perf_dynamic_threshold_add_gpu(uvm_gpu_t *gpu);

// Stop and destroy a GPU's threshold adjustment state.
void uvm_perf_dynamic_threshold_remove_gpu(uvm_gpu_t *gpu);

// Subscribe to the VA space's migration events. The callbacks are destroyed
// with the VA space's perf events. Return NV_ERR_NO_MEMORY if the callback
// cannot be registered.
NV_STATUS uvm_perf_dynamic_threshold_load(uvm_va_space_t *va_space);

#endif
