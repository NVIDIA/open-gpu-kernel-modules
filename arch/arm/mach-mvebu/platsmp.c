/*
 * Symmetric Multi Processing (SMP) support for Armada XP
 *
 * Copyright (C) 2012 Marvell
 *
 * Lior Amsalem <alior@marvell.com>
 * Yehuda Yitschak <yehuday@marvell.com>
 * Gregory CLEMENT <gregory.clement@free-electrons.com>
 * Thomas Petazzoni <thomas.petazzoni@free-electrons.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 *
 * The Armada XP SoC has 4 ARMv7 PJ4B CPUs running in full HW coherency
 * This file implements the routines for preparing the SMP infrastructure
 * and waking up the secondary CPUs
 */

#include <linux/init.h>
#include <linux/smp.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/mbus.h>
#include <asm/cacheflush.h>
#include <asm/smp_plat.h>
#include "common.h"
#include "armada-370-xp.h"
#include "pmsu.h"
#include "coherency.h"

#define ARMADA_XP_MAX_CPUS 4

#define AXP_BOOTROM_BASE 0xfff00000
#define AXP_BOOTROM_SIZE 0x100000

static struct clk *boot_cpu_clk;

static struct clk *get_cpu_clk(int cpu)
{
	struct clk *cpu_clk;
	struct device_node *np = of_get_cpu_node(cpu, NULL);

	if (WARN(!np, "missing cpu node\n"))
		return NULL;
	cpu_clk = of_clk_get(np, 0);
	if (WARN_ON(IS_ERR(cpu_clk)))
		return NULL;
	return cpu_clk;
}

static int armada_xp_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	int ret, hw_cpu;

	pr_info("Booting CPU %d\n", cpu);

	hw_cpu = cpu_logical_map(cpu);
	mvebu_pmsu_set_cpu_boot_addr(hw_cpu, armada_xp_secondary_startup);

	/*
	 * This is needed to wake up CPUs in the offline state after
	 * using CPU hotplug.
	 */
	arch_send_wakeup_ipi_mask(cpumask_of(cpu));

	/*
	 * This is needed to take secondary CPUs out of reset on the
	 * initial boot.
	 */
	ret = mvebu_cpu_reset_deassert(hw_cpu);
	if (ret) {
		pr_warn("unable to boot CPU: %d\n", ret);
		return ret;
	}

	return 0;
}

/*
 * When a CPU is brought back online, either through CPU hotplug, or
 * because of the boot of a kexec'ed kernel, the PMSU configuration
 * for this CPU might be in the deep idle state, preventing this CPU
 * from receiving interrupts. Here, we therefore take out the current
 * CPU from this state, which was entered by armada_xp_cpu_die()
 * below.
 */
static void armada_xp_secondary_init(unsigned int cpu)
{
	mvebu_v7_pmsu_idle_exit();
}

static void __init armada_xp_smp_init_cpus(void)
{
	unsigned int ncores = num_possible_cpus();

	if (ncores == 0 || ncores > ARMADA_XP_MAX_CPUS)
		panic("Invalid number of CPUs in DT\n");
}

static int armada_xp_sync_secondary_clk(unsigned int cpu)
{
	struct clk *cpu_clk = get_cpu_clk(cpu);

	if (!cpu_clk || !boot_cpu_clk)
		return 0;

	clk_prepare_enable(cpu_clk);
	clk_set_rate(cpu_clk, clk_get_rate(boot_cpu_clk));

	return 0;
}

static void __init armada_xp_smp_prepare_cpus(unsigned int max_cpus)
{
	struct device_node *node;
	struct resource res;
	int err;

	flush_cache_all();
	set_cpu_coherent();

	boot_cpu_clk = get_cpu_clk(smp_processor_id());
	if (boot_cpu_clk) {
		clk_prepare_enable(boot_cpu_clk);
		cpuhp_setup_state_nocalls(CPUHP_AP_ARM_MVEBU_SYNC_CLOCKS,
					  "arm/mvebu/sync_clocks:online",
					  armada_xp_sync_secondary_clk, NULL);
	}

	/*
	 * In order to boot the secondary CPUs we need to ensure
	 * the bootROM is mapped at the correct address.
	 */
	node = of_find_compatible_node(NULL, NULL, "marvell,bootrom");
	if (!node)
		panic("Cannot find 'marvell,bootrom' compatible node");

	err = of_address_to_resource(node, 0, &res);
	of_node_put(node);
	if (err < 0)
		panic("Cannot get 'bootrom' node address");

	if (res.start != AXP_BOOTROM_BASE ||
	    resource_size(&res) != AXP_BOOTROM_SIZE)
		panic("The address for the BootROM is incorrect");
}

