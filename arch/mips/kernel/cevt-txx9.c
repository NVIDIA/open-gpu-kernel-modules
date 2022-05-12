/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Based on linux/arch/mips/kernel/cevt-r4k.c,
 *	    linux/arch/mips/jmr3927/rbhma3100/setup.c
 *
 * Copyright 2001 MontaVista Software Inc.
 * Copyright (C) 2000-2001 Toshiba Corporation
 * Copyright (C) 2007 MIPS Technologies, Inc.
 * Copyright (C) 2007 Ralf Baechle <ralf@linux-mips.org>
 */
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched_clock.h>
#include <asm/time.h>
#include <asm/txx9tmr.h>

#define TCR_BASE (TXx9_TMTCR_CCDE | TXx9_TMTCR_CRE | TXx9_TMTCR_TMODE_ITVL)
#define TIMER_CCD	0	/* 1/2 */
#define TIMER_CLK(imclk)	((imclk) / (2 << TIMER_CCD))

struct txx9_clocksource {
	struct clocksource cs;
	struct txx9_tmr_reg __iomem *tmrptr;
};

static u64 txx9_cs_read(struct clocksource *cs)
{
	struct txx9_clocksource *txx9_cs =
		container_of(cs, struct txx9_clocksource, cs);
	return __raw_readl(&txx9_cs->tmrptr->trr);
}

/* Use 1 bit smaller width to use full bits in that width */
#define TXX9_CLOCKSOURCE_BITS (TXX9_TIMER_BITS - 1)

static struct txx9_clocksource txx9_clocksource = {
	.cs = {
		.name		= "TXx9",
		.rating		= 200,
		.read		= txx9_cs_read,
		.mask		= CLOCKSOURCE_MASK(TXX9_CLOCKSOURCE_BITS),
		.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
	},
};

static u64 notrace txx9_read_sched_clock(void)
{
	return __raw_readl(&txx9_clocksource.tmrptr->trr);
}

void __init txx9_clocksource_init(unsigned long baseaddr,
				  unsigned int imbusclk)
{
	struct txx9_tmr_reg __iomem *tmrptr;

	clocksource_register_hz(&txx9_clocksource.cs, TIMER_CLK(imbusclk));

	tmrptr = ioremap(baseaddr, sizeof(struct txx9_tmr_reg));
	__raw_writel(TCR_BASE, &tmrptr->tcr);
	__raw_writel(0, &tmrptr->tisr);
	__raw_writel(TIMER_CCD, &tmrptr->ccdr);
	__raw_writel(TXx9_TMITMR_TZCE, &tmrptr->itmr);
	__raw_writel(1 << TXX9_CLOCKSOURCE_BITS, &tmrptr->cpra);
	__raw_writel(TCR_BASE | TXx9_TMTCR_TCE, &tmrptr->tcr);
	txx9_clocksource.tmrptr = tmrptr;

	sched_clock_register(txx9_read_sched_clock, TXX9_CLOCKSOURCE_BITS,
			     TIMER_CLK(imbusclk));
}

struct txx9_clock_event_device {
	struct clock_event_device cd;
	struct txx9_tmr_reg __iomem *tmrptr;
};

static void txx9tmr_stop_and_clear(struct txx9_tmr_reg __iomem *tmrptr)
{
	/* stop and reset counter */
	__raw_writel(TCR_BASE, &tmrptr->tcr);
	/* clear pending interrupt */
	__raw_writel(0, &tmrptr->tisr);
}

static int txx9tmr_set_state_periodic(struct clock_event_device *evt)
{
	struct txx9_clock_event_device *txx9_cd =
		container_of(evt, struct txx9_clock_event_device, cd);
	struct txx9_tmr_reg __iomem *tmrptr = txx9_cd->tmrptr;

	txx9tmr_stop_and_clear(tmrptr);

	__raw_writel(TXx9_TMITMR_TIIE | TXx9_TMITMR_TZCE, &tmrptr->itmr);
	/* start timer */
	__raw_writel(((u64)(NSEC_PER_SEC / HZ) * evt->mult) >> evt->shift,
		     &tmrptr->cpra);
	__raw_writel(TCR_BASE | TXx9_TMTCR_TCE, &tmrptr->tcr);
	return 0;
}

static int txx9tmr_set_state_oneshot(struct clock_event_device *evt)
{
	struct txx9_clock_event_device *txx9_cd =
		container_of(evt, struct txx9_clock_event_device, cd);
	struct txx9_tmr_reg __iomem *tmrptr = txx9_cd->tmrptr;

	txx9tmr_stop_and_clear(tmrptr);
	__raw_writel(TXx9_TMITMR_TIIE, &tmrptr->itmr);
	return 0;
}

