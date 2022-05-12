// SPDX-License-Identifier: GPL-2.0+
//
// Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.

#include <linux/slab.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/mfd/syscon.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/regmap.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/of_regulator.h>
#include <linux/regulator/machine.h>

#define LDO_RAMP_UP_UNIT_IN_CYCLES      64 /* 64 cycles per step */
#define LDO_RAMP_UP_FREQ_IN_MHZ         24 /* cycle based on 24M OSC */

#define LDO_POWER_GATE			0x00
#define LDO_FET_FULL_ON			0x1f

struct anatop_regulator {
	u32 delay_reg;
	int delay_bit_shift;
	int delay_bit_width;
	struct regulator_desc rdesc;
	bool bypass;
	int sel;
};

static int anatop_regmap_set_voltage_time_sel(struct regulator_dev *reg,
	unsigned int old_sel,
	unsigned int new_sel)
{
	struct anatop_regulator *anatop_reg = rdev_get_drvdata(reg);
	u32 val;
	int ret = 0;

	/* check whether need to care about LDO ramp up speed */
	if (anatop_reg->delay_bit_width && new_sel > old_sel) {
		/*
		 * the delay for LDO ramp up time is
		 * based on the register setting, we need
		 * to calculate how many steps LDO need to
		 * ramp up, and how much delay needed. (us)
		 */
		regmap_read(reg->regmap, anatop_reg->delay_reg, &val);
		val = (val >> anatop_reg->delay_bit_shift) &
			((1 << anatop_reg->delay_bit_width) - 1);
		ret = (new_sel - old_sel) * (LDO_RAMP_UP_UNIT_IN_CYCLES <<
			val) / LDO_RAMP_UP_FREQ_IN_MHZ + 1;
	}

	return ret;
}

static int anatop_regmap_enable(struct regulator_dev *reg)
{
	struct anatop_regulator *anatop_reg = rdev_get_drvdata(reg);
	int sel;

	sel = anatop_reg->bypass ? LDO_FET_FULL_ON : anatop_reg->sel;
	return regulator_set_voltage_sel_regmap(reg, sel);
}

static int anatop_regmap_disable(struct regulator_dev *reg)
{
	return regulator_set_voltage_sel_regmap(reg, LDO_POWER_GATE);
}

static int anatop_regmap_is_enabled(struct regulator_dev *reg)
{
	return regulator_get_voltage_sel_regmap(reg) != LDO_POWER_GATE;
}

static int anatop_regmap_core_set_voltage_sel(struct regulator_dev *reg,
					      unsigned selector)
{
	struct anatop_regulator *anatop_reg = rdev_get_drvdata(reg);
	int ret;

	if (anatop_reg->bypass || !anatop_regmap_is_enabled(reg)) {
		anatop_reg->sel = selector;
		return 0;
	}

	ret = regulator_set_voltage_sel_regmap(reg, selector);
	if (!ret)
		anatop_reg->sel = selector;
	return ret;
}

static int anatop_regmap_core_get_voltage_sel(struct regulator_dev *reg)
{
	struct anatop_regulator *anatop_reg = rdev_get_drvdata(reg);

	if (anatop_reg->bypass || !anatop_regmap_is_enabled(reg))
		return anatop_reg->sel;

	return regulator_get_voltage_sel_regmap(reg);
}

static int anatop_regmap_get_bypass(struct regulator_dev *reg, bool *enable)
{
	struct anatop_regulator *anatop_reg = rdev_get_drvdata(reg);
	int sel;

	sel = regulator_get_voltage_sel_regmap(reg);
	if (sel == LDO_FET_FULL_ON)
		WARN_ON(!anatop_reg->bypass);
	else if (sel != LDO_POWER_GATE)
		WARN_ON(anatop_reg->bypass);

	*enable = anatop_reg->bypass;
	return 0;
}

