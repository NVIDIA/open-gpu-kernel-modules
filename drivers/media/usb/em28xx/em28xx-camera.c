// SPDX-License-Identifier: GPL-2.0+
//
// em28xx-camera.c - driver for Empia EM25xx/27xx/28xx USB video capture devices
//
// Copyright (C) 2009 Mauro Carvalho Chehab <mchehab@kernel.org>
// Copyright (C) 2013 Frank Schäfer <fschaefer.oss@googlemail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include "em28xx.h"

#include <linux/i2c.h>
#include <linux/usb.h>
#include <media/i2c/mt9v011.h>
#include <media/v4l2-common.h>

/* Possible i2c addresses of Micron sensors */
static unsigned short micron_sensor_addrs[] = {
	0xb8 >> 1,   /* MT9V111, MT9V403 */
	0xba >> 1,   /* MT9M001/011/111/112, MT9V011/012/112, MT9D011 */
	0x90 >> 1,   /* MT9V012/112, MT9D011 (alternative address) */
	I2C_CLIENT_END
};

/* Possible i2c addresses of Omnivision sensors */
static unsigned short omnivision_sensor_addrs[] = {
	0x42 >> 1,   /* OV7725, OV7670/60/48 */
	0x60 >> 1,   /* OV2640, OV9650/53/55 */
	I2C_CLIENT_END
};

/* FIXME: Should be replaced by a proper mt9m111 driver */
static int em28xx_initialize_mt9m111(struct em28xx *dev)
{
	int i;
	unsigned char regs[][3] = {
		{ 0x0d, 0x00, 0x01, },  /* reset and use defaults */
		{ 0x0d, 0x00, 0x00, },
		{ 0x0a, 0x00, 0x21, },
		{ 0x21, 0x04, 0x00, },  /* full readout spd, no row/col skip */
	};

	for (i = 0; i < ARRAY_SIZE(regs); i++)
		i2c_master_send(&dev->i2c_client[dev->def_i2c_bus],
				&regs[i][0], 3);

	/* FIXME: This won't be creating a sensor at the media graph */

	return 0;
}

/* FIXME: Should be replaced by a proper mt9m001 driver */
static int em28xx_initialize_mt9m001(struct em28xx *dev)
{
	int i;
	unsigned char regs[][3] = {
		{ 0x0d, 0x00, 0x01, },
		{ 0x0d, 0x00, 0x00, },
		{ 0x04, 0x05, 0x00, },	/* hres = 1280 */
		{ 0x03, 0x04, 0x00, },  /* vres = 1024 */
		{ 0x20, 0x11, 0x00, },
		{ 0x06, 0x00, 0x10, },
		{ 0x2b, 0x00, 0x24, },
		{ 0x2e, 0x00, 0x24, },
		{ 0x35, 0x00, 0x24, },
		{ 0x2d, 0x00, 0x20, },
		{ 0x2c, 0x00, 0x20, },
		{ 0x09, 0x0a, 0xd4, },
		{ 0x35, 0x00, 0x57, },
	};

	for (i = 0; i < ARRAY_SIZE(regs); i++)
		i2c_master_send(&dev->i2c_client[dev->def_i2c_bus],
				&regs[i][0], 3);

	/* FIXME: This won't be creating a sensor at the media graph */

	return 0;
}

/*
 * Probes Micron sensors with 8 bit address and 16 bit register width
 */
static int em28xx_probe_sensor_micron(struct em28xx *dev)
{
	int ret, i;
	char *name;
	u16 id;

	struct i2c_client *client = &dev->i2c_client[dev->def_i2c_bus];

	dev->em28xx_sensor = EM28XX_NOSENSOR;
	for (i = 0; micron_sensor_addrs[i] != I2C_CLIENT_END; i++) {
		client->addr = micron_sensor_addrs[i];
		/* Read chip ID from register 0x00 */
		ret = i2c_smbus_read_word_data(client, 0x00); /* assumes LE */
		if (ret < 0) {
			if (ret != -ENXIO)
				dev_err(&dev->intf->dev,
					"couldn't read from i2c device 0x%02x: error %i\n",
				       client->addr << 1, ret);
			continue;
		}
		id = swab16(ret); /* LE -> BE */
		/* Read chip ID from register 0xff */
		ret = i2c_smbus_read_word_data(client, 0xff);
		if (ret < 0) {
			dev_err(&dev->intf->dev,
				"couldn't read from i2c device 0x%02x: error %i\n",
				client->addr << 1, ret);
			continue;
		}
		/* Validate chip ID to be sure we have a Micron device */
		if (id != swab16(ret))
			continue;
		/* Check chip ID */
		switch (id) {
		case 0x1222:
			name = "MT9V012"; /* MI370 */ /* 640x480 */
			break;
		case 0x1229:
			name = "MT9V112"; /* 640x480 */
			break;
		case 0x1433:
			name = "MT9M011"; /* 1280x1024 */
			break;
		case 0x143a:    /* found in the ECS G200 */
			name = "MT9M111"; /* MI1310 */ /* 1280x1024 */
			dev->em28xx_sensor = EM28XX_MT9M111;
			break;
		case 0x148c:
			name = "MT9M112"; /* MI1320 */ /* 1280x1024 */
			break;
		case 0x1511:
			name = "MT9D011"; /* MI2010 */ /* 1600x1200 */
			break;
		case 0x8232:
		case 0x8243:	/* rev B */
			name = "MT9V011"; /* MI360 */ /* 640x480 */
			dev->em28xx_sensor = EM28XX_MT9V011;
			break;
		case 0x8431:
			name = "MT9M001"; /* 1280x1024 */
			dev->em28xx_sensor = EM28XX_MT9M001;
			break;
		default:
			dev_info(&dev->intf->dev,
				 "unknown Micron sensor detected: 0x%04x\n",
				 id);
			return 0;
		}

		if (dev->em28xx_sensor == EM28XX_NOSENSOR)
			dev_info(&dev->intf->dev,
				 "unsupported sensor detected: %s\n", name);
		else
			dev_info(&dev->intf->dev,
				 "sensor %s detected\n", name);

		return 0;
	}

	return -ENODEV;
}

