/*******************************************************************************
    Copyright (c) 2021-2023 NVIDIA Corporation

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
#include "uvm_push.h"
#include "uvm_common.h"

void uvm_hal_maxwell_sec2_init_noop(uvm_push_t *push)
{
}

void uvm_hal_maxwell_sec2_semaphore_release_unsupported(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    UVM_ASSERT_MSG(false, "SEC2 semaphore_release is not supported on GPU: %s.\n", uvm_gpu_name(gpu));
}

void uvm_hal_maxwell_sec2_semaphore_timestamp_unsupported(uvm_push_t *push, NvU64 gpu_va)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    UVM_ASSERT_MSG(false, "SEC2 semaphore_timestamp is not supported on GPU: %s.\n", uvm_gpu_name(gpu));
}

void uvm_hal_maxwell_sec2_decrypt_unsupported(uvm_push_t *push,
                                              NvU64 dst_va,
                                              NvU64 src_va,
                                              NvU32 size,
                                              NvU64 auth_tag_va)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    UVM_ASSERT_MSG(false, "SEC2 decrypt is not supported on GPU: %s.\n", uvm_gpu_name(gpu));
}
