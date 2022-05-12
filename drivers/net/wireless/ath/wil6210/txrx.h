/* SPDX-License-Identifier: ISC */
/*
 * Copyright (c) 2012-2016 Qualcomm Atheros, Inc.
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved.
 */

#ifndef WIL6210_TXRX_H
#define WIL6210_TXRX_H

#include "wil6210.h"
#include "txrx_edma.h"

#define BUF_SW_OWNED    (1)
#define BUF_HW_OWNED    (0)

/* default size of MAC Tx/Rx buffers */
#define TXRX_BUF_LEN_DEFAULT (2048)

/* how many bytes to reserve for rtap header? */
#define WIL6210_RTAP_SIZE (128)

/* Tx/Rx path */

static inline dma_addr_t wil_desc_addr(struct wil_ring_dma_addr *addr)
{
	return le32_to_cpu(addr->addr_low) |
			   ((u64)le16_to_cpu(addr->addr_high) << 32);
}

static inline void wil_desc_addr_set(struct wil_ring_dma_addr *addr,
				     dma_addr_t pa)
{
	addr->addr_low = cpu_to_le32(lower_32_bits(pa));
	addr->addr_high = cpu_to_le16((u16)upper_32_bits(pa));
}

/* Tx descriptor - MAC part
 * [dword 0]
 * bit  0.. 9 : lifetime_expiry_value:10
 * bit     10 : interrupt_en:1
 * bit     11 : status_en:1
 * bit 12..13 : txss_override:2
 * bit     14 : timestamp_insertion:1
 * bit     15 : duration_preserve:1
 * bit 16..21 : reserved0:6
 * bit 22..26 : mcs_index:5
 * bit     27 : mcs_en:1
 * bit 28..30 : reserved1:3
 * bit     31 : sn_preserved:1
 * [dword 1]
 * bit  0.. 3 : pkt_mode:4
 * bit      4 : pkt_mode_en:1
 * bit      5 : mac_id_en:1
 * bit   6..7 : mac_id:2
 * bit  8..14 : reserved0:7
 * bit     15 : ack_policy_en:1
 * bit 16..19 : dst_index:4
 * bit     20 : dst_index_en:1
 * bit 21..22 : ack_policy:2
 * bit     23 : lifetime_en:1
 * bit 24..30 : max_retry:7
 * bit     31 : max_retry_en:1
 * [dword 2]
 * bit  0.. 7 : num_of_descriptors:8
 * bit  8..17 : reserved:10
 * bit 18..19 : l2_translation_type:2 00 - bypass, 01 - 802.3, 10 - 802.11
 * bit     20 : snap_hdr_insertion_en:1
 * bit     21 : vlan_removal_en:1
 * bit 22..31 : reserved0:10
 * [dword 3]
 * bit  0.. 31: ucode_cmd:32
 */
struct vring_tx_mac {
	u32 d[3];
	u32 ucode_cmd;
} __packed;

/* TX MAC Dword 0 */
#define MAC_CFG_DESC_TX_0_LIFETIME_EXPIRY_VALUE_POS 0
#define MAC_CFG_DESC_TX_0_LIFETIME_EXPIRY_VALUE_LEN 10
#define MAC_CFG_DESC_TX_0_LIFETIME_EXPIRY_VALUE_MSK 0x3FF

#define MAC_CFG_DESC_TX_0_INTERRUP_EN_POS 10
#define MAC_CFG_DESC_TX_0_INTERRUP_EN_LEN 1
#define MAC_CFG_DESC_TX_0_INTERRUP_EN_MSK 0x400

#define MAC_CFG_DESC_TX_0_STATUS_EN_POS 11
#define MAC_CFG_DESC_TX_0_STATUS_EN_LEN 1
#define MAC_CFG_DESC_TX_0_STATUS_EN_MSK 0x800

