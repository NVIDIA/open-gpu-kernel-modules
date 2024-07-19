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

#ifndef __UVM_TEST_H__
#define __UVM_TEST_H__

#include "uvm_linux.h"
#include "uvm_common.h"
#include "uvm_test_ioctl.h"

// Unlike UVM_INFO_PRINT, this prints on release builds
#define UVM_TEST_PRINT(fmt, ...) UVM_PRINT_FUNC(pr_info, " " fmt, ##__VA_ARGS__)

// WARNING: This macro will return out of the current scope
#define TEST_CHECK_RET(cond)                                                        \
    do {                                                                            \
        if (unlikely(!(cond))) {                                                    \
            UVM_TEST_PRINT("Test check failed, condition '%s' not true\n", #cond);  \
            on_uvm_test_fail();                                                     \
            return NV_ERR_INVALID_STATE;                                            \
        }                                                                           \
    } while (0)

// WARNING: This macro will return out of the current scope
#define TEST_NV_CHECK_RET(call)                                                             \
    do {                                                                                    \
        NV_STATUS _status = (call);                                                         \
        if (unlikely(_status != NV_OK)) {                                                   \
            UVM_TEST_PRINT("Test check failed, call '%s' returned '%s', expected '%s'\n",   \
                           #call,                                                           \
                           nvstatusToString(_status),                                       \
                           nvstatusToString(NV_OK));                                        \
            on_uvm_test_fail();                                                             \
            return _status;                                                                 \
        }                                                                                   \
    } while (0)

// Checking macro which doesn't mask NV_ERR_NO_MEMORY
#define MEM_NV_CHECK_RET(call, expected)                                                    \
    do {                                                                                    \
        NV_STATUS _status = (call);                                                         \
        if (unlikely(_status != (expected))) {                                              \
            UVM_TEST_PRINT("Test check failed, call '%s' returned '%s', expected '%s'\n",   \
                           #call,                                                           \
                           nvstatusToString(_status),                                       \
                           nvstatusToString(expected));                                     \
            on_uvm_test_fail();                                                             \
            if (_status == NV_ERR_NO_MEMORY)                                                \
                return _status;                                                             \
            return NV_ERR_INVALID_STATE;                                                    \
        }                                                                                   \
    } while (0)

// Checking macro which sets a local variable 'status' (assumed to be in scope)
// on failure and jumps to the label.
#define TEST_NV_CHECK_GOTO(call, label)                                                     \
    do {                                                                                    \
        NV_STATUS _status = (call);                                                         \
        if (unlikely(_status != NV_OK)) {                                                   \
            UVM_TEST_PRINT("Test check failed, call '%s' returned '%s', expected '%s'\n",   \
                           #call,                                                           \
                           nvstatusToString(_status),                                       \
                           nvstatusToString(NV_OK));                                        \
            on_uvm_test_fail();                                                             \
            status = _status;                                                               \
            goto label;                                                                     \
        }                                                                                   \
    } while (0)

// WARNING: This macro sets status and jumps to (goto) a label on failure
#define TEST_CHECK_GOTO(cond, label)                                                \
    do {                                                                            \
        if (unlikely(!(cond))) {                                                    \
            UVM_TEST_PRINT("Test check failed, condition '%s' not true\n", #cond);  \
            status = NV_ERR_INVALID_STATE;                                          \
            on_uvm_test_fail();                                                     \
            goto label;                                                             \
        }                                                                           \
    } while (0)


long uvm_test_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

NV_STATUS uvm_test_range_tree_directed(UVM_TEST_RANGE_TREE_DIRECTED_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_range_tree_random(UVM_TEST_RANGE_TREE_RANDOM_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_range_allocator_sanity(UVM_TEST_RANGE_ALLOCATOR_SANITY_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_page_tree(UVM_TEST_PAGE_TREE_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_rm_mem_sanity(UVM_TEST_RM_MEM_SANITY_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_mem_sanity(UVM_TEST_MEM_SANITY_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_gpu_semaphore_sanity(UVM_TEST_GPU_SEMAPHORE_SANITY_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_tracker_sanity(UVM_TEST_TRACKER_SANITY_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_push_sanity(UVM_TEST_PUSH_SANITY_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_channel_sanity(UVM_TEST_CHANNEL_SANITY_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_channel_stress(UVM_TEST_CHANNEL_STRESS_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_ce_sanity(UVM_TEST_CE_SANITY_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_host_sanity(UVM_TEST_HOST_SANITY_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_lock_sanity(UVM_TEST_LOCK_SANITY_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_perf_utils_sanity(UVM_TEST_PERF_UTILS_SANITY_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_pmm_query(UVM_TEST_PMM_QUERY_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_pmm_sanity(UVM_TEST_PMM_SANITY_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_pmm_check_leak(UVM_TEST_PMM_CHECK_LEAK_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_pmm_async_alloc(UVM_TEST_PMM_ASYNC_ALLOC_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_pma_alloc_free(UVM_TEST_PMA_ALLOC_FREE_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_pma_get_batch_size(UVM_TEST_PMA_GET_BATCH_SIZE_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_pmm_alloc_free_root(UVM_TEST_PMM_ALLOC_FREE_ROOT_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_pmm_inject_pma_evict_error(UVM_TEST_PMM_INJECT_PMA_EVICT_ERROR_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_pmm_query_pma_stats(UVM_TEST_PMM_QUERY_PMA_STATS_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_perf_events_sanity(UVM_TEST_PERF_EVENTS_SANITY_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_perf_module_sanity(UVM_TEST_PERF_MODULE_SANITY_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_get_rm_ptes(UVM_TEST_GET_RM_PTES_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_fault_buffer_flush(UVM_TEST_FAULT_BUFFER_FLUSH_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_peer_identity_mappings(UVM_TEST_PEER_IDENTITY_MAPPINGS_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_set_prefetch_filtering(UVM_TEST_SET_PREFETCH_FILTERING_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_va_block(UVM_TEST_VA_BLOCK_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_evict_chunk(UVM_TEST_EVICT_CHUNK_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_flush_deferred_work(UVM_TEST_FLUSH_DEFERRED_WORK_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_set_page_prefetch_policy(UVM_TEST_SET_PAGE_PREFETCH_POLICY_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_get_page_thrashing_policy(UVM_TEST_GET_PAGE_THRASHING_POLICY_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_set_page_thrashing_policy(UVM_TEST_SET_PAGE_THRASHING_POLICY_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_range_group_tree(UVM_TEST_RANGE_GROUP_TREE_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_range_group_range_info(UVM_TEST_RANGE_GROUP_RANGE_INFO_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_range_group_range_count(UVM_TEST_RANGE_GROUP_RANGE_COUNT_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_get_prefetch_faults_reenable_lapse(UVM_TEST_GET_PREFETCH_FAULTS_REENABLE_LAPSE_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_set_prefetch_faults_reenable_lapse(UVM_TEST_SET_PREFETCH_FAULTS_REENABLE_LAPSE_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_check_channel_va_space(UVM_TEST_CHECK_CHANNEL_VA_SPACE_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_pmm_sysmem(UVM_TEST_PMM_SYSMEM_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_pmm_reverse_map(UVM_TEST_PMM_REVERSE_MAP_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_pmm_chunk_with_elevated_page(UVM_TEST_PMM_CHUNK_WITH_ELEVATED_PAGE_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_va_space_inject_error(UVM_TEST_VA_SPACE_INJECT_ERROR_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_get_gpu_time(UVM_TEST_GET_GPU_TIME_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_pmm_release_free_root_chunks(UVM_TEST_PMM_RELEASE_FREE_ROOT_CHUNKS_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_drain_replayable_faults(UVM_TEST_DRAIN_REPLAYABLE_FAULTS_PARAMS *params, struct file *filp);

NV_STATUS uvm_test_va_space_add_dummy_thread_contexts(UVM_TEST_VA_SPACE_ADD_DUMMY_THREAD_CONTEXTS_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_va_space_remove_dummy_thread_contexts(UVM_TEST_VA_SPACE_REMOVE_DUMMY_THREAD_CONTEXTS_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_thread_context_sanity(UVM_TEST_THREAD_CONTEXT_SANITY_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_thread_context_perf(UVM_TEST_THREAD_CONTEXT_PERF_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_tools_flush_replay_events(UVM_TEST_TOOLS_FLUSH_REPLAY_EVENTS_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_register_unload_state_buffer(UVM_TEST_REGISTER_UNLOAD_STATE_BUFFER_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_rb_tree_directed(UVM_TEST_RB_TREE_DIRECTED_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_rb_tree_random(UVM_TEST_RB_TREE_RANDOM_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_sec2_sanity(UVM_TEST_SEC2_SANITY_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_sec2_cpu_gpu_roundtrip(UVM_TEST_SEC2_CPU_GPU_ROUNDTRIP_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_cpu_chunk_api(UVM_TEST_CPU_CHUNK_API_PARAMS *params, struct file *filp);
#endif
