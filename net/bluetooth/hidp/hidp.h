/*
   HIDP implementation for Linux Bluetooth stack (BlueZ).
   Copyright (C) 2003-2004 Marcel Holtmann <marcel@holtmann.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation;

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
   IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) AND AUTHOR(S) BE LIABLE FOR ANY
   CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ALL LIABILITY, INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PATENTS,
   COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS, RELATING TO USE OF THIS
   SOFTWARE IS DISCLAIMED.
*/

#ifndef __HIDP_H
#define __HIDP_H

#include <linux/types.h>
#include <linux/hid.h>
#include <linux/kref.h>
#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/l2cap.h>

/* HIDP header masks */
#define HIDP_HEADER_TRANS_MASK			0xf0
#define HIDP_HEADER_PARAM_MASK			0x0f

/* HIDP transaction types */
#define HIDP_TRANS_HANDSHAKE			0x00
#define HIDP_TRANS_HID_CONTROL			0x10
#define HIDP_TRANS_GET_REPORT			0x40
#define HIDP_TRANS_SET_REPORT			0x50
#define HIDP_TRANS_GET_PROTOCOL			0x60
#define HIDP_TRANS_SET_PROTOCOL			0x70
#define HIDP_TRANS_GET_IDLE			0x80
#define HIDP_TRANS_SET_IDLE			0x90
#define HIDP_TRANS_DATA				0xa0
#define HIDP_TRANS_DATC				0xb0

/* HIDP handshake results */
#define HIDP_HSHK_SUCCESSFUL			0x00
#define HIDP_HSHK_NOT_READY			0x01
#define HIDP_HSHK_ERR_INVALID_REPORT_ID		0x02
#define HIDP_HSHK_ERR_UNSUPPORTED_REQUEST	0x03
#define HIDP_HSHK_ERR_INVALID_PARAMETER		0x04
#define HIDP_HSHK_ERR_UNKNOWN			0x0e
#define HIDP_HSHK_ERR_FATAL			0x0f

/* HIDP control operation parameters */
#define HIDP_CTRL_NOP				0x00
#define HIDP_CTRL_HARD_RESET			0x01
#define HIDP_CTRL_SOFT_RESET			0x02
#define HIDP_CTRL_SUSPEND			0x03
#define HIDP_CTRL_EXIT_SUSPEND			0x04
#define HIDP_CTRL_VIRTUAL_CABLE_UNPLUG		0x05

/* HIDP data transaction headers */
#define HIDP_DATA_RTYPE_MASK			0x03
#define HIDP_DATA_RSRVD_MASK			0x0c
#define HIDP_DATA_RTYPE_OTHER			0x00
#define HIDP_DATA_RTYPE_INPUT			0x01
#define HIDP_DATA_RTYPE_OUPUT			0x02
#define HIDP_DATA_RTYPE_FEATURE			0x03

/* HIDP protocol header parameters */
#define HIDP_PROTO_BOOT				0x00
#define HIDP_PROTO_REPORT			0x01

/* HIDP ioctl defines */
#define HIDPCONNADD	_IOW('H', 200, int)
#define HIDPCONNDEL	_IOW('H', 201, int)
#define HIDPGETCONNLIST	_IOR('H', 210, int)
#define HIDPGETCONNINFO	_IOR('H', 211, int)

#define HIDP_VIRTUAL_CABLE_UNPLUG	0
#define HIDP_BOOT_PROTOCOL_MODE		1
#define HIDP_BLUETOOTH_VENDOR_ID	9
#define HIDP_WAITING_FOR_RETURN		10
#define HIDP_WAITING_FOR_SEND_ACK	11

struct hidp_connadd_req {
	int   ctrl_sock;	/* Connected control socket */
	int   intr_sock;	/* Connected interrupt socket */
	__u16 parser;
	__u16 rd_size;
	__u8 __user *rd_data;
	__u8  country;
	__u8  subclass;
	__u16 vendor;
	__u16 product;
	__u16 version;
	__u32 flags;
	__u32 idle_to;
	char  name[128];
};

struct hidp_conndel_req {
	bdaddr_t bdaddr;
	__u32    flags;
};

struct hidp_conninfo {
	bdaddr_t bdaddr;
	__u32    flags;
	__u16    state;
	__u16    vendor;
	__u16    product;
	__u16    version;
	char     name[128];
};

struct hidp_connlist_req {
	__u32  cnum;
	struct hidp_conninfo __user *ci;
};

int hidp_connection_add(const struct hidp_connadd_req *req, struct socket *ctrl_sock, struct socket *intr_sock);
int hidp_connection_del(struct hidp_conndel_req *req);
int hidp_get_connlist(struct hidp_connlist_req *req);
int hidp_get_conninfo(struct hidp_conninfo *ci);

enum hidp_session_state {
	HIDP_SESSION_IDLING,
	HIDP_SESSION_PREPARING,
	HIDP_SESSION_RUNNING,
};

/* HIDP session defines */
struct hidp_session {
	struct list_head list;
	struct kref ref;

	/* runtime management */
	atomic_t state;
	wait_queue_head_t state_queue;
	atomic_t terminate;
	struct task_struct *task;
	unsigned long flags;

	/* connection management */
	bdaddr_t bdaddr;
	struct l2cap_conn *conn;
	struct l2cap_user user;
	struct socket *ctrl_sock;
	struct socket *intr_sock;
	struct sk_buff_head ctrl_transmit;
	struct sk_buff_head intr_transmit;
	uint ctrl_mtu;
	uint intr_mtu;
	unsigned long idle_to;

	/* device management */
	struct work_struct dev_init;
	struct input_dev *input;
	struct hid_device *hid;
	struct timer_list timer;

	/* Report descriptor */
	__u8 *rd_data;
	uint rd_size;

	/* session data */
	unsigned char keys[8];
	unsigned char leds;

	/* Used in hidp_get_raw_report() */
	int waiting_report_type; /* HIDP_DATA_RTYPE_* */
	int waiting_report_number; /* -1 for not numbered */
	struct mutex report_mutex;
	struct sk_buff *report_return;
	wait_queue_head_t report_queue;

	/* Used in hidp_output_raw_report() */
	int output_report_success; /* boolean */

	/* temporary input buffer */
	u8 input_buf[HID_MAX_BUFFER_SIZE];
};

/* HIDP init defines */
int __init hidp_init_sockets(void);
void __exit hidp_cleanup_sockets(void);

#endif /* __HIDP_H */
