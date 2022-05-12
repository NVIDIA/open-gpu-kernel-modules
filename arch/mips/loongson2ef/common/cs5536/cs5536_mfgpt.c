// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * CS5536 General timer functions
 *
 * Copyright (C) 2007 Lemote Inc. & Institute of Computing Technology
 * Author: Yanhua, yanh@lemote.com
 *
 * Copyright (C) 2009 Lemote Inc.
 * Author: Wu zhangjin, wuzhangjin@gmail.com
 *
 * Reference: AMD Geode(TM) CS5536 Companion Device Data Book
 */

#include <linux/io.h>
#include <linux/init.h>
#include <linux/export.h>
#include <linux/jiffies.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/clockchips.h>

#include <asm/time.h>

#include <cs5536/cs5536_mfgpt.h>

static DEFINE_RAW_SPINLOCK(mfgpt_lock);

static u32 mfgpt_base;

/*
 * Initialize the MFGPT timer.
 *
 * This is also called after resume to bring the MFGPT into operation again.
 */

/* disable counter */
void disable_mfgpt0_counter(void)
{
	outw(inw(MFGPT0_SETUP) & 0x7fff, MFGPT0_SETUP);
}
EXPORT_SYMBOL(disable_mfgpt0_counter);

/* enable counter, comparator2 to event mode, 14.318MHz clock */
void enable_mfgpt0_counter(void)
{
	outw(0xe310, MFGPT0_SETUP);
}
EXPORT_SYMBOL(enable_mfgpt0_counter);

static int mfgpt_timer_set_periodic(struct clock_event_device *evt)
{
	raw_spin_lock(&mfgpt_lock);

	outw(COMPARE, MFGPT0_CMP2);	/* set comparator2 */
	outw(0, MFGPT0_CNT);		/* set counter to 0 */
	enable_mfgpt0_counter();

	raw_spin_unlock(&mfgpt_lock);
	return 0;
}

static int mfgpt_timer_shutdown(struct clock_event_device *evt)
{
	if (clockevent_state_periodic(evt) || clockevent_state_oneshot(evt)) {
		raw_spin_lock(&mfgpt_lock);
		disable_mfgpt0_counter();
		raw_spin_unlock(&mfgpt_lock);
	}

	return 0;
}

static struct clock_event_device mfgpt_clockevent = {
	.name = "mfgpt",
	.features = CLOCK_EVT_FEAT_PERIODIC,

	/* The oneshot mode have very high deviation, don't use it! */
	.set_state_shutdown = mfgpt_timer_shutdown,
	.set_state_periodic = mfgpt_timer_set_periodic,
	.irq = CS5536_MFGPT_INTR,
};

static irqreturn_t timer_interrupt(int irq, void *dev_id)
{
	u32 basehi;

	/*
	 * get MFGPT base address
	 *
	 * NOTE: do not remove me, it's need for the value of mfgpt_base is
	 * variable
	 */
	_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_MFGPT), &basehi, &mfgpt_base);

	/* ack */
	outw(inw(MFGPT0_SETUP) | 0x4000, MFGPT0_SETUP);

	mfgpt_clockevent.event_handler(&mfgpt_clockevent);

	return IRQ_HANDLED;
}

/*
 * Initialize the conversion factor and the min/max deltas of the clock event
 * structure and register the clock event source with the framework.
 */
void __init setup_mfgpt0_timer(void)
{
	u32 basehi;
	struct clock_event_device *cd = &mfgpt_clockevent;
	unsigned int cpu = smp_processor_id();

	cd->cpumask = cpumask_of(cpu);
	clockevent_set_clock(cd, MFGPT_TICK_RATE);
	cd->max_delta_ns = clockevent_delta2ns(0xffff, cd);
	cd->max_delta_ticks = 0xffff;
	cd->min_delta_ns = clockevent_delta2ns(0xf, cd);
	cd->min_delta_ticks = 0xf;

	/* Enable MFGPT0 Comparator 2 Output to the Interrupt Mapper */
	_wrmsr(DIVIL_MSR_REG(MFGPT_IRQ), 0, 0x100);

	/* Enable Interrupt Gate 5 */
	_wrmsr(DIVIL_MSR_REG(PIC_ZSEL_LOW), 0, 0x50000);

	/* get MFGPT base address */
	_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_MFGPT), &basehi, &mfgpt_base);

	clockevents_register_device(cd);

	if (request_irq(CS5536_MFGPT_INTR, timer_interrupt,
			IRQF_NOBALANCING | IRQF_TIMER, "timer", NULL))
		pr_err("Failed to register timer interrupt\n");
}

/*
 * Since the MFGPT overflows every tick, its not very useful
 * to just read by itself. So use jiffies to emulate a free
 * running counter:
 */
static u64 mfgpt_read(struct clocksource *cs)
{
	unsigned long flags;
	int count;
	u32 jifs;
	static int old_count;
	static u32 old_jifs;

	raw_spin_lock_irqsave(&mfgpt_lock, flags);
	/*
	 * Although our caller may have the read side of xtime_lock,
	 * this is now a seqlock, and we are cheating in this routine
	 * by having side effects on state that we cannot undo if
	 * there is a collision on the seqlock and our caller has to
	 * retry.  (Namely, old_jifs and old_count.)  So we must treat
	 * jiffies as volatile despite the lock.  We read jiffies
	 * before latching the timer count to guarantee that although
	 * the jiffies value might be older than the count (that is,
	 * the counter may underflow between the last point where
	 * jiffies was incremented and the point where we latch the
	 * count), it cannot be newer.
	 */
	jifs = jiffies;
	/* read the count */
	count = inw(MFGPT0_CNT);

	/*
	 * It's possible for count to appear to go the wrong way for this
	 * reason:
	 *
	 *  The timer counter underflows, but we haven't handled the resulting
	 *  interrupt and incremented jiffies yet.
	 *
	 * Previous attempts to handle these cases intelligently were buggy, so
	 * we just do the simple thing now.
	 */
	if (count < old_count && jifs == old_jifs)
		count = old_count;

	old_count = count;
	old_jifs = jifs;

	raw_spin_unlock_irqrestore(&mfgpt_lock, flags);

	return (u64) (jifs * COMPARE) + count;
}

static struct clocksource clocksource_mfgpt = {
	.name = "mfgpt",
	.rating = 120, /* Functional for real use, but not desired */
	.read = mfgpt_read,
	.mask = CLOCKSOURCE_MASK(32),
};

int __init init_mfgpt_clocksource(void)
{
	if (num_possible_cpus() > 1)	/* MFGPT does not scale! */
		return 0;

	return clocksource_register_hz(&clocksource_mfgpt, MFGPT_TICK_RATE);
}

arch_initcall(init_mfgpt_clocksource);
