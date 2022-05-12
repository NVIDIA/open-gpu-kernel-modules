/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2007 - 2018 Intel Corporation. */

#ifndef _E1000_DEFINES_H_
#define _E1000_DEFINES_H_

/* Number of Transmit and Receive Descriptors must be a multiple of 8 */
#define REQ_TX_DESCRIPTOR_MULTIPLE  8
#define REQ_RX_DESCRIPTOR_MULTIPLE  8

/* Definitions for power management and wakeup registers */
/* Wake Up Control */
#define E1000_WUC_PME_EN     0x00000002 /* PME Enable */

/* Wake Up Filter Control */
#define E1000_WUFC_LNKC 0x00000001 /* Link Status Change Wakeup Enable */
#define E1000_WUFC_MAG  0x00000002 /* Magic Packet Wakeup Enable */
#define E1000_WUFC_EX   0x00000004 /* Directed Exact Wakeup Enable */
#define E1000_WUFC_MC   0x00000008 /* Directed Multicast Wakeup Enable */
#define E1000_WUFC_BC   0x00000010 /* Broadcast Wakeup Enable */

/* Wake Up Status */
#define E1000_WUS_EX	0x00000004 /* Directed Exact */
#define E1000_WUS_ARPD	0x00000020 /* Directed ARP Request */
#define E1000_WUS_IPV4	0x00000040 /* Directed IPv4 */
#define E1000_WUS_IPV6	0x00000080 /* Directed IPv6 */
#define E1000_WUS_NSD	0x00000400 /* Directed IPv6 Neighbor Solicitation */

/* Packet types that are enabled for wake packet delivery */
#define WAKE_PKT_WUS ( \
	E1000_WUS_EX   | \
	E1000_WUS_ARPD | \
	E1000_WUS_IPV4 | \
	E1000_WUS_IPV6 | \
	E1000_WUS_NSD)

/* Wake Up Packet Length */
#define E1000_WUPL_MASK	0x00000FFF

/* Wake Up Packet Memory stores the first 128 bytes of the wake up packet */
#define E1000_WUPM_BYTES	128

/* Extended Device Control */
#define E1000_CTRL_EXT_SDP2_DATA 0x00000040 /* Value of SW Defineable Pin 2 */
#define E1000_CTRL_EXT_SDP3_DATA 0x00000080 /* Value of SW Defineable Pin 3 */
#define E1000_CTRL_EXT_SDP2_DIR  0x00000400 /* SDP2 Data direction */
#define E1000_CTRL_EXT_SDP3_DIR  0x00000800 /* SDP3 Data direction */

/* Physical Func Reset Done Indication */
#define E1000_CTRL_EXT_PFRSTD	0x00004000
#define E1000_CTRL_EXT_SDLPE	0X00040000  /* SerDes Low Power Enable */
#define E1000_CTRL_EXT_LINK_MODE_MASK	0x00C00000
#define E1000_CTRL_EXT_LINK_MODE_PCIE_SERDES	0x00C00000
#define E1000_CTRL_EXT_LINK_MODE_1000BASE_KX	0x00400000
#define E1000_CTRL_EXT_LINK_MODE_SGMII	0x00800000
#define E1000_CTRL_EXT_LINK_MODE_GMII	0x00000000
#define E1000_CTRL_EXT_EIAME	0x01000000
#define E1000_CTRL_EXT_IRCA		0x00000001
/* Interrupt delay cancellation */
/* Driver loaded bit for FW */
#define E1000_CTRL_EXT_DRV_LOAD       0x10000000
/* Interrupt acknowledge Auto-mask */
/* Clear Interrupt timers after IMS clear */
/* packet buffer parity error detection enabled */
/* descriptor FIFO parity error detection enable */
#define E1000_CTRL_EXT_PBA_CLR		0x80000000 /* PBA Clear */
#define E1000_CTRL_EXT_PHYPDEN		0x00100000
#define E1000_I2CCMD_REG_ADDR_SHIFT	16
#define E1000_I2CCMD_PHY_ADDR_SHIFT	24
#define E1000_I2CCMD_OPCODE_READ	0x08000000
#define E1000_I2CCMD_OPCODE_WRITE	0x00000000
#define E1000_I2CCMD_READY		0x20000000
#define E1000_I2CCMD_ERROR		0x80000000
#define E1000_I2CCMD_SFP_DATA_ADDR(a)	(0x0000 + (a))
#define E1000_I2CCMD_SFP_DIAG_ADDR(a)	(0x0100 + (a))
#define E1000_MAX_SGMII_PHY_REG_ADDR	255
#define E1000_I2CCMD_PHY_TIMEOUT	200
#define E1000_IVAR_VALID		0x80
#define E1000_GPIE_NSICR		0x00000001
#define E1000_GPIE_MSIX_MODE		0x00000010
#define E1000_GPIE_EIAME		0x40000000
#define E1000_GPIE_PBA			0x80000000

/* Receive Descriptor bit definitions */
#define E1000_RXD_STAT_DD       0x01    /* Descriptor Done */
#define E1000_RXD_STAT_EOP      0x02    /* End of Packet */
#define E1000_RXD_STAT_IXSM     0x04    /* Ignore checksum */
#define E1000_RXD_STAT_VP       0x08    /* IEEE VLAN Packet */
#define E1000_RXD_STAT_UDPCS    0x10    /* UDP xsum calculated */
#define E1000_RXD_STAT_TCPCS    0x20    /* TCP xsum calculated */
#define E1000_RXD_STAT_TS       0x10000 /* Pkt was time stamped */

#define E1000_RXDEXT_STATERR_LB    0x00040000
#define E1000_RXDEXT_STATERR_CE    0x01000000
#define E1000_RXDEXT_STATERR_SE    0x02000000
#define E1000_RXDEXT_STATERR_SEQ   0x04000000
#define E1000_RXDEXT_STATERR_CXE   0x10000000
#define E1000_RXDEXT_STATERR_TCPE  0x20000000
#define E1000_RXDEXT_STATERR_IPE   0x40000000
#define E1000_RXDEXT_STATERR_RXE   0x80000000

/* Same mask, but for extended and packet split descriptors */
#define E1000_RXDEXT_ERR_FRAME_ERR_MASK ( \
	E1000_RXDEXT_STATERR_CE  |            \
	E1000_RXDEXT_STATERR_SE  |            \
	E1000_RXDEXT_STATERR_SEQ |            \
	E1000_RXDEXT_STATERR_CXE |            \
	E1000_RXDEXT_STATERR_RXE)

#define E1000_MRQC_RSS_FIELD_IPV4_TCP          0x00010000
#define E1000_MRQC_RSS_FIELD_IPV4              0x00020000
#define E1000_MRQC_RSS_FIELD_IPV6_TCP_EX       0x00040000
#define E1000_MRQC_RSS_FIELD_IPV6              0x00100000
#define E1000_MRQC_RSS_FIELD_IPV6_TCP          0x00200000


/* Management Control */
#define E1000_MANC_SMBUS_EN      0x00000001 /* SMBus Enabled - RO */
#define E1000_MANC_ASF_EN        0x00000002 /* ASF Enabled - RO */
#define E1000_MANC_EN_BMC2OS     0x10000000 /* OSBMC is Enabled or not */
/* Enable Neighbor Discovery Filtering */
#define E1000_MANC_RCV_TCO_EN    0x00020000 /* Receive TCO Packets Enabled */
#define E1000_MANC_BLK_PHY_RST_ON_IDE   0x00040000 /* Block phy resets */
/* Enable MAC address filtering */
#define E1000_MANC_EN_MAC_ADDR_FILTER   0x00100000

/* Receive Control */
#define E1000_RCTL_EN             0x00000002    /* enable */
#define E1000_RCTL_SBP            0x00000004    /* store bad packet */
#define E1000_RCTL_UPE            0x00000008    /* unicast promiscuous enable */
#define E1000_RCTL_MPE            0x00000010    /* multicast promiscuous enab */
#define E1000_RCTL_LPE            0x00000020    /* long packet enable */
#define E1000_RCTL_LBM_MAC        0x00000040    /* MAC loopback mode */
#define E1000_RCTL_LBM_TCVR       0x000000C0    /* tcvr loopback mode */
#define E1000_RCTL_RDMTS_HALF     0x00000000    /* rx desc min threshold size */
#define E1000_RCTL_MO_SHIFT       12            /* multicast offset shift */
#define E1000_RCTL_BAM            0x00008000    /* broadcast enable */
#define E1000_RCTL_SZ_512         0x00020000    /* rx buffer size 512 */
#define E1000_RCTL_SZ_256         0x00030000    /* rx buffer size 256 */
#define E1000_RCTL_VFE            0x00040000    /* vlan filter enable */
#define E1000_RCTL_CFIEN          0x00080000    /* canonical form enable */
#define E1000_RCTL_DPF            0x00400000    /* Discard Pause Frames */
#define E1000_RCTL_PMCF           0x00800000    /* pass MAC control frames */
#define E1000_RCTL_SECRC          0x04000000    /* Strip Ethernet CRC */

