// SPDX-License-Identifier: GPL-2.0-only
/*
 *  linux/arch/x86_64/mm/init.c
 *
 *  Copyright (C) 1995  Linus Torvalds
 *  Copyright (C) 2000  Pavel Machek <pavel@ucw.cz>
 *  Copyright (C) 2002,2003 Andi Kleen <ak@suse.de>
 */

#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/ptrace.h>
#include <linux/mman.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/smp.h>
#include <linux/init.h>
#include <linux/initrd.h>
#include <linux/pagemap.h>
#include <linux/memblock.h>
#include <linux/proc_fs.h>
#include <linux/pci.h>
#include <linux/pfn.h>
#include <linux/poison.h>
#include <linux/dma-mapping.h>
#include <linux/memory.h>
#include <linux/memory_hotplug.h>
#include <linux/memremap.h>
#include <linux/nmi.h>
#include <linux/gfp.h>
#include <linux/kcore.h>

#include <asm/processor.h>
#include <asm/bios_ebda.h>
#include <linux/uaccess.h>
#include <asm/pgalloc.h>
#include <asm/dma.h>
#include <asm/fixmap.h>
#include <asm/e820/api.h>
#include <asm/apic.h>
#include <asm/tlb.h>
#include <asm/mmu_context.h>
#include <asm/proto.h>
#include <asm/smp.h>
#include <asm/sections.h>
#include <asm/kdebug.h>
#include <asm/numa.h>
#include <asm/set_memory.h>
#include <asm/init.h>
#include <asm/uv/uv.h>
#include <asm/setup.h>
#include <asm/ftrace.h>

#include "mm_internal.h"

#include "ident_map.c"

#define DEFINE_POPULATE(fname, type1, type2, init)		\
static inline void fname##_init(struct mm_struct *mm,		\
		type1##_t *arg1, type2##_t *arg2, bool init)	\
{								\
	if (init)						\
		fname##_safe(mm, arg1, arg2);			\
	else							\
		fname(mm, arg1, arg2);				\
}

DEFINE_POPULATE(p4d_populate, p4d, pud, init)
DEFINE_POPULATE(pgd_populate, pgd, p4d, init)
DEFINE_POPULATE(pud_populate, pud, pmd, init)
DEFINE_POPULATE(pmd_populate_kernel, pmd, pte, init)

#define DEFINE_ENTRY(type1, type2, init)			\
static inline void set_##type1##_init(type1##_t *arg1,		\
			type2##_t arg2, bool init)		\
{								\
	if (init)						\
		set_##type1##_safe(arg1, arg2);			\
	else							\
		set_##type1(arg1, arg2);			\
}

DEFINE_ENTRY(p4d, p4d, init)
DEFINE_ENTRY(pud, pud, init)
DEFINE_ENTRY(pmd, pmd, init)
DEFINE_ENTRY(pte, pte, init)


/*
 * NOTE: pagetable_init alloc all the fixmap pagetables contiguous on the
 * physical space so we can cache the place of the first one and move
 * around without checking the pgd every time.
 */

/* Bits supported by the hardware: */
pteval_t __supported_pte_mask __read_mostly = ~0;
/* Bits allowed in normal kernel mappings: */
pteval_t __default_kernel_pte_mask __read_mostly = ~0;
EXPORT_SYMBOL_GPL(__supported_pte_mask);
/* Used in PAGE_KERNEL_* macros which are reasonably used out-of-tree: */
EXPORT_SYMBOL(__default_kernel_pte_mask);

int force_personality32;

/*
 * noexec32=on|off
 * Control non executable heap for 32bit processes.
 * To control the stack too use noexec=off
 *
 * on	PROT_READ does not imply PROT_EXEC for 32-bit processes (default)
 * off	PROT_READ implies PROT_EXEC
 */
static int __init nonx32_setup(char *str)
{
	if (!strcmp(str, "on"))
		force_personality32 &= ~READ_IMPLIES_EXEC;
	else if (!strcmp(str, "off"))
		force_personality32 |= READ_IMPLIES_EXEC;
	return 1;
}
__setup("noexec32=", nonx32_setup);

static void sync_global_pgds_l5(unsigned long start, unsigned long end)
{
	unsigned long addr;

	for (addr = start; addr <= end; addr = ALIGN(addr + 1, PGDIR_SIZE)) {
		const pgd_t *pgd_ref = pgd_offset_k(addr);
		struct page *page;

		/* Check for overflow */
		if (addr < start)
			break;

		if (pgd_none(*pgd_ref))
			continue;

		spin_lock(&pgd_lock);
		list_for_each_entry(page, &pgd_list, lru) {
			pgd_t *pgd;
			spinlock_t *pgt_lock;

			pgd = (pgd_t *)page_address(page) + pgd_index(addr);
			/* the pgt_lock only for Xen */
			pgt_lock = &pgd_page_get_mm(page)->page_table_lock;
			spin_lock(pgt_lock);

			if (!pgd_none(*pgd_ref) && !pgd_none(*pgd))
				BUG_ON(pgd_page_vaddr(*pgd) != pgd_page_vaddr(*pgd_ref));

			if (pgd_none(*pgd))
				set_pgd(pgd, *pgd_ref);

			spin_unlock(pgt_lock);
		}
		spin_unlock(&pgd_lock);
	}
}

static void sync_global_pgds_l4(unsigned long start, unsigned long end)
{
	unsigned long addr;

	for (addr = start; addr <= end; addr = ALIGN(addr + 1, PGDIR_SIZE)) {
		pgd_t *pgd_ref = pgd_offset_k(addr);
		const p4d_t *p4d_ref;
		struct page *page;

		/*
		 * With folded p4d, pgd_none() is always false, we need to
		 * handle synchronization on p4d level.
		 */
		MAYBE_BUILD_BUG_ON(pgd_none(*pgd_ref));
		p4d_ref = p4d_offset(pgd_ref, addr);

		if (p4d_none(*p4d_ref))
			continue;

		spin_lock(&pgd_lock);
		list_for_each_entry(page, &pgd_list, lru) {
			pgd_t *pgd;
			p4d_t *p4d;
			spinlock_t *pgt_lock;

			pgd = (pgd_t *)page_address(page) + pgd_index(addr);
			p4d = p4d_offset(pgd, addr);
			/* the pgt_lock only for Xen */
			pgt_lock = &pgd_page_get_mm(page)->page_table_lock;
			spin_lock(pgt_lock);

			if (!p4d_none(*p4d_ref) && !p4d_none(*p4d))
				BUG_ON(p4d_page_vaddr(*p4d)
				       != p4d_page_vaddr(*p4d_ref));

			if (p4d_none(*p4d))
				set_p4d(p4d, *p4d_ref);

			spin_unlock(pgt_lock);
		}
		spin_unlock(&pgd_lock);
	}
}

/*
 * When memory was added make sure all the processes MM have
 * suitable PGD entries in the local PGD level page.
 */
static void sync_global_pgds(unsigned long start, unsigned long end)
{
	if (pgtable_l5_enabled())
		sync_global_pgds_l5(start, end);
	else
		sync_global_pgds_l4(start, end);
}

