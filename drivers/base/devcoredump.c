// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright(c) 2014 Intel Mobile Communications GmbH
 * Copyright(c) 2015 Intel Deutschland GmbH
 *
 * Contact Information:
 *  Intel Linux Wireless <ilw@linux.intel.com>
 * Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497
 *
 * Author: Johannes Berg <johannes@sipsolutions.net>
 */
#include <linux/module.h>
#include <linux/device.h>
#include <linux/devcoredump.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/workqueue.h>

static struct class devcd_class;

/* global disable flag, for security purposes */
static bool devcd_disabled;

/* if data isn't read by userspace after 5 minutes then delete it */
#define DEVCD_TIMEOUT	(HZ * 60 * 5)

struct devcd_entry {
	struct device devcd_dev;
	void *data;
	size_t datalen;
	struct module *owner;
	ssize_t (*read)(char *buffer, loff_t offset, size_t count,
			void *data, size_t datalen);
	void (*free)(void *data);
	struct delayed_work del_wk;
	struct device *failing_dev;
};

static struct devcd_entry *dev_to_devcd(struct device *dev)
{
	return container_of(dev, struct devcd_entry, devcd_dev);
}

static void devcd_dev_release(struct device *dev)
{
	struct devcd_entry *devcd = dev_to_devcd(dev);

	devcd->free(devcd->data);
	module_put(devcd->owner);

	/*
	 * this seems racy, but I don't see a notifier or such on
	 * a struct device to know when it goes away?
	 */
	if (devcd->failing_dev->kobj.sd)
		sysfs_delete_link(&devcd->failing_dev->kobj, &dev->kobj,
				  "devcoredump");

	put_device(devcd->failing_dev);
	kfree(devcd);
}

static void devcd_del(struct work_struct *wk)
{
	struct devcd_entry *devcd;

	devcd = container_of(wk, struct devcd_entry, del_wk.work);

	device_del(&devcd->devcd_dev);
	put_device(&devcd->devcd_dev);
}

static ssize_t devcd_data_read(struct file *filp, struct kobject *kobj,
			       struct bin_attribute *bin_attr,
			       char *buffer, loff_t offset, size_t count)
{
	struct device *dev = kobj_to_dev(kobj);
	struct devcd_entry *devcd = dev_to_devcd(dev);

	return devcd->read(buffer, offset, count, devcd->data, devcd->datalen);
}

static ssize_t devcd_data_write(struct file *filp, struct kobject *kobj,
				struct bin_attribute *bin_attr,
				char *buffer, loff_t offset, size_t count)
{
	struct device *dev = kobj_to_dev(kobj);
	struct devcd_entry *devcd = dev_to_devcd(dev);

	mod_delayed_work(system_wq, &devcd->del_wk, 0);

	return count;
}

static struct bin_attribute devcd_attr_data = {
	.attr = { .name = "data", .mode = S_IRUSR | S_IWUSR, },
	.size = 0,
	.read = devcd_data_read,
	.write = devcd_data_write,
};

static struct bin_attribute *devcd_dev_bin_attrs[] = {
	&devcd_attr_data, NULL,
};

static const struct attribute_group devcd_dev_group = {
	.bin_attrs = devcd_dev_bin_attrs,
};

static const struct attribute_group *devcd_dev_groups[] = {
	&devcd_dev_group, NULL,
};

static int devcd_free(struct device *dev, void *data)
{
	struct devcd_entry *devcd = dev_to_devcd(dev);

	flush_delayed_work(&devcd->del_wk);
	return 0;
}

static ssize_t disabled_show(struct class *class, struct class_attribute *attr,
			     char *buf)
{
	return sysfs_emit(buf, "%d\n", devcd_disabled);
}

static ssize_t disabled_store(struct class *class, struct class_attribute *attr,
			      const char *buf, size_t count)
{
	long tmp = simple_strtol(buf, NULL, 10);

	/*
	 * This essentially makes the attribute write-once, since you can't
	 * go back to not having it disabled. This is intentional, it serves
	 * as a system lockdown feature.
	 */
	if (tmp != 1)
		return -EINVAL;

	devcd_disabled = true;

	class_for_each_device(&devcd_class, NULL, NULL, devcd_free);

	return count;
}
static CLASS_ATTR_RW(disabled);

static struct attribute *devcd_class_attrs[] = {
	&class_attr_disabled.attr,
	NULL,
};
ATTRIBUTE_GROUPS(devcd_class);

static struct class devcd_class = {
	.name		= "devcoredump",
	.owner		= THIS_MODULE,
	.dev_release	= devcd_dev_release,
	.dev_groups	= devcd_dev_groups,
	.class_groups	= devcd_class_groups,
};

static ssize_t devcd_readv(char *buffer, loff_t offset, size_t count,
			   void *data, size_t datalen)
{
	return memory_read_from_buffer(buffer, count, &offset, data, datalen);
}

static void devcd_freev(void *data)
{
	vfree(data);
}

/**
 * dev_coredumpv - create device coredump with vmalloc data
 * @dev: the struct device for the crashed device
 * @data: vmalloc data containing the device coredump
 * @datalen: length of the data
 * @gfp: allocation flags
 *
 * This function takes ownership of the vmalloc'ed data and will free
 * it when it is no longer used. See dev_coredumpm() for more information.
 */
