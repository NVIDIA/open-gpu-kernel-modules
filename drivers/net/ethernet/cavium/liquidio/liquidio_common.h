/**********************************************************************
 * Author: Cavium, Inc.
 *
 * Contact: support@cavium.com
 *          Please include "LiquidIO" in the subject.
 *
 * Copyright (c) 2003-2016 Cavium, Inc.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, Version 2, as
 * published by the Free Software Foundation.
 *
 * This file is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT.  See the GNU General Public License for more details.
 ***********************************************************************/
/*!  \file  liquidio_common.h
 *   \brief Common: Structures and macros used in PCI-NIC package by core and
 *   host driver.
 */

#ifndef __LIQUIDIO_COMMON_H__
#define __LIQUIDIO_COMMON_H__

#include "octeon_config.h"

#define LIQUIDIO_BASE_MAJOR_VERSION 1
#define LIQUIDIO_BASE_MINOR_VERSION 7
#define LIQUIDIO_BASE_MICRO_VERSION 2
#define LIQUIDIO_BASE_VERSION   __stringify(LIQUIDIO_BASE_MAJOR_VERSION) "." \
				__stringify(LIQUIDIO_BASE_MINOR_VERSION)

struct lio_version {
	u16  major;
	u16  minor;
	u16  micro;
	u16  reserved;
};

#define CONTROL_IQ 0
/** Tag types used by Octeon cores in its work. */
enum octeon_tag_type {
	ORDERED_TAG = 0,
	ATOMIC_TAG = 1,
	NULL_TAG = 2,
	NULL_NULL_TAG = 3
};

/* pre-defined host->NIC tag values */
#define LIO_CONTROL  (0x11111110)
#define LIO_DATA(i)  (0x11111111 + (i))

/* Opcodes used by host driver/apps to perform operations on the core.
 * These are used to identify the major subsystem that the operation
 * is for.
 */
#define OPCODE_CORE 0           /* used for generic core operations */
#define OPCODE_NIC  1           /* used for NIC operations */
/* Subcodes are used by host driver/apps to identify the sub-operation
 * for the core. They only need to by unique for a given subsystem.
 */
#define OPCODE_SUBCODE(op, sub)       ((((op) & 0x0f) << 8) | ((sub) & 0x7f))

/** OPCODE_CORE subcodes. For future use. */

/** OPCODE_NIC subcodes */

/* This subcode is sent by core PCI driver to indicate cores are ready. */
#define OPCODE_NIC_CORE_DRV_ACTIVE     0x01
#define OPCODE_NIC_NW_DATA             0x02     /* network packet data */
#define OPCODE_NIC_CMD                 0x03
#define OPCODE_NIC_INFO                0x04
#define OPCODE_NIC_PORT_STATS          0x05
#define OPCODE_NIC_MDIO45              0x06
#define OPCODE_NIC_TIMESTAMP           0x07
#define OPCODE_NIC_INTRMOD_CFG         0x08
#define OPCODE_NIC_IF_CFG              0x09
#define OPCODE_NIC_VF_DRV_NOTICE       0x0A
#define OPCODE_NIC_INTRMOD_PARAMS      0x0B
#define OPCODE_NIC_QCOUNT_UPDATE       0x12
#define OPCODE_NIC_SET_TRUSTED_VF	0x13
#define OPCODE_NIC_SYNC_OCTEON_TIME	0x14
#define VF_DRV_LOADED                  1
#define VF_DRV_REMOVED                -1
#define VF_DRV_MACADDR_CHANGED         2

#define OPCODE_NIC_VF_REP_PKT          0x15
#define OPCODE_NIC_VF_REP_CMD          0x16
#define OPCODE_NIC_UBOOT_CTL           0x17

#define CORE_DRV_TEST_SCATTER_OP    0xFFF5

/* Application codes advertised by the core driver initialization packet. */
#define CVM_DRV_APP_START           0x0
#define CVM_DRV_NO_APP              0
#define CVM_DRV_APP_COUNT           0x2
#define CVM_DRV_BASE_APP            (CVM_DRV_APP_START + 0x0)
#define CVM_DRV_NIC_APP             (CVM_DRV_APP_START + 0x1)
#define CVM_DRV_INVALID_APP         (CVM_DRV_APP_START + 0x2)
#define CVM_DRV_APP_END             (CVM_DRV_INVALID_APP - 1)

