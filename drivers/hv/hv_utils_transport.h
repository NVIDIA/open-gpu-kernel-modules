/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Kernel/userspace transport abstraction for Hyper-V util driver.
 *
 * Copyright (C) 2015, Vitaly Kuznetsov <vkuznets@redhat.com>
 */

#ifndef _HV_UTILS_TRANSPORT_H
#define _HV_UTILS_TRANSPORT_H

#include <linux/connector.h>
#include <linux/miscdevice.h>

enum hvutil_transport_mode {
	HVUTIL_TRANSPORT_INIT = 0,
	HVUTIL_TRANSPORT_NETLINK,
	HVUTIL_TRANSPORT_CHARDEV,
	HVUTIL_TRANSPORT_DESTROY,
};

struct hvutil_transport {
	int mode;                           /* hvutil_transport_mode */
	struct file_operations fops;        /* file operations */
	struct miscdevice mdev;             /* misc device */
	struct cb_id cn_id;                 /* CN_*_IDX/CN_*_VAL */
	struct list_head list;              /* hvt_list */
	int (*on_msg)(void *, int);         /* callback on new user message */
	void (*on_reset)(void);             /* callback when userspace drops */
	void (*on_read)(void);              /* callback on message read */
	u8 *outmsg;                         /* message to the userspace */
	int outmsg_len;                     /* its length */
	wait_queue_head_t outmsg_q;         /* poll/read wait queue */
	struct mutex lock;                  /* protects struct members */
	struct completion release;          /* synchronize with fd release */
};

struct hvutil_transport *hvutil_transport_init(const char *name,
					       u32 cn_idx, u32 cn_val,
					       int (*on_msg)(void *, int),
					       void (*on_reset)(void));
int hvutil_transport_send(struct hvutil_transport *hvt, void *msg, int len,
			  void (*on_read_cb)(void));
void hvutil_transport_destroy(struct hvutil_transport *hvt);

#endif /* _HV_UTILS_TRANSPORT_H */
