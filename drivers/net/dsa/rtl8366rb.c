// SPDX-License-Identifier: GPL-2.0
/* Realtek SMI subdriver for the Realtek RTL8366RB ethernet switch
 *
 * This is a sparsely documented chip, the only viable documentation seems
 * to be a patched up code drop from the vendor that appear in various
 * GPL source trees.
 *
 * Copyright (C) 2017 Linus Walleij <linus.walleij@linaro.org>
 * Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2010 Antti Seppälä <a.seppala@gmail.com>
 * Copyright (C) 2010 Roman Yeryomin <roman@advem.lv>
 * Copyright (C) 2011 Colin Leitner <colin.leitner@googlemail.com>
 */

#include <linux/bitops.h>
#include <linux/etherdevice.h>
#include <linux/interrupt.h>
#include <linux/irqdomain.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/of_irq.h>
#include <linux/regmap.h>

#include "realtek-smi-core.h"

#define RTL8366RB_PORT_NUM_CPU		5
#define RTL8366RB_NUM_PORTS		6
#define RTL8366RB_PHY_NO_MAX		4
#define RTL8366RB_PHY_ADDR_MAX		31

/* Switch Global Configuration register */
#define RTL8366RB_SGCR				0x0000
#define RTL8366RB_SGCR_EN_BC_STORM_CTRL		BIT(0)
#define RTL8366RB_SGCR_MAX_LENGTH(a)		((a) << 4)
#define RTL8366RB_SGCR_MAX_LENGTH_MASK		RTL8366RB_SGCR_MAX_LENGTH(0x3)
#define RTL8366RB_SGCR_MAX_LENGTH_1522		RTL8366RB_SGCR_MAX_LENGTH(0x0)
#define RTL8366RB_SGCR_MAX_LENGTH_1536		RTL8366RB_SGCR_MAX_LENGTH(0x1)
#define RTL8366RB_SGCR_MAX_LENGTH_1552		RTL8366RB_SGCR_MAX_LENGTH(0x2)
#define RTL8366RB_SGCR_MAX_LENGTH_16000		RTL8366RB_SGCR_MAX_LENGTH(0x3)
#define RTL8366RB_SGCR_EN_VLAN			BIT(13)
#define RTL8366RB_SGCR_EN_VLAN_4KTB		BIT(14)

/* Port Enable Control register */
#define RTL8366RB_PECR				0x0001

/* Switch Security Control registers */
#define RTL8366RB_SSCR0				0x0002
#define RTL8366RB_SSCR1				0x0003
#define RTL8366RB_SSCR2				0x0004
#define RTL8366RB_SSCR2_DROP_UNKNOWN_DA		BIT(0)

/* Port Mode Control registers */
#define RTL8366RB_PMC0				0x0005
#define RTL8366RB_PMC0_SPI			BIT(0)
#define RTL8366RB_PMC0_EN_AUTOLOAD		BIT(1)
#define RTL8366RB_PMC0_PROBE			BIT(2)
#define RTL8366RB_PMC0_DIS_BISR			BIT(3)
#define RTL8366RB_PMC0_ADCTEST			BIT(4)
#define RTL8366RB_PMC0_SRAM_DIAG		BIT(5)
#define RTL8366RB_PMC0_EN_SCAN			BIT(6)
#define RTL8366RB_PMC0_P4_IOMODE_SHIFT		7
#define RTL8366RB_PMC0_P4_IOMODE_MASK		GENMASK(9, 7)
#define RTL8366RB_PMC0_P5_IOMODE_SHIFT		10
#define RTL8366RB_PMC0_P5_IOMODE_MASK		GENMASK(12, 10)
#define RTL8366RB_PMC0_SDSMODE_SHIFT		13
#define RTL8366RB_PMC0_SDSMODE_MASK		GENMASK(15, 13)
#define RTL8366RB_PMC1				0x0006

/* Port Mirror Control Register */
#define RTL8366RB_PMCR				0x0007
#define RTL8366RB_PMCR_SOURCE_PORT(a)		(a)
#define RTL8366RB_PMCR_SOURCE_PORT_MASK		0x000f
#define RTL8366RB_PMCR_MONITOR_PORT(a)		((a) << 4)
#define RTL8366RB_PMCR_MONITOR_PORT_MASK	0x00f0
#define RTL8366RB_PMCR_MIRROR_RX		BIT(8)
#define RTL8366RB_PMCR_MIRROR_TX		BIT(9)
#define RTL8366RB_PMCR_MIRROR_SPC		BIT(10)
#define RTL8366RB_PMCR_MIRROR_ISO		BIT(11)

/* bits 0..7 = port 0, bits 8..15 = port 1 */
#define RTL8366RB_PAACR0		0x0010
/* bits 0..7 = port 2, bits 8..15 = port 3 */
#define RTL8366RB_PAACR1		0x0011
/* bits 0..7 = port 4, bits 8..15 = port 5 */
#define RTL8366RB_PAACR2		0x0012
#define RTL8366RB_PAACR_SPEED_10M	0
#define RTL8366RB_PAACR_SPEED_100M	1
#define RTL8366RB_PAACR_SPEED_1000M	2
#define RTL8366RB_PAACR_FULL_DUPLEX	BIT(2)
#define RTL8366RB_PAACR_LINK_UP		BIT(4)
#define RTL8366RB_PAACR_TX_PAUSE	BIT(5)
#define RTL8366RB_PAACR_RX_PAUSE	BIT(6)
#define RTL8366RB_PAACR_AN		BIT(7)

#define RTL8366RB_PAACR_CPU_PORT	(RTL8366RB_PAACR_SPEED_1000M | \
					 RTL8366RB_PAACR_FULL_DUPLEX | \
					 RTL8366RB_PAACR_LINK_UP | \
					 RTL8366RB_PAACR_TX_PAUSE | \
					 RTL8366RB_PAACR_RX_PAUSE)

/* bits 0..7 = port 0, bits 8..15 = port 1 */
#define RTL8366RB_PSTAT0		0x0014
/* bits 0..7 = port 2, bits 8..15 = port 3 */
#define RTL8366RB_PSTAT1		0x0015
/* bits 0..7 = port 4, bits 8..15 = port 5 */
#define RTL8366RB_PSTAT2		0x0016

#define RTL8366RB_POWER_SAVING_REG	0x0021

/* CPU port control reg */
#define RTL8368RB_CPU_CTRL_REG		0x0061
#define RTL8368RB_CPU_PORTS_MSK		0x00FF
/* Disables inserting custom tag length/type 0x8899 */
#define RTL8368RB_CPU_NO_TAG		BIT(15)

#define RTL8366RB_SMAR0			0x0070 /* bits 0..15 */
#define RTL8366RB_SMAR1			0x0071 /* bits 16..31 */
#define RTL8366RB_SMAR2			0x0072 /* bits 32..47 */

#define RTL8366RB_RESET_CTRL_REG		0x0100
#define RTL8366RB_CHIP_CTRL_RESET_HW		BIT(0)
#define RTL8366RB_CHIP_CTRL_RESET_SW		BIT(1)

#define RTL8366RB_CHIP_ID_REG			0x0509
#define RTL8366RB_CHIP_ID_8366			0x5937
#define RTL8366RB_CHIP_VERSION_CTRL_REG		0x050A
#define RTL8366RB_CHIP_VERSION_MASK		0xf

/* PHY registers control */
#define RTL8366RB_PHY_ACCESS_CTRL_REG		0x8000
#define RTL8366RB_PHY_CTRL_READ			BIT(0)
#define RTL8366RB_PHY_CTRL_WRITE		0
#define RTL8366RB_PHY_ACCESS_BUSY_REG		0x8001
#define RTL8366RB_PHY_INT_BUSY			BIT(0)
#define RTL8366RB_PHY_EXT_BUSY			BIT(4)
#define RTL8366RB_PHY_ACCESS_DATA_REG		0x8002
#define RTL8366RB_PHY_EXT_CTRL_REG		0x8010
#define RTL8366RB_PHY_EXT_WRDATA_REG		0x8011
#define RTL8366RB_PHY_EXT_RDDATA_REG		0x8012

#define RTL8366RB_PHY_REG_MASK			0x1f
#define RTL8366RB_PHY_PAGE_OFFSET		5
#define RTL8366RB_PHY_PAGE_MASK			(0xf << 5)
#define RTL8366RB_PHY_NO_OFFSET			9
#define RTL8366RB_PHY_NO_MASK			(0x1f << 9)

#define RTL8366RB_VLAN_INGRESS_CTRL2_REG	0x037f

/* LED control registers */
#define RTL8366RB_LED_BLINKRATE_REG		0x0430
#define RTL8366RB_LED_BLINKRATE_MASK		0x0007
#define RTL8366RB_LED_BLINKRATE_28MS		0x0000
#define RTL8366RB_LED_BLINKRATE_56MS		0x0001
#define RTL8366RB_LED_BLINKRATE_84MS		0x0002
#define RTL8366RB_LED_BLINKRATE_111MS		0x0003
#define RTL8366RB_LED_BLINKRATE_222MS		0x0004
#define RTL8366RB_LED_BLINKRATE_446MS		0x0005

