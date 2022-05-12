/* SPDX-License-Identifier: GPL-2.0 */
/*
 * OMAP3-common clock function prototypes and macros
 *
 * Copyright (C) 2007-2010 Texas Instruments, Inc.
 * Copyright (C) 2007-2010 Nokia Corporation
 */

#ifndef __ARCH_ARM_MACH_OMAP2_CLOCK3XXX_H
#define __ARCH_ARM_MACH_OMAP2_CLOCK3XXX_H

int omap3xxx_clk_init(void);
int omap3_core_dpll_m2_set_rate(struct clk_hw *clk, unsigned long rate,
					unsigned long parent_rate);

extern struct clk *sdrc_ick_p;
extern struct clk *arm_fck_p;

extern const struct clkops clkops_noncore_dpll_ops;

#endif
