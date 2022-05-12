// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2016 Imagination Technologies
 * Author: Paul Burton <paul.burton@mips.com>
 */

#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/clocksource.h>
#include <linux/init.h>
#include <linux/types.h>

#include <asm/irq.h>
#include <asm/mips-cps.h>
#include <asm/time.h>

int get_c0_fdc_int(void)
{
	int mips_cpu_fdc_irq;

	if (mips_gic_present())
		mips_cpu_fdc_irq = gic_get_c0_fdc_int();
	else if (cpu_has_veic)
		panic("Unimplemented!");
	else if (cp0_fdc_irq >= 0)
		mips_cpu_fdc_irq = MIPS_CPU_IRQ_BASE + cp0_fdc_irq;
	else
		mips_cpu_fdc_irq = -1;

	return mips_cpu_fdc_irq;
}

int get_c0_perfcount_int(void)
{
	int mips_cpu_perf_irq;

	if (mips_gic_present())
		mips_cpu_perf_irq = gic_get_c0_perfcount_int();
	else if (cpu_has_veic)
		panic("Unimplemented!");
	else if (cp0_perfcount_irq >= 0)
		mips_cpu_perf_irq = MIPS_CPU_IRQ_BASE + cp0_perfcount_irq;
	else
		mips_cpu_perf_irq = -1;

	return mips_cpu_perf_irq;
}

unsigned int get_c0_compare_int(void)
{
	int mips_cpu_timer_irq;

	if (mips_gic_present())
		mips_cpu_timer_irq = gic_get_c0_compare_int();
	else if (cpu_has_veic)
		panic("Unimplemented!");
	else
		mips_cpu_timer_irq = MIPS_CPU_IRQ_BASE + cp0_compare_irq;

	return mips_cpu_timer_irq;
}
