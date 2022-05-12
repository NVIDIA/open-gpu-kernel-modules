// SPDX-License-Identifier: GPL-2.0-only
/*
 * rtc-ds1307.c - RTC driver for some mostly-compatible I2C chips.
 *
 *  Copyright (C) 2005 James Chapman (ds1337 core)
 *  Copyright (C) 2006 David Brownell
 *  Copyright (C) 2009 Matthias Fuchs (rx8025 support)
 *  Copyright (C) 2012 Bertrand Achard (nvram access fixes)
 */

#include <linux/bcd.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/property.h>
#include <linux/rtc/ds1307.h>
#include <linux/rtc.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/clk-provider.h>
#include <linux/regmap.h>
#include <linux/watchdog.h>

/*
 * We can't determine type by probing, but if we expect pre-Linux code
 * to have set the chip up as a clock (turning on the oscillator and
 * setting the date and time), Linux can ignore the non-clock features.
 * That's a natural job for a factory or repair bench.
 */
enum ds_type {
	unknown_ds_type, /* always first and 0 */
	ds_1307,
	ds_1308,
	ds_1337,
	ds_1338,
	ds_1339,
	ds_1340,
	ds_1341,
	ds_1388,
	ds_3231,
	m41t0,
	m41t00,
	m41t11,
	mcp794xx,
	rx_8025,
	rx_8130,
	last_ds_type /* always last */
	/* rs5c372 too?  different address... */
};

/* RTC registers don't differ much, except for the century flag */
#define DS1307_REG_SECS		0x00	/* 00-59 */
#	define DS1307_BIT_CH		0x80
#	define DS1340_BIT_nEOSC		0x80
#	define MCP794XX_BIT_ST		0x80
#define DS1307_REG_MIN		0x01	/* 00-59 */
#	define M41T0_BIT_OF		0x80
#define DS1307_REG_HOUR		0x02	/* 00-23, or 1-12{am,pm} */
#	define DS1307_BIT_12HR		0x40	/* in REG_HOUR */
#	define DS1307_BIT_PM		0x20	/* in REG_HOUR */
#	define DS1340_BIT_CENTURY_EN	0x80	/* in REG_HOUR */
#	define DS1340_BIT_CENTURY	0x40	/* in REG_HOUR */
#define DS1307_REG_WDAY		0x03	/* 01-07 */
#	define MCP794XX_BIT_VBATEN	0x08
#define DS1307_REG_MDAY		0x04	/* 01-31 */
#define DS1307_REG_MONTH	0x05	/* 01-12 */
#	define DS1337_BIT_CENTURY	0x80	/* in REG_MONTH */
#define DS1307_REG_YEAR		0x06	/* 00-99 */

/*
 * Other registers (control, status, alarms, trickle charge, NVRAM, etc)
 * start at 7, and they differ a LOT. Only control and status matter for
 * basic RTC date and time functionality; be careful using them.
 */
#define DS1307_REG_CONTROL	0x07		/* or ds1338 */
#	define DS1307_BIT_OUT		0x80
#	define DS1338_BIT_OSF		0x20
#	define DS1307_BIT_SQWE		0x10
#	define DS1307_BIT_RS1		0x02
#	define DS1307_BIT_RS0		0x01
#define DS1337_REG_CONTROL	0x0e
#	define DS1337_BIT_nEOSC		0x80
#	define DS1339_BIT_BBSQI		0x20
#	define DS3231_BIT_BBSQW		0x40 /* same as BBSQI */
#	define DS1337_BIT_RS2		0x10
#	define DS1337_BIT_RS1		0x08
#	define DS1337_BIT_INTCN		0x04
#	define DS1337_BIT_A2IE		0x02
#	define DS1337_BIT_A1IE		0x01
#define DS1340_REG_CONTROL	0x07
#	define DS1340_BIT_OUT		0x80
#	define DS1340_BIT_FT		0x40
#	define DS1340_BIT_CALIB_SIGN	0x20
#	define DS1340_M_CALIBRATION	0x1f
#define DS1340_REG_FLAG		0x09
#	define DS1340_BIT_OSF		0x80
#define DS1337_REG_STATUS	0x0f
#	define DS1337_BIT_OSF		0x80
#	define DS3231_BIT_EN32KHZ	0x08
#	define DS1337_BIT_A2I		0x02
#	define DS1337_BIT_A1I		0x01
#define DS1339_REG_ALARM1_SECS	0x07

#define DS13XX_TRICKLE_CHARGER_MAGIC	0xa0

#define RX8025_REG_CTRL1	0x0e
#	define RX8025_BIT_2412		0x20
#define RX8025_REG_CTRL2	0x0f
#	define RX8025_BIT_PON		0x10
#	define RX8025_BIT_VDET		0x40
#	define RX8025_BIT_XST		0x20

#define RX8130_REG_ALARM_MIN		0x17
#define RX8130_REG_ALARM_HOUR		0x18
#define RX8130_REG_ALARM_WEEK_OR_DAY	0x19
#define RX8130_REG_EXTENSION		0x1c
#define RX8130_REG_EXTENSION_WADA	BIT(3)
#define RX8130_REG_FLAG			0x1d
#define RX8130_REG_FLAG_VLF		BIT(1)
#define RX8130_REG_FLAG_AF		BIT(3)
#define RX8130_REG_CONTROL0		0x1e
#define RX8130_REG_CONTROL0_AIE		BIT(3)
#define RX8130_REG_CONTROL1		0x1f
#define RX8130_REG_CONTROL1_INIEN	BIT(4)
#define RX8130_REG_CONTROL1_CHGEN	BIT(5)

#define MCP794XX_REG_CONTROL		0x07
#	define MCP794XX_BIT_ALM0_EN	0x10
#	define MCP794XX_BIT_ALM1_EN	0x20
#define MCP794XX_REG_ALARM0_BASE	0x0a
#define MCP794XX_REG_ALARM0_CTRL	0x0d
#define MCP794XX_REG_ALARM1_BASE	0x11
#define MCP794XX_REG_ALARM1_CTRL	0x14
#	define MCP794XX_BIT_ALMX_IF	BIT(3)
#	define MCP794XX_BIT_ALMX_C0	BIT(4)
#	define MCP794XX_BIT_ALMX_C1	BIT(5)
#	define MCP794XX_BIT_ALMX_C2	BIT(6)
#	define MCP794XX_BIT_ALMX_POL	BIT(7)
#	define MCP794XX_MSK_ALMX_MATCH	(MCP794XX_BIT_ALMX_C0 | \
					 MCP794XX_BIT_ALMX_C1 | \
					 MCP794XX_BIT_ALMX_C2)

#define M41TXX_REG_CONTROL	0x07
#	define M41TXX_BIT_OUT		BIT(7)
#	define M41TXX_BIT_FT		BIT(6)
#	define M41TXX_BIT_CALIB_SIGN	BIT(5)
#	define M41TXX_M_CALIBRATION	GENMASK(4, 0)

#define DS1388_REG_WDOG_HUN_SECS	0x08
#define DS1388_REG_WDOG_SECS		0x09
#define DS1388_REG_FLAG			0x0b
#	define DS1388_BIT_WF		BIT(6)
#	define DS1388_BIT_OSF		BIT(7)
#define DS1388_REG_CONTROL		0x0c
#	define DS1388_BIT_RST		BIT(0)
#	define DS1388_BIT_WDE		BIT(1)
#	define DS1388_BIT_nEOSC		BIT(7)

/* negative offset step is -2.034ppm */
#define M41TXX_NEG_OFFSET_STEP_PPB	2034
/* positive offset step is +4.068ppm */
#define M41TXX_POS_OFFSET_STEP_PPB	4068
/* Min and max values supported with 'offset' interface by M41TXX */
#define M41TXX_MIN_OFFSET	((-31) * M41TXX_NEG_OFFSET_STEP_PPB)
#define M41TXX_MAX_OFFSET	((31) * M41TXX_POS_OFFSET_STEP_PPB)

struct ds1307 {
	enum ds_type		type;
	struct device		*dev;
	struct regmap		*regmap;
	const char		*name;
	struct rtc_device	*rtc;
#ifdef CONFIG_COMMON_CLK
	struct clk_hw		clks[2];
#endif
};

struct chip_desc {
	unsigned		alarm:1;
	u16			nvram_offset;
	u16			nvram_size;
	u8			offset; /* register's offset */
	u8			century_reg;
	u8			century_enable_bit;
	u8			century_bit;
	u8			bbsqi_bit;
	irq_handler_t		irq_handler;
	const struct rtc_class_ops *rtc_ops;
	u16			trickle_charger_reg;
	u8			(*do_trickle_setup)(struct ds1307 *, u32,
						    bool);
	/* Does the RTC require trickle-resistor-ohms to select the value of
	 * the resistor between Vcc and Vbackup?
	 */
	bool			requires_trickle_resistor;
	/* Some RTC's batteries and supercaps were charged by default, others
	 * allow charging but were not configured previously to do so.
	 * Remember this behavior to stay backwards compatible.
	 */
	bool			charge_default;
};

