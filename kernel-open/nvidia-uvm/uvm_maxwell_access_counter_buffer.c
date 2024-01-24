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

#include "uvm_gpu.h"
#include "uvm_hal.h"

void uvm_hal_maxwell_enable_access_counter_notifications_unsupported(uvm_parent_gpu_t *parent_gpu)
{
    UVM_ASSERT_MSG(false,
                   "enable_access_counter_notifications is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
}

void uvm_hal_maxwell_disable_access_counter_notifications_unsupported(uvm_parent_gpu_t *parent_gpu)
{
    UVM_ASSERT_MSG(false,
                   "disable_access_counter_notifications is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
}

void uvm_hal_maxwell_clear_access_counter_notifications_unsupported(uvm_parent_gpu_t *parent_gpu, NvU32 get)
{
    UVM_ASSERT_MSG(false,
                   "clear_access_counter_notifications is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
}

NvU32 uvm_hal_maxwell_access_counter_buffer_entry_size_unsupported(uvm_parent_gpu_t *parent_gpu)
{
    UVM_ASSERT_MSG(false,
                   "access_counter_buffer_entry_size is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
    return 0;
}

bool uvm_hal_maxwell_access_counter_buffer_entry_is_valid_unsupported(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    UVM_ASSERT_MSG(false,
                   "access_counter_buffer_entry_is_valid is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
    return false;
}

void uvm_hal_maxwell_access_counter_buffer_entry_clear_valid_unsupported(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    UVM_ASSERT_MSG(false,
                   "access_counter_buffer_entry_clear_valid is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
}

void uvm_hal_maxwell_access_counter_buffer_parse_entry_unsupported(uvm_parent_gpu_t *parent_gpu,
                                                                   NvU32 index,
                                                                   uvm_access_counter_buffer_entry_t *buffer_entry)
{
    UVM_ASSERT_MSG(false,
                   "access_counter_buffer_parse_entry is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
}
