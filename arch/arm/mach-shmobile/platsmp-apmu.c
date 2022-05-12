// SPDX-License-Identifier: GPL-2.0
/*
 * SMP support for SoCs with APMU
 *
 * Copyright (C) 2014  Renesas Electronics Corporation
 * Copyright (C) 2013  Magnus Damm
 */
#include <linux/cpu_pm.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/of_address.h>
#include <linux/smp.h>
#include <linux/suspend.h>
#include <linux/threads.h>
#include <asm/cacheflush.h>
#include <asm/cp15.h>
#include <asm/proc-fns.h>
#include <asm/smp_plat.h>
#include <asm/suspend.h>
#include "common.h"
#include "rcar-gen2.h"

static struct {
	void __iomem *iomem;
	int bit;
} apmu_cpus[NR_CPUS];

#define WUPCR_OFFS	 0x10		/* Wake Up Control Register */
#define PSTR_OFFS	 0x40		/* Power Status Register */
#define CPUNCR_OFFS(n)	(0x100 + (0x10 * (n)))
					/* CPUn Power Status Control Register */
#define DBGRCR_OFFS	0x180		/* Debug Resource Reset Control Reg. */

/* Power Status Register */
#define CPUNST(r, n)	(((r) >> (n * 4)) & 3)	/* CPUn Status Bit */
#define CPUST_RUN	0		/* Run Mode */
#define CPUST_STANDBY	3		/* CoreStandby Mode */

/* Debug Resource Reset Control Register */
#define DBGCPUREN	BIT(24)		/* CPU Other Reset Request Enable */
#define DBGCPUNREN(n)	BIT((n) + 20)	/* CPUn Reset Request Enable */
#define DBGCPUPREN	BIT(19)		/* CPU Peripheral Reset Req. Enable */

static int __maybe_unused apmu_power_on(void __iomem *p, int bit)
{
	/* request power on */
	writel_relaxed(BIT(bit), p + WUPCR_OFFS);

	/* wait for APMU to finish */
	while (readl_relaxed(p + WUPCR_OFFS) != 0)
		;

	return 0;
}

static int __maybe_unused apmu_power_off(void __iomem *p, int bit)
{
	/* request Core Standby for next WFI */
	writel_relaxed(3, p + CPUNCR_OFFS(bit));
	return 0;
}

static int __maybe_unused apmu_power_off_poll(void __iomem *p, int bit)
{
	int k;

	for (k = 0; k < 1000; k++) {
		if (CPUNST(readl_relaxed(p + PSTR_OFFS), bit) == CPUST_STANDBY)
			return 1;

		mdelay(1);
	}

	return 0;
}

static int __maybe_unused apmu_wrap(int cpu, int (*fn)(void __iomem *p, int cpu))
{
	void __iomem *p = apmu_cpus[cpu].iomem;

	return p ? fn(p, apmu_cpus[cpu].bit) : -EINVAL;
}

#if defined(CONFIG_HOTPLUG_CPU) || defined(CONFIG_SUSPEND)
/* nicked from arch/arm/mach-exynos/hotplug.c */
static inline void cpu_enter_lowpower_a15(void)
{
	unsigned int v;

	asm volatile(
	"       mrc     p15, 0, %0, c1, c0, 0\n"
	"       bic     %0, %0, %1\n"
	"       mcr     p15, 0, %0, c1, c0, 0\n"
		: "=&r" (v)
		: "Ir" (CR_C)
		: "cc");

	flush_cache_louis();

	asm volatile(
	/*
	 * Turn off coherency
	 */
	"       mrc     p15, 0, %0, c1, c0, 1\n"
	"       bic     %0, %0, %1\n"
	"       mcr     p15, 0, %0, c1, c0, 1\n"
		: "=&r" (v)
		: "Ir" (0x40)
		: "cc");

	isb();
	dsb();
}

static void shmobile_smp_apmu_cpu_shutdown(unsigned int cpu)
{

	/* Select next sleep mode using the APMU */
	apmu_wrap(cpu, apmu_power_off);

	/* Do ARM specific CPU shutdown */
	cpu_enter_lowpower_a15();
}
#endif

#if defined(CONFIG_HOTPLUG_CPU)
static void shmobile_smp_apmu_cpu_die(unsigned int cpu)
{
	/* For this particular CPU deregister boot vector */
	shmobile_smp_hook(cpu, 0, 0);

	/* Shutdown CPU core */
	shmobile_smp_apmu_cpu_shutdown(cpu);

	/* jump to shared mach-shmobile sleep / reset code */
	shmobile_smp_sleep();
}

static int shmobile_smp_apmu_cpu_kill(unsigned int cpu)
{
	return apmu_wrap(cpu, apmu_power_off_poll);
}
#endif