/* Use byte values for the following shift parameters
 * Usage:
 *     psrctl |= (((ROUNDUP(value0, 128) >> E1000_PSRCTL_BSIZE0_SHIFT) &
 *                  E1000_PSRCTL_BSIZE0_MASK) |
 *                ((ROUNDUP(value1, 1024) >> E1000_PSRCTL_BSIZE1_SHIFT) &
 *                  E1000_PSRCTL_BSIZE1_MASK) |
 *                ((ROUNDUP(value2, 1024) << E1000_PSRCTL_BSIZE2_SHIFT) &
 *                  E1000_PSRCTL_BSIZE2_MASK) |
 *                ((ROUNDUP(value3, 1024) << E1000_PSRCTL_BSIZE3_SHIFT) |;
 *                  E1000_PSRCTL_BSIZE3_MASK))
 * where value0 = [128..16256],  default=256
 *       value1 = [1024..64512], default=4096
 *       value2 = [0..64512],    default=4096
 *       value3 = [0..64512],    default=0
 */

#define E1000_PSRCTL_BSIZE0_MASK   0x0000007F
#define E1000_PSRCTL_BSIZE1_MASK   0x00003F00
#define E1000_PSRCTL_BSIZE2_MASK   0x003F0000
#define E1000_PSRCTL_BSIZE3_MASK   0x3F000000

#define E1000_PSRCTL_BSIZE0_SHIFT  7            /* Shift _right_ 7 */
#define E1000_PSRCTL_BSIZE1_SHIFT  2            /* Shift _right_ 2 */
#define E1000_PSRCTL_BSIZE2_SHIFT  6            /* Shift _left_ 6 */
#define E1000_PSRCTL_BSIZE3_SHIFT 14            /* Shift _left_ 14 */

/* SWFW_SYNC Definitions */
#define E1000_SWFW_EEP_SM   0x1
#define E1000_SWFW_PHY0_SM  0x2
#define E1000_SWFW_PHY1_SM  0x4
#define E1000_SWFW_PHY2_SM  0x20
#define E1000_SWFW_PHY3_SM  0x40

/* FACTPS Definitions */
/* Device Control */
#define E1000_CTRL_FD       0x00000001  /* Full duplex.0=half; 1=full */
#define E1000_CTRL_GIO_MASTER_DISABLE 0x00000004 /*Blocks new Master requests */
#define E1000_CTRL_LRST     0x00000008  /* Link reset. 0=normal,1=reset */
#define E1000_CTRL_ASDE     0x00000020  /* Auto-speed detect enable */
#define E1000_CTRL_SLU      0x00000040  /* Set link up (Force Link) */
#define E1000_CTRL_ILOS     0x00000080  /* Invert Loss-Of Signal */
#define E1000_CTRL_SPD_SEL  0x00000300  /* Speed Select Mask */
#define E1000_CTRL_SPD_100  0x00000100  /* Force 100Mb */
#define E1000_CTRL_SPD_1000 0x00000200  /* Force 1Gb */
#define E1000_CTRL_FRCSPD   0x00000800  /* Force Speed */
#define E1000_CTRL_FRCDPX   0x00001000  /* Force Duplex */
/* Defined polarity of Dock/Undock indication in SDP[0] */
/* Reset both PHY ports, through PHYRST_N pin */
/* enable link status from external LINK_0 and LINK_1 pins */
#define E1000_CTRL_SWDPIN0  0x00040000  /* SWDPIN 0 value */
#define E1000_CTRL_SWDPIN1  0x00080000  /* SWDPIN 1 value */
#define E1000_CTRL_ADVD3WUC 0x00100000  /* D3 WUC */
#define E1000_CTRL_EN_PHY_PWR_MGMT 0x00200000 /* PHY PM enable */
#define E1000_CTRL_SDP0_DIR 0x00400000  /* SDP0 Data direction */
#define E1000_CTRL_SDP1_DIR 0x00800000  /* SDP1 Data direction */
#define E1000_CTRL_RST      0x04000000  /* Global reset */
#define E1000_CTRL_RFCE     0x08000000  /* Receive Flow Control enable */
#define E1000_CTRL_TFCE     0x10000000  /* Transmit flow control enable */
#define E1000_CTRL_VME      0x40000000  /* IEEE VLAN mode enable */
#define E1000_CTRL_PHY_RST  0x80000000  /* PHY Reset */
/* Initiate an interrupt to manageability engine */
#define E1000_CTRL_I2C_ENA  0x02000000  /* I2C enable */

/* Bit definitions for the Management Data IO (MDIO) and Management Data
 * Clock (MDC) pins in the Device Control Register.
 */

#define E1000_CONNSW_ENRGSRC             0x4
#define E1000_CONNSW_PHYSD		0x400
#define E1000_CONNSW_PHY_PDN		0x800
#define E1000_CONNSW_SERDESD		0x200
#define E1000_CONNSW_AUTOSENSE_CONF	0x2
#define E1000_CONNSW_AUTOSENSE_EN	0x1
#define E1000_PCS_CFG_PCS_EN             8
#define E1000_PCS_LCTL_FLV_LINK_UP       1
#define E1000_PCS_LCTL_FSV_100           2
#define E1000_PCS_LCTL_FSV_1000          4
#define E1000_PCS_LCTL_FDV_FULL          8
#define E1000_PCS_LCTL_FSD               0x10
#define E1000_PCS_LCTL_FORCE_LINK        0x20
#define E1000_PCS_LCTL_FORCE_FCTRL       0x80
#define E1000_PCS_LCTL_AN_ENABLE         0x10000
#define E1000_PCS_LCTL_AN_RESTART        0x20000
#define E1000_PCS_LCTL_AN_TIMEOUT        0x40000
#define E1000_ENABLE_SERDES_LOOPBACK     0x0410

#define E1000_PCS_LSTS_LINK_OK           1
#define E1000_PCS_LSTS_SPEED_100         2
#define E1000_PCS_LSTS_SPEED_1000        4
#define E1000_PCS_LSTS_DUPLEX_FULL       8
#define E1000_PCS_LSTS_SYNK_OK           0x10

/* Device Status */
#define E1000_STATUS_FD         0x00000001      /* Full duplex.0=half,1=full */
#define E1000_STATUS_LU         0x00000002      /* Link up.0=no,1=link */
#define E1000_STATUS_FUNC_MASK  0x0000000C      /* PCI Function Mask */
#define E1000_STATUS_FUNC_SHIFT 2
#define E1000_STATUS_FUNC_1     0x00000004      /* Function 1 */
#define E1000_STATUS_TXOFF      0x00000010      /* transmission paused */
#define E1000_STATUS_SPEED_100  0x00000040      /* Speed 100Mb/s */
#define E1000_STATUS_SPEED_1000 0x00000080      /* Speed 1000Mb/s */
/* Change in Dock/Undock state. Clear on write '0'. */
/* Status of Master requests. */
#define E1000_STATUS_GIO_MASTER_ENABLE 0x00080000
/* BMC external code execution disabled */

#define E1000_STATUS_2P5_SKU		0x00001000 /* Val of 2.5GBE SKU strap */
#define E1000_STATUS_2P5_SKU_OVER	0x00002000 /* Val of 2.5GBE SKU Over */
/* Constants used to intrepret the masked PCI-X bus speed. */

#define SPEED_10    10
#define SPEED_100   100
#define SPEED_1000  1000
#define SPEED_2500  2500
#define HALF_DUPLEX 1
#define FULL_DUPLEX 2


#define ADVERTISE_10_HALF                 0x0001
#define ADVERTISE_10_FULL                 0x0002
#define ADVERTISE_100_HALF                0x0004
#define ADVERTISE_100_FULL                0x0008
#define ADVERTISE_1000_HALF               0x0010 /* Not used, just FYI */
#define ADVERTISE_1000_FULL               0x0020

/* 1000/H is not supported, nor spec-compliant. */
#define E1000_ALL_SPEED_DUPLEX (ADVERTISE_10_HALF  |  ADVERTISE_10_FULL | \
				ADVERTISE_100_HALF |  ADVERTISE_100_FULL | \
						      ADVERTISE_1000_FULL)
