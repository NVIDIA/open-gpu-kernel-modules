// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Randomness driver for virtio
 *  Copyright (C) 2007, 2008 Rusty Russell IBM Corporation
 */

#include <linux/err.h>
#include <linux/hw_random.h>
#include <linux/scatterlist.h>
#include <linux/spinlock.h>
#include <linux/virtio.h>
#include <linux/virtio_rng.h>
#include <linux/module.h>
#include <linux/slab.h>

static DEFINE_IDA(rng_index_ida);

struct virtrng_info {
	struct hwrng hwrng;
	struct virtqueue *vq;
	struct completion have_data;
	char name[25];
	unsigned int data_avail;
	int index;
	bool busy;
	bool hwrng_register_done;
	bool hwrng_removed;
};

static void random_recv_done(struct virtqueue *vq)
{
	struct virtrng_info *vi = vq->vdev->priv;

	/* We can get spurious callbacks, e.g. shared IRQs + virtio_pci. */
	if (!virtqueue_get_buf(vi->vq, &vi->data_avail))
		return;

	complete(&vi->have_data);
}

/* The host will fill any buffer we give it with sweet, sweet randomness. */
static void register_buffer(struct virtrng_info *vi, u8 *buf, size_t size)
{
	struct scatterlist sg;

	sg_init_one(&sg, buf, size);

	/* There should always be room for one buffer. */
	virtqueue_add_inbuf(vi->vq, &sg, 1, buf, GFP_KERNEL);

	virtqueue_kick(vi->vq);
}

static int virtio_read(struct hwrng *rng, void *buf, size_t size, bool wait)
{
	int ret;
	struct virtrng_info *vi = (struct virtrng_info *)rng->priv;

	if (vi->hwrng_removed)
		return -ENODEV;

	if (!vi->busy) {
		vi->busy = true;
		reinit_completion(&vi->have_data);
		register_buffer(vi, buf, size);
	}

	if (!wait)
		return 0;

	ret = wait_for_completion_killable(&vi->have_data);
	if (ret < 0)
		return ret;

	vi->busy = false;

	return vi->data_avail;
}

static void virtio_cleanup(struct hwrng *rng)
{
	struct virtrng_info *vi = (struct virtrng_info *)rng->priv;

	if (vi->busy)
		wait_for_completion(&vi->have_data);
}

static int probe_common(struct virtio_device *vdev)
{
	int err, index;
	struct virtrng_info *vi = NULL;

	vi = kzalloc(sizeof(struct virtrng_info), GFP_KERNEL);
	if (!vi)
		return -ENOMEM;

	vi->index = index = ida_simple_get(&rng_index_ida, 0, 0, GFP_KERNEL);
	if (index < 0) {
		err = index;
		goto err_ida;
	}
	sprintf(vi->name, "virtio_rng.%d", index);
	init_completion(&vi->have_data);

	vi->hwrng = (struct hwrng) {
		.read = virtio_read,
		.cleanup = virtio_cleanup,
		.priv = (unsigned long)vi,
		.name = vi->name,
		.quality = 1000,
	};
	vdev->priv = vi;

	/* We expect a single virtqueue. */
	vi->vq = virtio_find_single_vq(vdev, random_recv_done, "input");
	if (IS_ERR(vi->vq)) {
		err = PTR_ERR(vi->vq);
		goto err_find;
	}

	return 0;

err_find:
	ida_simple_remove(&rng_index_ida, index);
err_ida:
	kfree(vi);
	return err;
}

static void remove_common(struct virtio_device *vdev)
{
	struct virtrng_info *vi = vdev->priv;

	vi->hwrng_removed = true;
	vi->data_avail = 0;
	complete(&vi->have_data);
	vdev->config->reset(vdev);
	vi->busy = false;
	if (vi->hwrng_register_done)
		hwrng_unregister(&vi->hwrng);
	vdev->config->del_vqs(vdev);
	ida_simple_remove(&rng_index_ida, vi->index);
	kfree(vi);
}

static int virtrng_probe(struct virtio_device *vdev)
{
	return probe_common(vdev);
}

static void virtrng_remove(struct virtio_device *vdev)
{
	remove_common(vdev);
}

static void virtrng_scan(struct virtio_device *vdev)
{
	struct virtrng_info *vi = vdev->priv;
	int err;

	err = hwrng_register(&vi->hwrng);
	if (!err)
		vi->hwrng_register_done = true;
}

#ifdef CONFIG_PM_SLEEP
static int virtrng_freeze(struct virtio_device *vdev)
{
	remove_common(vdev);
	return 0;
}

static int virtrng_restore(struct virtio_device *vdev)
{
	int err;

	err = probe_common(vdev);
	if (!err) {
		struct virtrng_info *vi = vdev->priv;

		/*
		 * Set hwrng_removed to ensure that virtio_read()
		 * does not block waiting for data before the
		 * registration is complete.
		 */
		vi->hwrng_removed = true;
		err = hwrng_register(&vi->hwrng);
		if (!err) {
			vi->hwrng_register_done = true;
			vi->hwrng_removed = false;
		}
	}

	return err;
}
#endif

static const struct virtio_device_id id_table[] = {
	{ VIRTIO_ID_RNG, VIRTIO_DEV_ANY_ID },
	{ 0 },
};

static struct virtio_driver virtio_rng_driver = {
	.driver.name =	KBUILD_MODNAME,
	.driver.owner =	THIS_MODULE,
	.id_table =	id_table,
	.probe =	virtrng_probe,
	.remove =	virtrng_remove,
	.scan =		virtrng_scan,
#ifdef CONFIG_PM_SLEEP
	.freeze =	virtrng_freeze,
	.restore =	virtrng_restore,
#endif
};

module_virtio_driver(virtio_rng_driver);
MODULE_DEVICE_TABLE(virtio, id_table);
MODULE_DESCRIPTION("Virtio random number driver");
MODULE_LICENSE("GPL");
