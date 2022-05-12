// SPDX-License-Identifier: GPL-2.0-only
/*
 * BGRT boot graphic support
 * Authors: Matthew Garrett, Josh Triplett <josh@joshtriplett.org>
 * Copyright 2012 Red Hat, Inc <mjg@redhat.com>
 * Copyright 2012 Intel Corporation
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/efi-bgrt.h>

static void *bgrt_image;
static struct kobject *bgrt_kobj;

static ssize_t version_show(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", bgrt_tab.version);
}
static DEVICE_ATTR_RO(version);

static ssize_t status_show(struct device *dev,
			   struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", bgrt_tab.status);
}
static DEVICE_ATTR_RO(status);

static ssize_t type_show(struct device *dev,
			 struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", bgrt_tab.image_type);
}
static DEVICE_ATTR_RO(type);

static ssize_t xoffset_show(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", bgrt_tab.image_offset_x);
}
static DEVICE_ATTR_RO(xoffset);

static ssize_t yoffset_show(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", bgrt_tab.image_offset_y);
}
static DEVICE_ATTR_RO(yoffset);

static ssize_t image_read(struct file *file, struct kobject *kobj,
	       struct bin_attribute *attr, char *buf, loff_t off, size_t count)
{
	memcpy(buf, attr->private + off, count);
	return count;
}

static BIN_ATTR_RO(image, 0);	/* size gets filled in later */

static struct attribute *bgrt_attributes[] = {
	&dev_attr_version.attr,
	&dev_attr_status.attr,
	&dev_attr_type.attr,
	&dev_attr_xoffset.attr,
	&dev_attr_yoffset.attr,
	NULL,
};

static struct bin_attribute *bgrt_bin_attributes[] = {
	&bin_attr_image,
	NULL,
};

static const struct attribute_group bgrt_attribute_group = {
	.attrs = bgrt_attributes,
	.bin_attrs = bgrt_bin_attributes,
};

int __init acpi_parse_bgrt(struct acpi_table_header *table)
{
	efi_bgrt_init(table);
	return 0;
}

static int __init bgrt_init(void)
{
	int ret;

	if (!bgrt_tab.image_address)
		return -ENODEV;

	bgrt_image = memremap(bgrt_tab.image_address, bgrt_image_size,
			      MEMREMAP_WB);
	if (!bgrt_image) {
		pr_notice("Ignoring BGRT: failed to map image memory\n");
		return -ENOMEM;
	}

	bin_attr_image.private = bgrt_image;
	bin_attr_image.size = bgrt_image_size;

	bgrt_kobj = kobject_create_and_add("bgrt", acpi_kobj);
	if (!bgrt_kobj) {
		ret = -EINVAL;
		goto out_memmap;
	}

	ret = sysfs_create_group(bgrt_kobj, &bgrt_attribute_group);
	if (ret)
		goto out_kobject;

	return 0;

out_kobject:
	kobject_put(bgrt_kobj);
out_memmap:
	memunmap(bgrt_image);
	return ret;
}
device_initcall(bgrt_init);
