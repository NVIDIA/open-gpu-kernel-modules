/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Core driver interface for TI TPS65090 PMIC family
 *
 * Copyright (C) 2012 NVIDIA Corporation
 */

#ifndef __LINUX_MFD_TPS65090_H
#define __LINUX_MFD_TPS65090_H

#include <linux/irq.h>
#include <linux/regmap.h>

/* TPS65090 IRQs */
enum {
	TPS65090_IRQ_INTERRUPT,
	TPS65090_IRQ_VAC_STATUS_CHANGE,
	TPS65090_IRQ_VSYS_STATUS_CHANGE,
	TPS65090_IRQ_BAT_STATUS_CHANGE,
	TPS65090_IRQ_CHARGING_STATUS_CHANGE,
	TPS65090_IRQ_CHARGING_COMPLETE,
	TPS65090_IRQ_OVERLOAD_DCDC1,
	TPS65090_IRQ_OVERLOAD_DCDC2,
	TPS65090_IRQ_OVERLOAD_DCDC3,
	TPS65090_IRQ_OVERLOAD_FET1,
	TPS65090_IRQ_OVERLOAD_FET2,
	TPS65090_IRQ_OVERLOAD_FET3,
	TPS65090_IRQ_OVERLOAD_FET4,
	TPS65090_IRQ_OVERLOAD_FET5,
	TPS65090_IRQ_OVERLOAD_FET6,
	TPS65090_IRQ_OVERLOAD_FET7,
};

/* TPS65090 Regulator ID */
enum {
	TPS65090_REGULATOR_DCDC1,
	TPS65090_REGULATOR_DCDC2,
	TPS65090_REGULATOR_DCDC3,
	TPS65090_REGULATOR_FET1,
	TPS65090_REGULATOR_FET2,
	TPS65090_REGULATOR_FET3,
	TPS65090_REGULATOR_FET4,
	TPS65090_REGULATOR_FET5,
	TPS65090_REGULATOR_FET6,
	TPS65090_REGULATOR_FET7,
	TPS65090_REGULATOR_LDO1,
	TPS65090_REGULATOR_LDO2,

	/* Last entry for maximum ID */
	TPS65090_REGULATOR_MAX,
};

/* Register addresses */
#define TPS65090_REG_INTR_STS	0x00
#define TPS65090_REG_INTR_STS2	0x01
#define TPS65090_REG_INTR_MASK	0x02
#define TPS65090_REG_INTR_MASK2	0x03
#define TPS65090_REG_CG_CTRL0	0x04
#define TPS65090_REG_CG_CTRL1	0x05
#define TPS65090_REG_CG_CTRL2	0x06
#define TPS65090_REG_CG_CTRL3	0x07
#define TPS65090_REG_CG_CTRL4	0x08
#define TPS65090_REG_CG_CTRL5	0x09
#define TPS65090_REG_CG_STATUS1	0x0a
#define TPS65090_REG_CG_STATUS2	0x0b
#define TPS65090_REG_AD_OUT1	0x17
#define TPS65090_REG_AD_OUT2	0x18

#define TPS65090_MAX_REG	TPS65090_REG_AD_OUT2
#define TPS65090_NUM_REGS	(TPS65090_MAX_REG + 1)

struct gpio_desc;

struct tps65090 {
	struct device		*dev;
	struct regmap		*rmap;
	struct regmap_irq_chip_data *irq_data;
};

/*
 * struct tps65090_regulator_plat_data
 *
 * @reg_init_data: The regulator init data.
 * @enable_ext_control: Enable extrenal control or not. Only available for
 *     DCDC1, DCDC2 and DCDC3.
 * @gpiod: Gpio descriptor if external control is enabled and controlled through
 *     gpio
 * @overcurrent_wait_valid: True if the overcurrent_wait should be applied.
 * @overcurrent_wait: Value to set as the overcurrent wait time.  This is the
 *     actual bitfield value, not a time in ms (valid value are 0 - 3).
 */
struct tps65090_regulator_plat_data {
	struct regulator_init_data *reg_init_data;
	bool enable_ext_control;
	struct gpio_desc *gpiod;
	bool overcurrent_wait_valid;
	int overcurrent_wait;
};

struct tps65090_platform_data {
	int irq_base;

	char **supplied_to;
	size_t num_supplicants;
	int enable_low_current_chrg;

	struct tps65090_regulator_plat_data *reg_pdata[TPS65090_REGULATOR_MAX];
};

/*
 * NOTE: the functions below are not intended for use outside
 * of the TPS65090 sub-device drivers
 */
static inline int tps65090_write(struct device *dev, int reg, uint8_t val)
{
	struct tps65090 *tps = dev_get_drvdata(dev);

	return regmap_write(tps->rmap, reg, val);
}

static inline int tps65090_read(struct device *dev, int reg, uint8_t *val)
{
	struct tps65090 *tps = dev_get_drvdata(dev);
	unsigned int temp_val;
	int ret;

	ret = regmap_read(tps->rmap, reg, &temp_val);
	if (!ret)
		*val = temp_val;
	return ret;
}

static inline int tps65090_set_bits(struct device *dev, int reg,
		uint8_t bit_num)
{
	struct tps65090 *tps = dev_get_drvdata(dev);

	return regmap_update_bits(tps->rmap, reg, BIT(bit_num), ~0u);
}

static inline int tps65090_clr_bits(struct device *dev, int reg,
		uint8_t bit_num)
{
	struct tps65090 *tps = dev_get_drvdata(dev);

	return regmap_update_bits(tps->rmap, reg, BIT(bit_num), 0u);
}

#endif /*__LINUX_MFD_TPS65090_H */
