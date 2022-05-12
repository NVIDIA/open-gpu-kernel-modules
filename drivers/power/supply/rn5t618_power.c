// SPDX-License-Identifier: GPL-2.0+
/*
 * Power supply driver for the RICOH RN5T618 power management chip family
 *
 * Copyright (C) 2020 Andreas Kemnade
 */

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/bitops.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/mfd/rn5t618.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/regmap.h>
#include <linux/slab.h>

#define CHG_STATE_ADP_INPUT 0x40
#define CHG_STATE_USB_INPUT 0x80
#define CHG_STATE_MASK	0x1f
#define CHG_STATE_CHG_OFF	0
#define CHG_STATE_CHG_READY_VADP	1
#define CHG_STATE_CHG_TRICKLE	2
#define CHG_STATE_CHG_RAPID	3
#define CHG_STATE_CHG_COMPLETE	4
#define CHG_STATE_SUSPEND	5
#define CHG_STATE_VCHG_OVER_VOL	6
#define CHG_STATE_BAT_ERROR	7
#define CHG_STATE_NO_BAT	8
#define CHG_STATE_BAT_OVER_VOL	9
#define CHG_STATE_BAT_TEMP_ERR	10
#define CHG_STATE_DIE_ERR	11
#define CHG_STATE_DIE_SHUTDOWN	12
#define CHG_STATE_NO_BAT2	13
#define CHG_STATE_CHG_READY_VUSB	14

#define FG_ENABLE 1

struct rn5t618_power_info {
	struct rn5t618 *rn5t618;
	struct platform_device *pdev;
	struct power_supply *battery;
	struct power_supply *usb;
	struct power_supply *adp;
	int irq;
};

static enum power_supply_property rn5t618_usb_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_ONLINE,
};

static enum power_supply_property rn5t618_adp_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_ONLINE,
};


static enum power_supply_property rn5t618_battery_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_TEMP,
	POWER_SUPPLY_PROP_TIME_TO_EMPTY_NOW,
	POWER_SUPPLY_PROP_TIME_TO_FULL_NOW,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_CHARGE_FULL,
	POWER_SUPPLY_PROP_CHARGE_NOW,
};

static int rn5t618_battery_read_doublereg(struct rn5t618_power_info *info,
					  u8 reg, u16 *result)
{
	int ret, i;
	u8 data[2];
	u16 old, new;

	old = 0;
	/* Prevent races when registers are changing. */
	for (i = 0; i < 3; i++) {
		ret = regmap_bulk_read(info->rn5t618->regmap,
				       reg, data, sizeof(data));
		if (ret)
			return ret;

		new = data[0] << 8;
		new |= data[1];
		if (new == old)
			break;

		old = new;
	}

	*result = new;

	return 0;
}

static int rn5t618_decode_status(unsigned int status)
{
	switch (status & CHG_STATE_MASK) {
	case CHG_STATE_CHG_OFF:
	case CHG_STATE_SUSPEND:
	case CHG_STATE_VCHG_OVER_VOL:
	case CHG_STATE_DIE_SHUTDOWN:
		return POWER_SUPPLY_STATUS_DISCHARGING;

	case CHG_STATE_CHG_TRICKLE:
	case CHG_STATE_CHG_RAPID:
		return POWER_SUPPLY_STATUS_CHARGING;

	case CHG_STATE_CHG_COMPLETE:
		return POWER_SUPPLY_STATUS_FULL;

	default:
		return POWER_SUPPLY_STATUS_NOT_CHARGING;
	}
}

static int rn5t618_battery_status(struct rn5t618_power_info *info,
				  union power_supply_propval *val)
{
	unsigned int v;
	int ret;

	ret = regmap_read(info->rn5t618->regmap, RN5T618_CHGSTATE, &v);
	if (ret)
		return ret;

	val->intval = POWER_SUPPLY_STATUS_UNKNOWN;

	if (v & 0xc0) { /* USB or ADP plugged */
		val->intval = rn5t618_decode_status(v);
	} else
		val->intval = POWER_SUPPLY_STATUS_DISCHARGING;

	return ret;
}

