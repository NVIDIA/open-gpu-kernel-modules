// SPDX-License-Identifier: GPL-2.0-only
/*
 * kexec.c - kexec system call core code.
 * Copyright (C) 2002-2004 Eric Biederman  <ebiederm@xmission.com>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/capability.h>
#include <linux/mm.h>
#include <linux/file.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/kexec.h>
#include <linux/mutex.h>
#include <linux/list.h>
#include <linux/highmem.h>
#include <linux/syscalls.h>
#include <linux/reboot.h>
#include <linux/ioport.h>
#include <linux/hardirq.h>
#include <linux/elf.h>
#include <linux/elfcore.h>
#include <linux/utsname.h>
#include <linux/numa.h>
#include <linux/suspend.h>
#include <linux/device.h>
#include <linux/freezer.h>
#include <linux/pm.h>
#include <linux/cpu.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/console.h>
#include <linux/vmalloc.h>
#include <linux/swap.h>
#include <linux/syscore_ops.h>
#include <linux/compiler.h>
#include <linux/hugetlb.h>
#include <linux/objtool.h>
#include <linux/kmsg_dump.h>

#include <asm/page.h>
#include <asm/sections.h>

#include <crypto/hash.h>
#include "kexec_internal.h"

DEFINE_MUTEX(kexec_mutex);

/* Per cpu memory for storing cpu states in case of system crash. */
note_buf_t __percpu *crash_notes;

/* Flag to indicate we are going to kexec a new kernel */
bool kexec_in_progress = false;


/* Location of the reserved area for the crash kernel */
struct resource crashk_res = {
	.name  = "Crash kernel",
	.start = 0,
	.end   = 0,
	.flags = IORESOURCE_BUSY | IORESOURCE_SYSTEM_RAM,
	.desc  = IORES_DESC_CRASH_KERNEL
};
struct resource crashk_low_res = {
	.name  = "Crash kernel",
	.start = 0,
	.end   = 0,
	.flags = IORESOURCE_BUSY | IORESOURCE_SYSTEM_RAM,
	.desc  = IORES_DESC_CRASH_KERNEL
};

int kexec_should_crash(struct task_struct *p)
{
	/*
	 * If crash_kexec_post_notifiers is enabled, don't run
	 * crash_kexec() here yet, which must be run after panic
	 * notifiers in panic().
	 */
	if (crash_kexec_post_notifiers)
		return 0;
	/*
	 * There are 4 panic() calls in do_exit() path, each of which
	 * corresponds to each of these 4 conditions.
	 */
	if (in_interrupt() || !p->pid || is_global_init(p) || panic_on_oops)
		return 1;
	return 0;
}

int kexec_crash_loaded(void)
{
	return !!kexec_crash_image;
}
EXPORT_SYMBOL_GPL(kexec_crash_loaded);

/*
 * When kexec transitions to the new kernel there is a one-to-one
 * mapping between physical and virtual addresses.  On processors
 * where you can disable the MMU this is trivial, and easy.  For
 * others it is still a simple predictable page table to setup.
 *
 * In that environment kexec copies the new kernel to its final
 * resting place.  This means I can only support memory whose
 * physical address can fit in an unsigned long.  In particular
 * addresses where (pfn << PAGE_SHIFT) > ULONG_MAX cannot be handled.
 * If the assembly stub has more restrictive requirements
 * KEXEC_SOURCE_MEMORY_LIMIT and KEXEC_DEST_MEMORY_LIMIT can be
 * defined more restrictively in <asm/kexec.h>.
 *
 * The code for the transition from the current kernel to the
 * new kernel is placed in the control_code_buffer, whose size
 * is given by KEXEC_CONTROL_PAGE_SIZE.  In the best case only a single
 * page of memory is necessary, but some architectures require more.
 * Because this memory must be identity mapped in the transition from
 * virtual to physical addresses it must live in the range
 * 0 - TASK_SIZE, as only the user space mappings are arbitrarily
 * modifiable.
 *
 * The assembly stub in the control code buffer is passed a linked list
 * of descriptor pages detailing the source pages of the new kernel,
 * and the destination addresses of those source pages.  As this data
 * structure is not used in the context of the current OS, it must
 * be self-contained.
 *
 * The code has been made to work with highmem pages and will use a
 * destination page in its final resting place (if it happens
 * to allocate it).  The end product of this is that most of the
 * physical address space, and most of RAM can be used.
 *
 * Future directions include:
 *  - allocating a page table with the control code buffer identity
 *    mapped, to simplify machine_kexec and make kexec_on_panic more
 *    reliable.
 */

/*
 * KIMAGE_NO_DEST is an impossible destination address..., for
 * allocating pages whose destination address we do not care about.
 */
