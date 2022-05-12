// SPDX-License-Identifier: GPL-2.0-only
/*
 *  linux/arch/arm/lib/uaccess_with_memcpy.c
 *
 *  Written by: Lennert Buytenhek and Nicolas Pitre
 *  Copyright (C) 2009 Marvell Semiconductor
 */

#include <linux/kernel.h>
#include <linux/ctype.h>
#include <linux/uaccess.h>
#include <linux/rwsem.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/hardirq.h> /* for in_atomic() */
#include <linux/gfp.h>
#include <linux/highmem.h>
#include <linux/hugetlb.h>
#include <asm/current.h>
#include <asm/page.h>

static int
pin_page_for_write(const void __user *_addr, pte_t **ptep, spinlock_t **ptlp)
{
	unsigned long addr = (unsigned long)_addr;
	pgd_t *pgd;
	p4d_t *p4d;
	pmd_t *pmd;
	pte_t *pte;
	pud_t *pud;
	spinlock_t *ptl;

	pgd = pgd_offset(current->mm, addr);
	if (unlikely(pgd_none(*pgd) || pgd_bad(*pgd)))
		return 0;

	p4d = p4d_offset(pgd, addr);
	if (unlikely(p4d_none(*p4d) || p4d_bad(*p4d)))
		return 0;

	pud = pud_offset(p4d, addr);
	if (unlikely(pud_none(*pud) || pud_bad(*pud)))
		return 0;

	pmd = pmd_offset(pud, addr);
	if (unlikely(pmd_none(*pmd)))
		return 0;

	/*
	 * A pmd can be bad if it refers to a HugeTLB or THP page.
	 *
	 * Both THP and HugeTLB pages have the same pmd layout
	 * and should not be manipulated by the pte functions.
	 *
	 * Lock the page table for the destination and check
	 * to see that it's still huge and whether or not we will
	 * need to fault on write.
	 */
	if (unlikely(pmd_thp_or_huge(*pmd))) {
		ptl = &current->mm->page_table_lock;
		spin_lock(ptl);
		if (unlikely(!pmd_thp_or_huge(*pmd)
			|| pmd_hugewillfault(*pmd))) {
			spin_unlock(ptl);
			return 0;
		}

		*ptep = NULL;
		*ptlp = ptl;
		return 1;
	}

	if (unlikely(pmd_bad(*pmd)))
		return 0;

	pte = pte_offset_map_lock(current->mm, pmd, addr, &ptl);
	if (unlikely(!pte_present(*pte) || !pte_young(*pte) ||
	    !pte_write(*pte) || !pte_dirty(*pte))) {
		pte_unmap_unlock(pte, ptl);
		return 0;
	}

	*ptep = pte;
	*ptlp = ptl;

	return 1;
}

static unsigned long noinline
__copy_to_user_memcpy(void __user *to, const void *from, unsigned long n)
{
	unsigned long ua_flags;
	int atomic;

	if (uaccess_kernel()) {
		memcpy((void *)to, from, n);
		return 0;
	}

	/* the mmap semaphore is taken only if not in an atomic context */
	atomic = faulthandler_disabled();

	if (!atomic)
		mmap_read_lock(current->mm);
	while (n) {
		pte_t *pte;
		spinlock_t *ptl;
		int tocopy;

		while (!pin_page_for_write(to, &pte, &ptl)) {
			if (!atomic)
				mmap_read_unlock(current->mm);
			if (__put_user(0, (char __user *)to))
				goto out;
			if (!atomic)
				mmap_read_lock(current->mm);
		}

		tocopy = (~(unsigned long)to & ~PAGE_MASK) + 1;
		if (tocopy > n)
			tocopy = n;

		ua_flags = uaccess_save_and_enable();
		memcpy((void *)to, from, tocopy);
		uaccess_restore(ua_flags);
		to += tocopy;
		from += tocopy;
		n -= tocopy;

		if (pte)
			pte_unmap_unlock(pte, ptl);
		else
			spin_unlock(ptl);
	}
	if (!atomic)
		mmap_read_unlock(current->mm);

out:
	return n;
}

