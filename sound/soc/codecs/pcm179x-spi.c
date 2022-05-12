// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * PCM179X ASoC SPI driver
 *
 * Copyright (c) Amarula Solutions B.V. 2013
 *
 *     Michael Trimarchi <michael@amarulasolutions.com>
 */

#include <linux/module.h>
#include <linux/of.h>
#include <linux/spi/spi.h>
#include <linux/regmap.h>

#include "pcm179x.h"

static int pcm179x_spi_probe(struct spi_device *spi)
{
	struct regmap *regmap;
	int ret;

	regmap = devm_regmap_init_spi(spi, &pcm179x_regmap_config);
	if (IS_ERR(regmap)) {
		ret = PTR_ERR(regmap);
		dev_err(&spi->dev, "Failed to allocate regmap: %d\n", ret);
		return ret;
	}

	return pcm179x_common_init(&spi->dev, regmap);
}

static const struct of_device_id pcm179x_of_match[] = {
	{ .compatible = "ti,pcm1792a", },
	{ }
};
MODULE_DEVICE_TABLE(of, pcm179x_of_match);

static const struct spi_device_id pcm179x_spi_ids[] = {
	{ "pcm179x", 0 },
	{ },
};
MODULE_DEVICE_TABLE(spi, pcm179x_spi_ids);

static struct spi_driver pcm179x_spi_driver = {
	.driver = {
		.name = "pcm179x",
		.of_match_table = of_match_ptr(pcm179x_of_match),
	},
	.id_table = pcm179x_spi_ids,
	.probe = pcm179x_spi_probe,
};

module_spi_driver(pcm179x_spi_driver);

MODULE_DESCRIPTION("ASoC PCM179X SPI driver");
MODULE_AUTHOR("Michael Trimarchi <michael@amarulasolutions.com>");
MODULE_LICENSE("GPL");
