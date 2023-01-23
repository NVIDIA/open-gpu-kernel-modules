/*******************************************************************************
    Copyright (c) 2020-2022 NVIDIA Corporation

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
#include "uvm_hal.h"
#include "uvm_hal_types.h"
#include "uvm_hopper_fault_buffer.h"
#include "hwref/hopper/gh100/dev_fault.h"
#include "hwref/hopper/gh100/dev_mmu.h"

#define MMU_BIG 0
#define MMU_SMALL 1

uvm_mmu_engine_type_t uvm_hal_hopper_mmu_engine_id_to_type(NvU16 mmu_engine_id)
{
    if (mmu_engine_id >= NV_PFAULT_MMU_ENG_ID_HOST0 && mmu_engine_id <= NV_PFAULT_MMU_ENG_ID_HOST44)
        return UVM_MMU_ENGINE_TYPE_HOST;

    if (mmu_engine_id >= NV_PFAULT_MMU_ENG_ID_CE0 && mmu_engine_id <= NV_PFAULT_MMU_ENG_ID_CE9)
        return UVM_MMU_ENGINE_TYPE_CE;

    // We shouldn't be servicing faults from any other engines
    UVM_ASSERT_MSG(mmu_engine_id >= NV_PFAULT_MMU_ENG_ID_GRAPHICS, "Unexpected engine ID: 0x%x\n", mmu_engine_id);

    return UVM_MMU_ENGINE_TYPE_GRAPHICS;
}

static NvU32 page_table_depth_hopper(NvU32 page_size)
{
    // The common-case is page_size == UVM_PAGE_SIZE_2M, hence the first check
    if (page_size == UVM_PAGE_SIZE_2M)
        return 4;
    else if (page_size == UVM_PAGE_SIZE_512M)
        return 3;
    return 5;
}

static NvU32 entries_per_index_hopper(NvU32 depth)
{
    UVM_ASSERT(depth < 6);
    if (depth == 4)
        return 2;
    return 1;
}

static NvLength entry_offset_hopper(NvU32 depth, NvU32 page_size)
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

static NvU32 index_bits_hopper(NvU32 depth, NvU32 page_size)
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

static NvLength allocation_size_hopper(NvU32 depth, NvU32 page_size)
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
    return HWCONST64(_MMU_VER3, PTE, VALID, TRUE) |
           HWVALUE64(_MMU_VER3, PTE, PCF, pte_pcf(UVM_PROT_READ_WRITE_ATOMIC, UVM_MMU_PTE_FLAGS_NONE)) |
           HWVALUE64(_MMU_VER3, PTE, KIND, NV_MMU_PTE_KIND_SMSKED_MESSAGE);
}

static NvU64 make_sparse_pte_hopper(void)
{
    return HWCONST64(_MMU_VER3, PTE, VALID, FALSE) |
           HWCONST64(_MMU_VER3, PTE, PCF, SPARSE);
}

static NvU64 unmapped_pte_hopper(NvU32 page_size)
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

static NvU64 single_pde_hopper(uvm_mmu_page_table_alloc_t *phys_alloc, NvU32 depth)
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

        // PCF (permission control flags) 5:3
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
        //
        // TODO: Bug 3254055: Relax the NO_ATS setting from 512MB (pde1) range
        // to PTEs.
        if (depth == 3 && g_uvm_global.ats.enabled)
            pde_bits |= HWCONST64(_MMU_VER3, PDE, PCF, VALID_UNCACHED_ATS_NOT_ALLOWED);
        else
            pde_bits |= HWCONST64(_MMU_VER3, PDE, PCF, VALID_UNCACHED_ATS_ALLOWED);

        // address 51:12
        pde_bits |= HWVALUE64(_MMU_VER3, PDE, ADDRESS, address);
    }

    return pde_bits;
}

static NvU64 big_half_pde_hopper(uvm_mmu_page_table_alloc_t *phys_alloc)
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

        // PCF (permission control flags) 5:3
        pde_bits |= HWCONST64(_MMU_VER3, DUAL_PDE, PCF_BIG, VALID_UNCACHED_ATS_NOT_ALLOWED);

        // address 51:8
        pde_bits |= HWVALUE64(_MMU_VER3, DUAL_PDE, ADDRESS_BIG, address);
    }

    return pde_bits;
}

static NvU64 small_half_pde_hopper(uvm_mmu_page_table_alloc_t *phys_alloc)
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

        // PCF (permission control flags) 69:67 [5:3]
        pde_bits |= HWCONST64(_MMU_VER3, DUAL_PDE, PCF_SMALL, VALID_UNCACHED_ATS_NOT_ALLOWED);

        // address 115:76 [51:12]
        pde_bits |= HWVALUE64(_MMU_VER3, DUAL_PDE, ADDRESS_SMALL, address);
    }
    return pde_bits;
}

static void make_pde_hopper(void *entry, uvm_mmu_page_table_alloc_t **phys_allocs, NvU32 depth)
{
    NvU32 entry_count = entries_per_index_hopper(depth);
    NvU64 *entry_bits = (NvU64 *)entry;

    if (entry_count == 1) {
        *entry_bits = single_pde_hopper(*phys_allocs, depth);
    }
    else if (entry_count == 2) {
        entry_bits[MMU_BIG] = big_half_pde_hopper(phys_allocs[MMU_BIG]);
        entry_bits[MMU_SMALL] = small_half_pde_hopper(phys_allocs[MMU_SMALL]);

        // This entry applies to the whole dual PDE but is stored in the lower
        // bits
        entry_bits[MMU_BIG] |= HWCONST64(_MMU_VER3, DUAL_PDE, IS_PTE, FALSE);
    }
    else {
        UVM_ASSERT_MSG(0, "Invalid number of entries per index: %d\n", entry_count);
    }
}

static uvm_mmu_mode_hal_t hopper_mmu_mode_hal;

uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_hopper(NvU32 big_page_size)
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
