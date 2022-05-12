// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * arch/arm/plat-iop/time.c
 *
 * Timer code for IOP32x and IOP33x based systems
 *
 * Author: Deepak Saxena <dsaxena@mvista.com>
 *
 * Copyright 2002-2003 MontaVista Software Inc.
 */

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/timex.h>
#include <linux/io.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/export.h>
#include <linux/sched_clock.h>
#include <asm/irq.h>
#include <linux/uaccess.h>
#include <asm/mach/irq.h>
#include <asm/mach/time.h>

#include "hardware.h"
#include "irqs.h"

/*
 * Minimum clocksource/clockevent timer range in seconds
 */
#define IOP_MIN_RANGE 4

/*
 * IOP clocksource (free-running timer 1).
 */
static u64 notrace iop_clocksource_read(struct clocksource *unused)
{
	return 0xffffffffu - read_tcr1();
}

static struct clocksource iop_clocksource = {
	.name 		= "iop_timer1",
	.rating		= 300,
	.read		= iop_clocksource_read,
	.mask		= CLOCKSOURCE_MASK(32),
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
};

/*
 * IOP sched_clock() implementation via its clocksource.
 */
static u64 notrace iop_read_sched_clock(void)
{
	return 0xffffffffu - read_tcr1();
}

/*
 * IOP clockevents (interrupting timer 0).
 */
static int iop_set_next_event(unsigned long delta,
			      struct clock_event_device *unused)
{
	u32 tmr = IOP_TMR_PRIVILEGED | IOP_TMR_RATIO_1_1;

	BUG_ON(delta == 0);
	write_tmr0(tmr & ~(IOP_TMR_EN | IOP_TMR_RELOAD));
	write_tcr0(delta);
	write_tmr0((tmr & ~IOP_TMR_RELOAD) | IOP_TMR_EN);

	return 0;
}

static unsigned long ticks_per_jiffy;

static int iop_set_periodic(struct clock_event_device *evt)
{
	u32 tmr = read_tmr0();

	write_tmr0(tmr & ~IOP_TMR_EN);
	write_tcr0(ticks_per_jiffy - 1);
	write_trr0(ticks_per_jiffy - 1);
	tmr |= (IOP_TMR_RELOAD | IOP_TMR_EN);

	write_tmr0(tmr);
	return 0;
}

static int iop_set_oneshot(struct clock_event_device *evt)
{
	u32 tmr = read_tmr0();

	/* ->set_next_event sets period and enables timer */
	tmr &= ~(IOP_TMR_RELOAD | IOP_TMR_EN);
	write_tmr0(tmr);
	return 0;
}

static int iop_shutdown(struct clock_event_device *evt)
{
	u32 tmr = read_tmr0();

	tmr &= ~IOP_TMR_EN;
	write_tmr0(tmr);
	return 0;
}

static int iop_resume(struct clock_event_device *evt)
{
	u32 tmr = read_tmr0();

	tmr |= IOP_TMR_EN;
	write_tmr0(tmr);
	return 0;
}

static struct clock_event_device iop_clockevent = {
	.name			= "iop_timer0",
	.features		= CLOCK_EVT_FEAT_PERIODIC |
				  CLOCK_EVT_FEAT_ONESHOT,
	.rating			= 300,
	.set_next_event		= iop_set_next_event,
	.set_state_shutdown	= iop_shutdown,
	.set_state_periodic	= iop_set_periodic,
	.tick_resume		= iop_resume,
	.set_state_oneshot	= iop_set_oneshot,
};

static irqreturn_t
iop_timer_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *evt = dev_id;

	write_tisr(1);
	evt->event_handler(evt);
	return IRQ_HANDLED;
}

static unsigned long iop_tick_rate;
unsigned long get_iop_tick_rate(void)
{
	return iop_tick_rate;
}
EXPORT_SYMBOL(get_iop_tick_rate);

void __init iop_init_time(unsigned long tick_rate)
{
	u32 timer_ctl;
	int irq = IRQ_IOP32X_TIMER0;

	sched_clock_register(iop_read_sched_clock, 32, tick_rate);

	ticks_per_jiffy = DIV_ROUND_CLOSEST(tick_rate, HZ);
	iop_tick_rate = tick_rate;

	timer_ctl = IOP_TMR_EN | IOP_TMR_PRIVILEGED |
			IOP_TMR_RELOAD | IOP_TMR_RATIO_1_1;

	/*
	 * Set up interrupting clockevent timer 0.
	 */
	write_tmr0(timer_ctl & ~IOP_TMR_EN);
	write_tisr(1);
	if (request_irq(irq, iop_timer_interrupt, IRQF_TIMER | IRQF_IRQPOLL,
			"IOP Timer Tick", &iop_clockevent))
		pr_err("Failed to request irq() %d (IOP Timer Tick)\n", irq);
	iop_clockevent.cpumask = cpumask_of(0);
	clockevents_config_and_register(&iop_clockevent, tick_rate,
					0xf, 0xfffffffe);

	/*
	 * Set up free-running clocksource timer 1.
	 */
	write_trr1(0xffffffff);
	write_tcr1(0xffffffff);
	write_tmr1(timer_ctl);
	clocksource_register_hz(&iop_clocksource, tick_rate);
}
