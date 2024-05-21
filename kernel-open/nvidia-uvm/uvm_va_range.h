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

#ifndef __UVM_VA_RANGE_H__
#define __UVM_VA_RANGE_H__

#include "uvm_linux.h"
#include "nv-kref.h"
#include "uvm_common.h"
#include "uvm_perf_module.h"
#include "uvm_processors.h"
#include "uvm_gpu.h"
#include "uvm_lock.h"
#include "uvm_va_space.h"
#include "uvm_range_tree.h"
#include "uvm_va_policy.h"
#include "uvm_test_ioctl.h"
#include "uvm_range_group.h"
#include "uvm_forward_decl.h"
#include "uvm_mmu.h"
#include "uvm_hal_types.h"
#include "uvm_mem.h"
#include "uvm_tracker.h"
#include "uvm_ioctl.h"

// VA Ranges are the UVM driver equivalent of Linux kernel vmas. They represent
// user allocations of any page-aligned size. We maintain these as a separate
// data structure from the vma tree for several reasons:
//
// 1) RM allocations mapped to the GPU by UVM don't have associated UVM vmas
//
// 2) We don't always have a separate reference on the vma's mm_struct, so we
//    can't always lock mmap_lock on paths where current->mm != vma->vm_mm.
//
// 3) HMM vmas aren't ours, so we can't use their vm_private_data pointers.
//
// The tree as a whole is protected by va_space->lock. Faults and mappings only
// need to take the lock in read mode.
// Modification of the range state (such as changes to logical permissions or
// location preferences) must take the lock in write mode.
//
// VA ranges with type == UVM_VA_RANGE_TYPE_MANAGED:
//     Each va_range is contained completely within a parent vma. There can be
//     multiple va_ranges under the same vma, but not vice versa. All VAs within
//     the va_range share the same policy state.
//
//     Each va_range is a collection of VA blocks. The VA blocks each have
//     individual locks, and they hold the current mapping and location state
//     for their block across all processors (CPU and all GPUs).
//
// VA ranges with type == UVM_VA_RANGE_TYPE_EXTERNAL:
//     These ranges track physical allocations made by RM. The UVM driver is
//     responsible for mapping them to the GPU(s), but not to the CPU. These
//     ranges do not support faulting nor migration, and they do not necessarily
//     correspond to valid vmas.
//
//     These ranges do not have blocks. All state (page tables, mapping handles,
//     etc) is maintained within the range.
//
// VA ranges with type == UVM_VA_RANGE_TYPE_CHANNEL:
//     These are similar to EXTERNAL ranges, except they represent internal
//     allocations required for user channels to operate (context save areas,
//     for example).
//
// VA ranges with type == UVM_VA_RANGE_TYPE_SKED_REFLECTED:
//     These ranges track special SKED reflected mappings required for CNP. The
//     mappings don't have any physical backing. They just use PTEs with a
//     special kind, see make_sked_reflected_pte_pascal() for an example of the
//     PTE encoding.
//     Notably the API that creates these ranges calls them "dynamic parallelism
//     regions", but we use "SKED reflected ranges" internally as it's more
//     descriptive.
//
// VA ranges with type == UVM_VA_RANGE_TYPE_SEMAPHORE_POOL:
//     These ranges track semaphore pool allocations. They are backed by sysmem,
//     and persistently mapped on the CPU and all GPUs (with registered VA
//     spaces) in a user VA space. The ranges are also mapped on UVM internal VA
//     space on the CPU and all registered GPUs.
//
//     These ranges do not have blocks.
//

// This enum must be kept in sync with UVM_TEST_VA_RANGE_TYPE in
// uvm_test_ioctl.h
typedef enum
{
    UVM_VA_RANGE_TYPE_INVALID = 0,
    UVM_VA_RANGE_TYPE_MANAGED,
    UVM_VA_RANGE_TYPE_EXTERNAL,
    UVM_VA_RANGE_TYPE_CHANNEL,
    UVM_VA_RANGE_TYPE_SKED_REFLECTED,
    UVM_VA_RANGE_TYPE_SEMAPHORE_POOL,
    UVM_VA_RANGE_TYPE_MAX
} uvm_va_range_type_t;

// Wrapper to protect access to VMA's vm_page_prot
typedef struct
{
    // Needed for creating CPU mappings on the va_range. Do not access this
    // directly, instead use uvm_va_range_vma and friends.
    struct vm_area_struct *vma;

    uvm_rw_semaphore_t lock;
} uvm_vma_wrapper_t;

// TODO: Bug 1733295. VA range types should really be inverted. Instead of
//       maintaining common node state with a union of structs, we should have
//       separate C types for each VA range type. Each type would embed a common
//       VA range node.
//
//       There's a lot of state in the top-level uvm_va_range_t struct below
//       which really belongs in the per-type structs (for example, blocks).
//       We're deferring that cleanup to the full refactor.

