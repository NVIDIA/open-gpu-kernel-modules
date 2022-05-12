/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 1999 - 2018 Intel Corporation. */

#ifndef _IXGBEVF_REGS_H_
#define _IXGBEVF_REGS_H_

#define IXGBE_VFCTRL		0x00000
#define IXGBE_VFSTATUS		0x00008
#define IXGBE_VFLINKS		0x00010
#define IXGBE_VFFRTIMER		0x00048
#define IXGBE_VFRXMEMWRAP	0x03190
#define IXGBE_VTEICR		0x00100
#define IXGBE_VTEICS		0x00104
#define IXGBE_VTEIMS		0x00108
#define IXGBE_VTEIMC		0x0010C
#define IXGBE_VTEIAC		0x00110
#define IXGBE_VTEIAM		0x00114
#define IXGBE_VTEITR(x)		(0x00820 + (4 * (x)))
#define IXGBE_VTIVAR(x)		(0x00120 + (4 * (x)))
#define IXGBE_VTIVAR_MISC	0x00140
#define IXGBE_VTRSCINT(x)	(0x00180 + (4 * (x)))
#define IXGBE_VFRDBAL(x)	(0x01000 + (0x40 * (x)))
#define IXGBE_VFRDBAH(x)	(0x01004 + (0x40 * (x)))
#define IXGBE_VFRDLEN(x)	(0x01008 + (0x40 * (x)))
#define IXGBE_VFRDH(x)		(0x01010 + (0x40 * (x)))
#define IXGBE_VFRDT(x)		(0x01018 + (0x40 * (x)))
#define IXGBE_VFRXDCTL(x)	(0x01028 + (0x40 * (x)))
#define IXGBE_VFSRRCTL(x)	(0x01014 + (0x40 * (x)))
#define IXGBE_VFRSCCTL(x)	(0x0102C + (0x40 * (x)))
#define IXGBE_VFPSRTYPE		0x00300
#define IXGBE_VFTDBAL(x)	(0x02000 + (0x40 * (x)))
#define IXGBE_VFTDBAH(x)	(0x02004 + (0x40 * (x)))
#define IXGBE_VFTDLEN(x)	(0x02008 + (0x40 * (x)))
#define IXGBE_VFTDH(x)		(0x02010 + (0x40 * (x)))
#define IXGBE_VFTDT(x)		(0x02018 + (0x40 * (x)))
#define IXGBE_VFTXDCTL(x)	(0x02028 + (0x40 * (x)))
#define IXGBE_VFTDWBAL(x)	(0x02038 + (0x40 * (x)))
#define IXGBE_VFTDWBAH(x)	(0x0203C + (0x40 * (x)))
#define IXGBE_VFDCA_RXCTRL(x)	(0x0100C + (0x40 * (x)))
#define IXGBE_VFDCA_TXCTRL(x)	(0x0200c + (0x40 * (x)))
#define IXGBE_VFGPRC		0x0101C
#define IXGBE_VFGPTC		0x0201C
#define IXGBE_VFGORC_LSB	0x01020
#define IXGBE_VFGORC_MSB	0x01024
#define IXGBE_VFGOTC_LSB	0x02020
#define IXGBE_VFGOTC_MSB	0x02024
#define IXGBE_VFMPRC		0x01034
#define IXGBE_VFMRQC		0x3000
#define IXGBE_VFRSSRK(x)	(0x3100 + ((x) * 4))
#define IXGBE_VFRETA(x)		(0x3200 + ((x) * 4))

/* VFMRQC bits */
#define IXGBE_VFMRQC_RSSEN		0x00000001  /* RSS Enable */
#define IXGBE_VFMRQC_RSS_FIELD_IPV4_TCP	0x00010000
#define IXGBE_VFMRQC_RSS_FIELD_IPV4	0x00020000
#define IXGBE_VFMRQC_RSS_FIELD_IPV6	0x00100000
#define IXGBE_VFMRQC_RSS_FIELD_IPV6_TCP	0x00200000

#define IXGBE_WRITE_FLUSH(a)	(IXGBE_READ_REG(a, IXGBE_VFSTATUS))

#endif /* _IXGBEVF_REGS_H_ */
