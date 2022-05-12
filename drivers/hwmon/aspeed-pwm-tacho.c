// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2016 Google, Inc
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/gpio/consumer.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/sysfs.h>
#include <linux/thermal.h>

/* ASPEED PWM & FAN Tach Register Definition */
#define ASPEED_PTCR_CTRL		0x00
#define ASPEED_PTCR_CLK_CTRL		0x04
#define ASPEED_PTCR_DUTY0_CTRL		0x08
#define ASPEED_PTCR_DUTY1_CTRL		0x0c
#define ASPEED_PTCR_TYPEM_CTRL		0x10
#define ASPEED_PTCR_TYPEM_CTRL1		0x14
#define ASPEED_PTCR_TYPEN_CTRL		0x18
#define ASPEED_PTCR_TYPEN_CTRL1		0x1c
#define ASPEED_PTCR_TACH_SOURCE		0x20
#define ASPEED_PTCR_TRIGGER		0x28
#define ASPEED_PTCR_RESULT		0x2c
#define ASPEED_PTCR_INTR_CTRL		0x30
#define ASPEED_PTCR_INTR_STS		0x34
#define ASPEED_PTCR_TYPEM_LIMIT		0x38
#define ASPEED_PTCR_TYPEN_LIMIT		0x3C
#define ASPEED_PTCR_CTRL_EXT		0x40
#define ASPEED_PTCR_CLK_CTRL_EXT	0x44
#define ASPEED_PTCR_DUTY2_CTRL		0x48
#define ASPEED_PTCR_DUTY3_CTRL		0x4c
#define ASPEED_PTCR_TYPEO_CTRL		0x50
#define ASPEED_PTCR_TYPEO_CTRL1		0x54
#define ASPEED_PTCR_TACH_SOURCE_EXT	0x60
#define ASPEED_PTCR_TYPEO_LIMIT		0x78

/* ASPEED_PTCR_CTRL : 0x00 - General Control Register */
#define ASPEED_PTCR_CTRL_SET_PWMD_TYPE_PART1	15
#define ASPEED_PTCR_CTRL_SET_PWMD_TYPE_PART2	6
#define ASPEED_PTCR_CTRL_SET_PWMD_TYPE_MASK	(BIT(7) | BIT(15))

#define ASPEED_PTCR_CTRL_SET_PWMC_TYPE_PART1	14
#define ASPEED_PTCR_CTRL_SET_PWMC_TYPE_PART2	5
#define ASPEED_PTCR_CTRL_SET_PWMC_TYPE_MASK	(BIT(6) | BIT(14))

#define ASPEED_PTCR_CTRL_SET_PWMB_TYPE_PART1	13
#define ASPEED_PTCR_CTRL_SET_PWMB_TYPE_PART2	4
#define ASPEED_PTCR_CTRL_SET_PWMB_TYPE_MASK	(BIT(5) | BIT(13))

#define ASPEED_PTCR_CTRL_SET_PWMA_TYPE_PART1	12
#define ASPEED_PTCR_CTRL_SET_PWMA_TYPE_PART2	3
#define ASPEED_PTCR_CTRL_SET_PWMA_TYPE_MASK	(BIT(4) | BIT(12))

#define	ASPEED_PTCR_CTRL_FAN_NUM_EN(x)	BIT(16 + (x))

#define	ASPEED_PTCR_CTRL_PWMD_EN	BIT(11)
#define	ASPEED_PTCR_CTRL_PWMC_EN	BIT(10)
#define	ASPEED_PTCR_CTRL_PWMB_EN	BIT(9)
#define	ASPEED_PTCR_CTRL_PWMA_EN	BIT(8)

#define	ASPEED_PTCR_CTRL_CLK_SRC	BIT(1)
#define	ASPEED_PTCR_CTRL_CLK_EN		BIT(0)

/* ASPEED_PTCR_CLK_CTRL : 0x04 - Clock Control Register */
/* TYPE N */
#define ASPEED_PTCR_CLK_CTRL_TYPEN_MASK		GENMASK(31, 16)
#define ASPEED_PTCR_CLK_CTRL_TYPEN_UNIT		24
#define ASPEED_PTCR_CLK_CTRL_TYPEN_H		20
#define ASPEED_PTCR_CLK_CTRL_TYPEN_L		16
/* TYPE M */
#define ASPEED_PTCR_CLK_CTRL_TYPEM_MASK         GENMASK(15, 0)
#define ASPEED_PTCR_CLK_CTRL_TYPEM_UNIT		8
#define ASPEED_PTCR_CLK_CTRL_TYPEM_H		4
#define ASPEED_PTCR_CLK_CTRL_TYPEM_L		0

/*
 * ASPEED_PTCR_DUTY_CTRL/1/2/3 : 0x08/0x0C/0x48/0x4C - PWM-FAN duty control
 * 0/1/2/3 register
 */
#define DUTY_CTRL_PWM2_FALL_POINT	24
#define DUTY_CTRL_PWM2_RISE_POINT	16
#define DUTY_CTRL_PWM2_RISE_FALL_MASK	GENMASK(31, 16)
#define DUTY_CTRL_PWM1_FALL_POINT	8
#define DUTY_CTRL_PWM1_RISE_POINT	0
#define DUTY_CTRL_PWM1_RISE_FALL_MASK   GENMASK(15, 0)

/* ASPEED_PTCR_TYPEM_CTRL : 0x10/0x18/0x50 - Type M/N/O Ctrl 0 Register */
#define TYPE_CTRL_FAN_MASK		(GENMASK(5, 1) | GENMASK(31, 16))
#define TYPE_CTRL_FAN1_MASK		GENMASK(31, 0)
#define TYPE_CTRL_FAN_PERIOD		16
#define TYPE_CTRL_FAN_MODE		4
#define TYPE_CTRL_FAN_DIVISION		1
#define TYPE_CTRL_FAN_TYPE_EN		1

