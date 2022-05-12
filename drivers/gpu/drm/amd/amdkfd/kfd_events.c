// SPDX-License-Identifier: GPL-2.0 OR MIT
/*
 * Copyright 2014-2022 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <linux/mm_types.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/sched/signal.h>
#include <linux/sched/mm.h>
#include <linux/uaccess.h>
#include <linux/mman.h>
#include <linux/memory.h>
#include "kfd_priv.h"
#include "kfd_events.h"
#include "kfd_iommu.h"
#include <linux/device.h>

/*
 * Wrapper around wait_queue_entry_t
 */
struct kfd_event_waiter {
#if defined(HAVE_WAIT_QUEUE_ENTRY)
	wait_queue_entry_t wait;
#else
	wait_queue_t wait;
#endif
	struct kfd_event *event; /* Event to wait for */
	bool activated;		 /* Becomes true when event is signaled */
};

/*
 * Each signal event needs a 64-bit signal slot where the signaler will write
 * a 1 before sending an interrupt. (This is needed because some interrupts
 * do not contain enough spare data bits to identify an event.)
 * We get whole pages and map them to the process VA.
 * Individual signal events use their event_id as slot index.
 */
struct kfd_signal_page {
	uint64_t *kernel_address;
	uint64_t __user *user_address;
	bool need_to_free_pages;
};

static uint64_t *page_slots(struct kfd_signal_page *page)
{
	return page->kernel_address;
}

static struct kfd_signal_page *allocate_signal_page(struct kfd_process *p)
{
	void *backing_store;
	struct kfd_signal_page *page;

	page = kzalloc(sizeof(*page), GFP_KERNEL);
	if (!page)
		return NULL;

	backing_store = (void *) __get_free_pages(GFP_KERNEL,
					get_order(KFD_SIGNAL_EVENT_LIMIT * 8));
	if (!backing_store)
		goto fail_alloc_signal_store;

	/* Initialize all events to unsignaled */
	memset(backing_store, (uint8_t) UNSIGNALED_EVENT_SLOT,
	       KFD_SIGNAL_EVENT_LIMIT * 8);

	page->kernel_address = backing_store;
	page->need_to_free_pages = true;
	pr_debug("Allocated new event signal page at %p, for process %p\n",
			page, p);

	return page;

fail_alloc_signal_store:
	kfree(page);
	return NULL;
}

static int allocate_event_notification_slot(struct kfd_process *p,
					    struct kfd_event *ev,
					    const int *restore_id)
{
	int id;

	if (!p->signal_page) {
		p->signal_page = allocate_signal_page(p);
		if (!p->signal_page)
			return -ENOMEM;
		/* Oldest user mode expects 256 event slots */
		p->signal_mapped_size = 256*8;
	}

	if (restore_id) {
		id = idr_alloc(&p->event_idr, ev, *restore_id, *restore_id + 1,
				GFP_KERNEL);
	} else {
		/*
		 * Compatibility with old user mode: Only use signal slots
		 * user mode has mapped, may be less than
		 * KFD_SIGNAL_EVENT_LIMIT. This also allows future increase
		 * of the event limit without breaking user mode.
		 */
		id = idr_alloc(&p->event_idr, ev, 0, p->signal_mapped_size / 8,
				GFP_KERNEL);
	}
	if (id < 0)
		return id;

	ev->event_id = id;
	page_slots(p->signal_page)[id] = UNSIGNALED_EVENT_SLOT;

	return 0;
}

/*
 * Assumes that p->event_mutex is held and of course that p is not going
 * away (current or locked).
 */
static struct kfd_event *lookup_event_by_id(struct kfd_process *p, uint32_t id)
{
	return idr_find(&p->event_idr, id);
}

/**
 * lookup_signaled_event_by_partial_id - Lookup signaled event from partial ID
 * @p:     Pointer to struct kfd_process
 * @id:    ID to look up
 * @bits:  Number of valid bits in @id
 *
 * Finds the first signaled event with a matching partial ID. If no
 * matching signaled event is found, returns NULL. In that case the
 * caller should assume that the partial ID is invalid and do an
 * exhaustive search of all siglaned events.
 *
 * If multiple events with the same partial ID signal at the same
 * time, they will be found one interrupt at a time, not necessarily
 * in the same order the interrupts occurred. As long as the number of
 * interrupts is correct, all signaled events will be seen by the
 * driver.
 */
static struct kfd_event *lookup_signaled_event_by_partial_id(
	struct kfd_process *p, uint32_t id, uint32_t bits)
{
	struct kfd_event *ev;

	if (!p->signal_page || id >= KFD_SIGNAL_EVENT_LIMIT)
		return NULL;

	/* Fast path for the common case that @id is not a partial ID
	 * and we only need a single lookup.
	 */
	if (bits > 31 || (1U << bits) >= KFD_SIGNAL_EVENT_LIMIT) {
		if (page_slots(p->signal_page)[id] == UNSIGNALED_EVENT_SLOT)
			return NULL;

		return idr_find(&p->event_idr, id);
	}

	/* General case for partial IDs: Iterate over all matching IDs
	 * and find the first one that has signaled.
	 */
	for (ev = NULL; id < KFD_SIGNAL_EVENT_LIMIT && !ev; id += 1U << bits) {
		if (page_slots(p->signal_page)[id] == UNSIGNALED_EVENT_SLOT)
			continue;

		ev = idr_find(&p->event_idr, id);
	}

	return ev;
}

static int create_signal_event(struct file *devkfd, struct kfd_process *p,
				struct kfd_event *ev, const int *restore_id)
{
	int ret;

