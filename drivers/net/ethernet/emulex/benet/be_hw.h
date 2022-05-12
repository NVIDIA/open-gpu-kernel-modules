/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2005-2016 Broadcom.
 * All rights reserved.
 *
 * Contact Information:
 * linux-drivers@emulex.com
 *
 * Emulex
 * 3333 Susan Street
 * Costa Mesa, CA 92626
 */

/********* Mailbox door bell *************/
/* Used for driver communication with the FW.
 * The software must write this register twice to post any command. First,
 * it writes the register with hi=1 and the upper bits of the physical address
 * for the MAILBOX structure. Software must poll the ready bit until this
 * is acknowledged. Then, sotware writes the register with hi=0 with the lower
 * bits in the address. It must poll the ready bit until the command is
 * complete. Upon completion, the MAILBOX will contain a valid completion
 * queue entry.
 */
#define MPU_MAILBOX_DB_OFFSET	0x160
#define MPU_MAILBOX_DB_RDY_MASK	0x1 	/* bit 0 */
#define MPU_MAILBOX_DB_HI_MASK	0x2	/* bit 1 */

#define MPU_EP_CONTROL 		0

/********** MPU semphore: used for SH & BE  *************/
#define SLIPORT_SOFTRESET_OFFSET		0x5c	/* CSR BAR offset */
#define SLIPORT_SEMAPHORE_OFFSET_BEx		0xac  /* CSR BAR offset */
#define SLIPORT_SEMAPHORE_OFFSET_SH		0x94  /* PCI-CFG offset */
#define POST_STAGE_MASK				0x0000FFFF
#define POST_ERR_MASK				0x1
#define POST_ERR_SHIFT				31
#define POST_ERR_RECOVERY_CODE_MASK		0xFFF

/* Soft Reset register masks */
#define SLIPORT_SOFTRESET_SR_MASK		0x00000080	/* SR bit */

/* MPU semphore POST stage values */
#define POST_STAGE_AWAITING_HOST_RDY 	0x1 /* FW awaiting goahead from host */
#define POST_STAGE_HOST_RDY 		0x2 /* Host has given go-ahed to FW */
#define POST_STAGE_BE_RESET		0x3 /* Host wants to reset chip */
#define POST_STAGE_ARMFW_RDY		0xc000	/* FW is done with POST */
#define POST_STAGE_RECOVERABLE_ERR	0xE000	/* Recoverable err detected */
/* FW has detected a UE and is dumping FAT log data */
#define POST_STAGE_FAT_LOG_START       0x0D00
#define POST_STAGE_ARMFW_UE            0xF000  /*FW has asserted an UE*/

/* Lancer SLIPORT registers */
#define SLIPORT_STATUS_OFFSET		0x404
#define SLIPORT_CONTROL_OFFSET		0x408
#define SLIPORT_ERROR1_OFFSET		0x40C
#define SLIPORT_ERROR2_OFFSET		0x410
#define PHYSDEV_CONTROL_OFFSET		0x414

#define SLIPORT_STATUS_ERR_MASK		0x80000000
#define SLIPORT_STATUS_DIP_MASK		0x02000000
#define SLIPORT_STATUS_RN_MASK		0x01000000
#define SLIPORT_STATUS_RDY_MASK		0x00800000
#define SLI_PORT_CONTROL_IP_MASK	0x08000000
#define PHYSDEV_CONTROL_FW_RESET_MASK	0x00000002
#define PHYSDEV_CONTROL_DD_MASK		0x00000004
#define PHYSDEV_CONTROL_INP_MASK	0x40000000

#define SLIPORT_ERROR_NO_RESOURCE1	0x2
#define SLIPORT_ERROR_NO_RESOURCE2	0x9

#define SLIPORT_ERROR_FW_RESET1		0x2
#define SLIPORT_ERROR_FW_RESET2		0x0

/********* Memory BAR register ************/
#define PCICFG_MEMBAR_CTRL_INT_CTRL_OFFSET 	0xfc
/* Host Interrupt Enable, if set interrupts are enabled although "PCI Interrupt
 * Disable" may still globally block interrupts in addition to individual
 * interrupt masks; a mechanism for the device driver to block all interrupts
 * atomically without having to arbitrate for the PCI Interrupt Disable bit
 * with the OS.
 */
#define MEMBAR_CTRL_INT_CTRL_HOSTINTR_MASK	BIT(29) /* bit 29 */

