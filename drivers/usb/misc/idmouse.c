// SPDX-License-Identifier: GPL-2.0+
/* Siemens ID Mouse driver v0.6

  Copyright (C) 2004-5 by Florian 'Floe' Echtler  <echtler@fs.tum.de>
                      and Andreas  'ad'  Deresch <aderesch@fs.tum.de>

  Derived from the USB Skeleton driver 1.1,
  Copyright (C) 2003 Greg Kroah-Hartman (greg@kroah.com)

  Additional information provided by Martin Reising
  <Martin.Reising@natural-computing.de>

*/

#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/completion.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/usb.h>

/* image constants */
#define WIDTH 225
#define HEIGHT 289
#define HEADER "P5 225 289 255 "
#define IMGSIZE ((WIDTH * HEIGHT) + sizeof(HEADER)-1)

#define DRIVER_SHORT   "idmouse"
#define DRIVER_AUTHOR  "Florian 'Floe' Echtler <echtler@fs.tum.de>"
#define DRIVER_DESC    "Siemens ID Mouse FingerTIP Sensor Driver"

/* minor number for misc USB devices */
#define USB_IDMOUSE_MINOR_BASE 132

/* vendor and device IDs */
#define ID_SIEMENS 0x0681
#define ID_IDMOUSE 0x0005
#define ID_CHERRY  0x0010

/* device ID table */
static const struct usb_device_id idmouse_table[] = {
	{USB_DEVICE(ID_SIEMENS, ID_IDMOUSE)}, /* Siemens ID Mouse (Professional) */
	{USB_DEVICE(ID_SIEMENS, ID_CHERRY )}, /* Cherry FingerTIP ID Board       */
	{}                                    /* terminating null entry          */
};

/* sensor commands */
#define FTIP_RESET   0x20
#define FTIP_ACQUIRE 0x21
#define FTIP_RELEASE 0x22
#define FTIP_BLINK   0x23  /* LSB of value = blink pulse width */
#define FTIP_SCROLL  0x24

#define ftip_command(dev, command, value, index) \
	usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0), command, \
	USB_TYPE_VENDOR | USB_RECIP_ENDPOINT | USB_DIR_OUT, value, index, NULL, 0, 1000)

MODULE_DEVICE_TABLE(usb, idmouse_table);

/* structure to hold all of our device specific stuff */
struct usb_idmouse {

	struct usb_device *udev; /* save off the usb device pointer */
	struct usb_interface *interface; /* the interface for this device */

	unsigned char *bulk_in_buffer; /* the buffer to receive data */
	size_t bulk_in_size; /* the maximum bulk packet size */
	size_t orig_bi_size; /* same as above, but reported by the device */
	__u8 bulk_in_endpointAddr; /* the address of the bulk in endpoint */

	int open; /* if the port is open or not */
	int present; /* if the device is not disconnected */
	struct mutex lock; /* locks this structure */

};

/* local function prototypes */
static ssize_t idmouse_read(struct file *file, char __user *buffer,
				size_t count, loff_t * ppos);

static int idmouse_open(struct inode *inode, struct file *file);
static int idmouse_release(struct inode *inode, struct file *file);

static int idmouse_probe(struct usb_interface *interface,
				const struct usb_device_id *id);

static void idmouse_disconnect(struct usb_interface *interface);
static int idmouse_suspend(struct usb_interface *intf, pm_message_t message);
static int idmouse_resume(struct usb_interface *intf);

/* file operation pointers */
static const struct file_operations idmouse_fops = {
	.owner = THIS_MODULE,
	.read = idmouse_read,
	.open = idmouse_open,
	.release = idmouse_release,
	.llseek = default_llseek,
};

/* class driver information */
static struct usb_class_driver idmouse_class = {
	.name = "idmouse%d",
	.fops = &idmouse_fops,
	.minor_base = USB_IDMOUSE_MINOR_BASE,
};