	if (p->signal_mapped_size &&
	    p->signal_event_count == p->signal_mapped_size / 8) {
		if (!p->signal_event_limit_reached) {
			pr_debug("Signal event wasn't created because limit was reached\n");
			p->signal_event_limit_reached = true;
		}
		return -ENOSPC;
	}

	ret = allocate_event_notification_slot(p, ev, restore_id);
	if (ret) {
		pr_warn("Signal event wasn't created because out of kernel memory\n");
		return ret;
	}

	p->signal_event_count++;

	ev->user_signal_address = &p->signal_page->user_address[ev->event_id];
	pr_debug("Signal event number %zu created with id %d, address %p\n",
			p->signal_event_count, ev->event_id,
			ev->user_signal_address);

	return 0;
}

static int create_other_event(struct kfd_process *p, struct kfd_event *ev, const int *restore_id)
{
	int id;

	if (restore_id)
		id = idr_alloc(&p->event_idr, ev, *restore_id, *restore_id + 1,
			GFP_KERNEL);
	else
		/* Cast KFD_LAST_NONSIGNAL_EVENT to uint32_t. This allows an
		 * intentional integer overflow to -1 without a compiler
		 * warning. idr_alloc treats a negative value as "maximum
		 * signed integer".
		 */
		id = idr_alloc(&p->event_idr, ev, KFD_FIRST_NONSIGNAL_EVENT_ID,
				(uint32_t)KFD_LAST_NONSIGNAL_EVENT_ID + 1,
				GFP_KERNEL);

	if (id < 0)
		return id;
	ev->event_id = id;

	return 0;
}

void kfd_event_init_process(struct kfd_process *p)
{
	mutex_init(&p->event_mutex);
	idr_init(&p->event_idr);
	p->signal_page = NULL;
	p->signal_event_count = 0;
}

static void destroy_event(struct kfd_process *p, struct kfd_event *ev)
{
	struct kfd_event_waiter *waiter;

	/* Wake up pending waiters. They will return failure */
#if !defined(HAVE_WAIT_QUEUE_ENTRY)
	list_for_each_entry(waiter, &ev->wq.task_list, wait.task_list)
#else
	list_for_each_entry(waiter, &ev->wq.head, wait.entry)
#endif
		waiter->event = NULL;
	wake_up_all(&ev->wq);

	if (ev->type == KFD_EVENT_TYPE_SIGNAL ||
	    ev->type == KFD_EVENT_TYPE_DEBUG)
		p->signal_event_count--;

	idr_remove(&p->event_idr, ev->event_id);
	kfree(ev);
}

static void destroy_events(struct kfd_process *p)
{
	struct kfd_event *ev;
	uint32_t id;

	idr_for_each_entry(&p->event_idr, ev, id)
		destroy_event(p, ev);
	idr_destroy(&p->event_idr);
}

/*
 * We assume that the process is being destroyed and there is no need to
 * unmap the pages or keep bookkeeping data in order.
 */
static void shutdown_signal_page(struct kfd_process *p)
{
	struct kfd_signal_page *page = p->signal_page;

	if (page) {
		if (page->need_to_free_pages)
			free_pages((unsigned long)page->kernel_address,
				   get_order(KFD_SIGNAL_EVENT_LIMIT * 8));
		kfree(page);
	}
}

void kfd_event_free_process(struct kfd_process *p)
{
	destroy_events(p);
	shutdown_signal_page(p);
}

static bool event_can_be_gpu_signaled(const struct kfd_event *ev)
{
	return ev->type == KFD_EVENT_TYPE_SIGNAL ||
					ev->type == KFD_EVENT_TYPE_DEBUG;
}

static bool event_can_be_cpu_signaled(const struct kfd_event *ev)
{
	return ev->type == KFD_EVENT_TYPE_SIGNAL;
}

static int kfd_event_page_set(struct kfd_process *p, void *kernel_address,
		       uint64_t size, uint64_t user_handle)
{
	struct kfd_signal_page *page;

	if (p->signal_page)
		return -EBUSY;

	page = kzalloc(sizeof(*page), GFP_KERNEL);
	if (!page)
		return -ENOMEM;

	/* Initialize all events to unsignaled */
	memset(kernel_address, (uint8_t) UNSIGNALED_EVENT_SLOT,
	       KFD_SIGNAL_EVENT_LIMIT * 8);

	page->kernel_address = kernel_address;

	p->signal_page = page;
	p->signal_mapped_size = size;
	p->signal_handle = user_handle;
	return 0;
}

int kfd_kmap_event_page(struct kfd_process *p, uint64_t event_page_offset)
{
	struct kfd_dev *kfd;
	struct kfd_process_device *pdd;
	void *mem, *kern_addr;
	uint64_t size;
	int err = 0;

	if (p->signal_page) {
		pr_err("Event page is already set\n");
		return -EINVAL;
	}

	pdd = kfd_process_device_data_by_id(p, GET_GPU_ID(event_page_offset));
	if (!pdd) {
		pr_err("Getting device by id failed in %s\n", __func__);
		return -EINVAL;
	}
	kfd = pdd->dev;

	pdd = kfd_bind_process_to_device(kfd, p);
	if (IS_ERR(pdd))
		return PTR_ERR(pdd);

	mem = kfd_process_device_translate_handle(pdd,
			GET_IDR_HANDLE(event_page_offset));
	if (!mem) {
		pr_err("Can't find BO, offset is 0x%llx\n", event_page_offset);
		return -EINVAL;
	}

	err = amdgpu_amdkfd_gpuvm_map_gtt_bo_to_kernel(kfd->adev,
					mem, &kern_addr, &size);
	if (err) {
		pr_err("Failed to map event page to kernel\n");
		return err;
	}

	err = kfd_event_page_set(p, kern_addr, size, event_page_offset);
	if (err) {
		pr_err("Failed to set event page\n");
		amdgpu_amdkfd_gpuvm_unmap_gtt_bo_from_kernel(kfd->adev, mem);
		return err;
	}
	return err;
}

