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

#ifndef __UVM_ATS_SVA_H__
#define __UVM_ATS_SVA_H__

#include "uvm_gpu.h"
#include "uvm_forward_decl.h"

#include <linux/iommu.h>

// For ATS support on aarch64, arm_smmu_sva_bind() is needed for
// iommu_sva_bind_device() calls. Unfortunately, arm_smmu_sva_bind() is not
// conftest-able. We instead look for the presence of ioasid_get() or
// mm_pasid_drop(). ioasid_get() was added in the same patch series as
// arm_smmu_sva_bind() and removed in v6.0. mm_pasid_drop() was added in the
// same patch as the removal of ioasid_get(). We assume the presence of
// arm_smmu_sva_bind() if ioasid_get(v5.11 - v5.17) or mm_pasid_drop(v5.18+) is
// present.
//
// arm_smmu_sva_bind() was added with commit
// 32784a9562fb0518b12e9797ee2aec52214adf6f and ioasid_get() was added with
// commit cb4789b0d19ff231ce9f73376a023341300aed96 (11/23/2020). Commit
// 701fac40384f07197b106136012804c3cae0b3de (02/15/2022) removed ioasid_get()
// and added mm_pasid_drop().
    #if UVM_CAN_USE_MMU_NOTIFIERS() && (defined(NV_IOASID_GET_PRESENT) || defined(NV_MM_PASID_DROP_PRESENT))
        #if defined(CONFIG_IOMMU_SVA)
            #define UVM_ATS_SVA_SUPPORTED() 1
        #else
            #define UVM_ATS_SVA_SUPPORTED() 0
        #endif
    #else
        #define UVM_ATS_SVA_SUPPORTED() 0
    #endif

// If NV_MMU_NOTIFIER_OPS_HAS_ARCH_INVALIDATE_SECONDARY_TLBS is defined it
// means the upstream fix is in place so no need for the WAR from
// Bug 4130089: [GH180][r535] WAR for  kernel not issuing SMMU TLB
// invalidates on read-only
#if defined(NV_MMU_NOTIFIER_OPS_HAS_ARCH_INVALIDATE_SECONDARY_TLBS)
    #define UVM_ATS_SMMU_WAR_REQUIRED() 0
#elif NVCPU_IS_AARCH64
    #define UVM_ATS_SMMU_WAR_REQUIRED() 1
#else
    #define UVM_ATS_SMMU_WAR_REQUIRED() 0
#endif

typedef struct
{
    int placeholder;
} uvm_sva_va_space_t;

typedef struct
{
    // Reference count for the iommu_handle
    nv_kref_t kref;
    struct iommu_sva *iommu_handle;
} uvm_sva_gpu_va_space_t;

#if UVM_ATS_SVA_SUPPORTED()
    NV_STATUS uvm_ats_sva_add_gpu(uvm_parent_gpu_t *parent_gpu);
    void uvm_ats_sva_remove_gpu(uvm_parent_gpu_t *parent_gpu);

    // LOCKING: mmap_lock must be lockable
    //          VA space lock must not be held.
    NV_STATUS uvm_ats_sva_bind_gpu(uvm_gpu_va_space_t *gpu_va_space);

    // LOCKING: VA space lock must not be held.
    void uvm_ats_sva_unbind_gpu(uvm_gpu_va_space_t *gpu_va_space);

    // LOCKING: None
    NV_STATUS uvm_ats_sva_register_gpu_va_space(uvm_gpu_va_space_t *gpu_va_space);

    // LOCKING: None
    void uvm_ats_sva_unregister_gpu_va_space(uvm_gpu_va_space_t *gpu_va_space);

    // Fix for Bug 4130089: [GH180][r535] WAR for kernel not issuing SMMU
    // TLB invalidates on read-only to read-write upgrades
    #if UVM_ATS_SMMU_WAR_REQUIRED()
        void uvm_ats_smmu_invalidate_tlbs(uvm_gpu_va_space_t *gpu_va_space, NvU64 addr, size_t size);
    #else
        static void uvm_ats_smmu_invalidate_tlbs(uvm_gpu_va_space_t *gpu_va_space, NvU64 addr, size_t size)
        {

        }
    #endif
#else
    static NV_STATUS uvm_ats_sva_add_gpu(uvm_parent_gpu_t *parent_gpu)
    {
        return NV_OK;
    }

    static void uvm_ats_sva_remove_gpu(uvm_parent_gpu_t *parent_gpu)
    {

    }

    static NV_STATUS uvm_ats_sva_bind_gpu(uvm_gpu_va_space_t *gpu_va_space)
    {
        return NV_OK;
    }

    static void uvm_ats_sva_unbind_gpu(uvm_gpu_va_space_t *gpu_va_space)
    {

    }

    static NV_STATUS uvm_ats_sva_register_gpu_va_space(uvm_gpu_va_space_t *gpu_va_space)
    {
        return NV_OK;
    }

    static void uvm_ats_sva_unregister_gpu_va_space(uvm_gpu_va_space_t *gpu_va_space)
    {

    }

    static void uvm_ats_smmu_invalidate_tlbs(uvm_gpu_va_space_t *gpu_va_space, NvU64 addr, size_t size)
    {

    }
#endif // UVM_ATS_SVA_SUPPORTED

#endif // __UVM_ATS_SVA_H__
