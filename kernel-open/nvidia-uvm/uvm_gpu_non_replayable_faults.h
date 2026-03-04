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

void uvm_parent_gpu_service_non_replayable_fault_buffer(uvm_parent_gpu_t *parent_gpu);

NV_STATUS uvm_parent_gpu_fault_buffer_init_non_replayable_faults(uvm_parent_gpu_t *parent_gpu);

void uvm_parent_gpu_fault_buffer_deinit_non_replayable_faults(uvm_parent_gpu_t *parent_gpu);

#endif // __UVM_GPU_NON_REPLAYABLE_FAULTS_H__
