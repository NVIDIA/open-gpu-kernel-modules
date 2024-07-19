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

#include "uvm_linux.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_hal.h"
#include "uvm_conf_computing.h"
#include "nv_uvm_types.h"
#include "hwref/volta/gv100/dev_fault.h"
#include "hwref/volta/gv100/dev_fb.h"
#include "clc369.h"
#include "uvm_volta_fault_buffer.h"

typedef struct {
    NvU8 bufferEntry[NVC369_BUF_SIZE];
} fault_buffer_entry_c369_t;

NvU32 uvm_hal_volta_fault_buffer_read_put(uvm_parent_gpu_t *parent_gpu)
{
    NvU32 put = UVM_GPU_READ_ONCE(*parent_gpu->fault_buffer_info.rm_info.replayable.pFaultBufferPut);
    NvU32 index = READ_HWVALUE(put, _PFB_PRI_MMU, FAULT_BUFFER_PUT, PTR);
    UVM_ASSERT(READ_HWVALUE(put, _PFB_PRI_MMU, FAULT_BUFFER_PUT, GETPTR_CORRUPTED) ==
               NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_GETPTR_CORRUPTED_NO);

    return index;
}

NvU32 uvm_hal_volta_fault_buffer_read_get(uvm_parent_gpu_t *parent_gpu)
{
    NvU32 get = UVM_GPU_READ_ONCE(*parent_gpu->fault_buffer_info.rm_info.replayable.pFaultBufferGet);
    UVM_ASSERT(get < parent_gpu->fault_buffer_info.replayable.max_faults);

    return READ_HWVALUE(get, _PFB_PRI_MMU, FAULT_BUFFER_GET, PTR);
}

void uvm_hal_volta_fault_buffer_write_get(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    NvU32 get = HWVALUE(_PFB_PRI_MMU, FAULT_BUFFER_GET, PTR, index);

    UVM_ASSERT(index < parent_gpu->fault_buffer_info.replayable.max_faults);

    // If HW has detected an overflow condition (PUT == GET - 1 and a fault has
    // arrived, which is dropped due to no more space in the fault buffer), it
    // will not deliver any more faults into the buffer until the overflow
    // condition has been cleared. The overflow condition is cleared by
    // updating the GET index to indicate space in the buffer and writing 1 to
    // the OVERFLOW bit in GET. Unfortunately, this can not be done in the same
    // write because it can collide with an arriving fault on the same cycle,
    // resulting in the overflow condition being instantly reasserted. However,
    // if the index is updated first and then the OVERFLOW bit is cleared such
    // a collision will not cause a reassertion of the overflow condition.
    UVM_GPU_WRITE_ONCE(*parent_gpu->fault_buffer_info.rm_info.replayable.pFaultBufferGet, get);

    // Clearing GETPTR_CORRUPTED and OVERFLOW is not needed when GSP-RM owns
    // the HW replayable fault buffer, because UVM does not write to the actual
    // GET register; GSP-RM is responsible for clearing the bits in the real
    // GET register.
    if (g_uvm_global.conf_computing_enabled)
        return;

    // Clear the GETPTR_CORRUPTED and OVERFLOW bits.
    get |= HWCONST(_PFB_PRI_MMU, FAULT_BUFFER_GET, GETPTR_CORRUPTED, CLEAR) |
           HWCONST(_PFB_PRI_MMU, FAULT_BUFFER_GET, OVERFLOW, CLEAR);
    UVM_GPU_WRITE_ONCE(*parent_gpu->fault_buffer_info.rm_info.replayable.pFaultBufferGet, get);
}

// TODO: Bug  1835884: [uvm] Query the maximum number of subcontexts from RM
// ... to validate the ve_id
#define MAX_SUBCONTEXTS 64
NvU8 uvm_hal_volta_fault_buffer_get_ve_id(NvU16 mmu_engine_id, uvm_mmu_engine_type_t mmu_engine_type)
{
    // Only graphics engines can generate MMU faults from different subcontexts
    if (mmu_engine_type == UVM_MMU_ENGINE_TYPE_GRAPHICS) {
        NvU16 ve_id = mmu_engine_id - NV_PFAULT_MMU_ENG_ID_GRAPHICS;
        UVM_ASSERT(ve_id < MAX_SUBCONTEXTS);

        return (NvU8)ve_id;
    }
    else {
        return 0;
    }
}

