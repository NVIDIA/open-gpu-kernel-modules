/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * mc13xxx.h - regulators for the Freescale mc13xxx PMIC
 *
 *  Copyright (C) 2010 Yong Shen <yong.shen@linaro.org>
 */

#ifndef __LINUX_REGULATOR_MC13XXX_H
#define __LINUX_REGULATOR_MC13XXX_H

#include <linux/regulator/driver.h>

struct mc13xxx_regulator {
	struct regulator_desc desc;
	int reg;
	int enable_bit;
	int vsel_reg;
	int vsel_shift;
	int vsel_mask;
};

struct mc13xxx_regulator_priv {
	struct mc13xxx *mc13xxx;
	u32 powermisc_pwgt_state;
	struct mc13xxx_regulator *mc13xxx_regulators;
	int num_regulators;
	struct regulator_dev *regulators[];
};

extern int mc13xxx_fixed_regulator_set_voltage(struct regulator_dev *rdev,
		int min_uV, int max_uV, unsigned *selector);

#ifdef CONFIG_OF
extern int mc13xxx_get_num_regulators_dt(struct platform_device *pdev);
extern struct mc13xxx_regulator_init_data *mc13xxx_parse_regulators_dt(
	struct platform_device *pdev, struct mc13xxx_regulator *regulators,
	int num_regulators);
#else
static inline int mc13xxx_get_num_regulators_dt(struct platform_device *pdev)
{
	return -ENODEV;
}

static inline struct mc13xxx_regulator_init_data *mc13xxx_parse_regulators_dt(
	struct platform_device *pdev, struct mc13xxx_regulator *regulators,
	int num_regulators)
{
	return NULL;
}
#endif

extern const struct regulator_ops mc13xxx_regulator_ops;
extern const struct regulator_ops mc13xxx_fixed_regulator_ops;

#define MC13xxx_DEFINE(prefix, _name, _node, _reg, _vsel_reg, _voltages, _ops)	\
	[prefix ## _name] = {				\
		.desc = {						\
			.name = #_node,					\
			.n_voltages = ARRAY_SIZE(_voltages),		\
			.volt_table =  _voltages,			\
			.ops = &_ops,			\
			.type = REGULATOR_VOLTAGE,			\
			.id = prefix ## _name,		\
			.owner = THIS_MODULE,				\
		},							\
		.reg = prefix ## _reg,				\
		.enable_bit = prefix ## _reg ## _ ## _name ## EN,	\
		.vsel_reg = prefix ## _vsel_reg,			\
		.vsel_shift = prefix ## _vsel_reg ## _ ## _name ## VSEL,\
		.vsel_mask = prefix ## _vsel_reg ## _ ## _name ## VSEL_M,\
	}

#define MC13xxx_FIXED_DEFINE(prefix, _name, _node, _reg, _voltages, _ops)	\
	[prefix ## _name] = {				\
		.desc = {						\
			.name = #_node,					\
			.n_voltages = ARRAY_SIZE(_voltages),		\
			.volt_table =  _voltages,			\
			.ops = &_ops,		\
			.type = REGULATOR_VOLTAGE,			\
			.id = prefix ## _name,		\
			.owner = THIS_MODULE,				\
		},							\
		.reg = prefix ## _reg,				\
		.enable_bit = prefix ## _reg ## _ ## _name ## EN,	\
	}

#define MC13xxx_GPO_DEFINE(prefix, _name, _node, _reg,  _voltages, _ops)	\
	[prefix ## _name] = {				\
		.desc = {						\
			.name = #_node,					\
			.n_voltages = ARRAY_SIZE(_voltages),		\
			.volt_table =  _voltages,			\
			.ops = &_ops,		\
			.type = REGULATOR_VOLTAGE,			\
			.id = prefix ## _name,		\
			.owner = THIS_MODULE,				\
		},							\
		.reg = prefix ## _reg,				\
		.enable_bit = prefix ## _reg ## _ ## _name ## EN,	\
	}

#define MC13xxx_DEFINE_SW(_name, _node, _reg, _vsel_reg, _voltages, ops) \
	MC13xxx_DEFINE(SW, _name, _node, _reg, _vsel_reg, _voltages, ops)
#define MC13xxx_DEFINE_REGU(_name, _node, _reg, _vsel_reg, _voltages, ops) \
	MC13xxx_DEFINE(REGU, _name, _node, _reg, _vsel_reg, _voltages, ops)

#endif
