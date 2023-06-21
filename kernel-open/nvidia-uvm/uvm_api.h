/*******************************************************************************
    Copyright (c) 2015-2019 NVIDIA Corporation

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

#ifndef __UVM_API_H__
#define __UVM_API_H__

#include "uvm_types.h"
#include "uvm_common.h"
#include "uvm_ioctl.h"
#include "uvm_linux.h"
#include "uvm_lock.h"
#include "uvm_thread_context.h"
#include "uvm_kvmalloc.h"
#include "uvm_va_space.h"
#include "nv_uvm_types.h"

// This weird number comes from UVM_PREVENT_MIGRATION_RANGE_GROUPS_PARAMS. That
// ioctl is called frequently so we don't want to allocate a copy every time.
// It's a little over 256 bytes in size.
#define UVM_MAX_IOCTL_PARAM_STACK_SIZE 288

// The UVM_ROUTE_CMD_* macros are only intended for use in the ioctl routines

// If the BUILD_BUG_ON fires, use __UVM_ROUTE_CMD_ALLOC instead.
#define __UVM_ROUTE_CMD_STACK(cmd, params_type, function_name, do_init_check)       \
    case cmd:                                                                       \
    {                                                                               \
        params_type params;                                                         \
        BUILD_BUG_ON(sizeof(params) > UVM_MAX_IOCTL_PARAM_STACK_SIZE);              \
        if (nv_copy_from_user(&params, (void __user*)arg, sizeof(params)))          \
            return -EFAULT;                                                         \
                                                                                    \
        params.rmStatus = uvm_global_get_status();                                  \
        if (params.rmStatus == NV_OK) {                                             \
            if (do_init_check) {                                                    \
                if (!uvm_fd_va_space(filp))                                         \
                    params.rmStatus = NV_ERR_ILLEGAL_ACTION;                        \
            }                                                                       \
            if (likely(params.rmStatus == NV_OK))                                   \
                params.rmStatus = function_name(&params, filp);                     \
        }                                                                           \
                                                                                    \
        if (nv_copy_to_user((void __user*)arg, &params, sizeof(params)))            \
            return -EFAULT;                                                         \
                                                                                    \
        return 0;                                                                   \
    }

// We need to concatenate cmd##_PARAMS here to avoid the preprocessor's argument
// prescan. Attempting concatenation in the lower-level macro will fail because
// it will have been expanded to a literal by then.
#define UVM_ROUTE_CMD_STACK_NO_INIT_CHECK(cmd, function_name) \
    __UVM_ROUTE_CMD_STACK(cmd, cmd##_PARAMS, function_name, false)

#define UVM_ROUTE_CMD_STACK_INIT_CHECK(cmd, function_name) \
    __UVM_ROUTE_CMD_STACK(cmd, cmd##_PARAMS, function_name, true)

// If the BUILD_BUG_ON fires, use __UVM_ROUTE_CMD_STACK instead
#define __UVM_ROUTE_CMD_ALLOC(cmd, params_type, function_name, do_init_check)           \
    case cmd:                                                                           \
    {                                                                                   \
        int ret = 0;                                                                    \
        params_type *params = uvm_kvmalloc(sizeof(*params));                            \
        if (!params)                                                                    \
            return -ENOMEM;                                                             \
        BUILD_BUG_ON(sizeof(*params) <= UVM_MAX_IOCTL_PARAM_STACK_SIZE);                \
        if (nv_copy_from_user(params, (void __user*)arg, sizeof(*params))) {            \
            uvm_kvfree(params);                                                         \
            return -EFAULT;                                                             \
        }                                                                               \
                                                                                        \
        params->rmStatus = uvm_global_get_status();                                     \
        if (params->rmStatus == NV_OK) {                                                \
            if (do_init_check) {                                                        \
                if (!uvm_fd_va_space(filp))                                             \
                    params->rmStatus = NV_ERR_ILLEGAL_ACTION;                           \
            }                                                                           \
            if (likely(params->rmStatus == NV_OK))                                      \
                params->rmStatus = function_name(params, filp);                         \
        }                                                                               \
                                                                                        \
        if (nv_copy_to_user((void __user*)arg, params, sizeof(*params)))                \
            ret = -EFAULT;                                                              \
                                                                                        \
        uvm_kvfree(params);                                                             \
        return ret;                                                                     \
    }

#define UVM_ROUTE_CMD_ALLOC_NO_INIT_CHECK(cmd, function_name) \
    __UVM_ROUTE_CMD_ALLOC(cmd, cmd##_PARAMS, function_name, false)

#define UVM_ROUTE_CMD_ALLOC_INIT_CHECK(cmd, function_name) \
    __UVM_ROUTE_CMD_ALLOC(cmd, cmd##_PARAMS, function_name, true)

// Wrap an entry point into the UVM module.
//
// An entry function with signature
//
//    return_type foo(...);
//
// is required to have a counterpart of the form
//
//    return_type foo_entry(...) {
//        UVM_ENTRY_RET(foo(...));
//   }
//
// An entry function with signature
//
//    void foo(...);
//
// is required to have a counterpart of the form
//
//    void foo_entry(...) {
//        UVM_ENTRY_VOID(foo(...));
//   }
//
// Invocations of foo must be replaced by invocations of foo_entry at the entry
// points.
#define UVM_ENTRY_WRAP(line)                                                        \
    do {                                                                            \
        bool added;                                                                 \
                                                                                    \
        if (in_interrupt()) {                                                       \
            line;                                                                   \
        }                                                                           \
        else if (uvm_thread_context_wrapper_is_used()) {                            \
            uvm_thread_context_wrapper_t thread_context_wrapper;                    \
                                                                                    \
            added = uvm_thread_context_add(&thread_context_wrapper.context);        \
            line;                                                                   \
            if (added)                                                              \
                uvm_thread_context_remove(&thread_context_wrapper.context);         \
        }                                                                           \
        else {                                                                      \
            uvm_thread_context_t thread_context;                                    \
                                                                                    \
            added = uvm_thread_context_add(&thread_context);                        \
            line;                                                                   \
            if (added)                                                              \
                uvm_thread_context_remove(&thread_context);                         \
        }                                                                           \
    } while (0)                                                                     \

// Wrapper for non-void functions
#define UVM_ENTRY_RET(func_call)               \
    do {                                       \
        typeof(func_call) ret;                 \
        UVM_ENTRY_WRAP((ret = (func_call)));   \
        return ret;                            \
    } while (0)                                \

// Wrapper for void functions
#define UVM_ENTRY_VOID UVM_ENTRY_WRAP

// Validate input ranges from the user with specific alignment requirement
static bool uvm_api_range_invalid_aligned(NvU64 base, NvU64 length, NvU64 alignment)
{
    return !IS_ALIGNED(base, alignment)     ||
           !IS_ALIGNED(length, alignment)   ||
           base == 0                        ||
           length == 0                      ||
           base + length < base; // Overflow
}

// Most APIs require PAGE_SIZE alignment
static bool uvm_api_range_invalid(NvU64 base, NvU64 length)
{
    return uvm_api_range_invalid_aligned(base, length, PAGE_SIZE);
}

// Some APIs can only enforce 4K alignment as it's the smallest GPU page size
// even when the smallest host page is larger (e.g. 64K on ppc64le).
static bool uvm_api_range_invalid_4k(NvU64 base, NvU64 length)
{
    return uvm_api_range_invalid_aligned(base, length, UVM_PAGE_SIZE_4K);
}

// Verify alignment on a 64K boundary.
static bool uvm_api_range_invalid_64k(NvU64 base, NvU64 length)
{
    return uvm_api_range_invalid_aligned(base, length, UVM_PAGE_SIZE_64K);
}

typedef enum
{
    UVM_API_RANGE_TYPE_MANAGED,
    UVM_API_RANGE_TYPE_HMM,
    UVM_API_RANGE_TYPE_ATS,
    UVM_API_RANGE_TYPE_INVALID
} uvm_api_range_type_t;

// If the interval [base, base + length) is fully covered by VMAs which all have
// the same uvm_api_range_type_t, that range type is returned.
//
// LOCKING: va_space->lock must be held in at least read mode. If mm != NULL,
//          mm->mmap_lock must also be held in at least read mode.
uvm_api_range_type_t uvm_api_range_type_check(uvm_va_space_t *va_space, struct mm_struct *mm, NvU64 base, NvU64 length);

NV_STATUS uvm_api_pageable_mem_access_on_gpu(UVM_PAGEABLE_MEM_ACCESS_ON_GPU_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_register_gpu(UVM_REGISTER_GPU_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_unregister_gpu(UVM_UNREGISTER_GPU_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_create_range_group(UVM_CREATE_RANGE_GROUP_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_destroy_range_group(UVM_DESTROY_RANGE_GROUP_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_enable_peer_access(UVM_ENABLE_PEER_ACCESS_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_disable_peer_access(UVM_DISABLE_PEER_ACCESS_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_set_range_group(UVM_SET_RANGE_GROUP_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_create_external_range(UVM_CREATE_EXTERNAL_RANGE_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_map_external_allocation(UVM_MAP_EXTERNAL_ALLOCATION_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_map_external_sparse(UVM_MAP_EXTERNAL_SPARSE_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_free(UVM_FREE_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_prevent_migration_range_groups(UVM_PREVENT_MIGRATION_RANGE_GROUPS_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_allow_migration_range_groups(UVM_ALLOW_MIGRATION_RANGE_GROUPS_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_set_preferred_location(const UVM_SET_PREFERRED_LOCATION_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_unset_preferred_location(const UVM_UNSET_PREFERRED_LOCATION_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_set_accessed_by(const UVM_SET_ACCESSED_BY_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_unset_accessed_by(const UVM_UNSET_ACCESSED_BY_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_register_gpu_va_space(UVM_REGISTER_GPU_VASPACE_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_unregister_gpu_va_space(UVM_UNREGISTER_GPU_VASPACE_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_register_channel(UVM_REGISTER_CHANNEL_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_unregister_channel(UVM_UNREGISTER_CHANNEL_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_enable_read_duplication(const UVM_ENABLE_READ_DUPLICATION_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_disable_read_duplication(const UVM_DISABLE_READ_DUPLICATION_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_migrate(UVM_MIGRATE_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_enable_system_wide_atomics(UVM_ENABLE_SYSTEM_WIDE_ATOMICS_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_disable_system_wide_atomics(UVM_DISABLE_SYSTEM_WIDE_ATOMICS_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_tools_init_event_tracker(UVM_TOOLS_INIT_EVENT_TRACKER_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_tools_set_notification_threshold(UVM_TOOLS_SET_NOTIFICATION_THRESHOLD_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_tools_event_queue_enable_events(UVM_TOOLS_EVENT_QUEUE_ENABLE_EVENTS_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_tools_event_queue_disable_events(UVM_TOOLS_EVENT_QUEUE_DISABLE_EVENTS_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_tools_enable_counters(UVM_TOOLS_ENABLE_COUNTERS_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_tools_disable_counters(UVM_TOOLS_DISABLE_COUNTERS_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_tools_read_process_memory(UVM_TOOLS_READ_PROCESS_MEMORY_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_tools_write_process_memory(UVM_TOOLS_WRITE_PROCESS_MEMORY_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_map_dynamic_parallelism_region(UVM_MAP_DYNAMIC_PARALLELISM_REGION_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_unmap_external(UVM_UNMAP_EXTERNAL_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_migrate_range_group(UVM_MIGRATE_RANGE_GROUP_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_alloc_semaphore_pool(UVM_ALLOC_SEMAPHORE_POOL_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_populate_pageable(const UVM_POPULATE_PAGEABLE_PARAMS *params, struct file *filp);

#endif // __UVM_API_H__
