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

#ifndef __UVM_HAL_TYPES_H__
#define __UVM_HAL_TYPES_H__

#include "uvm_common.h"
#include "uvm_forward_decl.h"
#include "uvm_processors.h"

#define UVM_GPU_MMU_MAX_FAULT_PACKET_SIZE 32

typedef enum
{
    UVM_APERTURE_PEER_0,
    UVM_APERTURE_PEER_1,
    UVM_APERTURE_PEER_2,
    UVM_APERTURE_PEER_3,
    UVM_APERTURE_PEER_4,
    UVM_APERTURE_PEER_5,
    UVM_APERTURE_PEER_6,
    UVM_APERTURE_PEER_7,
    UVM_APERTURE_PEER_MAX,
    UVM_APERTURE_SYS,
    UVM_APERTURE_VID,

    // DEFAULT is a special value to let MMU pick the location of page tables
    UVM_APERTURE_DEFAULT,

    UVM_APERTURE_MAX
} uvm_aperture_t;

const char *uvm_aperture_string(uvm_aperture_t aperture);

static bool uvm_aperture_is_peer(uvm_aperture_t aperture)
{
    return (aperture >= UVM_APERTURE_PEER_0) && (aperture < UVM_APERTURE_PEER_MAX);
}

static inline NvU32 UVM_APERTURE_PEER_ID(uvm_aperture_t aperture)
{
    UVM_ASSERT(uvm_aperture_is_peer(aperture));

    return (NvU32)aperture;
}

static inline uvm_aperture_t UVM_APERTURE_PEER(NvU32 id)
{
    uvm_aperture_t aperture = (uvm_aperture_t)id;

    UVM_ASSERT(UVM_APERTURE_PEER_ID(aperture) == id);

    return aperture;
}

// A physical GPU address
typedef struct
{
    NvU64 address;

    uvm_aperture_t aperture;
} uvm_gpu_phys_address_t;

// Create a physical GPU address
static uvm_gpu_phys_address_t uvm_gpu_phys_address(uvm_aperture_t aperture, NvU64 address)
{
    return (uvm_gpu_phys_address_t){ address, aperture };
}

// Compare two gpu physical addresses
static int uvm_gpu_phys_addr_cmp(uvm_gpu_phys_address_t a, uvm_gpu_phys_address_t b)
{
    int result = UVM_CMP_DEFAULT(a.aperture, b.aperture);
    if (result != 0)
        return result;

    return UVM_CMP_DEFAULT(a.address, b.address);
}

// A physical or virtual address directly accessible by a GPU.
// This implies that the address already went through identity mapping and IOMMU
// translations and is only valid for a specific GPU.
typedef struct
{
    // Physical or virtual address
    // In general, only valid for a specific GPU
    NvU64 address;

    // Aperture for a physical address
    uvm_aperture_t aperture;

    // Whether the address is virtual
    bool is_virtual;

    // Whether the address resides in a non-protected memory region when the
    // Confidential Computing feature is enabled. Default is protected.
    // Ignored if the feature is disabled and should not be used.
    bool is_unprotected;
} uvm_gpu_address_t;

// Create a virtual GPU address
static uvm_gpu_address_t uvm_gpu_address_virtual(NvU64 va)
{
    uvm_gpu_address_t address = {0};
    address.address = va;
    address.aperture = UVM_APERTURE_MAX;
    address.is_virtual = true;
    return address;
}

static uvm_gpu_address_t uvm_gpu_address_virtual_unprotected(NvU64 va)
{
    uvm_gpu_address_t address = uvm_gpu_address_virtual(va);
    address.is_unprotected = true;
    return address;
}

// Create a physical GPU address
static uvm_gpu_address_t uvm_gpu_address_physical(uvm_aperture_t aperture, NvU64 pa)
{
    uvm_gpu_address_t address = {0};
    address.aperture = aperture;
    address.address = pa;
    return address;
}

// Create a GPU address from a physical GPU address
static uvm_gpu_address_t uvm_gpu_address_from_phys(uvm_gpu_phys_address_t phys_address)
{
    return uvm_gpu_address_physical(phys_address.aperture, phys_address.address);
}

static const char *uvm_gpu_address_aperture_string(uvm_gpu_address_t addr)
{
    if (addr.is_virtual)
        return "VIRTUAL";
    return uvm_aperture_string(addr.aperture);
}

// Compare two gpu addresses
static int uvm_gpu_addr_cmp(uvm_gpu_address_t a, uvm_gpu_address_t b)
{
    int result = UVM_CMP_DEFAULT(a.is_virtual, b.is_virtual);
    if (result != 0)
        return result;

    if (a.is_virtual) {
        return UVM_CMP_DEFAULT(a.address, b.address);
    }
    else {
        uvm_gpu_phys_address_t phys_a = { a.address, a.aperture };
        uvm_gpu_phys_address_t phys_b = { b.address, b.aperture };

        return uvm_gpu_phys_addr_cmp(phys_a, phys_b);
    }
}

