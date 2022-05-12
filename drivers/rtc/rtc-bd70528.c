// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (C) 2018 ROHM Semiconductors
//
// RTC driver for ROHM BD70528 PMIC

#include <linux/bcd.h>
#include <linux/mfd/rohm-bd70528.h>
#include <linux/mfd/rohm-bd71815.h>
#include <linux/mfd/rohm-bd71828.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/rtc.h>

/*
 * On BD71828 and BD71815 the ALM0 MASK is 14 bytes after the ALM0
 * block start
 */
#define BD718XX_ALM_EN_OFFSET 14

/*
 * We read regs RTC_SEC => RTC_YEAR
 * this struct is ordered according to chip registers.
 * Keep it u8 only (or packed) to avoid padding issues.
 */
struct bd70528_rtc_day {
	u8 sec;
	u8 min;
	u8 hour;
} __packed;

struct bd70528_rtc_data {
	struct bd70528_rtc_day time;
	u8 week;
	u8 day;
	u8 month;
	u8 year;
} __packed;

struct bd70528_rtc_wake {
	struct bd70528_rtc_day time;
	u8 ctrl;
} __packed;

struct bd71828_rtc_alm {
	struct bd70528_rtc_data alm0;
	struct bd70528_rtc_data alm1;
	u8 alm_mask;
	u8 alm1_mask;
} __packed;

struct bd70528_rtc_alm {
	struct bd70528_rtc_data data;
	u8 alm_mask;
	u8 alm_repeat;
} __packed;

struct bd70528_rtc {
	struct rohm_regmap_dev *parent;
	struct regmap *regmap;
	struct device *dev;
	u8 reg_time_start;
	u8 bd718xx_alm_block_start;
	bool has_rtc_timers;
};

static int bd70528_set_wake(struct rohm_regmap_dev *bd70528,
			    int enable, int *old_state)
{
	int ret;
	unsigned int ctrl_reg;

	ret = regmap_read(bd70528->regmap, BD70528_REG_WAKE_EN, &ctrl_reg);
	if (ret)
		return ret;

	if (old_state) {
		if (ctrl_reg & BD70528_MASK_WAKE_EN)
			*old_state |= BD70528_WAKE_STATE_BIT;
		else
			*old_state &= ~BD70528_WAKE_STATE_BIT;

		if (!enable == !(*old_state & BD70528_WAKE_STATE_BIT))
			return 0;
	}

	if (enable)
		ctrl_reg |= BD70528_MASK_WAKE_EN;
	else
		ctrl_reg &= ~BD70528_MASK_WAKE_EN;

	return regmap_write(bd70528->regmap, BD70528_REG_WAKE_EN,
			    ctrl_reg);
}

static int bd70528_set_elapsed_tmr(struct rohm_regmap_dev *bd70528,
				   int enable, int *old_state)
{
	int ret;
	unsigned int ctrl_reg;

	/*
	 * TBD
	 * What is the purpose of elapsed timer ?
	 * Is the timeout registers counting down, or is the disable - re-enable
	 * going to restart the elapsed-time counting? If counting is restarted
	 * the timeout should be decreased by the amount of time that has
	 * elapsed since starting the timer. Maybe we should store the monotonic
	 * clock value when timer is started so that if RTC is set while timer
	 * is armed we could do the compensation. This is a hack if RTC/system
	 * clk are drifting. OTOH, RTC controlled via I2C is in any case
	 * inaccurate...
	 */
	ret = regmap_read(bd70528->regmap, BD70528_REG_ELAPSED_TIMER_EN,
			  &ctrl_reg);
	if (ret)
		return ret;

	if (old_state) {
		if (ctrl_reg & BD70528_MASK_ELAPSED_TIMER_EN)
			*old_state |= BD70528_ELAPSED_STATE_BIT;
		else
			*old_state &= ~BD70528_ELAPSED_STATE_BIT;

		if ((!enable) == (!(*old_state & BD70528_ELAPSED_STATE_BIT)))
			return 0;
	}

	if (enable)
		ctrl_reg |= BD70528_MASK_ELAPSED_TIMER_EN;
	else
		ctrl_reg &= ~BD70528_MASK_ELAPSED_TIMER_EN;

	return regmap_write(bd70528->regmap, BD70528_REG_ELAPSED_TIMER_EN,
			    ctrl_reg);
}