/*
 * Probes Omnivision sensors with 8 bit address and register width
 */
static int em28xx_probe_sensor_omnivision(struct em28xx *dev)
{
	int ret, i;
	char *name;
	u8 reg;
	u16 id;
	struct i2c_client *client = &dev->i2c_client[dev->def_i2c_bus];

	dev->em28xx_sensor = EM28XX_NOSENSOR;
	/*
	 * NOTE: these devices have the register auto incrementation disabled
	 * by default, so we have to use single byte reads !
	 */
	for (i = 0; omnivision_sensor_addrs[i] != I2C_CLIENT_END; i++) {
		client->addr = omnivision_sensor_addrs[i];
		/* Read manufacturer ID from registers 0x1c-0x1d (BE) */
		reg = 0x1c;
		ret = i2c_smbus_read_byte_data(client, reg);
		if (ret < 0) {
			if (ret != -ENXIO)
				dev_err(&dev->intf->dev,
					"couldn't read from i2c device 0x%02x: error %i\n",
					client->addr << 1, ret);
			continue;
		}
		id = ret << 8;
		reg = 0x1d;
		ret = i2c_smbus_read_byte_data(client, reg);
		if (ret < 0) {
			dev_err(&dev->intf->dev,
				"couldn't read from i2c device 0x%02x: error %i\n",
				client->addr << 1, ret);
			continue;
		}
		id += ret;
		/* Check manufacturer ID */
		if (id != 0x7fa2)
			continue;
		/* Read product ID from registers 0x0a-0x0b (BE) */
		reg = 0x0a;
		ret = i2c_smbus_read_byte_data(client, reg);
		if (ret < 0) {
			dev_err(&dev->intf->dev,
				"couldn't read from i2c device 0x%02x: error %i\n",
				client->addr << 1, ret);
			continue;
		}
		id = ret << 8;
		reg = 0x0b;
		ret = i2c_smbus_read_byte_data(client, reg);
		if (ret < 0) {
			dev_err(&dev->intf->dev,
				"couldn't read from i2c device 0x%02x: error %i\n",
				client->addr << 1, ret);
			continue;
		}
		id += ret;
		/* Check product ID */
		switch (id) {
		case 0x2642:
			name = "OV2640";
			dev->em28xx_sensor = EM28XX_OV2640;
			break;
		case 0x7648:
			name = "OV7648";
			break;
		case 0x7660:
			name = "OV7660";
			break;
		case 0x7673:
			name = "OV7670";
			break;
		case 0x7720:
			name = "OV7720";
			break;
		case 0x7721:
			name = "OV7725";
			break;
		case 0x9648: /* Rev 2 */
		case 0x9649: /* Rev 3 */
			name = "OV9640";
			break;
		case 0x9650:
		case 0x9652: /* OV9653 */
			name = "OV9650";
			break;
		case 0x9656: /* Rev 4 */
		case 0x9657: /* Rev 5 */
			name = "OV9655";
			break;
		default:
			dev_info(&dev->intf->dev,
				 "unknown OmniVision sensor detected: 0x%04x\n",
				id);
			return 0;
		}

		if (dev->em28xx_sensor == EM28XX_NOSENSOR)
			dev_info(&dev->intf->dev,
				 "unsupported sensor detected: %s\n", name);
		else
			dev_info(&dev->intf->dev,
				 "sensor %s detected\n", name);

		return 0;
	}

	return -ENODEV;
}

