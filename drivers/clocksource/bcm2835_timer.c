// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2012 Simon Arlott
 */

#include <linux/bitops.h>
#include <linux/clockchips.h>
#include <linux/clocksource.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/sched_clock.h>

#include <asm/irq.h>

#define REG_CONTROL	0x00
#define REG_COUNTER_LO	0x04
#define REG_COUNTER_HI	0x08
#define REG_COMPARE(n)	(0x0c + (n) * 4)
#define MAX_TIMER	3
#define DEFAULT_TIMER	3

struct bcm2835_timer {
	void __iomem *control;
	void __iomem *compare;
	int match_mask;
	struct clock_event_device evt;
};

static void __iomem *system_clock __read_mostly;

static u64 notrace bcm2835_sched_read(void)
{
	return readl_relaxed(system_clock);
}

static int bcm2835_time_set_next_event(unsigned long event,
	struct clock_event_device *evt_dev)
{
	struct bcm2835_timer *timer = container_of(evt_dev,
		struct bcm2835_timer, evt);
	writel_relaxed(readl_relaxed(system_clock) + event,
		timer->compare);
	return 0;
}

static irqreturn_t bcm2835_time_interrupt(int irq, void *dev_id)
{
	struct bcm2835_timer *timer = dev_id;
	void (*event_handler)(struct clock_event_device *);
	if (readl_relaxed(timer->control) & timer->match_mask) {
		writel_relaxed(timer->match_mask, timer->control);

		event_handler = READ_ONCE(timer->evt.event_handler);
		if (event_handler)
			event_handler(&timer->evt);
		return IRQ_HANDLED;
	} else {
		return IRQ_NONE;
	}
}

static int __init bcm2835_timer_init(struct device_node *node)
{
	void __iomem *base;
	u32 freq;
	int irq, ret;
	struct bcm2835_timer *timer;

	base = of_iomap(node, 0);
	if (!base) {
		pr_err("Can't remap registers\n");
		return -ENXIO;
	}

	ret = of_property_read_u32(node, "clock-frequency", &freq);
	if (ret) {
		pr_err("Can't read clock-frequency\n");
		goto err_iounmap;
	}

	system_clock = base + REG_COUNTER_LO;
	sched_clock_register(bcm2835_sched_read, 32, freq);

	clocksource_mmio_init(base + REG_COUNTER_LO, node->name,
		freq, 300, 32, clocksource_mmio_readl_up);

	irq = irq_of_parse_and_map(node, DEFAULT_TIMER);
	if (irq <= 0) {
		pr_err("Can't parse IRQ\n");
		ret = -EINVAL;
		goto err_iounmap;
	}

	timer = kzalloc(sizeof(*timer), GFP_KERNEL);
	if (!timer) {
		ret = -ENOMEM;
		goto err_iounmap;
	}

	timer->control = base + REG_CONTROL;
	timer->compare = base + REG_COMPARE(DEFAULT_TIMER);
	timer->match_mask = BIT(DEFAULT_TIMER);
	timer->evt.name = node->name;
	timer->evt.rating = 300;
	timer->evt.features = CLOCK_EVT_FEAT_ONESHOT;
	timer->evt.set_next_event = bcm2835_time_set_next_event;
	timer->evt.cpumask = cpumask_of(0);

	ret = request_irq(irq, bcm2835_time_interrupt, IRQF_TIMER | IRQF_SHARED,
			  node->name, timer);
	if (ret) {
		pr_err("Can't set up timer IRQ\n");
		goto err_timer_free;
	}

	clockevents_config_and_register(&timer->evt, freq, 0xf, 0xffffffff);

	pr_info("bcm2835: system timer (irq = %d)\n", irq);

	return 0;

err_timer_free:
	kfree(timer);

err_iounmap:
	iounmap(base);
	return ret;
}
TIMER_OF_DECLARE(bcm2835, "brcm,bcm2835-system-timer",
			bcm2835_timer_init);
