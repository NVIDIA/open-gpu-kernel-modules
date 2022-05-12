// SPDX-License-Identifier: GPL-2.0
// Copyright (C) STMicroelectronics 2018
// Author: Pascal Paillet <p.paillet@st.com> for STMicroelectronics.

#include <linux/interrupt.h>
#include <linux/mfd/stpmic1.h>
#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/of_regulator.h>

#include <dt-bindings/mfd/st,stpmic1.h>

/**
 * struct stpmic1 regulator description: this structure is used as driver data
 * @desc: regulator framework description
 * @mask_reset_reg: mask reset register address
 * @mask_reset_mask: mask rank and mask reset register mask
 * @icc_reg: icc register address
 * @icc_mask: icc register mask
 */
struct stpmic1_regulator_cfg {
	struct regulator_desc desc;
	u8 mask_reset_reg;
	u8 mask_reset_mask;
	u8 icc_reg;
	u8 icc_mask;
};

static int stpmic1_set_mode(struct regulator_dev *rdev, unsigned int mode);
static unsigned int stpmic1_get_mode(struct regulator_dev *rdev);
static int stpmic1_set_icc(struct regulator_dev *rdev);
static unsigned int stpmic1_map_mode(unsigned int mode);

enum {
	STPMIC1_BUCK1 = 0,
	STPMIC1_BUCK2 = 1,
	STPMIC1_BUCK3 = 2,
	STPMIC1_BUCK4 = 3,
	STPMIC1_LDO1 = 4,
	STPMIC1_LDO2 = 5,
	STPMIC1_LDO3 = 6,
	STPMIC1_LDO4 = 7,
	STPMIC1_LDO5 = 8,
	STPMIC1_LDO6 = 9,
	STPMIC1_VREF_DDR = 10,
	STPMIC1_BOOST = 11,
	STPMIC1_VBUS_OTG = 12,
	STPMIC1_SW_OUT = 13,
};

/* Enable time worst case is 5000mV/(2250uV/uS) */
#define PMIC_ENABLE_TIME_US 2200
/* Ramp delay worst case is (2250uV/uS) */
#define PMIC_RAMP_DELAY 2200

static const struct linear_range buck1_ranges[] = {
	REGULATOR_LINEAR_RANGE(725000, 0, 4, 0),
	REGULATOR_LINEAR_RANGE(725000, 5, 36, 25000),
	REGULATOR_LINEAR_RANGE(1500000, 37, 63, 0),
};

static const struct linear_range buck2_ranges[] = {
	REGULATOR_LINEAR_RANGE(1000000, 0, 17, 0),
	REGULATOR_LINEAR_RANGE(1050000, 18, 19, 0),
	REGULATOR_LINEAR_RANGE(1100000, 20, 21, 0),
	REGULATOR_LINEAR_RANGE(1150000, 22, 23, 0),
	REGULATOR_LINEAR_RANGE(1200000, 24, 25, 0),
	REGULATOR_LINEAR_RANGE(1250000, 26, 27, 0),
	REGULATOR_LINEAR_RANGE(1300000, 28, 29, 0),
	REGULATOR_LINEAR_RANGE(1350000, 30, 31, 0),
	REGULATOR_LINEAR_RANGE(1400000, 32, 33, 0),
	REGULATOR_LINEAR_RANGE(1450000, 34, 35, 0),
	REGULATOR_LINEAR_RANGE(1500000, 36, 63, 0),
};

static const struct linear_range buck3_ranges[] = {
	REGULATOR_LINEAR_RANGE(1000000, 0, 19, 0),
	REGULATOR_LINEAR_RANGE(1100000, 20, 23, 0),
	REGULATOR_LINEAR_RANGE(1200000, 24, 27, 0),
	REGULATOR_LINEAR_RANGE(1300000, 28, 31, 0),
	REGULATOR_LINEAR_RANGE(1400000, 32, 35, 0),
	REGULATOR_LINEAR_RANGE(1500000, 36, 55, 100000),
	REGULATOR_LINEAR_RANGE(3400000, 56, 63, 0),
};