static uvm_fault_access_type_t get_fault_access_type(const NvU32 *fault_entry)
{
    NvU32 hw_access_type_value = READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, ACCESS_TYPE);

    switch (hw_access_type_value) {
        case NV_PFAULT_ACCESS_TYPE_PHYS_READ:
        case NV_PFAULT_ACCESS_TYPE_VIRT_READ:
            return UVM_FAULT_ACCESS_TYPE_READ;
        case NV_PFAULT_ACCESS_TYPE_PHYS_WRITE:
        case NV_PFAULT_ACCESS_TYPE_VIRT_WRITE:
            return UVM_FAULT_ACCESS_TYPE_WRITE;
        case NV_PFAULT_ACCESS_TYPE_PHYS_ATOMIC:
        case NV_PFAULT_ACCESS_TYPE_VIRT_ATOMIC_STRONG:
            return UVM_FAULT_ACCESS_TYPE_ATOMIC_STRONG;
        case NV_PFAULT_ACCESS_TYPE_VIRT_ATOMIC_WEAK:
            return UVM_FAULT_ACCESS_TYPE_ATOMIC_WEAK;
        case NV_PFAULT_ACCESS_TYPE_PHYS_PREFETCH:
        case NV_PFAULT_ACCESS_TYPE_VIRT_PREFETCH:
            return UVM_FAULT_ACCESS_TYPE_PREFETCH;
    }

    UVM_ASSERT_MSG(false, "Invalid fault access type value: %d\n", hw_access_type_value);

    return UVM_FAULT_ACCESS_TYPE_COUNT;
}

static bool is_fault_address_virtual(const NvU32 *fault_entry)
{
    NvU32 hw_access_type_value = READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, ACCESS_TYPE);

    switch (hw_access_type_value) {
        case NV_PFAULT_ACCESS_TYPE_PHYS_READ:
        case NV_PFAULT_ACCESS_TYPE_PHYS_WRITE:
        case NV_PFAULT_ACCESS_TYPE_PHYS_ATOMIC:
        case NV_PFAULT_ACCESS_TYPE_PHYS_PREFETCH:
            return false;
        case NV_PFAULT_ACCESS_TYPE_VIRT_READ:
        case NV_PFAULT_ACCESS_TYPE_VIRT_WRITE:
        case NV_PFAULT_ACCESS_TYPE_VIRT_ATOMIC_STRONG:
        case NV_PFAULT_ACCESS_TYPE_VIRT_ATOMIC_WEAK:
        case NV_PFAULT_ACCESS_TYPE_VIRT_PREFETCH:
            return true;
    }

    UVM_ASSERT_MSG(false, "Invalid fault access type value: %d\n", hw_access_type_value);

    return UVM_FAULT_ACCESS_TYPE_COUNT;
}

uvm_fault_type_t uvm_hal_volta_fault_buffer_get_fault_type(const NvU32 *fault_entry)
{
    NvU32 hw_fault_type_value = READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, FAULT_TYPE);

    switch (hw_fault_type_value) {
        case NV_PFAULT_FAULT_TYPE_PDE:
            return UVM_FAULT_TYPE_INVALID_PDE;
        case NV_PFAULT_FAULT_TYPE_PTE:
            return UVM_FAULT_TYPE_INVALID_PTE;
        case NV_PFAULT_FAULT_TYPE_RO_VIOLATION:
            return UVM_FAULT_TYPE_WRITE;
        case NV_PFAULT_FAULT_TYPE_ATOMIC_VIOLATION:
            return UVM_FAULT_TYPE_ATOMIC;
        case NV_PFAULT_FAULT_TYPE_WO_VIOLATION:
            return UVM_FAULT_TYPE_READ;

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
    NvU32 hw_client_type_value = READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, MMU_CLIENT_TYPE);

    switch (hw_client_type_value) {
        case NV_PFAULT_MMU_CLIENT_TYPE_GPC:
            return UVM_FAULT_CLIENT_TYPE_GPC;
        case NV_PFAULT_MMU_CLIENT_TYPE_HUB:
            return UVM_FAULT_CLIENT_TYPE_HUB;
    }

    UVM_ASSERT_MSG(false, "Invalid mmu client type value: %d\n", hw_client_type_value);

    return UVM_FAULT_CLIENT_TYPE_COUNT;
}

