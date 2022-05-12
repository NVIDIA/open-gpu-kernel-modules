// SPDX-License-Identifier: GPL-2.0-only
/*
 * Ralink MT7621/MT7628 built-in hardware watchdog timer
 *
 * Copyright (C) 2014 John Crispin <john@phrozen.org>
 *
 * This driver was based on: drivers/watchdog/rt2880_wdt.c
 */

#include <linux/clk.h>
#include <linux/reset.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/watchdog.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>

#include <asm/mach-ralink/ralink_regs.h>

#define SYSC_RSTSTAT			0x38
#define WDT_RST_CAUSE			BIT(1)

#define RALINK_WDT_TIMEOUT		30

#define TIMER_REG_TMRSTAT		0x00
#define TIMER_REG_TMR1LOAD		0x24
#define TIMER_REG_TMR1CTL		0x20

#define TMR1CTL_ENABLE			BIT(7)
#define TMR1CTL_RESTART			BIT(9)
#define TMR1CTL_PRESCALE_SHIFT		16

static void __iomem *mt7621_wdt_base;
static struct reset_control *mt7621_wdt_reset;

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout,
		 "Watchdog cannot be stopped once started (default="
		 __MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

static inline void rt_wdt_w32(unsigned reg, u32 val)
{
	iowrite32(val, mt7621_wdt_base + reg);
}

static inline u32 rt_wdt_r32(unsigned reg)
{
	return ioread32(mt7621_wdt_base + reg);
}

static int mt7621_wdt_ping(struct watchdog_device *w)
{
	rt_wdt_w32(TIMER_REG_TMRSTAT, TMR1CTL_RESTART);

	return 0;
}

static int mt7621_wdt_set_timeout(struct watchdog_device *w, unsigned int t)
{
	w->timeout = t;
	rt_wdt_w32(TIMER_REG_TMR1LOAD, t * 1000);
	mt7621_wdt_ping(w);

	return 0;
}

static int mt7621_wdt_start(struct watchdog_device *w)
{
	u32 t;

	/* set the prescaler to 1ms == 1000us */
	rt_wdt_w32(TIMER_REG_TMR1CTL, 1000 << TMR1CTL_PRESCALE_SHIFT);

	mt7621_wdt_set_timeout(w, w->timeout);

	t = rt_wdt_r32(TIMER_REG_TMR1CTL);
	t |= TMR1CTL_ENABLE;
	rt_wdt_w32(TIMER_REG_TMR1CTL, t);

	return 0;
}

static int mt7621_wdt_stop(struct watchdog_device *w)
{
	u32 t;

	mt7621_wdt_ping(w);

	t = rt_wdt_r32(TIMER_REG_TMR1CTL);
	t &= ~TMR1CTL_ENABLE;
	rt_wdt_w32(TIMER_REG_TMR1CTL, t);

	return 0;
}

static int mt7621_wdt_bootcause(void)
{
	if (rt_sysc_r32(SYSC_RSTSTAT) & WDT_RST_CAUSE)
		return WDIOF_CARDRESET;

	return 0;
}

static int mt7621_wdt_is_running(struct watchdog_device *w)
{
	return !!(rt_wdt_r32(TIMER_REG_TMR1CTL) & TMR1CTL_ENABLE);
}

static const struct watchdog_info mt7621_wdt_info = {
	.identity = "Mediatek Watchdog",
	.options = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE,
};

static const struct watchdog_ops mt7621_wdt_ops = {
	.owner = THIS_MODULE,
	.start = mt7621_wdt_start,
	.stop = mt7621_wdt_stop,
	.ping = mt7621_wdt_ping,
	.set_timeout = mt7621_wdt_set_timeout,
};

static struct watchdog_device mt7621_wdt_dev = {
	.info = &mt7621_wdt_info,
	.ops = &mt7621_wdt_ops,
	.min_timeout = 1,
	.max_timeout = 0xfffful / 1000,
};

static int mt7621_wdt_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	mt7621_wdt_base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(mt7621_wdt_base))
		return PTR_ERR(mt7621_wdt_base);

	mt7621_wdt_reset = devm_reset_control_get_exclusive(dev, NULL);
	if (!IS_ERR(mt7621_wdt_reset))
		reset_control_deassert(mt7621_wdt_reset);

	mt7621_wdt_dev.bootstatus = mt7621_wdt_bootcause();

	watchdog_init_timeout(&mt7621_wdt_dev, mt7621_wdt_dev.max_timeout,
			      dev);
	watchdog_set_nowayout(&mt7621_wdt_dev, nowayout);
	if (mt7621_wdt_is_running(&mt7621_wdt_dev)) {
		/*
		 * Make sure to apply timeout from watchdog core, taking
		 * the prescaler of this driver here into account (the
		 * boot loader might be using a different prescaler).
		 *
		 * To avoid spurious resets because of different scaling,
		 * we first disable the watchdog, set the new prescaler
		 * and timeout, and then re-enable the watchdog.
		 */
		mt7621_wdt_stop(&mt7621_wdt_dev);
		mt7621_wdt_start(&mt7621_wdt_dev);
		set_bit(WDOG_HW_RUNNING, &mt7621_wdt_dev.status);
	}

	return devm_watchdog_register_device(dev, &mt7621_wdt_dev);
}

static void mt7621_wdt_shutdown(struct platform_device *pdev)
{
	mt7621_wdt_stop(&mt7621_wdt_dev);
}

static const struct of_device_id mt7621_wdt_match[] = {
	{ .compatible = "mediatek,mt7621-wdt" },
	{},
};
MODULE_DEVICE_TABLE(of, mt7621_wdt_match);

static struct platform_driver mt7621_wdt_driver = {
	.probe		= mt7621_wdt_probe,
	.shutdown	= mt7621_wdt_shutdown,
	.driver		= {
		.name		= KBUILD_MODNAME,
		.of_match_table	= mt7621_wdt_match,
	},
};

module_platform_driver(mt7621_wdt_driver);

MODULE_DESCRIPTION("MediaTek MT762x hardware watchdog driver");
MODULE_AUTHOR("John Crispin <john@phrozen.org");
MODULE_LICENSE("GPL v2");
