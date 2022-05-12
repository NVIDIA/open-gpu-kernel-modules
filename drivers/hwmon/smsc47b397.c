// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * smsc47b397.c - Part of lm_sensors, Linux kernel modules
 * for hardware monitoring
 *
 * Supports the SMSC LPC47B397-NC Super-I/O chip.
 *
 * Author/Maintainer: Mark M. Hoffman <mhoffman@lightlink.com>
 * Copyright (C) 2004 Utilitek Systems, Inc.
 *
 * derived in part from smsc47m1.c:
 * Copyright (C) 2002 Mark D. Studebaker <mdsxyz123@yahoo.com>
 * Copyright (C) 2004 Jean Delvare <jdelvare@suse.de>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/jiffies.h>
#include <linux/platform_device.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/acpi.h>
#include <linux/io.h>

static unsigned short force_id;
module_param(force_id, ushort, 0);
MODULE_PARM_DESC(force_id, "Override the detected device ID");

static struct platform_device *pdev;

#define DRVNAME "smsc47b397"

/* Super-I/0 registers and commands */

#define	REG	0x2e	/* The register to read/write */
#define	VAL	0x2f	/* The value to read/write */

static inline void superio_outb(int reg, int val)
{
	outb(reg, REG);
	outb(val, VAL);
}

static inline int superio_inb(int reg)
{
	outb(reg, REG);
	return inb(VAL);
}

/* select superio logical device */
static inline void superio_select(int ld)
{
	superio_outb(0x07, ld);
}

static inline int superio_enter(void)
{
	if (!request_muxed_region(REG, 2, DRVNAME))
		return -EBUSY;

	outb(0x55, REG);
	return 0;
}

static inline void superio_exit(void)
{
	outb(0xAA, REG);
	release_region(REG, 2);
}

#define SUPERIO_REG_DEVID	0x20
#define SUPERIO_REG_DEVREV	0x21
#define SUPERIO_REG_BASE_MSB	0x60
#define SUPERIO_REG_BASE_LSB	0x61
#define SUPERIO_REG_LD8		0x08

#define SMSC_EXTENT		0x02

/* 0 <= nr <= 3 */
static u8 smsc47b397_reg_temp[] = {0x25, 0x26, 0x27, 0x80};
#define SMSC47B397_REG_TEMP(nr)	(smsc47b397_reg_temp[(nr)])

/* 0 <= nr <= 3 */
#define SMSC47B397_REG_FAN_LSB(nr) (0x28 + 2 * (nr))
#define SMSC47B397_REG_FAN_MSB(nr) (0x29 + 2 * (nr))

struct smsc47b397_data {
	unsigned short addr;
	struct mutex lock;

	struct mutex update_lock;
	unsigned long last_updated; /* in jiffies */
	int valid;

	/* register values */
	u16 fan[4];
	u8 temp[4];
};

static int smsc47b397_read_value(struct smsc47b397_data *data, u8 reg)
{
	int res;

	mutex_lock(&data->lock);
	outb(reg, data->addr);
	res = inb_p(data->addr + 1);
	mutex_unlock(&data->lock);
	return res;
}

static struct smsc47b397_data *smsc47b397_update_device(struct device *dev)
{
	struct smsc47b397_data *data = dev_get_drvdata(dev);
	int i;

	mutex_lock(&data->update_lock);

	if (time_after(jiffies, data->last_updated + HZ) || !data->valid) {
		dev_dbg(dev, "starting device update...\n");

		/* 4 temperature inputs, 4 fan inputs */
		for (i = 0; i < 4; i++) {
			data->temp[i] = smsc47b397_read_value(data,
					SMSC47B397_REG_TEMP(i));

			/* must read LSB first */
			data->fan[i]  = smsc47b397_read_value(data,
					SMSC47B397_REG_FAN_LSB(i));
			data->fan[i] |= smsc47b397_read_value(data,
					SMSC47B397_REG_FAN_MSB(i)) << 8;
		}

		data->last_updated = jiffies;
		data->valid = 1;

		dev_dbg(dev, "... device update complete\n");
	}

	mutex_unlock(&data->update_lock);

	return data;
}

/*
 * TEMP: 0.001C/bit (-128C to +127C)
 * REG: 1C/bit, two's complement
 */
static int temp_from_reg(u8 reg)
{
	return (s8)reg * 1000;
}

static ssize_t temp_show(struct device *dev, struct device_attribute *devattr,
			 char *buf)
{
	struct sensor_device_attribute *attr = to_sensor_dev_attr(devattr);
	struct smsc47b397_data *data = smsc47b397_update_device(dev);
	return sprintf(buf, "%d\n", temp_from_reg(data->temp[attr->index]));
}

static SENSOR_DEVICE_ATTR_RO(temp1_input, temp, 0);
static SENSOR_DEVICE_ATTR_RO(temp2_input, temp, 1);
static SENSOR_DEVICE_ATTR_RO(temp3_input, temp, 2);
static SENSOR_DEVICE_ATTR_RO(temp4_input, temp, 3);

/*
 * FAN: 1 RPM/bit
 * REG: count of 90kHz pulses / revolution
 */
static int fan_from_reg(u16 reg)
{
	if (reg == 0 || reg == 0xffff)
		return 0;
	return 90000 * 60 / reg;
}

