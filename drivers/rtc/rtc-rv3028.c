// SPDX-License-Identifier: GPL-2.0
/*
 * RTC driver for the Micro Crystal RV3028
 *
 * Copyright (C) 2019 Micro Crystal SA
 *
 * Alexandre Belloni <alexandre.belloni@bootlin.com>
 *
 */

#include <linux/clk-provider.h>
#include <linux/bcd.h>
#include <linux/bitops.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/log2.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/regmap.h>
#include <linux/rtc.h>

#define RV3028_SEC			0x00
#define RV3028_MIN			0x01
#define RV3028_HOUR			0x02
#define RV3028_WDAY			0x03
#define RV3028_DAY			0x04
#define RV3028_MONTH			0x05
#define RV3028_YEAR			0x06
#define RV3028_ALARM_MIN		0x07
#define RV3028_ALARM_HOUR		0x08
#define RV3028_ALARM_DAY		0x09
#define RV3028_STATUS			0x0E
#define RV3028_CTRL1			0x0F
#define RV3028_CTRL2			0x10
#define RV3028_EVT_CTRL			0x13
#define RV3028_TS_COUNT			0x14
#define RV3028_TS_SEC			0x15
#define RV3028_RAM1			0x1F
#define RV3028_EEPROM_ADDR		0x25
#define RV3028_EEPROM_DATA		0x26
#define RV3028_EEPROM_CMD		0x27
#define RV3028_CLKOUT			0x35
#define RV3028_OFFSET			0x36
#define RV3028_BACKUP			0x37

#define RV3028_STATUS_PORF		BIT(0)
#define RV3028_STATUS_EVF		BIT(1)
#define RV3028_STATUS_AF		BIT(2)
#define RV3028_STATUS_TF		BIT(3)
#define RV3028_STATUS_UF		BIT(4)
#define RV3028_STATUS_BSF		BIT(5)
#define RV3028_STATUS_CLKF		BIT(6)
#define RV3028_STATUS_EEBUSY		BIT(7)

#define RV3028_CLKOUT_FD_MASK		GENMASK(2, 0)
#define RV3028_CLKOUT_PORIE		BIT(3)
#define RV3028_CLKOUT_CLKSY		BIT(6)
#define RV3028_CLKOUT_CLKOE		BIT(7)

#define RV3028_CTRL1_EERD		BIT(3)
#define RV3028_CTRL1_WADA		BIT(5)

#define RV3028_CTRL2_RESET		BIT(0)
#define RV3028_CTRL2_12_24		BIT(1)
#define RV3028_CTRL2_EIE		BIT(2)
#define RV3028_CTRL2_AIE		BIT(3)
#define RV3028_CTRL2_TIE		BIT(4)
#define RV3028_CTRL2_UIE		BIT(5)
#define RV3028_CTRL2_TSE		BIT(7)

#define RV3028_EVT_CTRL_TSR		BIT(2)

#define RV3028_EEPROM_CMD_UPDATE	0x11
#define RV3028_EEPROM_CMD_WRITE		0x21
#define RV3028_EEPROM_CMD_READ		0x22

#define RV3028_EEBUSY_POLL		10000
#define RV3028_EEBUSY_TIMEOUT		100000

#define RV3028_BACKUP_TCE		BIT(5)
#define RV3028_BACKUP_TCR_MASK		GENMASK(1,0)

#define OFFSET_STEP_PPT			953674

enum rv3028_type {
	rv_3028,
};

struct rv3028_data {
	struct regmap *regmap;
	struct rtc_device *rtc;
	enum rv3028_type type;
#ifdef CONFIG_COMMON_CLK
	struct clk_hw clkout_hw;
#endif
};

static u16 rv3028_trickle_resistors[] = {3000, 5000, 9000, 15000};

static ssize_t timestamp0_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	struct rv3028_data *rv3028 = dev_get_drvdata(dev->parent);

	regmap_update_bits(rv3028->regmap, RV3028_EVT_CTRL, RV3028_EVT_CTRL_TSR,
			   RV3028_EVT_CTRL_TSR);

	return count;
};

