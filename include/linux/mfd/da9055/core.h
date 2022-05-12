/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * da9055 declarations for DA9055 PMICs.
 *
 * Copyright(c) 2012 Dialog Semiconductor Ltd.
 *
 * Author: David Dajun Chen <dchen@diasemi.com>
 */

#ifndef __DA9055_CORE_H
#define __DA9055_CORE_H

#include <linux/interrupt.h>
#include <linux/regmap.h>

/*
 * PMIC IRQ
 */
#define DA9055_IRQ_ALARM	0x01
#define DA9055_IRQ_TICK		0x02
#define DA9055_IRQ_NONKEY	0x00
#define DA9055_IRQ_REGULATOR	0x0B
#define DA9055_IRQ_HWMON	0x03

struct da9055_pdata;

struct da9055 {
	struct regmap *regmap;
	struct regmap_irq_chip_data *irq_data;
	struct device *dev;
	struct i2c_client *i2c_client;

	int irq_base;
	int chip_irq;
};

/* Device I/O */
static inline int da9055_reg_read(struct da9055 *da9055, unsigned char reg)
{
	int val, ret;

	ret = regmap_read(da9055->regmap, reg, &val);
	if (ret < 0)
		return ret;

	return val;
}

static inline int da9055_reg_write(struct da9055 *da9055, unsigned char reg,
				    unsigned char val)
{
	return regmap_write(da9055->regmap, reg, val);
}

static inline int da9055_group_read(struct da9055 *da9055, unsigned char reg,
				     unsigned reg_cnt, unsigned char *val)
{
	return regmap_bulk_read(da9055->regmap, reg, val, reg_cnt);
}

static inline int da9055_group_write(struct da9055 *da9055, unsigned char reg,
				      unsigned reg_cnt, unsigned char *val)
{
	return regmap_raw_write(da9055->regmap, reg, val, reg_cnt);
}

static inline int da9055_reg_update(struct da9055 *da9055, unsigned char reg,
				     unsigned char bit_mask,
				     unsigned char reg_val)
{
	return regmap_update_bits(da9055->regmap, reg, bit_mask, reg_val);
}

/* Generic Device API */
int da9055_device_init(struct da9055 *da9055);
void da9055_device_exit(struct da9055 *da9055);

extern const struct regmap_config da9055_regmap_config;

#endif /* __DA9055_CORE_H */
