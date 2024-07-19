/*******************************************************************************
    Copyright (c) 2015 NVIDIA Corporation

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

#ifndef __UVM_GPU_SEMAPHORE_H__
#define __UVM_GPU_SEMAPHORE_H__

#include "uvm_forward_decl.h"
#include "uvm_lock.h"
#include "uvm_rm_mem.h"
#include "uvm_linux.h"

// A GPU semaphore is a memory location accessible by the GPUs and the CPU
// that's used for synchronization among them.
// The GPU has primitives to acquire (wait for) and release (set) 4-byte memory
// locations. The same memory can be accessed by multiple GPUs and the CPU
// allowing for different synchronization schemes.
//
// The UVM driver maintains a per-GPU semaphore pool that grows on demand as
// semaphores are allocated out of it.
//
// TODO: Bug 200194638: Add support for timestamps (the GPU also supports
//       releasing 16-byte semaphores that include an 8-byte timestamp).
struct uvm_gpu_semaphore_struct
{
    // The semaphore pool page the semaphore came from
    uvm_gpu_semaphore_pool_page_t *page;

    // Index of the semaphore in semaphore page
    NvU16 index;

    struct {
        UvmCslIv *ivs;
        NvU32 cached_payload;

        NvU32 last_pushed_notifier;
        NvU32 last_observed_notifier;
    } conf_computing;
};

// A primitive used for tracking progress of the GPU
// Whenever a stream of GPU operations needs to be synchronized it increments
// the semaphore's payload as the last step so that other processors
// can acquire (wait for) it.
// The primitive maintains a 64-bit counter on top of the 32-bit GPU semaphore
// to support 2^64 synchronization points instead of just 2^32. The logic relies
// on being able to notice every time the 32-bit counter wraps around (see
// update_completed_value()).
struct uvm_gpu_tracking_semaphore_struct
{
    uvm_gpu_semaphore_t semaphore;

    // Last completed value
    // The bottom 32-bits will always match the latest semaphore payload seen in
    // update_completed_value_locked().
    atomic64_t completed_value;

    // Lock protecting updates to the completed_value
    union {
        uvm_spinlock_t s_lock;
        uvm_mutex_t m_lock;
    };

    // Last queued value
    // All accesses to the queued value should be handled by the user of the GPU
    // tracking semaphore.
    NvU64 queued_value;
};

// Create a semaphore pool for a GPU.
NV_STATUS uvm_gpu_semaphore_pool_create(uvm_gpu_t *gpu, uvm_gpu_semaphore_pool_t **pool_out);

// When the Confidential Computing feature is enabled, semaphore pools
// associated with CE channels are allocated in the CPR of vidmem and as such
// have all the associated access restrictions. Because of this, they're called
// secure pools and secure semaphores are allocated out of said secure pools.
NV_STATUS uvm_gpu_semaphore_secure_pool_create(uvm_gpu_t *gpu, uvm_gpu_semaphore_pool_t **pool_out);

// Destroy a semaphore pool
// Locking:
//  - Global lock needs to be held in read mode (for unmapping from all GPUs)
//  - Internally acquires:
//    - GPU semaphore pool lock
//    - RM API lock
//    - RM GPUs lock
void uvm_gpu_semaphore_pool_destroy(uvm_gpu_semaphore_pool_t *pool);

// Allocate a semaphore from the pool.
// The semaphore will be mapped on all GPUs currently registered with the UVM
// driver, and on all new GPUs which will be registered in the future.
// Unless the Confidential Computing feature is enabled and the pool is a
// secure pool. In this case, it is only mapped to the GPU that holds the
// allocation.
// The mappings are added to UVM's internal address space, and (in SR-IOV heavy)
// to the proxy address space.
//
// The semaphore's payload will be initially set to 0.
//
// Locking:
//  - Global lock needs to be held in read mode (for mapping on all GPUs)
//  - Internally synchronized and hence safe to be called from multiple threads
//  - Internally acquires:
//    - GPU semaphore pool lock
//    - RM API lock
//    - RM GPUs lock
NV_STATUS uvm_gpu_semaphore_alloc(uvm_gpu_semaphore_pool_t *pool, uvm_gpu_semaphore_t *semaphore);

// Free a semaphore
// Locking:
//  - Internally synchronized and hence safe to be called from multiple threads
void uvm_gpu_semaphore_free(uvm_gpu_semaphore_t *semaphore);

// Map all the semaphores from the pool on a GPU
//
// The mappings are added to UVM's internal address space, and (in SR-IOV heavy)
// to the proxy address space.
NV_STATUS uvm_gpu_semaphore_pool_map_gpu(uvm_gpu_semaphore_pool_t *pool, uvm_gpu_t *gpu);

// Unmap all the semaphores from the pool from a GPU
//
// The unmapping affects all the VA spaces where the semaphores are currently
// mapped.
void uvm_gpu_semaphore_pool_unmap_gpu(uvm_gpu_semaphore_pool_t *pool, uvm_gpu_t *gpu);

// Get the GPU VA of a semaphore in UVM's internal address space.
NvU64 uvm_gpu_semaphore_get_gpu_uvm_va(uvm_gpu_semaphore_t *semaphore, uvm_gpu_t *gpu);

// Get the GPU VA of a semaphore in the proxy address space.
NvU64 uvm_gpu_semaphore_get_gpu_proxy_va(uvm_gpu_semaphore_t *semaphore, uvm_gpu_t *gpu);

NvU64 uvm_gpu_semaphore_get_gpu_va(uvm_gpu_semaphore_t *semaphore, uvm_gpu_t *gpu, bool is_proxy_va_space);

NvU32 *uvm_gpu_semaphore_get_cpu_va(uvm_gpu_semaphore_t *semaphore);

NvU32 *uvm_gpu_semaphore_get_encrypted_payload_cpu_va(uvm_gpu_semaphore_t *semaphore);
uvm_gpu_address_t uvm_gpu_semaphore_get_encrypted_payload_gpu_va(uvm_gpu_semaphore_t *semaphore);

NvU32 *uvm_gpu_semaphore_get_notifier_cpu_va(uvm_gpu_semaphore_t *semaphore);
uvm_gpu_address_t uvm_gpu_semaphore_get_notifier_gpu_va(uvm_gpu_semaphore_t *semaphore);

void *uvm_gpu_semaphore_get_auth_tag_cpu_va(uvm_gpu_semaphore_t *semaphore);
uvm_gpu_address_t uvm_gpu_semaphore_get_auth_tag_gpu_va(uvm_gpu_semaphore_t *semaphore);

// Read the 32-bit payload of the semaphore
// Notably doesn't provide any memory ordering guarantees and needs to be used with
// care. For an example of what needs to be considered see
// uvm_gpu_tracking_semaphore_update_completed_value().
NvU32 uvm_gpu_semaphore_get_payload(uvm_gpu_semaphore_t *semaphore);

// Set the 32-bit payload of the semaphore
// Guarantees that all memory accesses preceding setting the payload won't be
// moved past it.
void uvm_gpu_semaphore_set_payload(uvm_gpu_semaphore_t *semaphore, NvU32 payload);

// Allocate a GPU tracking semaphore from the pool
// Locking same as uvm_gpu_semaphore_alloc()
NV_STATUS uvm_gpu_tracking_semaphore_alloc(uvm_gpu_semaphore_pool_t *pool, uvm_gpu_tracking_semaphore_t *tracking_sem);

// Free a GPU tracking semaphore
// Locking same as uvm_gpu_semaphore_free()
void uvm_gpu_tracking_semaphore_free(uvm_gpu_tracking_semaphore_t *tracking_sem);

// Check whether a specific value has been completed
//
// If true is returned, guarantees that all operations ordered prior to a
// processor (commonly a GPU) completing the specific value will be visible to
// the caller.
//
// In case a GPU is supposed to complete a value, care needs to be taken for all GPU
// operations to be ordered correctly with the semaphore release that sets the value.
// In case it's the CPU completing the value, uvm_gpu_semaphore_set_payload()
// should be used that provides the necessary ordering guarantees.
//
// Locking: this operation is internally synchronized and hence safe to be
// called from multiple threads.
bool uvm_gpu_tracking_semaphore_is_value_completed(uvm_gpu_tracking_semaphore_t *tracking_sem, NvU64 value);

// Update and return the completed value
//
// Provides the same guarantees as if uvm_gpu_tracking_semaphore_is_value_completed()
// returned true for the returned completed value.
//
// Locking: this operation is internally synchronized and hence safe to be
// called from multiple threads.
NvU64 uvm_gpu_tracking_semaphore_update_completed_value(uvm_gpu_tracking_semaphore_t *tracking_sem);

// See the comments for uvm_gpu_tracking_semaphore_is_value_completed
static bool uvm_gpu_tracking_semaphore_is_completed(uvm_gpu_tracking_semaphore_t *tracking_sem)
{
    return uvm_gpu_tracking_semaphore_is_value_completed(tracking_sem, tracking_sem->queued_value);
}

#endif // __UVM_GPU_SEMAPHORE_H__
