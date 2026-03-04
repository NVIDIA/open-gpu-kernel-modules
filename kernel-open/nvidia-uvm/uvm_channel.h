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

#ifndef __UVM_CHANNEL_H__
#define __UVM_CHANNEL_H__

#include "nv_uvm_types.h"
#include "uvm_forward_decl.h"
#include "uvm_gpu_semaphore.h"
#include "uvm_pushbuffer.h"
#include "uvm_tracker.h"

//
// UVM channels
//
// A channel manager is created as part of the GPU addition. This involves
// creating channels for each of the supported types (uvm_channel_type_t) in
// separate channel pools possibly using different CE instances in the HW. Each
// channel has a uvm_gpu_tracking_semaphore_t and a set of uvm_gpfifo_entry_t
// (one per each HW GPFIFO entry) allowing to track completion of pushes on the
// channel.
//
// Beginning a push on a channel implies reserving a GPFIFO entry in that
// channel and hence there can only be as many on-going pushes per channel as
// there are free GPFIFO entries. This ensures that ending a push won't have to
// wait for a GPFIFO entry to free up.
//

#define UVM_CHANNEL_NUM_GPFIFO_ENTRIES_DEFAULT 1024
#define UVM_CHANNEL_NUM_GPFIFO_ENTRIES_MIN 32
#define UVM_CHANNEL_NUM_GPFIFO_ENTRIES_MAX (1024 * 1024)

// Maximum number of channels per pool.
#define UVM_CHANNEL_MAX_NUM_CHANNELS_PER_POOL UVM_PUSH_MAX_CONCURRENT_PUSHES

// Semaphore payloads cannot advance too much between calls to
// uvm_gpu_tracking_semaphore_update_completed_value(). In practice the jumps
// are bound by gpfifo sizing as we have to update the completed value to
// reclaim gpfifo entries. Set a limit based on the max gpfifo entries we could
// ever see.
//
// Logically this define belongs to uvm_gpu_semaphore.h but it depends on the
// channel GPFIFO sizing defined here so it's easiest to just have it here as
// uvm_channel.h includes uvm_gpu_semaphore.h.
#define UVM_GPU_SEMAPHORE_MAX_JUMP (2 * UVM_CHANNEL_NUM_GPFIFO_ENTRIES_MAX)

#define uvm_channel_pool_assert_locked(pool) (          \
{                                                       \
    if (uvm_channel_pool_uses_mutex(pool))              \
        uvm_assert_mutex_locked(&(pool)->mutex);        \
    else                                                \
        uvm_assert_spinlock_locked(&(pool)->spinlock);  \
})

// Channel types
typedef enum
{
    // CPU to GPU copies
    UVM_CHANNEL_TYPE_CPU_TO_GPU,

    // GPU to CPU copies
    UVM_CHANNEL_TYPE_GPU_TO_CPU,

    // Memsets and copies within the GPU
    UVM_CHANNEL_TYPE_GPU_INTERNAL,

    // Memops and small memsets/copies for writing PTEs
    UVM_CHANNEL_TYPE_MEMOPS,

    // GPU to GPU peer copies
    UVM_CHANNEL_TYPE_GPU_TO_GPU,

    UVM_CHANNEL_TYPE_CE_COUNT,

    // ^^^^^^
    // Channel types backed by a CE.
    // ----------------------------------
    // Channel types not backed by a CE.
    // vvvvvv

    // SEC2 channels
    UVM_CHANNEL_TYPE_SEC2 = UVM_CHANNEL_TYPE_CE_COUNT,

    // ----------------------------------
    // Channel type with fixed schedules

    // Work Launch Channel (WLC) is a specialized channel for launching work on
    // other channels when the Confidential Computing is feature enabled. It is
    // paired with LCIC (below)
    UVM_CHANNEL_TYPE_WLC,

    // Launch Confirmation Indicator Channel (LCIC) is a specialized channel
    // with fixed schedule. It gets triggered by executing WLC work, and makes
    // sure that WLC get/put pointers are up-to-date.
    UVM_CHANNEL_TYPE_LCIC,

    UVM_CHANNEL_TYPE_COUNT,
} uvm_channel_type_t;

