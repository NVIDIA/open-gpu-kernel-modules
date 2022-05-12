/******************************************************************************
 * Client-facing interface for the Xenbus driver.  In other words, the
 * interface between the Xenbus and the device-specific code, be it the
 * frontend or the backend of that driver.
 *
 * Copyright (C) 2005 XenSource Ltd
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

#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/vmalloc.h>
#include <linux/export.h>
#include <asm/xen/hypervisor.h>
#include <xen/page.h>
#include <xen/interface/xen.h>
#include <xen/interface/event_channel.h>
#include <xen/balloon.h>
#include <xen/events.h>
#include <xen/grant_table.h>
#include <xen/xenbus.h>
#include <xen/xen.h>
#include <xen/features.h>

#include "xenbus.h"

#define XENBUS_PAGES(_grants)	(DIV_ROUND_UP(_grants, XEN_PFN_PER_PAGE))

#define XENBUS_MAX_RING_PAGES	(XENBUS_PAGES(XENBUS_MAX_RING_GRANTS))

struct xenbus_map_node {
	struct list_head next;
	union {
		struct {
			struct vm_struct *area;
		} pv;
		struct {
			struct page *pages[XENBUS_MAX_RING_PAGES];
			unsigned long addrs[XENBUS_MAX_RING_GRANTS];
			void *addr;
		} hvm;
	};
	grant_handle_t handles[XENBUS_MAX_RING_GRANTS];
	unsigned int   nr_handles;
};

struct map_ring_valloc {
	struct xenbus_map_node *node;

	/* Why do we need two arrays? See comment of __xenbus_map_ring */
	unsigned long addrs[XENBUS_MAX_RING_GRANTS];
	phys_addr_t phys_addrs[XENBUS_MAX_RING_GRANTS];

	struct gnttab_map_grant_ref map[XENBUS_MAX_RING_GRANTS];
	struct gnttab_unmap_grant_ref unmap[XENBUS_MAX_RING_GRANTS];

	unsigned int idx;
};

static DEFINE_SPINLOCK(xenbus_valloc_lock);
static LIST_HEAD(xenbus_valloc_pages);

struct xenbus_ring_ops {
	int (*map)(struct xenbus_device *dev, struct map_ring_valloc *info,
		   grant_ref_t *gnt_refs, unsigned int nr_grefs,
		   void **vaddr);
	int (*unmap)(struct xenbus_device *dev, void *vaddr);
};

static const struct xenbus_ring_ops *ring_ops __read_mostly;

const char *xenbus_strstate(enum xenbus_state state)
{
	static const char *const name[] = {
		[ XenbusStateUnknown      ] = "Unknown",
		[ XenbusStateInitialising ] = "Initialising",
		[ XenbusStateInitWait     ] = "InitWait",
		[ XenbusStateInitialised  ] = "Initialised",
		[ XenbusStateConnected    ] = "Connected",
		[ XenbusStateClosing      ] = "Closing",
		[ XenbusStateClosed	  ] = "Closed",
		[XenbusStateReconfiguring] = "Reconfiguring",
		[XenbusStateReconfigured] = "Reconfigured",
	};
	return (state < ARRAY_SIZE(name)) ? name[state] : "INVALID";
}
EXPORT_SYMBOL_GPL(xenbus_strstate);

/**
 * xenbus_watch_path - register a watch
 * @dev: xenbus device
 * @path: path to watch
 * @watch: watch to register
 * @callback: callback to register
 *
 * Register a @watch on the given path, using the given xenbus_watch structure
 * for storage, and the given @callback function as the callback.  Return 0 on
 * success, or -errno on error.  On success, the given @path will be saved as
 * @watch->node, and remains the caller's to free.  On error, @watch->node will
 * be NULL, the device will switch to %XenbusStateClosing, and the error will
 * be saved in the store.
 */
