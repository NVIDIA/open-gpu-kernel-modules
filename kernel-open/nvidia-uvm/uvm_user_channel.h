/*******************************************************************************
    Copyright (c) 2016-2024 NVIDIA Corporation

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

#ifndef __UVM_USER_CHANNEL_H__
#define __UVM_USER_CHANNEL_H__

#include "uvm_forward_decl.h"
#include "uvm_va_space.h"
#include "uvm_hal_types.h"
#include "uvm_rb_tree.h"
#include "nv-kref.h"

// This structure contains the GPU VA spaces of all the subcontexts in a TSG. It
// is stored in a per-GPU UVM RB tree and is required to perform instance_ptr
// to GPU VA space translations when channels are registered in a subcontext,
// since SM fault/access counter notification packets may report any
// instance_ptr in the TSG.
typedef struct
{
    // Number of instance pointers referencing this subcontext info descriptor
    NvU32 total_refcount;

    // All channels in a TSG must be bound to the same SMC Engine
    NvU32 smc_engine_id;

    // Array of per-subcontext information
    struct
    {
        uvm_gpu_va_space_t *gpu_va_space;

        // Number of instance pointers referencing this specific subcontext
        NvU32 refcount;
    } *subctxs;

    // UVM RB tree node for insertion into the parent GPU's tsg_table.
    uvm_rb_tree_node_t node;
} uvm_user_channel_subctx_info_t;

struct uvm_user_channel_struct
{
    // Parent GPU VA space
    uvm_gpu_va_space_t *gpu_va_space;

    // Parent GPU. This is also available in gpu_va_space->gpu, but we need a
    // separate pointer which outlives the gpu_va_space during deferred channel
    // teardown.
    uvm_gpu_t *gpu;

    // RM handles used to register this channel. We store them for UVM-internal
    // purposes to look up the uvm_user_channel_t for unregistration, rather
    // than introducing a new "UVM channel handle" object for user space.
    //
    // DO NOT TRUST THESE VALUES AFTER UVM_REGISTER_CHANNEL. They are passed by
    // user-space at channel registration time to validate the channel with RM,
    // but afterwards the user could free and reallocate either of the client or
    // object handles, so we can't pass them to RM trusting that they still
    // represent this channel.
    //
    // That's ok because we never pass these handles to RM again after
    // registration.
    uvm_rm_user_object_t user_rm_channel;

    // Type of the engine the channel is bound to
    UVM_GPU_CHANNEL_ENGINE_TYPE engine_type;

    // true if the channel belongs to a subcontext or false if the channel
    // belongs to a regular context
    bool in_subctx;

    // Subcontext ID, aka VEID
    NvU32 subctx_id;

    struct
    {
        // Whether the channel belongs to a TSG or not
        bool valid;

        // If valid is true, tsg_id contains the ID of the TSG
        NvU32 id;

        // If valid is true, this is the maximum number of subcontexts in the
        // TSG
        NvU32 max_subctx_count;
    } tsg;

    // On Turing+, the CLEAR_FAULTED method requires passing a RM-provided
    // handle to identify the channel.
    NvU32 clear_faulted_token;

    // Tracker used to aggregate clear faulted operations, needed for user
    // channel removal
    uvm_tracker_t clear_faulted_tracker;

    // Address of the NV_CHRAM_CHANNEL register and the runlist PRI base
    // register. Only valid on GPUs with
    // non_replayable_faults_supported && !has_clear_faulted_channel_method.
    volatile NvU32 *chram_channel_register;
    volatile NvU32 *runlist_pri_base_register;

    // Id of the SMC engine this channel is bound to, or zero if the GPU
    // does not support SMC or it is a CE channel
    NvU32 smc_engine_id;

    // VEIDs are partitioned under SMC (each SMC engine owns a subrange of
    // VEIDs), but the VEID reported in fault packets* is the global (although
    // GFID-local) VEID. In order to compute the SMC engine-local VEID, we need
    // to subtract smc_engine_ve_id_offset from the reported one.
    //
    // *Access counter packets already report SMC engine-local VEIDs.
    NvU32 smc_engine_ve_id_offset;

    // If in_subctx is true, subctx_info will point at a per-TSG data structure
    // that contains the VA spaces of all the subcontexts in the TSG. This value
    // is assigned in uvm_parent_gpu_add_user_channel.
    uvm_user_channel_subctx_info_t *subctx_info;

    // Number of resources reported by RM. This is the size of both the
    // resources and va_ranges arrays.
    size_t num_resources;

    // Array of all resources for this channel, shared or not. Virtual mappings
    // for matching physical resources are shared across all channels in the
    // same GPU VA space and TSG. Each channel will retain the mappings (VA
    // ranges) it uses at channel register and will release them at
    // uvm_user_channel_destroy_detached, so these physical resources outlive
    // the corresponding VA ranges.
    UvmGpuChannelResourceInfo *resources;

    // Array of all VA ranges associated with this channel. Entry i in this
    // array corresponds to resource i in the resources array above and has the
    // same descriptor. uvm_user_channel_detach will drop the ref counts for
    // these VA ranges, potentially destroying them.
    uvm_va_range_t **va_ranges;

    // Physical instance pointer. There is a 1:1 mapping between instance
    // pointer and channel. GPU faults report an instance pointer, and the GPU
    // fault handler converts this instance pointer into the parent
    // uvm_va_space_t.
    struct
    {
        // Physical address of the instance pointer.
        uvm_gpu_phys_address_t addr;

        // Node for inserting the user channel in the parent GPU instance
        // pointer table. The node will be initialized as an empty UVM RB node
        // on user channel creation and will transition to not empty when
        // instance_ptr -> user_channel translation has been added
        // to the per-GPU UVM RB tree
        uvm_rb_tree_node_t node;
    } instance_ptr;

    // Opaque object which refers to this channel in the nvUvmInterface APIs
    void *rm_retained_channel;

    // Hardware runlist and channel IDs, used for debugging and fault processing
    NvU32 hw_runlist_id;
    NvU32 hw_channel_id;

    // Node in the owning gpu_va_space's registered_channels list. Cleared once
    // the channel is detached.
    struct list_head list_node;

    // Boolean which is set during the window between
    // nvUvmInterfaceBindChannelResources and nvUvmInterfaceStopChannel. This is
    // an atomic_t because multiple threads may call nvUvmInterfaceStopChannel
    // and clear this concurrently.
    atomic_t is_bound;

    // Node for the deferred free list where this channel is stored upon being
    // detached.
    uvm_deferred_free_object_t deferred_free;

    // Reference count for this user channel. This only protects the memory
    // object itself, for use in cases when user channel needs to be accessed
    // across dropping and re-acquiring the VA space lock.
    nv_kref_t kref;

    struct
    {
        bool scheduled;

        nv_kthread_q_item_t kill_channel_q_item;

        uvm_va_space_t *va_space;

        char fault_packet[UVM_GPU_MMU_MAX_FAULT_PACKET_SIZE];
    } kill_channel;
};

// Retains the user channel memory object. uvm_user_channel_destroy_detached and
// uvm_user_channel_release drop the count. This is used to keep the
// user channel object allocated when dropping and re-taking the VA space lock.
// If another thread called uvm_user_channel_detach in the meantime,
// user_channel->gpu_va_space will be NULL.
static inline void uvm_user_channel_retain(uvm_user_channel_t *user_channel)
{
    nv_kref_get(&user_channel->kref);
}

// This only frees the user channel object itself, so the user channel must have
// been detached and destroyed prior to the final release.
void uvm_user_channel_release(uvm_user_channel_t *user_channel);

// User-facing APIs (uvm_api_register_channel, uvm_api_unregister_channel) are
// declared in uvm_api.h.

// First phase of user channel destroy which stops a user channel, forcibly if
// necessary. After calling this function no new GPU faults targeting this
// channel will arrive, but old faults may continue to be serviced.
//
// LOCKING: The owning VA space must be locked in read mode, not write mode.
void uvm_user_channel_stop(uvm_user_channel_t *user_channel);

// Second phase of user channel destroy which detaches the channel from the
// parent gpu_va_space and adds it to the list of pending objects to be freed.
// uvm_user_channel_stop must have already been called on this channel.
//
// All virtual mappings associated with the channel are torn down. The
// user_channel object and the instance pointer and resources it contains are
// not destroyed. The caller must use uvm_user_channel_destroy_detached to do
// that.
//
// This multi-phase approach allows the caller to drop the VA space lock and
// flush the fault buffer before removing the instance pointer. See
// uvm_gpu_destroy_detached_channels.
//
// LOCKING: The owning VA space must be locked in write mode.
void uvm_user_channel_detach(uvm_user_channel_t *user_channel, struct list_head *deferred_free_list);

// Third phase of user channel destroy which frees the user_channel object and
// releases the corresponding resources and instance pointer. The channel must
// have been detached first.
//
// LOCKING: No lock is required, but the owning GPU must be retained.
void uvm_user_channel_destroy_detached(uvm_user_channel_t *user_channel);

#endif // __UVM_USER_CHANNEL_H__
