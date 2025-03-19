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
#include "uvm_common.h"
#include "uvm_ioctl.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_hal.h"
#include "uvm_tools.h"
#include "uvm_tools_init.h"
#include "uvm_va_space.h"
#include "uvm_api.h"
#include "uvm_hal_types.h"
#include "uvm_va_block.h"
#include "uvm_va_range.h"
#include "uvm_push.h"
#include "uvm_forward_decl.h"
#include "uvm_range_group.h"
#include "uvm_mem.h"
#include "nv_speculation_barrier.h"

// We limit the number of times a page can be retained by the kernel
// to prevent the user from maliciously passing UVM tools the same page
// over and over again in an attempt to overflow the refcount.
#define MAX_PAGE_COUNT (1 << 20)

typedef struct
{
    NvU32 get_ahead;
    NvU32 get_behind;
    NvU32 put_ahead;
    NvU32 put_behind;
} uvm_tools_queue_snapshot_t;

typedef struct
{
    uvm_spinlock_t lock;
    NvU64 subscribed_queues;
    struct list_head queue_nodes[UvmEventNumTypesAll];

    struct page **queue_buffer_pages;
    void *queue_buffer;
    NvU32 queue_buffer_count;
    NvU32 notification_threshold;

    struct page **control_buffer_pages;
    UvmToolsEventControlData *control;

    wait_queue_head_t wait_queue;
    bool is_wakeup_get_valid;
    NvU32 wakeup_get;
} uvm_tools_queue_t;

typedef struct
{
    struct list_head counter_nodes[UVM_TOTAL_COUNTERS];
    NvU64 subscribed_counters;

    struct page **counter_buffer_pages;
    NvU64 *counters;

    bool all_processors;
    NvProcessorUuid processor;
} uvm_tools_counter_t;

// private_data for /dev/nvidia-uvm-tools
typedef struct
{
    size_t entry_size;
    bool is_queue;
    struct file *uvm_file;
    union
    {
        uvm_tools_queue_t queue;
        uvm_tools_counter_t counter;
    };
} uvm_tools_event_tracker_t;

// Delayed events
//
// Events that require gpu timestamps for asynchronous operations use a delayed
// notification mechanism. Each event type registers a callback that is invoked
// from the update_progress channel routines. The callback then enqueues a
// work item that takes care of notifying the events. This module keeps a
// global list of channels with pending events. Other modules or user apps (via
// ioctl) may call uvm_tools_flush_events to update the progress of the channels
// in the list, as needed.
//
// User apps will need to flush events before removing gpus to avoid getting
// events with gpus ids that have been removed.

// This object describes the pending migrations operations within a VA block
typedef struct
{
    nv_kthread_q_item_t queue_item;
    uvm_processor_id_t dst;
    NvS16 dst_nid;
    uvm_processor_id_t src;
    NvS16 src_nid;
    uvm_va_space_t *va_space;

    uvm_channel_t *channel;
    struct list_head events;
    NvU64 start_timestamp_cpu;
    NvU64 end_timestamp_cpu;
    NvU64 *start_timestamp_gpu_addr;
    NvU64 start_timestamp_gpu;
    NvU64 range_group_id;
} block_migration_data_t;

// This object represents a specific pending migration within a VA block
typedef struct
{
    struct list_head events_node;
    NvU64 bytes;
    NvU64 address;
    NvU64 *end_timestamp_gpu_addr;
    NvU64 end_timestamp_gpu;
    UvmEventMigrationCause cause;
} migration_data_t;

// This object represents a pending gpu faut replay operation
typedef struct
{
    nv_kthread_q_item_t queue_item;
    uvm_channel_t *channel;
    uvm_gpu_id_t gpu_id;
    NvU32 batch_id;
    uvm_fault_client_type_t client_type;
    NvU64 timestamp;
    NvU64 timestamp_gpu;
    NvU64 *timestamp_gpu_addr;
} replay_data_t;

// This object describes the pending map remote operations within a VA block
typedef struct
{
    nv_kthread_q_item_t queue_item;
    uvm_processor_id_t src;
    uvm_processor_id_t dst;
    UvmEventMapRemoteCause cause;
    NvU64 timestamp;
    uvm_va_space_t *va_space;

    uvm_channel_t *channel;
    struct list_head events;
} block_map_remote_data_t;

// This object represents a pending map remote operation
typedef struct
{
    struct list_head events_node;

    NvU64 address;
    NvU64 size;
    NvU64 timestamp_gpu;
    NvU64 *timestamp_gpu_addr;
} map_remote_data_t;


static struct cdev g_uvm_tools_cdev;
static LIST_HEAD(g_tools_va_space_list);
static NvU32 g_tools_enabled_event_count[UvmEventNumTypesAll];
static uvm_rw_semaphore_t g_tools_va_space_list_lock;
static struct kmem_cache *g_tools_event_tracker_cache __read_mostly = NULL;
static struct kmem_cache *g_tools_block_migration_data_cache __read_mostly = NULL;
static struct kmem_cache *g_tools_migration_data_cache __read_mostly = NULL;
static struct kmem_cache *g_tools_replay_data_cache __read_mostly = NULL;
static struct kmem_cache *g_tools_block_map_remote_data_cache __read_mostly = NULL;
static struct kmem_cache *g_tools_map_remote_data_cache __read_mostly = NULL;
static uvm_spinlock_t g_tools_channel_list_lock;
static LIST_HEAD(g_tools_channel_list);
static nv_kthread_q_t g_tools_queue;

static NV_STATUS tools_update_status(uvm_va_space_t *va_space);

static uvm_tools_event_tracker_t *tools_event_tracker(struct file *filp)
{
    return (uvm_tools_event_tracker_t *)atomic_long_read((atomic_long_t *)&filp->private_data);
}

static bool tracker_is_queue(uvm_tools_event_tracker_t *event_tracker)
{
    return event_tracker != NULL && event_tracker->is_queue;
}

static bool tracker_is_counter(uvm_tools_event_tracker_t *event_tracker)
{
    return event_tracker != NULL && !event_tracker->is_queue;
}

static uvm_va_space_t *tools_event_tracker_va_space(uvm_tools_event_tracker_t *event_tracker)
{
    uvm_va_space_t *va_space;
    UVM_ASSERT(event_tracker->uvm_file);
    va_space = uvm_va_space_get(event_tracker->uvm_file);
    return va_space;
}

static void uvm_put_user_pages_dirty(struct page **pages, NvU64 page_count)
{
    NvU64 i;

    for (i = 0; i < page_count; i++) {
        set_page_dirty(pages[i]);
        NV_UNPIN_USER_PAGE(pages[i]);
    }
}

static void unmap_user_pages(struct page **pages, void *addr, NvU64 size)
{
    size = DIV_ROUND_UP(size, PAGE_SIZE);
    vunmap((NvU8 *)addr);
    uvm_put_user_pages_dirty(pages, size);
    uvm_kvfree(pages);
}

// This must be called with the mmap_lock held in read mode or better.
static NV_STATUS check_vmas(struct mm_struct *mm, NvU64 start_va, NvU64 size)
{
    struct vm_area_struct *vma;
    NvU64 addr = start_va;
    NvU64 region_end = start_va + size;

    do {
        vma = find_vma(mm, addr);
        if (!vma || !(addr >= vma->vm_start) || uvm_file_is_nvidia_uvm(vma->vm_file))
            return NV_ERR_INVALID_ARGUMENT;

        addr = vma->vm_end;
    } while (addr < region_end);

    return NV_OK;
}

// Map virtual memory of data from [user_va, user_va + size) of current process into kernel.
// Sets *addr to kernel mapping and *pages to the array of struct pages that contain the memory.
static NV_STATUS map_user_pages(NvU64 user_va, NvU64 size, void **addr, struct page ***pages)
{
    NV_STATUS status = NV_OK;
    long ret = 0;
    long num_pages;
    long i;

    *addr = NULL;
    *pages = NULL;
    num_pages = DIV_ROUND_UP(size, PAGE_SIZE);

    if (uvm_api_range_invalid(user_va, num_pages * PAGE_SIZE)) {
        status = NV_ERR_INVALID_ADDRESS;
        goto fail;
    }

    *pages = uvm_kvmalloc(sizeof(struct page *) * num_pages);
    if (*pages == NULL) {
        status = NV_ERR_NO_MEMORY;
        goto fail;
    }

    // Although uvm_down_read_mmap_lock() is preferable due to its participation
    // in the UVM lock dependency tracker, it cannot be used here. That's
    // because pin_user_pages() may fault in HMM pages which are GPU-resident.
    // When that happens, the UVM page fault handler would record another
    // mmap_read_lock() on the same thread as this one, leading to a false
    // positive lock dependency report.
    //
    // Therefore, use the lower level nv_mmap_read_lock() here.
    nv_mmap_read_lock(current->mm);
    status = check_vmas(current->mm, user_va, size);
    if (status != NV_OK) {
        nv_mmap_read_unlock(current->mm);
        goto fail;
    }
    ret = NV_PIN_USER_PAGES(user_va, num_pages, FOLL_WRITE, *pages);
    nv_mmap_read_unlock(current->mm);

    if (ret != num_pages) {
        status = NV_ERR_INVALID_ARGUMENT;
        goto fail;
    }

    for (i = 0; i < num_pages; i++) {
        if (page_count((*pages)[i]) > MAX_PAGE_COUNT) {
            status = NV_ERR_INVALID_ARGUMENT;
            goto fail;
        }
    }

    *addr = vmap(*pages, num_pages, VM_MAP, PAGE_KERNEL);
    if (*addr == NULL)
        goto fail;

    return NV_OK;

fail:
    if (*pages == NULL)
        return status;

    if (ret > 0)
        uvm_put_user_pages_dirty(*pages, ret);
    else if (ret < 0)
        status = errno_to_nv_status(ret);

    uvm_kvfree(*pages);
    *pages = NULL;
    return status;
}

static void insert_event_tracker(uvm_va_space_t *va_space,
                                 struct list_head *node,
                                 NvU32 list_count,
                                 NvU64 list_mask,
                                 NvU64 *subscribed_mask,
                                 struct list_head *lists,
                                 NvU64 *inserted_lists)
{
    NvU32 i;
    NvU64 insertable_lists = list_mask & ~*subscribed_mask;

    uvm_assert_rwsem_locked_write(&g_tools_va_space_list_lock);
    uvm_assert_rwsem_locked_write(&va_space->tools.lock);

    for (i = 0; i < list_count; i++) {
        if (insertable_lists & (1ULL << i)) {
            ++g_tools_enabled_event_count[i];
            list_add(node + i, lists + i);
        }
    }

    *subscribed_mask |= list_mask;
    *inserted_lists = insertable_lists;
}

static void remove_event_tracker(uvm_va_space_t *va_space,
                                 struct list_head *node,
                                 NvU32 list_count,
                                 NvU64 list_mask,
                                 NvU64 *subscribed_mask)
{
    NvU32 i;
    NvU64 removable_lists = list_mask & *subscribed_mask;

    uvm_assert_rwsem_locked_write(&g_tools_va_space_list_lock);
    uvm_assert_rwsem_locked_write(&va_space->tools.lock);

    for (i = 0; i < list_count; i++) {
        if (removable_lists & (1ULL << i)) {
            UVM_ASSERT(g_tools_enabled_event_count[i] > 0);
            --g_tools_enabled_event_count[i];
            list_del(node + i);
        }
    }

    *subscribed_mask &= ~list_mask;
}

static bool queue_needs_wakeup(uvm_tools_queue_t *queue, uvm_tools_queue_snapshot_t *sn)
{
    NvU32 queue_mask = queue->queue_buffer_count - 1;

    uvm_assert_spinlock_locked(&queue->lock);
    return ((queue->queue_buffer_count + sn->put_behind - sn->get_ahead) & queue_mask) >= queue->notification_threshold;
}

static void destroy_event_tracker(uvm_tools_event_tracker_t *event_tracker)
{
    if (event_tracker->uvm_file != NULL) {
        NV_STATUS status;
        uvm_va_space_t *va_space = tools_event_tracker_va_space(event_tracker);

        uvm_down_write(&g_tools_va_space_list_lock);
        uvm_down_write(&va_space->perf_events.lock);
        uvm_down_write(&va_space->tools.lock);

        if (event_tracker->is_queue) {
            uvm_tools_queue_t *queue = &event_tracker->queue;
            NvU64 buffer_size;

            buffer_size = queue->queue_buffer_count * event_tracker->entry_size;

            remove_event_tracker(va_space,
                                 queue->queue_nodes,
                                 UvmEventNumTypesAll,
                                 queue->subscribed_queues,
                                 &queue->subscribed_queues);

            if (queue->queue_buffer != NULL) {
                unmap_user_pages(queue->queue_buffer_pages,
                                 queue->queue_buffer,
                                 buffer_size);
            }

            if (queue->control != NULL) {
                unmap_user_pages(queue->control_buffer_pages,
                                 queue->control,
                                 sizeof(UvmToolsEventControlData));
            }
        }
        else {
            uvm_tools_counter_t *counters = &event_tracker->counter;

            remove_event_tracker(va_space,
                                 counters->counter_nodes,
                                 UVM_TOTAL_COUNTERS,
                                 counters->subscribed_counters,
                                 &counters->subscribed_counters);

            if (counters->counters != NULL) {
                unmap_user_pages(counters->counter_buffer_pages,
                                 counters->counters,
                                 UVM_TOTAL_COUNTERS * sizeof(NvU64));
            }
        }

        // de-registration should not fail
        status = tools_update_status(va_space);
        UVM_ASSERT(status == NV_OK);

        uvm_up_write(&va_space->tools.lock);
        uvm_up_write(&va_space->perf_events.lock);
        uvm_up_write(&g_tools_va_space_list_lock);

        fput(event_tracker->uvm_file);
    }

    kmem_cache_free(g_tools_event_tracker_cache, event_tracker);
}