int xenbus_watch_path(struct xenbus_device *dev, const char *path,
		      struct xenbus_watch *watch,
		      bool (*will_handle)(struct xenbus_watch *,
					  const char *, const char *),
		      void (*callback)(struct xenbus_watch *,
				       const char *, const char *))
{
	int err;

	watch->node = path;
	watch->will_handle = will_handle;
	watch->callback = callback;

	err = register_xenbus_watch(watch);

	if (err) {
		watch->node = NULL;
		watch->will_handle = NULL;
		watch->callback = NULL;
		xenbus_dev_fatal(dev, err, "adding watch on %s", path);
	}

	return err;
}
EXPORT_SYMBOL_GPL(xenbus_watch_path);


/**
 * xenbus_watch_pathfmt - register a watch on a sprintf-formatted path
 * @dev: xenbus device
 * @watch: watch to register
 * @callback: callback to register
 * @pathfmt: format of path to watch
 *
 * Register a watch on the given @path, using the given xenbus_watch
 * structure for storage, and the given @callback function as the callback.
 * Return 0 on success, or -errno on error.  On success, the watched path
 * (@path/@path2) will be saved as @watch->node, and becomes the caller's to
 * kfree().  On error, watch->node will be NULL, so the caller has nothing to
 * free, the device will switch to %XenbusStateClosing, and the error will be
 * saved in the store.
 */
int xenbus_watch_pathfmt(struct xenbus_device *dev,
			 struct xenbus_watch *watch,
			 bool (*will_handle)(struct xenbus_watch *,
					const char *, const char *),
			 void (*callback)(struct xenbus_watch *,
					  const char *, const char *),
			 const char *pathfmt, ...)
{
	int err;
	va_list ap;
	char *path;

	va_start(ap, pathfmt);
	path = kvasprintf(GFP_NOIO | __GFP_HIGH, pathfmt, ap);
	va_end(ap);

	if (!path) {
		xenbus_dev_fatal(dev, -ENOMEM, "allocating path for watch");
		return -ENOMEM;
	}
	err = xenbus_watch_path(dev, path, watch, will_handle, callback);

	if (err)
		kfree(path);
	return err;
}
EXPORT_SYMBOL_GPL(xenbus_watch_pathfmt);

static void xenbus_switch_fatal(struct xenbus_device *, int, int,
				const char *, ...);

static int
__xenbus_switch_state(struct xenbus_device *dev,
		      enum xenbus_state state, int depth)
{
	/* We check whether the state is currently set to the given value, and
	   if not, then the state is set.  We don't want to unconditionally
	   write the given state, because we don't want to fire watches
	   unnecessarily.  Furthermore, if the node has gone, we don't write
	   to it, as the device will be tearing down, and we don't want to
	   resurrect that directory.

	   Note that, because of this cached value of our state, this
	   function will not take a caller's Xenstore transaction
	   (something it was trying to in the past) because dev->state
	   would not get reset if the transaction was aborted.
	 */

	struct xenbus_transaction xbt;
	int current_state;
	int err, abort;

	if (state == dev->state)
		return 0;

again:
	abort = 1;

	err = xenbus_transaction_start(&xbt);
	if (err) {
		xenbus_switch_fatal(dev, depth, err, "starting transaction");
		return 0;
	}

	err = xenbus_scanf(xbt, dev->nodename, "state", "%d", &current_state);
	if (err != 1)
		goto abort;

	err = xenbus_printf(xbt, dev->nodename, "state", "%d", state);
	if (err) {
		xenbus_switch_fatal(dev, depth, err, "writing new state");
		goto abort;
	}

	abort = 0;
abort:
	err = xenbus_transaction_end(xbt, abort);
	if (err) {
		if (err == -EAGAIN && !abort)
			goto again;
		xenbus_switch_fatal(dev, depth, err, "ending transaction");
	} else
		dev->state = state;

	return 0;
}

/**
 * xenbus_switch_state
 * @dev: xenbus device
 * @state: new state
 *
 * Advertise in the store a change of the given driver to the given new_state.
 * Return 0 on success, or -errno on error.  On error, the device will switch
 * to XenbusStateClosing, and the error will be saved in the store.
 */
