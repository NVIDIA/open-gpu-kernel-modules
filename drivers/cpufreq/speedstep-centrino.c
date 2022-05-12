// SPDX-License-Identifier: GPL-2.0-only
/*
 * cpufreq driver for Enhanced SpeedStep, as found in Intel's Pentium
 * M (part of the Centrino chipset).
 *
 * Since the original Pentium M, most new Intel CPUs support Enhanced
 * SpeedStep.
 *
 * Despite the "SpeedStep" in the name, this is almost entirely unlike
 * traditional SpeedStep.
 *
 * Modelled on speedstep.c
 *
 * Copyright (C) 2003 Jeremy Fitzhardinge <jeremy@goop.org>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cpufreq.h>
#include <linux/sched.h>	/* current */
#include <linux/delay.h>
#include <linux/compiler.h>
#include <linux/gfp.h>

#include <asm/msr.h>
#include <asm/processor.h>
#include <asm/cpufeature.h>
#include <asm/cpu_device_id.h>

#define MAINTAINER	"linux-pm@vger.kernel.org"

#define INTEL_MSR_RANGE	(0xffff)

struct cpu_id
{
	__u8	x86;            /* CPU family */
	__u8	x86_model;	/* model */
	__u8	x86_stepping;	/* stepping */
};

enum {
	CPU_BANIAS,
	CPU_DOTHAN_A1,
	CPU_DOTHAN_A2,
	CPU_DOTHAN_B0,
	CPU_MP4HT_D0,
	CPU_MP4HT_E0,
};

static const struct cpu_id cpu_ids[] = {
	[CPU_BANIAS]	= { 6,  9, 5 },
	[CPU_DOTHAN_A1]	= { 6, 13, 1 },
	[CPU_DOTHAN_A2]	= { 6, 13, 2 },
	[CPU_DOTHAN_B0]	= { 6, 13, 6 },
	[CPU_MP4HT_D0]	= {15,  3, 4 },
	[CPU_MP4HT_E0]	= {15,  4, 1 },
};
#define N_IDS	ARRAY_SIZE(cpu_ids)

struct cpu_model
{
	const struct cpu_id *cpu_id;
	const char	*model_name;
	unsigned	max_freq; /* max clock in kHz */

	struct cpufreq_frequency_table *op_points; /* clock/voltage pairs */
};
static int centrino_verify_cpu_id(const struct cpuinfo_x86 *c,
				  const struct cpu_id *x);

/* Operating points for current CPU */
static DEFINE_PER_CPU(struct cpu_model *, centrino_model);
static DEFINE_PER_CPU(const struct cpu_id *, centrino_cpu);

static struct cpufreq_driver centrino_driver;

#ifdef CONFIG_X86_SPEEDSTEP_CENTRINO_TABLE

/* Computes the correct form for IA32_PERF_CTL MSR for a particular
   frequency/voltage operating point; frequency in MHz, volts in mV.
   This is stored as "driver_data" in the structure. */
#define OP(mhz, mv)							\
	{								\
		.frequency = (mhz) * 1000,				\
		.driver_data = (((mhz)/100) << 8) | ((mv - 700) / 16)		\
	}

/*
 * These voltage tables were derived from the Intel Pentium M
 * datasheet, document 25261202.pdf, Table 5.  I have verified they
 * are consistent with my IBM ThinkPad X31, which has a 1.3GHz Pentium
 * M.
 */

/* Ultra Low Voltage Intel Pentium M processor 900MHz (Banias) */
static struct cpufreq_frequency_table banias_900[] =
{
	OP(600,  844),
	OP(800,  988),
	OP(900, 1004),
	{ .frequency = CPUFREQ_TABLE_END }
};

/* Ultra Low Voltage Intel Pentium M processor 1000MHz (Banias) */
static struct cpufreq_frequency_table banias_1000[] =
{
	OP(600,   844),
	OP(800,   972),
	OP(900,   988),
	OP(1000, 1004),
	{ .frequency = CPUFREQ_TABLE_END }
};

