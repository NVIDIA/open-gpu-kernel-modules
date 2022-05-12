// SPDX-License-Identifier: GPL-2.0-only
/*
 * l4f00242t03.c -- support for Epson L4F00242T03 LCD
 *
 * Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * Copyright (c) 2009 Alberto Panizzo <maramaopercheseimorto@gmail.com>
 *	Inspired by Marek Vasut work in l4f00242t03.c
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/gpio/consumer.h>
#include <linux/lcd.h>
#include <linux/slab.h>
#include <linux/regulator/consumer.h>
#include <linux/spi/spi.h>

struct l4f00242t03_priv {
	struct spi_device	*spi;
	struct lcd_device	*ld;
	int lcd_state;
	struct regulator *io_reg;
	struct regulator *core_reg;
	struct gpio_desc *reset;
	struct gpio_desc *enable;
};

static void l4f00242t03_reset(struct gpio_desc *gpiod)
{
	pr_debug("l4f00242t03_reset.\n");
	gpiod_set_value(gpiod, 1);
	mdelay(100);
	gpiod_set_value(gpiod, 0);
	mdelay(10);	/* tRES >= 100us */
	gpiod_set_value(gpiod, 1);
	mdelay(20);
}

#define param(x) ((x) | 0x100)

static void l4f00242t03_lcd_init(struct spi_device *spi)
{
	struct l4f00242t03_priv *priv = spi_get_drvdata(spi);
	const u16 cmd[] = { 0x36, param(0), 0x3A, param(0x60) };
	int ret;

	dev_dbg(&spi->dev, "initializing LCD\n");

	ret = regulator_set_voltage(priv->io_reg, 1800000, 1800000);
	if (ret) {
		dev_err(&spi->dev, "failed to set the IO regulator voltage.\n");
		return;
	}
	ret = regulator_enable(priv->io_reg);
	if (ret) {
		dev_err(&spi->dev, "failed to enable the IO regulator.\n");
		return;
	}

	ret = regulator_set_voltage(priv->core_reg, 2800000, 2800000);
	if (ret) {
		dev_err(&spi->dev, "failed to set the core regulator voltage.\n");
		regulator_disable(priv->io_reg);
		return;
	}
	ret = regulator_enable(priv->core_reg);
	if (ret) {
		dev_err(&spi->dev, "failed to enable the core regulator.\n");
		regulator_disable(priv->io_reg);
		return;
	}

	l4f00242t03_reset(priv->reset);

	gpiod_set_value(priv->enable, 1);
	msleep(60);
	spi_write(spi, (const u8 *)cmd, ARRAY_SIZE(cmd) * sizeof(u16));
}

static void l4f00242t03_lcd_powerdown(struct spi_device *spi)
{
	struct l4f00242t03_priv *priv = spi_get_drvdata(spi);

	dev_dbg(&spi->dev, "Powering down LCD\n");

	gpiod_set_value(priv->enable, 0);

	regulator_disable(priv->io_reg);
	regulator_disable(priv->core_reg);
}

static int l4f00242t03_lcd_power_get(struct lcd_device *ld)
{
	struct l4f00242t03_priv *priv = lcd_get_data(ld);

	return priv->lcd_state;
}

static int l4f00242t03_lcd_power_set(struct lcd_device *ld, int power)
{
	struct l4f00242t03_priv *priv = lcd_get_data(ld);
	struct spi_device *spi = priv->spi;

	const u16 slpout = 0x11;
	const u16 dison = 0x29;

	const u16 slpin = 0x10;
	const u16 disoff = 0x28;

	if (power <= FB_BLANK_NORMAL) {
		if (priv->lcd_state <= FB_BLANK_NORMAL) {
			/* Do nothing, the LCD is running */
		} else if (priv->lcd_state < FB_BLANK_POWERDOWN) {
			dev_dbg(&spi->dev, "Resuming LCD\n");

			spi_write(spi, (const u8 *)&slpout, sizeof(u16));
			msleep(60);
			spi_write(spi, (const u8 *)&dison, sizeof(u16));
		} else {
			/* priv->lcd_state == FB_BLANK_POWERDOWN */
			l4f00242t03_lcd_init(spi);
			priv->lcd_state = FB_BLANK_VSYNC_SUSPEND;
			l4f00242t03_lcd_power_set(priv->ld, power);
		}
	} else if (power < FB_BLANK_POWERDOWN) {
		if (priv->lcd_state <= FB_BLANK_NORMAL) {
			/* Send the display in standby */
			dev_dbg(&spi->dev, "Standby the LCD\n");

			spi_write(spi, (const u8 *)&disoff, sizeof(u16));
			msleep(60);
			spi_write(spi, (const u8 *)&slpin, sizeof(u16));
		} else if (priv->lcd_state < FB_BLANK_POWERDOWN) {
			/* Do nothing, the LCD is already in standby */
		} else {
			/* priv->lcd_state == FB_BLANK_POWERDOWN */
			l4f00242t03_lcd_init(spi);
			priv->lcd_state = FB_BLANK_UNBLANK;
			l4f00242t03_lcd_power_set(ld, power);
		}
	} else {
		/* power == FB_BLANK_POWERDOWN */
		if (priv->lcd_state != FB_BLANK_POWERDOWN) {
			/* Clear the screen before shutting down */
			spi_write(spi, (const u8 *)&disoff, sizeof(u16));
			msleep(60);
			l4f00242t03_lcd_powerdown(spi);
		}
	}

	priv->lcd_state = power;

	return 0;
}

