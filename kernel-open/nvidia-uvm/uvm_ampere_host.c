/*******************************************************************************
    Copyright (c) 2018-2024 NVIDIA Corporation

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
#include "uvm_global.h"
#include "uvm_user_channel.h"
#include "uvm_push_macros.h"
#include "hwref/ampere/ga100/dev_runlist.h"
#include "clc56f.h"
#include "clc076.h"

bool uvm_hal_ampere_host_method_is_valid(uvm_push_t *push, NvU32 method_address, NvU32 method_data)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    if (!uvm_parent_gpu_is_virt_mode_sriov_heavy(gpu->parent))
        return true;

    UVM_ASSERT(push->channel);

    if (uvm_channel_is_privileged(push->channel)) {
        switch (method_address) {
            case NVC56F_SET_OBJECT:
            case NVC56F_NON_STALL_INTERRUPT:
            case NVC56F_MEM_OP_A:
            case NVC56F_MEM_OP_B:
            case NVC56F_MEM_OP_C:
            case NVC56F_MEM_OP_D:
            case NVC56F_SEM_ADDR_LO:
            case NVC56F_SEM_ADDR_HI:
            case NVC56F_SEM_PAYLOAD_LO:
            case NVC56F_SEM_PAYLOAD_HI:
            case NVC56F_SEM_EXECUTE:
            case NVC56F_WFI:
            case NVC56F_NOP:
                return true;
        }

        UVM_ERR_PRINT("Unsupported Host method 0x%x\n", method_address);
        return false;
    }
    else if (method_address == NVC56F_MEM_OP_D) {
        NvU32 operation = READ_HWVALUE(method_data, C56F, MEM_OP_D, OPERATION);

        // Prohibit privileged operations from being pushed to non-privileged
        // channels.

        // TLB invalidations.
        if ((operation == NVC56F_MEM_OP_D_OPERATION_MMU_TLB_INVALIDATE) ||
            (operation == NVC56F_MEM_OP_D_OPERATION_MMU_TLB_INVALIDATE_TARGETED)) {
            UVM_ERR_PRINT("Pushed privileged operation 0x%x to non-privileged channel\n", operation);
            return false;
        }

        // Access counter clearing is a privileged operation. But access
        // counters are not supported on SR-IOV heavy, so the presence of the
        // operation indicates a missing check for access counters support.
        if (operation == NVC56F_MEM_OP_D_OPERATION_ACCESS_COUNTER_CLR) {
            UVM_ERR_PRINT("Pushed access counters operation 0x%x, but access counters are not supported\n", operation);
            return false;
        }
   }

   return true;
}

bool uvm_hal_ampere_host_sw_method_is_valid(uvm_push_t *push, NvU32 method_address, NvU32 method_data)
{
    UVM_ASSERT(push->channel);

    if (!uvm_channel_is_proxy(push->channel))
        return true;

    switch (method_address) {
        case NVC076_SET_OBJECT:
        case NVC076_CLEAR_FAULTED_A:
        case NVC076_CLEAR_FAULTED_B:
        case NVC076_FAULT_CANCEL_A:
        case NVC076_FAULT_CANCEL_B:
        case NVC076_FAULT_CANCEL_C:
            return true;
    }

    UVM_ERR_PRINT("Unsupported SW method 0x%x\n", method_address);
    return false;
}

void uvm_hal_ampere_host_clear_faulted_channel_register(uvm_user_channel_t *user_channel,
                                                        const uvm_fault_buffer_entry_t *fault)
{
    uvm_spin_loop_t spin;
    NvU32 channel_faulted_mask = 0;
    NvU32 clear_type_value = 0;
    NvU32 doorbell_value = 0;
    volatile NvU32 *doorbell_ptr;

    UVM_ASSERT(!user_channel->gpu->parent->has_clear_faulted_channel_method);

    if (fault->fault_source.mmu_engine_type == UVM_MMU_ENGINE_TYPE_HOST) {
        clear_type_value = NV_CHRAM_CHANNEL_UPDATE_RESET_PBDMA_FAULTED;
        channel_faulted_mask = HWCONST(_CHRAM, CHANNEL, PBDMA_FAULTED, TRUE);
    }
    else if (fault->fault_source.mmu_engine_type == UVM_MMU_ENGINE_TYPE_CE) {
        clear_type_value = NV_CHRAM_CHANNEL_UPDATE_RESET_ENG_FAULTED;
        channel_faulted_mask = HWCONST(_CHRAM, CHANNEL, ENG_FAULTED, TRUE);
    }
    else {
        UVM_ASSERT_MSG(false, "Unsupported MMU engine type %s\n",
                       uvm_mmu_engine_type_string(fault->fault_source.mmu_engine_type));
    }

    doorbell_ptr = (NvU32 *)((NvU8 *)user_channel->runlist_pri_base_register + NV_RUNLIST_INTERNAL_DOORBELL);

    // GFID is not required since we clear faulted channel with a SW method on
    // SRIOV. On baremetal, GFID is always zero.
    doorbell_value = HWVALUE(_RUNLIST, INTERNAL_DOORBELL, CHID, user_channel->hw_channel_id);

    // Wait for the channel to have the FAULTED bit set as this can race with
    // interrupt notification
    UVM_SPIN_WHILE(!(UVM_GPU_READ_ONCE(*user_channel->chram_channel_register) & channel_faulted_mask), &spin);

    UVM_GPU_WRITE_ONCE(*user_channel->chram_channel_register, clear_type_value);

    wmb();

    UVM_GPU_WRITE_ONCE(*doorbell_ptr, doorbell_value);
}

static NvU32 instance_ptr_aperture_type_to_hw_value(uvm_aperture_t aperture)
{
    switch (aperture) {
        case UVM_APERTURE_SYS:
            return HWCONST(C076, CLEAR_FAULTED_A, INST_APERTURE, SYS_MEM_COHERENT);
        case UVM_APERTURE_VID:
            return HWCONST(C076, CLEAR_FAULTED_A, INST_APERTURE, VID_MEM);
        default:
            UVM_ASSERT_MSG(false, "Invalid aperture_type %d\n", aperture);
    }

    return 0;
}

static void instance_ptr_address_to_hw_values(NvU64 instance_ptr_address,
                                              NvU32 *instance_ptr_lo,
                                              NvU32 *instance_ptr_hi)
{
    // instance_ptr must be 4K aligned
    UVM_ASSERT_MSG(IS_ALIGNED(instance_ptr_address, 1 << 12), "instance_ptr 0x%llx\n", instance_ptr_address);
    instance_ptr_address >>= 12;

    *instance_ptr_lo = instance_ptr_address & HWMASK(C076, CLEAR_FAULTED_A, INST_LOW);
    *instance_ptr_hi = instance_ptr_address >> HWSIZE(C076, CLEAR_FAULTED_A, INST_LOW);
}

static NvU32 mmu_engine_type_to_hw_value(uvm_mmu_engine_type_t mmu_engine_type)
{
    switch (mmu_engine_type) {
        case UVM_MMU_ENGINE_TYPE_HOST:
            return HWCONST(C076, CLEAR_FAULTED_A, TYPE, PBDMA_FAULTED);
        case UVM_MMU_ENGINE_TYPE_CE:
            return HWCONST(C076, CLEAR_FAULTED_A, TYPE, ENG_FAULTED);
        default:
            UVM_ASSERT_MSG(false, "Unsupported MMU engine type %s\n",
                       uvm_mmu_engine_type_string(mmu_engine_type));
    }

    return 0;
}

void uvm_hal_ampere_host_clear_faulted_channel_sw_method(uvm_push_t *push,
                                                         uvm_user_channel_t *user_channel,
                                                         const uvm_fault_buffer_entry_t *fault)
{
    NvU32 clear_type_value;
    NvU32 aperture_type_value;
    NvU32 instance_ptr_lo, instance_ptr_hi;
    uvm_gpu_phys_address_t instance_ptr = user_channel->instance_ptr.addr;

    UVM_ASSERT(user_channel->gpu->parent->has_clear_faulted_channel_sw_method);

    clear_type_value = mmu_engine_type_to_hw_value(fault->fault_source.mmu_engine_type);
    aperture_type_value = instance_ptr_aperture_type_to_hw_value(instance_ptr.aperture);

    instance_ptr_address_to_hw_values(instance_ptr.address, &instance_ptr_lo, &instance_ptr_hi);

    NV_PUSH_2U(C076, CLEAR_FAULTED_A, HWVALUE(C076, CLEAR_FAULTED_A, INST_LOW, instance_ptr_lo) |
                                      aperture_type_value |
                                      clear_type_value,
                     CLEAR_FAULTED_B, HWVALUE(C076, CLEAR_FAULTED_B, INST_HI, instance_ptr_hi));
}

// Copy from Turing, this version sets TLB_INVALIDATE_INVAL_SCOPE.
void uvm_hal_ampere_host_tlb_invalidate_all(uvm_push_t *push,
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
        aperture_value = HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    // PDE3 is the highest level on Pascal-Ampere, see the comment in
    // uvm_pascal_mmu.c for details.
    UVM_ASSERT_MSG(depth < NVC56F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3, "depth %u", depth);
    page_table_level = NVC56F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3 - depth;

    if (membar != UVM_MEMBAR_NONE) {
        // If a GPU or SYS membar is needed, ACK_TYPE needs to be set to
        // GLOBALLY to make sure all the pending accesses can be picked up by
        // the membar.
        ack_value = HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);
    }

    if (membar == UVM_MEMBAR_SYS)
        sysmembar_value = HWCONST(C56F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, EN);
    else
        sysmembar_value = HWCONST(C56F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS);

    NV_PUSH_4U(C56F, MEM_OP_A, sysmembar_value |
                               HWCONST(C56F, MEM_OP_A, TLB_INVALIDATE_INVAL_SCOPE, NON_LINK_TLBS),
                     MEM_OP_B, 0,
                     MEM_OP_C, HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWVALUE(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                               HWVALUE(C56F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                               aperture_value |
                               ack_value,
                     MEM_OP_D, HWCONST(C56F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE) |
                               HWVALUE(C56F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));

    // GPU membar still requires an explicit membar method.
    if (membar == UVM_MEMBAR_GPU)
        uvm_push_get_gpu(push)->parent->host_hal->membar_gpu(push);
}

// Copy from Turing, this version sets TLB_INVALIDATE_INVAL_SCOPE.
void uvm_hal_ampere_host_tlb_invalidate_va(uvm_push_t *push,
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
    va_lo = base & HWMASK(C56F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
    va_hi = base >> HWSIZE(C56F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);

    UVM_ASSERT_MSG(pdb.aperture == UVM_APERTURE_VID || pdb.aperture == UVM_APERTURE_SYS, "aperture: %u", pdb.aperture);

    if (pdb.aperture == UVM_APERTURE_VID)
        aperture_value = HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    // PDE3 is the highest level on Pascal-Ampere, see the comment in
    // uvm_pascal_mmu.c for details.
    UVM_ASSERT_MSG(depth < NVC56F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3, "depth %u", depth);
    page_table_level = NVC56F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3 - depth;

    if (membar != UVM_MEMBAR_NONE) {
        // If a GPU or SYS membar is needed, ACK_TYPE needs to be set to
        // GLOBALLY to make sure all the pending accesses can be picked up by
        // the membar.
        ack_value = HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);
    }

    if (membar == UVM_MEMBAR_SYS)
        sysmembar_value = HWCONST(C56F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, EN);
    else
        sysmembar_value = HWCONST(C56F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS);

    NV_PUSH_4U(C56F, MEM_OP_A, HWVALUE(C56F, MEM_OP_A, TLB_INVALIDATE_INVALIDATION_SIZE, log2_invalidation_size) |
                               HWCONST(C56F, MEM_OP_A, TLB_INVALIDATE_INVAL_SCOPE, NON_LINK_TLBS) |
                               sysmembar_value |
                               HWVALUE(C56F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, va_lo),
                     MEM_OP_B, HWVALUE(C56F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, va_hi),
                     MEM_OP_C, HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWVALUE(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                               HWVALUE(C56F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                               aperture_value |
                               ack_value,
                     MEM_OP_D, HWCONST(C56F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                               HWVALUE(C56F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));

    // GPU membar still requires an explicit membar method.
    if (membar == UVM_MEMBAR_GPU)
        gpu->parent->host_hal->membar_gpu(push);
}

// Copy from Turing, this version sets TLB_INVALIDATE_INVAL_SCOPE.
void uvm_hal_ampere_host_tlb_invalidate_test(uvm_push_t *push,
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
        aperture_value = HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    if (params->page_table_level != UvmInvalidatePageTableLevelAll) {
        // PDE3 is the highest level on Pascal-Ampere, see the comment in
        // uvm_pascal_mmu.c for details.
        page_table_level = min((NvU32)UvmInvalidatePageTableLevelPde3, params->page_table_level) - 1;
    }

    if (params->membar != UvmInvalidateTlbMemBarNone) {
        // If a GPU or SYS membar is needed, ack_value needs to be set to
        // GLOBALLY to make sure all the pending accesses can be picked up by
        // the membar.
        ack_value = HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);
    }

    if (params->membar == UvmInvalidateTlbMemBarSys)
        sysmembar_value = HWCONST(C56F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, EN);
    else
        sysmembar_value = HWCONST(C56F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS);

    if (params->disable_gpc_invalidate)
        invalidate_gpc_value = HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_GPC, DISABLE);
    else
        invalidate_gpc_value = HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE);

    if (params->target_va_mode == UvmTargetVaModeTargeted) {
        NvU64 va = params->va >> 12;

        NvU32 va_lo = va & HWMASK(C56F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
        NvU32 va_hi = va >> HWSIZE(C56F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
        NV_PUSH_4U(C56F, MEM_OP_A, sysmembar_value |
                                   HWCONST(C56F, MEM_OP_A, TLB_INVALIDATE_INVAL_SCOPE, NON_LINK_TLBS) |
                                   HWVALUE(C56F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, va_lo),
                         MEM_OP_B, HWVALUE(C56F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, va_hi),
                         MEM_OP_C, HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                                   HWVALUE(C56F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                                   HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                                   HWVALUE(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                                   invalidate_gpc_value |
                                   aperture_value |
                                   ack_value,
                         MEM_OP_D, HWCONST(C56F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                                   HWVALUE(C56F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
    }
    else {
        NV_PUSH_4U(C56F, MEM_OP_A, sysmembar_value |
                                   HWCONST(C56F, MEM_OP_A, TLB_INVALIDATE_INVAL_SCOPE, NON_LINK_TLBS),
                         MEM_OP_B, 0,
                         MEM_OP_C, HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                                   HWVALUE(C56F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                                   HWCONST(C56F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                                   HWVALUE(C56F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                                   invalidate_gpc_value |
                                   aperture_value |
                                   ack_value,
                         MEM_OP_D, HWCONST(C56F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE) |
                                   HWVALUE(C56F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
    }

    // GPU membar still requires an explicit membar method.
    if (params->membar == UvmInvalidateTlbMemBarLocal)
        uvm_push_get_gpu(push)->parent->host_hal->membar_gpu(push);
}
