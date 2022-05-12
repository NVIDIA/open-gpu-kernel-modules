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

#ifndef __LINUX_MFD_TPS65086_H
#define __LINUX_MFD_TPS65086_H

#include <linux/device.h>
#include <linux/regmap.h>

/* List of registers for TPS65086 */
#define TPS65086_DEVICEID		0x01
#define TPS65086_IRQ			0x02
#define TPS65086_IRQ_MASK		0x03
#define TPS65086_PMICSTAT		0x04
#define TPS65086_SHUTDNSRC		0x05
#define TPS65086_BUCK1CTRL		0x20
#define TPS65086_BUCK2CTRL		0x21
#define TPS65086_BUCK3DECAY		0x22
#define TPS65086_BUCK3VID		0x23
#define TPS65086_BUCK3SLPCTRL		0x24
#define TPS65086_BUCK4CTRL		0x25
#define TPS65086_BUCK5CTRL		0x26
#define TPS65086_BUCK6CTRL		0x27
#define TPS65086_LDOA2CTRL		0x28
#define TPS65086_LDOA3CTRL		0x29
#define TPS65086_DISCHCTRL1		0x40
#define TPS65086_DISCHCTRL2		0x41
#define TPS65086_DISCHCTRL3		0x42
#define TPS65086_PG_DELAY1		0x43
#define TPS65086_FORCESHUTDN		0x91
#define TPS65086_BUCK1SLPCTRL		0x92
#define TPS65086_BUCK2SLPCTRL		0x93
#define TPS65086_BUCK4VID		0x94
#define TPS65086_BUCK4SLPVID		0x95
#define TPS65086_BUCK5VID		0x96
#define TPS65086_BUCK5SLPVID		0x97
#define TPS65086_BUCK6VID		0x98
#define TPS65086_BUCK6SLPVID		0x99
#define TPS65086_LDOA2VID		0x9A
#define TPS65086_LDOA3VID		0x9B
#define TPS65086_BUCK123CTRL		0x9C
#define TPS65086_PG_DELAY2		0x9D
#define TPS65086_PIN_EN_MASK1		0x9E
#define TPS65086_PIN_EN_MASK2		0x9F
#define TPS65086_SWVTT_EN		0x9F
#define TPS65086_PIN_EN_OVR1		0xA0
#define TPS65086_PIN_EN_OVR2		0xA1
#define TPS65086_GPOCTRL		0xA1
#define TPS65086_PWR_FAULT_MASK1	0xA2
#define TPS65086_PWR_FAULT_MASK2	0xA3
#define TPS65086_GPO1PG_CTRL1		0xA4
#define TPS65086_GPO1PG_CTRL2		0xA5
#define TPS65086_GPO4PG_CTRL1		0xA6
#define TPS65086_GPO4PG_CTRL2		0xA7
#define TPS65086_GPO2PG_CTRL1		0xA8
#define TPS65086_GPO2PG_CTRL2		0xA9
#define TPS65086_GPO3PG_CTRL1		0xAA
#define TPS65086_GPO3PG_CTRL2		0xAB
#define TPS65086_LDOA1CTRL		0xAE
#define TPS65086_PG_STATUS1		0xB0
#define TPS65086_PG_STATUS2		0xB1
#define TPS65086_PWR_FAULT_STATUS1	0xB2
#define TPS65086_PWR_FAULT_STATUS2	0xB3
#define TPS65086_TEMPCRIT		0xB4
#define TPS65086_TEMPHOT		0xB5
#define TPS65086_OC_STATUS		0xB6

/* IRQ Register field definitions */
#define TPS65086_IRQ_DIETEMP_MASK	BIT(0)
#define TPS65086_IRQ_SHUTDN_MASK	BIT(3)
#define TPS65086_IRQ_FAULT_MASK		BIT(7)

/* DEVICEID Register field definitions */
#define TPS65086_DEVICEID_PART_MASK	GENMASK(3, 0)
#define TPS65086_DEVICEID_OTP_MASK	GENMASK(5, 4)
#define TPS65086_DEVICEID_REV_MASK	GENMASK(7, 6)

/* VID Masks */
#define BUCK_VID_MASK			GENMASK(7, 1)
#define VDOA1_VID_MASK			GENMASK(4, 1)
#define VDOA23_VID_MASK			GENMASK(3, 0)

/* Define the TPS65086 IRQ numbers */
enum tps65086_irqs {
	TPS65086_IRQ_DIETEMP,
	TPS65086_IRQ_SHUTDN,
	TPS65086_IRQ_FAULT,
};

/**
 * struct tps65086 - state holder for the tps65086 driver
 *
 * Device data may be used to access the TPS65086 chip
 */
struct tps65086 {
	struct device *dev;
	struct regmap *regmap;

	/* IRQ Data */
	int irq;
	struct regmap_irq_chip_data *irq_data;
};

#endif /* __LINUX_MFD_TPS65086_H */
