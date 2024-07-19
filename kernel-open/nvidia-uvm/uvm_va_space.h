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

#ifndef __UVM_VA_SPACE_H__
#define __UVM_VA_SPACE_H__

#include "uvm_processors.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_range_tree.h"
#include "uvm_range_group.h"
#include "uvm_forward_decl.h"
#include "uvm_mmu.h"
#include "uvm_linux.h"
#include "uvm_common.h"
#include "nv-kref.h"
#include "nv-linux.h"
#include "uvm_perf_events.h"
#include "uvm_perf_module.h"
#include "uvm_va_block_types.h"
#include "uvm_va_block.h"
#include "uvm_hmm.h"
#include "uvm_test_ioctl.h"
#include "uvm_ats.h"
#include "uvm_va_space_mm.h"
#include "uvm_conf_computing.h"

// uvm_deferred_free_object provides a mechanism for building and later freeing
// a list of objects which are owned by a VA space, but can't be freed while the
// VA space lock is held.

typedef enum
{
    UVM_DEFERRED_FREE_OBJECT_TYPE_CHANNEL,
    UVM_DEFERRED_FREE_OBJECT_GPU_VA_SPACE,
    UVM_DEFERRED_FREE_OBJECT_TYPE_EXTERNAL_ALLOCATION,
    UVM_DEFERRED_FREE_OBJECT_TYPE_COUNT
} uvm_deferred_free_object_type_t;

typedef struct
{
    uvm_deferred_free_object_type_t type;
    struct list_head list_node;
} uvm_deferred_free_object_t;

static void uvm_deferred_free_object_add(struct list_head *list,
                                         uvm_deferred_free_object_t *object,
                                         uvm_deferred_free_object_type_t type)
{
    object->type = type;
    list_add_tail(&object->list_node, list);
}

// Walks the list of pending objects and frees each one as appropriate to its
// type.
//
// LOCKING: May take the GPU isr_lock and the RM locks.
void uvm_deferred_free_object_list(struct list_head *deferred_free_list);

typedef enum
{
    // The GPU VA space has been initialized but not yet inserted into the
    // parent VA space.
    UVM_GPU_VA_SPACE_STATE_INIT = 0,

    // The GPU VA space is active in the VA space.
    UVM_GPU_VA_SPACE_STATE_ACTIVE,

    // The GPU VA space is no longer active in the VA space. This state can be
    // observed when threads retain the gpu_va_space then drop the VA space
    // lock. After re-taking the VA space lock, the state must be inspected to
    // see if another thread unregistered the gpu_va_space in the meantime.
    UVM_GPU_VA_SPACE_STATE_DEAD,

    UVM_GPU_VA_SPACE_STATE_COUNT
} uvm_gpu_va_space_state_t;

struct uvm_gpu_va_space_struct
{
    // Parent pointers
    uvm_va_space_t *va_space;
    uvm_gpu_t *gpu;

    uvm_gpu_va_space_state_t state;

    // Handle to the duped GPU VA space
    // to be used for all further GPU VA space related UVM-RM interactions.
    uvmGpuAddressSpaceHandle duped_gpu_va_space;
    bool did_set_page_directory;

    uvm_page_tree_t page_tables;

    // List of all uvm_user_channel_t's under this GPU VA space
    struct list_head registered_channels;

    // List of all uvm_va_range_t's under this GPU VA space with type ==
    // UVM_VA_RANGE_TYPE_CHANNEL. Used at channel registration time to find
    // shareable VA ranges without having to iterate through all VA ranges in
    // the VA space.
    struct list_head channel_va_ranges;

    // Boolean which is 1 if no new channel registration is allowed. This is set
    // when all the channels under the GPU VA space have been stopped to prevent
    // new ones from entering after we drop the VA space lock. It is an atomic_t
    // because multiple threads may set it to 1 concurrently.
    atomic_t disallow_new_channels;

    // Node for the deferred free list where this GPU VA space is stored upon
    // being unregistered.
    uvm_deferred_free_object_t deferred_free;

    // Reference count for this gpu_va_space. This only protects the memory
    // object itself, for use in cases when the gpu_va_space needs to be
    // accessed across dropping and re-acquiring the VA space lock.
    nv_kref_t kref;

    // ATS specific state
    uvm_ats_gpu_va_space_t ats;
};

typedef struct
{
    int                  numa_node;

    uvm_processor_mask_t gpus;
} uvm_cpu_gpu_affinity_t;

struct uvm_va_space_struct
{
    // Mask of gpus registered with the va space
    uvm_processor_mask_t registered_gpus;