typedef enum
{
    // A pool that contains CE channels owned by UVM.
    UVM_CHANNEL_POOL_TYPE_CE = (1 << 0),

    // A proxy pool contains only proxy channels, so it only exists in SR-IOV
    // heavy. The pool is only used for UVM_CHANNEL_TYPE_MEMOPS pushes.
    //
    // A proxy channel is a privileged CE channel owned by the vGPU plugin. A
    // proxy channel cannot be manipulated directly by the UVM driver, who
    // instead can only submit work to it by invoking an RM API.
    //
    // There is a single proxy pool and channel per GPU.
    UVM_CHANNEL_POOL_TYPE_CE_PROXY = (1 << 1),

    // A pool of SEC2 channels owned by UVM. These channels are backed by a SEC2
    // engine.
    UVM_CHANNEL_POOL_TYPE_SEC2 = (1 << 2),

    UVM_CHANNEL_POOL_TYPE_WLC = (1 << 3),

    UVM_CHANNEL_POOL_TYPE_LCIC = (1 << 4),

    UVM_CHANNEL_POOL_TYPE_COUNT = 5,

    // A mask used to select pools of any type.
    UVM_CHANNEL_POOL_TYPE_MASK  = ((1U << UVM_CHANNEL_POOL_TYPE_COUNT) - 1)
} uvm_channel_pool_type_t;

typedef enum
{
    // Push-based GPFIFO entry
    UVM_GPFIFO_ENTRY_TYPE_NORMAL,

    // Control GPFIFO entry, i.e., the LENGTH field is zero, not associated with
    // a push.
    UVM_GPFIFO_ENTRY_TYPE_CONTROL
} uvm_gpfifo_entry_type_t;

struct uvm_gpfifo_entry_struct
{
    uvm_gpfifo_entry_type_t type;

    // Channel tracking semaphore value that indicates completion of
    // this entry.
    NvU64 tracking_semaphore_value;

    union {
        struct {
            // Offset of the pushbuffer in the pushbuffer allocation used by
            // this entry.
            NvU32 pushbuffer_offset;

            // Size of the pushbuffer used for this entry.
            NvU32 pushbuffer_size;
        };

        // Value of control entry
        // Exact value of GPFIFO entry copied directly to GPFIFO[PUT] location.
        NvU64 control_value;
    };

    // The following fields are only valid when type is
    // UVM_GPFIFO_ENTRY_TYPE_NORMAL.

    // List node used by the pushbuffer tracking
    struct list_head pending_list_node;

    // Push info for the pending push that used this GPFIFO entry
    uvm_push_info_t *push_info;
};

