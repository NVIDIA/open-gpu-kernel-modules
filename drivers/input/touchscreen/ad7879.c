// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * AD7879/AD7889 based touchscreen and GPIO driver
 *
 * Copyright (C) 2008-2010 Michael Hennerich, Analog Devices Inc.
 *
 * History:
 * Copyright (c) 2005 David Brownell
 * Copyright (c) 2006 Nokia Corporation
 * Various changes: Imre Deak <imre.deak@nokia.com>
 *
 * Using code from:
 *  - corgi_ts.c
 *	Copyright (C) 2004-2005 Richard Purdie
 *  - omap_ts.[hc], ads7846.h, ts_osk.c
 *	Copyright (C) 2002 MontaVista Software
 *	Copyright (C) 2004 Texas Instruments
 *	Copyright (C) 2005 Dirk Behme
 *  - ad7877.c
 *	Copyright (C) 2006-2008 Analog Devices Inc.
 */

#include <linux/device.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/property.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/gpio/driver.h>

#include <linux/input/touchscreen.h>
#include <linux/module.h>
#include "ad7879.h"

#define AD7879_REG_ZEROS		0
#define AD7879_REG_CTRL1		1
#define AD7879_REG_CTRL2		2
#define AD7879_REG_CTRL3		3
#define AD7879_REG_AUX1HIGH		4
#define AD7879_REG_AUX1LOW		5
#define AD7879_REG_TEMP1HIGH		6
#define AD7879_REG_TEMP1LOW		7
#define AD7879_REG_XPLUS		8
#define AD7879_REG_YPLUS		9
#define AD7879_REG_Z1			10
#define AD7879_REG_Z2			11
#define AD7879_REG_AUXVBAT		12
#define AD7879_REG_TEMP			13
#define AD7879_REG_REVID		14

/* Control REG 1 */
#define AD7879_TMR(x)			((x & 0xFF) << 0)
#define AD7879_ACQ(x)			((x & 0x3) << 8)
#define AD7879_MODE_NOC			(0 << 10)	/* Do not convert */
#define AD7879_MODE_SCC			(1 << 10)	/* Single channel conversion */
#define AD7879_MODE_SEQ0		(2 << 10)	/* Sequence 0 in Slave Mode */
#define AD7879_MODE_SEQ1		(3 << 10)	/* Sequence 1 in Master Mode */
#define AD7879_MODE_INT			(1 << 15)	/* PENIRQ disabled INT enabled */

/* Control REG 2 */
#define AD7879_FCD(x)			((x & 0x3) << 0)
#define AD7879_RESET			(1 << 4)
#define AD7879_MFS(x)			((x & 0x3) << 5)
#define AD7879_AVG(x)			((x & 0x3) << 7)
#define	AD7879_SER			(1 << 9)	/* non-differential */
#define	AD7879_DFR			(0 << 9)	/* differential */
#define AD7879_GPIOPOL			(1 << 10)
#define AD7879_GPIODIR			(1 << 11)
#define AD7879_GPIO_DATA		(1 << 12)
#define AD7879_GPIO_EN			(1 << 13)
#define AD7879_PM(x)			((x & 0x3) << 14)
#define AD7879_PM_SHUTDOWN		(0)
#define AD7879_PM_DYN			(1)
#define AD7879_PM_FULLON		(2)

/* Control REG 3 */
#define AD7879_TEMPMASK_BIT		(1<<15)
#define AD7879_AUXVBATMASK_BIT		(1<<14)
#define AD7879_INTMODE_BIT		(1<<13)
#define AD7879_GPIOALERTMASK_BIT	(1<<12)
#define AD7879_AUXLOW_BIT		(1<<11)
#define AD7879_AUXHIGH_BIT		(1<<10)
#define AD7879_TEMPLOW_BIT		(1<<9)
#define AD7879_TEMPHIGH_BIT		(1<<8)
#define AD7879_YPLUS_BIT		(1<<7)
#define AD7879_XPLUS_BIT		(1<<6)
#define AD7879_Z1_BIT			(1<<5)
#define AD7879_Z2_BIT			(1<<4)
#define AD7879_AUX_BIT			(1<<3)
#define AD7879_VBAT_BIT			(1<<2)
#define AD7879_TEMP_BIT			(1<<1)