/* ASPEED_PTCR_TACH_SOURCE : 0x20/0x60 - Tach Source Register */
/* bit [0,1] at 0x20, bit [2] at 0x60 */
#define TACH_PWM_SOURCE_BIT01(x)	((x) * 2)
#define TACH_PWM_SOURCE_BIT2(x)		((x) * 2)
#define TACH_PWM_SOURCE_MASK_BIT01(x)	(0x3 << ((x) * 2))
#define TACH_PWM_SOURCE_MASK_BIT2(x)	BIT((x) * 2)

/* ASPEED_PTCR_RESULT : 0x2c - Result Register */
#define RESULT_STATUS_MASK		BIT(31)
#define RESULT_VALUE_MASK		0xfffff

/* ASPEED_PTCR_CTRL_EXT : 0x40 - General Control Extension #1 Register */
#define ASPEED_PTCR_CTRL_SET_PWMH_TYPE_PART1	15
#define ASPEED_PTCR_CTRL_SET_PWMH_TYPE_PART2	6
#define ASPEED_PTCR_CTRL_SET_PWMH_TYPE_MASK	(BIT(7) | BIT(15))

#define ASPEED_PTCR_CTRL_SET_PWMG_TYPE_PART1	14
#define ASPEED_PTCR_CTRL_SET_PWMG_TYPE_PART2	5
#define ASPEED_PTCR_CTRL_SET_PWMG_TYPE_MASK	(BIT(6) | BIT(14))

#define ASPEED_PTCR_CTRL_SET_PWMF_TYPE_PART1	13
#define ASPEED_PTCR_CTRL_SET_PWMF_TYPE_PART2	4
#define ASPEED_PTCR_CTRL_SET_PWMF_TYPE_MASK	(BIT(5) | BIT(13))

#define ASPEED_PTCR_CTRL_SET_PWME_TYPE_PART1	12
#define ASPEED_PTCR_CTRL_SET_PWME_TYPE_PART2	3
#define ASPEED_PTCR_CTRL_SET_PWME_TYPE_MASK	(BIT(4) | BIT(12))

#define	ASPEED_PTCR_CTRL_PWMH_EN	BIT(11)
#define	ASPEED_PTCR_CTRL_PWMG_EN	BIT(10)
#define	ASPEED_PTCR_CTRL_PWMF_EN	BIT(9)
#define	ASPEED_PTCR_CTRL_PWME_EN	BIT(8)

/* ASPEED_PTCR_CLK_EXT_CTRL : 0x44 - Clock Control Extension #1 Register */
/* TYPE O */
#define ASPEED_PTCR_CLK_CTRL_TYPEO_MASK         GENMASK(15, 0)
#define ASPEED_PTCR_CLK_CTRL_TYPEO_UNIT		8
#define ASPEED_PTCR_CLK_CTRL_TYPEO_H		4
#define ASPEED_PTCR_CLK_CTRL_TYPEO_L		0

#define PWM_MAX 255

#define BOTH_EDGES 0x02 /* 10b */

#define M_PWM_DIV_H 0x00
#define M_PWM_DIV_L 0x05
#define M_PWM_PERIOD 0x5F
#define M_TACH_CLK_DIV 0x00
/*
 * 5:4 Type N fan tach mode selection bit:
 * 00: falling
 * 01: rising
 * 10: both
 * 11: reserved.
 */
#define M_TACH_MODE 0x02 /* 10b */
#define M_TACH_UNIT 0x0210
#define INIT_FAN_CTRL 0xFF

/* How long we sleep in us while waiting for an RPM result. */
#define ASPEED_RPM_STATUS_SLEEP_USEC	500

#define MAX_CDEV_NAME_LEN 16

struct aspeed_cooling_device {
	char name[16];
	struct aspeed_pwm_tacho_data *priv;
	struct thermal_cooling_device *tcdev;
	int pwm_port;
	u8 *cooling_levels;
	u8 max_state;
	u8 cur_state;
};

struct aspeed_pwm_tacho_data {
	struct regmap *regmap;
	struct reset_control *rst;
	unsigned long clk_freq;
	bool pwm_present[8];
	bool fan_tach_present[16];
	u8 type_pwm_clock_unit[3];
	u8 type_pwm_clock_division_h[3];
	u8 type_pwm_clock_division_l[3];
	u8 type_fan_tach_clock_division[3];
	u8 type_fan_tach_mode[3];
	u16 type_fan_tach_unit[3];
	u8 pwm_port_type[8];
	u8 pwm_port_fan_ctrl[8];
	u8 fan_tach_ch_source[16];
	struct aspeed_cooling_device *cdev[8];
	const struct attribute_group *groups[3];
};

enum type { TYPEM, TYPEN, TYPEO };

struct type_params {
	u32 l_value;
	u32 h_value;
	u32 unit_value;
	u32 clk_ctrl_mask;
	u32 clk_ctrl_reg;
	u32 ctrl_reg;
	u32 ctrl_reg1;
};

