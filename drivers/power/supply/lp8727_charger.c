// SPDX-License-Identifier: GPL-2.0-only
/*
 * Driver for LP8727 Micro/Mini USB IC with integrated charger
 *
 *			Copyright (C) 2011 Texas Instruments
 *			Copyright (C) 2011 National Semiconductor
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/power_supply.h>
#include <linux/platform_data/lp8727.h>
#include <linux/of.h>

#define LP8788_NUM_INTREGS	2
#define DEFAULT_DEBOUNCE_MSEC	270

/* Registers */
#define LP8727_CTRL1		0x1
#define LP8727_CTRL2		0x2
#define LP8727_SWCTRL		0x3
#define LP8727_INT1		0x4
#define LP8727_INT2		0x5
#define LP8727_STATUS1		0x6
#define LP8727_STATUS2		0x7
#define LP8727_CHGCTRL2		0x9

/* CTRL1 register */
#define LP8727_CP_EN		BIT(0)
#define LP8727_ADC_EN		BIT(1)
#define LP8727_ID200_EN		BIT(4)

/* CTRL2 register */
#define LP8727_CHGDET_EN	BIT(1)
#define LP8727_INT_EN		BIT(6)

/* SWCTRL register */
#define LP8727_SW_DM1_DM	(0x0 << 0)
#define LP8727_SW_DM1_HiZ	(0x7 << 0)
#define LP8727_SW_DP2_DP	(0x0 << 3)
#define LP8727_SW_DP2_HiZ	(0x7 << 3)

/* INT1 register */
#define LP8727_IDNO		(0xF << 0)
#define LP8727_VBUS		BIT(4)

/* STATUS1 register */
#define LP8727_CHGSTAT		(3 << 4)
#define LP8727_CHPORT		BIT(6)
#define LP8727_DCPORT		BIT(7)
#define LP8727_STAT_EOC		0x30

/* STATUS2 register */
#define LP8727_TEMP_STAT	(3 << 5)
#define LP8727_TEMP_SHIFT	5

/* CHGCTRL2 register */
#define LP8727_ICHG_SHIFT	4

enum lp8727_dev_id {
	LP8727_ID_NONE,
	LP8727_ID_TA,
	LP8727_ID_DEDICATED_CHG,
	LP8727_ID_USB_CHG,
	LP8727_ID_USB_DS,
	LP8727_ID_MAX,
};

enum lp8727_die_temp {
	LP8788_TEMP_75C,
	LP8788_TEMP_95C,
	LP8788_TEMP_115C,
	LP8788_TEMP_135C,
};

struct lp8727_psy {
	struct power_supply *ac;
	struct power_supply *usb;
	struct power_supply *batt;
};

struct lp8727_chg {
	struct device *dev;
	struct i2c_client *client;
	struct mutex xfer_lock;
	struct lp8727_psy *psy;
	struct lp8727_platform_data *pdata;

	/* Charger Data */
	enum lp8727_dev_id devid;
	struct lp8727_chg_param *chg_param;

	/* Interrupt Handling */
	int irq;
	struct delayed_work work;
	unsigned long debounce_jiffies;
};

static int lp8727_read_bytes(struct lp8727_chg *pchg, u8 reg, u8 *data, u8 len)
{
	s32 ret;

	mutex_lock(&pchg->xfer_lock);
	ret = i2c_smbus_read_i2c_block_data(pchg->client, reg, len, data);
	mutex_unlock(&pchg->xfer_lock);

	return (ret != len) ? -EIO : 0;
}

static inline int lp8727_read_byte(struct lp8727_chg *pchg, u8 reg, u8 *data)
{
	return lp8727_read_bytes(pchg, reg, data, 1);
}

static int lp8727_write_byte(struct lp8727_chg *pchg, u8 reg, u8 data)
{
	int ret;

	mutex_lock(&pchg->xfer_lock);
	ret = i2c_smbus_write_byte_data(pchg->client, reg, data);
	mutex_unlock(&pchg->xfer_lock);

	return ret;
}