// va_range state when va_range.type == UVM_VA_RANGE_TYPE_MANAGED
typedef struct
{
    // This is null in the case of a zombie allocation. Zombie allocations are
    // created from unfreed allocations at termination of a process which used
    // UVM_INIT_FLAGS_MULTI_PROCESS_SHARING_MODE, when at least one other
    // process is sharing the UVM file descriptor.
    uvm_vma_wrapper_t *vma_wrapper;

    // Managed allocations only use this policy and never use the policy
    // stored in the va_block for HMM allocations.
    uvm_va_policy_t policy;

    uvm_perf_module_data_desc_t perf_modules_data[UVM_PERF_MODULE_TYPE_COUNT];
} uvm_va_range_managed_t;

typedef struct
{
    // GPU mapping the allocation. The GPU's RM address space is required when
    // releasing the handle.
    uvm_gpu_t *gpu;

    // RM handle to the physical allocation. This handle is dup'd into our client
    // once - on initial mapping of the external allocation. If the allocation is
    // ever split, its ref_count is incremented. The allocation is not released
    // until the ref_count drops to 0.
    NvHandle rm_handle;

    // Refcount for this handle/allocation. The refcount is used when external
    // ranges are split, resulting in two ranges using the same physical allocation.
    nv_kref_t ref_count;
} uvm_ext_gpu_mem_handle;

typedef struct
{
    uvm_range_tree_node_t node;

    // Handle to the physical user allocation dup'd into our client. This
    // prevents the allocation from being removed until we free it, even if the
    // user frees their handle without telling us.
    // This will be NULL for sparse mappings, which don't correspond to actual
    // allocations.
    uvm_ext_gpu_mem_handle *mem_handle;

    // Tracks completion of PTE writes on pt_range_vec. The tree lock
    // protecting this ext_gpu_map may be dropped before those writes are
    // complete, so subsequent operations on this ext_gpu_map must acquire this
    // tracker before operating on pt_range_vec.
    uvm_tracker_t tracker;

    // GPU on which this allocation is mapped.
    uvm_gpu_t *gpu;

    // GPU which owns the allocation. For sysmem, this is the GPU that the
    // sysmem was originally allocated under. For the allocation to remain valid
    // we need to prevent the GPU from going away, similarly to P2P mapped
    // memory and to EGM memory.
    //
    // This field is not used for sparse mappings as they don't have an
    // allocation and, hence, owning GPU.
    //
    // TODO: Bug 1811006: The semantics of sysmem might change depending on the
    // resolution of this bug.
    //
    // TODO: Bug 1757136: For SLI, this is any GPU in the SLI group. We may need
    //       to handle that specially.
    uvm_gpu_t *owning_gpu;

    // We need to know whether this memory is actually located on owning_gpu so
    // we know what type of membar is needed at TLB invalidate time, and to know
    // if the mapping GPU has to be unmapped on UvmDisablePeerAccess.
    //
    // This field is not used for sparse mappings as they don't have physical
    // backing.
    bool is_sysmem;

    // EGM memory. If true is_sysmem also has to be true and owning_gpu
    // has to be valid.
    bool is_egm;

    // GPU page tables mapping the allocation
    uvm_page_table_range_vec_t pt_range_vec;

    // Node for the deferred free list where this allocation is stored upon
    // unmapped.
    //
    // This field is unused for sparse mappings. Since they don't have physical
    // backing there is no RM object to be freed when the mapping is unmapped.
    uvm_deferred_free_object_t deferred_free;
} uvm_ext_gpu_map_t;

typedef struct
{
    // Lock protecting the range tree.
    uvm_mutex_t lock;

    // Range tree that contains all of the mapped portions of an External VA
    // range. The tree holds uvm_ext_gpu_map_t instances.
    uvm_range_tree_t tree;
} uvm_ext_gpu_range_tree_t;

typedef struct
{
    // Mask of GPUs which have mappings to this VA range. If a bit in this mask
    // is set, the corresponding pointer in gpu_ranges is valid.
    // The bitmap can be safely accessed by following the locking rules:
    //   * If the VA space lock is held for write, the mask can be read or written
    //     normally.
    //   * If the VA space lock is held for read, and one of the range tree locks is
    //     held, only the bit corresponding to that GPU range tree can be accessed.
    //     Writes must use uvm_processor_mask_set_atomic and
    //     uvm_processor_mask_clear_atomic to avoid clobbering other bits in the
    //     mask. If no range tree lock is held, the mask cannot be accessed.
    //   * If the VA space lock is not held, the mask cannot be accessed
    uvm_processor_mask_t mapped_gpus;

    // Per-GPU tree of mapped external allocations. This has to be per-GPU in the VA
    // range because each GPU is able to map a completely different set of
    // allocations to the same VA range.
    uvm_ext_gpu_range_tree_t gpu_ranges[UVM_ID_MAX_GPUS];

    // Dynamically allocated page mask allocated in
    // uvm_va_range_create_external() and used and freed in uvm_free().
    uvm_processor_mask_t *retained_mask;
} uvm_va_range_external_t;

