/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Marvell 88E6xxx Switch Port Registers support
 *
 * Copyright (c) 2008 Marvell Semiconductor
 *
 * Copyright (c) 2016-2017 Savoir-faire Linux Inc.
 *	Vivien Didelot <vivien.didelot@savoirfairelinux.com>
 */

#ifndef _MV88E6XXX_PORT_H
#define _MV88E6XXX_PORT_H

#include "chip.h"

/* Offset 0x00: Port Status Register */
#define MV88E6XXX_PORT_STS			0x00
#define MV88E6XXX_PORT_STS_PAUSE_EN		0x8000
#define MV88E6XXX_PORT_STS_MY_PAUSE		0x4000
#define MV88E6XXX_PORT_STS_HD_FLOW		0x2000
#define MV88E6XXX_PORT_STS_PHY_DETECT		0x1000
#define MV88E6250_PORT_STS_LINK				0x1000
#define MV88E6250_PORT_STS_PORTMODE_MASK		0x0f00
#define MV88E6250_PORT_STS_PORTMODE_PHY_10_HALF		0x0800
#define MV88E6250_PORT_STS_PORTMODE_PHY_100_HALF	0x0900
#define MV88E6250_PORT_STS_PORTMODE_PHY_10_FULL		0x0a00
#define MV88E6250_PORT_STS_PORTMODE_PHY_100_FULL	0x0b00
#define MV88E6250_PORT_STS_PORTMODE_MII_10_HALF		0x0c00
#define MV88E6250_PORT_STS_PORTMODE_MII_100_HALF	0x0d00
#define MV88E6250_PORT_STS_PORTMODE_MII_10_FULL		0x0e00
#define MV88E6250_PORT_STS_PORTMODE_MII_100_FULL	0x0f00
#define MV88E6XXX_PORT_STS_LINK			0x0800
#define MV88E6XXX_PORT_STS_DUPLEX		0x0400
#define MV88E6XXX_PORT_STS_SPEED_MASK		0x0300
#define MV88E6XXX_PORT_STS_SPEED_10		0x0000
#define MV88E6XXX_PORT_STS_SPEED_100		0x0100
#define MV88E6XXX_PORT_STS_SPEED_1000		0x0200
#define MV88E6XXX_PORT_STS_SPEED_10000		0x0300
#define MV88E6352_PORT_STS_EEE			0x0040
#define MV88E6165_PORT_STS_AM_DIS		0x0040
#define MV88E6185_PORT_STS_MGMII		0x0040
#define MV88E6XXX_PORT_STS_TX_PAUSED		0x0020
#define MV88E6XXX_PORT_STS_FLOW_CTL		0x0010
#define MV88E6XXX_PORT_STS_CMODE_MASK		0x000f
#define MV88E6XXX_PORT_STS_CMODE_RGMII		0x0007
#define MV88E6XXX_PORT_STS_CMODE_100BASEX	0x0008
#define MV88E6XXX_PORT_STS_CMODE_1000BASEX	0x0009
#define MV88E6XXX_PORT_STS_CMODE_SGMII		0x000a
#define MV88E6XXX_PORT_STS_CMODE_2500BASEX	0x000b
#define MV88E6XXX_PORT_STS_CMODE_XAUI		0x000c
#define MV88E6XXX_PORT_STS_CMODE_RXAUI		0x000d
#define MV88E6393X_PORT_STS_CMODE_5GBASER	0x000c
#define MV88E6393X_PORT_STS_CMODE_10GBASER	0x000d
#define MV88E6393X_PORT_STS_CMODE_USXGMII	0x000e
#define MV88E6185_PORT_STS_CDUPLEX		0x0008
#define MV88E6185_PORT_STS_CMODE_MASK		0x0007
#define MV88E6185_PORT_STS_CMODE_GMII_FD	0x0000
#define MV88E6185_PORT_STS_CMODE_MII_100_FD_PS	0x0001
#define MV88E6185_PORT_STS_CMODE_MII_100	0x0002
#define MV88E6185_PORT_STS_CMODE_MII_10		0x0003
#define MV88E6185_PORT_STS_CMODE_SERDES		0x0004
#define MV88E6185_PORT_STS_CMODE_1000BASE_X	0x0005
#define MV88E6185_PORT_STS_CMODE_PHY		0x0006
#define MV88E6185_PORT_STS_CMODE_DISABLED	0x0007

