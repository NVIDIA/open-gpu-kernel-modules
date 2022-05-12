// SPDX-License-Identifier: GPL-2.0+
//
// da9210-regulator.c - Regulator device driver for DA9210
// Copyright (C) 2013  Dialog Semiconductor Ltd.

#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/of_device.h>
#include <linux/regulator/of_regulator.h>
#include <linux/regmap.h>

#include "da9210-regulator.h"

struct da9210 {
	struct regulator_dev *rdev;
	struct regmap *regmap;
};

static const struct regmap_config da9210_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
};

static const struct regulator_ops da9210_buck_ops = {
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.is_enabled = regulator_is_enabled_regmap,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.list_voltage = regulator_list_voltage_linear,
	.set_current_limit = regulator_set_current_limit_regmap,
	.get_current_limit = regulator_get_current_limit_regmap,
};

/* Default limits measured in millivolts and milliamps */
#define DA9210_MIN_MV		300
#define DA9210_MAX_MV		1570
#define DA9210_STEP_MV		10

/* Current limits for buck (uA) indices corresponds with register values */
static const unsigned int da9210_buck_limits[] = {
	1600000, 1800000, 2000000, 2200000, 2400000, 2600000, 2800000, 3000000,
	3200000, 3400000, 3600000, 3800000, 4000000, 4200000, 4400000, 4600000
};

static const struct regulator_desc da9210_reg = {
	.name = "DA9210",
	.id = 0,
	.ops = &da9210_buck_ops,
	.type = REGULATOR_VOLTAGE,
	.n_voltages = ((DA9210_MAX_MV - DA9210_MIN_MV) / DA9210_STEP_MV) + 1,
	.min_uV = (DA9210_MIN_MV * 1000),
	.uV_step = (DA9210_STEP_MV * 1000),
	.vsel_reg = DA9210_REG_VBUCK_A,
	.vsel_mask = DA9210_VBUCK_MASK,
	.enable_reg = DA9210_REG_BUCK_CONT,
	.enable_mask = DA9210_BUCK_EN,
	.owner = THIS_MODULE,
	.curr_table = da9210_buck_limits,
	.n_current_limits = ARRAY_SIZE(da9210_buck_limits),
	.csel_reg = DA9210_REG_BUCK_ILIM,
	.csel_mask = DA9210_BUCK_ILIM_MASK,
};

static irqreturn_t da9210_irq_handler(int irq, void *data)
{
	struct da9210 *chip = data;
	unsigned int val, handled = 0;
	int error, ret = IRQ_NONE;

	error = regmap_read(chip->regmap, DA9210_REG_EVENT_B, &val);
	if (error < 0)
		goto error_i2c;

	if (val & DA9210_E_OVCURR) {
		regulator_notifier_call_chain(chip->rdev,
					      REGULATOR_EVENT_OVER_CURRENT,
					      NULL);
		handled |= DA9210_E_OVCURR;
	}
	if (val & DA9210_E_NPWRGOOD) {
		regulator_notifier_call_chain(chip->rdev,
					      REGULATOR_EVENT_UNDER_VOLTAGE,
					      NULL);
		handled |= DA9210_E_NPWRGOOD;
	}
	if (val & (DA9210_E_TEMP_WARN | DA9210_E_TEMP_CRIT)) {
		regulator_notifier_call_chain(chip->rdev,
					      REGULATOR_EVENT_OVER_TEMP, NULL);
		handled |= val & (DA9210_E_TEMP_WARN | DA9210_E_TEMP_CRIT);
	}
	if (val & DA9210_E_VMAX) {
		regulator_notifier_call_chain(chip->rdev,
					      REGULATOR_EVENT_REGULATION_OUT,
					      NULL);
		handled |= DA9210_E_VMAX;
	}

	if (handled) {
		/* Clear handled events */
		error = regmap_write(chip->regmap, DA9210_REG_EVENT_B, handled);
		if (error < 0)
			goto error_i2c;

		ret = IRQ_HANDLED;
	}

	return ret;

error_i2c:
	dev_err(regmap_get_device(chip->regmap), "I2C error : %d\n", error);
	return ret;
}

