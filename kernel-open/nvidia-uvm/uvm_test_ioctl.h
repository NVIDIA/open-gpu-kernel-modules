/*******************************************************************************
    Copyright (c) 2015-2024 NVidia Corporation

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

#ifndef __UVM_TEST_IOCTL_H__
#define __UVM_TEST_IOCTL_H__


#include "uvm_types.h"
#include "uvm_ioctl.h"
#include "nv_uvm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Offset the test ioctl to leave space for the api ones
#define UVM_TEST_IOCTL_BASE(i)                          UVM_IOCTL_BASE(200 + i)

#define UVM_TEST_GET_GPU_REF_COUNT                       UVM_TEST_IOCTL_BASE(0)
typedef struct
{
    // In params
    NvProcessorUuid gpu_uuid;
    // Out params
    NvU64           ref_count NV_ALIGN_BYTES(8);
    NV_STATUS       rmStatus;
} UVM_TEST_GET_GPU_REF_COUNT_PARAMS;

#define UVM_TEST_RNG_SANITY                              UVM_TEST_IOCTL_BASE(1)
typedef struct
{
    NV_STATUS rmStatus;
} UVM_TEST_RNG_SANITY_PARAMS;

#define UVM_TEST_RANGE_TREE_DIRECTED                     UVM_TEST_IOCTL_BASE(2)
typedef struct
{
    NV_STATUS rmStatus;
} UVM_TEST_RANGE_TREE_DIRECTED_PARAMS;

#define UVM_TEST_RANGE_TREE_RANDOM                       UVM_TEST_IOCTL_BASE(3)
typedef struct
{
    NvU32     seed;                                 // In
    NvU64     main_iterations    NV_ALIGN_BYTES(8); // In
    NvU32     verbose;                              // In

    // Probability (0-100)
    //
    // When the test starts up, it adds and splits ranges with high_probability.
    // Eventually when adds and splits fail too often, they'll invert their
    // probability to 100 - high_probability. They'll switch back when the tree
    // becomes too empty.
    //
    // This can be < 50, but the test will not be very interesting.
    NvU32     high_probability;                     // In

    // Probability (0-100)
    //
    // Every main iteration a group of operations is selected with this
    // probability. The group consists of either "add/remove" or "split/merge."
    // This is the chance that the "add/remove" group is selected each
    // iteration.
    NvU32     add_remove_shrink_group_probability;

    // Probability (0-100)
    //
    // Probability of picking the shrink operation instead of add/remove if the
    // add/remove/shrink group of operations is selected.
    NvU32     shrink_probability;

    // The number of collision verification checks to make each main iteration
    NvU32     collision_checks;                     // In

    // The number of tree iterator verification checks to make each main
    // iteration.
    NvU32     iterator_checks;                      // In

    // Highest range value to use
    NvU64     max_end            NV_ALIGN_BYTES(8); // In

    // Maximum number of range nodes to put in the tree
    NvU64     max_ranges         NV_ALIGN_BYTES(8); // In

    // Maximum number of range nodes to add or remove at one time
    NvU64     max_batch_count    NV_ALIGN_BYTES(8); // In

    // add, split, and merge operations all operate on randomly-selected ranges
    // or nodes. It's possible, sometimes even likely, that the operation cannot
    // be performed on the selected range or node.
    //
    // For example, when a range node is added its range is selected at random
    // without regard to range nodes already in the tree. If a collision occurs
    // when the test attempts to add that node to the tree, a new, smaller
    // random range is selected and the attempt is made again.
    //
    // max_attempts is the maximum number of times to keep picking new ranges or
    // nodes before giving up on the operation.
    NvU32     max_attempts;                          // In

    struct
    {
        NvU64 total_adds         NV_ALIGN_BYTES(8);
        NvU64 failed_adds        NV_ALIGN_BYTES(8);
        NvU64 max_attempts_add   NV_ALIGN_BYTES(8);
        NvU64 total_removes      NV_ALIGN_BYTES(8);
        NvU64 total_splits       NV_ALIGN_BYTES(8);
        NvU64 failed_splits      NV_ALIGN_BYTES(8);
        NvU64 max_attempts_split NV_ALIGN_BYTES(8);
        NvU64 total_merges       NV_ALIGN_BYTES(8);
        NvU64 failed_merges      NV_ALIGN_BYTES(8);
        NvU64 max_attempts_merge NV_ALIGN_BYTES(8);
        NvU64 total_shrinks      NV_ALIGN_BYTES(8);
        NvU64 failed_shrinks     NV_ALIGN_BYTES(8);
    } stats;                                        // Out

    NV_STATUS rmStatus;                             // Out
} UVM_TEST_RANGE_TREE_RANDOM_PARAMS;

// Keep this in sync with uvm_va_range_type_t in uvm_va_range.h
typedef enum
{
    UVM_TEST_VA_RANGE_TYPE_INVALID = 0,
    UVM_TEST_VA_RANGE_TYPE_MANAGED,
    UVM_TEST_VA_RANGE_TYPE_EXTERNAL,
    UVM_TEST_VA_RANGE_TYPE_CHANNEL,
    UVM_TEST_VA_RANGE_TYPE_SKED_REFLECTED,
    UVM_TEST_VA_RANGE_TYPE_SEMAPHORE_POOL,
    UVM_TEST_VA_RANGE_TYPE_DEVICE_P2P,
    UVM_TEST_VA_RANGE_TYPE_MAX
} UVM_TEST_VA_RANGE_TYPE;

typedef enum
{
    UVM_TEST_RANGE_SUBTYPE_INVALID = 0,
    UVM_TEST_RANGE_SUBTYPE_UVM,
    UVM_TEST_RANGE_SUBTYPE_HMM,
    UVM_TEST_RANGE_SUBTYPE_MAX
} UVM_TEST_RANGE_SUBTYPE;

// Keep this in sync with uvm_read_duplication_t in uvm_va_range.h
typedef enum
{
    UVM_TEST_READ_DUPLICATION_UNSET = 0,
    UVM_TEST_READ_DUPLICATION_ENABLED,
    UVM_TEST_READ_DUPLICATION_DISABLED,
    UVM_TEST_READ_DUPLICATION_MAX
} UVM_TEST_READ_DUPLICATION_POLICY;

typedef enum
{
    UVM_TEST_NVLINK_ERROR_NONE = 0,
    UVM_TEST_NVLINK_ERROR_UNRESOLVED,
    UVM_TEST_NVLINK_ERROR_RESOLVED
} UVM_TEST_NVLINK_ERROR_TYPE;

typedef struct
{
    // Note: if this is a zombie or not owned by the calling process, the vma info
    // will not be filled out and is invalid.
    NvU64  vma_start NV_ALIGN_BYTES(8); // Out
    NvU64  vma_end   NV_ALIGN_BYTES(8); // Out, inclusive
    NvBool is_zombie;                   // Out
    // Note: if this is a zombie, this field is meaningless.
    NvBool owned_by_calling_process;    // Out
    NvU32  subtype;                     // Out (UVM_TEST_RANGE_SUBTYPE)
} UVM_TEST_VA_RANGE_INFO_MANAGED;

#define UVM_TEST_VA_RANGE_INFO                           UVM_TEST_IOCTL_BASE(4)
typedef struct
{
    NvU64                           lookup_address                   NV_ALIGN_BYTES(8); // In

    // For HMM ranges va_range_start/end will contain the lookup address but not
    // neccessarily the maximal range over which the returned policy applies.
    // For example there could be adjacent ranges with the same policy, implying
    // the returned range could be as small as a page in the worst case for HMM.
    NvU64                           va_range_start                   NV_ALIGN_BYTES(8); // Out
    NvU64                           va_range_end                     NV_ALIGN_BYTES(8); // Out, inclusive
    NvU32                           read_duplication;                                   // Out (UVM_TEST_READ_DUPLICATION_POLICY)
    NvProcessorUuid                 preferred_location;                                 // Out
    NvS32                           preferred_cpu_nid;                                  // Out
    NvProcessorUuid                 accessed_by[UVM_MAX_PROCESSORS];                    // Out
    NvU32                           accessed_by_count;                                  // Out
    NvU32                           type;                                               // Out (UVM_TEST_VA_RANGE_TYPE)
    union
    {
        UVM_TEST_VA_RANGE_INFO_MANAGED managed                       NV_ALIGN_BYTES(8); // Out
        // More here eventually
    };

    // NV_ERR_INVALID_ADDRESS   lookup_address doesn't match a UVM range
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_VA_RANGE_INFO_PARAMS;

#define UVM_TEST_RM_MEM_SANITY                           UVM_TEST_IOCTL_BASE(5)
typedef struct
{
    // Out params
    NV_STATUS rmStatus;
} UVM_TEST_RM_MEM_SANITY_PARAMS;

#define UVM_TEST_GPU_SEMAPHORE_SANITY                    UVM_TEST_IOCTL_BASE(6)
typedef struct
{
    // Out params
    NV_STATUS rmStatus;
} UVM_TEST_GPU_SEMAPHORE_SANITY_PARAMS;

#define UVM_TEST_PEER_REF_COUNT                          UVM_TEST_IOCTL_BASE(7)
typedef struct
{
    // In params
    NvProcessorUuid gpu_uuid_1;
    NvProcessorUuid gpu_uuid_2;

    // Out params
    NV_STATUS       rmStatus;
    NvU64           ref_count   NV_ALIGN_BYTES(8);
} UVM_TEST_PEER_REF_COUNT_PARAMS;

// Force an existing UVM range to split. split_address will be the new end of
// the existing range. A new range will be created covering
// [split_address+1, original end].
//
// Error returns:
// NV_ERR_INVALID_ADDRESS
//  - split_address+1 isn't page-aligned
//  - split_address doesn't match a splittable UVM range
//  - The range cannot be split at split_address because split_address is
//    already the end of the range.
#define UVM_TEST_VA_RANGE_SPLIT                          UVM_TEST_IOCTL_BASE(8)
typedef struct
{
    NvU64     split_address NV_ALIGN_BYTES(8); // In
    NV_STATUS rmStatus;                        // Out
} UVM_TEST_VA_RANGE_SPLIT_PARAMS;

// Forces the next range split on the range covering lookup_address to fail with
// an out-of-memory error. Only the next split will fail. Subsequent ones will
// succeed. The split can come from any source, such as vma splitting or
// UVM_TEST_VA_RANGE_SPLIT.
//
// Error returns:
// NV_ERR_INVALID_ADDRESS
//  - lookup_address doesn't match a UVM range
#define UVM_TEST_VA_RANGE_INJECT_SPLIT_ERROR             UVM_TEST_IOCTL_BASE(9)
typedef struct
{
    NvU64     lookup_address NV_ALIGN_BYTES(8); // In
    NV_STATUS rmStatus;                         // Out
} UVM_TEST_VA_RANGE_INJECT_SPLIT_ERROR_PARAMS;

#define UVM_TEST_PAGE_TREE                               UVM_TEST_IOCTL_BASE(10)
typedef struct
{
    NV_STATUS rmStatus;                     // Out
} UVM_TEST_PAGE_TREE_PARAMS;

// Given a VA and a target processor, forcibly set that processor's mapping to
// the VA to the given permissions. This may require changing other processors'
// mappings. For example, setting an atomic mapping for a given GPU might make
// other GPUs' mappings read-only.
//
// If the mapping changes from invalid to anything else, this call always
// attempts to create direct mappings from the given processor to the current
// physical memory backing the target address. If a direct mapping cannot be
// created, or no physical memory currently backs the VA,
// NV_ERR_INVALID_OPERATION is returned.
//
// uuid is allowed to be NV_PROCESSOR_UUID_CPU_DEFAULT.
//
// Error returns:
// NV_ERR_INVALID_DEVICE
//  - uuid is an unknown value
//  - uuid is a GPU that hasn't been registered with this process
//
// NV_ERR_INVALID_ADDRESS
// - VA is unknown to the kernel
// - VA isn't aligned to the system page size
//
// NV_ERR_INVALID_STATE
// - A mapping for va can't be accessed because it belongs to another process
//
// NV_ERR_INVALID_ARGUMENT
// - mapping is not a valid enum value
//
// NV_ERR_INVALID_ACCESS_TYPE
// - The mapping permissions aren't logically allowed. For example,
//   UVM_TEST_PTE_MAPPING_READ_WRITE can't be set on a read-only mapping.
//
// NV_ERR_INVALID_OPERATION
// - mapping is not UVM_TEST_PTE_MAPPING_INVALID, and a direct mapping from the
//   given processor to the physical memory currently backing VA cannot be
//   created.
#define UVM_TEST_CHANGE_PTE_MAPPING                      UVM_TEST_IOCTL_BASE(11)

typedef enum
{
    UVM_TEST_PTE_MAPPING_INVALID = 0,
    UVM_TEST_PTE_MAPPING_READ_ONLY,
    UVM_TEST_PTE_MAPPING_READ_WRITE,
    UVM_TEST_PTE_MAPPING_READ_WRITE_ATOMIC,
    UVM_TEST_PTE_MAPPING_MAX
} UVM_TEST_PTE_MAPPING;

typedef struct
{
    NvProcessorUuid      uuid      NV_ALIGN_BYTES(8); // In
    NvU64                va        NV_ALIGN_BYTES(8); // In
    NvU32                mapping;                     // In (UVM_TEST_PTE_MAPPING)
    NV_STATUS            rmStatus;                    // Out
} UVM_TEST_CHANGE_PTE_MAPPING_PARAMS;

#define UVM_TEST_TRACKER_SANITY                          UVM_TEST_IOCTL_BASE(12)
typedef struct
{
    NV_STATUS rmStatus;               // Out
} UVM_TEST_TRACKER_SANITY_PARAMS;

#define UVM_TEST_PUSH_SANITY                             UVM_TEST_IOCTL_BASE(13)
typedef struct
{
    NvBool    skipTimestampTest;      // In
    NV_STATUS rmStatus;               // Out
} UVM_TEST_PUSH_SANITY_PARAMS;

#define UVM_TEST_CHANNEL_SANITY                          UVM_TEST_IOCTL_BASE(14)
typedef struct
{
    NV_STATUS rmStatus;               // Out
} UVM_TEST_CHANNEL_SANITY_PARAMS;

typedef enum
{
    UVM_TEST_CHANNEL_STRESS_MODE_NOOP_PUSH = 0,
    UVM_TEST_CHANNEL_STRESS_MODE_UPDATE_CHANNELS,
    UVM_TEST_CHANNEL_STRESS_MODE_STREAM,
    UVM_TEST_CHANNEL_STRESS_MODE_KEY_ROTATION,
} UVM_TEST_CHANNEL_STRESS_MODE;

typedef enum
{
    UVM_TEST_CHANNEL_STRESS_KEY_ROTATION_OPERATION_CPU_TO_GPU,
    UVM_TEST_CHANNEL_STRESS_KEY_ROTATION_OPERATION_GPU_TO_CPU,
    UVM_TEST_CHANNEL_STRESS_KEY_ROTATION_OPERATION_ROTATE,
} UVM_TEST_CHANNEL_STRESS_KEY_ROTATION_OPERATION;

#define UVM_TEST_CHANNEL_STRESS                          UVM_TEST_IOCTL_BASE(15)
typedef struct
{
    NvU32     mode;                   // In, one of UVM_TEST_CHANNEL_STRESS_MODE

    // Number of iterations:
    //   mode == NOOP_PUSH: number of noop pushes
    //   mode == UPDATE_CHANNELS: number of updates
    //   mode == STREAM: number of iterations per stream
    //   mode == ROTATION: number of operations
    NvU32     iterations;

    NvU32     num_streams;            // In, used only if mode == STREAM
    NvU32     key_rotation_operation; // In, used only if mode == ROTATION
    NvU32     seed;                   // In
    NvU32     verbose;                // In
    NV_STATUS rmStatus;               // Out
} UVM_TEST_CHANNEL_STRESS_PARAMS;

#define UVM_TEST_CE_SANITY                               UVM_TEST_IOCTL_BASE(16)
typedef struct
{
    NvBool    skipTimestampTest;      // In
    NV_STATUS rmStatus;               // Out
} UVM_TEST_CE_SANITY_PARAMS;

#define UVM_TEST_VA_BLOCK_INFO                           UVM_TEST_IOCTL_BASE(17)

// See UVM_VA_BLOCK_SIZE in uvm_va_block.h for an explanation of this number
#define UVM_TEST_VA_BLOCK_SIZE (2ull*1024*1024)

typedef struct
{
    NvU64     lookup_address    NV_ALIGN_BYTES(8); // In


    NvU64     va_block_start    NV_ALIGN_BYTES(8); // Out
    NvU64     va_block_end      NV_ALIGN_BYTES(8); // Out, inclusive

    // NV_ERR_INVALID_ADDRESS   lookup_address doesn't match a UVM range
    //
    // NV_ERR_OBJECT_NOT_FOUND  lookup_address matched a UVM range on this file
    //                          but the corresponding block has not yet been
    //                          populated
    NV_STATUS rmStatus;                            // Out
} UVM_TEST_VA_BLOCK_INFO_PARAMS;

#define UVM_TEST_LOCK_SANITY                             UVM_TEST_IOCTL_BASE(18)
typedef struct
{
    NV_STATUS rmStatus; // Out
} UVM_TEST_LOCK_SANITY_PARAMS;

#define UVM_TEST_PERF_UTILS_SANITY                       UVM_TEST_IOCTL_BASE(19)
typedef struct
{
    NV_STATUS rmStatus; // Out
} UVM_TEST_PERF_UTILS_SANITY_PARAMS;

#define UVM_TEST_KVMALLOC                                UVM_TEST_IOCTL_BASE(20)
typedef struct
{
    NV_STATUS rmStatus; // Out
} UVM_TEST_KVMALLOC_PARAMS;

#define UVM_TEST_PMM_QUERY                               UVM_TEST_IOCTL_BASE(21)
typedef enum
{
    // Get the value of valid user allocations as key
    UVM_TEST_CHUNK_SIZE_GET_USER_SIZE
} uvm_test_pmm_query_key_t;

typedef struct
{
    // In params
    NvProcessorUuid gpu_uuid;
    NvU64 key;
    // Out params
    NvU64 value;
    NV_STATUS rmStatus;
} UVM_TEST_PMM_QUERY_PARAMS;

#define UVM_TEST_PMM_CHECK_LEAK                          UVM_TEST_IOCTL_BASE(22)

typedef struct
{
    NvProcessorUuid gpu_uuid; // In
    NvU64 chunk_size;         // In
    NvS64 alloc_limit;        // In. Number of chunks to allocate. -1 means unlimited
    NvU64 allocated;          // Out. Number of chunks actually allocated
    NV_STATUS rmStatus;       // Out
} UVM_TEST_PMM_CHECK_LEAK_PARAMS;

#define UVM_TEST_PERF_EVENTS_SANITY                      UVM_TEST_IOCTL_BASE(23)
typedef struct
{
    // Out params
    NV_STATUS rmStatus;
} UVM_TEST_PERF_EVENTS_SANITY_PARAMS;

#define UVM_TEST_PERF_MODULE_SANITY                      UVM_TEST_IOCTL_BASE(24)
typedef struct
{
    // In params
    NvU64 range_address              NV_ALIGN_BYTES(8);
    NvU32 range_size;
    // Out params
    NV_STATUS rmStatus;
} UVM_TEST_PERF_MODULE_SANITY_PARAMS;

#define UVM_TEST_RANGE_ALLOCATOR_SANITY                  UVM_TEST_IOCTL_BASE(25)
typedef struct
{
    // In params
    NvU32 verbose;
    NvU32 seed;
    NvU32 iters;

    // Out params
    NV_STATUS rmStatus;
} UVM_TEST_RANGE_ALLOCATOR_SANITY_PARAMS;

#define UVM_TEST_GET_RM_PTES                             UVM_TEST_IOCTL_BASE(26)
typedef enum
{
    UVM_TEST_GET_RM_PTES_SINGLE_GPU = 0,
    UVM_TEST_GET_RM_PTES_MULTI_GPU_SUPPORTED,
    UVM_TEST_GET_RM_PTES_MULTI_GPU_SLI_SUPPORTED,
    UVM_TEST_GET_RM_PTES_MULTI_GPU_NOT_SUPPORTED,
    UVM_TEST_GET_RM_PTES_MAX
} UVM_TEST_PTE_RM_PTES_TEST_MODE;

typedef struct
{
    // In
    NvS32 rmCtrlFd;             // For future use. (security check)
    NvHandle hClient;
    NvHandle hMemory;
    NvU32 test_mode;            // (UVM_TEST_PTE_RM_PTES_TEST_MODE)
    NvU64 size                  NV_ALIGN_BYTES(8);
    NvProcessorUuid gpu_uuid;

    // Out
    NV_STATUS rmStatus;
} UVM_TEST_GET_RM_PTES_PARAMS;

#define UVM_TEST_FAULT_BUFFER_FLUSH                      UVM_TEST_IOCTL_BASE(27)
typedef struct
{
    NvU64 iterations;           // In
    NV_STATUS rmStatus;         // Out
} UVM_TEST_FAULT_BUFFER_FLUSH_PARAMS;

#define UVM_TEST_INJECT_TOOLS_EVENT                      UVM_TEST_IOCTL_BASE(28)
typedef struct
{
    // In params
    UvmEventEntry    entry;    // contains only NvUxx types
    NvU32 count;

    // Out param
    NV_STATUS rmStatus;
} UVM_TEST_INJECT_TOOLS_EVENT_PARAMS;

#define UVM_TEST_INCREMENT_TOOLS_COUNTER                 UVM_TEST_IOCTL_BASE(29)
typedef struct
{
    // In params
    NvU64 amount                     NV_ALIGN_BYTES(8); // amount to increment
    NvU32 counter;                                      // name of counter
    NvProcessorUuid processor;
    NvU32 count;                                        // number of times to increment

    // Out param
    NV_STATUS rmStatus;
} UVM_TEST_INCREMENT_TOOLS_COUNTER_PARAMS;

#define UVM_TEST_MEM_SANITY                              UVM_TEST_IOCTL_BASE(30)
typedef struct
{
    // Out params
    NV_STATUS rmStatus;
} UVM_TEST_MEM_SANITY_PARAMS;

#define UVM_TEST_MAKE_CHANNEL_STOPS_IMMEDIATE            UVM_TEST_IOCTL_BASE(32)
typedef struct
{
    // Out params
    NV_STATUS rmStatus;
} UVM_TEST_MAKE_CHANNEL_STOPS_IMMEDIATE_PARAMS;

// Inject an error into the VA block covering the lookup_address
//
// If page_table_allocation_retry_force_count is non-0 then the next count
// page table allocations under the VA block will be forced to do
// allocation-retry.
//
// If user_pages_allocation_retry_force_count is non-0 then the next count user
// memory allocations under the VA block will be forced to do allocation-retry.
//
// If cpu_chunk_allocation_error_count is not zero, the subsequent operations
// that need to allocate CPU chunks will fail with NV_ERR_NO_MEMORY for
// cpu_chunk_allocation_error_count times. If cpu_chunk_allocation_error_count
// is equal to ~0U, the count is infinite.
//
// If eviction_failure is NV_TRUE, the next eviction attempt from the VA block
// will fail with NV_ERR_NO_MEMORY.
//
// If populate_failure is NV_TRUE, a retry error will be injected after the next
// successful user memory allocation under the VA block but before that
// allocation is used by the block. This is similar to
// user_pages_allocation_retry_force_count, but the injection point simulates
// driver metadata allocation failure.
//
// cpu_chunk_allocation_target_id and cpu_chunk_allocation_actual_id are used
// to control the NUMA node IDs for CPU chunk allocations, specifically for
// testing overlapping CPU chunk allocations.
//
// Currently, uvm_api_migrate() does not pass the preferred CPU NUMA node to for
// managed memory so it is not possible to request a specific node.
// cpu_chunk_allocation_target_id is used to request the allocation be made on
// specific node. On the other hand, cpu_chunk_allocation_actual_id is the node
// on which the allocation will actually be made.
//
// The two parameters can be used to force a CPU chunk allocation to overlap a
// previously allocated chunk.
//
// Please note that even when specifying cpu_cpu_allocation_actual_id, the
// kernel may end up allocating on a different node.
//
// Error returns:
// NV_ERR_INVALID_ADDRESS
//  - lookup_address doesn't match a UVM range
#define UVM_TEST_VA_BLOCK_INJECT_ERROR                   UVM_TEST_IOCTL_BASE(33)
typedef struct
{
    NvU64     lookup_address NV_ALIGN_BYTES(8);         // In
    NvU32     page_table_allocation_retry_force_count;  // In
    NvU32     user_pages_allocation_retry_force_count;  // In
    NvU64     cpu_chunk_allocation_size_mask;           // In
    NvS32     cpu_chunk_allocation_target_id;           // In
    NvS32     cpu_chunk_allocation_actual_id;           // In
    NvU32     cpu_chunk_allocation_error_count;         // In
    NvBool    eviction_error;                           // In
    NvBool    populate_error;                           // In
    NV_STATUS rmStatus;                                 // Out
} UVM_TEST_VA_BLOCK_INJECT_ERROR_PARAMS;

#define UVM_TEST_PEER_IDENTITY_MAPPINGS                  UVM_TEST_IOCTL_BASE(34)
typedef struct
{
    // In params
    NvProcessorUuid gpuA;
    NvProcessorUuid gpuB;
    // Out param
    NV_STATUS rmStatus;
} UVM_TEST_PEER_IDENTITY_MAPPINGS_PARAMS;

#define UVM_TEST_VA_RESIDENCY_INFO                       UVM_TEST_IOCTL_BASE(35)
typedef struct
{
    NvU64                           lookup_address                   NV_ALIGN_BYTES(8); // In

    // Whether to wait on the block tracker before returning. Fields like
    // resident_on and mapped_on represent state which will be valid when the
    // block tracker is complete. If is_async is true, then those fields will
    // still be filled out as if the tracker is done, but the actual residency
    // or mapping changes may not have been performed yet.
    NvBool                          is_async;                                           // In

    // Array of processors which have a resident copy of the page containing
    // lookup_address.
    NvProcessorUuid                 resident_on[UVM_MAX_PROCESSORS];                    // Out
    NvU32                           resident_on_count;                                  // Out

    // If the memory is resident on the CPU, the NUMA node on which the page
    // is resident. Otherwise, -1.
    NvS32                           resident_nid;                                       // Out

    // The size of the physical allocation backing lookup_address. Only the
    // system-page-sized portion of this allocation which contains
    // lookup_address is guaranteed to be resident on the corresponding
    // processor.
    NvU32                           resident_physical_size[UVM_MAX_PROCESSORS];         // Out

    // The physical address of the physical allocation backing lookup_address.
    NvU64                           resident_physical_address[UVM_MAX_PROCESSORS] NV_ALIGN_BYTES(8); // Out

    // Array of processors which have a virtual mapping covering lookup_address.
    NvProcessorUuid                 mapped_on[UVM_MAX_PROCESSORS];                      // Out
    NvU32                           mapping_type[UVM_MAX_PROCESSORS];                   // Out
    NvU64                           mapping_physical_address[UVM_MAX_PROCESSORS] NV_ALIGN_BYTES(8); // Out
    NvBool                          is_egm_mapping[UVM_MAX_PROCESSORS];                 // Out
    NvU32                           mapped_on_count;                                    // Out

    // The size of the virtual mapping covering lookup_address on each
    // mapped_on processor.
    NvU64                           page_size[UVM_MAX_PROCESSORS];                      // Out

    // Array of processors which have physical memory populated that would back
    // lookup_address if it was resident.
    NvProcessorUuid                 populated_on[UVM_MAX_PROCESSORS];                   // Out
    NvU32                           populated_on_count;                                 // Out

    NV_STATUS rmStatus;                                                                 // Out
} UVM_TEST_VA_RESIDENCY_INFO_PARAMS;

#define UVM_TEST_PMM_ASYNC_ALLOC                         UVM_TEST_IOCTL_BASE(36)
typedef struct
{
    NvProcessorUuid gpu_uuid;                           // In
    NvU32 num_chunks;                                   // In
    NvU32 num_work_iterations;                          // In
    NV_STATUS rmStatus;                                 // Out
} UVM_TEST_PMM_ASYNC_ALLOC_PARAMS;

typedef enum
{
    UVM_TEST_PREFETCH_FILTERING_MODE_FILTER_ALL,  // Disable all prefetch faults
    UVM_TEST_PREFETCH_FILTERING_MODE_FILTER_NONE, // Enable all prefetch faults
} UvmTestPrefetchFilteringMode;

#define UVM_TEST_SET_PREFETCH_FILTERING                  UVM_TEST_IOCTL_BASE(37)
typedef struct
{
    NvProcessorUuid gpu_uuid;                           // In
    NvU32           filtering_mode;                     // In (UvmTestPrefetchFilteringMode)
    NV_STATUS       rmStatus;                           // Out
} UVM_TEST_SET_PREFETCH_FILTERING_PARAMS;

typedef enum
{
    UvmTestPmmSanityModeFull  = 1,
    UvmTestPmmSanityModeBasic = 2,
} UvmTestPmmSanityMode;

#define UVM_TEST_PMM_SANITY                              UVM_TEST_IOCTL_BASE(40)
typedef struct
{
    // Test mode of type UvmTestPmmSanityMode
    NvU32         mode; // In
    NV_STATUS rmStatus; // Out
} UVM_TEST_PMM_SANITY_PARAMS;

typedef enum
{
    UvmInvalidateTlbMemBarNone  = 1,
    UvmInvalidateTlbMemBarSys   = 2,
    UvmInvalidateTlbMemBarLocal = 3,
} UvmInvalidateTlbMembarType;

typedef enum
{
    UvmInvalidatePageTableLevelAll = 1,
    UvmInvalidatePageTableLevelPte = 2,
    UvmInvalidatePageTableLevelPde0 = 3,
    UvmInvalidatePageTableLevelPde1 = 4,
    UvmInvalidatePageTableLevelPde2 = 5,
    UvmInvalidatePageTableLevelPde3 = 6,
    UvmInvalidatePageTableLevelPde4 = 7,
} UvmInvalidatePageTableLevel;

typedef enum
{
    UvmTargetVaModeAll      = 1,
    UvmTargetVaModeTargeted = 2,
} UvmTargetVaMode;

#define UVM_TEST_INVALIDATE_TLB                          UVM_TEST_IOCTL_BASE(41)
typedef struct
{
    // In params
    NvProcessorUuid  gpu_uuid;
    NvU64            va NV_ALIGN_BYTES(8);
    NvU32            target_va_mode;           // UvmTargetVaMode
    NvU32            page_table_level;         // UvmInvalidatePageTableLevel
    NvU32            membar;                   // UvmInvalidateTlbMembarType
    NvBool           disable_gpc_invalidate;

    // Out params
    NV_STATUS        rmStatus;
} UVM_TEST_INVALIDATE_TLB_PARAMS;

#define UVM_TEST_VA_BLOCK                                UVM_TEST_IOCTL_BASE(42)
typedef struct
{
    NV_STATUS rmStatus; // Out
} UVM_TEST_VA_BLOCK_PARAMS;

typedef enum
{
    // Default policy based eviction
    //
    // Evicts a chunk that the default eviction path would pick.
    UvmTestEvictModeDefault = 1,

    // Virtual address based eviction
    //
    // Evicts the root chunk that the chunk backing the provided virtual address
    // belongs to.
    UvmTestEvictModeVirtual,

    // Physical address based eviction
    //
    // Evicts the root chunk covering the provided physical address.
    UvmTestEvictModePhysical,
} UvmTestEvictMode;

// Evict a chunk chosen according to one the test eviction modes specified
// above. Eviction may not always be possible, but as long as the arguments are
// valid NV_OK will be returned. To check whether eviction happened, the
// chunk_was_evicted flag needs to be inspected.
#define UVM_TEST_EVICT_CHUNK                             UVM_TEST_IOCTL_BASE(43)
typedef struct
{
    // The GPU to evict from, has to be registered in the VA space.
    NvProcessorUuid                 gpu_uuid;                                           // In

    // UvmTestEvictMode
    NvU32                           eviction_mode;                                      // In

    // Virtual or physical address if evictionMode is UvmTestEvictModeVirtual or
    // UvmTestEvictModePhysical.
    NvU64                           address                          NV_ALIGN_BYTES(8); // In

    // Flag indicating whether the eviction was performed.
    NvBool                          chunk_was_evicted;                                  // Out

    // Physical address of the evicted root chunk. Notably 0 is a valid physical address.
    NvU64                           evicted_physical_address         NV_ALIGN_BYTES(8); // Out

    // For the virtual eviction mode, returns the size of the chunk that was
    // backing the virtual address before being evicted. 0 otherwise.
    NvU64                           chunk_size_backing_virtual       NV_ALIGN_BYTES(8); // Out

    NV_STATUS rmStatus;                                                                 // Out
} UVM_TEST_EVICT_CHUNK_PARAMS;

typedef enum
{
    // Flush deferred accessed by mappings
    UvmTestDeferredWorkTypeAcessedByMappings = 1,
} UvmTestDeferredWorkType;

#define UVM_TEST_FLUSH_DEFERRED_WORK                     UVM_TEST_IOCTL_BASE(44)
typedef struct
{
    // UvmTestDeferredWorkType
    NvU32                           work_type;                                          // In

    NV_STATUS rmStatus;                                                                 // Out
} UVM_TEST_FLUSH_DEFERRED_WORK_PARAMS;

#define UVM_TEST_NV_KTHREAD_Q                            UVM_TEST_IOCTL_BASE(45)
typedef struct
{
    NV_STATUS rmStatus; // Out
} UVM_TEST_NV_KTHREAD_Q_PARAMS;

typedef enum
{
    UVM_TEST_PAGE_PREFETCH_POLICY_ENABLE = 0,
    UVM_TEST_PAGE_PREFETCH_POLICY_DISABLE,
    UVM_TEST_PAGE_PREFETCH_POLICY_MAX
} UVM_TEST_PAGE_PREFETCH_POLICY;

#define UVM_TEST_SET_PAGE_PREFETCH_POLICY                UVM_TEST_IOCTL_BASE(46)
typedef struct
{
    NvU32       policy; // In (UVM_TEST_PAGE_PREFETCH_POLICY)
    NV_STATUS rmStatus; // Out
} UVM_TEST_SET_PAGE_PREFETCH_POLICY_PARAMS;

#define UVM_TEST_RANGE_GROUP_TREE                        UVM_TEST_IOCTL_BASE(47)
typedef struct
{
    NvU64 rangeGroupIds[4]                                           NV_ALIGN_BYTES(8); // In
    NV_STATUS rmStatus;                                                                 // Out
} UVM_TEST_RANGE_GROUP_TREE_PARAMS;

#define UVM_TEST_RANGE_GROUP_RANGE_INFO                  UVM_TEST_IOCTL_BASE(48)
typedef struct
{
    NvU64                           lookup_address                   NV_ALIGN_BYTES(8); // In

    NvU64                           range_group_range_start          NV_ALIGN_BYTES(8); // Out
    NvU64                           range_group_range_end            NV_ALIGN_BYTES(8); // Out, inclusive
    NvU64                           range_group_id                   NV_ALIGN_BYTES(8); // Out
    NvU32                           range_group_present;                                // Out
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_RANGE_GROUP_RANGE_INFO_PARAMS;

#define UVM_TEST_RANGE_GROUP_RANGE_COUNT                 UVM_TEST_IOCTL_BASE(49)
typedef struct
{
    NvU64                           rangeGroupId                     NV_ALIGN_BYTES(8); // In
    NvU64                           count                            NV_ALIGN_BYTES(8); // Out
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_RANGE_GROUP_RANGE_COUNT_PARAMS;

#define UVM_TEST_GET_PREFETCH_FAULTS_REENABLE_LAPSE      UVM_TEST_IOCTL_BASE(50)
typedef struct
{
    NvU32       reenable_lapse; // Out: Lapse in miliseconds
    NV_STATUS         rmStatus; // Out
} UVM_TEST_GET_PREFETCH_FAULTS_REENABLE_LAPSE_PARAMS;

#define UVM_TEST_SET_PREFETCH_FAULTS_REENABLE_LAPSE      UVM_TEST_IOCTL_BASE(51)
typedef struct
{
    NvU32       reenable_lapse; // In: Lapse in miliseconds
    NV_STATUS         rmStatus; // Out
} UVM_TEST_SET_PREFETCH_FAULTS_REENABLE_LAPSE_PARAMS;

#define UVM_TEST_GET_KERNEL_VIRTUAL_ADDRESS              UVM_TEST_IOCTL_BASE(52)
typedef struct
{
    NvU64                           addr                            NV_ALIGN_BYTES(8); // Out
    NV_STATUS                       rmStatus;                                          // Out
} UVM_TEST_GET_KERNEL_VIRTUAL_ADDRESS_PARAMS;

// Allocate and free memory directly from PMA with eviction enabled. This allows
// to simulate RM-like allocations, but without the RM API lock serializing
// everything.
#define UVM_TEST_PMA_ALLOC_FREE                          UVM_TEST_IOCTL_BASE(53)
typedef struct
{
    NvProcessorUuid                 gpu_uuid;                                           // In
    NvU64                           page_size;
    NvBool                          contiguous;
    NvU64                           num_pages                        NV_ALIGN_BYTES(8); // In
    NvU64                           phys_begin                       NV_ALIGN_BYTES(8); // In
    NvU64                           phys_end                         NV_ALIGN_BYTES(8); // In
    NvU32                           nap_us_before_free;                                 // In
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_PMA_ALLOC_FREE_PARAMS;

// Allocate and free user memory directly from PMM with eviction enabled.
//
// Provides a direct way of exercising PMM allocs, eviction and frees of user
// memory type.
#define UVM_TEST_PMM_ALLOC_FREE_ROOT                     UVM_TEST_IOCTL_BASE(54)
typedef struct
{
    NvProcessorUuid                 gpu_uuid;                                           // In
    NvU32                           nap_us_before_free;                                 // In
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_PMM_ALLOC_FREE_ROOT_PARAMS;

// Inject a PMA eviction error after the specified number of chunks are
// evicted.
#define UVM_TEST_PMM_INJECT_PMA_EVICT_ERROR              UVM_TEST_IOCTL_BASE(55)
typedef struct
{
    NvProcessorUuid                 gpu_uuid;                                           // In
    NvU32                           error_after_num_chunks;                             // In
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_PMM_INJECT_PMA_EVICT_ERROR_PARAMS;

// Change configuration of access counters. This call will disable access
// counters and reenable them using the new configuration. All previous
// notifications will be lost.
//
// The reconfiguration affects all VA spaces that rely on the access
// counters information for the same GPU. To avoid conflicting configurations,
// only one VA space is allowed to reconfigure the GPU at a time.
//
// When the reconfiguration VA space is destroyed, the bottom-half control
// settings are reset.
//
// Error returns:
// NV_ERR_INVALID_STATE
//  - The GPU has already been reconfigured in a different VA space.
#define UVM_TEST_RECONFIGURE_ACCESS_COUNTERS             UVM_TEST_IOCTL_BASE(56)
typedef struct
{
    NvProcessorUuid                 gpu_uuid;                                           // In

    // Type UVM_ACCESS_COUNTER_GRANULARITY from nv_uvm_types.h
    NvU32                           granularity;                                        // In

    NvU32                           threshold;                                          // In
    NvBool                          enable_migrations;                                  // In

    // Settings to control how notifications are serviced by the access counters
    // bottom-half. These settings help tests to exercise races in the driver,
    // e.g., unregister a GPU while (valid) pending notifications remain in the
    // notification buffer.
    //
    // 0 max_batch_size doesn't change driver's behavior.
    NvU32                           max_batch_size;                                     // In
    NvBool                          one_iteration_per_batch;                            // In
    NvU32                           sleep_per_iteration_us;                             // In

    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_RECONFIGURE_ACCESS_COUNTERS_PARAMS;

typedef enum
{
    UVM_TEST_ACCESS_COUNTER_RESET_MODE_ALL = 0,
    UVM_TEST_ACCESS_COUNTER_RESET_MODE_TARGETED,
    UVM_TEST_ACCESS_COUNTER_RESET_MODE_MAX
} UVM_TEST_ACCESS_COUNTER_RESET_MODE;

// Clear the contents of the access counters. This call supports different
// modes for targeted/global resets.
#define UVM_TEST_RESET_ACCESS_COUNTERS                   UVM_TEST_IOCTL_BASE(57)
typedef struct
{
    NvProcessorUuid                 gpu_uuid;                                           // In

    // Type UVM_TEST_ACCESS_COUNTER_RESET_MODE
    NvU32                           mode;                                               // In

    NvU32                           bank;                                               // In
    NvU32                           tag;                                                // In
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_RESET_ACCESS_COUNTERS_PARAMS;

// Do not handle access counter notifications when they arrive. This call is
// used to force an overflow of the access counter notification buffer
#define UVM_TEST_SET_IGNORE_ACCESS_COUNTERS              UVM_TEST_IOCTL_BASE(58)
typedef struct
{
    NvProcessorUuid                 gpu_uuid;                                           // In
    NvBool                          ignore;                                             // In
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_SET_IGNORE_ACCESS_COUNTERS_PARAMS;

// Verifies that the given channel is registered under the UVM VA space of
// vaSpaceFd. Returns NV_OK if so, NV_ERR_INVALID_CHANNEL if not.
#define UVM_TEST_CHECK_CHANNEL_VA_SPACE                  UVM_TEST_IOCTL_BASE(59)
typedef struct
{
    NvProcessorUuid                 gpu_uuid;                                           // In
    NvS32                           rm_ctrl_fd;                                         // In
    NvHandle                        client;                                             // In
    NvHandle                        channel;                                            // In
    NvU32                           ve_id;                                              // In
    NvS32                           va_space_fd;                                        // In
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_CHECK_CHANNEL_VA_SPACE_PARAMS;

//
// UvmTestEnableNvlinkPeerAccess
//
#define UVM_TEST_ENABLE_NVLINK_PEER_ACCESS               UVM_TEST_IOCTL_BASE(60)
typedef struct
{
    NvProcessorUuid gpuUuidA; // IN
    NvProcessorUuid gpuUuidB; // IN
    NV_STATUS  rmStatus; // OUT
} UVM_TEST_ENABLE_NVLINK_PEER_ACCESS_PARAMS;

//
// UvmTestDisableNvlinkPeerAccess
//
#define UVM_TEST_DISABLE_NVLINK_PEER_ACCESS              UVM_TEST_IOCTL_BASE(61)
typedef struct
{
    NvProcessorUuid gpuUuidA; // IN
    NvProcessorUuid gpuUuidB; // IN
    NV_STATUS  rmStatus; // OUT
} UVM_TEST_DISABLE_NVLINK_PEER_ACCESS_PARAMS;

typedef enum
{
    UVM_TEST_PAGE_THRASHING_POLICY_ENABLE = 0,
    UVM_TEST_PAGE_THRASHING_POLICY_DISABLE,
    UVM_TEST_PAGE_THRASHING_POLICY_MAX
} UVM_TEST_PAGE_THRASHING_POLICY;

// This ioctl returns the thrashing mitigation parameters on the current VA
// space. Note that these values may change after a simulated/emulated GPU is
// registered on the VA space.
#define UVM_TEST_GET_PAGE_THRASHING_POLICY               UVM_TEST_IOCTL_BASE(62)
typedef struct
{
    NvU32                           policy;                                             // Out (UVM_TEST_PAGE_THRASHING_POLICY)
    NvU64                           nap_ns                           NV_ALIGN_BYTES(8); // Out
    NvU64                           pin_ns                           NV_ALIGN_BYTES(8); // Out
    NvBool                          map_remote_on_native_atomics_fault;                 // Out
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_GET_PAGE_THRASHING_POLICY_PARAMS;

#define UVM_TEST_SET_PAGE_THRASHING_POLICY               UVM_TEST_IOCTL_BASE(63)
typedef struct
{
    NvU32                           policy;                                             // In (UVM_TEST_PAGE_THRASHING_POLICY)
    NvU64                           pin_ns                           NV_ALIGN_BYTES(8); // In
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_SET_PAGE_THRASHING_POLICY_PARAMS;

#define UVM_TEST_PMM_SYSMEM                              UVM_TEST_IOCTL_BASE(64)
typedef struct
{
    NvU64                           range_address1                   NV_ALIGN_BYTES(8); // In
    NvU64                           range_address2                   NV_ALIGN_BYTES(8); // In
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_PMM_SYSMEM_PARAMS;

#define UVM_TEST_PMM_REVERSE_MAP                         UVM_TEST_IOCTL_BASE(65)
typedef struct
{
    NvProcessorUuid                 gpu_uuid;                                           // In
    NvU64                           range_address1                   NV_ALIGN_BYTES(8); // In
    NvU64                           range_address2                   NV_ALIGN_BYTES(8); // In
    NvU64                           range_size2                      NV_ALIGN_BYTES(8); // In
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_PMM_REVERSE_MAP_PARAMS;

// Calls uvm_va_space_mm_retain on a VA space, operates on the mm, optionally
// sleeps for a while, then releases the va_space_mm and returns. The idea is to
// simulate retaining a va_space_mm from a thread like the GPU fault handler
// which operates outside of the normal context of the VA space.
#define UVM_TEST_VA_SPACE_MM_RETAIN                      UVM_TEST_IOCTL_BASE(67)
typedef struct
{
    // The kernel virtual address of the uvm_va_space on which to attempt
    // retain. This can be obtained via UVM_TEST_GET_KERNEL_VIRTUAL_ADDRESS.
    //
    // The reason to use this instead of looking it up from an fd as normal is
    // to allow testing of calling threads which race with UVM VA space destroy
    // (file close). We wouldn't be able to test that path if this was an fd.
    NvU64 va_space_ptr                                               NV_ALIGN_BYTES(8); // In

    // User virtual address within the va_space_mm. If the va_space_mm is
    // successfully retained, this address is read once before sleeping and once
    // after (if sleep_us > 0).
    NvU64 addr                                                       NV_ALIGN_BYTES(8); // In

    // On success, this contains the value of addr read prior to the sleep.
    NvU64 val_before                                                 NV_ALIGN_BYTES(8); // In

    // On success, and if sleep_us > 0, this contains the value of addr read
    // after the sleep. This is invalid if sleep_us == 0.
    NvU64 val_after                                                  NV_ALIGN_BYTES(8); // In

    // Approximate duration for which to sleep with the va_space_mm retained.
    NvU64 sleep_us                                                   NV_ALIGN_BYTES(8); // In

    // NV_ERR_MISSING_TABLE_ENTRY   va_space_ptr is not a valid VA space
    // NV_ERR_PAGE_TABLE_NOT_AVAIL  Could not retain va_space_mm
    //                              (uvm_va_space_mm_retain returned NULL)
    // NV_ERR_INVALID_ADDRESS       addr is invalid in va_space_mm
    NV_STATUS rmStatus;                                                                 // Out
} UVM_TEST_VA_SPACE_MM_RETAIN_PARAMS;

#define UVM_TEST_PMM_CHUNK_WITH_ELEVATED_PAGE            UVM_TEST_IOCTL_BASE(69)
typedef struct
{
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_PMM_CHUNK_WITH_ELEVATED_PAGE_PARAMS;

#define UVM_TEST_GET_GPU_TIME                            UVM_TEST_IOCTL_BASE(70)
typedef struct
{
    // GPU to query time from. GPU must have been previously registered
    NvProcessorUuid                 gpu_uuid;                                           // In

    NvU64                           timestamp_ns                     NV_ALIGN_BYTES(8); // Out
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_GET_GPU_TIME_PARAMS;

// Check if access counters are enabled upon registration of the given GPU
#define UVM_TEST_ACCESS_COUNTERS_ENABLED_BY_DEFAULT      UVM_TEST_IOCTL_BASE(71)
typedef struct
{
    NvProcessorUuid                 gpu_uuid;                                           // In
    NvBool                          enabled;                                            // Out

    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_ACCESS_COUNTERS_ENABLED_BY_DEFAULT_PARAMS;

// Inject an error into the VA space or into a to-be registered GPU.
//
// If migrate_vma_allocation_fail_nth is greater than 0, the nth page
// allocation within migrate_vma will fail.
//
// If va_block_allocation_fail_nth is greater than 0, the nth call to
// uvm_va_block_find_create() will fail with NV_ERR_NO_MEMORY.
//
// If gpu_access_counters_alloc_buffer is set, the parent_gpu's access counters
// buffer allocation will fail with NV_ERR_NO_MEMORY.
//
// If gpu_access_counters_alloc_block_context is set, the access counters
// buffer's block_context allocation will fail with NV_ERR_NO_MEMORY.
//
// If gpu_isr_access_counters_alloc is set, the ISR access counters allocation
// will fail with NV_ERR_NO_MEMORY.
//
// If gpu_isr_access_counters_alloc_stats_cpu is set, the ISR access counters
// buffer's stats_cpu allocation will fail with NV_ERR_NO_MEMORY.
//
// If access_counters_batch_context_notifications is set, the access counters
// batch_context's notifications allocation will fail with NV_ERR_NO_MEMORY.
//
// If access_counters_batch_context_notification_cache is set, the access
// counters batch_context's notification cache allocation will fail with
// NV_ERR_NO_MEMORY.
//
// Note that only one of the gpu_* or access_counters_* setting can be selected
// at a time.
#define UVM_TEST_VA_SPACE_INJECT_ERROR                   UVM_TEST_IOCTL_BASE(72)
typedef struct
{
    NvU32                           migrate_vma_allocation_fail_nth;                    // In
    NvU32                           va_block_allocation_fail_nth;                       // In
    NvBool                          gpu_access_counters_alloc_buffer;                   // In
    NvBool                          gpu_access_counters_alloc_block_context;            // In
    NvBool                          gpu_isr_access_counters_alloc;                      // In
    NvBool                          gpu_isr_access_counters_alloc_stats_cpu;            // In
    NvBool                          access_counters_batch_context_notifications;        // In
    NvBool                          access_counters_batch_context_notification_cache;   // In

    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_VA_SPACE_INJECT_ERROR_PARAMS;

// Release to PMA all free root chunks
#define UVM_TEST_PMM_RELEASE_FREE_ROOT_CHUNKS            UVM_TEST_IOCTL_BASE(73)
typedef struct
{
    NvProcessorUuid                 gpu_uuid;                                           // In

    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_PMM_RELEASE_FREE_ROOT_CHUNKS_PARAMS;

// Wait until all pending replayable faults have been processed. If there are
// still pending packets when timeout_ns is reached, the ioctl returns
// NV_ERR_TIMEOUT.
//
// This function should be called after the kernel producing the faults has been
// synchronized. This should ensure that PUT != GET and faults will not be
// missed even if the driver has not started to process them, yet.
#define UVM_TEST_DRAIN_REPLAYABLE_FAULTS                 UVM_TEST_IOCTL_BASE(74)
typedef struct
{
    NvProcessorUuid                 gpu_uuid;                                           // In
    NvU64                           timeout_ns;                                         // In

    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_DRAIN_REPLAYABLE_FAULTS_PARAMS;

// Get module config PMA batch size in bytes
#define UVM_TEST_PMA_GET_BATCH_SIZE                      UVM_TEST_IOCTL_BASE(75)
typedef struct
{
    NvProcessorUuid                 gpu_uuid;                                           // In
    NvU64                           pma_batch_size;     NV_ALIGN_BYTES(8)               // Out

    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_PMA_GET_BATCH_SIZE_PARAMS;

// Request PMA's global statistics
#define UVM_TEST_PMM_QUERY_PMA_STATS                     UVM_TEST_IOCTL_BASE(76)
typedef struct
{
    NvProcessorUuid                 gpu_uuid;                                           // In
    UvmPmaStatistics                pma_stats;                                          // Out

    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_PMM_QUERY_PMA_STATS_PARAMS;

// Test whether the bottom halves have run on the correct CPUs based on the
// NUMA node locality of the GPU.
//
// Failure is reported if:
//   1. The GPU has serviced faults but the mask tracking which CPUs the
//      bottom half ran on was empty, or
//   2. The set of CPUs where the bottom half ran is not a subset of the CPUs
//      attached to the NUMA node.
//
// This IOCTL returns NV_OK on success, NV_ERR_INVALID_STATE on failure, or
// NV_ERR_NOT_SUPPORTED if UVM thread affinity is not supported.
#define UVM_TEST_NUMA_CHECK_AFFINITY                     UVM_TEST_IOCTL_BASE(78)
typedef struct
{
    NvProcessorUuid                 gpu_uuid;                                           // In

    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_NUMA_CHECK_AFFINITY_PARAMS;

#define UVM_TEST_VA_SPACE_ADD_DUMMY_THREAD_CONTEXTS      UVM_TEST_IOCTL_BASE(79)
typedef struct
{
    // Number of thread contexts to add per thread context table entry
    NvU32                           num_dummy_thread_contexts;                          // In

    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_VA_SPACE_ADD_DUMMY_THREAD_CONTEXTS_PARAMS;

#define UVM_TEST_VA_SPACE_REMOVE_DUMMY_THREAD_CONTEXTS   UVM_TEST_IOCTL_BASE(80)
typedef struct
{
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_VA_SPACE_REMOVE_DUMMY_THREAD_CONTEXTS_PARAMS;

#define UVM_TEST_THREAD_CONTEXT_SANITY                   UVM_TEST_IOCTL_BASE(81)
typedef struct
{
    // Iterations to run.
    NvU32                           iterations;                                         // In

    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_THREAD_CONTEXT_SANITY_PARAMS;

#define UVM_TEST_THREAD_CONTEXT_PERF                     UVM_TEST_IOCTL_BASE(82)
typedef struct
{
    // Iterations to run.
    NvU32                           iterations;                                         // In

    // Delay, in microseconds, between thread context addition and removal
    NvU32                           delay_us;                                           // In

    // Median time, in nanoseconds, spent in adding and then deleting a thread
    // context.
    NvU64                           ns NV_ALIGN_BYTES(8);                               // Out

    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_THREAD_CONTEXT_PERF_PARAMS;

typedef enum
{
    UVM_TEST_PAGEABLE_MEM_ACCESS_TYPE_NONE = 0,

    // Pageable memory cannot be accessed, but there is an association between
    // this VA space and its owning process. For example, this enables the GPU
    // fault handler to establish CPU mappings.
    UVM_TEST_PAGEABLE_MEM_ACCESS_TYPE_MMU_NOTIFIER,

    UVM_TEST_PAGEABLE_MEM_ACCESS_TYPE_HMM,
    UVM_TEST_PAGEABLE_MEM_ACCESS_TYPE_ATS_KERNEL,
    UVM_TEST_PAGEABLE_MEM_ACCESS_TYPE_ATS_DRIVER,
    UVM_TEST_PAGEABLE_MEM_ACCESS_TYPE_COUNT
} UVM_TEST_PAGEABLE_MEM_ACCESS_TYPE;

#define UVM_TEST_GET_PAGEABLE_MEM_ACCESS_TYPE            UVM_TEST_IOCTL_BASE(83)
typedef struct
{
    // UVM_TEST_PAGEABLE_MEM_ACCESS_TYPE
    NvU32                           type;                                               // Out

    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_GET_PAGEABLE_MEM_ACCESS_TYPE_PARAMS;

// Some events, like fault replays, may not immediately show up in the events
// queue despite calling UVM_TOOLS_FLUSH_EVENTS since that will only flush
// completed events but not pending events. Successful completion of this IOCTL
// guarantees that any replays issued on the given GPU prior to the call will
// have its event enqueued in all the tools sessions which have replay events
// enabled. Also, this IOCTL includes an implicit UVM_TOOLS_FLUSH_EVENTS call.
// Hence, this IOCTL is a superset of UVM_TOOLS_FLUSH_EVENTS. Since this call is
// more expensive than UVM_TOOLS_FLUSH_EVENTS, callers who don't need the above
// mentioned guarantee should consider calling UVM_TOOLS_FLUSH_EVENTS instead.
#define UVM_TEST_TOOLS_FLUSH_REPLAY_EVENTS               UVM_TEST_IOCTL_BASE(84)
typedef struct
{
    NvProcessorUuid                 gpuUuid;                                            // In

    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_TOOLS_FLUSH_REPLAY_EVENTS_PARAMS;

// Many checks are performed when the driver is unloaded. In the event of an
// error, a warning message may be printed to the kernel log. In automated
// testing, a systematic way to check the state of the driver after it is
// unloaded is required for additional test coverage. One userland process may
// register to receive the driver state after its unload, since we cannot use
// /proc or /sys to retrieve driver-specific information for an unloaded driver.
// Any userland process registers the given address (unload_state_buf) with the
// UVM driver. On module unload, if an address has been registered, debugging
// state is written to that address. The data in the address is valid once
// module unload completes.
// Error returns:
// NV_ERR_IN_USE
//  - The unload state buffer has already been registered.
// NV_ERR_INVALID_ADDRESS
//  - unload_state_buf is invalid.
//  - unload_state_buf is not 8-byte aligned.

#define UVM_TEST_REGISTER_UNLOAD_STATE_BUFFER            UVM_TEST_IOCTL_BASE(85)

// Unload debugging states:
#define UVM_TEST_UNLOAD_STATE_MEMORY_LEAK        ((NvU64)0x1)

typedef struct
{
    // unload_state_buf points to a 8-byte buf and must be aligned to 8 bytes.
    NvU64                           unload_state_buf;                                   // In

    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_REGISTER_UNLOAD_STATE_BUFFER_PARAMS;

#define UVM_TEST_RB_TREE_DIRECTED                        UVM_TEST_IOCTL_BASE(86)

typedef struct
{
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_RB_TREE_DIRECTED_PARAMS;

#define UVM_TEST_RB_TREE_RANDOM                          UVM_TEST_IOCTL_BASE(87)

typedef struct
{
    NvU64                           iterations                       NV_ALIGN_BYTES(8); // In

    // Upper key range bound. Randomly generated node keys will not exceed this
    // value.
    NvU64                           range_max;                                          // In

    // This parameter is used to control the size of the tree.
    // The number of nodes in the tree will bounce between 0 and this limit.
    // See uvm_rb_tree_test.c:rbtt_test_get_random_op() for full description.
    NvU32                           node_limit;                                         // In
    NvU32                           seed;                                               // In

    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_RB_TREE_RANDOM_PARAMS;

#define UVM_TEST_HOST_SANITY                             UVM_TEST_IOCTL_BASE(88)
typedef struct
{
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_HOST_SANITY_PARAMS;

// Calls uvm_va_space_mm_or_current_retain() on a VA space,
// then releases the va_space_mm and returns.
#define UVM_TEST_VA_SPACE_MM_OR_CURRENT_RETAIN           UVM_TEST_IOCTL_BASE(89)
typedef struct
{
    // User address of a flag to act as a semaphore. If non-NULL, the address
    // is set to 1 after successful retain but before the sleep.
    NvU64 retain_done_ptr                                            NV_ALIGN_BYTES(8); // In

    // Approximate duration for which to sleep with the va_space_mm retained.
    NvU64 sleep_us                                                   NV_ALIGN_BYTES(8); // In

    // NV_ERR_PAGE_TABLE_NOT_AVAIL  Could not retain va_space_mm
    //                              (uvm_va_space_mm_or_current_retain returned
    //                              NULL)
    NV_STATUS rmStatus;                                                                 // Out
} UVM_TEST_VA_SPACE_MM_OR_CURRENT_RETAIN_PARAMS;

#define UVM_TEST_GET_USER_SPACE_END_ADDRESS              UVM_TEST_IOCTL_BASE(90)
typedef struct
{
    NvU64                           user_space_end_address;                             // Out
    NV_STATUS                       rmStatus;                                           // Out
} UVM_TEST_GET_USER_SPACE_END_ADDRESS_PARAMS;

#define UVM_TEST_GET_CPU_CHUNK_ALLOC_SIZES               UVM_TEST_IOCTL_BASE(91)
typedef struct
{
    NvU64                           alloc_size_mask;                                    // Out
    NvU32                           rmStatus;                                           // Out
} UVM_TEST_GET_CPU_CHUNK_ALLOC_SIZES_PARAMS;

// Forces the next range covering the lookup_address to fail in
// uvm_va_range_add_gpu_va_space() with an out-of-memory error. Only the next
// uvm_va_range_add_gpu_va_space() will fail. Subsequent ones will succeed.
//
// Error returns:
// NV_ERR_INVALID_ADDRESS
//  - lookup_address doesn't match a UVM range
#define UVM_TEST_VA_RANGE_INJECT_ADD_GPU_VA_SPACE_ERROR  UVM_TEST_IOCTL_BASE(93)
typedef struct
{
    NvU64     lookup_address NV_ALIGN_BYTES(8);          // In
    NV_STATUS rmStatus;                                  // Out
} UVM_TEST_VA_RANGE_INJECT_ADD_GPU_VA_SPACE_ERROR_PARAMS;

// Forces destroy_gpu_va_space() to delay execution. This provides a high
// probability of exercising the race condition between concurrent
// UvmRegisterGpuVaSpace() calls on the same {va_space, gpu} pair in the
// ATS_KERNEL case.
#define UVM_TEST_DESTROY_GPU_VA_SPACE_DELAY              UVM_TEST_IOCTL_BASE(94)
typedef struct
{
    NvU64 delay_us;                                      // In
    NV_STATUS rmStatus;                                  // Out
} UVM_TEST_DESTROY_GPU_VA_SPACE_DELAY_PARAMS;

#define UVM_TEST_SEC2_SANITY                             UVM_TEST_IOCTL_BASE(95)
typedef struct
{
    NV_STATUS rmStatus;                                  // Out
} UVM_TEST_SEC2_SANITY_PARAMS;

#define UVM_TEST_CGROUP_ACCOUNTING_SUPPORTED             UVM_TEST_IOCTL_BASE(96)
typedef struct
{
    NV_STATUS rmStatus;                                  // Out
} UVM_TEST_CGROUP_ACCOUNTING_SUPPORTED_PARAMS;

#define UVM_TEST_SPLIT_INVALIDATE_DELAY                  UVM_TEST_IOCTL_BASE(98)
typedef struct
{
    NvU64 delay_us;                                      // In
    NV_STATUS rmStatus;                                  // Out
} UVM_TEST_SPLIT_INVALIDATE_DELAY_PARAMS;

// Tests the CSL/SEC2 encryption/decryption methods by doing a secure transfer
// of memory from CPU->GPU and a subsequent GPU->CPU transfer.
#define UVM_TEST_SEC2_CPU_GPU_ROUNDTRIP                  UVM_TEST_IOCTL_BASE(99)
typedef struct
{
    NV_STATUS rmStatus;                                  // Out
} UVM_TEST_SEC2_CPU_GPU_ROUNDTRIP_PARAMS;

#define UVM_TEST_CPU_CHUNK_API                           UVM_TEST_IOCTL_BASE(100)
typedef struct
{
    NV_STATUS rmStatus;                                  // Out
} UVM_TEST_CPU_CHUNK_API_PARAMS;

#define UVM_TEST_FORCE_CPU_TO_CPU_COPY_WITH_CE          UVM_TEST_IOCTL_BASE(101)
typedef struct
{
    NvBool force_copy_with_ce;                          // In
    NV_STATUS rmStatus;                                 // Out
} UVM_TEST_FORCE_CPU_TO_CPU_COPY_WITH_CE_PARAMS;

#define UVM_TEST_VA_SPACE_ALLOW_MOVABLE_ALLOCATIONS     UVM_TEST_IOCTL_BASE(102)
typedef struct
{
    NvBool allow_movable;                               // In
    NV_STATUS rmStatus;                                 // Out
} UVM_TEST_VA_SPACE_ALLOW_MOVABLE_ALLOCATIONS_PARAMS;

#define UVM_TEST_SKIP_MIGRATE_VMA                        UVM_TEST_IOCTL_BASE(103)
typedef struct
{
    NvBool skip;                                         // In
    NV_STATUS rmStatus;                                  // Out
} UVM_TEST_SKIP_MIGRATE_VMA_PARAMS;

#define UVM_TEST_INJECT_TOOLS_EVENT_V2                   UVM_TEST_IOCTL_BASE(104)
typedef struct
{
    // In params
    UvmEventEntry_V2    entry_v2;    // contains only NvUxx types
    NvU32 count;

    // Out param
    NV_STATUS rmStatus;
} UVM_TEST_INJECT_TOOLS_EVENT_V2_PARAMS;

#define UVM_TEST_SET_P2P_SUSPENDED                       UVM_TEST_IOCTL_BASE(105)
typedef struct
{
    // UUID of the processor to check.
    NvProcessorUuid gpu_uuid          NV_ALIGN_BYTES(8); // In
    NvBool suspended;                                    // In

    NV_STATUS rmStatus;                                  // Out
} UVM_TEST_SET_P2P_SUSPENDED_PARAMS;

#define UVM_TEST_INJECT_NVLINK_ERROR                     UVM_TEST_IOCTL_BASE(106)
typedef struct
{
    NvProcessorUuid gpu_uuid;                           // In
    NvU32           error_type;                         // In (UVM_TEST_NVLINK_ERROR_TYPE)
    NV_STATUS       rmStatus;                           // Out
} UVM_TEST_INJECT_NVLINK_ERROR_PARAMS;

#define UVM_TEST_FILE_INITIALIZE                         UVM_TEST_IOCTL_BASE(107)
typedef struct
{
    NV_STATUS               rmStatus;                    // Out
} UVM_TEST_FILE_INITIALIZE_PARAMS;

#define UVM_TEST_FILE_UNMAP                              UVM_TEST_IOCTL_BASE(108)
typedef struct
{
    // File offset at which to start unmapping. Note that this will unmap file
    // offsets, not virtual addresses, so all virtual addresses mapping these
    // offsets will be unmapped.
    NvU64 offset        NV_ALIGN_BYTES(8);  // In

    // Bytes to unmap
    NvU64 length        NV_ALIGN_BYTES(8);  // In

    NV_STATUS rmStatus;                     // Out
} UVM_TEST_FILE_UNMAP_PARAMS;

#define UVM_TEST_QUERY_ACCESS_COUNTERS                   UVM_TEST_IOCTL_BASE(109)
typedef struct
{
    NvProcessorUuid gpu_uuid;               // In
    NvU8 num_notification_buffers;          // Out
    NvU32 num_notification_entries;         // Out

    NV_STATUS rmStatus;                     // Out
} UVM_TEST_QUERY_ACCESS_COUNTERS_PARAMS;

#ifdef __cplusplus
}
#endif

#endif // __UVM_TEST_IOCTL_H__
