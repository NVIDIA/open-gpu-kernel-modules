// SPDX-License-Identifier: GPL-2.0
/*
 *  Written by Paul B Schroeder < pschroeder "at" uplogix "dot" com >
 *  Based on 8250_boca.
 *
 *  Copyright (C) 2005 Russell King.
 *  Data taken from include/asm-i386/serial.h
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/serial_8250.h>

#include "8250.h"

static struct plat_serial8250_port exar_data[] = {
	SERIAL8250_PORT(0x100, 5),
	SERIAL8250_PORT(0x108, 5),
	SERIAL8250_PORT(0x110, 5),
	SERIAL8250_PORT(0x118, 5),
	{ },
};

static struct platform_device exar_device = {
	.name			= "serial8250",
	.id			= PLAT8250_DEV_EXAR_ST16C554,
	.dev			= {
		.platform_data	= exar_data,
	},
};

static int __init exar_init(void)
{
	return platform_device_register(&exar_device);
}

module_init(exar_init);

MODULE_AUTHOR("Paul B Schroeder");
MODULE_DESCRIPTION("8250 serial probe module for Exar cards");
MODULE_LICENSE("GPL");
