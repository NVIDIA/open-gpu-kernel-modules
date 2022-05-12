// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2008, 2009 Intel Corporation
 * Authors: Andi Kleen, Fengguang Wu
 *
 * High level machine check handler. Handles pages reported by the
 * hardware as being corrupted usually due to a multi-bit ECC memory or cache
 * failure.
 * 
 * In addition there is a "soft offline" entry point that allows stop using
 * not-yet-corrupted-by-suspicious pages without killing anything.
 *
 * Handles page cache pages in various states.	The tricky part
 * here is that we can access any page asynchronously in respect to 
 * other VM users, because memory failures could happen anytime and 
 * anywhere. This could violate some of their assumptions. This is why 
 * this code has to be extremely careful. Generally it tries to use 
 * normal locking rules, as in get the standard locks, even if that means 
 * the error handling takes potentially a long time.
 *
 * It can be very tempting to add handling for obscure cases here.
 * In general any code for handling new cases should only be added iff:
 * - You know how to test it.
 * - You have a test that can be added to mce-test
 *   https://git.kernel.org/cgit/utils/cpu/mce/mce-test.git/
 * - The case actually shows up as a frequent (top 10) page state in
 *   tools/vm/page-types when running a real workload.
 * 
 * There are several operations here with exponential complexity because
 * of unsuitable VM data structures. For example the operation to map back 
 * from RMAP chains to processes has to walk the complete process list and 
 * has non linear complexity with the number. But since memory corruptions
 * are rare we hope to get away with this. This avoids impacting the core 
 * VM.
 */
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/page-flags.h>
#include <linux/kernel-page-flags.h>
#include <linux/sched/signal.h>
#include <linux/sched/task.h>
#include <linux/ksm.h>
#include <linux/rmap.h>
#include <linux/export.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/backing-dev.h>
#include <linux/migrate.h>
#include <linux/suspend.h>
#include <linux/slab.h>
#include <linux/swapops.h>
#include <linux/hugetlb.h>
#include <linux/memory_hotplug.h>
#include <linux/mm_inline.h>
#include <linux/memremap.h>
#include <linux/kfifo.h>
#include <linux/ratelimit.h>
#include <linux/page-isolation.h>
#include "internal.h"
#include "ras/ras_event.h"

int sysctl_memory_failure_early_kill __read_mostly = 0;

int sysctl_memory_failure_recovery __read_mostly = 1;

atomic_long_t num_poisoned_pages __read_mostly = ATOMIC_LONG_INIT(0);

static bool page_handle_poison(struct page *page, bool hugepage_or_freepage, bool release)
{
	if (hugepage_or_freepage) {
		/*
		 * Doing this check for free pages is also fine since dissolve_free_huge_page
		 * returns 0 for non-hugetlb pages as well.
		 */
		if (dissolve_free_huge_page(page) || !take_page_off_buddy(page))
			/*
			 * We could fail to take off the target page from buddy
			 * for example due to racy page allocation, but that's
			 * acceptable because soft-offlined page is not broken
			 * and if someone really want to use it, they should
			 * take it.
			 */
			return false;
	}

	SetPageHWPoison(page);
	if (release)
		put_page(page);
	page_ref_inc(page);
	num_poisoned_pages_inc();

	return true;
}

#if defined(CONFIG_HWPOISON_INJECT) || defined(CONFIG_HWPOISON_INJECT_MODULE)

u32 hwpoison_filter_enable = 0;
u32 hwpoison_filter_dev_major = ~0U;
u32 hwpoison_filter_dev_minor = ~0U;
u64 hwpoison_filter_flags_mask;
u64 hwpoison_filter_flags_value;
EXPORT_SYMBOL_GPL(hwpoison_filter_enable);
EXPORT_SYMBOL_GPL(hwpoison_filter_dev_major);
EXPORT_SYMBOL_GPL(hwpoison_filter_dev_minor);
EXPORT_SYMBOL_GPL(hwpoison_filter_flags_mask);
EXPORT_SYMBOL_GPL(hwpoison_filter_flags_value);

static int hwpoison_filter_dev(struct page *p)
{
	struct address_space *mapping;
	dev_t dev;

	if (hwpoison_filter_dev_major == ~0U &&
	    hwpoison_filter_dev_minor == ~0U)
		return 0;

	/*
	 * page_mapping() does not accept slab pages.
	 */
	if (PageSlab(p))
		return -EINVAL;

	mapping = page_mapping(p);
	if (mapping == NULL || mapping->host == NULL)
		return -EINVAL;

	dev = mapping->host->i_sb->s_dev;
	if (hwpoison_filter_dev_major != ~0U &&
	    hwpoison_filter_dev_major != MAJOR(dev))
		return -EINVAL;
	if (hwpoison_filter_dev_minor != ~0U &&
	    hwpoison_filter_dev_minor != MINOR(dev))
		return -EINVAL;

	return 0;
}

static int hwpoison_filter_flags(struct page *p)
{
	if (!hwpoison_filter_flags_mask)
		return 0;

	if ((stable_page_flags(p) & hwpoison_filter_flags_mask) ==
				    hwpoison_filter_flags_value)
		return 0;
	else
		return -EINVAL;
}

/*
 * This allows stress tests to limit test scope to a collection of tasks
 * by putting them under some memcg. This prevents killing unrelated/important
 * processes such as /sbin/init. Note that the target task may share clean
 * pages with init (eg. libc text), which is harmless. If the target task
 * share _dirty_ pages with another task B, the test scheme must make sure B
 * is also included in the memcg. At last, due to race conditions this filter
 * can only guarantee that the page either belongs to the memcg tasks, or is
 * a freed page.
 */
#ifdef CONFIG_MEMCG
u64 hwpoison_filter_memcg;
EXPORT_SYMBOL_GPL(hwpoison_filter_memcg);
static int hwpoison_filter_task(struct page *p)
{
	if (!hwpoison_filter_memcg)
		return 0;

	if (page_cgroup_ino(p) != hwpoison_filter_memcg)
		return -EINVAL;

	return 0;
}
#else
static int hwpoison_filter_task(struct page *p) { return 0; }
#endif

int hwpoison_filter(struct page *p)
{
	if (!hwpoison_filter_enable)
		return 0;

	if (hwpoison_filter_dev(p))
		return -EINVAL;

	if (hwpoison_filter_flags(p))
		return -EINVAL;

	if (hwpoison_filter_task(p))
		return -EINVAL;

	return 0;
}
#else
int hwpoison_filter(struct page *p)
{
	return 0;
}
#endif

EXPORT_SYMBOL_GPL(hwpoison_filter);

/*
 * Kill all processes that have a poisoned page mapped and then isolate
 * the page.
 *
 * General strategy:
 * Find all processes having the page mapped and kill them.
 * But we keep a page reference around so that the page is not
 * actually freed yet.
 * Then stash the page away
 *
 * There's no convenient way to get back to mapped processes
 * from the VMAs. So do a brute-force search over all
 * running processes.
 *
 * Remember that machine checks are not common (or rather
 * if they are common you have other problems), so this shouldn't
 * be a performance issue.
 *
 * Also there are some races possible while we get from the
 * error detection to actually handle it.
 */

struct to_kill {
	struct list_head nd;
	struct task_struct *tsk;
	unsigned long addr;
	short size_shift;
};

/*
 * Send all the processes who have the page mapped a signal.
 * ``action optional'' if they are not immediately affected by the error
 * ``action required'' if error happened in current execution context
 */
static int kill_proc(struct to_kill *tk, unsigned long pfn, int flags)
{
	struct task_struct *t = tk->tsk;
	short addr_lsb = tk->size_shift;
	int ret = 0;

	pr_err("Memory failure: %#lx: Sending SIGBUS to %s:%d due to hardware memory corruption\n",
			pfn, t->comm, t->pid);

	if (flags & MF_ACTION_REQUIRED) {
		if (t == current)
			ret = force_sig_mceerr(BUS_MCEERR_AR,
					 (void __user *)tk->addr, addr_lsb);
		else
			/* Signal other processes sharing the page if they have PF_MCE_EARLY set. */
			ret = send_sig_mceerr(BUS_MCEERR_AO, (void __user *)tk->addr,
				addr_lsb, t);
	} else {
		/*
		 * Don't use force here, it's convenient if the signal
		 * can be temporarily blocked.
		 * This could cause a loop when the user sets SIGBUS
		 * to SIG_IGN, but hopefully no one will do that?
		 */
		ret = send_sig_mceerr(BUS_MCEERR_AO, (void __user *)tk->addr,
				      addr_lsb, t);  /* synchronous? */
	}
	if (ret < 0)
		pr_info("Memory failure: Error sending signal to %s:%d: %d\n",
			t->comm, t->pid, ret);
	return ret;
}

