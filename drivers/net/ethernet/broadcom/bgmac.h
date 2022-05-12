/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _BGMAC_H
#define _BGMAC_H

#include <linux/netdevice.h>

#include "unimac.h"

#define BGMAC_DEV_CTL				0x000
#define  BGMAC_DC_TSM				0x00000002
#define  BGMAC_DC_CFCO				0x00000004
#define  BGMAC_DC_RLSS				0x00000008
#define  BGMAC_DC_MROR				0x00000010
#define  BGMAC_DC_FCM_MASK			0x00000060
#define  BGMAC_DC_FCM_SHIFT			5
#define  BGMAC_DC_NAE				0x00000080
#define  BGMAC_DC_TF				0x00000100
#define  BGMAC_DC_RDS_MASK			0x00030000
#define  BGMAC_DC_RDS_SHIFT			16
#define  BGMAC_DC_TDS_MASK			0x000c0000
#define  BGMAC_DC_TDS_SHIFT			18
#define BGMAC_DEV_STATUS			0x004		/* Configuration of the interface */
#define  BGMAC_DS_RBF				0x00000001
#define  BGMAC_DS_RDF				0x00000002
#define  BGMAC_DS_RIF				0x00000004
#define  BGMAC_DS_TBF				0x00000008
#define  BGMAC_DS_TDF				0x00000010
#define  BGMAC_DS_TIF				0x00000020
#define  BGMAC_DS_PO				0x00000040
#define  BGMAC_DS_MM_MASK			0x00000300	/* Mode of the interface */
#define  BGMAC_DS_MM_SHIFT			8
#define BGMAC_BIST_STATUS			0x00c
#define BGMAC_INT_STATUS			0x020		/* Interrupt status */
#define  BGMAC_IS_MRO				0x00000001
#define  BGMAC_IS_MTO				0x00000002
#define  BGMAC_IS_TFD				0x00000004
#define  BGMAC_IS_LS				0x00000008
#define  BGMAC_IS_MDIO				0x00000010
#define  BGMAC_IS_MR				0x00000020
#define  BGMAC_IS_MT				0x00000040
#define  BGMAC_IS_TO				0x00000080
#define  BGMAC_IS_DESC_ERR			0x00000400	/* Descriptor error */
#define  BGMAC_IS_DATA_ERR			0x00000800	/* Data error */
#define  BGMAC_IS_DESC_PROT_ERR			0x00001000	/* Descriptor protocol error */
#define  BGMAC_IS_RX_DESC_UNDERF		0x00002000	/* Receive descriptor underflow */
#define  BGMAC_IS_RX_F_OVERF			0x00004000	/* Receive FIFO overflow */
#define  BGMAC_IS_TX_F_UNDERF			0x00008000	/* Transmit FIFO underflow */
#define  BGMAC_IS_RX				0x00010000	/* Interrupt for RX queue 0 */
#define  BGMAC_IS_TX0				0x01000000	/* Interrupt for TX queue 0 */
#define  BGMAC_IS_TX1				0x02000000	/* Interrupt for TX queue 1 */
#define  BGMAC_IS_TX2				0x04000000	/* Interrupt for TX queue 2 */
#define  BGMAC_IS_TX3				0x08000000	/* Interrupt for TX queue 3 */
#define  BGMAC_IS_TX_MASK			0x0f000000
#define  BGMAC_IS_INTMASK			0x0f01fcff
#define  BGMAC_IS_ERRMASK			0x0000fc00
#define BGMAC_INT_MASK				0x024		/* Interrupt mask */
#define BGMAC_GP_TIMER				0x028
#define BGMAC_INT_RECV_LAZY			0x100
#define  BGMAC_IRL_TO_MASK			0x00ffffff
#define  BGMAC_IRL_FC_MASK			0xff000000
#define  BGMAC_IRL_FC_SHIFT			24		/* Shift the number of interrupts triggered per received frame */
#define BGMAC_FLOW_CTL_THRESH			0x104		/* Flow control thresholds */
#define BGMAC_WRRTHRESH				0x108
#define BGMAC_GMAC_IDLE_CNT_THRESH		0x10c
#define BGMAC_PHY_ACCESS			0x180		/* PHY access address */
#define  BGMAC_PA_DATA_MASK			0x0000ffff
#define  BGMAC_PA_ADDR_MASK			0x001f0000
#define  BGMAC_PA_ADDR_SHIFT			16
#define  BGMAC_PA_REG_MASK			0x1f000000
#define  BGMAC_PA_REG_SHIFT			24
#define  BGMAC_PA_WRITE				0x20000000
#define  BGMAC_PA_START				0x40000000
#define BGMAC_PHY_CNTL				0x188		/* PHY control address */
#define  BGMAC_PC_EPA_MASK			0x0000001f
#define  BGMAC_PC_MCT_MASK			0x007f0000
#define  BGMAC_PC_MCT_SHIFT			16
#define  BGMAC_PC_MTE				0x00800000
#define BGMAC_TXQ_CTL				0x18c
#define  BGMAC_TXQ_CTL_DBT_MASK			0x00000fff
#define  BGMAC_TXQ_CTL_DBT_SHIFT		0
#define BGMAC_RXQ_CTL				0x190
#define  BGMAC_RXQ_CTL_DBT_MASK			0x00000fff
#define  BGMAC_RXQ_CTL_DBT_SHIFT		0
#define  BGMAC_RXQ_CTL_PTE			0x00001000
#define  BGMAC_RXQ_CTL_MDP_MASK			0x3f000000
#define  BGMAC_RXQ_CTL_MDP_SHIFT		24
#define BGMAC_GPIO_SELECT			0x194
#define BGMAC_GPIO_OUTPUT_EN			0x198

