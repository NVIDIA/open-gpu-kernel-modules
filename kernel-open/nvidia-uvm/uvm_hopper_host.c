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
#include "uvm_push.h"
#include "uvm_push_macros.h"
#include "clc86f.h"

// TODO: Bug 3210931: Rename HOST references and files to ESCHED.

void uvm_hal_hopper_host_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    NvU32 sem_lo;

    UVM_ASSERT_MSG(uvm_push_get_gpu(push)->parent->host_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel ? push->channel->name : "fake",
                   uvm_gpu_name(uvm_push_get_gpu(push)));

    UVM_ASSERT(!(NvOffset_LO32(gpu_va) & ~HWSHIFTMASK(C86F, SEM_ADDR_LO, OFFSET)));
    sem_lo = READ_HWVALUE(NvOffset_LO32(gpu_va), C86F, SEM_ADDR_LO, OFFSET);

    uvm_hal_wfi_membar(push, uvm_push_get_and_reset_membar_flag(push));

    NV_PUSH_5U(C86F, SEM_ADDR_LO,    HWVALUE(C86F, SEM_ADDR_LO, OFFSET, sem_lo),
                     SEM_ADDR_HI,    HWVALUE(C86F, SEM_ADDR_HI, OFFSET, NvOffset_HI32(gpu_va)),
                     SEM_PAYLOAD_LO, payload,
                     SEM_PAYLOAD_HI, 0,
                     SEM_EXECUTE,    HWCONST(C86F, SEM_EXECUTE, OPERATION, RELEASE) |
                                     HWCONST(C86F, SEM_EXECUTE, PAYLOAD_SIZE, 32BIT) |
                                     HWCONST(C86F, SEM_EXECUTE, RELEASE_TIMESTAMP, DIS) |
                                     HWCONST(C86F, SEM_EXECUTE, RELEASE_WFI, DIS));
}

void uvm_hal_hopper_host_semaphore_acquire(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    NvU32 sem_lo;

    UVM_ASSERT_MSG(uvm_push_get_gpu(push)->parent->host_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel ? push->channel->name : "fake",
                   uvm_gpu_name(uvm_push_get_gpu(push)));

    UVM_ASSERT(!(NvOffset_LO32(gpu_va) & ~HWSHIFTMASK(C86F, SEM_ADDR_LO, OFFSET)));
    sem_lo = READ_HWVALUE(NvOffset_LO32(gpu_va), C86F, SEM_ADDR_LO, OFFSET);
    NV_PUSH_5U(C86F, SEM_ADDR_LO,    HWVALUE(C86F, SEM_ADDR_LO, OFFSET, sem_lo),
                     SEM_ADDR_HI,    HWVALUE(C86F, SEM_ADDR_HI, OFFSET, NvOffset_HI32(gpu_va)),
                     SEM_PAYLOAD_LO, payload,
                     SEM_PAYLOAD_HI, 0,
                     SEM_EXECUTE,    HWCONST(C86F, SEM_EXECUTE, OPERATION, ACQ_CIRC_GEQ) |
                                     HWCONST(C86F, SEM_EXECUTE, PAYLOAD_SIZE, 32BIT) |
                                     HWCONST(C86F, SEM_EXECUTE, ACQUIRE_SWITCH_TSG, EN));
}

void uvm_hal_hopper_host_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va)
{
    NvU32 sem_lo;

    UVM_ASSERT_MSG(uvm_push_get_gpu(push)->parent->host_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel ? push->channel->name : "fake",
                   uvm_gpu_name(uvm_push_get_gpu(push)));

    UVM_ASSERT(!(NvOffset_LO32(gpu_va) & ~HWSHIFTMASK(C86F, SEM_ADDR_LO, OFFSET)));
    sem_lo = READ_HWVALUE(NvOffset_LO32(gpu_va), C86F, SEM_ADDR_LO, OFFSET);

    uvm_hal_wfi_membar(push, uvm_push_get_and_reset_membar_flag(push));

    NV_PUSH_5U(C86F, SEM_ADDR_LO,    HWVALUE(C86F, SEM_ADDR_LO, OFFSET, sem_lo),
                     SEM_ADDR_HI,    HWVALUE(C86F, SEM_ADDR_HI, OFFSET, NvOffset_HI32(gpu_va)),
                     SEM_PAYLOAD_LO, 0xdeadbeef,
                     SEM_PAYLOAD_HI, 0,
                     SEM_EXECUTE,    HWCONST(C86F, SEM_EXECUTE, OPERATION, RELEASE) |
                                     HWCONST(C86F, SEM_EXECUTE, PAYLOAD_SIZE, 32BIT) |
                                     HWCONST(C86F, SEM_EXECUTE, RELEASE_TIMESTAMP, EN) |
                                     HWCONST(C86F, SEM_EXECUTE, RELEASE_WFI, DIS));
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
            return HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_ACCESS_TYPE, VIRT_ALL);
        case UVM_FAULT_CANCEL_VA_MODE_WRITE_AND_ATOMIC:
            return HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_ACCESS_TYPE, VIRT_WRITE_AND_ATOMIC);
        default:
            UVM_ASSERT_MSG(false, "Invalid cancel_va_mode %d\n", cancel_va_mode);
    }

    return 0;
}