/* usb specific object needed to register this driver with the usb subsystem */
static struct usb_driver idmouse_driver = {
	.name = DRIVER_SHORT,
	.probe = idmouse_probe,
	.disconnect = idmouse_disconnect,
	.suspend = idmouse_suspend,
	.resume = idmouse_resume,
	.reset_resume = idmouse_resume,
	.id_table = idmouse_table,
	.supports_autosuspend = 1,
};

static int idmouse_create_image(struct usb_idmouse *dev)
{
	int bytes_read;
	int bulk_read;
	int result;

	memcpy(dev->bulk_in_buffer, HEADER, sizeof(HEADER)-1);
	bytes_read = sizeof(HEADER)-1;

	/* reset the device and set a fast blink rate */
	result = ftip_command(dev, FTIP_RELEASE, 0, 0);
	if (result < 0)
		goto reset;
	result = ftip_command(dev, FTIP_BLINK,   1, 0);
	if (result < 0)
		goto reset;

	/* initialize the sensor - sending this command twice */
	/* significantly reduces the rate of failed reads     */
	result = ftip_command(dev, FTIP_ACQUIRE, 0, 0);
	if (result < 0)
		goto reset;
	result = ftip_command(dev, FTIP_ACQUIRE, 0, 0);
	if (result < 0)
		goto reset;

	/* start the readout - sending this command twice */
	/* presumably enables the high dynamic range mode */
	result = ftip_command(dev, FTIP_RESET,   0, 0);
	if (result < 0)
		goto reset;
	result = ftip_command(dev, FTIP_RESET,   0, 0);
	if (result < 0)
		goto reset;

	/* loop over a blocking bulk read to get data from the device */
	while (bytes_read < IMGSIZE) {
		result = usb_bulk_msg(dev->udev,
				usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
				dev->bulk_in_buffer + bytes_read,
				dev->bulk_in_size, &bulk_read, 5000);
		if (result < 0) {
			/* Maybe this error was caused by the increased packet size? */
			/* Reset to the original value and tell userspace to retry.  */
			if (dev->bulk_in_size != dev->orig_bi_size) {
				dev->bulk_in_size = dev->orig_bi_size;
				result = -EAGAIN;
			}
			break;
		}
		if (signal_pending(current)) {
			result = -EINTR;
			break;
		}
		bytes_read += bulk_read;
	}

	/* reset the device */
reset:
	ftip_command(dev, FTIP_RELEASE, 0, 0);

	/* check for valid image */
	/* right border should be black (0x00) */
	for (bytes_read = sizeof(HEADER)-1 + WIDTH-1; bytes_read < IMGSIZE; bytes_read += WIDTH)
		if (dev->bulk_in_buffer[bytes_read] != 0x00)
			return -EAGAIN;

	/* lower border should be white (0xFF) */
	for (bytes_read = IMGSIZE-WIDTH; bytes_read < IMGSIZE-1; bytes_read++)
		if (dev->bulk_in_buffer[bytes_read] != 0xFF)
			return -EAGAIN;

	/* should be IMGSIZE == 65040 */
	dev_dbg(&dev->interface->dev, "read %d bytes fingerprint data\n",
		bytes_read);
	return result;
}

/* PM operations are nops as this driver does IO only during open() */
static int idmouse_suspend(struct usb_interface *intf, pm_message_t message)
{
	return 0;
}

static int idmouse_resume(struct usb_interface *intf)
{
	return 0;
}

static inline void idmouse_delete(struct usb_idmouse *dev)
{
	kfree(dev->bulk_in_buffer);
	kfree(dev);
}

static int idmouse_open(struct inode *inode, struct file *file)
{
	struct usb_idmouse *dev;
	struct usb_interface *interface;
	int result;

	/* get the interface from minor number and driver information */
	interface = usb_find_interface(&idmouse_driver, iminor(inode));
	if (!interface)
		return -ENODEV;

	/* get the device information block from the interface */
	dev = usb_get_intfdata(interface);
	if (!dev)
		return -ENODEV;

	/* lock this device */
	mutex_lock(&dev->lock);

	/* check if already open */
	if (dev->open) {

		/* already open, so fail */
		result = -EBUSY;

	} else {

		/* create a new image and check for success */
		result = usb_autopm_get_interface(interface);
		if (result)
			goto error;
		result = idmouse_create_image(dev);
		usb_autopm_put_interface(interface);
		if (result)
			goto error;

		/* increment our usage count for the driver */
		++dev->open;

		/* save our object in the file's private structure */
		file->private_data = dev;

	} 

error:

	/* unlock this device */
	mutex_unlock(&dev->lock);
	return result;
}

