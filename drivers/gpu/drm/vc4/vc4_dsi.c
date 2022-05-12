// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2016 Broadcom
 */

/**
 * DOC: VC4 DSI0/DSI1 module
 *
 * BCM2835 contains two DSI modules, DSI0 and DSI1.  DSI0 is a
 * single-lane DSI controller, while DSI1 is a more modern 4-lane DSI
 * controller.
 *
 * Most Raspberry Pi boards expose DSI1 as their "DISPLAY" connector,
 * while the compute module brings both DSI0 and DSI1 out.
 *
 * This driver has been tested for DSI1 video-mode display only
 * currently, with most of the information necessary for DSI0
 * hopefully present.
 */

#include <linux/clk-provider.h>
#include <linux/clk.h>
#include <linux/completion.h>
#include <linux/component.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/pm_runtime.h>

#include <drm/drm_atomic_helper.h>
#include <drm/drm_bridge.h>
#include <drm/drm_edid.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_of.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_simple_kms_helper.h>

#include "vc4_drv.h"
#include "vc4_regs.h"

#define DSI_CMD_FIFO_DEPTH  16
#define DSI_PIX_FIFO_DEPTH 256
#define DSI_PIX_FIFO_WIDTH   4

#define DSI0_CTRL		0x00

/* Command packet control. */
#define DSI0_TXPKT1C		0x04 /* AKA PKTC */
#define DSI1_TXPKT1C		0x04
# define DSI_TXPKT1C_TRIG_CMD_MASK	VC4_MASK(31, 24)
# define DSI_TXPKT1C_TRIG_CMD_SHIFT	24
# define DSI_TXPKT1C_CMD_REPEAT_MASK	VC4_MASK(23, 10)
# define DSI_TXPKT1C_CMD_REPEAT_SHIFT	10

# define DSI_TXPKT1C_DISPLAY_NO_MASK	VC4_MASK(9, 8)
# define DSI_TXPKT1C_DISPLAY_NO_SHIFT	8
/* Short, trigger, BTA, or a long packet that fits all in CMDFIFO. */
# define DSI_TXPKT1C_DISPLAY_NO_SHORT		0
/* Primary display where cmdfifo provides part of the payload and
 * pixelvalve the rest.
 */
# define DSI_TXPKT1C_DISPLAY_NO_PRIMARY		1
/* Secondary display where cmdfifo provides part of the payload and
 * pixfifo the rest.
 */
# define DSI_TXPKT1C_DISPLAY_NO_SECONDARY	2

# define DSI_TXPKT1C_CMD_TX_TIME_MASK	VC4_MASK(7, 6)
# define DSI_TXPKT1C_CMD_TX_TIME_SHIFT	6

# define DSI_TXPKT1C_CMD_CTRL_MASK	VC4_MASK(5, 4)
# define DSI_TXPKT1C_CMD_CTRL_SHIFT	4
/* Command only.  Uses TXPKT1H and DISPLAY_NO */
# define DSI_TXPKT1C_CMD_CTRL_TX	0
/* Command with BTA for either ack or read data. */
# define DSI_TXPKT1C_CMD_CTRL_RX	1
/* Trigger according to TRIG_CMD */
# define DSI_TXPKT1C_CMD_CTRL_TRIG	2
/* BTA alone for getting error status after a command, or a TE trigger
 * without a previous command.
 */
# define DSI_TXPKT1C_CMD_CTRL_BTA	3

# define DSI_TXPKT1C_CMD_MODE_LP	BIT(3)
# define DSI_TXPKT1C_CMD_TYPE_LONG	BIT(2)
# define DSI_TXPKT1C_CMD_TE_EN		BIT(1)
# define DSI_TXPKT1C_CMD_EN		BIT(0)

/* Command packet header. */
#define DSI0_TXPKT1H		0x08 /* AKA PKTH */
#define DSI1_TXPKT1H		0x08
# define DSI_TXPKT1H_BC_CMDFIFO_MASK	VC4_MASK(31, 24)
# define DSI_TXPKT1H_BC_CMDFIFO_SHIFT	24
# define DSI_TXPKT1H_BC_PARAM_MASK	VC4_MASK(23, 8)
# define DSI_TXPKT1H_BC_PARAM_SHIFT	8
# define DSI_TXPKT1H_BC_DT_MASK		VC4_MASK(7, 0)
# define DSI_TXPKT1H_BC_DT_SHIFT	0

#define DSI0_RXPKT1H		0x0c /* AKA RX1_PKTH */
#define DSI1_RXPKT1H		0x14
# define DSI_RXPKT1H_CRC_ERR		BIT(31)
# define DSI_RXPKT1H_DET_ERR		BIT(30)
# define DSI_RXPKT1H_ECC_ERR		BIT(29)
# define DSI_RXPKT1H_COR_ERR		BIT(28)
# define DSI_RXPKT1H_INCOMP_PKT		BIT(25)
# define DSI_RXPKT1H_PKT_TYPE_LONG	BIT(24)
/* Byte count if DSI_RXPKT1H_PKT_TYPE_LONG */
# define DSI_RXPKT1H_BC_PARAM_MASK	VC4_MASK(23, 8)
# define DSI_RXPKT1H_BC_PARAM_SHIFT	8
/* Short return bytes if !DSI_RXPKT1H_PKT_TYPE_LONG */
# define DSI_RXPKT1H_SHORT_1_MASK	VC4_MASK(23, 16)
# define DSI_RXPKT1H_SHORT_1_SHIFT	16
# define DSI_RXPKT1H_SHORT_0_MASK	VC4_MASK(15, 8)
# define DSI_RXPKT1H_SHORT_0_SHIFT	8
# define DSI_RXPKT1H_DT_LP_CMD_MASK	VC4_MASK(7, 0)
# define DSI_RXPKT1H_DT_LP_CMD_SHIFT	0

#define DSI0_RXPKT2H		0x10 /* AKA RX2_PKTH */
#define DSI1_RXPKT2H		0x18
# define DSI_RXPKT1H_DET_ERR		BIT(30)
# define DSI_RXPKT1H_ECC_ERR		BIT(29)
# define DSI_RXPKT1H_COR_ERR		BIT(28)
# define DSI_RXPKT1H_INCOMP_PKT		BIT(25)
# define DSI_RXPKT1H_BC_PARAM_MASK	VC4_MASK(23, 8)
# define DSI_RXPKT1H_BC_PARAM_SHIFT	8
# define DSI_RXPKT1H_DT_MASK		VC4_MASK(7, 0)
# define DSI_RXPKT1H_DT_SHIFT		0

#define DSI0_TXPKT_CMD_FIFO	0x14 /* AKA CMD_DATAF */
#define DSI1_TXPKT_CMD_FIFO	0x1c

#define DSI0_DISP0_CTRL		0x18
# define DSI_DISP0_PIX_CLK_DIV_MASK	VC4_MASK(21, 13)
# define DSI_DISP0_PIX_CLK_DIV_SHIFT	13
# define DSI_DISP0_LP_STOP_CTRL_MASK	VC4_MASK(12, 11)
# define DSI_DISP0_LP_STOP_CTRL_SHIFT	11
# define DSI_DISP0_LP_STOP_DISABLE	0
# define DSI_DISP0_LP_STOP_PERLINE	1
# define DSI_DISP0_LP_STOP_PERFRAME	2

/* Transmit RGB pixels and null packets only during HACTIVE, instead
 * of going to LP-STOP.
 */
# define DSI_DISP_HACTIVE_NULL		BIT(10)
/* Transmit blanking packet only during vblank, instead of allowing LP-STOP. */
# define DSI_DISP_VBLP_CTRL		BIT(9)
/* Transmit blanking packet only during HFP, instead of allowing LP-STOP. */
# define DSI_DISP_HFP_CTRL		BIT(8)
/* Transmit blanking packet only during HBP, instead of allowing LP-STOP. */
# define DSI_DISP_HBP_CTRL		BIT(7)
# define DSI_DISP0_CHANNEL_MASK		VC4_MASK(6, 5)
# define DSI_DISP0_CHANNEL_SHIFT	5
/* Enables end events for HSYNC/VSYNC, not just start events. */
# define DSI_DISP0_ST_END		BIT(4)
# define DSI_DISP0_PFORMAT_MASK		VC4_MASK(3, 2)
# define DSI_DISP0_PFORMAT_SHIFT	2
# define DSI_PFORMAT_RGB565		0
# define DSI_PFORMAT_RGB666_PACKED	1
# define DSI_PFORMAT_RGB666		2
# define DSI_PFORMAT_RGB888		3
/* Default is VIDEO mode. */
# define DSI_DISP0_COMMAND_MODE		BIT(1)
# define DSI_DISP0_ENABLE		BIT(0)

#define DSI0_DISP1_CTRL		0x1c
#define DSI1_DISP1_CTRL		0x2c
/* Format of the data written to TXPKT_PIX_FIFO. */
# define DSI_DISP1_PFORMAT_MASK		VC4_MASK(2, 1)
# define DSI_DISP1_PFORMAT_SHIFT	1
# define DSI_DISP1_PFORMAT_16BIT	0
# define DSI_DISP1_PFORMAT_24BIT	1
# define DSI_DISP1_PFORMAT_32BIT_LE	2
# define DSI_DISP1_PFORMAT_32BIT_BE	3

/* DISP1 is always command mode. */
# define DSI_DISP1_ENABLE		BIT(0)

#define DSI0_TXPKT_PIX_FIFO		0x20 /* AKA PIX_FIFO */

#define DSI0_INT_STAT		0x24
#define DSI0_INT_EN		0x28
# define DSI1_INT_PHY_D3_ULPS		BIT(30)
# define DSI1_INT_PHY_D3_STOP		BIT(29)
# define DSI1_INT_PHY_D2_ULPS		BIT(28)
# define DSI1_INT_PHY_D2_STOP		BIT(27)
# define DSI1_INT_PHY_D1_ULPS		BIT(26)
# define DSI1_INT_PHY_D1_STOP		BIT(25)
# define DSI1_INT_PHY_D0_ULPS		BIT(24)
# define DSI1_INT_PHY_D0_STOP		BIT(23)
# define DSI1_INT_FIFO_ERR		BIT(22)
# define DSI1_INT_PHY_DIR_RTF		BIT(21)
# define DSI1_INT_PHY_RXLPDT		BIT(20)
# define DSI1_INT_PHY_RXTRIG		BIT(19)
# define DSI1_INT_PHY_D0_LPDT		BIT(18)
# define DSI1_INT_PHY_DIR_FTR		BIT(17)

