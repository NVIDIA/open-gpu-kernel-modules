// SPDX-License-Identifier: GPL-2.0-only
/*
 * VIRTIO based driver for vDPA device
 *
 * Copyright (c) 2020, Red Hat. All rights reserved.
 *     Author: Jason Wang <jasowang@redhat.com>
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/uuid.h>
#include <linux/virtio.h>
#include <linux/vdpa.h>
#include <linux/virtio_config.h>
#include <linux/virtio_ring.h>

#define MOD_VERSION  "0.1"
#define MOD_AUTHOR   "Jason Wang <jasowang@redhat.com>"
#define MOD_DESC     "vDPA bus driver for virtio devices"
#define MOD_LICENSE  "GPL v2"

struct virtio_vdpa_device {
	struct virtio_device vdev;
	struct vdpa_device *vdpa;
	u64 features;

	/* The lock to protect virtqueue list */
	spinlock_t lock;
	/* List of virtio_vdpa_vq_info */
	struct list_head virtqueues;
};

struct virtio_vdpa_vq_info {
	/* the actual virtqueue */
	struct virtqueue *vq;

	/* the list node for the virtqueues list */
	struct list_head node;
};

static inline struct virtio_vdpa_device *
to_virtio_vdpa_device(struct virtio_device *dev)
{
	return container_of(dev, struct virtio_vdpa_device, vdev);
}

static struct vdpa_device *vd_get_vdpa(struct virtio_device *vdev)
{
	return to_virtio_vdpa_device(vdev)->vdpa;
}

static void virtio_vdpa_get(struct virtio_device *vdev, unsigned offset,
			    void *buf, unsigned len)
{
	struct vdpa_device *vdpa = vd_get_vdpa(vdev);

	vdpa_get_config(vdpa, offset, buf, len);
}

static void virtio_vdpa_set(struct virtio_device *vdev, unsigned offset,
			    const void *buf, unsigned len)
{
	struct vdpa_device *vdpa = vd_get_vdpa(vdev);
	const struct vdpa_config_ops *ops = vdpa->config;

	ops->set_config(vdpa, offset, buf, len);
}

static u32 virtio_vdpa_generation(struct virtio_device *vdev)
{
	struct vdpa_device *vdpa = vd_get_vdpa(vdev);
	const struct vdpa_config_ops *ops = vdpa->config;

	if (ops->get_generation)
		return ops->get_generation(vdpa);

	return 0;
}

static u8 virtio_vdpa_get_status(struct virtio_device *vdev)
{
	struct vdpa_device *vdpa = vd_get_vdpa(vdev);
	const struct vdpa_config_ops *ops = vdpa->config;

	return ops->get_status(vdpa);
}

static void virtio_vdpa_set_status(struct virtio_device *vdev, u8 status)
{
	struct vdpa_device *vdpa = vd_get_vdpa(vdev);
	const struct vdpa_config_ops *ops = vdpa->config;

	return ops->set_status(vdpa, status);
}

static void virtio_vdpa_reset(struct virtio_device *vdev)
{
	struct vdpa_device *vdpa = vd_get_vdpa(vdev);

	vdpa_reset(vdpa);
}

static bool virtio_vdpa_notify(struct virtqueue *vq)
{
	struct vdpa_device *vdpa = vd_get_vdpa(vq->vdev);
	const struct vdpa_config_ops *ops = vdpa->config;

	ops->kick_vq(vdpa, vq->index);

	return true;
}

static irqreturn_t virtio_vdpa_config_cb(void *private)
{
	struct virtio_vdpa_device *vd_dev = private;

	virtio_config_changed(&vd_dev->vdev);

	return IRQ_HANDLED;
}

static irqreturn_t virtio_vdpa_virtqueue_cb(void *private)
{
	struct virtio_vdpa_vq_info *info = private;

	return vring_interrupt(0, info->vq);
}