static int bd70528_set_rtc_based_timers(struct bd70528_rtc *r, int new_state,
					int *old_state)
{
	int ret;

	ret = bd70528_wdt_set(r->parent, new_state & BD70528_WDT_STATE_BIT,
			      old_state);
	if (ret) {
		dev_err(r->dev,
			"Failed to disable WDG for RTC setting (%d)\n", ret);
		return ret;
	}
	ret = bd70528_set_elapsed_tmr(r->parent,
				      new_state & BD70528_ELAPSED_STATE_BIT,
				      old_state);
	if (ret) {
		dev_err(r->dev,
			"Failed to disable 'elapsed timer' for RTC setting\n");
		return ret;
	}
	ret = bd70528_set_wake(r->parent, new_state & BD70528_WAKE_STATE_BIT,
			       old_state);
	if (ret) {
		dev_err(r->dev,
			"Failed to disable 'wake timer' for RTC setting\n");
		return ret;
	}

	return ret;
}

static int bd70528_re_enable_rtc_based_timers(struct bd70528_rtc *r,
					      int old_state)
{
	if (!r->has_rtc_timers)
		return 0;

	return bd70528_set_rtc_based_timers(r, old_state, NULL);
}

static int bd70528_disable_rtc_based_timers(struct bd70528_rtc *r,
					    int *old_state)
{
	if (!r->has_rtc_timers)
		return 0;

	return bd70528_set_rtc_based_timers(r, 0, old_state);
}

static inline void tmday2rtc(struct rtc_time *t, struct bd70528_rtc_day *d)
{
	d->sec &= ~BD70528_MASK_RTC_SEC;
	d->min &= ~BD70528_MASK_RTC_MINUTE;
	d->hour &= ~BD70528_MASK_RTC_HOUR;
	d->sec |= bin2bcd(t->tm_sec);
	d->min |= bin2bcd(t->tm_min);
	d->hour |= bin2bcd(t->tm_hour);
}

static inline void tm2rtc(struct rtc_time *t, struct bd70528_rtc_data *r)
{
	r->day &= ~BD70528_MASK_RTC_DAY;
	r->week &= ~BD70528_MASK_RTC_WEEK;
	r->month &= ~BD70528_MASK_RTC_MONTH;
	/*
	 * PM and 24H bits are not used by Wake - thus we clear them
	 * here and not in tmday2rtc() which is also used by wake.
	 */
	r->time.hour &= ~(BD70528_MASK_RTC_HOUR_PM | BD70528_MASK_RTC_HOUR_24H);

	tmday2rtc(t, &r->time);
	/*
	 * We do always set time in 24H mode.
	 */
	r->time.hour |= BD70528_MASK_RTC_HOUR_24H;
	r->day |= bin2bcd(t->tm_mday);
	r->week |= bin2bcd(t->tm_wday);
	r->month |= bin2bcd(t->tm_mon + 1);
	r->year = bin2bcd(t->tm_year - 100);
}

static inline void rtc2tm(struct bd70528_rtc_data *r, struct rtc_time *t)
{
	t->tm_sec = bcd2bin(r->time.sec & BD70528_MASK_RTC_SEC);
	t->tm_min = bcd2bin(r->time.min & BD70528_MASK_RTC_MINUTE);
	t->tm_hour = bcd2bin(r->time.hour & BD70528_MASK_RTC_HOUR);
	/*
	 * If RTC is in 12H mode, then bit BD70528_MASK_RTC_HOUR_PM
	 * is not BCD value but tells whether it is AM or PM
	 */
	if (!(r->time.hour & BD70528_MASK_RTC_HOUR_24H)) {
		t->tm_hour %= 12;
		if (r->time.hour & BD70528_MASK_RTC_HOUR_PM)
			t->tm_hour += 12;
	}
	t->tm_mday = bcd2bin(r->day & BD70528_MASK_RTC_DAY);
	t->tm_mon = bcd2bin(r->month & BD70528_MASK_RTC_MONTH) - 1;
	t->tm_year = 100 + bcd2bin(r->year & BD70528_MASK_RTC_YEAR);
	t->tm_wday = bcd2bin(r->week & BD70528_MASK_RTC_WEEK);
}

static int bd71828_set_alarm(struct device *dev, struct rtc_wkalrm *a)
{
	int ret;
	struct bd71828_rtc_alm alm;
	struct bd70528_rtc *r = dev_get_drvdata(dev);

	ret = regmap_bulk_read(r->regmap, r->bd718xx_alm_block_start, &alm,
			       sizeof(alm));
	if (ret) {
		dev_err(dev, "Failed to read alarm regs\n");
		return ret;
	}

	tm2rtc(&a->time, &alm.alm0);

	if (!a->enabled)
		alm.alm_mask &= ~BD70528_MASK_ALM_EN;
	else
		alm.alm_mask |= BD70528_MASK_ALM_EN;

	ret = regmap_bulk_write(r->regmap, r->bd718xx_alm_block_start, &alm,
				sizeof(alm));
	if (ret)
		dev_err(dev, "Failed to set alarm time\n");

	return ret;

}

