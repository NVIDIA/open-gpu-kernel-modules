/*******************************************************************************
    Copyright (c) 2020-2024 NVIDIA Corporation

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

#include "uvm_hal.h"
#include "uvm_hal_types.h"
#include "hwref/hopper/gh100/dev_fault.h"

// TODO: Bug  1835884: [uvm] Query the maximum number of subcontexts from RM
// ... to validate the ve_id
#define MAX_SUBCONTEXTS 64
NvU8 uvm_hal_hopper_fault_buffer_get_ve_id(NvU16 mmu_engine_id, uvm_mmu_engine_type_t mmu_engine_type)
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

static bool client_id_ce(NvU16 client_id)
{
    if (client_id >= NV_PFAULT_CLIENT_HUB_HSCE0 && client_id <= NV_PFAULT_CLIENT_HUB_HSCE9)
        return true;

    if (client_id >= NV_PFAULT_CLIENT_HUB_HSCE10 && client_id <= NV_PFAULT_CLIENT_HUB_HSCE15)
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

uvm_mmu_engine_type_t uvm_hal_hopper_fault_buffer_get_mmu_engine_type(NvU16 mmu_engine_id,
                                                                      uvm_fault_client_type_t client_type,
                                                                      NvU16 client_id)
{
    // Servicing CE and Host (HUB clients) faults.
    if (client_type == UVM_FAULT_CLIENT_TYPE_HUB) {
        if (client_id_ce(client_id)) {
            UVM_ASSERT(mmu_engine_id >= NV_PFAULT_MMU_ENG_ID_CE0 && mmu_engine_id <= NV_PFAULT_MMU_ENG_ID_CE9);

            return UVM_MMU_ENGINE_TYPE_CE;
        }

        if (client_id == NV_PFAULT_CLIENT_HUB_HOST || client_id == NV_PFAULT_CLIENT_HUB_ESC) {
            UVM_ASSERT(mmu_engine_id >= NV_PFAULT_MMU_ENG_ID_HOST0 && mmu_engine_id <= NV_PFAULT_MMU_ENG_ID_HOST44);

            return UVM_MMU_ENGINE_TYPE_HOST;
        }
    }

    // We shouldn't be servicing faults from any other engines other than GR.
    UVM_ASSERT_MSG(client_id <= NV_PFAULT_CLIENT_GPC_ROP_3, "Unexpected client ID: 0x%x\n", client_id);
    UVM_ASSERT_MSG(mmu_engine_id >= NV_PFAULT_MMU_ENG_ID_GRAPHICS, "Unexpected engine ID: 0x%x\n", mmu_engine_id);
    UVM_ASSERT(client_type == UVM_FAULT_CLIENT_TYPE_GPC);

    return UVM_MMU_ENGINE_TYPE_GRAPHICS;
}
