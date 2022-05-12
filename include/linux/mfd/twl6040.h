/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * MFD driver for twl6040
 *
 * Authors:     Jorge Eduardo Candelaria <jorge.candelaria@ti.com>
 *              Misael Lopez Cruz <misael.lopez@ti.com>
 *
 * Copyright:   (C) 2011 Texas Instruments, Inc.
 */

#ifndef __TWL6040_CODEC_H__
#define __TWL6040_CODEC_H__

#include <linux/interrupt.h>
#include <linux/mfd/core.h>
#include <linux/regulator/consumer.h>
#include <linux/clk.h>

#define TWL6040_REG_ASICID		0x01
#define TWL6040_REG_ASICREV		0x02
#define TWL6040_REG_INTID		0x03
#define TWL6040_REG_INTMR		0x04
#define TWL6040_REG_NCPCTL		0x05
#define TWL6040_REG_LDOCTL		0x06
#define TWL6040_REG_HPPLLCTL		0x07
#define TWL6040_REG_LPPLLCTL		0x08
#define TWL6040_REG_LPPLLDIV		0x09
#define TWL6040_REG_AMICBCTL		0x0A
#define TWL6040_REG_DMICBCTL		0x0B
#define TWL6040_REG_MICLCTL		0x0C
#define TWL6040_REG_MICRCTL		0x0D
#define TWL6040_REG_MICGAIN		0x0E
#define TWL6040_REG_LINEGAIN		0x0F
#define TWL6040_REG_HSLCTL		0x10
#define TWL6040_REG_HSRCTL		0x11
#define TWL6040_REG_HSGAIN		0x12
#define TWL6040_REG_EARCTL		0x13
#define TWL6040_REG_HFLCTL		0x14
#define TWL6040_REG_HFLGAIN		0x15
#define TWL6040_REG_HFRCTL		0x16
#define TWL6040_REG_HFRGAIN		0x17
#define TWL6040_REG_VIBCTLL		0x18
#define TWL6040_REG_VIBDATL		0x19
#define TWL6040_REG_VIBCTLR		0x1A
#define TWL6040_REG_VIBDATR		0x1B
#define TWL6040_REG_HKCTL1		0x1C
#define TWL6040_REG_HKCTL2		0x1D
#define TWL6040_REG_GPOCTL		0x1E
#define TWL6040_REG_ALB			0x1F
#define TWL6040_REG_DLB			0x20
#define TWL6040_REG_TRIM1		0x28
#define TWL6040_REG_TRIM2		0x29
#define TWL6040_REG_TRIM3		0x2A
#define TWL6040_REG_HSOTRIM		0x2B
#define TWL6040_REG_HFOTRIM		0x2C
#define TWL6040_REG_ACCCTL		0x2D
#define TWL6040_REG_STATUS		0x2E

/* INTID (0x03) fields */

#define TWL6040_THINT			0x01
#define TWL6040_PLUGINT			0x02
#define TWL6040_UNPLUGINT		0x04
#define TWL6040_HOOKINT			0x08
#define TWL6040_HFINT			0x10
#define TWL6040_VIBINT			0x20
#define TWL6040_READYINT		0x40

/* INTMR (0x04) fields */

#define TWL6040_THMSK			0x01
#define TWL6040_PLUGMSK			0x02
#define TWL6040_HOOKMSK			0x08
#define TWL6040_HFMSK			0x10
#define TWL6040_VIBMSK			0x20
#define TWL6040_READYMSK		0x40
#define TWL6040_ALLINT_MSK		0x7B

/* NCPCTL (0x05) fields */

#define TWL6040_NCPENA			0x01
#define TWL6040_NCPOPEN			0x40

/* LDOCTL (0x06) fields */

#define TWL6040_LSLDOENA		0x01
#define TWL6040_HSLDOENA		0x04
#define TWL6040_REFENA			0x40
#define TWL6040_OSCENA			0x80

/* HPPLLCTL (0x07) fields */

#define TWL6040_HPLLENA			0x01
#define TWL6040_HPLLRST			0x02
#define TWL6040_HPLLBP			0x04
#define TWL6040_HPLLSQRENA		0x08
#define TWL6040_MCLK_12000KHZ		(0 << 5)
#define TWL6040_MCLK_19200KHZ		(1 << 5)
#define TWL6040_MCLK_26000KHZ		(2 << 5)
#define TWL6040_MCLK_38400KHZ		(3 << 5)
#define TWL6040_MCLK_MSK		0x60

/* LPPLLCTL (0x08) fields */

#define TWL6040_LPLLENA			0x01
#define TWL6040_LPLLRST			0x02
#define TWL6040_LPLLSEL			0x04
#define TWL6040_LPLLFIN			0x08
#define TWL6040_HPLLSEL			0x10

/* HSLCTL/R (0x10/0x11) fields */

#define TWL6040_HSDACENA		(1 << 0)
#define TWL6040_HSDACMODE		(1 << 1)
#define TWL6040_HSDRVENA		(1 << 2)
#define TWL6040_HSDRVMODE		(1 << 3)

