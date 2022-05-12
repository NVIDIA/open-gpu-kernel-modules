// SPDX-License-Identifier: GPL-2.0
/*
 * drivers/usb/generic.c - generic driver for USB devices (not interfaces)
 *
 * (C) Copyright 2005 Greg Kroah-Hartman <gregkh@suse.de>
 *
 * based on drivers/usb/usb.c which had the following copyrights:
 *	(C) Copyright Linus Torvalds 1999
 *	(C) Copyright Johannes Erdfelt 1999-2001
 *	(C) Copyright Andreas Gal 1999
 *	(C) Copyright Gregory P. Smith 1999
 *	(C) Copyright Deti Fliegl 1999 (new USB architecture)
 *	(C) Copyright Randy Dunlap 2000
 *	(C) Copyright David Brownell 2000-2004
 *	(C) Copyright Yggdrasil Computing, Inc. 2000
 *		(usb_device_id matching changes by Adam J. Richter)
 *	(C) Copyright Greg Kroah-Hartman 2002-2003
 *
 * Released under the GPLv2 only.
 */

#include <linux/usb.h>
#include <linux/usb/hcd.h>
#include <uapi/linux/usb/audio.h>
#include "usb.h"

static inline const char *plural(int n)
{
	return (n == 1 ? "" : "s");
}

static int is_rndis(struct usb_interface_descriptor *desc)
{
	return desc->bInterfaceClass == USB_CLASS_COMM
		&& desc->bInterfaceSubClass == 2
		&& desc->bInterfaceProtocol == 0xff;
}

static int is_activesync(struct usb_interface_descriptor *desc)
{
	return desc->bInterfaceClass == USB_CLASS_MISC
		&& desc->bInterfaceSubClass == 1
		&& desc->bInterfaceProtocol == 1;
}

static bool is_audio(struct usb_interface_descriptor *desc)
{
	return desc->bInterfaceClass == USB_CLASS_AUDIO;
}

static bool is_uac3_config(struct usb_interface_descriptor *desc)
{
	return desc->bInterfaceProtocol == UAC_VERSION_3;
}

int usb_choose_configuration(struct usb_device *udev)
{
	int i;
	int num_configs;
	int insufficient_power = 0;
	struct usb_host_config *c, *best;

	if (usb_device_is_owned(udev))
		return 0;

	best = NULL;
	c = udev->config;
	num_configs = udev->descriptor.bNumConfigurations;
	for (i = 0; i < num_configs; (i++, c++)) {
		struct usb_interface_descriptor	*desc = NULL;

		/* It's possible that a config has no interfaces! */
		if (c->desc.bNumInterfaces > 0)
			desc = &c->intf_cache[0]->altsetting->desc;

		/*
		 * HP's USB bus-powered keyboard has only one configuration
		 * and it claims to be self-powered; other devices may have
		 * similar errors in their descriptors.  If the next test
		 * were allowed to execute, such configurations would always
		 * be rejected and the devices would not work as expected.
		 * In the meantime, we run the risk of selecting a config
		 * that requires external power at a time when that power
		 * isn't available.  It seems to be the lesser of two evils.
		 *
		 * Bugzilla #6448 reports a device that appears to crash
		 * when it receives a GET_DEVICE_STATUS request!  We don't
		 * have any other way to tell whether a device is self-powered,
		 * but since we don't use that information anywhere but here,
		 * the call has been removed.
		 *
		 * Maybe the GET_DEVICE_STATUS call and the test below can
		 * be reinstated when device firmwares become more reliable.
		 * Don't hold your breath.
		 */
#if 0
		/* Rule out self-powered configs for a bus-powered device */
		if (bus_powered && (c->desc.bmAttributes &
					USB_CONFIG_ATT_SELFPOWER))
			continue;
#endif

		/*
		 * The next test may not be as effective as it should be.
		 * Some hubs have errors in their descriptor, claiming
		 * to be self-powered when they are really bus-powered.
		 * We will overestimate the amount of current such hubs
		 * make available for each port.
		 *
		 * This is a fairly benign sort of failure.  It won't
		 * cause us to reject configurations that we should have
		 * accepted.
		 */

		/* Rule out configs that draw too much bus current */
		if (usb_get_max_power(udev, c) > udev->bus_mA) {
			insufficient_power++;
			continue;
		}

		/*
		 * Select first configuration as default for audio so that
		 * devices that don't comply with UAC3 protocol are supported.
		 * But, still iterate through other configurations and
		 * select UAC3 compliant config if present.
		 */
		if (desc && is_audio(desc)) {
			/* Always prefer the first found UAC3 config */
			if (is_uac3_config(desc)) {
				best = c;
				break;
			}

			/* If there is no UAC3 config, prefer the first config */
			else if (i == 0)
				best = c;

			/* Unconditional continue, because the rest of the code
			 * in the loop is irrelevant for audio devices, and
			 * because it can reassign best, which for audio devices
			 * we don't want.
			 */
			continue;
		}

		/* When the first config's first interface is one of Microsoft's
		 * pet nonstandard Ethernet-over-USB protocols, ignore it unless
		 * this kernel has enabled the necessary host side driver.
		 * But: Don't ignore it if it's the only config.
		 */
		if (i == 0 && num_configs > 1 && desc &&
				(is_rndis(desc) || is_activesync(desc))) {
#if !defined(CONFIG_USB_NET_RNDIS_HOST) && !defined(CONFIG_USB_NET_RNDIS_HOST_MODULE)
			continue;
#else
			best = c;
#endif
		}

		/* From the remaining configs, choose the first one whose
		 * first interface is for a non-vendor-specific class.
		 * Reason: Linux is more likely to have a class driver
		 * than a vendor-specific driver. */
		else if (udev->descriptor.bDeviceClass !=
						USB_CLASS_VENDOR_SPEC &&
				(desc && desc->bInterfaceClass !=
						USB_CLASS_VENDOR_SPEC)) {
			best = c;
			break;
		}

		/* If all the remaining configs are vendor-specific,
		 * choose the first one. */
		else if (!best)
			best = c;
	}

	if (insufficient_power > 0)
		dev_info(&udev->dev, "rejected %d configuration%s "
			"due to insufficient available bus power\n",
			insufficient_power, plural(insufficient_power));

	if (best) {
		i = best->desc.bConfigurationValue;
		dev_dbg(&udev->dev,
			"configuration #%d chosen from %d choice%s\n",
			i, num_configs, plural(num_configs));
	} else {
		i = -1;
		dev_warn(&udev->dev,
			"no configuration chosen from %d choice%s\n",
			num_configs, plural(num_configs));
	}
	return i;
}
EXPORT_SYMBOL_GPL(usb_choose_configuration);