enum {
	AD7879_SEQ_YPOS  = 0,
	AD7879_SEQ_XPOS  = 1,
	AD7879_SEQ_Z1    = 2,
	AD7879_SEQ_Z2    = 3,
	AD7879_NR_SENSE  = 4,
};

#define	MAX_12BIT			((1<<12)-1)
#define	TS_PEN_UP_TIMEOUT		msecs_to_jiffies(50)

struct ad7879 {
	struct regmap		*regmap;
	struct device		*dev;
	struct input_dev	*input;
	struct timer_list	timer;
#ifdef CONFIG_GPIOLIB
	struct gpio_chip	gc;
	struct mutex		mutex;
#endif
	unsigned int		irq;
	bool			disabled;	/* P: input->mutex */
	bool			suspended;	/* P: input->mutex */
	bool			swap_xy;
	u16			conversion_data[AD7879_NR_SENSE];
	char			phys[32];
	u8			first_conversion_delay;
	u8			acquisition_time;
	u8			averaging;
	u8			pen_down_acc_interval;
	u8			median;
	u16			x_plate_ohms;
	u16			cmd_crtl1;
	u16			cmd_crtl2;
	u16			cmd_crtl3;
	int			x;
	int			y;
	int			Rt;
};

static int ad7879_read(struct ad7879 *ts, u8 reg)
{
	unsigned int val;
	int error;

	error = regmap_read(ts->regmap, reg, &val);
	if (error) {
		dev_err(ts->dev, "failed to read register %#02x: %d\n",
			reg, error);
		return error;
	}

	return val;
}

static int ad7879_write(struct ad7879 *ts, u8 reg, u16 val)
{
	int error;

	error = regmap_write(ts->regmap, reg, val);
	if (error) {
		dev_err(ts->dev,
			"failed to write %#04x to register %#02x: %d\n",
			val, reg, error);
		return error;
	}

	return 0;
}

static int ad7879_report(struct ad7879 *ts)
{
	struct input_dev *input_dev = ts->input;
	unsigned Rt;
	u16 x, y, z1, z2;

	x = ts->conversion_data[AD7879_SEQ_XPOS] & MAX_12BIT;
	y = ts->conversion_data[AD7879_SEQ_YPOS] & MAX_12BIT;
	z1 = ts->conversion_data[AD7879_SEQ_Z1] & MAX_12BIT;
	z2 = ts->conversion_data[AD7879_SEQ_Z2] & MAX_12BIT;

	if (ts->swap_xy)
		swap(x, y);

	/*
	 * The samples processed here are already preprocessed by the AD7879.
	 * The preprocessing function consists of a median and an averaging
	 * filter.  The combination of these two techniques provides a robust
	 * solution, discarding the spurious noise in the signal and keeping
	 * only the data of interest.  The size of both filters is
	 * programmable. (dev.platform_data, see linux/platform_data/ad7879.h)
	 * Other user-programmable conversion controls include variable
	 * acquisition time, and first conversion delay. Up to 16 averages can
	 * be taken per conversion.
	 */

	if (likely(x && z1)) {
		/* compute touch pressure resistance using equation #1 */
		Rt = (z2 - z1) * x * ts->x_plate_ohms;
		Rt /= z1;
		Rt = (Rt + 2047) >> 12;

		/*
		 * Sample found inconsistent, pressure is beyond
		 * the maximum. Don't report it to user space.
		 */
		if (Rt > input_abs_get_max(input_dev, ABS_PRESSURE))
			return -EINVAL;

		/*
		 * Note that we delay reporting events by one sample.
		 * This is done to avoid reporting last sample of the
		 * touch sequence, which may be incomplete if finger
		 * leaves the surface before last reading is taken.
		 */
		if (timer_pending(&ts->timer)) {
			/* Touch continues */
			input_report_key(input_dev, BTN_TOUCH, 1);
			input_report_abs(input_dev, ABS_X, ts->x);
			input_report_abs(input_dev, ABS_Y, ts->y);
			input_report_abs(input_dev, ABS_PRESSURE, ts->Rt);
			input_sync(input_dev);
		}

		ts->x = x;
		ts->y = y;
		ts->Rt = Rt;

		return 0;
	}

	return -EINVAL;
}

static void ad7879_ts_event_release(struct ad7879 *ts)
{
	struct input_dev *input_dev = ts->input;

	input_report_abs(input_dev, ABS_PRESSURE, 0);
	input_report_key(input_dev, BTN_TOUCH, 0);
	input_sync(input_dev);
}

