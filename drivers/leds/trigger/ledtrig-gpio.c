// SPDX-License-Identifier: GPL-2.0-only
/*
 * ledtrig-gio.c - LED Trigger Based on GPIO events
 *
 * Copyright 2009 Felipe Balbi <me@felipebalbi.com>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/leds.h>
#include <linux/slab.h>
#include "../leds.h"

struct gpio_trig_data {
	struct led_classdev *led;

	unsigned desired_brightness;	/* desired brightness when led is on */
	unsigned inverted;		/* true when gpio is inverted */
	unsigned gpio;			/* gpio that triggers the leds */
};

static irqreturn_t gpio_trig_irq(int irq, void *_led)
{
	struct led_classdev *led = _led;
	struct gpio_trig_data *gpio_data = led_get_trigger_data(led);
	int tmp;

	tmp = gpio_get_value_cansleep(gpio_data->gpio);
	if (gpio_data->inverted)
		tmp = !tmp;

	if (tmp) {
		if (gpio_data->desired_brightness)
			led_set_brightness_nosleep(gpio_data->led,
					   gpio_data->desired_brightness);
		else
			led_set_brightness_nosleep(gpio_data->led, LED_FULL);
	} else {
		led_set_brightness_nosleep(gpio_data->led, LED_OFF);
	}

	return IRQ_HANDLED;
}

static ssize_t gpio_trig_brightness_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct gpio_trig_data *gpio_data = led_trigger_get_drvdata(dev);

	return sprintf(buf, "%u\n", gpio_data->desired_brightness);
}

static ssize_t gpio_trig_brightness_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t n)
{
	struct gpio_trig_data *gpio_data = led_trigger_get_drvdata(dev);
	unsigned desired_brightness;
	int ret;

	ret = sscanf(buf, "%u", &desired_brightness);
	if (ret < 1 || desired_brightness > 255) {
		dev_err(dev, "invalid value\n");
		return -EINVAL;
	}

	gpio_data->desired_brightness = desired_brightness;

	return n;
}
static DEVICE_ATTR(desired_brightness, 0644, gpio_trig_brightness_show,
		gpio_trig_brightness_store);

static ssize_t gpio_trig_inverted_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct gpio_trig_data *gpio_data = led_trigger_get_drvdata(dev);

	return sprintf(buf, "%u\n", gpio_data->inverted);
}

static ssize_t gpio_trig_inverted_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t n)
{
	struct led_classdev *led = led_trigger_get_led(dev);
	struct gpio_trig_data *gpio_data = led_trigger_get_drvdata(dev);
	unsigned long inverted;
	int ret;

	ret = kstrtoul(buf, 10, &inverted);
	if (ret < 0)
		return ret;

	if (inverted > 1)
		return -EINVAL;

	gpio_data->inverted = inverted;

	/* After inverting, we need to update the LED. */
	if (gpio_is_valid(gpio_data->gpio))
		gpio_trig_irq(0, led);

	return n;
}
static DEVICE_ATTR(inverted, 0644, gpio_trig_inverted_show,
		gpio_trig_inverted_store);

static ssize_t gpio_trig_gpio_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct gpio_trig_data *gpio_data = led_trigger_get_drvdata(dev);

	return sprintf(buf, "%u\n", gpio_data->gpio);
}

static ssize_t gpio_trig_gpio_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t n)
{
	struct led_classdev *led = led_trigger_get_led(dev);
	struct gpio_trig_data *gpio_data = led_trigger_get_drvdata(dev);
	unsigned gpio;
	int ret;

	ret = sscanf(buf, "%u", &gpio);
	if (ret < 1) {
		dev_err(dev, "couldn't read gpio number\n");
		return -EINVAL;
	}

	if (gpio_data->gpio == gpio)
		return n;

	if (!gpio_is_valid(gpio)) {
		if (gpio_is_valid(gpio_data->gpio))
			free_irq(gpio_to_irq(gpio_data->gpio), led);
		gpio_data->gpio = gpio;
		return n;
	}

	ret = request_threaded_irq(gpio_to_irq(gpio), NULL, gpio_trig_irq,
			IRQF_ONESHOT | IRQF_SHARED | IRQF_TRIGGER_RISING
			| IRQF_TRIGGER_FALLING, "ledtrig-gpio", led);
	if (ret) {
		dev_err(dev, "request_irq failed with error %d\n", ret);
	} else {
		if (gpio_is_valid(gpio_data->gpio))
			free_irq(gpio_to_irq(gpio_data->gpio), led);
		gpio_data->gpio = gpio;
		/* After changing the GPIO, we need to update the LED. */
		gpio_trig_irq(0, led);
	}

	return ret ? ret : n;
}
static DEVICE_ATTR(gpio, 0644, gpio_trig_gpio_show, gpio_trig_gpio_store);

static struct attribute *gpio_trig_attrs[] = {
	&dev_attr_desired_brightness.attr,
	&dev_attr_inverted.attr,
	&dev_attr_gpio.attr,
	NULL
};
ATTRIBUTE_GROUPS(gpio_trig);

static int gpio_trig_activate(struct led_classdev *led)
{
	struct gpio_trig_data *gpio_data;

	gpio_data = kzalloc(sizeof(*gpio_data), GFP_KERNEL);
	if (!gpio_data)
		return -ENOMEM;

	gpio_data->led = led;
	gpio_data->gpio = -ENOENT;

	led_set_trigger_data(led, gpio_data);

	return 0;
}

static void gpio_trig_deactivate(struct led_classdev *led)
{
	struct gpio_trig_data *gpio_data = led_get_trigger_data(led);

	if (gpio_is_valid(gpio_data->gpio))
		free_irq(gpio_to_irq(gpio_data->gpio), led);
	kfree(gpio_data);
}

static struct led_trigger gpio_led_trigger = {
	.name		= "gpio",
	.activate	= gpio_trig_activate,
	.deactivate	= gpio_trig_deactivate,
	.groups		= gpio_trig_groups,
};
module_led_trigger(gpio_led_trigger);

MODULE_AUTHOR("Felipe Balbi <me@felipebalbi.com>");
MODULE_DESCRIPTION("GPIO LED trigger");
MODULE_LICENSE("GPL v2");
