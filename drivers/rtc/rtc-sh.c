// SPDX-License-Identifier: GPL-2.0
/*
 * SuperH On-Chip RTC Support
 *
 * Copyright (C) 2006 - 2009  Paul Mundt
 * Copyright (C) 2006  Jamie Lenehan
 * Copyright (C) 2008  Angelo Castello
 *
 * Based on the old arch/sh/kernel/cpu/rtc.c by:
 *
 *  Copyright (C) 2000  Philipp Rumpf <prumpf@tux.org>
 *  Copyright (C) 1999  Tetsuya Okada & Niibe Yutaka
 */
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/kernel.h>
#include <linux/bcd.h>
#include <linux/rtc.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/seq_file.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/log2.h>
#include <linux/clk.h>
#include <linux/slab.h>
#ifdef CONFIG_SUPERH
#include <asm/rtc.h>
#else
/* Default values for RZ/A RTC */
#define rtc_reg_size		sizeof(u16)
#define RTC_BIT_INVERTED        0	/* no chip bugs */
#define RTC_CAP_4_DIGIT_YEAR    (1 << 0)
#define RTC_DEF_CAPABILITIES    RTC_CAP_4_DIGIT_YEAR
#endif

#define DRV_NAME	"sh-rtc"

#define RTC_REG(r)	((r) * rtc_reg_size)

#define R64CNT		RTC_REG(0)

#define RSECCNT		RTC_REG(1)	/* RTC sec */
#define RMINCNT		RTC_REG(2)	/* RTC min */
#define RHRCNT		RTC_REG(3)	/* RTC hour */
#define RWKCNT		RTC_REG(4)	/* RTC week */
#define RDAYCNT		RTC_REG(5)	/* RTC day */
#define RMONCNT		RTC_REG(6)	/* RTC month */
#define RYRCNT		RTC_REG(7)	/* RTC year */
#define RSECAR		RTC_REG(8)	/* ALARM sec */
#define RMINAR		RTC_REG(9)	/* ALARM min */
#define RHRAR		RTC_REG(10)	/* ALARM hour */
#define RWKAR		RTC_REG(11)	/* ALARM week */
#define RDAYAR		RTC_REG(12)	/* ALARM day */
#define RMONAR		RTC_REG(13)	/* ALARM month */
#define RCR1		RTC_REG(14)	/* Control */
#define RCR2		RTC_REG(15)	/* Control */

/*
 * Note on RYRAR and RCR3: Up until this point most of the register
 * definitions are consistent across all of the available parts. However,
 * the placement of the optional RYRAR and RCR3 (the RYRAR control
 * register used to control RYRCNT/RYRAR compare) varies considerably
 * across various parts, occasionally being mapped in to a completely
 * unrelated address space. For proper RYRAR support a separate resource
 * would have to be handed off, but as this is purely optional in
 * practice, we simply opt not to support it, thereby keeping the code
 * quite a bit more simplified.
 */

/* ALARM Bits - or with BCD encoded value */
#define AR_ENB		0x80	/* Enable for alarm cmp   */

/* Period Bits */
#define PF_HP		0x100	/* Enable Half Period to support 8,32,128Hz */
#define PF_COUNT	0x200	/* Half periodic counter */
#define PF_OXS		0x400	/* Periodic One x Second */
#define PF_KOU		0x800	/* Kernel or User periodic request 1=kernel */
#define PF_MASK		0xf00

/* RCR1 Bits */
#define RCR1_CF		0x80	/* Carry Flag             */
#define RCR1_CIE	0x10	/* Carry Interrupt Enable */
#define RCR1_AIE	0x08	/* Alarm Interrupt Enable */
#define RCR1_AF		0x01	/* Alarm Flag             */

/* RCR2 Bits */
#define RCR2_PEF	0x80	/* PEriodic interrupt Flag */
#define RCR2_PESMASK	0x70	/* Periodic interrupt Set  */
#define RCR2_RTCEN	0x08	/* ENable RTC              */
#define RCR2_ADJ	0x04	/* ADJustment (30-second)  */
#define RCR2_RESET	0x02	/* Reset bit               */
#define RCR2_START	0x01	/* Start bit               */

