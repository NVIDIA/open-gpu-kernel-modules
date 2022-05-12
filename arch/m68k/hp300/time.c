// SPDX-License-Identifier: GPL-2.0
/*
 *  linux/arch/m68k/hp300/time.c
 *
 *  Copyright (C) 1998 Philip Blundell <philb@gnu.org>
 *
 *  This file contains the HP300-specific time handling code.
 */

#include <asm/ptrace.h>
#include <linux/clocksource.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel_stat.h>
#include <linux/interrupt.h>
#include <asm/machdep.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/traps.h>
#include <asm/blinken.h>

static u64 hp300_read_clk(struct clocksource *cs);

static struct clocksource hp300_clk = {
	.name   = "timer",
	.rating = 250,
	.read   = hp300_read_clk,
	.mask   = CLOCKSOURCE_MASK(32),
	.flags  = CLOCK_SOURCE_IS_CONTINUOUS,
};

static u32 clk_total, clk_offset;

/* Clock hardware definitions */

#define CLOCKBASE	0xf05f8000

#define	CLKCR1		0x1
#define	CLKCR2		0x3
#define	CLKCR3		CLKCR1
#define	CLKSR		CLKCR2
#define	CLKMSB1		0x5
#define	CLKLSB1		0x7
#define	CLKMSB2		0x9
#define	CLKMSB3		0xD

#define	CLKSR_INT1	BIT(0)

/* This is for machines which generate the exact clock. */

#define HP300_TIMER_CLOCK_FREQ 250000
#define HP300_TIMER_CYCLES     (HP300_TIMER_CLOCK_FREQ / HZ)
#define INTVAL                 (HP300_TIMER_CYCLES - 1)

static irqreturn_t hp300_tick(int irq, void *dev_id)
{
	unsigned long flags;
	unsigned long tmp;

	local_irq_save(flags);
	in_8(CLOCKBASE + CLKSR);
	asm volatile ("movpw %1@(5),%0" : "=d" (tmp) : "a" (CLOCKBASE));
	clk_total += INTVAL;
	clk_offset = 0;
	legacy_timer_tick(1);
	timer_heartbeat();
	local_irq_restore(flags);

	/* Turn off the network and SCSI leds */
	blinken_leds(0, 0xe0);
	return IRQ_HANDLED;
}

static u64 hp300_read_clk(struct clocksource *cs)
{
	unsigned long flags;
	unsigned char lsb, msb, msb_new;
	u32 ticks;

	local_irq_save(flags);
	/* Read current timer 1 value */
	msb = in_8(CLOCKBASE + CLKMSB1);
again:
	if ((in_8(CLOCKBASE + CLKSR) & CLKSR_INT1) && msb > 0)
		clk_offset = INTVAL;
	lsb = in_8(CLOCKBASE + CLKLSB1);
	msb_new = in_8(CLOCKBASE + CLKMSB1);
	if (msb_new != msb) {
		msb = msb_new;
		goto again;
	}

	ticks = INTVAL - ((msb << 8) | lsb);
	ticks += clk_offset + clk_total;
	local_irq_restore(flags);

	return ticks;
}

void __init hp300_sched_init(void)
{
  out_8(CLOCKBASE + CLKCR2, 0x1);		/* select CR1 */
  out_8(CLOCKBASE + CLKCR1, 0x1);		/* reset */

  asm volatile(" movpw %0,%1@(5)" : : "d" (INTVAL), "a" (CLOCKBASE));

  if (request_irq(IRQ_AUTO_6, hp300_tick, IRQF_TIMER, "timer tick", NULL))
    pr_err("Couldn't register timer interrupt\n");

  out_8(CLOCKBASE + CLKCR2, 0x1);		/* select CR1 */
  out_8(CLOCKBASE + CLKCR1, 0x40);		/* enable irq */

  clocksource_register_hz(&hp300_clk, HP300_TIMER_CLOCK_FREQ);
}
