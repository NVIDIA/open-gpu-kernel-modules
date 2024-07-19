/*******************************************************************************
    Copyright (c) 2024 NVIDIA Corporation

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
#include "clc96f.h"

// TODO: Bug 3210931: Rename HOST references and files to ESCHED.

void uvm_hal_blackwell_host_tlb_invalidate_all(uvm_push_t *push,
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
        aperture_value = HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    // PDE4 is the highest level on Blackwell, see the comment in
    // uvm_blackwell_mmu.c for details.
    UVM_ASSERT_MSG(depth < NVC96F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE4, "depth %u", depth);
    page_table_level = NVC96F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE4 - depth;

    if (membar != UVM_MEMBAR_NONE)
        ack_value = HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);

    if (membar == UVM_MEMBAR_SYS)
        sysmembar_value = HWCONST(C96F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, EN);
    else
        sysmembar_value = HWCONST(C96F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS);

    NV_PUSH_4U(C96F, MEM_OP_A, sysmembar_value |
                               HWCONST(C96F, MEM_OP_A, TLB_INVALIDATE_INVAL_SCOPE, NON_LINK_TLBS),
                     MEM_OP_B, 0,
                     MEM_OP_C, HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWVALUE(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                               HWVALUE(C96F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                               aperture_value |
                               ack_value,
                     MEM_OP_D, HWCONST(C96F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE) |
                               HWVALUE(C96F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
}

void uvm_hal_blackwell_host_tlb_invalidate_va(uvm_push_t *push,
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
    va_lo = base & HWMASK(C96F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
    va_hi = base >> HWSIZE(C96F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);

    UVM_ASSERT_MSG(pdb.aperture == UVM_APERTURE_VID || pdb.aperture == UVM_APERTURE_SYS, "aperture: %u", pdb.aperture);

    if (pdb.aperture == UVM_APERTURE_VID)
        aperture_value = HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    // PDE4 is the highest level on Blackwell, see the comment in
    // uvm_blackwell_mmu.c for details.
    UVM_ASSERT_MSG(depth < NVC96F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE4, "depth %u", depth);
    page_table_level = NVC96F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE4 - depth;

    if (membar != UVM_MEMBAR_NONE)
        ack_value = HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);

    if (membar == UVM_MEMBAR_SYS)
        sysmembar_value = HWCONST(C96F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, EN);
    else
        sysmembar_value = HWCONST(C96F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS);

    NV_PUSH_4U(C96F, MEM_OP_A, HWVALUE(C96F, MEM_OP_A, TLB_INVALIDATE_INVALIDATION_SIZE, log2_invalidation_size) |
                               sysmembar_value |
                               HWCONST(C96F, MEM_OP_A, TLB_INVALIDATE_INVAL_SCOPE, NON_LINK_TLBS) |
                               HWVALUE(C96F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, va_lo),
                     MEM_OP_B, HWVALUE(C96F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, va_hi),
                     MEM_OP_C, HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWVALUE(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                               HWVALUE(C96F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                               aperture_value |
                               ack_value,
                     MEM_OP_D, HWCONST(C96F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                               HWVALUE(C96F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
}

void uvm_hal_blackwell_host_tlb_invalidate_test(uvm_push_t *push,
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
        aperture_value = HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    if (params->page_table_level != UvmInvalidatePageTableLevelAll) {
        // PDE4 is the highest level on Blackwell, see the comment in
        // uvm_blackwell_mmu.c for details.
        page_table_level = min((NvU32)UvmInvalidatePageTableLevelPde4, params->page_table_level) - 1;
    }

    if (params->membar != UvmInvalidateTlbMemBarNone)
        ack_value = HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);

    if (params->membar == UvmInvalidateTlbMemBarSys)
        sysmembar_value = HWCONST(C96F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, EN);
    else
        sysmembar_value = HWCONST(C96F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS);

    if (params->disable_gpc_invalidate)
        invalidate_gpc_value = HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_GPC, DISABLE);
    else
        invalidate_gpc_value = HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE);

    if (params->target_va_mode == UvmTargetVaModeTargeted) {
        NvU64 va = params->va >> 12;

        NvU32 va_lo = va & HWMASK(C96F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
        NvU32 va_hi = va >> HWSIZE(C96F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);

        NV_PUSH_4U(C96F, MEM_OP_A, sysmembar_value |
                                   HWCONST(C96F, MEM_OP_A, TLB_INVALIDATE_INVAL_SCOPE, NON_LINK_TLBS) |
                                   HWVALUE(C96F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, va_lo),
                         MEM_OP_B, HWVALUE(C96F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, va_hi),
                         MEM_OP_C, HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                                   HWVALUE(C96F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                                   HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                                   HWVALUE(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                                   invalidate_gpc_value |
                                   aperture_value |
                                   ack_value,
                         MEM_OP_D, HWCONST(C96F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                                   HWVALUE(C96F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
    }
    else {
        NV_PUSH_4U(C96F, MEM_OP_A, sysmembar_value |
                                   HWCONST(C96F, MEM_OP_A, TLB_INVALIDATE_INVAL_SCOPE, NON_LINK_TLBS),
                         MEM_OP_B, 0,
                         MEM_OP_C, HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                                   HWVALUE(C96F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                                   HWCONST(C96F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                                   HWVALUE(C96F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                                   invalidate_gpc_value |
                                   aperture_value |
                                   ack_value,
                         MEM_OP_D, HWCONST(C96F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE) |
                                   HWVALUE(C96F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
    }
}
