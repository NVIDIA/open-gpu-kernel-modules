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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/cdev.h>
#include <linux/anon_inodes.h>
#include <linux/slab.h>
#include <linux/sched/mm.h>

#include <linux/uaccess.h>

#include <rdma/ib.h>
#include <rdma/uverbs_std_types.h>
#include <rdma/rdma_netlink.h>

#include "uverbs.h"
#include "core_priv.h"
#include "rdma_core.h"

MODULE_AUTHOR("Roland Dreier");
MODULE_DESCRIPTION("InfiniBand userspace verbs access");
MODULE_LICENSE("Dual BSD/GPL");

enum {
	IB_UVERBS_MAJOR       = 231,
	IB_UVERBS_BASE_MINOR  = 192,
	IB_UVERBS_MAX_DEVICES = RDMA_MAX_PORTS,
	IB_UVERBS_NUM_FIXED_MINOR = 32,
	IB_UVERBS_NUM_DYNAMIC_MINOR = IB_UVERBS_MAX_DEVICES - IB_UVERBS_NUM_FIXED_MINOR,
};

#define IB_UVERBS_BASE_DEV	MKDEV(IB_UVERBS_MAJOR, IB_UVERBS_BASE_MINOR)

static dev_t dynamic_uverbs_dev;
static struct class *uverbs_class;

static DEFINE_IDA(uverbs_ida);
static int ib_uverbs_add_one(struct ib_device *device);
static void ib_uverbs_remove_one(struct ib_device *device, void *client_data);

/*
 * Must be called with the ufile->device->disassociate_srcu held, and the lock
 * must be held until use of the ucontext is finished.
 */
struct ib_ucontext *ib_uverbs_get_ucontext_file(struct ib_uverbs_file *ufile)
{
	/*
	 * We do not hold the hw_destroy_rwsem lock for this flow, instead
	 * srcu is used. It does not matter if someone races this with
	 * get_context, we get NULL or valid ucontext.
	 */
	struct ib_ucontext *ucontext = smp_load_acquire(&ufile->ucontext);

	if (!srcu_dereference(ufile->device->ib_dev,
			      &ufile->device->disassociate_srcu))
		return ERR_PTR(-EIO);

	if (!ucontext)
		return ERR_PTR(-EINVAL);

	return ucontext;
}
EXPORT_SYMBOL(ib_uverbs_get_ucontext_file);

int uverbs_dealloc_mw(struct ib_mw *mw)
{
	struct ib_pd *pd = mw->pd;
	int ret;

	ret = mw->device->ops.dealloc_mw(mw);
	if (ret)
		return ret;

	atomic_dec(&pd->usecnt);
	kfree(mw);
	return ret;
}

static void ib_uverbs_release_dev(struct device *device)
{
	struct ib_uverbs_device *dev =
			container_of(device, struct ib_uverbs_device, dev);

	uverbs_destroy_api(dev->uapi);
	cleanup_srcu_struct(&dev->disassociate_srcu);
	mutex_destroy(&dev->lists_mutex);
	mutex_destroy(&dev->xrcd_tree_mutex);
	kfree(dev);
}

void ib_uverbs_release_ucq(struct ib_uverbs_completion_event_file *ev_file,
			   struct ib_ucq_object *uobj)
{
	struct ib_uverbs_event *evt, *tmp;

	if (ev_file) {
		spin_lock_irq(&ev_file->ev_queue.lock);
		list_for_each_entry_safe(evt, tmp, &uobj->comp_list, obj_list) {
			list_del(&evt->list);
			kfree(evt);
		}
		spin_unlock_irq(&ev_file->ev_queue.lock);

		uverbs_uobject_put(&ev_file->uobj);
	}

	ib_uverbs_release_uevent(&uobj->uevent);
}

void ib_uverbs_release_uevent(struct ib_uevent_object *uobj)
{
	struct ib_uverbs_async_event_file *async_file = uobj->event_file;
	struct ib_uverbs_event *evt, *tmp;

	if (!async_file)
		return;

	spin_lock_irq(&async_file->ev_queue.lock);
	list_for_each_entry_safe(evt, tmp, &uobj->event_list, obj_list) {
		list_del(&evt->list);
		kfree(evt);
	}
	spin_unlock_irq(&async_file->ev_queue.lock);
	uverbs_uobject_put(&async_file->uobj);
}

void ib_uverbs_detach_umcast(struct ib_qp *qp,
			     struct ib_uqp_object *uobj)
{
	struct ib_uverbs_mcast_entry *mcast, *tmp;

	list_for_each_entry_safe(mcast, tmp, &uobj->mcast_list, list) {
		ib_detach_mcast(qp, &mcast->gid, mcast->lid);
		list_del(&mcast->list);
		kfree(mcast);
	}
}

static void ib_uverbs_comp_dev(struct ib_uverbs_device *dev)
{
	complete(&dev->comp);
}

void ib_uverbs_release_file(struct kref *ref)
{
	struct ib_uverbs_file *file =
		container_of(ref, struct ib_uverbs_file, ref);
	struct ib_device *ib_dev;
	int srcu_key;

	release_ufile_idr_uobject(file);

	srcu_key = srcu_read_lock(&file->device->disassociate_srcu);
	ib_dev = srcu_dereference(file->device->ib_dev,
				  &file->device->disassociate_srcu);
	if (ib_dev && !ib_dev->ops.disassociate_ucontext)
		module_put(ib_dev->ops.owner);
	srcu_read_unlock(&file->device->disassociate_srcu, srcu_key);

	if (atomic_dec_and_test(&file->device->refcount))
		ib_uverbs_comp_dev(file->device);

	if (file->default_async_file)
		uverbs_uobject_put(&file->default_async_file->uobj);
	put_device(&file->device->dev);

	if (file->disassociate_page)
		__free_pages(file->disassociate_page, 0);
	mutex_destroy(&file->umap_lock);
	mutex_destroy(&file->ucontext_lock);
	kfree(file);
}

