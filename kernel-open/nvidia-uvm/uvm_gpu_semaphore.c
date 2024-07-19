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

#include "uvm_gpu_semaphore.h"
#include "uvm_lock.h"
#include "uvm_global.h"
#include "uvm_kvmalloc.h"
#include "uvm_channel.h" // For UVM_GPU_SEMAPHORE_MAX_JUMP
#include "uvm_conf_computing.h"

#define UVM_SEMAPHORE_SIZE 4
#define UVM_SEMAPHORE_PAGE_SIZE PAGE_SIZE
#define UVM_SEMAPHORE_COUNT_PER_PAGE (PAGE_SIZE / UVM_SEMAPHORE_SIZE)

// The top nibble of the canary base is intentionally 0. The rest of the value
// is arbitrary. See the comments below on make_canary.
#define UVM_SEMAPHORE_CANARY_BASE     0x0badc0de
#define UVM_SEMAPHORE_CANARY_MASK     0xf0000000

struct uvm_gpu_semaphore_pool_struct
{
    // The GPU owning the pool
    uvm_gpu_t *gpu;

    // List of all the semaphore pages belonging to the pool
    struct list_head pages;

    // Pages aperture.
    uvm_aperture_t aperture;

    // Count of free semaphores among all the pages
    NvU32 free_semaphores_count;

    // Lock protecting the state of the pool
    uvm_mutex_t mutex;
};

struct uvm_gpu_semaphore_pool_page_struct
{
    // Allocation backing the page
    uvm_rm_mem_t *memory;

    struct {
        // Unprotected sysmem storing encrypted value of semaphores
        uvm_rm_mem_t *encrypted_payload_memory;

        // Unprotected sysmem storing encryption auth tags
        uvm_rm_mem_t *auth_tag_memory;

        // Unprotected sysmem storing plain text notifier values
        uvm_rm_mem_t *notifier_memory;
    } conf_computing;

    // Pool the page is part of
    uvm_gpu_semaphore_pool_t *pool;

    // Node in the list of all pages in a semaphore pool
    struct list_head all_pages_node;

    // Mask indicating free semaphore indices within the page
    DECLARE_BITMAP(free_semaphores, UVM_SEMAPHORE_COUNT_PER_PAGE);
};

static bool gpu_semaphore_pool_is_secure(uvm_gpu_semaphore_pool_t *pool)
{
    return g_uvm_global.conf_computing_enabled && (pool->aperture == UVM_APERTURE_VID);
}

static bool gpu_semaphore_is_secure(uvm_gpu_semaphore_t *semaphore)
{
    return gpu_semaphore_pool_is_secure(semaphore->page->pool);
}

// Use canary values on debug builds to catch semaphore use-after-free. We can
// catch release-after-free by simply setting the payload to a known value at
// free then checking it on alloc or pool free, but catching acquire-after-free
// is a little trickier.
//
// In order to make still-pending GEQ acquires stall indefinitely we need to
// reduce the current payload as much as we can, subject to two restrictions:
//
// 1) The pending acquires could be comparing against values much less than and
//    much greater than the current payload, so we have to set the payload to a
//    value reasonably less than the acquires which we might expect to be
//    pending.
//
// 2) Going over halfway past a pending acquire on the 32-bit number wheel will
//    cause Host to wrap and think the acquire succeeded. So we shouldn't reduce
//    by more than 2^31.
//
// To handle these restrictions we'll deal with quadrants of 2^32, under the
// assumption that it's unlikely for a payload to outpace a pending acquire by
// more than 2^30.
//
// We also need for the base value to have some 0s in the upper significant
// bits, otherwise those bits might carry us past the quadrant boundary when we
// OR them in.
static NvU32 make_canary(NvU32 payload)
{
    NvU32 prev_quadrant = payload - (1 << 30);
    return (prev_quadrant & UVM_SEMAPHORE_CANARY_MASK) | UVM_SEMAPHORE_CANARY_BASE;
}

static bool is_canary(NvU32 val)
{
    return (val & ~UVM_SEMAPHORE_CANARY_MASK) == UVM_SEMAPHORE_CANARY_BASE;
}

static bool semaphore_uses_canary(uvm_gpu_semaphore_pool_t *pool)
{
    // A pool allocated in the CPR of vidmem cannot be read/written from the
    // CPU.
    return !gpu_semaphore_pool_is_secure(pool) && UVM_IS_DEBUG();
    return UVM_IS_DEBUG();
}