static int __check_for_non_generic_match(struct device_driver *drv, void *data)
{
	struct usb_device *udev = data;
	struct usb_device_driver *udrv;

	if (!is_usb_device_driver(drv))
		return 0;
	udrv = to_usb_device_driver(drv);
	if (udrv == &usb_generic_driver)
		return 0;
	return usb_driver_applicable(udev, udrv);
}

static bool usb_generic_driver_match(struct usb_device *udev)
{
	if (udev->use_generic_driver)
		return true;

	/*
	 * If any other driver wants the device, leave the device to this other
	 * driver.
	 */
	if (bus_for_each_drv(&usb_bus_type, NULL, udev, __check_for_non_generic_match))
		return false;

	return true;
}

int usb_generic_driver_probe(struct usb_device *udev)
{
	int err, c;

	/* Choose and set the configuration.  This registers the interfaces
	 * with the driver core and lets interface drivers bind to them.
	 */
	if (udev->authorized == 0)
		dev_err(&udev->dev, "Device is not authorized for usage\n");
	else {
		c = usb_choose_configuration(udev);
		if (c >= 0) {
			err = usb_set_configuration(udev, c);
			if (err && err != -ENODEV) {
				dev_err(&udev->dev, "can't set config #%d, error %d\n",
					c, err);
				/* This need not be fatal.  The user can try to
				 * set other configurations. */
			}
		}
	}
	/* USB device state == configured ... usable */
	usb_notify_add_device(udev);

	return 0;
}

void usb_generic_driver_disconnect(struct usb_device *udev)
{
	usb_notify_remove_device(udev);

	/* if this is only an unbind, not a physical disconnect, then
	 * unconfigure the device */
	if (udev->actconfig)
		usb_set_configuration(udev, -1);
}

#ifdef	CONFIG_PM

int usb_generic_driver_suspend(struct usb_device *udev, pm_message_t msg)
{
	int rc;

	/* Normal USB devices suspend through their upstream port.
	 * Root hubs don't have upstream ports to suspend,
	 * so we have to shut down their downstream HC-to-USB
	 * interfaces manually by doing a bus (or "global") suspend.
	 */
	if (!udev->parent)
		rc = hcd_bus_suspend(udev, msg);

	/*
	 * Non-root USB2 devices don't need to do anything for FREEZE
	 * or PRETHAW. USB3 devices don't support global suspend and
	 * needs to be selectively suspended.
	 */
	else if ((msg.event == PM_EVENT_FREEZE || msg.event == PM_EVENT_PRETHAW)
		 && (udev->speed < USB_SPEED_SUPER))
		rc = 0;
	else
		rc = usb_port_suspend(udev, msg);

	if (rc == 0)
		usbfs_notify_suspend(udev);
	return rc;
}

int usb_generic_driver_resume(struct usb_device *udev, pm_message_t msg)
{
	int rc;

	/* Normal USB devices resume/reset through their upstream port.
	 * Root hubs don't have upstream ports to resume or reset,
	 * so we have to start up their downstream HC-to-USB
	 * interfaces manually by doing a bus (or "global") resume.
	 */
	if (!udev->parent)
		rc = hcd_bus_resume(udev, msg);
	else
		rc = usb_port_resume(udev, msg);

	if (rc == 0)
		usbfs_notify_resume(udev);
	return rc;
}

#endif	/* CONFIG_PM */

struct usb_device_driver usb_generic_driver = {
	.name =	"usb",
	.match = usb_generic_driver_match,
	.probe = usb_generic_driver_probe,
	.disconnect = usb_generic_driver_disconnect,
#ifdef	CONFIG_PM
	.suspend = usb_generic_driver_suspend,
	.resume = usb_generic_driver_resume,
#endif
	.supports_autosuspend = 1,
};
