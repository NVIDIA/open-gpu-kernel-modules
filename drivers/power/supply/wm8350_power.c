// SPDX-License-Identifier: GPL-2.0-only
/*
 * Battery driver for wm8350 PMIC
 *
 * Copyright 2007, 2008 Wolfson Microelectronics PLC.
 *
 * Based on OLPC Battery Driver
 *
 * Copyright 2006  David Woodhouse <dwmw2@infradead.org>
 */

#include <linux/module.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/mfd/wm8350/supply.h>
#include <linux/mfd/wm8350/core.h>
#include <linux/mfd/wm8350/comparator.h>

static int wm8350_read_battery_uvolts(struct wm8350 *wm8350)
{
	return wm8350_read_auxadc(wm8350, WM8350_AUXADC_BATT, 0, 0)
		* WM8350_AUX_COEFF;
}

static int wm8350_read_line_uvolts(struct wm8350 *wm8350)
{
	return wm8350_read_auxadc(wm8350, WM8350_AUXADC_LINE, 0, 0)
		* WM8350_AUX_COEFF;
}

static int wm8350_read_usb_uvolts(struct wm8350 *wm8350)
{
	return wm8350_read_auxadc(wm8350, WM8350_AUXADC_USB, 0, 0)
		* WM8350_AUX_COEFF;
}

#define WM8350_BATT_SUPPLY	1
#define WM8350_USB_SUPPLY	2
#define WM8350_LINE_SUPPLY	4

static inline int wm8350_charge_time_min(struct wm8350 *wm8350, int min)
{
	if (!wm8350->power.rev_g_coeff)
		return (((min - 30) / 15) & 0xf) << 8;
	else
		return (((min - 30) / 30) & 0xf) << 8;
}

static int wm8350_get_supplies(struct wm8350 *wm8350)
{
	u16 sm, ov, co, chrg;
	int supplies = 0;

	sm = wm8350_reg_read(wm8350, WM8350_STATE_MACHINE_STATUS);
	ov = wm8350_reg_read(wm8350, WM8350_MISC_OVERRIDES);
	co = wm8350_reg_read(wm8350, WM8350_COMPARATOR_OVERRIDES);
	chrg = wm8350_reg_read(wm8350, WM8350_BATTERY_CHARGER_CONTROL_2);

	/* USB_SM */
	sm = (sm & WM8350_USB_SM_MASK) >> WM8350_USB_SM_SHIFT;

	/* CHG_ISEL */
	chrg &= WM8350_CHG_ISEL_MASK;

	/* If the USB state machine is active then we're using that with or
	 * without battery, otherwise check for wall supply */
	if (((sm == WM8350_USB_SM_100_SLV) ||
	     (sm == WM8350_USB_SM_500_SLV) ||
	     (sm == WM8350_USB_SM_STDBY_SLV))
	    && !(ov & WM8350_USB_LIMIT_OVRDE))
		supplies = WM8350_USB_SUPPLY;
	else if (((sm == WM8350_USB_SM_100_SLV) ||
		  (sm == WM8350_USB_SM_500_SLV) ||
		  (sm == WM8350_USB_SM_STDBY_SLV))
		 && (ov & WM8350_USB_LIMIT_OVRDE) && (chrg == 0))
		supplies = WM8350_USB_SUPPLY | WM8350_BATT_SUPPLY;
	else if (co & WM8350_WALL_FB_OVRDE)
		supplies = WM8350_LINE_SUPPLY;
	else
		supplies = WM8350_BATT_SUPPLY;

	return supplies;
}

static int wm8350_charger_config(struct wm8350 *wm8350,
				 struct wm8350_charger_policy *policy)
{
	u16 reg, eoc_mA, fast_limit_mA;

	if (!policy) {
		dev_warn(wm8350->dev,
			 "No charger policy, charger not configured.\n");
		return -EINVAL;
	}

	/* make sure USB fast charge current is not > 500mA */
	if (policy->fast_limit_USB_mA > 500) {
		dev_err(wm8350->dev, "USB fast charge > 500mA\n");
		return -EINVAL;
	}

