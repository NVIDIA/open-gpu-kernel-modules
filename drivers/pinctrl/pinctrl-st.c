// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2013 STMicroelectronics (R&D) Limited.
 * Authors:
 *	Srinivas Kandagatla <srinivas.kandagatla@st.com>
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h> /* of_get_named_gpio() */
#include <linux/of_address.h>
#include <linux/gpio/driver.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/platform_device.h>
#include "core.h"

/* PIO Block registers */
/* PIO output */
#define REG_PIO_POUT			0x00
/* Set bits of POUT */
#define REG_PIO_SET_POUT		0x04
/* Clear bits of POUT */
#define REG_PIO_CLR_POUT		0x08
/* PIO input */
#define REG_PIO_PIN			0x10
/* PIO configuration */
#define REG_PIO_PC(n)			(0x20 + (n) * 0x10)
/* Set bits of PC[2:0] */
#define REG_PIO_SET_PC(n)		(0x24 + (n) * 0x10)
/* Clear bits of PC[2:0] */
#define REG_PIO_CLR_PC(n)		(0x28 + (n) * 0x10)
/* PIO input comparison */
#define REG_PIO_PCOMP			0x50
/* Set bits of PCOMP */
#define REG_PIO_SET_PCOMP		0x54
/* Clear bits of PCOMP */
#define REG_PIO_CLR_PCOMP		0x58
/* PIO input comparison mask */
#define REG_PIO_PMASK			0x60
/* Set bits of PMASK */
#define REG_PIO_SET_PMASK		0x64
/* Clear bits of PMASK */
#define REG_PIO_CLR_PMASK		0x68

#define ST_GPIO_DIRECTION_BIDIR	0x1
#define ST_GPIO_DIRECTION_OUT	0x2
#define ST_GPIO_DIRECTION_IN	0x4

/**
 *  Packed style retime configuration.
 *  There are two registers cfg0 and cfg1 in this style for each bank.
 *  Each field in this register is 8 bit corresponding to 8 pins in the bank.
 */
#define RT_P_CFGS_PER_BANK			2
#define RT_P_CFG0_CLK1NOTCLK0_FIELD(reg)	REG_FIELD(reg, 0, 7)
#define RT_P_CFG0_DELAY_0_FIELD(reg)		REG_FIELD(reg, 16, 23)
#define RT_P_CFG0_DELAY_1_FIELD(reg)		REG_FIELD(reg, 24, 31)
#define RT_P_CFG1_INVERTCLK_FIELD(reg)		REG_FIELD(reg, 0, 7)
#define RT_P_CFG1_RETIME_FIELD(reg)		REG_FIELD(reg, 8, 15)
#define RT_P_CFG1_CLKNOTDATA_FIELD(reg)		REG_FIELD(reg, 16, 23)
#define RT_P_CFG1_DOUBLE_EDGE_FIELD(reg)	REG_FIELD(reg, 24, 31)

/**
 * Dedicated style retime Configuration register
 * each register is dedicated per pin.
 */
#define RT_D_CFGS_PER_BANK		8
#define RT_D_CFG_CLK_SHIFT		0
#define RT_D_CFG_CLK_MASK		(0x3 << 0)
#define RT_D_CFG_CLKNOTDATA_SHIFT	2
#define RT_D_CFG_CLKNOTDATA_MASK	BIT(2)
#define RT_D_CFG_DELAY_SHIFT		3
#define RT_D_CFG_DELAY_MASK		(0xf << 3)
#define RT_D_CFG_DELAY_INNOTOUT_SHIFT	7
#define RT_D_CFG_DELAY_INNOTOUT_MASK	BIT(7)
#define RT_D_CFG_DOUBLE_EDGE_SHIFT	8
#define RT_D_CFG_DOUBLE_EDGE_MASK	BIT(8)
#define RT_D_CFG_INVERTCLK_SHIFT	9
#define RT_D_CFG_INVERTCLK_MASK		BIT(9)
#define RT_D_CFG_RETIME_SHIFT		10
#define RT_D_CFG_RETIME_MASK		BIT(10)

/*
 * Pinconf is represented in an opaque unsigned long variable.
 * Below is the bit allocation details for each possible configuration.
 * All the bit fields can be encapsulated into four variables
 * (direction, retime-type, retime-clk, retime-delay)
 *
 *	 +----------------+
 *[31:28]| reserved-3     |
 *	 +----------------+-------------
 *[27]   |	oe	  |		|
 *	 +----------------+		v
 *[26]   |	pu	  |	[Direction	]
 *	 +----------------+		^
 *[25]   |	od	  |		|
 *	 +----------------+-------------
 *[24]   | reserved-2     |
 *	 +----------------+-------------
 *[23]   |    retime      |		|
 *	 +----------------+		|
 *[22]   | retime-invclk  |		|
 *	 +----------------+		v
 *[21]   |retime-clknotdat|	[Retime-type	]
 *	 +----------------+		^
 *[20]   | retime-de      |		|
 *	 +----------------+-------------
 *[19:18]| retime-clk     |------>[Retime-Clk	]
 *	 +----------------+
 *[17:16]|  reserved-1    |
 *	 +----------------+
 *[15..0]| retime-delay   |------>[Retime Delay]
 *	 +----------------+
 */