/*
 * Unknown page type encountered. Try to check whether it can turn PageLRU by
 * lru_add_drain_all, or a free page by reclaiming slabs when possible.
 */
void shake_page(struct page *p, int access)
{
	if (PageHuge(p))
		return;

	if (!PageSlab(p)) {
		lru_add_drain_all();
		if (PageLRU(p) || is_free_buddy_page(p))
			return;
	}

	/*
	 * Only call shrink_node_slabs here (which would also shrink
	 * other caches) if access is not potentially fatal.
	 */
	if (access)
		drop_slab_node(page_to_nid(p));
}
EXPORT_SYMBOL_GPL(shake_page);

static unsigned long dev_pagemap_mapping_shift(struct page *page,
		struct vm_area_struct *vma)
{
	unsigned long address = vma_address(page, vma);
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	pgd = pgd_offset(vma->vm_mm, address);
	if (!pgd_present(*pgd))
		return 0;
	p4d = p4d_offset(pgd, address);
	if (!p4d_present(*p4d))
		return 0;
	pud = pud_offset(p4d, address);
	if (!pud_present(*pud))
		return 0;
	if (pud_devmap(*pud))
		return PUD_SHIFT;
	pmd = pmd_offset(pud, address);
	if (!pmd_present(*pmd))
		return 0;
	if (pmd_devmap(*pmd))
		return PMD_SHIFT;
	pte = pte_offset_map(pmd, address);
	if (!pte_present(*pte))
		return 0;
	if (pte_devmap(*pte))
		return PAGE_SHIFT;
	return 0;
}

/*
 * Failure handling: if we can't find or can't kill a process there's
 * not much we can do.	We just print a message and ignore otherwise.
 */

/*
 * Schedule a process for later kill.
 * Uses GFP_ATOMIC allocations to avoid potential recursions in the VM.
 */
static void add_to_kill(struct task_struct *tsk, struct page *p,
		       struct vm_area_struct *vma,
		       struct list_head *to_kill)
{
	struct to_kill *tk;

	tk = kmalloc(sizeof(struct to_kill), GFP_ATOMIC);
	if (!tk) {
		pr_err("Memory failure: Out of memory while machine check handling\n");
		return;
	}

	tk->addr = page_address_in_vma(p, vma);
	if (is_zone_device_page(p))
		tk->size_shift = dev_pagemap_mapping_shift(p, vma);
	else
		tk->size_shift = page_shift(compound_head(p));

	/*
	 * Send SIGKILL if "tk->addr == -EFAULT". Also, as
	 * "tk->size_shift" is always non-zero for !is_zone_device_page(),
	 * so "tk->size_shift == 0" effectively checks no mapping on
	 * ZONE_DEVICE. Indeed, when a devdax page is mmapped N times
	 * to a process' address space, it's possible not all N VMAs
	 * contain mappings for the page, but at least one VMA does.
	 * Only deliver SIGBUS with payload derived from the VMA that
	 * has a mapping for the page.
	 */
	if (tk->addr == -EFAULT) {
		pr_info("Memory failure: Unable to find user space address %lx in %s\n",
			page_to_pfn(p), tsk->comm);
	} else if (tk->size_shift == 0) {
		kfree(tk);
		return;
	}

	get_task_struct(tsk);
	tk->tsk = tsk;
	list_add_tail(&tk->nd, to_kill);
}

/*
 * Kill the processes that have been collected earlier.
 *
 * Only do anything when DOIT is set, otherwise just free the list
 * (this is used for clean pages which do not need killing)
 * Also when FAIL is set do a force kill because something went
 * wrong earlier.
 */
static void kill_procs(struct list_head *to_kill, int forcekill, bool fail,
		unsigned long pfn, int flags)
{
	struct to_kill *tk, *next;

	list_for_each_entry_safe (tk, next, to_kill, nd) {
		if (forcekill) {
			/*
			 * In case something went wrong with munmapping
			 * make sure the process doesn't catch the
			 * signal and then access the memory. Just kill it.
			 */
			if (fail || tk->addr == -EFAULT) {
				pr_err("Memory failure: %#lx: forcibly killing %s:%d because of failure to unmap corrupted page\n",
				       pfn, tk->tsk->comm, tk->tsk->pid);
				do_send_sig_info(SIGKILL, SEND_SIG_PRIV,
						 tk->tsk, PIDTYPE_PID);
			}

			/*
			 * In theory the process could have mapped
			 * something else on the address in-between. We could
			 * check for that, but we need to tell the
			 * process anyways.
			 */
			else if (kill_proc(tk, pfn, flags) < 0)
				pr_err("Memory failure: %#lx: Cannot send advisory machine check signal to %s:%d\n",
				       pfn, tk->tsk->comm, tk->tsk->pid);
		}
		put_task_struct(tk->tsk);
		kfree(tk);
	}
}

/*
 * Find a dedicated thread which is supposed to handle SIGBUS(BUS_MCEERR_AO)
 * on behalf of the thread group. Return task_struct of the (first found)
 * dedicated thread if found, and return NULL otherwise.
 *
 * We already hold read_lock(&tasklist_lock) in the caller, so we don't
 * have to call rcu_read_lock/unlock() in this function.
 */
static struct task_struct *find_early_kill_thread(struct task_struct *tsk)
{
	struct task_struct *t;

	for_each_thread(tsk, t) {
		if (t->flags & PF_MCE_PROCESS) {
			if (t->flags & PF_MCE_EARLY)
				return t;
		} else {
			if (sysctl_memory_failure_early_kill)
				return t;
		}
	}
	return NULL;
}

/*
 * Determine whether a given process is "early kill" process which expects
 * to be signaled when some page under the process is hwpoisoned.
 * Return task_struct of the dedicated thread (main thread unless explicitly
 * specified) if the process is "early kill" and otherwise returns NULL.
 *
 * Note that the above is true for Action Optional case. For Action Required
 * case, it's only meaningful to the current thread which need to be signaled
 * with SIGBUS, this error is Action Optional for other non current
 * processes sharing the same error page,if the process is "early kill", the
 * task_struct of the dedicated thread will also be returned.
 */
static struct task_struct *task_early_kill(struct task_struct *tsk,
					   int force_early)
{
	if (!tsk->mm)
		return NULL;
	/*
	 * Comparing ->mm here because current task might represent
	 * a subthread, while tsk always points to the main thread.
	 */
	if (force_early && tsk->mm == current->mm)
		return current;

	return find_early_kill_thread(tsk);
}

/*
 * Collect processes when the error hit an anonymous page.
 */
static void collect_procs_anon(struct page *page, struct list_head *to_kill,
				int force_early)
{
	struct vm_area_struct *vma;
	struct task_struct *tsk;
	struct anon_vma *av;
	pgoff_t pgoff;

	av = page_lock_anon_vma_read(page);
	if (av == NULL)	/* Not actually mapped anymore */
		return;

	pgoff = page_to_pgoff(page);
	read_lock(&tasklist_lock);
	for_each_process (tsk) {
		struct anon_vma_chain *vmac;
		struct task_struct *t = task_early_kill(tsk, force_early);

		if (!t)
			continue;
		anon_vma_interval_tree_foreach(vmac, &av->rb_root,
					       pgoff, pgoff) {
			vma = vmac->vma;
			if (!page_mapped_in_vma(page, vma))
				continue;
			if (vma->vm_mm == t->mm)
				add_to_kill(t, page, vma, to_kill);
		}
	}
	read_unlock(&tasklist_lock);
	page_unlock_anon_vma_read(av);
}

/*
 * Collect processes when the error hit a file mapped page.
 */
