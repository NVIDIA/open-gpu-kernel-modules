/*******************************************************************************
    Copyright (c) 2023-2024 NVIDIA Corporation

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
#include "uvm_hal_types.h"
#include "hwref/blackwell/gb100/dev_fault.h"
#include "clc369.h"

// NV_PFAULT_FAULT_TYPE_COMPRESSION_FAILURE fault type is deprecated on
// Blackwell.
uvm_fault_type_t uvm_hal_blackwell_fault_buffer_get_fault_type(const NvU32 *fault_entry)
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
        case NV_PFAULT_FAULT_TYPE_CC_VIOLATION:
            return UVM_FAULT_TYPE_CC_VIOLATION;
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

static bool client_id_ce(NvU16 client_id)
{
    if (client_id >= NV_PFAULT_CLIENT_HUB_HSCE0 && client_id <= NV_PFAULT_CLIENT_HUB_HSCE7)
        return true;

    switch (client_id) {
        case NV_PFAULT_CLIENT_HUB_CE0:
        case NV_PFAULT_CLIENT_HUB_CE1:
        case NV_PFAULT_CLIENT_HUB_CE2:
        case NV_PFAULT_CLIENT_HUB_CE3:
            return true;
    }

    return false;
}

uvm_mmu_engine_type_t uvm_hal_blackwell_fault_buffer_get_mmu_engine_type(NvU16 mmu_engine_id,
                                                                         uvm_fault_client_type_t client_type,
                                                                         NvU16 client_id)
{
    // Servicing CE and Host (HUB clients) faults.
    if (client_type == UVM_FAULT_CLIENT_TYPE_HUB) {
        if (client_id_ce(client_id)) {
            UVM_ASSERT(mmu_engine_id >= NV_PFAULT_MMU_ENG_ID_CE0 && mmu_engine_id <= NV_PFAULT_MMU_ENG_ID_CE19);

            return UVM_MMU_ENGINE_TYPE_CE;
        }

        if (client_id == NV_PFAULT_CLIENT_HUB_HOST ||
            (client_id >= NV_PFAULT_CLIENT_HUB_ESC0 && client_id <= NV_PFAULT_CLIENT_HUB_ESC11)) {
            UVM_ASSERT((mmu_engine_id >= NV_PFAULT_MMU_ENG_ID_HOST0 && mmu_engine_id <= NV_PFAULT_MMU_ENG_ID_HOST44) ||
                       (mmu_engine_id >= NV_PFAULT_MMU_ENG_ID_GRAPHICS));

            return UVM_MMU_ENGINE_TYPE_HOST;
        }
    }

    // We shouldn't be servicing faults from any other engines other than GR.
    UVM_ASSERT_MSG(client_id <= NV_PFAULT_CLIENT_GPC_ROP_3, "Unexpected client ID: 0x%x\n", client_id);
    UVM_ASSERT_MSG(mmu_engine_id >= NV_PFAULT_MMU_ENG_ID_GRAPHICS, "Unexpected engine ID: 0x%x\n", mmu_engine_id);
    UVM_ASSERT(client_type == UVM_FAULT_CLIENT_TYPE_GPC);

    return UVM_MMU_ENGINE_TYPE_GRAPHICS;
}
