/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * stv0900.h
 *
 * Driver for ST STV0900 satellite demodulator IC.
 *
 * Copyright (C) ST Microelectronics.
 * Copyright (C) 2009 NetUP Inc.
 * Copyright (C) 2009 Igor M. Liplianin <liplianin@netup.ru>
 */

#ifndef STV0900_H
#define STV0900_H

#include <linux/dvb/frontend.h>
#include <media/dvb_frontend.h>

struct stv0900_reg {
	u16 addr;
	u8  val;
};

struct stv0900_config {
	u8 demod_address;
	u8 demod_mode;
	u32 xtal;
	u8 clkmode;/* 0 for CLKI,  2 for XTALI */

	u8 diseqc_mode;

	u8 path1_mode;
	u8 path2_mode;
	struct stv0900_reg *ts_config_regs;
	u8 tun1_maddress;/* 0, 1, 2, 3 for 0xc0, 0xc2, 0xc4, 0xc6 */
	u8 tun2_maddress;
	u8 tun1_adc;/* 1 for stv6110, 2 for stb6100 */
	u8 tun2_adc;
	u8 tun1_type;/* for now 3 for stb6100 auto, else - software */
	u8 tun2_type;
	/* Set device param to start dma */
	int (*set_ts_params)(struct dvb_frontend *fe, int is_punctured);
	/* Hook for Lock LED */
	void (*set_lock_led)(struct dvb_frontend *fe, int offon);
};

#if IS_REACHABLE(CONFIG_DVB_STV0900)
extern struct dvb_frontend *stv0900_attach(const struct stv0900_config *config,
					struct i2c_adapter *i2c, int demod);
#else
static inline struct dvb_frontend *stv0900_attach(const struct stv0900_config *config,
					struct i2c_adapter *i2c, int demod)
{
	printk(KERN_WARNING "%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}
#endif

#endif

