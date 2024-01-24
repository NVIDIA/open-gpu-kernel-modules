/*******************************************************************************
    Copyright (c) 2015-2022 NVIDIA Corporation

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

#include "uvm_api.h"
#include "uvm_pushbuffer.h"
#include "uvm_channel.h"
#include "uvm_global.h"
#include "uvm_lock.h"
#include "uvm_procfs.h"
#include "uvm_push.h"
#include "uvm_kvmalloc.h"
#include "uvm_gpu.h"
#include "uvm_common.h"
#include "uvm_linux.h"
#include "uvm_conf_computing.h"

// Print pushbuffer state into a seq_file if provided or with UVM_DBG_PRINT() if not.
static void uvm_pushbuffer_print_common(uvm_pushbuffer_t *pushbuffer, struct seq_file *s);

static int nv_procfs_read_pushbuffer_info(struct seq_file *s, void *v)
{
    uvm_pushbuffer_t *pushbuffer = (uvm_pushbuffer_t *)s->private;

    if (!uvm_down_read_trylock(&g_uvm_global.pm.lock))
            return -EAGAIN;

    uvm_pushbuffer_print_common(pushbuffer, s);

    uvm_up_read(&g_uvm_global.pm.lock);

    return 0;
}

static int nv_procfs_read_pushbuffer_info_entry(struct seq_file *s, void *v)
{
    UVM_ENTRY_RET(nv_procfs_read_pushbuffer_info(s, v));
}

UVM_DEFINE_SINGLE_PROCFS_FILE(pushbuffer_info_entry);

static NV_STATUS create_procfs(uvm_pushbuffer_t *pushbuffer)
{
    uvm_gpu_t *gpu = pushbuffer->channel_manager->gpu;

    // The pushbuffer info file is for debug only
    if (!uvm_procfs_is_debug_enabled())
        return NV_OK;

    pushbuffer->procfs.info_file = NV_CREATE_PROC_FILE("pushbuffer",
                                                       gpu->procfs.dir,
                                                       pushbuffer_info_entry,
                                                       pushbuffer);
    if (pushbuffer->procfs.info_file == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    return NV_OK;
}

NV_STATUS uvm_pushbuffer_create(uvm_channel_manager_t *channel_manager, uvm_pushbuffer_t **pushbuffer_out)
{
    NV_STATUS status;
    int i;
    uvm_gpu_t *gpu = channel_manager->gpu;
    NvU64 pushbuffer_alignment;

    uvm_pushbuffer_t *pushbuffer = uvm_kvmalloc_zero(sizeof(*pushbuffer));
    if (pushbuffer == NULL)
        return NV_ERR_NO_MEMORY;

    pushbuffer->channel_manager = channel_manager;

    uvm_spin_lock_init(&pushbuffer->lock, UVM_LOCK_ORDER_LEAF);

    // Currently the pushbuffer supports UVM_PUSHBUFFER_CHUNKS of concurrent
    // pushes.
    uvm_sema_init(&pushbuffer->concurrent_pushes_sema, UVM_PUSHBUFFER_CHUNKS, UVM_LOCK_ORDER_PUSH);

    UVM_ASSERT(channel_manager->conf.pushbuffer_loc == UVM_BUFFER_LOCATION_SYS ||
               channel_manager->conf.pushbuffer_loc == UVM_BUFFER_LOCATION_VID);

    // The pushbuffer allocation is aligned to UVM_PUSHBUFFER_SIZE and its size
    // (UVM_PUSHBUFFER_SIZE) is a power of 2. These constraints guarantee that
    // the entire pushbuffer belongs to a 1TB (2^40) segment. Thus, we can set
    // the Esched/PBDMA segment base for all channels during their
    // initialization and it is immutable for the entire channels' lifetime.
    BUILD_BUG_ON_NOT_POWER_OF_2(UVM_PUSHBUFFER_SIZE);
    BUILD_BUG_ON(UVM_PUSHBUFFER_SIZE >= (1ull << 40));

    if (gpu->uvm_test_force_upper_pushbuffer_segment)
        pushbuffer_alignment = (1ull << 40);
    else
        pushbuffer_alignment = UVM_PUSHBUFFER_SIZE;

    status = uvm_rm_mem_alloc_and_map_cpu(gpu,
                                          (channel_manager->conf.pushbuffer_loc == UVM_BUFFER_LOCATION_SYS) ?
                                              UVM_RM_MEM_TYPE_SYS:
                                              UVM_RM_MEM_TYPE_GPU,
                                          UVM_PUSHBUFFER_SIZE,
                                          pushbuffer_alignment,
                                          &pushbuffer->memory);
    if (status != NV_OK)
        goto error;

    if (g_uvm_global.conf_computing_enabled) {
        UVM_ASSERT(channel_manager->conf.pushbuffer_loc == UVM_BUFFER_LOCATION_SYS);

        // Move the above allocation to unprotected_sysmem
        pushbuffer->memory_unprotected_sysmem = pushbuffer->memory;
        pushbuffer->memory = NULL;

        // Make sure the base can be least 4KB aligned. Pushes can include inline buffers
        // with specific alignment requirement. Different base between backing memory
        // locations would change that.
        pushbuffer->memory_protected_sysmem = uvm_kvmalloc_zero(UVM_PUSHBUFFER_SIZE + UVM_PAGE_SIZE_4K);
        if (!pushbuffer->memory_protected_sysmem) {
            status = NV_ERR_NO_MEMORY;
            goto error;
        }

        status = uvm_rm_mem_alloc(gpu,
                                  UVM_RM_MEM_TYPE_GPU,
                                  UVM_PUSHBUFFER_SIZE,
                                  pushbuffer_alignment,
                                  &pushbuffer->memory);
        if (status != NV_OK)
            goto error;

        status = uvm_rm_mem_map_gpu(pushbuffer->memory_unprotected_sysmem, gpu, pushbuffer_alignment);
        if (status != NV_OK)
            goto error;
    }

    // Verify the GPU can access the pushbuffer.
    UVM_ASSERT((uvm_pushbuffer_get_gpu_va_base(pushbuffer) + UVM_PUSHBUFFER_SIZE - 1) < gpu->parent->max_host_va);

    bitmap_fill(pushbuffer->idle_chunks, UVM_PUSHBUFFER_CHUNKS);
    bitmap_fill(pushbuffer->available_chunks, UVM_PUSHBUFFER_CHUNKS);

    for (i = 0; i < UVM_PUSHBUFFER_CHUNKS; ++i)
        INIT_LIST_HEAD(&pushbuffer->chunks[i].pending_gpfifos);

    status = create_procfs(pushbuffer);
    if (status != NV_OK)
        goto error;

    *pushbuffer_out = pushbuffer;

    return status;

error:
    uvm_pushbuffer_destroy(pushbuffer);
    return status;
}

static uvm_pushbuffer_chunk_t *get_chunk_in_mask(uvm_pushbuffer_t *pushbuffer, unsigned long *mask)
{
    NvU32 index = find_first_bit(mask, UVM_PUSHBUFFER_CHUNKS);

    uvm_assert_spinlock_locked(&pushbuffer->lock);

    if (index == UVM_PUSHBUFFER_CHUNKS)
        return NULL;

    return &pushbuffer->chunks[index];
}

static uvm_pushbuffer_chunk_t *get_available_chunk(uvm_pushbuffer_t *pushbuffer)
{
    return get_chunk_in_mask(pushbuffer, pushbuffer->available_chunks);
}

static uvm_pushbuffer_chunk_t *get_idle_chunk(uvm_pushbuffer_t *pushbuffer)
{
    return get_chunk_in_mask(pushbuffer, pushbuffer->idle_chunks);
}

static NvU32 chunk_get_index(uvm_pushbuffer_t *pushbuffer, uvm_pushbuffer_chunk_t *chunk)
{
    NvU32 index = chunk - pushbuffer->chunks;
    UVM_ASSERT(index < UVM_PUSHBUFFER_CHUNKS);
    return index;
}

static NvU32 chunk_get_offset(uvm_pushbuffer_t *pushbuffer, uvm_pushbuffer_chunk_t *chunk)
{
    return chunk_get_index(pushbuffer, chunk) * UVM_PUSHBUFFER_CHUNK_SIZE;
}

static void set_chunk(uvm_pushbuffer_t *pushbuffer, uvm_pushbuffer_chunk_t *chunk, unsigned long *mask)
{
    NvU32 index = chunk_get_index(pushbuffer, chunk);

    uvm_assert_spinlock_locked(&pushbuffer->lock);

    __set_bit(index, mask);
}

static void clear_chunk(uvm_pushbuffer_t *pushbuffer, uvm_pushbuffer_chunk_t *chunk, unsigned long *mask)
{
    NvU32 index = chunk_get_index(pushbuffer, chunk);

    uvm_assert_spinlock_locked(&pushbuffer->lock);

    __clear_bit(index, mask);
}

static uvm_pushbuffer_chunk_t *pick_chunk(uvm_pushbuffer_t *pushbuffer)
{
    uvm_pushbuffer_chunk_t *chunk = get_idle_chunk(pushbuffer);

    uvm_assert_spinlock_locked(&pushbuffer->lock);

    if (chunk == NULL)
        chunk = get_available_chunk(pushbuffer);

    return chunk;
}

static bool try_claim_chunk(uvm_pushbuffer_t *pushbuffer, uvm_push_t *push, uvm_pushbuffer_chunk_t **chunk_out)
{
    uvm_pushbuffer_chunk_t *chunk;

    uvm_spin_lock(&pushbuffer->lock);

    chunk = pick_chunk(pushbuffer);
    if (!chunk)
        goto done;

    chunk->current_push = push;
    clear_chunk(pushbuffer, chunk, pushbuffer->idle_chunks);
    clear_chunk(pushbuffer, chunk, pushbuffer->available_chunks);

done:
    uvm_spin_unlock(&pushbuffer->lock);
    *chunk_out = chunk;

    return chunk != NULL;
}

static char *get_base_cpu_va(uvm_pushbuffer_t *pushbuffer)
{
    // Confidential Computing pushes are assembled in protected sysmem
    // and safely (through encrypt/decrypt) moved to protected vidmem.
    // Or signed and moved to unprotected sysmem.
    //
    // The protected sysmem base is aligned to 4kB. This is enough to give
    // the same alignment behaviour for inline buffers as the other two
    // backing memory locations.
    if (g_uvm_global.conf_computing_enabled)
        return (char*)(UVM_ALIGN_UP((uintptr_t)pushbuffer->memory_protected_sysmem, UVM_PAGE_SIZE_4K));

    return (char *)uvm_rm_mem_get_cpu_va(pushbuffer->memory);
}

static NvU32 *chunk_get_next_push_start_addr(uvm_pushbuffer_t *pushbuffer, uvm_pushbuffer_chunk_t *chunk)
{
    char *push_start = get_base_cpu_va(pushbuffer);
    push_start += chunk_get_offset(pushbuffer, chunk);
    push_start += chunk->next_push_start;

    UVM_ASSERT(((NvU64)push_start) % sizeof(NvU32) == 0);

    return (NvU32*)push_start;
}

static NV_STATUS claim_chunk(uvm_pushbuffer_t *pushbuffer, uvm_push_t *push, uvm_pushbuffer_chunk_t **chunk_out)
{
    NV_STATUS status = NV_OK;
    uvm_channel_manager_t *channel_manager = pushbuffer->channel_manager;
    uvm_spin_loop_t spin;

    if (try_claim_chunk(pushbuffer, push, chunk_out))
        return NV_OK;

    uvm_channel_manager_update_progress(channel_manager);

    uvm_spin_loop_init(&spin);
    while (!try_claim_chunk(pushbuffer, push, chunk_out) && status == NV_OK) {
        UVM_SPIN_LOOP(&spin);
        status = uvm_channel_manager_check_errors(channel_manager);
        uvm_channel_manager_update_progress(channel_manager);
    }

    return status;
}

NV_STATUS uvm_pushbuffer_begin_push(uvm_pushbuffer_t *pushbuffer, uvm_push_t *push)
{
    uvm_pushbuffer_chunk_t *chunk;
    NV_STATUS status;

    UVM_ASSERT(pushbuffer);
    UVM_ASSERT(push);
    UVM_ASSERT(push->channel);

    if (uvm_channel_is_wlc(push->channel)) {
        // WLC pushes use static PB and don't count against max concurrent
        // pushes.
        push->begin = (void*)UVM_ALIGN_UP((uintptr_t)push->channel->conf_computing.static_pb_protected_sysmem,
                                          UVM_PAGE_SIZE_4K);
        push->next = push->begin;
        return NV_OK;
    }

    // Note that this semaphore is uvm_up()ed in end_push().
    uvm_down(&pushbuffer->concurrent_pushes_sema);

    status = claim_chunk(pushbuffer, push, &chunk);
    if (status != NV_OK) {
        uvm_up(&pushbuffer->concurrent_pushes_sema);
        return status;
    }

    UVM_ASSERT(chunk);

    push->begin = chunk_get_next_push_start_addr(pushbuffer, chunk);
    push->next = push->begin;

    return NV_OK;
}

static uvm_gpfifo_entry_t *chunk_get_first_gpfifo(uvm_pushbuffer_chunk_t *chunk)
{
    return list_first_entry_or_null(&chunk->pending_gpfifos, uvm_gpfifo_entry_t, pending_list_node);
}

static uvm_gpfifo_entry_t *chunk_get_last_gpfifo(uvm_pushbuffer_chunk_t *chunk)
{
    return list_last_entry_or_null(&chunk->pending_gpfifos, uvm_gpfifo_entry_t, pending_list_node);
}

// Get the cpu put within the chunk (in range [0, UVM_PUSHBUFFER_CHUNK_SIZE])
static NvU32 chunk_get_cpu_put(uvm_pushbuffer_t *pushbuffer, uvm_pushbuffer_chunk_t *chunk)
{
    uvm_gpfifo_entry_t *gpfifo = chunk_get_last_gpfifo(chunk);

    uvm_assert_spinlock_locked(&pushbuffer->lock);

    if (gpfifo != NULL)
        return gpfifo->pushbuffer_offset + gpfifo->pushbuffer_size - chunk_get_offset(pushbuffer, chunk);
    else
        return 0;
}

// Get the gpu get within the chunk (in range [0, UVM_PUSHBUFFER_CHUNK_SIZE))
static NvU32 chunk_get_gpu_get(uvm_pushbuffer_t *pushbuffer, uvm_pushbuffer_chunk_t *chunk)
{
    uvm_gpfifo_entry_t *gpfifo = chunk_get_first_gpfifo(chunk);

    uvm_assert_spinlock_locked(&pushbuffer->lock);

    if (gpfifo != NULL)
        return gpfifo->pushbuffer_offset - chunk_get_offset(pushbuffer, chunk);
    else
        return 0;
}

static void update_chunk(uvm_pushbuffer_t *pushbuffer, uvm_pushbuffer_chunk_t *chunk)
{
    NvU32 gpu_get = chunk_get_gpu_get(pushbuffer, chunk);
    NvU32 cpu_put = chunk_get_cpu_put(pushbuffer, chunk);

    uvm_assert_spinlock_locked(&pushbuffer->lock);

    if (gpu_get == cpu_put) {
        // cpu_put can be equal to gpu_get both when the chunk is full and empty. We
        // can tell apart the cases by checking whether the pending GPFIFOs list is
        // empty.
        if (!list_empty(&chunk->pending_gpfifos))
            return;

        // Chunk completely idle
        set_chunk(pushbuffer, chunk, pushbuffer->idle_chunks);
        set_chunk(pushbuffer, chunk, pushbuffer->available_chunks);
        UVM_ASSERT_MSG(cpu_put == 0, "cpu put %u\n", cpu_put);

        // For a completely idle chunk, always start at the very beginning. This
        // helps avoid the waste that can happen at the very end of the chunk
        // described at the top of uvm_pushbuffer.h.
        chunk->next_push_start = 0;
    }
    else if (gpu_get > cpu_put) {
        if (gpu_get - cpu_put >= UVM_MAX_PUSH_SIZE) {
            // Enough space between put and get
            set_chunk(pushbuffer, chunk, pushbuffer->available_chunks);
            chunk->next_push_start = cpu_put;
        }
    }
    else if (UVM_PUSHBUFFER_CHUNK_SIZE >= cpu_put + UVM_MAX_PUSH_SIZE) {
        UVM_ASSERT_MSG(gpu_get < cpu_put, "gpu_get %u cpu_put %u\n", gpu_get, cpu_put);

        // Enough space at the end
        set_chunk(pushbuffer, chunk, pushbuffer->available_chunks);
        chunk->next_push_start = cpu_put;
    }
    else if (gpu_get >= UVM_MAX_PUSH_SIZE) {
        UVM_ASSERT_MSG(gpu_get < cpu_put, "gpu_get %u cpu_put %u\n", gpu_get, cpu_put);

        // Enough space at the beginning
        set_chunk(pushbuffer, chunk, pushbuffer->available_chunks);
        chunk->next_push_start = 0;
    }
}

void uvm_pushbuffer_destroy(uvm_pushbuffer_t *pushbuffer)
{
    if (pushbuffer == NULL)
        return;

    proc_remove(pushbuffer->procfs.info_file);

    uvm_rm_mem_free(pushbuffer->memory_unprotected_sysmem);
    uvm_kvfree(pushbuffer->memory_protected_sysmem);
    uvm_rm_mem_free(pushbuffer->memory);
    uvm_kvfree(pushbuffer);
}

static uvm_pushbuffer_chunk_t *offset_to_chunk(uvm_pushbuffer_t *pushbuffer, NvU32 offset)
{
    UVM_ASSERT(offset < UVM_PUSHBUFFER_SIZE);
    return &pushbuffer->chunks[offset / UVM_PUSHBUFFER_CHUNK_SIZE];
}

static uvm_pushbuffer_chunk_t *gpfifo_to_chunk(uvm_pushbuffer_t *pushbuffer, uvm_gpfifo_entry_t *gpfifo)
{
    uvm_pushbuffer_chunk_t *chunk = offset_to_chunk(pushbuffer, gpfifo->pushbuffer_offset);
    UVM_ASSERT(offset_to_chunk(pushbuffer, gpfifo->pushbuffer_offset + gpfifo->pushbuffer_size - 1) == chunk);
    return chunk;
}

static void decrypt_push(uvm_channel_t *channel, uvm_gpfifo_entry_t *gpfifo)
{
    NV_STATUS status;
    NvU32 auth_tag_offset;
    void *auth_tag_cpu_va;
    void *push_protected_cpu_va;
    void *push_unprotected_cpu_va;
    NvU32 pushbuffer_offset = gpfifo->pushbuffer_offset;
    NvU32 push_info_index = gpfifo->push_info - channel->push_infos;
    uvm_pushbuffer_t *pushbuffer = uvm_channel_get_pushbuffer(channel);
    uvm_push_crypto_bundle_t *crypto_bundle = channel->conf_computing.push_crypto_bundles + push_info_index;

    if (channel->conf_computing.push_crypto_bundles == NULL)
        return;

    // When the crypto bundle is used, the push size cannot be zero
    if (crypto_bundle->push_size == 0)
        return;

    UVM_ASSERT(!uvm_channel_is_wlc(channel));
    UVM_ASSERT(!uvm_channel_is_lcic(channel));

    push_protected_cpu_va = (char *)get_base_cpu_va(pushbuffer) + pushbuffer_offset;
    push_unprotected_cpu_va = (char *)uvm_rm_mem_get_cpu_va(pushbuffer->memory_unprotected_sysmem) + pushbuffer_offset;
    auth_tag_offset = push_info_index * UVM_CONF_COMPUTING_AUTH_TAG_SIZE;
    auth_tag_cpu_va = (char *)uvm_rm_mem_get_cpu_va(channel->conf_computing.push_crypto_bundle_auth_tags) +
                              auth_tag_offset;

    status = uvm_conf_computing_cpu_decrypt(channel,
                                            push_protected_cpu_va,
                                            push_unprotected_cpu_va,
                                            &crypto_bundle->iv,
                                            crypto_bundle->push_size,
                                            auth_tag_cpu_va);

    // A decryption failure here is not fatal because it does not
    // prevent UVM from running fine in the future and cannot be used
    // maliciously to leak information or otherwise derail UVM from its
    // regular duties.
    UVM_ASSERT_MSG_RELEASE(status == NV_OK, "Pushbuffer decryption failure: %s\n", nvstatusToString(status));

    // Avoid reusing the bundle across multiple pushes
    crypto_bundle->push_size = 0;
}

void uvm_pushbuffer_mark_completed(uvm_channel_t *channel, uvm_gpfifo_entry_t *gpfifo)
{
    uvm_pushbuffer_chunk_t *chunk;
    bool need_to_update_chunk = false;
    uvm_push_info_t *push_info = gpfifo->push_info;
    uvm_pushbuffer_t *pushbuffer = uvm_channel_get_pushbuffer(channel);

    UVM_ASSERT(gpfifo->type == UVM_GPFIFO_ENTRY_TYPE_NORMAL);

    chunk = gpfifo_to_chunk(pushbuffer, gpfifo);

    if (push_info->on_complete != NULL) {
        decrypt_push(channel, gpfifo);
        push_info->on_complete(push_info->on_complete_data);
        push_info->on_complete = NULL;
        push_info->on_complete_data = NULL;
    }

    uvm_spin_lock(&pushbuffer->lock);

    if (gpfifo == chunk_get_first_gpfifo(chunk))
        need_to_update_chunk = true;
    else if (gpfifo == chunk_get_last_gpfifo(chunk))
        need_to_update_chunk = true;

    list_del(&gpfifo->pending_list_node);

    // If current_push is not NULL, updating the chunk is delayed till
    // uvm_pushbuffer_end_push() is called for that push.
    if (need_to_update_chunk && chunk->current_push == NULL)
        update_chunk(pushbuffer, chunk);

    uvm_spin_unlock(&pushbuffer->lock);
}

NvU32 uvm_pushbuffer_get_offset_for_push(uvm_pushbuffer_t *pushbuffer, uvm_push_t *push)
{
    NvU32 offset;

    if (uvm_channel_is_wlc(push->channel)) {
        // WLC channels use private static PB and their gpfifo entries are not
        // added to any chunk's list. This only needs to return legal offset.
        // Completion cleanup will not find WLC gpfifo entries as either first
        // or last entry of any chunk.
        return 0;
    }

    offset = (char*)push->begin - get_base_cpu_va(pushbuffer);

    UVM_ASSERT(((NvU64)offset) % sizeof(NvU32) == 0);

    return offset;
}

NvU64 uvm_pushbuffer_get_gpu_va_for_push(uvm_pushbuffer_t *pushbuffer, uvm_push_t *push)
{
    NvU64 pushbuffer_base;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    bool is_proxy_channel = uvm_channel_is_proxy(push->channel);

    pushbuffer_base = uvm_rm_mem_get_gpu_va(pushbuffer->memory, gpu, is_proxy_channel).address;

    if (uvm_channel_is_wlc(push->channel) || uvm_channel_is_lcic(push->channel)) {
        // We need to use the same static locations for PB as the fixed
        // schedule because that's what the channels are initialized to use.
        return uvm_rm_mem_get_gpu_uvm_va(push->channel->conf_computing.static_pb_protected_vidmem, gpu);
    }
    else if (uvm_channel_is_sec2(push->channel)) {
        // SEC2 PBs are in unprotected sysmem
        pushbuffer_base = uvm_pushbuffer_get_sec2_gpu_va_base(pushbuffer);
    }

    return pushbuffer_base + uvm_pushbuffer_get_offset_for_push(pushbuffer, push);
}

void *uvm_pushbuffer_get_unprotected_cpu_va_for_push(uvm_pushbuffer_t *pushbuffer, uvm_push_t *push)
{
    char *pushbuffer_base;

    if (uvm_channel_is_wlc(push->channel)) {
        // Reuse existing WLC static pb for initialization
        UVM_ASSERT(!uvm_channel_manager_is_wlc_ready(push->channel->pool->manager));
        return push->channel->conf_computing.static_pb_unprotected_sysmem_cpu;
    }

    pushbuffer_base = uvm_rm_mem_get_cpu_va(pushbuffer->memory_unprotected_sysmem);

    return pushbuffer_base + uvm_pushbuffer_get_offset_for_push(pushbuffer, push);
}

NvU64 uvm_pushbuffer_get_unprotected_gpu_va_for_push(uvm_pushbuffer_t *pushbuffer, uvm_push_t *push)
{
    NvU64 pushbuffer_base;

    if (uvm_channel_is_wlc(push->channel)) {
        // Reuse existing WLC static pb for initialization
        UVM_ASSERT(!uvm_channel_manager_is_wlc_ready(push->channel->pool->manager));
        return uvm_rm_mem_get_gpu_uvm_va(push->channel->conf_computing.static_pb_unprotected_sysmem,
                                         uvm_push_get_gpu(push));
    }

    pushbuffer_base = uvm_rm_mem_get_gpu_uvm_va(pushbuffer->memory_unprotected_sysmem, uvm_push_get_gpu(push));

    return pushbuffer_base + uvm_pushbuffer_get_offset_for_push(pushbuffer, push);
}

void uvm_pushbuffer_end_push(uvm_pushbuffer_t *pushbuffer, uvm_push_t *push, uvm_gpfifo_entry_t *gpfifo)
{
    uvm_pushbuffer_chunk_t *chunk;

    if (uvm_channel_is_wlc(push->channel)) {
        // WLC channels use static pushbuffer and don't count towards max
        // concurrent pushes. Initializing the list as head makes sure the
        // deletion in "uvm_pushbuffer_mark_completed" doesn't crash.
        INIT_LIST_HEAD(&gpfifo->pending_list_node);
        return;
    }

    chunk = gpfifo_to_chunk(pushbuffer, gpfifo);

    uvm_channel_pool_assert_locked(push->channel->pool);

    uvm_spin_lock(&pushbuffer->lock);

    list_add_tail(&gpfifo->pending_list_node, &chunk->pending_gpfifos);

    update_chunk(pushbuffer, chunk);

    UVM_ASSERT(chunk->current_push == push);
    chunk->current_push = NULL;

    uvm_spin_unlock(&pushbuffer->lock);

    // uvm_pushbuffer_end_push() needs to be called with the channel lock held
    // while the concurrent pushes sema has a higher lock order. To keep the
    // code structure simple, just up out of order here.
    uvm_up_out_of_order(&pushbuffer->concurrent_pushes_sema);
}

bool uvm_pushbuffer_has_space(uvm_pushbuffer_t *pushbuffer)
{
    bool has_space;

    uvm_spin_lock(&pushbuffer->lock);

    has_space = pick_chunk(pushbuffer) != NULL;

    uvm_spin_unlock(&pushbuffer->lock);

    return has_space;
}

void uvm_pushbuffer_print_common(uvm_pushbuffer_t *pushbuffer, struct seq_file *s)
{
    NvU32 i;

    UVM_SEQ_OR_DBG_PRINT(s, "Pushbuffer for GPU %s\n", uvm_gpu_name(pushbuffer->channel_manager->gpu));
    UVM_SEQ_OR_DBG_PRINT(s, " has space: %d\n", uvm_pushbuffer_has_space(pushbuffer));

    uvm_spin_lock(&pushbuffer->lock);

    for (i = 0; i < UVM_PUSHBUFFER_CHUNKS; ++i) {
        uvm_pushbuffer_chunk_t *chunk = &pushbuffer->chunks[i];
        NvU32 cpu_put = chunk_get_cpu_put(pushbuffer, chunk);
        NvU32 gpu_get = chunk_get_gpu_get(pushbuffer, chunk);
        UVM_SEQ_OR_DBG_PRINT(s, " chunk %u put %u get %u next %u available %d idle %d\n",
                i,
                cpu_put, gpu_get, chunk->next_push_start,
                test_bit(i, pushbuffer->available_chunks) ? 1 : 0,
                test_bit(i, pushbuffer->idle_chunks) ? 1 : 0);

    }

    uvm_spin_unlock(&pushbuffer->lock);
}

void uvm_pushbuffer_print(uvm_pushbuffer_t *pushbuffer)
{
    return uvm_pushbuffer_print_common(pushbuffer, NULL);
}

NvU64 uvm_pushbuffer_get_gpu_va_base(uvm_pushbuffer_t *pushbuffer)
{
    return uvm_rm_mem_get_gpu_uvm_va(pushbuffer->memory, pushbuffer->channel_manager->gpu);
}

NvU64 uvm_pushbuffer_get_sec2_gpu_va_base(uvm_pushbuffer_t *pushbuffer)
{
    UVM_ASSERT(g_uvm_global.conf_computing_enabled);

    return uvm_rm_mem_get_gpu_uvm_va(pushbuffer->memory_unprotected_sysmem, pushbuffer->channel_manager->gpu);
}
