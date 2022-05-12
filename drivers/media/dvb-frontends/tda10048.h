/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
    NXP TDA10048HN DVB OFDM demodulator driver

    Copyright (C) 2009 Steven Toth <stoth@kernellabs.com>


*/

#ifndef TDA10048_H
#define TDA10048_H

#include <linux/dvb/frontend.h>
#include <linux/firmware.h>

struct tda10048_config {

	/* the demodulator's i2c address */
	u8 demod_address;

	/* serial/parallel output */
#define TDA10048_PARALLEL_OUTPUT 0
#define TDA10048_SERIAL_OUTPUT   1
	u8 output_mode;

#define TDA10048_BULKWRITE_200	200
#define TDA10048_BULKWRITE_50	50
	u8 fwbulkwritelen;

	/* Spectral Inversion */
#define TDA10048_INVERSION_OFF 0
#define TDA10048_INVERSION_ON  1
	u8 inversion;

#define TDA10048_IF_3300  3300
#define TDA10048_IF_3500  3500
#define TDA10048_IF_3800  3800
#define TDA10048_IF_4000  4000
#define TDA10048_IF_4300  4300
#define TDA10048_IF_4500  4500
#define TDA10048_IF_4750  4750
#define TDA10048_IF_5000  5000
#define TDA10048_IF_36130 36130
	u16 dtv6_if_freq_khz;
	u16 dtv7_if_freq_khz;
	u16 dtv8_if_freq_khz;

#define TDA10048_CLK_4000  4000
#define TDA10048_CLK_16000 16000
	u16 clk_freq_khz;

	/* Disable I2C gate access */
	u8 disable_gate_access;

	bool no_firmware;

	bool set_pll;
	u8 pll_m;
	u8 pll_p;
	u8 pll_n;
};

#if IS_REACHABLE(CONFIG_DVB_TDA10048)
extern struct dvb_frontend *tda10048_attach(
	const struct tda10048_config *config,
	struct i2c_adapter *i2c);
#else
static inline struct dvb_frontend *tda10048_attach(
	const struct tda10048_config *config,
	struct i2c_adapter *i2c)
{
	printk(KERN_WARNING "%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}
#endif /* CONFIG_DVB_TDA10048 */

#endif /* TDA10048_H */
