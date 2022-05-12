/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * wm8804.h  --  WM8804 S/PDIF transceiver driver
 *
 * Copyright 2010 Wolfson Microelectronics plc
 *
 * Author: Dimitris Papastamos <dp@opensource.wolfsonmicro.com>
 */

#ifndef _WM8804_H
#define _WM8804_H

#include <linux/regmap.h>

/*
 * Register values.
 */
#define WM8804_RST_DEVID1			0x00
#define WM8804_DEVID2				0x01
#define WM8804_DEVREV				0x02
#define WM8804_PLL1				0x03
#define WM8804_PLL2				0x04
#define WM8804_PLL3				0x05
#define WM8804_PLL4				0x06
#define WM8804_PLL5				0x07
#define WM8804_PLL6				0x08
#define WM8804_SPDMODE				0x09
#define WM8804_INTMASK				0x0A
#define WM8804_INTSTAT				0x0B
#define WM8804_SPDSTAT				0x0C
#define WM8804_RXCHAN1				0x0D
#define WM8804_RXCHAN2				0x0E
#define WM8804_RXCHAN3				0x0F
#define WM8804_RXCHAN4				0x10
#define WM8804_RXCHAN5				0x11
#define WM8804_SPDTX1				0x12
#define WM8804_SPDTX2				0x13
#define WM8804_SPDTX3				0x14
#define WM8804_SPDTX4				0x15
#define WM8804_SPDTX5				0x16
#define WM8804_GPO0				0x17
#define WM8804_GPO1				0x18
#define WM8804_GPO2				0x1A
#define WM8804_AIFTX				0x1B
#define WM8804_AIFRX				0x1C
#define WM8804_SPDRX1				0x1D
#define WM8804_PWRDN				0x1E

#define WM8804_REGISTER_COUNT			30
#define WM8804_MAX_REGISTER			0x1E

#define WM8804_TX_CLKSRC_MCLK			1
#define WM8804_TX_CLKSRC_PLL			2

#define WM8804_CLKOUT_SRC_CLK1			3
#define WM8804_CLKOUT_SRC_OSCCLK		4

#define WM8804_CLKOUT_DIV			1
#define WM8804_MCLK_DIV				2

#define WM8804_MCLKDIV_256FS			0
#define WM8804_MCLKDIV_128FS			1

extern const struct regmap_config wm8804_regmap_config;
extern const struct dev_pm_ops wm8804_pm;

int wm8804_probe(struct device *dev, struct regmap *regmap);
void wm8804_remove(struct device *dev);

#endif  /* _WM8804_H */
