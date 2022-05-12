/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 1999 - 2018 Intel Corporation. */

#ifndef _IXGBEVF_DEFINES_H_
#define _IXGBEVF_DEFINES_H_

/* Device IDs */
#define IXGBE_DEV_ID_82599_VF		0x10ED
#define IXGBE_DEV_ID_X540_VF		0x1515
#define IXGBE_DEV_ID_X550_VF		0x1565
#define IXGBE_DEV_ID_X550EM_X_VF	0x15A8
#define IXGBE_DEV_ID_X550EM_A_VF	0x15C5

#define IXGBE_DEV_ID_82599_VF_HV	0x152E
#define IXGBE_DEV_ID_X540_VF_HV		0x1530
#define IXGBE_DEV_ID_X550_VF_HV		0x1564
#define IXGBE_DEV_ID_X550EM_X_VF_HV	0x15A9

#define IXGBE_VF_IRQ_CLEAR_MASK		7
#define IXGBE_VF_MAX_TX_QUEUES		8
#define IXGBE_VF_MAX_RX_QUEUES		8

/* DCB define */
#define IXGBE_VF_MAX_TRAFFIC_CLASS	8

/* Link speed */
typedef u32 ixgbe_link_speed;
#define IXGBE_LINK_SPEED_1GB_FULL	0x0020
#define IXGBE_LINK_SPEED_10GB_FULL	0x0080
#define IXGBE_LINK_SPEED_100_FULL	0x0008

#define IXGBE_CTRL_RST		0x04000000 /* Reset (SW) */
#define IXGBE_RXDCTL_ENABLE	0x02000000 /* Enable specific Rx Queue */
#define IXGBE_TXDCTL_ENABLE	0x02000000 /* Enable specific Tx Queue */
#define IXGBE_LINKS_UP		0x40000000
#define IXGBE_LINKS_SPEED_82599		0x30000000
#define IXGBE_LINKS_SPEED_10G_82599	0x30000000
#define IXGBE_LINKS_SPEED_1G_82599	0x20000000
#define IXGBE_LINKS_SPEED_100_82599	0x10000000

/* Number of Transmit and Receive Descriptors must be a multiple of 8 */
#define IXGBE_REQ_TX_DESCRIPTOR_MULTIPLE	8
#define IXGBE_REQ_RX_DESCRIPTOR_MULTIPLE	8
#define IXGBE_REQ_TX_BUFFER_GRANULARITY		1024

/* Interrupt Vector Allocation Registers */
#define IXGBE_IVAR_ALLOC_VAL	0x80 /* Interrupt Allocation valid */

#define IXGBE_VF_INIT_TIMEOUT	200 /* Number of retries to clear RSTI */

/* Receive Config masks */
#define IXGBE_RXCTRL_RXEN	0x00000001  /* Enable Receiver */
#define IXGBE_RXCTRL_DMBYPS	0x00000002  /* Descriptor Monitor Bypass */
#define IXGBE_RXDCTL_ENABLE	0x02000000  /* Enable specific Rx Queue */
#define IXGBE_RXDCTL_VME	0x40000000  /* VLAN mode enable */
#define IXGBE_RXDCTL_RLPMLMASK	0x00003FFF  /* Only supported on the X540 */
#define IXGBE_RXDCTL_RLPML_EN	0x00008000

/* DCA Control */
#define IXGBE_DCA_TXCTRL_TX_WB_RO_EN BIT(11) /* Tx Desc writeback RO bit */

/* PSRTYPE bit definitions */
#define IXGBE_PSRTYPE_TCPHDR	0x00000010
#define IXGBE_PSRTYPE_UDPHDR	0x00000020
#define IXGBE_PSRTYPE_IPV4HDR	0x00000100
#define IXGBE_PSRTYPE_IPV6HDR	0x00000200
#define IXGBE_PSRTYPE_L2HDR	0x00001000