static ssize_t ib_uverbs_event_read(struct ib_uverbs_event_queue *ev_queue,
				    struct file *filp, char __user *buf,
				    size_t count, loff_t *pos,
				    size_t eventsz)
{
	struct ib_uverbs_event *event;
	int ret = 0;

	spin_lock_irq(&ev_queue->lock);

	while (list_empty(&ev_queue->event_list)) {
		spin_unlock_irq(&ev_queue->lock);

		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;

		if (wait_event_interruptible(ev_queue->poll_wait,
					     (!list_empty(&ev_queue->event_list) ||
					      ev_queue->is_closed)))
			return -ERESTARTSYS;

		spin_lock_irq(&ev_queue->lock);

		/* If device was disassociated and no event exists set an error */
		if (list_empty(&ev_queue->event_list) && ev_queue->is_closed) {
			spin_unlock_irq(&ev_queue->lock);
			return -EIO;
		}
	}

	event = list_entry(ev_queue->event_list.next, struct ib_uverbs_event, list);

	if (eventsz > count) {
		ret   = -EINVAL;
		event = NULL;
	} else {
		list_del(ev_queue->event_list.next);
		if (event->counter) {
			++(*event->counter);
			list_del(&event->obj_list);
		}
	}

	spin_unlock_irq(&ev_queue->lock);

	if (event) {
		if (copy_to_user(buf, event, eventsz))
			ret = -EFAULT;
		else
			ret = eventsz;
	}

	kfree(event);

	return ret;
}

static ssize_t ib_uverbs_async_event_read(struct file *filp, char __user *buf,
					  size_t count, loff_t *pos)
{
	struct ib_uverbs_async_event_file *file = filp->private_data;

	return ib_uverbs_event_read(&file->ev_queue, filp, buf, count, pos,
				    sizeof(struct ib_uverbs_async_event_desc));
}

static ssize_t ib_uverbs_comp_event_read(struct file *filp, char __user *buf,
					 size_t count, loff_t *pos)
{
	struct ib_uverbs_completion_event_file *comp_ev_file =
		filp->private_data;

	return ib_uverbs_event_read(&comp_ev_file->ev_queue, filp, buf, count,
				    pos,
				    sizeof(struct ib_uverbs_comp_event_desc));
}

static __poll_t ib_uverbs_event_poll(struct ib_uverbs_event_queue *ev_queue,
					 struct file *filp,
					 struct poll_table_struct *wait)
{
	__poll_t pollflags = 0;

	poll_wait(filp, &ev_queue->poll_wait, wait);

	spin_lock_irq(&ev_queue->lock);
	if (!list_empty(&ev_queue->event_list))
		pollflags = EPOLLIN | EPOLLRDNORM;
	else if (ev_queue->is_closed)
		pollflags = EPOLLERR;
	spin_unlock_irq(&ev_queue->lock);

	return pollflags;
}

static __poll_t ib_uverbs_async_event_poll(struct file *filp,
					       struct poll_table_struct *wait)
{
	struct ib_uverbs_async_event_file *file = filp->private_data;

	return ib_uverbs_event_poll(&file->ev_queue, filp, wait);
}

static __poll_t ib_uverbs_comp_event_poll(struct file *filp,
					      struct poll_table_struct *wait)
{
	struct ib_uverbs_completion_event_file *comp_ev_file =
		filp->private_data;

	return ib_uverbs_event_poll(&comp_ev_file->ev_queue, filp, wait);
}

static int ib_uverbs_async_event_fasync(int fd, struct file *filp, int on)
{
	struct ib_uverbs_async_event_file *file = filp->private_data;

	return fasync_helper(fd, filp, on, &file->ev_queue.async_queue);
}

static int ib_uverbs_comp_event_fasync(int fd, struct file *filp, int on)
{
	struct ib_uverbs_completion_event_file *comp_ev_file =
		filp->private_data;

	return fasync_helper(fd, filp, on, &comp_ev_file->ev_queue.async_queue);
}

const struct file_operations uverbs_event_fops = {
	.owner	 = THIS_MODULE,
	.read	 = ib_uverbs_comp_event_read,
	.poll    = ib_uverbs_comp_event_poll,
	.release = uverbs_uobject_fd_release,
	.fasync  = ib_uverbs_comp_event_fasync,
	.llseek	 = no_llseek,
};

const struct file_operations uverbs_async_event_fops = {
	.owner	 = THIS_MODULE,
	.read	 = ib_uverbs_async_event_read,
	.poll    = ib_uverbs_async_event_poll,
	.release = uverbs_async_event_release,
	.fasync  = ib_uverbs_async_event_fasync,
	.llseek	 = no_llseek,
};