static void collect_procs_file(struct page *page, struct list_head *to_kill,
				int force_early)
{
	struct vm_area_struct *vma;
	struct task_struct *tsk;
	struct address_space *mapping = page->mapping;
	pgoff_t pgoff;

	i_mmap_lock_read(mapping);
	read_lock(&tasklist_lock);
	pgoff = page_to_pgoff(page);
	for_each_process(tsk) {
		struct task_struct *t = task_early_kill(tsk, force_early);

		if (!t)
			continue;
		vma_interval_tree_foreach(vma, &mapping->i_mmap, pgoff,
				      pgoff) {
			/*
			 * Send early kill signal to tasks where a vma covers
			 * the page but the corrupted page is not necessarily
			 * mapped it in its pte.
			 * Assume applications who requested early kill want
			 * to be informed of all such data corruptions.
			 */
			if (vma->vm_mm == t->mm)
				add_to_kill(t, page, vma, to_kill);
		}
	}
	read_unlock(&tasklist_lock);
	i_mmap_unlock_read(mapping);
}

/*
 * Collect the processes who have the corrupted page mapped to kill.
 */
static void collect_procs(struct page *page, struct list_head *tokill,
				int force_early)
{
	if (!page->mapping)
		return;

	if (PageAnon(page))
		collect_procs_anon(page, tokill, force_early);
	else
		collect_procs_file(page, tokill, force_early);
}

static const char *action_name[] = {
	[MF_IGNORED] = "Ignored",
	[MF_FAILED] = "Failed",
	[MF_DELAYED] = "Delayed",
	[MF_RECOVERED] = "Recovered",
};

static const char * const action_page_types[] = {
	[MF_MSG_KERNEL]			= "reserved kernel page",
	[MF_MSG_KERNEL_HIGH_ORDER]	= "high-order kernel page",
	[MF_MSG_SLAB]			= "kernel slab page",
	[MF_MSG_DIFFERENT_COMPOUND]	= "different compound page after locking",
	[MF_MSG_POISONED_HUGE]		= "huge page already hardware poisoned",
	[MF_MSG_HUGE]			= "huge page",
	[MF_MSG_FREE_HUGE]		= "free huge page",
	[MF_MSG_NON_PMD_HUGE]		= "non-pmd-sized huge page",
	[MF_MSG_UNMAP_FAILED]		= "unmapping failed page",
	[MF_MSG_DIRTY_SWAPCACHE]	= "dirty swapcache page",
	[MF_MSG_CLEAN_SWAPCACHE]	= "clean swapcache page",
	[MF_MSG_DIRTY_MLOCKED_LRU]	= "dirty mlocked LRU page",
	[MF_MSG_CLEAN_MLOCKED_LRU]	= "clean mlocked LRU page",
	[MF_MSG_DIRTY_UNEVICTABLE_LRU]	= "dirty unevictable LRU page",
	[MF_MSG_CLEAN_UNEVICTABLE_LRU]	= "clean unevictable LRU page",
	[MF_MSG_DIRTY_LRU]		= "dirty LRU page",
	[MF_MSG_CLEAN_LRU]		= "clean LRU page",
	[MF_MSG_TRUNCATED_LRU]		= "already truncated LRU page",
	[MF_MSG_BUDDY]			= "free buddy page",
	[MF_MSG_BUDDY_2ND]		= "free buddy page (2nd try)",
	[MF_MSG_DAX]			= "dax page",
	[MF_MSG_UNSPLIT_THP]		= "unsplit thp",
	[MF_MSG_UNKNOWN]		= "unknown page",
};

/*
 * XXX: It is possible that a page is isolated from LRU cache,
 * and then kept in swap cache or failed to remove from page cache.
 * The page count will stop it from being freed by unpoison.
 * Stress tests should be aware of this memory leak problem.
 */
static int delete_from_lru_cache(struct page *p)
{
	if (!isolate_lru_page(p)) {
		/*
		 * Clear sensible page flags, so that the buddy system won't
		 * complain when the page is unpoison-and-freed.
		 */
		ClearPageActive(p);
		ClearPageUnevictable(p);

		/*
		 * Poisoned page might never drop its ref count to 0 so we have
		 * to uncharge it manually from its memcg.
		 */
		mem_cgroup_uncharge(p);

		/*
		 * drop the page count elevated by isolate_lru_page()
		 */
		put_page(p);
		return 0;
	}
	return -EIO;
}

static int truncate_error_page(struct page *p, unsigned long pfn,
				struct address_space *mapping)
{
	int ret = MF_FAILED;

	if (mapping->a_ops->error_remove_page) {
		int err = mapping->a_ops->error_remove_page(mapping, p);

		if (err != 0) {
			pr_info("Memory failure: %#lx: Failed to punch page: %d\n",
				pfn, err);
		} else if (page_has_private(p) &&
			   !try_to_release_page(p, GFP_NOIO)) {
			pr_info("Memory failure: %#lx: failed to release buffers\n",
				pfn);
		} else {
			ret = MF_RECOVERED;
		}
	} else {
		/*
		 * If the file system doesn't support it just invalidate
		 * This fails on dirty or anything with private pages
		 */
		if (invalidate_inode_page(p))
			ret = MF_RECOVERED;
		else
			pr_info("Memory failure: %#lx: Failed to invalidate\n",
				pfn);
	}

	return ret;
}

/*
 * Error hit kernel page.
 * Do nothing, try to be lucky and not touch this instead. For a few cases we
 * could be more sophisticated.
 */
static int me_kernel(struct page *p, unsigned long pfn)
{
	unlock_page(p);
	return MF_IGNORED;
}

/*
 * Page in unknown state. Do nothing.
 */
static int me_unknown(struct page *p, unsigned long pfn)
{
	pr_err("Memory failure: %#lx: Unknown page state\n", pfn);
	unlock_page(p);
	return MF_FAILED;
}

/*
 * Clean (or cleaned) page cache page.
 */
static int me_pagecache_clean(struct page *p, unsigned long pfn)
{
	int ret;
	struct address_space *mapping;

	delete_from_lru_cache(p);

	/*
	 * For anonymous pages we're done the only reference left
	 * should be the one m_f() holds.
	 */
	if (PageAnon(p)) {
		ret = MF_RECOVERED;
		goto out;
	}

	/*
	 * Now truncate the page in the page cache. This is really
	 * more like a "temporary hole punch"
	 * Don't do this for block devices when someone else
	 * has a reference, because it could be file system metadata
	 * and that's not safe to truncate.
	 */
	mapping = page_mapping(p);
	if (!mapping) {
		/*
		 * Page has been teared down in the meanwhile
		 */
		ret = MF_FAILED;
		goto out;
	}

	/*
	 * Truncation is a bit tricky. Enable it per file system for now.
	 *
	 * Open: to take i_mutex or not for this? Right now we don't.
	 */
	ret = truncate_error_page(p, pfn, mapping);
out:
	unlock_page(p);
	return ret;
}

/*
 * Dirty pagecache page
 * Issues: when the error hit a hole page the error is not properly
 * propagated.
 */
static int me_pagecache_dirty(struct page *p, unsigned long pfn)
{
	struct address_space *mapping = page_mapping(p);

	SetPageError(p);
	/* TBD: print more information about the file. */
	if (mapping) {
		/*
		 * IO error will be reported by write(), fsync(), etc.
		 * who check the mapping.
		 * This way the application knows that something went
		 * wrong with its dirty file data.
		 *
		 * There's one open issue:
		 *
		 * The EIO will be only reported on the next IO
		 * operation and then cleared through the IO map.
		 * Normally Linux has two mechanisms to pass IO error
		 * first through the AS_EIO flag in the address space
		 * and then through the PageError flag in the page.
		 * Since we drop pages on memory failure handling the
		 * only mechanism open to use is through AS_AIO.
		 *
		 * This has the disadvantage that it gets cleared on
		 * the first operation that returns an error, while
		 * the PageError bit is more sticky and only cleared
		 * when the page is reread or dropped.  If an
		 * application assumes it will always get error on
		 * fsync, but does other operations on the fd before
		 * and the page is dropped between then the error
		 * will not be properly reported.
		 *
		 * This can already happen even without hwpoisoned
		 * pages: first on metadata IO errors (which only
		 * report through AS_EIO) or when the page is dropped
		 * at the wrong time.
		 *
		 * So right now we assume that the application DTRT on
		 * the first EIO, but we're not worse than other parts
		 * of the kernel.
		 */
		mapping_set_error(mapping, -EIO);
	}

	return me_pagecache_clean(p, pfn);
}