	eoc_mA = WM8350_CHG_EOC_mA(policy->eoc_mA);

	wm8350_reg_unlock(wm8350);

	reg = wm8350_reg_read(wm8350, WM8350_BATTERY_CHARGER_CONTROL_1)
		& WM8350_CHG_ENA_R168;
	wm8350_reg_write(wm8350, WM8350_BATTERY_CHARGER_CONTROL_1,
			 reg | eoc_mA | policy->trickle_start_mV |
			 WM8350_CHG_TRICKLE_TEMP_CHOKE |
			 WM8350_CHG_TRICKLE_USB_CHOKE |
			 WM8350_CHG_FAST_USB_THROTTLE);

	if (wm8350_get_supplies(wm8350) & WM8350_USB_SUPPLY) {
		fast_limit_mA =
			WM8350_CHG_FAST_LIMIT_mA(policy->fast_limit_USB_mA);
		wm8350_reg_write(wm8350, WM8350_BATTERY_CHARGER_CONTROL_2,
			    policy->charge_mV | policy->trickle_charge_USB_mA |
			    fast_limit_mA | wm8350_charge_time_min(wm8350,
						policy->charge_timeout));

	} else {
		fast_limit_mA =
			WM8350_CHG_FAST_LIMIT_mA(policy->fast_limit_mA);
		wm8350_reg_write(wm8350, WM8350_BATTERY_CHARGER_CONTROL_2,
			    policy->charge_mV | policy->trickle_charge_mA |
			    fast_limit_mA | wm8350_charge_time_min(wm8350,
						policy->charge_timeout));
	}

	wm8350_reg_lock(wm8350);
	return 0;
}

static int wm8350_batt_status(struct wm8350 *wm8350)
{
	u16 state;

	state = wm8350_reg_read(wm8350, WM8350_BATTERY_CHARGER_CONTROL_2);
	state &= WM8350_CHG_STS_MASK;

	switch (state) {
	case WM8350_CHG_STS_OFF:
		return POWER_SUPPLY_STATUS_DISCHARGING;

	case WM8350_CHG_STS_TRICKLE:
	case WM8350_CHG_STS_FAST:
		return POWER_SUPPLY_STATUS_CHARGING;

	default:
		return POWER_SUPPLY_STATUS_UNKNOWN;
	}
}

static ssize_t charger_state_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct wm8350 *wm8350 = dev_get_drvdata(dev);
	char *charge;
	int state;

	state = wm8350_reg_read(wm8350, WM8350_BATTERY_CHARGER_CONTROL_2) &
	    WM8350_CHG_STS_MASK;
	switch (state) {
	case WM8350_CHG_STS_OFF:
		charge = "Charger Off";
		break;
	case WM8350_CHG_STS_TRICKLE:
		charge = "Trickle Charging";
		break;
	case WM8350_CHG_STS_FAST:
		charge = "Fast Charging";
		break;
	default:
		return 0;
	}

	return sprintf(buf, "%s\n", charge);
}

static DEVICE_ATTR_RO(charger_state);

