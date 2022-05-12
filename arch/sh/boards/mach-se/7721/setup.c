// SPDX-License-Identifier: GPL-2.0
/*
 * linux/arch/sh/boards/se/7721/setup.c
 *
 * Copyright (C) 2008 Renesas Solutions Corp.
 *
 * Hitachi UL SolutionEngine 7721 Support.
 */
#include <linux/init.h>
#include <linux/platform_device.h>
#include <mach-se/mach/se7721.h>
#include <mach-se/mach/mrshpc.h>
#include <asm/machvec.h>
#include <asm/io.h>
#include <asm/heartbeat.h>

static unsigned char heartbeat_bit_pos[] = { 8, 9, 10, 11, 12, 13, 14, 15 };

static struct heartbeat_data heartbeat_data = {
	.bit_pos	= heartbeat_bit_pos,
	.nr_bits	= ARRAY_SIZE(heartbeat_bit_pos),
};

static struct resource heartbeat_resource = {
	.start	= PA_LED,
	.end	= PA_LED,
	.flags	= IORESOURCE_MEM | IORESOURCE_MEM_16BIT,
};

static struct platform_device heartbeat_device = {
	.name		= "heartbeat",
	.id		= -1,
	.dev	= {
		.platform_data	= &heartbeat_data,
	},
	.num_resources	= 1,
	.resource	= &heartbeat_resource,
};

static struct resource cf_ide_resources[] = {
	[0] = {
		.start	= PA_MRSHPC_IO + 0x1f0,
		.end	= PA_MRSHPC_IO + 0x1f0 + 8 ,
		.flags	= IORESOURCE_IO,
	},
	[1] = {
		.start	= PA_MRSHPC_IO + 0x1f0 + 0x206,
		.end	= PA_MRSHPC_IO + 0x1f0 + 8 + 0x206 + 8,
		.flags	= IORESOURCE_IO,
	},
	[2] = {
		.start  = MRSHPC_IRQ0,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device cf_ide_device = {
	.name		= "pata_platform",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(cf_ide_resources),
	.resource	= cf_ide_resources,
};

static struct platform_device *se7721_devices[] __initdata = {
	&cf_ide_device,
	&heartbeat_device
};

static int __init se7721_devices_setup(void)
{
	mrshpc_setup_windows();
	return platform_add_devices(se7721_devices, ARRAY_SIZE(se7721_devices));
}
device_initcall(se7721_devices_setup);

static void __init se7721_setup(char **cmdline_p)
{
	/* for USB */
	__raw_writew(0x0000, 0xA405010C);	/* PGCR */
	__raw_writew(0x0000, 0xA405010E);	/* PHCR */
	__raw_writew(0x00AA, 0xA4050118);	/* PPCR */
	__raw_writew(0x0000, 0xA4050124);	/* PSELA */
}

/*
 * The Machine Vector
 */
struct sh_machine_vector mv_se7721 __initmv = {
	.mv_name		= "Solution Engine 7721",
	.mv_setup		= se7721_setup,
	.mv_init_irq		= init_se7721_IRQ,
};
