/*******************************************************************************
    Copyright (c) 2024-2025 NVIDIA Corporation

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
#include "uvm_rubin_fault_buffer.h"

void uvm_hal_rubin_arch_init_properties(uvm_parent_gpu_t *parent_gpu)
{
    // TODO: Bug 1767241: Run benchmarks to figure out a good number
    parent_gpu->tlb_batch.max_ranges = 8;

    parent_gpu->utlb_per_gpc_count = uvm_rubin_get_utlbs_per_gpc(parent_gpu);

    parent_gpu->fault_buffer.replayable.utlb_count = parent_gpu->rm_info.maxGpcCount * parent_gpu->utlb_per_gpc_count;
    {
        uvm_fault_buffer_entry_t *dummy;
        UVM_ASSERT(parent_gpu->fault_buffer.replayable.utlb_count <= (1 << (sizeof(dummy->fault_source.utlb_id) * 8)));
    }

    // A single top level PDE on Rubin covers 64 PB and that's the minimum size
    // that can be used.
    parent_gpu->rm_va_base = 0;
    parent_gpu->rm_va_size = 64 * UVM_SIZE_1PB;

    parent_gpu->peer_va_base = parent_gpu->rm_va_base + parent_gpu->rm_va_size;
    parent_gpu->peer_va_size = NV_MAX_DEVICES * UVM_PEER_IDENTITY_VA_SIZE;

    parent_gpu->uvm_mem_va_base = parent_gpu->rm_va_size + 384 * UVM_SIZE_1TB;
    parent_gpu->uvm_mem_va_size = UVM_MEM_VA_SIZE;

    // See uvm_mmu.h for mapping placement
    parent_gpu->flat_vidmem_va_base = (64 * UVM_SIZE_1PB) + (32 * UVM_SIZE_1TB);

    parent_gpu->ce_phys_vidmem_write_supported = true;

    parent_gpu->peer_copy_mode = g_uvm_global.peer_copy_mode;

    // All GR context buffers may be mapped to 57b wide VAs. All "compute" units
    // accessing GR context buffers support the 57-bit VA range.
    parent_gpu->max_channel_va = 1ull << 57;

    parent_gpu->max_host_va = 1ull << 57;

    parent_gpu->access_counters_serialize_clear_ops_by_type = false;

    parent_gpu->access_bits_supported = true;

    parent_gpu->has_clear_faulted_channel_sw_method = true;

    parent_gpu->has_clear_faulted_channel_method = false;

    parent_gpu->smc.supported = true;

    parent_gpu->map_remap_larger_page_promotion = false;

    parent_gpu->ats.no_ats_range_required = false;

        parent_gpu->ats.gmmu_pt_depth0_init_required = parent_gpu->ats.non_pasid_ats_enabled;

    // Rubin disables its physical translation prefetcher for non-PASID ATS
    // translations, and it will not cache physical translations that come back
    // as 4K. Therefore we don't have to worry about invalidation.
    //
    // Systems with 4K pages have a snag. Suppose pages A and B are in the same
    // 64K address region. Page A is already valid and we are transitioning B
    // from valid to invalid. If B is translated while an old in-flight
    // translation to page A is already pending, it is possible for the in-
    // flight translation to also return the stale invalid translation for B.
    //
    // That can happen with virtual accesses too, but that's fine since we'll
    // just handle and replay them. But physical access IOMMU faults are
    // considered globally fatal, so we must avoid them. To solve that we must
    // flush out pending access before accessing the newly-valid translation.
    //
    // 64K systems do not have this issue since no mixing of invalid and valid
    // translations in the same 64K physical region is possible. They must all
    // be valid by the time the first GPU access is made, otherwise we might
    // see a fatal IOMMU fault.
    if (parent_gpu->ats.non_pasid_ats_enabled && PAGE_SIZE == UVM_PAGE_SIZE_4K)
        parent_gpu->ats.dma_map_invalidation = UVM_DMA_MAP_INVALIDATION_FLUSH;

    parent_gpu->conf_computing.per_channel_key_rotation = true;
}
