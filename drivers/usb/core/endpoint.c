// SPDX-License-Identifier: GPL-2.0
/*
 * drivers/usb/core/endpoint.c
 *
 * (C) Copyright 2002,2004,2006 Greg Kroah-Hartman
 * (C) Copyright 2002,2004 IBM Corp.
 * (C) Copyright 2006 Novell Inc.
 *
 * Released under the GPLv2 only.
 *
 * Endpoint sysfs stuff
 */

#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include "usb.h"

struct ep_device {
	struct usb_endpoint_descriptor *desc;
	struct usb_device *udev;
	struct device dev;
};
#define to_ep_device(_dev) \
	container_of(_dev, struct ep_device, dev)

struct ep_attribute {
	struct attribute attr;
	ssize_t (*show)(struct usb_device *,
			struct usb_endpoint_descriptor *, char *);
};
#define to_ep_attribute(_attr) \
	container_of(_attr, struct ep_attribute, attr)

#define usb_ep_attr(field, format_string)			\
static ssize_t field##_show(struct device *dev,			\
			       struct device_attribute *attr,	\
			       char *buf)			\
{								\
	struct ep_device *ep = to_ep_device(dev);		\
	return sprintf(buf, format_string, ep->desc->field);	\
}								\
static DEVICE_ATTR_RO(field)

usb_ep_attr(bLength, "%02x\n");
usb_ep_attr(bEndpointAddress, "%02x\n");
usb_ep_attr(bmAttributes, "%02x\n");
usb_ep_attr(bInterval, "%02x\n");

static ssize_t wMaxPacketSize_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct ep_device *ep = to_ep_device(dev);
	return sprintf(buf, "%04x\n", usb_endpoint_maxp(ep->desc));
}
static DEVICE_ATTR_RO(wMaxPacketSize);

static ssize_t type_show(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	struct ep_device *ep = to_ep_device(dev);
	char *type = "unknown";

	switch (usb_endpoint_type(ep->desc)) {
	case USB_ENDPOINT_XFER_CONTROL:
		type = "Control";
		break;
	case USB_ENDPOINT_XFER_ISOC:
		type = "Isoc";
		break;
	case USB_ENDPOINT_XFER_BULK:
		type = "Bulk";
		break;
	case USB_ENDPOINT_XFER_INT:
		type = "Interrupt";
		break;
	}
	return sprintf(buf, "%s\n", type);
}
static DEVICE_ATTR_RO(type);

static ssize_t interval_show(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	struct ep_device *ep = to_ep_device(dev);
	unsigned int interval;
	char unit;

	interval = usb_decode_interval(ep->desc, ep->udev->speed);
	if (interval % 1000) {
		unit = 'u';
	} else {
		unit = 'm';
		interval /= 1000;
	}

	return sprintf(buf, "%d%cs\n", interval, unit);
}
static DEVICE_ATTR_RO(interval);

static ssize_t direction_show(struct device *dev, struct device_attribute *attr,
			      char *buf)
{
	struct ep_device *ep = to_ep_device(dev);
	char *direction;

	if (usb_endpoint_xfer_control(ep->desc))
		direction = "both";
	else if (usb_endpoint_dir_in(ep->desc))
		direction = "in";
	else
		direction = "out";
	return sprintf(buf, "%s\n", direction);
}
static DEVICE_ATTR_RO(direction);

static struct attribute *ep_dev_attrs[] = {
	&dev_attr_bLength.attr,
	&dev_attr_bEndpointAddress.attr,
	&dev_attr_bmAttributes.attr,
	&dev_attr_bInterval.attr,
	&dev_attr_wMaxPacketSize.attr,
	&dev_attr_interval.attr,
	&dev_attr_type.attr,
	&dev_attr_direction.attr,
	NULL,
};
static const struct attribute_group ep_dev_attr_grp = {
	.attrs = ep_dev_attrs,
};
static const struct attribute_group *ep_dev_groups[] = {
	&ep_dev_attr_grp,
	NULL
};

static void ep_device_release(struct device *dev)
{
	struct ep_device *ep_dev = to_ep_device(dev);

	kfree(ep_dev);
}

struct device_type usb_ep_device_type = {
	.name =		"usb_endpoint",
	.release = ep_device_release,
};

int usb_create_ep_devs(struct device *parent,
			struct usb_host_endpoint *endpoint,
			struct usb_device *udev)
{
	struct ep_device *ep_dev;
	int retval;

	ep_dev = kzalloc(sizeof(*ep_dev), GFP_KERNEL);
	if (!ep_dev) {
		retval = -ENOMEM;
		goto exit;
	}

	ep_dev->desc = &endpoint->desc;
	ep_dev->udev = udev;
	ep_dev->dev.groups = ep_dev_groups;
	ep_dev->dev.type = &usb_ep_device_type;
	ep_dev->dev.parent = parent;
	dev_set_name(&ep_dev->dev, "ep_%02x", endpoint->desc.bEndpointAddress);

	retval = device_register(&ep_dev->dev);
	if (retval)
		goto error_register;

	device_enable_async_suspend(&ep_dev->dev);
	endpoint->ep_dev = ep_dev;
	return retval;

error_register:
	put_device(&ep_dev->dev);
exit:
	return retval;
}

void usb_remove_ep_devs(struct usb_host_endpoint *endpoint)
{
	struct ep_device *ep_dev = endpoint->ep_dev;

	if (ep_dev) {
		device_unregister(&ep_dev->dev);
		endpoint->ep_dev = NULL;
	}
}
