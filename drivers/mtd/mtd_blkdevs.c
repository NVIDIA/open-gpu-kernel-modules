// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Interface to Linux block layer for MTD 'translation layers'.
 *
 * Copyright © 2003-2010 David Woodhouse <dwmw2@infradead.org>
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/mtd/blktrans.h>
#include <linux/mtd/mtd.h>
#include <linux/blkdev.h>
#include <linux/blk-mq.h>
#include <linux/blkpg.h>
#include <linux/spinlock.h>
#include <linux/hdreg.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>

#include "mtdcore.h"

static LIST_HEAD(blktrans_majors);
static DEFINE_MUTEX(blktrans_ref_mutex);

static void blktrans_dev_release(struct kref *kref)
{
	struct mtd_blktrans_dev *dev =
		container_of(kref, struct mtd_blktrans_dev, ref);

	dev->disk->private_data = NULL;
	blk_cleanup_queue(dev->rq);
	blk_mq_free_tag_set(dev->tag_set);
	kfree(dev->tag_set);
	put_disk(dev->disk);
	list_del(&dev->list);
	kfree(dev);
}

static struct mtd_blktrans_dev *blktrans_dev_get(struct gendisk *disk)
{
	struct mtd_blktrans_dev *dev;

	mutex_lock(&blktrans_ref_mutex);
	dev = disk->private_data;

	if (!dev)
		goto unlock;
	kref_get(&dev->ref);
unlock:
	mutex_unlock(&blktrans_ref_mutex);
	return dev;
}

static void blktrans_dev_put(struct mtd_blktrans_dev *dev)
{
	mutex_lock(&blktrans_ref_mutex);
	kref_put(&dev->ref, blktrans_dev_release);
	mutex_unlock(&blktrans_ref_mutex);
}


static blk_status_t do_blktrans_request(struct mtd_blktrans_ops *tr,
			       struct mtd_blktrans_dev *dev,
			       struct request *req)
{
	unsigned long block, nsect;
	char *buf;

	block = blk_rq_pos(req) << 9 >> tr->blkshift;
	nsect = blk_rq_cur_bytes(req) >> tr->blkshift;

	if (req_op(req) == REQ_OP_FLUSH) {
		if (tr->flush(dev))
			return BLK_STS_IOERR;
		return BLK_STS_OK;
	}

	if (blk_rq_pos(req) + blk_rq_cur_sectors(req) >
	    get_capacity(req->rq_disk))
		return BLK_STS_IOERR;

	switch (req_op(req)) {
	case REQ_OP_DISCARD:
		if (tr->discard(dev, block, nsect))
			return BLK_STS_IOERR;
		return BLK_STS_OK;
	case REQ_OP_READ:
		buf = kmap(bio_page(req->bio)) + bio_offset(req->bio);
		for (; nsect > 0; nsect--, block++, buf += tr->blksize) {
			if (tr->readsect(dev, block, buf)) {
				kunmap(bio_page(req->bio));
				return BLK_STS_IOERR;
			}
		}
		kunmap(bio_page(req->bio));
		rq_flush_dcache_pages(req);
		return BLK_STS_OK;
	case REQ_OP_WRITE:
		if (!tr->writesect)
			return BLK_STS_IOERR;

		rq_flush_dcache_pages(req);
		buf = kmap(bio_page(req->bio)) + bio_offset(req->bio);
		for (; nsect > 0; nsect--, block++, buf += tr->blksize) {
			if (tr->writesect(dev, block, buf)) {
				kunmap(bio_page(req->bio));
				return BLK_STS_IOERR;
			}
		}
		kunmap(bio_page(req->bio));
		return BLK_STS_OK;
	default:
		return BLK_STS_IOERR;
	}
}

int mtd_blktrans_cease_background(struct mtd_blktrans_dev *dev)
{
	return dev->bg_stop;
}
EXPORT_SYMBOL_GPL(mtd_blktrans_cease_background);

static struct request *mtd_next_request(struct mtd_blktrans_dev *dev)
{
	struct request *rq;

	rq = list_first_entry_or_null(&dev->rq_list, struct request, queuelist);
	if (rq) {
		list_del_init(&rq->queuelist);
		blk_mq_start_request(rq);
		return rq;
	}

	return NULL;
}

