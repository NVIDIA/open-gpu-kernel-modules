// SPDX-License-Identifier: GPL-2.0+
/*
 * comedi/comedi_fops.c
 * comedi kernel module
 *
 * COMEDI - Linux Control and Measurement Device Interface
 * Copyright (C) 1997-2007 David A. Schleef <ds@schleef.org>
 * compat ioctls:
 * Author: Ian Abbott, MEV Ltd. <abbotti@mev.co.uk>
 * Copyright (C) 2007 MEV Ltd. <http://www.mev.co.uk/>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/fcntl.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/fs.h>
#include "comedidev.h"
#include <linux/cdev.h>

#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/compat.h>

#include "comedi_internal.h"

/*
 * comedi_subdevice "runflags"
 * COMEDI_SRF_RT:		DEPRECATED: command is running real-time
 * COMEDI_SRF_ERROR:		indicates an COMEDI_CB_ERROR event has occurred
 *				since the last command was started
 * COMEDI_SRF_RUNNING:		command is running
 * COMEDI_SRF_FREE_SPRIV:	free s->private on detach
 *
 * COMEDI_SRF_BUSY_MASK:	runflags that indicate the subdevice is "busy"
 */
#define COMEDI_SRF_RT		BIT(1)
#define COMEDI_SRF_ERROR	BIT(2)
#define COMEDI_SRF_RUNNING	BIT(27)
#define COMEDI_SRF_FREE_SPRIV	BIT(31)

#define COMEDI_SRF_BUSY_MASK	(COMEDI_SRF_ERROR | COMEDI_SRF_RUNNING)

/**
 * struct comedi_file - Per-file private data for COMEDI device
 * @dev: COMEDI device.
 * @read_subdev: Current "read" subdevice.
 * @write_subdev: Current "write" subdevice.
 * @last_detach_count: Last known detach count.
 * @last_attached: Last known attached/detached state.
 */
struct comedi_file {
	struct comedi_device *dev;
	struct comedi_subdevice *read_subdev;
	struct comedi_subdevice *write_subdev;
	unsigned int last_detach_count;
	unsigned int last_attached:1;
};

#define COMEDI_NUM_MINORS 0x100
#define COMEDI_NUM_SUBDEVICE_MINORS	\
	(COMEDI_NUM_MINORS - COMEDI_NUM_BOARD_MINORS)

static unsigned short comedi_num_legacy_minors;
module_param(comedi_num_legacy_minors, ushort, 0444);
MODULE_PARM_DESC(comedi_num_legacy_minors,
		 "number of comedi minor devices to reserve for non-auto-configured devices (default 0)"
		);

unsigned int comedi_default_buf_size_kb = CONFIG_COMEDI_DEFAULT_BUF_SIZE_KB;
module_param(comedi_default_buf_size_kb, uint, 0644);
MODULE_PARM_DESC(comedi_default_buf_size_kb,
		 "default asynchronous buffer size in KiB (default "
		 __MODULE_STRING(CONFIG_COMEDI_DEFAULT_BUF_SIZE_KB) ")");

unsigned int comedi_default_buf_maxsize_kb =
	CONFIG_COMEDI_DEFAULT_BUF_MAXSIZE_KB;
module_param(comedi_default_buf_maxsize_kb, uint, 0644);
MODULE_PARM_DESC(comedi_default_buf_maxsize_kb,
		 "default maximum size of asynchronous buffer in KiB (default "
		 __MODULE_STRING(CONFIG_COMEDI_DEFAULT_BUF_MAXSIZE_KB) ")");

static DEFINE_MUTEX(comedi_board_minor_table_lock);
static struct comedi_device
*comedi_board_minor_table[COMEDI_NUM_BOARD_MINORS];

static DEFINE_MUTEX(comedi_subdevice_minor_table_lock);
/* Note: indexed by minor - COMEDI_NUM_BOARD_MINORS. */
static struct comedi_subdevice
*comedi_subdevice_minor_table[COMEDI_NUM_SUBDEVICE_MINORS];

static struct class *comedi_class;
static struct cdev comedi_cdev;

static void comedi_device_init(struct comedi_device *dev)
{
	kref_init(&dev->refcount);
	spin_lock_init(&dev->spinlock);
	mutex_init(&dev->mutex);
	init_rwsem(&dev->attach_lock);
	dev->minor = -1;
}

static void comedi_dev_kref_release(struct kref *kref)
{
	struct comedi_device *dev =
		container_of(kref, struct comedi_device, refcount);

	mutex_destroy(&dev->mutex);
	put_device(dev->class_dev);
	kfree(dev);
}

/**
 * comedi_dev_put() - Release a use of a COMEDI device
 * @dev: COMEDI device.
 *
 * Must be called when a user of a COMEDI device is finished with it.
 * When the last user of the COMEDI device calls this function, the
 * COMEDI device is destroyed.
 *
 * Return: 1 if the COMEDI device is destroyed by this call or @dev is
 * NULL, otherwise return 0.  Callers must not assume the COMEDI
 * device is still valid if this function returns 0.
 */
int comedi_dev_put(struct comedi_device *dev)
{
	if (dev)
		return kref_put(&dev->refcount, comedi_dev_kref_release);
	return 1;
}
EXPORT_SYMBOL_GPL(comedi_dev_put);

static struct comedi_device *comedi_dev_get(struct comedi_device *dev)
{
	if (dev)
		kref_get(&dev->refcount);
	return dev;
}

static void comedi_device_cleanup(struct comedi_device *dev)
{
	struct module *driver_module = NULL;

	if (!dev)
		return;
	mutex_lock(&dev->mutex);
	if (dev->attached)
		driver_module = dev->driver->module;
	comedi_device_detach(dev);
	if (driver_module && dev->use_count)
		module_put(driver_module);
	mutex_unlock(&dev->mutex);
}

static bool comedi_clear_board_dev(struct comedi_device *dev)
{
	unsigned int i = dev->minor;
	bool cleared = false;

	lockdep_assert_held(&dev->mutex);
	mutex_lock(&comedi_board_minor_table_lock);
	if (dev == comedi_board_minor_table[i]) {
		comedi_board_minor_table[i] = NULL;
		cleared = true;
	}
	mutex_unlock(&comedi_board_minor_table_lock);
	return cleared;
}

static struct comedi_device *comedi_clear_board_minor(unsigned int minor)
{
	struct comedi_device *dev;

	mutex_lock(&comedi_board_minor_table_lock);
	dev = comedi_board_minor_table[minor];
	comedi_board_minor_table[minor] = NULL;
	mutex_unlock(&comedi_board_minor_table_lock);
	return dev;
}

static void comedi_free_board_dev(struct comedi_device *dev)
{
	if (dev) {
		comedi_device_cleanup(dev);
		if (dev->class_dev) {
			device_destroy(comedi_class,
				       MKDEV(COMEDI_MAJOR, dev->minor));
		}
		comedi_dev_put(dev);
	}
}

static struct comedi_subdevice *
comedi_subdevice_from_minor(const struct comedi_device *dev, unsigned int minor)
{
	struct comedi_subdevice *s;
	unsigned int i = minor - COMEDI_NUM_BOARD_MINORS;

	mutex_lock(&comedi_subdevice_minor_table_lock);
	s = comedi_subdevice_minor_table[i];
	if (s && s->device != dev)
		s = NULL;
	mutex_unlock(&comedi_subdevice_minor_table_lock);
	return s;
}

static struct comedi_device *comedi_dev_get_from_board_minor(unsigned int minor)
{
	struct comedi_device *dev;

	mutex_lock(&comedi_board_minor_table_lock);
	dev = comedi_dev_get(comedi_board_minor_table[minor]);
	mutex_unlock(&comedi_board_minor_table_lock);
	return dev;
}

static struct comedi_device *
comedi_dev_get_from_subdevice_minor(unsigned int minor)
{
	struct comedi_device *dev;
	struct comedi_subdevice *s;
	unsigned int i = minor - COMEDI_NUM_BOARD_MINORS;

	mutex_lock(&comedi_subdevice_minor_table_lock);
	s = comedi_subdevice_minor_table[i];
	dev = comedi_dev_get(s ? s->device : NULL);
	mutex_unlock(&comedi_subdevice_minor_table_lock);
	return dev;
}

/**
 * comedi_dev_get_from_minor() - Get COMEDI device by minor device number
 * @minor: Minor device number.
 *
 * Finds the COMEDI device associated with the minor device number, if any,
 * and increments its reference count.  The COMEDI device is prevented from
 * being freed until a matching call is made to comedi_dev_put().
 *
 * Return: A pointer to the COMEDI device if it exists, with its usage
 * reference incremented.  Return NULL if no COMEDI device exists with the
 * specified minor device number.
 */
struct comedi_device *comedi_dev_get_from_minor(unsigned int minor)
{
	if (minor < COMEDI_NUM_BOARD_MINORS)
		return comedi_dev_get_from_board_minor(minor);

	return comedi_dev_get_from_subdevice_minor(minor);
}
EXPORT_SYMBOL_GPL(comedi_dev_get_from_minor);

static struct comedi_subdevice *
comedi_read_subdevice(const struct comedi_device *dev, unsigned int minor)
{
	struct comedi_subdevice *s;

	lockdep_assert_held(&dev->mutex);
	if (minor >= COMEDI_NUM_BOARD_MINORS) {
		s = comedi_subdevice_from_minor(dev, minor);
		if (!s || (s->subdev_flags & SDF_CMD_READ))
			return s;
	}
	return dev->read_subdev;
}

static struct comedi_subdevice *
comedi_write_subdevice(const struct comedi_device *dev, unsigned int minor)
{
	struct comedi_subdevice *s;

	lockdep_assert_held(&dev->mutex);
	if (minor >= COMEDI_NUM_BOARD_MINORS) {
		s = comedi_subdevice_from_minor(dev, minor);
		if (!s || (s->subdev_flags & SDF_CMD_WRITE))
			return s;
	}
	return dev->write_subdev;
}

static void comedi_file_reset(struct file *file)
{
	struct comedi_file *cfp = file->private_data;
	struct comedi_device *dev = cfp->dev;
	struct comedi_subdevice *s, *read_s, *write_s;
	unsigned int minor = iminor(file_inode(file));

	read_s = dev->read_subdev;
	write_s = dev->write_subdev;
	if (minor >= COMEDI_NUM_BOARD_MINORS) {
		s = comedi_subdevice_from_minor(dev, minor);
		if (!s || s->subdev_flags & SDF_CMD_READ)
			read_s = s;
		if (!s || s->subdev_flags & SDF_CMD_WRITE)
			write_s = s;
	}
	cfp->last_attached = dev->attached;
	cfp->last_detach_count = dev->detach_count;
	WRITE_ONCE(cfp->read_subdev, read_s);
	WRITE_ONCE(cfp->write_subdev, write_s);
}

static void comedi_file_check(struct file *file)
{
	struct comedi_file *cfp = file->private_data;
	struct comedi_device *dev = cfp->dev;

	if (cfp->last_attached != dev->attached ||
	    cfp->last_detach_count != dev->detach_count)
		comedi_file_reset(file);
}

static struct comedi_subdevice *comedi_file_read_subdevice(struct file *file)
{
	struct comedi_file *cfp = file->private_data;

	comedi_file_check(file);
	return READ_ONCE(cfp->read_subdev);
}

static struct comedi_subdevice *comedi_file_write_subdevice(struct file *file)
{
	struct comedi_file *cfp = file->private_data;

	comedi_file_check(file);
	return READ_ONCE(cfp->write_subdev);
}

static int resize_async_buffer(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       unsigned int new_size)
{
	struct comedi_async *async = s->async;
	int retval;

	lockdep_assert_held(&dev->mutex);

	if (new_size > async->max_bufsize)
		return -EPERM;

	if (s->busy) {
		dev_dbg(dev->class_dev,
			"subdevice is busy, cannot resize buffer\n");
		return -EBUSY;
	}
	if (comedi_buf_is_mmapped(s)) {
		dev_dbg(dev->class_dev,
			"subdevice is mmapped, cannot resize buffer\n");
		return -EBUSY;
	}

	/* make sure buffer is an integral number of pages (we round up) */
	new_size = (new_size + PAGE_SIZE - 1) & PAGE_MASK;

	retval = comedi_buf_alloc(dev, s, new_size);
	if (retval < 0)
		return retval;

	if (s->buf_change) {
		retval = s->buf_change(dev, s);
		if (retval < 0)
			return retval;
	}

	dev_dbg(dev->class_dev, "subd %d buffer resized to %i bytes\n",
		s->index, async->prealloc_bufsz);
	return 0;
}

/* sysfs attribute files */

static ssize_t max_read_buffer_kb_show(struct device *csdev,
				       struct device_attribute *attr, char *buf)
{
	unsigned int minor = MINOR(csdev->devt);
	struct comedi_device *dev;
	struct comedi_subdevice *s;
	unsigned int size = 0;

	dev = comedi_dev_get_from_minor(minor);
	if (!dev)
		return -ENODEV;

