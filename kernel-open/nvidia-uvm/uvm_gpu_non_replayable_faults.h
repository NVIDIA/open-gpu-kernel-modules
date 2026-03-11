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

#ifndef __UVM_GPU_NON_REPLAYABLE_FAULTS_H__
#define __UVM_GPU_NON_REPLAYABLE_FAULTS_H__

#include <nvstatus.h>
#include "uvm_forward_decl.h"

bool uvm_parent_gpu_non_replayable_faults_pending(uvm_parent_gpu_t *parent_gpu);

// Flush the non-replayable shadow buffer (see the comment at the top of
// uvm_gpu_non_replayable_faults.c for an explanation of the shadow buffer).
// All faults in the shadow buffer when this function is called will have been
// removed and serviced upon return.
//
// For faults that are flushed, it is not guaranteed that the HW channel faulted
// state will have been cleared on return, nor that fatal faults have been
// serviced by RM.
//
// Faults added to the shadow buffer during this call are not guaranteed to be
// flushed.
//
// LOCKING: This waits for faults to be serviced, so it must not be called while
// holding the ISR or VA space locks.
void uvm_parent_gpu_non_replayable_buffer_flush(uvm_parent_gpu_t *parent_gpu);

void uvm_parent_gpu_service_non_replayable_fault_buffer(uvm_parent_gpu_t *parent_gpu);

NV_STATUS uvm_parent_gpu_fault_buffer_init_non_replayable_faults(uvm_parent_gpu_t *parent_gpu);

void uvm_parent_gpu_fault_buffer_deinit_non_replayable_faults(uvm_parent_gpu_t *parent_gpu);

NV_STATUS uvm_test_set_non_replayable_delay(UVM_TEST_SET_NON_REPLAYABLE_DELAY_PARAMS *params, struct file *filp);

#endif // __UVM_GPU_NON_REPLAYABLE_FAULTS_H__