static void ad7879_timer(struct timer_list *t)
{
	struct ad7879 *ts = from_timer(ts, t, timer);

	ad7879_ts_event_release(ts);
}

static irqreturn_t ad7879_irq(int irq, void *handle)
{
	struct ad7879 *ts = handle;
	int error;

	error = regmap_bulk_read(ts->regmap, AD7879_REG_XPLUS,
				 ts->conversion_data, AD7879_NR_SENSE);
	if (error)
		dev_err_ratelimited(ts->dev, "failed to read %#02x: %d\n",
				    AD7879_REG_XPLUS, error);
	else if (!ad7879_report(ts))
		mod_timer(&ts->timer, jiffies + TS_PEN_UP_TIMEOUT);

	return IRQ_HANDLED;
}

static void __ad7879_enable(struct ad7879 *ts)
{
	ad7879_write(ts, AD7879_REG_CTRL2, ts->cmd_crtl2);
	ad7879_write(ts, AD7879_REG_CTRL3, ts->cmd_crtl3);
	ad7879_write(ts, AD7879_REG_CTRL1, ts->cmd_crtl1);

	enable_irq(ts->irq);
}

static void __ad7879_disable(struct ad7879 *ts)
{
	u16 reg = (ts->cmd_crtl2 & ~AD7879_PM(-1)) |
		AD7879_PM(AD7879_PM_SHUTDOWN);
	disable_irq(ts->irq);

	if (del_timer_sync(&ts->timer))
		ad7879_ts_event_release(ts);

	ad7879_write(ts, AD7879_REG_CTRL2, reg);
}


static int ad7879_open(struct input_dev *input)
{
	struct ad7879 *ts = input_get_drvdata(input);

	/* protected by input->mutex */
	if (!ts->disabled && !ts->suspended)
		__ad7879_enable(ts);

	return 0;
}

static void ad7879_close(struct input_dev *input)
{
	struct ad7879 *ts = input_get_drvdata(input);

	/* protected by input->mutex */
	if (!ts->disabled && !ts->suspended)
		__ad7879_disable(ts);
}

static int __maybe_unused ad7879_suspend(struct device *dev)
{
	struct ad7879 *ts = dev_get_drvdata(dev);

	mutex_lock(&ts->input->mutex);

	if (!ts->suspended && !ts->disabled && input_device_enabled(ts->input))
		__ad7879_disable(ts);

	ts->suspended = true;

	mutex_unlock(&ts->input->mutex);

	return 0;
}

static int __maybe_unused ad7879_resume(struct device *dev)
{
	struct ad7879 *ts = dev_get_drvdata(dev);

	mutex_lock(&ts->input->mutex);

	if (ts->suspended && !ts->disabled && input_device_enabled(ts->input))
		__ad7879_enable(ts);

	ts->suspended = false;

	mutex_unlock(&ts->input->mutex);

	return 0;
}

SIMPLE_DEV_PM_OPS(ad7879_pm_ops, ad7879_suspend, ad7879_resume);
EXPORT_SYMBOL(ad7879_pm_ops);

static void ad7879_toggle(struct ad7879 *ts, bool disable)
{
	mutex_lock(&ts->input->mutex);

	if (!ts->suspended && input_device_enabled(ts->input)) {

		if (disable) {
			if (ts->disabled)
				__ad7879_enable(ts);
		} else {
			if (!ts->disabled)
				__ad7879_disable(ts);
		}
	}

	ts->disabled = disable;

	mutex_unlock(&ts->input->mutex);
}

static ssize_t ad7879_disable_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct ad7879 *ts = dev_get_drvdata(dev);

	return sprintf(buf, "%u\n", ts->disabled);
}

static ssize_t ad7879_disable_store(struct device *dev,
				     struct device_attribute *attr,
				     const char *buf, size_t count)
{
	struct ad7879 *ts = dev_get_drvdata(dev);
	unsigned int val;
	int error;

	error = kstrtouint(buf, 10, &val);
	if (error)
		return error;

	ad7879_toggle(ts, val);

	return count;
}

static DEVICE_ATTR(disable, 0664, ad7879_disable_show, ad7879_disable_store);