static void enqueue_event(const void *entry, size_t entry_size, NvU8 eventType, uvm_tools_queue_t *queue)
{
    UvmToolsEventControlData *ctrl = queue->control;
    uvm_tools_queue_snapshot_t sn;
    NvU32 queue_size = queue->queue_buffer_count;
    NvU32 queue_mask = queue_size - 1;

    // Prevent processor speculation prior to accessing user-mapped memory to
    // avoid leaking information from side-channel attacks. There are many
    // possible paths leading to this point and it would be difficult and error-
    // prone to audit all of them to determine whether user mode could guide
    // this access to kernel memory under speculative execution, so to be on the
    // safe side we'll just always block speculation.
    nv_speculation_barrier();

    uvm_spin_lock(&queue->lock);

    // ctrl is mapped into user space with read and write permissions,
    // so its values cannot be trusted.
    sn.get_behind = atomic_read((atomic_t *)&ctrl->get_behind) & queue_mask;
    sn.put_behind = atomic_read((atomic_t *)&ctrl->put_behind) & queue_mask;
    sn.put_ahead = (sn.put_behind + 1) & queue_mask;

    // one free element means that the queue is full
    if (((queue_size + sn.get_behind - sn.put_behind) & queue_mask) == 1) {
        atomic64_inc((atomic64_t *)&ctrl->dropped + eventType);
        goto unlock;
    }

    memcpy((char *)queue->queue_buffer + sn.put_behind * entry_size, entry, entry_size);

    sn.put_behind = sn.put_ahead;

    // put_ahead and put_behind will always be the same outside of queue->lock
    // this allows the user-space consumer to choose either a 2 or 4 pointer
    // synchronization approach.
    atomic_set((atomic_t *)&ctrl->put_ahead, sn.put_behind);
    atomic_set((atomic_t *)&ctrl->put_behind, sn.put_behind);

    sn.get_ahead = atomic_read((atomic_t *)&ctrl->get_ahead);

    // if the queue needs to be woken up, only signal if we haven't signaled
    // before for this value of get_ahead.
    if (queue_needs_wakeup(queue, &sn) && !(queue->is_wakeup_get_valid && queue->wakeup_get == sn.get_ahead)) {
        queue->is_wakeup_get_valid = true;
        queue->wakeup_get = sn.get_ahead;
        wake_up_all(&queue->wait_queue);
    }

unlock:
    uvm_spin_unlock(&queue->lock);
}

static void uvm_tools_enqueue_event(struct list_head *head, const void *entry, size_t entry_size, NvU8 eventType)
{
    uvm_tools_queue_t *queue;

    UVM_ASSERT(eventType < UvmEventNumTypesAll);

    list_for_each_entry(queue, head + eventType, queue_nodes[eventType])
        enqueue_event(entry, entry_size, eventType, queue);
}

static void uvm_tools_record_event(uvm_va_space_t *va_space, const UvmEventEntry *entry)
{
    NvU8 eventType = entry->eventData.eventType;

    uvm_assert_rwsem_locked(&va_space->tools.lock);

    uvm_tools_enqueue_event(va_space->tools.queues, entry, sizeof(*entry), eventType);
}

static void uvm_tools_record_event_v2(uvm_va_space_t *va_space, const UvmEventEntry_V2 *entry)
{
    NvU8 eventType = entry->eventData.eventType;

    uvm_assert_rwsem_locked(&va_space->tools.lock);

    uvm_tools_enqueue_event(va_space->tools.queues_v2, entry, sizeof(*entry), eventType);
}

static bool counter_matches_processor(UvmCounterName counter, const NvProcessorUuid *processor)
{
    // For compatibility with older counters, CPU faults for memory with a
    // preferred location are reported for their preferred location as well as
    // for the CPU device itself.
    // This check prevents double counting in the aggregate count.
    if (counter == UvmCounterNameCpuPageFaultCount)
        return uvm_uuid_eq(processor, &NV_PROCESSOR_UUID_CPU_DEFAULT);
    return true;
}

static void uvm_tools_inc_counter(uvm_va_space_t *va_space,
                                  UvmCounterName counter,
                                  NvU64 amount,
                                  const NvProcessorUuid *processor)
{
    UVM_ASSERT((NvU32)counter < UVM_TOTAL_COUNTERS);
    uvm_assert_rwsem_locked(&va_space->tools.lock);

    if (amount > 0) {
        uvm_tools_counter_t *counters;

        // Prevent processor speculation prior to accessing user-mapped memory
        // to avoid leaking information from side-channel attacks. There are
        // many possible paths leading to this point and it would be difficult
        // and error-prone to audit all of them to determine whether user mode
        // could guide this access to kernel memory under speculative execution,
        // so to be on the safe side we'll just always block speculation.
        nv_speculation_barrier();

        list_for_each_entry(counters, va_space->tools.counters + counter, counter_nodes[counter]) {
            if ((counters->all_processors && counter_matches_processor(counter, processor)) ||
                uvm_uuid_eq(&counters->processor, processor)) {
                atomic64_add(amount, (atomic64_t *)(counters->counters + counter));
            }
        }
    }
}

static bool tools_is_counter_enabled(uvm_va_space_t *va_space, UvmCounterName counter)
{
    uvm_assert_rwsem_locked(&va_space->tools.lock);

    UVM_ASSERT(counter < UVM_TOTAL_COUNTERS);

    return !list_empty(va_space->tools.counters + counter);
}

static bool tools_is_event_enabled_v1(uvm_va_space_t *va_space, UvmEventType event)
{
    uvm_assert_rwsem_locked(&va_space->tools.lock);

    UVM_ASSERT(event < UvmEventNumTypesAll);

    return !list_empty(va_space->tools.queues + event);
}

static bool tools_is_event_enabled_v2(uvm_va_space_t *va_space, UvmEventType event)
{
    uvm_assert_rwsem_locked(&va_space->tools.lock);

    UVM_ASSERT(event < UvmEventNumTypesAll);

    return !list_empty(va_space->tools.queues_v2 + event);
}

static bool tools_is_event_enabled(uvm_va_space_t *va_space, UvmEventType event)
{
    return tools_is_event_enabled_v1(va_space, event) || tools_is_event_enabled_v2(va_space, event);
}

static bool tools_is_event_enabled_in_any_va_space(UvmEventType event)
{
    bool ret = false;

    uvm_down_read(&g_tools_va_space_list_lock);
    ret = g_tools_enabled_event_count[event] != 0;
    uvm_up_read(&g_tools_va_space_list_lock);

    return ret;
}

static bool tools_are_enabled(uvm_va_space_t *va_space)
{
    NvU32 i;

    uvm_assert_rwsem_locked(&va_space->tools.lock);

    for (i = 0; i < UVM_TOTAL_COUNTERS; i++) {
        if (tools_is_counter_enabled(va_space, i))
            return true;
    }
    for (i = 0; i < UvmEventNumTypesAll; i++) {
        if (tools_is_event_enabled(va_space, i))
            return true;
    }
    return false;
}

static bool tools_is_fault_callback_needed(uvm_va_space_t *va_space)
{
    return tools_is_event_enabled(va_space, UvmEventTypeCpuFault) ||
           tools_is_event_enabled(va_space, UvmEventTypeGpuFault) ||
           tools_is_counter_enabled(va_space, UvmCounterNameCpuPageFaultCount) ||
           tools_is_counter_enabled(va_space, UvmCounterNameGpuPageFaultCount);
}

static bool tools_is_migration_callback_needed(uvm_va_space_t *va_space)
{
    return tools_is_event_enabled(va_space, UvmEventTypeMigration) ||
           tools_is_event_enabled(va_space, UvmEventTypeReadDuplicate) ||
           tools_is_counter_enabled(va_space, UvmCounterNameBytesXferDtH) ||
           tools_is_counter_enabled(va_space, UvmCounterNameBytesXferHtD);
}

static int uvm_tools_open(struct inode *inode, struct file *filp)
{
    filp->private_data = NULL;
    return -nv_status_to_errno(uvm_global_get_status());
}

static int uvm_tools_open_entry(struct inode *inode, struct file *filp)
{
    UVM_ENTRY_RET(uvm_tools_open(inode, filp));
}

static int uvm_tools_release(struct inode *inode, struct file *filp)
{
    uvm_tools_event_tracker_t *event_tracker = tools_event_tracker(filp);
    if (event_tracker != NULL) {
        destroy_event_tracker(event_tracker);
        filp->private_data = NULL;
    }
    return -nv_status_to_errno(uvm_global_get_status());
}

static int uvm_tools_release_entry(struct inode *inode, struct file *filp)
{
    UVM_ENTRY_RET(uvm_tools_release(inode, filp));
}

static long uvm_tools_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
        UVM_ROUTE_CMD_STACK_NO_INIT_CHECK(UVM_TOOLS_INIT_EVENT_TRACKER,         uvm_api_tools_init_event_tracker);
        UVM_ROUTE_CMD_STACK_NO_INIT_CHECK(UVM_TOOLS_SET_NOTIFICATION_THRESHOLD, uvm_api_tools_set_notification_threshold);
        UVM_ROUTE_CMD_STACK_NO_INIT_CHECK(UVM_TOOLS_EVENT_QUEUE_ENABLE_EVENTS,  uvm_api_tools_event_queue_enable_events);
        UVM_ROUTE_CMD_STACK_NO_INIT_CHECK(UVM_TOOLS_EVENT_QUEUE_DISABLE_EVENTS, uvm_api_tools_event_queue_disable_events);
        UVM_ROUTE_CMD_STACK_NO_INIT_CHECK(UVM_TOOLS_ENABLE_COUNTERS,            uvm_api_tools_enable_counters);
        UVM_ROUTE_CMD_STACK_NO_INIT_CHECK(UVM_TOOLS_DISABLE_COUNTERS,           uvm_api_tools_disable_counters);
        UVM_ROUTE_CMD_STACK_NO_INIT_CHECK(UVM_TOOLS_INIT_EVENT_TRACKER_V2,      uvm_api_tools_init_event_tracker_v2);
    }

    uvm_thread_assert_all_unlocked();

    return -EINVAL;
}

static long uvm_tools_unlocked_ioctl_entry(struct file *filp, unsigned int cmd, unsigned long arg)
{
    UVM_ENTRY_RET(uvm_tools_unlocked_ioctl(filp, cmd, arg));
}

static unsigned uvm_tools_poll(struct file *filp, poll_table *wait)
{
    int flags = 0;
    uvm_tools_queue_snapshot_t sn;
    uvm_tools_event_tracker_t *event_tracker;
    UvmToolsEventControlData *ctrl;

    if (uvm_global_get_status() != NV_OK)
        return POLLERR;

    event_tracker = tools_event_tracker(filp);
    if (!tracker_is_queue(event_tracker))
        return POLLERR;

    uvm_spin_lock(&event_tracker->queue.lock);

    event_tracker->queue.is_wakeup_get_valid = false;
    ctrl = event_tracker->queue.control;
    sn.get_ahead = atomic_read((atomic_t *)&ctrl->get_ahead);
    sn.put_behind = atomic_read((atomic_t *)&ctrl->put_behind);

    if (queue_needs_wakeup(&event_tracker->queue, &sn))
        flags = POLLIN | POLLRDNORM;

    uvm_spin_unlock(&event_tracker->queue.lock);

    poll_wait(filp, &event_tracker->queue.wait_queue, wait);
    return flags;
}

static unsigned uvm_tools_poll_entry(struct file *filp, poll_table *wait)
{
    UVM_ENTRY_RET(uvm_tools_poll(filp, wait));
}

static UvmEventFaultType g_hal_to_tools_fault_type_table[UVM_FAULT_TYPE_COUNT] = {
    [UVM_FAULT_TYPE_INVALID_PDE]          = UvmFaultTypeInvalidPde,
    [UVM_FAULT_TYPE_INVALID_PTE]          = UvmFaultTypeInvalidPte,
    [UVM_FAULT_TYPE_ATOMIC]               = UvmFaultTypeAtomic,
    [UVM_FAULT_TYPE_WRITE]                = UvmFaultTypeWrite,
    [UVM_FAULT_TYPE_PDE_SIZE]             = UvmFaultTypeInvalidPdeSize,
    [UVM_FAULT_TYPE_VA_LIMIT_VIOLATION]   = UvmFaultTypeLimitViolation,
    [UVM_FAULT_TYPE_UNBOUND_INST_BLOCK]   = UvmFaultTypeUnboundInstBlock,
    [UVM_FAULT_TYPE_PRIV_VIOLATION]       = UvmFaultTypePrivViolation,
    [UVM_FAULT_TYPE_PITCH_MASK_VIOLATION] = UvmFaultTypePitchMaskViolation,
    [UVM_FAULT_TYPE_WORK_CREATION]        = UvmFaultTypeWorkCreation,
    [UVM_FAULT_TYPE_UNSUPPORTED_APERTURE] = UvmFaultTypeUnsupportedAperture,
    [UVM_FAULT_TYPE_COMPRESSION_FAILURE]  = UvmFaultTypeCompressionFailure,
    [UVM_FAULT_TYPE_UNSUPPORTED_KIND]     = UvmFaultTypeUnsupportedKind,
    [UVM_FAULT_TYPE_REGION_VIOLATION]     = UvmFaultTypeRegionViolation,
    [UVM_FAULT_TYPE_POISONED]             = UvmFaultTypePoison,
    [UVM_FAULT_TYPE_CC_VIOLATION]         = UvmFaultTypeCcViolation,
};

// TODO: add new value for weak atomics in tools
static UvmEventMemoryAccessType g_hal_to_tools_fault_access_type_table[UVM_FAULT_ACCESS_TYPE_COUNT] = {
    [UVM_FAULT_ACCESS_TYPE_ATOMIC_STRONG] = UvmEventMemoryAccessTypeAtomic,
    [UVM_FAULT_ACCESS_TYPE_ATOMIC_WEAK]   = UvmEventMemoryAccessTypeAtomic,
    [UVM_FAULT_ACCESS_TYPE_WRITE]         = UvmEventMemoryAccessTypeWrite,
    [UVM_FAULT_ACCESS_TYPE_READ]          = UvmEventMemoryAccessTypeRead,
    [UVM_FAULT_ACCESS_TYPE_PREFETCH]      = UvmEventMemoryAccessTypePrefetch
};

static UvmEventApertureType g_hal_to_tools_aperture_table[UVM_APERTURE_MAX] = {
    [UVM_APERTURE_PEER_0] = UvmEventAperturePeer0,
    [UVM_APERTURE_PEER_1] = UvmEventAperturePeer1,
    [UVM_APERTURE_PEER_2] = UvmEventAperturePeer2,
    [UVM_APERTURE_PEER_3] = UvmEventAperturePeer3,
    [UVM_APERTURE_PEER_4] = UvmEventAperturePeer4,
    [UVM_APERTURE_PEER_5] = UvmEventAperturePeer5,
    [UVM_APERTURE_PEER_6] = UvmEventAperturePeer6,
    [UVM_APERTURE_PEER_7] = UvmEventAperturePeer7,
    [UVM_APERTURE_SYS]    = UvmEventApertureSys,
    [UVM_APERTURE_VID]    = UvmEventApertureVid,
};