/*
 * Clean and dirty swap cache.
 *
 * Dirty swap cache page is tricky to handle. The page could live both in page
 * cache and swap cache(ie. page is freshly swapped in). So it could be
 * referenced concurrently by 2 types of PTEs:
 * normal PTEs and swap PTEs. We try to handle them consistently by calling
 * try_to_unmap(TTU_IGNORE_HWPOISON) to convert the normal PTEs to swap PTEs,
 * and then
 *      - clear dirty bit to prevent IO
 *      - remove from LRU
 *      - but keep in the swap cache, so that when we return to it on
 *        a later page fault, we know the application is accessing
 *        corrupted data and shall be killed (we installed simple
 *        interception code in do_swap_page to catch it).
 *
 * Clean swap cache pages can be directly isolated. A later page fault will
 * bring in the known good data from disk.
 */
static int me_swapcache_dirty(struct page *p, unsigned long pfn)
{
	int ret;

	ClearPageDirty(p);
	/* Trigger EIO in shmem: */
	ClearPageUptodate(p);

	ret = delete_from_lru_cache(p) ? MF_FAILED : MF_DELAYED;
	unlock_page(p);
	return ret;
}

static int me_swapcache_clean(struct page *p, unsigned long pfn)
{
	int ret;

	delete_from_swap_cache(p);

	ret = delete_from_lru_cache(p) ? MF_FAILED : MF_RECOVERED;
	unlock_page(p);
	return ret;
}

/*
 * Huge pages. Needs work.
 * Issues:
 * - Error on hugepage is contained in hugepage unit (not in raw page unit.)
 *   To narrow down kill region to one page, we need to break up pmd.
 */
static int me_huge_page(struct page *p, unsigned long pfn)
{
	int res;
	struct page *hpage = compound_head(p);
	struct address_space *mapping;

	if (!PageHuge(hpage))
		return MF_DELAYED;

	mapping = page_mapping(hpage);
	if (mapping) {
		res = truncate_error_page(hpage, pfn, mapping);
		unlock_page(hpage);
	} else {
		res = MF_FAILED;
		unlock_page(hpage);
		/*
		 * migration entry prevents later access on error anonymous
		 * hugepage, so we can free and dissolve it into buddy to
		 * save healthy subpages.
		 */
		if (PageAnon(hpage))
			put_page(hpage);
		if (!dissolve_free_huge_page(p) && take_page_off_buddy(p)) {
			page_ref_inc(p);
			res = MF_RECOVERED;
		}
	}

	return res;
}

/*
 * Various page states we can handle.
 *
 * A page state is defined by its current page->flags bits.
 * The table matches them in order and calls the right handler.
 *
 * This is quite tricky because we can access page at any time
 * in its live cycle, so all accesses have to be extremely careful.
 *
 * This is not complete. More states could be added.
 * For any missing state don't attempt recovery.
 */

#define dirty		(1UL << PG_dirty)
#define sc		((1UL << PG_swapcache) | (1UL << PG_swapbacked))
#define unevict		(1UL << PG_unevictable)
#define mlock		(1UL << PG_mlocked)
#define lru		(1UL << PG_lru)
#define head		(1UL << PG_head)
#define slab		(1UL << PG_slab)
#define reserved	(1UL << PG_reserved)

static struct page_state {
	unsigned long mask;
	unsigned long res;
	enum mf_action_page_type type;

	/* Callback ->action() has to unlock the relevant page inside it. */
	int (*action)(struct page *p, unsigned long pfn);
} error_states[] = {
	{ reserved,	reserved,	MF_MSG_KERNEL,	me_kernel },
	/*
	 * free pages are specially detected outside this table:
	 * PG_buddy pages only make a small fraction of all free pages.
	 */

	/*
	 * Could in theory check if slab page is free or if we can drop
	 * currently unused objects without touching them. But just
	 * treat it as standard kernel for now.
	 */
	{ slab,		slab,		MF_MSG_SLAB,	me_kernel },

	{ head,		head,		MF_MSG_HUGE,		me_huge_page },

	{ sc|dirty,	sc|dirty,	MF_MSG_DIRTY_SWAPCACHE,	me_swapcache_dirty },
	{ sc|dirty,	sc,		MF_MSG_CLEAN_SWAPCACHE,	me_swapcache_clean },

	{ mlock|dirty,	mlock|dirty,	MF_MSG_DIRTY_MLOCKED_LRU,	me_pagecache_dirty },
	{ mlock|dirty,	mlock,		MF_MSG_CLEAN_MLOCKED_LRU,	me_pagecache_clean },

	{ unevict|dirty, unevict|dirty,	MF_MSG_DIRTY_UNEVICTABLE_LRU,	me_pagecache_dirty },
	{ unevict|dirty, unevict,	MF_MSG_CLEAN_UNEVICTABLE_LRU,	me_pagecache_clean },

	{ lru|dirty,	lru|dirty,	MF_MSG_DIRTY_LRU,	me_pagecache_dirty },
	{ lru|dirty,	lru,		MF_MSG_CLEAN_LRU,	me_pagecache_clean },

	/*
	 * Catchall entry: must be at end.
	 */
	{ 0,		0,		MF_MSG_UNKNOWN,	me_unknown },
};

#undef dirty
#undef sc
#undef unevict
#undef mlock
#undef lru
#undef head
#undef slab
#undef reserved

/*
 * "Dirty/Clean" indication is not 100% accurate due to the possibility of
 * setting PG_dirty outside page lock. See also comment above set_page_dirty().
 */
static void action_result(unsigned long pfn, enum mf_action_page_type type,
			  enum mf_result result)
{
	trace_memory_failure_event(pfn, type, result);

	pr_err("Memory failure: %#lx: recovery action for %s: %s\n",
		pfn, action_page_types[type], action_name[result]);
}

static int page_action(struct page_state *ps, struct page *p,
			unsigned long pfn)
{
	int result;
	int count;

	/* page p should be unlocked after returning from ps->action().  */
	result = ps->action(p, pfn);

	count = page_count(p) - 1;
	if (ps->action == me_swapcache_dirty && result == MF_DELAYED)
		count--;
	if (count > 0) {
		pr_err("Memory failure: %#lx: %s still referenced by %d users\n",
		       pfn, action_page_types[ps->type], count);
		result = MF_FAILED;
	}
	action_result(pfn, ps->type, result);

	/* Could do more checks here if page looks ok */
	/*
	 * Could adjust zone counters here to correct for the missing page.
	 */

	return (result == MF_RECOVERED || result == MF_DELAYED) ? 0 : -EBUSY;
}

/*
 * Return true if a page type of a given page is supported by hwpoison
 * mechanism (while handling could fail), otherwise false.  This function
 * does not return true for hugetlb or device memory pages, so it's assumed
 * to be called only in the context where we never have such pages.
 */
static inline bool HWPoisonHandlable(struct page *page)
{
	return PageLRU(page) || __PageMovable(page);
}

/**
 * __get_hwpoison_page() - Get refcount for memory error handling:
 * @page:	raw error page (hit by memory error)
 *
 * Return: return 0 if failed to grab the refcount, otherwise true (some
 * non-zero value.)
 */
static int __get_hwpoison_page(struct page *page)
{
	struct page *head = compound_head(page);
	int ret = 0;
	bool hugetlb = false;

	ret = get_hwpoison_huge_page(head, &hugetlb);
	if (hugetlb)
		return ret;

	/*
	 * This check prevents from calling get_hwpoison_unless_zero()
	 * for any unsupported type of page in order to reduce the risk of
	 * unexpected races caused by taking a page refcount.
	 */
	if (!HWPoisonHandlable(head))
		return 0;

	if (PageTransHuge(head)) {
		/*
		 * Non anonymous thp exists only in allocation/free time. We
		 * can't handle such a case correctly, so let's give it up.
		 * This should be better than triggering BUG_ON when kernel
		 * tries to touch the "partially handled" page.
		 */
		if (!PageAnon(head)) {
			pr_err("Memory failure: %#lx: non anonymous thp\n",
				page_to_pfn(page));
			return 0;
		}
	}

	if (get_page_unless_zero(head)) {
		if (head == compound_head(page))
			return 1;

		pr_info("Memory failure: %#lx cannot catch tail\n",
			page_to_pfn(page));
		put_page(head);
	}

	return 0;
}