static const struct linear_range buck4_ranges[] = {
	REGULATOR_LINEAR_RANGE(600000, 0, 27, 25000),
	REGULATOR_LINEAR_RANGE(1300000, 28, 29, 0),
	REGULATOR_LINEAR_RANGE(1350000, 30, 31, 0),
	REGULATOR_LINEAR_RANGE(1400000, 32, 33, 0),
	REGULATOR_LINEAR_RANGE(1450000, 34, 35, 0),
	REGULATOR_LINEAR_RANGE(1500000, 36, 60, 100000),
	REGULATOR_LINEAR_RANGE(3900000, 61, 63, 0),
};

static const struct linear_range ldo1_ranges[] = {
	REGULATOR_LINEAR_RANGE(1700000, 0, 7, 0),
	REGULATOR_LINEAR_RANGE(1700000, 8, 24, 100000),
	REGULATOR_LINEAR_RANGE(3300000, 25, 31, 0),
};

static const struct linear_range ldo2_ranges[] = {
	REGULATOR_LINEAR_RANGE(1700000, 0, 7, 0),
	REGULATOR_LINEAR_RANGE(1700000, 8, 24, 100000),
	REGULATOR_LINEAR_RANGE(3300000, 25, 30, 0),
};

static const struct linear_range ldo3_ranges[] = {
	REGULATOR_LINEAR_RANGE(1700000, 0, 7, 0),
	REGULATOR_LINEAR_RANGE(1700000, 8, 24, 100000),
	REGULATOR_LINEAR_RANGE(3300000, 25, 30, 0),
	/* with index 31 LDO3 is in DDR mode */
	REGULATOR_LINEAR_RANGE(500000, 31, 31, 0),
};

static const struct linear_range ldo5_ranges[] = {
	REGULATOR_LINEAR_RANGE(1700000, 0, 7, 0),
	REGULATOR_LINEAR_RANGE(1700000, 8, 30, 100000),
	REGULATOR_LINEAR_RANGE(3900000, 31, 31, 0),
};

static const struct linear_range ldo6_ranges[] = {
	REGULATOR_LINEAR_RANGE(900000, 0, 24, 100000),
	REGULATOR_LINEAR_RANGE(3300000, 25, 31, 0),
};

static const struct regulator_ops stpmic1_ldo_ops = {
	.list_voltage = regulator_list_voltage_linear_range,
	.map_voltage = regulator_map_voltage_linear_range,
	.is_enabled = regulator_is_enabled_regmap,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.set_over_current_protection = stpmic1_set_icc,
};

static const struct regulator_ops stpmic1_ldo3_ops = {
	.list_voltage = regulator_list_voltage_linear_range,
	.map_voltage = regulator_map_voltage_iterate,
	.is_enabled = regulator_is_enabled_regmap,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_bypass = regulator_get_bypass_regmap,
	.set_bypass = regulator_set_bypass_regmap,
	.set_over_current_protection = stpmic1_set_icc,
};

static const struct regulator_ops stpmic1_ldo4_fixed_regul_ops = {
	.is_enabled = regulator_is_enabled_regmap,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.set_over_current_protection = stpmic1_set_icc,
};

static const struct regulator_ops stpmic1_buck_ops = {
	.list_voltage = regulator_list_voltage_linear_range,
	.map_voltage = regulator_map_voltage_linear_range,
	.is_enabled = regulator_is_enabled_regmap,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.set_pull_down = regulator_set_pull_down_regmap,
	.set_mode = stpmic1_set_mode,
	.get_mode = stpmic1_get_mode,
	.set_over_current_protection = stpmic1_set_icc,
};

static const struct regulator_ops stpmic1_vref_ddr_ops = {
	.is_enabled = regulator_is_enabled_regmap,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
};

static const struct regulator_ops stpmic1_boost_regul_ops = {
	.is_enabled = regulator_is_enabled_regmap,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.set_over_current_protection = stpmic1_set_icc,
};

static const struct regulator_ops stpmic1_switch_regul_ops = {
	.is_enabled = regulator_is_enabled_regmap,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.set_over_current_protection = stpmic1_set_icc,
	.set_active_discharge = regulator_set_active_discharge_regmap,
};