#define E1000_ALL_NOT_GIG      (ADVERTISE_10_HALF  |  ADVERTISE_10_FULL | \
				ADVERTISE_100_HALF |  ADVERTISE_100_FULL)
#define E1000_ALL_100_SPEED    (ADVERTISE_100_HALF |  ADVERTISE_100_FULL)
#define E1000_ALL_10_SPEED     (ADVERTISE_10_HALF  |  ADVERTISE_10_FULL)
#define E1000_ALL_FULL_DUPLEX  (ADVERTISE_10_FULL  |  ADVERTISE_100_FULL | \
						      ADVERTISE_1000_FULL)
#define E1000_ALL_HALF_DUPLEX  (ADVERTISE_10_HALF  |  ADVERTISE_100_HALF)

#define AUTONEG_ADVERTISE_SPEED_DEFAULT   E1000_ALL_SPEED_DUPLEX

/* LED Control */
#define E1000_LEDCTL_LED0_MODE_SHIFT	0
#define E1000_LEDCTL_LED0_BLINK		0x00000080
#define E1000_LEDCTL_LED0_MODE_MASK	0x0000000F
#define E1000_LEDCTL_LED0_IVRT		0x00000040

#define E1000_LEDCTL_MODE_LED_ON        0xE
#define E1000_LEDCTL_MODE_LED_OFF       0xF

/* Transmit Descriptor bit definitions */
#define E1000_TXD_POPTS_IXSM 0x01       /* Insert IP checksum */
#define E1000_TXD_POPTS_TXSM 0x02       /* Insert TCP/UDP checksum */
#define E1000_TXD_CMD_EOP    0x01000000 /* End of Packet */
#define E1000_TXD_CMD_IFCS   0x02000000 /* Insert FCS (Ethernet CRC) */
#define E1000_TXD_CMD_RS     0x08000000 /* Report Status */
#define E1000_TXD_CMD_DEXT   0x20000000 /* Descriptor extension (0 = legacy) */
#define E1000_TXD_STAT_DD    0x00000001 /* Descriptor Done */
/* Extended desc bits for Linksec and timesync */

/* Transmit Control */
#define E1000_TCTL_EN     0x00000002    /* enable tx */
#define E1000_TCTL_PSP    0x00000008    /* pad short packets */
#define E1000_TCTL_CT     0x00000ff0    /* collision threshold */
#define E1000_TCTL_COLD   0x003ff000    /* collision distance */
#define E1000_TCTL_RTLC   0x01000000    /* Re-transmit on late collision */

/* DMA Coalescing register fields */
#define E1000_DMACR_DMACWT_MASK         0x00003FFF /* DMA Coal Watchdog Timer */
#define E1000_DMACR_DMACTHR_MASK        0x00FF0000 /* DMA Coal Rx Threshold */
#define E1000_DMACR_DMACTHR_SHIFT       16
#define E1000_DMACR_DMAC_LX_MASK        0x30000000 /* Lx when no PCIe trans */
#define E1000_DMACR_DMAC_LX_SHIFT       28
#define E1000_DMACR_DMAC_EN             0x80000000 /* Enable DMA Coalescing */
/* DMA Coalescing BMC-to-OS Watchdog Enable */
#define E1000_DMACR_DC_BMC2OSW_EN	0x00008000

#define E1000_DMCTXTH_DMCTTHR_MASK      0x00000FFF /* DMA Coal Tx Threshold */

#define E1000_DMCTLX_TTLX_MASK          0x00000FFF /* Time to LX request */

#define E1000_DMCRTRH_UTRESH_MASK       0x0007FFFF /* Rx Traffic Rate Thresh */
#define E1000_DMCRTRH_LRPRCW            0x80000000 /* Rx pkt rate curr window */

#define E1000_DMCCNT_CCOUNT_MASK        0x01FFFFFF /* DMA Coal Rx Current Cnt */

#define E1000_FCRTC_RTH_COAL_MASK       0x0003FFF0 /* FC Rx Thresh High val */
#define E1000_FCRTC_RTH_COAL_SHIFT      4
#define E1000_PCIEMISC_LX_DECISION      0x00000080 /* Lx power decision */

/* Timestamp in Rx buffer */
#define E1000_RXPBS_CFG_TS_EN           0x80000000

#define I210_RXPBSIZE_DEFAULT		0x000000A2 /* RXPBSIZE default */
#define I210_RXPBSIZE_MASK		0x0000003F
#define I210_RXPBSIZE_PB_30KB		0x0000001E
#define I210_RXPBSIZE_PB_32KB		0x00000020
#define I210_TXPBSIZE_DEFAULT		0x04000014 /* TXPBSIZE default */
#define I210_TXPBSIZE_MASK		0xC0FFFFFF
#define I210_TXPBSIZE_PB0_6KB		(6 << 0)
#define I210_TXPBSIZE_PB1_6KB		(6 << 6)
#define I210_TXPBSIZE_PB2_6KB		(6 << 12)
#define I210_TXPBSIZE_PB3_6KB		(6 << 18)

#define I210_DTXMXPKTSZ_DEFAULT		0x00000098

#define I210_SR_QUEUES_NUM		2

/* SerDes Control */
#define E1000_SCTL_DISABLE_SERDES_LOOPBACK 0x0400

/* Receive Checksum Control */
#define E1000_RXCSUM_IPOFL     0x00000100   /* IPv4 checksum offload */
#define E1000_RXCSUM_TUOFL     0x00000200   /* TCP / UDP checksum offload */
#define E1000_RXCSUM_CRCOFL    0x00000800   /* CRC32 offload enable */
#define E1000_RXCSUM_PCSD      0x00002000   /* packet checksum disabled */

/* Header split receive */
#define E1000_RFCTL_IPV6_EX_DIS         0x00010000
#define E1000_RFCTL_LEF                 0x00040000

/* Collision related configuration parameters */
#define E1000_COLLISION_THRESHOLD       15
#define E1000_CT_SHIFT                  4
#define E1000_COLLISION_DISTANCE        63
#define E1000_COLD_SHIFT                12

/* Ethertype field values */
#define ETHERNET_IEEE_VLAN_TYPE 0x8100  /* 802.3ac packet */

/* As per the EAS the maximum supported size is 9.5KB (9728 bytes) */
#define MAX_JUMBO_FRAME_SIZE		0x2600
#define MAX_STD_JUMBO_FRAME_SIZE	9216

/* PBA constants */
#define E1000_PBA_34K 0x0022
#define E1000_PBA_64K 0x0040    /* 64KB */

/* SW Semaphore Register */
#define E1000_SWSM_SMBI         0x00000001 /* Driver Semaphore bit */
#define E1000_SWSM_SWESMBI      0x00000002 /* FW Semaphore bit */

/* Interrupt Cause Read */
#define E1000_ICR_TXDW          0x00000001 /* Transmit desc written back */
#define E1000_ICR_LSC           0x00000004 /* Link Status Change */
#define E1000_ICR_RXSEQ         0x00000008 /* rx sequence error */
#define E1000_ICR_RXDMT0        0x00000010 /* rx desc min. threshold (0) */
#define E1000_ICR_RXT0          0x00000080 /* rx timer intr (ring 0) */
#define E1000_ICR_VMMB          0x00000100 /* VM MB event */
#define E1000_ICR_TS            0x00080000 /* Time Sync Interrupt */
#define E1000_ICR_DRSTA         0x40000000 /* Device Reset Asserted */
/* If this bit asserted, the driver should claim the interrupt */
#define E1000_ICR_INT_ASSERTED  0x80000000
/* LAN connected device generates an interrupt */
#define E1000_ICR_DOUTSYNC      0x10000000 /* NIC DMA out of sync */

/* Extended Interrupt Cause Read */
#define E1000_EICR_RX_QUEUE0    0x00000001 /* Rx Queue 0 Interrupt */
#define E1000_EICR_RX_QUEUE1    0x00000002 /* Rx Queue 1 Interrupt */
#define E1000_EICR_RX_QUEUE2    0x00000004 /* Rx Queue 2 Interrupt */
#define E1000_EICR_RX_QUEUE3    0x00000008 /* Rx Queue 3 Interrupt */
#define E1000_EICR_TX_QUEUE0    0x00000100 /* Tx Queue 0 Interrupt */
#define E1000_EICR_TX_QUEUE1    0x00000200 /* Tx Queue 1 Interrupt */
#define E1000_EICR_TX_QUEUE2    0x00000400 /* Tx Queue 2 Interrupt */
#define E1000_EICR_TX_QUEUE3    0x00000800 /* Tx Queue 3 Interrupt */
#define E1000_EICR_OTHER        0x80000000 /* Interrupt Cause Active */
/* TCP Timer */

