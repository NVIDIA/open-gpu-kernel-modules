// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2016 Imagination Technologies
 * Author: Paul Burton <paul.burton@mips.com>
 */

#include <linux/clk.h>
#include <linux/clocksource.h>
#include <linux/init.h>
#include <linux/irqchip.h>
#include <linux/of_clk.h>
#include <linux/of_fdt.h>

#include <asm/bootinfo.h>
#include <asm/fw/fw.h>
#include <asm/irq_cpu.h>
#include <asm/machine.h>
#include <asm/mips-cps.h>
#include <asm/prom.h>
#include <asm/smp-ops.h>
#include <asm/time.h>

static __initconst const void *fdt;
static __initconst const struct mips_machine *mach;
static __initconst const void *mach_match_data;

void __init prom_init(void)
{
	plat_get_fdt();
	BUG_ON(!fdt);
}

void __init *plat_get_fdt(void)
{
	const struct mips_machine *check_mach;
	const struct of_device_id *match;

	if (fdt)
		/* Already set up */
		return (void *)fdt;

	fdt = (void *)get_fdt();
	if (fdt && !fdt_check_header(fdt)) {
		/*
		 * We have been provided with the appropriate device tree for
		 * the board. Make use of it & search for any machine struct
		 * based upon the root compatible string.
		 */
		for_each_mips_machine(check_mach) {
			match = mips_machine_is_compatible(check_mach, fdt);
			if (match) {
				mach = check_mach;
				mach_match_data = match->data;
				break;
			}
		}
	} else if (IS_ENABLED(CONFIG_LEGACY_BOARDS)) {
		/*
		 * We weren't booted using the UHI boot protocol, but do
		 * support some number of boards with legacy boot protocols.
		 * Attempt to find the right one.
		 */
		for_each_mips_machine(check_mach) {
			if (!check_mach->detect)
				continue;

			if (!check_mach->detect())
				continue;

			mach = check_mach;
		}

		/*
		 * If we don't recognise the machine then we can't continue, so
		 * die here.
		 */
		BUG_ON(!mach);

		/* Retrieve the machine's FDT */
		fdt = mach->fdt;
	}
	return (void *)fdt;
}

#ifdef CONFIG_RELOCATABLE

void __init plat_fdt_relocated(void *new_location)
{
	/*
	 * reset fdt as the cached value would point to the location
	 * before relocations happened and update the location argument
	 * if it was passed using UHI
	 */
	fdt = NULL;

	if (fw_arg0 == -2)
		fw_arg1 = (unsigned long)new_location;
}

#endif /* CONFIG_RELOCATABLE */

void __init plat_mem_setup(void)
{
	if (mach && mach->fixup_fdt)
		fdt = mach->fixup_fdt(fdt, mach_match_data);

	fw_init_cmdline();
	__dt_setup_arch((void *)fdt);
}

void __init device_tree_init(void)
{
	int err;

	unflatten_and_copy_device_tree();
	mips_cpc_probe();

	err = register_cps_smp_ops();
	if (err)
		err = register_up_smp_ops();
}

int __init apply_mips_fdt_fixups(void *fdt_out, size_t fdt_out_size,
				 const void *fdt_in,
				 const struct mips_fdt_fixup *fixups)
{
	int err;

	err = fdt_open_into(fdt_in, fdt_out, fdt_out_size);
	if (err) {
		pr_err("Failed to open FDT\n");
		return err;
	}

	for (; fixups->apply; fixups++) {
		err = fixups->apply(fdt_out);
		if (err) {
			pr_err("Failed to apply FDT fixup \"%s\"\n",
			       fixups->description);
			return err;
		}
	}

	err = fdt_pack(fdt_out);
	if (err)
		pr_err("Failed to pack FDT\n");
	return err;
}

void __init plat_time_init(void)
{
	struct device_node *np;
	struct clk *clk;

	of_clk_init(NULL);

	if (!cpu_has_counter) {
		mips_hpt_frequency = 0;
	} else if (mach && mach->measure_hpt_freq) {
		mips_hpt_frequency = mach->measure_hpt_freq();
	} else {
		np = of_get_cpu_node(0, NULL);
		if (!np) {
			pr_err("Failed to get CPU node\n");
			return;
		}

		clk = of_clk_get(np, 0);
		if (IS_ERR(clk)) {
			pr_err("Failed to get CPU clock: %ld\n", PTR_ERR(clk));
			return;
		}

		mips_hpt_frequency = clk_get_rate(clk);
		clk_put(clk);

		switch (boot_cpu_type()) {
		case CPU_20KC:
		case CPU_25KF:
			/* The counter runs at the CPU clock rate */
			break;
		default:
			/* The counter runs at half the CPU clock rate */
			mips_hpt_frequency /= 2;
			break;
		}
	}

	timer_probe();
}

void __init arch_init_irq(void)
{
	struct device_node *intc_node;

	intc_node = of_find_compatible_node(NULL, NULL,
					    "mti,cpu-interrupt-controller");
	if (!cpu_has_veic && !intc_node)
		mips_cpu_irq_init();
	of_node_put(intc_node);

	irqchip_init();
}