#define RTL8366RB_LED_CTRL_REG			0x0431
#define RTL8366RB_LED_OFF			0x0
#define RTL8366RB_LED_DUP_COL			0x1
#define RTL8366RB_LED_LINK_ACT			0x2
#define RTL8366RB_LED_SPD1000			0x3
#define RTL8366RB_LED_SPD100			0x4
#define RTL8366RB_LED_SPD10			0x5
#define RTL8366RB_LED_SPD1000_ACT		0x6
#define RTL8366RB_LED_SPD100_ACT		0x7
#define RTL8366RB_LED_SPD10_ACT			0x8
#define RTL8366RB_LED_SPD100_10_ACT		0x9
#define RTL8366RB_LED_FIBER			0xa
#define RTL8366RB_LED_AN_FAULT			0xb
#define RTL8366RB_LED_LINK_RX			0xc
#define RTL8366RB_LED_LINK_TX			0xd
#define RTL8366RB_LED_MASTER			0xe
#define RTL8366RB_LED_FORCE			0xf
#define RTL8366RB_LED_0_1_CTRL_REG		0x0432
#define RTL8366RB_LED_1_OFFSET			6
#define RTL8366RB_LED_2_3_CTRL_REG		0x0433
#define RTL8366RB_LED_3_OFFSET			6

#define RTL8366RB_MIB_COUNT			33
#define RTL8366RB_GLOBAL_MIB_COUNT		1
#define RTL8366RB_MIB_COUNTER_PORT_OFFSET	0x0050
#define RTL8366RB_MIB_COUNTER_BASE		0x1000
#define RTL8366RB_MIB_CTRL_REG			0x13F0
#define RTL8366RB_MIB_CTRL_USER_MASK		0x0FFC
#define RTL8366RB_MIB_CTRL_BUSY_MASK		BIT(0)
#define RTL8366RB_MIB_CTRL_RESET_MASK		BIT(1)
#define RTL8366RB_MIB_CTRL_PORT_RESET(_p)	BIT(2 + (_p))
#define RTL8366RB_MIB_CTRL_GLOBAL_RESET		BIT(11)

#define RTL8366RB_PORT_VLAN_CTRL_BASE		0x0063
#define RTL8366RB_PORT_VLAN_CTRL_REG(_p)  \
		(RTL8366RB_PORT_VLAN_CTRL_BASE + (_p) / 4)
#define RTL8366RB_PORT_VLAN_CTRL_MASK		0xf
#define RTL8366RB_PORT_VLAN_CTRL_SHIFT(_p)	(4 * ((_p) % 4))

#define RTL8366RB_VLAN_TABLE_READ_BASE		0x018C
#define RTL8366RB_VLAN_TABLE_WRITE_BASE		0x0185

#define RTL8366RB_TABLE_ACCESS_CTRL_REG		0x0180
#define RTL8366RB_TABLE_VLAN_READ_CTRL		0x0E01
#define RTL8366RB_TABLE_VLAN_WRITE_CTRL		0x0F01

#define RTL8366RB_VLAN_MC_BASE(_x)		(0x0020 + (_x) * 3)

#define RTL8366RB_PORT_LINK_STATUS_BASE		0x0014
#define RTL8366RB_PORT_STATUS_SPEED_MASK	0x0003
#define RTL8366RB_PORT_STATUS_DUPLEX_MASK	0x0004
#define RTL8366RB_PORT_STATUS_LINK_MASK		0x0010
#define RTL8366RB_PORT_STATUS_TXPAUSE_MASK	0x0020
#define RTL8366RB_PORT_STATUS_RXPAUSE_MASK	0x0040
#define RTL8366RB_PORT_STATUS_AN_MASK		0x0080

#define RTL8366RB_NUM_VLANS		16
#define RTL8366RB_NUM_LEDGROUPS		4
#define RTL8366RB_NUM_VIDS		4096
#define RTL8366RB_PRIORITYMAX		7
#define RTL8366RB_FIDMAX		7

#define RTL8366RB_PORT_1		BIT(0) /* In userspace port 0 */
#define RTL8366RB_PORT_2		BIT(1) /* In userspace port 1 */
#define RTL8366RB_PORT_3		BIT(2) /* In userspace port 2 */
#define RTL8366RB_PORT_4		BIT(3) /* In userspace port 3 */
#define RTL8366RB_PORT_5		BIT(4) /* In userspace port 4 */

#define RTL8366RB_PORT_CPU		BIT(5) /* CPU port */

#define RTL8366RB_PORT_ALL		(RTL8366RB_PORT_1 |	\
					 RTL8366RB_PORT_2 |	\
					 RTL8366RB_PORT_3 |	\
					 RTL8366RB_PORT_4 |	\
					 RTL8366RB_PORT_5 |	\
					 RTL8366RB_PORT_CPU)

#define RTL8366RB_PORT_ALL_BUT_CPU	(RTL8366RB_PORT_1 |	\
					 RTL8366RB_PORT_2 |	\
					 RTL8366RB_PORT_3 |	\
					 RTL8366RB_PORT_4 |	\
					 RTL8366RB_PORT_5)

#define RTL8366RB_PORT_ALL_EXTERNAL	(RTL8366RB_PORT_1 |	\
					 RTL8366RB_PORT_2 |	\
					 RTL8366RB_PORT_3 |	\
					 RTL8366RB_PORT_4)

#define RTL8366RB_PORT_ALL_INTERNAL	 RTL8366RB_PORT_CPU

/* First configuration word per member config, VID and prio */
#define RTL8366RB_VLAN_VID_MASK		0xfff
#define RTL8366RB_VLAN_PRIORITY_SHIFT	12
#define RTL8366RB_VLAN_PRIORITY_MASK	0x7
/* Second configuration word per member config, member and untagged */
#define RTL8366RB_VLAN_UNTAG_SHIFT	8
#define RTL8366RB_VLAN_UNTAG_MASK	0xff
#define RTL8366RB_VLAN_MEMBER_MASK	0xff
/* Third config word per member config, STAG currently unused */
#define RTL8366RB_VLAN_STAG_MBR_MASK	0xff
#define RTL8366RB_VLAN_STAG_MBR_SHIFT	8
#define RTL8366RB_VLAN_STAG_IDX_MASK	0x7
#define RTL8366RB_VLAN_STAG_IDX_SHIFT	5
#define RTL8366RB_VLAN_FID_MASK		0x7

/* Port ingress bandwidth control */
#define RTL8366RB_IB_BASE		0x0200
#define RTL8366RB_IB_REG(pnum)		(RTL8366RB_IB_BASE + (pnum))
#define RTL8366RB_IB_BDTH_MASK		0x3fff
#define RTL8366RB_IB_PREIFG		BIT(14)

/* Port egress bandwidth control */
#define RTL8366RB_EB_BASE		0x02d1
#define RTL8366RB_EB_REG(pnum)		(RTL8366RB_EB_BASE + (pnum))
#define RTL8366RB_EB_BDTH_MASK		0x3fff
#define RTL8366RB_EB_PREIFG_REG		0x02f8
#define RTL8366RB_EB_PREIFG		BIT(9)

#define RTL8366RB_BDTH_SW_MAX		1048512 /* 1048576? */
#define RTL8366RB_BDTH_UNIT		64
#define RTL8366RB_BDTH_REG_DEFAULT	16383

/* QOS */
#define RTL8366RB_QOS			BIT(15)
/* Include/Exclude Preamble and IFG (20 bytes). 0:Exclude, 1:Include. */
#define RTL8366RB_QOS_DEFAULT_PREIFG	1

/* Interrupt handling */
#define RTL8366RB_INTERRUPT_CONTROL_REG	0x0440
#define RTL8366RB_INTERRUPT_POLARITY	BIT(0)
#define RTL8366RB_P4_RGMII_LED		BIT(2)
#define RTL8366RB_INTERRUPT_MASK_REG	0x0441
#define RTL8366RB_INTERRUPT_LINK_CHGALL	GENMASK(11, 0)
#define RTL8366RB_INTERRUPT_ACLEXCEED	BIT(8)
#define RTL8366RB_INTERRUPT_STORMEXCEED	BIT(9)
#define RTL8366RB_INTERRUPT_P4_FIBER	BIT(12)
#define RTL8366RB_INTERRUPT_P4_UTP	BIT(13)
#define RTL8366RB_INTERRUPT_VALID	(RTL8366RB_INTERRUPT_LINK_CHGALL | \
					 RTL8366RB_INTERRUPT_ACLEXCEED | \
					 RTL8366RB_INTERRUPT_STORMEXCEED | \
					 RTL8366RB_INTERRUPT_P4_FIBER | \
					 RTL8366RB_INTERRUPT_P4_UTP)
#define RTL8366RB_INTERRUPT_STATUS_REG	0x0442
#define RTL8366RB_NUM_INTERRUPT		14 /* 0..13 */

/* bits 0..5 enable force when cleared */
#define RTL8366RB_MAC_FORCE_CTRL_REG	0x0F11