/* Signaled when the clock lane enters the given state. */
# define DSI1_INT_PHY_CLOCK_ULPS	BIT(16)
# define DSI1_INT_PHY_CLOCK_HS		BIT(15)
# define DSI1_INT_PHY_CLOCK_STOP	BIT(14)

/* Signaled on timeouts */
# define DSI1_INT_PR_TO			BIT(13)
# define DSI1_INT_TA_TO			BIT(12)
# define DSI1_INT_LPRX_TO		BIT(11)
# define DSI1_INT_HSTX_TO		BIT(10)

/* Contention on a line when trying to drive the line low */
# define DSI1_INT_ERR_CONT_LP1		BIT(9)
# define DSI1_INT_ERR_CONT_LP0		BIT(8)

/* Control error: incorrect line state sequence on data lane 0. */
# define DSI1_INT_ERR_CONTROL		BIT(7)
/* LPDT synchronization error (bits received not a multiple of 8. */

# define DSI1_INT_ERR_SYNC_ESC		BIT(6)
/* Signaled after receiving an error packet from the display in
 * response to a read.
 */
# define DSI1_INT_RXPKT2		BIT(5)
/* Signaled after receiving a packet.  The header and optional short
 * response will be in RXPKT1H, and a long response will be in the
 * RXPKT_FIFO.
 */
# define DSI1_INT_RXPKT1		BIT(4)
# define DSI1_INT_TXPKT2_DONE		BIT(3)
# define DSI1_INT_TXPKT2_END		BIT(2)
/* Signaled after all repeats of TXPKT1 are transferred. */
# define DSI1_INT_TXPKT1_DONE		BIT(1)
/* Signaled after each TXPKT1 repeat is scheduled. */
# define DSI1_INT_TXPKT1_END		BIT(0)

#define DSI1_INTERRUPTS_ALWAYS_ENABLED	(DSI1_INT_ERR_SYNC_ESC | \
					 DSI1_INT_ERR_CONTROL |	 \
					 DSI1_INT_ERR_CONT_LP0 | \
					 DSI1_INT_ERR_CONT_LP1 | \
					 DSI1_INT_HSTX_TO |	 \
					 DSI1_INT_LPRX_TO |	 \
					 DSI1_INT_TA_TO |	 \
					 DSI1_INT_PR_TO)

#define DSI0_STAT		0x2c
#define DSI0_HSTX_TO_CNT	0x30
#define DSI0_LPRX_TO_CNT	0x34
#define DSI0_TA_TO_CNT		0x38
#define DSI0_PR_TO_CNT		0x3c
#define DSI0_PHYC		0x40
# define DSI1_PHYC_ESC_CLK_LPDT_MASK	VC4_MASK(25, 20)
# define DSI1_PHYC_ESC_CLK_LPDT_SHIFT	20
# define DSI1_PHYC_HS_CLK_CONTINUOUS	BIT(18)
# define DSI0_PHYC_ESC_CLK_LPDT_MASK	VC4_MASK(17, 12)
# define DSI0_PHYC_ESC_CLK_LPDT_SHIFT	12
# define DSI1_PHYC_CLANE_ULPS		BIT(17)
# define DSI1_PHYC_CLANE_ENABLE		BIT(16)
# define DSI_PHYC_DLANE3_ULPS		BIT(13)
# define DSI_PHYC_DLANE3_ENABLE		BIT(12)
# define DSI0_PHYC_HS_CLK_CONTINUOUS	BIT(10)
# define DSI0_PHYC_CLANE_ULPS		BIT(9)
# define DSI_PHYC_DLANE2_ULPS		BIT(9)
# define DSI0_PHYC_CLANE_ENABLE		BIT(8)
# define DSI_PHYC_DLANE2_ENABLE		BIT(8)
# define DSI_PHYC_DLANE1_ULPS		BIT(5)
# define DSI_PHYC_DLANE1_ENABLE		BIT(4)
# define DSI_PHYC_DLANE0_FORCE_STOP	BIT(2)
# define DSI_PHYC_DLANE0_ULPS		BIT(1)
# define DSI_PHYC_DLANE0_ENABLE		BIT(0)

#define DSI0_HS_CLT0		0x44
#define DSI0_HS_CLT1		0x48
#define DSI0_HS_CLT2		0x4c
#define DSI0_HS_DLT3		0x50
#define DSI0_HS_DLT4		0x54
#define DSI0_HS_DLT5		0x58
#define DSI0_HS_DLT6		0x5c
#define DSI0_HS_DLT7		0x60

#define DSI0_PHY_AFEC0		0x64
# define DSI0_PHY_AFEC0_DDR2CLK_EN		BIT(26)
# define DSI0_PHY_AFEC0_DDRCLK_EN		BIT(25)
# define DSI0_PHY_AFEC0_LATCH_ULPS		BIT(24)
# define DSI1_PHY_AFEC0_IDR_DLANE3_MASK		VC4_MASK(31, 29)
# define DSI1_PHY_AFEC0_IDR_DLANE3_SHIFT	29
# define DSI1_PHY_AFEC0_IDR_DLANE2_MASK		VC4_MASK(28, 26)
# define DSI1_PHY_AFEC0_IDR_DLANE2_SHIFT	26
# define DSI1_PHY_AFEC0_IDR_DLANE1_MASK		VC4_MASK(27, 23)
# define DSI1_PHY_AFEC0_IDR_DLANE1_SHIFT	23
# define DSI1_PHY_AFEC0_IDR_DLANE0_MASK		VC4_MASK(22, 20)
# define DSI1_PHY_AFEC0_IDR_DLANE0_SHIFT	20
# define DSI1_PHY_AFEC0_IDR_CLANE_MASK		VC4_MASK(19, 17)
# define DSI1_PHY_AFEC0_IDR_CLANE_SHIFT		17
# define DSI0_PHY_AFEC0_ACTRL_DLANE1_MASK	VC4_MASK(23, 20)
# define DSI0_PHY_AFEC0_ACTRL_DLANE1_SHIFT	20
# define DSI0_PHY_AFEC0_ACTRL_DLANE0_MASK	VC4_MASK(19, 16)
# define DSI0_PHY_AFEC0_ACTRL_DLANE0_SHIFT	16
# define DSI0_PHY_AFEC0_ACTRL_CLANE_MASK	VC4_MASK(15, 12)
# define DSI0_PHY_AFEC0_ACTRL_CLANE_SHIFT	12
# define DSI1_PHY_AFEC0_DDR2CLK_EN		BIT(16)
# define DSI1_PHY_AFEC0_DDRCLK_EN		BIT(15)
# define DSI1_PHY_AFEC0_LATCH_ULPS		BIT(14)
# define DSI1_PHY_AFEC0_RESET			BIT(13)
# define DSI1_PHY_AFEC0_PD			BIT(12)
# define DSI0_PHY_AFEC0_RESET			BIT(11)
# define DSI1_PHY_AFEC0_PD_BG			BIT(11)
# define DSI0_PHY_AFEC0_PD			BIT(10)
# define DSI1_PHY_AFEC0_PD_DLANE1		BIT(10)
# define DSI0_PHY_AFEC0_PD_BG			BIT(9)
# define DSI1_PHY_AFEC0_PD_DLANE2		BIT(9)
# define DSI0_PHY_AFEC0_PD_DLANE1		BIT(8)
# define DSI1_PHY_AFEC0_PD_DLANE3		BIT(8)
# define DSI_PHY_AFEC0_PTATADJ_MASK		VC4_MASK(7, 4)
# define DSI_PHY_AFEC0_PTATADJ_SHIFT		4
# define DSI_PHY_AFEC0_CTATADJ_MASK		VC4_MASK(3, 0)
# define DSI_PHY_AFEC0_CTATADJ_SHIFT		0

#define DSI0_PHY_AFEC1		0x68
# define DSI0_PHY_AFEC1_IDR_DLANE1_MASK		VC4_MASK(10, 8)
# define DSI0_PHY_AFEC1_IDR_DLANE1_SHIFT	8
# define DSI0_PHY_AFEC1_IDR_DLANE0_MASK		VC4_MASK(6, 4)
# define DSI0_PHY_AFEC1_IDR_DLANE0_SHIFT	4
# define DSI0_PHY_AFEC1_IDR_CLANE_MASK		VC4_MASK(2, 0)
# define DSI0_PHY_AFEC1_IDR_CLANE_SHIFT		0

#define DSI0_TST_SEL		0x6c
#define DSI0_TST_MON		0x70
#define DSI0_ID			0x74
# define DSI_ID_VALUE		0x00647369

#define DSI1_CTRL		0x00
# define DSI_CTRL_HS_CLKC_MASK		VC4_MASK(15, 14)
# define DSI_CTRL_HS_CLKC_SHIFT		14
# define DSI_CTRL_HS_CLKC_BYTE		0
# define DSI_CTRL_HS_CLKC_DDR2		1
# define DSI_CTRL_HS_CLKC_DDR		2

