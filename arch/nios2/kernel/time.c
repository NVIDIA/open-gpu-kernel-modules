/*
 * Copyright (C) 2013-2014 Altera Corporation
 * Copyright (C) 2010 Tobias Klauser <tklauser@distanz.ch>
 * Copyright (C) 2004 Microtronix Datacom Ltd.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <linux/export.h>
#include <linux/interrupt.h>
#include <linux/clockchips.h>
#include <linux/clocksource.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/io.h>
#include <linux/slab.h>

#define ALTR_TIMER_COMPATIBLE		"altr,timer-1.0"

#define ALTERA_TIMER_STATUS_REG	0
#define ALTERA_TIMER_CONTROL_REG	4
#define ALTERA_TIMER_PERIODL_REG	8
#define ALTERA_TIMER_PERIODH_REG	12
#define ALTERA_TIMER_SNAPL_REG		16
#define ALTERA_TIMER_SNAPH_REG		20

#define ALTERA_TIMER_CONTROL_ITO_MSK	(0x1)
#define ALTERA_TIMER_CONTROL_CONT_MSK	(0x2)
#define ALTERA_TIMER_CONTROL_START_MSK	(0x4)
#define ALTERA_TIMER_CONTROL_STOP_MSK	(0x8)

struct nios2_timer {
	void __iomem *base;
	unsigned long freq;
};

struct nios2_clockevent_dev {
	struct nios2_timer timer;
	struct clock_event_device ced;
};

struct nios2_clocksource {
	struct nios2_timer timer;
	struct clocksource cs;
};

static inline struct nios2_clockevent_dev *
	to_nios2_clkevent(struct clock_event_device *evt)
{
	return container_of(evt, struct nios2_clockevent_dev, ced);
}

static inline struct nios2_clocksource *
	to_nios2_clksource(struct clocksource *cs)
{
	return container_of(cs, struct nios2_clocksource, cs);
}

static u16 timer_readw(struct nios2_timer *timer, u32 offs)
{
	return readw(timer->base + offs);
}

static void timer_writew(struct nios2_timer *timer, u16 val, u32 offs)
{
	writew(val, timer->base + offs);
}

static inline unsigned long read_timersnapshot(struct nios2_timer *timer)
{
	unsigned long count;

	timer_writew(timer, 0, ALTERA_TIMER_SNAPL_REG);
	count = timer_readw(timer, ALTERA_TIMER_SNAPH_REG) << 16 |
		timer_readw(timer, ALTERA_TIMER_SNAPL_REG);

	return count;
}

static u64 nios2_timer_read(struct clocksource *cs)
{
	struct nios2_clocksource *nios2_cs = to_nios2_clksource(cs);
	unsigned long flags;
	u32 count;

	local_irq_save(flags);
	count = read_timersnapshot(&nios2_cs->timer);
	local_irq_restore(flags);

	/* Counter is counting down */
	return ~count;
}

static struct nios2_clocksource nios2_cs = {
	.cs = {
		.name	= "nios2-clksrc",
		.rating	= 250,
		.read	= nios2_timer_read,
		.mask	= CLOCKSOURCE_MASK(32),
		.flags	= CLOCK_SOURCE_IS_CONTINUOUS,
	},
};

cycles_t get_cycles(void)
{
	/* Only read timer if it has been initialized */
	if (nios2_cs.timer.base)
		return nios2_timer_read(&nios2_cs.cs);
	return 0;
}
EXPORT_SYMBOL(get_cycles);

static void nios2_timer_start(struct nios2_timer *timer)
{
	u16 ctrl;

	ctrl = timer_readw(timer, ALTERA_TIMER_CONTROL_REG);
	ctrl |= ALTERA_TIMER_CONTROL_START_MSK;
	timer_writew(timer, ctrl, ALTERA_TIMER_CONTROL_REG);
}

static void nios2_timer_stop(struct nios2_timer *timer)
{
	u16 ctrl;

	ctrl = timer_readw(timer, ALTERA_TIMER_CONTROL_REG);
	ctrl |= ALTERA_TIMER_CONTROL_STOP_MSK;
	timer_writew(timer, ctrl, ALTERA_TIMER_CONTROL_REG);
}

static void nios2_timer_config(struct nios2_timer *timer, unsigned long period,
			       bool periodic)
{
	u16 ctrl;

	/* The timer's actual period is one cycle greater than the value
	 * stored in the period register. */
	 period--;

	ctrl = timer_readw(timer, ALTERA_TIMER_CONTROL_REG);
	/* stop counter */
	timer_writew(timer, ctrl | ALTERA_TIMER_CONTROL_STOP_MSK,
		ALTERA_TIMER_CONTROL_REG);

	/* write new count */
	timer_writew(timer, period, ALTERA_TIMER_PERIODL_REG);
	timer_writew(timer, period >> 16, ALTERA_TIMER_PERIODH_REG);

	ctrl |= ALTERA_TIMER_CONTROL_START_MSK | ALTERA_TIMER_CONTROL_ITO_MSK;
	if (periodic)
		ctrl |= ALTERA_TIMER_CONTROL_CONT_MSK;
	else
		ctrl &= ~ALTERA_TIMER_CONTROL_CONT_MSK;
	timer_writew(timer, ctrl, ALTERA_TIMER_CONTROL_REG);
}

static int nios2_timer_set_next_event(unsigned long delta,
	struct clock_event_device *evt)
{
	struct nios2_clockevent_dev *nios2_ced = to_nios2_clkevent(evt);

	nios2_timer_config(&nios2_ced->timer, delta, false);

	return 0;
}

