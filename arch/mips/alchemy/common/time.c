// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2008-2009 Manuel Lauss <manuel.lauss@gmail.com>
 *
 * Previous incarnations were:
 * Copyright (C) 2001, 2006, 2008 MontaVista Software, <source@mvista.com>
 * Copied and modified Carsten Langgaard's time.c
 *
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
 *
 * ########################################################################
 *
 * ########################################################################
 *
 * Clocksource/event using the 32.768kHz-clocked Counter1 ('RTC' in the
 * databooks).  Firmware/Board init code must enable the counters in the
 * counter control register, otherwise the CP0 counter clocksource/event
 * will be installed instead (and use of 'wait' instruction is prohibited).
 */

#include <linux/clockchips.h>
#include <linux/clocksource.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>

#include <asm/idle.h>
#include <asm/processor.h>
#include <asm/time.h>
#include <asm/mach-au1x00/au1000.h>

/* 32kHz clock enabled and detected */
#define CNTR_OK (SYS_CNTRL_E0 | SYS_CNTRL_32S)

static u64 au1x_counter1_read(struct clocksource *cs)
{
	return alchemy_rdsys(AU1000_SYS_RTCREAD);
}

static struct clocksource au1x_counter1_clocksource = {
	.name		= "alchemy-counter1",
	.read		= au1x_counter1_read,
	.mask		= CLOCKSOURCE_MASK(32),
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
	.rating		= 1500,
};

static int au1x_rtcmatch2_set_next_event(unsigned long delta,
					 struct clock_event_device *cd)
{
	delta += alchemy_rdsys(AU1000_SYS_RTCREAD);
	/* wait for register access */
	while (alchemy_rdsys(AU1000_SYS_CNTRCTRL) & SYS_CNTRL_M21)
		;
	alchemy_wrsys(delta, AU1000_SYS_RTCMATCH2);

	return 0;
}

static irqreturn_t au1x_rtcmatch2_irq(int irq, void *dev_id)
{
	struct clock_event_device *cd = dev_id;
	cd->event_handler(cd);
	return IRQ_HANDLED;
}

static struct clock_event_device au1x_rtcmatch2_clockdev = {
	.name		= "rtcmatch2",
	.features	= CLOCK_EVT_FEAT_ONESHOT,
	.rating		= 1500,
	.set_next_event = au1x_rtcmatch2_set_next_event,
	.cpumask	= cpu_possible_mask,
};

static int __init alchemy_time_init(unsigned int m2int)
{
	struct clock_event_device *cd = &au1x_rtcmatch2_clockdev;
	unsigned long t;

	au1x_rtcmatch2_clockdev.irq = m2int;

	/* Check if firmware (YAMON, ...) has enabled 32kHz and clock
	 * has been detected.  If so install the rtcmatch2 clocksource,
	 * otherwise don't bother.  Note that both bits being set is by
	 * no means a definite guarantee that the counters actually work
	 * (the 32S bit seems to be stuck set to 1 once a single clock-
	 * edge is detected, hence the timeouts).
	 */
	if (CNTR_OK != (alchemy_rdsys(AU1000_SYS_CNTRCTRL) & CNTR_OK))
		goto cntr_err;

	/*
	 * setup counter 1 (RTC) to tick at full speed
	 */
	t = 0xffffff;
	while ((alchemy_rdsys(AU1000_SYS_CNTRCTRL) & SYS_CNTRL_T1S) && --t)
		asm volatile ("nop");
	if (!t)
		goto cntr_err;

	alchemy_wrsys(0, AU1000_SYS_RTCTRIM);	/* 32.768 kHz */

	t = 0xffffff;
	while ((alchemy_rdsys(AU1000_SYS_CNTRCTRL) & SYS_CNTRL_C1S) && --t)
		asm volatile ("nop");
	if (!t)
		goto cntr_err;
	alchemy_wrsys(0, AU1000_SYS_RTCWRITE);

	t = 0xffffff;
	while ((alchemy_rdsys(AU1000_SYS_CNTRCTRL) & SYS_CNTRL_C1S) && --t)
		asm volatile ("nop");
	if (!t)
		goto cntr_err;

	/* register counter1 clocksource and event device */
	clocksource_register_hz(&au1x_counter1_clocksource, 32768);

	cd->shift = 32;
	cd->mult = div_sc(32768, NSEC_PER_SEC, cd->shift);
	cd->max_delta_ns = clockevent_delta2ns(0xffffffff, cd);
	cd->max_delta_ticks = 0xffffffff;
	cd->min_delta_ns = clockevent_delta2ns(9, cd);
	cd->min_delta_ticks = 9;	/* ~0.28ms */
	clockevents_register_device(cd);
	if (request_irq(m2int, au1x_rtcmatch2_irq, IRQF_TIMER, "timer",
			&au1x_rtcmatch2_clockdev))
		pr_err("Failed to register timer interrupt\n");

	printk(KERN_INFO "Alchemy clocksource installed\n");

	return 0;

cntr_err:
	return -1;
}

static int alchemy_m2inttab[] __initdata = {
	AU1000_RTC_MATCH2_INT,
	AU1500_RTC_MATCH2_INT,
	AU1100_RTC_MATCH2_INT,
	AU1550_RTC_MATCH2_INT,
	AU1200_RTC_MATCH2_INT,
	AU1300_RTC_MATCH2_INT,
};

void __init plat_time_init(void)
{
	int t;

	t = alchemy_get_cputype();
	if (t == ALCHEMY_CPU_UNKNOWN ||
	    alchemy_time_init(alchemy_m2inttab[t]))
		cpu_wait = NULL;	/* wait doesn't work with r4k timer */
}