/* For 0x1e0 see BCMA_CLKCTLST. Below are BGMAC specific bits */
#define  BGMAC_BCMA_CLKCTLST_MISC_PLL_REQ	0x00000100
#define  BGMAC_BCMA_CLKCTLST_MISC_PLL_ST	0x01000000

#define BGMAC_HW_WAR				0x1e4
#define BGMAC_PWR_CTL				0x1e8
#define BGMAC_DMA_BASE0				0x200		/* Tx and Rx controller */
#define BGMAC_DMA_BASE1				0x240		/* Tx controller only */
#define BGMAC_DMA_BASE2				0x280		/* Tx controller only */
#define BGMAC_DMA_BASE3				0x2C0		/* Tx controller only */
#define BGMAC_TX_GOOD_OCTETS			0x300
#define BGMAC_TX_GOOD_OCTETS_HIGH		0x304
#define BGMAC_TX_GOOD_PKTS			0x308
#define BGMAC_TX_OCTETS				0x30c
#define BGMAC_TX_OCTETS_HIGH			0x310
#define BGMAC_TX_PKTS				0x314
#define BGMAC_TX_BROADCAST_PKTS			0x318
#define BGMAC_TX_MULTICAST_PKTS			0x31c
#define BGMAC_TX_LEN_64				0x320
#define BGMAC_TX_LEN_65_TO_127			0x324
#define BGMAC_TX_LEN_128_TO_255			0x328
#define BGMAC_TX_LEN_256_TO_511			0x32c
#define BGMAC_TX_LEN_512_TO_1023		0x330
#define BGMAC_TX_LEN_1024_TO_1522		0x334
#define BGMAC_TX_LEN_1523_TO_2047		0x338
#define BGMAC_TX_LEN_2048_TO_4095		0x33c
#define BGMAC_TX_LEN_4096_TO_8191		0x340
#define BGMAC_TX_LEN_8192_TO_MAX		0x344
#define BGMAC_TX_JABBER_PKTS			0x348		/* Error */
#define BGMAC_TX_OVERSIZE_PKTS			0x34c		/* Error */
#define BGMAC_TX_FRAGMENT_PKTS			0x350
#define BGMAC_TX_UNDERRUNS			0x354		/* Error */
#define BGMAC_TX_TOTAL_COLS			0x358
#define BGMAC_TX_SINGLE_COLS			0x35c
#define BGMAC_TX_MULTIPLE_COLS			0x360
#define BGMAC_TX_EXCESSIVE_COLS			0x364		/* Error */
#define BGMAC_TX_LATE_COLS			0x368		/* Error */
#define BGMAC_TX_DEFERED			0x36c
#define BGMAC_TX_CARRIER_LOST			0x370
#define BGMAC_TX_PAUSE_PKTS			0x374
#define BGMAC_TX_UNI_PKTS			0x378
#define BGMAC_TX_Q0_PKTS			0x37c
#define BGMAC_TX_Q0_OCTETS			0x380
#define BGMAC_TX_Q0_OCTETS_HIGH			0x384
#define BGMAC_TX_Q1_PKTS			0x388
#define BGMAC_TX_Q1_OCTETS			0x38c
#define BGMAC_TX_Q1_OCTETS_HIGH			0x390
#define BGMAC_TX_Q2_PKTS			0x394
#define BGMAC_TX_Q2_OCTETS			0x398
#define BGMAC_TX_Q2_OCTETS_HIGH			0x39c
#define BGMAC_TX_Q3_PKTS			0x3a0
#define BGMAC_TX_Q3_OCTETS			0x3a4
#define BGMAC_TX_Q3_OCTETS_HIGH			0x3a8
#define BGMAC_RX_GOOD_OCTETS			0x3b0
#define BGMAC_RX_GOOD_OCTETS_HIGH		0x3b4
#define BGMAC_RX_GOOD_PKTS			0x3b8
#define BGMAC_RX_OCTETS				0x3bc
#define BGMAC_RX_OCTETS_HIGH			0x3c0
#define BGMAC_RX_PKTS				0x3c4
#define BGMAC_RX_BROADCAST_PKTS			0x3c8
#define BGMAC_RX_MULTICAST_PKTS			0x3cc
#define BGMAC_RX_LEN_64				0x3d0
#define BGMAC_RX_LEN_65_TO_127			0x3d4
#define BGMAC_RX_LEN_128_TO_255			0x3d8
#define BGMAC_RX_LEN_256_TO_511			0x3dc
#define BGMAC_RX_LEN_512_TO_1023		0x3e0
#define BGMAC_RX_LEN_1024_TO_1522		0x3e4
#define BGMAC_RX_LEN_1523_TO_2047		0x3e8
#define BGMAC_RX_LEN_2048_TO_4095		0x3ec
#define BGMAC_RX_LEN_4096_TO_8191		0x3f0
#define BGMAC_RX_LEN_8192_TO_MAX		0x3f4
#define BGMAC_RX_JABBER_PKTS			0x3f8		/* Error */
#define BGMAC_RX_OVERSIZE_PKTS			0x3fc		/* Error */
#define BGMAC_RX_FRAGMENT_PKTS			0x400
#define BGMAC_RX_MISSED_PKTS			0x404		/* Error */
#define BGMAC_RX_CRC_ALIGN_ERRS			0x408		/* Error */
#define BGMAC_RX_UNDERSIZE			0x40c		/* Error */
#define BGMAC_RX_CRC_ERRS			0x410		/* Error */
#define BGMAC_RX_ALIGN_ERRS			0x414		/* Error */
#define BGMAC_RX_SYMBOL_ERRS			0x418		/* Error */
#define BGMAC_RX_PAUSE_PKTS			0x41c
#define BGMAC_RX_NONPAUSE_PKTS			0x420
#define BGMAC_RX_SACHANGES			0x424
#define BGMAC_RX_UNI_PKTS			0x428
#define BGMAC_UNIMAC				0x800

