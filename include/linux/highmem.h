/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_HIGHMEM_H
#define _LINUX_HIGHMEM_H

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/hardirq.h>

#include <asm/cacheflush.h>

#include "highmem-internal.h"

/**
 * kmap - Map a page for long term usage
 * @page:	Pointer to the page to be mapped
 *
 * Returns: The virtual address of the mapping
 *
 * Can only be invoked from preemptible task context because on 32bit
 * systems with CONFIG_HIGHMEM enabled this function might sleep.
 *
 * For systems with CONFIG_HIGHMEM=n and for pages in the low memory area
 * this returns the virtual address of the direct kernel mapping.
 *
 * The returned virtual address is globally visible and valid up to the
 * point where it is unmapped via kunmap(). The pointer can be handed to
 * other contexts.
 *
 * For highmem pages on 32bit systems this can be slow as the mapping space
 * is limited and protected by a global lock. In case that there is no
 * mapping slot available the function blocks until a slot is released via
 * kunmap().
 */
static inline void *kmap(struct page *page);

/**
 * kunmap - Unmap the virtual address mapped by kmap()
 * @addr:	Virtual address to be unmapped
 *
 * Counterpart to kmap(). A NOOP for CONFIG_HIGHMEM=n and for mappings of
 * pages in the low memory area.
 */
static inline void kunmap(struct page *page);

/**
 * kmap_to_page - Get the page for a kmap'ed address
 * @addr:	The address to look up
 *
 * Returns: The page which is mapped to @addr.
 */
static inline struct page *kmap_to_page(void *addr);

/**
 * kmap_flush_unused - Flush all unused kmap mappings in order to
 *		       remove stray mappings
 */
static inline void kmap_flush_unused(void);

/**
 * kmap_local_page - Map a page for temporary usage
 * @page:	Pointer to the page to be mapped
 *
 * Returns: The virtual address of the mapping
 *
 * Can be invoked from any context.
 *
 * Requires careful handling when nesting multiple mappings because the map
 * management is stack based. The unmap has to be in the reverse order of
 * the map operation:
 *
 * addr1 = kmap_local_page(page1);
 * addr2 = kmap_local_page(page2);
 * ...
 * kunmap_local(addr2);
 * kunmap_local(addr1);
 *
 * Unmapping addr1 before addr2 is invalid and causes malfunction.
 *
 * Contrary to kmap() mappings the mapping is only valid in the context of
 * the caller and cannot be handed to other contexts.
 *
 * On CONFIG_HIGHMEM=n kernels and for low memory pages this returns the
 * virtual address of the direct mapping. Only real highmem pages are
 * temporarily mapped.
 *
 * While it is significantly faster than kmap() for the higmem case it
 * comes with restrictions about the pointer validity. Only use when really
 * necessary.
 *
 * On HIGHMEM enabled systems mapping a highmem page has the side effect of
 * disabling migration in order to keep the virtual address stable across
 * preemption. No caller of kmap_local_page() can rely on this side effect.
 */
static inline void *kmap_local_page(struct page *page);

/**
 * kmap_atomic - Atomically map a page for temporary usage - Deprecated!
 * @page:	Pointer to the page to be mapped
 *
 * Returns: The virtual address of the mapping
 *
 * Effectively a wrapper around kmap_local_page() which disables pagefaults
 * and preemption.
 *
 * Do not use in new code. Use kmap_local_page() instead.
 */
static inline void *kmap_atomic(struct page *page);

/**
 * kunmap_atomic - Unmap the virtual address mapped by kmap_atomic()
 * @addr:	Virtual address to be unmapped
 *
 * Counterpart to kmap_atomic().
 *
 * Effectively a wrapper around kunmap_local() which additionally undoes
 * the side effects of kmap_atomic(), i.e. reenabling pagefaults and
 * preemption.
 */

/* Highmem related interfaces for management code */
static inline unsigned int nr_free_highpages(void);
static inline unsigned long totalhigh_pages(void);

#ifndef ARCH_HAS_FLUSH_ANON_PAGE
static inline void flush_anon_page(struct vm_area_struct *vma, struct page *page, unsigned long vmaddr)
{
}
#endif

#ifndef ARCH_HAS_FLUSH_KERNEL_DCACHE_PAGE
static inline void flush_kernel_dcache_page(struct page *page)
{
}
static inline void flush_kernel_vmap_range(void *vaddr, int size)
{
}
static inline void invalidate_kernel_vmap_range(void *vaddr, int size)
{
}
#endif

/* when CONFIG_HIGHMEM is not set these will be plain clear/copy_page */
#ifndef clear_user_highpage
static inline void clear_user_highpage(struct page *page, unsigned long vaddr)
{
	void *addr = kmap_atomic(page);
	clear_user_page(addr, vaddr, page);
	kunmap_atomic(addr);
}
#endif

#ifndef __HAVE_ARCH_ALLOC_ZEROED_USER_HIGHPAGE
/**
 * __alloc_zeroed_user_highpage - Allocate a zeroed HIGHMEM page for a VMA with caller-specified movable GFP flags
 * @movableflags: The GFP flags related to the pages future ability to move like __GFP_MOVABLE
 * @vma: The VMA the page is to be allocated for
 * @vaddr: The virtual address the page will be inserted into
 *
 * This function will allocate a page for a VMA but the caller is expected
 * to specify via movableflags whether the page will be movable in the
 * future or not
 *
 * An architecture may override this function by defining
 * __HAVE_ARCH_ALLOC_ZEROED_USER_HIGHPAGE and providing their own
 * implementation.
 */
