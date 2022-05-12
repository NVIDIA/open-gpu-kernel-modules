// SPDX-License-Identifier: GPL-2.0
/*
** PARISC 1.1 Dynamic DMA mapping support.
** This implementation is for PA-RISC platforms that do not support
** I/O TLBs (aka DMA address translation hardware).
** See Documentation/core-api/dma-api-howto.rst for interface definitions.
**
**      (c) Copyright 1999,2000 Hewlett-Packard Company
**      (c) Copyright 2000 Grant Grundler
**	(c) Copyright 2000 Philipp Rumpf <prumpf@tux.org>
**      (c) Copyright 2000 John Marvin
**
** "leveraged" from 2.3.47: arch/ia64/kernel/pci-dma.c.
** (I assume it's from David Mosberger-Tang but there was no Copyright)
**
** AFAIK, all PA7100LC and PA7300LC platforms can use this code.
**
** - ggg
*/

#include <linux/init.h>
#include <linux/gfp.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/dma-direct.h>
#include <linux/dma-map-ops.h>

#include <asm/cacheflush.h>
#include <asm/dma.h>    /* for DMA_CHUNK_SIZE */
#include <asm/io.h>
#include <asm/page.h>	/* get_order */
#include <linux/uaccess.h>
#include <asm/tlbflush.h>	/* for purge_tlb_*() macros */

static struct proc_dir_entry * proc_gsc_root __read_mostly = NULL;
static unsigned long pcxl_used_bytes __read_mostly = 0;
static unsigned long pcxl_used_pages __read_mostly = 0;

extern unsigned long pcxl_dma_start; /* Start of pcxl dma mapping area */
static DEFINE_SPINLOCK(pcxl_res_lock);
static char    *pcxl_res_map;
static int     pcxl_res_hint;
static int     pcxl_res_size;

#ifdef DEBUG_PCXL_RESOURCE
#define DBG_RES(x...)	printk(x)
#else
#define DBG_RES(x...)
#endif


/*
** Dump a hex representation of the resource map.
*/

#ifdef DUMP_RESMAP
static
void dump_resmap(void)
{
	u_long *res_ptr = (unsigned long *)pcxl_res_map;
	u_long i = 0;

	printk("res_map: ");
	for(; i < (pcxl_res_size / sizeof(unsigned long)); ++i, ++res_ptr)
		printk("%08lx ", *res_ptr);

	printk("\n");
}
#else
static inline void dump_resmap(void) {;}
#endif

static inline int map_pte_uncached(pte_t * pte,
		unsigned long vaddr,
		unsigned long size, unsigned long *paddr_ptr)
{
	unsigned long end;
	unsigned long orig_vaddr = vaddr;

	vaddr &= ~PMD_MASK;
	end = vaddr + size;
	if (end > PMD_SIZE)
		end = PMD_SIZE;
	do {
		unsigned long flags;

		if (!pte_none(*pte))
			printk(KERN_ERR "map_pte_uncached: page already exists\n");
		purge_tlb_start(flags);
		set_pte(pte, __mk_pte(*paddr_ptr, PAGE_KERNEL_UNC));
		pdtlb_kernel(orig_vaddr);
		purge_tlb_end(flags);
		vaddr += PAGE_SIZE;
		orig_vaddr += PAGE_SIZE;
		(*paddr_ptr) += PAGE_SIZE;
		pte++;
	} while (vaddr < end);
	return 0;
}

static inline int map_pmd_uncached(pmd_t * pmd, unsigned long vaddr,
		unsigned long size, unsigned long *paddr_ptr)
{
	unsigned long end;
	unsigned long orig_vaddr = vaddr;

	vaddr &= ~PGDIR_MASK;
	end = vaddr + size;
	if (end > PGDIR_SIZE)
		end = PGDIR_SIZE;
	do {
		pte_t * pte = pte_alloc_kernel(pmd, vaddr);
		if (!pte)
			return -ENOMEM;
		if (map_pte_uncached(pte, orig_vaddr, end - vaddr, paddr_ptr))
			return -ENOMEM;
		vaddr = (vaddr + PMD_SIZE) & PMD_MASK;
		orig_vaddr += PMD_SIZE;
		pmd++;
	} while (vaddr < end);
	return 0;
}