static ssize_t fan_show(struct device *dev, struct device_attribute *devattr,
			char *buf)
{
	struct sensor_device_attribute *attr = to_sensor_dev_attr(devattr);
	struct smsc47b397_data *data = smsc47b397_update_device(dev);
	return sprintf(buf, "%d\n", fan_from_reg(data->fan[attr->index]));
}
static SENSOR_DEVICE_ATTR_RO(fan1_input, fan, 0);
static SENSOR_DEVICE_ATTR_RO(fan2_input, fan, 1);
static SENSOR_DEVICE_ATTR_RO(fan3_input, fan, 2);
static SENSOR_DEVICE_ATTR_RO(fan4_input, fan, 3);

static struct attribute *smsc47b397_attrs[] = {
	&sensor_dev_attr_temp1_input.dev_attr.attr,
	&sensor_dev_attr_temp2_input.dev_attr.attr,
	&sensor_dev_attr_temp3_input.dev_attr.attr,
	&sensor_dev_attr_temp4_input.dev_attr.attr,
	&sensor_dev_attr_fan1_input.dev_attr.attr,
	&sensor_dev_attr_fan2_input.dev_attr.attr,
	&sensor_dev_attr_fan3_input.dev_attr.attr,
	&sensor_dev_attr_fan4_input.dev_attr.attr,

	NULL
};

ATTRIBUTE_GROUPS(smsc47b397);

static int smsc47b397_probe(struct platform_device *pdev);

static struct platform_driver smsc47b397_driver = {
	.driver = {
		.name	= DRVNAME,
	},
	.probe		= smsc47b397_probe,
};

static int smsc47b397_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct smsc47b397_data *data;
	struct device *hwmon_dev;
	struct resource *res;

	res = platform_get_resource(pdev, IORESOURCE_IO, 0);
	if (!devm_request_region(dev, res->start, SMSC_EXTENT,
				 smsc47b397_driver.driver.name)) {
		dev_err(dev, "Region 0x%lx-0x%lx already in use!\n",
			(unsigned long)res->start,
			(unsigned long)res->start + SMSC_EXTENT - 1);
		return -EBUSY;
	}

	data = devm_kzalloc(dev, sizeof(struct smsc47b397_data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->addr = res->start;
	mutex_init(&data->lock);
	mutex_init(&data->update_lock);

	hwmon_dev = devm_hwmon_device_register_with_groups(dev, "smsc47b397",
							   data,
							   smsc47b397_groups);
	return PTR_ERR_OR_ZERO(hwmon_dev);
}

static int __init smsc47b397_device_add(unsigned short address)
{
	struct resource res = {
		.start	= address,
		.end	= address + SMSC_EXTENT - 1,
		.name	= DRVNAME,
		.flags	= IORESOURCE_IO,
	};
	int err;

	err = acpi_check_resource_conflict(&res);
	if (err)
		goto exit;

	pdev = platform_device_alloc(DRVNAME, address);
	if (!pdev) {
		err = -ENOMEM;
		pr_err("Device allocation failed\n");
		goto exit;
	}

	err = platform_device_add_resources(pdev, &res, 1);
	if (err) {
		pr_err("Device resource addition failed (%d)\n", err);
		goto exit_device_put;
	}

	err = platform_device_add(pdev);
	if (err) {
		pr_err("Device addition failed (%d)\n", err);
		goto exit_device_put;
	}

	return 0;

exit_device_put:
	platform_device_put(pdev);
exit:
	return err;
}

static int __init smsc47b397_find(void)
{
	u8 id, rev;
	char *name;
	unsigned short addr;
	int err;

	err = superio_enter();
	if (err)
		return err;

	id = force_id ? force_id : superio_inb(SUPERIO_REG_DEVID);

	switch (id) {
	case 0x81:
		name = "SCH5307-NS";
		break;
	case 0x6f:
		name = "LPC47B397-NC";
		break;
	case 0x85:
	case 0x8c:
		name = "SCH5317";
		break;
	default:
		superio_exit();
		return -ENODEV;
	}

	rev = superio_inb(SUPERIO_REG_DEVREV);

	superio_select(SUPERIO_REG_LD8);
	addr = (superio_inb(SUPERIO_REG_BASE_MSB) << 8)
		 |  superio_inb(SUPERIO_REG_BASE_LSB);

	pr_info("found SMSC %s (base address 0x%04x, revision %u)\n",
		name, addr, rev);

	superio_exit();
	return addr;
}

static int __init smsc47b397_init(void)
{
	unsigned short address;
	int ret;

	ret = smsc47b397_find();
	if (ret < 0)
		return ret;
	address = ret;

	ret = platform_driver_register(&smsc47b397_driver);
	if (ret)
		goto exit;

	/* Sets global pdev as a side effect */
	ret = smsc47b397_device_add(address);
	if (ret)
		goto exit_driver;

	return 0;

exit_driver:
	platform_driver_unregister(&smsc47b397_driver);
exit:
	return ret;
}

static void __exit smsc47b397_exit(void)
{
	platform_device_unregister(pdev);
	platform_driver_unregister(&smsc47b397_driver);
}

MODULE_AUTHOR("Mark M. Hoffman <mhoffman@lightlink.com>");
MODULE_DESCRIPTION("SMSC LPC47B397 driver");
MODULE_LICENSE("GPL");

module_init(smsc47b397_init);
module_exit(smsc47b397_exit);