// Can the GPU access the semaphore, i.e., can Host/Esched address the semaphore
// pool?
static bool gpu_can_access_semaphore_pool(uvm_gpu_t *gpu, uvm_rm_mem_t *rm_mem)
{
    return ((uvm_rm_mem_get_gpu_uvm_va(rm_mem, gpu) + rm_mem->size - 1) < gpu->parent->max_host_va);
}

static void pool_page_free_buffers(uvm_gpu_semaphore_pool_page_t *page)
{
    uvm_rm_mem_free(page->memory);
    page->memory = NULL;

    if (gpu_semaphore_pool_is_secure(page->pool)) {
        uvm_rm_mem_free(page->conf_computing.encrypted_payload_memory);
        uvm_rm_mem_free(page->conf_computing.auth_tag_memory);
        uvm_rm_mem_free(page->conf_computing.notifier_memory);

        page->conf_computing.encrypted_payload_memory = NULL;
        page->conf_computing.auth_tag_memory = NULL;
        page->conf_computing.notifier_memory = NULL;
    }
    else {
        UVM_ASSERT(!page->conf_computing.encrypted_payload_memory);
        UVM_ASSERT(!page->conf_computing.auth_tag_memory);
        UVM_ASSERT(!page->conf_computing.notifier_memory);
    }
}

static NV_STATUS pool_page_alloc_buffers(uvm_gpu_semaphore_pool_page_t *page)
{
    NV_STATUS status;
    uvm_gpu_semaphore_pool_t *pool = page->pool;
    uvm_rm_mem_type_t memory_type = (pool->aperture == UVM_APERTURE_SYS) ? UVM_RM_MEM_TYPE_SYS : UVM_RM_MEM_TYPE_GPU;
    size_t align = 0;
    bool map_all = true;
    align = gpu_semaphore_pool_is_secure(pool) ? UVM_CONF_COMPUTING_BUF_ALIGNMENT : 0;
    map_all = gpu_semaphore_pool_is_secure(pool) ? false : true;

    if (map_all)
        status = uvm_rm_mem_alloc_and_map_all(pool->gpu, memory_type, UVM_SEMAPHORE_PAGE_SIZE, align, &page->memory);
    else
        status = uvm_rm_mem_alloc(pool->gpu, memory_type, UVM_SEMAPHORE_PAGE_SIZE, align, &page->memory);

    if (status != NV_OK)
        goto error;

    if (!gpu_semaphore_pool_is_secure(pool))
        return NV_OK;

    status = uvm_rm_mem_alloc_and_map_cpu(pool->gpu,
                                          UVM_RM_MEM_TYPE_SYS,
                                          UVM_SEMAPHORE_PAGE_SIZE,
                                          UVM_CONF_COMPUTING_BUF_ALIGNMENT,
                                          &page->conf_computing.encrypted_payload_memory);
    if (status != NV_OK)
        goto error;

    BUILD_BUG_ON(UVM_CONF_COMPUTING_AUTH_TAG_SIZE % UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT);
    status = uvm_rm_mem_alloc_and_map_cpu(pool->gpu,
                                          UVM_RM_MEM_TYPE_SYS,
                                          UVM_SEMAPHORE_COUNT_PER_PAGE * UVM_CONF_COMPUTING_AUTH_TAG_SIZE,
                                          UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT,
                                          &page->conf_computing.auth_tag_memory);
    if (status != NV_OK)
        goto error;

    status = uvm_rm_mem_alloc_and_map_cpu(pool->gpu,
                                          UVM_RM_MEM_TYPE_SYS,
                                          UVM_SEMAPHORE_COUNT_PER_PAGE * sizeof(NvU32),
                                          0,
                                          &page->conf_computing.notifier_memory);
    if (status != NV_OK)
        goto error;

    return NV_OK;
error:
    pool_page_free_buffers(page);
    return status;
}

