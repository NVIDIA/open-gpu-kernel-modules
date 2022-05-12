/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright 2004-2007, 2014 Freescale Semiconductor, Inc. All Rights Reserved.
 * Copyright 2008 Juergen Beisert, kernel@pengutronix.de
 */

#ifndef __ASM_ARCH_MXC_HARDWARE_H__
#define __ASM_ARCH_MXC_HARDWARE_H__

#ifndef __ASSEMBLY__
#include <asm/io.h>
#include <soc/imx/revision.h>
#endif
#include <linux/sizes.h>

#define addr_in_module(addr, mod) \
	((unsigned long)(addr) - mod ## _BASE_ADDR < mod ## _SIZE)

#define IMX_IO_P2V_MODULE(addr, module)					\
	(((addr) - module ## _BASE_ADDR) < module ## _SIZE ?		\
	 (addr) - (module ## _BASE_ADDR) + (module ## _BASE_ADDR_VIRT) : 0)

/*
 * This is rather complicated for humans and ugly to verify, but for a machine
 * it's OK.  Still more as it is usually only applied to constants.  The upsides
 * on using this approach are:
 *
 *  - same mapping on all i.MX machines
 *  - works for assembler, too
 *  - no need to nurture #defines for virtual addresses
 *
 * The downside it, it's hard to verify (but I have a script for that).
 *
 * Obviously this needs to be injective for each SoC.  In general it maps the
 * whole address space to [0xf4000000, 0xf5ffffff].  So [0xf6000000,0xfeffffff]
 * is free for per-machine use (e.g. KZM_ARM11_01 uses 64MiB there).
 *
 * It applies the following mappings for the different SoCs:
 *
 * mx1:
 *	IO	0x00200000+0x100000	->	0xf4000000+0x100000
 * mx21:
 *	AIPI	0x10000000+0x100000	->	0xf4400000+0x100000
 *	SAHB1	0x80000000+0x100000	->	0xf5000000+0x100000
 *	X_MEMC	0xdf000000+0x004000	->	0xf5f00000+0x004000
 * mx25:
 *	AIPS1	0x43f00000+0x100000	->	0xf5300000+0x100000
 *	AIPS2	0x53f00000+0x100000	->	0xf5700000+0x100000
 *	AVIC	0x68000000+0x100000	->	0xf5800000+0x100000
 * mx27:
 *	AIPI	0x10000000+0x100000	->	0xf4400000+0x100000
 *	SAHB1	0x80000000+0x100000	->	0xf5000000+0x100000
 *	X_MEMC	0xd8000000+0x100000	->	0xf5c00000+0x100000
 * mx31:
 *	AIPS1	0x43f00000+0x100000	->	0xf5300000+0x100000
 *	AIPS2	0x53f00000+0x100000	->	0xf5700000+0x100000
 *	AVIC	0x68000000+0x100000	->	0xf5800000+0x100000
 *	X_MEMC	0xb8000000+0x010000	->	0xf5c00000+0x010000
 *	SPBA0	0x50000000+0x100000	->	0xf5400000+0x100000
 * mx35:
 *	AIPS1	0x43f00000+0x100000	->	0xf5300000+0x100000
 *	AIPS2	0x53f00000+0x100000	->	0xf5700000+0x100000
 *	AVIC	0x68000000+0x100000	->	0xf5800000+0x100000
 *	X_MEMC	0xb8000000+0x010000	->	0xf5c00000+0x010000
 *	SPBA0	0x50000000+0x100000	->	0xf5400000+0x100000
 * mx51:
 *	TZIC	0x0fffc000+0x004000	->	0xf4bfc000+0x004000
 *	IRAM	0x1ffe0000+0x020000	->	0xf4fe0000+0x020000
 *	DEBUG	0x60000000+0x100000	->	0xf5000000+0x100000
 *	SPBA0	0x70000000+0x100000	->	0xf5400000+0x100000
 *	AIPS1	0x73f00000+0x100000	->	0xf5700000+0x100000
 *	AIPS2	0x83f00000+0x100000	->	0xf5300000+0x100000
 * mx53:
 *	TZIC	0x0fffc000+0x004000	->	0xf4bfc000+0x004000
 *	DEBUG	0x40000000+0x100000	->	0xf5000000+0x100000
 *	SPBA0	0x50000000+0x100000	->	0xf5400000+0x100000
 *	AIPS1	0x53f00000+0x100000	->	0xf5700000+0x100000
 *	AIPS2	0x63f00000+0x100000	->	0xf5300000+0x100000
 * mx6q:
 *	SCU	0x00a00000+0x004000	->	0xf4000000+0x004000
 *	CCM	0x020c4000+0x004000	->	0xf42c4000+0x004000
 *	ANATOP	0x020c8000+0x004000	->	0xf42c8000+0x004000
 *	UART4	0x021f0000+0x004000	->	0xf42f0000+0x004000
 */
#define IMX_IO_P2V(x)	(						\
			(((x) & 0x80000000) >> 7) |			\
			(0xf4000000 +					\
			(((x) & 0x50000000) >> 6) +			\
			(((x) & 0x0b000000) >> 4) +			\
			(((x) & 0x000fffff))))

#define IMX_IO_ADDRESS(x)	IOMEM(IMX_IO_P2V(x))

#include "mxc.h"

#include "mx3x.h"
#include "mx31.h"
#include "mx35.h"
#include "mx2x.h"
#include "mx27.h"

#define imx_map_entry(soc, name, _type)	{				\
	.virtual = soc ## _IO_P2V(soc ## _ ## name ## _BASE_ADDR),	\
	.pfn = __phys_to_pfn(soc ## _ ## name ## _BASE_ADDR),		\
	.length = soc ## _ ## name ## _SIZE,				\
	.type = _type,							\
}

#endif /* __ASM_ARCH_MXC_HARDWARE_H__ */