static bool lp8727_is_charger_attached(const char *name, int id)
{
	if (!strcmp(name, "ac"))
		return id == LP8727_ID_TA || id == LP8727_ID_DEDICATED_CHG;
	else if (!strcmp(name, "usb"))
		return id == LP8727_ID_USB_CHG;

	return id >= LP8727_ID_TA && id <= LP8727_ID_USB_CHG;
}

static int lp8727_init_device(struct lp8727_chg *pchg)
{
	u8 val;
	int ret;
	u8 intstat[LP8788_NUM_INTREGS];

	/* clear interrupts */
	ret = lp8727_read_bytes(pchg, LP8727_INT1, intstat, LP8788_NUM_INTREGS);
	if (ret)
		return ret;

	val = LP8727_ID200_EN | LP8727_ADC_EN | LP8727_CP_EN;
	ret = lp8727_write_byte(pchg, LP8727_CTRL1, val);
	if (ret)
		return ret;

	val = LP8727_INT_EN | LP8727_CHGDET_EN;
	return lp8727_write_byte(pchg, LP8727_CTRL2, val);
}

static int lp8727_is_dedicated_charger(struct lp8727_chg *pchg)
{
	u8 val;

	lp8727_read_byte(pchg, LP8727_STATUS1, &val);
	return val & LP8727_DCPORT;
}

static int lp8727_is_usb_charger(struct lp8727_chg *pchg)
{
	u8 val;

	lp8727_read_byte(pchg, LP8727_STATUS1, &val);
	return val & LP8727_CHPORT;
}

static inline void lp8727_ctrl_switch(struct lp8727_chg *pchg, u8 sw)
{
	lp8727_write_byte(pchg, LP8727_SWCTRL, sw);
}

static void lp8727_id_detection(struct lp8727_chg *pchg, u8 id, int vbusin)
{
	struct lp8727_platform_data *pdata = pchg->pdata;
	u8 devid = LP8727_ID_NONE;
	u8 swctrl = LP8727_SW_DM1_HiZ | LP8727_SW_DP2_HiZ;

	switch (id) {
	case 0x5:
		devid = LP8727_ID_TA;
		pchg->chg_param = pdata ? pdata->ac : NULL;
		break;
	case 0xB:
		if (lp8727_is_dedicated_charger(pchg)) {
			pchg->chg_param = pdata ? pdata->ac : NULL;
			devid = LP8727_ID_DEDICATED_CHG;
		} else if (lp8727_is_usb_charger(pchg)) {
			pchg->chg_param = pdata ? pdata->usb : NULL;
			devid = LP8727_ID_USB_CHG;
			swctrl = LP8727_SW_DM1_DM | LP8727_SW_DP2_DP;
		} else if (vbusin) {
			devid = LP8727_ID_USB_DS;
			swctrl = LP8727_SW_DM1_DM | LP8727_SW_DP2_DP;
		}
		break;
	default:
		devid = LP8727_ID_NONE;
		pchg->chg_param = NULL;
		break;
	}

	pchg->devid = devid;
	lp8727_ctrl_switch(pchg, swctrl);
}

static void lp8727_enable_chgdet(struct lp8727_chg *pchg)
{
	u8 val;

	lp8727_read_byte(pchg, LP8727_CTRL2, &val);
	val |= LP8727_CHGDET_EN;
	lp8727_write_byte(pchg, LP8727_CTRL2, val);
}

static void lp8727_delayed_func(struct work_struct *_work)
{
	struct lp8727_chg *pchg = container_of(_work, struct lp8727_chg,
						work.work);
	u8 intstat[LP8788_NUM_INTREGS];
	u8 idno;
	u8 vbus;

	if (lp8727_read_bytes(pchg, LP8727_INT1, intstat, LP8788_NUM_INTREGS)) {
		dev_err(pchg->dev, "can not read INT registers\n");
		return;
	}

	idno = intstat[0] & LP8727_IDNO;
	vbus = intstat[0] & LP8727_VBUS;

	lp8727_id_detection(pchg, idno, vbus);
	lp8727_enable_chgdet(pchg);

	power_supply_changed(pchg->psy->ac);
	power_supply_changed(pchg->psy->usb);
	power_supply_changed(pchg->psy->batt);
}

