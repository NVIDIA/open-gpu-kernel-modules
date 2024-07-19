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

// For Ampere, UVM page tree 'depth' maps to hardware as follows:
//
// UVM depth   HW level                            VA bits
// 0           PDE3                                48:47
// 1           PDE2                                46:38
// 2           PDE1 (or 512M PTE)                  37:29
// 3           PDE0 (dual 64k/4k PDE, or 2M PTE)   28:21
// 4           PTE_64K / PTE_4K                    20:16 / 20:12

#include "uvm_types.h"
#include "uvm_global.h"
#include "uvm_hal.h"
#include "uvm_ampere_fault_buffer.h"
#include "hwref/ampere/ga100/dev_fault.h"

static NvU32 page_table_depth_ampere(NvU64 page_size)
{
    // The common-case is page_size == UVM_PAGE_SIZE_2M, hence the first check
    if (page_size == UVM_PAGE_SIZE_2M)
        return 3;
    else if (page_size == UVM_PAGE_SIZE_512M)
        return 2;
    else
        return 4;
}

static NvU64 page_sizes_ampere(void)
{
    return UVM_PAGE_SIZE_512M | UVM_PAGE_SIZE_2M | UVM_PAGE_SIZE_64K | UVM_PAGE_SIZE_4K;
}

static uvm_mmu_mode_hal_t ampere_mmu_mode_hal;

uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_ampere(NvU64 big_page_size)
{
    static bool initialized = false;

    UVM_ASSERT(big_page_size == UVM_PAGE_SIZE_64K || big_page_size == UVM_PAGE_SIZE_128K);

    // TODO: Bug 1789555: RM should reject the creation of GPU VA spaces with
    // 128K big page size for Pascal+ GPUs
    if (big_page_size == UVM_PAGE_SIZE_128K)
        return NULL;

    if (!initialized) {
        uvm_mmu_mode_hal_t *turing_mmu_mode_hal = uvm_hal_mmu_mode_turing(big_page_size);
        UVM_ASSERT(turing_mmu_mode_hal);

        // The assumption made is that arch_hal->mmu_mode_hal() will be
        // called under the global lock the first time, so check it here.
        uvm_assert_mutex_locked(&g_uvm_global.global_lock);

        ampere_mmu_mode_hal = *turing_mmu_mode_hal;
        ampere_mmu_mode_hal.page_table_depth = page_table_depth_ampere;
        ampere_mmu_mode_hal.page_sizes = page_sizes_ampere;

        initialized = true;
    }

    return &ampere_mmu_mode_hal;
}

NvU16 uvm_hal_ampere_mmu_client_id_to_utlb_id(NvU16 client_id)
{
    switch (client_id) {
        case NV_PFAULT_CLIENT_GPC_RAST:
        case NV_PFAULT_CLIENT_GPC_GCC:
        case NV_PFAULT_CLIENT_GPC_GPCCS:
            return UVM_AMPERE_GPC_UTLB_ID_RGG;
        case NV_PFAULT_CLIENT_GPC_T1_0:
            return UVM_AMPERE_GPC_UTLB_ID_LTP0;
        case NV_PFAULT_CLIENT_GPC_T1_1:
        case NV_PFAULT_CLIENT_GPC_PE_0:
        case NV_PFAULT_CLIENT_GPC_TPCCS_0:
            return UVM_AMPERE_GPC_UTLB_ID_LTP1;
        case NV_PFAULT_CLIENT_GPC_T1_2:
            return UVM_AMPERE_GPC_UTLB_ID_LTP2;
        case NV_PFAULT_CLIENT_GPC_T1_3:
        case NV_PFAULT_CLIENT_GPC_PE_1:
        case NV_PFAULT_CLIENT_GPC_TPCCS_1:
            return UVM_AMPERE_GPC_UTLB_ID_LTP3;
        case NV_PFAULT_CLIENT_GPC_T1_4:
            return UVM_AMPERE_GPC_UTLB_ID_LTP4;
        case NV_PFAULT_CLIENT_GPC_T1_5:
        case NV_PFAULT_CLIENT_GPC_PE_2:
        case NV_PFAULT_CLIENT_GPC_TPCCS_2:
            return UVM_AMPERE_GPC_UTLB_ID_LTP5;
        case NV_PFAULT_CLIENT_GPC_T1_6:
            return UVM_AMPERE_GPC_UTLB_ID_LTP6;
        case NV_PFAULT_CLIENT_GPC_T1_7:
        case NV_PFAULT_CLIENT_GPC_PE_3:
        case NV_PFAULT_CLIENT_GPC_TPCCS_3:
            return UVM_AMPERE_GPC_UTLB_ID_LTP7;
        case NV_PFAULT_CLIENT_GPC_T1_8:
            return UVM_AMPERE_GPC_UTLB_ID_LTP8;
        case NV_PFAULT_CLIENT_GPC_T1_9:
        case NV_PFAULT_CLIENT_GPC_PE_4:
        case NV_PFAULT_CLIENT_GPC_TPCCS_4:
            return UVM_AMPERE_GPC_UTLB_ID_LTP9;
        case NV_PFAULT_CLIENT_GPC_T1_10:
            return UVM_AMPERE_GPC_UTLB_ID_LTP10;
        case NV_PFAULT_CLIENT_GPC_T1_11:
        case NV_PFAULT_CLIENT_GPC_PE_5:
        case NV_PFAULT_CLIENT_GPC_TPCCS_5:
            return UVM_AMPERE_GPC_UTLB_ID_LTP11;
        case NV_PFAULT_CLIENT_GPC_T1_12:
            return UVM_AMPERE_GPC_UTLB_ID_LTP12;
        case NV_PFAULT_CLIENT_GPC_T1_13:
        case NV_PFAULT_CLIENT_GPC_PE_6:
        case NV_PFAULT_CLIENT_GPC_TPCCS_6:
            return UVM_AMPERE_GPC_UTLB_ID_LTP13;
        case NV_PFAULT_CLIENT_GPC_T1_14:
            return UVM_AMPERE_GPC_UTLB_ID_LTP14;
        case NV_PFAULT_CLIENT_GPC_T1_15:
        case NV_PFAULT_CLIENT_GPC_PE_7:
        case NV_PFAULT_CLIENT_GPC_TPCCS_7:
            return UVM_AMPERE_GPC_UTLB_ID_LTP15;

        default:
            UVM_ASSERT_MSG(false, "Invalid client value: 0x%x\n", client_id);
    }

    return 0;
}
