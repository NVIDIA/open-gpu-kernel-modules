// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * 74xx MMIO GPIO driver
 *
 *  Copyright (C) 2014 Alexander Shiyan <shc_work@mail.ru>
 */

#include <linux/err.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/gpio/driver.h>
#include <linux/platform_device.h>

#define MMIO_74XX_DIR_IN	(0 << 8)
#define MMIO_74XX_DIR_OUT	(1 << 8)
#define MMIO_74XX_BIT_CNT(x)	((x) & 0xff)

struct mmio_74xx_gpio_priv {
	struct gpio_chip	gc;
	unsigned		flags;
};

static const struct of_device_id mmio_74xx_gpio_ids[] = {
	{
		.compatible	= "ti,741g125",
		.data		= (const void *)(MMIO_74XX_DIR_IN | 1),
	},
	{
		.compatible	= "ti,742g125",
		.data		= (const void *)(MMIO_74XX_DIR_IN | 2),
	},
	{
		.compatible	= "ti,74125",
		.data		= (const void *)(MMIO_74XX_DIR_IN | 4),
	},
	{
		.compatible	= "ti,74365",
		.data		= (const void *)(MMIO_74XX_DIR_IN | 6),
	},
	{
		.compatible	= "ti,74244",
		.data		= (const void *)(MMIO_74XX_DIR_IN | 8),
	},
	{
		.compatible	= "ti,741624",
		.data		= (const void *)(MMIO_74XX_DIR_IN | 16),
	},
	{
		.compatible	= "ti,741g74",
		.data		= (const void *)(MMIO_74XX_DIR_OUT | 1),
	},
	{
		.compatible	= "ti,7474",
		.data		= (const void *)(MMIO_74XX_DIR_OUT | 2),
	},
	{
		.compatible	= "ti,74175",
		.data		= (const void *)(MMIO_74XX_DIR_OUT | 4),
	},
	{
		.compatible	= "ti,74174",
		.data		= (const void *)(MMIO_74XX_DIR_OUT | 6),
	},
	{
		.compatible	= "ti,74273",
		.data		= (const void *)(MMIO_74XX_DIR_OUT | 8),
	},
	{
		.compatible	= "ti,7416374",
		.data		= (const void *)(MMIO_74XX_DIR_OUT | 16),
	},
	{ }
};
MODULE_DEVICE_TABLE(of, mmio_74xx_gpio_ids);

static int mmio_74xx_get_direction(struct gpio_chip *gc, unsigned offset)
{
	struct mmio_74xx_gpio_priv *priv = gpiochip_get_data(gc);

	if (priv->flags & MMIO_74XX_DIR_OUT)
		return GPIO_LINE_DIRECTION_OUT;

	return  GPIO_LINE_DIRECTION_IN;
}

static int mmio_74xx_dir_in(struct gpio_chip *gc, unsigned int gpio)
{
	struct mmio_74xx_gpio_priv *priv = gpiochip_get_data(gc);

	return (priv->flags & MMIO_74XX_DIR_OUT) ? -ENOTSUPP : 0;
}

static int mmio_74xx_dir_out(struct gpio_chip *gc, unsigned int gpio, int val)
{
	struct mmio_74xx_gpio_priv *priv = gpiochip_get_data(gc);

	if (priv->flags & MMIO_74XX_DIR_OUT) {
		gc->set(gc, gpio, val);
		return 0;
	}

	return -ENOTSUPP;
}

static int mmio_74xx_gpio_probe(struct platform_device *pdev)
{
	struct mmio_74xx_gpio_priv *priv;
	void __iomem *dat;
	int err;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->flags = (uintptr_t)of_device_get_match_data(&pdev->dev);

	dat = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(dat))
		return PTR_ERR(dat);

	err = bgpio_init(&priv->gc, &pdev->dev,
			 DIV_ROUND_UP(MMIO_74XX_BIT_CNT(priv->flags), 8),
			 dat, NULL, NULL, NULL, NULL, 0);
	if (err)
		return err;

	priv->gc.direction_input = mmio_74xx_dir_in;
	priv->gc.direction_output = mmio_74xx_dir_out;
	priv->gc.get_direction = mmio_74xx_get_direction;
	priv->gc.ngpio = MMIO_74XX_BIT_CNT(priv->flags);
	priv->gc.owner = THIS_MODULE;

	platform_set_drvdata(pdev, priv);

	return devm_gpiochip_add_data(&pdev->dev, &priv->gc, priv);
}

static struct platform_driver mmio_74xx_gpio_driver = {
	.driver	= {
		.name		= "74xx-mmio-gpio",
		.of_match_table	= mmio_74xx_gpio_ids,
	},
	.probe	= mmio_74xx_gpio_probe,
};
module_platform_driver(mmio_74xx_gpio_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexander Shiyan <shc_work@mail.ru>");
MODULE_DESCRIPTION("74xx MMIO GPIO driver");