static NV_STATUS pool_alloc_page(uvm_gpu_semaphore_pool_t *pool)
{
    NV_STATUS status;
    uvm_gpu_semaphore_pool_page_t *pool_page;

    uvm_assert_mutex_locked(&pool->mutex);

    pool_page = uvm_kvmalloc_zero(sizeof(*pool_page));

    if (!pool_page)
        return NV_ERR_NO_MEMORY;

    pool_page->pool = pool;

    status = pool_page_alloc_buffers(pool_page);
    if (status != NV_OK)
        goto error;

    // Verify the GPU can access the semaphore pool.
    UVM_ASSERT(gpu_can_access_semaphore_pool(pool->gpu, pool_page->memory));

    // All semaphores are initially free
    bitmap_fill(pool_page->free_semaphores, UVM_SEMAPHORE_COUNT_PER_PAGE);

    list_add(&pool_page->all_pages_node, &pool->pages);
    pool->free_semaphores_count += UVM_SEMAPHORE_COUNT_PER_PAGE;

    if (semaphore_uses_canary(pool)) {
        size_t i;
        NvU32 *payloads = uvm_rm_mem_get_cpu_va(pool_page->memory);

        for (i = 0; i < UVM_SEMAPHORE_COUNT_PER_PAGE; i++)
            payloads[i] = make_canary(0);
    }

    return NV_OK;

error:
    uvm_kvfree(pool_page);
    return status;
}

static void pool_free_page(uvm_gpu_semaphore_pool_page_t *page)
{
    uvm_gpu_semaphore_pool_t *pool;

    UVM_ASSERT(page);
    pool = page->pool;

    uvm_assert_mutex_locked(&pool->mutex);

    // Assert that no semaphores are still allocated
    UVM_ASSERT(bitmap_full(page->free_semaphores, UVM_SEMAPHORE_COUNT_PER_PAGE));
    UVM_ASSERT_MSG(pool->free_semaphores_count >= UVM_SEMAPHORE_COUNT_PER_PAGE,
                   "count: %u\n",
                   pool->free_semaphores_count);

    if (semaphore_uses_canary(pool)) {
        size_t i;
        NvU32 *payloads = uvm_rm_mem_get_cpu_va(page->memory);
        for (i = 0; i < UVM_SEMAPHORE_COUNT_PER_PAGE; i++)
            UVM_ASSERT(is_canary(payloads[i]));
    }

    pool->free_semaphores_count -= UVM_SEMAPHORE_COUNT_PER_PAGE;
    list_del(&page->all_pages_node);
    pool_page_free_buffers(page);
    uvm_kvfree(page);
}

NV_STATUS uvm_gpu_semaphore_alloc(uvm_gpu_semaphore_pool_t *pool, uvm_gpu_semaphore_t *semaphore)
{
    NV_STATUS status = NV_OK;
    uvm_gpu_semaphore_pool_page_t *page;

    memset(semaphore, 0, sizeof(*semaphore));

    uvm_mutex_lock(&pool->mutex);

    if (pool->free_semaphores_count == 0)
        status = pool_alloc_page(pool);

    if (status != NV_OK)
        goto done;

    list_for_each_entry(page, &pool->pages, all_pages_node) {
        const NvU32 semaphore_index = find_first_bit(page->free_semaphores, UVM_SEMAPHORE_COUNT_PER_PAGE);

        UVM_ASSERT(semaphore_index <= UVM_SEMAPHORE_COUNT_PER_PAGE);

        if (semaphore_index == UVM_SEMAPHORE_COUNT_PER_PAGE)
            continue;

        semaphore->page = page;
        semaphore->index = semaphore_index;

        if (gpu_semaphore_pool_is_secure(pool)) {

            // Reset the notifier to prevent detection of false attack when
            // checking for updated value
            *uvm_gpu_semaphore_get_notifier_cpu_va(semaphore) = semaphore->conf_computing.last_observed_notifier;
        }

        if (semaphore_uses_canary(pool))
            UVM_ASSERT(is_canary(uvm_gpu_semaphore_get_payload(semaphore)));

        uvm_gpu_semaphore_set_payload(semaphore, 0);

        __clear_bit(semaphore_index, page->free_semaphores);
        --pool->free_semaphores_count;

        goto done;
    }

    UVM_ASSERT_MSG(0, "Failed to find a semaphore after allocating a new page\n");
    status = NV_ERR_GENERIC;

done:
    uvm_mutex_unlock(&pool->mutex);

    return status;
}