/*
 * NOTE: This function is marked __ref because it calls __init function
 * (alloc_bootmem_pages). It's safe to do it ONLY when after_bootmem == 0.
 */
static __ref void *spp_getpage(void)
{
	void *ptr;

	if (after_bootmem)
		ptr = (void *) get_zeroed_page(GFP_ATOMIC);
	else
		ptr = memblock_alloc(PAGE_SIZE, PAGE_SIZE);

	if (!ptr || ((unsigned long)ptr & ~PAGE_MASK)) {
		panic("set_pte_phys: cannot allocate page data %s\n",
			after_bootmem ? "after bootmem" : "");
	}

	pr_debug("spp_getpage %p\n", ptr);

	return ptr;
}

static p4d_t *fill_p4d(pgd_t *pgd, unsigned long vaddr)
{
	if (pgd_none(*pgd)) {
		p4d_t *p4d = (p4d_t *)spp_getpage();
		pgd_populate(&init_mm, pgd, p4d);
		if (p4d != p4d_offset(pgd, 0))
			printk(KERN_ERR "PAGETABLE BUG #00! %p <-> %p\n",
			       p4d, p4d_offset(pgd, 0));
	}
	return p4d_offset(pgd, vaddr);
}

static pud_t *fill_pud(p4d_t *p4d, unsigned long vaddr)
{
	if (p4d_none(*p4d)) {
		pud_t *pud = (pud_t *)spp_getpage();
		p4d_populate(&init_mm, p4d, pud);
		if (pud != pud_offset(p4d, 0))
			printk(KERN_ERR "PAGETABLE BUG #01! %p <-> %p\n",
			       pud, pud_offset(p4d, 0));
	}
	return pud_offset(p4d, vaddr);
}

static pmd_t *fill_pmd(pud_t *pud, unsigned long vaddr)
{
	if (pud_none(*pud)) {
		pmd_t *pmd = (pmd_t *) spp_getpage();
		pud_populate(&init_mm, pud, pmd);
		if (pmd != pmd_offset(pud, 0))
			printk(KERN_ERR "PAGETABLE BUG #02! %p <-> %p\n",
			       pmd, pmd_offset(pud, 0));
	}
	return pmd_offset(pud, vaddr);
}

static pte_t *fill_pte(pmd_t *pmd, unsigned long vaddr)
{
	if (pmd_none(*pmd)) {
		pte_t *pte = (pte_t *) spp_getpage();
		pmd_populate_kernel(&init_mm, pmd, pte);
		if (pte != pte_offset_kernel(pmd, 0))
			printk(KERN_ERR "PAGETABLE BUG #03!\n");
	}
	return pte_offset_kernel(pmd, vaddr);
}

static void __set_pte_vaddr(pud_t *pud, unsigned long vaddr, pte_t new_pte)
{
	pmd_t *pmd = fill_pmd(pud, vaddr);
	pte_t *pte = fill_pte(pmd, vaddr);

	set_pte(pte, new_pte);

	/*
	 * It's enough to flush this one mapping.
	 * (PGE mappings get flushed as well)
	 */
	flush_tlb_one_kernel(vaddr);
}

void set_pte_vaddr_p4d(p4d_t *p4d_page, unsigned long vaddr, pte_t new_pte)
{
	p4d_t *p4d = p4d_page + p4d_index(vaddr);
	pud_t *pud = fill_pud(p4d, vaddr);

	__set_pte_vaddr(pud, vaddr, new_pte);
}

void set_pte_vaddr_pud(pud_t *pud_page, unsigned long vaddr, pte_t new_pte)
{
	pud_t *pud = pud_page + pud_index(vaddr);

	__set_pte_vaddr(pud, vaddr, new_pte);
}

void set_pte_vaddr(unsigned long vaddr, pte_t pteval)
{
	pgd_t *pgd;
	p4d_t *p4d_page;

	pr_debug("set_pte_vaddr %lx to %lx\n", vaddr, native_pte_val(pteval));

	pgd = pgd_offset_k(vaddr);
	if (pgd_none(*pgd)) {
		printk(KERN_ERR
			"PGD FIXMAP MISSING, it should be setup in head.S!\n");
		return;
	}

	p4d_page = p4d_offset(pgd, 0);
	set_pte_vaddr_p4d(p4d_page, vaddr, pteval);
}

pmd_t * __init populate_extra_pmd(unsigned long vaddr)
{
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;

	pgd = pgd_offset_k(vaddr);
	p4d = fill_p4d(pgd, vaddr);
	pud = fill_pud(p4d, vaddr);
	return fill_pmd(pud, vaddr);
}

pte_t * __init populate_extra_pte(unsigned long vaddr)
{
	pmd_t *pmd;

	pmd = populate_extra_pmd(vaddr);
	return fill_pte(pmd, vaddr);
}

/*
 * Create large page table mappings for a range of physical addresses.
 */
static void __init __init_extra_mapping(unsigned long phys, unsigned long size,
					enum page_cache_mode cache)
{
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pgprot_t prot;

	pgprot_val(prot) = pgprot_val(PAGE_KERNEL_LARGE) |
		protval_4k_2_large(cachemode2protval(cache));
	BUG_ON((phys & ~PMD_MASK) || (size & ~PMD_MASK));
	for (; size; phys += PMD_SIZE, size -= PMD_SIZE) {
		pgd = pgd_offset_k((unsigned long)__va(phys));
		if (pgd_none(*pgd)) {
			p4d = (p4d_t *) spp_getpage();
			set_pgd(pgd, __pgd(__pa(p4d) | _KERNPG_TABLE |
						_PAGE_USER));
		}
		p4d = p4d_offset(pgd, (unsigned long)__va(phys));
		if (p4d_none(*p4d)) {
			pud = (pud_t *) spp_getpage();
			set_p4d(p4d, __p4d(__pa(pud) | _KERNPG_TABLE |
						_PAGE_USER));
		}
		pud = pud_offset(p4d, (unsigned long)__va(phys));
		if (pud_none(*pud)) {
			pmd = (pmd_t *) spp_getpage();
			set_pud(pud, __pud(__pa(pmd) | _KERNPG_TABLE |
						_PAGE_USER));
		}
		pmd = pmd_offset(pud, phys);
		BUG_ON(!pmd_none(*pmd));
		set_pmd(pmd, __pmd(phys | pgprot_val(prot)));
	}
}

void __init init_extra_mapping_wb(unsigned long phys, unsigned long size)
{
	__init_extra_mapping(phys, size, _PAGE_CACHE_MODE_WB);
}

void __init init_extra_mapping_uc(unsigned long phys, unsigned long size)
{
	__init_extra_mapping(phys, size, _PAGE_CACHE_MODE_UC);
}

/*
 * The head.S code sets up the kernel high mapping:
 *
 *   from __START_KERNEL_map to __START_KERNEL_map + size (== _end-_text)
 *
 * phys_base holds the negative offset to the kernel, which is added
 * to the compile time generated pmds. This results in invalid pmds up
 * to the point where we hit the physaddr 0 mapping.
 *
 * We limit the mappings to the region from _text to _brk_end.  _brk_end
 * is rounded up to the 2MB boundary. This catches the invalid pmds as
 * well, as they are located before _text:
 */