    // Array of pointers to the uvm_gpu_t objects that correspond to the
    // uvm_processor_id_t index.
    //
    // With SMC, GPUs can be partitioned so the number of uvm_gpu_t objects can
    // be larger than UVM_ID_MAX_GPUS. However, each VA space can only
    // subscribe to a single partition per GPU, so it is fine to have a regular
    // processor mask.
    uvm_gpu_t *registered_gpus_table[UVM_ID_MAX_GPUS];

    // Mask of processors registered with the va space that support replayable
    // faults.
    uvm_processor_mask_t faultable_processors;

    // Mask of processors registered with the va space that don't support
    // faulting.
    uvm_processor_mask_t non_faultable_processors;

    // This is a count of non fault capable processors with a GPU VA space
    // registered.
    NvU32 num_non_faultable_gpu_va_spaces;

    // Semaphore protecting the state of the va space
    uvm_rw_semaphore_t lock;

    // Lock taken prior to taking the VA space lock in write mode, or prior to
    // taking the VA space lock in read mode on a path which will call in RM.
    // See UVM_LOCK_ORDER_VA_SPACE_SERIALIZE_WRITERS in uvm_lock.h.
    uvm_mutex_t serialize_writers_lock;

    // Lock taken to serialize down_reads on the VA space lock with up_writes in
    // other threads. See
    // UVM_LOCK_ORDER_VA_SPACE_READ_ACQUIRE_WRITE_RELEASE_LOCK in uvm_lock.h.
    uvm_mutex_t read_acquire_write_release_lock;

    // Tree of uvm_va_range_t's
    uvm_range_tree_t va_range_tree;

    // Kernel mapping structure passed to unmap_mapping range to unmap CPU PTEs
    // in this process.
    struct address_space *mapping;

    // Storage in g_uvm_global.va_spaces.list
    struct list_head list_node;

    // Monotonically increasing counter for range groups IDs
    atomic64_t range_group_id_counter;

    // Range groups
    struct radix_tree_root range_groups;
    uvm_range_tree_t range_group_ranges;

    // Peer to peer table
    // A bitmask of peer to peer pairs enabled in this va_space
    // indexed by a peer_table_index returned by uvm_gpu_peer_table_index().
    DECLARE_BITMAP(enabled_peers, UVM_MAX_UNIQUE_GPU_PAIRS);

    // Temporary copy of the above state used to avoid allocation during VA
    // space destroy.
    DECLARE_BITMAP(enabled_peers_teardown, UVM_MAX_UNIQUE_GPU_PAIRS);

    // Interpreting these processor masks:
    //      uvm_processor_mask_test(foo[A], B)
    // ...should be read as "test if A foo B." For example:
    //      uvm_processor_mask_test(accessible_from[B], A)
    // means "test if B is accessible_from A."

    // Pre-computed masks that contain, for each processor, a mask of processors
    // which that processor can directly access. In other words, this will test
    // whether A has direct access to B:
    //      uvm_processor_mask_test(can_access[A], B)
    uvm_processor_mask_t can_access[UVM_ID_MAX_PROCESSORS];

    // Pre-computed masks that contain, for each processor memory, a mask with
    // the processors that have direct access enabled to its memory. This is the
    // opposite direction as can_access. In other words, this will test whether
    // A has direct access to B:
    //      uvm_processor_mask_test(accessible_from[B], A)
    uvm_processor_mask_t accessible_from[UVM_ID_MAX_PROCESSORS];

    // Pre-computed masks that contain, for each processor memory, a mask with
    // the processors that can directly copy to and from its memory, using the
    // Copy Engine. These masks are usually the same as accessible_from masks.
    //
    // In certain configurations, peer identity mappings must be created to
    // enable CE copies between peers.
    uvm_processor_mask_t can_copy_from[UVM_ID_MAX_PROCESSORS];

    // Pre-computed masks that contain, for each processor, a mask of processors
    // to which that processor has NVLINK access. In other words, this will test
    // whether A has NVLINK access to B:
    //      uvm_processor_mask_test(has_nvlink[A], B)
    // This is a subset of can_access.
    uvm_processor_mask_t has_nvlink[UVM_ID_MAX_PROCESSORS];

    // Pre-computed masks that contain, for each processor memory, a mask with
    // the processors that have direct access to its memory and native support
    // for atomics in HW. This is a subset of accessible_from.
    uvm_processor_mask_t has_native_atomics[UVM_ID_MAX_PROCESSORS];

    // Mask of gpu_va_spaces registered with the va space
    // indexed by gpu->id
    uvm_processor_mask_t registered_gpu_va_spaces;