static const struct type_params type_params[] = {
	[TYPEM] = {
		.l_value = ASPEED_PTCR_CLK_CTRL_TYPEM_L,
		.h_value = ASPEED_PTCR_CLK_CTRL_TYPEM_H,
		.unit_value = ASPEED_PTCR_CLK_CTRL_TYPEM_UNIT,
		.clk_ctrl_mask = ASPEED_PTCR_CLK_CTRL_TYPEM_MASK,
		.clk_ctrl_reg = ASPEED_PTCR_CLK_CTRL,
		.ctrl_reg = ASPEED_PTCR_TYPEM_CTRL,
		.ctrl_reg1 = ASPEED_PTCR_TYPEM_CTRL1,
	},
	[TYPEN] = {
		.l_value = ASPEED_PTCR_CLK_CTRL_TYPEN_L,
		.h_value = ASPEED_PTCR_CLK_CTRL_TYPEN_H,
		.unit_value = ASPEED_PTCR_CLK_CTRL_TYPEN_UNIT,
		.clk_ctrl_mask = ASPEED_PTCR_CLK_CTRL_TYPEN_MASK,
		.clk_ctrl_reg = ASPEED_PTCR_CLK_CTRL,
		.ctrl_reg = ASPEED_PTCR_TYPEN_CTRL,
		.ctrl_reg1 = ASPEED_PTCR_TYPEN_CTRL1,
	},
	[TYPEO] = {
		.l_value = ASPEED_PTCR_CLK_CTRL_TYPEO_L,
		.h_value = ASPEED_PTCR_CLK_CTRL_TYPEO_H,
		.unit_value = ASPEED_PTCR_CLK_CTRL_TYPEO_UNIT,
		.clk_ctrl_mask = ASPEED_PTCR_CLK_CTRL_TYPEO_MASK,
		.clk_ctrl_reg = ASPEED_PTCR_CLK_CTRL_EXT,
		.ctrl_reg = ASPEED_PTCR_TYPEO_CTRL,
		.ctrl_reg1 = ASPEED_PTCR_TYPEO_CTRL1,
	}
};

enum pwm_port { PWMA, PWMB, PWMC, PWMD, PWME, PWMF, PWMG, PWMH };

struct pwm_port_params {
	u32 pwm_en;
	u32 ctrl_reg;
	u32 type_part1;
	u32 type_part2;
	u32 type_mask;
	u32 duty_ctrl_rise_point;
	u32 duty_ctrl_fall_point;
	u32 duty_ctrl_reg;
	u32 duty_ctrl_rise_fall_mask;
};