static int rn5t618_battery_present(struct rn5t618_power_info *info,
				   union power_supply_propval *val)
{
	unsigned int v;
	int ret;

	ret = regmap_read(info->rn5t618->regmap, RN5T618_CHGSTATE, &v);
	if (ret)
		return ret;

	v &= CHG_STATE_MASK;
	if ((v == CHG_STATE_NO_BAT) || (v == CHG_STATE_NO_BAT2))
		val->intval = 0;
	else
		val->intval = 1;

	return ret;
}

static int rn5t618_battery_voltage_now(struct rn5t618_power_info *info,
				       union power_supply_propval *val)
{
	u16 res;
	int ret;

	ret = rn5t618_battery_read_doublereg(info, RN5T618_VOLTAGE_1, &res);
	if (ret)
		return ret;

	val->intval = res * 2 * 2500 / 4095 * 1000;

	return 0;
}

static int rn5t618_battery_current_now(struct rn5t618_power_info *info,
				       union power_supply_propval *val)
{
	u16 res;
	int ret;

	ret = rn5t618_battery_read_doublereg(info, RN5T618_CC_AVEREG1, &res);
	if (ret)
		return ret;

	/* current is negative when discharging */
	val->intval = sign_extend32(res, 13) * 1000;

	return 0;
}

static int rn5t618_battery_capacity(struct rn5t618_power_info *info,
				    union power_supply_propval *val)
{
	unsigned int v;
	int ret;

	ret = regmap_read(info->rn5t618->regmap, RN5T618_SOC, &v);
	if (ret)
		return ret;

	val->intval = v;

	return 0;
}

static int rn5t618_battery_temp(struct rn5t618_power_info *info,
				union power_supply_propval *val)
{
	u16 res;
	int ret;

	ret = rn5t618_battery_read_doublereg(info, RN5T618_TEMP_1, &res);
	if (ret)
		return ret;

	val->intval = sign_extend32(res, 11) * 10 / 16;

	return 0;
}

static int rn5t618_battery_tte(struct rn5t618_power_info *info,
			       union power_supply_propval *val)
{
	u16 res;
	int ret;

	ret = rn5t618_battery_read_doublereg(info, RN5T618_TT_EMPTY_H, &res);
	if (ret)
		return ret;

	if (res == 65535)
		return -ENODATA;

	val->intval = res * 60;

	return 0;
}

static int rn5t618_battery_ttf(struct rn5t618_power_info *info,
			       union power_supply_propval *val)
{
	u16 res;
	int ret;

	ret = rn5t618_battery_read_doublereg(info, RN5T618_TT_FULL_H, &res);
	if (ret)
		return ret;

	if (res == 65535)
		return -ENODATA;

	val->intval = res * 60;

	return 0;
}

static int rn5t618_battery_charge_full(struct rn5t618_power_info *info,
				       union power_supply_propval *val)
{
	u16 res;
	int ret;

	ret = rn5t618_battery_read_doublereg(info, RN5T618_FA_CAP_H, &res);
	if (ret)
		return ret;

	val->intval = res * 1000;

	return 0;
}

static int rn5t618_battery_charge_now(struct rn5t618_power_info *info,
				      union power_supply_propval *val)
{
	u16 res;
	int ret;

	ret = rn5t618_battery_read_doublereg(info, RN5T618_RE_CAP_H, &res);
	if (ret)
		return ret;

	val->intval = res * 1000;

	return 0;
}

