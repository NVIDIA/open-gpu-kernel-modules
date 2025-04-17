/*******************************************************************************
    Copyright (c) 2022-2024 NVIDIA Corporation

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

// On Blackwell, the UVM page tree 'depth' maps to hardware as follows:
//
// UVM depth   HW level                            VA bits
// 0           PDE4                                56:56
// 1           PDE3                                55:47
// 2           PDE2 (or 256G PTE)                  46:38
// 3           PDE1 (or 512M PTE)                  37:29
// 4           PDE0 (dual 64K/4K PDE, or 2M PTE)   28:21
// 5           PTE_64K / PTE_4K                    20:16 / 20:12

#include "uvm_types.h"
#include "uvm_global.h"
#include "uvm_hal.h"
#include "uvm_hal_types.h"
#include "uvm_blackwell_fault_buffer.h"
#include "hwref/blackwell/gb100/dev_fault.h"

static uvm_mmu_mode_hal_t blackwell_mmu_mode_hal;
static uvm_mmu_mode_hal_t blackwell_integrated_mmu_mode_hal;

static NvU32 page_table_depth_blackwell(NvU64 page_size)
{
    switch (page_size) {
        case UVM_PAGE_SIZE_2M:
            return 4;
        case UVM_PAGE_SIZE_512M:
            return 3;
        case UVM_PAGE_SIZE_256G:
            return 2;
        default:
            return 5;
    }
}

static NvU64 page_sizes_blackwell(void)
{
    return UVM_PAGE_SIZE_256G | UVM_PAGE_SIZE_512M | UVM_PAGE_SIZE_2M | UVM_PAGE_SIZE_64K | UVM_PAGE_SIZE_4K;
}

static NvU64 page_sizes_blackwell_integrated(void)
{
    return UVM_PAGE_SIZE_2M | UVM_PAGE_SIZE_64K | UVM_PAGE_SIZE_4K;
}

static uvm_mmu_mode_hal_t *__uvm_hal_mmu_mode_blackwell(uvm_mmu_mode_hal_t *mmu_mode_hal,
                                                        NvU64 big_page_size)
{
    uvm_mmu_mode_hal_t *hopper_mmu_mode_hal;

    UVM_ASSERT(big_page_size == UVM_PAGE_SIZE_64K || big_page_size == UVM_PAGE_SIZE_128K);

    hopper_mmu_mode_hal = uvm_hal_mmu_mode_hopper(big_page_size);
    UVM_ASSERT(hopper_mmu_mode_hal);

    // The assumption made is that arch_hal->mmu_mode_hal() will be called
    // under the global lock the first time, so check it here.
    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    *mmu_mode_hal = *hopper_mmu_mode_hal;
    mmu_mode_hal->page_table_depth = page_table_depth_blackwell;

    return mmu_mode_hal;
}

uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_blackwell(NvU64 big_page_size)
{
    static bool initialized = false;

    // TODO: Bug 1789555: RM should reject the creation of GPU VA spaces with
    // 128K big page size for Pascal+ GPUs
    if (big_page_size == UVM_PAGE_SIZE_128K)
        return NULL;

    if (!initialized) {
        uvm_mmu_mode_hal_t *mmu_mode_hal;

        mmu_mode_hal = __uvm_hal_mmu_mode_blackwell(&blackwell_mmu_mode_hal, big_page_size);
        mmu_mode_hal->page_sizes = page_sizes_blackwell;
        initialized = true;
    }

    return &blackwell_mmu_mode_hal;
}

uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_blackwell_integrated(NvU64 big_page_size)
{
    static bool initialized = false;

    // TODO: Bug 1789555: RM should reject the creation of GPU VA spaces with
    // 128K big page size for Pascal+ GPUs
    if (big_page_size == UVM_PAGE_SIZE_128K)
        return NULL;

    if (!initialized) {
        uvm_mmu_mode_hal_t *mmu_mode_hal;

        mmu_mode_hal = __uvm_hal_mmu_mode_blackwell(&blackwell_integrated_mmu_mode_hal, big_page_size);
        mmu_mode_hal->page_sizes = page_sizes_blackwell_integrated;
        initialized = true;
    }

    return &blackwell_integrated_mmu_mode_hal;
}

NvU16 uvm_hal_blackwell_mmu_client_id_to_utlb_id(NvU16 client_id)
{
    switch (client_id) {
        case NV_PFAULT_CLIENT_GPC_RAST:
        case NV_PFAULT_CLIENT_GPC_GCC:
        case NV_PFAULT_CLIENT_GPC_GPCCS:
            return UVM_BLACKWELL_GPC_UTLB_ID_RGG;
        case NV_PFAULT_CLIENT_GPC_T1_0:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP0;
        case NV_PFAULT_CLIENT_GPC_T1_1:
        case NV_PFAULT_CLIENT_GPC_PE_0:
        case NV_PFAULT_CLIENT_GPC_TPCCS_0:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP1;
        case NV_PFAULT_CLIENT_GPC_T1_2:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP2;
        case NV_PFAULT_CLIENT_GPC_T1_3:
        case NV_PFAULT_CLIENT_GPC_PE_1:
        case NV_PFAULT_CLIENT_GPC_TPCCS_1:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP3;
        case NV_PFAULT_CLIENT_GPC_T1_4:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP4;
        case NV_PFAULT_CLIENT_GPC_T1_5:
        case NV_PFAULT_CLIENT_GPC_PE_2:
        case NV_PFAULT_CLIENT_GPC_TPCCS_2:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP5;
        case NV_PFAULT_CLIENT_GPC_T1_6:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP6;
        case NV_PFAULT_CLIENT_GPC_T1_7:
        case NV_PFAULT_CLIENT_GPC_PE_3:
        case NV_PFAULT_CLIENT_GPC_TPCCS_3:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP7;
        case NV_PFAULT_CLIENT_GPC_T1_8:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP8;
        case NV_PFAULT_CLIENT_GPC_T1_9:
        case NV_PFAULT_CLIENT_GPC_PE_4:
        case NV_PFAULT_CLIENT_GPC_TPCCS_4:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP9;
        case NV_PFAULT_CLIENT_GPC_T1_10:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP10;
        case NV_PFAULT_CLIENT_GPC_T1_11:
        case NV_PFAULT_CLIENT_GPC_PE_5:
        case NV_PFAULT_CLIENT_GPC_TPCCS_5:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP11;
        case NV_PFAULT_CLIENT_GPC_T1_12:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP12;
        case NV_PFAULT_CLIENT_GPC_T1_13:
        case NV_PFAULT_CLIENT_GPC_PE_6:
        case NV_PFAULT_CLIENT_GPC_TPCCS_6:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP13;
        case NV_PFAULT_CLIENT_GPC_T1_14:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP14;
        case NV_PFAULT_CLIENT_GPC_T1_15:
        case NV_PFAULT_CLIENT_GPC_PE_7:
        case NV_PFAULT_CLIENT_GPC_TPCCS_7:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP15;
        case NV_PFAULT_CLIENT_GPC_T1_16:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP16;
        case NV_PFAULT_CLIENT_GPC_T1_17:
        case NV_PFAULT_CLIENT_GPC_PE_8:
        case NV_PFAULT_CLIENT_GPC_TPCCS_8:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP17;
        case NV_PFAULT_CLIENT_GPC_T1_18:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP18;
        case NV_PFAULT_CLIENT_GPC_T1_19:
        case NV_PFAULT_CLIENT_GPC_PE_9:
        case NV_PFAULT_CLIENT_GPC_TPCCS_9:
            return UVM_BLACKWELL_GPC_UTLB_ID_LTP19;

        default:
            UVM_ASSERT_MSG(false, "Invalid client value: 0x%x\n", client_id);
    }

    return 0;
}