int xenbus_switch_state(struct xenbus_device *dev, enum xenbus_state state)
{
	return __xenbus_switch_state(dev, state, 0);
}

EXPORT_SYMBOL_GPL(xenbus_switch_state);

int xenbus_frontend_closed(struct xenbus_device *dev)
{
	xenbus_switch_state(dev, XenbusStateClosed);
	complete(&dev->down);
	return 0;
}
EXPORT_SYMBOL_GPL(xenbus_frontend_closed);

static void xenbus_va_dev_error(struct xenbus_device *dev, int err,
				const char *fmt, va_list ap)
{
	unsigned int len;
	char *printf_buffer;
	char *path_buffer;

#define PRINTF_BUFFER_SIZE 4096

	printf_buffer = kmalloc(PRINTF_BUFFER_SIZE, GFP_KERNEL);
	if (!printf_buffer)
		return;

	len = sprintf(printf_buffer, "%i ", -err);
	vsnprintf(printf_buffer + len, PRINTF_BUFFER_SIZE - len, fmt, ap);

	dev_err(&dev->dev, "%s\n", printf_buffer);

	path_buffer = kasprintf(GFP_KERNEL, "error/%s", dev->nodename);
	if (path_buffer)
		xenbus_write(XBT_NIL, path_buffer, "error", printf_buffer);

	kfree(printf_buffer);
	kfree(path_buffer);
}

/**
 * xenbus_dev_error
 * @dev: xenbus device
 * @err: error to report
 * @fmt: error message format
 *
 * Report the given negative errno into the store, along with the given
 * formatted message.
 */
void xenbus_dev_error(struct xenbus_device *dev, int err, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	xenbus_va_dev_error(dev, err, fmt, ap);
	va_end(ap);
}
EXPORT_SYMBOL_GPL(xenbus_dev_error);

/**
 * xenbus_dev_fatal
 * @dev: xenbus device
 * @err: error to report
 * @fmt: error message format
 *
 * Equivalent to xenbus_dev_error(dev, err, fmt, args), followed by
 * xenbus_switch_state(dev, XenbusStateClosing) to schedule an orderly
 * closedown of this driver and its peer.
 */

void xenbus_dev_fatal(struct xenbus_device *dev, int err, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	xenbus_va_dev_error(dev, err, fmt, ap);
	va_end(ap);

	xenbus_switch_state(dev, XenbusStateClosing);
}
EXPORT_SYMBOL_GPL(xenbus_dev_fatal);

/**
 * Equivalent to xenbus_dev_fatal(dev, err, fmt, args), but helps
 * avoiding recursion within xenbus_switch_state.
 */
static void xenbus_switch_fatal(struct xenbus_device *dev, int depth, int err,
				const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	xenbus_va_dev_error(dev, err, fmt, ap);
	va_end(ap);

	if (!depth)
		__xenbus_switch_state(dev, XenbusStateClosing, 1);
}

/**
 * xenbus_grant_ring
 * @dev: xenbus device
 * @vaddr: starting virtual address of the ring
 * @nr_pages: number of pages to be granted
 * @grefs: grant reference array to be filled in
 *
 * Grant access to the given @vaddr to the peer of the given device.
 * Then fill in @grefs with grant references.  Return 0 on success, or
 * -errno on error.  On error, the device will switch to
 * XenbusStateClosing, and the error will be saved in the store.
 */
int xenbus_grant_ring(struct xenbus_device *dev, void *vaddr,
		      unsigned int nr_pages, grant_ref_t *grefs)
{
	int err;
	int i, j;

	for (i = 0; i < nr_pages; i++) {
		unsigned long gfn;

		if (is_vmalloc_addr(vaddr))
			gfn = pfn_to_gfn(vmalloc_to_pfn(vaddr));
		else
			gfn = virt_to_gfn(vaddr);

		err = gnttab_grant_foreign_access(dev->otherend_id, gfn, 0);
		if (err < 0) {
			xenbus_dev_fatal(dev, err,
					 "granting access to ring page");
			goto fail;
		}
		grefs[i] = err;

		vaddr = vaddr + XEN_PAGE_SIZE;
	}

	return 0;

fail:
	for (j = 0; j < i; j++)
		gnttab_end_foreign_access_ref(grefs[j], 0);
	return err;
}
EXPORT_SYMBOL_GPL(xenbus_grant_ring);


