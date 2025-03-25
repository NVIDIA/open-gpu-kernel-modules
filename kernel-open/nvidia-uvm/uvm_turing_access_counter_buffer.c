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
#include "clc365.h"

typedef struct {
    NvU8 bufferEntry[NVC365_NOTIFY_BUF_SIZE];
} access_counter_buffer_entry_c365_t;

void uvm_hal_turing_enable_access_counter_notifications(uvm_access_counter_buffer_t *access_counters)
{
    volatile NvU32 *reg;
    NvU32 mask;

    reg = access_counters->rm_info.pHubIntrEnSet;
    mask = access_counters->rm_info.accessCounterMask;

    UVM_GPU_WRITE_ONCE(*reg, mask);
}

static void clear_access_counter_notifications_interrupt(uvm_access_counter_buffer_t *access_counters)
{
    volatile NvU32 *reg;
    NvU32 mask;

    reg = access_counters->rm_info.pHubIntr;
    mask = access_counters->rm_info.accessCounterMask;

    UVM_GPU_WRITE_ONCE(*reg, mask);
}

void uvm_hal_turing_disable_access_counter_notifications(uvm_access_counter_buffer_t *access_counters)
{
    volatile NvU32 *reg;
    NvU32 mask;

    reg = access_counters->rm_info.pHubIntrEnClear;
    mask = access_counters->rm_info.accessCounterMask;

    UVM_GPU_WRITE_ONCE(*reg, mask);

    wmb();

    // See the comment in uvm_hal_turing_disable_replayable_faults
    clear_access_counter_notifications_interrupt(access_counters);
}

void uvm_hal_turing_clear_access_counter_notifications(uvm_access_counter_buffer_t *access_counters, NvU32 get)
{
    clear_access_counter_notifications_interrupt(access_counters);

    wmb();

    // Write GET to force the re-evaluation of the interrupt condition after the
    // interrupt bit has been cleared.
    UVM_GPU_WRITE_ONCE(*access_counters->rm_info.pAccessCntrBufferGet, get);
}

NvU32 uvm_hal_turing_access_counter_buffer_entry_size(uvm_parent_gpu_t *parent_gpu)
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

static NvU64 get_address(uvm_parent_gpu_t *parent_gpu, NvU32 *access_counter_entry)
{
    NvU64 address;
    NvU64 addr_hi = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, ADDR_HI);
    NvU64 addr_lo = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, ADDR_LO);
    NvU32 addr_type_value = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, ADDR_TYPE);

    UVM_ASSERT(addr_type_value == NVC365_NOTIFY_BUF_ENTRY_ADDR_TYPE_GVA);

    address = addr_lo + (addr_hi << HWSIZE_MW(C365, NOTIFY_BUF_ENTRY, ADDR_LO));
    address = uvm_parent_gpu_canonical_address(parent_gpu, address);

    return address;
}

static NvU32 *get_access_counter_buffer_entry(uvm_access_counter_buffer_t *access_counters, NvU32 index)
{
    access_counter_buffer_entry_c365_t *buffer_start;
    NvU32 *access_counter_entry;

    UVM_ASSERT(index < access_counters->max_notifications);

    buffer_start = (access_counter_buffer_entry_c365_t *)access_counters->rm_info.bufferAddress;
    access_counter_entry = (NvU32 *)&buffer_start[index];

    return access_counter_entry;
}

bool uvm_hal_turing_access_counter_buffer_entry_is_valid(uvm_access_counter_buffer_t *access_counters, NvU32 index)
{
    NvU32 *access_counter_entry;
    bool is_valid;

    access_counter_entry = get_access_counter_buffer_entry(access_counters, index);

    is_valid = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, VALID);

    return is_valid;
}

void uvm_hal_turing_access_counter_buffer_entry_clear_valid(uvm_access_counter_buffer_t *access_counters, NvU32 index)
{
    NvU32 *access_counter_entry;

    access_counter_entry = get_access_counter_buffer_entry(access_counters, index);

    WRITE_HWCONST_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, VALID, FALSE);
}

void uvm_hal_turing_access_counter_buffer_parse_entry(uvm_access_counter_buffer_t *access_counters,
                                                      NvU32 index,
                                                      uvm_access_counter_buffer_entry_t *buffer_entry)
{
    NvU32 *access_counter_entry;
    NvU64 inst_hi, inst_lo;

    // Valid bit must be set before this function is called
    UVM_ASSERT(uvm_hal_turing_access_counter_buffer_entry_is_valid(access_counters, index));

    access_counter_entry = get_access_counter_buffer_entry(access_counters, index);

    UVM_ASSERT(READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, TYPE) != NVC365_NOTIFY_BUF_ENTRY_TYPE_CPU);

    buffer_entry->address = get_address(access_counters->parent_gpu, access_counter_entry);

    inst_hi = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, INST_HI);
    inst_lo = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, INST_LO);
    buffer_entry->instance_ptr.address = inst_lo + (inst_hi << HWSIZE_MW(C365, NOTIFY_BUF_ENTRY, INST_LO));

    // HW value contains the 4K page number. Shift to build the full address
    buffer_entry->instance_ptr.address <<= 12;

    buffer_entry->instance_ptr.aperture = get_access_counter_inst_aperture(access_counter_entry);

    buffer_entry->mmu_engine_id = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, MMU_ENGINE_ID);

    buffer_entry->mmu_engine_type = UVM_MMU_ENGINE_TYPE_GRAPHICS;

    // MMU engine id aligns with the fault buffer packets. Therefore, we reuse
    // the helper to compute the VE ID from the fault buffer class.
    buffer_entry->ve_id = access_counters->parent_gpu->fault_buffer_hal->get_ve_id(buffer_entry->mmu_engine_id,
                                                                                   buffer_entry->mmu_engine_type);

    buffer_entry->counter_value = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, COUNTER_VAL);

    buffer_entry->sub_granularity = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, SUB_GRANULARITY);

    buffer_entry->bank = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, BANK);

    buffer_entry->tag = READ_HWVALUE_MW(access_counter_entry, C365, NOTIFY_BUF_ENTRY, NOTIFY_TAG);

    // Automatically clear valid bit for the entry in the access counter buffer
    uvm_hal_turing_access_counter_buffer_entry_clear_valid(access_counters, index);
}