static struct virtqueue *
virtio_vdpa_setup_vq(struct virtio_device *vdev, unsigned int index,
		     void (*callback)(struct virtqueue *vq),
		     const char *name, bool ctx)
{
	struct virtio_vdpa_device *vd_dev = to_virtio_vdpa_device(vdev);
	struct vdpa_device *vdpa = vd_get_vdpa(vdev);
	const struct vdpa_config_ops *ops = vdpa->config;
	struct virtio_vdpa_vq_info *info;
	struct vdpa_callback cb;
	struct virtqueue *vq;
	u64 desc_addr, driver_addr, device_addr;
	unsigned long flags;
	u32 align, num;
	int err;

	if (!name)
		return NULL;

	/* Queue shouldn't already be set up. */
	if (ops->get_vq_ready(vdpa, index))
		return ERR_PTR(-ENOENT);

	/* Allocate and fill out our active queue description */
	info = kmalloc(sizeof(*info), GFP_KERNEL);
	if (!info)
		return ERR_PTR(-ENOMEM);

	num = ops->get_vq_num_max(vdpa);
	if (num == 0) {
		err = -ENOENT;
		goto error_new_virtqueue;
	}

	/* Create the vring */
	align = ops->get_vq_align(vdpa);
	vq = vring_create_virtqueue(index, num, align, vdev,
				    true, true, ctx,
				    virtio_vdpa_notify, callback, name);
	if (!vq) {
		err = -ENOMEM;
		goto error_new_virtqueue;
	}

	/* Setup virtqueue callback */
	cb.callback = virtio_vdpa_virtqueue_cb;
	cb.private = info;
	ops->set_vq_cb(vdpa, index, &cb);
	ops->set_vq_num(vdpa, index, virtqueue_get_vring_size(vq));

	desc_addr = virtqueue_get_desc_addr(vq);
	driver_addr = virtqueue_get_avail_addr(vq);
	device_addr = virtqueue_get_used_addr(vq);

	if (ops->set_vq_address(vdpa, index,
				desc_addr, driver_addr,
				device_addr)) {
		err = -EINVAL;
		goto err_vq;
	}

	ops->set_vq_ready(vdpa, index, 1);

	vq->priv = info;
	info->vq = vq;

	spin_lock_irqsave(&vd_dev->lock, flags);
	list_add(&info->node, &vd_dev->virtqueues);
	spin_unlock_irqrestore(&vd_dev->lock, flags);

	return vq;

err_vq:
	vring_del_virtqueue(vq);
error_new_virtqueue:
	ops->set_vq_ready(vdpa, index, 0);
	/* VDPA driver should make sure vq is stopeed here */
	WARN_ON(ops->get_vq_ready(vdpa, index));
	kfree(info);
	return ERR_PTR(err);
}

static void virtio_vdpa_del_vq(struct virtqueue *vq)
{
	struct virtio_vdpa_device *vd_dev = to_virtio_vdpa_device(vq->vdev);
	struct vdpa_device *vdpa = vd_dev->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;
	struct virtio_vdpa_vq_info *info = vq->priv;
	unsigned int index = vq->index;
	unsigned long flags;

	spin_lock_irqsave(&vd_dev->lock, flags);
	list_del(&info->node);
	spin_unlock_irqrestore(&vd_dev->lock, flags);

	/* Select and deactivate the queue (best effort) */
	ops->set_vq_ready(vdpa, index, 0);

	vring_del_virtqueue(vq);

	kfree(info);
}

static void virtio_vdpa_del_vqs(struct virtio_device *vdev)
{
	struct virtqueue *vq, *n;

	list_for_each_entry_safe(vq, n, &vdev->vqs, list)
		virtio_vdpa_del_vq(vq);
}

static int virtio_vdpa_find_vqs(struct virtio_device *vdev, unsigned nvqs,
				struct virtqueue *vqs[],
				vq_callback_t *callbacks[],
				const char * const names[],
				const bool *ctx,
				struct irq_affinity *desc)
{
	struct virtio_vdpa_device *vd_dev = to_virtio_vdpa_device(vdev);
	struct vdpa_device *vdpa = vd_get_vdpa(vdev);
	const struct vdpa_config_ops *ops = vdpa->config;
	struct vdpa_callback cb;
	int i, err, queue_idx = 0;

	for (i = 0; i < nvqs; ++i) {
		if (!names[i]) {
			vqs[i] = NULL;
			continue;
		}

		vqs[i] = virtio_vdpa_setup_vq(vdev, queue_idx++,
					      callbacks[i], names[i], ctx ?
					      ctx[i] : false);
		if (IS_ERR(vqs[i])) {
			err = PTR_ERR(vqs[i]);
			goto err_setup_vq;
		}
	}