static struct attribute *ad7879_attributes[] = {
	&dev_attr_disable.attr,
	NULL
};

static const struct attribute_group ad7879_attr_group = {
	.attrs = ad7879_attributes,
};

#ifdef CONFIG_GPIOLIB
static int ad7879_gpio_direction_input(struct gpio_chip *chip,
					unsigned gpio)
{
	struct ad7879 *ts = gpiochip_get_data(chip);
	int err;

	mutex_lock(&ts->mutex);
	ts->cmd_crtl2 |= AD7879_GPIO_EN | AD7879_GPIODIR | AD7879_GPIOPOL;
	err = ad7879_write(ts, AD7879_REG_CTRL2, ts->cmd_crtl2);
	mutex_unlock(&ts->mutex);

	return err;
}

static int ad7879_gpio_direction_output(struct gpio_chip *chip,
					unsigned gpio, int level)
{
	struct ad7879 *ts = gpiochip_get_data(chip);
	int err;

	mutex_lock(&ts->mutex);
	ts->cmd_crtl2 &= ~AD7879_GPIODIR;
	ts->cmd_crtl2 |= AD7879_GPIO_EN | AD7879_GPIOPOL;
	if (level)
		ts->cmd_crtl2 |= AD7879_GPIO_DATA;
	else
		ts->cmd_crtl2 &= ~AD7879_GPIO_DATA;

	err = ad7879_write(ts, AD7879_REG_CTRL2, ts->cmd_crtl2);
	mutex_unlock(&ts->mutex);

	return err;
}

static int ad7879_gpio_get_value(struct gpio_chip *chip, unsigned gpio)
{
	struct ad7879 *ts = gpiochip_get_data(chip);
	u16 val;

	mutex_lock(&ts->mutex);
	val = ad7879_read(ts, AD7879_REG_CTRL2);
	mutex_unlock(&ts->mutex);

	return !!(val & AD7879_GPIO_DATA);
}

static void ad7879_gpio_set_value(struct gpio_chip *chip,
				  unsigned gpio, int value)
{
	struct ad7879 *ts = gpiochip_get_data(chip);

	mutex_lock(&ts->mutex);
	if (value)
		ts->cmd_crtl2 |= AD7879_GPIO_DATA;
	else
		ts->cmd_crtl2 &= ~AD7879_GPIO_DATA;

	ad7879_write(ts, AD7879_REG_CTRL2, ts->cmd_crtl2);
	mutex_unlock(&ts->mutex);
}

static int ad7879_gpio_add(struct ad7879 *ts)
{
	int ret = 0;

	mutex_init(&ts->mutex);

	/* Do not create a chip unless flagged for it */
	if (!device_property_read_bool(ts->dev, "gpio-controller"))
		return 0;

	ts->gc.direction_input = ad7879_gpio_direction_input;
	ts->gc.direction_output = ad7879_gpio_direction_output;
	ts->gc.get = ad7879_gpio_get_value;
	ts->gc.set = ad7879_gpio_set_value;
	ts->gc.can_sleep = 1;
	ts->gc.base = -1;
	ts->gc.ngpio = 1;
	ts->gc.label = "AD7879-GPIO";
	ts->gc.owner = THIS_MODULE;
	ts->gc.parent = ts->dev;

	ret = devm_gpiochip_add_data(ts->dev, &ts->gc, ts);
	if (ret)
		dev_err(ts->dev, "failed to register gpio %d\n",
			ts->gc.base);

	return ret;
}
#else
static int ad7879_gpio_add(struct ad7879 *ts)
{
	return 0;
}
#endif

static int ad7879_parse_dt(struct device *dev, struct ad7879 *ts)
{
	int err;
	u32 tmp;

	err = device_property_read_u32(dev, "adi,resistance-plate-x", &tmp);
	if (err) {
		dev_err(dev, "failed to get resistance-plate-x property\n");
		return err;
	}
	ts->x_plate_ohms = (u16)tmp;

	device_property_read_u8(dev, "adi,first-conversion-delay",
				&ts->first_conversion_delay);
	device_property_read_u8(dev, "adi,acquisition-time",
				&ts->acquisition_time);
	device_property_read_u8(dev, "adi,median-filter-size", &ts->median);
	device_property_read_u8(dev, "adi,averaging", &ts->averaging);
	device_property_read_u8(dev, "adi,conversion-interval",
				&ts->pen_down_acc_interval);

	ts->swap_xy = device_property_read_bool(dev, "touchscreen-swapped-x-y");

	return 0;
}

