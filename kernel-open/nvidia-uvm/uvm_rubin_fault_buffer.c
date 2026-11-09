/*******************************************************************************
    Copyright (c) 2025 NVIDIA Corporation

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

#include "uvm_common.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_hal.h"
#include "hwref/rubin/gr100/dev_fb.h"

void uvm_hal_rubin_fault_buffer_write_get(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    NvU32 get;

    UVM_ASSERT(index < parent_gpu->fault_buffer.replayable.max_faults);

    get = HWVALUE(_PFB_PRI_MMU, FAULT_BUFFER_GET, PTR, index);

    // When GSP-RM owns the HW replayable fault buffer, UVM only advances the
    // GET pointer in the UVM-visible shadow buffer. GSP-RM is responsible for
    // clearing GETPTR_CORRUPTED and OVERFLOW in the real GET register.
    if (!g_uvm_global.conf_computing_enabled) {
        get |= HWCONST(_PFB_PRI_MMU, FAULT_BUFFER_GET, GETPTR_CORRUPTED, CLEAR) |
               HWCONST(_PFB_PRI_MMU, FAULT_BUFFER_GET, OVERFLOW, CLEAR);
    }

    UVM_GPU_WRITE_ONCE(*parent_gpu->fault_buffer.rm_info.replayable.pFaultBufferGet, get);
}
