// SPDX-License-Identifier: GPL-2.0
/*
 * arch/sh/kernel/cpu/sh3/probe.c
 *
 * CPU Subtype Probing for SH-3.
 *
 * Copyright (C) 1999, 2000  Niibe Yutaka
 * Copyright (C) 2002  Paul Mundt
 */

#include <linux/init.h>
#include <asm/processor.h>
#include <asm/cache.h>
#include <asm/io.h>

void cpu_probe(void)
{
	unsigned long addr0, addr1, data0, data1, data2, data3;

	jump_to_uncached();
	/*
	 * Check if the entry shadows or not.
	 * When shadowed, it's 128-entry system.
	 * Otherwise, it's 256-entry system.
	 */
	addr0 = CACHE_OC_ADDRESS_ARRAY + (3 << 12);
	addr1 = CACHE_OC_ADDRESS_ARRAY + (1 << 12);

	/* First, write back & invalidate */
	data0  = __raw_readl(addr0);
	__raw_writel(data0&~(SH_CACHE_VALID|SH_CACHE_UPDATED), addr0);
	data1  = __raw_readl(addr1);
	__raw_writel(data1&~(SH_CACHE_VALID|SH_CACHE_UPDATED), addr1);

	/* Next, check if there's shadow or not */
	data0 = __raw_readl(addr0);
	data0 ^= SH_CACHE_VALID;
	__raw_writel(data0, addr0);
	data1 = __raw_readl(addr1);
	data2 = data1 ^ SH_CACHE_VALID;
	__raw_writel(data2, addr1);
	data3 = __raw_readl(addr0);

	/* Lastly, invaliate them. */
	__raw_writel(data0&~SH_CACHE_VALID, addr0);
	__raw_writel(data2&~SH_CACHE_VALID, addr1);

	back_to_cached();

	boot_cpu_data.dcache.ways		= 4;
	boot_cpu_data.dcache.entry_shift	= 4;
	boot_cpu_data.dcache.linesz		= L1_CACHE_BYTES;
	boot_cpu_data.dcache.flags		= 0;

	/*
	 * 7709A/7729 has 16K cache (256-entry), while 7702 has only
	 * 2K(direct) 7702 is not supported (yet)
	 */
	if (data0 == data1 && data2 == data3) {	/* Shadow */
		boot_cpu_data.dcache.way_incr	= (1 << 11);
		boot_cpu_data.dcache.entry_mask	= 0x7f0;
		boot_cpu_data.dcache.sets	= 128;
		boot_cpu_data.type = CPU_SH7708;

		boot_cpu_data.flags |= CPU_HAS_MMU_PAGE_ASSOC;
	} else {				/* 7709A or 7729  */
		boot_cpu_data.dcache.way_incr	= (1 << 12);
		boot_cpu_data.dcache.entry_mask	= 0xff0;
		boot_cpu_data.dcache.sets	= 256;
		boot_cpu_data.type = CPU_SH7729;

#if defined(CONFIG_CPU_SUBTYPE_SH7706)
		boot_cpu_data.type = CPU_SH7706;
#endif
#if defined(CONFIG_CPU_SUBTYPE_SH7710)
		boot_cpu_data.type = CPU_SH7710;
#endif
#if defined(CONFIG_CPU_SUBTYPE_SH7712)
		boot_cpu_data.type = CPU_SH7712;
#endif
#if defined(CONFIG_CPU_SUBTYPE_SH7720)
		boot_cpu_data.type = CPU_SH7720;
#endif
#if defined(CONFIG_CPU_SUBTYPE_SH7721)
		boot_cpu_data.type = CPU_SH7721;
#endif
#if defined(CONFIG_CPU_SUBTYPE_SH7705)
		boot_cpu_data.type = CPU_SH7705;

#if defined(CONFIG_SH7705_CACHE_32KB)
		boot_cpu_data.dcache.way_incr	= (1 << 13);
		boot_cpu_data.dcache.entry_mask	= 0x1ff0;
		boot_cpu_data.dcache.sets	= 512;
		__raw_writel(CCR_CACHE_32KB, CCR3_REG);
#else
		__raw_writel(CCR_CACHE_16KB, CCR3_REG);
#endif
#endif
	}

	/*
	 * SH-3 doesn't have separate caches
	 */
	boot_cpu_data.dcache.flags |= SH_CACHE_COMBINED;
	boot_cpu_data.icache = boot_cpu_data.dcache;

	boot_cpu_data.family = CPU_FAMILY_SH3;
}
