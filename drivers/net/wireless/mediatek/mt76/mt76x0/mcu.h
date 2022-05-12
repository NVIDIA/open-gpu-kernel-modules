/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2014 Felix Fietkau <nbd@openwrt.org>
 * Copyright (C) 2015 Jakub Kicinski <kubakici@wp.pl>
 */

#ifndef __MT76X0U_MCU_H
#define __MT76X0U_MCU_H

#include "../mt76x02_mcu.h"

struct mt76x02_dev;

#define MT_MCU_IVB_SIZE			0x40
#define MT_MCU_DLM_OFFSET		0x80000

/* We use same space for BBP as for MAC regs
 * #define MT_MCU_MEMMAP_BBP		0x40000000
 */
#define MT_MCU_MEMMAP_RF		0x80000000

enum mcu_calibrate {
	MCU_CAL_R = 1,
	MCU_CAL_RXDCOC,
	MCU_CAL_LC,
	MCU_CAL_LOFT,
	MCU_CAL_TXIQ,
	MCU_CAL_BW,
	MCU_CAL_DPD,
	MCU_CAL_RXIQ,
	MCU_CAL_TXDCOC,
	MCU_CAL_RX_GROUP_DELAY,
	MCU_CAL_TX_GROUP_DELAY,
	MCU_CAL_VCO,
	MCU_CAL_NO_SIGNAL = 0xfe,
	MCU_CAL_FULL = 0xff,
};

int mt76x0e_mcu_init(struct mt76x02_dev *dev);
int mt76x0u_mcu_init(struct mt76x02_dev *dev);
static inline int mt76x0_firmware_running(struct mt76x02_dev *dev)
{
	return mt76_rr(dev, MT_MCU_COM_REG0) == 1;
}

#endif