static void mtd_blktrans_work(struct mtd_blktrans_dev *dev)
	__releases(&dev->queue_lock)
	__acquires(&dev->queue_lock)
{
	struct mtd_blktrans_ops *tr = dev->tr;
	struct request *req = NULL;
	int background_done = 0;

	while (1) {
		blk_status_t res;

		dev->bg_stop = false;
		if (!req && !(req = mtd_next_request(dev))) {
			if (tr->background && !background_done) {
				spin_unlock_irq(&dev->queue_lock);
				mutex_lock(&dev->lock);
				tr->background(dev);
				mutex_unlock(&dev->lock);
				spin_lock_irq(&dev->queue_lock);
				/*
				 * Do background processing just once per idle
				 * period.
				 */
				background_done = !dev->bg_stop;
				continue;
			}
			break;
		}

		spin_unlock_irq(&dev->queue_lock);

		mutex_lock(&dev->lock);
		res = do_blktrans_request(dev->tr, dev, req);
		mutex_unlock(&dev->lock);

		if (!blk_update_request(req, res, blk_rq_cur_bytes(req))) {
			__blk_mq_end_request(req, res);
			req = NULL;
		}

		background_done = 0;
		spin_lock_irq(&dev->queue_lock);
	}
}

static blk_status_t mtd_queue_rq(struct blk_mq_hw_ctx *hctx,
				 const struct blk_mq_queue_data *bd)
{
	struct mtd_blktrans_dev *dev;

	dev = hctx->queue->queuedata;
	if (!dev) {
		blk_mq_start_request(bd->rq);
		return BLK_STS_IOERR;
	}

	spin_lock_irq(&dev->queue_lock);
	list_add_tail(&bd->rq->queuelist, &dev->rq_list);
	mtd_blktrans_work(dev);
	spin_unlock_irq(&dev->queue_lock);

	return BLK_STS_OK;
}

static int blktrans_open(struct block_device *bdev, fmode_t mode)
{
	struct mtd_blktrans_dev *dev = blktrans_dev_get(bdev->bd_disk);
	int ret = 0;

	if (!dev)
		return -ERESTARTSYS; /* FIXME: busy loop! -arnd*/

	mutex_lock(&mtd_table_mutex);
	mutex_lock(&dev->lock);

	if (dev->open)
		goto unlock;

	kref_get(&dev->ref);
	__module_get(dev->tr->owner);

	if (!dev->mtd)
		goto unlock;

	if (dev->tr->open) {
		ret = dev->tr->open(dev);
		if (ret)
			goto error_put;
	}

	ret = __get_mtd_device(dev->mtd);
	if (ret)
		goto error_release;
	dev->file_mode = mode;

unlock:
	dev->open++;
	mutex_unlock(&dev->lock);
	mutex_unlock(&mtd_table_mutex);
	blktrans_dev_put(dev);
	return ret;

error_release:
	if (dev->tr->release)
		dev->tr->release(dev);
error_put:
	module_put(dev->tr->owner);
	kref_put(&dev->ref, blktrans_dev_release);
	mutex_unlock(&dev->lock);
	mutex_unlock(&mtd_table_mutex);
	blktrans_dev_put(dev);
	return ret;
}

static void blktrans_release(struct gendisk *disk, fmode_t mode)
{
	struct mtd_blktrans_dev *dev = blktrans_dev_get(disk);

	if (!dev)
		return;

	mutex_lock(&mtd_table_mutex);
	mutex_lock(&dev->lock);

	if (--dev->open)
		goto unlock;

	kref_put(&dev->ref, blktrans_dev_release);
	module_put(dev->tr->owner);

	if (dev->mtd) {
		if (dev->tr->release)
			dev->tr->release(dev);
		__put_mtd_device(dev->mtd);
	}
unlock:
	mutex_unlock(&dev->lock);
	mutex_unlock(&mtd_table_mutex);
	blktrans_dev_put(dev);
}

static int blktrans_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
	struct mtd_blktrans_dev *dev = blktrans_dev_get(bdev->bd_disk);
	int ret = -ENXIO;

	if (!dev)
		return ret;

	mutex_lock(&dev->lock);

	if (!dev->mtd)
		goto unlock;

	ret = dev->tr->getgeo ? dev->tr->getgeo(dev, geo) : -ENOTTY;
unlock:
	mutex_unlock(&dev->lock);
	blktrans_dev_put(dev);
	return ret;
}

static const struct block_device_operations mtd_block_ops = {
	.owner		= THIS_MODULE,
	.open		= blktrans_open,
	.release	= blktrans_release,
	.getgeo		= blktrans_getgeo,
};

static const struct blk_mq_ops mtd_mq_ops = {
	.queue_rq	= mtd_queue_rq,
};

