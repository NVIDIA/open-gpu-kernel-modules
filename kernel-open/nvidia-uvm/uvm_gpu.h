/*******************************************************************************
    Copyright (c) 2015-2022 NVIDIA Corporation

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

// Buffer length to store uvm gpu id, RM device name and gpu uuid.
#define UVM_GPU_NICE_NAME_BUFFER_LENGTH (sizeof("ID 999: : ") + \
            UVM_GPU_NAME_LENGTH + UVM_GPU_UUID_TEXT_BUFFER_LENGTH)

#define UVM_GPU_MAGIC_VALUE 0xc001d00d12341993ULL

typedef struct
{
    // Number of faults from this uTLB that have been fetched but have not been serviced yet
    NvU32 num_pending_faults;

    // Whether the uTLB contains fatal faults
    bool has_fatal_faults;

    // We have issued a replay of type START_ACK_ALL while containing fatal faults. This puts
    // the uTLB in lockdown mode and no new translations are accepted
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
        // fault handler returns, but after the VA space lock has been unlocked to
        // avoid the RM/UVM VA space lock deadlocks.
        uvm_processor_mask_t gpus_to_check_for_ecc;

        // This is set to throttle page fault thrashing.
        NvU64 wakeup_time_stamp;

        // This is set if the page migrated to/from the GPU and CPU.
        bool did_migrate;
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

    struct
    {
        // Per-processor mask with the pages that will be resident after servicing.
        // We need one mask per processor because we may coalesce faults that
        // trigger migrations to different processors.
        uvm_page_mask_t new_residency;
    } per_processor_masks[UVM_ID_MAX_PROCESSORS];

    // State used by the VA block routines called by the servicing routine
    uvm_va_block_context_t block_context;

    // Prefetch state hint
    uvm_perf_prefetch_hint_t prefetch_hint;

    // Prefetch temporary state.
    uvm_perf_prefetch_bitmap_tree_t prefetch_bitmap_tree;
};

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

    bool has_fatal_faults;

    bool has_throttled_faults;

    NvU32 num_invalid_prefetch_faults;

    NvU32 num_duplicate_faults;

    NvU32 num_replays;

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
    // Whether the TLB batch contains any information
    bool            write_faults_in_batch;

    // Batch of TLB entries to be invalidated
    uvm_tlb_batch_t write_faults_tlb_batch;
};

typedef struct
{
    // Fault buffer information and structures provided by RM
    UvmGpuFaultInfo rm_info;

    // Maximum number of faults to be processed in batch before fetching new
    // entries from the GPU buffer
    NvU32 max_batch_size;

    struct uvm_replayable_fault_buffer_info_struct
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

    struct uvm_non_replayable_fault_buffer_info_struct
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

        // Information required to invalidate stale ATS PTEs from the GPU TLBs
        uvm_ats_fault_invalidate_t ats_invalidate;
    } non_replayable;

    // Flag that tells if prefetch faults are enabled in HW
    bool prefetch_faults_enabled;

    // Timestamp when prefetch faults where disabled last time
    NvU64 disable_prefetch_faults_timestamp;
} uvm_fault_buffer_info_t;

typedef struct
{
    // True if the platform supports HW coherence (P9) and RM has exposed the
    // GPU's memory as a NUMA node to the kernel.
    bool enabled;

    // Range in the system physical address space where the memory of this GPU
    // is mapped
    NvU64 system_memory_window_start;
    NvU64 system_memory_window_end;

    NvU64 memblock_size;

    unsigned node_id;
} uvm_numa_info_t;

struct uvm_access_counter_service_batch_context_struct
{
    uvm_access_counter_buffer_entry_t *notification_cache;

    NvU32 num_cached_notifications;

    struct
    {
        uvm_access_counter_buffer_entry_t   **notifications;

        NvU32                             num_notifications;

        // Boolean used to avoid sorting the fault batch by instance_ptr if we
        // determine at fetch time that all the access counter notifications in the
        // batch report the same instance_ptr
        bool is_single_instance_ptr;

        // Scratch space, used to generate artificial physically addressed notifications.
        // Virtual address notifications are always aligned to 64k. This means up to 16
        // different physical locations could have been accessed to trigger one notification.
        // The sub-granularity mask can correspond to any of them.
        struct {
            uvm_processor_id_t resident_processors[16];
            uvm_gpu_phys_address_t phys_addresses[16];
            uvm_access_counter_buffer_entry_t phys_entry;
        } scratch;
    } virt;

    struct
    {
        uvm_access_counter_buffer_entry_t    **notifications;
        uvm_reverse_map_t                      *translations;

        NvU32                              num_notifications;

        // Boolean used to avoid sorting the fault batch by aperture if we
        // determine at fetch time that all the access counter notifications in the
        // batch report the same aperture
        bool                              is_single_aperture;
    } phys;

    // Helper page mask to compute the accessed pages within a VA block
    uvm_page_mask_t accessed_pages;

    // Structure used to coalesce access counter servicing in a VA block
    uvm_service_block_context_t block_service_context;

    // Unique id (per-GPU) generated for tools events recording
    NvU32 batch_id;
};

typedef struct
{
    // Values used to configure access counters in RM
    struct
    {
        UVM_ACCESS_COUNTER_GRANULARITY  granularity;
        UVM_ACCESS_COUNTER_USE_LIMIT    use_limit;
    } rm;

    // The following values are precomputed by the access counter notification
    // handling code. See comments for UVM_MAX_TRANSLATION_SIZE in
    // uvm_gpu_access_counters.c for more details.
    NvU64 translation_size;

    NvU64 translations_per_counter;

    NvU64 sub_granularity_region_size;

    NvU64 sub_granularity_regions_per_translation;
} uvm_gpu_access_counter_type_config_t;

typedef struct
{
    UvmGpuAccessCntrInfo rm_info;

    NvU32 max_notifications;

    NvU32 max_batch_size;

    // Cached value of the GPU GET register to minimize the round-trips
    // over PCIe
    NvU32 cached_get;

    // Cached value of the GPU PUT register to minimize the round-trips over
    // PCIe
    NvU32 cached_put;

    // Tracker used to aggregate access counters clear operations, needed for
    // GPU removal
    uvm_tracker_t clear_tracker;

    // Current access counter configuration. During normal operation this
    // information is computed once during GPU initialization. However, tests
    // may override it to try different configuration values.
    struct
    {
        uvm_gpu_access_counter_type_config_t mimc;
        uvm_gpu_access_counter_type_config_t momc;

        NvU32                                threshold;
    } current_config;

    // Access counter statistics
    struct
    {
        atomic64_t num_pages_out;

        atomic64_t num_pages_in;
    } stats;

    // Ignoring access counters means that notifications are left in the HW
    // buffer without being serviced.  Requests to ignore access counters
    // are counted since the suspend path inhibits access counter interrupts,
    // and the resume path needs to know whether to reenable them.
    NvU32 notifications_ignored_count;

    // Context structure used to service a GPU access counter batch
    uvm_access_counter_service_batch_context_t batch_service_context;

    // VA space that reconfigured the access counters configuration, if any.
    // Used in builtin tests only, to avoid reconfigurations from different
    // processes
    //
    // Locking: both readers and writers must hold the access counters ISR lock
    uvm_va_space_t *reconfiguration_owner;
} uvm_access_counter_buffer_info_t;

typedef struct
{
    // VA where the identity mapping should be mapped in the internal VA
    // space managed by uvm_gpu_t.address_space_tree (see below).
    NvU64 base;

    // Page tables with the mapping.
    uvm_page_table_range_vec_t *range_vec;
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
    UVM_GPU_LINK_C2C,
    UVM_GPU_LINK_MAX
} uvm_gpu_link_type_t;

// UVM does not support P2P copies on pre-Pascal GPUs. Pascal+ GPUs only
// support virtual addresses in P2P copies. Therefore, a peer identity mapping
// needs to be created.
// Ampere+ GPUs support physical peer copies, too, so identity mappings are not
// needed
typedef enum
{
    UVM_GPU_PEER_COPY_MODE_UNSUPPORTED,
    UVM_GPU_PEER_COPY_MODE_VIRTUAL,
    UVM_GPU_PEER_COPY_MODE_PHYSICAL,
    UVM_GPU_PEER_COPY_MODE_COUNT
} uvm_gpu_peer_copy_mode_t;

struct uvm_gpu_struct
{
    uvm_parent_gpu_t *parent;

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

    // A unique uvm gpu id in range [1, UVM_ID_MAX_PROCESSORS); this is a copy
    // of the parent's id.
    uvm_gpu_id_t id;

    // A unique uvm global_gpu id in range [1, UVM_GLOBAL_ID_MAX_PROCESSORS)
    uvm_global_gpu_id_t global_id;

    // Should be UVM_GPU_MAGIC_VALUE. Used for memory checking.
    NvU64 magic;

    struct
    {
        // The amount of memory the GPU has in total, in bytes. If the GPU is in
        // ZeroFB testing mode, this will be 0.
        NvU64 size;

        // Max (inclusive) physical address of this GPU's memory that the driver
        // can allocate through PMM (PMA).
        NvU64 max_allocatable_address;
    } mem_info;

    struct
    {
        // Big page size used by the internal UVM VA space
        // Notably it may be different than the big page size used by a user's VA
        // space in general.
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
        // Mask of peer_gpus set
        //
        // We can use a regular processor id because P2P is not allowed between
        // partitioned GPUs when SMC is enabled
        uvm_processor_mask_t peer_gpu_mask;

        // lazily-populated array of peer GPUs, indexed by the peer's GPU index
        uvm_gpu_t *peer_gpus[UVM_ID_MAX_GPUS];

        // Leaf spinlock used to synchronize access to the peer_gpus table so that
        // it can be safely accessed from the access counters bottom half
        uvm_spinlock_t peer_gpus_lock;
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

    struct
    {
        NvU32 swizz_id;

        uvmGpuSessionHandle rm_session_handle;

        // RM device handle used in many of the UVM/RM APIs.
        //
        // Do not read this field directly, use uvm_gpu_device_handle instead.
        uvmGpuDeviceHandle rm_device;
    } smc;

    struct
    {
        struct proc_dir_entry *dir;

        struct proc_dir_entry *dir_symlink;

        struct proc_dir_entry *info_file;

        struct proc_dir_entry *dir_peers;
    } procfs;

    // Placeholder for per-GPU performance heuristics information
    uvm_perf_module_data_desc_t perf_modules_data[UVM_PERF_MODULE_TYPE_COUNT];

    // Force pushbuffer's GPU VA to be >= 1TB; used only for testing purposes.
    bool uvm_test_force_upper_pushbuffer_segment;
};

struct uvm_parent_gpu_struct
{
    // Reference count for how many places are holding on to a parent GPU
    // (internal to the UVM driver).  This includes any GPUs we know about, not
    // just GPUs that are registered with a VA space.  Most GPUs end up being
    // registered, but there are brief periods when they are not registered,
    // such as during interrupt handling, and in add_gpu() or remove_gpu().
    nv_kref_t gpu_kref;

    // The number of uvm_gpu_ts referencing this uvm_parent_gpu_t.
    NvU32 num_retained_gpus;

    uvm_gpu_t *gpus[UVM_ID_MAX_SUB_PROCESSORS];

    // Bitmap of valid child entries in the gpus[] table.  Used to retrieve a
    // usable child GPU in bottom-halves.
    DECLARE_BITMAP(valid_gpus, UVM_ID_MAX_SUB_PROCESSORS);

    // The gpu's uuid
    NvProcessorUuid uuid;

    // Nice printable name including the uvm gpu id, ascii name from RM and uuid
    char name[UVM_GPU_NICE_NAME_BUFFER_LENGTH];

    // GPU information and provided by RM (architecture, implementation,
    // hardware classes, etc.).
    UvmGpuInfo rm_info;

    // A unique uvm gpu id in range [1, UVM_ID_MAX_PROCESSORS)
    uvm_gpu_id_t id;

    // Reference to the Linux PCI device
    //
    // The reference to the PCI device remains valid as long as the GPU is
    // registered with RM's Linux layer (between nvUvmInterfaceRegisterGpu() and
    // nvUvmInterfaceUnregisterGpu()).
    struct pci_dev *pci_dev;

    // NVLINK Processing Unit (NPU) on PowerPC platforms. The NPU is a
    // collection of CPU-side PCI devices which bridge GPU NVLINKs and the CPU
    // memory bus.
    //
    // There is one PCI device per NVLINK. A set of NVLINKs connects to a single
    // GPU, and all NVLINKs for a given socket are collected logically under
    // this UVM NPU because some resources (such as register mappings) are
    // shared by all those NVLINKs. This means multiple GPUs may connect to the
    // same UVM NPU.
    uvm_ibm_npu_t *npu;

    // On kernels with NUMA support, this entry contains the closest CPU NUMA
    // node to this GPU. Otherwise, the value will be -1.
    int closest_cpu_numa_node;

    // RM device handle used in many of the UVM/RM APIs.
    //
    // Do not read this field directly, use uvm_gpu_device_handle instead.
    uvmGpuDeviceHandle rm_device;

    // The physical address range addressable by the GPU
    //
    // The GPU has its NV_PFB_XV_UPPER_ADDR register set by RM to
    // dma_addressable_start (in bifSetupDmaWindow_IMPL()) and hence when
    // referencing sysmem from the GPU, dma_addressable_start should be
    // subtracted from the physical address. The DMA mapping helpers like
    // uvm_gpu_map_cpu_pages() and uvm_gpu_dma_alloc_page() take care of that.
    NvU64 dma_addressable_start;
    NvU64 dma_addressable_limit;

    // Total size (in bytes) of physically mapped (with uvm_gpu_map_cpu_pages)
    // sysmem pages, used for leak detection.
    atomic64_t mapped_cpu_pages_size;

    // Hardware Abstraction Layer
    uvm_host_hal_t *host_hal;
    uvm_ce_hal_t *ce_hal;
    uvm_arch_hal_t *arch_hal;
    uvm_fault_buffer_hal_t *fault_buffer_hal;
    uvm_access_counter_buffer_hal_t *access_counter_buffer_hal;

    uvm_gpu_peer_copy_mode_t peer_copy_mode;

    // Virtualization mode of the GPU.
    UVM_VIRT_MODE virt_mode;

    // Whether the GPU can trigger faults on prefetch instructions
    bool prefetch_fault_supported;

    // Number of membars required to flush out HSHUB following a TLB invalidate
    NvU32 num_hshub_tlb_invalidate_membars;

    // Whether the channels can configure GPFIFO in vidmem
    bool gpfifo_in_vidmem_supported;

    bool replayable_faults_supported;

    bool non_replayable_faults_supported;

    bool access_counters_supported;

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
        struct proc_dir_entry *dir;

        struct proc_dir_entry *fault_stats_file;

        struct proc_dir_entry *access_counters_file;
    } procfs;

    // Interrupt handling state and locks
    uvm_isr_info_t isr;

    // Fault buffer info. This is only valid if supports_replayable_faults is set to true
    uvm_fault_buffer_info_t fault_buffer_info;

    // NUMA info, mainly for ATS
    uvm_numa_info_t numa_info;

    // Access counter buffer info. This is only valid if supports_access_counters is set to true
    uvm_access_counter_buffer_info_t access_counter_buffer_info;

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

    // This is set to true if the GPU belongs to an SLI group. Else, set to false.
    bool sli_enabled;

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
    // physical address spaces are routed through the switch to the corresponding
    // peer.
    struct
    {
        bool is_nvswitch_connected;

        // 47-bit fabric memory physical offset that peer gpus need to access
        // to read a peer's memory
        NvU64 fabric_memory_window_start;
    } nvswitch_info;

    uvm_gpu_link_type_t sysmem_link;
    NvU32 sysmem_link_rate_mbyte_per_s;
};

static const char *uvm_gpu_name(uvm_gpu_t *gpu)
{
    return gpu->parent->name;
}

static const NvProcessorUuid *uvm_gpu_uuid(uvm_gpu_t *gpu)
{
    return &gpu->parent->uuid;
}

static uvmGpuDeviceHandle uvm_gpu_device_handle(uvm_gpu_t *gpu)
{
    if (gpu->parent->smc.enabled)
        return gpu->smc.rm_device;
    return gpu->parent->rm_device;
}

struct uvm_gpu_peer_struct
{
    // The fields in this global structure can only be inspected under one of
    // the following conditions:
    //
    // - The VA space lock is held for either read or write, both GPUs are
    //   registered in the VA space, and the corresponding bit in the
    //   va_space.enabled_peers bitmap is set.
    //
    // - The global lock is held.
    //
    // - While the global lock was held in the past, the two GPUs were detected
    //   to be NVLINK peers and were both retained.
    //
    // - While the global lock was held in the past, the two GPUs were detected
    //   to be PCIe peers and uvm_gpu_retain_pcie_peer_access() was called.
    //
    // - The peer_gpus_lock is held on one of the GPUs. In this case, the other
    //   GPU must be read from the original GPU's peer_gpus table. The fields
    //   will not change while the lock is held, but they may no longer be valid
    //   because the other GPU might be in teardown.

    // Peer Id associated with this device w.r.t. to a peer GPU.
    // Note: peerId (A -> B) != peerId (B -> A)
    // peer_id[0] from min(gpu_id_1, gpu_id_2) -> max(gpu_id_1, gpu_id_2)
    // peer_id[1] from max(gpu_id_1, gpu_id_2) -> min(gpu_id_1, gpu_id_2)
    NvU8 peer_ids[2];

    // Indirect peers are GPUs which can coherently access each others' memory
    // over NVLINK, but are routed through the CPU using the SYS aperture rather
    // than a PEER aperture
    NvU8 is_indirect_peer : 1;

    // The link type between the peer GPUs, currently either PCIe or NVLINK.
    // This field is used to determine the when this peer struct has been
    // initialized (link_type != UVM_GPU_LINK_INVALID). NVLink peers are
    // initialized at GPU registration time. PCIe peers are initialized when
    // the refcount below goes from 0 to 1.
    uvm_gpu_link_type_t link_type;

    // Maximum unidirectional bandwidth between the peers in megabytes per
    // second, not taking into account the protocols' overhead. The reported
    // bandwidth for indirect peers is zero. See UvmGpuP2PCapsParams.
    NvU32 total_link_line_rate_mbyte_per_s;

    // For PCIe, the number of times that this has been retained by a VA space.
    // For NVLINK this will always be 1.
    NvU64 ref_count;

    // This handle gets populated when enable_peer_access successfully creates
    // an NV50_P2P object. disable_peer_access resets the same on the object
    // deletion.
    NvHandle p2p_handle;

    struct {
        struct proc_dir_entry *peer_file[2];
        struct proc_dir_entry *peer_symlink_file[2];

        // GPU-A <-> GPU-B link is bidirectional, pairs[x][0] is always the
        // local GPU, while pairs[x][1] is the remote GPU. The table shall be
        // filled like so: [[GPU-A, GPU-B], [GPU-B, GPU-A]].
        uvm_gpu_t *pairs[2][2];
    } procfs;
};

// Initialize global gpu state
NV_STATUS uvm_gpu_init(void);

// Deinitialize global state (called from module exit)
void uvm_gpu_exit(void);

NV_STATUS uvm_gpu_init_va_space(uvm_va_space_t *va_space);

void uvm_gpu_exit_va_space(uvm_va_space_t *va_space);

static uvm_numa_info_t *uvm_gpu_numa_info(uvm_gpu_t *gpu)
{
    UVM_ASSERT(gpu->parent->numa_info.enabled);

    return &gpu->parent->numa_info;
}

static uvm_gpu_phys_address_t uvm_gpu_page_to_phys_address(uvm_gpu_t *gpu, struct page *page)
{
    uvm_numa_info_t *numa_info = uvm_gpu_numa_info(gpu);

    unsigned long sys_addr = page_to_pfn(page) << PAGE_SHIFT;
    unsigned long gpu_offset = sys_addr - numa_info->system_memory_window_start;

    UVM_ASSERT(page_to_nid(page) == numa_info->node_id);
    UVM_ASSERT(sys_addr >= numa_info->system_memory_window_start);
    UVM_ASSERT(sys_addr + PAGE_SIZE - 1 <= numa_info->system_memory_window_end);

    return uvm_gpu_phys_address(UVM_APERTURE_VID, gpu_offset);
}

// Note that there is a uvm_gpu_get() function defined in uvm_global.h to break
// a circular dep between global and gpu modules.

// Get a uvm_gpu_t by UUID.  This returns NULL if the GPU is not present.  This
// is the general purpose call that should be used normally.
// That is, unless a uvm_gpu_t for a specific SMC partition needs to be
// retrieved, in which case uvm_gpu_get_by_parent_and_swizz_id() must be used
// instead.
//
// LOCKING: requires the global lock to be held
uvm_gpu_t *uvm_gpu_get_by_uuid(const NvProcessorUuid *gpu_uuid);

// Get a uvm_parent_gpu_t by UUID.  Like uvm_gpu_get_by_uuid(), this function
// returns NULL if the GPU has not been registered.
//
// LOCKING: requires the global lock to be held
uvm_parent_gpu_t *uvm_parent_gpu_get_by_uuid(const NvProcessorUuid *gpu_uuid);

// Like uvm_parent_gpu_get_by_uuid(), but this variant does not assertion-check
// that the caller is holding the global_lock.  This is a narrower-purpose
// function, and is only intended for use by the top-half ISR, or other very
// limited cases.
uvm_parent_gpu_t *uvm_parent_gpu_get_by_uuid_locked(const NvProcessorUuid *gpu_uuid);

// Get the uvm_gpu_t for a partition by parent and swizzId. This returns NULL if
// the partition hasn't been registered. This call needs to be used instead of
// uvm_gpu_get_by_uuid() when a specific partition is targeted.
//
// LOCKING: requires the global lock to be held
uvm_gpu_t *uvm_gpu_get_by_parent_and_swizz_id(uvm_parent_gpu_t *parent_gpu, NvU32 swizz_id);

// Retain a gpu by uuid
// Returns the retained uvm_gpu_t in gpu_out on success
//
// LOCKING: Takes and releases the global lock for the caller.
NV_STATUS uvm_gpu_retain_by_uuid(const NvProcessorUuid *gpu_uuid,
                                 const uvm_rm_user_object_t *user_rm_device,
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

// Decrease the refcount on the parent GPU object, and actually delete the object
// if the refcount hits zero.
void uvm_parent_gpu_kref_put(uvm_parent_gpu_t *gpu);

// Calculates peer table index using GPU ids.
NvU32 uvm_gpu_peer_table_index(uvm_gpu_id_t gpu_id1, uvm_gpu_id_t gpu_id2);

// Either retains an existing PCIe peer entry or creates a new one. In both
// cases the two GPUs are also each retained.
// LOCKING: requires the global lock to be held
NV_STATUS uvm_gpu_retain_pcie_peer_access(uvm_gpu_t *gpu0, uvm_gpu_t *gpu1);

// Releases a PCIe peer entry and the two GPUs.
// LOCKING: requires the global lock to be held
void uvm_gpu_release_pcie_peer_access(uvm_gpu_t *gpu0, uvm_gpu_t *gpu1);

// Get the aperture for local_gpu to use to map memory resident on remote_gpu.
// They must not be the same gpu.
uvm_aperture_t uvm_gpu_peer_aperture(uvm_gpu_t *local_gpu, uvm_gpu_t *remote_gpu);

// Get the processor id accessible by the given GPU for the given physical address
uvm_processor_id_t uvm_gpu_get_processor_id_by_address(uvm_gpu_t *gpu, uvm_gpu_phys_address_t addr);

// Get the P2P capabilities between the gpus with the given indexes
uvm_gpu_peer_t *uvm_gpu_index_peer_caps(uvm_gpu_id_t gpu_id1, uvm_gpu_id_t gpu_id2);

// Get the P2P capabilities between the given gpus
static uvm_gpu_peer_t *uvm_gpu_peer_caps(const uvm_gpu_t *gpu0, const uvm_gpu_t *gpu1)
{
    return uvm_gpu_index_peer_caps(gpu0->id, gpu1->id);
}

static bool uvm_gpus_are_nvswitch_connected(uvm_gpu_t *gpu1, uvm_gpu_t *gpu2)
{
    if (gpu1->parent->nvswitch_info.is_nvswitch_connected && gpu2->parent->nvswitch_info.is_nvswitch_connected) {
        UVM_ASSERT(uvm_gpu_peer_caps(gpu1, gpu2)->link_type >= UVM_GPU_LINK_NVLINK_2);
        return true;
    }

    return false;
}

static bool uvm_gpus_are_indirect_peers(uvm_gpu_t *gpu0, uvm_gpu_t *gpu1)
{
    uvm_gpu_peer_t *peer_caps = uvm_gpu_peer_caps(gpu0, gpu1);

    if (peer_caps->link_type != UVM_GPU_LINK_INVALID && peer_caps->is_indirect_peer) {
        UVM_ASSERT(gpu0->parent->numa_info.enabled);
        UVM_ASSERT(gpu1->parent->numa_info.enabled);
        UVM_ASSERT(peer_caps->link_type != UVM_GPU_LINK_PCIE);
        UVM_ASSERT(!uvm_gpus_are_nvswitch_connected(gpu0, gpu1));
        return true;
    }

    return false;
}

// Retrieve the virtual address corresponding to the given vidmem physical
// address, according to the linear vidmem mapping in the GPU kernel address
// space.
//
// The actual GPU mapping only exists if a full flat mapping, or a partial flat
// mapping covering the passed address, has been previously created.
static uvm_gpu_address_t uvm_gpu_address_virtual_from_vidmem_phys(uvm_gpu_t *gpu, NvU64 pa)
{
    UVM_ASSERT(uvm_mmu_gpu_needs_static_vidmem_mapping(gpu) || uvm_mmu_gpu_needs_dynamic_vidmem_mapping(gpu));
    UVM_ASSERT(pa <= gpu->mem_info.max_allocatable_address);

    return uvm_gpu_address_virtual(gpu->parent->flat_vidmem_va_base + pa);
}

// Retrieve the virtual address corresponding to the given sysmem physical
// address, according to the linear sysmem mapping in the GPU kernel address
// space.
//
// The actual GPU mapping only exists if a linear mapping covering the passed
// address has been previously created.
static uvm_gpu_address_t uvm_gpu_address_virtual_from_sysmem_phys(uvm_gpu_t *gpu, NvU64 pa)
{
    UVM_ASSERT(uvm_mmu_gpu_needs_dynamic_sysmem_mapping(gpu));
    UVM_ASSERT(pa <= (gpu->parent->dma_addressable_limit - gpu->parent->dma_addressable_start));

    return uvm_gpu_address_virtual(gpu->parent->flat_sysmem_va_base + pa);
}

static uvm_gpu_identity_mapping_t *uvm_gpu_get_peer_mapping(uvm_gpu_t *gpu, uvm_gpu_id_t peer_id)
{
    return &gpu->peer_mappings[uvm_id_gpu_index(peer_id)];
}

// Check for ECC errors
//
// Notably this check cannot be performed where it's not safe to call into RM.
NV_STATUS uvm_gpu_check_ecc_error(uvm_gpu_t *gpu);

// Check for ECC errors without calling into RM
//
// Calling into RM is problematic in many places, this check is always safe to do.
// Returns NV_WARN_MORE_PROCESSING_REQUIRED if there might be an ECC error and
// it's required to call uvm_gpu_check_ecc_error() to be sure.
NV_STATUS uvm_gpu_check_ecc_error_no_rm(uvm_gpu_t *gpu);

// Map size bytes of contiguous sysmem on the GPU for physical access
//
// size has to be aligned to PAGE_SIZE.
//
// Returns the physical address of the pages that can be used to access them on
// the GPU.
NV_STATUS uvm_gpu_map_cpu_pages(uvm_parent_gpu_t *parent_gpu, struct page *page, size_t size, NvU64 *dma_address_out);

// Unmap num_pages pages previously mapped with uvm_gpu_map_cpu_pages().
void uvm_gpu_unmap_cpu_pages(uvm_parent_gpu_t *parent_gpu, NvU64 dma_address, size_t size);

static NV_STATUS uvm_gpu_map_cpu_page(uvm_parent_gpu_t *parent_gpu, struct page *page, NvU64 *dma_address_out)
{
    return uvm_gpu_map_cpu_pages(parent_gpu, page, PAGE_SIZE, dma_address_out);
}

static void uvm_gpu_unmap_cpu_page(uvm_parent_gpu_t *parent_gpu, NvU64 dma_address)
{
    uvm_gpu_unmap_cpu_pages(parent_gpu, dma_address, PAGE_SIZE);
}

// Allocate and map a page of system DMA memory on the GPU for physical access
//
// Returns
// - the address of the page that can be used to access them on
//   the GPU in the dma_address_out parameter.
// - the address of allocated memory in CPU virtual address space.
void *uvm_gpu_dma_alloc_page(uvm_parent_gpu_t *parent_gpu,
                             gfp_t gfp_flags,
                             NvU64 *dma_address_out);

// Unmap and free size bytes of contiguous sysmem DMA previously allocated
// with uvm_gpu_map_cpu_pages().
void uvm_gpu_dma_free_page(uvm_parent_gpu_t *parent_gpu, void *va, NvU64 dma_address);

// Returns whether the given range is within the GPU's addressable VA ranges.
// It requires the input 'addr' to be in canonical form for platforms compliant
// to canonical form addresses, i.e., ARM64, and x86.
// Warning: This only checks whether the GPU's MMU can support the given
// address. Some HW units on that GPU might only support a smaller range.
//
// The GPU must be initialized before calling this function.
bool uvm_gpu_can_address(uvm_gpu_t *gpu, NvU64 addr, NvU64 size);

// Returns addr's canonical form for host systems that use canonical form
// addresses.
NvU64 uvm_parent_gpu_canonical_address(uvm_parent_gpu_t *parent_gpu, NvU64 addr);

static bool uvm_gpu_has_pushbuffer_segments(uvm_gpu_t *gpu)
{
    return gpu->parent->max_host_va > (1ull << 40);
}

static bool uvm_gpu_supports_eviction(uvm_gpu_t *gpu)
{
    // Eviction is supported only if the GPU supports replayable faults
    return gpu->parent->replayable_faults_supported;
}

static bool uvm_gpu_is_virt_mode_sriov_heavy(const uvm_gpu_t *gpu)
{
    return gpu->parent->virt_mode == UVM_VIRT_MODE_SRIOV_HEAVY;
}

static bool uvm_gpu_is_virt_mode_sriov_standard(const uvm_gpu_t *gpu)
{
    return gpu->parent->virt_mode == UVM_VIRT_MODE_SRIOV_STANDARD;
}

// Returns true if the virtualization mode is SR-IOV heavy or SR-IOV standard.
static bool uvm_gpu_is_virt_mode_sriov(const uvm_gpu_t *gpu)
{
    return uvm_gpu_is_virt_mode_sriov_heavy(gpu) || uvm_gpu_is_virt_mode_sriov_standard(gpu);
}

static bool uvm_gpu_uses_proxy_channel_pool(const uvm_gpu_t *gpu)
{
    return uvm_gpu_is_virt_mode_sriov_heavy(gpu);
}

uvm_aperture_t uvm_gpu_page_tree_init_location(const uvm_gpu_t *gpu);

// Debug print of GPU properties
void uvm_gpu_print(uvm_gpu_t *gpu);

// Add the given instance pointer -> user_channel mapping to this GPU. The bottom
// half GPU page fault handler uses this to look up the VA space for GPU faults.
NV_STATUS uvm_gpu_add_user_channel(uvm_gpu_t *gpu, uvm_user_channel_t *user_channel);
void uvm_gpu_remove_user_channel(uvm_gpu_t *gpu, uvm_user_channel_t *user_channel);

// Looks up an entry added by uvm_gpu_add_user_channel. Return codes:
//  NV_OK                        Translation successful
//  NV_ERR_INVALID_CHANNEL       Entry's instance pointer was not found
//  NV_ERR_PAGE_TABLE_NOT_AVAIL  Entry's instance pointer is valid but the entry
//                               targets an invalid subcontext
//
// out_va_space is valid if NV_OK is returned, otherwise it's NULL. The caller
// is responsibile for ensuring that the returned va_space can't be destroyed,
// so these functions should only be called from the bottom half.
NV_STATUS uvm_gpu_fault_entry_to_va_space(uvm_gpu_t *gpu,
                                          uvm_fault_buffer_entry_t *fault,
                                          uvm_va_space_t **out_va_space);

NV_STATUS uvm_gpu_access_counter_entry_to_va_space(uvm_gpu_t *gpu,
                                                   uvm_access_counter_buffer_entry_t *entry,
                                                   uvm_va_space_t **out_va_space);

typedef enum
{
    UVM_GPU_BUFFER_FLUSH_MODE_CACHED_PUT,
    UVM_GPU_BUFFER_FLUSH_MODE_UPDATE_PUT,
} uvm_gpu_buffer_flush_mode_t;

#endif // __UVM_GPU_H__
