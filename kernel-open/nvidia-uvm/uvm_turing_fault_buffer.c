/*******************************************************************************
    Copyright (c) 2021-2025 NVIDIA Corporation

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
#include "hwref/turing/tu102/dev_fault.h"

static void clear_replayable_faults_interrupt(uvm_parent_gpu_t *parent_gpu)
{
    volatile NvU32 *reg;
    NvU32 mask;

    reg = parent_gpu->fault_buffer.rm_info.replayable.pPmcIntr;
    mask = parent_gpu->fault_buffer.rm_info.replayable.replayableFaultMask;

    UVM_GPU_WRITE_ONCE(*reg, mask);
}

void uvm_hal_turing_clear_replayable_faults(uvm_parent_gpu_t *parent_gpu, NvU32 get)
{
    clear_replayable_faults_interrupt(parent_gpu);

    wmb();

    // Write GET to force the re-evaluation of the interrupt condition after the
    // interrupt bit has been cleared.
    parent_gpu->fault_buffer_hal->write_get(parent_gpu, get);
}

void uvm_hal_turing_disable_replayable_faults(uvm_parent_gpu_t *parent_gpu)
{
    volatile NvU32 *reg;
    NvU32 mask;

    reg = parent_gpu->fault_buffer.rm_info.replayable.pPmcIntrEnClear;
    mask = parent_gpu->fault_buffer.rm_info.replayable.replayableFaultMask;

    UVM_GPU_WRITE_ONCE(*reg, mask);

    wmb();

    // We clear the interrupts right after disabling them in order to avoid
    // triggering unnecessary new interrupts after re-enabling them if the
    // interrupt condition is no longer true.
    clear_replayable_faults_interrupt(parent_gpu);
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

uvm_mmu_engine_type_t uvm_hal_turing_fault_buffer_get_mmu_engine_type(NvU16 mmu_engine_id,
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
            UVM_ASSERT(mmu_engine_id >= NV_PFAULT_MMU_ENG_ID_HOST0 && mmu_engine_id <= NV_PFAULT_MMU_ENG_ID_HOST14);

            return UVM_MMU_ENGINE_TYPE_HOST;
        }
    }

    // We shouldn't be servicing faults from any other engines other than GR.
    UVM_ASSERT_MSG(client_id <= NV_PFAULT_CLIENT_GPC_T1_39, "Unexpected client ID: 0x%x\n", client_id);
    UVM_ASSERT_MSG(mmu_engine_id >= NV_PFAULT_MMU_ENG_ID_GRAPHICS && mmu_engine_id < NV_PFAULT_MMU_ENG_ID_BAR1,
                   "Unexpected engine ID: 0x%x\n",
                   mmu_engine_id);
    UVM_ASSERT(client_type == UVM_FAULT_CLIENT_TYPE_GPC);

    return UVM_MMU_ENGINE_TYPE_GRAPHICS;
}
