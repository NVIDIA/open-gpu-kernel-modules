// SPDX-License-Identifier: GPL-2.0+
/*
 * LEGO USB Tower driver
 *
 * Copyright (C) 2003 David Glance <davidgsf@sourceforge.net>
 *               2001-2004 Juergen Stuber <starblue@users.sourceforge.net>
 *
 * derived from USB Skeleton driver - 0.5
 * Copyright (C) 2001 Greg Kroah-Hartman (greg@kroah.com)
 *
 * History:
 *
 * 2001-10-13 - 0.1 js
 *   - first version
 * 2001-11-03 - 0.2 js
 *   - simplified buffering, one-shot URBs for writing
 * 2001-11-10 - 0.3 js
 *   - removed IOCTL (setting power/mode is more complicated, postponed)
 * 2001-11-28 - 0.4 js
 *   - added vendor commands for mode of operation and power level in open
 * 2001-12-04 - 0.5 js
 *   - set IR mode by default (by oversight 0.4 set VLL mode)
 * 2002-01-11 - 0.5? pcchan
 *   - make read buffer reusable and work around bytes_to_write issue between
 *     uhci and legusbtower
 * 2002-09-23 - 0.52 david (david@csse.uwa.edu.au)
 *   - imported into lejos project
 *   - changed wake_up to wake_up_interruptible
 *   - changed to use lego0 rather than tower0
 *   - changed dbg() to use __func__ rather than deprecated __func__
 * 2003-01-12 - 0.53 david (david@csse.uwa.edu.au)
 *   - changed read and write to write everything or
 *     timeout (from a patch by Chris Riesen and Brett Thaeler driver)
 *   - added ioctl functionality to set timeouts
 * 2003-07-18 - 0.54 davidgsf (david@csse.uwa.edu.au)
 *   - initial import into LegoUSB project
 *   - merge of existing LegoUSB.c driver
 * 2003-07-18 - 0.56 davidgsf (david@csse.uwa.edu.au)
 *   - port to 2.6 style driver
 * 2004-02-29 - 0.6 Juergen Stuber <starblue@users.sourceforge.net>
 *   - fix locking
 *   - unlink read URBs which are no longer needed
 *   - allow increased buffer size, eliminates need for timeout on write
 *   - have read URB running continuously
 *   - added poll
 *   - forbid seeking
 *   - added nonblocking I/O
 *   - changed back __func__ to __func__
 *   - read and log tower firmware version
 *   - reset tower on probe, avoids failure of first write
 * 2004-03-09 - 0.7 Juergen Stuber <starblue@users.sourceforge.net>
 *   - timeout read now only after inactivity, shorten default accordingly
 * 2004-03-11 - 0.8 Juergen Stuber <starblue@users.sourceforge.net>
 *   - log major, minor instead of possibly confusing device filename
 *   - whitespace cleanup
 * 2004-03-12 - 0.9 Juergen Stuber <starblue@users.sourceforge.net>
 *   - normalize whitespace in debug messages
 *   - take care about endianness in control message responses
 * 2004-03-13 - 0.91 Juergen Stuber <starblue@users.sourceforge.net>
 *   - make default intervals longer to accommodate current EHCI driver
 * 2004-03-19 - 0.92 Juergen Stuber <starblue@users.sourceforge.net>
 *   - replaced atomic_t by memory barriers
 * 2004-04-21 - 0.93 Juergen Stuber <starblue@users.sourceforge.net>
 *   - wait for completion of write urb in release (needed for remotecontrol)
 *   - corrected poll for write direction (missing negation)
 * 2004-04-22 - 0.94 Juergen Stuber <starblue@users.sourceforge.net>
 *   - make device locking interruptible
 * 2004-04-30 - 0.95 Juergen Stuber <starblue@users.sourceforge.net>
 *   - check for valid udev on resubmitting and unlinking urbs
 * 2004-08-03 - 0.96 Juergen Stuber <starblue@users.sourceforge.net>
 *   - move reset into open to clean out spurious data
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/completion.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/usb.h>
#include <linux/poll.h>


#define DRIVER_AUTHOR "Juergen Stuber <starblue@sourceforge.net>"
#define DRIVER_DESC "LEGO USB Tower Driver"


/* The defaults are chosen to work with the latest versions of leJOS and NQC.
 */

