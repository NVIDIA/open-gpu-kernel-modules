/*******************************************************************************
    Copyright (c) 2015-2023 NVIDIA Corporation

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

#ifndef __UVM_GLOBAL_H__
#define __UVM_GLOBAL_H__

#include "nv_uvm_types.h"
#include "uvm_extern_decl.h"
#include "uvm_linux.h"
#include "uvm_common.h"
#include "uvm_processors.h"
#include "uvm_gpu.h"
#include "uvm_lock.h"
#include "uvm_ats_ibm.h"

// Global state of the uvm driver
struct uvm_global_struct
{
    // Mask of retained GPUs.
    // Note that GPUs are added to this mask as the last step of add_gpu() and
    // removed from it as the first step of remove_gpu() implying that a GPU
    // that's being initialized or deinitialized will not be in it.
    uvm_processor_mask_t retained_gpus;

    // Array of the parent GPUs registered with UVM. Note that GPUs will have
    // ids offset by 1 to accomodate the UVM_ID_CPU so e.g., parent_gpus[0]
    // will have GPU id = 1. A GPU entry is unused iff it does not exist
    // (is a NULL pointer) in this table.
    uvm_parent_gpu_t *parent_gpus[UVM_PARENT_ID_MAX_GPUS];

    // A global RM session (RM client)
    // Created on module load and destroyed on module unload
    uvmGpuSessionHandle rm_session_handle;

    // peer-to-peer table
    // peer info is added and removed from this table when usermode
    // driver calls UvmEnablePeerAccess and UvmDisablePeerAccess
    // respectively.
    uvm_gpu_peer_t peers[UVM_MAX_UNIQUE_GPU_PAIRS];

    // peer-to-peer copy mode
    // Pascal+ GPUs support virtual addresses in p2p copies.
    // Ampere+ GPUs add support for physical addresses in p2p copies.
    uvm_gpu_peer_copy_mode_t peer_copy_mode;

    // Stores an NV_STATUS, once it becomes != NV_OK, the driver should refuse to
    // do most anything other than try and clean up as much as possible.
    // An example of a fatal error is an unrecoverable ECC error on one of the
    // GPUs.
    atomic_t fatal_error;

    // A flag to disable the assert on fatal error
    // To be used by tests and only consulted if tests are enabled.
    bool disable_fatal_error_assert;

    // Lock protecting the global state
    uvm_mutex_t global_lock;

    struct
    {
        // Lock synchronizing user threads with power management activity
        uvm_rw_semaphore_t lock;

        // Power management state flag; tested by UVM_GPU_WRITE_ONCE()
        // and UVM_GPU_READ_ONCE() to detect accesses to GPUs when
        // UVM is suspended.
        bool is_suspended;
    } pm;

    // This lock synchronizes addition and removal of GPUs from UVM's global
    // table. It must be held whenever g_uvm_global.parent_gpus[] is written. In
    // order to read from this table, you must hold either the gpu_table_lock,
    // or the global_lock.
    //
    // This is a leaf lock.
    uvm_spinlock_irqsave_t gpu_table_lock;

    // Number of simulated/emulated devices that have registered with UVM
    unsigned num_simulated_devices;

    // A single queue for deferred work that is non-GPU-specific.
    nv_kthread_q_t global_q;

    // A single queue for deferred f_ops->release() handling.  Items scheduled to
    // run on it may block for the duration of system sleep cycles, stalling
    // the queue and preventing any other items from running.
    nv_kthread_q_t deferred_release_q;

    struct
    {
        // Indicates whether the system HW supports ATS. This field is set once
        // during global initialization (uvm_global_init), and can be read
        // afterwards without acquiring any locks.
        bool supported;

        // On top of HW platform support, ATS support can be overridden using
        // the module parameter uvm_ats_mode. This field is set once during
        // global initialization (uvm_global_init), and can be read afterwards
        // without acquiring any locks.
        bool enabled;
    } ats;

#if UVM_IBM_NPU_SUPPORTED()
    // On IBM systems this array tracks the active NPUs (the NPUs which are
    // attached to retained GPUs).
    uvm_ibm_npu_t npus[NV_MAX_NPUS];
#endif

    // List of all active VA spaces
    struct
    {
        uvm_mutex_t lock;
        struct list_head list;
    } va_spaces;

    // Notify a registered process about the driver state after it's unloaded.
    // The intent is to systematically report any error during the driver
    // teardown. unload_state is used for testing only.
    struct
    {
        // ptr points to a 8-byte buffer within page.
        NvU64 *ptr;
        struct page *page;
    } unload_state;

    // True if the VM has AMD's SEV, or equivalent HW security extensions such
    // as Intel's TDX, enabled. The flag is always false on the host.
    //
    // This value moves in tandem with that of Confidential Computing in the
    // GPU(s) in all supported configurations, so it is used as a proxy for the
    // Confidential Computing state.
    //
    // This field is set once during global initialization (uvm_global_init),
    // and can be read afterwards without acquiring any locks.
    bool conf_computing_enabled;
};

// Initialize global uvm state
NV_STATUS uvm_global_init(void);

// Deinitialize global state (called from module exit)
void uvm_global_exit(void);

// Prepare for entry into a system sleep state
NV_STATUS uvm_suspend_entry(void);

// Recover after exit from a system sleep state
NV_STATUS uvm_resume_entry(void);

// Add parent GPU to the global table.
//
// LOCKING: requires that you hold the global lock and gpu_table_lock
static void uvm_global_add_parent_gpu(uvm_parent_gpu_t *parent_gpu)
{
    NvU32 gpu_index = uvm_parent_id_gpu_index(parent_gpu->id);

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);
    uvm_assert_spinlock_locked(&g_uvm_global.gpu_table_lock);

    UVM_ASSERT(!g_uvm_global.parent_gpus[gpu_index]);
    g_uvm_global.parent_gpus[gpu_index] = parent_gpu;
}

// Remove parent GPU from the global table.
//
// LOCKING: requires that you hold the global lock and gpu_table_lock
static void uvm_global_remove_parent_gpu(uvm_parent_gpu_t *parent_gpu)
{
    NvU32 gpu_index = uvm_parent_id_gpu_index(parent_gpu->id);

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);
    uvm_assert_spinlock_locked(&g_uvm_global.gpu_table_lock);

    UVM_ASSERT(g_uvm_global.parent_gpus[gpu_index] == NULL || g_uvm_global.parent_gpus[gpu_index] == parent_gpu);

    g_uvm_global.parent_gpus[gpu_index] = NULL;
}

// Get a parent gpu by its id.
// Returns a pointer to the parent GPU object, or NULL if not found.
//
// LOCKING: requires that you hold the gpu_table_lock, the global lock, or have
// retained at least one of the child GPUs.
static uvm_parent_gpu_t *uvm_parent_gpu_get(uvm_parent_gpu_id_t id)
{
    return g_uvm_global.parent_gpus[uvm_parent_id_gpu_index(id)];
}

// Get a gpu by its GPU id.
// Returns a pointer to the GPU object, or NULL if not found.
//
// LOCKING: requires that you hold the gpu_table_lock, the global_lock, or have
// retained the gpu.
static uvm_gpu_t *uvm_gpu_get(uvm_gpu_id_t gpu_id)
{
    uvm_parent_gpu_t *parent_gpu;

    parent_gpu = g_uvm_global.parent_gpus[uvm_parent_id_gpu_index_from_gpu_id(gpu_id)];
    if (!parent_gpu)
        return NULL;

    return parent_gpu->gpus[uvm_id_sub_processor_index(gpu_id)];
}

static uvmGpuSessionHandle uvm_global_session_handle(void)
{
    return g_uvm_global.rm_session_handle;
}

// Use these READ_ONCE()/WRITE_ONCE() wrappers when accessing GPU resources
// in BAR0/BAR1 to detect cases in which GPUs are accessed when UVM is
// suspended.
#define UVM_GPU_WRITE_ONCE(x, val) do {         \
        UVM_ASSERT(!uvm_global_is_suspended()); \
        UVM_WRITE_ONCE(x, val);                 \
    } while (0)

#define UVM_GPU_READ_ONCE(x) ({                 \
        UVM_ASSERT(!uvm_global_is_suspended()); \
        UVM_READ_ONCE(x);                       \
    })

static bool global_is_fatal_error_assert_disabled(void)
{
    // Only allow the assert to be disabled if tests are enabled
    if (!uvm_enable_builtin_tests)
        return false;

    return g_uvm_global.disable_fatal_error_assert;
}

// Set a global fatal error
// Once that happens the the driver should refuse to do anything other than try
// and clean up as much as possible.
// An example of a fatal error is an unrecoverable ECC error on one of the
// GPUs.
// Use a macro so that the assert below provides precise file and line info and
// a backtrace.
#define uvm_global_set_fatal_error(error)                                       \
    do {                                                                        \
        if (!global_is_fatal_error_assert_disabled())                           \
            UVM_ASSERT_MSG(0, "Fatal error: %s\n", nvstatusToString(error));    \
        uvm_global_set_fatal_error_impl(error);                                 \
    } while (0)
void uvm_global_set_fatal_error_impl(NV_STATUS error);

// Get the global status
static NV_STATUS uvm_global_get_status(void)
{
    return atomic_read(&g_uvm_global.fatal_error);
}

// Reset global fatal error
// This is to be used by tests triggering the global error on purpose only.
// Returns the value of the global error field that existed just before this
// reset call was made.
NV_STATUS uvm_global_reset_fatal_error(void);

static uvm_gpu_t *uvm_processor_mask_find_first_gpu(const uvm_processor_mask_t *gpus)
{
    uvm_gpu_t *gpu;
    uvm_gpu_id_t gpu_id = uvm_processor_mask_find_first_gpu_id(gpus);

    if (UVM_ID_IS_INVALID(gpu_id))
        return NULL;

    gpu = uvm_gpu_get(gpu_id);

    // If there is valid GPU id in the mask, assert that the corresponding
    // uvm_gpu_t is present. Otherwise it would stop a
    // for_each_gpu_in_mask() loop pre-maturely. Today, this could only
    // happen in remove_gpu() because the GPU being removed is deleted from the
    // global table very early.
    UVM_ASSERT_MSG(gpu, "gpu_id %u\n", uvm_id_value(gpu_id));

    return gpu;
}

static uvm_gpu_t *__uvm_processor_mask_find_next_gpu(const uvm_processor_mask_t *gpus, uvm_gpu_t *gpu)
{
    uvm_gpu_id_t gpu_id;

    UVM_ASSERT(gpu);

    gpu_id = uvm_processor_mask_find_next_id(gpus, uvm_gpu_id_next(gpu->id));
    if (UVM_ID_IS_INVALID(gpu_id))
        return NULL;

    gpu = uvm_gpu_get(gpu_id);

    // See comment in uvm_processor_mask_find_first_gpu().
    UVM_ASSERT_MSG(gpu, "gpu_id %u\n", uvm_id_value(gpu_id));

    return gpu;
}

// Helper to iterate over all GPUs in the input mask
#define for_each_gpu_in_mask(gpu, mask)                         \
    for (gpu = uvm_processor_mask_find_first_gpu(mask);         \
         gpu != NULL;                                           \
         gpu = __uvm_processor_mask_find_next_gpu(mask, gpu))

// Helper to iterate over all GPUs retained by the UVM driver
// (across all va spaces).
#define for_each_gpu(gpu)                                                              \
    for (({uvm_assert_mutex_locked(&g_uvm_global.global_lock);                         \
           gpu = uvm_processor_mask_find_first_gpu(&g_uvm_global.retained_gpus);});    \
           gpu != NULL;                                                                \
           gpu = __uvm_processor_mask_find_next_gpu(&g_uvm_global.retained_gpus, gpu))

// LOCKING: Must hold either the global_lock or the gpu_table_lock
static uvm_parent_gpu_t *uvm_global_find_next_parent_gpu(uvm_parent_gpu_t *parent_gpu)
{
    NvU32 i;

    if (parent_gpu) {
        NvU32 gpu_index = uvm_parent_id_gpu_index(parent_gpu->id);
        i = gpu_index + 1;
    }
    else {
        i = 0;
    }

    parent_gpu = NULL;

    while (i < UVM_PARENT_ID_MAX_GPUS) {
        if (g_uvm_global.parent_gpus[i]) {
            parent_gpu = g_uvm_global.parent_gpus[i];
            break;
        }

        i++;
    }

    return parent_gpu;
}

// LOCKING: Must hold the global_lock
static uvm_gpu_t *uvm_gpu_find_next_valid_gpu_in_parent(uvm_parent_gpu_t *parent_gpu, uvm_gpu_t *cur_gpu)
{
    uvm_gpu_t *gpu = NULL;
    uvm_gpu_id_t gpu_id;
    NvU32 sub_processor_index;
    NvU32 cur_sub_processor_index;

    UVM_ASSERT(parent_gpu);

    gpu_id = uvm_gpu_id_from_parent_gpu_id(parent_gpu->id);
    cur_sub_processor_index = cur_gpu ? uvm_id_sub_processor_index(cur_gpu->id) : -1;

    sub_processor_index = find_next_bit(parent_gpu->valid_gpus, UVM_PARENT_ID_MAX_SUB_PROCESSORS, cur_sub_processor_index + 1);
    if (sub_processor_index < UVM_PARENT_ID_MAX_SUB_PROCESSORS) {
        gpu = uvm_gpu_get(uvm_id_from_value(uvm_id_value(gpu_id) + sub_processor_index));
        UVM_ASSERT(gpu != NULL);
    }

    return gpu;
}

// LOCKING: Must hold either the global_lock or the gpu_table_lock
#define for_each_parent_gpu(parent_gpu)                                                         \
    for ((parent_gpu) = uvm_global_find_next_parent_gpu(NULL);                                  \
         (parent_gpu) != NULL;                                                                  \
         (parent_gpu) = uvm_global_find_next_parent_gpu((parent_gpu)))

// LOCKING: Must hold the global_lock
#define for_each_gpu_in_parent(parent_gpu, gpu)                                                 \
    for (({uvm_assert_mutex_locked(&g_uvm_global.global_lock);                                  \
         (gpu) = uvm_gpu_find_next_valid_gpu_in_parent((parent_gpu), NULL);});                  \
         (gpu) != NULL;                                                                         \
         (gpu) = uvm_gpu_find_next_valid_gpu_in_parent((parent_gpu), (gpu)))

// Helper which calls uvm_gpu_retain() on each GPU in mask.
void uvm_global_gpu_retain(const uvm_processor_mask_t *mask);

// Helper which calls uvm_gpu_release_locked on each GPU in mask.
//
// LOCKING: this function takes and releases the global lock if the input mask
//          is not empty
void uvm_global_gpu_release(const uvm_processor_mask_t *mask);

// Check for ECC errors for all GPUs in a mask
// Notably this check cannot be performed where it's not safe to call into RM.
NV_STATUS uvm_global_gpu_check_ecc_error(uvm_processor_mask_t *gpus);

// Pre-allocate fault service contexts.
NV_STATUS uvm_service_block_context_init(void);

// Release fault service contexts if any exist.
void uvm_service_block_context_exit(void);

// Allocate a service block context
uvm_service_block_context_t *uvm_service_block_context_alloc(struct mm_struct *mm);

// Free a servic block context
void uvm_service_block_context_free(uvm_service_block_context_t *service_context);

#endif // __UVM_GLOBAL_H__
