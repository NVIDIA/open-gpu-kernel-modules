/*******************************************************************************
    Copyright (c) 2016-2025 NVIDIA Corporation

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
#include "uvm_gpu.h"
#include "uvm_mem.h"
#include "uvm_pascal_fault_buffer.h"

static unsigned uvm_force_prefetch_fault_support = 0;
module_param(uvm_force_prefetch_fault_support, uint, S_IRUGO);

void uvm_hal_pascal_arch_init_properties(uvm_parent_gpu_t *parent_gpu)
{
    parent_gpu->tlb_batch.va_invalidate_supported = true;

    parent_gpu->tlb_batch.va_range_invalidate_supported = false;

    // TODO: Bug 1767241: Run benchmarks to figure out a good number
    parent_gpu->tlb_batch.max_pages = 32;

    parent_gpu->utlb_per_gpc_count = uvm_pascal_get_utlbs_per_gpc(parent_gpu);

    parent_gpu->fault_buffer.replayable.utlb_count = parent_gpu->rm_info.gpcCount * parent_gpu->utlb_per_gpc_count;
    {
        uvm_fault_buffer_entry_t *dummy;
        UVM_ASSERT(parent_gpu->fault_buffer.replayable.utlb_count <= (1 << (sizeof(dummy->fault_source.utlb_id) * 8)));
    }

    // A single top level PDE on Pascal covers 128 TB and that's the minimum
    // size that can be used.
    parent_gpu->rm_va_base = 0;
    parent_gpu->rm_va_size = 128 * UVM_SIZE_1TB;

    parent_gpu->peer_va_base = parent_gpu->rm_va_base + parent_gpu->rm_va_size;
    parent_gpu->peer_va_size = NV_MAX_DEVICES * UVM_PEER_IDENTITY_VA_SIZE;

    parent_gpu->uvm_mem_va_base = 384 * UVM_SIZE_1TB;
    parent_gpu->uvm_mem_va_size = UVM_MEM_VA_SIZE;

    parent_gpu->ce_phys_vidmem_write_supported = true;

    parent_gpu->peer_copy_mode = UVM_GPU_PEER_COPY_MODE_VIRTUAL;

    // Not all units on Pascal support 49-bit addressing, including those which
    // access channel buffers.
    parent_gpu->max_channel_va = 1ULL << 40;

    parent_gpu->max_host_va = 1ULL << 40;

    // Pascal can map sysmem with any page size
    parent_gpu->can_map_sysmem_with_large_pages = true;

    // Prefetch faults are disabled by default in Pascal
    parent_gpu->prefetch_fault_supported = uvm_force_prefetch_fault_support != 0;

    // Pascal and Volta require post-invalidate membars to flush out HSHUB. See
    // bug 1975028. GP10x chips do not have HSHUB, so they don't need any.
    UVM_ASSERT(parent_gpu->rm_info.gpuArch == NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GP100);
    if (parent_gpu->rm_info.gpuImplementation == NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GP100 ||
        parent_gpu->rm_info.gpuImplementation == NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GP000) {
        parent_gpu->num_hshub_tlb_invalidate_membars = 2;
    }
    else
        parent_gpu->num_hshub_tlb_invalidate_membars = 0;

    // Pascal cannot place GPFIFO in vidmem
    parent_gpu->gpfifo_in_vidmem_supported = false;

    parent_gpu->replayable_faults_supported = true;

    parent_gpu->non_replayable_faults_supported = false;

    parent_gpu->fault_cancel_va_supported = false;

    parent_gpu->scoped_atomics_supported = false;

    parent_gpu->sparse_mappings_supported = true;

    parent_gpu->map_remap_larger_page_promotion = false;

    parent_gpu->smc.supported = false;

    parent_gpu->plc_supported = false;

    parent_gpu->no_ats_range_required = false;

    parent_gpu->conf_computing.per_channel_key_rotation = false;
}