/* Low Voltage Intel Pentium M processor 1.10GHz (Banias) */
static struct cpufreq_frequency_table banias_1100[] =
{
	OP( 600,  956),
	OP( 800, 1020),
	OP( 900, 1100),
	OP(1000, 1164),
	OP(1100, 1180),
	{ .frequency = CPUFREQ_TABLE_END }
};


/* Low Voltage Intel Pentium M processor 1.20GHz (Banias) */
static struct cpufreq_frequency_table banias_1200[] =
{
	OP( 600,  956),
	OP( 800, 1004),
	OP( 900, 1020),
	OP(1000, 1100),
	OP(1100, 1164),
	OP(1200, 1180),
	{ .frequency = CPUFREQ_TABLE_END }
};

/* Intel Pentium M processor 1.30GHz (Banias) */
static struct cpufreq_frequency_table banias_1300[] =
{
	OP( 600,  956),
	OP( 800, 1260),
	OP(1000, 1292),
	OP(1200, 1356),
	OP(1300, 1388),
	{ .frequency = CPUFREQ_TABLE_END }
};

/* Intel Pentium M processor 1.40GHz (Banias) */
static struct cpufreq_frequency_table banias_1400[] =
{
	OP( 600,  956),
	OP( 800, 1180),
	OP(1000, 1308),
	OP(1200, 1436),
	OP(1400, 1484),
	{ .frequency = CPUFREQ_TABLE_END }
};

/* Intel Pentium M processor 1.50GHz (Banias) */
static struct cpufreq_frequency_table banias_1500[] =
{
	OP( 600,  956),
	OP( 800, 1116),
	OP(1000, 1228),
	OP(1200, 1356),
	OP(1400, 1452),
	OP(1500, 1484),
	{ .frequency = CPUFREQ_TABLE_END }
};

/* Intel Pentium M processor 1.60GHz (Banias) */
static struct cpufreq_frequency_table banias_1600[] =
{
	OP( 600,  956),
	OP( 800, 1036),
	OP(1000, 1164),
	OP(1200, 1276),
	OP(1400, 1420),
	OP(1600, 1484),
	{ .frequency = CPUFREQ_TABLE_END }
};

/* Intel Pentium M processor 1.70GHz (Banias) */
static struct cpufreq_frequency_table banias_1700[] =
{
	OP( 600,  956),
	OP( 800, 1004),
	OP(1000, 1116),
	OP(1200, 1228),
	OP(1400, 1308),
	OP(1700, 1484),
	{ .frequency = CPUFREQ_TABLE_END }
};
#undef OP

#define _BANIAS(cpuid, max, name)	\
{	.cpu_id		= cpuid,	\
	.model_name	= "Intel(R) Pentium(R) M processor " name "MHz", \
	.max_freq	= (max)*1000,	\
	.op_points	= banias_##max,	\
}
#define BANIAS(max)	_BANIAS(&cpu_ids[CPU_BANIAS], max, #max)

/* CPU models, their operating frequency range, and freq/voltage
   operating points */
static struct cpu_model models[] =
{
	_BANIAS(&cpu_ids[CPU_BANIAS], 900, " 900"),
	BANIAS(1000),
	BANIAS(1100),
	BANIAS(1200),
	BANIAS(1300),
	BANIAS(1400),
	BANIAS(1500),
	BANIAS(1600),
	BANIAS(1700),

	/* NULL model_name is a wildcard */
	{ &cpu_ids[CPU_DOTHAN_A1], NULL, 0, NULL },
	{ &cpu_ids[CPU_DOTHAN_A2], NULL, 0, NULL },
	{ &cpu_ids[CPU_DOTHAN_B0], NULL, 0, NULL },
	{ &cpu_ids[CPU_MP4HT_D0], NULL, 0, NULL },
	{ &cpu_ids[CPU_MP4HT_E0], NULL, 0, NULL },