#define BYTES_PER_DHLEN_UNIT        8
#define MAX_REG_CNT                 2000000U
#define INTRNAMSIZ                  32
#define IRQ_NAME_OFF(i)             ((i) * INTRNAMSIZ)
#define MAX_IOQ_INTERRUPTS_PER_PF   (64 * 2)
#define MAX_IOQ_INTERRUPTS_PER_VF   (8 * 2)

#define SCR2_BIT_FW_LOADED	    63

/* App specific capabilities from firmware to pf driver */
#define LIQUIDIO_TIME_SYNC_CAP 0x1
#define LIQUIDIO_SWITCHDEV_CAP 0x2
#define LIQUIDIO_SPOOFCHK_CAP  0x4

/* error status return from firmware */
#define OCTEON_REQUEST_NO_PERMISSION 0xc

static inline u32 incr_index(u32 index, u32 count, u32 max)
{
	if ((index + count) >= max)
		index = index + count - max;
	else
		index += count;

	return index;
}

#define OCT_BOARD_NAME 32
#define OCT_SERIAL_LEN 64

/* Structure used by core driver to send indication that the Octeon
 * application is ready.
 */
struct octeon_core_setup {
	u64 corefreq;

	char boardname[OCT_BOARD_NAME];

	char board_serial_number[OCT_SERIAL_LEN];

	u64 board_rev_major;

	u64 board_rev_minor;

};

/*---------------------------  SCATTER GATHER ENTRY  -----------------------*/

/* The Scatter-Gather List Entry. The scatter or gather component used with
 * a Octeon input instruction has this format.
 */
struct octeon_sg_entry {
	/** The first 64 bit gives the size of data in each dptr.*/
	union {
		u16 size[4];
		u64 size64;
	} u;

	/** The 4 dptr pointers for this entry. */
	u64 ptr[4];

};

#define OCT_SG_ENTRY_SIZE    (sizeof(struct octeon_sg_entry))

/* \brief Add size to gather list
 * @param sg_entry scatter/gather entry
 * @param size size to add
 * @param pos position to add it.
 */
static inline void add_sg_size(struct octeon_sg_entry *sg_entry,
			       u16 size,
			       u32 pos)
{
#ifdef __BIG_ENDIAN_BITFIELD
	sg_entry->u.size[pos] = size;
#else
	sg_entry->u.size[3 - pos] = size;
#endif
}

/*------------------------- End Scatter/Gather ---------------------------*/

#define   OCTNET_FRM_LENGTH_SIZE      8

#define   OCTNET_FRM_PTP_HEADER_SIZE  8

#define   OCTNET_FRM_HEADER_SIZE     22 /* VLAN + Ethernet */

#define   OCTNET_MIN_FRM_SIZE        64

#define   OCTNET_MAX_FRM_SIZE        (16000 + OCTNET_FRM_HEADER_SIZE)

#define   OCTNET_DEFAULT_MTU         (1500)
#define   OCTNET_DEFAULT_FRM_SIZE  (OCTNET_DEFAULT_MTU + OCTNET_FRM_HEADER_SIZE)

/** NIC Commands are sent using this Octeon Input Queue */
#define   OCTNET_CMD_Q                0

/* NIC Command types */
#define   OCTNET_CMD_CHANGE_MTU       0x1
#define   OCTNET_CMD_CHANGE_MACADDR   0x2
#define   OCTNET_CMD_CHANGE_DEVFLAGS  0x3
#define   OCTNET_CMD_RX_CTL           0x4

#define	  OCTNET_CMD_SET_MULTI_LIST   0x5
#define   OCTNET_CMD_CLEAR_STATS      0x6

/* command for setting the speed, duplex & autoneg */
#define   OCTNET_CMD_SET_SETTINGS     0x7
#define   OCTNET_CMD_SET_FLOW_CTL     0x8

