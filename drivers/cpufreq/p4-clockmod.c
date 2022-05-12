// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *	Pentium 4/Xeon CPU on demand clock modulation/speed scaling
 *	(C) 2002 - 2003 Dominik Brodowski <linux@brodo.de>
 *	(C) 2002 Zwane Mwaikambo <zwane@commfireservices.com>
 *	(C) 2002 Arjan van de Ven <arjanv@redhat.com>
 *	(C) 2002 Tora T. Engstad
 *	All Rights Reserved
 *
 *      The author(s) of this software shall not be held liable for damages
 *      of any nature resulting due to the use of this software. This
 *      software is provided AS-IS with no warranties.
 *
 *	Date		Errata			Description
 *	20020525	N44, O17	12.5% or 25% DC causes lockup
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/smp.h>
#include <linux/cpufreq.h>
#include <linux/cpumask.h>
#include <linux/timex.h>

#include <asm/processor.h>
#include <asm/msr.h>
#include <asm/timer.h>
#include <asm/cpu_device_id.h>

#include "speedstep-lib.h"

/*
 * Duty Cycle (3bits), note DC_DISABLE is not specified in
 * intel docs i just use it to mean disable
 */
enum {
	DC_RESV, DC_DFLT, DC_25PT, DC_38PT, DC_50PT,
	DC_64PT, DC_75PT, DC_88PT, DC_DISABLE
};

#define DC_ENTRIES	8


static int has_N44_O17_errata[NR_CPUS];
static unsigned int stock_freq;
static struct cpufreq_driver p4clockmod_driver;
static unsigned int cpufreq_p4_get(unsigned int cpu);

static int cpufreq_p4_setdc(unsigned int cpu, unsigned int newstate)
{
	u32 l, h;

	if ((newstate > DC_DISABLE) || (newstate == DC_RESV))
		return -EINVAL;

	rdmsr_on_cpu(cpu, MSR_IA32_THERM_STATUS, &l, &h);

	if (l & 0x01)
		pr_debug("CPU#%d currently thermal throttled\n", cpu);

	if (has_N44_O17_errata[cpu] &&
	    (newstate == DC_25PT || newstate == DC_DFLT))
		newstate = DC_38PT;

	rdmsr_on_cpu(cpu, MSR_IA32_THERM_CONTROL, &l, &h);
	if (newstate == DC_DISABLE) {
		pr_debug("CPU#%d disabling modulation\n", cpu);
		wrmsr_on_cpu(cpu, MSR_IA32_THERM_CONTROL, l & ~(1<<4), h);
	} else {
		pr_debug("CPU#%d setting duty cycle to %d%%\n",
			cpu, ((125 * newstate) / 10));
		/* bits 63 - 5	: reserved
		 * bit  4	: enable/disable
		 * bits 3-1	: duty cycle
		 * bit  0	: reserved
		 */
		l = (l & ~14);
		l = l | (1<<4) | ((newstate & 0x7)<<1);
		wrmsr_on_cpu(cpu, MSR_IA32_THERM_CONTROL, l, h);
	}

	return 0;
}


static struct cpufreq_frequency_table p4clockmod_table[] = {
	{0, DC_RESV, CPUFREQ_ENTRY_INVALID},
	{0, DC_DFLT, 0},
	{0, DC_25PT, 0},
	{0, DC_38PT, 0},
	{0, DC_50PT, 0},
	{0, DC_64PT, 0},
	{0, DC_75PT, 0},
	{0, DC_88PT, 0},
	{0, DC_DISABLE, 0},
	{0, DC_RESV, CPUFREQ_TABLE_END},
};


static int cpufreq_p4_target(struct cpufreq_policy *policy, unsigned int index)
{
	int i;

	/* run on each logical CPU,
	 * see section 13.15.3 of IA32 Intel Architecture Software
	 * Developer's Manual, Volume 3
	 */
	for_each_cpu(i, policy->cpus)
		cpufreq_p4_setdc(i, p4clockmod_table[index].driver_data);

	return 0;
}