static int bd70528_set_alarm(struct device *dev, struct rtc_wkalrm *a)
{
	struct bd70528_rtc_wake wake;
	struct bd70528_rtc_alm alm;
	int ret;
	struct bd70528_rtc *r = dev_get_drvdata(dev);

	ret = regmap_bulk_read(r->regmap, BD70528_REG_RTC_WAKE_START, &wake,
			       sizeof(wake));
	if (ret) {
		dev_err(dev, "Failed to read wake regs\n");
		return ret;
	}

	ret = regmap_bulk_read(r->regmap, BD70528_REG_RTC_ALM_START, &alm,
			       sizeof(alm));
	if (ret) {
		dev_err(dev, "Failed to read alarm regs\n");
		return ret;
	}

	tm2rtc(&a->time, &alm.data);
	tmday2rtc(&a->time, &wake.time);

	if (a->enabled) {
		alm.alm_mask &= ~BD70528_MASK_ALM_EN;
		wake.ctrl |= BD70528_MASK_WAKE_EN;
	} else {
		alm.alm_mask |= BD70528_MASK_ALM_EN;
		wake.ctrl &= ~BD70528_MASK_WAKE_EN;
	}

	ret = regmap_bulk_write(r->regmap, BD70528_REG_RTC_WAKE_START, &wake,
				sizeof(wake));
	if (ret) {
		dev_err(dev, "Failed to set wake time\n");
		return ret;
	}
	ret = regmap_bulk_write(r->regmap, BD70528_REG_RTC_ALM_START, &alm,
				sizeof(alm));
	if (ret)
		dev_err(dev, "Failed to set alarm time\n");

	return ret;
}

static int bd71828_read_alarm(struct device *dev, struct rtc_wkalrm *a)
{
	int ret;
	struct bd71828_rtc_alm alm;
	struct bd70528_rtc *r = dev_get_drvdata(dev);

	ret = regmap_bulk_read(r->regmap, r->bd718xx_alm_block_start, &alm,
			       sizeof(alm));
	if (ret) {
		dev_err(dev, "Failed to read alarm regs\n");
		return ret;
	}

	rtc2tm(&alm.alm0, &a->time);
	a->time.tm_mday = -1;
	a->time.tm_mon = -1;
	a->time.tm_year = -1;
	a->enabled = !!(alm.alm_mask & BD70528_MASK_ALM_EN);
	a->pending = 0;

	return 0;
}

static int bd70528_read_alarm(struct device *dev, struct rtc_wkalrm *a)
{
	struct bd70528_rtc_alm alm;
	int ret;
	struct bd70528_rtc *r = dev_get_drvdata(dev);

	ret = regmap_bulk_read(r->regmap, BD70528_REG_RTC_ALM_START, &alm,
			       sizeof(alm));
	if (ret) {
		dev_err(dev, "Failed to read alarm regs\n");
		return ret;
	}

	rtc2tm(&alm.data, &a->time);
	a->time.tm_mday = -1;
	a->time.tm_mon = -1;
	a->time.tm_year = -1;
	a->enabled = !(alm.alm_mask & BD70528_MASK_ALM_EN);
	a->pending = 0;

	return 0;
}

static int bd70528_set_time_locked(struct device *dev, struct rtc_time *t)
{
	int ret, tmpret, old_states;
	struct bd70528_rtc_data rtc_data;
	struct bd70528_rtc *r = dev_get_drvdata(dev);

	ret = bd70528_disable_rtc_based_timers(r, &old_states);
	if (ret)
		return ret;

	tmpret = regmap_bulk_read(r->regmap, r->reg_time_start, &rtc_data,
				  sizeof(rtc_data));
	if (tmpret) {
		dev_err(dev, "Failed to read RTC time registers\n");
		goto renable_out;
	}
	tm2rtc(t, &rtc_data);

	tmpret = regmap_bulk_write(r->regmap, r->reg_time_start, &rtc_data,
				   sizeof(rtc_data));
	if (tmpret) {
		dev_err(dev, "Failed to set RTC time\n");
		goto renable_out;
	}

renable_out:
	ret = bd70528_re_enable_rtc_based_timers(r, old_states);
	if (tmpret)
		ret = tmpret;

	return ret;
}

static int bd71828_set_time(struct device *dev, struct rtc_time *t)
{
	return bd70528_set_time_locked(dev, t);
}