#define   OCTNET_CMD_MDIO_READ_WRITE  0x9
#define   OCTNET_CMD_GPIO_ACCESS      0xA
#define   OCTNET_CMD_LRO_ENABLE       0xB
#define   OCTNET_CMD_LRO_DISABLE      0xC
#define   OCTNET_CMD_SET_RSS          0xD
#define   OCTNET_CMD_WRITE_SA         0xE
#define   OCTNET_CMD_DELETE_SA        0xF
#define   OCTNET_CMD_UPDATE_SA        0x12

#define   OCTNET_CMD_TNL_RX_CSUM_CTL 0x10
#define   OCTNET_CMD_TNL_TX_CSUM_CTL 0x11
#define   OCTNET_CMD_IPSECV2_AH_ESP_CTL 0x13
#define   OCTNET_CMD_VERBOSE_ENABLE   0x14
#define   OCTNET_CMD_VERBOSE_DISABLE  0x15

#define   OCTNET_CMD_VLAN_FILTER_CTL 0x16
#define   OCTNET_CMD_ADD_VLAN_FILTER  0x17
#define   OCTNET_CMD_DEL_VLAN_FILTER  0x18
#define   OCTNET_CMD_VXLAN_PORT_CONFIG 0x19

#define   OCTNET_CMD_ID_ACTIVE         0x1a

#define   OCTNET_CMD_SET_UC_LIST       0x1b
#define   OCTNET_CMD_SET_VF_LINKSTATE  0x1c

#define   OCTNET_CMD_QUEUE_COUNT_CTL	0x1f

#define   OCTNET_CMD_GROUP1             1
#define   OCTNET_CMD_SET_VF_SPOOFCHK    0x1
#define   OCTNET_GROUP1_LAST_CMD        OCTNET_CMD_SET_VF_SPOOFCHK

#define   OCTNET_CMD_VXLAN_PORT_ADD    0x0
#define   OCTNET_CMD_VXLAN_PORT_DEL    0x1
#define   OCTNET_CMD_RXCSUM_ENABLE     0x0
#define   OCTNET_CMD_RXCSUM_DISABLE    0x1
#define   OCTNET_CMD_TXCSUM_ENABLE     0x0
#define   OCTNET_CMD_TXCSUM_DISABLE    0x1
#define   OCTNET_CMD_VLAN_FILTER_ENABLE 0x1
#define   OCTNET_CMD_VLAN_FILTER_DISABLE 0x0

#define   OCTNET_CMD_FAIL 0x1

#define   SEAPI_CMD_FEC_SET             0x0
#define   SEAPI_CMD_FEC_SET_DISABLE       0x0
#define   SEAPI_CMD_FEC_SET_RS            0x1
#define   SEAPI_CMD_FEC_GET             0x1

#define   SEAPI_CMD_SPEED_SET           0x2
#define   SEAPI_CMD_SPEED_GET           0x3

#define OPCODE_NIC_VF_PORT_STATS        0x22

#define   LIO_CMD_WAIT_TM 100

/* RX(packets coming from wire) Checksum verification flags */
/* TCP/UDP csum */
#define   CNNIC_L4SUM_VERIFIED             0x1
#define   CNNIC_IPSUM_VERIFIED             0x2
#define   CNNIC_TUN_CSUM_VERIFIED          0x4
#define   CNNIC_CSUM_VERIFIED (CNNIC_IPSUM_VERIFIED | CNNIC_L4SUM_VERIFIED)

/*LROIPV4 and LROIPV6 Flags*/
#define   OCTNIC_LROIPV4    0x1
#define   OCTNIC_LROIPV6    0x2

/* Interface flags communicated between host driver and core app. */
enum octnet_ifflags {
	OCTNET_IFFLAG_PROMISC   = 0x01,
	OCTNET_IFFLAG_ALLMULTI  = 0x02,
	OCTNET_IFFLAG_MULTICAST = 0x04,
	OCTNET_IFFLAG_BROADCAST = 0x08,
	OCTNET_IFFLAG_UNICAST   = 0x10
};

/*   wqe
 *  ---------------  0
 * |  wqe  word0-3 |
 *  ---------------  32
 * |    PCI IH     |
 *  ---------------  40
 * |     RPTR      |
 *  ---------------  48
 * |    PCI IRH    |
 *  ---------------  56
 * |  OCT_NET_CMD  |
 *  ---------------  64
 * | Addtl 8-BData |
 * |               |
 *  ---------------
 */