/********* PCI Function Capability *********/
#define BE_FUNCTION_CAPS_RSS			0x2
#define BE_FUNCTION_CAPS_SUPER_NIC		0x40

/********* Power management (WOL) **********/
#define PCICFG_PM_CONTROL_OFFSET		0x44
#define PCICFG_PM_CONTROL_MASK			0x108	/* bits 3 & 8 */

/********* Online Control Registers *******/
#define PCICFG_ONLINE0				0xB0
#define PCICFG_ONLINE1				0xB4

/********* UE Status and Mask Registers ***/
#define PCICFG_UE_STATUS_LOW			0xA0
#define PCICFG_UE_STATUS_HIGH			0xA4
#define PCICFG_UE_STATUS_LOW_MASK		0xA8
#define PCICFG_UE_STATUS_HI_MASK		0xAC

/******** SLI_INTF ***********************/
#define SLI_INTF_REG_OFFSET			0x58
#define SLI_INTF_VALID_MASK			0xE0000000
#define SLI_INTF_VALID				0xC0000000
#define SLI_INTF_HINT2_MASK			0x1F000000
#define SLI_INTF_HINT2_SHIFT			24
#define SLI_INTF_HINT1_MASK			0x00FF0000
#define SLI_INTF_HINT1_SHIFT			16
#define SLI_INTF_FAMILY_MASK			0x00000F00
#define SLI_INTF_FAMILY_SHIFT			8
#define SLI_INTF_IF_TYPE_MASK			0x0000F000
#define SLI_INTF_IF_TYPE_SHIFT			12
#define SLI_INTF_REV_MASK			0x000000F0
#define SLI_INTF_REV_SHIFT			4
#define SLI_INTF_FT_MASK			0x00000001

#define SLI_INTF_TYPE_2		2
#define SLI_INTF_TYPE_3		3

/********* ISR0 Register offset **********/
#define CEV_ISR0_OFFSET 			0xC18
#define CEV_ISR_SIZE				4

/********* Event Q door bell *************/
#define DB_EQ_OFFSET			DB_CQ_OFFSET
#define DB_EQ_RING_ID_MASK		0x1FF	/* bits 0 - 8 */
#define DB_EQ_RING_ID_EXT_MASK		0x3e00  /* bits 9-13 */
#define DB_EQ_RING_ID_EXT_MASK_SHIFT	(2) /* qid bits 9-13 placing at 11-15 */

/* Clear the interrupt for this eq */
#define DB_EQ_CLR_SHIFT			(9)	/* bit 9 */
/* Must be 1 */
#define DB_EQ_EVNT_SHIFT		(10)	/* bit 10 */
/* Number of event entries processed */
#define DB_EQ_NUM_POPPED_SHIFT		(16)	/* bits 16 - 28 */
/* Rearm bit */
#define DB_EQ_REARM_SHIFT		(29)	/* bit 29 */
/* Rearm to interrupt delay encoding */
#define DB_EQ_R2I_DLY_SHIFT		(30)    /* bits 30 - 31 */

/* Rearm to interrupt (R2I) delay multiplier encoding represents 3 different
 * values configured in CEV_REARM2IRPT_DLY_MULT_CSR register. This value is
 * programmed by host driver while ringing an EQ doorbell(EQ_DB) if a delay
 * between rearming the EQ and next interrupt on this EQ is desired.
 */
#define	R2I_DLY_ENC_0			0	/* No delay */
#define	R2I_DLY_ENC_1			1	/* maps to 160us EQ delay */
#define	R2I_DLY_ENC_2			2	/* maps to 96us EQ delay */
#define	R2I_DLY_ENC_3			3	/* maps to 48us EQ delay */

/********* Compl Q door bell *************/
#define DB_CQ_OFFSET 			0x120
#define DB_CQ_RING_ID_MASK		0x3FF	/* bits 0 - 9 */
#define DB_CQ_RING_ID_EXT_MASK		0x7C00	/* bits 10-14 */
#define DB_CQ_RING_ID_EXT_MASK_SHIFT	(1)	/* qid bits 10-14
						 placing at 11-15 */

/* Number of event entries processed */
#define DB_CQ_NUM_POPPED_SHIFT		(16) 	/* bits 16 - 28 */
/* Rearm bit */
#define DB_CQ_REARM_SHIFT		(29) 	/* bit 29 */

