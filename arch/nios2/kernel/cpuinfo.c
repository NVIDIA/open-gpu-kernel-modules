// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2013 Altera Corporation
 * Copyright (C) 2011 Tobias Klauser <tklauser@distanz.ch>
 *
 * Based on cpuinfo.c from microblaze
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/seq_file.h>
#include <linux/string.h>
#include <linux/of.h>
#include <asm/cpuinfo.h>

struct cpuinfo cpuinfo;

#define err_cpu(x) \
	pr_err("ERROR: Nios II " x " different for kernel and DTS\n")

static inline u32 fcpu(struct device_node *cpu, const char *n)
{
	u32 val = 0;

	of_property_read_u32(cpu, n, &val);

	return val;
}

void __init setup_cpuinfo(void)
{
	struct device_node *cpu;
	const char *str;
	int len;

	cpu = of_get_cpu_node(0, NULL);
	if (!cpu)
		panic("%s: No CPU found in devicetree!\n", __func__);

	if (!of_property_read_bool(cpu, "altr,has-initda"))
		panic("initda instruction is unimplemented. Please update your "
			"hardware system to have more than 4-byte line data "
			"cache\n");

	cpuinfo.cpu_clock_freq = fcpu(cpu, "clock-frequency");

	str = of_get_property(cpu, "altr,implementation", &len);
	if (str)
		strlcpy(cpuinfo.cpu_impl, str, sizeof(cpuinfo.cpu_impl));
	else
		strcpy(cpuinfo.cpu_impl, "<unknown>");

	cpuinfo.has_div = of_property_read_bool(cpu, "altr,has-div");
	cpuinfo.has_mul = of_property_read_bool(cpu, "altr,has-mul");
	cpuinfo.has_mulx = of_property_read_bool(cpu, "altr,has-mulx");
	cpuinfo.has_bmx = of_property_read_bool(cpu, "altr,has-bmx");
	cpuinfo.has_cdx = of_property_read_bool(cpu, "altr,has-cdx");
	cpuinfo.mmu = of_property_read_bool(cpu, "altr,has-mmu");

	if (IS_ENABLED(CONFIG_NIOS2_HW_DIV_SUPPORT) && !cpuinfo.has_div)
		err_cpu("DIV");

	if (IS_ENABLED(CONFIG_NIOS2_HW_MUL_SUPPORT) && !cpuinfo.has_mul)
		err_cpu("MUL");

	if (IS_ENABLED(CONFIG_NIOS2_HW_MULX_SUPPORT) && !cpuinfo.has_mulx)
		err_cpu("MULX");

	if (IS_ENABLED(CONFIG_NIOS2_BMX_SUPPORT) && !cpuinfo.has_bmx)
		err_cpu("BMX");

	if (IS_ENABLED(CONFIG_NIOS2_CDX_SUPPORT) && !cpuinfo.has_cdx)
		err_cpu("CDX");

	cpuinfo.tlb_num_ways = fcpu(cpu, "altr,tlb-num-ways");
	if (!cpuinfo.tlb_num_ways)
		panic("altr,tlb-num-ways can't be 0. Please check your hardware "
			"system\n");
	cpuinfo.icache_line_size = fcpu(cpu, "icache-line-size");
	cpuinfo.icache_size = fcpu(cpu, "icache-size");
	if (CONFIG_NIOS2_ICACHE_SIZE != cpuinfo.icache_size)
		pr_warn("Warning: icache size configuration mismatch "
		"(0x%x vs 0x%x) of CONFIG_NIOS2_ICACHE_SIZE vs "
		"device tree icache-size\n",
		CONFIG_NIOS2_ICACHE_SIZE, cpuinfo.icache_size);

	cpuinfo.dcache_line_size = fcpu(cpu, "dcache-line-size");
	if (CONFIG_NIOS2_DCACHE_LINE_SIZE != cpuinfo.dcache_line_size)
		pr_warn("Warning: dcache line size configuration mismatch "
		"(0x%x vs 0x%x) of CONFIG_NIOS2_DCACHE_LINE_SIZE vs "
		"device tree dcache-line-size\n",
		CONFIG_NIOS2_DCACHE_LINE_SIZE, cpuinfo.dcache_line_size);
	cpuinfo.dcache_size = fcpu(cpu, "dcache-size");
	if (CONFIG_NIOS2_DCACHE_SIZE != cpuinfo.dcache_size)
		pr_warn("Warning: dcache size configuration mismatch "
			"(0x%x vs 0x%x) of CONFIG_NIOS2_DCACHE_SIZE vs "
			"device tree dcache-size\n",
			CONFIG_NIOS2_DCACHE_SIZE, cpuinfo.dcache_size);

	cpuinfo.tlb_pid_num_bits = fcpu(cpu, "altr,pid-num-bits");
	cpuinfo.tlb_num_ways_log2 = ilog2(cpuinfo.tlb_num_ways);
	cpuinfo.tlb_num_entries = fcpu(cpu, "altr,tlb-num-entries");
	cpuinfo.tlb_num_lines = cpuinfo.tlb_num_entries / cpuinfo.tlb_num_ways;
	cpuinfo.tlb_ptr_sz = fcpu(cpu, "altr,tlb-ptr-sz");

	cpuinfo.reset_addr = fcpu(cpu, "altr,reset-addr");
	cpuinfo.exception_addr = fcpu(cpu, "altr,exception-addr");
	cpuinfo.fast_tlb_miss_exc_addr = fcpu(cpu, "altr,fast-tlb-miss-addr");

	of_node_put(cpu);
}

