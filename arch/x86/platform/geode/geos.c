// SPDX-License-Identifier: GPL-2.0-only
/*
 * System Specific setup for Traverse Technologies GEOS.
 * At the moment this means setup of GPIO control of LEDs.
 *
 * Copyright (C) 2008 Constantin Baranov <const@mimas.ru>
 * Copyright (C) 2011 Ed Wildgoose <kernel@wildgooses.com>
 *                and Philip Prindeville <philipp@redfish-solutions.com>
 *
 * TODO: There are large similarities with leds-net5501.c
 * by Alessandro Zummo <a.zummo@towertech.it>
 * In the future leds-net5501.c should be migrated over to platform
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/string.h>
#include <linux/leds.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <linux/gpio/machine.h>
#include <linux/dmi.h>

#include <asm/geode.h>

static struct gpio_keys_button geos_gpio_buttons[] = {
	{
		.code = KEY_RESTART,
		.gpio = 3,
		.active_low = 1,
		.desc = "Reset button",
		.type = EV_KEY,
		.wakeup = 0,
		.debounce_interval = 100,
		.can_disable = 0,
	}
};
static struct gpio_keys_platform_data geos_buttons_data = {
	.buttons = geos_gpio_buttons,
	.nbuttons = ARRAY_SIZE(geos_gpio_buttons),
	.poll_interval = 20,
};

static struct platform_device geos_buttons_dev = {
	.name = "gpio-keys-polled",
	.id = 1,
	.dev = {
		.platform_data = &geos_buttons_data,
	}
};

static struct gpio_led geos_leds[] = {
	{
		.name = "geos:1",
		.default_trigger = "default-on",
	},
	{
		.name = "geos:2",
		.default_trigger = "default-off",
	},
	{
		.name = "geos:3",
		.default_trigger = "default-off",
	},
};

static struct gpio_led_platform_data geos_leds_data = {
	.num_leds = ARRAY_SIZE(geos_leds),
	.leds = geos_leds,
};

static struct gpiod_lookup_table geos_leds_gpio_table = {
	.dev_id = "leds-gpio",
	.table = {
		/* The Geode GPIOs should be on the CS5535 companion chip */
		GPIO_LOOKUP_IDX("cs5535-gpio", 6, NULL, 0, GPIO_ACTIVE_LOW),
		GPIO_LOOKUP_IDX("cs5535-gpio", 25, NULL, 1, GPIO_ACTIVE_LOW),
		GPIO_LOOKUP_IDX("cs5535-gpio", 27, NULL, 2, GPIO_ACTIVE_LOW),
		{ }
	},
};

static struct platform_device geos_leds_dev = {
	.name = "leds-gpio",
	.id = -1,
	.dev.platform_data = &geos_leds_data,
};

static struct platform_device *geos_devs[] __initdata = {
	&geos_buttons_dev,
	&geos_leds_dev,
};

static void __init register_geos(void)
{
	/* Setup LED control through leds-gpio driver */
	gpiod_add_lookup_table(&geos_leds_gpio_table);
	platform_add_devices(geos_devs, ARRAY_SIZE(geos_devs));
}

static int __init geos_init(void)
{
	const char *vendor, *product;

	if (!is_geode())
		return 0;

	vendor = dmi_get_system_info(DMI_SYS_VENDOR);
	if (!vendor || strcmp(vendor, "Traverse Technologies"))
		return 0;

	product = dmi_get_system_info(DMI_PRODUCT_NAME);
	if (!product || strcmp(product, "Geos"))
		return 0;

	printk(KERN_INFO "%s: system is recognized as \"%s %s\"\n",
	       KBUILD_MODNAME, vendor, product);

	register_geos();

	return 0;
}
device_initcall(geos_init);