static UvmEventFaultClientType g_hal_to_tools_fault_client_type_table[UVM_FAULT_CLIENT_TYPE_COUNT] = {
    [UVM_FAULT_CLIENT_TYPE_GPC] = UvmEventFaultClientTypeGpc,
    [UVM_FAULT_CLIENT_TYPE_HUB] = UvmEventFaultClientTypeHub,
};

static void record_gpu_fault_instance(uvm_gpu_t *gpu,
                                      uvm_va_space_t *va_space,
                                      const uvm_fault_buffer_entry_t *fault_entry,
                                      NvU64 batch_id,
                                      NvU64 timestamp)
{
    if (tools_is_event_enabled_v1(va_space, UvmEventTypeGpuFault)) {
        UvmEventEntry entry;
        UvmEventGpuFaultInfo *info = &entry.eventData.gpuFault;
        memset(&entry, 0, sizeof(entry));

        info->eventType     = UvmEventTypeGpuFault;
        info->gpuIndex      = uvm_parent_id_value_from_processor_id(gpu->id);
        info->faultType     = g_hal_to_tools_fault_type_table[fault_entry->fault_type];
        info->accessType    = g_hal_to_tools_fault_access_type_table[fault_entry->fault_access_type];
        info->clientType    = g_hal_to_tools_fault_client_type_table[fault_entry->fault_source.client_type];
        if (fault_entry->is_replayable)
            info->gpcId     = fault_entry->fault_source.gpc_id;
        else
            info->channelId = fault_entry->fault_source.channel_id;
        info->clientId      = fault_entry->fault_source.client_id;
        info->address       = fault_entry->fault_address;
        info->timeStamp     = timestamp;
        info->timeStampGpu  = fault_entry->timestamp;
        info->batchId       = batch_id;

        uvm_tools_record_event(va_space, &entry);
    }
    if (tools_is_event_enabled_v2(va_space, UvmEventTypeGpuFault)) {
        UvmEventEntry_V2 entry;
        UvmEventGpuFaultInfo_V2 *info = &entry.eventData.gpuFault;
        memset(&entry, 0, sizeof(entry));

        info->eventType     = UvmEventTypeGpuFault;
        info->gpuIndex      = uvm_id_value(gpu->id);
        info->faultType     = g_hal_to_tools_fault_type_table[fault_entry->fault_type];
        info->accessType    = g_hal_to_tools_fault_access_type_table[fault_entry->fault_access_type];
        info->clientType    = g_hal_to_tools_fault_client_type_table[fault_entry->fault_source.client_type];
        if (fault_entry->is_replayable)
            info->gpcId     = fault_entry->fault_source.gpc_id;
        else
            info->channelId = fault_entry->fault_source.channel_id;
        info->clientId      = fault_entry->fault_source.client_id;
        info->address       = fault_entry->fault_address;
        info->timeStamp     = timestamp;
        info->timeStampGpu  = fault_entry->timestamp;
        info->batchId       = batch_id;

        uvm_tools_record_event_v2(va_space, &entry);
    }
}

static void record_cpu_fault(UvmEventCpuFaultInfo *info, uvm_perf_event_data_t *event_data)
{
    info->eventType = UvmEventTypeCpuFault;
    if (event_data->fault.cpu.is_write)
        info->accessType = UvmEventMemoryAccessTypeWrite;
    else
        info->accessType = UvmEventMemoryAccessTypeRead;

    info->address = event_data->fault.cpu.fault_va;
    info->timeStamp = NV_GETTIME();
    // assume that current owns va_space
    info->pid = uvm_get_stale_process_id();
    info->threadId = uvm_get_stale_thread_id();
    info->pc = event_data->fault.cpu.pc;
    info->cpuId = event_data->fault.cpu.cpu_num;
}

static void uvm_tools_record_fault(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data)
{
    uvm_va_space_t *va_space = event_data->fault.space;

    UVM_ASSERT(event_id == UVM_PERF_EVENT_FAULT);
    UVM_ASSERT(event_data->fault.space);

    uvm_assert_rwsem_locked(&va_space->lock);
    uvm_assert_rwsem_locked(&va_space->perf_events.lock);
    UVM_ASSERT(va_space->tools.enabled);

    uvm_down_read(&va_space->tools.lock);
    UVM_ASSERT(tools_is_fault_callback_needed(va_space));

    if (UVM_ID_IS_CPU(event_data->fault.proc_id)) {
        if (tools_is_event_enabled_v1(va_space, UvmEventTypeCpuFault)) {
            UvmEventEntry entry;
            memset(&entry, 0, sizeof(entry));

            record_cpu_fault(&entry.eventData.cpuFault, event_data);

            uvm_tools_record_event(va_space, &entry);
        }
        if (tools_is_event_enabled_v2(va_space, UvmEventTypeCpuFault)) {
            UvmEventEntry_V2 entry;
            memset(&entry, 0, sizeof(entry));

            record_cpu_fault(&entry.eventData.cpuFault, event_data);

            uvm_tools_record_event_v2(va_space, &entry);
        }
        if (tools_is_counter_enabled(va_space, UvmCounterNameCpuPageFaultCount)) {
            uvm_processor_id_t preferred_location;

            // The UVM Lite tools interface did not represent the CPU as a UVM
            // device. It reported CPU faults against the corresponding
            // allocation's 'home location'. Though this driver's tools
            // interface does include a CPU device, for compatibility, the
            // driver still reports faults against a buffer's preferred
            // location, in addition to the CPU.
            uvm_tools_inc_counter(va_space, UvmCounterNameCpuPageFaultCount, 1, &NV_PROCESSOR_UUID_CPU_DEFAULT);

            preferred_location = event_data->fault.preferred_location;
            if (UVM_ID_IS_GPU(preferred_location)) {
                uvm_gpu_t *gpu = uvm_gpu_get(preferred_location);
                uvm_tools_inc_counter(va_space, UvmCounterNameCpuPageFaultCount, 1, &gpu->uuid);
            }
        }
    }
    else {
        uvm_gpu_t *gpu = uvm_gpu_get(event_data->fault.proc_id);
        UVM_ASSERT(gpu);

        if (tools_is_event_enabled(va_space, UvmEventTypeGpuFault)) {
            NvU64 timestamp = NV_GETTIME();
            uvm_fault_buffer_entry_t *fault_entry = event_data->fault.gpu.buffer_entry;
            uvm_fault_buffer_entry_t *fault_instance;

            record_gpu_fault_instance(gpu, va_space, fault_entry, event_data->fault.gpu.batch_id, timestamp);

            list_for_each_entry(fault_instance, &fault_entry->merged_instances_list, merged_instances_list)
                record_gpu_fault_instance(gpu, va_space, fault_instance, event_data->fault.gpu.batch_id, timestamp);
        }

        if (tools_is_counter_enabled(va_space, UvmCounterNameGpuPageFaultCount))
            uvm_tools_inc_counter(va_space, UvmCounterNameGpuPageFaultCount, 1, &gpu->uuid);
    }
    uvm_up_read(&va_space->tools.lock);
}

static void add_pending_event_for_channel(uvm_channel_t *channel)
{
    uvm_assert_spinlock_locked(&g_tools_channel_list_lock);

    if (channel->tools.pending_event_count++ == 0)
        list_add_tail(&channel->tools.channel_list_node, &g_tools_channel_list);
}

static void remove_pending_event_for_channel(uvm_channel_t *channel)
{
    uvm_assert_spinlock_locked(&g_tools_channel_list_lock);
    UVM_ASSERT(channel->tools.pending_event_count > 0);
    if (--channel->tools.pending_event_count == 0)
        list_del_init(&channel->tools.channel_list_node);
}

static void record_migration_events(void *args)
{
    block_migration_data_t *block_mig = (block_migration_data_t *)args;
    migration_data_t *mig;
    migration_data_t *next;
    uvm_va_space_t *va_space = block_mig->va_space;
    NvU64 gpu_timestamp = block_mig->start_timestamp_gpu;

    uvm_down_read(&va_space->tools.lock);
    list_for_each_entry_safe(mig, next, &block_mig->events, events_node) {
        UVM_ASSERT(mig->bytes > 0);
        list_del(&mig->events_node);

        if (tools_is_event_enabled_v1(va_space, UvmEventTypeMigration)) {
            UvmEventEntry entry;
            UvmEventMigrationInfo *info = &entry.eventData.migration;

            // Initialize fields that are constant throughout the whole block
            memset(&entry, 0, sizeof(entry));
            info->eventType         = UvmEventTypeMigration;
            info->srcIndex          = uvm_parent_id_value_from_processor_id(block_mig->src);
            info->dstIndex          = uvm_parent_id_value_from_processor_id(block_mig->dst);
            info->beginTimeStamp    = block_mig->start_timestamp_cpu;
            info->endTimeStamp      = block_mig->end_timestamp_cpu;
            info->rangeGroupId      = block_mig->range_group_id;
            info->address           = mig->address;
            info->migratedBytes     = mig->bytes;
            info->beginTimeStampGpu = gpu_timestamp;
            info->endTimeStampGpu   = mig->end_timestamp_gpu;
            info->migrationCause    = mig->cause;

            uvm_tools_record_event(va_space, &entry);
        }

        if (tools_is_event_enabled_v2(va_space, UvmEventTypeMigration)) {
            UvmEventEntry_V2 entry;
            UvmEventMigrationInfo_V2 *info = &entry.eventData.migration;

            // Initialize fields that are constant throughout the whole block
            memset(&entry, 0, sizeof(entry));
            info->eventType         = UvmEventTypeMigration;
            info->srcIndex          = uvm_id_value(block_mig->src);
            info->srcNid            = block_mig->src_nid;
            info->dstIndex          = uvm_id_value(block_mig->dst);
            info->dstNid            = block_mig->dst_nid;
            info->beginTimeStamp    = block_mig->start_timestamp_cpu;
            info->endTimeStamp      = block_mig->end_timestamp_cpu;
            info->rangeGroupId      = block_mig->range_group_id;
            info->address           = mig->address;
            info->migratedBytes     = mig->bytes;
            info->beginTimeStampGpu = gpu_timestamp;
            info->endTimeStampGpu   = mig->end_timestamp_gpu;
            info->migrationCause    = mig->cause;

            uvm_tools_record_event_v2(va_space, &entry);
        }

        gpu_timestamp = mig->end_timestamp_gpu;
        kmem_cache_free(g_tools_migration_data_cache, mig);
    }
    uvm_up_read(&va_space->tools.lock);

    UVM_ASSERT(list_empty(&block_mig->events));
    kmem_cache_free(g_tools_block_migration_data_cache, block_mig);
}

static void record_migration_events_entry(void *args)
{
    UVM_ENTRY_VOID(record_migration_events(args));
}

static void on_block_migration_complete(void *ptr)
{
    migration_data_t *mig;
    block_migration_data_t *block_mig = (block_migration_data_t *)ptr;

    block_mig->end_timestamp_cpu = NV_GETTIME();
    block_mig->start_timestamp_gpu = *block_mig->start_timestamp_gpu_addr;
    list_for_each_entry(mig, &block_mig->events, events_node)
        mig->end_timestamp_gpu = *mig->end_timestamp_gpu_addr;

    nv_kthread_q_item_init(&block_mig->queue_item, record_migration_events_entry, block_mig);

    // The UVM driver may notice that work in a channel is complete in a variety of situations
    // and the va_space lock is not always held in all of them, nor can it always be taken safely on them.
    // Dispatching events requires the va_space lock to be held in at least read mode, so
    // this callback simply enqueues the dispatching onto a queue, where the
    // va_space lock is always safe to acquire.
    uvm_spin_lock(&g_tools_channel_list_lock);
    remove_pending_event_for_channel(block_mig->channel);
    nv_kthread_q_schedule_q_item(&g_tools_queue, &block_mig->queue_item);
    uvm_spin_unlock(&g_tools_channel_list_lock);
}

static void record_replay_event_helper(uvm_va_space_t *va_space,
                                       uvm_gpu_id_t gpu_id,
                                       NvU32 batch_id,
                                       uvm_fault_client_type_t client_type,
                                       NvU64 timestamp,
                                       NvU64 timestamp_gpu)
{
    uvm_down_read(&va_space->tools.lock);

    if (tools_is_event_enabled_v1(va_space, UvmEventTypeGpuFaultReplay)) {
        UvmEventEntry entry;

        memset(&entry, 0, sizeof(entry));
        entry.eventData.gpuFaultReplay.eventType    = UvmEventTypeGpuFaultReplay;
        entry.eventData.gpuFaultReplay.gpuIndex     = uvm_parent_id_value_from_processor_id(gpu_id);
        entry.eventData.gpuFaultReplay.batchId      = batch_id;
        entry.eventData.gpuFaultReplay.clientType   = g_hal_to_tools_fault_client_type_table[client_type];
        entry.eventData.gpuFaultReplay.timeStamp    = timestamp;
        entry.eventData.gpuFaultReplay.timeStampGpu = timestamp_gpu;

        uvm_tools_record_event(va_space, &entry);
    }
    if (tools_is_event_enabled_v2(va_space, UvmEventTypeGpuFaultReplay)) {
        UvmEventEntry_V2 entry;

        memset(&entry, 0, sizeof(entry));
        entry.eventData.gpuFaultReplay.eventType    = UvmEventTypeGpuFaultReplay;
        entry.eventData.gpuFaultReplay.gpuIndex     = uvm_id_value(gpu_id);
        entry.eventData.gpuFaultReplay.batchId      = batch_id;
        entry.eventData.gpuFaultReplay.clientType   = g_hal_to_tools_fault_client_type_table[client_type];
        entry.eventData.gpuFaultReplay.timeStamp    = timestamp;
        entry.eventData.gpuFaultReplay.timeStampGpu = timestamp_gpu;

        uvm_tools_record_event_v2(va_space, &entry);
    }

    uvm_up_read(&va_space->tools.lock);
}

