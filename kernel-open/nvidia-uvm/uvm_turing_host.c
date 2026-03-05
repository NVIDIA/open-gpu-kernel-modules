/*******************************************************************************
    Copyright (c) 2017-2025 NVIDIA Corporation

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
#include "clc076.h"
#include "clc46f.h"

void uvm_hal_turing_host_init(uvm_push_t *push)
{
    if (uvm_channel_is_ce(push->channel))
        NV_PUSH_1U(C076, SET_OBJECT, GP100_UVM_SW);
}

void uvm_hal_turing_host_wait_for_idle(uvm_push_t *push)
{
    NV_PUSH_1U(C46F, WFI, 0);
}

void uvm_hal_turing_host_membar_sys(uvm_push_t *push)
{
    NV_PUSH_4U(C46F, MEM_OP_A, 0,
                     MEM_OP_B, 0,
                     MEM_OP_C, HWCONST(C46F, MEM_OP_C, MEMBAR_TYPE, SYS_MEMBAR),
                     MEM_OP_D, HWCONST(C46F, MEM_OP_D, OPERATION, MEMBAR));
}

void uvm_hal_turing_host_membar_gpu(uvm_push_t *push)
{
    NV_PUSH_4U(C46F, MEM_OP_A, 0,
                     MEM_OP_B, 0,
                     MEM_OP_C, HWCONST(C46F, MEM_OP_C, MEMBAR_TYPE, MEMBAR),
                     MEM_OP_D, HWCONST(C46F, MEM_OP_D, OPERATION, MEMBAR));
}

void uvm_hal_turing_host_noop(uvm_push_t *push, NvU32 size)
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
        NV_PUSH_NU_NONINC(C46F, NOP, noop_this_time - 1);

        size -= noop_this_time;
    }
}

void uvm_hal_turing_host_interrupt(uvm_push_t *push)
{
    NV_PUSH_1U(C46F, NON_STALL_INTERRUPT, 0);
}

void uvm_hal_turing_host_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    NvU32 sem_lo;

    UVM_ASSERT_MSG(uvm_push_get_gpu(push)->parent->host_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel->name,
                   uvm_gpu_name(uvm_push_get_gpu(push)));

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

    UVM_ASSERT_MSG(uvm_push_get_gpu(push)->parent->host_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel->name,
                   uvm_gpu_name(uvm_push_get_gpu(push)));

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

void uvm_hal_turing_host_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va)
{
    NvU32 sem_lo;

    UVM_ASSERT_MSG(uvm_push_get_gpu(push)->parent->host_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel->name,
                   uvm_gpu_name(uvm_push_get_gpu(push)));

    UVM_ASSERT(!(NvOffset_LO32(gpu_va) & ~HWSHIFTMASK(C46F, SEM_ADDR_LO, OFFSET)));
    sem_lo = READ_HWVALUE(NvOffset_LO32(gpu_va), C46F, SEM_ADDR_LO, OFFSET);

    uvm_hal_wfi_membar(push, uvm_push_get_and_reset_membar_flag(push));

    NV_PUSH_5U(C46F, SEM_ADDR_LO,    HWVALUE(C46F, SEM_ADDR_LO, OFFSET, sem_lo),
                     SEM_ADDR_HI,    HWVALUE(C46F, SEM_ADDR_HI, OFFSET, NvOffset_HI32(gpu_va)),
                     SEM_PAYLOAD_LO, 0xdeadbeef,
                     SEM_PAYLOAD_HI, 0,
                     SEM_EXECUTE,    HWCONST(C46F, SEM_EXECUTE, OPERATION, RELEASE) |
                                     HWCONST(C46F, SEM_EXECUTE, PAYLOAD_SIZE, 32BIT) |
                                     HWCONST(C46F, SEM_EXECUTE, RELEASE_TIMESTAMP, EN) |
                                     HWCONST(C46F, SEM_EXECUTE, RELEASE_WFI, DIS));
}

void uvm_hal_turing_host_clear_faulted_channel_sw_method_unsupported(uvm_push_t *push,
                                                                     uvm_user_channel_t *user_channel,
                                                                     const uvm_fault_buffer_entry_t *buffer_entry)
{
    UVM_ASSERT_MSG(false, "host clear_faulted_channel_sw_method called on Turing GPU\n");
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

void uvm_hal_turing_host_clear_faulted_channel_register_unsupported(uvm_user_channel_t *user_channel,
                                                                    const uvm_fault_buffer_entry_t *buffer_entry)
{
    UVM_ASSERT_MSG(false, "host clear_faulted_channel_register called on Turing GPU\n");
}

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

void uvm_hal_turing_host_set_gpfifo_noop(NvU64 *fifo_entry)
{
    UVM_ASSERT(!uvm_global_is_suspended());

    // A NOOP control GPFIFO does not require a GP_ENTRY0.
    *fifo_entry = (NvU64)(HWVALUE(C46F, GP_ENTRY1, LENGTH, 0) | HWCONST(C46F, GP_ENTRY1, OPCODE, NOP)) << 32;
}

void uvm_hal_turing_host_set_gpfifo_pushbuffer_segment_base_unsupported(NvU64 *fifo_entry, NvU64 pushbuffer_va)
{
    UVM_ASSERT_MSG(false, "host set_gpfifo_pushbuffer_segment_base called on unsupported GPU\n");
}

void uvm_hal_turing_host_write_gpu_put(uvm_channel_t *channel, NvU32 gpu_put)
{
    // We need to add a BAR1 read if GPPut is located in sysmem. This
    // guarantees that any in-flight BAR1 writes from the CPU will have reached
    // the GPU by the time the GPU reads the updated GPPut. Read the provided
    // BAR1 mapping in channel_info.
    if (channel->channel_info.dummyBar1Mapping)
        UVM_GPU_READ_ONCE(*channel->channel_info.dummyBar1Mapping);

    UVM_GPU_WRITE_ONCE(*channel->channel_info.gpPut, gpu_put);

    wmb();

    UVM_GPU_WRITE_ONCE(*channel->channel_info.workSubmissionOffset, channel->channel_info.workSubmissionToken);
}

void uvm_hal_turing_host_tlb_invalidate_all(uvm_push_t *push,
                                            uvm_gpu_phys_address_t pdb,
                                            NvU32 depth,
                                            uvm_membar_t membar)
{
    NvU32 aperture_value;
    NvU32 page_table_level;
    NvU32 pdb_lo;
    NvU32 pdb_hi;
    NvU32 ack_value = 0;
    NvU32 sysmembar_value = 0;

    UVM_ASSERT_MSG(pdb.aperture == UVM_APERTURE_VID || pdb.aperture == UVM_APERTURE_SYS, "aperture: %u", pdb.aperture);

    if (pdb.aperture == UVM_APERTURE_VID)
        aperture_value = HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    // PDE3 is the highest level on Pascal-Turing, see the comment in
    // uvm_pascal_mmu.c for details.
    UVM_ASSERT_MSG(depth < NVC46F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3, "depth %u", depth);
    page_table_level = NVC46F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3 - depth;

    if (membar != UVM_MEMBAR_NONE) {
        // If a GPU or SYS membar is needed, ACK_TYPE needs to be set to
        // GLOBALLY to make sure all the pending accesses can be picked up by
        // the membar.
        ack_value = HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);
    }

    if (membar == UVM_MEMBAR_SYS)
        sysmembar_value = HWCONST(C46F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, EN);
    else
        sysmembar_value = HWCONST(C46F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS);

    NV_PUSH_4U(C46F, MEM_OP_A, sysmembar_value,
                     MEM_OP_B, 0,
                     MEM_OP_C, HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWVALUE(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                               HWVALUE(C46F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                               aperture_value |
                               ack_value,
                     MEM_OP_D, HWCONST(C46F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE) |
                               HWVALUE(C46F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));

    // GPU membar still requires an explicit membar method.
    if (membar == UVM_MEMBAR_GPU)
        uvm_push_get_gpu(push)->parent->host_hal->membar_gpu(push);
}

void uvm_hal_turing_host_tlb_invalidate_va(uvm_push_t *push,
                                           uvm_gpu_phys_address_t pdb,
                                           NvU32 depth,
                                           NvU64 base,
                                           NvU64 size,
                                           NvU64 page_size,
                                           uvm_membar_t membar)
{
    NvU32 aperture_value;
    NvU32 page_table_level;
    NvU32 pdb_lo;
    NvU32 pdb_hi;
    NvU32 ack_value = 0;
    NvU32 sysmembar_value = 0;
    NvU32 va_lo;
    NvU32 va_hi;
    NvU64 end;
    NvU64 actual_base;
    NvU64 actual_size;
    NvU64 actual_end;
    NvU32 log2_invalidation_size;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    UVM_ASSERT_MSG(IS_ALIGNED(page_size, 1 << 12), "page_size 0x%llx\n", page_size);
    UVM_ASSERT_MSG(IS_ALIGNED(base, page_size), "base 0x%llx page_size 0x%llx\n", base, page_size);
    UVM_ASSERT_MSG(IS_ALIGNED(size, page_size), "size 0x%llx page_size 0x%llx\n", size, page_size);
    UVM_ASSERT_MSG(size > 0, "size 0x%llx\n", size);

    // The invalidation size must be a power-of-two number of pages containing
    // the passed interval
    end = base + size - 1;
    log2_invalidation_size = __fls((unsigned long)(end ^ base)) + 1;

    if (log2_invalidation_size == 64) {
        // Invalidate everything
        gpu->parent->host_hal->tlb_invalidate_all(push, pdb, depth, membar);
        return;
    }

    // The hardware aligns the target address down to the invalidation size.
    actual_size = 1ULL << log2_invalidation_size;
    actual_base = UVM_ALIGN_DOWN(base, actual_size);
    actual_end = actual_base + actual_size - 1;
    UVM_ASSERT(actual_end >= end);

    // The invalidation size field expects log2(invalidation size in 4K), not
    // log2(invalidation size in bytes)
    log2_invalidation_size -= 12;

    // Address to invalidate, as a multiple of 4K.
    base >>= 12;
    va_lo = base & HWMASK(C46F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
    va_hi = base >> HWSIZE(C46F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);

    UVM_ASSERT_MSG(pdb.aperture == UVM_APERTURE_VID || pdb.aperture == UVM_APERTURE_SYS, "aperture: %u", pdb.aperture);

    if (pdb.aperture == UVM_APERTURE_VID)
        aperture_value = HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    // PDE3 is the highest level on Pascal-Turing, see the comment in
    // uvm_pascal_mmu.c for details.
    UVM_ASSERT_MSG(depth < NVC46F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3, "depth %u", depth);
    page_table_level = NVC46F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3 - depth;

    if (membar != UVM_MEMBAR_NONE) {
        // If a GPU or SYS membar is needed, ACK_TYPE needs to be set to
        // GLOBALLY to make sure all the pending accesses can be picked up by
        // the membar.
        ack_value = HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);
    }

    if (membar == UVM_MEMBAR_SYS)
        sysmembar_value = HWCONST(C46F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, EN);
    else
        sysmembar_value = HWCONST(C46F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS);

    NV_PUSH_4U(C46F, MEM_OP_A, HWVALUE(C46F, MEM_OP_A, TLB_INVALIDATE_INVALIDATION_SIZE, log2_invalidation_size) |
                               sysmembar_value |
                               HWVALUE(C46F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, va_lo),
                     MEM_OP_B, HWVALUE(C46F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, va_hi),
                     MEM_OP_C, HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWVALUE(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                               HWVALUE(C46F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                               aperture_value |
                               ack_value,
                     MEM_OP_D, HWCONST(C46F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                               HWVALUE(C46F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));

    // GPU membar still requires an explicit membar method.
    if (membar == UVM_MEMBAR_GPU)
        gpu->parent->host_hal->membar_gpu(push);
}

void uvm_hal_turing_host_tlb_invalidate_phys_unsupported(uvm_push_t *push)
{
    UVM_ASSERT_MSG(false, "GPA caching is not supported\n");
}

void uvm_hal_turing_host_tlb_invalidate_test(uvm_push_t *push,
                                             uvm_gpu_phys_address_t pdb,
                                             UVM_TEST_INVALIDATE_TLB_PARAMS *params)
{
    NvU32 ack_value = 0;
    NvU32 sysmembar_value = 0;
    NvU32 invalidate_gpc_value = 0;
    NvU32 aperture_value = 0;
    NvU32 pdb_lo = 0;
    NvU32 pdb_hi = 0;
    NvU32 page_table_level = 0;

    UVM_ASSERT_MSG(pdb.aperture == UVM_APERTURE_VID || pdb.aperture == UVM_APERTURE_SYS, "aperture: %u", pdb.aperture);
    if (pdb.aperture == UVM_APERTURE_VID)
        aperture_value = HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    if (params->page_table_level != UvmInvalidatePageTableLevelAll) {
        // PDE3 is the highest level on Pascal-Turing, see the comment in
        // uvm_pascal_mmu.c for details.
        page_table_level = min((NvU32)UvmInvalidatePageTableLevelPde3, params->page_table_level) - 1;
    }

    if (params->membar != UvmInvalidateTlbMemBarNone) {
        // If a GPU or SYS membar is needed, ack_value needs to be set to
        // GLOBALLY to make sure all the pending accesses can be picked up by
        // the membar.
        ack_value = HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);
    }

    if (params->membar == UvmInvalidateTlbMemBarSys)
        sysmembar_value = HWCONST(C46F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, EN);
    else
        sysmembar_value = HWCONST(C46F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS);

    if (params->disable_gpc_invalidate)
        invalidate_gpc_value = HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_GPC, DISABLE);
    else
        invalidate_gpc_value = HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE);

    if (params->target_va_mode == UvmTargetVaModeTargeted) {
        NvU64 va = params->va >> 12;

        NvU32 va_lo = va & HWMASK(C46F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
        NvU32 va_hi = va >> HWSIZE(C46F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
        NV_PUSH_4U(C46F, MEM_OP_A, sysmembar_value |
                                   HWVALUE(C46F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, va_lo),
                         MEM_OP_B, HWVALUE(C46F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, va_hi),
                         MEM_OP_C, HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                                   HWVALUE(C46F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                                   HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                                   HWVALUE(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                                   invalidate_gpc_value |
                                   aperture_value |
                                   ack_value,
                         MEM_OP_D, HWCONST(C46F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                                   HWVALUE(C46F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
    }
    else {
        NV_PUSH_4U(C46F, MEM_OP_A, sysmembar_value,
                         MEM_OP_B, 0,
                         MEM_OP_C, HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                                   HWVALUE(C46F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                                   HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                                   HWVALUE(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                                   invalidate_gpc_value |
                                   aperture_value |
                                   ack_value,
                         MEM_OP_D, HWCONST(C46F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE) |
                                   HWVALUE(C46F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
    }

    // GPU membar still requires an explicit membar method.
    if (params->membar == UvmInvalidateTlbMemBarLocal)
        uvm_push_get_gpu(push)->parent->host_hal->membar_gpu(push);
}

void uvm_hal_turing_host_tlb_flush_prefetch_unsupported(uvm_push_t *push)
{
    UVM_ASSERT_MSG(false, "Prefetch flushing not needed on %s\n", uvm_gpu_name(uvm_push_get_gpu(push)));
}

void uvm_hal_turing_replay_faults(uvm_push_t *push, uvm_fault_replay_type_t type)
{
    NvU32 replay_value = 0;
    const NvU32 va_lo = 0;
    const NvU32 va_hi = 0;
    const NvU32 pdb_lo = 0;
    const NvU32 pdb_hi = 0;

    // On Turing+ the MMU will forward the replay to the uTLBs even if the PDB
    // is not in the MMU PDB_ID cache. Therefore, we target a dummy PDB to
    // avoid any VA invalidation, which could impact on the performance.
    UVM_ASSERT_MSG(type == UVM_FAULT_REPLAY_TYPE_START || type == UVM_FAULT_REPLAY_TYPE_START_ACK_ALL,
                   "replay_type: %u\n",
                   type);

    if (type == UVM_FAULT_REPLAY_TYPE_START)
        replay_value = HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_REPLAY, START);
    else if (type == UVM_FAULT_REPLAY_TYPE_START_ACK_ALL)
        replay_value = HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_REPLAY, START_ACK_ALL);

    NV_PUSH_4U(C46F, MEM_OP_A, HWCONST(C46F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS) |
                               HWVALUE(C46F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, va_lo),
                     MEM_OP_B, HWVALUE(C46F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, va_hi),
                     MEM_OP_C, HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWVALUE(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, PTE_ONLY) |
                               HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM) |
                               replay_value,
                     MEM_OP_D, HWCONST(C46F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                               HWVALUE(C46F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
}

static void instance_ptr_address_to_hw_values(NvU64 instance_ptr_address,
                                              NvU32 *instance_ptr_lo,
                                              NvU32 *instance_ptr_hi)
{
    // instance_ptr must be 4K aligned
    UVM_ASSERT_MSG(IS_ALIGNED(instance_ptr_address, 1 << 12), "instance_ptr 0x%llx\n", instance_ptr_address);
    instance_ptr_address >>= 12;

    *instance_ptr_lo = instance_ptr_address & HWMASK(C076, FAULT_CANCEL_A, INST_LOW);
    *instance_ptr_hi = instance_ptr_address >> HWSIZE(C076, FAULT_CANCEL_A, INST_LOW);
}

static NvU32 instance_ptr_aperture_type_to_hw_value(uvm_aperture_t aperture)
{
    switch (aperture)
    {
        case UVM_APERTURE_SYS:
            return HWCONST(C076, FAULT_CANCEL_A, INST_APERTURE, SYS_MEM_COHERENT);
        case UVM_APERTURE_VID:
            return HWCONST(C076, FAULT_CANCEL_A, INST_APERTURE, VID_MEM);
        default:
            UVM_ASSERT_MSG(false, "Invalid aperture_type %d\n", aperture);
    }

    return 0;
}

void uvm_hal_turing_cancel_faults_global(uvm_push_t *push, uvm_gpu_phys_address_t instance_ptr)
{
    NvU32 instance_ptr_lo, instance_ptr_hi;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    // Global cancellations affect all MIG instances, not only the one that
    // originated the fault to be cancelled
    UVM_ASSERT(!gpu->parent->smc.enabled);

    instance_ptr_address_to_hw_values(instance_ptr.address, &instance_ptr_lo, &instance_ptr_hi);

    NV_PUSH_3U(C076, FAULT_CANCEL_A, HWVALUE(C076, FAULT_CANCEL_A, INST_LOW, instance_ptr_lo) |
                                     instance_ptr_aperture_type_to_hw_value(instance_ptr.aperture),
                     FAULT_CANCEL_B, HWVALUE(C076, FAULT_CANCEL_B, INST_HI, instance_ptr_hi),
                     FAULT_CANCEL_C, HWCONST(C076, FAULT_CANCEL_C, MODE, GLOBAL));
}

void uvm_hal_turing_cancel_faults_targeted(uvm_push_t *push,
                                           uvm_gpu_phys_address_t instance_ptr,
                                           NvU32 gpc_id,
                                           NvU32 client_id)
{
    NvU32 instance_ptr_lo, instance_ptr_hi;

    instance_ptr_address_to_hw_values(instance_ptr.address, &instance_ptr_lo, &instance_ptr_hi);

    NV_PUSH_3U(C076, FAULT_CANCEL_A, HWVALUE(C076, FAULT_CANCEL_A, INST_LOW, instance_ptr_lo) |
                                     instance_ptr_aperture_type_to_hw_value(instance_ptr.aperture),
                     FAULT_CANCEL_B, HWVALUE(C076, FAULT_CANCEL_B, INST_HI, instance_ptr_hi),
                     FAULT_CANCEL_C, HWVALUE(C076, FAULT_CANCEL_C, CLIENT_ID, client_id) |
                                     HWVALUE(C076, FAULT_CANCEL_C, GPC_ID, gpc_id) |
                                     HWCONST(C076, FAULT_CANCEL_C, MODE, TARGETED));
}

static NvU32 fault_cancel_va_mode_to_cancel_access_type(uvm_fault_cancel_va_mode_t cancel_va_mode)
{
    // There are only two logical cases from the perspective of UVM. Accesses to
    // an invalid address, which will cancel all accesses on the page, and
    // accesses with an invalid type on a read-only page, which will cancel all
    // write/atomic accesses on the page.
    switch (cancel_va_mode)
    {
        case UVM_FAULT_CANCEL_VA_MODE_ALL:
            return HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_ACCESS_TYPE, VIRT_ALL);
        case UVM_FAULT_CANCEL_VA_MODE_WRITE_AND_ATOMIC:
            return HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_ACCESS_TYPE, VIRT_WRITE_AND_ATOMIC);
        default:
            UVM_ASSERT_MSG(false, "Invalid cancel_va_mode %d\n", cancel_va_mode);
    }

    return 0;
}

void uvm_hal_turing_cancel_faults_va(uvm_push_t *push,
                                     uvm_gpu_phys_address_t pdb,
                                     const uvm_fault_buffer_entry_t *fault_entry,
                                     uvm_fault_cancel_va_mode_t cancel_va_mode)
{
    NvU32 aperture_value;
    NvU32 pdb_lo;
    NvU32 pdb_hi;
    NvU32 addr_lo;
    NvU32 addr_hi;
    NvU32 access_type_value;
    NvU64 addr = fault_entry->fault_address;
    NvU32 mmu_engine_id = fault_entry->fault_source.mmu_engine_id;

    UVM_ASSERT_MSG(pdb.aperture == UVM_APERTURE_VID || pdb.aperture == UVM_APERTURE_SYS, "aperture: %u", pdb.aperture);

    if (pdb.aperture == UVM_APERTURE_VID)
        aperture_value = HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx not aligned to 4KB\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    access_type_value = fault_cancel_va_mode_to_cancel_access_type(cancel_va_mode);

    UVM_ASSERT_MSG(IS_ALIGNED(addr, 1 << 12), "addr 0x%llx not aligned to 4KB\n", addr);
    addr >>= 12;

    addr_lo = addr & HWMASK(C46F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
    addr_hi = addr >> HWSIZE(C46F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);

    NV_PUSH_4U(C46F, MEM_OP_A, HWCONST(C46F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS) |
                               HWVALUE(C46F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, addr_lo) |
                               HWVALUE(C46F, MEM_OP_A, TLB_INVALIDATE_CANCEL_MMU_ENGINE_ID, mmu_engine_id),
                     MEM_OP_B, HWVALUE(C46F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, addr_hi),
                     MEM_OP_C, HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWVALUE(C46F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_REPLAY, CANCEL_VA_GLOBAL) |
                               HWCONST(C46F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, NONE) |
                               access_type_value |
                               aperture_value,
                     MEM_OP_D, HWCONST(C46F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                               HWVALUE(C46F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
}

void uvm_hal_turing_access_counter_clear_all(uvm_push_t *push)
{
    NV_PUSH_4U(C46F, MEM_OP_A, 0,
                     MEM_OP_B, 0,
                     MEM_OP_C, 0,
                     MEM_OP_D, HWCONST(C46F, MEM_OP_D, OPERATION, ACCESS_COUNTER_CLR) |
                               HWCONST(C46F, MEM_OP_D, ACCESS_COUNTER_CLR_TYPE, ALL));
}

void uvm_hal_turing_access_counter_clear_targeted(uvm_push_t *push,
                                                  const uvm_access_counter_buffer_entry_t *buffer_entry)
{
    NV_PUSH_4U(C46F, MEM_OP_A, 0,
                     MEM_OP_B, 0,
                     MEM_OP_C, HWVALUE(C46F, MEM_OP_C, ACCESS_COUNTER_CLR_TARGETED_NOTIFY_TAG, buffer_entry->tag),
                     MEM_OP_D, HWCONST(C46F, MEM_OP_D, OPERATION, ACCESS_COUNTER_CLR) |
                               HWCONST(C46F, MEM_OP_D, ACCESS_COUNTER_CLR_TYPE, TARGETED) |
                               HWCONST(C46F, MEM_OP_D, ACCESS_COUNTER_CLR_TARGETED_TYPE, MIMC) |
                               HWVALUE(C46F, MEM_OP_D, ACCESS_COUNTER_CLR_TARGETED_BANK, buffer_entry->bank));
}

uvm_access_counter_clear_op_t
uvm_hal_turing_access_counter_query_clear_op(uvm_parent_gpu_t *parent_gpu,
                                             uvm_access_counter_buffer_entry_t **buffer_entries,
                                             NvU32 num_entries)
{
    return UVM_ACCESS_COUNTER_CLEAR_OP_TARGETED;
}

NvU64 uvm_hal_turing_get_time(uvm_gpu_t *gpu)
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