static irqreturn_t lp8727_isr_func(int irq, void *ptr)
{
	struct lp8727_chg *pchg = ptr;

	schedule_delayed_work(&pchg->work, pchg->debounce_jiffies);
	return IRQ_HANDLED;
}

static int lp8727_setup_irq(struct lp8727_chg *pchg)
{
	int ret;
	int irq = pchg->client->irq;
	unsigned delay_msec = pchg->pdata ? pchg->pdata->debounce_msec :
						DEFAULT_DEBOUNCE_MSEC;

	INIT_DELAYED_WORK(&pchg->work, lp8727_delayed_func);

	if (irq <= 0) {
		dev_warn(pchg->dev, "invalid irq number: %d\n", irq);
		return 0;
	}

	ret = request_threaded_irq(irq,	NULL, lp8727_isr_func,
				IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
				"lp8727_irq", pchg);

	if (ret)
		return ret;

	pchg->irq = irq;
	pchg->debounce_jiffies = msecs_to_jiffies(delay_msec);

	return 0;
}

static void lp8727_release_irq(struct lp8727_chg *pchg)
{
	cancel_delayed_work_sync(&pchg->work);

	if (pchg->irq)
		free_irq(pchg->irq, pchg);
}

static enum power_supply_property lp8727_charger_prop[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static enum power_supply_property lp8727_battery_prop[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_TEMP,
};

static char *battery_supplied_to[] = {
	"main_batt",
};

static int lp8727_charger_get_property(struct power_supply *psy,
				       enum power_supply_property psp,
				       union power_supply_propval *val)
{
	struct lp8727_chg *pchg = dev_get_drvdata(psy->dev.parent);

	if (psp != POWER_SUPPLY_PROP_ONLINE)
		return -EINVAL;

	val->intval = lp8727_is_charger_attached(psy->desc->name, pchg->devid);

	return 0;
}

static bool lp8727_is_high_temperature(enum lp8727_die_temp temp)
{
	switch (temp) {
	case LP8788_TEMP_95C:
	case LP8788_TEMP_115C:
	case LP8788_TEMP_135C:
		return true;
	default:
		return false;
	}
}

static int lp8727_battery_get_property(struct power_supply *psy,
				       enum power_supply_property psp,
				       union power_supply_propval *val)
{
	struct lp8727_chg *pchg = dev_get_drvdata(psy->dev.parent);
	struct lp8727_platform_data *pdata = pchg->pdata;
	enum lp8727_die_temp temp;
	u8 read;

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		if (!lp8727_is_charger_attached(psy->desc->name, pchg->devid)) {
			val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
			return 0;
		}

		lp8727_read_byte(pchg, LP8727_STATUS1, &read);

		val->intval = (read & LP8727_CHGSTAT) == LP8727_STAT_EOC ?
				POWER_SUPPLY_STATUS_FULL :
				POWER_SUPPLY_STATUS_CHARGING;
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		lp8727_read_byte(pchg, LP8727_STATUS2, &read);
		temp = (read & LP8727_TEMP_STAT) >> LP8727_TEMP_SHIFT;

		val->intval = lp8727_is_high_temperature(temp) ?
			POWER_SUPPLY_HEALTH_OVERHEAT :
			POWER_SUPPLY_HEALTH_GOOD;
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		if (!pdata)
			return -EINVAL;

		if (pdata->get_batt_present)
			val->intval = pdata->get_batt_present();
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		if (!pdata)
			return -EINVAL;

		if (pdata->get_batt_level)
			val->intval = pdata->get_batt_level();
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		if (!pdata)
			return -EINVAL;

		if (pdata->get_batt_capacity)
			val->intval = pdata->get_batt_capacity();
		break;
	case POWER_SUPPLY_PROP_TEMP:
		if (!pdata)
			return -EINVAL;

		if (pdata->get_batt_temp)
			val->intval = pdata->get_batt_temp();
		break;
	default:
		break;
	}

	return 0;
}

