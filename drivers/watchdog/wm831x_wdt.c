// SPDX-License-Identifier: GPL-2.0+
/*
 * Watchdog driver for the wm831x PMICs
 *
 * Copyright (C) 2009 Wolfson Microelectronics
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/watchdog.h>
#include <linux/uaccess.h>

#include <linux/mfd/wm831x/core.h>
#include <linux/mfd/wm831x/pdata.h>
#include <linux/mfd/wm831x/watchdog.h>

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout,
		 "Watchdog cannot be stopped once started (default="
		 __MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

struct wm831x_wdt_drvdata {
	struct watchdog_device wdt;
	struct wm831x *wm831x;
	struct mutex lock;
	int update_state;
};

/* We can't use the sub-second values here but they're included
 * for completeness.  */
static struct {
	unsigned int time;  /* Seconds */
	u16 val;            /* WDOG_TO value */
} wm831x_wdt_cfgs[] = {
	{  1, 2 },
	{  2, 3 },
	{  4, 4 },
	{  8, 5 },
	{ 16, 6 },
	{ 32, 7 },
	{ 33, 7 },  /* Actually 32.768s so include both, others round down */
};

static int wm831x_wdt_start(struct watchdog_device *wdt_dev)
{
	struct wm831x_wdt_drvdata *driver_data = watchdog_get_drvdata(wdt_dev);
	struct wm831x *wm831x = driver_data->wm831x;
	int ret;

	mutex_lock(&driver_data->lock);

	ret = wm831x_reg_unlock(wm831x);
	if (ret == 0) {
		ret = wm831x_set_bits(wm831x, WM831X_WATCHDOG,
				      WM831X_WDOG_ENA, WM831X_WDOG_ENA);
		wm831x_reg_lock(wm831x);
	} else {
		dev_err(wm831x->dev, "Failed to unlock security key: %d\n",
			ret);
	}

	mutex_unlock(&driver_data->lock);

	return ret;
}

static int wm831x_wdt_stop(struct watchdog_device *wdt_dev)
{
	struct wm831x_wdt_drvdata *driver_data = watchdog_get_drvdata(wdt_dev);
	struct wm831x *wm831x = driver_data->wm831x;
	int ret;

	mutex_lock(&driver_data->lock);

	ret = wm831x_reg_unlock(wm831x);
	if (ret == 0) {
		ret = wm831x_set_bits(wm831x, WM831X_WATCHDOG,
				      WM831X_WDOG_ENA, 0);
		wm831x_reg_lock(wm831x);
	} else {
		dev_err(wm831x->dev, "Failed to unlock security key: %d\n",
			ret);
	}

	mutex_unlock(&driver_data->lock);

	return ret;
}

static int wm831x_wdt_ping(struct watchdog_device *wdt_dev)
{
	struct wm831x_wdt_drvdata *driver_data = watchdog_get_drvdata(wdt_dev);
	struct wm831x *wm831x = driver_data->wm831x;
	int ret;
	u16 reg;

	mutex_lock(&driver_data->lock);

	reg = wm831x_reg_read(wm831x, WM831X_WATCHDOG);

	if (!(reg & WM831X_WDOG_RST_SRC)) {
		dev_err(wm831x->dev, "Hardware watchdog update unsupported\n");
		ret = -EINVAL;
		goto out;
	}

	reg |= WM831X_WDOG_RESET;

	ret = wm831x_reg_unlock(wm831x);
	if (ret == 0) {
		ret = wm831x_reg_write(wm831x, WM831X_WATCHDOG, reg);
		wm831x_reg_lock(wm831x);
	} else {
		dev_err(wm831x->dev, "Failed to unlock security key: %d\n",
			ret);
	}

out:
	mutex_unlock(&driver_data->lock);

	return ret;
}

static int wm831x_wdt_set_timeout(struct watchdog_device *wdt_dev,
				  unsigned int timeout)
{
	struct wm831x_wdt_drvdata *driver_data = watchdog_get_drvdata(wdt_dev);
	struct wm831x *wm831x = driver_data->wm831x;
	int ret, i;

	for (i = 0; i < ARRAY_SIZE(wm831x_wdt_cfgs); i++)
		if (wm831x_wdt_cfgs[i].time == timeout)
			break;
	if (i == ARRAY_SIZE(wm831x_wdt_cfgs))
		return -EINVAL;

	ret = wm831x_reg_unlock(wm831x);
	if (ret == 0) {
		ret = wm831x_set_bits(wm831x, WM831X_WATCHDOG,
				      WM831X_WDOG_TO_MASK,
				      wm831x_wdt_cfgs[i].val);
		wm831x_reg_lock(wm831x);
	} else {
		dev_err(wm831x->dev, "Failed to unlock security key: %d\n",
			ret);
	}

	wdt_dev->timeout = timeout;

	return ret;
}