/* Some legacy software likes to receive packets in one piece.
 * In this case read_buffer_size should exceed the maximal packet length
 * (417 for datalog uploads), and packet_timeout should be set.
 */
static int read_buffer_size = 480;
module_param(read_buffer_size, int, 0);
MODULE_PARM_DESC(read_buffer_size, "Read buffer size");

/* Some legacy software likes to send packets in one piece.
 * In this case write_buffer_size should exceed the maximal packet length
 * (417 for firmware and program downloads).
 * A problem with long writes is that the following read may time out
 * if the software is not prepared to wait long enough.
 */
static int write_buffer_size = 480;
module_param(write_buffer_size, int, 0);
MODULE_PARM_DESC(write_buffer_size, "Write buffer size");

/* Some legacy software expects reads to contain whole LASM packets.
 * To achieve this, characters which arrive before a packet timeout
 * occurs will be returned in a single read operation.
 * A problem with long reads is that the software may time out
 * if it is not prepared to wait long enough.
 * The packet timeout should be greater than the time between the
 * reception of subsequent characters, which should arrive about
 * every 5ms for the standard 2400 baud.
 * Set it to 0 to disable.
 */
static int packet_timeout = 50;
module_param(packet_timeout, int, 0);
MODULE_PARM_DESC(packet_timeout, "Packet timeout in ms");

/* Some legacy software expects blocking reads to time out.
 * Timeout occurs after the specified time of read and write inactivity.
 * Set it to 0 to disable.
 */
static int read_timeout = 200;
module_param(read_timeout, int, 0);
MODULE_PARM_DESC(read_timeout, "Read timeout in ms");

/* As of kernel version 2.6.4 ehci-hcd uses an
 * "only one interrupt transfer per frame" shortcut
 * to simplify the scheduling of periodic transfers.
 * This conflicts with our standard 1ms intervals for in and out URBs.
 * We use default intervals of 2ms for in and 8ms for out transfers,
 * which is fast enough for 2400 baud and allows a small additional load.
 * Increase the interval to allow more devices that do interrupt transfers,
 * or set to 0 to use the standard interval from the endpoint descriptors.
 */
static int interrupt_in_interval = 2;
module_param(interrupt_in_interval, int, 0);
MODULE_PARM_DESC(interrupt_in_interval, "Interrupt in interval in ms");

static int interrupt_out_interval = 8;
module_param(interrupt_out_interval, int, 0);
MODULE_PARM_DESC(interrupt_out_interval, "Interrupt out interval in ms");

/* Define these values to match your device */
#define LEGO_USB_TOWER_VENDOR_ID	0x0694
#define LEGO_USB_TOWER_PRODUCT_ID	0x0001

/* Vendor requests */
#define LEGO_USB_TOWER_REQUEST_RESET		0x04
#define LEGO_USB_TOWER_REQUEST_GET_VERSION	0xFD

struct tower_reset_reply {
	__le16 size;
	__u8 err_code;
	__u8 spare;
};

struct tower_get_version_reply {
	__le16 size;
	__u8 err_code;
	__u8 spare;
	__u8 major;
	__u8 minor;
	__le16 build_no;
};