#define MAC_CFG_DESC_TX_0_TXSS_OVERRIDE_POS 12
#define MAC_CFG_DESC_TX_0_TXSS_OVERRIDE_LEN 2
#define MAC_CFG_DESC_TX_0_TXSS_OVERRIDE_MSK 0x3000

#define MAC_CFG_DESC_TX_0_TIMESTAMP_INSERTION_POS 14
#define MAC_CFG_DESC_TX_0_TIMESTAMP_INSERTION_LEN 1
#define MAC_CFG_DESC_TX_0_TIMESTAMP_INSERTION_MSK 0x4000

#define MAC_CFG_DESC_TX_0_DURATION_PRESERVE_POS 15
#define MAC_CFG_DESC_TX_0_DURATION_PRESERVE_LEN 1
#define MAC_CFG_DESC_TX_0_DURATION_PRESERVE_MSK 0x8000

#define MAC_CFG_DESC_TX_0_MCS_INDEX_POS 22
#define MAC_CFG_DESC_TX_0_MCS_INDEX_LEN 5
#define MAC_CFG_DESC_TX_0_MCS_INDEX_MSK 0x7C00000

#define MAC_CFG_DESC_TX_0_MCS_EN_POS 27
#define MAC_CFG_DESC_TX_0_MCS_EN_LEN 1
#define MAC_CFG_DESC_TX_0_MCS_EN_MSK 0x8000000

#define MAC_CFG_DESC_TX_0_SN_PRESERVED_POS 31
#define MAC_CFG_DESC_TX_0_SN_PRESERVED_LEN 1
#define MAC_CFG_DESC_TX_0_SN_PRESERVED_MSK 0x80000000

/* TX MAC Dword 1 */
#define MAC_CFG_DESC_TX_1_PKT_MODE_POS 0
#define MAC_CFG_DESC_TX_1_PKT_MODE_LEN 4
#define MAC_CFG_DESC_TX_1_PKT_MODE_MSK 0xF

#define MAC_CFG_DESC_TX_1_PKT_MODE_EN_POS 4
#define MAC_CFG_DESC_TX_1_PKT_MODE_EN_LEN 1
#define MAC_CFG_DESC_TX_1_PKT_MODE_EN_MSK 0x10

#define MAC_CFG_DESC_TX_1_MAC_ID_EN_POS 5
#define MAC_CFG_DESC_TX_1_MAC_ID_EN_LEN 1
#define MAC_CFG_DESC_TX_1_MAC_ID_EN_MSK 0x20

#define MAC_CFG_DESC_TX_1_MAC_ID_POS 6
#define MAC_CFG_DESC_TX_1_MAC_ID_LEN 2
#define MAC_CFG_DESC_TX_1_MAC_ID_MSK 0xc0

#define MAC_CFG_DESC_TX_1_ACK_POLICY_EN_POS 15
#define MAC_CFG_DESC_TX_1_ACK_POLICY_EN_LEN 1
#define MAC_CFG_DESC_TX_1_ACK_POLICY_EN_MSK 0x8000

#define MAC_CFG_DESC_TX_1_DST_INDEX_POS 16
#define MAC_CFG_DESC_TX_1_DST_INDEX_LEN 4
#define MAC_CFG_DESC_TX_1_DST_INDEX_MSK 0xF0000

#define MAC_CFG_DESC_TX_1_DST_INDEX_EN_POS 20
#define MAC_CFG_DESC_TX_1_DST_INDEX_EN_LEN 1
#define MAC_CFG_DESC_TX_1_DST_INDEX_EN_MSK 0x100000

#define MAC_CFG_DESC_TX_1_ACK_POLICY_POS 21
#define MAC_CFG_DESC_TX_1_ACK_POLICY_LEN 2
#define MAC_CFG_DESC_TX_1_ACK_POLICY_MSK 0x600000

#define MAC_CFG_DESC_TX_1_LIFETIME_EN_POS 23
#define MAC_CFG_DESC_TX_1_LIFETIME_EN_LEN 1
#define MAC_CFG_DESC_TX_1_LIFETIME_EN_MSK 0x800000

