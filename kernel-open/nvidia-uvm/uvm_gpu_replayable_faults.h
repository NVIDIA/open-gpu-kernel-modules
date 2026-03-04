/*******************************************************************************
    Copyright (c) 2015-2024 NVIDIA Corporation

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

#ifndef __UVM_GPU_PAGE_FAULT_H__
#define __UVM_GPU_PAGE_FAULT_H__

#include "nvtypes.h"
#include "uvm_types.h"
#include "uvm_hal_types.h"
#include "uvm_tracker.h"

typedef enum
{
    // Issue a fault replay after all faults for a block within a batch have been serviced
    UVM_PERF_FAULT_REPLAY_POLICY_BLOCK = 0,

    // Issue a fault replay after each fault batch has been serviced
    UVM_PERF_FAULT_REPLAY_POLICY_BATCH,

    // Like UVM_PERF_FAULT_REPLAY_POLICY_BATCH but only one batch of faults is serviced. The fault buffer is flushed
    // before issuing the replay. The potential benefit is that we can resume execution of some SMs earlier, if SMs
    // are faulting on different sets of pages.
    UVM_PERF_FAULT_REPLAY_POLICY_BATCH_FLUSH,

    // Issue a fault replay after all faults in the buffer have been serviced
    UVM_PERF_FAULT_REPLAY_POLICY_ONCE,

    // TODO: Bug 1768226: Implement uTLB-aware fault replay policy

    UVM_PERF_FAULT_REPLAY_POLICY_MAX,
} uvm_perf_fault_replay_policy_t;

const char *uvm_perf_fault_replay_policy_string(uvm_perf_fault_replay_policy_t fault_replay);

NV_STATUS uvm_parent_gpu_fault_buffer_init(uvm_parent_gpu_t *parent_gpu);
void uvm_parent_gpu_fault_buffer_deinit(uvm_parent_gpu_t *parent_gpu);

void uvm_parent_gpu_fault_buffer_resume(uvm_parent_gpu_t *parent_gpu);

bool uvm_parent_gpu_replayable_faults_pending(uvm_parent_gpu_t *parent_gpu);

// Clear valid bit for all remaining unserviced faults in the buffer, set GET to
// PUT, and push a fault replay of type UVM_FAULT_REPLAY_TYPE_START. It does not
// wait for the replay to complete before returning. The pushed replay is added
// to the GPU's replay_tracker.
//
// LOCKING: Takes gpu->isr_lock
NV_STATUS uvm_gpu_fault_buffer_flush(uvm_gpu_t *gpu);

// Enable/disable HW support for prefetch-initiated faults
void uvm_parent_gpu_enable_prefetch_faults(uvm_parent_gpu_t *parent_gpu);
void uvm_parent_gpu_disable_prefetch_faults(uvm_parent_gpu_t *parent_gpu);

// Service pending replayable faults on the given GPU. This function must be
// only called from the ISR bottom half
void uvm_parent_gpu_service_replayable_faults(uvm_parent_gpu_t *parent_gpu);
#endif // __UVM_GPU_PAGE_FAULT_H__
