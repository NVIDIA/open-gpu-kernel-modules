/*******************************************************************************
    Copyright (c) 2018-2024 NVIDIA Corporation

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

#include "uvm_va_space.h"
#include "uvm_ats.h"
#include "uvm_global.h"
#include "uvm_gpu.h"

static int uvm_ats_mode = 1;
module_param(uvm_ats_mode, int, S_IRUGO);
MODULE_PARM_DESC(uvm_ats_mode, "Set to 0 to disable ATS (Address Translation Services). "
                               "Any other value is ignored. Has no effect unless the "
                               "platform supports ATS.");

void uvm_ats_init(const UvmPlatformInfo *platform_info)
{
    g_uvm_global.ats.supported = platform_info->atsSupported;

    g_uvm_global.ats.enabled = uvm_ats_mode                     &&
                               g_uvm_global.ats.supported       &&
                               UVM_ATS_SUPPORTED()              &&
                               uvm_va_space_mm_enabled_system();
}

void uvm_ats_init_va_space(uvm_va_space_t *va_space)
{
    uvm_init_rwsem(&va_space->ats.lock, UVM_LOCK_ORDER_LEAF);

    if (UVM_ATS_IBM_SUPPORTED())
        uvm_ats_ibm_init_va_space(va_space);
}

NV_STATUS uvm_ats_add_gpu(uvm_parent_gpu_t *parent_gpu)
{
    if (UVM_ATS_IBM_SUPPORTED()) {
        // uvm_ibm_add_gpu() needs to be called even if ATS is disabled since it
        // sets parent_gpu->npu. Not setting parent_gpu->npu will result in
        // incorrect NVLink addresses. See dma_addr_to_gpu_addr().

        return uvm_ats_ibm_add_gpu(parent_gpu);
    }
    else if (UVM_ATS_SVA_SUPPORTED()) {
        if (g_uvm_global.ats.enabled)
            return uvm_ats_sva_add_gpu(parent_gpu);
    }

    return NV_OK;
}

void uvm_ats_remove_gpu(uvm_parent_gpu_t *parent_gpu)
{
    if (UVM_ATS_IBM_SUPPORTED()) {
        // uvm_ibm_remove_gpu() needs to be called even if ATS is disabled since
        // uvm_ibm_add_gpu() is called even in that case and
        // uvm_ibm_remove_gpu() needs to undo the work done by
        // uvm_ats_add_gpu() (gpu retained_count etc.).

        uvm_ats_ibm_remove_gpu(parent_gpu);
    }
    else if (UVM_ATS_SVA_SUPPORTED()) {
        if (g_uvm_global.ats.enabled)
            uvm_ats_sva_remove_gpu(parent_gpu);
    }
}

NV_STATUS uvm_ats_bind_gpu(uvm_gpu_va_space_t *gpu_va_space)
{
    NV_STATUS status = NV_OK;

    UVM_ASSERT(gpu_va_space);

    if (!gpu_va_space->ats.enabled)
        return status;

    uvm_assert_lockable_order(UVM_LOCK_ORDER_MMAP_LOCK);
    uvm_assert_lockable_order(UVM_LOCK_ORDER_VA_SPACE);

    if (UVM_ATS_IBM_SUPPORTED())
        status = uvm_ats_ibm_bind_gpu(gpu_va_space);
    else if (UVM_ATS_SVA_SUPPORTED())
        status = uvm_ats_sva_bind_gpu(gpu_va_space);

    return status;
}

void uvm_ats_unbind_gpu(uvm_gpu_va_space_t *gpu_va_space)
{
    UVM_ASSERT(gpu_va_space);

    if (!gpu_va_space->ats.enabled)
        return;

    if (UVM_ATS_IBM_SUPPORTED())
        uvm_ats_ibm_unbind_gpu(gpu_va_space);
    else if (UVM_ATS_SVA_SUPPORTED())
        uvm_ats_sva_unbind_gpu(gpu_va_space);
}

NV_STATUS uvm_ats_register_gpu_va_space(uvm_gpu_va_space_t *gpu_va_space)
{
    NV_STATUS status = NV_OK;
    uvm_va_space_t *va_space;
    uvm_gpu_id_t gpu_id;

    UVM_ASSERT(gpu_va_space);

    if (!gpu_va_space->ats.enabled)
        return status;

    va_space = gpu_va_space->va_space;
    UVM_ASSERT(va_space);

    uvm_assert_rwsem_locked_write(&va_space->lock);

    gpu_id = gpu_va_space->gpu->id;

    // Prevent multiple registrations of the same gpu_va_space for ATS access.
    if (uvm_processor_mask_test(&va_space->ats.registered_gpu_va_spaces, gpu_id))
        return NV_ERR_INVALID_DEVICE;

    if (UVM_ATS_IBM_SUPPORTED())
        status = uvm_ats_ibm_register_gpu_va_space(gpu_va_space);
    else if (UVM_ATS_SVA_SUPPORTED())
        status = uvm_ats_sva_register_gpu_va_space(gpu_va_space);

    if (status == NV_OK)
        uvm_processor_mask_set(&va_space->ats.registered_gpu_va_spaces, gpu_id);

    return status;
}

void uvm_ats_unregister_gpu_va_space(uvm_gpu_va_space_t *gpu_va_space)
{
    uvm_gpu_id_t gpu_id;
    uvm_va_space_t *va_space;

    UVM_ASSERT(gpu_va_space);

    if (!gpu_va_space->ats.enabled)
        return;

    va_space = gpu_va_space->va_space;
    gpu_id = gpu_va_space->gpu->id;

    if (UVM_ATS_IBM_SUPPORTED())
        uvm_ats_ibm_unregister_gpu_va_space(gpu_va_space);
    else if (UVM_ATS_SVA_SUPPORTED())
        uvm_ats_sva_unregister_gpu_va_space(gpu_va_space);

    uvm_va_space_down_write(va_space);
    uvm_processor_mask_clear(&va_space->ats.registered_gpu_va_spaces, gpu_id);
    uvm_va_space_up_write(va_space);
}

void uvm_ats_invalidate(uvm_va_space_t *va_space, NvU64 start, NvU64 end)
{
    // We can only reach here from the mmu_notifier callbacks and these callbacks
    // wouldn't have been registered if ATS wasn't enabled.
    UVM_ASSERT(g_uvm_global.ats.enabled);

    if (UVM_ATS_IBM_SUPPORTED())
        uvm_ats_ibm_invalidate(va_space, start, end);
}