#define MAC_CFG_DESC_TX_1_MAX_RETRY_POS 24
#define MAC_CFG_DESC_TX_1_MAX_RETRY_LEN 7
#define MAC_CFG_DESC_TX_1_MAX_RETRY_MSK 0x7F000000

#define MAC_CFG_DESC_TX_1_MAX_RETRY_EN_POS 31
#define MAC_CFG_DESC_TX_1_MAX_RETRY_EN_LEN 1
#define MAC_CFG_DESC_TX_1_MAX_RETRY_EN_MSK 0x80000000

/* TX MAC Dword 2 */
#define MAC_CFG_DESC_TX_2_NUM_OF_DESCRIPTORS_POS 0
#define MAC_CFG_DESC_TX_2_NUM_OF_DESCRIPTORS_LEN 8
#define MAC_CFG_DESC_TX_2_NUM_OF_DESCRIPTORS_MSK 0xFF

#define MAC_CFG_DESC_TX_2_RESERVED_POS 8
#define MAC_CFG_DESC_TX_2_RESERVED_LEN 10
#define MAC_CFG_DESC_TX_2_RESERVED_MSK 0x3FF00

#define MAC_CFG_DESC_TX_2_L2_TRANSLATION_TYPE_POS 18
#define MAC_CFG_DESC_TX_2_L2_TRANSLATION_TYPE_LEN 2
#define MAC_CFG_DESC_TX_2_L2_TRANSLATION_TYPE_MSK 0xC0000

#define MAC_CFG_DESC_TX_2_SNAP_HDR_INSERTION_EN_POS 20
#define MAC_CFG_DESC_TX_2_SNAP_HDR_INSERTION_EN_LEN 1
#define MAC_CFG_DESC_TX_2_SNAP_HDR_INSERTION_EN_MSK 0x100000

#define MAC_CFG_DESC_TX_2_VLAN_REMOVAL_EN_POS 21
#define MAC_CFG_DESC_TX_2_VLAN_REMOVAL_EN_LEN 1
#define MAC_CFG_DESC_TX_2_VLAN_REMOVAL_EN_MSK 0x200000

/* TX MAC Dword 3 */
#define MAC_CFG_DESC_TX_3_UCODE_CMD_POS 0
#define MAC_CFG_DESC_TX_3_UCODE_CMD_LEN 32
#define MAC_CFG_DESC_TX_3_UCODE_CMD_MSK 0xFFFFFFFF

/* TX DMA Dword 0 */
#define DMA_CFG_DESC_TX_0_L4_LENGTH_POS 0
#define DMA_CFG_DESC_TX_0_L4_LENGTH_LEN 8
#define DMA_CFG_DESC_TX_0_L4_LENGTH_MSK 0xFF

#define DMA_CFG_DESC_TX_0_CMD_EOP_POS 8
#define DMA_CFG_DESC_TX_0_CMD_EOP_LEN 1
#define DMA_CFG_DESC_TX_0_CMD_EOP_MSK 0x100

#define DMA_CFG_DESC_TX_0_CMD_MARK_WB_POS 9
#define DMA_CFG_DESC_TX_0_CMD_MARK_WB_LEN 1
#define DMA_CFG_DESC_TX_0_CMD_MARK_WB_MSK 0x200

#define DMA_CFG_DESC_TX_0_CMD_DMA_IT_POS 10
#define DMA_CFG_DESC_TX_0_CMD_DMA_IT_LEN 1
#define DMA_CFG_DESC_TX_0_CMD_DMA_IT_MSK 0x400

#define DMA_CFG_DESC_TX_0_SEGMENT_BUF_DETAILS_POS 11
#define DMA_CFG_DESC_TX_0_SEGMENT_BUF_DETAILS_LEN 2
#define DMA_CFG_DESC_TX_0_SEGMENT_BUF_DETAILS_MSK 0x1800

