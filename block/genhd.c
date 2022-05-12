// SPDX-License-Identifier: GPL-2.0
/*
 *  gendisk handling
 *
 * Portions Copyright (C) 2020 Christoph Hellwig
 */

#include <linux/module.h>
#include <linux/ctype.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/blkdev.h>
#include <linux/backing-dev.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/kmod.h>
#include <linux/mutex.h>
#include <linux/idr.h>
#include <linux/log2.h>
#include <linux/pm_runtime.h>
#include <linux/badblocks.h>

#include "blk.h"

static struct kobject *block_depr;

/* for extended dynamic devt allocation, currently only one major is used */
#define NR_EXT_DEVT		(1 << MINORBITS)
static DEFINE_IDA(ext_devt_ida);

static void disk_check_events(struct disk_events *ev,
			      unsigned int *clearing_ptr);
static void disk_alloc_events(struct gendisk *disk);
static void disk_add_events(struct gendisk *disk);
static void disk_del_events(struct gendisk *disk);
static void disk_release_events(struct gendisk *disk);

void set_capacity(struct gendisk *disk, sector_t sectors)
{
	struct block_device *bdev = disk->part0;

	spin_lock(&bdev->bd_size_lock);
	i_size_write(bdev->bd_inode, (loff_t)sectors << SECTOR_SHIFT);
	spin_unlock(&bdev->bd_size_lock);
}
EXPORT_SYMBOL(set_capacity);

/*
 * Set disk capacity and notify if the size is not currently zero and will not
 * be set to zero.  Returns true if a uevent was sent, otherwise false.
 */
bool set_capacity_and_notify(struct gendisk *disk, sector_t size)
{
	sector_t capacity = get_capacity(disk);
	char *envp[] = { "RESIZE=1", NULL };

	set_capacity(disk, size);

	/*
	 * Only print a message and send a uevent if the gendisk is user visible
	 * and alive.  This avoids spamming the log and udev when setting the
	 * initial capacity during probing.
	 */
	if (size == capacity ||
	    (disk->flags & (GENHD_FL_UP | GENHD_FL_HIDDEN)) != GENHD_FL_UP)
		return false;

	pr_info("%s: detected capacity change from %lld to %lld\n",
		disk->disk_name, capacity, size);

	/*
	 * Historically we did not send a uevent for changes to/from an empty
	 * device.
	 */
	if (!capacity || !size)
		return false;
	kobject_uevent_env(&disk_to_dev(disk)->kobj, KOBJ_CHANGE, envp);
	return true;
}
EXPORT_SYMBOL_GPL(set_capacity_and_notify);

/*
 * Format the device name of the indicated disk into the supplied buffer and
 * return a pointer to that same buffer for convenience.
 */
char *disk_name(struct gendisk *hd, int partno, char *buf)
{
	if (!partno)
		snprintf(buf, BDEVNAME_SIZE, "%s", hd->disk_name);
	else if (isdigit(hd->disk_name[strlen(hd->disk_name)-1]))
		snprintf(buf, BDEVNAME_SIZE, "%sp%d", hd->disk_name, partno);
	else
		snprintf(buf, BDEVNAME_SIZE, "%s%d", hd->disk_name, partno);

	return buf;
}

const char *bdevname(struct block_device *bdev, char *buf)
{
	return disk_name(bdev->bd_disk, bdev->bd_partno, buf);
}
EXPORT_SYMBOL(bdevname);

static void part_stat_read_all(struct block_device *part,
		struct disk_stats *stat)
{
	int cpu;

	memset(stat, 0, sizeof(struct disk_stats));
	for_each_possible_cpu(cpu) {
		struct disk_stats *ptr = per_cpu_ptr(part->bd_stats, cpu);
		int group;

		for (group = 0; group < NR_STAT_GROUPS; group++) {
			stat->nsecs[group] += ptr->nsecs[group];
			stat->sectors[group] += ptr->sectors[group];
			stat->ios[group] += ptr->ios[group];
			stat->merges[group] += ptr->merges[group];
		}

		stat->io_ticks += ptr->io_ticks;
	}
}

static unsigned int part_in_flight(struct block_device *part)
{
	unsigned int inflight = 0;
	int cpu;

	for_each_possible_cpu(cpu) {
		inflight += part_stat_local_read_cpu(part, in_flight[0], cpu) +
			    part_stat_local_read_cpu(part, in_flight[1], cpu);
	}
	if ((int)inflight < 0)
		inflight = 0;

	return inflight;
}

static void part_in_flight_rw(struct block_device *part,
		unsigned int inflight[2])
{
	int cpu;

	inflight[0] = 0;
	inflight[1] = 0;
	for_each_possible_cpu(cpu) {
		inflight[0] += part_stat_local_read_cpu(part, in_flight[0], cpu);
		inflight[1] += part_stat_local_read_cpu(part, in_flight[1], cpu);
	}
	if ((int)inflight[0] < 0)
		inflight[0] = 0;
	if ((int)inflight[1] < 0)
		inflight[1] = 0;
}

/*
 * Can be deleted altogether. Later.
 *
 */
#define BLKDEV_MAJOR_HASH_SIZE 255
static struct blk_major_name {
	struct blk_major_name *next;
	int major;
	char name[16];
	void (*probe)(dev_t devt);
} *major_names[BLKDEV_MAJOR_HASH_SIZE];
static DEFINE_MUTEX(major_names_lock);

/* index in the above - for now: assume no multimajor ranges */
static inline int major_to_index(unsigned major)
{
	return major % BLKDEV_MAJOR_HASH_SIZE;
}

#ifdef CONFIG_PROC_FS
void blkdev_show(struct seq_file *seqf, off_t offset)
{
	struct blk_major_name *dp;

	mutex_lock(&major_names_lock);
	for (dp = major_names[major_to_index(offset)]; dp; dp = dp->next)
		if (dp->major == offset)
			seq_printf(seqf, "%3d %s\n", dp->major, dp->name);
	mutex_unlock(&major_names_lock);
}
#endif /* CONFIG_PROC_FS */

/**
 * __register_blkdev - register a new block device
 *
 * @major: the requested major device number [1..BLKDEV_MAJOR_MAX-1]. If
 *         @major = 0, try to allocate any unused major number.
 * @name: the name of the new block device as a zero terminated string
 * @probe: allback that is called on access to any minor number of @major
 *
 * The @name must be unique within the system.
 *
 * The return value depends on the @major input parameter:
 *
 *  - if a major device number was requested in range [1..BLKDEV_MAJOR_MAX-1]
 *    then the function returns zero on success, or a negative error code
 *  - if any unused major number was requested with @major = 0 parameter
 *    then the return value is the allocated major number in range
 *    [1..BLKDEV_MAJOR_MAX-1] or a negative error code otherwise
 *
 * See Documentation/admin-guide/devices.txt for the list of allocated
 * major numbers.
 *
 * Use register_blkdev instead for any new code.
 */