static irqreturn_t wm8350_charger_handler(int irq, void *data)
{
	struct wm8350 *wm8350 = data;
	struct wm8350_power *power = &wm8350->power;
	struct wm8350_charger_policy *policy = power->policy;

	switch (irq - wm8350->irq_base) {
	case WM8350_IRQ_CHG_BAT_FAIL:
		dev_err(wm8350->dev, "battery failed\n");
		break;
	case WM8350_IRQ_CHG_TO:
		dev_err(wm8350->dev, "charger timeout\n");
		power_supply_changed(power->battery);
		break;

	case WM8350_IRQ_CHG_BAT_HOT:
	case WM8350_IRQ_CHG_BAT_COLD:
	case WM8350_IRQ_CHG_START:
	case WM8350_IRQ_CHG_END:
		power_supply_changed(power->battery);
		break;

	case WM8350_IRQ_CHG_FAST_RDY:
		dev_dbg(wm8350->dev, "fast charger ready\n");
		wm8350_charger_config(wm8350, policy);
		wm8350_reg_unlock(wm8350);
		wm8350_set_bits(wm8350, WM8350_BATTERY_CHARGER_CONTROL_1,
				WM8350_CHG_FAST);
		wm8350_reg_lock(wm8350);
		break;

	case WM8350_IRQ_CHG_VBATT_LT_3P9:
		dev_warn(wm8350->dev, "battery < 3.9V\n");
		break;
	case WM8350_IRQ_CHG_VBATT_LT_3P1:
		dev_warn(wm8350->dev, "battery < 3.1V\n");
		break;
	case WM8350_IRQ_CHG_VBATT_LT_2P85:
		dev_warn(wm8350->dev, "battery < 2.85V\n");
		break;

		/* Supply change.  We will overnotify but it should do
		 * no harm. */
	case WM8350_IRQ_EXT_USB_FB:
	case WM8350_IRQ_EXT_WALL_FB:
		wm8350_charger_config(wm8350, policy);
		fallthrough;
	case WM8350_IRQ_EXT_BAT_FB:
		power_supply_changed(power->battery);
		power_supply_changed(power->usb);
		power_supply_changed(power->ac);
		break;

	default:
		dev_err(wm8350->dev, "Unknown interrupt %d\n", irq);
	}

	return IRQ_HANDLED;
}

/*********************************************************************
 *		AC Power
 *********************************************************************/
static int wm8350_ac_get_prop(struct power_supply *psy,
			      enum power_supply_property psp,
			      union power_supply_propval *val)
{
	struct wm8350 *wm8350 = dev_get_drvdata(psy->dev.parent);
	int ret = 0;

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = !!(wm8350_get_supplies(wm8350) &
				 WM8350_LINE_SUPPLY);
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = wm8350_read_line_uvolts(wm8350);
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

static enum power_supply_property wm8350_ac_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
};

/*********************************************************************
 *		USB Power
 *********************************************************************/
static int wm8350_usb_get_prop(struct power_supply *psy,
			       enum power_supply_property psp,
			       union power_supply_propval *val)
{
	struct wm8350 *wm8350 = dev_get_drvdata(psy->dev.parent);
	int ret = 0;

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = !!(wm8350_get_supplies(wm8350) &
				 WM8350_USB_SUPPLY);
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = wm8350_read_usb_uvolts(wm8350);
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

static enum power_supply_property wm8350_usb_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
};

/*********************************************************************
 *		Battery properties
 *********************************************************************/

static int wm8350_bat_check_health(struct wm8350 *wm8350)
{
	u16 reg;

	if (wm8350_read_battery_uvolts(wm8350) < 2850000)
		return POWER_SUPPLY_HEALTH_UNSPEC_FAILURE;

	reg = wm8350_reg_read(wm8350, WM8350_CHARGER_OVERRIDES);
	if (reg & WM8350_CHG_BATT_HOT_OVRDE)
		return POWER_SUPPLY_HEALTH_OVERHEAT;

	if (reg & WM8350_CHG_BATT_COLD_OVRDE)
		return POWER_SUPPLY_HEALTH_COLD;

	return POWER_SUPPLY_HEALTH_GOOD;
}

static int wm8350_bat_get_charge_type(struct wm8350 *wm8350)
{
	int state;

	state = wm8350_reg_read(wm8350, WM8350_BATTERY_CHARGER_CONTROL_2) &
	    WM8350_CHG_STS_MASK;
	switch (state) {
	case WM8350_CHG_STS_OFF:
		return POWER_SUPPLY_CHARGE_TYPE_NONE;
	case WM8350_CHG_STS_TRICKLE:
		return POWER_SUPPLY_CHARGE_TYPE_TRICKLE;
	case WM8350_CHG_STS_FAST:
		return POWER_SUPPLY_CHARGE_TYPE_FAST;
	default:
		return POWER_SUPPLY_CHARGE_TYPE_UNKNOWN;
	}
}

