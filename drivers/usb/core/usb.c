// SPDX-License-Identifier: GPL-2.0
/*
 * drivers/usb/core/usb.c
 *
 * (C) Copyright Linus Torvalds 1999
 * (C) Copyright Johannes Erdfelt 1999-2001
 * (C) Copyright Andreas Gal 1999
 * (C) Copyright Gregory P. Smith 1999
 * (C) Copyright Deti Fliegl 1999 (new USB architecture)
 * (C) Copyright Randy Dunlap 2000
 * (C) Copyright David Brownell 2000-2004
 * (C) Copyright Yggdrasil Computing, Inc. 2000
 *     (usb_device_id matching changes by Adam J. Richter)
 * (C) Copyright Greg Kroah-Hartman 2002-2003
 *
 * Released under the GPLv2 only.
 *
 * NOTE! This is not actually a driver at all, rather this is
 * just a collection of helper routines that implement the
 * generic USB things that the real drivers can use..
 *
 * Think of this as a "USB library" rather than anything else,
 * with no callbacks.  Callbacks are evil.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/string.h>
#include <linux/bitops.h>
#include <linux/slab.h>
#include <linux/kmod.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/errno.h>
#include <linux/usb.h>
#include <linux/usb/hcd.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/debugfs.h>
#include <linux/usb/of.h>

#include <asm/io.h>
#include <linux/scatterlist.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>

#include "hub.h"

const char *usbcore_name = "usbcore";

static bool nousb;	/* Disable USB when built into kernel image */

module_param(nousb, bool, 0444);

/*
 * for external read access to <nousb>
 */
int usb_disabled(void)
{
	return nousb;
}
EXPORT_SYMBOL_GPL(usb_disabled);

#ifdef	CONFIG_PM
/* Default delay value, in seconds */
static int usb_autosuspend_delay = CONFIG_USB_AUTOSUSPEND_DELAY;
module_param_named(autosuspend, usb_autosuspend_delay, int, 0644);
MODULE_PARM_DESC(autosuspend, "default autosuspend delay");

#else
#define usb_autosuspend_delay		0
#endif

static bool match_endpoint(struct usb_endpoint_descriptor *epd,
		struct usb_endpoint_descriptor **bulk_in,
		struct usb_endpoint_descriptor **bulk_out,
		struct usb_endpoint_descriptor **int_in,
		struct usb_endpoint_descriptor **int_out)
{
	switch (usb_endpoint_type(epd)) {
	case USB_ENDPOINT_XFER_BULK:
		if (usb_endpoint_dir_in(epd)) {
			if (bulk_in && !*bulk_in) {
				*bulk_in = epd;
				break;
			}
		} else {
			if (bulk_out && !*bulk_out) {
				*bulk_out = epd;
				break;
			}
		}

		return false;
	case USB_ENDPOINT_XFER_INT:
		if (usb_endpoint_dir_in(epd)) {
			if (int_in && !*int_in) {
				*int_in = epd;
				break;
			}
		} else {
			if (int_out && !*int_out) {
				*int_out = epd;
				break;
			}
		}

		return false;
	default:
		return false;
	}

	return (!bulk_in || *bulk_in) && (!bulk_out || *bulk_out) &&
			(!int_in || *int_in) && (!int_out || *int_out);
}

/**
 * usb_find_common_endpoints() -- look up common endpoint descriptors
 * @alt:	alternate setting to search
 * @bulk_in:	pointer to descriptor pointer, or NULL
 * @bulk_out:	pointer to descriptor pointer, or NULL
 * @int_in:	pointer to descriptor pointer, or NULL
 * @int_out:	pointer to descriptor pointer, or NULL
 *
 * Search the alternate setting's endpoint descriptors for the first bulk-in,
 * bulk-out, interrupt-in and interrupt-out endpoints and return them in the
 * provided pointers (unless they are NULL).
 *
 * If a requested endpoint is not found, the corresponding pointer is set to
 * NULL.
 *
 * Return: Zero if all requested descriptors were found, or -ENXIO otherwise.
 */
int usb_find_common_endpoints(struct usb_host_interface *alt,
		struct usb_endpoint_descriptor **bulk_in,
		struct usb_endpoint_descriptor **bulk_out,
		struct usb_endpoint_descriptor **int_in,
		struct usb_endpoint_descriptor **int_out)
{
	struct usb_endpoint_descriptor *epd;
	int i;

	if (bulk_in)
		*bulk_in = NULL;
	if (bulk_out)
		*bulk_out = NULL;
	if (int_in)
		*int_in = NULL;
	if (int_out)
		*int_out = NULL;