static int rn5t618_battery_get_property(struct power_supply *psy,
					enum power_supply_property psp,
					union power_supply_propval *val)
{
	int ret = 0;
	struct rn5t618_power_info *info = power_supply_get_drvdata(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		ret = rn5t618_battery_status(info, val);
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		ret = rn5t618_battery_present(info, val);
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		ret = rn5t618_battery_voltage_now(info, val);
		break;
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		ret = rn5t618_battery_current_now(info, val);
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		ret = rn5t618_battery_capacity(info, val);
		break;
	case POWER_SUPPLY_PROP_TEMP:
		ret = rn5t618_battery_temp(info, val);
		break;
	case POWER_SUPPLY_PROP_TIME_TO_EMPTY_NOW:
		ret = rn5t618_battery_tte(info, val);
		break;
	case POWER_SUPPLY_PROP_TIME_TO_FULL_NOW:
		ret = rn5t618_battery_ttf(info, val);
		break;
	case POWER_SUPPLY_PROP_TECHNOLOGY:
		val->intval = POWER_SUPPLY_TECHNOLOGY_LION;
		break;
	case POWER_SUPPLY_PROP_CHARGE_FULL:
		ret = rn5t618_battery_charge_full(info, val);
		break;
	case POWER_SUPPLY_PROP_CHARGE_NOW:
		ret = rn5t618_battery_charge_now(info, val);
		break;
	default:
		return -EINVAL;
	}

	return ret;
}

static int rn5t618_adp_get_property(struct power_supply *psy,
				    enum power_supply_property psp,
				    union power_supply_propval *val)
{
	struct rn5t618_power_info *info = power_supply_get_drvdata(psy);
	unsigned int chgstate;
	bool online;
	int ret;

	ret = regmap_read(info->rn5t618->regmap, RN5T618_CHGSTATE, &chgstate);
	if (ret)
		return ret;

	online = !!(chgstate & CHG_STATE_ADP_INPUT);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = online;
		break;
	case POWER_SUPPLY_PROP_STATUS:
		if (!online) {
			val->intval = POWER_SUPPLY_STATUS_NOT_CHARGING;
			break;
		}
		val->intval = rn5t618_decode_status(chgstate);
		if (val->intval != POWER_SUPPLY_STATUS_CHARGING)
			val->intval = POWER_SUPPLY_STATUS_NOT_CHARGING;

		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int rn5t618_usb_get_property(struct power_supply *psy,
				    enum power_supply_property psp,
				    union power_supply_propval *val)
{
	struct rn5t618_power_info *info = power_supply_get_drvdata(psy);
	unsigned int chgstate;
	bool online;
	int ret;

	ret = regmap_read(info->rn5t618->regmap, RN5T618_CHGSTATE, &chgstate);
	if (ret)
		return ret;

