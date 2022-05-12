/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Realtek RTL2832U SDR driver
 *
 * Copyright (C) 2013 Antti Palosaari <crope@iki.fi>
 *
 * GNU Radio plugin "gr-kernel" for device usage will be on:
 * http://git.linuxtv.org/anttip/gr-kernel.git
 */

#ifndef RTL2832_SDR_H
#define RTL2832_SDR_H

#include <linux/i2c.h>
#include <media/v4l2-subdev.h>
#include <media/dvb_frontend.h>

/**
 * struct rtl2832_sdr_platform_data - Platform data for the rtl2832_sdr driver
 * @clk: Clock frequency (4000000, 16000000, 25000000, 28800000).
 * @tuner: Used tuner model.
 * @regmap: pointer to &struct regmap.
 * @dvb_frontend: rtl2832 DVB frontend.
 * @v4l2_subdev: Tuner v4l2 controls.
 * @dvb_usb_device: DVB USB interface for USB streaming.
 */
struct rtl2832_sdr_platform_data {
	u32 clk;
	/*
	 * XXX: This list must be kept sync with dvb_usb_rtl28xxu USB IF driver.
	 */
#define RTL2832_SDR_TUNER_FC2580    0x21
#define RTL2832_SDR_TUNER_TUA9001   0x24
#define RTL2832_SDR_TUNER_FC0012    0x26
#define RTL2832_SDR_TUNER_E4000     0x27
#define RTL2832_SDR_TUNER_FC0013    0x29
#define RTL2832_SDR_TUNER_R820T     0x2a
#define RTL2832_SDR_TUNER_R828D     0x2b
	u8 tuner;

	struct regmap *regmap;
	struct dvb_frontend *dvb_frontend;
	struct v4l2_subdev *v4l2_subdev;
	struct dvb_usb_device *dvb_usb_device;
};

#endif /* RTL2832_SDR_H */