void __init cleanup_highmap(void)
{
	unsigned long vaddr = __START_KERNEL_map;
	unsigned long vaddr_end = __START_KERNEL_map + KERNEL_IMAGE_SIZE;
	unsigned long end = roundup((unsigned long)_brk_end, PMD_SIZE) - 1;
	pmd_t *pmd = level2_kernel_pgt;

	/*
	 * Native path, max_pfn_mapped is not set yet.
	 * Xen has valid max_pfn_mapped set in
	 *	arch/x86/xen/mmu.c:xen_setup_kernel_pagetable().
	 */
	if (max_pfn_mapped)
		vaddr_end = __START_KERNEL_map + (max_pfn_mapped << PAGE_SHIFT);

	for (; vaddr + PMD_SIZE - 1 < vaddr_end; pmd++, vaddr += PMD_SIZE) {
		if (pmd_none(*pmd))
			continue;
		if (vaddr < (unsigned long) _text || vaddr > end)
			set_pmd(pmd, __pmd(0));
	}
}

/*
 * Create PTE level page table mapping for physical addresses.
 * It returns the last physical address mapped.
 */
static unsigned long __meminit
phys_pte_init(pte_t *pte_page, unsigned long paddr, unsigned long paddr_end,
	      pgprot_t prot, bool init)
{
	unsigned long pages = 0, paddr_next;
	unsigned long paddr_last = paddr_end;
	pte_t *pte;
	int i;

	pte = pte_page + pte_index(paddr);
	i = pte_index(paddr);

	for (; i < PTRS_PER_PTE; i++, paddr = paddr_next, pte++) {
		paddr_next = (paddr & PAGE_MASK) + PAGE_SIZE;
		if (paddr >= paddr_end) {
			if (!after_bootmem &&
			    !e820__mapped_any(paddr & PAGE_MASK, paddr_next,
					     E820_TYPE_RAM) &&
			    !e820__mapped_any(paddr & PAGE_MASK, paddr_next,
					     E820_TYPE_RESERVED_KERN))
				set_pte_init(pte, __pte(0), init);
			continue;
		}

		/*
		 * We will re-use the existing mapping.
		 * Xen for example has some special requirements, like mapping
		 * pagetable pages as RO. So assume someone who pre-setup
		 * these mappings are more intelligent.
		 */
		if (!pte_none(*pte)) {
			if (!after_bootmem)
				pages++;
			continue;
		}

		if (0)
			pr_info("   pte=%p addr=%lx pte=%016lx\n", pte, paddr,
				pfn_pte(paddr >> PAGE_SHIFT, PAGE_KERNEL).pte);
		pages++;
		set_pte_init(pte, pfn_pte(paddr >> PAGE_SHIFT, prot), init);
		paddr_last = (paddr & PAGE_MASK) + PAGE_SIZE;
	}

	update_page_count(PG_LEVEL_4K, pages);

	return paddr_last;
}

/*
 * Create PMD level page table mapping for physical addresses. The virtual
 * and physical address have to be aligned at this level.
 * It returns the last physical address mapped.
 */
static unsigned long __meminit
phys_pmd_init(pmd_t *pmd_page, unsigned long paddr, unsigned long paddr_end,
	      unsigned long page_size_mask, pgprot_t prot, bool init)
{
	unsigned long pages = 0, paddr_next;
	unsigned long paddr_last = paddr_end;

	int i = pmd_index(paddr);

	for (; i < PTRS_PER_PMD; i++, paddr = paddr_next) {
		pmd_t *pmd = pmd_page + pmd_index(paddr);
		pte_t *pte;
		pgprot_t new_prot = prot;

		paddr_next = (paddr & PMD_MASK) + PMD_SIZE;
		if (paddr >= paddr_end) {
			if (!after_bootmem &&
			    !e820__mapped_any(paddr & PMD_MASK, paddr_next,
					     E820_TYPE_RAM) &&
			    !e820__mapped_any(paddr & PMD_MASK, paddr_next,
					     E820_TYPE_RESERVED_KERN))
				set_pmd_init(pmd, __pmd(0), init);
			continue;
		}

		if (!pmd_none(*pmd)) {
			if (!pmd_large(*pmd)) {
				spin_lock(&init_mm.page_table_lock);
				pte = (pte_t *)pmd_page_vaddr(*pmd);
				paddr_last = phys_pte_init(pte, paddr,
							   paddr_end, prot,
							   init);
				spin_unlock(&init_mm.page_table_lock);
				continue;
			}
			/*
			 * If we are ok with PG_LEVEL_2M mapping, then we will
			 * use the existing mapping,
			 *
			 * Otherwise, we will split the large page mapping but
			 * use the same existing protection bits except for
			 * large page, so that we don't violate Intel's TLB
			 * Application note (317080) which says, while changing
			 * the page sizes, new and old translations should
			 * not differ with respect to page frame and
			 * attributes.
			 */
			if (page_size_mask & (1 << PG_LEVEL_2M)) {
				if (!after_bootmem)
					pages++;
				paddr_last = paddr_next;
				continue;
			}
			new_prot = pte_pgprot(pte_clrhuge(*(pte_t *)pmd));
		}

		if (page_size_mask & (1<<PG_LEVEL_2M)) {
			pages++;
			spin_lock(&init_mm.page_table_lock);
			set_pte_init((pte_t *)pmd,
				     pfn_pte((paddr & PMD_MASK) >> PAGE_SHIFT,
					     __pgprot(pgprot_val(prot) | _PAGE_PSE)),
				     init);
			spin_unlock(&init_mm.page_table_lock);
			paddr_last = paddr_next;
			continue;
		}

		pte = alloc_low_page();
		paddr_last = phys_pte_init(pte, paddr, paddr_end, new_prot, init);

		spin_lock(&init_mm.page_table_lock);
		pmd_populate_kernel_init(&init_mm, pmd, pte, init);
		spin_unlock(&init_mm.page_table_lock);
	}
	update_page_count(PG_LEVEL_2M, pages);
	return paddr_last;
}

/*
 * Create PUD level page table mapping for physical addresses. The virtual
 * and physical address do not have to be aligned at this level. KASLR can
 * randomize virtual addresses up to this level.
 * It returns the last physical address mapped.
 */