/*
 * Safely get reference count of an arbitrary page.
 *
 * Returns 0 for a free page, 1 for an in-use page,
 * -EIO for a page-type we cannot handle and -EBUSY if we raced with an
 * allocation.
 * We only incremented refcount in case the page was already in-use and it
 * is a known type we can handle.
 */
static int get_any_page(struct page *p, unsigned long flags)
{
	int ret = 0, pass = 0;
	bool count_increased = false;

	if (flags & MF_COUNT_INCREASED)
		count_increased = true;

try_again:
	if (!count_increased && !__get_hwpoison_page(p)) {
		if (page_count(p)) {
			/* We raced with an allocation, retry. */
			if (pass++ < 3)
				goto try_again;
			ret = -EBUSY;
		} else if (!PageHuge(p) && !is_free_buddy_page(p)) {
			/* We raced with put_page, retry. */
			if (pass++ < 3)
				goto try_again;
			ret = -EIO;
		}
	} else {
		if (PageHuge(p) || HWPoisonHandlable(p)) {
			ret = 1;
		} else {
			/*
			 * A page we cannot handle. Check whether we can turn
			 * it into something we can handle.
			 */
			if (pass++ < 3) {
				put_page(p);
				shake_page(p, 1);
				count_increased = false;
				goto try_again;
			}
			put_page(p);
			ret = -EIO;
		}
	}

	return ret;
}

static int get_hwpoison_page(struct page *p, unsigned long flags,
			     enum mf_flags ctxt)
{
	int ret;

	zone_pcp_disable(page_zone(p));
	if (ctxt == MF_SOFT_OFFLINE)
		ret = get_any_page(p, flags);
	else
		ret = __get_hwpoison_page(p);
	zone_pcp_enable(page_zone(p));

	return ret;
}

/*
 * Do all that is necessary to remove user space mappings. Unmap
 * the pages and send SIGBUS to the processes if the data was dirty.
 */
static bool hwpoison_user_mappings(struct page *p, unsigned long pfn,
				  int flags, struct page **hpagep)
{
	enum ttu_flags ttu = TTU_IGNORE_MLOCK;
	struct address_space *mapping;
	LIST_HEAD(tokill);
	bool unmap_success = true;
	int kill = 1, forcekill;
	struct page *hpage = *hpagep;
	bool mlocked = PageMlocked(hpage);

	/*
	 * Here we are interested only in user-mapped pages, so skip any
	 * other types of pages.
	 */
	if (PageReserved(p) || PageSlab(p))
		return true;
	if (!(PageLRU(hpage) || PageHuge(p)))
		return true;

	/*
	 * This check implies we don't kill processes if their pages
	 * are in the swap cache early. Those are always late kills.
	 */
	if (!page_mapped(hpage))
		return true;

	if (PageKsm(p)) {
		pr_err("Memory failure: %#lx: can't handle KSM pages.\n", pfn);
		return false;
	}

	if (PageSwapCache(p)) {
		pr_err("Memory failure: %#lx: keeping poisoned page in swap cache\n",
			pfn);
		ttu |= TTU_IGNORE_HWPOISON;
	}

	/*
	 * Propagate the dirty bit from PTEs to struct page first, because we
	 * need this to decide if we should kill or just drop the page.
	 * XXX: the dirty test could be racy: set_page_dirty() may not always
	 * be called inside page lock (it's recommended but not enforced).
	 */
	mapping = page_mapping(hpage);
	if (!(flags & MF_MUST_KILL) && !PageDirty(hpage) && mapping &&
	    mapping_can_writeback(mapping)) {
		if (page_mkclean(hpage)) {
			SetPageDirty(hpage);
		} else {
			kill = 0;
			ttu |= TTU_IGNORE_HWPOISON;
			pr_info("Memory failure: %#lx: corrupted page was clean: dropped without side effects\n",
				pfn);
		}
	}

	/*
	 * First collect all the processes that have the page
	 * mapped in dirty form.  This has to be done before try_to_unmap,
	 * because ttu takes the rmap data structures down.
	 *
	 * Error handling: We ignore errors here because
	 * there's nothing that can be done.
	 */
	if (kill)
		collect_procs(hpage, &tokill, flags & MF_ACTION_REQUIRED);

	if (!PageHuge(hpage)) {
		unmap_success = try_to_unmap(hpage, ttu);
	} else {
		if (!PageAnon(hpage)) {
			/*
			 * For hugetlb pages in shared mappings, try_to_unmap
			 * could potentially call huge_pmd_unshare.  Because of
			 * this, take semaphore in write mode here and set
			 * TTU_RMAP_LOCKED to indicate we have taken the lock
			 * at this higer level.
			 */
			mapping = hugetlb_page_mapping_lock_write(hpage);
			if (mapping) {
				unmap_success = try_to_unmap(hpage,
						     ttu|TTU_RMAP_LOCKED);
				i_mmap_unlock_write(mapping);
			} else {
				pr_info("Memory failure: %#lx: could not lock mapping for mapped huge page\n", pfn);
				unmap_success = false;
			}
		} else {
			unmap_success = try_to_unmap(hpage, ttu);
		}
	}
	if (!unmap_success)
		pr_err("Memory failure: %#lx: failed to unmap page (mapcount=%d)\n",
		       pfn, page_mapcount(hpage));

	/*
	 * try_to_unmap() might put mlocked page in lru cache, so call
	 * shake_page() again to ensure that it's flushed.
	 */
	if (mlocked)
		shake_page(hpage, 0);

	/*
	 * Now that the dirty bit has been propagated to the
	 * struct page and all unmaps done we can decide if
	 * killing is needed or not.  Only kill when the page
	 * was dirty or the process is not restartable,
	 * otherwise the tokill list is merely
	 * freed.  When there was a problem unmapping earlier
	 * use a more force-full uncatchable kill to prevent
	 * any accesses to the poisoned memory.
	 */
	forcekill = PageDirty(hpage) || (flags & MF_MUST_KILL);
	kill_procs(&tokill, forcekill, !unmap_success, pfn, flags);

	return unmap_success;
}

static int identify_page_state(unsigned long pfn, struct page *p,
				unsigned long page_flags)
{
	struct page_state *ps;

	/*
	 * The first check uses the current page flags which may not have any
	 * relevant information. The second check with the saved page flags is
	 * carried out only if the first check can't determine the page status.
	 */
	for (ps = error_states;; ps++)
		if ((p->flags & ps->mask) == ps->res)
			break;

	page_flags |= (p->flags & (1UL << PG_dirty));

	if (!ps->mask)
		for (ps = error_states;; ps++)
			if ((page_flags & ps->mask) == ps->res)
				break;
	return page_action(ps, p, pfn);
}

static int try_to_split_thp_page(struct page *page, const char *msg)
{
	lock_page(page);
	if (!PageAnon(page) || unlikely(split_huge_page(page))) {
		unsigned long pfn = page_to_pfn(page);

		unlock_page(page);
		if (!PageAnon(page))
			pr_info("%s: %#lx: non anonymous thp\n", msg, pfn);
		else
			pr_info("%s: %#lx: thp split failed\n", msg, pfn);
		put_page(page);
		return -EBUSY;
	}
	unlock_page(page);

	return 0;
}