/* Offset 0x01: MAC (or PCS or Physical) Control Register */
#define MV88E6XXX_PORT_MAC_CTL				0x01
#define MV88E6XXX_PORT_MAC_CTL_RGMII_DELAY_RXCLK	0x8000
#define MV88E6XXX_PORT_MAC_CTL_RGMII_DELAY_TXCLK	0x4000
#define MV88E6185_PORT_MAC_CTL_SYNC_OK			0x4000
#define MV88E6390_PORT_MAC_CTL_FORCE_SPEED		0x2000
#define MV88E6390_PORT_MAC_CTL_ALTSPEED			0x1000
#define MV88E6352_PORT_MAC_CTL_200BASE			0x1000
#define MV88E6XXX_PORT_MAC_CTL_EEE			0x0200
#define MV88E6XXX_PORT_MAC_CTL_FORCE_EEE		0x0100
#define MV88E6185_PORT_MAC_CTL_AN_EN			0x0400
#define MV88E6185_PORT_MAC_CTL_AN_RESTART		0x0200
#define MV88E6185_PORT_MAC_CTL_AN_DONE			0x0100
#define MV88E6XXX_PORT_MAC_CTL_FC			0x0080
#define MV88E6XXX_PORT_MAC_CTL_FORCE_FC			0x0040
#define MV88E6XXX_PORT_MAC_CTL_LINK_UP			0x0020
#define MV88E6XXX_PORT_MAC_CTL_FORCE_LINK		0x0010
#define MV88E6XXX_PORT_MAC_CTL_DUPLEX_FULL		0x0008
#define MV88E6XXX_PORT_MAC_CTL_FORCE_DUPLEX		0x0004
#define MV88E6XXX_PORT_MAC_CTL_SPEED_MASK		0x0003
#define MV88E6XXX_PORT_MAC_CTL_SPEED_10			0x0000
#define MV88E6XXX_PORT_MAC_CTL_SPEED_100		0x0001
#define MV88E6065_PORT_MAC_CTL_SPEED_200		0x0002
#define MV88E6XXX_PORT_MAC_CTL_SPEED_1000		0x0002
#define MV88E6390_PORT_MAC_CTL_SPEED_10000		0x0003
#define MV88E6XXX_PORT_MAC_CTL_SPEED_UNFORCED		0x0003

/* Offset 0x02: Jamming Control Register */
#define MV88E6097_PORT_JAM_CTL			0x02
#define MV88E6097_PORT_JAM_CTL_LIMIT_OUT_MASK	0xff00
#define MV88E6097_PORT_JAM_CTL_LIMIT_IN_MASK	0x00ff

/* Offset 0x02: Flow Control Register */
#define MV88E6390_PORT_FLOW_CTL			0x02
#define MV88E6390_PORT_FLOW_CTL_UPDATE		0x8000
#define MV88E6390_PORT_FLOW_CTL_PTR_MASK	0x7f00
#define MV88E6390_PORT_FLOW_CTL_LIMIT_IN	0x0000
#define MV88E6390_PORT_FLOW_CTL_LIMIT_OUT	0x0100
#define MV88E6390_PORT_FLOW_CTL_DATA_MASK	0x00ff

/* Offset 0x03: Switch Identifier Register */
#define MV88E6XXX_PORT_SWITCH_ID		0x03
#define MV88E6XXX_PORT_SWITCH_ID_PROD_MASK	0xfff0
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6085	0x04a0
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6095	0x0950
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6097	0x0990
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6190X	0x0a00
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6390X	0x0a10
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6131	0x1060
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6320	0x1150
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6123	0x1210
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6161	0x1610
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6165	0x1650
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6171	0x1710
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6172	0x1720
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6175	0x1750
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6176	0x1760
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6190	0x1900
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6191	0x1910
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6191X	0x1920
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6193X	0x1930
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6185	0x1a70
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6220	0x2200
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6240	0x2400
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6250	0x2500
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6290	0x2900
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6321	0x3100
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6141	0x3400
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6341	0x3410
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6352	0x3520
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6350	0x3710
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6351	0x3750
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6390	0x3900
#define MV88E6XXX_PORT_SWITCH_ID_PROD_6393X	0x3930
#define MV88E6XXX_PORT_SWITCH_ID_REV_MASK	0x000f