/**
 * Allocate an event channel for the given xenbus_device, assigning the newly
 * created local port to *port.  Return 0 on success, or -errno on error.  On
 * error, the device will switch to XenbusStateClosing, and the error will be
 * saved in the store.
 */
int xenbus_alloc_evtchn(struct xenbus_device *dev, evtchn_port_t *port)
{
	struct evtchn_alloc_unbound alloc_unbound;
	int err;

	alloc_unbound.dom = DOMID_SELF;
	alloc_unbound.remote_dom = dev->otherend_id;

	err = HYPERVISOR_event_channel_op(EVTCHNOP_alloc_unbound,
					  &alloc_unbound);
	if (err)
		xenbus_dev_fatal(dev, err, "allocating event channel");
	else
		*port = alloc_unbound.port;

	return err;
}
EXPORT_SYMBOL_GPL(xenbus_alloc_evtchn);


/**
 * Free an existing event channel. Returns 0 on success or -errno on error.
 */
int xenbus_free_evtchn(struct xenbus_device *dev, evtchn_port_t port)
{
	struct evtchn_close close;
	int err;

	close.port = port;

	err = HYPERVISOR_event_channel_op(EVTCHNOP_close, &close);
	if (err)
		xenbus_dev_error(dev, err, "freeing event channel %u", port);

	return err;
}
EXPORT_SYMBOL_GPL(xenbus_free_evtchn);


/**
 * xenbus_map_ring_valloc
 * @dev: xenbus device
 * @gnt_refs: grant reference array
 * @nr_grefs: number of grant references
 * @vaddr: pointer to address to be filled out by mapping
 *
 * Map @nr_grefs pages of memory into this domain from another
 * domain's grant table.  xenbus_map_ring_valloc allocates @nr_grefs
 * pages of virtual address space, maps the pages to that address, and
 * sets *vaddr to that address.  Returns 0 on success, and -errno on
 * error. If an error is returned, device will switch to
 * XenbusStateClosing and the error message will be saved in XenStore.
 */
int xenbus_map_ring_valloc(struct xenbus_device *dev, grant_ref_t *gnt_refs,
			   unsigned int nr_grefs, void **vaddr)
{
	int err;
	struct map_ring_valloc *info;

	*vaddr = NULL;

	if (nr_grefs > XENBUS_MAX_RING_GRANTS)
		return -EINVAL;

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->node = kzalloc(sizeof(*info->node), GFP_KERNEL);
	if (!info->node)
		err = -ENOMEM;
	else
		err = ring_ops->map(dev, info, gnt_refs, nr_grefs, vaddr);

	kfree(info->node);
	kfree(info);
	return err;
}
EXPORT_SYMBOL_GPL(xenbus_map_ring_valloc);

/* N.B. sizeof(phys_addr_t) doesn't always equal to sizeof(unsigned
 * long), e.g. 32-on-64.  Caller is responsible for preparing the
 * right array to feed into this function */
