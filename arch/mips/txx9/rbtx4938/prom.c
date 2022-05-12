/*
 * rbtx4938 specific prom routines
 * Copyright (C) 2000-2001 Toshiba Corporation
 *
 * 2003-2005 (c) MontaVista Software, Inc. This file is licensed under the
 * terms of the GNU General Public License version 2. This program is
 * licensed "as is" without any warranty of any kind, whether express
 * or implied.
 *
 * Support for TX4938 in 2.6 - Manish Lachwani (mlachwani@mvista.com)
 */

#include <linux/init.h>
#include <linux/memblock.h>
#include <asm/txx9/generic.h>
#include <asm/txx9/rbtx4938.h>

void __init rbtx4938_prom_init(void)
{
	memblock_add(0, tx4938_get_mem_size());
	txx9_sio_putchar_init(TX4938_SIO_REG(0) & 0xfffffffffULL);
}