int kfd_event_create(struct file *devkfd, struct kfd_process *p,
		     uint32_t event_type, bool auto_reset, uint32_t node_id,
		     uint32_t *event_id, uint32_t *event_trigger_data,
		     uint64_t *event_page_offset, uint32_t *event_slot_index)
{
	int ret = 0;
	struct kfd_event *ev = kzalloc(sizeof(*ev), GFP_KERNEL);

	if (!ev)
		return -ENOMEM;

	ev->type = event_type;
	ev->auto_reset = auto_reset;
	ev->signaled = false;

	init_waitqueue_head(&ev->wq);

	*event_page_offset = 0;

	mutex_lock(&p->event_mutex);

	switch (event_type) {
	case KFD_EVENT_TYPE_SIGNAL:
	case KFD_EVENT_TYPE_DEBUG:
		ret = create_signal_event(devkfd, p, ev, NULL);
		if (!ret) {
			*event_page_offset = KFD_MMAP_TYPE_EVENTS;
			*event_slot_index = ev->event_id;
		}
		break;
	default:
		ret = create_other_event(p, ev, NULL);
		break;
	}

	if (!ret) {
		*event_id = ev->event_id;
		*event_trigger_data = ev->event_id;
	} else {
		kfree(ev);
	}

	mutex_unlock(&p->event_mutex);

	return ret;
}

int kfd_criu_restore_event(struct file *devkfd,
			   struct kfd_process *p,
			   uint8_t __user *user_priv_ptr,
			   uint64_t *priv_data_offset,
			   uint64_t max_priv_data_size)
{
	struct kfd_criu_event_priv_data *ev_priv;
	struct kfd_event *ev = NULL;
	int ret = 0;

	ev_priv = kmalloc(sizeof(*ev_priv), GFP_KERNEL);
	if (!ev_priv)
		return -ENOMEM;

	ev = kzalloc(sizeof(*ev), GFP_KERNEL);
	if (!ev) {
		ret = -ENOMEM;
		goto exit;
	}

	if (*priv_data_offset + sizeof(*ev_priv) > max_priv_data_size) {
		ret = -EINVAL;
		goto exit;
	}

	ret = copy_from_user(ev_priv, user_priv_ptr + *priv_data_offset, sizeof(*ev_priv));
	if (ret) {
		ret = -EFAULT;
		goto exit;
	}
	*priv_data_offset += sizeof(*ev_priv);

	if (ev_priv->user_handle) {
		ret = kfd_kmap_event_page(p, ev_priv->user_handle);
		if (ret)
			goto exit;
	}

	ev->type = ev_priv->type;
	ev->auto_reset = ev_priv->auto_reset;
	ev->signaled = ev_priv->signaled;

	init_waitqueue_head(&ev->wq);

	mutex_lock(&p->event_mutex);
	switch (ev->type) {
	case KFD_EVENT_TYPE_SIGNAL:
	case KFD_EVENT_TYPE_DEBUG:
		ret = create_signal_event(devkfd, p, ev, &ev_priv->event_id);
		break;
	case KFD_EVENT_TYPE_MEMORY:
		memcpy(&ev->memory_exception_data,
			&ev_priv->memory_exception_data,
			sizeof(struct kfd_hsa_memory_exception_data));

		ret = create_other_event(p, ev, &ev_priv->event_id);
		break;
	case KFD_EVENT_TYPE_HW_EXCEPTION:
		memcpy(&ev->hw_exception_data,
			&ev_priv->hw_exception_data,
			sizeof(struct kfd_hsa_hw_exception_data));

		ret = create_other_event(p, ev, &ev_priv->event_id);
		break;
	}

exit:
	if (ret)
		kfree(ev);

	kfree(ev_priv);

	mutex_unlock(&p->event_mutex);

	return ret;
}

int kfd_criu_checkpoint_events(struct kfd_process *p,
			 uint8_t __user *user_priv_data,
			 uint64_t *priv_data_offset)
{
	struct kfd_criu_event_priv_data *ev_privs;
	int i = 0;
	int ret =  0;
	struct kfd_event *ev;
	uint32_t ev_id;

	uint32_t num_events = kfd_get_num_events(p);

	if (!num_events)
		return 0;

	ev_privs = kvzalloc(num_events * sizeof(*ev_privs), GFP_KERNEL);
	if (!ev_privs)
		return -ENOMEM;


	idr_for_each_entry(&p->event_idr, ev, ev_id) {
		struct kfd_criu_event_priv_data *ev_priv;

		/*
		 * Currently, all events have same size of private_data, but the current ioctl's
		 * and CRIU plugin supports private_data of variable sizes
		 */
		ev_priv = &ev_privs[i];

		ev_priv->object_type = KFD_CRIU_OBJECT_TYPE_EVENT;

		/* We store the user_handle with the first event */
		if (i == 0 && p->signal_page)
			ev_priv->user_handle = p->signal_handle;

		ev_priv->event_id = ev->event_id;
		ev_priv->auto_reset = ev->auto_reset;
		ev_priv->type = ev->type;
		ev_priv->signaled = ev->signaled;

		if (ev_priv->type == KFD_EVENT_TYPE_MEMORY)
			memcpy(&ev_priv->memory_exception_data,
				&ev->memory_exception_data,
				sizeof(struct kfd_hsa_memory_exception_data));
		else if (ev_priv->type == KFD_EVENT_TYPE_HW_EXCEPTION)
			memcpy(&ev_priv->hw_exception_data,
				&ev->hw_exception_data,
				sizeof(struct kfd_hsa_hw_exception_data));

		pr_debug("Checkpointed event[%d] id = 0x%08x auto_reset = %x type = %x signaled = %x\n",
			  i,
			  ev_priv->event_id,
			  ev_priv->auto_reset,
			  ev_priv->type,
			  ev_priv->signaled);
		i++;
	}

	ret = copy_to_user(user_priv_data + *priv_data_offset,
			   ev_privs, num_events * sizeof(*ev_privs));
	if (ret) {
		pr_err("Failed to copy events priv to user\n");
		ret = -EFAULT;
	}

	*priv_data_offset += num_events * sizeof(*ev_privs);

	kvfree(ev_privs);
	return ret;
}