int em28xx_detect_sensor(struct em28xx *dev)
{
	int ret;

	ret = em28xx_probe_sensor_micron(dev);

	if (dev->em28xx_sensor == EM28XX_NOSENSOR && ret < 0)
		ret = em28xx_probe_sensor_omnivision(dev);

	/*
	 * NOTE: the Windows driver also probes i2c addresses
	 *       0x22 (Samsung ?) and 0x66 (Kodak ?)
	 */

	if (dev->em28xx_sensor == EM28XX_NOSENSOR && ret < 0) {
		dev_info(&dev->intf->dev,
			 "No sensor detected\n");
		return -ENODEV;
	}

	return 0;
}

int em28xx_init_camera(struct em28xx *dev)
{
	struct i2c_client *client = &dev->i2c_client[dev->def_i2c_bus];
	struct i2c_adapter *adap = &dev->i2c_adap[dev->def_i2c_bus];
	struct em28xx_v4l2 *v4l2 = dev->v4l2;

	switch (dev->em28xx_sensor) {
	case EM28XX_MT9V011:
	{
		struct mt9v011_platform_data pdata;
		struct i2c_board_info mt9v011_info = {
			.type = "mt9v011",
			.addr = client->addr,
			.platform_data = &pdata,
		};

		v4l2->sensor_xres = 640;
		v4l2->sensor_yres = 480;

		/*
		 * FIXME: mt9v011 uses I2S speed as xtal clk - at least with
		 * the Silvercrest cam I have here for testing - for higher
		 * resolutions, a high clock cause horizontal artifacts, so we
		 * need to use a lower xclk frequency.
		 * Yet, it would be possible to adjust xclk depending on the
		 * desired resolution, since this affects directly the
		 * frame rate.
		 */
		dev->board.xclk = EM28XX_XCLK_FREQUENCY_4_3MHZ;
		em28xx_write_reg(dev, EM28XX_R0F_XCLK, dev->board.xclk);
		v4l2->sensor_xtal = 4300000;
		pdata.xtal = v4l2->sensor_xtal;
		if (NULL ==
		    v4l2_i2c_new_subdev_board(&v4l2->v4l2_dev, adap,
					      &mt9v011_info, NULL))
			return -ENODEV;
		v4l2->vinmode = EM28XX_VINMODE_RGB8_GRBG;
		v4l2->vinctl = 0x00;

		break;
	}
	case EM28XX_MT9M001:
		v4l2->sensor_xres = 1280;
		v4l2->sensor_yres = 1024;

		em28xx_initialize_mt9m001(dev);

		v4l2->vinmode = EM28XX_VINMODE_RGB8_BGGR;
		v4l2->vinctl = 0x00;

		break;
	case EM28XX_MT9M111:
		v4l2->sensor_xres = 640;
		v4l2->sensor_yres = 512;

		dev->board.xclk = EM28XX_XCLK_FREQUENCY_48MHZ;
		em28xx_write_reg(dev, EM28XX_R0F_XCLK, dev->board.xclk);
		em28xx_initialize_mt9m111(dev);

		v4l2->vinmode = EM28XX_VINMODE_YUV422_UYVY;
		v4l2->vinctl = 0x00;

		break;
	case EM28XX_OV2640:
	{
		struct v4l2_subdev *subdev;
		struct i2c_board_info ov2640_info = {
			.type = "ov2640",
			.flags = I2C_CLIENT_SCCB,
			.addr = client->addr,
		};
		struct v4l2_subdev_format format = {
			.which = V4L2_SUBDEV_FORMAT_ACTIVE,
		};

		/*
		 * FIXME: sensor supports resolutions up to 1600x1200, but
		 * resolution setting/switching needs to be modified to
		 * - switch sensor output resolution (including further
		 *   configuration changes)
		 * - adjust bridge xclk
		 * - disable 16 bit (12 bit) output formats on high resolutions
		 */
		v4l2->sensor_xres = 640;
		v4l2->sensor_yres = 480;

		subdev =
		     v4l2_i2c_new_subdev_board(&v4l2->v4l2_dev, adap,
					       &ov2640_info, NULL);
		if (!subdev)
			return -ENODEV;

		format.format.code = MEDIA_BUS_FMT_YUYV8_2X8;
		format.format.width = 640;
		format.format.height = 480;
		v4l2_subdev_call(subdev, pad, set_fmt, NULL, &format);

		/* NOTE: for UXGA=1600x1200 switch to 12MHz */
		dev->board.xclk = EM28XX_XCLK_FREQUENCY_24MHZ;
		em28xx_write_reg(dev, EM28XX_R0F_XCLK, dev->board.xclk);
		v4l2->vinmode = EM28XX_VINMODE_YUV422_YUYV;
		v4l2->vinctl = 0x00;

		break;
	}
	case EM28XX_NOSENSOR:
	default:
		return -EINVAL;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(em28xx_init_camera);
