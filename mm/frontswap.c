// SPDX-License-Identifier: GPL-2.0-only
/*
 * Frontswap frontend
 *
 * This code provides the generic "frontend" layer to call a matching
 * "backend" driver implementation of frontswap.  See
 * Documentation/vm/frontswap.rst for more information.
 *
 * Copyright (C) 2009-2012 Oracle Corp.  All rights reserved.
 * Author: Dan Magenheimer
 */

#include <linux/mman.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <linux/security.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/frontswap.h>
#include <linux/swapfile.h>

DEFINE_STATIC_KEY_FALSE(frontswap_enabled_key);

/*
 * frontswap_ops are added by frontswap_register_ops, and provide the
 * frontswap "backend" implementation functions.  Multiple implementations
 * may be registered, but implementations can never deregister.  This
 * is a simple singly-linked list of all registered implementations.
 */
static struct frontswap_ops *frontswap_ops __read_mostly;

#define for_each_frontswap_ops(ops)		\
	for ((ops) = frontswap_ops; (ops); (ops) = (ops)->next)

/*
 * If enabled, frontswap_store will return failure even on success.  As
 * a result, the swap subsystem will always write the page to swap, in
 * effect converting frontswap into a writethrough cache.  In this mode,
 * there is no direct reduction in swap writes, but a frontswap backend
 * can unilaterally "reclaim" any pages in use with no data loss, thus
 * providing increases control over maximum memory usage due to frontswap.
 */
static bool frontswap_writethrough_enabled __read_mostly;

/*
 * If enabled, the underlying tmem implementation is capable of doing
 * exclusive gets, so frontswap_load, on a successful tmem_get must
 * mark the page as no longer in frontswap AND mark it dirty.
 */
static bool frontswap_tmem_exclusive_gets_enabled __read_mostly;

#ifdef CONFIG_DEBUG_FS
/*
 * Counters available via /sys/kernel/debug/frontswap (if debugfs is
 * properly configured).  These are for information only so are not protected
 * against increment races.
 */
static u64 frontswap_loads;
static u64 frontswap_succ_stores;
static u64 frontswap_failed_stores;
static u64 frontswap_invalidates;

static inline void inc_frontswap_loads(void)
{
	data_race(frontswap_loads++);
}
static inline void inc_frontswap_succ_stores(void)
{
	data_race(frontswap_succ_stores++);
}
static inline void inc_frontswap_failed_stores(void)
{
	data_race(frontswap_failed_stores++);
}
static inline void inc_frontswap_invalidates(void)
{
	data_race(frontswap_invalidates++);
}
#else
static inline void inc_frontswap_loads(void) { }
static inline void inc_frontswap_succ_stores(void) { }
static inline void inc_frontswap_failed_stores(void) { }
static inline void inc_frontswap_invalidates(void) { }
#endif

/*
 * Due to the asynchronous nature of the backends loading potentially
 * _after_ the swap system has been activated, we have chokepoints
 * on all frontswap functions to not call the backend until the backend
 * has registered.
 *
 * This would not guards us against the user deciding to call swapoff right as
 * we are calling the backend to initialize (so swapon is in action).
 * Fortunately for us, the swapon_mutex has been taken by the callee so we are
 * OK. The other scenario where calls to frontswap_store (called via
 * swap_writepage) is racing with frontswap_invalidate_area (called via
 * swapoff) is again guarded by the swap subsystem.
 *
 * While no backend is registered all calls to frontswap_[store|load|
 * invalidate_area|invalidate_page] are ignored or fail.
 *
 * The time between the backend being registered and the swap file system
 * calling the backend (via the frontswap_* functions) is indeterminate as
 * frontswap_ops is not atomic_t (or a value guarded by a spinlock).
 * That is OK as we are comfortable missing some of these calls to the newly
 * registered backend.
 *
 * Obviously the opposite (unloading the backend) must be done after all
 * the frontswap_[store|load|invalidate_area|invalidate_page] start
 * ignoring or failing the requests.  However, there is currently no way
 * to unload a backend once it is registered.
 */

/*
 * Register operations for frontswap
 */