void uvm_gpu_semaphore_free(uvm_gpu_semaphore_t *semaphore)
{
    uvm_gpu_semaphore_pool_page_t *page;
    uvm_gpu_semaphore_pool_t *pool;

    UVM_ASSERT(semaphore);

    // uvm_gpu_semaphore_t is to be embedded in other structures so it should always
    // be accessible, but it may not be initialized in error cases. Early out if
    // page is NULL indicating the semaphore hasn't been allocated successfully.
    page = semaphore->page;
    if (page == NULL)
        return;

    pool = page->pool;

    // Write a known value lower than the current payload in an attempt to catch
    // release-after-free and acquire-after-free.
    if (semaphore_uses_canary(pool))
        uvm_gpu_semaphore_set_payload(semaphore, make_canary(uvm_gpu_semaphore_get_payload(semaphore)));

    uvm_mutex_lock(&pool->mutex);

    semaphore->page = NULL;

    ++pool->free_semaphores_count;
    __set_bit(semaphore->index, page->free_semaphores);

    uvm_mutex_unlock(&pool->mutex);
}

NV_STATUS uvm_gpu_semaphore_pool_create(uvm_gpu_t *gpu, uvm_gpu_semaphore_pool_t **pool_out)
{
    uvm_gpu_semaphore_pool_t *pool;
    pool = uvm_kvmalloc_zero(sizeof(*pool));

    if (!pool)
        return NV_ERR_NO_MEMORY;

    uvm_mutex_init(&pool->mutex, UVM_LOCK_ORDER_GPU_SEMAPHORE_POOL);

    INIT_LIST_HEAD(&pool->pages);

    pool->free_semaphores_count = 0;
    pool->gpu = gpu;
    pool->aperture = UVM_APERTURE_SYS;

    *pool_out = pool;

    return NV_OK;
}

NV_STATUS uvm_gpu_semaphore_secure_pool_create(uvm_gpu_t *gpu, uvm_gpu_semaphore_pool_t **pool_out)
{
    NV_STATUS status;

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);

    status = uvm_gpu_semaphore_pool_create(gpu, pool_out);
    if (status == NV_OK)
        (*pool_out)->aperture = UVM_APERTURE_VID;

    return status;
}

void uvm_gpu_semaphore_pool_destroy(uvm_gpu_semaphore_pool_t *pool)
{
    uvm_gpu_semaphore_pool_page_t *page;
    uvm_gpu_semaphore_pool_page_t *next_page;

    if (!pool)
        return;

    // No other thread should be touching the pool once it's being destroyed
    uvm_assert_mutex_unlocked(&pool->mutex);

    // Keep pool_free_page happy
    uvm_mutex_lock(&pool->mutex);

    list_for_each_entry_safe(page, next_page, &pool->pages, all_pages_node)
        pool_free_page(page);

    UVM_ASSERT_MSG(pool->free_semaphores_count == 0, "unused: %u", pool->free_semaphores_count);
    UVM_ASSERT(list_empty(&pool->pages));

    uvm_mutex_unlock(&pool->mutex);

    uvm_kvfree(pool);
}

NV_STATUS uvm_gpu_semaphore_pool_map_gpu(uvm_gpu_semaphore_pool_t *pool, uvm_gpu_t *gpu)
{
    NV_STATUS status = NV_OK;
    uvm_gpu_semaphore_pool_page_t *page;

    UVM_ASSERT(pool);
    UVM_ASSERT(gpu);

    uvm_mutex_lock(&pool->mutex);

    list_for_each_entry(page, &pool->pages, all_pages_node) {
        status = uvm_rm_mem_map_gpu(page->memory, gpu, 0);
        if (status != NV_OK)
            goto done;
    }

done:
    uvm_mutex_unlock(&pool->mutex);

    return status;
}

void uvm_gpu_semaphore_pool_unmap_gpu(uvm_gpu_semaphore_pool_t *pool, uvm_gpu_t *gpu)
{
    uvm_gpu_semaphore_pool_page_t *page;

    UVM_ASSERT(pool);
    UVM_ASSERT(gpu);

    uvm_mutex_lock(&pool->mutex);

    list_for_each_entry(page, &pool->pages, all_pages_node)
        uvm_rm_mem_unmap_gpu(page->memory, gpu);

    uvm_mutex_unlock(&pool->mutex);
}

NvU64 uvm_gpu_semaphore_get_gpu_uvm_va(uvm_gpu_semaphore_t *semaphore, uvm_gpu_t *gpu)
{
    return uvm_gpu_semaphore_get_gpu_va(semaphore, gpu, false);
}

NvU64 uvm_gpu_semaphore_get_gpu_proxy_va(uvm_gpu_semaphore_t *semaphore, uvm_gpu_t *gpu)
{
    return uvm_gpu_semaphore_get_gpu_va(semaphore, gpu, true);
}