/* This defines the bits that are set in the Interrupt Mask
 * Set/Read Register.  Each bit is documented below:
 *   o RXT0   = Receiver Timer Interrupt (ring 0)
 *   o TXDW   = Transmit Descriptor Written Back
 *   o RXDMT0 = Receive Descriptor Minimum Threshold hit (ring 0)
 *   o RXSEQ  = Receive Sequence Error
 *   o LSC    = Link Status Change
 */
#define IMS_ENABLE_MASK ( \
	E1000_IMS_RXT0   |    \
	E1000_IMS_TXDW   |    \
	E1000_IMS_RXDMT0 |    \
	E1000_IMS_RXSEQ  |    \
	E1000_IMS_LSC    |    \
	E1000_IMS_DOUTSYNC)

/* Interrupt Mask Set */
#define E1000_IMS_TXDW      E1000_ICR_TXDW      /* Transmit desc written back */
#define E1000_IMS_LSC       E1000_ICR_LSC       /* Link Status Change */
#define E1000_IMS_VMMB      E1000_ICR_VMMB      /* Mail box activity */
#define E1000_IMS_TS        E1000_ICR_TS        /* Time Sync Interrupt */
#define E1000_IMS_RXSEQ     E1000_ICR_RXSEQ     /* rx sequence error */
#define E1000_IMS_RXDMT0    E1000_ICR_RXDMT0    /* rx desc min. threshold */
#define E1000_IMS_RXT0      E1000_ICR_RXT0      /* rx timer intr */
#define E1000_IMS_DRSTA     E1000_ICR_DRSTA     /* Device Reset Asserted */
#define E1000_IMS_DOUTSYNC  E1000_ICR_DOUTSYNC /* NIC DMA out of sync */

/* Extended Interrupt Mask Set */
#define E1000_EIMS_OTHER        E1000_EICR_OTHER   /* Interrupt Cause Active */

/* Interrupt Cause Set */
#define E1000_ICS_LSC       E1000_ICR_LSC       /* Link Status Change */
#define E1000_ICS_RXDMT0    E1000_ICR_RXDMT0    /* rx desc min. threshold */
#define E1000_ICS_DRSTA     E1000_ICR_DRSTA     /* Device Reset Aserted */

/* Extended Interrupt Cause Set */
/* E1000_EITR_CNT_IGNR is only for 82576 and newer */
#define E1000_EITR_CNT_IGNR     0x80000000 /* Don't reset counters on write */


/* Transmit Descriptor Control */
/* Enable the counting of descriptors still to be processed. */

/* Flow Control Constants */
#define FLOW_CONTROL_ADDRESS_LOW  0x00C28001
#define FLOW_CONTROL_ADDRESS_HIGH 0x00000100
#define FLOW_CONTROL_TYPE         0x8808

/* Transmit Config Word */
#define E1000_TXCW_ASM_DIR	0x00000100 /* TXCW astm pause direction */
#define E1000_TXCW_PAUSE	0x00000080 /* TXCW sym pause request */

/* 802.1q VLAN Packet Size */
#define VLAN_TAG_SIZE              4    /* 802.3ac tag (not DMA'd) */
#define E1000_VLAN_FILTER_TBL_SIZE 128  /* VLAN Filter Table (4096 bits) */

/* Receive Address */
/* Number of high/low register pairs in the RAR. The RAR (Receive Address
 * Registers) holds the directed and multicast addresses that we monitor.
 * Technically, we have 16 spots.  However, we reserve one of these spots
 * (RAR[15]) for our directed address used by controllers with
 * manageability enabled, allowing us room for 15 multicast addresses.
 */
#define E1000_RAH_AV  0x80000000        /* Receive descriptor valid */
#define E1000_RAH_ASEL_SRC_ADDR 0x00010000
#define E1000_RAH_QSEL_ENABLE 0x10000000
#define E1000_RAL_MAC_ADDR_LEN 4
#define E1000_RAH_MAC_ADDR_LEN 2
#define E1000_RAH_POOL_MASK 0x03FC0000
#define E1000_RAH_POOL_1 0x00040000

/* Error Codes */
#define E1000_ERR_NVM      1
#define E1000_ERR_PHY      2
#define E1000_ERR_CONFIG   3
#define E1000_ERR_PARAM    4
#define E1000_ERR_MAC_INIT 5
#define E1000_ERR_RESET   9
#define E1000_ERR_MASTER_REQUESTS_PENDING 10
#define E1000_BLK_PHY_RESET   12
#define E1000_ERR_SWFW_SYNC 13
#define E1000_NOT_IMPLEMENTED 14
#define E1000_ERR_MBX      15
#define E1000_ERR_INVALID_ARGUMENT  16
#define E1000_ERR_NO_SPACE          17
#define E1000_ERR_NVM_PBA_SECTION   18
#define E1000_ERR_INVM_VALUE_NOT_FOUND	19
#define E1000_ERR_I2C               20

/* Loop limit on how long we wait for auto-negotiation to complete */
#define COPPER_LINK_UP_LIMIT              10
#define PHY_AUTO_NEG_LIMIT                45
#define PHY_FORCE_LIMIT                   20
/* Number of 100 microseconds we wait for PCI Express master disable */
#define MASTER_DISABLE_TIMEOUT      800
/* Number of milliseconds we wait for PHY configuration done after MAC reset */
#define PHY_CFG_TIMEOUT             100
/* Number of 2 milliseconds we wait for acquiring MDIO ownership. */
/* Number of milliseconds for NVM auto read done after MAC reset. */
#define AUTO_READ_DONE_TIMEOUT      10

/* Flow Control */
#define E1000_FCRTL_XONE 0x80000000     /* Enable XON frame transmission */

#define E1000_TSYNCTXCTL_VALID    0x00000001 /* tx timestamp valid */
#define E1000_TSYNCTXCTL_ENABLED  0x00000010 /* enable tx timestampping */

#define E1000_TSYNCRXCTL_VALID      0x00000001 /* rx timestamp valid */
#define E1000_TSYNCRXCTL_TYPE_MASK  0x0000000E /* rx type mask */
#define E1000_TSYNCRXCTL_TYPE_L2_V2       0x00
#define E1000_TSYNCRXCTL_TYPE_L4_V1       0x02
#define E1000_TSYNCRXCTL_TYPE_L2_L4_V2    0x04
#define E1000_TSYNCRXCTL_TYPE_ALL         0x08
#define E1000_TSYNCRXCTL_TYPE_EVENT_V2    0x0A
#define E1000_TSYNCRXCTL_ENABLED    0x00000010 /* enable rx timestampping */

#define E1000_TSYNCRXCFG_PTP_V1_CTRLT_MASK   0x000000FF
#define E1000_TSYNCRXCFG_PTP_V1_SYNC_MESSAGE       0x00
#define E1000_TSYNCRXCFG_PTP_V1_DELAY_REQ_MESSAGE  0x01
#define E1000_TSYNCRXCFG_PTP_V1_FOLLOWUP_MESSAGE   0x02
#define E1000_TSYNCRXCFG_PTP_V1_DELAY_RESP_MESSAGE 0x03
#define E1000_TSYNCRXCFG_PTP_V1_MANAGEMENT_MESSAGE 0x04

#define E1000_TSYNCRXCFG_PTP_V2_MSGID_MASK               0x00000F00
#define E1000_TSYNCRXCFG_PTP_V2_SYNC_MESSAGE                 0x0000
#define E1000_TSYNCRXCFG_PTP_V2_DELAY_REQ_MESSAGE            0x0100
#define E1000_TSYNCRXCFG_PTP_V2_PATH_DELAY_REQ_MESSAGE       0x0200
#define E1000_TSYNCRXCFG_PTP_V2_PATH_DELAY_RESP_MESSAGE      0x0300
#define E1000_TSYNCRXCFG_PTP_V2_FOLLOWUP_MESSAGE             0x0800
#define E1000_TSYNCRXCFG_PTP_V2_DELAY_RESP_MESSAGE           0x0900
#define E1000_TSYNCRXCFG_PTP_V2_PATH_DELAY_FOLLOWUP_MESSAGE  0x0A00
#define E1000_TSYNCRXCFG_PTP_V2_ANNOUNCE_MESSAGE             0x0B00
#define E1000_TSYNCRXCFG_PTP_V2_SIGNALLING_MESSAGE           0x0C00
#define E1000_TSYNCRXCFG_PTP_V2_MANAGEMENT_MESSAGE           0x0D00

