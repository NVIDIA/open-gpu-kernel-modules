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

NV_STATUS uvm_ats_service_fault_entry(uvm_gpu_va_space_t *gpu_va_space,
                                      uvm_fault_buffer_entry_t *current_entry,
                                      uvm_ats_fault_invalidate_t *ats_invalidate);

// This function performs pending TLB invalidations for ATS and clears the
// ats_invalidate->write_faults_in_batch flag
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
