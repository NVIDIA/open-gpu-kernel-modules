// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2008
 * Guennadi Liakhovetski, DENX Software Engineering, <lg@denx.de>
 *
 * LED driver for the DAC124S085 SPI DAC
 */

#include <linux/leds.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/spi/spi.h>

struct dac124s085_led {
	struct led_classdev	ldev;
	struct spi_device	*spi;
	int			id;
	char			name[sizeof("dac124s085-3")];

	struct mutex		mutex;
};

struct dac124s085 {
	struct dac124s085_led leds[4];
};

#define REG_WRITE		(0 << 12)
#define REG_WRITE_UPDATE	(1 << 12)
#define ALL_WRITE_UPDATE	(2 << 12)
#define POWER_DOWN_OUTPUT	(3 << 12)

static int dac124s085_set_brightness(struct led_classdev *ldev,
				      enum led_brightness brightness)
{
	struct dac124s085_led *led = container_of(ldev, struct dac124s085_led,
						  ldev);
	u16 word;
	int ret;

	mutex_lock(&led->mutex);
	word = cpu_to_le16(((led->id) << 14) | REG_WRITE_UPDATE |
			   (brightness & 0xfff));
	ret = spi_write(led->spi, (const u8 *)&word, sizeof(word));
	mutex_unlock(&led->mutex);

	return ret;
}

static int dac124s085_probe(struct spi_device *spi)
{
	struct dac124s085	*dac;
	struct dac124s085_led	*led;
	int i, ret;

	dac = devm_kzalloc(&spi->dev, sizeof(*dac), GFP_KERNEL);
	if (!dac)
		return -ENOMEM;

	spi->bits_per_word = 16;

	for (i = 0; i < ARRAY_SIZE(dac->leds); i++) {
		led		= dac->leds + i;
		led->id		= i;
		led->spi	= spi;
		snprintf(led->name, sizeof(led->name), "dac124s085-%d", i);
		mutex_init(&led->mutex);
		led->ldev.name = led->name;
		led->ldev.brightness = LED_OFF;
		led->ldev.max_brightness = 0xfff;
		led->ldev.brightness_set_blocking = dac124s085_set_brightness;
		ret = led_classdev_register(&spi->dev, &led->ldev);
		if (ret < 0)
			goto eledcr;
	}

	spi_set_drvdata(spi, dac);

	return 0;

eledcr:
	while (i--)
		led_classdev_unregister(&dac->leds[i].ldev);

	return ret;
}

static int dac124s085_remove(struct spi_device *spi)
{
	struct dac124s085	*dac = spi_get_drvdata(spi);
	int i;

	for (i = 0; i < ARRAY_SIZE(dac->leds); i++)
		led_classdev_unregister(&dac->leds[i].ldev);

	return 0;
}

static struct spi_driver dac124s085_driver = {
	.probe		= dac124s085_probe,
	.remove		= dac124s085_remove,
	.driver = {
		.name	= "dac124s085",
	},
};

module_spi_driver(dac124s085_driver);

MODULE_AUTHOR("Guennadi Liakhovetski <lg@denx.de>");
MODULE_DESCRIPTION("DAC124S085 LED driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("spi:dac124s085");
