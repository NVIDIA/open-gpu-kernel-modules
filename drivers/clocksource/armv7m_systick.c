// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) Maxime Coquelin 2015
 * Author:  Maxime Coquelin <mcoquelin.stm32@gmail.com>
 */

#include <linux/kernel.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/clk.h>
#include <linux/bitops.h>

#define SYST_CSR	0x00
#define SYST_RVR	0x04
#define SYST_CVR	0x08
#define SYST_CALIB	0x0c

#define SYST_CSR_ENABLE BIT(0)

#define SYSTICK_LOAD_RELOAD_MASK 0x00FFFFFF

static int __init system_timer_of_register(struct device_node *np)
{
	struct clk *clk = NULL;
	void __iomem *base;
	u32 rate;
	int ret;

	base = of_iomap(np, 0);
	if (!base) {
		pr_warn("system-timer: invalid base address\n");
		return -ENXIO;
	}

	ret = of_property_read_u32(np, "clock-frequency", &rate);
	if (ret) {
		clk = of_clk_get(np, 0);
		if (IS_ERR(clk)) {
			ret = PTR_ERR(clk);
			goto out_unmap;
		}

		ret = clk_prepare_enable(clk);
		if (ret)
			goto out_clk_put;

		rate = clk_get_rate(clk);
		if (!rate) {
			ret = -EINVAL;
			goto out_clk_disable;
		}
	}

	writel_relaxed(SYSTICK_LOAD_RELOAD_MASK, base + SYST_RVR);
	writel_relaxed(SYST_CSR_ENABLE, base + SYST_CSR);

	ret = clocksource_mmio_init(base + SYST_CVR, "arm_system_timer", rate,
			200, 24, clocksource_mmio_readl_down);
	if (ret) {
		pr_err("failed to init clocksource (%d)\n", ret);
		if (clk)
			goto out_clk_disable;
		else
			goto out_unmap;
	}

	pr_info("ARM System timer initialized as clocksource\n");

	return 0;

out_clk_disable:
	clk_disable_unprepare(clk);
out_clk_put:
	clk_put(clk);
out_unmap:
	iounmap(base);
	pr_warn("ARM System timer register failed (%d)\n", ret);

	return ret;
}

TIMER_OF_DECLARE(arm_systick, "arm,armv7m-systick",
			system_timer_of_register);
