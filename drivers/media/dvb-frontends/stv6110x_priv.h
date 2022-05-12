/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
	STV6110(A) Silicon tuner driver

	Copyright (C) Manu Abraham <abraham.manu@gmail.com>

	Copyright (C) ST Microelectronics

*/

#ifndef __STV6110x_PRIV_H
#define __STV6110x_PRIV_H

#define FE_ERROR				0
#define FE_NOTICE				1
#define FE_INFO					2
#define FE_DEBUG				3
#define FE_DEBUGREG				4

#define dprintk(__y, __z, format, arg...) do {						\
	if (__z) {									\
		if	((verbose > FE_ERROR) && (verbose > __y))			\
			printk(KERN_ERR "%s: " format "\n", __func__ , ##arg);		\
		else if	((verbose > FE_NOTICE) && (verbose > __y))			\
			printk(KERN_NOTICE "%s: " format "\n", __func__ , ##arg);	\
		else if ((verbose > FE_INFO) && (verbose > __y))			\
			printk(KERN_INFO "%s: " format "\n", __func__ , ##arg);		\
		else if ((verbose > FE_DEBUG) && (verbose > __y))			\
			printk(KERN_DEBUG "%s: " format "\n", __func__ , ##arg);	\
	} else {									\
		if (verbose > __y)							\
			printk(format, ##arg);						\
	}										\
} while (0)


#define STV6110x_SETFIELD(mask, bitf, val)				\
	(mask = (mask & (~(((1 << STV6110x_WIDTH_##bitf) - 1) <<	\
				  STV6110x_OFFST_##bitf))) |		\
			  (val << STV6110x_OFFST_##bitf))

#define STV6110x_GETFIELD(bitf, val)					\
	((val >> STV6110x_OFFST_##bitf) &				\
	((1 << STV6110x_WIDTH_##bitf) - 1))

#define MAKEWORD16(a, b)			(((a) << 8) | (b))

#define LSB(x)					((x & 0xff))
#define MSB(y)					((y >> 8) & 0xff)

#define TRIALS					10
#define R_DIV(__div)				(1 << (__div + 1))
#define REFCLOCK_kHz				(stv6110x->config->refclk /    1000)
#define REFCLOCK_MHz				(stv6110x->config->refclk / 1000000)

struct stv6110x_state {
	struct dvb_frontend		*frontend;
	struct i2c_adapter		*i2c;
	const struct stv6110x_config	*config;
	u8				regs[8];

	struct stv6110x_devctl	*devctl;
};

#endif /* __STV6110x_PRIV_H */