static const struct chip_desc chips[last_ds_type];

static int ds1307_get_time(struct device *dev, struct rtc_time *t)
{
	struct ds1307	*ds1307 = dev_get_drvdata(dev);
	int		tmp, ret;
	const struct chip_desc *chip = &chips[ds1307->type];
	u8 regs[7];

	if (ds1307->type == rx_8130) {
		unsigned int regflag;
		ret = regmap_read(ds1307->regmap, RX8130_REG_FLAG, &regflag);
		if (ret) {
			dev_err(dev, "%s error %d\n", "read", ret);
			return ret;
		}

		if (regflag & RX8130_REG_FLAG_VLF) {
			dev_warn_once(dev, "oscillator failed, set time!\n");
			return -EINVAL;
		}
	}

	/* read the RTC date and time registers all at once */
	ret = regmap_bulk_read(ds1307->regmap, chip->offset, regs,
			       sizeof(regs));
	if (ret) {
		dev_err(dev, "%s error %d\n", "read", ret);
		return ret;
	}

	dev_dbg(dev, "%s: %7ph\n", "read", regs);

	/* if oscillator fail bit is set, no data can be trusted */
	if (ds1307->type == m41t0 &&
	    regs[DS1307_REG_MIN] & M41T0_BIT_OF) {
		dev_warn_once(dev, "oscillator failed, set time!\n");
		return -EINVAL;
	}

	tmp = regs[DS1307_REG_SECS];
	switch (ds1307->type) {
	case ds_1307:
	case m41t0:
	case m41t00:
	case m41t11:
		if (tmp & DS1307_BIT_CH)
			return -EINVAL;
		break;
	case ds_1308:
	case ds_1338:
		if (tmp & DS1307_BIT_CH)
			return -EINVAL;

		ret = regmap_read(ds1307->regmap, DS1307_REG_CONTROL, &tmp);
		if (ret)
			return ret;
		if (tmp & DS1338_BIT_OSF)
			return -EINVAL;
		break;
	case ds_1340:
		if (tmp & DS1340_BIT_nEOSC)
			return -EINVAL;

		ret = regmap_read(ds1307->regmap, DS1340_REG_FLAG, &tmp);
		if (ret)
			return ret;
		if (tmp & DS1340_BIT_OSF)
			return -EINVAL;
		break;
	case ds_1388:
		ret = regmap_read(ds1307->regmap, DS1388_REG_FLAG, &tmp);
		if (ret)
			return ret;
		if (tmp & DS1388_BIT_OSF)
			return -EINVAL;
		break;
	case mcp794xx:
		if (!(tmp & MCP794XX_BIT_ST))
			return -EINVAL;

		break;
	default:
		break;
	}

	t->tm_sec = bcd2bin(regs[DS1307_REG_SECS] & 0x7f);
	t->tm_min = bcd2bin(regs[DS1307_REG_MIN] & 0x7f);
	tmp = regs[DS1307_REG_HOUR] & 0x3f;
	t->tm_hour = bcd2bin(tmp);
	/* rx8130 is bit position, not BCD */
	if (ds1307->type == rx_8130)
		t->tm_wday = fls(regs[DS1307_REG_WDAY] & 0x7f);
	else
		t->tm_wday = bcd2bin(regs[DS1307_REG_WDAY] & 0x07) - 1;
	t->tm_mday = bcd2bin(regs[DS1307_REG_MDAY] & 0x3f);
	tmp = regs[DS1307_REG_MONTH] & 0x1f;
	t->tm_mon = bcd2bin(tmp) - 1;
	t->tm_year = bcd2bin(regs[DS1307_REG_YEAR]) + 100;

	if (regs[chip->century_reg] & chip->century_bit &&
	    IS_ENABLED(CONFIG_RTC_DRV_DS1307_CENTURY))
		t->tm_year += 100;

	dev_dbg(dev, "%s secs=%d, mins=%d, "
		"hours=%d, mday=%d, mon=%d, year=%d, wday=%d\n",
		"read", t->tm_sec, t->tm_min,
		t->tm_hour, t->tm_mday,
		t->tm_mon, t->tm_year, t->tm_wday);

	return 0;
}

static int ds1307_set_time(struct device *dev, struct rtc_time *t)
{
	struct ds1307	*ds1307 = dev_get_drvdata(dev);
	const struct chip_desc *chip = &chips[ds1307->type];
	int		result;
	int		tmp;
	u8		regs[7];

	dev_dbg(dev, "%s secs=%d, mins=%d, "
		"hours=%d, mday=%d, mon=%d, year=%d, wday=%d\n",
		"write", t->tm_sec, t->tm_min,
		t->tm_hour, t->tm_mday,
		t->tm_mon, t->tm_year, t->tm_wday);

	if (t->tm_year < 100)
		return -EINVAL;

#ifdef CONFIG_RTC_DRV_DS1307_CENTURY
	if (t->tm_year > (chip->century_bit ? 299 : 199))
		return -EINVAL;
#else
	if (t->tm_year > 199)
		return -EINVAL;
#endif

	regs[DS1307_REG_SECS] = bin2bcd(t->tm_sec);
	regs[DS1307_REG_MIN] = bin2bcd(t->tm_min);
	regs[DS1307_REG_HOUR] = bin2bcd(t->tm_hour);
	/* rx8130 is bit position, not BCD */
	if (ds1307->type == rx_8130)
		regs[DS1307_REG_WDAY] = 1 << t->tm_wday;
	else
		regs[DS1307_REG_WDAY] = bin2bcd(t->tm_wday + 1);
	regs[DS1307_REG_MDAY] = bin2bcd(t->tm_mday);
	regs[DS1307_REG_MONTH] = bin2bcd(t->tm_mon + 1);

	/* assume 20YY not 19YY */
	tmp = t->tm_year - 100;
	regs[DS1307_REG_YEAR] = bin2bcd(tmp);

	if (chip->century_enable_bit)
		regs[chip->century_reg] |= chip->century_enable_bit;
	if (t->tm_year > 199 && chip->century_bit)
		regs[chip->century_reg] |= chip->century_bit;

	switch (ds1307->type) {
	case ds_1308:
	case ds_1338:
		regmap_update_bits(ds1307->regmap, DS1307_REG_CONTROL,
				   DS1338_BIT_OSF, 0);
		break;
	case ds_1340:
		regmap_update_bits(ds1307->regmap, DS1340_REG_FLAG,
				   DS1340_BIT_OSF, 0);
		break;
	case ds_1388:
		regmap_update_bits(ds1307->regmap, DS1388_REG_FLAG,
				   DS1388_BIT_OSF, 0);
		break;
	case mcp794xx:
		/*
		 * these bits were cleared when preparing the date/time
		 * values and need to be set again before writing the
		 * regsfer out to the device.
		 */
		regs[DS1307_REG_SECS] |= MCP794XX_BIT_ST;
		regs[DS1307_REG_WDAY] |= MCP794XX_BIT_VBATEN;
		break;
	default:
		break;
	}

	dev_dbg(dev, "%s: %7ph\n", "write", regs);

	result = regmap_bulk_write(ds1307->regmap, chip->offset, regs,
				   sizeof(regs));
	if (result) {
		dev_err(dev, "%s error %d\n", "write", result);
		return result;
	}

	if (ds1307->type == rx_8130) {
		/* clear Voltage Loss Flag as data is available now */
		result = regmap_write(ds1307->regmap, RX8130_REG_FLAG,
				      ~(u8)RX8130_REG_FLAG_VLF);
		if (result) {
			dev_err(dev, "%s error %d\n", "write", result);
			return result;
		}
	}

	return 0;
}

static int ds1337_read_alarm(struct device *dev, struct rtc_wkalrm *t)
{
	struct ds1307		*ds1307 = dev_get_drvdata(dev);
	int			ret;
	u8			regs[9];

	/* read all ALARM1, ALARM2, and status registers at once */
	ret = regmap_bulk_read(ds1307->regmap, DS1339_REG_ALARM1_SECS,
			       regs, sizeof(regs));
	if (ret) {
		dev_err(dev, "%s error %d\n", "alarm read", ret);
		return ret;
	}

	dev_dbg(dev, "%s: %4ph, %3ph, %2ph\n", "alarm read",
		&regs[0], &regs[4], &regs[7]);

	/*
	 * report alarm time (ALARM1); assume 24 hour and day-of-month modes,
	 * and that all four fields are checked matches
	 */
	t->time.tm_sec = bcd2bin(regs[0] & 0x7f);
	t->time.tm_min = bcd2bin(regs[1] & 0x7f);
	t->time.tm_hour = bcd2bin(regs[2] & 0x3f);
	t->time.tm_mday = bcd2bin(regs[3] & 0x3f);

	/* ... and status */
	t->enabled = !!(regs[7] & DS1337_BIT_A1IE);
	t->pending = !!(regs[8] & DS1337_BIT_A1I);

	dev_dbg(dev, "%s secs=%d, mins=%d, "
		"hours=%d, mday=%d, enabled=%d, pending=%d\n",
		"alarm read", t->time.tm_sec, t->time.tm_min,
		t->time.tm_hour, t->time.tm_mday,
		t->enabled, t->pending);

	return 0;
}