int kfd_get_num_events(struct kfd_process *p)
{
	struct kfd_event *ev;
	uint32_t id;
	u32 num_events = 0;

	idr_for_each_entry(&p->event_idr, ev, id)
		num_events++;

	return num_events;
}

/* Assumes that p is current. */
int kfd_event_destroy(struct kfd_process *p, uint32_t event_id)
{
	struct kfd_event *ev;
	int ret = 0;

	mutex_lock(&p->event_mutex);

	ev = lookup_event_by_id(p, event_id);

	if (ev)
		destroy_event(p, ev);
	else
		ret = -EINVAL;

	mutex_unlock(&p->event_mutex);
	return ret;
}

static void set_event(struct kfd_event *ev)
{
	struct kfd_event_waiter *waiter;

	/* Auto reset if the list is non-empty and we're waking
	 * someone. waitqueue_active is safe here because we're
	 * protected by the p->event_mutex, which is also held when
	 * updating the wait queues in kfd_wait_on_events.
	 */
	ev->signaled = !ev->auto_reset || !waitqueue_active(&ev->wq);

#if !defined(HAVE_WAIT_QUEUE_ENTRY)
	list_for_each_entry(waiter, &ev->wq.task_list, wait.task_list)
#else
	list_for_each_entry(waiter, &ev->wq.head, wait.entry)
#endif
		waiter->activated = true;

	wake_up_all(&ev->wq);
}

/* Assumes that p is current. */
int kfd_set_event(struct kfd_process *p, uint32_t event_id)
{
	int ret = 0;
	struct kfd_event *ev;

	mutex_lock(&p->event_mutex);

	ev = lookup_event_by_id(p, event_id);

	if (ev && event_can_be_cpu_signaled(ev))
		set_event(ev);
	else
		ret = -EINVAL;

	mutex_unlock(&p->event_mutex);
	return ret;
}

static void reset_event(struct kfd_event *ev)
{
	ev->signaled = false;
}

/* Assumes that p is current. */
int kfd_reset_event(struct kfd_process *p, uint32_t event_id)
{
	int ret = 0;
	struct kfd_event *ev;

	mutex_lock(&p->event_mutex);

	ev = lookup_event_by_id(p, event_id);

	if (ev && event_can_be_cpu_signaled(ev))
		reset_event(ev);
	else
		ret = -EINVAL;

	mutex_unlock(&p->event_mutex);
	return ret;

}

static void acknowledge_signal(struct kfd_process *p, struct kfd_event *ev)
{
	page_slots(p->signal_page)[ev->event_id] = UNSIGNALED_EVENT_SLOT;
}

static void set_event_from_interrupt(struct kfd_process *p,
					struct kfd_event *ev)
{
	if (ev && event_can_be_gpu_signaled(ev)) {
		acknowledge_signal(p, ev);
		set_event(ev);
	}
}

void kfd_signal_event_interrupt(u32 pasid, uint32_t partial_id,
				uint32_t valid_id_bits)
{
	struct kfd_event *ev = NULL;

	/*
	 * Because we are called from arbitrary context (workqueue) as opposed
	 * to process context, kfd_process could attempt to exit while we are
	 * running so the lookup function increments the process ref count.
	 */
	struct kfd_process *p = kfd_lookup_process_by_pasid(pasid);

	if (!p)
		return; /* Presumably process exited. */

	mutex_lock(&p->event_mutex);

	if (valid_id_bits)
		ev = lookup_signaled_event_by_partial_id(p, partial_id,
							 valid_id_bits);
	if (ev) {
		set_event_from_interrupt(p, ev);
	} else if (p->signal_page) {
		/*
		 * Partial ID lookup failed. Assume that the event ID
		 * in the interrupt payload was invalid and do an
		 * exhaustive search of signaled events.
		 */
		uint64_t *slots = page_slots(p->signal_page);
		uint32_t id;

		if (valid_id_bits)
			pr_debug_ratelimited("Partial ID invalid: %u (%u valid bits)\n",
					     partial_id, valid_id_bits);

		if (p->signal_event_count < KFD_SIGNAL_EVENT_LIMIT / 64) {
			/* With relatively few events, it's faster to
			 * iterate over the event IDR
			 */
			idr_for_each_entry(&p->event_idr, ev, id) {
				if (id >= KFD_SIGNAL_EVENT_LIMIT)
					break;

				if (slots[id] != UNSIGNALED_EVENT_SLOT)
					set_event_from_interrupt(p, ev);
			}
		} else {
			/* With relatively many events, it's faster to
			 * iterate over the signal slots and lookup
			 * only signaled events from the IDR.
			 */
			for (id = 0; id < KFD_SIGNAL_EVENT_LIMIT; id++)
				if (slots[id] != UNSIGNALED_EVENT_SLOT) {
					ev = lookup_event_by_id(p, id);
					set_event_from_interrupt(p, ev);
				}
		}
	}

	mutex_unlock(&p->event_mutex);
	kfd_unref_process(p);
}