static int txx9tmr_set_state_shutdown(struct clock_event_device *evt)
{
	struct txx9_clock_event_device *txx9_cd =
		container_of(evt, struct txx9_clock_event_device, cd);
	struct txx9_tmr_reg __iomem *tmrptr = txx9_cd->tmrptr;

	txx9tmr_stop_and_clear(tmrptr);
	__raw_writel(0, &tmrptr->itmr);
	return 0;
}

static int txx9tmr_tick_resume(struct clock_event_device *evt)
{
	struct txx9_clock_event_device *txx9_cd =
		container_of(evt, struct txx9_clock_event_device, cd);
	struct txx9_tmr_reg __iomem *tmrptr = txx9_cd->tmrptr;

	txx9tmr_stop_and_clear(tmrptr);
	__raw_writel(TIMER_CCD, &tmrptr->ccdr);
	__raw_writel(0, &tmrptr->itmr);
	return 0;
}

static int txx9tmr_set_next_event(unsigned long delta,
				  struct clock_event_device *evt)
{
	struct txx9_clock_event_device *txx9_cd =
		container_of(evt, struct txx9_clock_event_device, cd);
	struct txx9_tmr_reg __iomem *tmrptr = txx9_cd->tmrptr;

	txx9tmr_stop_and_clear(tmrptr);
	/* start timer */
	__raw_writel(delta, &tmrptr->cpra);
	__raw_writel(TCR_BASE | TXx9_TMTCR_TCE, &tmrptr->tcr);
	return 0;
}

static struct txx9_clock_event_device txx9_clock_event_device = {
	.cd = {
		.name			= "TXx9",
		.features		= CLOCK_EVT_FEAT_PERIODIC |
					  CLOCK_EVT_FEAT_ONESHOT,
		.rating			= 200,
		.set_state_shutdown	= txx9tmr_set_state_shutdown,
		.set_state_periodic	= txx9tmr_set_state_periodic,
		.set_state_oneshot	= txx9tmr_set_state_oneshot,
		.tick_resume		= txx9tmr_tick_resume,
		.set_next_event		= txx9tmr_set_next_event,
	},
};

static irqreturn_t txx9tmr_interrupt(int irq, void *dev_id)
{
	struct txx9_clock_event_device *txx9_cd = dev_id;
	struct clock_event_device *cd = &txx9_cd->cd;
	struct txx9_tmr_reg __iomem *tmrptr = txx9_cd->tmrptr;

	__raw_writel(0, &tmrptr->tisr); /* ack interrupt */
	cd->event_handler(cd);
	return IRQ_HANDLED;
}

void __init txx9_clockevent_init(unsigned long baseaddr, int irq,
				 unsigned int imbusclk)
{
	struct clock_event_device *cd = &txx9_clock_event_device.cd;
	struct txx9_tmr_reg __iomem *tmrptr;

	tmrptr = ioremap(baseaddr, sizeof(struct txx9_tmr_reg));
	txx9tmr_stop_and_clear(tmrptr);
	__raw_writel(TIMER_CCD, &tmrptr->ccdr);
	__raw_writel(0, &tmrptr->itmr);
	txx9_clock_event_device.tmrptr = tmrptr;

	clockevent_set_clock(cd, TIMER_CLK(imbusclk));
	cd->max_delta_ns =
		clockevent_delta2ns(0xffffffff >> (32 - TXX9_TIMER_BITS), cd);
	cd->max_delta_ticks = 0xffffffff >> (32 - TXX9_TIMER_BITS);
	cd->min_delta_ns = clockevent_delta2ns(0xf, cd);
	cd->min_delta_ticks = 0xf;
	cd->irq = irq;
	cd->cpumask = cpumask_of(0);
	clockevents_register_device(cd);
	if (request_irq(irq, txx9tmr_interrupt, IRQF_PERCPU | IRQF_TIMER,
			"txx9tmr", &txx9_clock_event_device))
		pr_err("Failed to request irq %d (txx9tmr)\n", irq);
	printk(KERN_INFO "TXx9: clockevent device at 0x%lx, irq %d\n",
	       baseaddr, irq);
}

void __init txx9_tmr_init(unsigned long baseaddr)
{
	struct txx9_tmr_reg __iomem *tmrptr;

	tmrptr = ioremap(baseaddr, sizeof(struct txx9_tmr_reg));
	/* Start once to make CounterResetEnable effective */
	__raw_writel(TXx9_TMTCR_CRE | TXx9_TMTCR_TCE, &tmrptr->tcr);
	/* Stop and reset the counter */
	__raw_writel(TXx9_TMTCR_CRE, &tmrptr->tcr);
	__raw_writel(0, &tmrptr->tisr);
	__raw_writel(0xffffffff, &tmrptr->cpra);
	__raw_writel(0, &tmrptr->itmr);
	__raw_writel(0, &tmrptr->ccdr);
	__raw_writel(0, &tmrptr->pgmr);
	iounmap(tmrptr);
}