// For processors with no concept of an atomic fault (the CPU and pre-Pascal
// GPUs), UVM_PROT_READ_WRITE and UVM_PROT_READ_WRITE_ATOMIC are
// interchangeable.
typedef enum
{
    UVM_PROT_NONE,
    UVM_PROT_READ_ONLY,
    UVM_PROT_READ_WRITE,
    UVM_PROT_READ_WRITE_ATOMIC,
    UVM_PROT_MAX
} uvm_prot_t;

const char *uvm_prot_string(uvm_prot_t prot);

typedef enum
{
    UVM_MEMBAR_NONE,
    UVM_MEMBAR_GPU,
    UVM_MEMBAR_SYS,
} uvm_membar_t;

const char *uvm_membar_string(uvm_membar_t membar);

// Types of memory accesses that can cause a replayable fault on the GPU. They
// are ordered by access "intrusiveness" to simplify fault preprocessing (e.g.
// to implement fault coalescing)
typedef enum
{
    UVM_FAULT_ACCESS_TYPE_PREFETCH = 0,
    UVM_FAULT_ACCESS_TYPE_READ,
    UVM_FAULT_ACCESS_TYPE_WRITE,
    UVM_FAULT_ACCESS_TYPE_ATOMIC_WEAK,
    UVM_FAULT_ACCESS_TYPE_ATOMIC_STRONG,
    UVM_FAULT_ACCESS_TYPE_COUNT
} uvm_fault_access_type_t;

const char *uvm_fault_access_type_string(uvm_fault_access_type_t fault_access_type);

static NvU32 uvm_fault_access_type_mask_bit(uvm_fault_access_type_t fault_access_type)
{
    BUILD_BUG_ON(UVM_FAULT_ACCESS_TYPE_COUNT >= 32);

    UVM_ASSERT(fault_access_type >= 0);
    UVM_ASSERT(fault_access_type < UVM_FAULT_ACCESS_TYPE_COUNT);

    return (NvU32)1 << fault_access_type;
}

static bool uvm_fault_access_type_mask_test(NvU32 mask, uvm_fault_access_type_t fault_access_type)
{
    return uvm_fault_access_type_mask_bit(fault_access_type) & mask;
}

static void uvm_fault_access_type_mask_set(NvU32 *mask, uvm_fault_access_type_t fault_access_type)
{
    *mask |= uvm_fault_access_type_mask_bit(fault_access_type);
}

static uvm_fault_access_type_t uvm_fault_access_type_mask_highest(NvU32 mask)
{
    int pos;

    UVM_ASSERT((1 << UVM_FAULT_ACCESS_TYPE_COUNT) > mask);
    UVM_ASSERT(mask != 0);

    pos = __fls(mask);
    UVM_ASSERT(pos < UVM_FAULT_ACCESS_TYPE_COUNT);

    return pos;
}

static uvm_fault_access_type_t uvm_fault_access_type_mask_lowest(NvU32 mask)
{
    int pos;

    UVM_ASSERT((1 << UVM_FAULT_ACCESS_TYPE_COUNT) > mask);
    UVM_ASSERT(mask != 0);

    pos = __ffs(mask);
    UVM_ASSERT(pos < UVM_FAULT_ACCESS_TYPE_COUNT);

    return pos;
}

typedef enum
{
    // Cancel all accesses on the page
    UVM_FAULT_CANCEL_VA_MODE_ALL = 0,

    // Cancel write and atomic accesses on the page
    UVM_FAULT_CANCEL_VA_MODE_WRITE_AND_ATOMIC,

    UVM_FAULT_CANCEL_VA_MODE_COUNT,
} uvm_fault_cancel_va_mode_t;

// Types of faults that can show up in the fault buffer. Non-UVM related faults
// are grouped in FATAL category since we don't care about the specific type.
typedef enum
{
    UVM_FAULT_TYPE_INVALID_PDE = 0,
    UVM_FAULT_TYPE_INVALID_PTE,
    UVM_FAULT_TYPE_ATOMIC,

    // WRITE to READ-ONLY
    UVM_FAULT_TYPE_WRITE,

    // READ to WRITE-ONLY (ATS)
    UVM_FAULT_TYPE_READ,

    // The next values are considered fatal and are not handled by the UVM
    // driver
    UVM_FAULT_TYPE_FATAL,

    // Values required for tools
    UVM_FAULT_TYPE_PDE_SIZE = UVM_FAULT_TYPE_FATAL,
    UVM_FAULT_TYPE_VA_LIMIT_VIOLATION,
    UVM_FAULT_TYPE_UNBOUND_INST_BLOCK,
    UVM_FAULT_TYPE_PRIV_VIOLATION,
    UVM_FAULT_TYPE_PITCH_MASK_VIOLATION,
    UVM_FAULT_TYPE_WORK_CREATION,
    UVM_FAULT_TYPE_UNSUPPORTED_APERTURE,
    UVM_FAULT_TYPE_COMPRESSION_FAILURE,
    UVM_FAULT_TYPE_UNSUPPORTED_KIND,
    UVM_FAULT_TYPE_REGION_VIOLATION,
    UVM_FAULT_TYPE_POISONED,
    UVM_FAULT_TYPE_CC_VIOLATION,

    UVM_FAULT_TYPE_COUNT
} uvm_fault_type_t;

