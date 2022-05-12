// SPDX-License-Identifier: GPL-2.0
/*
 *  Copyright (C) 2005 Russell King.
 *  Data taken from include/asm-i386/serial.h
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/serial_8250.h>

#include "8250.h"

static struct plat_serial8250_port boca_data[] = {
	SERIAL8250_PORT(0x100, 12),
	SERIAL8250_PORT(0x108, 12),
	SERIAL8250_PORT(0x110, 12),
	SERIAL8250_PORT(0x118, 12),
	SERIAL8250_PORT(0x120, 12),
	SERIAL8250_PORT(0x128, 12),
	SERIAL8250_PORT(0x130, 12),
	SERIAL8250_PORT(0x138, 12),
	SERIAL8250_PORT(0x140, 12),
	SERIAL8250_PORT(0x148, 12),
	SERIAL8250_PORT(0x150, 12),
	SERIAL8250_PORT(0x158, 12),
	SERIAL8250_PORT(0x160, 12),
	SERIAL8250_PORT(0x168, 12),
	SERIAL8250_PORT(0x170, 12),
	SERIAL8250_PORT(0x178, 12),
	{ },
};

static struct platform_device boca_device = {
	.name			= "serial8250",
	.id			= PLAT8250_DEV_BOCA,
	.dev			= {
		.platform_data	= boca_data,
	},
};

static int __init boca_init(void)
{
	return platform_device_register(&boca_device);
}

module_init(boca_init);

MODULE_AUTHOR("Russell King");
MODULE_DESCRIPTION("8250 serial probe module for Boca cards");
MODULE_LICENSE("GPL");
