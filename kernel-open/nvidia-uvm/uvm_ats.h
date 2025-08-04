/*******************************************************************************
    Copyright (c) 2018-2025 NVIDIA Corporation

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

#ifndef __UVM_ATS_H__
#define __UVM_ATS_H__

#include "uvm_linux.h"
#include "uvm_forward_decl.h"
#include "nv_uvm_types.h"
#include "uvm_lock.h"
#include "uvm_ats_sva.h"

#define UVM_ATS_SUPPORTED() UVM_ATS_SVA_SUPPORTED()

typedef enum
{
    // The VA space has been initialized on a system that supports ATS but it is
    // unknown yet whether it will hold ATS capable GPUs or not.
    UVM_ATS_VA_SPACE_ATS_UNSET = 0,

    // The VA space only allows registering GPUs that support ATS. This state is
    // entered on a system that supports ATS by:
    //   - Registering an ATS capable GPU
    //   - Migrating or setting a policy on pageable memory
    // Once entered, this state is never left.
    UVM_ATS_VA_SPACE_ATS_SUPPORTED,

    // The VA space only allows registering GPUs that do not support ATS. This
    // state is entered by:
    //   - Initialization on platforms that do not support ATS
    //   - Registering a non ATS capable GPU on platforms that do support ATS
    // Once entered, this state is never left.
    UVM_ATS_VA_SPACE_ATS_UNSUPPORTED
} uvm_ats_va_space_state_t;

typedef struct
{
    // Mask of gpu_va_spaces which are registered for ATS access. The mask is
    // indexed by gpu->id. This mask is protected by the VA space lock.
    uvm_processor_mask_t registered_gpu_va_spaces;

    // Protects racing invalidates in the VA space while hmm_range_fault() is
    // being called in ats_compute_residency_mask().
    uvm_rw_semaphore_t lock;

    uvm_sva_va_space_t sva;

    // Tracks if ATS is supported in this va_space. The state is set during GPU
    // registration or some ATS related calls, and is protected as an atomic.
    // This is because during some ATS related API calls a VA space can
    // transition from UNSET to ATS_SUPPORTED. In these cases the va_space's
    // semaphore is only held in a read-locked state. A race results in the
    // duplicate writing of ATS_SUPPORTED to the state value. 
    // This OK as the only possible transition here is
    // UVM_ATS_VA_SPACE_ATS_UNSET -> UVM_ATS_VA_SPACE_ATS_SUPPORTED.
    // Entering UVM_ATS_VA_SPACE_ATS_UNSUPPORTED requires registering a
    // GPU in the VA space which must hold the lock in write mode.
    // Enums from uvm_ats_va_space_state_t are stored in this atomic_t value.
    atomic_t state;
} uvm_ats_va_space_t;

typedef struct
{
    // Each GPU VA space can have ATS enabled or disabled in its hardware
    // state. This is controlled by user space when it allocates that GPU VA
    // space object from RM. This flag indicates the mode user space
    // requested when allocating this GPU VA space.
    bool enabled;

    NvU32 pasid;

    uvm_sva_gpu_va_space_t sva;
} uvm_ats_gpu_va_space_t;

// Initializes driver-wide ATS state
//
// LOCKING: None
void uvm_ats_init(const UvmPlatformInfo *platform_info);

// Enables ATS feature on the GPU.
//
// LOCKING: g_uvm_global.global lock mutex must be held.
NV_STATUS uvm_ats_add_gpu(uvm_parent_gpu_t *parent_gpu);

// Disables ATS feature on the GPU. The caller is responsible for ensuring
// that the GPU won't issue ATS requests anymore prior to calling this function.
//
// LOCKING: g_uvm_global.global lock mutex must be held.
void uvm_ats_remove_gpu(uvm_parent_gpu_t *parent_gpu);

// Creates a binding on the GPU for the mm associated with the VA space
// (va_space_mm). Multiple calls to this function are tracked and refcounted for
// the specific {gpu, mm} pair. A successful uvm_ats_add_gpu() must precede a
// call to this function.
//
// LOCKING: mmap_lock must be lockable.
//          VA space lock must be lockable.
//          gpu_va_space->gpu must be retained.
//          mm must be retained with uvm_va_space_mm_retain() iff
//          UVM_ATS_SVA_SUPPORTED() is 1
NV_STATUS uvm_ats_bind_gpu(uvm_gpu_va_space_t *gpu_va_space);

// Decrements the refcount on the {gpu, mm} pair. Removes the binding from the
// mm (va_space_mm) to this GPU when the refcount reaches zero.
//
// LOCKING: None
void uvm_ats_unbind_gpu(uvm_gpu_va_space_t *gpu_va_space);

// Enables ATS access on the GPU for the mm_struct associated with the VA space
// (va_space_mm) and assigns a PASID. A successful uvm_ats_bind_gpu() must
// precede a call to this function. Returns NV_ERR_INVALID_DEVICE if the
// gpu_va_space is already registered for ATS access.
//
// LOCKING: The VA space lock must be held in write mode.
//          mm has to be retained prior to calling this function.
NV_STATUS uvm_ats_register_gpu_va_space(uvm_gpu_va_space_t *gpu_va_space);

// Disables ATS access for the gpu_va_space. Prior to calling this function,
// the caller must guarantee that the GPU will no longer make any ATS
// accesses in this GPU VA space, and that no ATS fault handling for this
// GPU will be attempted.
//
// LOCKING: This function will acquire the VA space lock, so it must not be
// held.
void uvm_ats_unregister_gpu_va_space(uvm_gpu_va_space_t *gpu_va_space);

#endif // __UVM_ATS_H__