// va_range state when va_range.type == UVM_VA_RANGE_TYPE_CHANNEL. This
// represents a channel buffer resource and mapping.
typedef struct
{
    // Only a single GPU can map a channel resource, so we only need one GPU
    // VA space parent.
    uvm_gpu_va_space_t *gpu_va_space;

    // Page tables mapped by this range
    uvm_page_table_range_vec_t pt_range_vec;

    // Physical location of this channel resource. All pages have the same
    // aperture.
    uvm_aperture_t aperture;

    // Note that this is not a normal RM object handle. It is a non-zero opaque
    // identifier underneath the GPU VA space which represents this channel
    // resource. Each channel using this VA range has retained this descriptor
    // and is responsible for releasing it. That's safe because channels outlive
    // their VA ranges.
    NvP64 rm_descriptor;

    // This is an ID assigned by RM to each resource descriptor.
    NvU32 rm_id;

    // The TSG which owns this mapping. Sharing of VA ranges is only allowed
    // within the same TSG. If valid == false, no sharing is allowed because the
    // channel is not in a TSG.
    struct
    {
        bool valid;
        NvU32 id;
    } tsg;

    NvU64 ref_count;

    // Storage in the corresponding uvm_gpu_va_space's channel_va_ranges list
    struct list_head list_node;
} uvm_va_range_channel_t;

// va_range state when va_range.type == UVM_VA_RANGE_TYPE_SKED_REFLECTED. This
// represents a sked reflected mapping.
typedef struct
{
    // Each SKED reflected range is unique to a single GPU so only a single GPU
    // VA space needs to be tracked.
    uvm_gpu_va_space_t *gpu_va_space;

    // Page tables mapped by this range
    uvm_page_table_range_vec_t pt_range_vec;
} uvm_va_range_sked_reflected_t;

typedef struct
{
    uvm_mem_t *mem;

    // The optional owner is a GPU (at most one) that has the allocation cached -
    // in this case, all writes must be done from this GPU.
    // protected by va_space lock
    uvm_gpu_t *owner;

    // Per-gpu attributes
    uvm_mem_gpu_mapping_attrs_t gpu_attrs[UVM_ID_MAX_GPUS];

    // Default attributes to assign when a new GPU is registered
    uvm_mem_gpu_mapping_attrs_t default_gpu_attrs;

    // Tracks all outstanding GPU work using this allocation.
    uvm_tracker_t tracker;
    uvm_mutex_t tracker_lock;
} uvm_va_range_semaphore_pool_t;

struct uvm_va_range_struct
{
    // Parent uvm_va_space.
    uvm_va_space_t *va_space;

    // Storage in VA range tree. Also contains range start and end.
    // start and end + 1 have to be PAGE_SIZED aligned.
    uvm_range_tree_node_t node;

    // Force the next split on this range to fail. Set by error injection ioctl
    // (testing purposes only).
    bool inject_split_error;

    // Force the next register_gpu_va_space to fail while adding this va_range.
    // Set by error injection ioctl (testing purposes only).
    bool inject_add_gpu_va_space_error;

    // Mask of UVM-Lite GPUs for the VA range
    //
    // If the preferred location is set to a non-faultable GPU or the CPU,
    // this mask contains all non-faultable GPUs that are in the accessed by
    // mask and the preferred location itself if it's a GPU. Empty otherwise.
    //
    // All UVM-Lite GPUs have mappings only to the preferred location. The
    // mappings are initially established only when the pages are resident on
    // the preferred location, but persist after that until the preferred
    // location is changed or a GPU stops being a UVM-Lite GPU.
    uvm_processor_mask_t uvm_lite_gpus;

    // This is a uvm_va_block_t ** array of all VA block pointers under this
    // range. The pointers can be accessed using the functions
    // uvm_va_range_block() and uvm_va_range_block_create(). The latter
    // allocates the block if it doesn't already exist. Once allocated, the
    // blocks persist in the array until the parent VA range is destroyed.
    //
    // Concurrent on-demand allocation requires the use of either atomics or a
    // spin lock. Given that we don't want to take a spin lock for every lookup,
    // and that the blocks are persistent, atomics are preferred.
    //
    // The number of blocks is calculated from the range size using
    // uvm_va_range_num_blocks().
    //
    // TODO: Bug 1766585: Compare perf of up-front allocation and demand-
    //       allocation of blocks in the common case (lots of accessed blocks)
    //       and the sparse case. If the common case is hurt by demand-
    //       allocation, or if the sparse case isn't helped much, just allocate
    //       them all at range allocation.
    atomic_long_t *blocks;

