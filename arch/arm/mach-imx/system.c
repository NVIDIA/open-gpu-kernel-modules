// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 1999 ARM Limited
 * Copyright (C) 2000 Deep Blue Solutions Ltd
 * Copyright 2006-2007 Freescale Semiconductor, Inc. All Rights Reserved.
 * Copyright 2008 Juergen Beisert, kernel@pengutronix.de
 * Copyright 2009 Ilya Yanok, Emcraft Systems Ltd, yanok@emcraft.com
 */

#include <linux/kernel.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_address.h>

#include <asm/system_misc.h>
#include <asm/proc-fns.h>
#include <asm/mach-types.h>
#include <asm/hardware/cache-l2x0.h>

#include "common.h"
#include "hardware.h"

static void __iomem *wdog_base;
static struct clk *wdog_clk;
static int wcr_enable = (1 << 2);

/*
 * Reset the system. It is called by machine_restart().
 */
void mxc_restart(enum reboot_mode mode, const char *cmd)
{
	if (!wdog_base)
		goto reset_fallback;

	if (!IS_ERR(wdog_clk))
		clk_enable(wdog_clk);

	/* Assert SRS signal */
	imx_writew(wcr_enable, wdog_base);
	/*
	 * Due to imx6q errata ERR004346 (WDOG: WDOG SRS bit requires to be
	 * written twice), we add another two writes to ensure there must be at
	 * least two writes happen in the same one 32kHz clock period.  We save
	 * the target check here, since the writes shouldn't be a huge burden
	 * for other platforms.
	 */
	imx_writew(wcr_enable, wdog_base);
	imx_writew(wcr_enable, wdog_base);

	/* wait for reset to assert... */
	mdelay(500);

	pr_err("%s: Watchdog reset failed to assert reset\n", __func__);

	/* delay to allow the serial port to show the message */
	mdelay(50);

reset_fallback:
	/* we'll take a jump through zero as a poor second */
	soft_restart(0);
}

void __init mxc_arch_reset_init(void __iomem *base)
{
	wdog_base = base;

	wdog_clk = clk_get_sys("imx2-wdt.0", NULL);
	if (IS_ERR(wdog_clk))
		pr_warn("%s: failed to get wdog clock\n", __func__);
	else
		clk_prepare(wdog_clk);
}

#ifdef CONFIG_SOC_IMX1
void __init imx1_reset_init(void __iomem *base)
{
	wcr_enable = (1 << 0);
	mxc_arch_reset_init(base);
}
#endif

#ifdef CONFIG_CACHE_L2X0
void __init imx_init_l2cache(void)
{
	void __iomem *l2x0_base;
	struct device_node *np;
	unsigned int val;

	np = of_find_compatible_node(NULL, NULL, "arm,pl310-cache");
	if (!np)
		return;

	l2x0_base = of_iomap(np, 0);
	if (!l2x0_base)
		goto put_node;

	if (!(readl_relaxed(l2x0_base + L2X0_CTRL) & L2X0_CTRL_EN)) {
		/* Configure the L2 PREFETCH and POWER registers */
		val = readl_relaxed(l2x0_base + L310_PREFETCH_CTRL);
		val |= L310_PREFETCH_CTRL_DBL_LINEFILL |
			L310_PREFETCH_CTRL_INSTR_PREFETCH |
			L310_PREFETCH_CTRL_DATA_PREFETCH;

		/* Set perfetch offset to improve performance */
		val &= ~L310_PREFETCH_CTRL_OFFSET_MASK;
		val |= 15;

		writel_relaxed(val, l2x0_base + L310_PREFETCH_CTRL);
	}

	iounmap(l2x0_base);
put_node:
	of_node_put(np);
}
#endif