static struct kfd_event_waiter *alloc_event_waiters(uint32_t num_events)
{
	struct kfd_event_waiter *event_waiters;
	uint32_t i;

	event_waiters = kmalloc_array(num_events,
					sizeof(struct kfd_event_waiter),
					GFP_KERNEL);

	for (i = 0; (event_waiters) && (i < num_events) ; i++) {
		init_wait(&event_waiters[i].wait);
		event_waiters[i].activated = false;
	}

	return event_waiters;
}

static int init_event_waiter_get_status(struct kfd_process *p,
		struct kfd_event_waiter *waiter,
		uint32_t event_id)
{
	struct kfd_event *ev = lookup_event_by_id(p, event_id);

	if (!ev)
		return -EINVAL;

	waiter->event = ev;
	waiter->activated = ev->signaled;
	ev->signaled = ev->signaled && !ev->auto_reset;

	return 0;
}

static void init_event_waiter_add_to_waitlist(struct kfd_event_waiter *waiter)
{
	struct kfd_event *ev = waiter->event;

	/* Only add to the wait list if we actually need to
	 * wait on this event.
	 */
	if (!waiter->activated)
		add_wait_queue(&ev->wq, &waiter->wait);
}

/* test_event_condition - Test condition of events being waited for
 * @all:           Return completion only if all events have signaled
 * @num_events:    Number of events to wait for
 * @event_waiters: Array of event waiters, one per event
 *
 * Returns KFD_IOC_WAIT_RESULT_COMPLETE if all (or one) event(s) have
 * signaled. Returns KFD_IOC_WAIT_RESULT_TIMEOUT if no (or not all)
 * events have signaled. Returns KFD_IOC_WAIT_RESULT_FAIL if any of
 * the events have been destroyed.
 */
static uint32_t test_event_condition(bool all, uint32_t num_events,
				struct kfd_event_waiter *event_waiters)
{
	uint32_t i;
	uint32_t activated_count = 0;

	for (i = 0; i < num_events; i++) {
		if (!event_waiters[i].event)
			return KFD_IOC_WAIT_RESULT_FAIL;

		if (event_waiters[i].activated) {
			if (!all)
				return KFD_IOC_WAIT_RESULT_COMPLETE;

			activated_count++;
		}
	}

	return activated_count == num_events ?
		KFD_IOC_WAIT_RESULT_COMPLETE : KFD_IOC_WAIT_RESULT_TIMEOUT;
}

/*
 * Copy event specific data, if defined.
 * Currently only memory exception events have additional data to copy to user
 */
static int copy_signaled_event_data(uint32_t num_events,
		struct kfd_event_waiter *event_waiters,
		struct kfd_event_data __user *data)
{
	struct kfd_hsa_memory_exception_data *src;
	struct kfd_hsa_memory_exception_data __user *dst;
	struct kfd_event_waiter *waiter;
	struct kfd_event *event;
	uint32_t i;

	for (i = 0; i < num_events; i++) {
		waiter = &event_waiters[i];
		event = waiter->event;
		if (waiter->activated && event->type == KFD_EVENT_TYPE_MEMORY) {
			dst = &data[i].memory_exception_data;
			src = &event->memory_exception_data;
			if (copy_to_user(dst, src,
				sizeof(struct kfd_hsa_memory_exception_data)))
				return -EFAULT;
		}
	}

	return 0;

}



static long user_timeout_to_jiffies(uint32_t user_timeout_ms)
{
	if (user_timeout_ms == KFD_EVENT_TIMEOUT_IMMEDIATE)
		return 0;

	if (user_timeout_ms == KFD_EVENT_TIMEOUT_INFINITE)
		return MAX_SCHEDULE_TIMEOUT;

	/*
	 * msecs_to_jiffies interprets all values above 2^31-1 as infinite,
	 * but we consider them finite.
	 * This hack is wrong, but nobody is likely to notice.
	 */
	user_timeout_ms = min_t(uint32_t, user_timeout_ms, 0x7FFFFFFF);

	return msecs_to_jiffies(user_timeout_ms) + 1;
}

static void free_waiters(uint32_t num_events, struct kfd_event_waiter *waiters)
{
	uint32_t i;

	for (i = 0; i < num_events; i++)
		if (waiters[i].event)
			remove_wait_queue(&waiters[i].event->wq,
					  &waiters[i].wait);

	kfree(waiters);
}