static const struct pwm_port_params pwm_port_params[] = {
	[PWMA] = {
		.pwm_en = ASPEED_PTCR_CTRL_PWMA_EN,
		.ctrl_reg = ASPEED_PTCR_CTRL,
		.type_part1 = ASPEED_PTCR_CTRL_SET_PWMA_TYPE_PART1,
		.type_part2 = ASPEED_PTCR_CTRL_SET_PWMA_TYPE_PART2,
		.type_mask = ASPEED_PTCR_CTRL_SET_PWMA_TYPE_MASK,
		.duty_ctrl_rise_point = DUTY_CTRL_PWM1_RISE_POINT,
		.duty_ctrl_fall_point = DUTY_CTRL_PWM1_FALL_POINT,
		.duty_ctrl_reg = ASPEED_PTCR_DUTY0_CTRL,
		.duty_ctrl_rise_fall_mask = DUTY_CTRL_PWM1_RISE_FALL_MASK,
	},
	[PWMB] = {
		.pwm_en = ASPEED_PTCR_CTRL_PWMB_EN,
		.ctrl_reg = ASPEED_PTCR_CTRL,
		.type_part1 = ASPEED_PTCR_CTRL_SET_PWMB_TYPE_PART1,
		.type_part2 = ASPEED_PTCR_CTRL_SET_PWMB_TYPE_PART2,
		.type_mask = ASPEED_PTCR_CTRL_SET_PWMB_TYPE_MASK,
		.duty_ctrl_rise_point = DUTY_CTRL_PWM2_RISE_POINT,
		.duty_ctrl_fall_point = DUTY_CTRL_PWM2_FALL_POINT,
		.duty_ctrl_reg = ASPEED_PTCR_DUTY0_CTRL,
		.duty_ctrl_rise_fall_mask = DUTY_CTRL_PWM2_RISE_FALL_MASK,
	},
	[PWMC] = {
		.pwm_en = ASPEED_PTCR_CTRL_PWMC_EN,
		.ctrl_reg = ASPEED_PTCR_CTRL,
		.type_part1 = ASPEED_PTCR_CTRL_SET_PWMC_TYPE_PART1,
		.type_part2 = ASPEED_PTCR_CTRL_SET_PWMC_TYPE_PART2,
		.type_mask = ASPEED_PTCR_CTRL_SET_PWMC_TYPE_MASK,
		.duty_ctrl_rise_point = DUTY_CTRL_PWM1_RISE_POINT,
		.duty_ctrl_fall_point = DUTY_CTRL_PWM1_FALL_POINT,
		.duty_ctrl_reg = ASPEED_PTCR_DUTY1_CTRL,
		.duty_ctrl_rise_fall_mask = DUTY_CTRL_PWM1_RISE_FALL_MASK,
	},
	[PWMD] = {
		.pwm_en = ASPEED_PTCR_CTRL_PWMD_EN,
		.ctrl_reg = ASPEED_PTCR_CTRL,
		.type_part1 = ASPEED_PTCR_CTRL_SET_PWMD_TYPE_PART1,
		.type_part2 = ASPEED_PTCR_CTRL_SET_PWMD_TYPE_PART2,
		.type_mask = ASPEED_PTCR_CTRL_SET_PWMD_TYPE_MASK,
		.duty_ctrl_rise_point = DUTY_CTRL_PWM2_RISE_POINT,
		.duty_ctrl_fall_point = DUTY_CTRL_PWM2_FALL_POINT,
		.duty_ctrl_reg = ASPEED_PTCR_DUTY1_CTRL,
		.duty_ctrl_rise_fall_mask = DUTY_CTRL_PWM2_RISE_FALL_MASK,
	},
	[PWME] = {
		.pwm_en = ASPEED_PTCR_CTRL_PWME_EN,
		.ctrl_reg = ASPEED_PTCR_CTRL_EXT,
		.type_part1 = ASPEED_PTCR_CTRL_SET_PWME_TYPE_PART1,
		.type_part2 = ASPEED_PTCR_CTRL_SET_PWME_TYPE_PART2,
		.type_mask = ASPEED_PTCR_CTRL_SET_PWME_TYPE_MASK,
		.duty_ctrl_rise_point = DUTY_CTRL_PWM1_RISE_POINT,
		.duty_ctrl_fall_point = DUTY_CTRL_PWM1_FALL_POINT,
		.duty_ctrl_reg = ASPEED_PTCR_DUTY2_CTRL,
		.duty_ctrl_rise_fall_mask = DUTY_CTRL_PWM1_RISE_FALL_MASK,
	},
	[PWMF] = {
		.pwm_en = ASPEED_PTCR_CTRL_PWMF_EN,
		.ctrl_reg = ASPEED_PTCR_CTRL_EXT,
		.type_part1 = ASPEED_PTCR_CTRL_SET_PWMF_TYPE_PART1,
		.type_part2 = ASPEED_PTCR_CTRL_SET_PWMF_TYPE_PART2,
		.type_mask = ASPEED_PTCR_CTRL_SET_PWMF_TYPE_MASK,
		.duty_ctrl_rise_point = DUTY_CTRL_PWM2_RISE_POINT,
		.duty_ctrl_fall_point = DUTY_CTRL_PWM2_FALL_POINT,
		.duty_ctrl_reg = ASPEED_PTCR_DUTY2_CTRL,
		.duty_ctrl_rise_fall_mask = DUTY_CTRL_PWM2_RISE_FALL_MASK,
	},
	[PWMG] = {
		.pwm_en = ASPEED_PTCR_CTRL_PWMG_EN,
		.ctrl_reg = ASPEED_PTCR_CTRL_EXT,
		.type_part1 = ASPEED_PTCR_CTRL_SET_PWMG_TYPE_PART1,
		.type_part2 = ASPEED_PTCR_CTRL_SET_PWMG_TYPE_PART2,
		.type_mask = ASPEED_PTCR_CTRL_SET_PWMG_TYPE_MASK,
		.duty_ctrl_rise_point = DUTY_CTRL_PWM1_RISE_POINT,
		.duty_ctrl_fall_point = DUTY_CTRL_PWM1_FALL_POINT,
		.duty_ctrl_reg = ASPEED_PTCR_DUTY3_CTRL,
		.duty_ctrl_rise_fall_mask = DUTY_CTRL_PWM1_RISE_FALL_MASK,
	},
	[PWMH] = {
		.pwm_en = ASPEED_PTCR_CTRL_PWMH_EN,
		.ctrl_reg = ASPEED_PTCR_CTRL_EXT,
		.type_part1 = ASPEED_PTCR_CTRL_SET_PWMH_TYPE_PART1,
		.type_part2 = ASPEED_PTCR_CTRL_SET_PWMH_TYPE_PART2,
		.type_mask = ASPEED_PTCR_CTRL_SET_PWMH_TYPE_MASK,
		.duty_ctrl_rise_point = DUTY_CTRL_PWM2_RISE_POINT,
		.duty_ctrl_fall_point = DUTY_CTRL_PWM2_FALL_POINT,
		.duty_ctrl_reg = ASPEED_PTCR_DUTY3_CTRL,
		.duty_ctrl_rise_fall_mask = DUTY_CTRL_PWM2_RISE_FALL_MASK,
	}
};

static int regmap_aspeed_pwm_tacho_reg_write(void *context, unsigned int reg,
					     unsigned int val)
{
	void __iomem *regs = (void __iomem *)context;

	writel(val, regs + reg);
	return 0;
}

static int regmap_aspeed_pwm_tacho_reg_read(void *context, unsigned int reg,
					    unsigned int *val)
{
	void __iomem *regs = (void __iomem *)context;

	*val = readl(regs + reg);
	return 0;
}

static const struct regmap_config aspeed_pwm_tacho_regmap_config = {
	.reg_bits = 32,
	.val_bits = 32,
	.reg_stride = 4,
	.max_register = ASPEED_PTCR_TYPEO_LIMIT,
	.reg_write = regmap_aspeed_pwm_tacho_reg_write,
	.reg_read = regmap_aspeed_pwm_tacho_reg_read,
	.fast_io = true,
};

static void aspeed_set_clock_enable(struct regmap *regmap, bool val)
{
	regmap_update_bits(regmap, ASPEED_PTCR_CTRL,
			   ASPEED_PTCR_CTRL_CLK_EN,
			   val ? ASPEED_PTCR_CTRL_CLK_EN : 0);
}

static void aspeed_set_clock_source(struct regmap *regmap, int val)
{
	regmap_update_bits(regmap, ASPEED_PTCR_CTRL,
			   ASPEED_PTCR_CTRL_CLK_SRC,
			   val ? ASPEED_PTCR_CTRL_CLK_SRC : 0);
}

static void aspeed_set_pwm_clock_values(struct regmap *regmap, u8 type,
					u8 div_high, u8 div_low, u8 unit)
{
	u32 reg_value = ((div_high << type_params[type].h_value) |
			 (div_low << type_params[type].l_value) |
			 (unit << type_params[type].unit_value));

	regmap_update_bits(regmap, type_params[type].clk_ctrl_reg,
			   type_params[type].clk_ctrl_mask, reg_value);
}