static int wm8350_bat_get_property(struct power_supply *psy,
				   enum power_supply_property psp,
				   union power_supply_propval *val)
{
	struct wm8350 *wm8350 = dev_get_drvdata(psy->dev.parent);
	int ret = 0;

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = wm8350_batt_status(wm8350);
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = !!(wm8350_get_supplies(wm8350) &
				 WM8350_BATT_SUPPLY);
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = wm8350_read_battery_uvolts(wm8350);
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		val->intval = wm8350_bat_check_health(wm8350);
		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		val->intval = wm8350_bat_get_charge_type(wm8350);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

static enum power_supply_property wm8350_bat_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_CHARGE_TYPE,
};

static const struct power_supply_desc wm8350_ac_desc = {
	.name		= "wm8350-ac",
	.type		= POWER_SUPPLY_TYPE_MAINS,
	.properties	= wm8350_ac_props,
	.num_properties	= ARRAY_SIZE(wm8350_ac_props),
	.get_property	= wm8350_ac_get_prop,
};

static const struct power_supply_desc wm8350_battery_desc = {
	.name		= "wm8350-battery",
	.properties	= wm8350_bat_props,
	.num_properties	= ARRAY_SIZE(wm8350_bat_props),
	.get_property	= wm8350_bat_get_property,
	.use_for_apm	= 1,
};

static const struct power_supply_desc wm8350_usb_desc = {
	.name		= "wm8350-usb",
	.type		= POWER_SUPPLY_TYPE_USB,
	.properties	= wm8350_usb_props,
	.num_properties	= ARRAY_SIZE(wm8350_usb_props),
	.get_property	= wm8350_usb_get_prop,
};

/*********************************************************************
 *		Initialisation
 *********************************************************************/

static void wm8350_init_charger(struct wm8350 *wm8350)
{
	/* register our interest in charger events */
	wm8350_register_irq(wm8350, WM8350_IRQ_CHG_BAT_HOT,
			    wm8350_charger_handler, 0, "Battery hot", wm8350);
	wm8350_register_irq(wm8350, WM8350_IRQ_CHG_BAT_COLD,
			    wm8350_charger_handler, 0, "Battery cold", wm8350);
	wm8350_register_irq(wm8350, WM8350_IRQ_CHG_BAT_FAIL,
			    wm8350_charger_handler, 0, "Battery fail", wm8350);
	wm8350_register_irq(wm8350, WM8350_IRQ_CHG_TO,
			    wm8350_charger_handler, 0,
			    "Charger timeout", wm8350);
	wm8350_register_irq(wm8350, WM8350_IRQ_CHG_END,
			    wm8350_charger_handler, 0,
			    "Charge end", wm8350);
	wm8350_register_irq(wm8350, WM8350_IRQ_CHG_START,
			    wm8350_charger_handler, 0,
			    "Charge start", wm8350);
	wm8350_register_irq(wm8350, WM8350_IRQ_CHG_FAST_RDY,
			    wm8350_charger_handler, 0,
			    "Fast charge ready", wm8350);
	wm8350_register_irq(wm8350, WM8350_IRQ_CHG_VBATT_LT_3P9,
			    wm8350_charger_handler, 0,
			    "Battery <3.9V", wm8350);
	wm8350_register_irq(wm8350, WM8350_IRQ_CHG_VBATT_LT_3P1,
			    wm8350_charger_handler, 0,
			    "Battery <3.1V", wm8350);
	wm8350_register_irq(wm8350, WM8350_IRQ_CHG_VBATT_LT_2P85,
			    wm8350_charger_handler, 0,
			    "Battery <2.85V", wm8350);

	/* and supply change events */
	wm8350_register_irq(wm8350, WM8350_IRQ_EXT_USB_FB,
			    wm8350_charger_handler, 0, "USB", wm8350);
	wm8350_register_irq(wm8350, WM8350_IRQ_EXT_WALL_FB,
			    wm8350_charger_handler, 0, "Wall", wm8350);
	wm8350_register_irq(wm8350, WM8350_IRQ_EXT_BAT_FB,
			    wm8350_charger_handler, 0, "Battery", wm8350);
}