int __register_blkdev(unsigned int major, const char *name,
		void (*probe)(dev_t devt))
{
	struct blk_major_name **n, *p;
	int index, ret = 0;

	mutex_lock(&major_names_lock);

	/* temporary */
	if (major == 0) {
		for (index = ARRAY_SIZE(major_names)-1; index > 0; index--) {
			if (major_names[index] == NULL)
				break;
		}

		if (index == 0) {
			printk("%s: failed to get major for %s\n",
			       __func__, name);
			ret = -EBUSY;
			goto out;
		}
		major = index;
		ret = major;
	}

	if (major >= BLKDEV_MAJOR_MAX) {
		pr_err("%s: major requested (%u) is greater than the maximum (%u) for %s\n",
		       __func__, major, BLKDEV_MAJOR_MAX-1, name);

		ret = -EINVAL;
		goto out;
	}

	p = kmalloc(sizeof(struct blk_major_name), GFP_KERNEL);
	if (p == NULL) {
		ret = -ENOMEM;
		goto out;
	}

	p->major = major;
	p->probe = probe;
	strlcpy(p->name, name, sizeof(p->name));
	p->next = NULL;
	index = major_to_index(major);

	for (n = &major_names[index]; *n; n = &(*n)->next) {
		if ((*n)->major == major)
			break;
	}
	if (!*n)
		*n = p;
	else
		ret = -EBUSY;

	if (ret < 0) {
		printk("register_blkdev: cannot get major %u for %s\n",
		       major, name);
		kfree(p);
	}
out:
	mutex_unlock(&major_names_lock);
	return ret;
}
EXPORT_SYMBOL(__register_blkdev);

void unregister_blkdev(unsigned int major, const char *name)
{
	struct blk_major_name **n;
	struct blk_major_name *p = NULL;
	int index = major_to_index(major);

	mutex_lock(&major_names_lock);
	for (n = &major_names[index]; *n; n = &(*n)->next)
		if ((*n)->major == major)
			break;
	if (!*n || strcmp((*n)->name, name)) {
		WARN_ON(1);
	} else {
		p = *n;
		*n = p->next;
	}
	mutex_unlock(&major_names_lock);
	kfree(p);
}

EXPORT_SYMBOL(unregister_blkdev);

/**
 * blk_mangle_minor - scatter minor numbers apart
 * @minor: minor number to mangle
 *
 * Scatter consecutively allocated @minor number apart if MANGLE_DEVT
 * is enabled.  Mangling twice gives the original value.
 *
 * RETURNS:
 * Mangled value.
 *
 * CONTEXT:
 * Don't care.
 */
static int blk_mangle_minor(int minor)
{
#ifdef CONFIG_DEBUG_BLOCK_EXT_DEVT
	int i;

	for (i = 0; i < MINORBITS / 2; i++) {
		int low = minor & (1 << i);
		int high = minor & (1 << (MINORBITS - 1 - i));
		int distance = MINORBITS - 1 - 2 * i;

		minor ^= low | high;	/* clear both bits */
		low <<= distance;	/* swap the positions */
		high >>= distance;
		minor |= low | high;	/* and set */
	}
#endif
	return minor;
}

/**
 * blk_alloc_devt - allocate a dev_t for a block device
 * @bdev: block device to allocate dev_t for
 * @devt: out parameter for resulting dev_t
 *
 * Allocate a dev_t for block device.
 *
 * RETURNS:
 * 0 on success, allocated dev_t is returned in *@devt.  -errno on
 * failure.
 *
 * CONTEXT:
 * Might sleep.
 */
int blk_alloc_devt(struct block_device *bdev, dev_t *devt)
{
	struct gendisk *disk = bdev->bd_disk;
	int idx;

	/* in consecutive minor range? */
	if (bdev->bd_partno < disk->minors) {
		*devt = MKDEV(disk->major, disk->first_minor + bdev->bd_partno);
		return 0;
	}

	idx = ida_alloc_range(&ext_devt_ida, 0, NR_EXT_DEVT, GFP_KERNEL);
	if (idx < 0)
		return idx == -ENOSPC ? -EBUSY : idx;

	*devt = MKDEV(BLOCK_EXT_MAJOR, blk_mangle_minor(idx));
	return 0;
}

/**
 * blk_free_devt - free a dev_t
 * @devt: dev_t to free
 *
 * Free @devt which was allocated using blk_alloc_devt().
 *
 * CONTEXT:
 * Might sleep.
 */
void blk_free_devt(dev_t devt)
{
	if (MAJOR(devt) == BLOCK_EXT_MAJOR)
		ida_free(&ext_devt_ida, blk_mangle_minor(MINOR(devt)));
}

static char *bdevt_str(dev_t devt, char *buf)
{
	if (MAJOR(devt) <= 0xff && MINOR(devt) <= 0xff) {
		char tbuf[BDEVT_SIZE];
		snprintf(tbuf, BDEVT_SIZE, "%02x%02x", MAJOR(devt), MINOR(devt));
		snprintf(buf, BDEVT_SIZE, "%-9s", tbuf);
	} else
		snprintf(buf, BDEVT_SIZE, "%03x:%05x", MAJOR(devt), MINOR(devt));

	return buf;
}

void disk_uevent(struct gendisk *disk, enum kobject_action action)
{
	struct block_device *part;
	unsigned long idx;

	rcu_read_lock();
	xa_for_each(&disk->part_tbl, idx, part) {
		if (bdev_is_partition(part) && !bdev_nr_sectors(part))
			continue;
		if (!bdgrab(part))
			continue;

		rcu_read_unlock();
		kobject_uevent(bdev_kobj(part), action);
		bdput(part);
		rcu_read_lock();
	}
	rcu_read_unlock();
}
EXPORT_SYMBOL_GPL(disk_uevent);

static void disk_scan_partitions(struct gendisk *disk)
{
	struct block_device *bdev;

	if (!get_capacity(disk) || !disk_part_scan_enabled(disk))
		return;

	set_bit(GD_NEED_PART_SCAN, &disk->state);
	bdev = blkdev_get_by_dev(disk_devt(disk), FMODE_READ, NULL);
	if (!IS_ERR(bdev))
		blkdev_put(bdev, FMODE_READ);
}

static void register_disk(struct device *parent, struct gendisk *disk,
			  const struct attribute_group **groups)
{
	struct device *ddev = disk_to_dev(disk);
	int err;

	ddev->parent = parent;

	dev_set_name(ddev, "%s", disk->disk_name);

	/* delay uevents, until we scanned partition table */
	dev_set_uevent_suppress(ddev, 1);

	if (groups) {
		WARN_ON(ddev->groups);
		ddev->groups = groups;
	}
	if (device_add(ddev))
		return;
	if (!sysfs_deprecated) {
		err = sysfs_create_link(block_depr, &ddev->kobj,
					kobject_name(&ddev->kobj));
		if (err) {
			device_del(ddev);
			return;
		}
	}

	/*
	 * avoid probable deadlock caused by allocating memory with
	 * GFP_KERNEL in runtime_resume callback of its all ancestor
	 * devices
	 */
	pm_runtime_set_memalloc_noio(ddev, true);

	disk->part0->bd_holder_dir =
		kobject_create_and_add("holders", &ddev->kobj);
	disk->slave_dir = kobject_create_and_add("slaves", &ddev->kobj);

	if (disk->flags & GENHD_FL_HIDDEN)
		return;

	disk_scan_partitions(disk);

	/* announce the disk and partitions after all partitions are created */
	dev_set_uevent_suppress(ddev, 0);
	disk_uevent(disk, KOBJ_ADD);

	if (disk->queue->backing_dev_info->dev) {
		err = sysfs_create_link(&ddev->kobj,
			  &disk->queue->backing_dev_info->dev->kobj,
			  "bdi");
		WARN_ON(err);
	}
}