static void aspeed_set_pwm_port_enable(struct regmap *regmap, u8 pwm_port,
				       bool enable)
{
	regmap_update_bits(regmap, pwm_port_params[pwm_port].ctrl_reg,
			   pwm_port_params[pwm_port].pwm_en,
			   enable ? pwm_port_params[pwm_port].pwm_en : 0);
}

static void aspeed_set_pwm_port_type(struct regmap *regmap,
				     u8 pwm_port, u8 type)
{
	u32 reg_value = (type & 0x1) << pwm_port_params[pwm_port].type_part1;

	reg_value |= (type & 0x2) << pwm_port_params[pwm_port].type_part2;

	regmap_update_bits(regmap, pwm_port_params[pwm_port].ctrl_reg,
			   pwm_port_params[pwm_port].type_mask, reg_value);
}

static void aspeed_set_pwm_port_duty_rising_falling(struct regmap *regmap,
						    u8 pwm_port, u8 rising,
						    u8 falling)
{
	u32 reg_value = (rising <<
			 pwm_port_params[pwm_port].duty_ctrl_rise_point);
	reg_value |= (falling <<
		      pwm_port_params[pwm_port].duty_ctrl_fall_point);

	regmap_update_bits(regmap, pwm_port_params[pwm_port].duty_ctrl_reg,
			   pwm_port_params[pwm_port].duty_ctrl_rise_fall_mask,
			   reg_value);
}

static void aspeed_set_tacho_type_enable(struct regmap *regmap, u8 type,
					 bool enable)
{
	regmap_update_bits(regmap, type_params[type].ctrl_reg,
			   TYPE_CTRL_FAN_TYPE_EN,
			   enable ? TYPE_CTRL_FAN_TYPE_EN : 0);
}

static void aspeed_set_tacho_type_values(struct regmap *regmap, u8 type,
					 u8 mode, u16 unit, u8 division)
{
	u32 reg_value = ((mode << TYPE_CTRL_FAN_MODE) |
			 (unit << TYPE_CTRL_FAN_PERIOD) |
			 (division << TYPE_CTRL_FAN_DIVISION));

	regmap_update_bits(regmap, type_params[type].ctrl_reg,
			   TYPE_CTRL_FAN_MASK, reg_value);
	regmap_update_bits(regmap, type_params[type].ctrl_reg1,
			   TYPE_CTRL_FAN1_MASK, unit << 16);
}

static void aspeed_set_fan_tach_ch_enable(struct regmap *regmap, u8 fan_tach_ch,
					  bool enable)
{
	regmap_update_bits(regmap, ASPEED_PTCR_CTRL,
			   ASPEED_PTCR_CTRL_FAN_NUM_EN(fan_tach_ch),
			   enable ?
			   ASPEED_PTCR_CTRL_FAN_NUM_EN(fan_tach_ch) : 0);
}

static void aspeed_set_fan_tach_ch_source(struct regmap *regmap, u8 fan_tach_ch,
					  u8 fan_tach_ch_source)
{
	u32 reg_value1 = ((fan_tach_ch_source & 0x3) <<
			  TACH_PWM_SOURCE_BIT01(fan_tach_ch));
	u32 reg_value2 = (((fan_tach_ch_source & 0x4) >> 2) <<
			  TACH_PWM_SOURCE_BIT2(fan_tach_ch));

	regmap_update_bits(regmap, ASPEED_PTCR_TACH_SOURCE,
			   TACH_PWM_SOURCE_MASK_BIT01(fan_tach_ch),
			   reg_value1);

	regmap_update_bits(regmap, ASPEED_PTCR_TACH_SOURCE_EXT,
			   TACH_PWM_SOURCE_MASK_BIT2(fan_tach_ch),
			   reg_value2);
}

static void aspeed_set_pwm_port_fan_ctrl(struct aspeed_pwm_tacho_data *priv,
					 u8 index, u8 fan_ctrl)
{
	u16 period, dc_time_on;

	period = priv->type_pwm_clock_unit[priv->pwm_port_type[index]];
	period += 1;
	dc_time_on = (fan_ctrl * period) / PWM_MAX;

	if (dc_time_on == 0) {
		aspeed_set_pwm_port_enable(priv->regmap, index, false);
	} else {
		if (dc_time_on == period)
			dc_time_on = 0;

		aspeed_set_pwm_port_duty_rising_falling(priv->regmap, index, 0,
							dc_time_on);
		aspeed_set_pwm_port_enable(priv->regmap, index, true);
	}
}

static u32 aspeed_get_fan_tach_ch_measure_period(struct aspeed_pwm_tacho_data
						 *priv, u8 type)
{
	u32 clk;
	u16 tacho_unit;
	u8 clk_unit, div_h, div_l, tacho_div;

	clk = priv->clk_freq;
	clk_unit = priv->type_pwm_clock_unit[type];
	div_h = priv->type_pwm_clock_division_h[type];
	div_h = 0x1 << div_h;
	div_l = priv->type_pwm_clock_division_l[type];
	if (div_l == 0)
		div_l = 1;
	else
		div_l = div_l * 2;

	tacho_unit = priv->type_fan_tach_unit[type];
	tacho_div = priv->type_fan_tach_clock_division[type];

	tacho_div = 0x4 << (tacho_div * 2);
	return clk / (clk_unit * div_h * div_l * tacho_div * tacho_unit);
}