#define RTL8366RB_OAM_PARSER_REG	0x0F14
#define RTL8366RB_OAM_MULTIPLEXER_REG	0x0F15

#define RTL8366RB_GREEN_FEATURE_REG	0x0F51
#define RTL8366RB_GREEN_FEATURE_MSK	0x0007
#define RTL8366RB_GREEN_FEATURE_TX	BIT(0)
#define RTL8366RB_GREEN_FEATURE_RX	BIT(2)

/**
 * struct rtl8366rb - RTL8366RB-specific data
 * @max_mtu: per-port max MTU setting
 */
struct rtl8366rb {
	unsigned int max_mtu[RTL8366RB_NUM_PORTS];
};

static struct rtl8366_mib_counter rtl8366rb_mib_counters[] = {
	{ 0,  0, 4, "IfInOctets"				},
	{ 0,  4, 4, "EtherStatsOctets"				},
	{ 0,  8, 2, "EtherStatsUnderSizePkts"			},
	{ 0, 10, 2, "EtherFragments"				},
	{ 0, 12, 2, "EtherStatsPkts64Octets"			},
	{ 0, 14, 2, "EtherStatsPkts65to127Octets"		},
	{ 0, 16, 2, "EtherStatsPkts128to255Octets"		},
	{ 0, 18, 2, "EtherStatsPkts256to511Octets"		},
	{ 0, 20, 2, "EtherStatsPkts512to1023Octets"		},
	{ 0, 22, 2, "EtherStatsPkts1024to1518Octets"		},
	{ 0, 24, 2, "EtherOversizeStats"			},
	{ 0, 26, 2, "EtherStatsJabbers"				},
	{ 0, 28, 2, "IfInUcastPkts"				},
	{ 0, 30, 2, "EtherStatsMulticastPkts"			},
	{ 0, 32, 2, "EtherStatsBroadcastPkts"			},
	{ 0, 34, 2, "EtherStatsDropEvents"			},
	{ 0, 36, 2, "Dot3StatsFCSErrors"			},
	{ 0, 38, 2, "Dot3StatsSymbolErrors"			},
	{ 0, 40, 2, "Dot3InPauseFrames"				},
	{ 0, 42, 2, "Dot3ControlInUnknownOpcodes"		},
	{ 0, 44, 4, "IfOutOctets"				},
	{ 0, 48, 2, "Dot3StatsSingleCollisionFrames"		},
	{ 0, 50, 2, "Dot3StatMultipleCollisionFrames"		},
	{ 0, 52, 2, "Dot3sDeferredTransmissions"		},
	{ 0, 54, 2, "Dot3StatsLateCollisions"			},
	{ 0, 56, 2, "EtherStatsCollisions"			},
	{ 0, 58, 2, "Dot3StatsExcessiveCollisions"		},
	{ 0, 60, 2, "Dot3OutPauseFrames"			},
	{ 0, 62, 2, "Dot1dBasePortDelayExceededDiscards"	},
	{ 0, 64, 2, "Dot1dTpPortInDiscards"			},
	{ 0, 66, 2, "IfOutUcastPkts"				},
	{ 0, 68, 2, "IfOutMulticastPkts"			},
	{ 0, 70, 2, "IfOutBroadcastPkts"			},
};

static int rtl8366rb_get_mib_counter(struct realtek_smi *smi,
				     int port,
				     struct rtl8366_mib_counter *mib,
				     u64 *mibvalue)
{
	u32 addr, val;
	int ret;
	int i;

	addr = RTL8366RB_MIB_COUNTER_BASE +
		RTL8366RB_MIB_COUNTER_PORT_OFFSET * (port) +
		mib->offset;

	/* Writing access counter address first
	 * then ASIC will prepare 64bits counter wait for being retrived
	 */
	ret = regmap_write(smi->map, addr, 0); /* Write whatever */
	if (ret)
		return ret;

	/* Read MIB control register */
	ret = regmap_read(smi->map, RTL8366RB_MIB_CTRL_REG, &val);
	if (ret)
		return -EIO;

	if (val & RTL8366RB_MIB_CTRL_BUSY_MASK)
		return -EBUSY;

	if (val & RTL8366RB_MIB_CTRL_RESET_MASK)
		return -EIO;

	/* Read each individual MIB 16 bits at the time */
	*mibvalue = 0;
	for (i = mib->length; i > 0; i--) {
		ret = regmap_read(smi->map, addr + (i - 1), &val);
		if (ret)
			return ret;
		*mibvalue = (*mibvalue << 16) | (val & 0xFFFF);
	}
	return 0;
}

static u32 rtl8366rb_get_irqmask(struct irq_data *d)
{
	int line = irqd_to_hwirq(d);
	u32 val;

	/* For line interrupts we combine link down in bits
	 * 6..11 with link up in bits 0..5 into one interrupt.
	 */
	if (line < 12)
		val = BIT(line) | BIT(line + 6);
	else
		val = BIT(line);
	return val;
}

static void rtl8366rb_mask_irq(struct irq_data *d)
{
	struct realtek_smi *smi = irq_data_get_irq_chip_data(d);
	int ret;

	ret = regmap_update_bits(smi->map, RTL8366RB_INTERRUPT_MASK_REG,
				 rtl8366rb_get_irqmask(d), 0);
	if (ret)
		dev_err(smi->dev, "could not mask IRQ\n");
}

static void rtl8366rb_unmask_irq(struct irq_data *d)
{
	struct realtek_smi *smi = irq_data_get_irq_chip_data(d);
	int ret;

	ret = regmap_update_bits(smi->map, RTL8366RB_INTERRUPT_MASK_REG,
				 rtl8366rb_get_irqmask(d),
				 rtl8366rb_get_irqmask(d));
	if (ret)
		dev_err(smi->dev, "could not unmask IRQ\n");
}

static irqreturn_t rtl8366rb_irq(int irq, void *data)
{
	struct realtek_smi *smi = data;
	u32 stat;
	int ret;

	/* This clears the IRQ status register */
	ret = regmap_read(smi->map, RTL8366RB_INTERRUPT_STATUS_REG,
			  &stat);
	if (ret) {
		dev_err(smi->dev, "can't read interrupt status\n");
		return IRQ_NONE;
	}
	stat &= RTL8366RB_INTERRUPT_VALID;
	if (!stat)
		return IRQ_NONE;
	while (stat) {
		int line = __ffs(stat);
		int child_irq;

		stat &= ~BIT(line);
		/* For line interrupts we combine link down in bits
		 * 6..11 with link up in bits 0..5 into one interrupt.
		 */
		if (line < 12 && line > 5)
			line -= 5;
		child_irq = irq_find_mapping(smi->irqdomain, line);
		handle_nested_irq(child_irq);
	}
	return IRQ_HANDLED;
}

static struct irq_chip rtl8366rb_irq_chip = {
	.name = "RTL8366RB",
	.irq_mask = rtl8366rb_mask_irq,
	.irq_unmask = rtl8366rb_unmask_irq,
};

static int rtl8366rb_irq_map(struct irq_domain *domain, unsigned int irq,
			     irq_hw_number_t hwirq)
{
	irq_set_chip_data(irq, domain->host_data);
	irq_set_chip_and_handler(irq, &rtl8366rb_irq_chip, handle_simple_irq);
	irq_set_nested_thread(irq, 1);
	irq_set_noprobe(irq);

	return 0;
}

static void rtl8366rb_irq_unmap(struct irq_domain *d, unsigned int irq)
{
	irq_set_nested_thread(irq, 0);
	irq_set_chip_and_handler(irq, NULL, NULL);
	irq_set_chip_data(irq, NULL);
}

static const struct irq_domain_ops rtl8366rb_irqdomain_ops = {
	.map = rtl8366rb_irq_map,
	.unmap = rtl8366rb_irq_unmap,
	.xlate  = irq_domain_xlate_onecell,
};

