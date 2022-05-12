// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Integrator/AP timer driver
 * Copyright (C) 2000-2003 Deep Blue Solutions Ltd
 * Copyright (c) 2014, Linaro Limited
 */

#include <linux/clk.h>
#include <linux/clocksource.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/clockchips.h>
#include <linux/interrupt.h>
#include <linux/sched_clock.h>

#include "timer-sp.h"

static void __iomem * sched_clk_base;

static u64 notrace integrator_read_sched_clock(void)
{
	return -readl(sched_clk_base + TIMER_VALUE);
}

static int __init integrator_clocksource_init(unsigned long inrate,
					      void __iomem *base)
{
	u32 ctrl = TIMER_CTRL_ENABLE | TIMER_CTRL_PERIODIC;
	unsigned long rate = inrate;
	int ret;

	if (rate >= 1500000) {
		rate /= 16;
		ctrl |= TIMER_CTRL_DIV16;
	}

	writel(0xffff, base + TIMER_LOAD);
	writel(ctrl, base + TIMER_CTRL);

	ret = clocksource_mmio_init(base + TIMER_VALUE, "timer2",
				    rate, 200, 16, clocksource_mmio_readl_down);
	if (ret)
		return ret;

	sched_clk_base = base;
	sched_clock_register(integrator_read_sched_clock, 16, rate);

	return 0;
}

static unsigned long timer_reload;
static void __iomem * clkevt_base;

/*
 * IRQ handler for the timer
 */
static irqreturn_t integrator_timer_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *evt = dev_id;

	/* clear the interrupt */
	writel(1, clkevt_base + TIMER_INTCLR);

	evt->event_handler(evt);

	return IRQ_HANDLED;
}

static int clkevt_shutdown(struct clock_event_device *evt)
{
	u32 ctrl = readl(clkevt_base + TIMER_CTRL) & ~TIMER_CTRL_ENABLE;

	/* Disable timer */
	writel(ctrl, clkevt_base + TIMER_CTRL);
	return 0;
}

static int clkevt_set_oneshot(struct clock_event_device *evt)
{
	u32 ctrl = readl(clkevt_base + TIMER_CTRL) &
		   ~(TIMER_CTRL_ENABLE | TIMER_CTRL_PERIODIC);

	/* Leave the timer disabled, .set_next_event will enable it */
	writel(ctrl, clkevt_base + TIMER_CTRL);
	return 0;
}

static int clkevt_set_periodic(struct clock_event_device *evt)
{
	u32 ctrl = readl(clkevt_base + TIMER_CTRL) & ~TIMER_CTRL_ENABLE;

	/* Disable timer */
	writel(ctrl, clkevt_base + TIMER_CTRL);

	/* Enable the timer and start the periodic tick */
	writel(timer_reload, clkevt_base + TIMER_LOAD);
	ctrl |= TIMER_CTRL_PERIODIC | TIMER_CTRL_ENABLE;
	writel(ctrl, clkevt_base + TIMER_CTRL);
	return 0;
}

static int clkevt_set_next_event(unsigned long next, struct clock_event_device *evt)
{
	unsigned long ctrl = readl(clkevt_base + TIMER_CTRL);

	writel(ctrl & ~TIMER_CTRL_ENABLE, clkevt_base + TIMER_CTRL);
	writel(next, clkevt_base + TIMER_LOAD);
	writel(ctrl | TIMER_CTRL_ENABLE, clkevt_base + TIMER_CTRL);

	return 0;
}

static struct clock_event_device integrator_clockevent = {
	.name			= "timer1",
	.features		= CLOCK_EVT_FEAT_PERIODIC |
				  CLOCK_EVT_FEAT_ONESHOT,
	.set_state_shutdown	= clkevt_shutdown,
	.set_state_periodic	= clkevt_set_periodic,
	.set_state_oneshot	= clkevt_set_oneshot,
	.tick_resume		= clkevt_shutdown,
	.set_next_event		= clkevt_set_next_event,
	.rating			= 300,
};

static int integrator_clockevent_init(unsigned long inrate,
				      void __iomem *base, int irq)
{
	unsigned long rate = inrate;
	unsigned int ctrl = 0;
	int ret;

	clkevt_base = base;
	/* Calculate and program a divisor */
	if (rate > 0x100000 * HZ) {
		rate /= 256;
		ctrl |= TIMER_CTRL_DIV256;
	} else if (rate > 0x10000 * HZ) {
		rate /= 16;
		ctrl |= TIMER_CTRL_DIV16;
	}
	timer_reload = rate / HZ;
	writel(ctrl, clkevt_base + TIMER_CTRL);

	ret = request_irq(irq, integrator_timer_interrupt,
			  IRQF_TIMER | IRQF_IRQPOLL, "timer",
			  &integrator_clockevent);
	if (ret)
		return ret;

	clockevents_config_and_register(&integrator_clockevent,
					rate,
					1,
					0xffffU);
	return 0;
}

static int __init integrator_ap_timer_init_of(struct device_node *node)
{
	const char *path;
	void __iomem *base;
	int err;
	int irq;
	struct clk *clk;
	unsigned long rate;
	struct device_node *alias_node;

	base = of_io_request_and_map(node, 0, "integrator-timer");
	if (IS_ERR(base))
		return PTR_ERR(base);

	clk = of_clk_get(node, 0);
	if (IS_ERR(clk)) {
		pr_err("No clock for %pOFn\n", node);
		return PTR_ERR(clk);
	}
	clk_prepare_enable(clk);
	rate = clk_get_rate(clk);
	writel(0, base + TIMER_CTRL);

	err = of_property_read_string(of_aliases,
				"arm,timer-primary", &path);
	if (err) {
		pr_warn("Failed to read property\n");
		return err;
	}

	alias_node = of_find_node_by_path(path);

	/*
	 * The pointer is used as an identifier not as a pointer, we
	 * can drop the refcount on the of__node immediately after
	 * getting it.
	 */
	of_node_put(alias_node);

	if (node == alias_node)
		/* The primary timer lacks IRQ, use as clocksource */
		return integrator_clocksource_init(rate, base);

	err = of_property_read_string(of_aliases,
				"arm,timer-secondary", &path);
	if (err) {
		pr_warn("Failed to read property\n");
		return err;
	}

	alias_node = of_find_node_by_path(path);

	of_node_put(alias_node);

	if (node == alias_node) {
		/* The secondary timer will drive the clock event */
		irq = irq_of_parse_and_map(node, 0);
		return integrator_clockevent_init(rate, base, irq);
	}

	pr_info("Timer @%p unused\n", base);
	clk_disable_unprepare(clk);

	return 0;
}

TIMER_OF_DECLARE(integrator_ap_timer, "arm,integrator-timer",
		       integrator_ap_timer_init_of);