static int aspeed_get_fan_tach_ch_rpm(struct aspeed_pwm_tacho_data *priv,
				      u8 fan_tach_ch)
{
	u32 raw_data, tach_div, clk_source, msec, usec, val;
	u8 fan_tach_ch_source, type, mode, both;
	int ret;

	regmap_write(priv->regmap, ASPEED_PTCR_TRIGGER, 0);
	regmap_write(priv->regmap, ASPEED_PTCR_TRIGGER, 0x1 << fan_tach_ch);

	fan_tach_ch_source = priv->fan_tach_ch_source[fan_tach_ch];
	type = priv->pwm_port_type[fan_tach_ch_source];

	msec = (1000 / aspeed_get_fan_tach_ch_measure_period(priv, type));
	usec = msec * 1000;

	ret = regmap_read_poll_timeout(
		priv->regmap,
		ASPEED_PTCR_RESULT,
		val,
		(val & RESULT_STATUS_MASK),
		ASPEED_RPM_STATUS_SLEEP_USEC,
		usec);

	/* return -ETIMEDOUT if we didn't get an answer. */
	if (ret)
		return ret;

	raw_data = val & RESULT_VALUE_MASK;
	tach_div = priv->type_fan_tach_clock_division[type];
	/*
	 * We need the mode to determine if the raw_data is double (from
	 * counting both edges).
	 */
	mode = priv->type_fan_tach_mode[type];
	both = (mode & BOTH_EDGES) ? 1 : 0;

	tach_div = (0x4 << both) << (tach_div * 2);
	clk_source = priv->clk_freq;

	if (raw_data == 0)
		return 0;

	return (clk_source * 60) / (2 * raw_data * tach_div);
}

static ssize_t pwm_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int index = sensor_attr->index;
	int ret;
	struct aspeed_pwm_tacho_data *priv = dev_get_drvdata(dev);
	long fan_ctrl;

	ret = kstrtol(buf, 10, &fan_ctrl);
	if (ret != 0)
		return ret;

	if (fan_ctrl < 0 || fan_ctrl > PWM_MAX)
		return -EINVAL;

	if (priv->pwm_port_fan_ctrl[index] == fan_ctrl)
		return count;

	priv->pwm_port_fan_ctrl[index] = fan_ctrl;
	aspeed_set_pwm_port_fan_ctrl(priv, index, fan_ctrl);

	return count;
}

static ssize_t pwm_show(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int index = sensor_attr->index;
	struct aspeed_pwm_tacho_data *priv = dev_get_drvdata(dev);

	return sprintf(buf, "%u\n", priv->pwm_port_fan_ctrl[index]);
}

static ssize_t rpm_show(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int index = sensor_attr->index;
	int rpm;
	struct aspeed_pwm_tacho_data *priv = dev_get_drvdata(dev);

	rpm = aspeed_get_fan_tach_ch_rpm(priv, index);
	if (rpm < 0)
		return rpm;

	return sprintf(buf, "%d\n", rpm);
}

static umode_t pwm_is_visible(struct kobject *kobj,
			      struct attribute *a, int index)
{
	struct device *dev = kobj_to_dev(kobj);
	struct aspeed_pwm_tacho_data *priv = dev_get_drvdata(dev);

	if (!priv->pwm_present[index])
		return 0;
	return a->mode;
}

static umode_t fan_dev_is_visible(struct kobject *kobj,
				  struct attribute *a, int index)
{
	struct device *dev = kobj_to_dev(kobj);
	struct aspeed_pwm_tacho_data *priv = dev_get_drvdata(dev);

	if (!priv->fan_tach_present[index])
		return 0;
	return a->mode;
}

static SENSOR_DEVICE_ATTR_RW(pwm1, pwm, 0);
static SENSOR_DEVICE_ATTR_RW(pwm2, pwm, 1);
static SENSOR_DEVICE_ATTR_RW(pwm3, pwm, 2);
static SENSOR_DEVICE_ATTR_RW(pwm4, pwm, 3);
static SENSOR_DEVICE_ATTR_RW(pwm5, pwm, 4);
static SENSOR_DEVICE_ATTR_RW(pwm6, pwm, 5);
static SENSOR_DEVICE_ATTR_RW(pwm7, pwm, 6);
static SENSOR_DEVICE_ATTR_RW(pwm8, pwm, 7);
static struct attribute *pwm_dev_attrs[] = {
	&sensor_dev_attr_pwm1.dev_attr.attr,
	&sensor_dev_attr_pwm2.dev_attr.attr,
	&sensor_dev_attr_pwm3.dev_attr.attr,
	&sensor_dev_attr_pwm4.dev_attr.attr,
	&sensor_dev_attr_pwm5.dev_attr.attr,
	&sensor_dev_attr_pwm6.dev_attr.attr,
	&sensor_dev_attr_pwm7.dev_attr.attr,
	&sensor_dev_attr_pwm8.dev_attr.attr,
	NULL,
};

static const struct attribute_group pwm_dev_group = {
	.attrs = pwm_dev_attrs,
	.is_visible = pwm_is_visible,
};