static void record_replay_event_broadcast(uvm_gpu_id_t gpu_id,
                                          NvU32 batch_id,
                                          uvm_fault_client_type_t client_type,
                                          NvU64 timestamp,
                                          NvU64 timestamp_gpu)
{
    uvm_va_space_t *va_space;

    uvm_down_read(&g_tools_va_space_list_lock);

    list_for_each_entry(va_space, &g_tools_va_space_list, tools.node) {
        record_replay_event_helper(va_space,
                                   gpu_id,
                                   batch_id,
                                   client_type,
                                   timestamp,
                                   timestamp_gpu);
    }

    uvm_up_read(&g_tools_va_space_list_lock);
}

static void record_replay_events(void *args)
{
    replay_data_t *replay = (replay_data_t *)args;

    record_replay_event_broadcast(replay->gpu_id,
                                  replay->batch_id,
                                  replay->client_type,
                                  replay->timestamp,
                                  replay->timestamp_gpu);

    kmem_cache_free(g_tools_replay_data_cache, replay);
}

static void record_replay_events_entry(void *args)
{
    UVM_ENTRY_VOID(record_replay_events(args));
}

static void on_replay_complete(void *ptr)
{
    replay_data_t *replay = (replay_data_t *)ptr;
    replay->timestamp_gpu = *replay->timestamp_gpu_addr;

    nv_kthread_q_item_init(&replay->queue_item, record_replay_events_entry, ptr);

    uvm_spin_lock(&g_tools_channel_list_lock);
    remove_pending_event_for_channel(replay->channel);
    nv_kthread_q_schedule_q_item(&g_tools_queue, &replay->queue_item);
    uvm_spin_unlock(&g_tools_channel_list_lock);

}

static UvmEventMigrationCause g_make_resident_to_tools_migration_cause[UVM_MAKE_RESIDENT_CAUSE_MAX] = {
    [UVM_MAKE_RESIDENT_CAUSE_REPLAYABLE_FAULT]     = UvmEventMigrationCauseCoherence,
    [UVM_MAKE_RESIDENT_CAUSE_NON_REPLAYABLE_FAULT] = UvmEventMigrationCauseCoherence,
    [UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER]       = UvmEventMigrationCauseAccessCounters,
    [UVM_MAKE_RESIDENT_CAUSE_PREFETCH]             = UvmEventMigrationCausePrefetch,
    [UVM_MAKE_RESIDENT_CAUSE_EVICTION]             = UvmEventMigrationCauseEviction,
    [UVM_MAKE_RESIDENT_CAUSE_API_TOOLS]            = UvmEventMigrationCauseInvalid,
    [UVM_MAKE_RESIDENT_CAUSE_API_MIGRATE]          = UvmEventMigrationCauseUser,
    [UVM_MAKE_RESIDENT_CAUSE_API_SET_RANGE_GROUP]  = UvmEventMigrationCauseCoherence,
    [UVM_MAKE_RESIDENT_CAUSE_API_HINT]             = UvmEventMigrationCauseUser,
};

static void uvm_tools_record_migration_cpu_to_cpu(uvm_va_space_t *va_space,
                                                  uvm_perf_event_data_t *event_data)
{
    if (tools_is_event_enabled_v1(va_space, UvmEventTypeMigration)) {
        UvmEventEntry entry;
        UvmEventMigrationInfo *info = &entry.eventData.migration;

        // CPU-to-CPU migration events can be added directly to the queue.
        memset(&entry, 0, sizeof(entry));
        info->eventType = UvmEventTypeMigration;
        info->srcIndex = uvm_parent_id_value_from_processor_id(event_data->migration.src);
        info->dstIndex = uvm_parent_id_value_from_processor_id(event_data->migration.dst);
        info->address = event_data->migration.address;
        info->migratedBytes = event_data->migration.bytes;
        info->beginTimeStamp = event_data->migration.cpu_start_timestamp;
        info->endTimeStamp = NV_GETTIME();
        info->migrationCause = event_data->migration.cause;
        info->rangeGroupId = UVM_RANGE_GROUP_ID_NONE;

        // During evictions, it is not safe to uvm_range_group_range_find()
        // because the va_space lock is not held.
        if (event_data->migration.cause != UVM_MAKE_RESIDENT_CAUSE_EVICTION) {
            uvm_range_group_range_t *range = uvm_range_group_range_find(va_space, event_data->migration.address);
            if (range != NULL)
                info->rangeGroupId = range->range_group->id;
        }

        uvm_tools_record_event(va_space, &entry);
    }
    if (tools_is_event_enabled_v2(va_space, UvmEventTypeMigration)) {
        UvmEventEntry_V2 entry;
        UvmEventMigrationInfo_V2 *info = &entry.eventData.migration;

        // CPU-to-CPU migration events can be added directly to the queue.
        memset(&entry, 0, sizeof(entry));
        info->eventType = UvmEventTypeMigration;
        info->srcIndex = uvm_id_value(event_data->migration.src);
        info->dstIndex = uvm_id_value(event_data->migration.dst);
        info->srcNid = event_data->migration.src_nid;
        info->dstNid = event_data->migration.dst_nid;
        info->address = event_data->migration.address;
        info->migratedBytes = event_data->migration.bytes;
        info->beginTimeStamp = event_data->migration.cpu_start_timestamp;
        info->endTimeStamp = NV_GETTIME();
        info->migrationCause = event_data->migration.cause;
        info->rangeGroupId = UVM_RANGE_GROUP_ID_NONE;

        // During evictions, it is not safe to uvm_range_group_range_find()
        // because the va_space lock is not held.
        if (event_data->migration.cause != UVM_MAKE_RESIDENT_CAUSE_EVICTION) {
            uvm_range_group_range_t *range = uvm_range_group_range_find(va_space, event_data->migration.address);
            if (range != NULL)
                info->rangeGroupId = range->range_group->id;
        }

        uvm_tools_record_event_v2(va_space, &entry);
    }
}

// For non-CPU-to-CPU migrations (or CPU-to-CPU copies using CEs), this event is
// notified asynchronously when all the migrations pushed to the same uvm_push_t
// object in a call to block_copy_resident_pages_between have finished.
// For CPU-to-CPU copies using memcpy, this event is notified when all of the
// page copies does by block_copy_resident_pages have finished.
static void uvm_tools_record_migration(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data)
{
    uvm_va_block_t *va_block = event_data->migration.block;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);

    UVM_ASSERT(event_id == UVM_PERF_EVENT_MIGRATION);

    uvm_assert_mutex_locked(&va_block->lock);
    uvm_assert_rwsem_locked(&va_space->perf_events.lock);
    UVM_ASSERT(va_space->tools.enabled);

    uvm_down_read(&va_space->tools.lock);
    UVM_ASSERT(tools_is_migration_callback_needed(va_space));

    if (tools_is_event_enabled(va_space, UvmEventTypeMigration)) {
        if (!UVM_ID_IS_CPU(event_data->migration.src) || !UVM_ID_IS_CPU(event_data->migration.dst)) {
            migration_data_t *mig;
            uvm_push_info_t *push_info = uvm_push_info_from_push(event_data->migration.push);
            block_migration_data_t *block_mig = (block_migration_data_t *)push_info->on_complete_data;

            if (push_info->on_complete != NULL) {
                mig = kmem_cache_alloc(g_tools_migration_data_cache, NV_UVM_GFP_FLAGS);
                if (mig == NULL)
                    goto done_unlock;

                mig->address = event_data->migration.address;
                mig->bytes = event_data->migration.bytes;
                mig->end_timestamp_gpu_addr = uvm_push_timestamp(event_data->migration.push);
                mig->cause = g_make_resident_to_tools_migration_cause[event_data->migration.cause];

                list_add_tail(&mig->events_node, &block_mig->events);
            }
        }
        else {
            uvm_tools_record_migration_cpu_to_cpu(va_space, event_data);
        }
    }

    // We don't want to increment neither UvmCounterNameBytesXferDtH nor
    // UvmCounterNameBytesXferHtD in a CPU-to-CPU migration.
    if (UVM_ID_IS_CPU(event_data->migration.src) && UVM_ID_IS_CPU(event_data->migration.dst))
        goto done_unlock;

    // Increment counters
    if (UVM_ID_IS_CPU(event_data->migration.src) &&
        tools_is_counter_enabled(va_space, UvmCounterNameBytesXferHtD)) {
        uvm_gpu_t *gpu = uvm_gpu_get(event_data->migration.dst);
        uvm_tools_inc_counter(va_space,
                              UvmCounterNameBytesXferHtD,
                              event_data->migration.bytes,
                              &gpu->uuid);
    }
    if (UVM_ID_IS_CPU(event_data->migration.dst) &&
        tools_is_counter_enabled(va_space, UvmCounterNameBytesXferDtH)) {
        uvm_gpu_t *gpu = uvm_gpu_get(event_data->migration.src);
        uvm_tools_inc_counter(va_space,
                              UvmCounterNameBytesXferDtH,
                              event_data->migration.bytes,
                              &gpu->uuid);
    }

done_unlock:
    uvm_up_read(&va_space->tools.lock);
}

// This event is notified asynchronously when it is marked as completed in the
// pushbuffer the replay method belongs to.
void uvm_tools_broadcast_replay(uvm_gpu_t *gpu,
                                uvm_push_t *push,
                                NvU32 batch_id,
                                uvm_fault_client_type_t client_type)
{
    uvm_push_info_t *push_info = uvm_push_info_from_push(push);
    replay_data_t *replay;

    // Perform delayed notification only if some VA space has signed up for
    // UvmEventTypeGpuFaultReplay
    if (!tools_is_event_enabled_in_any_va_space(UvmEventTypeGpuFaultReplay))
        return;

    replay = kmem_cache_alloc(g_tools_replay_data_cache, NV_UVM_GFP_FLAGS);
    if (replay == NULL)
        return;

    UVM_ASSERT(push_info->on_complete == NULL && push_info->on_complete_data == NULL);

    replay->timestamp_gpu_addr = uvm_push_timestamp(push);
    replay->gpu_id             = gpu->id;
    replay->batch_id           = batch_id;
    replay->client_type        = client_type;
    replay->timestamp          = NV_GETTIME();
    replay->channel            = push->channel;

    push_info->on_complete_data = replay;
    push_info->on_complete = on_replay_complete;

    uvm_spin_lock(&g_tools_channel_list_lock);
    add_pending_event_for_channel(replay->channel);
    uvm_spin_unlock(&g_tools_channel_list_lock);
}

void uvm_tools_broadcast_replay_sync(uvm_gpu_t *gpu, NvU32 batch_id, uvm_fault_client_type_t client_type)
{
    UVM_ASSERT(!gpu->parent->has_clear_faulted_channel_method);

    if (!tools_is_event_enabled_in_any_va_space(UvmEventTypeGpuFaultReplay))
        return;

    record_replay_event_broadcast(gpu->id,
                                  batch_id,
                                  client_type,
                                  NV_GETTIME(),
                                  gpu->parent->host_hal->get_time(gpu));
}

void uvm_tools_record_access_counter(uvm_va_space_t *va_space,
                                     uvm_gpu_id_t gpu_id,
                                     const uvm_access_counter_buffer_entry_t *buffer_entry)
{
    uvm_down_read(&va_space->tools.lock);

    if (tools_is_event_enabled_v1(va_space, UvmEventTypeTestAccessCounter)) {
        UvmEventEntry entry;
        UvmEventTestAccessCounterInfo *info = &entry.testEventData.accessCounter;

        memset(&entry, 0, sizeof(entry));

        info->eventType           = UvmEventTypeTestAccessCounter;
        info->srcIndex            = uvm_parent_id_value_from_processor_id(gpu_id);
        info->address             = buffer_entry->address;
        info->instancePtr         = buffer_entry->instance_ptr.address;
        info->instancePtrAperture = g_hal_to_tools_aperture_table[buffer_entry->instance_ptr.aperture];
        info->veId                = buffer_entry->ve_id;
        info->value               = buffer_entry->counter_value;
        info->subGranularity      = buffer_entry->sub_granularity;
        info->bank                = buffer_entry->bank;
        info->tag                 = buffer_entry->tag;

        uvm_tools_record_event(va_space, &entry);
    }
    if (tools_is_event_enabled_v2(va_space, UvmEventTypeTestAccessCounter)) {
        UvmEventEntry_V2 entry;
        UvmEventTestAccessCounterInfo_V2 *info = &entry.testEventData.accessCounter;

        memset(&entry, 0, sizeof(entry));

        info->eventType           = UvmEventTypeTestAccessCounter;
        info->srcIndex            = uvm_id_value(gpu_id);
        info->address             = buffer_entry->address;
        info->instancePtr         = buffer_entry->instance_ptr.address;
        info->instancePtrAperture = g_hal_to_tools_aperture_table[buffer_entry->instance_ptr.aperture];
        info->veId                = buffer_entry->ve_id;
        info->value               = buffer_entry->counter_value;
        info->subGranularity      = buffer_entry->sub_granularity;
        info->bank                = buffer_entry->bank;
        info->tag                 = buffer_entry->tag;

        uvm_tools_record_event_v2(va_space, &entry);
    }

    uvm_up_read(&va_space->tools.lock);
}

void uvm_tools_broadcast_access_counter(uvm_gpu_t *gpu, const uvm_access_counter_buffer_entry_t *buffer_entry)
{
    uvm_va_space_t *va_space;

    uvm_down_read(&g_tools_va_space_list_lock);
    list_for_each_entry(va_space, &g_tools_va_space_list, tools.node) {
        uvm_tools_record_access_counter(va_space, gpu->id, buffer_entry);
    }
    uvm_up_read(&g_tools_va_space_list_lock);
}

void uvm_tools_test_hmm_split_invalidate(uvm_va_space_t *va_space)
{
    UvmEventEntry_V2 entry;

    if (!va_space->tools.enabled)
        return;

    entry.testEventData.splitInvalidate.eventType = UvmEventTypeTestHmmSplitInvalidate;
    uvm_down_read(&va_space->tools.lock);
    uvm_tools_record_event_v2(va_space, &entry);
    uvm_up_read(&va_space->tools.lock);
}