#define E1000_TIMINCA_16NS_SHIFT 24

/* Time Sync Interrupt Cause/Mask Register Bits */

#define TSINTR_SYS_WRAP  BIT(0) /* SYSTIM Wrap around. */
#define TSINTR_TXTS      BIT(1) /* Transmit Timestamp. */
#define TSINTR_RXTS      BIT(2) /* Receive Timestamp. */
#define TSINTR_TT0       BIT(3) /* Target Time 0 Trigger. */
#define TSINTR_TT1       BIT(4) /* Target Time 1 Trigger. */
#define TSINTR_AUTT0     BIT(5) /* Auxiliary Timestamp 0 Taken. */
#define TSINTR_AUTT1     BIT(6) /* Auxiliary Timestamp 1 Taken. */
#define TSINTR_TADJ      BIT(7) /* Time Adjust Done. */

#define TSYNC_INTERRUPTS TSINTR_TXTS
#define E1000_TSICR_TXTS TSINTR_TXTS

/* TSAUXC Configuration Bits */
#define TSAUXC_EN_TT0    BIT(0)  /* Enable target time 0. */
#define TSAUXC_EN_TT1    BIT(1)  /* Enable target time 1. */
#define TSAUXC_EN_CLK0   BIT(2)  /* Enable Configurable Frequency Clock 0. */
#define TSAUXC_SAMP_AUT0 BIT(3)  /* Latch SYSTIML/H into AUXSTMPL/0. */
#define TSAUXC_ST0       BIT(4)  /* Start Clock 0 Toggle on Target Time 0. */
#define TSAUXC_EN_CLK1   BIT(5)  /* Enable Configurable Frequency Clock 1. */
#define TSAUXC_SAMP_AUT1 BIT(6)  /* Latch SYSTIML/H into AUXSTMPL/1. */
#define TSAUXC_ST1       BIT(7)  /* Start Clock 1 Toggle on Target Time 1. */
#define TSAUXC_EN_TS0    BIT(8)  /* Enable hardware timestamp 0. */
#define TSAUXC_AUTT0     BIT(9)  /* Auxiliary Timestamp Taken. */
#define TSAUXC_EN_TS1    BIT(10) /* Enable hardware timestamp 0. */
#define TSAUXC_AUTT1     BIT(11) /* Auxiliary Timestamp Taken. */
#define TSAUXC_PLSG      BIT(17) /* Generate a pulse. */
#define TSAUXC_DISABLE   BIT(31) /* Disable SYSTIM Count Operation. */

/* SDP Configuration Bits */
#define AUX0_SEL_SDP0    (0u << 0)  /* Assign SDP0 to auxiliary time stamp 0. */
#define AUX0_SEL_SDP1    (1u << 0)  /* Assign SDP1 to auxiliary time stamp 0. */
#define AUX0_SEL_SDP2    (2u << 0)  /* Assign SDP2 to auxiliary time stamp 0. */
#define AUX0_SEL_SDP3    (3u << 0)  /* Assign SDP3 to auxiliary time stamp 0. */
#define AUX0_TS_SDP_EN   (1u << 2)  /* Enable auxiliary time stamp trigger 0. */
#define AUX1_SEL_SDP0    (0u << 3)  /* Assign SDP0 to auxiliary time stamp 1. */
#define AUX1_SEL_SDP1    (1u << 3)  /* Assign SDP1 to auxiliary time stamp 1. */
#define AUX1_SEL_SDP2    (2u << 3)  /* Assign SDP2 to auxiliary time stamp 1. */
#define AUX1_SEL_SDP3    (3u << 3)  /* Assign SDP3 to auxiliary time stamp 1. */
#define AUX1_TS_SDP_EN   (1u << 5)  /* Enable auxiliary time stamp trigger 1. */
#define TS_SDP0_SEL_TT0  (0u << 6)  /* Target time 0 is output on SDP0. */
#define TS_SDP0_SEL_TT1  (1u << 6)  /* Target time 1 is output on SDP0. */
#define TS_SDP0_SEL_FC0  (2u << 6)  /* Freq clock  0 is output on SDP0. */
#define TS_SDP0_SEL_FC1  (3u << 6)  /* Freq clock  1 is output on SDP0. */
#define TS_SDP0_EN       (1u << 8)  /* SDP0 is assigned to Tsync. */
#define TS_SDP1_SEL_TT0  (0u << 9)  /* Target time 0 is output on SDP1. */
#define TS_SDP1_SEL_TT1  (1u << 9)  /* Target time 1 is output on SDP1. */
#define TS_SDP1_SEL_FC0  (2u << 9)  /* Freq clock  0 is output on SDP1. */
#define TS_SDP1_SEL_FC1  (3u << 9)  /* Freq clock  1 is output on SDP1. */
#define TS_SDP1_EN       (1u << 11) /* SDP1 is assigned to Tsync. */
#define TS_SDP2_SEL_TT0  (0u << 12) /* Target time 0 is output on SDP2. */
#define TS_SDP2_SEL_TT1  (1u << 12) /* Target time 1 is output on SDP2. */
#define TS_SDP2_SEL_FC0  (2u << 12) /* Freq clock  0 is output on SDP2. */
#define TS_SDP2_SEL_FC1  (3u << 12) /* Freq clock  1 is output on SDP2. */
#define TS_SDP2_EN       (1u << 14) /* SDP2 is assigned to Tsync. */
#define TS_SDP3_SEL_TT0  (0u << 15) /* Target time 0 is output on SDP3. */
#define TS_SDP3_SEL_TT1  (1u << 15) /* Target time 1 is output on SDP3. */
#define TS_SDP3_SEL_FC0  (2u << 15) /* Freq clock  0 is output on SDP3. */
#define TS_SDP3_SEL_FC1  (3u << 15) /* Freq clock  1 is output on SDP3. */
#define TS_SDP3_EN       (1u << 17) /* SDP3 is assigned to Tsync. */

#define E1000_MDICNFG_EXT_MDIO    0x80000000      /* MDI ext/int destination */
#define E1000_MDICNFG_COM_MDIO    0x40000000      /* MDI shared w/ lan 0 */
#define E1000_MDICNFG_PHY_MASK    0x03E00000
#define E1000_MDICNFG_PHY_SHIFT   21

#define E1000_MEDIA_PORT_COPPER			1
#define E1000_MEDIA_PORT_OTHER			2
#define E1000_M88E1112_AUTO_COPPER_SGMII	0x2
#define E1000_M88E1112_AUTO_COPPER_BASEX	0x3
#define E1000_M88E1112_STATUS_LINK		0x0004 /* Interface Link Bit */
#define E1000_M88E1112_MAC_CTRL_1		0x10
#define E1000_M88E1112_MAC_CTRL_1_MODE_MASK	0x0380 /* Mode Select */
#define E1000_M88E1112_MAC_CTRL_1_MODE_SHIFT	7
#define E1000_M88E1112_PAGE_ADDR		0x16
#define E1000_M88E1112_STATUS			0x01
#define E1000_M88E1512_CFG_REG_1		0x0010
#define E1000_M88E1512_CFG_REG_2		0x0011
#define E1000_M88E1512_CFG_REG_3		0x0007
#define E1000_M88E1512_MODE			0x0014

/* PCI Express Control */
#define E1000_GCR_CMPL_TMOUT_MASK       0x0000F000
#define E1000_GCR_CMPL_TMOUT_10ms       0x00001000
#define E1000_GCR_CMPL_TMOUT_RESEND     0x00010000
#define E1000_GCR_CAP_VER2              0x00040000

/* mPHY Address Control and Data Registers */
#define E1000_MPHY_ADDR_CTL          0x0024 /* mPHY Address Control Register */
#define E1000_MPHY_ADDR_CTL_OFFSET_MASK 0xFFFF0000
#define E1000_MPHY_DATA                 0x0E10 /* mPHY Data Register */

/* mPHY PCS CLK Register */
#define E1000_MPHY_PCS_CLK_REG_OFFSET  0x0004 /* mPHY PCS CLK AFE CSR Offset */
/* mPHY Near End Digital Loopback Override Bit */
#define E1000_MPHY_PCS_CLK_REG_DIGINELBEN 0x10

#define E1000_PCS_LCTL_FORCE_FCTRL	0x80
#define E1000_PCS_LSTS_AN_COMPLETE	0x10000

