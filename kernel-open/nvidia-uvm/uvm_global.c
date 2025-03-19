/*******************************************************************************
    Copyright (c) 2015-2025 NVIDIA Corporation

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
#include "uvm_ats.h"
#include "uvm_global.h"
#include "uvm_gpu_replayable_faults.h"
#include "uvm_mem.h"
#include "uvm_perf_events.h"
#include "uvm_processors.h"
#include "uvm_procfs.h"
#include "uvm_thread_context.h"
#include "uvm_va_range.h"
#include "uvm_kvmalloc.h"
#include "uvm_mmu.h"
#include "uvm_perf_heuristics.h"
#include "uvm_pmm_sysmem.h"
#include "uvm_pmm_gpu.h"
#include "uvm_migrate.h"
#include "uvm_gpu_access_counters.h"
#include "uvm_va_space_mm.h"
#include "nv_uvm_interface.h"

uvm_global_t g_uvm_global;
static struct UvmOpsUvmEvents g_exported_uvm_ops;
static bool g_ops_registered = false;

static NV_STATUS uvm_register_callbacks(void)
{
    NV_STATUS status = NV_OK;

    g_exported_uvm_ops.suspend = uvm_suspend_entry;
    g_exported_uvm_ops.resume = uvm_resume_entry;
    g_exported_uvm_ops.startDevice = NULL;
    g_exported_uvm_ops.stopDevice  = NULL;
    g_exported_uvm_ops.isrTopHalf  = uvm_isr_top_half_entry;

    g_exported_uvm_ops.drainP2P = uvm_suspend_and_drainP2P_entry;
    g_exported_uvm_ops.resumeP2P = uvm_resumeP2P_entry;

    // Register the UVM callbacks with the main GPU driver:
    status = uvm_rm_locked_call(nvUvmInterfaceRegisterUvmCallbacks(&g_exported_uvm_ops));
    if (status != NV_OK)
        return status;

    g_ops_registered = true;
    return NV_OK;
}

// Calling this function more than once is harmless:
static void uvm_unregister_callbacks(void)
{
    if (g_ops_registered) {
        uvm_rm_locked_call_void(nvUvmInterfaceDeRegisterUvmOps());
        g_ops_registered = false;
    }
}

NV_STATUS uvm_global_init(void)
{
    NV_STATUS status;
    UvmPlatformInfo platform_info;

    // Initialization of thread contexts happened already, during registration
    // (addition) of the thread context associated with the UVM module entry
    // point that is calling this function.
    UVM_ASSERT(uvm_thread_context_global_initialized());

    uvm_mutex_init(&g_uvm_global.global_lock, UVM_LOCK_ORDER_GLOBAL);
    uvm_init_rwsem(&g_uvm_global.pm.lock, UVM_LOCK_ORDER_GLOBAL_PM);
    uvm_spin_lock_irqsave_init(&g_uvm_global.gpu_table_lock, UVM_LOCK_ORDER_LEAF);
    uvm_mutex_init(&g_uvm_global.va_spaces.lock, UVM_LOCK_ORDER_VA_SPACES_LIST);
    INIT_LIST_HEAD(&g_uvm_global.va_spaces.list);
    uvm_mutex_init(&g_uvm_global.devmem_ranges.lock, UVM_LOCK_ORDER_LEAF);
    INIT_LIST_HEAD(&g_uvm_global.devmem_ranges.list);

    status = uvm_kvmalloc_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_kvmalloc_init() failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = errno_to_nv_status(nv_kthread_q_init(&g_uvm_global.global_q, "UVM global queue"));
    if (status != NV_OK) {
        UVM_DBG_PRINT("nv_kthread_q_init() failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = uvm_procfs_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_procfs_init() failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = uvm_rm_locked_call(nvUvmInterfaceSessionCreate(&g_uvm_global.rm_session_handle, &platform_info));
    if (status != NV_OK) {
        UVM_ERR_PRINT("nvUvmInterfaceSessionCreate() failed: %s\n", nvstatusToString(status));
        return status;
    }

    uvm_ats_init(&platform_info);
    g_uvm_global.num_simulated_devices = 0;
    g_uvm_global.conf_computing_enabled = platform_info.confComputingEnabled;

    status = uvm_processor_mask_cache_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_processor_mask_cache_init() failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = uvm_gpu_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_gpu_init() failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = uvm_pmm_sysmem_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_pmm_sysmem_init() failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = uvm_mmu_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_mmu_init() failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = uvm_mem_global_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_mem_gloal_init() failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = uvm_va_policy_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_va_policy_init() failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = uvm_va_range_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_va_range_init() failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = uvm_range_group_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_range_group_init() failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = uvm_migrate_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_migrate_init() failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = uvm_perf_events_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_perf_events_init() failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = uvm_perf_heuristics_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_perf_heuristics_init() failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = uvm_service_block_context_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_service_block_context_init failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = uvm_access_counters_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_access_counters_init failed: %s\n", nvstatusToString(status));
        goto error;
    }

    // This sets up the ISR (interrupt service routine), by hooking into RM's
    // top-half ISR callback. As soon as this call completes, GPU interrupts
    // will start arriving, so it's important to be prepared to receive
    // interrupts before this point.
    status = uvm_register_callbacks();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_register_callbacks failed: %s\n", nvstatusToString(status));
        goto error;
    }

    status = errno_to_nv_status(nv_kthread_q_init(&g_uvm_global.deferred_release_q, "UVM deferred release queue"));
    if (status != NV_OK) {
        UVM_DBG_PRINT("nv_kthread_q_init() failed: %s\n", nvstatusToString(status));
        goto error;
    }

    return NV_OK;

error:
    uvm_global_exit();
    return status;
}

void uvm_global_exit(void)
{
    uvm_assert_mutex_unlocked(&g_uvm_global.global_lock);

    nv_kthread_q_stop(&g_uvm_global.deferred_release_q);

    uvm_unregister_callbacks();
    uvm_access_counters_exit();
    uvm_service_block_context_exit();
    uvm_perf_heuristics_exit();
    uvm_perf_events_exit();
    uvm_migrate_exit();
    uvm_range_group_exit();
    uvm_va_range_exit();
    uvm_va_policy_exit();
    uvm_mem_global_exit();
    uvm_pmm_sysmem_exit();
    uvm_pmm_devmem_exit();
    uvm_gpu_exit();
    uvm_processor_mask_cache_exit();

    if (g_uvm_global.rm_session_handle != 0)
        uvm_rm_locked_call_void(nvUvmInterfaceSessionDestroy(g_uvm_global.rm_session_handle));

    uvm_procfs_exit();

    nv_kthread_q_stop(&g_uvm_global.global_q);

    uvm_assert_mutex_unlocked(&g_uvm_global.va_spaces.lock);
    UVM_ASSERT(list_empty(&g_uvm_global.va_spaces.list));

    uvm_thread_context_global_exit();
    uvm_kvmalloc_exit();
}

// Signal to the top-half ISR whether calls from the RM's top-half ISR are to
// be completed without processing.
static void uvm_parent_gpu_set_isr_suspended(uvm_parent_gpu_t *parent_gpu, bool is_suspended)
{
    uvm_spin_lock_irqsave(&parent_gpu->isr.interrupts_lock);

    parent_gpu->isr.is_suspended = is_suspended;

    uvm_spin_unlock_irqrestore(&parent_gpu->isr.interrupts_lock);
}

static NV_STATUS uvm_suspend(void)
{
    uvm_va_space_t *va_space = NULL;
    uvm_gpu_id_t gpu_id;
    uvm_gpu_t *gpu;

    // Upon entry into this function, the following is true:
    //   * GPU interrupts are enabled
    //   * Any number of fault or access counter notifications could
    //     be pending
    //   * No new fault notifications will appear, but new access
    //     counter notifications could
    //   * Any of the bottom halves could be running
    //   * New bottom halves of all types could be scheduled as GPU
    //     interrupts are handled
    // Due to this, the sequence of suspend operations for each GPU is the
    // following:
    //   * Flush the fault buffer to prevent fault interrupts when
    //     the top-half ISR is suspended
    //   * Suspend access counter processing
    //   * Suspend the top-half ISR
    //   * Flush relevant kthread queues (bottom half, etc.)

    // Some locks acquired by this function, such as pm.lock, are released
    // by uvm_resume(). This is contrary to the lock tracking code's
    // expectations, so lock tracking is disabled.
    uvm_thread_context_lock_disable_tracking();

    // Take the global power management lock in write mode to lock out
    // most user-facing entry points.
    uvm_down_write(&g_uvm_global.pm.lock);

    nv_kthread_q_flush(&g_uvm_global.global_q);

    // Though global_lock isn't held here, pm.lock indirectly prevents the
    // addition and removal of GPUs, since these operations can currently
    // only occur in response to ioctl() calls.
    for_each_gpu_id_in_mask(gpu_id, &g_uvm_global.retained_gpus) {
        gpu = uvm_gpu_get(gpu_id);

        // Since fault buffer state may be lost across sleep cycles, UVM must
        // ensure any outstanding replayable faults are dismissed. The RM
        // guarantees that all user channels have been preempted before
        // uvm_suspend() is called, which implies that no user channels can be
        // stalled on faults when this point is reached.
        if (gpu->parent->replayable_faults_supported)
            uvm_gpu_fault_buffer_flush(gpu);

        // TODO: Bug 2535118: flush the non-replayable fault buffer

        // Stop access counter interrupt processing for the duration of this
        // sleep cycle to defend against potential interrupt storms in
        // the suspend path: if rate limiting is applied to access counter
        // interrupts in the bottom half in the future, the bottom half flush
        // below will no longer be able to guarantee that all outstanding
        // notifications have been handled.
        uvm_parent_gpu_access_counters_set_ignore(gpu->parent, true);

        uvm_parent_gpu_set_isr_suspended(gpu->parent, true);

        nv_kthread_q_flush(&gpu->parent->isr.bottom_half_q);

        if (gpu->parent->isr.non_replayable_faults.handling)
            nv_kthread_q_flush(&gpu->parent->isr.kill_channel_q);
    }

    // Acquire each VA space's lock in write mode to lock out VMA open and
    // release callbacks. These entry points do not have feasible early exit
    // options, and so aren't suitable for synchronization with pm.lock.
    uvm_mutex_lock(&g_uvm_global.va_spaces.lock);

    list_for_each_entry(va_space, &g_uvm_global.va_spaces.list, list_node)
        uvm_va_space_down_write(va_space);

    uvm_mutex_unlock(&g_uvm_global.va_spaces.lock);

    uvm_thread_context_lock_enable_tracking();

    g_uvm_global.pm.is_suspended = true;

    return NV_OK;
}

NV_STATUS uvm_suspend_entry(void)
{
    UVM_ENTRY_RET(uvm_suspend());
}

static NV_STATUS uvm_resume(void)
{
    uvm_va_space_t *va_space = NULL;
    uvm_gpu_id_t gpu_id;
    uvm_gpu_t *gpu;

    g_uvm_global.pm.is_suspended = false;

    // Some locks released by this function, such as pm.lock, were acquired
    // by uvm_suspend(). This is contrary to the lock tracking code's
    // expectations, so lock tracking is disabled.
    uvm_thread_context_lock_disable_tracking();

    // Release each VA space's lock.
    uvm_mutex_lock(&g_uvm_global.va_spaces.lock);

    list_for_each_entry(va_space, &g_uvm_global.va_spaces.list, list_node)
        uvm_va_space_up_write(va_space);

    uvm_mutex_unlock(&g_uvm_global.va_spaces.lock);

    // pm.lock is held in lieu of global_lock to prevent GPU addition/removal
    for_each_gpu_id_in_mask(gpu_id, &g_uvm_global.retained_gpus) {
        gpu = uvm_gpu_get(gpu_id);

        // Bring the fault buffer software state back in sync with the
        // hardware state.
        uvm_parent_gpu_fault_buffer_resume(gpu->parent);

        uvm_parent_gpu_set_isr_suspended(gpu->parent, false);

        // Reenable access counter interrupt processing unless notifications
        // have been set to be suppressed.
        uvm_parent_gpu_access_counters_set_ignore(gpu->parent, false);
    }

    uvm_up_write(&g_uvm_global.pm.lock);

    uvm_thread_context_lock_enable_tracking();

    // Force completion of any release callbacks successfully queued for
    // deferred completion while suspended. The deferred release
    // queue is not guaranteed to remain empty following this flush since
    // some threads that failed to acquire pm.lock in uvm_release() may
    // not have scheduled their handlers yet.
    nv_kthread_q_flush(&g_uvm_global.deferred_release_q);

    return NV_OK;
}

NV_STATUS uvm_resume_entry(void)
{
    UVM_ENTRY_RET(uvm_resume());
}

bool uvm_global_is_suspended(void)
{
    return g_uvm_global.pm.is_suspended;
}

void uvm_global_set_fatal_error_impl(NV_STATUS error)
{
    NV_STATUS previous_error;

    UVM_ASSERT(error != NV_OK);

    previous_error = atomic_cmpxchg(&g_uvm_global.fatal_error, NV_OK, error);

    if (previous_error == NV_OK) {
        UVM_ERR_PRINT("Encountered a global fatal error: %s\n", nvstatusToString(error));
    }
    else {
        UVM_ERR_PRINT("Encountered a global fatal error: %s after a global error has been already set: %s\n",
                nvstatusToString(error),
                nvstatusToString(previous_error));
    }

    nvUvmInterfaceReportFatalError(error);
}

NV_STATUS uvm_global_reset_fatal_error(void)
{
    if (!uvm_enable_builtin_tests) {
        UVM_ASSERT_MSG(0, "Resetting global fatal error without tests being enabled\n");
        return NV_ERR_INVALID_STATE;
    }

    return atomic_xchg(&g_uvm_global.fatal_error, NV_OK);
}

void uvm_global_gpu_retain(const uvm_processor_mask_t *mask)
{
    uvm_gpu_t *gpu;

    for_each_gpu_in_mask(gpu, mask)
        uvm_gpu_retain(gpu);
}

void uvm_global_gpu_release(const uvm_processor_mask_t *mask)
{
    uvm_gpu_id_t gpu_id;

    if (uvm_processor_mask_empty(mask))
        return;

    uvm_mutex_lock(&g_uvm_global.global_lock);

    // Do not use for_each_gpu_in_mask as it reads the GPU state and it
    // might get destroyed.
    for_each_gpu_id_in_mask(gpu_id, mask)
        uvm_gpu_release_locked(uvm_gpu_get(gpu_id));

    uvm_mutex_unlock(&g_uvm_global.global_lock);
}

NV_STATUS uvm_global_gpu_check_ecc_error(uvm_processor_mask_t *gpus)
{
    uvm_gpu_t *gpu;

    for_each_gpu_in_mask(gpu, gpus) {
        NV_STATUS status = uvm_gpu_check_ecc_error(gpu);
        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

static NV_STATUS suspend_and_drainP2P(const NvProcessorUuid *parent_uuid)
{
    NV_STATUS status = NV_OK;
    uvm_parent_gpu_t *parent_gpu;

    uvm_mutex_lock(&g_uvm_global.global_lock);

    // NVLINK STO recovery is not supported in combination with MIG
    parent_gpu = uvm_parent_gpu_get_by_uuid(parent_uuid);
    if (!parent_gpu || parent_gpu->smc.enabled) {
        status = NV_ERR_INVALID_DEVICE;
        goto unlock;
    }

    status = uvm_channel_manager_suspend_p2p(parent_gpu->gpus[0]->channel_manager);

unlock:
    uvm_mutex_unlock(&g_uvm_global.global_lock);
    return status;
}

static NV_STATUS resumeP2P(const NvProcessorUuid *parent_uuid)
{
    NV_STATUS status = NV_OK;
    uvm_parent_gpu_t *parent_gpu;

    uvm_mutex_lock(&g_uvm_global.global_lock);

    // NVLINK STO recovery is not supported in combination with MIG
    parent_gpu = uvm_parent_gpu_get_by_uuid(parent_uuid);
    if (!parent_gpu || parent_gpu->smc.enabled) {
        status = NV_ERR_INVALID_DEVICE;
        goto unlock;
    }

    uvm_channel_manager_resume_p2p(parent_gpu->gpus[0]->channel_manager);

unlock:
    uvm_mutex_unlock(&g_uvm_global.global_lock);
    return status;
}

NV_STATUS uvm_suspend_and_drainP2P_entry(const NvProcessorUuid *uuid)
{
    UVM_ENTRY_RET(suspend_and_drainP2P(uuid));
}

NV_STATUS uvm_resumeP2P_entry(const NvProcessorUuid *uuid)
{
    UVM_ENTRY_RET(resumeP2P(uuid));
}

NV_STATUS uvm_global_gpu_check_nvlink_error(uvm_processor_mask_t *gpus)
{
    uvm_gpu_t *gpu;

    for_each_gpu_in_mask(gpu, gpus) {
        NV_STATUS status = uvm_gpu_check_nvlink_error(gpu);
        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}