int kfd_wait_on_events(struct kfd_process *p,
		       uint32_t num_events, void __user *data,
		       bool all, uint32_t user_timeout_ms,
		       uint32_t *wait_result)
{
	struct kfd_event_data __user *events =
			(struct kfd_event_data __user *) data;
	uint32_t i;
	int ret = 0;

	struct kfd_event_waiter *event_waiters = NULL;
	long timeout = user_timeout_to_jiffies(user_timeout_ms);

	event_waiters = alloc_event_waiters(num_events);
	if (!event_waiters) {
		ret = -ENOMEM;
		goto out;
	}

	mutex_lock(&p->event_mutex);

	for (i = 0; i < num_events; i++) {
		struct kfd_event_data event_data;

		if (copy_from_user(&event_data, &events[i],
				sizeof(struct kfd_event_data))) {
			ret = -EFAULT;
			goto out_unlock;
		}

		ret = init_event_waiter_get_status(p, &event_waiters[i],
				event_data.event_id);
		if (ret)
			goto out_unlock;
	}

	/* Check condition once. */
	*wait_result = test_event_condition(all, num_events, event_waiters);
	if (*wait_result == KFD_IOC_WAIT_RESULT_COMPLETE) {
		ret = copy_signaled_event_data(num_events,
					       event_waiters, events);
		goto out_unlock;
	} else if (WARN_ON(*wait_result == KFD_IOC_WAIT_RESULT_FAIL)) {
		/* This should not happen. Events shouldn't be
		 * destroyed while we're holding the event_mutex
		 */
		goto out_unlock;
	}

	/* Add to wait lists if we need to wait. */
	for (i = 0; i < num_events; i++)
		init_event_waiter_add_to_waitlist(&event_waiters[i]);

	mutex_unlock(&p->event_mutex);

	while (true) {
		if (fatal_signal_pending(current)) {
			ret = -EINTR;
			break;
		}

		if (signal_pending(current)) {
			/*
			 * This is wrong when a nonzero, non-infinite timeout
			 * is specified. We need to use
			 * ERESTARTSYS_RESTARTBLOCK, but struct restart_block
			 * contains a union with data for each user and it's
			 * in generic kernel code that I don't want to
			 * touch yet.
			 */
			ret = -ERESTARTSYS;
			break;
		}

		/* Set task state to interruptible sleep before
		 * checking wake-up conditions. A concurrent wake-up
		 * will put the task back into runnable state. In that
		 * case schedule_timeout will not put the task to
		 * sleep and we'll get a chance to re-check the
		 * updated conditions almost immediately. Otherwise,
		 * this race condition would lead to a soft hang or a
		 * very long sleep.
		 */
		set_current_state(TASK_INTERRUPTIBLE);

		*wait_result = test_event_condition(all, num_events,
						    event_waiters);
		if (*wait_result != KFD_IOC_WAIT_RESULT_TIMEOUT)
			break;

		if (timeout <= 0)
			break;

		timeout = schedule_timeout(timeout);
	}
	__set_current_state(TASK_RUNNING);

	/* copy_signaled_event_data may sleep. So this has to happen
	 * after the task state is set back to RUNNING.
	 */
	if (!ret && *wait_result == KFD_IOC_WAIT_RESULT_COMPLETE)
		ret = copy_signaled_event_data(num_events,
					       event_waiters, events);

	mutex_lock(&p->event_mutex);
out_unlock:
	free_waiters(num_events, event_waiters);
	mutex_unlock(&p->event_mutex);
out:
	if (ret)
		*wait_result = KFD_IOC_WAIT_RESULT_FAIL;
	else if (*wait_result == KFD_IOC_WAIT_RESULT_FAIL)
		ret = -EIO;

	return ret;
}

int kfd_event_mmap(struct kfd_process *p, struct vm_area_struct *vma)
{
	unsigned long pfn;
	struct kfd_signal_page *page;
	int ret;

	/* check required size doesn't exceed the allocated size */
	if (get_order(KFD_SIGNAL_EVENT_LIMIT * 8) <
			get_order(vma->vm_end - vma->vm_start)) {
		pr_err("Event page mmap requested illegal size\n");
		return -EINVAL;
	}

	page = p->signal_page;
	if (!page) {
		/* Probably KFD bug, but mmap is user-accessible. */
		pr_debug("Signal page could not be found\n");
		return -EINVAL;
	}

	pfn = __pa(page->kernel_address);
	pfn >>= PAGE_SHIFT;

	vma->vm_flags |= VM_IO | VM_DONTCOPY | VM_DONTEXPAND | VM_NORESERVE
		       | VM_DONTDUMP | VM_PFNMAP;

	pr_debug("Mapping signal page\n");
	pr_debug("     start user address  == 0x%08lx\n", vma->vm_start);
	pr_debug("     end user address    == 0x%08lx\n", vma->vm_end);
	pr_debug("     pfn                 == 0x%016lX\n", pfn);
	pr_debug("     vm_flags            == 0x%08lX\n", vma->vm_flags);
	pr_debug("     size                == 0x%08lX\n",
			vma->vm_end - vma->vm_start);

	page->user_address = (uint64_t __user *)vma->vm_start;

	/* mapping the page to user process */
	ret = remap_pfn_range(vma, vma->vm_start, pfn,
			vma->vm_end - vma->vm_start, vma->vm_page_prot);
	if (!ret)
		p->signal_mapped_size = vma->vm_end - vma->vm_start;

	return ret;
}

/*
 * Assumes that p->event_mutex is held and of course
 * that p is not going away (current or locked).
 */