#ifdef CONFIG_PROC_FS

/*
 * Get CPU information for use by the procfs.
 */
static int show_cpuinfo(struct seq_file *m, void *v)
{
	const u32 clockfreq = cpuinfo.cpu_clock_freq;

	seq_printf(m,
		   "CPU:\t\tNios II/%s\n"
		   "REV:\t\t%i\n"
		   "MMU:\t\t%s\n"
		   "FPU:\t\tnone\n"
		   "Clocking:\t%u.%02u MHz\n"
		   "BogoMips:\t%lu.%02lu\n"
		   "Calibration:\t%lu loops\n",
		   cpuinfo.cpu_impl,
		   CONFIG_NIOS2_ARCH_REVISION,
		   cpuinfo.mmu ? "present" : "none",
		   clockfreq / 1000000, (clockfreq / 100000) % 10,
		   (loops_per_jiffy * HZ) / 500000,
		   ((loops_per_jiffy * HZ) / 5000) % 100,
		   (loops_per_jiffy * HZ));

	seq_printf(m,
		   "HW:\n"
		   " MUL:\t\t%s\n"
		   " MULX:\t\t%s\n"
		   " DIV:\t\t%s\n"
		   " BMX:\t\t%s\n"
		   " CDX:\t\t%s\n",
		   cpuinfo.has_mul ? "yes" : "no",
		   cpuinfo.has_mulx ? "yes" : "no",
		   cpuinfo.has_div ? "yes" : "no",
		   cpuinfo.has_bmx ? "yes" : "no",
		   cpuinfo.has_cdx ? "yes" : "no");

	seq_printf(m,
		   "Icache:\t\t%ukB, line length: %u\n",
		   cpuinfo.icache_size >> 10,
		   cpuinfo.icache_line_size);

	seq_printf(m,
		   "Dcache:\t\t%ukB, line length: %u\n",
		   cpuinfo.dcache_size >> 10,
		   cpuinfo.dcache_line_size);

	seq_printf(m,
		   "TLB:\t\t%u ways, %u entries, %u PID bits\n",
		   cpuinfo.tlb_num_ways,
		   cpuinfo.tlb_num_entries,
		   cpuinfo.tlb_pid_num_bits);

	return 0;
}

static void *cpuinfo_start(struct seq_file *m, loff_t *pos)
{
	unsigned long i = *pos;

	return i < num_possible_cpus() ? (void *) (i + 1) : NULL;
}

static void *cpuinfo_next(struct seq_file *m, void *v, loff_t *pos)
{
	++*pos;
	return cpuinfo_start(m, pos);
}

static void cpuinfo_stop(struct seq_file *m, void *v)
{
}

const struct seq_operations cpuinfo_op = {
	.start	= cpuinfo_start,
	.next	= cpuinfo_next,
	.stop	= cpuinfo_stop,
	.show	= show_cpuinfo
};

#endif /* CONFIG_PROC_FS */
