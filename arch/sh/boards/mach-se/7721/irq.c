// SPDX-License-Identifier: GPL-2.0
/*
 * linux/arch/sh/boards/se/7721/irq.c
 *
 * Copyright (C) 2008  Renesas Solutions Corp.
 */
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <mach-se/mach/se7721.h>

enum {
	UNUSED = 0,

	/* board specific interrupt sources */
	MRSHPC,
};

static struct intc_vect vectors[] __initdata = {
	INTC_IRQ(MRSHPC, MRSHPC_IRQ0),
};

static struct intc_prio_reg prio_registers[] __initdata = {
	{ FPGA_ILSR6, 0, 8, 4, /* IRLMSK */
	  { 0, MRSHPC } },
};

static DECLARE_INTC_DESC(intc_desc, "SE7721", vectors,
			 NULL, NULL, prio_registers, NULL);

/*
 * Initialize IRQ setting
 */
void __init init_se7721_IRQ(void)
{
	/* PPCR */
	__raw_writew(__raw_readw(0xa4050118) & ~0x00ff, 0xa4050118);

	register_intc_controller(&intc_desc);
	intc_set_priority(MRSHPC_IRQ0, 0xf - MRSHPC_IRQ0);
}