static ssize_t timestamp0_show(struct device *dev,
			       struct device_attribute *attr, char *buf)
{
	struct rv3028_data *rv3028 = dev_get_drvdata(dev->parent);
	struct rtc_time tm;
	int ret, count;
	u8 date[6];

	ret = regmap_read(rv3028->regmap, RV3028_TS_COUNT, &count);
	if (ret)
		return ret;

	if (!count)
		return 0;

	ret = regmap_bulk_read(rv3028->regmap, RV3028_TS_SEC, date,
			       sizeof(date));
	if (ret)
		return ret;

	tm.tm_sec = bcd2bin(date[0]);
	tm.tm_min = bcd2bin(date[1]);
	tm.tm_hour = bcd2bin(date[2]);
	tm.tm_mday = bcd2bin(date[3]);
	tm.tm_mon = bcd2bin(date[4]) - 1;
	tm.tm_year = bcd2bin(date[5]) + 100;

	ret = rtc_valid_tm(&tm);
	if (ret)
		return ret;

	return sprintf(buf, "%llu\n",
		       (unsigned long long)rtc_tm_to_time64(&tm));
};

static DEVICE_ATTR_RW(timestamp0);

static ssize_t timestamp0_count_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct rv3028_data *rv3028 = dev_get_drvdata(dev->parent);
	int ret, count;

	ret = regmap_read(rv3028->regmap, RV3028_TS_COUNT, &count);
	if (ret)
		return ret;

	return sprintf(buf, "%u\n", count);
};

static DEVICE_ATTR_RO(timestamp0_count);

static struct attribute *rv3028_attrs[] = {
	&dev_attr_timestamp0.attr,
	&dev_attr_timestamp0_count.attr,
	NULL
};

static const struct attribute_group rv3028_attr_group = {
	.attrs	= rv3028_attrs,
};

static int rv3028_exit_eerd(struct rv3028_data *rv3028, u32 eerd)
{
	if (eerd)
		return 0;

	return regmap_update_bits(rv3028->regmap, RV3028_CTRL1, RV3028_CTRL1_EERD, 0);
}

static int rv3028_enter_eerd(struct rv3028_data *rv3028, u32 *eerd)
{
	u32 ctrl1, status;
	int ret;

	ret = regmap_read(rv3028->regmap, RV3028_CTRL1, &ctrl1);
	if (ret)
		return ret;

	*eerd = ctrl1 & RV3028_CTRL1_EERD;
	if (*eerd)
		return 0;

	ret = regmap_update_bits(rv3028->regmap, RV3028_CTRL1,
				 RV3028_CTRL1_EERD, RV3028_CTRL1_EERD);
	if (ret)
		return ret;

	ret = regmap_read_poll_timeout(rv3028->regmap, RV3028_STATUS, status,
				       !(status & RV3028_STATUS_EEBUSY),
				       RV3028_EEBUSY_POLL, RV3028_EEBUSY_TIMEOUT);
	if (ret) {
		rv3028_exit_eerd(rv3028, *eerd);

		return ret;
	}

	return 0;
}

static int rv3028_update_eeprom(struct rv3028_data *rv3028, u32 eerd)
{
	u32 status;
	int ret;

	ret = regmap_write(rv3028->regmap, RV3028_EEPROM_CMD, 0x0);
	if (ret)
		goto exit_eerd;

	ret = regmap_write(rv3028->regmap, RV3028_EEPROM_CMD, RV3028_EEPROM_CMD_UPDATE);
	if (ret)
		goto exit_eerd;

	usleep_range(63000, RV3028_EEBUSY_TIMEOUT);

	ret = regmap_read_poll_timeout(rv3028->regmap, RV3028_STATUS, status,
				       !(status & RV3028_STATUS_EEBUSY),
				       RV3028_EEBUSY_POLL, RV3028_EEBUSY_TIMEOUT);

exit_eerd:
	rv3028_exit_eerd(rv3028, eerd);

	return ret;
}