#define DMA_CFG_DESC_TX_0_TCP_SEG_EN_POS 13
#define DMA_CFG_DESC_TX_0_TCP_SEG_EN_LEN 1
#define DMA_CFG_DESC_TX_0_TCP_SEG_EN_MSK 0x2000

#define DMA_CFG_DESC_TX_0_IPV4_CHECKSUM_EN_POS 14
#define DMA_CFG_DESC_TX_0_IPV4_CHECKSUM_EN_LEN 1
#define DMA_CFG_DESC_TX_0_IPV4_CHECKSUM_EN_MSK 0x4000

#define DMA_CFG_DESC_TX_0_TCP_UDP_CHECKSUM_EN_POS 15
#define DMA_CFG_DESC_TX_0_TCP_UDP_CHECKSUM_EN_LEN 1
#define DMA_CFG_DESC_TX_0_TCP_UDP_CHECKSUM_EN_MSK 0x8000

#define DMA_CFG_DESC_TX_0_QID_POS 16
#define DMA_CFG_DESC_TX_0_QID_LEN 5
#define DMA_CFG_DESC_TX_0_QID_MSK 0x1F0000

#define DMA_CFG_DESC_TX_0_PSEUDO_HEADER_CALC_EN_POS 21
#define DMA_CFG_DESC_TX_0_PSEUDO_HEADER_CALC_EN_LEN 1
#define DMA_CFG_DESC_TX_0_PSEUDO_HEADER_CALC_EN_MSK 0x200000

#define DMA_CFG_DESC_TX_0_L4_TYPE_POS 30
#define DMA_CFG_DESC_TX_0_L4_TYPE_LEN 2
#define DMA_CFG_DESC_TX_0_L4_TYPE_MSK 0xC0000000 /* L4 type: 0-UDP, 2-TCP */

#define DMA_CFG_DESC_TX_OFFLOAD_CFG_MAC_LEN_POS 0
#define DMA_CFG_DESC_TX_OFFLOAD_CFG_MAC_LEN_LEN 7
#define DMA_CFG_DESC_TX_OFFLOAD_CFG_MAC_LEN_MSK 0x7F /* MAC hdr len */

#define DMA_CFG_DESC_TX_OFFLOAD_CFG_L3T_IPV4_POS 7
#define DMA_CFG_DESC_TX_OFFLOAD_CFG_L3T_IPV4_LEN 1
#define DMA_CFG_DESC_TX_OFFLOAD_CFG_L3T_IPV4_MSK 0x80 /* 1-IPv4, 0-IPv6 */

#define TX_DMA_STATUS_DU         BIT(0)

/* Tx descriptor - DMA part
 * [dword 0]
 * bit  0.. 7 : l4_length:8 layer 4 length
 * bit      8 : cmd_eop:1 This descriptor is the last one in the packet
 * bit      9 : reserved
 * bit     10 : cmd_dma_it:1 immediate interrupt
 * bit 11..12 : SBD - Segment Buffer Details
 *		00 - Header Segment
 *		01 - First Data Segment
 *		10 - Medium Data Segment
 *		11 - Last Data Segment
 * bit     13 : TSE - TCP Segmentation Enable
 * bit     14 : IIC - Directs the HW to Insert IPv4 Checksum
 * bit     15 : ITC - Directs the HW to Insert TCP/UDP Checksum
 * bit 16..20 : QID - The target QID that the packet should be stored
 *		in the MAC.
 * bit     21 : PO - Pseudo header Offload:
 *		0 - Use the pseudo header value from the TCP checksum field
 *		1- Calculate Pseudo header Checksum
 * bit     22 : NC - No UDP Checksum
 * bit 23..29 : reserved
 * bit 30..31 : L4T - Layer 4 Type: 00 - UDP , 10 - TCP , 10, 11 - Reserved
 *		If L4Len equal 0, no L4 at all
 * [dword 1]
 * bit  0..31 : addr_low:32 The payload buffer low address
 * [dword 2]
 * bit  0..15 : addr_high:16 The payload buffer high address
 * bit 16..23 : ip_length:8 The IP header length for the TX IP checksum
 *		offload feature
 * bit 24..30 : mac_length:7
 * bit     31 : ip_version:1 1 - IPv4, 0 - IPv6
 * [dword 3]
 *  [byte 12] error
 * bit  0   2 : mac_status:3
 * bit  3   7 : reserved:5
 *  [byte 13] status
 * bit      0 : DU:1 Descriptor Used
 * bit  1   7 : reserved:7
 *  [word 7] length
 */