	mutex_lock(&dev->mutex);
	s = comedi_read_subdevice(dev, minor);
	if (s && (s->subdev_flags & SDF_CMD_READ) && s->async)
		size = s->async->max_bufsize / 1024;
	mutex_unlock(&dev->mutex);

	comedi_dev_put(dev);
	return snprintf(buf, PAGE_SIZE, "%u\n", size);
}

static ssize_t max_read_buffer_kb_store(struct device *csdev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	unsigned int minor = MINOR(csdev->devt);
	struct comedi_device *dev;
	struct comedi_subdevice *s;
	unsigned int size;
	int err;

	err = kstrtouint(buf, 10, &size);
	if (err)
		return err;
	if (size > (UINT_MAX / 1024))
		return -EINVAL;
	size *= 1024;

	dev = comedi_dev_get_from_minor(minor);
	if (!dev)
		return -ENODEV;

	mutex_lock(&dev->mutex);
	s = comedi_read_subdevice(dev, minor);
	if (s && (s->subdev_flags & SDF_CMD_READ) && s->async)
		s->async->max_bufsize = size;
	else
		err = -EINVAL;
	mutex_unlock(&dev->mutex);

	comedi_dev_put(dev);
	return err ? err : count;
}
static DEVICE_ATTR_RW(max_read_buffer_kb);

static ssize_t read_buffer_kb_show(struct device *csdev,
				   struct device_attribute *attr, char *buf)
{
	unsigned int minor = MINOR(csdev->devt);
	struct comedi_device *dev;
	struct comedi_subdevice *s;
	unsigned int size = 0;

	dev = comedi_dev_get_from_minor(minor);
	if (!dev)
		return -ENODEV;

	mutex_lock(&dev->mutex);
	s = comedi_read_subdevice(dev, minor);
	if (s && (s->subdev_flags & SDF_CMD_READ) && s->async)
		size = s->async->prealloc_bufsz / 1024;
	mutex_unlock(&dev->mutex);

	comedi_dev_put(dev);
	return snprintf(buf, PAGE_SIZE, "%u\n", size);
}

static ssize_t read_buffer_kb_store(struct device *csdev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
	unsigned int minor = MINOR(csdev->devt);
	struct comedi_device *dev;
	struct comedi_subdevice *s;
	unsigned int size;
	int err;

	err = kstrtouint(buf, 10, &size);
	if (err)
		return err;
	if (size > (UINT_MAX / 1024))
		return -EINVAL;
	size *= 1024;

	dev = comedi_dev_get_from_minor(minor);
	if (!dev)
		return -ENODEV;

	mutex_lock(&dev->mutex);
	s = comedi_read_subdevice(dev, minor);
	if (s && (s->subdev_flags & SDF_CMD_READ) && s->async)
		err = resize_async_buffer(dev, s, size);
	else
		err = -EINVAL;
	mutex_unlock(&dev->mutex);

	comedi_dev_put(dev);
	return err ? err : count;
}
static DEVICE_ATTR_RW(read_buffer_kb);

static ssize_t max_write_buffer_kb_show(struct device *csdev,
					struct device_attribute *attr,
					char *buf)
{
	unsigned int minor = MINOR(csdev->devt);
	struct comedi_device *dev;
	struct comedi_subdevice *s;
	unsigned int size = 0;

	dev = comedi_dev_get_from_minor(minor);
	if (!dev)
		return -ENODEV;

	mutex_lock(&dev->mutex);
	s = comedi_write_subdevice(dev, minor);
	if (s && (s->subdev_flags & SDF_CMD_WRITE) && s->async)
		size = s->async->max_bufsize / 1024;
	mutex_unlock(&dev->mutex);

	comedi_dev_put(dev);
	return snprintf(buf, PAGE_SIZE, "%u\n", size);
}

static ssize_t max_write_buffer_kb_store(struct device *csdev,
					 struct device_attribute *attr,
					 const char *buf, size_t count)
{
	unsigned int minor = MINOR(csdev->devt);
	struct comedi_device *dev;
	struct comedi_subdevice *s;
	unsigned int size;
	int err;

	err = kstrtouint(buf, 10, &size);
	if (err)
		return err;
	if (size > (UINT_MAX / 1024))
		return -EINVAL;
	size *= 1024;

	dev = comedi_dev_get_from_minor(minor);
	if (!dev)
		return -ENODEV;

	mutex_lock(&dev->mutex);
	s = comedi_write_subdevice(dev, minor);
	if (s && (s->subdev_flags & SDF_CMD_WRITE) && s->async)
		s->async->max_bufsize = size;
	else
		err = -EINVAL;
	mutex_unlock(&dev->mutex);

	comedi_dev_put(dev);
	return err ? err : count;
}
static DEVICE_ATTR_RW(max_write_buffer_kb);

static ssize_t write_buffer_kb_show(struct device *csdev,
				    struct device_attribute *attr, char *buf)
{
	unsigned int minor = MINOR(csdev->devt);
	struct comedi_device *dev;
	struct comedi_subdevice *s;
	unsigned int size = 0;

	dev = comedi_dev_get_from_minor(minor);
	if (!dev)
		return -ENODEV;

	mutex_lock(&dev->mutex);
	s = comedi_write_subdevice(dev, minor);
	if (s && (s->subdev_flags & SDF_CMD_WRITE) && s->async)
		size = s->async->prealloc_bufsz / 1024;
	mutex_unlock(&dev->mutex);

	comedi_dev_put(dev);
	return snprintf(buf, PAGE_SIZE, "%u\n", size);
}

static ssize_t write_buffer_kb_store(struct device *csdev,
				     struct device_attribute *attr,
				     const char *buf, size_t count)
{
	unsigned int minor = MINOR(csdev->devt);
	struct comedi_device *dev;
	struct comedi_subdevice *s;
	unsigned int size;
	int err;

	err = kstrtouint(buf, 10, &size);
	if (err)
		return err;
	if (size > (UINT_MAX / 1024))
		return -EINVAL;
	size *= 1024;

	dev = comedi_dev_get_from_minor(minor);
	if (!dev)
		return -ENODEV;

	mutex_lock(&dev->mutex);
	s = comedi_write_subdevice(dev, minor);
	if (s && (s->subdev_flags & SDF_CMD_WRITE) && s->async)
		err = resize_async_buffer(dev, s, size);
	else
		err = -EINVAL;
	mutex_unlock(&dev->mutex);

	comedi_dev_put(dev);
	return err ? err : count;
}
static DEVICE_ATTR_RW(write_buffer_kb);

static struct attribute *comedi_dev_attrs[] = {
	&dev_attr_max_read_buffer_kb.attr,
	&dev_attr_read_buffer_kb.attr,
	&dev_attr_max_write_buffer_kb.attr,
	&dev_attr_write_buffer_kb.attr,
	NULL,
};
ATTRIBUTE_GROUPS(comedi_dev);

static void __comedi_clear_subdevice_runflags(struct comedi_subdevice *s,
					      unsigned int bits)
{
	s->runflags &= ~bits;
}

static void __comedi_set_subdevice_runflags(struct comedi_subdevice *s,
					    unsigned int bits)
{
	s->runflags |= bits;
}

static void comedi_update_subdevice_runflags(struct comedi_subdevice *s,
					     unsigned int mask,
					     unsigned int bits)
{
	unsigned long flags;

	spin_lock_irqsave(&s->spin_lock, flags);
	__comedi_clear_subdevice_runflags(s, mask);
	__comedi_set_subdevice_runflags(s, bits & mask);
	spin_unlock_irqrestore(&s->spin_lock, flags);
}

static unsigned int __comedi_get_subdevice_runflags(struct comedi_subdevice *s)
{
	return s->runflags;
}

static unsigned int comedi_get_subdevice_runflags(struct comedi_subdevice *s)
{
	unsigned long flags;
	unsigned int runflags;

	spin_lock_irqsave(&s->spin_lock, flags);
	runflags = __comedi_get_subdevice_runflags(s);
	spin_unlock_irqrestore(&s->spin_lock, flags);
	return runflags;
}

static bool comedi_is_runflags_running(unsigned int runflags)
{
	return runflags & COMEDI_SRF_RUNNING;
}

static bool comedi_is_runflags_in_error(unsigned int runflags)
{
	return runflags & COMEDI_SRF_ERROR;
}

/**
 * comedi_is_subdevice_running() - Check if async command running on subdevice
 * @s: COMEDI subdevice.
 *
 * Return: %true if an asynchronous COMEDI command is active on the
 * subdevice, else %false.
 */
bool comedi_is_subdevice_running(struct comedi_subdevice *s)
{
	unsigned int runflags = comedi_get_subdevice_runflags(s);

	return comedi_is_runflags_running(runflags);
}
EXPORT_SYMBOL_GPL(comedi_is_subdevice_running);

static bool __comedi_is_subdevice_running(struct comedi_subdevice *s)
{
	unsigned int runflags = __comedi_get_subdevice_runflags(s);

	return comedi_is_runflags_running(runflags);
}

bool comedi_can_auto_free_spriv(struct comedi_subdevice *s)
{
	unsigned int runflags = __comedi_get_subdevice_runflags(s);

	return runflags & COMEDI_SRF_FREE_SPRIV;
}

/**
 * comedi_set_spriv_auto_free() - Mark subdevice private data as freeable
 * @s: COMEDI subdevice.
 *
 * Mark the subdevice as having a pointer to private data that can be
 * automatically freed when the COMEDI device is detached from the low-level
 * driver.
 */
void comedi_set_spriv_auto_free(struct comedi_subdevice *s)
{
	__comedi_set_subdevice_runflags(s, COMEDI_SRF_FREE_SPRIV);
}
EXPORT_SYMBOL_GPL(comedi_set_spriv_auto_free);

/**
 * comedi_alloc_spriv - Allocate memory for the subdevice private data
 * @s: COMEDI subdevice.
 * @size: Size of the memory to allocate.
 *
 * Allocate memory for the subdevice private data and point @s->private
 * to it.  The memory will be freed automatically when the COMEDI device
 * is detached from the low-level driver.
 *
 * Return: A pointer to the allocated memory @s->private on success.
 * Return NULL on failure.
 */
void *comedi_alloc_spriv(struct comedi_subdevice *s, size_t size)
{
	s->private = kzalloc(size, GFP_KERNEL);
	if (s->private)
		comedi_set_spriv_auto_free(s);
	return s->private;
}
EXPORT_SYMBOL_GPL(comedi_alloc_spriv);

/*
 * This function restores a subdevice to an idle state.
 */
static void do_become_nonbusy(struct comedi_device *dev,
			      struct comedi_subdevice *s)
{
	struct comedi_async *async = s->async;

	lockdep_assert_held(&dev->mutex);
	comedi_update_subdevice_runflags(s, COMEDI_SRF_RUNNING, 0);
	if (async) {
		comedi_buf_reset(s);
		async->inttrig = NULL;
		kfree(async->cmd.chanlist);
		async->cmd.chanlist = NULL;
		s->busy = NULL;
		wake_up_interruptible_all(&async->wait_head);
	} else {
		dev_err(dev->class_dev,
			"BUG: (?) %s called with async=NULL\n", __func__);
		s->busy = NULL;
	}
}

static int do_cancel(struct comedi_device *dev, struct comedi_subdevice *s)
{
	int ret = 0;

	lockdep_assert_held(&dev->mutex);
	if (comedi_is_subdevice_running(s) && s->cancel)
		ret = s->cancel(dev, s);

	do_become_nonbusy(dev, s);

	return ret;
}

void comedi_device_cancel_all(struct comedi_device *dev)
{
	struct comedi_subdevice *s;
	int i;

	lockdep_assert_held(&dev->mutex);
	if (!dev->attached)
		return;

	for (i = 0; i < dev->n_subdevices; i++) {
		s = &dev->subdevices[i];
		if (s->async)
			do_cancel(dev, s);
	}
}

static int is_device_busy(struct comedi_device *dev)
{
	struct comedi_subdevice *s;
	int i;

	lockdep_assert_held(&dev->mutex);
	if (!dev->attached)
		return 0;

	for (i = 0; i < dev->n_subdevices; i++) {
		s = &dev->subdevices[i];
		if (s->busy)
			return 1;
		if (s->async && comedi_buf_is_mmapped(s))
			return 1;
	}

	return 0;
}

/*
 * COMEDI_DEVCONFIG ioctl
 * attaches (and configures) or detaches a legacy device
 *
 * arg:
 *	pointer to comedi_devconfig structure (NULL if detaching)
 *
 * reads:
 *	comedi_devconfig structure (if attaching)
 *
 * writes:
 *	nothing
 */
static int do_devconfig_ioctl(struct comedi_device *dev,
			      struct comedi_devconfig __user *arg)
{
	struct comedi_devconfig it;

	lockdep_assert_held(&dev->mutex);
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (!arg) {
		if (is_device_busy(dev))
			return -EBUSY;
		if (dev->attached) {
			struct module *driver_module = dev->driver->module;

			comedi_device_detach(dev);
			module_put(driver_module);
		}
		return 0;
	}

	if (copy_from_user(&it, arg, sizeof(it)))
		return -EFAULT;

	it.board_name[COMEDI_NAMELEN - 1] = 0;