static int rv3028_update_cfg(struct rv3028_data *rv3028, unsigned int reg,
			     unsigned int mask, unsigned int val)
{
	u32 eerd;
	int ret;

	ret = rv3028_enter_eerd(rv3028, &eerd);
	if (ret)
		return ret;

	ret = regmap_update_bits(rv3028->regmap, reg, mask, val);
	if (ret) {
		rv3028_exit_eerd(rv3028, eerd);
		return ret;
	}

	return rv3028_update_eeprom(rv3028, eerd);
}

static irqreturn_t rv3028_handle_irq(int irq, void *dev_id)
{
	struct rv3028_data *rv3028 = dev_id;
	unsigned long events = 0;
	u32 status = 0, ctrl = 0;

	if (regmap_read(rv3028->regmap, RV3028_STATUS, &status) < 0 ||
	   status == 0) {
		return IRQ_NONE;
	}

	status &= ~RV3028_STATUS_PORF;

	if (status & RV3028_STATUS_TF) {
		status |= RV3028_STATUS_TF;
		ctrl |= RV3028_CTRL2_TIE;
		events |= RTC_PF;
	}

	if (status & RV3028_STATUS_AF) {
		status |= RV3028_STATUS_AF;
		ctrl |= RV3028_CTRL2_AIE;
		events |= RTC_AF;
	}

	if (status & RV3028_STATUS_UF) {
		status |= RV3028_STATUS_UF;
		ctrl |= RV3028_CTRL2_UIE;
		events |= RTC_UF;
	}

	if (events) {
		rtc_update_irq(rv3028->rtc, 1, events);
		regmap_update_bits(rv3028->regmap, RV3028_STATUS, status, 0);
		regmap_update_bits(rv3028->regmap, RV3028_CTRL2, ctrl, 0);
	}

	if (status & RV3028_STATUS_EVF) {
		sysfs_notify(&rv3028->rtc->dev.kobj, NULL,
			     dev_attr_timestamp0.attr.name);
		dev_warn(&rv3028->rtc->dev, "event detected");
	}

	return IRQ_HANDLED;
}

static int rv3028_get_time(struct device *dev, struct rtc_time *tm)
{
	struct rv3028_data *rv3028 = dev_get_drvdata(dev);
	u8 date[7];
	int ret, status;

	ret = regmap_read(rv3028->regmap, RV3028_STATUS, &status);
	if (ret < 0)
		return ret;

	if (status & RV3028_STATUS_PORF)
		return -EINVAL;

	ret = regmap_bulk_read(rv3028->regmap, RV3028_SEC, date, sizeof(date));
	if (ret)
		return ret;

	tm->tm_sec  = bcd2bin(date[RV3028_SEC] & 0x7f);
	tm->tm_min  = bcd2bin(date[RV3028_MIN] & 0x7f);
	tm->tm_hour = bcd2bin(date[RV3028_HOUR] & 0x3f);
	tm->tm_wday = date[RV3028_WDAY] & 0x7f;
	tm->tm_mday = bcd2bin(date[RV3028_DAY] & 0x3f);
	tm->tm_mon  = bcd2bin(date[RV3028_MONTH] & 0x1f) - 1;
	tm->tm_year = bcd2bin(date[RV3028_YEAR]) + 100;

	return 0;
}

static int rv3028_set_time(struct device *dev, struct rtc_time *tm)
{
	struct rv3028_data *rv3028 = dev_get_drvdata(dev);
	u8 date[7];
	int ret;

	date[RV3028_SEC]   = bin2bcd(tm->tm_sec);
	date[RV3028_MIN]   = bin2bcd(tm->tm_min);
	date[RV3028_HOUR]  = bin2bcd(tm->tm_hour);
	date[RV3028_WDAY]  = tm->tm_wday;
	date[RV3028_DAY]   = bin2bcd(tm->tm_mday);
	date[RV3028_MONTH] = bin2bcd(tm->tm_mon + 1);
	date[RV3028_YEAR]  = bin2bcd(tm->tm_year - 100);

	/*
	 * Writing to the Seconds register has the same effect as setting RESET
	 * bit to 1
	 */
	ret = regmap_bulk_write(rv3028->regmap, RV3028_SEC, date,
				sizeof(date));
	if (ret)
		return ret;

	ret = regmap_update_bits(rv3028->regmap, RV3028_STATUS,
				 RV3028_STATUS_PORF, 0);

	return ret;
}