// A channel pool is a set of channels that use the same engine. For example,
// all channels in a CE pool share the same (logical) Copy Engine.
typedef struct
{
    // Owning channel manager
    uvm_channel_manager_t *manager;

    // On Volta+ GPUs, all channels in a pool are members of the same TSG, i.e.,
    // num_tsgs is 1. Pre-Volta GPUs also have a single TSG object, but since HW
    // does not support TSG for CE engines, a HW TSG is not created, but a TSG
    // object is required to allocate channels.
    //
    // When Confidential Computing mode is enabled, the WLC and LCIC channel
    // types require one TSG for each WLC/LCIC pair of channels. In this case,
    // we do not use a TSG per channel pool, but instead a TSG per WLC/LCIC
    // channel pair, num_tsgs equals to the number of channel pairs.
    uvmGpuTsgHandle *tsg_handles;

    // Number TSG handles owned by this pool.
    NvU32 num_tsgs;

    // Channels in this pool
    uvm_channel_t *channels;

    // Number of elements in the channel array
    NvU32 num_channels;

    // Index of the engine associated with the pool (index is an offset from the
    // first engine of the same engine type.)
    unsigned engine_index;

    // Pool type: Refer to the uvm_channel_pool_type_t enum.
    uvm_channel_pool_type_t pool_type;

    // Lock protecting the state of channels in the pool.
    //
    // There are two pool lock types available: spinlock and mutex. The mutex
    // variant is required when the thread holding the pool lock must sleep
    // (ex: acquire another mutex) deeper in the call stack, either in UVM or
    // RM.
    union
    {
        uvm_spinlock_t spinlock;
        uvm_mutex_t mutex;
    };

    struct
    {
        // Secure operations require that uvm_push_begin order matches
        // uvm_push_end order, because the engine's state is used in its
        // internal operation and each push may modify this state.
        // push_locks is protected by the channel pool lock.
        DECLARE_BITMAP(push_locks, UVM_CHANNEL_MAX_NUM_CHANNELS_PER_POOL);

        // Counting semaphore for available and unlocked channels, it must be
        // acquired before submitting work to a channel when the Confidential
        // Computing feature is enabled.
        uvm_semaphore_t push_sem;

        // Per channel buffers in unprotected sysmem.
        uvm_rm_mem_t *pool_sysmem;

        // Per channel buffers in protected vidmem.
        uvm_rm_mem_t *pool_vidmem;

       struct
       {
            // Current encryption key version, incremented upon key rotation.
            // While there are separate keys for encryption and decryption, the
            // two keys are rotated at once, so the versioning applies to both.
            NvU32 version;

            // Lock used to ensure mutual exclusion during key rotation.
            uvm_mutex_t mutex;

            // CSL contexts passed to RM for key rotation. This is usually an
            // array containing the CSL contexts associated with the channels in
            // the pool. In the case of the WLC pool, the array also includes
            // CSL contexts associated with LCIC channels.
            UvmCslContext **csl_contexts;

            // Number of elements in the CSL context array.
            unsigned num_csl_contexts;

            // Number of bytes encrypted, or decrypted, on the engine associated
            // with the pool since the last key rotation. Only used during
            // testing, to force key rotations after a certain encryption size,
            // see UVM_CONF_COMPUTING_KEY_ROTATION_LOWER_THRESHOLD.
            //
            // Encryptions on a LCIC pool are accounted for in the paired WLC
            // pool.
            //
            // TODO: Bug 4612912: these accounting variables can be removed once
            // RM exposes an API to set the key rotation lower threshold.
            atomic64_t encrypted;
            atomic64_t decrypted;
        } key_rotation;

    } conf_computing;
} uvm_channel_pool_t;

struct uvm_channel_struct
{
    // Owning pool
    uvm_channel_pool_t *pool;

    // The channel name contains the CE index, and (for UVM internal channels)
    // the HW runlist and channel IDs.
    char name[64];

    // Array of gpfifo entries, one per each HW GPFIFO
    uvm_gpfifo_entry_t *gpfifo_entries;

    // Number of GPFIFO entries in gpfifo_entries
    NvU32 num_gpfifo_entries;

    // Latest GPFIFO entry submitted to the GPU
    // Updated when new pushes are submitted to the GPU in
    // uvm_channel_end_push().
    NvU32 cpu_put;

    // Latest GPFIFO entry completed by the GPU
    // Updated by uvm_channel_update_progress() after checking pending GPFIFOs
    // for completion.
    NvU32 gpu_get;

    // Number of currently on-going gpfifo entries on this channel
    // A new push or control GPFIFO is only allowed to begin on the channel if
    // there is a free GPFIFO entry for it.
    NvU32 current_gpfifo_count;

    // Array of uvm_push_info_t for all pending pushes on the channel
    uvm_push_info_t *push_infos;

    // Array of uvm_push_acquire_info_t for all pending pushes on the channel.
    // Each entry corresponds to the push_infos entry with the same index.
    uvm_push_acquire_info_t *push_acquire_infos;

    // List of uvm_push_info_entry_t that are currently available. A push info
    // entry is not available if it has been assigned to a push
    // (uvm_push_begin), and the GPFIFO entry associated with the push has not
    // been marked as completed.
    struct list_head available_push_infos;

    // GPU tracking semaphore tracking the work in the channel.
    // Each push on the channel increments the semaphore, see
    // uvm_channel_end_push().
    uvm_gpu_tracking_semaphore_t tracking_sem;

    struct
    {
        // Secure operations require that uvm_push_begin order matches
        // uvm_push_end order, because the engine's state is used in
        // its internal operation and each push may modify this state.
        uvm_mutex_t push_lock;

        // When the Confidential Computing feature is enabled, every channel has
        // cryptographic state in HW, which is mirrored here for CPU-side
        // operations.
        UvmCslContext ctx;
        bool is_ctx_initialized;