	if (it.options[COMEDI_DEVCONF_AUX_DATA_LENGTH]) {
		dev_warn(dev->class_dev,
			 "comedi_config --init_data is deprecated\n");
		return -EINVAL;
	}

	if (dev->minor >= comedi_num_legacy_minors)
		/* don't re-use dynamically allocated comedi devices */
		return -EBUSY;

	/* This increments the driver module count on success. */
	return comedi_device_attach(dev, &it);
}

/*
 * COMEDI_BUFCONFIG ioctl
 * buffer configuration
 *
 * arg:
 *	pointer to comedi_bufconfig structure
 *
 * reads:
 *	comedi_bufconfig structure
 *
 * writes:
 *	modified comedi_bufconfig structure
 */
static int do_bufconfig_ioctl(struct comedi_device *dev,
			      struct comedi_bufconfig __user *arg)
{
	struct comedi_bufconfig bc;
	struct comedi_async *async;
	struct comedi_subdevice *s;
	int retval = 0;

	lockdep_assert_held(&dev->mutex);
	if (copy_from_user(&bc, arg, sizeof(bc)))
		return -EFAULT;

	if (bc.subdevice >= dev->n_subdevices)
		return -EINVAL;

	s = &dev->subdevices[bc.subdevice];
	async = s->async;

	if (!async) {
		dev_dbg(dev->class_dev,
			"subdevice does not have async capability\n");
		bc.size = 0;
		bc.maximum_size = 0;
		goto copyback;
	}

	if (bc.maximum_size) {
		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;

		async->max_bufsize = bc.maximum_size;
	}

	if (bc.size) {
		retval = resize_async_buffer(dev, s, bc.size);
		if (retval < 0)
			return retval;
	}

	bc.size = async->prealloc_bufsz;
	bc.maximum_size = async->max_bufsize;

copyback:
	if (copy_to_user(arg, &bc, sizeof(bc)))
		return -EFAULT;

	return 0;
}

/*
 * COMEDI_DEVINFO ioctl
 * device info
 *
 * arg:
 *	pointer to comedi_devinfo structure
 *
 * reads:
 *	nothing
 *
 * writes:
 *	comedi_devinfo structure
 */
static int do_devinfo_ioctl(struct comedi_device *dev,
			    struct comedi_devinfo __user *arg,
			    struct file *file)
{
	struct comedi_subdevice *s;
	struct comedi_devinfo devinfo;

	lockdep_assert_held(&dev->mutex);
	memset(&devinfo, 0, sizeof(devinfo));

	/* fill devinfo structure */
	devinfo.version_code = COMEDI_VERSION_CODE;
	devinfo.n_subdevs = dev->n_subdevices;
	strscpy(devinfo.driver_name, dev->driver->driver_name, COMEDI_NAMELEN);
	strscpy(devinfo.board_name, dev->board_name, COMEDI_NAMELEN);

	s = comedi_file_read_subdevice(file);
	if (s)
		devinfo.read_subdevice = s->index;
	else
		devinfo.read_subdevice = -1;

	s = comedi_file_write_subdevice(file);
	if (s)
		devinfo.write_subdevice = s->index;
	else
		devinfo.write_subdevice = -1;

	if (copy_to_user(arg, &devinfo, sizeof(devinfo)))
		return -EFAULT;

	return 0;
}

/*
 * COMEDI_SUBDINFO ioctl
 * subdevices info
 *
 * arg:
 *	pointer to array of comedi_subdinfo structures
 *
 * reads:
 *	nothing
 *
 * writes:
 *	array of comedi_subdinfo structures
 */
static int do_subdinfo_ioctl(struct comedi_device *dev,
			     struct comedi_subdinfo __user *arg, void *file)
{
	int ret, i;
	struct comedi_subdinfo *tmp, *us;
	struct comedi_subdevice *s;

	lockdep_assert_held(&dev->mutex);
	tmp = kcalloc(dev->n_subdevices, sizeof(*tmp), GFP_KERNEL);
	if (!tmp)
		return -ENOMEM;

	/* fill subdinfo structs */
	for (i = 0; i < dev->n_subdevices; i++) {
		s = &dev->subdevices[i];
		us = tmp + i;

		us->type = s->type;
		us->n_chan = s->n_chan;
		us->subd_flags = s->subdev_flags;
		if (comedi_is_subdevice_running(s))
			us->subd_flags |= SDF_RUNNING;
#define TIMER_nanosec 5		/* backwards compatibility */
		us->timer_type = TIMER_nanosec;
		us->len_chanlist = s->len_chanlist;
		us->maxdata = s->maxdata;
		if (s->range_table) {
			us->range_type =
			    (i << 24) | (0 << 16) | (s->range_table->length);
		} else {
			us->range_type = 0;	/* XXX */
		}

		if (s->busy)
			us->subd_flags |= SDF_BUSY;
		if (s->busy == file)
			us->subd_flags |= SDF_BUSY_OWNER;
		if (s->lock)
			us->subd_flags |= SDF_LOCKED;
		if (s->lock == file)
			us->subd_flags |= SDF_LOCK_OWNER;
		if (!s->maxdata && s->maxdata_list)
			us->subd_flags |= SDF_MAXDATA;
		if (s->range_table_list)
			us->subd_flags |= SDF_RANGETYPE;
		if (s->do_cmd)
			us->subd_flags |= SDF_CMD;

		if (s->insn_bits != &insn_inval)
			us->insn_bits_support = COMEDI_SUPPORTED;
		else
			us->insn_bits_support = COMEDI_UNSUPPORTED;
	}

	ret = copy_to_user(arg, tmp, dev->n_subdevices * sizeof(*tmp));

	kfree(tmp);

	return ret ? -EFAULT : 0;
}

/*
 * COMEDI_CHANINFO ioctl
 * subdevice channel info
 *
 * arg:
 *	pointer to comedi_chaninfo structure
 *
 * reads:
 *	comedi_chaninfo structure
 *
 * writes:
 *	array of maxdata values to chaninfo->maxdata_list if requested
 *	array of range table lengths to chaninfo->range_table_list if requested
 */
static int do_chaninfo_ioctl(struct comedi_device *dev,
			     struct comedi_chaninfo *it)
{
	struct comedi_subdevice *s;

	lockdep_assert_held(&dev->mutex);

	if (it->subdev >= dev->n_subdevices)
		return -EINVAL;
	s = &dev->subdevices[it->subdev];

	if (it->maxdata_list) {
		if (s->maxdata || !s->maxdata_list)
			return -EINVAL;
		if (copy_to_user(it->maxdata_list, s->maxdata_list,
				 s->n_chan * sizeof(unsigned int)))
			return -EFAULT;
	}

	if (it->flaglist)
		return -EINVAL;	/* flaglist not supported */

	if (it->rangelist) {
		int i;

		if (!s->range_table_list)
			return -EINVAL;
		for (i = 0; i < s->n_chan; i++) {
			int x;

			x = (dev->minor << 28) | (it->subdev << 24) | (i << 16) |
			    (s->range_table_list[i]->length);
			if (put_user(x, it->rangelist + i))
				return -EFAULT;
		}
	}

	return 0;
}

/*
 * COMEDI_BUFINFO ioctl
 * buffer information
 *
 * arg:
 *	pointer to comedi_bufinfo structure
 *
 * reads:
 *	comedi_bufinfo structure
 *
 * writes:
 *	modified comedi_bufinfo structure
 */
static int do_bufinfo_ioctl(struct comedi_device *dev,
			    struct comedi_bufinfo __user *arg, void *file)
{
	struct comedi_bufinfo bi;
	struct comedi_subdevice *s;
	struct comedi_async *async;
	unsigned int runflags;
	int retval = 0;
	bool become_nonbusy = false;

	lockdep_assert_held(&dev->mutex);
	if (copy_from_user(&bi, arg, sizeof(bi)))
		return -EFAULT;

	if (bi.subdevice >= dev->n_subdevices)
		return -EINVAL;

	s = &dev->subdevices[bi.subdevice];

	async = s->async;

	if (!async || s->busy != file)
		return -EINVAL;

	runflags = comedi_get_subdevice_runflags(s);
	if (!(async->cmd.flags & CMDF_WRITE)) {
		/* command was set up in "read" direction */
		if (bi.bytes_read) {
			comedi_buf_read_alloc(s, bi.bytes_read);
			bi.bytes_read = comedi_buf_read_free(s, bi.bytes_read);
		}
		/*
		 * If nothing left to read, and command has stopped, and
		 * {"read" position not updated or command stopped normally},
		 * then become non-busy.
		 */
		if (comedi_buf_read_n_available(s) == 0 &&
		    !comedi_is_runflags_running(runflags) &&
		    (bi.bytes_read == 0 ||
		     !comedi_is_runflags_in_error(runflags))) {
			become_nonbusy = true;
			if (comedi_is_runflags_in_error(runflags))
				retval = -EPIPE;
		}
		bi.bytes_written = 0;
	} else {
		/* command was set up in "write" direction */
		if (!comedi_is_runflags_running(runflags)) {
			bi.bytes_written = 0;
			become_nonbusy = true;
			if (comedi_is_runflags_in_error(runflags))
				retval = -EPIPE;
		} else if (bi.bytes_written) {
			comedi_buf_write_alloc(s, bi.bytes_written);
			bi.bytes_written =
			    comedi_buf_write_free(s, bi.bytes_written);
		}
		bi.bytes_read = 0;
	}

	bi.buf_write_count = async->buf_write_count;
	bi.buf_write_ptr = async->buf_write_ptr;
	bi.buf_read_count = async->buf_read_count;
	bi.buf_read_ptr = async->buf_read_ptr;

	if (become_nonbusy)
		do_become_nonbusy(dev, s);

	if (retval)
		return retval;

	if (copy_to_user(arg, &bi, sizeof(bi)))
		return -EFAULT;

	return 0;
}

static int check_insn_config_length(struct comedi_insn *insn,
				    unsigned int *data)
{
	if (insn->n < 1)
		return -EINVAL;

	switch (data[0]) {
	case INSN_CONFIG_DIO_OUTPUT:
	case INSN_CONFIG_DIO_INPUT:
	case INSN_CONFIG_DISARM:
	case INSN_CONFIG_RESET:
		if (insn->n == 1)
			return 0;
		break;
	case INSN_CONFIG_ARM:
	case INSN_CONFIG_DIO_QUERY:
	case INSN_CONFIG_BLOCK_SIZE:
	case INSN_CONFIG_FILTER:
	case INSN_CONFIG_SERIAL_CLOCK:
	case INSN_CONFIG_BIDIRECTIONAL_DATA:
	case INSN_CONFIG_ALT_SOURCE:
	case INSN_CONFIG_SET_COUNTER_MODE:
	case INSN_CONFIG_8254_READ_STATUS:
	case INSN_CONFIG_SET_ROUTING:
	case INSN_CONFIG_GET_ROUTING:
	case INSN_CONFIG_GET_PWM_STATUS:
	case INSN_CONFIG_PWM_SET_PERIOD:
	case INSN_CONFIG_PWM_GET_PERIOD:
		if (insn->n == 2)
			return 0;
		break;
	case INSN_CONFIG_SET_GATE_SRC:
	case INSN_CONFIG_GET_GATE_SRC:
	case INSN_CONFIG_SET_CLOCK_SRC:
	case INSN_CONFIG_GET_CLOCK_SRC:
	case INSN_CONFIG_SET_OTHER_SRC:
	case INSN_CONFIG_GET_COUNTER_STATUS:
	case INSN_CONFIG_PWM_SET_H_BRIDGE:
	case INSN_CONFIG_PWM_GET_H_BRIDGE:
	case INSN_CONFIG_GET_HARDWARE_BUFFER_SIZE:
		if (insn->n == 3)
			return 0;
		break;
	case INSN_CONFIG_PWM_OUTPUT:
	case INSN_CONFIG_ANALOG_TRIG:
	case INSN_CONFIG_TIMER_1:
		if (insn->n == 5)
			return 0;
		break;
	case INSN_CONFIG_DIGITAL_TRIG:
		if (insn->n == 6)
			return 0;
		break;
	case INSN_CONFIG_GET_CMD_TIMING_CONSTRAINTS:
		if (insn->n >= 4)
			return 0;
		break;
		/*
		 * by default we allow the insn since we don't have checks for
		 * all possible cases yet
		 */
	default:
		pr_warn("No check for data length of config insn id %i is implemented\n",
			data[0]);
		pr_warn("Add a check to %s in %s\n", __func__, __FILE__);
		pr_warn("Assuming n=%i is correct\n", insn->n);
		return 0;
	}
	return -EINVAL;
}

static int check_insn_device_config_length(struct comedi_insn *insn,
					   unsigned int *data)
{
	if (insn->n < 1)
		return -EINVAL;

	switch (data[0]) {
	case INSN_DEVICE_CONFIG_TEST_ROUTE:
	case INSN_DEVICE_CONFIG_CONNECT_ROUTE:
	case INSN_DEVICE_CONFIG_DISCONNECT_ROUTE:
		if (insn->n == 3)
			return 0;
		break;
	case INSN_DEVICE_CONFIG_GET_ROUTES:
		/*
		 * Big enough for config_id and the length of the userland
		 * memory buffer.  Additional length should be in factors of 2
		 * to communicate any returned route pairs (source,destination).
		 */
		if (insn->n >= 2)
			return 0;
		break;
	}
	return -EINVAL;
}