static void lookup_events_by_type_and_signal(struct kfd_process *p,
		int type, void *event_data)
{
	struct kfd_hsa_memory_exception_data *ev_data;
	struct kfd_event *ev;
	uint32_t id;
	bool send_signal = true;

	ev_data = (struct kfd_hsa_memory_exception_data *) event_data;

	id = KFD_FIRST_NONSIGNAL_EVENT_ID;
	idr_for_each_entry_continue(&p->event_idr, ev, id)
		if (ev->type == type) {
			send_signal = false;
			dev_dbg(kfd_device,
					"Event found: id %X type %d",
					ev->event_id, ev->type);
			set_event(ev);
			if (ev->type == KFD_EVENT_TYPE_MEMORY && ev_data)
				ev->memory_exception_data = *ev_data;
		}

	if (type == KFD_EVENT_TYPE_MEMORY) {
		dev_warn(kfd_device,
			"Sending SIGSEGV to process %d (pasid 0x%x)",
				p->lead_thread->pid, p->pasid);
		send_sig(SIGSEGV, p->lead_thread, 0);
	}

	/* Send SIGTERM no event of type "type" has been found*/
	if (send_signal) {
		if (send_sigterm) {
			dev_warn(kfd_device,
				"Sending SIGTERM to process %d (pasid 0x%x)",
					p->lead_thread->pid, p->pasid);
			send_sig(SIGTERM, p->lead_thread, 0);
		} else {
			dev_err(kfd_device,
				"Process %d (pasid 0x%x) got unhandled exception",
				p->lead_thread->pid, p->pasid);
		}
	}
}

#ifdef KFD_SUPPORT_IOMMU_V2
void kfd_signal_iommu_event(struct kfd_dev *dev, u32 pasid,
		unsigned long address, bool is_write_requested,
		bool is_execute_requested)
{
	struct kfd_hsa_memory_exception_data memory_exception_data;
	struct vm_area_struct *vma;
	int user_gpu_id;

	/*
	 * Because we are called from arbitrary context (workqueue) as opposed
	 * to process context, kfd_process could attempt to exit while we are
	 * running so the lookup function increments the process ref count.
	 */
	struct kfd_process *p = kfd_lookup_process_by_pasid(pasid);
	struct mm_struct *mm;

	if (!p)
		return; /* Presumably process exited. */

	/* Take a safe reference to the mm_struct, which may otherwise
	 * disappear even while the kfd_process is still referenced.
	 */
	mm = get_task_mm(p->lead_thread);
	if (!mm) {
		kfd_unref_process(p);
		return; /* Process is exiting */
	}

	user_gpu_id = kfd_process_get_user_gpu_id(p, dev->id);
	if (unlikely(user_gpu_id == -EINVAL)) {
		WARN_ONCE(1, "Could not get user_gpu_id from dev->id:%x\n", dev->id);
		return;
	}
	memset(&memory_exception_data, 0, sizeof(memory_exception_data));

	mmap_read_lock(mm);
	vma = find_vma(mm, address);

	memory_exception_data.gpu_id = user_gpu_id;
	memory_exception_data.va = address;
	/* Set failure reason */
	memory_exception_data.failure.NotPresent = 1;
	memory_exception_data.failure.NoExecute = 0;
	memory_exception_data.failure.ReadOnly = 0;
	if (vma && address >= vma->vm_start) {
		memory_exception_data.failure.NotPresent = 0;

		if (is_write_requested && !(vma->vm_flags & VM_WRITE))
			memory_exception_data.failure.ReadOnly = 1;
		else
			memory_exception_data.failure.ReadOnly = 0;

		if (is_execute_requested && !(vma->vm_flags & VM_EXEC))
			memory_exception_data.failure.NoExecute = 1;
		else
			memory_exception_data.failure.NoExecute = 0;
	}

	mmap_read_unlock(mm);
	mmput(mm);

	pr_debug("notpresent %d, noexecute %d, readonly %d\n",
			memory_exception_data.failure.NotPresent,
			memory_exception_data.failure.NoExecute,
			memory_exception_data.failure.ReadOnly);

	/* Workaround on Raven to not kill the process when memory is freed
	 * before IOMMU is able to finish processing all the excessive PPRs
	 */

	if (KFD_GC_VERSION(dev) != IP_VERSION(9, 1, 0) &&
	    KFD_GC_VERSION(dev) != IP_VERSION(9, 2, 2) &&
	    KFD_GC_VERSION(dev) != IP_VERSION(9, 3, 0)) {
		mutex_lock(&p->event_mutex);

		/* Lookup events by type and signal them */
		lookup_events_by_type_and_signal(p, KFD_EVENT_TYPE_MEMORY,
				&memory_exception_data);

		mutex_unlock(&p->event_mutex);
	}

	kfd_unref_process(p);
}
#endif /* KFD_SUPPORT_IOMMU_V2 */

void kfd_signal_hw_exception_event(u32 pasid)
{
	/*
	 * Because we are called from arbitrary context (workqueue) as opposed
	 * to process context, kfd_process could attempt to exit while we are
	 * running so the lookup function increments the process ref count.
	 */
	struct kfd_process *p = kfd_lookup_process_by_pasid(pasid);

	if (!p)
		return; /* Presumably process exited. */

	mutex_lock(&p->event_mutex);

	/* Lookup events by type and signal them */
	lookup_events_by_type_and_signal(p, KFD_EVENT_TYPE_HW_EXCEPTION, NULL);

	mutex_unlock(&p->event_mutex);
	kfd_unref_process(p);
}

void kfd_signal_vm_fault_event(struct kfd_dev *dev, u32 pasid,
				struct kfd_vm_fault_info *info,
				struct kfd_hsa_memory_exception_data *data)
{
	struct kfd_event *ev;
	uint32_t id;
	struct kfd_process *p = kfd_lookup_process_by_pasid(pasid);
	struct kfd_hsa_memory_exception_data memory_exception_data;
	int user_gpu_id;

	if (!p)
		return; /* Presumably process exited. */

	user_gpu_id = kfd_process_get_user_gpu_id(p, dev->id);
	if (unlikely(user_gpu_id == -EINVAL)) {
		WARN_ONCE(1, "Could not get user_gpu_id from dev->id:%x\n", dev->id);
		return;
	}