# define DSI_CTRL_RX_LPDT_EOT_DISABLE	BIT(13)
# define DSI_CTRL_LPDT_EOT_DISABLE	BIT(12)
# define DSI_CTRL_HSDT_EOT_DISABLE	BIT(11)
# define DSI_CTRL_SOFT_RESET_CFG	BIT(10)
# define DSI_CTRL_CAL_BYTE		BIT(9)
# define DSI_CTRL_INV_BYTE		BIT(8)
# define DSI_CTRL_CLR_LDF		BIT(7)
# define DSI0_CTRL_CLR_PBCF		BIT(6)
# define DSI1_CTRL_CLR_RXF		BIT(6)
# define DSI0_CTRL_CLR_CPBCF		BIT(5)
# define DSI1_CTRL_CLR_PDF		BIT(5)
# define DSI0_CTRL_CLR_PDF		BIT(4)
# define DSI1_CTRL_CLR_CDF		BIT(4)
# define DSI0_CTRL_CLR_CDF		BIT(3)
# define DSI0_CTRL_CTRL2		BIT(2)
# define DSI1_CTRL_DISABLE_DISP_CRCC	BIT(2)
# define DSI0_CTRL_CTRL1		BIT(1)
# define DSI1_CTRL_DISABLE_DISP_ECCC	BIT(1)
# define DSI0_CTRL_CTRL0		BIT(0)
# define DSI1_CTRL_EN			BIT(0)
# define DSI0_CTRL_RESET_FIFOS		(DSI_CTRL_CLR_LDF | \
					 DSI0_CTRL_CLR_PBCF | \
					 DSI0_CTRL_CLR_CPBCF |	\
					 DSI0_CTRL_CLR_PDF | \
					 DSI0_CTRL_CLR_CDF)
# define DSI1_CTRL_RESET_FIFOS		(DSI_CTRL_CLR_LDF | \
					 DSI1_CTRL_CLR_RXF | \
					 DSI1_CTRL_CLR_PDF | \
					 DSI1_CTRL_CLR_CDF)

#define DSI1_TXPKT2C		0x0c
#define DSI1_TXPKT2H		0x10
#define DSI1_TXPKT_PIX_FIFO	0x20
#define DSI1_RXPKT_FIFO		0x24
#define DSI1_DISP0_CTRL		0x28
#define DSI1_INT_STAT		0x30
#define DSI1_INT_EN		0x34
/* State reporting bits.  These mostly behave like INT_STAT, where
 * writing a 1 clears the bit.
 */
#define DSI1_STAT		0x38
# define DSI1_STAT_PHY_D3_ULPS		BIT(31)
# define DSI1_STAT_PHY_D3_STOP		BIT(30)
# define DSI1_STAT_PHY_D2_ULPS		BIT(29)
# define DSI1_STAT_PHY_D2_STOP		BIT(28)
# define DSI1_STAT_PHY_D1_ULPS		BIT(27)
# define DSI1_STAT_PHY_D1_STOP		BIT(26)
# define DSI1_STAT_PHY_D0_ULPS		BIT(25)
# define DSI1_STAT_PHY_D0_STOP		BIT(24)
# define DSI1_STAT_FIFO_ERR		BIT(23)
# define DSI1_STAT_PHY_RXLPDT		BIT(22)
# define DSI1_STAT_PHY_RXTRIG		BIT(21)
# define DSI1_STAT_PHY_D0_LPDT		BIT(20)
/* Set when in forward direction */
# define DSI1_STAT_PHY_DIR		BIT(19)
# define DSI1_STAT_PHY_CLOCK_ULPS	BIT(18)
# define DSI1_STAT_PHY_CLOCK_HS		BIT(17)
# define DSI1_STAT_PHY_CLOCK_STOP	BIT(16)
# define DSI1_STAT_PR_TO		BIT(15)
# define DSI1_STAT_TA_TO		BIT(14)
# define DSI1_STAT_LPRX_TO		BIT(13)
# define DSI1_STAT_HSTX_TO		BIT(12)
# define DSI1_STAT_ERR_CONT_LP1		BIT(11)
# define DSI1_STAT_ERR_CONT_LP0		BIT(10)
# define DSI1_STAT_ERR_CONTROL		BIT(9)
# define DSI1_STAT_ERR_SYNC_ESC		BIT(8)
# define DSI1_STAT_RXPKT2		BIT(7)
# define DSI1_STAT_RXPKT1		BIT(6)
# define DSI1_STAT_TXPKT2_BUSY		BIT(5)
# define DSI1_STAT_TXPKT2_DONE		BIT(4)
# define DSI1_STAT_TXPKT2_END		BIT(3)
# define DSI1_STAT_TXPKT1_BUSY		BIT(2)
# define DSI1_STAT_TXPKT1_DONE		BIT(1)
# define DSI1_STAT_TXPKT1_END		BIT(0)

#define DSI1_HSTX_TO_CNT	0x3c
#define DSI1_LPRX_TO_CNT	0x40
#define DSI1_TA_TO_CNT		0x44
#define DSI1_PR_TO_CNT		0x48
#define DSI1_PHYC		0x4c

#define DSI1_HS_CLT0		0x50
# define DSI_HS_CLT0_CZERO_MASK		VC4_MASK(26, 18)
# define DSI_HS_CLT0_CZERO_SHIFT	18
# define DSI_HS_CLT0_CPRE_MASK		VC4_MASK(17, 9)
# define DSI_HS_CLT0_CPRE_SHIFT		9
# define DSI_HS_CLT0_CPREP_MASK		VC4_MASK(8, 0)
# define DSI_HS_CLT0_CPREP_SHIFT	0

#define DSI1_HS_CLT1		0x54
# define DSI_HS_CLT1_CTRAIL_MASK	VC4_MASK(17, 9)
# define DSI_HS_CLT1_CTRAIL_SHIFT	9
# define DSI_HS_CLT1_CPOST_MASK		VC4_MASK(8, 0)
# define DSI_HS_CLT1_CPOST_SHIFT	0

#define DSI1_HS_CLT2		0x58
# define DSI_HS_CLT2_WUP_MASK		VC4_MASK(23, 0)
# define DSI_HS_CLT2_WUP_SHIFT		0

#define DSI1_HS_DLT3		0x5c
# define DSI_HS_DLT3_EXIT_MASK		VC4_MASK(26, 18)
# define DSI_HS_DLT3_EXIT_SHIFT		18
# define DSI_HS_DLT3_ZERO_MASK		VC4_MASK(17, 9)
# define DSI_HS_DLT3_ZERO_SHIFT		9
# define DSI_HS_DLT3_PRE_MASK		VC4_MASK(8, 0)
# define DSI_HS_DLT3_PRE_SHIFT		0

#define DSI1_HS_DLT4		0x60
# define DSI_HS_DLT4_ANLAT_MASK		VC4_MASK(22, 18)
# define DSI_HS_DLT4_ANLAT_SHIFT	18
# define DSI_HS_DLT4_TRAIL_MASK		VC4_MASK(17, 9)
# define DSI_HS_DLT4_TRAIL_SHIFT	9
# define DSI_HS_DLT4_LPX_MASK		VC4_MASK(8, 0)
# define DSI_HS_DLT4_LPX_SHIFT		0

#define DSI1_HS_DLT5		0x64
# define DSI_HS_DLT5_INIT_MASK		VC4_MASK(23, 0)
# define DSI_HS_DLT5_INIT_SHIFT		0

#define DSI1_HS_DLT6		0x68
# define DSI_HS_DLT6_TA_GET_MASK	VC4_MASK(31, 24)
# define DSI_HS_DLT6_TA_GET_SHIFT	24
# define DSI_HS_DLT6_TA_SURE_MASK	VC4_MASK(23, 16)
# define DSI_HS_DLT6_TA_SURE_SHIFT	16
# define DSI_HS_DLT6_TA_GO_MASK		VC4_MASK(15, 8)
# define DSI_HS_DLT6_TA_GO_SHIFT	8
# define DSI_HS_DLT6_LP_LPX_MASK	VC4_MASK(7, 0)
# define DSI_HS_DLT6_LP_LPX_SHIFT	0

#define DSI1_HS_DLT7		0x6c
# define DSI_HS_DLT7_LP_WUP_MASK	VC4_MASK(23, 0)
# define DSI_HS_DLT7_LP_WUP_SHIFT	0

#define DSI1_PHY_AFEC0		0x70

#define DSI1_PHY_AFEC1		0x74
# define DSI1_PHY_AFEC1_ACTRL_DLANE3_MASK	VC4_MASK(19, 16)
# define DSI1_PHY_AFEC1_ACTRL_DLANE3_SHIFT	16
# define DSI1_PHY_AFEC1_ACTRL_DLANE2_MASK	VC4_MASK(15, 12)
# define DSI1_PHY_AFEC1_ACTRL_DLANE2_SHIFT	12
# define DSI1_PHY_AFEC1_ACTRL_DLANE1_MASK	VC4_MASK(11, 8)
# define DSI1_PHY_AFEC1_ACTRL_DLANE1_SHIFT	8
# define DSI1_PHY_AFEC1_ACTRL_DLANE0_MASK	VC4_MASK(7, 4)
# define DSI1_PHY_AFEC1_ACTRL_DLANE0_SHIFT	4
# define DSI1_PHY_AFEC1_ACTRL_CLANE_MASK	VC4_MASK(3, 0)
# define DSI1_PHY_AFEC1_ACTRL_CLANE_SHIFT	0

#define DSI1_TST_SEL		0x78
#define DSI1_TST_MON		0x7c
#define DSI1_PHY_TST1		0x80
#define DSI1_PHY_TST2		0x84
#define DSI1_PHY_FIFO_STAT	0x88
/* Actually, all registers in the range that aren't otherwise claimed
 * will return the ID.
 */
#define DSI1_ID			0x8c

struct vc4_dsi_variant {
	/* Whether we're on bcm2835's DSI0 or DSI1. */
	unsigned int port;

	bool broken_axi_workaround;

	const char *debugfs_name;
	const struct debugfs_reg32 *regs;
	size_t nregs;

};

/* General DSI hardware state. */
struct vc4_dsi {
	struct platform_device *pdev;

	struct mipi_dsi_host dsi_host;
	struct drm_encoder *encoder;
	struct drm_bridge *bridge;
	struct list_head bridge_chain;

	void __iomem *regs;

	struct dma_chan *reg_dma_chan;
	dma_addr_t reg_dma_paddr;
	u32 *reg_dma_mem;
	dma_addr_t reg_paddr;

	const struct vc4_dsi_variant *variant;

	/* DSI channel for the panel we're connected to. */
	u32 channel;
	u32 lanes;
	u32 format;
	u32 divider;
	u32 mode_flags;