#define KIMAGE_NO_DEST (-1UL)
#define PAGE_COUNT(x) (((x) + PAGE_SIZE - 1) >> PAGE_SHIFT)

static struct page *kimage_alloc_page(struct kimage *image,
				       gfp_t gfp_mask,
				       unsigned long dest);

int sanity_check_segment_list(struct kimage *image)
{
	int i;
	unsigned long nr_segments = image->nr_segments;
	unsigned long total_pages = 0;
	unsigned long nr_pages = totalram_pages();

	/*
	 * Verify we have good destination addresses.  The caller is
	 * responsible for making certain we don't attempt to load
	 * the new image into invalid or reserved areas of RAM.  This
	 * just verifies it is an address we can use.
	 *
	 * Since the kernel does everything in page size chunks ensure
	 * the destination addresses are page aligned.  Too many
	 * special cases crop of when we don't do this.  The most
	 * insidious is getting overlapping destination addresses
	 * simply because addresses are changed to page size
	 * granularity.
	 */
	for (i = 0; i < nr_segments; i++) {
		unsigned long mstart, mend;

		mstart = image->segment[i].mem;
		mend   = mstart + image->segment[i].memsz;
		if (mstart > mend)
			return -EADDRNOTAVAIL;
		if ((mstart & ~PAGE_MASK) || (mend & ~PAGE_MASK))
			return -EADDRNOTAVAIL;
		if (mend >= KEXEC_DESTINATION_MEMORY_LIMIT)
			return -EADDRNOTAVAIL;
	}

	/* Verify our destination addresses do not overlap.
	 * If we alloed overlapping destination addresses
	 * through very weird things can happen with no
	 * easy explanation as one segment stops on another.
	 */
	for (i = 0; i < nr_segments; i++) {
		unsigned long mstart, mend;
		unsigned long j;

		mstart = image->segment[i].mem;
		mend   = mstart + image->segment[i].memsz;
		for (j = 0; j < i; j++) {
			unsigned long pstart, pend;

			pstart = image->segment[j].mem;
			pend   = pstart + image->segment[j].memsz;
			/* Do the segments overlap ? */
			if ((mend > pstart) && (mstart < pend))
				return -EINVAL;
		}
	}

	/* Ensure our buffer sizes are strictly less than
	 * our memory sizes.  This should always be the case,
	 * and it is easier to check up front than to be surprised
	 * later on.
	 */
	for (i = 0; i < nr_segments; i++) {
		if (image->segment[i].bufsz > image->segment[i].memsz)
			return -EINVAL;
	}

	/*
	 * Verify that no more than half of memory will be consumed. If the
	 * request from userspace is too large, a large amount of time will be
	 * wasted allocating pages, which can cause a soft lockup.
	 */
	for (i = 0; i < nr_segments; i++) {
		if (PAGE_COUNT(image->segment[i].memsz) > nr_pages / 2)
			return -EINVAL;

		total_pages += PAGE_COUNT(image->segment[i].memsz);
	}

	if (total_pages > nr_pages / 2)
		return -EINVAL;

	/*
	 * Verify we have good destination addresses.  Normally
	 * the caller is responsible for making certain we don't
	 * attempt to load the new image into invalid or reserved
	 * areas of RAM.  But crash kernels are preloaded into a
	 * reserved area of ram.  We must ensure the addresses
	 * are in the reserved area otherwise preloading the
	 * kernel could corrupt things.
	 */

	if (image->type == KEXEC_TYPE_CRASH) {
		for (i = 0; i < nr_segments; i++) {
			unsigned long mstart, mend;

			mstart = image->segment[i].mem;
			mend = mstart + image->segment[i].memsz - 1;
			/* Ensure we are within the crash kernel limits */
			if ((mstart < phys_to_boot_phys(crashk_res.start)) ||
			    (mend > phys_to_boot_phys(crashk_res.end)))
				return -EADDRNOTAVAIL;
		}
	}

	return 0;
}

struct kimage *do_kimage_alloc_init(void)
{
	struct kimage *image;

	/* Allocate a controlling structure */
	image = kzalloc(sizeof(*image), GFP_KERNEL);
	if (!image)
		return NULL;

	image->head = 0;
	image->entry = &image->head;
	image->last_entry = &image->head;
	image->control_page = ~0; /* By default this does not apply */
	image->type = KEXEC_TYPE_DEFAULT;

	/* Initialize the list of control pages */
	INIT_LIST_HEAD(&image->control_pages);

	/* Initialize the list of destination pages */
	INIT_LIST_HEAD(&image->dest_pages);

	/* Initialize the list of unusable pages */
	INIT_LIST_HEAD(&image->unusable_pages);

