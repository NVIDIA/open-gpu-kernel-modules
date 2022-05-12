// SPDX-License-Identifier: GPL-2.0
#include <linux/kernel.h>
#include <linux/stat.h>
#include <asm/macio.h>

static ssize_t
compatible_show (struct device *dev, struct device_attribute *attr, char *buf)
{
	struct platform_device *of;
	const char *compat;
	int cplen;
	int length = 0;

	of = &to_macio_device (dev)->ofdev;
	compat = of_get_property(of->dev.of_node, "compatible", &cplen);
	if (!compat) {
		*buf = '\0';
		return 0;
	}
	while (cplen > 0) {
		int l;
		length += sprintf (buf, "%s\n", compat);
		buf += length;
		l = strlen (compat) + 1;
		compat += l;
		cplen -= l;
	}

	return length;
}
static DEVICE_ATTR_RO(compatible);

static ssize_t modalias_show (struct device *dev, struct device_attribute *attr,
			      char *buf)
{
	return of_device_modalias(dev, buf, PAGE_SIZE);
}

static ssize_t devspec_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct platform_device *ofdev;

	ofdev = to_platform_device(dev);
	return sprintf(buf, "%pOF\n", ofdev->dev.of_node);
}
static DEVICE_ATTR_RO(modalias);
static DEVICE_ATTR_RO(devspec);

static ssize_t name_show(struct device *dev,
			 struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%pOFn\n", dev->of_node);
}
static DEVICE_ATTR_RO(name);

static ssize_t type_show(struct device *dev,
			 struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", of_node_get_device_type(dev->of_node));
}
static DEVICE_ATTR_RO(type);

static struct attribute *macio_dev_attrs[] = {
	&dev_attr_name.attr,
	&dev_attr_type.attr,
	&dev_attr_compatible.attr,
	&dev_attr_modalias.attr,
	&dev_attr_devspec.attr,
	NULL,
};

static const struct attribute_group macio_dev_group = {
	.attrs = macio_dev_attrs,
};

const struct attribute_group *macio_dev_groups[] = {
	&macio_dev_group,
	NULL,
};