	{ NULL, }
};
#undef _BANIAS
#undef BANIAS

static int centrino_cpu_init_table(struct cpufreq_policy *policy)
{
	struct cpuinfo_x86 *cpu = &cpu_data(policy->cpu);
	struct cpu_model *model;

	for(model = models; model->cpu_id != NULL; model++)
		if (centrino_verify_cpu_id(cpu, model->cpu_id) &&
		    (model->model_name == NULL ||
		     strcmp(cpu->x86_model_id, model->model_name) == 0))
			break;

	if (model->cpu_id == NULL) {
		/* No match at all */
		pr_debug("no support for CPU model \"%s\": "
		       "send /proc/cpuinfo to " MAINTAINER "\n",
		       cpu->x86_model_id);
		return -ENOENT;
	}

	if (model->op_points == NULL) {
		/* Matched a non-match */
		pr_debug("no table support for CPU model \"%s\"\n",
		       cpu->x86_model_id);
		pr_debug("try using the acpi-cpufreq driver\n");
		return -ENOENT;
	}

	per_cpu(centrino_model, policy->cpu) = model;

	pr_debug("found \"%s\": max frequency: %dkHz\n",
	       model->model_name, model->max_freq);

	return 0;
}

#else
static inline int centrino_cpu_init_table(struct cpufreq_policy *policy)
{
	return -ENODEV;
}
#endif /* CONFIG_X86_SPEEDSTEP_CENTRINO_TABLE */

static int centrino_verify_cpu_id(const struct cpuinfo_x86 *c,
				  const struct cpu_id *x)
{
	if ((c->x86 == x->x86) &&
	    (c->x86_model == x->x86_model) &&
	    (c->x86_stepping == x->x86_stepping))
		return 1;
	return 0;
}

/* To be called only after centrino_model is initialized */
static unsigned extract_clock(unsigned msr, unsigned int cpu, int failsafe)
{
	int i;

	/*
	 * Extract clock in kHz from PERF_CTL value
	 * for centrino, as some DSDTs are buggy.
	 * Ideally, this can be done using the acpi_data structure.
	 */
	if ((per_cpu(centrino_cpu, cpu) == &cpu_ids[CPU_BANIAS]) ||
	    (per_cpu(centrino_cpu, cpu) == &cpu_ids[CPU_DOTHAN_A1]) ||
	    (per_cpu(centrino_cpu, cpu) == &cpu_ids[CPU_DOTHAN_B0])) {
		msr = (msr >> 8) & 0xff;
		return msr * 100000;
	}

	if ((!per_cpu(centrino_model, cpu)) ||
	    (!per_cpu(centrino_model, cpu)->op_points))
		return 0;

	msr &= 0xffff;
	for (i = 0;
		per_cpu(centrino_model, cpu)->op_points[i].frequency
							!= CPUFREQ_TABLE_END;
	     i++) {
		if (msr == per_cpu(centrino_model, cpu)->op_points[i].driver_data)
			return per_cpu(centrino_model, cpu)->
							op_points[i].frequency;
	}
	if (failsafe)
		return per_cpu(centrino_model, cpu)->op_points[i-1].frequency;
	else
		return 0;
}

/* Return the current CPU frequency in kHz */
static unsigned int get_cur_freq(unsigned int cpu)
{
	unsigned l, h;
	unsigned clock_freq;

	rdmsr_on_cpu(cpu, MSR_IA32_PERF_STATUS, &l, &h);
	clock_freq = extract_clock(l, cpu, 0);

	if (unlikely(clock_freq == 0)) {
		/*
		 * On some CPUs, we can see transient MSR values (which are
		 * not present in _PSS), while CPU is doing some automatic
		 * P-state transition (like TM2). Get the last freq set 
		 * in PERF_CTL.
		 */
		rdmsr_on_cpu(cpu, MSR_IA32_PERF_CTL, &l, &h);
		clock_freq = extract_clock(l, cpu, 1);
	}
	return clock_freq;
}


