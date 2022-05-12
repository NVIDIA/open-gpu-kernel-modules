// SPDX-License-Identifier: GPL-2.0
/*
 * SuperH Timer Support - CMT
 *
 *  Copyright (C) 2008 Magnus Damm
 */

#include <linux/clk.h>
#include <linux/clockchips.h>
#include <linux/clocksource.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/irq.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/pm_domain.h>
#include <linux/pm_runtime.h>
#include <linux/sh_timer.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#ifdef CONFIG_SUPERH
#include <asm/platform_early.h>
#endif

struct sh_cmt_device;

/*
 * The CMT comes in 5 different identified flavours, depending not only on the
 * SoC but also on the particular instance. The following table lists the main
 * characteristics of those flavours.
 *
 *			16B	32B	32B-F	48B	R-Car Gen2
 * -----------------------------------------------------------------------------
 * Channels		2	1/4	1	6	2/8
 * Control Width	16	16	16	16	32
 * Counter Width	16	32	32	32/48	32/48
 * Shared Start/Stop	Y	Y	Y	Y	N
 *
 * The r8a73a4 / R-Car Gen2 version has a per-channel start/stop register
 * located in the channel registers block. All other versions have a shared
 * start/stop register located in the global space.
 *
 * Channels are indexed from 0 to N-1 in the documentation. The channel index
 * infers the start/stop bit position in the control register and the channel
 * registers block address. Some CMT instances have a subset of channels
 * available, in which case the index in the documentation doesn't match the
 * "real" index as implemented in hardware. This is for instance the case with
 * CMT0 on r8a7740, which is a 32-bit variant with a single channel numbered 0
 * in the documentation but using start/stop bit 5 and having its registers
 * block at 0x60.
 *
 * Similarly CMT0 on r8a73a4, r8a7790 and r8a7791, while implementing 32-bit
 * channels only, is a 48-bit gen2 CMT with the 48-bit channels unavailable.
 */

enum sh_cmt_model {
	SH_CMT_16BIT,
	SH_CMT_32BIT,
	SH_CMT_48BIT,
	SH_CMT0_RCAR_GEN2,
	SH_CMT1_RCAR_GEN2,
};

struct sh_cmt_info {
	enum sh_cmt_model model;

	unsigned int channels_mask;

	unsigned long width; /* 16 or 32 bit version of hardware block */
	u32 overflow_bit;
	u32 clear_bits;

	/* callbacks for CMSTR and CMCSR access */
	u32 (*read_control)(void __iomem *base, unsigned long offs);
	void (*write_control)(void __iomem *base, unsigned long offs,
			      u32 value);

	/* callbacks for CMCNT and CMCOR access */
	u32 (*read_count)(void __iomem *base, unsigned long offs);
	void (*write_count)(void __iomem *base, unsigned long offs, u32 value);
};

struct sh_cmt_channel {
	struct sh_cmt_device *cmt;

	unsigned int index;	/* Index in the documentation */
	unsigned int hwidx;	/* Real hardware index */

	void __iomem *iostart;
	void __iomem *ioctrl;

	unsigned int timer_bit;
	unsigned long flags;
	u32 match_value;
	u32 next_match_value;
	u32 max_match_value;
	raw_spinlock_t lock;
	struct clock_event_device ced;
	struct clocksource cs;
	u64 total_cycles;
	bool cs_enabled;
};

struct sh_cmt_device {
	struct platform_device *pdev;

	const struct sh_cmt_info *info;

	void __iomem *mapbase;
	struct clk *clk;
	unsigned long rate;

	raw_spinlock_t lock; /* Protect the shared start/stop register */

	struct sh_cmt_channel *channels;
	unsigned int num_channels;
	unsigned int hw_channels;

	bool has_clockevent;
	bool has_clocksource;
};

#define SH_CMT16_CMCSR_CMF		(1 << 7)
#define SH_CMT16_CMCSR_CMIE		(1 << 6)
#define SH_CMT16_CMCSR_CKS8		(0 << 0)
#define SH_CMT16_CMCSR_CKS32		(1 << 0)
#define SH_CMT16_CMCSR_CKS128		(2 << 0)
#define SH_CMT16_CMCSR_CKS512		(3 << 0)
#define SH_CMT16_CMCSR_CKS_MASK		(3 << 0)