static unsigned long __meminit
phys_pud_init(pud_t *pud_page, unsigned long paddr, unsigned long paddr_end,
	      unsigned long page_size_mask, pgprot_t _prot, bool init)
{
	unsigned long pages = 0, paddr_next;
	unsigned long paddr_last = paddr_end;
	unsigned long vaddr = (unsigned long)__va(paddr);
	int i = pud_index(vaddr);

	for (; i < PTRS_PER_PUD; i++, paddr = paddr_next) {
		pud_t *pud;
		pmd_t *pmd;
		pgprot_t prot = _prot;

		vaddr = (unsigned long)__va(paddr);
		pud = pud_page + pud_index(vaddr);
		paddr_next = (paddr & PUD_MASK) + PUD_SIZE;

		if (paddr >= paddr_end) {
			if (!after_bootmem &&
			    !e820__mapped_any(paddr & PUD_MASK, paddr_next,
					     E820_TYPE_RAM) &&
			    !e820__mapped_any(paddr & PUD_MASK, paddr_next,
					     E820_TYPE_RESERVED_KERN))
				set_pud_init(pud, __pud(0), init);
			continue;
		}

		if (!pud_none(*pud)) {
			if (!pud_large(*pud)) {
				pmd = pmd_offset(pud, 0);
				paddr_last = phys_pmd_init(pmd, paddr,
							   paddr_end,
							   page_size_mask,
							   prot, init);
				continue;
			}
			/*
			 * If we are ok with PG_LEVEL_1G mapping, then we will
			 * use the existing mapping.
			 *
			 * Otherwise, we will split the gbpage mapping but use
			 * the same existing protection  bits except for large
			 * page, so that we don't violate Intel's TLB
			 * Application note (317080) which says, while changing
			 * the page sizes, new and old translations should
			 * not differ with respect to page frame and
			 * attributes.
			 */
			if (page_size_mask & (1 << PG_LEVEL_1G)) {
				if (!after_bootmem)
					pages++;
				paddr_last = paddr_next;
				continue;
			}
			prot = pte_pgprot(pte_clrhuge(*(pte_t *)pud));
		}

		if (page_size_mask & (1<<PG_LEVEL_1G)) {
			pages++;
			spin_lock(&init_mm.page_table_lock);

			prot = __pgprot(pgprot_val(prot) | __PAGE_KERNEL_LARGE);

			set_pte_init((pte_t *)pud,
				     pfn_pte((paddr & PUD_MASK) >> PAGE_SHIFT,
					     prot),
				     init);
			spin_unlock(&init_mm.page_table_lock);
			paddr_last = paddr_next;
			continue;
		}

		pmd = alloc_low_page();
		paddr_last = phys_pmd_init(pmd, paddr, paddr_end,
					   page_size_mask, prot, init);

		spin_lock(&init_mm.page_table_lock);
		pud_populate_init(&init_mm, pud, pmd, init);
		spin_unlock(&init_mm.page_table_lock);
	}

	update_page_count(PG_LEVEL_1G, pages);

	return paddr_last;
}

static unsigned long __meminit
phys_p4d_init(p4d_t *p4d_page, unsigned long paddr, unsigned long paddr_end,
	      unsigned long page_size_mask, pgprot_t prot, bool init)
{
	unsigned long vaddr, vaddr_end, vaddr_next, paddr_next, paddr_last;

	paddr_last = paddr_end;
	vaddr = (unsigned long)__va(paddr);
	vaddr_end = (unsigned long)__va(paddr_end);

	if (!pgtable_l5_enabled())
		return phys_pud_init((pud_t *) p4d_page, paddr, paddr_end,
				     page_size_mask, prot, init);

	for (; vaddr < vaddr_end; vaddr = vaddr_next) {
		p4d_t *p4d = p4d_page + p4d_index(vaddr);
		pud_t *pud;

		vaddr_next = (vaddr & P4D_MASK) + P4D_SIZE;
		paddr = __pa(vaddr);

		if (paddr >= paddr_end) {
			paddr_next = __pa(vaddr_next);
			if (!after_bootmem &&
			    !e820__mapped_any(paddr & P4D_MASK, paddr_next,
					     E820_TYPE_RAM) &&
			    !e820__mapped_any(paddr & P4D_MASK, paddr_next,
					     E820_TYPE_RESERVED_KERN))
				set_p4d_init(p4d, __p4d(0), init);
			continue;
		}

		if (!p4d_none(*p4d)) {
			pud = pud_offset(p4d, 0);
			paddr_last = phys_pud_init(pud, paddr, __pa(vaddr_end),
					page_size_mask, prot, init);
			continue;
		}

		pud = alloc_low_page();
		paddr_last = phys_pud_init(pud, paddr, __pa(vaddr_end),
					   page_size_mask, prot, init);

		spin_lock(&init_mm.page_table_lock);
		p4d_populate_init(&init_mm, p4d, pud, init);
		spin_unlock(&init_mm.page_table_lock);
	}

	return paddr_last;
}

static unsigned long __meminit
__kernel_physical_mapping_init(unsigned long paddr_start,
			       unsigned long paddr_end,
			       unsigned long page_size_mask,
			       pgprot_t prot, bool init)
{
	bool pgd_changed = false;
	unsigned long vaddr, vaddr_start, vaddr_end, vaddr_next, paddr_last;

	paddr_last = paddr_end;
	vaddr = (unsigned long)__va(paddr_start);
	vaddr_end = (unsigned long)__va(paddr_end);
	vaddr_start = vaddr;

	for (; vaddr < vaddr_end; vaddr = vaddr_next) {
		pgd_t *pgd = pgd_offset_k(vaddr);
		p4d_t *p4d;

		vaddr_next = (vaddr & PGDIR_MASK) + PGDIR_SIZE;

		if (pgd_val(*pgd)) {
			p4d = (p4d_t *)pgd_page_vaddr(*pgd);
			paddr_last = phys_p4d_init(p4d, __pa(vaddr),
						   __pa(vaddr_end),
						   page_size_mask,
						   prot, init);
			continue;
		}

		p4d = alloc_low_page();
		paddr_last = phys_p4d_init(p4d, __pa(vaddr), __pa(vaddr_end),
					   page_size_mask, prot, init);

		spin_lock(&init_mm.page_table_lock);
		if (pgtable_l5_enabled())
			pgd_populate_init(&init_mm, pgd, p4d, init);
		else
			p4d_populate_init(&init_mm, p4d_offset(pgd, vaddr),
					  (pud_t *) p4d, init);

		spin_unlock(&init_mm.page_table_lock);
		pgd_changed = true;
	}

	if (pgd_changed)
		sync_global_pgds(vaddr_start, vaddr_end - 1);

	return paddr_last;
}


/*
 * Create page table mapping for the physical memory for specific physical
 * addresses. Note that it can only be used to populate non-present entries.
 * The virtual and physical addresses have to be aligned on PMD level
 * down. It returns the last physical address mapped.
 */
unsigned long __meminit
kernel_physical_mapping_init(unsigned long paddr_start,
			     unsigned long paddr_end,
			     unsigned long page_size_mask, pgprot_t prot)
{
	return __kernel_physical_mapping_init(paddr_start, paddr_end,
					      page_size_mask, prot, true);
}

/*
 * This function is similar to kernel_physical_mapping_init() above with the
 * exception that it uses set_{pud,pmd}() instead of the set_{pud,pte}_safe()
 * when updating the mapping. The caller is responsible to flush the TLBs after
 * the function returns.
 */
unsigned long __meminit
kernel_physical_mapping_change(unsigned long paddr_start,
			       unsigned long paddr_end,
			       unsigned long page_size_mask)
{
	return __kernel_physical_mapping_init(paddr_start, paddr_end,
					      page_size_mask, PAGE_KERNEL,
					      false);
}

