/*******************************************************************************
    Copyright (c) 2017-2021 NVIDIA Corporation

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
#include "uvm_push.h"
#include "uvm_user_channel.h"
#include "clc46f.h"

void uvm_hal_turing_host_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    NvU32 sem_lo;
    UVM_ASSERT(!(NvOffset_LO32(gpu_va) & ~HWSHIFTMASK(C46F, SEM_ADDR_LO, OFFSET)));
    sem_lo = READ_HWVALUE(NvOffset_LO32(gpu_va), C46F, SEM_ADDR_LO, OFFSET);

    uvm_hal_wfi_membar(push, uvm_push_get_and_reset_membar_flag(push));

    NV_PUSH_5U(C46F, SEM_ADDR_LO,    HWVALUE(C46F, SEM_ADDR_LO, OFFSET, sem_lo),
                     SEM_ADDR_HI,    HWVALUE(C46F, SEM_ADDR_HI, OFFSET, NvOffset_HI32(gpu_va)),
                     SEM_PAYLOAD_LO, payload,
                     SEM_PAYLOAD_HI, 0,
                     SEM_EXECUTE,    HWCONST(C46F, SEM_EXECUTE, OPERATION, RELEASE) |
                                     HWCONST(C46F, SEM_EXECUTE, PAYLOAD_SIZE, 32BIT) |
                                     HWCONST(C46F, SEM_EXECUTE, RELEASE_TIMESTAMP, DIS) |
                                     HWCONST(C46F, SEM_EXECUTE, RELEASE_WFI, DIS));
}

void uvm_hal_turing_host_semaphore_acquire(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    NvU32 sem_lo;
    UVM_ASSERT(!(NvOffset_LO32(gpu_va) & ~HWSHIFTMASK(C46F, SEM_ADDR_LO, OFFSET)));
    sem_lo = READ_HWVALUE(NvOffset_LO32(gpu_va), C46F, SEM_ADDR_LO, OFFSET);
    NV_PUSH_5U(C46F, SEM_ADDR_LO,    HWVALUE(C46F, SEM_ADDR_LO, OFFSET, sem_lo),
                     SEM_ADDR_HI,    HWVALUE(C46F, SEM_ADDR_HI, OFFSET, NvOffset_HI32(gpu_va)),
                     SEM_PAYLOAD_LO, payload,
                     SEM_PAYLOAD_HI, 0,
                     SEM_EXECUTE,    HWCONST(C46F, SEM_EXECUTE, OPERATION, ACQ_CIRC_GEQ) |
                                     HWCONST(C46F, SEM_EXECUTE, PAYLOAD_SIZE, 32BIT) |
                                     HWCONST(C46F, SEM_EXECUTE, ACQUIRE_SWITCH_TSG, EN));
}

void uvm_hal_turing_host_clear_faulted_channel_method(uvm_push_t *push,
                                                      uvm_user_channel_t *user_channel,
                                                      const uvm_fault_buffer_entry_t *fault)
{
    NvU32 clear_type_value = 0;

    UVM_ASSERT(user_channel->gpu->parent->has_clear_faulted_channel_method);

    if (fault->fault_source.mmu_engine_type == UVM_MMU_ENGINE_TYPE_HOST) {
        clear_type_value = HWCONST(C46F, CLEAR_FAULTED, TYPE, PBDMA_FAULTED);
    }
    else if (fault->fault_source.mmu_engine_type == UVM_MMU_ENGINE_TYPE_CE) {
        clear_type_value = HWCONST(C46F, CLEAR_FAULTED, TYPE, ENG_FAULTED);
    }
    else {
        UVM_ASSERT_MSG(false, "Unsupported MMU engine type %s\n",
                       uvm_mmu_engine_type_string(fault->fault_source.mmu_engine_type));
    }

    NV_PUSH_1U(C46F, CLEAR_FAULTED, HWVALUE(C46F, CLEAR_FAULTED, HANDLE, user_channel->clear_faulted_token) |
                                    clear_type_value);
}

// Direct copy of uvm_hal_maxwell_host_set_gpfifo_entry(). It removes
// GP_ENTRY1_PRIV_KERNEL, which has been deprecated in Turing+.
void uvm_hal_turing_host_set_gpfifo_entry(NvU64 *fifo_entry,
                                          NvU64 pushbuffer_va,
                                          NvU32 pushbuffer_length,
                                          uvm_gpfifo_sync_t sync_flag)
{
    NvU64 fifo_entry_value;
    const NvU32 sync_value = (sync_flag == UVM_GPFIFO_SYNC_WAIT) ? HWCONST(C46F, GP_ENTRY1, SYNC, WAIT) :
                                                                   HWCONST(C46F, GP_ENTRY1, SYNC, PROCEED);

    UVM_ASSERT(!uvm_global_is_suspended());
    UVM_ASSERT_MSG(pushbuffer_va % 4 == 0, "pushbuffer va unaligned: %llu\n", pushbuffer_va);
    UVM_ASSERT_MSG(pushbuffer_length % 4 == 0, "pushbuffer length unaligned: %u\n", pushbuffer_length);

    fifo_entry_value =          HWVALUE(C46F, GP_ENTRY0, GET, NvU64_LO32(pushbuffer_va) >> 2);
    fifo_entry_value |= (NvU64)(HWVALUE(C46F, GP_ENTRY1, GET_HI, NvU64_HI32(pushbuffer_va)) |
                                HWVALUE(C46F, GP_ENTRY1, LENGTH, pushbuffer_length >> 2) |
                                sync_value) << 32;

    *fifo_entry = fifo_entry_value;
}