#define ST_PINCONF_UNPACK(conf, param)\
				((conf >> ST_PINCONF_ ##param ##_SHIFT) \
				& ST_PINCONF_ ##param ##_MASK)

#define ST_PINCONF_PACK(conf, val, param)	(conf |=\
				((val & ST_PINCONF_ ##param ##_MASK) << \
					ST_PINCONF_ ##param ##_SHIFT))

/* Output enable */
#define ST_PINCONF_OE_MASK		0x1
#define ST_PINCONF_OE_SHIFT		27
#define ST_PINCONF_OE			BIT(27)
#define ST_PINCONF_UNPACK_OE(conf)	ST_PINCONF_UNPACK(conf, OE)
#define ST_PINCONF_PACK_OE(conf)	ST_PINCONF_PACK(conf, 1, OE)

/* Pull Up */
#define ST_PINCONF_PU_MASK		0x1
#define ST_PINCONF_PU_SHIFT		26
#define ST_PINCONF_PU			BIT(26)
#define ST_PINCONF_UNPACK_PU(conf)	ST_PINCONF_UNPACK(conf, PU)
#define ST_PINCONF_PACK_PU(conf)	ST_PINCONF_PACK(conf, 1, PU)

/* Open Drain */
#define ST_PINCONF_OD_MASK		0x1
#define ST_PINCONF_OD_SHIFT		25
#define ST_PINCONF_OD			BIT(25)
#define ST_PINCONF_UNPACK_OD(conf)	ST_PINCONF_UNPACK(conf, OD)
#define ST_PINCONF_PACK_OD(conf)	ST_PINCONF_PACK(conf, 1, OD)

#define ST_PINCONF_RT_MASK		0x1
#define ST_PINCONF_RT_SHIFT		23
#define ST_PINCONF_RT			BIT(23)
#define ST_PINCONF_UNPACK_RT(conf)	ST_PINCONF_UNPACK(conf, RT)
#define ST_PINCONF_PACK_RT(conf)	ST_PINCONF_PACK(conf, 1, RT)

#define ST_PINCONF_RT_INVERTCLK_MASK	0x1
#define ST_PINCONF_RT_INVERTCLK_SHIFT	22
#define ST_PINCONF_RT_INVERTCLK		BIT(22)
#define ST_PINCONF_UNPACK_RT_INVERTCLK(conf) \
			ST_PINCONF_UNPACK(conf, RT_INVERTCLK)
#define ST_PINCONF_PACK_RT_INVERTCLK(conf) \
			ST_PINCONF_PACK(conf, 1, RT_INVERTCLK)

#define ST_PINCONF_RT_CLKNOTDATA_MASK	0x1
#define ST_PINCONF_RT_CLKNOTDATA_SHIFT	21
#define ST_PINCONF_RT_CLKNOTDATA	BIT(21)
#define ST_PINCONF_UNPACK_RT_CLKNOTDATA(conf)	\
				ST_PINCONF_UNPACK(conf, RT_CLKNOTDATA)
#define ST_PINCONF_PACK_RT_CLKNOTDATA(conf) \
				ST_PINCONF_PACK(conf, 1, RT_CLKNOTDATA)

#define ST_PINCONF_RT_DOUBLE_EDGE_MASK	0x1
#define ST_PINCONF_RT_DOUBLE_EDGE_SHIFT	20
#define ST_PINCONF_RT_DOUBLE_EDGE	BIT(20)
#define ST_PINCONF_UNPACK_RT_DOUBLE_EDGE(conf) \
				ST_PINCONF_UNPACK(conf, RT_DOUBLE_EDGE)
#define ST_PINCONF_PACK_RT_DOUBLE_EDGE(conf) \
				ST_PINCONF_PACK(conf, 1, RT_DOUBLE_EDGE)

#define ST_PINCONF_RT_CLK_MASK		0x3
#define ST_PINCONF_RT_CLK_SHIFT		18
#define ST_PINCONF_RT_CLK		BIT(18)
#define ST_PINCONF_UNPACK_RT_CLK(conf)	ST_PINCONF_UNPACK(conf, RT_CLK)
#define ST_PINCONF_PACK_RT_CLK(conf, val) ST_PINCONF_PACK(conf, val, RT_CLK)

/* RETIME_DELAY in Pico Secs */
#define ST_PINCONF_RT_DELAY_MASK	0xffff
#define ST_PINCONF_RT_DELAY_SHIFT	0
#define ST_PINCONF_UNPACK_RT_DELAY(conf) ST_PINCONF_UNPACK(conf, RT_DELAY)
#define ST_PINCONF_PACK_RT_DELAY(conf, val) \
				ST_PINCONF_PACK(conf, val, RT_DELAY)

#define ST_GPIO_PINS_PER_BANK	(8)
#define OF_GPIO_ARGS_MIN	(4)
#define OF_RT_ARGS_MIN		(2)

#define gpio_range_to_bank(chip) \
		container_of(chip, struct st_gpio_bank, range)

#define pc_to_bank(pc) \
		container_of(pc, struct st_gpio_bank, pc)

enum st_retime_style {
	st_retime_style_none,
	st_retime_style_packed,
	st_retime_style_dedicated,
};

struct st_retime_dedicated {
	struct regmap_field *rt[ST_GPIO_PINS_PER_BANK];
};

struct st_retime_packed {
	struct regmap_field *clk1notclk0;
	struct regmap_field *delay_0;
	struct regmap_field *delay_1;
	struct regmap_field *invertclk;
	struct regmap_field *retime;
	struct regmap_field *clknotdata;
	struct regmap_field *double_edge;
};

struct st_pio_control {
	u32 rt_pin_mask;
	struct regmap_field *alt, *oe, *pu, *od;
	/* retiming */
	union {
		struct st_retime_packed		rt_p;
		struct st_retime_dedicated	rt_d;
	} rt;
};

struct st_pctl_data {
	const enum st_retime_style	rt_style;
	const unsigned int		*input_delays;
	const int			ninput_delays;
	const unsigned int		*output_delays;
	const int			noutput_delays;
	/* register offset information */
	const int alt, oe, pu, od, rt;
};

struct st_pinconf {
	int		pin;
	const char	*name;
	unsigned long	config;
	int		altfunc;
};

struct st_pmx_func {
	const char	*name;
	const char	**groups;
	unsigned	ngroups;
};

struct st_pctl_group {
	const char		*name;
	unsigned int		*pins;
	unsigned		npins;
	struct st_pinconf	*pin_conf;
};

/*
 * Edge triggers are not supported at hardware level, it is supported by
 * software by exploiting the level trigger support in hardware.
 * Software uses a virtual register (EDGE_CONF) for edge trigger configuration
 * of each gpio pin in a GPIO bank.
 *
 * Each bank has a 32 bit EDGE_CONF register which is divided in to 8 parts of
 * 4-bits. Each 4-bit space is allocated for each pin in a gpio bank.
 *
 * bit allocation per pin is:
 * Bits:  [0 - 3] | [4 - 7]  [8 - 11] ... ... ... ...  [ 28 - 31]
 *       --------------------------------------------------------
 *       |  pin-0  |  pin-2 | pin-3  | ... ... ... ... | pin -7 |
 *       --------------------------------------------------------
 *
 *  A pin can have one of following the values in its edge configuration field.
 *
 *	-------   ----------------------------
 *	[0-3]	- Description
 *	-------   ----------------------------
 *	0000	- No edge IRQ.
 *	0001	- Falling edge IRQ.
 *	0010	- Rising edge IRQ.
 *	0011	- Rising and Falling edge IRQ.
 *	-------   ----------------------------
 */

#define ST_IRQ_EDGE_CONF_BITS_PER_PIN	4
#define ST_IRQ_EDGE_MASK		0xf
#define ST_IRQ_EDGE_FALLING		BIT(0)
#define ST_IRQ_EDGE_RISING		BIT(1)
#define ST_IRQ_EDGE_BOTH		(BIT(0) | BIT(1))

#define ST_IRQ_RISING_EDGE_CONF(pin) \
	(ST_IRQ_EDGE_RISING << (pin * ST_IRQ_EDGE_CONF_BITS_PER_PIN))

#define ST_IRQ_FALLING_EDGE_CONF(pin) \
	(ST_IRQ_EDGE_FALLING << (pin * ST_IRQ_EDGE_CONF_BITS_PER_PIN))

#define ST_IRQ_BOTH_EDGE_CONF(pin) \
	(ST_IRQ_EDGE_BOTH << (pin * ST_IRQ_EDGE_CONF_BITS_PER_PIN))

#define ST_IRQ_EDGE_CONF(conf, pin) \
	(conf >> (pin * ST_IRQ_EDGE_CONF_BITS_PER_PIN) & ST_IRQ_EDGE_MASK)

struct st_gpio_bank {
	struct gpio_chip		gpio_chip;
	struct pinctrl_gpio_range	range;
	void __iomem			*base;
	struct st_pio_control		pc;
	unsigned long			irq_edge_conf;
	spinlock_t                      lock;
};

struct st_pinctrl {
	struct device			*dev;
	struct pinctrl_dev		*pctl;
	struct st_gpio_bank		*banks;
	int				nbanks;
	struct st_pmx_func		*functions;
	int				nfunctions;
	struct st_pctl_group		*groups;
	int				ngroups;
	struct regmap			*regmap;
	const struct st_pctl_data	*data;
	void __iomem			*irqmux_base;
};

/* SOC specific data */

static const unsigned int stih407_delays[] = {0, 300, 500, 750, 1000, 1250,
			1500, 1750, 2000, 2250, 2500, 2750, 3000, 3250 };

static const struct st_pctl_data  stih407_data = {
	.rt_style       = st_retime_style_dedicated,
	.input_delays   = stih407_delays,
	.ninput_delays  = ARRAY_SIZE(stih407_delays),
	.output_delays  = stih407_delays,
	.noutput_delays = ARRAY_SIZE(stih407_delays),
	.alt = 0, .oe = 40, .pu = 50, .od = 60, .rt = 100,
};

static const struct st_pctl_data stih407_flashdata = {
	.rt_style	= st_retime_style_none,
	.input_delays	= stih407_delays,
	.ninput_delays	= ARRAY_SIZE(stih407_delays),
	.output_delays	= stih407_delays,
	.noutput_delays = ARRAY_SIZE(stih407_delays),
	.alt = 0,
	.oe = -1, /* Not Available */
	.pu = -1, /* Not Available */
	.od = 60,
	.rt = 100,
};

static struct st_pio_control *st_get_pio_control(
			struct pinctrl_dev *pctldev, int pin)
{
	struct pinctrl_gpio_range *range =
			 pinctrl_find_gpio_range_from_pin(pctldev, pin);
	struct st_gpio_bank *bank = gpio_range_to_bank(range);

	return &bank->pc;
}

/* Low level functions.. */
static inline int st_gpio_bank(int gpio)
{
	return gpio/ST_GPIO_PINS_PER_BANK;
}

static inline int st_gpio_pin(int gpio)
{
	return gpio%ST_GPIO_PINS_PER_BANK;
}

static void st_pinconf_set_config(struct st_pio_control *pc,
				int pin, unsigned long config)
{
	struct regmap_field *output_enable = pc->oe;
	struct regmap_field *pull_up = pc->pu;
	struct regmap_field *open_drain = pc->od;
	unsigned int oe_value, pu_value, od_value;
	unsigned long mask = BIT(pin);

	if (output_enable) {
		regmap_field_read(output_enable, &oe_value);
		oe_value &= ~mask;
		if (config & ST_PINCONF_OE)
			oe_value |= mask;
		regmap_field_write(output_enable, oe_value);
	}

	if (pull_up) {
		regmap_field_read(pull_up, &pu_value);
		pu_value &= ~mask;
		if (config & ST_PINCONF_PU)
			pu_value |= mask;
		regmap_field_write(pull_up, pu_value);
	}

	if (open_drain) {
		regmap_field_read(open_drain, &od_value);
		od_value &= ~mask;
		if (config & ST_PINCONF_OD)
			od_value |= mask;
		regmap_field_write(open_drain, od_value);
	}
}

static void st_pctl_set_function(struct st_pio_control *pc,
				int pin_id, int function)
{
	struct regmap_field *alt = pc->alt;
	unsigned int val;
	int pin = st_gpio_pin(pin_id);
	int offset = pin * 4;

	if (!alt)
		return;

	regmap_field_read(alt, &val);
	val &= ~(0xf << offset);
	val |= function << offset;
	regmap_field_write(alt, val);
}

static unsigned int st_pctl_get_pin_function(struct st_pio_control *pc, int pin)
{
	struct regmap_field *alt = pc->alt;
	unsigned int val;
	int offset = pin * 4;

	if (!alt)
		return 0;

	regmap_field_read(alt, &val);

	return (val >> offset) & 0xf;
}

static unsigned long st_pinconf_delay_to_bit(unsigned int delay,
	const struct st_pctl_data *data, unsigned long config)
{
	const unsigned int *delay_times;
	int num_delay_times, i, closest_index = -1;
	unsigned int closest_divergence = UINT_MAX;

	if (ST_PINCONF_UNPACK_OE(config)) {
		delay_times = data->output_delays;
		num_delay_times = data->noutput_delays;
	} else {
		delay_times = data->input_delays;
		num_delay_times = data->ninput_delays;
	}

	for (i = 0; i < num_delay_times; i++) {
		unsigned int divergence = abs(delay - delay_times[i]);

		if (divergence == 0)
			return i;

		if (divergence < closest_divergence) {
			closest_divergence = divergence;
			closest_index = i;
		}
	}

	pr_warn("Attempt to set delay %d, closest available %d\n",
	     delay, delay_times[closest_index]);

	return closest_index;
}

static unsigned long st_pinconf_bit_to_delay(unsigned int index,
	const struct st_pctl_data *data, unsigned long output)
{
	const unsigned int *delay_times;
	int num_delay_times;

	if (output) {
		delay_times = data->output_delays;
		num_delay_times = data->noutput_delays;
	} else {
		delay_times = data->input_delays;
		num_delay_times = data->ninput_delays;
	}

	if (index < num_delay_times) {
		return delay_times[index];
	} else {
		pr_warn("Delay not found in/out delay list\n");
		return 0;
	}
}

static void st_regmap_field_bit_set_clear_pin(struct regmap_field *field,
	int enable, int pin)
{
	unsigned int val = 0;

	regmap_field_read(field, &val);
	if (enable)
		val |= BIT(pin);
	else
		val &= ~BIT(pin);
	regmap_field_write(field, val);
}

static void st_pinconf_set_retime_packed(struct st_pinctrl *info,
	struct st_pio_control *pc,	unsigned long config, int pin)
{
	const struct st_pctl_data *data = info->data;
	struct st_retime_packed *rt_p = &pc->rt.rt_p;
	unsigned int delay;

	st_regmap_field_bit_set_clear_pin(rt_p->clk1notclk0,
				ST_PINCONF_UNPACK_RT_CLK(config), pin);

	st_regmap_field_bit_set_clear_pin(rt_p->clknotdata,
				ST_PINCONF_UNPACK_RT_CLKNOTDATA(config), pin);

	st_regmap_field_bit_set_clear_pin(rt_p->double_edge,
				ST_PINCONF_UNPACK_RT_DOUBLE_EDGE(config), pin);

	st_regmap_field_bit_set_clear_pin(rt_p->invertclk,
				ST_PINCONF_UNPACK_RT_INVERTCLK(config), pin);

	st_regmap_field_bit_set_clear_pin(rt_p->retime,
				ST_PINCONF_UNPACK_RT(config), pin);

	delay = st_pinconf_delay_to_bit(ST_PINCONF_UNPACK_RT_DELAY(config),
					data, config);
	/* 2 bit delay, lsb */
	st_regmap_field_bit_set_clear_pin(rt_p->delay_0, delay & 0x1, pin);
	/* 2 bit delay, msb */
	st_regmap_field_bit_set_clear_pin(rt_p->delay_1, delay & 0x2, pin);
}

static void st_pinconf_set_retime_dedicated(struct st_pinctrl *info,
	struct st_pio_control *pc, unsigned long config, int pin)
{
	int input	= ST_PINCONF_UNPACK_OE(config) ? 0 : 1;
	int clk		= ST_PINCONF_UNPACK_RT_CLK(config);
	int clknotdata	= ST_PINCONF_UNPACK_RT_CLKNOTDATA(config);
	int double_edge	= ST_PINCONF_UNPACK_RT_DOUBLE_EDGE(config);
	int invertclk	= ST_PINCONF_UNPACK_RT_INVERTCLK(config);
	int retime	= ST_PINCONF_UNPACK_RT(config);

	unsigned long delay = st_pinconf_delay_to_bit(
			ST_PINCONF_UNPACK_RT_DELAY(config),
			info->data, config);
	struct st_retime_dedicated *rt_d = &pc->rt.rt_d;

	unsigned long retime_config =
		((clk) << RT_D_CFG_CLK_SHIFT) |
		((delay) << RT_D_CFG_DELAY_SHIFT) |
		((input) << RT_D_CFG_DELAY_INNOTOUT_SHIFT) |
		((retime) << RT_D_CFG_RETIME_SHIFT) |
		((clknotdata) << RT_D_CFG_CLKNOTDATA_SHIFT) |
		((invertclk) << RT_D_CFG_INVERTCLK_SHIFT) |
		((double_edge) << RT_D_CFG_DOUBLE_EDGE_SHIFT);

	regmap_field_write(rt_d->rt[pin], retime_config);
}

static void st_pinconf_get_direction(struct st_pio_control *pc,
	int pin, unsigned long *config)
{
	unsigned int oe_value, pu_value, od_value;

	if (pc->oe) {
		regmap_field_read(pc->oe, &oe_value);
		if (oe_value & BIT(pin))
			ST_PINCONF_PACK_OE(*config);
	}

	if (pc->pu) {
		regmap_field_read(pc->pu, &pu_value);
		if (pu_value & BIT(pin))
			ST_PINCONF_PACK_PU(*config);
	}

	if (pc->od) {
		regmap_field_read(pc->od, &od_value);
		if (od_value & BIT(pin))
			ST_PINCONF_PACK_OD(*config);
	}
}

static int st_pinconf_get_retime_packed(struct st_pinctrl *info,
	struct st_pio_control *pc,	int pin, unsigned long *config)
{
	const struct st_pctl_data *data = info->data;
	struct st_retime_packed *rt_p = &pc->rt.rt_p;
	unsigned int delay_bits, delay, delay0, delay1, val;
	int output = ST_PINCONF_UNPACK_OE(*config);

	if (!regmap_field_read(rt_p->retime, &val) && (val & BIT(pin)))
		ST_PINCONF_PACK_RT(*config);

	if (!regmap_field_read(rt_p->clk1notclk0, &val) && (val & BIT(pin)))
		ST_PINCONF_PACK_RT_CLK(*config, 1);

	if (!regmap_field_read(rt_p->clknotdata, &val) && (val & BIT(pin)))
		ST_PINCONF_PACK_RT_CLKNOTDATA(*config);

	if (!regmap_field_read(rt_p->double_edge, &val) && (val & BIT(pin)))
		ST_PINCONF_PACK_RT_DOUBLE_EDGE(*config);

	if (!regmap_field_read(rt_p->invertclk, &val) && (val & BIT(pin)))
		ST_PINCONF_PACK_RT_INVERTCLK(*config);

	regmap_field_read(rt_p->delay_0, &delay0);
	regmap_field_read(rt_p->delay_1, &delay1);
	delay_bits = (((delay1 & BIT(pin)) ? 1 : 0) << 1) |
			(((delay0 & BIT(pin)) ? 1 : 0));
	delay =  st_pinconf_bit_to_delay(delay_bits, data, output);
	ST_PINCONF_PACK_RT_DELAY(*config, delay);

	return 0;
}

static int st_pinconf_get_retime_dedicated(struct st_pinctrl *info,
	struct st_pio_control *pc,	int pin, unsigned long *config)
{
	unsigned int value;
	unsigned long delay_bits, delay, rt_clk;
	int output = ST_PINCONF_UNPACK_OE(*config);
	struct st_retime_dedicated *rt_d = &pc->rt.rt_d;

	regmap_field_read(rt_d->rt[pin], &value);

	rt_clk = (value & RT_D_CFG_CLK_MASK) >> RT_D_CFG_CLK_SHIFT;
	ST_PINCONF_PACK_RT_CLK(*config, rt_clk);

	delay_bits = (value & RT_D_CFG_DELAY_MASK) >> RT_D_CFG_DELAY_SHIFT;
	delay =  st_pinconf_bit_to_delay(delay_bits, info->data, output);
	ST_PINCONF_PACK_RT_DELAY(*config, delay);

	if (value & RT_D_CFG_CLKNOTDATA_MASK)
		ST_PINCONF_PACK_RT_CLKNOTDATA(*config);

	if (value & RT_D_CFG_DOUBLE_EDGE_MASK)
		ST_PINCONF_PACK_RT_DOUBLE_EDGE(*config);

	if (value & RT_D_CFG_INVERTCLK_MASK)
		ST_PINCONF_PACK_RT_INVERTCLK(*config);

	if (value & RT_D_CFG_RETIME_MASK)
		ST_PINCONF_PACK_RT(*config);

	return 0;
}

/* GPIO related functions */

static inline void __st_gpio_set(struct st_gpio_bank *bank,
	unsigned offset, int value)
{
	if (value)
		writel(BIT(offset), bank->base + REG_PIO_SET_POUT);
	else
		writel(BIT(offset), bank->base + REG_PIO_CLR_POUT);
}

static void st_gpio_direction(struct st_gpio_bank *bank,
		unsigned int gpio, unsigned int direction)
{
	int offset = st_gpio_pin(gpio);
	int i = 0;
	/**
	 * There are three configuration registers (PIOn_PC0, PIOn_PC1
	 * and PIOn_PC2) for each port. These are used to configure the
	 * PIO port pins. Each pin can be configured as an input, output,
	 * bidirectional, or alternative function pin. Three bits, one bit
	 * from each of the three registers, configure the corresponding bit of
	 * the port. Valid bit settings is:
	 *
	 * PC2		PC1		PC0	Direction.
	 * 0		0		0	[Input Weak pull-up]
	 * 0		0 or 1		1	[Bidirection]
	 * 0		1		0	[Output]
	 * 1		0		0	[Input]
	 *
	 * PIOn_SET_PC and PIOn_CLR_PC registers are used to set and clear bits
	 * individually.
	 */
	for (i = 0; i <= 2; i++) {
		if (direction & BIT(i))
			writel(BIT(offset), bank->base + REG_PIO_SET_PC(i));
		else
			writel(BIT(offset), bank->base + REG_PIO_CLR_PC(i));
	}
}

static int st_gpio_get(struct gpio_chip *chip, unsigned offset)
{
	struct st_gpio_bank *bank = gpiochip_get_data(chip);

	return !!(readl(bank->base + REG_PIO_PIN) & BIT(offset));
}

static void st_gpio_set(struct gpio_chip *chip, unsigned offset, int value)
{
	struct st_gpio_bank *bank = gpiochip_get_data(chip);
	__st_gpio_set(bank, offset, value);
}

static int st_gpio_direction_input(struct gpio_chip *chip, unsigned offset)
{
	pinctrl_gpio_direction_input(chip->base + offset);

	return 0;
}

static int st_gpio_direction_output(struct gpio_chip *chip,
	unsigned offset, int value)
{
	struct st_gpio_bank *bank = gpiochip_get_data(chip);

	__st_gpio_set(bank, offset, value);
	pinctrl_gpio_direction_output(chip->base + offset);

	return 0;
}

static int st_gpio_get_direction(struct gpio_chip *chip, unsigned offset)
{
	struct st_gpio_bank *bank = gpiochip_get_data(chip);
	struct st_pio_control pc = bank->pc;
	unsigned long config;
	unsigned int direction = 0;
	unsigned int function;
	unsigned int value;
	int i = 0;

	/* Alternate function direction is handled by Pinctrl */
	function = st_pctl_get_pin_function(&pc, offset);
	if (function) {
		st_pinconf_get_direction(&pc, offset, &config);
		if (ST_PINCONF_UNPACK_OE(config))
			return GPIO_LINE_DIRECTION_OUT;

		return GPIO_LINE_DIRECTION_IN;
	}

	/*
	 * GPIO direction is handled differently
	 * - See st_gpio_direction() above for an explanation
	 */
	for (i = 0; i <= 2; i++) {
		value = readl(bank->base + REG_PIO_PC(i));
		direction |= ((value >> offset) & 0x1) << i;
	}

	if (direction == ST_GPIO_DIRECTION_IN)
		return GPIO_LINE_DIRECTION_IN;

	return GPIO_LINE_DIRECTION_OUT;
}

/* Pinctrl Groups */
static int st_pctl_get_groups_count(struct pinctrl_dev *pctldev)
{
	struct st_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);

	return info->ngroups;
}

static const char *st_pctl_get_group_name(struct pinctrl_dev *pctldev,
				       unsigned selector)
{
	struct st_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);

	return info->groups[selector].name;
}

static int st_pctl_get_group_pins(struct pinctrl_dev *pctldev,
	unsigned selector, const unsigned **pins, unsigned *npins)
{
	struct st_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);

	if (selector >= info->ngroups)
		return -EINVAL;

	*pins = info->groups[selector].pins;
	*npins = info->groups[selector].npins;

	return 0;
}

static inline const struct st_pctl_group *st_pctl_find_group_by_name(
	const struct st_pinctrl *info, const char *name)
{
	int i;

	for (i = 0; i < info->ngroups; i++) {
		if (!strcmp(info->groups[i].name, name))
			return &info->groups[i];
	}

	return NULL;
}

static int st_pctl_dt_node_to_map(struct pinctrl_dev *pctldev,
	struct device_node *np, struct pinctrl_map **map, unsigned *num_maps)
{
	struct st_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);
	const struct st_pctl_group *grp;
	struct pinctrl_map *new_map;
	struct device_node *parent;
	int map_num, i;

	grp = st_pctl_find_group_by_name(info, np->name);
	if (!grp) {
		dev_err(info->dev, "unable to find group for node %pOFn\n",
			np);
		return -EINVAL;
	}

	map_num = grp->npins + 1;
	new_map = devm_kcalloc(pctldev->dev,
				map_num, sizeof(*new_map), GFP_KERNEL);
	if (!new_map)
		return -ENOMEM;

	parent = of_get_parent(np);
	if (!parent) {
		devm_kfree(pctldev->dev, new_map);
		return -EINVAL;
	}

	*map = new_map;
	*num_maps = map_num;
	new_map[0].type = PIN_MAP_TYPE_MUX_GROUP;
	new_map[0].data.mux.function = parent->name;
	new_map[0].data.mux.group = np->name;
	of_node_put(parent);

	/* create config map per pin */
	new_map++;
	for (i = 0; i < grp->npins; i++) {
		new_map[i].type = PIN_MAP_TYPE_CONFIGS_PIN;
		new_map[i].data.configs.group_or_pin =
				pin_get_name(pctldev, grp->pins[i]);
		new_map[i].data.configs.configs = &grp->pin_conf[i].config;
		new_map[i].data.configs.num_configs = 1;
	}
	dev_info(pctldev->dev, "maps: function %s group %s num %d\n",
		(*map)->data.mux.function, grp->name, map_num);

	return 0;
}