	for (i = 0; i < alt->desc.bNumEndpoints; ++i) {
		epd = &alt->endpoint[i].desc;

		if (match_endpoint(epd, bulk_in, bulk_out, int_in, int_out))
			return 0;
	}

	return -ENXIO;
}
EXPORT_SYMBOL_GPL(usb_find_common_endpoints);

/**
 * usb_find_common_endpoints_reverse() -- look up common endpoint descriptors
 * @alt:	alternate setting to search
 * @bulk_in:	pointer to descriptor pointer, or NULL
 * @bulk_out:	pointer to descriptor pointer, or NULL
 * @int_in:	pointer to descriptor pointer, or NULL
 * @int_out:	pointer to descriptor pointer, or NULL
 *
 * Search the alternate setting's endpoint descriptors for the last bulk-in,
 * bulk-out, interrupt-in and interrupt-out endpoints and return them in the
 * provided pointers (unless they are NULL).
 *
 * If a requested endpoint is not found, the corresponding pointer is set to
 * NULL.
 *
 * Return: Zero if all requested descriptors were found, or -ENXIO otherwise.
 */
int usb_find_common_endpoints_reverse(struct usb_host_interface *alt,
		struct usb_endpoint_descriptor **bulk_in,
		struct usb_endpoint_descriptor **bulk_out,
		struct usb_endpoint_descriptor **int_in,
		struct usb_endpoint_descriptor **int_out)
{
	struct usb_endpoint_descriptor *epd;
	int i;

	if (bulk_in)
		*bulk_in = NULL;
	if (bulk_out)
		*bulk_out = NULL;
	if (int_in)
		*int_in = NULL;
	if (int_out)
		*int_out = NULL;

	for (i = alt->desc.bNumEndpoints - 1; i >= 0; --i) {
		epd = &alt->endpoint[i].desc;

		if (match_endpoint(epd, bulk_in, bulk_out, int_in, int_out))
			return 0;
	}

	return -ENXIO;
}
EXPORT_SYMBOL_GPL(usb_find_common_endpoints_reverse);

/**
 * usb_find_alt_setting() - Given a configuration, find the alternate setting
 * for the given interface.
 * @config: the configuration to search (not necessarily the current config).
 * @iface_num: interface number to search in
 * @alt_num: alternate interface setting number to search for.
 *
 * Search the configuration's interface cache for the given alt setting.
 *
 * Return: The alternate setting, if found. %NULL otherwise.
 */
struct usb_host_interface *usb_find_alt_setting(
		struct usb_host_config *config,
		unsigned int iface_num,
		unsigned int alt_num)
{
	struct usb_interface_cache *intf_cache = NULL;
	int i;

	if (!config)
		return NULL;
	for (i = 0; i < config->desc.bNumInterfaces; i++) {
		if (config->intf_cache[i]->altsetting[0].desc.bInterfaceNumber
				== iface_num) {
			intf_cache = config->intf_cache[i];
			break;
		}
	}
	if (!intf_cache)
		return NULL;
	for (i = 0; i < intf_cache->num_altsetting; i++)
		if (intf_cache->altsetting[i].desc.bAlternateSetting == alt_num)
			return &intf_cache->altsetting[i];

	printk(KERN_DEBUG "Did not find alt setting %u for intf %u, "
			"config %u\n", alt_num, iface_num,
			config->desc.bConfigurationValue);
	return NULL;
}
EXPORT_SYMBOL_GPL(usb_find_alt_setting);

/**
 * usb_ifnum_to_if - get the interface object with a given interface number
 * @dev: the device whose current configuration is considered
 * @ifnum: the desired interface
 *
 * This walks the device descriptor for the currently active configuration
 * to find the interface object with the particular interface number.
 *
 * Note that configuration descriptors are not required to assign interface
 * numbers sequentially, so that it would be incorrect to assume that
 * the first interface in that descriptor corresponds to interface zero.
 * This routine helps device drivers avoid such mistakes.
 * However, you should make sure that you do the right thing with any
 * alternate settings available for this interfaces.
 *
 * Don't call this function unless you are bound to one of the interfaces
 * on this device or you have locked the device!
 *
 * Return: A pointer to the interface that has @ifnum as interface number,
 * if found. %NULL otherwise.
 */
