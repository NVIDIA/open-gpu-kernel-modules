// SPDX-License-Identifier: GPL-2.0-only
/*
 * Regulators driver for Maxim max8925
 *
 * Copyright (C) 2009 Marvell International Ltd.
 *      Haojian Zhuang <haojian.zhuang@marvell.com>
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/mfd/max8925.h>
#include <linux/of.h>
#include <linux/regulator/of_regulator.h>

#define SD1_DVM_VMIN		850000
#define SD1_DVM_VMAX		1000000
#define SD1_DVM_STEP		50000
#define SD1_DVM_SHIFT		5		/* SDCTL1 bit5 */
#define SD1_DVM_EN		6		/* SDV1 bit 6 */

/* bit definitions in LDO control registers */
#define LDO_SEQ_I2C		0x7		/* Power U/D by i2c */
#define LDO_SEQ_MASK		0x7		/* Power U/D sequence mask */
#define LDO_SEQ_SHIFT		2		/* Power U/D sequence offset */
#define LDO_I2C_EN		0x1		/* Enable by i2c */
#define LDO_I2C_EN_MASK		0x1		/* Enable mask by i2c */
#define LDO_I2C_EN_SHIFT	0		/* Enable offset by i2c */

struct max8925_regulator_info {
	struct regulator_desc	desc;
	struct i2c_client	*i2c;

	int	vol_reg;
	int	enable_reg;
};

static int max8925_set_voltage_sel(struct regulator_dev *rdev,
				   unsigned int selector)
{
	struct max8925_regulator_info *info = rdev_get_drvdata(rdev);
	unsigned char mask = rdev->desc->n_voltages - 1;

	return max8925_set_bits(info->i2c, info->vol_reg, mask, selector);
}

static int max8925_get_voltage_sel(struct regulator_dev *rdev)
{
	struct max8925_regulator_info *info = rdev_get_drvdata(rdev);
	unsigned char data, mask;
	int ret;

	ret = max8925_reg_read(info->i2c, info->vol_reg);
	if (ret < 0)
		return ret;
	mask = rdev->desc->n_voltages - 1;
	data = ret & mask;

	return data;
}

static int max8925_enable(struct regulator_dev *rdev)
{
	struct max8925_regulator_info *info = rdev_get_drvdata(rdev);

	return max8925_set_bits(info->i2c, info->enable_reg,
				LDO_SEQ_MASK << LDO_SEQ_SHIFT |
				LDO_I2C_EN_MASK << LDO_I2C_EN_SHIFT,
				LDO_SEQ_I2C << LDO_SEQ_SHIFT |
				LDO_I2C_EN << LDO_I2C_EN_SHIFT);
}

static int max8925_disable(struct regulator_dev *rdev)
{
	struct max8925_regulator_info *info = rdev_get_drvdata(rdev);

	return max8925_set_bits(info->i2c, info->enable_reg,
				LDO_SEQ_MASK << LDO_SEQ_SHIFT |
				LDO_I2C_EN_MASK << LDO_I2C_EN_SHIFT,
				LDO_SEQ_I2C << LDO_SEQ_SHIFT);
}

static int max8925_is_enabled(struct regulator_dev *rdev)
{
	struct max8925_regulator_info *info = rdev_get_drvdata(rdev);
	int ldo_seq, ret;

	ret = max8925_reg_read(info->i2c, info->enable_reg);
	if (ret < 0)
		return ret;
	ldo_seq = (ret >> LDO_SEQ_SHIFT) & LDO_SEQ_MASK;
	if (ldo_seq != LDO_SEQ_I2C)
		return 1;
	else
		return ret & (LDO_I2C_EN_MASK << LDO_I2C_EN_SHIFT);
}

static int max8925_set_dvm_voltage(struct regulator_dev *rdev, int uV)
{
	struct max8925_regulator_info *info = rdev_get_drvdata(rdev);
	unsigned char data, mask;

	if (uV < SD1_DVM_VMIN || uV > SD1_DVM_VMAX)
		return -EINVAL;

	data = DIV_ROUND_UP(uV - SD1_DVM_VMIN, SD1_DVM_STEP);
	data <<= SD1_DVM_SHIFT;
	mask = 3 << SD1_DVM_SHIFT;

	return max8925_set_bits(info->i2c, info->enable_reg, mask, data);
}

static int max8925_set_dvm_enable(struct regulator_dev *rdev)
{
	struct max8925_regulator_info *info = rdev_get_drvdata(rdev);

	return max8925_set_bits(info->i2c, info->vol_reg, 1 << SD1_DVM_EN,
				1 << SD1_DVM_EN);
}

static int max8925_set_dvm_disable(struct regulator_dev *rdev)
{
	struct max8925_regulator_info *info = rdev_get_drvdata(rdev);

	return max8925_set_bits(info->i2c, info->vol_reg, 1 << SD1_DVM_EN, 0);
}

static const struct regulator_ops max8925_regulator_sdv_ops = {
	.map_voltage		= regulator_map_voltage_linear,
	.list_voltage		= regulator_list_voltage_linear,
	.set_voltage_sel	= max8925_set_voltage_sel,
	.get_voltage_sel	= max8925_get_voltage_sel,
	.enable			= max8925_enable,
	.disable		= max8925_disable,
	.is_enabled		= max8925_is_enabled,
	.set_suspend_voltage	= max8925_set_dvm_voltage,
	.set_suspend_enable	= max8925_set_dvm_enable,
	.set_suspend_disable	= max8925_set_dvm_disable,
};

