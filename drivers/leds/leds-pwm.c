// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/drivers/leds-pwm.c
 *
 * simple PWM based LED control
 *
 * Copyright 2009 Luotao Fu @ Pengutronix (l.fu@pengutronix.de)
 *
 * based on leds-gpio.c by Raphael Assenat <raph@8d.com>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/leds.h>
#include <linux/err.h>
#include <linux/pwm.h>
#include <linux/slab.h>

struct led_pwm {
	const char	*name;
	u8		active_low;
	unsigned int	max_brightness;
};

struct led_pwm_data {
	struct led_classdev	cdev;
	struct pwm_device	*pwm;
	struct pwm_state	pwmstate;
	unsigned int		active_low;
};

struct led_pwm_priv {
	int num_leds;
	struct led_pwm_data leds[];
};

static int led_pwm_set(struct led_classdev *led_cdev,
		       enum led_brightness brightness)
{
	struct led_pwm_data *led_dat =
		container_of(led_cdev, struct led_pwm_data, cdev);
	unsigned int max = led_dat->cdev.max_brightness;
	unsigned long long duty = led_dat->pwmstate.period;

	duty *= brightness;
	do_div(duty, max);

	if (led_dat->active_low)
		duty = led_dat->pwmstate.period - duty;

	led_dat->pwmstate.duty_cycle = duty;
	led_dat->pwmstate.enabled = duty > 0;
	return pwm_apply_state(led_dat->pwm, &led_dat->pwmstate);
}

__attribute__((nonnull))
static int led_pwm_add(struct device *dev, struct led_pwm_priv *priv,
		       struct led_pwm *led, struct fwnode_handle *fwnode)
{
	struct led_pwm_data *led_data = &priv->leds[priv->num_leds];
	struct led_init_data init_data = { .fwnode = fwnode };
	int ret;

	led_data->active_low = led->active_low;
	led_data->cdev.name = led->name;
	led_data->cdev.brightness = LED_OFF;
	led_data->cdev.max_brightness = led->max_brightness;
	led_data->cdev.flags = LED_CORE_SUSPENDRESUME;

	led_data->pwm = devm_fwnode_pwm_get(dev, fwnode, NULL);
	if (IS_ERR(led_data->pwm))
		return dev_err_probe(dev, PTR_ERR(led_data->pwm),
				     "unable to request PWM for %s\n",
				     led->name);

	led_data->cdev.brightness_set_blocking = led_pwm_set;

	pwm_init_state(led_data->pwm, &led_data->pwmstate);

	ret = devm_led_classdev_register_ext(dev, &led_data->cdev, &init_data);
	if (ret) {
		dev_err(dev, "failed to register PWM led for %s: %d\n",
			led->name, ret);
		return ret;
	}

	ret = led_pwm_set(&led_data->cdev, led_data->cdev.brightness);
	if (ret) {
		dev_err(dev, "failed to set led PWM value for %s: %d",
			led->name, ret);
		return ret;
	}

	priv->num_leds++;
	return 0;
}

static int led_pwm_create_fwnode(struct device *dev, struct led_pwm_priv *priv)
{
	struct fwnode_handle *fwnode;
	struct led_pwm led;
	int ret = 0;

	memset(&led, 0, sizeof(led));

	device_for_each_child_node(dev, fwnode) {
		ret = fwnode_property_read_string(fwnode, "label", &led.name);
		if (ret && is_of_node(fwnode))
			led.name = to_of_node(fwnode)->name;

		if (!led.name) {
			fwnode_handle_put(fwnode);
			return -EINVAL;
		}

		led.active_low = fwnode_property_read_bool(fwnode,
							   "active-low");
		fwnode_property_read_u32(fwnode, "max-brightness",
					 &led.max_brightness);

		ret = led_pwm_add(dev, priv, &led, fwnode);
		if (ret) {
			fwnode_handle_put(fwnode);
			break;
		}
	}

	return ret;
}

static int led_pwm_probe(struct platform_device *pdev)
{
	struct led_pwm_priv *priv;
	int ret = 0;
	int count;

	count = device_get_child_node_count(&pdev->dev);

	if (!count)
		return -EINVAL;

	priv = devm_kzalloc(&pdev->dev, struct_size(priv, leds, count),
			    GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	ret = led_pwm_create_fwnode(&pdev->dev, priv);

	if (ret)
		return ret;

	platform_set_drvdata(pdev, priv);

	return 0;
}

static const struct of_device_id of_pwm_leds_match[] = {
	{ .compatible = "pwm-leds", },
	{},
};
MODULE_DEVICE_TABLE(of, of_pwm_leds_match);

static struct platform_driver led_pwm_driver = {
	.probe		= led_pwm_probe,
	.driver		= {
		.name	= "leds_pwm",
		.of_match_table = of_pwm_leds_match,
	},
};

module_platform_driver(led_pwm_driver);

MODULE_AUTHOR("Luotao Fu <l.fu@pengutronix.de>");
MODULE_DESCRIPTION("generic PWM LED driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:leds-pwm");
