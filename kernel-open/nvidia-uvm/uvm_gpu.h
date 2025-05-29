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

#ifndef __UVM_GPU_H__
#define __UVM_GPU_H__

#include "nvtypes.h"
#include "nvmisc.h"
#include "uvm_types.h"
#include "nv_uvm_types.h"
#include "uvm_linux.h"
#include "nv-kref.h"
#include "uvm_common.h"
#include "ctrl2080mc.h"
#include "uvm_forward_decl.h"
#include "uvm_processors.h"
#include "uvm_pmm_gpu.h"
#include "uvm_pmm_sysmem.h"
#include "uvm_mmu.h"
#include "uvm_gpu_replayable_faults.h"
#include "uvm_gpu_isr.h"
#include "uvm_hal_types.h"
#include "uvm_hmm.h"
#include "uvm_va_block_types.h"
#include "uvm_perf_module.h"
#include "uvm_rb_tree.h"
#include "uvm_perf_prefetch.h"
#include "nv-kthread-q.h"
#include <linux/mmu_notifier.h>
#include "uvm_conf_computing.h"

#define UVM_PARENT_GPU_UUID_PREFIX "GPU-"
#define UVM_GPU_UUID_PREFIX "GI-"

// UVM_UUID_STRING_LENGTH already includes NULL, don't double-count it with
// sizeof()
#define UVM_PARENT_GPU_UUID_STRING_LENGTH (sizeof(UVM_PARENT_GPU_UUID_PREFIX) - 1 + UVM_UUID_STRING_LENGTH)
#define UVM_GPU_UUID_STRING_LENGTH (sizeof(UVM_GPU_UUID_PREFIX) - 1 + UVM_UUID_STRING_LENGTH)

#define UVM_GPU_MAGIC_VALUE 0xc001d00d12341993ULL

typedef struct
{
    // Number of faults from this uTLB that have been fetched but have not been
    // serviced yet.
    NvU32 num_pending_faults;

    // Whether the uTLB contains fatal faults
    bool has_fatal_faults;

    // We have issued a replay of type START_ACK_ALL while containing fatal
    // faults. This puts the uTLB in lockdown mode and no new translations are
    // accepted.
    bool in_lockdown;

    // We have issued a cancel on this uTLB
    bool cancelled;

    uvm_fault_buffer_entry_t prev_fatal_fault;

    // Last fetched fault that was originated from this uTLB. Used for fault
    // filtering.
    uvm_fault_buffer_entry_t *last_fault;
} uvm_fault_utlb_info_t;

struct uvm_service_block_context_struct
{
    //
    // Fields initialized by CPU/GPU fault handling and access counter routines
    //

    // Whether the information refers to replayable/non-replayable faults or
    // access counters
    uvm_service_operation_t operation;

    // Processors that will be the residency of pages after the operation has
    // been serviced
    uvm_processor_mask_t resident_processors;

    // A mask of GPUs that need to be checked for NVLINK errors before the
    // handler returns, but after the VA space lock has been unlocked
    // to avoid RM/UVM VA space lock deadlocks.
    uvm_processor_mask_t gpus_to_check_for_nvlink_errors;

    // VA block region that contains all the pages affected by the operation
    uvm_va_block_region_t region;

    // Array of type uvm_fault_access_type_t that contains the type of the
    // access that caused the fault/access_counter notification to be serviced
    // for each page.
    NvU8 access_type[PAGES_PER_UVM_VA_BLOCK];

    // Number of times the service operation has been retried
    unsigned num_retries;

    // Pages that need to be pinned due to thrashing
    uvm_page_mask_t thrashing_pin_mask;

    // Number of pages that need to be pinned due to thrashing. This is the same
    // value as the result of bitmap_weight(thrashing_pin_mask)
    unsigned thrashing_pin_count;

    // Pages that can be read-duplicated
    uvm_page_mask_t read_duplicate_mask;

    // Number of pages that can be read-duplicated. This is the same value as
    // the result of bitmap_weight(read_duplicate_count_mask)
    unsigned read_duplicate_count;

    //
    // Fields used by the CPU fault handling routine
    //

    struct
    {
        // Node of the list of fault service contexts used by the CPU
        struct list_head service_context_list;

        // A mask of GPUs that need to be checked for ECC errors before the CPU
        // fault handler returns, but after the VA space lock has been unlocked
        // to avoid the RM/UVM VA space lock deadlocks.
        uvm_processor_mask_t gpus_to_check_for_ecc;

        // This is set to throttle page fault thrashing.
        NvU64 wakeup_time_stamp;

        // This is set if the page migrated to/from the GPU and CPU.
        bool did_migrate;

        // Sequence number used to start a mmu notifier read side critical
        // section.
        unsigned long notifier_seq;

        struct vm_fault *vmf;
    } cpu_fault;

    //
    // Fields managed by the common operation servicing routine
    //

    uvm_prot_page_mask_array_t mappings_by_prot;

    // Mask with the pages that did not migrate to the processor (they were
    // already resident) in the last call to uvm_va_block_make_resident.
    // This is used to compute the pages that need to revoke mapping permissions
    // from other processors.
    uvm_page_mask_t did_not_migrate_mask;

    // Pages whose permissions need to be revoked from other processors
    uvm_page_mask_t revocation_mask;

    // Temporary mask used in service_va_block_locked() in
    // uvm_gpu_access_counters.c.
    uvm_processor_mask_t update_processors;

    struct
    {
        // Per-processor mask with the pages that will be resident after
        // servicing. We need one mask per processor because we may coalesce
        // faults that trigger migrations to different processors.
        uvm_page_mask_t new_residency;
    } per_processor_masks[UVM_ID_MAX_PROCESSORS];

    // State used by the VA block routines called by the servicing routine
    uvm_va_block_context_t *block_context;

    // Prefetch state hint
    uvm_perf_prefetch_hint_t prefetch_hint;

    // Prefetch temporary state.
    uvm_perf_prefetch_bitmap_tree_t prefetch_bitmap_tree;

    // Access counters notification buffer index.
    NvU32 access_counters_buffer_index;
};

typedef struct
{
    union
    {
        struct
        {
            // Mask of prefetch faulted pages in a UVM_VA_BLOCK_SIZE aligned
            // region of a SAM VMA. Used for batching ATS faults in a vma.
            uvm_page_mask_t prefetch_only_fault_mask;

            // Mask of read faulted pages in a UVM_VA_BLOCK_SIZE aligned region
            // of a SAM VMA. Used for batching ATS faults in a vma.
            uvm_page_mask_t read_fault_mask;

            // Mask of write faulted pages in a UVM_VA_BLOCK_SIZE aligned region
            // of a SAM VMA. Used for batching ATS faults in a vma.
            uvm_page_mask_t write_fault_mask;

            // Mask of all faulted pages in a UVM_VA_BLOCK_SIZE aligned region
            // of a SAM VMA. This is a logical or of read_fault_mask and
            // write_mask and prefetch_only_fault_mask.
            uvm_page_mask_t accessed_mask;

            // Mask of successfully serviced pages in a UVM_VA_BLOCK_SIZE
            // aligned region of a SAM VMA. Used to return ATS fault status.
            uvm_page_mask_t faults_serviced_mask;

            // Mask of successfully serviced read faults on pages in
            // write_fault_mask.
            uvm_page_mask_t reads_serviced_mask;

        } faults;

        struct
        {
            // Mask of all accessed pages in a UVM_VA_BLOCK_SIZE aligned region
            // of a SAM VMA.
            uvm_page_mask_t accessed_mask;

            // Mask of successfully migrated pages in a UVM_VA_BLOCK_SIZE
            // aligned region of a SAM VMA.
            uvm_page_mask_t migrated_mask;

        } access_counters;
    };

    // Client type of the service requestor.
    uvm_fault_client_type_t client_type;

    // New residency ID of the faulting region.
    uvm_processor_id_t residency_id;

    // New residency NUMA node ID of the faulting region.
    int residency_node;

    struct
    {
        // True if preferred_location was set on this faulting region.
        // UVM_VA_BLOCK_SIZE sized region in the faulting region bound by the
        // VMA is is prefetched if preferred_location was set and if first_touch
        // is true;
        bool has_preferred_location;

        // True if the UVM_VA_BLOCK_SIZE sized region isn't resident on any
        // node. False if any page in the region is resident somewhere.
        bool first_touch;

        // Mask of prefetched pages in a UVM_VA_BLOCK_SIZE aligned region of a
        // SAM VMA.
        uvm_page_mask_t prefetch_pages_mask;

        // PFN info of the faulting region
        unsigned long pfns[PAGES_PER_UVM_VA_BLOCK];

        // Faulting/preferred processor residency mask of the faulting region.
        uvm_page_mask_t residency_mask;

#if defined(NV_MMU_INTERVAL_NOTIFIER)
        // MMU notifier used to compute residency of this faulting region.
        struct mmu_interval_notifier notifier;
#endif

        uvm_va_space_t *va_space;

        // Prefetch temporary state.
        uvm_perf_prefetch_bitmap_tree_t bitmap_tree;
    } prefetch_state;
} uvm_ats_fault_context_t;