static int rv3028_get_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct rv3028_data *rv3028 = dev_get_drvdata(dev);
	u8 alarmvals[3];
	int status, ctrl, ret;

	ret = regmap_bulk_read(rv3028->regmap, RV3028_ALARM_MIN, alarmvals,
			       sizeof(alarmvals));
	if (ret)
		return ret;

	ret = regmap_read(rv3028->regmap, RV3028_STATUS, &status);
	if (ret < 0)
		return ret;

	ret = regmap_read(rv3028->regmap, RV3028_CTRL2, &ctrl);
	if (ret < 0)
		return ret;

	alrm->time.tm_sec  = 0;
	alrm->time.tm_min  = bcd2bin(alarmvals[0] & 0x7f);
	alrm->time.tm_hour = bcd2bin(alarmvals[1] & 0x3f);
	alrm->time.tm_mday = bcd2bin(alarmvals[2] & 0x3f);

	alrm->enabled = !!(ctrl & RV3028_CTRL2_AIE);
	alrm->pending = (status & RV3028_STATUS_AF) && alrm->enabled;

	return 0;
}

static int rv3028_set_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct rv3028_data *rv3028 = dev_get_drvdata(dev);
	u8 alarmvals[3];
	u8 ctrl = 0;
	int ret;

	/* The alarm has no seconds, round up to nearest minute */
	if (alrm->time.tm_sec) {
		time64_t alarm_time = rtc_tm_to_time64(&alrm->time);

		alarm_time += 60 - alrm->time.tm_sec;
		rtc_time64_to_tm(alarm_time, &alrm->time);
	}

	ret = regmap_update_bits(rv3028->regmap, RV3028_CTRL2,
				 RV3028_CTRL2_AIE | RV3028_CTRL2_UIE, 0);
	if (ret)
		return ret;

	alarmvals[0] = bin2bcd(alrm->time.tm_min);
	alarmvals[1] = bin2bcd(alrm->time.tm_hour);
	alarmvals[2] = bin2bcd(alrm->time.tm_mday);

	ret = regmap_update_bits(rv3028->regmap, RV3028_STATUS,
				 RV3028_STATUS_AF, 0);
	if (ret)
		return ret;

	ret = regmap_bulk_write(rv3028->regmap, RV3028_ALARM_MIN, alarmvals,
				sizeof(alarmvals));
	if (ret)
		return ret;

	if (alrm->enabled) {
		if (rv3028->rtc->uie_rtctimer.enabled)
			ctrl |= RV3028_CTRL2_UIE;
		if (rv3028->rtc->aie_timer.enabled)
			ctrl |= RV3028_CTRL2_AIE;
	}

	ret = regmap_update_bits(rv3028->regmap, RV3028_CTRL2,
				 RV3028_CTRL2_UIE | RV3028_CTRL2_AIE, ctrl);

	return ret;
}

static int rv3028_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	struct rv3028_data *rv3028 = dev_get_drvdata(dev);
	int ctrl = 0, ret;

	if (enabled) {
		if (rv3028->rtc->uie_rtctimer.enabled)
			ctrl |= RV3028_CTRL2_UIE;
		if (rv3028->rtc->aie_timer.enabled)
			ctrl |= RV3028_CTRL2_AIE;
	}

	ret = regmap_update_bits(rv3028->regmap, RV3028_STATUS,
				 RV3028_STATUS_AF | RV3028_STATUS_UF, 0);
	if (ret)
		return ret;

	ret = regmap_update_bits(rv3028->regmap, RV3028_CTRL2,
				 RV3028_CTRL2_UIE | RV3028_CTRL2_AIE, ctrl);
	if (ret)
		return ret;

	return 0;
}

