/*******************************************************************************
    Copyright (c) 2020 NVIDIA Corporation

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