/* PHY Control Register */
#define MII_CR_FULL_DUPLEX      0x0100  /* FDX =1, half duplex =0 */
#define MII_CR_RESTART_AUTO_NEG 0x0200  /* Restart auto negotiation */
#define MII_CR_POWER_DOWN       0x0800  /* Power down */
#define MII_CR_AUTO_NEG_EN      0x1000  /* Auto Neg Enable */
#define MII_CR_LOOPBACK         0x4000  /* 0 = normal, 1 = loopback */
#define MII_CR_RESET            0x8000  /* 0 = normal, 1 = PHY reset */
#define MII_CR_SPEED_1000       0x0040
#define MII_CR_SPEED_100        0x2000
#define MII_CR_SPEED_10         0x0000

/* PHY Status Register */
#define MII_SR_LINK_STATUS       0x0004 /* Link Status 1 = link */
#define MII_SR_AUTONEG_COMPLETE  0x0020 /* Auto Neg Complete */

/* Autoneg Advertisement Register */
#define NWAY_AR_10T_HD_CAPS      0x0020   /* 10T   Half Duplex Capable */
#define NWAY_AR_10T_FD_CAPS      0x0040   /* 10T   Full Duplex Capable */
#define NWAY_AR_100TX_HD_CAPS    0x0080   /* 100TX Half Duplex Capable */
#define NWAY_AR_100TX_FD_CAPS    0x0100   /* 100TX Full Duplex Capable */
#define NWAY_AR_PAUSE            0x0400   /* Pause operation desired */
#define NWAY_AR_ASM_DIR          0x0800   /* Asymmetric Pause Direction bit */

/* Link Partner Ability Register (Base Page) */
#define NWAY_LPAR_PAUSE          0x0400 /* LP Pause operation desired */
#define NWAY_LPAR_ASM_DIR        0x0800 /* LP Asymmetric Pause Direction bit */

/* Autoneg Expansion Register */

/* 1000BASE-T Control Register */
#define CR_1000T_HD_CAPS         0x0100 /* Advertise 1000T HD capability */
#define CR_1000T_FD_CAPS         0x0200 /* Advertise 1000T FD capability  */
#define CR_1000T_MS_VALUE        0x0800 /* 1=Configure PHY as Master */
					/* 0=Configure PHY as Slave */
#define CR_1000T_MS_ENABLE       0x1000 /* 1=Master/Slave manual config value */
					/* 0=Automatic Master/Slave config */

/* 1000BASE-T Status Register */
#define SR_1000T_REMOTE_RX_STATUS 0x1000 /* Remote receiver OK */
#define SR_1000T_LOCAL_RX_STATUS  0x2000 /* Local receiver OK */


/* PHY 1000 MII Register/Bit Definitions */
/* PHY Registers defined by IEEE */
#define PHY_CONTROL      0x00 /* Control Register */
#define PHY_STATUS       0x01 /* Status Register */
#define PHY_ID1          0x02 /* Phy Id Reg (word 1) */
#define PHY_ID2          0x03 /* Phy Id Reg (word 2) */
#define PHY_AUTONEG_ADV  0x04 /* Autoneg Advertisement */
#define PHY_LP_ABILITY   0x05 /* Link Partner Ability (Base Page) */
#define PHY_1000T_CTRL   0x09 /* 1000Base-T Control Reg */
#define PHY_1000T_STATUS 0x0A /* 1000Base-T Status Reg */

/* NVM Control */
#define E1000_EECD_SK        0x00000001 /* NVM Clock */
#define E1000_EECD_CS        0x00000002 /* NVM Chip Select */
#define E1000_EECD_DI        0x00000004 /* NVM Data In */
#define E1000_EECD_DO        0x00000008 /* NVM Data Out */
#define E1000_EECD_REQ       0x00000040 /* NVM Access Request */
#define E1000_EECD_GNT       0x00000080 /* NVM Access Grant */
#define E1000_EECD_PRES      0x00000100 /* NVM Present */
/* NVM Addressing bits based on type 0=small, 1=large */
#define E1000_EECD_ADDR_BITS 0x00000400
#define E1000_NVM_GRANT_ATTEMPTS   1000 /* NVM # attempts to gain grant */
#define E1000_EECD_AUTO_RD          0x00000200  /* NVM Auto Read done */
#define E1000_EECD_SIZE_EX_MASK     0x00007800  /* NVM Size */
#define E1000_EECD_SIZE_EX_SHIFT     11
#define E1000_EECD_FLUPD_I210		0x00800000 /* Update FLASH */
#define E1000_EECD_FLUDONE_I210		0x04000000 /* Update FLASH done*/
#define E1000_EECD_FLASH_DETECTED_I210	0x00080000 /* FLASH detected */
#define E1000_FLUDONE_ATTEMPTS		20000
#define E1000_EERD_EEWR_MAX_COUNT	512 /* buffered EEPROM words rw */
#define E1000_I210_FIFO_SEL_RX		0x00
#define E1000_I210_FIFO_SEL_TX_QAV(_i)	(0x02 + (_i))
#define E1000_I210_FIFO_SEL_TX_LEGACY	E1000_I210_FIFO_SEL_TX_QAV(0)
#define E1000_I210_FIFO_SEL_BMC2OS_TX	0x06
#define E1000_I210_FIFO_SEL_BMC2OS_RX	0x01
#define E1000_I210_FLASH_SECTOR_SIZE	0x1000 /* 4KB FLASH sector unit size */
/* Secure FLASH mode requires removing MSb */
#define E1000_I210_FW_PTR_MASK		0x7FFF
/* Firmware code revision field word offset*/
#define E1000_I210_FW_VER_OFFSET	328
#define E1000_EECD_FLUPD_I210		0x00800000 /* Update FLASH */
#define E1000_EECD_FLUDONE_I210		0x04000000 /* Update FLASH done*/
#define E1000_FLUDONE_ATTEMPTS		20000
#define E1000_EERD_EEWR_MAX_COUNT	512 /* buffered EEPROM words rw */
#define E1000_I210_FIFO_SEL_RX		0x00
#define E1000_I210_FIFO_SEL_TX_QAV(_i)	(0x02 + (_i))
#define E1000_I210_FIFO_SEL_TX_LEGACY	E1000_I210_FIFO_SEL_TX_QAV(0)
#define E1000_I210_FIFO_SEL_BMC2OS_TX	0x06
#define E1000_I210_FIFO_SEL_BMC2OS_RX	0x01


/* Offset to data in NVM read/write registers */
#define E1000_NVM_RW_REG_DATA   16
#define E1000_NVM_RW_REG_DONE   2    /* Offset to READ/WRITE done bit */
#define E1000_NVM_RW_REG_START  1    /* Start operation */
#define E1000_NVM_RW_ADDR_SHIFT 2    /* Shift to the address bits */
#define E1000_NVM_POLL_READ     0    /* Flag for polling for read complete */

/* NVM Word Offsets */
#define NVM_COMPAT                 0x0003
#define NVM_ID_LED_SETTINGS        0x0004 /* SERDES output amplitude */
#define NVM_VERSION                0x0005
#define NVM_INIT_CONTROL2_REG      0x000F
#define NVM_INIT_CONTROL3_PORT_B   0x0014
#define NVM_INIT_CONTROL3_PORT_A   0x0024
#define NVM_ALT_MAC_ADDR_PTR       0x0037
#define NVM_CHECKSUM_REG           0x003F
#define NVM_COMPATIBILITY_REG_3    0x0003
#define NVM_COMPATIBILITY_BIT_MASK 0x8000
#define NVM_MAC_ADDR               0x0000
#define NVM_SUB_DEV_ID             0x000B
#define NVM_SUB_VEN_ID             0x000C
#define NVM_DEV_ID                 0x000D
#define NVM_VEN_ID                 0x000E
#define NVM_INIT_CTRL_2            0x000F
#define NVM_INIT_CTRL_4            0x0013
#define NVM_LED_1_CFG              0x001C
#define NVM_LED_0_2_CFG            0x001F
#define NVM_ETRACK_WORD            0x0042
#define NVM_ETRACK_HIWORD          0x0043
#define NVM_COMB_VER_OFF           0x0083
#define NVM_COMB_VER_PTR           0x003d