/**
 * __device_add_disk - add disk information to kernel list
 * @parent: parent device for the disk
 * @disk: per-device partitioning information
 * @groups: Additional per-device sysfs groups
 * @register_queue: register the queue if set to true
 *
 * This function registers the partitioning information in @disk
 * with the kernel.
 *
 * FIXME: error handling
 */
static void __device_add_disk(struct device *parent, struct gendisk *disk,
			      const struct attribute_group **groups,
			      bool register_queue)
{
	dev_t devt;
	int retval;

	/*
	 * The disk queue should now be all set with enough information about
	 * the device for the elevator code to pick an adequate default
	 * elevator if one is needed, that is, for devices requesting queue
	 * registration.
	 */
	if (register_queue)
		elevator_init_mq(disk->queue);

	/* minors == 0 indicates to use ext devt from part0 and should
	 * be accompanied with EXT_DEVT flag.  Make sure all
	 * parameters make sense.
	 */
	WARN_ON(disk->minors && !(disk->major || disk->first_minor));
	WARN_ON(!disk->minors &&
		!(disk->flags & (GENHD_FL_EXT_DEVT | GENHD_FL_HIDDEN)));

	disk->flags |= GENHD_FL_UP;

	retval = blk_alloc_devt(disk->part0, &devt);
	if (retval) {
		WARN_ON(1);
		return;
	}
	disk->major = MAJOR(devt);
	disk->first_minor = MINOR(devt);

	disk_alloc_events(disk);

	if (disk->flags & GENHD_FL_HIDDEN) {
		/*
		 * Don't let hidden disks show up in /proc/partitions,
		 * and don't bother scanning for partitions either.
		 */
		disk->flags |= GENHD_FL_SUPPRESS_PARTITION_INFO;
		disk->flags |= GENHD_FL_NO_PART_SCAN;
	} else {
		struct backing_dev_info *bdi = disk->queue->backing_dev_info;
		struct device *dev = disk_to_dev(disk);
		int ret;

		/* Register BDI before referencing it from bdev */
		dev->devt = devt;
		ret = bdi_register(bdi, "%u:%u", MAJOR(devt), MINOR(devt));
		WARN_ON(ret);
		bdi_set_owner(bdi, dev);
		bdev_add(disk->part0, devt);
	}
	register_disk(parent, disk, groups);
	if (register_queue)
		blk_register_queue(disk);

	/*
	 * Take an extra ref on queue which will be put on disk_release()
	 * so that it sticks around as long as @disk is there.
	 */
	WARN_ON_ONCE(!blk_get_queue(disk->queue));

	disk_add_events(disk);
	blk_integrity_add(disk);
}

void device_add_disk(struct device *parent, struct gendisk *disk,
		     const struct attribute_group **groups)

{
	__device_add_disk(parent, disk, groups, true);
}
EXPORT_SYMBOL(device_add_disk);

void device_add_disk_no_queue_reg(struct device *parent, struct gendisk *disk)
{
	__device_add_disk(parent, disk, NULL, false);
}
EXPORT_SYMBOL(device_add_disk_no_queue_reg);

/**
 * del_gendisk - remove the gendisk
 * @disk: the struct gendisk to remove
 *
 * Removes the gendisk and all its associated resources. This deletes the
 * partitions associated with the gendisk, and unregisters the associated
 * request_queue.
 *
 * This is the counter to the respective __device_add_disk() call.
 *
 * The final removal of the struct gendisk happens when its refcount reaches 0
 * with put_disk(), which should be called after del_gendisk(), if
 * __device_add_disk() was used.
 *
 * Drivers exist which depend on the release of the gendisk to be synchronous,
 * it should not be deferred.
 *
 * Context: can sleep
 */
void del_gendisk(struct gendisk *disk)
{
	might_sleep();

	if (WARN_ON_ONCE(!disk->queue))
		return;

	blk_integrity_del(disk);
	disk_del_events(disk);

	mutex_lock(&disk->part0->bd_mutex);
	disk->flags &= ~GENHD_FL_UP;
	blk_drop_partitions(disk);
	mutex_unlock(&disk->part0->bd_mutex);

	fsync_bdev(disk->part0);
	__invalidate_device(disk->part0, true);

	/*
	 * Unhash the bdev inode for this device so that it can't be looked
	 * up any more even if openers still hold references to it.
	 */
	remove_inode_hash(disk->part0->bd_inode);

	set_capacity(disk, 0);

	if (!(disk->flags & GENHD_FL_HIDDEN)) {
		sysfs_remove_link(&disk_to_dev(disk)->kobj, "bdi");

		/*
		 * Unregister bdi before releasing device numbers (as they can
		 * get reused and we'd get clashes in sysfs).
		 */
		bdi_unregister(disk->queue->backing_dev_info);
	}

	blk_unregister_queue(disk);

	kobject_put(disk->part0->bd_holder_dir);
	kobject_put(disk->slave_dir);

	part_stat_set_all(disk->part0, 0);
	disk->part0->bd_stamp = 0;
	if (!sysfs_deprecated)
		sysfs_remove_link(block_depr, dev_name(disk_to_dev(disk)));
	pm_runtime_set_memalloc_noio(disk_to_dev(disk), false);
	device_del(disk_to_dev(disk));
}
EXPORT_SYMBOL(del_gendisk);

/* sysfs access to bad-blocks list. */
static ssize_t disk_badblocks_show(struct device *dev,
					struct device_attribute *attr,
					char *page)
{
	struct gendisk *disk = dev_to_disk(dev);

	if (!disk->bb)
		return sprintf(page, "\n");

	return badblocks_show(disk->bb, page, 0);
}

static ssize_t disk_badblocks_store(struct device *dev,
					struct device_attribute *attr,
					const char *page, size_t len)
{
	struct gendisk *disk = dev_to_disk(dev);

	if (!disk->bb)
		return -ENXIO;

	return badblocks_store(disk->bb, page, len, 0);
}

void blk_request_module(dev_t devt)
{
	unsigned int major = MAJOR(devt);
	struct blk_major_name **n;

	mutex_lock(&major_names_lock);
	for (n = &major_names[major_to_index(major)]; *n; n = &(*n)->next) {
		if ((*n)->major == major && (*n)->probe) {
			(*n)->probe(devt);
			mutex_unlock(&major_names_lock);
			return;
		}
	}
	mutex_unlock(&major_names_lock);

	if (request_module("block-major-%d-%d", MAJOR(devt), MINOR(devt)) > 0)
		/* Make old-style 2.4 aliases work */
		request_module("block-major-%d", MAJOR(devt));
}

/**
 * bdget_disk - do bdget() by gendisk and partition number
 * @disk: gendisk of interest
 * @partno: partition number
 *
 * Find partition @partno from @disk, do bdget() on it.
 *
 * CONTEXT:
 * Don't care.
 *
 * RETURNS:
 * Resulting block_device on success, NULL on failure.
 */
