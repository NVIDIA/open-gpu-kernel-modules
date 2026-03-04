/*******************************************************************************
    Copyright (c) 2016-2023 NVIDIA Corporation

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
#include "uvm_linux.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_gpu_replayable_faults.h"
#include "uvm_test.h"
#include "uvm_va_space.h"

NV_STATUS uvm_test_fault_buffer_flush(UVM_TEST_FAULT_BUFFER_FLUSH_PARAMS *params, struct file *filp)
{
    NV_STATUS status = NV_OK;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_gpu_t *gpu;
    uvm_processor_mask_t *retained_gpus;
    NvU64 i;

    retained_gpus = uvm_processor_mask_cache_alloc();
    if (!retained_gpus)
        return NV_ERR_NO_MEMORY;

    uvm_processor_mask_zero(retained_gpus);

    uvm_va_space_down_read(va_space);

    uvm_processor_mask_and(retained_gpus, &va_space->faultable_processors, &va_space->registered_gpus);

    uvm_global_gpu_retain(retained_gpus);

    uvm_va_space_up_read(va_space);

    if (uvm_processor_mask_empty(retained_gpus)) {
        status = NV_ERR_INVALID_DEVICE;
        goto out;
    }

    for (i = 0; i < params->iterations; i++) {
        if (fatal_signal_pending(current)) {
            status = NV_ERR_SIGNAL_PENDING;
            break;
        }

        for_each_gpu_in_mask(gpu, retained_gpus)
            TEST_CHECK_GOTO(uvm_gpu_fault_buffer_flush(gpu) == NV_OK, out);
    }

out:
    uvm_global_gpu_release(retained_gpus);
    uvm_processor_mask_cache_free(retained_gpus);
    return status;
}