static int ds1337_set_alarm(struct device *dev, struct rtc_wkalrm *t)
{
	struct ds1307		*ds1307 = dev_get_drvdata(dev);
	unsigned char		regs[9];
	u8			control, status;
	int			ret;

	dev_dbg(dev, "%s secs=%d, mins=%d, "
		"hours=%d, mday=%d, enabled=%d, pending=%d\n",
		"alarm set", t->time.tm_sec, t->time.tm_min,
		t->time.tm_hour, t->time.tm_mday,
		t->enabled, t->pending);

	/* read current status of both alarms and the chip */
	ret = regmap_bulk_read(ds1307->regmap, DS1339_REG_ALARM1_SECS, regs,
			       sizeof(regs));
	if (ret) {
		dev_err(dev, "%s error %d\n", "alarm write", ret);
		return ret;
	}
	control = regs[7];
	status = regs[8];

	dev_dbg(dev, "%s: %4ph, %3ph, %02x %02x\n", "alarm set (old status)",
		&regs[0], &regs[4], control, status);

	/* set ALARM1, using 24 hour and day-of-month modes */
	regs[0] = bin2bcd(t->time.tm_sec);
	regs[1] = bin2bcd(t->time.tm_min);
	regs[2] = bin2bcd(t->time.tm_hour);
	regs[3] = bin2bcd(t->time.tm_mday);

	/* set ALARM2 to non-garbage */
	regs[4] = 0;
	regs[5] = 0;
	regs[6] = 0;

	/* disable alarms */
	regs[7] = control & ~(DS1337_BIT_A1IE | DS1337_BIT_A2IE);
	regs[8] = status & ~(DS1337_BIT_A1I | DS1337_BIT_A2I);

	ret = regmap_bulk_write(ds1307->regmap, DS1339_REG_ALARM1_SECS, regs,
				sizeof(regs));
	if (ret) {
		dev_err(dev, "can't set alarm time\n");
		return ret;
	}

	/* optionally enable ALARM1 */
	if (t->enabled) {
		dev_dbg(dev, "alarm IRQ armed\n");
		regs[7] |= DS1337_BIT_A1IE;	/* only ALARM1 is used */
		regmap_write(ds1307->regmap, DS1337_REG_CONTROL, regs[7]);
	}

	return 0;
}

static int ds1307_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	struct ds1307		*ds1307 = dev_get_drvdata(dev);

	return regmap_update_bits(ds1307->regmap, DS1337_REG_CONTROL,
				  DS1337_BIT_A1IE,
				  enabled ? DS1337_BIT_A1IE : 0);
}

static u8 do_trickle_setup_ds1339(struct ds1307 *ds1307, u32 ohms, bool diode)
{
	u8 setup = (diode) ? DS1307_TRICKLE_CHARGER_DIODE :
		DS1307_TRICKLE_CHARGER_NO_DIODE;

	setup |= DS13XX_TRICKLE_CHARGER_MAGIC;

	switch (ohms) {
	case 250:
		setup |= DS1307_TRICKLE_CHARGER_250_OHM;
		break;
	case 2000:
		setup |= DS1307_TRICKLE_CHARGER_2K_OHM;
		break;
	case 4000:
		setup |= DS1307_TRICKLE_CHARGER_4K_OHM;
		break;
	default:
		dev_warn(ds1307->dev,
			 "Unsupported ohm value %u in dt\n", ohms);
		return 0;
	}
	return setup;
}

static u8 do_trickle_setup_rx8130(struct ds1307 *ds1307, u32 ohms, bool diode)
{
	/* make sure that the backup battery is enabled */
	u8 setup = RX8130_REG_CONTROL1_INIEN;
	if (diode)
		setup |= RX8130_REG_CONTROL1_CHGEN;

	return setup;
}

static irqreturn_t rx8130_irq(int irq, void *dev_id)
{
	struct ds1307           *ds1307 = dev_id;
	u8 ctl[3];
	int ret;

	rtc_lock(ds1307->rtc);

	/* Read control registers. */
	ret = regmap_bulk_read(ds1307->regmap, RX8130_REG_EXTENSION, ctl,
			       sizeof(ctl));
	if (ret < 0)
		goto out;
	if (!(ctl[1] & RX8130_REG_FLAG_AF))
		goto out;
	ctl[1] &= ~RX8130_REG_FLAG_AF;
	ctl[2] &= ~RX8130_REG_CONTROL0_AIE;

	ret = regmap_bulk_write(ds1307->regmap, RX8130_REG_EXTENSION, ctl,
				sizeof(ctl));
	if (ret < 0)
		goto out;

	rtc_update_irq(ds1307->rtc, 1, RTC_AF | RTC_IRQF);

out:
	rtc_unlock(ds1307->rtc);

	return IRQ_HANDLED;
}

static int rx8130_read_alarm(struct device *dev, struct rtc_wkalrm *t)
{
	struct ds1307 *ds1307 = dev_get_drvdata(dev);
	u8 ald[3], ctl[3];
	int ret;

	/* Read alarm registers. */
	ret = regmap_bulk_read(ds1307->regmap, RX8130_REG_ALARM_MIN, ald,
			       sizeof(ald));
	if (ret < 0)
		return ret;

	/* Read control registers. */
	ret = regmap_bulk_read(ds1307->regmap, RX8130_REG_EXTENSION, ctl,
			       sizeof(ctl));
	if (ret < 0)
		return ret;

	t->enabled = !!(ctl[2] & RX8130_REG_CONTROL0_AIE);
	t->pending = !!(ctl[1] & RX8130_REG_FLAG_AF);

	/* Report alarm 0 time assuming 24-hour and day-of-month modes. */
	t->time.tm_sec = -1;
	t->time.tm_min = bcd2bin(ald[0] & 0x7f);
	t->time.tm_hour = bcd2bin(ald[1] & 0x7f);
	t->time.tm_wday = -1;
	t->time.tm_mday = bcd2bin(ald[2] & 0x7f);
	t->time.tm_mon = -1;
	t->time.tm_year = -1;
	t->time.tm_yday = -1;
	t->time.tm_isdst = -1;

	dev_dbg(dev, "%s, sec=%d min=%d hour=%d wday=%d mday=%d mon=%d enabled=%d\n",
		__func__, t->time.tm_sec, t->time.tm_min, t->time.tm_hour,
		t->time.tm_wday, t->time.tm_mday, t->time.tm_mon, t->enabled);

	return 0;
}

static int rx8130_set_alarm(struct device *dev, struct rtc_wkalrm *t)
{
	struct ds1307 *ds1307 = dev_get_drvdata(dev);
	u8 ald[3], ctl[3];
	int ret;

	dev_dbg(dev, "%s, sec=%d min=%d hour=%d wday=%d mday=%d mon=%d "
		"enabled=%d pending=%d\n", __func__,
		t->time.tm_sec, t->time.tm_min, t->time.tm_hour,
		t->time.tm_wday, t->time.tm_mday, t->time.tm_mon,
		t->enabled, t->pending);

	/* Read control registers. */
	ret = regmap_bulk_read(ds1307->regmap, RX8130_REG_EXTENSION, ctl,
			       sizeof(ctl));
	if (ret < 0)
		return ret;

	ctl[0] &= RX8130_REG_EXTENSION_WADA;
	ctl[1] &= ~RX8130_REG_FLAG_AF;
	ctl[2] &= ~RX8130_REG_CONTROL0_AIE;

	ret = regmap_bulk_write(ds1307->regmap, RX8130_REG_EXTENSION, ctl,
				sizeof(ctl));
	if (ret < 0)
		return ret;

	/* Hardware alarm precision is 1 minute! */
	ald[0] = bin2bcd(t->time.tm_min);
	ald[1] = bin2bcd(t->time.tm_hour);
	ald[2] = bin2bcd(t->time.tm_mday);

	ret = regmap_bulk_write(ds1307->regmap, RX8130_REG_ALARM_MIN, ald,
				sizeof(ald));
	if (ret < 0)
		return ret;

	if (!t->enabled)
		return 0;

	ctl[2] |= RX8130_REG_CONTROL0_AIE;

	return regmap_write(ds1307->regmap, RX8130_REG_CONTROL0, ctl[2]);
}

static int rx8130_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	struct ds1307 *ds1307 = dev_get_drvdata(dev);
	int ret, reg;

	ret = regmap_read(ds1307->regmap, RX8130_REG_CONTROL0, &reg);
	if (ret < 0)
		return ret;

	if (enabled)
		reg |= RX8130_REG_CONTROL0_AIE;
	else
		reg &= ~RX8130_REG_CONTROL0_AIE;

	return regmap_write(ds1307->regmap, RX8130_REG_CONTROL0, reg);
}