static const struct watchdog_info wm831x_wdt_info = {
	.options = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE,
	.identity = "WM831x Watchdog",
};

static const struct watchdog_ops wm831x_wdt_ops = {
	.owner = THIS_MODULE,
	.start = wm831x_wdt_start,
	.stop = wm831x_wdt_stop,
	.ping = wm831x_wdt_ping,
	.set_timeout = wm831x_wdt_set_timeout,
};

static int wm831x_wdt_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct wm831x *wm831x = dev_get_drvdata(dev->parent);
	struct wm831x_pdata *chip_pdata = dev_get_platdata(dev->parent);
	struct wm831x_watchdog_pdata *pdata;
	struct wm831x_wdt_drvdata *driver_data;
	struct watchdog_device *wm831x_wdt;
	int reg, ret, i;

	ret = wm831x_reg_read(wm831x, WM831X_WATCHDOG);
	if (ret < 0) {
		dev_err(wm831x->dev, "Failed to read watchdog status: %d\n",
			ret);
		return ret;
	}
	reg = ret;

	if (reg & WM831X_WDOG_DEBUG)
		dev_warn(wm831x->dev, "Watchdog is paused\n");

	driver_data = devm_kzalloc(dev, sizeof(*driver_data), GFP_KERNEL);
	if (!driver_data)
		return -ENOMEM;

	mutex_init(&driver_data->lock);
	driver_data->wm831x = wm831x;

	wm831x_wdt = &driver_data->wdt;

	wm831x_wdt->info = &wm831x_wdt_info;
	wm831x_wdt->ops = &wm831x_wdt_ops;
	wm831x_wdt->parent = dev;
	watchdog_set_nowayout(wm831x_wdt, nowayout);
	watchdog_set_drvdata(wm831x_wdt, driver_data);

	reg = wm831x_reg_read(wm831x, WM831X_WATCHDOG);
	reg &= WM831X_WDOG_TO_MASK;
	for (i = 0; i < ARRAY_SIZE(wm831x_wdt_cfgs); i++)
		if (wm831x_wdt_cfgs[i].val == reg)
			break;
	if (i == ARRAY_SIZE(wm831x_wdt_cfgs))
		dev_warn(wm831x->dev,
			 "Unknown watchdog timeout: %x\n", reg);
	else
		wm831x_wdt->timeout = wm831x_wdt_cfgs[i].time;

	/* Apply any configuration */
	if (chip_pdata)
		pdata = chip_pdata->watchdog;
	else
		pdata = NULL;

	if (pdata) {
		reg &= ~(WM831X_WDOG_SECACT_MASK | WM831X_WDOG_PRIMACT_MASK |
			 WM831X_WDOG_RST_SRC);

		reg |= pdata->primary << WM831X_WDOG_PRIMACT_SHIFT;
		reg |= pdata->secondary << WM831X_WDOG_SECACT_SHIFT;
		reg |= pdata->software << WM831X_WDOG_RST_SRC_SHIFT;

		ret = wm831x_reg_unlock(wm831x);
		if (ret == 0) {
			ret = wm831x_reg_write(wm831x, WM831X_WATCHDOG, reg);
			wm831x_reg_lock(wm831x);
		} else {
			dev_err(wm831x->dev,
				"Failed to unlock security key: %d\n", ret);
			return ret;
		}
	}

	return devm_watchdog_register_device(dev, &driver_data->wdt);
}

static struct platform_driver wm831x_wdt_driver = {
	.probe = wm831x_wdt_probe,
	.driver = {
		.name = "wm831x-watchdog",
	},
};

module_platform_driver(wm831x_wdt_driver);

MODULE_AUTHOR("Mark Brown");
MODULE_DESCRIPTION("WM831x Watchdog");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:wm831x-watchdog");
