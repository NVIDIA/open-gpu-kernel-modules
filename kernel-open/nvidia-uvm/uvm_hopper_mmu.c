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

// For Hopper, the UVM page tree 'depth' maps to hardware as follows:
//
// UVM depth   HW level                            VA bits
// 0           PDE4                                56:56
// 1           PDE3                                55:47
// 2           PDE2                                46:38
// 3           PDE1 (or 512M PTE)                  37:29
// 4           PDE0 (dual 64k/4k PDE, or 2M PTE)   28:21
// 5           PTE_64K / PTE_4K                    20:16 / 20:12

#include "uvm_types.h"
#include "uvm_global.h"
#include "uvm_common.h"
#include "uvm_hal.h"
#include "uvm_hal_types.h"
#include "uvm_hopper_fault_buffer.h"
#include "hwref/hopper/gh100/dev_fault.h"
#include "hwref/hopper/gh100/dev_mmu.h"

#define MMU_BIG 0
#define MMU_SMALL 1

// Used in pde_pcf().
#define ATS_ALLOWED 0
#define ATS_NOT_ALLOWED 1

static NvU32 page_table_depth_hopper(NvU64 page_size)
{
    // The common-case is page_size == UVM_PAGE_SIZE_2M, hence the first check
    if (page_size == UVM_PAGE_SIZE_2M)
        return 4;
    else if (page_size == UVM_PAGE_SIZE_512M)
        return 3;

    UVM_ASSERT((page_size == UVM_PAGE_SIZE_4K) || (page_size == UVM_PAGE_SIZE_64K) ||
               (page_size == UVM_PAGE_SIZE_DEFAULT));

    return 5;
}

static NvU32 entries_per_index_hopper(NvU32 depth)
{
    UVM_ASSERT(depth < 6);
    if (depth == 4)
        return 2;
    return 1;
}

static NvLength entry_offset_hopper(NvU32 depth, NvU64 page_size)
{
    UVM_ASSERT(depth < 6);
    if ((page_size == UVM_PAGE_SIZE_4K) && (depth == 4))
        return MMU_SMALL;
    return MMU_BIG;
}

static NvLength entry_size_hopper(NvU32 depth)
{
    return entries_per_index_hopper(depth) * 8;
}