	return image;
}

int kimage_is_destination_range(struct kimage *image,
					unsigned long start,
					unsigned long end)
{
	unsigned long i;

	for (i = 0; i < image->nr_segments; i++) {
		unsigned long mstart, mend;

		mstart = image->segment[i].mem;
		mend = mstart + image->segment[i].memsz;
		if ((end > mstart) && (start < mend))
			return 1;
	}

	return 0;
}

static struct page *kimage_alloc_pages(gfp_t gfp_mask, unsigned int order)
{
	struct page *pages;

	if (fatal_signal_pending(current))
		return NULL;
	pages = alloc_pages(gfp_mask & ~__GFP_ZERO, order);
	if (pages) {
		unsigned int count, i;

		pages->mapping = NULL;
		set_page_private(pages, order);
		count = 1 << order;
		for (i = 0; i < count; i++)
			SetPageReserved(pages + i);

		arch_kexec_post_alloc_pages(page_address(pages), count,
					    gfp_mask);

		if (gfp_mask & __GFP_ZERO)
			for (i = 0; i < count; i++)
				clear_highpage(pages + i);
	}

	return pages;
}

static void kimage_free_pages(struct page *page)
{
	unsigned int order, count, i;

	order = page_private(page);
	count = 1 << order;

	arch_kexec_pre_free_pages(page_address(page), count);

	for (i = 0; i < count; i++)
		ClearPageReserved(page + i);
	__free_pages(page, order);
}

void kimage_free_page_list(struct list_head *list)
{
	struct page *page, *next;

	list_for_each_entry_safe(page, next, list, lru) {
		list_del(&page->lru);
		kimage_free_pages(page);
	}
}

static struct page *kimage_alloc_normal_control_pages(struct kimage *image,
							unsigned int order)
{
	/* Control pages are special, they are the intermediaries
	 * that are needed while we copy the rest of the pages
	 * to their final resting place.  As such they must
	 * not conflict with either the destination addresses
	 * or memory the kernel is already using.
	 *
	 * The only case where we really need more than one of
	 * these are for architectures where we cannot disable
	 * the MMU and must instead generate an identity mapped
	 * page table for all of the memory.
	 *
	 * At worst this runs in O(N) of the image size.
	 */
	struct list_head extra_pages;
	struct page *pages;
	unsigned int count;

	count = 1 << order;
	INIT_LIST_HEAD(&extra_pages);

	/* Loop while I can allocate a page and the page allocated
	 * is a destination page.
	 */
	do {
		unsigned long pfn, epfn, addr, eaddr;

		pages = kimage_alloc_pages(KEXEC_CONTROL_MEMORY_GFP, order);
		if (!pages)
			break;
		pfn   = page_to_boot_pfn(pages);
		epfn  = pfn + count;
		addr  = pfn << PAGE_SHIFT;
		eaddr = epfn << PAGE_SHIFT;
		if ((epfn >= (KEXEC_CONTROL_MEMORY_LIMIT >> PAGE_SHIFT)) ||
			      kimage_is_destination_range(image, addr, eaddr)) {
			list_add(&pages->lru, &extra_pages);
			pages = NULL;
		}
	} while (!pages);

	if (pages) {
		/* Remember the allocated page... */
		list_add(&pages->lru, &image->control_pages);

		/* Because the page is already in it's destination
		 * location we will never allocate another page at
		 * that address.  Therefore kimage_alloc_pages
		 * will not return it (again) and we don't need
		 * to give it an entry in image->segment[].
		 */
	}
	/* Deal with the destination pages I have inadvertently allocated.
	 *
	 * Ideally I would convert multi-page allocations into single
	 * page allocations, and add everything to image->dest_pages.
	 *
	 * For now it is simpler to just free the pages.
	 */
	kimage_free_page_list(&extra_pages);

	return pages;
}