    // Mask of GPUs which have temporarily dropped the VA space lock mid-
    // unregister. Used to make other paths return an error rather than
    // corrupting state.
    uvm_processor_mask_t gpu_unregister_in_progress;

    // Mask of processors that are participating in system-wide atomics
    uvm_processor_mask_t system_wide_atomics_enabled_processors;

    // Temporary copy of registered_gpus used to avoid allocation during VA
    // space destroy.
    uvm_processor_mask_t registered_gpus_teardown;

    // Allocated in uvm_va_space_register_gpu(), used and free'd in
    // uvm_va_space_unregister_gpu().
    uvm_processor_mask_t *peers_to_release[UVM_ID_MAX_PROCESSORS];

    // Mask of processors to unmap. Used in range_unmap().
    uvm_processor_mask_t unmap_mask;

    // Available as scratch space for the internal APIs. This is like a caller-
    // save register: it shouldn't be used across function calls which also take
    // this va_space.
    uvm_processor_mask_t scratch_processor_mask;

    // Mask of physical GPUs where access counters are enabled on this VA space
    uvm_parent_processor_mask_t access_counters_enabled_processors;

    // Array with information regarding CPU/GPU NUMA affinity. There is one
    // entry per CPU NUMA node. Entries in the array are populated sequentially
    // as new CPU NUMA nodes are discovered on GPU registration. Each entry
    // contains a CPU NUMA node id, and a mask with the GPUs attached to it.
    // Since each GPU can only be attached to one CPU node id, the array can
    // contain information for up to UVM_ID_MAX_GPUS nodes. The information is
    // stored in the VA space to avoid taking the global lock.
    uvm_cpu_gpu_affinity_t gpu_cpu_numa_affinity[UVM_ID_MAX_GPUS];

    // Unregistering a GPU may trigger memory eviction from the GPU to the CPU.
    // This must happen without allocation, thus, a buffer is preallocated
    // at GPU register and freed at GPU unregister.
    uvm_conf_computing_dma_buffer_t *gpu_unregister_dma_buffer[UVM_ID_MAX_GPUS];

    // Array of GPU VA spaces
    uvm_gpu_va_space_t *gpu_va_spaces[UVM_ID_MAX_GPUS];

    // Tracking of GPU VA spaces which have dropped the VA space lock and are
    // pending destruction. uvm_va_space_mm_shutdown has to wait for those
    // destroy operations to be completely done.
    struct
    {
        atomic_t num_pending;
        wait_queue_head_t wait_queue;
    } gpu_va_space_deferred_free;

    // Per-va_space event notification information for performance heuristics
    uvm_perf_va_space_events_t perf_events;

    uvm_perf_module_data_desc_t perf_modules_data[UVM_PERF_MODULE_TYPE_COUNT];

    // Array of modules that are loaded in the va_space, indexed by module type
    uvm_perf_module_t *perf_modules[UVM_PERF_MODULE_TYPE_COUNT];

    // Lists of counters listening for events on this VA space
    // Protected by lock
    struct
    {
        bool enabled;

        uvm_rw_semaphore_t lock;

        // Lists of counters listening for events on this VA space
        struct list_head counters[UVM_TOTAL_COUNTERS];
        struct list_head queues_v1[UvmEventNumTypesAll];
        struct list_head queues_v2[UvmEventNumTypesAll];

        // Node for this va_space in global subscribers list
        struct list_head node;
    } tools;

    // Boolean which is 1 if all user channels have been already stopped. This
    // is an atomic_t because multiple threads may call
    // uvm_va_space_stop_all_user_channels concurrently.
    atomic_t user_channels_stopped;

    // Prevent future registrations of any kind (GPU, GPU VA space, channel).
    // This is used when the associated va_space_mm is torn down, which has to
    // prevent any new work from being started in this VA space.
    bool disallow_new_registers;

    bool user_channel_stops_are_immediate;

    // Block context used for GPU unmap operations so that allocation is not
    // required on the teardown path. This can only be used while the VA space
    // lock is held in write mode. Access using uvm_va_space_block_context().
    uvm_va_block_context_t *va_block_context;

    NvU64 initialization_flags;

    // The mm currently associated with this VA space, if any.
    uvm_va_space_mm_t va_space_mm;

    union
    {
        uvm_ats_va_space_t ats;

        // HMM information about this VA space.
        uvm_hmm_va_space_t hmm;
    };

    struct
    {
        // Temporary mask used to calculate closest_processors in
        // uvm_processor_mask_find_closest_id.
        uvm_processor_mask_t mask;

        // Protects the mask above.
        uvm_mutex_t mask_mutex;
    } closest_processors;

