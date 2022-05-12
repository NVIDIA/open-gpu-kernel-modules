/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * stv6110.h
 *
 * Driver for ST STV6110 satellite tuner IC.
 *
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 */

#ifndef __DVB_STV6110_H__
#define __DVB_STV6110_H__

#include <linux/i2c.h>
#include <media/dvb_frontend.h>

/* registers */
#define RSTV6110_CTRL1		0
#define RSTV6110_CTRL2		1
#define RSTV6110_TUNING1	2
#define RSTV6110_TUNING2	3
#define RSTV6110_CTRL3		4
#define RSTV6110_STAT1		5
#define RSTV6110_STAT2		6
#define RSTV6110_STAT3		7

struct stv6110_config {
	u8 i2c_address;
	u32 mclk;
	u8 gain;
	u8 clk_div;	/* divisor value for the output clock */
};

#if IS_REACHABLE(CONFIG_DVB_STV6110)
extern struct dvb_frontend *stv6110_attach(struct dvb_frontend *fe,
					const struct stv6110_config *config,
					struct i2c_adapter *i2c);
#else
static inline struct dvb_frontend *stv6110_attach(struct dvb_frontend *fe,
					const struct stv6110_config *config,
					struct i2c_adapter *i2c)
{
	printk(KERN_WARNING "%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}
#endif

#endif
