// SPDX-License-Identifier: GPL-2.0-only
/*
 * Regulator driver for Ricoh RN5T618 PMIC
 *
 * Copyright (C) 2014 Beniamino Galvani <b.galvani@gmail.com>
 */

#include <linux/mfd/rn5t618.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/of_regulator.h>

static const struct regulator_ops rn5t618_reg_ops = {
	.enable			= regulator_enable_regmap,
	.disable		= regulator_disable_regmap,
	.is_enabled		= regulator_is_enabled_regmap,
	.set_voltage_sel	= regulator_set_voltage_sel_regmap,
	.get_voltage_sel	= regulator_get_voltage_sel_regmap,
	.list_voltage		= regulator_list_voltage_linear,
};

#define REG(rid, ereg, emask, vreg, vmask, min, max, step)		\
	{								\
		.name		= #rid,					\
		.of_match	= of_match_ptr(#rid),			\
		.regulators_node = of_match_ptr("regulators"),		\
		.id		= RN5T618_##rid,			\
		.type		= REGULATOR_VOLTAGE,			\
		.owner		= THIS_MODULE,				\
		.ops		= &rn5t618_reg_ops,			\
		.n_voltages	= ((max) - (min)) / (step) + 1,		\
		.min_uV		= (min),				\
		.uV_step	= (step),				\
		.enable_reg	= RN5T618_##ereg,			\
		.enable_mask	= (emask),				\
		.vsel_reg	= RN5T618_##vreg,			\
		.vsel_mask	= (vmask),				\
	}

static const struct regulator_desc rn5t567_regulators[] = {
	/* DCDC */
	REG(DCDC1, DC1CTL, BIT(0), DC1DAC, 0xff, 600000, 3500000, 12500),
	REG(DCDC2, DC2CTL, BIT(0), DC2DAC, 0xff, 600000, 3500000, 12500),
	REG(DCDC3, DC3CTL, BIT(0), DC3DAC, 0xff, 600000, 3500000, 12500),
	REG(DCDC4, DC4CTL, BIT(0), DC4DAC, 0xff, 600000, 3500000, 12500),
	/* LDO */
	REG(LDO1, LDOEN1, BIT(0), LDO1DAC, 0x7f, 900000, 3500000, 25000),
	REG(LDO2, LDOEN1, BIT(1), LDO2DAC, 0x7f, 900000, 3500000, 25000),
	REG(LDO3, LDOEN1, BIT(2), LDO3DAC, 0x7f, 600000, 3500000, 25000),
	REG(LDO4, LDOEN1, BIT(3), LDO4DAC, 0x7f, 900000, 3500000, 25000),
	REG(LDO5, LDOEN1, BIT(4), LDO5DAC, 0x7f, 900000, 3500000, 25000),
	/* LDO RTC */
	REG(LDORTC1, LDOEN2, BIT(4), LDORTCDAC, 0x7f, 1200000, 3500000, 25000),
	REG(LDORTC2, LDOEN2, BIT(5), LDORTC2DAC, 0x7f, 900000, 3500000, 25000),
};

static const struct regulator_desc rn5t618_regulators[] = {
	/* DCDC */
	REG(DCDC1, DC1CTL, BIT(0), DC1DAC, 0xff, 600000, 3500000, 12500),
	REG(DCDC2, DC2CTL, BIT(0), DC2DAC, 0xff, 600000, 3500000, 12500),
	REG(DCDC3, DC3CTL, BIT(0), DC3DAC, 0xff, 600000, 3500000, 12500),
	/* LDO */
	REG(LDO1, LDOEN1, BIT(0), LDO1DAC, 0x7f, 900000, 3500000, 25000),
	REG(LDO2, LDOEN1, BIT(1), LDO2DAC, 0x7f, 900000, 3500000, 25000),
	REG(LDO3, LDOEN1, BIT(2), LDO3DAC, 0x7f, 600000, 3500000, 25000),
	REG(LDO4, LDOEN1, BIT(3), LDO4DAC, 0x7f, 900000, 3500000, 25000),
	REG(LDO5, LDOEN1, BIT(4), LDO5DAC, 0x7f, 900000, 3500000, 25000),
	/* LDO RTC */
	REG(LDORTC1, LDOEN2, BIT(4), LDORTCDAC, 0x7f, 1700000, 3500000, 25000),
	REG(LDORTC2, LDOEN2, BIT(5), LDORTC2DAC, 0x7f, 900000, 3500000, 25000),
};

static const struct regulator_desc rc5t619_regulators[] = {
	/* DCDC */
	REG(DCDC1, DC1CTL, BIT(0), DC1DAC, 0xff, 600000, 3500000, 12500),
	REG(DCDC2, DC2CTL, BIT(0), DC2DAC, 0xff, 600000, 3500000, 12500),
	REG(DCDC3, DC3CTL, BIT(0), DC3DAC, 0xff, 600000, 3500000, 12500),
	REG(DCDC4, DC4CTL, BIT(0), DC4DAC, 0xff, 600000, 3500000, 12500),
	REG(DCDC5, DC5CTL, BIT(0), DC5DAC, 0xff, 600000, 3500000, 12500),
	/* LDO */
	REG(LDO1, LDOEN1, BIT(0), LDO1DAC, 0x7f, 900000, 3500000, 25000),
	REG(LDO2, LDOEN1, BIT(1), LDO2DAC, 0x7f, 900000, 3500000, 25000),
	REG(LDO3, LDOEN1, BIT(2), LDO3DAC, 0x7f, 900000, 3500000, 25000),
	REG(LDO4, LDOEN1, BIT(3), LDO4DAC, 0x7f, 900000, 3500000, 25000),
	REG(LDO5, LDOEN1, BIT(4), LDO5DAC, 0x7f, 600000, 3500000, 25000),
	REG(LDO6, LDOEN1, BIT(5), LDO6DAC, 0x7f, 600000, 3500000, 25000),
	REG(LDO7, LDOEN1, BIT(6), LDO7DAC, 0x7f, 900000, 3500000, 25000),
	REG(LDO8, LDOEN1, BIT(7), LDO8DAC, 0x7f, 900000, 3500000, 25000),
	REG(LDO9, LDOEN2, BIT(0), LDO9DAC, 0x7f, 900000, 3500000, 25000),
	REG(LDO10, LDOEN2, BIT(1), LDO10DAC, 0x7f, 900000, 3500000, 25000),
	/* LDO RTC */
	REG(LDORTC1, LDOEN2, BIT(4), LDORTCDAC, 0x7f, 1700000, 3500000, 25000),
	REG(LDORTC2, LDOEN2, BIT(5), LDORTC2DAC, 0x7f, 900000, 3500000, 25000),
};

static int rn5t618_regulator_probe(struct platform_device *pdev)
{
	struct rn5t618 *rn5t618 = dev_get_drvdata(pdev->dev.parent);
	struct regulator_config config = { };
	struct regulator_dev *rdev;
	const struct regulator_desc *regulators;
	int i;
	int num_regulators = 0;

	switch (rn5t618->variant) {
	case RN5T567:
		regulators = rn5t567_regulators;
		num_regulators = ARRAY_SIZE(rn5t567_regulators);
		break;
	case RN5T618:
		regulators = rn5t618_regulators;
		num_regulators = ARRAY_SIZE(rn5t618_regulators);
		break;
	case RC5T619:
		regulators = rc5t619_regulators;
		num_regulators = ARRAY_SIZE(rc5t619_regulators);
		break;
	default:
		return -EINVAL;
	}

	config.dev = pdev->dev.parent;
	config.regmap = rn5t618->regmap;

	for (i = 0; i < num_regulators; i++) {
		rdev = devm_regulator_register(&pdev->dev,
					       &regulators[i],
					       &config);
		if (IS_ERR(rdev)) {
			dev_err(&pdev->dev, "failed to register %s regulator\n",
				regulators[i].name);
			return PTR_ERR(rdev);
		}
	}

	return 0;
}

static struct platform_driver rn5t618_regulator_driver = {
	.probe = rn5t618_regulator_probe,
	.driver = {
		.name	= "rn5t618-regulator",
	},
};

module_platform_driver(rn5t618_regulator_driver);

MODULE_ALIAS("platform:rn5t618-regulator");
MODULE_AUTHOR("Beniamino Galvani <b.galvani@gmail.com>");
MODULE_DESCRIPTION("RN5T618 regulator driver");
MODULE_LICENSE("GPL v2");
