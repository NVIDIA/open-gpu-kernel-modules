/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Applied Micro X-Gene SoC Ethernet Driver
 *
 * Copyright (c) 2014, Applied Micro Circuits Corporation
 * Authors: Iyappan Subramanian <isubramanian@apm.com>
 *	    Keyur Chudgar <kchudgar@apm.com>
 */

#ifndef __XGENE_ENET_SGMAC_H__
#define __XGENE_ENET_SGMAC_H__

#define PHY_ADDR(src)		(((src)<<8) & GENMASK(12, 8))
#define REG_ADDR(src)		((src) & GENMASK(4, 0))
#define PHY_CONTROL(src)	((src) & GENMASK(15, 0))
#define LINK_SPEED(src)		(((src) & GENMASK(11, 10)) >> 10)
#define INT_PHY_ADDR			0x1e
#define SGMII_TBI_CONTROL_ADDR		0x44
#define SGMII_CONTROL_ADDR		0x00
#define SGMII_STATUS_ADDR		0x04
#define SGMII_BASE_PAGE_ABILITY_ADDR	0x14
#define AUTO_NEG_COMPLETE		BIT(5)
#define LINK_STATUS			BIT(2)
#define LINK_UP				BIT(15)
#define MPA_IDLE_WITH_QMI_EMPTY		BIT(12)
#define SG_RX_DV_GATE_REG_0_ADDR	0x05fc
#define SGMII_EN			0x1

enum xgene_phy_speed {
	PHY_SPEED_10,
	PHY_SPEED_100,
	PHY_SPEED_1000
};

extern const struct xgene_mac_ops xgene_sgmac_ops;
extern const struct xgene_port_ops xgene_sgport_ops;

#endif  /* __XGENE_ENET_SGMAC_H__ */
