/*
 * Copyright (C) 2015 Texas Instruments Incorporated - https://www.ti.com/
 *	Andrew F. Davis <afd@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether expressed or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * Based on the TPS65912 driver
 */

#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/mfd/core.h>
#include <linux/module.h>

#include <linux/mfd/tps65086.h>

static const struct mfd_cell tps65086_cells[] = {
	{ .name = "tps65086-regulator", },
	{ .name = "tps65086-gpio", },
};

static const struct regmap_range tps65086_yes_ranges[] = {
	regmap_reg_range(TPS65086_IRQ, TPS65086_IRQ),
	regmap_reg_range(TPS65086_PMICSTAT, TPS65086_SHUTDNSRC),
	regmap_reg_range(TPS65086_GPOCTRL, TPS65086_GPOCTRL),
	regmap_reg_range(TPS65086_PG_STATUS1, TPS65086_OC_STATUS),
};

static const struct regmap_access_table tps65086_volatile_table = {
	.yes_ranges = tps65086_yes_ranges,
	.n_yes_ranges = ARRAY_SIZE(tps65086_yes_ranges),
};

static const struct regmap_config tps65086_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.cache_type = REGCACHE_RBTREE,
	.volatile_table = &tps65086_volatile_table,
};

static const struct regmap_irq tps65086_irqs[] = {
	REGMAP_IRQ_REG(TPS65086_IRQ_DIETEMP, 0, TPS65086_IRQ_DIETEMP_MASK),
	REGMAP_IRQ_REG(TPS65086_IRQ_SHUTDN, 0, TPS65086_IRQ_SHUTDN_MASK),
	REGMAP_IRQ_REG(TPS65086_IRQ_FAULT, 0, TPS65086_IRQ_FAULT_MASK),
};

static struct regmap_irq_chip tps65086_irq_chip = {
	.name = "tps65086",
	.status_base = TPS65086_IRQ,
	.mask_base = TPS65086_IRQ_MASK,
	.ack_base = TPS65086_IRQ,
	.init_ack_masked = true,
	.num_regs = 1,
	.irqs = tps65086_irqs,
	.num_irqs = ARRAY_SIZE(tps65086_irqs),
};

static const struct of_device_id tps65086_of_match_table[] = {
	{ .compatible = "ti,tps65086", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, tps65086_of_match_table);

static int tps65086_probe(struct i2c_client *client,
			  const struct i2c_device_id *ids)
{
	struct tps65086 *tps;
	unsigned int version;
	int ret;

	tps = devm_kzalloc(&client->dev, sizeof(*tps), GFP_KERNEL);
	if (!tps)
		return -ENOMEM;

	i2c_set_clientdata(client, tps);
	tps->dev = &client->dev;
	tps->irq = client->irq;

	tps->regmap = devm_regmap_init_i2c(client, &tps65086_regmap_config);
	if (IS_ERR(tps->regmap)) {
		dev_err(tps->dev, "Failed to initialize register map\n");
		return PTR_ERR(tps->regmap);
	}

	ret = regmap_read(tps->regmap, TPS65086_DEVICEID, &version);
	if (ret) {
		dev_err(tps->dev, "Failed to read revision register\n");
		return ret;
	}

	dev_info(tps->dev, "Device: TPS65086%01lX, OTP: %c, Rev: %ld\n",
		 (version & TPS65086_DEVICEID_PART_MASK),
		 (char)((version & TPS65086_DEVICEID_OTP_MASK) >> 4) + 'A',
		 (version & TPS65086_DEVICEID_REV_MASK) >> 6);

	ret = regmap_add_irq_chip(tps->regmap, tps->irq, IRQF_ONESHOT, 0,
				  &tps65086_irq_chip, &tps->irq_data);
	if (ret) {
		dev_err(tps->dev, "Failed to register IRQ chip\n");
		return ret;
	}

	ret = mfd_add_devices(tps->dev, PLATFORM_DEVID_AUTO, tps65086_cells,
			      ARRAY_SIZE(tps65086_cells), NULL, 0,
			      regmap_irq_get_domain(tps->irq_data));
	if (ret) {
		regmap_del_irq_chip(tps->irq, tps->irq_data);
		return ret;
	}

	return 0;
}

static int tps65086_remove(struct i2c_client *client)
{
	struct tps65086 *tps = i2c_get_clientdata(client);

	regmap_del_irq_chip(tps->irq, tps->irq_data);

	return 0;
}

static const struct i2c_device_id tps65086_id_table[] = {
	{ "tps65086", 0 },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(i2c, tps65086_id_table);

static struct i2c_driver tps65086_driver = {
	.driver		= {
		.name	= "tps65086",
		.of_match_table = tps65086_of_match_table,
	},
	.probe		= tps65086_probe,
	.remove		= tps65086_remove,
	.id_table       = tps65086_id_table,
};
module_i2c_driver(tps65086_driver);

MODULE_AUTHOR("Andrew F. Davis <afd@ti.com>");
MODULE_DESCRIPTION("TPS65086 PMIC Driver");
MODULE_LICENSE("GPL v2");