struct uvm_fault_service_batch_context_struct
{
    // Array of elements fetched from the GPU fault buffer. The number of
    // elements in this array is exactly max_batch_size
    uvm_fault_buffer_entry_t *fault_cache;

    // Array of pointers to elements in fault cache used for fault
    // preprocessing. The number of elements in this array is exactly
    // max_batch_size
    uvm_fault_buffer_entry_t **ordered_fault_cache;

    // Per uTLB fault information. Used for replay policies and fault
    // cancellation on Pascal
    uvm_fault_utlb_info_t *utlbs;

    // Largest uTLB id seen in a GPU fault
    NvU32 max_utlb_id;

    NvU32 num_cached_faults;

    NvU32 num_coalesced_faults;

    // One of the VA spaces in this batch which had fatal faults. If NULL, no
    // faults were fatal. More than one VA space could have fatal faults, but we
    // pick one to be the target of the cancel sequence.
    uvm_va_space_t *fatal_va_space;

    // TODO: Bug 3900733: refactor service_fault_batch_for_cancel() to handle
    // iterating over multiple GPU VA spaces and remove fatal_gpu.
    uvm_gpu_t *fatal_gpu;

    bool has_throttled_faults;

    NvU32 num_invalid_prefetch_faults;

    NvU32 num_duplicate_faults;

    NvU32 num_replays;

    uvm_ats_fault_context_t ats_context;

    // Unique id (per-GPU) generated for tools events recording
    NvU32 batch_id;

    uvm_tracker_t tracker;

    // Boolean used to avoid sorting the fault batch by instance_ptr if we
    // determine at fetch time that all the faults in the batch report the same
    // instance_ptr
    bool is_single_instance_ptr;

    // Last fetched fault. Used for fault filtering.
    uvm_fault_buffer_entry_t *last_fault;
};

struct uvm_ats_fault_invalidate_struct
{
    bool            tlb_batch_pending;
    uvm_tlb_batch_t tlb_batch;
};

typedef struct
{
    // Fault buffer information and structures provided by RM
    UvmGpuFaultInfo rm_info;

    // Maximum number of faults to be processed in batch before fetching new
    // entries from the GPU buffer
    NvU32 max_batch_size;

    struct uvm_replayable_fault_buffer_struct
    {
        // Maximum number of faults entries that can be stored in the buffer
        NvU32 max_faults;

        // Cached value of the GPU GET register to minimize the round-trips
        // over PCIe
        NvU32 cached_get;

        // Cached value of the GPU PUT register to minimize the round-trips over
        // PCIe
        NvU32 cached_put;

        // Policy that determines when GPU replays are issued during normal
        // fault servicing
        uvm_perf_fault_replay_policy_t replay_policy;

        // Tracker used to aggregate replay operations, needed for fault cancel
        // and GPU removal
        uvm_tracker_t replay_tracker;

        // If there is a ratio larger than replay_update_put_ratio of duplicate
        // faults in a batch, PUT pointer is updated before flushing the buffer
        // that comes before the replay method.
        NvU32 replay_update_put_ratio;

        // Fault statistics. These fields are per-GPU and most of them are only
        // updated during fault servicing, and can be safely incremented.
        // Migrations may be triggered by different GPUs and need to be
        // incremented using atomics
        struct
        {
            NvU64 num_prefetch_faults;

            NvU64 num_read_faults;

            NvU64 num_write_faults;

            NvU64 num_atomic_faults;

            NvU64 num_duplicate_faults;

            atomic64_t num_pages_out;

            atomic64_t num_pages_in;

            NvU64 num_replays;

            NvU64 num_replays_ack_all;
        } stats;

        // Number of uTLBs in the chip
        NvU32 utlb_count;

        // Context structure used to service a GPU fault batch
        uvm_fault_service_batch_context_t batch_service_context;

        // Structure used to coalesce fault servicing in a VA block
        uvm_service_block_context_t block_service_context;

        // Information required to invalidate stale ATS PTEs from the GPU TLBs
        uvm_ats_fault_invalidate_t ats_invalidate;
    } replayable;

    struct uvm_non_replayable_fault_buffer_struct
    {
        // Maximum number of faults entries that can be stored in the buffer
        NvU32 max_faults;

        // Tracker used to aggregate clear faulted operations, needed for GPU
        // removal
        uvm_tracker_t clear_faulted_tracker;

        // Buffer used to store elements popped out from the queue shared with
        // RM for fault servicing.
        void *shadow_buffer_copy;

        // Array of elements fetched from the GPU fault buffer. The number of
        // elements in this array is exactly max_batch_size
        uvm_fault_buffer_entry_t *fault_cache;

        // Fault statistics. See replayable fault stats for more details.
        struct
        {
            NvU64 num_read_faults;

            NvU64 num_write_faults;

            NvU64 num_atomic_faults;

            NvU64 num_physical_faults;

            atomic64_t num_pages_out;

            atomic64_t num_pages_in;
        } stats;

        // Tracker which temporarily holds the work pushed to service faults
        uvm_tracker_t fault_service_tracker;

        // Structure used to coalesce fault servicing in a VA block
        uvm_service_block_context_t block_service_context;

        // Unique id (per-GPU) generated for tools events recording
        NvU32 batch_id;

        // Information required to service ATS faults.
        uvm_ats_fault_context_t ats_context;

        // Information required to invalidate stale ATS PTEs from the GPU TLBs
        uvm_ats_fault_invalidate_t ats_invalidate;
    } non_replayable;

    // Flag that tells if prefetch faults are enabled in HW
    bool prefetch_faults_enabled;

    // Timestamp when prefetch faults where disabled last time
    NvU64 disable_prefetch_faults_timestamp;
} uvm_fault_buffer_t;

struct uvm_access_counter_service_batch_context_struct
{
    uvm_access_counter_buffer_entry_t *notification_cache;

    NvU32 num_cached_notifications;

    uvm_access_counter_buffer_entry_t **notifications;

    NvU32 num_notifications;

    // Boolean used to avoid sorting the fault batch by instance_ptr if we
    // determine at fetch time that all the access counter notifications in
    // the batch report the same instance_ptr
    bool is_single_instance_ptr;

    // Helper page mask to compute the accessed pages within a VA block
    uvm_page_mask_t accessed_pages;

    // Structure used to coalesce access counter servicing in a VA block
    uvm_service_block_context_t block_service_context;

    // Structure used to service access counter migrations in an ATS block.
    uvm_ats_fault_context_t ats_context;

    // Unique id (per-GPU) generated for tools events recording
    NvU32 batch_id;
};

struct uvm_access_counter_buffer_struct
{
    uvm_parent_gpu_t *parent_gpu;

    UvmGpuAccessCntrInfo rm_info;

    // Access counters may have multiple notification buffers.
    NvU32 index;

    NvU32 max_notifications;

    NvU32 max_batch_size;

    // Cached value of the GPU GET register to minimize the round-trips
    // over PCIe
    NvU32 cached_get;

    // Cached value of the GPU PUT register to minimize the round-trips over
    // PCIe
    NvU32 cached_put;

    // Current access counter configuration. During normal operation this
    // information is computed once during GPU initialization. However, tests
    // may override it to try different configuration values.
    struct
    {
        // Values used to configure access counters in RM
        struct
        {
            UVM_ACCESS_COUNTER_GRANULARITY granularity;
        } rm;

        // The following values are precomputed by the access counter
        // notification handling code. See comments for UVM_MAX_TRANSLATION_SIZE
        // in uvm_gpu_access_counters.c for more details.
        NvU64 translation_size;

        NvU64 sub_granularity_region_size;

        NvU64 sub_granularity_regions_per_translation;

        NvU32 threshold;
    } current_config;

    // Access counter statistics
    struct
    {
        atomic64_t num_pages_out;

        atomic64_t num_pages_in;
    } stats;

    // Ignoring access counters means that notifications are left in the HW
    // buffer without being serviced. Requests to ignore access counters
    // are counted since the suspend path inhibits access counter interrupts,
    // and the resume path needs to know whether to reenable them.
    NvU32 notifications_ignored_count;

    // Context structure used to service a GPU access counter batch
    uvm_access_counter_service_batch_context_t batch_service_context;

    struct
    {
        // VA space that reconfigured the access counters configuration, if any.
        // Used in builtin tests only, to avoid reconfigurations from different
        // processes.
        //
        // Locking: both readers and writers must hold the access counters ISR
        // lock.
        uvm_va_space_t *reconfiguration_owner;