    struct
    {
        bool  page_prefetch_enabled;
        bool  skip_migrate_vma;

        atomic_t migrate_vma_allocation_fail_nth;

        atomic_t va_block_allocation_fail_nth;

        uvm_thread_context_wrapper_t *dummy_thread_context_wrappers;
        size_t num_dummy_thread_context_wrappers;

        atomic64_t destroy_gpu_va_space_delay_us;

        atomic64_t split_invalidate_delay_us;

        bool force_cpu_to_cpu_copy_with_ce;

        bool allow_allocation_from_movable;
    } test;

    // Queue item for deferred f_ops->release() handling
    nv_kthread_q_item_t deferred_release_q_item;
};

static uvm_gpu_t *uvm_va_space_get_gpu(uvm_va_space_t *va_space, uvm_gpu_id_t gpu_id)
{
    uvm_gpu_t *gpu;

    UVM_ASSERT(uvm_processor_mask_test(&va_space->registered_gpus, gpu_id));

    gpu = va_space->registered_gpus_table[uvm_id_gpu_index(gpu_id)];

    UVM_ASSERT(gpu);
    UVM_ASSERT(uvm_gpu_get(gpu->id) == gpu);

    return gpu;
}

static const char *uvm_va_space_processor_name(uvm_va_space_t *va_space, uvm_processor_id_t id)
{
    if (UVM_ID_IS_CPU(id))
        return "0: CPU";
    else
        return uvm_gpu_name(uvm_va_space_get_gpu(va_space, id));
}

static void uvm_va_space_processor_uuid(uvm_va_space_t *va_space, NvProcessorUuid *uuid, uvm_processor_id_t id)
{
    if (UVM_ID_IS_CPU(id)) {
        memcpy(uuid, &NV_PROCESSOR_UUID_CPU_DEFAULT, sizeof(*uuid));
    }
    else {
        uvm_gpu_t *gpu = uvm_va_space_get_gpu(va_space, id);
        UVM_ASSERT(gpu);
        memcpy(uuid, &gpu->uuid, sizeof(*uuid));
    }
}

static bool uvm_va_space_processor_has_memory(uvm_va_space_t *va_space, uvm_processor_id_t id)
{
    if (UVM_ID_IS_CPU(id))
        return true;

    return uvm_va_space_get_gpu(va_space, id)->mem_info.size > 0;
}

NV_STATUS uvm_va_space_create(struct address_space *mapping, uvm_va_space_t **va_space_ptr, NvU64 flags);
void uvm_va_space_destroy(uvm_va_space_t *va_space);

// All VA space locking should be done with these wrappers. They're macros so
// lock assertions are attributed to line numbers correctly.

#define uvm_va_space_down_write(__va_space)                             \
    do {                                                                \
        uvm_mutex_lock(&(__va_space)->serialize_writers_lock);          \
        uvm_mutex_lock(&(__va_space)->read_acquire_write_release_lock); \
        uvm_down_write(&(__va_space)->lock);                            \
    } while (0)

#define uvm_va_space_up_write(__va_space)                                   \
    do {                                                                    \
        uvm_up_write(&(__va_space)->lock);                                  \
        uvm_mutex_unlock(&(__va_space)->read_acquire_write_release_lock);   \
        uvm_mutex_unlock(&(__va_space)->serialize_writers_lock);            \
    } while (0)

#define uvm_va_space_downgrade_write(__va_space)                                        \
    do {                                                                                \
        uvm_downgrade_write(&(__va_space)->lock);                                       \
        uvm_mutex_unlock_out_of_order(&(__va_space)->read_acquire_write_release_lock);  \
        uvm_mutex_unlock_out_of_order(&(__va_space)->serialize_writers_lock);           \
    } while (0)

// Call this when holding the VA space lock for write in order to downgrade to
// read on a path which also needs to make RM calls.
#define uvm_va_space_downgrade_write_rm(__va_space)                                     \
    do {                                                                                \
        uvm_assert_mutex_locked(&(__va_space)->serialize_writers_lock);                 \
        uvm_downgrade_write(&(__va_space)->lock);                                       \
        uvm_mutex_unlock_out_of_order(&(__va_space)->read_acquire_write_release_lock);  \
    } while (0)

#define uvm_va_space_down_read(__va_space)                                              \
    do {                                                                                \
        uvm_mutex_lock(&(__va_space)->read_acquire_write_release_lock);                 \
        uvm_down_read(&(__va_space)->lock);                                             \
        uvm_mutex_unlock_out_of_order(&(__va_space)->read_acquire_write_release_lock);  \
    } while (0)

