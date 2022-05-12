// SPDX-License-Identifier: GPL-2.0
/*
 * Trapped io support
 *
 * Copyright (C) 2008 Magnus Damm
 *
 * Intercept io operations by trapping.
 */
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/bitops.h>
#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/mmu_context.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <asm/io_trapped.h>

#define TRAPPED_PAGES_MAX 16

#ifdef CONFIG_HAS_IOPORT_MAP
LIST_HEAD(trapped_io);
EXPORT_SYMBOL_GPL(trapped_io);
#endif
#ifdef CONFIG_HAS_IOMEM
LIST_HEAD(trapped_mem);
EXPORT_SYMBOL_GPL(trapped_mem);
#endif
static DEFINE_SPINLOCK(trapped_lock);

static int trapped_io_disable __read_mostly;

static int __init trapped_io_setup(char *__unused)
{
	trapped_io_disable = 1;
	return 1;
}
__setup("noiotrap", trapped_io_setup);

int register_trapped_io(struct trapped_io *tiop)
{
	struct resource *res;
	unsigned long len = 0, flags = 0;
	struct page *pages[TRAPPED_PAGES_MAX];
	int k, n;

	if (unlikely(trapped_io_disable))
		return 0;

	/* structure must be page aligned */
	if ((unsigned long)tiop & (PAGE_SIZE - 1))
		goto bad;

	for (k = 0; k < tiop->num_resources; k++) {
		res = tiop->resource + k;
		len += roundup(resource_size(res), PAGE_SIZE);
		flags |= res->flags;
	}

	/* support IORESOURCE_IO _or_ MEM, not both */
	if (hweight_long(flags) != 1)
		goto bad;

	n = len >> PAGE_SHIFT;

	if (n >= TRAPPED_PAGES_MAX)
		goto bad;

	for (k = 0; k < n; k++)
		pages[k] = virt_to_page(tiop);

	tiop->virt_base = vmap(pages, n, VM_MAP, PAGE_NONE);
	if (!tiop->virt_base)
		goto bad;

	len = 0;
	for (k = 0; k < tiop->num_resources; k++) {
		res = tiop->resource + k;
		pr_info("trapped io 0x%08lx overrides %s 0x%08lx\n",
		       (unsigned long)(tiop->virt_base + len),
		       res->flags & IORESOURCE_IO ? "io" : "mmio",
		       (unsigned long)res->start);
		len += roundup(resource_size(res), PAGE_SIZE);
	}

	tiop->magic = IO_TRAPPED_MAGIC;
	INIT_LIST_HEAD(&tiop->list);
	spin_lock_irq(&trapped_lock);
#ifdef CONFIG_HAS_IOPORT_MAP
	if (flags & IORESOURCE_IO)
		list_add(&tiop->list, &trapped_io);
#endif
#ifdef CONFIG_HAS_IOMEM
	if (flags & IORESOURCE_MEM)
		list_add(&tiop->list, &trapped_mem);
#endif
	spin_unlock_irq(&trapped_lock);

	return 0;
 bad:
	pr_warn("unable to install trapped io filter\n");
	return -1;
}

void __iomem *match_trapped_io_handler(struct list_head *list,
				       unsigned long offset,
				       unsigned long size)
{
	unsigned long voffs;
	struct trapped_io *tiop;
	struct resource *res;
	int k, len;
	unsigned long flags;

	spin_lock_irqsave(&trapped_lock, flags);
	list_for_each_entry(tiop, list, list) {
		voffs = 0;
		for (k = 0; k < tiop->num_resources; k++) {
			res = tiop->resource + k;
			if (res->start == offset) {
				spin_unlock_irqrestore(&trapped_lock, flags);
				return tiop->virt_base + voffs;
			}

			len = resource_size(res);
			voffs += roundup(len, PAGE_SIZE);
		}
	}
	spin_unlock_irqrestore(&trapped_lock, flags);
	return NULL;
}

static struct trapped_io *lookup_tiop(unsigned long address)
{
	pgd_t *pgd_k;
	p4d_t *p4d_k;
	pud_t *pud_k;
	pmd_t *pmd_k;
	pte_t *pte_k;
	pte_t entry;

	pgd_k = swapper_pg_dir + pgd_index(address);
	if (!pgd_present(*pgd_k))
		return NULL;

