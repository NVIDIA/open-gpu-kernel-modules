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
#include "hwref/turing/tu102/dev_mmu.h"
#include "hwref/turing/tu102/dev_fault.h"

// This is mainly a copy of make_pte_volta in uvm_volta_mmu. This version
// sets NV_MMU_PTE_KIND_GENERIC_MEMORY, instead, since NV_MMU_PTE_KIND_PITCH
// no longer exists.
static NvU64 make_pte_turing(uvm_aperture_t aperture, NvU64 address, uvm_prot_t prot, NvU64 flags)
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

    pte_bits |= HWVALUE64(_MMU_VER2, PTE, KIND, NV_MMU_PTE_KIND_GENERIC_MEMORY);

    return pte_bits;
}

static NvU64 make_sked_reflected_pte_turing(void)
{
    NvU64 pte_bits = 0;

    pte_bits |= HWCONST64(_MMU_VER2, PTE, VALID, TRUE);
    pte_bits |= HWVALUE64(_MMU_VER2, PTE, KIND, NV_MMU_PTE_KIND_SMSKED_MESSAGE);

    return pte_bits;
}

static NvU64 poisoned_pte_turing(void)
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

    NvU64 pte_bits = make_pte_turing(UVM_APERTURE_VID, phys_addr, UVM_PROT_READ_ONLY, UVM_MMU_PTE_FLAGS_NONE);
    return WRITE_HWCONST64(pte_bits, _MMU_VER2, PTE, PRIVILEGE, TRUE);
}


static uvm_mmu_mode_hal_t turing_mmu_mode_hal;

uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_turing(NvU64 big_page_size)
{
    static bool initialized = false;

    UVM_ASSERT(big_page_size == UVM_PAGE_SIZE_64K || big_page_size == UVM_PAGE_SIZE_128K);

    // TODO: Bug 1789555: RM should reject the creation of GPU VA spaces with
    // 128K big page size for Pascal+ GPUs
    if (big_page_size == UVM_PAGE_SIZE_128K)
        return NULL;

    if (!initialized) {
        uvm_mmu_mode_hal_t *volta_mmu_mode_hal = uvm_hal_mmu_mode_volta(big_page_size);
        UVM_ASSERT(volta_mmu_mode_hal);

        // The assumption made is that arch_hal->mmu_mode_hal() will be
        // called under the global lock the first time, so check it here.
        uvm_assert_mutex_locked(&g_uvm_global.global_lock);

        turing_mmu_mode_hal = *volta_mmu_mode_hal;
        turing_mmu_mode_hal.make_pte = make_pte_turing;
        turing_mmu_mode_hal.make_sked_reflected_pte = make_sked_reflected_pte_turing;
        turing_mmu_mode_hal.poisoned_pte = poisoned_pte_turing;

        initialized = true;
    }

    return &turing_mmu_mode_hal;
}