struct usb_interface *usb_ifnum_to_if(const struct usb_device *dev,
				      unsigned ifnum)
{
	struct usb_host_config *config = dev->actconfig;
	int i;

	if (!config)
		return NULL;
	for (i = 0; i < config->desc.bNumInterfaces; i++)
		if (config->interface[i]->altsetting[0]
				.desc.bInterfaceNumber == ifnum)
			return config->interface[i];

	return NULL;
}
EXPORT_SYMBOL_GPL(usb_ifnum_to_if);

/**
 * usb_altnum_to_altsetting - get the altsetting structure with a given alternate setting number.
 * @intf: the interface containing the altsetting in question
 * @altnum: the desired alternate setting number
 *
 * This searches the altsetting array of the specified interface for
 * an entry with the correct bAlternateSetting value.
 *
 * Note that altsettings need not be stored sequentially by number, so
 * it would be incorrect to assume that the first altsetting entry in
 * the array corresponds to altsetting zero.  This routine helps device
 * drivers avoid such mistakes.
 *
 * Don't call this function unless you are bound to the intf interface
 * or you have locked the device!
 *
 * Return: A pointer to the entry of the altsetting array of @intf that
 * has @altnum as the alternate setting number. %NULL if not found.
 */
struct usb_host_interface *usb_altnum_to_altsetting(
					const struct usb_interface *intf,
					unsigned int altnum)
{
	int i;

	for (i = 0; i < intf->num_altsetting; i++) {
		if (intf->altsetting[i].desc.bAlternateSetting == altnum)
			return &intf->altsetting[i];
	}
	return NULL;
}
EXPORT_SYMBOL_GPL(usb_altnum_to_altsetting);

struct find_interface_arg {
	int minor;
	struct device_driver *drv;
};

static int __find_interface(struct device *dev, const void *data)
{
	const struct find_interface_arg *arg = data;
	struct usb_interface *intf;

	if (!is_usb_interface(dev))
		return 0;

	if (dev->driver != arg->drv)
		return 0;
	intf = to_usb_interface(dev);
	return intf->minor == arg->minor;
}

/**
 * usb_find_interface - find usb_interface pointer for driver and device
 * @drv: the driver whose current configuration is considered
 * @minor: the minor number of the desired device
 *
 * This walks the bus device list and returns a pointer to the interface
 * with the matching minor and driver.  Note, this only works for devices
 * that share the USB major number.
 *
 * Return: A pointer to the interface with the matching major and @minor.
 */
struct usb_interface *usb_find_interface(struct usb_driver *drv, int minor)
{
	struct find_interface_arg argb;
	struct device *dev;

	argb.minor = minor;
	argb.drv = &drv->drvwrap.driver;

	dev = bus_find_device(&usb_bus_type, NULL, &argb, __find_interface);

	/* Drop reference count from bus_find_device */
	put_device(dev);

	return dev ? to_usb_interface(dev) : NULL;
}
EXPORT_SYMBOL_GPL(usb_find_interface);

struct each_dev_arg {
	void *data;
	int (*fn)(struct usb_device *, void *);
};

static int __each_dev(struct device *dev, void *data)
{
	struct each_dev_arg *arg = (struct each_dev_arg *)data;

	/* There are struct usb_interface on the same bus, filter them out */
	if (!is_usb_device(dev))
		return 0;

	return arg->fn(to_usb_device(dev), arg->data);
}

/**
 * usb_for_each_dev - iterate over all USB devices in the system
 * @data: data pointer that will be handed to the callback function
 * @fn: callback function to be called for each USB device
 *
 * Iterate over all USB devices and call @fn for each, passing it @data. If it
 * returns anything other than 0, we break the iteration prematurely and return
 * that value.
 */
int usb_for_each_dev(void *data, int (*fn)(struct usb_device *, void *))
{
	struct each_dev_arg arg = {data, fn};

	return bus_for_each_dev(&usb_bus_type, NULL, &arg, __each_dev);
}
EXPORT_SYMBOL_GPL(usb_for_each_dev);

struct each_hub_arg {
	void *data;
	int (*fn)(struct device *, void *);
};

static int __each_hub(struct usb_device *hdev, void *data)
{
	struct each_hub_arg *arg = (struct each_hub_arg *)data;
	struct usb_hub *hub;
	int ret = 0;
	int i;

	hub = usb_hub_to_struct_hub(hdev);
	if (!hub)
		return 0;

	mutex_lock(&usb_port_peer_mutex);

	for (i = 0; i < hdev->maxchild; i++) {
		ret = arg->fn(&hub->ports[i]->dev, arg->data);
		if (ret)
			break;
	}

	mutex_unlock(&usb_port_peer_mutex);

	return ret;
}

