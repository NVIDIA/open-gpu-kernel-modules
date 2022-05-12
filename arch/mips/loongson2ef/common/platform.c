// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2009 Lemote Inc.
 * Author: Wu Zhangjin, wuzhangjin@gmail.com
 */

#include <linux/err.h>
#include <linux/smp.h>
#include <linux/platform_device.h>

static struct platform_device loongson2_cpufreq_device = {
	.name = "loongson2_cpufreq",
	.id = -1,
};

static int __init loongson2_cpufreq_init(void)
{
	struct cpuinfo_mips *c = &current_cpu_data;

	/* Only 2F revision and it's successors support CPUFreq */
	if ((c->processor_id & PRID_REV_MASK) >= PRID_REV_LOONGSON2F)
		return platform_device_register(&loongson2_cpufreq_device);

	return -ENODEV;
}

arch_initcall(loongson2_cpufreq_init);