static int nios2_timer_shutdown(struct clock_event_device *evt)
{
	struct nios2_clockevent_dev *nios2_ced = to_nios2_clkevent(evt);
	struct nios2_timer *timer = &nios2_ced->timer;

	nios2_timer_stop(timer);
	return 0;
}

static int nios2_timer_set_periodic(struct clock_event_device *evt)
{
	unsigned long period;
	struct nios2_clockevent_dev *nios2_ced = to_nios2_clkevent(evt);
	struct nios2_timer *timer = &nios2_ced->timer;

	period = DIV_ROUND_UP(timer->freq, HZ);
	nios2_timer_config(timer, period, true);
	return 0;
}

static int nios2_timer_resume(struct clock_event_device *evt)
{
	struct nios2_clockevent_dev *nios2_ced = to_nios2_clkevent(evt);
	struct nios2_timer *timer = &nios2_ced->timer;

	nios2_timer_start(timer);
	return 0;
}

irqreturn_t timer_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *evt = (struct clock_event_device *) dev_id;
	struct nios2_clockevent_dev *nios2_ced = to_nios2_clkevent(evt);

	/* Clear the interrupt condition */
	timer_writew(&nios2_ced->timer, 0, ALTERA_TIMER_STATUS_REG);
	evt->event_handler(evt);

	return IRQ_HANDLED;
}

static int __init nios2_timer_get_base_and_freq(struct device_node *np,
				void __iomem **base, u32 *freq)
{
	*base = of_iomap(np, 0);
	if (!*base) {
		pr_crit("Unable to map reg for %pOFn\n", np);
		return -ENXIO;
	}

	if (of_property_read_u32(np, "clock-frequency", freq)) {
		pr_crit("Unable to get %pOFn clock frequency\n", np);
		return -EINVAL;
	}

	return 0;
}

static struct nios2_clockevent_dev nios2_ce = {
	.ced = {
		.name = "nios2-clkevent",
		.features = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
		.rating = 250,
		.shift = 32,
		.set_next_event = nios2_timer_set_next_event,
		.set_state_shutdown = nios2_timer_shutdown,
		.set_state_periodic = nios2_timer_set_periodic,
		.set_state_oneshot = nios2_timer_shutdown,
		.tick_resume = nios2_timer_resume,
	},
};

static __init int nios2_clockevent_init(struct device_node *timer)
{
	void __iomem *iobase;
	u32 freq;
	int irq, ret;

	ret = nios2_timer_get_base_and_freq(timer, &iobase, &freq);
	if (ret)
		return ret;

	irq = irq_of_parse_and_map(timer, 0);
	if (!irq) {
		pr_crit("Unable to parse timer irq\n");
		return -EINVAL;
	}

	nios2_ce.timer.base = iobase;
	nios2_ce.timer.freq = freq;

	nios2_ce.ced.cpumask = cpumask_of(0);
	nios2_ce.ced.irq = irq;

	nios2_timer_stop(&nios2_ce.timer);
	/* clear pending interrupt */
	timer_writew(&nios2_ce.timer, 0, ALTERA_TIMER_STATUS_REG);

	ret = request_irq(irq, timer_interrupt, IRQF_TIMER, timer->name,
			  &nios2_ce.ced);
	if (ret) {
		pr_crit("Unable to setup timer irq\n");
		return ret;
	}

	clockevents_config_and_register(&nios2_ce.ced, freq, 1, ULONG_MAX);

	return 0;
}

static __init int nios2_clocksource_init(struct device_node *timer)
{
	unsigned int ctrl;
	void __iomem *iobase;
	u32 freq;
	int ret;

	ret = nios2_timer_get_base_and_freq(timer, &iobase, &freq);
	if (ret)
		return ret;

	nios2_cs.timer.base = iobase;
	nios2_cs.timer.freq = freq;

	ret = clocksource_register_hz(&nios2_cs.cs, freq);
	if (ret)
		return ret;

	timer_writew(&nios2_cs.timer, USHRT_MAX, ALTERA_TIMER_PERIODL_REG);
	timer_writew(&nios2_cs.timer, USHRT_MAX, ALTERA_TIMER_PERIODH_REG);

	/* interrupt disable + continuous + start */
	ctrl = ALTERA_TIMER_CONTROL_CONT_MSK | ALTERA_TIMER_CONTROL_START_MSK;
	timer_writew(&nios2_cs.timer, ctrl, ALTERA_TIMER_CONTROL_REG);

	/* Calibrate the delay loop directly */
	lpj_fine = freq / HZ;

	return 0;
}

/*
 * The first timer instance will use as a clockevent. If there are two or
 * more instances, the second one gets used as clocksource and all
 * others are unused.
*/
static int __init nios2_time_init(struct device_node *timer)
{
	static int num_called;
	int ret;

	switch (num_called) {
	case 0:
		ret = nios2_clockevent_init(timer);
		break;
	case 1:
		ret = nios2_clocksource_init(timer);
		break;
	default:
		ret = 0;
		break;
	}

	num_called++;

	return ret;
}

void read_persistent_clock64(struct timespec64 *ts)
{
	ts->tv_sec = mktime64(2007, 1, 1, 0, 0, 0);
	ts->tv_nsec = 0;
}

void __init time_init(void)
{
	struct device_node *np;
	int count = 0;

	for_each_compatible_node(np, NULL,  ALTR_TIMER_COMPATIBLE)
		count++;

	if (count < 2)
		panic("%d timer is found, it needs 2 timers in system\n", count);

	timer_probe();
}

TIMER_OF_DECLARE(nios2_timer, ALTR_TIMER_COMPATIBLE, nios2_time_init);