static irqreturn_t mcp794xx_irq(int irq, void *dev_id)
{
	struct ds1307           *ds1307 = dev_id;
	struct mutex            *lock = &ds1307->rtc->ops_lock;
	int reg, ret;

	mutex_lock(lock);

	/* Check and clear alarm 0 interrupt flag. */
	ret = regmap_read(ds1307->regmap, MCP794XX_REG_ALARM0_CTRL, &reg);
	if (ret)
		goto out;
	if (!(reg & MCP794XX_BIT_ALMX_IF))
		goto out;
	reg &= ~MCP794XX_BIT_ALMX_IF;
	ret = regmap_write(ds1307->regmap, MCP794XX_REG_ALARM0_CTRL, reg);
	if (ret)
		goto out;

	/* Disable alarm 0. */
	ret = regmap_update_bits(ds1307->regmap, MCP794XX_REG_CONTROL,
				 MCP794XX_BIT_ALM0_EN, 0);
	if (ret)
		goto out;

	rtc_update_irq(ds1307->rtc, 1, RTC_AF | RTC_IRQF);

out:
	mutex_unlock(lock);

	return IRQ_HANDLED;
}

static int mcp794xx_read_alarm(struct device *dev, struct rtc_wkalrm *t)
{
	struct ds1307 *ds1307 = dev_get_drvdata(dev);
	u8 regs[10];
	int ret;

	/* Read control and alarm 0 registers. */
	ret = regmap_bulk_read(ds1307->regmap, MCP794XX_REG_CONTROL, regs,
			       sizeof(regs));
	if (ret)
		return ret;

	t->enabled = !!(regs[0] & MCP794XX_BIT_ALM0_EN);

	/* Report alarm 0 time assuming 24-hour and day-of-month modes. */
	t->time.tm_sec = bcd2bin(regs[3] & 0x7f);
	t->time.tm_min = bcd2bin(regs[4] & 0x7f);
	t->time.tm_hour = bcd2bin(regs[5] & 0x3f);
	t->time.tm_wday = bcd2bin(regs[6] & 0x7) - 1;
	t->time.tm_mday = bcd2bin(regs[7] & 0x3f);
	t->time.tm_mon = bcd2bin(regs[8] & 0x1f) - 1;
	t->time.tm_year = -1;
	t->time.tm_yday = -1;
	t->time.tm_isdst = -1;

	dev_dbg(dev, "%s, sec=%d min=%d hour=%d wday=%d mday=%d mon=%d "
		"enabled=%d polarity=%d irq=%d match=%lu\n", __func__,
		t->time.tm_sec, t->time.tm_min, t->time.tm_hour,
		t->time.tm_wday, t->time.tm_mday, t->time.tm_mon, t->enabled,
		!!(regs[6] & MCP794XX_BIT_ALMX_POL),
		!!(regs[6] & MCP794XX_BIT_ALMX_IF),
		(regs[6] & MCP794XX_MSK_ALMX_MATCH) >> 4);

	return 0;
}

/*
 * We may have a random RTC weekday, therefore calculate alarm weekday based
 * on current weekday we read from the RTC timekeeping regs
 */
static int mcp794xx_alm_weekday(struct device *dev, struct rtc_time *tm_alarm)
{
	struct rtc_time tm_now;
	int days_now, days_alarm, ret;

	ret = ds1307_get_time(dev, &tm_now);
	if (ret)
		return ret;

	days_now = div_s64(rtc_tm_to_time64(&tm_now), 24 * 60 * 60);
	days_alarm = div_s64(rtc_tm_to_time64(tm_alarm), 24 * 60 * 60);

	return (tm_now.tm_wday + days_alarm - days_now) % 7 + 1;
}

static int mcp794xx_set_alarm(struct device *dev, struct rtc_wkalrm *t)
{
	struct ds1307 *ds1307 = dev_get_drvdata(dev);
	unsigned char regs[10];
	int wday, ret;

	wday = mcp794xx_alm_weekday(dev, &t->time);
	if (wday < 0)
		return wday;

	dev_dbg(dev, "%s, sec=%d min=%d hour=%d wday=%d mday=%d mon=%d "
		"enabled=%d pending=%d\n", __func__,
		t->time.tm_sec, t->time.tm_min, t->time.tm_hour,
		t->time.tm_wday, t->time.tm_mday, t->time.tm_mon,
		t->enabled, t->pending);

	/* Read control and alarm 0 registers. */
	ret = regmap_bulk_read(ds1307->regmap, MCP794XX_REG_CONTROL, regs,
			       sizeof(regs));
	if (ret)
		return ret;

	/* Set alarm 0, using 24-hour and day-of-month modes. */
	regs[3] = bin2bcd(t->time.tm_sec);
	regs[4] = bin2bcd(t->time.tm_min);
	regs[5] = bin2bcd(t->time.tm_hour);
	regs[6] = wday;
	regs[7] = bin2bcd(t->time.tm_mday);
	regs[8] = bin2bcd(t->time.tm_mon + 1);

	/* Clear the alarm 0 interrupt flag. */
	regs[6] &= ~MCP794XX_BIT_ALMX_IF;
	/* Set alarm match: second, minute, hour, day, date, month. */
	regs[6] |= MCP794XX_MSK_ALMX_MATCH;
	/* Disable interrupt. We will not enable until completely programmed */
	regs[0] &= ~MCP794XX_BIT_ALM0_EN;

	ret = regmap_bulk_write(ds1307->regmap, MCP794XX_REG_CONTROL, regs,
				sizeof(regs));
	if (ret)
		return ret;

	if (!t->enabled)
		return 0;
	regs[0] |= MCP794XX_BIT_ALM0_EN;
	return regmap_write(ds1307->regmap, MCP794XX_REG_CONTROL, regs[0]);
}

static int mcp794xx_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	struct ds1307 *ds1307 = dev_get_drvdata(dev);

	return regmap_update_bits(ds1307->regmap, MCP794XX_REG_CONTROL,
				  MCP794XX_BIT_ALM0_EN,
				  enabled ? MCP794XX_BIT_ALM0_EN : 0);
}

static int m41txx_rtc_read_offset(struct device *dev, long *offset)
{
	struct ds1307 *ds1307 = dev_get_drvdata(dev);
	unsigned int ctrl_reg;
	u8 val;

	regmap_read(ds1307->regmap, M41TXX_REG_CONTROL, &ctrl_reg);

	val = ctrl_reg & M41TXX_M_CALIBRATION;

	/* check if positive */
	if (ctrl_reg & M41TXX_BIT_CALIB_SIGN)
		*offset = (val * M41TXX_POS_OFFSET_STEP_PPB);
	else
		*offset = -(val * M41TXX_NEG_OFFSET_STEP_PPB);

	return 0;
}

static int m41txx_rtc_set_offset(struct device *dev, long offset)
{
	struct ds1307 *ds1307 = dev_get_drvdata(dev);
	unsigned int ctrl_reg;

	if ((offset < M41TXX_MIN_OFFSET) || (offset > M41TXX_MAX_OFFSET))
		return -ERANGE;

	if (offset >= 0) {
		ctrl_reg = DIV_ROUND_CLOSEST(offset,
					     M41TXX_POS_OFFSET_STEP_PPB);
		ctrl_reg |= M41TXX_BIT_CALIB_SIGN;
	} else {
		ctrl_reg = DIV_ROUND_CLOSEST(abs(offset),
					     M41TXX_NEG_OFFSET_STEP_PPB);
	}

	return regmap_update_bits(ds1307->regmap, M41TXX_REG_CONTROL,
				  M41TXX_M_CALIBRATION | M41TXX_BIT_CALIB_SIGN,
				  ctrl_reg);
}

#ifdef CONFIG_WATCHDOG_CORE
static int ds1388_wdt_start(struct watchdog_device *wdt_dev)
{
	struct ds1307 *ds1307 = watchdog_get_drvdata(wdt_dev);
	u8 regs[2];
	int ret;

	ret = regmap_update_bits(ds1307->regmap, DS1388_REG_FLAG,
				 DS1388_BIT_WF, 0);
	if (ret)
		return ret;

	ret = regmap_update_bits(ds1307->regmap, DS1388_REG_CONTROL,
				 DS1388_BIT_WDE | DS1388_BIT_RST, 0);
	if (ret)
		return ret;

	/*
	 * watchdog timeouts are measured in seconds. So ignore hundredths of
	 * seconds field.
	 */
	regs[0] = 0;
	regs[1] = bin2bcd(wdt_dev->timeout);

	ret = regmap_bulk_write(ds1307->regmap, DS1388_REG_WDOG_HUN_SECS, regs,
				sizeof(regs));
	if (ret)
		return ret;

	return regmap_update_bits(ds1307->regmap, DS1388_REG_CONTROL,
				  DS1388_BIT_WDE | DS1388_BIT_RST,
				  DS1388_BIT_WDE | DS1388_BIT_RST);
}

