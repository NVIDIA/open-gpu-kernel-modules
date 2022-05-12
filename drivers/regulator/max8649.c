// SPDX-License-Identifier: GPL-2.0-only
/*
 * Regulators driver for Maxim max8649
 *
 * Copyright (C) 2009-2010 Marvell International Ltd.
 *      Haojian Zhuang <haojian.zhuang@marvell.com>
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/slab.h>
#include <linux/regulator/max8649.h>
#include <linux/regmap.h>

#define MAX8649_DCDC_VMIN	750000		/* uV */
#define MAX8649_DCDC_VMAX	1380000		/* uV */
#define MAX8649_DCDC_STEP	10000		/* uV */
#define MAX8649_VOL_MASK	0x3f

/* Registers */
#define MAX8649_MODE0		0x00
#define MAX8649_MODE1		0x01
#define MAX8649_MODE2		0x02
#define MAX8649_MODE3		0x03
#define MAX8649_CONTROL		0x04
#define MAX8649_SYNC		0x05
#define MAX8649_RAMP		0x06
#define MAX8649_CHIP_ID1	0x08
#define MAX8649_CHIP_ID2	0x09

/* Bits */
#define MAX8649_EN_PD		(1 << 7)
#define MAX8649_VID0_PD		(1 << 6)
#define MAX8649_VID1_PD		(1 << 5)
#define MAX8649_VID_MASK	(3 << 5)

#define MAX8649_FORCE_PWM	(1 << 7)
#define MAX8649_SYNC_EXTCLK	(1 << 6)

#define MAX8649_EXT_MASK	(3 << 6)

#define MAX8649_RAMP_MASK	(7 << 5)
#define MAX8649_RAMP_DOWN	(1 << 1)

struct max8649_regulator_info {
	struct device		*dev;
	struct regmap		*regmap;

	unsigned	mode:2;	/* bit[1:0] = VID1, VID0 */
	unsigned	extclk_freq:2;
	unsigned	extclk:1;
	unsigned	ramp_timing:3;
	unsigned	ramp_down:1;
};

static int max8649_enable_time(struct regulator_dev *rdev)
{
	struct max8649_regulator_info *info = rdev_get_drvdata(rdev);
	int voltage, rate, ret;
	unsigned int val;

	/* get voltage */
	ret = regmap_read(info->regmap, rdev->desc->vsel_reg, &val);
	if (ret != 0)
		return ret;
	val &= MAX8649_VOL_MASK;
	voltage = regulator_list_voltage_linear(rdev, (unsigned char)val);

	/* get rate */
	ret = regmap_read(info->regmap, MAX8649_RAMP, &val);
	if (ret != 0)
		return ret;
	ret = (val & MAX8649_RAMP_MASK) >> 5;
	rate = (32 * 1000) >> ret;	/* uV/uS */

	return DIV_ROUND_UP(voltage, rate);
}

