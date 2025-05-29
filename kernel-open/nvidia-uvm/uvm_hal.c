/*******************************************************************************
    Copyright (c) 2015-2024 NVIDIA Corporation

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
#include "uvm_kvmalloc.h"

#include "cla16f.h"
#include "clb069.h"
#include "clb06f.h"
#include "clb0b5.h"
#include "clc06f.h"
#include "clc0b5.h"
#include "clc1b5.h"
#include "ctrl2080mc.h"
#include "clc3b5.h"
#include "clc36f.h"
#include "clc369.h"
#include "clc365.h"
#include "clc46f.h"
#include "clc5b5.h"
#include "clc6b5.h"
#include "clc56f.h"
#include "clc7b5.h"
#include "clc86f.h"
#include "clc8b5.h"
#include "clc96f.h"
#include "clc9b5.h"
#include "clca6f.h"
#include "clcab5.h"

static int uvm_downgrade_force_membar_sys = 1;
module_param(uvm_downgrade_force_membar_sys, uint, 0644);
MODULE_PARM_DESC(uvm_downgrade_force_membar_sys, "Force all TLB invalidation downgrades to use MEMBAR_SYS");

#define CE_OP_COUNT (sizeof(uvm_ce_hal_t) / sizeof(void *))
#define HOST_OP_COUNT (sizeof(uvm_host_hal_t) / sizeof(void *))
#define ARCH_OP_COUNT (sizeof(uvm_arch_hal_t) / sizeof(void *))
#define FAULT_BUFFER_OP_COUNT (sizeof(uvm_fault_buffer_hal_t) / sizeof(void *))
#define ACCESS_COUNTER_BUFFER_OP_COUNT (sizeof(uvm_access_counter_buffer_hal_t) / sizeof(void *))
#define SEC2_OP_COUNT (sizeof(uvm_sec2_hal_t) / sizeof(void *))

// Table for copy engine functions.
// Each entry is associated with a copy engine class through the 'class' field.
// By setting the 'parent_class' field, a class will inherit the parent class's
// functions for any fields left NULL when uvm_hal_init_table() runs upon module
// load. The parent class must appear earlier in the array than the child.
static uvm_hal_class_ops_t ce_table[] =
{
    {
        .id = MAXWELL_DMA_COPY_A,
        .u.ce_ops = {
            .init = uvm_hal_maxwell_ce_init,
            .method_is_valid = uvm_hal_method_is_valid_stub,
            .semaphore_release = uvm_hal_maxwell_ce_semaphore_release,
            .semaphore_timestamp = uvm_hal_maxwell_ce_semaphore_timestamp,
            .semaphore_reduction_inc = uvm_hal_maxwell_ce_semaphore_reduction_inc,
            .semaphore_target_is_valid = uvm_hal_maxwell_semaphore_target_is_valid,
            .offset_out = uvm_hal_maxwell_ce_offset_out,
            .offset_in_out = uvm_hal_maxwell_ce_offset_in_out,
            .phys_mode = uvm_hal_maxwell_ce_phys_mode,
            .plc_mode = uvm_hal_maxwell_ce_plc_mode,
            .memcopy_copy_type = uvm_hal_maxwell_ce_memcopy_copy_type,
            .memcopy_is_valid = uvm_hal_maxwell_ce_memcopy_is_valid,
            .memcopy_patch_src = uvm_hal_ce_memcopy_patch_src_stub,
            .memcopy = uvm_hal_maxwell_ce_memcopy,
            .memcopy_v_to_v = uvm_hal_maxwell_ce_memcopy_v_to_v,
            .memset_is_valid = uvm_hal_maxwell_ce_memset_is_valid,
            .memset_1 = uvm_hal_maxwell_ce_memset_1,
            .memset_4 = uvm_hal_maxwell_ce_memset_4,
            .memset_8 = uvm_hal_maxwell_ce_memset_8,
            .memset_v_4 = uvm_hal_maxwell_ce_memset_v_4,
            .encrypt = uvm_hal_maxwell_ce_encrypt_unsupported,
            .decrypt = uvm_hal_maxwell_ce_decrypt_unsupported,
        }
    },
    {
        .id = PASCAL_DMA_COPY_A,
        .parent_id = MAXWELL_DMA_COPY_A,
        .u.ce_ops = {
            .semaphore_release = uvm_hal_pascal_ce_semaphore_release,
            .semaphore_timestamp = uvm_hal_pascal_ce_semaphore_timestamp,
            .semaphore_reduction_inc = uvm_hal_pascal_ce_semaphore_reduction_inc,
            .offset_out = uvm_hal_pascal_ce_offset_out,
            .offset_in_out = uvm_hal_pascal_ce_offset_in_out,
        }
    },
    {
        .id = PASCAL_DMA_COPY_B,
        .parent_id = PASCAL_DMA_COPY_A,
        .u.ce_ops = {}
    },
    {
        .id = VOLTA_DMA_COPY_A,
        .parent_id = PASCAL_DMA_COPY_B,
        .u.ce_ops = {
            .semaphore_release = uvm_hal_volta_ce_semaphore_release,
            .semaphore_timestamp = uvm_hal_volta_ce_semaphore_timestamp,
            .semaphore_reduction_inc = uvm_hal_volta_ce_semaphore_reduction_inc,
            .memcopy = uvm_hal_volta_ce_memcopy,
            .memset_1 = uvm_hal_volta_ce_memset_1,
            .memset_4 = uvm_hal_volta_ce_memset_4,
            .memset_8 = uvm_hal_volta_ce_memset_8,
        },
    },
    {
        .id = TURING_DMA_COPY_A,
        .parent_id = VOLTA_DMA_COPY_A,
        .u.ce_ops = {},
    },
    {
        .id = AMPERE_DMA_COPY_A,
        .parent_id = TURING_DMA_COPY_A,
        .u.ce_ops = {
            .method_is_valid = uvm_hal_ampere_ce_method_is_valid_c6b5,
            .phys_mode = uvm_hal_ampere_ce_phys_mode,
            .memcopy_is_valid = uvm_hal_ampere_ce_memcopy_is_valid_c6b5,
            .memcopy_patch_src = uvm_hal_ampere_ce_memcopy_patch_src_c6b5,
            .memset_is_valid = uvm_hal_ampere_ce_memset_is_valid_c6b5,
        },
    },
    {
        .id = AMPERE_DMA_COPY_B,
        .parent_id = AMPERE_DMA_COPY_A,
        .u.ce_ops = {
            .method_is_valid = uvm_hal_method_is_valid_stub,
            .plc_mode = uvm_hal_ampere_ce_plc_mode_c7b5,
            .memcopy_is_valid = uvm_hal_maxwell_ce_memcopy_is_valid,
            .memcopy_patch_src = uvm_hal_ce_memcopy_patch_src_stub,
            .memset_is_valid = uvm_hal_maxwell_ce_memset_is_valid,
        },
    },
    {
        .id = HOPPER_DMA_COPY_A,
        .parent_id = AMPERE_DMA_COPY_B,
        .u.ce_ops = {
            .semaphore_release = uvm_hal_hopper_ce_semaphore_release,
            .semaphore_timestamp = uvm_hal_hopper_ce_semaphore_timestamp,
            .semaphore_reduction_inc = uvm_hal_hopper_ce_semaphore_reduction_inc,
            .offset_out = uvm_hal_hopper_ce_offset_out,
            .offset_in_out = uvm_hal_hopper_ce_offset_in_out,
            .memcopy_copy_type = uvm_hal_hopper_ce_memcopy_copy_type,
            .memset_1 = uvm_hal_hopper_ce_memset_1,
            .memset_4 = uvm_hal_hopper_ce_memset_4,
            .memset_8 = uvm_hal_hopper_ce_memset_8,
            .memcopy_is_valid = uvm_hal_hopper_ce_memcopy_is_valid,
            .memset_is_valid = uvm_hal_hopper_ce_memset_is_valid,
            .encrypt = uvm_hal_hopper_ce_encrypt,
            .decrypt = uvm_hal_hopper_ce_decrypt,
        },
    },
    {
        .id = BLACKWELL_DMA_COPY_A,
        .parent_id = HOPPER_DMA_COPY_A,
        .u.ce_ops = {},
    },
    {
        .id = BLACKWELL_DMA_COPY_B,
        .parent_id = BLACKWELL_DMA_COPY_A,
        .u.ce_ops = {},
    },
};

// Table for GPFIFO functions.  Same idea as the copy engine table.
static uvm_hal_class_ops_t host_table[] =
{
    {
        // This host class is reported for GM10x
        .id = KEPLER_CHANNEL_GPFIFO_B,
        .u.host_ops = {
            .init = uvm_hal_maxwell_host_init_noop,
            .method_is_valid = uvm_hal_method_is_valid_stub,
            .sw_method_is_valid = uvm_hal_method_is_valid_stub,
            .wait_for_idle = uvm_hal_maxwell_host_wait_for_idle,
            .membar_sys = uvm_hal_maxwell_host_membar_sys,

            // No MEMBAR GPU until Pascal, just do a MEMBAR SYS.
            .membar_gpu = uvm_hal_maxwell_host_membar_sys,
            .noop = uvm_hal_maxwell_host_noop,
            .interrupt = uvm_hal_maxwell_host_interrupt,
            .semaphore_acquire = uvm_hal_maxwell_host_semaphore_acquire,
            .semaphore_release = uvm_hal_maxwell_host_semaphore_release,
            .semaphore_timestamp = uvm_hal_maxwell_host_semaphore_timestamp,
            .semaphore_target_is_valid = uvm_hal_maxwell_semaphore_target_is_valid,
            .set_gpfifo_entry = uvm_hal_maxwell_host_set_gpfifo_entry,
            .set_gpfifo_noop = uvm_hal_maxwell_host_set_gpfifo_noop,
            .set_gpfifo_pushbuffer_segment_base = uvm_hal_maxwell_host_set_gpfifo_pushbuffer_segment_base_unsupported,
            .write_gpu_put = uvm_hal_maxwell_host_write_gpu_put,
            .tlb_invalidate_all = uvm_hal_maxwell_host_tlb_invalidate_all_a16f,
            .tlb_invalidate_va = uvm_hal_maxwell_host_tlb_invalidate_va,
            .tlb_invalidate_test = uvm_hal_maxwell_host_tlb_invalidate_test,
            .replay_faults = uvm_hal_maxwell_replay_faults_unsupported,
            .cancel_faults_global = uvm_hal_maxwell_cancel_faults_global_unsupported,
            .cancel_faults_targeted = uvm_hal_maxwell_cancel_faults_targeted_unsupported,
            .cancel_faults_va = uvm_hal_maxwell_cancel_faults_va_unsupported,
            .clear_faulted_channel_sw_method = uvm_hal_maxwell_host_clear_faulted_channel_sw_method_unsupported,
            .clear_faulted_channel_method = uvm_hal_maxwell_host_clear_faulted_channel_method_unsupported,
            .clear_faulted_channel_register = uvm_hal_maxwell_host_clear_faulted_channel_register_unsupported,
            .access_counter_clear_all = uvm_hal_maxwell_access_counter_clear_all_unsupported,
            .access_counter_clear_targeted = uvm_hal_maxwell_access_counter_clear_targeted_unsupported,
            .access_counter_query_clear_op = uvm_hal_maxwell_access_counter_query_clear_op_unsupported,
            .get_time = uvm_hal_maxwell_get_time,
        }
    },
    {
        // This host class is reported for GM20x
        .id = MAXWELL_CHANNEL_GPFIFO_A,
        .parent_id = KEPLER_CHANNEL_GPFIFO_B,
        .u.host_ops = {
            .tlb_invalidate_all = uvm_hal_maxwell_host_tlb_invalidate_all_b06f,
        }
    },
    {
        .id = PASCAL_CHANNEL_GPFIFO_A,
        .parent_id = MAXWELL_CHANNEL_GPFIFO_A,
        .u.host_ops = {
            .init = uvm_hal_pascal_host_init,
            .membar_sys = uvm_hal_pascal_host_membar_sys,
            .membar_gpu = uvm_hal_pascal_host_membar_gpu,
            .tlb_invalidate_all = uvm_hal_pascal_host_tlb_invalidate_all,
            .tlb_invalidate_va = uvm_hal_pascal_host_tlb_invalidate_va,
            .tlb_invalidate_test = uvm_hal_pascal_host_tlb_invalidate_test,
            .replay_faults = uvm_hal_pascal_replay_faults,
            .cancel_faults_global = uvm_hal_pascal_cancel_faults_global,
            .cancel_faults_targeted = uvm_hal_pascal_cancel_faults_targeted,
        }
    },
    {
        .id = VOLTA_CHANNEL_GPFIFO_A,
        .parent_id = PASCAL_CHANNEL_GPFIFO_A,
        .u.host_ops = {
            .write_gpu_put = uvm_hal_volta_host_write_gpu_put,
            .tlb_invalidate_va = uvm_hal_volta_host_tlb_invalidate_va,
            .replay_faults = uvm_hal_volta_replay_faults,
            .cancel_faults_va = uvm_hal_volta_cancel_faults_va,
            .clear_faulted_channel_method = uvm_hal_volta_host_clear_faulted_channel_method,
            .semaphore_timestamp = uvm_hal_volta_host_semaphore_timestamp,
        }
    },
    {
        .id = TURING_CHANNEL_GPFIFO_A,
        .parent_id = VOLTA_CHANNEL_GPFIFO_A,
        .u.host_ops = {
            .semaphore_acquire = uvm_hal_turing_host_semaphore_acquire,
            .semaphore_release = uvm_hal_turing_host_semaphore_release,
            .clear_faulted_channel_method = uvm_hal_turing_host_clear_faulted_channel_method,
            .set_gpfifo_entry = uvm_hal_turing_host_set_gpfifo_entry,
            .tlb_invalidate_all = uvm_hal_turing_host_tlb_invalidate_all,
            .tlb_invalidate_va = uvm_hal_turing_host_tlb_invalidate_va,
            .tlb_invalidate_test = uvm_hal_turing_host_tlb_invalidate_test,
            .access_counter_clear_all = uvm_hal_turing_access_counter_clear_all,
            .access_counter_clear_targeted = uvm_hal_turing_access_counter_clear_targeted,
            .access_counter_query_clear_op = uvm_hal_turing_access_counter_query_clear_op,
        }
    },
    {
        .id = AMPERE_CHANNEL_GPFIFO_A,
        .parent_id = TURING_CHANNEL_GPFIFO_A,
        .u.host_ops = {
            .method_is_valid = uvm_hal_ampere_host_method_is_valid,
            .sw_method_is_valid = uvm_hal_ampere_host_sw_method_is_valid,
            .clear_faulted_channel_sw_method = uvm_hal_ampere_host_clear_faulted_channel_sw_method,
            .clear_faulted_channel_register = uvm_hal_ampere_host_clear_faulted_channel_register,
            .tlb_invalidate_all = uvm_hal_ampere_host_tlb_invalidate_all,
            .tlb_invalidate_va = uvm_hal_ampere_host_tlb_invalidate_va,
            .tlb_invalidate_test = uvm_hal_ampere_host_tlb_invalidate_test,
        }
    },
    {
        .id = HOPPER_CHANNEL_GPFIFO_A,
        .parent_id = AMPERE_CHANNEL_GPFIFO_A,
        .u.host_ops = {
            .method_is_valid = uvm_hal_method_is_valid_stub,
            .sw_method_is_valid = uvm_hal_method_is_valid_stub,
            .semaphore_acquire = uvm_hal_hopper_host_semaphore_acquire,
            .semaphore_release = uvm_hal_hopper_host_semaphore_release,
            .semaphore_timestamp = uvm_hal_hopper_host_semaphore_timestamp,
            .tlb_invalidate_all = uvm_hal_hopper_host_tlb_invalidate_all,
            .tlb_invalidate_va = uvm_hal_hopper_host_tlb_invalidate_va,
            .tlb_invalidate_test = uvm_hal_hopper_host_tlb_invalidate_test,
            .cancel_faults_va = uvm_hal_hopper_cancel_faults_va,
            .set_gpfifo_entry = uvm_hal_hopper_host_set_gpfifo_entry,
            .set_gpfifo_pushbuffer_segment_base = uvm_hal_hopper_host_set_gpfifo_pushbuffer_segment_base,
        }
    },
    {
        .id = BLACKWELL_CHANNEL_GPFIFO_A,
        .parent_id = HOPPER_CHANNEL_GPFIFO_A,
        .u.host_ops = {
            .tlb_invalidate_all = uvm_hal_blackwell_host_tlb_invalidate_all,
            .tlb_invalidate_va = uvm_hal_blackwell_host_tlb_invalidate_va,
            .tlb_invalidate_test = uvm_hal_blackwell_host_tlb_invalidate_test,
            .access_counter_query_clear_op = uvm_hal_blackwell_access_counter_query_clear_op_gb100,
        }
    },
    {
        .id = BLACKWELL_CHANNEL_GPFIFO_B,
        .parent_id = BLACKWELL_CHANNEL_GPFIFO_A,
        .u.host_ops = {
            .access_counter_query_clear_op = uvm_hal_blackwell_access_counter_query_clear_op_gb20x
        }
    },
};

static uvm_hal_class_ops_t arch_table[] =
{
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM000,
        .u.arch_ops = {
            .init_properties = uvm_hal_maxwell_arch_init_properties,
            .mmu_mode_hal = uvm_hal_mmu_mode_maxwell,
            .enable_prefetch_faults = uvm_hal_maxwell_mmu_enable_prefetch_faults_unsupported,
            .disable_prefetch_faults = uvm_hal_maxwell_mmu_disable_prefetch_faults_unsupported,
            .mmu_client_id_to_utlb_id = uvm_hal_maxwell_mmu_client_id_to_utlb_id_unsupported,
        }
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM200,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM000,
        .u.arch_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GP100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM200,
        .u.arch_ops = {
            .init_properties = uvm_hal_pascal_arch_init_properties,
            .mmu_mode_hal = uvm_hal_mmu_mode_pascal,
            .enable_prefetch_faults = uvm_hal_pascal_mmu_enable_prefetch_faults,
            .disable_prefetch_faults = uvm_hal_pascal_mmu_disable_prefetch_faults,
            .mmu_client_id_to_utlb_id = uvm_hal_pascal_mmu_client_id_to_utlb_id,
        }
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GV100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GP100,
        .u.arch_ops = {
            .init_properties = uvm_hal_volta_arch_init_properties,
            .mmu_mode_hal = uvm_hal_mmu_mode_volta,
            .mmu_client_id_to_utlb_id = uvm_hal_volta_mmu_client_id_to_utlb_id,
        },
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_TU100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GV100,
        .u.arch_ops = {
            .init_properties = uvm_hal_turing_arch_init_properties,
            .mmu_mode_hal = uvm_hal_mmu_mode_turing,
        },
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GA100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_TU100,
        .u.arch_ops = {
            .init_properties = uvm_hal_ampere_arch_init_properties,
            .mmu_mode_hal = uvm_hal_mmu_mode_ampere,
            .mmu_client_id_to_utlb_id = uvm_hal_ampere_mmu_client_id_to_utlb_id,
        },
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_AD100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GA100,
        .u.arch_ops = {
            .init_properties = uvm_hal_ada_arch_init_properties,
        },
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GH100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_AD100,
        .u.arch_ops = {
            .init_properties = uvm_hal_hopper_arch_init_properties,
            .mmu_mode_hal = uvm_hal_mmu_mode_hopper,
            .mmu_client_id_to_utlb_id = uvm_hal_hopper_mmu_client_id_to_utlb_id,
        },
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GH100,
        .u.arch_ops = {
            .init_properties = uvm_hal_blackwell_arch_init_properties,
            .mmu_mode_hal = uvm_hal_mmu_mode_blackwell,
            .mmu_client_id_to_utlb_id = uvm_hal_blackwell_mmu_client_id_to_utlb_id,
        }
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB200,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB100,
        .u.arch_ops = {
            // Note that GB20x MMU behaves as Hopper MMU, so it inherits from
            // Hopper's MMU, not from GB10x.
            .mmu_mode_hal = uvm_hal_mmu_mode_hopper,
        }
    },
};

// chip_table[] is different from the other class op tables - it is used to
// apply chip specific overrides to arch ops. This means unlike the other class
// op tables, parent_id does not refer to a preceding entry within the table
// itself. parent_id is an architecture (not a chip id) and instead refers to an
// entry in arch_table[]. This means that arch_table[] must be initialized
// before chip_table[]. chip_table[] must be initialized using
// ops_init_from_table(arch_table) instead of ops_init_from_parent().
// TODO: BUG 5044266: the chip ops should be separated from the arch ops.
static uvm_hal_class_ops_t chip_table[] =
{
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB100 | NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GB10B,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB100,
        .u.arch_ops = {
            .mmu_mode_hal = uvm_hal_mmu_mode_blackwell_integrated,
        }
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB200 | NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GB20B,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB200,
        .u.arch_ops = {
            .mmu_mode_hal = uvm_hal_mmu_mode_blackwell_integrated,
        }
    },
};

static uvm_hal_class_ops_t fault_buffer_table[] =
{
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM000,
        .u.fault_buffer_ops = {
            .enable_replayable_faults  = uvm_hal_maxwell_enable_replayable_faults_unsupported,
            .disable_replayable_faults = uvm_hal_maxwell_disable_replayable_faults_unsupported,
            .clear_replayable_faults = uvm_hal_maxwell_clear_replayable_faults_unsupported,
            .read_put = uvm_hal_maxwell_fault_buffer_read_put_unsupported,
            .read_get = uvm_hal_maxwell_fault_buffer_read_get_unsupported,
            .write_get = uvm_hal_maxwell_fault_buffer_write_get_unsupported,
            .get_ve_id = uvm_hal_maxwell_fault_buffer_get_ve_id_unsupported,
            .get_mmu_engine_type = uvm_hal_maxwell_fault_buffer_get_mmu_engine_type_unsupported,
            .parse_replayable_entry = uvm_hal_maxwell_fault_buffer_parse_replayable_entry_unsupported,
            .entry_is_valid = uvm_hal_maxwell_fault_buffer_entry_is_valid_unsupported,
            .entry_clear_valid = uvm_hal_maxwell_fault_buffer_entry_clear_valid_unsupported,
            .entry_size = uvm_hal_maxwell_fault_buffer_entry_size_unsupported,
            .parse_non_replayable_entry = uvm_hal_maxwell_fault_buffer_parse_non_replayable_entry_unsupported,
            .get_fault_type = uvm_hal_maxwell_fault_buffer_get_fault_type_unsupported,
        }
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM200,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM000,
        .u.fault_buffer_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GP100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM200,
        .u.fault_buffer_ops = {
            .enable_replayable_faults  = uvm_hal_pascal_enable_replayable_faults,
            .disable_replayable_faults = uvm_hal_pascal_disable_replayable_faults,
            .clear_replayable_faults = uvm_hal_pascal_clear_replayable_faults,
            .read_put = uvm_hal_pascal_fault_buffer_read_put,
            .read_get = uvm_hal_pascal_fault_buffer_read_get,
            .write_get = uvm_hal_pascal_fault_buffer_write_get,
            .parse_replayable_entry = uvm_hal_pascal_fault_buffer_parse_replayable_entry,
            .entry_is_valid = uvm_hal_pascal_fault_buffer_entry_is_valid,
            .entry_clear_valid = uvm_hal_pascal_fault_buffer_entry_clear_valid,
            .entry_size = uvm_hal_pascal_fault_buffer_entry_size,
            .get_fault_type = uvm_hal_pascal_fault_buffer_get_fault_type,
        }
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GV100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GP100,
        .u.fault_buffer_ops = {
            .read_put = uvm_hal_volta_fault_buffer_read_put,
            .read_get = uvm_hal_volta_fault_buffer_read_get,
            .write_get = uvm_hal_volta_fault_buffer_write_get,
            .get_ve_id = uvm_hal_volta_fault_buffer_get_ve_id,
            .get_mmu_engine_type = uvm_hal_volta_fault_buffer_get_mmu_engine_type,
            .parse_replayable_entry = uvm_hal_volta_fault_buffer_parse_replayable_entry,
            .parse_non_replayable_entry = uvm_hal_volta_fault_buffer_parse_non_replayable_entry,
            .get_fault_type = uvm_hal_volta_fault_buffer_get_fault_type,
        }
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_TU100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GV100,
        .u.fault_buffer_ops = {
            .disable_replayable_faults = uvm_hal_turing_disable_replayable_faults,
            .clear_replayable_faults = uvm_hal_turing_clear_replayable_faults,
            .get_mmu_engine_type = uvm_hal_turing_fault_buffer_get_mmu_engine_type,
        }
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GA100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_TU100,
        .u.fault_buffer_ops = {
            .get_mmu_engine_type = uvm_hal_ampere_fault_buffer_get_mmu_engine_type,
        }
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_AD100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GA100,
        .u.fault_buffer_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GH100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_AD100,
        .u.fault_buffer_ops = {
            .get_ve_id = uvm_hal_hopper_fault_buffer_get_ve_id,
            .get_mmu_engine_type = uvm_hal_hopper_fault_buffer_get_mmu_engine_type,
        }
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GH100,
        .u.fault_buffer_ops = {
            .get_fault_type = uvm_hal_blackwell_fault_buffer_get_fault_type,
            .get_mmu_engine_type = uvm_hal_blackwell_fault_buffer_get_mmu_engine_type,
        }
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB200,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB100,
        .u.fault_buffer_ops = {}
    },
};

static uvm_hal_class_ops_t access_counter_buffer_table[] =
{
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM000,
        .u.access_counter_buffer_ops = {
            .enable_access_counter_notifications  = uvm_hal_maxwell_enable_access_counter_notifications_unsupported,
            .disable_access_counter_notifications = uvm_hal_maxwell_disable_access_counter_notifications_unsupported,
            .clear_access_counter_notifications = uvm_hal_maxwell_clear_access_counter_notifications_unsupported,
            .parse_entry = uvm_hal_maxwell_access_counter_buffer_parse_entry_unsupported,
            .entry_is_valid = uvm_hal_maxwell_access_counter_buffer_entry_is_valid_unsupported,
            .entry_clear_valid = uvm_hal_maxwell_access_counter_buffer_entry_clear_valid_unsupported,
            .entry_size = uvm_hal_maxwell_access_counter_buffer_entry_size_unsupported,
        }
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM200,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM000,
        .u.access_counter_buffer_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GP100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM200,
        .u.access_counter_buffer_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GV100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GP100,
        .u.access_counter_buffer_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_TU100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GV100,
        .u.access_counter_buffer_ops = {
            .enable_access_counter_notifications  = uvm_hal_turing_enable_access_counter_notifications,
            .disable_access_counter_notifications = uvm_hal_turing_disable_access_counter_notifications,
            .clear_access_counter_notifications = uvm_hal_turing_clear_access_counter_notifications,
            .parse_entry = uvm_hal_turing_access_counter_buffer_parse_entry,
            .entry_is_valid = uvm_hal_turing_access_counter_buffer_entry_is_valid,
            .entry_clear_valid = uvm_hal_turing_access_counter_buffer_entry_clear_valid,
            .entry_size = uvm_hal_turing_access_counter_buffer_entry_size,
        }
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GA100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_TU100,
        .u.access_counter_buffer_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_AD100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GA100,
        .u.access_counter_buffer_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GH100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_AD100,
        .u.access_counter_buffer_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GH100,
        .u.access_counter_buffer_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB200,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB100,
        .u.access_counter_buffer_ops = {}
    },
};

static uvm_hal_class_ops_t sec2_table[] =
{
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM000,
        .u.sec2_ops = {
            .init = uvm_hal_maxwell_sec2_init_noop,
            .decrypt = uvm_hal_maxwell_sec2_decrypt_unsupported,
            .semaphore_release = uvm_hal_maxwell_sec2_semaphore_release_unsupported,
            .semaphore_timestamp = uvm_hal_maxwell_sec2_semaphore_timestamp_unsupported,
            .semaphore_target_is_valid = uvm_hal_maxwell_semaphore_target_is_valid,
        }
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM200,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM000,
        .u.sec2_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GP100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM200,
        .u.sec2_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GV100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GP100,
        .u.sec2_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_TU100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GV100,
        .u.sec2_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GA100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_TU100,
        .u.sec2_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_AD100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GA100,
        .u.sec2_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GH100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_AD100,
        .u.sec2_ops = {
            .init = uvm_hal_hopper_sec2_init,
            .semaphore_release = uvm_hal_hopper_sec2_semaphore_release,
            .semaphore_timestamp = uvm_hal_hopper_sec2_semaphore_timestamp_unsupported,
            .decrypt = uvm_hal_hopper_sec2_decrypt,
        }
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB100,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GH100,
        .u.sec2_ops = {}
    },
    {
        .id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB200,
        .parent_id = NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB100,
        .u.sec2_ops = {}
    },
};

static inline uvm_hal_class_ops_t *ops_find_by_id(uvm_hal_class_ops_t *table, NvU32 row_count, NvU32 id)
{
    NvLength i;

    // go through array and match on class.
    for (i = 0; i < row_count; i++) {
        if (table[i].id == id)
            return table + i;
    }

    return NULL;
}

// use memcmp to check for function pointer assignment in a well defined,
// general way.
static inline bool op_is_null(uvm_hal_class_ops_t *row, NvLength op_idx, NvLength op_offset)
{
    void *temp = NULL;
    return memcmp(&temp, (char *)row + op_offset + sizeof(void *) * op_idx, sizeof(void *)) == 0;
}

// use memcpy to copy function pointers in a well defined, general way.
static inline void op_copy(uvm_hal_class_ops_t *dst, uvm_hal_class_ops_t *src, NvLength op_idx, NvLength op_offset)
{
    void *m_dst = (char *)dst + op_offset + sizeof(void *) * op_idx;
    void *m_src = (char *)src + op_offset + sizeof(void *) * op_idx;
    memcpy(m_dst, m_src, sizeof(void *));
}

static inline NV_STATUS ops_init_from_table(uvm_hal_class_ops_t *dest_table,
                                            NvU32 dest_row_count,
                                            uvm_hal_class_ops_t *src_table,
                                            NvU32 src_row_count,
                                            NvLength op_count,
                                            NvLength op_offset)
{
    NvLength i;

    for (i = 0; i < dest_row_count; i++) {
        NvLength j;
        uvm_hal_class_ops_t *parent = NULL;

        if (dest_table[i].parent_id != 0) {
            parent = ops_find_by_id(src_table, src_row_count, dest_table[i].parent_id);
            if (parent == NULL)
                return NV_ERR_INVALID_CLASS;

            // Go through all the ops and assign from parent's corresponding op
            // if NULL
            for (j = 0; j < op_count; j++) {
                if (op_is_null(dest_table + i, j, op_offset))
                    op_copy(dest_table + i, parent, j, op_offset);
            }
        }

        // At this point, it is an error to have missing HAL operations
        for (j = 0; j < op_count; j++) {
            if (op_is_null(dest_table + i, j, op_offset))
                return NV_ERR_INVALID_STATE;
        }
    }

    return NV_OK;
}

static inline NV_STATUS ops_init_from_parent(uvm_hal_class_ops_t *table,
                                             NvU32 row_count,
                                             NvLength op_count,
                                             NvLength op_offset)
{
    return ops_init_from_table(table,
                               row_count,
                               table,
                               row_count,
                               op_count,
                               op_offset);
}

NV_STATUS uvm_hal_init_table(void)
{
    NV_STATUS status;

    status = ops_init_from_parent(ce_table, ARRAY_SIZE(ce_table), CE_OP_COUNT, offsetof(uvm_hal_class_ops_t, u.ce_ops));
    if (status != NV_OK) {
        UVM_ERR_PRINT("ops_init_from_parent(ce_table) failed: %s\n", nvstatusToString(status));
        return status;
    }

    status = ops_init_from_parent(host_table,
                                  ARRAY_SIZE(host_table),
                                  HOST_OP_COUNT,
                                  offsetof(uvm_hal_class_ops_t, u.host_ops));
    if (status != NV_OK) {
        UVM_ERR_PRINT("ops_init_from_parent(host_table) failed: %s\n", nvstatusToString(status));
        return status;
    }

    status = ops_init_from_parent(arch_table,
                                  ARRAY_SIZE(arch_table),
                                  ARCH_OP_COUNT,
                                  offsetof(uvm_hal_class_ops_t, u.arch_ops));
    if (status != NV_OK) {
        UVM_ERR_PRINT("ops_init_from_parent(arch_table) failed: %s\n", nvstatusToString(status));
        return status;
    }

    // chip_table[] must be initialized after arch_table[].
    status = ops_init_from_table(chip_table,
                                 ARRAY_SIZE(chip_table),
                                 arch_table,
                                 ARRAY_SIZE(arch_table),
                                 ARCH_OP_COUNT,
                                 offsetof(uvm_hal_class_ops_t, u.arch_ops));
    if (status != NV_OK) {
        UVM_ERR_PRINT("ops_init_from_table(chip_table) failed: %s\n", nvstatusToString(status));
        return status;
    }

    status = ops_init_from_parent(fault_buffer_table,
                                  ARRAY_SIZE(fault_buffer_table),
                                  FAULT_BUFFER_OP_COUNT,
                                  offsetof(uvm_hal_class_ops_t, u.fault_buffer_ops));
    if (status != NV_OK) {
        UVM_ERR_PRINT("ops_init_from_parent(fault_buffer_table) failed: %s\n", nvstatusToString(status));
        return status;
    }

    status = ops_init_from_parent(access_counter_buffer_table,
                                  ARRAY_SIZE(access_counter_buffer_table),
                                  ACCESS_COUNTER_BUFFER_OP_COUNT,
                                  offsetof(uvm_hal_class_ops_t, u.access_counter_buffer_ops));
    if (status != NV_OK) {
        UVM_ERR_PRINT("ops_init_from_parent(access_counter_buffer_table) failed: %s\n", nvstatusToString(status));
        return status;
    }

    status = ops_init_from_parent(sec2_table,
                                  ARRAY_SIZE(sec2_table),
                                  SEC2_OP_COUNT,
                                  offsetof(uvm_hal_class_ops_t, u.sec2_ops));
    if (status != NV_OK) {
        UVM_ERR_PRINT("ops_init_from_parent(sec2_table) failed: %s\n", nvstatusToString(status));
        return status;
    }

    return NV_OK;
}

NV_STATUS uvm_hal_init_gpu(uvm_parent_gpu_t *parent_gpu)
{
    const UvmGpuInfo *gpu_info = &parent_gpu->rm_info;
    uvm_hal_class_ops_t *class_ops;

    class_ops = ops_find_by_id(ce_table, ARRAY_SIZE(ce_table), gpu_info->ceClass);
    if (class_ops == NULL) {
        UVM_ERR_PRINT("Unsupported ce class: 0x%X, GPU %s\n",
                      gpu_info->ceClass,
                      uvm_parent_gpu_name(parent_gpu));
        return NV_ERR_INVALID_CLASS;
    }

    parent_gpu->ce_hal = &class_ops->u.ce_ops;

    class_ops = ops_find_by_id(host_table, ARRAY_SIZE(host_table), gpu_info->hostClass);
    if (class_ops == NULL) {
        UVM_ERR_PRINT("Unsupported host class: 0x%X, GPU %s\n",
                      gpu_info->hostClass,
                      uvm_parent_gpu_name(parent_gpu));
        return NV_ERR_INVALID_CLASS;
    }

    parent_gpu->host_hal = &class_ops->u.host_ops;

    class_ops = ops_find_by_id(arch_table, ARRAY_SIZE(arch_table), gpu_info->gpuArch);
    if (class_ops == NULL) {
        UVM_ERR_PRINT("Unsupported GPU architecture: 0x%X, GPU %s\n",
                      gpu_info->gpuArch,
                      uvm_parent_gpu_name(parent_gpu));
        return NV_ERR_INVALID_CLASS;
    }

    parent_gpu->arch_hal = &class_ops->u.arch_ops;

    // Apply per chip overrides if required
    class_ops = ops_find_by_id(chip_table,
                               ARRAY_SIZE(chip_table),
                               gpu_info->gpuArch | gpu_info->gpuImplementation);
    if (class_ops)
        parent_gpu->arch_hal = &class_ops->u.arch_ops;

    class_ops = ops_find_by_id(fault_buffer_table, ARRAY_SIZE(fault_buffer_table), gpu_info->gpuArch);
    if (class_ops == NULL) {
        UVM_ERR_PRINT("Fault buffer HAL not found, GPU %s, arch: 0x%X\n",
                      uvm_parent_gpu_name(parent_gpu),
                      gpu_info->gpuArch);
        return NV_ERR_INVALID_CLASS;
    }

    parent_gpu->fault_buffer_hal = &class_ops->u.fault_buffer_ops;

    class_ops = ops_find_by_id(access_counter_buffer_table,
                               ARRAY_SIZE(access_counter_buffer_table),
                               gpu_info->gpuArch);
    if (class_ops == NULL) {
        UVM_ERR_PRINT("Access counter HAL not found, GPU %s, arch: 0x%X\n",
                      uvm_parent_gpu_name(parent_gpu),
                      gpu_info->gpuArch);
        return NV_ERR_INVALID_CLASS;
    }

    parent_gpu->access_counter_buffer_hal = &class_ops->u.access_counter_buffer_ops;

    class_ops = ops_find_by_id(sec2_table, ARRAY_SIZE(sec2_table), gpu_info->gpuArch);
    if (class_ops == NULL) {
        UVM_ERR_PRINT("SEC2 HAL not found, GPU %s, arch: 0x%X\n",
                      uvm_parent_gpu_name(parent_gpu),
                      gpu_info->gpuArch);
        return NV_ERR_INVALID_CLASS;
    }

    parent_gpu->sec2_hal = &class_ops->u.sec2_ops;

    return NV_OK;
}

static void hal_override_properties(uvm_parent_gpu_t *parent_gpu)
{
    // Access counters are currently not supported in vGPU or Confidential
    // Computing.
    //
    // TODO: Bug 200692962: Add support for access counters in vGPU
    if ((parent_gpu->virt_mode != UVM_VIRT_MODE_NONE) || g_uvm_global.conf_computing_enabled)
        parent_gpu->access_counters_supported = false;


    // TODO: Bug 4637114: [UVM] Remove support for physical access counter
    // notifications. Always set to false, until we remove the PMM reverse
    // mapping code.
    parent_gpu->access_counters_can_use_physical_addresses = false;
}

void uvm_hal_init_properties(uvm_parent_gpu_t *parent_gpu)
{
    parent_gpu->arch_hal->init_properties(parent_gpu);

    hal_override_properties(parent_gpu);
}

void uvm_hal_tlb_invalidate_membar(uvm_push_t *push, uvm_membar_t membar)
{
    uvm_gpu_t *gpu;
    NvU32 i;

    if (membar == UVM_MEMBAR_NONE)
        return;

    gpu = uvm_push_get_gpu(push);

    // TLB invalidate on Blackwell+ GPUs should not use a standalone membar.
    UVM_ASSERT(gpu->parent->rm_info.gpuArch < NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB100);

    for (i = 0; i < gpu->parent->num_hshub_tlb_invalidate_membars; i++)
        gpu->parent->host_hal->membar_gpu(push);

    uvm_hal_membar(gpu, push, membar);
}

bool uvm_hal_membar_before_semaphore(uvm_push_t *push)
{
    uvm_membar_t membar = uvm_push_get_and_reset_membar_flag(push);

    if (membar == UVM_MEMBAR_NONE) {
        // No MEMBAR requested, don't use a flush.
        return false;
    }

    if (membar == UVM_MEMBAR_GPU) {
        // MEMBAR GPU requested, do it on the HOST and skip the engine flush as
        // it doesn't have this capability.
        uvm_hal_wfi_membar(push, UVM_MEMBAR_GPU);
        return false;
    }

    // By default do a MEMBAR SYS and for that we can just use flush on the
    // semaphore operation.
    return true;
}

uvm_membar_t uvm_hal_downgrade_membar_type(uvm_gpu_t *gpu, bool is_local_vidmem)
{
    // If the mapped memory was local, and we're not using a coherence protocol,
    // we only need a GPU-local membar. This is because all accesses to this
    // memory, including those from other processors like the CPU or peer GPUs,
    // must come through this GPU's L2. In all current architectures, MEMBAR_GPU
    // is sufficient to resolve ordering at the L2 level.
    if (is_local_vidmem && !uvm_parent_gpu_is_coherent(gpu->parent) && !uvm_downgrade_force_membar_sys)
        return UVM_MEMBAR_GPU;

    // If the mapped memory was remote, or if a coherence protocol can cache
    // this GPU's memory, then there are external ways for other processors to
    // access the memory without always going the local GPU L2, so we must use a
    // MEMBAR_SYS.
    return UVM_MEMBAR_SYS;
}

const char *uvm_aperture_string(uvm_aperture_t aperture)
{
    BUILD_BUG_ON(UVM_APERTURE_MAX != 12);

    switch (aperture) {
        UVM_ENUM_STRING_CASE(UVM_APERTURE_PEER_0);
        UVM_ENUM_STRING_CASE(UVM_APERTURE_PEER_1);
        UVM_ENUM_STRING_CASE(UVM_APERTURE_PEER_2);
        UVM_ENUM_STRING_CASE(UVM_APERTURE_PEER_3);
        UVM_ENUM_STRING_CASE(UVM_APERTURE_PEER_4);
        UVM_ENUM_STRING_CASE(UVM_APERTURE_PEER_5);
        UVM_ENUM_STRING_CASE(UVM_APERTURE_PEER_6);
        UVM_ENUM_STRING_CASE(UVM_APERTURE_PEER_7);
        UVM_ENUM_STRING_CASE(UVM_APERTURE_PEER_MAX);
        UVM_ENUM_STRING_CASE(UVM_APERTURE_SYS);
        UVM_ENUM_STRING_CASE(UVM_APERTURE_VID);
        UVM_ENUM_STRING_CASE(UVM_APERTURE_DEFAULT);
        UVM_ENUM_STRING_DEFAULT();
    }
}

const char *uvm_prot_string(uvm_prot_t prot)
{
    BUILD_BUG_ON(UVM_PROT_MAX != 4);

    switch (prot) {
        UVM_ENUM_STRING_CASE(UVM_PROT_NONE);
        UVM_ENUM_STRING_CASE(UVM_PROT_READ_ONLY);
        UVM_ENUM_STRING_CASE(UVM_PROT_READ_WRITE);
        UVM_ENUM_STRING_CASE(UVM_PROT_READ_WRITE_ATOMIC);
        UVM_ENUM_STRING_DEFAULT();
    }
}

const char *uvm_membar_string(uvm_membar_t membar)
{
    switch (membar) {
        UVM_ENUM_STRING_CASE(UVM_MEMBAR_SYS);
        UVM_ENUM_STRING_CASE(UVM_MEMBAR_GPU);
        UVM_ENUM_STRING_CASE(UVM_MEMBAR_NONE);
    }

    return "UNKNOWN";
}

const char *uvm_fault_access_type_string(uvm_fault_access_type_t fault_access_type)
{
    BUILD_BUG_ON(UVM_FAULT_ACCESS_TYPE_COUNT != 5);

    switch (fault_access_type) {
        UVM_ENUM_STRING_CASE(UVM_FAULT_ACCESS_TYPE_ATOMIC_STRONG);
        UVM_ENUM_STRING_CASE(UVM_FAULT_ACCESS_TYPE_ATOMIC_WEAK);
        UVM_ENUM_STRING_CASE(UVM_FAULT_ACCESS_TYPE_WRITE);
        UVM_ENUM_STRING_CASE(UVM_FAULT_ACCESS_TYPE_READ);
        UVM_ENUM_STRING_CASE(UVM_FAULT_ACCESS_TYPE_PREFETCH);
        UVM_ENUM_STRING_DEFAULT();
    }
}

const char *uvm_fault_type_string(uvm_fault_type_t fault_type)
{
    BUILD_BUG_ON(UVM_FAULT_TYPE_COUNT != 17);

    switch (fault_type) {
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_INVALID_PDE);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_INVALID_PTE);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_ATOMIC);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_WRITE);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_READ);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_PDE_SIZE);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_VA_LIMIT_VIOLATION);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_UNBOUND_INST_BLOCK);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_PRIV_VIOLATION);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_PITCH_MASK_VIOLATION);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_WORK_CREATION);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_UNSUPPORTED_APERTURE);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_COMPRESSION_FAILURE);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_UNSUPPORTED_KIND);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_REGION_VIOLATION);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_POISONED);
        UVM_ENUM_STRING_CASE(UVM_FAULT_TYPE_CC_VIOLATION);
        UVM_ENUM_STRING_DEFAULT();
    }
}

const char *uvm_fault_client_type_string(uvm_fault_client_type_t fault_client_type)
{
    BUILD_BUG_ON(UVM_FAULT_CLIENT_TYPE_COUNT != 2);

    switch (fault_client_type) {
        UVM_ENUM_STRING_CASE(UVM_FAULT_CLIENT_TYPE_GPC);
        UVM_ENUM_STRING_CASE(UVM_FAULT_CLIENT_TYPE_HUB);
        UVM_ENUM_STRING_DEFAULT();
    }
}

const char *uvm_mmu_engine_type_string(uvm_mmu_engine_type_t mmu_engine_type)
{
    BUILD_BUG_ON(UVM_MMU_ENGINE_TYPE_COUNT != 3);

    switch (mmu_engine_type) {
        UVM_ENUM_STRING_CASE(UVM_MMU_ENGINE_TYPE_GRAPHICS);
        UVM_ENUM_STRING_CASE(UVM_MMU_ENGINE_TYPE_HOST);
        UVM_ENUM_STRING_CASE(UVM_MMU_ENGINE_TYPE_CE);
        UVM_ENUM_STRING_DEFAULT();
    }
}

void uvm_hal_print_fault_entry(const uvm_fault_buffer_entry_t *entry)
{
    UVM_DBG_PRINT("fault_address:                    0x%llx\n", entry->fault_address);
    UVM_DBG_PRINT("    fault_instance_ptr:           {0x%llx:%s}\n",
                  entry->instance_ptr.address,
                  uvm_aperture_string(entry->instance_ptr.aperture));
    UVM_DBG_PRINT("    fault_type:                   %s\n", uvm_fault_type_string(entry->fault_type));
    UVM_DBG_PRINT("    fault_access_type:            %s\n", uvm_fault_access_type_string(entry->fault_access_type));
    UVM_DBG_PRINT("    is_replayable:                %s\n", entry->is_replayable? "true": "false");
    UVM_DBG_PRINT("    is_virtual:                   %s\n", entry->is_virtual? "true": "false");
    UVM_DBG_PRINT("    in_protected_mode:            %s\n", entry->in_protected_mode? "true": "false");
    UVM_DBG_PRINT("    fault_source.client_type:     %s\n",
                  uvm_fault_client_type_string(entry->fault_source.client_type));
    UVM_DBG_PRINT("    fault_source.client_id:       %d\n", entry->fault_source.client_id);
    UVM_DBG_PRINT("    fault_source.gpc_id:          %d\n", entry->fault_source.gpc_id);
    UVM_DBG_PRINT("    fault_source.mmu_engine_id:   %d\n", entry->fault_source.mmu_engine_id);
    UVM_DBG_PRINT("    fault_source.mmu_engine_type: %s\n",
                  uvm_mmu_engine_type_string(entry->fault_source.mmu_engine_type));
    UVM_DBG_PRINT("    timestamp:                    %llu\n", entry->timestamp);
}

void uvm_hal_print_access_counter_buffer_entry(const uvm_access_counter_buffer_entry_t *entry)
{
    UVM_DBG_PRINT("virtual address: 0x%llx\n", entry->address);
    UVM_DBG_PRINT("    instance_ptr    {0x%llx:%s}\n",
                  entry->instance_ptr.address,
                  uvm_aperture_string(entry->instance_ptr.aperture));
    UVM_DBG_PRINT("    mmu_engine_type %s\n", uvm_mmu_engine_type_string(entry->mmu_engine_type));
    UVM_DBG_PRINT("    mmu_engine_id   %u\n", entry->mmu_engine_id);
    UVM_DBG_PRINT("    ve_id           %u\n", entry->ve_id);
    UVM_DBG_PRINT("    counter_value   %u\n", entry->counter_value);
    UVM_DBG_PRINT("    subgranularity  0x%08x\n", entry->sub_granularity);
    UVM_DBG_PRINT("    bank            %u\n", entry->bank);
    UVM_DBG_PRINT("    tag             %x\n", entry->tag);
}

bool uvm_hal_method_is_valid_stub(uvm_push_t *push, NvU32 method_address, NvU32 method_data)
{
    return true;
}

void uvm_hal_ce_memcopy_patch_src_stub(uvm_push_t *push, uvm_gpu_address_t *src)
{
}