static int ds1388_wdt_stop(struct watchdog_device *wdt_dev)
{
	struct ds1307 *ds1307 = watchdog_get_drvdata(wdt_dev);

	return regmap_update_bits(ds1307->regmap, DS1388_REG_CONTROL,
				  DS1388_BIT_WDE | DS1388_BIT_RST, 0);
}

static int ds1388_wdt_ping(struct watchdog_device *wdt_dev)
{
	struct ds1307 *ds1307 = watchdog_get_drvdata(wdt_dev);
	u8 regs[2];

	return regmap_bulk_read(ds1307->regmap, DS1388_REG_WDOG_HUN_SECS, regs,
				sizeof(regs));
}

static int ds1388_wdt_set_timeout(struct watchdog_device *wdt_dev,
				  unsigned int val)
{
	struct ds1307 *ds1307 = watchdog_get_drvdata(wdt_dev);
	u8 regs[2];

	wdt_dev->timeout = val;
	regs[0] = 0;
	regs[1] = bin2bcd(wdt_dev->timeout);

	return regmap_bulk_write(ds1307->regmap, DS1388_REG_WDOG_HUN_SECS, regs,
				 sizeof(regs));
}
#endif

static const struct rtc_class_ops rx8130_rtc_ops = {
	.read_time      = ds1307_get_time,
	.set_time       = ds1307_set_time,
	.read_alarm     = rx8130_read_alarm,
	.set_alarm      = rx8130_set_alarm,
	.alarm_irq_enable = rx8130_alarm_irq_enable,
};

static const struct rtc_class_ops mcp794xx_rtc_ops = {
	.read_time      = ds1307_get_time,
	.set_time       = ds1307_set_time,
	.read_alarm     = mcp794xx_read_alarm,
	.set_alarm      = mcp794xx_set_alarm,
	.alarm_irq_enable = mcp794xx_alarm_irq_enable,
};

static const struct rtc_class_ops m41txx_rtc_ops = {
	.read_time      = ds1307_get_time,
	.set_time       = ds1307_set_time,
	.read_alarm	= ds1337_read_alarm,
	.set_alarm	= ds1337_set_alarm,
	.alarm_irq_enable = ds1307_alarm_irq_enable,
	.read_offset	= m41txx_rtc_read_offset,
	.set_offset	= m41txx_rtc_set_offset,
};

static const struct chip_desc chips[last_ds_type] = {
	[ds_1307] = {
		.nvram_offset	= 8,
		.nvram_size	= 56,
	},
	[ds_1308] = {
		.nvram_offset	= 8,
		.nvram_size	= 56,
	},
	[ds_1337] = {
		.alarm		= 1,
		.century_reg	= DS1307_REG_MONTH,
		.century_bit	= DS1337_BIT_CENTURY,
	},
	[ds_1338] = {
		.nvram_offset	= 8,
		.nvram_size	= 56,
	},
	[ds_1339] = {
		.alarm		= 1,
		.century_reg	= DS1307_REG_MONTH,
		.century_bit	= DS1337_BIT_CENTURY,
		.bbsqi_bit	= DS1339_BIT_BBSQI,
		.trickle_charger_reg = 0x10,
		.do_trickle_setup = &do_trickle_setup_ds1339,
		.requires_trickle_resistor = true,
		.charge_default = true,
	},
	[ds_1340] = {
		.century_reg	= DS1307_REG_HOUR,
		.century_enable_bit = DS1340_BIT_CENTURY_EN,
		.century_bit	= DS1340_BIT_CENTURY,
		.do_trickle_setup = &do_trickle_setup_ds1339,
		.trickle_charger_reg = 0x08,
		.requires_trickle_resistor = true,
		.charge_default = true,
	},
	[ds_1341] = {
		.century_reg	= DS1307_REG_MONTH,
		.century_bit	= DS1337_BIT_CENTURY,
	},
	[ds_1388] = {
		.offset		= 1,
		.trickle_charger_reg = 0x0a,
	},
	[ds_3231] = {
		.alarm		= 1,
		.century_reg	= DS1307_REG_MONTH,
		.century_bit	= DS1337_BIT_CENTURY,
		.bbsqi_bit	= DS3231_BIT_BBSQW,
	},
	[rx_8130] = {
		.alarm		= 1,
		/* this is battery backed SRAM */
		.nvram_offset	= 0x20,
		.nvram_size	= 4,	/* 32bit (4 word x 8 bit) */
		.offset		= 0x10,
		.irq_handler = rx8130_irq,
		.rtc_ops = &rx8130_rtc_ops,
		.trickle_charger_reg = RX8130_REG_CONTROL1,
		.do_trickle_setup = &do_trickle_setup_rx8130,
	},
	[m41t0] = {
		.rtc_ops	= &m41txx_rtc_ops,
	},
	[m41t00] = {
		.rtc_ops	= &m41txx_rtc_ops,
	},
	[m41t11] = {
		/* this is battery backed SRAM */
		.nvram_offset	= 8,
		.nvram_size	= 56,
		.rtc_ops	= &m41txx_rtc_ops,
	},
	[mcp794xx] = {
		.alarm		= 1,
		/* this is battery backed SRAM */
		.nvram_offset	= 0x20,
		.nvram_size	= 0x40,
		.irq_handler = mcp794xx_irq,
		.rtc_ops = &mcp794xx_rtc_ops,
	},
};