/* table of devices that work with this driver */
static const struct usb_device_id tower_table[] = {
	{ USB_DEVICE(LEGO_USB_TOWER_VENDOR_ID, LEGO_USB_TOWER_PRODUCT_ID) },
	{ }					/* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, tower_table);

#define LEGO_USB_TOWER_MINOR_BASE	160


/* Structure to hold all of our device specific stuff */
struct lego_usb_tower {
	struct mutex		lock;		/* locks this structure */
	struct usb_device	*udev;		/* save off the usb device pointer */
	unsigned char		minor;		/* the starting minor number for this device */

	int			open_count;	/* number of times this port has been opened */
	unsigned long		disconnected:1;

	char			*read_buffer;
	size_t			read_buffer_length; /* this much came in */
	size_t			read_packet_length; /* this much will be returned on read */
	spinlock_t		read_buffer_lock;
	int			packet_timeout_jiffies;
	unsigned long		read_last_arrival;

	wait_queue_head_t	read_wait;
	wait_queue_head_t	write_wait;

	char			*interrupt_in_buffer;
	struct usb_endpoint_descriptor *interrupt_in_endpoint;
	struct urb		*interrupt_in_urb;
	int			interrupt_in_interval;
	int			interrupt_in_done;

	char			*interrupt_out_buffer;
	struct usb_endpoint_descriptor *interrupt_out_endpoint;
	struct urb		*interrupt_out_urb;
	int			interrupt_out_interval;
	int			interrupt_out_busy;

};


/* local function prototypes */
static ssize_t tower_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos);
static ssize_t tower_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos);
static inline void tower_delete(struct lego_usb_tower *dev);
static int tower_open(struct inode *inode, struct file *file);
static int tower_release(struct inode *inode, struct file *file);
static __poll_t tower_poll(struct file *file, poll_table *wait);
static loff_t tower_llseek(struct file *file, loff_t off, int whence);

static void tower_check_for_read_packet(struct lego_usb_tower *dev);
static void tower_interrupt_in_callback(struct urb *urb);
static void tower_interrupt_out_callback(struct urb *urb);

static int  tower_probe(struct usb_interface *interface, const struct usb_device_id *id);
static void tower_disconnect(struct usb_interface *interface);


/* file operations needed when we register this driver */
static const struct file_operations tower_fops = {
	.owner =	THIS_MODULE,
	.read  =	tower_read,
	.write =	tower_write,
	.open =		tower_open,
	.release =	tower_release,
	.poll =		tower_poll,
	.llseek =	tower_llseek,
};

static char *legousbtower_devnode(struct device *dev, umode_t *mode)
{
	return kasprintf(GFP_KERNEL, "usb/%s", dev_name(dev));
}

/*
 * usb class driver info in order to get a minor number from the usb core,
 * and to have the device registered with the driver core
 */
static struct usb_class_driver tower_class = {
	.name =		"legousbtower%d",
	.devnode = 	legousbtower_devnode,
	.fops =		&tower_fops,
	.minor_base =	LEGO_USB_TOWER_MINOR_BASE,
};


/* usb specific object needed to register this driver with the usb subsystem */
static struct usb_driver tower_driver = {
	.name =		"legousbtower",
	.probe =	tower_probe,
	.disconnect =	tower_disconnect,
	.id_table =	tower_table,
};


/*
 *	lego_usb_tower_debug_data
 */
static inline void lego_usb_tower_debug_data(struct device *dev,
					     const char *function, int size,
					     const unsigned char *data)
{
	dev_dbg(dev, "%s - length = %d, data = %*ph\n",
		function, size, size, data);
}


/*
 *	tower_delete
 */
static inline void tower_delete(struct lego_usb_tower *dev)
{
	/* free data structures */
	usb_free_urb(dev->interrupt_in_urb);
	usb_free_urb(dev->interrupt_out_urb);
	kfree(dev->read_buffer);
	kfree(dev->interrupt_in_buffer);
	kfree(dev->interrupt_out_buffer);
	usb_put_dev(dev->udev);
	kfree(dev);
}


/*
 *	tower_open
 */
