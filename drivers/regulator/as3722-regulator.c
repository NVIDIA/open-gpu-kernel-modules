// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Voltage regulator support for AMS AS3722 PMIC
 *
 * Copyright (C) 2013 ams
 *
 * Author: Florian Lobmaier <florian.lobmaier@ams.com>
 * Author: Laxman Dewangan <ldewangan@nvidia.com>
 */

#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mfd/as3722.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/of_regulator.h>
#include <linux/slab.h>

/* Regulator IDs */
enum as3722_regulators_id {
	AS3722_REGULATOR_ID_SD0,
	AS3722_REGULATOR_ID_SD1,
	AS3722_REGULATOR_ID_SD2,
	AS3722_REGULATOR_ID_SD3,
	AS3722_REGULATOR_ID_SD4,
	AS3722_REGULATOR_ID_SD5,
	AS3722_REGULATOR_ID_SD6,
	AS3722_REGULATOR_ID_LDO0,
	AS3722_REGULATOR_ID_LDO1,
	AS3722_REGULATOR_ID_LDO2,
	AS3722_REGULATOR_ID_LDO3,
	AS3722_REGULATOR_ID_LDO4,
	AS3722_REGULATOR_ID_LDO5,
	AS3722_REGULATOR_ID_LDO6,
	AS3722_REGULATOR_ID_LDO7,
	AS3722_REGULATOR_ID_LDO9,
	AS3722_REGULATOR_ID_LDO10,
	AS3722_REGULATOR_ID_LDO11,
	AS3722_REGULATOR_ID_MAX,
};

struct as3722_register_mapping {
	u8 regulator_id;
	const char *name;
	const char *sname;
	u8 vsel_reg;
	u8 vsel_mask;
	int n_voltages;
	u32 enable_reg;
	u8 enable_mask;
	u32 control_reg;
	u8 mode_mask;
	u32 sleep_ctrl_reg;
	u8 sleep_ctrl_mask;
};

struct as3722_regulator_config_data {
	struct regulator_init_data *reg_init;
	bool enable_tracking;
	int ext_control;
};

struct as3722_regulators {
	struct device *dev;
	struct as3722 *as3722;
	struct regulator_desc desc[AS3722_REGULATOR_ID_MAX];
	struct as3722_regulator_config_data
			reg_config_data[AS3722_REGULATOR_ID_MAX];
};