static const struct i2c_device_id ds1307_id[] = {
	{ "ds1307", ds_1307 },
	{ "ds1308", ds_1308 },
	{ "ds1337", ds_1337 },
	{ "ds1338", ds_1338 },
	{ "ds1339", ds_1339 },
	{ "ds1388", ds_1388 },
	{ "ds1340", ds_1340 },
	{ "ds1341", ds_1341 },
	{ "ds3231", ds_3231 },
	{ "m41t0", m41t0 },
	{ "m41t00", m41t00 },
	{ "m41t11", m41t11 },
	{ "mcp7940x", mcp794xx },
	{ "mcp7941x", mcp794xx },
	{ "pt7c4338", ds_1307 },
	{ "rx8025", rx_8025 },
	{ "isl12057", ds_1337 },
	{ "rx8130", rx_8130 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ds1307_id);

static const struct of_device_id ds1307_of_match[] = {
	{
		.compatible = "dallas,ds1307",
		.data = (void *)ds_1307
	},
	{
		.compatible = "dallas,ds1308",
		.data = (void *)ds_1308
	},
	{
		.compatible = "dallas,ds1337",
		.data = (void *)ds_1337
	},
	{
		.compatible = "dallas,ds1338",
		.data = (void *)ds_1338
	},
	{
		.compatible = "dallas,ds1339",
		.data = (void *)ds_1339
	},
	{
		.compatible = "dallas,ds1388",
		.data = (void *)ds_1388
	},
	{
		.compatible = "dallas,ds1340",
		.data = (void *)ds_1340
	},
	{
		.compatible = "dallas,ds1341",
		.data = (void *)ds_1341
	},
	{
		.compatible = "maxim,ds3231",
		.data = (void *)ds_3231
	},
	{
		.compatible = "st,m41t0",
		.data = (void *)m41t0
	},
	{
		.compatible = "st,m41t00",
		.data = (void *)m41t00
	},
	{
		.compatible = "st,m41t11",
		.data = (void *)m41t11
	},
	{
		.compatible = "microchip,mcp7940x",
		.data = (void *)mcp794xx
	},
	{
		.compatible = "microchip,mcp7941x",
		.data = (void *)mcp794xx
	},
	{
		.compatible = "pericom,pt7c4338",
		.data = (void *)ds_1307
	},
	{
		.compatible = "epson,rx8025",
		.data = (void *)rx_8025
	},
	{
		.compatible = "isil,isl12057",
		.data = (void *)ds_1337
	},
	{
		.compatible = "epson,rx8130",
		.data = (void *)rx_8130
	},
	{ }
};
MODULE_DEVICE_TABLE(of, ds1307_of_match);

/*
 * The ds1337 and ds1339 both have two alarms, but we only use the first
 * one (with a "seconds" field).  For ds1337 we expect nINTA is our alarm
 * signal; ds1339 chips have only one alarm signal.
 */
static irqreturn_t ds1307_irq(int irq, void *dev_id)
{
	struct ds1307		*ds1307 = dev_id;
	struct mutex		*lock = &ds1307->rtc->ops_lock;
	int			stat, ret;

	mutex_lock(lock);
	ret = regmap_read(ds1307->regmap, DS1337_REG_STATUS, &stat);
	if (ret)
		goto out;

	if (stat & DS1337_BIT_A1I) {
		stat &= ~DS1337_BIT_A1I;
		regmap_write(ds1307->regmap, DS1337_REG_STATUS, stat);

		ret = regmap_update_bits(ds1307->regmap, DS1337_REG_CONTROL,
					 DS1337_BIT_A1IE, 0);
		if (ret)
			goto out;

		rtc_update_irq(ds1307->rtc, 1, RTC_AF | RTC_IRQF);
	}

out:
	mutex_unlock(lock);

	return IRQ_HANDLED;
}

/*----------------------------------------------------------------------*/

static const struct rtc_class_ops ds13xx_rtc_ops = {
	.read_time	= ds1307_get_time,
	.set_time	= ds1307_set_time,
	.read_alarm	= ds1337_read_alarm,
	.set_alarm	= ds1337_set_alarm,
	.alarm_irq_enable = ds1307_alarm_irq_enable,
};

static ssize_t frequency_test_store(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
	struct ds1307 *ds1307 = dev_get_drvdata(dev->parent);
	bool freq_test_en;
	int ret;

	ret = kstrtobool(buf, &freq_test_en);
	if (ret) {
		dev_err(dev, "Failed to store RTC Frequency Test attribute\n");
		return ret;
	}

	regmap_update_bits(ds1307->regmap, M41TXX_REG_CONTROL, M41TXX_BIT_FT,
			   freq_test_en ? M41TXX_BIT_FT : 0);

	return count;
}

static ssize_t frequency_test_show(struct device *dev,
				   struct device_attribute *attr,
				   char *buf)
{
	struct ds1307 *ds1307 = dev_get_drvdata(dev->parent);
	unsigned int ctrl_reg;

	regmap_read(ds1307->regmap, M41TXX_REG_CONTROL, &ctrl_reg);

	return scnprintf(buf, PAGE_SIZE, (ctrl_reg & M41TXX_BIT_FT) ? "on\n" :
			"off\n");
}

static DEVICE_ATTR_RW(frequency_test);

static struct attribute *rtc_freq_test_attrs[] = {
	&dev_attr_frequency_test.attr,
	NULL,
};

static const struct attribute_group rtc_freq_test_attr_group = {
	.attrs		= rtc_freq_test_attrs,
};

static int ds1307_add_frequency_test(struct ds1307 *ds1307)
{
	int err;

	switch (ds1307->type) {
	case m41t0:
	case m41t00:
	case m41t11:
		err = rtc_add_group(ds1307->rtc, &rtc_freq_test_attr_group);
		if (err)
			return err;
		break;
	default:
		break;
	}

	return 0;
}

/*----------------------------------------------------------------------*/

static int ds1307_nvram_read(void *priv, unsigned int offset, void *val,
			     size_t bytes)
{
	struct ds1307 *ds1307 = priv;
	const struct chip_desc *chip = &chips[ds1307->type];

	return regmap_bulk_read(ds1307->regmap, chip->nvram_offset + offset,
				val, bytes);
}

static int ds1307_nvram_write(void *priv, unsigned int offset, void *val,
			      size_t bytes)
{
	struct ds1307 *ds1307 = priv;
	const struct chip_desc *chip = &chips[ds1307->type];

	return regmap_bulk_write(ds1307->regmap, chip->nvram_offset + offset,
				 val, bytes);
}

/*----------------------------------------------------------------------*/

static u8 ds1307_trickle_init(struct ds1307 *ds1307,
			      const struct chip_desc *chip)
{
	u32 ohms, chargeable;
	bool diode = chip->charge_default;

	if (!chip->do_trickle_setup)
		return 0;

	if (device_property_read_u32(ds1307->dev, "trickle-resistor-ohms",
				     &ohms) && chip->requires_trickle_resistor)
		return 0;

	/* aux-voltage-chargeable takes precedence over the deprecated
	 * trickle-diode-disable
	 */
	if (!device_property_read_u32(ds1307->dev, "aux-voltage-chargeable",
				     &chargeable)) {
		switch (chargeable) {
		case 0:
			diode = false;
			break;
		case 1:
			diode = true;
			break;
		default:
			dev_warn(ds1307->dev,
				 "unsupported aux-voltage-chargeable value\n");
			break;
		}
	} else if (device_property_read_bool(ds1307->dev,
					     "trickle-diode-disable")) {
		diode = false;
	}

	return chip->do_trickle_setup(ds1307, ohms, diode);
}

/*----------------------------------------------------------------------*/

#if IS_REACHABLE(CONFIG_HWMON)

/*
 * Temperature sensor support for ds3231 devices.
 */

#define DS3231_REG_TEMPERATURE	0x11

/*
 * A user-initiated temperature conversion is not started by this function,
 * so the temperature is updated once every 64 seconds.
 */
static int ds3231_hwmon_read_temp(struct device *dev, s32 *mC)
{
	struct ds1307 *ds1307 = dev_get_drvdata(dev);
	u8 temp_buf[2];
	s16 temp;
	int ret;

	ret = regmap_bulk_read(ds1307->regmap, DS3231_REG_TEMPERATURE,
			       temp_buf, sizeof(temp_buf));
	if (ret)
		return ret;
	/*
	 * Temperature is represented as a 10-bit code with a resolution of
	 * 0.25 degree celsius and encoded in two's complement format.
	 */
	temp = (temp_buf[0] << 8) | temp_buf[1];
	temp >>= 6;
	*mC = temp * 250;

	return 0;
}

static ssize_t ds3231_hwmon_show_temp(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	int ret;
	s32 temp;

	ret = ds3231_hwmon_read_temp(dev, &temp);
	if (ret)
		return ret;

	return sprintf(buf, "%d\n", temp);
}
static SENSOR_DEVICE_ATTR(temp1_input, 0444, ds3231_hwmon_show_temp,
			  NULL, 0);

static struct attribute *ds3231_hwmon_attrs[] = {
	&sensor_dev_attr_temp1_input.dev_attr.attr,
	NULL,
};
ATTRIBUTE_GROUPS(ds3231_hwmon);

static void ds1307_hwmon_register(struct ds1307 *ds1307)
{
	struct device *dev;

	if (ds1307->type != ds_3231)
		return;

	dev = devm_hwmon_device_register_with_groups(ds1307->dev, ds1307->name,
						     ds1307,
						     ds3231_hwmon_groups);
	if (IS_ERR(dev)) {
		dev_warn(ds1307->dev, "unable to register hwmon device %ld\n",
			 PTR_ERR(dev));
	}
}

#else

static void ds1307_hwmon_register(struct ds1307 *ds1307)
{
}

#endif /* CONFIG_RTC_DRV_DS1307_HWMON */

/*----------------------------------------------------------------------*/

/*
 * Square-wave output support for DS3231
 * Datasheet: https://datasheets.maximintegrated.com/en/ds/DS3231.pdf
 */
#ifdef CONFIG_COMMON_CLK

enum {
	DS3231_CLK_SQW = 0,
	DS3231_CLK_32KHZ,
};

#define clk_sqw_to_ds1307(clk)	\
	container_of(clk, struct ds1307, clks[DS3231_CLK_SQW])
#define clk_32khz_to_ds1307(clk)	\
	container_of(clk, struct ds1307, clks[DS3231_CLK_32KHZ])

static int ds3231_clk_sqw_rates[] = {
	1,
	1024,
	4096,
	8192,
};

static int ds1337_write_control(struct ds1307 *ds1307, u8 mask, u8 value)
{
	struct mutex *lock = &ds1307->rtc->ops_lock;
	int ret;

	mutex_lock(lock);
	ret = regmap_update_bits(ds1307->regmap, DS1337_REG_CONTROL,
				 mask, value);
	mutex_unlock(lock);

	return ret;
}

static unsigned long ds3231_clk_sqw_recalc_rate(struct clk_hw *hw,
						unsigned long parent_rate)
{
	struct ds1307 *ds1307 = clk_sqw_to_ds1307(hw);
	int control, ret;
	int rate_sel = 0;

	ret = regmap_read(ds1307->regmap, DS1337_REG_CONTROL, &control);
	if (ret)
		return ret;
	if (control & DS1337_BIT_RS1)
		rate_sel += 1;
	if (control & DS1337_BIT_RS2)
		rate_sel += 2;

	return ds3231_clk_sqw_rates[rate_sel];
}

static long ds3231_clk_sqw_round_rate(struct clk_hw *hw, unsigned long rate,
				      unsigned long *prate)
{
	int i;

	for (i = ARRAY_SIZE(ds3231_clk_sqw_rates) - 1; i >= 0; i--) {
		if (ds3231_clk_sqw_rates[i] <= rate)
			return ds3231_clk_sqw_rates[i];
	}

	return 0;
}

static int ds3231_clk_sqw_set_rate(struct clk_hw *hw, unsigned long rate,
				   unsigned long parent_rate)
{
	struct ds1307 *ds1307 = clk_sqw_to_ds1307(hw);
	int control = 0;
	int rate_sel;

	for (rate_sel = 0; rate_sel < ARRAY_SIZE(ds3231_clk_sqw_rates);
			rate_sel++) {
		if (ds3231_clk_sqw_rates[rate_sel] == rate)
			break;
	}

	if (rate_sel == ARRAY_SIZE(ds3231_clk_sqw_rates))
		return -EINVAL;

	if (rate_sel & 1)
		control |= DS1337_BIT_RS1;
	if (rate_sel & 2)
		control |= DS1337_BIT_RS2;

	return ds1337_write_control(ds1307, DS1337_BIT_RS1 | DS1337_BIT_RS2,
				control);
}

static int ds3231_clk_sqw_prepare(struct clk_hw *hw)
{
	struct ds1307 *ds1307 = clk_sqw_to_ds1307(hw);

	return ds1337_write_control(ds1307, DS1337_BIT_INTCN, 0);
}

static void ds3231_clk_sqw_unprepare(struct clk_hw *hw)
{
	struct ds1307 *ds1307 = clk_sqw_to_ds1307(hw);

	ds1337_write_control(ds1307, DS1337_BIT_INTCN, DS1337_BIT_INTCN);
}

static int ds3231_clk_sqw_is_prepared(struct clk_hw *hw)
{
	struct ds1307 *ds1307 = clk_sqw_to_ds1307(hw);
	int control, ret;

	ret = regmap_read(ds1307->regmap, DS1337_REG_CONTROL, &control);
	if (ret)
		return ret;

	return !(control & DS1337_BIT_INTCN);
}

static const struct clk_ops ds3231_clk_sqw_ops = {
	.prepare = ds3231_clk_sqw_prepare,
	.unprepare = ds3231_clk_sqw_unprepare,
	.is_prepared = ds3231_clk_sqw_is_prepared,
	.recalc_rate = ds3231_clk_sqw_recalc_rate,
	.round_rate = ds3231_clk_sqw_round_rate,
	.set_rate = ds3231_clk_sqw_set_rate,
};

static unsigned long ds3231_clk_32khz_recalc_rate(struct clk_hw *hw,
						  unsigned long parent_rate)
{
	return 32768;
}

static int ds3231_clk_32khz_control(struct ds1307 *ds1307, bool enable)
{
	struct mutex *lock = &ds1307->rtc->ops_lock;
	int ret;

	mutex_lock(lock);
	ret = regmap_update_bits(ds1307->regmap, DS1337_REG_STATUS,
				 DS3231_BIT_EN32KHZ,
				 enable ? DS3231_BIT_EN32KHZ : 0);
	mutex_unlock(lock);

	return ret;
}

static int ds3231_clk_32khz_prepare(struct clk_hw *hw)
{
	struct ds1307 *ds1307 = clk_32khz_to_ds1307(hw);

	return ds3231_clk_32khz_control(ds1307, true);
}

static void ds3231_clk_32khz_unprepare(struct clk_hw *hw)
{
	struct ds1307 *ds1307 = clk_32khz_to_ds1307(hw);

	ds3231_clk_32khz_control(ds1307, false);
}

static int ds3231_clk_32khz_is_prepared(struct clk_hw *hw)
{
	struct ds1307 *ds1307 = clk_32khz_to_ds1307(hw);
	int status, ret;

	ret = regmap_read(ds1307->regmap, DS1337_REG_STATUS, &status);
	if (ret)
		return ret;

	return !!(status & DS3231_BIT_EN32KHZ);
}

static const struct clk_ops ds3231_clk_32khz_ops = {
	.prepare = ds3231_clk_32khz_prepare,
	.unprepare = ds3231_clk_32khz_unprepare,
	.is_prepared = ds3231_clk_32khz_is_prepared,
	.recalc_rate = ds3231_clk_32khz_recalc_rate,
};

static const char *ds3231_clks_names[] = {
	[DS3231_CLK_SQW] = "ds3231_clk_sqw",
	[DS3231_CLK_32KHZ] = "ds3231_clk_32khz",
};

static struct clk_init_data ds3231_clks_init[] = {
	[DS3231_CLK_SQW] = {
		.ops = &ds3231_clk_sqw_ops,
	},
	[DS3231_CLK_32KHZ] = {
		.ops = &ds3231_clk_32khz_ops,
	},
};

static int ds3231_clks_register(struct ds1307 *ds1307)
{
	struct device_node *node = ds1307->dev->of_node;
	struct clk_onecell_data	*onecell;
	int i;

	onecell = devm_kzalloc(ds1307->dev, sizeof(*onecell), GFP_KERNEL);
	if (!onecell)
		return -ENOMEM;

	onecell->clk_num = ARRAY_SIZE(ds3231_clks_init);
	onecell->clks = devm_kcalloc(ds1307->dev, onecell->clk_num,
				     sizeof(onecell->clks[0]), GFP_KERNEL);
	if (!onecell->clks)
		return -ENOMEM;

	/* optional override of the clockname */
	device_property_read_string_array(ds1307->dev, "clock-output-names",
					  ds3231_clks_names,
					  ARRAY_SIZE(ds3231_clks_names));

	for (i = 0; i < ARRAY_SIZE(ds3231_clks_init); i++) {
		struct clk_init_data init = ds3231_clks_init[i];

		/*
		 * Interrupt signal due to alarm conditions and square-wave
		 * output share same pin, so don't initialize both.
		 */
		if (i == DS3231_CLK_SQW && test_bit(RTC_FEATURE_ALARM, ds1307->rtc->features))
			continue;

		init.name = ds3231_clks_names[i];
		ds1307->clks[i].init = &init;

		onecell->clks[i] = devm_clk_register(ds1307->dev,
						     &ds1307->clks[i]);
		if (IS_ERR(onecell->clks[i]))
			return PTR_ERR(onecell->clks[i]);
	}

	if (node)
		of_clk_add_provider(node, of_clk_src_onecell_get, onecell);

	return 0;
}

static void ds1307_clks_register(struct ds1307 *ds1307)
{
	int ret;

	if (ds1307->type != ds_3231)
		return;

	ret = ds3231_clks_register(ds1307);
	if (ret) {
		dev_warn(ds1307->dev, "unable to register clock device %d\n",
			 ret);
	}
}

#else

static void ds1307_clks_register(struct ds1307 *ds1307)
{
}

#endif /* CONFIG_COMMON_CLK */

#ifdef CONFIG_WATCHDOG_CORE
static const struct watchdog_info ds1388_wdt_info = {
	.options = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE,
	.identity = "DS1388 watchdog",
};

static const struct watchdog_ops ds1388_wdt_ops = {
	.owner = THIS_MODULE,
	.start = ds1388_wdt_start,
	.stop = ds1388_wdt_stop,
	.ping = ds1388_wdt_ping,
	.set_timeout = ds1388_wdt_set_timeout,

};

static void ds1307_wdt_register(struct ds1307 *ds1307)
{
	struct watchdog_device	*wdt;
	int err;
	int val;

	if (ds1307->type != ds_1388)
		return;

	wdt = devm_kzalloc(ds1307->dev, sizeof(*wdt), GFP_KERNEL);
	if (!wdt)
		return;

	err = regmap_read(ds1307->regmap, DS1388_REG_FLAG, &val);
	if (!err && val & DS1388_BIT_WF)
		wdt->bootstatus = WDIOF_CARDRESET;

	wdt->info = &ds1388_wdt_info;
	wdt->ops = &ds1388_wdt_ops;
	wdt->timeout = 99;
	wdt->max_timeout = 99;
	wdt->min_timeout = 1;

	watchdog_init_timeout(wdt, 0, ds1307->dev);
	watchdog_set_drvdata(wdt, ds1307);
	devm_watchdog_register_device(ds1307->dev, wdt);
}
#else
static void ds1307_wdt_register(struct ds1307 *ds1307)
{
}
#endif /* CONFIG_WATCHDOG_CORE */

static const struct regmap_config regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
};