#define SH_CMT32_CMCSR_CMF		(1 << 15)
#define SH_CMT32_CMCSR_OVF		(1 << 14)
#define SH_CMT32_CMCSR_WRFLG		(1 << 13)
#define SH_CMT32_CMCSR_STTF		(1 << 12)
#define SH_CMT32_CMCSR_STPF		(1 << 11)
#define SH_CMT32_CMCSR_SSIE		(1 << 10)
#define SH_CMT32_CMCSR_CMS		(1 << 9)
#define SH_CMT32_CMCSR_CMM		(1 << 8)
#define SH_CMT32_CMCSR_CMTOUT_IE	(1 << 7)
#define SH_CMT32_CMCSR_CMR_NONE		(0 << 4)
#define SH_CMT32_CMCSR_CMR_DMA		(1 << 4)
#define SH_CMT32_CMCSR_CMR_IRQ		(2 << 4)
#define SH_CMT32_CMCSR_CMR_MASK		(3 << 4)
#define SH_CMT32_CMCSR_DBGIVD		(1 << 3)
#define SH_CMT32_CMCSR_CKS_RCLK8	(4 << 0)
#define SH_CMT32_CMCSR_CKS_RCLK32	(5 << 0)
#define SH_CMT32_CMCSR_CKS_RCLK128	(6 << 0)
#define SH_CMT32_CMCSR_CKS_RCLK1	(7 << 0)
#define SH_CMT32_CMCSR_CKS_MASK		(7 << 0)

static u32 sh_cmt_read16(void __iomem *base, unsigned long offs)
{
	return ioread16(base + (offs << 1));
}

static u32 sh_cmt_read32(void __iomem *base, unsigned long offs)
{
	return ioread32(base + (offs << 2));
}

static void sh_cmt_write16(void __iomem *base, unsigned long offs, u32 value)
{
	iowrite16(value, base + (offs << 1));
}

static void sh_cmt_write32(void __iomem *base, unsigned long offs, u32 value)
{
	iowrite32(value, base + (offs << 2));
}

static const struct sh_cmt_info sh_cmt_info[] = {
	[SH_CMT_16BIT] = {
		.model = SH_CMT_16BIT,
		.width = 16,
		.overflow_bit = SH_CMT16_CMCSR_CMF,
		.clear_bits = ~SH_CMT16_CMCSR_CMF,
		.read_control = sh_cmt_read16,
		.write_control = sh_cmt_write16,
		.read_count = sh_cmt_read16,
		.write_count = sh_cmt_write16,
	},
	[SH_CMT_32BIT] = {
		.model = SH_CMT_32BIT,
		.width = 32,
		.overflow_bit = SH_CMT32_CMCSR_CMF,
		.clear_bits = ~(SH_CMT32_CMCSR_CMF | SH_CMT32_CMCSR_OVF),
		.read_control = sh_cmt_read16,
		.write_control = sh_cmt_write16,
		.read_count = sh_cmt_read32,
		.write_count = sh_cmt_write32,
	},
	[SH_CMT_48BIT] = {
		.model = SH_CMT_48BIT,
		.channels_mask = 0x3f,
		.width = 32,
		.overflow_bit = SH_CMT32_CMCSR_CMF,
		.clear_bits = ~(SH_CMT32_CMCSR_CMF | SH_CMT32_CMCSR_OVF),
		.read_control = sh_cmt_read32,
		.write_control = sh_cmt_write32,
		.read_count = sh_cmt_read32,
		.write_count = sh_cmt_write32,
	},
	[SH_CMT0_RCAR_GEN2] = {
		.model = SH_CMT0_RCAR_GEN2,
		.channels_mask = 0x60,
		.width = 32,
		.overflow_bit = SH_CMT32_CMCSR_CMF,
		.clear_bits = ~(SH_CMT32_CMCSR_CMF | SH_CMT32_CMCSR_OVF),
		.read_control = sh_cmt_read32,
		.write_control = sh_cmt_write32,
		.read_count = sh_cmt_read32,
		.write_count = sh_cmt_write32,
	},
	[SH_CMT1_RCAR_GEN2] = {
		.model = SH_CMT1_RCAR_GEN2,
		.channels_mask = 0xff,
		.width = 32,
		.overflow_bit = SH_CMT32_CMCSR_CMF,
		.clear_bits = ~(SH_CMT32_CMCSR_CMF | SH_CMT32_CMCSR_OVF),
		.read_control = sh_cmt_read32,
		.write_control = sh_cmt_write32,
		.read_count = sh_cmt_read32,
		.write_count = sh_cmt_write32,
	},
};

#define CMCSR 0 /* channel register */
#define CMCNT 1 /* channel register */
#define CMCOR 2 /* channel register */

#define CMCLKE	0x1000	/* CLK Enable Register (R-Car Gen2) */

static inline u32 sh_cmt_read_cmstr(struct sh_cmt_channel *ch)
{
	if (ch->iostart)
		return ch->cmt->info->read_control(ch->iostart, 0);
	else
		return ch->cmt->info->read_control(ch->cmt->mapbase, 0);
}

static inline void sh_cmt_write_cmstr(struct sh_cmt_channel *ch, u32 value)
{
	if (ch->iostart)
		ch->cmt->info->write_control(ch->iostart, 0, value);
	else
		ch->cmt->info->write_control(ch->cmt->mapbase, 0, value);
}

static inline u32 sh_cmt_read_cmcsr(struct sh_cmt_channel *ch)
{
	return ch->cmt->info->read_control(ch->ioctrl, CMCSR);
}

static inline void sh_cmt_write_cmcsr(struct sh_cmt_channel *ch, u32 value)
{
	ch->cmt->info->write_control(ch->ioctrl, CMCSR, value);
}