/* SRRCTL bit definitions */
#define IXGBE_SRRCTL_BSIZEPKT_SHIFT	10     /* so many KBs */
#define IXGBE_SRRCTL_RDMTS_SHIFT	22
#define IXGBE_SRRCTL_RDMTS_MASK		0x01C00000
#define IXGBE_SRRCTL_DROP_EN		0x10000000
#define IXGBE_SRRCTL_BSIZEPKT_MASK	0x0000007F
#define IXGBE_SRRCTL_BSIZEHDR_MASK	0x00003F00
#define IXGBE_SRRCTL_DESCTYPE_LEGACY	0x00000000
#define IXGBE_SRRCTL_DESCTYPE_ADV_ONEBUF 0x02000000
#define IXGBE_SRRCTL_DESCTYPE_HDR_SPLIT	0x04000000
#define IXGBE_SRRCTL_DESCTYPE_HDR_REPLICATION_LARGE_PKT 0x08000000
#define IXGBE_SRRCTL_DESCTYPE_HDR_SPLIT_ALWAYS 0x0A000000
#define IXGBE_SRRCTL_DESCTYPE_MASK	0x0E000000

/* Receive Descriptor bit definitions */
#define IXGBE_RXD_STAT_DD	0x01    /* Descriptor Done */
#define IXGBE_RXD_STAT_EOP	0x02    /* End of Packet */
#define IXGBE_RXD_STAT_FLM	0x04    /* FDir Match */
#define IXGBE_RXD_STAT_VP	0x08    /* IEEE VLAN Packet */
#define IXGBE_RXDADV_NEXTP_MASK	0x000FFFF0 /* Next Descriptor Index */
#define IXGBE_RXDADV_NEXTP_SHIFT	0x00000004
#define IXGBE_RXD_STAT_UDPCS	0x10    /* UDP xsum calculated */
#define IXGBE_RXD_STAT_L4CS	0x20    /* L4 xsum calculated */
#define IXGBE_RXD_STAT_IPCS	0x40    /* IP xsum calculated */
#define IXGBE_RXD_STAT_PIF	0x80    /* passed in-exact filter */
#define IXGBE_RXD_STAT_CRCV	0x100   /* Speculative CRC Valid */
#define IXGBE_RXD_STAT_VEXT	0x200   /* 1st VLAN found */
#define IXGBE_RXD_STAT_UDPV	0x400   /* Valid UDP checksum */
#define IXGBE_RXD_STAT_DYNINT	0x800   /* Pkt caused INT via DYNINT */
#define IXGBE_RXD_STAT_TS	0x10000 /* Time Stamp */
#define IXGBE_RXD_STAT_SECP	0x20000 /* Security Processing */
#define IXGBE_RXD_STAT_LB	0x40000 /* Loopback Status */
#define IXGBE_RXD_STAT_ACK	0x8000  /* ACK Packet indication */
#define IXGBE_RXD_ERR_CE	0x01    /* CRC Error */
#define IXGBE_RXD_ERR_LE	0x02    /* Length Error */
#define IXGBE_RXD_ERR_PE	0x08    /* Packet Error */
#define IXGBE_RXD_ERR_OSE	0x10    /* Oversize Error */
#define IXGBE_RXD_ERR_USE	0x20    /* Undersize Error */
#define IXGBE_RXD_ERR_TCPE	0x40    /* TCP/UDP Checksum Error */
#define IXGBE_RXD_ERR_IPE	0x80    /* IP Checksum Error */
#define IXGBE_RXDADV_ERR_MASK	0xFFF00000 /* RDESC.ERRORS mask */
#define IXGBE_RXDADV_ERR_SHIFT	20         /* RDESC.ERRORS shift */
#define IXGBE_RXDADV_ERR_HBO	0x00800000 /*Header Buffer Overflow */
#define IXGBE_RXDADV_ERR_CE	0x01000000 /* CRC Error */
#define IXGBE_RXDADV_ERR_LE	0x02000000 /* Length Error */
#define IXGBE_RXDADV_ERR_PE	0x08000000 /* Packet Error */
#define IXGBE_RXDADV_ERR_OSE	0x10000000 /* Oversize Error */
#define IXGBE_RXDADV_ERR_USE	0x20000000 /* Undersize Error */
#define IXGBE_RXDADV_ERR_TCPE	0x40000000 /* TCP/UDP Checksum Error */
#define IXGBE_RXDADV_ERR_IPE	0x80000000 /* IP Checksum Error */
#define IXGBE_RXD_VLAN_ID_MASK	0x0FFF  /* VLAN ID is in lower 12 bits */
#define IXGBE_RXD_PRI_MASK	0xE000  /* Priority is in upper 3 bits */
#define IXGBE_RXD_PRI_SHIFT	13
#define IXGBE_RXD_CFI_MASK	0x1000  /* CFI is bit 12 */
#define IXGBE_RXD_CFI_SHIFT	12