    uvm_va_range_type_t type;
    union
    {
        uvm_va_range_managed_t managed;
        uvm_va_range_external_t external;
        uvm_va_range_channel_t channel;
        uvm_va_range_sked_reflected_t sked_reflected;
        uvm_va_range_semaphore_pool_t semaphore_pool;
    };
};

// Module load/exit
NV_STATUS uvm_va_range_init(void);
void uvm_va_range_exit(void);

static NvU64 uvm_va_range_size(uvm_va_range_t *va_range)
{
    return uvm_range_tree_node_size(&va_range->node);
}

static bool uvm_va_range_is_aligned(uvm_va_range_t *va_range, NvU64 alignment)
{
    return IS_ALIGNED(va_range->node.start, alignment) && IS_ALIGNED(uvm_va_range_size(va_range), alignment);
}

static bool uvm_va_range_is_managed_zombie(uvm_va_range_t *va_range)
{
    return va_range->type == UVM_VA_RANGE_TYPE_MANAGED && va_range->managed.vma_wrapper == NULL;
}

// Create a va_range with type UVM_VA_RANGE_TYPE_MANAGED. The out va_range pointer
// is optional.
//
// Returns NV_ERR_UVM_ADDRESS_IN_USE if the vma overlaps with an existing range
// in the va_space tree.
NV_STATUS uvm_va_range_create_mmap(uvm_va_space_t *va_space,
                                   struct mm_struct *mm,
                                   uvm_vma_wrapper_t *vma_wrapper,
                                   uvm_va_range_t **out_va_range);

// Create a va_range with type UVM_VA_RANGE_TYPE_EXTERNAL. The out va_range
// pointer is optional.
//
// Returns NV_ERR_UVM_ADDRESS_IN_USE if the range overlaps with an existing
// range in the va_space tree.
NV_STATUS uvm_va_range_create_external(uvm_va_space_t *va_space,
                                       struct mm_struct *mm,
                                       NvU64 start,
                                       NvU64 length,
                                       uvm_va_range_t **out_va_range);

// Create a va_range with type UVM_VA_RANGE_TYPE_CHANNEL. The out va_range
// pointer is optional.
//
// Returns NV_ERR_UVM_ADDRESS_IN_USE if the range overlaps with an existing
// range in the va_space tree.
NV_STATUS uvm_va_range_create_channel(uvm_va_space_t *va_space,
                                      struct mm_struct *mm,
                                      NvU64 start,
                                      NvU64 end,
                                      uvm_va_range_t **out_va_range);

NV_STATUS uvm_va_range_create_sked_reflected(uvm_va_space_t *va_space,
                                             struct mm_struct *mm,
                                             NvU64 start,
                                             NvU64 length,
                                             uvm_va_range_t **out_va_range);

NV_STATUS uvm_va_range_create_semaphore_pool(uvm_va_space_t *va_space,
                                             struct mm_struct *mm,
                                             NvU64 start,
                                             NvU64 length,
                                             const UvmGpuMappingAttributes *per_gpu_attrs,
                                             NvU32 per_gpu_attrs_count,
                                             uvm_va_range_t **out_va_range);

// Destroys any state associated with this VA range, removes the VA range from
// the VA space, and frees the VA range.
//
// deferred_free_list may be NULL if the VA range type is known to not require
// deferred free. Otherwise this function adds entries to the list for later
// processing by uvm_deferred_free_object_list.
void uvm_va_range_destroy(uvm_va_range_t *va_range, struct list_head *deferred_free_list);

void uvm_va_range_zombify(uvm_va_range_t *va_range);

NV_STATUS uvm_api_clean_up_zombie_resources(UVM_CLEAN_UP_ZOMBIE_RESOURCES_PARAMS *params, struct file *filp);
NV_STATUS uvm_api_validate_va_range(UVM_VALIDATE_VA_RANGE_PARAMS *params, struct file *filp);

// Inform the VA range that a GPU VA space is now available for them to map, if
// the VA range is supposed to proactively map GPUs (UvmAllocSemaphorePool,
// UvmSetAccessedBy).
//
// If mm != NULL, that mm is used for any CPU mappings which may be created as
// a result of this call. See uvm_va_block_context_t::mm for details.
//
// LOCKING: If mm != NULL, the caller must hold mm->mmap_lock in at least read
//          mode.
NV_STATUS uvm_va_range_add_gpu_va_space(uvm_va_range_t *va_range,
                                        uvm_gpu_va_space_t *gpu_va_space,
                                        struct mm_struct *mm);

