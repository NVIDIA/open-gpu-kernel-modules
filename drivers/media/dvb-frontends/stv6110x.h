/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
	STV6110(A) Silicon tuner driver

	Copyright (C) Manu Abraham <abraham.manu@gmail.com>

	Copyright (C) ST Microelectronics

*/

#ifndef __STV6110x_H
#define __STV6110x_H

struct stv6110x_config {
	u8	addr;
	u32	refclk;
	u8	clk_div; /* divisor value for the output clock */
	struct dvb_frontend		*frontend;

	struct stv6110x_devctl* (*get_devctl)(struct i2c_client *i2c);
};

enum tuner_mode {
	TUNER_SLEEP = 1,
	TUNER_WAKE,
};

enum tuner_status {
	TUNER_PHASELOCKED = 1,
};

struct stv6110x_devctl {
	int (*tuner_init) (struct dvb_frontend *fe);
	int (*tuner_sleep) (struct dvb_frontend *fe);
	int (*tuner_set_mode) (struct dvb_frontend *fe, enum tuner_mode mode);
	int (*tuner_set_frequency) (struct dvb_frontend *fe, u32 frequency);
	int (*tuner_get_frequency) (struct dvb_frontend *fe, u32 *frequency);
	int (*tuner_set_bandwidth) (struct dvb_frontend *fe, u32 bandwidth);
	int (*tuner_get_bandwidth) (struct dvb_frontend *fe, u32 *bandwidth);
	int (*tuner_set_bbgain) (struct dvb_frontend *fe, u32 gain);
	int (*tuner_get_bbgain) (struct dvb_frontend *fe, u32 *gain);
	int (*tuner_set_refclk)  (struct dvb_frontend *fe, u32 refclk);
	int (*tuner_get_status) (struct dvb_frontend *fe, u32 *status);
};


#if IS_REACHABLE(CONFIG_DVB_STV6110x)

extern const struct stv6110x_devctl *stv6110x_attach(struct dvb_frontend *fe,
					       const struct stv6110x_config *config,
					       struct i2c_adapter *i2c);

#else
static inline const struct stv6110x_devctl *stv6110x_attach(struct dvb_frontend *fe,
						      const struct stv6110x_config *config,
						      struct i2c_adapter *i2c)
{
	printk(KERN_WARNING "%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}

#endif /* CONFIG_DVB_STV6110x */

#endif /* __STV6110x_H */