static inline u32 sh_cmt_read_cmcnt(struct sh_cmt_channel *ch)
{
	return ch->cmt->info->read_count(ch->ioctrl, CMCNT);
}

static inline void sh_cmt_write_cmcnt(struct sh_cmt_channel *ch, u32 value)
{
	ch->cmt->info->write_count(ch->ioctrl, CMCNT, value);
}

static inline void sh_cmt_write_cmcor(struct sh_cmt_channel *ch, u32 value)
{
	ch->cmt->info->write_count(ch->ioctrl, CMCOR, value);
}

static u32 sh_cmt_get_counter(struct sh_cmt_channel *ch, u32 *has_wrapped)
{
	u32 v1, v2, v3;
	u32 o1, o2;

	o1 = sh_cmt_read_cmcsr(ch) & ch->cmt->info->overflow_bit;

	/* Make sure the timer value is stable. Stolen from acpi_pm.c */
	do {
		o2 = o1;
		v1 = sh_cmt_read_cmcnt(ch);
		v2 = sh_cmt_read_cmcnt(ch);
		v3 = sh_cmt_read_cmcnt(ch);
		o1 = sh_cmt_read_cmcsr(ch) & ch->cmt->info->overflow_bit;
	} while (unlikely((o1 != o2) || (v1 > v2 && v1 < v3)
			  || (v2 > v3 && v2 < v1) || (v3 > v1 && v3 < v2)));

	*has_wrapped = o1;
	return v2;
}

static void sh_cmt_start_stop_ch(struct sh_cmt_channel *ch, int start)
{
	unsigned long flags;
	u32 value;

	/* start stop register shared by multiple timer channels */
	raw_spin_lock_irqsave(&ch->cmt->lock, flags);
	value = sh_cmt_read_cmstr(ch);

	if (start)
		value |= 1 << ch->timer_bit;
	else
		value &= ~(1 << ch->timer_bit);

	sh_cmt_write_cmstr(ch, value);
	raw_spin_unlock_irqrestore(&ch->cmt->lock, flags);
}

static int sh_cmt_enable(struct sh_cmt_channel *ch)
{
	int k, ret;

	dev_pm_syscore_device(&ch->cmt->pdev->dev, true);

	/* enable clock */
	ret = clk_enable(ch->cmt->clk);
	if (ret) {
		dev_err(&ch->cmt->pdev->dev, "ch%u: cannot enable clock\n",
			ch->index);
		goto err0;
	}

	/* make sure channel is disabled */
	sh_cmt_start_stop_ch(ch, 0);

	/* configure channel, periodic mode and maximum timeout */
	if (ch->cmt->info->width == 16) {
		sh_cmt_write_cmcsr(ch, SH_CMT16_CMCSR_CMIE |
				   SH_CMT16_CMCSR_CKS512);
	} else {
		u32 cmtout = ch->cmt->info->model <= SH_CMT_48BIT ?
			      SH_CMT32_CMCSR_CMTOUT_IE : 0;
		sh_cmt_write_cmcsr(ch, cmtout | SH_CMT32_CMCSR_CMM |
				   SH_CMT32_CMCSR_CMR_IRQ |
				   SH_CMT32_CMCSR_CKS_RCLK8);
	}

	sh_cmt_write_cmcor(ch, 0xffffffff);
	sh_cmt_write_cmcnt(ch, 0);

	/*
	 * According to the sh73a0 user's manual, as CMCNT can be operated
	 * only by the RCLK (Pseudo 32 kHz), there's one restriction on
	 * modifying CMCNT register; two RCLK cycles are necessary before
	 * this register is either read or any modification of the value
	 * it holds is reflected in the LSI's actual operation.
	 *
	 * While at it, we're supposed to clear out the CMCNT as of this
	 * moment, so make sure it's processed properly here.  This will
	 * take RCLKx2 at maximum.
	 */
	for (k = 0; k < 100; k++) {
		if (!sh_cmt_read_cmcnt(ch))
			break;
		udelay(1);
	}

	if (sh_cmt_read_cmcnt(ch)) {
		dev_err(&ch->cmt->pdev->dev, "ch%u: cannot clear CMCNT\n",
			ch->index);
		ret = -ETIMEDOUT;
		goto err1;
	}

	/* enable channel */
	sh_cmt_start_stop_ch(ch, 1);
	return 0;
 err1:
	/* stop clock */
	clk_disable(ch->cmt->clk);

 err0:
	return ret;
}

static void sh_cmt_disable(struct sh_cmt_channel *ch)
{
	/* disable channel */
	sh_cmt_start_stop_ch(ch, 0);

	/* disable interrupts in CMT block */
	sh_cmt_write_cmcsr(ch, 0);

	/* stop clock */
	clk_disable(ch->cmt->clk);

	dev_pm_syscore_device(&ch->cmt->pdev->dev, false);
}