/* BCMA GMAC core specific IO Control (BCMA_IOCTL) flags */
#define BGMAC_BCMA_IOCTL_SW_CLKEN		0x00000004	/* PHY Clock Enable */
#define BGMAC_BCMA_IOCTL_SW_RESET		0x00000008	/* PHY Reset */
/* The IOCTL values appear to be different in NS, NSP, and NS2, and do not match
 * the values directly above
 */
#define BGMAC_CLK_EN				BIT(0)
#define BGMAC_RESERVED_0			BIT(1)
#define BGMAC_SOURCE_SYNC_MODE_EN		BIT(2)
#define BGMAC_DEST_SYNC_MODE_EN			BIT(3)
#define BGMAC_TX_CLK_OUT_INVERT_EN		BIT(4)
#define BGMAC_DIRECT_GMII_MODE			BIT(5)
#define BGMAC_CLK_250_SEL			BIT(6)
#define BGMAC_AWCACHE				(0xf << 7)
#define BGMAC_RESERVED_1			(0x1f << 11)
#define BGMAC_ARCACHE				(0xf << 16)
#define BGMAC_AWUSER				(0x3f << 20)
#define BGMAC_ARUSER				(0x3f << 26)
#define BGMAC_RESERVED				BIT(31)

/* BCMA GMAC core specific IO status (BCMA_IOST) flags */
#define BGMAC_BCMA_IOST_ATTACHED		0x00000800

