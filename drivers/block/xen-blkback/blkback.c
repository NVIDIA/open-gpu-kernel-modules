/******************************************************************************
 *
 * Back-end of the driver for virtual block devices. This portion of the
 * driver exports a 'unified' block-device interface that can be accessed
 * by any operating system that implements a compatible front end. A
 * reference front-end implementation can be found in:
 *  drivers/block/xen-blkfront.c
 *
 * Copyright (c) 2003-2004, Keir Fraser & Steve Hand
 * Copyright (c) 2005, Christopher Clark
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation; or, when distributed
 * separately from the Linux kernel or incorporated into other
 * software packages, subject to the following license:
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this source file (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#define pr_fmt(fmt) "xen-blkback: " fmt

#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/freezer.h>
#include <linux/bitmap.h>

#include <xen/events.h>
#include <xen/page.h>
#include <xen/xen.h>
#include <asm/xen/hypervisor.h>
#include <asm/xen/hypercall.h>
#include <xen/balloon.h>
#include <xen/grant_table.h>
#include "common.h"

/*
 * Maximum number of unused free pages to keep in the internal buffer.
 * Setting this to a value too low will reduce memory used in each backend,
 * but can have a performance penalty.
 *
 * A sane value is xen_blkif_reqs * BLKIF_MAX_SEGMENTS_PER_REQUEST, but can
 * be set to a lower value that might degrade performance on some intensive
 * IO workloads.
 */

static int max_buffer_pages = 1024;
module_param_named(max_buffer_pages, max_buffer_pages, int, 0644);
MODULE_PARM_DESC(max_buffer_pages,
"Maximum number of free pages to keep in each block backend buffer");

/*
 * Maximum number of grants to map persistently in blkback. For maximum
 * performance this should be the total numbers of grants that can be used
 * to fill the ring, but since this might become too high, specially with
 * the use of indirect descriptors, we set it to a value that provides good
 * performance without using too much memory.
 *
 * When the list of persistent grants is full we clean it up using a LRU
 * algorithm.
 */

static int max_pgrants = 1056;
module_param_named(max_persistent_grants, max_pgrants, int, 0644);
MODULE_PARM_DESC(max_persistent_grants,
                 "Maximum number of grants to map persistently");

/*
 * How long a persistent grant is allowed to remain allocated without being in
 * use. The time is in seconds, 0 means indefinitely long.
 */

static unsigned int pgrant_timeout = 60;
module_param_named(persistent_grant_unused_seconds, pgrant_timeout,
		   uint, 0644);
MODULE_PARM_DESC(persistent_grant_unused_seconds,
		 "Time in seconds an unused persistent grant is allowed to "
		 "remain allocated. Default is 60, 0 means unlimited.");

/*
 * Maximum number of rings/queues blkback supports, allow as many queues as there
 * are CPUs if user has not specified a value.
 */
unsigned int xenblk_max_queues;
module_param_named(max_queues, xenblk_max_queues, uint, 0644);
MODULE_PARM_DESC(max_queues,
		 "Maximum number of hardware queues per virtual disk." \
		 "By default it is the number of online CPUs.");

/*
 * Maximum order of pages to be used for the shared ring between front and
 * backend, 4KB page granularity is used.
 */
unsigned int xen_blkif_max_ring_order = XENBUS_MAX_RING_GRANT_ORDER;
module_param_named(max_ring_page_order, xen_blkif_max_ring_order, int, 0444);
MODULE_PARM_DESC(max_ring_page_order, "Maximum order of pages to be used for the shared ring");
/*
 * The LRU mechanism to clean the lists of persistent grants needs to
 * be executed periodically. The time interval between consecutive executions
 * of the purge mechanism is set in ms.
 */
#define LRU_INTERVAL 100

/*
 * When the persistent grants list is full we will remove unused grants
 * from the list. The percent number of grants to be removed at each LRU
 * execution.
 */
#define LRU_PERCENT_CLEAN 5

/* Run-time switchable: /sys/module/blkback/parameters/ */
static unsigned int log_stats;
module_param(log_stats, int, 0644);

#define BLKBACK_INVALID_HANDLE (~0)

static inline bool persistent_gnt_timeout(struct persistent_gnt *persistent_gnt)
{
	return pgrant_timeout && (jiffies - persistent_gnt->last_used >=
			HZ * pgrant_timeout);
}

#define vaddr(page) ((unsigned long)pfn_to_kaddr(page_to_pfn(page)))

static int do_block_io_op(struct xen_blkif_ring *ring, unsigned int *eoi_flags);
static int dispatch_rw_block_io(struct xen_blkif_ring *ring,
				struct blkif_request *req,
				struct pending_req *pending_req);
static void make_response(struct xen_blkif_ring *ring, u64 id,
			  unsigned short op, int st);

#define foreach_grant_safe(pos, n, rbtree, node) \
	for ((pos) = container_of(rb_first((rbtree)), typeof(*(pos)), node), \
	     (n) = (&(pos)->node != NULL) ? rb_next(&(pos)->node) : NULL; \
	     &(pos)->node != NULL; \
	     (pos) = container_of(n, typeof(*(pos)), node), \
	     (n) = (&(pos)->node != NULL) ? rb_next(&(pos)->node) : NULL)


/*
 * We don't need locking around the persistent grant helpers
 * because blkback uses a single-thread for each backend, so we
 * can be sure that this functions will never be called recursively.
 *
 * The only exception to that is put_persistent_grant, that can be called
 * from interrupt context (by xen_blkbk_unmap), so we have to use atomic
 * bit operations to modify the flags of a persistent grant and to count
 * the number of used grants.
 */
static int add_persistent_gnt(struct xen_blkif_ring *ring,
			       struct persistent_gnt *persistent_gnt)
{
	struct rb_node **new = NULL, *parent = NULL;
	struct persistent_gnt *this;
	struct xen_blkif *blkif = ring->blkif;

	if (ring->persistent_gnt_c >= max_pgrants) {
		if (!blkif->vbd.overflow_max_grants)
			blkif->vbd.overflow_max_grants = 1;
		return -EBUSY;
	}
	/* Figure out where to put new node */
	new = &ring->persistent_gnts.rb_node;
	while (*new) {
		this = container_of(*new, struct persistent_gnt, node);

		parent = *new;
		if (persistent_gnt->gnt < this->gnt)
			new = &((*new)->rb_left);
		else if (persistent_gnt->gnt > this->gnt)
			new = &((*new)->rb_right);
		else {
			pr_alert_ratelimited("trying to add a gref that's already in the tree\n");
			return -EINVAL;
		}
	}