/* private flags */
#define FLAG_CLOCKEVENT (1 << 0)
#define FLAG_CLOCKSOURCE (1 << 1)
#define FLAG_REPROGRAM (1 << 2)
#define FLAG_SKIPEVENT (1 << 3)
#define FLAG_IRQCONTEXT (1 << 4)

static void sh_cmt_clock_event_program_verify(struct sh_cmt_channel *ch,
					      int absolute)
{
	u32 value = ch->next_match_value;
	u32 new_match;
	u32 delay = 0;
	u32 now = 0;
	u32 has_wrapped;

	now = sh_cmt_get_counter(ch, &has_wrapped);
	ch->flags |= FLAG_REPROGRAM; /* force reprogram */

	if (has_wrapped) {
		/* we're competing with the interrupt handler.
		 *  -> let the interrupt handler reprogram the timer.
		 *  -> interrupt number two handles the event.
		 */
		ch->flags |= FLAG_SKIPEVENT;
		return;
	}

	if (absolute)
		now = 0;

	do {
		/* reprogram the timer hardware,
		 * but don't save the new match value yet.
		 */
		new_match = now + value + delay;
		if (new_match > ch->max_match_value)
			new_match = ch->max_match_value;

		sh_cmt_write_cmcor(ch, new_match);

		now = sh_cmt_get_counter(ch, &has_wrapped);
		if (has_wrapped && (new_match > ch->match_value)) {
			/* we are changing to a greater match value,
			 * so this wrap must be caused by the counter
			 * matching the old value.
			 * -> first interrupt reprograms the timer.
			 * -> interrupt number two handles the event.
			 */
			ch->flags |= FLAG_SKIPEVENT;
			break;
		}

		if (has_wrapped) {
			/* we are changing to a smaller match value,
			 * so the wrap must be caused by the counter
			 * matching the new value.
			 * -> save programmed match value.
			 * -> let isr handle the event.
			 */
			ch->match_value = new_match;
			break;
		}

		/* be safe: verify hardware settings */
		if (now < new_match) {
			/* timer value is below match value, all good.
			 * this makes sure we won't miss any match events.
			 * -> save programmed match value.
			 * -> let isr handle the event.
			 */
			ch->match_value = new_match;
			break;
		}

		/* the counter has reached a value greater
		 * than our new match value. and since the
		 * has_wrapped flag isn't set we must have
		 * programmed a too close event.
		 * -> increase delay and retry.
		 */
		if (delay)
			delay <<= 1;
		else
			delay = 1;

		if (!delay)
			dev_warn(&ch->cmt->pdev->dev, "ch%u: too long delay\n",
				 ch->index);

	} while (delay);
}

static void __sh_cmt_set_next(struct sh_cmt_channel *ch, unsigned long delta)
{
	if (delta > ch->max_match_value)
		dev_warn(&ch->cmt->pdev->dev, "ch%u: delta out of range\n",
			 ch->index);

	ch->next_match_value = delta;
	sh_cmt_clock_event_program_verify(ch, 0);
}

static void sh_cmt_set_next(struct sh_cmt_channel *ch, unsigned long delta)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&ch->lock, flags);
	__sh_cmt_set_next(ch, delta);
	raw_spin_unlock_irqrestore(&ch->lock, flags);
}

static irqreturn_t sh_cmt_interrupt(int irq, void *dev_id)
{
	struct sh_cmt_channel *ch = dev_id;

	/* clear flags */
	sh_cmt_write_cmcsr(ch, sh_cmt_read_cmcsr(ch) &
			   ch->cmt->info->clear_bits);

	/* update clock source counter to begin with if enabled
	 * the wrap flag should be cleared by the timer specific
	 * isr before we end up here.
	 */
	if (ch->flags & FLAG_CLOCKSOURCE)
		ch->total_cycles += ch->match_value + 1;

	if (!(ch->flags & FLAG_REPROGRAM))
		ch->next_match_value = ch->max_match_value;

	ch->flags |= FLAG_IRQCONTEXT;

	if (ch->flags & FLAG_CLOCKEVENT) {
		if (!(ch->flags & FLAG_SKIPEVENT)) {
			if (clockevent_state_oneshot(&ch->ced)) {
				ch->next_match_value = ch->max_match_value;
				ch->flags |= FLAG_REPROGRAM;
			}

			ch->ced.event_handler(&ch->ced);
		}
	}

	ch->flags &= ~FLAG_SKIPEVENT;

	if (ch->flags & FLAG_REPROGRAM) {
		ch->flags &= ~FLAG_REPROGRAM;
		sh_cmt_clock_event_program_verify(ch, 1);

		if (ch->flags & FLAG_CLOCKEVENT)
			if ((clockevent_state_shutdown(&ch->ced))
			    || (ch->match_value == ch->next_match_value))
				ch->flags &= ~FLAG_REPROGRAM;
	}

	ch->flags &= ~FLAG_IRQCONTEXT;

	return IRQ_HANDLED;
}

