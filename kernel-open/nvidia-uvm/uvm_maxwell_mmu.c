/*******************************************************************************
    Copyright (c) 2016-2023 NVIDIA Corporation

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


// For Maxwell, UVM page tree 'depth' maps to hardware as follows:
//
// 64k Big page size:
// UVM depth   HW level                            VA bits
// 0           PDE                                 39:26
// 1           PTE Big / PTE 4k                    25:16 / 25:12
//
// 128k Big page size (same levels, just different VA bits):
// UVM depth   HW level                            VA bits
// 0           PDE                                 39:27
// 1           PTE Big / PTE 4k                    26:17 / 26:12

#include "uvm_types.h"
#include "uvm_forward_decl.h"
#include "uvm_gpu.h"
#include "uvm_mmu.h"
#include "uvm_hal.h"
#include "uvm_push_macros.h"
#include "hwref/maxwell/gm107/dev_mmu.h"

#define MMU_BIG 0
#define MMU_SMALL 1

static NvU32 entries_per_index_maxwell(NvU32 depth)
{
    UVM_ASSERT(depth < 2);
    if (depth == 0)
        return 2;
    return 1;
}

static NvLength entry_offset_maxwell(NvU32 depth, NvU64 page_size)
{
    UVM_ASSERT(depth < 2);
    if (page_size == UVM_PAGE_SIZE_4K && depth == 0)
        return MMU_SMALL;
    return MMU_BIG;
}

static NvU64 big_half_pde_maxwell(uvm_mmu_page_table_alloc_t *phys_alloc)
{
    NvU64 pde_bits = 0;
    if (phys_alloc != NULL) {
       NvU64 address = phys_alloc->addr.address >> NV_MMU_PDE_ADDRESS_SHIFT;
       pde_bits |= HWCONST64(_MMU, PDE, VOL_BIG, TRUE);
       switch (phys_alloc->addr.aperture) {
           case UVM_APERTURE_SYS:
               pde_bits |= HWCONST64(_MMU, PDE, APERTURE_BIG, SYSTEM_COHERENT_MEMORY) |
                           HWVALUE64(_MMU, PDE, ADDRESS_BIG_SYS, address);
               break;
           case UVM_APERTURE_VID:
               pde_bits |= HWCONST64(_MMU, PDE, APERTURE_BIG, VIDEO_MEMORY) |
                           HWVALUE64(_MMU, PDE, ADDRESS_BIG_VID, phys_alloc->addr.address >> NV_MMU_PDE_ADDRESS_SHIFT);
               break;
           default:
               UVM_ASSERT_MSG(0, "Invalid big aperture: %d\n", phys_alloc->addr.aperture);
               break;
        }
    }
    return pde_bits;
}

static NvU64 small_half_pde_maxwell(uvm_mmu_page_table_alloc_t *phys_alloc)
{
    NvU64 pde_bits = 0;
    if (phys_alloc != NULL) {
        NvU64 address = phys_alloc->addr.address >> NV_MMU_PDE_ADDRESS_SHIFT;
        pde_bits |= HWCONST64(_MMU, PDE, VOL_SMALL, TRUE);
        switch (phys_alloc->addr.aperture) {
            case UVM_APERTURE_SYS:
                pde_bits |= HWCONST64(_MMU, PDE, APERTURE_SMALL, SYSTEM_COHERENT_MEMORY) |
                            HWVALUE64(_MMU, PDE, ADDRESS_SMALL_SYS, address);
                break;
            case UVM_APERTURE_VID:
                pde_bits |= HWCONST64(_MMU, PDE, APERTURE_SMALL, VIDEO_MEMORY) |
                            HWVALUE64(_MMU, PDE, ADDRESS_SMALL_VID, address);
                break;
            default:
                UVM_ASSERT_MSG(0, "Invalid small aperture: %d\n", phys_alloc->addr.aperture);
                break;
        }
    }
    return pde_bits;
}

static void make_pde_maxwell(void *entry,
                             uvm_mmu_page_table_alloc_t **phys_allocs,
                             uvm_page_directory_t *dir,
                             NvU32 child_index)
{
    NvU64 pde_bits = 0;

    UVM_ASSERT(dir);
    UVM_ASSERT(dir->depth == 0);

    pde_bits |= HWCONST64(_MMU, PDE, SIZE, FULL);
    pde_bits |= big_half_pde_maxwell(phys_allocs[MMU_BIG]) | small_half_pde_maxwell(phys_allocs[MMU_SMALL]);

    *(NvU64 *)entry = pde_bits;
}

static NvLength entry_size_maxwell(NvU32 depth)
{
    UVM_ASSERT(depth < 2);
    return 8;
}

static NvU32 index_bits_maxwell_64(NvU32 depth, NvU64 page_size)
{
    UVM_ASSERT(depth < 2);
    UVM_ASSERT(page_size == UVM_PAGE_SIZE_4K ||
               page_size == UVM_PAGE_SIZE_64K ||
               (depth == 0 && page_size == UVM_PAGE_SIZE_AGNOSTIC));

    if (depth == 0) {
        return 14;
    }
    else {
        if (page_size == UVM_PAGE_SIZE_4K)
            return 14;
        else
            return 10;
    }
}

static NvU32 index_bits_maxwell_128(NvU32 depth, NvU64 page_size)
{
    UVM_ASSERT(depth < 2);
    UVM_ASSERT(page_size == UVM_PAGE_SIZE_4K ||
               page_size == UVM_PAGE_SIZE_128K ||
               (depth == 0 && page_size == UVM_PAGE_SIZE_AGNOSTIC));

    if (depth == 0) {
        return 13;
    }
    else {
        if (page_size == UVM_PAGE_SIZE_4K)
            return 15;
        else
            return 10;
    }
}

static NvU32 num_va_bits_maxwell(void)
{
    return 40;
}

static NvLength allocation_size_maxwell_64(NvU32 depth, NvU64 page_size)
{
    return entry_size_maxwell(depth) << index_bits_maxwell_64(depth, page_size);
}

static NvLength allocation_size_maxwell_128(NvU32 depth, NvU64 page_size)
{
    return entry_size_maxwell(depth) << index_bits_maxwell_128(depth, page_size);
}

static NvU32 page_table_depth_maxwell(NvU64 page_size)
{
    return 1;
}

static NvU64 page_sizes_maxwell_128(void)
{
    return UVM_PAGE_SIZE_128K | UVM_PAGE_SIZE_4K;
}

static NvU64 page_sizes_maxwell_64(void)
{
    return UVM_PAGE_SIZE_64K | UVM_PAGE_SIZE_4K;
}

static NvU64 unmapped_pte_maxwell(NvU64 page_size)
{
    // Setting the privilege bit on an otherwise-zeroed big PTE causes the
    // corresponding 4k PTEs to be ignored. This allows the invalidation of a
    // mixed PDE range to be much faster.
    if (page_size == UVM_PAGE_SIZE_4K)
        return 0;

    // When VALID == 0, MMU still reads the VOL and PRIV fields. VOL == 1
    // indicates that the PTE is sparse, so make sure we don't use it.
    return HWCONST64(_MMU, PTE, VALID,     FALSE) |
           HWCONST64(_MMU, PTE, PRIVILEGE, TRUE)  |
           HWCONST64(_MMU, PTE, VOL,       FALSE);
}

static NvU64 make_pte_maxwell(uvm_aperture_t aperture, NvU64 address, uvm_prot_t prot, NvU64 flags)
{
    NvU64 pte_bits = 0;
    NvU8 aperture_bits = 0;

    UVM_ASSERT(prot != UVM_PROT_NONE);
    UVM_ASSERT((flags & ~UVM_MMU_PTE_FLAGS_MASK) == 0);

    // valid 0:0
    pte_bits |= HWCONST64(_MMU, PTE, VALID, TRUE);

    // privilege 1:1
    pte_bits |= HWCONST64(_MMU, PTE, PRIVILEGE, FALSE);

    // read-only 2:2 (used by everything except L1 and GCC)
    if (prot == UVM_PROT_READ_ONLY)
        pte_bits |= HWCONST64(_MMU, PTE, READ_ONLY, TRUE);
    else
        pte_bits |= HWCONST64(_MMU, PTE, READ_ONLY, FALSE);

    // encrypted 3:3
    pte_bits |= HWCONST64(_MMU, PTE, ENCRYPTED, FALSE);

    address >>= NV_MMU_PTE_ADDRESS_SHIFT;
    if (aperture == UVM_APERTURE_SYS) {
        // sys address 31:4
        pte_bits |= HWVALUE64(_MMU, PTE, ADDRESS_SYS, address);
    }
    else {
        // vid address 28:4
        pte_bits |= HWVALUE64(_MMU, PTE, ADDRESS_VID, address);

        // peer 29:31
        if (aperture != UVM_APERTURE_VID)
            pte_bits |= HWVALUE64(_MMU, PTE, ADDRESS_VID_PEER, UVM_APERTURE_PEER_ID(aperture));
    }

    // volatile 32:32
    if (flags & UVM_MMU_PTE_FLAGS_CACHED)
        pte_bits |= HWCONST64(_MMU, PTE, VOL, FALSE);
    else
        pte_bits |= HWCONST64(_MMU, PTE, VOL, TRUE);

    // aperture 34:32
    if (aperture == UVM_APERTURE_SYS)
        aperture_bits = NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY;
    else if (aperture == UVM_APERTURE_VID)
        aperture_bits = NV_MMU_PTE_APERTURE_VIDEO_MEMORY;
    else if (aperture >= UVM_APERTURE_PEER_0 && aperture <= UVM_APERTURE_PEER_7)
        aperture_bits = NV_MMU_PTE_APERTURE_PEER_MEMORY;
    else
        UVM_ASSERT_MSG(NV_FALSE, "Invalid location: %d", aperture);

    pte_bits |= HWVALUE64(_MMU, PTE, APERTURE, aperture_bits);

    // lock 35:35
    pte_bits |= HWCONST64(_MMU, PTE, LOCK, FALSE);

    // kind 43:36
    pte_bits |= HWCONST64(_MMU, PTE, KIND, PITCH);

    // comptagline 61:44
    pte_bits |= HWVALUE64(_MMU, PTE, COMPTAGLINE, 0);

    // read disable 62:62 (used only by L1 and GCC)
    pte_bits |= HWCONST64(_MMU, PTE, READ_DISABLE, FALSE);

    // write disable 63:63 (used only by L1 and GCC: everything else uses READ_ONLY)
    if (prot == UVM_PROT_READ_ONLY)
        pte_bits |= HWCONST64(_MMU, PTE, WRITE_DISABLE, TRUE);
    else
        pte_bits |= HWCONST64(_MMU, PTE, WRITE_DISABLE, FALSE);

    return pte_bits;
}

static NvU64 make_sked_reflected_pte_maxwell(void)
{
    NvU64 pte_bits = 0;

    pte_bits |= HWCONST64(_MMU, PTE, VALID, TRUE);
    pte_bits |= HWCONST64(_MMU, PTE, KIND, SMSKED_MESSAGE);

    return pte_bits;
}

static NvU64 poisoned_pte_maxwell(void)
{
    // An invalid PTE is also fatal on Maxwell, but a PRIV violation will
    // immediately identify bad PTE usage.

    // Engines with priv accesses won't fault on the priv PTE, so add a backup
    // mechanism using an impossible memory address. This will trigger an
    // interrupt starting with GM20x. On earlier GPUs the upper bits will
    // silently be dropped.
    //
    // This address has to fit within 37 bits (max address width of vidmem) and
    // be aligned to page_size.
    NvU64 phys_addr = 0x1bad000000ULL;
    NvU64 pte_bits = make_pte_maxwell(UVM_APERTURE_VID, phys_addr, UVM_PROT_READ_ONLY, UVM_MMU_PTE_FLAGS_NONE);

    return WRITE_HWCONST64(pte_bits, _MMU, PTE, PRIVILEGE, TRUE);
}

// Sparse mappings are not supported.
static NvU64 make_sparse_pte_maxwell_unsupported(void)
{
    UVM_ASSERT_MSG(0, "Sparse mappings unsupported on pre-Pascal GPUs\n");
    return poisoned_pte_maxwell();
}

static uvm_mmu_mode_hal_t maxwell_64_mmu_mode_hal =
{
    .make_pte = make_pte_maxwell,
    .make_sked_reflected_pte = make_sked_reflected_pte_maxwell,
    .make_sparse_pte = make_sparse_pte_maxwell_unsupported,
    .unmapped_pte = unmapped_pte_maxwell,
    .poisoned_pte = poisoned_pte_maxwell,
    .make_pde = make_pde_maxwell,
    .entry_size = entry_size_maxwell,
    .index_bits = index_bits_maxwell_64,
    .entries_per_index = entries_per_index_maxwell,
    .entry_offset = entry_offset_maxwell,
    .num_va_bits = num_va_bits_maxwell,
    .allocation_size = allocation_size_maxwell_64,
    .page_table_depth = page_table_depth_maxwell,
    .page_sizes = page_sizes_maxwell_64
};

static uvm_mmu_mode_hal_t maxwell_128_mmu_mode_hal =
{
    .make_pte = make_pte_maxwell,
    .make_sked_reflected_pte = make_sked_reflected_pte_maxwell,
    .make_sparse_pte = make_sparse_pte_maxwell_unsupported,
    .unmapped_pte = unmapped_pte_maxwell,
    .poisoned_pte = poisoned_pte_maxwell,
    .make_pde = make_pde_maxwell,
    .entry_size = entry_size_maxwell,
    .index_bits = index_bits_maxwell_128,
    .entries_per_index = entries_per_index_maxwell,
    .entry_offset = entry_offset_maxwell,
    .num_va_bits = num_va_bits_maxwell,
    .allocation_size = allocation_size_maxwell_128,
    .page_table_depth = page_table_depth_maxwell,
    .page_sizes = page_sizes_maxwell_128
};

uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_maxwell(NvU64 big_page_size)
{
    UVM_ASSERT(big_page_size == UVM_PAGE_SIZE_64K || big_page_size == UVM_PAGE_SIZE_128K);
    if (big_page_size == UVM_PAGE_SIZE_64K)
        return &maxwell_64_mmu_mode_hal;

    return &maxwell_128_mmu_mode_hal;
}

void uvm_hal_maxwell_mmu_enable_prefetch_faults_unsupported(uvm_parent_gpu_t *parent_gpu)
{
    UVM_ASSERT_MSG(false, "mmu enable_prefetch_faults called on Maxwell GPU\n");
}

void uvm_hal_maxwell_mmu_disable_prefetch_faults_unsupported(uvm_parent_gpu_t *parent_gpu)
{
    UVM_ASSERT_MSG(false, "mmu disable_prefetch_faults called on Maxwell GPU\n");
}

NvU16 uvm_hal_maxwell_mmu_client_id_to_utlb_id_unsupported(NvU16 client_id)
{
    UVM_ASSERT(0);
    return 0;
}