void uvm_hal_hopper_cancel_faults_va(uvm_push_t *push,
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
        aperture_value = HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx not aligned to 4KB\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    access_type_value = fault_cancel_va_mode_to_cancel_access_type(cancel_va_mode);

    UVM_ASSERT_MSG(IS_ALIGNED(addr, 1 << 12), "addr 0x%llx not aligned to 4KB\n", addr);
    addr >>= 12;

    addr_lo = addr & HWMASK(C86F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
    addr_hi = addr >> HWSIZE(C86F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);

    NV_PUSH_4U(C86F, MEM_OP_A, HWCONST(C86F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS) |
                               HWVALUE(C86F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, addr_lo) |
                               HWVALUE(C86F, MEM_OP_A, TLB_INVALIDATE_CANCEL_MMU_ENGINE_ID, mmu_engine_id),
                     MEM_OP_B, HWVALUE(C86F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, addr_hi),
                     MEM_OP_C, HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWVALUE(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_REPLAY, CANCEL_VA_GLOBAL) |
                               HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, NONE) |
                               access_type_value |
                               aperture_value,
                     MEM_OP_D, HWCONST(C86F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                               HWVALUE(C86F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
}

void uvm_hal_hopper_host_tlb_invalidate_all(uvm_push_t *push,
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
        aperture_value = HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    // PDE4 is the highest level on Hopper, see the comment in uvm_hopper_mmu.c
    // for details.
    UVM_ASSERT_MSG(depth < NVC86F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE4, "depth %u", depth);
    page_table_level = NVC86F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE4 - depth;

    if (membar != UVM_MEMBAR_NONE) {
        // If a GPU or SYS membar is needed, ACK_TYPE needs to be set to
        // GLOBALLY to make sure all the pending accesses can be picked up by
        // the membar.
        ack_value = HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);
    }

    if (membar == UVM_MEMBAR_SYS)
        sysmembar_value = HWCONST(C86F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, EN);
    else
        sysmembar_value = HWCONST(C86F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS);

    NV_PUSH_4U(C86F, MEM_OP_A, sysmembar_value |
                               HWCONST(C86F, MEM_OP_A, TLB_INVALIDATE_INVAL_SCOPE, NON_LINK_TLBS),
                     MEM_OP_B, 0,
                     MEM_OP_C, HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWVALUE(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                               HWVALUE(C86F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                               aperture_value |
                               ack_value,
                     MEM_OP_D, HWCONST(C86F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE) |
                               HWVALUE(C86F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));

    // GPU membar still requires an explicit membar method.
    if (membar == UVM_MEMBAR_GPU)
        uvm_push_get_gpu(push)->parent->host_hal->membar_gpu(push);
}

void uvm_hal_hopper_host_tlb_invalidate_va(uvm_push_t *push,
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
    va_lo = base & HWMASK(C86F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
    va_hi = base >> HWSIZE(C86F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);

    UVM_ASSERT_MSG(pdb.aperture == UVM_APERTURE_VID || pdb.aperture == UVM_APERTURE_SYS, "aperture: %u", pdb.aperture);

    if (pdb.aperture == UVM_APERTURE_VID)
        aperture_value = HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    // PDE4 is the highest level on Hopper, see the comment in uvm_hopper_mmu.c
    // for details.
    UVM_ASSERT_MSG(depth < NVC86F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE4, "depth %u", depth);
    page_table_level = NVC86F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE4 - depth;

    if (membar != UVM_MEMBAR_NONE) {
        // If a GPU or SYS membar is needed, ACK_TYPE needs to be set to
        // GLOBALLY to make sure all the pending accesses can be picked up by
        // the membar.
        ack_value = HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);
    }

    if (membar == UVM_MEMBAR_SYS)
        sysmembar_value = HWCONST(C86F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, EN);
    else
        sysmembar_value = HWCONST(C86F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS);

    NV_PUSH_4U(C86F, MEM_OP_A, HWVALUE(C86F, MEM_OP_A, TLB_INVALIDATE_INVALIDATION_SIZE, log2_invalidation_size) |
                               sysmembar_value |
                               HWCONST(C86F, MEM_OP_A, TLB_INVALIDATE_INVAL_SCOPE, NON_LINK_TLBS) |
                               HWVALUE(C86F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, va_lo),
                     MEM_OP_B, HWVALUE(C86F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, va_hi),
                     MEM_OP_C, HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWVALUE(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                               HWVALUE(C86F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                               aperture_value |
                               ack_value,
                     MEM_OP_D, HWCONST(C86F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                               HWVALUE(C86F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));

    // GPU membar still requires an explicit membar method.
    if (membar == UVM_MEMBAR_GPU)
        gpu->parent->host_hal->membar_gpu(push);
}

void uvm_hal_hopper_host_tlb_invalidate_test(uvm_push_t *push,
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
        aperture_value = HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    if (params->page_table_level != UvmInvalidatePageTableLevelAll) {
        // PDE4 is the highest level on Hopper, see the comment in
        // uvm_hopper_mmu.c for details.
        page_table_level = min((NvU32)UvmInvalidatePageTableLevelPde4, params->page_table_level) - 1;
    }

    if (params->membar != UvmInvalidateTlbMemBarNone) {
        // If a GPU or SYS membar is needed, ack_value needs to be set to
        // GLOBALLY to make sure all the pending accesses can be picked up by
        // the membar.
        ack_value = HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);
    }

    if (params->membar == UvmInvalidateTlbMemBarSys)
        sysmembar_value = HWCONST(C86F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, EN);
    else
        sysmembar_value = HWCONST(C86F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS);

    if (params->disable_gpc_invalidate)
        invalidate_gpc_value = HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_GPC, DISABLE);
    else
        invalidate_gpc_value = HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE);

    if (params->target_va_mode == UvmTargetVaModeTargeted) {
        NvU64 va = params->va >> 12;

        NvU32 va_lo = va & HWMASK(C86F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
        NvU32 va_hi = va >> HWSIZE(C86F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);

        NV_PUSH_4U(C86F, MEM_OP_A, sysmembar_value |
                                   HWCONST(C86F, MEM_OP_A, TLB_INVALIDATE_INVAL_SCOPE, NON_LINK_TLBS) |
                                   HWVALUE(C86F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, va_lo),
                         MEM_OP_B, HWVALUE(C86F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, va_hi),
                         MEM_OP_C, HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                                   HWVALUE(C86F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                                   HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                                   HWVALUE(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                                   invalidate_gpc_value |
                                   aperture_value |
                                   ack_value,
                         MEM_OP_D, HWCONST(C86F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                                   HWVALUE(C86F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
    }
    else {
        NV_PUSH_4U(C86F, MEM_OP_A, sysmembar_value |
                                   HWCONST(C86F, MEM_OP_A, TLB_INVALIDATE_INVAL_SCOPE, NON_LINK_TLBS),
                         MEM_OP_B, 0,
                         MEM_OP_C, HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                                   HWVALUE(C86F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                                   HWCONST(C86F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                                   HWVALUE(C86F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                                   invalidate_gpc_value |
                                   aperture_value |
                                   ack_value,
                         MEM_OP_D, HWCONST(C86F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE) |
                                   HWVALUE(C86F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
    }

    // GPU membar still requires an explicit membar method.
    if (params->membar == UvmInvalidateTlbMemBarLocal)
        uvm_push_get_gpu(push)->parent->host_hal->membar_gpu(push);
}

void uvm_hal_hopper_host_set_gpfifo_pushbuffer_segment_base(NvU64 *fifo_entry, NvU64 pushbuffer_va)
{
    UVM_ASSERT(!uvm_global_is_suspended());
    UVM_ASSERT((pushbuffer_va >> 40) == ((NvU64)(pushbuffer_va + UVM_PUSHBUFFER_SIZE) >> 40));

    *fifo_entry  =         HWVALUE(C86F, GP_ENTRY0, PB_EXTENDED_BASE_OPERAND, (pushbuffer_va >> 40));
    *fifo_entry |= (NvU64)(HWCONST(C86F, GP_ENTRY1, OPCODE, SET_PB_SEGMENT_EXTENDED_BASE)) << 32;
}

void uvm_hal_hopper_host_set_gpfifo_entry(NvU64 *fifo_entry,
                                          NvU64 pushbuffer_va,
                                          NvU32 pushbuffer_length,
                                          uvm_gpfifo_sync_t sync_flag)
{
    NvU64 fifo_entry_value;
    NvU64 pb_low_bits_mask = (1ull << 40) - 1;
    const NvU32 sync_value = (sync_flag == UVM_GPFIFO_SYNC_WAIT) ? HWCONST(C86F, GP_ENTRY1, SYNC, WAIT) :
                                                                   HWCONST(C86F, GP_ENTRY1, SYNC, PROCEED);

    UVM_ASSERT(!uvm_global_is_suspended());
    UVM_ASSERT_MSG(IS_ALIGNED(pushbuffer_va, 4), "pushbuffer va unaligned: %llu\n", pushbuffer_va);
    UVM_ASSERT_MSG(IS_ALIGNED(pushbuffer_length, 4), "pushbuffer length unaligned: %u\n", pushbuffer_length);

    // The PBDMA/Esched fetches the lower 40-bits of a pushbuffer VA from the
    // GPFIFO entry. The VA upper bits are stored internally by Esched and set
    // by uvm_channel_write_ctrl_gpfifo()/set_gpfifo_pushbuffer_segment_base().
    pushbuffer_va &= pb_low_bits_mask;
    fifo_entry_value =          HWVALUE(C86F, GP_ENTRY0, GET, NvU64_LO32(pushbuffer_va) >> 2);
    fifo_entry_value |= (NvU64)(HWVALUE(C86F, GP_ENTRY1, GET_HI, NvU64_HI32(pushbuffer_va)) |
                                HWVALUE(C86F, GP_ENTRY1, LENGTH, pushbuffer_length >> 2) |
                                sync_value) << 32;

    *fifo_entry = fifo_entry_value;
}
