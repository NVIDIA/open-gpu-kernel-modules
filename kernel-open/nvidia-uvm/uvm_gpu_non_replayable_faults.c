/*******************************************************************************
    Copyright (c) 2017-2025 NVIDIA Corporation

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

#include "nv_uvm_interface.h"
#include "uvm_common.h"
#include "uvm_api.h"
#include "uvm_gpu_non_replayable_faults.h"
#include "uvm_gpu.h"
#include "uvm_hal.h"
#include "uvm_lock.h"
#include "uvm_tools.h"
#include "uvm_user_channel.h"
#include "uvm_va_space_mm.h"
#include "uvm_va_block.h"
#include "uvm_va_range.h"
#include "uvm_kvmalloc.h"
#include "uvm_ats_faults.h"

// In the context of a CUDA application using Unified Memory, it is sometimes
// assumed that there is a single type of fault, originated by a memory
// load/store in a SM (Graphics Engine), which itself can be traced back to a
// memory access in a CUDA kernel written by a developer. In reality, faults can
// also be triggered by other parts of the GPU i.e. by other engines, as the
// result of developer-facing APIs, or operations initiated by a user-mode
// driver. The Graphics Engine faults are called replayable faults, while the
// rest are called non-replayable. The differences between the two types of
// faults go well beyond the engine originating the fault.
//
// A non-replayable fault originates in an engine other than Graphics. UVM
// services non-replayable faults from the Copy and PBDMA (Host/ESCHED) Engines.
// Non-replayable faults originated in other engines are considered fatal, and
// do not reach the UVM driver. While UVM can distinguish between faults
// originated in the Copy Engine and faults originated in the PBDMA Engine, in
// practice they are all processed in the same way. Replayable fault support in
// Graphics was introduced in Pascal, and non-replayable fault support in CE and
// PBDMA Engines was introduced in Volta; all non-replayable faults were fatal
// before Volta.
//
// An example of a Copy Engine non-replayable fault is a memory copy between two
// virtual addresses on a GPU, in which either the source or destination
// pointers are not currently mapped to a physical address in the page tables of
// the GPU. An example of a PBDMA non-replayable fault is a semaphore acquire in
// which the semaphore virtual address passed as argument is currently not
// mapped to any physical address.
//
// Non-replayable faults originated in the CE and PBDMA Engines result in HW
// preempting the channel associated with the fault, a mechanism called "fault
// and switch". More precisely, the switching out affects not only the channel
// that caused the fault, but all the channels in the same Time Slice Group
// (TSG). SW intervention is required so all the channels in the TSG can be
// scheduled again, but channels in other TSGs can be scheduled and resume their
// normal execution. In the case of the non-replayable faults serviced by UVM,
// the driver clears a channel's faulted bit upon successful servicing, but it
// is only when the servicing has completed for all the channels in the TSG that
// they are all allowed to be switched in.  Non-replayable faults originated in
// engines other than CE and PBDMA are fatal because these other units lack
// hardware support for the "fault and switch" and restart mechanisms just
// described.
// On the other hand, replayable faults block preemption of the channel until
// software (UVM) services the fault. This is sometimes known as "fault and
// stall". Note that replayable faults prevent the execution of other channels,
// which are stalled until the fault is serviced.
//
// The "non-replayable" naming alludes to the fact that, historically, these
// faults indicated a fatal condition so there was no recovery ("replay")
// process, and SW could not ignore or drop the fault. As discussed before, this
// is no longer the case and while at times the hardware documentation uses the
// "fault and replay" expression for CE and PBDMA faults, we reserve that
// expression for Graphics faults and favor the term "fault and reschedule"
// instead. Replaying a fault does not necessarily imply that UVM has serviced
// it. For example, the UVM driver may choose to ignore the replayable faults
// associated with a GPU for some period of time if it detects that there is
// thrashing going on, and the GPU needs to be throttled. The fault entries
// corresponding to the ignored faults are never saved by UVM, but new entries
// (and new interrupts) will be generated by hardware each time after UVM issues
// a replay.
//
// While replayable faults are always the responsibility of UVM, the servicing
// of non-replayable faults is split between RM and UVM. In the case of
// replayable faults, UVM has sole SW ownership of the hardware buffer
// containing the faults, and it is responsible for updating the GET pointer to
// signal the hardware that a number of faults have been read. UVM also reads
// the PUT pointer value written by hardware. But in the case of non-replayable
// faults, UVM reads the fault entries out of a regular CPU buffer, shared with
// RM, called "shadow buffer". RM is responsible for accessing the actual
// non-replayable hardware buffer, reading the PUT pointer, updating the GET
// pointer, and moving CE and PBDMA faults from the hardware buffer to the
// shadow buffer. Because the Resource Manager owns the HW buffer, UVM needs to
// call RM when servicing a non-replayable fault, first to figure out if there
// is a pending fault, and then to read entries from the shadow buffer.
//
// Once UVM has parsed a non-replayable fault entry corresponding to managed
// memory, and identified the VA block associated with it, the servicing logic
// for that block is identical to that of a replayable fault, see
// uvm_va_block_service_locked. Another similarity between the two types of
// faults is that they use the same entry format, uvm_fault_buffer_entry_t.


// There is no error handling in this function. The caller is in charge of
// calling uvm_parent_gpu_fault_buffer_deinit_non_replayable_faults on failure.
NV_STATUS uvm_parent_gpu_fault_buffer_init_non_replayable_faults(uvm_parent_gpu_t *parent_gpu)
{
    uvm_non_replayable_fault_buffer_t *non_replayable_faults = &parent_gpu->fault_buffer.non_replayable;

    UVM_ASSERT(parent_gpu->non_replayable_faults_supported);

    non_replayable_faults->shadow_buffer_copy = NULL;
    non_replayable_faults->fault_cache        = NULL;

    non_replayable_faults->max_faults = parent_gpu->fault_buffer.rm_info.nonReplayable.bufferSize /
                                        parent_gpu->fault_buffer_hal->entry_size(parent_gpu);

    non_replayable_faults->shadow_buffer_copy =
        uvm_kvmalloc_zero(parent_gpu->fault_buffer.rm_info.nonReplayable.bufferSize);
    if (!non_replayable_faults->shadow_buffer_copy)
        return NV_ERR_NO_MEMORY;

    non_replayable_faults->fault_cache = uvm_kvmalloc_zero(non_replayable_faults->max_faults *
                                                           sizeof(*non_replayable_faults->fault_cache));
    if (!non_replayable_faults->fault_cache)
        return NV_ERR_NO_MEMORY;

    uvm_tracker_init(&non_replayable_faults->clear_faulted_tracker);
    uvm_tracker_init(&non_replayable_faults->fault_service_tracker);

    return NV_OK;
}

void uvm_parent_gpu_fault_buffer_deinit_non_replayable_faults(uvm_parent_gpu_t *parent_gpu)
{
    uvm_non_replayable_fault_buffer_t *non_replayable_faults = &parent_gpu->fault_buffer.non_replayable;

    if (non_replayable_faults->fault_cache) {
        UVM_ASSERT(uvm_tracker_is_empty(&non_replayable_faults->clear_faulted_tracker));
        uvm_tracker_deinit(&non_replayable_faults->clear_faulted_tracker);

        UVM_ASSERT(uvm_tracker_is_empty(&non_replayable_faults->fault_service_tracker));
        uvm_tracker_deinit(&non_replayable_faults->fault_service_tracker);
    }

    uvm_kvfree(non_replayable_faults->shadow_buffer_copy);
    uvm_kvfree(non_replayable_faults->fault_cache);
    non_replayable_faults->shadow_buffer_copy = NULL;
    non_replayable_faults->fault_cache        = NULL;
}

bool uvm_parent_gpu_non_replayable_faults_pending(uvm_parent_gpu_t *parent_gpu)
{
    NV_STATUS status;
    NvBool has_pending_faults;

    UVM_ASSERT(parent_gpu->isr.non_replayable_faults.handling);

    status = nvUvmInterfaceHasPendingNonReplayableFaults(&parent_gpu->fault_buffer.rm_info,
                                                         &has_pending_faults);
    UVM_ASSERT(status == NV_OK);

    return has_pending_faults == NV_TRUE;
}

static NV_STATUS fetch_non_replayable_fault_buffer_entries(uvm_parent_gpu_t *parent_gpu, NvU32 *cached_faults)
{
    NV_STATUS status;
    NvU32 i;
    NvU32 entry_size = parent_gpu->fault_buffer_hal->entry_size(parent_gpu);
    uvm_non_replayable_fault_buffer_t *non_replayable_faults = &parent_gpu->fault_buffer.non_replayable;
    char *current_hw_entry = (char *)non_replayable_faults->shadow_buffer_copy;
    uvm_fault_buffer_entry_t *fault_entry = non_replayable_faults->fault_cache;

    UVM_ASSERT(uvm_sem_is_locked(&parent_gpu->isr.non_replayable_faults.service_lock));
    UVM_ASSERT(parent_gpu->non_replayable_faults_supported);

    status = nvUvmInterfaceGetNonReplayableFaults(&parent_gpu->fault_buffer.rm_info,
                                                  current_hw_entry,
                                                  cached_faults);

    if (status != NV_OK) {
        UVM_ERR_PRINT("nvUvmInterfaceGetNonReplayableFaults() failed: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_parent_gpu_name(parent_gpu));

        uvm_global_set_fatal_error(status);
        return status;
    }

    // Parse all faults
    for (i = 0; i < *cached_faults; ++i) {
        parent_gpu->fault_buffer_hal->parse_non_replayable_entry(parent_gpu, current_hw_entry, fault_entry);

        // The GPU aligns the fault addresses to 4k, but all of our tracking is
        // done in PAGE_SIZE chunks which might be larger.
        fault_entry->fault_address = UVM_PAGE_ALIGN_DOWN(fault_entry->fault_address);

        // Make sure that all fields in the entry are properly initialized
        fault_entry->va_space = NULL;
        fault_entry->gpu = NULL;
        fault_entry->is_fatal = (fault_entry->fault_type >= UVM_FAULT_TYPE_FATAL);
        fault_entry->filtered = false;

        fault_entry->num_instances = 1;
        fault_entry->access_type_mask = uvm_fault_access_type_mask_bit(fault_entry->fault_access_type);
        INIT_LIST_HEAD(&fault_entry->merged_instances_list);
        fault_entry->non_replayable.buffer_index = i;

        if (fault_entry->is_fatal) {
            // Record the fatal fault event later as we need the va_space locked
            fault_entry->fatal_reason = UvmEventFatalReasonInvalidFaultType;
        }
        else {
            fault_entry->fatal_reason = UvmEventFatalReasonInvalid;
        }

        current_hw_entry += entry_size;
        fault_entry++;
    }

    return NV_OK;
}

static bool use_clear_faulted_channel_sw_method(uvm_parent_gpu_t *parent_gpu)
{
    // If true, UVM uses a SW method to request RM to do the clearing on its
    // behalf.
    bool use_sw_method = false;

    // In SRIOV, the UVM (guest) driver does not have access to the privileged
    // registers used to clear the faulted bit.
    if (uvm_parent_gpu_is_virt_mode_sriov(parent_gpu))
        use_sw_method = true;

    // In Confidential Computing access to the privileged registers is blocked,
    // in order to prevent interference between guests, or between the
    // (untrusted) host and the guests.
    if (g_uvm_global.conf_computing_enabled)
        use_sw_method = true;

    if (use_sw_method)
        UVM_ASSERT(parent_gpu->has_clear_faulted_channel_sw_method);

    return use_sw_method;
}

static NV_STATUS clear_faulted_method_on_gpu(uvm_user_channel_t *user_channel,
                                             const uvm_fault_buffer_entry_t *fault_entry,
                                             NvU32 batch_id,
                                             uvm_tracker_t *tracker)
{
    uvm_gpu_t *gpu = user_channel->gpu;
    NV_STATUS status;
    uvm_push_t push;
    uvm_non_replayable_fault_buffer_t *non_replayable_faults = &gpu->parent->fault_buffer.non_replayable;

    UVM_ASSERT(!fault_entry->is_fatal);

    status = uvm_push_begin_acquire(gpu->channel_manager,
                                    UVM_CHANNEL_TYPE_MEMOPS,
                                    tracker,
                                    &push,
                                    "Clearing set bit for address 0x%llx",
                                    fault_entry->fault_address);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Error acquiring tracker before clearing faulted: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        return status;
    }

    if (use_clear_faulted_channel_sw_method(gpu->parent))
        gpu->parent->host_hal->clear_faulted_channel_sw_method(&push, user_channel, fault_entry);
    else
        gpu->parent->host_hal->clear_faulted_channel_method(&push, user_channel, fault_entry);

    uvm_tools_broadcast_replay(gpu, &push, batch_id, fault_entry->fault_source.client_type);

    uvm_push_end(&push);

    // Add this push to the GPU's clear_faulted_tracker so GPU removal can wait
    // on it.
    status = uvm_tracker_add_push_safe(&non_replayable_faults->clear_faulted_tracker, &push);

    // Add this push to the channel's clear_faulted_tracker so user channel
    // removal can wait on it instead of using the per-GPU tracker, which would
    // require a lock.
    if (status == NV_OK)
        status = uvm_tracker_add_push_safe(&user_channel->clear_faulted_tracker, &push);

    return status;
}

static NV_STATUS clear_faulted_register_on_gpu(uvm_user_channel_t *user_channel,
                                               const uvm_fault_buffer_entry_t *fault_entry,
                                               NvU32 batch_id,
                                               uvm_tracker_t *tracker)
{
    uvm_gpu_t *gpu = user_channel->gpu;
    NV_STATUS status;

    UVM_ASSERT(!gpu->parent->has_clear_faulted_channel_method);

    // We need to wait for all pending work before writing to the channel
    // register
    status = uvm_tracker_wait(tracker);
    if (status != NV_OK)
        return status;

    gpu->parent->host_hal->clear_faulted_channel_register(user_channel, fault_entry);

    uvm_tools_broadcast_replay_sync(gpu, batch_id, fault_entry->fault_source.client_type);

    return NV_OK;
}

static NV_STATUS clear_faulted_on_gpu(uvm_user_channel_t *user_channel,
                                      const uvm_fault_buffer_entry_t *fault_entry,
                                      NvU32 batch_id,
                                      uvm_tracker_t *tracker)
{
    uvm_gpu_t *gpu = user_channel->gpu;

    if (gpu->parent->has_clear_faulted_channel_method || use_clear_faulted_channel_sw_method(gpu->parent))
        return clear_faulted_method_on_gpu(user_channel, fault_entry, batch_id, tracker);

    return clear_faulted_register_on_gpu(user_channel, fault_entry, batch_id, tracker);
}

static NV_STATUS service_managed_fault_in_block_locked(uvm_va_block_t *va_block,
                                                       uvm_va_block_retry_t *va_block_retry,
                                                       uvm_fault_buffer_entry_t *fault_entry,
                                                       uvm_service_block_context_t *service_context,
                                                       const bool hmm_migratable)
{
    uvm_gpu_t *gpu = fault_entry->gpu;
    NV_STATUS status = NV_OK;
    uvm_page_index_t page_index;
    uvm_perf_thrashing_hint_t thrashing_hint;
    uvm_processor_id_t new_residency;
    bool read_duplicate;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_non_replayable_fault_buffer_t *non_replayable_faults = &gpu->parent->fault_buffer.non_replayable;
    const uvm_va_policy_t *policy;

    UVM_ASSERT(!fault_entry->is_fatal);

    uvm_assert_rwsem_locked(&va_space->lock);

    UVM_ASSERT(fault_entry->va_space == va_space);
    UVM_ASSERT(fault_entry->fault_address >= va_block->start);
    UVM_ASSERT(fault_entry->fault_address <= va_block->end);

    policy = uvm_va_policy_get(va_block, fault_entry->fault_address);

    if (service_context->num_retries == 0) {
        // notify event to tools/performance heuristics. For now we use a
        // unique batch id per fault, since we clear the faulted channel for
        // each fault.
        uvm_perf_event_notify_gpu_fault(&va_space->perf_events,
                                        va_block,
                                        gpu->id,
                                        policy->preferred_location,
                                        fault_entry,
                                        ++non_replayable_faults->batch_id,
                                        false);
    }

    // Check logical permissions
    status = uvm_va_block_check_logical_permissions(va_block,
                                                    service_context->block_context,
                                                    gpu->id,
                                                    uvm_va_block_cpu_page_index(va_block,
                                                                                fault_entry->fault_address),
                                                    fault_entry->fault_access_type,
                                                    uvm_range_group_address_migratable(va_space,
                                                                                       fault_entry->fault_address));
    if (status != NV_OK) {
        fault_entry->is_fatal = true;
        fault_entry->fatal_reason = uvm_tools_status_to_fatal_fault_reason(status);
        return NV_OK;
    }

    // TODO: Bug 1880194: Revisit thrashing detection
    thrashing_hint.type = UVM_PERF_THRASHING_HINT_TYPE_NONE;

    service_context->read_duplicate_count = 0;
    service_context->thrashing_pin_count = 0;

    page_index = uvm_va_block_cpu_page_index(va_block, fault_entry->fault_address);

    // Compute new residency and update the masks
    new_residency = uvm_va_block_select_residency(va_block,
                                                  service_context->block_context,
                                                  page_index,
                                                  gpu->id,
                                                  fault_entry->access_type_mask,
                                                  policy,
                                                  &thrashing_hint,
                                                  UVM_SERVICE_OPERATION_NON_REPLAYABLE_FAULTS,
                                                  hmm_migratable,
                                                  &read_duplicate);

    // Initialize the minimum necessary state in the fault service context
    uvm_processor_mask_zero(&service_context->resident_processors);

    // Set new residency and update the masks
    uvm_processor_mask_set(&service_context->resident_processors, new_residency);

    // The masks need to be fully zeroed as the fault region may grow due to prefetching
    uvm_page_mask_zero(&service_context->per_processor_masks[uvm_id_value(new_residency)].new_residency);
    uvm_page_mask_set(&service_context->per_processor_masks[uvm_id_value(new_residency)].new_residency, page_index);

    if (read_duplicate) {
        uvm_page_mask_zero(&service_context->read_duplicate_mask);
        uvm_page_mask_set(&service_context->read_duplicate_mask, page_index);
        service_context->read_duplicate_count = 1;
    }

    service_context->access_type[page_index] = fault_entry->fault_access_type;

    service_context->region = uvm_va_block_region_for_page(page_index);

    status = uvm_va_block_service_locked(gpu->id, va_block, va_block_retry, service_context);

    ++service_context->num_retries;

    return status;
}

static NV_STATUS service_managed_fault_in_block(uvm_va_block_t *va_block,
                                                uvm_fault_buffer_entry_t *fault_entry,
                                                const bool hmm_migratable)
{
    NV_STATUS status, tracker_status;
    uvm_va_block_retry_t va_block_retry;
    uvm_gpu_t *gpu = fault_entry->gpu;
    uvm_service_block_context_t *service_context = &gpu->parent->fault_buffer.non_replayable.block_service_context;

    service_context->operation = UVM_SERVICE_OPERATION_NON_REPLAYABLE_FAULTS;
    service_context->num_retries = 0;

    if (uvm_va_block_is_hmm(va_block))
        uvm_hmm_migrate_begin_wait(va_block);

    uvm_mutex_lock(&va_block->lock);

    status = UVM_VA_BLOCK_RETRY_LOCKED(va_block, &va_block_retry,
                                       service_managed_fault_in_block_locked(va_block,
                                                                             &va_block_retry,
                                                                             fault_entry,
                                                                             service_context,
                                                                             hmm_migratable));

    tracker_status = uvm_tracker_add_tracker_safe(&gpu->parent->fault_buffer.non_replayable.fault_service_tracker,
                                                  &va_block->tracker);

    uvm_mutex_unlock(&va_block->lock);

    if (uvm_va_block_is_hmm(va_block))
        uvm_hmm_migrate_finish(va_block);

    return status == NV_OK? tracker_status: status;
}

// See uvm_unregister_channel for comments on the the channel destruction
// sequence.
static void kill_channel_delayed(void *_user_channel)
{
    uvm_user_channel_t *user_channel = (uvm_user_channel_t *)_user_channel;
    uvm_va_space_t *va_space = user_channel->kill_channel.va_space;

    uvm_va_space_down_read_rm(va_space);
    if (user_channel->gpu_va_space) {
        // RM handles the fault, which will do the correct fault reporting in the
        // kernel logs and will initiate channel teardown
        NV_STATUS status = nvUvmInterfaceReportNonReplayableFault(uvm_gpu_device_handle(user_channel->gpu),
                                                                  user_channel->kill_channel.fault_packet);
        UVM_ASSERT(status == NV_OK);
    }
    uvm_va_space_up_read_rm(va_space);

    uvm_user_channel_release(user_channel);
}

static void kill_channel_delayed_entry(void *user_channel)
{
    UVM_ENTRY_VOID(kill_channel_delayed(user_channel));
}

static void schedule_kill_channel(uvm_fault_buffer_entry_t *fault_entry, uvm_user_channel_t *user_channel)
{
    uvm_va_space_t *va_space = fault_entry->va_space;
    uvm_parent_gpu_t *parent_gpu = fault_entry->gpu->parent;
    uvm_non_replayable_fault_buffer_t *non_replayable_faults = &parent_gpu->fault_buffer.non_replayable;
    void *packet = (char *)non_replayable_faults->shadow_buffer_copy +
                   (fault_entry->non_replayable.buffer_index * parent_gpu->fault_buffer_hal->entry_size(parent_gpu));

    UVM_ASSERT(va_space);
    UVM_ASSERT(user_channel);

    if (user_channel->kill_channel.scheduled)
        return;

    user_channel->kill_channel.scheduled = true;
    user_channel->kill_channel.va_space = va_space;

    // Save the packet to be handled by RM in the channel structure
    memcpy(user_channel->kill_channel.fault_packet, packet, parent_gpu->fault_buffer_hal->entry_size(parent_gpu));

    // Retain the channel here so it is not prematurely destroyed. It will be
    // released after forwarding the fault to RM in kill_channel_delayed.
    uvm_user_channel_retain(user_channel);

    // Schedule a work item to kill the channel
    nv_kthread_q_item_init(&user_channel->kill_channel.kill_channel_q_item,
                           kill_channel_delayed_entry,
                           user_channel);

    nv_kthread_q_schedule_q_item(&parent_gpu->isr.kill_channel_q,
                                 &user_channel->kill_channel.kill_channel_q_item);
}

static void service_fault_fatal(uvm_fault_buffer_entry_t *fault_entry, NV_STATUS status)
{
    UVM_ASSERT(fault_entry->fault_access_type != UVM_FAULT_ACCESS_TYPE_PREFETCH);

    fault_entry->is_fatal = true;
    fault_entry->fatal_reason = uvm_tools_status_to_fatal_fault_reason(status);
}

static NV_STATUS service_non_managed_fault(uvm_gpu_va_space_t *gpu_va_space,
                                           struct mm_struct *mm,
                                           uvm_fault_buffer_entry_t *fault_entry,
                                           NV_STATUS lookup_status)
{
    uvm_va_space_t *va_space = gpu_va_space->va_space;
    uvm_gpu_t *gpu = gpu_va_space->gpu;
    uvm_non_replayable_fault_buffer_t *non_replayable_faults = &gpu->parent->fault_buffer.non_replayable;
    uvm_ats_fault_invalidate_t *ats_invalidate = &non_replayable_faults->ats_invalidate;
    NV_STATUS status = lookup_status;
    NV_STATUS fatal_fault_status = NV_ERR_INVALID_ADDRESS;

    UVM_ASSERT(!fault_entry->is_fatal);
    UVM_ASSERT(fault_entry->va_space == va_space);
    UVM_ASSERT(fault_entry->gpu == gpu);

    // Avoid dropping fault events when the VA block is not found or cannot be created
    uvm_perf_event_notify_gpu_fault(&va_space->perf_events,
                                    NULL,
                                    gpu->id,
                                    UVM_ID_INVALID,
                                    fault_entry,
                                    ++non_replayable_faults->batch_id,
                                    false);

    if (status != NV_ERR_INVALID_ADDRESS)
        return status;

    if (uvm_ats_can_service_faults(gpu_va_space, mm)) {
        struct vm_area_struct *vma;
        uvm_va_range_t *va_range_next;
        NvU64 fault_address = fault_entry->fault_address;
        uvm_fault_access_type_t fault_access_type = fault_entry->fault_access_type;
        uvm_ats_fault_context_t *ats_context = &non_replayable_faults->ats_context;

        uvm_page_mask_zero(&ats_context->faults.prefetch_only_fault_mask);
        uvm_page_mask_zero(&ats_context->faults.read_fault_mask);
        uvm_page_mask_zero(&ats_context->faults.write_fault_mask);
        uvm_page_mask_zero(&ats_context->faults.accessed_mask);

        ats_context->client_type = UVM_FAULT_CLIENT_TYPE_HUB;

        ats_invalidate->tlb_batch_pending = false;

        va_range_next = uvm_va_space_iter_gmmu_mappable_first(va_space, fault_entry->fault_address);

        // The VA isn't managed. See if ATS knows about it.
        vma = find_vma_intersection(mm, fault_address, fault_address + 1);
        if (!vma || uvm_ats_check_in_gmmu_region(va_space, fault_address, va_range_next)) {

            // Do not return error due to logical errors in the application
            status = NV_OK;
        }
        else {
            NvU64 base = UVM_VA_BLOCK_ALIGN_DOWN(fault_address);
            uvm_page_mask_t *faults_serviced_mask = &ats_context->faults.faults_serviced_mask;
            uvm_page_mask_t *accessed_mask = &ats_context->faults.accessed_mask;
            uvm_page_index_t page_index = (fault_address - base) / PAGE_SIZE;
            uvm_page_mask_t *fault_mask = (fault_access_type >= UVM_FAULT_ACCESS_TYPE_WRITE) ?
                                                                                &ats_context->faults.write_fault_mask :
                                                                                &ats_context->faults.read_fault_mask;

            uvm_page_mask_set(fault_mask, page_index);

            uvm_page_mask_set(accessed_mask, page_index);

            status = uvm_ats_service_faults(gpu_va_space, vma, base, ats_context);
            if (status == NV_OK) {
                // Invalidate ATS TLB entries if needed
                if (uvm_page_mask_test(faults_serviced_mask, page_index)) {
                    status = uvm_ats_invalidate_tlbs(gpu_va_space,
                                                     ats_invalidate,
                                                     &non_replayable_faults->fault_service_tracker);
                    fatal_fault_status = NV_OK;
                }
            }
            else {
                fatal_fault_status = status;
            }
        }
    }
    else {
        fatal_fault_status = status;

        // Do not return error due to logical errors in the application
        status = NV_OK;
    }

    if (fatal_fault_status != NV_OK)
        service_fault_fatal(fault_entry, fatal_fault_status);

    return status;
}

static NV_STATUS service_fault_once(uvm_parent_gpu_t *parent_gpu,
                                    uvm_fault_buffer_entry_t *fault_entry,
                                    const bool hmm_migratable)
{
    NV_STATUS status;
    uvm_user_channel_t *user_channel;
    uvm_va_block_t *va_block;
    uvm_va_space_t *va_space;
    struct mm_struct *mm;
    uvm_gpu_va_space_t *gpu_va_space;
    uvm_gpu_t *gpu;
    uvm_non_replayable_fault_buffer_t *non_replayable_faults = &parent_gpu->fault_buffer.non_replayable;
    uvm_va_block_context_t *va_block_context = non_replayable_faults->block_service_context.block_context;

    status = uvm_parent_gpu_fault_entry_to_va_space(parent_gpu,
                                                    fault_entry,
                                                    &va_space,
                                                    &gpu);
    if (status != NV_OK) {
        // The VA space lookup will fail if we're running concurrently with
        // removal of the channel from the VA space (channel unregister, GPU VA
        // space unregister, VA space destroy, etc). The other thread will stop
        // the channel and remove the channel from the table, so the faulting
        // condition will be gone. In the case of replayable faults we need to
        // flush the buffer, but here we can just ignore the entry and proceed
        // on.
        //
        // Note that we can't have any subcontext issues here, since non-
        // replayable faults only use the address space of their channel.
        UVM_ASSERT(status == NV_ERR_INVALID_CHANNEL);
        UVM_ASSERT(!va_space);
        UVM_ASSERT(!gpu);
        return NV_OK;
    }

    UVM_ASSERT(va_space);
    UVM_ASSERT(gpu);

    // If an mm is registered with the VA space, we have to retain it
    // in order to lock it before locking the VA space. It is guaranteed
    // to remain valid until we release. If no mm is registered, we
    // can only service managed faults, not ATS/HMM faults.
    mm = uvm_va_space_mm_retain_lock(va_space);
    uvm_va_block_context_init(va_block_context, mm);

    uvm_va_space_down_read(va_space);

    gpu_va_space = uvm_gpu_va_space_get(va_space, gpu);
    if (!gpu_va_space) {
        // The va_space might have gone away. See the comment above.
        status = NV_OK;
        goto exit_no_channel;
    }

    fault_entry->va_space = va_space;
    fault_entry->gpu = gpu;

    user_channel = uvm_gpu_va_space_get_user_channel(gpu_va_space, fault_entry->instance_ptr);
    if (!user_channel) {
        // The channel might have gone away. See the comment above.
        status = NV_OK;
        goto exit_no_channel;
    }

    fault_entry->fault_source.channel_id = user_channel->hw_channel_id;

    if (!fault_entry->is_fatal) {
        if (mm) {
            status = uvm_va_block_find_create(va_space,
                                              fault_entry->fault_address,
                                              &va_block_context->hmm.vma,
                                              &va_block);
        }
        else {
            status = uvm_va_block_find_create_managed(va_space,
                                                      fault_entry->fault_address,
                                                      &va_block);
        }
        if (status == NV_OK)
            status = service_managed_fault_in_block(va_block, fault_entry, hmm_migratable);
        else
            status = service_non_managed_fault(gpu_va_space, mm, fault_entry, status);

        // We are done, we clear the faulted bit on the channel, so it can be
        // re-scheduled again
        if (status == NV_OK && !fault_entry->is_fatal) {
            status = clear_faulted_on_gpu(user_channel,
                                          fault_entry,
                                          non_replayable_faults->batch_id,
                                          &non_replayable_faults->fault_service_tracker);
            uvm_tracker_clear(&non_replayable_faults->fault_service_tracker);
        }
    }

    if (fault_entry->is_fatal)
        uvm_tools_record_gpu_fatal_fault(gpu->id, va_space, fault_entry, fault_entry->fatal_reason);

    if (fault_entry->is_fatal ||
        (status != NV_OK &&
         status != NV_WARN_MORE_PROCESSING_REQUIRED &&
         status != NV_WARN_MISMATCHED_TARGET))
        schedule_kill_channel(fault_entry, user_channel);

exit_no_channel:
    uvm_va_space_up_read(va_space);
    uvm_va_space_mm_release_unlock(va_space, mm);

    if (status != NV_OK &&
        status != NV_WARN_MORE_PROCESSING_REQUIRED &&
        status != NV_WARN_MISMATCHED_TARGET)
        UVM_DBG_PRINT("Error servicing non-replayable faults on GPU: %s\n",
                      uvm_parent_gpu_name(parent_gpu));

    return status;
}

static NV_STATUS service_fault(uvm_parent_gpu_t *parent_gpu, uvm_fault_buffer_entry_t *fault_entry)
{
    uvm_service_block_context_t *service_context =
        &parent_gpu->fault_buffer.non_replayable.block_service_context;
    NV_STATUS status;
    bool hmm_migratable = true;

    service_context->num_retries = 0;

    do {
        status = service_fault_once(parent_gpu, fault_entry, hmm_migratable);
        if (status == NV_WARN_MISMATCHED_TARGET) {
            hmm_migratable = false;
            status = NV_WARN_MORE_PROCESSING_REQUIRED;
        }
    } while (status == NV_WARN_MORE_PROCESSING_REQUIRED);

    return status;
}

void uvm_parent_gpu_service_non_replayable_fault_buffer(uvm_parent_gpu_t *parent_gpu)
{
    NvU32 cached_faults;

    // If this handler is modified to handle fewer than all of the outstanding
    // faults, then special handling will need to be added to uvm_suspend()
    // to guarantee that fault processing has completed before control is
    // returned to the RM.
    do {
        NV_STATUS status;
        NvU32 i;

        status = fetch_non_replayable_fault_buffer_entries(parent_gpu, &cached_faults);
        if (status != NV_OK)
            return;

        // Differently to replayable faults, we do not batch up and preprocess
        // non-replayable faults since getting multiple faults on the same
        // memory region is not very likely
        for (i = 0; i < cached_faults; ++i) {
            status = service_fault(parent_gpu, &parent_gpu->fault_buffer.non_replayable.fault_cache[i]);
            if (status != NV_OK)
                return;
        }
    } while (cached_faults > 0);
}
