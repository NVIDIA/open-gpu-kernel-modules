// SPDX-License-Identifier: GPL-2.0-only
/*
 * ALSA SoC TLV320AIC23 codec driver SPI interface
 *
 * Author:      Arun KS, <arunks@mistralsolutions.com>
 * Copyright:   (C) 2008 Mistral Solutions Pvt Ltd.,
 *
 * Based on sound/soc/codecs/wm8731.c by Richard Purdie
 */

#include <linux/module.h>
#include <linux/regmap.h>
#include <linux/spi/spi.h>
#include <sound/soc.h>

#include "tlv320aic23.h"

static int aic23_spi_probe(struct spi_device *spi)
{
	int ret;
	struct regmap *regmap;

	dev_dbg(&spi->dev, "probing tlv320aic23 spi device\n");

	spi->mode = SPI_MODE_0;
	ret = spi_setup(spi);
	if (ret < 0)
		return ret;

	regmap = devm_regmap_init_spi(spi, &tlv320aic23_regmap);
	return tlv320aic23_probe(&spi->dev, regmap);
}

static struct spi_driver aic23_spi = {
	.driver = {
		.name = "tlv320aic23",
	},
	.probe = aic23_spi_probe,
};

module_spi_driver(aic23_spi);

MODULE_DESCRIPTION("ASoC TLV320AIC23 codec driver SPI");
MODULE_AUTHOR("Arun KS <arunks@mistralsolutions.com>");
MODULE_LICENSE("GPL");