static int rv3028_read_offset(struct device *dev, long *offset)
{
	struct rv3028_data *rv3028 = dev_get_drvdata(dev);
	int ret, value, steps;

	ret = regmap_read(rv3028->regmap, RV3028_OFFSET, &value);
	if (ret < 0)
		return ret;

	steps = sign_extend32(value << 1, 8);

	ret = regmap_read(rv3028->regmap, RV3028_BACKUP, &value);
	if (ret < 0)
		return ret;

	steps += value >> 7;

	*offset = DIV_ROUND_CLOSEST(steps * OFFSET_STEP_PPT, 1000);

	return 0;
}

static int rv3028_set_offset(struct device *dev, long offset)
{
	struct rv3028_data *rv3028 = dev_get_drvdata(dev);
	u32 eerd;
	int ret;

	offset = clamp(offset, -244141L, 243187L) * 1000;
	offset = DIV_ROUND_CLOSEST(offset, OFFSET_STEP_PPT);

	ret = rv3028_enter_eerd(rv3028, &eerd);
	if (ret)
		return ret;

	ret = regmap_write(rv3028->regmap, RV3028_OFFSET, offset >> 1);
	if (ret < 0)
		goto exit_eerd;

	ret = regmap_update_bits(rv3028->regmap, RV3028_BACKUP, BIT(7),
				 offset << 7);
	if (ret < 0)
		goto exit_eerd;

	return rv3028_update_eeprom(rv3028, eerd);

exit_eerd:
	rv3028_exit_eerd(rv3028, eerd);

	return ret;

}

static int rv3028_ioctl(struct device *dev, unsigned int cmd, unsigned long arg)
{
	struct rv3028_data *rv3028 = dev_get_drvdata(dev);
	int status, ret = 0;

	switch (cmd) {
	case RTC_VL_READ:
		ret = regmap_read(rv3028->regmap, RV3028_STATUS, &status);
		if (ret < 0)
			return ret;

		status = status & RV3028_STATUS_PORF ? RTC_VL_DATA_INVALID : 0;
		return put_user(status, (unsigned int __user *)arg);

	default:
		return -ENOIOCTLCMD;
	}
}

static int rv3028_nvram_write(void *priv, unsigned int offset, void *val,
			      size_t bytes)
{
	return regmap_bulk_write(priv, RV3028_RAM1 + offset, val, bytes);
}

static int rv3028_nvram_read(void *priv, unsigned int offset, void *val,
			     size_t bytes)
{
	return regmap_bulk_read(priv, RV3028_RAM1 + offset, val, bytes);
}

static int rv3028_eeprom_write(void *priv, unsigned int offset, void *val,
			       size_t bytes)
{
	struct rv3028_data *rv3028 = priv;
	u32 status, eerd;
	int i, ret;
	u8 *buf = val;

	ret = rv3028_enter_eerd(rv3028, &eerd);
	if (ret)
		return ret;

	for (i = 0; i < bytes; i++) {
		ret = regmap_write(rv3028->regmap, RV3028_EEPROM_ADDR, offset + i);
		if (ret)
			goto restore_eerd;

		ret = regmap_write(rv3028->regmap, RV3028_EEPROM_DATA, buf[i]);
		if (ret)
			goto restore_eerd;

		ret = regmap_write(rv3028->regmap, RV3028_EEPROM_CMD, 0x0);
		if (ret)
			goto restore_eerd;

		ret = regmap_write(rv3028->regmap, RV3028_EEPROM_CMD,
				   RV3028_EEPROM_CMD_WRITE);
		if (ret)
			goto restore_eerd;

		usleep_range(RV3028_EEBUSY_POLL, RV3028_EEBUSY_TIMEOUT);

		ret = regmap_read_poll_timeout(rv3028->regmap, RV3028_STATUS, status,
					       !(status & RV3028_STATUS_EEBUSY),
					       RV3028_EEBUSY_POLL,
					       RV3028_EEBUSY_TIMEOUT);
		if (ret)
			goto restore_eerd;
	}

restore_eerd:
	rv3028_exit_eerd(rv3028, eerd);

	return ret;
}

