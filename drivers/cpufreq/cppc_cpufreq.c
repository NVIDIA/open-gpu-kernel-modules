// SPDX-License-Identifier: GPL-2.0-only
/*
 * CPPC (Collaborative Processor Performance Control) driver for
 * interfacing with the CPUfreq layer and governors. See
 * cppc_acpi.c for CPPC specific methods.
 *
 * (C) Copyright 2014, 2015 Linaro Ltd.
 * Author: Ashwin Chaugule <ashwin.chaugule@linaro.org>
 */

#define pr_fmt(fmt)	"CPPC Cpufreq:"	fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/dmi.h>
#include <linux/time.h>
#include <linux/vmalloc.h>

#include <asm/unaligned.h>

#include <acpi/cppc_acpi.h>

/* Minimum struct length needed for the DMI processor entry we want */
#define DMI_ENTRY_PROCESSOR_MIN_LENGTH	48

/* Offset in the DMI processor structure for the max frequency */
#define DMI_PROCESSOR_MAX_SPEED		0x14

/*
 * This list contains information parsed from per CPU ACPI _CPC and _PSD
 * structures: e.g. the highest and lowest supported performance, capabilities,
 * desired performance, level requested etc. Depending on the share_type, not
 * all CPUs will have an entry in the list.
 */
static LIST_HEAD(cpu_data_list);

static bool boost_supported;

struct cppc_workaround_oem_info {
	char oem_id[ACPI_OEM_ID_SIZE + 1];
	char oem_table_id[ACPI_OEM_TABLE_ID_SIZE + 1];
	u32 oem_revision;
};

static struct cppc_workaround_oem_info wa_info[] = {
	{
		.oem_id		= "HISI  ",
		.oem_table_id	= "HIP07   ",
		.oem_revision	= 0,
	}, {
		.oem_id		= "HISI  ",
		.oem_table_id	= "HIP08   ",
		.oem_revision	= 0,
	}
};

/* Callback function used to retrieve the max frequency from DMI */
static void cppc_find_dmi_mhz(const struct dmi_header *dm, void *private)
{
	const u8 *dmi_data = (const u8 *)dm;
	u16 *mhz = (u16 *)private;

	if (dm->type == DMI_ENTRY_PROCESSOR &&
	    dm->length >= DMI_ENTRY_PROCESSOR_MIN_LENGTH) {
		u16 val = (u16)get_unaligned((const u16 *)
				(dmi_data + DMI_PROCESSOR_MAX_SPEED));
		*mhz = val > *mhz ? val : *mhz;
	}
}

/* Look up the max frequency in DMI */
static u64 cppc_get_dmi_max_khz(void)
{
	u16 mhz = 0;

	dmi_walk(cppc_find_dmi_mhz, &mhz);

	/*
	 * Real stupid fallback value, just in case there is no
	 * actual value set.
	 */
	mhz = mhz ? mhz : 1;

	return (1000 * mhz);
}

/*
 * If CPPC lowest_freq and nominal_freq registers are exposed then we can
 * use them to convert perf to freq and vice versa
 *
 * If the perf/freq point lies between Nominal and Lowest, we can treat
 * (Low perf, Low freq) and (Nom Perf, Nom freq) as 2D co-ordinates of a line
 * and extrapolate the rest
 * For perf/freq > Nominal, we use the ratio perf:freq at Nominal for conversion
 */
static unsigned int cppc_cpufreq_perf_to_khz(struct cppc_cpudata *cpu_data,
					     unsigned int perf)
{
	struct cppc_perf_caps *caps = &cpu_data->perf_caps;
	static u64 max_khz;
	u64 mul, div;

	if (caps->lowest_freq && caps->nominal_freq) {
		if (perf >= caps->nominal_perf) {
			mul = caps->nominal_freq;
			div = caps->nominal_perf;
		} else {
			mul = caps->nominal_freq - caps->lowest_freq;
			div = caps->nominal_perf - caps->lowest_perf;
		}
	} else {
		if (!max_khz)
			max_khz = cppc_get_dmi_max_khz();
		mul = max_khz;
		div = caps->highest_perf;
	}
	return (u64)perf * mul / div;
}

static unsigned int cppc_cpufreq_khz_to_perf(struct cppc_cpudata *cpu_data,
					     unsigned int freq)
{
	struct cppc_perf_caps *caps = &cpu_data->perf_caps;
	static u64 max_khz;
	u64  mul, div;

	if (caps->lowest_freq && caps->nominal_freq) {
		if (freq >= caps->nominal_freq) {
			mul = caps->nominal_perf;
			div = caps->nominal_freq;
		} else {
			mul = caps->lowest_perf;
			div = caps->lowest_freq;
		}
	} else {
		if (!max_khz)
			max_khz = cppc_get_dmi_max_khz();
		mul = caps->highest_perf;
		div = max_khz;
	}

	return (u64)freq * mul / div;
}