static struct page *kimage_alloc_crash_control_pages(struct kimage *image,
						      unsigned int order)
{
	/* Control pages are special, they are the intermediaries
	 * that are needed while we copy the rest of the pages
	 * to their final resting place.  As such they must
	 * not conflict with either the destination addresses
	 * or memory the kernel is already using.
	 *
	 * Control pages are also the only pags we must allocate
	 * when loading a crash kernel.  All of the other pages
	 * are specified by the segments and we just memcpy
	 * into them directly.
	 *
	 * The only case where we really need more than one of
	 * these are for architectures where we cannot disable
	 * the MMU and must instead generate an identity mapped
	 * page table for all of the memory.
	 *
	 * Given the low demand this implements a very simple
	 * allocator that finds the first hole of the appropriate
	 * size in the reserved memory region, and allocates all
	 * of the memory up to and including the hole.
	 */
	unsigned long hole_start, hole_end, size;
	struct page *pages;

	pages = NULL;
	size = (1 << order) << PAGE_SHIFT;
	hole_start = (image->control_page + (size - 1)) & ~(size - 1);
	hole_end   = hole_start + size - 1;
	while (hole_end <= crashk_res.end) {
		unsigned long i;

		cond_resched();

		if (hole_end > KEXEC_CRASH_CONTROL_MEMORY_LIMIT)
			break;
		/* See if I overlap any of the segments */
		for (i = 0; i < image->nr_segments; i++) {
			unsigned long mstart, mend;

			mstart = image->segment[i].mem;
			mend   = mstart + image->segment[i].memsz - 1;
			if ((hole_end >= mstart) && (hole_start <= mend)) {
				/* Advance the hole to the end of the segment */
				hole_start = (mend + (size - 1)) & ~(size - 1);
				hole_end   = hole_start + size - 1;
				break;
			}
		}
		/* If I don't overlap any segments I have found my hole! */
		if (i == image->nr_segments) {
			pages = pfn_to_page(hole_start >> PAGE_SHIFT);
			image->control_page = hole_end;
			break;
		}
	}

	/* Ensure that these pages are decrypted if SME is enabled. */
	if (pages)
		arch_kexec_post_alloc_pages(page_address(pages), 1 << order, 0);

	return pages;
}


struct page *kimage_alloc_control_pages(struct kimage *image,
					 unsigned int order)
{
	struct page *pages = NULL;

	switch (image->type) {
	case KEXEC_TYPE_DEFAULT:
		pages = kimage_alloc_normal_control_pages(image, order);
		break;
	case KEXEC_TYPE_CRASH:
		pages = kimage_alloc_crash_control_pages(image, order);
		break;
	}

	return pages;
}

int kimage_crash_copy_vmcoreinfo(struct kimage *image)
{
	struct page *vmcoreinfo_page;
	void *safecopy;

	if (image->type != KEXEC_TYPE_CRASH)
		return 0;

	/*
	 * For kdump, allocate one vmcoreinfo safe copy from the
	 * crash memory. as we have arch_kexec_protect_crashkres()
	 * after kexec syscall, we naturally protect it from write
	 * (even read) access under kernel direct mapping. But on
	 * the other hand, we still need to operate it when crash
	 * happens to generate vmcoreinfo note, hereby we rely on
	 * vmap for this purpose.
	 */
	vmcoreinfo_page = kimage_alloc_control_pages(image, 0);
	if (!vmcoreinfo_page) {
		pr_warn("Could not allocate vmcoreinfo buffer\n");
		return -ENOMEM;
	}
	safecopy = vmap(&vmcoreinfo_page, 1, VM_MAP, PAGE_KERNEL);
	if (!safecopy) {
		pr_warn("Could not vmap vmcoreinfo buffer\n");
		return -ENOMEM;
	}

	image->vmcoreinfo_data_copy = safecopy;
	crash_update_vmcoreinfo_safecopy(safecopy);

	return 0;
}

static int kimage_add_entry(struct kimage *image, kimage_entry_t entry)
{
	if (*image->entry != 0)
		image->entry++;

	if (image->entry == image->last_entry) {
		kimage_entry_t *ind_page;
		struct page *page;

		page = kimage_alloc_page(image, GFP_KERNEL, KIMAGE_NO_DEST);
		if (!page)
			return -ENOMEM;

		ind_page = page_address(page);
		*image->entry = virt_to_boot_phys(ind_page) | IND_INDIRECTION;
		image->entry = ind_page;
		image->last_entry = ind_page +
				      ((PAGE_SIZE/sizeof(kimage_entry_t)) - 1);
	}
	*image->entry = entry;
	image->entry++;
	*image->entry = 0;

	return 0;
}

static int kimage_set_destination(struct kimage *image,
				   unsigned long destination)
{
	int result;

	destination &= PAGE_MASK;
	result = kimage_add_entry(image, destination | IND_DESTINATION);

	return result;
}


static int kimage_add_page(struct kimage *image, unsigned long page)
{
	int result;

	page &= PAGE_MASK;
	result = kimage_add_entry(image, page | IND_SOURCE);

	return result;
}


static void kimage_free_extra_pages(struct kimage *image)
{
	/* Walk through and free any extra destination pages I may have */
	kimage_free_page_list(&image->dest_pages);

	/* Walk through and free any unusable pages I have cached */
	kimage_free_page_list(&image->unusable_pages);

}

int __weak machine_kexec_post_load(struct kimage *image)
{
	return 0;
}