/*
 * I2C driver interface functions
 */

static const struct of_device_id __maybe_unused da9210_dt_ids[] = {
	{ .compatible = "dlg,da9210", },
	{ }
};
MODULE_DEVICE_TABLE(of, da9210_dt_ids);

static int da9210_i2c_probe(struct i2c_client *i2c)
{
	struct da9210 *chip;
	struct device *dev = &i2c->dev;
	struct da9210_pdata *pdata = dev_get_platdata(dev);
	struct regulator_dev *rdev = NULL;
	struct regulator_config config = { };
	int error;
	const struct of_device_id *match;

	if (i2c->dev.of_node && !pdata) {
		match = of_match_device(of_match_ptr(da9210_dt_ids),
						&i2c->dev);
		if (!match) {
			dev_err(&i2c->dev, "Error: No device match found\n");
			return -ENODEV;
		}
	}

	chip = devm_kzalloc(&i2c->dev, sizeof(struct da9210), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	chip->regmap = devm_regmap_init_i2c(i2c, &da9210_regmap_config);
	if (IS_ERR(chip->regmap)) {
		error = PTR_ERR(chip->regmap);
		dev_err(&i2c->dev, "Failed to allocate register map: %d\n",
			error);
		return error;
	}

	config.dev = &i2c->dev;
	config.init_data = pdata ? &pdata->da9210_constraints :
		of_get_regulator_init_data(dev, dev->of_node, &da9210_reg);
	config.driver_data = chip;
	config.regmap = chip->regmap;
	config.of_node = dev->of_node;

	/* Mask all interrupt sources to deassert interrupt line */
	error = regmap_write(chip->regmap, DA9210_REG_MASK_A, ~0);
	if (!error)
		error = regmap_write(chip->regmap, DA9210_REG_MASK_B, ~0);
	if (error) {
		dev_err(&i2c->dev, "Failed to write to mask reg: %d\n", error);
		return error;
	}

	rdev = devm_regulator_register(&i2c->dev, &da9210_reg, &config);
	if (IS_ERR(rdev)) {
		dev_err(&i2c->dev, "Failed to register DA9210 regulator\n");
		return PTR_ERR(rdev);
	}

	chip->rdev = rdev;
	if (i2c->irq) {
		error = devm_request_threaded_irq(&i2c->dev, i2c->irq, NULL,
						  da9210_irq_handler,
						  IRQF_TRIGGER_LOW |
						  IRQF_ONESHOT | IRQF_SHARED,
						  "da9210", chip);
		if (error) {
			dev_err(&i2c->dev, "Failed to request IRQ%u: %d\n",
				i2c->irq, error);
			return error;
		}

		error = regmap_update_bits(chip->regmap, DA9210_REG_MASK_B,
					 DA9210_M_OVCURR | DA9210_M_NPWRGOOD |
					 DA9210_M_TEMP_WARN |
					 DA9210_M_TEMP_CRIT | DA9210_M_VMAX, 0);
		if (error < 0) {
			dev_err(&i2c->dev, "Failed to update mask reg: %d\n",
				error);
			return error;
		}
	} else {
		dev_warn(&i2c->dev, "No IRQ configured\n");
	}

	i2c_set_clientdata(i2c, chip);

	return 0;
}

static const struct i2c_device_id da9210_i2c_id[] = {
	{"da9210", 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, da9210_i2c_id);

static struct i2c_driver da9210_regulator_driver = {
	.driver = {
		.name = "da9210",
		.of_match_table = of_match_ptr(da9210_dt_ids),
	},
	.probe_new = da9210_i2c_probe,
	.id_table = da9210_i2c_id,
};

module_i2c_driver(da9210_regulator_driver);

MODULE_AUTHOR("S Twiss <stwiss.opensource@diasemi.com>");
MODULE_DESCRIPTION("Regulator device driver for Dialog DA9210");
MODULE_LICENSE("GPL v2");