static int memory_failure_hugetlb(unsigned long pfn, int flags)
{
	struct page *p = pfn_to_page(pfn);
	struct page *head = compound_head(p);
	int res;
	unsigned long page_flags;

	if (TestSetPageHWPoison(head)) {
		pr_err("Memory failure: %#lx: already hardware poisoned\n",
		       pfn);
		return -EHWPOISON;
	}

	num_poisoned_pages_inc();

	if (!(flags & MF_COUNT_INCREASED) && !get_hwpoison_page(p, flags, 0)) {
		/*
		 * Check "filter hit" and "race with other subpage."
		 */
		lock_page(head);
		if (PageHWPoison(head)) {
			if ((hwpoison_filter(p) && TestClearPageHWPoison(p))
			    || (p != head && TestSetPageHWPoison(head))) {
				num_poisoned_pages_dec();
				unlock_page(head);
				return 0;
			}
		}
		unlock_page(head);
		res = MF_FAILED;
		if (!dissolve_free_huge_page(p) && take_page_off_buddy(p)) {
			page_ref_inc(p);
			res = MF_RECOVERED;
		}
		action_result(pfn, MF_MSG_FREE_HUGE, res);
		return res == MF_RECOVERED ? 0 : -EBUSY;
	}

	lock_page(head);
	page_flags = head->flags;

	if (!PageHWPoison(head)) {
		pr_err("Memory failure: %#lx: just unpoisoned\n", pfn);
		num_poisoned_pages_dec();
		unlock_page(head);
		put_page(head);
		return 0;
	}

	/*
	 * TODO: hwpoison for pud-sized hugetlb doesn't work right now, so
	 * simply disable it. In order to make it work properly, we need
	 * make sure that:
	 *  - conversion of a pud that maps an error hugetlb into hwpoison
	 *    entry properly works, and
	 *  - other mm code walking over page table is aware of pud-aligned
	 *    hwpoison entries.
	 */
	if (huge_page_size(page_hstate(head)) > PMD_SIZE) {
		action_result(pfn, MF_MSG_NON_PMD_HUGE, MF_IGNORED);
		res = -EBUSY;
		goto out;
	}

	if (!hwpoison_user_mappings(p, pfn, flags, &head)) {
		action_result(pfn, MF_MSG_UNMAP_FAILED, MF_IGNORED);
		res = -EBUSY;
		goto out;
	}

	return identify_page_state(pfn, p, page_flags);
out:
	unlock_page(head);
	return res;
}

static int memory_failure_dev_pagemap(unsigned long pfn, int flags,
		struct dev_pagemap *pgmap)
{
	struct page *page = pfn_to_page(pfn);
	const bool unmap_success = true;
	unsigned long size = 0;
	struct to_kill *tk;
	LIST_HEAD(tokill);
	int rc = -EBUSY;
	loff_t start;
	dax_entry_t cookie;

	if (flags & MF_COUNT_INCREASED)
		/*
		 * Drop the extra refcount in case we come from madvise().
		 */
		put_page(page);

	/* device metadata space is not recoverable */
	if (!pgmap_pfn_valid(pgmap, pfn)) {
		rc = -ENXIO;
		goto out;
	}

	/*
	 * Prevent the inode from being freed while we are interrogating
	 * the address_space, typically this would be handled by
	 * lock_page(), but dax pages do not use the page lock. This
	 * also prevents changes to the mapping of this pfn until
	 * poison signaling is complete.
	 */
	cookie = dax_lock_page(page);
	if (!cookie)
		goto out;

	if (hwpoison_filter(page)) {
		rc = 0;
		goto unlock;
	}

	if (pgmap->type == MEMORY_DEVICE_PRIVATE) {
		/*
		 * TODO: Handle HMM pages which may need coordination
		 * with device-side memory.
		 */
		goto unlock;
	}

	/*
	 * Use this flag as an indication that the dax page has been
	 * remapped UC to prevent speculative consumption of poison.
	 */
	SetPageHWPoison(page);

	/*
	 * Unlike System-RAM there is no possibility to swap in a
	 * different physical page at a given virtual address, so all
	 * userspace consumption of ZONE_DEVICE memory necessitates
	 * SIGBUS (i.e. MF_MUST_KILL)
	 */
	flags |= MF_ACTION_REQUIRED | MF_MUST_KILL;
	collect_procs(page, &tokill, flags & MF_ACTION_REQUIRED);

	list_for_each_entry(tk, &tokill, nd)
		if (tk->size_shift)
			size = max(size, 1UL << tk->size_shift);
	if (size) {
		/*
		 * Unmap the largest mapping to avoid breaking up
		 * device-dax mappings which are constant size. The
		 * actual size of the mapping being torn down is
		 * communicated in siginfo, see kill_proc()
		 */
		start = (page->index << PAGE_SHIFT) & ~(size - 1);
		unmap_mapping_range(page->mapping, start, size, 0);
	}
	kill_procs(&tokill, flags & MF_MUST_KILL, !unmap_success, pfn, flags);
	rc = 0;
unlock:
	dax_unlock_page(page, cookie);
out:
	/* drop pgmap ref acquired in caller */
	put_dev_pagemap(pgmap);
	action_result(pfn, MF_MSG_DAX, rc ? MF_FAILED : MF_RECOVERED);
	return rc;
}

/**
 * memory_failure - Handle memory failure of a page.
 * @pfn: Page Number of the corrupted page
 * @flags: fine tune action taken
 *
 * This function is called by the low level machine check code
 * of an architecture when it detects hardware memory corruption
 * of a page. It tries its best to recover, which includes
 * dropping pages, killing processes etc.
 *
 * The function is primarily of use for corruptions that
 * happen outside the current execution context (e.g. when
 * detected by a background scrubber)
 *
 * Must run in process context (e.g. a work queue) with interrupts
 * enabled and no spinlocks hold.
 */