        // CPU-side CSL crypto operations which operate on the same CSL state
        // are not thread-safe, so they must be wrapped in locks at the UVM
        // level. Encryption, decryption and logging operations must be
        // protected with the ctx_lock.
        uvm_mutex_t ctx_lock;
    } csl;

    struct
    {
        // The value of GPU side PUT index.
        // Indirect work submission introduces delay between updating the CPU
        // put when ending a push, and updating the GPU visible value via
        // indirect work launch. It is used to order multiple pending indirect
        // work launches to match the order of push end-s that triggered them.
        volatile NvU32 gpu_put;

        // Protected sysmem location makes WLC independent from the pushbuffer
        // allocator. Unprotected sysmem and protected vidmem counterparts
        // are allocated from the channel pool (sysmem, vidmem).
        void *static_pb_protected_sysmem;

        // Used to decrypt the push back to protected sysmem.
        // This happens when profilers register callbacks for migration data.
        uvm_push_crypto_bundle_t *push_crypto_bundles;
    } conf_computing;

    // RM channel information
    union
    {
        // UVM internal channels
        struct
        {
            // UVM-RM interface handle
            uvmGpuChannelHandle handle;

            // Channel state populated by RM. Includes the GPFIFO, error
            // notifier, work submission information etc.
            UvmGpuChannelInfo channel_info;
        };

        // Proxy channels (SR-IOV heavy only)
        struct
        {
            // UVM-RM interface handle
            UvmGpuPagingChannelHandle handle;

            // Channel state populated by RM. Includes the error notifier.
            UvmGpuPagingChannelInfo channel_info;
        } proxy;
    };

    struct
    {
        struct proc_dir_entry *dir;
        struct proc_dir_entry *info;
        struct proc_dir_entry *pushes;
    } procfs;

    // Information managed by the tools event notification mechanism. Mainly
    // used to keep a list of channels with pending events, which is needed
    // to collect the timestamps of asynchronous operations.
    struct
    {
        struct list_head channel_list_node;
        NvU32 pending_event_count;
    } tools;

    bool suspended_p2p;
};

struct uvm_channel_manager_struct
{
    // The owning GPU
    uvm_gpu_t *gpu;

    // The pushbuffer used for all pushes done with this channel manager
    uvm_pushbuffer_t *pushbuffer;

    // Array of channel pools.
    uvm_channel_pool_t *channel_pools;

    // Number of elements in the pool array
    unsigned num_channel_pools;

    // Mask containing the indexes of the usable Copy Engines. Each usable CE
    // has at least one pool of type UVM_CHANNEL_POOL_TYPE_CE associated with it
    DECLARE_BITMAP(ce_mask, UVM_COPY_ENGINE_COUNT_MAX);

    struct
    {
        // Pools to be used by each channel type by default.
        //
        // Transfers of a given type may use a pool different from that in
        // default_for_type[type]. For example, transfers to NvLink GPU
        // peers may instead use the more optimal pool stored in the gpu_to_gpu
        // array
        uvm_channel_pool_t *default_for_type[UVM_CHANNEL_TYPE_COUNT];

        // Optimal pools to use when writing from the owning GPU to its NvLink
        // peers.
        // If there is no optimal pool (the entry is NULL), use default pool
        // default_for_type[UVM_CHANNEL_GPU_TO_GPU] instead.
        uvm_channel_pool_t *gpu_to_gpu[UVM_ID_MAX_GPUS];
    } pool_to_use;

    struct
    {
        struct proc_dir_entry *channels_dir;
        struct proc_dir_entry *pending_pushes;
    } procfs;

    struct
    {
        NvU32 num_gpfifo_entries;
        UVM_BUFFER_LOCATION gpfifo_loc;
        UVM_BUFFER_LOCATION gpput_loc;
        UVM_BUFFER_LOCATION pushbuffer_loc;
    } conf;

    struct
    {
        // Flag indicating that the WLC/LCIC mechanism is ready/setup; should
        // only be false during (de)initialization.
        bool wlc_ready;

        // True indicates that key rotation is enabled (UVM-wise).
        bool key_rotation_enabled;
    } conf_computing;
};