static int tower_open(struct inode *inode, struct file *file)
{
	struct lego_usb_tower *dev = NULL;
	int subminor;
	int retval = 0;
	struct usb_interface *interface;
	struct tower_reset_reply reset_reply;
	int result;

	nonseekable_open(inode, file);
	subminor = iminor(inode);

	interface = usb_find_interface(&tower_driver, subminor);
	if (!interface) {
		pr_err("error, can't find device for minor %d\n", subminor);
		retval = -ENODEV;
		goto exit;
	}

	dev = usb_get_intfdata(interface);
	if (!dev) {
		retval = -ENODEV;
		goto exit;
	}

	/* lock this device */
	if (mutex_lock_interruptible(&dev->lock)) {
	        retval = -ERESTARTSYS;
		goto exit;
	}


	/* allow opening only once */
	if (dev->open_count) {
		retval = -EBUSY;
		goto unlock_exit;
	}

	/* reset the tower */
	result = usb_control_msg_recv(dev->udev, 0,
				      LEGO_USB_TOWER_REQUEST_RESET,
				      USB_TYPE_VENDOR | USB_DIR_IN | USB_RECIP_DEVICE,
				      0, 0,
				      &reset_reply, sizeof(reset_reply), 1000,
				      GFP_KERNEL);
	if (result < 0) {
		dev_err(&dev->udev->dev,
			"LEGO USB Tower reset control request failed\n");
		retval = result;
		goto unlock_exit;
	}

	/* initialize in direction */
	dev->read_buffer_length = 0;
	dev->read_packet_length = 0;
	usb_fill_int_urb(dev->interrupt_in_urb,
			 dev->udev,
			 usb_rcvintpipe(dev->udev, dev->interrupt_in_endpoint->bEndpointAddress),
			 dev->interrupt_in_buffer,
			 usb_endpoint_maxp(dev->interrupt_in_endpoint),
			 tower_interrupt_in_callback,
			 dev,
			 dev->interrupt_in_interval);

	dev->interrupt_in_done = 0;
	mb();

	retval = usb_submit_urb(dev->interrupt_in_urb, GFP_KERNEL);
	if (retval) {
		dev_err(&dev->udev->dev,
			"Couldn't submit interrupt_in_urb %d\n", retval);
		goto unlock_exit;
	}

	/* save device in the file's private structure */
	file->private_data = dev;

	dev->open_count = 1;

unlock_exit:
	mutex_unlock(&dev->lock);

exit:
	return retval;
}

/*
 *	tower_release
 */
static int tower_release(struct inode *inode, struct file *file)
{
	struct lego_usb_tower *dev;
	int retval = 0;

	dev = file->private_data;
	if (dev == NULL) {
		retval = -ENODEV;
		goto exit;
	}

	mutex_lock(&dev->lock);

	if (dev->disconnected) {
		/* the device was unplugged before the file was released */

		/* unlock here as tower_delete frees dev */
		mutex_unlock(&dev->lock);
		tower_delete(dev);
		goto exit;
	}

	/* wait until write transfer is finished */
	if (dev->interrupt_out_busy) {
		wait_event_interruptible_timeout(dev->write_wait, !dev->interrupt_out_busy,
						 2 * HZ);
	}

	/* shutdown transfers */
	usb_kill_urb(dev->interrupt_in_urb);
	usb_kill_urb(dev->interrupt_out_urb);

	dev->open_count = 0;

	mutex_unlock(&dev->lock);
exit:
	return retval;
}

/*
 *	tower_check_for_read_packet
 *
 *      To get correct semantics for signals and non-blocking I/O
 *      with packetizing we pretend not to see any data in the read buffer
 *      until it has been there unchanged for at least
 *      dev->packet_timeout_jiffies, or until the buffer is full.
 */
static void tower_check_for_read_packet(struct lego_usb_tower *dev)
{
	spin_lock_irq(&dev->read_buffer_lock);
	if (!packet_timeout
	    || time_after(jiffies, dev->read_last_arrival + dev->packet_timeout_jiffies)
	    || dev->read_buffer_length == read_buffer_size) {
		dev->read_packet_length = dev->read_buffer_length;
	}
	dev->interrupt_in_done = 0;
	spin_unlock_irq(&dev->read_buffer_lock);
}


/*
 *	tower_poll
 */
static __poll_t tower_poll(struct file *file, poll_table *wait)
{
	struct lego_usb_tower *dev;
	__poll_t mask = 0;

	dev = file->private_data;

	if (dev->disconnected)
		return EPOLLERR | EPOLLHUP;

	poll_wait(file, &dev->read_wait, wait);
	poll_wait(file, &dev->write_wait, wait);

	tower_check_for_read_packet(dev);
	if (dev->read_packet_length > 0)
		mask |= EPOLLIN | EPOLLRDNORM;
	if (!dev->interrupt_out_busy)
		mask |= EPOLLOUT | EPOLLWRNORM;

	return mask;
}