int ad7879_probe(struct device *dev, struct regmap *regmap,
		 int irq, u16 bustype, u8 devid)
{
	struct ad7879 *ts;
	struct input_dev *input_dev;
	int err;
	u16 revid;

	if (irq <= 0) {
		dev_err(dev, "No IRQ specified\n");
		return -EINVAL;
	}

	ts = devm_kzalloc(dev, sizeof(*ts), GFP_KERNEL);
	if (!ts)
		return -ENOMEM;

	err = ad7879_parse_dt(dev, ts);
	if (err)
		return err;

	input_dev = devm_input_allocate_device(dev);
	if (!input_dev) {
		dev_err(dev, "Failed to allocate input device\n");
		return -ENOMEM;
	}

	ts->dev = dev;
	ts->input = input_dev;
	ts->irq = irq;
	ts->regmap = regmap;

	timer_setup(&ts->timer, ad7879_timer, 0);
	snprintf(ts->phys, sizeof(ts->phys), "%s/input0", dev_name(dev));

	input_dev->name = "AD7879 Touchscreen";
	input_dev->phys = ts->phys;
	input_dev->dev.parent = dev;
	input_dev->id.bustype = bustype;

	input_dev->open = ad7879_open;
	input_dev->close = ad7879_close;

	input_set_drvdata(input_dev, ts);

	input_set_capability(input_dev, EV_KEY, BTN_TOUCH);

	input_set_abs_params(input_dev, ABS_X, 0, MAX_12BIT, 0, 0);
	input_set_abs_params(input_dev, ABS_Y, 0, MAX_12BIT, 0, 0);
	input_set_capability(input_dev, EV_ABS, ABS_PRESSURE);
	touchscreen_parse_properties(input_dev, false, NULL);
	if (!input_abs_get_max(input_dev, ABS_PRESSURE)) {
		dev_err(dev, "Touchscreen pressure is not specified\n");
		return -EINVAL;
	}

	err = ad7879_write(ts, AD7879_REG_CTRL2, AD7879_RESET);
	if (err < 0) {
		dev_err(dev, "Failed to write %s\n", input_dev->name);
		return err;
	}

	revid = ad7879_read(ts, AD7879_REG_REVID);
	input_dev->id.product = (revid & 0xff);
	input_dev->id.version = revid >> 8;
	if (input_dev->id.product != devid) {
		dev_err(dev, "Failed to probe %s (%x vs %x)\n",
			input_dev->name, devid, revid);
		return -ENODEV;
	}

	ts->cmd_crtl3 = AD7879_YPLUS_BIT |
			AD7879_XPLUS_BIT |
			AD7879_Z2_BIT |
			AD7879_Z1_BIT |
			AD7879_TEMPMASK_BIT |
			AD7879_AUXVBATMASK_BIT |
			AD7879_GPIOALERTMASK_BIT;

	ts->cmd_crtl2 = AD7879_PM(AD7879_PM_DYN) | AD7879_DFR |
			AD7879_AVG(ts->averaging) |
			AD7879_MFS(ts->median) |
			AD7879_FCD(ts->first_conversion_delay);

	ts->cmd_crtl1 = AD7879_MODE_INT | AD7879_MODE_SEQ1 |
			AD7879_ACQ(ts->acquisition_time) |
			AD7879_TMR(ts->pen_down_acc_interval);

	err = devm_request_threaded_irq(dev, ts->irq, NULL, ad7879_irq,
					IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
					dev_name(dev), ts);
	if (err) {
		dev_err(dev, "Failed to request IRQ: %d\n", err);
		return err;
	}

	__ad7879_disable(ts);

	err = devm_device_add_group(dev, &ad7879_attr_group);
	if (err)
		return err;

	err = ad7879_gpio_add(ts);
	if (err)
		return err;

	err = input_register_device(input_dev);
	if (err)
		return err;

	dev_set_drvdata(dev, ts);

	return 0;
}
EXPORT_SYMBOL(ad7879_probe);

MODULE_AUTHOR("Michael Hennerich <michael.hennerich@analog.com>");
MODULE_DESCRIPTION("AD7879(-1) touchscreen Driver");
MODULE_LICENSE("GPL");