/**
 * get_valid_routes() - Calls low-level driver get_valid_routes function to
 *			either return a count of valid routes to user, or copy
 *			of list of all valid device routes to buffer in
 *			userspace.
 * @dev: comedi device pointer
 * @data: data from user insn call.  The length of the data must be >= 2.
 *	  data[0] must contain the INSN_DEVICE_CONFIG config_id.
 *	  data[1](input) contains the number of _pairs_ for which memory is
 *		  allotted from the user.  If the user specifies '0', then only
 *		  the number of pairs available is returned.
 *	  data[1](output) returns either the number of pairs available (if none
 *		  where requested) or the number of _pairs_ that are copied back
 *		  to the user.
 *	  data[2::2] returns each (source, destination) pair.
 *
 * Return: -EINVAL if low-level driver does not allocate and return routes as
 *	   expected.  Returns 0 otherwise.
 */
static int get_valid_routes(struct comedi_device *dev, unsigned int *data)
{
	lockdep_assert_held(&dev->mutex);
	data[1] = dev->get_valid_routes(dev, data[1], data + 2);
	return 0;
}

static int parse_insn(struct comedi_device *dev, struct comedi_insn *insn,
		      unsigned int *data, void *file)
{
	struct comedi_subdevice *s;
	int ret = 0;
	int i;

	lockdep_assert_held(&dev->mutex);
	if (insn->insn & INSN_MASK_SPECIAL) {
		/* a non-subdevice instruction */

		switch (insn->insn) {
		case INSN_GTOD:
			{
				struct timespec64 tv;

				if (insn->n != 2) {
					ret = -EINVAL;
					break;
				}

				ktime_get_real_ts64(&tv);
				/* unsigned data safe until 2106 */
				data[0] = (unsigned int)tv.tv_sec;
				data[1] = tv.tv_nsec / NSEC_PER_USEC;
				ret = 2;

				break;
			}
		case INSN_WAIT:
			if (insn->n != 1 || data[0] >= 100000) {
				ret = -EINVAL;
				break;
			}
			udelay(data[0] / 1000);
			ret = 1;
			break;
		case INSN_INTTRIG:
			if (insn->n != 1) {
				ret = -EINVAL;
				break;
			}
			if (insn->subdev >= dev->n_subdevices) {
				dev_dbg(dev->class_dev,
					"%d not usable subdevice\n",
					insn->subdev);
				ret = -EINVAL;
				break;
			}
			s = &dev->subdevices[insn->subdev];
			if (!s->async) {
				dev_dbg(dev->class_dev, "no async\n");
				ret = -EINVAL;
				break;
			}
			if (!s->async->inttrig) {
				dev_dbg(dev->class_dev, "no inttrig\n");
				ret = -EAGAIN;
				break;
			}
			ret = s->async->inttrig(dev, s, data[0]);
			if (ret >= 0)
				ret = 1;
			break;
		case INSN_DEVICE_CONFIG:
			ret = check_insn_device_config_length(insn, data);
			if (ret)
				break;

			if (data[0] == INSN_DEVICE_CONFIG_GET_ROUTES) {
				/*
				 * data[1] should be the number of _pairs_ that
				 * the memory can hold.
				 */
				data[1] = (insn->n - 2) / 2;
				ret = get_valid_routes(dev, data);
				break;
			}

			/* other global device config instructions. */
			ret = dev->insn_device_config(dev, insn, data);
			break;
		default:
			dev_dbg(dev->class_dev, "invalid insn\n");
			ret = -EINVAL;
			break;
		}
	} else {
		/* a subdevice instruction */
		unsigned int maxdata;

		if (insn->subdev >= dev->n_subdevices) {
			dev_dbg(dev->class_dev, "subdevice %d out of range\n",
				insn->subdev);
			ret = -EINVAL;
			goto out;
		}
		s = &dev->subdevices[insn->subdev];

		if (s->type == COMEDI_SUBD_UNUSED) {
			dev_dbg(dev->class_dev, "%d not usable subdevice\n",
				insn->subdev);
			ret = -EIO;
			goto out;
		}

		/* are we locked? (ioctl lock) */
		if (s->lock && s->lock != file) {
			dev_dbg(dev->class_dev, "device locked\n");
			ret = -EACCES;
			goto out;
		}

		ret = comedi_check_chanlist(s, 1, &insn->chanspec);
		if (ret < 0) {
			ret = -EINVAL;
			dev_dbg(dev->class_dev, "bad chanspec\n");
			goto out;
		}

		if (s->busy) {
			ret = -EBUSY;
			goto out;
		}
		/* This looks arbitrary.  It is. */
		s->busy = parse_insn;
		switch (insn->insn) {
		case INSN_READ:
			ret = s->insn_read(dev, s, insn, data);
			if (ret == -ETIMEDOUT) {
				dev_dbg(dev->class_dev,
					"subdevice %d read instruction timed out\n",
					s->index);
			}
			break;
		case INSN_WRITE:
			maxdata = s->maxdata_list
			    ? s->maxdata_list[CR_CHAN(insn->chanspec)]
			    : s->maxdata;
			for (i = 0; i < insn->n; ++i) {
				if (data[i] > maxdata) {
					ret = -EINVAL;
					dev_dbg(dev->class_dev,
						"bad data value(s)\n");
					break;
				}
			}
			if (ret == 0) {
				ret = s->insn_write(dev, s, insn, data);
				if (ret == -ETIMEDOUT) {
					dev_dbg(dev->class_dev,
						"subdevice %d write instruction timed out\n",
						s->index);
				}
			}
			break;
		case INSN_BITS:
			if (insn->n != 2) {
				ret = -EINVAL;
			} else {
				/*
				 * Most drivers ignore the base channel in
				 * insn->chanspec.  Fix this here if
				 * the subdevice has <= 32 channels.
				 */
				unsigned int orig_mask = data[0];
				unsigned int shift = 0;

				if (s->n_chan <= 32) {
					shift = CR_CHAN(insn->chanspec);
					if (shift > 0) {
						insn->chanspec = 0;
						data[0] <<= shift;
						data[1] <<= shift;
					}
				}
				ret = s->insn_bits(dev, s, insn, data);
				data[0] = orig_mask;
				if (shift > 0)
					data[1] >>= shift;
			}
			break;
		case INSN_CONFIG:
			ret = check_insn_config_length(insn, data);
			if (ret)
				break;
			ret = s->insn_config(dev, s, insn, data);
			break;
		default:
			ret = -EINVAL;
			break;
		}

		s->busy = NULL;
	}

out:
	return ret;
}

/*
 * COMEDI_INSNLIST ioctl
 * synchronous instruction list
 *
 * arg:
 *	pointer to comedi_insnlist structure
 *
 * reads:
 *	comedi_insnlist structure
 *	array of comedi_insn structures from insnlist->insns pointer
 *	data (for writes) from insns[].data pointers
 *
 * writes:
 *	data (for reads) to insns[].data pointers
 */
/* arbitrary limits */
#define MIN_SAMPLES 16
#define MAX_SAMPLES 65536
static int do_insnlist_ioctl(struct comedi_device *dev,
			     struct comedi_insn *insns,
			     unsigned int n_insns,
			     void *file)
{
	unsigned int *data = NULL;
	unsigned int max_n_data_required = MIN_SAMPLES;
	int i = 0;
	int ret = 0;

	lockdep_assert_held(&dev->mutex);

	/* Determine maximum memory needed for all instructions. */
	for (i = 0; i < n_insns; ++i) {
		if (insns[i].n > MAX_SAMPLES) {
			dev_dbg(dev->class_dev,
				"number of samples too large\n");
			ret = -EINVAL;
			goto error;
		}
		max_n_data_required = max(max_n_data_required, insns[i].n);
	}

	/* Allocate scratch space for all instruction data. */
	data = kmalloc_array(max_n_data_required, sizeof(unsigned int),
			     GFP_KERNEL);
	if (!data) {
		ret = -ENOMEM;
		goto error;
	}

	for (i = 0; i < n_insns; ++i) {
		if (insns[i].insn & INSN_MASK_WRITE) {
			if (copy_from_user(data, insns[i].data,
					   insns[i].n * sizeof(unsigned int))) {
				dev_dbg(dev->class_dev,
					"copy_from_user failed\n");
				ret = -EFAULT;
				goto error;
			}
		}
		ret = parse_insn(dev, insns + i, data, file);
		if (ret < 0)
			goto error;
		if (insns[i].insn & INSN_MASK_READ) {
			if (copy_to_user(insns[i].data, data,
					 insns[i].n * sizeof(unsigned int))) {
				dev_dbg(dev->class_dev,
					"copy_to_user failed\n");
				ret = -EFAULT;
				goto error;
			}
		}
		if (need_resched())
			schedule();
	}

error:
	kfree(data);

	if (ret < 0)
		return ret;
	return i;
}

/*
 * COMEDI_INSN ioctl
 * synchronous instruction
 *
 * arg:
 *	pointer to comedi_insn structure
 *
 * reads:
 *	comedi_insn structure
 *	data (for writes) from insn->data pointer
 *
 * writes:
 *	data (for reads) to insn->data pointer
 */
static int do_insn_ioctl(struct comedi_device *dev,
			 struct comedi_insn *insn, void *file)
{
	unsigned int *data = NULL;
	unsigned int n_data = MIN_SAMPLES;
	int ret = 0;

	lockdep_assert_held(&dev->mutex);

	n_data = max(n_data, insn->n);

	/* This is where the behavior of insn and insnlist deviate. */
	if (insn->n > MAX_SAMPLES) {
		insn->n = MAX_SAMPLES;
		n_data = MAX_SAMPLES;
	}

	data = kmalloc_array(n_data, sizeof(unsigned int), GFP_KERNEL);
	if (!data) {
		ret = -ENOMEM;
		goto error;
	}

	if (insn->insn & INSN_MASK_WRITE) {
		if (copy_from_user(data,
				   insn->data,
				   insn->n * sizeof(unsigned int))) {
			ret = -EFAULT;
			goto error;
		}
	}
	ret = parse_insn(dev, insn, data, file);
	if (ret < 0)
		goto error;
	if (insn->insn & INSN_MASK_READ) {
		if (copy_to_user(insn->data,
				 data,
				 insn->n * sizeof(unsigned int))) {
			ret = -EFAULT;
			goto error;
		}
	}
	ret = insn->n;

error:
	kfree(data);

	return ret;
}

static int __comedi_get_user_cmd(struct comedi_device *dev,
				 struct comedi_cmd *cmd)
{
	struct comedi_subdevice *s;

	lockdep_assert_held(&dev->mutex);
	if (cmd->subdev >= dev->n_subdevices) {
		dev_dbg(dev->class_dev, "%d no such subdevice\n", cmd->subdev);
		return -ENODEV;
	}

	s = &dev->subdevices[cmd->subdev];

	if (s->type == COMEDI_SUBD_UNUSED) {
		dev_dbg(dev->class_dev, "%d not valid subdevice\n",
			cmd->subdev);
		return -EIO;
	}

	if (!s->do_cmd || !s->do_cmdtest || !s->async) {
		dev_dbg(dev->class_dev,
			"subdevice %d does not support commands\n",
			cmd->subdev);
		return -EIO;
	}

	/* make sure channel/gain list isn't too long */
	if (cmd->chanlist_len > s->len_chanlist) {
		dev_dbg(dev->class_dev, "channel/gain list too long %d > %d\n",
			cmd->chanlist_len, s->len_chanlist);
		return -EINVAL;
	}

	/*
	 * Set the CMDF_WRITE flag to the correct state if the subdevice
	 * supports only "read" commands or only "write" commands.
	 */
	switch (s->subdev_flags & (SDF_CMD_READ | SDF_CMD_WRITE)) {
	case SDF_CMD_READ:
		cmd->flags &= ~CMDF_WRITE;
		break;
	case SDF_CMD_WRITE:
		cmd->flags |= CMDF_WRITE;
		break;
	default:
		break;
	}

	return 0;
}

static int __comedi_get_user_chanlist(struct comedi_device *dev,
				      struct comedi_subdevice *s,
				      unsigned int __user *user_chanlist,
				      struct comedi_cmd *cmd)
{
	unsigned int *chanlist;
	int ret;

	lockdep_assert_held(&dev->mutex);
	cmd->chanlist = NULL;
	chanlist = memdup_user(user_chanlist,
			       cmd->chanlist_len * sizeof(unsigned int));
	if (IS_ERR(chanlist))
		return PTR_ERR(chanlist);

	/* make sure each element in channel/gain list is valid */
	ret = comedi_check_chanlist(s, cmd->chanlist_len, chanlist);
	if (ret < 0) {
		kfree(chanlist);
		return ret;
	}

	cmd->chanlist = chanlist;