static int cppc_cpufreq_set_target(struct cpufreq_policy *policy,
				   unsigned int target_freq,
				   unsigned int relation)

{
	struct cppc_cpudata *cpu_data = policy->driver_data;
	unsigned int cpu = policy->cpu;
	struct cpufreq_freqs freqs;
	u32 desired_perf;
	int ret = 0;

	desired_perf = cppc_cpufreq_khz_to_perf(cpu_data, target_freq);
	/* Return if it is exactly the same perf */
	if (desired_perf == cpu_data->perf_ctrls.desired_perf)
		return ret;

	cpu_data->perf_ctrls.desired_perf = desired_perf;
	freqs.old = policy->cur;
	freqs.new = target_freq;

	cpufreq_freq_transition_begin(policy, &freqs);
	ret = cppc_set_perf(cpu, &cpu_data->perf_ctrls);
	cpufreq_freq_transition_end(policy, &freqs, ret != 0);

	if (ret)
		pr_debug("Failed to set target on CPU:%d. ret:%d\n",
			 cpu, ret);

	return ret;
}

static int cppc_verify_policy(struct cpufreq_policy_data *policy)
{
	cpufreq_verify_within_cpu_limits(policy);
	return 0;
}

static void cppc_cpufreq_stop_cpu(struct cpufreq_policy *policy)
{
	struct cppc_cpudata *cpu_data = policy->driver_data;
	struct cppc_perf_caps *caps = &cpu_data->perf_caps;
	unsigned int cpu = policy->cpu;
	int ret;

	cpu_data->perf_ctrls.desired_perf = caps->lowest_perf;

	ret = cppc_set_perf(cpu, &cpu_data->perf_ctrls);
	if (ret)
		pr_debug("Err setting perf value:%d on CPU:%d. ret:%d\n",
			 caps->lowest_perf, cpu, ret);

	/* Remove CPU node from list and free driver data for policy */
	free_cpumask_var(cpu_data->shared_cpu_map);
	list_del(&cpu_data->node);
	kfree(policy->driver_data);
	policy->driver_data = NULL;
}

/*
 * The PCC subspace describes the rate at which platform can accept commands
 * on the shared PCC channel (including READs which do not count towards freq
 * transition requests), so ideally we need to use the PCC values as a fallback
 * if we don't have a platform specific transition_delay_us
 */
#ifdef CONFIG_ARM64
#include <asm/cputype.h>

static unsigned int cppc_cpufreq_get_transition_delay_us(unsigned int cpu)
{
	unsigned long implementor = read_cpuid_implementor();
	unsigned long part_num = read_cpuid_part_number();

	switch (implementor) {
	case ARM_CPU_IMP_QCOM:
		switch (part_num) {
		case QCOM_CPU_PART_FALKOR_V1:
		case QCOM_CPU_PART_FALKOR:
			return 10000;
		}
	}
	return cppc_get_transition_latency(cpu) / NSEC_PER_USEC;
}

#else

static unsigned int cppc_cpufreq_get_transition_delay_us(unsigned int cpu)
{
	return cppc_get_transition_latency(cpu) / NSEC_PER_USEC;
}
#endif


static struct cppc_cpudata *cppc_cpufreq_get_cpu_data(unsigned int cpu)
{
	struct cppc_cpudata *cpu_data;
	int ret;

	cpu_data = kzalloc(sizeof(struct cppc_cpudata), GFP_KERNEL);
	if (!cpu_data)
		goto out;

	if (!zalloc_cpumask_var(&cpu_data->shared_cpu_map, GFP_KERNEL))
		goto free_cpu;

	ret = acpi_get_psd_map(cpu, cpu_data);
	if (ret) {
		pr_debug("Err parsing CPU%d PSD data: ret:%d\n", cpu, ret);
		goto free_mask;
	}

	ret = cppc_get_perf_caps(cpu, &cpu_data->perf_caps);
	if (ret) {
		pr_debug("Err reading CPU%d perf caps: ret:%d\n", cpu, ret);
		goto free_mask;
	}

	/* Convert the lowest and nominal freq from MHz to KHz */
	cpu_data->perf_caps.lowest_freq *= 1000;
	cpu_data->perf_caps.nominal_freq *= 1000;

	list_add(&cpu_data->node, &cpu_data_list);

	return cpu_data;

free_mask:
	free_cpumask_var(cpu_data->shared_cpu_map);
free_cpu:
	kfree(cpu_data);
out:
	return NULL;
}