int memory_failure(unsigned long pfn, int flags)
{
	struct page *p;
	struct page *hpage;
	struct page *orig_head;
	struct dev_pagemap *pgmap;
	int res = 0;
	unsigned long page_flags;
	bool retry = true;
	static DEFINE_MUTEX(mf_mutex);

	if (!sysctl_memory_failure_recovery)
		panic("Memory failure on page %lx", pfn);

	p = pfn_to_online_page(pfn);
	if (!p) {
		if (pfn_valid(pfn)) {
			pgmap = get_dev_pagemap(pfn, NULL);
			if (pgmap)
				return memory_failure_dev_pagemap(pfn, flags,
								  pgmap);
		}
		pr_err("Memory failure: %#lx: memory outside kernel control\n",
			pfn);
		return -ENXIO;
	}

	mutex_lock(&mf_mutex);

try_again:
	if (PageHuge(p)) {
		res = memory_failure_hugetlb(pfn, flags);
		goto unlock_mutex;
	}

	if (TestSetPageHWPoison(p)) {
		pr_err("Memory failure: %#lx: already hardware poisoned\n",
			pfn);
		res = -EHWPOISON;
		goto unlock_mutex;
	}

	orig_head = hpage = compound_head(p);
	num_poisoned_pages_inc();

	/*
	 * We need/can do nothing about count=0 pages.
	 * 1) it's a free page, and therefore in safe hand:
	 *    prep_new_page() will be the gate keeper.
	 * 2) it's part of a non-compound high order page.
	 *    Implies some kernel user: cannot stop them from
	 *    R/W the page; let's pray that the page has been
	 *    used and will be freed some time later.
	 * In fact it's dangerous to directly bump up page count from 0,
	 * that may make page_ref_freeze()/page_ref_unfreeze() mismatch.
	 */
	if (!(flags & MF_COUNT_INCREASED) && !get_hwpoison_page(p, flags, 0)) {
		if (is_free_buddy_page(p)) {
			if (take_page_off_buddy(p)) {
				page_ref_inc(p);
				res = MF_RECOVERED;
			} else {
				/* We lost the race, try again */
				if (retry) {
					ClearPageHWPoison(p);
					num_poisoned_pages_dec();
					retry = false;
					goto try_again;
				}
				res = MF_FAILED;
			}
			action_result(pfn, MF_MSG_BUDDY, res);
			res = res == MF_RECOVERED ? 0 : -EBUSY;
		} else {
			action_result(pfn, MF_MSG_KERNEL_HIGH_ORDER, MF_IGNORED);
			res = -EBUSY;
		}
		goto unlock_mutex;
	}

	if (PageTransHuge(hpage)) {
		if (try_to_split_thp_page(p, "Memory Failure") < 0) {
			action_result(pfn, MF_MSG_UNSPLIT_THP, MF_IGNORED);
			res = -EBUSY;
			goto unlock_mutex;
		}
		VM_BUG_ON_PAGE(!page_count(p), p);
	}

	/*
	 * We ignore non-LRU pages for good reasons.
	 * - PG_locked is only well defined for LRU pages and a few others
	 * - to avoid races with __SetPageLocked()
	 * - to avoid races with __SetPageSlab*() (and more non-atomic ops)
	 * The check (unnecessarily) ignores LRU pages being isolated and
	 * walked by the page reclaim code, however that's not a big loss.
	 */
	shake_page(p, 0);

	lock_page(p);

	/*
	 * The page could have changed compound pages during the locking.
	 * If this happens just bail out.
	 */
	if (PageCompound(p) && compound_head(p) != orig_head) {
		action_result(pfn, MF_MSG_DIFFERENT_COMPOUND, MF_IGNORED);
		res = -EBUSY;
		goto unlock_page;
	}

	/*
	 * We use page flags to determine what action should be taken, but
	 * the flags can be modified by the error containment action.  One
	 * example is an mlocked page, where PG_mlocked is cleared by
	 * page_remove_rmap() in try_to_unmap_one(). So to determine page status
	 * correctly, we save a copy of the page flags at this time.
	 */
	page_flags = p->flags;

	/*
	 * unpoison always clear PG_hwpoison inside page lock
	 */
	if (!PageHWPoison(p)) {
		pr_err("Memory failure: %#lx: just unpoisoned\n", pfn);
		num_poisoned_pages_dec();
		unlock_page(p);
		put_page(p);
		goto unlock_mutex;
	}
	if (hwpoison_filter(p)) {
		if (TestClearPageHWPoison(p))
			num_poisoned_pages_dec();
		unlock_page(p);
		put_page(p);
		goto unlock_mutex;
	}

	/*
	 * __munlock_pagevec may clear a writeback page's LRU flag without
	 * page_lock. We need wait writeback completion for this page or it
	 * may trigger vfs BUG while evict inode.
	 */
	if (!PageTransTail(p) && !PageLRU(p) && !PageWriteback(p))
		goto identify_page_state;

	/*
	 * It's very difficult to mess with pages currently under IO
	 * and in many cases impossible, so we just avoid it here.
	 */
	wait_on_page_writeback(p);

	/*
	 * Now take care of user space mappings.
	 * Abort on fail: __delete_from_page_cache() assumes unmapped page.
	 */
	if (!hwpoison_user_mappings(p, pfn, flags, &p)) {
		action_result(pfn, MF_MSG_UNMAP_FAILED, MF_IGNORED);
		res = -EBUSY;
		goto unlock_page;
	}

	/*
	 * Torn down by someone else?
	 */
	if (PageLRU(p) && !PageSwapCache(p) && p->mapping == NULL) {
		action_result(pfn, MF_MSG_TRUNCATED_LRU, MF_IGNORED);
		res = -EBUSY;
		goto unlock_page;
	}

identify_page_state:
	res = identify_page_state(pfn, p, page_flags);
	mutex_unlock(&mf_mutex);
	return res;
unlock_page:
	unlock_page(p);
unlock_mutex:
	mutex_unlock(&mf_mutex);
	return res;
}
EXPORT_SYMBOL_GPL(memory_failure);

#define MEMORY_FAILURE_FIFO_ORDER	4
#define MEMORY_FAILURE_FIFO_SIZE	(1 << MEMORY_FAILURE_FIFO_ORDER)

struct memory_failure_entry {
	unsigned long pfn;
	int flags;
};

struct memory_failure_cpu {
	DECLARE_KFIFO(fifo, struct memory_failure_entry,
		      MEMORY_FAILURE_FIFO_SIZE);
	spinlock_t lock;
	struct work_struct work;
};

static DEFINE_PER_CPU(struct memory_failure_cpu, memory_failure_cpu);

/**
 * memory_failure_queue - Schedule handling memory failure of a page.
 * @pfn: Page Number of the corrupted page
 * @flags: Flags for memory failure handling
 *
 * This function is called by the low level hardware error handler
 * when it detects hardware memory corruption of a page. It schedules
 * the recovering of error page, including dropping pages, killing
 * processes etc.
 *
 * The function is primarily of use for corruptions that
 * happen outside the current execution context (e.g. when
 * detected by a background scrubber)
 *
 * Can run in IRQ context.
 */
void memory_failure_queue(unsigned long pfn, int flags)
{
	struct memory_failure_cpu *mf_cpu;
	unsigned long proc_flags;
	struct memory_failure_entry entry = {
		.pfn =		pfn,
		.flags =	flags,
	};

	mf_cpu = &get_cpu_var(memory_failure_cpu);
	spin_lock_irqsave(&mf_cpu->lock, proc_flags);
	if (kfifo_put(&mf_cpu->fifo, entry))
		schedule_work_on(smp_processor_id(), &mf_cpu->work);
	else
		pr_err("Memory failure: buffer overflow when queuing memory failure at %#lx\n",
		       pfn);
	spin_unlock_irqrestore(&mf_cpu->lock, proc_flags);
	put_cpu_var(memory_failure_cpu);
}
EXPORT_SYMBOL_GPL(memory_failure_queue);

static void memory_failure_work_func(struct work_struct *work)
{
	struct memory_failure_cpu *mf_cpu;
	struct memory_failure_entry entry = { 0, };
	unsigned long proc_flags;
	int gotten;

	mf_cpu = container_of(work, struct memory_failure_cpu, work);
	for (;;) {
		spin_lock_irqsave(&mf_cpu->lock, proc_flags);
		gotten = kfifo_get(&mf_cpu->fifo, &entry);
		spin_unlock_irqrestore(&mf_cpu->lock, proc_flags);
		if (!gotten)
			break;
		if (entry.flags & MF_SOFT_OFFLINE)
			soft_offline_page(entry.pfn, entry.flags);
		else
			memory_failure(entry.pfn, entry.flags);
	}
}

/*
 * Process memory_failure work queued on the specified CPU.
 * Used to avoid return-to-userspace racing with the memory_failure workqueue.
 */
void memory_failure_queue_kick(int cpu)
{
	struct memory_failure_cpu *mf_cpu;

	mf_cpu = &per_cpu(memory_failure_cpu, cpu);
	cancel_work_sync(&mf_cpu->work);
	memory_failure_work_func(&mf_cpu->work);
}

static int __init memory_failure_init(void)
{
	struct memory_failure_cpu *mf_cpu;
	int cpu;

	for_each_possible_cpu(cpu) {
		mf_cpu = &per_cpu(memory_failure_cpu, cpu);
		spin_lock_init(&mf_cpu->lock);
		INIT_KFIFO(mf_cpu->fifo);
		INIT_WORK(&mf_cpu->work, memory_failure_work_func);
	}

	return 0;
}
core_initcall(memory_failure_init);

#define unpoison_pr_info(fmt, pfn, rs)			\
({							\
	if (__ratelimit(rs))				\
		pr_info(fmt, pfn);			\
})

/**
 * unpoison_memory - Unpoison a previously poisoned page
 * @pfn: Page number of the to be unpoisoned page
 *
 * Software-unpoison a page that has been poisoned by
 * memory_failure() earlier.
 *
 * This is only done on the software-level, so it only works
 * for linux injected failures, not real hardware failures
 *
 * Returns 0 for success, otherwise -errno.
 */
