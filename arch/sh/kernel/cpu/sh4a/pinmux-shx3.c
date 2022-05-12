// SPDX-License-Identifier: GPL-2.0
/*
 * SH-X3 prototype CPU pinmux
 *
 * Copyright (C) 2010  Paul Mundt
 */
#include <linux/bug.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <cpu/pfc.h>

static struct resource shx3_pfc_resources[] = {
	[0] = {
		.start	= 0xffc70000,
		.end	= 0xffc7001f,
		.flags	= IORESOURCE_MEM,
	},
};

static int __init plat_pinmux_setup(void)
{
	return sh_pfc_register("pfc-shx3", shx3_pfc_resources,
			       ARRAY_SIZE(shx3_pfc_resources));
}
arch_initcall(plat_pinmux_setup);
