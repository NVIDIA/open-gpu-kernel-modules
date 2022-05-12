// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2018 Hangzhou C-SKY Microsystems co.,ltd.

#include <linux/console.h>
#include <linux/memblock.h>
#include <linux/initrd.h>
#include <linux/of.h>
#include <linux/of_fdt.h>
#include <linux/start_kernel.h>
#include <linux/dma-map-ops.h>
#include <linux/screen_info.h>
#include <asm/sections.h>
#include <asm/mmu_context.h>
#include <asm/pgalloc.h>

#ifdef CONFIG_DUMMY_CONSOLE
struct screen_info screen_info = {
	.orig_video_lines	= 30,
	.orig_video_cols	= 80,
	.orig_video_mode	= 0,
	.orig_video_ega_bx	= 0,
	.orig_video_isVGA	= 1,
	.orig_video_points	= 8
};
#endif

static void __init csky_memblock_init(void)
{
	unsigned long lowmem_size = PFN_DOWN(LOWMEM_LIMIT - PHYS_OFFSET_OFFSET);
	unsigned long sseg_size = PFN_DOWN(SSEG_SIZE - PHYS_OFFSET_OFFSET);
	unsigned long max_zone_pfn[MAX_NR_ZONES] = { 0 };
	signed long size;

	memblock_reserve(__pa(_stext), _end - _stext);

	early_init_fdt_reserve_self();
	early_init_fdt_scan_reserved_mem();

	memblock_dump_all();

	min_low_pfn = PFN_UP(memblock_start_of_DRAM());
	max_low_pfn = max_pfn = PFN_DOWN(memblock_end_of_DRAM());

	size = max_pfn - min_low_pfn;

	if (size >= lowmem_size) {
		max_low_pfn = min_low_pfn + lowmem_size;
#ifdef CONFIG_PAGE_OFFSET_80000000
		write_mmu_msa1(read_mmu_msa0() + SSEG_SIZE);
#endif
	} else if (size > sseg_size) {
		max_low_pfn = min_low_pfn + sseg_size;
	}

	max_zone_pfn[ZONE_NORMAL] = max_low_pfn;

	mmu_init(min_low_pfn, max_low_pfn);

#ifdef CONFIG_HIGHMEM
	max_zone_pfn[ZONE_HIGHMEM] = max_pfn;

	highstart_pfn = max_low_pfn;
	highend_pfn   = max_pfn;
#endif
	memblock_set_current_limit(PFN_PHYS(max_low_pfn));

	dma_contiguous_reserve(0);

	free_area_init(max_zone_pfn);
}

void __init setup_arch(char **cmdline_p)
{
	*cmdline_p = boot_command_line;

	console_verbose();

	pr_info("Phys. mem: %ldMB\n",
		(unsigned long) memblock_phys_mem_size()/1024/1024);

	init_mm.start_code = (unsigned long) _stext;
	init_mm.end_code = (unsigned long) _etext;
	init_mm.end_data = (unsigned long) _edata;
	init_mm.brk = (unsigned long) _end;

	parse_early_param();

	csky_memblock_init();

	unflatten_and_copy_device_tree();

#ifdef CONFIG_SMP
	setup_smp();
#endif

	sparse_init();

	fixaddr_init();

#ifdef CONFIG_HIGHMEM
	kmap_init();
#endif
}

unsigned long va_pa_offset;
EXPORT_SYMBOL(va_pa_offset);

static inline unsigned long read_mmu_msa(void)
{
#ifdef CONFIG_PAGE_OFFSET_80000000
	return read_mmu_msa0();
#endif

#ifdef CONFIG_PAGE_OFFSET_A0000000
	return read_mmu_msa1();
#endif
}

asmlinkage __visible void __init csky_start(unsigned int unused,
					    void *dtb_start)
{
	/* Clean up bss section */
	memset(__bss_start, 0, __bss_stop - __bss_start);

	va_pa_offset = read_mmu_msa() & ~(SSEG_SIZE - 1);

	pre_trap_init();

	if (dtb_start == NULL)
		early_init_dt_scan(__dtb_start);
	else
		early_init_dt_scan(dtb_start);

	start_kernel();

	asm volatile("br .\n");
}