int unpoison_memory(unsigned long pfn)
{
	struct page *page;
	struct page *p;
	int freeit = 0;
	unsigned long flags = 0;
	static DEFINE_RATELIMIT_STATE(unpoison_rs, DEFAULT_RATELIMIT_INTERVAL,
					DEFAULT_RATELIMIT_BURST);

	if (!pfn_valid(pfn))
		return -ENXIO;

	p = pfn_to_page(pfn);
	page = compound_head(p);

	if (!PageHWPoison(p)) {
		unpoison_pr_info("Unpoison: Page was already unpoisoned %#lx\n",
				 pfn, &unpoison_rs);
		return 0;
	}

	if (page_count(page) > 1) {
		unpoison_pr_info("Unpoison: Someone grabs the hwpoison page %#lx\n",
				 pfn, &unpoison_rs);
		return 0;
	}

	if (page_mapped(page)) {
		unpoison_pr_info("Unpoison: Someone maps the hwpoison page %#lx\n",
				 pfn, &unpoison_rs);
		return 0;
	}

	if (page_mapping(page)) {
		unpoison_pr_info("Unpoison: the hwpoison page has non-NULL mapping %#lx\n",
				 pfn, &unpoison_rs);
		return 0;
	}

	/*
	 * unpoison_memory() can encounter thp only when the thp is being
	 * worked by memory_failure() and the page lock is not held yet.
	 * In such case, we yield to memory_failure() and make unpoison fail.
	 */
	if (!PageHuge(page) && PageTransHuge(page)) {
		unpoison_pr_info("Unpoison: Memory failure is now running on %#lx\n",
				 pfn, &unpoison_rs);
		return 0;
	}

	if (!get_hwpoison_page(p, flags, 0)) {
		if (TestClearPageHWPoison(p))
			num_poisoned_pages_dec();
		unpoison_pr_info("Unpoison: Software-unpoisoned free page %#lx\n",
				 pfn, &unpoison_rs);
		return 0;
	}

	lock_page(page);
	/*
	 * This test is racy because PG_hwpoison is set outside of page lock.
	 * That's acceptable because that won't trigger kernel panic. Instead,
	 * the PG_hwpoison page will be caught and isolated on the entrance to
	 * the free buddy page pool.
	 */
	if (TestClearPageHWPoison(page)) {
		unpoison_pr_info("Unpoison: Software-unpoisoned page %#lx\n",
				 pfn, &unpoison_rs);
		num_poisoned_pages_dec();
		freeit = 1;
	}
	unlock_page(page);

	put_page(page);
	if (freeit && !(pfn == my_zero_pfn(0) && page_count(p) == 1))
		put_page(page);

	return 0;
}
EXPORT_SYMBOL(unpoison_memory);

static bool isolate_page(struct page *page, struct list_head *pagelist)
{
	bool isolated = false;
	bool lru = PageLRU(page);

	if (PageHuge(page)) {
		isolated = isolate_huge_page(page, pagelist);
	} else {
		if (lru)
			isolated = !isolate_lru_page(page);
		else
			isolated = !isolate_movable_page(page, ISOLATE_UNEVICTABLE);

		if (isolated)
			list_add(&page->lru, pagelist);
	}

	if (isolated && lru)
		inc_node_page_state(page, NR_ISOLATED_ANON +
				    page_is_file_lru(page));

	/*
	 * If we succeed to isolate the page, we grabbed another refcount on
	 * the page, so we can safely drop the one we got from get_any_pages().
	 * If we failed to isolate the page, it means that we cannot go further
	 * and we will return an error, so drop the reference we got from
	 * get_any_pages() as well.
	 */
	put_page(page);
	return isolated;
}

/*
 * __soft_offline_page handles hugetlb-pages and non-hugetlb pages.
 * If the page is a non-dirty unmapped page-cache page, it simply invalidates.
 * If the page is mapped, it migrates the contents over.
 */
static int __soft_offline_page(struct page *page)
{
	int ret = 0;
	unsigned long pfn = page_to_pfn(page);
	struct page *hpage = compound_head(page);
	char const *msg_page[] = {"page", "hugepage"};
	bool huge = PageHuge(page);
	LIST_HEAD(pagelist);
	struct migration_target_control mtc = {
		.nid = NUMA_NO_NODE,
		.gfp_mask = GFP_USER | __GFP_MOVABLE | __GFP_RETRY_MAYFAIL,
	};

	/*
	 * Check PageHWPoison again inside page lock because PageHWPoison
	 * is set by memory_failure() outside page lock. Note that
	 * memory_failure() also double-checks PageHWPoison inside page lock,
	 * so there's no race between soft_offline_page() and memory_failure().
	 */
	lock_page(page);
	if (!PageHuge(page))
		wait_on_page_writeback(page);
	if (PageHWPoison(page)) {
		unlock_page(page);
		put_page(page);
		pr_info("soft offline: %#lx page already poisoned\n", pfn);
		return 0;
	}

	if (!PageHuge(page))
		/*
		 * Try to invalidate first. This should work for
		 * non dirty unmapped page cache pages.
		 */
		ret = invalidate_inode_page(page);
	unlock_page(page);

	/*
	 * RED-PEN would be better to keep it isolated here, but we
	 * would need to fix isolation locking first.
	 */
	if (ret) {
		pr_info("soft_offline: %#lx: invalidated\n", pfn);
		page_handle_poison(page, false, true);
		return 0;
	}

	if (isolate_page(hpage, &pagelist)) {
		ret = migrate_pages(&pagelist, alloc_migration_target, NULL,
			(unsigned long)&mtc, MIGRATE_SYNC, MR_MEMORY_FAILURE);
		if (!ret) {
			bool release = !huge;

			if (!page_handle_poison(page, huge, release))
				ret = -EBUSY;
		} else {
			if (!list_empty(&pagelist))
				putback_movable_pages(&pagelist);

			pr_info("soft offline: %#lx: %s migration failed %d, type %lx (%pGp)\n",
				pfn, msg_page[huge], ret, page->flags, &page->flags);
			if (ret > 0)
				ret = -EBUSY;
		}
	} else {
		pr_info("soft offline: %#lx: %s isolation failed, page count %d, type %lx (%pGp)\n",
			pfn, msg_page[huge], page_count(page), page->flags, &page->flags);
		ret = -EBUSY;
	}
	return ret;
}

static int soft_offline_in_use_page(struct page *page)
{
	struct page *hpage = compound_head(page);

	if (!PageHuge(page) && PageTransHuge(hpage))
		if (try_to_split_thp_page(page, "soft offline") < 0)
			return -EBUSY;
	return __soft_offline_page(page);
}

static int soft_offline_free_page(struct page *page)
{
	int rc = 0;

	if (!page_handle_poison(page, true, false))
		rc = -EBUSY;

	return rc;
}

static void put_ref_page(struct page *page)
{
	if (page)
		put_page(page);
}

/**
 * soft_offline_page - Soft offline a page.
 * @pfn: pfn to soft-offline
 * @flags: flags. Same as memory_failure().
 *
 * Returns 0 on success, otherwise negated errno.
 *
 * Soft offline a page, by migration or invalidation,
 * without killing anything. This is for the case when
 * a page is not corrupted yet (so it's still valid to access),
 * but has had a number of corrected errors and is better taken
 * out.
 *
 * The actual policy on when to do that is maintained by
 * user space.
 *
 * This should never impact any application or cause data loss,
 * however it might take some time.
 *
 * This is not a 100% solution for all memory, but tries to be
 * ``good enough'' for the majority of memory.
 */
int soft_offline_page(unsigned long pfn, int flags)
{
	int ret;
	bool try_again = true;
	struct page *page, *ref_page = NULL;

	WARN_ON_ONCE(!pfn_valid(pfn) && (flags & MF_COUNT_INCREASED));

	if (!pfn_valid(pfn))
		return -ENXIO;
	if (flags & MF_COUNT_INCREASED)
		ref_page = pfn_to_page(pfn);

	/* Only online pages can be soft-offlined (esp., not ZONE_DEVICE). */
	page = pfn_to_online_page(pfn);
	if (!page) {
		put_ref_page(ref_page);
		return -EIO;
	}

	if (PageHWPoison(page)) {
		pr_info("%s: %#lx page already poisoned\n", __func__, pfn);
		put_ref_page(ref_page);
		return 0;
	}

retry:
	get_online_mems();
	ret = get_hwpoison_page(page, flags, MF_SOFT_OFFLINE);
	put_online_mems();

	if (ret > 0) {
		ret = soft_offline_in_use_page(page);
	} else if (ret == 0) {
		if (soft_offline_free_page(page) && try_again) {
			try_again = false;
			goto retry;
		}
	} else if (ret == -EIO) {
		pr_info("%s: %#lx: unknown page type: %lx (%pGp)\n",
			 __func__, pfn, page->flags, &page->flags);
	}

	return ret;
}