static void lp8727_charger_changed(struct power_supply *psy)
{
	struct lp8727_chg *pchg = dev_get_drvdata(psy->dev.parent);
	u8 eoc_level;
	u8 ichg;
	u8 val;

	/* skip if no charger exists */
	if (!lp8727_is_charger_attached(psy->desc->name, pchg->devid))
		return;

	/* update charging parameters */
	if (pchg->chg_param) {
		eoc_level = pchg->chg_param->eoc_level;
		ichg = pchg->chg_param->ichg;
		val = (ichg << LP8727_ICHG_SHIFT) | eoc_level;
		lp8727_write_byte(pchg, LP8727_CHGCTRL2, val);
	}
}

static const struct power_supply_desc lp8727_ac_desc = {
	.name			= "ac",
	.type			= POWER_SUPPLY_TYPE_MAINS,
	.properties		= lp8727_charger_prop,
	.num_properties		= ARRAY_SIZE(lp8727_charger_prop),
	.get_property		= lp8727_charger_get_property,
};

static const struct power_supply_desc lp8727_usb_desc = {
	.name			= "usb",
	.type			= POWER_SUPPLY_TYPE_USB,
	.properties		= lp8727_charger_prop,
	.num_properties		= ARRAY_SIZE(lp8727_charger_prop),
	.get_property		= lp8727_charger_get_property,
};

static const struct power_supply_desc lp8727_batt_desc = {
	.name			= "main_batt",
	.type			= POWER_SUPPLY_TYPE_BATTERY,
	.properties		= lp8727_battery_prop,
	.num_properties		= ARRAY_SIZE(lp8727_battery_prop),
	.get_property		= lp8727_battery_get_property,
	.external_power_changed	= lp8727_charger_changed,
};

static int lp8727_register_psy(struct lp8727_chg *pchg)
{
	struct power_supply_config psy_cfg = {}; /* Only for ac and usb */
	struct lp8727_psy *psy;

	psy = devm_kzalloc(pchg->dev, sizeof(*psy), GFP_KERNEL);
	if (!psy)
		return -ENOMEM;

	pchg->psy = psy;

	psy_cfg.supplied_to = battery_supplied_to;
	psy_cfg.num_supplicants = ARRAY_SIZE(battery_supplied_to);

	psy->ac = power_supply_register(pchg->dev, &lp8727_ac_desc, &psy_cfg);
	if (IS_ERR(psy->ac))
		goto err_psy_ac;

	psy->usb = power_supply_register(pchg->dev, &lp8727_usb_desc,
					 &psy_cfg);
	if (IS_ERR(psy->usb))
		goto err_psy_usb;

	psy->batt = power_supply_register(pchg->dev, &lp8727_batt_desc, NULL);
	if (IS_ERR(psy->batt))
		goto err_psy_batt;

	return 0;

err_psy_batt:
	power_supply_unregister(psy->usb);
err_psy_usb:
	power_supply_unregister(psy->ac);
err_psy_ac:
	return -EPERM;
}

static void lp8727_unregister_psy(struct lp8727_chg *pchg)
{
	struct lp8727_psy *psy = pchg->psy;

	if (!psy)
		return;

	power_supply_unregister(psy->ac);
	power_supply_unregister(psy->usb);
	power_supply_unregister(psy->batt);
}