static int centrino_cpu_init(struct cpufreq_policy *policy)
{
	struct cpuinfo_x86 *cpu = &cpu_data(policy->cpu);
	unsigned l, h;
	int i;

	/* Only Intel makes Enhanced Speedstep-capable CPUs */
	if (cpu->x86_vendor != X86_VENDOR_INTEL ||
	    !cpu_has(cpu, X86_FEATURE_EST))
		return -ENODEV;

	if (cpu_has(cpu, X86_FEATURE_CONSTANT_TSC))
		centrino_driver.flags |= CPUFREQ_CONST_LOOPS;

	if (policy->cpu != 0)
		return -ENODEV;

	for (i = 0; i < N_IDS; i++)
		if (centrino_verify_cpu_id(cpu, &cpu_ids[i]))
			break;

	if (i != N_IDS)
		per_cpu(centrino_cpu, policy->cpu) = &cpu_ids[i];

	if (!per_cpu(centrino_cpu, policy->cpu)) {
		pr_debug("found unsupported CPU with "
		"Enhanced SpeedStep: send /proc/cpuinfo to "
		MAINTAINER "\n");
		return -ENODEV;
	}

	if (centrino_cpu_init_table(policy))
		return -ENODEV;

	/* Check to see if Enhanced SpeedStep is enabled, and try to
	   enable it if not. */
	rdmsr(MSR_IA32_MISC_ENABLE, l, h);

	if (!(l & MSR_IA32_MISC_ENABLE_ENHANCED_SPEEDSTEP)) {
		l |= MSR_IA32_MISC_ENABLE_ENHANCED_SPEEDSTEP;
		pr_debug("trying to enable Enhanced SpeedStep (%x)\n", l);
		wrmsr(MSR_IA32_MISC_ENABLE, l, h);

		/* check to see if it stuck */
		rdmsr(MSR_IA32_MISC_ENABLE, l, h);
		if (!(l & MSR_IA32_MISC_ENABLE_ENHANCED_SPEEDSTEP)) {
			pr_info("couldn't enable Enhanced SpeedStep\n");
			return -ENODEV;
		}
	}

	policy->cpuinfo.transition_latency = 10000;
						/* 10uS transition latency */
	policy->freq_table = per_cpu(centrino_model, policy->cpu)->op_points;

	return 0;
}

static int centrino_cpu_exit(struct cpufreq_policy *policy)
{
	unsigned int cpu = policy->cpu;

	if (!per_cpu(centrino_model, cpu))
		return -ENODEV;

	per_cpu(centrino_model, cpu) = NULL;

	return 0;
}

/**
 * centrino_target - set a new CPUFreq policy
 * @policy: new policy
 * @index: index of target frequency
 *
 * Sets a new CPUFreq policy.
 */