// This function is used as a begin marker to group all migrations within a VA
// block that are performed in the same call to
// block_copy_resident_pages_between. All of these are pushed to the same
// uvm_push_t object, and will be notified in burst when the last one finishes.
void uvm_tools_record_block_migration_begin(uvm_va_block_t *va_block,
                                            uvm_push_t *push,
                                            uvm_processor_id_t dst_id,
                                            int dst_nid,
                                            uvm_processor_id_t src_id,
                                            int src_nid,
                                            NvU64 start,
                                            uvm_make_resident_cause_t cause)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_range_group_range_t *range;

    // Calls from tools read/write functions to make_resident must not trigger
    // any migration
    UVM_ASSERT(cause != UVM_MAKE_RESIDENT_CAUSE_API_TOOLS);

    // During evictions the va_space lock is not held.
    if (cause != UVM_MAKE_RESIDENT_CAUSE_EVICTION)
        uvm_assert_rwsem_locked(&va_space->lock);

    if (!va_space->tools.enabled)
        return;

    uvm_down_read(&va_space->tools.lock);

    // Perform delayed notification only if the VA space has signed up for
    // UvmEventTypeMigration
    if (tools_is_event_enabled(va_space, UvmEventTypeMigration)) {
        block_migration_data_t *block_mig;
        uvm_push_info_t *push_info = uvm_push_info_from_push(push);

        UVM_ASSERT(push_info->on_complete == NULL && push_info->on_complete_data == NULL);

        block_mig = kmem_cache_alloc(g_tools_block_migration_data_cache, NV_UVM_GFP_FLAGS);
        if (block_mig == NULL)
            goto done_unlock;

        block_mig->start_timestamp_gpu_addr = uvm_push_timestamp(push);
        block_mig->channel = push->channel;
        block_mig->start_timestamp_cpu = NV_GETTIME();
        block_mig->dst = dst_id;
        block_mig->dst_nid = dst_nid;
        block_mig->src = src_id;
        block_mig->src_nid = src_nid;
        block_mig->range_group_id = UVM_RANGE_GROUP_ID_NONE;

        // During evictions, it is not safe to uvm_range_group_range_find() because the va_space lock is not held.
        if (cause != UVM_MAKE_RESIDENT_CAUSE_EVICTION) {
            range = uvm_range_group_range_find(va_space, start);
            if (range != NULL)
                block_mig->range_group_id = range->range_group->id;
        }
        block_mig->va_space = va_space;

        INIT_LIST_HEAD(&block_mig->events);
        push_info->on_complete_data = block_mig;
        push_info->on_complete = on_block_migration_complete;

        uvm_spin_lock(&g_tools_channel_list_lock);
        add_pending_event_for_channel(block_mig->channel);
        uvm_spin_unlock(&g_tools_channel_list_lock);
    }

done_unlock:
    uvm_up_read(&va_space->tools.lock);
}

void uvm_tools_record_read_duplicate(uvm_va_block_t *va_block,
                                     uvm_processor_id_t dst,
                                     uvm_va_block_region_t region,
                                     const uvm_page_mask_t *page_mask)
{
    uvm_processor_mask_t *resident_processors;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);

    if (!va_space->tools.enabled)
        return;

    resident_processors = uvm_processor_mask_cache_alloc();
    if (!resident_processors)
        return;

    uvm_down_read(&va_space->tools.lock);

    if (tools_is_event_enabled_v1(va_space, UvmEventTypeReadDuplicate)) {
        UvmEventEntry entry;
        UvmEventReadDuplicateInfo *info_read_duplicate = &entry.eventData.readDuplicate;
        uvm_page_index_t page_index;

        memset(&entry, 0, sizeof(entry));

        info_read_duplicate->eventType = UvmEventTypeReadDuplicate;
        info_read_duplicate->size      = PAGE_SIZE;
        info_read_duplicate->timeStamp = NV_GETTIME();

        for_each_va_block_page_in_region_mask(page_index, page_mask, region) {
            uvm_processor_id_t id;

            info_read_duplicate->address = uvm_va_block_cpu_page_address(va_block, page_index);
            info_read_duplicate->processors = 0;

            uvm_va_block_page_resident_processors(va_block, page_index, resident_processors);

            for_each_id_in_mask(id, resident_processors)
                __set_bit(uvm_parent_id_value_from_processor_id(id), (unsigned long *)&info_read_duplicate->processors);

            uvm_tools_record_event(va_space, &entry);
        }
    }

    if (tools_is_event_enabled_v2(va_space, UvmEventTypeReadDuplicate)) {
        UvmEventEntry_V2 entry;
        UvmEventReadDuplicateInfo_V2 *info_read_duplicate = &entry.eventData.readDuplicate;
        uvm_page_index_t page_index;

        memset(&entry, 0, sizeof(entry));

        info_read_duplicate->eventType = UvmEventTypeReadDuplicate;
        info_read_duplicate->size      = PAGE_SIZE;
        info_read_duplicate->timeStamp = NV_GETTIME();

        for_each_va_block_page_in_region_mask(page_index, page_mask, region) {
            uvm_processor_id_t id;

            info_read_duplicate->address = uvm_va_block_cpu_page_address(va_block, page_index);
            memset(info_read_duplicate->processors, 0, sizeof(info_read_duplicate->processors));

            uvm_va_block_page_resident_processors(va_block, page_index, resident_processors);

            for_each_id_in_mask(id, resident_processors)
                __set_bit(uvm_id_value(id), (unsigned long *)info_read_duplicate->processors);

            uvm_tools_record_event_v2(va_space, &entry);
        }
    }

    uvm_up_read(&va_space->tools.lock);

    uvm_processor_mask_cache_free(resident_processors);
}

void uvm_tools_record_read_duplicate_invalidate(uvm_va_block_t *va_block,
                                                uvm_processor_id_t dst,
                                                uvm_va_block_region_t region,
                                                const uvm_page_mask_t *page_mask)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);

    if (!va_space->tools.enabled)
        return;

    uvm_down_read(&va_space->tools.lock);
    if (tools_is_event_enabled_v1(va_space, UvmEventTypeReadDuplicateInvalidate)) {
        UvmEventEntry entry;
        uvm_page_index_t page_index;
        UvmEventReadDuplicateInvalidateInfo *info = &entry.eventData.readDuplicateInvalidate;

        memset(&entry, 0, sizeof(entry));

        info->eventType     = UvmEventTypeReadDuplicateInvalidate;
        info->residentIndex = uvm_parent_id_value_from_processor_id(dst);
        info->size          = PAGE_SIZE;
        info->timeStamp     = NV_GETTIME();

        for_each_va_block_page_in_region_mask(page_index, page_mask, region) {
            UVM_ASSERT(uvm_page_mask_test(&va_block->read_duplicated_pages, page_index));

            info->address = uvm_va_block_cpu_page_address(va_block, page_index);
            uvm_tools_record_event(va_space, &entry);
        }
    }
    if (tools_is_event_enabled_v2(va_space, UvmEventTypeReadDuplicateInvalidate)) {
        UvmEventEntry_V2 entry;
        uvm_page_index_t page_index;
        UvmEventReadDuplicateInvalidateInfo_V2 *info = &entry.eventData.readDuplicateInvalidate;

        memset(&entry, 0, sizeof(entry));

        info->eventType     = UvmEventTypeReadDuplicateInvalidate;
        info->residentIndex = uvm_id_value(dst);
        info->size          = PAGE_SIZE;
        info->timeStamp     = NV_GETTIME();

        for_each_va_block_page_in_region_mask(page_index, page_mask, region) {
            UVM_ASSERT(uvm_page_mask_test(&va_block->read_duplicated_pages, page_index));

            info->address = uvm_va_block_cpu_page_address(va_block, page_index);
            uvm_tools_record_event_v2(va_space, &entry);
        }
    }
    uvm_up_read(&va_space->tools.lock);
}

static void tools_schedule_completed_events(void)
{
    uvm_channel_t *channel;
    uvm_channel_t *next_channel;
    NvU64 channel_count = 0;
    NvU64 i;

    uvm_spin_lock(&g_tools_channel_list_lock);

    // retain every channel list entry currently in the list and keep track of their count.
    list_for_each_entry(channel, &g_tools_channel_list, tools.channel_list_node) {
        ++channel->tools.pending_event_count;
        ++channel_count;
    }
    uvm_spin_unlock(&g_tools_channel_list_lock);

    if (channel_count == 0)
        return;

    // new entries always appear at the end, and all the entries seen in the first loop have been retained
    // so it is safe to go through them
    channel = list_first_entry(&g_tools_channel_list, uvm_channel_t, tools.channel_list_node);
    for (i = 0; i < channel_count; i++) {
        uvm_channel_update_progress_all(channel);
        channel = list_next_entry(channel, tools.channel_list_node);
    }

    // now release all the entries we retained in the beginning
    i = 0;
    uvm_spin_lock(&g_tools_channel_list_lock);
    list_for_each_entry_safe(channel, next_channel, &g_tools_channel_list, tools.channel_list_node) {
        if (i++ == channel_count)
            break;

        remove_pending_event_for_channel(channel);
    }
    uvm_spin_unlock(&g_tools_channel_list_lock);
}

void uvm_tools_record_cpu_fatal_fault(uvm_va_space_t *va_space,
                                      NvU64 address,
                                      bool is_write,
                                      UvmEventFatalReason reason)
{
    uvm_assert_rwsem_locked(&va_space->lock);

    if (!va_space->tools.enabled)
        return;

    uvm_down_read(&va_space->tools.lock);
    if (tools_is_event_enabled_v1(va_space, UvmEventTypeFatalFault)) {
        UvmEventEntry entry;
        UvmEventFatalFaultInfo *info = &entry.eventData.fatalFault;

        memset(&entry, 0, sizeof(entry));

        info->eventType      = UvmEventTypeFatalFault;
        info->processorIndex = UVM_ID_CPU_VALUE;
        info->timeStamp      = NV_GETTIME();
        info->address        = address;
        info->accessType     = is_write? UvmEventMemoryAccessTypeWrite: UvmEventMemoryAccessTypeRead;
        // info->faultType is not valid for cpu faults
        info->reason         = reason;

        uvm_tools_record_event(va_space, &entry);
    }
    if (tools_is_event_enabled_v2(va_space, UvmEventTypeFatalFault)) {
        UvmEventEntry_V2 entry;
        UvmEventFatalFaultInfo_V2 *info = &entry.eventData.fatalFault;

        memset(&entry, 0, sizeof(entry));

        info->eventType      = UvmEventTypeFatalFault;
        info->processorIndex = UVM_ID_CPU_VALUE;
        info->timeStamp      = NV_GETTIME();
        info->address        = address;
        info->accessType     = is_write? UvmEventMemoryAccessTypeWrite: UvmEventMemoryAccessTypeRead;
        // info->faultType is not valid for cpu faults
        info->reason         = reason;

        uvm_tools_record_event_v2(va_space, &entry);
    }
    uvm_up_read(&va_space->tools.lock);
}

void uvm_tools_record_gpu_fatal_fault(uvm_gpu_id_t gpu_id,
                                      uvm_va_space_t *va_space,
                                      const uvm_fault_buffer_entry_t *buffer_entry,
                                      UvmEventFatalReason reason)
{
    uvm_assert_rwsem_locked(&va_space->lock);

    if (!va_space->tools.enabled)
        return;

    uvm_down_read(&va_space->tools.lock);
    if (tools_is_event_enabled_v1(va_space, UvmEventTypeFatalFault)) {
        UvmEventEntry entry;
        UvmEventFatalFaultInfo *info = &entry.eventData.fatalFault;

        memset(&entry, 0, sizeof(entry));

        info->eventType      = UvmEventTypeFatalFault;
        info->processorIndex = uvm_parent_id_value_from_processor_id(gpu_id);
        info->timeStamp      = NV_GETTIME();
        info->address        = buffer_entry->fault_address;
        info->accessType     = g_hal_to_tools_fault_access_type_table[buffer_entry->fault_access_type];
        info->faultType      = g_hal_to_tools_fault_type_table[buffer_entry->fault_type];
        info->reason         = reason;

        uvm_tools_record_event(va_space, &entry);
    }
    if (tools_is_event_enabled_v2(va_space, UvmEventTypeFatalFault)) {
        UvmEventEntry_V2 entry;
        UvmEventFatalFaultInfo_V2 *info = &entry.eventData.fatalFault;

        memset(&entry, 0, sizeof(entry));

        info->eventType      = UvmEventTypeFatalFault;
        info->processorIndex = uvm_id_value(gpu_id);
        info->timeStamp      = NV_GETTIME();
        info->address        = buffer_entry->fault_address;
        info->accessType     = g_hal_to_tools_fault_access_type_table[buffer_entry->fault_access_type];
        info->faultType      = g_hal_to_tools_fault_type_table[buffer_entry->fault_type];
        info->reason         = reason;

        uvm_tools_record_event_v2(va_space, &entry);
    }
    uvm_up_read(&va_space->tools.lock);
}

void uvm_tools_record_thrashing(uvm_va_space_t *va_space,
                                NvU64 address,
                                size_t region_size,
                                const uvm_processor_mask_t *processors)
{
    UVM_ASSERT(address);
    UVM_ASSERT(PAGE_ALIGNED(address));
    UVM_ASSERT(region_size > 0);

    uvm_assert_rwsem_locked(&va_space->lock);

    if (!va_space->tools.enabled)
        return;

    uvm_down_read(&va_space->tools.lock);
    if (tools_is_event_enabled_v1(va_space, UvmEventTypeThrashingDetected)) {
        UvmEventEntry entry;
        UvmEventThrashingDetectedInfo *info = &entry.eventData.thrashing;
        uvm_processor_id_t id;

        memset(&entry, 0, sizeof(entry));

        info->eventType = UvmEventTypeThrashingDetected;
        info->address   = address;
        info->size      = region_size;
        info->timeStamp = NV_GETTIME();

        for_each_id_in_mask(id, processors)
            __set_bit(uvm_parent_id_value_from_processor_id(id),
                      (unsigned long *)&info->processors);

        uvm_tools_record_event(va_space, &entry);
    }
    if (tools_is_event_enabled_v2(va_space, UvmEventTypeThrashingDetected)) {
        UvmEventEntry_V2 entry;
        UvmEventThrashingDetectedInfo_V2 *info = &entry.eventData.thrashing;

        memset(&entry, 0, sizeof(entry));

        info->eventType = UvmEventTypeThrashingDetected;
        info->address   = address;
        info->size      = region_size;
        info->timeStamp = NV_GETTIME();

        BUILD_BUG_ON(UVM_MAX_PROCESSORS < UVM_ID_MAX_PROCESSORS);
        bitmap_copy((long unsigned *)&info->processors, processors->bitmap, UVM_ID_MAX_PROCESSORS);

        uvm_tools_record_event_v2(va_space, &entry);
    }
    uvm_up_read(&va_space->tools.lock);
}