/* NVM version defines */
#define NVM_MAJOR_MASK			0xF000
#define NVM_MINOR_MASK			0x0FF0
#define NVM_IMAGE_ID_MASK		0x000F
#define NVM_COMB_VER_MASK		0x00FF
#define NVM_MAJOR_SHIFT			12
#define NVM_MINOR_SHIFT			4
#define NVM_COMB_VER_SHFT		8
#define NVM_VER_INVALID			0xFFFF
#define NVM_ETRACK_SHIFT		16
#define NVM_ETRACK_VALID		0x8000
#define NVM_NEW_DEC_MASK		0x0F00
#define NVM_HEX_CONV			16
#define NVM_HEX_TENS			10

#define NVM_ETS_CFG			0x003E
#define NVM_ETS_LTHRES_DELTA_MASK	0x07C0
#define NVM_ETS_LTHRES_DELTA_SHIFT	6
#define NVM_ETS_TYPE_MASK		0x0038
#define NVM_ETS_TYPE_SHIFT		3
#define NVM_ETS_TYPE_EMC		0x000
#define NVM_ETS_NUM_SENSORS_MASK	0x0007
#define NVM_ETS_DATA_LOC_MASK		0x3C00
#define NVM_ETS_DATA_LOC_SHIFT		10
#define NVM_ETS_DATA_INDEX_MASK		0x0300
#define NVM_ETS_DATA_INDEX_SHIFT	8
#define NVM_ETS_DATA_HTHRESH_MASK	0x00FF

#define E1000_NVM_CFG_DONE_PORT_0  0x040000 /* MNG config cycle done */
#define E1000_NVM_CFG_DONE_PORT_1  0x080000 /* ...for second port */
#define E1000_NVM_CFG_DONE_PORT_2  0x100000 /* ...for third port */
#define E1000_NVM_CFG_DONE_PORT_3  0x200000 /* ...for fourth port */

#define NVM_82580_LAN_FUNC_OFFSET(a) (a ? (0x40 + (0x40 * a)) : 0)

/* Mask bits for fields in Word 0x24 of the NVM */
#define NVM_WORD24_COM_MDIO         0x0008 /* MDIO interface shared */
#define NVM_WORD24_EXT_MDIO         0x0004 /* MDIO accesses routed external */

/* Mask bits for fields in Word 0x0f of the NVM */
#define NVM_WORD0F_PAUSE_MASK       0x3000
#define NVM_WORD0F_ASM_DIR          0x2000

/* Mask bits for fields in Word 0x1a of the NVM */

/* length of string needed to store part num */
#define E1000_PBANUM_LENGTH         11

/* For checksumming, the sum of all words in the NVM should equal 0xBABA. */
#define NVM_SUM                    0xBABA

#define NVM_PBA_OFFSET_0           8
#define NVM_PBA_OFFSET_1           9
#define NVM_RESERVED_WORD		0xFFFF
#define NVM_PBA_PTR_GUARD          0xFAFA
#define NVM_WORD_SIZE_BASE_SHIFT   6

/* NVM Commands - Microwire */

/* NVM Commands - SPI */
#define NVM_MAX_RETRY_SPI          5000 /* Max wait of 5ms, for RDY signal */
#define NVM_WRITE_OPCODE_SPI       0x02 /* NVM write opcode */
#define NVM_READ_OPCODE_SPI        0x03 /* NVM read opcode */
#define NVM_A8_OPCODE_SPI          0x08 /* opcode bit-3 = address bit-8 */
#define NVM_WREN_OPCODE_SPI        0x06 /* NVM set Write Enable latch */
#define NVM_RDSR_OPCODE_SPI        0x05 /* NVM read Status register */

/* SPI NVM Status Register */
#define NVM_STATUS_RDY_SPI         0x01

/* Word definitions for ID LED Settings */
#define ID_LED_RESERVED_0000 0x0000
#define ID_LED_RESERVED_FFFF 0xFFFF
#define ID_LED_DEFAULT       ((ID_LED_OFF1_ON2  << 12) | \
			      (ID_LED_OFF1_OFF2 <<  8) | \
			      (ID_LED_DEF1_DEF2 <<  4) | \
			      (ID_LED_DEF1_DEF2))
#define ID_LED_DEF1_DEF2     0x1
#define ID_LED_DEF1_ON2      0x2
#define ID_LED_DEF1_OFF2     0x3
#define ID_LED_ON1_DEF2      0x4
#define ID_LED_ON1_ON2       0x5
#define ID_LED_ON1_OFF2      0x6
#define ID_LED_OFF1_DEF2     0x7
#define ID_LED_OFF1_ON2      0x8
#define ID_LED_OFF1_OFF2     0x9

#define IGP_ACTIVITY_LED_MASK   0xFFFFF0FF
#define IGP_ACTIVITY_LED_ENABLE 0x0300
#define IGP_LED3_MODE           0x07000000

/* PCI/PCI-X/PCI-EX Config space */
#define PCIE_DEVICE_CONTROL2         0x28
#define PCIE_DEVICE_CONTROL2_16ms    0x0005

#define PHY_REVISION_MASK      0xFFFFFFF0
#define MAX_PHY_REG_ADDRESS    0x1F  /* 5 bit address bus (0-0x1F) */
#define MAX_PHY_MULTI_PAGE_REG 0xF

/* Bit definitions for valid PHY IDs. */
/* I = Integrated
 * E = External
 */
#define M88E1111_I_PHY_ID    0x01410CC0
#define M88E1112_E_PHY_ID    0x01410C90
#define I347AT4_E_PHY_ID     0x01410DC0
#define IGP03E1000_E_PHY_ID  0x02A80390
#define I82580_I_PHY_ID      0x015403A0
#define I350_I_PHY_ID        0x015403B0
#define M88_VENDOR           0x0141
#define I210_I_PHY_ID        0x01410C00
#define M88E1543_E_PHY_ID    0x01410EA0
#define M88E1512_E_PHY_ID    0x01410DD0
#define BCM54616_E_PHY_ID    0x03625D10

/* M88E1000 Specific Registers */
#define M88E1000_PHY_SPEC_CTRL     0x10  /* PHY Specific Control Register */
#define M88E1000_PHY_SPEC_STATUS   0x11  /* PHY Specific Status Register */
#define M88E1000_EXT_PHY_SPEC_CTRL 0x14  /* Extended PHY Specific Control */

#define M88E1000_PHY_PAGE_SELECT   0x1D  /* Reg 29 for page number setting */
#define M88E1000_PHY_GEN_CONTROL   0x1E  /* Its meaning depends on reg 29 */

/* M88E1000 PHY Specific Control Register */
#define M88E1000_PSCR_POLARITY_REVERSAL 0x0002 /* 1=Polarity Reversal enabled */
/* 1=CLK125 low, 0=CLK125 toggling */
#define M88E1000_PSCR_MDI_MANUAL_MODE  0x0000  /* MDI Crossover Mode bits 6:5 */
					       /* Manual MDI configuration */
#define M88E1000_PSCR_MDIX_MANUAL_MODE 0x0020  /* Manual MDIX configuration */
/* 1000BASE-T: Auto crossover, 100BASE-TX/10BASE-T: MDI Mode */
#define M88E1000_PSCR_AUTO_X_1000T     0x0040
/* Auto crossover enabled all speeds */
#define M88E1000_PSCR_AUTO_X_MODE      0x0060
/* 1=Enable Extended 10BASE-T distance (Lower 10BASE-T Rx Threshold
 * 0=Normal 10BASE-T Rx Threshold
 */
/* 1=5-bit interface in 100BASE-TX, 0=MII interface in 100BASE-TX */
#define M88E1000_PSCR_ASSERT_CRS_ON_TX     0x0800 /* 1=Assert CRS on Transmit */

/* M88E1000 PHY Specific Status Register */
#define M88E1000_PSSR_REV_POLARITY       0x0002 /* 1=Polarity reversed */
#define M88E1000_PSSR_DOWNSHIFT          0x0020 /* 1=Downshifted */
#define M88E1000_PSSR_MDIX               0x0040 /* 1=MDIX; 0=MDI */
/* 0 = <50M
 * 1 = 50-80M
 * 2 = 80-110M
 * 3 = 110-140M
 * 4 = >140M
 */
#define M88E1000_PSSR_CABLE_LENGTH       0x0380
#define M88E1000_PSSR_SPEED              0xC000 /* Speed, bits 14:15 */
#define M88E1000_PSSR_1000MBS            0x8000 /* 10=1000Mbs */

#define M88E1000_PSSR_CABLE_LENGTH_SHIFT 7

/* M88E1000 Extended PHY Specific Control Register */
/* 1 = Lost lock detect enabled.
 * Will assert lost lock and bring
 * link down if idle not seen
 * within 1ms in 1000BASE-T
 */
/* Number of times we will attempt to autonegotiate before downshifting if we
 * are the master
 */
