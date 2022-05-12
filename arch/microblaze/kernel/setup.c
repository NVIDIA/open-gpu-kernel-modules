/*
 * Copyright (C) 2007-2009 Michal Simek <monstr@monstr.eu>
 * Copyright (C) 2007-2009 PetaLogix
 * Copyright (C) 2006 Atmark Techno, Inc.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <linux/init.h>
#include <linux/of_clk.h>
#include <linux/clocksource.h>
#include <linux/string.h>
#include <linux/seq_file.h>
#include <linux/cpu.h>
#include <linux/initrd.h>
#include <linux/console.h>
#include <linux/debugfs.h>
#include <linux/of_fdt.h>
#include <linux/pgtable.h>

#include <asm/setup.h>
#include <asm/sections.h>
#include <asm/page.h>
#include <linux/io.h>
#include <linux/bug.h>
#include <linux/param.h>
#include <linux/pci.h>
#include <linux/cache.h>
#include <linux/of.h>
#include <linux/dma-mapping.h>
#include <asm/cacheflush.h>
#include <asm/entry.h>
#include <asm/cpuinfo.h>


DEFINE_PER_CPU(unsigned int, KSP);	/* Saved kernel stack pointer */
DEFINE_PER_CPU(unsigned int, KM);	/* Kernel/user mode */
DEFINE_PER_CPU(unsigned int, ENTRY_SP);	/* Saved SP on kernel entry */
DEFINE_PER_CPU(unsigned int, R11_SAVE);	/* Temp variable for entry */
DEFINE_PER_CPU(unsigned int, CURRENT_SAVE);	/* Saved current pointer */

/*
 * Placed cmd_line to .data section because can be initialized from
 * ASM code. Default position is BSS section which is cleared
 * in machine_early_init().
 */
char cmd_line[COMMAND_LINE_SIZE] __section(".data");

void __init setup_arch(char **cmdline_p)
{
	*cmdline_p = boot_command_line;

	setup_memory();

	console_verbose();

	unflatten_device_tree();

	setup_cpuinfo();

	microblaze_cache_init();

	xilinx_pci_init();
}

#ifdef CONFIG_MTD_UCLINUX
/* Handle both romfs and cramfs types, without generating unnecessary
 code (ie no point checking for CRAMFS if it's not even enabled) */
inline unsigned get_romfs_len(unsigned *addr)
{
#ifdef CONFIG_ROMFS_FS
	if (memcmp(&addr[0], "-rom1fs-", 8) == 0) /* romfs */
		return be32_to_cpu(addr[2]);
#endif

#ifdef CONFIG_CRAMFS
	if (addr[0] == le32_to_cpu(0x28cd3d45)) /* cramfs */
		return le32_to_cpu(addr[1]);
#endif
	return 0;
}
#endif	/* CONFIG_MTD_UCLINUX_EBSS */

unsigned long kernel_tlb;

void __init machine_early_init(const char *cmdline, unsigned int ram,
		unsigned int fdt, unsigned int msr, unsigned int tlb0,
		unsigned int tlb1)
{
	unsigned long *src, *dst;
	unsigned int offset = 0;

	/* If CONFIG_MTD_UCLINUX is defined, assume ROMFS is at the
	 * end of kernel. There are two position which we want to check.
	 * The first is __init_end and the second __bss_start.
	 */
#ifdef CONFIG_MTD_UCLINUX
	int romfs_size;
	unsigned int romfs_base;
	char *old_klimit = klimit;

	romfs_base = (ram ? ram : (unsigned int)&__init_end);
	romfs_size = PAGE_ALIGN(get_romfs_len((unsigned *)romfs_base));
	if (!romfs_size) {
		romfs_base = (unsigned int)&__bss_start;
		romfs_size = PAGE_ALIGN(get_romfs_len((unsigned *)romfs_base));
	}

	/* Move ROMFS out of BSS before clearing it */
	if (romfs_size > 0) {
		memmove(&__bss_stop, (int *)romfs_base, romfs_size);
		klimit += romfs_size;
	}
#endif

/* clearing bss section */
	memset(__bss_start, 0, __bss_stop-__bss_start);
	memset(_ssbss, 0, _esbss-_ssbss);

/* initialize device tree for usage in early_printk */
	early_init_devtree(_fdt_start);

	/* setup kernel_tlb after BSS cleaning
	 * Maybe worth to move to asm code */
	kernel_tlb = tlb0 + tlb1;
	/* printk("TLB1 0x%08x, TLB0 0x%08x, tlb 0x%x\n", tlb0,
							tlb1, kernel_tlb); */

	pr_info("Ramdisk addr 0x%08x, ", ram);
	if (fdt)
		pr_info("FDT at 0x%08x\n", fdt);
	else
		pr_info("Compiled-in FDT at %p\n", _fdt_start);

#ifdef CONFIG_MTD_UCLINUX
	pr_info("Found romfs @ 0x%08x (0x%08x)\n",
			romfs_base, romfs_size);
	pr_info("#### klimit %p ####\n", old_klimit);
	BUG_ON(romfs_size < 0); /* What else can we do? */

	pr_info("Moved 0x%08x bytes from 0x%08x to 0x%08x\n",
			romfs_size, romfs_base, (unsigned)&__bss_stop);

	pr_info("New klimit: 0x%08x\n", (unsigned)klimit);
#endif

#if CONFIG_XILINX_MICROBLAZE0_USE_MSR_INSTR
	if (msr) {
		pr_info("!!!Your kernel has setup MSR instruction but ");
		pr_cont("CPU don't have it %x\n", msr);
	}
#else
	if (!msr) {
		pr_info("!!!Your kernel not setup MSR instruction but ");
		pr_cont("CPU have it %x\n", msr);
	}
#endif

	/* Do not copy reset vectors. offset = 0x2 means skip the first
	 * two instructions. dst is pointer to MB vectors which are placed
	 * in block ram. If you want to copy reset vector setup offset to 0x0 */
#if !CONFIG_MANUAL_RESET_VECTOR
	offset = 0x2;
#endif
	dst = (unsigned long *) (offset * sizeof(u32));
	for (src = __ivt_start + offset; src < __ivt_end; src++, dst++)
		*dst = *src;

	/* Initialize global data */
	per_cpu(KM, 0) = 0x1;	/* We start in kernel mode */
	per_cpu(CURRENT_SAVE, 0) = (unsigned long)current;
}

void __init time_init(void)
{
	of_clk_init(NULL);
	setup_cpuinfo_clk();
	timer_probe();
}

#ifdef CONFIG_DEBUG_FS
struct dentry *of_debugfs_root;

static int microblaze_debugfs_init(void)
{
	of_debugfs_root = debugfs_create_dir("microblaze", NULL);
	return 0;
}
arch_initcall(microblaze_debugfs_init);

static int __init debugfs_tlb(void)
{
	debugfs_create_u32("tlb_skip", S_IRUGO, of_debugfs_root, &tlb_skip);
	return 0;
}
device_initcall(debugfs_tlb);
#endif