// Destroy the VA range's mappings on the GPU, if it has any
//
// If mm != NULL, that mm is used for any CPU mappings which may be created as
// a result of this call. See uvm_va_block_context_t::mm for details.
//
// LOCKING: If mm != NULL, the caller must hold mm->mmap_lock in at least read
//          mode.
void uvm_va_range_remove_gpu_va_space(uvm_va_range_t *va_range,
                                      uvm_gpu_va_space_t *gpu_va_space,
                                      struct mm_struct *mm,
                                      struct list_head *deferred_free_list);

// Inform the VA range that peer mappings can now be established between the
// GPUs, if the VA range is supposed to proactively create them (UvmSetAccessedBy).
NV_STATUS uvm_va_range_enable_peer(uvm_va_range_t *va_range, uvm_gpu_t *gpu0, uvm_gpu_t *gpu1);

// Unmap all page tables in this VA range which have peer mappings between these
// two GPUs, in either direction.
void uvm_va_range_disable_peer(uvm_va_range_t *va_range,
                               uvm_gpu_t *gpu0,
                               uvm_gpu_t *gpu1,
                               struct list_head *deferred_free_list);

// Notify the VA range of a newly registered GPU.
//
// LOCKING: the lock of the enclosing VA space is held in R/W mode
NV_STATUS uvm_va_range_register_gpu(uvm_va_range_t *va_range, uvm_gpu_t *gpu);

// Unmap all page tables in this VA range which map memory owned by this GPU.
// Managed ranges will have any memory still resident on this GPU evicted to
// system memory.
//
// deferred_free_list may be NULL if the VA range type is known to not require
// deferred free. Otherwise this function adds entries to the list for later
// processing by uvm_deferred_free_object_list.
//
// If mm != NULL, that mm is used for any CPU mappings which may be created as
// a result of this call. See uvm_va_block_context_t::mm for details.
//
// LOCKING: If mm != NULL, the caller must hold mm->mmap_lock in at least read
//          mode.
void uvm_va_range_unregister_gpu(uvm_va_range_t *va_range,
                                 uvm_gpu_t *gpu,
                                 struct mm_struct *mm,
                                 struct list_head *deferred_free_list);

// Splits existing_va_range into two pieces, with new_va_range always after
// existing. existing is updated to have new_end. new_end+1 must be page-
// aligned.
//
// Before: [----------- existing ------------]
// After:  [---- existing ----][---- new ----]
//                            ^new_end
//
// On error, existing_va_range is still accessible and is left in its original
// functional state.
//
// The va_range must have type UVM_VA_RANGE_TYPE_MANAGED.
NV_STATUS uvm_va_range_split(uvm_va_range_t *existing_va_range,
                             NvU64 new_end,
                             uvm_va_range_t **new_va_range);

// TODO: Bug 1707562: Merge va ranges

static uvm_va_range_t *uvm_va_range_container(uvm_range_tree_node_t *node)
{
    if (!node)
        return NULL;
    return container_of(node, uvm_va_range_t, node);
}

// Returns the va_range containing addr, if any
uvm_va_range_t *uvm_va_range_find(uvm_va_space_t *va_space, NvU64 addr);

static uvm_ext_gpu_map_t *uvm_ext_gpu_map_container(uvm_range_tree_node_t *node)
{
    if (!node)
        return NULL;
    return container_of(node, uvm_ext_gpu_map_t, node);
}

// Iterators for all va_ranges

#define uvm_for_each_va_range(va_range, va_space) \
    list_for_each_entry((va_range), &(va_space)->va_range_tree.head, node.list)

#define uvm_for_each_va_range_safe(va_range, va_range_next, va_space) \
    list_for_each_entry_safe((va_range), (va_range_next), &(va_space)->va_range_tree.head, node.list)


// Iterators for specific ranges

// Returns the first va_range in the range [start, end], if any
uvm_va_range_t *uvm_va_space_iter_first(uvm_va_space_t *va_space, NvU64 start, NvU64 end);

// Returns the va_range following the provided va_range in address order, if
// that va_range's start <= the provided end.
uvm_va_range_t *uvm_va_space_iter_next(uvm_va_range_t *va_range, NvU64 end);

// Like uvm_va_space_iter_next, but also returns NULL if the next va_range
// is not adjacent to the provided va_range.
static uvm_va_range_t *uvm_va_space_iter_next_contig(uvm_va_range_t *va_range, NvU64 end)
{
    uvm_va_range_t *next = uvm_va_space_iter_next(va_range, end);
    if (next && next->node.start != va_range->node.end + 1)
        return NULL;
    return next;
}

// Returns whether the range [start, end] has any VA ranges within it
static bool uvm_va_space_range_empty(uvm_va_space_t *va_space, NvU64 start, NvU64 end)
{
    return uvm_va_space_iter_first(va_space, start, end) == NULL;
}