	cb.callback = virtio_vdpa_config_cb;
	cb.private = vd_dev;
	ops->set_config_cb(vdpa, &cb);

	return 0;

err_setup_vq:
	virtio_vdpa_del_vqs(vdev);
	return err;
}

static u64 virtio_vdpa_get_features(struct virtio_device *vdev)
{
	struct vdpa_device *vdpa = vd_get_vdpa(vdev);
	const struct vdpa_config_ops *ops = vdpa->config;

	return ops->get_features(vdpa);
}

static int virtio_vdpa_finalize_features(struct virtio_device *vdev)
{
	struct vdpa_device *vdpa = vd_get_vdpa(vdev);

	/* Give virtio_ring a chance to accept features. */
	vring_transport_features(vdev);

	return vdpa_set_features(vdpa, vdev->features);
}

static const char *virtio_vdpa_bus_name(struct virtio_device *vdev)
{
	struct virtio_vdpa_device *vd_dev = to_virtio_vdpa_device(vdev);
	struct vdpa_device *vdpa = vd_dev->vdpa;

	return dev_name(&vdpa->dev);
}

static const struct virtio_config_ops virtio_vdpa_config_ops = {
	.get		= virtio_vdpa_get,
	.set		= virtio_vdpa_set,
	.generation	= virtio_vdpa_generation,
	.get_status	= virtio_vdpa_get_status,
	.set_status	= virtio_vdpa_set_status,
	.reset		= virtio_vdpa_reset,
	.find_vqs	= virtio_vdpa_find_vqs,
	.del_vqs	= virtio_vdpa_del_vqs,
	.get_features	= virtio_vdpa_get_features,
	.finalize_features = virtio_vdpa_finalize_features,
	.bus_name	= virtio_vdpa_bus_name,
};

static void virtio_vdpa_release_dev(struct device *_d)
{
	struct virtio_device *vdev =
	       container_of(_d, struct virtio_device, dev);
	struct virtio_vdpa_device *vd_dev =
	       container_of(vdev, struct virtio_vdpa_device, vdev);

	kfree(vd_dev);
}

static int virtio_vdpa_probe(struct vdpa_device *vdpa)
{
	const struct vdpa_config_ops *ops = vdpa->config;
	struct virtio_vdpa_device *vd_dev, *reg_dev = NULL;
	int ret = -EINVAL;

	vd_dev = kzalloc(sizeof(*vd_dev), GFP_KERNEL);
	if (!vd_dev)
		return -ENOMEM;

	vd_dev->vdev.dev.parent = vdpa_get_dma_dev(vdpa);
	vd_dev->vdev.dev.release = virtio_vdpa_release_dev;
	vd_dev->vdev.config = &virtio_vdpa_config_ops;
	vd_dev->vdpa = vdpa;
	INIT_LIST_HEAD(&vd_dev->virtqueues);
	spin_lock_init(&vd_dev->lock);

	vd_dev->vdev.id.device = ops->get_device_id(vdpa);
	if (vd_dev->vdev.id.device == 0)
		goto err;

	vd_dev->vdev.id.vendor = ops->get_vendor_id(vdpa);
	ret = register_virtio_device(&vd_dev->vdev);
	reg_dev = vd_dev;
	if (ret)
		goto err;

	vdpa_set_drvdata(vdpa, vd_dev);

	return 0;

err:
	if (reg_dev)
		put_device(&vd_dev->vdev.dev);
	else
		kfree(vd_dev);
	return ret;
}

static void virtio_vdpa_remove(struct vdpa_device *vdpa)
{
	struct virtio_vdpa_device *vd_dev = vdpa_get_drvdata(vdpa);

	unregister_virtio_device(&vd_dev->vdev);
}

static struct vdpa_driver virtio_vdpa_driver = {
	.driver = {
		.name	= "virtio_vdpa",
	},
	.probe	= virtio_vdpa_probe,
	.remove = virtio_vdpa_remove,
};

module_vdpa_driver(virtio_vdpa_driver);

MODULE_VERSION(MOD_VERSION);
MODULE_LICENSE(MOD_LICENSE);
MODULE_AUTHOR(MOD_AUTHOR);
MODULE_DESCRIPTION(MOD_DESC);