static struct lcd_ops l4f_ops = {
	.set_power	= l4f00242t03_lcd_power_set,
	.get_power	= l4f00242t03_lcd_power_get,
};

static int l4f00242t03_probe(struct spi_device *spi)
{
	struct l4f00242t03_priv *priv;

	priv = devm_kzalloc(&spi->dev, sizeof(struct l4f00242t03_priv),
				GFP_KERNEL);
	if (priv == NULL)
		return -ENOMEM;

	spi_set_drvdata(spi, priv);
	spi->bits_per_word = 9;
	spi_setup(spi);

	priv->spi = spi;

	priv->reset = devm_gpiod_get(&spi->dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(priv->reset)) {
		dev_err(&spi->dev,
			"Unable to get the lcd l4f00242t03 reset gpio.\n");
		return PTR_ERR(priv->reset);
	}
	gpiod_set_consumer_name(priv->reset, "lcd l4f00242t03 reset");

	priv->enable = devm_gpiod_get(&spi->dev, "enable", GPIOD_OUT_LOW);
	if (IS_ERR(priv->enable)) {
		dev_err(&spi->dev,
			"Unable to get the lcd l4f00242t03 data en gpio.\n");
		return PTR_ERR(priv->enable);
	}
	gpiod_set_consumer_name(priv->enable, "lcd l4f00242t03 data enable");

	priv->io_reg = devm_regulator_get(&spi->dev, "vdd");
	if (IS_ERR(priv->io_reg)) {
		dev_err(&spi->dev, "%s: Unable to get the IO regulator\n",
		       __func__);
		return PTR_ERR(priv->io_reg);
	}

	priv->core_reg = devm_regulator_get(&spi->dev, "vcore");
	if (IS_ERR(priv->core_reg)) {
		dev_err(&spi->dev, "%s: Unable to get the core regulator\n",
		       __func__);
		return PTR_ERR(priv->core_reg);
	}

	priv->ld = devm_lcd_device_register(&spi->dev, "l4f00242t03", &spi->dev,
					priv, &l4f_ops);
	if (IS_ERR(priv->ld))
		return PTR_ERR(priv->ld);

	/* Init the LCD */
	l4f00242t03_lcd_init(spi);
	priv->lcd_state = FB_BLANK_VSYNC_SUSPEND;
	l4f00242t03_lcd_power_set(priv->ld, FB_BLANK_UNBLANK);

	dev_info(&spi->dev, "Epson l4f00242t03 lcd probed.\n");

	return 0;
}

static int l4f00242t03_remove(struct spi_device *spi)
{
	struct l4f00242t03_priv *priv = spi_get_drvdata(spi);

	l4f00242t03_lcd_power_set(priv->ld, FB_BLANK_POWERDOWN);
	return 0;
}

static void l4f00242t03_shutdown(struct spi_device *spi)
{
	struct l4f00242t03_priv *priv = spi_get_drvdata(spi);

	if (priv)
		l4f00242t03_lcd_power_set(priv->ld, FB_BLANK_POWERDOWN);

}

static struct spi_driver l4f00242t03_driver = {
	.driver = {
		.name	= "l4f00242t03",
	},
	.probe		= l4f00242t03_probe,
	.remove		= l4f00242t03_remove,
	.shutdown	= l4f00242t03_shutdown,
};

module_spi_driver(l4f00242t03_driver);

MODULE_AUTHOR("Alberto Panizzo <maramaopercheseimorto@gmail.com>");
MODULE_DESCRIPTION("EPSON L4F00242T03 LCD");
MODULE_LICENSE("GPL v2");