static int sh_cmt_start(struct sh_cmt_channel *ch, unsigned long flag)
{
	int ret = 0;
	unsigned long flags;

	if (flag & FLAG_CLOCKSOURCE)
		pm_runtime_get_sync(&ch->cmt->pdev->dev);

	raw_spin_lock_irqsave(&ch->lock, flags);

	if (!(ch->flags & (FLAG_CLOCKEVENT | FLAG_CLOCKSOURCE))) {
		if (flag & FLAG_CLOCKEVENT)
			pm_runtime_get_sync(&ch->cmt->pdev->dev);
		ret = sh_cmt_enable(ch);
	}

	if (ret)
		goto out;
	ch->flags |= flag;

	/* setup timeout if no clockevent */
	if ((flag == FLAG_CLOCKSOURCE) && (!(ch->flags & FLAG_CLOCKEVENT)))
		__sh_cmt_set_next(ch, ch->max_match_value);
 out:
	raw_spin_unlock_irqrestore(&ch->lock, flags);

	return ret;
}

static void sh_cmt_stop(struct sh_cmt_channel *ch, unsigned long flag)
{
	unsigned long flags;
	unsigned long f;

	raw_spin_lock_irqsave(&ch->lock, flags);

	f = ch->flags & (FLAG_CLOCKEVENT | FLAG_CLOCKSOURCE);
	ch->flags &= ~flag;

	if (f && !(ch->flags & (FLAG_CLOCKEVENT | FLAG_CLOCKSOURCE))) {
		sh_cmt_disable(ch);
		if (flag & FLAG_CLOCKEVENT)
			pm_runtime_put(&ch->cmt->pdev->dev);
	}

	/* adjust the timeout to maximum if only clocksource left */
	if ((flag == FLAG_CLOCKEVENT) && (ch->flags & FLAG_CLOCKSOURCE))
		__sh_cmt_set_next(ch, ch->max_match_value);

	raw_spin_unlock_irqrestore(&ch->lock, flags);

	if (flag & FLAG_CLOCKSOURCE)
		pm_runtime_put(&ch->cmt->pdev->dev);
}

static struct sh_cmt_channel *cs_to_sh_cmt(struct clocksource *cs)
{
	return container_of(cs, struct sh_cmt_channel, cs);
}

static u64 sh_cmt_clocksource_read(struct clocksource *cs)
{
	struct sh_cmt_channel *ch = cs_to_sh_cmt(cs);
	unsigned long flags;
	u32 has_wrapped;
	u64 value;
	u32 raw;

	raw_spin_lock_irqsave(&ch->lock, flags);
	value = ch->total_cycles;
	raw = sh_cmt_get_counter(ch, &has_wrapped);

	if (unlikely(has_wrapped))
		raw += ch->match_value + 1;
	raw_spin_unlock_irqrestore(&ch->lock, flags);

	return value + raw;
}

static int sh_cmt_clocksource_enable(struct clocksource *cs)
{
	int ret;
	struct sh_cmt_channel *ch = cs_to_sh_cmt(cs);

	WARN_ON(ch->cs_enabled);

	ch->total_cycles = 0;

	ret = sh_cmt_start(ch, FLAG_CLOCKSOURCE);
	if (!ret)
		ch->cs_enabled = true;

	return ret;
}

static void sh_cmt_clocksource_disable(struct clocksource *cs)
{
	struct sh_cmt_channel *ch = cs_to_sh_cmt(cs);

	WARN_ON(!ch->cs_enabled);

	sh_cmt_stop(ch, FLAG_CLOCKSOURCE);
	ch->cs_enabled = false;
}

static void sh_cmt_clocksource_suspend(struct clocksource *cs)
{
	struct sh_cmt_channel *ch = cs_to_sh_cmt(cs);

	if (!ch->cs_enabled)
		return;

	sh_cmt_stop(ch, FLAG_CLOCKSOURCE);
	dev_pm_genpd_suspend(&ch->cmt->pdev->dev);
}

static void sh_cmt_clocksource_resume(struct clocksource *cs)
{
	struct sh_cmt_channel *ch = cs_to_sh_cmt(cs);

	if (!ch->cs_enabled)
		return;

	dev_pm_genpd_resume(&ch->cmt->pdev->dev);
	sh_cmt_start(ch, FLAG_CLOCKSOURCE);
}

static int sh_cmt_register_clocksource(struct sh_cmt_channel *ch,
				       const char *name)
{
	struct clocksource *cs = &ch->cs;

	cs->name = name;
	cs->rating = 125;
	cs->read = sh_cmt_clocksource_read;
	cs->enable = sh_cmt_clocksource_enable;
	cs->disable = sh_cmt_clocksource_disable;
	cs->suspend = sh_cmt_clocksource_suspend;
	cs->resume = sh_cmt_clocksource_resume;
	cs->mask = CLOCKSOURCE_MASK(sizeof(u64) * 8);
	cs->flags = CLOCK_SOURCE_IS_CONTINUOUS;

	dev_info(&ch->cmt->pdev->dev, "ch%u: used as clock source\n",
		 ch->index);

	clocksource_register_hz(cs, ch->cmt->rate);
	return 0;
}

