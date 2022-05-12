/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 */

#ifndef __DTS_HISTB_CLOCK_H
#define __DTS_HISTB_CLOCK_H

/* clocks provided by core CRG */
#define HISTB_OSC_CLK			0
#define HISTB_APB_CLK			1
#define HISTB_AHB_CLK			2
#define HISTB_UART1_CLK			3
#define HISTB_UART2_CLK			4
#define HISTB_UART3_CLK			5
#define HISTB_I2C0_CLK			6
#define HISTB_I2C1_CLK			7
#define HISTB_I2C2_CLK			8
#define HISTB_I2C3_CLK			9
#define HISTB_I2C4_CLK			10
#define HISTB_I2C5_CLK			11
#define HISTB_SPI0_CLK			12
#define HISTB_SPI1_CLK			13
#define HISTB_SPI2_CLK			14
#define HISTB_SCI_CLK			15
#define HISTB_FMC_CLK			16
#define HISTB_MMC_BIU_CLK		17
#define HISTB_MMC_CIU_CLK		18
#define HISTB_MMC_DRV_CLK		19
#define HISTB_MMC_SAMPLE_CLK		20
#define HISTB_SDIO0_BIU_CLK		21
#define HISTB_SDIO0_CIU_CLK		22
#define HISTB_SDIO0_DRV_CLK		23
#define HISTB_SDIO0_SAMPLE_CLK		24
#define HISTB_PCIE_AUX_CLK		25
#define HISTB_PCIE_PIPE_CLK		26
#define HISTB_PCIE_SYS_CLK		27
#define HISTB_PCIE_BUS_CLK		28
#define HISTB_ETH0_MAC_CLK		29
#define HISTB_ETH0_MACIF_CLK		30
#define HISTB_ETH1_MAC_CLK		31
#define HISTB_ETH1_MACIF_CLK		32
#define HISTB_COMBPHY1_CLK		33
#define HISTB_USB2_BUS_CLK		34
#define HISTB_USB2_PHY_CLK		35
#define HISTB_USB2_UTMI_CLK		36
#define HISTB_USB2_12M_CLK		37
#define HISTB_USB2_48M_CLK		38
#define HISTB_USB2_OTG_UTMI_CLK		39
#define HISTB_USB2_PHY1_REF_CLK		40
#define HISTB_USB2_PHY2_REF_CLK		41
#define HISTB_COMBPHY0_CLK		42
#define HISTB_USB3_BUS_CLK		43
#define HISTB_USB3_UTMI_CLK		44
#define HISTB_USB3_PIPE_CLK		45
#define HISTB_USB3_SUSPEND_CLK		46
#define HISTB_USB3_BUS_CLK1		47
#define HISTB_USB3_UTMI_CLK1		48
#define HISTB_USB3_PIPE_CLK1		49
#define HISTB_USB3_SUSPEND_CLK1		50

/* clocks provided by mcu CRG */
#define HISTB_MCE_CLK			1
#define HISTB_IR_CLK			2
#define HISTB_TIMER01_CLK		3
#define HISTB_LEDC_CLK			4
#define HISTB_UART0_CLK			5
#define HISTB_LSADC_CLK			6

#endif	/* __DTS_HISTB_CLOCK_H */