#define BGMAC_NUM_MIB_TX_REGS	\
		(((BGMAC_TX_Q3_OCTETS_HIGH - BGMAC_TX_GOOD_OCTETS) / 4) + 1)
#define BGMAC_NUM_MIB_RX_REGS	\
		(((BGMAC_RX_UNI_PKTS - BGMAC_RX_GOOD_OCTETS) / 4) + 1)

#define BGMAC_DMA_TX_CTL			0x00
#define  BGMAC_DMA_TX_ENABLE			0x00000001
#define  BGMAC_DMA_TX_SUSPEND			0x00000002
#define  BGMAC_DMA_TX_LOOPBACK			0x00000004
#define  BGMAC_DMA_TX_FLUSH			0x00000010
#define  BGMAC_DMA_TX_MR_MASK			0x000000C0	/* Multiple outstanding reads */
#define  BGMAC_DMA_TX_MR_SHIFT			6
#define   BGMAC_DMA_TX_MR_1			0
#define   BGMAC_DMA_TX_MR_2			1
#define  BGMAC_DMA_TX_PARITY_DISABLE		0x00000800
#define  BGMAC_DMA_TX_ADDREXT_MASK		0x00030000
#define  BGMAC_DMA_TX_ADDREXT_SHIFT		16
#define  BGMAC_DMA_TX_BL_MASK			0x001C0000	/* BurstLen bits */
#define  BGMAC_DMA_TX_BL_SHIFT			18
#define   BGMAC_DMA_TX_BL_16			0
#define   BGMAC_DMA_TX_BL_32			1
#define   BGMAC_DMA_TX_BL_64			2
#define   BGMAC_DMA_TX_BL_128			3
#define   BGMAC_DMA_TX_BL_256			4
#define   BGMAC_DMA_TX_BL_512			5
#define   BGMAC_DMA_TX_BL_1024			6
#define  BGMAC_DMA_TX_PC_MASK			0x00E00000	/* Prefetch control */
#define  BGMAC_DMA_TX_PC_SHIFT			21
#define   BGMAC_DMA_TX_PC_0			0
#define   BGMAC_DMA_TX_PC_4			1
#define   BGMAC_DMA_TX_PC_8			2
#define   BGMAC_DMA_TX_PC_16			3
#define  BGMAC_DMA_TX_PT_MASK			0x03000000	/* Prefetch threshold */
#define  BGMAC_DMA_TX_PT_SHIFT			24
#define   BGMAC_DMA_TX_PT_1			0
#define   BGMAC_DMA_TX_PT_2			1
#define   BGMAC_DMA_TX_PT_4			2
#define   BGMAC_DMA_TX_PT_8			3
#define BGMAC_DMA_TX_INDEX			0x04
#define BGMAC_DMA_TX_RINGLO			0x08
#define BGMAC_DMA_TX_RINGHI			0x0C
#define BGMAC_DMA_TX_STATUS			0x10
#define  BGMAC_DMA_TX_STATDPTR			0x00001FFF
#define  BGMAC_DMA_TX_STAT			0xF0000000
#define   BGMAC_DMA_TX_STAT_DISABLED		0x00000000
#define   BGMAC_DMA_TX_STAT_ACTIVE		0x10000000
#define   BGMAC_DMA_TX_STAT_IDLEWAIT		0x20000000
#define   BGMAC_DMA_TX_STAT_STOPPED		0x30000000
#define   BGMAC_DMA_TX_STAT_SUSP		0x40000000
#define BGMAC_DMA_TX_ERROR			0x14
#define  BGMAC_DMA_TX_ERRDPTR			0x0001FFFF
#define  BGMAC_DMA_TX_ERR			0xF0000000
#define   BGMAC_DMA_TX_ERR_NOERR		0x00000000
#define   BGMAC_DMA_TX_ERR_PROT			0x10000000
#define   BGMAC_DMA_TX_ERR_UNDERRUN		0x20000000
#define   BGMAC_DMA_TX_ERR_TRANSFER		0x30000000
#define   BGMAC_DMA_TX_ERR_DESCREAD		0x40000000
#define   BGMAC_DMA_TX_ERR_CORE			0x50000000
#define BGMAC_DMA_RX_CTL			0x20
#define  BGMAC_DMA_RX_ENABLE			0x00000001
#define  BGMAC_DMA_RX_FRAME_OFFSET_MASK		0x000000FE
#define  BGMAC_DMA_RX_FRAME_OFFSET_SHIFT	1
#define  BGMAC_DMA_RX_DIRECT_FIFO		0x00000100
#define  BGMAC_DMA_RX_OVERFLOW_CONT		0x00000400
#define  BGMAC_DMA_RX_PARITY_DISABLE		0x00000800
#define  BGMAC_DMA_RX_MR_MASK			0x000000C0	/* Multiple outstanding reads */
#define  BGMAC_DMA_RX_MR_SHIFT			6
#define   BGMAC_DMA_TX_MR_1			0
#define   BGMAC_DMA_TX_MR_2			1
#define  BGMAC_DMA_RX_ADDREXT_MASK		0x00030000
#define  BGMAC_DMA_RX_ADDREXT_SHIFT		16
#define  BGMAC_DMA_RX_BL_MASK			0x001C0000	/* BurstLen bits */
#define  BGMAC_DMA_RX_BL_SHIFT			18
#define   BGMAC_DMA_RX_BL_16			0
#define   BGMAC_DMA_RX_BL_32			1
#define   BGMAC_DMA_RX_BL_64			2
#define   BGMAC_DMA_RX_BL_128			3
#define   BGMAC_DMA_RX_BL_256			4
#define   BGMAC_DMA_RX_BL_512			5
#define   BGMAC_DMA_RX_BL_1024			6
#define  BGMAC_DMA_RX_PC_MASK			0x00E00000	/* Prefetch control */
#define  BGMAC_DMA_RX_PC_SHIFT			21
#define   BGMAC_DMA_RX_PC_0			0
#define   BGMAC_DMA_RX_PC_4			1
#define   BGMAC_DMA_RX_PC_8			2
#define   BGMAC_DMA_RX_PC_16			3
#define  BGMAC_DMA_RX_PT_MASK			0x03000000	/* Prefetch threshold */
#define  BGMAC_DMA_RX_PT_SHIFT			24
#define   BGMAC_DMA_RX_PT_1			0
#define   BGMAC_DMA_RX_PT_2			1
#define   BGMAC_DMA_RX_PT_4			2
#define   BGMAC_DMA_RX_PT_8			3
#define BGMAC_DMA_RX_INDEX			0x24
#define BGMAC_DMA_RX_RINGLO			0x28
#define BGMAC_DMA_RX_RINGHI			0x2C
#define BGMAC_DMA_RX_STATUS			0x30
#define  BGMAC_DMA_RX_STATDPTR			0x00001FFF
#define  BGMAC_DMA_RX_STAT			0xF0000000
#define   BGMAC_DMA_RX_STAT_DISABLED		0x00000000
#define   BGMAC_DMA_RX_STAT_ACTIVE		0x10000000
#define   BGMAC_DMA_RX_STAT_IDLEWAIT		0x20000000
#define   BGMAC_DMA_RX_STAT_STOPPED		0x30000000
#define   BGMAC_DMA_RX_STAT_SUSP		0x40000000
#define BGMAC_DMA_RX_ERROR			0x34
#define  BGMAC_DMA_RX_ERRDPTR			0x0001FFFF
#define  BGMAC_DMA_RX_ERR			0xF0000000
#define   BGMAC_DMA_RX_ERR_NOERR		0x00000000
#define   BGMAC_DMA_RX_ERR_PROT			0x10000000
#define   BGMAC_DMA_RX_ERR_UNDERRUN		0x20000000
#define   BGMAC_DMA_RX_ERR_TRANSFER		0x30000000
#define   BGMAC_DMA_RX_ERR_DESCREAD		0x40000000
#define   BGMAC_DMA_RX_ERR_CORE			0x50000000