#ifndef CONFIG_NUMA
void __init initmem_init(void)
{
	memblock_set_node(0, PHYS_ADDR_MAX, &memblock.memory, 0);
}
#endif

void __init paging_init(void)
{
	sparse_init();

	/*
	 * clear the default setting with node 0
	 * note: don't use nodes_clear here, that is really clearing when
	 *	 numa support is not compiled in, and later node_set_state
	 *	 will not set it back.
	 */
	node_clear_state(0, N_MEMORY);
	node_clear_state(0, N_NORMAL_MEMORY);

	zone_sizes_init();
}

#ifdef CONFIG_SPARSEMEM_VMEMMAP
#define PAGE_UNUSED 0xFD

/*
 * The unused vmemmap range, which was not yet memset(PAGE_UNUSED), ranges
 * from unused_pmd_start to next PMD_SIZE boundary.
 */
static unsigned long unused_pmd_start __meminitdata;

static void __meminit vmemmap_flush_unused_pmd(void)
{
	if (!unused_pmd_start)
		return;
	/*
	 * Clears (unused_pmd_start, PMD_END]
	 */
	memset((void *)unused_pmd_start, PAGE_UNUSED,
	       ALIGN(unused_pmd_start, PMD_SIZE) - unused_pmd_start);
	unused_pmd_start = 0;
}

#ifdef CONFIG_MEMORY_HOTPLUG
/* Returns true if the PMD is completely unused and thus it can be freed */
static bool __meminit vmemmap_pmd_is_unused(unsigned long addr, unsigned long end)
{
	unsigned long start = ALIGN_DOWN(addr, PMD_SIZE);

	/*
	 * Flush the unused range cache to ensure that memchr_inv() will work
	 * for the whole range.
	 */
	vmemmap_flush_unused_pmd();
	memset((void *)addr, PAGE_UNUSED, end - addr);

	return !memchr_inv((void *)start, PAGE_UNUSED, PMD_SIZE);
}
#endif

static void __meminit __vmemmap_use_sub_pmd(unsigned long start)
{
	/*
	 * As we expect to add in the same granularity as we remove, it's
	 * sufficient to mark only some piece used to block the memmap page from
	 * getting removed when removing some other adjacent memmap (just in
	 * case the first memmap never gets initialized e.g., because the memory
	 * block never gets onlined).
	 */
	memset((void *)start, 0, sizeof(struct page));
}

static void __meminit vmemmap_use_sub_pmd(unsigned long start, unsigned long end)
{
	/*
	 * We only optimize if the new used range directly follows the
	 * previously unused range (esp., when populating consecutive sections).
	 */
	if (unused_pmd_start == start) {
		if (likely(IS_ALIGNED(end, PMD_SIZE)))
			unused_pmd_start = 0;
		else
			unused_pmd_start = end;
		return;
	}

	/*
	 * If the range does not contiguously follows previous one, make sure
	 * to mark the unused range of the previous one so it can be removed.
	 */
	vmemmap_flush_unused_pmd();
	__vmemmap_use_sub_pmd(start);
}


static void __meminit vmemmap_use_new_sub_pmd(unsigned long start, unsigned long end)
{
	vmemmap_flush_unused_pmd();

	/*
	 * Could be our memmap page is filled with PAGE_UNUSED already from a
	 * previous remove. Make sure to reset it.
	 */
	__vmemmap_use_sub_pmd(start);

	/*
	 * Mark with PAGE_UNUSED the unused parts of the new memmap range
	 */
	if (!IS_ALIGNED(start, PMD_SIZE))
		memset((void *)start, PAGE_UNUSED,
			start - ALIGN_DOWN(start, PMD_SIZE));

	/*
	 * We want to avoid memset(PAGE_UNUSED) when populating the vmemmap of
	 * consecutive sections. Remember for the last added PMD where the
	 * unused range begins.
	 */
	if (!IS_ALIGNED(end, PMD_SIZE))
		unused_pmd_start = end;
}
#endif

/*
 * Memory hotplug specific functions
 */
#ifdef CONFIG_MEMORY_HOTPLUG
/*
 * After memory hotplug the variables max_pfn, max_low_pfn and high_memory need
 * updating.
 */
static void update_end_of_memory_vars(u64 start, u64 size)
{
	unsigned long end_pfn = PFN_UP(start + size);

	if (end_pfn > max_pfn) {
		max_pfn = end_pfn;
		max_low_pfn = end_pfn;
		high_memory = (void *)__va(max_pfn * PAGE_SIZE - 1) + 1;
	}
}

int add_pages(int nid, unsigned long start_pfn, unsigned long nr_pages,
	      struct mhp_params *params)
{
	int ret;

	ret = __add_pages(nid, start_pfn, nr_pages, params);
	WARN_ON_ONCE(ret);

	/* update max_pfn, max_low_pfn and high_memory */
	update_end_of_memory_vars(start_pfn << PAGE_SHIFT,
				  nr_pages << PAGE_SHIFT);

	return ret;
}

int arch_add_memory(int nid, u64 start, u64 size,
		    struct mhp_params *params)
{
	unsigned long start_pfn = start >> PAGE_SHIFT;
	unsigned long nr_pages = size >> PAGE_SHIFT;

	init_memory_mapping(start, start + size, params->pgprot);

	return add_pages(nid, start_pfn, nr_pages, params);
}

static void __meminit free_pagetable(struct page *page, int order)
{
	unsigned long magic;
	unsigned int nr_pages = 1 << order;

	/* bootmem page has reserved flag */
	if (PageReserved(page)) {
		__ClearPageReserved(page);

		magic = (unsigned long)page->freelist;
		if (magic == SECTION_INFO || magic == MIX_SECTION_INFO) {
			while (nr_pages--)
				put_page_bootmem(page++);
		} else
			while (nr_pages--)
				free_reserved_page(page++);
	} else
		free_pages((unsigned long)page_address(page), order);
}

static void __meminit free_hugepage_table(struct page *page,
		struct vmem_altmap *altmap)
{
	if (altmap)
		vmem_altmap_free(altmap, PMD_SIZE / PAGE_SIZE);
	else
		free_pagetable(page, get_order(PMD_SIZE));
}

static void __meminit free_pte_table(pte_t *pte_start, pmd_t *pmd)
{
	pte_t *pte;
	int i;

	for (i = 0; i < PTRS_PER_PTE; i++) {
		pte = pte_start + i;
		if (!pte_none(*pte))
			return;
	}

	/* free a pte talbe */
	free_pagetable(pmd_page(*pmd), 0);
	spin_lock(&init_mm.page_table_lock);
	pmd_clear(pmd);
	spin_unlock(&init_mm.page_table_lock);
}

static void __meminit free_pmd_table(pmd_t *pmd_start, pud_t *pud)
{
	pmd_t *pmd;
	int i;

	for (i = 0; i < PTRS_PER_PMD; i++) {
		pmd = pmd_start + i;
		if (!pmd_none(*pmd))
			return;
	}

	/* free a pmd talbe */
	free_pagetable(pud_page(*pud), 0);
	spin_lock(&init_mm.page_table_lock);
	pud_clear(pud);
	spin_unlock(&init_mm.page_table_lock);
}