	return 0;
}

/*
 * COMEDI_CMD ioctl
 * asynchronous acquisition command set-up
 *
 * arg:
 *	pointer to comedi_cmd structure
 *
 * reads:
 *	comedi_cmd structure
 *	channel/range list from cmd->chanlist pointer
 *
 * writes:
 *	possibly modified comedi_cmd structure (when -EAGAIN returned)
 */
static int do_cmd_ioctl(struct comedi_device *dev,
			struct comedi_cmd *cmd, bool *copy, void *file)
{
	struct comedi_subdevice *s;
	struct comedi_async *async;
	unsigned int __user *user_chanlist;
	int ret;

	lockdep_assert_held(&dev->mutex);

	/* do some simple cmd validation */
	ret = __comedi_get_user_cmd(dev, cmd);
	if (ret)
		return ret;

	/* save user's chanlist pointer so it can be restored later */
	user_chanlist = (unsigned int __user *)cmd->chanlist;

	s = &dev->subdevices[cmd->subdev];
	async = s->async;

	/* are we locked? (ioctl lock) */
	if (s->lock && s->lock != file) {
		dev_dbg(dev->class_dev, "subdevice locked\n");
		return -EACCES;
	}

	/* are we busy? */
	if (s->busy) {
		dev_dbg(dev->class_dev, "subdevice busy\n");
		return -EBUSY;
	}

	/* make sure channel/gain list isn't too short */
	if (cmd->chanlist_len < 1) {
		dev_dbg(dev->class_dev, "channel/gain list too short %u < 1\n",
			cmd->chanlist_len);
		return -EINVAL;
	}

	async->cmd = *cmd;
	async->cmd.data = NULL;

	/* load channel/gain list */
	ret = __comedi_get_user_chanlist(dev, s, user_chanlist, &async->cmd);
	if (ret)
		goto cleanup;

	ret = s->do_cmdtest(dev, s, &async->cmd);

	if (async->cmd.flags & CMDF_BOGUS || ret) {
		dev_dbg(dev->class_dev, "test returned %d\n", ret);
		*cmd = async->cmd;
		/* restore chanlist pointer before copying back */
		cmd->chanlist = (unsigned int __force *)user_chanlist;
		cmd->data = NULL;
		*copy = true;
		ret = -EAGAIN;
		goto cleanup;
	}

	if (!async->prealloc_bufsz) {
		ret = -ENOMEM;
		dev_dbg(dev->class_dev, "no buffer (?)\n");
		goto cleanup;
	}

	comedi_buf_reset(s);

	async->cb_mask = COMEDI_CB_BLOCK | COMEDI_CB_CANCEL_MASK;
	if (async->cmd.flags & CMDF_WAKE_EOS)
		async->cb_mask |= COMEDI_CB_EOS;

	comedi_update_subdevice_runflags(s, COMEDI_SRF_BUSY_MASK,
					 COMEDI_SRF_RUNNING);

	/*
	 * Set s->busy _after_ setting COMEDI_SRF_RUNNING flag to avoid
	 * race with comedi_read() or comedi_write().
	 */
	s->busy = file;
	ret = s->do_cmd(dev, s);
	if (ret == 0)
		return 0;

cleanup:
	do_become_nonbusy(dev, s);

	return ret;
}

/*
 * COMEDI_CMDTEST ioctl
 * asynchronous acquisition command testing
 *
 * arg:
 *	pointer to comedi_cmd structure
 *
 * reads:
 *	comedi_cmd structure
 *	channel/range list from cmd->chanlist pointer
 *
 * writes:
 *	possibly modified comedi_cmd structure
 */
static int do_cmdtest_ioctl(struct comedi_device *dev,
			    struct comedi_cmd *cmd, bool *copy, void *file)
{
	struct comedi_subdevice *s;
	unsigned int __user *user_chanlist;
	int ret;

	lockdep_assert_held(&dev->mutex);

	/* do some simple cmd validation */
	ret = __comedi_get_user_cmd(dev, cmd);
	if (ret)
		return ret;

	/* save user's chanlist pointer so it can be restored later */
	user_chanlist = (unsigned int __user *)cmd->chanlist;

	s = &dev->subdevices[cmd->subdev];

	/* user_chanlist can be NULL for COMEDI_CMDTEST ioctl */
	if (user_chanlist) {
		/* load channel/gain list */
		ret = __comedi_get_user_chanlist(dev, s, user_chanlist, cmd);
		if (ret)
			return ret;
	}

	ret = s->do_cmdtest(dev, s, cmd);

	kfree(cmd->chanlist);	/* free kernel copy of user chanlist */

	/* restore chanlist pointer before copying back */
	cmd->chanlist = (unsigned int __force *)user_chanlist;
	*copy = true;

	return ret;
}

/*
 * COMEDI_LOCK ioctl
 * lock subdevice
 *
 * arg:
 *	subdevice number
 *
 * reads:
 *	nothing
 *
 * writes:
 *	nothing
 */
static int do_lock_ioctl(struct comedi_device *dev, unsigned long arg,
			 void *file)
{
	int ret = 0;
	unsigned long flags;
	struct comedi_subdevice *s;

	lockdep_assert_held(&dev->mutex);
	if (arg >= dev->n_subdevices)
		return -EINVAL;
	s = &dev->subdevices[arg];

	spin_lock_irqsave(&s->spin_lock, flags);
	if (s->busy || s->lock)
		ret = -EBUSY;
	else
		s->lock = file;
	spin_unlock_irqrestore(&s->spin_lock, flags);

	return ret;
}

/*
 * COMEDI_UNLOCK ioctl
 * unlock subdevice
 *
 * arg:
 *	subdevice number
 *
 * reads:
 *	nothing
 *
 * writes:
 *	nothing
 */
static int do_unlock_ioctl(struct comedi_device *dev, unsigned long arg,
			   void *file)
{
	struct comedi_subdevice *s;

	lockdep_assert_held(&dev->mutex);
	if (arg >= dev->n_subdevices)
		return -EINVAL;
	s = &dev->subdevices[arg];

	if (s->busy)
		return -EBUSY;

	if (s->lock && s->lock != file)
		return -EACCES;

	if (s->lock == file)
		s->lock = NULL;

	return 0;
}

/*
 * COMEDI_CANCEL ioctl
 * cancel asynchronous acquisition
 *
 * arg:
 *	subdevice number
 *
 * reads:
 *	nothing
 *
 * writes:
 *	nothing
 */
static int do_cancel_ioctl(struct comedi_device *dev, unsigned long arg,
			   void *file)
{
	struct comedi_subdevice *s;

	lockdep_assert_held(&dev->mutex);
	if (arg >= dev->n_subdevices)
		return -EINVAL;
	s = &dev->subdevices[arg];
	if (!s->async)
		return -EINVAL;

	if (!s->busy)
		return 0;

	if (s->busy != file)
		return -EBUSY;

	return do_cancel(dev, s);
}

/*
 * COMEDI_POLL ioctl
 * instructs driver to synchronize buffers
 *
 * arg:
 *	subdevice number
 *
 * reads:
 *	nothing
 *
 * writes:
 *	nothing
 */
static int do_poll_ioctl(struct comedi_device *dev, unsigned long arg,
			 void *file)
{
	struct comedi_subdevice *s;

	lockdep_assert_held(&dev->mutex);
	if (arg >= dev->n_subdevices)
		return -EINVAL;
	s = &dev->subdevices[arg];

	if (!s->busy)
		return 0;

	if (s->busy != file)
		return -EBUSY;

	if (s->poll)
		return s->poll(dev, s);

	return -EINVAL;
}

/*
 * COMEDI_SETRSUBD ioctl
 * sets the current "read" subdevice on a per-file basis
 *
 * arg:
 *	subdevice number
 *
 * reads:
 *	nothing
 *
 * writes:
 *	nothing
 */
static int do_setrsubd_ioctl(struct comedi_device *dev, unsigned long arg,
			     struct file *file)
{
	struct comedi_file *cfp = file->private_data;
	struct comedi_subdevice *s_old, *s_new;

	lockdep_assert_held(&dev->mutex);
	if (arg >= dev->n_subdevices)
		return -EINVAL;

	s_new = &dev->subdevices[arg];
	s_old = comedi_file_read_subdevice(file);
	if (s_old == s_new)
		return 0;	/* no change */

	if (!(s_new->subdev_flags & SDF_CMD_READ))
		return -EINVAL;

	/*
	 * Check the file isn't still busy handling a "read" command on the
	 * old subdevice (if any).
	 */
	if (s_old && s_old->busy == file && s_old->async &&
	    !(s_old->async->cmd.flags & CMDF_WRITE))
		return -EBUSY;

	WRITE_ONCE(cfp->read_subdev, s_new);
	return 0;
}

/*
 * COMEDI_SETWSUBD ioctl
 * sets the current "write" subdevice on a per-file basis
 *
 * arg:
 *	subdevice number
 *
 * reads:
 *	nothing
 *
 * writes:
 *	nothing
 */
static int do_setwsubd_ioctl(struct comedi_device *dev, unsigned long arg,
			     struct file *file)
{
	struct comedi_file *cfp = file->private_data;
	struct comedi_subdevice *s_old, *s_new;

	lockdep_assert_held(&dev->mutex);
	if (arg >= dev->n_subdevices)
		return -EINVAL;

	s_new = &dev->subdevices[arg];
	s_old = comedi_file_write_subdevice(file);
	if (s_old == s_new)
		return 0;	/* no change */

	if (!(s_new->subdev_flags & SDF_CMD_WRITE))
		return -EINVAL;

	/*
	 * Check the file isn't still busy handling a "write" command on the
	 * old subdevice (if any).
	 */
	if (s_old && s_old->busy == file && s_old->async &&
	    (s_old->async->cmd.flags & CMDF_WRITE))
		return -EBUSY;

	WRITE_ONCE(cfp->write_subdev, s_new);
	return 0;
}

static long comedi_unlocked_ioctl(struct file *file, unsigned int cmd,
				  unsigned long arg)
{
	unsigned int minor = iminor(file_inode(file));
	struct comedi_file *cfp = file->private_data;
	struct comedi_device *dev = cfp->dev;
	int rc;

	mutex_lock(&dev->mutex);

	/*
	 * Device config is special, because it must work on
	 * an unconfigured device.
	 */
	if (cmd == COMEDI_DEVCONFIG) {
		if (minor >= COMEDI_NUM_BOARD_MINORS) {
			/* Device config not appropriate on non-board minors. */
			rc = -ENOTTY;
			goto done;
		}
		rc = do_devconfig_ioctl(dev,
					(struct comedi_devconfig __user *)arg);
		if (rc == 0) {
			if (arg == 0 &&
			    dev->minor >= comedi_num_legacy_minors) {
				/*
				 * Successfully unconfigured a dynamically
				 * allocated device.  Try and remove it.
				 */
				if (comedi_clear_board_dev(dev)) {
					mutex_unlock(&dev->mutex);
					comedi_free_board_dev(dev);
					return rc;
				}
			}
		}
		goto done;
	}

	if (!dev->attached) {
		dev_dbg(dev->class_dev, "no driver attached\n");
		rc = -ENODEV;
		goto done;
	}

	switch (cmd) {
	case COMEDI_BUFCONFIG:
		rc = do_bufconfig_ioctl(dev,
					(struct comedi_bufconfig __user *)arg);
		break;
	case COMEDI_DEVINFO:
		rc = do_devinfo_ioctl(dev, (struct comedi_devinfo __user *)arg,
				      file);
		break;
	case COMEDI_SUBDINFO:
		rc = do_subdinfo_ioctl(dev,
				       (struct comedi_subdinfo __user *)arg,
				       file);
		break;
	case COMEDI_CHANINFO: {
		struct comedi_chaninfo it;

		if (copy_from_user(&it, (void __user *)arg, sizeof(it)))
			rc = -EFAULT;
		else
			rc = do_chaninfo_ioctl(dev, &it);
		break;
	}
	case COMEDI_RANGEINFO: {
		struct comedi_rangeinfo it;

		if (copy_from_user(&it, (void __user *)arg, sizeof(it)))
			rc = -EFAULT;
		else
			rc = do_rangeinfo_ioctl(dev, &it);
		break;
	}
	case COMEDI_BUFINFO:
		rc = do_bufinfo_ioctl(dev,
				      (struct comedi_bufinfo __user *)arg,
				      file);
		break;
	case COMEDI_LOCK:
		rc = do_lock_ioctl(dev, arg, file);
		break;
	case COMEDI_UNLOCK:
		rc = do_unlock_ioctl(dev, arg, file);
		break;
	case COMEDI_CANCEL:
		rc = do_cancel_ioctl(dev, arg, file);
		break;
	case COMEDI_CMD: {
		struct comedi_cmd cmd;
		bool copy = false;

		if (copy_from_user(&cmd, (void __user *)arg, sizeof(cmd))) {
			rc = -EFAULT;
			break;
		}
		rc = do_cmd_ioctl(dev, &cmd, &copy, file);
		if (copy && copy_to_user((void __user *)arg, &cmd, sizeof(cmd)))
			rc = -EFAULT;
		break;
	}
	case COMEDI_CMDTEST: {
		struct comedi_cmd cmd;
		bool copy = false;

		if (copy_from_user(&cmd, (void __user *)arg, sizeof(cmd))) {
			rc = -EFAULT;
			break;
		}
		rc = do_cmdtest_ioctl(dev, &cmd, &copy, file);
		if (copy && copy_to_user((void __user *)arg, &cmd, sizeof(cmd)))
			rc = -EFAULT;
		break;
	}
	case COMEDI_INSNLIST: {
		struct comedi_insnlist insnlist;
		struct comedi_insn *insns = NULL;

		if (copy_from_user(&insnlist, (void __user *)arg,
				   sizeof(insnlist))) {
			rc = -EFAULT;
			break;
		}
		insns = kcalloc(insnlist.n_insns, sizeof(*insns), GFP_KERNEL);
		if (!insns) {
			rc = -ENOMEM;
			break;
		}
		if (copy_from_user(insns, insnlist.insns,
				   sizeof(*insns) * insnlist.n_insns)) {
			rc = -EFAULT;
			kfree(insns);
			break;
		}
		rc = do_insnlist_ioctl(dev, insns, insnlist.n_insns, file);
		kfree(insns);
		break;
	}
	case COMEDI_INSN: {
		struct comedi_insn insn;

		if (copy_from_user(&insn, (void __user *)arg, sizeof(insn)))
			rc = -EFAULT;
		else
			rc = do_insn_ioctl(dev, &insn, file);
		break;
	}
	case COMEDI_POLL:
		rc = do_poll_ioctl(dev, arg, file);
		break;
	case COMEDI_SETRSUBD:
		rc = do_setrsubd_ioctl(dev, arg, file);
		break;
	case COMEDI_SETWSUBD:
		rc = do_setwsubd_ioctl(dev, arg, file);
		break;
	default:
		rc = -ENOTTY;
		break;
	}

done:
	mutex_unlock(&dev->mutex);
	return rc;
}

