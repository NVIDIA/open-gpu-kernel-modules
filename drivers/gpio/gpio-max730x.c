// SPDX-License-Identifier: GPL-2.0-only
/**
 * Copyright (C) 2006 Juergen Beisert, Pengutronix
 * Copyright (C) 2008 Guennadi Liakhovetski, Pengutronix
 * Copyright (C) 2009 Wolfram Sang, Pengutronix
 *
 * The Maxim MAX7300/1 device is an I2C/SPI driven GPIO expander. There are
 * 28 GPIOs. 8 of them can trigger an interrupt. See datasheet for more
 * details
 * Note:
 * - DIN must be stable at the rising edge of clock.
 * - when writing:
 *   - always clock in 16 clocks at once
 *   - at DIN: D15 first, D0 last
 *   - D0..D7 = databyte, D8..D14 = commandbyte
 *   - D15 = low -> write command
 * - when reading
 *   - always clock in 16 clocks at once
 *   - at DIN: D15 first, D0 last
 *   - D0..D7 = dummy, D8..D14 = register address
 *   - D15 = high -> read command
 *   - raise CS and assert it again
 *   - always clock in 16 clocks at once
 *   - at DOUT: D15 first, D0 last
 *   - D0..D7 contains the data from the first cycle
 *
 * The driver exports a standard gpiochip interface
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/spi/max7301.h>
#include <linux/gpio/driver.h>
#include <linux/slab.h>

/*
 * Pin configurations, see MAX7301 datasheet page 6
 */
#define PIN_CONFIG_MASK 0x03
#define PIN_CONFIG_IN_PULLUP 0x03
#define PIN_CONFIG_IN_WO_PULLUP 0x02
#define PIN_CONFIG_OUT 0x01

#define PIN_NUMBER 28

static int max7301_direction_input(struct gpio_chip *chip, unsigned offset)
{
	struct max7301 *ts = container_of(chip, struct max7301, chip);
	u8 *config;
	u8 offset_bits, pin_config;
	int ret;

	/* First 4 pins are unused in the controller */
	offset += 4;
	offset_bits = (offset & 3) << 1;

	config = &ts->port_config[offset >> 2];

	if (ts->input_pullup_active & BIT(offset))
		pin_config = PIN_CONFIG_IN_PULLUP;
	else
		pin_config = PIN_CONFIG_IN_WO_PULLUP;

	mutex_lock(&ts->lock);

	*config = (*config & ~(PIN_CONFIG_MASK << offset_bits))
			   | (pin_config << offset_bits);

	ret = ts->write(ts->dev, 0x08 + (offset >> 2), *config);

	mutex_unlock(&ts->lock);

	return ret;
}

static int __max7301_set(struct max7301 *ts, unsigned offset, int value)
{
	if (value) {
		ts->out_level |= 1 << offset;
		return ts->write(ts->dev, 0x20 + offset, 0x01);
	} else {
		ts->out_level &= ~(1 << offset);
		return ts->write(ts->dev, 0x20 + offset, 0x00);
	}
}

static int max7301_direction_output(struct gpio_chip *chip, unsigned offset,
				    int value)
{
	struct max7301 *ts = container_of(chip, struct max7301, chip);
	u8 *config;
	u8 offset_bits;
	int ret;

	/* First 4 pins are unused in the controller */
	offset += 4;
	offset_bits = (offset & 3) << 1;

	config = &ts->port_config[offset >> 2];

	mutex_lock(&ts->lock);

	*config = (*config & ~(PIN_CONFIG_MASK << offset_bits))
			   | (PIN_CONFIG_OUT << offset_bits);

	ret = __max7301_set(ts, offset, value);

	if (!ret)
		ret = ts->write(ts->dev, 0x08 + (offset >> 2), *config);

	mutex_unlock(&ts->lock);

	return ret;
}

static int max7301_get(struct gpio_chip *chip, unsigned offset)
{
	struct max7301 *ts = gpiochip_get_data(chip);
	int config, level = -EINVAL;

	/* First 4 pins are unused in the controller */
	offset += 4;

	mutex_lock(&ts->lock);

	config = (ts->port_config[offset >> 2] >> ((offset & 3) << 1))
			& PIN_CONFIG_MASK;

	switch (config) {
	case PIN_CONFIG_OUT:
		/* Output: return cached level */
		level =  !!(ts->out_level & (1 << offset));
		break;
	case PIN_CONFIG_IN_WO_PULLUP:
	case PIN_CONFIG_IN_PULLUP:
		/* Input: read out */
		level = ts->read(ts->dev, 0x20 + offset) & 0x01;
	}
	mutex_unlock(&ts->lock);

	return level;
}

static void max7301_set(struct gpio_chip *chip, unsigned offset, int value)
{
	struct max7301 *ts = gpiochip_get_data(chip);

	/* First 4 pins are unused in the controller */
	offset += 4;

	mutex_lock(&ts->lock);

	__max7301_set(ts, offset, value);

	mutex_unlock(&ts->lock);
}

int __max730x_probe(struct max7301 *ts)
{
	struct device *dev = ts->dev;
	struct max7301_platform_data *pdata;
	int i, ret;

	pdata = dev_get_platdata(dev);

	mutex_init(&ts->lock);
	dev_set_drvdata(dev, ts);

	/* Power up the chip and disable IRQ output */
	ts->write(dev, 0x04, 0x01);

	if (pdata) {
		ts->input_pullup_active = pdata->input_pullup_active;
		ts->chip.base = pdata->base;
	} else {
		ts->chip.base = -1;
	}
	ts->chip.label = dev->driver->name;

	ts->chip.direction_input = max7301_direction_input;
	ts->chip.get = max7301_get;
	ts->chip.direction_output = max7301_direction_output;
	ts->chip.set = max7301_set;

	ts->chip.ngpio = PIN_NUMBER;
	ts->chip.can_sleep = true;
	ts->chip.parent = dev;
	ts->chip.owner = THIS_MODULE;

	/*
	 * initialize pullups according to platform data and cache the
	 * register values for later use.
	 */
	for (i = 1; i < 8; i++) {
		int j;
		/*
		 * initialize port_config with "0xAA", which means
		 * input with internal pullup disabled. This is needed
		 * to avoid writing zeros (in the inner for loop),
		 * which is not allowed according to the datasheet.
		 */
		ts->port_config[i] = 0xAA;
		for (j = 0; j < 4; j++) {
			int offset = (i - 1) * 4 + j;
			ret = max7301_direction_input(&ts->chip, offset);
			if (ret)
				goto exit_destroy;
		}
	}

	ret = gpiochip_add_data(&ts->chip, ts);
	if (!ret)
		return ret;

exit_destroy:
	mutex_destroy(&ts->lock);
	return ret;
}
EXPORT_SYMBOL_GPL(__max730x_probe);

int __max730x_remove(struct device *dev)
{
	struct max7301 *ts = dev_get_drvdata(dev);

	if (ts == NULL)
		return -ENODEV;

	/* Power down the chip and disable IRQ output */
	ts->write(dev, 0x04, 0x00);
	gpiochip_remove(&ts->chip);
	mutex_destroy(&ts->lock);
	return 0;
}
EXPORT_SYMBOL_GPL(__max730x_remove);

MODULE_AUTHOR("Juergen Beisert, Wolfram Sang");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("MAX730x GPIO-Expanders, generic parts");