// Call this if RM calls need to be made while holding the VA space lock in read
// mode. Note that taking read_acquire_write_release_lock is unnecessary since
// the down_read is serialized with another thread's up_write by the
// serialize_writers_lock.
#define uvm_va_space_down_read_rm(__va_space)                           \
    do {                                                                \
        uvm_mutex_lock(&(__va_space)->serialize_writers_lock);          \
        uvm_down_read(&(__va_space)->lock);                             \
    } while (0)

#define uvm_va_space_up_read(__va_space) uvm_up_read(&(__va_space)->lock)

#define uvm_va_space_up_read_rm(__va_space)                             \
    do {                                                                \
        uvm_up_read(&(__va_space)->lock);                               \
        uvm_mutex_unlock(&(__va_space)->serialize_writers_lock);        \
    } while (0)

// Get a registered gpu by uuid. This restricts the search for GPUs, to those
// that have been registered with a va_space. This returns NULL if the GPU is
// not present, or not registered with the va_space.
//
// LOCKING: The VA space lock must be held.
uvm_gpu_t *uvm_va_space_get_gpu_by_uuid(uvm_va_space_t *va_space, const NvProcessorUuid *gpu_uuid);

// Like uvm_va_space_get_gpu_by_uuid, but also returns NULL if the GPU does
// not have a GPU VA space registered in the UVM va_space.
//
// LOCKING: The VA space lock must be held.
uvm_gpu_t *uvm_va_space_get_gpu_by_uuid_with_gpu_va_space(uvm_va_space_t *va_space, const NvProcessorUuid *gpu_uuid);

// Same as uvm_va_space_get_gpu_by_uuid but it also retains the GPU. The caller
// cannot assume that the GPU is still registered in the VA space after the
// function returns.
//
// LOCKING: The function takes and releases the VA space lock in read mode.
uvm_gpu_t *uvm_va_space_retain_gpu_by_uuid(uvm_va_space_t *va_space, const NvProcessorUuid *gpu_uuid);

// Returns whether read-duplication is supported.
// If gpu is NULL, returns the current state.
// otherwise, it returns what the result would be once the gpu's va space is
// added or removed (by inverting the gpu's current state).
bool uvm_va_space_can_read_duplicate(uvm_va_space_t *va_space, uvm_gpu_t *changing_gpu);

// Register a gpu in the va space
// Note that each gpu can be only registered once in a va space
//
// The input gpu_uuid is for the phyisical GPU. The user_rm_va_space argument
// identifies the SMC partition if provided and SMC is enabled.
//
// This call returns whether the GPU memory is a NUMA node in the kernel and the
// corresponding node id.
// It also returns the GI UUID (if gpu_uuid is a SMC partition) or a copy of
// gpu_uuid if the GPU is not SMC capable or SMC is not enabled.
NV_STATUS uvm_va_space_register_gpu(uvm_va_space_t *va_space,
                                    const NvProcessorUuid *gpu_uuid,
                                    const uvm_rm_user_object_t *user_rm_va_space,
                                    NvBool *numa_enabled,
                                    NvS32 *numa_node_id,
                                    NvProcessorUuid *uuid_out);

// Unregister a gpu from the va space
NV_STATUS uvm_va_space_unregister_gpu(uvm_va_space_t *va_space, const NvProcessorUuid *gpu_uuid);

// Registers a GPU VA space with the UVM VA space.
NV_STATUS uvm_va_space_register_gpu_va_space(uvm_va_space_t *va_space,
                                             uvm_rm_user_object_t *user_rm_va_space,
                                             const NvProcessorUuid *gpu_uuid);

// Unregisters a GPU VA space from the UVM VA space.
NV_STATUS uvm_va_space_unregister_gpu_va_space(uvm_va_space_t *va_space, const NvProcessorUuid *gpu_uuid);

// Stop all user channels
//
// This function sets a flag in the VA space indicating that all the channels
// have been already stopped and should only be used when no new user channels
// can be registered.
//
// LOCKING: The VA space lock must be held in read mode, not write.
void uvm_va_space_stop_all_user_channels(uvm_va_space_t *va_space);

// Calls uvm_user_channel_detach on all user channels in a VA space.
//
// The detached channels are added to the input list. The caller is expected to
// drop the VA space lock and call uvm_deferred_free_object_list to complete the
// destroy operation.
//
// LOCKING: The owning VA space must be locked in write mode.
void uvm_va_space_detach_all_user_channels(uvm_va_space_t *va_space, struct list_head *deferred_free_list);

// Returns whether peer access between these two GPUs has been enabled in this
// VA space. Both GPUs must be registered in the VA space.
bool uvm_va_space_peer_enabled(uvm_va_space_t *va_space, const uvm_gpu_t *gpu0, const uvm_gpu_t *gpu1);