/**
 * usb_for_each_port - interate over all USB ports in the system
 * @data: data pointer that will be handed to the callback function
 * @fn: callback function to be called for each USB port
 *
 * Iterate over all USB ports and call @fn for each, passing it @data. If it
 * returns anything other than 0, we break the iteration prematurely and return
 * that value.
 */
int usb_for_each_port(void *data, int (*fn)(struct device *, void *))
{
	struct each_hub_arg arg = {data, fn};

	return usb_for_each_dev(&arg, __each_hub);
}
EXPORT_SYMBOL_GPL(usb_for_each_port);

/**
 * usb_release_dev - free a usb device structure when all users of it are finished.
 * @dev: device that's been disconnected
 *
 * Will be called only by the device core when all users of this usb device are
 * done.
 */
static void usb_release_dev(struct device *dev)
{
	struct usb_device *udev;
	struct usb_hcd *hcd;

	udev = to_usb_device(dev);
	hcd = bus_to_hcd(udev->bus);

	usb_destroy_configuration(udev);
	usb_release_bos_descriptor(udev);
	of_node_put(dev->of_node);
	usb_put_hcd(hcd);
	kfree(udev->product);
	kfree(udev->manufacturer);
	kfree(udev->serial);
	kfree(udev);
}

static int usb_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	struct usb_device *usb_dev;

	usb_dev = to_usb_device(dev);

	if (add_uevent_var(env, "BUSNUM=%03d", usb_dev->bus->busnum))
		return -ENOMEM;

	if (add_uevent_var(env, "DEVNUM=%03d", usb_dev->devnum))
		return -ENOMEM;

	return 0;
}

#ifdef	CONFIG_PM

/* USB device Power-Management thunks.
 * There's no need to distinguish here between quiescing a USB device
 * and powering it down; the generic_suspend() routine takes care of
 * it by skipping the usb_port_suspend() call for a quiesce.  And for
 * USB interfaces there's no difference at all.
 */

static int usb_dev_prepare(struct device *dev)
{
	return 0;		/* Implement eventually? */
}

static void usb_dev_complete(struct device *dev)
{
	/* Currently used only for rebinding interfaces */
	usb_resume_complete(dev);
}

static int usb_dev_suspend(struct device *dev)
{
	return usb_suspend(dev, PMSG_SUSPEND);
}

static int usb_dev_resume(struct device *dev)
{
	return usb_resume(dev, PMSG_RESUME);
}

static int usb_dev_freeze(struct device *dev)
{
	return usb_suspend(dev, PMSG_FREEZE);
}

static int usb_dev_thaw(struct device *dev)
{
	return usb_resume(dev, PMSG_THAW);
}

static int usb_dev_poweroff(struct device *dev)
{
	return usb_suspend(dev, PMSG_HIBERNATE);
}

static int usb_dev_restore(struct device *dev)
{
	return usb_resume(dev, PMSG_RESTORE);
}

static const struct dev_pm_ops usb_device_pm_ops = {
	.prepare =	usb_dev_prepare,
	.complete =	usb_dev_complete,
	.suspend =	usb_dev_suspend,
	.resume =	usb_dev_resume,
	.freeze =	usb_dev_freeze,
	.thaw =		usb_dev_thaw,
	.poweroff =	usb_dev_poweroff,
	.restore =	usb_dev_restore,
	.runtime_suspend =	usb_runtime_suspend,
	.runtime_resume =	usb_runtime_resume,
	.runtime_idle =		usb_runtime_idle,
};

#endif	/* CONFIG_PM */


static char *usb_devnode(struct device *dev,
			 umode_t *mode, kuid_t *uid, kgid_t *gid)
{
	struct usb_device *usb_dev;

	usb_dev = to_usb_device(dev);
	return kasprintf(GFP_KERNEL, "bus/usb/%03d/%03d",
			 usb_dev->bus->busnum, usb_dev->devnum);
}

struct device_type usb_device_type = {
	.name =		"usb_device",
	.release =	usb_release_dev,
	.uevent =	usb_dev_uevent,
	.devnode = 	usb_devnode,
#ifdef CONFIG_PM
	.pm =		&usb_device_pm_ops,
#endif
};


/* Returns 1 if @usb_bus is WUSB, 0 otherwise */
static unsigned usb_bus_is_wusb(struct usb_bus *bus)
{
	struct usb_hcd *hcd = bus_to_hcd(bus);
	return hcd->wireless;
}