void frontswap_register_ops(struct frontswap_ops *ops)
{
	DECLARE_BITMAP(a, MAX_SWAPFILES);
	DECLARE_BITMAP(b, MAX_SWAPFILES);
	struct swap_info_struct *si;
	unsigned int i;

	bitmap_zero(a, MAX_SWAPFILES);
	bitmap_zero(b, MAX_SWAPFILES);

	spin_lock(&swap_lock);
	plist_for_each_entry(si, &swap_active_head, list) {
		if (!WARN_ON(!si->frontswap_map))
			set_bit(si->type, a);
	}
	spin_unlock(&swap_lock);

	/* the new ops needs to know the currently active swap devices */
	for_each_set_bit(i, a, MAX_SWAPFILES)
		ops->init(i);

	/*
	 * Setting frontswap_ops must happen after the ops->init() calls
	 * above; cmpxchg implies smp_mb() which will ensure the init is
	 * complete at this point.
	 */
	do {
		ops->next = frontswap_ops;
	} while (cmpxchg(&frontswap_ops, ops->next, ops) != ops->next);

	static_branch_inc(&frontswap_enabled_key);

	spin_lock(&swap_lock);
	plist_for_each_entry(si, &swap_active_head, list) {
		if (si->frontswap_map)
			set_bit(si->type, b);
	}
	spin_unlock(&swap_lock);

	/*
	 * On the very unlikely chance that a swap device was added or
	 * removed between setting the "a" list bits and the ops init
	 * calls, we re-check and do init or invalidate for any changed
	 * bits.
	 */
	if (unlikely(!bitmap_equal(a, b, MAX_SWAPFILES))) {
		for (i = 0; i < MAX_SWAPFILES; i++) {
			if (!test_bit(i, a) && test_bit(i, b))
				ops->init(i);
			else if (test_bit(i, a) && !test_bit(i, b))
				ops->invalidate_area(i);
		}
	}
}
EXPORT_SYMBOL(frontswap_register_ops);

/*
 * Enable/disable frontswap writethrough (see above).
 */
void frontswap_writethrough(bool enable)
{
	frontswap_writethrough_enabled = enable;
}
EXPORT_SYMBOL(frontswap_writethrough);

/*
 * Enable/disable frontswap exclusive gets (see above).
 */
void frontswap_tmem_exclusive_gets(bool enable)
{
	frontswap_tmem_exclusive_gets_enabled = enable;
}
EXPORT_SYMBOL(frontswap_tmem_exclusive_gets);

/*
 * Called when a swap device is swapon'd.
 */
void __frontswap_init(unsigned type, unsigned long *map)
{
	struct swap_info_struct *sis = swap_info[type];
	struct frontswap_ops *ops;

	VM_BUG_ON(sis == NULL);

	/*
	 * p->frontswap is a bitmap that we MUST have to figure out which page
	 * has gone in frontswap. Without it there is no point of continuing.
	 */
	if (WARN_ON(!map))
		return;
	/*
	 * Irregardless of whether the frontswap backend has been loaded
	 * before this function or it will be later, we _MUST_ have the
	 * p->frontswap set to something valid to work properly.
	 */
	frontswap_map_set(sis, map);

	for_each_frontswap_ops(ops)
		ops->init(type);
}
EXPORT_SYMBOL(__frontswap_init);

bool __frontswap_test(struct swap_info_struct *sis,
				pgoff_t offset)
{
	if (sis->frontswap_map)
		return test_bit(offset, sis->frontswap_map);
	return false;
}
EXPORT_SYMBOL(__frontswap_test);

static inline void __frontswap_set(struct swap_info_struct *sis,
				   pgoff_t offset)
{
	set_bit(offset, sis->frontswap_map);
	atomic_inc(&sis->frontswap_pages);
}

static inline void __frontswap_clear(struct swap_info_struct *sis,
				     pgoff_t offset)
{
	clear_bit(offset, sis->frontswap_map);
	atomic_dec(&sis->frontswap_pages);
}

/*
 * "Store" data from a page to frontswap and associate it with the page's
 * swaptype and offset.  Page must be locked and in the swap cache.
 * If frontswap already contains a page with matching swaptype and
 * offset, the frontswap implementation may either overwrite the data and
 * return success or invalidate the page from frontswap and return failure.
 */