static int __xenbus_map_ring(struct xenbus_device *dev,
			     grant_ref_t *gnt_refs,
			     unsigned int nr_grefs,
			     grant_handle_t *handles,
			     struct map_ring_valloc *info,
			     unsigned int flags,
			     bool *leaked)
{
	int i, j;

	if (nr_grefs > XENBUS_MAX_RING_GRANTS)
		return -EINVAL;

	for (i = 0; i < nr_grefs; i++) {
		gnttab_set_map_op(&info->map[i], info->phys_addrs[i], flags,
				  gnt_refs[i], dev->otherend_id);
		handles[i] = INVALID_GRANT_HANDLE;
	}

	gnttab_batch_map(info->map, i);

	for (i = 0; i < nr_grefs; i++) {
		if (info->map[i].status != GNTST_okay) {
			xenbus_dev_fatal(dev, info->map[i].status,
					 "mapping in shared page %d from domain %d",
					 gnt_refs[i], dev->otherend_id);
			goto fail;
		} else
			handles[i] = info->map[i].handle;
	}

	return 0;

 fail:
	for (i = j = 0; i < nr_grefs; i++) {
		if (handles[i] != INVALID_GRANT_HANDLE) {
			gnttab_set_unmap_op(&info->unmap[j],
					    info->phys_addrs[i],
					    GNTMAP_host_map, handles[i]);
			j++;
		}
	}

	if (HYPERVISOR_grant_table_op(GNTTABOP_unmap_grant_ref, info->unmap, j))
		BUG();

	*leaked = false;
	for (i = 0; i < j; i++) {
		if (info->unmap[i].status != GNTST_okay) {
			*leaked = true;
			break;
		}
	}

	return -ENOENT;
}

/**
 * xenbus_unmap_ring
 * @dev: xenbus device
 * @handles: grant handle array
 * @nr_handles: number of handles in the array
 * @vaddrs: addresses to unmap
 *
 * Unmap memory in this domain that was imported from another domain.
 * Returns 0 on success and returns GNTST_* on error
 * (see xen/include/interface/grant_table.h).
 */
static int xenbus_unmap_ring(struct xenbus_device *dev, grant_handle_t *handles,
			     unsigned int nr_handles, unsigned long *vaddrs)
{
	struct gnttab_unmap_grant_ref unmap[XENBUS_MAX_RING_GRANTS];
	int i;
	int err;

	if (nr_handles > XENBUS_MAX_RING_GRANTS)
		return -EINVAL;

	for (i = 0; i < nr_handles; i++)
		gnttab_set_unmap_op(&unmap[i], vaddrs[i],
				    GNTMAP_host_map, handles[i]);

	if (HYPERVISOR_grant_table_op(GNTTABOP_unmap_grant_ref, unmap, i))
		BUG();

	err = GNTST_okay;
	for (i = 0; i < nr_handles; i++) {
		if (unmap[i].status != GNTST_okay) {
			xenbus_dev_error(dev, unmap[i].status,
					 "unmapping page at handle %d error %d",
					 handles[i], unmap[i].status);
			err = unmap[i].status;
			break;
		}
	}

	return err;
}

static void xenbus_map_ring_setup_grant_hvm(unsigned long gfn,
					    unsigned int goffset,
					    unsigned int len,
					    void *data)
{
	struct map_ring_valloc *info = data;
	unsigned long vaddr = (unsigned long)gfn_to_virt(gfn);

	info->phys_addrs[info->idx] = vaddr;
	info->addrs[info->idx] = vaddr;

	info->idx++;
}

static int xenbus_map_ring_hvm(struct xenbus_device *dev,
			       struct map_ring_valloc *info,
			       grant_ref_t *gnt_ref,
			       unsigned int nr_grefs,
			       void **vaddr)
{
	struct xenbus_map_node *node = info->node;
	int err;
	void *addr;
	bool leaked = false;
	unsigned int nr_pages = XENBUS_PAGES(nr_grefs);

	err = xen_alloc_unpopulated_pages(nr_pages, node->hvm.pages);
	if (err)
		goto out_err;

	gnttab_foreach_grant(node->hvm.pages, nr_grefs,
			     xenbus_map_ring_setup_grant_hvm,
			     info);

	err = __xenbus_map_ring(dev, gnt_ref, nr_grefs, node->handles,
				info, GNTMAP_host_map, &leaked);
	node->nr_handles = nr_grefs;

	if (err)
		goto out_free_ballooned_pages;

	addr = vmap(node->hvm.pages, nr_pages, VM_MAP | VM_IOREMAP,
		    PAGE_KERNEL);
	if (!addr) {
		err = -ENOMEM;
		goto out_xenbus_unmap_ring;
	}

	node->hvm.addr = addr;

	spin_lock(&xenbus_valloc_lock);
	list_add(&node->next, &xenbus_valloc_pages);
	spin_unlock(&xenbus_valloc_lock);

	*vaddr = addr;
	info->node = NULL;

	return 0;

 out_xenbus_unmap_ring:
	if (!leaked)
		xenbus_unmap_ring(dev, node->handles, nr_grefs, info->addrs);
	else
		pr_alert("leaking %p size %u page(s)",
			 addr, nr_pages);
 out_free_ballooned_pages:
	if (!leaked)
		xen_free_unpopulated_pages(nr_pages, node->hvm.pages);
 out_err:
	return err;
}