static int rtl8366rb_setup_cascaded_irq(struct realtek_smi *smi)
{
	struct device_node *intc;
	unsigned long irq_trig;
	int irq;
	int ret;
	u32 val;
	int i;

	intc = of_get_child_by_name(smi->dev->of_node, "interrupt-controller");
	if (!intc) {
		dev_err(smi->dev, "missing child interrupt-controller node\n");
		return -EINVAL;
	}
	/* RB8366RB IRQs cascade off this one */
	irq = of_irq_get(intc, 0);
	if (irq <= 0) {
		dev_err(smi->dev, "failed to get parent IRQ\n");
		ret = irq ? irq : -EINVAL;
		goto out_put_node;
	}

	/* This clears the IRQ status register */
	ret = regmap_read(smi->map, RTL8366RB_INTERRUPT_STATUS_REG,
			  &val);
	if (ret) {
		dev_err(smi->dev, "can't read interrupt status\n");
		goto out_put_node;
	}

	/* Fetch IRQ edge information from the descriptor */
	irq_trig = irqd_get_trigger_type(irq_get_irq_data(irq));
	switch (irq_trig) {
	case IRQF_TRIGGER_RISING:
	case IRQF_TRIGGER_HIGH:
		dev_info(smi->dev, "active high/rising IRQ\n");
		val = 0;
		break;
	case IRQF_TRIGGER_FALLING:
	case IRQF_TRIGGER_LOW:
		dev_info(smi->dev, "active low/falling IRQ\n");
		val = RTL8366RB_INTERRUPT_POLARITY;
		break;
	}
	ret = regmap_update_bits(smi->map, RTL8366RB_INTERRUPT_CONTROL_REG,
				 RTL8366RB_INTERRUPT_POLARITY,
				 val);
	if (ret) {
		dev_err(smi->dev, "could not configure IRQ polarity\n");
		goto out_put_node;
	}

	ret = devm_request_threaded_irq(smi->dev, irq, NULL,
					rtl8366rb_irq, IRQF_ONESHOT,
					"RTL8366RB", smi);
	if (ret) {
		dev_err(smi->dev, "unable to request irq: %d\n", ret);
		goto out_put_node;
	}
	smi->irqdomain = irq_domain_add_linear(intc,
					       RTL8366RB_NUM_INTERRUPT,
					       &rtl8366rb_irqdomain_ops,
					       smi);
	if (!smi->irqdomain) {
		dev_err(smi->dev, "failed to create IRQ domain\n");
		ret = -EINVAL;
		goto out_put_node;
	}
	for (i = 0; i < smi->num_ports; i++)
		irq_set_parent(irq_create_mapping(smi->irqdomain, i), irq);

out_put_node:
	of_node_put(intc);
	return ret;
}