#ifdef CONFIG_HOTPLUG_CPU
static void armada_xp_cpu_die(unsigned int cpu)
{
	/*
	 * CPU hotplug is implemented by putting offline CPUs into the
	 * deep idle sleep state.
	 */
	armada_370_xp_pmsu_idle_enter(true);
}

/*
 * We need a dummy function, so that platform_can_cpu_hotplug() knows
 * we support CPU hotplug. However, the function does not need to do
 * anything, because CPUs going offline can enter the deep idle state
 * by themselves, without any help from a still alive CPU.
 */
static int armada_xp_cpu_kill(unsigned int cpu)
{
	return 1;
}
#endif

const struct smp_operations armada_xp_smp_ops __initconst = {
	.smp_init_cpus		= armada_xp_smp_init_cpus,
	.smp_prepare_cpus	= armada_xp_smp_prepare_cpus,
	.smp_boot_secondary	= armada_xp_boot_secondary,
	.smp_secondary_init     = armada_xp_secondary_init,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= armada_xp_cpu_die,
	.cpu_kill               = armada_xp_cpu_kill,
#endif
};

CPU_METHOD_OF_DECLARE(armada_xp_smp, "marvell,armada-xp-smp",
		      &armada_xp_smp_ops);

#define MV98DX3236_CPU_RESUME_CTRL_REG 0x08
#define MV98DX3236_CPU_RESUME_ADDR_REG 0x04

static const struct of_device_id of_mv98dx3236_resume_table[] = {
	{
		.compatible = "marvell,98dx3336-resume-ctrl",
	},
	{ /* end of list */ },
};

static int mv98dx3236_resume_set_cpu_boot_addr(int hw_cpu, void *boot_addr)
{
	struct device_node *np;
	void __iomem *base;
	WARN_ON(hw_cpu != 1);

	np = of_find_matching_node(NULL, of_mv98dx3236_resume_table);
	if (!np)
		return -ENODEV;

	base = of_io_request_and_map(np, 0, of_node_full_name(np));
	of_node_put(np);
	if (IS_ERR(base))
		return PTR_ERR(base);

	writel(0, base + MV98DX3236_CPU_RESUME_CTRL_REG);
	writel(__pa_symbol(boot_addr), base + MV98DX3236_CPU_RESUME_ADDR_REG);

	iounmap(base);

	return 0;
}

static int mv98dx3236_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	int ret, hw_cpu;

	hw_cpu = cpu_logical_map(cpu);
	mv98dx3236_resume_set_cpu_boot_addr(hw_cpu,
					    armada_xp_secondary_startup);

	/*
	 * This is needed to wake up CPUs in the offline state after
	 * using CPU hotplug.
	 */
	arch_send_wakeup_ipi_mask(cpumask_of(cpu));

	/*
	 * This is needed to take secondary CPUs out of reset on the
	 * initial boot.
	 */
	ret = mvebu_cpu_reset_deassert(hw_cpu);
	if (ret) {
		pr_warn("unable to boot CPU: %d\n", ret);
		return ret;
	}

	return 0;
}

static const struct smp_operations mv98dx3236_smp_ops __initconst = {
	.smp_init_cpus		= armada_xp_smp_init_cpus,
	.smp_prepare_cpus	= armada_xp_smp_prepare_cpus,
	.smp_boot_secondary	= mv98dx3236_boot_secondary,
	.smp_secondary_init     = armada_xp_secondary_init,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= armada_xp_cpu_die,
	.cpu_kill               = armada_xp_cpu_kill,
#endif
};

CPU_METHOD_OF_DECLARE(mv98dx3236_smp, "marvell,98dx3236-smp",
		      &mv98dx3236_smp_ops);