/**
 * xenbus_unmap_ring_vfree
 * @dev: xenbus device
 * @vaddr: addr to unmap
 *
 * Based on Rusty Russell's skeleton driver's unmap_page.
 * Unmap a page of memory in this domain that was imported from another domain.
 * Use xenbus_unmap_ring_vfree if you mapped in your memory with
 * xenbus_map_ring_valloc (it will free the virtual address space).
 * Returns 0 on success and returns GNTST_* on error
 * (see xen/include/interface/grant_table.h).
 */
int xenbus_unmap_ring_vfree(struct xenbus_device *dev, void *vaddr)
{
	return ring_ops->unmap(dev, vaddr);
}
EXPORT_SYMBOL_GPL(xenbus_unmap_ring_vfree);

#ifdef CONFIG_XEN_PV
static int map_ring_apply(pte_t *pte, unsigned long addr, void *data)
{
	struct map_ring_valloc *info = data;

	info->phys_addrs[info->idx++] = arbitrary_virt_to_machine(pte).maddr;
	return 0;
}

static int xenbus_map_ring_pv(struct xenbus_device *dev,
			      struct map_ring_valloc *info,
			      grant_ref_t *gnt_refs,
			      unsigned int nr_grefs,
			      void **vaddr)
{
	struct xenbus_map_node *node = info->node;
	struct vm_struct *area;
	bool leaked = false;
	int err = -ENOMEM;

	area = get_vm_area(XEN_PAGE_SIZE * nr_grefs, VM_IOREMAP);
	if (!area)
		return -ENOMEM;
	if (apply_to_page_range(&init_mm, (unsigned long)area->addr,
				XEN_PAGE_SIZE * nr_grefs, map_ring_apply, info))
		goto failed;
	err = __xenbus_map_ring(dev, gnt_refs, nr_grefs, node->handles,
				info, GNTMAP_host_map | GNTMAP_contains_pte,
				&leaked);
	if (err)
		goto failed;

	node->nr_handles = nr_grefs;
	node->pv.area = area;

	spin_lock(&xenbus_valloc_lock);
	list_add(&node->next, &xenbus_valloc_pages);
	spin_unlock(&xenbus_valloc_lock);

	*vaddr = area->addr;
	info->node = NULL;

	return 0;

failed:
	if (!leaked)
		free_vm_area(area);
	else
		pr_alert("leaking VM area %p size %u page(s)", area, nr_grefs);

	return err;
}

static int xenbus_unmap_ring_pv(struct xenbus_device *dev, void *vaddr)
{
	struct xenbus_map_node *node;
	struct gnttab_unmap_grant_ref unmap[XENBUS_MAX_RING_GRANTS];
	unsigned int level;
	int i;
	bool leaked = false;
	int err;

	spin_lock(&xenbus_valloc_lock);
	list_for_each_entry(node, &xenbus_valloc_pages, next) {
		if (node->pv.area->addr == vaddr) {
			list_del(&node->next);
			goto found;
		}
	}
	node = NULL;
 found:
	spin_unlock(&xenbus_valloc_lock);

	if (!node) {
		xenbus_dev_error(dev, -ENOENT,
				 "can't find mapped virtual address %p", vaddr);
		return GNTST_bad_virt_addr;
	}

	for (i = 0; i < node->nr_handles; i++) {
		unsigned long addr;

		memset(&unmap[i], 0, sizeof(unmap[i]));
		addr = (unsigned long)vaddr + (XEN_PAGE_SIZE * i);
		unmap[i].host_addr = arbitrary_virt_to_machine(
			lookup_address(addr, &level)).maddr;
		unmap[i].dev_bus_addr = 0;
		unmap[i].handle = node->handles[i];
	}

	if (HYPERVISOR_grant_table_op(GNTTABOP_unmap_grant_ref, unmap, i))
		BUG();

	err = GNTST_okay;
	leaked = false;
	for (i = 0; i < node->nr_handles; i++) {
		if (unmap[i].status != GNTST_okay) {
			leaked = true;
			xenbus_dev_error(dev, unmap[i].status,
					 "unmapping page at handle %d error %d",
					 node->handles[i], unmap[i].status);
			err = unmap[i].status;
			break;
		}
	}

	if (!leaked)
		free_vm_area(node->pv.area);
	else
		pr_alert("leaking VM area %p size %u page(s)",
			 node->pv.area, node->nr_handles);

	kfree(node);
	return err;
}