static int rv3028_eeprom_read(void *priv, unsigned int offset, void *val,
			      size_t bytes)
{
	struct rv3028_data *rv3028 = priv;
	u32 status, eerd, data;
	int i, ret;
	u8 *buf = val;

	ret = rv3028_enter_eerd(rv3028, &eerd);
	if (ret)
		return ret;

	for (i = 0; i < bytes; i++) {
		ret = regmap_write(rv3028->regmap, RV3028_EEPROM_ADDR, offset + i);
		if (ret)
			goto restore_eerd;

		ret = regmap_write(rv3028->regmap, RV3028_EEPROM_CMD, 0x0);
		if (ret)
			goto restore_eerd;

		ret = regmap_write(rv3028->regmap, RV3028_EEPROM_CMD,
				   RV3028_EEPROM_CMD_READ);
		if (ret)
			goto restore_eerd;

		ret = regmap_read_poll_timeout(rv3028->regmap, RV3028_STATUS, status,
					       !(status & RV3028_STATUS_EEBUSY),
					       RV3028_EEBUSY_POLL,
					       RV3028_EEBUSY_TIMEOUT);
		if (ret)
			goto restore_eerd;

		ret = regmap_read(rv3028->regmap, RV3028_EEPROM_DATA, &data);
		if (ret)
			goto restore_eerd;
		buf[i] = data;
	}

restore_eerd:
	rv3028_exit_eerd(rv3028, eerd);

	return ret;
}

#ifdef CONFIG_COMMON_CLK
#define clkout_hw_to_rv3028(hw) container_of(hw, struct rv3028_data, clkout_hw)

static int clkout_rates[] = {
	32768,
	8192,
	1024,
	64,
	32,
	1,
};

static unsigned long rv3028_clkout_recalc_rate(struct clk_hw *hw,
					       unsigned long parent_rate)
{
	int clkout, ret;
	struct rv3028_data *rv3028 = clkout_hw_to_rv3028(hw);

	ret = regmap_read(rv3028->regmap, RV3028_CLKOUT, &clkout);
	if (ret < 0)
		return 0;

	clkout &= RV3028_CLKOUT_FD_MASK;
	return clkout_rates[clkout];
}

static long rv3028_clkout_round_rate(struct clk_hw *hw, unsigned long rate,
				     unsigned long *prate)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(clkout_rates); i++)
		if (clkout_rates[i] <= rate)
			return clkout_rates[i];

	return 0;
}

static int rv3028_clkout_set_rate(struct clk_hw *hw, unsigned long rate,
				  unsigned long parent_rate)
{
	int i, ret;
	u32 enabled;
	struct rv3028_data *rv3028 = clkout_hw_to_rv3028(hw);

	ret = regmap_read(rv3028->regmap, RV3028_CLKOUT, &enabled);
	if (ret < 0)
		return ret;

	ret = regmap_write(rv3028->regmap, RV3028_CLKOUT, 0x0);
	if (ret < 0)
		return ret;

	enabled &= RV3028_CLKOUT_CLKOE;

	for (i = 0; i < ARRAY_SIZE(clkout_rates); i++)
		if (clkout_rates[i] == rate)
			return rv3028_update_cfg(rv3028, RV3028_CLKOUT, 0xff,
						 RV3028_CLKOUT_CLKSY | enabled | i);

	return -EINVAL;
}

static int rv3028_clkout_prepare(struct clk_hw *hw)
{
	struct rv3028_data *rv3028 = clkout_hw_to_rv3028(hw);

	return regmap_write(rv3028->regmap, RV3028_CLKOUT,
			    RV3028_CLKOUT_CLKSY | RV3028_CLKOUT_CLKOE);
}