union octnet_cmd {
	u64 u64;

	struct {
#ifdef __BIG_ENDIAN_BITFIELD
		u64 cmd:5;

		u64 more:6; /* How many udd words follow the command */

		u64 cmdgroup:8;
		u64 reserved:21;

		u64 param1:16;

		u64 param2:8;

#else

		u64 param2:8;

		u64 param1:16;

		u64 reserved:21;
		u64 cmdgroup:8;

		u64 more:6;

		u64 cmd:5;

#endif
	} s;

};

#define   OCTNET_CMD_SIZE     (sizeof(union octnet_cmd))

/*pkiih3 + irh + ossp[0] + ossp[1] + rdp + rptr = 40 bytes */
#define LIO_SOFTCMDRESP_IH2       40
#define LIO_SOFTCMDRESP_IH3       (40 + 8)

#define LIO_PCICMD_O2             24
#define LIO_PCICMD_O3             (24 + 8)

/* Instruction Header(DPI) - for OCTEON-III models */
struct  octeon_instr_ih3 {
#ifdef __BIG_ENDIAN_BITFIELD

	/** Reserved3 */
	u64     reserved3:1;

	/** Gather indicator 1=gather*/
	u64     gather:1;

	/** Data length OR no. of entries in gather list */
	u64     dlengsz:14;

	/** Front Data size */
	u64     fsz:6;

	/** Reserved2 */
	u64     reserved2:4;

	/** PKI port kind - PKIND */
	u64     pkind:6;

	/** Reserved1 */
	u64     reserved1:32;

#else
	/** Reserved1 */
	u64     reserved1:32;

	/** PKI port kind - PKIND */
	u64     pkind:6;

	/** Reserved2 */
	u64     reserved2:4;

	/** Front Data size */
	u64     fsz:6;

	/** Data length OR no. of entries in gather list */
	u64     dlengsz:14;

	/** Gather indicator 1=gather*/
	u64     gather:1;

	/** Reserved3 */
	u64     reserved3:1;

#endif
};

/* Optional PKI Instruction Header(PKI IH) - for OCTEON-III models */
/** BIG ENDIAN format.   */
struct  octeon_instr_pki_ih3 {
#ifdef __BIG_ENDIAN_BITFIELD

	/** Wider bit */
	u64     w:1;

	/** Raw mode indicator 1 = RAW */
	u64     raw:1;

	/** Use Tag */
	u64     utag:1;

	/** Use QPG */
	u64     uqpg:1;

	/** Reserved2 */
	u64     reserved2:1;

	/** Parse Mode */
	u64     pm:3;

	/** Skip Length */
	u64     sl:8;

	/** Use Tag Type */
	u64     utt:1;

	/** Tag type */
	u64     tagtype:2;

	/** Reserved1 */
	u64     reserved1:2;

	/** QPG Value */
	u64     qpg:11;

	/** Tag Value */
	u64     tag:32;

#else

	/** Tag Value */
	u64     tag:32;

	/** QPG Value */
	u64     qpg:11;

	/** Reserved1 */
	u64     reserved1:2;

	/** Tag type */
	u64     tagtype:2;

	/** Use Tag Type */
	u64     utt:1;

	/** Skip Length */
	u64     sl:8;

	/** Parse Mode */
	u64     pm:3;

	/** Reserved2 */
	u64     reserved2:1;

	/** Use QPG */
	u64     uqpg:1;

	/** Use Tag */
	u64     utag:1;

	/** Raw mode indicator 1 = RAW */
	u64     raw:1;

	/** Wider bit */
	u64     w:1;
#endif

};

/** Instruction Header */
struct octeon_instr_ih2 {
#ifdef __BIG_ENDIAN_BITFIELD
	/** Raw mode indicator 1 = RAW */
	u64 raw:1;

	/** Gather indicator 1=gather*/
	u64 gather:1;

	/** Data length OR no. of entries in gather list */
	u64 dlengsz:14;

	/** Front Data size */
	u64 fsz:6;

	/** Packet Order / Work Unit selection (1 of 8)*/
	u64 qos:3;

	/** Core group selection (1 of 16) */
	u64 grp:4;

