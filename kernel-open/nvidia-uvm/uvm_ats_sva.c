/*******************************************************************************
    Copyright (c) 2018-2023 NVIDIA Corporation

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

#include "uvm_ats_sva.h"

#if UVM_ATS_SVA_SUPPORTED()

#include "uvm_gpu.h"
#include "uvm_va_space.h"
#include "uvm_va_space_mm.h"

#include <linux/iommu.h>
#include <linux/mm_types.h>

// linux/sched/mm.h is needed for mmget_not_zero and mmput to get the mm
// reference required for the iommu_sva_bind_device() call. This header is not
// present in all the supported versions. Instead of adding a conftest just for
// this header file, use UVM_ATS_SVA_SUPPORTED().
#include <linux/sched/mm.h>

// iommu_sva_bind_device() removed drvdata paramter with commit
// 942fd5435dccb273f90176b046ae6bbba60cfbd8 (10/31/2022).
#if defined(NV_IOMMU_SVA_BIND_DEVICE_HAS_DRVDATA_ARG)
#define UVM_IOMMU_SVA_BIND_DEVICE(dev, mm) iommu_sva_bind_device(dev, mm, NULL)
#else
#define UVM_IOMMU_SVA_BIND_DEVICE(dev, mm) iommu_sva_bind_device(dev, mm)
#endif

NV_STATUS uvm_ats_sva_add_gpu(uvm_parent_gpu_t *parent_gpu)
{
    int ret;

    ret = iommu_dev_enable_feature(&parent_gpu->pci_dev->dev, IOMMU_DEV_FEAT_SVA);

    return errno_to_nv_status(ret);
}

void uvm_ats_sva_remove_gpu(uvm_parent_gpu_t *parent_gpu)
{
    iommu_dev_disable_feature(&parent_gpu->pci_dev->dev, IOMMU_DEV_FEAT_SVA);
}

NV_STATUS uvm_ats_sva_bind_gpu(uvm_gpu_va_space_t *gpu_va_space)
{
    NV_STATUS status = NV_OK;
    struct iommu_sva *iommu_handle;
    struct pci_dev *pci_dev = gpu_va_space->gpu->parent->pci_dev;
    uvm_sva_gpu_va_space_t *sva_gpu_va_space = &gpu_va_space->ats.sva;
    struct mm_struct *mm = gpu_va_space->va_space->va_space_mm.mm;

    UVM_ASSERT(gpu_va_space->ats.enabled);
    UVM_ASSERT(uvm_gpu_va_space_state(gpu_va_space) == UVM_GPU_VA_SPACE_STATE_INIT);
    UVM_ASSERT(mm);

    // The mmput() below may trigger the kernel's mm teardown with exit_mmap()
    // and uvm_va_space_mm_shutdown() and uvm_vm_close_managed() in that path
    // will try to grab the va_space lock and deadlock if va_space was already
    // locked.
    uvm_assert_unlocked_order(UVM_LOCK_ORDER_VA_SPACE);

    // iommu_sva_bind_device() requires the mm reference to be acquired. Since
    // the mm is already retained, mm is still valid but may be inactive since
    // mm_users can still be zero since UVM doesn't use mm_users and maintains a
    // separate refcount (retained_count) for the mm in va_space_mm. See the
    // block comment in va_space_mm.c for more details. So, return an error if
    // mm_users is zero.
    if (!mmget_not_zero(mm))
        return NV_ERR_PAGE_TABLE_NOT_AVAIL;

    // Multiple calls for the {same pci_dev, mm} pair are refcounted by the ARM
    // SMMU Layer.
    iommu_handle = UVM_IOMMU_SVA_BIND_DEVICE(&pci_dev->dev, mm);
    if (IS_ERR(iommu_handle)) {
        status = errno_to_nv_status(PTR_ERR(iommu_handle));
        goto out;
    }

    // If this is not the first bind of the gpu in the mm, then the previously
    // stored iommu_handle in the gpu_va_space must match the handle returned by
    // iommu_sva_bind_device().
    if (sva_gpu_va_space->iommu_handle) {
        UVM_ASSERT(sva_gpu_va_space->iommu_handle == iommu_handle);
        nv_kref_get(&sva_gpu_va_space->kref);
    }
    else {
        sva_gpu_va_space->iommu_handle = iommu_handle;
        nv_kref_init(&sva_gpu_va_space->kref);
    }

out:
    mmput(mm);
    return status;
}

static void uvm_sva_reset_iommu_handle(nv_kref_t *nv_kref)
{
    uvm_sva_gpu_va_space_t *sva_gpu_va_space = container_of(nv_kref, uvm_sva_gpu_va_space_t, kref);
    sva_gpu_va_space->iommu_handle = NULL;
}

void uvm_ats_sva_unbind_gpu(uvm_gpu_va_space_t *gpu_va_space)
{
    uvm_sva_gpu_va_space_t *sva_gpu_va_space = &gpu_va_space->ats.sva;

    // ARM SMMU layer decrements the refcount for the {pci_dev, mm} pair.
    // The actual unbind happens only when the refcount reaches zero.
    if (sva_gpu_va_space->iommu_handle) {
        iommu_sva_unbind_device(sva_gpu_va_space->iommu_handle);
        nv_kref_put(&sva_gpu_va_space->kref, uvm_sva_reset_iommu_handle);
    }
}

NV_STATUS uvm_ats_sva_register_gpu_va_space(uvm_gpu_va_space_t *gpu_va_space)
{
    NvU32 pasid;
    NV_STATUS status = NV_OK;
    uvm_sva_gpu_va_space_t *sva_gpu_va_space = &gpu_va_space->ats.sva;

    // A successful iommu_sva_bind_device() should have preceded this call.
    UVM_ASSERT(sva_gpu_va_space->iommu_handle);

    pasid = iommu_sva_get_pasid(sva_gpu_va_space->iommu_handle);
    if (pasid == IOMMU_PASID_INVALID)
        status = errno_to_nv_status(ENODEV);
    else
        gpu_va_space->ats.pasid = pasid;

    return status;
}

void uvm_ats_sva_unregister_gpu_va_space(uvm_gpu_va_space_t *gpu_va_space)
{
    gpu_va_space->ats.pasid = -1U;
}

#endif // UVM_ATS_SVA_SUPPORTED()