void uvm_tools_record_throttling_start(uvm_va_space_t *va_space, NvU64 address, uvm_processor_id_t processor)
{
    UVM_ASSERT(address);
    UVM_ASSERT(PAGE_ALIGNED(address));
    UVM_ASSERT(UVM_ID_IS_VALID(processor));

    uvm_assert_rwsem_locked(&va_space->lock);

    if (!va_space->tools.enabled)
        return;

    uvm_down_read(&va_space->tools.lock);
    if (tools_is_event_enabled_v1(va_space, UvmEventTypeThrottlingStart)) {
        UvmEventEntry entry;
        UvmEventThrottlingStartInfo *info = &entry.eventData.throttlingStart;

        memset(&entry, 0, sizeof(entry));

        info->eventType      = UvmEventTypeThrottlingStart;
        info->processorIndex = uvm_parent_id_value_from_processor_id(processor);
        info->address        = address;
        info->timeStamp      = NV_GETTIME();

        uvm_tools_record_event(va_space, &entry);
    }
    if (tools_is_event_enabled_v2(va_space, UvmEventTypeThrottlingStart)) {
        UvmEventEntry_V2 entry;
        UvmEventThrottlingStartInfo_V2 *info = &entry.eventData.throttlingStart;

        memset(&entry, 0, sizeof(entry));

        info->eventType      = UvmEventTypeThrottlingStart;
        info->processorIndex = uvm_id_value(processor);
        info->address        = address;
        info->timeStamp      = NV_GETTIME();

        uvm_tools_record_event_v2(va_space, &entry);
    }
    uvm_up_read(&va_space->tools.lock);
}

void uvm_tools_record_throttling_end(uvm_va_space_t *va_space, NvU64 address, uvm_processor_id_t processor)
{
    UVM_ASSERT(address);
    UVM_ASSERT(PAGE_ALIGNED(address));
    UVM_ASSERT(UVM_ID_IS_VALID(processor));

    uvm_assert_rwsem_locked(&va_space->lock);

    if (!va_space->tools.enabled)
        return;

    uvm_down_read(&va_space->tools.lock);
    if (tools_is_event_enabled_v1(va_space, UvmEventTypeThrottlingEnd)) {
        UvmEventEntry entry;
        UvmEventThrottlingEndInfo *info = &entry.eventData.throttlingEnd;

        memset(&entry, 0, sizeof(entry));

        info->eventType      = UvmEventTypeThrottlingEnd;
        info->processorIndex = uvm_parent_id_value_from_processor_id(processor);
        info->address        = address;
        info->timeStamp      = NV_GETTIME();

        uvm_tools_record_event(va_space, &entry);
    }
    if (tools_is_event_enabled_v2(va_space, UvmEventTypeThrottlingEnd)) {
        UvmEventEntry_V2 entry;
        UvmEventThrottlingEndInfo_V2 *info = &entry.eventData.throttlingEnd;

        memset(&entry, 0, sizeof(entry));

        info->eventType      = UvmEventTypeThrottlingEnd;
        info->processorIndex = uvm_id_value(processor);
        info->address        = address;
        info->timeStamp      = NV_GETTIME();

        uvm_tools_record_event_v2(va_space, &entry);
    }
    uvm_up_read(&va_space->tools.lock);
}

static void record_map_remote_events(void *args)
{
    block_map_remote_data_t *block_map_remote = (block_map_remote_data_t *)args;
    map_remote_data_t *map_remote, *next;
    uvm_va_space_t *va_space = block_map_remote->va_space;

    uvm_down_read(&va_space->tools.lock);
    list_for_each_entry_safe(map_remote, next, &block_map_remote->events, events_node) {
        list_del(&map_remote->events_node);

        if (tools_is_event_enabled_v1(va_space, UvmEventTypeMapRemote)) {
            UvmEventEntry entry;

            memset(&entry, 0, sizeof(entry));

            entry.eventData.mapRemote.eventType      = UvmEventTypeMapRemote;
            entry.eventData.mapRemote.srcIndex       = uvm_parent_id_value_from_processor_id(block_map_remote->src);
            entry.eventData.mapRemote.dstIndex       = uvm_parent_id_value_from_processor_id(block_map_remote->dst);
            entry.eventData.mapRemote.mapRemoteCause = block_map_remote->cause;
            entry.eventData.mapRemote.timeStamp      = block_map_remote->timestamp;
            entry.eventData.mapRemote.address        = map_remote->address;
            entry.eventData.mapRemote.size           = map_remote->size;
            entry.eventData.mapRemote.timeStampGpu   = map_remote->timestamp_gpu;

            uvm_tools_record_event(va_space, &entry);
        }

        if (tools_is_event_enabled_v2(va_space, UvmEventTypeMapRemote)) {
            UvmEventEntry_V2 entry;

            memset(&entry, 0, sizeof(entry));

            entry.eventData.mapRemote.eventType      = UvmEventTypeMapRemote;
            entry.eventData.mapRemote.srcIndex       = uvm_id_value(block_map_remote->src);
            entry.eventData.mapRemote.dstIndex       = uvm_id_value(block_map_remote->dst);
            entry.eventData.mapRemote.mapRemoteCause = block_map_remote->cause;
            entry.eventData.mapRemote.timeStamp      = block_map_remote->timestamp;
            entry.eventData.mapRemote.address        = map_remote->address;
            entry.eventData.mapRemote.size           = map_remote->size;
            entry.eventData.mapRemote.timeStampGpu   = map_remote->timestamp_gpu;

            uvm_tools_record_event_v2(va_space, &entry);
        }

        kmem_cache_free(g_tools_map_remote_data_cache, map_remote);
    }
    uvm_up_read(&va_space->tools.lock);

    UVM_ASSERT(list_empty(&block_map_remote->events));
    kmem_cache_free(g_tools_block_map_remote_data_cache, block_map_remote);
}

static void record_map_remote_events_entry(void *args)
{
    UVM_ENTRY_VOID(record_map_remote_events(args));
}

static void on_map_remote_complete(void *ptr)
{
    block_map_remote_data_t *block_map_remote = (block_map_remote_data_t *)ptr;
    map_remote_data_t *map_remote;

    // Only GPU mappings use the deferred mechanism
    UVM_ASSERT(UVM_ID_IS_GPU(block_map_remote->src));
    list_for_each_entry(map_remote, &block_map_remote->events, events_node)
        map_remote->timestamp_gpu = *map_remote->timestamp_gpu_addr;

    nv_kthread_q_item_init(&block_map_remote->queue_item, record_map_remote_events_entry, ptr);

    uvm_spin_lock(&g_tools_channel_list_lock);
    remove_pending_event_for_channel(block_map_remote->channel);
    nv_kthread_q_schedule_q_item(&g_tools_queue, &block_map_remote->queue_item);
    uvm_spin_unlock(&g_tools_channel_list_lock);
}

void uvm_tools_record_map_remote(uvm_va_block_t *va_block,
                                 uvm_push_t *push,
                                 uvm_processor_id_t processor,
                                 uvm_processor_id_t residency,
                                 NvU64 address,
                                 size_t region_size,
                                 UvmEventMapRemoteCause cause)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);

    UVM_ASSERT(UVM_ID_IS_VALID(processor));
    UVM_ASSERT(UVM_ID_IS_VALID(residency));
    UVM_ASSERT(cause != UvmEventMapRemoteCauseInvalid);

    uvm_assert_rwsem_locked(&va_space->lock);

    if (!va_space->tools.enabled)
        return;

    uvm_down_read(&va_space->tools.lock);

    if (UVM_ID_IS_CPU(processor)) {
        if (tools_is_event_enabled_v1(va_space, UvmEventTypeMapRemote)) {
            UvmEventEntry entry;

            memset(&entry, 0, sizeof(entry));

            entry.eventData.mapRemote.eventType      = UvmEventTypeMapRemote;
            entry.eventData.mapRemote.srcIndex       = uvm_parent_id_value_from_processor_id(processor);
            entry.eventData.mapRemote.dstIndex       = uvm_parent_id_value_from_processor_id(residency);
            entry.eventData.mapRemote.mapRemoteCause = cause;
            entry.eventData.mapRemote.timeStamp      = NV_GETTIME();
            entry.eventData.mapRemote.address        = address;
            entry.eventData.mapRemote.size           = region_size;
            entry.eventData.mapRemote.timeStampGpu   = 0;

            UVM_ASSERT(entry.eventData.mapRemote.mapRemoteCause != UvmEventMapRemoteCauseInvalid);

            uvm_tools_record_event(va_space, &entry);
        }
        if (tools_is_event_enabled_v2(va_space, UvmEventTypeMapRemote)) {
            UvmEventEntry_V2 entry;

            memset(&entry, 0, sizeof(entry));

            entry.eventData.mapRemote.eventType      = UvmEventTypeMapRemote;
            entry.eventData.mapRemote.srcIndex       = uvm_id_value(processor);
            entry.eventData.mapRemote.dstIndex       = uvm_id_value(residency);
            entry.eventData.mapRemote.mapRemoteCause = cause;
            entry.eventData.mapRemote.timeStamp      = NV_GETTIME();
            entry.eventData.mapRemote.address        = address;
            entry.eventData.mapRemote.size           = region_size;
            entry.eventData.mapRemote.timeStampGpu   = 0;

            UVM_ASSERT(entry.eventData.mapRemote.mapRemoteCause != UvmEventMapRemoteCauseInvalid);

            uvm_tools_record_event_v2(va_space, &entry);
        }
    }
    else if (tools_is_event_enabled(va_space, UvmEventTypeMapRemote)) {
        uvm_push_info_t *push_info = uvm_push_info_from_push(push);
        block_map_remote_data_t *block_map_remote;
        map_remote_data_t *map_remote;

        // The first call on this pushbuffer creates the per-VA block structure
        if (push_info->on_complete == NULL) {
            UVM_ASSERT(push_info->on_complete_data == NULL);

            block_map_remote = kmem_cache_alloc(g_tools_block_map_remote_data_cache, NV_UVM_GFP_FLAGS);
            if (block_map_remote == NULL)
                goto done;

            block_map_remote->src = processor;
            block_map_remote->dst = residency;
            block_map_remote->cause = cause;
            block_map_remote->timestamp = NV_GETTIME();
            block_map_remote->va_space = va_space;
            block_map_remote->channel = push->channel;
            INIT_LIST_HEAD(&block_map_remote->events);

            push_info->on_complete_data = block_map_remote;
            push_info->on_complete = on_map_remote_complete;

            uvm_spin_lock(&g_tools_channel_list_lock);
            add_pending_event_for_channel(block_map_remote->channel);
            uvm_spin_unlock(&g_tools_channel_list_lock);
        }
        else {
            block_map_remote = push_info->on_complete_data;
        }
        UVM_ASSERT(block_map_remote);

        map_remote = kmem_cache_alloc(g_tools_map_remote_data_cache, NV_UVM_GFP_FLAGS);
        if (map_remote == NULL)
            goto done;

        map_remote->address = address;
        map_remote->size = region_size;
        map_remote->timestamp_gpu_addr = uvm_push_timestamp(push);

        list_add_tail(&map_remote->events_node, &block_map_remote->events);
    }

done:
    uvm_up_read(&va_space->tools.lock);
}

static NV_STATUS create_event_tracker(UVM_TOOLS_INIT_EVENT_TRACKER_V2_PARAMS *params,
                                      size_t entry_size,
                                      struct file *filp)
{
    NV_STATUS status = NV_OK;
    uvm_tools_event_tracker_t *event_tracker;

    event_tracker = nv_kmem_cache_zalloc(g_tools_event_tracker_cache, NV_UVM_GFP_FLAGS);
    if (event_tracker == NULL)
        return NV_ERR_NO_MEMORY;

    event_tracker->entry_size = entry_size;

    event_tracker->uvm_file = fget(params->uvmFd);
    if (event_tracker->uvm_file == NULL) {
        status = NV_ERR_INSUFFICIENT_PERMISSIONS;
        goto fail;
    }

    if (!uvm_file_is_nvidia_uvm(event_tracker->uvm_file)) {
        fput(event_tracker->uvm_file);
        event_tracker->uvm_file = NULL;
        status = NV_ERR_INSUFFICIENT_PERMISSIONS;
        goto fail;
    }

    // We don't use uvm_fd_va_space() here because tools can work
    // without an associated va_space_mm.
    if (!uvm_fd_get_type(event_tracker->uvm_file, UVM_FD_VA_SPACE)) {
        fput(event_tracker->uvm_file);
        event_tracker->uvm_file = NULL;
        status = NV_ERR_ILLEGAL_ACTION;
        goto fail;
    }

    event_tracker->is_queue = params->queueBufferSize != 0;
    if (event_tracker->is_queue) {
        uvm_tools_queue_t *queue = &event_tracker->queue;
        NvU64 buffer_size;

        uvm_spin_lock_init(&queue->lock, UVM_LOCK_ORDER_LEAF);
        init_waitqueue_head(&queue->wait_queue);

        if (params->queueBufferSize > UINT_MAX) {
            status = NV_ERR_INVALID_ARGUMENT;
            goto fail;
        }

        queue->queue_buffer_count = (NvU32)params->queueBufferSize;
        queue->notification_threshold = queue->queue_buffer_count / 2;

        // queue_buffer_count must be a power of 2, of at least 2
        if (!is_power_of_2(queue->queue_buffer_count) || queue->queue_buffer_count < 2) {
            status = NV_ERR_INVALID_ARGUMENT;
            goto fail;
        }

        buffer_size = queue->queue_buffer_count * entry_size;

        status = map_user_pages(params->queueBuffer,
                                buffer_size,
                                &queue->queue_buffer,
                                &queue->queue_buffer_pages);
        if (status != NV_OK)
            goto fail;

        status = map_user_pages(params->controlBuffer,
                                sizeof(UvmToolsEventControlData),
                                (void **)&queue->control,
                                &queue->control_buffer_pages);

        if (status != NV_OK)
            goto fail;
    }
    else {
        uvm_tools_counter_t *counter = &event_tracker->counter;
        counter->all_processors = params->allProcessors;
        counter->processor = params->processor;
        status = map_user_pages(params->controlBuffer,
                                sizeof(NvU64) * UVM_TOTAL_COUNTERS,
                                (void **)&counter->counters,
                                &counter->counter_buffer_pages);
        if (status != NV_OK)
            goto fail;
    }