static NvU32 index_bits_hopper(NvU32 depth, NvU64 page_size)
{
    static const NvU32 bit_widths[] = {1, 9, 9, 9, 8};

    // some code paths keep on querying this until they get a 0, meaning only
    // the page offset remains.
    UVM_ASSERT(depth < 6);
    if (depth < 5) {
        return bit_widths[depth];
    }
    else if (depth == 5) {
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

static NvU32 num_va_bits_hopper(void)
{
    return 57;
}

static NvLength allocation_size_hopper(NvU32 depth, NvU64 page_size)
{
    UVM_ASSERT(depth < 6);
    if (depth == 5 && page_size == UVM_PAGE_SIZE_64K)
        return 256;

    // depth 0 requires only a 16-byte allocation, but it must be 4k aligned.
    return 4096;
}

// PTE Permission Control Flags
static NvU64 pte_pcf(uvm_prot_t prot, NvU64 flags)
{
    bool ac = !(flags & UVM_MMU_PTE_FLAGS_ACCESS_COUNTERS_DISABLED);
    bool cached = flags & UVM_MMU_PTE_FLAGS_CACHED;

    UVM_ASSERT(prot != UVM_PROT_NONE);
    UVM_ASSERT((flags & ~UVM_MMU_PTE_FLAGS_MASK) == 0);

    if (ac) {
        switch (prot) {
            case UVM_PROT_READ_ONLY:
                return cached ? NV_MMU_VER3_PTE_PCF_REGULAR_RO_NO_ATOMIC_CACHED_ACE :
                                NV_MMU_VER3_PTE_PCF_REGULAR_RO_NO_ATOMIC_UNCACHED_ACE;
            case UVM_PROT_READ_WRITE:
                return cached ? NV_MMU_VER3_PTE_PCF_REGULAR_RW_NO_ATOMIC_CACHED_ACE :
                                NV_MMU_VER3_PTE_PCF_REGULAR_RW_NO_ATOMIC_UNCACHED_ACE;
            case UVM_PROT_READ_WRITE_ATOMIC:
                return cached ? NV_MMU_VER3_PTE_PCF_REGULAR_RW_ATOMIC_CACHED_ACE :
                                NV_MMU_VER3_PTE_PCF_REGULAR_RW_ATOMIC_UNCACHED_ACE;
            default:
                break;
        }
    }
    else {
        switch (prot) {
            case UVM_PROT_READ_ONLY:
                return cached ? NV_MMU_VER3_PTE_PCF_REGULAR_RO_NO_ATOMIC_CACHED_ACD :
                                NV_MMU_VER3_PTE_PCF_REGULAR_RO_NO_ATOMIC_UNCACHED_ACD;
            case UVM_PROT_READ_WRITE:
                return cached ? NV_MMU_VER3_PTE_PCF_REGULAR_RW_NO_ATOMIC_CACHED_ACD :
                                NV_MMU_VER3_PTE_PCF_REGULAR_RW_NO_ATOMIC_UNCACHED_ACD;
            case UVM_PROT_READ_WRITE_ATOMIC:
                return cached ? NV_MMU_VER3_PTE_PCF_REGULAR_RW_ATOMIC_CACHED_ACD :
                                NV_MMU_VER3_PTE_PCF_REGULAR_RW_ATOMIC_UNCACHED_ACD;
            default:
                break;
        }
    }

    // Unsupported PCF
    UVM_ASSERT_MSG(0, "Unsupported PTE PCF: prot: %s, ac: %d, cached: %d\n", uvm_prot_string(prot), ac, cached);

    return NV_MMU_VER3_PTE_PCF_INVALID;
}

static NvU64 make_pte_hopper(uvm_aperture_t aperture, NvU64 address, uvm_prot_t prot, NvU64 flags)
{
    NvU8 aperture_bits = 0;
    NvU64 pte_bits = 0;

    // valid 0:0
    pte_bits |= HWCONST64(_MMU_VER3, PTE, VALID, TRUE);

    // aperture 2:1
    if (aperture == UVM_APERTURE_SYS)
        aperture_bits = NV_MMU_VER3_PTE_APERTURE_SYSTEM_COHERENT_MEMORY;
    else if (aperture == UVM_APERTURE_VID)
        aperture_bits = NV_MMU_VER3_PTE_APERTURE_VIDEO_MEMORY;
    else if (aperture >= UVM_APERTURE_PEER_0 && aperture <= UVM_APERTURE_PEER_7)
        aperture_bits = NV_MMU_VER3_PTE_APERTURE_PEER_MEMORY;
    else
        UVM_ASSERT_MSG(0, "Invalid aperture: %d\n", aperture);

    pte_bits |= HWVALUE64(_MMU_VER3, PTE, APERTURE, aperture_bits);

    // PCF (permission control flags) 7:3
    pte_bits |= HWVALUE64(_MMU_VER3, PTE, PCF, pte_pcf(prot, flags));

    // kind 11:8
    pte_bits |= HWVALUE64(_MMU_VER3, PTE, KIND, NV_MMU_PTE_KIND_GENERIC_MEMORY);

    address >>= NV_MMU_VER3_PTE_ADDRESS_SHIFT;

    if (aperture == UVM_APERTURE_VID) {
        // vid address 39:12
        pte_bits |= HWVALUE64(_MMU_VER3, PTE, ADDRESS_VID, address);
    }
    else {
        // sys/peer address 51:12
        pte_bits |= HWVALUE64(_MMU_VER3, PTE, ADDRESS, address);

        // peer id 63:61
        if (aperture >= UVM_APERTURE_PEER_0 && aperture <= UVM_APERTURE_PEER_7)
            pte_bits |= HWVALUE64(_MMU_VER3, PTE, PEER_ID, UVM_APERTURE_PEER_ID(aperture));
    }

    return pte_bits;
}

static NvU64 make_sked_reflected_pte_hopper(void)
{
    // On discrete GPUs, SKED Reflected PTEs may use either the local aperture
    // or the system non coherent aperture. However, integrated GPUs may only
    // use the system non-coherent aperture. We always use the system
    // non-coherent aperture as that is common to both discrete and integrated
    // GPUs.
    return HWCONST64(_MMU_VER3, PTE, VALID, TRUE) |
           HWCONST64(_MMU_VER3, PTE, APERTURE, SYSTEM_NON_COHERENT_MEMORY) |
           HWVALUE64(_MMU_VER3, PTE, PCF, pte_pcf(UVM_PROT_READ_WRITE_ATOMIC, UVM_MMU_PTE_FLAGS_NONE)) |
           HWVALUE64(_MMU_VER3, PTE, KIND, NV_MMU_PTE_KIND_SMSKED_MESSAGE);
}

static NvU64 make_sparse_pte_hopper(void)
{
    return HWCONST64(_MMU_VER3, PTE, VALID, FALSE) |
           HWCONST64(_MMU_VER3, PTE, PCF, SPARSE);
}

static NvU64 unmapped_pte_hopper(NvU64 page_size)
{
    // Setting PCF to NO_VALID_4KB_PAGE on an otherwise-zeroed big PTE causes
    // the corresponding 4k PTEs to be ignored. This allows the invalidation of
    // a mixed PDE range to be much faster.
    if (page_size != UVM_PAGE_SIZE_64K)
        return 0;

    // When VALID == 0, GMMU still reads the PCF field, which indicates the PTE
    // is sparse (make_sparse_pte_hopper) or an unmapped big-page PTE.
    return HWCONST64(_MMU_VER3, PTE, VALID, FALSE) |
           HWCONST64(_MMU_VER3, PTE, PCF, NO_VALID_4KB_PAGE);
}

static NvU64 poisoned_pte_hopper(void)
{
    // An invalid PTE won't be fatal from faultable units like SM, which is the
    // most likely source of bad PTE accesses.

    // Engines with priv accesses won't fault on the priv PTE, so add a backup
    // mechanism using an impossible memory address. MMU will trigger an
    // interrupt when it detects a bad physical address, i.e., a physical
    // address > GPU memory size.
    //
    // This address has to fit within 38 bits (max address width of vidmem) and
    // be aligned to page_size.
    NvU64 phys_addr = 0x2bad000000ULL;

    NvU64 pte_bits = make_pte_hopper(UVM_APERTURE_VID, phys_addr, UVM_PROT_READ_ONLY, UVM_MMU_PTE_FLAGS_NONE);
    return WRITE_HWCONST64(pte_bits, _MMU_VER3, PTE, PCF, PRIVILEGE_RO_NO_ATOMIC_UNCACHED_ACD);
}

typedef enum
{
    PDE_TYPE_SINGLE,
    PDE_TYPE_DUAL_BIG,
    PDE_TYPE_DUAL_SMALL,
    PDE_TYPE_COUNT,
} pde_type_t;

static const NvU8 valid_pcf[][2] = { { NV_MMU_VER3_PDE_PCF_VALID_UNCACHED_ATS_ALLOWED,
                                       NV_MMU_VER3_PDE_PCF_VALID_UNCACHED_ATS_NOT_ALLOWED },
                                     { NV_MMU_VER3_DUAL_PDE_PCF_BIG_VALID_UNCACHED_ATS_ALLOWED,
                                       NV_MMU_VER3_DUAL_PDE_PCF_BIG_VALID_UNCACHED_ATS_NOT_ALLOWED },
                                     { NV_MMU_VER3_DUAL_PDE_PCF_SMALL_VALID_UNCACHED_ATS_ALLOWED,
                                       NV_MMU_VER3_DUAL_PDE_PCF_SMALL_VALID_UNCACHED_ATS_NOT_ALLOWED } };

static const NvU8 invalid_pcf[][2] = { { NV_MMU_VER3_PDE_PCF_INVALID_ATS_ALLOWED,
                                         NV_MMU_VER3_PDE_PCF_INVALID_ATS_NOT_ALLOWED },
                                       { NV_MMU_VER3_DUAL_PDE_PCF_BIG_INVALID_ATS_ALLOWED,
                                         NV_MMU_VER3_DUAL_PDE_PCF_BIG_INVALID_ATS_NOT_ALLOWED },
                                       { NV_MMU_VER3_DUAL_PDE_PCF_SMALL_INVALID_ATS_ALLOWED,
                                         NV_MMU_VER3_DUAL_PDE_PCF_SMALL_INVALID_ATS_NOT_ALLOWED } };

static const NvU8 va_base[] = { 56, 47, 38, 29, 21 };

static bool is_ats_range_valid(uvm_page_directory_t *dir, NvU32 child_index)
{
    NvU64 pde_base_va;
    NvU64 min_va_upper;
    NvU64 max_va_lower;
    NvU32 index_in_dir;

    uvm_cpu_get_unaddressable_range(&max_va_lower, &min_va_upper);

    UVM_ASSERT(dir->depth < ARRAY_SIZE(va_base));

    // We can use UVM_PAGE_SIZE_AGNOSTIC because page_size is only used in
    // index_bits_hopper() for PTE table, i.e., depth 5+, which does not use a
    // PDE PCF or an ATS_ALLOWED/NOT_ALLOWED setting.
    UVM_ASSERT(child_index < (1ull << index_bits_hopper(dir->depth, UVM_PAGE_SIZE_AGNOSTIC)));

    pde_base_va = 0;
    index_in_dir = child_index;
    while (dir) {
        pde_base_va += index_in_dir * (1ull << va_base[dir->depth]);
        index_in_dir = dir->index_in_parent;
        dir = dir->host_parent;
    }
    pde_base_va = (NvU64)((NvS64)(pde_base_va << (64 - num_va_bits_hopper())) >> (64 - num_va_bits_hopper()));

    if (pde_base_va < max_va_lower || pde_base_va >= min_va_upper)
        return true;

    return false;
}

// PDE Permission Control Flags
static NvU32 pde_pcf(bool valid, pde_type_t pde_type, uvm_page_directory_t *dir, NvU32 child_index)
{
    const NvU8 (*pcf)[2] = valid ? valid_pcf : invalid_pcf;
    NvU8 depth = dir->depth;

    UVM_ASSERT(pde_type < PDE_TYPE_COUNT);
    UVM_ASSERT(depth < 5);

    // On non-ATS systems, PDE PCF only sets the valid and volatile/cache bits.
    if (!g_uvm_global.ats.enabled)
        return pcf[pde_type][ATS_ALLOWED];

    // Hopper GPUs on ATS-enabled systems, perform a parallel lookup on both
    // ATS and GMMU page tables. For managed memory we need to prevent this
    // parallel lookup since we would not get any GPU fault if the CPU has
    // a valid mapping. Also, for external ranges that are known to be
    // mapped entirely on the GMMU page table we can skip the ATS lookup
    // for performance reasons. Parallel ATS lookup is disabled in PDE1
    // (depth 3) and, therefore, it applies to the underlying 512MB VA
    // range.
    //
    // UVM sets ATS_NOT_ALLOWED for all Hopper+ mappings on ATS systems.
    // This is fine because CUDA ensures that all managed and external
    // allocations are properly compartmentalized in 512MB-aligned VA
    // regions. For cudaHostRegister CUDA cannot control the VA range, but
    // we rely on ATS for those allocations so they can't choose the
    // ATS_NOT_ALLOWED mode.
    // TODO: Bug 3254055: Relax the NO_ATS setting from 512MB (pde1) range to
    //                    PTEs.
    // HW complies with the leaf PDE's ATS_ALLOWED/ATS_NOT_ALLOWED settings,
    // enabling us to treat any upper-level PDE as a don't care as long as there
    // are leaf PDEs for the entire upper-level PDE range. We assume PDE4
    // entries (depth == 0) are always ATS enabled, and the no_ats_range is in
    // PDE3 or lower.
    if (depth == 0 || (!valid && is_ats_range_valid(dir, child_index)))
        return pcf[pde_type][ATS_ALLOWED];

    return pcf[pde_type][ATS_NOT_ALLOWED];
}

static NvU64 single_pde_hopper(uvm_mmu_page_table_alloc_t *phys_alloc, uvm_page_directory_t *dir, NvU32 child_index)
{
    NvU64 pde_bits = 0;

    if (phys_alloc != NULL) {
        NvU64 address = phys_alloc->addr.address >> NV_MMU_VER3_PDE_ADDRESS_SHIFT;
        pde_bits |= HWCONST64(_MMU_VER3, PDE, IS_PTE, FALSE);

        switch (phys_alloc->addr.aperture) {
            case UVM_APERTURE_SYS:
                pde_bits |= HWCONST64(_MMU_VER3, PDE, APERTURE, SYSTEM_COHERENT_MEMORY);
                break;
            case UVM_APERTURE_VID:
                pde_bits |= HWCONST64(_MMU_VER3, PDE, APERTURE, VIDEO_MEMORY);
                break;
            default:
                UVM_ASSERT_MSG(0, "Invalid aperture: %d\n", phys_alloc->addr.aperture);
                break;
        }

        // address 51:12
        pde_bits |= HWVALUE64(_MMU_VER3, PDE, ADDRESS, address);
    }

    // PCF (permission control flags) 5:3
    pde_bits |= HWVALUE64(_MMU_VER3, PDE, PCF, pde_pcf(phys_alloc != NULL, PDE_TYPE_SINGLE, dir, child_index));

    return pde_bits;
}

static NvU64 big_half_pde_hopper(uvm_mmu_page_table_alloc_t *phys_alloc, uvm_page_directory_t *dir, NvU32 child_index)
{
    NvU64 pde_bits = 0;

    if (phys_alloc != NULL) {
        NvU64 address = phys_alloc->addr.address >> NV_MMU_VER3_DUAL_PDE_ADDRESS_BIG_SHIFT;

        switch (phys_alloc->addr.aperture) {
            case UVM_APERTURE_SYS:
                pde_bits |= HWCONST64(_MMU_VER3, DUAL_PDE, APERTURE_BIG, SYSTEM_COHERENT_MEMORY);
                break;
            case UVM_APERTURE_VID:
                pde_bits |= HWCONST64(_MMU_VER3, DUAL_PDE, APERTURE_BIG, VIDEO_MEMORY);
                break;
            default:
                UVM_ASSERT_MSG(0, "Invalid big aperture %d\n", phys_alloc->addr.aperture);
                break;
        }

        // address 51:8
        pde_bits |= HWVALUE64(_MMU_VER3, DUAL_PDE, ADDRESS_BIG, address);
    }

    // PCF (permission control flags) 5:3
    pde_bits |= HWVALUE64(_MMU_VER3,
                          DUAL_PDE,
                          PCF_BIG,
                          pde_pcf(phys_alloc != NULL, PDE_TYPE_DUAL_BIG, dir, child_index));

    return pde_bits;
}

static NvU64 small_half_pde_hopper(uvm_mmu_page_table_alloc_t *phys_alloc, uvm_page_directory_t *dir, NvU32 child_index)
{
    NvU64 pde_bits = 0;

    if (phys_alloc != NULL) {
        NvU64 address = phys_alloc->addr.address >> NV_MMU_VER3_DUAL_PDE_ADDRESS_SHIFT;

        switch (phys_alloc->addr.aperture) {
            case UVM_APERTURE_SYS:
                pde_bits |= HWCONST64(_MMU_VER3, DUAL_PDE, APERTURE_SMALL, SYSTEM_COHERENT_MEMORY);
                break;
            case UVM_APERTURE_VID:
                pde_bits |= HWCONST64(_MMU_VER3, DUAL_PDE, APERTURE_SMALL, VIDEO_MEMORY);
                break;
            default:
                UVM_ASSERT_MSG(0, "Invalid small aperture %d\n", phys_alloc->addr.aperture);
                break;
        }

        // address 115:76 [51:12]
        pde_bits |= HWVALUE64(_MMU_VER3, DUAL_PDE, ADDRESS_SMALL, address);
    }

    // PCF (permission control flags) 69:67 [5:3]
    pde_bits |= HWVALUE64(_MMU_VER3,
                          DUAL_PDE,
                          PCF_SMALL,
                          pde_pcf(phys_alloc != NULL, PDE_TYPE_DUAL_SMALL, dir, child_index));

    return pde_bits;
}

static void make_pde_hopper(void *entry,
                            uvm_mmu_page_table_alloc_t **phys_allocs,
                            uvm_page_directory_t *dir,
                            NvU32 child_index)
{
    NvU32 entry_count;
    NvU64 *entry_bits = (NvU64 *)entry;

    UVM_ASSERT(dir);

    entry_count = entries_per_index_hopper(dir->depth);

    if (entry_count == 1) {
        *entry_bits = single_pde_hopper(*phys_allocs, dir, child_index);
    }
    else if (entry_count == 2) {
        entry_bits[MMU_BIG] = big_half_pde_hopper(phys_allocs[MMU_BIG], dir, child_index);
        entry_bits[MMU_SMALL] = small_half_pde_hopper(phys_allocs[MMU_SMALL], dir, child_index);

        // This entry applies to the whole dual PDE but is stored in the lower
        // bits.
        entry_bits[MMU_BIG] |= HWCONST64(_MMU_VER3, DUAL_PDE, IS_PTE, FALSE);
    }
    else {
        UVM_ASSERT_MSG(0, "Invalid number of entries per index: %d\n", entry_count);
    }
}

static uvm_mmu_mode_hal_t hopper_mmu_mode_hal;

uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_hopper(NvU64 big_page_size)
{
    static bool initialized = false;

    UVM_ASSERT(big_page_size == UVM_PAGE_SIZE_64K || big_page_size == UVM_PAGE_SIZE_128K);

    // TODO: Bug 1789555: RM should reject the creation of GPU VA spaces with
    // 128K big page size for Pascal+ GPUs
    if (big_page_size == UVM_PAGE_SIZE_128K)
        return NULL;

    if (!initialized) {
        uvm_mmu_mode_hal_t *ampere_mmu_mode_hal = uvm_hal_mmu_mode_ampere(big_page_size);
        UVM_ASSERT(ampere_mmu_mode_hal);

        // The assumption made is that arch_hal->mmu_mode_hal() will be called
        // under the global lock the first time, so check it here.
        uvm_assert_mutex_locked(&g_uvm_global.global_lock);

        hopper_mmu_mode_hal = *ampere_mmu_mode_hal;
        hopper_mmu_mode_hal.entry_size = entry_size_hopper;
        hopper_mmu_mode_hal.index_bits = index_bits_hopper;
        hopper_mmu_mode_hal.entries_per_index = entries_per_index_hopper;
        hopper_mmu_mode_hal.entry_offset = entry_offset_hopper;
        hopper_mmu_mode_hal.num_va_bits = num_va_bits_hopper;
        hopper_mmu_mode_hal.allocation_size = allocation_size_hopper;
        hopper_mmu_mode_hal.page_table_depth = page_table_depth_hopper;
        hopper_mmu_mode_hal.make_pte = make_pte_hopper;
        hopper_mmu_mode_hal.make_sked_reflected_pte = make_sked_reflected_pte_hopper;
        hopper_mmu_mode_hal.make_sparse_pte = make_sparse_pte_hopper;
        hopper_mmu_mode_hal.unmapped_pte = unmapped_pte_hopper;
        hopper_mmu_mode_hal.poisoned_pte = poisoned_pte_hopper;
        hopper_mmu_mode_hal.make_pde = make_pde_hopper;

        initialized = true;
    }

    return &hopper_mmu_mode_hal;
}

NvU16 uvm_hal_hopper_mmu_client_id_to_utlb_id(NvU16 client_id)
{
    switch (client_id) {
        case NV_PFAULT_CLIENT_GPC_RAST:
        case NV_PFAULT_CLIENT_GPC_GCC:
        case NV_PFAULT_CLIENT_GPC_GPCCS:
            return UVM_HOPPER_GPC_UTLB_ID_RGG;
        case NV_PFAULT_CLIENT_GPC_T1_0:
            return UVM_HOPPER_GPC_UTLB_ID_LTP0;
        case NV_PFAULT_CLIENT_GPC_T1_1:
        case NV_PFAULT_CLIENT_GPC_PE_0:
        case NV_PFAULT_CLIENT_GPC_TPCCS_0:
            return UVM_HOPPER_GPC_UTLB_ID_LTP1;
        case NV_PFAULT_CLIENT_GPC_T1_2:
            return UVM_HOPPER_GPC_UTLB_ID_LTP2;
        case NV_PFAULT_CLIENT_GPC_T1_3:
        case NV_PFAULT_CLIENT_GPC_PE_1:
        case NV_PFAULT_CLIENT_GPC_TPCCS_1:
            return UVM_HOPPER_GPC_UTLB_ID_LTP3;
        case NV_PFAULT_CLIENT_GPC_T1_4:
            return UVM_HOPPER_GPC_UTLB_ID_LTP4;
        case NV_PFAULT_CLIENT_GPC_T1_5:
        case NV_PFAULT_CLIENT_GPC_PE_2:
        case NV_PFAULT_CLIENT_GPC_TPCCS_2:
            return UVM_HOPPER_GPC_UTLB_ID_LTP5;
        case NV_PFAULT_CLIENT_GPC_T1_6:
            return UVM_HOPPER_GPC_UTLB_ID_LTP6;
        case NV_PFAULT_CLIENT_GPC_T1_7:
        case NV_PFAULT_CLIENT_GPC_PE_3:
        case NV_PFAULT_CLIENT_GPC_TPCCS_3:
            return UVM_HOPPER_GPC_UTLB_ID_LTP7;
        case NV_PFAULT_CLIENT_GPC_T1_8:
            return UVM_HOPPER_GPC_UTLB_ID_LTP8;
        case NV_PFAULT_CLIENT_GPC_T1_9:
        case NV_PFAULT_CLIENT_GPC_PE_4:
        case NV_PFAULT_CLIENT_GPC_TPCCS_4:
            return UVM_HOPPER_GPC_UTLB_ID_LTP9;
        case NV_PFAULT_CLIENT_GPC_T1_10:
            return UVM_HOPPER_GPC_UTLB_ID_LTP10;
        case NV_PFAULT_CLIENT_GPC_T1_11:
        case NV_PFAULT_CLIENT_GPC_PE_5:
        case NV_PFAULT_CLIENT_GPC_TPCCS_5:
            return UVM_HOPPER_GPC_UTLB_ID_LTP11;
        case NV_PFAULT_CLIENT_GPC_T1_12:
            return UVM_HOPPER_GPC_UTLB_ID_LTP12;
        case NV_PFAULT_CLIENT_GPC_T1_13:
        case NV_PFAULT_CLIENT_GPC_PE_6:
        case NV_PFAULT_CLIENT_GPC_TPCCS_6:
            return UVM_HOPPER_GPC_UTLB_ID_LTP13;
        case NV_PFAULT_CLIENT_GPC_T1_14:
            return UVM_HOPPER_GPC_UTLB_ID_LTP14;
        case NV_PFAULT_CLIENT_GPC_T1_15:
        case NV_PFAULT_CLIENT_GPC_PE_7:
        case NV_PFAULT_CLIENT_GPC_TPCCS_7:
            return UVM_HOPPER_GPC_UTLB_ID_LTP15;
        case NV_PFAULT_CLIENT_GPC_T1_16:
            return UVM_HOPPER_GPC_UTLB_ID_LTP16;
        case NV_PFAULT_CLIENT_GPC_T1_17:
        case NV_PFAULT_CLIENT_GPC_PE_8:
        case NV_PFAULT_CLIENT_GPC_TPCCS_8:
            return UVM_HOPPER_GPC_UTLB_ID_LTP17;

        default:
            UVM_ASSERT_MSG(false, "Invalid client value: 0x%x\n", client_id);
    }

    return 0;
}
