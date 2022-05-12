// SPDX-License-Identifier: GPL-2.0-only
/*
 * Bluetooth built-in chip control
 *
 * Copyright (c) 2008 Dmitry Baryshkov
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/rfkill.h>

#include "tosa_bt.h"

static void tosa_bt_on(struct tosa_bt_data *data)
{
	gpio_set_value(data->gpio_reset, 0);
	gpio_set_value(data->gpio_pwr, 1);
	gpio_set_value(data->gpio_reset, 1);
	mdelay(20);
	gpio_set_value(data->gpio_reset, 0);
}

static void tosa_bt_off(struct tosa_bt_data *data)
{
	gpio_set_value(data->gpio_reset, 1);
	mdelay(10);
	gpio_set_value(data->gpio_pwr, 0);
	gpio_set_value(data->gpio_reset, 0);
}

static int tosa_bt_set_block(void *data, bool blocked)
{
	pr_info("BT_RADIO going: %s\n", blocked ? "off" : "on");

	if (!blocked) {
		pr_info("TOSA_BT: going ON\n");
		tosa_bt_on(data);
	} else {
		pr_info("TOSA_BT: going OFF\n");
		tosa_bt_off(data);
	}

	return 0;
}

static const struct rfkill_ops tosa_bt_rfkill_ops = {
	.set_block = tosa_bt_set_block,
};

static int tosa_bt_probe(struct platform_device *dev)
{
	int rc;
	struct rfkill *rfk;

	struct tosa_bt_data *data = dev->dev.platform_data;

	rc = gpio_request(data->gpio_reset, "Bluetooth reset");
	if (rc)
		goto err_reset;
	rc = gpio_direction_output(data->gpio_reset, 0);
	if (rc)
		goto err_reset_dir;
	rc = gpio_request(data->gpio_pwr, "Bluetooth power");
	if (rc)
		goto err_pwr;
	rc = gpio_direction_output(data->gpio_pwr, 0);
	if (rc)
		goto err_pwr_dir;

	rfk = rfkill_alloc("tosa-bt", &dev->dev, RFKILL_TYPE_BLUETOOTH,
			   &tosa_bt_rfkill_ops, data);
	if (!rfk) {
		rc = -ENOMEM;
		goto err_rfk_alloc;
	}

	rc = rfkill_register(rfk);
	if (rc)
		goto err_rfkill;

	platform_set_drvdata(dev, rfk);

	return 0;

err_rfkill:
	rfkill_destroy(rfk);
err_rfk_alloc:
	tosa_bt_off(data);
err_pwr_dir:
	gpio_free(data->gpio_pwr);
err_pwr:
err_reset_dir:
	gpio_free(data->gpio_reset);
err_reset:
	return rc;
}

static int tosa_bt_remove(struct platform_device *dev)
{
	struct tosa_bt_data *data = dev->dev.platform_data;
	struct rfkill *rfk = platform_get_drvdata(dev);

	platform_set_drvdata(dev, NULL);

	if (rfk) {
		rfkill_unregister(rfk);
		rfkill_destroy(rfk);
	}
	rfk = NULL;

	tosa_bt_off(data);

	gpio_free(data->gpio_pwr);
	gpio_free(data->gpio_reset);

	return 0;
}

static struct platform_driver tosa_bt_driver = {
	.probe = tosa_bt_probe,
	.remove = tosa_bt_remove,

	.driver = {
		.name = "tosa-bt",
	},
};
module_platform_driver(tosa_bt_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry Baryshkov");
MODULE_DESCRIPTION("Bluetooth built-in chip control");