static int anatop_regmap_set_bypass(struct regulator_dev *reg, bool enable)
{
	struct anatop_regulator *anatop_reg = rdev_get_drvdata(reg);
	int sel;

	if (enable == anatop_reg->bypass)
		return 0;

	sel = enable ? LDO_FET_FULL_ON : anatop_reg->sel;
	anatop_reg->bypass = enable;

	return regulator_set_voltage_sel_regmap(reg, sel);
}

static struct regulator_ops anatop_rops = {
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.list_voltage = regulator_list_voltage_linear,
	.map_voltage = regulator_map_voltage_linear,
};

static const struct regulator_ops anatop_core_rops = {
	.enable = anatop_regmap_enable,
	.disable = anatop_regmap_disable,
	.is_enabled = anatop_regmap_is_enabled,
	.set_voltage_sel = anatop_regmap_core_set_voltage_sel,
	.set_voltage_time_sel = anatop_regmap_set_voltage_time_sel,
	.get_voltage_sel = anatop_regmap_core_get_voltage_sel,
	.list_voltage = regulator_list_voltage_linear,
	.map_voltage = regulator_map_voltage_linear,
	.get_bypass = anatop_regmap_get_bypass,
	.set_bypass = anatop_regmap_set_bypass,
};

static int anatop_regulator_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct device_node *anatop_np;
	struct regulator_desc *rdesc;
	struct regulator_dev *rdev;
	struct anatop_regulator *sreg;
	struct regulator_init_data *initdata;
	struct regulator_config config = { };
	struct regmap *regmap;
	u32 control_reg;
	u32 vol_bit_shift;
	u32 vol_bit_width;
	u32 min_bit_val;
	u32 min_voltage;
	u32 max_voltage;
	int ret = 0;
	u32 val;

	sreg = devm_kzalloc(dev, sizeof(*sreg), GFP_KERNEL);
	if (!sreg)
		return -ENOMEM;

	rdesc = &sreg->rdesc;
	rdesc->type = REGULATOR_VOLTAGE;
	rdesc->owner = THIS_MODULE;

	of_property_read_string(np, "regulator-name", &rdesc->name);
	if (!rdesc->name) {
		dev_err(dev, "failed to get a regulator-name\n");
		return -EINVAL;
	}

	initdata = of_get_regulator_init_data(dev, np, rdesc);
	if (!initdata)
		return -ENOMEM;

	initdata->supply_regulator = "vin";

	anatop_np = of_get_parent(np);
	if (!anatop_np)
		return -ENODEV;
	regmap = syscon_node_to_regmap(anatop_np);
	of_node_put(anatop_np);
	if (IS_ERR(regmap))
		return PTR_ERR(regmap);

	ret = of_property_read_u32(np, "anatop-reg-offset", &control_reg);
	if (ret) {
		dev_err(dev, "no anatop-reg-offset property set\n");
		return ret;
	}
	ret = of_property_read_u32(np, "anatop-vol-bit-width", &vol_bit_width);
	if (ret) {
		dev_err(dev, "no anatop-vol-bit-width property set\n");
		return ret;
	}
	ret = of_property_read_u32(np, "anatop-vol-bit-shift", &vol_bit_shift);
	if (ret) {
		dev_err(dev, "no anatop-vol-bit-shift property set\n");
		return ret;
	}
	ret = of_property_read_u32(np, "anatop-min-bit-val", &min_bit_val);
	if (ret) {
		dev_err(dev, "no anatop-min-bit-val property set\n");
		return ret;
	}
	ret = of_property_read_u32(np, "anatop-min-voltage", &min_voltage);
	if (ret) {
		dev_err(dev, "no anatop-min-voltage property set\n");
		return ret;
	}
	ret = of_property_read_u32(np, "anatop-max-voltage", &max_voltage);
	if (ret) {
		dev_err(dev, "no anatop-max-voltage property set\n");
		return ret;
	}

	/* read LDO ramp up setting, only for core reg */
	of_property_read_u32(np, "anatop-delay-reg-offset",
			     &sreg->delay_reg);
	of_property_read_u32(np, "anatop-delay-bit-width",
			     &sreg->delay_bit_width);
	of_property_read_u32(np, "anatop-delay-bit-shift",
			     &sreg->delay_bit_shift);

	rdesc->n_voltages = (max_voltage - min_voltage) / 25000 + 1
			    + min_bit_val;
	rdesc->min_uV = min_voltage;
	rdesc->uV_step = 25000;
	rdesc->linear_min_sel = min_bit_val;
	rdesc->vsel_reg = control_reg;
	rdesc->vsel_mask = ((1 << vol_bit_width) - 1) << vol_bit_shift;
	rdesc->min_dropout_uV = 125000;

	config.dev = &pdev->dev;
	config.init_data = initdata;
	config.driver_data = sreg;
	config.of_node = pdev->dev.of_node;
	config.regmap = regmap;

	/* Only core regulators have the ramp up delay configuration. */
	if (control_reg && sreg->delay_bit_width) {
		rdesc->ops = &anatop_core_rops;

		ret = regmap_read(config.regmap, rdesc->vsel_reg, &val);
		if (ret) {
			dev_err(dev, "failed to read initial state\n");
			return ret;
		}

		sreg->sel = (val & rdesc->vsel_mask) >> vol_bit_shift;
		if (sreg->sel == LDO_FET_FULL_ON) {
			sreg->sel = 0;
			sreg->bypass = true;
		}

		/*
		 * In case vddpu was disabled by the bootloader, we need to set
		 * a sane default until imx6-cpufreq was probed and changes the
		 * voltage to the correct value. In this case we set 1.25V.
		 */
		if (!sreg->sel && !strcmp(rdesc->name, "vddpu"))
			sreg->sel = 22;

		/* set the default voltage of the pcie phy to be 1.100v */
		if (!sreg->sel && !strcmp(rdesc->name, "vddpcie"))
			sreg->sel = 0x10;

		if (!sreg->bypass && !sreg->sel) {
			dev_err(&pdev->dev, "Failed to read a valid default voltage selector.\n");
			return -EINVAL;
		}
	} else {
		u32 enable_bit;

		rdesc->ops = &anatop_rops;

		if (!of_property_read_u32(np, "anatop-enable-bit",
					  &enable_bit)) {
			anatop_rops.enable  = regulator_enable_regmap;
			anatop_rops.disable = regulator_disable_regmap;
			anatop_rops.is_enabled = regulator_is_enabled_regmap;

			rdesc->enable_reg = control_reg;
			rdesc->enable_mask = BIT(enable_bit);
		}
	}

	/* register regulator */
	rdev = devm_regulator_register(dev, rdesc, &config);
	if (IS_ERR(rdev)) {
		ret = PTR_ERR(rdev);
		if (ret == -EPROBE_DEFER)
			dev_dbg(dev, "failed to register %s, deferring...\n",
				rdesc->name);
		else
			dev_err(dev, "failed to register %s\n", rdesc->name);
		return ret;
	}

	platform_set_drvdata(pdev, rdev);

	return 0;
}

static const struct of_device_id of_anatop_regulator_match_tbl[] = {
	{ .compatible = "fsl,anatop-regulator", },
	{ /* end */ }
};
MODULE_DEVICE_TABLE(of, of_anatop_regulator_match_tbl);

static struct platform_driver anatop_regulator_driver = {
	.driver = {
		.name	= "anatop_regulator",
		.of_match_table = of_anatop_regulator_match_tbl,
	},
	.probe	= anatop_regulator_probe,
};

static int __init anatop_regulator_init(void)
{
	return platform_driver_register(&anatop_regulator_driver);
}
postcore_initcall(anatop_regulator_init);

static void __exit anatop_regulator_exit(void)
{
	platform_driver_unregister(&anatop_regulator_driver);
}
module_exit(anatop_regulator_exit);

MODULE_AUTHOR("Nancy Chen <Nancy.Chen@freescale.com>");
MODULE_AUTHOR("Ying-Chun Liu (PaulLiu) <paul.liu@linaro.org>");
MODULE_DESCRIPTION("ANATOP Regulator driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:anatop_regulator");