static unsigned int cpufreq_p4_get_frequency(struct cpuinfo_x86 *c)
{
	if (c->x86 == 0x06) {
		if (cpu_has(c, X86_FEATURE_EST))
			pr_warn_once("Warning: EST-capable CPU detected. The acpi-cpufreq module offers voltage scaling in addition to frequency scaling. You should use that instead of p4-clockmod, if possible.\n");
		switch (c->x86_model) {
		case 0x0E: /* Core */
		case 0x0F: /* Core Duo */
		case 0x16: /* Celeron Core */
		case 0x1C: /* Atom */
			p4clockmod_driver.flags |= CPUFREQ_CONST_LOOPS;
			return speedstep_get_frequency(SPEEDSTEP_CPU_PCORE);
		case 0x0D: /* Pentium M (Dothan) */
			p4clockmod_driver.flags |= CPUFREQ_CONST_LOOPS;
			fallthrough;
		case 0x09: /* Pentium M (Banias) */
			return speedstep_get_frequency(SPEEDSTEP_CPU_PM);
		}
	}

	if (c->x86 != 0xF)
		return 0;

	/* on P-4s, the TSC runs with constant frequency independent whether
	 * throttling is active or not. */
	p4clockmod_driver.flags |= CPUFREQ_CONST_LOOPS;

	if (speedstep_detect_processor() == SPEEDSTEP_CPU_P4M) {
		pr_warn("Warning: Pentium 4-M detected. The speedstep-ich or acpi cpufreq modules offer voltage scaling in addition of frequency scaling. You should use either one instead of p4-clockmod, if possible.\n");
		return speedstep_get_frequency(SPEEDSTEP_CPU_P4M);
	}

	return speedstep_get_frequency(SPEEDSTEP_CPU_P4D);
}



static int cpufreq_p4_cpu_init(struct cpufreq_policy *policy)
{
	struct cpuinfo_x86 *c = &cpu_data(policy->cpu);
	int cpuid = 0;
	unsigned int i;

#ifdef CONFIG_SMP
	cpumask_copy(policy->cpus, topology_sibling_cpumask(policy->cpu));
#endif

	/* Errata workaround */
	cpuid = (c->x86 << 8) | (c->x86_model << 4) | c->x86_stepping;
	switch (cpuid) {
	case 0x0f07:
	case 0x0f0a:
	case 0x0f11:
	case 0x0f12:
		has_N44_O17_errata[policy->cpu] = 1;
		pr_debug("has errata -- disabling low frequencies\n");
	}

	if (speedstep_detect_processor() == SPEEDSTEP_CPU_P4D &&
	    c->x86_model < 2) {
		/* switch to maximum frequency and measure result */
		cpufreq_p4_setdc(policy->cpu, DC_DISABLE);
		recalibrate_cpu_khz();
	}
	/* get max frequency */
	stock_freq = cpufreq_p4_get_frequency(c);
	if (!stock_freq)
		return -EINVAL;

	/* table init */
	for (i = 1; (p4clockmod_table[i].frequency != CPUFREQ_TABLE_END); i++) {
		if ((i < 2) && (has_N44_O17_errata[policy->cpu]))
			p4clockmod_table[i].frequency = CPUFREQ_ENTRY_INVALID;
		else
			p4clockmod_table[i].frequency = (stock_freq * i)/8;
	}

	/* cpuinfo and default policy values */

	/* the transition latency is set to be 1 higher than the maximum
	 * transition latency of the ondemand governor */
	policy->cpuinfo.transition_latency = 10000001;
	policy->freq_table = &p4clockmod_table[0];

	return 0;
}


static unsigned int cpufreq_p4_get(unsigned int cpu)
{
	u32 l, h;

	rdmsr_on_cpu(cpu, MSR_IA32_THERM_CONTROL, &l, &h);

	if (l & 0x10) {
		l = l >> 1;
		l &= 0x7;
	} else
		l = DC_DISABLE;

	if (l != DC_DISABLE)
		return stock_freq * l / 8;

	return stock_freq;
}

static struct cpufreq_driver p4clockmod_driver = {
	.verify		= cpufreq_generic_frequency_table_verify,
	.target_index	= cpufreq_p4_target,
	.init		= cpufreq_p4_cpu_init,
	.get		= cpufreq_p4_get,
	.name		= "p4-clockmod",
	.attr		= cpufreq_generic_attr,
};

static const struct x86_cpu_id cpufreq_p4_id[] = {
	X86_MATCH_VENDOR_FEATURE(INTEL, X86_FEATURE_ACC, NULL),
	{}
};

/*
 * Intentionally no MODULE_DEVICE_TABLE here: this driver should not
 * be auto loaded.  Please don't add one.
 */

static int __init cpufreq_p4_init(void)
{
	int ret;

	/*
	 * THERM_CONTROL is architectural for IA32 now, so
	 * we can rely on the capability checks
	 */
	if (!x86_match_cpu(cpufreq_p4_id) || !boot_cpu_has(X86_FEATURE_ACPI))
		return -ENODEV;

	ret = cpufreq_register_driver(&p4clockmod_driver);
	if (!ret)
		pr_info("P4/Xeon(TM) CPU On-Demand Clock Modulation available\n");

	return ret;
}


static void __exit cpufreq_p4_exit(void)
{
	cpufreq_unregister_driver(&p4clockmod_driver);
}


MODULE_AUTHOR("Zwane Mwaikambo <zwane@commfireservices.com>");
MODULE_DESCRIPTION("cpufreq driver for Pentium(TM) 4/Xeon(TM)");
MODULE_LICENSE("GPL");

late_initcall(cpufreq_p4_init);
module_exit(cpufreq_p4_exit);
