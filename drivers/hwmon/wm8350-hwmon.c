// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/hwmon/wm8350-hwmon.c - Wolfson Microelectronics WM8350 PMIC
 *                                  hardware monitoring features.
 *
 * Copyright (C) 2009 Wolfson Microelectronics plc
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>

#include <linux/mfd/wm8350/core.h>
#include <linux/mfd/wm8350/comparator.h>

static const char * const input_names[] = {
	[WM8350_AUXADC_USB]  = "USB",
	[WM8350_AUXADC_LINE] = "Line",
	[WM8350_AUXADC_BATT] = "Battery",
};

static ssize_t show_voltage(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	struct wm8350 *wm8350 = dev_get_drvdata(dev);
	int channel = to_sensor_dev_attr(attr)->index;
	int val;

	val = wm8350_read_auxadc(wm8350, channel, 0, 0) * WM8350_AUX_COEFF;
	val = DIV_ROUND_CLOSEST(val, 1000);

	return sprintf(buf, "%d\n", val);
}

static ssize_t show_label(struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	int channel = to_sensor_dev_attr(attr)->index;

	return sprintf(buf, "%s\n", input_names[channel]);
}

#define WM8350_NAMED_VOLTAGE(id, name) \
	static SENSOR_DEVICE_ATTR(in##id##_input, S_IRUGO, show_voltage,\
				  NULL, name);		\
	static SENSOR_DEVICE_ATTR(in##id##_label, S_IRUGO, show_label,	\
				  NULL, name)

WM8350_NAMED_VOLTAGE(0, WM8350_AUXADC_USB);
WM8350_NAMED_VOLTAGE(1, WM8350_AUXADC_BATT);
WM8350_NAMED_VOLTAGE(2, WM8350_AUXADC_LINE);

static struct attribute *wm8350_attrs[] = {
	&sensor_dev_attr_in0_input.dev_attr.attr,
	&sensor_dev_attr_in0_label.dev_attr.attr,
	&sensor_dev_attr_in1_input.dev_attr.attr,
	&sensor_dev_attr_in1_label.dev_attr.attr,
	&sensor_dev_attr_in2_input.dev_attr.attr,
	&sensor_dev_attr_in2_label.dev_attr.attr,

	NULL,
};

ATTRIBUTE_GROUPS(wm8350);

static int wm8350_hwmon_probe(struct platform_device *pdev)
{
	struct wm8350 *wm8350 = platform_get_drvdata(pdev);
	struct device *hwmon_dev;

	hwmon_dev = devm_hwmon_device_register_with_groups(&pdev->dev, "wm8350",
							   wm8350,
							   wm8350_groups);
	return PTR_ERR_OR_ZERO(hwmon_dev);
}

static struct platform_driver wm8350_hwmon_driver = {
	.probe = wm8350_hwmon_probe,
	.driver = {
		.name = "wm8350-hwmon",
	},
};

module_platform_driver(wm8350_hwmon_driver);

MODULE_AUTHOR("Mark Brown <broonie@opensource.wolfsonmicro.com>");
MODULE_DESCRIPTION("WM8350 Hardware Monitoring");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:wm8350-hwmon");