void ib_uverbs_comp_handler(struct ib_cq *cq, void *cq_context)
{
	struct ib_uverbs_event_queue   *ev_queue = cq_context;
	struct ib_ucq_object	       *uobj;
	struct ib_uverbs_event	       *entry;
	unsigned long			flags;

	if (!ev_queue)
		return;

	spin_lock_irqsave(&ev_queue->lock, flags);
	if (ev_queue->is_closed) {
		spin_unlock_irqrestore(&ev_queue->lock, flags);
		return;
	}

	entry = kmalloc(sizeof(*entry), GFP_ATOMIC);
	if (!entry) {
		spin_unlock_irqrestore(&ev_queue->lock, flags);
		return;
	}

	uobj = cq->uobject;

	entry->desc.comp.cq_handle = cq->uobject->uevent.uobject.user_handle;
	entry->counter		   = &uobj->comp_events_reported;

	list_add_tail(&entry->list, &ev_queue->event_list);
	list_add_tail(&entry->obj_list, &uobj->comp_list);
	spin_unlock_irqrestore(&ev_queue->lock, flags);

	wake_up_interruptible(&ev_queue->poll_wait);
	kill_fasync(&ev_queue->async_queue, SIGIO, POLL_IN);
}

void ib_uverbs_async_handler(struct ib_uverbs_async_event_file *async_file,
			     __u64 element, __u64 event,
			     struct list_head *obj_list, u32 *counter)
{
	struct ib_uverbs_event *entry;
	unsigned long flags;

	if (!async_file)
		return;

	spin_lock_irqsave(&async_file->ev_queue.lock, flags);
	if (async_file->ev_queue.is_closed) {
		spin_unlock_irqrestore(&async_file->ev_queue.lock, flags);
		return;
	}

	entry = kmalloc(sizeof(*entry), GFP_ATOMIC);
	if (!entry) {
		spin_unlock_irqrestore(&async_file->ev_queue.lock, flags);
		return;
	}

	entry->desc.async.element = element;
	entry->desc.async.event_type = event;
	entry->desc.async.reserved = 0;
	entry->counter = counter;

	list_add_tail(&entry->list, &async_file->ev_queue.event_list);
	if (obj_list)
		list_add_tail(&entry->obj_list, obj_list);
	spin_unlock_irqrestore(&async_file->ev_queue.lock, flags);

	wake_up_interruptible(&async_file->ev_queue.poll_wait);
	kill_fasync(&async_file->ev_queue.async_queue, SIGIO, POLL_IN);
}

static void uverbs_uobj_event(struct ib_uevent_object *eobj,
			      struct ib_event *event)
{
	ib_uverbs_async_handler(eobj->event_file,
				eobj->uobject.user_handle, event->event,
				&eobj->event_list, &eobj->events_reported);
}

void ib_uverbs_cq_event_handler(struct ib_event *event, void *context_ptr)
{
	uverbs_uobj_event(&event->element.cq->uobject->uevent, event);
}

void ib_uverbs_qp_event_handler(struct ib_event *event, void *context_ptr)
{
	/* for XRC target qp's, check that qp is live */
	if (!event->element.qp->uobject)
		return;

	uverbs_uobj_event(&event->element.qp->uobject->uevent, event);
}

void ib_uverbs_wq_event_handler(struct ib_event *event, void *context_ptr)
{
	uverbs_uobj_event(&event->element.wq->uobject->uevent, event);
}

void ib_uverbs_srq_event_handler(struct ib_event *event, void *context_ptr)
{
	uverbs_uobj_event(&event->element.srq->uobject->uevent, event);
}

static void ib_uverbs_event_handler(struct ib_event_handler *handler,
				    struct ib_event *event)
{
	ib_uverbs_async_handler(
		container_of(handler, struct ib_uverbs_async_event_file,
			     event_handler),
		event->element.port_num, event->event, NULL, NULL);
}

void ib_uverbs_init_event_queue(struct ib_uverbs_event_queue *ev_queue)
{
	spin_lock_init(&ev_queue->lock);
	INIT_LIST_HEAD(&ev_queue->event_list);
	init_waitqueue_head(&ev_queue->poll_wait);
	ev_queue->is_closed   = 0;
	ev_queue->async_queue = NULL;
}

void ib_uverbs_init_async_event_file(
	struct ib_uverbs_async_event_file *async_file)
{
	struct ib_uverbs_file *uverbs_file = async_file->uobj.ufile;
	struct ib_device *ib_dev = async_file->uobj.context->device;

	ib_uverbs_init_event_queue(&async_file->ev_queue);

	/* The first async_event_file becomes the default one for the file. */
	mutex_lock(&uverbs_file->ucontext_lock);
	if (!uverbs_file->default_async_file) {
		/* Pairs with the put in ib_uverbs_release_file */
		uverbs_uobject_get(&async_file->uobj);
		smp_store_release(&uverbs_file->default_async_file, async_file);
	}
	mutex_unlock(&uverbs_file->ucontext_lock);

	INIT_IB_EVENT_HANDLER(&async_file->event_handler, ib_dev,
			      ib_uverbs_event_handler);
	ib_register_event_handler(&async_file->event_handler);
}