static inline int map_uncached_pages(unsigned long vaddr, unsigned long size,
		unsigned long paddr)
{
	pgd_t * dir;
	unsigned long end = vaddr + size;

	dir = pgd_offset_k(vaddr);
	do {
		p4d_t *p4d;
		pud_t *pud;
		pmd_t *pmd;

		p4d = p4d_offset(dir, vaddr);
		pud = pud_offset(p4d, vaddr);
		pmd = pmd_alloc(NULL, pud, vaddr);

		if (!pmd)
			return -ENOMEM;
		if (map_pmd_uncached(pmd, vaddr, end - vaddr, &paddr))
			return -ENOMEM;
		vaddr = vaddr + PGDIR_SIZE;
		dir++;
	} while (vaddr && (vaddr < end));
	return 0;
}

static inline void unmap_uncached_pte(pmd_t * pmd, unsigned long vaddr,
		unsigned long size)
{
	pte_t * pte;
	unsigned long end;
	unsigned long orig_vaddr = vaddr;

	if (pmd_none(*pmd))
		return;
	if (pmd_bad(*pmd)) {
		pmd_ERROR(*pmd);
		pmd_clear(pmd);
		return;
	}
	pte = pte_offset_map(pmd, vaddr);
	vaddr &= ~PMD_MASK;
	end = vaddr + size;
	if (end > PMD_SIZE)
		end = PMD_SIZE;
	do {
		unsigned long flags;
		pte_t page = *pte;

		pte_clear(&init_mm, vaddr, pte);
		purge_tlb_start(flags);
		pdtlb_kernel(orig_vaddr);
		purge_tlb_end(flags);
		vaddr += PAGE_SIZE;
		orig_vaddr += PAGE_SIZE;
		pte++;
		if (pte_none(page) || pte_present(page))
			continue;
		printk(KERN_CRIT "Whee.. Swapped out page in kernel page table\n");
	} while (vaddr < end);
}

static inline void unmap_uncached_pmd(pgd_t * dir, unsigned long vaddr,
		unsigned long size)
{
	pmd_t * pmd;
	unsigned long end;
	unsigned long orig_vaddr = vaddr;

	if (pgd_none(*dir))
		return;
	if (pgd_bad(*dir)) {
		pgd_ERROR(*dir);
		pgd_clear(dir);
		return;
	}
	pmd = pmd_offset(pud_offset(p4d_offset(dir, vaddr), vaddr), vaddr);
	vaddr &= ~PGDIR_MASK;
	end = vaddr + size;
	if (end > PGDIR_SIZE)
		end = PGDIR_SIZE;
	do {
		unmap_uncached_pte(pmd, orig_vaddr, end - vaddr);
		vaddr = (vaddr + PMD_SIZE) & PMD_MASK;
		orig_vaddr += PMD_SIZE;
		pmd++;
	} while (vaddr < end);
}

static void unmap_uncached_pages(unsigned long vaddr, unsigned long size)
{
	pgd_t * dir;
	unsigned long end = vaddr + size;

	dir = pgd_offset_k(vaddr);
	do {
		unmap_uncached_pmd(dir, vaddr, end - vaddr);
		vaddr = vaddr + PGDIR_SIZE;
		dir++;
	} while (vaddr && (vaddr < end));
}

#define PCXL_SEARCH_LOOP(idx, mask, size)  \
       for(; res_ptr < res_end; ++res_ptr) \
       { \
               if(0 == ((*res_ptr) & mask)) { \
                       *res_ptr |= mask; \
		       idx = (int)((u_long)res_ptr - (u_long)pcxl_res_map); \
		       pcxl_res_hint = idx + (size >> 3); \
                       goto resource_found; \
               } \
       }

#define PCXL_FIND_FREE_MAPPING(idx, mask, size)  { \
       u##size *res_ptr = (u##size *)&(pcxl_res_map[pcxl_res_hint & ~((size >> 3) - 1)]); \
       u##size *res_end = (u##size *)&pcxl_res_map[pcxl_res_size]; \
       PCXL_SEARCH_LOOP(idx, mask, size); \
       res_ptr = (u##size *)&pcxl_res_map[0]; \
       PCXL_SEARCH_LOOP(idx, mask, size); \
}