static void comedi_vm_open(struct vm_area_struct *area)
{
	struct comedi_buf_map *bm;

	bm = area->vm_private_data;
	comedi_buf_map_get(bm);
}

static void comedi_vm_close(struct vm_area_struct *area)
{
	struct comedi_buf_map *bm;

	bm = area->vm_private_data;
	comedi_buf_map_put(bm);
}

static int comedi_vm_access(struct vm_area_struct *vma, unsigned long addr,
			    void *buf, int len, int write)
{
	struct comedi_buf_map *bm = vma->vm_private_data;
	unsigned long offset =
	    addr - vma->vm_start + (vma->vm_pgoff << PAGE_SHIFT);

	if (len < 0)
		return -EINVAL;
	if (len > vma->vm_end - addr)
		len = vma->vm_end - addr;
	return comedi_buf_map_access(bm, offset, buf, len, write);
}

static const struct vm_operations_struct comedi_vm_ops = {
	.open = comedi_vm_open,
	.close = comedi_vm_close,
	.access = comedi_vm_access,
};

static int comedi_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct comedi_file *cfp = file->private_data;
	struct comedi_device *dev = cfp->dev;
	struct comedi_subdevice *s;
	struct comedi_async *async;
	struct comedi_buf_map *bm = NULL;
	struct comedi_buf_page *buf;
	unsigned long start = vma->vm_start;
	unsigned long size;
	int n_pages;
	int i;
	int retval = 0;

	/*
	 * 'trylock' avoids circular dependency with current->mm->mmap_lock
	 * and down-reading &dev->attach_lock should normally succeed without
	 * contention unless the device is in the process of being attached
	 * or detached.
	 */
	if (!down_read_trylock(&dev->attach_lock))
		return -EAGAIN;

	if (!dev->attached) {
		dev_dbg(dev->class_dev, "no driver attached\n");
		retval = -ENODEV;
		goto done;
	}

	if (vma->vm_flags & VM_WRITE)
		s = comedi_file_write_subdevice(file);
	else
		s = comedi_file_read_subdevice(file);
	if (!s) {
		retval = -EINVAL;
		goto done;
	}

	async = s->async;
	if (!async) {
		retval = -EINVAL;
		goto done;
	}

	if (vma->vm_pgoff != 0) {
		dev_dbg(dev->class_dev, "mmap() offset must be 0.\n");
		retval = -EINVAL;
		goto done;
	}

	size = vma->vm_end - vma->vm_start;
	if (size > async->prealloc_bufsz) {
		retval = -EFAULT;
		goto done;
	}
	if (offset_in_page(size)) {
		retval = -EFAULT;
		goto done;
	}

	n_pages = vma_pages(vma);

	/* get reference to current buf map (if any) */
	bm = comedi_buf_map_from_subdev_get(s);
	if (!bm || n_pages > bm->n_pages) {
		retval = -EINVAL;
		goto done;
	}
	if (bm->dma_dir != DMA_NONE) {
		/*
		 * DMA buffer was allocated as a single block.
		 * Address is in page_list[0].
		 */
		buf = &bm->page_list[0];
		retval = dma_mmap_coherent(bm->dma_hw_dev, vma, buf->virt_addr,
					   buf->dma_addr, n_pages * PAGE_SIZE);
	} else {
		for (i = 0; i < n_pages; ++i) {
			unsigned long pfn;

			buf = &bm->page_list[i];
			pfn = page_to_pfn(virt_to_page(buf->virt_addr));
			retval = remap_pfn_range(vma, start, pfn, PAGE_SIZE,
						 PAGE_SHARED);
			if (retval)
				break;

			start += PAGE_SIZE;
		}
	}

	if (retval == 0) {
		vma->vm_ops = &comedi_vm_ops;
		vma->vm_private_data = bm;

		vma->vm_ops->open(vma);
	}

done:
	up_read(&dev->attach_lock);
	comedi_buf_map_put(bm);	/* put reference to buf map - okay if NULL */
	return retval;
}

static __poll_t comedi_poll(struct file *file, poll_table *wait)
{
	__poll_t mask = 0;
	struct comedi_file *cfp = file->private_data;
	struct comedi_device *dev = cfp->dev;
	struct comedi_subdevice *s, *s_read;

	down_read(&dev->attach_lock);

	if (!dev->attached) {
		dev_dbg(dev->class_dev, "no driver attached\n");
		goto done;
	}

	s = comedi_file_read_subdevice(file);
	s_read = s;
	if (s && s->async) {
		poll_wait(file, &s->async->wait_head, wait);
		if (s->busy != file || !comedi_is_subdevice_running(s) ||
		    (s->async->cmd.flags & CMDF_WRITE) ||
		    comedi_buf_read_n_available(s) > 0)
			mask |= EPOLLIN | EPOLLRDNORM;
	}

	s = comedi_file_write_subdevice(file);
	if (s && s->async) {
		unsigned int bps = comedi_bytes_per_sample(s);

		if (s != s_read)
			poll_wait(file, &s->async->wait_head, wait);
		if (s->busy != file || !comedi_is_subdevice_running(s) ||
		    !(s->async->cmd.flags & CMDF_WRITE) ||
		    comedi_buf_write_n_available(s) >= bps)
			mask |= EPOLLOUT | EPOLLWRNORM;
	}

done:
	up_read(&dev->attach_lock);
	return mask;
}

static ssize_t comedi_write(struct file *file, const char __user *buf,
			    size_t nbytes, loff_t *offset)
{
	struct comedi_subdevice *s;
	struct comedi_async *async;
	unsigned int n, m;
	ssize_t count = 0;
	int retval = 0;
	DECLARE_WAITQUEUE(wait, current);
	struct comedi_file *cfp = file->private_data;
	struct comedi_device *dev = cfp->dev;
	bool become_nonbusy = false;
	bool attach_locked;
	unsigned int old_detach_count;

	/* Protect against device detachment during operation. */
	down_read(&dev->attach_lock);
	attach_locked = true;
	old_detach_count = dev->detach_count;

	if (!dev->attached) {
		dev_dbg(dev->class_dev, "no driver attached\n");
		retval = -ENODEV;
		goto out;
	}

	s = comedi_file_write_subdevice(file);
	if (!s || !s->async) {
		retval = -EIO;
		goto out;
	}

	async = s->async;
	if (s->busy != file || !(async->cmd.flags & CMDF_WRITE)) {
		retval = -EINVAL;
		goto out;
	}

	add_wait_queue(&async->wait_head, &wait);
	while (count == 0 && !retval) {
		unsigned int runflags;
		unsigned int wp, n1, n2;

		set_current_state(TASK_INTERRUPTIBLE);

		runflags = comedi_get_subdevice_runflags(s);
		if (!comedi_is_runflags_running(runflags)) {
			if (comedi_is_runflags_in_error(runflags))
				retval = -EPIPE;
			if (retval || nbytes)
				become_nonbusy = true;
			break;
		}
		if (nbytes == 0)
			break;

		/* Allocate all free buffer space. */
		comedi_buf_write_alloc(s, async->prealloc_bufsz);
		m = comedi_buf_write_n_allocated(s);
		n = min_t(size_t, m, nbytes);

		if (n == 0) {
			if (file->f_flags & O_NONBLOCK) {
				retval = -EAGAIN;
				break;
			}
			schedule();
			if (signal_pending(current)) {
				retval = -ERESTARTSYS;
				break;
			}
			if (s->busy != file ||
			    !(async->cmd.flags & CMDF_WRITE)) {
				retval = -EINVAL;
				break;
			}
			continue;
		}

		set_current_state(TASK_RUNNING);
		wp = async->buf_write_ptr;
		n1 = min(n, async->prealloc_bufsz - wp);
		n2 = n - n1;
		m = copy_from_user(async->prealloc_buf + wp, buf, n1);
		if (m)
			m += n2;
		else if (n2)
			m = copy_from_user(async->prealloc_buf, buf + n1, n2);
		if (m) {
			n -= m;
			retval = -EFAULT;
		}
		comedi_buf_write_free(s, n);

		count += n;
		nbytes -= n;

		buf += n;
	}
	remove_wait_queue(&async->wait_head, &wait);
	set_current_state(TASK_RUNNING);
	if (become_nonbusy && count == 0) {
		struct comedi_subdevice *new_s;

		/*
		 * To avoid deadlock, cannot acquire dev->mutex
		 * while dev->attach_lock is held.
		 */
		up_read(&dev->attach_lock);
		attach_locked = false;
		mutex_lock(&dev->mutex);
		/*
		 * Check device hasn't become detached behind our back.
		 * Checking dev->detach_count is unchanged ought to be
		 * sufficient (unless there have been 2**32 detaches in the
		 * meantime!), but check the subdevice pointer as well just in
		 * case.
		 *
		 * Also check the subdevice is still in a suitable state to
		 * become non-busy in case it changed behind our back.
		 */
		new_s = comedi_file_write_subdevice(file);
		if (dev->attached && old_detach_count == dev->detach_count &&
		    s == new_s && new_s->async == async && s->busy == file &&
		    (async->cmd.flags & CMDF_WRITE) &&
		    !comedi_is_subdevice_running(s))
			do_become_nonbusy(dev, s);
		mutex_unlock(&dev->mutex);
	}
out:
	if (attach_locked)
		up_read(&dev->attach_lock);

	return count ? count : retval;
}