static int max8649_set_mode(struct regulator_dev *rdev, unsigned int mode)
{
	struct max8649_regulator_info *info = rdev_get_drvdata(rdev);

	switch (mode) {
	case REGULATOR_MODE_FAST:
		regmap_update_bits(info->regmap, rdev->desc->vsel_reg,
				   MAX8649_FORCE_PWM, MAX8649_FORCE_PWM);
		break;
	case REGULATOR_MODE_NORMAL:
		regmap_update_bits(info->regmap, rdev->desc->vsel_reg,
				   MAX8649_FORCE_PWM, 0);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static unsigned int max8649_get_mode(struct regulator_dev *rdev)
{
	struct max8649_regulator_info *info = rdev_get_drvdata(rdev);
	unsigned int val;
	int ret;

	ret = regmap_read(info->regmap, rdev->desc->vsel_reg, &val);
	if (ret != 0)
		return ret;
	if (val & MAX8649_FORCE_PWM)
		return REGULATOR_MODE_FAST;
	return REGULATOR_MODE_NORMAL;
}

static const struct regulator_ops max8649_dcdc_ops = {
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.list_voltage	= regulator_list_voltage_linear,
	.map_voltage	= regulator_map_voltage_linear,
	.enable		= regulator_enable_regmap,
	.disable	= regulator_disable_regmap,
	.is_enabled	= regulator_is_enabled_regmap,
	.enable_time	= max8649_enable_time,
	.set_mode	= max8649_set_mode,
	.get_mode	= max8649_get_mode,

};

static struct regulator_desc dcdc_desc = {
	.name		= "max8649",
	.ops		= &max8649_dcdc_ops,
	.type		= REGULATOR_VOLTAGE,
	.n_voltages	= 1 << 6,
	.owner		= THIS_MODULE,
	.vsel_mask	= MAX8649_VOL_MASK,
	.min_uV		= MAX8649_DCDC_VMIN,
	.uV_step	= MAX8649_DCDC_STEP,
	.enable_reg	= MAX8649_CONTROL,
	.enable_mask	= MAX8649_EN_PD,
	.enable_is_inverted = true,
};

static const struct regmap_config max8649_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
};

static int max8649_regulator_probe(struct i2c_client *client,
					     const struct i2c_device_id *id)
{
	struct max8649_platform_data *pdata = dev_get_platdata(&client->dev);
	struct max8649_regulator_info *info = NULL;
	struct regulator_dev *regulator;
	struct regulator_config config = { };
	unsigned int val;
	unsigned char data;
	int ret;

	info = devm_kzalloc(&client->dev, sizeof(struct max8649_regulator_info),
			    GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->regmap = devm_regmap_init_i2c(client, &max8649_regmap_config);
	if (IS_ERR(info->regmap)) {
		ret = PTR_ERR(info->regmap);
		dev_err(&client->dev, "Failed to allocate register map: %d\n", ret);
		return ret;
	}

	info->dev = &client->dev;
	i2c_set_clientdata(client, info);

	info->mode = pdata->mode;
	switch (info->mode) {
	case 0:
		dcdc_desc.vsel_reg = MAX8649_MODE0;
		break;
	case 1:
		dcdc_desc.vsel_reg = MAX8649_MODE1;
		break;
	case 2:
		dcdc_desc.vsel_reg = MAX8649_MODE2;
		break;
	case 3:
		dcdc_desc.vsel_reg = MAX8649_MODE3;
		break;
	default:
		break;
	}

	ret = regmap_read(info->regmap, MAX8649_CHIP_ID1, &val);
	if (ret != 0) {
		dev_err(info->dev, "Failed to detect ID of MAX8649:%d\n",
			ret);
		return ret;
	}
	dev_info(info->dev, "Detected MAX8649 (ID:%x)\n", val);

	/* enable VID0 & VID1 */
	regmap_update_bits(info->regmap, MAX8649_CONTROL, MAX8649_VID_MASK, 0);

	/* enable/disable external clock synchronization */
	info->extclk = pdata->extclk;
	data = (info->extclk) ? MAX8649_SYNC_EXTCLK : 0;
	regmap_update_bits(info->regmap, dcdc_desc.vsel_reg,
			   MAX8649_SYNC_EXTCLK, data);
	if (info->extclk) {
		/* set external clock frequency */
		info->extclk_freq = pdata->extclk_freq;
		regmap_update_bits(info->regmap, MAX8649_SYNC, MAX8649_EXT_MASK,
				   info->extclk_freq << 6);
	}

	if (pdata->ramp_timing) {
		info->ramp_timing = pdata->ramp_timing;
		regmap_update_bits(info->regmap, MAX8649_RAMP, MAX8649_RAMP_MASK,
				   info->ramp_timing << 5);
	}

	info->ramp_down = pdata->ramp_down;
	if (info->ramp_down) {
		regmap_update_bits(info->regmap, MAX8649_RAMP, MAX8649_RAMP_DOWN,
				   MAX8649_RAMP_DOWN);
	}

	config.dev = &client->dev;
	config.init_data = pdata->regulator;
	config.driver_data = info;
	config.regmap = info->regmap;

	regulator = devm_regulator_register(&client->dev, &dcdc_desc,
						  &config);
	if (IS_ERR(regulator)) {
		dev_err(info->dev, "failed to register regulator %s\n",
			dcdc_desc.name);
		return PTR_ERR(regulator);
	}

	return 0;
}

static const struct i2c_device_id max8649_id[] = {
	{ "max8649", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, max8649_id);

static struct i2c_driver max8649_driver = {
	.probe		= max8649_regulator_probe,
	.driver		= {
		.name	= "max8649",
	},
	.id_table	= max8649_id,
};

static int __init max8649_init(void)
{
	return i2c_add_driver(&max8649_driver);
}
subsys_initcall(max8649_init);

static void __exit max8649_exit(void)
{
	i2c_del_driver(&max8649_driver);
}
module_exit(max8649_exit);

/* Module information */
MODULE_DESCRIPTION("MAXIM 8649 voltage regulator driver");
MODULE_AUTHOR("Haojian Zhuang <haojian.zhuang@marvell.com>");
MODULE_LICENSE("GPL");