static bool usb_dev_authorized(struct usb_device *dev, struct usb_hcd *hcd)
{
	struct usb_hub *hub;

	if (!dev->parent)
		return true; /* Root hub always ok [and always wired] */

	switch (hcd->dev_policy) {
	case USB_DEVICE_AUTHORIZE_NONE:
	default:
		return false;

	case USB_DEVICE_AUTHORIZE_ALL:
		return true;

	case USB_DEVICE_AUTHORIZE_INTERNAL:
		hub = usb_hub_to_struct_hub(dev->parent);
		return hub->ports[dev->portnum - 1]->connect_type ==
				USB_PORT_CONNECT_TYPE_HARD_WIRED;
	}
}

/**
 * usb_alloc_dev - usb device constructor (usbcore-internal)
 * @parent: hub to which device is connected; null to allocate a root hub
 * @bus: bus used to access the device
 * @port1: one-based index of port; ignored for root hubs
 *
 * Context: task context, might sleep.
 *
 * Only hub drivers (including virtual root hub drivers for host
 * controllers) should ever call this.
 *
 * This call may not be used in a non-sleeping context.
 *
 * Return: On success, a pointer to the allocated usb device. %NULL on
 * failure.
 */
struct usb_device *usb_alloc_dev(struct usb_device *parent,
				 struct usb_bus *bus, unsigned port1)
{
	struct usb_device *dev;
	struct usb_hcd *usb_hcd = bus_to_hcd(bus);
	unsigned root_hub = 0;
	unsigned raw_port = port1;

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return NULL;

	if (!usb_get_hcd(usb_hcd)) {
		kfree(dev);
		return NULL;
	}
	/* Root hubs aren't true devices, so don't allocate HCD resources */
	if (usb_hcd->driver->alloc_dev && parent &&
		!usb_hcd->driver->alloc_dev(usb_hcd, dev)) {
		usb_put_hcd(bus_to_hcd(bus));
		kfree(dev);
		return NULL;
	}

	device_initialize(&dev->dev);
	dev->dev.bus = &usb_bus_type;
	dev->dev.type = &usb_device_type;
	dev->dev.groups = usb_device_groups;
	set_dev_node(&dev->dev, dev_to_node(bus->sysdev));
	dev->state = USB_STATE_ATTACHED;
	dev->lpm_disable_count = 1;
	atomic_set(&dev->urbnum, 0);

	INIT_LIST_HEAD(&dev->ep0.urb_list);
	dev->ep0.desc.bLength = USB_DT_ENDPOINT_SIZE;
	dev->ep0.desc.bDescriptorType = USB_DT_ENDPOINT;
	/* ep0 maxpacket comes later, from device descriptor */
	usb_enable_endpoint(dev, &dev->ep0, false);
	dev->can_submit = 1;

	/* Save readable and stable topology id, distinguishing devices
	 * by location for diagnostics, tools, driver model, etc.  The
	 * string is a path along hub ports, from the root.  Each device's
	 * dev->devpath will be stable until USB is re-cabled, and hubs
	 * are often labeled with these port numbers.  The name isn't
	 * as stable:  bus->busnum changes easily from modprobe order,
	 * cardbus or pci hotplugging, and so on.
	 */
	if (unlikely(!parent)) {
		dev->devpath[0] = '0';
		dev->route = 0;

		dev->dev.parent = bus->controller;
		device_set_of_node_from_dev(&dev->dev, bus->sysdev);
		dev_set_name(&dev->dev, "usb%d", bus->busnum);
		root_hub = 1;
	} else {
		/* match any labeling on the hubs; it's one-based */
		if (parent->devpath[0] == '0') {
			snprintf(dev->devpath, sizeof dev->devpath,
				"%d", port1);
			/* Root ports are not counted in route string */
			dev->route = 0;
		} else {
			snprintf(dev->devpath, sizeof dev->devpath,
				"%s.%d", parent->devpath, port1);
			/* Route string assumes hubs have less than 16 ports */
			if (port1 < 15)
				dev->route = parent->route +
					(port1 << ((parent->level - 1)*4));
			else
				dev->route = parent->route +
					(15 << ((parent->level - 1)*4));
		}

		dev->dev.parent = &parent->dev;
		dev_set_name(&dev->dev, "%d-%s", bus->busnum, dev->devpath);

		if (!parent->parent) {
			/* device under root hub's port */
			raw_port = usb_hcd_find_raw_port_number(usb_hcd,
				port1);
		}
		dev->dev.of_node = usb_of_get_device_node(parent, raw_port);

		/* hub driver sets up TT records */
	}