static void __meminit free_pud_table(pud_t *pud_start, p4d_t *p4d)
{
	pud_t *pud;
	int i;

	for (i = 0; i < PTRS_PER_PUD; i++) {
		pud = pud_start + i;
		if (!pud_none(*pud))
			return;
	}

	/* free a pud talbe */
	free_pagetable(p4d_page(*p4d), 0);
	spin_lock(&init_mm.page_table_lock);
	p4d_clear(p4d);
	spin_unlock(&init_mm.page_table_lock);
}

static void __meminit
remove_pte_table(pte_t *pte_start, unsigned long addr, unsigned long end,
		 bool direct)
{
	unsigned long next, pages = 0;
	pte_t *pte;
	phys_addr_t phys_addr;

	pte = pte_start + pte_index(addr);
	for (; addr < end; addr = next, pte++) {
		next = (addr + PAGE_SIZE) & PAGE_MASK;
		if (next > end)
			next = end;

		if (!pte_present(*pte))
			continue;

		/*
		 * We mapped [0,1G) memory as identity mapping when
		 * initializing, in arch/x86/kernel/head_64.S. These
		 * pagetables cannot be removed.
		 */
		phys_addr = pte_val(*pte) + (addr & PAGE_MASK);
		if (phys_addr < (phys_addr_t)0x40000000)
			return;

		if (!direct)
			free_pagetable(pte_page(*pte), 0);

		spin_lock(&init_mm.page_table_lock);
		pte_clear(&init_mm, addr, pte);
		spin_unlock(&init_mm.page_table_lock);

		/* For non-direct mapping, pages means nothing. */
		pages++;
	}

	/* Call free_pte_table() in remove_pmd_table(). */
	flush_tlb_all();
	if (direct)
		update_page_count(PG_LEVEL_4K, -pages);
}

static void __meminit
remove_pmd_table(pmd_t *pmd_start, unsigned long addr, unsigned long end,
		 bool direct, struct vmem_altmap *altmap)
{
	unsigned long next, pages = 0;
	pte_t *pte_base;
	pmd_t *pmd;

	pmd = pmd_start + pmd_index(addr);
	for (; addr < end; addr = next, pmd++) {
		next = pmd_addr_end(addr, end);

		if (!pmd_present(*pmd))
			continue;

		if (pmd_large(*pmd)) {
			if (IS_ALIGNED(addr, PMD_SIZE) &&
			    IS_ALIGNED(next, PMD_SIZE)) {
				if (!direct)
					free_hugepage_table(pmd_page(*pmd),
							    altmap);

				spin_lock(&init_mm.page_table_lock);
				pmd_clear(pmd);
				spin_unlock(&init_mm.page_table_lock);
				pages++;
			}
#ifdef CONFIG_SPARSEMEM_VMEMMAP
			else if (vmemmap_pmd_is_unused(addr, next)) {
					free_hugepage_table(pmd_page(*pmd),
							    altmap);
					spin_lock(&init_mm.page_table_lock);
					pmd_clear(pmd);
					spin_unlock(&init_mm.page_table_lock);
			}
#endif
			continue;
		}

		pte_base = (pte_t *)pmd_page_vaddr(*pmd);
		remove_pte_table(pte_base, addr, next, direct);
		free_pte_table(pte_base, pmd);
	}

	/* Call free_pmd_table() in remove_pud_table(). */
	if (direct)
		update_page_count(PG_LEVEL_2M, -pages);
}

static void __meminit
remove_pud_table(pud_t *pud_start, unsigned long addr, unsigned long end,
		 struct vmem_altmap *altmap, bool direct)
{
	unsigned long next, pages = 0;
	pmd_t *pmd_base;
	pud_t *pud;

	pud = pud_start + pud_index(addr);
	for (; addr < end; addr = next, pud++) {
		next = pud_addr_end(addr, end);

		if (!pud_present(*pud))
			continue;

		if (pud_large(*pud) &&
		    IS_ALIGNED(addr, PUD_SIZE) &&
		    IS_ALIGNED(next, PUD_SIZE)) {
			spin_lock(&init_mm.page_table_lock);
			pud_clear(pud);
			spin_unlock(&init_mm.page_table_lock);
			pages++;
			continue;
		}

		pmd_base = pmd_offset(pud, 0);
		remove_pmd_table(pmd_base, addr, next, direct, altmap);
		free_pmd_table(pmd_base, pud);
	}

	if (direct)
		update_page_count(PG_LEVEL_1G, -pages);
}

static void __meminit
remove_p4d_table(p4d_t *p4d_start, unsigned long addr, unsigned long end,
		 struct vmem_altmap *altmap, bool direct)
{
	unsigned long next, pages = 0;
	pud_t *pud_base;
	p4d_t *p4d;

	p4d = p4d_start + p4d_index(addr);
	for (; addr < end; addr = next, p4d++) {
		next = p4d_addr_end(addr, end);

		if (!p4d_present(*p4d))
			continue;

		BUILD_BUG_ON(p4d_large(*p4d));

		pud_base = pud_offset(p4d, 0);
		remove_pud_table(pud_base, addr, next, altmap, direct);
		/*
		 * For 4-level page tables we do not want to free PUDs, but in the
		 * 5-level case we should free them. This code will have to change
		 * to adapt for boot-time switching between 4 and 5 level page tables.
		 */
		if (pgtable_l5_enabled())
			free_pud_table(pud_base, p4d);
	}

	if (direct)
		update_page_count(PG_LEVEL_512G, -pages);
}

/* start and end are both virtual address. */
static void __meminit
remove_pagetable(unsigned long start, unsigned long end, bool direct,
		struct vmem_altmap *altmap)
{
	unsigned long next;
	unsigned long addr;
	pgd_t *pgd;
	p4d_t *p4d;

	for (addr = start; addr < end; addr = next) {
		next = pgd_addr_end(addr, end);

		pgd = pgd_offset_k(addr);
		if (!pgd_present(*pgd))
			continue;

		p4d = p4d_offset(pgd, 0);
		remove_p4d_table(p4d, addr, next, altmap, direct);
	}

	flush_tlb_all();
}

void __ref vmemmap_free(unsigned long start, unsigned long end,
		struct vmem_altmap *altmap)
{
	VM_BUG_ON(!IS_ALIGNED(start, PAGE_SIZE));
	VM_BUG_ON(!IS_ALIGNED(end, PAGE_SIZE));

	remove_pagetable(start, end, false, altmap);
}

static void __meminit
kernel_physical_mapping_remove(unsigned long start, unsigned long end)
{
	start = (unsigned long)__va(start);
	end = (unsigned long)__va(end);

	remove_pagetable(start, end, true, NULL);
}

void __ref arch_remove_memory(int nid, u64 start, u64 size,
			      struct vmem_altmap *altmap)
{
	unsigned long start_pfn = start >> PAGE_SHIFT;
	unsigned long nr_pages = size >> PAGE_SHIFT;

