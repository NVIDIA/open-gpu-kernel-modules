// SPDX-License-Identifier: GPL-2.0-only
/*
 * Export SMBIOS/DMI info via sysfs to userspace
 *
 * Copyright 2007, Lennart Poettering
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/dmi.h>
#include <linux/device.h>
#include <linux/slab.h>

struct dmi_device_attribute{
	struct device_attribute dev_attr;
	int field;
};
#define to_dmi_dev_attr(_dev_attr) \
	container_of(_dev_attr, struct dmi_device_attribute, dev_attr)

static ssize_t sys_dmi_field_show(struct device *dev,
				  struct device_attribute *attr,
				  char *page)
{
	int field = to_dmi_dev_attr(attr)->field;
	ssize_t len;
	len = scnprintf(page, PAGE_SIZE, "%s\n", dmi_get_system_info(field));
	page[len-1] = '\n';
	return len;
}

#define DMI_ATTR(_name, _mode, _show, _field)			\
	{ .dev_attr = __ATTR(_name, _mode, _show, NULL),	\
	  .field = _field }

#define DEFINE_DMI_ATTR_WITH_SHOW(_name, _mode, _field)		\
static struct dmi_device_attribute sys_dmi_##_name##_attr =	\
	DMI_ATTR(_name, _mode, sys_dmi_field_show, _field);

DEFINE_DMI_ATTR_WITH_SHOW(bios_vendor,		0444, DMI_BIOS_VENDOR);
DEFINE_DMI_ATTR_WITH_SHOW(bios_version,		0444, DMI_BIOS_VERSION);
DEFINE_DMI_ATTR_WITH_SHOW(bios_date,		0444, DMI_BIOS_DATE);
DEFINE_DMI_ATTR_WITH_SHOW(sys_vendor,		0444, DMI_SYS_VENDOR);
DEFINE_DMI_ATTR_WITH_SHOW(bios_release,		0444, DMI_BIOS_RELEASE);
DEFINE_DMI_ATTR_WITH_SHOW(ec_firmware_release,	0444, DMI_EC_FIRMWARE_RELEASE);
DEFINE_DMI_ATTR_WITH_SHOW(product_name,		0444, DMI_PRODUCT_NAME);
DEFINE_DMI_ATTR_WITH_SHOW(product_version,	0444, DMI_PRODUCT_VERSION);
DEFINE_DMI_ATTR_WITH_SHOW(product_serial,	0400, DMI_PRODUCT_SERIAL);
DEFINE_DMI_ATTR_WITH_SHOW(product_uuid,		0400, DMI_PRODUCT_UUID);
DEFINE_DMI_ATTR_WITH_SHOW(product_sku,		0444, DMI_PRODUCT_SKU);
DEFINE_DMI_ATTR_WITH_SHOW(product_family,	0444, DMI_PRODUCT_FAMILY);
DEFINE_DMI_ATTR_WITH_SHOW(board_vendor,		0444, DMI_BOARD_VENDOR);
DEFINE_DMI_ATTR_WITH_SHOW(board_name,		0444, DMI_BOARD_NAME);
DEFINE_DMI_ATTR_WITH_SHOW(board_version,	0444, DMI_BOARD_VERSION);
DEFINE_DMI_ATTR_WITH_SHOW(board_serial,		0400, DMI_BOARD_SERIAL);
DEFINE_DMI_ATTR_WITH_SHOW(board_asset_tag,	0444, DMI_BOARD_ASSET_TAG);
DEFINE_DMI_ATTR_WITH_SHOW(chassis_vendor,	0444, DMI_CHASSIS_VENDOR);
DEFINE_DMI_ATTR_WITH_SHOW(chassis_type,		0444, DMI_CHASSIS_TYPE);
DEFINE_DMI_ATTR_WITH_SHOW(chassis_version,	0444, DMI_CHASSIS_VERSION);
DEFINE_DMI_ATTR_WITH_SHOW(chassis_serial,	0400, DMI_CHASSIS_SERIAL);
DEFINE_DMI_ATTR_WITH_SHOW(chassis_asset_tag,	0444, DMI_CHASSIS_ASSET_TAG);

static void ascii_filter(char *d, const char *s)
{
	/* Filter out characters we don't want to see in the modalias string */
	for (; *s; s++)
		if (*s > ' ' && *s < 127 && *s != ':')
			*(d++) = *s;

	*d = 0;
}

static ssize_t get_modalias(char *buffer, size_t buffer_size)
{
	static const struct mafield {
		const char *prefix;
		int field;
	} fields[] = {
		{ "bvn", DMI_BIOS_VENDOR },
		{ "bvr", DMI_BIOS_VERSION },
		{ "bd",  DMI_BIOS_DATE },
		{ "br",  DMI_BIOS_RELEASE },
		{ "efr", DMI_EC_FIRMWARE_RELEASE },
		{ "svn", DMI_SYS_VENDOR },
		{ "pn",  DMI_PRODUCT_NAME },
		{ "pvr", DMI_PRODUCT_VERSION },
		{ "sku", DMI_PRODUCT_SKU },
		{ "rvn", DMI_BOARD_VENDOR },
		{ "rn",  DMI_BOARD_NAME },
		{ "rvr", DMI_BOARD_VERSION },
		{ "cvn", DMI_CHASSIS_VENDOR },
		{ "ct",  DMI_CHASSIS_TYPE },
		{ "cvr", DMI_CHASSIS_VERSION },
		{ NULL,  DMI_NONE }
	};

	ssize_t l, left;
	char *p;
	const struct mafield *f;

	strcpy(buffer, "dmi");
	p = buffer + 3; left = buffer_size - 4;

	for (f = fields; f->prefix && left > 0; f++) {
		const char *c;
		char *t;

		c = dmi_get_system_info(f->field);
		if (!c)
			continue;

		t = kmalloc(strlen(c) + 1, GFP_KERNEL);
		if (!t)
			break;
		ascii_filter(t, c);
		l = scnprintf(p, left, ":%s%s", f->prefix, t);
		kfree(t);

		p += l;
		left -= l;
	}

	p[0] = ':';
	p[1] = 0;

	return p - buffer + 1;
}