void kimage_terminate(struct kimage *image)
{
	if (*image->entry != 0)
		image->entry++;

	*image->entry = IND_DONE;
}

#define for_each_kimage_entry(image, ptr, entry) \
	for (ptr = &image->head; (entry = *ptr) && !(entry & IND_DONE); \
		ptr = (entry & IND_INDIRECTION) ? \
			boot_phys_to_virt((entry & PAGE_MASK)) : ptr + 1)

static void kimage_free_entry(kimage_entry_t entry)
{
	struct page *page;

	page = boot_pfn_to_page(entry >> PAGE_SHIFT);
	kimage_free_pages(page);
}

void kimage_free(struct kimage *image)
{
	kimage_entry_t *ptr, entry;
	kimage_entry_t ind = 0;

	if (!image)
		return;

	if (image->vmcoreinfo_data_copy) {
		crash_update_vmcoreinfo_safecopy(NULL);
		vunmap(image->vmcoreinfo_data_copy);
	}

	kimage_free_extra_pages(image);
	for_each_kimage_entry(image, ptr, entry) {
		if (entry & IND_INDIRECTION) {
			/* Free the previous indirection page */
			if (ind & IND_INDIRECTION)
				kimage_free_entry(ind);
			/* Save this indirection page until we are
			 * done with it.
			 */
			ind = entry;
		} else if (entry & IND_SOURCE)
			kimage_free_entry(entry);
	}
	/* Free the final indirection page */
	if (ind & IND_INDIRECTION)
		kimage_free_entry(ind);

	/* Handle any machine specific cleanup */
	machine_kexec_cleanup(image);

	/* Free the kexec control pages... */
	kimage_free_page_list(&image->control_pages);

	/*
	 * Free up any temporary buffers allocated. This might hit if
	 * error occurred much later after buffer allocation.
	 */
	if (image->file_mode)
		kimage_file_post_load_cleanup(image);

	kfree(image);
}

static kimage_entry_t *kimage_dst_used(struct kimage *image,
					unsigned long page)
{
	kimage_entry_t *ptr, entry;
	unsigned long destination = 0;

	for_each_kimage_entry(image, ptr, entry) {
		if (entry & IND_DESTINATION)
			destination = entry & PAGE_MASK;
		else if (entry & IND_SOURCE) {
			if (page == destination)
				return ptr;
			destination += PAGE_SIZE;
		}
	}

	return NULL;
}

static struct page *kimage_alloc_page(struct kimage *image,
					gfp_t gfp_mask,
					unsigned long destination)
{
	/*
	 * Here we implement safeguards to ensure that a source page
	 * is not copied to its destination page before the data on
	 * the destination page is no longer useful.
	 *
	 * To do this we maintain the invariant that a source page is
	 * either its own destination page, or it is not a
	 * destination page at all.
	 *
	 * That is slightly stronger than required, but the proof
	 * that no problems will not occur is trivial, and the
	 * implementation is simply to verify.
	 *
	 * When allocating all pages normally this algorithm will run
	 * in O(N) time, but in the worst case it will run in O(N^2)
	 * time.   If the runtime is a problem the data structures can
	 * be fixed.
	 */
	struct page *page;
	unsigned long addr;

	/*
	 * Walk through the list of destination pages, and see if I
	 * have a match.
	 */
	list_for_each_entry(page, &image->dest_pages, lru) {
		addr = page_to_boot_pfn(page) << PAGE_SHIFT;
		if (addr == destination) {
			list_del(&page->lru);
			return page;
		}
	}
	page = NULL;
	while (1) {
		kimage_entry_t *old;

		/* Allocate a page, if we run out of memory give up */
		page = kimage_alloc_pages(gfp_mask, 0);
		if (!page)
			return NULL;
		/* If the page cannot be used file it away */
		if (page_to_boot_pfn(page) >
				(KEXEC_SOURCE_MEMORY_LIMIT >> PAGE_SHIFT)) {
			list_add(&page->lru, &image->unusable_pages);
			continue;
		}
		addr = page_to_boot_pfn(page) << PAGE_SHIFT;

		/* If it is the destination page we want use it */
		if (addr == destination)
			break;

		/* If the page is not a destination page use it */
		if (!kimage_is_destination_range(image, addr,
						  addr + PAGE_SIZE))
			break;

		/*
		 * I know that the page is someones destination page.
		 * See if there is already a source page for this
		 * destination page.  And if so swap the source pages.
		 */
		old = kimage_dst_used(image, addr);
		if (old) {
			/* If so move it */
			unsigned long old_addr;
			struct page *old_page;

			old_addr = *old & PAGE_MASK;
			old_page = boot_pfn_to_page(old_addr >> PAGE_SHIFT);
			copy_highpage(page, old_page);
			*old = addr | (*old & ~PAGE_MASK);

			/* The old page I have found cannot be a
			 * destination page, so return it if it's
			 * gfp_flags honor the ones passed in.
			 */
			if (!(gfp_mask & __GFP_HIGHMEM) &&
			    PageHighMem(old_page)) {
				kimage_free_pages(old_page);
				continue;
			}
			addr = old_addr;
			page = old_page;
			break;
		}
		/* Place the page on the destination list, to be used later */
		list_add(&page->lru, &image->dest_pages);
	}

	return page;
}