/********** TX ULP door bell *************/
#define DB_TXULP1_OFFSET		0x60
#define DB_TXULP_RING_ID_MASK		0x7FF	/* bits 0 - 10 */
/* Number of tx entries posted */
#define DB_TXULP_NUM_POSTED_SHIFT	(16)	/* bits 16 - 29 */
#define DB_TXULP_NUM_POSTED_MASK	0x3FFF	/* bits 16 - 29 */

/********** RQ(erx) door bell ************/
#define DB_RQ_OFFSET 			0x100
#define DB_RQ_RING_ID_MASK		0x3FF	/* bits 0 - 9 */
/* Number of rx frags posted */
#define DB_RQ_NUM_POSTED_SHIFT		(24)	/* bits 24 - 31 */

/********** MCC door bell ************/
#define DB_MCCQ_OFFSET 			0x140
#define DB_MCCQ_RING_ID_MASK		0x7FF	/* bits 0 - 10 */
/* Number of entries posted */
#define DB_MCCQ_NUM_POSTED_SHIFT	(16)	/* bits 16 - 29 */

/********** SRIOV VF PCICFG OFFSET ********/
#define SRIOV_VF_PCICFG_OFFSET		(4096)

/********** FAT TABLE  ********/
#define RETRIEVE_FAT	0
#define QUERY_FAT	1

/************* Rx Packet Type Encoding **************/
#define BE_UNICAST_PACKET		0
#define BE_MULTICAST_PACKET		1
#define BE_BROADCAST_PACKET		2
#define BE_RSVD_PACKET			3

/*
 * BE descriptors: host memory data structures whose formats
 * are hardwired in BE silicon.
 */
/* Event Queue Descriptor */
#define EQ_ENTRY_VALID_MASK 		0x1	/* bit 0 */
#define EQ_ENTRY_RES_ID_MASK 		0xFFFF	/* bits 16 - 31 */
#define EQ_ENTRY_RES_ID_SHIFT 		16

struct be_eq_entry {
	u32 evt;
};

/* TX Queue Descriptor */
#define ETH_WRB_FRAG_LEN_MASK		0xFFFF
struct be_eth_wrb {
	__le32 frag_pa_hi;		/* dword 0 */
	__le32 frag_pa_lo;		/* dword 1 */
	u32 rsvd0;			/* dword 2 */
	__le32 frag_len;		/* dword 3: bits 0 - 15 */
} __packed;

/* Pseudo amap definition for eth_hdr_wrb in which each bit of the
 * actual structure is defined as a byte : used to calculate
 * offset/shift/mask of each field */
struct amap_eth_hdr_wrb {
	u8 rsvd0[32];		/* dword 0 */
	u8 rsvd1[32];		/* dword 1 */
	u8 complete;		/* dword 2 */
	u8 event;
	u8 crc;
	u8 forward;
	u8 lso6;
	u8 mgmt;
	u8 ipcs;
	u8 udpcs;
	u8 tcpcs;
	u8 lso;
	u8 vlan;
	u8 gso[2];
	u8 num_wrb[5];
	u8 lso_mss[14];
	u8 len[16];		/* dword 3 */
	u8 vlan_tag[16];
} __packed;

#define TX_HDR_WRB_COMPL		1		/* word 2 */
#define TX_HDR_WRB_EVT			BIT(1)		/* word 2 */
#define TX_HDR_WRB_NUM_SHIFT		13		/* word 2: bits 13:17 */
#define TX_HDR_WRB_NUM_MASK		0x1F		/* word 2: bits 13:17 */

struct be_eth_hdr_wrb {
	__le32 dw[4];
};

/********* Tx Compl Status Encoding *********/
#define BE_TX_COMP_HDR_PARSE_ERR	0x2
#define BE_TX_COMP_NDMA_ERR		0x3
#define BE_TX_COMP_ACL_ERR		0x5

#define LANCER_TX_COMP_LSO_ERR			0x1
#define LANCER_TX_COMP_HSW_DROP_MAC_ERR		0x3
#define LANCER_TX_COMP_HSW_DROP_VLAN_ERR	0x5
#define LANCER_TX_COMP_QINQ_ERR			0x7
#define LANCER_TX_COMP_SGE_ERR			0x9
#define LANCER_TX_COMP_PARITY_ERR		0xb
#define LANCER_TX_COMP_DMA_ERR			0xd

/* TX Compl Queue Descriptor */

/* Pseudo amap definition for eth_tx_compl in which each bit of the
 * actual structure is defined as a byte: used to calculate
 * offset/shift/mask of each field */