// Index of a channel pool within the manager
static unsigned uvm_channel_pool_index_in_channel_manager(const uvm_channel_pool_t *pool)
{
    return pool - pool->manager->channel_pools;
}

// Create a channel manager for the GPU
NV_STATUS uvm_channel_manager_create(uvm_gpu_t *gpu, uvm_channel_manager_t **manager_out);

static bool uvm_pool_type_is_valid(uvm_channel_pool_type_t pool_type)
{
    return (is_power_of_2(pool_type) && (pool_type < UVM_CHANNEL_POOL_TYPE_MASK));
}

static bool uvm_channel_pool_is_sec2(uvm_channel_pool_t *pool)
{
    UVM_ASSERT(uvm_pool_type_is_valid(pool->pool_type));

    return (pool->pool_type == UVM_CHANNEL_POOL_TYPE_SEC2);
}

static bool uvm_channel_pool_is_wlc(uvm_channel_pool_t *pool)
{
    UVM_ASSERT(uvm_pool_type_is_valid(pool->pool_type));

    return (pool->pool_type == UVM_CHANNEL_POOL_TYPE_WLC);
}

static bool uvm_channel_pool_is_lcic(uvm_channel_pool_t *pool)
{
    UVM_ASSERT(uvm_pool_type_is_valid(pool->pool_type));

    return (pool->pool_type == UVM_CHANNEL_POOL_TYPE_LCIC);
}

static bool uvm_channel_is_sec2(uvm_channel_t *channel)
{
    return uvm_channel_pool_is_sec2(channel->pool);
}

static bool uvm_channel_is_wlc(uvm_channel_t *channel)
{
    return uvm_channel_pool_is_wlc(channel->pool);
}

static bool uvm_channel_is_lcic(uvm_channel_t *channel)
{
    return uvm_channel_pool_is_lcic(channel->pool);
}

uvm_channel_t *uvm_channel_lcic_get_paired_wlc(uvm_channel_t *lcic_channel);

uvm_channel_t *uvm_channel_wlc_get_paired_lcic(uvm_channel_t *wlc_channel);

NvU64 uvm_channel_get_static_pb_protected_vidmem_gpu_va(uvm_channel_t *channel);

NvU64 uvm_channel_get_static_pb_unprotected_sysmem_gpu_va(uvm_channel_t *channel);

char* uvm_channel_get_static_pb_unprotected_sysmem_cpu(uvm_channel_t *channel);

bool uvm_channel_pool_is_p2p(uvm_channel_pool_t *pool);

static bool uvm_channel_pool_is_proxy(uvm_channel_pool_t *pool)
{
    UVM_ASSERT(uvm_pool_type_is_valid(pool->pool_type));

    return pool->pool_type == UVM_CHANNEL_POOL_TYPE_CE_PROXY;
}

static bool uvm_channel_is_proxy(uvm_channel_t *channel)
{
    return uvm_channel_pool_is_proxy(channel->pool);
}

static bool uvm_channel_pool_is_ce(uvm_channel_pool_t *pool)
{
    return !uvm_channel_pool_is_sec2(pool);
}

static bool uvm_channel_is_p2p(uvm_channel_t *channel)
{
    return uvm_channel_pool_is_p2p(channel->pool);
}

static bool uvm_channel_is_ce(uvm_channel_t *channel)
{
    return uvm_channel_pool_is_ce(channel->pool);
}

bool uvm_channel_pool_uses_mutex(uvm_channel_pool_t *pool);

// Proxy channels are used to push page tree related methods, so their channel
// type is UVM_CHANNEL_TYPE_MEMOPS.
static uvm_channel_type_t uvm_channel_proxy_channel_type(void)
{
    return UVM_CHANNEL_TYPE_MEMOPS;
}

// Force key rotation in the engine associated with the given channel pool.
// Rotation may still not happen if RM cannot acquire the necessary locks (in
// which case the function returns NV_ERR_STATE_IN_USE).
//
// This function should be only invoked in pools in which key rotation is
// enabled.
NV_STATUS uvm_channel_pool_rotate_key(uvm_channel_pool_t *pool);

// Retrieve the current encryption key version associated with the channel pool.
NvU32 uvm_channel_pool_key_version(uvm_channel_pool_t *pool);

