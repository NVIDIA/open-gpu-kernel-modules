// SPDX-License-Identifier: GPL-2.0-only
/*
 * Time related functions for Hexagon architecture
 *
 * Copyright (c) 2010-2011, The Linux Foundation. All rights reserved.
 */

#include <linux/init.h>
#include <linux/clockchips.h>
#include <linux/clocksource.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/module.h>

#include <asm/timer-regs.h>
#include <asm/hexagon_vm.h>

/*
 * For the clocksource we need:
 *	pcycle frequency (600MHz)
 * For the loops_per_jiffy we need:
 *	thread/cpu frequency (100MHz)
 * And for the timer, we need:
 *	sleep clock rate
 */

cycles_t	pcycle_freq_mhz;
cycles_t	thread_freq_mhz;
cycles_t	sleep_clk_freq;

static struct resource rtos_timer_resources[] = {
	{
		.start	= RTOS_TIMER_REGS_ADDR,
		.end	= RTOS_TIMER_REGS_ADDR+PAGE_SIZE-1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device rtos_timer_device = {
	.name		= "rtos_timer",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(rtos_timer_resources),
	.resource	= rtos_timer_resources,
};

/*  A lot of this stuff should move into a platform specific section.  */
struct adsp_hw_timer_struct {
	u32 match;   /*  Match value  */
	u32 count;
	u32 enable;  /*  [1] - CLR_ON_MATCH_EN, [0] - EN  */
	u32 clear;   /*  one-shot register that clears the count  */
};

/*  Look for "TCX0" for related constants.  */
static __iomem struct adsp_hw_timer_struct *rtos_timer;

static u64 timer_get_cycles(struct clocksource *cs)
{
	return (u64) __vmgettime();
}

static struct clocksource hexagon_clocksource = {
	.name		= "pcycles",
	.rating		= 250,
	.read		= timer_get_cycles,
	.mask		= CLOCKSOURCE_MASK(64),
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
};

static int set_next_event(unsigned long delta, struct clock_event_device *evt)
{
	/*  Assuming the timer will be disabled when we enter here.  */

	iowrite32(1, &rtos_timer->clear);
	iowrite32(0, &rtos_timer->clear);

	iowrite32(delta, &rtos_timer->match);
	iowrite32(1 << TIMER_ENABLE, &rtos_timer->enable);
	return 0;
}

#ifdef CONFIG_SMP
/*  Broadcast mechanism  */
static void broadcast(const struct cpumask *mask)
{
	send_ipi(mask, IPI_TIMER);
}
#endif

/* XXX Implement set_state_shutdown() */
static struct clock_event_device hexagon_clockevent_dev = {
	.name		= "clockevent",
	.features	= CLOCK_EVT_FEAT_ONESHOT,
	.rating		= 400,
	.irq		= RTOS_TIMER_INT,
	.set_next_event = set_next_event,
#ifdef CONFIG_SMP
	.broadcast	= broadcast,
#endif
};

#ifdef CONFIG_SMP
static DEFINE_PER_CPU(struct clock_event_device, clock_events);

void setup_percpu_clockdev(void)
{
	int cpu = smp_processor_id();
	struct clock_event_device *ce_dev = &hexagon_clockevent_dev;
	struct clock_event_device *dummy_clock_dev =
		&per_cpu(clock_events, cpu);

	memcpy(dummy_clock_dev, ce_dev, sizeof(*dummy_clock_dev));
	INIT_LIST_HEAD(&dummy_clock_dev->list);

	dummy_clock_dev->features = CLOCK_EVT_FEAT_DUMMY;
	dummy_clock_dev->cpumask = cpumask_of(cpu);

	clockevents_register_device(dummy_clock_dev);
}

/*  Called from smp.c for each CPU's timer ipi call  */
void ipi_timer(void)
{
	int cpu = smp_processor_id();
	struct clock_event_device *ce_dev = &per_cpu(clock_events, cpu);

	ce_dev->event_handler(ce_dev);
}
#endif /* CONFIG_SMP */

static irqreturn_t timer_interrupt(int irq, void *devid)
{
	struct clock_event_device *ce_dev = &hexagon_clockevent_dev;

	iowrite32(0, &rtos_timer->enable);
	ce_dev->event_handler(ce_dev);

	return IRQ_HANDLED;
}

/*
 * time_init_deferred - called by start_kernel to set up timer/clock source
 *
 * Install the IRQ handler for the clock, setup timers.
 * This is done late, as that way, we can use ioremap().
 *
 * This runs just before the delay loop is calibrated, and
 * is used for delay calibration.
 */
void __init time_init_deferred(void)
{
	struct resource *resource = NULL;
	struct clock_event_device *ce_dev = &hexagon_clockevent_dev;
	unsigned long flag = IRQF_TIMER | IRQF_TRIGGER_RISING;

	ce_dev->cpumask = cpu_all_mask;

	if (!resource)
		resource = rtos_timer_device.resource;

	/*  ioremap here means this has to run later, after paging init  */
	rtos_timer = ioremap(resource->start, resource_size(resource));

	if (!rtos_timer) {
		release_mem_region(resource->start, resource_size(resource));
	}
	clocksource_register_khz(&hexagon_clocksource, pcycle_freq_mhz * 1000);

	/*  Note: the sim generic RTOS clock is apparently really 18750Hz  */

	/*
	 * Last arg is some guaranteed seconds for which the conversion will
	 * work without overflow.
	 */
	clockevents_calc_mult_shift(ce_dev, sleep_clk_freq, 4);

	ce_dev->max_delta_ns = clockevent_delta2ns(0x7fffffff, ce_dev);
	ce_dev->max_delta_ticks = 0x7fffffff;
	ce_dev->min_delta_ns = clockevent_delta2ns(0xf, ce_dev);
	ce_dev->min_delta_ticks = 0xf;

#ifdef CONFIG_SMP
	setup_percpu_clockdev();
#endif

	clockevents_register_device(ce_dev);
	if (request_irq(ce_dev->irq, timer_interrupt, flag, "rtos_timer", NULL))
		pr_err("Failed to register rtos_timer interrupt\n");
}

void __init time_init(void)
{
	late_time_init = time_init_deferred;
}

void __delay(unsigned long cycles)
{
	unsigned long long start = __vmgettime();

	while ((__vmgettime() - start) < cycles)
		cpu_relax();
}
EXPORT_SYMBOL(__delay);

/*
 * This could become parametric or perhaps even computed at run-time,
 * but for now we take the observed simulator jitter.
 */
static long long fudgefactor = 350;  /* Maybe lower if kernel optimized. */

void __udelay(unsigned long usecs)
{
	unsigned long long start = __vmgettime();
	unsigned long long finish = (pcycle_freq_mhz * usecs) - fudgefactor;

	while ((__vmgettime() - start) < finish)
		cpu_relax(); /*  not sure how this improves readability  */
}
EXPORT_SYMBOL(__udelay);