	__remove_pages(start_pfn, nr_pages, altmap);
	kernel_physical_mapping_remove(start, start + size);
}
#endif /* CONFIG_MEMORY_HOTPLUG */

static struct kcore_list kcore_vsyscall;

static void __init register_page_bootmem_info(void)
{
#ifdef CONFIG_NUMA
	int i;

	for_each_online_node(i)
		register_page_bootmem_info_node(NODE_DATA(i));
#endif
}

/*
 * Pre-allocates page-table pages for the vmalloc area in the kernel page-table.
 * Only the level which needs to be synchronized between all page-tables is
 * allocated because the synchronization can be expensive.
 */
static void __init preallocate_vmalloc_pages(void)
{
	unsigned long addr;
	const char *lvl;

	for (addr = VMALLOC_START; addr <= VMALLOC_END; addr = ALIGN(addr + 1, PGDIR_SIZE)) {
		pgd_t *pgd = pgd_offset_k(addr);
		p4d_t *p4d;
		pud_t *pud;

		lvl = "p4d";
		p4d = p4d_alloc(&init_mm, pgd, addr);
		if (!p4d)
			goto failed;

		if (pgtable_l5_enabled())
			continue;

		/*
		 * The goal here is to allocate all possibly required
		 * hardware page tables pointed to by the top hardware
		 * level.
		 *
		 * On 4-level systems, the P4D layer is folded away and
		 * the above code does no preallocation.  Below, go down
		 * to the pud _software_ level to ensure the second
		 * hardware level is allocated on 4-level systems too.
		 */
		lvl = "pud";
		pud = pud_alloc(&init_mm, p4d, addr);
		if (!pud)
			goto failed;
	}

	return;

failed:

	/*
	 * The pages have to be there now or they will be missing in
	 * process page-tables later.
	 */
	panic("Failed to pre-allocate %s pages for vmalloc area\n", lvl);
}

void __init mem_init(void)
{
	pci_iommu_alloc();

	/* clear_bss() already clear the empty_zero_page */

	/* this will put all memory onto the freelists */
	memblock_free_all();
	after_bootmem = 1;
	x86_init.hyper.init_after_bootmem();

	/*
	 * Must be done after boot memory is put on freelist, because here we
	 * might set fields in deferred struct pages that have not yet been
	 * initialized, and memblock_free_all() initializes all the reserved
	 * deferred pages for us.
	 */
	register_page_bootmem_info();

	/* Register memory areas for /proc/kcore */
	if (get_gate_vma(&init_mm))
		kclist_add(&kcore_vsyscall, (void *)VSYSCALL_ADDR, PAGE_SIZE, KCORE_USER);

	preallocate_vmalloc_pages();
}

#ifdef CONFIG_DEFERRED_STRUCT_PAGE_INIT
int __init deferred_page_init_max_threads(const struct cpumask *node_cpumask)
{
	/*
	 * More CPUs always led to greater speedups on tested systems, up to
	 * all the nodes' CPUs.  Use all since the system is otherwise idle
	 * now.
	 */
	return max_t(int, cpumask_weight(node_cpumask), 1);
}
#endif

int kernel_set_to_readonly;

void mark_rodata_ro(void)
{
	unsigned long start = PFN_ALIGN(_text);
	unsigned long rodata_start = PFN_ALIGN(__start_rodata);
	unsigned long end = (unsigned long)__end_rodata_hpage_align;
	unsigned long text_end = PFN_ALIGN(_etext);
	unsigned long rodata_end = PFN_ALIGN(__end_rodata);
	unsigned long all_end;

	printk(KERN_INFO "Write protecting the kernel read-only data: %luk\n",
	       (end - start) >> 10);
	set_memory_ro(start, (end - start) >> PAGE_SHIFT);

	kernel_set_to_readonly = 1;

	/*
	 * The rodata/data/bss/brk section (but not the kernel text!)
	 * should also be not-executable.
	 *
	 * We align all_end to PMD_SIZE because the existing mapping
	 * is a full PMD. If we would align _brk_end to PAGE_SIZE we
	 * split the PMD and the reminder between _brk_end and the end
	 * of the PMD will remain mapped executable.
	 *
	 * Any PMD which was setup after the one which covers _brk_end
	 * has been zapped already via cleanup_highmem().
	 */
	all_end = roundup((unsigned long)_brk_end, PMD_SIZE);
	set_memory_nx(text_end, (all_end - text_end) >> PAGE_SHIFT);

	set_ftrace_ops_ro();

#ifdef CONFIG_CPA_DEBUG
	printk(KERN_INFO "Testing CPA: undo %lx-%lx\n", start, end);
	set_memory_rw(start, (end-start) >> PAGE_SHIFT);

	printk(KERN_INFO "Testing CPA: again\n");
	set_memory_ro(start, (end-start) >> PAGE_SHIFT);
#endif

	free_kernel_image_pages("unused kernel image (text/rodata gap)",
				(void *)text_end, (void *)rodata_start);
	free_kernel_image_pages("unused kernel image (rodata/data gap)",
				(void *)rodata_end, (void *)_sdata);

	debug_checkwx();
}

int kern_addr_valid(unsigned long addr)
{
	unsigned long above = ((long)addr) >> __VIRTUAL_MASK_SHIFT;
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	if (above != 0 && above != -1UL)
		return 0;

	pgd = pgd_offset_k(addr);
	if (pgd_none(*pgd))
		return 0;

	p4d = p4d_offset(pgd, addr);
	if (p4d_none(*p4d))
		return 0;

	pud = pud_offset(p4d, addr);
	if (pud_none(*pud))
		return 0;

	if (pud_large(*pud))
		return pfn_valid(pud_pfn(*pud));

	pmd = pmd_offset(pud, addr);
	if (pmd_none(*pmd))
		return 0;

	if (pmd_large(*pmd))
		return pfn_valid(pmd_pfn(*pmd));

	pte = pte_offset_kernel(pmd, addr);
	if (pte_none(*pte))
		return 0;

	return pfn_valid(pte_pfn(*pte));
}

/*
 * Block size is the minimum amount of memory which can be hotplugged or
 * hotremoved. It must be power of two and must be equal or larger than
 * MIN_MEMORY_BLOCK_SIZE.
 */
#define MAX_BLOCK_SIZE (2UL << 30)

/* Amount of ram needed to start using large blocks */
#define MEM_SIZE_FOR_LARGE_BLOCK (64UL << 30)

/* Adjustable memory block size */
static unsigned long set_memory_block_size;
int __init set_memory_block_size_order(unsigned int order)
{
	unsigned long size = 1UL << order;

	if (size > MEM_SIZE_FOR_LARGE_BLOCK || size < MIN_MEMORY_BLOCK_SIZE)
		return -EINVAL;

	set_memory_block_size = size;
	return 0;
}

