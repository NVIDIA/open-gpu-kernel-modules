/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
	Driver for ST STV0288 demodulator

	Copyright (C) 2006 Georg Acher, BayCom GmbH, acher (at) baycom (dot) de
		for Reel Multimedia
	Copyright (C) 2008 TurboSight.com, <bob@turbosight.com>
	Copyright (C) 2008 Igor M. Liplianin <liplianin@me.by>
		Removed stb6000 specific tuner code and revised some
		procedures.


*/

#ifndef STV0288_H
#define STV0288_H

#include <linux/dvb/frontend.h>
#include <media/dvb_frontend.h>

struct stv0288_config {
	/* the demodulator's i2c address */
	u8 demod_address;

	u8* inittab;

	/* minimum delay before retuning */
	int min_delay_ms;

	int (*set_ts_params)(struct dvb_frontend *fe, int is_punctured);
};

#if IS_REACHABLE(CONFIG_DVB_STV0288)
extern struct dvb_frontend *stv0288_attach(const struct stv0288_config *config,
					   struct i2c_adapter *i2c);
#else
static inline struct dvb_frontend *stv0288_attach(const struct stv0288_config *config,
					   struct i2c_adapter *i2c)
{
	printk(KERN_WARNING "%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}
#endif /* CONFIG_DVB_STV0288 */

static inline int stv0288_writereg(struct dvb_frontend *fe, u8 reg, u8 val)
{
	int r = 0;
	u8 buf[] = { reg, val };
	if (fe->ops.write)
		r = fe->ops.write(fe, buf, 2);
	return r;
}

#endif /* STV0288_H */
