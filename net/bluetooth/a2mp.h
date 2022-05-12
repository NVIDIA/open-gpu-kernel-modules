/* SPDX-License-Identifier: GPL-2.0-only */
/*
   Copyright (c) 2010,2011 Code Aurora Forum.  All rights reserved.
   Copyright (c) 2011,2012 Intel Corp.

*/

#ifndef __A2MP_H
#define __A2MP_H

#include <net/bluetooth/l2cap.h>

enum amp_mgr_state {
	READ_LOC_AMP_INFO,
	READ_LOC_AMP_ASSOC,
	READ_LOC_AMP_ASSOC_FINAL,
	WRITE_REMOTE_AMP_ASSOC,
};

struct amp_mgr {
	struct list_head	list;
	struct l2cap_conn	*l2cap_conn;
	struct l2cap_chan	*a2mp_chan;
	struct l2cap_chan	*bredr_chan;
	struct kref		kref;
	__u8			ident;
	__u8			handle;
	unsigned long		state;
	unsigned long		flags;

	struct list_head	amp_ctrls;
	struct mutex		amp_ctrls_lock;
};

struct a2mp_cmd {
	__u8	code;
	__u8	ident;
	__le16	len;
	__u8	data[];
} __packed;

/* A2MP command codes */
#define A2MP_COMMAND_REJ         0x01
struct a2mp_cmd_rej {
	__le16	reason;
	__u8	data[];
} __packed;

#define A2MP_DISCOVER_REQ        0x02
struct a2mp_discov_req {
	__le16	mtu;
	__le16	ext_feat;
} __packed;

struct a2mp_cl {
	__u8	id;
	__u8	type;
	__u8	status;
} __packed;

#define A2MP_DISCOVER_RSP        0x03
struct a2mp_discov_rsp {
	__le16     mtu;
	__le16     ext_feat;
	struct a2mp_cl cl[];
} __packed;

#define A2MP_CHANGE_NOTIFY       0x04
#define A2MP_CHANGE_RSP          0x05

#define A2MP_GETINFO_REQ         0x06
struct a2mp_info_req {
	__u8       id;
} __packed;

#define A2MP_GETINFO_RSP         0x07
struct a2mp_info_rsp {
	__u8	id;
	__u8	status;
	__le32	total_bw;
	__le32	max_bw;
	__le32	min_latency;
	__le16	pal_cap;
	__le16	assoc_size;
} __packed;

#define A2MP_GETAMPASSOC_REQ     0x08
struct a2mp_amp_assoc_req {
	__u8	id;
} __packed;

#define A2MP_GETAMPASSOC_RSP     0x09
struct a2mp_amp_assoc_rsp {
	__u8	id;
	__u8	status;
	__u8	amp_assoc[];
} __packed;

#define A2MP_CREATEPHYSLINK_REQ  0x0A
#define A2MP_DISCONNPHYSLINK_REQ 0x0C
struct a2mp_physlink_req {
	__u8	local_id;
	__u8	remote_id;
	__u8	amp_assoc[];
} __packed;

#define A2MP_CREATEPHYSLINK_RSP  0x0B
#define A2MP_DISCONNPHYSLINK_RSP 0x0D
struct a2mp_physlink_rsp {
	__u8	local_id;
	__u8	remote_id;
	__u8	status;
} __packed;

/* A2MP response status */
#define A2MP_STATUS_SUCCESS			0x00
#define A2MP_STATUS_INVALID_CTRL_ID		0x01
#define A2MP_STATUS_UNABLE_START_LINK_CREATION	0x02
#define A2MP_STATUS_NO_PHYSICAL_LINK_EXISTS	0x02
#define A2MP_STATUS_COLLISION_OCCURED		0x03
#define A2MP_STATUS_DISCONN_REQ_RECVD		0x04
#define A2MP_STATUS_PHYS_LINK_EXISTS		0x05
#define A2MP_STATUS_SECURITY_VIOLATION		0x06

struct amp_mgr *amp_mgr_get(struct amp_mgr *mgr);

#if IS_ENABLED(CONFIG_BT_HS)
int amp_mgr_put(struct amp_mgr *mgr);
struct l2cap_chan *a2mp_channel_create(struct l2cap_conn *conn,
				       struct sk_buff *skb);
void a2mp_discover_amp(struct l2cap_chan *chan);
#else
static inline int amp_mgr_put(struct amp_mgr *mgr)
{
	return 0;
}

static inline struct l2cap_chan *a2mp_channel_create(struct l2cap_conn *conn,
						     struct sk_buff *skb)
{
	return NULL;
}

static inline void a2mp_discover_amp(struct l2cap_chan *chan)
{
}
#endif

void a2mp_send_getinfo_rsp(struct hci_dev *hdev);
void a2mp_send_getampassoc_rsp(struct hci_dev *hdev, u8 status);
void a2mp_send_create_phy_link_req(struct hci_dev *hdev, u8 status);
void a2mp_send_create_phy_link_rsp(struct hci_dev *hdev, u8 status);

#endif /* __A2MP_H */