        // The service access counters loop breaks after processing the first
        // batch. It will be retriggered if there are pending notifications, but
        // it releases the ISR service lock to check certain races that would be
        // difficult to hit otherwise.
        bool one_iteration_per_batch;
        NvU32 sleep_per_iteration_us;
    } test;

};

typedef struct
{
    // VA where the identity mapping should be mapped in the internal VA
    // space managed by uvm_gpu_t.address_space_tree (see below).
    NvU64 base;

    // Page tables with the mapping.
    uvm_page_table_range_vec_t *range_vec;

    // Used during init to indicate whether the mapping has been fully
    // initialized.
    bool ready;
} uvm_gpu_identity_mapping_t;

// Root chunk mapping
typedef struct
{
    // Page table range representation of the mapping. Because a root chunk
    // fits into a single 2MB page, in practice the range consists of a single
    // 2MB PTE.
    uvm_page_table_range_t *range;

    // Number of mapped pages of size PAGE_SIZE.
    NvU32 num_mapped_pages;
} uvm_gpu_root_chunk_mapping_t;

typedef enum
{
    UVM_GPU_LINK_INVALID = 0,
    UVM_GPU_LINK_PCIE,
    UVM_GPU_LINK_NVLINK_1,
    UVM_GPU_LINK_NVLINK_2,
    UVM_GPU_LINK_NVLINK_3,
    UVM_GPU_LINK_NVLINK_4,
    UVM_GPU_LINK_NVLINK_5,
    UVM_GPU_LINK_C2C,
    UVM_GPU_LINK_MAX
} uvm_gpu_link_type_t;

typedef enum
{
    // Peer copies can be disallowed for a variety of reasons. For example,
    // P2P transfers are disabled in pre-Pascal GPUs because there is no
    // compelling use case for direct peer migrations.
    UVM_GPU_PEER_COPY_MODE_UNSUPPORTED,

    // Pascal+ GPUs support virtual addresses in P2P copies. Virtual peer copies
    // require the creation of peer identity mappings.
    UVM_GPU_PEER_COPY_MODE_VIRTUAL,

    // Ampere+ GPUs support virtual and physical peer copies. Physical peer
    // copies do not depend on peer identity mappings.
    UVM_GPU_PEER_COPY_MODE_PHYSICAL,

    UVM_GPU_PEER_COPY_MODE_COUNT
} uvm_gpu_peer_copy_mode_t;

// In order to support SMC/MIG GPU partitions, we split UVM GPUs into two
// parts: parent GPUs (uvm_parent_gpu_t) which represent unique PCIe devices
// (including VFs), and sub/child GPUs (uvm_gpu_t) which represent individual
// partitions within the parent. The parent GPU and partition GPU have
// different "id" and "uuid".
struct uvm_gpu_struct
{
    uvm_parent_gpu_t *parent;

    // The gpu's GI uuid if SMC is enabled; otherwise, a copy of parent->uuid.
    NvProcessorUuid uuid;

    // Nice printable name in the format:
    // ID: 999: GPU-<parent_uuid> GI-<gi_uuid>
    // UVM_PARENT_GPU_UUID_STRING_LENGTH includes a NULL character but will be
    // used for a space instead.
    char name[sizeof("ID: 999: ") - 1 + UVM_PARENT_GPU_UUID_STRING_LENGTH - 1 + 1 + UVM_GPU_UUID_STRING_LENGTH];

    // Refcount of the gpu, i.e. how many times it has been retained. This is
    // roughly a count of how many times it has been registered with a VA space,
    // except that some paths retain the GPU temporarily without a VA space.
    //
    // While this is >0, the GPU can't be removed. This differs from gpu_kref,
    // which merely prevents the uvm_gpu_t object from being freed.
    //
    // In most cases this count is protected by the global lock: retaining a GPU
    // from a UUID and any release require the global lock to be taken. But it's
    // also useful for a caller to retain a GPU they've already retained, in
    // which case there's no need to take the global lock. This can happen when
    // an operation needs to drop the VA space lock but continue operating on a
    // GPU. This is an atomic variable to handle those cases.
    //
    // Security note: keep it as a 64-bit counter to prevent overflow cases (a
    // user can create a lot of va spaces and register the gpu with them).
    atomic64_t retained_count;

    // A unique uvm gpu id in range [1, UVM_ID_MAX_PROCESSORS).
    uvm_gpu_id_t id;

    // Should be UVM_GPU_MAGIC_VALUE. Used for memory checking.
    NvU64 magic;

    struct
    {
        // The amount of memory the GPU has in total, in bytes. If the GPU is in
        // ZeroFB testing mode, this will be 0.
        NvU64 size;

        // Physical start of heap, for SMC enabled GPUs, this is useful to
        // partition PMM, it is used by HMM to figure out the right translation
        // between HMM ranges and PMM offsets.
        NvU64 phys_start;

        // Max (inclusive) physical address of this GPU's memory that the driver
        // can allocate through PMM (PMA).
        NvU64 max_allocatable_address;

        // Max supported vidmem page size may be smaller than the max GMMU page
        // size, because of the vMMU supported page sizes.
        NvU64 max_vidmem_page_size;

        struct
        {
            // True if the platform supports HW coherence and the GPU's memory
            // is exposed as a NUMA node to the kernel.
            bool enabled;
            int node_id;
        } numa;

        // Physical address of the start of statically mapped fb memory in BAR1
        NvU64 static_bar1_start;

        // Size of statically mapped fb memory in BAR1.
        NvU64 static_bar1_size;
    } mem_info;

    struct
    {
        // Big page size used by the internal UVM VA space
        // Notably it may be different than the big page size used by a user's
        // VA space in general.
        NvU32 internal_size;
    } big_page;

    // Mapped registers needed to obtain the current GPU timestamp
    struct
    {
        volatile NvU32 *time0_register;
        volatile NvU32 *time1_register;
    } time;

    // Identity peer mappings are only defined when
    // peer_copy_mode == UVM_GPU_PEER_COPY_MODE_VIRTUAL
    uvm_gpu_identity_mapping_t peer_mappings[UVM_ID_MAX_GPUS];

    struct
    {
        // Mask of peer_gpus set.
        uvm_processor_mask_t peer_gpu_mask;

        // Leaf spinlock used to synchronize access to peer_gpu_mask.
        uvm_spinlock_t peer_gpu_lock;
    } peer_info;

    // Maximum number of subcontexts supported
    NvU32 max_subcontexts;

    // RM address space handle used in many of the UVM/RM APIs
    // Represents a GPU VA space within rm_device.
    //
    // In SR-IOV heavy, proxy channels are not associated with this address
    // space.
    uvmGpuAddressSpaceHandle rm_address_space;

    // Page tree used for the internal UVM VA space shared with RM
    uvm_page_tree_t address_space_tree;

    // Set to true during add_gpu() as soon as the RM's address space is moved
    // to the address_space_tree.
    bool rm_address_space_moved_to_page_tree;

    uvm_gpu_semaphore_pool_t *semaphore_pool;

    uvm_gpu_semaphore_pool_t *secure_semaphore_pool;

    uvm_channel_manager_t *channel_manager;

    uvm_pmm_gpu_t pmm;

    // Flat linear mapping covering vidmem. This is a kernel mapping that is
    // only created in certain configurations.
    //
    // There are two mutually exclusive versions of the mapping. The simplest
    // version covers the entire GPU memory, and it is created during GPU
    // initialization. The dynamic version is a partial vidmem mapping that
    // creates and destroys mappings to GPU root chunks on demand.
    union
    {
        // Static mapping covering the whole GPU memory.
        uvm_gpu_identity_mapping_t static_flat_mapping;

        // Dynamic mapping of GPU memory.
        struct
        {
            // Array of root chunk mappings.
            uvm_gpu_root_chunk_mapping_t *array;

            // Number of elements in the array.
            size_t count;

            // Each bit in the bitlock protects a single root chunk mapping.
            uvm_bit_locks_t bitlocks;

        } root_chunk_mappings;
    };

    // Linear sysmem mappings. Mappings are added on demand, and removed upon
    // GPU deinitialization. The mappings are added to UVM's internal address
    // space i.e. they are kernel mappings.
    //
    // Only used in SR-IOV heavy.
    struct
    {
        // Size of each mapping, in bytes.
        NvU64 mapping_size;

        // Array of sysmem mappings.
        uvm_gpu_identity_mapping_t *array;

        // Number of elements in the array.
        size_t count;

        // Each bit in the bitlock protects a sysmem mapping.
        uvm_bit_locks_t bitlocks;
    } sysmem_mappings;

    // Reverse lookup table used to query the user mapping associated with a
    // sysmem (DMA) physical address.
    //
    // The system memory mapping information referred to by this field is
    // different from that of sysmem_mappings, because it relates to user
    // mappings (instead of kernel), and it is used in most configurations.
    uvm_pmm_sysmem_mappings_t pmm_reverse_sysmem_mappings;