/*
 *	tower_llseek
 */
static loff_t tower_llseek(struct file *file, loff_t off, int whence)
{
	return -ESPIPE;		/* unseekable */
}


/*
 *	tower_read
 */
static ssize_t tower_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
	struct lego_usb_tower *dev;
	size_t bytes_to_read;
	int i;
	int retval = 0;
	unsigned long timeout = 0;

	dev = file->private_data;

	/* lock this object */
	if (mutex_lock_interruptible(&dev->lock)) {
		retval = -ERESTARTSYS;
		goto exit;
	}

	/* verify that the device wasn't unplugged */
	if (dev->disconnected) {
		retval = -ENODEV;
		goto unlock_exit;
	}

	/* verify that we actually have some data to read */
	if (count == 0) {
		dev_dbg(&dev->udev->dev, "read request of 0 bytes\n");
		goto unlock_exit;
	}

	if (read_timeout)
		timeout = jiffies + msecs_to_jiffies(read_timeout);

	/* wait for data */
	tower_check_for_read_packet(dev);
	while (dev->read_packet_length == 0) {
		if (file->f_flags & O_NONBLOCK) {
			retval = -EAGAIN;
			goto unlock_exit;
		}
		retval = wait_event_interruptible_timeout(dev->read_wait, dev->interrupt_in_done, dev->packet_timeout_jiffies);
		if (retval < 0)
			goto unlock_exit;

		/* reset read timeout during read or write activity */
		if (read_timeout
		    && (dev->read_buffer_length || dev->interrupt_out_busy)) {
			timeout = jiffies + msecs_to_jiffies(read_timeout);
		}
		/* check for read timeout */
		if (read_timeout && time_after(jiffies, timeout)) {
			retval = -ETIMEDOUT;
			goto unlock_exit;
		}
		tower_check_for_read_packet(dev);
	}

	/* copy the data from read_buffer into userspace */
	bytes_to_read = min(count, dev->read_packet_length);

	if (copy_to_user(buffer, dev->read_buffer, bytes_to_read)) {
		retval = -EFAULT;
		goto unlock_exit;
	}

	spin_lock_irq(&dev->read_buffer_lock);
	dev->read_buffer_length -= bytes_to_read;
	dev->read_packet_length -= bytes_to_read;
	for (i = 0; i < dev->read_buffer_length; i++)
		dev->read_buffer[i] = dev->read_buffer[i+bytes_to_read];
	spin_unlock_irq(&dev->read_buffer_lock);

	retval = bytes_to_read;

unlock_exit:
	/* unlock the device */
	mutex_unlock(&dev->lock);

exit:
	return retval;
}


/*
 *	tower_write
 */
static ssize_t tower_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	struct lego_usb_tower *dev;
	size_t bytes_to_write;
	int retval = 0;

	dev = file->private_data;

	/* lock this object */
	if (mutex_lock_interruptible(&dev->lock)) {
		retval = -ERESTARTSYS;
		goto exit;
	}

	/* verify that the device wasn't unplugged */
	if (dev->disconnected) {
		retval = -ENODEV;
		goto unlock_exit;
	}

	/* verify that we actually have some data to write */
	if (count == 0) {
		dev_dbg(&dev->udev->dev, "write request of 0 bytes\n");
		goto unlock_exit;
	}

	/* wait until previous transfer is finished */
	while (dev->interrupt_out_busy) {
		if (file->f_flags & O_NONBLOCK) {
			retval = -EAGAIN;
			goto unlock_exit;
		}
		retval = wait_event_interruptible(dev->write_wait,
						  !dev->interrupt_out_busy);
		if (retval)
			goto unlock_exit;
	}

	/* write the data into interrupt_out_buffer from userspace */
	bytes_to_write = min_t(int, count, write_buffer_size);
	dev_dbg(&dev->udev->dev, "%s: count = %zd, bytes_to_write = %zd\n",
		__func__, count, bytes_to_write);

	if (copy_from_user(dev->interrupt_out_buffer, buffer, bytes_to_write)) {
		retval = -EFAULT;
		goto unlock_exit;
	}

	/* send off the urb */
	usb_fill_int_urb(dev->interrupt_out_urb,
			 dev->udev,
			 usb_sndintpipe(dev->udev, dev->interrupt_out_endpoint->bEndpointAddress),
			 dev->interrupt_out_buffer,
			 bytes_to_write,
			 tower_interrupt_out_callback,
			 dev,
			 dev->interrupt_out_interval);

	dev->interrupt_out_busy = 1;
	wmb();

	retval = usb_submit_urb(dev->interrupt_out_urb, GFP_KERNEL);
	if (retval) {
		dev->interrupt_out_busy = 0;
		dev_err(&dev->udev->dev,
			"Couldn't submit interrupt_out_urb %d\n", retval);
		goto unlock_exit;
	}
	retval = bytes_to_write;