#define BGMAC_DESC_CTL0_EOT			0x10000000	/* End of ring */
#define BGMAC_DESC_CTL0_IOC			0x20000000	/* IRQ on complete */
#define BGMAC_DESC_CTL0_EOF			0x40000000	/* End of frame */
#define BGMAC_DESC_CTL0_SOF			0x80000000	/* Start of frame */
#define BGMAC_DESC_CTL1_LEN			0x00003FFF

#define BGMAC_PHY_NOREGS			BRCM_PSEUDO_PHY_ADDR
#define BGMAC_PHY_MASK				0x1F

#define BGMAC_MAX_TX_RINGS			4
#define BGMAC_MAX_RX_RINGS			1

#define BGMAC_TX_RING_SLOTS			128
#define BGMAC_RX_RING_SLOTS			512

#define BGMAC_RX_HEADER_LEN			28		/* Last 24 bytes are unused. Well... */
#define BGMAC_RX_FRAME_OFFSET			30		/* There are 2 unused bytes between header and real data */
#define BGMAC_RX_BUF_OFFSET			(NET_SKB_PAD + NET_IP_ALIGN - \
						 BGMAC_RX_FRAME_OFFSET)
/* Jumbo frame size with FCS */
#define BGMAC_RX_MAX_FRAME_SIZE			9724
#define BGMAC_RX_BUF_SIZE			(BGMAC_RX_FRAME_OFFSET + BGMAC_RX_MAX_FRAME_SIZE)
#define BGMAC_RX_ALLOC_SIZE			(SKB_DATA_ALIGN(BGMAC_RX_BUF_SIZE + BGMAC_RX_BUF_OFFSET) + \
						 SKB_DATA_ALIGN(sizeof(struct skb_shared_info)))