static int ds1307_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct ds1307		*ds1307;
	const void		*match;
	int			err = -ENODEV;
	int			tmp;
	const struct chip_desc	*chip;
	bool			want_irq;
	bool			ds1307_can_wakeup_device = false;
	unsigned char		regs[8];
	struct ds1307_platform_data *pdata = dev_get_platdata(&client->dev);
	u8			trickle_charger_setup = 0;

	ds1307 = devm_kzalloc(&client->dev, sizeof(struct ds1307), GFP_KERNEL);
	if (!ds1307)
		return -ENOMEM;

	dev_set_drvdata(&client->dev, ds1307);
	ds1307->dev = &client->dev;
	ds1307->name = client->name;

	ds1307->regmap = devm_regmap_init_i2c(client, &regmap_config);
	if (IS_ERR(ds1307->regmap)) {
		dev_err(ds1307->dev, "regmap allocation failed\n");
		return PTR_ERR(ds1307->regmap);
	}

	i2c_set_clientdata(client, ds1307);

	match = device_get_match_data(&client->dev);
	if (match) {
		ds1307->type = (enum ds_type)match;
		chip = &chips[ds1307->type];
	} else if (id) {
		chip = &chips[id->driver_data];
		ds1307->type = id->driver_data;
	} else {
		return -ENODEV;
	}

	want_irq = client->irq > 0 && chip->alarm;

	if (!pdata)
		trickle_charger_setup = ds1307_trickle_init(ds1307, chip);
	else if (pdata->trickle_charger_setup)
		trickle_charger_setup = pdata->trickle_charger_setup;

	if (trickle_charger_setup && chip->trickle_charger_reg) {
		dev_dbg(ds1307->dev,
			"writing trickle charger info 0x%x to 0x%x\n",
			trickle_charger_setup, chip->trickle_charger_reg);
		regmap_write(ds1307->regmap, chip->trickle_charger_reg,
			     trickle_charger_setup);
	}