struct vring_tx_dma {
	u32 d0;
	struct wil_ring_dma_addr addr;
	u8  ip_length;
	u8  b11;       /* 0..6: mac_length; 7:ip_version */
	u8  error;     /* 0..2: err; 3..7: reserved; */
	u8  status;    /* 0: used; 1..7; reserved */
	__le16 length;
} __packed;

/* TSO type used in dma descriptor d0 bits 11-12 */
enum {
	wil_tso_type_hdr = 0,
	wil_tso_type_first = 1,
	wil_tso_type_mid  = 2,
	wil_tso_type_lst  = 3,
};

/* Rx descriptor - MAC part
 * [dword 0]
 * bit  0.. 3 : tid:4 The QoS (b3-0) TID Field
 * bit  4.. 6 : cid:3 The Source index that  was found during parsing the TA.
 *		This field is used to define the source of the packet
 * bit      7 : MAC_id_valid:1, 1 if MAC virtual number is valid.
 * bit  8.. 9 : mid:2 The MAC virtual number
 * bit 10..11 : frame_type:2 : The FC (b3-2) - MPDU Type
 *		(management, data, control and extension)
 * bit 12..15 : frame_subtype:4 : The FC (b7-4) - Frame Subtype
 * bit 16..27 : seq_number:12 The received Sequence number field
 * bit 28..31 : extended:4 extended subtype
 * [dword 1]
 * bit  0.. 3 : reserved
 * bit  4.. 5 : key_id:2
 * bit      6 : decrypt_bypass:1
 * bit      7 : security:1 FC (b14)
 * bit  8.. 9 : ds_bits:2 FC (b9-8)
 * bit     10 : a_msdu_present:1  QoS (b7)
 * bit     11 : a_msdu_type:1  QoS (b8)
 * bit     12 : a_mpdu:1  part of AMPDU aggregation
 * bit     13 : broadcast:1
 * bit     14 : mutlicast:1
 * bit     15 : reserved:1
 * bit 16..20 : rx_mac_qid:5 The Queue Identifier that the packet
 *		is received from
 * bit 21..24 : mcs:4
 * bit 25..28 : mic_icr:4 this signal tells the DMA to assert an interrupt
 *		after it writes the packet
 * bit 29..31 : reserved:3
 * [dword 2]
 * bit  0.. 2 : time_slot:3 The timeslot that the MPDU is received
 * bit  3.. 4 : fc_protocol_ver:1 The FC (b1-0) - Protocol Version
 * bit      5 : fc_order:1 The FC Control (b15) -Order
 * bit  6.. 7 : qos_ack_policy:2 The QoS (b6-5) ack policy Field
 * bit      8 : esop:1 The QoS (b4) ESOP field
 * bit      9 : qos_rdg_more_ppdu:1 The QoS (b9) RDG field
 * bit 10..14 : qos_reserved:5 The QoS (b14-10) Reserved field
 * bit     15 : qos_ac_constraint:1 QoS (b15)
 * bit 16..31 : pn_15_0:16 low 2 bytes of PN
 * [dword 3]
 * bit  0..31 : pn_47_16:32 high 4 bytes of PN
 */
struct vring_rx_mac {
	u32 d0;
	u32 d1;
	u16 w4;
	u16 pn_15_0;
	u32 pn_47_16;
} __packed;