#define BGMAC_BFL_ENETROBO			0x0010		/* has ephy roboswitch spi */
#define BGMAC_BFL_ENETADM			0x0080		/* has ADMtek switch */
#define BGMAC_BFL_ENETVLAN			0x0100		/* can do vlan */

#define BGMAC_CHIPCTL_1_IF_TYPE_MASK		0x00000030
#define BGMAC_CHIPCTL_1_IF_TYPE_RMII		0x00000000
#define BGMAC_CHIPCTL_1_IF_TYPE_MII		0x00000010
#define BGMAC_CHIPCTL_1_IF_TYPE_RGMII		0x00000020
#define BGMAC_CHIPCTL_1_SW_TYPE_MASK		0x000000C0
#define BGMAC_CHIPCTL_1_SW_TYPE_EPHY		0x00000000
#define BGMAC_CHIPCTL_1_SW_TYPE_EPHYMII		0x00000040
#define BGMAC_CHIPCTL_1_SW_TYPE_EPHYRMII	0x00000080
#define BGMAC_CHIPCTL_1_SW_TYPE_RGMII		0x000000C0
#define BGMAC_CHIPCTL_1_RXC_DLL_BYPASS		0x00010000

#define BGMAC_CHIPCTL_4_IF_TYPE_MASK		0x00003000
#define BGMAC_CHIPCTL_4_IF_TYPE_RMII		0x00000000
#define BGMAC_CHIPCTL_4_IF_TYPE_MII		0x00001000
#define BGMAC_CHIPCTL_4_IF_TYPE_RGMII		0x00002000
#define BGMAC_CHIPCTL_4_SW_TYPE_MASK		0x0000C000
#define BGMAC_CHIPCTL_4_SW_TYPE_EPHY		0x00000000
#define BGMAC_CHIPCTL_4_SW_TYPE_EPHYMII		0x00004000
#define BGMAC_CHIPCTL_4_SW_TYPE_EPHYRMII	0x00008000
#define BGMAC_CHIPCTL_4_SW_TYPE_RGMII		0x0000C000

#define BGMAC_CHIPCTL_7_IF_TYPE_MASK		0x000000C0
#define BGMAC_CHIPCTL_7_IF_TYPE_RMII		0x00000000
#define BGMAC_CHIPCTL_7_IF_TYPE_MII		0x00000040
#define BGMAC_CHIPCTL_7_IF_TYPE_RGMII		0x00000080

#define BGMAC_WEIGHT	64