static ssize_t verify_hdr(struct ib_uverbs_cmd_hdr *hdr,
			  struct ib_uverbs_ex_cmd_hdr *ex_hdr, size_t count,
			  const struct uverbs_api_write_method *method_elm)
{
	if (method_elm->is_ex) {
		count -= sizeof(*hdr) + sizeof(*ex_hdr);

		if ((hdr->in_words + ex_hdr->provider_in_words) * 8 != count)
			return -EINVAL;

		if (hdr->in_words * 8 < method_elm->req_size)
			return -ENOSPC;

		if (ex_hdr->cmd_hdr_reserved)
			return -EINVAL;

		if (ex_hdr->response) {
			if (!hdr->out_words && !ex_hdr->provider_out_words)
				return -EINVAL;

			if (hdr->out_words * 8 < method_elm->resp_size)
				return -ENOSPC;

			if (!access_ok(u64_to_user_ptr(ex_hdr->response),
				       (hdr->out_words + ex_hdr->provider_out_words) * 8))
				return -EFAULT;
		} else {
			if (hdr->out_words || ex_hdr->provider_out_words)
				return -EINVAL;
		}

		return 0;
	}

	/* not extended command */
	if (hdr->in_words * 4 != count)
		return -EINVAL;

	if (count < method_elm->req_size + sizeof(hdr)) {
		/*
		 * rdma-core v18 and v19 have a bug where they send DESTROY_CQ
		 * with a 16 byte write instead of 24. Old kernels didn't
		 * check the size so they allowed this. Now that the size is
		 * checked provide a compatibility work around to not break
		 * those userspaces.
		 */
		if (hdr->command == IB_USER_VERBS_CMD_DESTROY_CQ &&
		    count == 16) {
			hdr->in_words = 6;
			return 0;
		}
		return -ENOSPC;
	}
	if (hdr->out_words * 4 < method_elm->resp_size)
		return -ENOSPC;

	return 0;
}

static ssize_t ib_uverbs_write(struct file *filp, const char __user *buf,
			     size_t count, loff_t *pos)
{
	struct ib_uverbs_file *file = filp->private_data;
	const struct uverbs_api_write_method *method_elm;
	struct uverbs_api *uapi = file->device->uapi;
	struct ib_uverbs_ex_cmd_hdr ex_hdr;
	struct ib_uverbs_cmd_hdr hdr;
	struct uverbs_attr_bundle bundle;
	int srcu_key;
	ssize_t ret;

	if (!ib_safe_file_access(filp)) {
		pr_err_once("uverbs_write: process %d (%s) changed security contexts after opening file descriptor, this is not allowed.\n",
			    task_tgid_vnr(current), current->comm);
		return -EACCES;
	}

	if (count < sizeof(hdr))
		return -EINVAL;

	if (copy_from_user(&hdr, buf, sizeof(hdr)))
		return -EFAULT;

	method_elm = uapi_get_method(uapi, hdr.command);
	if (IS_ERR(method_elm))
		return PTR_ERR(method_elm);

	if (method_elm->is_ex) {
		if (count < (sizeof(hdr) + sizeof(ex_hdr)))
			return -EINVAL;
		if (copy_from_user(&ex_hdr, buf + sizeof(hdr), sizeof(ex_hdr)))
			return -EFAULT;
	}

	ret = verify_hdr(&hdr, &ex_hdr, count, method_elm);
	if (ret)
		return ret;

	srcu_key = srcu_read_lock(&file->device->disassociate_srcu);

	buf += sizeof(hdr);

	memset(bundle.attr_present, 0, sizeof(bundle.attr_present));
	bundle.ufile = file;
	bundle.context = NULL; /* only valid if bundle has uobject */
	bundle.uobject = NULL;
	if (!method_elm->is_ex) {
		size_t in_len = hdr.in_words * 4 - sizeof(hdr);
		size_t out_len = hdr.out_words * 4;
		u64 response = 0;

		if (method_elm->has_udata) {
			bundle.driver_udata.inlen =
				in_len - method_elm->req_size;
			in_len = method_elm->req_size;
			if (bundle.driver_udata.inlen)
				bundle.driver_udata.inbuf = buf + in_len;
			else
				bundle.driver_udata.inbuf = NULL;
		} else {
			memset(&bundle.driver_udata, 0,
			       sizeof(bundle.driver_udata));
		}

		if (method_elm->has_resp) {
			/*
			 * The macros check that if has_resp is set
			 * then the command request structure starts
			 * with a '__aligned u64 response' member.
			 */
			ret = get_user(response, (const u64 __user *)buf);
			if (ret)
				goto out_unlock;

			if (method_elm->has_udata) {
				bundle.driver_udata.outlen =
					out_len - method_elm->resp_size;
				out_len = method_elm->resp_size;
				if (bundle.driver_udata.outlen)
					bundle.driver_udata.outbuf =
						u64_to_user_ptr(response +
								out_len);
				else
					bundle.driver_udata.outbuf = NULL;
			}
		} else {
			bundle.driver_udata.outlen = 0;
			bundle.driver_udata.outbuf = NULL;
		}

		ib_uverbs_init_udata_buf_or_null(
			&bundle.ucore, buf, u64_to_user_ptr(response),
			in_len, out_len);
	} else {
		buf += sizeof(ex_hdr);

		ib_uverbs_init_udata_buf_or_null(&bundle.ucore, buf,
					u64_to_user_ptr(ex_hdr.response),
					hdr.in_words * 8, hdr.out_words * 8);

		ib_uverbs_init_udata_buf_or_null(
			&bundle.driver_udata, buf + bundle.ucore.inlen,
			u64_to_user_ptr(ex_hdr.response) + bundle.ucore.outlen,
			ex_hdr.provider_in_words * 8,
			ex_hdr.provider_out_words * 8);

	}

	ret = method_elm->handler(&bundle);
	if (bundle.uobject)
		uverbs_finalize_object(bundle.uobject, UVERBS_ACCESS_NEW, true,
				       !ret, &bundle);
out_unlock:
	srcu_read_unlock(&file->device->disassociate_srcu, srcu_key);
	return (ret) ? : count;
}

