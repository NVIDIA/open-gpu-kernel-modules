// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2011 Calxeda, Inc.
 */

#include <linux/cpu_pm.h>
#include <linux/init.h>
#include <linux/psci.h>
#include <linux/suspend.h>

#include <asm/suspend.h>

#include <uapi/linux/psci.h>

#define HIGHBANK_SUSPEND_PARAM \
	((0 << PSCI_0_2_POWER_STATE_ID_SHIFT) | \
	 (1 << PSCI_0_2_POWER_STATE_AFFL_SHIFT) | \
	 (PSCI_POWER_STATE_TYPE_POWER_DOWN << PSCI_0_2_POWER_STATE_TYPE_SHIFT))

static int highbank_suspend_finish(unsigned long val)
{
	return psci_ops.cpu_suspend(HIGHBANK_SUSPEND_PARAM, __pa(cpu_resume));
}

static int highbank_pm_enter(suspend_state_t state)
{
	cpu_pm_enter();
	cpu_cluster_pm_enter();

	cpu_suspend(0, highbank_suspend_finish);

	cpu_cluster_pm_exit();
	cpu_pm_exit();

	return 0;
}

static const struct platform_suspend_ops highbank_pm_ops = {
	.enter = highbank_pm_enter,
	.valid = suspend_valid_only_mem,
};

void __init highbank_pm_init(void)
{
	if (!psci_ops.cpu_suspend)
		return;

	suspend_set_ops(&highbank_pm_ops);
}