/* Offset 0x04: Port Control Register */
#define MV88E6XXX_PORT_CTL0					0x04
#define MV88E6XXX_PORT_CTL0_USE_CORE_TAG			0x8000
#define MV88E6XXX_PORT_CTL0_DROP_ON_LOCK			0x4000
#define MV88E6XXX_PORT_CTL0_EGRESS_MODE_MASK			0x3000
#define MV88E6XXX_PORT_CTL0_EGRESS_MODE_UNMODIFIED		0x0000
#define MV88E6XXX_PORT_CTL0_EGRESS_MODE_UNTAGGED		0x1000
#define MV88E6XXX_PORT_CTL0_EGRESS_MODE_TAGGED			0x2000
#define MV88E6XXX_PORT_CTL0_EGRESS_MODE_ETHER_TYPE_DSA		0x3000
#define MV88E6XXX_PORT_CTL0_HEADER				0x0800
#define MV88E6XXX_PORT_CTL0_IGMP_MLD_SNOOP			0x0400
#define MV88E6XXX_PORT_CTL0_DOUBLE_TAG				0x0200
#define MV88E6XXX_PORT_CTL0_FRAME_MODE_MASK			0x0300
#define MV88E6XXX_PORT_CTL0_FRAME_MODE_NORMAL			0x0000
#define MV88E6XXX_PORT_CTL0_FRAME_MODE_DSA			0x0100
#define MV88E6XXX_PORT_CTL0_FRAME_MODE_PROVIDER			0x0200
#define MV88E6XXX_PORT_CTL0_FRAME_MODE_ETHER_TYPE_DSA		0x0300
#define MV88E6XXX_PORT_CTL0_DSA_TAG				0x0100
#define MV88E6XXX_PORT_CTL0_VLAN_TUNNEL				0x0080
#define MV88E6XXX_PORT_CTL0_TAG_IF_BOTH				0x0040
#define MV88E6185_PORT_CTL0_USE_IP				0x0020
#define MV88E6185_PORT_CTL0_USE_TAG				0x0010
#define MV88E6185_PORT_CTL0_FORWARD_UNKNOWN			0x0004
#define MV88E6352_PORT_CTL0_EGRESS_FLOODS_UC			0x0004
#define MV88E6352_PORT_CTL0_EGRESS_FLOODS_MC			0x0008
#define MV88E6XXX_PORT_CTL0_STATE_MASK				0x0003
#define MV88E6XXX_PORT_CTL0_STATE_DISABLED			0x0000
#define MV88E6XXX_PORT_CTL0_STATE_BLOCKING			0x0001
#define MV88E6XXX_PORT_CTL0_STATE_LEARNING			0x0002
#define MV88E6XXX_PORT_CTL0_STATE_FORWARDING			0x0003

/* Offset 0x05: Port Control 1 */
#define MV88E6XXX_PORT_CTL1			0x05
#define MV88E6XXX_PORT_CTL1_MESSAGE_PORT	0x8000
#define MV88E6XXX_PORT_CTL1_TRUNK_PORT		0x4000
#define MV88E6XXX_PORT_CTL1_TRUNK_ID_MASK	0x0f00
#define MV88E6XXX_PORT_CTL1_TRUNK_ID_SHIFT	8
#define MV88E6XXX_PORT_CTL1_FID_11_4_MASK	0x00ff

/* Offset 0x06: Port Based VLAN Map */
#define MV88E6XXX_PORT_BASE_VLAN		0x06
#define MV88E6XXX_PORT_BASE_VLAN_FID_3_0_MASK	0xf000

/* Offset 0x07: Default Port VLAN ID & Priority */
#define MV88E6XXX_PORT_DEFAULT_VLAN		0x07
#define MV88E6XXX_PORT_DEFAULT_VLAN_MASK	0x0fff