static void st_pctl_dt_free_map(struct pinctrl_dev *pctldev,
			struct pinctrl_map *map, unsigned num_maps)
{
}

static const struct pinctrl_ops st_pctlops = {
	.get_groups_count	= st_pctl_get_groups_count,
	.get_group_pins		= st_pctl_get_group_pins,
	.get_group_name		= st_pctl_get_group_name,
	.dt_node_to_map		= st_pctl_dt_node_to_map,
	.dt_free_map		= st_pctl_dt_free_map,
};

/* Pinmux */
static int st_pmx_get_funcs_count(struct pinctrl_dev *pctldev)
{
	struct st_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);

	return info->nfunctions;
}

static const char *st_pmx_get_fname(struct pinctrl_dev *pctldev,
	unsigned selector)
{
	struct st_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);

	return info->functions[selector].name;
}

static int st_pmx_get_groups(struct pinctrl_dev *pctldev,
	unsigned selector, const char * const **grps, unsigned * const ngrps)
{
	struct st_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);
	*grps = info->functions[selector].groups;
	*ngrps = info->functions[selector].ngroups;

	return 0;
}

static int st_pmx_set_mux(struct pinctrl_dev *pctldev, unsigned fselector,
			unsigned group)
{
	struct st_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);
	struct st_pinconf *conf = info->groups[group].pin_conf;
	struct st_pio_control *pc;
	int i;

	for (i = 0; i < info->groups[group].npins; i++) {
		pc = st_get_pio_control(pctldev, conf[i].pin);
		st_pctl_set_function(pc, conf[i].pin, conf[i].altfunc);
	}

	return 0;
}