struct block_device *bdget_disk(struct gendisk *disk, int partno)
{
	struct block_device *bdev = NULL;

	rcu_read_lock();
	bdev = xa_load(&disk->part_tbl, partno);
	if (bdev && !bdgrab(bdev))
		bdev = NULL;
	rcu_read_unlock();

	return bdev;
}

/*
 * print a full list of all partitions - intended for places where the root
 * filesystem can't be mounted and thus to give the victim some idea of what
 * went wrong
 */
void __init printk_all_partitions(void)
{
	struct class_dev_iter iter;
	struct device *dev;

	class_dev_iter_init(&iter, &block_class, NULL, &disk_type);
	while ((dev = class_dev_iter_next(&iter))) {
		struct gendisk *disk = dev_to_disk(dev);
		struct block_device *part;
		char name_buf[BDEVNAME_SIZE];
		char devt_buf[BDEVT_SIZE];
		unsigned long idx;

		/*
		 * Don't show empty devices or things that have been
		 * suppressed
		 */
		if (get_capacity(disk) == 0 ||
		    (disk->flags & GENHD_FL_SUPPRESS_PARTITION_INFO))
			continue;

		/*
		 * Note, unlike /proc/partitions, I am showing the numbers in
		 * hex - the same format as the root= option takes.
		 */
		rcu_read_lock();
		xa_for_each(&disk->part_tbl, idx, part) {
			if (!bdev_nr_sectors(part))
				continue;
			printk("%s%s %10llu %s %s",
			       bdev_is_partition(part) ? "  " : "",
			       bdevt_str(part->bd_dev, devt_buf),
			       bdev_nr_sectors(part) >> 1,
			       disk_name(disk, part->bd_partno, name_buf),
			       part->bd_meta_info ?
					part->bd_meta_info->uuid : "");
			if (bdev_is_partition(part))
				printk("\n");
			else if (dev->parent && dev->parent->driver)
				printk(" driver: %s\n",
					dev->parent->driver->name);
			else
				printk(" (driver?)\n");
		}
		rcu_read_unlock();
	}
	class_dev_iter_exit(&iter);
}

#ifdef CONFIG_PROC_FS
/* iterator */
static void *disk_seqf_start(struct seq_file *seqf, loff_t *pos)
{
	loff_t skip = *pos;
	struct class_dev_iter *iter;
	struct device *dev;

	iter = kmalloc(sizeof(*iter), GFP_KERNEL);
	if (!iter)
		return ERR_PTR(-ENOMEM);

	seqf->private = iter;
	class_dev_iter_init(iter, &block_class, NULL, &disk_type);
	do {
		dev = class_dev_iter_next(iter);
		if (!dev)
			return NULL;
	} while (skip--);

	return dev_to_disk(dev);
}

static void *disk_seqf_next(struct seq_file *seqf, void *v, loff_t *pos)
{
	struct device *dev;

	(*pos)++;
	dev = class_dev_iter_next(seqf->private);
	if (dev)
		return dev_to_disk(dev);

	return NULL;
}

static void disk_seqf_stop(struct seq_file *seqf, void *v)
{
	struct class_dev_iter *iter = seqf->private;

	/* stop is called even after start failed :-( */
	if (iter) {
		class_dev_iter_exit(iter);
		kfree(iter);
		seqf->private = NULL;
	}
}

static void *show_partition_start(struct seq_file *seqf, loff_t *pos)
{
	void *p;

	p = disk_seqf_start(seqf, pos);
	if (!IS_ERR_OR_NULL(p) && !*pos)
		seq_puts(seqf, "major minor  #blocks  name\n\n");
	return p;
}

static int show_partition(struct seq_file *seqf, void *v)
{
	struct gendisk *sgp = v;
	struct block_device *part;
	unsigned long idx;
	char buf[BDEVNAME_SIZE];

	/* Don't show non-partitionable removeable devices or empty devices */
	if (!get_capacity(sgp) || (!disk_max_parts(sgp) &&
				   (sgp->flags & GENHD_FL_REMOVABLE)))
		return 0;
	if (sgp->flags & GENHD_FL_SUPPRESS_PARTITION_INFO)
		return 0;

	rcu_read_lock();
	xa_for_each(&sgp->part_tbl, idx, part) {
		if (!bdev_nr_sectors(part))
			continue;
		seq_printf(seqf, "%4d  %7d %10llu %s\n",
			   MAJOR(part->bd_dev), MINOR(part->bd_dev),
			   bdev_nr_sectors(part) >> 1,
			   disk_name(sgp, part->bd_partno, buf));
	}
	rcu_read_unlock();
	return 0;
}

static const struct seq_operations partitions_op = {
	.start	= show_partition_start,
	.next	= disk_seqf_next,
	.stop	= disk_seqf_stop,
	.show	= show_partition
};
#endif

static int __init genhd_device_init(void)
{
	int error;

	block_class.dev_kobj = sysfs_dev_block_kobj;
	error = class_register(&block_class);
	if (unlikely(error))
		return error;
	blk_dev_init();

	register_blkdev(BLOCK_EXT_MAJOR, "blkext");

	/* create top-level block dir */
	if (!sysfs_deprecated)
		block_depr = kobject_create_and_add("block", NULL);
	return 0;
}

subsys_initcall(genhd_device_init);

static ssize_t disk_range_show(struct device *dev,
			       struct device_attribute *attr, char *buf)
{
	struct gendisk *disk = dev_to_disk(dev);

	return sprintf(buf, "%d\n", disk->minors);
}

static ssize_t disk_ext_range_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct gendisk *disk = dev_to_disk(dev);

	return sprintf(buf, "%d\n", disk_max_parts(disk));
}

static ssize_t disk_removable_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct gendisk *disk = dev_to_disk(dev);

	return sprintf(buf, "%d\n",
		       (disk->flags & GENHD_FL_REMOVABLE ? 1 : 0));
}

static ssize_t disk_hidden_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct gendisk *disk = dev_to_disk(dev);

	return sprintf(buf, "%d\n",
		       (disk->flags & GENHD_FL_HIDDEN ? 1 : 0));
}

static ssize_t disk_ro_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct gendisk *disk = dev_to_disk(dev);

	return sprintf(buf, "%d\n", get_disk_ro(disk) ? 1 : 0);
}

ssize_t part_size_show(struct device *dev,
		       struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%llu\n", bdev_nr_sectors(dev_to_bdev(dev)));
}

ssize_t part_stat_show(struct device *dev,
		       struct device_attribute *attr, char *buf)
{
	struct block_device *bdev = dev_to_bdev(dev);
	struct request_queue *q = bdev->bd_disk->queue;
	struct disk_stats stat;
	unsigned int inflight;

	part_stat_read_all(bdev, &stat);
	if (queue_is_mq(q))
		inflight = blk_mq_in_flight(q, bdev);
	else
		inflight = part_in_flight(bdev);