	/** Short Raw Packet Indicator 1=short raw pkt */
	u64 rs:1;

	/** Tag type */
	u64 tagtype:2;

	/** Tag Value */
	u64 tag:32;
#else
	/** Tag Value */
	u64 tag:32;

	/** Tag type */
	u64 tagtype:2;

	/** Short Raw Packet Indicator 1=short raw pkt */
	u64 rs:1;

	/** Core group selection (1 of 16) */
	u64 grp:4;

	/** Packet Order / Work Unit selection (1 of 8)*/
	u64 qos:3;

	/** Front Data size */
	u64 fsz:6;

	/** Data length OR no. of entries in gather list */
	u64 dlengsz:14;

	/** Gather indicator 1=gather*/
	u64 gather:1;

	/** Raw mode indicator 1 = RAW */
	u64 raw:1;
#endif
};

/** Input Request Header */
struct octeon_instr_irh {
#ifdef __BIG_ENDIAN_BITFIELD
	u64 opcode:4;
	u64 rflag:1;
	u64 subcode:7;
	u64 vlan:12;
	u64 priority:3;
	u64 reserved:5;
	u64 ossp:32;             /* opcode/subcode specific parameters */
#else
	u64 ossp:32;             /* opcode/subcode specific parameters */
	u64 reserved:5;
	u64 priority:3;
	u64 vlan:12;
	u64 subcode:7;
	u64 rflag:1;
	u64 opcode:4;
#endif
};

/** Return Data Parameters */
struct octeon_instr_rdp {
#ifdef __BIG_ENDIAN_BITFIELD
	u64 reserved:49;
	u64 pcie_port:3;
	u64 rlen:12;
#else
	u64 rlen:12;
	u64 pcie_port:3;
	u64 reserved:49;
#endif
};

/** Receive Header */
union octeon_rh {
#ifdef __BIG_ENDIAN_BITFIELD
	u64 u64;
	struct {
		u64 opcode:4;
		u64 subcode:8;
		u64 len:3;     /** additional 64-bit words */
		u64 reserved:17;
		u64 ossp:32;   /** opcode/subcode specific parameters */
	} r;
	struct {
		u64 opcode:4;
		u64 subcode:8;
		u64 len:3;     /** additional 64-bit words */
		u64 extra:28;
		u64 vlan:12;
		u64 priority:3;
		u64 csum_verified:3;     /** checksum verified. */
		u64 has_hwtstamp:1;      /** Has hardware timestamp. 1 = yes. */
		u64 encap_on:1;
		u64 has_hash:1;          /** Has hash (rth or rss). 1 = yes. */
	} r_dh;
	struct {
		u64 opcode:4;
		u64 subcode:8;
		u64 len:3;     /** additional 64-bit words */
		u64 reserved:11;
		u64 num_gmx_ports:8;
		u64 max_nic_ports:10;
		u64 app_cap_flags:4;
		u64 app_mode:8;
		u64 pkind:8;
	} r_core_drv_init;
	struct {
		u64 opcode:4;
		u64 subcode:8;
		u64 len:3;       /** additional 64-bit words */
		u64 reserved:8;
		u64 extra:25;
		u64 gmxport:16;
	} r_nic_info;
#else
	u64 u64;
	struct {
		u64 ossp:32;  /** opcode/subcode specific parameters */
		u64 reserved:17;
		u64 len:3;    /** additional 64-bit words */
		u64 subcode:8;
		u64 opcode:4;
	} r;
	struct {
		u64 has_hash:1;          /** Has hash (rth or rss). 1 = yes. */
		u64 encap_on:1;
		u64 has_hwtstamp:1;      /** 1 = has hwtstamp */
		u64 csum_verified:3;     /** checksum verified. */
		u64 priority:3;
		u64 vlan:12;
		u64 extra:28;
		u64 len:3;    /** additional 64-bit words */
		u64 subcode:8;
		u64 opcode:4;
	} r_dh;
	struct {
		u64 pkind:8;
		u64 app_mode:8;
		u64 app_cap_flags:4;
		u64 max_nic_ports:10;
		u64 num_gmx_ports:8;
		u64 reserved:11;
		u64 len:3;       /** additional 64-bit words */
		u64 subcode:8;
		u64 opcode:4;
	} r_core_drv_init;
	struct {
		u64 gmxport:16;
		u64 extra:25;
		u64 reserved:8;
		u64 len:3;       /** additional 64-bit words */
		u64 subcode:8;
		u64 opcode:4;
	} r_nic_info;
#endif
};