static ssize_t sys_dmi_modalias_show(struct device *dev,
				     struct device_attribute *attr, char *page)
{
	ssize_t r;
	r = get_modalias(page, PAGE_SIZE-1);
	page[r] = '\n';
	page[r+1] = 0;
	return r+1;
}

static struct device_attribute sys_dmi_modalias_attr =
	__ATTR(modalias, 0444, sys_dmi_modalias_show, NULL);

static struct attribute *sys_dmi_attributes[DMI_STRING_MAX+2];

static struct attribute_group sys_dmi_attribute_group = {
	.attrs = sys_dmi_attributes,
};

static const struct attribute_group* sys_dmi_attribute_groups[] = {
	&sys_dmi_attribute_group,
	NULL
};

static int dmi_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	ssize_t len;

	if (add_uevent_var(env, "MODALIAS="))
		return -ENOMEM;
	len = get_modalias(&env->buf[env->buflen - 1],
			   sizeof(env->buf) - env->buflen);
	if (len >= (sizeof(env->buf) - env->buflen))
		return -ENOMEM;
	env->buflen += len;
	return 0;
}

static struct class dmi_class = {
	.name = "dmi",
	.dev_release = (void(*)(struct device *)) kfree,
	.dev_uevent = dmi_dev_uevent,
};

static struct device *dmi_dev;

/* Initialization */

#define ADD_DMI_ATTR(_name, _field) \
	if (dmi_get_system_info(_field)) \
		sys_dmi_attributes[i++] = &sys_dmi_##_name##_attr.dev_attr.attr;

/* In a separate function to keep gcc 3.2 happy - do NOT merge this in
   dmi_id_init! */
static void __init dmi_id_init_attr_table(void)
{
	int i;

	/* Not necessarily all DMI fields are available on all
	 * systems, hence let's built an attribute table of just
	 * what's available */
	i = 0;
	ADD_DMI_ATTR(bios_vendor,       DMI_BIOS_VENDOR);
	ADD_DMI_ATTR(bios_version,      DMI_BIOS_VERSION);
	ADD_DMI_ATTR(bios_date,         DMI_BIOS_DATE);
	ADD_DMI_ATTR(bios_release,      DMI_BIOS_RELEASE);
	ADD_DMI_ATTR(ec_firmware_release, DMI_EC_FIRMWARE_RELEASE);
	ADD_DMI_ATTR(sys_vendor,        DMI_SYS_VENDOR);
	ADD_DMI_ATTR(product_name,      DMI_PRODUCT_NAME);
	ADD_DMI_ATTR(product_version,   DMI_PRODUCT_VERSION);
	ADD_DMI_ATTR(product_serial,    DMI_PRODUCT_SERIAL);
	ADD_DMI_ATTR(product_uuid,      DMI_PRODUCT_UUID);
	ADD_DMI_ATTR(product_family,    DMI_PRODUCT_FAMILY);
	ADD_DMI_ATTR(product_sku,       DMI_PRODUCT_SKU);
	ADD_DMI_ATTR(board_vendor,      DMI_BOARD_VENDOR);
	ADD_DMI_ATTR(board_name,        DMI_BOARD_NAME);
	ADD_DMI_ATTR(board_version,     DMI_BOARD_VERSION);
	ADD_DMI_ATTR(board_serial,      DMI_BOARD_SERIAL);
	ADD_DMI_ATTR(board_asset_tag,   DMI_BOARD_ASSET_TAG);
	ADD_DMI_ATTR(chassis_vendor,    DMI_CHASSIS_VENDOR);
	ADD_DMI_ATTR(chassis_type,      DMI_CHASSIS_TYPE);
	ADD_DMI_ATTR(chassis_version,   DMI_CHASSIS_VERSION);
	ADD_DMI_ATTR(chassis_serial,    DMI_CHASSIS_SERIAL);
	ADD_DMI_ATTR(chassis_asset_tag, DMI_CHASSIS_ASSET_TAG);
	sys_dmi_attributes[i++] = &sys_dmi_modalias_attr.attr;
}

static int __init dmi_id_init(void)
{
	int ret;

	if (!dmi_available)
		return -ENODEV;

	dmi_id_init_attr_table();

	ret = class_register(&dmi_class);
	if (ret)
		return ret;

	dmi_dev = kzalloc(sizeof(*dmi_dev), GFP_KERNEL);
	if (!dmi_dev) {
		ret = -ENOMEM;
		goto fail_class_unregister;
	}

	dmi_dev->class = &dmi_class;
	dev_set_name(dmi_dev, "id");
	dmi_dev->groups = sys_dmi_attribute_groups;

	ret = device_register(dmi_dev);
	if (ret)
		goto fail_put_dmi_dev;

	return 0;

fail_put_dmi_dev:
	put_device(dmi_dev);

fail_class_unregister:
	class_unregister(&dmi_class);

	return ret;
}

arch_initcall(dmi_id_init);
