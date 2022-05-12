/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright 2011-2019 NW Digital Radio
 *
 * Author: Annaliese McDermond <nh6z@nh6z.net>
 *
 * Based on sound/soc/codecs/wm8974 and TI driver for kernel 2.6.27.
 *
 */

#include <linux/spi/spi.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regmap.h>
#include <sound/soc.h>

#include "tlv320aic32x4.h"

static int aic32x4_spi_probe(struct spi_device *spi)
{
	struct regmap *regmap;
	struct regmap_config config;

	config = aic32x4_regmap_config;
	config.reg_bits = 7;
	config.pad_bits = 1;
	config.val_bits = 8;
	config.read_flag_mask = 0x01;

	regmap = devm_regmap_init_spi(spi, &config);
	return aic32x4_probe(&spi->dev, regmap);
}

static int aic32x4_spi_remove(struct spi_device *spi)
{
	return aic32x4_remove(&spi->dev);
}

static const struct spi_device_id aic32x4_spi_id[] = {
	{ "tlv320aic32x4", 0 },
	{ "tlv320aic32x6", 1 },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(spi, aic32x4_spi_id);

static const struct of_device_id aic32x4_of_id[] = {
	{ .compatible = "ti,tlv320aic32x4", },
	{ .compatible = "ti,tlv320aic32x6", },
	{ /* senitel */ }
};
MODULE_DEVICE_TABLE(of, aic32x4_of_id);

static struct spi_driver aic32x4_spi_driver = {
	.driver = {
		.name = "tlv320aic32x4",
		.owner = THIS_MODULE,
		.of_match_table = aic32x4_of_id,
	},
	.probe =    aic32x4_spi_probe,
	.remove =   aic32x4_spi_remove,
	.id_table = aic32x4_spi_id,
};

module_spi_driver(aic32x4_spi_driver);

MODULE_DESCRIPTION("ASoC TLV320AIC32x4 codec driver SPI");
MODULE_AUTHOR("Annaliese McDermond <nh6z@nh6z.net>");
MODULE_LICENSE("GPL");