static int st_pmx_set_gpio_direction(struct pinctrl_dev *pctldev,
			struct pinctrl_gpio_range *range, unsigned gpio,
			bool input)
{
	struct st_gpio_bank *bank = gpio_range_to_bank(range);
	/*
	 * When a PIO bank is used in its primary function mode (altfunc = 0)
	 * Output Enable (OE), Open Drain(OD), and Pull Up (PU)
	 * for the primary PIO functions are driven by the related PIO block
	 */
	st_pctl_set_function(&bank->pc, gpio, 0);
	st_gpio_direction(bank, gpio, input ?
		ST_GPIO_DIRECTION_IN : ST_GPIO_DIRECTION_OUT);

	return 0;
}

static const struct pinmux_ops st_pmxops = {
	.get_functions_count	= st_pmx_get_funcs_count,
	.get_function_name	= st_pmx_get_fname,
	.get_function_groups	= st_pmx_get_groups,
	.set_mux		= st_pmx_set_mux,
	.gpio_set_direction	= st_pmx_set_gpio_direction,
	.strict			= true,
};

/* Pinconf  */
static void st_pinconf_get_retime(struct st_pinctrl *info,
	struct st_pio_control *pc, int pin, unsigned long *config)
{
	if (info->data->rt_style == st_retime_style_packed)
		st_pinconf_get_retime_packed(info, pc, pin, config);
	else if (info->data->rt_style == st_retime_style_dedicated)
		if ((BIT(pin) & pc->rt_pin_mask))
			st_pinconf_get_retime_dedicated(info, pc,
					pin, config);
}