#define  OCT_RH_SIZE   (sizeof(union  octeon_rh))

union octnic_packet_params {
	u32 u32;
	struct {
#ifdef __BIG_ENDIAN_BITFIELD
		u32 reserved:24;
		u32 ip_csum:1;		/* Perform IP header checksum(s) */
		/* Perform Outer transport header checksum */
		u32 transport_csum:1;
		/* Find tunnel, and perform transport csum. */
		u32 tnl_csum:1;
		u32 tsflag:1;		/* Timestamp this packet */
		u32 ipsec_ops:4;	/* IPsec operation */
#else
		u32 ipsec_ops:4;
		u32 tsflag:1;
		u32 tnl_csum:1;
		u32 transport_csum:1;
		u32 ip_csum:1;
		u32 reserved:24;
#endif
	} s;
};

/** Status of a RGMII Link on Octeon as seen by core driver. */
union oct_link_status {
	u64 u64;

	struct {
#ifdef __BIG_ENDIAN_BITFIELD
		u64 duplex:8;
		u64 mtu:16;
		u64 speed:16;
		u64 link_up:1;
		u64 autoneg:1;
		u64 if_mode:5;
		u64 pause:1;
		u64 flashing:1;
		u64 phy_type:5;
		u64 reserved:10;
#else
		u64 reserved:10;
		u64 phy_type:5;
		u64 flashing:1;
		u64 pause:1;
		u64 if_mode:5;
		u64 autoneg:1;
		u64 link_up:1;
		u64 speed:16;
		u64 mtu:16;
		u64 duplex:8;
#endif
	} s;
};

enum lio_phy_type {
	LIO_PHY_PORT_TP = 0x0,
	LIO_PHY_PORT_FIBRE = 0x1,
	LIO_PHY_PORT_UNKNOWN,
};

/** The txpciq info passed to host from the firmware */

union oct_txpciq {
	u64 u64;

	struct {
#ifdef __BIG_ENDIAN_BITFIELD
		u64 q_no:8;
		u64 port:8;
		u64 pkind:6;
		u64 use_qpg:1;
		u64 qpg:11;
		u64 reserved0:10;
		u64 ctrl_qpg:11;
		u64 reserved:9;
#else
		u64 reserved:9;
		u64 ctrl_qpg:11;
		u64 reserved0:10;
		u64 qpg:11;
		u64 use_qpg:1;
		u64 pkind:6;
		u64 port:8;
		u64 q_no:8;
#endif
	} s;
};

/** The rxpciq info passed to host from the firmware */

union oct_rxpciq {
	u64 u64;

	struct {
#ifdef __BIG_ENDIAN_BITFIELD
		u64 q_no:8;
		u64 reserved:56;
#else
		u64 reserved:56;
		u64 q_no:8;
#endif
	} s;
};

/** Information for a OCTEON ethernet interface shared between core & host. */
struct oct_link_info {
	union oct_link_status link;
	u64 hw_addr;

#ifdef __BIG_ENDIAN_BITFIELD
	u64 gmxport:16;
	u64 macaddr_is_admin_asgnd:1;
	u64 rsvd:13;
	u64 macaddr_spoofchk:1;
	u64 rsvd1:17;
	u64 num_txpciq:8;
	u64 num_rxpciq:8;
#else
	u64 num_rxpciq:8;
	u64 num_txpciq:8;
	u64 rsvd1:17;
	u64 macaddr_spoofchk:1;
	u64 rsvd:13;
	u64 macaddr_is_admin_asgnd:1;
	u64 gmxport:16;
#endif

	union oct_txpciq txpciq[MAX_IOQS_PER_NICIF];
	union oct_rxpciq rxpciq[MAX_IOQS_PER_NICIF];
};

#define OCT_LINK_INFO_SIZE   (sizeof(struct oct_link_info))