/* HFLCTL/R (0x14/0x16) fields */

#define TWL6040_HFDACENA		(1 << 0)
#define TWL6040_HFPGAENA		(1 << 1)
#define TWL6040_HFDRVENA		(1 << 4)
#define TWL6040_HFSWENA			(1 << 6)

/* VIBCTLL/R (0x18/0x1A) fields */

#define TWL6040_VIBENA			(1 << 0)
#define TWL6040_VIBSEL			(1 << 1)
#define TWL6040_VIBCTRL			(1 << 2)
#define TWL6040_VIBCTRL_P		(1 << 3)
#define TWL6040_VIBCTRL_N		(1 << 4)

/* VIBDATL/R (0x19/0x1B) fields */

#define TWL6040_VIBDAT_MAX		0x64

/* GPOCTL (0x1E) fields */

#define TWL6040_GPO1			0x01
#define TWL6040_GPO2			0x02
#define TWL6040_GPO3			0x04

/* ACCCTL (0x2D) fields */

#define TWL6040_I2CSEL			0x01
#define TWL6040_RESETSPLIT		0x04
#define TWL6040_INTCLRMODE		0x08
#define TWL6040_I2CMODE(x)		((x & 0x3) << 4)

/* STATUS (0x2E) fields */

#define TWL6040_PLUGCOMP		0x02
#define TWL6040_VIBLOCDET		0x10
#define TWL6040_VIBROCDET		0x20
#define TWL6040_TSHUTDET                0x40

#define TWL6040_CELLS			4

#define TWL6040_REV_ES1_0		0x00
#define TWL6040_REV_ES1_1		0x01 /* Rev ES1.1 and ES1.2 */
#define TWL6040_REV_ES1_3		0x02
#define TWL6041_REV_ES2_0		0x10

#define TWL6040_IRQ_TH			0
#define TWL6040_IRQ_PLUG		1
#define TWL6040_IRQ_HOOK		2
#define TWL6040_IRQ_HF			3
#define TWL6040_IRQ_VIB			4
#define TWL6040_IRQ_READY		5

/* PLL selection */
#define TWL6040_SYSCLK_SEL_LPPLL	0
#define TWL6040_SYSCLK_SEL_HPPLL	1

#define TWL6040_GPO_MAX	3

/* TODO: All platform data struct can be removed */
struct twl6040_codec_data {
	u16 hs_left_step;
	u16 hs_right_step;
	u16 hf_left_step;
	u16 hf_right_step;
};

struct twl6040_vibra_data {
	unsigned int vibldrv_res;	/* left driver resistance */
	unsigned int vibrdrv_res;	/* right driver resistance */
	unsigned int viblmotor_res;	/* left motor resistance */
	unsigned int vibrmotor_res;	/* right motor resistance */
	int vddvibl_uV;			/* VDDVIBL volt, set 0 for fixed reg */
	int vddvibr_uV;			/* VDDVIBR volt, set 0 for fixed reg */
};

struct twl6040_gpo_data {
	int gpio_base;
};

struct twl6040_platform_data {
	int audpwron_gpio;	/* audio power-on gpio */

	struct twl6040_codec_data *codec;
	struct twl6040_vibra_data *vibra;
	struct twl6040_gpo_data *gpo;
};

struct regmap;
struct regmap_irq_chips_data;

struct twl6040 {
	struct device *dev;
	struct regmap *regmap;
	struct regmap_irq_chip_data *irq_data;
	struct regulator_bulk_data supplies[2]; /* supplies for vio, v2v1 */
	struct clk *clk32k;
	struct clk *mclk;
	struct mutex mutex;
	struct mutex irq_mutex;
	struct mfd_cell cells[TWL6040_CELLS];
	struct completion ready;

	int audpwron;
	int power_count;
	int rev;

	/* PLL configuration */
	int pll;
	unsigned int sysclk_rate;
	unsigned int mclk_rate;

	unsigned int irq;
	unsigned int irq_ready;
	unsigned int irq_th;
};

int twl6040_reg_read(struct twl6040 *twl6040, unsigned int reg);
int twl6040_reg_write(struct twl6040 *twl6040, unsigned int reg,
		      u8 val);
int twl6040_set_bits(struct twl6040 *twl6040, unsigned int reg,
		     u8 mask);
int twl6040_clear_bits(struct twl6040 *twl6040, unsigned int reg,
		       u8 mask);
int twl6040_power(struct twl6040 *twl6040, int on);
int twl6040_set_pll(struct twl6040 *twl6040, int pll_id,
		    unsigned int freq_in, unsigned int freq_out);
int twl6040_get_pll(struct twl6040 *twl6040);
unsigned int twl6040_get_sysclk(struct twl6040 *twl6040);

/* Get the combined status of the vibra control register */
int twl6040_get_vibralr_status(struct twl6040 *twl6040);

static inline int twl6040_get_revid(struct twl6040 *twl6040)
{
	return twl6040->rev;
}


#endif  /* End of __TWL6040_CODEC_H__ */