static inline struct page *
__alloc_zeroed_user_highpage(gfp_t movableflags,
			struct vm_area_struct *vma,
			unsigned long vaddr)
{
	struct page *page = alloc_page_vma(GFP_HIGHUSER | movableflags,
			vma, vaddr);

	if (page)
		clear_user_highpage(page, vaddr);

	return page;
}
#endif

/**
 * alloc_zeroed_user_highpage_movable - Allocate a zeroed HIGHMEM page for a VMA that the caller knows can move
 * @vma: The VMA the page is to be allocated for
 * @vaddr: The virtual address the page will be inserted into
 *
 * This function will allocate a page for a VMA that the caller knows will
 * be able to migrate in the future using move_pages() or reclaimed
 */
static inline struct page *
alloc_zeroed_user_highpage_movable(struct vm_area_struct *vma,
					unsigned long vaddr)
{
	return __alloc_zeroed_user_highpage(__GFP_MOVABLE, vma, vaddr);
}

static inline void clear_highpage(struct page *page)
{
	void *kaddr = kmap_atomic(page);
	clear_page(kaddr);
	kunmap_atomic(kaddr);
}

/*
 * If we pass in a base or tail page, we can zero up to PAGE_SIZE.
 * If we pass in a head page, we can zero up to the size of the compound page.
 */
#if defined(CONFIG_HIGHMEM) && defined(CONFIG_TRANSPARENT_HUGEPAGE)
void zero_user_segments(struct page *page, unsigned start1, unsigned end1,
		unsigned start2, unsigned end2);
#else /* !HIGHMEM || !TRANSPARENT_HUGEPAGE */
static inline void zero_user_segments(struct page *page,
		unsigned start1, unsigned end1,
		unsigned start2, unsigned end2)
{
	void *kaddr = kmap_atomic(page);
	unsigned int i;

	BUG_ON(end1 > page_size(page) || end2 > page_size(page));

	if (end1 > start1)
		memset(kaddr + start1, 0, end1 - start1);

	if (end2 > start2)
		memset(kaddr + start2, 0, end2 - start2);

	kunmap_atomic(kaddr);
	for (i = 0; i < compound_nr(page); i++)
		flush_dcache_page(page + i);
}
#endif /* !HIGHMEM || !TRANSPARENT_HUGEPAGE */

static inline void zero_user_segment(struct page *page,
	unsigned start, unsigned end)
{
	zero_user_segments(page, start, end, 0, 0);
}

static inline void zero_user(struct page *page,
	unsigned start, unsigned size)
{
	zero_user_segments(page, start, start + size, 0, 0);
}

#ifndef __HAVE_ARCH_COPY_USER_HIGHPAGE

static inline void copy_user_highpage(struct page *to, struct page *from,
	unsigned long vaddr, struct vm_area_struct *vma)
{
	char *vfrom, *vto;

	vfrom = kmap_atomic(from);
	vto = kmap_atomic(to);
	copy_user_page(vto, vfrom, vaddr, to);
	kunmap_atomic(vto);
	kunmap_atomic(vfrom);
}

#endif

#ifndef __HAVE_ARCH_COPY_HIGHPAGE

static inline void copy_highpage(struct page *to, struct page *from)
{
	char *vfrom, *vto;

	vfrom = kmap_atomic(from);
	vto = kmap_atomic(to);
	copy_page(vto, vfrom);
	kunmap_atomic(vto);
	kunmap_atomic(vfrom);
}

#endif

static inline void memcpy_page(struct page *dst_page, size_t dst_off,
			       struct page *src_page, size_t src_off,
			       size_t len)
{
	char *dst = kmap_local_page(dst_page);
	char *src = kmap_local_page(src_page);

	VM_BUG_ON(dst_off + len > PAGE_SIZE || src_off + len > PAGE_SIZE);
	memcpy(dst + dst_off, src + src_off, len);
	kunmap_local(src);
	kunmap_local(dst);
}

static inline void memmove_page(struct page *dst_page, size_t dst_off,
			       struct page *src_page, size_t src_off,
			       size_t len)
{
	char *dst = kmap_local_page(dst_page);
	char *src = kmap_local_page(src_page);

	VM_BUG_ON(dst_off + len > PAGE_SIZE || src_off + len > PAGE_SIZE);
	memmove(dst + dst_off, src + src_off, len);
	kunmap_local(src);
	kunmap_local(dst);
}

static inline void memset_page(struct page *page, size_t offset, int val,
			       size_t len)
{
	char *addr = kmap_local_page(page);

	VM_BUG_ON(offset + len > PAGE_SIZE);
	memset(addr + offset, val, len);
	kunmap_local(addr);
}

static inline void memcpy_from_page(char *to, struct page *page,
				    size_t offset, size_t len)
{
	char *from = kmap_local_page(page);

	VM_BUG_ON(offset + len > PAGE_SIZE);
	memcpy(to, from + offset, len);
	kunmap_local(from);
}

static inline void memcpy_to_page(struct page *page, size_t offset,
				  const char *from, size_t len)
{
	char *to = kmap_local_page(page);

	VM_BUG_ON(offset + len > PAGE_SIZE);
	memcpy(to + offset, from, len);
	kunmap_local(to);
}

static inline void memzero_page(struct page *page, size_t offset, size_t len)
{
	char *addr = kmap_atomic(page);
	memset(addr + offset, 0, len);
	kunmap_atomic(addr);
}

#endif /* _LINUX_HIGHMEM_H */