static const struct vm_operations_struct rdma_umap_ops;

static int ib_uverbs_mmap(struct file *filp, struct vm_area_struct *vma)
{
	struct ib_uverbs_file *file = filp->private_data;
	struct ib_ucontext *ucontext;
	int ret = 0;
	int srcu_key;

	srcu_key = srcu_read_lock(&file->device->disassociate_srcu);
	ucontext = ib_uverbs_get_ucontext_file(file);
	if (IS_ERR(ucontext)) {
		ret = PTR_ERR(ucontext);
		goto out;
	}
	vma->vm_ops = &rdma_umap_ops;
	ret = ucontext->device->ops.mmap(ucontext, vma);
out:
	srcu_read_unlock(&file->device->disassociate_srcu, srcu_key);
	return ret;
}

/*
 * The VMA has been dup'd, initialize the vm_private_data with a new tracking
 * struct
 */
static void rdma_umap_open(struct vm_area_struct *vma)
{
	struct ib_uverbs_file *ufile = vma->vm_file->private_data;
	struct rdma_umap_priv *opriv = vma->vm_private_data;
	struct rdma_umap_priv *priv;

	if (!opriv)
		return;

	/* We are racing with disassociation */
	if (!down_read_trylock(&ufile->hw_destroy_rwsem))
		goto out_zap;
	/*
	 * Disassociation already completed, the VMA should already be zapped.
	 */
	if (!ufile->ucontext)
		goto out_unlock;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv)
		goto out_unlock;
	rdma_umap_priv_init(priv, vma, opriv->entry);

	up_read(&ufile->hw_destroy_rwsem);
	return;

out_unlock:
	up_read(&ufile->hw_destroy_rwsem);
out_zap:
	/*
	 * We can't allow the VMA to be created with the actual IO pages, that
	 * would break our API contract, and it can't be stopped at this
	 * point, so zap it.
	 */
	vma->vm_private_data = NULL;
	zap_vma_ptes(vma, vma->vm_start, vma->vm_end - vma->vm_start);
}

static void rdma_umap_close(struct vm_area_struct *vma)
{
	struct ib_uverbs_file *ufile = vma->vm_file->private_data;
	struct rdma_umap_priv *priv = vma->vm_private_data;

	if (!priv)
		return;

	/*
	 * The vma holds a reference on the struct file that created it, which
	 * in turn means that the ib_uverbs_file is guaranteed to exist at
	 * this point.
	 */
	mutex_lock(&ufile->umap_lock);
	if (priv->entry)
		rdma_user_mmap_entry_put(priv->entry);

	list_del(&priv->list);
	mutex_unlock(&ufile->umap_lock);
	kfree(priv);
}

/*
 * Once the zap_vma_ptes has been called touches to the VMA will come here and
 * we return a dummy writable zero page for all the pfns.
 */
static vm_fault_t rdma_umap_fault(struct vm_fault *vmf)
{
	struct ib_uverbs_file *ufile = vmf->vma->vm_file->private_data;
	struct rdma_umap_priv *priv = vmf->vma->vm_private_data;
	vm_fault_t ret = 0;

	if (!priv)
		return VM_FAULT_SIGBUS;

	/* Read only pages can just use the system zero page. */
	if (!(vmf->vma->vm_flags & (VM_WRITE | VM_MAYWRITE))) {
		vmf->page = ZERO_PAGE(vmf->address);
		get_page(vmf->page);
		return 0;
	}

	mutex_lock(&ufile->umap_lock);
	if (!ufile->disassociate_page)
		ufile->disassociate_page =
			alloc_pages(vmf->gfp_mask | __GFP_ZERO, 0);

	if (ufile->disassociate_page) {
		/*
		 * This VMA is forced to always be shared so this doesn't have
		 * to worry about COW.
		 */
		vmf->page = ufile->disassociate_page;
		get_page(vmf->page);
	} else {
		ret = VM_FAULT_SIGBUS;
	}
	mutex_unlock(&ufile->umap_lock);

	return ret;
}

static const struct vm_operations_struct rdma_umap_ops = {
	.open = rdma_umap_open,
	.close = rdma_umap_close,
	.fault = rdma_umap_fault,
};

void uverbs_user_mmap_disassociate(struct ib_uverbs_file *ufile)
{
	struct rdma_umap_priv *priv, *next_priv;

	lockdep_assert_held(&ufile->hw_destroy_rwsem);

	while (1) {
		struct mm_struct *mm = NULL;

		/* Get an arbitrary mm pointer that hasn't been cleaned yet */
		mutex_lock(&ufile->umap_lock);
		while (!list_empty(&ufile->umaps)) {
			int ret;

			priv = list_first_entry(&ufile->umaps,
						struct rdma_umap_priv, list);
			mm = priv->vma->vm_mm;
			ret = mmget_not_zero(mm);
			if (!ret) {
				list_del_init(&priv->list);
				if (priv->entry) {
					rdma_user_mmap_entry_put(priv->entry);
					priv->entry = NULL;
				}
				mm = NULL;
				continue;
			}
			break;
		}
		mutex_unlock(&ufile->umap_lock);
		if (!mm)
			return;

		/*
		 * The umap_lock is nested under mmap_lock since it used within
		 * the vma_ops callbacks, so we have to clean the list one mm
		 * at a time to get the lock ordering right. Typically there
		 * will only be one mm, so no big deal.
		 */
		mmap_read_lock(mm);
		mutex_lock(&ufile->umap_lock);
		list_for_each_entry_safe (priv, next_priv, &ufile->umaps,
					  list) {
			struct vm_area_struct *vma = priv->vma;

			if (vma->vm_mm != mm)
				continue;
			list_del_init(&priv->list);

			zap_vma_ptes(vma, vma->vm_start,
				     vma->vm_end - vma->vm_start);

			if (priv->entry) {
				rdma_user_mmap_entry_put(priv->entry);
				priv->entry = NULL;
			}
		}
		mutex_unlock(&ufile->umap_lock);
		mmap_read_unlock(mm);
		mmput(mm);
	}
}