static unsigned long probe_memory_block_size(void)
{
	unsigned long boot_mem_end = max_pfn << PAGE_SHIFT;
	unsigned long bz;

	/* If memory block size has been set, then use it */
	bz = set_memory_block_size;
	if (bz)
		goto done;

	/* Use regular block if RAM is smaller than MEM_SIZE_FOR_LARGE_BLOCK */
	if (boot_mem_end < MEM_SIZE_FOR_LARGE_BLOCK) {
		bz = MIN_MEMORY_BLOCK_SIZE;
		goto done;
	}

	/*
	 * Use max block size to minimize overhead on bare metal, where
	 * alignment for memory hotplug isn't a concern.
	 */
	if (!boot_cpu_has(X86_FEATURE_HYPERVISOR)) {
		bz = MAX_BLOCK_SIZE;
		goto done;
	}

	/* Find the largest allowed block size that aligns to memory end */
	for (bz = MAX_BLOCK_SIZE; bz > MIN_MEMORY_BLOCK_SIZE; bz >>= 1) {
		if (IS_ALIGNED(boot_mem_end, bz))
			break;
	}
done:
	pr_info("x86/mm: Memory block size: %ldMB\n", bz >> 20);

	return bz;
}

static unsigned long memory_block_size_probed;
unsigned long memory_block_size_bytes(void)
{
	if (!memory_block_size_probed)
		memory_block_size_probed = probe_memory_block_size();

	return memory_block_size_probed;
}

#ifdef CONFIG_SPARSEMEM_VMEMMAP
/*
 * Initialise the sparsemem vmemmap using huge-pages at the PMD level.
 */
static long __meminitdata addr_start, addr_end;
static void __meminitdata *p_start, *p_end;
static int __meminitdata node_start;

static int __meminit vmemmap_populate_hugepages(unsigned long start,
		unsigned long end, int node, struct vmem_altmap *altmap)
{
	unsigned long addr;
	unsigned long next;
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;

	for (addr = start; addr < end; addr = next) {
		next = pmd_addr_end(addr, end);

		pgd = vmemmap_pgd_populate(addr, node);
		if (!pgd)
			return -ENOMEM;

		p4d = vmemmap_p4d_populate(pgd, addr, node);
		if (!p4d)
			return -ENOMEM;

		pud = vmemmap_pud_populate(p4d, addr, node);
		if (!pud)
			return -ENOMEM;

		pmd = pmd_offset(pud, addr);
		if (pmd_none(*pmd)) {
			void *p;

			p = vmemmap_alloc_block_buf(PMD_SIZE, node, altmap);
			if (p) {
				pte_t entry;

				entry = pfn_pte(__pa(p) >> PAGE_SHIFT,
						PAGE_KERNEL_LARGE);
				set_pmd(pmd, __pmd(pte_val(entry)));

				/* check to see if we have contiguous blocks */
				if (p_end != p || node_start != node) {
					if (p_start)
						pr_debug(" [%lx-%lx] PMD -> [%p-%p] on node %d\n",
						       addr_start, addr_end-1, p_start, p_end-1, node_start);
					addr_start = addr;
					node_start = node;
					p_start = p;
				}

				addr_end = addr + PMD_SIZE;
				p_end = p + PMD_SIZE;

				if (!IS_ALIGNED(addr, PMD_SIZE) ||
				    !IS_ALIGNED(next, PMD_SIZE))
					vmemmap_use_new_sub_pmd(addr, next);

				continue;
			} else if (altmap)
				return -ENOMEM; /* no fallback */
		} else if (pmd_large(*pmd)) {
			vmemmap_verify((pte_t *)pmd, node, addr, next);
			vmemmap_use_sub_pmd(addr, next);
			continue;
		}
		if (vmemmap_populate_basepages(addr, next, node, NULL))
			return -ENOMEM;
	}
	return 0;
}

int __meminit vmemmap_populate(unsigned long start, unsigned long end, int node,
		struct vmem_altmap *altmap)
{
	int err;

	VM_BUG_ON(!IS_ALIGNED(start, PAGE_SIZE));
	VM_BUG_ON(!IS_ALIGNED(end, PAGE_SIZE));

	if (end - start < PAGES_PER_SECTION * sizeof(struct page))
		err = vmemmap_populate_basepages(start, end, node, NULL);
	else if (boot_cpu_has(X86_FEATURE_PSE))
		err = vmemmap_populate_hugepages(start, end, node, altmap);
	else if (altmap) {
		pr_err_once("%s: no cpu support for altmap allocations\n",
				__func__);
		err = -ENOMEM;
	} else
		err = vmemmap_populate_basepages(start, end, node, NULL);
	if (!err)
		sync_global_pgds(start, end - 1);
	return err;
}

#if defined(CONFIG_MEMORY_HOTPLUG_SPARSE) && defined(CONFIG_HAVE_BOOTMEM_INFO_NODE)
void register_page_bootmem_memmap(unsigned long section_nr,
				  struct page *start_page, unsigned long nr_pages)
{
	unsigned long addr = (unsigned long)start_page;
	unsigned long end = (unsigned long)(start_page + nr_pages);
	unsigned long next;
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	unsigned int nr_pmd_pages;
	struct page *page;

	for (; addr < end; addr = next) {
		pte_t *pte = NULL;

		pgd = pgd_offset_k(addr);
		if (pgd_none(*pgd)) {
			next = (addr + PAGE_SIZE) & PAGE_MASK;
			continue;
		}
		get_page_bootmem(section_nr, pgd_page(*pgd), MIX_SECTION_INFO);

		p4d = p4d_offset(pgd, addr);
		if (p4d_none(*p4d)) {
			next = (addr + PAGE_SIZE) & PAGE_MASK;
			continue;
		}
		get_page_bootmem(section_nr, p4d_page(*p4d), MIX_SECTION_INFO);

		pud = pud_offset(p4d, addr);
		if (pud_none(*pud)) {
			next = (addr + PAGE_SIZE) & PAGE_MASK;
			continue;
		}
		get_page_bootmem(section_nr, pud_page(*pud), MIX_SECTION_INFO);

		if (!boot_cpu_has(X86_FEATURE_PSE)) {
			next = (addr + PAGE_SIZE) & PAGE_MASK;
			pmd = pmd_offset(pud, addr);
			if (pmd_none(*pmd))
				continue;
			get_page_bootmem(section_nr, pmd_page(*pmd),
					 MIX_SECTION_INFO);

			pte = pte_offset_kernel(pmd, addr);
			if (pte_none(*pte))
				continue;
			get_page_bootmem(section_nr, pte_page(*pte),
					 SECTION_INFO);
		} else {
			next = pmd_addr_end(addr, end);

			pmd = pmd_offset(pud, addr);
			if (pmd_none(*pmd))
				continue;

			nr_pmd_pages = 1 << get_order(PMD_SIZE);
			page = pmd_page(*pmd);
			while (nr_pmd_pages--)
				get_page_bootmem(section_nr, page++,
						 SECTION_INFO);
		}
	}
}
#endif

void __meminit vmemmap_populate_print_last(void)
{
	if (p_start) {
		pr_debug(" [%lx-%lx] PMD -> [%p-%p] on node %d\n",
			addr_start, addr_end-1, p_start, p_end-1, node_start);
		p_start = NULL;
		p_end = NULL;
		node_start = 0;
	}
}
#endif