static void rv3028_clkout_unprepare(struct clk_hw *hw)
{
	struct rv3028_data *rv3028 = clkout_hw_to_rv3028(hw);

	regmap_write(rv3028->regmap, RV3028_CLKOUT, 0x0);
	regmap_update_bits(rv3028->regmap, RV3028_STATUS,
			   RV3028_STATUS_CLKF, 0);
}

static int rv3028_clkout_is_prepared(struct clk_hw *hw)
{
	int clkout, ret;
	struct rv3028_data *rv3028 = clkout_hw_to_rv3028(hw);

	ret = regmap_read(rv3028->regmap, RV3028_CLKOUT, &clkout);
	if (ret < 0)
		return ret;

	return !!(clkout & RV3028_CLKOUT_CLKOE);
}

static const struct clk_ops rv3028_clkout_ops = {
	.prepare = rv3028_clkout_prepare,
	.unprepare = rv3028_clkout_unprepare,
	.is_prepared = rv3028_clkout_is_prepared,
	.recalc_rate = rv3028_clkout_recalc_rate,
	.round_rate = rv3028_clkout_round_rate,
	.set_rate = rv3028_clkout_set_rate,
};

static int rv3028_clkout_register_clk(struct rv3028_data *rv3028,
				      struct i2c_client *client)
{
	int ret;
	struct clk *clk;
	struct clk_init_data init;
	struct device_node *node = client->dev.of_node;

	ret = regmap_update_bits(rv3028->regmap, RV3028_STATUS,
				 RV3028_STATUS_CLKF, 0);
	if (ret < 0)
		return ret;

	init.name = "rv3028-clkout";
	init.ops = &rv3028_clkout_ops;
	init.flags = 0;
	init.parent_names = NULL;
	init.num_parents = 0;
	rv3028->clkout_hw.init = &init;

	/* optional override of the clockname */
	of_property_read_string(node, "clock-output-names", &init.name);

	/* register the clock */
	clk = devm_clk_register(&client->dev, &rv3028->clkout_hw);
	if (!IS_ERR(clk))
		of_clk_add_provider(node, of_clk_src_simple_get, clk);

	return 0;
}
#endif

static const struct rtc_class_ops rv3028_rtc_ops = {
	.read_time = rv3028_get_time,
	.set_time = rv3028_set_time,
	.read_alarm = rv3028_get_alarm,
	.set_alarm = rv3028_set_alarm,
	.alarm_irq_enable = rv3028_alarm_irq_enable,
	.read_offset = rv3028_read_offset,
	.set_offset = rv3028_set_offset,
	.ioctl = rv3028_ioctl,
};

static const struct regmap_config regmap_config = {
        .reg_bits = 8,
        .val_bits = 8,
        .max_register = 0x37,
};