NvU64 uvm_gpu_semaphore_get_gpu_va(uvm_gpu_semaphore_t *semaphore, uvm_gpu_t *gpu, bool is_proxy_va_space)
{
    NvU64 base_va = uvm_rm_mem_get_gpu_va(semaphore->page->memory, gpu, is_proxy_va_space).address;

    return base_va + semaphore->index * UVM_SEMAPHORE_SIZE;
}

NvU32 *uvm_gpu_semaphore_get_cpu_va(uvm_gpu_semaphore_t *semaphore)
{
    char *base_va;

    if (gpu_semaphore_is_secure(semaphore))
        return &semaphore->conf_computing.cached_payload;

    base_va = uvm_rm_mem_get_cpu_va(semaphore->page->memory);
    return (NvU32*)(base_va + semaphore->index * UVM_SEMAPHORE_SIZE);
}

NvU32 *uvm_gpu_semaphore_get_encrypted_payload_cpu_va(uvm_gpu_semaphore_t *semaphore)
{
    char *encrypted_base_va = uvm_rm_mem_get_cpu_va(semaphore->page->conf_computing.encrypted_payload_memory);

    return (NvU32*)(encrypted_base_va + semaphore->index * UVM_SEMAPHORE_SIZE);
}

uvm_gpu_address_t uvm_gpu_semaphore_get_encrypted_payload_gpu_va(uvm_gpu_semaphore_t *semaphore)
{
    NvU64 encrypted_base_va = uvm_rm_mem_get_gpu_uvm_va(semaphore->page->conf_computing.encrypted_payload_memory,
                                                        semaphore->page->pool->gpu);

    return uvm_gpu_address_virtual_unprotected(encrypted_base_va + semaphore->index * UVM_SEMAPHORE_SIZE);
}

NvU32 *uvm_gpu_semaphore_get_notifier_cpu_va(uvm_gpu_semaphore_t *semaphore)
{
    char *notifier_base_va = uvm_rm_mem_get_cpu_va(semaphore->page->conf_computing.notifier_memory);

    return (NvU32*)(notifier_base_va + semaphore->index * sizeof(NvU32));
}

uvm_gpu_address_t uvm_gpu_semaphore_get_notifier_gpu_va(uvm_gpu_semaphore_t *semaphore)
{
    NvU64 notifier_base_va = uvm_rm_mem_get_gpu_uvm_va(semaphore->page->conf_computing.notifier_memory,
                                                       semaphore->page->pool->gpu);

    return uvm_gpu_address_virtual_unprotected(notifier_base_va + semaphore->index * sizeof(NvU32));
}

void *uvm_gpu_semaphore_get_auth_tag_cpu_va(uvm_gpu_semaphore_t *semaphore)
{
    char *auth_tag_base_va = uvm_rm_mem_get_cpu_va(semaphore->page->conf_computing.auth_tag_memory);

    return (void*)(auth_tag_base_va + semaphore->index * UVM_CONF_COMPUTING_AUTH_TAG_SIZE);
}

uvm_gpu_address_t uvm_gpu_semaphore_get_auth_tag_gpu_va(uvm_gpu_semaphore_t *semaphore)
{
    NvU64 auth_tag_base_va = uvm_rm_mem_get_gpu_uvm_va(semaphore->page->conf_computing.auth_tag_memory,
                                                       semaphore->page->pool->gpu);

    return uvm_gpu_address_virtual_unprotected(auth_tag_base_va + semaphore->index * UVM_CONF_COMPUTING_AUTH_TAG_SIZE);
}

NvU32 uvm_gpu_semaphore_get_payload(uvm_gpu_semaphore_t *semaphore)
{
    return UVM_GPU_READ_ONCE(*uvm_gpu_semaphore_get_cpu_va(semaphore));
}

void uvm_gpu_semaphore_set_payload(uvm_gpu_semaphore_t *semaphore, NvU32 payload)
{
    // Provide a guarantee that all memory accesses prior to setting the payload
    // won't be moved past it.
    // Use a big hammer mb() as set_payload() is not used in any performance path
    // today.
    // This could likely be optimized to be either an smp_store_release() or use
    // an smp_mb__before_atomic() barrier. The former is a recent addition to
    // kernel though, and it's not clear whether combining the latter with a
    // regular 32bit store is well defined in all cases. Both also seem to risk
    // being optimized out on non-SMP configs (we need them for interacting with
    // the GPU correctly even on non-SMP).
    mb();

    UVM_GPU_WRITE_ONCE(*uvm_gpu_semaphore_get_cpu_va(semaphore), payload);
}