static const struct as3722_register_mapping as3722_reg_lookup[] = {
	{
		.regulator_id = AS3722_REGULATOR_ID_SD0,
		.name = "as3722-sd0",
		.vsel_reg = AS3722_SD0_VOLTAGE_REG,
		.vsel_mask = AS3722_SD_VSEL_MASK,
		.enable_reg = AS3722_SD_CONTROL_REG,
		.enable_mask = AS3722_SDn_CTRL(0),
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL1_REG,
		.sleep_ctrl_mask = AS3722_SD0_EXT_ENABLE_MASK,
		.control_reg = AS3722_SD0_CONTROL_REG,
		.mode_mask = AS3722_SD0_MODE_FAST,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_SD1,
		.name = "as3722-sd1",
		.vsel_reg = AS3722_SD1_VOLTAGE_REG,
		.vsel_mask = AS3722_SD_VSEL_MASK,
		.enable_reg = AS3722_SD_CONTROL_REG,
		.enable_mask = AS3722_SDn_CTRL(1),
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL1_REG,
		.sleep_ctrl_mask = AS3722_SD1_EXT_ENABLE_MASK,
		.control_reg = AS3722_SD1_CONTROL_REG,
		.mode_mask = AS3722_SD1_MODE_FAST,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_SD2,
		.name = "as3722-sd2",
		.sname = "vsup-sd2",
		.vsel_reg = AS3722_SD2_VOLTAGE_REG,
		.vsel_mask = AS3722_SD_VSEL_MASK,
		.enable_reg = AS3722_SD_CONTROL_REG,
		.enable_mask = AS3722_SDn_CTRL(2),
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL1_REG,
		.sleep_ctrl_mask = AS3722_SD2_EXT_ENABLE_MASK,
		.control_reg = AS3722_SD23_CONTROL_REG,
		.mode_mask = AS3722_SD2_MODE_FAST,
		.n_voltages = AS3722_SD2_VSEL_MAX + 1,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_SD3,
		.name = "as3722-sd3",
		.sname = "vsup-sd3",
		.vsel_reg = AS3722_SD3_VOLTAGE_REG,
		.vsel_mask = AS3722_SD_VSEL_MASK,
		.enable_reg = AS3722_SD_CONTROL_REG,
		.enable_mask = AS3722_SDn_CTRL(3),
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL1_REG,
		.sleep_ctrl_mask = AS3722_SD3_EXT_ENABLE_MASK,
		.control_reg = AS3722_SD23_CONTROL_REG,
		.mode_mask = AS3722_SD3_MODE_FAST,
		.n_voltages = AS3722_SD2_VSEL_MAX + 1,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_SD4,
		.name = "as3722-sd4",
		.sname = "vsup-sd4",
		.vsel_reg = AS3722_SD4_VOLTAGE_REG,
		.vsel_mask = AS3722_SD_VSEL_MASK,
		.enable_reg = AS3722_SD_CONTROL_REG,
		.enable_mask = AS3722_SDn_CTRL(4),
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL2_REG,
		.sleep_ctrl_mask = AS3722_SD4_EXT_ENABLE_MASK,
		.control_reg = AS3722_SD4_CONTROL_REG,
		.mode_mask = AS3722_SD4_MODE_FAST,
		.n_voltages = AS3722_SD2_VSEL_MAX + 1,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_SD5,
		.name = "as3722-sd5",
		.sname = "vsup-sd5",
		.vsel_reg = AS3722_SD5_VOLTAGE_REG,
		.vsel_mask = AS3722_SD_VSEL_MASK,
		.enable_reg = AS3722_SD_CONTROL_REG,
		.enable_mask = AS3722_SDn_CTRL(5),
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL2_REG,
		.sleep_ctrl_mask = AS3722_SD5_EXT_ENABLE_MASK,
		.control_reg = AS3722_SD5_CONTROL_REG,
		.mode_mask = AS3722_SD5_MODE_FAST,
		.n_voltages = AS3722_SD2_VSEL_MAX + 1,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_SD6,
		.name = "as3722-sd6",
		.vsel_reg = AS3722_SD6_VOLTAGE_REG,
		.vsel_mask = AS3722_SD_VSEL_MASK,
		.enable_reg = AS3722_SD_CONTROL_REG,
		.enable_mask = AS3722_SDn_CTRL(6),
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL2_REG,
		.sleep_ctrl_mask = AS3722_SD6_EXT_ENABLE_MASK,
		.control_reg = AS3722_SD6_CONTROL_REG,
		.mode_mask = AS3722_SD6_MODE_FAST,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_LDO0,
		.name = "as3722-ldo0",
		.sname = "vin-ldo0",
		.vsel_reg = AS3722_LDO0_VOLTAGE_REG,
		.vsel_mask = AS3722_LDO0_VSEL_MASK,
		.enable_reg = AS3722_LDOCONTROL0_REG,
		.enable_mask = AS3722_LDO0_CTRL,
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL3_REG,
		.sleep_ctrl_mask = AS3722_LDO0_EXT_ENABLE_MASK,
		.n_voltages = AS3722_LDO0_NUM_VOLT,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_LDO1,
		.name = "as3722-ldo1",
		.sname = "vin-ldo1-6",
		.vsel_reg = AS3722_LDO1_VOLTAGE_REG,
		.vsel_mask = AS3722_LDO_VSEL_MASK,
		.enable_reg = AS3722_LDOCONTROL0_REG,
		.enable_mask = AS3722_LDO1_CTRL,
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL3_REG,
		.sleep_ctrl_mask = AS3722_LDO1_EXT_ENABLE_MASK,
		.n_voltages = AS3722_LDO_NUM_VOLT,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_LDO2,
		.name = "as3722-ldo2",
		.sname = "vin-ldo2-5-7",
		.vsel_reg = AS3722_LDO2_VOLTAGE_REG,
		.vsel_mask = AS3722_LDO_VSEL_MASK,
		.enable_reg = AS3722_LDOCONTROL0_REG,
		.enable_mask = AS3722_LDO2_CTRL,
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL3_REG,
		.sleep_ctrl_mask = AS3722_LDO2_EXT_ENABLE_MASK,
		.n_voltages = AS3722_LDO_NUM_VOLT,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_LDO3,
		.name = "as3722-ldo3",
		.sname = "vin-ldo3-4",
		.vsel_reg = AS3722_LDO3_VOLTAGE_REG,
		.vsel_mask = AS3722_LDO3_VSEL_MASK,
		.enable_reg = AS3722_LDOCONTROL0_REG,
		.enable_mask = AS3722_LDO3_CTRL,
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL3_REG,
		.sleep_ctrl_mask = AS3722_LDO3_EXT_ENABLE_MASK,
		.n_voltages = AS3722_LDO3_NUM_VOLT,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_LDO4,
		.name = "as3722-ldo4",
		.sname = "vin-ldo3-4",
		.vsel_reg = AS3722_LDO4_VOLTAGE_REG,
		.vsel_mask = AS3722_LDO_VSEL_MASK,
		.enable_reg = AS3722_LDOCONTROL0_REG,
		.enable_mask = AS3722_LDO4_CTRL,
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL4_REG,
		.sleep_ctrl_mask = AS3722_LDO4_EXT_ENABLE_MASK,
		.n_voltages = AS3722_LDO_NUM_VOLT,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_LDO5,
		.name = "as3722-ldo5",
		.sname = "vin-ldo2-5-7",
		.vsel_reg = AS3722_LDO5_VOLTAGE_REG,
		.vsel_mask = AS3722_LDO_VSEL_MASK,
		.enable_reg = AS3722_LDOCONTROL0_REG,
		.enable_mask = AS3722_LDO5_CTRL,
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL4_REG,
		.sleep_ctrl_mask = AS3722_LDO5_EXT_ENABLE_MASK,
		.n_voltages = AS3722_LDO_NUM_VOLT,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_LDO6,
		.name = "as3722-ldo6",
		.sname = "vin-ldo1-6",
		.vsel_reg = AS3722_LDO6_VOLTAGE_REG,
		.vsel_mask = AS3722_LDO_VSEL_MASK,
		.enable_reg = AS3722_LDOCONTROL0_REG,
		.enable_mask = AS3722_LDO6_CTRL,
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL4_REG,
		.sleep_ctrl_mask = AS3722_LDO6_EXT_ENABLE_MASK,
		.n_voltages = AS3722_LDO_NUM_VOLT,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_LDO7,
		.name = "as3722-ldo7",
		.sname = "vin-ldo2-5-7",
		.vsel_reg = AS3722_LDO7_VOLTAGE_REG,
		.vsel_mask = AS3722_LDO_VSEL_MASK,
		.enable_reg = AS3722_LDOCONTROL0_REG,
		.enable_mask = AS3722_LDO7_CTRL,
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL4_REG,
		.sleep_ctrl_mask = AS3722_LDO7_EXT_ENABLE_MASK,
		.n_voltages = AS3722_LDO_NUM_VOLT,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_LDO9,
		.name = "as3722-ldo9",
		.sname = "vin-ldo9-10",
		.vsel_reg = AS3722_LDO9_VOLTAGE_REG,
		.vsel_mask = AS3722_LDO_VSEL_MASK,
		.enable_reg = AS3722_LDOCONTROL1_REG,
		.enable_mask = AS3722_LDO9_CTRL,
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL5_REG,
		.sleep_ctrl_mask = AS3722_LDO9_EXT_ENABLE_MASK,
		.n_voltages = AS3722_LDO_NUM_VOLT,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_LDO10,
		.name = "as3722-ldo10",
		.sname = "vin-ldo9-10",
		.vsel_reg = AS3722_LDO10_VOLTAGE_REG,
		.vsel_mask = AS3722_LDO_VSEL_MASK,
		.enable_reg = AS3722_LDOCONTROL1_REG,
		.enable_mask = AS3722_LDO10_CTRL,
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL5_REG,
		.sleep_ctrl_mask = AS3722_LDO10_EXT_ENABLE_MASK,
		.n_voltages = AS3722_LDO_NUM_VOLT,
	},
	{
		.regulator_id = AS3722_REGULATOR_ID_LDO11,
		.name = "as3722-ldo11",
		.sname = "vin-ldo11",
		.vsel_reg = AS3722_LDO11_VOLTAGE_REG,
		.vsel_mask = AS3722_LDO_VSEL_MASK,
		.enable_reg = AS3722_LDOCONTROL1_REG,
		.enable_mask = AS3722_LDO11_CTRL,
		.sleep_ctrl_reg = AS3722_ENABLE_CTRL5_REG,
		.sleep_ctrl_mask = AS3722_LDO11_EXT_ENABLE_MASK,
		.n_voltages = AS3722_LDO_NUM_VOLT,
	},
};