#define ETHER_MAX_LEN	(ETH_FRAME_LEN + ETH_FCS_LEN)

/* Feature Flags */
#define BGMAC_FEAT_TX_MASK_SETUP	BIT(0)
#define BGMAC_FEAT_RX_MASK_SETUP	BIT(1)
#define BGMAC_FEAT_IOST_ATTACHED	BIT(2)
#define BGMAC_FEAT_NO_RESET		BIT(3)
#define BGMAC_FEAT_MISC_PLL_REQ		BIT(4)
#define BGMAC_FEAT_SW_TYPE_PHY		BIT(5)
#define BGMAC_FEAT_SW_TYPE_EPHYRMII	BIT(6)
#define BGMAC_FEAT_SW_TYPE_RGMII	BIT(7)
#define BGMAC_FEAT_CMN_PHY_CTL		BIT(8)
#define BGMAC_FEAT_FLW_CTRL1		BIT(9)
#define BGMAC_FEAT_FLW_CTRL2		BIT(10)
#define BGMAC_FEAT_SET_RXQ_CLK		BIT(11)
#define BGMAC_FEAT_CLKCTLST		BIT(12)
#define BGMAC_FEAT_NO_CLR_MIB		BIT(13)
#define BGMAC_FEAT_FORCE_SPEED_2500	BIT(14)
#define BGMAC_FEAT_CMDCFG_SR_REV4	BIT(15)
#define BGMAC_FEAT_IRQ_ID_OOB_6		BIT(16)
#define BGMAC_FEAT_CC4_IF_SW_TYPE	BIT(17)
#define BGMAC_FEAT_CC4_IF_SW_TYPE_RGMII	BIT(18)
#define BGMAC_FEAT_CC7_IF_TYPE_RGMII	BIT(19)
#define BGMAC_FEAT_IDM_MASK		BIT(20)

struct bgmac_slot_info {
	union {
		struct sk_buff *skb;
		void *buf;
	};
	dma_addr_t dma_addr;
};

struct bgmac_dma_desc {
	__le32 ctl0;
	__le32 ctl1;
	__le32 addr_low;
	__le32 addr_high;
} __packed;

enum bgmac_dma_ring_type {
	BGMAC_DMA_RING_TX,
	BGMAC_DMA_RING_RX,
};

/**
 * bgmac_dma_ring - contains info about DMA ring (either TX or RX one)
 * @start: index of the first slot containing data
 * @end: index of a slot that can *not* be read (yet)
 *
 * Be really aware of the specific @end meaning. It's an index of a slot *after*
 * the one containing data that can be read. If @start equals @end the ring is
 * empty.
 */
struct bgmac_dma_ring {
	u32 start;
	u32 end;

	struct bgmac_dma_desc *cpu_base;
	dma_addr_t dma_base;
	u32 index_base; /* Used for unaligned rings only, otherwise 0 */
	u16 mmio_base;
	bool unaligned;

	struct bgmac_slot_info slots[BGMAC_RX_RING_SLOTS];
};

struct bgmac_rx_header {
	__le16 len;
	__le16 flags;
	__le16 pad[12];
};

struct bgmac {
	union {
		struct {
			void __iomem *base;
			void __iomem *idm_base;
			void __iomem *nicpm_base;
		} plat;
		struct {
			struct bcma_device *core;
			/* Reference to CMN core for BCM4706 */
			struct bcma_device *cmn;
		} bcma;
	};

	struct device *dev;
	struct device *dma_dev;
	u32 feature_flags;

	struct net_device *net_dev;
	struct napi_struct napi;
	struct mii_bus *mii_bus;

	/* DMA */
	struct bgmac_dma_ring tx_ring[BGMAC_MAX_TX_RINGS];
	struct bgmac_dma_ring rx_ring[BGMAC_MAX_RX_RINGS];

	/* Stats */
	bool stats_grabbed;
	u32 mib_tx_regs[BGMAC_NUM_MIB_TX_REGS];
	u32 mib_rx_regs[BGMAC_NUM_MIB_RX_REGS];

	/* Int */
	int irq;
	u32 int_mask;

	/* Current MAC state */
	int mac_speed;
	int mac_duplex;

	u8 phyaddr;
	bool has_robosw;