static int kimage_load_normal_segment(struct kimage *image,
					 struct kexec_segment *segment)
{
	unsigned long maddr;
	size_t ubytes, mbytes;
	int result;
	unsigned char __user *buf = NULL;
	unsigned char *kbuf = NULL;

	result = 0;
	if (image->file_mode)
		kbuf = segment->kbuf;
	else
		buf = segment->buf;
	ubytes = segment->bufsz;
	mbytes = segment->memsz;
	maddr = segment->mem;

	result = kimage_set_destination(image, maddr);
	if (result < 0)
		goto out;

	while (mbytes) {
		struct page *page;
		char *ptr;
		size_t uchunk, mchunk;

		page = kimage_alloc_page(image, GFP_HIGHUSER, maddr);
		if (!page) {
			result  = -ENOMEM;
			goto out;
		}
		result = kimage_add_page(image, page_to_boot_pfn(page)
								<< PAGE_SHIFT);
		if (result < 0)
			goto out;

		ptr = kmap(page);
		/* Start with a clear page */
		clear_page(ptr);
		ptr += maddr & ~PAGE_MASK;
		mchunk = min_t(size_t, mbytes,
				PAGE_SIZE - (maddr & ~PAGE_MASK));
		uchunk = min(ubytes, mchunk);

		/* For file based kexec, source pages are in kernel memory */
		if (image->file_mode)
			memcpy(ptr, kbuf, uchunk);
		else
			result = copy_from_user(ptr, buf, uchunk);
		kunmap(page);
		if (result) {
			result = -EFAULT;
			goto out;
		}
		ubytes -= uchunk;
		maddr  += mchunk;
		if (image->file_mode)
			kbuf += mchunk;
		else
			buf += mchunk;
		mbytes -= mchunk;

		cond_resched();
	}
out:
	return result;
}

static int kimage_load_crash_segment(struct kimage *image,
					struct kexec_segment *segment)
{
	/* For crash dumps kernels we simply copy the data from
	 * user space to it's destination.
	 * We do things a page at a time for the sake of kmap.
	 */
	unsigned long maddr;
	size_t ubytes, mbytes;
	int result;
	unsigned char __user *buf = NULL;
	unsigned char *kbuf = NULL;

	result = 0;
	if (image->file_mode)
		kbuf = segment->kbuf;
	else
		buf = segment->buf;
	ubytes = segment->bufsz;
	mbytes = segment->memsz;
	maddr = segment->mem;
	while (mbytes) {
		struct page *page;
		char *ptr;
		size_t uchunk, mchunk;

		page = boot_pfn_to_page(maddr >> PAGE_SHIFT);
		if (!page) {
			result  = -ENOMEM;
			goto out;
		}
		arch_kexec_post_alloc_pages(page_address(page), 1, 0);
		ptr = kmap(page);
		ptr += maddr & ~PAGE_MASK;
		mchunk = min_t(size_t, mbytes,
				PAGE_SIZE - (maddr & ~PAGE_MASK));
		uchunk = min(ubytes, mchunk);
		if (mchunk > uchunk) {
			/* Zero the trailing part of the page */
			memset(ptr + uchunk, 0, mchunk - uchunk);
		}

		/* For file based kexec, source pages are in kernel memory */
		if (image->file_mode)
			memcpy(ptr, kbuf, uchunk);
		else
			result = copy_from_user(ptr, buf, uchunk);
		kexec_flush_icache_page(page);
		kunmap(page);
		arch_kexec_pre_free_pages(page_address(page), 1);
		if (result) {
			result = -EFAULT;
			goto out;
		}
		ubytes -= uchunk;
		maddr  += mchunk;
		if (image->file_mode)
			kbuf += mchunk;
		else
			buf += mchunk;
		mbytes -= mchunk;

		cond_resched();
	}
out:
	return result;
}

int kimage_load_segment(struct kimage *image,
				struct kexec_segment *segment)
{
	int result = -ENOMEM;