static const unsigned int as3722_ldo_current[] = { 150000, 300000 };
static const unsigned int as3722_sd016_current[] = {
	2500000, 3000000, 3500000
};

static const struct regulator_ops as3722_ldo0_ops = {
	.is_enabled = regulator_is_enabled_regmap,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.list_voltage = regulator_list_voltage_linear,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_current_limit = regulator_get_current_limit_regmap,
	.set_current_limit = regulator_set_current_limit_regmap,
};

static const struct regulator_ops as3722_ldo0_extcntrl_ops = {
	.list_voltage = regulator_list_voltage_linear,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_current_limit = regulator_get_current_limit_regmap,
	.set_current_limit = regulator_set_current_limit_regmap,
};

static int as3722_ldo3_set_tracking_mode(struct as3722_regulators *as3722_reg,
		int id, u8 mode)
{
	struct as3722 *as3722 = as3722_reg->as3722;

	switch (mode) {
	case AS3722_LDO3_MODE_PMOS:
	case AS3722_LDO3_MODE_PMOS_TRACKING:
	case AS3722_LDO3_MODE_NMOS:
	case AS3722_LDO3_MODE_SWITCH:
		return as3722_update_bits(as3722,
			as3722_reg_lookup[id].vsel_reg,
			AS3722_LDO3_MODE_MASK, mode);

	default:
		return -EINVAL;
	}
}