static int centrino_target(struct cpufreq_policy *policy, unsigned int index)
{
	unsigned int	msr, oldmsr = 0, h = 0, cpu = policy->cpu;
	int			retval = 0;
	unsigned int		j, first_cpu;
	struct cpufreq_frequency_table *op_points;
	cpumask_var_t covered_cpus;

	if (unlikely(!zalloc_cpumask_var(&covered_cpus, GFP_KERNEL)))
		return -ENOMEM;

	if (unlikely(per_cpu(centrino_model, cpu) == NULL)) {
		retval = -ENODEV;
		goto out;
	}

	first_cpu = 1;
	op_points = &per_cpu(centrino_model, cpu)->op_points[index];
	for_each_cpu(j, policy->cpus) {
		int good_cpu;

		/*
		 * Support for SMP systems.
		 * Make sure we are running on CPU that wants to change freq
		 */
		if (policy->shared_type == CPUFREQ_SHARED_TYPE_ANY)
			good_cpu = cpumask_any_and(policy->cpus,
						   cpu_online_mask);
		else
			good_cpu = j;

		if (good_cpu >= nr_cpu_ids) {
			pr_debug("couldn't limit to CPUs in this domain\n");
			retval = -EAGAIN;
			if (first_cpu) {
				/* We haven't started the transition yet. */
				goto out;
			}
			break;
		}

		msr = op_points->driver_data;

		if (first_cpu) {
			rdmsr_on_cpu(good_cpu, MSR_IA32_PERF_CTL, &oldmsr, &h);
			if (msr == (oldmsr & 0xffff)) {
				pr_debug("no change needed - msr was and needs "
					"to be %x\n", oldmsr);
				retval = 0;
				goto out;
			}

			first_cpu = 0;
			/* all but 16 LSB are reserved, treat them with care */
			oldmsr &= ~0xffff;
			msr &= 0xffff;
			oldmsr |= msr;
		}

		wrmsr_on_cpu(good_cpu, MSR_IA32_PERF_CTL, oldmsr, h);
		if (policy->shared_type == CPUFREQ_SHARED_TYPE_ANY)
			break;

		cpumask_set_cpu(j, covered_cpus);
	}

	if (unlikely(retval)) {
		/*
		 * We have failed halfway through the frequency change.
		 * We have sent callbacks to policy->cpus and
		 * MSRs have already been written on coverd_cpus.
		 * Best effort undo..
		 */

		for_each_cpu(j, covered_cpus)
			wrmsr_on_cpu(j, MSR_IA32_PERF_CTL, oldmsr, h);
	}
	retval = 0;

out:
	free_cpumask_var(covered_cpus);
	return retval;
}

static struct cpufreq_driver centrino_driver = {
	.name		= "centrino", /* should be speedstep-centrino,
					 but there's a 16 char limit */
	.init		= centrino_cpu_init,
	.exit		= centrino_cpu_exit,
	.verify		= cpufreq_generic_frequency_table_verify,
	.target_index	= centrino_target,
	.get		= get_cur_freq,
	.attr		= cpufreq_generic_attr,
};

/*
 * This doesn't replace the detailed checks above because
 * the generic CPU IDs don't have a way to match for steppings
 * or ASCII model IDs.
 */
static const struct x86_cpu_id centrino_ids[] = {
	X86_MATCH_VENDOR_FAM_MODEL_FEATURE(INTEL,  6,  9, X86_FEATURE_EST, NULL),
	X86_MATCH_VENDOR_FAM_MODEL_FEATURE(INTEL,  6, 13, X86_FEATURE_EST, NULL),
	X86_MATCH_VENDOR_FAM_MODEL_FEATURE(INTEL, 15,  3, X86_FEATURE_EST, NULL),
	X86_MATCH_VENDOR_FAM_MODEL_FEATURE(INTEL, 15,  4, X86_FEATURE_EST, NULL),
	{}
};

/**
 * centrino_init - initializes the Enhanced SpeedStep CPUFreq driver
 *
 * Initializes the Enhanced SpeedStep support. Returns -ENODEV on
 * unsupported devices, -ENOENT if there's no voltage table for this
 * particular CPU model, -EINVAL on problems during initiatization,
 * and zero on success.
 *
 * This is quite picky.  Not only does the CPU have to advertise the
 * "est" flag in the cpuid capability flags, we look for a specific
 * CPU model and stepping, and we need to have the exact model name in
 * our voltage tables.  That is, be paranoid about not releasing
 * someone's valuable magic smoke.
 */
static int __init centrino_init(void)
{
	if (!x86_match_cpu(centrino_ids))
		return -ENODEV;
	return cpufreq_register_driver(&centrino_driver);
}

static void __exit centrino_exit(void)
{
	cpufreq_unregister_driver(&centrino_driver);
}

MODULE_AUTHOR ("Jeremy Fitzhardinge <jeremy@goop.org>");
MODULE_DESCRIPTION ("Enhanced SpeedStep driver for Intel Pentium M processors.");
MODULE_LICENSE ("GPL");

late_initcall(centrino_init);
module_exit(centrino_exit);