static void st_pinconf_set_retime(struct st_pinctrl *info,
	struct st_pio_control *pc, int pin, unsigned long config)
{
	if (info->data->rt_style == st_retime_style_packed)
		st_pinconf_set_retime_packed(info, pc, config, pin);
	else if (info->data->rt_style == st_retime_style_dedicated)
		if ((BIT(pin) & pc->rt_pin_mask))
			st_pinconf_set_retime_dedicated(info, pc,
							config, pin);
}

static int st_pinconf_set(struct pinctrl_dev *pctldev, unsigned pin_id,
			unsigned long *configs, unsigned num_configs)
{
	int pin = st_gpio_pin(pin_id);
	struct st_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);
	struct st_pio_control *pc = st_get_pio_control(pctldev, pin_id);
	int i;

	for (i = 0; i < num_configs; i++) {
		st_pinconf_set_config(pc, pin, configs[i]);
		st_pinconf_set_retime(info, pc, pin, configs[i]);
	} /* for each config */

	return 0;
}

static int st_pinconf_get(struct pinctrl_dev *pctldev,
			     unsigned pin_id, unsigned long *config)
{
	int pin = st_gpio_pin(pin_id);
	struct st_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);
	struct st_pio_control *pc = st_get_pio_control(pctldev, pin_id);

	*config = 0;
	st_pinconf_get_direction(pc, pin, config);
	st_pinconf_get_retime(info, pc, pin, config);

	return 0;
}

static void st_pinconf_dbg_show(struct pinctrl_dev *pctldev,
				   struct seq_file *s, unsigned pin_id)
{
	struct st_pio_control *pc;
	unsigned long config;
	unsigned int function;
	int offset = st_gpio_pin(pin_id);
	char f[16];
	int oe;

	mutex_unlock(&pctldev->mutex);
	pc = st_get_pio_control(pctldev, pin_id);
	st_pinconf_get(pctldev, pin_id, &config);
	mutex_lock(&pctldev->mutex);

	function = st_pctl_get_pin_function(pc, offset);
	if (function)
		snprintf(f, 10, "Alt Fn %u", function);
	else
		snprintf(f, 5, "GPIO");

	oe = st_gpio_get_direction(&pc_to_bank(pc)->gpio_chip, offset);
	seq_printf(s, "[OE:%d,PU:%ld,OD:%ld]\t%s\n"
		"\t\t[retime:%ld,invclk:%ld,clknotdat:%ld,"
		"de:%ld,rt-clk:%ld,rt-delay:%ld]",
		(oe == GPIO_LINE_DIRECTION_OUT),
		ST_PINCONF_UNPACK_PU(config),
		ST_PINCONF_UNPACK_OD(config),
		f,
		ST_PINCONF_UNPACK_RT(config),
		ST_PINCONF_UNPACK_RT_INVERTCLK(config),
		ST_PINCONF_UNPACK_RT_CLKNOTDATA(config),
		ST_PINCONF_UNPACK_RT_DOUBLE_EDGE(config),
		ST_PINCONF_UNPACK_RT_CLK(config),
		ST_PINCONF_UNPACK_RT_DELAY(config));
}

static const struct pinconf_ops st_confops = {
	.pin_config_get		= st_pinconf_get,
	.pin_config_set		= st_pinconf_set,
	.pin_config_dbg_show	= st_pinconf_dbg_show,
};

static void st_pctl_dt_child_count(struct st_pinctrl *info,
				     struct device_node *np)
{
	struct device_node *child;
	for_each_child_of_node(np, child) {
		if (of_property_read_bool(child, "gpio-controller")) {
			info->nbanks++;
		} else {
			info->nfunctions++;
			info->ngroups += of_get_child_count(child);
		}
	}
}