/* Offset 0x08: Port Control 2 Register */
#define MV88E6XXX_PORT_CTL2				0x08
#define MV88E6XXX_PORT_CTL2_IGNORE_FCS			0x8000
#define MV88E6XXX_PORT_CTL2_VTU_PRI_OVERRIDE		0x4000
#define MV88E6XXX_PORT_CTL2_SA_PRIO_OVERRIDE		0x2000
#define MV88E6XXX_PORT_CTL2_DA_PRIO_OVERRIDE		0x1000
#define MV88E6XXX_PORT_CTL2_JUMBO_MODE_MASK		0x3000
#define MV88E6XXX_PORT_CTL2_JUMBO_MODE_1522		0x0000
#define MV88E6XXX_PORT_CTL2_JUMBO_MODE_2048		0x1000
#define MV88E6XXX_PORT_CTL2_JUMBO_MODE_10240		0x2000
#define MV88E6XXX_PORT_CTL2_8021Q_MODE_MASK		0x0c00
#define MV88E6XXX_PORT_CTL2_8021Q_MODE_DISABLED		0x0000
#define MV88E6XXX_PORT_CTL2_8021Q_MODE_FALLBACK		0x0400
#define MV88E6XXX_PORT_CTL2_8021Q_MODE_CHECK		0x0800
#define MV88E6XXX_PORT_CTL2_8021Q_MODE_SECURE		0x0c00
#define MV88E6XXX_PORT_CTL2_DISCARD_TAGGED		0x0200
#define MV88E6XXX_PORT_CTL2_DISCARD_UNTAGGED		0x0100
#define MV88E6XXX_PORT_CTL2_MAP_DA			0x0080
#define MV88E6XXX_PORT_CTL2_DEFAULT_FORWARD		0x0040
#define MV88E6XXX_PORT_CTL2_EGRESS_MONITOR		0x0020
#define MV88E6XXX_PORT_CTL2_INGRESS_MONITOR		0x0010
#define MV88E6095_PORT_CTL2_CPU_PORT_MASK		0x000f

/* Offset 0x09: Egress Rate Control */
#define MV88E6XXX_PORT_EGRESS_RATE_CTL1		0x09

/* Offset 0x0A: Egress Rate Control 2 */
#define MV88E6XXX_PORT_EGRESS_RATE_CTL2		0x0a

/* Offset 0x0B: Port Association Vector */
#define MV88E6XXX_PORT_ASSOC_VECTOR			0x0b
#define MV88E6XXX_PORT_ASSOC_VECTOR_HOLD_AT_1		0x8000
#define MV88E6XXX_PORT_ASSOC_VECTOR_INT_AGE_OUT		0x4000
#define MV88E6XXX_PORT_ASSOC_VECTOR_LOCKED_PORT		0x2000
#define MV88E6XXX_PORT_ASSOC_VECTOR_IGNORE_WRONG	0x1000
#define MV88E6XXX_PORT_ASSOC_VECTOR_REFRESH_LOCKED	0x0800

/* Offset 0x0C: Port ATU Control */
#define MV88E6XXX_PORT_ATU_CTL		0x0c

/* Offset 0x0D: Priority Override Register */
#define MV88E6XXX_PORT_PRI_OVERRIDE	0x0d

/* Offset 0x0E: Policy Control Register */
#define MV88E6XXX_PORT_POLICY_CTL		0x0e
#define MV88E6XXX_PORT_POLICY_CTL_DA_MASK	0xc000
#define MV88E6XXX_PORT_POLICY_CTL_SA_MASK	0x3000
#define MV88E6XXX_PORT_POLICY_CTL_VTU_MASK	0x0c00
#define MV88E6XXX_PORT_POLICY_CTL_ETYPE_MASK	0x0300
#define MV88E6XXX_PORT_POLICY_CTL_PPPOE_MASK	0x00c0
#define MV88E6XXX_PORT_POLICY_CTL_VBAS_MASK	0x0030
#define MV88E6XXX_PORT_POLICY_CTL_OPT82_MASK	0x000c
#define MV88E6XXX_PORT_POLICY_CTL_UDP_MASK	0x0003
#define MV88E6XXX_PORT_POLICY_CTL_NORMAL	0x0000
#define MV88E6XXX_PORT_POLICY_CTL_MIRROR	0x0001
#define MV88E6XXX_PORT_POLICY_CTL_TRAP		0x0002
#define MV88E6XXX_PORT_POLICY_CTL_DISCARD	0x0003

