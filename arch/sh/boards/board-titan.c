// SPDX-License-Identifier: GPL-2.0
/*
 * arch/sh/boards/titan/setup.c - Setup for Titan
 *
 *  Copyright (C) 2006  Jamie Lenehan
 */
#include <linux/init.h>
#include <linux/irq.h>
#include <mach/titan.h>
#include <asm/io.h>

static void __init init_titan_irq(void)
{
	/* enable individual interrupt mode for externals */
	plat_irq_setup_pins(IRQ_MODE_IRQ);
}

static struct sh_machine_vector mv_titan __initmv = {
	.mv_name	= "Titan",
	.mv_init_irq	= init_titan_irq,
};