// Privileged channels support all the Host and engine methods, while
// non-privileged channels don't support privileged methods.
//
// A major limitation of non-privileged CE channels is lack of physical
// addressing support.
bool uvm_channel_is_privileged(uvm_channel_t *channel);

// Destroy the channel manager
void uvm_channel_manager_destroy(uvm_channel_manager_t *channel_manager);

// Suspend p2p traffic on channels used for p2p operations.
// This is used in STO recovery sequence to quiet nvlink traffic before the
// links can be restored.
NV_STATUS uvm_channel_manager_suspend_p2p(uvm_channel_manager_t *channel_manager);

// Resume p2p traffic on channels used for p2p operations.
// This is used at the end of the STO recovery sequence to resume suspended p2p
// traffic on p2p channels.
void uvm_channel_manager_resume_p2p(uvm_channel_manager_t *channel_manager);

// Get the current status of the channel
// Returns NV_OK if the channel is in a good state,
//         NV_ERR_RC_ERROR otherwise.
// Notably this never sets the global fatal error.
NV_STATUS uvm_channel_get_status(uvm_channel_t *channel);

// Check for channel errors
// Checks for channel errors by calling uvm_channel_get_status().
// If a fatal error occurred, sets the global fatal error and prints errors.
NV_STATUS uvm_channel_check_errors(uvm_channel_t *channel);

// Check errors on all channels in the channel manager
// Also includes uvm_global_get_status
NV_STATUS uvm_channel_manager_check_errors(uvm_channel_manager_t *channel_manager);

// Retrieve the GPFIFO entry that caused a channel error
// The channel has to be in error state prior to calling this function.
uvm_gpfifo_entry_t *uvm_channel_get_fatal_entry(uvm_channel_t *channel);

// Update progress of a specific channel
// Returns the number of still pending GPFIFO entries for that channel.
// Notably some of the pending GPFIFO entries might be already completed, but
// the update early-outs after completing a fixed number of them to spread the
// cost of the updates across calls.
NvU32 uvm_channel_update_progress(uvm_channel_t *channel);

// Update progress of all channels
// Returns the number of still pending GPFIFO entries for all channels.
// Notably some of the pending GPFIFO entries might be already completed, but
// the update early-outs after completing a fixed number of them to spread the
// cost of the updates across calls.
NvU32 uvm_channel_manager_update_progress(uvm_channel_manager_t *channel_manager);

// Wait for all channels to idle
// It waits for anything that is running, but doesn't prevent new work from
// beginning.
NV_STATUS uvm_channel_manager_wait(uvm_channel_manager_t *manager);

static bool uvm_channel_manager_is_wlc_ready(uvm_channel_manager_t *manager)
{
    return manager->conf_computing.wlc_ready;
}

// Get the GPU VA of semaphore_channel's tracking semaphore within the VA space
// associated with access_channel.
//
// The channels can belong to different GPUs, the same GPU, or even be
// identical, in which case uvm_channel_tracking_semaphore_get_gpu_va can be
// used instead.
NvU64 uvm_channel_tracking_semaphore_get_gpu_va_in_channel(uvm_channel_t *semaphore_channel,
                                                           uvm_channel_t *access_channel);

// See above.
static NvU64 uvm_channel_tracking_semaphore_get_gpu_va(uvm_channel_t *channel)
{
    return uvm_channel_tracking_semaphore_get_gpu_va_in_channel(channel, channel);
}

// Check whether the channel completed a value
bool uvm_channel_is_value_completed(uvm_channel_t *channel, NvU64 value);

// Update and get the latest completed value by the channel
NvU64 uvm_channel_update_completed_value(uvm_channel_t *channel);

// Wait for the channel to idle
// It waits for anything that is running, but doesn't prevent new work from
// beginning.
NV_STATUS uvm_channel_wait(uvm_channel_t *channel);

// Select and reserve a channel with the specified type for a push
NV_STATUS uvm_channel_reserve_type(uvm_channel_manager_t *manager,
                                   uvm_channel_type_t type,
                                   uvm_channel_t **channel_out);

// Select and reserve a channel for a transfer from channel_manager->gpu to
// dst_gpu.
NV_STATUS uvm_channel_reserve_gpu_to_gpu(uvm_channel_manager_t *channel_manager,
                                         uvm_gpu_t *dst_gpu,
                                         uvm_channel_t **channel_out);