int __frontswap_store(struct page *page)
{
	int ret = -1;
	swp_entry_t entry = { .val = page_private(page), };
	int type = swp_type(entry);
	struct swap_info_struct *sis = swap_info[type];
	pgoff_t offset = swp_offset(entry);
	struct frontswap_ops *ops;

	VM_BUG_ON(!frontswap_ops);
	VM_BUG_ON(!PageLocked(page));
	VM_BUG_ON(sis == NULL);

	/*
	 * If a dup, we must remove the old page first; we can't leave the
	 * old page no matter if the store of the new page succeeds or fails,
	 * and we can't rely on the new page replacing the old page as we may
	 * not store to the same implementation that contains the old page.
	 */
	if (__frontswap_test(sis, offset)) {
		__frontswap_clear(sis, offset);
		for_each_frontswap_ops(ops)
			ops->invalidate_page(type, offset);
	}

	/* Try to store in each implementation, until one succeeds. */
	for_each_frontswap_ops(ops) {
		ret = ops->store(type, offset, page);
		if (!ret) /* successful store */
			break;
	}
	if (ret == 0) {
		__frontswap_set(sis, offset);
		inc_frontswap_succ_stores();
	} else {
		inc_frontswap_failed_stores();
	}
	if (frontswap_writethrough_enabled)
		/* report failure so swap also writes to swap device */
		ret = -1;
	return ret;
}
EXPORT_SYMBOL(__frontswap_store);

/*
 * "Get" data from frontswap associated with swaptype and offset that were
 * specified when the data was put to frontswap and use it to fill the
 * specified page with data. Page must be locked and in the swap cache.
 */
int __frontswap_load(struct page *page)
{
	int ret = -1;
	swp_entry_t entry = { .val = page_private(page), };
	int type = swp_type(entry);
	struct swap_info_struct *sis = swap_info[type];
	pgoff_t offset = swp_offset(entry);
	struct frontswap_ops *ops;

	VM_BUG_ON(!frontswap_ops);
	VM_BUG_ON(!PageLocked(page));
	VM_BUG_ON(sis == NULL);

	if (!__frontswap_test(sis, offset))
		return -1;

	/* Try loading from each implementation, until one succeeds. */
	for_each_frontswap_ops(ops) {
		ret = ops->load(type, offset, page);
		if (!ret) /* successful load */
			break;
	}
	if (ret == 0) {
		inc_frontswap_loads();
		if (frontswap_tmem_exclusive_gets_enabled) {
			SetPageDirty(page);
			__frontswap_clear(sis, offset);
		}
	}
	return ret;
}
EXPORT_SYMBOL(__frontswap_load);

/*
 * Invalidate any data from frontswap associated with the specified swaptype
 * and offset so that a subsequent "get" will fail.
 */
void __frontswap_invalidate_page(unsigned type, pgoff_t offset)
{
	struct swap_info_struct *sis = swap_info[type];
	struct frontswap_ops *ops;

	VM_BUG_ON(!frontswap_ops);
	VM_BUG_ON(sis == NULL);

	if (!__frontswap_test(sis, offset))
		return;

	for_each_frontswap_ops(ops)
		ops->invalidate_page(type, offset);
	__frontswap_clear(sis, offset);
	inc_frontswap_invalidates();
}
EXPORT_SYMBOL(__frontswap_invalidate_page);

/*
 * Invalidate all data from frontswap associated with all offsets for the
 * specified swaptype.
 */
void __frontswap_invalidate_area(unsigned type)
{
	struct swap_info_struct *sis = swap_info[type];
	struct frontswap_ops *ops;

	VM_BUG_ON(!frontswap_ops);
	VM_BUG_ON(sis == NULL);

	if (sis->frontswap_map == NULL)
		return;

	for_each_frontswap_ops(ops)
		ops->invalidate_area(type);
	atomic_set(&sis->frontswap_pages, 0);
	bitmap_zero(sis->frontswap_map, sis->max);
}
EXPORT_SYMBOL(__frontswap_invalidate_area);