unsigned long
pcxl_alloc_range(size_t size)
{
	int res_idx;
	u_long mask, flags;
	unsigned int pages_needed = size >> PAGE_SHIFT;

	mask = (u_long) -1L;
 	mask >>= BITS_PER_LONG - pages_needed;

	DBG_RES("pcxl_alloc_range() size: %d pages_needed %d pages_mask 0x%08lx\n", 
		size, pages_needed, mask);

	spin_lock_irqsave(&pcxl_res_lock, flags);

	if(pages_needed <= 8) {
		PCXL_FIND_FREE_MAPPING(res_idx, mask, 8);
	} else if(pages_needed <= 16) {
		PCXL_FIND_FREE_MAPPING(res_idx, mask, 16);
	} else if(pages_needed <= 32) {
		PCXL_FIND_FREE_MAPPING(res_idx, mask, 32);
	} else {
		panic("%s: pcxl_alloc_range() Too many pages to map.\n",
		      __FILE__);
	}

	dump_resmap();
	panic("%s: pcxl_alloc_range() out of dma mapping resources\n",
	      __FILE__);
	
resource_found:
	
	DBG_RES("pcxl_alloc_range() res_idx %d mask 0x%08lx res_hint: %d\n",
		res_idx, mask, pcxl_res_hint);

	pcxl_used_pages += pages_needed;
	pcxl_used_bytes += ((pages_needed >> 3) ? (pages_needed >> 3) : 1);

	spin_unlock_irqrestore(&pcxl_res_lock, flags);

	dump_resmap();

	/* 
	** return the corresponding vaddr in the pcxl dma map
	*/
	return (pcxl_dma_start + (res_idx << (PAGE_SHIFT + 3)));
}

#define PCXL_FREE_MAPPINGS(idx, m, size) \
		u##size *res_ptr = (u##size *)&(pcxl_res_map[(idx) + (((size >> 3) - 1) & (~((size >> 3) - 1)))]); \
		/* BUG_ON((*res_ptr & m) != m); */ \
		*res_ptr &= ~m;

/*
** clear bits in the pcxl resource map
*/
static void
pcxl_free_range(unsigned long vaddr, size_t size)
{
	u_long mask, flags;
	unsigned int res_idx = (vaddr - pcxl_dma_start) >> (PAGE_SHIFT + 3);
	unsigned int pages_mapped = size >> PAGE_SHIFT;

	mask = (u_long) -1L;
 	mask >>= BITS_PER_LONG - pages_mapped;

	DBG_RES("pcxl_free_range() res_idx: %d size: %d pages_mapped %d mask 0x%08lx\n", 
		res_idx, size, pages_mapped, mask);

	spin_lock_irqsave(&pcxl_res_lock, flags);

	if(pages_mapped <= 8) {
		PCXL_FREE_MAPPINGS(res_idx, mask, 8);
	} else if(pages_mapped <= 16) {
		PCXL_FREE_MAPPINGS(res_idx, mask, 16);
	} else if(pages_mapped <= 32) {
		PCXL_FREE_MAPPINGS(res_idx, mask, 32);
	} else {
		panic("%s: pcxl_free_range() Too many pages to unmap.\n",
		      __FILE__);
	}
	
	pcxl_used_pages -= (pages_mapped ? pages_mapped : 1);
	pcxl_used_bytes -= ((pages_mapped >> 3) ? (pages_mapped >> 3) : 1);

	spin_unlock_irqrestore(&pcxl_res_lock, flags);

	dump_resmap();
}

