// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2012 Calxeda, Inc.
 *
 * Based on arch/arm/plat-mxc/cpuidle.c: #v3.7
 * Copyright 2012 Freescale Semiconductor, Inc.
 * Copyright 2012 Linaro Ltd.
 *
 * Maintainer: Rob Herring <rob.herring@calxeda.com>
 */

#include <linux/cpuidle.h>
#include <linux/cpu_pm.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/platform_device.h>
#include <linux/psci.h>

#include <asm/cpuidle.h>
#include <asm/suspend.h>

#include <uapi/linux/psci.h>

#define CALXEDA_IDLE_PARAM \
	((0 << PSCI_0_2_POWER_STATE_ID_SHIFT) | \
	 (0 << PSCI_0_2_POWER_STATE_AFFL_SHIFT) | \
	 (PSCI_POWER_STATE_TYPE_POWER_DOWN << PSCI_0_2_POWER_STATE_TYPE_SHIFT))

static int calxeda_idle_finish(unsigned long val)
{
	return psci_ops.cpu_suspend(CALXEDA_IDLE_PARAM, __pa(cpu_resume));
}

static int calxeda_pwrdown_idle(struct cpuidle_device *dev,
				struct cpuidle_driver *drv,
				int index)
{
	cpu_pm_enter();
	cpu_suspend(0, calxeda_idle_finish);
	cpu_pm_exit();

	return index;
}

static struct cpuidle_driver calxeda_idle_driver = {
	.name = "calxeda_idle",
	.states = {
		ARM_CPUIDLE_WFI_STATE,
		{
			.name = "PG",
			.desc = "Power Gate",
			.exit_latency = 30,
			.power_usage = 50,
			.target_residency = 200,
			.enter = calxeda_pwrdown_idle,
		},
	},
	.state_count = 2,
};

static int calxeda_cpuidle_probe(struct platform_device *pdev)
{
	return cpuidle_register(&calxeda_idle_driver, NULL);
}

static struct platform_driver calxeda_cpuidle_plat_driver = {
        .driver = {
                .name = "cpuidle-calxeda",
        },
        .probe = calxeda_cpuidle_probe,
};
builtin_platform_driver(calxeda_cpuidle_plat_driver);