static int st_pctl_dt_setup_retime_packed(struct st_pinctrl *info,
	int bank, struct st_pio_control *pc)
{
	struct device *dev = info->dev;
	struct regmap *rm = info->regmap;
	const struct st_pctl_data *data = info->data;
	/* 2 registers per bank */
	int reg = (data->rt + bank * RT_P_CFGS_PER_BANK) * 4;
	struct st_retime_packed *rt_p = &pc->rt.rt_p;
	/* cfg0 */
	struct reg_field clk1notclk0 = RT_P_CFG0_CLK1NOTCLK0_FIELD(reg);
	struct reg_field delay_0 = RT_P_CFG0_DELAY_0_FIELD(reg);
	struct reg_field delay_1 = RT_P_CFG0_DELAY_1_FIELD(reg);
	/* cfg1 */
	struct reg_field invertclk = RT_P_CFG1_INVERTCLK_FIELD(reg + 4);
	struct reg_field retime = RT_P_CFG1_RETIME_FIELD(reg + 4);
	struct reg_field clknotdata = RT_P_CFG1_CLKNOTDATA_FIELD(reg + 4);
	struct reg_field double_edge = RT_P_CFG1_DOUBLE_EDGE_FIELD(reg + 4);

	rt_p->clk1notclk0 = devm_regmap_field_alloc(dev, rm, clk1notclk0);
	rt_p->delay_0	= devm_regmap_field_alloc(dev, rm, delay_0);
	rt_p->delay_1 = devm_regmap_field_alloc(dev, rm, delay_1);
	rt_p->invertclk = devm_regmap_field_alloc(dev, rm, invertclk);
	rt_p->retime = devm_regmap_field_alloc(dev, rm, retime);
	rt_p->clknotdata = devm_regmap_field_alloc(dev, rm, clknotdata);
	rt_p->double_edge = devm_regmap_field_alloc(dev, rm, double_edge);

	if (IS_ERR(rt_p->clk1notclk0) || IS_ERR(rt_p->delay_0) ||
		 IS_ERR(rt_p->delay_1) || IS_ERR(rt_p->invertclk) ||
		 IS_ERR(rt_p->retime) || IS_ERR(rt_p->clknotdata) ||
		 IS_ERR(rt_p->double_edge))
		return -EINVAL;

	return 0;
}

static int st_pctl_dt_setup_retime_dedicated(struct st_pinctrl *info,
	int bank, struct st_pio_control *pc)
{
	struct device *dev = info->dev;
	struct regmap *rm = info->regmap;
	const struct st_pctl_data *data = info->data;
	/* 8 registers per bank */
	int reg_offset = (data->rt + bank * RT_D_CFGS_PER_BANK) * 4;
	struct st_retime_dedicated *rt_d = &pc->rt.rt_d;
	unsigned int j;
	u32 pin_mask = pc->rt_pin_mask;

	for (j = 0; j < RT_D_CFGS_PER_BANK; j++) {
		if (BIT(j) & pin_mask) {
			struct reg_field reg = REG_FIELD(reg_offset, 0, 31);
			rt_d->rt[j] = devm_regmap_field_alloc(dev, rm, reg);
			if (IS_ERR(rt_d->rt[j]))
				return -EINVAL;
			reg_offset += 4;
		}
	}
	return 0;
}

static int st_pctl_dt_setup_retime(struct st_pinctrl *info,
	int bank, struct st_pio_control *pc)
{
	const struct st_pctl_data *data = info->data;
	if (data->rt_style  == st_retime_style_packed)
		return st_pctl_dt_setup_retime_packed(info, bank, pc);
	else if (data->rt_style == st_retime_style_dedicated)
		return st_pctl_dt_setup_retime_dedicated(info, bank, pc);

	return -EINVAL;
}


static struct regmap_field *st_pc_get_value(struct device *dev,
					    struct regmap *regmap, int bank,
					    int data, int lsb, int msb)
{
	struct reg_field reg = REG_FIELD((data + bank) * 4, lsb, msb);

	if (data < 0)
		return NULL;

	return devm_regmap_field_alloc(dev, regmap, reg);
}

static void st_parse_syscfgs(struct st_pinctrl *info, int bank,
			     struct device_node *np)
{
	const struct st_pctl_data *data = info->data;
	/**
	 * For a given shared register like OE/PU/OD, there are 8 bits per bank
	 * 0:7 belongs to bank0, 8:15 belongs to bank1 ...
	 * So each register is shared across 4 banks.
	 */
	int lsb = (bank%4) * ST_GPIO_PINS_PER_BANK;
	int msb = lsb + ST_GPIO_PINS_PER_BANK - 1;
	struct st_pio_control *pc = &info->banks[bank].pc;
	struct device *dev = info->dev;
	struct regmap *regmap  = info->regmap;

	pc->alt = st_pc_get_value(dev, regmap, bank, data->alt, 0, 31);
	pc->oe = st_pc_get_value(dev, regmap, bank/4, data->oe, lsb, msb);
	pc->pu = st_pc_get_value(dev, regmap, bank/4, data->pu, lsb, msb);
	pc->od = st_pc_get_value(dev, regmap, bank/4, data->od, lsb, msb);

	/* retime avaiable for all pins by default */
	pc->rt_pin_mask = 0xff;
	of_property_read_u32(np, "st,retime-pin-mask", &pc->rt_pin_mask);
	st_pctl_dt_setup_retime(info, bank, pc);

	return;
}

/*
 * Each pin is represented in of the below forms.
 * <bank offset mux direction rt_type rt_delay rt_clk>
 */
static int st_pctl_dt_parse_groups(struct device_node *np,
	struct st_pctl_group *grp, struct st_pinctrl *info, int idx)
{
	/* bank pad direction val altfunction */
	const __be32 *list;
	struct property *pp;
	struct st_pinconf *conf;
	struct device_node *pins;
	int i = 0, npins = 0, nr_props, ret = 0;

	pins = of_get_child_by_name(np, "st,pins");
	if (!pins)
		return -ENODATA;

	for_each_property_of_node(pins, pp) {
		/* Skip those we do not want to proceed */
		if (!strcmp(pp->name, "name"))
			continue;

		if (pp->length / sizeof(__be32) >= OF_GPIO_ARGS_MIN) {
			npins++;
		} else {
			pr_warn("Invalid st,pins in %pOFn node\n", np);
			ret = -EINVAL;
			goto out_put_node;
		}
	}

	grp->npins = npins;
	grp->name = np->name;
	grp->pins = devm_kcalloc(info->dev, npins, sizeof(u32), GFP_KERNEL);
	grp->pin_conf = devm_kcalloc(info->dev,
					npins, sizeof(*conf), GFP_KERNEL);

	if (!grp->pins || !grp->pin_conf) {
		ret = -ENOMEM;
		goto out_put_node;
	}

	/* <bank offset mux direction rt_type rt_delay rt_clk> */
	for_each_property_of_node(pins, pp) {
		if (!strcmp(pp->name, "name"))
			continue;
		nr_props = pp->length/sizeof(u32);
		list = pp->value;
		conf = &grp->pin_conf[i];

		/* bank & offset */
		be32_to_cpup(list++);
		be32_to_cpup(list++);
		conf->pin = of_get_named_gpio(pins, pp->name, 0);
		conf->name = pp->name;
		grp->pins[i] = conf->pin;
		/* mux */
		conf->altfunc = be32_to_cpup(list++);
		conf->config = 0;
		/* direction */
		conf->config |= be32_to_cpup(list++);
		/* rt_type rt_delay rt_clk */
		if (nr_props >= OF_GPIO_ARGS_MIN + OF_RT_ARGS_MIN) {
			/* rt_type */
			conf->config |= be32_to_cpup(list++);
			/* rt_delay */
			conf->config |= be32_to_cpup(list++);
			/* rt_clk */
			if (nr_props > OF_GPIO_ARGS_MIN + OF_RT_ARGS_MIN)
				conf->config |= be32_to_cpup(list++);
		}
		i++;
	}

out_put_node:
	of_node_put(pins);

	return ret;
}

static int st_pctl_parse_functions(struct device_node *np,
			struct st_pinctrl *info, u32 index, int *grp_index)
{
	struct device_node *child;
	struct st_pmx_func *func;
	struct st_pctl_group *grp;
	int ret, i;

	func = &info->functions[index];
	func->name = np->name;
	func->ngroups = of_get_child_count(np);
	if (func->ngroups == 0) {
		dev_err(info->dev, "No groups defined\n");
		return -EINVAL;
	}
	func->groups = devm_kcalloc(info->dev,
			func->ngroups, sizeof(char *), GFP_KERNEL);
	if (!func->groups)
		return -ENOMEM;