static int idmouse_release(struct inode *inode, struct file *file)
{
	struct usb_idmouse *dev;

	dev = file->private_data;

	if (dev == NULL)
		return -ENODEV;

	/* lock our device */
	mutex_lock(&dev->lock);

	--dev->open;

	if (!dev->present) {
		/* the device was unplugged before the file was released */
		mutex_unlock(&dev->lock);
		idmouse_delete(dev);
	} else {
		mutex_unlock(&dev->lock);
	}
	return 0;
}

static ssize_t idmouse_read(struct file *file, char __user *buffer, size_t count,
				loff_t * ppos)
{
	struct usb_idmouse *dev = file->private_data;
	int result;

	/* lock this object */
	mutex_lock(&dev->lock);

	/* verify that the device wasn't unplugged */
	if (!dev->present) {
		mutex_unlock(&dev->lock);
		return -ENODEV;
	}

	result = simple_read_from_buffer(buffer, count, ppos,
					dev->bulk_in_buffer, IMGSIZE);
	/* unlock the device */
	mutex_unlock(&dev->lock);
	return result;
}

static int idmouse_probe(struct usb_interface *interface,
				const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(interface);
	struct usb_idmouse *dev;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	int result;

	/* check if we have gotten the data or the hid interface */
	iface_desc = interface->cur_altsetting;
	if (iface_desc->desc.bInterfaceClass != 0x0A)
		return -ENODEV;

	if (iface_desc->desc.bNumEndpoints < 1)
		return -ENODEV;

	/* allocate memory for our device state and initialize it */
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (dev == NULL)
		return -ENOMEM;

	mutex_init(&dev->lock);
	dev->udev = udev;
	dev->interface = interface;

	/* set up the endpoint information - use only the first bulk-in endpoint */
	result = usb_find_bulk_in_endpoint(iface_desc, &endpoint);
	if (result) {
		dev_err(&interface->dev, "Unable to find bulk-in endpoint.\n");
		idmouse_delete(dev);
		return result;
	}

	dev->orig_bi_size = usb_endpoint_maxp(endpoint);
	dev->bulk_in_size = 0x200; /* works _much_ faster */
	dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
	dev->bulk_in_buffer = kmalloc(IMGSIZE + dev->bulk_in_size, GFP_KERNEL);
	if (!dev->bulk_in_buffer) {
		idmouse_delete(dev);
		return -ENOMEM;
	}

	/* allow device read, write and ioctl */
	dev->present = 1;

	/* we can register the device now, as it is ready */
	usb_set_intfdata(interface, dev);
	result = usb_register_dev(interface, &idmouse_class);
	if (result) {
		/* something prevented us from registering this device */
		dev_err(&interface->dev, "Unable to allocate minor number.\n");
		idmouse_delete(dev);
		return result;
	}

	/* be noisy */
	dev_info(&interface->dev,"%s now attached\n",DRIVER_DESC);

	return 0;
}

static void idmouse_disconnect(struct usb_interface *interface)
{
	struct usb_idmouse *dev = usb_get_intfdata(interface);

	/* give back our minor */
	usb_deregister_dev(interface, &idmouse_class);

	/* lock the device */
	mutex_lock(&dev->lock);

	/* prevent device read, write and ioctl */
	dev->present = 0;

	/* if the device is opened, idmouse_release will clean this up */
	if (!dev->open) {
		mutex_unlock(&dev->lock);
		idmouse_delete(dev);
	} else {
		/* unlock */
		mutex_unlock(&dev->lock);
	}

	dev_info(&interface->dev, "disconnected\n");
}

module_usb_driver(idmouse_driver);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