// Returns the va_space this file points to. Returns NULL if this file
// does not point to a va_space.
static uvm_va_space_t *uvm_fd_va_space(struct file *filp)
{
    uvm_va_space_t *va_space;
    uvm_fd_type_t type;

    type = uvm_fd_type(filp, (void **) &va_space);
    if (type != UVM_FD_VA_SPACE)
        return NULL;

    return va_space;
}

static uvm_va_space_t *uvm_va_space_get(struct file *filp)
{
    uvm_fd_type_t fd_type;
    uvm_va_space_t *va_space;

    fd_type = uvm_fd_type(filp, (void **)&va_space);
    UVM_ASSERT(uvm_file_is_nvidia_uvm(filp));
    UVM_ASSERT_MSG(fd_type == UVM_FD_VA_SPACE, "filp: 0x%llx", (NvU64)filp);

    return va_space;
}

static uvm_va_block_context_t *uvm_va_space_block_context(uvm_va_space_t *va_space, struct mm_struct *mm)
{
    uvm_assert_rwsem_locked_write(&va_space->lock);
    if (mm)
        uvm_assert_mmap_lock_locked(mm);

    uvm_va_block_context_init(va_space->va_block_context, mm);
    return va_space->va_block_context;
}

// Retains the GPU VA space memory object. destroy_gpu_va_space and
// uvm_gpu_va_space_release drop the count. This is used to keep the GPU VA
// space object allocated when dropping and re-taking the VA space lock. If
// another thread called remove_gpu_va_space in the meantime,
// gpu_va_space->state will be UVM_GPU_VA_SPACE_STATE_DEAD.
static inline void uvm_gpu_va_space_retain(uvm_gpu_va_space_t *gpu_va_space)
{
    nv_kref_get(&gpu_va_space->kref);
}

// This only frees the GPU VA space object itself, so it must have been removed
// from its VA space and destroyed prior to the final release.
void uvm_gpu_va_space_release(uvm_gpu_va_space_t *gpu_va_space);

// Wrapper for nvUvmInterfaceUnsetPageDirectory
void uvm_gpu_va_space_unset_page_dir(uvm_gpu_va_space_t *gpu_va_space);

static uvm_gpu_va_space_state_t uvm_gpu_va_space_state(uvm_gpu_va_space_t *gpu_va_space)
{
    UVM_ASSERT(gpu_va_space->gpu);
    UVM_ASSERT(gpu_va_space->va_space);

    return gpu_va_space->state;
}

// Return the GPU VA space for the given GPU.
// Locking: the va_space lock must be held.
uvm_gpu_va_space_t *uvm_gpu_va_space_get(uvm_va_space_t *va_space, uvm_gpu_t *gpu);

#define for_each_gpu_va_space(__gpu_va_space, __va_space)                                                   \
    for (__gpu_va_space =                                                                                   \
            uvm_gpu_va_space_get(                                                                           \
                __va_space,                                                                                 \
                uvm_processor_mask_find_first_va_space_gpu(&__va_space->registered_gpu_va_spaces, va_space) \
            );                                                                                              \
         __gpu_va_space;                                                                                    \
         __gpu_va_space =                                                                                   \
            uvm_gpu_va_space_get(                                                                           \
                __va_space,                                                                                 \
                __uvm_processor_mask_find_next_va_space_gpu(&__va_space->registered_gpu_va_spaces,          \
                                                            va_space,                                       \
                                                            __gpu_va_space->gpu)                            \
            )                                                                                               \
        )

// Return the first GPU set in the given mask or NULL. The caller must ensure
// that the GPUs set in the mask are registered in the VA space and cannot be
// unregistered during this call.
static uvm_gpu_t *uvm_processor_mask_find_first_va_space_gpu(const uvm_processor_mask_t *mask, uvm_va_space_t *va_space)
{
    uvm_gpu_t *gpu;
    uvm_gpu_id_t gpu_id;

    UVM_ASSERT(uvm_processor_mask_gpu_subset(mask, &va_space->registered_gpus));

    gpu_id = uvm_processor_mask_find_first_gpu_id(mask);
    if (UVM_ID_IS_INVALID(gpu_id))
        return NULL;

    gpu = uvm_va_space_get_gpu(va_space, gpu_id);
    UVM_ASSERT_MSG(gpu, "gpu_id %u\n", uvm_id_value(gpu_id));

    return gpu;
}