	/* Input clock from CPRMAN to the digital PHY, for the DSI
	 * escape clock.
	 */
	struct clk *escape_clock;

	/* Input clock to the analog PHY, used to generate the DSI bit
	 * clock.
	 */
	struct clk *pll_phy_clock;

	/* HS Clocks generated within the DSI analog PHY. */
	struct clk_fixed_factor phy_clocks[3];

	struct clk_hw_onecell_data *clk_onecell;

	/* Pixel clock output to the pixelvalve, generated from the HS
	 * clock.
	 */
	struct clk *pixel_clock;

	struct completion xfer_completion;
	int xfer_result;

	struct debugfs_regset32 regset;
};

#define host_to_dsi(host) container_of(host, struct vc4_dsi, dsi_host)

static inline void
dsi_dma_workaround_write(struct vc4_dsi *dsi, u32 offset, u32 val)
{
	struct dma_chan *chan = dsi->reg_dma_chan;
	struct dma_async_tx_descriptor *tx;
	dma_cookie_t cookie;
	int ret;

	/* DSI0 should be able to write normally. */
	if (!chan) {
		writel(val, dsi->regs + offset);
		return;
	}

	*dsi->reg_dma_mem = val;

	tx = chan->device->device_prep_dma_memcpy(chan,
						  dsi->reg_paddr + offset,
						  dsi->reg_dma_paddr,
						  4, 0);
	if (!tx) {
		DRM_ERROR("Failed to set up DMA register write\n");
		return;
	}

	cookie = tx->tx_submit(tx);
	ret = dma_submit_error(cookie);
	if (ret) {
		DRM_ERROR("Failed to submit DMA: %d\n", ret);
		return;
	}
	ret = dma_sync_wait(chan, cookie);
	if (ret)
		DRM_ERROR("Failed to wait for DMA: %d\n", ret);
}

