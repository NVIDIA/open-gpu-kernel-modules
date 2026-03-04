/*******************************************************************************
    Copyright (c) 2021-2024 NVIDIA Corporation

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

void uvm_hal_maxwell_enable_replayable_faults_unsupported(uvm_parent_gpu_t *parent_gpu)
{
    UVM_ASSERT_MSG(false,
                   "enable_replayable_faults is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
}

void uvm_hal_maxwell_disable_replayable_faults_unsupported(uvm_parent_gpu_t *parent_gpu)
{
    UVM_ASSERT_MSG(false,
                   "disable_replayable_faults is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
}

void uvm_hal_maxwell_clear_replayable_faults_unsupported(uvm_parent_gpu_t *parent_gpu, NvU32 get)
{
    UVM_ASSERT_MSG(false,
                   "clear_replayable_faults is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
}

NvU32 uvm_hal_maxwell_fault_buffer_read_put_unsupported(uvm_parent_gpu_t *parent_gpu)
{
    UVM_ASSERT_MSG(false,
                   "fault_buffer_read_put is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
    return 0;
}

NvU32 uvm_hal_maxwell_fault_buffer_read_get_unsupported(uvm_parent_gpu_t *parent_gpu)
{
    UVM_ASSERT_MSG(false,
                   "fault_buffer_read_get is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
    return 0;
}

void uvm_hal_maxwell_fault_buffer_write_get_unsupported(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    UVM_ASSERT_MSG(false,
                   "fault_buffer_write_get is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
}

NvU8 uvm_hal_maxwell_fault_buffer_get_ve_id_unsupported(NvU16 mmu_engine_id, uvm_mmu_engine_type_t mmu_engine_type)
{
    UVM_ASSERT_MSG(false, "fault_buffer_get_ve_id is not supported on Maxwell GPUs.\n");
    return 0;
}

uvm_mmu_engine_type_t uvm_hal_maxwell_fault_buffer_get_mmu_engine_type_unsupported(NvU16 mmu_engine_id,
                                                                                   uvm_fault_client_type_t client_type,
                                                                                   NvU16 client_id)
{
    UVM_ASSERT_MSG(false, "fault_buffer_get_mmu_engine_type is not supported on Maxwell GPUs.\n");
    return UVM_MMU_ENGINE_TYPE_GRAPHICS;
}

uvm_fault_type_t uvm_hal_maxwell_fault_buffer_get_fault_type_unsupported(const NvU32 *fault_entry)
{
    UVM_ASSERT_MSG(false, "fault_buffer_get_fault_type is not supported.\n");
    return UVM_FAULT_TYPE_COUNT;
}

NV_STATUS uvm_hal_maxwell_fault_buffer_parse_replayable_entry_unsupported(uvm_parent_gpu_t *parent_gpu,
                                                                          NvU32 index,
                                                                          uvm_fault_buffer_entry_t *buffer_entry)
{
    UVM_ASSERT_MSG(false,
                   "fault_buffer_parse_entry is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
    return NV_ERR_NOT_SUPPORTED;
}

bool uvm_hal_maxwell_fault_buffer_entry_is_valid_unsupported(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    UVM_ASSERT_MSG(false,
                   "fault_buffer_entry_is_valid is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
    return false;
}

void uvm_hal_maxwell_fault_buffer_entry_clear_valid_unsupported(uvm_parent_gpu_t *parent_gpu, NvU32 index)
{
    UVM_ASSERT_MSG(false,
                   "fault_buffer_entry_clear_valid is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
}

NvU32 uvm_hal_maxwell_fault_buffer_entry_size_unsupported(uvm_parent_gpu_t *parent_gpu)
{
    UVM_ASSERT_MSG(false,
                   "fault_buffer_entry_size is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
    return 0;
}

void uvm_hal_maxwell_fault_buffer_parse_non_replayable_entry_unsupported(uvm_parent_gpu_t *parent_gpu,
                                                                         void *fault_packet,
                                                                         uvm_fault_buffer_entry_t *buffer_entry)
{
    UVM_ASSERT_MSG(false,
                   "fault_buffer_parse_non_replayable_entry is not supported on GPU: %s.\n",
                   uvm_parent_gpu_name(parent_gpu));
}