/* Offset 0x0E: Policy & MGMT Control Register (FAMILY_6393X) */
#define MV88E6393X_PORT_POLICY_MGMT_CTL				0x0e
#define MV88E6393X_PORT_POLICY_MGMT_CTL_UPDATE			0x8000
#define MV88E6393X_PORT_POLICY_MGMT_CTL_PTR_MASK		0x3f00
#define MV88E6393X_PORT_POLICY_MGMT_CTL_DATA_MASK		0x00ff
#define MV88E6393X_PORT_POLICY_MGMT_CTL_PTR_01C280000000XLO	0x2000
#define MV88E6393X_PORT_POLICY_MGMT_CTL_PTR_01C280000000XHI	0x2100
#define MV88E6393X_PORT_POLICY_MGMT_CTL_PTR_01C280000002XLO	0x2400
#define MV88E6393X_PORT_POLICY_MGMT_CTL_PTR_01C280000002XHI	0x2500
#define MV88E6393X_PORT_POLICY_MGMT_CTL_PTR_INGRESS_DEST	0x3000
#define MV88E6393X_PORT_POLICY_MGMT_CTL_PTR_CPU_DEST		0x3800
#define MV88E6393X_PORT_POLICY_MGMT_CTL_CPU_DEST_MGMTPRI	0x00e0

/* Offset 0x0F: Port Special Ether Type */
#define MV88E6XXX_PORT_ETH_TYPE		0x0f
#define MV88E6XXX_PORT_ETH_TYPE_DEFAULT	0x9100

/* Offset 0x10: InDiscards Low Counter */
#define MV88E6XXX_PORT_IN_DISCARD_LO	0x10

/* Offset 0x10: Extended Port Control Command */
#define MV88E6393X_PORT_EPC_CMD		0x10
#define MV88E6393X_PORT_EPC_CMD_BUSY	0x8000
#define MV88E6393X_PORT_EPC_CMD_WRITE	0x0300
#define MV88E6393X_PORT_EPC_INDEX_PORT_ETYPE	0x02

/* Offset 0x11: Extended Port Control Data */
#define MV88E6393X_PORT_EPC_DATA	0x11

/* Offset 0x11: InDiscards High Counter */
#define MV88E6XXX_PORT_IN_DISCARD_HI	0x11

/* Offset 0x12: InFiltered Counter */
#define MV88E6XXX_PORT_IN_FILTERED	0x12

/* Offset 0x13: OutFiltered Counter */
#define MV88E6XXX_PORT_OUT_FILTERED	0x13

/* Offset 0x18: IEEE Priority Mapping Table */
#define MV88E6390_PORT_IEEE_PRIO_MAP_TABLE			0x18
#define MV88E6390_PORT_IEEE_PRIO_MAP_TABLE_UPDATE		0x8000
#define MV88E6390_PORT_IEEE_PRIO_MAP_TABLE_MASK			0x7000
#define MV88E6390_PORT_IEEE_PRIO_MAP_TABLE_INGRESS_PCP		0x0000
#define MV88E6390_PORT_IEEE_PRIO_MAP_TABLE_EGRESS_GREEN_PCP	0x1000
#define MV88E6390_PORT_IEEE_PRIO_MAP_TABLE_EGRESS_YELLOW_PCP	0x2000
#define MV88E6390_PORT_IEEE_PRIO_MAP_TABLE_EGRESS_AVB_PCP	0x3000
#define MV88E6390_PORT_IEEE_PRIO_MAP_TABLE_EGRESS_GREEN_DSCP	0x5000
#define MV88E6390_PORT_IEEE_PRIO_MAP_TABLE_EGRESS_YELLOW_DSCP	0x6000
#define MV88E6390_PORT_IEEE_PRIO_MAP_TABLE_EGRESS_AVB_DSCP	0x7000
#define MV88E6390_PORT_IEEE_PRIO_MAP_TABLE_PTR_MASK		0x0e00
#define MV88E6390_PORT_IEEE_PRIO_MAP_TABLE_DATA_MASK		0x01ff

/* Offset 0x18: Port IEEE Priority Remapping Registers (0-3) */
#define MV88E6095_PORT_IEEE_PRIO_REMAP_0123	0x18

/* Offset 0x19: Port IEEE Priority Remapping Registers (4-7) */
#define MV88E6095_PORT_IEEE_PRIO_REMAP_4567	0x19