#define M88E1000_EPSCR_MASTER_DOWNSHIFT_MASK 0x0C00
#define M88E1000_EPSCR_MASTER_DOWNSHIFT_1X   0x0000
/* Number of times we will attempt to autonegotiate before downshifting if we
 * are the slave
 */
#define M88E1000_EPSCR_SLAVE_DOWNSHIFT_MASK  0x0300
#define M88E1000_EPSCR_SLAVE_DOWNSHIFT_1X    0x0100
#define M88E1000_EPSCR_TX_CLK_25      0x0070 /* 25  MHz TX_CLK */

/* Intel i347-AT4 Registers */

#define I347AT4_PCDL0                  0x10 /* Pair 0 PHY Cable Diagnostics Length */
#define I347AT4_PCDL1                  0x11 /* Pair 1 PHY Cable Diagnostics Length */
#define I347AT4_PCDL2                  0x12 /* Pair 2 PHY Cable Diagnostics Length */
#define I347AT4_PCDL3                  0x13 /* Pair 3 PHY Cable Diagnostics Length */
#define I347AT4_PCDC                   0x15 /* PHY Cable Diagnostics Control */
#define I347AT4_PAGE_SELECT            0x16

/* i347-AT4 Extended PHY Specific Control Register */

/*  Number of times we will attempt to autonegotiate before downshifting if we
 *  are the master
 */
#define I347AT4_PSCR_DOWNSHIFT_ENABLE 0x0800
#define I347AT4_PSCR_DOWNSHIFT_MASK   0x7000
#define I347AT4_PSCR_DOWNSHIFT_1X     0x0000
#define I347AT4_PSCR_DOWNSHIFT_2X     0x1000
#define I347AT4_PSCR_DOWNSHIFT_3X     0x2000
#define I347AT4_PSCR_DOWNSHIFT_4X     0x3000
#define I347AT4_PSCR_DOWNSHIFT_5X     0x4000
#define I347AT4_PSCR_DOWNSHIFT_6X     0x5000
#define I347AT4_PSCR_DOWNSHIFT_7X     0x6000
#define I347AT4_PSCR_DOWNSHIFT_8X     0x7000

/* i347-AT4 PHY Cable Diagnostics Control */
#define I347AT4_PCDC_CABLE_LENGTH_UNIT 0x0400 /* 0=cm 1=meters */

/* Marvell 1112 only registers */
#define M88E1112_VCT_DSP_DISTANCE       0x001A

/* M88EC018 Rev 2 specific DownShift settings */
#define M88EC018_EPSCR_DOWNSHIFT_COUNTER_MASK  0x0E00
#define M88EC018_EPSCR_DOWNSHIFT_COUNTER_5X    0x0800

/* MDI Control */
#define E1000_MDIC_DATA_MASK 0x0000FFFF
#define E1000_MDIC_REG_MASK  0x001F0000
#define E1000_MDIC_REG_SHIFT 16
#define E1000_MDIC_PHY_MASK  0x03E00000
#define E1000_MDIC_PHY_SHIFT 21
#define E1000_MDIC_OP_WRITE  0x04000000
#define E1000_MDIC_OP_READ   0x08000000
#define E1000_MDIC_READY     0x10000000
#define E1000_MDIC_INT_EN    0x20000000
#define E1000_MDIC_ERROR     0x40000000
#define E1000_MDIC_DEST      0x80000000

/* Thermal Sensor */
#define E1000_THSTAT_PWR_DOWN       0x00000001 /* Power Down Event */
#define E1000_THSTAT_LINK_THROTTLE  0x00000002 /* Link Speed Throttle Event */

/* Energy Efficient Ethernet */
#define E1000_IPCNFG_EEE_1G_AN       0x00000008  /* EEE Enable 1G AN */
#define E1000_IPCNFG_EEE_100M_AN     0x00000004  /* EEE Enable 100M AN */
#define E1000_EEER_TX_LPI_EN         0x00010000  /* EEE Tx LPI Enable */
#define E1000_EEER_RX_LPI_EN         0x00020000  /* EEE Rx LPI Enable */
#define E1000_EEER_FRC_AN            0x10000000  /* Enable EEE in loopback */
#define E1000_EEER_LPI_FC            0x00040000  /* EEE Enable on FC */
#define E1000_EEE_SU_LPI_CLK_STP     0X00800000  /* EEE LPI Clock Stop */
#define E1000_EEER_EEE_NEG           0x20000000  /* EEE capability nego */
#define E1000_EEE_LP_ADV_ADDR_I350   0x040F      /* EEE LP Advertisement */
#define E1000_EEE_LP_ADV_DEV_I210    7           /* EEE LP Adv Device */
#define E1000_EEE_LP_ADV_ADDR_I210   61          /* EEE LP Adv Register */
#define E1000_MMDAC_FUNC_DATA        0x4000      /* Data, no post increment */
#define E1000_M88E1543_PAGE_ADDR	0x16       /* Page Offset Register */
#define E1000_M88E1543_EEE_CTRL_1	0x0
#define E1000_M88E1543_EEE_CTRL_1_MS	0x0001     /* EEE Master/Slave */
#define E1000_M88E1543_FIBER_CTRL	0x0
#define E1000_EEE_ADV_DEV_I354		7
#define E1000_EEE_ADV_ADDR_I354		60
#define E1000_EEE_ADV_100_SUPPORTED	BIT(1)   /* 100BaseTx EEE Supported */
#define E1000_EEE_ADV_1000_SUPPORTED	BIT(2)   /* 1000BaseT EEE Supported */
#define E1000_PCS_STATUS_DEV_I354	3
#define E1000_PCS_STATUS_ADDR_I354	1
#define E1000_PCS_STATUS_TX_LPI_IND	0x0200     /* Tx in LPI state */
#define E1000_PCS_STATUS_RX_LPI_RCVD	0x0400
#define E1000_PCS_STATUS_TX_LPI_RCVD	0x0800

/* SerDes Control */
#define E1000_GEN_CTL_READY             0x80000000
#define E1000_GEN_CTL_ADDRESS_SHIFT     8
#define E1000_GEN_POLL_TIMEOUT          640

#define E1000_VFTA_ENTRY_SHIFT               5
#define E1000_VFTA_ENTRY_MASK                0x7F
#define E1000_VFTA_ENTRY_BIT_SHIFT_MASK      0x1F

/* DMA Coalescing register fields */
#define E1000_PCIEMISC_LX_DECISION      0x00000080 /* Lx power on DMA coal */

/* Tx Rate-Scheduler Config fields */
#define E1000_RTTBCNRC_RS_ENA		0x80000000
#define E1000_RTTBCNRC_RF_DEC_MASK	0x00003FFF
#define E1000_RTTBCNRC_RF_INT_SHIFT	14
#define E1000_RTTBCNRC_RF_INT_MASK	\
	(E1000_RTTBCNRC_RF_DEC_MASK << E1000_RTTBCNRC_RF_INT_SHIFT)

#define E1000_VLAPQF_QUEUE_SEL(_n, q_idx) (q_idx << ((_n) * 4))
#define E1000_VLAPQF_P_VALID(_n)	(0x1 << (3 + (_n) * 4))
#define E1000_VLAPQF_QUEUE_MASK	0x03

/* TX Qav Control fields */
#define E1000_TQAVCTRL_XMIT_MODE	BIT(0)
#define E1000_TQAVCTRL_DATAFETCHARB	BIT(4)
#define E1000_TQAVCTRL_DATATRANARB	BIT(8)
#define E1000_TQAVCTRL_DATATRANTIM	BIT(9)
#define E1000_TQAVCTRL_SP_WAIT_SR	BIT(10)
/* Fetch Time Delta - bits 31:16
 *
 * This field holds the value to be reduced from the launch time for
 * fetch time decision. The FetchTimeDelta value is defined in 32 ns
 * granularity.
 *
 * This field is 16 bits wide, and so the maximum value is:
 *
 * 65535 * 32 = 2097120 ~= 2.1 msec
 *
 * XXX: We are configuring the max value here since we couldn't come up
 * with a reason for not doing so.
 */
#define E1000_TQAVCTRL_FETCHTIME_DELTA	(0xFFFF << 16)

/* TX Qav Credit Control fields */
#define E1000_TQAVCC_IDLESLOPE_MASK	0xFFFF
#define E1000_TQAVCC_QUEUEMODE		BIT(31)

/* Transmit Descriptor Control fields */
#define E1000_TXDCTL_PRIORITY		BIT(27)

#endif