#define REG_LDO(ids, base) { \
	.name = #ids, \
	.id = STPMIC1_##ids, \
	.n_voltages = 32, \
	.ops = &stpmic1_ldo_ops, \
	.linear_ranges = base ## _ranges, \
	.n_linear_ranges = ARRAY_SIZE(base ## _ranges), \
	.type = REGULATOR_VOLTAGE, \
	.owner = THIS_MODULE, \
	.vsel_reg = ids##_ACTIVE_CR, \
	.vsel_mask = LDO_VOLTAGE_MASK, \
	.enable_reg = ids##_ACTIVE_CR, \
	.enable_mask = LDO_ENABLE_MASK, \
	.enable_val = 1, \
	.disable_val = 0, \
	.enable_time = PMIC_ENABLE_TIME_US, \
	.ramp_delay = PMIC_RAMP_DELAY, \
	.supply_name = #base, \
}

#define REG_LDO3(ids, base) { \
	.name = #ids, \
	.id = STPMIC1_##ids, \
	.n_voltages = 32, \
	.ops = &stpmic1_ldo3_ops, \
	.linear_ranges = ldo3_ranges, \
	.n_linear_ranges = ARRAY_SIZE(ldo3_ranges), \
	.type = REGULATOR_VOLTAGE, \
	.owner = THIS_MODULE, \
	.vsel_reg = LDO3_ACTIVE_CR, \
	.vsel_mask = LDO_VOLTAGE_MASK, \
	.enable_reg = LDO3_ACTIVE_CR, \
	.enable_mask = LDO_ENABLE_MASK, \
	.enable_val = 1, \
	.disable_val = 0, \
	.enable_time = PMIC_ENABLE_TIME_US, \
	.ramp_delay = PMIC_RAMP_DELAY, \
	.bypass_reg = LDO3_ACTIVE_CR, \
	.bypass_mask = LDO_BYPASS_MASK, \
	.bypass_val_on = LDO_BYPASS_MASK, \
	.bypass_val_off = 0, \
	.supply_name = #base, \
}

#define REG_LDO4(ids, base) { \
	.name = #ids, \
	.id = STPMIC1_##ids, \
	.n_voltages = 1, \
	.ops = &stpmic1_ldo4_fixed_regul_ops, \
	.type = REGULATOR_VOLTAGE, \
	.owner = THIS_MODULE, \
	.min_uV = 3300000, \
	.fixed_uV = 3300000, \
	.enable_reg = LDO4_ACTIVE_CR, \
	.enable_mask = LDO_ENABLE_MASK, \
	.enable_val = 1, \
	.disable_val = 0, \
	.enable_time = PMIC_ENABLE_TIME_US, \
	.ramp_delay = PMIC_RAMP_DELAY, \
	.supply_name = #base, \
}

#define REG_BUCK(ids, base) { \
	.name = #ids, \
	.id = STPMIC1_##ids, \
	.ops = &stpmic1_buck_ops, \
	.n_voltages = 64, \
	.linear_ranges = base ## _ranges, \
	.n_linear_ranges = ARRAY_SIZE(base ## _ranges), \
	.type = REGULATOR_VOLTAGE, \
	.owner = THIS_MODULE, \
	.vsel_reg = ids##_ACTIVE_CR, \
	.vsel_mask = BUCK_VOLTAGE_MASK, \
	.enable_reg = ids##_ACTIVE_CR, \
	.enable_mask = BUCK_ENABLE_MASK, \
	.enable_val = 1, \
	.disable_val = 0, \
	.enable_time = PMIC_ENABLE_TIME_US, \
	.ramp_delay = PMIC_RAMP_DELAY, \
	.of_map_mode = stpmic1_map_mode, \
	.pull_down_reg = ids##_PULL_DOWN_REG, \
	.pull_down_mask = ids##_PULL_DOWN_MASK, \
	.supply_name = #base, \
}

#define REG_VREF_DDR(ids, base) { \
	.name = #ids, \
	.id = STPMIC1_##ids, \
	.n_voltages = 1, \
	.ops = &stpmic1_vref_ddr_ops, \
	.type = REGULATOR_VOLTAGE, \
	.owner = THIS_MODULE, \
	.min_uV = 500000, \
	.fixed_uV = 500000, \
	.enable_reg = VREF_DDR_ACTIVE_CR, \
	.enable_mask = BUCK_ENABLE_MASK, \
	.enable_val = 1, \
	.disable_val = 0, \
	.enable_time = PMIC_ENABLE_TIME_US, \
	.supply_name = #base, \
}