	p4d_k = p4d_offset(pgd_k, address);
	if (!p4d_present(*p4d_k))
		return NULL;

	pud_k = pud_offset(p4d_k, address);
	if (!pud_present(*pud_k))
		return NULL;

	pmd_k = pmd_offset(pud_k, address);
	if (!pmd_present(*pmd_k))
		return NULL;

	pte_k = pte_offset_kernel(pmd_k, address);
	entry = *pte_k;

	return pfn_to_kaddr(pte_pfn(entry));
}

static unsigned long lookup_address(struct trapped_io *tiop,
				    unsigned long address)
{
	struct resource *res;
	unsigned long vaddr = (unsigned long)tiop->virt_base;
	unsigned long len;
	int k;

	for (k = 0; k < tiop->num_resources; k++) {
		res = tiop->resource + k;
		len = roundup(resource_size(res), PAGE_SIZE);
		if (address < (vaddr + len))
			return res->start + (address - vaddr);
		vaddr += len;
	}
	return 0;
}

static unsigned long long copy_word(unsigned long src_addr, int src_len,
				    unsigned long dst_addr, int dst_len)
{
	unsigned long long tmp = 0;

	switch (src_len) {
	case 1:
		tmp = __raw_readb(src_addr);
		break;
	case 2:
		tmp = __raw_readw(src_addr);
		break;
	case 4:
		tmp = __raw_readl(src_addr);
		break;
	case 8:
		tmp = __raw_readq(src_addr);
		break;
	}

	switch (dst_len) {
	case 1:
		__raw_writeb(tmp, dst_addr);
		break;
	case 2:
		__raw_writew(tmp, dst_addr);
		break;
	case 4:
		__raw_writel(tmp, dst_addr);
		break;
	case 8:
		__raw_writeq(tmp, dst_addr);
		break;
	}

	return tmp;
}

static unsigned long from_device(void *dst, const void *src, unsigned long cnt)
{
	struct trapped_io *tiop;
	unsigned long src_addr = (unsigned long)src;
	unsigned long long tmp;

	pr_debug("trapped io read 0x%08lx (%ld)\n", src_addr, cnt);
	tiop = lookup_tiop(src_addr);
	WARN_ON(!tiop || (tiop->magic != IO_TRAPPED_MAGIC));

	src_addr = lookup_address(tiop, src_addr);
	if (!src_addr)
		return cnt;

	tmp = copy_word(src_addr,
			max_t(unsigned long, cnt,
			      (tiop->minimum_bus_width / 8)),
			(unsigned long)dst, cnt);

	pr_debug("trapped io read 0x%08lx -> 0x%08llx\n", src_addr, tmp);
	return 0;
}

static unsigned long to_device(void *dst, const void *src, unsigned long cnt)
{
	struct trapped_io *tiop;
	unsigned long dst_addr = (unsigned long)dst;
	unsigned long long tmp;

	pr_debug("trapped io write 0x%08lx (%ld)\n", dst_addr, cnt);
	tiop = lookup_tiop(dst_addr);
	WARN_ON(!tiop || (tiop->magic != IO_TRAPPED_MAGIC));

	dst_addr = lookup_address(tiop, dst_addr);
	if (!dst_addr)
		return cnt;

	tmp = copy_word((unsigned long)src, cnt,
			dst_addr, max_t(unsigned long, cnt,
					(tiop->minimum_bus_width / 8)));

	pr_debug("trapped io write 0x%08lx -> 0x%08llx\n", dst_addr, tmp);
	return 0;
}

static struct mem_access trapped_io_access = {
	from_device,
	to_device,
};

int handle_trapped_io(struct pt_regs *regs, unsigned long address)
{
	mm_segment_t oldfs;
	insn_size_t instruction;
	int tmp;

	if (trapped_io_disable)
		return 0;
	if (!lookup_tiop(address))
		return 0;

	WARN_ON(user_mode(regs));

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	if (copy_from_user(&instruction, (void *)(regs->pc),
			   sizeof(instruction))) {
		set_fs(oldfs);
		return 0;
	}

	tmp = handle_unaligned_access(instruction, regs,
				      &trapped_io_access, 1, address);
	set_fs(oldfs);
	return tmp == 0;
}