	return sprintf(buf,
		"%8lu %8lu %8llu %8u "
		"%8lu %8lu %8llu %8u "
		"%8u %8u %8u "
		"%8lu %8lu %8llu %8u "
		"%8lu %8u"
		"\n",
		stat.ios[STAT_READ],
		stat.merges[STAT_READ],
		(unsigned long long)stat.sectors[STAT_READ],
		(unsigned int)div_u64(stat.nsecs[STAT_READ], NSEC_PER_MSEC),
		stat.ios[STAT_WRITE],
		stat.merges[STAT_WRITE],
		(unsigned long long)stat.sectors[STAT_WRITE],
		(unsigned int)div_u64(stat.nsecs[STAT_WRITE], NSEC_PER_MSEC),
		inflight,
		jiffies_to_msecs(stat.io_ticks),
		(unsigned int)div_u64(stat.nsecs[STAT_READ] +
				      stat.nsecs[STAT_WRITE] +
				      stat.nsecs[STAT_DISCARD] +
				      stat.nsecs[STAT_FLUSH],
						NSEC_PER_MSEC),
		stat.ios[STAT_DISCARD],
		stat.merges[STAT_DISCARD],
		(unsigned long long)stat.sectors[STAT_DISCARD],
		(unsigned int)div_u64(stat.nsecs[STAT_DISCARD], NSEC_PER_MSEC),
		stat.ios[STAT_FLUSH],
		(unsigned int)div_u64(stat.nsecs[STAT_FLUSH], NSEC_PER_MSEC));
}

ssize_t part_inflight_show(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	struct block_device *bdev = dev_to_bdev(dev);
	struct request_queue *q = bdev->bd_disk->queue;
	unsigned int inflight[2];

	if (queue_is_mq(q))
		blk_mq_in_flight_rw(q, bdev, inflight);
	else
		part_in_flight_rw(bdev, inflight);

	return sprintf(buf, "%8u %8u\n", inflight[0], inflight[1]);
}

static ssize_t disk_capability_show(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	struct gendisk *disk = dev_to_disk(dev);

	return sprintf(buf, "%x\n", disk->flags);
}

static ssize_t disk_alignment_offset_show(struct device *dev,
					  struct device_attribute *attr,
					  char *buf)
{
	struct gendisk *disk = dev_to_disk(dev);

	return sprintf(buf, "%d\n", queue_alignment_offset(disk->queue));
}

static ssize_t disk_discard_alignment_show(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	struct gendisk *disk = dev_to_disk(dev);

	return sprintf(buf, "%d\n", queue_discard_alignment(disk->queue));
}

static DEVICE_ATTR(range, 0444, disk_range_show, NULL);
static DEVICE_ATTR(ext_range, 0444, disk_ext_range_show, NULL);
static DEVICE_ATTR(removable, 0444, disk_removable_show, NULL);
static DEVICE_ATTR(hidden, 0444, disk_hidden_show, NULL);
static DEVICE_ATTR(ro, 0444, disk_ro_show, NULL);
static DEVICE_ATTR(size, 0444, part_size_show, NULL);
static DEVICE_ATTR(alignment_offset, 0444, disk_alignment_offset_show, NULL);
static DEVICE_ATTR(discard_alignment, 0444, disk_discard_alignment_show, NULL);
static DEVICE_ATTR(capability, 0444, disk_capability_show, NULL);
static DEVICE_ATTR(stat, 0444, part_stat_show, NULL);
static DEVICE_ATTR(inflight, 0444, part_inflight_show, NULL);
static DEVICE_ATTR(badblocks, 0644, disk_badblocks_show, disk_badblocks_store);

#ifdef CONFIG_FAIL_MAKE_REQUEST
ssize_t part_fail_show(struct device *dev,
		       struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", dev_to_bdev(dev)->bd_make_it_fail);
}

ssize_t part_fail_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{
	int i;

	if (count > 0 && sscanf(buf, "%d", &i) > 0)
		dev_to_bdev(dev)->bd_make_it_fail = i;

	return count;
}

static struct device_attribute dev_attr_fail =
	__ATTR(make-it-fail, 0644, part_fail_show, part_fail_store);
#endif /* CONFIG_FAIL_MAKE_REQUEST */

#ifdef CONFIG_FAIL_IO_TIMEOUT
static struct device_attribute dev_attr_fail_timeout =
	__ATTR(io-timeout-fail, 0644, part_timeout_show, part_timeout_store);
#endif

static struct attribute *disk_attrs[] = {
	&dev_attr_range.attr,
	&dev_attr_ext_range.attr,
	&dev_attr_removable.attr,
	&dev_attr_hidden.attr,
	&dev_attr_ro.attr,
	&dev_attr_size.attr,
	&dev_attr_alignment_offset.attr,
	&dev_attr_discard_alignment.attr,
	&dev_attr_capability.attr,
	&dev_attr_stat.attr,
	&dev_attr_inflight.attr,
	&dev_attr_badblocks.attr,
#ifdef CONFIG_FAIL_MAKE_REQUEST
	&dev_attr_fail.attr,
#endif
#ifdef CONFIG_FAIL_IO_TIMEOUT
	&dev_attr_fail_timeout.attr,
#endif
	NULL
};

static umode_t disk_visible(struct kobject *kobj, struct attribute *a, int n)
{
	struct device *dev = container_of(kobj, typeof(*dev), kobj);
	struct gendisk *disk = dev_to_disk(dev);

	if (a == &dev_attr_badblocks.attr && !disk->bb)
		return 0;
	return a->mode;
}

static struct attribute_group disk_attr_group = {
	.attrs = disk_attrs,
	.is_visible = disk_visible,
};

static const struct attribute_group *disk_attr_groups[] = {
	&disk_attr_group,
	NULL
};

/**
 * disk_release - releases all allocated resources of the gendisk
 * @dev: the device representing this disk
 *
 * This function releases all allocated resources of the gendisk.
 *
 * Drivers which used __device_add_disk() have a gendisk with a request_queue
 * assigned. Since the request_queue sits on top of the gendisk for these
 * drivers we also call blk_put_queue() for them, and we expect the
 * request_queue refcount to reach 0 at this point, and so the request_queue
 * will also be freed prior to the disk.
 *
 * Context: can sleep
 */
static void disk_release(struct device *dev)
{
	struct gendisk *disk = dev_to_disk(dev);

	might_sleep();

	blk_free_devt(dev->devt);
	disk_release_events(disk);
	kfree(disk->random);
	xa_destroy(&disk->part_tbl);
	bdput(disk->part0);
	if (disk->queue)
		blk_put_queue(disk->queue);
	kfree(disk);
}
struct class block_class = {
	.name		= "block",
};

static char *block_devnode(struct device *dev, umode_t *mode,
			   kuid_t *uid, kgid_t *gid)
{
	struct gendisk *disk = dev_to_disk(dev);

	if (disk->fops->devnode)
		return disk->fops->devnode(disk, mode);
	return NULL;
}

const struct device_type disk_type = {
	.name		= "disk",
	.groups		= disk_attr_groups,
	.release	= disk_release,
	.devnode	= block_devnode,
};

#ifdef CONFIG_PROC_FS
/*
 * aggregate disk stat collector.  Uses the same stats that the sysfs
 * entries do, above, but makes them available through one seq_file.
 *
 * The output looks suspiciously like /proc/partitions with a bunch of
 * extra fields.
 */
