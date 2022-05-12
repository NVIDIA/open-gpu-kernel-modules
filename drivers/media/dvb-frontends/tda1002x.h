/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
    TDA10021/TDA10023  - Single Chip Cable Channel Receiver driver module
			 used on the the Siemens DVB-C cards

    Copyright (C) 1999 Convergence Integrated Media GmbH <ralph@convergence.de>
    Copyright (C) 2004 Markus Schulz <msc@antzsystem.de>
		   Support for TDA10021

*/

#ifndef TDA1002x_H
#define TDA1002x_H

#include <linux/dvb/frontend.h>

struct tda1002x_config {
	/* the demodulator's i2c address */
	u8 demod_address;
	u8 invert;
};

enum tda10023_output_mode {
	TDA10023_OUTPUT_MODE_PARALLEL_A = 0xe0,
	TDA10023_OUTPUT_MODE_PARALLEL_B = 0xa1,
	TDA10023_OUTPUT_MODE_PARALLEL_C = 0xa0,
	TDA10023_OUTPUT_MODE_SERIAL, /* TODO: not implemented */
};

struct tda10023_config {
	/* the demodulator's i2c address */
	u8 demod_address;
	u8 invert;

	/* clock settings */
	u32 xtal; /* defaults: 28920000 */
	u8 pll_m; /* defaults: 8 */
	u8 pll_p; /* defaults: 4 */
	u8 pll_n; /* defaults: 1 */

	/* MPEG2 TS output mode */
	u8 output_mode;

	/* input freq offset + baseband conversion type */
	u16 deltaf;
};

#if IS_REACHABLE(CONFIG_DVB_TDA10021)
extern struct dvb_frontend* tda10021_attach(const struct tda1002x_config* config,
					    struct i2c_adapter* i2c, u8 pwm);
#else
static inline struct dvb_frontend* tda10021_attach(const struct tda1002x_config* config,
					    struct i2c_adapter* i2c, u8 pwm)
{
	printk(KERN_WARNING "%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}
#endif // CONFIG_DVB_TDA10021

#if IS_REACHABLE(CONFIG_DVB_TDA10023)
extern struct dvb_frontend *tda10023_attach(
	const struct tda10023_config *config,
	struct i2c_adapter *i2c, u8 pwm);
#else
static inline struct dvb_frontend *tda10023_attach(
	const struct tda10023_config *config,
	struct i2c_adapter *i2c, u8 pwm)
{
	printk(KERN_WARNING "%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}
#endif // CONFIG_DVB_TDA10023

#endif // TDA1002x_H