static ssize_t comedi_read(struct file *file, char __user *buf, size_t nbytes,
			   loff_t *offset)
{
	struct comedi_subdevice *s;
	struct comedi_async *async;
	unsigned int n, m;
	ssize_t count = 0;
	int retval = 0;
	DECLARE_WAITQUEUE(wait, current);
	struct comedi_file *cfp = file->private_data;
	struct comedi_device *dev = cfp->dev;
	unsigned int old_detach_count;
	bool become_nonbusy = false;
	bool attach_locked;

	/* Protect against device detachment during operation. */
	down_read(&dev->attach_lock);
	attach_locked = true;
	old_detach_count = dev->detach_count;

	if (!dev->attached) {
		dev_dbg(dev->class_dev, "no driver attached\n");
		retval = -ENODEV;
		goto out;
	}

	s = comedi_file_read_subdevice(file);
	if (!s || !s->async) {
		retval = -EIO;
		goto out;
	}

	async = s->async;
	if (s->busy != file || (async->cmd.flags & CMDF_WRITE)) {
		retval = -EINVAL;
		goto out;
	}

	add_wait_queue(&async->wait_head, &wait);
	while (count == 0 && !retval) {
		unsigned int rp, n1, n2;

		set_current_state(TASK_INTERRUPTIBLE);

		m = comedi_buf_read_n_available(s);
		n = min_t(size_t, m, nbytes);

		if (n == 0) {
			unsigned int runflags =
				     comedi_get_subdevice_runflags(s);

			if (!comedi_is_runflags_running(runflags)) {
				if (comedi_is_runflags_in_error(runflags))
					retval = -EPIPE;
				if (retval || nbytes)
					become_nonbusy = true;
				break;
			}
			if (nbytes == 0)
				break;
			if (file->f_flags & O_NONBLOCK) {
				retval = -EAGAIN;
				break;
			}
			schedule();
			if (signal_pending(current)) {
				retval = -ERESTARTSYS;
				break;
			}
			if (s->busy != file ||
			    (async->cmd.flags & CMDF_WRITE)) {
				retval = -EINVAL;
				break;
			}
			continue;
		}

		set_current_state(TASK_RUNNING);
		rp = async->buf_read_ptr;
		n1 = min(n, async->prealloc_bufsz - rp);
		n2 = n - n1;
		m = copy_to_user(buf, async->prealloc_buf + rp, n1);
		if (m)
			m += n2;
		else if (n2)
			m = copy_to_user(buf + n1, async->prealloc_buf, n2);
		if (m) {
			n -= m;
			retval = -EFAULT;
		}

		comedi_buf_read_alloc(s, n);
		comedi_buf_read_free(s, n);

		count += n;
		nbytes -= n;

		buf += n;
	}
	remove_wait_queue(&async->wait_head, &wait);
	set_current_state(TASK_RUNNING);
	if (become_nonbusy && count == 0) {
		struct comedi_subdevice *new_s;

		/*
		 * To avoid deadlock, cannot acquire dev->mutex
		 * while dev->attach_lock is held.
		 */
		up_read(&dev->attach_lock);
		attach_locked = false;
		mutex_lock(&dev->mutex);
		/*
		 * Check device hasn't become detached behind our back.
		 * Checking dev->detach_count is unchanged ought to be
		 * sufficient (unless there have been 2**32 detaches in the
		 * meantime!), but check the subdevice pointer as well just in
		 * case.
		 *
		 * Also check the subdevice is still in a suitable state to
		 * become non-busy in case it changed behind our back.
		 */
		new_s = comedi_file_read_subdevice(file);
		if (dev->attached && old_detach_count == dev->detach_count &&
		    s == new_s && new_s->async == async && s->busy == file &&
		    !(async->cmd.flags & CMDF_WRITE) &&
		    !comedi_is_subdevice_running(s) &&
		    comedi_buf_read_n_available(s) == 0)
			do_become_nonbusy(dev, s);
		mutex_unlock(&dev->mutex);
	}
out:
	if (attach_locked)
		up_read(&dev->attach_lock);

	return count ? count : retval;
}

static int comedi_open(struct inode *inode, struct file *file)
{
	const unsigned int minor = iminor(inode);
	struct comedi_file *cfp;
	struct comedi_device *dev = comedi_dev_get_from_minor(minor);
	int rc;

	if (!dev) {
		pr_debug("invalid minor number\n");
		return -ENODEV;
	}

	cfp = kzalloc(sizeof(*cfp), GFP_KERNEL);
	if (!cfp) {
		comedi_dev_put(dev);
		return -ENOMEM;
	}

	cfp->dev = dev;

	mutex_lock(&dev->mutex);
	if (!dev->attached && !capable(CAP_SYS_ADMIN)) {
		dev_dbg(dev->class_dev, "not attached and not CAP_SYS_ADMIN\n");
		rc = -ENODEV;
		goto out;
	}
	if (dev->attached && dev->use_count == 0) {
		if (!try_module_get(dev->driver->module)) {
			rc = -ENXIO;
			goto out;
		}
		if (dev->open) {
			rc = dev->open(dev);
			if (rc < 0) {
				module_put(dev->driver->module);
				goto out;
			}
		}
	}

	dev->use_count++;
	file->private_data = cfp;
	comedi_file_reset(file);
	rc = 0;

out:
	mutex_unlock(&dev->mutex);
	if (rc) {
		comedi_dev_put(dev);
		kfree(cfp);
	}
	return rc;
}

static int comedi_fasync(int fd, struct file *file, int on)
{
	struct comedi_file *cfp = file->private_data;
	struct comedi_device *dev = cfp->dev;

	return fasync_helper(fd, file, on, &dev->async_queue);
}

static int comedi_close(struct inode *inode, struct file *file)
{
	struct comedi_file *cfp = file->private_data;
	struct comedi_device *dev = cfp->dev;
	struct comedi_subdevice *s = NULL;
	int i;

	mutex_lock(&dev->mutex);

	if (dev->subdevices) {
		for (i = 0; i < dev->n_subdevices; i++) {
			s = &dev->subdevices[i];

			if (s->busy == file)
				do_cancel(dev, s);
			if (s->lock == file)
				s->lock = NULL;
		}
	}
	if (dev->attached && dev->use_count == 1) {
		if (dev->close)
			dev->close(dev);
		module_put(dev->driver->module);
	}

	dev->use_count--;

	mutex_unlock(&dev->mutex);
	comedi_dev_put(dev);
	kfree(cfp);

	return 0;
}

#ifdef CONFIG_COMPAT

#define COMEDI32_CHANINFO _IOR(CIO, 3, struct comedi32_chaninfo_struct)
#define COMEDI32_RANGEINFO _IOR(CIO, 8, struct comedi32_rangeinfo_struct)
/*
 * N.B. COMEDI32_CMD and COMEDI_CMD ought to use _IOWR, not _IOR.
 * It's too late to change it now, but it only affects the command number.
 */
#define COMEDI32_CMD _IOR(CIO, 9, struct comedi32_cmd_struct)
/*
 * N.B. COMEDI32_CMDTEST and COMEDI_CMDTEST ought to use _IOWR, not _IOR.
 * It's too late to change it now, but it only affects the command number.
 */
#define COMEDI32_CMDTEST _IOR(CIO, 10, struct comedi32_cmd_struct)
#define COMEDI32_INSNLIST _IOR(CIO, 11, struct comedi32_insnlist_struct)
#define COMEDI32_INSN _IOR(CIO, 12, struct comedi32_insn_struct)

struct comedi32_chaninfo_struct {
	unsigned int subdev;
	compat_uptr_t maxdata_list;	/* 32-bit 'unsigned int *' */
	compat_uptr_t flaglist;	/* 32-bit 'unsigned int *' */
	compat_uptr_t rangelist;	/* 32-bit 'unsigned int *' */
	unsigned int unused[4];
};

struct comedi32_rangeinfo_struct {
	unsigned int range_type;
	compat_uptr_t range_ptr;	/* 32-bit 'void *' */
};

struct comedi32_cmd_struct {
	unsigned int subdev;
	unsigned int flags;
	unsigned int start_src;
	unsigned int start_arg;
	unsigned int scan_begin_src;
	unsigned int scan_begin_arg;
	unsigned int convert_src;
	unsigned int convert_arg;
	unsigned int scan_end_src;
	unsigned int scan_end_arg;
	unsigned int stop_src;
	unsigned int stop_arg;
	compat_uptr_t chanlist;	/* 32-bit 'unsigned int *' */
	unsigned int chanlist_len;
	compat_uptr_t data;	/* 32-bit 'short *' */
	unsigned int data_len;
};

struct comedi32_insn_struct {
	unsigned int insn;
	unsigned int n;
	compat_uptr_t data;	/* 32-bit 'unsigned int *' */
	unsigned int subdev;
	unsigned int chanspec;
	unsigned int unused[3];
};

struct comedi32_insnlist_struct {
	unsigned int n_insns;
	compat_uptr_t insns;	/* 32-bit 'struct comedi_insn *' */
};

/* Handle 32-bit COMEDI_CHANINFO ioctl. */
static int compat_chaninfo(struct file *file, unsigned long arg)
{
	struct comedi_file *cfp = file->private_data;
	struct comedi_device *dev = cfp->dev;
	struct comedi32_chaninfo_struct chaninfo32;
	struct comedi_chaninfo chaninfo;
	int err;

	if (copy_from_user(&chaninfo32, compat_ptr(arg), sizeof(chaninfo32)))
		return -EFAULT;

	memset(&chaninfo, 0, sizeof(chaninfo));
	chaninfo.subdev = chaninfo32.subdev;
	chaninfo.maxdata_list = compat_ptr(chaninfo32.maxdata_list);
	chaninfo.flaglist = compat_ptr(chaninfo32.flaglist);
	chaninfo.rangelist = compat_ptr(chaninfo32.rangelist);

	mutex_lock(&dev->mutex);
	err = do_chaninfo_ioctl(dev, &chaninfo);
	mutex_unlock(&dev->mutex);
	return err;
}

/* Handle 32-bit COMEDI_RANGEINFO ioctl. */
static int compat_rangeinfo(struct file *file, unsigned long arg)
{
	struct comedi_file *cfp = file->private_data;
	struct comedi_device *dev = cfp->dev;
	struct comedi32_rangeinfo_struct rangeinfo32;
	struct comedi_rangeinfo rangeinfo;
	int err;

	if (copy_from_user(&rangeinfo32, compat_ptr(arg), sizeof(rangeinfo32)))
		return -EFAULT;
	memset(&rangeinfo, 0, sizeof(rangeinfo));
	rangeinfo.range_type = rangeinfo32.range_type;
	rangeinfo.range_ptr = compat_ptr(rangeinfo32.range_ptr);

	mutex_lock(&dev->mutex);
	err = do_rangeinfo_ioctl(dev, &rangeinfo);
	mutex_unlock(&dev->mutex);
	return err;
}

/* Copy 32-bit cmd structure to native cmd structure. */
static int get_compat_cmd(struct comedi_cmd *cmd,
			  struct comedi32_cmd_struct __user *cmd32)
{
	struct comedi32_cmd_struct v32;

	if (copy_from_user(&v32, cmd32, sizeof(v32)))
		return -EFAULT;

	cmd->subdev = v32.subdev;
	cmd->flags = v32.flags;
	cmd->start_src = v32.start_src;
	cmd->start_arg = v32.start_arg;
	cmd->scan_begin_src = v32.scan_begin_src;
	cmd->scan_begin_arg = v32.scan_begin_arg;
	cmd->convert_src = v32.convert_src;
	cmd->convert_arg = v32.convert_arg;
	cmd->scan_end_src = v32.scan_end_src;
	cmd->scan_end_arg = v32.scan_end_arg;
	cmd->stop_src = v32.stop_src;
	cmd->stop_arg = v32.stop_arg;
	cmd->chanlist = (unsigned int __force *)compat_ptr(v32.chanlist);
	cmd->chanlist_len = v32.chanlist_len;
	cmd->data = compat_ptr(v32.data);
	cmd->data_len = v32.data_len;
	return 0;
}

/* Copy native cmd structure to 32-bit cmd structure. */
static int put_compat_cmd(struct comedi32_cmd_struct __user *cmd32,
			  struct comedi_cmd *cmd)
{
	struct comedi32_cmd_struct v32;

	memset(&v32, 0, sizeof(v32));
	v32.subdev = cmd->subdev;
	v32.flags = cmd->flags;
	v32.start_src = cmd->start_src;
	v32.start_arg = cmd->start_arg;
	v32.scan_begin_src = cmd->scan_begin_src;
	v32.scan_begin_arg = cmd->scan_begin_arg;
	v32.convert_src = cmd->convert_src;
	v32.convert_arg = cmd->convert_arg;
	v32.scan_end_src = cmd->scan_end_src;
	v32.scan_end_arg = cmd->scan_end_arg;
	v32.stop_src = cmd->stop_src;
	v32.stop_arg = cmd->stop_arg;
	/* Assume chanlist pointer is unchanged. */
	v32.chanlist = ptr_to_compat((unsigned int __user *)cmd->chanlist);
	v32.chanlist_len = cmd->chanlist_len;
	v32.data = ptr_to_compat(cmd->data);
	v32.data_len = cmd->data_len;
	if (copy_to_user(cmd32, &v32, sizeof(v32)))
		return -EFAULT;
	return 0;
}

/* Handle 32-bit COMEDI_CMD ioctl. */
static int compat_cmd(struct file *file, unsigned long arg)
{
	struct comedi_file *cfp = file->private_data;
	struct comedi_device *dev = cfp->dev;
	struct comedi_cmd cmd;
	bool copy = false;
	int rc, err;

	rc = get_compat_cmd(&cmd, compat_ptr(arg));
	if (rc)
		return rc;

	mutex_lock(&dev->mutex);
	rc = do_cmd_ioctl(dev, &cmd, &copy, file);
	mutex_unlock(&dev->mutex);
	if (copy) {
		/* Special case: copy cmd back to user. */
		err = put_compat_cmd(compat_ptr(arg), &cmd);
		if (err)
			rc = err;
	}
	return rc;
}

/* Handle 32-bit COMEDI_CMDTEST ioctl. */
static int compat_cmdtest(struct file *file, unsigned long arg)
{
	struct comedi_file *cfp = file->private_data;
	struct comedi_device *dev = cfp->dev;
	struct comedi_cmd cmd;
	bool copy = false;
	int rc, err;

	rc = get_compat_cmd(&cmd, compat_ptr(arg));
	if (rc)
		return rc;

	mutex_lock(&dev->mutex);
	rc = do_cmdtest_ioctl(dev, &cmd, &copy, file);
	mutex_unlock(&dev->mutex);
	if (copy) {
		err = put_compat_cmd(compat_ptr(arg), &cmd);
		if (err)
			rc = err;
	}
	return rc;
}