#define IXGBE_RXDADV_STAT_DD		IXGBE_RXD_STAT_DD  /* Done */
#define IXGBE_RXDADV_STAT_EOP		IXGBE_RXD_STAT_EOP /* End of Packet */
#define IXGBE_RXDADV_STAT_FLM		IXGBE_RXD_STAT_FLM /* FDir Match */
#define IXGBE_RXDADV_STAT_VP		IXGBE_RXD_STAT_VP  /* IEEE VLAN Pkt */
#define IXGBE_RXDADV_STAT_MASK		0x000FFFFF /* Stat/NEXTP: bit 0-19 */
#define IXGBE_RXDADV_STAT_FCEOFS	0x00000040 /* FCoE EOF/SOF Stat */
#define IXGBE_RXDADV_STAT_FCSTAT	0x00000030 /* FCoE Pkt Stat */
#define IXGBE_RXDADV_STAT_FCSTAT_NOMTCH	0x00000000 /* 00: No Ctxt Match */
#define IXGBE_RXDADV_STAT_FCSTAT_NODDP	0x00000010 /* 01: Ctxt w/o DDP */
#define IXGBE_RXDADV_STAT_FCSTAT_FCPRSP	0x00000020 /* 10: Recv. FCP_RSP */
#define IXGBE_RXDADV_STAT_FCSTAT_DDP	0x00000030 /* 11: Ctxt w/ DDP */
#define IXGBE_RXDADV_STAT_SECP		0x00020000 /* IPsec/MACsec pkt found */

#define IXGBE_RXDADV_RSSTYPE_MASK	0x0000000F
#define IXGBE_RXDADV_PKTTYPE_MASK	0x0000FFF0
#define IXGBE_RXDADV_PKTTYPE_IPV4	0x00000010 /* IPv4 hdr present */
#define IXGBE_RXDADV_PKTTYPE_IPV6	0x00000040 /* IPv6 hdr present */
#define IXGBE_RXDADV_PKTTYPE_IPSEC_ESP	0x00001000 /* IPSec ESP */
#define IXGBE_RXDADV_PKTTYPE_IPSEC_AH	0x00002000 /* IPSec AH */
#define IXGBE_RXDADV_PKTTYPE_MASK_EX	0x0001FFF0
#define IXGBE_RXDADV_HDRBUFLEN_MASK	0x00007FE0
#define IXGBE_RXDADV_RSCCNT_MASK	0x001E0000
#define IXGBE_RXDADV_RSCCNT_SHIFT	17
#define IXGBE_RXDADV_HDRBUFLEN_SHIFT	5
#define IXGBE_RXDADV_SPLITHEADER_EN	0x00001000
#define IXGBE_RXDADV_SPH		0x8000

/* RSS Hash results */
#define IXGBE_RXDADV_RSSTYPE_NONE		0x00000000
#define IXGBE_RXDADV_RSSTYPE_IPV4_TCP		0x00000001
#define IXGBE_RXDADV_RSSTYPE_IPV4		0x00000002
#define IXGBE_RXDADV_RSSTYPE_IPV6_TCP		0x00000003
#define IXGBE_RXDADV_RSSTYPE_IPV6_EX		0x00000004
#define IXGBE_RXDADV_RSSTYPE_IPV6		0x00000005
#define IXGBE_RXDADV_RSSTYPE_IPV6_TCP_EX	0x00000006
#define IXGBE_RXDADV_RSSTYPE_IPV4_UDP		0x00000007
#define IXGBE_RXDADV_RSSTYPE_IPV6_UDP		0x00000008
#define IXGBE_RXDADV_RSSTYPE_IPV6_UDP_EX	0x00000009

#define IXGBE_RXD_ERR_FRAME_ERR_MASK ( \
				      IXGBE_RXD_ERR_CE |  \
				      IXGBE_RXD_ERR_LE |  \
				      IXGBE_RXD_ERR_PE |  \
				      IXGBE_RXD_ERR_OSE | \
				      IXGBE_RXD_ERR_USE)