#define REG_BOOST(ids, base) { \
	.name = #ids, \
	.id = STPMIC1_##ids, \
	.n_voltages = 1, \
	.ops = &stpmic1_boost_regul_ops, \
	.type = REGULATOR_VOLTAGE, \
	.owner = THIS_MODULE, \
	.min_uV = 0, \
	.fixed_uV = 5000000, \
	.enable_reg = BST_SW_CR, \
	.enable_mask = BOOST_ENABLED, \
	.enable_val = BOOST_ENABLED, \
	.disable_val = 0, \
	.enable_time = PMIC_ENABLE_TIME_US, \
	.supply_name = #base, \
}

#define REG_VBUS_OTG(ids, base) { \
	.name = #ids, \
	.id = STPMIC1_##ids, \
	.n_voltages = 1, \
	.ops = &stpmic1_switch_regul_ops, \
	.type = REGULATOR_VOLTAGE, \
	.owner = THIS_MODULE, \
	.min_uV = 0, \
	.fixed_uV = 5000000, \
	.enable_reg = BST_SW_CR, \
	.enable_mask = USBSW_OTG_SWITCH_ENABLED, \
	.enable_val = USBSW_OTG_SWITCH_ENABLED, \
	.disable_val = 0, \
	.enable_time = PMIC_ENABLE_TIME_US, \
	.supply_name = #base, \
	.active_discharge_reg = BST_SW_CR, \
	.active_discharge_mask = VBUS_OTG_DISCHARGE, \
	.active_discharge_on = VBUS_OTG_DISCHARGE, \
}

#define REG_SW_OUT(ids, base) { \
	.name = #ids, \
	.id = STPMIC1_##ids, \
	.n_voltages = 1, \
	.ops = &stpmic1_switch_regul_ops, \
	.type = REGULATOR_VOLTAGE, \
	.owner = THIS_MODULE, \
	.min_uV = 0, \
	.fixed_uV = 5000000, \
	.enable_reg = BST_SW_CR, \
	.enable_mask = SWIN_SWOUT_ENABLED, \
	.enable_val = SWIN_SWOUT_ENABLED, \
	.disable_val = 0, \
	.enable_time = PMIC_ENABLE_TIME_US, \
	.supply_name = #base, \
	.active_discharge_reg = BST_SW_CR, \
	.active_discharge_mask = SW_OUT_DISCHARGE, \
	.active_discharge_on = SW_OUT_DISCHARGE, \
}

