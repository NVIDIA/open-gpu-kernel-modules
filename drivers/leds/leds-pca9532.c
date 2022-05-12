// SPDX-License-Identifier: GPL-2.0-only
/*
 * pca9532.c - 16-bit Led dimmer
 *
 * Copyright (C) 2011 Jan Weitzel
 * Copyright (C) 2008 Riku Voipio
 *
 * Datasheet: http://www.nxp.com/documents/data_sheet/PCA9532.pdf
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/leds.h>
#include <linux/input.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/leds-pca9532.h>
#include <linux/gpio/driver.h>
#include <linux/of.h>
#include <linux/of_device.h>

/* m =  num_leds*/
#define PCA9532_REG_INPUT(i)	((i) >> 3)
#define PCA9532_REG_OFFSET(m)	((m) >> 4)
#define PCA9532_REG_PSC(m, i)	(PCA9532_REG_OFFSET(m) + 0x1 + (i) * 2)
#define PCA9532_REG_PWM(m, i)	(PCA9532_REG_OFFSET(m) + 0x2 + (i) * 2)
#define LED_REG(m, led)		(PCA9532_REG_OFFSET(m) + 0x5 + (led >> 2))
#define LED_NUM(led)		(led & 0x3)
#define LED_SHIFT(led)		(LED_NUM(led) * 2)
#define LED_MASK(led)		(0x3 << LED_SHIFT(led))

#define ldev_to_led(c)       container_of(c, struct pca9532_led, ldev)

struct pca9532_chip_info {
	u8	num_leds;
};

struct pca9532_data {
	struct i2c_client *client;
	struct pca9532_led leds[16];
	struct mutex update_lock;
	struct input_dev *idev;
	struct work_struct work;
#ifdef CONFIG_LEDS_PCA9532_GPIO
	struct gpio_chip gpio;
#endif
	const struct pca9532_chip_info *chip_info;
	u8 pwm[2];
	u8 psc[2];
};

static int pca9532_probe(struct i2c_client *client,
	const struct i2c_device_id *id);
static int pca9532_remove(struct i2c_client *client);

enum {
	pca9530,
	pca9531,
	pca9532,
	pca9533,
};

