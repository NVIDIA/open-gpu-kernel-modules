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
#include "clc46f.h"

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