struct sh_rtc {
	void __iomem		*regbase;
	unsigned long		regsize;
	struct resource		*res;
	int			alarm_irq;
	int			periodic_irq;
	int			carry_irq;
	struct clk		*clk;
	struct rtc_device	*rtc_dev;
	spinlock_t		lock;
	unsigned long		capabilities;	/* See asm/rtc.h for cap bits */
	unsigned short		periodic_freq;
};

static int __sh_rtc_interrupt(struct sh_rtc *rtc)
{
	unsigned int tmp, pending;

	tmp = readb(rtc->regbase + RCR1);
	pending = tmp & RCR1_CF;
	tmp &= ~RCR1_CF;
	writeb(tmp, rtc->regbase + RCR1);

	/* Users have requested One x Second IRQ */
	if (pending && rtc->periodic_freq & PF_OXS)
		rtc_update_irq(rtc->rtc_dev, 1, RTC_UF | RTC_IRQF);

	return pending;
}

static int __sh_rtc_alarm(struct sh_rtc *rtc)
{
	unsigned int tmp, pending;

	tmp = readb(rtc->regbase + RCR1);
	pending = tmp & RCR1_AF;
	tmp &= ~(RCR1_AF | RCR1_AIE);
	writeb(tmp, rtc->regbase + RCR1);

	if (pending)
		rtc_update_irq(rtc->rtc_dev, 1, RTC_AF | RTC_IRQF);

	return pending;
}

static int __sh_rtc_periodic(struct sh_rtc *rtc)
{
	unsigned int tmp, pending;

	tmp = readb(rtc->regbase + RCR2);
	pending = tmp & RCR2_PEF;
	tmp &= ~RCR2_PEF;
	writeb(tmp, rtc->regbase + RCR2);

	if (!pending)
		return 0;

	/* Half period enabled than one skipped and the next notified */
	if ((rtc->periodic_freq & PF_HP) && (rtc->periodic_freq & PF_COUNT))
		rtc->periodic_freq &= ~PF_COUNT;
	else {
		if (rtc->periodic_freq & PF_HP)
			rtc->periodic_freq |= PF_COUNT;
		rtc_update_irq(rtc->rtc_dev, 1, RTC_PF | RTC_IRQF);
	}

	return pending;
}

static irqreturn_t sh_rtc_interrupt(int irq, void *dev_id)
{
	struct sh_rtc *rtc = dev_id;
	int ret;

	spin_lock(&rtc->lock);
	ret = __sh_rtc_interrupt(rtc);
	spin_unlock(&rtc->lock);

	return IRQ_RETVAL(ret);
}

static irqreturn_t sh_rtc_alarm(int irq, void *dev_id)
{
	struct sh_rtc *rtc = dev_id;
	int ret;

	spin_lock(&rtc->lock);
	ret = __sh_rtc_alarm(rtc);
	spin_unlock(&rtc->lock);

	return IRQ_RETVAL(ret);
}

static irqreturn_t sh_rtc_periodic(int irq, void *dev_id)
{
	struct sh_rtc *rtc = dev_id;
	int ret;

	spin_lock(&rtc->lock);
	ret = __sh_rtc_periodic(rtc);
	spin_unlock(&rtc->lock);

	return IRQ_RETVAL(ret);
}

static irqreturn_t sh_rtc_shared(int irq, void *dev_id)
{
	struct sh_rtc *rtc = dev_id;
	int ret;

	spin_lock(&rtc->lock);
	ret = __sh_rtc_interrupt(rtc);
	ret |= __sh_rtc_alarm(rtc);
	ret |= __sh_rtc_periodic(rtc);
	spin_unlock(&rtc->lock);

	return IRQ_RETVAL(ret);
}

static inline void sh_rtc_setaie(struct device *dev, unsigned int enable)
{
	struct sh_rtc *rtc = dev_get_drvdata(dev);
	unsigned int tmp;

	spin_lock_irq(&rtc->lock);

	tmp = readb(rtc->regbase + RCR1);

	if (enable)
		tmp |= RCR1_AIE;
	else
		tmp &= ~RCR1_AIE;

	writeb(tmp, rtc->regbase + RCR1);

	spin_unlock_irq(&rtc->lock);
}