#define IXGBE_RXDADV_ERR_FRAME_ERR_MASK ( \
					 IXGBE_RXDADV_ERR_CE |  \
					 IXGBE_RXDADV_ERR_LE |  \
					 IXGBE_RXDADV_ERR_PE |  \
					 IXGBE_RXDADV_ERR_OSE | \
					 IXGBE_RXDADV_ERR_USE)

#define IXGBE_TXD_POPTS_IXSM	0x01       /* Insert IP checksum */
#define IXGBE_TXD_POPTS_TXSM	0x02       /* Insert TCP/UDP checksum */
#define IXGBE_TXD_CMD_EOP	0x01000000 /* End of Packet */
#define IXGBE_TXD_CMD_IFCS	0x02000000 /* Insert FCS (Ethernet CRC) */
#define IXGBE_TXD_CMD_IC	0x04000000 /* Insert Checksum */
#define IXGBE_TXD_CMD_RS	0x08000000 /* Report Status */
#define IXGBE_TXD_CMD_DEXT	0x20000000 /* Descriptor ext (0 = legacy) */
#define IXGBE_TXD_CMD_VLE	0x40000000 /* Add VLAN tag */
#define IXGBE_TXD_STAT_DD	0x00000001 /* Descriptor Done */
#define IXGBE_TXD_CMD		(IXGBE_TXD_CMD_EOP | IXGBE_TXD_CMD_RS)

/* Transmit Descriptor - Advanced */
union ixgbe_adv_tx_desc {
	struct {
		__le64 buffer_addr;      /* Address of descriptor's data buf */
		__le32 cmd_type_len;
		__le32 olinfo_status;
	} read;
	struct {
		__le64 rsvd;       /* Reserved */
		__le32 nxtseq_seed;
		__le32 status;
	} wb;
};

/* Receive Descriptor - Advanced */
union ixgbe_adv_rx_desc {
	struct {
		__le64 pkt_addr; /* Packet buffer address */
		__le64 hdr_addr; /* Header buffer address */
	} read;
	struct {
		struct {
			union {
				__le32 data;
				struct {
					__le16 pkt_info; /* RSS, Pkt type */
					__le16 hdr_info; /* Splithdr, hdrlen */
				} hs_rss;
			} lo_dword;
			union {
				__le32 rss; /* RSS Hash */
				struct {
					__le16 ip_id; /* IP id */
					__le16 csum; /* Packet Checksum */
				} csum_ip;
			} hi_dword;
		} lower;
		struct {
			__le32 status_error; /* ext status/error */
			__le16 length; /* Packet length */
			__le16 vlan; /* VLAN tag */
		} upper;
	} wb;  /* writeback */
};

/* Context descriptors */
struct ixgbe_adv_tx_context_desc {
	__le32 vlan_macip_lens;
	__le32 fceof_saidx;
	__le32 type_tucmd_mlhl;
	__le32 mss_l4len_idx;
};

/* Adv Transmit Descriptor Config Masks */
#define IXGBE_ADVTXD_DTYP_MASK	0x00F00000 /* DTYP mask */
#define IXGBE_ADVTXD_DTYP_CTXT	0x00200000 /* Advanced Context Desc */
#define IXGBE_ADVTXD_DTYP_DATA	0x00300000 /* Advanced Data Descriptor */
#define IXGBE_ADVTXD_DCMD_EOP	IXGBE_TXD_CMD_EOP  /* End of Packet */
#define IXGBE_ADVTXD_DCMD_IFCS	IXGBE_TXD_CMD_IFCS /* Insert FCS */
#define IXGBE_ADVTXD_DCMD_RS	IXGBE_TXD_CMD_RS   /* Report Status */
#define IXGBE_ADVTXD_DCMD_DEXT	IXGBE_TXD_CMD_DEXT /* Desc ext (1=Adv) */
#define IXGBE_ADVTXD_DCMD_VLE	IXGBE_TXD_CMD_VLE  /* VLAN pkt enable */
#define IXGBE_ADVTXD_DCMD_TSE	0x80000000 /* TCP Seg enable */
#define IXGBE_ADVTXD_STAT_DD	IXGBE_TXD_STAT_DD  /* Descriptor Done */
#define IXGBE_ADVTXD_TUCMD_IPV4	0x00000400  /* IP Packet Type: 1=IPv4 */
#define IXGBE_ADVTXD_TUCMD_IPV6	0x00000000  /* IP Packet Type: 0=IPv6 */
#define IXGBE_ADVTXD_TUCMD_L4T_UDP	0x00000000  /* L4 Packet TYPE of UDP */
#define IXGBE_ADVTXD_TUCMD_L4T_TCP	0x00000800  /* L4 Packet TYPE of TCP */
#define IXGBE_ADVTXD_TUCMD_L4T_SCTP	0x00001000  /* L4 Packet TYPE of SCTP */
#define IXGBE_ADVTXD_TUCMD_IPSEC_TYPE_ESP   0x00002000 /* IPSec Type ESP */
#define IXGBE_ADVTXD_TUCMD_IPSEC_ENCRYPT_EN 0x00004000 /* ESP Encrypt Enable */
#define IXGBE_ADVTXD_IDX_SHIFT	4 /* Adv desc Index shift */
#define IXGBE_ADVTXD_CC		0x00000080 /* Check Context */
#define IXGBE_ADVTXD_POPTS_SHIFT	8  /* Adv desc POPTS shift */
#define IXGBE_ADVTXD_POPTS_IPSEC	0x00000400 /* IPSec offload request */
#define IXGBE_ADVTXD_POPTS_IXSM	(IXGBE_TXD_POPTS_IXSM << \
				 IXGBE_ADVTXD_POPTS_SHIFT)