#define uvm_for_each_va_range_in(va_range, va_space, start, end)            \
    for ((va_range) = uvm_va_space_iter_first((va_space), (start), (end));  \
         (va_range);                                                        \
         (va_range) = uvm_va_space_iter_next((va_range), (end)))

#define uvm_for_each_va_range_in_safe(va_range, va_range_next, va_space, start, end)    \
    for ((va_range) = uvm_va_space_iter_first((va_space), (start), (end)),              \
             (va_range_next) = uvm_va_space_iter_next((va_range), (end));               \
         (va_range);                                                                    \
         (va_range) = (va_range_next), (va_range_next) = uvm_va_space_iter_next((va_range), (end)))

// Iterator for all contiguous VA ranges between [start, end]. If any part of
// [start, end] is not covered by a VA range, iteration stops.
#define uvm_for_each_va_range_in_contig(va_range, va_space, start, end)         \
    for ((va_range) = uvm_va_space_iter_first((va_space), (start), (start));    \
         (va_range);                                                            \
         (va_range) = uvm_va_space_iter_next_contig((va_range), (end)))

#define uvm_for_each_va_range_in_contig_from(va_range, va_space, first_va_range, end) \
    for ((va_range) = (first_va_range);                                               \
         (va_range);                                                                  \
         (va_range) = uvm_va_space_iter_next_contig((va_range), (end)))

// Like uvm_for_each_va_range_in_contig but also stops iteration if any VA range
// has a type other than UVM_VA_RANGE_TYPE_MANAGED.
#define uvm_for_each_managed_va_range_in_contig(va_range, va_space, start, end) \
    for ((va_range) = uvm_va_space_iter_first((va_space), (start), (start));    \
         (va_range) && (va_range)->type == UVM_VA_RANGE_TYPE_MANAGED;           \
         (va_range) = uvm_va_space_iter_next_contig((va_range), (end)))

#define uvm_for_each_va_range_in_vma(va_range, vma)             \
    uvm_for_each_va_range_in(va_range,                          \
                             uvm_va_space_get(vma->vm_file),    \
                             vma->vm_start,                     \
                             vma->vm_end - 1)

#define uvm_for_each_va_range_in_vma_safe(va_range, va_range_next, vma) \
    uvm_for_each_va_range_in_safe(va_range,                             \
                                  va_range_next,                        \
                                  uvm_va_space_get(vma->vm_file),       \
                                  vma->vm_start,                        \
                                  vma->vm_end - 1)

// Only call this if you're sure that either:
// 1) You have a reference on the vma's vm_mm and that vma->vm_mm's mmap_lock is
//    held; or
// 2) You won't be operating on the vma (as with vm_insert_page) or accessing
//    any fields in the vma that can change without va_space->lock being held
//    (such as vm_flags).
//
// Otherwise, use uvm_va_range_vma_current or uvm_va_range_vma_check and be
// prepared to handle a NULL return value.
static struct vm_area_struct *uvm_va_range_vma(uvm_va_range_t *va_range)
{
    struct vm_area_struct *vma;
    UVM_ASSERT_MSG(va_range->type == UVM_VA_RANGE_TYPE_MANAGED, "type: %d", va_range->type);
    UVM_ASSERT(va_range->managed.vma_wrapper);

    uvm_assert_rwsem_locked(&va_range->va_space->lock);

    // vm_file, vm_private_data, vm_start, and vm_end are all safe to access
    // here because they can't change without the kernel calling vm_ops->open
    // or vm_ops->close, which both take va_space->lock.
    vma = va_range->managed.vma_wrapper->vma;
    UVM_ASSERT(vma);
    UVM_ASSERT_MSG(vma->vm_private_data == va_range->managed.vma_wrapper,
                   "vma: 0x%llx [0x%lx, 0x%lx] has vm_private_data 0x%llx\n",
                   (NvU64)vma,
                   vma->vm_start,
                   vma->vm_end - 1,
                   (NvU64)vma->vm_private_data);
    UVM_ASSERT_MSG(va_range->va_space == uvm_va_space_get(vma->vm_file),
                   "va_range va_space: 0x%llx vm_file: 0x%llx vm_file va_space: 0x%llx",
                   (NvU64)va_range->va_space,
                   (NvU64)vma->vm_file,
                   (NvU64)uvm_va_space_get(vma->vm_file));
    UVM_ASSERT_MSG(va_range->node.start >= vma->vm_start,
                   "Range mismatch: va_range: [0x%llx, 0x%llx] vma: [0x%lx, 0x%lx]\n",
                   va_range->node.start,
                   va_range->node.end,
                   vma->vm_start,
                   vma->vm_end - 1);
    UVM_ASSERT_MSG(va_range->node.end <= vma->vm_end - 1,
                   "Range mismatch: va_range: [0x%llx, 0x%llx] vma: [0x%lx, 0x%lx]\n",
                   va_range->node.start,
                   va_range->node.end,
                   vma->vm_start,
                   vma->vm_end - 1);

