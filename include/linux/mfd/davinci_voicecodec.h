/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * DaVinci Voice Codec Core Interface for TI platforms
 *
 * Copyright (C) 2010 Texas Instruments, Inc
 *
 * Author: Miguel Aguilar <miguel.aguilar@ridgerun.com>
 */

#ifndef __LINUX_MFD_DAVINCI_VOICECODEC_H_
#define __LINUX_MFD_DAVINCI_VOICECODEC_H_

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/mfd/core.h>
#include <linux/platform_data/edma.h>

struct regmap;

/*
 * Register values.
 */
#define DAVINCI_VC_PID			0x00
#define DAVINCI_VC_CTRL			0x04
#define DAVINCI_VC_INTEN		0x08
#define DAVINCI_VC_INTSTATUS		0x0c
#define DAVINCI_VC_INTCLR		0x10
#define DAVINCI_VC_EMUL_CTRL		0x14
#define DAVINCI_VC_RFIFO		0x20
#define DAVINCI_VC_WFIFO		0x24
#define DAVINCI_VC_FIFOSTAT		0x28
#define DAVINCI_VC_TST_CTRL		0x2C
#define DAVINCI_VC_REG05		0x94
#define DAVINCI_VC_REG09		0xA4
#define DAVINCI_VC_REG12		0xB0

/* DAVINCI_VC_CTRL bit fields */
#define DAVINCI_VC_CTRL_MASK		0x5500
#define DAVINCI_VC_CTRL_RSTADC		BIT(0)
#define DAVINCI_VC_CTRL_RSTDAC		BIT(1)
#define DAVINCI_VC_CTRL_RD_BITS_8	BIT(4)
#define DAVINCI_VC_CTRL_RD_UNSIGNED	BIT(5)
#define DAVINCI_VC_CTRL_WD_BITS_8	BIT(6)
#define DAVINCI_VC_CTRL_WD_UNSIGNED	BIT(7)
#define DAVINCI_VC_CTRL_RFIFOEN		BIT(8)
#define DAVINCI_VC_CTRL_RFIFOCL		BIT(9)
#define DAVINCI_VC_CTRL_RFIFOMD_WORD_1	BIT(10)
#define DAVINCI_VC_CTRL_WFIFOEN		BIT(12)
#define DAVINCI_VC_CTRL_WFIFOCL		BIT(13)
#define DAVINCI_VC_CTRL_WFIFOMD_WORD_1	BIT(14)

/* DAVINCI_VC_INT bit fields */
#define DAVINCI_VC_INT_MASK		0x3F
#define DAVINCI_VC_INT_RDRDY_MASK	BIT(0)
#define DAVINCI_VC_INT_RERROVF_MASK	BIT(1)
#define DAVINCI_VC_INT_RERRUDR_MASK	BIT(2)
#define DAVINCI_VC_INT_WDREQ_MASK	BIT(3)
#define DAVINCI_VC_INT_WERROVF_MASKBIT	BIT(4)
#define DAVINCI_VC_INT_WERRUDR_MASK	BIT(5)

/* DAVINCI_VC_REG05 bit fields */
#define DAVINCI_VC_REG05_PGA_GAIN	0x07

/* DAVINCI_VC_REG09 bit fields */
#define DAVINCI_VC_REG09_MUTE		0x40
#define DAVINCI_VC_REG09_DIG_ATTEN	0x3F

/* DAVINCI_VC_REG12 bit fields */
#define DAVINCI_VC_REG12_POWER_ALL_ON	0xFD
#define DAVINCI_VC_REG12_POWER_ALL_OFF	0x00

#define DAVINCI_VC_CELLS		2

enum davinci_vc_cells {
	DAVINCI_VC_VCIF_CELL,
	DAVINCI_VC_CQ93VC_CELL,
};

struct davinci_vcif {
	struct platform_device	*pdev;
	u32 dma_tx_channel;
	u32 dma_rx_channel;
	dma_addr_t dma_tx_addr;
	dma_addr_t dma_rx_addr;
};

struct davinci_vc {
	/* Device data */
	struct device *dev;
	struct platform_device *pdev;
	struct clk *clk;

	/* Memory resources */
	void __iomem *base;
	struct regmap *regmap;

	/* MFD cells */
	struct mfd_cell cells[DAVINCI_VC_CELLS];

	/* Client devices */
	struct davinci_vcif davinci_vcif;
};

#endif
