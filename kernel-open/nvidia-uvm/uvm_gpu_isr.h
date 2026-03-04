/*******************************************************************************
    Copyright (c) 2016-2025 NVIDIA Corporation

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

#ifndef __UVM_GPU_ISR_H__
#define __UVM_GPU_ISR_H__

#include "nv-kthread-q.h"
#include "uvm_common.h"
#include "uvm_lock.h"
#include "uvm_forward_decl.h"

// ISR handling state for a specific interrupt type
typedef struct
{
    // Protects against changes to the GPU data structures used by the handling
    // routines of this interrupt type.
    uvm_semaphore_t service_lock;

    // Bottom-half to be executed for this interrupt. There is one bottom-half
    // per interrupt type.
    nv_kthread_q_item_t bottom_half_q_item;

    union
    {
        // Used for replayable and non-replayable faults.
        struct
        {
            // This is set to true during add_gpu(), if the GPU supports the
            // interrupt. It is set back to false during remove_gpu().
            // interrupts_lock must be held in order to write this variable.
            bool handling;

            // Variable set in uvm_gpu_disable_isr() during remove_gpu() to
            // indicate if this type of interrupt was being handled by the
            // driver.
            bool was_handling;
        };

        // Used for access counters.
        //
        // If the GPU does not support access counters, the ref count is always
        // zero. Otherwise, the refcount is incremented when the GPU is
        // registered in a VA space for the first time, and decremented when
        // unregistered or the VA space is destroyed.
        //
        // Locking: protected by the GPU access counters ISR lock. Naked
        // accesses are allowed during GPU addition and removal.
        NvU64 handling_ref_count;
    };

    struct
    {
        // Number of the bottom-half invocations for this interrupt on a GPU
        // over its lifetime.
        NvU64 bottom_half_count;

        // A bitmask of the CPUs on which the bottom half has executed. The
        // corresponding bit gets set once the bottom half executes on that
        // CPU.
        // This mask is useful when testing that the bottom half is getting
        // executed on the correct set of CPUs.
        struct cpumask cpus_used_mask;

        // An array (one per possible CPU), which holds the number of times the
        // bottom half has executed on that CPU.
        NvU64 *cpu_exec_count;
    } stats;

    // This is the number of times the function that disables this type of
    // interrupt has been called without a corresponding call to the function
    // that enables it. If this is > 0, interrupts are disabled. This field is
    // protected by interrupts_lock. This field is only valid for interrupts
    // directly owned by UVM:
    // - replayable_faults
    // - access_counters
    NvU64 disable_intr_ref_count;
} uvm_intr_handler_t;

// State for all ISR handling in UVM
typedef struct
{
    // This is set by uvm_suspend() and uvm_resume() to indicate whether
    // top-half ISR processing is suspended for power management.  Calls from
    // the RM's top-half are to be completed without processing when this
    // flag is set to true.
    bool is_suspended;

    // There is exactly one nv_kthread_q per GPU. It is used for the ISR bottom
    // halves. So N CPUs will be servicing M GPUs, in general. There is one
    // bottom-half per interrupt type.
    nv_kthread_q_t bottom_half_q;

    // Protects the state of interrupts (enabled/disabled) and whether the GPU
    // is currently handling them. Taken in both interrupt and process context.
    uvm_spinlock_irqsave_t interrupts_lock;

    uvm_intr_handler_t replayable_faults;
    uvm_intr_handler_t non_replayable_faults;
    uvm_intr_handler_t *access_counters;

    // Kernel thread used to kill channels on fatal non-replayable faults.
    // This is needed because we cannot call into RM from the bottom-half to
    // avoid deadlocks.
    nv_kthread_q_t kill_channel_q;

    // Number of top-half ISRs called for this GPU over its lifetime.
    NvU64 interrupt_count;
} uvm_isr_info_t;

// Entry point for interrupt handling. This is called from RM's top half
NV_STATUS uvm_isr_top_half_entry(const NvProcessorUuid *gpu_uuid);

// Initialize ISR handling state
NV_STATUS uvm_parent_gpu_init_isr(uvm_parent_gpu_t *parent_gpu);

// Flush any currently scheduled bottom halves. This is called during GPU
// removal.
void uvm_parent_gpu_flush_bottom_halves(uvm_parent_gpu_t *parent_gpu);

// Prevent new bottom halves from being scheduled. This is called during parent
// GPU removal.
void uvm_parent_gpu_disable_isr(uvm_parent_gpu_t *parent_gpu);

// Destroy ISR handling state and return interrupt ownership to RM. This is
// called during parent GPU removal
void uvm_parent_gpu_deinit_isr(uvm_parent_gpu_t *parent_gpu);

// Take parent_gpu->isr.replayable_faults.service_lock from a non-top/bottom
// half thread. This will also disable replayable page fault interrupts (if
// supported by the GPU) because the top half attempts to take this lock, and we
// would cause an interrupt storm if we didn't disable them first.
//
// At least one GPU under the parent must have been previously retained.
void uvm_parent_gpu_replayable_faults_isr_lock(uvm_parent_gpu_t *parent_gpu);

// Unlock parent_gpu->isr.replayable_faults.service_lock. This call may
// re-enable replayable page fault interrupts. Unlike
// uvm_parent_gpu_replayable_faults_isr_lock(), which should only be called from
// non-top/bottom half threads, this can be called by any thread.
void uvm_parent_gpu_replayable_faults_isr_unlock(uvm_parent_gpu_t *parent_gpu);

// Lock/unlock routines for non-replayable faults. These do not need to prevent
// interrupt storms since the GPU fault buffers for non-replayable faults are
// managed by RM. Unlike uvm_parent_gpu_replayable_faults_isr_lock, no GPUs
// under the parent need to have been previously retained.
void uvm_parent_gpu_non_replayable_faults_isr_lock(uvm_parent_gpu_t *parent_gpu);
void uvm_parent_gpu_non_replayable_faults_isr_unlock(uvm_parent_gpu_t *parent_gpu);

// See uvm_parent_gpu_replayable_faults_isr_lock/unlock
void uvm_access_counters_isr_lock(uvm_access_counter_buffer_t *access_counters);
void uvm_access_counters_isr_unlock(uvm_access_counter_buffer_t *access_counters);

// Increments the reference count tracking whether access counter interrupts
// should be disabled. The caller is guaranteed that access counter interrupts
// are disabled upon return. Interrupts might already be disabled prior to
// making this call. Each call is ref-counted, so this must be paired with a
// call to uvm_access_counters_intr_enable().
//
// parent_gpu->isr.interrupts_lock must be held to call this function.
void uvm_access_counters_intr_disable(uvm_access_counter_buffer_t *access_counters);

// Decrements the reference count tracking whether access counter interrupts
// should be disabled. Only once the count reaches 0 are the HW interrupts
// actually enabled, so this call does not guarantee that the interrupts have
// been re-enabled upon return.
//
// uvm_access_counters_intr_disable() must have been called prior to calling
// this function.
//
// NOTE: For pulse-based interrupts, the caller is responsible for re-arming
// the interrupt.
//
// parent_gpu->isr.interrupts_lock must be held to call this function.
void uvm_access_counters_intr_enable(uvm_access_counter_buffer_t *access_counters);

// Return the first valid GPU given the parent GPU or NULL if no MIG instances
// are registered. This should only be called from bottom halves or if the
// g_uvm_global.global_lock is held so that the returned pointer remains valid.
uvm_gpu_t *uvm_parent_gpu_find_first_valid_gpu(uvm_parent_gpu_t *parent_gpu);

#endif // __UVM_GPU_ISR_H__