    struct
    {
        uvm_conf_computing_dma_buffer_pool_t dma_buffer_pool;

        // Dummy memory used to store the IV contents during CE encryption.
        // This memory location is also only available after CE channels
        // because we use them to write PTEs for allocations such as this one.
        // This location is used when a physical addressing for the IV buffer
        // is required. See uvm_hal_hopper_ce_encrypt().
        uvm_mem_t *iv_mem;

        // Dummy memory used to store the IV contents during CE encryption.
        // Because of the limitations of `iv_mem', and the need to have such
        // buffer at channel initialization, we use an RM allocation.
        // This location is used when a virtual addressing for the IV buffer
        // is required. See uvm_hal_hopper_ce_encrypt().
        uvm_rm_mem_t *iv_rm_mem;
    } conf_computing;

    // ECC handling
    // In order to trap ECC errors as soon as possible the driver has the hw
    // interrupt register mapped directly. If an ECC interrupt is ever noticed
    // to be pending, then the UVM driver needs to:
    //
    //   1) ask RM to service interrupts, and then
    //   2) inspect the ECC error notifier state.
    //
    // Notably, checking for channel errors is not enough, because ECC errors
    // can be pending, even after a channel has become idle.
    //
    // See more details in uvm_gpu_check_ecc_error().
    struct
    {
        // Does the GPU have ECC enabled?
        bool enabled;

        // Direct mapping of the 32-bit part of the hw interrupt tree that has
        // the ECC bits.
        volatile NvU32 *hw_interrupt_tree_location;

        // Mask to get the ECC interrupt bits from the 32-bits above.
        NvU32 mask;

        // Set to true by RM when a fatal ECC error is encountered (requires
        // asking RM to service pending interrupts to be current).
        NvBool *error_notifier;
    } ecc;

    // NVLINK STO recovery handling
    // In order to trap STO errors as soon as possible the driver has the hw
    // interrupt register mapped directly. If an STO interrupt is ever noticed
    // to be pending, then the UVM driver needs to:
    //
    //   1) ask RM to service interrupts, and then
    //   2) inspect the NVLINK error notifier state.
    //
    // Notably, checking for channel errors is not enough, because STO errors
    // can be pending, even after a channel has become idle.
    //
    // See more details in uvm_gpu_check_nvlink_error().
    struct
    {
        // Does the GPU have NVLINK STO recovery enabled?
        bool enabled;

        // Artificially injected error for testing
        atomic_t injected_error;

        // Direct mapping of the 32-bit part of the hw interrupt tree that has
        // the NVLINK error bits.
        volatile NvU32 *hw_interrupt_tree_location;

        // Mask to get the NVLINK error interrupt bits from the 32-bits above.
        NvU32 mask;

        // Set to true by RM when a fatal NVLINK error is encountered (requires
        // asking RM to service pending interrupts to be current).
        NvBool *error_notifier;
    } nvlink_status;

    struct
    {
        NvU32 swizz_id;

        // RM device handle used in many of the UVM/RM APIs.
        //
        // Do not read this field directly, use uvm_gpu_device_handle instead.
        uvmGpuDeviceHandle rm_device;
    } smc;

    struct
    {
        // "gpus/UVM-GPU-${physical-UUID}/${sub_processor_index}/"
        struct proc_dir_entry *dir;

        // "gpus/${gpu_id}" -> "UVM-GPU-${physical-UUID}/${sub_processor_index}"
        struct proc_dir_entry *dir_symlink;

        // The GPU instance UUID symlink.
        // "gpus/UVM-GI-${GI-UUID}" ->
        //     "UVM-GPU-${physical-UUID}/${sub_processor_index}"
        struct proc_dir_entry *gpu_instance_uuid_symlink;

        // "gpus/UVM-GPU-${physical-UUID}/${sub_processor_index}/info"
        struct proc_dir_entry *info_file;
    } procfs;

    // Placeholder for per-GPU performance heuristics information
    uvm_perf_module_data_desc_t perf_modules_data[UVM_PERF_MODULE_TYPE_COUNT];

    // Force pushbuffer's GPU VA to be >= 1TB; used only for testing purposes.
    bool uvm_test_force_upper_pushbuffer_segment;

    // Have we initialised device p2p pages.
    bool device_p2p_initialised;

    // Used to protect allocation of p2p_mem and assignment of the page
    // zone_device_data fields.
    uvm_mutex_t device_p2p_lock;
};

typedef struct
{
    bool access_counters_alloc_buffer;
    bool access_counters_alloc_block_context;
    bool isr_access_counters_alloc;
    bool isr_access_counters_alloc_stats_cpu;
    bool access_counters_batch_context_notifications;
    bool access_counters_batch_context_notification_cache;
} uvm_test_parent_gpu_inject_error_t;

// In order to support SMC/MIG GPU partitions, we split UVM GPUs into two
// parts: parent GPUs (uvm_parent_gpu_t) which represent unique PCIe devices
// (including VFs), and sub/child GPUs (uvm_gpu_t) which represent individual
// partitions within the parent. The parent GPU and partition GPU have
// different "id" and "uuid".
struct uvm_parent_gpu_struct
{
    // Reference count for how many places are holding on to a parent GPU
    // (internal to the UVM driver). This includes any GPUs we know about, not
    // just GPUs that are registered with a VA space. Most GPUs end up being
    // registered, but there are brief periods when they are not registered,
    // such as during interrupt handling, and in add_gpu() or remove_gpu().
    nv_kref_t gpu_kref;

    // The number of uvm_gpu_ts referencing this uvm_parent_gpu_t.
    NvU32 num_retained_gpus;

    uvm_gpu_t *gpus[UVM_PARENT_ID_MAX_SUB_PROCESSORS];

    // Bitmap of valid child entries in the gpus[] table. Used to retrieve a
    // usable child GPU in bottom-halves.
    DECLARE_BITMAP(valid_gpus, UVM_PARENT_ID_MAX_SUB_PROCESSORS);

    // The gpu's uuid
    NvProcessorUuid uuid;

    // Nice printable name including the uvm gpu id, ascii name from RM and uuid
    char name[sizeof("ID 999: : ") - 1 + UVM_GPU_NAME_LENGTH + UVM_PARENT_GPU_UUID_STRING_LENGTH];

    // GPU information and provided by RM (architecture, implementation,
    // hardware classes, etc.).
    UvmGpuInfo rm_info;

    // A unique uvm gpu id in range [1, UVM_PARENT_ID_MAX_PROCESSORS)
    uvm_parent_gpu_id_t id;

    // Reference to the Linux PCI device
    //
    // The reference to the PCI device remains valid as long as the GPU is
    // registered with RM's Linux layer (between nvUvmInterfaceRegisterGpu() and
    // nvUvmInterfaceUnregisterGpu()).
    struct pci_dev *pci_dev;

    // On kernels with NUMA support, this entry contains the closest CPU NUMA
    // node to this GPU. Otherwise, the value will be -1.
    int closest_cpu_numa_node;

    // RM device handle used in many of the UVM/RM APIs.
    //
    // Do not read this field directly, use uvm_gpu_device_handle instead.
    uvmGpuDeviceHandle rm_device;

    // Total amount of physical memory available on the parent GPU.
    NvU64 max_allocatable_address;

#if UVM_IS_CONFIG_HMM()
    uvm_pmm_gpu_devmem_t *devmem;
#endif

    // The physical address range addressable by the GPU
    //
    // The GPU has its NV_PFB_XV_UPPER_ADDR register set by RM to
    // dma_addressable_start (in bifSetupDmaWindow_IMPL()) and hence when
    // referencing sysmem from the GPU, dma_addressable_start should be
    // subtracted from the physical address. The DMA mapping helpers like
    // uvm_gpu_map_cpu_pages() and uvm_gpu_dma_alloc_page() take care of that.
    NvU64 dma_addressable_start;
    NvU64 dma_addressable_limit;

    // Total size (in bytes) of physically mapped (with
    // uvm_gpu_map_cpu_pages) sysmem pages, used for leak detection.
    atomic64_t mapped_cpu_pages_size;

    // Hardware Abstraction Layer
    uvm_host_hal_t *host_hal;
    uvm_ce_hal_t *ce_hal;
    uvm_arch_hal_t *arch_hal;
    uvm_fault_buffer_hal_t *fault_buffer_hal;
    uvm_access_counter_buffer_hal_t *access_counter_buffer_hal;
    uvm_sec2_hal_t *sec2_hal;

    // Whether CE supports physical addressing mode for writes to vidmem
    bool ce_phys_vidmem_write_supported;

