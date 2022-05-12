/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2005-2006 by Texas Instruments
 */

#ifndef __MUSB_OMAP243X_H__
#define __MUSB_OMAP243X_H__

#include <linux/platform_data/usb-omap.h>

/*
 * OMAP2430-specific definitions
 */

#define OTG_REVISION		0x400

#define OTG_SYSCONFIG		0x404
#	define	MIDLEMODE	12	/* bit position */
#	define	FORCESTDBY		(0 << MIDLEMODE)
#	define	NOSTDBY			(1 << MIDLEMODE)
#	define	SMARTSTDBY		(2 << MIDLEMODE)

#	define	SIDLEMODE		3	/* bit position */
#	define	FORCEIDLE		(0 << SIDLEMODE)
#	define	NOIDLE			(1 << SIDLEMODE)
#	define	SMARTIDLE		(2 << SIDLEMODE)

#	define	ENABLEWAKEUP		(1 << 2)
#	define	SOFTRST			(1 << 1)
#	define	AUTOIDLE		(1 << 0)

#define OTG_SYSSTATUS		0x408
#	define	RESETDONE		(1 << 0)

#define OTG_INTERFSEL		0x40c
#	define	EXTCP			(1 << 2)
#	define	PHYSEL			0	/* bit position */
#	define	UTMI_8BIT		(0 << PHYSEL)
#	define	ULPI_12PIN		(1 << PHYSEL)
#	define	ULPI_8PIN		(2 << PHYSEL)

#define OTG_SIMENABLE		0x410
#	define	TM1			(1 << 0)

#define OTG_FORCESTDBY		0x414
#	define	ENABLEFORCE		(1 << 0)

#endif	/* __MUSB_OMAP243X_H__ */
