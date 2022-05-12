// SPDX-License-Identifier: GPL-2.0-only
/*
 * Power Management driver for Marvell Kirkwood SoCs
 *
 * Copyright (C) 2013 Ezequiel Garcia <ezequiel@free-electrons.com>
 * Copyright (C) 2010 Simon Guinot <sguinot@lacie.com>
 */

#include <linux/kernel.h>
#include <linux/suspend.h>
#include <linux/io.h>
#include "kirkwood.h"
#include "kirkwood-pm.h"

static void __iomem *ddr_operation_base;
static void __iomem *memory_pm_ctrl;

static void kirkwood_low_power(void)
{
	u32 mem_pm_ctrl;

	mem_pm_ctrl = readl(memory_pm_ctrl);

	/* Set peripherals to low-power mode */
	writel_relaxed(~0, memory_pm_ctrl);

	/* Set DDR in self-refresh */
	writel_relaxed(0x7, ddr_operation_base);

	/*
	 * Set CPU in wait-for-interrupt state.
	 * This disables the CPU core clocks,
	 * the array clocks, and also the L2 controller.
	 */
	cpu_do_idle();

	writel_relaxed(mem_pm_ctrl, memory_pm_ctrl);
}

static int kirkwood_suspend_enter(suspend_state_t state)
{
	switch (state) {
	case PM_SUSPEND_STANDBY:
		kirkwood_low_power();
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int kirkwood_pm_valid_standby(suspend_state_t state)
{
	return state == PM_SUSPEND_STANDBY;
}

static const struct platform_suspend_ops kirkwood_suspend_ops = {
	.enter = kirkwood_suspend_enter,
	.valid = kirkwood_pm_valid_standby,
};

void __init kirkwood_pm_init(void)
{
	ddr_operation_base = ioremap(DDR_OPERATION_BASE, 4);
	memory_pm_ctrl = ioremap(MEMORY_PM_CTRL_PHYS, 4);

	suspend_set_ops(&kirkwood_suspend_ops);
}
