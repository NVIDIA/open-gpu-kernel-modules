/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright 2008 Cisco Systems, Inc.  All rights reserved.
 */
#ifndef _FC_FIP_H_
#define _FC_FIP_H_

#include <scsi/fc/fc_ns.h>

/*
 * This version is based on:
 * http://www.t11.org/ftp/t11/pub/fc/bb-5/08-543v1.pdf
 * and T11 FC-BB-6 13-091v5.pdf (December 2013 VN2VN proposal)
 */

#define FIP_DEF_PRI	128	/* default selection priority */
#define FIP_DEF_FC_MAP	0x0efc00 /* default FCoE MAP (MAC OUI) value */
#define FIP_DEF_FKA	8000	/* default FCF keep-alive/advert period (mS) */
#define FIP_VN_KA_PERIOD 90000	/* required VN_port keep-alive period (mS) */
#define FIP_FCF_FUZZ	100	/* random time added by FCF (mS) */

/*
 * VN2VN proposed-standard values.
 */
#define FIP_VN_FC_MAP	0x0efd00 /* MAC OUI for VN2VN use */
#define FIP_VN_PROBE_WAIT 100	/* interval between VN2VN probes (ms) */
#define FIP_VN_ANN_WAIT 400	/* interval between VN2VN announcements (ms) */
#define FIP_VN_RLIM_INT 10000	/* interval between probes when rate limited */
#define FIP_VN_RLIM_COUNT 10	/* number of probes before rate limiting */
#define FIP_VN_BEACON_INT 8000	/* interval between VN2VN beacons */
#define FIP_VN_BEACON_FUZZ 100	/* random time to add to beacon period (ms) */

/*
 * Multicast MAC addresses.  T11-adopted.
 */
#define FIP_ALL_FCOE_MACS	((__u8[6]) { 1, 0x10, 0x18, 1, 0, 0 })
#define FIP_ALL_ENODE_MACS	((__u8[6]) { 1, 0x10, 0x18, 1, 0, 1 })
#define FIP_ALL_FCF_MACS	((__u8[6]) { 1, 0x10, 0x18, 1, 0, 2 })
#define FIP_ALL_VN2VN_MACS	((__u8[6]) { 1, 0x10, 0x18, 1, 0, 4 })
#define FIP_ALL_P2P_MACS	((__u8[6]) { 1, 0x10, 0x18, 1, 0, 5 })

#define FIP_VER		1		/* version for fip_header */

struct fip_header {
	__u8	fip_ver;		/* upper 4 bits are the version */
	__u8	fip_resv1;		/* reserved */
	__be16	fip_op;			/* operation code */
	__u8	fip_resv2;		/* reserved */
	__u8	fip_subcode;		/* lower 4 bits are sub-code */
	__be16	fip_dl_len;		/* length of descriptors in words */
	__be16	fip_flags;		/* header flags */
} __attribute__((packed));

#define FIP_VER_SHIFT	4
#define FIP_VER_ENCAPS(v) ((v) << FIP_VER_SHIFT)
#define FIP_VER_DECAPS(v) ((v) >> FIP_VER_SHIFT)
#define FIP_BPW		4		/* bytes per word for lengths */

/*
 * fip_op.
 */
enum fip_opcode {
	FIP_OP_DISC =	1,		/* discovery, advertisement, etc. */
	FIP_OP_LS =	2,		/* Link Service request or reply */
	FIP_OP_CTRL =	3,		/* Keep Alive / Link Reset */
	FIP_OP_VLAN =	4,		/* VLAN discovery */
	FIP_OP_VN2VN =	5,		/* VN2VN operation */
	FIP_OP_VENDOR_MIN = 0xfff8,	/* min vendor-specific opcode */
	FIP_OP_VENDOR_MAX = 0xfffe,	/* max vendor-specific opcode */
};

/*
 * Subcodes for FIP_OP_DISC.
 */
enum fip_disc_subcode {
	FIP_SC_SOL =	1,		/* solicitation */
	FIP_SC_ADV =	2,		/* advertisement */
};

/*
 * Subcodes for FIP_OP_LS.
 */
enum fip_trans_subcode {
	FIP_SC_REQ =	1,		/* request */
	FIP_SC_REP =	2,		/* reply */
};

/*
 * Subcodes for FIP_OP_RESET.
 */
enum fip_reset_subcode {
	FIP_SC_KEEP_ALIVE = 1,		/* keep-alive from VN_Port */
	FIP_SC_CLR_VLINK = 2,		/* clear virtual link from VF_Port */
};

/*
 * Subcodes for FIP_OP_VLAN.
 */
enum fip_vlan_subcode {
	FIP_SC_VL_REQ =	1,		/* vlan request */
	FIP_SC_VL_NOTE = 2,		/* vlan notification */
	FIP_SC_VL_VN2VN_NOTE = 3,	/* VN2VN vlan notification */
};

/*
 * Subcodes for FIP_OP_VN2VN.
 */
enum fip_vn2vn_subcode {
	FIP_SC_VN_PROBE_REQ = 1,	/* probe request */
	FIP_SC_VN_PROBE_REP = 2,	/* probe reply */
	FIP_SC_VN_CLAIM_NOTIFY = 3,	/* claim notification */
	FIP_SC_VN_CLAIM_REP = 4,	/* claim response */
	FIP_SC_VN_BEACON = 5,		/* beacon */
};

/*
 * flags in header fip_flags.
 */