/* Offset 0x1a: Magic undocumented errata register */
#define MV88E6XXX_PORT_RESERVED_1A		0x1a
#define MV88E6XXX_PORT_RESERVED_1A_BUSY		0x8000
#define MV88E6XXX_PORT_RESERVED_1A_WRITE	0x4000
#define MV88E6XXX_PORT_RESERVED_1A_READ		0x0000
#define MV88E6XXX_PORT_RESERVED_1A_PORT_SHIFT	5
#define MV88E6XXX_PORT_RESERVED_1A_BLOCK_SHIFT	10
#define MV88E6XXX_PORT_RESERVED_1A_CTRL_PORT	0x04
#define MV88E6XXX_PORT_RESERVED_1A_DATA_PORT	0x05
#define MV88E6341_PORT_RESERVED_1A_FORCE_CMODE	0x8000
#define MV88E6341_PORT_RESERVED_1A_SGMII_AN	0x2000

int mv88e6xxx_port_read(struct mv88e6xxx_chip *chip, int port, int reg,
			u16 *val);
int mv88e6xxx_port_write(struct mv88e6xxx_chip *chip, int port, int reg,
			 u16 val);
int mv88e6xxx_port_wait_bit(struct mv88e6xxx_chip *chip, int port, int reg,
			    int bit, int val);

int mv88e6185_port_set_pause(struct mv88e6xxx_chip *chip, int port,
			     int pause);
int mv88e6352_port_set_rgmii_delay(struct mv88e6xxx_chip *chip, int port,
				   phy_interface_t mode);
int mv88e6390_port_set_rgmii_delay(struct mv88e6xxx_chip *chip, int port,
				   phy_interface_t mode);

int mv88e6xxx_port_set_link(struct mv88e6xxx_chip *chip, int port, int link);

int mv88e6xxx_port_sync_link(struct mv88e6xxx_chip *chip, int port, unsigned int mode, bool isup);
int mv88e6185_port_sync_link(struct mv88e6xxx_chip *chip, int port, unsigned int mode, bool isup);

int mv88e6065_port_set_speed_duplex(struct mv88e6xxx_chip *chip, int port,
				    int speed, int duplex);
int mv88e6185_port_set_speed_duplex(struct mv88e6xxx_chip *chip, int port,
				    int speed, int duplex);
int mv88e6250_port_set_speed_duplex(struct mv88e6xxx_chip *chip, int port,
				    int speed, int duplex);
int mv88e6341_port_set_speed_duplex(struct mv88e6xxx_chip *chip, int port,
				    int speed, int duplex);
int mv88e6352_port_set_speed_duplex(struct mv88e6xxx_chip *chip, int port,
				    int speed, int duplex);
int mv88e6390_port_set_speed_duplex(struct mv88e6xxx_chip *chip, int port,
				    int speed, int duplex);
int mv88e6390x_port_set_speed_duplex(struct mv88e6xxx_chip *chip, int port,
				     int speed, int duplex);
int mv88e6393x_port_set_speed_duplex(struct mv88e6xxx_chip *chip, int port,
				     int speed, int duplex);

phy_interface_t mv88e6341_port_max_speed_mode(int port);
phy_interface_t mv88e6390_port_max_speed_mode(int port);
phy_interface_t mv88e6390x_port_max_speed_mode(int port);
phy_interface_t mv88e6393x_port_max_speed_mode(int port);

int mv88e6xxx_port_set_state(struct mv88e6xxx_chip *chip, int port, u8 state);

int mv88e6xxx_port_set_vlan_map(struct mv88e6xxx_chip *chip, int port, u16 map);

int mv88e6xxx_port_get_fid(struct mv88e6xxx_chip *chip, int port, u16 *fid);
int mv88e6xxx_port_set_fid(struct mv88e6xxx_chip *chip, int port, u16 fid);

int mv88e6xxx_port_get_pvid(struct mv88e6xxx_chip *chip, int port, u16 *pvid);
int mv88e6xxx_port_set_pvid(struct mv88e6xxx_chip *chip, int port, u16 pvid);

int mv88e6xxx_port_set_8021q_mode(struct mv88e6xxx_chip *chip, int port,
				  u16 mode);
int mv88e6095_port_tag_remap(struct mv88e6xxx_chip *chip, int port);
int mv88e6390_port_tag_remap(struct mv88e6xxx_chip *chip, int port);
int mv88e6xxx_port_set_egress_mode(struct mv88e6xxx_chip *chip, int port,
				   enum mv88e6xxx_egress_mode mode);
int mv88e6085_port_set_frame_mode(struct mv88e6xxx_chip *chip, int port,
				  enum mv88e6xxx_frame_mode mode);