static int diskstats_show(struct seq_file *seqf, void *v)
{
	struct gendisk *gp = v;
	struct block_device *hd;
	char buf[BDEVNAME_SIZE];
	unsigned int inflight;
	struct disk_stats stat;
	unsigned long idx;

	/*
	if (&disk_to_dev(gp)->kobj.entry == block_class.devices.next)
		seq_puts(seqf,	"major minor name"
				"     rio rmerge rsect ruse wio wmerge "
				"wsect wuse running use aveq"
				"\n\n");
	*/

	rcu_read_lock();
	xa_for_each(&gp->part_tbl, idx, hd) {
		if (bdev_is_partition(hd) && !bdev_nr_sectors(hd))
			continue;
		part_stat_read_all(hd, &stat);
		if (queue_is_mq(gp->queue))
			inflight = blk_mq_in_flight(gp->queue, hd);
		else
			inflight = part_in_flight(hd);

		seq_printf(seqf, "%4d %7d %s "
			   "%lu %lu %lu %u "
			   "%lu %lu %lu %u "
			   "%u %u %u "
			   "%lu %lu %lu %u "
			   "%lu %u"
			   "\n",
			   MAJOR(hd->bd_dev), MINOR(hd->bd_dev),
			   disk_name(gp, hd->bd_partno, buf),
			   stat.ios[STAT_READ],
			   stat.merges[STAT_READ],
			   stat.sectors[STAT_READ],
			   (unsigned int)div_u64(stat.nsecs[STAT_READ],
							NSEC_PER_MSEC),
			   stat.ios[STAT_WRITE],
			   stat.merges[STAT_WRITE],
			   stat.sectors[STAT_WRITE],
			   (unsigned int)div_u64(stat.nsecs[STAT_WRITE],
							NSEC_PER_MSEC),
			   inflight,
			   jiffies_to_msecs(stat.io_ticks),
			   (unsigned int)div_u64(stat.nsecs[STAT_READ] +
						 stat.nsecs[STAT_WRITE] +
						 stat.nsecs[STAT_DISCARD] +
						 stat.nsecs[STAT_FLUSH],
							NSEC_PER_MSEC),
			   stat.ios[STAT_DISCARD],
			   stat.merges[STAT_DISCARD],
			   stat.sectors[STAT_DISCARD],
			   (unsigned int)div_u64(stat.nsecs[STAT_DISCARD],
						 NSEC_PER_MSEC),
			   stat.ios[STAT_FLUSH],
			   (unsigned int)div_u64(stat.nsecs[STAT_FLUSH],
						 NSEC_PER_MSEC)
			);
	}
	rcu_read_unlock();

	return 0;
}

static const struct seq_operations diskstats_op = {
	.start	= disk_seqf_start,
	.next	= disk_seqf_next,
	.stop	= disk_seqf_stop,
	.show	= diskstats_show
};

static int __init proc_genhd_init(void)
{
	proc_create_seq("diskstats", 0, NULL, &diskstats_op);
	proc_create_seq("partitions", 0, NULL, &partitions_op);
	return 0;
}
module_init(proc_genhd_init);
#endif /* CONFIG_PROC_FS */

dev_t blk_lookup_devt(const char *name, int partno)
{
	dev_t devt = MKDEV(0, 0);
	struct class_dev_iter iter;
	struct device *dev;

	class_dev_iter_init(&iter, &block_class, NULL, &disk_type);
	while ((dev = class_dev_iter_next(&iter))) {
		struct gendisk *disk = dev_to_disk(dev);
		struct block_device *part;

		if (strcmp(dev_name(dev), name))
			continue;

		if (partno < disk->minors) {
			/* We need to return the right devno, even
			 * if the partition doesn't exist yet.
			 */
			devt = MKDEV(MAJOR(dev->devt),
				     MINOR(dev->devt) + partno);
			break;
		}
		part = bdget_disk(disk, partno);
		if (part) {
			devt = part->bd_dev;
			bdput(part);
			break;
		}
	}
	class_dev_iter_exit(&iter);
	return devt;
}

struct gendisk *__alloc_disk_node(int minors, int node_id)
{
	struct gendisk *disk;

	if (minors > DISK_MAX_PARTS) {
		printk(KERN_ERR
			"block: can't allocate more than %d partitions\n",
			DISK_MAX_PARTS);
		minors = DISK_MAX_PARTS;
	}

	disk = kzalloc_node(sizeof(struct gendisk), GFP_KERNEL, node_id);
	if (!disk)
		return NULL;

	disk->part0 = bdev_alloc(disk, 0);
	if (!disk->part0)
		goto out_free_disk;

	disk->node_id = node_id;
	xa_init(&disk->part_tbl);
	if (xa_insert(&disk->part_tbl, 0, disk->part0, GFP_KERNEL))
		goto out_destroy_part_tbl;

	disk->minors = minors;
	rand_initialize_disk(disk);
	disk_to_dev(disk)->class = &block_class;
	disk_to_dev(disk)->type = &disk_type;
	device_initialize(disk_to_dev(disk));
	return disk;

out_destroy_part_tbl:
	xa_destroy(&disk->part_tbl);
	bdput(disk->part0);
out_free_disk:
	kfree(disk);
	return NULL;
}
EXPORT_SYMBOL(__alloc_disk_node);

/**
 * put_disk - decrements the gendisk refcount
 * @disk: the struct gendisk to decrement the refcount for
 *
 * This decrements the refcount for the struct gendisk. When this reaches 0
 * we'll have disk_release() called.
 *
 * Context: Any context, but the last reference must not be dropped from
 *          atomic context.
 */
void put_disk(struct gendisk *disk)
{
	if (disk)
		put_device(disk_to_dev(disk));
}
EXPORT_SYMBOL(put_disk);

static void set_disk_ro_uevent(struct gendisk *gd, int ro)
{
	char event[] = "DISK_RO=1";
	char *envp[] = { event, NULL };

	if (!ro)
		event[8] = '0';
	kobject_uevent_env(&disk_to_dev(gd)->kobj, KOBJ_CHANGE, envp);
}

/**
 * set_disk_ro - set a gendisk read-only
 * @disk:	gendisk to operate on
 * @read_only:	%true to set the disk read-only, %false set the disk read/write
 *
 * This function is used to indicate whether a given disk device should have its
 * read-only flag set. set_disk_ro() is typically used by device drivers to
 * indicate whether the underlying physical device is write-protected.
 */
void set_disk_ro(struct gendisk *disk, bool read_only)
{
	if (read_only) {
		if (test_and_set_bit(GD_READ_ONLY, &disk->state))
			return;
	} else {
		if (!test_and_clear_bit(GD_READ_ONLY, &disk->state))
			return;
	}
	set_disk_ro_uevent(disk, read_only);
}
EXPORT_SYMBOL(set_disk_ro);

int bdev_read_only(struct block_device *bdev)
{
	return bdev->bd_read_only || get_disk_ro(bdev->bd_disk);
}
EXPORT_SYMBOL(bdev_read_only);

/*
 * Disk events - monitor disk events like media change and eject request.
 */
struct disk_events {
	struct list_head	node;		/* all disk_event's */
	struct gendisk		*disk;		/* the associated disk */
	spinlock_t		lock;