const char *uvm_fault_type_string(uvm_fault_type_t fault_type);

// Main MMU client type that triggered the fault
typedef enum
{
    UVM_FAULT_CLIENT_TYPE_GPC = 0,
    UVM_FAULT_CLIENT_TYPE_HUB,
    UVM_FAULT_CLIENT_TYPE_COUNT
} uvm_fault_client_type_t;

const char *uvm_fault_client_type_string(uvm_fault_client_type_t fault_client_type);

typedef enum
{
    UVM_MMU_ENGINE_TYPE_GRAPHICS = 0,
    UVM_MMU_ENGINE_TYPE_HOST,
    UVM_MMU_ENGINE_TYPE_CE,
    UVM_MMU_ENGINE_TYPE_COUNT,
} uvm_mmu_engine_type_t;

typedef enum
{
    // Allow entry to be fetched before the previous entry finishes ESCHED
    // execution.
    UVM_GPFIFO_SYNC_PROCEED = 0,

    // Fetch of this entry has to wait until the previous entry has finished
    // executing by ESCHED.
    // For a complete engine sync the previous entry needs to include
    // WAIT_FOR_IDLE command or other engine synchronization.
    UVM_GPFIFO_SYNC_WAIT,
} uvm_gpfifo_sync_t;

const char *uvm_mmu_engine_type_string(uvm_mmu_engine_type_t mmu_engine_type);

// HW unit that triggered the fault. We include the fields required for fault
// cancelling. Including more information might be useful for performance
// heuristics in the future.
typedef struct
{
    uvm_fault_client_type_t                client_type  : order_base_2(UVM_FAULT_CLIENT_TYPE_COUNT) + 1;

    uvm_mmu_engine_type_t              mmu_engine_type  : order_base_2(UVM_MMU_ENGINE_TYPE_COUNT) + 1;

    NvU16                                    client_id;

    NvU16                                mmu_engine_id;

    union
    {
        struct
        {
            NvU16                              utlb_id;

            NvU8                                gpc_id;
        };

        // TODO: Bug 3283289: the channel ID, which is only populated for
        // non-replayable faults, is never consumed.
        NvU16                               channel_id;
    };


    // Identifier of the subcontext that caused the fault. HW uses it as an
    // offset in the instance block to obtain the GPU VA space PDB of the
    // faulting process.
    NvU8                                         ve_id;
} uvm_fault_source_t;

struct uvm_fault_buffer_entry_struct
{
    //
    // The next fields are filled by the fault buffer parsing code
    //

    // Virtual address of the faulting request aligned to CPU page size
    NvU64                                fault_address;

    // GPU timestamp in (nanoseconds) when the fault was inserted in the fault
    // buffer
    NvU64                                    timestamp;

    uvm_gpu_phys_address_t                instance_ptr;

    uvm_fault_source_t                    fault_source;

    uvm_fault_type_t                        fault_type : order_base_2(UVM_FAULT_TYPE_COUNT) + 1;

    uvm_fault_access_type_t          fault_access_type : order_base_2(UVM_FAULT_ACCESS_TYPE_COUNT) + 1;

    //
    // The next fields are managed by the fault handling code
    //

    uvm_va_space_t                           *va_space;
    uvm_gpu_t                                     *gpu;

    // This is set to true when some fault could not be serviced and a
    // cancel command needs to be issued
    bool                                      is_fatal : 1;

    // This is set to true for all GPU faults on a page that is thrashing
    bool                                  is_throttled : 1;

    // This is set to true if the fault has prefetch access type and the
    // address or the access privileges are not valid
    bool                           is_invalid_prefetch : 1;

    bool                                 is_replayable : 1;

    bool                                    is_virtual : 1;

    bool                             in_protected_mode : 1;

    bool                                      filtered : 1;

    // Reason for the fault to be fatal
    UvmEventFatalReason                   fatal_reason : order_base_2(UvmEventNumFatalReasons) + 1;

