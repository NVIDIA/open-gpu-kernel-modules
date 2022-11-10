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

#include "uvm_gpu_semaphore.h"
#include "uvm_lock.h"
#include "uvm_global.h"
#include "uvm_kvmalloc.h"
#include "uvm_channel.h" // For UVM_GPU_SEMAPHORE_MAX_JUMP

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

    // Count of free semaphores among all the pages
    NvU32 free_semaphores_count;

    // Lock protecting the state of the pool
    uvm_mutex_t mutex;
};

struct uvm_gpu_semaphore_pool_page_struct
{
    // Allocation backing the page
    uvm_rm_mem_t *memory;

    // Pool the page is part of
    uvm_gpu_semaphore_pool_t *pool;

    // Node in the list of all pages in a semaphore pool
    struct list_head all_pages_node;

    // Mask indicating free semaphore indices within the page
    DECLARE_BITMAP(free_semaphores, UVM_SEMAPHORE_COUNT_PER_PAGE);
};

static NvU32 get_index(uvm_gpu_semaphore_t *semaphore)
{
    NvU32 offset;
    NvU32 index;

    UVM_ASSERT(semaphore->payload != NULL);
    UVM_ASSERT(semaphore->page != NULL);

    offset = (char*)semaphore->payload - (char*)uvm_rm_mem_get_cpu_va(semaphore->page->memory);
    UVM_ASSERT(offset % UVM_SEMAPHORE_SIZE == 0);

    index = offset / UVM_SEMAPHORE_SIZE;
    UVM_ASSERT(index < UVM_SEMAPHORE_COUNT_PER_PAGE);

    return index;
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

// Can the GPU access the semaphore, i.e., can Host/Esched address the semaphore
// pool?
static bool gpu_can_access_semaphore_pool(uvm_gpu_t *gpu, uvm_rm_mem_t *rm_mem)
{
    return ((uvm_rm_mem_get_gpu_uvm_va(rm_mem, gpu) + rm_mem->size - 1) < gpu->parent->max_host_va);
}

static NV_STATUS pool_alloc_page(uvm_gpu_semaphore_pool_t *pool)
{
    NV_STATUS status;
    uvm_gpu_semaphore_pool_page_t *pool_page;
    NvU32 *payloads;
    size_t i;

    uvm_assert_mutex_locked(&pool->mutex);

    pool_page = uvm_kvmalloc_zero(sizeof(*pool_page));

    if (!pool_page)
        return NV_ERR_NO_MEMORY;

    pool_page->pool = pool;

    status = uvm_rm_mem_alloc_and_map_all(pool->gpu,
                                          UVM_RM_MEM_TYPE_SYS,
                                          UVM_SEMAPHORE_PAGE_SIZE,
                                          0,
                                          &pool_page->memory);
    if (status != NV_OK)
        goto error;

    // Verify the GPU can access the semaphore pool.
    UVM_ASSERT(gpu_can_access_semaphore_pool(pool->gpu, pool_page->memory));

    // All semaphores are initially free
    bitmap_fill(pool_page->free_semaphores, UVM_SEMAPHORE_COUNT_PER_PAGE);

    list_add(&pool_page->all_pages_node, &pool->pages);
    pool->free_semaphores_count += UVM_SEMAPHORE_COUNT_PER_PAGE;

    // Initialize the semaphore payloads to known values
    if (UVM_IS_DEBUG()) {
        payloads = uvm_rm_mem_get_cpu_va(pool_page->memory);
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
    NvU32 *payloads;
    size_t i;

    UVM_ASSERT(page);
    pool = page->pool;

    uvm_assert_mutex_locked(&pool->mutex);

    // Assert that no semaphores are still allocated
    UVM_ASSERT(bitmap_full(page->free_semaphores, UVM_SEMAPHORE_COUNT_PER_PAGE));
    UVM_ASSERT_MSG(pool->free_semaphores_count >= UVM_SEMAPHORE_COUNT_PER_PAGE,
                   "count: %u\n",
                   pool->free_semaphores_count);

    // Check for semaphore release-after-free
    if (UVM_IS_DEBUG()) {
        payloads = uvm_rm_mem_get_cpu_va(page->memory);
        for (i = 0; i < UVM_SEMAPHORE_COUNT_PER_PAGE; i++)
            UVM_ASSERT(is_canary(payloads[i]));
    }

    pool->free_semaphores_count -= UVM_SEMAPHORE_COUNT_PER_PAGE;
    list_del(&page->all_pages_node);
    uvm_rm_mem_free(page->memory);
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
        NvU32 semaphore_index = find_first_bit(page->free_semaphores, UVM_SEMAPHORE_COUNT_PER_PAGE);
        if (semaphore_index == UVM_SEMAPHORE_COUNT_PER_PAGE)
            continue;

        semaphore->payload = (NvU32*)((char*)uvm_rm_mem_get_cpu_va(page->memory) + semaphore_index * UVM_SEMAPHORE_SIZE);
        semaphore->page = page;

        // Check for semaphore release-after-free
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
    NvU32 index;

    UVM_ASSERT(semaphore);

    // uvm_gpu_semaphore_t is to be embedded in other structures so it should always
    // be accessible, but it may not be initialized in error cases. Early out if
    // page is NULL indicating the semaphore hasn't been allocated successfully.
    page = semaphore->page;
    if (page == NULL)
        return;

    pool = page->pool;
    index = get_index(semaphore);

    // Write a known value lower than the current payload in an attempt to catch
    // release-after-free and acquire-after-free.
    if (UVM_IS_DEBUG())
        uvm_gpu_semaphore_set_payload(semaphore, make_canary(uvm_gpu_semaphore_get_payload(semaphore)));

    uvm_mutex_lock(&pool->mutex);

    semaphore->page = NULL;
    semaphore->payload = NULL;

    ++pool->free_semaphores_count;
    __set_bit(index, page->free_semaphores);

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

    *pool_out = pool;

    return NV_OK;
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
    NvU32 index = get_index(semaphore);
    NvU64 base_va = uvm_rm_mem_get_gpu_va(semaphore->page->memory, gpu, is_proxy_va_space);

    return base_va + UVM_SEMAPHORE_SIZE * index;
}

NvU32 uvm_gpu_semaphore_get_payload(uvm_gpu_semaphore_t *semaphore)
{
    return UVM_GPU_READ_ONCE(*semaphore->payload);
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
    UVM_GPU_WRITE_ONCE(*semaphore->payload, payload);
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
    table_gpu = uvm_gpu_get(gpu->global_id);
    if (table_gpu)
        UVM_ASSERT(table_gpu == gpu);

    // Return a boolean so this function can be used in assertions for
    // conditional compilation
    return true;
}

NV_STATUS uvm_gpu_tracking_semaphore_alloc(uvm_gpu_semaphore_pool_t *pool, uvm_gpu_tracking_semaphore_t *tracking_sem)
{
    NV_STATUS status;

    memset(tracking_sem, 0, sizeof(*tracking_sem));

    status = uvm_gpu_semaphore_alloc(pool, &tracking_sem->semaphore);
    if (status != NV_OK)
        return status;

    UVM_ASSERT(uvm_gpu_semaphore_get_payload(&tracking_sem->semaphore) == 0);

    uvm_spin_lock_init(&tracking_sem->lock, UVM_LOCK_ORDER_LEAF);
    atomic64_set(&tracking_sem->completed_value, 0);
    tracking_sem->queued_value = 0;

    return NV_OK;
}

void uvm_gpu_tracking_semaphore_free(uvm_gpu_tracking_semaphore_t *tracking_sem)
{
    uvm_gpu_semaphore_free(&tracking_sem->semaphore);
}

static NvU64 update_completed_value_locked(uvm_gpu_tracking_semaphore_t *tracking_semaphore)
{
    NvU64 old_value = atomic64_read(&tracking_semaphore->completed_value);
    // The semaphore value is the bottom 32 bits of completed_value
    NvU32 old_sem_value = (NvU32)old_value;
    NvU32 new_sem_value = uvm_gpu_semaphore_get_payload(&tracking_semaphore->semaphore);
    NvU64 new_value;

    uvm_assert_spinlock_locked(&tracking_semaphore->lock);

    // The following logic to update the completed value is very subtle, it
    // helps to read https://www.kernel.org/doc/Documentation/memory-barriers.txt
    // before going through this code.

    if (old_sem_value == new_sem_value) {
        // No progress since the last update.
        // No additional memory barrier required in this case as completed_value
        // is always updated under the spinlock that this thread just acquired.
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
                           tracking_semaphore->semaphore.page->pool->gpu->parent->name,
                           (NvU64)(uintptr_t)tracking_semaphore->semaphore.payload,
                           old_value, new_value);

    // Use an atomic write even though the spinlock is held so that the value can
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
    // Notably the soon following uvm_spin_unlock() is a release barrier that
    // allows later memory accesses to be reordered above it and hence doesn't
    // provide the necessary ordering with the GPU semaphore read.
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

    uvm_spin_lock(&tracking_semaphore->lock);

    completed = update_completed_value_locked(tracking_semaphore);

    uvm_spin_unlock(&tracking_semaphore->lock);

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