/*
 * ib_uverbs_open() does not need the BKL:
 *
 *  - the ib_uverbs_device structures are properly reference counted and
 *    everything else is purely local to the file being created, so
 *    races against other open calls are not a problem;
 *  - there is no ioctl method to race against;
 *  - the open method will either immediately run -ENXIO, or all
 *    required initialization will be done.
 */
static int ib_uverbs_open(struct inode *inode, struct file *filp)
{
	struct ib_uverbs_device *dev;
	struct ib_uverbs_file *file;
	struct ib_device *ib_dev;
	int ret;
	int module_dependent;
	int srcu_key;

	dev = container_of(inode->i_cdev, struct ib_uverbs_device, cdev);
	if (!atomic_inc_not_zero(&dev->refcount))
		return -ENXIO;

	get_device(&dev->dev);
	srcu_key = srcu_read_lock(&dev->disassociate_srcu);
	mutex_lock(&dev->lists_mutex);
	ib_dev = srcu_dereference(dev->ib_dev,
				  &dev->disassociate_srcu);
	if (!ib_dev) {
		ret = -EIO;
		goto err;
	}

	if (!rdma_dev_access_netns(ib_dev, current->nsproxy->net_ns)) {
		ret = -EPERM;
		goto err;
	}

	/* In case IB device supports disassociate ucontext, there is no hard
	 * dependency between uverbs device and its low level device.
	 */
	module_dependent = !(ib_dev->ops.disassociate_ucontext);

	if (module_dependent) {
		if (!try_module_get(ib_dev->ops.owner)) {
			ret = -ENODEV;
			goto err;
		}
	}

	file = kzalloc(sizeof(*file), GFP_KERNEL);
	if (!file) {
		ret = -ENOMEM;
		if (module_dependent)
			goto err_module;

		goto err;
	}

	file->device	 = dev;
	kref_init(&file->ref);
	mutex_init(&file->ucontext_lock);

	spin_lock_init(&file->uobjects_lock);
	INIT_LIST_HEAD(&file->uobjects);
	init_rwsem(&file->hw_destroy_rwsem);
	mutex_init(&file->umap_lock);
	INIT_LIST_HEAD(&file->umaps);

	filp->private_data = file;
	list_add_tail(&file->list, &dev->uverbs_file_list);
	mutex_unlock(&dev->lists_mutex);
	srcu_read_unlock(&dev->disassociate_srcu, srcu_key);

	setup_ufile_idr_uobject(file);

	return stream_open(inode, filp);

err_module:
	module_put(ib_dev->ops.owner);

err:
	mutex_unlock(&dev->lists_mutex);
	srcu_read_unlock(&dev->disassociate_srcu, srcu_key);
	if (atomic_dec_and_test(&dev->refcount))
		ib_uverbs_comp_dev(dev);

	put_device(&dev->dev);
	return ret;
}

static int ib_uverbs_close(struct inode *inode, struct file *filp)
{
	struct ib_uverbs_file *file = filp->private_data;

	uverbs_destroy_ufile_hw(file, RDMA_REMOVE_CLOSE);

	mutex_lock(&file->device->lists_mutex);
	list_del_init(&file->list);
	mutex_unlock(&file->device->lists_mutex);

	kref_put(&file->ref, ib_uverbs_release_file);

	return 0;
}

static const struct file_operations uverbs_fops = {
	.owner	 = THIS_MODULE,
	.write	 = ib_uverbs_write,
	.open	 = ib_uverbs_open,
	.release = ib_uverbs_close,
	.llseek	 = no_llseek,
	.unlocked_ioctl = ib_uverbs_ioctl,
	.compat_ioctl = compat_ptr_ioctl,
};

static const struct file_operations uverbs_mmap_fops = {
	.owner	 = THIS_MODULE,
	.write	 = ib_uverbs_write,
	.mmap    = ib_uverbs_mmap,
	.open	 = ib_uverbs_open,
	.release = ib_uverbs_close,
	.llseek	 = no_llseek,
	.unlocked_ioctl = ib_uverbs_ioctl,
	.compat_ioctl = compat_ptr_ioctl,
};

static int ib_uverbs_get_nl_info(struct ib_device *ibdev, void *client_data,
				 struct ib_client_nl_info *res)
{
	struct ib_uverbs_device *uverbs_dev = client_data;
	int ret;

	if (res->port != -1)
		return -EINVAL;

	res->abi = ibdev->ops.uverbs_abi_ver;
	res->cdev = &uverbs_dev->dev;