    // Addressing mode(s) supported for CE transfers between this GPU and its
    // peers: none, physical only, physical and virtual, etc.
    uvm_gpu_peer_copy_mode_t peer_copy_mode;

    // Virtualization mode of the GPU.
    UVM_VIRT_MODE virt_mode;

    // Pascal+ GPUs can trigger faults on prefetch instructions. If false, this
    // feature must be disabled at all times in GPUs of the given architecture.
    // If true, the feature can be toggled at will by SW.
    //
    // The field should not be used unless the GPU supports replayable faults.
    bool prefetch_fault_supported;

    // Number of membars required to flush out HSHUB following a TLB invalidate
    NvU32 num_hshub_tlb_invalidate_membars;

    // Whether the channels can configure GPFIFO in vidmem
    bool gpfifo_in_vidmem_supported;

    bool replayable_faults_supported;

    bool non_replayable_faults_supported;

    bool access_counters_supported;

    // TODO: Bug 4637114: [UVM] Remove support for physical access counter
    // notifications. Always set to false, until we remove the PMM reverse
    // mapping code.
    bool access_counters_can_use_physical_addresses;

    bool fault_cancel_va_supported;

    // True if the GPU has hardware support for scoped atomics
    bool scoped_atomics_supported;

    // If true, a HW method can be used to clear a faulted channel.
    // If false, then the GPU supports clearing faulted channels using registers
    // instead of a HW method.
    // This value is only defined for GPUs that support non-replayable faults.
    bool has_clear_faulted_channel_method;

    // If true, a SW method can be used to clear a faulted channel.
    // If false, the HW method or the registers (whichever is available
    // according to has_clear_faulted_channel_method) needs to be used.
    //
    // This value is only defined for GPUs that support non-replayable faults.
    bool has_clear_faulted_channel_sw_method;

    bool sparse_mappings_supported;

    // Ampere(GA100) requires map->invalidate->remap->invalidate for page size
    // promotion
    bool map_remap_larger_page_promotion;

    bool plc_supported;

    // If true, page_tree initialization pre-populates no_ats_ranges. It only
    // affects ATS systems.
    bool no_ats_range_required;

    // Parameters used by the TLB batching API
    struct
    {
        // Is the targeted (single page) VA invalidate supported at all?
        NvBool va_invalidate_supported;

        // Is the VA range invalidate supported?
        NvBool va_range_invalidate_supported;

        union
        {
            // Maximum (inclusive) number of single page invalidations before
            // falling back to invalidate all
            NvU32 max_pages;

            // Maximum (inclusive) number of range invalidations before falling
            // back to invalidate all
            NvU32 max_ranges;
        };
    } tlb_batch;

    // Largest VA (exclusive) which can be used for channel buffer mappings
    NvU64 max_channel_va;

    // Largest VA (exclusive) which Host can operate.
    NvU64 max_host_va;

    // Indicates whether the GPU can map sysmem with pages larger than 4k
    bool can_map_sysmem_with_large_pages;

    // An integrated GPU has no vidmem and coherent access to sysmem. Note
    // integrated GPUs have a write-back L2 cache (cf. discrete GPUs
    // write-through cache.)
    // TODO: Bug 5023085: this should be queried from RM instead of determined
    // by UVM.
    bool is_integrated_gpu;

    struct
    {
        // If true, the granularity of key rotation is a single channel. If
        // false, the key replacement affects all channels on the engine. The
        // supported granularity is dependent on the number of key slots
        // available in HW.
        bool per_channel_key_rotation;
    } conf_computing;

    // VA base and size of the RM managed part of the internal UVM VA space.
    //
    // The internal UVM VA is shared with RM by RM controlling some of the top
    // level PDEs and leaving the rest for UVM to control.
    // On Pascal a single top level PDE covers 128 TB of VA and given that
    // semaphores and other allocations limited to 40bit are currently allocated
    // through RM, RM needs to control the [0, 128TB) VA range at least for now.
    // On Maxwell, limit RMs VA to [0, 128GB) that should easily fit
    // all RM allocations and leave enough space for UVM.
    NvU64 rm_va_base;
    NvU64 rm_va_size;

    // Base and size of the GPU VA space used for peer identity mappings,
    // it is used only if peer_copy_mode is UVM_GPU_PEER_COPY_MODE_VIRTUAL.
    NvU64 peer_va_base;
    NvU64 peer_va_size;

    // Base and size of the GPU VA used for uvm_mem_t allocations mapped in the
    // internal address_space_tree.
    NvU64 uvm_mem_va_base;
    NvU64 uvm_mem_va_size;

    // Base of the GPU VAs used for the vidmem and sysmem flat mappings.
    NvU64 flat_vidmem_va_base;
    NvU64 flat_sysmem_va_base;

    // Bitmap of allocation sizes for user memory supported by a GPU. PAGE_SIZE
    // is guaranteed to be both present and the smallest size.
    uvm_chunk_sizes_mask_t mmu_user_chunk_sizes;

    // Bitmap of allocation sizes that could be requested by the page tree for
    // a GPU
    uvm_chunk_sizes_mask_t mmu_kernel_chunk_sizes;

    struct
    {
        // "gpus/UVM-GPU-${physical-UUID}/"
        struct proc_dir_entry *dir;

        // "gpus/UVM-GPU-${physical-UUID}/fault_stats"
        struct proc_dir_entry *fault_stats_file;

        // "gpus/UVM-GPU-${physical-UUID}/access_counters"
        struct proc_dir_entry *access_counters_file;

        // "gpus/UVM-GPU-${physical-UUID}/peers/"
        struct proc_dir_entry *dir_peers;
    } procfs;

    // Interrupt handling state and locks
    uvm_isr_info_t isr;

    // This is only valid if supports_replayable_faults is set to true.
    uvm_fault_buffer_t fault_buffer;

    // PMM lazy free processing queue.
    // TODO: Bug 3881835: revisit whether to use nv_kthread_q_t or workqueue.
    nv_kthread_q_t lazy_free_q;

    // This is only valid if supports_access_counters is set to true. This array
    // has rm_info.accessCntrBufferCount entries.
    uvm_access_counter_buffer_t *access_counter_buffer;
    uvm_mutex_t access_counters_enablement_lock;

    // Tracker used to aggregate access counters clear operations, needed for
    // GPU removal. It is only used when supports_access_counters is set.
    uvm_tracker_t access_counters_clear_tracker;
    uvm_mutex_t access_counters_clear_tracker_lock;

    // Number of uTLBs per GPC. This information is only valid on Pascal+ GPUs.
    NvU32 utlb_per_gpc_count;

    // In order to service GPU faults, UVM must be able to obtain the VA
    // space for each reported fault. The fault packet contains the
    // instance_ptr of the channel that was bound when the SMs triggered
    // the fault. On fault any instance pointer in the TSG may be
    // reported. This is a problem on Volta, which allow different channels
    // in the TSG to be bound to different VA spaces in order to support
    // subcontexts. In order to be able to obtain the correct VA space, HW
    // provides the subcontext id (or VEID) in addition to the instance_ptr.
    //
    // Summary:
    //
    // 1) Channels in a TSG may be in different VA spaces, identified by their
    // subcontext ID.
    // 2) Different subcontext IDs may map to the same or different VA spaces.
    // 3) On fault, any instance pointer in the TSG may be reported. The
    // reported subcontext ID identifies which VA space within the TSG actually
    // encountered the fault.
    //
    // Thus, UVM needs to keep track of all the instance pointers that belong
    // to the same TSG. We use two tables:
    //
    // - instance_ptr_table (instance_ptr -> subctx_info) this table maps
    // instance pointers to the subcontext info descriptor for the channel. If
    // the channel belongs to a subcontext, this descriptor will contain all
    // the VA spaces for the subcontexts in the same TSG. If the channel does
    // not belong to a subcontext, it will only contain a pointer to its VA
    // space.
    // - tsg_table (tsg_id -> subctx_info): this table also stores the
    // subctx information, but in this case it is indexed by TSG ID. Thus,
    // when a new channel bound to a subcontext is registered, it will check
    // first in this table if the subcontext information descriptor for its TSG
    // already exists, otherwise it will create it. Channels not bound to
    // subcontexts will not use this table.
    //
    // The bottom half reads the tables under
    // isr.replayable_faults_handler.lock, but a separate lock is necessary
    // because entries are added and removed from the table under the va_space
    // lock, and we can't take isr.replayable_faults_handler.lock while holding
    // the va_space lock.
    uvm_rb_tree_t tsg_table;

    uvm_rb_tree_t instance_ptr_table;
    uvm_spinlock_t instance_ptr_table_lock;

    struct
    {
        bool supported;

        bool enabled;
    } smc;

    // Global statistics. These fields are per-GPU and most of them are only
    // updated during fault servicing, and can be safely incremented.
    struct
    {
        NvU64          num_replayable_faults;

        NvU64      num_non_replayable_faults;

