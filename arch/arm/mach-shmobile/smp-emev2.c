// SPDX-License-Identifier: GPL-2.0
/*
 * SMP support for Emma Mobile EV2
 *
 * Copyright (C) 2012  Renesas Solutions Corp.
 * Copyright (C) 2012  Magnus Damm
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/smp.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <asm/smp_plat.h>
#include <asm/smp_scu.h>

#include "common.h"
#include "emev2.h"

#define EMEV2_SCU_BASE 0x1e000000
#define EMEV2_SMU_BASE 0xe0110000
#define SMU_GENERAL_REG0 0x7c0

static int emev2_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	arch_send_wakeup_ipi_mask(cpumask_of(cpu_logical_map(cpu)));
	return 0;
}

static void __init emev2_smp_prepare_cpus(unsigned int max_cpus)
{
	void __iomem *smu;

	/* Tell ROM loader about our vector (in headsmp.S) */
	smu = ioremap(EMEV2_SMU_BASE, PAGE_SIZE);
	if (smu) {
		iowrite32(__pa(shmobile_boot_vector), smu + SMU_GENERAL_REG0);
		iounmap(smu);
	}

	/* setup EMEV2 specific SCU bits */
	shmobile_smp_scu_prepare_cpus(EMEV2_SCU_BASE, max_cpus);
}

const struct smp_operations emev2_smp_ops __initconst = {
	.smp_prepare_cpus	= emev2_smp_prepare_cpus,
	.smp_boot_secondary	= emev2_boot_secondary,
};
