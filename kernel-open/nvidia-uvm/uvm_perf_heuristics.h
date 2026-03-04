/*******************************************************************************
    Copyright (c) 2016-2019 NVIDIA Corporation

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

#ifndef __UVM_PERF_HEURISTICS_H__
#define __UVM_PERF_HEURISTICS_H__

#include "nvtypes.h"
#include "uvm_forward_decl.h"

// Global initialization/cleanup functions
NV_STATUS uvm_perf_heuristics_init(void);
void uvm_perf_heuristics_exit(void);

// Per-GPU initialization/cleanup functions. They are called from add_gpu,
// remove_gpu.
//
// Locking: the global lock must be held when calling this function
NV_STATUS uvm_perf_heuristics_add_gpu(uvm_gpu_t *gpu);
void uvm_perf_heuristics_remove_gpu(uvm_gpu_t *gpu);

// Per-VA space initialization/cleanup functions

// This function is called during VA space creation
NV_STATUS uvm_perf_heuristics_load(uvm_va_space_t *va_space);

// This function is called when a GPU is registered on a VA space
void uvm_perf_heuristics_register_gpu(uvm_va_space_t *va_space, uvm_gpu_t *gpu);

// The following two functions are called during VA space teardown.
// uvm_perf_heuristics_stop is called first with no lock taken on the VA space.
// This is required because the performance heuristics modues may have scheduled
// delayed work that needs to take the VA space lock. Therefore,
// uvm_perf_heuristics_stop needs to cancel/flush any pending work.
void uvm_perf_heuristics_stop(uvm_va_space_t *va_space);
void uvm_perf_heuristics_unload(uvm_va_space_t *va_space);

#endif
