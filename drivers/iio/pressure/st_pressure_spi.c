// SPDX-License-Identifier: GPL-2.0-only
/*
 * STMicroelectronics pressures driver
 *
 * Copyright 2013 STMicroelectronics Inc.
 *
 * Denis Ciocca <denis.ciocca@st.com>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/spi/spi.h>
#include <linux/iio/iio.h>

#include <linux/iio/common/st_sensors.h>
#include <linux/iio/common/st_sensors_spi.h>
#include "st_pressure.h"

/*
 * For new single-chip sensors use <device_name> as compatible string.
 * For old single-chip devices keep <device_name>-press to maintain
 * compatibility
 */
static const struct of_device_id st_press_of_match[] = {
	{
		.compatible = "st,lps001wp-press",
		.data = LPS001WP_PRESS_DEV_NAME,
	},
	{
		.compatible = "st,lps25h-press",
		.data = LPS25H_PRESS_DEV_NAME,
	},
	{
		.compatible = "st,lps331ap-press",
		.data = LPS331AP_PRESS_DEV_NAME,
	},
	{
		.compatible = "st,lps22hb-press",
		.data = LPS22HB_PRESS_DEV_NAME,
	},
	{
		.compatible = "st,lps33hw",
		.data = LPS33HW_PRESS_DEV_NAME,
	},
	{
		.compatible = "st,lps35hw",
		.data = LPS35HW_PRESS_DEV_NAME,
	},
	{
		.compatible = "st,lps22hh",
		.data = LPS22HH_PRESS_DEV_NAME,
	},
	{},
};
MODULE_DEVICE_TABLE(of, st_press_of_match);

static int st_press_spi_probe(struct spi_device *spi)
{
	const struct st_sensor_settings *settings;
	struct st_sensor_data *press_data;
	struct iio_dev *indio_dev;
	int err;

	st_sensors_dev_name_probe(&spi->dev, spi->modalias, sizeof(spi->modalias));

	settings = st_press_get_settings(spi->modalias);
	if (!settings) {
		dev_err(&spi->dev, "device name %s not recognized.\n",
			spi->modalias);
		return -ENODEV;
	}

	indio_dev = devm_iio_device_alloc(&spi->dev, sizeof(*press_data));
	if (!indio_dev)
		return -ENOMEM;

	press_data = iio_priv(indio_dev);
	press_data->sensor_settings = (struct st_sensor_settings *)settings;

	err = st_sensors_spi_configure(indio_dev, spi);
	if (err < 0)
		return err;

	err = st_press_common_probe(indio_dev);
	if (err < 0)
		return err;

	return 0;
}

static int st_press_spi_remove(struct spi_device *spi)
{
	st_press_common_remove(spi_get_drvdata(spi));

	return 0;
}

static const struct spi_device_id st_press_id_table[] = {
	{ LPS001WP_PRESS_DEV_NAME },
	{ LPS25H_PRESS_DEV_NAME },
	{ LPS331AP_PRESS_DEV_NAME },
	{ LPS22HB_PRESS_DEV_NAME },
	{ LPS33HW_PRESS_DEV_NAME },
	{ LPS35HW_PRESS_DEV_NAME },
	{ LPS22HH_PRESS_DEV_NAME },
	{},
};
MODULE_DEVICE_TABLE(spi, st_press_id_table);

static struct spi_driver st_press_driver = {
	.driver = {
		.name = "st-press-spi",
		.of_match_table = st_press_of_match,
	},
	.probe = st_press_spi_probe,
	.remove = st_press_spi_remove,
	.id_table = st_press_id_table,
};
module_spi_driver(st_press_driver);

MODULE_AUTHOR("Denis Ciocca <denis.ciocca@st.com>");
MODULE_DESCRIPTION("STMicroelectronics pressures spi driver");
MODULE_LICENSE("GPL v2");