struct liquidio_if_cfg_info {
	u64 iqmask; /** mask for IQs enabled for  the port */
	u64 oqmask; /** mask for OQs enabled for the port */
	struct oct_link_info linfo; /** initial link information */
	char   liquidio_firmware_version[32];
};

/** Stats for each NIC port in RX direction. */
struct nic_rx_stats {
	/* link-level stats */
	u64 total_rcvd;		/* Received packets */
	u64 bytes_rcvd;		/* Octets of received packets */
	u64 total_bcst;		/* Number of non-dropped L2 broadcast packets */
	u64 total_mcst;		/* Number of non-dropped L2 multicast packets */
	u64 runts;		/* Packets shorter than allowed */
	u64 ctl_rcvd;		/* Received PAUSE packets */
	u64 fifo_err;		/* Packets dropped due to RX FIFO full */
	u64 dmac_drop;		/* Packets dropped by the DMAC filter */
	u64 fcs_err;		/* Sum of fragment, overrun, and FCS errors */
	u64 jabber_err;		/* Packets larger than allowed */
	u64 l2_err;		/* Sum of DMA, parity, PCAM access, no memory,
				 * buffer overflow, malformed L2 header or
				 * length, oversize errors
				 **/
	u64 frame_err;		/* Sum of IPv4 and L4 checksum errors */
	u64 red_drops;		/* Packets dropped by RED due to buffer
				 * exhaustion
				 **/

	/* firmware stats */
	u64 fw_total_rcvd;
	u64 fw_total_fwd;
	u64 fw_total_fwd_bytes;
	u64 fw_total_mcast;
	u64 fw_total_bcast;

	u64 fw_err_pko;
	u64 fw_err_link;
	u64 fw_err_drop;
	u64 fw_rx_vxlan;
	u64 fw_rx_vxlan_err;

	/* LRO */
	u64 fw_lro_pkts;   /* Number of packets that are LROed      */
	u64 fw_lro_octs;   /* Number of octets that are LROed       */
	u64 fw_total_lro;  /* Number of LRO packets formed          */
	u64 fw_lro_aborts; /* Number of times LRO of packet aborted */
	u64 fw_lro_aborts_port;
	u64 fw_lro_aborts_seq;
	u64 fw_lro_aborts_tsval;
	u64 fw_lro_aborts_timer;	/* Timer setting error */
	/* intrmod: packet forward rate */
	u64 fwd_rate;
};

/** Stats for each NIC port in RX direction. */
struct nic_tx_stats {
	/* link-level stats */
	u64 total_pkts_sent;		/* Total frames sent on the interface */
	u64 total_bytes_sent;		/* Total octets sent on the interface */
	u64 mcast_pkts_sent;		/* Packets sent to the multicast DMAC */
	u64 bcast_pkts_sent;		/* Packets sent to a broadcast DMAC */
	u64 ctl_sent;			/* Control/PAUSE packets sent */
	u64 one_collision_sent;		/* Packets sent that experienced a
					 * single collision before successful
					 * transmission
					 **/
	u64 multi_collision_sent;	/* Packets sent that experienced
					 * multiple collisions before successful
					 * transmission
					 **/
	u64 max_collision_fail;		/* Packets dropped due to excessive
					 * collisions
					 **/
	u64 max_deferral_fail;		/* Packets not sent due to max
					 * deferrals
					 **/
	u64 fifo_err;			/* Packets sent that experienced a
					 * transmit underflow and were
					 * truncated
					 **/
	u64 runts;			/* Packets sent with an octet count
					 * lessthan 64
					 **/
	u64 total_collisions;		/* Packets dropped due to excessive
					 * collisions
					 **/

	/* firmware stats */
	u64 fw_total_sent;
	u64 fw_total_fwd;
	u64 fw_total_fwd_bytes;
	u64 fw_total_mcast_sent;
	u64 fw_total_bcast_sent;
	u64 fw_err_pko;
	u64 fw_err_link;
	u64 fw_err_drop;
	u64 fw_err_tso;
	u64 fw_tso;		/* number of tso requests */
	u64 fw_tso_fwd;		/* number of packets segmented in tso */
	u64 fw_tx_vxlan;
	u64 fw_err_pki;
};

struct oct_link_stats {
	struct nic_rx_stats fromwire;
	struct nic_tx_stats fromhost;

};

