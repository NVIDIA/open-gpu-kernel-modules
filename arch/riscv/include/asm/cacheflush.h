/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2015 Regents of the University of California
 */

#ifndef _ASM_RISCV_CACHEFLUSH_H
#define _ASM_RISCV_CACHEFLUSH_H

#include <linux/mm.h>

static inline void local_flush_icache_all(void)
{
	asm volatile ("fence.i" ::: "memory");
}

#define PG_dcache_clean PG_arch_1

static inline void flush_dcache_page(struct page *page)
{
	if (test_bit(PG_dcache_clean, &page->flags))
		clear_bit(PG_dcache_clean, &page->flags);
}
#define ARCH_IMPLEMENTS_FLUSH_DCACHE_PAGE 1

/*
 * RISC-V doesn't have an instruction to flush parts of the instruction cache,
 * so instead we just flush the whole thing.
 */
#define flush_icache_range(start, end) flush_icache_all()
#define flush_icache_user_page(vma, pg, addr, len) \
	flush_icache_mm(vma->vm_mm, 0)

#ifndef CONFIG_SMP

#define flush_icache_all() local_flush_icache_all()
#define flush_icache_mm(mm, local) flush_icache_all()

#else /* CONFIG_SMP */

void flush_icache_all(void);
void flush_icache_mm(struct mm_struct *mm, bool local);

#endif /* CONFIG_SMP */

/*
 * Bits in sys_riscv_flush_icache()'s flags argument.
 */
#define SYS_RISCV_FLUSH_ICACHE_LOCAL 1UL
#define SYS_RISCV_FLUSH_ICACHE_ALL   (SYS_RISCV_FLUSH_ICACHE_LOCAL)

#include <asm-generic/cacheflush.h>

#endif /* _ASM_RISCV_CACHEFLUSH_H */
