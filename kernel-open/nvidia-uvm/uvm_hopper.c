/*******************************************************************************
    Copyright (c) 2020-2023 NVIDIA Corporation

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

#include "uvm_global.h"
#include "uvm_hal.h"
#include "uvm_gpu.h"
#include "uvm_mem.h"
#include "uvm_hopper_fault_buffer.h"

static uvm_gpu_peer_copy_mode_t hopper_peer_copy_mode(uvm_parent_gpu_t *parent_gpu)
{
    // In Confidential Computing the Copy Engine supports encrypted copies
    // between peers. But in Hopper these transfers require significant
    // software support (ex: unprotected vidmem), so in practice they are not
    // allowed.
    if (g_uvm_global.conf_computing_enabled)
        return UVM_GPU_PEER_COPY_MODE_UNSUPPORTED;

    // TODO: Bug 4174553: In some Grace Hopper setups, physical peer copies
    // result on errors. Force peer copies to use virtual addressing until the
    // issue is clarified.
    if (uvm_parent_gpu_is_coherent(parent_gpu))
        return UVM_GPU_PEER_COPY_MODE_VIRTUAL;

    return g_uvm_global.peer_copy_mode;
}

void uvm_hal_hopper_arch_init_properties(uvm_parent_gpu_t *parent_gpu)
{
    parent_gpu->tlb_batch.va_invalidate_supported = true;

    parent_gpu->tlb_batch.va_range_invalidate_supported = true;

    // TODO: Bug 1767241: Run benchmarks to figure out a good number
    parent_gpu->tlb_batch.max_ranges = 8;

    parent_gpu->utlb_per_gpc_count = uvm_hopper_get_utlbs_per_gpc(parent_gpu);

    parent_gpu->fault_buffer_info.replayable.utlb_count = parent_gpu->rm_info.maxGpcCount *
                                                          parent_gpu->utlb_per_gpc_count;
    {
        uvm_fault_buffer_entry_t *dummy;
        UVM_ASSERT(parent_gpu->fault_buffer_info.replayable.utlb_count <= (1 <<
                                                                           (sizeof(dummy->fault_source.utlb_id) * 8)));
    }

    // A single top level PDE on Hopper covers 64 PB and that's the minimum
    // size that can be used.
    parent_gpu->rm_va_base = 0;
    parent_gpu->rm_va_size = 64 * UVM_SIZE_1PB;

    parent_gpu->uvm_mem_va_base = parent_gpu->rm_va_size + 384 * UVM_SIZE_1TB;
    parent_gpu->uvm_mem_va_size = UVM_MEM_VA_SIZE;

    // See uvm_mmu.h for mapping placement
    parent_gpu->flat_vidmem_va_base = (64 * UVM_SIZE_1PB) + (32 * UVM_SIZE_1TB);

    // Physical CE writes to vidmem are non-coherent with respect to the CPU on
    // Grace Hopper.
    parent_gpu->ce_phys_vidmem_write_supported = !uvm_parent_gpu_is_coherent(parent_gpu);

    parent_gpu->peer_copy_mode = hopper_peer_copy_mode(parent_gpu);

    // All GR context buffers may be mapped to 57b wide VAs. All "compute" units
    // accessing GR context buffers support the 57-bit VA range.
    parent_gpu->max_channel_va = 1ull << 57;

    parent_gpu->max_host_va = 1ull << 57;

    // Hopper can map sysmem with any page size
    parent_gpu->can_map_sysmem_with_large_pages = true;

    // Prefetch instructions will generate faults
    parent_gpu->prefetch_fault_supported = true;

    // Hopper can place GPFIFO in vidmem
    parent_gpu->gpfifo_in_vidmem_supported = true;

    parent_gpu->replayable_faults_supported = true;

    parent_gpu->non_replayable_faults_supported = true;

    parent_gpu->access_counters_supported = true;

    parent_gpu->access_counters_can_use_physical_addresses = false;

    parent_gpu->fault_cancel_va_supported = true;

    parent_gpu->scoped_atomics_supported = true;

    parent_gpu->has_clear_faulted_channel_sw_method = true;

    parent_gpu->has_clear_faulted_channel_method = false;

    parent_gpu->smc.supported = true;

    parent_gpu->sparse_mappings_supported = true;

    parent_gpu->map_remap_larger_page_promotion = false;

    parent_gpu->plc_supported = true;

    parent_gpu->no_ats_range_required = true;
}
