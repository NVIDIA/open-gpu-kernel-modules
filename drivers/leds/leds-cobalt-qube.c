// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2006 - Florian Fainelli <florian@openwrt.org>
 *
 * Control the Cobalt Qube/RaQ front LED
 */
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/types.h>

#define LED_FRONT_LEFT	0x01
#define LED_FRONT_RIGHT	0x02

static void __iomem *led_port;
static u8 led_value;

static void qube_front_led_set(struct led_classdev *led_cdev,
			       enum led_brightness brightness)
{
	if (brightness)
		led_value = LED_FRONT_LEFT | LED_FRONT_RIGHT;
	else
		led_value = ~(LED_FRONT_LEFT | LED_FRONT_RIGHT);
	writeb(led_value, led_port);
}

static struct led_classdev qube_front_led = {
	.name			= "qube::front",
	.brightness		= LED_FULL,
	.brightness_set		= qube_front_led_set,
	.default_trigger	= "default-on",
};

static int cobalt_qube_led_probe(struct platform_device *pdev)
{
	struct resource *res;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -EBUSY;

	led_port = devm_ioremap(&pdev->dev, res->start, resource_size(res));
	if (!led_port)
		return -ENOMEM;

	led_value = LED_FRONT_LEFT | LED_FRONT_RIGHT;
	writeb(led_value, led_port);

	return devm_led_classdev_register(&pdev->dev, &qube_front_led);
}

static struct platform_driver cobalt_qube_led_driver = {
	.probe	= cobalt_qube_led_probe,
	.driver	= {
		.name	= "cobalt-qube-leds",
	},
};

module_platform_driver(cobalt_qube_led_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Front LED support for Cobalt Server");
MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org>");
MODULE_ALIAS("platform:cobalt-qube-leds");