	/*
	 * To support DRIVER_ID binding in userspace some of the driver need
	 * upgrading to expose their PCI dependent revision information
	 * through get_context instead of relying on modalias matching. When
	 * the drivers are fixed they can drop this flag.
	 */
	if (!ibdev->ops.uverbs_no_driver_id_binding) {
		ret = nla_put_u32(res->nl_msg, RDMA_NLDEV_ATTR_UVERBS_DRIVER_ID,
				  ibdev->ops.driver_id);
		if (ret)
			return ret;
	}
	return 0;
}

static struct ib_client uverbs_client = {
	.name   = "uverbs",
	.no_kverbs_req = true,
	.add    = ib_uverbs_add_one,
	.remove = ib_uverbs_remove_one,
	.get_nl_info = ib_uverbs_get_nl_info,
};
MODULE_ALIAS_RDMA_CLIENT("uverbs");

static ssize_t ibdev_show(struct device *device, struct device_attribute *attr,
			  char *buf)
{
	struct ib_uverbs_device *dev =
			container_of(device, struct ib_uverbs_device, dev);
	int ret = -ENODEV;
	int srcu_key;
	struct ib_device *ib_dev;

	srcu_key = srcu_read_lock(&dev->disassociate_srcu);
	ib_dev = srcu_dereference(dev->ib_dev, &dev->disassociate_srcu);
	if (ib_dev)
		ret = sysfs_emit(buf, "%s\n", dev_name(&ib_dev->dev));
	srcu_read_unlock(&dev->disassociate_srcu, srcu_key);

	return ret;
}
static DEVICE_ATTR_RO(ibdev);

static ssize_t abi_version_show(struct device *device,
				struct device_attribute *attr, char *buf)
{
	struct ib_uverbs_device *dev =
			container_of(device, struct ib_uverbs_device, dev);
	int ret = -ENODEV;
	int srcu_key;
	struct ib_device *ib_dev;

	srcu_key = srcu_read_lock(&dev->disassociate_srcu);
	ib_dev = srcu_dereference(dev->ib_dev, &dev->disassociate_srcu);
	if (ib_dev)
		ret = sysfs_emit(buf, "%u\n", ib_dev->ops.uverbs_abi_ver);
	srcu_read_unlock(&dev->disassociate_srcu, srcu_key);

	return ret;
}
static DEVICE_ATTR_RO(abi_version);

static struct attribute *ib_dev_attrs[] = {
	&dev_attr_abi_version.attr,
	&dev_attr_ibdev.attr,
	NULL,
};

static const struct attribute_group dev_attr_group = {
	.attrs = ib_dev_attrs,
};

static CLASS_ATTR_STRING(abi_version, S_IRUGO,
			 __stringify(IB_USER_VERBS_ABI_VERSION));

static int ib_uverbs_create_uapi(struct ib_device *device,
				 struct ib_uverbs_device *uverbs_dev)
{
	struct uverbs_api *uapi;

	uapi = uverbs_alloc_api(device);
	if (IS_ERR(uapi))
		return PTR_ERR(uapi);

	uverbs_dev->uapi = uapi;
	return 0;
}

static int ib_uverbs_add_one(struct ib_device *device)
{
	int devnum;
	dev_t base;
	struct ib_uverbs_device *uverbs_dev;
	int ret;

	if (!device->ops.alloc_ucontext)
		return -EOPNOTSUPP;

	uverbs_dev = kzalloc(sizeof(*uverbs_dev), GFP_KERNEL);
	if (!uverbs_dev)
		return -ENOMEM;

	ret = init_srcu_struct(&uverbs_dev->disassociate_srcu);
	if (ret) {
		kfree(uverbs_dev);
		return -ENOMEM;
	}

	device_initialize(&uverbs_dev->dev);
	uverbs_dev->dev.class = uverbs_class;
	uverbs_dev->dev.parent = device->dev.parent;
	uverbs_dev->dev.release = ib_uverbs_release_dev;
	uverbs_dev->groups[0] = &dev_attr_group;
	uverbs_dev->dev.groups = uverbs_dev->groups;
	atomic_set(&uverbs_dev->refcount, 1);
	init_completion(&uverbs_dev->comp);
	uverbs_dev->xrcd_tree = RB_ROOT;
	mutex_init(&uverbs_dev->xrcd_tree_mutex);
	mutex_init(&uverbs_dev->lists_mutex);
	INIT_LIST_HEAD(&uverbs_dev->uverbs_file_list);
	rcu_assign_pointer(uverbs_dev->ib_dev, device);
	uverbs_dev->num_comp_vectors = device->num_comp_vectors;

	devnum = ida_alloc_max(&uverbs_ida, IB_UVERBS_MAX_DEVICES - 1,
			       GFP_KERNEL);
	if (devnum < 0) {
		ret = -ENOMEM;
		goto err;
	}
	uverbs_dev->devnum = devnum;
	if (devnum >= IB_UVERBS_NUM_FIXED_MINOR)
		base = dynamic_uverbs_dev + devnum - IB_UVERBS_NUM_FIXED_MINOR;
	else
		base = IB_UVERBS_BASE_DEV + devnum;

	ret = ib_uverbs_create_uapi(device, uverbs_dev);
	if (ret)
		goto err_uapi;

	uverbs_dev->dev.devt = base;
	dev_set_name(&uverbs_dev->dev, "uverbs%d", uverbs_dev->devnum);

	cdev_init(&uverbs_dev->cdev,
		  device->ops.mmap ? &uverbs_mmap_fops : &uverbs_fops);
	uverbs_dev->cdev.owner = THIS_MODULE;

	ret = cdev_device_add(&uverbs_dev->cdev, &uverbs_dev->dev);
	if (ret)
		goto err_uapi;

	ib_set_client_data(device, &uverbs_client, uverbs_dev);
	return 0;

err_uapi:
	ida_free(&uverbs_ida, devnum);
err:
	if (atomic_dec_and_test(&uverbs_dev->refcount))
		ib_uverbs_comp_dev(uverbs_dev);
	wait_for_completion(&uverbs_dev->comp);
	put_device(&uverbs_dev->dev);
	return ret;
}