static int sh_rtc_proc(struct device *dev, struct seq_file *seq)
{
	struct sh_rtc *rtc = dev_get_drvdata(dev);
	unsigned int tmp;

	tmp = readb(rtc->regbase + RCR1);
	seq_printf(seq, "carry_IRQ\t: %s\n", (tmp & RCR1_CIE) ? "yes" : "no");

	tmp = readb(rtc->regbase + RCR2);
	seq_printf(seq, "periodic_IRQ\t: %s\n",
		   (tmp & RCR2_PESMASK) ? "yes" : "no");

	return 0;
}

static inline void sh_rtc_setcie(struct device *dev, unsigned int enable)
{
	struct sh_rtc *rtc = dev_get_drvdata(dev);
	unsigned int tmp;

	spin_lock_irq(&rtc->lock);

	tmp = readb(rtc->regbase + RCR1);

	if (!enable)
		tmp &= ~RCR1_CIE;
	else
		tmp |= RCR1_CIE;

	writeb(tmp, rtc->regbase + RCR1);

	spin_unlock_irq(&rtc->lock);
}

static int sh_rtc_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	sh_rtc_setaie(dev, enabled);
	return 0;
}

static int sh_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	struct sh_rtc *rtc = dev_get_drvdata(dev);
	unsigned int sec128, sec2, yr, yr100, cf_bit;

	if (!(readb(rtc->regbase + RCR2) & RCR2_RTCEN))
		return -EINVAL;

	do {
		unsigned int tmp;

		spin_lock_irq(&rtc->lock);

		tmp = readb(rtc->regbase + RCR1);
		tmp &= ~RCR1_CF; /* Clear CF-bit */
		tmp |= RCR1_CIE;
		writeb(tmp, rtc->regbase + RCR1);

		sec128 = readb(rtc->regbase + R64CNT);

		tm->tm_sec	= bcd2bin(readb(rtc->regbase + RSECCNT));
		tm->tm_min	= bcd2bin(readb(rtc->regbase + RMINCNT));
		tm->tm_hour	= bcd2bin(readb(rtc->regbase + RHRCNT));
		tm->tm_wday	= bcd2bin(readb(rtc->regbase + RWKCNT));
		tm->tm_mday	= bcd2bin(readb(rtc->regbase + RDAYCNT));
		tm->tm_mon	= bcd2bin(readb(rtc->regbase + RMONCNT)) - 1;

		if (rtc->capabilities & RTC_CAP_4_DIGIT_YEAR) {
			yr  = readw(rtc->regbase + RYRCNT);
			yr100 = bcd2bin(yr >> 8);
			yr &= 0xff;
		} else {
			yr  = readb(rtc->regbase + RYRCNT);
			yr100 = bcd2bin((yr == 0x99) ? 0x19 : 0x20);
		}

		tm->tm_year = (yr100 * 100 + bcd2bin(yr)) - 1900;

		sec2 = readb(rtc->regbase + R64CNT);
		cf_bit = readb(rtc->regbase + RCR1) & RCR1_CF;

		spin_unlock_irq(&rtc->lock);
	} while (cf_bit != 0 || ((sec128 ^ sec2) & RTC_BIT_INVERTED) != 0);

#if RTC_BIT_INVERTED != 0
	if ((sec128 & RTC_BIT_INVERTED))
		tm->tm_sec--;
#endif

	/* only keep the carry interrupt enabled if UIE is on */
	if (!(rtc->periodic_freq & PF_OXS))
		sh_rtc_setcie(dev, 0);

	dev_dbg(dev, "%s: tm is secs=%d, mins=%d, hours=%d, "
		"mday=%d, mon=%d, year=%d, wday=%d\n",
		__func__,
		tm->tm_sec, tm->tm_min, tm->tm_hour,
		tm->tm_mday, tm->tm_mon + 1, tm->tm_year, tm->tm_wday);

	return 0;
}