	bool loopback;

	u32 (*read)(struct bgmac *bgmac, u16 offset);
	void (*write)(struct bgmac *bgmac, u16 offset, u32 value);
	u32 (*idm_read)(struct bgmac *bgmac, u16 offset);
	void (*idm_write)(struct bgmac *bgmac, u16 offset, u32 value);
	bool (*clk_enabled)(struct bgmac *bgmac);
	void (*clk_enable)(struct bgmac *bgmac, u32 flags);
	void (*cco_ctl_maskset)(struct bgmac *bgmac, u32 offset, u32 mask,
				u32 set);
	u32 (*get_bus_clock)(struct bgmac *bgmac);
	void (*cmn_maskset32)(struct bgmac *bgmac, u16 offset, u32 mask,
			      u32 set);
	int (*phy_connect)(struct bgmac *bgmac);
};

struct bgmac *bgmac_alloc(struct device *dev);
int bgmac_enet_probe(struct bgmac *bgmac);
void bgmac_enet_remove(struct bgmac *bgmac);
void bgmac_adjust_link(struct net_device *net_dev);
int bgmac_phy_connect_direct(struct bgmac *bgmac);
int bgmac_enet_suspend(struct bgmac *bgmac);
int bgmac_enet_resume(struct bgmac *bgmac);

struct mii_bus *bcma_mdio_mii_register(struct bgmac *bgmac);
void bcma_mdio_mii_unregister(struct mii_bus *mii_bus);

static inline u32 bgmac_read(struct bgmac *bgmac, u16 offset)
{
	return bgmac->read(bgmac, offset);
}

static inline void bgmac_write(struct bgmac *bgmac, u16 offset, u32 value)
{
	bgmac->write(bgmac, offset, value);
}

static inline u32 bgmac_umac_read(struct bgmac *bgmac, u16 offset)
{
	return bgmac_read(bgmac, BGMAC_UNIMAC + offset);
}

static inline void bgmac_umac_write(struct bgmac *bgmac, u16 offset, u32 value)
{
	bgmac_write(bgmac, BGMAC_UNIMAC + offset, value);
}

static inline u32 bgmac_idm_read(struct bgmac *bgmac, u16 offset)
{
	return bgmac->idm_read(bgmac, offset);
}

static inline void bgmac_idm_write(struct bgmac *bgmac, u16 offset, u32 value)
{
	bgmac->idm_write(bgmac, offset, value);
}

static inline bool bgmac_clk_enabled(struct bgmac *bgmac)
{
	return bgmac->clk_enabled(bgmac);
}

static inline void bgmac_clk_enable(struct bgmac *bgmac, u32 flags)
{
	bgmac->clk_enable(bgmac, flags);
}

static inline void bgmac_cco_ctl_maskset(struct bgmac *bgmac, u32 offset,
					 u32 mask, u32 set)
{
	bgmac->cco_ctl_maskset(bgmac, offset, mask, set);
}

static inline u32 bgmac_get_bus_clock(struct bgmac *bgmac)
{
	return bgmac->get_bus_clock(bgmac);
}

static inline void bgmac_cmn_maskset32(struct bgmac *bgmac, u16 offset,
				       u32 mask, u32 set)
{
	bgmac->cmn_maskset32(bgmac, offset, mask, set);
}

static inline void bgmac_maskset(struct bgmac *bgmac, u16 offset, u32 mask,
				   u32 set)
{
	bgmac_write(bgmac, offset, (bgmac_read(bgmac, offset) & mask) | set);
}

static inline void bgmac_mask(struct bgmac *bgmac, u16 offset, u32 mask)
{
	bgmac_maskset(bgmac, offset, mask, 0);
}

static inline void bgmac_set(struct bgmac *bgmac, u16 offset, u32 set)
{
	bgmac_maskset(bgmac, offset, ~0, set);
}

static inline void bgmac_umac_maskset(struct bgmac *bgmac, u16 offset, u32 mask, u32 set)
{
	bgmac_maskset(bgmac, BGMAC_UNIMAC + offset, mask, set);
}

static inline int bgmac_phy_connect(struct bgmac *bgmac)
{
	return bgmac->phy_connect(bgmac);
}
#endif /* _BGMAC_H */