// Reserve a specific channel for a push or for a control GPFIFO entry.
NV_STATUS uvm_channel_reserve(uvm_channel_t *channel, NvU32 num_gpfifo_entries);

// Release reservation on a specific channel
void uvm_channel_release(uvm_channel_t *channel, NvU32 num_gpfifo_entries);

// Set optimal CE for P2P transfers between manager->gpu and peer
void uvm_channel_manager_set_p2p_ce(uvm_channel_manager_t *manager, uvm_gpu_t *peer, NvU32 optimal_ce);

// Begin a push on a previously reserved channel
// Should be used by uvm_push_*() only.
NV_STATUS uvm_channel_begin_push(uvm_channel_t *channel, uvm_push_t *push);

// End a push
// Should be used by uvm_push_end() only.
void uvm_channel_end_push(uvm_push_t *push);

// Write/send a control GPFIFO to channel. This is not supported by proxy
// channels.
// Ordering guarantees:
// Input: Control GPFIFO entries are guaranteed to be processed by ESCHED after
// all prior GPFIFO entries and pushbuffers have been fetched, but not
// necessarily completed.
// Output ordering: A caller can wait for this control entry to complete with
// uvm_channel_manager_wait(), or by waiting for any later push in the same
// channel to complete.
NV_STATUS uvm_channel_write_ctrl_gpfifo(uvm_channel_t *channel, NvU64 ctrl_fifo_entry_value);

const char *uvm_channel_type_to_string(uvm_channel_type_t channel_type);
const char *uvm_channel_pool_type_to_string(uvm_channel_pool_type_t channel_pool_type);

// Returns the number of available GPFIFO entries. The function internally
// acquires the channel pool lock.
NvU32 uvm_channel_get_available_gpfifo_entries(uvm_channel_t *channel);

void uvm_channel_print_pending_pushes(uvm_channel_t *channel);

bool uvm_channel_is_locked_for_push(uvm_channel_t *channel);

static uvm_gpu_t *uvm_channel_get_gpu(uvm_channel_t *channel)
{
    return channel->pool->manager->gpu;
}

static uvm_pushbuffer_t *uvm_channel_get_pushbuffer(uvm_channel_t *channel)
{
    return channel->pool->manager->pushbuffer;
}

// Index of a channel within the owning pool
static unsigned uvm_channel_index_in_pool(const uvm_channel_t *channel)
{
    return channel - channel->pool->channels;
}

NvU32 uvm_channel_update_progress_all(uvm_channel_t *channel);

// Return an arbitrary channel of the given type(s)
uvm_channel_t *uvm_channel_any_of_type(uvm_channel_manager_t *manager, NvU32 pool_type_mask);

// Return an arbitrary channel of any type
static uvm_channel_t *uvm_channel_any(uvm_channel_manager_t *manager)
{
    return uvm_channel_any_of_type(manager, UVM_CHANNEL_POOL_TYPE_MASK);
}

// Helper to iterate over all the channels in a pool.
#define uvm_for_each_channel_in_pool(channel, pool)                            \
    for (({UVM_ASSERT(pool->channels);                                         \
         channel = pool->channels;});                                          \
         channel != pool->channels + pool->num_channels;                       \
         channel++)

uvm_channel_pool_t *uvm_channel_pool_first(uvm_channel_manager_t *manager, NvU32 pool_type_mask);
uvm_channel_pool_t *uvm_channel_pool_next(uvm_channel_manager_t *manager,
                                          uvm_channel_pool_t *curr_pool,
                                          NvU32 pool_type_mask);

// Helper to iterate over all the channel pools of the given type(s) in a GPU.
// The pool mask must not be zero.
#define uvm_for_each_pool_of_type(pool, manager, pool_type_mask)               \
    for (pool = uvm_channel_pool_first(manager, pool_type_mask);               \
         pool != NULL;                                                         \
         pool = uvm_channel_pool_next(manager, pool, pool_type_mask))

#define uvm_for_each_pool(pool, manager) uvm_for_each_pool_of_type(pool, manager, UVM_CHANNEL_POOL_TYPE_MASK)

#endif // __UVM_CHANNEL_H__