// This function is intended to catch channels which have been left dangling in
// trackers after their owning GPUs have been destroyed.
static bool tracking_semaphore_check_gpu(uvm_gpu_tracking_semaphore_t *tracking_sem)
{
    uvm_gpu_t *gpu = tracking_sem->semaphore.page->pool->gpu;
    uvm_gpu_t *table_gpu;

    UVM_ASSERT_MSG(gpu->magic == UVM_GPU_MAGIC_VALUE, "Corruption detected: magic number is 0x%llx\n", gpu->magic);

    // It's ok for the GPU to not be in the global table, since add_gpu operates
    // on trackers before adding the GPU to the table, and remove_gpu operates
    // on trackers after removing the GPU. We rely on the magic value to catch
    // those cases.
    //
    // But if a pointer is in the table it must match.
    table_gpu = uvm_gpu_get(gpu->id);
    if (table_gpu)
        UVM_ASSERT(table_gpu == gpu);

    // Return a boolean so this function can be used in assertions for
    // conditional compilation
    return true;
}

static bool tracking_semaphore_uses_mutex(uvm_gpu_tracking_semaphore_t *tracking_semaphore)
{
    UVM_ASSERT(tracking_semaphore_check_gpu(tracking_semaphore));

    return g_uvm_global.conf_computing_enabled;
}

NV_STATUS uvm_gpu_tracking_semaphore_alloc(uvm_gpu_semaphore_pool_t *pool, uvm_gpu_tracking_semaphore_t *tracking_sem)
{
    NV_STATUS status;
    uvm_lock_order_t order = UVM_LOCK_ORDER_LEAF;

    memset(tracking_sem, 0, sizeof(*tracking_sem));

    status = uvm_gpu_semaphore_alloc(pool, &tracking_sem->semaphore);
    if (status != NV_OK)
        return status;

    UVM_ASSERT(uvm_gpu_semaphore_get_payload(&tracking_sem->semaphore) == 0);

    if (g_uvm_global.conf_computing_enabled)
        order = UVM_LOCK_ORDER_SECURE_SEMAPHORE;

    if (tracking_semaphore_uses_mutex(tracking_sem))
        uvm_mutex_init(&tracking_sem->m_lock, order);
    else
        uvm_spin_lock_init(&tracking_sem->s_lock, order);

    atomic64_set(&tracking_sem->completed_value, 0);
    tracking_sem->queued_value = 0;

    return NV_OK;
}

void uvm_gpu_tracking_semaphore_free(uvm_gpu_tracking_semaphore_t *tracking_sem)
{
    uvm_gpu_semaphore_free(&tracking_sem->semaphore);
}

static bool should_skip_secure_semaphore_update(NvU32 last_observed_notifier, NvU32 gpu_notifier)
{
    // No new value, or the GPU is currently writing the new encrypted material
    // and no change in value would still result in corrupted data.
    return (last_observed_notifier == gpu_notifier) || (gpu_notifier % 2);
}