static struct sh_cmt_channel *ced_to_sh_cmt(struct clock_event_device *ced)
{
	return container_of(ced, struct sh_cmt_channel, ced);
}

static void sh_cmt_clock_event_start(struct sh_cmt_channel *ch, int periodic)
{
	sh_cmt_start(ch, FLAG_CLOCKEVENT);

	if (periodic)
		sh_cmt_set_next(ch, ((ch->cmt->rate + HZ/2) / HZ) - 1);
	else
		sh_cmt_set_next(ch, ch->max_match_value);
}

static int sh_cmt_clock_event_shutdown(struct clock_event_device *ced)
{
	struct sh_cmt_channel *ch = ced_to_sh_cmt(ced);

	sh_cmt_stop(ch, FLAG_CLOCKEVENT);
	return 0;
}

static int sh_cmt_clock_event_set_state(struct clock_event_device *ced,
					int periodic)
{
	struct sh_cmt_channel *ch = ced_to_sh_cmt(ced);

	/* deal with old setting first */
	if (clockevent_state_oneshot(ced) || clockevent_state_periodic(ced))
		sh_cmt_stop(ch, FLAG_CLOCKEVENT);

	dev_info(&ch->cmt->pdev->dev, "ch%u: used for %s clock events\n",
		 ch->index, periodic ? "periodic" : "oneshot");
	sh_cmt_clock_event_start(ch, periodic);
	return 0;
}

static int sh_cmt_clock_event_set_oneshot(struct clock_event_device *ced)
{
	return sh_cmt_clock_event_set_state(ced, 0);
}

static int sh_cmt_clock_event_set_periodic(struct clock_event_device *ced)
{
	return sh_cmt_clock_event_set_state(ced, 1);
}

static int sh_cmt_clock_event_next(unsigned long delta,
				   struct clock_event_device *ced)
{
	struct sh_cmt_channel *ch = ced_to_sh_cmt(ced);

	BUG_ON(!clockevent_state_oneshot(ced));
	if (likely(ch->flags & FLAG_IRQCONTEXT))
		ch->next_match_value = delta - 1;
	else
		sh_cmt_set_next(ch, delta - 1);

	return 0;
}

static void sh_cmt_clock_event_suspend(struct clock_event_device *ced)
{
	struct sh_cmt_channel *ch = ced_to_sh_cmt(ced);

	dev_pm_genpd_suspend(&ch->cmt->pdev->dev);
	clk_unprepare(ch->cmt->clk);
}

static void sh_cmt_clock_event_resume(struct clock_event_device *ced)
{
	struct sh_cmt_channel *ch = ced_to_sh_cmt(ced);

	clk_prepare(ch->cmt->clk);
	dev_pm_genpd_resume(&ch->cmt->pdev->dev);
}

static int sh_cmt_register_clockevent(struct sh_cmt_channel *ch,
				      const char *name)
{
	struct clock_event_device *ced = &ch->ced;
	int irq;
	int ret;

	irq = platform_get_irq(ch->cmt->pdev, ch->index);
	if (irq < 0)
		return irq;

	ret = request_irq(irq, sh_cmt_interrupt,
			  IRQF_TIMER | IRQF_IRQPOLL | IRQF_NOBALANCING,
			  dev_name(&ch->cmt->pdev->dev), ch);
	if (ret) {
		dev_err(&ch->cmt->pdev->dev, "ch%u: failed to request irq %d\n",
			ch->index, irq);
		return ret;
	}

	ced->name = name;
	ced->features = CLOCK_EVT_FEAT_PERIODIC;
	ced->features |= CLOCK_EVT_FEAT_ONESHOT;
	ced->rating = 125;
	ced->cpumask = cpu_possible_mask;
	ced->set_next_event = sh_cmt_clock_event_next;
	ced->set_state_shutdown = sh_cmt_clock_event_shutdown;
	ced->set_state_periodic = sh_cmt_clock_event_set_periodic;
	ced->set_state_oneshot = sh_cmt_clock_event_set_oneshot;
	ced->suspend = sh_cmt_clock_event_suspend;
	ced->resume = sh_cmt_clock_event_resume;

	/* TODO: calculate good shift from rate and counter bit width */
	ced->shift = 32;
	ced->mult = div_sc(ch->cmt->rate, NSEC_PER_SEC, ced->shift);
	ced->max_delta_ns = clockevent_delta2ns(ch->max_match_value, ced);
	ced->max_delta_ticks = ch->max_match_value;
	ced->min_delta_ns = clockevent_delta2ns(0x1f, ced);
	ced->min_delta_ticks = 0x1f;

	dev_info(&ch->cmt->pdev->dev, "ch%u: used for clock events\n",
		 ch->index);
	clockevents_register_device(ced);

	return 0;
}

