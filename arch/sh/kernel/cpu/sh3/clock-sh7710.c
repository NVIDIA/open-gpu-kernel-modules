// SPDX-License-Identifier: GPL-2.0
/*
 * arch/sh/kernel/cpu/sh3/clock-sh7710.c
 *
 * SH7710 support for the clock framework
 *
 *  Copyright (C) 2005  Paul Mundt
 *
 * FRQCR parsing hacked out of arch/sh/kernel/time.c
 *
 *  Copyright (C) 1999  Tetsuya Okada & Niibe Yutaka
 *  Copyright (C) 2000  Philipp Rumpf <prumpf@tux.org>
 *  Copyright (C) 2002, 2003, 2004  Paul Mundt
 *  Copyright (C) 2002  M. R. Brown  <mrbrown@linux-sh.org>
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/clock.h>
#include <asm/freq.h>
#include <asm/io.h>

static int md_table[] = { 1, 2, 3, 4, 6, 8, 12 };

static void master_clk_init(struct clk *clk)
{
	clk->rate *= md_table[__raw_readw(FRQCR) & 0x0007];
}

static struct sh_clk_ops sh7710_master_clk_ops = {
	.init		= master_clk_init,
};

static unsigned long module_clk_recalc(struct clk *clk)
{
	int idx = (__raw_readw(FRQCR) & 0x0007);
	return clk->parent->rate / md_table[idx];
}

static struct sh_clk_ops sh7710_module_clk_ops = {
	.recalc		= module_clk_recalc,
};

static unsigned long bus_clk_recalc(struct clk *clk)
{
	int idx = (__raw_readw(FRQCR) & 0x0700) >> 8;
	return clk->parent->rate / md_table[idx];
}

static struct sh_clk_ops sh7710_bus_clk_ops = {
	.recalc		= bus_clk_recalc,
};

static unsigned long cpu_clk_recalc(struct clk *clk)
{
	int idx = (__raw_readw(FRQCR) & 0x0070) >> 4;
	return clk->parent->rate / md_table[idx];
}

static struct sh_clk_ops sh7710_cpu_clk_ops = {
	.recalc		= cpu_clk_recalc,
};

static struct sh_clk_ops *sh7710_clk_ops[] = {
	&sh7710_master_clk_ops,
	&sh7710_module_clk_ops,
	&sh7710_bus_clk_ops,
	&sh7710_cpu_clk_ops,
};

void __init arch_init_clk_ops(struct sh_clk_ops **ops, int idx)
{
	if (idx < ARRAY_SIZE(sh7710_clk_ops))
		*ops = sh7710_clk_ops[idx];
}

