/*******************************************************************************
    Copyright (c) 2016-2024 NVIDIA Corporation

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

void uvm_hal_maxwell_arch_init_properties(uvm_parent_gpu_t *parent_gpu)
{
    parent_gpu->tlb_batch.va_invalidate_supported = false;

    // 128 GB should be enough for all current RM allocations and leaves enough
    // space for UVM internal mappings.
    // A single top level PDE covers 64 or 128 MB on Maxwell so 128 GB is fine to use.
    parent_gpu->rm_va_base = 0;
    parent_gpu->rm_va_size = 128 * UVM_SIZE_1GB;

    parent_gpu->peer_va_base = 0;
    parent_gpu->peer_va_size = 0;

    parent_gpu->uvm_mem_va_base = 768 * UVM_SIZE_1GB;
    parent_gpu->uvm_mem_va_size = UVM_MEM_VA_SIZE;

    parent_gpu->ce_phys_vidmem_write_supported = true;

    // We don't have a compelling use case in UVM-Lite for direct peer
    // migrations between GPUs, so don't bother setting them up.
    parent_gpu->peer_copy_mode = UVM_GPU_PEER_COPY_MODE_UNSUPPORTED;

    parent_gpu->max_channel_va = 1ULL << 40;

    parent_gpu->max_host_va = 1ULL << 40;

    // Maxwell can only map sysmem with 4K pages
    parent_gpu->can_map_sysmem_with_large_pages = false;

    // Maxwell cannot place GPFIFO in vidmem
    parent_gpu->gpfifo_in_vidmem_supported = false;

    parent_gpu->replayable_faults_supported = false;

    parent_gpu->non_replayable_faults_supported = false;

    parent_gpu->fault_cancel_va_supported = false;

    parent_gpu->scoped_atomics_supported = false;

    parent_gpu->sparse_mappings_supported = false;

    parent_gpu->map_remap_larger_page_promotion = false;

    parent_gpu->smc.supported = false;

    parent_gpu->plc_supported = false;

    parent_gpu->no_ats_range_required = false;

    parent_gpu->conf_computing.per_channel_key_rotation = false;
}
