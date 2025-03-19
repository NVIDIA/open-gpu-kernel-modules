/*******************************************************************************
    Copyright (c) 2016-2024 NVIDIA Corporation

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
#include "uvm_hal.h"
#include "uvm_push.h"
#include "uvm_user_channel.h"
#include "clc36f.h"

void uvm_hal_volta_host_write_gpu_put(uvm_channel_t *channel, NvU32 gpu_put)
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

static NvU32 fault_cancel_va_mode_to_cancel_access_type(uvm_fault_cancel_va_mode_t cancel_va_mode)
{
    // There are only two logical cases from the perspective of UVM. Accesses to
    // an invalid address, which will cancel all accesses on the page, and
    // accesses with an invalid type on a read-only page, which will cancel all
    // write/atomic accesses on the page.
    switch (cancel_va_mode)
    {
        case UVM_FAULT_CANCEL_VA_MODE_ALL:
            return HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_ACCESS_TYPE, VIRT_ALL);
        case UVM_FAULT_CANCEL_VA_MODE_WRITE_AND_ATOMIC:
            return HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_ACCESS_TYPE, VIRT_WRITE_AND_ATOMIC);
        default:
            UVM_ASSERT_MSG(false, "Invalid cancel_va_mode %d\n", cancel_va_mode);
    }

    return 0;
}

void uvm_hal_volta_cancel_faults_va(uvm_push_t *push,
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
        aperture_value = HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx not aligned to 4KB\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C36F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C36F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    access_type_value = fault_cancel_va_mode_to_cancel_access_type(cancel_va_mode);

    UVM_ASSERT_MSG(IS_ALIGNED(addr, 1 << 12), "addr 0x%llx not aligned to 4KB\n", addr);
    addr >>= 12;

    addr_lo = addr & HWMASK(C36F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
    addr_hi = addr >> HWSIZE(C36F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);

    NV_PUSH_4U(C36F, MEM_OP_A, HWCONST(C36F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS) |
                               HWVALUE(C36F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, addr_lo) |
                               HWVALUE(C36F, MEM_OP_A, TLB_INVALIDATE_CANCEL_MMU_ENGINE_ID, mmu_engine_id),
                     MEM_OP_B, HWVALUE(C36F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, addr_hi),
                     MEM_OP_C, HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWVALUE(C36F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_REPLAY, CANCEL_VA_GLOBAL) |
                               HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, NONE) |
                               access_type_value |
                               aperture_value,
                     MEM_OP_D, HWCONST(C36F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                               HWVALUE(C36F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
}

void uvm_hal_volta_host_clear_faulted_channel_method(uvm_push_t *push,
                                                     uvm_user_channel_t *user_channel,
                                                     const uvm_fault_buffer_entry_t *fault)
{
    NvU32 clear_type_value = 0;

    UVM_ASSERT(user_channel->gpu->parent->has_clear_faulted_channel_method);

    if (fault->fault_source.mmu_engine_type == UVM_MMU_ENGINE_TYPE_HOST) {
        clear_type_value = HWCONST(C36F, CLEAR_FAULTED, TYPE, PBDMA_FAULTED);
    }
    else if (fault->fault_source.mmu_engine_type == UVM_MMU_ENGINE_TYPE_CE) {
        clear_type_value = HWCONST(C36F, CLEAR_FAULTED, TYPE, ENG_FAULTED);
    }
    else {
        UVM_ASSERT_MSG(false, "Unsupported MMU engine type %s\n",
                       uvm_mmu_engine_type_string(fault->fault_source.mmu_engine_type));
    }

    NV_PUSH_1U(C36F, CLEAR_FAULTED, HWVALUE(C36F, CLEAR_FAULTED, CHID, user_channel->hw_channel_id) |
                                    clear_type_value);
}

void uvm_hal_volta_host_tlb_invalidate_va(uvm_push_t *push,
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
    va_lo = base & HWMASK(C36F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
    va_hi = base >> HWSIZE(C36F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);

    UVM_ASSERT_MSG(pdb.aperture == UVM_APERTURE_VID || pdb.aperture == UVM_APERTURE_SYS, "aperture: %u", pdb.aperture);

    if (pdb.aperture == UVM_APERTURE_VID)
        aperture_value = HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C36F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C36F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    // PDE3 is the highest level on Pascal and Volta, see the comment in
    // uvm_pascal_mmu.c for details.
    UVM_ASSERT_MSG(depth < NVC36F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3, "depth %u", depth);
    page_table_level = NVC36F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3 - depth;

    if (membar != UVM_MEMBAR_NONE) {
        // If a GPU or SYS membar is needed, ACK_TYPE needs to be set to
        // GLOBALLY to make sure all the pending accesses can be picked up by
        // the membar.
        ack_value = HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);
    }

    NV_PUSH_4U(C36F, MEM_OP_A, HWVALUE(C36F, MEM_OP_A, TLB_INVALIDATE_INVALIDATION_SIZE, log2_invalidation_size) |
                               HWCONST(C36F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS) |
                               HWVALUE(C36F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, va_lo),
                     MEM_OP_B, HWVALUE(C36F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, va_hi),
                     MEM_OP_C, HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWVALUE(C36F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                               HWVALUE(C36F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                               aperture_value |
                               ack_value,
                     MEM_OP_D, HWCONST(C36F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                               HWVALUE(C36F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));

    uvm_hal_tlb_invalidate_membar(push, membar);
}

void uvm_hal_volta_replay_faults(uvm_push_t *push, uvm_fault_replay_type_t type)
{
    NvU32 replay_value = 0;
    const NvU32 va_lo = 0;
    const NvU32 va_hi = 0;
    const NvU32 pdb_lo = 0;
    const NvU32 pdb_hi = 0;

    // On Volta+ the MMU will forward the replay to the uTLBs even if the PDB
    // is not in the MMU PDB_ID cache. Therefore, we target a dummy PDB to
    // avoid any VA invalidation, which could impact on the performance.
    UVM_ASSERT_MSG(type == UVM_FAULT_REPLAY_TYPE_START || type == UVM_FAULT_REPLAY_TYPE_START_ACK_ALL,
                   "replay_type: %u\n", type);

    if (type == UVM_FAULT_REPLAY_TYPE_START)
        replay_value = HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_REPLAY, START);
    else if (type == UVM_FAULT_REPLAY_TYPE_START_ACK_ALL)
        replay_value = HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_REPLAY, START_ACK_ALL);

    NV_PUSH_4U(C36F, MEM_OP_A, HWCONST(C36F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS) |
                               HWVALUE(C36F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, va_lo),
                     MEM_OP_B, HWVALUE(C36F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, va_hi),
                     MEM_OP_C, HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWVALUE(C36F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, PTE_ONLY) |
                               HWCONST(C36F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM) |
                               replay_value,
                     MEM_OP_D, HWCONST(C36F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                               HWVALUE(C36F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
}

void uvm_hal_volta_host_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va)
{
    NvU32 sem_lo;

    UVM_ASSERT_MSG(uvm_push_get_gpu(push)->parent->host_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel->name,
                   uvm_gpu_name(uvm_push_get_gpu(push)));

    UVM_ASSERT(!(NvOffset_LO32(gpu_va) & ~HWSHIFTMASK(C36F, SEM_ADDR_LO, OFFSET)));
    sem_lo = READ_HWVALUE(NvOffset_LO32(gpu_va), C36F, SEM_ADDR_LO, OFFSET);

    uvm_hal_wfi_membar(push, uvm_push_get_and_reset_membar_flag(push));

    NV_PUSH_5U(C36F, SEM_ADDR_LO,    HWVALUE(C36F, SEM_ADDR_LO, OFFSET, sem_lo),
                     SEM_ADDR_HI,    HWVALUE(C36F, SEM_ADDR_HI, OFFSET, NvOffset_HI32(gpu_va)),
                     SEM_PAYLOAD_LO, 0xdeadbeef,
                     SEM_PAYLOAD_HI, 0,
                     SEM_EXECUTE,    HWCONST(C36F, SEM_EXECUTE, OPERATION, RELEASE) |
                                     HWCONST(C36F, SEM_EXECUTE, PAYLOAD_SIZE, 32BIT) |
                                     HWCONST(C36F, SEM_EXECUTE, RELEASE_TIMESTAMP, EN) |
                                     HWCONST(C36F, SEM_EXECUTE, RELEASE_WFI, DIS));
}