static int sh_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	struct sh_rtc *rtc = dev_get_drvdata(dev);
	unsigned int tmp;
	int year;

	spin_lock_irq(&rtc->lock);

	/* Reset pre-scaler & stop RTC */
	tmp = readb(rtc->regbase + RCR2);
	tmp |= RCR2_RESET;
	tmp &= ~RCR2_START;
	writeb(tmp, rtc->regbase + RCR2);

	writeb(bin2bcd(tm->tm_sec),  rtc->regbase + RSECCNT);
	writeb(bin2bcd(tm->tm_min),  rtc->regbase + RMINCNT);
	writeb(bin2bcd(tm->tm_hour), rtc->regbase + RHRCNT);
	writeb(bin2bcd(tm->tm_wday), rtc->regbase + RWKCNT);
	writeb(bin2bcd(tm->tm_mday), rtc->regbase + RDAYCNT);
	writeb(bin2bcd(tm->tm_mon + 1), rtc->regbase + RMONCNT);

	if (rtc->capabilities & RTC_CAP_4_DIGIT_YEAR) {
		year = (bin2bcd((tm->tm_year + 1900) / 100) << 8) |
			bin2bcd(tm->tm_year % 100);
		writew(year, rtc->regbase + RYRCNT);
	} else {
		year = tm->tm_year % 100;
		writeb(bin2bcd(year), rtc->regbase + RYRCNT);
	}

	/* Start RTC */
	tmp = readb(rtc->regbase + RCR2);
	tmp &= ~RCR2_RESET;
	tmp |= RCR2_RTCEN | RCR2_START;
	writeb(tmp, rtc->regbase + RCR2);

	spin_unlock_irq(&rtc->lock);

	return 0;
}

static inline int sh_rtc_read_alarm_value(struct sh_rtc *rtc, int reg_off)
{
	unsigned int byte;
	int value = -1;			/* return -1 for ignored values */

	byte = readb(rtc->regbase + reg_off);
	if (byte & AR_ENB) {
		byte &= ~AR_ENB;	/* strip the enable bit */
		value = bcd2bin(byte);
	}

	return value;
}

static int sh_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *wkalrm)
{
	struct sh_rtc *rtc = dev_get_drvdata(dev);
	struct rtc_time *tm = &wkalrm->time;

	spin_lock_irq(&rtc->lock);

	tm->tm_sec	= sh_rtc_read_alarm_value(rtc, RSECAR);
	tm->tm_min	= sh_rtc_read_alarm_value(rtc, RMINAR);
	tm->tm_hour	= sh_rtc_read_alarm_value(rtc, RHRAR);
	tm->tm_wday	= sh_rtc_read_alarm_value(rtc, RWKAR);
	tm->tm_mday	= sh_rtc_read_alarm_value(rtc, RDAYAR);
	tm->tm_mon	= sh_rtc_read_alarm_value(rtc, RMONAR);
	if (tm->tm_mon > 0)
		tm->tm_mon -= 1; /* RTC is 1-12, tm_mon is 0-11 */

	wkalrm->enabled = (readb(rtc->regbase + RCR1) & RCR1_AIE) ? 1 : 0;

	spin_unlock_irq(&rtc->lock);

	return 0;
}

static inline void sh_rtc_write_alarm_value(struct sh_rtc *rtc,
					    int value, int reg_off)
{
	/* < 0 for a value that is ignored */
	if (value < 0)
		writeb(0, rtc->regbase + reg_off);
	else
		writeb(bin2bcd(value) | AR_ENB,  rtc->regbase + reg_off);
}

static int sh_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *wkalrm)
{
	struct sh_rtc *rtc = dev_get_drvdata(dev);
	unsigned int rcr1;
	struct rtc_time *tm = &wkalrm->time;
	int mon;

	spin_lock_irq(&rtc->lock);

	/* disable alarm interrupt and clear the alarm flag */
	rcr1 = readb(rtc->regbase + RCR1);
	rcr1 &= ~(RCR1_AF | RCR1_AIE);
	writeb(rcr1, rtc->regbase + RCR1);

	/* set alarm time */
	sh_rtc_write_alarm_value(rtc, tm->tm_sec,  RSECAR);
	sh_rtc_write_alarm_value(rtc, tm->tm_min,  RMINAR);
	sh_rtc_write_alarm_value(rtc, tm->tm_hour, RHRAR);
	sh_rtc_write_alarm_value(rtc, tm->tm_wday, RWKAR);
	sh_rtc_write_alarm_value(rtc, tm->tm_mday, RDAYAR);
	mon = tm->tm_mon;
	if (mon >= 0)
		mon += 1;
	sh_rtc_write_alarm_value(rtc, mon, RMONAR);

	if (wkalrm->enabled) {
		rcr1 |= RCR1_AIE;
		writeb(rcr1, rtc->regbase + RCR1);
	}

	spin_unlock_irq(&rtc->lock);

	return 0;
}