static int cppc_cpufreq_cpu_init(struct cpufreq_policy *policy)
{
	unsigned int cpu = policy->cpu;
	struct cppc_cpudata *cpu_data;
	struct cppc_perf_caps *caps;
	int ret;

	cpu_data = cppc_cpufreq_get_cpu_data(cpu);
	if (!cpu_data) {
		pr_err("Error in acquiring _CPC/_PSD data for CPU%d.\n", cpu);
		return -ENODEV;
	}
	caps = &cpu_data->perf_caps;
	policy->driver_data = cpu_data;

	/*
	 * Set min to lowest nonlinear perf to avoid any efficiency penalty (see
	 * Section 8.4.7.1.1.5 of ACPI 6.1 spec)
	 */
	policy->min = cppc_cpufreq_perf_to_khz(cpu_data,
					       caps->lowest_nonlinear_perf);
	policy->max = cppc_cpufreq_perf_to_khz(cpu_data,
					       caps->nominal_perf);

	/*
	 * Set cpuinfo.min_freq to Lowest to make the full range of performance
	 * available if userspace wants to use any perf between lowest & lowest
	 * nonlinear perf
	 */
	policy->cpuinfo.min_freq = cppc_cpufreq_perf_to_khz(cpu_data,
							    caps->lowest_perf);
	policy->cpuinfo.max_freq = cppc_cpufreq_perf_to_khz(cpu_data,
							    caps->nominal_perf);

	policy->transition_delay_us = cppc_cpufreq_get_transition_delay_us(cpu);
	policy->shared_type = cpu_data->shared_type;

	switch (policy->shared_type) {
	case CPUFREQ_SHARED_TYPE_HW:
	case CPUFREQ_SHARED_TYPE_NONE:
		/* Nothing to be done - we'll have a policy for each CPU */
		break;
	case CPUFREQ_SHARED_TYPE_ANY:
		/*
		 * All CPUs in the domain will share a policy and all cpufreq
		 * operations will use a single cppc_cpudata structure stored
		 * in policy->driver_data.
		 */
		cpumask_copy(policy->cpus, cpu_data->shared_cpu_map);
		break;
	default:
		pr_debug("Unsupported CPU co-ord type: %d\n",
			 policy->shared_type);
		return -EFAULT;
	}

	/*
	 * If 'highest_perf' is greater than 'nominal_perf', we assume CPU Boost
	 * is supported.
	 */
	if (caps->highest_perf > caps->nominal_perf)
		boost_supported = true;

	/* Set policy->cur to max now. The governors will adjust later. */
	policy->cur = cppc_cpufreq_perf_to_khz(cpu_data, caps->highest_perf);
	cpu_data->perf_ctrls.desired_perf =  caps->highest_perf;

	ret = cppc_set_perf(cpu, &cpu_data->perf_ctrls);
	if (ret)
		pr_debug("Err setting perf value:%d on CPU:%d. ret:%d\n",
			 caps->highest_perf, cpu, ret);

	return ret;
}

static inline u64 get_delta(u64 t1, u64 t0)
{
	if (t1 > t0 || t0 > ~(u32)0)
		return t1 - t0;

	return (u32)t1 - (u32)t0;
}

static int cppc_get_rate_from_fbctrs(struct cppc_cpudata *cpu_data,
				     struct cppc_perf_fb_ctrs fb_ctrs_t0,
				     struct cppc_perf_fb_ctrs fb_ctrs_t1)
{
	u64 delta_reference, delta_delivered;
	u64 reference_perf, delivered_perf;

	reference_perf = fb_ctrs_t0.reference_perf;

	delta_reference = get_delta(fb_ctrs_t1.reference,
				    fb_ctrs_t0.reference);
	delta_delivered = get_delta(fb_ctrs_t1.delivered,
				    fb_ctrs_t0.delivered);

	/* Check to avoid divide-by zero */
	if (delta_reference || delta_delivered)
		delivered_perf = (reference_perf * delta_delivered) /
					delta_reference;
	else
		delivered_perf = cpu_data->perf_ctrls.desired_perf;

	return cppc_cpufreq_perf_to_khz(cpu_data, delivered_perf);
}

static unsigned int cppc_cpufreq_get_rate(unsigned int cpu)
{
	struct cppc_perf_fb_ctrs fb_ctrs_t0 = {0}, fb_ctrs_t1 = {0};
	struct cpufreq_policy *policy = cpufreq_cpu_get(cpu);
	struct cppc_cpudata *cpu_data = policy->driver_data;
	int ret;

	cpufreq_cpu_put(policy);

	ret = cppc_get_perf_ctrs(cpu, &fb_ctrs_t0);
	if (ret)
		return ret;

	udelay(2); /* 2usec delay between sampling */

	ret = cppc_get_perf_ctrs(cpu, &fb_ctrs_t1);
	if (ret)
		return ret;

	return cppc_get_rate_from_fbctrs(cpu_data, fb_ctrs_t0, fb_ctrs_t1);
}

