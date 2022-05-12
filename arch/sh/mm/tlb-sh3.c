// SPDX-License-Identifier: GPL-2.0-only
/*
 * arch/sh/mm/tlb-sh3.c
 *
 * SH-3 specific TLB operations
 *
 * Copyright (C) 1999  Niibe Yutaka
 * Copyright (C) 2002  Paul Mundt
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
#include <linux/smp.h>
#include <linux/interrupt.h>

#include <asm/io.h>
#include <linux/uaccess.h>
#include <asm/mmu_context.h>
#include <asm/cacheflush.h>

void __update_tlb(struct vm_area_struct *vma, unsigned long address, pte_t pte)
{
	unsigned long flags, pteval, vpn;

	/*
	 * Handle debugger faulting in for debugee.
	 */
	if (vma && current->active_mm != vma->vm_mm)
		return;

	local_irq_save(flags);

	/* Set PTEH register */
	vpn = (address & MMU_VPN_MASK) | get_asid();
	__raw_writel(vpn, MMU_PTEH);

	pteval = pte_val(pte);

	/* Set PTEL register */
	pteval &= _PAGE_FLAGS_HARDWARE_MASK; /* drop software flags */
	/* conveniently, we want all the software flags to be 0 anyway */
	__raw_writel(pteval, MMU_PTEL);

	/* Load the TLB */
	asm volatile("ldtlb": /* no output */ : /* no input */ : "memory");
	local_irq_restore(flags);
}

void local_flush_tlb_one(unsigned long asid, unsigned long page)
{
	unsigned long addr, data;
	int i, ways = MMU_NTLB_WAYS;

	/*
	 * NOTE: PTEH.ASID should be set to this MM
	 *       _AND_ we need to write ASID to the array.
	 *
	 * It would be simple if we didn't need to set PTEH.ASID...
	 */
	addr = MMU_TLB_ADDRESS_ARRAY | (page & 0x1F000);
	data = (page & 0xfffe0000) | asid; /* VALID bit is off */

	if ((current_cpu_data.flags & CPU_HAS_MMU_PAGE_ASSOC)) {
		addr |= MMU_PAGE_ASSOC_BIT;
		ways = 1;	/* we already know the way .. */
	}

	for (i = 0; i < ways; i++)
		__raw_writel(data, addr + (i << 8));
}

void local_flush_tlb_all(void)
{
	unsigned long flags, status;

	/*
	 * Flush all the TLB.
	 *
	 * Write to the MMU control register's bit:
	 *	TF-bit for SH-3, TI-bit for SH-4.
	 *      It's same position, bit #2.
	 */
	local_irq_save(flags);
	status = __raw_readl(MMUCR);
	status |= 0x04;
	__raw_writel(status, MMUCR);
	ctrl_barrier();
	local_irq_restore(flags);
}