static int as3722_ldo3_get_current_limit(struct regulator_dev *rdev)
{
	return 150000;
}

static const struct regulator_ops as3722_ldo3_ops = {
	.is_enabled = regulator_is_enabled_regmap,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.list_voltage = regulator_list_voltage_linear,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_current_limit = as3722_ldo3_get_current_limit,
};

static const struct regulator_ops as3722_ldo3_extcntrl_ops = {
	.list_voltage = regulator_list_voltage_linear,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_current_limit = as3722_ldo3_get_current_limit,
};

static const struct regulator_ops as3722_ldo6_ops = {
	.is_enabled = regulator_is_enabled_regmap,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.map_voltage = regulator_map_voltage_linear_range,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.list_voltage = regulator_list_voltage_linear_range,
	.get_current_limit = regulator_get_current_limit_regmap,
	.set_current_limit = regulator_set_current_limit_regmap,
	.get_bypass = regulator_get_bypass_regmap,
	.set_bypass = regulator_set_bypass_regmap,
};

static const struct regulator_ops as3722_ldo6_extcntrl_ops = {
	.map_voltage = regulator_map_voltage_linear_range,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.list_voltage = regulator_list_voltage_linear_range,
	.get_current_limit = regulator_get_current_limit_regmap,
	.set_current_limit = regulator_set_current_limit_regmap,
	.get_bypass = regulator_get_bypass_regmap,
	.set_bypass = regulator_set_bypass_regmap,
};

static const struct linear_range as3722_ldo_ranges[] = {
	REGULATOR_LINEAR_RANGE(0, 0x00, 0x00, 0),
	REGULATOR_LINEAR_RANGE(825000, 0x01, 0x24, 25000),
	REGULATOR_LINEAR_RANGE(1725000, 0x40, 0x7F, 25000),
};

static const struct regulator_ops as3722_ldo_ops = {
	.is_enabled = regulator_is_enabled_regmap,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.map_voltage = regulator_map_voltage_linear_range,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.list_voltage = regulator_list_voltage_linear_range,
	.get_current_limit = regulator_get_current_limit_regmap,
	.set_current_limit = regulator_set_current_limit_regmap,
};