static uvm_aperture_t get_fault_inst_aperture(const NvU32 *fault_entry)
{
    NvU32 hw_aperture_value = READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, INST_APERTURE);

    switch (hw_aperture_value) {
        case NVC369_BUF_ENTRY_INST_APERTURE_VID_MEM:
            return UVM_APERTURE_VID;
        case NVC369_BUF_ENTRY_INST_APERTURE_SYS_MEM_COHERENT:
        case NVC369_BUF_ENTRY_INST_APERTURE_SYS_MEM_NONCOHERENT:
             return UVM_APERTURE_SYS;
    }

    UVM_ASSERT_MSG(false, "Invalid inst aperture value: %d\n", hw_aperture_value);

    return UVM_APERTURE_MAX;
}

static NvU32 *get_fault_buffer_entry(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    fault_buffer_entry_c369_t *buffer_start;
    NvU32 *fault_entry;

    UVM_ASSERT(index < parent_gpu->fault_buffer_info.replayable.max_faults);

    buffer_start = (fault_buffer_entry_c369_t *)parent_gpu->fault_buffer_info.rm_info.replayable.bufferAddress;
    fault_entry = (NvU32 *)&buffer_start[index];

    return fault_entry;
}

// See uvm_pascal_fault_buffer.c::get_fault_buffer_entry_metadata
static UvmFaultMetadataPacket *get_fault_buffer_entry_metadata(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    UvmFaultMetadataPacket *fault_entry_metadata;

    UVM_ASSERT(index < parent_gpu->fault_buffer_info.replayable.max_faults);
    UVM_ASSERT(g_uvm_global.conf_computing_enabled);

    fault_entry_metadata = parent_gpu->fault_buffer_info.rm_info.replayable.bufferMetadata;
    UVM_ASSERT(fault_entry_metadata != NULL);

    return fault_entry_metadata + index;
}

static bool client_id_ce(NvU16 client_id)
{
    if (client_id >= NV_PFAULT_CLIENT_HUB_HSCE0 && client_id <= NV_PFAULT_CLIENT_HUB_HSCE9)
        return true;

    switch (client_id) {
        case NV_PFAULT_CLIENT_HUB_CE0:
        case NV_PFAULT_CLIENT_HUB_CE1:
        case NV_PFAULT_CLIENT_HUB_CE2:
            return true;
    }

    return false;
}

static bool client_id_host(NvU16 client_id)
{
    switch (client_id) {
        case NV_PFAULT_CLIENT_HUB_HOST:
        case NV_PFAULT_CLIENT_HUB_HOST_CPU:
            return true;
    }

    return false;
}

uvm_mmu_engine_type_t uvm_hal_volta_fault_buffer_get_mmu_engine_type(NvU16 mmu_engine_id,
                                                                     uvm_fault_client_type_t client_type,
                                                                     NvU16 client_id)
{
    // Servicing CE and Host (HUB clients) faults.
    if (client_type == UVM_FAULT_CLIENT_TYPE_HUB) {
        if (client_id_ce(client_id)) {
            UVM_ASSERT(mmu_engine_id >= NV_PFAULT_MMU_ENG_ID_CE0 && mmu_engine_id <= NV_PFAULT_MMU_ENG_ID_CE8);

            return UVM_MMU_ENGINE_TYPE_CE;
        }

        if (client_id_host(client_id)) {
            UVM_ASSERT(mmu_engine_id >= NV_PFAULT_MMU_ENG_ID_HOST0 && mmu_engine_id <= NV_PFAULT_MMU_ENG_ID_HOST13);

            return UVM_MMU_ENGINE_TYPE_HOST;
        }
    }

    // We shouldn't be servicing faults from any other engines oither than GR.
    UVM_ASSERT_MSG(client_id <= NV_PFAULT_CLIENT_GPC_T1_39, "Unexpected client ID: 0x%x\n", client_id);
    UVM_ASSERT_MSG(mmu_engine_id >= NV_PFAULT_MMU_ENG_ID_GRAPHICS, "Unexpected engine ID: 0x%x\n", mmu_engine_id);
    UVM_ASSERT(client_type == UVM_FAULT_CLIENT_TYPE_GPC);

    return UVM_MMU_ENGINE_TYPE_GRAPHICS;
}