static uvm_gpu_t *uvm_va_space_find_first_gpu(uvm_va_space_t *va_space)
{
    uvm_assert_rwsem_locked(&va_space->lock);

    return uvm_processor_mask_find_first_va_space_gpu(&va_space->registered_gpus, va_space);
}

// Same as uvm_processor_mask_find_next_va_space_gpu below, but gpu cannot be
// NULL
static uvm_gpu_t *__uvm_processor_mask_find_next_va_space_gpu(const uvm_processor_mask_t *mask,
                                                              uvm_va_space_t *va_space,
                                                              uvm_gpu_t *gpu)
{
    uvm_gpu_id_t gpu_id;

    UVM_ASSERT(gpu != NULL);
    UVM_ASSERT(uvm_processor_mask_gpu_subset(mask, &va_space->registered_gpus));

    gpu_id = uvm_processor_mask_find_next_id(mask, uvm_gpu_id_next(gpu->id));
    if (UVM_ID_IS_INVALID(gpu_id))
        return NULL;

    gpu = uvm_va_space_get_gpu(va_space, gpu_id);
    UVM_ASSERT_MSG(gpu, "gpu_id %u\n", uvm_id_value(gpu_id));

    return gpu;
}

// Return the next GPU with an id larger than gpu->id set in the given mask.
// The function returns NULL if gpu is NULL. The caller must ensure that the
// GPUs set in the mask are registered in the VA space and cannot be
// unregistered during this call.
static uvm_gpu_t *uvm_processor_mask_find_next_va_space_gpu(const uvm_processor_mask_t *mask,
                                                            uvm_va_space_t *va_space,
                                                            uvm_gpu_t *gpu)
{
    if (gpu == NULL)
        return NULL;

    return __uvm_processor_mask_find_next_va_space_gpu(mask, va_space, gpu);
}

#define for_each_va_space_gpu_in_mask(gpu, va_space, mask)                                       \
    for (({uvm_assert_rwsem_locked(&(va_space)->lock);                                           \
           gpu = uvm_processor_mask_find_first_va_space_gpu(mask, va_space);});                  \
           gpu != NULL;                                                                          \
           gpu = __uvm_processor_mask_find_next_va_space_gpu(mask, va_space, gpu))

// Helper to iterate over all GPUs registered in a UVM VA space
#define for_each_va_space_gpu(gpu, va_space) \
    for_each_va_space_gpu_in_mask(gpu, va_space, &(va_space)->registered_gpus)

// Return the processor in the candidates mask that is "closest" to src, or
// UVM_ID_MAX_PROCESSORS if candidates is empty. The order is:
// - src itself
// - Direct NVLINK GPU peers if src is CPU or GPU (1)
// - NVLINK CPU if src is GPU
// - PCIe peers if src is GPU (2)
// - CPU if src is GPU
// - Deterministic selection from the pool of candidates
//
// (1) When src is a GPU, NVLINK GPU peers are preferred over the CPU because in
//     NUMA systems the CPU processor may refer to multiple CPU NUMA nodes, and
//     the bandwidth between src and the farthest CPU node can be substantially
//     lower than the bandwidth src and its peer GPUs.
// (2) TODO: Bug 1764943: Is copying from a PCI peer always better than copying
//     from CPU?
uvm_processor_id_t uvm_processor_mask_find_closest_id(uvm_va_space_t *va_space,
                                                      const uvm_processor_mask_t *candidates,
                                                      uvm_processor_id_t src);

// Iterate over each ID in mask in order of proximity to src. This is
// destructive to mask.
#define for_each_closest_id(id, mask, src, va_space)                    \
    for (id = uvm_processor_mask_find_closest_id(va_space, mask, src);  \
         UVM_ID_IS_VALID(id);                                           \
         uvm_processor_mask_clear(mask, id), id = uvm_processor_mask_find_closest_id(va_space, mask, src))

// Return the GPU whose memory corresponds to the given node_id
static uvm_gpu_t *uvm_va_space_find_gpu_with_memory_node_id(uvm_va_space_t *va_space, int node_id)
{
    uvm_gpu_t *gpu;

    UVM_ASSERT(nv_numa_node_has_memory(node_id));

    if (!g_uvm_global.ats.supported)
        return NULL;

    for_each_va_space_gpu(gpu, va_space) {
        if (uvm_gpu_numa_node(gpu) == node_id)
            return gpu;
    }

    return NULL;
}

static bool uvm_va_space_memory_node_is_gpu(uvm_va_space_t *va_space, int node_id)
{
    return uvm_va_space_find_gpu_with_memory_node_id(va_space, node_id) != NULL;
}