static int __maybe_unused proc_pcxl_dma_show(struct seq_file *m, void *v)
{
#if 0
	u_long i = 0;
	unsigned long *res_ptr = (u_long *)pcxl_res_map;
#endif
	unsigned long total_pages = pcxl_res_size << 3;   /* 8 bits per byte */

	seq_printf(m, "\nDMA Mapping Area size    : %d bytes (%ld pages)\n",
		PCXL_DMA_MAP_SIZE, total_pages);

	seq_printf(m, "Resource bitmap : %d bytes\n", pcxl_res_size);

	seq_puts(m,  "     	  total:    free:    used:   % used:\n");
	seq_printf(m, "blocks  %8d %8ld %8ld %8ld%%\n", pcxl_res_size,
		pcxl_res_size - pcxl_used_bytes, pcxl_used_bytes,
		(pcxl_used_bytes * 100) / pcxl_res_size);

	seq_printf(m, "pages   %8ld %8ld %8ld %8ld%%\n", total_pages,
		total_pages - pcxl_used_pages, pcxl_used_pages,
		(pcxl_used_pages * 100 / total_pages));

#if 0
	seq_puts(m, "\nResource bitmap:");

	for(; i < (pcxl_res_size / sizeof(u_long)); ++i, ++res_ptr) {
		if ((i & 7) == 0)
		    seq_puts(m,"\n   ");
		seq_printf(m, "%s %08lx", buf, *res_ptr);
	}
#endif
	seq_putc(m, '\n');
	return 0;
}

static int __init
pcxl_dma_init(void)
{
	if (pcxl_dma_start == 0)
		return 0;

	pcxl_res_size = PCXL_DMA_MAP_SIZE >> (PAGE_SHIFT + 3);
	pcxl_res_hint = 0;
	pcxl_res_map = (char *)__get_free_pages(GFP_KERNEL,
					    get_order(pcxl_res_size));
	memset(pcxl_res_map, 0, pcxl_res_size);
	proc_gsc_root = proc_mkdir("gsc", NULL);
	if (!proc_gsc_root)
    		printk(KERN_WARNING
			"pcxl_dma_init: Unable to create gsc /proc dir entry\n");
	else {
		struct proc_dir_entry* ent;
		ent = proc_create_single("pcxl_dma", 0, proc_gsc_root,
				proc_pcxl_dma_show);
		if (!ent)
			printk(KERN_WARNING
				"pci-dma.c: Unable to create pcxl_dma /proc entry.\n");
	}
	return 0;
}

__initcall(pcxl_dma_init);

void *arch_dma_alloc(struct device *dev, size_t size,
		dma_addr_t *dma_handle, gfp_t gfp, unsigned long attrs)
{
	unsigned long vaddr;
	unsigned long paddr;
	int order;

	if (boot_cpu_data.cpu_type != pcxl2 && boot_cpu_data.cpu_type != pcxl)
		return NULL;

	order = get_order(size);
	size = 1 << (order + PAGE_SHIFT);
	vaddr = pcxl_alloc_range(size);
	paddr = __get_free_pages(gfp | __GFP_ZERO, order);
	flush_kernel_dcache_range(paddr, size);
	paddr = __pa(paddr);
	map_uncached_pages(vaddr, size, paddr);
	*dma_handle = (dma_addr_t) paddr;

#if 0
/* This probably isn't needed to support EISA cards.
** ISA cards will certainly only support 24-bit DMA addressing.
** Not clear if we can, want, or need to support ISA.
*/
	if (!dev || *dev->coherent_dma_mask < 0xffffffff)
		gfp |= GFP_DMA;
#endif
	return (void *)vaddr;
}

void arch_dma_free(struct device *dev, size_t size, void *vaddr,
		dma_addr_t dma_handle, unsigned long attrs)
{
	int order = get_order(size);

	WARN_ON_ONCE(boot_cpu_data.cpu_type != pcxl2 &&
		     boot_cpu_data.cpu_type != pcxl);

	size = 1 << (order + PAGE_SHIFT);
	unmap_uncached_pages((unsigned long)vaddr, size);
	pcxl_free_range((unsigned long)vaddr, size);

	free_pages((unsigned long)__va(dma_handle), order);
}

void arch_sync_dma_for_device(phys_addr_t paddr, size_t size,
		enum dma_data_direction dir)
{
	flush_kernel_dcache_range((unsigned long)phys_to_virt(paddr), size);
}

void arch_sync_dma_for_cpu(phys_addr_t paddr, size_t size,
		enum dma_data_direction dir)
{
	flush_kernel_dcache_range((unsigned long)phys_to_virt(paddr), size);
}