    // Mode to be used to cancel faults. This must be set according to the
    // fatal fault reason and the fault access types of the merged fault
    // instances.
    union
    {
        struct
        {
            uvm_fault_cancel_va_mode_t  cancel_va_mode : order_base_2(UVM_FAULT_CANCEL_VA_MODE_COUNT) + 1;
        } replayable;

        struct
        {
            NvU32                         buffer_index;
        } non_replayable;
    };

    // List of duplicate fault buffer entries that have been merged into this
    // one
    struct list_head        merged_instances_list;

    // Access types to this page for all accesses that have been coalesced at
    // fetch time. It must include, at least, fault_access_type
    NvU32                        access_type_mask;

    // Number of faults with the same properties that have been coalesced at
    // fetch time
    NvU16                           num_instances;
};

typedef enum
{
    // Completes when all fault replays are in-flight
    UVM_FAULT_REPLAY_TYPE_START = 0,

    // Completes when all faulting accesses have been correctly translated or
    // faulted again
    UVM_FAULT_REPLAY_TYPE_START_ACK_ALL,

    UVM_FAULT_REPLAY_TYPE_MAX
} uvm_fault_replay_type_t;

static uvm_membar_t uvm_membar_max(uvm_membar_t membar_1, uvm_membar_t membar_2)
{
    BUILD_BUG_ON(UVM_MEMBAR_NONE >= UVM_MEMBAR_GPU);
    BUILD_BUG_ON(UVM_MEMBAR_GPU >= UVM_MEMBAR_SYS);
    return max(membar_1, membar_2);
}

typedef enum
{
    UVM_ACCESS_COUNTER_TYPE_MIMC = 0,
    UVM_ACCESS_COUNTER_TYPE_MOMC,

    UVM_ACCESS_COUNTER_TYPE_MAX,
} uvm_access_counter_type_t;

const char *uvm_access_counter_type_string(uvm_access_counter_type_t access_counter_type);

struct uvm_access_counter_buffer_entry_struct
{
    // Whether this counter refers to outbound accesses to remote GPUs or
    // sysmem (MIMC), or it refers to inbound accesses from CPU or a non-peer
    // GPU (whose accesses are routed through the CPU, too) to vidmem (MOMC)
    uvm_access_counter_type_t counter_type;

    // Address of the region for which a notification was sent
    uvm_gpu_address_t address;

    union
    {
        // These fields are only valid if address.is_virtual is true
        struct
        {
            // Instance pointer of one of the channels in the TSG that triggered
            // the notification.
            uvm_gpu_phys_address_t instance_ptr;

            uvm_mmu_engine_type_t mmu_engine_type;

            NvU32 mmu_engine_id;

            // Identifier of the subcontext that performed the memory accesses
            // that triggered the notification. This value, combined with the
            // instance_ptr, is needed to obtain the GPU VA space of the process
            // that triggered the notification.
            NvU32 ve_id;

            // VA space for the address that triggered the notification
            uvm_va_space_t *va_space;
        } virtual_info;

        // These fields are only valid if address.is_virtual is false
        struct
        {
            // Processor id where data is resident
            //
            // Although this information is not tied to a VA space, we can use
            // a regular processor id because P2P is not allowed between
            // partitioned GPUs.
            uvm_processor_id_t resident_id;

        } physical_info;
    };

    // This is the GPU that triggered the notification. Note that physical
    // address based notifications are only supported on non-MIG-capable GPUs.
    uvm_gpu_t *gpu;

    // Number of times the tracked region was accessed since the last time it
    // was cleared. Counter values saturate at the maximum value supported by
    // the GPU (2^16 - 1 in Volta)
    NvU32 counter_value;

    // When the granularity of the tracked regions is greater than 64KB, the
    // region is split into 32 equal subregions. Each bit in this field
    // represents one of those subregions. 1 means that the subregion has been
    // accessed
    NvU32 sub_granularity;

    // Opaque fields provided by HW, required for targeted clear of a counter
    NvU32 bank;
    NvU32 tag;
};

static uvm_prot_t uvm_fault_access_type_to_prot(uvm_fault_access_type_t access_type)
{
    switch (access_type) {
        case UVM_FAULT_ACCESS_TYPE_ATOMIC_STRONG:
            return UVM_PROT_READ_WRITE_ATOMIC;

        case UVM_FAULT_ACCESS_TYPE_ATOMIC_WEAK:
        case UVM_FAULT_ACCESS_TYPE_WRITE:
            return UVM_PROT_READ_WRITE;

        default:
            // Prefetch faults, if not ignored, are handled like read faults and
            // requirea mapping with, at least, READ_ONLY access permission.
            return UVM_PROT_READ_ONLY;
    }
}

#endif // __UVM_HAL_TYPES_H__