#define DSI_READ(offset) readl(dsi->regs + (offset))
#define DSI_WRITE(offset, val) dsi_dma_workaround_write(dsi, offset, val)
#define DSI_PORT_READ(offset) \
	DSI_READ(dsi->variant->port ? DSI1_##offset : DSI0_##offset)
#define DSI_PORT_WRITE(offset, val) \
	DSI_WRITE(dsi->variant->port ? DSI1_##offset : DSI0_##offset, val)
#define DSI_PORT_BIT(bit) (dsi->variant->port ? DSI1_##bit : DSI0_##bit)

/* VC4 DSI encoder KMS struct */
struct vc4_dsi_encoder {
	struct vc4_encoder base;
	struct vc4_dsi *dsi;
};

static inline struct vc4_dsi_encoder *
to_vc4_dsi_encoder(struct drm_encoder *encoder)
{
	return container_of(encoder, struct vc4_dsi_encoder, base.base);
}

static const struct debugfs_reg32 dsi0_regs[] = {
	VC4_REG32(DSI0_CTRL),
	VC4_REG32(DSI0_STAT),
	VC4_REG32(DSI0_HSTX_TO_CNT),
	VC4_REG32(DSI0_LPRX_TO_CNT),
	VC4_REG32(DSI0_TA_TO_CNT),
	VC4_REG32(DSI0_PR_TO_CNT),
	VC4_REG32(DSI0_DISP0_CTRL),
	VC4_REG32(DSI0_DISP1_CTRL),
	VC4_REG32(DSI0_INT_STAT),
	VC4_REG32(DSI0_INT_EN),
	VC4_REG32(DSI0_PHYC),
	VC4_REG32(DSI0_HS_CLT0),
	VC4_REG32(DSI0_HS_CLT1),
	VC4_REG32(DSI0_HS_CLT2),
	VC4_REG32(DSI0_HS_DLT3),
	VC4_REG32(DSI0_HS_DLT4),
	VC4_REG32(DSI0_HS_DLT5),
	VC4_REG32(DSI0_HS_DLT6),
	VC4_REG32(DSI0_HS_DLT7),
	VC4_REG32(DSI0_PHY_AFEC0),
	VC4_REG32(DSI0_PHY_AFEC1),
	VC4_REG32(DSI0_ID),
};

static const struct debugfs_reg32 dsi1_regs[] = {
	VC4_REG32(DSI1_CTRL),
	VC4_REG32(DSI1_STAT),
	VC4_REG32(DSI1_HSTX_TO_CNT),
	VC4_REG32(DSI1_LPRX_TO_CNT),
	VC4_REG32(DSI1_TA_TO_CNT),
	VC4_REG32(DSI1_PR_TO_CNT),
	VC4_REG32(DSI1_DISP0_CTRL),
	VC4_REG32(DSI1_DISP1_CTRL),
	VC4_REG32(DSI1_INT_STAT),
	VC4_REG32(DSI1_INT_EN),
	VC4_REG32(DSI1_PHYC),
	VC4_REG32(DSI1_HS_CLT0),
	VC4_REG32(DSI1_HS_CLT1),
	VC4_REG32(DSI1_HS_CLT2),
	VC4_REG32(DSI1_HS_DLT3),
	VC4_REG32(DSI1_HS_DLT4),
	VC4_REG32(DSI1_HS_DLT5),
	VC4_REG32(DSI1_HS_DLT6),
	VC4_REG32(DSI1_HS_DLT7),
	VC4_REG32(DSI1_PHY_AFEC0),
	VC4_REG32(DSI1_PHY_AFEC1),
	VC4_REG32(DSI1_ID),
};

static void vc4_dsi_latch_ulps(struct vc4_dsi *dsi, bool latch)
{
	u32 afec0 = DSI_PORT_READ(PHY_AFEC0);

	if (latch)
		afec0 |= DSI_PORT_BIT(PHY_AFEC0_LATCH_ULPS);
	else
		afec0 &= ~DSI_PORT_BIT(PHY_AFEC0_LATCH_ULPS);

	DSI_PORT_WRITE(PHY_AFEC0, afec0);
}

/* Enters or exits Ultra Low Power State. */
static void vc4_dsi_ulps(struct vc4_dsi *dsi, bool ulps)
{
	bool non_continuous = dsi->mode_flags & MIPI_DSI_CLOCK_NON_CONTINUOUS;
	u32 phyc_ulps = ((non_continuous ? DSI_PORT_BIT(PHYC_CLANE_ULPS) : 0) |
			 DSI_PHYC_DLANE0_ULPS |
			 (dsi->lanes > 1 ? DSI_PHYC_DLANE1_ULPS : 0) |
			 (dsi->lanes > 2 ? DSI_PHYC_DLANE2_ULPS : 0) |
			 (dsi->lanes > 3 ? DSI_PHYC_DLANE3_ULPS : 0));
	u32 stat_ulps = ((non_continuous ? DSI1_STAT_PHY_CLOCK_ULPS : 0) |
			 DSI1_STAT_PHY_D0_ULPS |
			 (dsi->lanes > 1 ? DSI1_STAT_PHY_D1_ULPS : 0) |
			 (dsi->lanes > 2 ? DSI1_STAT_PHY_D2_ULPS : 0) |
			 (dsi->lanes > 3 ? DSI1_STAT_PHY_D3_ULPS : 0));
	u32 stat_stop = ((non_continuous ? DSI1_STAT_PHY_CLOCK_STOP : 0) |
			 DSI1_STAT_PHY_D0_STOP |
			 (dsi->lanes > 1 ? DSI1_STAT_PHY_D1_STOP : 0) |
			 (dsi->lanes > 2 ? DSI1_STAT_PHY_D2_STOP : 0) |
			 (dsi->lanes > 3 ? DSI1_STAT_PHY_D3_STOP : 0));
	int ret;
	bool ulps_currently_enabled = (DSI_PORT_READ(PHY_AFEC0) &
				       DSI_PORT_BIT(PHY_AFEC0_LATCH_ULPS));

	if (ulps == ulps_currently_enabled)
		return;

	DSI_PORT_WRITE(STAT, stat_ulps);
	DSI_PORT_WRITE(PHYC, DSI_PORT_READ(PHYC) | phyc_ulps);
	ret = wait_for((DSI_PORT_READ(STAT) & stat_ulps) == stat_ulps, 200);
	if (ret) {
		dev_warn(&dsi->pdev->dev,
			 "Timeout waiting for DSI ULPS entry: STAT 0x%08x",
			 DSI_PORT_READ(STAT));
		DSI_PORT_WRITE(PHYC, DSI_PORT_READ(PHYC) & ~phyc_ulps);
		vc4_dsi_latch_ulps(dsi, false);
		return;
	}

	/* The DSI module can't be disabled while the module is
	 * generating ULPS state.  So, to be able to disable the
	 * module, we have the AFE latch the ULPS state and continue
	 * on to having the module enter STOP.
	 */
	vc4_dsi_latch_ulps(dsi, ulps);

	DSI_PORT_WRITE(STAT, stat_stop);
	DSI_PORT_WRITE(PHYC, DSI_PORT_READ(PHYC) & ~phyc_ulps);
	ret = wait_for((DSI_PORT_READ(STAT) & stat_stop) == stat_stop, 200);
	if (ret) {
		dev_warn(&dsi->pdev->dev,
			 "Timeout waiting for DSI STOP entry: STAT 0x%08x",
			 DSI_PORT_READ(STAT));
		DSI_PORT_WRITE(PHYC, DSI_PORT_READ(PHYC) & ~phyc_ulps);
		return;
	}
}

static u32
dsi_hs_timing(u32 ui_ns, u32 ns, u32 ui)
{
	/* The HS timings have to be rounded up to a multiple of 8
	 * because we're using the byte clock.
	 */
	return roundup(ui + DIV_ROUND_UP(ns, ui_ns), 8);
}

/* ESC always runs at 100Mhz. */
#define ESC_TIME_NS 10

static u32
dsi_esc_timing(u32 ns)
{
	return DIV_ROUND_UP(ns, ESC_TIME_NS);
}

static void vc4_dsi_encoder_disable(struct drm_encoder *encoder)
{
	struct vc4_dsi_encoder *vc4_encoder = to_vc4_dsi_encoder(encoder);
	struct vc4_dsi *dsi = vc4_encoder->dsi;
	struct device *dev = &dsi->pdev->dev;
	struct drm_bridge *iter;

	list_for_each_entry_reverse(iter, &dsi->bridge_chain, chain_node) {
		if (iter->funcs->disable)
			iter->funcs->disable(iter);
	}

	vc4_dsi_ulps(dsi, true);

	list_for_each_entry_from(iter, &dsi->bridge_chain, chain_node) {
		if (iter->funcs->post_disable)
			iter->funcs->post_disable(iter);
	}

	clk_disable_unprepare(dsi->pll_phy_clock);
	clk_disable_unprepare(dsi->escape_clock);
	clk_disable_unprepare(dsi->pixel_clock);

	pm_runtime_put(dev);
}

/* Extends the mode's blank intervals to handle BCM2835's integer-only
 * DSI PLL divider.
 *
 * On 2835, PLLD is set to 2Ghz, and may not be changed by the display
 * driver since most peripherals are hanging off of the PLLD_PER
 * divider.  PLLD_DSI1, which drives our DSI bit clock (and therefore
 * the pixel clock), only has an integer divider off of DSI.
 *
 * To get our panel mode to refresh at the expected 60Hz, we need to
 * extend the horizontal blank time.  This means we drive a
 * higher-than-expected clock rate to the panel, but that's what the
 * firmware does too.
 */
static bool vc4_dsi_encoder_mode_fixup(struct drm_encoder *encoder,
				       const struct drm_display_mode *mode,
				       struct drm_display_mode *adjusted_mode)
{
	struct vc4_dsi_encoder *vc4_encoder = to_vc4_dsi_encoder(encoder);
	struct vc4_dsi *dsi = vc4_encoder->dsi;
	struct clk *phy_parent = clk_get_parent(dsi->pll_phy_clock);
	unsigned long parent_rate = clk_get_rate(phy_parent);
	unsigned long pixel_clock_hz = mode->clock * 1000;
	unsigned long pll_clock = pixel_clock_hz * dsi->divider;
	int divider;

	/* Find what divider gets us a faster clock than the requested
	 * pixel clock.
	 */
	for (divider = 1; divider < 8; divider++) {
		if (parent_rate / divider < pll_clock) {
			divider--;
			break;
		}
	}

	/* Now that we've picked a PLL divider, calculate back to its
	 * pixel clock.
	 */
	pll_clock = parent_rate / divider;
	pixel_clock_hz = pll_clock / dsi->divider;

	adjusted_mode->clock = pixel_clock_hz / 1000;

	/* Given the new pixel clock, adjust HFP to keep vrefresh the same. */
	adjusted_mode->htotal = adjusted_mode->clock * mode->htotal /
				mode->clock;
	adjusted_mode->hsync_end += adjusted_mode->htotal - mode->htotal;
	adjusted_mode->hsync_start += adjusted_mode->htotal - mode->htotal;

	return true;
}

static void vc4_dsi_encoder_enable(struct drm_encoder *encoder)
{
	struct drm_display_mode *mode = &encoder->crtc->state->adjusted_mode;
	struct vc4_dsi_encoder *vc4_encoder = to_vc4_dsi_encoder(encoder);
	struct vc4_dsi *dsi = vc4_encoder->dsi;
	struct device *dev = &dsi->pdev->dev;
	bool debug_dump_regs = false;
	struct drm_bridge *iter;
	unsigned long hs_clock;
	u32 ui_ns;
	/* Minimum LP state duration in escape clock cycles. */
	u32 lpx = dsi_esc_timing(60);
	unsigned long pixel_clock_hz = mode->clock * 1000;
	unsigned long dsip_clock;
	unsigned long phy_clock;
	int ret;

	ret = pm_runtime_get_sync(dev);
	if (ret) {
		DRM_ERROR("Failed to runtime PM enable on DSI%d\n", dsi->variant->port);
		return;
	}

	if (debug_dump_regs) {
		struct drm_printer p = drm_info_printer(&dsi->pdev->dev);
		dev_info(&dsi->pdev->dev, "DSI regs before:\n");
		drm_print_regset32(&p, &dsi->regset);
	}

	/* Round up the clk_set_rate() request slightly, since
	 * PLLD_DSI1 is an integer divider and its rate selection will
	 * never round up.
	 */
	phy_clock = (pixel_clock_hz + 1000) * dsi->divider;
	ret = clk_set_rate(dsi->pll_phy_clock, phy_clock);
	if (ret) {
		dev_err(&dsi->pdev->dev,
			"Failed to set phy clock to %ld: %d\n", phy_clock, ret);
	}

	/* Reset the DSI and all its fifos. */
	DSI_PORT_WRITE(CTRL,
		       DSI_CTRL_SOFT_RESET_CFG |
		       DSI_PORT_BIT(CTRL_RESET_FIFOS));

	DSI_PORT_WRITE(CTRL,
		       DSI_CTRL_HSDT_EOT_DISABLE |
		       DSI_CTRL_RX_LPDT_EOT_DISABLE);

	/* Clear all stat bits so we see what has happened during enable. */
	DSI_PORT_WRITE(STAT, DSI_PORT_READ(STAT));

	/* Set AFE CTR00/CTR1 to release powerdown of analog. */
	if (dsi->variant->port == 0) {
		u32 afec0 = (VC4_SET_FIELD(7, DSI_PHY_AFEC0_PTATADJ) |
			     VC4_SET_FIELD(7, DSI_PHY_AFEC0_CTATADJ));

		if (dsi->lanes < 2)
			afec0 |= DSI0_PHY_AFEC0_PD_DLANE1;

		if (!(dsi->mode_flags & MIPI_DSI_MODE_VIDEO))
			afec0 |= DSI0_PHY_AFEC0_RESET;

		DSI_PORT_WRITE(PHY_AFEC0, afec0);

		DSI_PORT_WRITE(PHY_AFEC1,
			       VC4_SET_FIELD(6,  DSI0_PHY_AFEC1_IDR_DLANE1) |
			       VC4_SET_FIELD(6,  DSI0_PHY_AFEC1_IDR_DLANE0) |
			       VC4_SET_FIELD(6,  DSI0_PHY_AFEC1_IDR_CLANE));
	} else {
		u32 afec0 = (VC4_SET_FIELD(7, DSI_PHY_AFEC0_PTATADJ) |
			     VC4_SET_FIELD(7, DSI_PHY_AFEC0_CTATADJ) |
			     VC4_SET_FIELD(6, DSI1_PHY_AFEC0_IDR_CLANE) |
			     VC4_SET_FIELD(6, DSI1_PHY_AFEC0_IDR_DLANE0) |
			     VC4_SET_FIELD(6, DSI1_PHY_AFEC0_IDR_DLANE1) |
			     VC4_SET_FIELD(6, DSI1_PHY_AFEC0_IDR_DLANE2) |
			     VC4_SET_FIELD(6, DSI1_PHY_AFEC0_IDR_DLANE3));

		if (dsi->lanes < 4)
			afec0 |= DSI1_PHY_AFEC0_PD_DLANE3;
		if (dsi->lanes < 3)
			afec0 |= DSI1_PHY_AFEC0_PD_DLANE2;
		if (dsi->lanes < 2)
			afec0 |= DSI1_PHY_AFEC0_PD_DLANE1;

		afec0 |= DSI1_PHY_AFEC0_RESET;

		DSI_PORT_WRITE(PHY_AFEC0, afec0);

		DSI_PORT_WRITE(PHY_AFEC1, 0);

		/* AFEC reset hold time */
		mdelay(1);
	}

	ret = clk_prepare_enable(dsi->escape_clock);
	if (ret) {
		DRM_ERROR("Failed to turn on DSI escape clock: %d\n", ret);
		return;
	}

	ret = clk_prepare_enable(dsi->pll_phy_clock);
	if (ret) {
		DRM_ERROR("Failed to turn on DSI PLL: %d\n", ret);
		return;
	}

	hs_clock = clk_get_rate(dsi->pll_phy_clock);

	/* Yes, we set the DSI0P/DSI1P pixel clock to the byte rate,
	 * not the pixel clock rate.  DSIxP take from the APHY's byte,
	 * DDR2, or DDR4 clock (we use byte) and feed into the PV at
	 * that rate.  Separately, a value derived from PIX_CLK_DIV
	 * and HS_CLKC is fed into the PV to divide down to the actual
	 * pixel clock for pushing pixels into DSI.
	 */
	dsip_clock = phy_clock / 8;
	ret = clk_set_rate(dsi->pixel_clock, dsip_clock);
	if (ret) {
		dev_err(dev, "Failed to set pixel clock to %ldHz: %d\n",
			dsip_clock, ret);
	}

	ret = clk_prepare_enable(dsi->pixel_clock);
	if (ret) {
		DRM_ERROR("Failed to turn on DSI pixel clock: %d\n", ret);
		return;
	}

	/* How many ns one DSI unit interval is.  Note that the clock
	 * is DDR, so there's an extra divide by 2.
	 */
	ui_ns = DIV_ROUND_UP(500000000, hs_clock);

	DSI_PORT_WRITE(HS_CLT0,
		       VC4_SET_FIELD(dsi_hs_timing(ui_ns, 262, 0),
				     DSI_HS_CLT0_CZERO) |
		       VC4_SET_FIELD(dsi_hs_timing(ui_ns, 0, 8),
				     DSI_HS_CLT0_CPRE) |
		       VC4_SET_FIELD(dsi_hs_timing(ui_ns, 38, 0),
				     DSI_HS_CLT0_CPREP));

	DSI_PORT_WRITE(HS_CLT1,
		       VC4_SET_FIELD(dsi_hs_timing(ui_ns, 60, 0),
				     DSI_HS_CLT1_CTRAIL) |
		       VC4_SET_FIELD(dsi_hs_timing(ui_ns, 60, 52),
				     DSI_HS_CLT1_CPOST));

	DSI_PORT_WRITE(HS_CLT2,
		       VC4_SET_FIELD(dsi_hs_timing(ui_ns, 1000000, 0),
				     DSI_HS_CLT2_WUP));

	DSI_PORT_WRITE(HS_DLT3,
		       VC4_SET_FIELD(dsi_hs_timing(ui_ns, 100, 0),
				     DSI_HS_DLT3_EXIT) |
		       VC4_SET_FIELD(dsi_hs_timing(ui_ns, 105, 6),
				     DSI_HS_DLT3_ZERO) |
		       VC4_SET_FIELD(dsi_hs_timing(ui_ns, 40, 4),
				     DSI_HS_DLT3_PRE));

	DSI_PORT_WRITE(HS_DLT4,
		       VC4_SET_FIELD(dsi_hs_timing(ui_ns, lpx * ESC_TIME_NS, 0),
				     DSI_HS_DLT4_LPX) |
		       VC4_SET_FIELD(max(dsi_hs_timing(ui_ns, 0, 8),
					 dsi_hs_timing(ui_ns, 60, 4)),
				     DSI_HS_DLT4_TRAIL) |
		       VC4_SET_FIELD(0, DSI_HS_DLT4_ANLAT));

	/* T_INIT is how long STOP is driven after power-up to
	 * indicate to the slave (also coming out of power-up) that
	 * master init is complete, and should be greater than the
	 * maximum of two value: T_INIT,MASTER and T_INIT,SLAVE.  The
	 * D-PHY spec gives a minimum 100us for T_INIT,MASTER and
	 * T_INIT,SLAVE, while allowing protocols on top of it to give
	 * greater minimums.  The vc4 firmware uses an extremely
	 * conservative 5ms, and we maintain that here.
	 */
	DSI_PORT_WRITE(HS_DLT5, VC4_SET_FIELD(dsi_hs_timing(ui_ns,
							    5 * 1000 * 1000, 0),
					      DSI_HS_DLT5_INIT));

	DSI_PORT_WRITE(HS_DLT6,
		       VC4_SET_FIELD(lpx * 5, DSI_HS_DLT6_TA_GET) |
		       VC4_SET_FIELD(lpx, DSI_HS_DLT6_TA_SURE) |
		       VC4_SET_FIELD(lpx * 4, DSI_HS_DLT6_TA_GO) |
		       VC4_SET_FIELD(lpx, DSI_HS_DLT6_LP_LPX));

	DSI_PORT_WRITE(HS_DLT7,
		       VC4_SET_FIELD(dsi_esc_timing(1000000),
				     DSI_HS_DLT7_LP_WUP));

	DSI_PORT_WRITE(PHYC,
		       DSI_PHYC_DLANE0_ENABLE |
		       (dsi->lanes >= 2 ? DSI_PHYC_DLANE1_ENABLE : 0) |
		       (dsi->lanes >= 3 ? DSI_PHYC_DLANE2_ENABLE : 0) |
		       (dsi->lanes >= 4 ? DSI_PHYC_DLANE3_ENABLE : 0) |
		       DSI_PORT_BIT(PHYC_CLANE_ENABLE) |
		       ((dsi->mode_flags & MIPI_DSI_CLOCK_NON_CONTINUOUS) ?
			0 : DSI_PORT_BIT(PHYC_HS_CLK_CONTINUOUS)) |
		       (dsi->variant->port == 0 ?
			VC4_SET_FIELD(lpx - 1, DSI0_PHYC_ESC_CLK_LPDT) :
			VC4_SET_FIELD(lpx - 1, DSI1_PHYC_ESC_CLK_LPDT)));

	DSI_PORT_WRITE(CTRL,
		       DSI_PORT_READ(CTRL) |
		       DSI_CTRL_CAL_BYTE);

	/* HS timeout in HS clock cycles: disabled. */
	DSI_PORT_WRITE(HSTX_TO_CNT, 0);
	/* LP receive timeout in HS clocks. */
	DSI_PORT_WRITE(LPRX_TO_CNT, 0xffffff);
	/* Bus turnaround timeout */
	DSI_PORT_WRITE(TA_TO_CNT, 100000);
	/* Display reset sequence timeout */
	DSI_PORT_WRITE(PR_TO_CNT, 100000);

	/* Set up DISP1 for transferring long command payloads through
	 * the pixfifo.
	 */
	DSI_PORT_WRITE(DISP1_CTRL,
		       VC4_SET_FIELD(DSI_DISP1_PFORMAT_32BIT_LE,
				     DSI_DISP1_PFORMAT) |
		       DSI_DISP1_ENABLE);

	/* Ungate the block. */
	if (dsi->variant->port == 0)
		DSI_PORT_WRITE(CTRL, DSI_PORT_READ(CTRL) | DSI0_CTRL_CTRL0);
	else
		DSI_PORT_WRITE(CTRL, DSI_PORT_READ(CTRL) | DSI1_CTRL_EN);

	/* Bring AFE out of reset. */
	if (dsi->variant->port == 0) {
	} else {
		DSI_PORT_WRITE(PHY_AFEC0,
			       DSI_PORT_READ(PHY_AFEC0) &
			       ~DSI1_PHY_AFEC0_RESET);
	}

	vc4_dsi_ulps(dsi, false);

	list_for_each_entry_reverse(iter, &dsi->bridge_chain, chain_node) {
		if (iter->funcs->pre_enable)
			iter->funcs->pre_enable(iter);
	}

	if (dsi->mode_flags & MIPI_DSI_MODE_VIDEO) {
		DSI_PORT_WRITE(DISP0_CTRL,
			       VC4_SET_FIELD(dsi->divider,
					     DSI_DISP0_PIX_CLK_DIV) |
			       VC4_SET_FIELD(dsi->format, DSI_DISP0_PFORMAT) |
			       VC4_SET_FIELD(DSI_DISP0_LP_STOP_PERFRAME,
					     DSI_DISP0_LP_STOP_CTRL) |
			       DSI_DISP0_ST_END |
			       DSI_DISP0_ENABLE);
	} else {
		DSI_PORT_WRITE(DISP0_CTRL,
			       DSI_DISP0_COMMAND_MODE |
			       DSI_DISP0_ENABLE);
	}

	list_for_each_entry(iter, &dsi->bridge_chain, chain_node) {
		if (iter->funcs->enable)
			iter->funcs->enable(iter);
	}

	if (debug_dump_regs) {
		struct drm_printer p = drm_info_printer(&dsi->pdev->dev);
		dev_info(&dsi->pdev->dev, "DSI regs after:\n");
		drm_print_regset32(&p, &dsi->regset);
	}
}

static ssize_t vc4_dsi_host_transfer(struct mipi_dsi_host *host,
				     const struct mipi_dsi_msg *msg)
{
	struct vc4_dsi *dsi = host_to_dsi(host);
	struct mipi_dsi_packet packet;
	u32 pkth = 0, pktc = 0;
	int i, ret;
	bool is_long = mipi_dsi_packet_format_is_long(msg->type);
	u32 cmd_fifo_len = 0, pix_fifo_len = 0;

	mipi_dsi_create_packet(&packet, msg);

	pkth |= VC4_SET_FIELD(packet.header[0], DSI_TXPKT1H_BC_DT);
	pkth |= VC4_SET_FIELD(packet.header[1] |
			      (packet.header[2] << 8),
			      DSI_TXPKT1H_BC_PARAM);
	if (is_long) {
		/* Divide data across the various FIFOs we have available.
		 * The command FIFO takes byte-oriented data, but is of
		 * limited size. The pixel FIFO (never actually used for
		 * pixel data in reality) is word oriented, and substantially
		 * larger. So, we use the pixel FIFO for most of the data,
		 * sending the residual bytes in the command FIFO at the start.
		 *
		 * With this arrangement, the command FIFO will never get full.
		 */
		if (packet.payload_length <= 16) {
			cmd_fifo_len = packet.payload_length;
			pix_fifo_len = 0;
		} else {
			cmd_fifo_len = (packet.payload_length %
					DSI_PIX_FIFO_WIDTH);
			pix_fifo_len = ((packet.payload_length - cmd_fifo_len) /
					DSI_PIX_FIFO_WIDTH);
		}

		WARN_ON_ONCE(pix_fifo_len >= DSI_PIX_FIFO_DEPTH);

		pkth |= VC4_SET_FIELD(cmd_fifo_len, DSI_TXPKT1H_BC_CMDFIFO);
	}

	if (msg->rx_len) {
		pktc |= VC4_SET_FIELD(DSI_TXPKT1C_CMD_CTRL_RX,
				      DSI_TXPKT1C_CMD_CTRL);
	} else {
		pktc |= VC4_SET_FIELD(DSI_TXPKT1C_CMD_CTRL_TX,
				      DSI_TXPKT1C_CMD_CTRL);
	}

	for (i = 0; i < cmd_fifo_len; i++)
		DSI_PORT_WRITE(TXPKT_CMD_FIFO, packet.payload[i]);
	for (i = 0; i < pix_fifo_len; i++) {
		const u8 *pix = packet.payload + cmd_fifo_len + i * 4;

		DSI_PORT_WRITE(TXPKT_PIX_FIFO,
			       pix[0] |
			       pix[1] << 8 |
			       pix[2] << 16 |
			       pix[3] << 24);
	}

	if (msg->flags & MIPI_DSI_MSG_USE_LPM)
		pktc |= DSI_TXPKT1C_CMD_MODE_LP;
	if (is_long)
		pktc |= DSI_TXPKT1C_CMD_TYPE_LONG;

	/* Send one copy of the packet.  Larger repeats are used for pixel
	 * data in command mode.
	 */
	pktc |= VC4_SET_FIELD(1, DSI_TXPKT1C_CMD_REPEAT);

	pktc |= DSI_TXPKT1C_CMD_EN;
	if (pix_fifo_len) {
		pktc |= VC4_SET_FIELD(DSI_TXPKT1C_DISPLAY_NO_SECONDARY,
				      DSI_TXPKT1C_DISPLAY_NO);
	} else {
		pktc |= VC4_SET_FIELD(DSI_TXPKT1C_DISPLAY_NO_SHORT,
				      DSI_TXPKT1C_DISPLAY_NO);
	}

	/* Enable the appropriate interrupt for the transfer completion. */
	dsi->xfer_result = 0;
	reinit_completion(&dsi->xfer_completion);
	DSI_PORT_WRITE(INT_STAT, DSI1_INT_TXPKT1_DONE | DSI1_INT_PHY_DIR_RTF);
	if (msg->rx_len) {
		DSI_PORT_WRITE(INT_EN, (DSI1_INTERRUPTS_ALWAYS_ENABLED |
					DSI1_INT_PHY_DIR_RTF));
	} else {
		DSI_PORT_WRITE(INT_EN, (DSI1_INTERRUPTS_ALWAYS_ENABLED |
					DSI1_INT_TXPKT1_DONE));
	}

	/* Send the packet. */
	DSI_PORT_WRITE(TXPKT1H, pkth);
	DSI_PORT_WRITE(TXPKT1C, pktc);

	if (!wait_for_completion_timeout(&dsi->xfer_completion,
					 msecs_to_jiffies(1000))) {
		dev_err(&dsi->pdev->dev, "transfer interrupt wait timeout");
		dev_err(&dsi->pdev->dev, "instat: 0x%08x\n",
			DSI_PORT_READ(INT_STAT));
		ret = -ETIMEDOUT;
	} else {
		ret = dsi->xfer_result;
	}

	DSI_PORT_WRITE(INT_EN, DSI1_INTERRUPTS_ALWAYS_ENABLED);

	if (ret)
		goto reset_fifo_and_return;

	if (ret == 0 && msg->rx_len) {
		u32 rxpkt1h = DSI_PORT_READ(RXPKT1H);
		u8 *msg_rx = msg->rx_buf;

		if (rxpkt1h & DSI_RXPKT1H_PKT_TYPE_LONG) {
			u32 rxlen = VC4_GET_FIELD(rxpkt1h,
						  DSI_RXPKT1H_BC_PARAM);

			if (rxlen != msg->rx_len) {
				DRM_ERROR("DSI returned %db, expecting %db\n",
					  rxlen, (int)msg->rx_len);
				ret = -ENXIO;
				goto reset_fifo_and_return;
			}

			for (i = 0; i < msg->rx_len; i++)
				msg_rx[i] = DSI_READ(DSI1_RXPKT_FIFO);
		} else {
			/* FINISHME: Handle AWER */

			msg_rx[0] = VC4_GET_FIELD(rxpkt1h,
						  DSI_RXPKT1H_SHORT_0);
			if (msg->rx_len > 1) {
				msg_rx[1] = VC4_GET_FIELD(rxpkt1h,
							  DSI_RXPKT1H_SHORT_1);
			}
		}
	}

	return ret;

reset_fifo_and_return:
	DRM_ERROR("DSI transfer failed, resetting: %d\n", ret);

	DSI_PORT_WRITE(TXPKT1C, DSI_PORT_READ(TXPKT1C) & ~DSI_TXPKT1C_CMD_EN);
	udelay(1);
	DSI_PORT_WRITE(CTRL,
		       DSI_PORT_READ(CTRL) |
		       DSI_PORT_BIT(CTRL_RESET_FIFOS));

	DSI_PORT_WRITE(TXPKT1C, 0);
	DSI_PORT_WRITE(INT_EN, DSI1_INTERRUPTS_ALWAYS_ENABLED);
	return ret;
}

static const struct component_ops vc4_dsi_ops;
static int vc4_dsi_host_attach(struct mipi_dsi_host *host,
			       struct mipi_dsi_device *device)
{
	struct vc4_dsi *dsi = host_to_dsi(host);
	int ret;

	dsi->lanes = device->lanes;
	dsi->channel = device->channel;
	dsi->mode_flags = device->mode_flags;

	switch (device->format) {
	case MIPI_DSI_FMT_RGB888:
		dsi->format = DSI_PFORMAT_RGB888;
		dsi->divider = 24 / dsi->lanes;
		break;
	case MIPI_DSI_FMT_RGB666:
		dsi->format = DSI_PFORMAT_RGB666;
		dsi->divider = 24 / dsi->lanes;
		break;
	case MIPI_DSI_FMT_RGB666_PACKED:
		dsi->format = DSI_PFORMAT_RGB666_PACKED;
		dsi->divider = 18 / dsi->lanes;
		break;
	case MIPI_DSI_FMT_RGB565:
		dsi->format = DSI_PFORMAT_RGB565;
		dsi->divider = 16 / dsi->lanes;
		break;
	default:
		dev_err(&dsi->pdev->dev, "Unknown DSI format: %d.\n",
			dsi->format);
		return 0;
	}

	if (!(dsi->mode_flags & MIPI_DSI_MODE_VIDEO)) {
		dev_err(&dsi->pdev->dev,
			"Only VIDEO mode panels supported currently.\n");
		return 0;
	}

	ret = component_add(&dsi->pdev->dev, &vc4_dsi_ops);
	if (ret) {
		mipi_dsi_host_unregister(&dsi->dsi_host);
		return ret;
	}

	return 0;
}

static int vc4_dsi_host_detach(struct mipi_dsi_host *host,
			       struct mipi_dsi_device *device)
{
	return 0;
}

static const struct mipi_dsi_host_ops vc4_dsi_host_ops = {
	.attach = vc4_dsi_host_attach,
	.detach = vc4_dsi_host_detach,
	.transfer = vc4_dsi_host_transfer,
};

static const struct drm_encoder_helper_funcs vc4_dsi_encoder_helper_funcs = {
	.disable = vc4_dsi_encoder_disable,
	.enable = vc4_dsi_encoder_enable,
	.mode_fixup = vc4_dsi_encoder_mode_fixup,
};

static const struct vc4_dsi_variant bcm2711_dsi1_variant = {
	.port			= 1,
	.debugfs_name		= "dsi1_regs",
	.regs			= dsi1_regs,
	.nregs			= ARRAY_SIZE(dsi1_regs),
};

static const struct vc4_dsi_variant bcm2835_dsi0_variant = {
	.port			= 0,
	.debugfs_name		= "dsi0_regs",
	.regs			= dsi0_regs,
	.nregs			= ARRAY_SIZE(dsi0_regs),
};

static const struct vc4_dsi_variant bcm2835_dsi1_variant = {
	.port			= 1,
	.broken_axi_workaround	= true,
	.debugfs_name		= "dsi1_regs",
	.regs			= dsi1_regs,
	.nregs			= ARRAY_SIZE(dsi1_regs),
};

static const struct of_device_id vc4_dsi_dt_match[] = {
	{ .compatible = "brcm,bcm2711-dsi1", &bcm2711_dsi1_variant },
	{ .compatible = "brcm,bcm2835-dsi0", &bcm2835_dsi0_variant },
	{ .compatible = "brcm,bcm2835-dsi1", &bcm2835_dsi1_variant },
	{}
};

static void dsi_handle_error(struct vc4_dsi *dsi,
			     irqreturn_t *ret, u32 stat, u32 bit,
			     const char *type)
{
	if (!(stat & bit))
		return;

	DRM_ERROR("DSI%d: %s error\n", dsi->variant->port, type);
	*ret = IRQ_HANDLED;
}

/*
 * Initial handler for port 1 where we need the reg_dma workaround.
 * The register DMA writes sleep, so we can't do it in the top half.
 * Instead we use IRQF_ONESHOT so that the IRQ gets disabled in the
 * parent interrupt contrller until our interrupt thread is done.
 */
static irqreturn_t vc4_dsi_irq_defer_to_thread_handler(int irq, void *data)
{
	struct vc4_dsi *dsi = data;
	u32 stat = DSI_PORT_READ(INT_STAT);

	if (!stat)
		return IRQ_NONE;

	return IRQ_WAKE_THREAD;
}

/*
 * Normal IRQ handler for port 0, or the threaded IRQ handler for port
 * 1 where we need the reg_dma workaround.
 */
static irqreturn_t vc4_dsi_irq_handler(int irq, void *data)
{
	struct vc4_dsi *dsi = data;
	u32 stat = DSI_PORT_READ(INT_STAT);
	irqreturn_t ret = IRQ_NONE;

	DSI_PORT_WRITE(INT_STAT, stat);

	dsi_handle_error(dsi, &ret, stat,
			 DSI1_INT_ERR_SYNC_ESC, "LPDT sync");
	dsi_handle_error(dsi, &ret, stat,
			 DSI1_INT_ERR_CONTROL, "data lane 0 sequence");
	dsi_handle_error(dsi, &ret, stat,
			 DSI1_INT_ERR_CONT_LP0, "LP0 contention");
	dsi_handle_error(dsi, &ret, stat,
			 DSI1_INT_ERR_CONT_LP1, "LP1 contention");
	dsi_handle_error(dsi, &ret, stat,
			 DSI1_INT_HSTX_TO, "HSTX timeout");
	dsi_handle_error(dsi, &ret, stat,
			 DSI1_INT_LPRX_TO, "LPRX timeout");
	dsi_handle_error(dsi, &ret, stat,
			 DSI1_INT_TA_TO, "turnaround timeout");
	dsi_handle_error(dsi, &ret, stat,
			 DSI1_INT_PR_TO, "peripheral reset timeout");

	if (stat & (DSI1_INT_TXPKT1_DONE | DSI1_INT_PHY_DIR_RTF)) {
		complete(&dsi->xfer_completion);
		ret = IRQ_HANDLED;
	} else if (stat & DSI1_INT_HSTX_TO) {
		complete(&dsi->xfer_completion);
		dsi->xfer_result = -ETIMEDOUT;
		ret = IRQ_HANDLED;
	}

	return ret;
}

/**
 * vc4_dsi_init_phy_clocks - Exposes clocks generated by the analog
 * PHY that are consumed by CPRMAN (clk-bcm2835.c).
 * @dsi: DSI encoder
 */
static int
vc4_dsi_init_phy_clocks(struct vc4_dsi *dsi)
{
	struct device *dev = &dsi->pdev->dev;
	const char *parent_name = __clk_get_name(dsi->pll_phy_clock);
	static const struct {
		const char *name;
		int div;
	} phy_clocks[] = {
		{ "byte", 8 },
		{ "ddr2", 4 },
		{ "ddr", 2 },
	};
	int i;

	dsi->clk_onecell = devm_kzalloc(dev,
					sizeof(*dsi->clk_onecell) +
					ARRAY_SIZE(phy_clocks) *
					sizeof(struct clk_hw *),
					GFP_KERNEL);
	if (!dsi->clk_onecell)
		return -ENOMEM;
	dsi->clk_onecell->num = ARRAY_SIZE(phy_clocks);

	for (i = 0; i < ARRAY_SIZE(phy_clocks); i++) {
		struct clk_fixed_factor *fix = &dsi->phy_clocks[i];
		struct clk_init_data init;
		char clk_name[16];
		int ret;

		snprintf(clk_name, sizeof(clk_name),
			 "dsi%u_%s", dsi->variant->port, phy_clocks[i].name);

		/* We just use core fixed factor clock ops for the PHY
		 * clocks.  The clocks are actually gated by the
		 * PHY_AFEC0_DDRCLK_EN bits, which we should be
		 * setting if we use the DDR/DDR2 clocks.  However,
		 * vc4_dsi_encoder_enable() is setting up both AFEC0,
		 * setting both our parent DSI PLL's rate and this
		 * clock's rate, so it knows if DDR/DDR2 are going to
		 * be used and could enable the gates itself.
		 */
		fix->mult = 1;
		fix->div = phy_clocks[i].div;
		fix->hw.init = &init;

		memset(&init, 0, sizeof(init));
		init.parent_names = &parent_name;
		init.num_parents = 1;
		init.name = clk_name;
		init.ops = &clk_fixed_factor_ops;

		ret = devm_clk_hw_register(dev, &fix->hw);
		if (ret)
			return ret;

		dsi->clk_onecell->hws[i] = &fix->hw;
	}

	return of_clk_add_hw_provider(dev->of_node,
				      of_clk_hw_onecell_get,
				      dsi->clk_onecell);
}

static int vc4_dsi_bind(struct device *dev, struct device *master, void *data)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct drm_device *drm = dev_get_drvdata(master);
	struct vc4_dsi *dsi = dev_get_drvdata(dev);
	struct vc4_dsi_encoder *vc4_dsi_encoder;
	struct drm_panel *panel;
	const struct of_device_id *match;
	dma_cap_mask_t dma_mask;
	int ret;

	match = of_match_device(vc4_dsi_dt_match, dev);
	if (!match)
		return -ENODEV;

	dsi->variant = match->data;

	vc4_dsi_encoder = devm_kzalloc(dev, sizeof(*vc4_dsi_encoder),
				       GFP_KERNEL);
	if (!vc4_dsi_encoder)
		return -ENOMEM;

	INIT_LIST_HEAD(&dsi->bridge_chain);
	vc4_dsi_encoder->base.type = VC4_ENCODER_TYPE_DSI1;
	vc4_dsi_encoder->dsi = dsi;
	dsi->encoder = &vc4_dsi_encoder->base.base;

	dsi->regs = vc4_ioremap_regs(pdev, 0);
	if (IS_ERR(dsi->regs))
		return PTR_ERR(dsi->regs);

	dsi->regset.base = dsi->regs;
	dsi->regset.regs = dsi->variant->regs;
	dsi->regset.nregs = dsi->variant->nregs;

	if (DSI_PORT_READ(ID) != DSI_ID_VALUE) {
		dev_err(dev, "Port returned 0x%08x for ID instead of 0x%08x\n",
			DSI_PORT_READ(ID), DSI_ID_VALUE);
		return -ENODEV;
	}

	/* DSI1 on BCM2835/6/7 has a broken AXI slave that doesn't respond to
	 * writes from the ARM.  It does handle writes from the DMA engine,
	 * so set up a channel for talking to it.
	 */
	if (dsi->variant->broken_axi_workaround) {
		dsi->reg_dma_mem = dma_alloc_coherent(dev, 4,
						      &dsi->reg_dma_paddr,
						      GFP_KERNEL);
		if (!dsi->reg_dma_mem) {
			DRM_ERROR("Failed to get DMA memory\n");
			return -ENOMEM;
		}

		dma_cap_zero(dma_mask);
		dma_cap_set(DMA_MEMCPY, dma_mask);
		dsi->reg_dma_chan = dma_request_chan_by_mask(&dma_mask);
		if (IS_ERR(dsi->reg_dma_chan)) {
			ret = PTR_ERR(dsi->reg_dma_chan);
			if (ret != -EPROBE_DEFER)
				DRM_ERROR("Failed to get DMA channel: %d\n",
					  ret);
			return ret;
		}

		/* Get the physical address of the device's registers.  The
		 * struct resource for the regs gives us the bus address
		 * instead.
		 */
		dsi->reg_paddr = be32_to_cpup(of_get_address(dev->of_node,
							     0, NULL, NULL));
	}

	init_completion(&dsi->xfer_completion);
	/* At startup enable error-reporting interrupts and nothing else. */
	DSI_PORT_WRITE(INT_EN, DSI1_INTERRUPTS_ALWAYS_ENABLED);
	/* Clear any existing interrupt state. */
	DSI_PORT_WRITE(INT_STAT, DSI_PORT_READ(INT_STAT));

	if (dsi->reg_dma_mem)
		ret = devm_request_threaded_irq(dev, platform_get_irq(pdev, 0),
						vc4_dsi_irq_defer_to_thread_handler,
						vc4_dsi_irq_handler,
						IRQF_ONESHOT,
						"vc4 dsi", dsi);
	else
		ret = devm_request_irq(dev, platform_get_irq(pdev, 0),
				       vc4_dsi_irq_handler, 0, "vc4 dsi", dsi);
	if (ret) {
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "Failed to get interrupt: %d\n", ret);
		return ret;
	}

	dsi->escape_clock = devm_clk_get(dev, "escape");
	if (IS_ERR(dsi->escape_clock)) {
		ret = PTR_ERR(dsi->escape_clock);
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "Failed to get escape clock: %d\n", ret);
		return ret;
	}

	dsi->pll_phy_clock = devm_clk_get(dev, "phy");
	if (IS_ERR(dsi->pll_phy_clock)) {
		ret = PTR_ERR(dsi->pll_phy_clock);
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "Failed to get phy clock: %d\n", ret);
		return ret;
	}

	dsi->pixel_clock = devm_clk_get(dev, "pixel");
	if (IS_ERR(dsi->pixel_clock)) {
		ret = PTR_ERR(dsi->pixel_clock);
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "Failed to get pixel clock: %d\n", ret);
		return ret;
	}

	ret = drm_of_find_panel_or_bridge(dev->of_node, 0, 0,
					  &panel, &dsi->bridge);
	if (ret) {
		/* If the bridge or panel pointed by dev->of_node is not
		 * enabled, just return 0 here so that we don't prevent the DRM
		 * dev from being registered. Of course that means the DSI
		 * encoder won't be exposed, but that's not a problem since
		 * nothing is connected to it.
		 */
		if (ret == -ENODEV)
			return 0;

		return ret;
	}

	if (panel) {
		dsi->bridge = devm_drm_panel_bridge_add_typed(dev, panel,
							      DRM_MODE_CONNECTOR_DSI);
		if (IS_ERR(dsi->bridge))
			return PTR_ERR(dsi->bridge);
	}

	/* The esc clock rate is supposed to always be 100Mhz. */
	ret = clk_set_rate(dsi->escape_clock, 100 * 1000000);
	if (ret) {
		dev_err(dev, "Failed to set esc clock: %d\n", ret);
		return ret;
	}

	ret = vc4_dsi_init_phy_clocks(dsi);
	if (ret)
		return ret;

	drm_simple_encoder_init(drm, dsi->encoder, DRM_MODE_ENCODER_DSI);
	drm_encoder_helper_add(dsi->encoder, &vc4_dsi_encoder_helper_funcs);

	ret = drm_bridge_attach(dsi->encoder, dsi->bridge, NULL, 0);
	if (ret) {
		dev_err(dev, "bridge attach failed: %d\n", ret);
		return ret;
	}
	/* Disable the atomic helper calls into the bridge.  We
	 * manually call the bridge pre_enable / enable / etc. calls
	 * from our driver, since we need to sequence them within the
	 * encoder's enable/disable paths.
	 */
	list_splice_init(&dsi->encoder->bridge_chain, &dsi->bridge_chain);

	vc4_debugfs_add_regset32(drm, dsi->variant->debugfs_name, &dsi->regset);

	pm_runtime_enable(dev);

	return 0;
}