int add_mtd_blktrans_dev(struct mtd_blktrans_dev *new)
{
	struct mtd_blktrans_ops *tr = new->tr;
	struct mtd_blktrans_dev *d;
	int last_devnum = -1;
	struct gendisk *gd;
	int ret;

	if (mutex_trylock(&mtd_table_mutex)) {
		mutex_unlock(&mtd_table_mutex);
		BUG();
	}

	mutex_lock(&blktrans_ref_mutex);
	list_for_each_entry(d, &tr->devs, list) {
		if (new->devnum == -1) {
			/* Use first free number */
			if (d->devnum != last_devnum+1) {
				/* Found a free devnum. Plug it in here */
				new->devnum = last_devnum+1;
				list_add_tail(&new->list, &d->list);
				goto added;
			}
		} else if (d->devnum == new->devnum) {
			/* Required number taken */
			mutex_unlock(&blktrans_ref_mutex);
			return -EBUSY;
		} else if (d->devnum > new->devnum) {
			/* Required number was free */
			list_add_tail(&new->list, &d->list);
			goto added;
		}
		last_devnum = d->devnum;
	}

	ret = -EBUSY;
	if (new->devnum == -1)
		new->devnum = last_devnum+1;

	/* Check that the device and any partitions will get valid
	 * minor numbers and that the disk naming code below can cope
	 * with this number. */
	if (new->devnum > (MINORMASK >> tr->part_bits) ||
	    (tr->part_bits && new->devnum >= 27 * 26)) {
		mutex_unlock(&blktrans_ref_mutex);
		goto error1;
	}

	list_add_tail(&new->list, &tr->devs);
 added:
	mutex_unlock(&blktrans_ref_mutex);

	mutex_init(&new->lock);
	kref_init(&new->ref);
	if (!tr->writesect)
		new->readonly = 1;

	/* Create gendisk */
	ret = -ENOMEM;
	gd = alloc_disk(1 << tr->part_bits);

	if (!gd)
		goto error2;

	new->disk = gd;
	gd->private_data = new;
	gd->major = tr->major;
	gd->first_minor = (new->devnum) << tr->part_bits;
	gd->fops = &mtd_block_ops;

	if (tr->part_bits)
		if (new->devnum < 26)
			snprintf(gd->disk_name, sizeof(gd->disk_name),
				 "%s%c", tr->name, 'a' + new->devnum);
		else
			snprintf(gd->disk_name, sizeof(gd->disk_name),
				 "%s%c%c", tr->name,
				 'a' - 1 + new->devnum / 26,
				 'a' + new->devnum % 26);
	else
		snprintf(gd->disk_name, sizeof(gd->disk_name),
			 "%s%d", tr->name, new->devnum);

	set_capacity(gd, ((u64)new->size * tr->blksize) >> 9);

	/* Create the request queue */
	spin_lock_init(&new->queue_lock);
	INIT_LIST_HEAD(&new->rq_list);

	new->tag_set = kzalloc(sizeof(*new->tag_set), GFP_KERNEL);
	if (!new->tag_set)
		goto error3;

	new->rq = blk_mq_init_sq_queue(new->tag_set, &mtd_mq_ops, 2,
				BLK_MQ_F_SHOULD_MERGE | BLK_MQ_F_BLOCKING);
	if (IS_ERR(new->rq)) {
		ret = PTR_ERR(new->rq);
		new->rq = NULL;
		goto error4;
	}

	if (tr->flush)
		blk_queue_write_cache(new->rq, true, false);

	new->rq->queuedata = new;
	blk_queue_logical_block_size(new->rq, tr->blksize);

	blk_queue_flag_set(QUEUE_FLAG_NONROT, new->rq);
	blk_queue_flag_clear(QUEUE_FLAG_ADD_RANDOM, new->rq);

	if (tr->discard) {
		blk_queue_flag_set(QUEUE_FLAG_DISCARD, new->rq);
		blk_queue_max_discard_sectors(new->rq, UINT_MAX);
	}

	gd->queue = new->rq;

	if (new->readonly)
		set_disk_ro(gd, 1);

	device_add_disk(&new->mtd->dev, gd, NULL);

	if (new->disk_attributes) {
		ret = sysfs_create_group(&disk_to_dev(gd)->kobj,
					new->disk_attributes);
		WARN_ON(ret);
	}
	return 0;
error4:
	kfree(new->tag_set);
error3:
	put_disk(new->disk);
error2:
	list_del(&new->list);
error1:
	return ret;
}