static void parse_fault_entry_common(uvm_parent_gpu_t *parent_gpu,
                                     NvU32 *fault_entry,
                                     uvm_fault_buffer_entry_t *buffer_entry)
{
    NvU64 addr_hi, addr_lo;
    NvU64 timestamp_hi, timestamp_lo;
    bool replayable_fault_enabled;

    addr_hi = READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, INST_HI);
    addr_lo = READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, INST_LO);
    buffer_entry->instance_ptr.address = addr_lo + (addr_hi << HWSIZE_MW(C369, BUF_ENTRY, INST_LO));

    // HW value contains the 4K page number. Shift to build the full address
    buffer_entry->instance_ptr.address <<= 12;

    buffer_entry->instance_ptr.aperture = get_fault_inst_aperture(fault_entry);

    addr_hi = READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, ADDR_HI);
    addr_lo = READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, ADDR_LO);

    // HW value contains the 4K page number. Shift to build the full address
    buffer_entry->fault_address = (addr_lo + (addr_hi << HWSIZE_MW(C369, BUF_ENTRY, ADDR_LO))) << 12;
    buffer_entry->fault_address = uvm_parent_gpu_canonical_address(parent_gpu, buffer_entry->fault_address);

    timestamp_hi = READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, TIMESTAMP_HI);
    timestamp_lo = READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, TIMESTAMP_LO);
    buffer_entry->timestamp = timestamp_lo + (timestamp_hi << HWSIZE_MW(C369, BUF_ENTRY, TIMESTAMP_LO));

    buffer_entry->fault_type = parent_gpu->fault_buffer_hal->get_fault_type(fault_entry);

    buffer_entry->fault_access_type = get_fault_access_type(fault_entry);

    buffer_entry->fault_source.client_type = get_fault_client_type(fault_entry);

    buffer_entry->fault_source.client_id = READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, CLIENT);
    BUILD_BUG_ON(sizeof(buffer_entry->fault_source.client_id) * 8 < DRF_SIZE_MW(NVC369_BUF_ENTRY_CLIENT));

    buffer_entry->fault_source.gpc_id = READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, GPC_ID);
    BUILD_BUG_ON(sizeof(buffer_entry->fault_source.gpc_id) * 8 < DRF_SIZE_MW(NVC369_BUF_ENTRY_GPC_ID));

    buffer_entry->is_replayable = (READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, REPLAYABLE_FAULT) ==
                                   NVC369_BUF_ENTRY_REPLAYABLE_FAULT_TRUE);

    // Compute global uTLB id
    if (buffer_entry->fault_source.client_type == UVM_FAULT_CLIENT_TYPE_GPC) {
        NvU16 gpc_utlb_id = parent_gpu->arch_hal->mmu_client_id_to_utlb_id(buffer_entry->fault_source.client_id);
        NvU32 utlb_id;
        UVM_ASSERT(gpc_utlb_id < parent_gpu->utlb_per_gpc_count);

        utlb_id = buffer_entry->fault_source.gpc_id * parent_gpu->utlb_per_gpc_count + gpc_utlb_id;
        UVM_ASSERT(utlb_id < parent_gpu->fault_buffer_info.replayable.utlb_count);

        buffer_entry->fault_source.utlb_id = utlb_id;
    }
    else if (buffer_entry->fault_source.client_type == UVM_FAULT_CLIENT_TYPE_HUB) {
        buffer_entry->fault_source.utlb_id = 0;
    }

    buffer_entry->fault_source.mmu_engine_id = READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, ENGINE_ID);
    BUILD_BUG_ON(sizeof(buffer_entry->fault_source.mmu_engine_id) * 8 < DRF_SIZE_MW(NVC369_BUF_ENTRY_ENGINE_ID));

    buffer_entry->fault_source.mmu_engine_type =
        parent_gpu->fault_buffer_hal->get_mmu_engine_type(buffer_entry->fault_source.mmu_engine_id,
                                                          buffer_entry->fault_source.client_type,
                                                          buffer_entry->fault_source.client_id);

    buffer_entry->fault_source.ve_id =
        parent_gpu->fault_buffer_hal->get_ve_id(buffer_entry->fault_source.mmu_engine_id,
                                                buffer_entry->fault_source.mmu_engine_type);
    BUILD_BUG_ON(1 << (sizeof(buffer_entry->fault_source.ve_id) * 8) < MAX_SUBCONTEXTS);

    buffer_entry->is_virtual = is_fault_address_virtual(fault_entry);

    buffer_entry->in_protected_mode = (READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, PROTECTED_MODE) ==
                                       NVC369_BUF_ENTRY_PROTECTED_MODE_TRUE);

    replayable_fault_enabled = (READ_HWVALUE_MW(fault_entry, C369, BUF_ENTRY, REPLAYABLE_FAULT_EN) ==
                                NVC369_BUF_ENTRY_REPLAYABLE_FAULT_EN_TRUE);
    UVM_ASSERT_MSG(replayable_fault_enabled, "Fault with REPLAYABLE_FAULT_EN bit unset\n");
}

