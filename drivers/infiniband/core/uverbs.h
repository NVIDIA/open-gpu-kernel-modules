/*
 * Copyright (c) 2005 Topspin Communications.  All rights reserved.
 * Copyright (c) 2005, 2006 Cisco Systems.  All rights reserved.
 * Copyright (c) 2005 Mellanox Technologies. All rights reserved.
 * Copyright (c) 2005 Voltaire, Inc. All rights reserved.
 * Copyright (c) 2005 PathScale, Inc. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef UVERBS_H
#define UVERBS_H

#include <linux/kref.h>
#include <linux/idr.h>
#include <linux/mutex.h>
#include <linux/completion.h>
#include <linux/cdev.h>

#include <rdma/ib_verbs.h>
#include <rdma/ib_umem.h>
#include <rdma/ib_user_verbs.h>
#include <rdma/uverbs_std_types.h>

#define UVERBS_MODULE_NAME ib_uverbs
#include <rdma/uverbs_named_ioctl.h>

static inline void
ib_uverbs_init_udata(struct ib_udata *udata,
		     const void __user *ibuf,
		     void __user *obuf,
		     size_t ilen, size_t olen)
{
	udata->inbuf  = ibuf;
	udata->outbuf = obuf;
	udata->inlen  = ilen;
	udata->outlen = olen;
}

static inline void
ib_uverbs_init_udata_buf_or_null(struct ib_udata *udata,
				 const void __user *ibuf,
				 void __user *obuf,
				 size_t ilen, size_t olen)
{
	ib_uverbs_init_udata(udata,
			     ilen ? ibuf : NULL, olen ? obuf : NULL,
			     ilen, olen);
}

/*
 * Our lifetime rules for these structs are the following:
 *
 * struct ib_uverbs_device: One reference is held by the module and
 * released in ib_uverbs_remove_one().  Another reference is taken by
 * ib_uverbs_open() each time the character special file is opened,
 * and released in ib_uverbs_release_file() when the file is released.
 *
 * struct ib_uverbs_file: One reference is held by the VFS and
 * released when the file is closed.  Another reference is taken when
 * an asynchronous event queue file is created and released when the
 * event file is closed.
 *
 * struct ib_uverbs_event_queue: Base structure for
 * struct ib_uverbs_async_event_file and struct ib_uverbs_completion_event_file.
 * One reference is held by the VFS and released when the file is closed.
 * For asynchronous event files, another reference is held by the corresponding
 * main context file and released when that file is closed.  For completion
 * event files, a reference is taken when a CQ is created that uses the file,
 * and released when the CQ is destroyed.
 */

struct ib_uverbs_device {
	atomic_t				refcount;
	u32					num_comp_vectors;
	struct completion			comp;
	struct device				dev;
	/* First group for device attributes, NULL terminated array */
	const struct attribute_group		*groups[2];
	struct ib_device	__rcu	       *ib_dev;
	int					devnum;
	struct cdev			        cdev;
	struct rb_root				xrcd_tree;
	struct mutex				xrcd_tree_mutex;
	struct srcu_struct			disassociate_srcu;
	struct mutex				lists_mutex; /* protect lists */
	struct list_head			uverbs_file_list;
	struct uverbs_api			*uapi;
};

struct ib_uverbs_event_queue {
	spinlock_t				lock;
	int					is_closed;
	wait_queue_head_t			poll_wait;
	struct fasync_struct		       *async_queue;
	struct list_head			event_list;
};

struct ib_uverbs_async_event_file {
	struct ib_uobject			uobj;
	struct ib_uverbs_event_queue		ev_queue;
	struct ib_event_handler			event_handler;
};

struct ib_uverbs_completion_event_file {
	struct ib_uobject			uobj;
	struct ib_uverbs_event_queue		ev_queue;
};

struct ib_uverbs_file {
	struct kref				ref;
	struct ib_uverbs_device		       *device;
	struct mutex				ucontext_lock;
	/*
	 * ucontext must be accessed via ib_uverbs_get_ucontext() or with
	 * ucontext_lock held
	 */
	struct ib_ucontext		       *ucontext;
	struct ib_uverbs_async_event_file      *default_async_file;
	struct list_head			list;

	/*
	 * To access the uobjects list hw_destroy_rwsem must be held for write
	 * OR hw_destroy_rwsem held for read AND uobjects_lock held.
	 * hw_destroy_rwsem should be called across any destruction of the HW
	 * object of an associated uobject.
	 */
	struct rw_semaphore	hw_destroy_rwsem;
	spinlock_t		uobjects_lock;
	struct list_head	uobjects;

	struct mutex umap_lock;
	struct list_head umaps;
	struct page *disassociate_page;

	struct xarray		idr;
};

struct ib_uverbs_event {
	union {
		struct ib_uverbs_async_event_desc	async;
		struct ib_uverbs_comp_event_desc	comp;
	}					desc;
	struct list_head			list;
	struct list_head			obj_list;
	u32				       *counter;
};

struct ib_uverbs_mcast_entry {
	struct list_head	list;
	union ib_gid 		gid;
	u16 			lid;
};

struct ib_uevent_object {
	struct ib_uobject	uobject;
	struct ib_uverbs_async_event_file *event_file;
	/* List member for ib_uverbs_async_event_file list */
	struct list_head	event_list;
	u32			events_reported;
};

struct ib_uxrcd_object {
	struct ib_uobject	uobject;
	atomic_t		refcnt;
};