static void vc4_dsi_unbind(struct device *dev, struct device *master,
			   void *data)
{
	struct vc4_dsi *dsi = dev_get_drvdata(dev);

	if (dsi->bridge)
		pm_runtime_disable(dev);

	/*
	 * Restore the bridge_chain so the bridge detach procedure can happen
	 * normally.
	 */
	list_splice_init(&dsi->bridge_chain, &dsi->encoder->bridge_chain);
	drm_encoder_cleanup(dsi->encoder);
}

static const struct component_ops vc4_dsi_ops = {
	.bind   = vc4_dsi_bind,
	.unbind = vc4_dsi_unbind,
};

static int vc4_dsi_dev_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct vc4_dsi *dsi;

	dsi = devm_kzalloc(dev, sizeof(*dsi), GFP_KERNEL);
	if (!dsi)
		return -ENOMEM;
	dev_set_drvdata(dev, dsi);

	dsi->pdev = pdev;
	dsi->dsi_host.ops = &vc4_dsi_host_ops;
	dsi->dsi_host.dev = dev;
	mipi_dsi_host_register(&dsi->dsi_host);

	return 0;
}

static int vc4_dsi_dev_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct vc4_dsi *dsi = dev_get_drvdata(dev);

	component_del(&pdev->dev, &vc4_dsi_ops);
	mipi_dsi_host_unregister(&dsi->dsi_host);

	return 0;
}

struct platform_driver vc4_dsi_driver = {
	.probe = vc4_dsi_dev_probe,
	.remove = vc4_dsi_dev_remove,
	.driver = {
		.name = "vc4_dsi",
		.of_match_table = vc4_dsi_dt_match,
	},
};