	i = 0;
	for_each_child_of_node(np, child) {
		func->groups[i] = child->name;
		grp = &info->groups[*grp_index];
		*grp_index += 1;
		ret = st_pctl_dt_parse_groups(child, grp, info, i++);
		if (ret) {
			of_node_put(child);
			return ret;
		}
	}
	dev_info(info->dev, "Function[%d\t name:%s,\tgroups:%d]\n",
				index, func->name, func->ngroups);

	return 0;
}

static void st_gpio_irq_mask(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct st_gpio_bank *bank = gpiochip_get_data(gc);

	writel(BIT(d->hwirq), bank->base + REG_PIO_CLR_PMASK);
}

static void st_gpio_irq_unmask(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct st_gpio_bank *bank = gpiochip_get_data(gc);

	writel(BIT(d->hwirq), bank->base + REG_PIO_SET_PMASK);
}

static int st_gpio_irq_request_resources(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);

	st_gpio_direction_input(gc, d->hwirq);

	return gpiochip_lock_as_irq(gc, d->hwirq);
}

static void st_gpio_irq_release_resources(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);

	gpiochip_unlock_as_irq(gc, d->hwirq);
}

static int st_gpio_irq_set_type(struct irq_data *d, unsigned type)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct st_gpio_bank *bank = gpiochip_get_data(gc);
	unsigned long flags;
	int comp, pin = d->hwirq;
	u32 val;
	u32 pin_edge_conf = 0;

	switch (type) {
	case IRQ_TYPE_LEVEL_HIGH:
		comp = 0;
		break;
	case IRQ_TYPE_EDGE_FALLING:
		comp = 0;
		pin_edge_conf = ST_IRQ_FALLING_EDGE_CONF(pin);
		break;
	case IRQ_TYPE_LEVEL_LOW:
		comp = 1;
		break;
	case IRQ_TYPE_EDGE_RISING:
		comp = 1;
		pin_edge_conf = ST_IRQ_RISING_EDGE_CONF(pin);
		break;
	case IRQ_TYPE_EDGE_BOTH:
		comp = st_gpio_get(&bank->gpio_chip, pin);
		pin_edge_conf = ST_IRQ_BOTH_EDGE_CONF(pin);
		break;
	default:
		return -EINVAL;
	}

	spin_lock_irqsave(&bank->lock, flags);
	bank->irq_edge_conf &=  ~(ST_IRQ_EDGE_MASK << (
				pin * ST_IRQ_EDGE_CONF_BITS_PER_PIN));
	bank->irq_edge_conf |= pin_edge_conf;
	spin_unlock_irqrestore(&bank->lock, flags);

	val = readl(bank->base + REG_PIO_PCOMP);
	val &= ~BIT(pin);
	val |= (comp << pin);
	writel(val, bank->base + REG_PIO_PCOMP);

	return 0;
}

/*
 * As edge triggers are not supported at hardware level, it is supported by
 * software by exploiting the level trigger support in hardware.
 *
 * Steps for detection raising edge interrupt in software.
 *
 * Step 1: CONFIGURE pin to detect level LOW interrupts.
 *
 * Step 2: DETECT level LOW interrupt and in irqmux/gpio bank interrupt handler,
 * if the value of pin is low, then CONFIGURE pin for level HIGH interrupt.
 * IGNORE calling the actual interrupt handler for the pin at this stage.
 *
 * Step 3: DETECT level HIGH interrupt and in irqmux/gpio-bank interrupt handler
 * if the value of pin is HIGH, CONFIGURE pin for level LOW interrupt and then
 * DISPATCH the interrupt to the interrupt handler of the pin.
 *
 *		 step-1  ________     __________
 *				|     | step - 3
 *			        |     |
 *			step -2 |_____|
 *
 * falling edge is also detected int the same way.
 *
 */
static void __gpio_irq_handler(struct st_gpio_bank *bank)
{
	unsigned long port_in, port_mask, port_comp, active_irqs;
	unsigned long bank_edge_mask, flags;
	int n, val, ecfg;

	spin_lock_irqsave(&bank->lock, flags);
	bank_edge_mask = bank->irq_edge_conf;
	spin_unlock_irqrestore(&bank->lock, flags);

	for (;;) {
		port_in = readl(bank->base + REG_PIO_PIN);
		port_comp = readl(bank->base + REG_PIO_PCOMP);
		port_mask = readl(bank->base + REG_PIO_PMASK);

		active_irqs = (port_in ^ port_comp) & port_mask;

		if (active_irqs == 0)
			break;

		for_each_set_bit(n, &active_irqs, BITS_PER_LONG) {
			/* check if we are detecting fake edges ... */
			ecfg = ST_IRQ_EDGE_CONF(bank_edge_mask, n);

			if (ecfg) {
				/* edge detection. */
				val = st_gpio_get(&bank->gpio_chip, n);

				writel(BIT(n),
					val ? bank->base + REG_PIO_SET_PCOMP :
					bank->base + REG_PIO_CLR_PCOMP);

				if (ecfg != ST_IRQ_EDGE_BOTH &&
					!((ecfg & ST_IRQ_EDGE_FALLING) ^ val))
					continue;
			}

			generic_handle_irq(irq_find_mapping(bank->gpio_chip.irq.domain, n));
		}
	}
}

static void st_gpio_irq_handler(struct irq_desc *desc)
{
	/* interrupt dedicated per bank */
	struct irq_chip *chip = irq_desc_get_chip(desc);
	struct gpio_chip *gc = irq_desc_get_handler_data(desc);
	struct st_gpio_bank *bank = gpiochip_get_data(gc);

	chained_irq_enter(chip, desc);
	__gpio_irq_handler(bank);
	chained_irq_exit(chip, desc);
}

static void st_gpio_irqmux_handler(struct irq_desc *desc)
{
	struct irq_chip *chip = irq_desc_get_chip(desc);
	struct st_pinctrl *info = irq_desc_get_handler_data(desc);
	unsigned long status;
	int n;

	chained_irq_enter(chip, desc);

	status = readl(info->irqmux_base);

	for_each_set_bit(n, &status, info->nbanks)
		__gpio_irq_handler(&info->banks[n]);

	chained_irq_exit(chip, desc);
}

static const struct gpio_chip st_gpio_template = {
	.request		= gpiochip_generic_request,
	.free			= gpiochip_generic_free,
	.get			= st_gpio_get,
	.set			= st_gpio_set,
	.direction_input	= st_gpio_direction_input,
	.direction_output	= st_gpio_direction_output,
	.get_direction		= st_gpio_get_direction,
	.ngpio			= ST_GPIO_PINS_PER_BANK,
};

static struct irq_chip st_gpio_irqchip = {
	.name			= "GPIO",
	.irq_request_resources	= st_gpio_irq_request_resources,
	.irq_release_resources	= st_gpio_irq_release_resources,
	.irq_disable		= st_gpio_irq_mask,
	.irq_mask		= st_gpio_irq_mask,
	.irq_unmask		= st_gpio_irq_unmask,
	.irq_set_type		= st_gpio_irq_set_type,
	.flags			= IRQCHIP_SKIP_SET_WAKE,
};

