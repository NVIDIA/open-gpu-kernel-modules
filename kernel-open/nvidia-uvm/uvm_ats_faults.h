/*******************************************************************************
    Copyright (c) 2021 NVIDIA Corporation

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

#include "uvm_linux.h"
#include "uvm_forward_decl.h"
#include "uvm_lock.h"
#include "uvm_global.h"
#include "uvm_va_space.h"
#include "uvm_gpu.h"

// Service ATS faults in the range (base, base + UVM_VA_BLOCK_SIZE) with service
// type for individual pages in the range requested by page masks set in
// ats_context->fault.read_fault_mask/write_fault_mask/prefetch_only_mask.
// base must be aligned to UVM_VA_BLOCK_SIZE. The caller is responsible for
// ensuring that faulting addresses fall completely within the VMA. The caller
// is also responsible for ensuring that the faulting addresses don't overlap
// a GMMU region. (See uvm_ats_check_in_gmmu_region). The caller is also
// responsible for handling any errors returned by this function (fault
// cancellations etc.).
//
// Returns the fault service status in ats_context->fault.faults_serviced_mask.
// In addition, ats_context->fault.reads_serviced_mask returns whether read
// servicing worked on write faults iff the read service was also requested in
// the corresponding bit in read_fault_mask. These returned masks are only valid
// if the return status is NV_OK. Status other than NV_OK indicate system global
// fault servicing failures.
//
// LOCKING: The caller must retain and hold the mmap_lock and hold the va_space
// lock.
NV_STATUS uvm_ats_service_faults(uvm_gpu_va_space_t *gpu_va_space,
                                 struct vm_area_struct *vma,
                                 NvU64 base,
                                 uvm_ats_fault_context_t *ats_context);

// Service access counter notifications on ATS regions in the range (base, base
// + UVM_VA_BLOCK_SIZE) for individual pages in the range requested by page_mask
// set in ats_context->access_counters.accessed_mask. base must be aligned to
// UVM_VA_BLOCK_SIZE. The caller is responsible for ensuring that the addresses
// in the accessed_mask is completely covered by the VMA. The caller is also
// responsible for handling any errors returned by this function.
//
// Returns NV_OK if servicing was successful. Any other error indicates an error
// while servicing the range.
//
// LOCKING: The caller must retain and hold the mmap_lock and hold the va_space
// lock.
NV_STATUS uvm_ats_service_access_counters(uvm_gpu_va_space_t *gpu_va_space,
                                          struct vm_area_struct *vma,
                                          NvU64 base,
                                          uvm_ats_fault_context_t *ats_context);

// Return whether there are any VA ranges (and thus GMMU mappings) within the
// UVM_GMMU_ATS_GRANULARITY-aligned region containing address.
bool uvm_ats_check_in_gmmu_region(uvm_va_space_t *va_space, NvU64 address, uvm_va_range_t *next);

// This function performs pending TLB invalidations for ATS and clears the
// ats_invalidate->tlb_batch_pending flag
NV_STATUS uvm_ats_invalidate_tlbs(uvm_gpu_va_space_t *gpu_va_space,
                                  uvm_ats_fault_invalidate_t *ats_invalidate,
                                  uvm_tracker_t *out_tracker);

static bool uvm_ats_can_service_faults(uvm_gpu_va_space_t *gpu_va_space, struct mm_struct *mm)
{
    if (mm)
        uvm_assert_mmap_lock_locked(mm);
    if (gpu_va_space->ats.enabled)
        UVM_ASSERT(g_uvm_global.ats.enabled);

    return gpu_va_space->ats.enabled && mm;
}