static SENSOR_DEVICE_ATTR_RO(fan1_input, rpm, 0);
static SENSOR_DEVICE_ATTR_RO(fan2_input, rpm, 1);
static SENSOR_DEVICE_ATTR_RO(fan3_input, rpm, 2);
static SENSOR_DEVICE_ATTR_RO(fan4_input, rpm, 3);
static SENSOR_DEVICE_ATTR_RO(fan5_input, rpm, 4);
static SENSOR_DEVICE_ATTR_RO(fan6_input, rpm, 5);
static SENSOR_DEVICE_ATTR_RO(fan7_input, rpm, 6);
static SENSOR_DEVICE_ATTR_RO(fan8_input, rpm, 7);
static SENSOR_DEVICE_ATTR_RO(fan9_input, rpm, 8);
static SENSOR_DEVICE_ATTR_RO(fan10_input, rpm, 9);
static SENSOR_DEVICE_ATTR_RO(fan11_input, rpm, 10);
static SENSOR_DEVICE_ATTR_RO(fan12_input, rpm, 11);
static SENSOR_DEVICE_ATTR_RO(fan13_input, rpm, 12);
static SENSOR_DEVICE_ATTR_RO(fan14_input, rpm, 13);
static SENSOR_DEVICE_ATTR_RO(fan15_input, rpm, 14);
static SENSOR_DEVICE_ATTR_RO(fan16_input, rpm, 15);
static struct attribute *fan_dev_attrs[] = {
	&sensor_dev_attr_fan1_input.dev_attr.attr,
	&sensor_dev_attr_fan2_input.dev_attr.attr,
	&sensor_dev_attr_fan3_input.dev_attr.attr,
	&sensor_dev_attr_fan4_input.dev_attr.attr,
	&sensor_dev_attr_fan5_input.dev_attr.attr,
	&sensor_dev_attr_fan6_input.dev_attr.attr,
	&sensor_dev_attr_fan7_input.dev_attr.attr,
	&sensor_dev_attr_fan8_input.dev_attr.attr,
	&sensor_dev_attr_fan9_input.dev_attr.attr,
	&sensor_dev_attr_fan10_input.dev_attr.attr,
	&sensor_dev_attr_fan11_input.dev_attr.attr,
	&sensor_dev_attr_fan12_input.dev_attr.attr,
	&sensor_dev_attr_fan13_input.dev_attr.attr,
	&sensor_dev_attr_fan14_input.dev_attr.attr,
	&sensor_dev_attr_fan15_input.dev_attr.attr,
	&sensor_dev_attr_fan16_input.dev_attr.attr,
	NULL
};

static const struct attribute_group fan_dev_group = {
	.attrs = fan_dev_attrs,
	.is_visible = fan_dev_is_visible,
};

/*
 * The clock type is type M :
 * The PWM frequency = 24MHz / (type M clock division L bit *
 * type M clock division H bit * (type M PWM period bit + 1))
 */
static void aspeed_create_type(struct aspeed_pwm_tacho_data *priv)
{
	priv->type_pwm_clock_division_h[TYPEM] = M_PWM_DIV_H;
	priv->type_pwm_clock_division_l[TYPEM] = M_PWM_DIV_L;
	priv->type_pwm_clock_unit[TYPEM] = M_PWM_PERIOD;
	aspeed_set_pwm_clock_values(priv->regmap, TYPEM, M_PWM_DIV_H,
				    M_PWM_DIV_L, M_PWM_PERIOD);
	aspeed_set_tacho_type_enable(priv->regmap, TYPEM, true);
	priv->type_fan_tach_clock_division[TYPEM] = M_TACH_CLK_DIV;
	priv->type_fan_tach_unit[TYPEM] = M_TACH_UNIT;
	priv->type_fan_tach_mode[TYPEM] = M_TACH_MODE;
	aspeed_set_tacho_type_values(priv->regmap, TYPEM, M_TACH_MODE,
				     M_TACH_UNIT, M_TACH_CLK_DIV);
}

static void aspeed_create_pwm_port(struct aspeed_pwm_tacho_data *priv,
				   u8 pwm_port)
{
	aspeed_set_pwm_port_enable(priv->regmap, pwm_port, true);
	priv->pwm_present[pwm_port] = true;

	priv->pwm_port_type[pwm_port] = TYPEM;
	aspeed_set_pwm_port_type(priv->regmap, pwm_port, TYPEM);

	priv->pwm_port_fan_ctrl[pwm_port] = INIT_FAN_CTRL;
	aspeed_set_pwm_port_fan_ctrl(priv, pwm_port, INIT_FAN_CTRL);
}

static void aspeed_create_fan_tach_channel(struct aspeed_pwm_tacho_data *priv,
					   u8 *fan_tach_ch,
					   int count,
					   u8 pwm_source)
{
	u8 val, index;

	for (val = 0; val < count; val++) {
		index = fan_tach_ch[val];
		aspeed_set_fan_tach_ch_enable(priv->regmap, index, true);
		priv->fan_tach_present[index] = true;
		priv->fan_tach_ch_source[index] = pwm_source;
		aspeed_set_fan_tach_ch_source(priv->regmap, index, pwm_source);
	}
}

static int
aspeed_pwm_cz_get_max_state(struct thermal_cooling_device *tcdev,
			    unsigned long *state)
{
	struct aspeed_cooling_device *cdev = tcdev->devdata;

	*state = cdev->max_state;

	return 0;
}

static int
aspeed_pwm_cz_get_cur_state(struct thermal_cooling_device *tcdev,
			    unsigned long *state)
{
	struct aspeed_cooling_device *cdev = tcdev->devdata;

	*state = cdev->cur_state;

	return 0;
}

static int
aspeed_pwm_cz_set_cur_state(struct thermal_cooling_device *tcdev,
			    unsigned long state)
{
	struct aspeed_cooling_device *cdev = tcdev->devdata;

	if (state > cdev->max_state)
		return -EINVAL;

	cdev->cur_state = state;
	cdev->priv->pwm_port_fan_ctrl[cdev->pwm_port] =
					cdev->cooling_levels[cdev->cur_state];
	aspeed_set_pwm_port_fan_ctrl(cdev->priv, cdev->pwm_port,
				     cdev->cooling_levels[cdev->cur_state]);

	return 0;
}

static const struct thermal_cooling_device_ops aspeed_pwm_cool_ops = {
	.get_max_state = aspeed_pwm_cz_get_max_state,
	.get_cur_state = aspeed_pwm_cz_get_cur_state,
	.set_cur_state = aspeed_pwm_cz_set_cur_state,
};