static int st_gpiolib_register_bank(struct st_pinctrl *info,
	int bank_nr, struct device_node *np)
{
	struct st_gpio_bank *bank = &info->banks[bank_nr];
	struct pinctrl_gpio_range *range = &bank->range;
	struct device *dev = info->dev;
	int bank_num = of_alias_get_id(np, "gpio");
	struct resource res, irq_res;
	int err;

	if (of_address_to_resource(np, 0, &res))
		return -ENODEV;

	bank->base = devm_ioremap_resource(dev, &res);
	if (IS_ERR(bank->base))
		return PTR_ERR(bank->base);

	bank->gpio_chip = st_gpio_template;
	bank->gpio_chip.base = bank_num * ST_GPIO_PINS_PER_BANK;
	bank->gpio_chip.ngpio = ST_GPIO_PINS_PER_BANK;
	bank->gpio_chip.of_node = np;
	bank->gpio_chip.parent = dev;
	spin_lock_init(&bank->lock);

	of_property_read_string(np, "st,bank-name", &range->name);
	bank->gpio_chip.label = range->name;

	range->id = bank_num;
	range->pin_base = range->base = range->id * ST_GPIO_PINS_PER_BANK;
	range->npins = bank->gpio_chip.ngpio;
	range->gc = &bank->gpio_chip;

	/**
	 * GPIO bank can have one of the two possible types of
	 * interrupt-wirings.
	 *
	 * First type is via irqmux, single interrupt is used by multiple
	 * gpio banks. This reduces number of overall interrupts numbers
	 * required. All these banks belong to a single pincontroller.
	 *		  _________
	 *		 |	   |----> [gpio-bank (n)    ]
	 *		 |	   |----> [gpio-bank (n + 1)]
	 *	[irqN]-- | irq-mux |----> [gpio-bank (n + 2)]
	 *		 |	   |----> [gpio-bank (...  )]
	 *		 |_________|----> [gpio-bank (n + 7)]
	 *
	 * Second type has a dedicated interrupt per each gpio bank.
	 *
	 *	[irqN]----> [gpio-bank (n)]
	 */

	if (of_irq_to_resource(np, 0, &irq_res) > 0) {
		struct gpio_irq_chip *girq;
		int gpio_irq = irq_res.start;

		/* This is not a valid IRQ */
		if (gpio_irq <= 0) {
			dev_err(dev, "invalid IRQ for %pOF bank\n", np);
			goto skip_irq;
		}
		/* We need to have a mux as well */
		if (!info->irqmux_base) {
			dev_err(dev, "no irqmux for %pOF bank\n", np);
			goto skip_irq;
		}

		girq = &bank->gpio_chip.irq;
		girq->chip = &st_gpio_irqchip;
		girq->parent_handler = st_gpio_irq_handler;
		girq->num_parents = 1;
		girq->parents = devm_kcalloc(dev, 1, sizeof(*girq->parents),
					     GFP_KERNEL);
		if (!girq->parents)
			return -ENOMEM;
		girq->parents[0] = gpio_irq;
		girq->default_type = IRQ_TYPE_NONE;
		girq->handler = handle_simple_irq;
	}

skip_irq:
	err  = gpiochip_add_data(&bank->gpio_chip, bank);
	if (err) {
		dev_err(dev, "Failed to add gpiochip(%d)!\n", bank_num);
		return err;
	}
	dev_info(dev, "%s bank added.\n", range->name);

	return 0;
}

static const struct of_device_id st_pctl_of_match[] = {
	{ .compatible = "st,stih407-sbc-pinctrl", .data = &stih407_data},
	{ .compatible = "st,stih407-front-pinctrl", .data = &stih407_data},
	{ .compatible = "st,stih407-rear-pinctrl", .data = &stih407_data},
	{ .compatible = "st,stih407-flash-pinctrl", .data = &stih407_flashdata},
	{ /* sentinel */ }
};

static int st_pctl_probe_dt(struct platform_device *pdev,
	struct pinctrl_desc *pctl_desc, struct st_pinctrl *info)
{
	int ret = 0;
	int i = 0, j = 0, k = 0, bank;
	struct pinctrl_pin_desc *pdesc;
	struct device_node *np = pdev->dev.of_node;
	struct device_node *child;
	int grp_index = 0;
	int irq = 0;
	struct resource *res;

	st_pctl_dt_child_count(info, np);
	if (!info->nbanks) {
		dev_err(&pdev->dev, "you need atleast one gpio bank\n");
		return -EINVAL;
	}

	dev_info(&pdev->dev, "nbanks = %d\n", info->nbanks);
	dev_info(&pdev->dev, "nfunctions = %d\n", info->nfunctions);
	dev_info(&pdev->dev, "ngroups = %d\n", info->ngroups);

	info->functions = devm_kcalloc(&pdev->dev,
		info->nfunctions, sizeof(*info->functions), GFP_KERNEL);

	info->groups = devm_kcalloc(&pdev->dev,
			info->ngroups, sizeof(*info->groups),
			GFP_KERNEL);

	info->banks = devm_kcalloc(&pdev->dev,
			info->nbanks, sizeof(*info->banks), GFP_KERNEL);

	if (!info->functions || !info->groups || !info->banks)
		return -ENOMEM;

	info->regmap = syscon_regmap_lookup_by_phandle(np, "st,syscfg");
	if (IS_ERR(info->regmap)) {
		dev_err(info->dev, "No syscfg phandle specified\n");
		return PTR_ERR(info->regmap);
	}
	info->data = of_match_node(st_pctl_of_match, np)->data;

	irq = platform_get_irq(pdev, 0);

	if (irq > 0) {
		res = platform_get_resource_byname(pdev,
					IORESOURCE_MEM, "irqmux");
		info->irqmux_base = devm_ioremap_resource(&pdev->dev, res);

		if (IS_ERR(info->irqmux_base))
			return PTR_ERR(info->irqmux_base);

		irq_set_chained_handler_and_data(irq, st_gpio_irqmux_handler,
						 info);

	}

	pctl_desc->npins = info->nbanks * ST_GPIO_PINS_PER_BANK;
	pdesc =	devm_kcalloc(&pdev->dev,
			pctl_desc->npins, sizeof(*pdesc), GFP_KERNEL);
	if (!pdesc)
		return -ENOMEM;

	pctl_desc->pins = pdesc;

	bank = 0;
	for_each_child_of_node(np, child) {
		if (of_property_read_bool(child, "gpio-controller")) {
			const char *bank_name = NULL;
			ret = st_gpiolib_register_bank(info, bank, child);
			if (ret) {
				of_node_put(child);
				return ret;
			}

			k = info->banks[bank].range.pin_base;
			bank_name = info->banks[bank].range.name;
			for (j = 0; j < ST_GPIO_PINS_PER_BANK; j++, k++) {
				pdesc->number = k;
				pdesc->name = kasprintf(GFP_KERNEL, "%s[%d]",
							bank_name, j);
				pdesc++;
			}
			st_parse_syscfgs(info, bank, child);
			bank++;
		} else {
			ret = st_pctl_parse_functions(child, info,
							i++, &grp_index);
			if (ret) {
				dev_err(&pdev->dev, "No functions found.\n");
				of_node_put(child);
				return ret;
			}
		}
	}

	return 0;
}

static int st_pctl_probe(struct platform_device *pdev)
{
	struct st_pinctrl *info;
	struct pinctrl_desc *pctl_desc;
	int ret, i;

	if (!pdev->dev.of_node) {
		dev_err(&pdev->dev, "device node not found.\n");
		return -EINVAL;
	}

	pctl_desc = devm_kzalloc(&pdev->dev, sizeof(*pctl_desc), GFP_KERNEL);
	if (!pctl_desc)
		return -ENOMEM;

	info = devm_kzalloc(&pdev->dev, sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->dev = &pdev->dev;
	platform_set_drvdata(pdev, info);
	ret = st_pctl_probe_dt(pdev, pctl_desc, info);
	if (ret)
		return ret;

	pctl_desc->owner	= THIS_MODULE;
	pctl_desc->pctlops	= &st_pctlops;
	pctl_desc->pmxops	= &st_pmxops;
	pctl_desc->confops	= &st_confops;
	pctl_desc->name		= dev_name(&pdev->dev);

	info->pctl = devm_pinctrl_register(&pdev->dev, pctl_desc, info);
	if (IS_ERR(info->pctl)) {
		dev_err(&pdev->dev, "Failed pinctrl registration\n");
		return PTR_ERR(info->pctl);
	}

	for (i = 0; i < info->nbanks; i++)
		pinctrl_add_gpio_range(info->pctl, &info->banks[i].range);

	return 0;
}

static struct platform_driver st_pctl_driver = {
	.driver = {
		.name = "st-pinctrl",
		.of_match_table = st_pctl_of_match,
	},
	.probe = st_pctl_probe,
};

static int __init st_pctl_init(void)
{
	return platform_driver_register(&st_pctl_driver);
}
arch_initcall(st_pctl_init);