static const struct regulator_ops max8925_regulator_ldo_ops = {
	.map_voltage		= regulator_map_voltage_linear,
	.list_voltage		= regulator_list_voltage_linear,
	.set_voltage_sel	= max8925_set_voltage_sel,
	.get_voltage_sel	= max8925_get_voltage_sel,
	.enable			= max8925_enable,
	.disable		= max8925_disable,
	.is_enabled		= max8925_is_enabled,
};

#define MAX8925_SDV(_id, min, max, step)			\
{								\
	.desc	= {						\
		.name	= "SDV" #_id,				\
		.of_match = of_match_ptr("SDV" #_id),		\
		.regulators_node = of_match_ptr("regulators"),	\
		.ops	= &max8925_regulator_sdv_ops,		\
		.type	= REGULATOR_VOLTAGE,			\
		.id	= MAX8925_ID_SD##_id,			\
		.owner	= THIS_MODULE,				\
		.n_voltages = 64,				\
		.min_uV = min * 1000,				\
		.uV_step = step * 1000,				\
	},							\
	.vol_reg	= MAX8925_SDV##_id,			\
	.enable_reg	= MAX8925_SDCTL##_id,			\
}

#define MAX8925_LDO(_id, min, max, step)			\
{								\
	.desc	= {						\
		.name	= "LDO" #_id,				\
		.of_match = of_match_ptr("LDO" #_id),		\
		.regulators_node = of_match_ptr("regulators"),	\
		.ops	= &max8925_regulator_ldo_ops,		\
		.type	= REGULATOR_VOLTAGE,			\
		.id	= MAX8925_ID_LDO##_id,			\
		.owner	= THIS_MODULE,				\
		.n_voltages = 64,				\
		.min_uV = min * 1000,				\
		.uV_step = step * 1000,				\
	},							\
	.vol_reg	= MAX8925_LDOVOUT##_id,			\
	.enable_reg	= MAX8925_LDOCTL##_id,			\
}

static struct max8925_regulator_info max8925_regulator_info[] = {
	MAX8925_SDV(1, 637.5, 1425, 12.5),
	MAX8925_SDV(2,   650, 2225,   25),
	MAX8925_SDV(3,   750, 3900,   50),

	MAX8925_LDO(1,  750, 3900, 50),
	MAX8925_LDO(2,  650, 2250, 25),
	MAX8925_LDO(3,  650, 2250, 25),
	MAX8925_LDO(4,  750, 3900, 50),
	MAX8925_LDO(5,  750, 3900, 50),
	MAX8925_LDO(6,  750, 3900, 50),
	MAX8925_LDO(7,  750, 3900, 50),
	MAX8925_LDO(8,  750, 3900, 50),
	MAX8925_LDO(9,  750, 3900, 50),
	MAX8925_LDO(10, 750, 3900, 50),
	MAX8925_LDO(11, 750, 3900, 50),
	MAX8925_LDO(12, 750, 3900, 50),
	MAX8925_LDO(13, 750, 3900, 50),
	MAX8925_LDO(14, 750, 3900, 50),
	MAX8925_LDO(15, 750, 3900, 50),
	MAX8925_LDO(16, 750, 3900, 50),
	MAX8925_LDO(17, 650, 2250, 25),
	MAX8925_LDO(18, 650, 2250, 25),
	MAX8925_LDO(19, 750, 3900, 50),
	MAX8925_LDO(20, 750, 3900, 50),
};

static int max8925_regulator_probe(struct platform_device *pdev)
{
	struct max8925_chip *chip = dev_get_drvdata(pdev->dev.parent);
	struct regulator_init_data *pdata = dev_get_platdata(&pdev->dev);
	struct regulator_config config = { };
	struct max8925_regulator_info *ri;
	struct resource *res;
	struct regulator_dev *rdev;
	int i;

	res = platform_get_resource(pdev, IORESOURCE_REG, 0);
	if (!res) {
		dev_err(&pdev->dev, "No REG resource!\n");
		return -EINVAL;
	}
	for (i = 0; i < ARRAY_SIZE(max8925_regulator_info); i++) {
		ri = &max8925_regulator_info[i];
		if (ri->vol_reg == res->start)
			break;
	}

	if (i == ARRAY_SIZE(max8925_regulator_info)) {
		dev_err(&pdev->dev, "Failed to find regulator %llu\n",
			(unsigned long long)res->start);
		return -EINVAL;
	}
	ri->i2c = chip->i2c;

	config.dev = chip->dev;
	config.driver_data = ri;

	if (pdata)
		config.init_data = pdata;

	rdev = devm_regulator_register(&pdev->dev, &ri->desc, &config);
	if (IS_ERR(rdev)) {
		dev_err(&pdev->dev, "failed to register regulator %s\n",
				ri->desc.name);
		return PTR_ERR(rdev);
	}

	platform_set_drvdata(pdev, rdev);
	return 0;
}

static struct platform_driver max8925_regulator_driver = {
	.driver		= {
		.name	= "max8925-regulator",
	},
	.probe		= max8925_regulator_probe,
};

static int __init max8925_regulator_init(void)
{
	return platform_driver_register(&max8925_regulator_driver);
}
subsys_initcall(max8925_regulator_init);

static void __exit max8925_regulator_exit(void)
{
	platform_driver_unregister(&max8925_regulator_driver);
}
module_exit(max8925_regulator_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haojian Zhuang <haojian.zhuang@marvell.com>");
MODULE_DESCRIPTION("Regulator Driver for Maxim 8925 PMIC");
MODULE_ALIAS("platform:max8925-regulator");