// Return a processor mask with the GPUs attached to the node_id CPU memory
// node
static void uvm_va_space_get_gpus_attached_to_cpu_node(uvm_va_space_t *va_space,
                                                       int node_id,
                                                       uvm_processor_mask_t *gpus)
{
    uvm_gpu_id_t gpu_id;

    UVM_ASSERT(!uvm_va_space_memory_node_is_gpu(va_space, node_id));

    for_each_gpu_id(gpu_id) {
        const uvm_cpu_gpu_affinity_t *affinity = &va_space->gpu_cpu_numa_affinity[uvm_id_gpu_index(gpu_id)];
        if (affinity->numa_node == node_id) {
            uvm_processor_mask_copy(gpus, &affinity->gpus);
            return;
        }
    }

    uvm_processor_mask_zero(gpus);
}

// Helper that returns the first GPU in the mask returned by
// uvm_va_space_get_gpus_attached_to_cpu_node or NULL if empty
static uvm_gpu_t *uvm_va_space_find_first_gpu_attached_to_cpu_node(uvm_va_space_t *va_space, int node_id)
{
    uvm_processor_mask_t gpus;

    uvm_va_space_get_gpus_attached_to_cpu_node(va_space, node_id, &gpus);

    return uvm_processor_mask_find_first_va_space_gpu(&gpus, va_space);
}

// Obtain the user channel with the given instance_ptr. This is used during
// non-replayable fault service. This function needs to be called with the va
// space lock held in order to prevent channels from being removed.
uvm_user_channel_t *uvm_gpu_va_space_get_user_channel(uvm_gpu_va_space_t *gpu_va_space,
                                                      uvm_gpu_phys_address_t instance_ptr);

// Whether some form of pageable access (ATS, HMM) is supported by the system on
// this VA space. This does NOT check whether GPUs with pageable support are
// present, just whether system + VA space support exists.
bool uvm_va_space_pageable_mem_access_supported(uvm_va_space_t *va_space);

NV_STATUS uvm_test_get_pageable_mem_access_type(UVM_TEST_GET_PAGEABLE_MEM_ACCESS_TYPE_PARAMS *params,
                                                 struct file *filp);
NV_STATUS uvm_test_enable_nvlink_peer_access(UVM_TEST_ENABLE_NVLINK_PEER_ACCESS_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_disable_nvlink_peer_access(UVM_TEST_DISABLE_NVLINK_PEER_ACCESS_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_destroy_gpu_va_space_delay(UVM_TEST_DESTROY_GPU_VA_SPACE_DELAY_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_force_cpu_to_cpu_copy_with_ce(UVM_TEST_FORCE_CPU_TO_CPU_COPY_WITH_CE_PARAMS *params,
                                                 struct file *filp);
NV_STATUS uvm_test_va_space_allow_movable_allocations(UVM_TEST_VA_SPACE_ALLOW_MOVABLE_ALLOCATIONS_PARAMS *params,
                                                      struct file *filp);

// Handle a CPU fault in the given VA space for a managed allocation,
// performing any operations necessary to establish a coherent CPU mapping
// (migrations, cache invalidates, etc.).
//
// Locking:
//  - vma->vm_mm->mmap_lock must be held in at least read mode. Note, that
//    might not be the same as current->mm->mmap_lock.
// Returns:
// VM_FAULT_NOPAGE: if page was faulted in OK
//     (possibly or'ed with VM_FAULT_MAJOR if a migration was needed).
// VM_FAULT_OOM: if system memory wasn't available.
// VM_FAULT_SIGBUS: if a CPU mapping to fault_addr cannot be accessed,
//     for example because it's within a range group which is non-migratable.
vm_fault_t uvm_va_space_cpu_fault_managed(uvm_va_space_t *va_space,
                                          struct vm_area_struct *vma,
                                          struct vm_fault *vmf);

// Handle a CPU fault in the given VA space for a HMM allocation,
// performing any operations necessary to establish a coherent CPU mapping
// (migrations, cache invalidates, etc.).
//
// Locking:
//  - vma->vm_mm->mmap_lock must be held in at least read mode. Note, that
//    might not be the same as current->mm->mmap_lock.
// Returns:
// VM_FAULT_NOPAGE: if page was faulted in OK
//     (possibly or'ed with VM_FAULT_MAJOR if a migration was needed).
// VM_FAULT_OOM: if system memory wasn't available.
// VM_FAULT_SIGBUS: if a CPU mapping to fault_addr cannot be accessed.
vm_fault_t uvm_va_space_cpu_fault_hmm(uvm_va_space_t *va_space,
                                      struct vm_area_struct *vma,
                                      struct vm_fault *vmf);

#endif // __UVM_VA_SPACE_H__