int mv88e6351_port_set_frame_mode(struct mv88e6xxx_chip *chip, int port,
				  enum mv88e6xxx_frame_mode mode);
int mv88e6185_port_set_forward_unknown(struct mv88e6xxx_chip *chip,
				       int port, bool unicast);
int mv88e6185_port_set_default_forward(struct mv88e6xxx_chip *chip,
				       int port, bool multicast);
int mv88e6352_port_set_ucast_flood(struct mv88e6xxx_chip *chip, int port,
				   bool unicast);
int mv88e6352_port_set_mcast_flood(struct mv88e6xxx_chip *chip, int port,
				   bool multicast);
int mv88e6352_port_set_policy(struct mv88e6xxx_chip *chip, int port,
			      enum mv88e6xxx_policy_mapping mapping,
			      enum mv88e6xxx_policy_action action);
int mv88e6393x_port_set_policy(struct mv88e6xxx_chip *chip, int port,
			       enum mv88e6xxx_policy_mapping mapping,
			       enum mv88e6xxx_policy_action action);
int mv88e6351_port_set_ether_type(struct mv88e6xxx_chip *chip, int port,
				  u16 etype);
int mv88e6393x_set_egress_port(struct mv88e6xxx_chip *chip,
			       enum mv88e6xxx_egress_direction direction,
			       int port);
int mv88e6393x_port_set_upstream_port(struct mv88e6xxx_chip *chip, int port,
				      int upstream_port);
int mv88e6393x_port_mgmt_rsvd2cpu(struct mv88e6xxx_chip *chip);
int mv88e6393x_port_set_ether_type(struct mv88e6xxx_chip *chip, int port,
				   u16 etype);
int mv88e6xxx_port_set_message_port(struct mv88e6xxx_chip *chip, int port,
				    bool message_port);
int mv88e6xxx_port_set_trunk(struct mv88e6xxx_chip *chip, int port,
			     bool trunk, u8 id);
int mv88e6165_port_set_jumbo_size(struct mv88e6xxx_chip *chip, int port,
				  size_t size);
int mv88e6095_port_egress_rate_limiting(struct mv88e6xxx_chip *chip, int port);
int mv88e6097_port_egress_rate_limiting(struct mv88e6xxx_chip *chip, int port);
int mv88e6xxx_port_set_assoc_vector(struct mv88e6xxx_chip *chip, int port,
				    u16 pav);
int mv88e6097_port_pause_limit(struct mv88e6xxx_chip *chip, int port, u8 in,
			       u8 out);
int mv88e6390_port_pause_limit(struct mv88e6xxx_chip *chip, int port, u8 in,
			       u8 out);
int mv88e6341_port_set_cmode(struct mv88e6xxx_chip *chip, int port,
			     phy_interface_t mode);
int mv88e6390_port_set_cmode(struct mv88e6xxx_chip *chip, int port,
			     phy_interface_t mode);
int mv88e6390x_port_set_cmode(struct mv88e6xxx_chip *chip, int port,
			      phy_interface_t mode);
int mv88e6393x_port_set_cmode(struct mv88e6xxx_chip *chip, int port,
			      phy_interface_t mode);
int mv88e6185_port_get_cmode(struct mv88e6xxx_chip *chip, int port, u8 *cmode);
int mv88e6352_port_get_cmode(struct mv88e6xxx_chip *chip, int port, u8 *cmode);
int mv88e6xxx_port_set_map_da(struct mv88e6xxx_chip *chip, int port);
int mv88e6095_port_set_upstream_port(struct mv88e6xxx_chip *chip, int port,
				     int upstream_port);
int mv88e6xxx_port_set_mirror(struct mv88e6xxx_chip *chip, int port,
			      enum mv88e6xxx_egress_direction direction,
			      bool mirror);

int mv88e6xxx_port_disable_learn_limit(struct mv88e6xxx_chip *chip, int port);
int mv88e6xxx_port_disable_pri_override(struct mv88e6xxx_chip *chip, int port);

int mv88e6xxx_port_hidden_write(struct mv88e6xxx_chip *chip, int block,
				int port, int reg, u16 val);
int mv88e6xxx_port_hidden_wait(struct mv88e6xxx_chip *chip);
int mv88e6xxx_port_hidden_read(struct mv88e6xxx_chip *chip, int block, int port,
			       int reg, u16 *val);

#endif /* _MV88E6XXX_PORT_H */
