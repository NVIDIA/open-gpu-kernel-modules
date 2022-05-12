/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Sony CXD2820R demodulator driver
 *
 * Copyright (C) 2010 Antti Palosaari <crope@iki.fi>
 */


#ifndef CXD2820R_PRIV_H
#define CXD2820R_PRIV_H

#include <linux/dvb/version.h>
#include <media/dvb_frontend.h>
#include <media/dvb_math.h>
#include "cxd2820r.h"
#include <linux/gpio.h>
#include <linux/math64.h>
#include <linux/regmap.h>

struct reg_val_mask {
	u32 reg;
	u8  val;
	u8  mask;
};

#define CXD2820R_CLK 41000000

struct cxd2820r_priv {
	struct i2c_client *client[2];
	struct regmap *regmap[2];
	struct i2c_adapter *i2c;
	struct dvb_frontend fe;
	u8 ts_mode;
	bool ts_clk_inv;
	bool if_agc_polarity;
	bool spec_inv;

	u64 post_bit_error_prev_dvbv3;
	u64 post_bit_error;

	bool ber_running;

#define GPIO_COUNT 3
	u8 gpio[GPIO_COUNT];
#ifdef CONFIG_GPIOLIB
	struct gpio_chip gpio_chip;
#endif

	enum fe_delivery_system delivery_system;
	bool last_tune_failed; /* for switch between T and T2 tune */
};

/* cxd2820r_core.c */

extern int cxd2820r_debug;

int cxd2820r_gpio(struct dvb_frontend *fe, u8 *gpio);

int cxd2820r_wr_reg_val_mask_tab(struct cxd2820r_priv *priv,
				 const struct reg_val_mask *tab, int tab_len);

int cxd2820r_wr_reg_mask(struct cxd2820r_priv *priv, u32 reg, u8 val,
	u8 mask);

int cxd2820r_wr_regs(struct cxd2820r_priv *priv, u32 reginfo, u8 *val,
	int len);

int cxd2820r_wr_regs(struct cxd2820r_priv *priv, u32 reginfo, u8 *val,
	int len);

int cxd2820r_rd_regs(struct cxd2820r_priv *priv, u32 reginfo, u8 *val,
	int len);

int cxd2820r_wr_reg(struct cxd2820r_priv *priv, u32 reg, u8 val);

int cxd2820r_rd_reg(struct cxd2820r_priv *priv, u32 reg, u8 *val);

/* cxd2820r_c.c */

int cxd2820r_get_frontend_c(struct dvb_frontend *fe,
			    struct dtv_frontend_properties *p);

int cxd2820r_set_frontend_c(struct dvb_frontend *fe);

int cxd2820r_read_status_c(struct dvb_frontend *fe, enum fe_status *status);

int cxd2820r_init_c(struct dvb_frontend *fe);

int cxd2820r_sleep_c(struct dvb_frontend *fe);

int cxd2820r_get_tune_settings_c(struct dvb_frontend *fe,
	struct dvb_frontend_tune_settings *s);

/* cxd2820r_t.c */

int cxd2820r_get_frontend_t(struct dvb_frontend *fe,
			    struct dtv_frontend_properties *p);

int cxd2820r_set_frontend_t(struct dvb_frontend *fe);

int cxd2820r_read_status_t(struct dvb_frontend *fe, enum fe_status *status);

int cxd2820r_init_t(struct dvb_frontend *fe);

int cxd2820r_sleep_t(struct dvb_frontend *fe);

int cxd2820r_get_tune_settings_t(struct dvb_frontend *fe,
	struct dvb_frontend_tune_settings *s);

/* cxd2820r_t2.c */

int cxd2820r_get_frontend_t2(struct dvb_frontend *fe,
			     struct dtv_frontend_properties *p);

int cxd2820r_set_frontend_t2(struct dvb_frontend *fe);

int cxd2820r_read_status_t2(struct dvb_frontend *fe, enum fe_status *status);

int cxd2820r_init_t2(struct dvb_frontend *fe);

int cxd2820r_sleep_t2(struct dvb_frontend *fe);

int cxd2820r_get_tune_settings_t2(struct dvb_frontend *fe,
	struct dvb_frontend_tune_settings *s);

#endif /* CXD2820R_PRIV_H */