	switch (image->type) {
	case KEXEC_TYPE_DEFAULT:
		result = kimage_load_normal_segment(image, segment);
		break;
	case KEXEC_TYPE_CRASH:
		result = kimage_load_crash_segment(image, segment);
		break;
	}

	return result;
}

struct kimage *kexec_image;
struct kimage *kexec_crash_image;
int kexec_load_disabled;

/*
 * No panic_cpu check version of crash_kexec().  This function is called
 * only when panic_cpu holds the current CPU number; this is the only CPU
 * which processes crash_kexec routines.
 */
void __noclone __crash_kexec(struct pt_regs *regs)
{
	/* Take the kexec_mutex here to prevent sys_kexec_load
	 * running on one cpu from replacing the crash kernel
	 * we are using after a panic on a different cpu.
	 *
	 * If the crash kernel was not located in a fixed area
	 * of memory the xchg(&kexec_crash_image) would be
	 * sufficient.  But since I reuse the memory...
	 */
	if (mutex_trylock(&kexec_mutex)) {
		if (kexec_crash_image) {
			struct pt_regs fixed_regs;

			crash_setup_regs(&fixed_regs, regs);
			crash_save_vmcoreinfo();
			machine_crash_shutdown(&fixed_regs);
			machine_kexec(kexec_crash_image);
		}
		mutex_unlock(&kexec_mutex);
	}
}
STACK_FRAME_NON_STANDARD(__crash_kexec);

void crash_kexec(struct pt_regs *regs)
{
	int old_cpu, this_cpu;

	/*
	 * Only one CPU is allowed to execute the crash_kexec() code as with
	 * panic().  Otherwise parallel calls of panic() and crash_kexec()
	 * may stop each other.  To exclude them, we use panic_cpu here too.
	 */
	this_cpu = raw_smp_processor_id();
	old_cpu = atomic_cmpxchg(&panic_cpu, PANIC_CPU_INVALID, this_cpu);
	if (old_cpu == PANIC_CPU_INVALID) {
		/* This is the 1st CPU which comes here, so go ahead. */
		printk_safe_flush_on_panic();
		__crash_kexec(regs);

		/*
		 * Reset panic_cpu to allow another panic()/crash_kexec()
		 * call.
		 */
		atomic_set(&panic_cpu, PANIC_CPU_INVALID);
	}
}

size_t crash_get_memory_size(void)
{
	size_t size = 0;

	mutex_lock(&kexec_mutex);
	if (crashk_res.end != crashk_res.start)
		size = resource_size(&crashk_res);
	mutex_unlock(&kexec_mutex);
	return size;
}

void __weak crash_free_reserved_phys_range(unsigned long begin,
					   unsigned long end)
{
	unsigned long addr;

	for (addr = begin; addr < end; addr += PAGE_SIZE)
		free_reserved_page(boot_pfn_to_page(addr >> PAGE_SHIFT));
}

int crash_shrink_memory(unsigned long new_size)
{
	int ret = 0;
	unsigned long start, end;
	unsigned long old_size;
	struct resource *ram_res;

	mutex_lock(&kexec_mutex);

	if (kexec_crash_image) {
		ret = -ENOENT;
		goto unlock;
	}
	start = crashk_res.start;
	end = crashk_res.end;
	old_size = (end == 0) ? 0 : end - start + 1;
	if (new_size >= old_size) {
		ret = (new_size == old_size) ? 0 : -EINVAL;
		goto unlock;
	}

	ram_res = kzalloc(sizeof(*ram_res), GFP_KERNEL);
	if (!ram_res) {
		ret = -ENOMEM;
		goto unlock;
	}

	start = roundup(start, KEXEC_CRASH_MEM_ALIGN);
	end = roundup(start + new_size, KEXEC_CRASH_MEM_ALIGN);

	crash_free_reserved_phys_range(end, crashk_res.end);

	if ((start == end) && (crashk_res.parent != NULL))
		release_resource(&crashk_res);

	ram_res->start = end;
	ram_res->end = crashk_res.end;
	ram_res->flags = IORESOURCE_BUSY | IORESOURCE_SYSTEM_RAM;
	ram_res->name = "System RAM";

	crashk_res.end = end - 1;

	insert_resource(&iomem_resource, ram_res);

unlock:
	mutex_unlock(&kexec_mutex);
	return ret;
}