static int cppc_cpufreq_set_boost(struct cpufreq_policy *policy, int state)
{
	struct cppc_cpudata *cpu_data = policy->driver_data;
	struct cppc_perf_caps *caps = &cpu_data->perf_caps;
	int ret;

	if (!boost_supported) {
		pr_err("BOOST not supported by CPU or firmware\n");
		return -EINVAL;
	}

	if (state)
		policy->max = cppc_cpufreq_perf_to_khz(cpu_data,
						       caps->highest_perf);
	else
		policy->max = cppc_cpufreq_perf_to_khz(cpu_data,
						       caps->nominal_perf);
	policy->cpuinfo.max_freq = policy->max;

	ret = freq_qos_update_request(policy->max_freq_req, policy->max);
	if (ret < 0)
		return ret;

	return 0;
}

static ssize_t show_freqdomain_cpus(struct cpufreq_policy *policy, char *buf)
{
	struct cppc_cpudata *cpu_data = policy->driver_data;

	return cpufreq_show_cpus(cpu_data->shared_cpu_map, buf);
}
cpufreq_freq_attr_ro(freqdomain_cpus);

static struct freq_attr *cppc_cpufreq_attr[] = {
	&freqdomain_cpus,
	NULL,
};

static struct cpufreq_driver cppc_cpufreq_driver = {
	.flags = CPUFREQ_CONST_LOOPS,
	.verify = cppc_verify_policy,
	.target = cppc_cpufreq_set_target,
	.get = cppc_cpufreq_get_rate,
	.init = cppc_cpufreq_cpu_init,
	.stop_cpu = cppc_cpufreq_stop_cpu,
	.set_boost = cppc_cpufreq_set_boost,
	.attr = cppc_cpufreq_attr,
	.name = "cppc_cpufreq",
};

/*
 * HISI platform does not support delivered performance counter and
 * reference performance counter. It can calculate the performance using the
 * platform specific mechanism. We reuse the desired performance register to
 * store the real performance calculated by the platform.
 */
static unsigned int hisi_cppc_cpufreq_get_rate(unsigned int cpu)
{
	struct cpufreq_policy *policy = cpufreq_cpu_get(cpu);
	struct cppc_cpudata *cpu_data = policy->driver_data;
	u64 desired_perf;
	int ret;

	cpufreq_cpu_put(policy);

	ret = cppc_get_desired_perf(cpu, &desired_perf);
	if (ret < 0)
		return -EIO;

	return cppc_cpufreq_perf_to_khz(cpu_data, desired_perf);
}

static void cppc_check_hisi_workaround(void)
{
	struct acpi_table_header *tbl;
	acpi_status status = AE_OK;
	int i;

	status = acpi_get_table(ACPI_SIG_PCCT, 0, &tbl);
	if (ACPI_FAILURE(status) || !tbl)
		return;

	for (i = 0; i < ARRAY_SIZE(wa_info); i++) {
		if (!memcmp(wa_info[i].oem_id, tbl->oem_id, ACPI_OEM_ID_SIZE) &&
		    !memcmp(wa_info[i].oem_table_id, tbl->oem_table_id, ACPI_OEM_TABLE_ID_SIZE) &&
		    wa_info[i].oem_revision == tbl->oem_revision) {
			/* Overwrite the get() callback */
			cppc_cpufreq_driver.get = hisi_cppc_cpufreq_get_rate;
			break;
		}
	}

	acpi_put_table(tbl);
}

static int __init cppc_cpufreq_init(void)
{
	if ((acpi_disabled) || !acpi_cpc_valid())
		return -ENODEV;

	INIT_LIST_HEAD(&cpu_data_list);

	cppc_check_hisi_workaround();

	return cpufreq_register_driver(&cppc_cpufreq_driver);
}

static inline void free_cpu_data(void)
{
	struct cppc_cpudata *iter, *tmp;

	list_for_each_entry_safe(iter, tmp, &cpu_data_list, node) {
		free_cpumask_var(iter->shared_cpu_map);
		list_del(&iter->node);
		kfree(iter);
	}

}

static void __exit cppc_cpufreq_exit(void)
{
	cpufreq_unregister_driver(&cppc_cpufreq_driver);

	free_cpu_data();
}

module_exit(cppc_cpufreq_exit);
MODULE_AUTHOR("Ashwin Chaugule");
MODULE_DESCRIPTION("CPUFreq driver based on the ACPI CPPC v5.0+ spec");
MODULE_LICENSE("GPL");

late_initcall(cppc_cpufreq_init);

static const struct acpi_device_id cppc_acpi_ids[] __used = {
	{ACPI_PROCESSOR_DEVICE_HID, },
	{}
};

MODULE_DEVICE_TABLE(acpi, cppc_acpi_ids);