static void uvm_gpu_semaphore_encrypted_payload_update(uvm_channel_t *channel, uvm_gpu_semaphore_t *semaphore)
{
    UvmCslIv local_iv;
    NvU32 local_payload;
    NvU32 new_sem_value;
    NvU32 gpu_notifier;
    NvU32 last_observed_notifier;
    NvU32 new_gpu_notifier = 0;
    NvU32 iv_index = 0;

    // A channel can have multiple entries pending and the tracking semaphore
    // update of each entry can race with this function. Since the semaphore
    // needs to be updated to release a used entry, we never need more
    // than 'num_gpfifo_entries' re-tries.
    unsigned tries_left = channel->num_gpfifo_entries;
    NV_STATUS status = NV_OK;
    NvU8 local_auth_tag[UVM_CONF_COMPUTING_AUTH_TAG_SIZE];
    UvmCslIv *ivs_cpu_addr = semaphore->conf_computing.ivs;
    NvU32 *gpu_notifier_cpu_addr = uvm_gpu_semaphore_get_notifier_cpu_va(semaphore);

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);
    UVM_ASSERT(uvm_channel_is_ce(channel));

    last_observed_notifier = semaphore->conf_computing.last_observed_notifier;
    gpu_notifier = UVM_READ_ONCE(*gpu_notifier_cpu_addr);
    UVM_ASSERT(last_observed_notifier <= gpu_notifier);

    if (should_skip_secure_semaphore_update(last_observed_notifier, gpu_notifier))
        return;

    do {
        gpu_notifier = UVM_READ_ONCE(*gpu_notifier_cpu_addr);

        // Odd notifier value means there's an update in progress.
        if (gpu_notifier % 2)
            continue;

        // Make sure no memory accesses happen before we read the notifier
        smp_mb__after_atomic();

        iv_index = (gpu_notifier / 2) % channel->num_gpfifo_entries;
        memcpy(local_auth_tag, uvm_gpu_semaphore_get_auth_tag_cpu_va(semaphore), sizeof(local_auth_tag));
        local_payload = UVM_READ_ONCE(*uvm_gpu_semaphore_get_encrypted_payload_cpu_va(semaphore));
        memcpy(&local_iv, &ivs_cpu_addr[iv_index], sizeof(local_iv));

        // Make sure the second read of notifier happens after
        // all memory accesses.
        smp_mb__before_atomic();
        new_gpu_notifier = UVM_READ_ONCE(*gpu_notifier_cpu_addr);
        tries_left--;
    } while ((tries_left > 0) && ((gpu_notifier != new_gpu_notifier) || (gpu_notifier % 2)));

    if (!tries_left) {
        status = NV_ERR_INVALID_STATE;
        goto error;
    }

    if (gpu_notifier == new_gpu_notifier) {
        status = uvm_conf_computing_cpu_decrypt(channel,
                                                &new_sem_value,
                                                &local_payload,
                                                &local_iv,
                                                sizeof(new_sem_value),
                                                &local_auth_tag);

        if (status != NV_OK)
            goto error;

        uvm_gpu_semaphore_set_payload(semaphore, new_sem_value);
        UVM_WRITE_ONCE(semaphore->conf_computing.last_observed_notifier, new_gpu_notifier);
    }

    return;

error:
    // Decryption failure is a fatal error as well as running out of try left.
    // Upon testing, all decryption happened within one try, anything that
    // would require ten retry would be considered active tampering with the
    // data structures.
    uvm_global_set_fatal_error(status);
}