struct amap_eth_tx_compl {
	u8 wrb_index[16];	/* dword 0 */
	u8 ct[2]; 		/* dword 0 */
	u8 port[2];		/* dword 0 */
	u8 rsvd0[8];		/* dword 0 */
	u8 status[4];		/* dword 0 */
	u8 user_bytes[16];	/* dword 1 */
	u8 nwh_bytes[8];	/* dword 1 */
	u8 lso;			/* dword 1 */
	u8 cast_enc[2];		/* dword 1 */
	u8 rsvd1[5];		/* dword 1 */
	u8 rsvd2[32];		/* dword 2 */
	u8 pkts[16];		/* dword 3 */
	u8 ringid[11];		/* dword 3 */
	u8 hash_val[4];		/* dword 3 */
	u8 valid;		/* dword 3 */
} __packed;

struct be_eth_tx_compl {
	u32 dw[4];
};

/* RX Queue Descriptor */
struct be_eth_rx_d {
	u32 fragpa_hi;
	u32 fragpa_lo;
};

/* RX Compl Queue Descriptor */

/* Pseudo amap definition for BE2 and BE3 legacy mode eth_rx_compl in which
 * each bit of the actual structure is defined as a byte: used to calculate
 * offset/shift/mask of each field */
struct amap_eth_rx_compl_v0 {
	u8 vlan_tag[16];	/* dword 0 */
	u8 pktsize[14];		/* dword 0 */
	u8 port;		/* dword 0 */
	u8 ip_opt;		/* dword 0 */
	u8 err;			/* dword 1 */
	u8 rsshp;		/* dword 1 */
	u8 ipf;			/* dword 1 */
	u8 tcpf;		/* dword 1 */
	u8 udpf;		/* dword 1 */
	u8 ipcksm;		/* dword 1 */
	u8 l4_cksm;		/* dword 1 */
	u8 ip_version;		/* dword 1 */
	u8 macdst[6];		/* dword 1 */
	u8 vtp;			/* dword 1 */
	u8 ip_frag;		/* dword 1 */
	u8 fragndx[10];		/* dword 1 */
	u8 ct[2];		/* dword 1 */
	u8 sw;			/* dword 1 */
	u8 numfrags[3];		/* dword 1 */
	u8 rss_flush;		/* dword 2 */
	u8 cast_enc[2];		/* dword 2 */
	u8 qnq;			/* dword 2 */
	u8 rss_bank;		/* dword 2 */
	u8 rsvd1[23];		/* dword 2 */
	u8 lro_pkt;		/* dword 2 */
	u8 rsvd2[2];		/* dword 2 */
	u8 valid;		/* dword 2 */
	u8 rsshash[32];		/* dword 3 */
} __packed;

/* Pseudo amap definition for BE3 native mode eth_rx_compl in which
 * each bit of the actual structure is defined as a byte: used to calculate
 * offset/shift/mask of each field */
struct amap_eth_rx_compl_v1 {
	u8 vlan_tag[16];	/* dword 0 */
	u8 pktsize[14];		/* dword 0 */
	u8 vtp;			/* dword 0 */
	u8 ip_opt;		/* dword 0 */
	u8 err;			/* dword 1 */
	u8 rsshp;		/* dword 1 */
	u8 ipf;			/* dword 1 */
	u8 tcpf;		/* dword 1 */
	u8 udpf;		/* dword 1 */
	u8 ipcksm;		/* dword 1 */
	u8 l4_cksm;		/* dword 1 */
	u8 ip_version;		/* dword 1 */
	u8 macdst[7];		/* dword 1 */
	u8 rsvd0;		/* dword 1 */
	u8 fragndx[10];		/* dword 1 */
	u8 ct[2];		/* dword 1 */
	u8 sw;			/* dword 1 */
	u8 numfrags[3];		/* dword 1 */
	u8 rss_flush;		/* dword 2 */
	u8 cast_enc[2];		/* dword 2 */
	u8 qnq;			/* dword 2 */
	u8 rss_bank;		/* dword 2 */
	u8 port[2];		/* dword 2 */
	u8 vntagp;		/* dword 2 */
	u8 header_len[8];	/* dword 2 */
	u8 header_split[2];	/* dword 2 */
	u8 rsvd1[12];		/* dword 2 */
	u8 tunneled;
	u8 valid;		/* dword 2 */
	u8 rsshash[32];		/* dword 3 */
} __packed;

struct be_eth_rx_compl {
	u32 dw[4];
};