static inline int opcode_slow_path(union octeon_rh *rh)
{
	u16 subcode1, subcode2;

	subcode1 = OPCODE_SUBCODE((rh)->r.opcode, (rh)->r.subcode);
	subcode2 = OPCODE_SUBCODE(OPCODE_NIC, OPCODE_NIC_NW_DATA);

	return (subcode2 != subcode1);
}

#define LIO68XX_LED_CTRL_ADDR     0x3501
#define LIO68XX_LED_CTRL_CFGON    0x1f
#define LIO68XX_LED_CTRL_CFGOFF   0x100
#define LIO68XX_LED_BEACON_ADDR   0x3508
#define LIO68XX_LED_BEACON_CFGON  0x47fd
#define LIO68XX_LED_BEACON_CFGOFF 0x11fc
#define VITESSE_PHY_GPIO_DRIVEON  0x1
#define VITESSE_PHY_GPIO_CFG      0x8
#define VITESSE_PHY_GPIO_DRIVEOFF 0x4
#define VITESSE_PHY_GPIO_HIGH     0x2
#define VITESSE_PHY_GPIO_LOW      0x3
#define LED_IDENTIFICATION_ON     0x1
#define LED_IDENTIFICATION_OFF    0x0
#define LIO23XX_COPPERHEAD_LED_GPIO 0x2

struct oct_mdio_cmd {
	u64 op;
	u64 mdio_addr;
	u64 value1;
	u64 value2;
	u64 value3;
};

#define OCT_LINK_STATS_SIZE   (sizeof(struct oct_link_stats))

struct oct_intrmod_cfg {
	u64 rx_enable;
	u64 tx_enable;
	u64 check_intrvl;
	u64 maxpkt_ratethr;
	u64 minpkt_ratethr;
	u64 rx_maxcnt_trigger;
	u64 rx_mincnt_trigger;
	u64 rx_maxtmr_trigger;
	u64 rx_mintmr_trigger;
	u64 tx_mincnt_trigger;
	u64 tx_maxcnt_trigger;
	u64 rx_frames;
	u64 tx_frames;
	u64 rx_usecs;
};

#define BASE_QUEUE_NOT_REQUESTED 65535

union oct_nic_if_cfg {
	u64 u64;
	struct {
#ifdef __BIG_ENDIAN_BITFIELD
		u64 base_queue:16;
		u64 num_iqueues:16;
		u64 num_oqueues:16;
		u64 gmx_port_id:8;
		u64 vf_id:8;
#else
		u64 vf_id:8;
		u64 gmx_port_id:8;
		u64 num_oqueues:16;
		u64 num_iqueues:16;
		u64 base_queue:16;
#endif
	} s;
};

struct lio_trusted_vf {
	uint64_t active: 1;
	uint64_t id : 8;
	uint64_t reserved: 55;
};

struct lio_time {
	s64 sec;   /* seconds */
	s64 nsec;  /* nanoseconds */
};

struct lio_vf_rep_stats {
	u64 tx_packets;
	u64 tx_bytes;
	u64 tx_dropped;

	u64 rx_packets;
	u64 rx_bytes;
	u64 rx_dropped;
};

enum lio_vf_rep_req_type {
	LIO_VF_REP_REQ_NONE,
	LIO_VF_REP_REQ_STATE,
	LIO_VF_REP_REQ_MTU,
	LIO_VF_REP_REQ_STATS,
	LIO_VF_REP_REQ_DEVNAME
};

enum {
	LIO_VF_REP_STATE_DOWN,
	LIO_VF_REP_STATE_UP
};

#define LIO_IF_NAME_SIZE 16
struct lio_vf_rep_req {
	u8 req_type;
	u8 ifidx;
	u8 rsvd[6];

	union {
		struct lio_vf_rep_name {
			char name[LIO_IF_NAME_SIZE];
		} rep_name;

		struct lio_vf_rep_mtu {
			u32 mtu;
			u32 rsvd;
		} rep_mtu;

		struct lio_vf_rep_state {
			u8 state;
			u8 rsvd[7];
		} rep_state;
	};
};

struct lio_vf_rep_resp {
	u64 rh;
	u8  status;
	u8  rsvd[7];
};
#endif