#define IXGBE_ADVTXD_POPTS_TXSM	(IXGBE_TXD_POPTS_TXSM << \
				 IXGBE_ADVTXD_POPTS_SHIFT)
#define IXGBE_ADVTXD_PAYLEN_SHIFT	14 /* Adv desc PAYLEN shift */
#define IXGBE_ADVTXD_MACLEN_SHIFT	9  /* Adv ctxt desc mac len shift */
#define IXGBE_ADVTXD_VLAN_SHIFT		16 /* Adv ctxt vlan tag shift */
#define IXGBE_ADVTXD_L4LEN_SHIFT	8  /* Adv ctxt L4LEN shift */
#define IXGBE_ADVTXD_MSS_SHIFT		16 /* Adv ctxt MSS shift */

/* Interrupt register bitmasks */

#define IXGBE_EITR_CNT_WDIS	0x80000000
#define IXGBE_MAX_EITR		0x00000FF8
#define IXGBE_MIN_EITR		8

/* Error Codes */
#define IXGBE_ERR_INVALID_MAC_ADDR	-1
#define IXGBE_ERR_RESET_FAILED		-2
#define IXGBE_ERR_INVALID_ARGUMENT	-3

/* Transmit Config masks */
#define IXGBE_TXDCTL_ENABLE		0x02000000 /* Ena specific Tx Queue */
#define IXGBE_TXDCTL_SWFLSH		0x04000000 /* Tx Desc. wr-bk flushing */
#define IXGBE_TXDCTL_WTHRESH_SHIFT	16	   /* shift to WTHRESH bits */

#define IXGBE_DCA_RXCTRL_DESC_DCA_EN	BIT(5)  /* Rx Desc enable */
#define IXGBE_DCA_RXCTRL_HEAD_DCA_EN	BIT(6)  /* Rx Desc header ena */
#define IXGBE_DCA_RXCTRL_DATA_DCA_EN	BIT(7)  /* Rx Desc payload ena */
#define IXGBE_DCA_RXCTRL_DESC_RRO_EN	BIT(9)  /* Rx rd Desc Relax Order */
#define IXGBE_DCA_RXCTRL_DATA_WRO_EN	BIT(13) /* Rx wr data Relax Order */
#define IXGBE_DCA_RXCTRL_HEAD_WRO_EN	BIT(15) /* Rx wr header RO */

#define IXGBE_DCA_TXCTRL_DESC_DCA_EN	BIT(5)  /* DCA Tx Desc enable */
#define IXGBE_DCA_TXCTRL_DESC_RRO_EN	BIT(9)  /* Tx rd Desc Relax Order */
#define IXGBE_DCA_TXCTRL_DESC_WRO_EN	BIT(11) /* Tx Desc writeback RO bit */
#define IXGBE_DCA_TXCTRL_DATA_RRO_EN	BIT(13) /* Tx rd data Relax Order */

#endif /* _IXGBEVF_DEFINES_H_ */