static const struct rtc_class_ops sh_rtc_ops = {
	.read_time	= sh_rtc_read_time,
	.set_time	= sh_rtc_set_time,
	.read_alarm	= sh_rtc_read_alarm,
	.set_alarm	= sh_rtc_set_alarm,
	.proc		= sh_rtc_proc,
	.alarm_irq_enable = sh_rtc_alarm_irq_enable,
};

static int __init sh_rtc_probe(struct platform_device *pdev)
{
	struct sh_rtc *rtc;
	struct resource *res;
	char clk_name[6];
	int clk_id, ret;

	rtc = devm_kzalloc(&pdev->dev, sizeof(*rtc), GFP_KERNEL);
	if (unlikely(!rtc))
		return -ENOMEM;

	spin_lock_init(&rtc->lock);

	/* get periodic/carry/alarm irqs */
	ret = platform_get_irq(pdev, 0);
	if (unlikely(ret <= 0)) {
		dev_err(&pdev->dev, "No IRQ resource\n");
		return -ENOENT;
	}

	rtc->periodic_irq = ret;
	rtc->carry_irq = platform_get_irq(pdev, 1);
	rtc->alarm_irq = platform_get_irq(pdev, 2);

	res = platform_get_resource(pdev, IORESOURCE_IO, 0);
	if (!res)
		res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (unlikely(res == NULL)) {
		dev_err(&pdev->dev, "No IO resource\n");
		return -ENOENT;
	}

	rtc->regsize = resource_size(res);

	rtc->res = devm_request_mem_region(&pdev->dev, res->start,
					rtc->regsize, pdev->name);
	if (unlikely(!rtc->res))
		return -EBUSY;

	rtc->regbase = devm_ioremap(&pdev->dev, rtc->res->start, rtc->regsize);
	if (unlikely(!rtc->regbase))
		return -EINVAL;

	if (!pdev->dev.of_node) {
		clk_id = pdev->id;
		/* With a single device, the clock id is still "rtc0" */
		if (clk_id < 0)
			clk_id = 0;

		snprintf(clk_name, sizeof(clk_name), "rtc%d", clk_id);
	} else
		snprintf(clk_name, sizeof(clk_name), "fck");

	rtc->clk = devm_clk_get(&pdev->dev, clk_name);
	if (IS_ERR(rtc->clk)) {
		/*
		 * No error handling for rtc->clk intentionally, not all
		 * platforms will have a unique clock for the RTC, and
		 * the clk API can handle the struct clk pointer being
		 * NULL.
		 */
		rtc->clk = NULL;
	}

	rtc->rtc_dev = devm_rtc_allocate_device(&pdev->dev);
	if (IS_ERR(rtc->rtc_dev))
		return PTR_ERR(rtc->rtc_dev);

	clk_enable(rtc->clk);

	rtc->capabilities = RTC_DEF_CAPABILITIES;

#ifdef CONFIG_SUPERH
	if (dev_get_platdata(&pdev->dev)) {
		struct sh_rtc_platform_info *pinfo =
			dev_get_platdata(&pdev->dev);

		/*
		 * Some CPUs have special capabilities in addition to the
		 * default set. Add those in here.
		 */
		rtc->capabilities |= pinfo->capabilities;
	}
#endif

	if (rtc->carry_irq <= 0) {
		/* register shared periodic/carry/alarm irq */
		ret = devm_request_irq(&pdev->dev, rtc->periodic_irq,
				sh_rtc_shared, 0, "sh-rtc", rtc);
		if (unlikely(ret)) {
			dev_err(&pdev->dev,
				"request IRQ failed with %d, IRQ %d\n", ret,
				rtc->periodic_irq);
			goto err_unmap;
		}
	} else {
		/* register periodic/carry/alarm irqs */
		ret = devm_request_irq(&pdev->dev, rtc->periodic_irq,
				sh_rtc_periodic, 0, "sh-rtc period", rtc);
		if (unlikely(ret)) {
			dev_err(&pdev->dev,
				"request period IRQ failed with %d, IRQ %d\n",
				ret, rtc->periodic_irq);
			goto err_unmap;
		}

		ret = devm_request_irq(&pdev->dev, rtc->carry_irq,
				sh_rtc_interrupt, 0, "sh-rtc carry", rtc);
		if (unlikely(ret)) {
			dev_err(&pdev->dev,
				"request carry IRQ failed with %d, IRQ %d\n",
				ret, rtc->carry_irq);
			goto err_unmap;
		}

		ret = devm_request_irq(&pdev->dev, rtc->alarm_irq,
				sh_rtc_alarm, 0, "sh-rtc alarm", rtc);
		if (unlikely(ret)) {
			dev_err(&pdev->dev,
				"request alarm IRQ failed with %d, IRQ %d\n",
				ret, rtc->alarm_irq);
			goto err_unmap;
		}
	}

	platform_set_drvdata(pdev, rtc);

	/* everything disabled by default */
	sh_rtc_setaie(&pdev->dev, 0);
	sh_rtc_setcie(&pdev->dev, 0);

	rtc->rtc_dev->ops = &sh_rtc_ops;
	rtc->rtc_dev->max_user_freq = 256;

	if (rtc->capabilities & RTC_CAP_4_DIGIT_YEAR) {
		rtc->rtc_dev->range_min = RTC_TIMESTAMP_BEGIN_1900;
		rtc->rtc_dev->range_max = RTC_TIMESTAMP_END_9999;
	} else {
		rtc->rtc_dev->range_min = mktime64(1999, 1, 1, 0, 0, 0);
		rtc->rtc_dev->range_max = mktime64(2098, 12, 31, 23, 59, 59);
	}

	ret = devm_rtc_register_device(rtc->rtc_dev);
	if (ret)
		goto err_unmap;

	device_init_wakeup(&pdev->dev, 1);
	return 0;

err_unmap:
	clk_disable(rtc->clk);

	return ret;
}