static const struct stpmic1_regulator_cfg stpmic1_regulator_cfgs[] = {
	[STPMIC1_BUCK1] = {
		.desc = REG_BUCK(BUCK1, buck1),
		.icc_reg = BUCKS_ICCTO_CR,
		.icc_mask = BIT(0),
		.mask_reset_reg = BUCKS_MASK_RESET_CR,
		.mask_reset_mask = BIT(0),
	},
	[STPMIC1_BUCK2] = {
		.desc = REG_BUCK(BUCK2, buck2),
		.icc_reg = BUCKS_ICCTO_CR,
		.icc_mask = BIT(1),
		.mask_reset_reg = BUCKS_MASK_RESET_CR,
		.mask_reset_mask = BIT(1),
	},
	[STPMIC1_BUCK3] = {
		.desc = REG_BUCK(BUCK3, buck3),
		.icc_reg = BUCKS_ICCTO_CR,
		.icc_mask = BIT(2),
		.mask_reset_reg = BUCKS_MASK_RESET_CR,
		.mask_reset_mask = BIT(2),
	},
	[STPMIC1_BUCK4] = {
		.desc = REG_BUCK(BUCK4, buck4),
		.icc_reg = BUCKS_ICCTO_CR,
		.icc_mask = BIT(3),
		.mask_reset_reg = BUCKS_MASK_RESET_CR,
		.mask_reset_mask = BIT(3),
	},
	[STPMIC1_LDO1] = {
		.desc = REG_LDO(LDO1, ldo1),
		.icc_reg = LDOS_ICCTO_CR,
		.icc_mask = BIT(0),
		.mask_reset_reg = LDOS_MASK_RESET_CR,
		.mask_reset_mask = BIT(0),
	},
	[STPMIC1_LDO2] = {
		.desc = REG_LDO(LDO2, ldo2),
		.icc_reg = LDOS_ICCTO_CR,
		.icc_mask = BIT(1),
		.mask_reset_reg = LDOS_MASK_RESET_CR,
		.mask_reset_mask = BIT(1),
	},
	[STPMIC1_LDO3] = {
		.desc = REG_LDO3(LDO3, ldo3),
		.icc_reg = LDOS_ICCTO_CR,
		.icc_mask = BIT(2),
		.mask_reset_reg = LDOS_MASK_RESET_CR,
		.mask_reset_mask = BIT(2),
	},
	[STPMIC1_LDO4] = {
		.desc = REG_LDO4(LDO4, ldo4),
		.icc_reg = LDOS_ICCTO_CR,
		.icc_mask = BIT(3),
		.mask_reset_reg = LDOS_MASK_RESET_CR,
		.mask_reset_mask = BIT(3),
	},
	[STPMIC1_LDO5] = {
		.desc = REG_LDO(LDO5, ldo5),
		.icc_reg = LDOS_ICCTO_CR,
		.icc_mask = BIT(4),
		.mask_reset_reg = LDOS_MASK_RESET_CR,
		.mask_reset_mask = BIT(4),
	},
	[STPMIC1_LDO6] = {
		.desc = REG_LDO(LDO6, ldo6),
		.icc_reg = LDOS_ICCTO_CR,
		.icc_mask = BIT(5),
		.mask_reset_reg = LDOS_MASK_RESET_CR,
		.mask_reset_mask = BIT(5),
	},
	[STPMIC1_VREF_DDR] = {
		.desc = REG_VREF_DDR(VREF_DDR, vref_ddr),
		.mask_reset_reg = LDOS_MASK_RESET_CR,
		.mask_reset_mask = BIT(6),
	},
	[STPMIC1_BOOST] = {
		.desc = REG_BOOST(BOOST, boost),
		.icc_reg = BUCKS_ICCTO_CR,
		.icc_mask = BIT(6),
	},
	[STPMIC1_VBUS_OTG] = {
		.desc = REG_VBUS_OTG(VBUS_OTG, pwr_sw1),
		.icc_reg = BUCKS_ICCTO_CR,
		.icc_mask = BIT(4),
	},
	[STPMIC1_SW_OUT] = {
		.desc = REG_SW_OUT(SW_OUT, pwr_sw2),
		.icc_reg = BUCKS_ICCTO_CR,
		.icc_mask = BIT(5),
	},
};

static unsigned int stpmic1_map_mode(unsigned int mode)
{
	switch (mode) {
	case STPMIC1_BUCK_MODE_NORMAL:
		return REGULATOR_MODE_NORMAL;
	case STPMIC1_BUCK_MODE_LP:
		return REGULATOR_MODE_STANDBY;
	default:
		return REGULATOR_MODE_INVALID;
	}
}

static unsigned int stpmic1_get_mode(struct regulator_dev *rdev)
{
	int value;
	struct regmap *regmap = rdev_get_regmap(rdev);

	regmap_read(regmap, rdev->desc->enable_reg, &value);

	if (value & STPMIC1_BUCK_MODE_LP)
		return REGULATOR_MODE_STANDBY;

	return REGULATOR_MODE_NORMAL;
}

static int stpmic1_set_mode(struct regulator_dev *rdev, unsigned int mode)
{
	int value;
	struct regmap *regmap = rdev_get_regmap(rdev);

	switch (mode) {
	case REGULATOR_MODE_NORMAL:
		value = STPMIC1_BUCK_MODE_NORMAL;
		break;
	case REGULATOR_MODE_STANDBY:
		value = STPMIC1_BUCK_MODE_LP;
		break;
	default:
		return -EINVAL;
	}

	return regmap_update_bits(regmap, rdev->desc->enable_reg,
				  STPMIC1_BUCK_MODE_LP, value);
}

static int stpmic1_set_icc(struct regulator_dev *rdev)
{
	struct stpmic1_regulator_cfg *cfg = rdev_get_drvdata(rdev);
	struct regmap *regmap = rdev_get_regmap(rdev);

	/* enable switch off in case of over current */
	return regmap_update_bits(regmap, cfg->icc_reg, cfg->icc_mask,
				  cfg->icc_mask);
}

static irqreturn_t stpmic1_curlim_irq_handler(int irq, void *data)
{
	struct regulator_dev *rdev = (struct regulator_dev *)data;

	/* Send an overcurrent notification */
	regulator_notifier_call_chain(rdev,
				      REGULATOR_EVENT_OVER_CURRENT,
				      NULL);

	return IRQ_HANDLED;
}