unlock_exit:
	/* unlock the device */
	mutex_unlock(&dev->lock);

exit:
	return retval;
}


/*
 *	tower_interrupt_in_callback
 */
static void tower_interrupt_in_callback(struct urb *urb)
{
	struct lego_usb_tower *dev = urb->context;
	int status = urb->status;
	int retval;
	unsigned long flags;

	lego_usb_tower_debug_data(&dev->udev->dev, __func__,
				  urb->actual_length, urb->transfer_buffer);

	if (status) {
		if (status == -ENOENT ||
		    status == -ECONNRESET ||
		    status == -ESHUTDOWN) {
			goto exit;
		} else {
			dev_dbg(&dev->udev->dev,
				"%s: nonzero status received: %d\n", __func__,
				status);
			goto resubmit; /* maybe we can recover */
		}
	}

	if (urb->actual_length > 0) {
		spin_lock_irqsave(&dev->read_buffer_lock, flags);
		if (dev->read_buffer_length + urb->actual_length < read_buffer_size) {
			memcpy(dev->read_buffer + dev->read_buffer_length,
			       dev->interrupt_in_buffer,
			       urb->actual_length);
			dev->read_buffer_length += urb->actual_length;
			dev->read_last_arrival = jiffies;
			dev_dbg(&dev->udev->dev, "%s: received %d bytes\n",
				__func__, urb->actual_length);
		} else {
			pr_warn("read_buffer overflow, %d bytes dropped\n",
				urb->actual_length);
		}
		spin_unlock_irqrestore(&dev->read_buffer_lock, flags);
	}

resubmit:
	retval = usb_submit_urb(dev->interrupt_in_urb, GFP_ATOMIC);
	if (retval) {
		dev_err(&dev->udev->dev, "%s: usb_submit_urb failed (%d)\n",
			__func__, retval);
	}
exit:
	dev->interrupt_in_done = 1;
	wake_up_interruptible(&dev->read_wait);
}


/*
 *	tower_interrupt_out_callback
 */
static void tower_interrupt_out_callback(struct urb *urb)
{
	struct lego_usb_tower *dev = urb->context;
	int status = urb->status;

	lego_usb_tower_debug_data(&dev->udev->dev, __func__,
				  urb->actual_length, urb->transfer_buffer);

	/* sync/async unlink faults aren't errors */
	if (status && !(status == -ENOENT ||
			status == -ECONNRESET ||
			status == -ESHUTDOWN)) {
		dev_dbg(&dev->udev->dev,
			"%s: nonzero write bulk status received: %d\n", __func__,
			status);
	}

	dev->interrupt_out_busy = 0;
	wake_up_interruptible(&dev->write_wait);
}


/*
 *	tower_probe
 *
 *	Called by the usb core when a new device is connected that it thinks
 *	this driver might be interested in.
 */