        atomic64_t             num_pages_out;

        atomic64_t              num_pages_in;
    } stats;

    // Structure to hold nvswitch specific information. In an nvswitch
    // environment, rather than using the peer-id field of the PTE (which can
    // only address 8 gpus), all gpus are assigned a 47-bit physical address
    // space by the fabric manager. Any physical address access to these
    // physical address spaces are routed through the switch to the
    // corresponding peer.
    struct
    {
        bool is_nvswitch_connected;

        // 47-bit fabric memory physical offset that peer gpus need to access
        // to read a peer's memory
        NvU64 fabric_memory_window_start;

        // 47-bit fabric memory physical offset that peer gpus need to access
        // to read remote EGM memory.
        NvU64 egm_fabric_memory_window_start;
    } nvswitch_info;

    struct
    {
        // Note that this represents the link to system memory, not the link the
        // system used to discover the GPU. There are some cases such as NVLINK2
        // where the GPU is still on the PCIe bus, but it accesses memory over
        // this link rather than PCIe.
        uvm_gpu_link_type_t link;
        NvU32 link_rate_mbyte_per_s;

        // Range in the system physical address space where the memory of this
        // GPU is exposed as coherent. memory_window_end is inclusive.
        // memory_window_start == memory_window_end indicates that no window is
        // present (coherence is not supported).
        NvU64 memory_window_start;
        NvU64 memory_window_end;
    } system_bus;

    // WAR to issue ATS TLB invalidation commands ourselves.
    struct
    {
        uvm_mutex_t smmu_lock;
        struct page *smmu_cmdq;
        void __iomem *smmu_cmdqv_base;
        unsigned long smmu_prod;
        unsigned long smmu_cons;
    } smmu_war;

    struct
    {
        // Is EGM support enabled on this GPU.
        bool enabled;

        // Local EGM peer ID. This ID is used to route EGM memory accesses to
        // the local CPU socket.
        NvU8 local_peer_id;

        // EGM base address of the EGM carveout for remote EGM accesses.
        // The base address is used when computing PTE PA address values for
        // accesses to the local CPU socket's EGM memory from other peer
        // GPUs.
        NvU64 base_address;
    } egm;

    uvm_test_parent_gpu_inject_error_t test;
};

NvU64 uvm_parent_gpu_dma_addr_to_gpu_addr(uvm_parent_gpu_t *parent_gpu, NvU64 dma_addr);

static const char *uvm_parent_gpu_name(uvm_parent_gpu_t *parent_gpu)
{
    return parent_gpu->name;
}

static const char *uvm_gpu_name(uvm_gpu_t *gpu)
{
    return gpu->name;
}

static uvmGpuDeviceHandle uvm_gpu_device_handle(uvm_gpu_t *gpu)
{
    if (gpu->parent->smc.enabled)
        return gpu->smc.rm_device;
    return gpu->parent->rm_device;
}

typedef struct
{
    // ref_count also controls state maintained in each GPU instance
    // (uvm_gpu_t). See init_peer_access().
    NvU64 ref_count;
} uvm_gpu_peer_t;

typedef struct
{
    // The fields in this global structure can only be inspected under one of
    // the following conditions:
    //
    // - The VA space lock is held for either read or write, both parent GPUs
    //   are registered in the VA space, and the corresponding bit in the
    //   va_space.enabled_peers bitmap is set.
    //
    // - The global lock is held.
    //
    // - While the global lock was held in the past, the two parent GPUs were
    //   both retained.
    //
    // - While the global lock was held in the past, the two parent GPUs were
    //   detected to be NVLINK peers and were both retained.
    //
    // - While the global lock was held in the past, the two parent GPUs were
    //   detected to be PCIe peers and uvm_gpu_retain_pcie_peer_access() was
    //   called.
    //
    // - The peer_gpu_lock is held on one of the GPUs. In this case, the other
    //   GPU must be referred from the original GPU's peer_gpu_mask reference.
    //   The fields will not change while the lock is held, but they may no
    //   longer be valid because the other GPU might be in teardown.

    // This field is used to determine when this struct has been initialized
    // (ref_count != 0). NVLink peers are initialized at GPU registration time.
    // PCIe peers are initialized when retain_pcie_peers_from_uuids() is called.
    NvU64 ref_count;

    // Saved values from UvmGpuP2PCapsParams to be used after GPU instance
    // creation. This should be per GPU instance since LCEs are associated with
    // GPU instances, not parent GPUs, but for now MIG is not supported for
    // NVLINK peers so RM associates this state with the parent GPUs. This will
    // need to be revisited if that NVLINK MIG peer support is added.
    NvU8 optimalNvlinkWriteCEs[2];

    // Peer Id associated with this device with respect to a peer parent GPU.
    // Note: peerId (A -> B) != peerId (B -> A)
    // peer_id[0] from min(gpu_id_1, gpu_id_2) -> max(gpu_id_1, gpu_id_2)
    // peer_id[1] from max(gpu_id_1, gpu_id_2) -> min(gpu_id_1, gpu_id_2)
    NvU8 peer_ids[2];

    // EGM peer Id associated with this device w.r.t. a peer GPU.
    // Note: egmPeerId (A -> B) != egmPeerId (B -> A)
    // egm_peer_id[0] from min(gpu_id_1, gpu_id_2) -> max(gpu_id_1, gpu_id_2)
    // egm_peer_id[1] from max(gpu_id_1, gpu_id_2) -> min(gpu_id_1, gpu_id_2)
    //
    // Unlike VIDMEM peers, EGM peers are not symmetric. This means that if
    // one of the GPUs is EGM-enabled, it does not automatically mean that
    // the other is also EGM-enabled. Therefore, an EGM peer Ids are only
    // valid if the peer GPU is EGM-enabled, i.e. egm_peer_id[0] is valid
    // iff max(gpu_id_1, gpu_id_2) is EGM-enabled.
    NvU8 egm_peer_ids[2];

    // The link type between the peer parent GPUs, currently either PCIe or
    // NVLINK.
    uvm_gpu_link_type_t link_type;

    // Maximum unidirectional bandwidth between the peers in megabytes per
    // second, not taking into account the protocols' overhead.
    // See UvmGpuP2PCapsParams.
    NvU32 total_link_line_rate_mbyte_per_s;

    // This handle gets populated when enable_peer_access successfully creates
    // an NV50_P2P object. disable_peer_access resets the same on the object
    // deletion.
    NvHandle p2p_handle;

    struct
    {
        struct proc_dir_entry *peer_file[2];
        struct proc_dir_entry *peer_symlink_file[2];

        // GPU-A <-> GPU-B link is bidirectional, pairs[x][0] is always the
        // local GPU, while pairs[x][1] is the remote GPU. The table shall be
        // filled like so: [[GPU-A, GPU-B], [GPU-B, GPU-A]].
        uvm_parent_gpu_t *pairs[2][2];
    } procfs;

    // Peer-to-peer state for MIG instance pairs between two different parent
    // GPUs.
    uvm_gpu_peer_t gpu_peers[UVM_MAX_UNIQUE_SUB_PROCESSOR_PAIRS];
} uvm_parent_gpu_peer_t;

// Initialize global gpu state
NV_STATUS uvm_gpu_init(void);

// Deinitialize global state (called from module exit)
void uvm_gpu_exit(void);

NV_STATUS uvm_gpu_init_va_space(uvm_va_space_t *va_space);

void uvm_gpu_exit_va_space(uvm_va_space_t *va_space);

static unsigned int uvm_gpu_numa_node(uvm_gpu_t *gpu)
{
    if (!gpu->mem_info.numa.enabled)
        UVM_ASSERT(gpu->mem_info.numa.node_id == NUMA_NO_NODE);

    return gpu->mem_info.numa.node_id;
}

static uvm_gpu_phys_address_t uvm_gpu_page_to_phys_address(uvm_gpu_t *gpu, struct page *page)
{
    unsigned long sys_addr = page_to_pfn(page) << PAGE_SHIFT;
    unsigned long gpu_offset = sys_addr - gpu->parent->system_bus.memory_window_start;

    UVM_ASSERT(gpu->mem_info.numa.enabled);
    UVM_ASSERT(page_to_nid(page) == uvm_gpu_numa_node(gpu));
    UVM_ASSERT(sys_addr >= gpu->parent->system_bus.memory_window_start);
    UVM_ASSERT(sys_addr + PAGE_SIZE - 1 <= gpu->parent->system_bus.memory_window_end);

    return uvm_gpu_phys_address(UVM_APERTURE_VID, gpu_offset);
}

// Note that there is a uvm_gpu_get() function defined in uvm_global.h to break
// a circular dep between global and gpu modules.