#if defined(CONFIG_SUSPEND)
static int shmobile_smp_apmu_do_suspend(unsigned long cpu)
{
	shmobile_smp_hook(cpu, __pa_symbol(cpu_resume), 0);
	shmobile_smp_apmu_cpu_shutdown(cpu);
	cpu_do_idle(); /* WFI selects Core Standby */
	return 1;
}

static inline void cpu_leave_lowpower(void)
{
	unsigned int v;

	asm volatile("mrc    p15, 0, %0, c1, c0, 0\n"
		     "       orr     %0, %0, %1\n"
		     "       mcr     p15, 0, %0, c1, c0, 0\n"
		     "       mrc     p15, 0, %0, c1, c0, 1\n"
		     "       orr     %0, %0, %2\n"
		     "       mcr     p15, 0, %0, c1, c0, 1\n"
		     : "=&r" (v)
		     : "Ir" (CR_C), "Ir" (0x40)
		     : "cc");
}

static int shmobile_smp_apmu_enter_suspend(suspend_state_t state)
{
	cpu_suspend(smp_processor_id(), shmobile_smp_apmu_do_suspend);
	cpu_leave_lowpower();
	return 0;
}

void __init shmobile_smp_apmu_suspend_init(void)
{
	shmobile_suspend_ops.enter = shmobile_smp_apmu_enter_suspend;
}
#endif

#ifdef CONFIG_SMP
static void apmu_init_cpu(struct resource *res, int cpu, int bit)
{
	u32 x;

	if ((cpu >= ARRAY_SIZE(apmu_cpus)) || apmu_cpus[cpu].iomem)
		return;

	apmu_cpus[cpu].iomem = ioremap(res->start, resource_size(res));
	apmu_cpus[cpu].bit = bit;

	pr_debug("apmu ioremap %d %d %pr\n", cpu, bit, res);

	/* Setup for debug mode */
	x = readl(apmu_cpus[cpu].iomem + DBGRCR_OFFS);
	x |= DBGCPUREN | DBGCPUNREN(bit) | DBGCPUPREN;
	writel(x, apmu_cpus[cpu].iomem + DBGRCR_OFFS);
}

static const struct of_device_id apmu_ids[] = {
	{ .compatible = "renesas,apmu" },
	{ /*sentinel*/ }
};

static void apmu_parse_dt(void (*fn)(struct resource *res, int cpu, int bit))
{
	struct device_node *np_apmu, *np_cpu;
	struct resource res;
	int bit, index;
	u32 id;

	for_each_matching_node(np_apmu, apmu_ids) {
		/* only enable the cluster that includes the boot CPU */
		bool is_allowed = false;

		for (bit = 0; bit < CONFIG_NR_CPUS; bit++) {
			np_cpu = of_parse_phandle(np_apmu, "cpus", bit);
			if (np_cpu) {
				if (!of_property_read_u32(np_cpu, "reg", &id)) {
					if (id == cpu_logical_map(0)) {
						is_allowed = true;
						of_node_put(np_cpu);
						break;
					}

				}
				of_node_put(np_cpu);
			}
		}
		if (!is_allowed)
			continue;

		for (bit = 0; bit < CONFIG_NR_CPUS; bit++) {
			np_cpu = of_parse_phandle(np_apmu, "cpus", bit);
			if (np_cpu) {
				if (!of_property_read_u32(np_cpu, "reg", &id)) {
					index = get_logical_index(id);
					if ((index >= 0) &&
					    !of_address_to_resource(np_apmu,
								    0, &res))
						fn(&res, index, bit);
				}
				of_node_put(np_cpu);
			}
		}
	}
}

static void __init shmobile_smp_apmu_setup_boot(void)
{
	/* install boot code shared by all CPUs */
	shmobile_boot_fn = __pa_symbol(shmobile_smp_boot);
	shmobile_boot_fn_gen2 = shmobile_boot_fn;
}

static int shmobile_smp_apmu_boot_secondary(unsigned int cpu,
					    struct task_struct *idle)
{
	/* For this particular CPU register boot vector */
	shmobile_smp_hook(cpu, __pa_symbol(shmobile_boot_apmu), 0);

	return apmu_wrap(cpu, apmu_power_on);
}

static void __init shmobile_smp_apmu_prepare_cpus_dt(unsigned int max_cpus)
{
	shmobile_smp_apmu_setup_boot();
	apmu_parse_dt(apmu_init_cpu);
	rcar_gen2_pm_init();
}

static struct smp_operations apmu_smp_ops __initdata = {
	.smp_prepare_cpus	= shmobile_smp_apmu_prepare_cpus_dt,
	.smp_boot_secondary	= shmobile_smp_apmu_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_can_disable	= shmobile_smp_cpu_can_disable,
	.cpu_die		= shmobile_smp_apmu_cpu_die,
	.cpu_kill		= shmobile_smp_apmu_cpu_kill,
#endif
};

CPU_METHOD_OF_DECLARE(shmobile_smp_apmu, "renesas,apmu", &apmu_smp_ops);
#endif /* CONFIG_SMP */