struct ib_usrq_object {
	struct ib_uevent_object	uevent;
	struct ib_uxrcd_object *uxrcd;
};

struct ib_uqp_object {
	struct ib_uevent_object	uevent;
	/* lock for mcast list */
	struct mutex		mcast_lock;
	struct list_head 	mcast_list;
	struct ib_uxrcd_object *uxrcd;
};

struct ib_uwq_object {
	struct ib_uevent_object	uevent;
};

struct ib_ucq_object {
	struct ib_uevent_object uevent;
	struct list_head	comp_list;
	u32			comp_events_reported;
};

extern const struct file_operations uverbs_event_fops;
extern const struct file_operations uverbs_async_event_fops;
void ib_uverbs_init_event_queue(struct ib_uverbs_event_queue *ev_queue);
void ib_uverbs_init_async_event_file(struct ib_uverbs_async_event_file *ev_file);
void ib_uverbs_free_event_queue(struct ib_uverbs_event_queue *event_queue);
void ib_uverbs_flow_resources_free(struct ib_uflow_resources *uflow_res);
int uverbs_async_event_release(struct inode *inode, struct file *filp);

int ib_alloc_ucontext(struct uverbs_attr_bundle *attrs);
int ib_init_ucontext(struct uverbs_attr_bundle *attrs);

void ib_uverbs_release_ucq(struct ib_uverbs_completion_event_file *ev_file,
			   struct ib_ucq_object *uobj);
void ib_uverbs_release_uevent(struct ib_uevent_object *uobj);
void ib_uverbs_release_file(struct kref *ref);
void ib_uverbs_async_handler(struct ib_uverbs_async_event_file *async_file,
			     __u64 element, __u64 event,
			     struct list_head *obj_list, u32 *counter);

void ib_uverbs_comp_handler(struct ib_cq *cq, void *cq_context);
void ib_uverbs_cq_event_handler(struct ib_event *event, void *context_ptr);
void ib_uverbs_qp_event_handler(struct ib_event *event, void *context_ptr);
void ib_uverbs_wq_event_handler(struct ib_event *event, void *context_ptr);
void ib_uverbs_srq_event_handler(struct ib_event *event, void *context_ptr);
int ib_uverbs_dealloc_xrcd(struct ib_uobject *uobject, struct ib_xrcd *xrcd,
			   enum rdma_remove_reason why,
			   struct uverbs_attr_bundle *attrs);

int uverbs_dealloc_mw(struct ib_mw *mw);
void ib_uverbs_detach_umcast(struct ib_qp *qp,
			     struct ib_uqp_object *uobj);

long ib_uverbs_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

struct ib_uverbs_flow_spec {
	union {
		union {
			struct ib_uverbs_flow_spec_hdr hdr;
			struct {
				__u32 type;
				__u16 size;
				__u16 reserved;
			};
		};
		struct ib_uverbs_flow_spec_eth     eth;
		struct ib_uverbs_flow_spec_ipv4    ipv4;
		struct ib_uverbs_flow_spec_esp     esp;
		struct ib_uverbs_flow_spec_tcp_udp tcp_udp;
		struct ib_uverbs_flow_spec_ipv6    ipv6;
		struct ib_uverbs_flow_spec_action_tag	flow_tag;
		struct ib_uverbs_flow_spec_action_drop	drop;
		struct ib_uverbs_flow_spec_action_handle action;
		struct ib_uverbs_flow_spec_action_count flow_count;
	};
};

int ib_uverbs_kern_spec_to_ib_spec_filter(enum ib_flow_spec_type type,
					  const void *kern_spec_mask,
					  const void *kern_spec_val,
					  size_t kern_filter_sz,
					  union ib_flow_spec *ib_spec);

/*
 * ib_uverbs_query_port_resp.port_cap_flags started out as just a copy of the
 * PortInfo CapabilityMask, but was extended with unique bits.
 */
static inline u32 make_port_cap_flags(const struct ib_port_attr *attr)
{
	u32 res;

	/* All IBA CapabilityMask bits are passed through here, except bit 26,
	 * which is overridden with IP_BASED_GIDS. This is due to a historical
	 * mistake in the implementation of IP_BASED_GIDS. Otherwise all other
	 * bits match the IBA definition across all kernel versions.
	 */
	res = attr->port_cap_flags & ~(u32)IB_UVERBS_PCF_IP_BASED_GIDS;

	if (attr->ip_gids)
		res |= IB_UVERBS_PCF_IP_BASED_GIDS;

	return res;
}

static inline struct ib_uverbs_async_event_file *
ib_uverbs_get_async_event(struct uverbs_attr_bundle *attrs,
			  u16 id)
{
	struct ib_uobject *async_ev_file_uobj;
	struct ib_uverbs_async_event_file *async_ev_file;

	async_ev_file_uobj = uverbs_attr_get_uobject(attrs, id);
	if (IS_ERR(async_ev_file_uobj))
		async_ev_file = READ_ONCE(attrs->ufile->default_async_file);
	else
		async_ev_file = container_of(async_ev_file_uobj,
				       struct ib_uverbs_async_event_file,
				       uobj);
	if (async_ev_file)
		uverbs_uobject_get(&async_ev_file->uobj);
	return async_ev_file;
}

void copy_port_attr_to_resp(struct ib_port_attr *attr,
			    struct ib_uverbs_query_port_resp *resp,
			    struct ib_device *ib_dev, u8 port_num);
#endif /* UVERBS_H */