static int bd70528_set_time(struct device *dev, struct rtc_time *t)
{
	int ret;
	struct bd70528_rtc *r = dev_get_drvdata(dev);

	bd70528_wdt_lock(r->parent);
	ret = bd70528_set_time_locked(dev, t);
	bd70528_wdt_unlock(r->parent);
	return ret;
}

static int bd70528_get_time(struct device *dev, struct rtc_time *t)
{
	struct bd70528_rtc *r = dev_get_drvdata(dev);
	struct bd70528_rtc_data rtc_data;
	int ret;

	/* read the RTC date and time registers all at once */
	ret = regmap_bulk_read(r->regmap, r->reg_time_start, &rtc_data,
			       sizeof(rtc_data));
	if (ret) {
		dev_err(dev, "Failed to read RTC time (err %d)\n", ret);
		return ret;
	}

	rtc2tm(&rtc_data, t);

	return 0;
}

static int bd70528_alm_enable(struct device *dev, unsigned int enabled)
{
	int ret;
	unsigned int enableval = BD70528_MASK_ALM_EN;
	struct bd70528_rtc *r = dev_get_drvdata(dev);

	if (enabled)
		enableval = 0;

	bd70528_wdt_lock(r->parent);
	ret = bd70528_set_wake(r->parent, enabled, NULL);
	if (ret) {
		dev_err(dev, "Failed to change wake state\n");
		goto out_unlock;
	}
	ret = regmap_update_bits(r->regmap, BD70528_REG_RTC_ALM_MASK,
				 BD70528_MASK_ALM_EN, enableval);
	if (ret)
		dev_err(dev, "Failed to change alarm state\n");

out_unlock:
	bd70528_wdt_unlock(r->parent);
	return ret;
}

static int bd71828_alm_enable(struct device *dev, unsigned int enabled)
{
	int ret;
	struct bd70528_rtc *r = dev_get_drvdata(dev);
	unsigned int enableval = BD70528_MASK_ALM_EN;

	if (!enabled)
		enableval = 0;

	ret = regmap_update_bits(r->regmap, r->bd718xx_alm_block_start +
				 BD718XX_ALM_EN_OFFSET, BD70528_MASK_ALM_EN,
				 enableval);
	if (ret)
		dev_err(dev, "Failed to change alarm state\n");

	return ret;
}

static const struct rtc_class_ops bd70528_rtc_ops = {
	.read_time		= bd70528_get_time,
	.set_time		= bd70528_set_time,
	.read_alarm		= bd70528_read_alarm,
	.set_alarm		= bd70528_set_alarm,
	.alarm_irq_enable	= bd70528_alm_enable,
};

static const struct rtc_class_ops bd71828_rtc_ops = {
	.read_time		= bd70528_get_time,
	.set_time		= bd71828_set_time,
	.read_alarm		= bd71828_read_alarm,
	.set_alarm		= bd71828_set_alarm,
	.alarm_irq_enable	= bd71828_alm_enable,
};

static irqreturn_t alm_hndlr(int irq, void *data)
{
	struct rtc_device *rtc = data;

	rtc_update_irq(rtc, 1, RTC_IRQF | RTC_AF | RTC_PF);
	return IRQ_HANDLED;
}