    return vma;
}

// Check that the VA range's vma is safe to use under mm. If not, NULL is
// returned. If the vma is returned, there must be a reference on mm and
// mm->mmap_lock must be held.
static struct vm_area_struct *uvm_va_range_vma_check(uvm_va_range_t *va_range, struct mm_struct *mm)
{
    struct vm_area_struct *vma;

    UVM_ASSERT_MSG(va_range->type == UVM_VA_RANGE_TYPE_MANAGED, "type: %d\n", va_range->type);

    // Zombies don't have a vma_wrapper.
    if (!va_range->managed.vma_wrapper)
        return NULL;

    vma = uvm_va_range_vma(va_range);

    // Examples of mm on various paths:
    //  - CPU fault         vma->vm_mm
    //  - GPU fault         current->mm or va_space->va_space_mm.mm
    //  - IOCTL             current->mm or va_space->va_space_mm.mm
    //  - Process teardown  NULL
    //
    // Since the "safe" mm varies based on the path, we may not have a reference
    // on the vma's owning mm_struct. We won't know that until we look at the
    // vma. By then it's too late to take mmap_lock since mmap_lock is above the
    // va_space lock in our lock ordering, and we must be holding the va_space
    // lock to query the va_range. Hence the need to detect the cases in which
    // it's safe to operate on the vma.
    //
    // When we can't detect for certain that mm is safe to use, we shouldn't
    // operate on the vma at all. The vma can't be outright freed until we drop
    // the va_space lock so the pointer itself will remain valid, but its fields
    // (like vm_start and vm_end) could be modified behind our back. We also
    // aren't allowed to call vm_insert_page unless we hold the vma's mmap_lock.
    //
    // Note that if uvm_va_space_mm_enabled() is true, then vma->vm_mm must be
    // va_space->va_space_mm.mm because we enforce that at mmap.
    //
    // An interesting case is when vma->vm_mm != current->mm. This can happen
    // due to fork, ptrace, process teardown, etc. It will also be the case in
    // the GPU fault handler.
    if (mm != vma->vm_mm)
        return NULL;

    uvm_assert_mmap_lock_locked(vma->vm_mm);
    return vma;
}

// Helper for use when the only mm which is known is current->mm
static struct vm_area_struct *uvm_va_range_vma_current(uvm_va_range_t *va_range)
{
    return uvm_va_range_vma_check(va_range, current->mm);
}

// Returns the maximum number of VA blocks which could be contained with the
// given va_range (number of elements in the va_range->blocks array).
// va_range->node.start and .end must be set.
//
// The va_range must have type UVM_VA_RANGE_TYPE_MANAGED.
size_t uvm_va_range_num_blocks(uvm_va_range_t *va_range);

// Get the index within the va_range->blocks array of the VA block
// corresponding to addr. The block pointer is not guaranteed to be valid. Use
// either uvm_va_range_block or uvm_va_range_block_create to look up the block.
//
// The va_range must have type UVM_VA_RANGE_TYPE_MANAGED.
size_t uvm_va_range_block_index(uvm_va_range_t *va_range, NvU64 addr);

// Looks up the VA block at va_range->blocks[index]. If no block is present at
// that index, NULL is returned.
//
// The va_range must have type UVM_VA_RANGE_TYPE_MANAGED.
static uvm_va_block_t *uvm_va_range_block(uvm_va_range_t *va_range, size_t index)
{
    UVM_ASSERT(va_range->type == UVM_VA_RANGE_TYPE_MANAGED);
    UVM_ASSERT(index < uvm_va_range_num_blocks(va_range));
    uvm_assert_rwsem_locked(&va_range->va_space->lock);

    return (uvm_va_block_t *)atomic_long_read(&va_range->blocks[index]);
}

// Same as uvm_va_range_block except that the block is created if not already
// present in the array. If NV_OK is returned, the block has been allocated
// successfully.
//
// The va_range must have type UVM_VA_RANGE_TYPE_MANAGED.
NV_STATUS uvm_va_range_block_create(uvm_va_range_t *va_range, size_t index, uvm_va_block_t **out_block);

// Returns the first populated VA block in the VA range after the input
// va_block, or NULL if none. If the input va_block is NULL, this returns the
// first VA block in the VA range, if any exists.
uvm_va_block_t *uvm_va_range_block_next(uvm_va_range_t *va_range, uvm_va_block_t *va_block);