// Get a uvm_gpu_t by UUID (physical GPU UUID if SMC is not enabled, otherwise
// GPU instance UUID).
// This returns NULL if the GPU is not present.
// This is the general purpose call that should be used normally.
//
// LOCKING: requires the global lock to be held
uvm_gpu_t *uvm_gpu_get_by_uuid(const NvProcessorUuid *gpu_uuid);

// Get a uvm_parent_gpu_t by UUID (physical GPU UUID).
// Like uvm_gpu_get_by_uuid(), this function returns NULL if the GPU has not
// been registered.
//
// LOCKING: requires the global lock to be held
uvm_parent_gpu_t *uvm_parent_gpu_get_by_uuid(const NvProcessorUuid *gpu_uuid);

// Like uvm_parent_gpu_get_by_uuid(), but this variant does not assertion-check
// that the caller is holding the global_lock. This is a narrower-purpose
// function, and is only intended for use by the top-half ISR, or other very
// limited cases.
uvm_parent_gpu_t *uvm_parent_gpu_get_by_uuid_locked(const NvProcessorUuid *gpu_uuid);

// Retain a gpu by uuid
// Returns the retained uvm_gpu_t in gpu_out on success
//
// LOCKING: Takes and releases the global lock for the caller.
NV_STATUS uvm_gpu_retain_by_uuid(const NvProcessorUuid *gpu_uuid,
                                 const uvm_rm_user_object_t *user_rm_device,
                                 const uvm_test_parent_gpu_inject_error_t *parent_gpu_error,
                                 uvm_gpu_t **gpu_out);

// Retain a gpu which is known to already be retained. Does NOT require the
// global lock to be held.
void uvm_gpu_retain(uvm_gpu_t *gpu);

// Release a gpu
// LOCKING: requires the global lock to be held
void uvm_gpu_release_locked(uvm_gpu_t *gpu);

// Like uvm_gpu_release_locked, but takes and releases the global lock for the
// caller.
void uvm_gpu_release(uvm_gpu_t *gpu);

static NvU64 uvm_gpu_retained_count(uvm_gpu_t *gpu)
{
    return atomic64_read(&gpu->retained_count);
}

// Decrease the refcount on the parent GPU object, and actually delete the
// object if the refcount hits zero.
void uvm_parent_gpu_kref_put(uvm_parent_gpu_t *gpu);

// Returns a GPU peer pair index in the range [0 .. UVM_MAX_UNIQUE_GPU_PAIRS).
NvU32 uvm_gpu_pair_index(const uvm_gpu_id_t id0, const uvm_gpu_id_t id1);

// Either retains an existing PCIe peer entry or creates a new one. In both
// cases the two GPUs are also each retained.
// LOCKING: requires the global lock to be held
NV_STATUS uvm_gpu_retain_pcie_peer_access(uvm_gpu_t *gpu0, uvm_gpu_t *gpu1);

// Releases a PCIe peer entry and the two GPUs.
// LOCKING: requires the global lock to be held
void uvm_gpu_release_pcie_peer_access(uvm_gpu_t *gpu0, uvm_gpu_t *gpu1);

uvm_gpu_link_type_t uvm_parent_gpu_peer_link_type(uvm_parent_gpu_t *parent_gpu0, uvm_parent_gpu_t *parent_gpu1);

// Get the aperture for local_gpu to use to map memory resident on remote_gpu.
// They must not be the same gpu.
uvm_aperture_t uvm_gpu_peer_aperture(uvm_gpu_t *local_gpu, uvm_gpu_t *remote_gpu);

// Returns the physical address for use by accessing_gpu of a vidmem allocation
// on the peer owning_gpu. This address can be used for making PTEs on
// accessing_gpu, but not for copying between the two GPUs. For that, use
// uvm_gpu_peer_copy_address.
uvm_gpu_phys_address_t uvm_gpu_peer_phys_address(uvm_gpu_t *owning_gpu, NvU64 address, uvm_gpu_t *accessing_gpu);

// Returns the physical or virtual address for use by accessing_gpu to copy to/
// from a vidmem allocation on the peer owning_gpu. This may be different from
// uvm_gpu_peer_phys_address to handle CE limitations in addressing peer
// physical memory directly.
uvm_gpu_address_t uvm_gpu_peer_copy_address(uvm_gpu_t *owning_gpu, NvU64 address, uvm_gpu_t *accessing_gpu);

// Return the reference count for the P2P state between the given GPUs.
// The two GPUs must have different parents.
NvU64 uvm_gpu_peer_ref_count(const uvm_gpu_t *gpu0, const uvm_gpu_t *gpu1);

// Get the EGM aperture for local_gpu to use to map memory resident on the CPU
// NUMA node that remote_gpu is attached to.
// Note that local_gpu can be equal to remote_gpu when memory is resident in
// CPU NUMA node local to local_gpu. In this case, the local EGM peer ID will
// be used.
uvm_aperture_t uvm_gpu_egm_peer_aperture(uvm_parent_gpu_t *local_gpu, uvm_parent_gpu_t *remote_gpu);

bool uvm_parent_gpus_are_nvswitch_connected(const uvm_parent_gpu_t *parent_gpu0, const uvm_parent_gpu_t *parent_gpu1);

static bool uvm_gpus_are_smc_peers(const uvm_gpu_t *gpu0, const uvm_gpu_t *gpu1)
{
    UVM_ASSERT(gpu0 != gpu1);

    return gpu0->parent == gpu1->parent;
}

// Retrieve the virtual address corresponding to the given vidmem physical
// address, according to the linear vidmem mapping in the GPU kernel address
// space.
//
// The actual GPU mapping only exists if a full flat mapping, or a partial flat
// mapping covering the passed address, has been previously created.
static uvm_gpu_address_t uvm_gpu_address_virtual_from_vidmem_phys(uvm_gpu_t *gpu, NvU64 pa)
{
    UVM_ASSERT(uvm_mmu_parent_gpu_needs_static_vidmem_mapping(gpu->parent) ||
               uvm_mmu_parent_gpu_needs_dynamic_vidmem_mapping(gpu->parent));
    UVM_ASSERT(pa <= gpu->mem_info.max_allocatable_address);

    if (uvm_mmu_parent_gpu_needs_static_vidmem_mapping(gpu->parent))
        UVM_ASSERT(gpu->static_flat_mapping.ready);

    return uvm_gpu_address_virtual(gpu->parent->flat_vidmem_va_base + pa);
}

// Retrieve the virtual address corresponding to the given sysmem physical
// address, according to the linear sysmem mapping in the GPU kernel address
// space.
//
// The actual GPU mapping only exists if a linear mapping covering the passed
// address has been previously created.
static uvm_gpu_address_t uvm_parent_gpu_address_virtual_from_sysmem_phys(uvm_parent_gpu_t *parent_gpu, NvU64 pa)
{
    UVM_ASSERT(uvm_mmu_parent_gpu_needs_dynamic_sysmem_mapping(parent_gpu));
    UVM_ASSERT(pa <= (parent_gpu->dma_addressable_limit - parent_gpu->dma_addressable_start));

    return uvm_gpu_address_virtual(parent_gpu->flat_sysmem_va_base + pa);
}

// Given a GPU, CPU, or EGM PEER physical address (not VIDMEM peer), retrieve an
// address suitable for CE access.
static uvm_gpu_address_t uvm_gpu_address_copy(uvm_gpu_t *gpu, uvm_gpu_phys_address_t phys_addr)
{
    UVM_ASSERT(phys_addr.aperture == UVM_APERTURE_VID || phys_addr.aperture == UVM_APERTURE_SYS);

    if (phys_addr.aperture == UVM_APERTURE_VID) {
        if (uvm_mmu_parent_gpu_needs_static_vidmem_mapping(gpu->parent) ||
            uvm_mmu_parent_gpu_needs_dynamic_vidmem_mapping(gpu->parent))
            return uvm_gpu_address_virtual_from_vidmem_phys(gpu, phys_addr.address);
    }
    else if (uvm_mmu_parent_gpu_needs_dynamic_sysmem_mapping(gpu->parent)) {
        return uvm_parent_gpu_address_virtual_from_sysmem_phys(gpu->parent, phys_addr.address);
    }

    return uvm_gpu_address_from_phys(phys_addr);
}

static uvm_gpu_identity_mapping_t *uvm_gpu_get_peer_mapping(uvm_gpu_t *gpu, uvm_gpu_id_t peer_id)
{
    return &gpu->peer_mappings[uvm_id_gpu_index(peer_id)];
}

// Check whether the provided address points to peer memory:
// * Physical address using one of the PEER apertures
// * Physical address using SYS aperture that belongs to an exposed coherent
//   memory
// * Virtual address in the region [peer_va_base, peer_va_base + peer_va_size)
bool uvm_gpu_address_is_peer(uvm_gpu_t *gpu, uvm_gpu_address_t address);

