/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *    Support for Legend Silicon GB20600 (a.k.a DMB-TH) demodulator
 *    LGS8913, LGS8GL5, LGS8G75
 *    experimental support LGS8G42, LGS8G52
 *
 *    Copyright (C) 2007-2009 David T.L. Wong <davidtlwong@gmail.com>
 *    Copyright (C) 2008 Sirius International (Hong Kong) Limited
 *    Timothy Lee <timothy.lee@siriushk.com> (for initial work on LGS8GL5)
 */

#ifndef LGS8913_PRIV_H
#define LGS8913_PRIV_H

struct lgs8gxx_state {
	struct i2c_adapter *i2c;
	/* configuration settings */
	const struct lgs8gxx_config *config;
	struct dvb_frontend frontend;
	u16 curr_gi; /* current guard interval */
};

#define SC_MASK		0x1C	/* Sub-Carrier Modulation Mask */
#define SC_QAM64	0x10	/* 64QAM modulation */
#define SC_QAM32	0x0C	/* 32QAM modulation */
#define SC_QAM16	0x08	/* 16QAM modulation */
#define SC_QAM4NR	0x04	/* 4QAM-NR modulation */
#define SC_QAM4		0x00	/* 4QAM modulation */

#define LGS_FEC_MASK	0x03	/* FEC Rate Mask */
#define LGS_FEC_0_4	0x00	/* FEC Rate 0.4 */
#define LGS_FEC_0_6	0x01	/* FEC Rate 0.6 */
#define LGS_FEC_0_8	0x02	/* FEC Rate 0.8 */

#define TIM_MASK	  0x20	/* Time Interleave Length Mask */
#define TIM_LONG	  0x20	/* Time Interleave Length = 720 */
#define TIM_MIDDLE     0x00   /* Time Interleave Length = 240 */

#define CF_MASK	0x80	/* Control Frame Mask */
#define CF_EN	0x80	/* Control Frame On */

#define GI_MASK	0x03	/* Guard Interval Mask */
#define GI_420	0x00	/* 1/9 Guard Interval */
#define GI_595	0x01	/* */
#define GI_945	0x02	/* 1/4 Guard Interval */


#define TS_PARALLEL	0x00	/* Parallel TS Output a.k.a. SPI */
#define TS_SERIAL	0x01	/* Serial TS Output a.k.a. SSI */
#define TS_CLK_NORMAL		0x00	/* MPEG Clock Normal */
#define TS_CLK_INVERTED		0x02	/* MPEG Clock Inverted */
#define TS_CLK_GATED		0x00	/* MPEG clock gated */
#define TS_CLK_FREERUN		0x04	/* MPEG clock free running*/


#endif
