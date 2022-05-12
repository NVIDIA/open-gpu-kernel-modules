// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Roccat Isku driver for Linux
 *
 * Copyright (c) 2011 Stefan Achatz <erazor_de@users.sourceforge.net>
 */

/*
 */

/*
 * Roccat Isku is a gamer keyboard with macro keys that can be configured in
 * 5 profiles.
 */

#include <linux/device.h>
#include <linux/input.h>
#include <linux/hid.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/hid-roccat.h>
#include "hid-ids.h"
#include "hid-roccat-common.h"
#include "hid-roccat-isku.h"

static struct class *isku_class;

static void isku_profile_activated(struct isku_device *isku, uint new_profile)
{
	isku->actual_profile = new_profile;
}

static int isku_receive(struct usb_device *usb_dev, uint command,
		void *buf, uint size)
{
	return roccat_common2_receive(usb_dev, command, buf, size);
}

static int isku_get_actual_profile(struct usb_device *usb_dev)
{
	struct isku_actual_profile buf;
	int retval;

	retval = isku_receive(usb_dev, ISKU_COMMAND_ACTUAL_PROFILE,
			&buf, sizeof(struct isku_actual_profile));
	return retval ? retval : buf.actual_profile;
}

static int isku_set_actual_profile(struct usb_device *usb_dev, int new_profile)
{
	struct isku_actual_profile buf;

	buf.command = ISKU_COMMAND_ACTUAL_PROFILE;
	buf.size = sizeof(struct isku_actual_profile);
	buf.actual_profile = new_profile;
	return roccat_common2_send_with_status(usb_dev,
			ISKU_COMMAND_ACTUAL_PROFILE, &buf,
			sizeof(struct isku_actual_profile));
}

static ssize_t isku_sysfs_show_actual_profile(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct isku_device *isku =
			hid_get_drvdata(dev_get_drvdata(dev->parent->parent));
	return snprintf(buf, PAGE_SIZE, "%d\n", isku->actual_profile);
}

static ssize_t isku_sysfs_set_actual_profile(struct device *dev,
		struct device_attribute *attr, char const *buf, size_t size)
{
	struct isku_device *isku;
	struct usb_device *usb_dev;
	unsigned long profile;
	int retval;
	struct isku_roccat_report roccat_report;

	dev = dev->parent->parent;
	isku = hid_get_drvdata(dev_get_drvdata(dev));
	usb_dev = interface_to_usbdev(to_usb_interface(dev));

	retval = kstrtoul(buf, 10, &profile);
	if (retval)
		return retval;

	if (profile > 4)
		return -EINVAL;

	mutex_lock(&isku->isku_lock);

	retval = isku_set_actual_profile(usb_dev, profile);
	if (retval) {
		mutex_unlock(&isku->isku_lock);
		return retval;
	}

	isku_profile_activated(isku, profile);

	roccat_report.event = ISKU_REPORT_BUTTON_EVENT_PROFILE;
	roccat_report.data1 = profile + 1;
	roccat_report.data2 = 0;
	roccat_report.profile = profile + 1;
	roccat_report_event(isku->chrdev_minor, (uint8_t const *)&roccat_report);

	mutex_unlock(&isku->isku_lock);

	return size;
}
static DEVICE_ATTR(actual_profile, 0660, isku_sysfs_show_actual_profile,
		   isku_sysfs_set_actual_profile);

static struct attribute *isku_attrs[] = {
	&dev_attr_actual_profile.attr,
	NULL,
};

static ssize_t isku_sysfs_read(struct file *fp, struct kobject *kobj,
		char *buf, loff_t off, size_t count,
		size_t real_size, uint command)
{
	struct device *dev = kobj_to_dev(kobj)->parent->parent;
	struct isku_device *isku = hid_get_drvdata(dev_get_drvdata(dev));
	struct usb_device *usb_dev = interface_to_usbdev(to_usb_interface(dev));
	int retval;

	if (off >= real_size)
		return 0;

	if (off != 0 || count > real_size)
		return -EINVAL;

	mutex_lock(&isku->isku_lock);
	retval = isku_receive(usb_dev, command, buf, count);
	mutex_unlock(&isku->isku_lock);

	return retval ? retval : count;
}

static ssize_t isku_sysfs_write(struct file *fp, struct kobject *kobj,
		void const *buf, loff_t off, size_t count,
		size_t real_size, uint command)
{
	struct device *dev = kobj_to_dev(kobj)->parent->parent;
	struct isku_device *isku = hid_get_drvdata(dev_get_drvdata(dev));
	struct usb_device *usb_dev = interface_to_usbdev(to_usb_interface(dev));
	int retval;

	if (off != 0 || count > real_size)
		return -EINVAL;

	mutex_lock(&isku->isku_lock);
	retval = roccat_common2_send_with_status(usb_dev, command,
			(void *)buf, count);
	mutex_unlock(&isku->isku_lock);

	return retval ? retval : count;
}