	dev->portnum = port1;
	dev->bus = bus;
	dev->parent = parent;
	INIT_LIST_HEAD(&dev->filelist);

#ifdef	CONFIG_PM
	pm_runtime_set_autosuspend_delay(&dev->dev,
			usb_autosuspend_delay * 1000);
	dev->connect_time = jiffies;
	dev->active_duration = -jiffies;
#endif

	dev->authorized = usb_dev_authorized(dev, usb_hcd);
	if (!root_hub)
		dev->wusb = usb_bus_is_wusb(bus) ? 1 : 0;

	return dev;
}
EXPORT_SYMBOL_GPL(usb_alloc_dev);

/**
 * usb_get_dev - increments the reference count of the usb device structure
 * @dev: the device being referenced
 *
 * Each live reference to a device should be refcounted.
 *
 * Drivers for USB interfaces should normally record such references in
 * their probe() methods, when they bind to an interface, and release
 * them by calling usb_put_dev(), in their disconnect() methods.
 *
 * Return: A pointer to the device with the incremented reference counter.
 */
struct usb_device *usb_get_dev(struct usb_device *dev)
{
	if (dev)
		get_device(&dev->dev);
	return dev;
}
EXPORT_SYMBOL_GPL(usb_get_dev);

/**
 * usb_put_dev - release a use of the usb device structure
 * @dev: device that's been disconnected
 *
 * Must be called when a user of a device is finished with it.  When the last
 * user of the device calls this function, the memory of the device is freed.
 */
void usb_put_dev(struct usb_device *dev)
{
	if (dev)
		put_device(&dev->dev);
}
EXPORT_SYMBOL_GPL(usb_put_dev);

/**
 * usb_get_intf - increments the reference count of the usb interface structure
 * @intf: the interface being referenced
 *
 * Each live reference to a interface must be refcounted.
 *
 * Drivers for USB interfaces should normally record such references in
 * their probe() methods, when they bind to an interface, and release
 * them by calling usb_put_intf(), in their disconnect() methods.
 *
 * Return: A pointer to the interface with the incremented reference counter.
 */
struct usb_interface *usb_get_intf(struct usb_interface *intf)
{
	if (intf)
		get_device(&intf->dev);
	return intf;
}
EXPORT_SYMBOL_GPL(usb_get_intf);

/**
 * usb_put_intf - release a use of the usb interface structure
 * @intf: interface that's been decremented
 *
 * Must be called when a user of an interface is finished with it.  When the
 * last user of the interface calls this function, the memory of the interface
 * is freed.
 */
void usb_put_intf(struct usb_interface *intf)
{
	if (intf)
		put_device(&intf->dev);
}
EXPORT_SYMBOL_GPL(usb_put_intf);

/**
 * usb_intf_get_dma_device - acquire a reference on the usb interface's DMA endpoint
 * @intf: the usb interface
 *
 * While a USB device cannot perform DMA operations by itself, many USB
 * controllers can. A call to usb_intf_get_dma_device() returns the DMA endpoint
 * for the given USB interface, if any. The returned device structure must be
 * released with put_device().
 *
 * See also usb_get_dma_device().
 *
 * Returns: A reference to the usb interface's DMA endpoint; or NULL if none
 *          exists.
 */
struct device *usb_intf_get_dma_device(struct usb_interface *intf)
{
	struct usb_device *udev = interface_to_usbdev(intf);
	struct device *dmadev;

	if (!udev->bus)
		return NULL;

	dmadev = get_device(udev->bus->sysdev);
	if (!dmadev || !dmadev->dma_mask) {
		put_device(dmadev);
		return NULL;
	}

	return dmadev;
}
EXPORT_SYMBOL_GPL(usb_intf_get_dma_device);

/*			USB device locking
 *
 * USB devices and interfaces are locked using the semaphore in their
 * embedded struct device.  The hub driver guarantees that whenever a
 * device is connected or disconnected, drivers are called with the
 * USB device locked as well as their particular interface.
 *
 * Complications arise when several devices are to be locked at the same
 * time.  Only hub-aware drivers that are part of usbcore ever have to
 * do this; nobody else needs to worry about it.  The rule for locking
 * is simple:
 *
 *	When locking both a device and its parent, always lock the
 *	the parent first.
 */

