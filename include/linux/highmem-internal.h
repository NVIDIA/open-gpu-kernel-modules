/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_HIGHMEM_INTERNAL_H
#define _LINUX_HIGHMEM_INTERNAL_H

/*
 * Outside of CONFIG_HIGHMEM to support X86 32bit iomap_atomic() cruft.
 */
#ifdef CONFIG_KMAP_LOCAL
void *__kmap_local_pfn_prot(unsigned long pfn, pgprot_t prot);
void *__kmap_local_page_prot(struct page *page, pgprot_t prot);
void kunmap_local_indexed(void *vaddr);
void kmap_local_fork(struct task_struct *tsk);
void __kmap_local_sched_out(void);
void __kmap_local_sched_in(void);
static inline void kmap_assert_nomap(void)
{
	DEBUG_LOCKS_WARN_ON(current->kmap_ctrl.idx);
}
#else
static inline void kmap_local_fork(struct task_struct *tsk) { }
static inline void kmap_assert_nomap(void) { }
#endif

#ifdef CONFIG_HIGHMEM
#include <asm/highmem.h>

#ifndef ARCH_HAS_KMAP_FLUSH_TLB
static inline void kmap_flush_tlb(unsigned long addr) { }
#endif

#ifndef kmap_prot
#define kmap_prot PAGE_KERNEL
#endif

void *kmap_high(struct page *page);
void kunmap_high(struct page *page);
void __kmap_flush_unused(void);
struct page *__kmap_to_page(void *addr);

static inline void *kmap(struct page *page)
{
	void *addr;

	might_sleep();
	if (!PageHighMem(page))
		addr = page_address(page);
	else
		addr = kmap_high(page);
	kmap_flush_tlb((unsigned long)addr);
	return addr;
}

static inline void kunmap(struct page *page)
{
	might_sleep();
	if (!PageHighMem(page))
		return;
	kunmap_high(page);
}

static inline struct page *kmap_to_page(void *addr)
{
	return __kmap_to_page(addr);
}

static inline void kmap_flush_unused(void)
{
	__kmap_flush_unused();
}

static inline void *kmap_local_page(struct page *page)
{
	return __kmap_local_page_prot(page, kmap_prot);
}

static inline void *kmap_local_page_prot(struct page *page, pgprot_t prot)
{
	return __kmap_local_page_prot(page, prot);
}

static inline void *kmap_local_pfn(unsigned long pfn)
{
	return __kmap_local_pfn_prot(pfn, kmap_prot);
}

static inline void __kunmap_local(void *vaddr)
{
	kunmap_local_indexed(vaddr);
}

static inline void *kmap_atomic_prot(struct page *page, pgprot_t prot)
{
	preempt_disable();
	pagefault_disable();
	return __kmap_local_page_prot(page, prot);
}

static inline void *kmap_atomic(struct page *page)
{
	return kmap_atomic_prot(page, kmap_prot);
}

static inline void *kmap_atomic_pfn(unsigned long pfn)
{
	preempt_disable();
	pagefault_disable();
	return __kmap_local_pfn_prot(pfn, kmap_prot);
}

static inline void __kunmap_atomic(void *addr)
{
	kunmap_local_indexed(addr);
	pagefault_enable();
	preempt_enable();
}

unsigned int __nr_free_highpages(void);
extern atomic_long_t _totalhigh_pages;

static inline unsigned int nr_free_highpages(void)
{
	return __nr_free_highpages();
}

static inline unsigned long totalhigh_pages(void)
{
	return (unsigned long)atomic_long_read(&_totalhigh_pages);
}

static inline void totalhigh_pages_add(long count)
{
	atomic_long_add(count, &_totalhigh_pages);
}

#else /* CONFIG_HIGHMEM */

static inline struct page *kmap_to_page(void *addr)
{
	return virt_to_page(addr);
}

static inline void *kmap(struct page *page)
{
	might_sleep();
	return page_address(page);
}

static inline void kunmap_high(struct page *page) { }
static inline void kmap_flush_unused(void) { }

static inline void kunmap(struct page *page)
{
#ifdef ARCH_HAS_FLUSH_ON_KUNMAP
	kunmap_flush_on_unmap(page_address(page));
#endif
}

static inline void *kmap_local_page(struct page *page)
{
	return page_address(page);
}

static inline void *kmap_local_page_prot(struct page *page, pgprot_t prot)
{
	return kmap_local_page(page);
}

static inline void *kmap_local_pfn(unsigned long pfn)
{
	return kmap_local_page(pfn_to_page(pfn));
}

static inline void __kunmap_local(void *addr)
{
#ifdef ARCH_HAS_FLUSH_ON_KUNMAP
	kunmap_flush_on_unmap(addr);
#endif
}

static inline void *kmap_atomic(struct page *page)
{
	preempt_disable();
	pagefault_disable();
	return page_address(page);
}

static inline void *kmap_atomic_prot(struct page *page, pgprot_t prot)
{
	return kmap_atomic(page);
}

static inline void *kmap_atomic_pfn(unsigned long pfn)
{
	return kmap_atomic(pfn_to_page(pfn));
}

static inline void __kunmap_atomic(void *addr)
{
#ifdef ARCH_HAS_FLUSH_ON_KUNMAP
	kunmap_flush_on_unmap(addr);
#endif
	pagefault_enable();
	preempt_enable();
}

static inline unsigned int nr_free_highpages(void) { return 0; }
static inline unsigned long totalhigh_pages(void) { return 0UL; }

#endif /* CONFIG_HIGHMEM */

/*
 * Prevent people trying to call kunmap_atomic() as if it were kunmap()
 * kunmap_atomic() should get the return value of kmap_atomic, not the page.
 */
#define kunmap_atomic(__addr)					\
do {								\
	BUILD_BUG_ON(__same_type((__addr), struct page *));	\
	__kunmap_atomic(__addr);				\
} while (0)

#define kunmap_local(__addr)					\
do {								\
	BUILD_BUG_ON(__same_type((__addr), struct page *));	\
	__kunmap_local(__addr);					\
} while (0)

#endif