static NvU64 update_completed_value_locked(uvm_gpu_tracking_semaphore_t *tracking_semaphore)
{
    NvU64 old_value = atomic64_read(&tracking_semaphore->completed_value);
    // The semaphore value is the bottom 32 bits of completed_value
    NvU32 old_sem_value = (NvU32)old_value;
    NvU32 new_sem_value;
    NvU64 new_value;

    if (tracking_semaphore_uses_mutex(tracking_semaphore))
        uvm_assert_mutex_locked(&tracking_semaphore->m_lock);
    else
        uvm_assert_spinlock_locked(&tracking_semaphore->s_lock);

    if (gpu_semaphore_is_secure(&tracking_semaphore->semaphore)) {
        // TODO: Bug 4008734: [UVM][HCC] Extend secure tracking semaphore
        //                     mechanism to all semaphore
        uvm_channel_t *channel = container_of(tracking_semaphore, uvm_channel_t, tracking_sem);
        uvm_gpu_semaphore_encrypted_payload_update(channel, &tracking_semaphore->semaphore);
    }

    new_sem_value = uvm_gpu_semaphore_get_payload(&tracking_semaphore->semaphore);

    // The following logic to update the completed value is very subtle, it
    // helps to read https://www.kernel.org/doc/Documentation/memory-barriers.txt
    // before going through this code.

    if (old_sem_value == new_sem_value) {
        // No progress since the last update.
        // No additional memory barrier required in this case as completed_value
        // is always updated under the lock that this thread just acquired.
        // That guarantees full ordering with all the accesses the thread that
        // updated completed_value did under the lock including the GPU
        // semaphore read.
        return old_value;
    }

    // Replace the bottom 32-bits with the new semaphore value
    new_value = (old_value & 0xFFFFFFFF00000000ull) | new_sem_value;

    // If we've wrapped around, add 2^32 to the value
    // Notably the user of the GPU tracking semaphore needs to guarantee that
    // the value is updated often enough to notice the wrap around each time it
    // happens. In case of a channel tracking semaphore that's released for each
    // push, it's easily guaranteed because of the small number of GPFIFO
    // entries available per channel (there could be at most as many pending
    // pushes as GPFIFO entries).
    if (unlikely(new_sem_value < old_sem_value))
        new_value += 1ULL << 32;

    // Check for unexpected large jumps of the semaphore value
    UVM_ASSERT_MSG_RELEASE(new_value - old_value <= UVM_GPU_SEMAPHORE_MAX_JUMP,
                           "GPU %s unexpected semaphore (CPU VA 0x%llx) jump from 0x%llx to 0x%llx\n",
                           uvm_gpu_name(tracking_semaphore->semaphore.page->pool->gpu),
                           (NvU64)(uintptr_t)uvm_gpu_semaphore_get_cpu_va(&tracking_semaphore->semaphore),
                           old_value, new_value);

    // Use an atomic write even though the lock is held so that the value can
    // be (carefully) read atomically outside of the lock.
    //
    // atomic64_set() on its own doesn't imply any memory barriers and we need
    // prior memory accesses (in particular the read of the GPU semaphore
    // payload) by this thread to be visible to other threads that see the newly
    // set completed_value. smp_mb__before_atomic() provides that ordering.
    //
    // Also see the comment and matching smp_mb__after_atomic() barrier in
    // uvm_gpu_tracking_semaphore_is_value_completed().
    //
    // Notably as of 4.3, atomic64_set_release() and atomic64_read_acquire()
    // have been added that are exactly what we need and could be slightly
    // faster on arm and powerpc than the implementation below. But at least in
    // 4.3 the implementation looks broken for arm32 (it maps directly to
    // smp_load_acquire() and that doesn't support 64-bit reads on 32-bit
    // architectures) so instead of dealing with that just use a slightly bigger
    // hammer.
    smp_mb__before_atomic();
    atomic64_set(&tracking_semaphore->completed_value, new_value);

    // For this thread, we don't want any later accesses to be ordered above the
    // GPU semaphore read. This could be accomplished by using a
    // smp_load_acquire() for reading it, but given that it's also a pretty
    // recent addition to the kernel, just leverage smp_mb__after_atomic() that
    // guarantees that no accesses will be ordered above the atomic (and hence
    // the GPU semaphore read).
    //
    // Notably the soon following unlock is a release barrier that allows later
    // memory accesses to be reordered above it and hence doesn't provide the
    // necessary ordering with the GPU semaphore read.
    //
    // Also notably this would still need to be handled if we ever switch to
    // atomic64_set_release() and atomic64_read_acquire() for accessing
    // completed_value.
    smp_mb__after_atomic();

    return new_value;
}

NvU64 uvm_gpu_tracking_semaphore_update_completed_value(uvm_gpu_tracking_semaphore_t *tracking_semaphore)
{
    NvU64 completed;

    // Check that the GPU which owns the semaphore is still present
    UVM_ASSERT(tracking_semaphore_check_gpu(tracking_semaphore));

    if (tracking_semaphore_uses_mutex(tracking_semaphore))
        uvm_mutex_lock(&tracking_semaphore->m_lock);
    else
        uvm_spin_lock(&tracking_semaphore->s_lock);

    completed = update_completed_value_locked(tracking_semaphore);

    if (tracking_semaphore_uses_mutex(tracking_semaphore))
        uvm_mutex_unlock(&tracking_semaphore->m_lock);
    else
        uvm_spin_unlock(&tracking_semaphore->s_lock);

    return completed;
}

bool uvm_gpu_tracking_semaphore_is_value_completed(uvm_gpu_tracking_semaphore_t *tracking_sem, NvU64 value)
{
    NvU64 completed = atomic64_read(&tracking_sem->completed_value);

    // Check that the GPU which owns the semaphore is still present
    UVM_ASSERT(tracking_semaphore_check_gpu(tracking_sem));

    if (completed >= value) {
        // atomic64_read() doesn't imply any memory barriers and we need all
        // subsequent memory accesses in this thread to be ordered after the
        // atomic read of the completed value above as that will also order them
        // with any accesses (in particular the GPU semaphore read) performed by
        // the other thread prior to it setting the completed_value we read.
        // smp_mb__after_atomic() provides that ordering.
        //
        // Also see the comment in update_completed_value_locked().
        smp_mb__after_atomic();

        return true;
    }

    return uvm_gpu_tracking_semaphore_update_completed_value(tracking_sem) >= value;
}
