/*******************************************************************************
    Copyright (c) 2015-2023 NVIDIA Corporation

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


// For Pascal, UVM page tree 'depth' maps to hardware as follows:
//
// UVM depth   HW level                            VA bits
// 0           PDE3                                48:47
// 1           PDE2                                46:38
// 2           PDE1                                37:29
// 3           PDE0 (dual 64k/4k PDE, or 2M PTE)   28:21
// 4           PTE_64K / PTE_4K                    20:16 / 20:12

#include "uvm_types.h"
#include "uvm_forward_decl.h"
#include "nv_uvm_interface.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_mmu.h"
#include "uvm_hal.h"
#include "uvm_push_macros.h"
#include "uvm_pascal_fault_buffer.h"
#include "hwref/pascal/gp100/dev_fault.h"
#include "hwref/pascal/gp100/dev_fb.h"
#include "hwref/pascal/gp100/dev_mmu.h"

#define MMU_BIG 0
#define MMU_SMALL 1

static NvU32 entries_per_index_pascal(NvU32 depth)
{
    UVM_ASSERT(depth < 5);
    if (depth == 3)
        return 2;
    return 1;
}

static NvLength entry_offset_pascal(NvU32 depth, NvU64 page_size)
{
    UVM_ASSERT(depth < 5);
    if (page_size == UVM_PAGE_SIZE_4K && depth == 3)
        return MMU_SMALL;
    return MMU_BIG;
}

static NvU64 single_pde_pascal(uvm_mmu_page_table_alloc_t *phys_alloc)
{
    NvU64 pde_bits = 0;

    if (phys_alloc != NULL) {
        NvU64 address = phys_alloc->addr.address >> NV_MMU_VER2_PDE_ADDRESS_SHIFT;
        pde_bits |= HWCONST64(_MMU_VER2, PDE, IS_PDE, TRUE) |
                    HWCONST64(_MMU_VER2, PDE, VOL, TRUE);

        switch (phys_alloc->addr.aperture) {
            case UVM_APERTURE_SYS:
                pde_bits |= HWCONST64(_MMU_VER2, PDE, APERTURE, SYSTEM_COHERENT_MEMORY) |
                            HWVALUE64(_MMU_VER2, PDE, ADDRESS_SYS, address);
                break;
            case UVM_APERTURE_VID:
                pde_bits |= HWCONST64(_MMU_VER2, PDE, APERTURE, VIDEO_MEMORY) |
                            HWVALUE64(_MMU_VER2, PDE, ADDRESS_VID, address);
                break;
            default:
                UVM_ASSERT_MSG(0, "Invalid aperture: %d\n", phys_alloc->addr.aperture);
                break;
        }
    }

    return pde_bits;
}

static NvU64 big_half_pde_pascal(uvm_mmu_page_table_alloc_t *phys_alloc)
{
    NvU64 pde_bits = 0;

    if (phys_alloc != NULL) {
        NvU64 address = phys_alloc->addr.address >> NV_MMU_VER2_DUAL_PDE_ADDRESS_BIG_SHIFT;
        pde_bits |= HWCONST64(_MMU_VER2, DUAL_PDE, VOL_BIG, TRUE);

        switch (phys_alloc->addr.aperture) {
            case UVM_APERTURE_SYS:
                pde_bits |= HWCONST64(_MMU_VER2, DUAL_PDE, APERTURE_BIG, SYSTEM_COHERENT_MEMORY) |
                            HWVALUE64(_MMU_VER2, DUAL_PDE, ADDRESS_BIG_SYS, address);
                break;
            case UVM_APERTURE_VID:
                pde_bits |= HWCONST64(_MMU_VER2, DUAL_PDE, APERTURE_BIG, VIDEO_MEMORY) |
                            HWVALUE64(_MMU_VER2, DUAL_PDE, ADDRESS_BIG_VID, address);
                break;
            default:
                UVM_ASSERT_MSG(0, "Invalid big aperture %d\n", phys_alloc->addr.aperture);
                break;
        }
    }

    return pde_bits;
}

static NvU64 small_half_pde_pascal(uvm_mmu_page_table_alloc_t *phys_alloc)
{
    NvU64 pde_bits = 0;

    if (phys_alloc != NULL) {
        NvU64 address = phys_alloc->addr.address >> NV_MMU_VER2_DUAL_PDE_ADDRESS_SHIFT;
        pde_bits |= HWCONST64(_MMU_VER2, DUAL_PDE, VOL_SMALL, TRUE);

        switch (phys_alloc->addr.aperture) {
            case UVM_APERTURE_SYS:
                pde_bits |= HWCONST64(_MMU_VER2, DUAL_PDE, APERTURE_SMALL, SYSTEM_COHERENT_MEMORY);
                pde_bits |= HWVALUE64(_MMU_VER2, DUAL_PDE, ADDRESS_SMALL_SYS, address);
                break;
            case UVM_APERTURE_VID:
                pde_bits |= HWCONST64(_MMU_VER2, DUAL_PDE, APERTURE_SMALL, VIDEO_MEMORY);
                pde_bits |= HWVALUE64(_MMU_VER2, DUAL_PDE, ADDRESS_SMALL_VID, address);
                break;
            default:
                UVM_ASSERT_MSG(0, "Invalid small aperture %d\n", phys_alloc->addr.aperture);
                break;
        }
    }

    return pde_bits;
}

static void make_pde_pascal(void *entry,
                            uvm_mmu_page_table_alloc_t **phys_allocs,
                            uvm_page_directory_t *dir,
                            NvU32 child_index)
{
    NvU32 entry_count;
    NvU64 *entry_bits = (NvU64 *)entry;

    UVM_ASSERT(dir);

    entry_count = entries_per_index_pascal(dir->depth);

    if (entry_count == 1) {
        *entry_bits = single_pde_pascal(*phys_allocs);
    }
    else if (entry_count == 2) {
        entry_bits[MMU_BIG] = big_half_pde_pascal(phys_allocs[MMU_BIG]);
        entry_bits[MMU_SMALL] = small_half_pde_pascal(phys_allocs[MMU_SMALL]);

        // This entry applies to the whole dual PDE but is stored in the lower
        // bits.
        entry_bits[MMU_BIG] |= HWCONST64(_MMU_VER2, DUAL_PDE, IS_PDE, TRUE);
    }
    else {
        UVM_ASSERT_MSG(0, "Invalid number of entries per index: %d\n", entry_count);
    }
}

static NvLength entry_size_pascal(NvU32 depth)
{
    UVM_ASSERT(depth < 5);
    if (depth == 3)
        return 16;
    else
        return 8;
}

static NvU32 index_bits_pascal(NvU32 depth, NvU64 page_size)
{
    static const NvU32 bit_widths[] = {2, 9, 9, 8};
    // some code paths keep on querying this until they get a 0, meaning only the page offset remains.
    UVM_ASSERT(depth < 5);
    if (depth < 4) {
        return bit_widths[depth];
    }
    else if (depth == 4) {
        switch (page_size) {
            case UVM_PAGE_SIZE_4K:
                return 9;
            case UVM_PAGE_SIZE_64K:
                return 5;
            default:
                break;
        }
    }
    return 0;
}

static NvU32 num_va_bits_pascal(void)
{
    return 49;
}

static NvLength allocation_size_pascal(NvU32 depth, NvU64 page_size)
{
    UVM_ASSERT(depth < 5);
    if (depth == 4 && page_size == UVM_PAGE_SIZE_64K)
        return 256;
    // depth 0 requires only a 32 byte allocation, but it must be 4k aligned
    return 4096;
}

static NvU32 page_table_depth_pascal(NvU64 page_size)
{
    if (page_size == UVM_PAGE_SIZE_2M)
        return 3;
    else
        return 4;
}

static NvU64 page_sizes_pascal(void)
{
    return UVM_PAGE_SIZE_2M | UVM_PAGE_SIZE_64K | UVM_PAGE_SIZE_4K;
}

static NvU64 unmapped_pte_pascal(NvU64 page_size)
{
    // Setting the privilege bit on an otherwise-zeroed big PTE causes the
    // corresponding 4k PTEs to be ignored. This allows the invalidation of a
    // mixed PDE range to be much faster.
    if (page_size != UVM_PAGE_SIZE_64K)
        return 0;

    // When VALID == 0, MMU still reads the VOL and PRIV fields. VOL == 1
    // indicates that the PTE is sparse, so make sure we don't use it.
    return HWCONST64(_MMU_VER2, PTE, VALID,     FALSE) |
           HWCONST64(_MMU_VER2, PTE, VOL,       FALSE) |
           HWCONST64(_MMU_VER2, PTE, PRIVILEGE, TRUE);
}

static NvU64 make_pte_pascal(uvm_aperture_t aperture, NvU64 address, uvm_prot_t prot, NvU64 flags)
{
    NvU8 aperture_bits = 0;
    NvU64 pte_bits = 0;

    UVM_ASSERT(prot != UVM_PROT_NONE);
    UVM_ASSERT((flags & ~UVM_MMU_PTE_FLAGS_MASK) == 0);

    // valid 0:0
    pte_bits |= HWCONST64(_MMU_VER2, PTE, VALID, TRUE);

    // aperture 2:1
    if (aperture == UVM_APERTURE_SYS)
        aperture_bits = NV_MMU_VER2_PTE_APERTURE_SYSTEM_COHERENT_MEMORY;
    else if (aperture == UVM_APERTURE_VID)
        aperture_bits = NV_MMU_VER2_PTE_APERTURE_VIDEO_MEMORY;
    else if (aperture >= UVM_APERTURE_PEER_0 && aperture <= UVM_APERTURE_PEER_7)
        aperture_bits = NV_MMU_VER2_PTE_APERTURE_PEER_MEMORY;
    else
        UVM_ASSERT_MSG(0, "Invalid aperture: %d\n", aperture);

    pte_bits |= HWVALUE64(_MMU_VER2, PTE, APERTURE, aperture_bits);

    // volatile 3:3
    if (flags & UVM_MMU_PTE_FLAGS_CACHED)
        pte_bits |= HWCONST64(_MMU_VER2, PTE, VOL, FALSE);
    else
        pte_bits |= HWCONST64(_MMU_VER2, PTE, VOL, TRUE);

    // encrypted 4:4
    pte_bits |= HWCONST64(_MMU_VER2, PTE, ENCRYPTED, FALSE);

    // privilege 5:5
    pte_bits |= HWCONST64(_MMU_VER2, PTE, PRIVILEGE, FALSE);

    // read only 6:6
    if (prot == UVM_PROT_READ_ONLY)
        pte_bits |= HWCONST64(_MMU_VER2, PTE, READ_ONLY, TRUE);
    else
        pte_bits |= HWCONST64(_MMU_VER2, PTE, READ_ONLY, FALSE);

    // atomic disable 7:7
    if (prot == UVM_PROT_READ_WRITE_ATOMIC)
        pte_bits |= HWCONST64(_MMU_VER2, PTE, ATOMIC_DISABLE, FALSE);
    else
        pte_bits |= HWCONST64(_MMU_VER2, PTE, ATOMIC_DISABLE, TRUE);

    address >>= NV_MMU_VER2_PTE_ADDRESS_SHIFT;
    if (aperture == UVM_APERTURE_SYS) {
        // sys address 53:8
        pte_bits |= HWVALUE64(_MMU_VER2, PTE, ADDRESS_SYS, address);
    }
    else {
        // vid address 32:8
        pte_bits |= HWVALUE64(_MMU_VER2, PTE, ADDRESS_VID, address);


        // peer id 35:33
        if (aperture != UVM_APERTURE_VID)
            pte_bits |= HWVALUE64(_MMU_VER2, PTE, ADDRESS_VID_PEER, UVM_APERTURE_PEER_ID(aperture));

        // comptagline 53:36
        pte_bits |= HWVALUE64(_MMU_VER2, PTE, COMPTAGLINE, 0);
    }

    pte_bits |= HWVALUE64(_MMU_VER2, PTE, KIND, NV_MMU_PTE_KIND_PITCH);

    return pte_bits;
}

static NvU64 make_sked_reflected_pte_pascal(void)
{
    NvU64 pte_bits = 0;

    pte_bits |= HWCONST64(_MMU_VER2, PTE, VALID, TRUE);
    pte_bits |= HWVALUE64(_MMU_VER2, PTE, KIND, NV_MMU_PTE_KIND_SMSKED_MESSAGE);

    return pte_bits;
}

static NvU64 make_sparse_pte_pascal(void)
{
    return HWCONST64(_MMU_VER2, PTE, VALID, FALSE) |
           HWCONST64(_MMU_VER2, PTE, VOL,   TRUE);
}

static NvU64 poisoned_pte_pascal(void)
{
    // An invalid PTE won't be fatal from faultable units like SM, which is the
    // most likely source of bad PTE accesses.

    // Engines with priv accesses won't fault on the priv PTE, so add a backup
    // mechanism using an impossible memory address. MMU will trigger an
    // interrupt when it detects a bad physical address.
    //
    // This address has to fit within 37 bits (max address width of vidmem) and
    // be aligned to page_size.
    NvU64 phys_addr = 0x1bad000000ULL;

    NvU64 pte_bits = make_pte_pascal(UVM_APERTURE_VID, phys_addr, UVM_PROT_READ_ONLY, UVM_MMU_PTE_FLAGS_NONE);
    return WRITE_HWCONST64(pte_bits, _MMU_VER2, PTE, PRIVILEGE, TRUE);
}

static uvm_mmu_mode_hal_t pascal_mmu_mode_hal =
{
    .make_pte = make_pte_pascal,
    .make_sked_reflected_pte = make_sked_reflected_pte_pascal,
    .make_sparse_pte = make_sparse_pte_pascal,
    .unmapped_pte = unmapped_pte_pascal,
    .poisoned_pte = poisoned_pte_pascal,
    .make_pde = make_pde_pascal,
    .entry_size = entry_size_pascal,
    .index_bits = index_bits_pascal,
    .entries_per_index = entries_per_index_pascal,
    .entry_offset = entry_offset_pascal,
    .num_va_bits = num_va_bits_pascal,
    .allocation_size = allocation_size_pascal,
    .page_table_depth = page_table_depth_pascal,
    .page_sizes = page_sizes_pascal
};

uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_pascal(NvU64 big_page_size)
{
    UVM_ASSERT(big_page_size == UVM_PAGE_SIZE_64K || big_page_size == UVM_PAGE_SIZE_128K);

    // TODO: Bug 1789555: RM should reject the creation of GPU VA spaces with
    // 128K big page size for Pascal+ GPUs
    if (big_page_size == UVM_PAGE_SIZE_128K)
        return NULL;

    return &pascal_mmu_mode_hal;
}

static void mmu_set_prefetch_faults(uvm_parent_gpu_t *parent_gpu, bool enable)
{
    volatile NvU32 *prefetch_ctrl = parent_gpu->fault_buffer_info.rm_info.replayable.pPrefetchCtrl;

    // A null prefetch control mapping indicates that UVM should toggle the
    // register's value using the RM API, instead of performing a direct access.
    if (prefetch_ctrl == NULL) {
        NV_STATUS status;

        // Access to the register is currently blocked only in Confidential
        // Computing.
        UVM_ASSERT(g_uvm_global.conf_computing_enabled);
        status = nvUvmInterfaceTogglePrefetchFaults(&parent_gpu->fault_buffer_info.rm_info, (NvBool)enable);

        UVM_ASSERT(status == NV_OK);
    }
    else {
        NvU32 prefetch_ctrl_value = UVM_GPU_READ_ONCE(*prefetch_ctrl);

        if (enable)
            prefetch_ctrl_value = WRITE_HWCONST(prefetch_ctrl_value, _PFB_PRI_MMU_PAGE, FAULT_CTRL, PRF_FILTER, SEND_ALL);
        else
            prefetch_ctrl_value = WRITE_HWCONST(prefetch_ctrl_value, _PFB_PRI_MMU_PAGE, FAULT_CTRL, PRF_FILTER, SEND_NONE);

        UVM_GPU_WRITE_ONCE(*prefetch_ctrl, prefetch_ctrl_value);
    }
}

void uvm_hal_pascal_mmu_enable_prefetch_faults(uvm_parent_gpu_t *parent_gpu)
{
    UVM_ASSERT(parent_gpu->prefetch_fault_supported);

    mmu_set_prefetch_faults(parent_gpu, true);
}

void uvm_hal_pascal_mmu_disable_prefetch_faults(uvm_parent_gpu_t *parent_gpu)
{
    mmu_set_prefetch_faults(parent_gpu, false);
}

NvU16 uvm_hal_pascal_mmu_client_id_to_utlb_id(NvU16 client_id)
{
    switch (client_id) {
        case NV_PFAULT_CLIENT_GPC_RAST:
        case NV_PFAULT_CLIENT_GPC_GCC:
        case NV_PFAULT_CLIENT_GPC_GPCCS:
            return UVM_PASCAL_GPC_UTLB_ID_RGG;
        case NV_PFAULT_CLIENT_GPC_PE_0:
        case NV_PFAULT_CLIENT_GPC_TPCCS_0:
        case NV_PFAULT_CLIENT_GPC_L1_0:
        case NV_PFAULT_CLIENT_GPC_T1_0:
        case NV_PFAULT_CLIENT_GPC_L1_1:
        case NV_PFAULT_CLIENT_GPC_T1_1:
            return UVM_PASCAL_GPC_UTLB_ID_LTP0;
        case NV_PFAULT_CLIENT_GPC_PE_1:
        case NV_PFAULT_CLIENT_GPC_TPCCS_1:
        case NV_PFAULT_CLIENT_GPC_L1_2:
        case NV_PFAULT_CLIENT_GPC_T1_2:
        case NV_PFAULT_CLIENT_GPC_L1_3:
        case NV_PFAULT_CLIENT_GPC_T1_3:
            return UVM_PASCAL_GPC_UTLB_ID_LTP1;
        case NV_PFAULT_CLIENT_GPC_PE_2:
        case NV_PFAULT_CLIENT_GPC_TPCCS_2:
        case NV_PFAULT_CLIENT_GPC_L1_4:
        case NV_PFAULT_CLIENT_GPC_T1_4:
        case NV_PFAULT_CLIENT_GPC_L1_5:
        case NV_PFAULT_CLIENT_GPC_T1_5:
            return UVM_PASCAL_GPC_UTLB_ID_LTP2;
        case NV_PFAULT_CLIENT_GPC_PE_3:
        case NV_PFAULT_CLIENT_GPC_TPCCS_3:
        case NV_PFAULT_CLIENT_GPC_L1_6:
        case NV_PFAULT_CLIENT_GPC_T1_6:
        case NV_PFAULT_CLIENT_GPC_L1_7:
        case NV_PFAULT_CLIENT_GPC_T1_7:
            return UVM_PASCAL_GPC_UTLB_ID_LTP3;
        case NV_PFAULT_CLIENT_GPC_PE_4:
        case NV_PFAULT_CLIENT_GPC_TPCCS_4:
        case NV_PFAULT_CLIENT_GPC_L1_8:
        case NV_PFAULT_CLIENT_GPC_T1_8:
        case NV_PFAULT_CLIENT_GPC_L1_9:
        case NV_PFAULT_CLIENT_GPC_T1_9:
            return UVM_PASCAL_GPC_UTLB_ID_LTP4;
        default:
            UVM_ASSERT_MSG(false, "Invalid client value: 0x%x\n", client_id);
    }

    return 0;
}
