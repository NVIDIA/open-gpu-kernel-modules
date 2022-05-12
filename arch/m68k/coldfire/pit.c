// SPDX-License-Identifier: GPL-2.0
/***************************************************************************/

/*
 *	pit.c -- Freescale ColdFire PIT timer. Currently this type of
 *	         hardware timer only exists in the Freescale ColdFire
 *		 5270/5271, 5282 and 5208 CPUs. No doubt newer ColdFire
 *		 family members will probably use it too.
 *
 *	Copyright (C) 1999-2008, Greg Ungerer (gerg@snapgear.com)
 *	Copyright (C) 2001-2004, SnapGear Inc. (www.snapgear.com)
 */

/***************************************************************************/

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/param.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/clockchips.h>
#include <asm/machdep.h>
#include <asm/io.h>
#include <asm/coldfire.h>
#include <asm/mcfpit.h>
#include <asm/mcfsim.h>

/***************************************************************************/

/*
 *	By default use timer1 as the system clock timer.
 */
#define	FREQ	((MCF_CLK / 2) / 64)
#define	TA(a)	(MCFPIT_BASE1 + (a))
#define PIT_CYCLES_PER_JIFFY (FREQ / HZ)

static u32 pit_cnt;

/*
 * Initialize the PIT timer.
 *
 * This is also called after resume to bring the PIT into operation again.
 */

static int cf_pit_set_periodic(struct clock_event_device *evt)
{
	__raw_writew(MCFPIT_PCSR_DISABLE, TA(MCFPIT_PCSR));
	__raw_writew(PIT_CYCLES_PER_JIFFY, TA(MCFPIT_PMR));
	__raw_writew(MCFPIT_PCSR_EN | MCFPIT_PCSR_PIE |
		     MCFPIT_PCSR_OVW | MCFPIT_PCSR_RLD |
		     MCFPIT_PCSR_CLK64, TA(MCFPIT_PCSR));
	return 0;
}

static int cf_pit_set_oneshot(struct clock_event_device *evt)
{
	__raw_writew(MCFPIT_PCSR_DISABLE, TA(MCFPIT_PCSR));
	__raw_writew(MCFPIT_PCSR_EN | MCFPIT_PCSR_PIE |
		     MCFPIT_PCSR_OVW | MCFPIT_PCSR_CLK64, TA(MCFPIT_PCSR));
	return 0;
}

static int cf_pit_shutdown(struct clock_event_device *evt)
{
	__raw_writew(MCFPIT_PCSR_DISABLE, TA(MCFPIT_PCSR));
	return 0;
}

/*
 * Program the next event in oneshot mode
 *
 * Delta is given in PIT ticks
 */
static int cf_pit_next_event(unsigned long delta,
		struct clock_event_device *evt)
{
	__raw_writew(delta, TA(MCFPIT_PMR));
	return 0;
}

struct clock_event_device cf_pit_clockevent = {
	.name			= "pit",
	.features		= CLOCK_EVT_FEAT_PERIODIC |
				  CLOCK_EVT_FEAT_ONESHOT,
	.set_state_shutdown	= cf_pit_shutdown,
	.set_state_periodic	= cf_pit_set_periodic,
	.set_state_oneshot	= cf_pit_set_oneshot,
	.set_next_event		= cf_pit_next_event,
	.shift			= 32,
	.irq			= MCF_IRQ_PIT1,
};



/***************************************************************************/

static irqreturn_t pit_tick(int irq, void *dummy)
{
	struct clock_event_device *evt = &cf_pit_clockevent;
	u16 pcsr;

	/* Reset the ColdFire timer */
	pcsr = __raw_readw(TA(MCFPIT_PCSR));
	__raw_writew(pcsr | MCFPIT_PCSR_PIF, TA(MCFPIT_PCSR));

	pit_cnt += PIT_CYCLES_PER_JIFFY;
	evt->event_handler(evt);
	return IRQ_HANDLED;
}

/***************************************************************************/

static u64 pit_read_clk(struct clocksource *cs)
{
	unsigned long flags;
	u32 cycles;
	u16 pcntr;

	local_irq_save(flags);
	pcntr = __raw_readw(TA(MCFPIT_PCNTR));
	cycles = pit_cnt;
	local_irq_restore(flags);

	return cycles + PIT_CYCLES_PER_JIFFY - pcntr;
}

/***************************************************************************/

static struct clocksource pit_clk = {
	.name	= "pit",
	.rating	= 100,
	.read	= pit_read_clk,
	.mask	= CLOCKSOURCE_MASK(32),
};

/***************************************************************************/

void hw_timer_init(void)
{
	int ret;

	cf_pit_clockevent.cpumask = cpumask_of(smp_processor_id());
	cf_pit_clockevent.mult = div_sc(FREQ, NSEC_PER_SEC, 32);
	cf_pit_clockevent.max_delta_ns =
		clockevent_delta2ns(0xFFFF, &cf_pit_clockevent);
	cf_pit_clockevent.max_delta_ticks = 0xFFFF;
	cf_pit_clockevent.min_delta_ns =
		clockevent_delta2ns(0x3f, &cf_pit_clockevent);
	cf_pit_clockevent.min_delta_ticks = 0x3f;
	clockevents_register_device(&cf_pit_clockevent);

	ret = request_irq(MCF_IRQ_PIT1, pit_tick, IRQF_TIMER, "timer", NULL);
	if (ret) {
		pr_err("Failed to request irq %d (timer): %pe\n", MCF_IRQ_PIT1,
		       ERR_PTR(ret));
	}

	clocksource_register_hz(&pit_clk, FREQ);
}

/***************************************************************************/