/* Rx descriptor - DMA part
 * [dword 0]
 * bit  0.. 7 : l4_length:8 layer 4 length. The field is only valid if
 *		L4I bit is set
 * bit      8 : cmd_eop:1 set to 1
 * bit      9 : cmd_rt:1 set to 1
 * bit     10 : cmd_dma_it:1 immediate interrupt
 * bit 11..15 : reserved:5
 * bit 16..29 : phy_info_length:14 It is valid when the PII is set.
 *		When the FFM bit is set bits 29-27 are used for for
 *		Flex Filter Match. Matching Index to one of the L2
 *		EtherType Flex Filter
 * bit 30..31 : l4_type:2 valid if the L4I bit is set in the status field
 *		00 - UDP, 01 - TCP, 10, 11 - reserved
 * [dword 1]
 * bit  0..31 : addr_low:32 The payload buffer low address
 * [dword 2]
 * bit  0..15 : addr_high:16 The payload buffer high address
 * bit 16..23 : ip_length:8 The filed is valid only if the L3I bit is set
 * bit 24..30 : mac_length:7
 * bit     31 : ip_version:1 1 - IPv4, 0 - IPv6
 * [dword 3]
 *  [byte 12] error
 * bit      0 : FCS:1
 * bit      1 : MIC:1
 * bit      2 : Key miss:1
 * bit      3 : Replay:1
 * bit      4 : L3:1 IPv4 checksum
 * bit      5 : L4:1 TCP/UDP checksum
 * bit  6   7 : reserved:2
 *  [byte 13] status
 * bit      0 : DU:1 Descriptor Used
 * bit      1 : EOP:1 The descriptor indicates the End of Packet
 * bit      2 : error:1
 * bit      3 : MI:1 MAC Interrupt is asserted (according to parser decision)
 * bit      4 : L3I:1 L3 identified and checksum calculated
 * bit      5 : L4I:1 L4 identified and checksum calculated
 * bit      6 : PII:1 PHY Info Included in the packet
 * bit      7 : FFM:1 EtherType Flex Filter Match
 *  [word 7] length
 */

#define RX_DMA_D0_CMD_DMA_EOP	BIT(8)
#define RX_DMA_D0_CMD_DMA_RT	BIT(9)  /* always 1 */
#define RX_DMA_D0_CMD_DMA_IT	BIT(10) /* interrupt */
#define RX_MAC_D0_MAC_ID_VALID	BIT(7)

/* Error field */
#define RX_DMA_ERROR_FCS	BIT(0)
#define RX_DMA_ERROR_MIC	BIT(1)
#define RX_DMA_ERROR_KEY	BIT(2) /* Key missing */
#define RX_DMA_ERROR_REPLAY	BIT(3)
#define RX_DMA_ERROR_L3_ERR	BIT(4)
#define RX_DMA_ERROR_L4_ERR	BIT(5)

/* Status field */
#define RX_DMA_STATUS_DU	BIT(0)
#define RX_DMA_STATUS_EOP	BIT(1)
#define RX_DMA_STATUS_ERROR	BIT(2)
#define RX_DMA_STATUS_MI	BIT(3) /* MAC Interrupt is asserted */
#define RX_DMA_STATUS_L3I	BIT(4)
#define RX_DMA_STATUS_L4I	BIT(5)
#define RX_DMA_STATUS_PHY_INFO	BIT(6)
#define RX_DMA_STATUS_FFM	BIT(7) /* EtherType Flex Filter Match */

/* IEEE 802.11, 8.5.2 EAPOL-Key frames */
#define WIL_KEY_INFO_KEY_TYPE BIT(3) /* val of 1 = Pairwise, 0 = Group key */

#define WIL_KEY_INFO_MIC BIT(8)
#define WIL_KEY_INFO_ENCR_KEY_DATA BIT(12) /* for rsn only */

#define WIL_EAP_NONCE_LEN 32
#define WIL_EAP_KEY_RSC_LEN 8
#define WIL_EAP_REPLAY_COUNTER_LEN 8
#define WIL_EAP_KEY_IV_LEN 16
#define WIL_EAP_KEY_ID_LEN 8