	online = !!(chgstate & CHG_STATE_USB_INPUT);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = online;
		break;
	case POWER_SUPPLY_PROP_STATUS:
		if (!online) {
			val->intval = POWER_SUPPLY_STATUS_NOT_CHARGING;
			break;
		}
		val->intval = rn5t618_decode_status(chgstate);
		if (val->intval != POWER_SUPPLY_STATUS_CHARGING)
			val->intval = POWER_SUPPLY_STATUS_NOT_CHARGING;

		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static const struct power_supply_desc rn5t618_battery_desc = {
	.name                   = "rn5t618-battery",
	.type                   = POWER_SUPPLY_TYPE_BATTERY,
	.properties             = rn5t618_battery_props,
	.num_properties         = ARRAY_SIZE(rn5t618_battery_props),
	.get_property           = rn5t618_battery_get_property,
};

static const struct power_supply_desc rn5t618_adp_desc = {
	.name                   = "rn5t618-adp",
	.type                   = POWER_SUPPLY_TYPE_MAINS,
	.properties             = rn5t618_adp_props,
	.num_properties         = ARRAY_SIZE(rn5t618_adp_props),
	.get_property           = rn5t618_adp_get_property,
};

static const struct power_supply_desc rn5t618_usb_desc = {
	.name                   = "rn5t618-usb",
	.type                   = POWER_SUPPLY_TYPE_USB,
	.properties             = rn5t618_usb_props,
	.num_properties         = ARRAY_SIZE(rn5t618_usb_props),
	.get_property           = rn5t618_usb_get_property,
};

static irqreturn_t rn5t618_charger_irq(int irq, void *data)
{
	struct device *dev = data;
	struct rn5t618_power_info *info = dev_get_drvdata(dev);

	unsigned int ctrl, stat1, stat2, err;

	regmap_read(info->rn5t618->regmap, RN5T618_CHGERR_IRR, &err);
	regmap_read(info->rn5t618->regmap, RN5T618_CHGCTRL_IRR, &ctrl);
	regmap_read(info->rn5t618->regmap, RN5T618_CHGSTAT_IRR1, &stat1);
	regmap_read(info->rn5t618->regmap, RN5T618_CHGSTAT_IRR2, &stat2);

	regmap_write(info->rn5t618->regmap, RN5T618_CHGERR_IRR, 0);
	regmap_write(info->rn5t618->regmap, RN5T618_CHGCTRL_IRR, 0);
	regmap_write(info->rn5t618->regmap, RN5T618_CHGSTAT_IRR1, 0);
	regmap_write(info->rn5t618->regmap, RN5T618_CHGSTAT_IRR2, 0);

	dev_dbg(dev, "chgerr: %x chgctrl: %x chgstat: %x chgstat2: %x\n",
		err, ctrl, stat1, stat2);

	power_supply_changed(info->usb);
	power_supply_changed(info->adp);
	power_supply_changed(info->battery);

	return IRQ_HANDLED;
}

static int rn5t618_power_probe(struct platform_device *pdev)
{
	int ret = 0;
	unsigned int v;
	struct power_supply_config psy_cfg = {};
	struct rn5t618_power_info *info;

	info = devm_kzalloc(&pdev->dev, sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->pdev = pdev;
	info->rn5t618 = dev_get_drvdata(pdev->dev.parent);
	info->irq = -1;

	platform_set_drvdata(pdev, info);

	ret = regmap_read(info->rn5t618->regmap, RN5T618_CONTROL, &v);
	if (ret)
		return ret;

	if (!(v & FG_ENABLE)) {
		/* E.g. the vendor kernels of various Kobo and Tolino Ebook
		 * readers disable the fuel gauge on shutdown. If a kernel
		 * without fuel gauge support is booted after that, the fuel
		 * gauge will get decalibrated.
		 */
		dev_info(&pdev->dev, "Fuel gauge not enabled, enabling now\n");
		dev_info(&pdev->dev, "Expect imprecise results\n");
		regmap_update_bits(info->rn5t618->regmap, RN5T618_CONTROL,
				   FG_ENABLE, FG_ENABLE);
	}

	psy_cfg.drv_data = info;
	info->battery = devm_power_supply_register(&pdev->dev,
						   &rn5t618_battery_desc,
						   &psy_cfg);
	if (IS_ERR(info->battery)) {
		ret = PTR_ERR(info->battery);
		dev_err(&pdev->dev, "failed to register battery: %d\n", ret);
		return ret;
	}

	info->adp = devm_power_supply_register(&pdev->dev,
					       &rn5t618_adp_desc,
					       &psy_cfg);
	if (IS_ERR(info->adp)) {
		ret = PTR_ERR(info->adp);
		dev_err(&pdev->dev, "failed to register adp: %d\n", ret);
		return ret;
	}

	info->usb = devm_power_supply_register(&pdev->dev,
					       &rn5t618_usb_desc,
					       &psy_cfg);
	if (IS_ERR(info->usb)) {
		ret = PTR_ERR(info->usb);
		dev_err(&pdev->dev, "failed to register usb: %d\n", ret);
		return ret;
	}

	if (info->rn5t618->irq_data)
		info->irq = regmap_irq_get_virq(info->rn5t618->irq_data,
						RN5T618_IRQ_CHG);

	if (info->irq < 0)
		info->irq = -1;
	else {
		ret = devm_request_threaded_irq(&pdev->dev, info->irq, NULL,
						rn5t618_charger_irq,
						IRQF_ONESHOT,
						"rn5t618_power",
						&pdev->dev);

		if (ret < 0) {
			dev_err(&pdev->dev, "request IRQ:%d fail\n",
				info->irq);
			info->irq = -1;
		}
	}

	return 0;
}

static struct platform_driver rn5t618_power_driver = {
	.driver = {
		.name   = "rn5t618-power",
	},
	.probe = rn5t618_power_probe,
};

module_platform_driver(rn5t618_power_driver);
MODULE_ALIAS("platform:rn5t618-power");
MODULE_DESCRIPTION("Power supply driver for RICOH RN5T618");
MODULE_LICENSE("GPL");