static int __exit sh_rtc_remove(struct platform_device *pdev)
{
	struct sh_rtc *rtc = platform_get_drvdata(pdev);

	sh_rtc_setaie(&pdev->dev, 0);
	sh_rtc_setcie(&pdev->dev, 0);

	clk_disable(rtc->clk);

	return 0;
}

static void sh_rtc_set_irq_wake(struct device *dev, int enabled)
{
	struct sh_rtc *rtc = dev_get_drvdata(dev);

	irq_set_irq_wake(rtc->periodic_irq, enabled);

	if (rtc->carry_irq > 0) {
		irq_set_irq_wake(rtc->carry_irq, enabled);
		irq_set_irq_wake(rtc->alarm_irq, enabled);
	}
}

static int __maybe_unused sh_rtc_suspend(struct device *dev)
{
	if (device_may_wakeup(dev))
		sh_rtc_set_irq_wake(dev, 1);

	return 0;
}

static int __maybe_unused sh_rtc_resume(struct device *dev)
{
	if (device_may_wakeup(dev))
		sh_rtc_set_irq_wake(dev, 0);

	return 0;
}

static SIMPLE_DEV_PM_OPS(sh_rtc_pm_ops, sh_rtc_suspend, sh_rtc_resume);

static const struct of_device_id sh_rtc_of_match[] = {
	{ .compatible = "renesas,sh-rtc", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, sh_rtc_of_match);

static struct platform_driver sh_rtc_platform_driver = {
	.driver		= {
		.name	= DRV_NAME,
		.pm	= &sh_rtc_pm_ops,
		.of_match_table = sh_rtc_of_match,
	},
	.remove		= __exit_p(sh_rtc_remove),
};

module_platform_driver_probe(sh_rtc_platform_driver, sh_rtc_probe);

MODULE_DESCRIPTION("SuperH on-chip RTC driver");
MODULE_AUTHOR("Paul Mundt <lethal@linux-sh.org>, "
	      "Jamie Lenehan <lenehan@twibble.org>, "
	      "Angelo Castello <angelo.castello@st.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRV_NAME);