enum fip_flag {
	FIP_FL_FPMA =	0x8000,		/* supports FPMA fabric-provided MACs */
	FIP_FL_SPMA =	0x4000,		/* supports SPMA server-provided MACs */
	FIP_FL_FCF =	0x0020,		/* originated from a controlling FCF */
	FIP_FL_FDF =	0x0010,		/* originated from an FDF */
	FIP_FL_REC_OR_P2P = 0x0008,	/* configured addr or point-to-point */
	FIP_FL_AVAIL =	0x0004,		/* available for FLOGI/ELP */
	FIP_FL_SOL =	0x0002,		/* this is a solicited message */
	FIP_FL_FPORT =	0x0001,		/* sent from an F port */
};

/*
 * Common descriptor header format.
 */
struct fip_desc {
	__u8	fip_dtype;		/* type - see below */
	__u8	fip_dlen;		/* length - in 32-bit words */
};

enum fip_desc_type {
	FIP_DT_PRI =	1,		/* priority for forwarder selection */
	FIP_DT_MAC =	2,		/* MAC address */
	FIP_DT_MAP_OUI = 3,		/* FC-MAP OUI */
	FIP_DT_NAME =	4,		/* switch name or node name */
	FIP_DT_FAB =	5,		/* fabric descriptor */
	FIP_DT_FCOE_SIZE = 6,		/* max FCoE frame size */
	FIP_DT_FLOGI =	7,		/* FLOGI request or response */
	FIP_DT_FDISC =	8,		/* FDISC request or response */
	FIP_DT_LOGO =	9,		/* LOGO request or response */
	FIP_DT_ELP =	10,		/* ELP request or response */
	FIP_DT_VN_ID =	11,		/* VN_Node Identifier */
	FIP_DT_FKA =	12,		/* advertisement keep-alive period */
	FIP_DT_VENDOR =	13,		/* vendor ID */
	FIP_DT_VLAN =	14,		/* vlan number */
	FIP_DT_FC4F =	15,		/* FC-4 features */
	FIP_DT_LIMIT,			/* max defined desc_type + 1 */
	FIP_DT_NON_CRITICAL = 128,	/* First non-critical descriptor */
	FIP_DT_CLR_VLINKS = 128,	/* Clear virtual links reason code */
	FIP_DT_VENDOR_BASE = 241,	/* first vendor-specific desc_type */
};

/*
 * FIP_DT_PRI - priority descriptor.
 */
struct fip_pri_desc {
	struct fip_desc fd_desc;
	__u8		fd_resvd;
	__u8		fd_pri;		/* FCF priority:  higher is better */
} __attribute__((packed));

/*
 * FIP_DT_MAC - MAC address descriptor.
 */
struct fip_mac_desc {
	struct fip_desc fd_desc;
	__u8		fd_mac[ETH_ALEN];
} __attribute__((packed));

/*
 * FIP_DT_MAP - descriptor.
 */
struct fip_map_desc {
	struct fip_desc fd_desc;
	__u8		fd_resvd[3];
	__u8		fd_map[3];
} __attribute__((packed));

/*
 * FIP_DT_NAME descriptor.
 */
struct fip_wwn_desc {
	struct fip_desc fd_desc;
	__u8		fd_resvd[2];
	__be64		fd_wwn;		/* 64-bit WWN, unaligned */
} __attribute__((packed));

/*
 * FIP_DT_FAB descriptor.
 */
struct fip_fab_desc {
	struct fip_desc fd_desc;
	__be16		fd_vfid;	/* virtual fabric ID */
	__u8		fd_resvd;
	__u8		fd_map[3];	/* FC-MAP value */
	__be64		fd_wwn;		/* fabric name, unaligned */
} __attribute__((packed));

/*
 * FIP_DT_FCOE_SIZE descriptor.
 */
struct fip_size_desc {
	struct fip_desc fd_desc;
	__be16		fd_size;
} __attribute__((packed));

/*
 * Descriptor that encapsulates an ELS or ILS frame.
 * The encapsulated frame immediately follows this header, without
 * SOF, EOF, or CRC.
 */
struct fip_encaps {
	struct fip_desc fd_desc;
	__u8		fd_resvd[2];
} __attribute__((packed));

/*
 * FIP_DT_VN_ID - VN_Node Identifier descriptor.
 */
struct fip_vn_desc {
	struct fip_desc fd_desc;
	__u8		fd_mac[ETH_ALEN];
	__u8		fd_resvd;
	__u8		fd_fc_id[3];
	__be64		fd_wwpn;	/* port name, unaligned */
} __attribute__((packed));

/*
 * FIP_DT_FKA - Advertisement keep-alive period.
 */
struct fip_fka_desc {
	struct fip_desc fd_desc;
	__u8		fd_resvd;
	__u8		fd_flags;	/* bit0 is fka disable flag */
	__be32		fd_fka_period;	/* adv./keep-alive period in mS */
} __attribute__((packed));

/*
 * flags for fip_fka_desc.fd_flags
 */
enum fip_fka_flags {
	FIP_FKA_ADV_D =	0x01,		/* no need for FKA from ENode */
};

/* FIP_DT_FKA flags */

/*
 * FIP_DT_VLAN descriptor
 */
struct fip_vlan_desc {
	struct fip_desc fd_desc;
	__be16		fd_vlan; /* Note: highest 4 bytes are unused */
} __attribute__((packed));

/*
 * FIP_DT_FC4F - FC-4 features.
 */
struct fip_fc4_feat {
	struct fip_desc fd_desc;
	__u8		fd_resvd[2];
	struct fc_ns_fts fd_fts;
	struct fc_ns_ff	fd_ff;
} __attribute__((packed));

/*
 * FIP_DT_VENDOR descriptor.
 */
struct fip_vendor_desc {
	struct fip_desc fd_desc;
	__u8		fd_resvd[2];
	__u8		fd_vendor_id[8];
} __attribute__((packed));

#endif /* _FC_FIP_H_ */
