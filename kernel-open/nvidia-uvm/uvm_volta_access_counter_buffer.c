/*******************************************************************************
    Copyright (c) 2016-2021 NVIDIA Corporation

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
#include "clc365.h"
#include "uvm_volta_fault_buffer.h"

typedef struct {
    NvU8 bufferEntry[NVC365_NOTIFY_BUF_SIZE];
} access_counter_buffer_entry_c365_t;

void uvm_hal_volta_enable_access_counter_notifications(uvm_parent_gpu_t *parent_gpu)
{
    volatile NvU32 *reg;
    NvU32 mask;

    reg = parent_gpu->access_counter_buffer_info.rm_info.pHubIntrEnSet;
    mask = parent_gpu->access_counter_buffer_info.rm_info.accessCounterMask;

    UVM_GPU_WRITE_ONCE(*reg, mask);
}

void uvm_hal_volta_disable_access_counter_notifications(uvm_parent_gpu_t *parent_gpu)
{
    volatile NvU32 *reg;
    NvU32 mask;

    reg = parent_gpu->access_counter_buffer_info.rm_info.pHubIntrEnClear;
    mask = parent_gpu->access_counter_buffer_info.rm_info.accessCounterMask;

    UVM_GPU_WRITE_ONCE(*reg, mask);
}

void uvm_hal_volta_clear_access_counter_notifications(uvm_parent_gpu_t *parent_gpu, NvU32 get)
{
    // No-op, this function is only used by pulse-based interrupt GPUs.
}

NvU32 uvm_hal_volta_access_counter_buffer_entry_size(uvm_parent_gpu_t *parent_gpu)
{
    return NVC365_NOTIFY_BUF_SIZE;
}

static uvm_aperture_t get_access_counter_inst_aperture(NvU32 *access_counter_entry)
{
    NvU32 hw_aperture_value = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, INST_APERTURE);

    switch (hw_aperture_value) {
        case NVC365_NOTIFY_BUF_ENTRY_APERTURE_VID_MEM:
            return UVM_APERTURE_VID;
        case NVC365_NOTIFY_BUF_ENTRY_APERTURE_SYS_MEM_COHERENT:
        case NVC365_NOTIFY_BUF_ENTRY_APERTURE_SYS_MEM_NONCOHERENT:
             return UVM_APERTURE_SYS;
    }

    UVM_ASSERT_MSG(false, "Invalid inst aperture value: %d\n", hw_aperture_value);
    return UVM_APERTURE_MAX;
}

static uvm_aperture_t get_access_counter_aperture(NvU32 *access_counter_entry)
{
    NvU32 hw_aperture_value = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, APERTURE);
    NvU32 peer_id = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, PEER_ID);

    switch (hw_aperture_value) {
        case NVC365_NOTIFY_BUF_ENTRY_APERTURE_VID_MEM:
            return UVM_APERTURE_VID;
        case NVC365_NOTIFY_BUF_ENTRY_APERTURE_PEER_MEM:
            return UVM_APERTURE_PEER(peer_id);
        case NVC365_NOTIFY_BUF_ENTRY_APERTURE_SYS_MEM_COHERENT:
        case NVC365_NOTIFY_BUF_ENTRY_APERTURE_SYS_MEM_NONCOHERENT:
             return UVM_APERTURE_SYS;
    }

    UVM_ASSERT_MSG(false, "Invalid aperture value: %d\n", hw_aperture_value);
    return UVM_APERTURE_MAX;
}

static uvm_gpu_address_t get_address(uvm_parent_gpu_t *parent_gpu, NvU32 *access_counter_entry)
{
    NvU64 address;
    bool is_virtual;
    NvU64 addr_hi = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, ADDR_HI);
    NvU64 addr_lo = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, ADDR_LO);
    NvU32 addr_type_value = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, ADDR_TYPE);

    address = addr_lo + (addr_hi << HWSIZE_MW(C365, NOTIFY_BUF_ENTRY, ADDR_LO));
    is_virtual = (addr_type_value == NVC365_NOTIFY_BUF_ENTRY_ADDR_TYPE_GVA);

    if (is_virtual) {
        address = uvm_parent_gpu_canonical_address(parent_gpu, address);
        return uvm_gpu_address_virtual(address);
    }
    else {
        uvm_aperture_t aperture = get_access_counter_aperture(access_counter_entry);

        UVM_ASSERT(parent_gpu->access_counters_can_use_physical_addresses);
        UVM_ASSERT_MSG(addr_type_value == NVC365_NOTIFY_BUF_ENTRY_ADDR_TYPE_GPA,
                       "Invalid address type%u\n", addr_type_value);

        return uvm_gpu_address_physical(aperture, address);
    }
}

static uvm_access_counter_type_t get_access_counter_type(NvU32 *access_counter_entry)
{
    NvU32 type_value = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, TYPE);
    if (type_value == NVC365_NOTIFY_BUF_ENTRY_TYPE_CPU)
        return UVM_ACCESS_COUNTER_TYPE_MOMC;
    else
        return UVM_ACCESS_COUNTER_TYPE_MIMC;
}

static NvU32 *get_access_counter_buffer_entry(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    access_counter_buffer_entry_c365_t *buffer_start;
    NvU32 *access_counter_entry;

    UVM_ASSERT(index < parent_gpu->access_counter_buffer_info.max_notifications);

    buffer_start = (access_counter_buffer_entry_c365_t *)parent_gpu->access_counter_buffer_info.rm_info.bufferAddress;
    access_counter_entry = (NvU32 *)&buffer_start[index];

    return access_counter_entry;
}

bool uvm_hal_volta_access_counter_buffer_entry_is_valid(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    NvU32 *access_counter_entry;
    bool is_valid;

    access_counter_entry = get_access_counter_buffer_entry(parent_gpu, index);

    is_valid = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, VALID);

    return is_valid;
}

void uvm_hal_volta_access_counter_buffer_entry_clear_valid(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    NvU32 *access_counter_entry;

    access_counter_entry = get_access_counter_buffer_entry(parent_gpu, index);

    WRITE_HWCONST_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, VALID, FALSE);
}

void uvm_hal_volta_access_counter_buffer_parse_entry(uvm_parent_gpu_t *parent_gpu,
                                                     NvU32 index,
                                                     uvm_access_counter_buffer_entry_t *buffer_entry)
{
    NvU32 *access_counter_entry;

    // Valid bit must be set before this function is called
    UVM_ASSERT(uvm_hal_volta_access_counter_buffer_entry_is_valid(parent_gpu, index));

    access_counter_entry = get_access_counter_buffer_entry(parent_gpu, index);

    buffer_entry->counter_type = get_access_counter_type(access_counter_entry);

    buffer_entry->address = get_address(parent_gpu, access_counter_entry);

    if (buffer_entry->address.is_virtual) {
        NvU64 inst_hi, inst_lo;

        inst_hi = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, INST_HI);
        inst_lo = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, INST_LO);
        buffer_entry->virtual_info.instance_ptr.address =
            inst_lo + (inst_hi << HWSIZE_MW(C365, NOTIFY_BUF_ENTRY, INST_LO));

        // HW value contains the 4K page number. Shift to build the full address
        buffer_entry->virtual_info.instance_ptr.address <<= 12;

        buffer_entry->virtual_info.instance_ptr.aperture = get_access_counter_inst_aperture(access_counter_entry);

        buffer_entry->virtual_info.mmu_engine_id =
            READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, MMU_ENGINE_ID);

        buffer_entry->virtual_info.mmu_engine_type = UVM_MMU_ENGINE_TYPE_GRAPHICS;

        // MMU engine id aligns with the fault buffer packets. Therefore, we
        // reuse the helper to compute the VE ID from the fault buffer class.
        buffer_entry->virtual_info.ve_id =
            parent_gpu->fault_buffer_hal->get_ve_id(buffer_entry->virtual_info.mmu_engine_id,
                                                    buffer_entry->virtual_info.mmu_engine_type);
    }
    else if (buffer_entry->counter_type == UVM_ACCESS_COUNTER_TYPE_MIMC) {
        // Ignore any set bit beyond 47 since it is the maximum physical address
        // supported by the GPU. See the definition of
        // uvm_gpu_t::dma_addressable_start for why higher bits might be set.
        const NvU64 mask_46_0 = (0x1UL << 47) - 1;
        buffer_entry->address.address &= mask_46_0;
    }

    buffer_entry->counter_value = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, COUNTER_VAL);

    buffer_entry->sub_granularity = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, SUB_GRANULARITY);

    buffer_entry->bank = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, BANK);

    buffer_entry->tag = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, NOTIFY_TAG);

    // Automatically clear valid bit for the entry in the access counter buffer
    uvm_hal_volta_access_counter_buffer_entry_clear_valid(parent_gpu, index);
}