static const struct i2c_device_id pca9532_id[] = {
	{ "pca9530", pca9530 },
	{ "pca9531", pca9531 },
	{ "pca9532", pca9532 },
	{ "pca9533", pca9533 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, pca9532_id);

static const struct pca9532_chip_info pca9532_chip_info_tbl[] = {
	[pca9530] = {
		.num_leds = 2,
	},
	[pca9531] = {
		.num_leds = 8,
	},
	[pca9532] = {
		.num_leds = 16,
	},
	[pca9533] = {
		.num_leds = 4,
	},
};

#ifdef CONFIG_OF
static const struct of_device_id of_pca9532_leds_match[] = {
	{ .compatible = "nxp,pca9530", .data = (void *)pca9530 },
	{ .compatible = "nxp,pca9531", .data = (void *)pca9531 },
	{ .compatible = "nxp,pca9532", .data = (void *)pca9532 },
	{ .compatible = "nxp,pca9533", .data = (void *)pca9533 },
	{},
};

MODULE_DEVICE_TABLE(of, of_pca9532_leds_match);
#endif

static struct i2c_driver pca9532_driver = {
	.driver = {
		.name = "leds-pca953x",
		.of_match_table = of_match_ptr(of_pca9532_leds_match),
	},
	.probe = pca9532_probe,
	.remove = pca9532_remove,
	.id_table = pca9532_id,
};

/* We have two pwm/blinkers, but 16 possible leds to drive. Additionally,
 * the clever Thecus people are using one pwm to drive the beeper. So,
 * as a compromise we average one pwm to the values requested by all
 * leds that are not ON/OFF.
 * */
static int pca9532_calcpwm(struct i2c_client *client, int pwm, int blink,
	enum led_brightness value)
{
	int a = 0, b = 0, i = 0;
	struct pca9532_data *data = i2c_get_clientdata(client);
	for (i = 0; i < data->chip_info->num_leds; i++) {
		if (data->leds[i].type == PCA9532_TYPE_LED &&
			data->leds[i].state == PCA9532_PWM0+pwm) {
				a++;
				b += data->leds[i].ldev.brightness;
		}
	}
	if (a == 0) {
		dev_err(&client->dev,
		"fear of division by zero %d/%d, wanted %d\n",
			b, a, value);
		return -EINVAL;
	}
	b = b/a;
	if (b > 0xFF)
		return -EINVAL;
	data->pwm[pwm] = b;
	data->psc[pwm] = blink;
	return 0;
}

static int pca9532_setpwm(struct i2c_client *client, int pwm)
{
	struct pca9532_data *data = i2c_get_clientdata(client);
	u8 maxleds = data->chip_info->num_leds;

	mutex_lock(&data->update_lock);
	i2c_smbus_write_byte_data(client, PCA9532_REG_PWM(maxleds, pwm),
		data->pwm[pwm]);
	i2c_smbus_write_byte_data(client, PCA9532_REG_PSC(maxleds, pwm),
		data->psc[pwm]);
	mutex_unlock(&data->update_lock);
	return 0;
}

/* Set LED routing */
static void pca9532_setled(struct pca9532_led *led)
{
	struct i2c_client *client = led->client;
	struct pca9532_data *data = i2c_get_clientdata(client);
	u8 maxleds = data->chip_info->num_leds;
	char reg;

	mutex_lock(&data->update_lock);
	reg = i2c_smbus_read_byte_data(client, LED_REG(maxleds, led->id));
	/* zero led bits */
	reg = reg & ~LED_MASK(led->id);
	/* set the new value */
	reg = reg | (led->state << LED_SHIFT(led->id));
	i2c_smbus_write_byte_data(client, LED_REG(maxleds, led->id), reg);
	mutex_unlock(&data->update_lock);
}

static int pca9532_set_brightness(struct led_classdev *led_cdev,
	enum led_brightness value)
{
	int err = 0;
	struct pca9532_led *led = ldev_to_led(led_cdev);

	if (value == LED_OFF)
		led->state = PCA9532_OFF;
	else if (value == LED_FULL)
		led->state = PCA9532_ON;
	else {
		led->state = PCA9532_PWM0; /* Thecus: hardcode one pwm */
		err = pca9532_calcpwm(led->client, 0, 0, value);
		if (err)
			return err;
	}
	if (led->state == PCA9532_PWM0)
		pca9532_setpwm(led->client, 0);
	pca9532_setled(led);
	return err;
}

static int pca9532_set_blink(struct led_classdev *led_cdev,
	unsigned long *delay_on, unsigned long *delay_off)
{
	struct pca9532_led *led = ldev_to_led(led_cdev);
	struct i2c_client *client = led->client;
	int psc;
	int err = 0;

	if (*delay_on == 0 && *delay_off == 0) {
		/* led subsystem ask us for a blink rate */
		*delay_on = 1000;
		*delay_off = 1000;
	}
	if (*delay_on != *delay_off || *delay_on > 1690 || *delay_on < 6)
		return -EINVAL;

	/* Thecus specific: only use PSC/PWM 0 */
	psc = (*delay_on * 152-1)/1000;
	err = pca9532_calcpwm(client, 0, psc, led_cdev->brightness);
	if (err)
		return err;
	if (led->state == PCA9532_PWM0)
		pca9532_setpwm(led->client, 0);
	pca9532_setled(led);

	return 0;
}

static int pca9532_event(struct input_dev *dev, unsigned int type,
	unsigned int code, int value)
{
	struct pca9532_data *data = input_get_drvdata(dev);

	if (!(type == EV_SND && (code == SND_BELL || code == SND_TONE)))
		return -1;

	/* XXX: allow different kind of beeps with psc/pwm modifications */
	if (value > 1 && value < 32767)
		data->pwm[1] = 127;
	else
		data->pwm[1] = 0;

	schedule_work(&data->work);

	return 0;
}

static void pca9532_input_work(struct work_struct *work)
{
	struct pca9532_data *data =
		container_of(work, struct pca9532_data, work);
	u8 maxleds = data->chip_info->num_leds;

	mutex_lock(&data->update_lock);
	i2c_smbus_write_byte_data(data->client, PCA9532_REG_PWM(maxleds, 1),
		data->pwm[1]);
	mutex_unlock(&data->update_lock);
}

static enum pca9532_state pca9532_getled(struct pca9532_led *led)
{
	struct i2c_client *client = led->client;
	struct pca9532_data *data = i2c_get_clientdata(client);
	u8 maxleds = data->chip_info->num_leds;
	char reg;
	enum pca9532_state ret;

	mutex_lock(&data->update_lock);
	reg = i2c_smbus_read_byte_data(client, LED_REG(maxleds, led->id));
	ret = (reg & LED_MASK(led->id)) >> LED_SHIFT(led->id);
	mutex_unlock(&data->update_lock);
	return ret;
}

#ifdef CONFIG_LEDS_PCA9532_GPIO
static int pca9532_gpio_request_pin(struct gpio_chip *gc, unsigned offset)
{
	struct pca9532_data *data = gpiochip_get_data(gc);
	struct pca9532_led *led = &data->leds[offset];

	if (led->type == PCA9532_TYPE_GPIO)
		return 0;

	return -EBUSY;
}

static void pca9532_gpio_set_value(struct gpio_chip *gc, unsigned offset, int val)
{
	struct pca9532_data *data = gpiochip_get_data(gc);
	struct pca9532_led *led = &data->leds[offset];

	if (val)
		led->state = PCA9532_ON;
	else
		led->state = PCA9532_OFF;

	pca9532_setled(led);
}

static int pca9532_gpio_get_value(struct gpio_chip *gc, unsigned offset)
{
	struct pca9532_data *data = gpiochip_get_data(gc);
	unsigned char reg;

	reg = i2c_smbus_read_byte_data(data->client, PCA9532_REG_INPUT(offset));

	return !!(reg & (1 << (offset % 8)));
}

static int pca9532_gpio_direction_input(struct gpio_chip *gc, unsigned offset)
{
	/* To use as input ensure pin is not driven */
	pca9532_gpio_set_value(gc, offset, 0);

	return 0;
}

static int pca9532_gpio_direction_output(struct gpio_chip *gc, unsigned offset, int val)
{
	pca9532_gpio_set_value(gc, offset, val);

	return 0;
}
#endif /* CONFIG_LEDS_PCA9532_GPIO */

static int pca9532_destroy_devices(struct pca9532_data *data, int n_devs)
{
	int i = n_devs;

	if (!data)
		return -EINVAL;

	while (--i >= 0) {
		switch (data->leds[i].type) {
		case PCA9532_TYPE_NONE:
		case PCA9532_TYPE_GPIO:
			break;
		case PCA9532_TYPE_LED:
			led_classdev_unregister(&data->leds[i].ldev);
			break;
		case PCA9532_TYPE_N2100_BEEP:
			if (data->idev != NULL) {
				cancel_work_sync(&data->work);
				data->idev = NULL;
			}
			break;
		}
	}

#ifdef CONFIG_LEDS_PCA9532_GPIO
	if (data->gpio.parent)
		gpiochip_remove(&data->gpio);
#endif

	return 0;
}

static int pca9532_configure(struct i2c_client *client,
	struct pca9532_data *data, struct pca9532_platform_data *pdata)
{
	int i, err = 0;
	int gpios = 0;
	u8 maxleds = data->chip_info->num_leds;

	for (i = 0; i < 2; i++)	{
		data->pwm[i] = pdata->pwm[i];
		data->psc[i] = pdata->psc[i];
		i2c_smbus_write_byte_data(client, PCA9532_REG_PWM(maxleds, i),
			data->pwm[i]);
		i2c_smbus_write_byte_data(client, PCA9532_REG_PSC(maxleds, i),
			data->psc[i]);
	}

	for (i = 0; i < data->chip_info->num_leds; i++) {
		struct pca9532_led *led = &data->leds[i];
		struct pca9532_led *pled = &pdata->leds[i];
		led->client = client;
		led->id = i;
		led->type = pled->type;
		switch (led->type) {
		case PCA9532_TYPE_NONE:
			break;
		case PCA9532_TYPE_GPIO:
			gpios++;
			break;
		case PCA9532_TYPE_LED:
			if (pled->state == PCA9532_KEEP)
				led->state = pca9532_getled(led);
			else
				led->state = pled->state;
			led->name = pled->name;
			led->ldev.name = led->name;
			led->ldev.default_trigger = pled->default_trigger;
			led->ldev.brightness = LED_OFF;
			led->ldev.brightness_set_blocking =
						pca9532_set_brightness;
			led->ldev.blink_set = pca9532_set_blink;
			err = led_classdev_register(&client->dev, &led->ldev);
			if (err < 0) {
				dev_err(&client->dev,
					"couldn't register LED %s\n",
					led->name);
				goto exit;
			}
			pca9532_setled(led);
			break;
		case PCA9532_TYPE_N2100_BEEP:
			BUG_ON(data->idev);
			led->state = PCA9532_PWM1;
			pca9532_setled(led);
			data->idev = devm_input_allocate_device(&client->dev);
			if (data->idev == NULL) {
				err = -ENOMEM;
				goto exit;
			}
			data->idev->name = pled->name;
			data->idev->phys = "i2c/pca9532";
			data->idev->id.bustype = BUS_HOST;
			data->idev->id.vendor = 0x001f;
			data->idev->id.product = 0x0001;
			data->idev->id.version = 0x0100;
			data->idev->evbit[0] = BIT_MASK(EV_SND);
			data->idev->sndbit[0] = BIT_MASK(SND_BELL) |
						BIT_MASK(SND_TONE);
			data->idev->event = pca9532_event;
			input_set_drvdata(data->idev, data);
			INIT_WORK(&data->work, pca9532_input_work);
			err = input_register_device(data->idev);
			if (err) {
				cancel_work_sync(&data->work);
				data->idev = NULL;
				goto exit;
			}
			break;
		}
	}

#ifdef CONFIG_LEDS_PCA9532_GPIO
	if (gpios) {
		data->gpio.label = "gpio-pca9532";
		data->gpio.direction_input = pca9532_gpio_direction_input;
		data->gpio.direction_output = pca9532_gpio_direction_output;
		data->gpio.set = pca9532_gpio_set_value;
		data->gpio.get = pca9532_gpio_get_value;
		data->gpio.request = pca9532_gpio_request_pin;
		data->gpio.can_sleep = 1;
		data->gpio.base = pdata->gpio_base;
		data->gpio.ngpio = data->chip_info->num_leds;
		data->gpio.parent = &client->dev;
		data->gpio.owner = THIS_MODULE;

		err = gpiochip_add_data(&data->gpio, data);
		if (err) {
			/* Use data->gpio.dev as a flag for freeing gpiochip */
			data->gpio.parent = NULL;
			dev_warn(&client->dev, "could not add gpiochip\n");
		} else {
			dev_info(&client->dev, "gpios %i...%i\n",
				data->gpio.base, data->gpio.base +
				data->gpio.ngpio - 1);
		}
	}
#endif

	return 0;

exit:
	pca9532_destroy_devices(data, i);
	return err;
}

static struct pca9532_platform_data *
pca9532_of_populate_pdata(struct device *dev, struct device_node *np)
{
	struct pca9532_platform_data *pdata;
	struct device_node *child;
	int devid, maxleds;
	int i = 0;
	const char *state;

	devid = (int)(uintptr_t)of_device_get_match_data(dev);
	maxleds = pca9532_chip_info_tbl[devid].num_leds;

	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return ERR_PTR(-ENOMEM);

	pdata->gpio_base = -1;

	of_property_read_u8_array(np, "nxp,pwm", &pdata->pwm[0],
				  ARRAY_SIZE(pdata->pwm));
	of_property_read_u8_array(np, "nxp,psc", &pdata->psc[0],
				  ARRAY_SIZE(pdata->psc));

	for_each_available_child_of_node(np, child) {
		if (of_property_read_string(child, "label",
					    &pdata->leds[i].name))
			pdata->leds[i].name = child->name;
		of_property_read_u32(child, "type", &pdata->leds[i].type);
		of_property_read_string(child, "linux,default-trigger",
					&pdata->leds[i].default_trigger);
		if (!of_property_read_string(child, "default-state", &state)) {
			if (!strcmp(state, "on"))
				pdata->leds[i].state = PCA9532_ON;
			else if (!strcmp(state, "keep"))
				pdata->leds[i].state = PCA9532_KEEP;
		}
		if (++i >= maxleds) {
			of_node_put(child);
			break;
		}
	}

	return pdata;
}

static int pca9532_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int devid;
	struct pca9532_data *data = i2c_get_clientdata(client);
	struct pca9532_platform_data *pca9532_pdata =
			dev_get_platdata(&client->dev);
	struct device_node *np = dev_of_node(&client->dev);

	if (!pca9532_pdata) {
		if (np) {
			pca9532_pdata =
				pca9532_of_populate_pdata(&client->dev, np);
			if (IS_ERR(pca9532_pdata))
				return PTR_ERR(pca9532_pdata);
		} else {
			dev_err(&client->dev, "no platform data\n");
			return -EINVAL;
		}
		devid = (int)(uintptr_t)of_device_get_match_data(&client->dev);
	} else {
		devid = id->driver_data;
	}

	if (!i2c_check_functionality(client->adapter,
		I2C_FUNC_SMBUS_BYTE_DATA))
		return -EIO;

	data = devm_kzalloc(&client->dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->chip_info = &pca9532_chip_info_tbl[devid];

	dev_info(&client->dev, "setting platform data\n");
	i2c_set_clientdata(client, data);
	data->client = client;
	mutex_init(&data->update_lock);

	return pca9532_configure(client, data, pca9532_pdata);
}

static int pca9532_remove(struct i2c_client *client)
{
	struct pca9532_data *data = i2c_get_clientdata(client);

	return pca9532_destroy_devices(data, data->chip_info->num_leds);
}

module_i2c_driver(pca9532_driver);

MODULE_AUTHOR("Riku Voipio");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PCA 9532 LED dimmer");