#define ISKU_SYSFS_W(thingy, THINGY) \
static ssize_t isku_sysfs_write_ ## thingy(struct file *fp, struct kobject *kobj, \
		struct bin_attribute *attr, char *buf, \
		loff_t off, size_t count) \
{ \
	return isku_sysfs_write(fp, kobj, buf, off, count, \
			ISKU_SIZE_ ## THINGY, ISKU_COMMAND_ ## THINGY); \
}

#define ISKU_SYSFS_R(thingy, THINGY) \
static ssize_t isku_sysfs_read_ ## thingy(struct file *fp, struct kobject *kobj, \
		struct bin_attribute *attr, char *buf, \
		loff_t off, size_t count) \
{ \
	return isku_sysfs_read(fp, kobj, buf, off, count, \
			ISKU_SIZE_ ## THINGY, ISKU_COMMAND_ ## THINGY); \
}

#define ISKU_SYSFS_RW(thingy, THINGY) \
ISKU_SYSFS_R(thingy, THINGY) \
ISKU_SYSFS_W(thingy, THINGY)

#define ISKU_BIN_ATTR_RW(thingy, THINGY) \
ISKU_SYSFS_RW(thingy, THINGY); \
static struct bin_attribute bin_attr_##thingy = { \
	.attr = { .name = #thingy, .mode = 0660 }, \
	.size = ISKU_SIZE_ ## THINGY, \
	.read = isku_sysfs_read_ ## thingy, \
	.write = isku_sysfs_write_ ## thingy \
}

#define ISKU_BIN_ATTR_R(thingy, THINGY) \
ISKU_SYSFS_R(thingy, THINGY); \
static struct bin_attribute bin_attr_##thingy = { \
	.attr = { .name = #thingy, .mode = 0440 }, \
	.size = ISKU_SIZE_ ## THINGY, \
	.read = isku_sysfs_read_ ## thingy, \
}

#define ISKU_BIN_ATTR_W(thingy, THINGY) \
ISKU_SYSFS_W(thingy, THINGY); \
static struct bin_attribute bin_attr_##thingy = { \
	.attr = { .name = #thingy, .mode = 0220 }, \
	.size = ISKU_SIZE_ ## THINGY, \
	.write = isku_sysfs_write_ ## thingy \
}

ISKU_BIN_ATTR_RW(macro, MACRO);
ISKU_BIN_ATTR_RW(keys_function, KEYS_FUNCTION);
ISKU_BIN_ATTR_RW(keys_easyzone, KEYS_EASYZONE);
ISKU_BIN_ATTR_RW(keys_media, KEYS_MEDIA);
ISKU_BIN_ATTR_RW(keys_thumbster, KEYS_THUMBSTER);
ISKU_BIN_ATTR_RW(keys_macro, KEYS_MACRO);
ISKU_BIN_ATTR_RW(keys_capslock, KEYS_CAPSLOCK);
ISKU_BIN_ATTR_RW(light, LIGHT);
ISKU_BIN_ATTR_RW(key_mask, KEY_MASK);
ISKU_BIN_ATTR_RW(last_set, LAST_SET);
ISKU_BIN_ATTR_W(talk, TALK);
ISKU_BIN_ATTR_W(talkfx, TALKFX);
ISKU_BIN_ATTR_W(control, CONTROL);
ISKU_BIN_ATTR_W(reset, RESET);
ISKU_BIN_ATTR_R(info, INFO);

static struct bin_attribute *isku_bin_attributes[] = {
	&bin_attr_macro,
	&bin_attr_keys_function,
	&bin_attr_keys_easyzone,
	&bin_attr_keys_media,
	&bin_attr_keys_thumbster,
	&bin_attr_keys_macro,
	&bin_attr_keys_capslock,
	&bin_attr_light,
	&bin_attr_key_mask,
	&bin_attr_last_set,
	&bin_attr_talk,
	&bin_attr_talkfx,
	&bin_attr_control,
	&bin_attr_reset,
	&bin_attr_info,
	NULL,
};

static const struct attribute_group isku_group = {
	.attrs = isku_attrs,
	.bin_attrs = isku_bin_attributes,
};

static const struct attribute_group *isku_groups[] = {
	&isku_group,
	NULL,
};

static int isku_init_isku_device_struct(struct usb_device *usb_dev,
		struct isku_device *isku)
{
	int retval;

	mutex_init(&isku->isku_lock);

	retval = isku_get_actual_profile(usb_dev);
	if (retval < 0)
		return retval;
	isku_profile_activated(isku, retval);

	return 0;
}

static int isku_init_specials(struct hid_device *hdev)
{
	struct usb_interface *intf = to_usb_interface(hdev->dev.parent);
	struct usb_device *usb_dev = interface_to_usbdev(intf);
	struct isku_device *isku;
	int retval;

	if (intf->cur_altsetting->desc.bInterfaceProtocol
			!= ISKU_USB_INTERFACE_PROTOCOL) {
		hid_set_drvdata(hdev, NULL);
		return 0;
	}

	isku = kzalloc(sizeof(*isku), GFP_KERNEL);
	if (!isku) {
		hid_err(hdev, "can't alloc device descriptor\n");
		return -ENOMEM;
	}
	hid_set_drvdata(hdev, isku);

	retval = isku_init_isku_device_struct(usb_dev, isku);
	if (retval) {
		hid_err(hdev, "couldn't init struct isku_device\n");
		goto exit_free;
	}

	retval = roccat_connect(isku_class, hdev,
			sizeof(struct isku_roccat_report));
	if (retval < 0) {
		hid_err(hdev, "couldn't init char dev\n");
	} else {
		isku->chrdev_minor = retval;
		isku->roccat_claimed = 1;
	}

	return 0;
exit_free:
	kfree(isku);
	return retval;
}

static void isku_remove_specials(struct hid_device *hdev)
{
	struct usb_interface *intf = to_usb_interface(hdev->dev.parent);
	struct isku_device *isku;

	if (intf->cur_altsetting->desc.bInterfaceProtocol
			!= ISKU_USB_INTERFACE_PROTOCOL)
		return;

	isku = hid_get_drvdata(hdev);
	if (isku->roccat_claimed)
		roccat_disconnect(isku->chrdev_minor);
	kfree(isku);
}

static int isku_probe(struct hid_device *hdev,
		const struct hid_device_id *id)
{
	int retval;

	retval = hid_parse(hdev);
	if (retval) {
		hid_err(hdev, "parse failed\n");
		goto exit;
	}

	retval = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	if (retval) {
		hid_err(hdev, "hw start failed\n");
		goto exit;
	}

	retval = isku_init_specials(hdev);
	if (retval) {
		hid_err(hdev, "couldn't install keyboard\n");
		goto exit_stop;
	}

	return 0;

exit_stop:
	hid_hw_stop(hdev);
exit:
	return retval;
}

static void isku_remove(struct hid_device *hdev)
{
	isku_remove_specials(hdev);
	hid_hw_stop(hdev);
}

static void isku_keep_values_up_to_date(struct isku_device *isku,
		u8 const *data)
{
	struct isku_report_button const *button_report;

	switch (data[0]) {
	case ISKU_REPORT_NUMBER_BUTTON:
		button_report = (struct isku_report_button const *)data;
		switch (button_report->event) {
		case ISKU_REPORT_BUTTON_EVENT_PROFILE:
			isku_profile_activated(isku, button_report->data1 - 1);
			break;
		}
		break;
	}
}

static void isku_report_to_chrdev(struct isku_device const *isku,
		u8 const *data)
{
	struct isku_roccat_report roccat_report;
	struct isku_report_button const *button_report;

	if (data[0] != ISKU_REPORT_NUMBER_BUTTON)
		return;

	button_report = (struct isku_report_button const *)data;

	roccat_report.event = button_report->event;
	roccat_report.data1 = button_report->data1;
	roccat_report.data2 = button_report->data2;
	roccat_report.profile = isku->actual_profile + 1;
	roccat_report_event(isku->chrdev_minor,
			(uint8_t const *)&roccat_report);
}

static int isku_raw_event(struct hid_device *hdev,
		struct hid_report *report, u8 *data, int size)
{
	struct usb_interface *intf = to_usb_interface(hdev->dev.parent);
	struct isku_device *isku = hid_get_drvdata(hdev);

	if (intf->cur_altsetting->desc.bInterfaceProtocol
			!= ISKU_USB_INTERFACE_PROTOCOL)
		return 0;

	if (isku == NULL)
		return 0;

	isku_keep_values_up_to_date(isku, data);

	if (isku->roccat_claimed)
		isku_report_to_chrdev(isku, data);

	return 0;
}

static const struct hid_device_id isku_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_ROCCAT, USB_DEVICE_ID_ROCCAT_ISKU) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_ROCCAT, USB_DEVICE_ID_ROCCAT_ISKUFX) },
	{ }
};

MODULE_DEVICE_TABLE(hid, isku_devices);

static struct hid_driver isku_driver = {
		.name = "isku",
		.id_table = isku_devices,
		.probe = isku_probe,
		.remove = isku_remove,
		.raw_event = isku_raw_event
};

static int __init isku_init(void)
{
	int retval;
	isku_class = class_create(THIS_MODULE, "isku");
	if (IS_ERR(isku_class))
		return PTR_ERR(isku_class);
	isku_class->dev_groups = isku_groups;

	retval = hid_register_driver(&isku_driver);
	if (retval)
		class_destroy(isku_class);
	return retval;
}

static void __exit isku_exit(void)
{
	hid_unregister_driver(&isku_driver);
	class_destroy(isku_class);
}

module_init(isku_init);
module_exit(isku_exit);

MODULE_AUTHOR("Stefan Achatz");
MODULE_DESCRIPTION("USB Roccat Isku/FX driver");
MODULE_LICENSE("GPL v2");