int del_mtd_blktrans_dev(struct mtd_blktrans_dev *old)
{
	unsigned long flags;

	if (mutex_trylock(&mtd_table_mutex)) {
		mutex_unlock(&mtd_table_mutex);
		BUG();
	}

	if (old->disk_attributes)
		sysfs_remove_group(&disk_to_dev(old->disk)->kobj,
						old->disk_attributes);

	/* Stop new requests to arrive */
	del_gendisk(old->disk);

	/* Kill current requests */
	spin_lock_irqsave(&old->queue_lock, flags);
	old->rq->queuedata = NULL;
	spin_unlock_irqrestore(&old->queue_lock, flags);

	/* freeze+quiesce queue to ensure all requests are flushed */
	blk_mq_freeze_queue(old->rq);
	blk_mq_quiesce_queue(old->rq);
	blk_mq_unquiesce_queue(old->rq);
	blk_mq_unfreeze_queue(old->rq);

	/* If the device is currently open, tell trans driver to close it,
		then put mtd device, and don't touch it again */
	mutex_lock(&old->lock);
	if (old->open) {
		if (old->tr->release)
			old->tr->release(old);
		__put_mtd_device(old->mtd);
	}

	old->mtd = NULL;

	mutex_unlock(&old->lock);
	blktrans_dev_put(old);
	return 0;
}

static void blktrans_notify_remove(struct mtd_info *mtd)
{
	struct mtd_blktrans_ops *tr;
	struct mtd_blktrans_dev *dev, *next;

	list_for_each_entry(tr, &blktrans_majors, list)
		list_for_each_entry_safe(dev, next, &tr->devs, list)
			if (dev->mtd == mtd)
				tr->remove_dev(dev);
}

static void blktrans_notify_add(struct mtd_info *mtd)
{
	struct mtd_blktrans_ops *tr;

	if (mtd->type == MTD_ABSENT)
		return;

	list_for_each_entry(tr, &blktrans_majors, list)
		tr->add_mtd(tr, mtd);
}

static struct mtd_notifier blktrans_notifier = {
	.add = blktrans_notify_add,
	.remove = blktrans_notify_remove,
};

int register_mtd_blktrans(struct mtd_blktrans_ops *tr)
{
	struct mtd_info *mtd;
	int ret;

	/* Register the notifier if/when the first device type is
	   registered, to prevent the link/init ordering from fucking
	   us over. */
	if (!blktrans_notifier.list.next)
		register_mtd_user(&blktrans_notifier);


	mutex_lock(&mtd_table_mutex);

	ret = register_blkdev(tr->major, tr->name);
	if (ret < 0) {
		printk(KERN_WARNING "Unable to register %s block device on major %d: %d\n",
		       tr->name, tr->major, ret);
		mutex_unlock(&mtd_table_mutex);
		return ret;
	}

	if (ret)
		tr->major = ret;

	tr->blkshift = ffs(tr->blksize) - 1;

	INIT_LIST_HEAD(&tr->devs);
	list_add(&tr->list, &blktrans_majors);

	mtd_for_each_device(mtd)
		if (mtd->type != MTD_ABSENT)
			tr->add_mtd(tr, mtd);

	mutex_unlock(&mtd_table_mutex);
	return 0;
}

int deregister_mtd_blktrans(struct mtd_blktrans_ops *tr)
{
	struct mtd_blktrans_dev *dev, *next;

	mutex_lock(&mtd_table_mutex);

	/* Remove it from the list of active majors */
	list_del(&tr->list);

	list_for_each_entry_safe(dev, next, &tr->devs, list)
		tr->remove_dev(dev);

	unregister_blkdev(tr->major, tr->name);
	mutex_unlock(&mtd_table_mutex);

	BUG_ON(!list_empty(&tr->devs));
	return 0;
}

static void __exit mtd_blktrans_exit(void)
{
	/* No race here -- if someone's currently in register_mtd_blktrans
	   we're screwed anyway. */
	if (blktrans_notifier.list.next)
		unregister_mtd_user(&blktrans_notifier);
}

module_exit(mtd_blktrans_exit);

EXPORT_SYMBOL_GPL(register_mtd_blktrans);
EXPORT_SYMBOL_GPL(deregister_mtd_blktrans);
EXPORT_SYMBOL_GPL(add_mtd_blktrans_dev);
EXPORT_SYMBOL_GPL(del_mtd_blktrans_dev);

MODULE_AUTHOR("David Woodhouse <dwmw2@infradead.org>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Common interface to block layer for MTD 'translation layers'");