	/* SoC15 chips will pass in data from now on. */
	if (!data) {
		memset(&memory_exception_data, 0, sizeof(memory_exception_data));
		memory_exception_data.gpu_id = user_gpu_id;
		memory_exception_data.failure.imprecise = true;

		/* Set failure reason */
		if (info) {
			memory_exception_data.va = (info->page_addr) <<
								PAGE_SHIFT;
			memory_exception_data.failure.NotPresent =
				info->prot_valid ? 1 : 0;
			memory_exception_data.failure.NoExecute =
				info->prot_exec ? 1 : 0;
			memory_exception_data.failure.ReadOnly =
				info->prot_write ? 1 : 0;
			memory_exception_data.failure.imprecise = 0;
		}
	}

	mutex_lock(&p->event_mutex);

	id = KFD_FIRST_NONSIGNAL_EVENT_ID;
	idr_for_each_entry_continue(&p->event_idr, ev, id)
		if (ev->type == KFD_EVENT_TYPE_MEMORY) {
			ev->memory_exception_data = data ? *data :
							memory_exception_data;
			set_event(ev);
		}

	mutex_unlock(&p->event_mutex);
	kfd_unref_process(p);
}

void kfd_signal_reset_event(struct kfd_dev *dev)
{
	struct kfd_hsa_hw_exception_data hw_exception_data;
	struct kfd_hsa_memory_exception_data memory_exception_data;
	struct kfd_process *p;
	struct kfd_event *ev;
	unsigned int temp;
	uint32_t id, idx;
	int reset_cause = atomic_read(&dev->sram_ecc_flag) ?
			KFD_HW_EXCEPTION_ECC :
			KFD_HW_EXCEPTION_GPU_HANG;

	/* Whole gpu reset caused by GPU hang and memory is lost */
	memset(&hw_exception_data, 0, sizeof(hw_exception_data));
	hw_exception_data.memory_lost = 1;
	hw_exception_data.reset_cause = reset_cause;

	memset(&memory_exception_data, 0, sizeof(memory_exception_data));
	memory_exception_data.ErrorType = KFD_MEM_ERR_SRAM_ECC;
	memory_exception_data.failure.imprecise = true;

	idx = srcu_read_lock(&kfd_processes_srcu);
	hash_for_each_rcu(kfd_processes_table, temp, p, kfd_processes) {
		int user_gpu_id = kfd_process_get_user_gpu_id(p, dev->id);

		if (unlikely(user_gpu_id == -EINVAL)) {
			WARN_ONCE(1, "Could not get user_gpu_id from dev->id:%x\n", dev->id);
			continue;
		}

		mutex_lock(&p->event_mutex);
		id = KFD_FIRST_NONSIGNAL_EVENT_ID;
		idr_for_each_entry_continue(&p->event_idr, ev, id) {
			if (ev->type == KFD_EVENT_TYPE_HW_EXCEPTION) {
				ev->hw_exception_data = hw_exception_data;
				ev->hw_exception_data.gpu_id = user_gpu_id;
				set_event(ev);
			}
			if (ev->type == KFD_EVENT_TYPE_MEMORY &&
			    reset_cause == KFD_HW_EXCEPTION_ECC) {
				ev->memory_exception_data = memory_exception_data;
				ev->memory_exception_data.gpu_id = user_gpu_id;
				set_event(ev);
			}
		}
		mutex_unlock(&p->event_mutex);
	}
	srcu_read_unlock(&kfd_processes_srcu, idx);
}

void kfd_signal_poison_consumed_event(struct kfd_dev *dev, u32 pasid)
{
	struct kfd_process *p = kfd_lookup_process_by_pasid(pasid);
	struct kfd_hsa_memory_exception_data memory_exception_data;
	struct kfd_hsa_hw_exception_data hw_exception_data;
	struct kfd_event *ev;
	uint32_t id = KFD_FIRST_NONSIGNAL_EVENT_ID;
	int user_gpu_id;

	if (!p)
		return; /* Presumably process exited. */

	user_gpu_id = kfd_process_get_user_gpu_id(p, dev->id);
	if (unlikely(user_gpu_id == -EINVAL)) {
		WARN_ONCE(1, "Could not get user_gpu_id from dev->id:%x\n", dev->id);
		return;
	}

	memset(&hw_exception_data, 0, sizeof(hw_exception_data));
	hw_exception_data.gpu_id = user_gpu_id;
	hw_exception_data.memory_lost = 1;
	hw_exception_data.reset_cause = KFD_HW_EXCEPTION_ECC;

	memset(&memory_exception_data, 0, sizeof(memory_exception_data));
	memory_exception_data.ErrorType = KFD_MEM_ERR_POISON_CONSUMED;
	memory_exception_data.gpu_id = user_gpu_id;
	memory_exception_data.failure.imprecise = true;

	mutex_lock(&p->event_mutex);
	idr_for_each_entry_continue(&p->event_idr, ev, id) {
		if (ev->type == KFD_EVENT_TYPE_HW_EXCEPTION) {
			ev->hw_exception_data = hw_exception_data;
			set_event(ev);
		}

		if (ev->type == KFD_EVENT_TYPE_MEMORY) {
			ev->memory_exception_data = memory_exception_data;
			set_event(ev);
		}
	}
	mutex_unlock(&p->event_mutex);

	/* user application will handle SIGBUS signal */
	send_sig(SIGBUS, p->lead_thread, 0);

	kfd_unref_process(p);
}