	struct mutex		block_mutex;	/* protects blocking */
	int			block;		/* event blocking depth */
	unsigned int		pending;	/* events already sent out */
	unsigned int		clearing;	/* events being cleared */

	long			poll_msecs;	/* interval, -1 for default */
	struct delayed_work	dwork;
};

static const char *disk_events_strs[] = {
	[ilog2(DISK_EVENT_MEDIA_CHANGE)]	= "media_change",
	[ilog2(DISK_EVENT_EJECT_REQUEST)]	= "eject_request",
};

static char *disk_uevents[] = {
	[ilog2(DISK_EVENT_MEDIA_CHANGE)]	= "DISK_MEDIA_CHANGE=1",
	[ilog2(DISK_EVENT_EJECT_REQUEST)]	= "DISK_EJECT_REQUEST=1",
};

/* list of all disk_events */
static DEFINE_MUTEX(disk_events_mutex);
static LIST_HEAD(disk_events);

/* disable in-kernel polling by default */
static unsigned long disk_events_dfl_poll_msecs;

static unsigned long disk_events_poll_jiffies(struct gendisk *disk)
{
	struct disk_events *ev = disk->ev;
	long intv_msecs = 0;

	/*
	 * If device-specific poll interval is set, always use it.  If
	 * the default is being used, poll if the POLL flag is set.
	 */
	if (ev->poll_msecs >= 0)
		intv_msecs = ev->poll_msecs;
	else if (disk->event_flags & DISK_EVENT_FLAG_POLL)
		intv_msecs = disk_events_dfl_poll_msecs;

	return msecs_to_jiffies(intv_msecs);
}

/**
 * disk_block_events - block and flush disk event checking
 * @disk: disk to block events for
 *
 * On return from this function, it is guaranteed that event checking
 * isn't in progress and won't happen until unblocked by
 * disk_unblock_events().  Events blocking is counted and the actual
 * unblocking happens after the matching number of unblocks are done.
 *
 * Note that this intentionally does not block event checking from
 * disk_clear_events().
 *
 * CONTEXT:
 * Might sleep.
 */
void disk_block_events(struct gendisk *disk)
{
	struct disk_events *ev = disk->ev;
	unsigned long flags;
	bool cancel;

	if (!ev)
		return;

	/*
	 * Outer mutex ensures that the first blocker completes canceling
	 * the event work before further blockers are allowed to finish.
	 */
	mutex_lock(&ev->block_mutex);

	spin_lock_irqsave(&ev->lock, flags);
	cancel = !ev->block++;
	spin_unlock_irqrestore(&ev->lock, flags);

	if (cancel)
		cancel_delayed_work_sync(&disk->ev->dwork);

	mutex_unlock(&ev->block_mutex);
}

static void __disk_unblock_events(struct gendisk *disk, bool check_now)
{
	struct disk_events *ev = disk->ev;
	unsigned long intv;
	unsigned long flags;

	spin_lock_irqsave(&ev->lock, flags);

	if (WARN_ON_ONCE(ev->block <= 0))
		goto out_unlock;

	if (--ev->block)
		goto out_unlock;

	intv = disk_events_poll_jiffies(disk);
	if (check_now)
		queue_delayed_work(system_freezable_power_efficient_wq,
				&ev->dwork, 0);
	else if (intv)
		queue_delayed_work(system_freezable_power_efficient_wq,
				&ev->dwork, intv);
out_unlock:
	spin_unlock_irqrestore(&ev->lock, flags);
}

/**
 * disk_unblock_events - unblock disk event checking
 * @disk: disk to unblock events for
 *
 * Undo disk_block_events().  When the block count reaches zero, it
 * starts events polling if configured.
 *
 * CONTEXT:
 * Don't care.  Safe to call from irq context.
 */
void disk_unblock_events(struct gendisk *disk)
{
	if (disk->ev)
		__disk_unblock_events(disk, false);
}

/**
 * disk_flush_events - schedule immediate event checking and flushing
 * @disk: disk to check and flush events for
 * @mask: events to flush
 *
 * Schedule immediate event checking on @disk if not blocked.  Events in
 * @mask are scheduled to be cleared from the driver.  Note that this
 * doesn't clear the events from @disk->ev.
 *
 * CONTEXT:
 * If @mask is non-zero must be called with bdev->bd_mutex held.
 */
void disk_flush_events(struct gendisk *disk, unsigned int mask)
{
	struct disk_events *ev = disk->ev;

	if (!ev)
		return;

	spin_lock_irq(&ev->lock);
	ev->clearing |= mask;
	if (!ev->block)
		mod_delayed_work(system_freezable_power_efficient_wq,
				&ev->dwork, 0);
	spin_unlock_irq(&ev->lock);
}

/**
 * disk_clear_events - synchronously check, clear and return pending events
 * @disk: disk to fetch and clear events from
 * @mask: mask of events to be fetched and cleared
 *
 * Disk events are synchronously checked and pending events in @mask
 * are cleared and returned.  This ignores the block count.
 *
 * CONTEXT:
 * Might sleep.
 */
static unsigned int disk_clear_events(struct gendisk *disk, unsigned int mask)
{
	struct disk_events *ev = disk->ev;
	unsigned int pending;
	unsigned int clearing = mask;

	if (!ev)
		return 0;

	disk_block_events(disk);

	/*
	 * store the union of mask and ev->clearing on the stack so that the
	 * race with disk_flush_events does not cause ambiguity (ev->clearing
	 * can still be modified even if events are blocked).
	 */
	spin_lock_irq(&ev->lock);
	clearing |= ev->clearing;
	ev->clearing = 0;
	spin_unlock_irq(&ev->lock);

	disk_check_events(ev, &clearing);
	/*
	 * if ev->clearing is not 0, the disk_flush_events got called in the
	 * middle of this function, so we want to run the workfn without delay.
	 */
	__disk_unblock_events(disk, ev->clearing ? true : false);

	/* then, fetch and clear pending events */
	spin_lock_irq(&ev->lock);
	pending = ev->pending & mask;
	ev->pending &= ~mask;
	spin_unlock_irq(&ev->lock);
	WARN_ON_ONCE(clearing & mask);

	return pending;
}

/**
 * bdev_check_media_change - check if a removable media has been changed
 * @bdev: block device to check
 *
 * Check whether a removable media has been changed, and attempt to free all
 * dentries and inodes and invalidates all block device page cache entries in
 * that case.
 *
 * Returns %true if the block device changed, or %false if not.
 */
bool bdev_check_media_change(struct block_device *bdev)
{
	unsigned int events;

	events = disk_clear_events(bdev->bd_disk, DISK_EVENT_MEDIA_CHANGE |
				   DISK_EVENT_EJECT_REQUEST);
	if (!(events & DISK_EVENT_MEDIA_CHANGE))
		return false;

	if (__invalidate_device(bdev, true))
		pr_warn("VFS: busy inodes on changed media %s\n",
			bdev->bd_disk->disk_name);
	set_bit(GD_NEED_PART_SCAN, &bdev->bd_disk->state);
	return true;
}
EXPORT_SYMBOL(bdev_check_media_change);

/*
 * Separate this part out so that a different pointer for clearing_ptr can be
 * passed in for disk_clear_events.
 */