	persistent_gnt->active = true;
	/* Add new node and rebalance tree. */
	rb_link_node(&(persistent_gnt->node), parent, new);
	rb_insert_color(&(persistent_gnt->node), &ring->persistent_gnts);
	ring->persistent_gnt_c++;
	atomic_inc(&ring->persistent_gnt_in_use);
	return 0;
}

static struct persistent_gnt *get_persistent_gnt(struct xen_blkif_ring *ring,
						 grant_ref_t gref)
{
	struct persistent_gnt *data;
	struct rb_node *node = NULL;

	node = ring->persistent_gnts.rb_node;
	while (node) {
		data = container_of(node, struct persistent_gnt, node);

		if (gref < data->gnt)
			node = node->rb_left;
		else if (gref > data->gnt)
			node = node->rb_right;
		else {
			if (data->active) {
				pr_alert_ratelimited("requesting a grant already in use\n");
				return NULL;
			}
			data->active = true;
			atomic_inc(&ring->persistent_gnt_in_use);
			return data;
		}
	}
	return NULL;
}

static void put_persistent_gnt(struct xen_blkif_ring *ring,
                               struct persistent_gnt *persistent_gnt)
{
	if (!persistent_gnt->active)
		pr_alert_ratelimited("freeing a grant already unused\n");
	persistent_gnt->last_used = jiffies;
	persistent_gnt->active = false;
	atomic_dec(&ring->persistent_gnt_in_use);
}