enum {
	WIL_1X_TYPE_EAP_PACKET = 0,
	WIL_1X_TYPE_EAPOL_START = 1,
	WIL_1X_TYPE_EAPOL_LOGOFF = 2,
	WIL_1X_TYPE_EAPOL_KEY = 3,
};

#define WIL_EAPOL_KEY_TYPE_RSN 2
#define WIL_EAPOL_KEY_TYPE_WPA 254

struct wil_1x_hdr {
	u8 version;
	u8 type;
	__be16 length;
	/* followed by data */
} __packed;

struct wil_eapol_key {
	u8 type;
	__be16 key_info;
	__be16 key_length;
	u8 replay_counter[WIL_EAP_REPLAY_COUNTER_LEN];
	u8 key_nonce[WIL_EAP_NONCE_LEN];
	u8 key_iv[WIL_EAP_KEY_IV_LEN];
	u8 key_rsc[WIL_EAP_KEY_RSC_LEN];
	u8 key_id[WIL_EAP_KEY_ID_LEN];
} __packed;

struct vring_rx_dma {
	u32 d0;
	struct wil_ring_dma_addr addr;
	u8  ip_length;
	u8  b11;
	u8  error;
	u8  status;
	__le16 length;
} __packed;

struct vring_tx_desc {
	struct vring_tx_mac mac;
	struct vring_tx_dma dma;
} __packed;

union wil_tx_desc {
	struct vring_tx_desc legacy;
	struct wil_tx_enhanced_desc enhanced;
} __packed;

struct vring_rx_desc {
	struct vring_rx_mac mac;
	struct vring_rx_dma dma;
} __packed;

union wil_rx_desc {
	struct vring_rx_desc legacy;
	struct wil_rx_enhanced_desc enhanced;
} __packed;

union wil_ring_desc {
	union wil_tx_desc tx;
	union wil_rx_desc rx;
} __packed;

struct packet_rx_info {
	u8 cid;
};

/* this struct will be stored in the skb cb buffer
 * max length of the struct is limited to 48 bytes
 */
struct skb_rx_info {
	struct vring_rx_desc rx_desc;
	struct packet_rx_info rx_info;
};

static inline int wil_rxdesc_tid(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d0, 0, 3);
}

static inline int wil_rxdesc_cid(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d0, 4, 6);
}

static inline int wil_rxdesc_mid(struct vring_rx_desc *d)
{
	return (d->mac.d0 & RX_MAC_D0_MAC_ID_VALID) ?
		WIL_GET_BITS(d->mac.d0, 8, 9) : 0;
}

static inline int wil_rxdesc_ftype(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d0, 10, 11);
}

static inline int wil_rxdesc_subtype(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d0, 12, 15);
}

/* 1-st byte (with frame type/subtype) of FC field */
static inline u8 wil_rxdesc_fc1(struct vring_rx_desc *d)
{
	return (u8)(WIL_GET_BITS(d->mac.d0, 10, 15) << 2);
}

static inline int wil_rxdesc_seq(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d0, 16, 27);
}

static inline int wil_rxdesc_ext_subtype(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d0, 28, 31);
}

static inline int wil_rxdesc_retry(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d0, 31, 31);
}

static inline int wil_rxdesc_key_id(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d1, 4, 5);
}

static inline int wil_rxdesc_security(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d1, 7, 7);
}

static inline int wil_rxdesc_ds_bits(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d1, 8, 9);
}

static inline int wil_rxdesc_mcs(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d1, 21, 24);
}

static inline int wil_rxdesc_mcast(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d1, 13, 14);
}

static inline struct vring_rx_desc *wil_skb_rxdesc(struct sk_buff *skb)
{
	return (void *)skb->cb;
}

static inline int wil_ring_is_empty(struct wil_ring *ring)
{
	return ring->swhead == ring->swtail;
}

