/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2009-2012  Realtek Corporation.*/

#ifndef __RTL8723E_PWRSEQCMD_H__
#define __RTL8723E_PWRSEQCMD_H__

#include "wifi.h"
/*---------------------------------------------
 * 3 The value of cmd: 4 bits
 *---------------------------------------------
 */
#define    PWR_CMD_READ		0x00
#define    PWR_CMD_WRITE	0x01
#define    PWR_CMD_POLLING	0x02
#define    PWR_CMD_DELAY	0x03
#define    PWR_CMD_END		0x04

/* define the base address of each block */
#define   PWR_BASEADDR_MAC	0x00
#define   PWR_BASEADDR_USB	0x01
#define   PWR_BASEADDR_PCIE	0x02
#define   PWR_BASEADDR_SDIO	0x03

#define	PWR_INTF_SDIO_MSK	BIT(0)
#define	PWR_INTF_USB_MSK	BIT(1)
#define	PWR_INTF_PCI_MSK	BIT(2)
#define	PWR_INTF_ALL_MSK	(BIT(0)|BIT(1)|BIT(2)|BIT(3))

#define	PWR_FAB_TSMC_MSK	BIT(0)
#define	PWR_FAB_UMC_MSK		BIT(1)
#define	PWR_FAB_ALL_MSK		(BIT(0)|BIT(1)|BIT(2)|BIT(3))

#define	PWR_CUT_TESTCHIP_MSK	BIT(0)
#define	PWR_CUT_A_MSK		BIT(1)
#define	PWR_CUT_B_MSK		BIT(2)
#define	PWR_CUT_C_MSK		BIT(3)
#define	PWR_CUT_D_MSK		BIT(4)
#define	PWR_CUT_E_MSK		BIT(5)
#define	PWR_CUT_F_MSK		BIT(6)
#define	PWR_CUT_G_MSK		BIT(7)
#define	PWR_CUT_ALL_MSK		0xFF

enum pwrseq_delay_unit {
	PWRSEQ_DELAY_US,
	PWRSEQ_DELAY_MS,
};

struct wlan_pwr_cfg {
	u16 offset;
	u8 cut_msk;
	u8 fab_msk:4;
	u8 interface_msk:4;
	u8 base:4;
	u8 cmd:4;
	u8 msk;
	u8 value;
};

#define	GET_PWR_CFG_OFFSET(__PWR_CMD)	(__PWR_CMD.offset)
#define	GET_PWR_CFG_CUT_MASK(__PWR_CMD)	(__PWR_CMD.cut_msk)
#define	GET_PWR_CFG_FAB_MASK(__PWR_CMD)	(__PWR_CMD.fab_msk)
#define	GET_PWR_CFG_INTF_MASK(__PWR_CMD)	(__PWR_CMD.interface_msk)
#define	GET_PWR_CFG_BASE(__PWR_CMD)	(__PWR_CMD.base)
#define	GET_PWR_CFG_CMD(__PWR_CMD)	(__PWR_CMD.cmd)
#define	GET_PWR_CFG_MASK(__PWR_CMD)	(__PWR_CMD.msk)
#define	GET_PWR_CFG_VALUE(__PWR_CMD)	(__PWR_CMD.value)

bool rtl_hal_pwrseqcmdparsing(struct rtl_priv *rtlpriv, u8 cut_version,
			      u8 fab_version, u8 interface_type,
			      struct wlan_pwr_cfg pwrcfgcmd[]);

#endif