// Iterate over populated VA blocks in the range. Does not create new VA blocks.
#define for_each_va_block_in_va_range(__va_range, __va_block)           \
    for (__va_block = uvm_va_range_block_next(__va_range, NULL);        \
         __va_block;                                                    \
         __va_block = uvm_va_range_block_next(__va_range, __va_block))

// Iterate over populated VA blocks in the range. Does not create new VA blocks. Safe version
#define for_each_va_block_in_va_range_safe(__va_range, __va_block, __va_block_next)            \
    for (__va_block = uvm_va_range_block_next(__va_range, NULL),                               \
         __va_block_next = uvm_va_range_block_next(__va_range, __va_block);                    \
         __va_block;                                                                           \
         __va_block = __va_block_next,                                                         \
         __va_block_next = __va_block? uvm_va_range_block_next(__va_range, __va_block) : NULL)

// Set the VA range preferred location (or unset it if preferred location is
// UVM_ID_INVALID).
//
// Unsetting the preferred location potentially changes the range group
// association to UVM_RANGE_GROUP_ID_NONE if the VA range was previously
// associated with a non-migratable range group.
//
// Changing the preferred location also updates the mask and mappings of GPUs
// in UVM-Lite mode.
//
// The va_range must have type UVM_VA_RANGE_TYPE_MANAGED.
//
// If mm != NULL, that mm is used for any CPU mappings which may be created as
// a result of this call. See uvm_va_block_context_t::mm for details.
//
// If out_tracker != NULL any block work will be added to that tracker.
//
// LOCKING: If mm != NULL, the caller must hold mm->mmap_lock in at least read
//          mode.
NV_STATUS uvm_va_range_set_preferred_location(uvm_va_range_t *va_range,
                                              uvm_processor_id_t preferred_location,
                                              int preferred_cpu_nid,
                                              struct mm_struct *mm,
                                              uvm_tracker_t *out_tracker);

// Add a processor to the accessed_by mask and establish any new required
// mappings.
//
// Also update the mask of UVM-Lite GPUs if needed.
//
// If mm != NULL, that mm is used for any CPU mappings which may be created as
// a result of this call. See uvm_va_block_context_t::mm for details.
//
// If out_tracker != NULL any block work will be added to that tracker.
//
// LOCKING: If mm != NULL, the caller must hold mm->mmap_lock in at least read
//          mode.
NV_STATUS uvm_va_range_set_accessed_by(uvm_va_range_t *va_range,
                                       uvm_processor_id_t processor_id,
                                       struct mm_struct *mm,
                                       uvm_tracker_t *out_tracker);

// Remove a processor from the accessed_by mask
//
// If out_tracker != NULL any block work will be added to that tracker.
//
// This also updates the mask and mappings of the UVM-Lite GPUs if required.
void uvm_va_range_unset_accessed_by(uvm_va_range_t *va_range,
                                    uvm_processor_id_t processor_id,
                                    uvm_tracker_t *out_tracker);

// Set read-duplication and remove any existing accessed_by and remote mappings
//
// If mm != NULL, that mm is used for any CPU mappings which may be created as
// a result of this call. See uvm_va_block_context_t::mm for details.
//
// LOCKING: If mm != NULL, the caller must hold mm->mmap_lock in at least read
//          mode.
NV_STATUS uvm_va_range_set_read_duplication(uvm_va_range_t *va_range, struct mm_struct *mm);

// Unset read-duplication and establish accessed_by mappings
//
// If mm != NULL, that mm is used for any CPU mappings which may be created as
// a result of this call. See uvm_va_block_context_t::mm for details.
//
// LOCKING: If mm != NULL, the caller must hold mm->mmap_lock in at least read
//          mode.
NV_STATUS uvm_va_range_unset_read_duplication(uvm_va_range_t *va_range, struct mm_struct *mm);

// Create and destroy vma wrappers
uvm_vma_wrapper_t *uvm_vma_wrapper_alloc(struct vm_area_struct *vma);
void uvm_vma_wrapper_destroy(uvm_vma_wrapper_t *vma_wrapper);

static uvm_va_policy_t *uvm_va_range_get_policy(uvm_va_range_t *va_range)
{
    UVM_ASSERT(va_range->type == UVM_VA_RANGE_TYPE_MANAGED);
    return &va_range->managed.policy;
}

NV_STATUS uvm_test_va_range_info(UVM_TEST_VA_RANGE_INFO_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_va_range_split(UVM_TEST_VA_RANGE_SPLIT_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_va_range_inject_split_error(UVM_TEST_VA_RANGE_INJECT_SPLIT_ERROR_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_va_range_inject_add_gpu_va_space_error(UVM_TEST_VA_RANGE_INJECT_ADD_GPU_VA_SPACE_ERROR_PARAMS *params,
                                                          struct file *filp);

#endif // __UVM_VA_RANGE_H__