    if (atomic_long_cmpxchg((atomic_long_t *)&filp->private_data, 0, (long) event_tracker) != 0) {
        status = NV_ERR_INVALID_ARGUMENT;
        goto fail;
    }

    return NV_OK;

fail:
    destroy_event_tracker(event_tracker);
    return status;
}

NV_STATUS uvm_api_tools_init_event_tracker(UVM_TOOLS_INIT_EVENT_TRACKER_PARAMS *params, struct file *filp)
{
    UVM_TOOLS_INIT_EVENT_TRACKER_V2_PARAMS *params_v2 = (UVM_TOOLS_INIT_EVENT_TRACKER_V2_PARAMS *) params;

    BUILD_BUG_ON(!__same_type(params, params_v2));

    return create_event_tracker(params_v2, sizeof(UvmEventEntry), filp);
}

NV_STATUS uvm_api_tools_init_event_tracker_v2(UVM_TOOLS_INIT_EVENT_TRACKER_V2_PARAMS *params, struct file *filp)
{
    return create_event_tracker(params, sizeof(UvmEventEntry_V2), filp);
}

NV_STATUS uvm_api_tools_set_notification_threshold(UVM_TOOLS_SET_NOTIFICATION_THRESHOLD_PARAMS *params, struct file *filp)
{
    uvm_tools_queue_snapshot_t sn;
    uvm_tools_event_tracker_t *event_tracker = tools_event_tracker(filp);
    UvmToolsEventControlData *ctrl;

    if (!tracker_is_queue(event_tracker))
        return NV_ERR_INVALID_ARGUMENT;

    uvm_spin_lock(&event_tracker->queue.lock);

    event_tracker->queue.notification_threshold = params->notificationThreshold;

    ctrl = event_tracker->queue.control;
    sn.put_behind = atomic_read((atomic_t *)&ctrl->put_behind);
    sn.get_ahead = atomic_read((atomic_t *)&ctrl->get_ahead);

    if (queue_needs_wakeup(&event_tracker->queue, &sn))
        wake_up_all(&event_tracker->queue.wait_queue);

    uvm_spin_unlock(&event_tracker->queue.lock);

    return NV_OK;
}

static NV_STATUS tools_update_perf_events_callbacks(uvm_va_space_t *va_space)
{
    NV_STATUS status;

    uvm_assert_rwsem_locked_write(&va_space->perf_events.lock);
    uvm_assert_rwsem_locked_write(&va_space->tools.lock);

    if (tools_is_fault_callback_needed(va_space)) {
        if (!uvm_perf_is_event_callback_registered(&va_space->perf_events, UVM_PERF_EVENT_FAULT, uvm_tools_record_fault)) {
            status = uvm_perf_register_event_callback_locked(&va_space->perf_events,
                                                             UVM_PERF_EVENT_FAULT,
                                                             uvm_tools_record_fault);

            if (status != NV_OK)
                return status;
        }
    }
    else {
        if (uvm_perf_is_event_callback_registered(&va_space->perf_events, UVM_PERF_EVENT_FAULT, uvm_tools_record_fault)) {
            uvm_perf_unregister_event_callback_locked(&va_space->perf_events,
                                                      UVM_PERF_EVENT_FAULT,
                                                      uvm_tools_record_fault);
        }
    }

    if (tools_is_migration_callback_needed(va_space)) {
        if (!uvm_perf_is_event_callback_registered(&va_space->perf_events, UVM_PERF_EVENT_MIGRATION, uvm_tools_record_migration)) {
            status = uvm_perf_register_event_callback_locked(&va_space->perf_events,
                                                             UVM_PERF_EVENT_MIGRATION,
                                                             uvm_tools_record_migration);

            if (status != NV_OK)
                return status;
        }
    }
    else {
        if (uvm_perf_is_event_callback_registered(&va_space->perf_events, UVM_PERF_EVENT_MIGRATION, uvm_tools_record_migration)) {
            uvm_perf_unregister_event_callback_locked(&va_space->perf_events,
                                                      UVM_PERF_EVENT_MIGRATION,
                                                      uvm_tools_record_migration);
        }
    }

    return NV_OK;
}

static NV_STATUS tools_update_status(uvm_va_space_t *va_space)
{
    NV_STATUS status;
    bool should_be_enabled;
    uvm_assert_rwsem_locked_write(&g_tools_va_space_list_lock);
    uvm_assert_rwsem_locked_write(&va_space->perf_events.lock);
    uvm_assert_rwsem_locked_write(&va_space->tools.lock);

    status = tools_update_perf_events_callbacks(va_space);
    if (status != NV_OK)
        return status;

    should_be_enabled = tools_are_enabled(va_space);
    if (should_be_enabled != va_space->tools.enabled) {
        if (should_be_enabled)
            list_add(&va_space->tools.node, &g_tools_va_space_list);
        else
            list_del(&va_space->tools.node);

        va_space->tools.enabled = should_be_enabled;
    }

    return NV_OK;
}

#define EVENT_FLAGS_BITS (sizeof(NvU64) * 8)

static bool mask_contains_invalid_events(NvU64 event_flags)
{
    const unsigned long *event_mask = (const unsigned long *)&event_flags;
    DECLARE_BITMAP(helper_mask, EVENT_FLAGS_BITS);
    DECLARE_BITMAP(valid_events_mask, EVENT_FLAGS_BITS);
    DECLARE_BITMAP(tests_events_mask, EVENT_FLAGS_BITS);

    bitmap_zero(tests_events_mask, EVENT_FLAGS_BITS);
    bitmap_set(tests_events_mask,
               UvmEventTestTypesFirst,
               UvmEventTestTypesLast - UvmEventTestTypesFirst + 1);

    bitmap_zero(valid_events_mask, EVENT_FLAGS_BITS);
    bitmap_set(valid_events_mask, 1, UvmEventNumTypes - 1);

    if (uvm_enable_builtin_tests)
        bitmap_or(valid_events_mask, valid_events_mask, tests_events_mask, EVENT_FLAGS_BITS);

    // Make sure that test event ids do not overlap with regular events
    BUILD_BUG_ON(UvmEventTestTypesFirst < UvmEventNumTypes);
    BUILD_BUG_ON(UvmEventTestTypesFirst > UvmEventTestTypesLast);
    BUILD_BUG_ON(UvmEventTestTypesLast >= UvmEventNumTypesAll);

    // Make sure that no test event ever changes the size of UvmEventEntry_V2
    BUILD_BUG_ON(sizeof(((UvmEventEntry_V2 *)NULL)->testEventData) >
                 sizeof(((UvmEventEntry_V2 *)NULL)->eventData));
    BUILD_BUG_ON(UvmEventNumTypesAll > EVENT_FLAGS_BITS);

    if (!bitmap_andnot(helper_mask, event_mask, valid_events_mask, EVENT_FLAGS_BITS))
        return false;

    if (!uvm_enable_builtin_tests && bitmap_and(helper_mask, event_mask, tests_events_mask, EVENT_FLAGS_BITS))
        UVM_INFO_PRINT("Event index not found. Did you mean to insmod with uvm_enable_builtin_tests=1?\n");

    return true;
}

NV_STATUS uvm_api_tools_event_queue_enable_events(UVM_TOOLS_EVENT_QUEUE_ENABLE_EVENTS_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space;
    uvm_tools_event_tracker_t *event_tracker = tools_event_tracker(filp);
    NV_STATUS status = NV_OK;
    NvU64 inserted_lists;

    if (!tracker_is_queue(event_tracker))
        return NV_ERR_INVALID_ARGUMENT;

    if (mask_contains_invalid_events(params->eventTypeFlags))
        return NV_ERR_INVALID_ARGUMENT;

    va_space = tools_event_tracker_va_space(event_tracker);

    uvm_down_write(&g_tools_va_space_list_lock);
    uvm_down_write(&va_space->perf_events.lock);
    uvm_down_write(&va_space->tools.lock);

    insert_event_tracker(va_space,
                         event_tracker->queue.queue_nodes,
                         UvmEventNumTypesAll,
                         params->eventTypeFlags,
                         &event_tracker->queue.subscribed_queues,
                         event_tracker->entry_size == sizeof(UvmEventEntry) ?
                                                      va_space->tools.queues : va_space->tools.queues_v2,
                         &inserted_lists);

    // perform any necessary registration
    status = tools_update_status(va_space);
    if (status != NV_OK) {
        // on error, unregister any newly registered event
        remove_event_tracker(va_space,
                             event_tracker->queue.queue_nodes,
                             UvmEventNumTypes,
                             inserted_lists,
                             &event_tracker->queue.subscribed_queues);
    }

    uvm_up_write(&va_space->tools.lock);
    uvm_up_write(&va_space->perf_events.lock);
    uvm_up_write(&g_tools_va_space_list_lock);

    return status;
}

NV_STATUS uvm_api_tools_event_queue_disable_events(UVM_TOOLS_EVENT_QUEUE_DISABLE_EVENTS_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_va_space_t *va_space;
    uvm_tools_event_tracker_t *event_tracker = tools_event_tracker(filp);

    if (!tracker_is_queue(event_tracker))
        return NV_ERR_INVALID_ARGUMENT;

    va_space = tools_event_tracker_va_space(event_tracker);

    uvm_down_write(&g_tools_va_space_list_lock);
    uvm_down_write(&va_space->perf_events.lock);
    uvm_down_write(&va_space->tools.lock);
    remove_event_tracker(va_space,
                         event_tracker->queue.queue_nodes,
                         UvmEventNumTypesAll,
                         params->eventTypeFlags,
                         &event_tracker->queue.subscribed_queues);

    // de-registration should not fail
    status = tools_update_status(va_space);
    UVM_ASSERT(status == NV_OK);

    uvm_up_write(&va_space->tools.lock);
    uvm_up_write(&va_space->perf_events.lock);
    uvm_up_write(&g_tools_va_space_list_lock);
    return NV_OK;
}

NV_STATUS uvm_api_tools_enable_counters(UVM_TOOLS_ENABLE_COUNTERS_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space;
    uvm_tools_event_tracker_t *event_tracker = tools_event_tracker(filp);
    NV_STATUS status = NV_OK;
    NvU64 inserted_lists;

    if (!tracker_is_counter(event_tracker))
        return NV_ERR_INVALID_ARGUMENT;

    va_space = tools_event_tracker_va_space(event_tracker);

    uvm_down_write(&g_tools_va_space_list_lock);
    uvm_down_write(&va_space->perf_events.lock);
    uvm_down_write(&va_space->tools.lock);

    insert_event_tracker(va_space,
                         event_tracker->counter.counter_nodes,
                         UVM_TOTAL_COUNTERS,
                         params->counterTypeFlags,
                         &event_tracker->counter.subscribed_counters,
                         va_space->tools.counters,
                         &inserted_lists);

    // perform any necessary registration
    status = tools_update_status(va_space);
    if (status != NV_OK) {
        remove_event_tracker(va_space,
                             event_tracker->counter.counter_nodes,
                             UVM_TOTAL_COUNTERS,
                             inserted_lists,
                             &event_tracker->counter.subscribed_counters);
    }

    uvm_up_write(&va_space->tools.lock);
    uvm_up_write(&va_space->perf_events.lock);
    uvm_up_write(&g_tools_va_space_list_lock);

    return status;
}

NV_STATUS uvm_api_tools_disable_counters(UVM_TOOLS_DISABLE_COUNTERS_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_va_space_t *va_space;
    uvm_tools_event_tracker_t *event_tracker = tools_event_tracker(filp);

    if (!tracker_is_counter(event_tracker))
        return NV_ERR_INVALID_ARGUMENT;

    va_space = tools_event_tracker_va_space(event_tracker);

    uvm_down_write(&g_tools_va_space_list_lock);
    uvm_down_write(&va_space->perf_events.lock);
    uvm_down_write(&va_space->tools.lock);
    remove_event_tracker(va_space,
                         event_tracker->counter.counter_nodes,
                         UVM_TOTAL_COUNTERS,
                         params->counterTypeFlags,
                         &event_tracker->counter.subscribed_counters);

    // de-registration should not fail
    status = tools_update_status(va_space);
    UVM_ASSERT(status == NV_OK);

    uvm_up_write(&va_space->tools.lock);
    uvm_up_write(&va_space->perf_events.lock);
    uvm_up_write(&g_tools_va_space_list_lock);

    return NV_OK;
}

static NV_STATUS tools_access_va_block(uvm_va_block_t *va_block,
                                       uvm_va_block_context_t *block_context,
                                       NvU64 target_va,
                                       NvU64 size,
                                       bool is_write,
                                       uvm_mem_t *stage_mem)
{
    if (is_write) {
        return UVM_VA_BLOCK_LOCK_RETRY(va_block,
                                       NULL,
                                       uvm_va_block_write_from_cpu(va_block, block_context, target_va, stage_mem, size));
    }
    else {
        return UVM_VA_BLOCK_LOCK_RETRY(va_block,
                                       NULL,
                                       uvm_va_block_read_to_cpu(va_block, block_context, stage_mem, target_va, size));

    }
}