static int sh_cmt_register(struct sh_cmt_channel *ch, const char *name,
			   bool clockevent, bool clocksource)
{
	int ret;

	if (clockevent) {
		ch->cmt->has_clockevent = true;
		ret = sh_cmt_register_clockevent(ch, name);
		if (ret < 0)
			return ret;
	}

	if (clocksource) {
		ch->cmt->has_clocksource = true;
		sh_cmt_register_clocksource(ch, name);
	}

	return 0;
}

static int sh_cmt_setup_channel(struct sh_cmt_channel *ch, unsigned int index,
				unsigned int hwidx, bool clockevent,
				bool clocksource, struct sh_cmt_device *cmt)
{
	u32 value;
	int ret;

	/* Skip unused channels. */
	if (!clockevent && !clocksource)
		return 0;

	ch->cmt = cmt;
	ch->index = index;
	ch->hwidx = hwidx;
	ch->timer_bit = hwidx;

	/*
	 * Compute the address of the channel control register block. For the
	 * timers with a per-channel start/stop register, compute its address
	 * as well.
	 */
	switch (cmt->info->model) {
	case SH_CMT_16BIT:
		ch->ioctrl = cmt->mapbase + 2 + ch->hwidx * 6;
		break;
	case SH_CMT_32BIT:
	case SH_CMT_48BIT:
		ch->ioctrl = cmt->mapbase + 0x10 + ch->hwidx * 0x10;
		break;
	case SH_CMT0_RCAR_GEN2:
	case SH_CMT1_RCAR_GEN2:
		ch->iostart = cmt->mapbase + ch->hwidx * 0x100;
		ch->ioctrl = ch->iostart + 0x10;
		ch->timer_bit = 0;

		/* Enable the clock supply to the channel */
		value = ioread32(cmt->mapbase + CMCLKE);
		value |= BIT(hwidx);
		iowrite32(value, cmt->mapbase + CMCLKE);
		break;
	}

	if (cmt->info->width == (sizeof(ch->max_match_value) * 8))
		ch->max_match_value = ~0;
	else
		ch->max_match_value = (1 << cmt->info->width) - 1;

	ch->match_value = ch->max_match_value;
	raw_spin_lock_init(&ch->lock);

	ret = sh_cmt_register(ch, dev_name(&cmt->pdev->dev),
			      clockevent, clocksource);
	if (ret) {
		dev_err(&cmt->pdev->dev, "ch%u: registration failed\n",
			ch->index);
		return ret;
	}
	ch->cs_enabled = false;

	return 0;
}

static int sh_cmt_map_memory(struct sh_cmt_device *cmt)
{
	struct resource *mem;

	mem = platform_get_resource(cmt->pdev, IORESOURCE_MEM, 0);
	if (!mem) {
		dev_err(&cmt->pdev->dev, "failed to get I/O memory\n");
		return -ENXIO;
	}

	cmt->mapbase = ioremap(mem->start, resource_size(mem));
	if (cmt->mapbase == NULL) {
		dev_err(&cmt->pdev->dev, "failed to remap I/O memory\n");
		return -ENXIO;
	}

	return 0;
}

static const struct platform_device_id sh_cmt_id_table[] = {
	{ "sh-cmt-16", (kernel_ulong_t)&sh_cmt_info[SH_CMT_16BIT] },
	{ "sh-cmt-32", (kernel_ulong_t)&sh_cmt_info[SH_CMT_32BIT] },
	{ }
};
MODULE_DEVICE_TABLE(platform, sh_cmt_id_table);

static const struct of_device_id sh_cmt_of_table[] __maybe_unused = {
	{
		/* deprecated, preserved for backward compatibility */
		.compatible = "renesas,cmt-48",
		.data = &sh_cmt_info[SH_CMT_48BIT]
	},
	{
		/* deprecated, preserved for backward compatibility */
		.compatible = "renesas,cmt-48-gen2",
		.data = &sh_cmt_info[SH_CMT0_RCAR_GEN2]
	},
	{
		.compatible = "renesas,r8a7740-cmt1",
		.data = &sh_cmt_info[SH_CMT_48BIT]
	},
	{
		.compatible = "renesas,sh73a0-cmt1",
		.data = &sh_cmt_info[SH_CMT_48BIT]
	},
	{
		.compatible = "renesas,rcar-gen2-cmt0",
		.data = &sh_cmt_info[SH_CMT0_RCAR_GEN2]
	},
	{
		.compatible = "renesas,rcar-gen2-cmt1",
		.data = &sh_cmt_info[SH_CMT1_RCAR_GEN2]
	},
	{
		.compatible = "renesas,rcar-gen3-cmt0",
		.data = &sh_cmt_info[SH_CMT0_RCAR_GEN2]
	},
	{
		.compatible = "renesas,rcar-gen3-cmt1",
		.data = &sh_cmt_info[SH_CMT1_RCAR_GEN2]
	},
	{ }
};
MODULE_DEVICE_TABLE(of, sh_cmt_of_table);