static const struct regulator_ops as3722_ldo_extcntrl_ops = {
	.map_voltage = regulator_map_voltage_linear_range,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.list_voltage = regulator_list_voltage_linear_range,
	.get_current_limit = regulator_get_current_limit_regmap,
	.set_current_limit = regulator_set_current_limit_regmap,
};

static unsigned int as3722_sd_get_mode(struct regulator_dev *rdev)
{
	struct as3722_regulators *as3722_regs = rdev_get_drvdata(rdev);
	struct as3722 *as3722 = as3722_regs->as3722;
	int id = rdev_get_id(rdev);
	u32 val;
	int ret;

	if (!as3722_reg_lookup[id].control_reg)
		return -ENOTSUPP;

	ret = as3722_read(as3722, as3722_reg_lookup[id].control_reg, &val);
	if (ret < 0) {
		dev_err(as3722_regs->dev, "Reg 0x%02x read failed: %d\n",
			as3722_reg_lookup[id].control_reg, ret);
		return ret;
	}

	if (val & as3722_reg_lookup[id].mode_mask)
		return REGULATOR_MODE_FAST;
	else
		return REGULATOR_MODE_NORMAL;
}

static int as3722_sd_set_mode(struct regulator_dev *rdev,
		unsigned int mode)
{
	struct as3722_regulators *as3722_regs = rdev_get_drvdata(rdev);
	struct as3722 *as3722 = as3722_regs->as3722;
	u8 id = rdev_get_id(rdev);
	u8 val = 0;
	int ret;

	if (!as3722_reg_lookup[id].control_reg)
		return -ERANGE;

	switch (mode) {
	case REGULATOR_MODE_FAST:
		val = as3722_reg_lookup[id].mode_mask;
		fallthrough;
	case REGULATOR_MODE_NORMAL:
		break;
	default:
		return -EINVAL;
	}

	ret = as3722_update_bits(as3722, as3722_reg_lookup[id].control_reg,
			as3722_reg_lookup[id].mode_mask, val);
	if (ret < 0) {
		dev_err(as3722_regs->dev, "Reg 0x%02x update failed: %d\n",
			as3722_reg_lookup[id].control_reg, ret);
		return ret;
	}
	return ret;
}

static bool as3722_sd0_is_low_voltage(struct as3722_regulators *as3722_regs)
{
	int err;
	unsigned val;

	err = as3722_read(as3722_regs->as3722, AS3722_FUSE7_REG, &val);
	if (err < 0) {
		dev_err(as3722_regs->dev, "Reg 0x%02x read failed: %d\n",
			AS3722_FUSE7_REG, err);
		return false;
	}
	if (val & AS3722_FUSE7_SD0_LOW_VOLTAGE)
		return true;
	return false;
}

static const struct linear_range as3722_sd2345_ranges[] = {
	REGULATOR_LINEAR_RANGE(0, 0x00, 0x00, 0),
	REGULATOR_LINEAR_RANGE(612500, 0x01, 0x40, 12500),
	REGULATOR_LINEAR_RANGE(1425000, 0x41, 0x70, 25000),
	REGULATOR_LINEAR_RANGE(2650000, 0x71, 0x7F, 50000),
};

static const struct regulator_ops as3722_sd016_ops = {
	.is_enabled = regulator_is_enabled_regmap,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.list_voltage = regulator_list_voltage_linear,
	.map_voltage = regulator_map_voltage_linear,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_current_limit = regulator_get_current_limit_regmap,
	.set_current_limit = regulator_set_current_limit_regmap,
	.get_mode = as3722_sd_get_mode,
	.set_mode = as3722_sd_set_mode,
};

static const struct regulator_ops as3722_sd016_extcntrl_ops = {
	.list_voltage = regulator_list_voltage_linear,
	.map_voltage = regulator_map_voltage_linear,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_current_limit = regulator_get_current_limit_regmap,
	.set_current_limit = regulator_set_current_limit_regmap,
	.get_mode = as3722_sd_get_mode,
	.set_mode = as3722_sd_set_mode,
};

static const struct regulator_ops as3722_sd2345_ops = {
	.is_enabled = regulator_is_enabled_regmap,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.list_voltage = regulator_list_voltage_linear_range,
	.map_voltage = regulator_map_voltage_linear_range,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.get_mode = as3722_sd_get_mode,
	.set_mode = as3722_sd_set_mode,
};