/*
 * For devices with no IRQ directly connected to the SoC, the RTC chip
 * can be forced as a wakeup source by stating that explicitly in
 * the device's .dts file using the "wakeup-source" boolean property.
 * If the "wakeup-source" property is set, don't request an IRQ.
 * This will guarantee the 'wakealarm' sysfs entry is available on the device,
 * if supported by the RTC.
 */
	if (chip->alarm && device_property_read_bool(&client->dev, "wakeup-source"))
		ds1307_can_wakeup_device = true;

	switch (ds1307->type) {
	case ds_1337:
	case ds_1339:
	case ds_1341:
	case ds_3231:
		/* get registers that the "rtc" read below won't read... */
		err = regmap_bulk_read(ds1307->regmap, DS1337_REG_CONTROL,
				       regs, 2);
		if (err) {
			dev_dbg(ds1307->dev, "read error %d\n", err);
			goto exit;
		}

		/* oscillator off?  turn it on, so clock can tick. */
		if (regs[0] & DS1337_BIT_nEOSC)
			regs[0] &= ~DS1337_BIT_nEOSC;

		/*
		 * Using IRQ or defined as wakeup-source?
		 * Disable the square wave and both alarms.
		 * For some variants, be sure alarms can trigger when we're
		 * running on Vbackup (BBSQI/BBSQW)
		 */
		if (want_irq || ds1307_can_wakeup_device) {
			regs[0] |= DS1337_BIT_INTCN | chip->bbsqi_bit;
			regs[0] &= ~(DS1337_BIT_A2IE | DS1337_BIT_A1IE);
		}

		regmap_write(ds1307->regmap, DS1337_REG_CONTROL,
			     regs[0]);

		/* oscillator fault?  clear flag, and warn */
		if (regs[1] & DS1337_BIT_OSF) {
			regmap_write(ds1307->regmap, DS1337_REG_STATUS,
				     regs[1] & ~DS1337_BIT_OSF);
			dev_warn(ds1307->dev, "SET TIME!\n");
		}
		break;

	case rx_8025:
		err = regmap_bulk_read(ds1307->regmap,
				       RX8025_REG_CTRL1 << 4 | 0x08, regs, 2);
		if (err) {
			dev_dbg(ds1307->dev, "read error %d\n", err);
			goto exit;
		}

		/* oscillator off?  turn it on, so clock can tick. */
		if (!(regs[1] & RX8025_BIT_XST)) {
			regs[1] |= RX8025_BIT_XST;
			regmap_write(ds1307->regmap,
				     RX8025_REG_CTRL2 << 4 | 0x08,
				     regs[1]);
			dev_warn(ds1307->dev,
				 "oscillator stop detected - SET TIME!\n");
		}

		if (regs[1] & RX8025_BIT_PON) {
			regs[1] &= ~RX8025_BIT_PON;
			regmap_write(ds1307->regmap,
				     RX8025_REG_CTRL2 << 4 | 0x08,
				     regs[1]);
			dev_warn(ds1307->dev, "power-on detected\n");
		}

		if (regs[1] & RX8025_BIT_VDET) {
			regs[1] &= ~RX8025_BIT_VDET;
			regmap_write(ds1307->regmap,
				     RX8025_REG_CTRL2 << 4 | 0x08,
				     regs[1]);
			dev_warn(ds1307->dev, "voltage drop detected\n");
		}

		/* make sure we are running in 24hour mode */
		if (!(regs[0] & RX8025_BIT_2412)) {
			u8 hour;

			/* switch to 24 hour mode */
			regmap_write(ds1307->regmap,
				     RX8025_REG_CTRL1 << 4 | 0x08,
				     regs[0] | RX8025_BIT_2412);

			err = regmap_bulk_read(ds1307->regmap,
					       RX8025_REG_CTRL1 << 4 | 0x08,
					       regs, 2);
			if (err) {
				dev_dbg(ds1307->dev, "read error %d\n", err);
				goto exit;
			}

			/* correct hour */
			hour = bcd2bin(regs[DS1307_REG_HOUR]);
			if (hour == 12)
				hour = 0;
			if (regs[DS1307_REG_HOUR] & DS1307_BIT_PM)
				hour += 12;

			regmap_write(ds1307->regmap,
				     DS1307_REG_HOUR << 4 | 0x08, hour);
		}
		break;
	case ds_1388:
		err = regmap_read(ds1307->regmap, DS1388_REG_CONTROL, &tmp);
		if (err) {
			dev_dbg(ds1307->dev, "read error %d\n", err);
			goto exit;
		}

		/* oscillator off?  turn it on, so clock can tick. */
		if (tmp & DS1388_BIT_nEOSC) {
			tmp &= ~DS1388_BIT_nEOSC;
			regmap_write(ds1307->regmap, DS1388_REG_CONTROL, tmp);
		}
		break;
	default:
		break;
	}

	/* read RTC registers */
	err = regmap_bulk_read(ds1307->regmap, chip->offset, regs,
			       sizeof(regs));
	if (err) {
		dev_dbg(ds1307->dev, "read error %d\n", err);
		goto exit;
	}

	if (ds1307->type == mcp794xx &&
	    !(regs[DS1307_REG_WDAY] & MCP794XX_BIT_VBATEN)) {
		regmap_write(ds1307->regmap, DS1307_REG_WDAY,
			     regs[DS1307_REG_WDAY] |
			     MCP794XX_BIT_VBATEN);
	}

	tmp = regs[DS1307_REG_HOUR];
	switch (ds1307->type) {
	case ds_1340:
	case m41t0:
	case m41t00:
	case m41t11:
		/*
		 * NOTE: ignores century bits; fix before deploying
		 * systems that will run through year 2100.
		 */
		break;
	case rx_8025:
		break;
	default:
		if (!(tmp & DS1307_BIT_12HR))
			break;

		/*
		 * Be sure we're in 24 hour mode.  Multi-master systems
		 * take note...
		 */
		tmp = bcd2bin(tmp & 0x1f);
		if (tmp == 12)
			tmp = 0;
		if (regs[DS1307_REG_HOUR] & DS1307_BIT_PM)
			tmp += 12;
		regmap_write(ds1307->regmap, chip->offset + DS1307_REG_HOUR,
			     bin2bcd(tmp));
	}

	ds1307->rtc = devm_rtc_allocate_device(ds1307->dev);
	if (IS_ERR(ds1307->rtc))
		return PTR_ERR(ds1307->rtc);

	if (want_irq || ds1307_can_wakeup_device)
		device_set_wakeup_capable(ds1307->dev, true);
	else
		clear_bit(RTC_FEATURE_ALARM, ds1307->rtc->features);

	if (ds1307_can_wakeup_device && !want_irq) {
		dev_info(ds1307->dev,
			 "'wakeup-source' is set, request for an IRQ is disabled!\n");
		/* We cannot support UIE mode if we do not have an IRQ line */
		ds1307->rtc->uie_unsupported = 1;
	}

	if (want_irq) {
		err = devm_request_threaded_irq(ds1307->dev, client->irq, NULL,
						chip->irq_handler ?: ds1307_irq,
						IRQF_SHARED | IRQF_ONESHOT,
						ds1307->name, ds1307);
		if (err) {
			client->irq = 0;
			device_set_wakeup_capable(ds1307->dev, false);
			clear_bit(RTC_FEATURE_ALARM, ds1307->rtc->features);
			dev_err(ds1307->dev, "unable to request IRQ!\n");
		} else {
			dev_dbg(ds1307->dev, "got IRQ %d\n", client->irq);
		}
	}

	ds1307->rtc->ops = chip->rtc_ops ?: &ds13xx_rtc_ops;
	err = ds1307_add_frequency_test(ds1307);
	if (err)
		return err;

	err = devm_rtc_register_device(ds1307->rtc);
	if (err)
		return err;

	if (chip->nvram_size) {
		struct nvmem_config nvmem_cfg = {
			.name = "ds1307_nvram",
			.word_size = 1,
			.stride = 1,
			.size = chip->nvram_size,
			.reg_read = ds1307_nvram_read,
			.reg_write = ds1307_nvram_write,
			.priv = ds1307,
		};

		devm_rtc_nvmem_register(ds1307->rtc, &nvmem_cfg);
	}

	ds1307_hwmon_register(ds1307);
	ds1307_clks_register(ds1307);
	ds1307_wdt_register(ds1307);

	return 0;

exit:
	return err;
}

static struct i2c_driver ds1307_driver = {
	.driver = {
		.name	= "rtc-ds1307",
		.of_match_table = ds1307_of_match,
	},
	.probe		= ds1307_probe,
	.id_table	= ds1307_id,
};

module_i2c_driver(ds1307_driver);

MODULE_DESCRIPTION("RTC driver for DS1307 and similar chips");
MODULE_LICENSE("GPL");