#define MATCH(_name, _id) \
	[STPMIC1_##_id] = { \
		.name = #_name, \
		.desc = &stpmic1_regulator_cfgs[STPMIC1_##_id].desc, \
	}

static struct of_regulator_match stpmic1_matches[] = {
	MATCH(buck1, BUCK1),
	MATCH(buck2, BUCK2),
	MATCH(buck3, BUCK3),
	MATCH(buck4, BUCK4),
	MATCH(ldo1, LDO1),
	MATCH(ldo2, LDO2),
	MATCH(ldo3, LDO3),
	MATCH(ldo4, LDO4),
	MATCH(ldo5, LDO5),
	MATCH(ldo6, LDO6),
	MATCH(vref_ddr, VREF_DDR),
	MATCH(boost, BOOST),
	MATCH(pwr_sw1, VBUS_OTG),
	MATCH(pwr_sw2, SW_OUT),
};

static int stpmic1_regulator_register(struct platform_device *pdev, int id,
				      struct of_regulator_match *match,
				      const struct stpmic1_regulator_cfg *cfg)
{
	struct stpmic1 *pmic_dev = dev_get_drvdata(pdev->dev.parent);
	struct regulator_dev *rdev;
	struct regulator_config config = {};
	int ret = 0;
	int irq;

	config.dev = &pdev->dev;
	config.init_data = match->init_data;
	config.of_node = match->of_node;
	config.regmap = pmic_dev->regmap;
	config.driver_data = (void *)cfg;

	rdev = devm_regulator_register(&pdev->dev, &cfg->desc, &config);
	if (IS_ERR(rdev)) {
		dev_err(&pdev->dev, "failed to register %s regulator\n",
			cfg->desc.name);
		return PTR_ERR(rdev);
	}

	/* set mask reset */
	if (of_get_property(config.of_node, "st,mask-reset", NULL) &&
	    cfg->mask_reset_reg != 0) {
		ret = regmap_update_bits(pmic_dev->regmap,
					 cfg->mask_reset_reg,
					 cfg->mask_reset_mask,
					 cfg->mask_reset_mask);
		if (ret) {
			dev_err(&pdev->dev, "set mask reset failed\n");
			return ret;
		}
	}

	/* setup an irq handler for over-current detection */
	irq = of_irq_get(config.of_node, 0);
	if (irq > 0) {
		ret = devm_request_threaded_irq(&pdev->dev,
						irq, NULL,
						stpmic1_curlim_irq_handler,
						IRQF_ONESHOT | IRQF_SHARED,
						pdev->name, rdev);
		if (ret) {
			dev_err(&pdev->dev, "Request IRQ failed\n");
			return ret;
		}
	}
	return 0;
}

static int stpmic1_regulator_probe(struct platform_device *pdev)
{
	int i, ret;

	ret = of_regulator_match(&pdev->dev, pdev->dev.of_node, stpmic1_matches,
				 ARRAY_SIZE(stpmic1_matches));
	if (ret < 0) {
		dev_err(&pdev->dev,
			"Error in PMIC regulator device tree node");
		return ret;
	}

	for (i = 0; i < ARRAY_SIZE(stpmic1_regulator_cfgs); i++) {
		ret = stpmic1_regulator_register(pdev, i, &stpmic1_matches[i],
						 &stpmic1_regulator_cfgs[i]);
		if (ret < 0)
			return ret;
	}

	dev_dbg(&pdev->dev, "stpmic1_regulator driver probed\n");

	return 0;
}

static const struct of_device_id of_pmic_regulator_match[] = {
	{ .compatible = "st,stpmic1-regulators" },
	{ },
};

MODULE_DEVICE_TABLE(of, of_pmic_regulator_match);

static struct platform_driver stpmic1_regulator_driver = {
	.driver = {
		.name = "stpmic1-regulator",
		.of_match_table = of_match_ptr(of_pmic_regulator_match),
	},
	.probe = stpmic1_regulator_probe,
};

module_platform_driver(stpmic1_regulator_driver);

MODULE_DESCRIPTION("STPMIC1 PMIC voltage regulator driver");
MODULE_AUTHOR("Pascal Paillet <p.paillet@st.com>");
MODULE_LICENSE("GPL v2");