static const struct regulator_ops as3722_sd2345_extcntrl_ops = {
	.list_voltage = regulator_list_voltage_linear_range,
	.map_voltage = regulator_map_voltage_linear_range,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.get_mode = as3722_sd_get_mode,
	.set_mode = as3722_sd_set_mode,
};

static int as3722_extreg_init(struct as3722_regulators *as3722_regs, int id,
		int ext_pwr_ctrl)
{
	int ret;
	unsigned int val;

	if ((ext_pwr_ctrl < AS3722_EXT_CONTROL_ENABLE1) ||
		(ext_pwr_ctrl > AS3722_EXT_CONTROL_ENABLE3))
		return -EINVAL;

	val =  ext_pwr_ctrl << (ffs(as3722_reg_lookup[id].sleep_ctrl_mask) - 1);
	ret = as3722_update_bits(as3722_regs->as3722,
			as3722_reg_lookup[id].sleep_ctrl_reg,
			as3722_reg_lookup[id].sleep_ctrl_mask, val);
	if (ret < 0)
		dev_err(as3722_regs->dev, "Reg 0x%02x update failed: %d\n",
			as3722_reg_lookup[id].sleep_ctrl_reg, ret);
	return ret;
}

static struct of_regulator_match as3722_regulator_matches[] = {
	{ .name = "sd0", },
	{ .name = "sd1", },
	{ .name = "sd2", },
	{ .name = "sd3", },
	{ .name = "sd4", },
	{ .name = "sd5", },
	{ .name = "sd6", },
	{ .name = "ldo0", },
	{ .name = "ldo1", },
	{ .name = "ldo2", },
	{ .name = "ldo3", },
	{ .name = "ldo4", },
	{ .name = "ldo5", },
	{ .name = "ldo6", },
	{ .name = "ldo7", },
	{ .name = "ldo9", },
	{ .name = "ldo10", },
	{ .name = "ldo11", },
};

static int as3722_get_regulator_dt_data(struct platform_device *pdev,
		struct as3722_regulators *as3722_regs)
{
	struct device_node *np;
	struct as3722_regulator_config_data *reg_config;
	u32 prop;
	int id;
	int ret;

	np = of_get_child_by_name(pdev->dev.parent->of_node, "regulators");
	if (!np) {
		dev_err(&pdev->dev, "Device is not having regulators node\n");
		return -ENODEV;
	}
	pdev->dev.of_node = np;

	ret = of_regulator_match(&pdev->dev, np, as3722_regulator_matches,
			ARRAY_SIZE(as3722_regulator_matches));
	of_node_put(np);
	if (ret < 0) {
		dev_err(&pdev->dev, "Parsing of regulator node failed: %d\n",
			ret);
		return ret;
	}

	for (id = 0; id < ARRAY_SIZE(as3722_regulator_matches); ++id) {
		struct device_node *reg_node;

		reg_config = &as3722_regs->reg_config_data[id];
		reg_config->reg_init = as3722_regulator_matches[id].init_data;
		reg_node = as3722_regulator_matches[id].of_node;

		if (!reg_config->reg_init || !reg_node)
			continue;

		ret = of_property_read_u32(reg_node, "ams,ext-control", &prop);
		if (!ret) {
			if (prop < 3)
				reg_config->ext_control = prop;
			else
				dev_warn(&pdev->dev,
					"ext-control have invalid option: %u\n",
					prop);
		}
		reg_config->enable_tracking =
			of_property_read_bool(reg_node, "ams,enable-tracking");
	}
	return 0;
}

