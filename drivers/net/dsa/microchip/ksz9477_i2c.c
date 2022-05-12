// SPDX-License-Identifier: GPL-2.0
/*
 * Microchip KSZ9477 series register access through I2C
 *
 * Copyright (C) 2018-2019 Microchip Technology Inc.
 */

#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/regmap.h>

#include "ksz_common.h"

KSZ_REGMAP_TABLE(ksz9477, not_used, 16, 0, 0);

static int ksz9477_i2c_probe(struct i2c_client *i2c,
			     const struct i2c_device_id *i2c_id)
{
	struct regmap_config rc;
	struct ksz_device *dev;
	int i, ret;

	dev = ksz_switch_alloc(&i2c->dev, i2c);
	if (!dev)
		return -ENOMEM;

	for (i = 0; i < ARRAY_SIZE(ksz9477_regmap_config); i++) {
		rc = ksz9477_regmap_config[i];
		rc.lock_arg = &dev->regmap_mutex;
		dev->regmap[i] = devm_regmap_init_i2c(i2c, &rc);
		if (IS_ERR(dev->regmap[i])) {
			ret = PTR_ERR(dev->regmap[i]);
			dev_err(&i2c->dev,
				"Failed to initialize regmap%i: %d\n",
				ksz9477_regmap_config[i].val_bits, ret);
			return ret;
		}
	}

	if (i2c->dev.platform_data)
		dev->pdata = i2c->dev.platform_data;

	ret = ksz9477_switch_register(dev);

	/* Main DSA driver may not be started yet. */
	if (ret)
		return ret;

	i2c_set_clientdata(i2c, dev);

	return 0;
}

static int ksz9477_i2c_remove(struct i2c_client *i2c)
{
	struct ksz_device *dev = i2c_get_clientdata(i2c);

	ksz_switch_remove(dev);

	return 0;
}

static void ksz9477_i2c_shutdown(struct i2c_client *i2c)
{
	struct ksz_device *dev = i2c_get_clientdata(i2c);

	if (dev && dev->dev_ops->shutdown)
		dev->dev_ops->shutdown(dev);
}

static const struct i2c_device_id ksz9477_i2c_id[] = {
	{ "ksz9477-switch", 0 },
	{},
};

MODULE_DEVICE_TABLE(i2c, ksz9477_i2c_id);

static const struct of_device_id ksz9477_dt_ids[] = {
	{ .compatible = "microchip,ksz9477" },
	{ .compatible = "microchip,ksz9897" },
	{ .compatible = "microchip,ksz9893" },
	{ .compatible = "microchip,ksz9563" },
	{ .compatible = "microchip,ksz9567" },
	{},
};
MODULE_DEVICE_TABLE(of, ksz9477_dt_ids);

static struct i2c_driver ksz9477_i2c_driver = {
	.driver = {
		.name	= "ksz9477-switch",
		.of_match_table = of_match_ptr(ksz9477_dt_ids),
	},
	.probe	= ksz9477_i2c_probe,
	.remove	= ksz9477_i2c_remove,
	.shutdown = ksz9477_i2c_shutdown,
	.id_table = ksz9477_i2c_id,
};

module_i2c_driver(ksz9477_i2c_driver);

MODULE_AUTHOR("Tristram Ha <Tristram.Ha@microchip.com>");
MODULE_DESCRIPTION("Microchip KSZ9477 Series Switch I2C access Driver");
MODULE_LICENSE("GPL v2");