#ifdef CONFIG_OF
static struct lp8727_chg_param
*lp8727_parse_charge_pdata(struct device *dev, struct device_node *np)
{
	struct lp8727_chg_param *param;

	param = devm_kzalloc(dev, sizeof(*param), GFP_KERNEL);
	if (!param)
		goto out;

	of_property_read_u8(np, "eoc-level", (u8 *)&param->eoc_level);
	of_property_read_u8(np, "charging-current", (u8 *)&param->ichg);
out:
	return param;
}

static struct lp8727_platform_data *lp8727_parse_dt(struct device *dev)
{
	struct device_node *np = dev->of_node;
	struct device_node *child;
	struct lp8727_platform_data *pdata;
	const char *type;

	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return ERR_PTR(-ENOMEM);

	of_property_read_u32(np, "debounce-ms", &pdata->debounce_msec);

	/* If charging parameter is not defined, just skip parsing the dt */
	if (of_get_child_count(np) == 0)
		return pdata;

	for_each_child_of_node(np, child) {
		of_property_read_string(child, "charger-type", &type);

		if (!strcmp(type, "ac"))
			pdata->ac = lp8727_parse_charge_pdata(dev, child);

		if (!strcmp(type, "usb"))
			pdata->usb = lp8727_parse_charge_pdata(dev, child);
	}

	return pdata;
}
#else
static struct lp8727_platform_data *lp8727_parse_dt(struct device *dev)
{
	return NULL;
}
#endif

static int lp8727_probe(struct i2c_client *cl, const struct i2c_device_id *id)
{
	struct lp8727_chg *pchg;
	struct lp8727_platform_data *pdata;
	int ret;

	if (!i2c_check_functionality(cl->adapter, I2C_FUNC_SMBUS_I2C_BLOCK))
		return -EIO;

	if (cl->dev.of_node) {
		pdata = lp8727_parse_dt(&cl->dev);
		if (IS_ERR(pdata))
			return PTR_ERR(pdata);
	} else {
		pdata = dev_get_platdata(&cl->dev);
	}

	pchg = devm_kzalloc(&cl->dev, sizeof(*pchg), GFP_KERNEL);
	if (!pchg)
		return -ENOMEM;

	pchg->client = cl;
	pchg->dev = &cl->dev;
	pchg->pdata = pdata;
	i2c_set_clientdata(cl, pchg);

	mutex_init(&pchg->xfer_lock);

	ret = lp8727_init_device(pchg);
	if (ret) {
		dev_err(pchg->dev, "i2c communication err: %d", ret);
		return ret;
	}

	ret = lp8727_register_psy(pchg);
	if (ret) {
		dev_err(pchg->dev, "power supplies register err: %d", ret);
		return ret;
	}

	ret = lp8727_setup_irq(pchg);
	if (ret) {
		dev_err(pchg->dev, "irq handler err: %d", ret);
		lp8727_unregister_psy(pchg);
		return ret;
	}

	return 0;
}

static int lp8727_remove(struct i2c_client *cl)
{
	struct lp8727_chg *pchg = i2c_get_clientdata(cl);

	lp8727_release_irq(pchg);
	lp8727_unregister_psy(pchg);
	return 0;
}

static const struct of_device_id lp8727_dt_ids[] = {
	{ .compatible = "ti,lp8727", },
	{ }
};
MODULE_DEVICE_TABLE(of, lp8727_dt_ids);

static const struct i2c_device_id lp8727_ids[] = {
	{"lp8727", 0},
	{ }
};
MODULE_DEVICE_TABLE(i2c, lp8727_ids);

static struct i2c_driver lp8727_driver = {
	.driver = {
		   .name = "lp8727",
		   .of_match_table = of_match_ptr(lp8727_dt_ids),
		   },
	.probe = lp8727_probe,
	.remove = lp8727_remove,
	.id_table = lp8727_ids,
};
module_i2c_driver(lp8727_driver);

MODULE_DESCRIPTION("TI/National Semiconductor LP8727 charger driver");
MODULE_AUTHOR("Milo Kim <milo.kim@ti.com>, Daniel Jeong <daniel.jeong@ti.com>");
MODULE_LICENSE("GPL");
