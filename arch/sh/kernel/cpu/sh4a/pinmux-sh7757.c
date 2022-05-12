// SPDX-License-Identifier: GPL-2.0
/*
 * SH7757 (B0 step) Pinmux
 *
 *  Copyright (C) 2009-2010  Renesas Solutions Corp.
 *
 *  Author : Yoshihiro Shimoda <shimoda.yoshihiro@renesas.com>
 *
 * Based on SH7723 Pinmux
 *  Copyright (C) 2008  Magnus Damm
 */

#include <linux/bug.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <cpu/pfc.h>

static struct resource sh7757_pfc_resources[] = {
	[0] = {
		.start	= 0xffec0000,
		.end	= 0xffec008f,
		.flags	= IORESOURCE_MEM,
	},
};

static int __init plat_pinmux_setup(void)
{
	return sh_pfc_register("pfc-sh7757", sh7757_pfc_resources,
			       ARRAY_SIZE(sh7757_pfc_resources));
}
arch_initcall(plat_pinmux_setup);