static inline u32 wil_ring_next_tail(struct wil_ring *ring)
{
	return (ring->swtail + 1) % ring->size;
}

static inline void wil_ring_advance_head(struct wil_ring *ring, int n)
{
	ring->swhead = (ring->swhead + n) % ring->size;
}

static inline int wil_ring_is_full(struct wil_ring *ring)
{
	return wil_ring_next_tail(ring) == ring->swhead;
}

static inline u8 *wil_skb_get_da(struct sk_buff *skb)
{
	struct ethhdr *eth = (void *)skb->data;

	return eth->h_dest;
}

static inline u8 *wil_skb_get_sa(struct sk_buff *skb)
{
	struct ethhdr *eth = (void *)skb->data;

	return eth->h_source;
}

static inline bool wil_need_txstat(struct sk_buff *skb)
{
	const u8 *da = wil_skb_get_da(skb);

	return is_unicast_ether_addr(da) && skb->sk &&
	       (skb_shinfo(skb)->tx_flags & SKBTX_WIFI_STATUS);
}

static inline void wil_consume_skb(struct sk_buff *skb, bool acked)
{
	if (unlikely(wil_need_txstat(skb)))
		skb_complete_wifi_ack(skb, acked);
	else
		acked ? dev_consume_skb_any(skb) : dev_kfree_skb_any(skb);
}

/* Used space in Tx ring */
static inline int wil_ring_used_tx(struct wil_ring *ring)
{
	u32 swhead = ring->swhead;
	u32 swtail = ring->swtail;

	return (ring->size + swhead - swtail) % ring->size;
}

/* Available space in Tx ring */
static inline int wil_ring_avail_tx(struct wil_ring *ring)
{
	return ring->size - wil_ring_used_tx(ring) - 1;
}

static inline int wil_get_min_tx_ring_id(struct wil6210_priv *wil)
{
	/* In Enhanced DMA ring 0 is reserved for RX */
	return wil->use_enhanced_dma_hw ? 1 : 0;
}

/* similar to ieee80211_ version, but FC contain only 1-st byte */
static inline int wil_is_back_req(u8 fc)
{
	return (fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       (IEEE80211_FTYPE_CTL | IEEE80211_STYPE_BACK_REQ);
}

/* wil_val_in_range - check if value in [min,max) */
static inline bool wil_val_in_range(int val, int min, int max)
{
	return val >= min && val < max;
}

static inline u8 wil_skb_get_cid(struct sk_buff *skb)
{
	struct skb_rx_info *skb_rx_info = (void *)skb->cb;

	return skb_rx_info->rx_info.cid;
}

static inline void wil_skb_set_cid(struct sk_buff *skb, u8 cid)
{
	struct skb_rx_info *skb_rx_info = (void *)skb->cb;

	skb_rx_info->rx_info.cid = cid;
}

void wil_netif_rx_any(struct sk_buff *skb, struct net_device *ndev);
void wil_netif_rx(struct sk_buff *skb, struct net_device *ndev, int cid,
		  struct wil_net_stats *stats, bool gro);
void wil_rx_reorder(struct wil6210_priv *wil, struct sk_buff *skb);
void wil_rx_bar(struct wil6210_priv *wil, struct wil6210_vif *vif,
		u8 cid, u8 tid, u16 seq);
struct wil_tid_ampdu_rx *wil_tid_ampdu_rx_alloc(struct wil6210_priv *wil,
						int size, u16 ssn);
void wil_tid_ampdu_rx_free(struct wil6210_priv *wil,
			   struct wil_tid_ampdu_rx *r);
void wil_tx_data_init(struct wil_ring_tx_data *txdata);
void wil_init_txrx_ops_legacy_dma(struct wil6210_priv *wil);
void wil_tx_latency_calc(struct wil6210_priv *wil, struct sk_buff *skb,
			 struct wil_sta_info *sta);

#endif /* WIL6210_TXRX_H */