static int tower_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct device *idev = &interface->dev;
	struct usb_device *udev = interface_to_usbdev(interface);
	struct lego_usb_tower *dev;
	struct tower_get_version_reply get_version_reply;
	int retval = -ENOMEM;
	int result;

	/* allocate memory for our device state and initialize it */
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		goto exit;

	mutex_init(&dev->lock);
	dev->udev = usb_get_dev(udev);
	spin_lock_init(&dev->read_buffer_lock);
	dev->packet_timeout_jiffies = msecs_to_jiffies(packet_timeout);
	dev->read_last_arrival = jiffies;
	init_waitqueue_head(&dev->read_wait);
	init_waitqueue_head(&dev->write_wait);

	result = usb_find_common_endpoints_reverse(interface->cur_altsetting,
			NULL, NULL,
			&dev->interrupt_in_endpoint,
			&dev->interrupt_out_endpoint);
	if (result) {
		dev_err(idev, "interrupt endpoints not found\n");
		retval = result;
		goto error;
	}

	dev->read_buffer = kmalloc(read_buffer_size, GFP_KERNEL);
	if (!dev->read_buffer)
		goto error;
	dev->interrupt_in_buffer = kmalloc(usb_endpoint_maxp(dev->interrupt_in_endpoint), GFP_KERNEL);
	if (!dev->interrupt_in_buffer)
		goto error;
	dev->interrupt_in_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!dev->interrupt_in_urb)
		goto error;
	dev->interrupt_out_buffer = kmalloc(write_buffer_size, GFP_KERNEL);
	if (!dev->interrupt_out_buffer)
		goto error;
	dev->interrupt_out_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!dev->interrupt_out_urb)
		goto error;
	dev->interrupt_in_interval = interrupt_in_interval ? interrupt_in_interval : dev->interrupt_in_endpoint->bInterval;
	dev->interrupt_out_interval = interrupt_out_interval ? interrupt_out_interval : dev->interrupt_out_endpoint->bInterval;

	/* get the firmware version and log it */
	result = usb_control_msg_recv(udev, 0,
				      LEGO_USB_TOWER_REQUEST_GET_VERSION,
				      USB_TYPE_VENDOR | USB_DIR_IN | USB_RECIP_DEVICE,
				      0,
				      0,
				      &get_version_reply,
				      sizeof(get_version_reply),
				      1000, GFP_KERNEL);
	if (result) {
		dev_err(idev, "get version request failed: %d\n", result);
		retval = result;
		goto error;
	}
	dev_info(&interface->dev,
		 "LEGO USB Tower firmware version is %d.%d build %d\n",
		 get_version_reply.major,
		 get_version_reply.minor,
		 le16_to_cpu(get_version_reply.build_no));

	/* we can register the device now, as it is ready */
	usb_set_intfdata(interface, dev);

	retval = usb_register_dev(interface, &tower_class);
	if (retval) {
		/* something prevented us from registering this driver */
		dev_err(idev, "Not able to get a minor for this device.\n");
		goto error;
	}
	dev->minor = interface->minor;

	/* let the user know what node this device is now attached to */
	dev_info(&interface->dev, "LEGO USB Tower #%d now attached to major "
		 "%d minor %d\n", (dev->minor - LEGO_USB_TOWER_MINOR_BASE),
		 USB_MAJOR, dev->minor);

exit:
	return retval;

error:
	tower_delete(dev);
	return retval;
}


/*
 *	tower_disconnect
 *
 *	Called by the usb core when the device is removed from the system.
 */
static void tower_disconnect(struct usb_interface *interface)
{
	struct lego_usb_tower *dev;
	int minor;

	dev = usb_get_intfdata(interface);

	minor = dev->minor;

	/* give back our minor and prevent further open() */
	usb_deregister_dev(interface, &tower_class);

	/* stop I/O */
	usb_poison_urb(dev->interrupt_in_urb);
	usb_poison_urb(dev->interrupt_out_urb);

	mutex_lock(&dev->lock);

	/* if the device is not opened, then we clean up right now */
	if (!dev->open_count) {
		mutex_unlock(&dev->lock);
		tower_delete(dev);
	} else {
		dev->disconnected = 1;
		/* wake up pollers */
		wake_up_interruptible_all(&dev->read_wait);
		wake_up_interruptible_all(&dev->write_wait);
		mutex_unlock(&dev->lock);
	}

	dev_info(&interface->dev, "LEGO USB Tower #%d now disconnected\n",
		 (minor - LEGO_USB_TOWER_MINOR_BASE));
}

module_usb_driver(tower_driver);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
