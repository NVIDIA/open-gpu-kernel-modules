/*******************************************************************************
    Copyright (c) 2021-2022 NVIDIA Corporation

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
#include "uvm_hal_types.h"
#include "uvm_hal.h"
#include "uvm_push.h"
#include "cla16f.h"
#include "clb06f.h"

void uvm_hal_maxwell_host_wait_for_idle(uvm_push_t *push)
{
    NV_PUSH_1U(A16F, WFI, 0);
}

void uvm_hal_maxwell_host_membar_sys(uvm_push_t *push)
{
    NV_PUSH_1U(A16F, MEM_OP_B,
       HWCONST(A16F, MEM_OP_B, OPERATION, SYSMEMBAR_FLUSH));
}

void uvm_hal_maxwell_host_tlb_invalidate_all_a16f(uvm_push_t *push,
                                                  uvm_gpu_phys_address_t pdb,
                                                  NvU32 depth,
                                                  uvm_membar_t membar)
{
    NvU32 target;

    UVM_ASSERT_MSG(pdb.aperture == UVM_APERTURE_VID || pdb.aperture == UVM_APERTURE_SYS, "aperture: %u", pdb.aperture);

    // Only Pascal+ supports invalidating down from a specific depth.
    (void)depth;

    (void)membar;

    if (pdb.aperture == UVM_APERTURE_VID)
        target = HWCONST(A16F, MEM_OP_A, TLB_INVALIDATE_TARGET, VID_MEM);
    else
        target = HWCONST(A16F, MEM_OP_A, TLB_INVALIDATE_TARGET, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    NV_PUSH_2U(A16F, MEM_OP_A, target |
                               HWVALUE(A16F, MEM_OP_A, TLB_INVALIDATE_ADDR, pdb.address),
                     MEM_OP_B, HWCONST(A16F, MEM_OP_B, OPERATION, MMU_TLB_INVALIDATE) |
                               HWCONST(A16F, MEM_OP_B, MMU_TLB_INVALIDATE_PDB, ONE) |
                               HWCONST(A16F, MEM_OP_B, MMU_TLB_INVALIDATE_GPC, ENABLE));
}

void uvm_hal_maxwell_host_tlb_invalidate_all_b06f(uvm_push_t *push,
                                                  uvm_gpu_phys_address_t pdb,
                                                  NvU32 depth,
                                                  uvm_membar_t membar)
{
    NvU32 target;
    NvU32 pdb_lo;
    NvU32 pdb_hi;

    UVM_ASSERT_MSG(pdb.aperture == UVM_APERTURE_VID || pdb.aperture == UVM_APERTURE_SYS, "aperture: %u", pdb.aperture);

    // Only Pascal+ supports invalidating down from a specific depth.
    (void)depth;

    (void)membar;

    if (pdb.aperture == UVM_APERTURE_VID)
        target = HWCONST(B06F, MEM_OP_C, TLB_INVALIDATE_TARGET, VID_MEM);
    else
        target = HWCONST(B06F, MEM_OP_C, TLB_INVALIDATE_TARGET, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;
    pdb_lo = pdb.address & HWMASK(B06F, MEM_OP_C, TLB_INVALIDATE_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(B06F, MEM_OP_C, TLB_INVALIDATE_ADDR_LO);

    NV_PUSH_2U(B06F, MEM_OP_C, target |
                               HWCONST(B06F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWCONST(B06F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWVALUE(B06F, MEM_OP_C, TLB_INVALIDATE_ADDR_LO, pdb_lo),
                     MEM_OP_D, HWCONST(B06F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE) |
                               HWVALUE(B06F, MEM_OP_D, TLB_INVALIDATE_ADDR_HI, pdb_hi));
}

void uvm_hal_maxwell_host_tlb_invalidate_va(uvm_push_t *push,
                                            uvm_gpu_phys_address_t pdb,
                                            NvU32 depth,
                                            NvU64 base,
                                            NvU64 size,
                                            NvU64 page_size,
                                            uvm_membar_t membar)
{
    // No per VA invalidate on Maxwell, redirect to invalidate all.
    uvm_push_get_gpu(push)->parent->host_hal->tlb_invalidate_all(push, pdb, depth, membar);
}

void uvm_hal_maxwell_host_tlb_invalidate_test(uvm_push_t *push,
                                              uvm_gpu_phys_address_t pdb,
                                              UVM_TEST_INVALIDATE_TLB_PARAMS *params)
{
    NvU32 target_pdb = 0;
    NvU32 invalidate_gpc_value;

    // Only Pascal+ supports invalidating down from a specific depth. We
    // invalidate all
    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    UVM_ASSERT_MSG(pdb.aperture == UVM_APERTURE_VID || pdb.aperture == UVM_APERTURE_SYS, "aperture: %u", pdb.aperture);
    if (pdb.aperture == UVM_APERTURE_VID)
        target_pdb = HWCONST(A16F, MEM_OP_A, TLB_INVALIDATE_TARGET, VID_MEM);
    else
        target_pdb = HWCONST(A16F, MEM_OP_A, TLB_INVALIDATE_TARGET, SYS_MEM_COHERENT);
    target_pdb |= HWVALUE(A16F, MEM_OP_A, TLB_INVALIDATE_ADDR, pdb.address);

    if (params->disable_gpc_invalidate)
        invalidate_gpc_value = HWCONST(A16F, MEM_OP_B, MMU_TLB_INVALIDATE_GPC, DISABLE);
    else
        invalidate_gpc_value = HWCONST(A16F, MEM_OP_B, MMU_TLB_INVALIDATE_GPC, ENABLE);

    NV_PUSH_2U(A16F, MEM_OP_A, target_pdb,
                     MEM_OP_B, HWCONST(A16F, MEM_OP_B, OPERATION, MMU_TLB_INVALIDATE) |
                               HWCONST(A16F, MEM_OP_B, MMU_TLB_INVALIDATE_PDB, ONE) |
                               invalidate_gpc_value);
}

void uvm_hal_maxwell_host_noop(uvm_push_t *push, NvU32 size)
{
    UVM_ASSERT_MSG(size % 4 == 0, "size %u\n", size);

    if (size == 0)
        return;

    // size is in bytes so divide by the method size (4 bytes)
    size /= 4;

    while (size > 0) {
        // noop_this_time includes the NOP method itself and hence can be
        // up to COUNT_MAX + 1.
        NvU32 noop_this_time = min(UVM_METHOD_COUNT_MAX + 1, size);

        // -1 for the NOP method itself.
        NV_PUSH_NU_NONINC(A16F, NOP, noop_this_time - 1);

        size -= noop_this_time;
    }
}

void uvm_hal_maxwell_host_interrupt(uvm_push_t *push)
{
    NV_PUSH_1U(A16F, NON_STALL_INTERRUPT, 0);
}

void uvm_hal_maxwell_host_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    NvU32 sem_lo;
    UVM_ASSERT(!(NvOffset_LO32(gpu_va) & ~HWSHIFTMASK(A16F, SEMAPHOREB, OFFSET_LOWER)));
    sem_lo = READ_HWVALUE(NvOffset_LO32(gpu_va), A16F, SEMAPHOREB, OFFSET_LOWER);

    uvm_hal_wfi_membar(push, uvm_push_get_and_reset_membar_flag(push));

    NV_PUSH_4U(A16F, SEMAPHOREA, HWVALUE(A16F, SEMAPHOREA, OFFSET_UPPER, NvOffset_HI32(gpu_va)),
                     SEMAPHOREB, HWVALUE(A16F, SEMAPHOREB, OFFSET_LOWER, sem_lo),
                     SEMAPHOREC, payload,
                     SEMAPHORED, HWCONST(A16F, SEMAPHORED, OPERATION, RELEASE) |
                                 HWCONST(A16F, SEMAPHORED, RELEASE_SIZE, 4BYTE)|
                                 HWCONST(A16F, SEMAPHORED, RELEASE_WFI, DIS));
}

void uvm_hal_maxwell_host_semaphore_acquire(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    NvU32 sem_lo;
    UVM_ASSERT(!(NvOffset_LO32(gpu_va) & ~HWSHIFTMASK(A16F, SEMAPHOREB, OFFSET_LOWER)));
    sem_lo = READ_HWVALUE(NvOffset_LO32(gpu_va), A16F, SEMAPHOREB, OFFSET_LOWER);
    NV_PUSH_4U(A16F, SEMAPHOREA, HWVALUE(A16F, SEMAPHOREA, OFFSET_UPPER, NvOffset_HI32(gpu_va)),
                     SEMAPHOREB, HWVALUE(A16F, SEMAPHOREB, OFFSET_LOWER, sem_lo),
                     SEMAPHOREC, payload,
                     SEMAPHORED, HWCONST(A16F, SEMAPHORED, ACQUIRE_SWITCH, ENABLED) |
                                 HWCONST(A16F, SEMAPHORED, OPERATION, ACQ_GEQ));

}

void uvm_hal_maxwell_host_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va)
{
    NvU32 sem_lo;
    UVM_ASSERT(!(NvOffset_LO32(gpu_va) & ~HWSHIFTMASK(A16F, SEMAPHOREB, OFFSET_LOWER)));
    sem_lo = READ_HWVALUE(NvOffset_LO32(gpu_va), A16F, SEMAPHOREB, OFFSET_LOWER);

    uvm_hal_wfi_membar(push, uvm_push_get_and_reset_membar_flag(push));

    NV_PUSH_4U(A16F, SEMAPHOREA, HWVALUE(A16F, SEMAPHOREA, OFFSET_UPPER, NvOffset_HI32(gpu_va)),
                     SEMAPHOREB, HWVALUE(A16F, SEMAPHOREB, OFFSET_LOWER, sem_lo),
                     SEMAPHOREC, 0xdeadbeef,
                     SEMAPHORED, HWCONST(A16F, SEMAPHORED, OPERATION, RELEASE) |
                                 HWCONST(A16F, SEMAPHORED, RELEASE_SIZE, 16BYTE)|
                                 HWCONST(A16F, SEMAPHORED, RELEASE_WFI, DIS));
}

void uvm_hal_maxwell_host_set_gpfifo_entry(NvU64 *fifo_entry,
                                           NvU64 pushbuffer_va,
                                           NvU32 pushbuffer_length,
                                           uvm_gpfifo_sync_t sync_flag)
{
    NvU64 fifo_entry_value;
    const NvU32 sync_value = (sync_flag == UVM_GPFIFO_SYNC_WAIT) ? HWCONST(A16F, GP_ENTRY1, SYNC, WAIT) :
                                                                   HWCONST(A16F, GP_ENTRY1, SYNC, PROCEED);

    UVM_ASSERT(!uvm_global_is_suspended());
    UVM_ASSERT_MSG(pushbuffer_va % 4 == 0, "pushbuffer va unaligned: %llu\n", pushbuffer_va);
    UVM_ASSERT_MSG(pushbuffer_length % 4 == 0, "pushbuffer length unaligned: %u\n", pushbuffer_length);

    fifo_entry_value =          HWVALUE(A16F, GP_ENTRY0, GET, NvU64_LO32(pushbuffer_va) >> 2);
    fifo_entry_value |= (NvU64)(HWVALUE(A16F, GP_ENTRY1, GET_HI, NvU64_HI32(pushbuffer_va)) |
                                HWVALUE(A16F, GP_ENTRY1, LENGTH, pushbuffer_length >> 2) |
                                HWCONST(A16F, GP_ENTRY1, PRIV,   KERNEL) |
                                sync_value) << 32;

    *fifo_entry = fifo_entry_value;
}

void uvm_hal_maxwell_host_set_gpfifo_noop(NvU64 *fifo_entry)
{
    UVM_ASSERT(!uvm_global_is_suspended());

    // A NOOP control GPFIFO does not require a GP_ENTRY0.
    *fifo_entry = (NvU64)(HWVALUE(A16F, GP_ENTRY1, LENGTH, 0) | HWCONST(A16F, GP_ENTRY1, OPCODE, NOP)) << 32;
}

void uvm_hal_maxwell_host_set_gpfifo_pushbuffer_segment_base_unsupported(NvU64 *fifo_entry, NvU64 pushbuffer_va)
{
    UVM_ASSERT_MSG(false, "host set_gpfifo_pushbuffer_segment_base called on Maxwell GPU\n");
}

void uvm_hal_maxwell_host_write_gpu_put(uvm_channel_t *channel, NvU32 gpu_put)
{
    UVM_GPU_WRITE_ONCE(*channel->channel_info.gpPut, gpu_put);
}

void uvm_hal_maxwell_host_init_noop(uvm_push_t *push)
{
}

void uvm_hal_maxwell_replay_faults_unsupported(uvm_push_t *push, uvm_fault_replay_type_t type)
{
    UVM_ASSERT_MSG(false, "host replay_faults called on Maxwell GPU\n");
}

void uvm_hal_maxwell_cancel_faults_global_unsupported(uvm_push_t *push, uvm_gpu_phys_address_t instance_ptr)
{
    UVM_ASSERT_MSG(false, "host cancel_faults_global called on Maxwell GPU\n");
}

void uvm_hal_maxwell_cancel_faults_targeted_unsupported(uvm_push_t *push,
                                                       uvm_gpu_phys_address_t instance_ptr,
                                                       NvU32 gpc_id,
                                                       NvU32 client_id)
{
    UVM_ASSERT_MSG(false, "host cancel_faults_targeted called on Maxwell GPU\n");
}

void uvm_hal_maxwell_cancel_faults_va_unsupported(uvm_push_t *push,
                                                  uvm_gpu_phys_address_t pdb,
                                                  const uvm_fault_buffer_entry_t *fault_entry,
                                                  uvm_fault_cancel_va_mode_t cancel_va_mode)
{
    UVM_ASSERT_MSG(false, "host cancel_faults_va called on Maxwell GPU\n");
}

void uvm_hal_maxwell_host_clear_faulted_channel_sw_method_unsupported(uvm_push_t *push,
                                                                      uvm_user_channel_t *user_channel,
                                                                      const uvm_fault_buffer_entry_t *buffer_entry)
{
    UVM_ASSERT_MSG(false, "host clear_faulted_channel_sw_method called on Maxwell GPU\n");
}

void uvm_hal_maxwell_host_clear_faulted_channel_method_unsupported(uvm_push_t *push,
                                                                   uvm_user_channel_t *user_channel,
                                                                   const uvm_fault_buffer_entry_t *buffer_entry)
{
    UVM_ASSERT_MSG(false, "host clear_faulted_channel_method called on Maxwell GPU\n");
}

void uvm_hal_maxwell_host_clear_faulted_channel_register_unsupported(uvm_user_channel_t *user_channel,
                                                                     const uvm_fault_buffer_entry_t *buffer_entry)
{
    UVM_ASSERT_MSG(false, "host clear_faulted_channel_register called on Maxwell GPU\n");
}

void uvm_hal_maxwell_access_counter_clear_all_unsupported(uvm_push_t *push)
{
    UVM_ASSERT_MSG(false, "host access_counter_clear_all called on Maxwell GPU\n");
}

void uvm_hal_maxwell_access_counter_clear_type_unsupported(uvm_push_t *push, uvm_access_counter_type_t type)
{
    UVM_ASSERT_MSG(false, "host access_counter_clear_type called on Maxwell GPU\n");
}

void uvm_hal_maxwell_access_counter_clear_targeted_unsupported(uvm_push_t *push,
                                                               const uvm_access_counter_buffer_entry_t *buffer_entry)
{
    UVM_ASSERT_MSG(false, "host access_counter_clear_targeted called on Maxwell GPU\n");
}

NvU64 uvm_hal_maxwell_get_time(uvm_gpu_t *gpu)
{
    NvU32 time0;
    NvU32 time1_first, time1_second;

    // When reading the TIME, TIME_1 should be read first, followed by TIME_0,
    // then a second reading of TIME_1 should be done. If the two readings of
    // do not match, this process should be repeated.
    //
    // Doing that will catch the 4-second wrap-around
    do {
        time1_first  = UVM_GPU_READ_ONCE(*gpu->time.time1_register);
        rmb();
        time0        = UVM_GPU_READ_ONCE(*gpu->time.time0_register);
        rmb();
        time1_second = UVM_GPU_READ_ONCE(*gpu->time.time1_register);
    } while (time1_second != time1_first);

    return (((NvU64)time1_first) << 32) + time0;
}