// Check for ECC errors
//
// Notably this check cannot be performed where it's not safe to call into RM.
NV_STATUS uvm_gpu_check_ecc_error(uvm_gpu_t *gpu);

// Check for ECC errors without calling into RM
//
// Calling into RM is problematic in many places, this check is always safe to
// do. Returns NV_WARN_MORE_PROCESSING_REQUIRED if there might be an ECC error
// and it's required to call uvm_gpu_check_ecc_error() to be sure.
NV_STATUS uvm_gpu_check_ecc_error_no_rm(uvm_gpu_t *gpu);

// Check for NVLINK errors
//
// Inject NVLINK error
NV_STATUS uvm_gpu_inject_nvlink_error(uvm_gpu_t *gpu, UVM_TEST_NVLINK_ERROR_TYPE error_type);

NV_STATUS uvm_gpu_get_injected_nvlink_error(uvm_gpu_t *gpu);

// Notably this check cannot be performed where it's not safe to call into RM.
NV_STATUS uvm_gpu_check_nvlink_error(uvm_gpu_t *gpu);

// Check for NVLINK errors without calling into RM
//
// Calling into RM is problematic in many places, this check is always safe to
// do. Returns NV_WARN_MORE_PROCESSING_REQUIRED if there might be an NVLINK
// error and it's required to call uvm_gpu_check_nvlink_error() to be sure.
NV_STATUS uvm_gpu_check_nvlink_error_no_rm(uvm_gpu_t *gpu);

// Map size bytes of contiguous sysmem on the GPU for physical access.
//
// size has to be aligned to PAGE_SIZE.
//
// Returns the physical address of the pages that can be used to access them on
// the GPU. This address is usable by any GPU under the same parent for the
// lifetime of that parent.
NV_STATUS uvm_gpu_map_cpu_pages(uvm_gpu_t *gpu, struct page *page, size_t size, NvU64 *dma_address_out);

// Unmap num_pages pages previously mapped with uvm_gpu_map_cpu_pages().
void uvm_parent_gpu_unmap_cpu_pages(uvm_parent_gpu_t *parent_gpu, NvU64 dma_address, size_t size);

static NV_STATUS uvm_gpu_map_cpu_page(uvm_gpu_t *gpu, struct page *page, NvU64 *dma_address_out)
{
    return uvm_gpu_map_cpu_pages(gpu, page, PAGE_SIZE, dma_address_out);
}

static void uvm_parent_gpu_unmap_cpu_page(uvm_parent_gpu_t *parent_gpu, NvU64 dma_address)
{
    uvm_parent_gpu_unmap_cpu_pages(parent_gpu, dma_address, PAGE_SIZE);
}

// Allocate and map a page of system DMA memory on the GPU for physical access
//
// Returns
// - the address of allocated memory in CPU virtual address space.
// - the address of the page that can be used to access them on
//   the GPU in the dma_address_out parameter. This address is usable by any GPU
//   under the same parent for the lifetime of that parent.
NV_STATUS uvm_gpu_dma_alloc_page(uvm_gpu_t *gpu, gfp_t gfp_flags, void **cpu_addr_out, NvU64 *dma_address_out);

// Unmap and free size bytes of contiguous sysmem DMA previously allocated
// with uvm_gpu_dma_alloc_page().
void uvm_parent_gpu_dma_free_page(uvm_parent_gpu_t *parent_gpu, void *cpu_addr, NvU64 dma_address);

// Returns whether the given range is within the GPU's addressable VA ranges.
// It requires the input 'addr' to be in canonical form for platforms compliant
// to canonical form addresses, i.e., ARM64, and x86.
// Warning: This only checks whether the GPU's MMU can support the given
// address. Some HW units on that GPU might only support a smaller range.
//
// The GPU must be initialized before calling this function.
bool uvm_gpu_can_address(uvm_gpu_t *gpu, NvU64 addr, NvU64 size);

// Returns whether the given range is within the GPU's addressable VA ranges in
// the internal GPU VA "kernel" address space, which is a linear address space.
// Therefore, the input 'addr' must not be in canonical form, even platforms
// that use to the canonical form addresses, i.e., ARM64, and x86.
// Warning: This only checks whether the GPU's MMU can support the given
// address. Some HW units on that GPU might only support a smaller range.
//
// The GPU must be initialized before calling this function.
bool uvm_gpu_can_address_kernel(uvm_gpu_t *gpu, NvU64 addr, NvU64 size);

// Returns addr's canonical form for host systems that use canonical form
// addresses.
NvU64 uvm_parent_gpu_canonical_address(uvm_parent_gpu_t *parent_gpu, NvU64 addr);

static bool uvm_parent_gpu_is_coherent(const uvm_parent_gpu_t *parent_gpu)
{
    return parent_gpu->system_bus.memory_window_end > parent_gpu->system_bus.memory_window_start;
}

static bool uvm_parent_gpu_needs_pushbuffer_segments(uvm_parent_gpu_t *parent_gpu)
{
    return parent_gpu->max_host_va > (1ull << 40);
}

static bool uvm_parent_gpu_supports_eviction(uvm_parent_gpu_t *parent_gpu)
{
    // Eviction is supported only if the GPU supports replayable faults
    return parent_gpu->replayable_faults_supported;
}

static bool uvm_parent_gpu_is_virt_mode_sriov_heavy(const uvm_parent_gpu_t *parent_gpu)
{
    return parent_gpu->virt_mode == UVM_VIRT_MODE_SRIOV_HEAVY;
}

static bool uvm_parent_gpu_is_virt_mode_sriov_standard(const uvm_parent_gpu_t *parent_gpu)
{
    return parent_gpu->virt_mode == UVM_VIRT_MODE_SRIOV_STANDARD;
}

// Returns true if the virtualization mode is SR-IOV heavy or SR-IOV standard.
static bool uvm_parent_gpu_is_virt_mode_sriov(const uvm_parent_gpu_t *parent_gpu)
{
    return uvm_parent_gpu_is_virt_mode_sriov_heavy(parent_gpu) ||
           uvm_parent_gpu_is_virt_mode_sriov_standard(parent_gpu);
}

static bool uvm_parent_gpu_needs_proxy_channel_pool(const uvm_parent_gpu_t *parent_gpu)
{
    return uvm_parent_gpu_is_virt_mode_sriov_heavy(parent_gpu);
}

uvm_aperture_t uvm_get_page_tree_location(const uvm_gpu_t *gpu);

// Add the given instance pointer -> user_channel mapping to this GPU. The
// bottom half GPU page fault handler uses this to look up the VA space for GPU
// faults.
NV_STATUS uvm_parent_gpu_add_user_channel(uvm_parent_gpu_t *parent_gpu, uvm_user_channel_t *user_channel);
void uvm_parent_gpu_remove_user_channel(uvm_parent_gpu_t *parent_gpu, uvm_user_channel_t *user_channel);

// Looks up an entry added by uvm_gpu_add_user_channel. Return codes:
//  NV_OK                        Translation successful
//  NV_ERR_INVALID_CHANNEL       Entry's instance pointer was not found
//  NV_ERR_PAGE_TABLE_NOT_AVAIL  Entry's instance pointer is valid but the entry
//                               targets an invalid subcontext
//
// out_va_space is valid if NV_OK is returned, otherwise it's NULL.
// out_gpu is valid if NV_OK is returned, otherwise it's NULL.
// The caller is responsible for ensuring that the returned va_space and gpu
// can't be destroyed, so this function should only be called from the bottom
// half.
NV_STATUS uvm_parent_gpu_fault_entry_to_va_space(uvm_parent_gpu_t *parent_gpu,
                                                 const uvm_fault_buffer_entry_t *fault,
                                                 uvm_va_space_t **out_va_space,
                                                 uvm_gpu_t **out_gpu);

// Return the GPU VA space for the given instance pointer and ve_id in the
// access counter entry. This function can only be used for virtual address
// entries.
// The return values are the same as uvm_parent_gpu_fault_entry_to_va_space()
// but for virtual access counter entries.
NV_STATUS uvm_parent_gpu_access_counter_entry_to_va_space(uvm_parent_gpu_t *parent_gpu,
                                                          const uvm_access_counter_buffer_entry_t *entry,
                                                          uvm_va_space_t **out_va_space,
                                                          uvm_gpu_t **out_gpu);

typedef enum
{
    UVM_GPU_BUFFER_FLUSH_MODE_CACHED_PUT,
    UVM_GPU_BUFFER_FLUSH_MODE_UPDATE_PUT,
    UVM_GPU_BUFFER_FLUSH_MODE_WAIT_UPDATE_PUT,
} uvm_gpu_buffer_flush_mode_t;

// PCIe BAR containing static framebuffer memory mappings for PCIe P2P
int uvm_device_p2p_static_bar(uvm_gpu_t *gpu);

#endif // __UVM_GPU_H__