static unsigned long __frontswap_curr_pages(void)
{
	unsigned long totalpages = 0;
	struct swap_info_struct *si = NULL;

	assert_spin_locked(&swap_lock);
	plist_for_each_entry(si, &swap_active_head, list)
		totalpages += atomic_read(&si->frontswap_pages);
	return totalpages;
}

static int __frontswap_unuse_pages(unsigned long total, unsigned long *unused,
					int *swapid)
{
	int ret = -EINVAL;
	struct swap_info_struct *si = NULL;
	int si_frontswap_pages;
	unsigned long total_pages_to_unuse = total;
	unsigned long pages = 0, pages_to_unuse = 0;

	assert_spin_locked(&swap_lock);
	plist_for_each_entry(si, &swap_active_head, list) {
		si_frontswap_pages = atomic_read(&si->frontswap_pages);
		if (total_pages_to_unuse < si_frontswap_pages) {
			pages = pages_to_unuse = total_pages_to_unuse;
		} else {
			pages = si_frontswap_pages;
			pages_to_unuse = 0; /* unuse all */
		}
		/* ensure there is enough RAM to fetch pages from frontswap */
		if (security_vm_enough_memory_mm(current->mm, pages)) {
			ret = -ENOMEM;
			continue;
		}
		vm_unacct_memory(pages);
		*unused = pages_to_unuse;
		*swapid = si->type;
		ret = 0;
		break;
	}

	return ret;
}

/*
 * Used to check if it's necessary and feasible to unuse pages.
 * Return 1 when nothing to do, 0 when need to shrink pages,
 * error code when there is an error.
 */
static int __frontswap_shrink(unsigned long target_pages,
				unsigned long *pages_to_unuse,
				int *type)
{
	unsigned long total_pages = 0, total_pages_to_unuse;

	assert_spin_locked(&swap_lock);

	total_pages = __frontswap_curr_pages();
	if (total_pages <= target_pages) {
		/* Nothing to do */
		*pages_to_unuse = 0;
		return 1;
	}
	total_pages_to_unuse = total_pages - target_pages;
	return __frontswap_unuse_pages(total_pages_to_unuse, pages_to_unuse, type);
}

/*
 * Frontswap, like a true swap device, may unnecessarily retain pages
 * under certain circumstances; "shrink" frontswap is essentially a
 * "partial swapoff" and works by calling try_to_unuse to attempt to
 * unuse enough frontswap pages to attempt to -- subject to memory
 * constraints -- reduce the number of pages in frontswap to the
 * number given in the parameter target_pages.
 */
void frontswap_shrink(unsigned long target_pages)
{
	unsigned long pages_to_unuse = 0;
	int type, ret;

	/*
	 * we don't want to hold swap_lock while doing a very
	 * lengthy try_to_unuse, but swap_list may change
	 * so restart scan from swap_active_head each time
	 */
	spin_lock(&swap_lock);
	ret = __frontswap_shrink(target_pages, &pages_to_unuse, &type);
	spin_unlock(&swap_lock);
	if (ret == 0)
		try_to_unuse(type, true, pages_to_unuse);
	return;
}
EXPORT_SYMBOL(frontswap_shrink);

/*
 * Count and return the number of frontswap pages across all
 * swap devices.  This is exported so that backend drivers can
 * determine current usage without reading debugfs.
 */
unsigned long frontswap_curr_pages(void)
{
	unsigned long totalpages = 0;

	spin_lock(&swap_lock);
	totalpages = __frontswap_curr_pages();
	spin_unlock(&swap_lock);

	return totalpages;
}
EXPORT_SYMBOL(frontswap_curr_pages);

static int __init init_frontswap(void)
{
#ifdef CONFIG_DEBUG_FS
	struct dentry *root = debugfs_create_dir("frontswap", NULL);
	if (root == NULL)
		return -ENXIO;
	debugfs_create_u64("loads", 0444, root, &frontswap_loads);
	debugfs_create_u64("succ_stores", 0444, root, &frontswap_succ_stores);
	debugfs_create_u64("failed_stores", 0444, root,
			   &frontswap_failed_stores);
	debugfs_create_u64("invalidates", 0444, root, &frontswap_invalidates);
#endif
	return 0;
}

module_init(init_frontswap);