/**
 * usb_lock_device_for_reset - cautiously acquire the lock for a usb device structure
 * @udev: device that's being locked
 * @iface: interface bound to the driver making the request (optional)
 *
 * Attempts to acquire the device lock, but fails if the device is
 * NOTATTACHED or SUSPENDED, or if iface is specified and the interface
 * is neither BINDING nor BOUND.  Rather than sleeping to wait for the
 * lock, the routine polls repeatedly.  This is to prevent deadlock with
 * disconnect; in some drivers (such as usb-storage) the disconnect()
 * or suspend() method will block waiting for a device reset to complete.
 *
 * Return: A negative error code for failure, otherwise 0.
 */
int usb_lock_device_for_reset(struct usb_device *udev,
			      const struct usb_interface *iface)
{
	unsigned long jiffies_expire = jiffies + HZ;

	if (udev->state == USB_STATE_NOTATTACHED)
		return -ENODEV;
	if (udev->state == USB_STATE_SUSPENDED)
		return -EHOSTUNREACH;
	if (iface && (iface->condition == USB_INTERFACE_UNBINDING ||
			iface->condition == USB_INTERFACE_UNBOUND))
		return -EINTR;

	while (!usb_trylock_device(udev)) {

		/* If we can't acquire the lock after waiting one second,
		 * we're probably deadlocked */
		if (time_after(jiffies, jiffies_expire))
			return -EBUSY;

		msleep(15);
		if (udev->state == USB_STATE_NOTATTACHED)
			return -ENODEV;
		if (udev->state == USB_STATE_SUSPENDED)
			return -EHOSTUNREACH;
		if (iface && (iface->condition == USB_INTERFACE_UNBINDING ||
				iface->condition == USB_INTERFACE_UNBOUND))
			return -EINTR;
	}
	return 0;
}
EXPORT_SYMBOL_GPL(usb_lock_device_for_reset);

/**
 * usb_get_current_frame_number - return current bus frame number
 * @dev: the device whose bus is being queried
 *
 * Return: The current frame number for the USB host controller used
 * with the given USB device. This can be used when scheduling
 * isochronous requests.
 *
 * Note: Different kinds of host controller have different "scheduling
 * horizons". While one type might support scheduling only 32 frames
 * into the future, others could support scheduling up to 1024 frames
 * into the future.
 *
 */
int usb_get_current_frame_number(struct usb_device *dev)
{
	return usb_hcd_get_frame_number(dev);
}
EXPORT_SYMBOL_GPL(usb_get_current_frame_number);

/*-------------------------------------------------------------------*/
/*
 * __usb_get_extra_descriptor() finds a descriptor of specific type in the
 * extra field of the interface and endpoint descriptor structs.
 */

int __usb_get_extra_descriptor(char *buffer, unsigned size,
			       unsigned char type, void **ptr, size_t minsize)
{
	struct usb_descriptor_header *header;

	while (size >= sizeof(struct usb_descriptor_header)) {
		header = (struct usb_descriptor_header *)buffer;

		if (header->bLength < 2 || header->bLength > size) {
			printk(KERN_ERR
				"%s: bogus descriptor, type %d length %d\n",
				usbcore_name,
				header->bDescriptorType,
				header->bLength);
			return -1;
		}

		if (header->bDescriptorType == type && header->bLength >= minsize) {
			*ptr = header;
			return 0;
		}

		buffer += header->bLength;
		size -= header->bLength;
	}
	return -1;
}
EXPORT_SYMBOL_GPL(__usb_get_extra_descriptor);

/**
 * usb_alloc_coherent - allocate dma-consistent buffer for URB_NO_xxx_DMA_MAP
 * @dev: device the buffer will be used with
 * @size: requested buffer size
 * @mem_flags: affect whether allocation may block
 * @dma: used to return DMA address of buffer
 *
 * Return: Either null (indicating no buffer could be allocated), or the
 * cpu-space pointer to a buffer that may be used to perform DMA to the
 * specified device.  Such cpu-space buffers are returned along with the DMA
 * address (through the pointer provided).
 *
 * Note:
 * These buffers are used with URB_NO_xxx_DMA_MAP set in urb->transfer_flags
 * to avoid behaviors like using "DMA bounce buffers", or thrashing IOMMU
 * hardware during URB completion/resubmit.  The implementation varies between
 * platforms, depending on details of how DMA will work to this device.
 * Using these buffers also eliminates cacheline sharing problems on
 * architectures where CPU caches are not DMA-coherent.  On systems without
 * bus-snooping caches, these buffers are uncached.
 *
 * When the buffer is no longer used, free it with usb_free_coherent().
 */