static int bd70528_probe(struct platform_device *pdev)
{
	struct bd70528_rtc *bd_rtc;
	const struct rtc_class_ops *rtc_ops;
	const char *irq_name;
	int ret;
	struct rtc_device *rtc;
	int irq;
	unsigned int hr;
	bool enable_main_irq = false;
	u8 hour_reg;
	enum rohm_chip_type chip = platform_get_device_id(pdev)->driver_data;

	bd_rtc = devm_kzalloc(&pdev->dev, sizeof(*bd_rtc), GFP_KERNEL);
	if (!bd_rtc)
		return -ENOMEM;

	bd_rtc->regmap = dev_get_regmap(pdev->dev.parent, NULL);
	if (!bd_rtc->regmap) {
		dev_err(&pdev->dev, "No regmap\n");
		return -EINVAL;
	}

	bd_rtc->dev = &pdev->dev;

	switch (chip) {
	case ROHM_CHIP_TYPE_BD70528:
		bd_rtc->parent = dev_get_drvdata(pdev->dev.parent);
		if (!bd_rtc->parent) {
			dev_err(&pdev->dev, "No MFD data\n");
			return -EINVAL;
		}
		irq_name = "bd70528-rtc-alm";
		bd_rtc->has_rtc_timers = true;
		bd_rtc->reg_time_start = BD70528_REG_RTC_START;
		hour_reg = BD70528_REG_RTC_HOUR;
		enable_main_irq = true;
		rtc_ops = &bd70528_rtc_ops;
		break;
	case ROHM_CHIP_TYPE_BD71815:
		irq_name = "bd71815-rtc-alm-0";
		bd_rtc->reg_time_start = BD71815_REG_RTC_START;

		/*
		 * See also BD718XX_ALM_EN_OFFSET:
		 * This works for BD71828 and BD71815 as they have same offset
		 * between ALM0 start and ALM0_MASK. If new ICs are to be
		 * added this requires proper check as ALM0_MASK is not located
		 * at the end of ALM0 block - but after all ALM blocks so if
		 * amount of ALMs differ the offset to enable/disable is likely
		 * to be incorrect and enable/disable must be given as own
		 * reg address here.
		 */
		bd_rtc->bd718xx_alm_block_start = BD71815_REG_RTC_ALM_START;
		hour_reg = BD71815_REG_HOUR;
		rtc_ops = &bd71828_rtc_ops;
		break;
	case ROHM_CHIP_TYPE_BD71828:
		irq_name = "bd71828-rtc-alm-0";
		bd_rtc->reg_time_start = BD71828_REG_RTC_START;
		bd_rtc->bd718xx_alm_block_start = BD71828_REG_RTC_ALM_START;
		hour_reg = BD71828_REG_RTC_HOUR;
		rtc_ops = &bd71828_rtc_ops;
		break;
	default:
		dev_err(&pdev->dev, "Unknown chip\n");
		return -ENOENT;
	}

	irq = platform_get_irq_byname(pdev, irq_name);

	if (irq < 0)
		return irq;

	platform_set_drvdata(pdev, bd_rtc);

	ret = regmap_read(bd_rtc->regmap, hour_reg, &hr);

	if (ret) {
		dev_err(&pdev->dev, "Failed to reag RTC clock\n");
		return ret;
	}

	if (!(hr & BD70528_MASK_RTC_HOUR_24H)) {
		struct rtc_time t;

		ret = rtc_ops->read_time(&pdev->dev, &t);

		if (!ret)
			ret = rtc_ops->set_time(&pdev->dev, &t);

		if (ret) {
			dev_err(&pdev->dev,
				"Setting 24H clock for RTC failed\n");
			return ret;
		}
	}

	device_set_wakeup_capable(&pdev->dev, true);
	device_wakeup_enable(&pdev->dev);

	rtc = devm_rtc_allocate_device(&pdev->dev);
	if (IS_ERR(rtc)) {
		dev_err(&pdev->dev, "RTC device creation failed\n");
		return PTR_ERR(rtc);
	}

	rtc->range_min = RTC_TIMESTAMP_BEGIN_2000;
	rtc->range_max = RTC_TIMESTAMP_END_2099;
	rtc->ops = rtc_ops;

	/* Request alarm IRQ prior to registerig the RTC */
	ret = devm_request_threaded_irq(&pdev->dev, irq, NULL, &alm_hndlr,
					IRQF_ONESHOT, "bd70528-rtc", rtc);
	if (ret)
		return ret;

	/*
	 *  BD70528 irq controller is not touching the main mask register.
	 *  So enable the RTC block interrupts at main level. We can just
	 *  leave them enabled as irq-controller should disable irqs
	 *  from sub-registers when IRQ is disabled or freed.
	 */
	if (enable_main_irq) {
		ret = regmap_update_bits(bd_rtc->regmap,
				 BD70528_REG_INT_MAIN_MASK,
				 BD70528_INT_RTC_MASK, 0);
		if (ret) {
			dev_err(&pdev->dev, "Failed to enable RTC interrupts\n");
			return ret;
		}
	}

	return devm_rtc_register_device(rtc);
}

static const struct platform_device_id bd718x7_rtc_id[] = {
	{ "bd70528-rtc", ROHM_CHIP_TYPE_BD70528 },
	{ "bd71828-rtc", ROHM_CHIP_TYPE_BD71828 },
	{ "bd71815-rtc", ROHM_CHIP_TYPE_BD71815 },
	{ },
};
MODULE_DEVICE_TABLE(platform, bd718x7_rtc_id);

static struct platform_driver bd70528_rtc = {
	.driver = {
		.name = "bd70528-rtc"
	},
	.probe = bd70528_probe,
	.id_table = bd718x7_rtc_id,
};

module_platform_driver(bd70528_rtc);

MODULE_AUTHOR("Matti Vaittinen <matti.vaittinen@fi.rohmeurope.com>");
MODULE_DESCRIPTION("ROHM BD70528 and BD71828 PMIC RTC driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:bd70528-rtc");