static int aspeed_create_pwm_cooling(struct device *dev,
				     struct device_node *child,
				     struct aspeed_pwm_tacho_data *priv,
				     u32 pwm_port, u8 num_levels)
{
	int ret;
	struct aspeed_cooling_device *cdev;

	cdev = devm_kzalloc(dev, sizeof(*cdev), GFP_KERNEL);

	if (!cdev)
		return -ENOMEM;

	cdev->cooling_levels = devm_kzalloc(dev, num_levels, GFP_KERNEL);
	if (!cdev->cooling_levels)
		return -ENOMEM;

	cdev->max_state = num_levels - 1;
	ret = of_property_read_u8_array(child, "cooling-levels",
					cdev->cooling_levels,
					num_levels);
	if (ret) {
		dev_err(dev, "Property 'cooling-levels' cannot be read.\n");
		return ret;
	}
	snprintf(cdev->name, MAX_CDEV_NAME_LEN, "%pOFn%d", child, pwm_port);

	cdev->tcdev = devm_thermal_of_cooling_device_register(dev, child,
					cdev->name, cdev, &aspeed_pwm_cool_ops);
	if (IS_ERR(cdev->tcdev))
		return PTR_ERR(cdev->tcdev);

	cdev->priv = priv;
	cdev->pwm_port = pwm_port;

	priv->cdev[pwm_port] = cdev;

	return 0;
}

static int aspeed_create_fan(struct device *dev,
			     struct device_node *child,
			     struct aspeed_pwm_tacho_data *priv)
{
	u8 *fan_tach_ch;
	u32 pwm_port;
	int ret, count;

	ret = of_property_read_u32(child, "reg", &pwm_port);
	if (ret)
		return ret;
	if (pwm_port >= ARRAY_SIZE(pwm_port_params))
		return -EINVAL;
	aspeed_create_pwm_port(priv, (u8)pwm_port);

	ret = of_property_count_u8_elems(child, "cooling-levels");

	if (ret > 0) {
		ret = aspeed_create_pwm_cooling(dev, child, priv, pwm_port,
						ret);
		if (ret)
			return ret;
	}

	count = of_property_count_u8_elems(child, "aspeed,fan-tach-ch");
	if (count < 1)
		return -EINVAL;
	fan_tach_ch = devm_kcalloc(dev, count, sizeof(*fan_tach_ch),
				   GFP_KERNEL);
	if (!fan_tach_ch)
		return -ENOMEM;
	ret = of_property_read_u8_array(child, "aspeed,fan-tach-ch",
					fan_tach_ch, count);
	if (ret)
		return ret;
	aspeed_create_fan_tach_channel(priv, fan_tach_ch, count, pwm_port);

	return 0;
}

static void aspeed_pwm_tacho_remove(void *data)
{
	struct aspeed_pwm_tacho_data *priv = data;

	reset_control_assert(priv->rst);
}

static int aspeed_pwm_tacho_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np, *child;
	struct aspeed_pwm_tacho_data *priv;
	void __iomem *regs;
	struct device *hwmon;
	struct clk *clk;
	int ret;

	np = dev->of_node;
	regs = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(regs))
		return PTR_ERR(regs);
	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;
	priv->regmap = devm_regmap_init(dev, NULL, (__force void *)regs,
			&aspeed_pwm_tacho_regmap_config);
	if (IS_ERR(priv->regmap))
		return PTR_ERR(priv->regmap);

	priv->rst = devm_reset_control_get_exclusive(dev, NULL);
	if (IS_ERR(priv->rst)) {
		dev_err(dev,
			"missing or invalid reset controller device tree entry");
		return PTR_ERR(priv->rst);
	}
	reset_control_deassert(priv->rst);

	ret = devm_add_action_or_reset(dev, aspeed_pwm_tacho_remove, priv);
	if (ret)
		return ret;

	regmap_write(priv->regmap, ASPEED_PTCR_TACH_SOURCE, 0);
	regmap_write(priv->regmap, ASPEED_PTCR_TACH_SOURCE_EXT, 0);

	clk = devm_clk_get(dev, NULL);
	if (IS_ERR(clk))
		return -ENODEV;
	priv->clk_freq = clk_get_rate(clk);
	aspeed_set_clock_enable(priv->regmap, true);
	aspeed_set_clock_source(priv->regmap, 0);

	aspeed_create_type(priv);

	for_each_child_of_node(np, child) {
		ret = aspeed_create_fan(dev, child, priv);
		if (ret) {
			of_node_put(child);
			return ret;
		}
	}

	priv->groups[0] = &pwm_dev_group;
	priv->groups[1] = &fan_dev_group;
	priv->groups[2] = NULL;
	hwmon = devm_hwmon_device_register_with_groups(dev,
						       "aspeed_pwm_tacho",
						       priv, priv->groups);
	return PTR_ERR_OR_ZERO(hwmon);
}

static const struct of_device_id of_pwm_tacho_match_table[] = {
	{ .compatible = "aspeed,ast2400-pwm-tacho", },
	{ .compatible = "aspeed,ast2500-pwm-tacho", },
	{},
};
MODULE_DEVICE_TABLE(of, of_pwm_tacho_match_table);

static struct platform_driver aspeed_pwm_tacho_driver = {
	.probe		= aspeed_pwm_tacho_probe,
	.driver		= {
		.name	= "aspeed_pwm_tacho",
		.of_match_table = of_pwm_tacho_match_table,
	},
};

module_platform_driver(aspeed_pwm_tacho_driver);

MODULE_AUTHOR("Jaghathiswari Rankappagounder Natarajan <jaghu@google.com>");
MODULE_DESCRIPTION("ASPEED PWM and Fan Tacho device driver");
MODULE_LICENSE("GPL");