static NV_STATUS tools_access_process_memory(uvm_va_space_t *va_space,
                                             NvU64 target_va,
                                             NvU64 size,
                                             NvU64 user_va,
                                             NvU64 *bytes,
                                             bool is_write)
{
    NV_STATUS status;
    uvm_mem_t *stage_mem = NULL;
    void *stage_addr;
    uvm_processor_mask_t *retained_gpus = NULL;
    uvm_va_block_context_t *block_context = NULL;
    struct mm_struct *mm = NULL;

    retained_gpus = uvm_processor_mask_cache_alloc();
    if (!retained_gpus)
        return NV_ERR_NO_MEMORY;

    uvm_processor_mask_zero(retained_gpus);

    mm = uvm_va_space_mm_or_current_retain(va_space);

    status = uvm_mem_alloc_sysmem_and_map_cpu_kernel(PAGE_SIZE, mm, &stage_mem);
    if (status != NV_OK)
        goto exit;

    block_context = uvm_va_block_context_alloc(mm);
    if (!block_context) {
        status = NV_ERR_NO_MEMORY;
        goto exit;
    }

    stage_addr = uvm_mem_get_cpu_addr_kernel(stage_mem);
    *bytes = 0;

    while (*bytes < size) {
        uvm_gpu_t *gpu;
        uvm_va_block_t *block;
        void *user_va_start = (void *) (user_va + *bytes);
        NvU64 target_va_start = target_va + *bytes;
        NvU64 bytes_left = size - *bytes;
        NvU64 page_offset = target_va_start & (PAGE_SIZE - 1);
        NvU64 bytes_now = min(bytes_left, (NvU64)(PAGE_SIZE - page_offset));

        if (is_write) {
            NvU64 remaining = copy_from_user(stage_addr, user_va_start, bytes_now);
            if (remaining != 0)  {
                status = NV_ERR_INVALID_ARGUMENT;
                goto exit;
            }
        }

        if (mm)
            uvm_down_read_mmap_lock(mm);

        uvm_va_space_down_read(va_space);

        if (mm)
            status = uvm_va_block_find_create(va_space, UVM_PAGE_ALIGN_DOWN(target_va_start), &block_context->hmm.vma, &block);
        else
            status = uvm_va_block_find_create_managed(va_space, UVM_PAGE_ALIGN_DOWN(target_va_start), &block);

        if (status != NV_OK)
            goto unlock_and_exit;

        for_each_gpu_in_mask(gpu, &va_space->registered_gpus) {
            if (uvm_processor_mask_test_and_set(retained_gpus, gpu->id))
                continue;

            // The retention of each GPU ensures that the staging memory is
            // freed before the unregistration of any of the GPUs is mapped
            // on. Each GPU is retained once.
            uvm_gpu_retain(gpu);

            // In Confidential Computing, the staging memory cannot be mapped on
            // the GPU (it is protected sysmem), but it is still used to store
            // the unencrypted version of the page contents when the page is
            // resident on vidmem.
            if (g_uvm_global.conf_computing_enabled)
                continue;

            // Accessing the VA block may result in copying data between the
            // CPU and a GPU. Conservatively add virtual mappings to all the
            // GPUs (even if those mappings may never be used) as tools
            // read/write is not on a performance critical path.
            status = uvm_mem_map_gpu_kernel(stage_mem, gpu);
            if (status != NV_OK)
                goto unlock_and_exit;
        }

        // Make sure a CPU resident page has an up to date struct page pointer.
        if (uvm_va_block_is_hmm(block)) {
            status = uvm_hmm_va_block_update_residency_info(block, mm, UVM_PAGE_ALIGN_DOWN(target_va_start), true);
            if (status != NV_OK)
                goto unlock_and_exit;
        }

        status = tools_access_va_block(block, block_context, target_va_start, bytes_now, is_write, stage_mem);

        uvm_va_space_up_read(va_space);
        if (mm)
            uvm_up_read_mmap_lock(mm);

        // Check for ECC errors on all retained GPUs, even if they are no longer
        // registered in the VA space.
        if (status == NV_OK)
            status = uvm_global_gpu_check_ecc_error(retained_gpus);

        if (status != NV_OK)
            goto exit;

        if (!is_write) {
            NvU64 remaining;

            // Prevent processor speculation prior to accessing user-mapped
            // memory to avoid leaking information from side-channel attacks.
            // Under speculation, a valid VA range which does not contain
            // target_va could be used, and the block index could run off the
            // end of the array. Information about the state of that kernel
            // memory could be inferred if speculative execution gets to the
            // point where the data is copied out.
            nv_speculation_barrier();

            remaining = copy_to_user(user_va_start, stage_addr, bytes_now);
            if (remaining > 0) {
                status = NV_ERR_INVALID_ARGUMENT;
                goto exit;
            }
        }

        *bytes += bytes_now;
    }

unlock_and_exit:
    if (status != NV_OK) {
        uvm_va_space_up_read(va_space);
        if (mm)
            uvm_up_read_mmap_lock(mm);
    }

exit:
    uvm_va_block_context_free(block_context);

    uvm_mem_free(stage_mem);

    uvm_global_gpu_release(retained_gpus);

    uvm_va_space_mm_or_current_release(va_space, mm);

    uvm_processor_mask_cache_free(retained_gpus);

    return status;
}

NV_STATUS uvm_api_tools_read_process_memory(UVM_TOOLS_READ_PROCESS_MEMORY_PARAMS *params, struct file *filp)
{
    return tools_access_process_memory(uvm_va_space_get(filp),
                                       params->targetVa,
                                       params->size,
                                       params->buffer,
                                       &params->bytesRead,
                                       false);
}

NV_STATUS uvm_api_tools_write_process_memory(UVM_TOOLS_WRITE_PROCESS_MEMORY_PARAMS *params, struct file *filp)
{
    return tools_access_process_memory(uvm_va_space_get(filp),
                                       params->targetVa,
                                       params->size,
                                       params->buffer,
                                       &params->bytesWritten,
                                       true);
}

NV_STATUS uvm_test_inject_tools_event(UVM_TEST_INJECT_TOOLS_EVENT_PARAMS *params, struct file *filp)
{
    NvU32 i;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    if (params->entry.eventData.eventType >= UvmEventNumTypesAll)
        return NV_ERR_INVALID_ARGUMENT;

    uvm_down_read(&va_space->tools.lock);

    for (i = 0; i < params->count; i++)
        uvm_tools_record_event(va_space, &params->entry);

    uvm_up_read(&va_space->tools.lock);

    return NV_OK;
}

NV_STATUS uvm_test_inject_tools_event_v2(UVM_TEST_INJECT_TOOLS_EVENT_V2_PARAMS *params, struct file *filp)
{
    NvU32 i;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    if (params->entry_v2.eventData.eventType >= UvmEventNumTypesAll)
        return NV_ERR_INVALID_ARGUMENT;

    uvm_down_read(&va_space->tools.lock);

    for (i = 0; i < params->count; i++)
        uvm_tools_record_event_v2(va_space, &params->entry_v2);

    uvm_up_read(&va_space->tools.lock);

    return NV_OK;
}

NV_STATUS uvm_test_increment_tools_counter(UVM_TEST_INCREMENT_TOOLS_COUNTER_PARAMS *params, struct file *filp)
{
    NvU32 i;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    if (params->counter >= UVM_TOTAL_COUNTERS)
        return NV_ERR_INVALID_ARGUMENT;

    uvm_down_read(&va_space->tools.lock);
    for (i = 0; i < params->count; i++)
        uvm_tools_inc_counter(va_space, params->counter, params->amount, &params->processor);
    uvm_up_read(&va_space->tools.lock);

    return NV_OK;
}

static NV_STATUS uvm_tools_get_processor_uuid_table_common(UVM_TOOLS_GET_PROCESSOR_UUID_TABLE_V2_PARAMS *params,
                                                           uvm_va_space_t *va_space,
                                                           NvU32 max_processors_count)
{
    NvProcessorUuid *uuids;
    NvU64 remaining;
    uvm_gpu_t *gpu;

    uuids = uvm_kvmalloc_zero(sizeof(NvProcessorUuid) * max_processors_count);
    if (uuids == NULL)
        return NV_ERR_NO_MEMORY;

    uvm_uuid_copy(&uuids[UVM_ID_CPU_VALUE], &NV_PROCESSOR_UUID_CPU_DEFAULT);

    uvm_va_space_down_read(va_space);

    for_each_va_space_gpu(gpu, va_space) {
        NvU32 id_value;
        const NvProcessorUuid *uuid;

        // Version 1 only supports processors 0..32 and uses the parent
        // GPU UUID.
        if (max_processors_count == UVM_MAX_PROCESSORS_V1) {
            id_value = uvm_parent_id_value(gpu->parent->id);
            uuid = &gpu->parent->uuid;
        }
        else {
            id_value = uvm_id_value(gpu->id);
            uuid = &gpu->uuid;
        }

        uvm_uuid_copy(&uuids[id_value], uuid);
    }

    uvm_va_space_up_read(va_space);

    remaining = copy_to_user((void *)params->tablePtr, uuids, sizeof(NvProcessorUuid) * max_processors_count);

    uvm_kvfree(uuids);

    if (remaining != 0)
        return NV_ERR_INVALID_ADDRESS;

    return NV_OK;
}

NV_STATUS uvm_api_tools_get_processor_uuid_table(UVM_TOOLS_GET_PROCESSOR_UUID_TABLE_PARAMS *params, struct file *filp)
{
    UVM_TOOLS_GET_PROCESSOR_UUID_TABLE_V2_PARAMS *params_v2 = (UVM_TOOLS_GET_PROCESSOR_UUID_TABLE_V2_PARAMS *) params;

    BUILD_BUG_ON(!__same_type(params, params_v2));

    return uvm_tools_get_processor_uuid_table_common(params_v2, uvm_va_space_get(filp), UVM_MAX_PROCESSORS_V1);
}

NV_STATUS uvm_api_tools_get_processor_uuid_table_v2(UVM_TOOLS_GET_PROCESSOR_UUID_TABLE_V2_PARAMS *params,
                                                    struct file *filp)
{
    return uvm_tools_get_processor_uuid_table_common(params, uvm_va_space_get(filp), UVM_ID_MAX_PROCESSORS);
}

void uvm_tools_flush_events(void)
{
    tools_schedule_completed_events();

    nv_kthread_q_flush(&g_tools_queue);
}

NV_STATUS uvm_api_tools_flush_events(UVM_TOOLS_FLUSH_EVENTS_PARAMS *params, struct file *filp)
{
    uvm_tools_flush_events();
    return NV_OK;
}

NV_STATUS uvm_test_tools_flush_replay_events(UVM_TEST_TOOLS_FLUSH_REPLAY_EVENTS_PARAMS *params, struct file *filp)
{
    NV_STATUS status = NV_OK;
    uvm_gpu_t *gpu = NULL;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    gpu = uvm_va_space_retain_gpu_by_uuid(va_space, &params->gpuUuid);
    if (!gpu)
        return NV_ERR_INVALID_DEVICE;

    // Wait for register-based fault clears to queue the replay event
    if (!gpu->parent->has_clear_faulted_channel_method) {
        uvm_parent_gpu_non_replayable_faults_isr_lock(gpu->parent);
        uvm_parent_gpu_non_replayable_faults_isr_unlock(gpu->parent);
    }

    // Wait for pending fault replay methods to complete (replayable faults on
    // all GPUs, and non-replayable faults on method-based GPUs).
    status = uvm_channel_manager_wait(gpu->channel_manager);

    // Flush any pending events even if (status != NV_OK)
    uvm_tools_flush_events();
    uvm_gpu_release(gpu);

    return status;
}

static const struct file_operations uvm_tools_fops =
{
    .open            = uvm_tools_open_entry,
    .release         = uvm_tools_release_entry,
    .unlocked_ioctl  = uvm_tools_unlocked_ioctl_entry,
#if NVCPU_IS_X86_64
    .compat_ioctl    = uvm_tools_unlocked_ioctl_entry,
#endif
    .poll            = uvm_tools_poll_entry,
    .owner           = THIS_MODULE,
};

static void _uvm_tools_destroy_cache_all(void)
{
    // The pointers are initialized to NULL,
    // it's safe to call destroy on all of them.
    kmem_cache_destroy_safe(&g_tools_event_tracker_cache);
    kmem_cache_destroy_safe(&g_tools_block_migration_data_cache);
    kmem_cache_destroy_safe(&g_tools_migration_data_cache);
    kmem_cache_destroy_safe(&g_tools_replay_data_cache);
    kmem_cache_destroy_safe(&g_tools_block_map_remote_data_cache);
    kmem_cache_destroy_safe(&g_tools_map_remote_data_cache);
}

int uvm_tools_init(dev_t uvm_base_dev)
{
    dev_t uvm_tools_dev = MKDEV(MAJOR(uvm_base_dev), NVIDIA_UVM_TOOLS_MINOR_NUMBER);
    int ret = -ENOMEM; // This will be updated later if allocations succeed

    uvm_init_rwsem(&g_tools_va_space_list_lock, UVM_LOCK_ORDER_TOOLS_VA_SPACE_LIST);

    g_tools_event_tracker_cache = NV_KMEM_CACHE_CREATE("uvm_tools_event_tracker_t",
                                                        uvm_tools_event_tracker_t);
    if (!g_tools_event_tracker_cache)
        goto err_cache_destroy;

    g_tools_block_migration_data_cache = NV_KMEM_CACHE_CREATE("uvm_tools_block_migration_data_t",
                                                              block_migration_data_t);
    if (!g_tools_block_migration_data_cache)
        goto err_cache_destroy;

    g_tools_migration_data_cache = NV_KMEM_CACHE_CREATE("uvm_tools_migration_data_t",
                                                        migration_data_t);
    if (!g_tools_migration_data_cache)
        goto err_cache_destroy;

    g_tools_replay_data_cache = NV_KMEM_CACHE_CREATE("uvm_tools_replay_data_t",
                                                     replay_data_t);
    if (!g_tools_replay_data_cache)
        goto err_cache_destroy;

    g_tools_block_map_remote_data_cache = NV_KMEM_CACHE_CREATE("uvm_tools_block_map_remote_data_t",
                                                               block_map_remote_data_t);
    if (!g_tools_block_map_remote_data_cache)
        goto err_cache_destroy;

    g_tools_map_remote_data_cache = NV_KMEM_CACHE_CREATE("uvm_tools_map_remote_data_t",
                                                         map_remote_data_t);
    if (!g_tools_map_remote_data_cache)
        goto err_cache_destroy;

    uvm_spin_lock_init(&g_tools_channel_list_lock, UVM_LOCK_ORDER_LEAF);

    ret = nv_kthread_q_init(&g_tools_queue, "UVM Tools Event Queue");
    if (ret < 0)
        goto err_cache_destroy;

    uvm_init_character_device(&g_uvm_tools_cdev, &uvm_tools_fops);
    ret = cdev_add(&g_uvm_tools_cdev, uvm_tools_dev, 1);
    if (ret != 0) {
        UVM_ERR_PRINT("cdev_add (major %u, minor %u) failed: %d\n", MAJOR(uvm_tools_dev),
                      MINOR(uvm_tools_dev), ret);
        goto err_stop_thread;
    }

    return ret;

err_stop_thread:
    nv_kthread_q_stop(&g_tools_queue);

err_cache_destroy:
    _uvm_tools_destroy_cache_all();
    return ret;
}

void uvm_tools_exit(void)
{
    unsigned i;
    cdev_del(&g_uvm_tools_cdev);

    nv_kthread_q_stop(&g_tools_queue);

    for (i = 0; i < UvmEventNumTypesAll; ++i)
        UVM_ASSERT(g_tools_enabled_event_count[i] == 0);

    UVM_ASSERT(list_empty(&g_tools_va_space_list));

    _uvm_tools_destroy_cache_all();
}