NV_STATUS uvm_hal_volta_fault_buffer_parse_replayable_entry(uvm_parent_gpu_t *parent_gpu,
                                                            NvU32 index,
                                                            uvm_fault_buffer_entry_t *buffer_entry)
{
    fault_buffer_entry_c369_t entry;
    NvU32 *fault_entry;

    BUILD_BUG_ON(sizeof(entry) > UVM_GPU_MMU_MAX_FAULT_PACKET_SIZE);

    // Valid bit must be set before this function is called
    UVM_ASSERT(parent_gpu->fault_buffer_hal->entry_is_valid(parent_gpu, index));

    fault_entry = get_fault_buffer_entry(parent_gpu, index);

    // When Confidential Computing is enabled, faults are encrypted by RM, so
    // they need to be decrypted before they can be parsed
    if (g_uvm_global.conf_computing_enabled) {
        NV_STATUS status;
        UvmFaultMetadataPacket *fault_entry_metadata = get_fault_buffer_entry_metadata(parent_gpu, index);

        status = uvm_conf_computing_fault_decrypt(parent_gpu,
                                                  &entry,
                                                  fault_entry,
                                                  fault_entry_metadata->authTag,
                                                  fault_entry_metadata->valid);
        if (status != NV_OK) {
            uvm_global_set_fatal_error(status);
            return status;
        }

        fault_entry = (NvU32 *) &entry;
    }

    parse_fault_entry_common(parent_gpu, fault_entry, buffer_entry);

    UVM_ASSERT(buffer_entry->is_replayable);

    // Automatically clear valid bit for the entry in the fault buffer
    parent_gpu->fault_buffer_hal->entry_clear_valid(parent_gpu, index);

    return NV_OK;
}

void uvm_hal_volta_fault_buffer_parse_non_replayable_entry(uvm_parent_gpu_t *parent_gpu,
                                                           void *fault_packet,
                                                           uvm_fault_buffer_entry_t *buffer_entry)
{
    parse_fault_entry_common(parent_gpu, fault_packet, buffer_entry);

    // No need to clear the valid bit since the fault buffer for non-replayable
    // faults is owned by RM and we are just parsing a copy of the packet
    UVM_ASSERT(!buffer_entry->is_replayable);
}