static void disk_events_workfn(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct disk_events *ev = container_of(dwork, struct disk_events, dwork);

	disk_check_events(ev, &ev->clearing);
}

static void disk_check_events(struct disk_events *ev,
			      unsigned int *clearing_ptr)
{
	struct gendisk *disk = ev->disk;
	char *envp[ARRAY_SIZE(disk_uevents) + 1] = { };
	unsigned int clearing = *clearing_ptr;
	unsigned int events;
	unsigned long intv;
	int nr_events = 0, i;

	/* check events */
	events = disk->fops->check_events(disk, clearing);

	/* accumulate pending events and schedule next poll if necessary */
	spin_lock_irq(&ev->lock);

	events &= ~ev->pending;
	ev->pending |= events;
	*clearing_ptr &= ~clearing;

	intv = disk_events_poll_jiffies(disk);
	if (!ev->block && intv)
		queue_delayed_work(system_freezable_power_efficient_wq,
				&ev->dwork, intv);

	spin_unlock_irq(&ev->lock);

	/*
	 * Tell userland about new events.  Only the events listed in
	 * @disk->events are reported, and only if DISK_EVENT_FLAG_UEVENT
	 * is set. Otherwise, events are processed internally but never
	 * get reported to userland.
	 */
	for (i = 0; i < ARRAY_SIZE(disk_uevents); i++)
		if ((events & disk->events & (1 << i)) &&
		    (disk->event_flags & DISK_EVENT_FLAG_UEVENT))
			envp[nr_events++] = disk_uevents[i];

	if (nr_events)
		kobject_uevent_env(&disk_to_dev(disk)->kobj, KOBJ_CHANGE, envp);
}

/*
 * A disk events enabled device has the following sysfs nodes under
 * its /sys/block/X/ directory.
 *
 * events		: list of all supported events
 * events_async		: list of events which can be detected w/o polling
 *			  (always empty, only for backwards compatibility)
 * events_poll_msecs	: polling interval, 0: disable, -1: system default
 */
static ssize_t __disk_events_show(unsigned int events, char *buf)
{
	const char *delim = "";
	ssize_t pos = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(disk_events_strs); i++)
		if (events & (1 << i)) {
			pos += sprintf(buf + pos, "%s%s",
				       delim, disk_events_strs[i]);
			delim = " ";
		}
	if (pos)
		pos += sprintf(buf + pos, "\n");
	return pos;
}

static ssize_t disk_events_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct gendisk *disk = dev_to_disk(dev);

	if (!(disk->event_flags & DISK_EVENT_FLAG_UEVENT))
		return 0;

	return __disk_events_show(disk->events, buf);
}

static ssize_t disk_events_async_show(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	return 0;
}

static ssize_t disk_events_poll_msecs_show(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	struct gendisk *disk = dev_to_disk(dev);

	if (!disk->ev)
		return sprintf(buf, "-1\n");

	return sprintf(buf, "%ld\n", disk->ev->poll_msecs);
}

static ssize_t disk_events_poll_msecs_store(struct device *dev,
					    struct device_attribute *attr,
					    const char *buf, size_t count)
{
	struct gendisk *disk = dev_to_disk(dev);
	long intv;

	if (!count || !sscanf(buf, "%ld", &intv))
		return -EINVAL;

	if (intv < 0 && intv != -1)
		return -EINVAL;

	if (!disk->ev)
		return -ENODEV;

	disk_block_events(disk);
	disk->ev->poll_msecs = intv;
	__disk_unblock_events(disk, true);

	return count;
}

static const DEVICE_ATTR(events, 0444, disk_events_show, NULL);
static const DEVICE_ATTR(events_async, 0444, disk_events_async_show, NULL);
static const DEVICE_ATTR(events_poll_msecs, 0644,
			 disk_events_poll_msecs_show,
			 disk_events_poll_msecs_store);

static const struct attribute *disk_events_attrs[] = {
	&dev_attr_events.attr,
	&dev_attr_events_async.attr,
	&dev_attr_events_poll_msecs.attr,
	NULL,
};

/*
 * The default polling interval can be specified by the kernel
 * parameter block.events_dfl_poll_msecs which defaults to 0
 * (disable).  This can also be modified runtime by writing to
 * /sys/module/block/parameters/events_dfl_poll_msecs.
 */
static int disk_events_set_dfl_poll_msecs(const char *val,
					  const struct kernel_param *kp)
{
	struct disk_events *ev;
	int ret;

	ret = param_set_ulong(val, kp);
	if (ret < 0)
		return ret;

	mutex_lock(&disk_events_mutex);

	list_for_each_entry(ev, &disk_events, node)
		disk_flush_events(ev->disk, 0);

	mutex_unlock(&disk_events_mutex);

	return 0;
}

static const struct kernel_param_ops disk_events_dfl_poll_msecs_param_ops = {
	.set	= disk_events_set_dfl_poll_msecs,
	.get	= param_get_ulong,
};

#undef MODULE_PARAM_PREFIX
#define MODULE_PARAM_PREFIX	"block."

module_param_cb(events_dfl_poll_msecs, &disk_events_dfl_poll_msecs_param_ops,
		&disk_events_dfl_poll_msecs, 0644);

/*
 * disk_{alloc|add|del|release}_events - initialize and destroy disk_events.
 */
static void disk_alloc_events(struct gendisk *disk)
{
	struct disk_events *ev;

	if (!disk->fops->check_events || !disk->events)
		return;

	ev = kzalloc(sizeof(*ev), GFP_KERNEL);
	if (!ev) {
		pr_warn("%s: failed to initialize events\n", disk->disk_name);
		return;
	}

	INIT_LIST_HEAD(&ev->node);
	ev->disk = disk;
	spin_lock_init(&ev->lock);
	mutex_init(&ev->block_mutex);
	ev->block = 1;
	ev->poll_msecs = -1;
	INIT_DELAYED_WORK(&ev->dwork, disk_events_workfn);

	disk->ev = ev;
}

static void disk_add_events(struct gendisk *disk)
{
	/* FIXME: error handling */
	if (sysfs_create_files(&disk_to_dev(disk)->kobj, disk_events_attrs) < 0)
		pr_warn("%s: failed to create sysfs files for events\n",
			disk->disk_name);

	if (!disk->ev)
		return;

	mutex_lock(&disk_events_mutex);
	list_add_tail(&disk->ev->node, &disk_events);
	mutex_unlock(&disk_events_mutex);

	/*
	 * Block count is initialized to 1 and the following initial
	 * unblock kicks it into action.
	 */
	__disk_unblock_events(disk, true);
}

static void disk_del_events(struct gendisk *disk)
{
	if (disk->ev) {
		disk_block_events(disk);

		mutex_lock(&disk_events_mutex);
		list_del_init(&disk->ev->node);
		mutex_unlock(&disk_events_mutex);
	}

	sysfs_remove_files(&disk_to_dev(disk)->kobj, disk_events_attrs);
}

static void disk_release_events(struct gendisk *disk)
{
	/* the block count should be 1 from disk_del_events() */
	WARN_ON_ONCE(disk->ev && disk->ev->block != 1);
	kfree(disk->ev);
}