static void ib_uverbs_free_hw_resources(struct ib_uverbs_device *uverbs_dev,
					struct ib_device *ib_dev)
{
	struct ib_uverbs_file *file;

	/* Pending running commands to terminate */
	uverbs_disassociate_api_pre(uverbs_dev);

	mutex_lock(&uverbs_dev->lists_mutex);
	while (!list_empty(&uverbs_dev->uverbs_file_list)) {
		file = list_first_entry(&uverbs_dev->uverbs_file_list,
					struct ib_uverbs_file, list);
		list_del_init(&file->list);
		kref_get(&file->ref);

		/* We must release the mutex before going ahead and calling
		 * uverbs_cleanup_ufile, as it might end up indirectly calling
		 * uverbs_close, for example due to freeing the resources (e.g
		 * mmput).
		 */
		mutex_unlock(&uverbs_dev->lists_mutex);

		uverbs_destroy_ufile_hw(file, RDMA_REMOVE_DRIVER_REMOVE);
		kref_put(&file->ref, ib_uverbs_release_file);

		mutex_lock(&uverbs_dev->lists_mutex);
	}
	mutex_unlock(&uverbs_dev->lists_mutex);

	uverbs_disassociate_api(uverbs_dev->uapi);
}

static void ib_uverbs_remove_one(struct ib_device *device, void *client_data)
{
	struct ib_uverbs_device *uverbs_dev = client_data;
	int wait_clients = 1;

	cdev_device_del(&uverbs_dev->cdev, &uverbs_dev->dev);
	ida_free(&uverbs_ida, uverbs_dev->devnum);

	if (device->ops.disassociate_ucontext) {
		/* We disassociate HW resources and immediately return.
		 * Userspace will see a EIO errno for all future access.
		 * Upon returning, ib_device may be freed internally and is not
		 * valid any more.
		 * uverbs_device is still available until all clients close
		 * their files, then the uverbs device ref count will be zero
		 * and its resources will be freed.
		 * Note: At this point no more files can be opened since the
		 * cdev was deleted, however active clients can still issue
		 * commands and close their open files.
		 */
		ib_uverbs_free_hw_resources(uverbs_dev, device);
		wait_clients = 0;
	}

	if (atomic_dec_and_test(&uverbs_dev->refcount))
		ib_uverbs_comp_dev(uverbs_dev);
	if (wait_clients)
		wait_for_completion(&uverbs_dev->comp);

	put_device(&uverbs_dev->dev);
}

static char *uverbs_devnode(struct device *dev, umode_t *mode)
{
	if (mode)
		*mode = 0666;
	return kasprintf(GFP_KERNEL, "infiniband/%s", dev_name(dev));
}

static int __init ib_uverbs_init(void)
{
	int ret;

	ret = register_chrdev_region(IB_UVERBS_BASE_DEV,
				     IB_UVERBS_NUM_FIXED_MINOR,
				     "infiniband_verbs");
	if (ret) {
		pr_err("user_verbs: couldn't register device number\n");
		goto out;
	}

	ret = alloc_chrdev_region(&dynamic_uverbs_dev, 0,
				  IB_UVERBS_NUM_DYNAMIC_MINOR,
				  "infiniband_verbs");
	if (ret) {
		pr_err("couldn't register dynamic device number\n");
		goto out_alloc;
	}

	uverbs_class = class_create(THIS_MODULE, "infiniband_verbs");
	if (IS_ERR(uverbs_class)) {
		ret = PTR_ERR(uverbs_class);
		pr_err("user_verbs: couldn't create class infiniband_verbs\n");
		goto out_chrdev;
	}

	uverbs_class->devnode = uverbs_devnode;

	ret = class_create_file(uverbs_class, &class_attr_abi_version.attr);
	if (ret) {
		pr_err("user_verbs: couldn't create abi_version attribute\n");
		goto out_class;
	}

	ret = ib_register_client(&uverbs_client);
	if (ret) {
		pr_err("user_verbs: couldn't register client\n");
		goto out_class;
	}

	return 0;

out_class:
	class_destroy(uverbs_class);

out_chrdev:
	unregister_chrdev_region(dynamic_uverbs_dev,
				 IB_UVERBS_NUM_DYNAMIC_MINOR);

out_alloc:
	unregister_chrdev_region(IB_UVERBS_BASE_DEV,
				 IB_UVERBS_NUM_FIXED_MINOR);

out:
	return ret;
}

static void __exit ib_uverbs_cleanup(void)
{
	ib_unregister_client(&uverbs_client);
	class_destroy(uverbs_class);
	unregister_chrdev_region(IB_UVERBS_BASE_DEV,
				 IB_UVERBS_NUM_FIXED_MINOR);
	unregister_chrdev_region(dynamic_uverbs_dev,
				 IB_UVERBS_NUM_DYNAMIC_MINOR);
	mmu_notifier_synchronize();
}

module_init(ib_uverbs_init);
module_exit(ib_uverbs_cleanup);
