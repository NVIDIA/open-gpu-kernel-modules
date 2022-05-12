/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Conexant CX24120/CX24118 - DVB-S/S2 demod/tuner driver
 *
 * Copyright (C) 2008 Patrick Boettcher <pb@linuxtv.org>
 * Copyright (C) 2009 Sergey Tyurin <forum.free-x.de>
 * Updated 2012 by Jannis Achstetter <jannis_achstetter@web.de>
 * Copyright (C) 2015 Jemma Denson <jdenson@gmail.com>
 */

#ifndef CX24120_H
#define CX24120_H

#include <linux/dvb/frontend.h>
#include <linux/firmware.h>

struct cx24120_initial_mpeg_config {
	u8 x1;
	u8 x2;
	u8 x3;
};

struct cx24120_config {
	u8 i2c_addr;
	u32 xtal_khz;
	struct cx24120_initial_mpeg_config initial_mpeg_config;

	int (*request_firmware)(struct dvb_frontend *fe,
				const struct firmware **fw, char *name);

	/* max bytes I2C provider can write at once */
	u16 i2c_wr_max;
};

#if IS_REACHABLE(CONFIG_DVB_CX24120)
struct dvb_frontend *cx24120_attach(const struct cx24120_config *config,
				    struct i2c_adapter *i2c);
#else
static inline
struct dvb_frontend *cx24120_attach(const struct cx24120_config *config,
				    struct i2c_adapter *i2c)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}
#endif

#endif /* CX24120_H */