void dev_coredumpv(struct device *dev, void *data, size_t datalen,
		   gfp_t gfp)
{
	dev_coredumpm(dev, NULL, data, datalen, gfp, devcd_readv, devcd_freev);
}
EXPORT_SYMBOL_GPL(dev_coredumpv);

static int devcd_match_failing(struct device *dev, const void *failing)
{
	struct devcd_entry *devcd = dev_to_devcd(dev);

	return devcd->failing_dev == failing;
}

/**
 * devcd_free_sgtable - free all the memory of the given scatterlist table
 * (i.e. both pages and scatterlist instances)
 * NOTE: if two tables allocated with devcd_alloc_sgtable and then chained
 * using the sg_chain function then that function should be called only once
 * on the chained table
 * @data: pointer to sg_table to free
 */
static void devcd_free_sgtable(void *data)
{
	_devcd_free_sgtable(data);
}

/**
 * devcd_read_from_sgtable - copy data from sg_table to a given buffer
 * and return the number of bytes read
 * @buffer: the buffer to copy the data to it
 * @buf_len: the length of the buffer
 * @data: the scatterlist table to copy from
 * @offset: start copy from @offset@ bytes from the head of the data
 *	in the given scatterlist
 * @data_len: the length of the data in the sg_table
 */
static ssize_t devcd_read_from_sgtable(char *buffer, loff_t offset,
				       size_t buf_len, void *data,
				       size_t data_len)
{
	struct scatterlist *table = data;

	if (offset > data_len)
		return -EINVAL;

	if (offset + buf_len > data_len)
		buf_len = data_len - offset;
	return sg_pcopy_to_buffer(table, sg_nents(table), buffer, buf_len,
				  offset);
}

/**
 * dev_coredumpm - create device coredump with read/free methods
 * @dev: the struct device for the crashed device
 * @owner: the module that contains the read/free functions, use %THIS_MODULE
 * @data: data cookie for the @read/@free functions
 * @datalen: length of the data
 * @gfp: allocation flags
 * @read: function to read from the given buffer
 * @free: function to free the given buffer
 *
 * Creates a new device coredump for the given device. If a previous one hasn't
 * been read yet, the new coredump is discarded. The data lifetime is determined
 * by the device coredump framework and when it is no longer needed the @free
 * function will be called to free the data.
 */
void dev_coredumpm(struct device *dev, struct module *owner,
		   void *data, size_t datalen, gfp_t gfp,
		   ssize_t (*read)(char *buffer, loff_t offset, size_t count,
				   void *data, size_t datalen),
		   void (*free)(void *data))
{
	static atomic_t devcd_count = ATOMIC_INIT(0);
	struct devcd_entry *devcd;
	struct device *existing;

	if (devcd_disabled)
		goto free;

	existing = class_find_device(&devcd_class, NULL, dev,
				     devcd_match_failing);
	if (existing) {
		put_device(existing);
		goto free;
	}

	if (!try_module_get(owner))
		goto free;

	devcd = kzalloc(sizeof(*devcd), gfp);
	if (!devcd)
		goto put_module;

	devcd->owner = owner;
	devcd->data = data;
	devcd->datalen = datalen;
	devcd->read = read;
	devcd->free = free;
	devcd->failing_dev = get_device(dev);

	device_initialize(&devcd->devcd_dev);

	dev_set_name(&devcd->devcd_dev, "devcd%d",
		     atomic_inc_return(&devcd_count));
	devcd->devcd_dev.class = &devcd_class;

	if (device_add(&devcd->devcd_dev))
		goto put_device;

	/*
	 * These should normally not fail, but there is no problem
	 * continuing without the links, so just warn instead of
	 * failing.
	 */
	if (sysfs_create_link(&devcd->devcd_dev.kobj, &dev->kobj,
			      "failing_device") ||
	    sysfs_create_link(&dev->kobj, &devcd->devcd_dev.kobj,
		              "devcoredump"))
		dev_warn(dev, "devcoredump create_link failed\n");

	INIT_DELAYED_WORK(&devcd->del_wk, devcd_del);
	schedule_delayed_work(&devcd->del_wk, DEVCD_TIMEOUT);

	return;
 put_device:
	put_device(&devcd->devcd_dev);
 put_module:
	module_put(owner);
 free:
	free(data);
}
EXPORT_SYMBOL_GPL(dev_coredumpm);

/**
 * dev_coredumpsg - create device coredump that uses scatterlist as data
 * parameter
 * @dev: the struct device for the crashed device
 * @table: the dump data
 * @datalen: length of the data
 * @gfp: allocation flags
 *
 * Creates a new device coredump for the given device. If a previous one hasn't
 * been read yet, the new coredump is discarded. The data lifetime is determined
 * by the device coredump framework and when it is no longer needed
 * it will free the data.
 */
void dev_coredumpsg(struct device *dev, struct scatterlist *table,
		    size_t datalen, gfp_t gfp)
{
	dev_coredumpm(dev, NULL, table, datalen, gfp, devcd_read_from_sgtable,
		      devcd_free_sgtable);
}
EXPORT_SYMBOL_GPL(dev_coredumpsg);

static int __init devcoredump_init(void)
{
	return class_register(&devcd_class);
}
__initcall(devcoredump_init);

static void __exit devcoredump_exit(void)
{
	class_for_each_device(&devcd_class, NULL, NULL, devcd_free);
	class_unregister(&devcd_class);
}
__exitcall(devcoredump_exit);