/* Copy 32-bit insn structure to native insn structure. */
static int get_compat_insn(struct comedi_insn *insn,
			   struct comedi32_insn_struct __user *insn32)
{
	struct comedi32_insn_struct v32;

	/* Copy insn structure.  Ignore the unused members. */
	if (copy_from_user(&v32, insn32, sizeof(v32)))
		return -EFAULT;
	memset(insn, 0, sizeof(*insn));
	insn->insn = v32.insn;
	insn->n = v32.n;
	insn->data = compat_ptr(v32.data);
	insn->subdev = v32.subdev;
	insn->chanspec = v32.chanspec;
	return 0;
}

/* Handle 32-bit COMEDI_INSNLIST ioctl. */
static int compat_insnlist(struct file *file, unsigned long arg)
{
	struct comedi_file *cfp = file->private_data;
	struct comedi_device *dev = cfp->dev;
	struct comedi32_insnlist_struct insnlist32;
	struct comedi32_insn_struct __user *insn32;
	struct comedi_insn *insns;
	unsigned int n;
	int rc;

	if (copy_from_user(&insnlist32, compat_ptr(arg), sizeof(insnlist32)))
		return -EFAULT;

	insns = kcalloc(insnlist32.n_insns, sizeof(*insns), GFP_KERNEL);
	if (!insns)
		return -ENOMEM;

	/* Copy insn structures. */
	insn32 = compat_ptr(insnlist32.insns);
	for (n = 0; n < insnlist32.n_insns; n++) {
		rc = get_compat_insn(insns + n, insn32 + n);
		if (rc) {
			kfree(insns);
			return rc;
		}
	}

	mutex_lock(&dev->mutex);
	rc = do_insnlist_ioctl(dev, insns, insnlist32.n_insns, file);
	mutex_unlock(&dev->mutex);
	return rc;
}

/* Handle 32-bit COMEDI_INSN ioctl. */
static int compat_insn(struct file *file, unsigned long arg)
{
	struct comedi_file *cfp = file->private_data;
	struct comedi_device *dev = cfp->dev;
	struct comedi_insn insn;
	int rc;

	rc = get_compat_insn(&insn, (void __user *)arg);
	if (rc)
		return rc;

	mutex_lock(&dev->mutex);
	rc = do_insn_ioctl(dev, &insn, file);
	mutex_unlock(&dev->mutex);
	return rc;
}

/*
 * compat_ioctl file operation.
 *
 * Returns -ENOIOCTLCMD for unrecognised ioctl codes.
 */
static long comedi_compat_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int rc;

	switch (cmd) {
	case COMEDI_DEVCONFIG:
	case COMEDI_DEVINFO:
	case COMEDI_SUBDINFO:
	case COMEDI_BUFCONFIG:
	case COMEDI_BUFINFO:
		/* Just need to translate the pointer argument. */
		arg = (unsigned long)compat_ptr(arg);
		rc = comedi_unlocked_ioctl(file, cmd, arg);
		break;
	case COMEDI_LOCK:
	case COMEDI_UNLOCK:
	case COMEDI_CANCEL:
	case COMEDI_POLL:
	case COMEDI_SETRSUBD:
	case COMEDI_SETWSUBD:
		/* No translation needed. */
		rc = comedi_unlocked_ioctl(file, cmd, arg);
		break;
	case COMEDI32_CHANINFO:
		rc = compat_chaninfo(file, arg);
		break;
	case COMEDI32_RANGEINFO:
		rc = compat_rangeinfo(file, arg);
		break;
	case COMEDI32_CMD:
		rc = compat_cmd(file, arg);
		break;
	case COMEDI32_CMDTEST:
		rc = compat_cmdtest(file, arg);
		break;
	case COMEDI32_INSNLIST:
		rc = compat_insnlist(file, arg);
		break;
	case COMEDI32_INSN:
		rc = compat_insn(file, arg);
		break;
	default:
		rc = -ENOIOCTLCMD;
		break;
	}
	return rc;
}
#else
#define comedi_compat_ioctl NULL
#endif

static const struct file_operations comedi_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = comedi_unlocked_ioctl,
	.compat_ioctl = comedi_compat_ioctl,
	.open = comedi_open,
	.release = comedi_close,
	.read = comedi_read,
	.write = comedi_write,
	.mmap = comedi_mmap,
	.poll = comedi_poll,
	.fasync = comedi_fasync,
	.llseek = noop_llseek,
};

/**
 * comedi_event() - Handle events for asynchronous COMEDI command
 * @dev: COMEDI device.
 * @s: COMEDI subdevice.
 * Context: in_interrupt() (usually), @s->spin_lock spin-lock not held.
 *
 * If an asynchronous COMEDI command is active on the subdevice, process
 * any %COMEDI_CB_... event flags that have been set, usually by an
 * interrupt handler.  These may change the run state of the asynchronous
 * command, wake a task, and/or send a %SIGIO signal.
 */
void comedi_event(struct comedi_device *dev, struct comedi_subdevice *s)
{
	struct comedi_async *async = s->async;
	unsigned int events;
	int si_code = 0;
	unsigned long flags;

	spin_lock_irqsave(&s->spin_lock, flags);

	events = async->events;
	async->events = 0;
	if (!__comedi_is_subdevice_running(s)) {
		spin_unlock_irqrestore(&s->spin_lock, flags);
		return;
	}

	if (events & COMEDI_CB_CANCEL_MASK)
		__comedi_clear_subdevice_runflags(s, COMEDI_SRF_RUNNING);

	/*
	 * Remember if an error event has occurred, so an error can be
	 * returned the next time the user does a read() or write().
	 */
	if (events & COMEDI_CB_ERROR_MASK)
		__comedi_set_subdevice_runflags(s, COMEDI_SRF_ERROR);

	if (async->cb_mask & events) {
		wake_up_interruptible(&async->wait_head);
		si_code = async->cmd.flags & CMDF_WRITE ? POLL_OUT : POLL_IN;
	}

	spin_unlock_irqrestore(&s->spin_lock, flags);

	if (si_code)
		kill_fasync(&dev->async_queue, SIGIO, si_code);
}
EXPORT_SYMBOL_GPL(comedi_event);

/* Note: the ->mutex is pre-locked on successful return */
struct comedi_device *comedi_alloc_board_minor(struct device *hardware_device)
{
	struct comedi_device *dev;
	struct device *csdev;
	unsigned int i;

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return ERR_PTR(-ENOMEM);
	comedi_device_init(dev);
	comedi_set_hw_dev(dev, hardware_device);
	mutex_lock(&dev->mutex);
	mutex_lock(&comedi_board_minor_table_lock);
	for (i = hardware_device ? comedi_num_legacy_minors : 0;
	     i < COMEDI_NUM_BOARD_MINORS; ++i) {
		if (!comedi_board_minor_table[i]) {
			comedi_board_minor_table[i] = dev;
			break;
		}
	}
	mutex_unlock(&comedi_board_minor_table_lock);
	if (i == COMEDI_NUM_BOARD_MINORS) {
		mutex_unlock(&dev->mutex);
		comedi_device_cleanup(dev);
		comedi_dev_put(dev);
		dev_err(hardware_device,
			"ran out of minor numbers for board device files\n");
		return ERR_PTR(-EBUSY);
	}
	dev->minor = i;
	csdev = device_create(comedi_class, hardware_device,
			      MKDEV(COMEDI_MAJOR, i), NULL, "comedi%i", i);
	if (!IS_ERR(csdev))
		dev->class_dev = get_device(csdev);

	/* Note: dev->mutex needs to be unlocked by the caller. */
	return dev;
}

void comedi_release_hardware_device(struct device *hardware_device)
{
	int minor;
	struct comedi_device *dev;

	for (minor = comedi_num_legacy_minors; minor < COMEDI_NUM_BOARD_MINORS;
	     minor++) {
		mutex_lock(&comedi_board_minor_table_lock);
		dev = comedi_board_minor_table[minor];
		if (dev && dev->hw_dev == hardware_device) {
			comedi_board_minor_table[minor] = NULL;
			mutex_unlock(&comedi_board_minor_table_lock);
			comedi_free_board_dev(dev);
			break;
		}
		mutex_unlock(&comedi_board_minor_table_lock);
	}
}

int comedi_alloc_subdevice_minor(struct comedi_subdevice *s)
{
	struct comedi_device *dev = s->device;
	struct device *csdev;
	unsigned int i;

	mutex_lock(&comedi_subdevice_minor_table_lock);
	for (i = 0; i < COMEDI_NUM_SUBDEVICE_MINORS; ++i) {
		if (!comedi_subdevice_minor_table[i]) {
			comedi_subdevice_minor_table[i] = s;
			break;
		}
	}
	mutex_unlock(&comedi_subdevice_minor_table_lock);
	if (i == COMEDI_NUM_SUBDEVICE_MINORS) {
		dev_err(dev->class_dev,
			"ran out of minor numbers for subdevice files\n");
		return -EBUSY;
	}
	i += COMEDI_NUM_BOARD_MINORS;
	s->minor = i;
	csdev = device_create(comedi_class, dev->class_dev,
			      MKDEV(COMEDI_MAJOR, i), NULL, "comedi%i_subd%i",
			      dev->minor, s->index);
	if (!IS_ERR(csdev))
		s->class_dev = csdev;

	return 0;
}

void comedi_free_subdevice_minor(struct comedi_subdevice *s)
{
	unsigned int i;

	if (!s)
		return;
	if (s->minor < COMEDI_NUM_BOARD_MINORS ||
	    s->minor >= COMEDI_NUM_MINORS)
		return;

	i = s->minor - COMEDI_NUM_BOARD_MINORS;
	mutex_lock(&comedi_subdevice_minor_table_lock);
	if (s == comedi_subdevice_minor_table[i])
		comedi_subdevice_minor_table[i] = NULL;
	mutex_unlock(&comedi_subdevice_minor_table_lock);
	if (s->class_dev) {
		device_destroy(comedi_class, MKDEV(COMEDI_MAJOR, s->minor));
		s->class_dev = NULL;
	}
}

static void comedi_cleanup_board_minors(void)
{
	struct comedi_device *dev;
	unsigned int i;

	for (i = 0; i < COMEDI_NUM_BOARD_MINORS; i++) {
		dev = comedi_clear_board_minor(i);
		comedi_free_board_dev(dev);
	}
}

static int __init comedi_init(void)
{
	int i;
	int retval;

	pr_info("version " COMEDI_RELEASE " - http://www.comedi.org\n");

	if (comedi_num_legacy_minors > COMEDI_NUM_BOARD_MINORS) {
		pr_err("invalid value for module parameter \"comedi_num_legacy_minors\".  Valid values are 0 through %i.\n",
		       COMEDI_NUM_BOARD_MINORS);
		return -EINVAL;
	}

	retval = register_chrdev_region(MKDEV(COMEDI_MAJOR, 0),
					COMEDI_NUM_MINORS, "comedi");
	if (retval)
		return retval;

	cdev_init(&comedi_cdev, &comedi_fops);
	comedi_cdev.owner = THIS_MODULE;

	retval = kobject_set_name(&comedi_cdev.kobj, "comedi");
	if (retval)
		goto out_unregister_chrdev_region;

	retval = cdev_add(&comedi_cdev, MKDEV(COMEDI_MAJOR, 0),
			  COMEDI_NUM_MINORS);
	if (retval)
		goto out_unregister_chrdev_region;

	comedi_class = class_create(THIS_MODULE, "comedi");
	if (IS_ERR(comedi_class)) {
		retval = PTR_ERR(comedi_class);
		pr_err("failed to create class\n");
		goto out_cdev_del;
	}

	comedi_class->dev_groups = comedi_dev_groups;

	/* create devices files for legacy/manual use */
	for (i = 0; i < comedi_num_legacy_minors; i++) {
		struct comedi_device *dev;

		dev = comedi_alloc_board_minor(NULL);
		if (IS_ERR(dev)) {
			retval = PTR_ERR(dev);
			goto out_cleanup_board_minors;
		}
		/* comedi_alloc_board_minor() locked the mutex */
		lockdep_assert_held(&dev->mutex);
		mutex_unlock(&dev->mutex);
	}

	/* XXX requires /proc interface */
	comedi_proc_init();

	return 0;

out_cleanup_board_minors:
	comedi_cleanup_board_minors();
	class_destroy(comedi_class);
out_cdev_del:
	cdev_del(&comedi_cdev);
out_unregister_chrdev_region:
	unregister_chrdev_region(MKDEV(COMEDI_MAJOR, 0), COMEDI_NUM_MINORS);
	return retval;
}
module_init(comedi_init);

static void __exit comedi_cleanup(void)
{
	comedi_cleanup_board_minors();
	class_destroy(comedi_class);
	cdev_del(&comedi_cdev);
	unregister_chrdev_region(MKDEV(COMEDI_MAJOR, 0), COMEDI_NUM_MINORS);

	comedi_proc_cleanup();
}
module_exit(comedi_cleanup);

MODULE_AUTHOR("https://www.comedi.org");
MODULE_DESCRIPTION("Comedi core module");
MODULE_LICENSE("GPL");