static void free_charger_irq(struct wm8350 *wm8350)
{
	wm8350_free_irq(wm8350, WM8350_IRQ_CHG_BAT_HOT, wm8350);
	wm8350_free_irq(wm8350, WM8350_IRQ_CHG_BAT_COLD, wm8350);
	wm8350_free_irq(wm8350, WM8350_IRQ_CHG_BAT_FAIL, wm8350);
	wm8350_free_irq(wm8350, WM8350_IRQ_CHG_TO, wm8350);
	wm8350_free_irq(wm8350, WM8350_IRQ_CHG_END, wm8350);
	wm8350_free_irq(wm8350, WM8350_IRQ_CHG_START, wm8350);
	wm8350_free_irq(wm8350, WM8350_IRQ_CHG_VBATT_LT_3P9, wm8350);
	wm8350_free_irq(wm8350, WM8350_IRQ_CHG_VBATT_LT_3P1, wm8350);
	wm8350_free_irq(wm8350, WM8350_IRQ_CHG_VBATT_LT_2P85, wm8350);
	wm8350_free_irq(wm8350, WM8350_IRQ_EXT_USB_FB, wm8350);
	wm8350_free_irq(wm8350, WM8350_IRQ_EXT_WALL_FB, wm8350);
	wm8350_free_irq(wm8350, WM8350_IRQ_EXT_BAT_FB, wm8350);
}

static int wm8350_power_probe(struct platform_device *pdev)
{
	struct wm8350 *wm8350 = platform_get_drvdata(pdev);
	struct wm8350_power *power = &wm8350->power;
	struct wm8350_charger_policy *policy = power->policy;
	int ret;

	power->ac = power_supply_register(&pdev->dev, &wm8350_ac_desc, NULL);
	if (IS_ERR(power->ac))
		return PTR_ERR(power->ac);

	power->battery = power_supply_register(&pdev->dev, &wm8350_battery_desc,
					       NULL);
	if (IS_ERR(power->battery)) {
		ret = PTR_ERR(power->battery);
		goto battery_failed;
	}

	power->usb = power_supply_register(&pdev->dev, &wm8350_usb_desc, NULL);
	if (IS_ERR(power->usb)) {
		ret = PTR_ERR(power->usb);
		goto usb_failed;
	}

	ret = device_create_file(&pdev->dev, &dev_attr_charger_state);
	if (ret < 0)
		dev_warn(wm8350->dev, "failed to add charge sysfs: %d\n", ret);
	ret = 0;

	wm8350_init_charger(wm8350);
	if (wm8350_charger_config(wm8350, policy) == 0) {
		wm8350_reg_unlock(wm8350);
		wm8350_set_bits(wm8350, WM8350_POWER_MGMT_5, WM8350_CHG_ENA);
		wm8350_reg_lock(wm8350);
	}

	return ret;

usb_failed:
	power_supply_unregister(power->battery);
battery_failed:
	power_supply_unregister(power->ac);

	return ret;
}

static int wm8350_power_remove(struct platform_device *pdev)
{
	struct wm8350 *wm8350 = platform_get_drvdata(pdev);
	struct wm8350_power *power = &wm8350->power;

	free_charger_irq(wm8350);
	device_remove_file(&pdev->dev, &dev_attr_charger_state);
	power_supply_unregister(power->battery);
	power_supply_unregister(power->ac);
	power_supply_unregister(power->usb);
	return 0;
}

static struct platform_driver wm8350_power_driver = {
	.probe = wm8350_power_probe,
	.remove = wm8350_power_remove,
	.driver = {
		.name = "wm8350-power",
	},
};

module_platform_driver(wm8350_power_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Power supply driver for WM8350");
MODULE_ALIAS("platform:wm8350-power");