static int rtl8366rb_set_addr(struct realtek_smi *smi)
{
	u8 addr[ETH_ALEN];
	u16 val;
	int ret;

	eth_random_addr(addr);

	dev_info(smi->dev, "set MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
		 addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	val = addr[0] << 8 | addr[1];
	ret = regmap_write(smi->map, RTL8366RB_SMAR0, val);
	if (ret)
		return ret;
	val = addr[2] << 8 | addr[3];
	ret = regmap_write(smi->map, RTL8366RB_SMAR1, val);
	if (ret)
		return ret;
	val = addr[4] << 8 | addr[5];
	ret = regmap_write(smi->map, RTL8366RB_SMAR2, val);
	if (ret)
		return ret;

	return 0;
}

/* Found in a vendor driver */

/* Struct for handling the jam tables' entries */
struct rtl8366rb_jam_tbl_entry {
	u16 reg;
	u16 val;
};

/* For the "version 0" early silicon, appear in most source releases */
static const struct rtl8366rb_jam_tbl_entry rtl8366rb_init_jam_ver_0[] = {
	{0x000B, 0x0001}, {0x03A6, 0x0100}, {0x03A7, 0x0001}, {0x02D1, 0x3FFF},
	{0x02D2, 0x3FFF}, {0x02D3, 0x3FFF}, {0x02D4, 0x3FFF}, {0x02D5, 0x3FFF},
	{0x02D6, 0x3FFF}, {0x02D7, 0x3FFF}, {0x02D8, 0x3FFF}, {0x022B, 0x0688},
	{0x022C, 0x0FAC}, {0x03D0, 0x4688}, {0x03D1, 0x01F5}, {0x0000, 0x0830},
	{0x02F9, 0x0200}, {0x02F7, 0x7FFF}, {0x02F8, 0x03FF}, {0x0080, 0x03E8},
	{0x0081, 0x00CE}, {0x0082, 0x00DA}, {0x0083, 0x0230}, {0xBE0F, 0x2000},
	{0x0231, 0x422A}, {0x0232, 0x422A}, {0x0233, 0x422A}, {0x0234, 0x422A},
	{0x0235, 0x422A}, {0x0236, 0x422A}, {0x0237, 0x422A}, {0x0238, 0x422A},
	{0x0239, 0x422A}, {0x023A, 0x422A}, {0x023B, 0x422A}, {0x023C, 0x422A},
	{0x023D, 0x422A}, {0x023E, 0x422A}, {0x023F, 0x422A}, {0x0240, 0x422A},
	{0x0241, 0x422A}, {0x0242, 0x422A}, {0x0243, 0x422A}, {0x0244, 0x422A},
	{0x0245, 0x422A}, {0x0246, 0x422A}, {0x0247, 0x422A}, {0x0248, 0x422A},
	{0x0249, 0x0146}, {0x024A, 0x0146}, {0x024B, 0x0146}, {0xBE03, 0xC961},
	{0x024D, 0x0146}, {0x024E, 0x0146}, {0x024F, 0x0146}, {0x0250, 0x0146},
	{0xBE64, 0x0226}, {0x0252, 0x0146}, {0x0253, 0x0146}, {0x024C, 0x0146},
	{0x0251, 0x0146}, {0x0254, 0x0146}, {0xBE62, 0x3FD0}, {0x0084, 0x0320},
	{0x0255, 0x0146}, {0x0256, 0x0146}, {0x0257, 0x0146}, {0x0258, 0x0146},
	{0x0259, 0x0146}, {0x025A, 0x0146}, {0x025B, 0x0146}, {0x025C, 0x0146},
	{0x025D, 0x0146}, {0x025E, 0x0146}, {0x025F, 0x0146}, {0x0260, 0x0146},
	{0x0261, 0xA23F}, {0x0262, 0x0294}, {0x0263, 0xA23F}, {0x0264, 0x0294},
	{0x0265, 0xA23F}, {0x0266, 0x0294}, {0x0267, 0xA23F}, {0x0268, 0x0294},
	{0x0269, 0xA23F}, {0x026A, 0x0294}, {0x026B, 0xA23F}, {0x026C, 0x0294},
	{0x026D, 0xA23F}, {0x026E, 0x0294}, {0x026F, 0xA23F}, {0x0270, 0x0294},
	{0x02F5, 0x0048}, {0xBE09, 0x0E00}, {0xBE1E, 0x0FA0}, {0xBE14, 0x8448},
	{0xBE15, 0x1007}, {0xBE4A, 0xA284}, {0xC454, 0x3F0B}, {0xC474, 0x3F0B},
	{0xBE48, 0x3672}, {0xBE4B, 0x17A7}, {0xBE4C, 0x0B15}, {0xBE52, 0x0EDD},
	{0xBE49, 0x8C00}, {0xBE5B, 0x785C}, {0xBE5C, 0x785C}, {0xBE5D, 0x785C},
	{0xBE61, 0x368A}, {0xBE63, 0x9B84}, {0xC456, 0xCC13}, {0xC476, 0xCC13},
	{0xBE65, 0x307D}, {0xBE6D, 0x0005}, {0xBE6E, 0xE120}, {0xBE2E, 0x7BAF},
};

/* This v1 init sequence is from Belkin F5D8235 U-Boot release */
static const struct rtl8366rb_jam_tbl_entry rtl8366rb_init_jam_ver_1[] = {
	{0x0000, 0x0830}, {0x0001, 0x8000}, {0x0400, 0x8130}, {0xBE78, 0x3C3C},
	{0x0431, 0x5432}, {0xBE37, 0x0CE4}, {0x02FA, 0xFFDF}, {0x02FB, 0xFFE0},
	{0xC44C, 0x1585}, {0xC44C, 0x1185}, {0xC44C, 0x1585}, {0xC46C, 0x1585},
	{0xC46C, 0x1185}, {0xC46C, 0x1585}, {0xC451, 0x2135}, {0xC471, 0x2135},
	{0xBE10, 0x8140}, {0xBE15, 0x0007}, {0xBE6E, 0xE120}, {0xBE69, 0xD20F},
	{0xBE6B, 0x0320}, {0xBE24, 0xB000}, {0xBE23, 0xFF51}, {0xBE22, 0xDF20},
	{0xBE21, 0x0140}, {0xBE20, 0x00BB}, {0xBE24, 0xB800}, {0xBE24, 0x0000},
	{0xBE24, 0x7000}, {0xBE23, 0xFF51}, {0xBE22, 0xDF60}, {0xBE21, 0x0140},
	{0xBE20, 0x0077}, {0xBE24, 0x7800}, {0xBE24, 0x0000}, {0xBE2E, 0x7B7A},
	{0xBE36, 0x0CE4}, {0x02F5, 0x0048}, {0xBE77, 0x2940}, {0x000A, 0x83E0},
	{0xBE79, 0x3C3C}, {0xBE00, 0x1340},
};

/* This v2 init sequence is from Belkin F5D8235 U-Boot release */
static const struct rtl8366rb_jam_tbl_entry rtl8366rb_init_jam_ver_2[] = {
	{0x0450, 0x0000}, {0x0400, 0x8130}, {0x000A, 0x83ED}, {0x0431, 0x5432},
	{0xC44F, 0x6250}, {0xC46F, 0x6250}, {0xC456, 0x0C14}, {0xC476, 0x0C14},
	{0xC44C, 0x1C85}, {0xC44C, 0x1885}, {0xC44C, 0x1C85}, {0xC46C, 0x1C85},
	{0xC46C, 0x1885}, {0xC46C, 0x1C85}, {0xC44C, 0x0885}, {0xC44C, 0x0881},
	{0xC44C, 0x0885}, {0xC46C, 0x0885}, {0xC46C, 0x0881}, {0xC46C, 0x0885},
	{0xBE2E, 0x7BA7}, {0xBE36, 0x1000}, {0xBE37, 0x1000}, {0x8000, 0x0001},
	{0xBE69, 0xD50F}, {0x8000, 0x0000}, {0xBE69, 0xD50F}, {0xBE6E, 0x0320},
	{0xBE77, 0x2940}, {0xBE78, 0x3C3C}, {0xBE79, 0x3C3C}, {0xBE6E, 0xE120},
	{0x8000, 0x0001}, {0xBE15, 0x1007}, {0x8000, 0x0000}, {0xBE15, 0x1007},
	{0xBE14, 0x0448}, {0xBE1E, 0x00A0}, {0xBE10, 0x8160}, {0xBE10, 0x8140},
	{0xBE00, 0x1340}, {0x0F51, 0x0010},
};

/* Appears in a DDWRT code dump */
static const struct rtl8366rb_jam_tbl_entry rtl8366rb_init_jam_ver_3[] = {
	{0x0000, 0x0830}, {0x0400, 0x8130}, {0x000A, 0x83ED}, {0x0431, 0x5432},
	{0x0F51, 0x0017}, {0x02F5, 0x0048}, {0x02FA, 0xFFDF}, {0x02FB, 0xFFE0},
	{0xC456, 0x0C14}, {0xC476, 0x0C14}, {0xC454, 0x3F8B}, {0xC474, 0x3F8B},
	{0xC450, 0x2071}, {0xC470, 0x2071}, {0xC451, 0x226B}, {0xC471, 0x226B},
	{0xC452, 0xA293}, {0xC472, 0xA293}, {0xC44C, 0x1585}, {0xC44C, 0x1185},
	{0xC44C, 0x1585}, {0xC46C, 0x1585}, {0xC46C, 0x1185}, {0xC46C, 0x1585},
	{0xC44C, 0x0185}, {0xC44C, 0x0181}, {0xC44C, 0x0185}, {0xC46C, 0x0185},
	{0xC46C, 0x0181}, {0xC46C, 0x0185}, {0xBE24, 0xB000}, {0xBE23, 0xFF51},
	{0xBE22, 0xDF20}, {0xBE21, 0x0140}, {0xBE20, 0x00BB}, {0xBE24, 0xB800},
	{0xBE24, 0x0000}, {0xBE24, 0x7000}, {0xBE23, 0xFF51}, {0xBE22, 0xDF60},
	{0xBE21, 0x0140}, {0xBE20, 0x0077}, {0xBE24, 0x7800}, {0xBE24, 0x0000},
	{0xBE2E, 0x7BA7}, {0xBE36, 0x1000}, {0xBE37, 0x1000}, {0x8000, 0x0001},
	{0xBE69, 0xD50F}, {0x8000, 0x0000}, {0xBE69, 0xD50F}, {0xBE6B, 0x0320},
	{0xBE77, 0x2800}, {0xBE78, 0x3C3C}, {0xBE79, 0x3C3C}, {0xBE6E, 0xE120},
	{0x8000, 0x0001}, {0xBE10, 0x8140}, {0x8000, 0x0000}, {0xBE10, 0x8140},
	{0xBE15, 0x1007}, {0xBE14, 0x0448}, {0xBE1E, 0x00A0}, {0xBE10, 0x8160},
	{0xBE10, 0x8140}, {0xBE00, 0x1340}, {0x0450, 0x0000}, {0x0401, 0x0000},
};

/* Belkin F5D8235 v1, "belkin,f5d8235-v1" */
static const struct rtl8366rb_jam_tbl_entry rtl8366rb_init_jam_f5d8235[] = {
	{0x0242, 0x02BF}, {0x0245, 0x02BF}, {0x0248, 0x02BF}, {0x024B, 0x02BF},
	{0x024E, 0x02BF}, {0x0251, 0x02BF}, {0x0254, 0x0A3F}, {0x0256, 0x0A3F},
	{0x0258, 0x0A3F}, {0x025A, 0x0A3F}, {0x025C, 0x0A3F}, {0x025E, 0x0A3F},
	{0x0263, 0x007C}, {0x0100, 0x0004}, {0xBE5B, 0x3500}, {0x800E, 0x200F},
	{0xBE1D, 0x0F00}, {0x8001, 0x5011}, {0x800A, 0xA2F4}, {0x800B, 0x17A3},
	{0xBE4B, 0x17A3}, {0xBE41, 0x5011}, {0xBE17, 0x2100}, {0x8000, 0x8304},
	{0xBE40, 0x8304}, {0xBE4A, 0xA2F4}, {0x800C, 0xA8D5}, {0x8014, 0x5500},
	{0x8015, 0x0004}, {0xBE4C, 0xA8D5}, {0xBE59, 0x0008}, {0xBE09, 0x0E00},
	{0xBE36, 0x1036}, {0xBE37, 0x1036}, {0x800D, 0x00FF}, {0xBE4D, 0x00FF},
};

/* DGN3500, "netgear,dgn3500", "netgear,dgn3500b" */
static const struct rtl8366rb_jam_tbl_entry rtl8366rb_init_jam_dgn3500[] = {
	{0x0000, 0x0830}, {0x0400, 0x8130}, {0x000A, 0x83ED}, {0x0F51, 0x0017},
	{0x02F5, 0x0048}, {0x02FA, 0xFFDF}, {0x02FB, 0xFFE0}, {0x0450, 0x0000},
	{0x0401, 0x0000}, {0x0431, 0x0960},
};

/* This jam table activates "green ethernet", which means low power mode
 * and is claimed to detect the cable length and not use more power than
 * necessary, and the ports should enter power saving mode 10 seconds after
 * a cable is disconnected. Seems to always be the same.
 */
static const struct rtl8366rb_jam_tbl_entry rtl8366rb_green_jam[] = {
	{0xBE78, 0x323C}, {0xBE77, 0x5000}, {0xBE2E, 0x7BA7},
	{0xBE59, 0x3459}, {0xBE5A, 0x745A}, {0xBE5B, 0x785C},
	{0xBE5C, 0x785C}, {0xBE6E, 0xE120}, {0xBE79, 0x323C},
};

/* Function that jams the tables in the proper registers */
static int rtl8366rb_jam_table(const struct rtl8366rb_jam_tbl_entry *jam_table,
			       int jam_size, struct realtek_smi *smi,
			       bool write_dbg)
{
	u32 val;
	int ret;
	int i;

	for (i = 0; i < jam_size; i++) {
		if ((jam_table[i].reg & 0xBE00) == 0xBE00) {
			ret = regmap_read(smi->map,
					  RTL8366RB_PHY_ACCESS_BUSY_REG,
					  &val);
			if (ret)
				return ret;
			if (!(val & RTL8366RB_PHY_INT_BUSY)) {
				ret = regmap_write(smi->map,
						RTL8366RB_PHY_ACCESS_CTRL_REG,
						RTL8366RB_PHY_CTRL_WRITE);
				if (ret)
					return ret;
			}
		}
		if (write_dbg)
			dev_dbg(smi->dev, "jam %04x into register %04x\n",
				jam_table[i].val,
				jam_table[i].reg);
		ret = regmap_write(smi->map,
				   jam_table[i].reg,
				   jam_table[i].val);
		if (ret)
			return ret;
	}
	return 0;
}

static int rtl8366rb_setup(struct dsa_switch *ds)
{
	struct realtek_smi *smi = ds->priv;
	const struct rtl8366rb_jam_tbl_entry *jam_table;
	struct rtl8366rb *rb;
	u32 chip_ver = 0;
	u32 chip_id = 0;
	int jam_size;
	u32 val;
	int ret;
	int i;

	rb = smi->chip_data;

	ret = regmap_read(smi->map, RTL8366RB_CHIP_ID_REG, &chip_id);
	if (ret) {
		dev_err(smi->dev, "unable to read chip id\n");
		return ret;
	}

	switch (chip_id) {
	case RTL8366RB_CHIP_ID_8366:
		break;
	default:
		dev_err(smi->dev, "unknown chip id (%04x)\n", chip_id);
		return -ENODEV;
	}

	ret = regmap_read(smi->map, RTL8366RB_CHIP_VERSION_CTRL_REG,
			  &chip_ver);
	if (ret) {
		dev_err(smi->dev, "unable to read chip version\n");
		return ret;
	}

	dev_info(smi->dev, "RTL%04x ver %u chip found\n",
		 chip_id, chip_ver & RTL8366RB_CHIP_VERSION_MASK);

	/* Do the init dance using the right jam table */
	switch (chip_ver) {
	case 0:
		jam_table = rtl8366rb_init_jam_ver_0;
		jam_size = ARRAY_SIZE(rtl8366rb_init_jam_ver_0);
		break;
	case 1:
		jam_table = rtl8366rb_init_jam_ver_1;
		jam_size = ARRAY_SIZE(rtl8366rb_init_jam_ver_1);
		break;
	case 2:
		jam_table = rtl8366rb_init_jam_ver_2;
		jam_size = ARRAY_SIZE(rtl8366rb_init_jam_ver_2);
		break;
	default:
		jam_table = rtl8366rb_init_jam_ver_3;
		jam_size = ARRAY_SIZE(rtl8366rb_init_jam_ver_3);
		break;
	}

	/* Special jam tables for special routers
	 * TODO: are these necessary? Maintainers, please test
	 * without them, using just the off-the-shelf tables.
	 */
	if (of_machine_is_compatible("belkin,f5d8235-v1")) {
		jam_table = rtl8366rb_init_jam_f5d8235;
		jam_size = ARRAY_SIZE(rtl8366rb_init_jam_f5d8235);
	}
	if (of_machine_is_compatible("netgear,dgn3500") ||
	    of_machine_is_compatible("netgear,dgn3500b")) {
		jam_table = rtl8366rb_init_jam_dgn3500;
		jam_size = ARRAY_SIZE(rtl8366rb_init_jam_dgn3500);
	}

	ret = rtl8366rb_jam_table(jam_table, jam_size, smi, true);
	if (ret)
		return ret;

	/* Set up the "green ethernet" feature */
	ret = rtl8366rb_jam_table(rtl8366rb_green_jam,
				  ARRAY_SIZE(rtl8366rb_green_jam), smi, false);
	if (ret)
		return ret;

	ret = regmap_write(smi->map,
			   RTL8366RB_GREEN_FEATURE_REG,
			   (chip_ver == 1) ? 0x0007 : 0x0003);
	if (ret)
		return ret;

	/* Vendor driver sets 0x240 in registers 0xc and 0xd (undocumented) */
	ret = regmap_write(smi->map, 0x0c, 0x240);
	if (ret)
		return ret;
	ret = regmap_write(smi->map, 0x0d, 0x240);
	if (ret)
		return ret;

	/* Set some random MAC address */
	ret = rtl8366rb_set_addr(smi);
	if (ret)
		return ret;

	/* Enable CPU port with custom DSA tag 8899.
	 *
	 * If you set RTL8368RB_CPU_NO_TAG (bit 15) in this registers
	 * the custom tag is turned off.
	 */
	ret = regmap_update_bits(smi->map, RTL8368RB_CPU_CTRL_REG,
				 0xFFFF,
				 BIT(smi->cpu_port));
	if (ret)
		return ret;

	/* Make sure we default-enable the fixed CPU port */
	ret = regmap_update_bits(smi->map, RTL8366RB_PECR,
				 BIT(smi->cpu_port),
				 0);
	if (ret)
		return ret;

	/* Set maximum packet length to 1536 bytes */
	ret = regmap_update_bits(smi->map, RTL8366RB_SGCR,
				 RTL8366RB_SGCR_MAX_LENGTH_MASK,
				 RTL8366RB_SGCR_MAX_LENGTH_1536);
	if (ret)
		return ret;
	for (i = 0; i < RTL8366RB_NUM_PORTS; i++)
		/* layer 2 size, see rtl8366rb_change_mtu() */
		rb->max_mtu[i] = 1532;

	/* Enable learning for all ports */
	ret = regmap_write(smi->map, RTL8366RB_SSCR0, 0);
	if (ret)
		return ret;

	/* Enable auto ageing for all ports */
	ret = regmap_write(smi->map, RTL8366RB_SSCR1, 0);
	if (ret)
		return ret;

	/* Port 4 setup: this enables Port 4, usually the WAN port,
	 * common PHY IO mode is apparently mode 0, and this is not what
	 * the port is initialized to. There is no explanation of the
	 * IO modes in the Realtek source code, if your WAN port is
	 * connected to something exotic such as fiber, then this might
	 * be worth experimenting with.
	 */
	ret = regmap_update_bits(smi->map, RTL8366RB_PMC0,
				 RTL8366RB_PMC0_P4_IOMODE_MASK,
				 0 << RTL8366RB_PMC0_P4_IOMODE_SHIFT);
	if (ret)
		return ret;

	/* Discard VLAN tagged packets if the port is not a member of
	 * the VLAN with which the packets is associated.
	 */
	ret = regmap_write(smi->map, RTL8366RB_VLAN_INGRESS_CTRL2_REG,
			   RTL8366RB_PORT_ALL);
	if (ret)
		return ret;

	/* Don't drop packets whose DA has not been learned */
	ret = regmap_update_bits(smi->map, RTL8366RB_SSCR2,
				 RTL8366RB_SSCR2_DROP_UNKNOWN_DA, 0);
	if (ret)
		return ret;

	/* Set blinking, TODO: make this configurable */
	ret = regmap_update_bits(smi->map, RTL8366RB_LED_BLINKRATE_REG,
				 RTL8366RB_LED_BLINKRATE_MASK,
				 RTL8366RB_LED_BLINKRATE_56MS);
	if (ret)
		return ret;

	/* Set up LED activity:
	 * Each port has 4 LEDs, we configure all ports to the same
	 * behaviour (no individual config) but we can set up each
	 * LED separately.
	 */
	if (smi->leds_disabled) {
		/* Turn everything off */
		regmap_update_bits(smi->map,
				   RTL8366RB_LED_0_1_CTRL_REG,
				   0x0FFF, 0);
		regmap_update_bits(smi->map,
				   RTL8366RB_LED_2_3_CTRL_REG,
				   0x0FFF, 0);
		regmap_update_bits(smi->map,
				   RTL8366RB_INTERRUPT_CONTROL_REG,
				   RTL8366RB_P4_RGMII_LED,
				   0);
		val = RTL8366RB_LED_OFF;
	} else {
		/* TODO: make this configurable per LED */
		val = RTL8366RB_LED_FORCE;
	}
	for (i = 0; i < 4; i++) {
		ret = regmap_update_bits(smi->map,
					 RTL8366RB_LED_CTRL_REG,
					 0xf << (i * 4),
					 val << (i * 4));
		if (ret)
			return ret;
	}

	ret = rtl8366_init_vlan(smi);
	if (ret)
		return ret;

	ret = rtl8366rb_setup_cascaded_irq(smi);
	if (ret)
		dev_info(smi->dev, "no interrupt support\n");

	ret = realtek_smi_setup_mdio(smi);
	if (ret) {
		dev_info(smi->dev, "could not set up MDIO bus\n");
		return -ENODEV;
	}

	ds->configure_vlan_while_not_filtering = false;

	return 0;
}

static enum dsa_tag_protocol rtl8366_get_tag_protocol(struct dsa_switch *ds,
						      int port,
						      enum dsa_tag_protocol mp)
{
	/* This switch uses the 4 byte protocol A Realtek DSA tag */
	return DSA_TAG_PROTO_RTL4_A;
}

static void
rtl8366rb_mac_link_up(struct dsa_switch *ds, int port, unsigned int mode,
		      phy_interface_t interface, struct phy_device *phydev,
		      int speed, int duplex, bool tx_pause, bool rx_pause)
{
	struct realtek_smi *smi = ds->priv;
	int ret;

	if (port != smi->cpu_port)
		return;

	dev_dbg(smi->dev, "MAC link up on CPU port (%d)\n", port);

	/* Force the fixed CPU port into 1Gbit mode, no autonegotiation */
	ret = regmap_update_bits(smi->map, RTL8366RB_MAC_FORCE_CTRL_REG,
				 BIT(port), BIT(port));
	if (ret) {
		dev_err(smi->dev, "failed to force 1Gbit on CPU port\n");
		return;
	}

	ret = regmap_update_bits(smi->map, RTL8366RB_PAACR2,
				 0xFF00U,
				 RTL8366RB_PAACR_CPU_PORT << 8);
	if (ret) {
		dev_err(smi->dev, "failed to set PAACR on CPU port\n");
		return;
	}

	/* Enable the CPU port */
	ret = regmap_update_bits(smi->map, RTL8366RB_PECR, BIT(port),
				 0);
	if (ret) {
		dev_err(smi->dev, "failed to enable the CPU port\n");
		return;
	}
}

static void
rtl8366rb_mac_link_down(struct dsa_switch *ds, int port, unsigned int mode,
			phy_interface_t interface)
{
	struct realtek_smi *smi = ds->priv;
	int ret;

	if (port != smi->cpu_port)
		return;

	dev_dbg(smi->dev, "MAC link down on CPU port (%d)\n", port);

	/* Disable the CPU port */
	ret = regmap_update_bits(smi->map, RTL8366RB_PECR, BIT(port),
				 BIT(port));
	if (ret) {
		dev_err(smi->dev, "failed to disable the CPU port\n");
		return;
	}
}

static void rb8366rb_set_port_led(struct realtek_smi *smi,
				  int port, bool enable)
{
	u16 val = enable ? 0x3f : 0;
	int ret;

	if (smi->leds_disabled)
		return;

	switch (port) {
	case 0:
		ret = regmap_update_bits(smi->map,
					 RTL8366RB_LED_0_1_CTRL_REG,
					 0x3F, val);
		break;
	case 1:
		ret = regmap_update_bits(smi->map,
					 RTL8366RB_LED_0_1_CTRL_REG,
					 0x3F << RTL8366RB_LED_1_OFFSET,
					 val << RTL8366RB_LED_1_OFFSET);
		break;
	case 2:
		ret = regmap_update_bits(smi->map,
					 RTL8366RB_LED_2_3_CTRL_REG,
					 0x3F, val);
		break;
	case 3:
		ret = regmap_update_bits(smi->map,
					 RTL8366RB_LED_2_3_CTRL_REG,
					 0x3F << RTL8366RB_LED_3_OFFSET,
					 val << RTL8366RB_LED_3_OFFSET);
		break;
	case 4:
		ret = regmap_update_bits(smi->map,
					 RTL8366RB_INTERRUPT_CONTROL_REG,
					 RTL8366RB_P4_RGMII_LED,
					 enable ? RTL8366RB_P4_RGMII_LED : 0);
		break;
	default:
		dev_err(smi->dev, "no LED for port %d\n", port);
		return;
	}
	if (ret)
		dev_err(smi->dev, "error updating LED on port %d\n", port);
}

static int
rtl8366rb_port_enable(struct dsa_switch *ds, int port,
		      struct phy_device *phy)
{
	struct realtek_smi *smi = ds->priv;
	int ret;

	dev_dbg(smi->dev, "enable port %d\n", port);
	ret = regmap_update_bits(smi->map, RTL8366RB_PECR, BIT(port),
				 0);
	if (ret)
		return ret;

	rb8366rb_set_port_led(smi, port, true);
	return 0;
}

static void
rtl8366rb_port_disable(struct dsa_switch *ds, int port)
{
	struct realtek_smi *smi = ds->priv;
	int ret;

	dev_dbg(smi->dev, "disable port %d\n", port);
	ret = regmap_update_bits(smi->map, RTL8366RB_PECR, BIT(port),
				 BIT(port));
	if (ret)
		return;

	rb8366rb_set_port_led(smi, port, false);
}

static int rtl8366rb_change_mtu(struct dsa_switch *ds, int port, int new_mtu)
{
	struct realtek_smi *smi = ds->priv;
	struct rtl8366rb *rb;
	unsigned int max_mtu;
	u32 len;
	int i;

	/* Cache the per-port MTU setting */
	rb = smi->chip_data;
	rb->max_mtu[port] = new_mtu;

	/* Roof out the MTU for the entire switch to the greatest
	 * common denominator: the biggest set for any one port will
	 * be the biggest MTU for the switch.
	 *
	 * The first setting, 1522 bytes, is max IP packet 1500 bytes,
	 * plus ethernet header, 1518 bytes, plus CPU tag, 4 bytes.
	 * This function should consider the parameter an SDU, so the
	 * MTU passed for this setting is 1518 bytes. The same logic
	 * of subtracting the DSA tag of 4 bytes apply to the other
	 * settings.
	 */
	max_mtu = 1518;
	for (i = 0; i < RTL8366RB_NUM_PORTS; i++) {
		if (rb->max_mtu[i] > max_mtu)
			max_mtu = rb->max_mtu[i];
	}
	if (max_mtu <= 1518)
		len = RTL8366RB_SGCR_MAX_LENGTH_1522;
	else if (max_mtu > 1518 && max_mtu <= 1532)
		len = RTL8366RB_SGCR_MAX_LENGTH_1536;
	else if (max_mtu > 1532 && max_mtu <= 1548)
		len = RTL8366RB_SGCR_MAX_LENGTH_1552;
	else
		len = RTL8366RB_SGCR_MAX_LENGTH_16000;

	return regmap_update_bits(smi->map, RTL8366RB_SGCR,
				  RTL8366RB_SGCR_MAX_LENGTH_MASK,
				  len);
}

static int rtl8366rb_max_mtu(struct dsa_switch *ds, int port)
{
	/* The max MTU is 16000 bytes, so we subtract the CPU tag
	 * and the max presented to the system is 15996 bytes.
	 */
	return 15996;
}

static int rtl8366rb_get_vlan_4k(struct realtek_smi *smi, u32 vid,
				 struct rtl8366_vlan_4k *vlan4k)
{
	u32 data[3];
	int ret;
	int i;

	memset(vlan4k, '\0', sizeof(struct rtl8366_vlan_4k));

	if (vid >= RTL8366RB_NUM_VIDS)
		return -EINVAL;

	/* write VID */
	ret = regmap_write(smi->map, RTL8366RB_VLAN_TABLE_WRITE_BASE,
			   vid & RTL8366RB_VLAN_VID_MASK);
	if (ret)
		return ret;

	/* write table access control word */
	ret = regmap_write(smi->map, RTL8366RB_TABLE_ACCESS_CTRL_REG,
			   RTL8366RB_TABLE_VLAN_READ_CTRL);
	if (ret)
		return ret;

	for (i = 0; i < 3; i++) {
		ret = regmap_read(smi->map,
				  RTL8366RB_VLAN_TABLE_READ_BASE + i,
				  &data[i]);
		if (ret)
			return ret;
	}

	vlan4k->vid = vid;
	vlan4k->untag = (data[1] >> RTL8366RB_VLAN_UNTAG_SHIFT) &
			RTL8366RB_VLAN_UNTAG_MASK;
	vlan4k->member = data[1] & RTL8366RB_VLAN_MEMBER_MASK;
	vlan4k->fid = data[2] & RTL8366RB_VLAN_FID_MASK;

	return 0;
}

static int rtl8366rb_set_vlan_4k(struct realtek_smi *smi,
				 const struct rtl8366_vlan_4k *vlan4k)
{
	u32 data[3];
	int ret;
	int i;

	if (vlan4k->vid >= RTL8366RB_NUM_VIDS ||
	    vlan4k->member > RTL8366RB_VLAN_MEMBER_MASK ||
	    vlan4k->untag > RTL8366RB_VLAN_UNTAG_MASK ||
	    vlan4k->fid > RTL8366RB_FIDMAX)
		return -EINVAL;

	data[0] = vlan4k->vid & RTL8366RB_VLAN_VID_MASK;
	data[1] = (vlan4k->member & RTL8366RB_VLAN_MEMBER_MASK) |
		  ((vlan4k->untag & RTL8366RB_VLAN_UNTAG_MASK) <<
			RTL8366RB_VLAN_UNTAG_SHIFT);
	data[2] = vlan4k->fid & RTL8366RB_VLAN_FID_MASK;

	for (i = 0; i < 3; i++) {
		ret = regmap_write(smi->map,
				   RTL8366RB_VLAN_TABLE_WRITE_BASE + i,
				   data[i]);
		if (ret)
			return ret;
	}

	/* write table access control word */
	ret = regmap_write(smi->map, RTL8366RB_TABLE_ACCESS_CTRL_REG,
			   RTL8366RB_TABLE_VLAN_WRITE_CTRL);

	return ret;
}

static int rtl8366rb_get_vlan_mc(struct realtek_smi *smi, u32 index,
				 struct rtl8366_vlan_mc *vlanmc)
{
	u32 data[3];
	int ret;
	int i;

	memset(vlanmc, '\0', sizeof(struct rtl8366_vlan_mc));

	if (index >= RTL8366RB_NUM_VLANS)
		return -EINVAL;

	for (i = 0; i < 3; i++) {
		ret = regmap_read(smi->map,
				  RTL8366RB_VLAN_MC_BASE(index) + i,
				  &data[i]);
		if (ret)
			return ret;
	}

	vlanmc->vid = data[0] & RTL8366RB_VLAN_VID_MASK;
	vlanmc->priority = (data[0] >> RTL8366RB_VLAN_PRIORITY_SHIFT) &
		RTL8366RB_VLAN_PRIORITY_MASK;
	vlanmc->untag = (data[1] >> RTL8366RB_VLAN_UNTAG_SHIFT) &
		RTL8366RB_VLAN_UNTAG_MASK;
	vlanmc->member = data[1] & RTL8366RB_VLAN_MEMBER_MASK;
	vlanmc->fid = data[2] & RTL8366RB_VLAN_FID_MASK;

	return 0;
}

static int rtl8366rb_set_vlan_mc(struct realtek_smi *smi, u32 index,
				 const struct rtl8366_vlan_mc *vlanmc)
{
	u32 data[3];
	int ret;
	int i;

	if (index >= RTL8366RB_NUM_VLANS ||
	    vlanmc->vid >= RTL8366RB_NUM_VIDS ||
	    vlanmc->priority > RTL8366RB_PRIORITYMAX ||
	    vlanmc->member > RTL8366RB_VLAN_MEMBER_MASK ||
	    vlanmc->untag > RTL8366RB_VLAN_UNTAG_MASK ||
	    vlanmc->fid > RTL8366RB_FIDMAX)
		return -EINVAL;

	data[0] = (vlanmc->vid & RTL8366RB_VLAN_VID_MASK) |
		  ((vlanmc->priority & RTL8366RB_VLAN_PRIORITY_MASK) <<
			RTL8366RB_VLAN_PRIORITY_SHIFT);
	data[1] = (vlanmc->member & RTL8366RB_VLAN_MEMBER_MASK) |
		  ((vlanmc->untag & RTL8366RB_VLAN_UNTAG_MASK) <<
			RTL8366RB_VLAN_UNTAG_SHIFT);
	data[2] = vlanmc->fid & RTL8366RB_VLAN_FID_MASK;

	for (i = 0; i < 3; i++) {
		ret = regmap_write(smi->map,
				   RTL8366RB_VLAN_MC_BASE(index) + i,
				   data[i]);
		if (ret)
			return ret;
	}

	return 0;
}

static int rtl8366rb_get_mc_index(struct realtek_smi *smi, int port, int *val)
{
	u32 data;
	int ret;

	if (port >= smi->num_ports)
		return -EINVAL;

	ret = regmap_read(smi->map, RTL8366RB_PORT_VLAN_CTRL_REG(port),
			  &data);
	if (ret)
		return ret;

	*val = (data >> RTL8366RB_PORT_VLAN_CTRL_SHIFT(port)) &
		RTL8366RB_PORT_VLAN_CTRL_MASK;

	return 0;
}

static int rtl8366rb_set_mc_index(struct realtek_smi *smi, int port, int index)
{
	if (port >= smi->num_ports || index >= RTL8366RB_NUM_VLANS)
		return -EINVAL;

	return regmap_update_bits(smi->map, RTL8366RB_PORT_VLAN_CTRL_REG(port),
				RTL8366RB_PORT_VLAN_CTRL_MASK <<
					RTL8366RB_PORT_VLAN_CTRL_SHIFT(port),
				(index & RTL8366RB_PORT_VLAN_CTRL_MASK) <<
					RTL8366RB_PORT_VLAN_CTRL_SHIFT(port));
}

static bool rtl8366rb_is_vlan_valid(struct realtek_smi *smi, unsigned int vlan)
{
	unsigned int max = RTL8366RB_NUM_VLANS;

	if (smi->vlan4k_enabled)
		max = RTL8366RB_NUM_VIDS - 1;

	if (vlan == 0 || vlan > max)
		return false;

	return true;
}

static int rtl8366rb_enable_vlan(struct realtek_smi *smi, bool enable)
{
	dev_dbg(smi->dev, "%s VLAN\n", enable ? "enable" : "disable");
	return regmap_update_bits(smi->map,
				  RTL8366RB_SGCR, RTL8366RB_SGCR_EN_VLAN,
				  enable ? RTL8366RB_SGCR_EN_VLAN : 0);
}

static int rtl8366rb_enable_vlan4k(struct realtek_smi *smi, bool enable)
{
	dev_dbg(smi->dev, "%s VLAN 4k\n", enable ? "enable" : "disable");
	return regmap_update_bits(smi->map, RTL8366RB_SGCR,
				  RTL8366RB_SGCR_EN_VLAN_4KTB,
				  enable ? RTL8366RB_SGCR_EN_VLAN_4KTB : 0);
}

static int rtl8366rb_phy_read(struct realtek_smi *smi, int phy, int regnum)
{
	u32 val;
	u32 reg;
	int ret;

	if (phy > RTL8366RB_PHY_NO_MAX)
		return -EINVAL;

	ret = regmap_write(smi->map, RTL8366RB_PHY_ACCESS_CTRL_REG,
			   RTL8366RB_PHY_CTRL_READ);
	if (ret)
		return ret;

	reg = 0x8000 | (1 << (phy + RTL8366RB_PHY_NO_OFFSET)) | regnum;

	ret = regmap_write(smi->map, reg, 0);
	if (ret) {
		dev_err(smi->dev,
			"failed to write PHY%d reg %04x @ %04x, ret %d\n",
			phy, regnum, reg, ret);
		return ret;
	}

	ret = regmap_read(smi->map, RTL8366RB_PHY_ACCESS_DATA_REG, &val);
	if (ret)
		return ret;

	dev_dbg(smi->dev, "read PHY%d register 0x%04x @ %08x, val <- %04x\n",
		phy, regnum, reg, val);

	return val;
}

static int rtl8366rb_phy_write(struct realtek_smi *smi, int phy, int regnum,
			       u16 val)
{
	u32 reg;
	int ret;

	if (phy > RTL8366RB_PHY_NO_MAX)
		return -EINVAL;

	ret = regmap_write(smi->map, RTL8366RB_PHY_ACCESS_CTRL_REG,
			   RTL8366RB_PHY_CTRL_WRITE);
	if (ret)
		return ret;

	reg = 0x8000 | (1 << (phy + RTL8366RB_PHY_NO_OFFSET)) | regnum;

	dev_dbg(smi->dev, "write PHY%d register 0x%04x @ %04x, val -> %04x\n",
		phy, regnum, reg, val);

	ret = regmap_write(smi->map, reg, val);
	if (ret)
		return ret;

	return 0;
}

static int rtl8366rb_reset_chip(struct realtek_smi *smi)
{
	int timeout = 10;
	u32 val;
	int ret;

	realtek_smi_write_reg_noack(smi, RTL8366RB_RESET_CTRL_REG,
				    RTL8366RB_CHIP_CTRL_RESET_HW);
	do {
		usleep_range(20000, 25000);
		ret = regmap_read(smi->map, RTL8366RB_RESET_CTRL_REG, &val);
		if (ret)
			return ret;

		if (!(val & RTL8366RB_CHIP_CTRL_RESET_HW))
			break;
	} while (--timeout);

	if (!timeout) {
		dev_err(smi->dev, "timeout waiting for the switch to reset\n");
		return -EIO;
	}

	return 0;
}

static int rtl8366rb_detect(struct realtek_smi *smi)
{
	struct device *dev = smi->dev;
	int ret;
	u32 val;

	/* Detect device */
	ret = regmap_read(smi->map, 0x5c, &val);
	if (ret) {
		dev_err(dev, "can't get chip ID (%d)\n", ret);
		return ret;
	}

	switch (val) {
	case 0x6027:
		dev_info(dev, "found an RTL8366S switch\n");
		dev_err(dev, "this switch is not yet supported, submit patches!\n");
		return -ENODEV;
	case 0x5937:
		dev_info(dev, "found an RTL8366RB switch\n");
		smi->cpu_port = RTL8366RB_PORT_NUM_CPU;
		smi->num_ports = RTL8366RB_NUM_PORTS;
		smi->num_vlan_mc = RTL8366RB_NUM_VLANS;
		smi->mib_counters = rtl8366rb_mib_counters;
		smi->num_mib_counters = ARRAY_SIZE(rtl8366rb_mib_counters);
		break;
	default:
		dev_info(dev, "found an Unknown Realtek switch (id=0x%04x)\n",
			 val);
		break;
	}

	ret = rtl8366rb_reset_chip(smi);
	if (ret)
		return ret;

	return 0;
}

static const struct dsa_switch_ops rtl8366rb_switch_ops = {
	.get_tag_protocol = rtl8366_get_tag_protocol,
	.setup = rtl8366rb_setup,
	.phylink_mac_link_up = rtl8366rb_mac_link_up,
	.phylink_mac_link_down = rtl8366rb_mac_link_down,
	.get_strings = rtl8366_get_strings,
	.get_ethtool_stats = rtl8366_get_ethtool_stats,
	.get_sset_count = rtl8366_get_sset_count,
	.port_vlan_filtering = rtl8366_vlan_filtering,
	.port_vlan_add = rtl8366_vlan_add,
	.port_vlan_del = rtl8366_vlan_del,
	.port_enable = rtl8366rb_port_enable,
	.port_disable = rtl8366rb_port_disable,
	.port_change_mtu = rtl8366rb_change_mtu,
	.port_max_mtu = rtl8366rb_max_mtu,
};

static const struct realtek_smi_ops rtl8366rb_smi_ops = {
	.detect		= rtl8366rb_detect,
	.get_vlan_mc	= rtl8366rb_get_vlan_mc,
	.set_vlan_mc	= rtl8366rb_set_vlan_mc,
	.get_vlan_4k	= rtl8366rb_get_vlan_4k,
	.set_vlan_4k	= rtl8366rb_set_vlan_4k,
	.get_mc_index	= rtl8366rb_get_mc_index,
	.set_mc_index	= rtl8366rb_set_mc_index,
	.get_mib_counter = rtl8366rb_get_mib_counter,
	.is_vlan_valid	= rtl8366rb_is_vlan_valid,
	.enable_vlan	= rtl8366rb_enable_vlan,
	.enable_vlan4k	= rtl8366rb_enable_vlan4k,
	.phy_read	= rtl8366rb_phy_read,
	.phy_write	= rtl8366rb_phy_write,
};

const struct realtek_smi_variant rtl8366rb_variant = {
	.ds_ops = &rtl8366rb_switch_ops,
	.ops = &rtl8366rb_smi_ops,
	.clk_delay = 10,
	.cmd_read = 0xa9,
	.cmd_write = 0xa8,
	.chip_data_sz = sizeof(struct rtl8366rb),
};
EXPORT_SYMBOL_GPL(rtl8366rb_variant);