static void free_persistent_gnts(struct xen_blkif_ring *ring, struct rb_root *root,
                                 unsigned int num)
{
	struct gnttab_unmap_grant_ref unmap[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	struct page *pages[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	struct persistent_gnt *persistent_gnt;
	struct rb_node *n;
	int segs_to_unmap = 0;
	struct gntab_unmap_queue_data unmap_data;

	unmap_data.pages = pages;
	unmap_data.unmap_ops = unmap;
	unmap_data.kunmap_ops = NULL;

	foreach_grant_safe(persistent_gnt, n, root, node) {
		BUG_ON(persistent_gnt->handle ==
			BLKBACK_INVALID_HANDLE);
		gnttab_set_unmap_op(&unmap[segs_to_unmap],
			(unsigned long) pfn_to_kaddr(page_to_pfn(
				persistent_gnt->page)),
			GNTMAP_host_map,
			persistent_gnt->handle);

		pages[segs_to_unmap] = persistent_gnt->page;

		if (++segs_to_unmap == BLKIF_MAX_SEGMENTS_PER_REQUEST ||
			!rb_next(&persistent_gnt->node)) {

			unmap_data.count = segs_to_unmap;
			BUG_ON(gnttab_unmap_refs_sync(&unmap_data));

			gnttab_page_cache_put(&ring->free_pages, pages,
					      segs_to_unmap);
			segs_to_unmap = 0;
		}

		rb_erase(&persistent_gnt->node, root);
		kfree(persistent_gnt);
		num--;
	}
	BUG_ON(num != 0);
}

void xen_blkbk_unmap_purged_grants(struct work_struct *work)
{
	struct gnttab_unmap_grant_ref unmap[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	struct page *pages[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	struct persistent_gnt *persistent_gnt;
	int segs_to_unmap = 0;
	struct xen_blkif_ring *ring = container_of(work, typeof(*ring), persistent_purge_work);
	struct gntab_unmap_queue_data unmap_data;

	unmap_data.pages = pages;
	unmap_data.unmap_ops = unmap;
	unmap_data.kunmap_ops = NULL;

	while(!list_empty(&ring->persistent_purge_list)) {
		persistent_gnt = list_first_entry(&ring->persistent_purge_list,
		                                  struct persistent_gnt,
		                                  remove_node);
		list_del(&persistent_gnt->remove_node);

		gnttab_set_unmap_op(&unmap[segs_to_unmap],
			vaddr(persistent_gnt->page),
			GNTMAP_host_map,
			persistent_gnt->handle);

		pages[segs_to_unmap] = persistent_gnt->page;

		if (++segs_to_unmap == BLKIF_MAX_SEGMENTS_PER_REQUEST) {
			unmap_data.count = segs_to_unmap;
			BUG_ON(gnttab_unmap_refs_sync(&unmap_data));
			gnttab_page_cache_put(&ring->free_pages, pages,
					      segs_to_unmap);
			segs_to_unmap = 0;
		}
		kfree(persistent_gnt);
	}
	if (segs_to_unmap > 0) {
		unmap_data.count = segs_to_unmap;
		BUG_ON(gnttab_unmap_refs_sync(&unmap_data));
		gnttab_page_cache_put(&ring->free_pages, pages, segs_to_unmap);
	}
}

static void purge_persistent_gnt(struct xen_blkif_ring *ring)
{
	struct persistent_gnt *persistent_gnt;
	struct rb_node *n;
	unsigned int num_clean, total;
	bool scan_used = false;
	struct rb_root *root;

	if (work_busy(&ring->persistent_purge_work)) {
		pr_alert_ratelimited("Scheduled work from previous purge is still busy, cannot purge list\n");
		goto out;
	}

	if (ring->persistent_gnt_c < max_pgrants ||
	    (ring->persistent_gnt_c == max_pgrants &&
	    !ring->blkif->vbd.overflow_max_grants)) {
		num_clean = 0;
	} else {
		num_clean = (max_pgrants / 100) * LRU_PERCENT_CLEAN;
		num_clean = ring->persistent_gnt_c - max_pgrants + num_clean;
		num_clean = min(ring->persistent_gnt_c, num_clean);
		pr_debug("Going to purge at least %u persistent grants\n",
			 num_clean);
	}

	/*
	 * At this point, we can assure that there will be no calls
         * to get_persistent_grant (because we are executing this code from
         * xen_blkif_schedule), there can only be calls to put_persistent_gnt,
         * which means that the number of currently used grants will go down,
         * but never up, so we will always be able to remove the requested
         * number of grants.
	 */

	total = 0;

	BUG_ON(!list_empty(&ring->persistent_purge_list));
	root = &ring->persistent_gnts;
purge_list:
	foreach_grant_safe(persistent_gnt, n, root, node) {
		BUG_ON(persistent_gnt->handle ==
			BLKBACK_INVALID_HANDLE);

		if (persistent_gnt->active)
			continue;
		if (!scan_used && !persistent_gnt_timeout(persistent_gnt))
			continue;
		if (scan_used && total >= num_clean)
			continue;

		rb_erase(&persistent_gnt->node, root);
		list_add(&persistent_gnt->remove_node,
			 &ring->persistent_purge_list);
		total++;
	}
	/*
	 * Check whether we also need to start cleaning
	 * grants that were used since last purge in order to cope
	 * with the requested num
	 */
	if (!scan_used && total < num_clean) {
		pr_debug("Still missing %u purged frames\n", num_clean - total);
		scan_used = true;
		goto purge_list;
	}

	if (total) {
		ring->persistent_gnt_c -= total;
		ring->blkif->vbd.overflow_max_grants = 0;

		/* We can defer this work */
		schedule_work(&ring->persistent_purge_work);
		pr_debug("Purged %u/%u\n", num_clean, total);
	}

out:
	return;
}

/*
 * Retrieve from the 'pending_reqs' a free pending_req structure to be used.
 */
static struct pending_req *alloc_req(struct xen_blkif_ring *ring)
{
	struct pending_req *req = NULL;
	unsigned long flags;

	spin_lock_irqsave(&ring->pending_free_lock, flags);
	if (!list_empty(&ring->pending_free)) {
		req = list_entry(ring->pending_free.next, struct pending_req,
				 free_list);
		list_del(&req->free_list);
	}
	spin_unlock_irqrestore(&ring->pending_free_lock, flags);
	return req;
}

/*
 * Return the 'pending_req' structure back to the freepool. We also
 * wake up the thread if it was waiting for a free page.
 */
static void free_req(struct xen_blkif_ring *ring, struct pending_req *req)
{
	unsigned long flags;
	int was_empty;

	spin_lock_irqsave(&ring->pending_free_lock, flags);
	was_empty = list_empty(&ring->pending_free);
	list_add(&req->free_list, &ring->pending_free);
	spin_unlock_irqrestore(&ring->pending_free_lock, flags);
	if (was_empty)
		wake_up(&ring->pending_free_wq);
}

/*
 * Routines for managing virtual block devices (vbds).
 */
static int xen_vbd_translate(struct phys_req *req, struct xen_blkif *blkif,
			     int operation)
{
	struct xen_vbd *vbd = &blkif->vbd;
	int rc = -EACCES;

	if ((operation != REQ_OP_READ) && vbd->readonly)
		goto out;

	if (likely(req->nr_sects)) {
		blkif_sector_t end = req->sector_number + req->nr_sects;

		if (unlikely(end < req->sector_number))
			goto out;
		if (unlikely(end > vbd_sz(vbd)))
			goto out;
	}

	req->dev  = vbd->pdevice;
	req->bdev = vbd->bdev;
	rc = 0;

 out:
	return rc;
}

static void xen_vbd_resize(struct xen_blkif *blkif)
{
	struct xen_vbd *vbd = &blkif->vbd;
	struct xenbus_transaction xbt;
	int err;
	struct xenbus_device *dev = xen_blkbk_xenbus(blkif->be);
	unsigned long long new_size = vbd_sz(vbd);

	pr_info("VBD Resize: Domid: %d, Device: (%d, %d)\n",
		blkif->domid, MAJOR(vbd->pdevice), MINOR(vbd->pdevice));
	pr_info("VBD Resize: new size %llu\n", new_size);
	vbd->size = new_size;
again:
	err = xenbus_transaction_start(&xbt);
	if (err) {
		pr_warn("Error starting transaction\n");
		return;
	}
	err = xenbus_printf(xbt, dev->nodename, "sectors", "%llu",
			    (unsigned long long)vbd_sz(vbd));
	if (err) {
		pr_warn("Error writing new size\n");
		goto abort;
	}
	/*
	 * Write the current state; we will use this to synchronize
	 * the front-end. If the current state is "connected" the
	 * front-end will get the new size information online.
	 */
	err = xenbus_printf(xbt, dev->nodename, "state", "%d", dev->state);
	if (err) {
		pr_warn("Error writing the state\n");
		goto abort;
	}

	err = xenbus_transaction_end(xbt, 0);
	if (err == -EAGAIN)
		goto again;
	if (err)
		pr_warn("Error ending transaction\n");
	return;
abort:
	xenbus_transaction_end(xbt, 1);
}

/*
 * Notification from the guest OS.
 */
static void blkif_notify_work(struct xen_blkif_ring *ring)
{
	ring->waiting_reqs = 1;
	wake_up(&ring->wq);
}

irqreturn_t xen_blkif_be_int(int irq, void *dev_id)
{
	blkif_notify_work(dev_id);
	return IRQ_HANDLED;
}

/*
 * SCHEDULER FUNCTIONS
 */

static void print_stats(struct xen_blkif_ring *ring)
{
	pr_info("(%s): oo %3llu  |  rd %4llu  |  wr %4llu  |  f %4llu"
		 "  |  ds %4llu | pg: %4u/%4d\n",
		 current->comm, ring->st_oo_req,
		 ring->st_rd_req, ring->st_wr_req,
		 ring->st_f_req, ring->st_ds_req,
		 ring->persistent_gnt_c, max_pgrants);
	ring->st_print = jiffies + msecs_to_jiffies(10 * 1000);
	ring->st_rd_req = 0;
	ring->st_wr_req = 0;
	ring->st_oo_req = 0;
	ring->st_ds_req = 0;
}

int xen_blkif_schedule(void *arg)
{
	struct xen_blkif_ring *ring = arg;
	struct xen_blkif *blkif = ring->blkif;
	struct xen_vbd *vbd = &blkif->vbd;
	unsigned long timeout;
	int ret;
	bool do_eoi;
	unsigned int eoi_flags = XEN_EOI_FLAG_SPURIOUS;

	set_freezable();
	while (!kthread_should_stop()) {
		if (try_to_freeze())
			continue;
		if (unlikely(vbd->size != vbd_sz(vbd)))
			xen_vbd_resize(blkif);

		timeout = msecs_to_jiffies(LRU_INTERVAL);

		timeout = wait_event_interruptible_timeout(
			ring->wq,
			ring->waiting_reqs || kthread_should_stop(),
			timeout);
		if (timeout == 0)
			goto purge_gnt_list;
		timeout = wait_event_interruptible_timeout(
			ring->pending_free_wq,
			!list_empty(&ring->pending_free) ||
			kthread_should_stop(),
			timeout);
		if (timeout == 0)
			goto purge_gnt_list;

		do_eoi = ring->waiting_reqs;

		ring->waiting_reqs = 0;
		smp_mb(); /* clear flag *before* checking for work */

		ret = do_block_io_op(ring, &eoi_flags);
		if (ret > 0)
			ring->waiting_reqs = 1;
		if (ret == -EACCES)
			wait_event_interruptible(ring->shutdown_wq,
						 kthread_should_stop());

		if (do_eoi && !ring->waiting_reqs) {
			xen_irq_lateeoi(ring->irq, eoi_flags);
			eoi_flags |= XEN_EOI_FLAG_SPURIOUS;
		}

purge_gnt_list:
		if (blkif->vbd.feature_gnt_persistent &&
		    time_after(jiffies, ring->next_lru)) {
			purge_persistent_gnt(ring);
			ring->next_lru = jiffies + msecs_to_jiffies(LRU_INTERVAL);
		}

		/* Shrink the free pages pool if it is too large. */
		if (time_before(jiffies, blkif->buffer_squeeze_end))
			gnttab_page_cache_shrink(&ring->free_pages, 0);
		else
			gnttab_page_cache_shrink(&ring->free_pages,
						 max_buffer_pages);

		if (log_stats && time_after(jiffies, ring->st_print))
			print_stats(ring);
	}

	/* Drain pending purge work */
	flush_work(&ring->persistent_purge_work);

	if (log_stats)
		print_stats(ring);

	ring->xenblkd = NULL;

	return 0;
}

/*
 * Remove persistent grants and empty the pool of free pages
 */
void xen_blkbk_free_caches(struct xen_blkif_ring *ring)
{
	/* Free all persistent grant pages */
	if (!RB_EMPTY_ROOT(&ring->persistent_gnts))
		free_persistent_gnts(ring, &ring->persistent_gnts,
			ring->persistent_gnt_c);

	BUG_ON(!RB_EMPTY_ROOT(&ring->persistent_gnts));
	ring->persistent_gnt_c = 0;

	/* Since we are shutting down remove all pages from the buffer */
	gnttab_page_cache_shrink(&ring->free_pages, 0 /* All */);
}

static unsigned int xen_blkbk_unmap_prepare(
	struct xen_blkif_ring *ring,
	struct grant_page **pages,
	unsigned int num,
	struct gnttab_unmap_grant_ref *unmap_ops,
	struct page **unmap_pages)
{
	unsigned int i, invcount = 0;

	for (i = 0; i < num; i++) {
		if (pages[i]->persistent_gnt != NULL) {
			put_persistent_gnt(ring, pages[i]->persistent_gnt);
			continue;
		}
		if (pages[i]->handle == BLKBACK_INVALID_HANDLE)
			continue;
		unmap_pages[invcount] = pages[i]->page;
		gnttab_set_unmap_op(&unmap_ops[invcount], vaddr(pages[i]->page),
				    GNTMAP_host_map, pages[i]->handle);
		pages[i]->handle = BLKBACK_INVALID_HANDLE;
		invcount++;
	}

	return invcount;
}

static void xen_blkbk_unmap_and_respond_callback(int result, struct gntab_unmap_queue_data *data)
{
	struct pending_req *pending_req = (struct pending_req *)(data->data);
	struct xen_blkif_ring *ring = pending_req->ring;
	struct xen_blkif *blkif = ring->blkif;

	/* BUG_ON used to reproduce existing behaviour,
	   but is this the best way to deal with this? */
	BUG_ON(result);

	gnttab_page_cache_put(&ring->free_pages, data->pages, data->count);
	make_response(ring, pending_req->id,
		      pending_req->operation, pending_req->status);
	free_req(ring, pending_req);
	/*
	 * Make sure the request is freed before releasing blkif,
	 * or there could be a race between free_req and the
	 * cleanup done in xen_blkif_free during shutdown.
	 *
	 * NB: The fact that we might try to wake up pending_free_wq
	 * before drain_complete (in case there's a drain going on)
	 * it's not a problem with our current implementation
	 * because we can assure there's no thread waiting on
	 * pending_free_wq if there's a drain going on, but it has
	 * to be taken into account if the current model is changed.
	 */
	if (atomic_dec_and_test(&ring->inflight) && atomic_read(&blkif->drain)) {
		complete(&blkif->drain_complete);
	}
	xen_blkif_put(blkif);
}

static void xen_blkbk_unmap_and_respond(struct pending_req *req)
{
	struct gntab_unmap_queue_data* work = &req->gnttab_unmap_data;
	struct xen_blkif_ring *ring = req->ring;
	struct grant_page **pages = req->segments;
	unsigned int invcount;

	invcount = xen_blkbk_unmap_prepare(ring, pages, req->nr_segs,
					   req->unmap, req->unmap_pages);

	work->data = req;
	work->done = xen_blkbk_unmap_and_respond_callback;
	work->unmap_ops = req->unmap;
	work->kunmap_ops = NULL;
	work->pages = req->unmap_pages;
	work->count = invcount;

	gnttab_unmap_refs_async(&req->gnttab_unmap_data);
}


/*
 * Unmap the grant references.
 *
 * This could accumulate ops up to the batch size to reduce the number
 * of hypercalls, but since this is only used in error paths there's
 * no real need.
 */
static void xen_blkbk_unmap(struct xen_blkif_ring *ring,
                            struct grant_page *pages[],
                            int num)
{
	struct gnttab_unmap_grant_ref unmap[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	struct page *unmap_pages[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	unsigned int invcount = 0;
	int ret;

	while (num) {
		unsigned int batch = min(num, BLKIF_MAX_SEGMENTS_PER_REQUEST);

		invcount = xen_blkbk_unmap_prepare(ring, pages, batch,
						   unmap, unmap_pages);
		if (invcount) {
			ret = gnttab_unmap_refs(unmap, NULL, unmap_pages, invcount);
			BUG_ON(ret);
			gnttab_page_cache_put(&ring->free_pages, unmap_pages,
					      invcount);
		}
		pages += batch;
		num -= batch;
	}
}

static int xen_blkbk_map(struct xen_blkif_ring *ring,
			 struct grant_page *pages[],
			 int num, bool ro)
{
	struct gnttab_map_grant_ref map[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	struct page *pages_to_gnt[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	struct persistent_gnt *persistent_gnt = NULL;
	phys_addr_t addr = 0;
	int i, seg_idx, new_map_idx;
	int segs_to_map = 0;
	int ret = 0;
	int last_map = 0, map_until = 0;
	int use_persistent_gnts;
	struct xen_blkif *blkif = ring->blkif;

	use_persistent_gnts = (blkif->vbd.feature_gnt_persistent);

	/*
	 * Fill out preq.nr_sects with proper amount of sectors, and setup
	 * assign map[..] with the PFN of the page in our domain with the
	 * corresponding grant reference for each page.
	 */
again:
	for (i = map_until; i < num; i++) {
		uint32_t flags;

		if (use_persistent_gnts) {
			persistent_gnt = get_persistent_gnt(
				ring,
				pages[i]->gref);
		}

		if (persistent_gnt) {
			/*
			 * We are using persistent grants and
			 * the grant is already mapped
			 */
			pages[i]->page = persistent_gnt->page;
			pages[i]->persistent_gnt = persistent_gnt;
		} else {
			if (gnttab_page_cache_get(&ring->free_pages,
						  &pages[i]->page)) {
				gnttab_page_cache_put(&ring->free_pages,
						      pages_to_gnt,
						      segs_to_map);
				ret = -ENOMEM;
				goto out;
			}
			addr = vaddr(pages[i]->page);
			pages_to_gnt[segs_to_map] = pages[i]->page;
			pages[i]->persistent_gnt = NULL;
			flags = GNTMAP_host_map;
			if (!use_persistent_gnts && ro)
				flags |= GNTMAP_readonly;
			gnttab_set_map_op(&map[segs_to_map++], addr,
					  flags, pages[i]->gref,
					  blkif->domid);
		}
		map_until = i + 1;
		if (segs_to_map == BLKIF_MAX_SEGMENTS_PER_REQUEST)
			break;
	}

	if (segs_to_map)
		ret = gnttab_map_refs(map, NULL, pages_to_gnt, segs_to_map);

	/*
	 * Now swizzle the MFN in our domain with the MFN from the other domain
	 * so that when we access vaddr(pending_req,i) it has the contents of
	 * the page from the other domain.
	 */
	for (seg_idx = last_map, new_map_idx = 0; seg_idx < map_until; seg_idx++) {
		if (!pages[seg_idx]->persistent_gnt) {
			/* This is a newly mapped grant */
			BUG_ON(new_map_idx >= segs_to_map);
			if (unlikely(map[new_map_idx].status != 0)) {
				pr_debug("invalid buffer -- could not remap it\n");
				gnttab_page_cache_put(&ring->free_pages,
						      &pages[seg_idx]->page, 1);
				pages[seg_idx]->handle = BLKBACK_INVALID_HANDLE;
				ret |= !ret;
				goto next;
			}
			pages[seg_idx]->handle = map[new_map_idx].handle;
		} else {
			continue;
		}
		if (use_persistent_gnts &&
		    ring->persistent_gnt_c < max_pgrants) {
			/*
			 * We are using persistent grants, the grant is
			 * not mapped but we might have room for it.
			 */
			persistent_gnt = kmalloc(sizeof(struct persistent_gnt),
				                 GFP_KERNEL);
			if (!persistent_gnt) {
				/*
				 * If we don't have enough memory to
				 * allocate the persistent_gnt struct
				 * map this grant non-persistenly
				 */
				goto next;
			}
			persistent_gnt->gnt = map[new_map_idx].ref;
			persistent_gnt->handle = map[new_map_idx].handle;
			persistent_gnt->page = pages[seg_idx]->page;
			if (add_persistent_gnt(ring,
			                       persistent_gnt)) {
				kfree(persistent_gnt);
				persistent_gnt = NULL;
				goto next;
			}
			pages[seg_idx]->persistent_gnt = persistent_gnt;
			pr_debug("grant %u added to the tree of persistent grants, using %u/%u\n",
				 persistent_gnt->gnt, ring->persistent_gnt_c,
				 max_pgrants);
			goto next;
		}
		if (use_persistent_gnts && !blkif->vbd.overflow_max_grants) {
			blkif->vbd.overflow_max_grants = 1;
			pr_debug("domain %u, device %#x is using maximum number of persistent grants\n",
			         blkif->domid, blkif->vbd.handle);
		}
		/*
		 * We could not map this grant persistently, so use it as
		 * a non-persistent grant.
		 */
next:
		new_map_idx++;
	}
	segs_to_map = 0;
	last_map = map_until;
	if (!ret && map_until != num)
		goto again;

out:
	for (i = last_map; i < num; i++) {
		/* Don't zap current batch's valid persistent grants. */
		if(i >= map_until)
			pages[i]->persistent_gnt = NULL;
		pages[i]->handle = BLKBACK_INVALID_HANDLE;
	}

	return ret;
}

static int xen_blkbk_map_seg(struct pending_req *pending_req)
{
	int rc;

	rc = xen_blkbk_map(pending_req->ring, pending_req->segments,
			   pending_req->nr_segs,
	                   (pending_req->operation != BLKIF_OP_READ));

	return rc;
}

static int xen_blkbk_parse_indirect(struct blkif_request *req,
				    struct pending_req *pending_req,
				    struct seg_buf seg[],
				    struct phys_req *preq)
{
	struct grant_page **pages = pending_req->indirect_pages;
	struct xen_blkif_ring *ring = pending_req->ring;
	int indirect_grefs, rc, n, nseg, i;
	struct blkif_request_segment *segments = NULL;

	nseg = pending_req->nr_segs;
	indirect_grefs = INDIRECT_PAGES(nseg);
	BUG_ON(indirect_grefs > BLKIF_MAX_INDIRECT_PAGES_PER_REQUEST);

	for (i = 0; i < indirect_grefs; i++)
		pages[i]->gref = req->u.indirect.indirect_grefs[i];

	rc = xen_blkbk_map(ring, pages, indirect_grefs, true);
	if (rc)
		goto unmap;

	for (n = 0, i = 0; n < nseg; n++) {
		uint8_t first_sect, last_sect;

		if ((n % SEGS_PER_INDIRECT_FRAME) == 0) {
			/* Map indirect segments */
			if (segments)
				kunmap_atomic(segments);
			segments = kmap_atomic(pages[n/SEGS_PER_INDIRECT_FRAME]->page);
		}
		i = n % SEGS_PER_INDIRECT_FRAME;

		pending_req->segments[n]->gref = segments[i].gref;

		first_sect = READ_ONCE(segments[i].first_sect);
		last_sect = READ_ONCE(segments[i].last_sect);
		if (last_sect >= (XEN_PAGE_SIZE >> 9) || last_sect < first_sect) {
			rc = -EINVAL;
			goto unmap;
		}

		seg[n].nsec = last_sect - first_sect + 1;
		seg[n].offset = first_sect << 9;
		preq->nr_sects += seg[n].nsec;
	}

unmap:
	if (segments)
		kunmap_atomic(segments);
	xen_blkbk_unmap(ring, pages, indirect_grefs);
	return rc;
}

static int dispatch_discard_io(struct xen_blkif_ring *ring,
				struct blkif_request *req)
{
	int err = 0;
	int status = BLKIF_RSP_OKAY;
	struct xen_blkif *blkif = ring->blkif;
	struct block_device *bdev = blkif->vbd.bdev;
	unsigned long secure;
	struct phys_req preq;

	xen_blkif_get(blkif);

	preq.sector_number = req->u.discard.sector_number;
	preq.nr_sects      = req->u.discard.nr_sectors;

	err = xen_vbd_translate(&preq, blkif, REQ_OP_WRITE);
	if (err) {
		pr_warn("access denied: DISCARD [%llu->%llu] on dev=%04x\n",
			preq.sector_number,
			preq.sector_number + preq.nr_sects, blkif->vbd.pdevice);
		goto fail_response;
	}
	ring->st_ds_req++;

	secure = (blkif->vbd.discard_secure &&
		 (req->u.discard.flag & BLKIF_DISCARD_SECURE)) ?
		 BLKDEV_DISCARD_SECURE : 0;

	err = blkdev_issue_discard(bdev, req->u.discard.sector_number,
				   req->u.discard.nr_sectors,
				   GFP_KERNEL, secure);
fail_response:
	if (err == -EOPNOTSUPP) {
		pr_debug("discard op failed, not supported\n");
		status = BLKIF_RSP_EOPNOTSUPP;
	} else if (err)
		status = BLKIF_RSP_ERROR;

	make_response(ring, req->u.discard.id, req->operation, status);
	xen_blkif_put(blkif);
	return err;
}

static int dispatch_other_io(struct xen_blkif_ring *ring,
			     struct blkif_request *req,
			     struct pending_req *pending_req)
{
	free_req(ring, pending_req);
	make_response(ring, req->u.other.id, req->operation,
		      BLKIF_RSP_EOPNOTSUPP);
	return -EIO;
}

static void xen_blk_drain_io(struct xen_blkif_ring *ring)
{
	struct xen_blkif *blkif = ring->blkif;

	atomic_set(&blkif->drain, 1);
	do {
		if (atomic_read(&ring->inflight) == 0)
			break;
		wait_for_completion_interruptible_timeout(
				&blkif->drain_complete, HZ);

		if (!atomic_read(&blkif->drain))
			break;
	} while (!kthread_should_stop());
	atomic_set(&blkif->drain, 0);
}

static void __end_block_io_op(struct pending_req *pending_req,
		blk_status_t error)
{
	/* An error fails the entire request. */
	if (pending_req->operation == BLKIF_OP_FLUSH_DISKCACHE &&
	    error == BLK_STS_NOTSUPP) {
		pr_debug("flush diskcache op failed, not supported\n");
		xen_blkbk_flush_diskcache(XBT_NIL, pending_req->ring->blkif->be, 0);
		pending_req->status = BLKIF_RSP_EOPNOTSUPP;
	} else if (pending_req->operation == BLKIF_OP_WRITE_BARRIER &&
		   error == BLK_STS_NOTSUPP) {
		pr_debug("write barrier op failed, not supported\n");
		xen_blkbk_barrier(XBT_NIL, pending_req->ring->blkif->be, 0);
		pending_req->status = BLKIF_RSP_EOPNOTSUPP;
	} else if (error) {
		pr_debug("Buffer not up-to-date at end of operation,"
			 " error=%d\n", error);
		pending_req->status = BLKIF_RSP_ERROR;
	}

	/*
	 * If all of the bio's have completed it is time to unmap
	 * the grant references associated with 'request' and provide
	 * the proper response on the ring.
	 */
	if (atomic_dec_and_test(&pending_req->pendcnt))
		xen_blkbk_unmap_and_respond(pending_req);
}

/*
 * bio callback.
 */
static void end_block_io_op(struct bio *bio)
{
	__end_block_io_op(bio->bi_private, bio->bi_status);
	bio_put(bio);
}



/*
 * Function to copy the from the ring buffer the 'struct blkif_request'
 * (which has the sectors we want, number of them, grant references, etc),
 * and transmute  it to the block API to hand it over to the proper block disk.
 */
static int
__do_block_io_op(struct xen_blkif_ring *ring, unsigned int *eoi_flags)
{
	union blkif_back_rings *blk_rings = &ring->blk_rings;
	struct blkif_request req;
	struct pending_req *pending_req;
	RING_IDX rc, rp;
	int more_to_do = 0;

	rc = blk_rings->common.req_cons;
	rp = blk_rings->common.sring->req_prod;
	rmb(); /* Ensure we see queued requests up to 'rp'. */

	if (RING_REQUEST_PROD_OVERFLOW(&blk_rings->common, rp)) {
		rc = blk_rings->common.rsp_prod_pvt;
		pr_warn("Frontend provided bogus ring requests (%d - %d = %d). Halting ring processing on dev=%04x\n",
			rp, rc, rp - rc, ring->blkif->vbd.pdevice);
		return -EACCES;
	}
	while (rc != rp) {

		if (RING_REQUEST_CONS_OVERFLOW(&blk_rings->common, rc))
			break;

		/* We've seen a request, so clear spurious eoi flag. */
		*eoi_flags &= ~XEN_EOI_FLAG_SPURIOUS;

		if (kthread_should_stop()) {
			more_to_do = 1;
			break;
		}

		pending_req = alloc_req(ring);
		if (NULL == pending_req) {
			ring->st_oo_req++;
			more_to_do = 1;
			break;
		}

		switch (ring->blkif->blk_protocol) {
		case BLKIF_PROTOCOL_NATIVE:
			memcpy(&req, RING_GET_REQUEST(&blk_rings->native, rc), sizeof(req));
			break;
		case BLKIF_PROTOCOL_X86_32:
			blkif_get_x86_32_req(&req, RING_GET_REQUEST(&blk_rings->x86_32, rc));
			break;
		case BLKIF_PROTOCOL_X86_64:
			blkif_get_x86_64_req(&req, RING_GET_REQUEST(&blk_rings->x86_64, rc));
			break;
		default:
			BUG();
		}
		blk_rings->common.req_cons = ++rc; /* before make_response() */

		/* Apply all sanity checks to /private copy/ of request. */
		barrier();

		switch (req.operation) {
		case BLKIF_OP_READ:
		case BLKIF_OP_WRITE:
		case BLKIF_OP_WRITE_BARRIER:
		case BLKIF_OP_FLUSH_DISKCACHE:
		case BLKIF_OP_INDIRECT:
			if (dispatch_rw_block_io(ring, &req, pending_req))
				goto done;
			break;
		case BLKIF_OP_DISCARD:
			free_req(ring, pending_req);
			if (dispatch_discard_io(ring, &req))
				goto done;
			break;
		default:
			if (dispatch_other_io(ring, &req, pending_req))
				goto done;
			break;
		}

		/* Yield point for this unbounded loop. */
		cond_resched();
	}
done:
	return more_to_do;
}

static int
do_block_io_op(struct xen_blkif_ring *ring, unsigned int *eoi_flags)
{
	union blkif_back_rings *blk_rings = &ring->blk_rings;
	int more_to_do;

	do {
		more_to_do = __do_block_io_op(ring, eoi_flags);
		if (more_to_do)
			break;

		RING_FINAL_CHECK_FOR_REQUESTS(&blk_rings->common, more_to_do);
	} while (more_to_do);

	return more_to_do;
}
/*
 * Transmutation of the 'struct blkif_request' to a proper 'struct bio'
 * and call the 'submit_bio' to pass it to the underlying storage.
 */
static int dispatch_rw_block_io(struct xen_blkif_ring *ring,
				struct blkif_request *req,
				struct pending_req *pending_req)
{
	struct phys_req preq;
	struct seg_buf *seg = pending_req->seg;
	unsigned int nseg;
	struct bio *bio = NULL;
	struct bio **biolist = pending_req->biolist;
	int i, nbio = 0;
	int operation;
	int operation_flags = 0;
	struct blk_plug plug;
	bool drain = false;
	struct grant_page **pages = pending_req->segments;
	unsigned short req_operation;

	req_operation = req->operation == BLKIF_OP_INDIRECT ?
			req->u.indirect.indirect_op : req->operation;

	if ((req->operation == BLKIF_OP_INDIRECT) &&
	    (req_operation != BLKIF_OP_READ) &&
	    (req_operation != BLKIF_OP_WRITE)) {
		pr_debug("Invalid indirect operation (%u)\n", req_operation);
		goto fail_response;
	}

	switch (req_operation) {
	case BLKIF_OP_READ:
		ring->st_rd_req++;
		operation = REQ_OP_READ;
		break;
	case BLKIF_OP_WRITE:
		ring->st_wr_req++;
		operation = REQ_OP_WRITE;
		operation_flags = REQ_SYNC | REQ_IDLE;
		break;
	case BLKIF_OP_WRITE_BARRIER:
		drain = true;
		fallthrough;
	case BLKIF_OP_FLUSH_DISKCACHE:
		ring->st_f_req++;
		operation = REQ_OP_WRITE;
		operation_flags = REQ_PREFLUSH;
		break;
	default:
		operation = 0; /* make gcc happy */
		goto fail_response;
		break;
	}

	/* Check that the number of segments is sane. */
	nseg = req->operation == BLKIF_OP_INDIRECT ?
	       req->u.indirect.nr_segments : req->u.rw.nr_segments;

	if (unlikely(nseg == 0 && operation_flags != REQ_PREFLUSH) ||
	    unlikely((req->operation != BLKIF_OP_INDIRECT) &&
		     (nseg > BLKIF_MAX_SEGMENTS_PER_REQUEST)) ||
	    unlikely((req->operation == BLKIF_OP_INDIRECT) &&
		     (nseg > MAX_INDIRECT_SEGMENTS))) {
		pr_debug("Bad number of segments in request (%d)\n", nseg);
		/* Haven't submitted any bio's yet. */
		goto fail_response;
	}

	preq.nr_sects      = 0;

	pending_req->ring      = ring;
	pending_req->id        = req->u.rw.id;
	pending_req->operation = req_operation;
	pending_req->status    = BLKIF_RSP_OKAY;
	pending_req->nr_segs   = nseg;

	if (req->operation != BLKIF_OP_INDIRECT) {
		preq.dev               = req->u.rw.handle;
		preq.sector_number     = req->u.rw.sector_number;
		for (i = 0; i < nseg; i++) {
			pages[i]->gref = req->u.rw.seg[i].gref;
			seg[i].nsec = req->u.rw.seg[i].last_sect -
				req->u.rw.seg[i].first_sect + 1;
			seg[i].offset = (req->u.rw.seg[i].first_sect << 9);
			if ((req->u.rw.seg[i].last_sect >= (XEN_PAGE_SIZE >> 9)) ||
			    (req->u.rw.seg[i].last_sect <
			     req->u.rw.seg[i].first_sect))
				goto fail_response;
			preq.nr_sects += seg[i].nsec;
		}
	} else {
		preq.dev               = req->u.indirect.handle;
		preq.sector_number     = req->u.indirect.sector_number;
		if (xen_blkbk_parse_indirect(req, pending_req, seg, &preq))
			goto fail_response;
	}

	if (xen_vbd_translate(&preq, ring->blkif, operation) != 0) {
		pr_debug("access denied: %s of [%llu,%llu] on dev=%04x\n",
			 operation == REQ_OP_READ ? "read" : "write",
			 preq.sector_number,
			 preq.sector_number + preq.nr_sects,
			 ring->blkif->vbd.pdevice);
		goto fail_response;
	}

	/*
	 * This check _MUST_ be done after xen_vbd_translate as the preq.bdev
	 * is set there.
	 */
	for (i = 0; i < nseg; i++) {
		if (((int)preq.sector_number|(int)seg[i].nsec) &
		    ((bdev_logical_block_size(preq.bdev) >> 9) - 1)) {
			pr_debug("Misaligned I/O request from domain %d\n",
				 ring->blkif->domid);
			goto fail_response;
		}
	}

	/* Wait on all outstanding I/O's and once that has been completed
	 * issue the flush.
	 */
	if (drain)
		xen_blk_drain_io(pending_req->ring);

	/*
	 * If we have failed at this point, we need to undo the M2P override,
	 * set gnttab_set_unmap_op on all of the grant references and perform
	 * the hypercall to unmap the grants - that is all done in
	 * xen_blkbk_unmap.
	 */
	if (xen_blkbk_map_seg(pending_req))
		goto fail_flush;

	/*
	 * This corresponding xen_blkif_put is done in __end_block_io_op, or
	 * below (in "!bio") if we are handling a BLKIF_OP_DISCARD.
	 */
	xen_blkif_get(ring->blkif);
	atomic_inc(&ring->inflight);

	for (i = 0; i < nseg; i++) {
		while ((bio == NULL) ||
		       (bio_add_page(bio,
				     pages[i]->page,
				     seg[i].nsec << 9,
				     seg[i].offset) == 0)) {
			bio = bio_alloc(GFP_KERNEL, bio_max_segs(nseg - i));
			if (unlikely(bio == NULL))
				goto fail_put_bio;

			biolist[nbio++] = bio;
			bio_set_dev(bio, preq.bdev);
			bio->bi_private = pending_req;
			bio->bi_end_io  = end_block_io_op;
			bio->bi_iter.bi_sector  = preq.sector_number;
			bio_set_op_attrs(bio, operation, operation_flags);
		}

		preq.sector_number += seg[i].nsec;
	}

	/* This will be hit if the operation was a flush or discard. */
	if (!bio) {
		BUG_ON(operation_flags != REQ_PREFLUSH);

		bio = bio_alloc(GFP_KERNEL, 0);
		if (unlikely(bio == NULL))
			goto fail_put_bio;

		biolist[nbio++] = bio;
		bio_set_dev(bio, preq.bdev);
		bio->bi_private = pending_req;
		bio->bi_end_io  = end_block_io_op;
		bio_set_op_attrs(bio, operation, operation_flags);
	}

	atomic_set(&pending_req->pendcnt, nbio);
	blk_start_plug(&plug);

	for (i = 0; i < nbio; i++)
		submit_bio(biolist[i]);

	/* Let the I/Os go.. */
	blk_finish_plug(&plug);

	if (operation == REQ_OP_READ)
		ring->st_rd_sect += preq.nr_sects;
	else if (operation == REQ_OP_WRITE)
		ring->st_wr_sect += preq.nr_sects;

	return 0;

 fail_flush:
	xen_blkbk_unmap(ring, pending_req->segments,
	                pending_req->nr_segs);
 fail_response:
	/* Haven't submitted any bio's yet. */
	make_response(ring, req->u.rw.id, req_operation, BLKIF_RSP_ERROR);
	free_req(ring, pending_req);
	msleep(1); /* back off a bit */
	return -EIO;

 fail_put_bio:
	for (i = 0; i < nbio; i++)
		bio_put(biolist[i]);
	atomic_set(&pending_req->pendcnt, 1);
	__end_block_io_op(pending_req, BLK_STS_RESOURCE);
	msleep(1); /* back off a bit */
	return -EIO;
}



/*
 * Put a response on the ring on how the operation fared.
 */
static void make_response(struct xen_blkif_ring *ring, u64 id,
			  unsigned short op, int st)
{
	struct blkif_response *resp;
	unsigned long     flags;
	union blkif_back_rings *blk_rings;
	int notify;

	spin_lock_irqsave(&ring->blk_ring_lock, flags);
	blk_rings = &ring->blk_rings;
	/* Place on the response ring for the relevant domain. */
	switch (ring->blkif->blk_protocol) {
	case BLKIF_PROTOCOL_NATIVE:
		resp = RING_GET_RESPONSE(&blk_rings->native,
					 blk_rings->native.rsp_prod_pvt);
		break;
	case BLKIF_PROTOCOL_X86_32:
		resp = RING_GET_RESPONSE(&blk_rings->x86_32,
					 blk_rings->x86_32.rsp_prod_pvt);
		break;
	case BLKIF_PROTOCOL_X86_64:
		resp = RING_GET_RESPONSE(&blk_rings->x86_64,
					 blk_rings->x86_64.rsp_prod_pvt);
		break;
	default:
		BUG();
	}

	resp->id        = id;
	resp->operation = op;
	resp->status    = st;

	blk_rings->common.rsp_prod_pvt++;
	RING_PUSH_RESPONSES_AND_CHECK_NOTIFY(&blk_rings->common, notify);
	spin_unlock_irqrestore(&ring->blk_ring_lock, flags);
	if (notify)
		notify_remote_via_irq(ring->irq);
}

static int __init xen_blkif_init(void)
{
	int rc = 0;

	if (!xen_domain())
		return -ENODEV;

	if (xen_blkif_max_ring_order > XENBUS_MAX_RING_GRANT_ORDER) {
		pr_info("Invalid max_ring_order (%d), will use default max: %d.\n",
			xen_blkif_max_ring_order, XENBUS_MAX_RING_GRANT_ORDER);
		xen_blkif_max_ring_order = XENBUS_MAX_RING_GRANT_ORDER;
	}

	if (xenblk_max_queues == 0)
		xenblk_max_queues = num_online_cpus();

	rc = xen_blkif_interface_init();
	if (rc)
		goto failed_init;

	rc = xen_blkif_xenbus_init();
	if (rc)
		goto failed_init;

 failed_init:
	return rc;
}

module_init(xen_blkif_init);

static void __exit xen_blkif_fini(void)
{
	xen_blkif_xenbus_fini();
	xen_blkif_interface_fini();
}

module_exit(xen_blkif_fini);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_ALIAS("xen-backend:vbd");