unsigned long
arm_copy_to_user(void __user *to, const void *from, unsigned long n)
{
	/*
	 * This test is stubbed out of the main function above to keep
	 * the overhead for small copies low by avoiding a large
	 * register dump on the stack just to reload them right away.
	 * With frame pointer disabled, tail call optimization kicks in
	 * as well making this test almost invisible.
	 */
	if (n < 64) {
		unsigned long ua_flags = uaccess_save_and_enable();
		n = __copy_to_user_std(to, from, n);
		uaccess_restore(ua_flags);
	} else {
		n = __copy_to_user_memcpy(uaccess_mask_range_ptr(to, n),
					  from, n);
	}
	return n;
}
	
static unsigned long noinline
__clear_user_memset(void __user *addr, unsigned long n)
{
	unsigned long ua_flags;

	if (uaccess_kernel()) {
		memset((void *)addr, 0, n);
		return 0;
	}

	mmap_read_lock(current->mm);
	while (n) {
		pte_t *pte;
		spinlock_t *ptl;
		int tocopy;

		while (!pin_page_for_write(addr, &pte, &ptl)) {
			mmap_read_unlock(current->mm);
			if (__put_user(0, (char __user *)addr))
				goto out;
			mmap_read_lock(current->mm);
		}

		tocopy = (~(unsigned long)addr & ~PAGE_MASK) + 1;
		if (tocopy > n)
			tocopy = n;

		ua_flags = uaccess_save_and_enable();
		memset((void *)addr, 0, tocopy);
		uaccess_restore(ua_flags);
		addr += tocopy;
		n -= tocopy;

		if (pte)
			pte_unmap_unlock(pte, ptl);
		else
			spin_unlock(ptl);
	}
	mmap_read_unlock(current->mm);

out:
	return n;
}

unsigned long arm_clear_user(void __user *addr, unsigned long n)
{
	/* See rational for this in __copy_to_user() above. */
	if (n < 64) {
		unsigned long ua_flags = uaccess_save_and_enable();
		n = __clear_user_std(addr, n);
		uaccess_restore(ua_flags);
	} else {
		n = __clear_user_memset(addr, n);
	}
	return n;
}

#if 0

/*
 * This code is disabled by default, but kept around in case the chosen
 * thresholds need to be revalidated.  Some overhead (small but still)
 * would be implied by a runtime determined variable threshold, and
 * so far the measurement on concerned targets didn't show a worthwhile
 * variation.
 *
 * Note that a fairly precise sched_clock() implementation is needed
 * for results to make some sense.
 */

#include <linux/vmalloc.h>

static int __init test_size_treshold(void)
{
	struct page *src_page, *dst_page;
	void *user_ptr, *kernel_ptr;
	unsigned long long t0, t1, t2;
	int size, ret;

	ret = -ENOMEM;
	src_page = alloc_page(GFP_KERNEL);
	if (!src_page)
		goto no_src;
	dst_page = alloc_page(GFP_KERNEL);
	if (!dst_page)
		goto no_dst;
	kernel_ptr = page_address(src_page);
	user_ptr = vmap(&dst_page, 1, VM_IOREMAP, __pgprot(__P010));
	if (!user_ptr)
		goto no_vmap;

	/* warm up the src page dcache */
	ret = __copy_to_user_memcpy(user_ptr, kernel_ptr, PAGE_SIZE);

	for (size = PAGE_SIZE; size >= 4; size /= 2) {
		t0 = sched_clock();
		ret |= __copy_to_user_memcpy(user_ptr, kernel_ptr, size);
		t1 = sched_clock();
		ret |= __copy_to_user_std(user_ptr, kernel_ptr, size);
		t2 = sched_clock();
		printk("copy_to_user: %d %llu %llu\n", size, t1 - t0, t2 - t1);
	}

	for (size = PAGE_SIZE; size >= 4; size /= 2) {
		t0 = sched_clock();
		ret |= __clear_user_memset(user_ptr, size);
		t1 = sched_clock();
		ret |= __clear_user_std(user_ptr, size);
		t2 = sched_clock();
		printk("clear_user: %d %llu %llu\n", size, t1 - t0, t2 - t1);
	}

	if (ret)
		ret = -EFAULT;

	vunmap(user_ptr);
no_vmap:
	put_page(dst_page);
no_dst:
	put_page(src_page);
no_src:
	return ret;
}

subsys_initcall(test_size_treshold);

#endif