static int as3722_regulator_probe(struct platform_device *pdev)
{
	struct as3722 *as3722 = dev_get_drvdata(pdev->dev.parent);
	struct as3722_regulators *as3722_regs;
	struct as3722_regulator_config_data *reg_config;
	struct regulator_dev *rdev;
	struct regulator_config config = { };
	const struct regulator_ops *ops;
	int id;
	int ret;

	as3722_regs = devm_kzalloc(&pdev->dev, sizeof(*as3722_regs),
				GFP_KERNEL);
	if (!as3722_regs)
		return -ENOMEM;

	as3722_regs->dev = &pdev->dev;
	as3722_regs->as3722 = as3722;
	platform_set_drvdata(pdev, as3722_regs);

	ret = as3722_get_regulator_dt_data(pdev, as3722_regs);
	if (ret < 0)
		return ret;

	config.dev = &pdev->dev;
	config.driver_data = as3722_regs;
	config.regmap = as3722->regmap;

	for (id = 0; id < AS3722_REGULATOR_ID_MAX; id++) {
		struct regulator_desc *desc;

		desc = &as3722_regs->desc[id];
		reg_config = &as3722_regs->reg_config_data[id];

		desc->name = as3722_reg_lookup[id].name;
		desc->supply_name = as3722_reg_lookup[id].sname;
		desc->id = as3722_reg_lookup[id].regulator_id;
		desc->n_voltages = as3722_reg_lookup[id].n_voltages;
		desc->type = REGULATOR_VOLTAGE;
		desc->owner = THIS_MODULE;
		desc->enable_reg = as3722_reg_lookup[id].enable_reg;
		desc->enable_mask = as3722_reg_lookup[id].enable_mask;
		desc->vsel_reg = as3722_reg_lookup[id].vsel_reg;
		desc->vsel_mask = as3722_reg_lookup[id].vsel_mask;
		switch (id) {
		case AS3722_REGULATOR_ID_LDO0:
			if (reg_config->ext_control)
				ops = &as3722_ldo0_extcntrl_ops;
			else
				ops = &as3722_ldo0_ops;
			desc->min_uV = 825000;
			desc->uV_step = 25000;
			desc->linear_min_sel = 1;
			desc->enable_time = 500;
			desc->curr_table = as3722_ldo_current;
			desc->n_current_limits = ARRAY_SIZE(as3722_ldo_current);
			desc->csel_reg = as3722_reg_lookup[id].vsel_reg;
			desc->csel_mask = AS3722_LDO_ILIMIT_MASK;
			break;
		case AS3722_REGULATOR_ID_LDO3:
			if (reg_config->ext_control)
				ops = &as3722_ldo3_extcntrl_ops;
			else
				ops = &as3722_ldo3_ops;
			desc->min_uV = 620000;
			desc->uV_step = 20000;
			desc->linear_min_sel = 1;
			desc->enable_time = 500;
			if (reg_config->enable_tracking) {
				ret = as3722_ldo3_set_tracking_mode(as3722_regs,
					id, AS3722_LDO3_MODE_PMOS_TRACKING);
				if (ret < 0) {
					dev_err(&pdev->dev,
						"LDO3 tracking failed: %d\n",
						ret);
					return ret;
				}
			}
			break;
		case AS3722_REGULATOR_ID_LDO6:
			if (reg_config->ext_control)
				ops = &as3722_ldo6_extcntrl_ops;
			else
				ops = &as3722_ldo6_ops;
			desc->enable_time = 500;
			desc->bypass_reg = AS3722_LDO6_VOLTAGE_REG;
			desc->bypass_mask = AS3722_LDO_VSEL_MASK;
			desc->bypass_val_on = AS3722_LDO6_VSEL_BYPASS;
			desc->bypass_val_off = AS3722_LDO6_VSEL_BYPASS;
			desc->linear_ranges = as3722_ldo_ranges;
			desc->n_linear_ranges = ARRAY_SIZE(as3722_ldo_ranges);
			desc->curr_table = as3722_ldo_current;
			desc->n_current_limits = ARRAY_SIZE(as3722_ldo_current);
			desc->csel_reg = as3722_reg_lookup[id].vsel_reg;
			desc->csel_mask = AS3722_LDO_ILIMIT_MASK;
			break;
		case AS3722_REGULATOR_ID_SD0:
		case AS3722_REGULATOR_ID_SD1:
		case AS3722_REGULATOR_ID_SD6:
			if (reg_config->ext_control)
				ops = &as3722_sd016_extcntrl_ops;
			else
				ops = &as3722_sd016_ops;
			if (id == AS3722_REGULATOR_ID_SD0 &&
			    as3722_sd0_is_low_voltage(as3722_regs)) {
				as3722_regs->desc[id].n_voltages =
					AS3722_SD0_VSEL_LOW_VOL_MAX + 1;
				as3722_regs->desc[id].min_uV = 410000;
			} else {
				as3722_regs->desc[id].n_voltages =
					AS3722_SD0_VSEL_MAX + 1;
				as3722_regs->desc[id].min_uV = 610000;
			}
			desc->uV_step = 10000;
			desc->linear_min_sel = 1;
			desc->enable_time = 600;
			desc->curr_table = as3722_sd016_current;
			desc->n_current_limits =
				ARRAY_SIZE(as3722_sd016_current);
			if (id == AS3722_REGULATOR_ID_SD0) {
				desc->csel_reg = AS3722_OVCURRENT_REG;
				desc->csel_mask =
					AS3722_OVCURRENT_SD0_TRIP_MASK;
			} else if (id == AS3722_REGULATOR_ID_SD1) {
				desc->csel_reg = AS3722_OVCURRENT_REG;
				desc->csel_mask =
					AS3722_OVCURRENT_SD1_TRIP_MASK;
			} else if (id == AS3722_REGULATOR_ID_SD6) {
				desc->csel_reg = AS3722_OVCURRENT_DEB_REG;
				desc->csel_mask =
					AS3722_OVCURRENT_SD6_TRIP_MASK;
			}
			break;
		case AS3722_REGULATOR_ID_SD2:
		case AS3722_REGULATOR_ID_SD3:
		case AS3722_REGULATOR_ID_SD4:
		case AS3722_REGULATOR_ID_SD5:
			if (reg_config->ext_control)
				ops = &as3722_sd2345_extcntrl_ops;
			else
				ops = &as3722_sd2345_ops;
			desc->linear_ranges = as3722_sd2345_ranges;
			desc->n_linear_ranges =
					ARRAY_SIZE(as3722_sd2345_ranges);
			break;
		default:
			if (reg_config->ext_control)
				ops = &as3722_ldo_extcntrl_ops;
			else
				ops = &as3722_ldo_ops;
			desc->enable_time = 500;
			desc->linear_ranges = as3722_ldo_ranges;
			desc->n_linear_ranges = ARRAY_SIZE(as3722_ldo_ranges);
			desc->curr_table = as3722_ldo_current;
			desc->n_current_limits = ARRAY_SIZE(as3722_ldo_current);
			desc->csel_reg = as3722_reg_lookup[id].vsel_reg;
			desc->csel_mask = AS3722_LDO_ILIMIT_MASK;
			break;
		}
		desc->ops = ops;
		config.init_data = reg_config->reg_init;
		config.of_node = as3722_regulator_matches[id].of_node;
		rdev = devm_regulator_register(&pdev->dev, desc, &config);
		if (IS_ERR(rdev)) {
			ret = PTR_ERR(rdev);
			dev_err(&pdev->dev, "regulator %d register failed %d\n",
				id, ret);
			return ret;
		}

		if (reg_config->ext_control) {
			ret = regulator_enable_regmap(rdev);
			if (ret < 0) {
				dev_err(&pdev->dev,
					"Regulator %d enable failed: %d\n",
					id, ret);
				return ret;
			}
			ret = as3722_extreg_init(as3722_regs, id,
					reg_config->ext_control);
			if (ret < 0) {
				dev_err(&pdev->dev,
					"AS3722 ext control failed: %d", ret);
				return ret;
			}
		}
	}
	return 0;
}

static const struct of_device_id of_as3722_regulator_match[] = {
	{ .compatible = "ams,as3722-regulator", },
	{},
};
MODULE_DEVICE_TABLE(of, of_as3722_regulator_match);

static struct platform_driver as3722_regulator_driver = {
	.driver = {
		.name = "as3722-regulator",
		.of_match_table = of_as3722_regulator_match,
	},
	.probe = as3722_regulator_probe,
};

module_platform_driver(as3722_regulator_driver);

MODULE_ALIAS("platform:as3722-regulator");
MODULE_DESCRIPTION("AS3722 regulator driver");
MODULE_AUTHOR("Florian Lobmaier <florian.lobmaier@ams.com>");
MODULE_AUTHOR("Laxman Dewangan <ldewangan@nvidia.com>");
MODULE_LICENSE("GPL");