void *usb_alloc_coherent(struct usb_device *dev, size_t size, gfp_t mem_flags,
			 dma_addr_t *dma)
{
	if (!dev || !dev->bus)
		return NULL;
	return hcd_buffer_alloc(dev->bus, size, mem_flags, dma);
}
EXPORT_SYMBOL_GPL(usb_alloc_coherent);

/**
 * usb_free_coherent - free memory allocated with usb_alloc_coherent()
 * @dev: device the buffer was used with
 * @size: requested buffer size
 * @addr: CPU address of buffer
 * @dma: DMA address of buffer
 *
 * This reclaims an I/O buffer, letting it be reused.  The memory must have
 * been allocated using usb_alloc_coherent(), and the parameters must match
 * those provided in that allocation request.
 */
void usb_free_coherent(struct usb_device *dev, size_t size, void *addr,
		       dma_addr_t dma)
{
	if (!dev || !dev->bus)
		return;
	if (!addr)
		return;
	hcd_buffer_free(dev->bus, size, addr, dma);
}
EXPORT_SYMBOL_GPL(usb_free_coherent);

/*
 * Notifications of device and interface registration
 */
static int usb_bus_notify(struct notifier_block *nb, unsigned long action,
		void *data)
{
	struct device *dev = data;

	switch (action) {
	case BUS_NOTIFY_ADD_DEVICE:
		if (dev->type == &usb_device_type)
			(void) usb_create_sysfs_dev_files(to_usb_device(dev));
		else if (dev->type == &usb_if_device_type)
			usb_create_sysfs_intf_files(to_usb_interface(dev));
		break;

	case BUS_NOTIFY_DEL_DEVICE:
		if (dev->type == &usb_device_type)
			usb_remove_sysfs_dev_files(to_usb_device(dev));
		else if (dev->type == &usb_if_device_type)
			usb_remove_sysfs_intf_files(to_usb_interface(dev));
		break;
	}
	return 0;
}

static struct notifier_block usb_bus_nb = {
	.notifier_call = usb_bus_notify,
};

static void usb_debugfs_init(void)
{
	debugfs_create_file("devices", 0444, usb_debug_root, NULL,
			    &usbfs_devices_fops);
}

static void usb_debugfs_cleanup(void)
{
	debugfs_remove(debugfs_lookup("devices", usb_debug_root));
}

/*
 * Init
 */
static int __init usb_init(void)
{
	int retval;
	if (usb_disabled()) {
		pr_info("%s: USB support disabled\n", usbcore_name);
		return 0;
	}
	usb_init_pool_max();

	usb_debugfs_init();

	usb_acpi_register();
	retval = bus_register(&usb_bus_type);
	if (retval)
		goto bus_register_failed;
	retval = bus_register_notifier(&usb_bus_type, &usb_bus_nb);
	if (retval)
		goto bus_notifier_failed;
	retval = usb_major_init();
	if (retval)
		goto major_init_failed;
	retval = usb_register(&usbfs_driver);
	if (retval)
		goto driver_register_failed;
	retval = usb_devio_init();
	if (retval)
		goto usb_devio_init_failed;
	retval = usb_hub_init();
	if (retval)
		goto hub_init_failed;
	retval = usb_register_device_driver(&usb_generic_driver, THIS_MODULE);
	if (!retval)
		goto out;

	usb_hub_cleanup();
hub_init_failed:
	usb_devio_cleanup();
usb_devio_init_failed:
	usb_deregister(&usbfs_driver);
driver_register_failed:
	usb_major_cleanup();
major_init_failed:
	bus_unregister_notifier(&usb_bus_type, &usb_bus_nb);
bus_notifier_failed:
	bus_unregister(&usb_bus_type);
bus_register_failed:
	usb_acpi_unregister();
	usb_debugfs_cleanup();
out:
	return retval;
}

/*
 * Cleanup
 */
static void __exit usb_exit(void)
{
	/* This will matter if shutdown/reboot does exitcalls. */
	if (usb_disabled())
		return;

	usb_release_quirk_list();
	usb_deregister_device_driver(&usb_generic_driver);
	usb_major_cleanup();
	usb_deregister(&usbfs_driver);
	usb_devio_cleanup();
	usb_hub_cleanup();
	bus_unregister_notifier(&usb_bus_type, &usb_bus_nb);
	bus_unregister(&usb_bus_type);
	usb_acpi_unregister();
	usb_debugfs_cleanup();
	idr_destroy(&usb_bus_idr);
}

subsys_initcall(usb_init);
module_exit(usb_exit);
MODULE_LICENSE("GPL");
