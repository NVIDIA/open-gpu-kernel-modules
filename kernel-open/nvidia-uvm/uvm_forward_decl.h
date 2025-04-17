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

#ifndef __UVM_FORWARD_DECL_H__
#define __UVM_FORWARD_DECL_H__

typedef struct uvm_global_struct uvm_global_t;

typedef struct uvm_gpu_struct uvm_gpu_t;
typedef struct uvm_parent_gpu_struct uvm_parent_gpu_t;
typedef struct uvm_gpu_chunk_struct uvm_gpu_chunk_t;
typedef struct uvm_cpu_chunk_struct uvm_cpu_chunk_t;
typedef struct uvm_rm_mem_struct uvm_rm_mem_t;
typedef struct uvm_mem_struct uvm_mem_t;
typedef struct uvm_host_hal_struct uvm_host_hal_t;
typedef struct uvm_ce_hal_struct uvm_ce_hal_t;
typedef struct uvm_arch_hal_struct uvm_arch_hal_t;
typedef struct uvm_fault_buffer_hal_struct uvm_fault_buffer_hal_t;
typedef struct uvm_access_counter_buffer_hal_struct uvm_access_counter_buffer_hal_t;
typedef struct uvm_sec2_hal_struct uvm_sec2_hal_t;
typedef struct uvm_gpu_semaphore_struct uvm_gpu_semaphore_t;
typedef struct uvm_gpu_tracking_semaphore_struct uvm_gpu_tracking_semaphore_t;
typedef struct uvm_gpu_semaphore_pool_struct uvm_gpu_semaphore_pool_t;
typedef struct uvm_gpu_semaphore_pool_page_struct uvm_gpu_semaphore_pool_page_t;
typedef struct uvm_mmu_mode_hal_struct uvm_mmu_mode_hal_t;

typedef struct uvm_channel_manager_struct uvm_channel_manager_t;
typedef struct uvm_channel_struct uvm_channel_t;
typedef struct uvm_user_channel_struct uvm_user_channel_t;
typedef struct uvm_push_struct uvm_push_t;
typedef struct uvm_push_info_struct uvm_push_info_t;
typedef struct uvm_push_crypto_bundle_struct uvm_push_crypto_bundle_t;
typedef struct uvm_push_acquire_info_struct uvm_push_acquire_info_t;
typedef struct uvm_pushbuffer_struct uvm_pushbuffer_t;
typedef struct uvm_gpfifo_entry_struct uvm_gpfifo_entry_t;

typedef struct uvm_va_policy_struct uvm_va_policy_t;
typedef struct uvm_va_range_struct uvm_va_range_t;
typedef struct uvm_va_range_managed_struct uvm_va_range_managed_t;
typedef struct uvm_va_range_external_struct uvm_va_range_external_t;
typedef struct uvm_va_range_channel_struct uvm_va_range_channel_t;
typedef struct uvm_va_range_sked_reflected_struct uvm_va_range_sked_reflected_t;
typedef struct uvm_va_range_semaphore_pool_struct uvm_va_range_semaphore_pool_t;
typedef struct uvm_va_range_device_p2p_struct uvm_va_range_device_p2p_t;
typedef struct uvm_va_block_struct uvm_va_block_t;
typedef struct uvm_va_block_test_struct uvm_va_block_test_t;
typedef struct uvm_va_block_wrapper_struct uvm_va_block_wrapper_t;
typedef struct uvm_va_block_retry_struct uvm_va_block_retry_t;
typedef struct uvm_va_space_struct uvm_va_space_t;
typedef struct uvm_va_space_mm_struct uvm_va_space_mm_t;

typedef struct uvm_make_resident_context_struct uvm_make_resident_context_t;

typedef struct uvm_gpu_va_space_struct uvm_gpu_va_space_t;

typedef struct uvm_thread_context_lock_struct uvm_thread_context_lock_t;
typedef struct uvm_thread_context_struct uvm_thread_context_t;
typedef struct uvm_thread_context_wrapper_struct uvm_thread_context_wrapper_t;

typedef struct uvm_perf_module_struct uvm_perf_module_t;

typedef struct uvm_page_table_range_vec_struct uvm_page_table_range_vec_t;
typedef struct uvm_page_table_range_struct uvm_page_table_range_t;
typedef struct uvm_page_tree_struct uvm_page_tree_t;

typedef struct uvm_fault_buffer_entry_struct uvm_fault_buffer_entry_t;

typedef struct uvm_pte_batch_struct uvm_pte_batch_t;
typedef struct uvm_tlb_batch_struct uvm_tlb_batch_t;

typedef struct uvm_fault_service_batch_context_struct uvm_fault_service_batch_context_t;
typedef struct uvm_service_block_context_struct uvm_service_block_context_t;

typedef struct uvm_ats_fault_invalidate_struct uvm_ats_fault_invalidate_t;

typedef struct uvm_replayable_fault_buffer_struct uvm_replayable_fault_buffer_t;
typedef struct uvm_non_replayable_fault_buffer_struct uvm_non_replayable_fault_buffer_t;
typedef struct uvm_access_counter_buffer_entry_struct uvm_access_counter_buffer_entry_t;
typedef struct uvm_access_counter_buffer_struct uvm_access_counter_buffer_t;
typedef struct uvm_access_counter_service_batch_context_struct uvm_access_counter_service_batch_context_t;
typedef struct uvm_pmm_sysmem_mappings_struct uvm_pmm_sysmem_mappings_t;

typedef struct uvm_reverse_map_struct uvm_reverse_map_t;
#endif //__UVM_FORWARD_DECL_H__