void crash_save_cpu(struct pt_regs *regs, int cpu)
{
	struct elf_prstatus prstatus;
	u32 *buf;

	if ((cpu < 0) || (cpu >= nr_cpu_ids))
		return;

	/* Using ELF notes here is opportunistic.
	 * I need a well defined structure format
	 * for the data I pass, and I need tags
	 * on the data to indicate what information I have
	 * squirrelled away.  ELF notes happen to provide
	 * all of that, so there is no need to invent something new.
	 */
	buf = (u32 *)per_cpu_ptr(crash_notes, cpu);
	if (!buf)
		return;
	memset(&prstatus, 0, sizeof(prstatus));
	prstatus.common.pr_pid = current->pid;
	elf_core_copy_kernel_regs(&prstatus.pr_reg, regs);
	buf = append_elf_note(buf, KEXEC_CORE_NOTE_NAME, NT_PRSTATUS,
			      &prstatus, sizeof(prstatus));
	final_note(buf);
}

static int __init crash_notes_memory_init(void)
{
	/* Allocate memory for saving cpu registers. */
	size_t size, align;

	/*
	 * crash_notes could be allocated across 2 vmalloc pages when percpu
	 * is vmalloc based . vmalloc doesn't guarantee 2 continuous vmalloc
	 * pages are also on 2 continuous physical pages. In this case the
	 * 2nd part of crash_notes in 2nd page could be lost since only the
	 * starting address and size of crash_notes are exported through sysfs.
	 * Here round up the size of crash_notes to the nearest power of two
	 * and pass it to __alloc_percpu as align value. This can make sure
	 * crash_notes is allocated inside one physical page.
	 */
	size = sizeof(note_buf_t);
	align = min(roundup_pow_of_two(sizeof(note_buf_t)), PAGE_SIZE);

	/*
	 * Break compile if size is bigger than PAGE_SIZE since crash_notes
	 * definitely will be in 2 pages with that.
	 */
	BUILD_BUG_ON(size > PAGE_SIZE);

	crash_notes = __alloc_percpu(size, align);
	if (!crash_notes) {
		pr_warn("Memory allocation for saving cpu register states failed\n");
		return -ENOMEM;
	}
	return 0;
}
subsys_initcall(crash_notes_memory_init);


/*
 * Move into place and start executing a preloaded standalone
 * executable.  If nothing was preloaded return an error.
 */
int kernel_kexec(void)
{
	int error = 0;

	if (!mutex_trylock(&kexec_mutex))
		return -EBUSY;
	if (!kexec_image) {
		error = -EINVAL;
		goto Unlock;
	}

#ifdef CONFIG_KEXEC_JUMP
	if (kexec_image->preserve_context) {
		pm_prepare_console();
		error = freeze_processes();
		if (error) {
			error = -EBUSY;
			goto Restore_console;
		}
		suspend_console();
		error = dpm_suspend_start(PMSG_FREEZE);
		if (error)
			goto Resume_console;
		/* At this point, dpm_suspend_start() has been called,
		 * but *not* dpm_suspend_end(). We *must* call
		 * dpm_suspend_end() now.  Otherwise, drivers for
		 * some devices (e.g. interrupt controllers) become
		 * desynchronized with the actual state of the
		 * hardware at resume time, and evil weirdness ensues.
		 */
		error = dpm_suspend_end(PMSG_FREEZE);
		if (error)
			goto Resume_devices;
		error = suspend_disable_secondary_cpus();
		if (error)
			goto Enable_cpus;
		local_irq_disable();
		error = syscore_suspend();
		if (error)
			goto Enable_irqs;
	} else
#endif
	{
		kexec_in_progress = true;
		kernel_restart_prepare("kexec reboot");
		migrate_to_reboot_cpu();

		/*
		 * migrate_to_reboot_cpu() disables CPU hotplug assuming that
		 * no further code needs to use CPU hotplug (which is true in
		 * the reboot case). However, the kexec path depends on using
		 * CPU hotplug again; so re-enable it here.
		 */
		cpu_hotplug_enable();
		pr_notice("Starting new kernel\n");
		machine_shutdown();
	}

	kmsg_dump(KMSG_DUMP_SHUTDOWN);
	machine_kexec(kexec_image);

#ifdef CONFIG_KEXEC_JUMP
	if (kexec_image->preserve_context) {
		syscore_resume();
 Enable_irqs:
		local_irq_enable();
 Enable_cpus:
		suspend_enable_secondary_cpus();
		dpm_resume_start(PMSG_RESTORE);
 Resume_devices:
		dpm_resume_end(PMSG_RESTORE);
 Resume_console:
		resume_console();
		thaw_processes();
 Restore_console:
		pm_restore_console();
	}
#endif

 Unlock:
	mutex_unlock(&kexec_mutex);
	return error;
}

/*
 * Protection mechanism for crashkernel reserved memory after
 * the kdump kernel is loaded.
 *
 * Provide an empty default implementation here -- architecture
 * code may override this
 */
void __weak arch_kexec_protect_crashkres(void)
{}

void __weak arch_kexec_unprotect_crashkres(void)
{}