static int rv3028_probe(struct i2c_client *client)
{
	struct rv3028_data *rv3028;
	int ret, status;
	u32 ohms;
	struct nvmem_config nvmem_cfg = {
		.name = "rv3028_nvram",
		.word_size = 1,
		.stride = 1,
		.size = 2,
		.type = NVMEM_TYPE_BATTERY_BACKED,
		.reg_read = rv3028_nvram_read,
		.reg_write = rv3028_nvram_write,
	};
	struct nvmem_config eeprom_cfg = {
		.name = "rv3028_eeprom",
		.word_size = 1,
		.stride = 1,
		.size = 43,
		.type = NVMEM_TYPE_EEPROM,
		.reg_read = rv3028_eeprom_read,
		.reg_write = rv3028_eeprom_write,
	};

	rv3028 = devm_kzalloc(&client->dev, sizeof(struct rv3028_data),
			      GFP_KERNEL);
	if (!rv3028)
		return -ENOMEM;

	rv3028->regmap = devm_regmap_init_i2c(client, &regmap_config);
	if (IS_ERR(rv3028->regmap))
		return PTR_ERR(rv3028->regmap);

	i2c_set_clientdata(client, rv3028);

	ret = regmap_read(rv3028->regmap, RV3028_STATUS, &status);
	if (ret < 0)
		return ret;

	if (status & RV3028_STATUS_AF)
		dev_warn(&client->dev, "An alarm may have been missed.\n");

	rv3028->rtc = devm_rtc_allocate_device(&client->dev);
	if (IS_ERR(rv3028->rtc))
		return PTR_ERR(rv3028->rtc);

	if (client->irq > 0) {
		ret = devm_request_threaded_irq(&client->dev, client->irq,
						NULL, rv3028_handle_irq,
						IRQF_TRIGGER_LOW | IRQF_ONESHOT,
						"rv3028", rv3028);
		if (ret) {
			dev_warn(&client->dev, "unable to request IRQ, alarms disabled\n");
			client->irq = 0;
		}
	}
	if (!client->irq)
		clear_bit(RTC_FEATURE_ALARM, rv3028->rtc->features);

	ret = regmap_update_bits(rv3028->regmap, RV3028_CTRL1,
				 RV3028_CTRL1_WADA, RV3028_CTRL1_WADA);
	if (ret)
		return ret;

	/* setup timestamping */
	ret = regmap_update_bits(rv3028->regmap, RV3028_CTRL2,
				 RV3028_CTRL2_EIE | RV3028_CTRL2_TSE,
				 RV3028_CTRL2_EIE | RV3028_CTRL2_TSE);
	if (ret)
		return ret;

	/* setup trickle charger */
	if (!device_property_read_u32(&client->dev, "trickle-resistor-ohms",
				      &ohms)) {
		int i;

		for (i = 0; i < ARRAY_SIZE(rv3028_trickle_resistors); i++)
			if (ohms == rv3028_trickle_resistors[i])
				break;

		if (i < ARRAY_SIZE(rv3028_trickle_resistors)) {
			ret = rv3028_update_cfg(rv3028, RV3028_BACKUP, RV3028_BACKUP_TCE |
						 RV3028_BACKUP_TCR_MASK, RV3028_BACKUP_TCE | i);
			if (ret)
				return ret;
		} else {
			dev_warn(&client->dev, "invalid trickle resistor value\n");
		}
	}

	ret = rtc_add_group(rv3028->rtc, &rv3028_attr_group);
	if (ret)
		return ret;

	rv3028->rtc->range_min = RTC_TIMESTAMP_BEGIN_2000;
	rv3028->rtc->range_max = RTC_TIMESTAMP_END_2099;
	rv3028->rtc->ops = &rv3028_rtc_ops;
	ret = devm_rtc_register_device(rv3028->rtc);
	if (ret)
		return ret;

	nvmem_cfg.priv = rv3028->regmap;
	devm_rtc_nvmem_register(rv3028->rtc, &nvmem_cfg);
	eeprom_cfg.priv = rv3028;
	devm_rtc_nvmem_register(rv3028->rtc, &eeprom_cfg);

	rv3028->rtc->max_user_freq = 1;

#ifdef CONFIG_COMMON_CLK
	rv3028_clkout_register_clk(rv3028, client);
#endif
	return 0;
}

static const __maybe_unused struct of_device_id rv3028_of_match[] = {
	{ .compatible = "microcrystal,rv3028", },
	{ }
};
MODULE_DEVICE_TABLE(of, rv3028_of_match);

static struct i2c_driver rv3028_driver = {
	.driver = {
		.name = "rtc-rv3028",
		.of_match_table = of_match_ptr(rv3028_of_match),
	},
	.probe_new	= rv3028_probe,
};
module_i2c_driver(rv3028_driver);

MODULE_AUTHOR("Alexandre Belloni <alexandre.belloni@bootlin.com>");
MODULE_DESCRIPTION("Micro Crystal RV3028 RTC driver");
MODULE_LICENSE("GPL v2");
