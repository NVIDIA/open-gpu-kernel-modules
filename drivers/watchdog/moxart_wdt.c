/*
 * MOXA ART SoCs watchdog driver.
 *
 * Copyright (C) 2013 Jonas Jensen
 *
 * Jonas Jensen <jonas.jensen@gmail.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/clk.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/watchdog.h>
#include <linux/moduleparam.h>

#define REG_COUNT			0x4
#define REG_MODE			0x8
#define REG_ENABLE			0xC

struct moxart_wdt_dev {
	struct watchdog_device dev;
	void __iomem *base;
	unsigned int clock_frequency;
};

static int heartbeat;

static int moxart_wdt_restart(struct watchdog_device *wdt_dev,
			      unsigned long action, void *data)
{
	struct moxart_wdt_dev *moxart_wdt = watchdog_get_drvdata(wdt_dev);

	writel(1, moxart_wdt->base + REG_COUNT);
	writel(0x5ab9, moxart_wdt->base + REG_MODE);
	writel(0x03, moxart_wdt->base + REG_ENABLE);

	return 0;
}

static int moxart_wdt_stop(struct watchdog_device *wdt_dev)
{
	struct moxart_wdt_dev *moxart_wdt = watchdog_get_drvdata(wdt_dev);

	writel(0, moxart_wdt->base + REG_ENABLE);

	return 0;
}

static int moxart_wdt_start(struct watchdog_device *wdt_dev)
{
	struct moxart_wdt_dev *moxart_wdt = watchdog_get_drvdata(wdt_dev);

	writel(moxart_wdt->clock_frequency * wdt_dev->timeout,
	       moxart_wdt->base + REG_COUNT);
	writel(0x5ab9, moxart_wdt->base + REG_MODE);
	writel(0x03, moxart_wdt->base + REG_ENABLE);

	return 0;
}

static int moxart_wdt_set_timeout(struct watchdog_device *wdt_dev,
				  unsigned int timeout)
{
	wdt_dev->timeout = timeout;

	return 0;
}

static const struct watchdog_info moxart_wdt_info = {
	.identity       = "moxart-wdt",
	.options        = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING |
			  WDIOF_MAGICCLOSE,
};

static const struct watchdog_ops moxart_wdt_ops = {
	.owner          = THIS_MODULE,
	.start          = moxart_wdt_start,
	.stop           = moxart_wdt_stop,
	.set_timeout    = moxart_wdt_set_timeout,
	.restart        = moxart_wdt_restart,
};

static int moxart_wdt_probe(struct platform_device *pdev)
{
	struct moxart_wdt_dev *moxart_wdt;
	struct device *dev = &pdev->dev;
	struct clk *clk;
	int err;
	unsigned int max_timeout;
	bool nowayout = WATCHDOG_NOWAYOUT;

	moxart_wdt = devm_kzalloc(dev, sizeof(*moxart_wdt), GFP_KERNEL);
	if (!moxart_wdt)
		return -ENOMEM;

	platform_set_drvdata(pdev, moxart_wdt);

	moxart_wdt->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(moxart_wdt->base))
		return PTR_ERR(moxart_wdt->base);

	clk = devm_clk_get(dev, NULL);
	if (IS_ERR(clk)) {
		pr_err("%s: of_clk_get failed\n", __func__);
		return PTR_ERR(clk);
	}

	moxart_wdt->clock_frequency = clk_get_rate(clk);
	if (moxart_wdt->clock_frequency == 0) {
		pr_err("%s: incorrect clock frequency\n", __func__);
		return -EINVAL;
	}

	max_timeout = UINT_MAX / moxart_wdt->clock_frequency;

	moxart_wdt->dev.info = &moxart_wdt_info;
	moxart_wdt->dev.ops = &moxart_wdt_ops;
	moxart_wdt->dev.timeout = max_timeout;
	moxart_wdt->dev.min_timeout = 1;
	moxart_wdt->dev.max_timeout = max_timeout;
	moxart_wdt->dev.parent = dev;

	watchdog_init_timeout(&moxart_wdt->dev, heartbeat, dev);
	watchdog_set_nowayout(&moxart_wdt->dev, nowayout);
	watchdog_set_restart_priority(&moxart_wdt->dev, 128);

	watchdog_set_drvdata(&moxart_wdt->dev, moxart_wdt);

	watchdog_stop_on_unregister(&moxart_wdt->dev);
	err = devm_watchdog_register_device(dev, &moxart_wdt->dev);
	if (err)
		return err;

	dev_dbg(dev, "Watchdog enabled (heartbeat=%d sec, nowayout=%d)\n",
		moxart_wdt->dev.timeout, nowayout);

	return 0;
}

static const struct of_device_id moxart_watchdog_match[] = {
	{ .compatible = "moxa,moxart-watchdog" },
	{ },
};
MODULE_DEVICE_TABLE(of, moxart_watchdog_match);

static struct platform_driver moxart_wdt_driver = {
	.probe      = moxart_wdt_probe,
	.driver     = {
		.name		= "moxart-watchdog",
		.of_match_table	= moxart_watchdog_match,
	},
};
module_platform_driver(moxart_wdt_driver);

module_param(heartbeat, int, 0);
MODULE_PARM_DESC(heartbeat, "Watchdog heartbeat in seconds");

MODULE_DESCRIPTION("MOXART watchdog driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jonas Jensen <jonas.jensen@gmail.com>");
