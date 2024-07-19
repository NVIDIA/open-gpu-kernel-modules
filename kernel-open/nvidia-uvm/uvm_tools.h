/*******************************************************************************
    Copyright (c) 2016-2024 NVIDIA Corporation

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

#ifndef __UVM_TOOLS_H__
#define __UVM_TOOLS_H__

#include "uvm_types.h"
#include "uvm_processors.h"
#include "uvm_forward_decl.h"
#include "uvm_test_ioctl.h"
#include "uvm_hal_types.h"
#include "uvm_va_block_types.h"

NV_STATUS uvm_test_inject_tools_event(UVM_TEST_INJECT_TOOLS_EVENT_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_increment_tools_counter(UVM_TEST_INCREMENT_TOOLS_COUNTER_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_tools_flush_replay_events(UVM_TEST_TOOLS_FLUSH_REPLAY_EVENTS_PARAMS *params, struct file *filp);

NV_STATUS uvm_api_tools_read_process_memory(UVM_TOOLS_READ_PROCESS_MEMORY_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_tools_write_process_memory(UVM_TOOLS_WRITE_PROCESS_MEMORY_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_tools_get_processor_uuid_table(UVM_TOOLS_GET_PROCESSOR_UUID_TABLE_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_tools_flush_events(UVM_TOOLS_FLUSH_EVENTS_PARAMS *params, struct file *filp);

static UvmEventFatalReason uvm_tools_status_to_fatal_fault_reason(NV_STATUS status)
{
    switch (status) {
        case NV_OK:
            return UvmEventFatalReasonInvalid;
        case NV_ERR_NO_MEMORY:
            return UvmEventFatalReasonOutOfMemory;
        case NV_ERR_INVALID_ADDRESS:
            return UvmEventFatalReasonInvalidAddress;
        case NV_ERR_INVALID_ACCESS_TYPE:
            return UvmEventFatalReasonInvalidPermissions;
        case NV_ERR_INVALID_OPERATION:
            return UvmEventFatalReasonInvalidOperation;
        default:
            return UvmEventFatalReasonInternalError;
    }
}

void uvm_tools_record_cpu_fatal_fault(uvm_va_space_t *va_space,
                                      NvU64 address,
                                      bool is_write,
                                      UvmEventFatalReason reason);

void uvm_tools_record_gpu_fatal_fault(uvm_gpu_id_t gpu_id,
                                      uvm_va_space_t *va_space,
                                      const uvm_fault_buffer_entry_t *fault_entry,
                                      UvmEventFatalReason reason);

void uvm_tools_record_thrashing(uvm_va_space_t *va_space,
                                NvU64 address,
                                size_t region_size,
                                const uvm_processor_mask_t *processors);

void uvm_tools_record_throttling_start(uvm_va_space_t *va_space, NvU64 address, uvm_processor_id_t processor);

void uvm_tools_record_throttling_end(uvm_va_space_t *va_space, NvU64 address, uvm_processor_id_t processor);

void uvm_tools_record_map_remote(uvm_va_block_t *va_block,
                                 uvm_push_t *push,
                                 uvm_processor_id_t processor,
                                 uvm_processor_id_t residency,
                                 NvU64 address,
                                 size_t region_size,
                                 UvmEventMapRemoteCause cause);

void uvm_tools_record_block_migration_begin(uvm_va_block_t *va_block,
                                            uvm_push_t *push,
                                            uvm_processor_id_t dst_id,
                                            uvm_processor_id_t src_id,
                                            NvU64 start,
                                            uvm_make_resident_cause_t cause);

void uvm_tools_record_read_duplicate(uvm_va_block_t *va_block,
                                     uvm_processor_id_t dst,
                                     uvm_va_block_region_t region,
                                     const uvm_page_mask_t *page_mask);

void uvm_tools_record_read_duplicate_invalidate(uvm_va_block_t *va_block,
                                                uvm_processor_id_t dst,
                                                uvm_va_block_region_t region,
                                                const uvm_page_mask_t *page_mask);

void uvm_tools_broadcast_replay(uvm_gpu_t *gpu, uvm_push_t *push, NvU32 batch_id, uvm_fault_client_type_t client_type);

void uvm_tools_broadcast_replay_sync(uvm_gpu_t *gpu, NvU32 batch_id, uvm_fault_client_type_t client_type);

void uvm_tools_broadcast_access_counter(uvm_gpu_t *gpu,
                                        const uvm_access_counter_buffer_entry_t *buffer_entry,
                                        bool on_managed_phys);

void uvm_tools_record_access_counter(uvm_va_space_t *va_space,
                                     uvm_gpu_id_t gpu_id,
                                     const uvm_access_counter_buffer_entry_t *buffer_entry,
                                     bool on_managed_phys);

void uvm_tools_test_hmm_split_invalidate(uvm_va_space_t *va_space);

// schedules completed events and then waits from the to be dispatched
void uvm_tools_flush_events(void);

#endif // __UVM_TOOLS_H__