static const struct xenbus_ring_ops ring_ops_pv = {
	.map = xenbus_map_ring_pv,
	.unmap = xenbus_unmap_ring_pv,
};
#endif

struct unmap_ring_hvm
{
	unsigned int idx;
	unsigned long addrs[XENBUS_MAX_RING_GRANTS];
};

static void xenbus_unmap_ring_setup_grant_hvm(unsigned long gfn,
					      unsigned int goffset,
					      unsigned int len,
					      void *data)
{
	struct unmap_ring_hvm *info = data;

	info->addrs[info->idx] = (unsigned long)gfn_to_virt(gfn);

	info->idx++;
}

static int xenbus_unmap_ring_hvm(struct xenbus_device *dev, void *vaddr)
{
	int rv;
	struct xenbus_map_node *node;
	void *addr;
	struct unmap_ring_hvm info = {
		.idx = 0,
	};
	unsigned int nr_pages;

	spin_lock(&xenbus_valloc_lock);
	list_for_each_entry(node, &xenbus_valloc_pages, next) {
		addr = node->hvm.addr;
		if (addr == vaddr) {
			list_del(&node->next);
			goto found;
		}
	}
	node = addr = NULL;
 found:
	spin_unlock(&xenbus_valloc_lock);

	if (!node) {
		xenbus_dev_error(dev, -ENOENT,
				 "can't find mapped virtual address %p", vaddr);
		return GNTST_bad_virt_addr;
	}

	nr_pages = XENBUS_PAGES(node->nr_handles);

	gnttab_foreach_grant(node->hvm.pages, node->nr_handles,
			     xenbus_unmap_ring_setup_grant_hvm,
			     &info);

	rv = xenbus_unmap_ring(dev, node->handles, node->nr_handles,
			       info.addrs);
	if (!rv) {
		vunmap(vaddr);
		xen_free_unpopulated_pages(nr_pages, node->hvm.pages);
	}
	else
		WARN(1, "Leaking %p, size %u page(s)\n", vaddr, nr_pages);

	kfree(node);
	return rv;
}

/**
 * xenbus_read_driver_state
 * @path: path for driver
 *
 * Return the state of the driver rooted at the given store path, or
 * XenbusStateUnknown if no state can be read.
 */
enum xenbus_state xenbus_read_driver_state(const char *path)
{
	enum xenbus_state result;
	int err = xenbus_gather(XBT_NIL, path, "state", "%d", &result, NULL);
	if (err)
		result = XenbusStateUnknown;

	return result;
}
EXPORT_SYMBOL_GPL(xenbus_read_driver_state);

static const struct xenbus_ring_ops ring_ops_hvm = {
	.map = xenbus_map_ring_hvm,
	.unmap = xenbus_unmap_ring_hvm,
};

void __init xenbus_ring_ops_init(void)
{
#ifdef CONFIG_XEN_PV
	if (!xen_feature(XENFEAT_auto_translated_physmap))
		ring_ops = &ring_ops_pv;
	else
#endif
		ring_ops = &ring_ops_hvm;
}
