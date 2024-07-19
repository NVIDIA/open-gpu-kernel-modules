/*******************************************************************************
    Copyright (c) 2017-2024 NVIDIA Corporation

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

#include "uvm_types.h"
#include "uvm_forward_decl.h"
#include "uvm_global.h"
#include "uvm_hal.h"
#include "uvm_mmu.h"
#include "uvm_volta_fault_buffer.h"
#include "hwref/volta/gv100/dev_mmu.h"
#include "hwref/volta/gv100/dev_fault.h"

// Direct copy of make_pde_pascal and helpers, but adds NO_ATS in PDE1
#define MMU_BIG 0
#define MMU_SMALL 1

static NvU32 entries_per_index_volta(NvU32 depth)
{
    UVM_ASSERT(depth < 5);
    if (depth == 3)
        return 2;
    return 1;
}

static NvLength entry_offset_volta(NvU32 depth, NvU64 page_size)
{
    UVM_ASSERT(depth < 5);
    if (page_size == UVM_PAGE_SIZE_4K && depth == 3)
        return MMU_SMALL;
    return MMU_BIG;
}

static NvU64 single_pde_volta(uvm_mmu_page_table_alloc_t *phys_alloc, NvU32 depth)
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

        // Volta GPUs on ATS-enabled systems, perform a parallel lookup on both
        // ATS and GMMU page tables. For managed memory we need to prevent this
        // parallel lookup since we would not get any GPU fault if the CPU has
        // a valid mapping. Also, for external ranges that are known to be
        // mapped entirely on the GMMU page table we can skip the ATS lookup
        // for performance reasons. This bit is set in PDE1 (depth 2) and,
        // therefore, it applies to the underlying 512MB VA range.
        //
        // UVM sets NO_ATS for all Volta+ mappings on ATS systems. This is fine
        // because CUDA ensures that all managed and external allocations are
        // properly compartmentalized in 512MB-aligned VA regions. For
        // cudaHostRegister CUDA cannot control the VA range, but we rely on
        // ATS for those allocations so they can't use the NO_ATS bit.
        if (depth == 2 && g_uvm_global.ats.enabled)
            pde_bits |= HWCONST64(_MMU_VER2, PDE, NO_ATS, TRUE);
    }

    return pde_bits;
}

static NvU64 big_half_pde_volta(uvm_mmu_page_table_alloc_t *phys_alloc)
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

static NvU64 small_half_pde_volta(uvm_mmu_page_table_alloc_t *phys_alloc)
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

static void make_pde_volta(void *entry,
                           uvm_mmu_page_table_alloc_t **phys_allocs,
                           uvm_page_directory_t *dir,
                           NvU32 child_index)
{
    NvU32 entry_count;
    NvU64 *entry_bits = (NvU64 *)entry;

    UVM_ASSERT(dir);

    entry_count = entries_per_index_volta(dir->depth);

    if (entry_count == 1) {
        *entry_bits = single_pde_volta(*phys_allocs, dir->depth);
    }
    else if (entry_count == 2) {
        entry_bits[MMU_BIG] = big_half_pde_volta(phys_allocs[MMU_BIG]);
        entry_bits[MMU_SMALL] = small_half_pde_volta(phys_allocs[MMU_SMALL]);

        // This entry applies to the whole dual PDE but is stored in the lower
        // bits
        entry_bits[MMU_BIG] |= HWCONST64(_MMU_VER2, DUAL_PDE, IS_PDE, TRUE);
    }
    else {
        UVM_ASSERT_MSG(0, "Invalid number of entries per index: %d\n", entry_count);
    }
}

// Direct copy of make_pte_pascal, but adds the bits necessary for 47-bit
// physical addressing
static NvU64 make_pte_volta(uvm_aperture_t aperture, NvU64 address, uvm_prot_t prot, NvU64 flags)
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
        NvU64 addr_lo = address & HWMASK64(_MMU_VER2, PTE, ADDRESS_VID);
        NvU64 addr_hi = address >> HWSIZE(_MMU_VER2, PTE, ADDRESS_VID);

        // vid address 32:8 for bits 36:12 of the physical address
        pte_bits |= HWVALUE64(_MMU_VER2, PTE, ADDRESS_VID, addr_lo);

        // comptagline 53:36 - this can be overloaded in some cases to reference
        // a 47-bit physical address.  Currently, the only known cases of this
        // is for nvswitch, where peer id is the fabric id programmed for
        // such peer mappings
        pte_bits |= HWVALUE64(_MMU_VER2, PTE, COMPTAGLINE, addr_hi);

        // peer id 35:33
        if (aperture != UVM_APERTURE_VID)
            pte_bits |= HWVALUE64(_MMU_VER2, PTE, ADDRESS_VID_PEER, UVM_APERTURE_PEER_ID(aperture));
    }

    pte_bits |= HWVALUE64(_MMU_VER2, PTE, KIND, NV_MMU_PTE_KIND_PITCH);

    return pte_bits;
}

static uvm_mmu_mode_hal_t volta_mmu_mode_hal;

uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_volta(NvU64 big_page_size)
{
    static bool initialized = false;

    UVM_ASSERT(big_page_size == UVM_PAGE_SIZE_64K || big_page_size == UVM_PAGE_SIZE_128K);

    // TODO: Bug 1789555: RM should reject the creation of GPU VA spaces with
    // 128K big page size for Pascal+ GPUs
    if (big_page_size == UVM_PAGE_SIZE_128K)
        return NULL;

    if (!initialized) {
        uvm_mmu_mode_hal_t *pascal_mmu_mode_hal = uvm_hal_mmu_mode_pascal(big_page_size);
        UVM_ASSERT(pascal_mmu_mode_hal);

        // The assumption made is that arch_hal->mmu_mode_hal() will be
        // called under the global lock the first time, so check it here.
        uvm_assert_mutex_locked(&g_uvm_global.global_lock);

        volta_mmu_mode_hal = *pascal_mmu_mode_hal;
        volta_mmu_mode_hal.make_pte = make_pte_volta;
        volta_mmu_mode_hal.make_pde = make_pde_volta;

        initialized = true;
    }

    return &volta_mmu_mode_hal;
}

NvU16 uvm_hal_volta_mmu_client_id_to_utlb_id(NvU16 client_id)
{
    switch (client_id) {
        case NV_PFAULT_CLIENT_GPC_RAST:
        case NV_PFAULT_CLIENT_GPC_GCC:
        case NV_PFAULT_CLIENT_GPC_GPCCS:
            return UVM_VOLTA_GPC_UTLB_ID_RGG;
        case NV_PFAULT_CLIENT_GPC_PE_0:
        case NV_PFAULT_CLIENT_GPC_TPCCS_0:
        case NV_PFAULT_CLIENT_GPC_T1_0:
        case NV_PFAULT_CLIENT_GPC_T1_1:
            return UVM_VOLTA_GPC_UTLB_ID_LTP0;
        case NV_PFAULT_CLIENT_GPC_PE_1:
        case NV_PFAULT_CLIENT_GPC_TPCCS_1:
        case NV_PFAULT_CLIENT_GPC_T1_2:
        case NV_PFAULT_CLIENT_GPC_T1_3:
            return UVM_VOLTA_GPC_UTLB_ID_LTP1;
        case NV_PFAULT_CLIENT_GPC_PE_2:
        case NV_PFAULT_CLIENT_GPC_TPCCS_2:
        case NV_PFAULT_CLIENT_GPC_T1_4:
        case NV_PFAULT_CLIENT_GPC_T1_5:
            return UVM_VOLTA_GPC_UTLB_ID_LTP2;
        case NV_PFAULT_CLIENT_GPC_PE_3:
        case NV_PFAULT_CLIENT_GPC_TPCCS_3:
        case NV_PFAULT_CLIENT_GPC_T1_6:
        case NV_PFAULT_CLIENT_GPC_T1_7:
            return UVM_VOLTA_GPC_UTLB_ID_LTP3;
        case NV_PFAULT_CLIENT_GPC_PE_4:
        case NV_PFAULT_CLIENT_GPC_TPCCS_4:
        case NV_PFAULT_CLIENT_GPC_T1_8:
        case NV_PFAULT_CLIENT_GPC_T1_9:
            return UVM_VOLTA_GPC_UTLB_ID_LTP4;
        case NV_PFAULT_CLIENT_GPC_PE_5:
        case NV_PFAULT_CLIENT_GPC_TPCCS_5:
        case NV_PFAULT_CLIENT_GPC_T1_10:
        case NV_PFAULT_CLIENT_GPC_T1_11:
            return UVM_VOLTA_GPC_UTLB_ID_LTP5;
        case NV_PFAULT_CLIENT_GPC_PE_6:
        case NV_PFAULT_CLIENT_GPC_TPCCS_6:
        case NV_PFAULT_CLIENT_GPC_T1_12:
        case NV_PFAULT_CLIENT_GPC_T1_13:
            return UVM_VOLTA_GPC_UTLB_ID_LTP6;
        case NV_PFAULT_CLIENT_GPC_PE_7:
        case NV_PFAULT_CLIENT_GPC_TPCCS_7:
        case NV_PFAULT_CLIENT_GPC_T1_14:
        case NV_PFAULT_CLIENT_GPC_T1_15:
            return UVM_VOLTA_GPC_UTLB_ID_LTP7;
        default:
            UVM_ASSERT_MSG(false, "Invalid client value: 0x%x\n", client_id);
    }

    return 0;
}
