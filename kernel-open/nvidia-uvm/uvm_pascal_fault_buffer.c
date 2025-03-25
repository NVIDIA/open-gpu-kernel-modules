/*******************************************************************************
    Copyright (c) 2016-2025 NVIDIA Corporation

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

#include "uvm_linux.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_hal.h"
#include "uvm_push.h"
#include "hwref/pascal/gp100/dev_fault.h"
#include "clb069.h"
#include "uvm_pascal_fault_buffer.h"

typedef struct {
    NvU8 bufferEntry[NVB069_FAULT_BUF_SIZE];
} fault_buffer_entry_b069_t;

void uvm_hal_pascal_clear_replayable_faults(uvm_parent_gpu_t *parent_gpu, NvU32 get)
{
    // No-op, this function is only used by pulse-based interrupt GPUs.
}

void uvm_hal_pascal_enable_replayable_faults(uvm_parent_gpu_t *parent_gpu)
{
    volatile NvU32 *reg;
    NvU32 mask;

    reg = parent_gpu->fault_buffer.rm_info.replayable.pPmcIntrEnSet;
    mask = parent_gpu->fault_buffer.rm_info.replayable.replayableFaultMask;

    UVM_GPU_WRITE_ONCE(*reg, mask);
}

void uvm_hal_pascal_disable_replayable_faults(uvm_parent_gpu_t *parent_gpu)
{
    volatile NvU32 *reg;
    NvU32 mask;

    reg = parent_gpu->fault_buffer.rm_info.replayable.pPmcIntrEnClear;
    mask = parent_gpu->fault_buffer.rm_info.replayable.replayableFaultMask;

    UVM_GPU_WRITE_ONCE(*reg, mask);
}

NvU32 uvm_hal_pascal_fault_buffer_read_put(uvm_parent_gpu_t *parent_gpu)
{
    NvU32 put = UVM_GPU_READ_ONCE(*parent_gpu->fault_buffer.rm_info.replayable.pFaultBufferPut);
    UVM_ASSERT(put < parent_gpu->fault_buffer.replayable.max_faults);

    return put;
}

NvU32 uvm_hal_pascal_fault_buffer_read_get(uvm_parent_gpu_t *parent_gpu)
{
    NvU32 get = UVM_GPU_READ_ONCE(*parent_gpu->fault_buffer.rm_info.replayable.pFaultBufferGet);
    UVM_ASSERT(get < parent_gpu->fault_buffer.replayable.max_faults);

    return get;
}

void uvm_hal_pascal_fault_buffer_write_get(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    UVM_ASSERT(index < parent_gpu->fault_buffer.replayable.max_faults);

    UVM_GPU_WRITE_ONCE(*parent_gpu->fault_buffer.rm_info.replayable.pFaultBufferGet, index);
}

static uvm_fault_access_type_t get_fault_access_type(const NvU32 *fault_entry)
{
    NvU32 hw_access_type_value = READ_HWVALUE_MW(fault_entry, B069, FAULT_BUF_ENTRY, ACCESS_TYPE);

    switch (hw_access_type_value)
    {
        case NV_PFAULT_ACCESS_TYPE_READ:
            return UVM_FAULT_ACCESS_TYPE_READ;
        case NV_PFAULT_ACCESS_TYPE_WRITE:
            return UVM_FAULT_ACCESS_TYPE_WRITE;
        case NV_PFAULT_ACCESS_TYPE_ATOMIC:
            return UVM_FAULT_ACCESS_TYPE_ATOMIC_STRONG;
        case NV_PFAULT_ACCESS_TYPE_PREFETCH:
            return UVM_FAULT_ACCESS_TYPE_PREFETCH;
    }

    UVM_ASSERT_MSG(false, "Invalid fault access type value: %d\n", hw_access_type_value);

    return UVM_FAULT_ACCESS_TYPE_COUNT;
}

uvm_fault_type_t uvm_hal_pascal_fault_buffer_get_fault_type(const NvU32 *fault_entry)
{
    NvU32 hw_fault_type_value = READ_HWVALUE_MW(fault_entry, B069, FAULT_BUF_ENTRY, FAULT_TYPE);

    switch (hw_fault_type_value)
    {
        case NV_PFAULT_FAULT_TYPE_PDE:
            return UVM_FAULT_TYPE_INVALID_PDE;
        case NV_PFAULT_FAULT_TYPE_PTE:
            return UVM_FAULT_TYPE_INVALID_PTE;
        case NV_PFAULT_FAULT_TYPE_RO_VIOLATION:
            return UVM_FAULT_TYPE_WRITE;
        case NV_PFAULT_FAULT_TYPE_ATOMIC_VIOLATION:
            return UVM_FAULT_TYPE_ATOMIC;

        case NV_PFAULT_FAULT_TYPE_PDE_SIZE:
            return UVM_FAULT_TYPE_PDE_SIZE;
        case NV_PFAULT_FAULT_TYPE_VA_LIMIT_VIOLATION:
            return UVM_FAULT_TYPE_VA_LIMIT_VIOLATION;
        case NV_PFAULT_FAULT_TYPE_UNBOUND_INST_BLOCK:
            return UVM_FAULT_TYPE_UNBOUND_INST_BLOCK;
        case NV_PFAULT_FAULT_TYPE_PRIV_VIOLATION:
            return UVM_FAULT_TYPE_PRIV_VIOLATION;
        case NV_PFAULT_FAULT_TYPE_PITCH_MASK_VIOLATION:
            return UVM_FAULT_TYPE_PITCH_MASK_VIOLATION;
        case NV_PFAULT_FAULT_TYPE_WORK_CREATION:
            return UVM_FAULT_TYPE_WORK_CREATION;
        case NV_PFAULT_FAULT_TYPE_UNSUPPORTED_APERTURE:
            return UVM_FAULT_TYPE_UNSUPPORTED_APERTURE;
        case NV_PFAULT_FAULT_TYPE_COMPRESSION_FAILURE:
            return UVM_FAULT_TYPE_COMPRESSION_FAILURE;
        case NV_PFAULT_FAULT_TYPE_UNSUPPORTED_KIND:
            return UVM_FAULT_TYPE_UNSUPPORTED_KIND;
        case NV_PFAULT_FAULT_TYPE_REGION_VIOLATION:
            return UVM_FAULT_TYPE_REGION_VIOLATION;
        case NV_PFAULT_FAULT_TYPE_POISONED:
            return UVM_FAULT_TYPE_POISONED;
    }

    UVM_ASSERT_MSG(false, "Invalid fault type value: %d\n", hw_fault_type_value);

    return UVM_FAULT_TYPE_COUNT;
}

static uvm_fault_client_type_t get_fault_client_type(const NvU32 *fault_entry)
{
    NvU32 hw_client_type_value = READ_HWVALUE_MW(fault_entry, B069, FAULT_BUF_ENTRY, MMU_CLIENT_TYPE);

    switch (hw_client_type_value)
    {
        case NV_PFAULT_MMU_CLIENT_TYPE_GPC:
            return UVM_FAULT_CLIENT_TYPE_GPC;
        case NV_PFAULT_MMU_CLIENT_TYPE_HUB:
            return UVM_FAULT_CLIENT_TYPE_HUB;
    }

    UVM_ASSERT_MSG(false, "Invalid mmu client type value: %d\n", hw_client_type_value);

    return UVM_FAULT_CLIENT_TYPE_COUNT;
}

static uvm_aperture_t get_fault_inst_aperture(NvU32 *fault_entry)
{
    NvU32 hw_aperture_value = READ_HWVALUE_MW(fault_entry, B069, FAULT_BUF_ENTRY, INST_APERTURE);

    switch (hw_aperture_value)
    {
        case NVB069_FAULT_BUF_ENTRY_INST_APERTURE_VID_MEM:
            return UVM_APERTURE_VID;
        case NVB069_FAULT_BUF_ENTRY_INST_APERTURE_SYS_MEM_COHERENT:
        case NVB069_FAULT_BUF_ENTRY_INST_APERTURE_SYS_MEM_NONCOHERENT:
             return UVM_APERTURE_SYS;
    }

    UVM_ASSERT_MSG(false, "Invalid inst aperture value: %d\n", hw_aperture_value);

    return UVM_APERTURE_MAX;
}

static NvU32 *get_fault_buffer_entry(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    fault_buffer_entry_b069_t *buffer_start;
    NvU32 *fault_entry;

    UVM_ASSERT(index < parent_gpu->fault_buffer.replayable.max_faults);

    buffer_start = (fault_buffer_entry_b069_t *)parent_gpu->fault_buffer.rm_info.replayable.bufferAddress;
    fault_entry = (NvU32 *)&buffer_start[index];

    return fault_entry;
}

// When Confidential Computing is enabled, fault entries are encrypted. Each
// fault has (unencrypted) metadata containing the authentication tag, and a
// valid bit that allows UVM to check if an encrypted fault is valid, without
// having to decrypt it first.
static UvmFaultMetadataPacket *get_fault_buffer_entry_metadata(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    UvmFaultMetadataPacket *fault_entry_metadata;

    UVM_ASSERT(index < parent_gpu->fault_buffer.replayable.max_faults);
    UVM_ASSERT(g_uvm_global.conf_computing_enabled);

    fault_entry_metadata = parent_gpu->fault_buffer.rm_info.replayable.bufferMetadata;
    UVM_ASSERT(fault_entry_metadata != NULL);

    return fault_entry_metadata + index;
}

NV_STATUS uvm_hal_pascal_fault_buffer_parse_replayable_entry(uvm_parent_gpu_t *parent_gpu,
                                                             NvU32 index,
                                                             uvm_fault_buffer_entry_t *buffer_entry)
{
    NvU32 *fault_entry;
    NvU64 addr_hi, addr_lo;
    NvU64 timestamp_hi, timestamp_lo;
    NvU16 gpc_utlb_id;
    NvU32 utlb_id;

    BUILD_BUG_ON(NVB069_FAULT_BUF_SIZE > UVM_GPU_MMU_MAX_FAULT_PACKET_SIZE);

    // Valid bit must be set before this function is called
    UVM_ASSERT(parent_gpu->fault_buffer_hal->entry_is_valid(parent_gpu, index));

    fault_entry = get_fault_buffer_entry(parent_gpu, index);

    addr_hi = READ_HWVALUE_MW(fault_entry, B069, FAULT_BUF_ENTRY, INST_HI);
    addr_lo = READ_HWVALUE_MW(fault_entry, B069, FAULT_BUF_ENTRY, INST_LO);
    buffer_entry->instance_ptr.address = addr_lo + (addr_hi << HWSIZE_MW(B069, FAULT_BUF_ENTRY, INST_LO));
    // HW value contains the 4K page number. Shift to build the full address
    buffer_entry->instance_ptr.address <<= 12;

    buffer_entry->instance_ptr.aperture = get_fault_inst_aperture(fault_entry);

    addr_hi = READ_HWVALUE_MW(fault_entry, B069, FAULT_BUF_ENTRY, ADDR_HI);
    addr_lo = READ_HWVALUE_MW(fault_entry, B069, FAULT_BUF_ENTRY, ADDR_LO);
    buffer_entry->fault_address = addr_lo + (addr_hi << HWSIZE_MW(B069, FAULT_BUF_ENTRY, ADDR_LO));
    buffer_entry->fault_address = uvm_parent_gpu_canonical_address(parent_gpu, buffer_entry->fault_address);

    timestamp_hi = READ_HWVALUE_MW(fault_entry, B069, FAULT_BUF_ENTRY, TIMESTAMP_HI);
    timestamp_lo = READ_HWVALUE_MW(fault_entry, B069, FAULT_BUF_ENTRY, TIMESTAMP_LO);
    buffer_entry->timestamp = timestamp_lo + (timestamp_hi << HWSIZE_MW(B069, FAULT_BUF_ENTRY, TIMESTAMP_LO));

    buffer_entry->fault_type = parent_gpu->fault_buffer_hal->get_fault_type(fault_entry);

    buffer_entry->fault_access_type = get_fault_access_type(fault_entry);

    buffer_entry->fault_source.client_type = get_fault_client_type(fault_entry);
    if (buffer_entry->fault_source.client_type == UVM_FAULT_CLIENT_TYPE_HUB)
        UVM_ASSERT_MSG(false, "Invalid client type: HUB\n");

    buffer_entry->fault_source.client_id = READ_HWVALUE_MW(fault_entry, B069, FAULT_BUF_ENTRY, CLIENT);
    BUILD_BUG_ON(sizeof(buffer_entry->fault_source.client_id) * 8 < DRF_SIZE_MW(NVB069_FAULT_BUF_ENTRY_CLIENT));

    buffer_entry->fault_source.gpc_id = READ_HWVALUE_MW(fault_entry, B069, FAULT_BUF_ENTRY, GPC_ID);
    BUILD_BUG_ON(sizeof(buffer_entry->fault_source.gpc_id) * 8 < DRF_SIZE_MW(NVB069_FAULT_BUF_ENTRY_GPC_ID));

    gpc_utlb_id = parent_gpu->arch_hal->mmu_client_id_to_utlb_id(buffer_entry->fault_source.client_id);
    UVM_ASSERT(gpc_utlb_id < parent_gpu->utlb_per_gpc_count);

    // Compute global uTLB id
    utlb_id = buffer_entry->fault_source.gpc_id * parent_gpu->utlb_per_gpc_count + gpc_utlb_id;
    UVM_ASSERT(utlb_id < parent_gpu->fault_buffer.replayable.utlb_count);

    buffer_entry->fault_source.utlb_id = utlb_id;

    buffer_entry->is_replayable = true;
    buffer_entry->is_virtual = true;
    buffer_entry->in_protected_mode = false;
    buffer_entry->fault_source.mmu_engine_type = UVM_MMU_ENGINE_TYPE_GRAPHICS;
    buffer_entry->fault_source.mmu_engine_id = NV_PFAULT_MMU_ENG_ID_GRAPHICS;
    buffer_entry->fault_source.ve_id = 0;

    // Automatically clear valid bit for the entry in the fault buffer
    uvm_hal_pascal_fault_buffer_entry_clear_valid(parent_gpu, index);

    return NV_OK;
}

bool uvm_hal_pascal_fault_buffer_entry_is_valid(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    if (g_uvm_global.conf_computing_enabled) {
        // Use the valid bit present in the encryption metadata, which is
        // unencrypted, instead of the valid bit present in the (encrypted)
        // fault itself.
        UvmFaultMetadataPacket *fault_entry_metadata = get_fault_buffer_entry_metadata(parent_gpu, index);

        return fault_entry_metadata->valid;
    }
    else {
        NvU32 *fault_entry = get_fault_buffer_entry(parent_gpu, index);

        return READ_HWVALUE_MW(fault_entry, B069, FAULT_BUF_ENTRY, VALID);
    }
}

void uvm_hal_pascal_fault_buffer_entry_clear_valid(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    if (g_uvm_global.conf_computing_enabled) {
        // Use the valid bit present in the encryption metadata, which is
        // unencrypted, instead of the valid bit present in the (encrypted)
        // fault itself.
        UvmFaultMetadataPacket *fault_entry_metadata = get_fault_buffer_entry_metadata(parent_gpu, index);

        fault_entry_metadata->valid = false;
    }
    else {
        NvU32 *fault_entry = get_fault_buffer_entry(parent_gpu, index);

        WRITE_HWCONST_MW(fault_entry, B069, FAULT_BUF_ENTRY, VALID, FALSE);
    }
}

NvU32 uvm_hal_pascal_fault_buffer_entry_size(uvm_parent_gpu_t *parent_gpu)
{
    return NVB069_FAULT_BUF_SIZE;
}