static int sh_cmt_setup(struct sh_cmt_device *cmt, struct platform_device *pdev)
{
	unsigned int mask;
	unsigned int i;
	int ret;

	cmt->pdev = pdev;
	raw_spin_lock_init(&cmt->lock);

	if (IS_ENABLED(CONFIG_OF) && pdev->dev.of_node) {
		cmt->info = of_device_get_match_data(&pdev->dev);
		cmt->hw_channels = cmt->info->channels_mask;
	} else if (pdev->dev.platform_data) {
		struct sh_timer_config *cfg = pdev->dev.platform_data;
		const struct platform_device_id *id = pdev->id_entry;

		cmt->info = (const struct sh_cmt_info *)id->driver_data;
		cmt->hw_channels = cfg->channels_mask;
	} else {
		dev_err(&cmt->pdev->dev, "missing platform data\n");
		return -ENXIO;
	}

	/* Get hold of clock. */
	cmt->clk = clk_get(&cmt->pdev->dev, "fck");
	if (IS_ERR(cmt->clk)) {
		dev_err(&cmt->pdev->dev, "cannot get clock\n");
		return PTR_ERR(cmt->clk);
	}

	ret = clk_prepare(cmt->clk);
	if (ret < 0)
		goto err_clk_put;

	/* Determine clock rate. */
	ret = clk_enable(cmt->clk);
	if (ret < 0)
		goto err_clk_unprepare;

	if (cmt->info->width == 16)
		cmt->rate = clk_get_rate(cmt->clk) / 512;
	else
		cmt->rate = clk_get_rate(cmt->clk) / 8;

	/* Map the memory resource(s). */
	ret = sh_cmt_map_memory(cmt);
	if (ret < 0)
		goto err_clk_disable;

	/* Allocate and setup the channels. */
	cmt->num_channels = hweight8(cmt->hw_channels);
	cmt->channels = kcalloc(cmt->num_channels, sizeof(*cmt->channels),
				GFP_KERNEL);
	if (cmt->channels == NULL) {
		ret = -ENOMEM;
		goto err_unmap;
	}

	/*
	 * Use the first channel as a clock event device and the second channel
	 * as a clock source. If only one channel is available use it for both.
	 */
	for (i = 0, mask = cmt->hw_channels; i < cmt->num_channels; ++i) {
		unsigned int hwidx = ffs(mask) - 1;
		bool clocksource = i == 1 || cmt->num_channels == 1;
		bool clockevent = i == 0;

		ret = sh_cmt_setup_channel(&cmt->channels[i], i, hwidx,
					   clockevent, clocksource, cmt);
		if (ret < 0)
			goto err_unmap;

		mask &= ~(1 << hwidx);
	}

	clk_disable(cmt->clk);

	platform_set_drvdata(pdev, cmt);

	return 0;

err_unmap:
	kfree(cmt->channels);
	iounmap(cmt->mapbase);
err_clk_disable:
	clk_disable(cmt->clk);
err_clk_unprepare:
	clk_unprepare(cmt->clk);
err_clk_put:
	clk_put(cmt->clk);
	return ret;
}

static int sh_cmt_probe(struct platform_device *pdev)
{
	struct sh_cmt_device *cmt = platform_get_drvdata(pdev);
	int ret;

	if (!is_sh_early_platform_device(pdev)) {
		pm_runtime_set_active(&pdev->dev);
		pm_runtime_enable(&pdev->dev);
	}

	if (cmt) {
		dev_info(&pdev->dev, "kept as earlytimer\n");
		goto out;
	}

	cmt = kzalloc(sizeof(*cmt), GFP_KERNEL);
	if (cmt == NULL)
		return -ENOMEM;

	ret = sh_cmt_setup(cmt, pdev);
	if (ret) {
		kfree(cmt);
		pm_runtime_idle(&pdev->dev);
		return ret;
	}
	if (is_sh_early_platform_device(pdev))
		return 0;

 out:
	if (cmt->has_clockevent || cmt->has_clocksource)
		pm_runtime_irq_safe(&pdev->dev);
	else
		pm_runtime_idle(&pdev->dev);

	return 0;
}

static int sh_cmt_remove(struct platform_device *pdev)
{
	return -EBUSY; /* cannot unregister clockevent and clocksource */
}

static struct platform_driver sh_cmt_device_driver = {
	.probe		= sh_cmt_probe,
	.remove		= sh_cmt_remove,
	.driver		= {
		.name	= "sh_cmt",
		.of_match_table = of_match_ptr(sh_cmt_of_table),
	},
	.id_table	= sh_cmt_id_table,
};

static int __init sh_cmt_init(void)
{
	return platform_driver_register(&sh_cmt_device_driver);
}

static void __exit sh_cmt_exit(void)
{
	platform_driver_unregister(&sh_cmt_device_driver);
}

#ifdef CONFIG_SUPERH
sh_early_platform_init("earlytimer", &sh_cmt_device_driver);
#endif

subsys_initcall(sh_cmt_init);
module_exit(sh_cmt_exit);

MODULE_AUTHOR("Magnus Damm");
MODULE_DESCRIPTION("SuperH CMT Timer Driver");
MODULE_LICENSE("GPL v2");
