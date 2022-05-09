/*******************************************************************************
    Copyright (c) 2016-2021 NVIDIA Corporation

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
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_hal.h"

static void clear_access_counter_notifications_interrupt(uvm_parent_gpu_t *parent_gpu)
{
    volatile NvU32 *reg;
    NvU32 mask;

    reg = parent_gpu->access_counter_buffer_info.rm_info.pHubIntr;
    mask = parent_gpu->access_counter_buffer_info.rm_info.accessCounterMask;

    UVM_GPU_WRITE_ONCE(*reg, mask);
}

void uvm_hal_turing_disable_access_counter_notifications(uvm_parent_gpu_t *parent_gpu)
{
    volatile NvU32 *reg;
    NvU32 mask;

    reg = parent_gpu->access_counter_buffer_info.rm_info.pHubIntrEnClear;
    mask = parent_gpu->access_counter_buffer_info.rm_info.accessCounterMask;

    UVM_GPU_WRITE_ONCE(*reg, mask);

    wmb();

    // See the comment in uvm_hal_turing_disable_replayable_faults
    clear_access_counter_notifications_interrupt(parent_gpu);
}

void uvm_hal_turing_clear_access_counter_notifications(uvm_parent_gpu_t *parent_gpu, NvU32 get)
{
    clear_access_counter_notifications_interrupt(parent_gpu);

    wmb();

    // Write GET to force the re-evaluation of the interrupt condition after the
    // interrupt bit has been cleared.
    UVM_GPU_WRITE_ONCE(*parent_gpu->access_counter_buffer_info.rm_info.pAccessCntrBufferGet, get);
}

