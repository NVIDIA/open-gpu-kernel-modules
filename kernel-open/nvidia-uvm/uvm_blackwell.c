/*******************************************************************************
    Copyright (c) 2022-2025 NVIDIA Corporation

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
#include "uvm_blackwell_fault_buffer.h"

void uvm_hal_blackwell_arch_init_properties(uvm_parent_gpu_t *parent_gpu)
{
    parent_gpu->tlb_batch.va_invalidate_supported = true;

    parent_gpu->tlb_batch.va_range_invalidate_supported = true;

    // TODO: Bug 1767241: Run benchmarks to figure out a good number
    parent_gpu->tlb_batch.max_ranges = 8;

    parent_gpu->utlb_per_gpc_count = uvm_blackwell_get_utlbs_per_gpc(parent_gpu);

    parent_gpu->fault_buffer.replayable.utlb_count = parent_gpu->rm_info.maxGpcCount * parent_gpu->utlb_per_gpc_count;
    {
        uvm_fault_buffer_entry_t *dummy;
        UVM_ASSERT(parent_gpu->fault_buffer.replayable.utlb_count <= (1 << (sizeof(dummy->fault_source.utlb_id) * 8)));
    }

    // A single top level PDE on Blackwell covers 64 PB and that's the minimum
    // size that can be used.
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

    // Blackwell can map sysmem with any page size
    parent_gpu->can_map_sysmem_with_large_pages = true;

    // Prefetch instructions will generate faults
    parent_gpu->prefetch_fault_supported = true;

    // Blackwell can place GPFIFO in vidmem
    parent_gpu->gpfifo_in_vidmem_supported = true;

    parent_gpu->replayable_faults_supported = true;

    parent_gpu->non_replayable_faults_supported = true;

    parent_gpu->fault_cancel_va_supported = true;

    parent_gpu->scoped_atomics_supported = true;

    parent_gpu->has_clear_faulted_channel_sw_method = true;

    parent_gpu->has_clear_faulted_channel_method = false;

    parent_gpu->smc.supported = true;

    parent_gpu->sparse_mappings_supported = true;

    parent_gpu->map_remap_larger_page_promotion = false;

    parent_gpu->plc_supported = true;

    parent_gpu->ats.no_ats_range_required = true;

    // Blackwell has a physical translation prefetcher, meaning SW must assume
    // that any physical ATS translation can be fetched at any time. The
    // specific behavior and impact differs with non-PASID ATS support, but
    // generally this can result in the GPU accessing a stale invalid physical
    // ATS translation after transitioning an IOMMU mapping from invalid to
    // valid, or in other words at dma_map_page() time.
    //
    // These cases could result in faults. That can happen with virtual accesses
    // too, but that's fine since we'll just handle and replay them. But
    // physical access IOMMU faults are considered globally fatal, so we must
    // avoid them.
    if (parent_gpu->ats.non_pasid_ats_enabled) {
        // On Blackwell GPUs that have non-PASID ATS, physical translations that
        // come back as 4K will not be cached. Therefore we don't have to worry
        // about invalidating them, and we don't have to worry the cross-
        // contamination issue within a 64K region described in the below case.
        // However, we do need to ensure that all pending prefetch requests to
        // the old invalid translation are complete before we access the new
        // valid translation, so we need to flush pending translations.
        parent_gpu->ats.dma_map_invalidation = UVM_DMA_MAP_INVALIDATION_FLUSH;
    }
    else if (g_uvm_global.ats.enabled && PAGE_SIZE == UVM_PAGE_SIZE_64K) {
        // If the page size is 64K here in the guest it's possible for the host
        // page size to be 4K, which we can't know here, and that could cause
        // new mappings to hit old stale invalid entries that aren't
        // automatically refetched by HW (see the comments on 4K support in
        // uvm_ats_service_faults_region()).
        //
        // Even with 64K only it's possible for the prefetcher to race with a
        // legitimate access in the same 64KB region and cause the legitimate
        // access to see an invalid translation, so we need an invalidate to
        // flush those out too.
        //
        // If the guest page size is 4K we don't enable physical ATS so we don't
        // have an issue.
        parent_gpu->ats.dma_map_invalidation = UVM_DMA_MAP_INVALIDATION_FULL;
    }

    parent_gpu->conf_computing.per_channel_key_rotation = true;

    // TODO: Bug 5023085: this should be queried from RM instead of determined
    // by UVM.
    if (parent_gpu->rm_info.gpuArch == NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB100 &&
        parent_gpu->rm_info.gpuImplementation ==
            NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GB10B) {
        parent_gpu->is_integrated_gpu = true;
        // GB10B has sticky L2 coherent cache lines.
        // For details, refer to the comments in uvm_gpu.h
        // where this field is declared.
        parent_gpu->sticky_l2_coherent_cache_lines = true;
    }
    if (parent_gpu->rm_info.gpuArch == NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB200 &&
        parent_gpu->rm_info.gpuImplementation ==
            NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GB20B)
        parent_gpu->is_integrated_gpu = true;
}
