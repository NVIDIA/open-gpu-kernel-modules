// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * emc2103.c - Support for SMSC EMC2103
 * Copyright (c) 2010 SMSC
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/mutex.h>

/* Addresses scanned */
static const unsigned short normal_i2c[] = { 0x2E, I2C_CLIENT_END };

static const u8 REG_TEMP[4] = { 0x00, 0x02, 0x04, 0x06 };
static const u8 REG_TEMP_MIN[4] = { 0x3c, 0x38, 0x39, 0x3a };
static const u8 REG_TEMP_MAX[4] = { 0x34, 0x30, 0x31, 0x32 };

#define REG_CONF1		0x20
#define REG_TEMP_MAX_ALARM	0x24
#define REG_TEMP_MIN_ALARM	0x25
#define REG_FAN_CONF1		0x42
#define REG_FAN_TARGET_LO	0x4c
#define REG_FAN_TARGET_HI	0x4d
#define REG_FAN_TACH_HI		0x4e
#define REG_FAN_TACH_LO		0x4f
#define REG_PRODUCT_ID		0xfd
#define REG_MFG_ID		0xfe

/* equation 4 from datasheet: rpm = (3932160 * multipler) / count */
#define FAN_RPM_FACTOR		3932160

/*
 * 2103-2 and 2103-4's 3rd temperature sensor can be connected to two diodes
 * in anti-parallel mode, and in this configuration both can be read
 * independently (so we have 4 temperature inputs).  The device can't
 * detect if it's connected in this mode, so we have to manually enable
 * it.  Default is to leave the device in the state it's already in (-1).
 * This parameter allows APD mode to be optionally forced on or off
 */
static int apd = -1;
module_param(apd, bint, 0);
MODULE_PARM_DESC(apd, "Set to zero to disable anti-parallel diode mode");

struct temperature {
	s8	degrees;
	u8	fraction;	/* 0-7 multiples of 0.125 */
};

struct emc2103_data {
	struct i2c_client	*client;
	const struct		attribute_group *groups[4];
	struct mutex		update_lock;
	bool			valid;		/* registers are valid */
	bool			fan_rpm_control;
	int			temp_count;	/* num of temp sensors */
	unsigned long		last_updated;	/* in jiffies */
	struct temperature	temp[4];	/* internal + 3 external */
	s8			temp_min[4];	/* no fractional part */
	s8			temp_max[4];    /* no fractional part */
	u8			temp_min_alarm;
	u8			temp_max_alarm;
	u8			fan_multiplier;
	u16			fan_tach;
	u16			fan_target;
};

static int read_u8_from_i2c(struct i2c_client *client, u8 i2c_reg, u8 *output)
{
	int status = i2c_smbus_read_byte_data(client, i2c_reg);
	if (status < 0) {
		dev_warn(&client->dev, "reg 0x%02x, err %d\n",
			i2c_reg, status);
	} else {
		*output = status;
	}
	return status;
}

static void read_temp_from_i2c(struct i2c_client *client, u8 i2c_reg,
			       struct temperature *temp)
{
	u8 degrees, fractional;

	if (read_u8_from_i2c(client, i2c_reg, &degrees) < 0)
		return;

	if (read_u8_from_i2c(client, i2c_reg + 1, &fractional) < 0)
		return;

	temp->degrees = degrees;
	temp->fraction = (fractional & 0xe0) >> 5;
}

static void read_fan_from_i2c(struct i2c_client *client, u16 *output,
			      u8 hi_addr, u8 lo_addr)
{
	u8 high_byte, lo_byte;

	if (read_u8_from_i2c(client, hi_addr, &high_byte) < 0)
		return;

	if (read_u8_from_i2c(client, lo_addr, &lo_byte) < 0)
		return;

	*output = ((u16)high_byte << 5) | (lo_byte >> 3);
}

static void write_fan_target_to_i2c(struct i2c_client *client, u16 new_target)
{
	u8 high_byte = (new_target & 0x1fe0) >> 5;
	u8 low_byte = (new_target & 0x001f) << 3;
	i2c_smbus_write_byte_data(client, REG_FAN_TARGET_LO, low_byte);
	i2c_smbus_write_byte_data(client, REG_FAN_TARGET_HI, high_byte);
}

static void read_fan_config_from_i2c(struct i2c_client *client)

{
	struct emc2103_data *data = i2c_get_clientdata(client);
	u8 conf1;

	if (read_u8_from_i2c(client, REG_FAN_CONF1, &conf1) < 0)
		return;

	data->fan_multiplier = 1 << ((conf1 & 0x60) >> 5);
	data->fan_rpm_control = (conf1 & 0x80) != 0;
}

static struct emc2103_data *emc2103_update_device(struct device *dev)
{
	struct emc2103_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;

	mutex_lock(&data->update_lock);

	if (time_after(jiffies, data->last_updated + HZ + HZ / 2)
	    || !data->valid) {
		int i;

		for (i = 0; i < data->temp_count; i++) {
			read_temp_from_i2c(client, REG_TEMP[i], &data->temp[i]);
			read_u8_from_i2c(client, REG_TEMP_MIN[i],
				&data->temp_min[i]);
			read_u8_from_i2c(client, REG_TEMP_MAX[i],
				&data->temp_max[i]);
		}

		read_u8_from_i2c(client, REG_TEMP_MIN_ALARM,
			&data->temp_min_alarm);
		read_u8_from_i2c(client, REG_TEMP_MAX_ALARM,
			&data->temp_max_alarm);

		read_fan_from_i2c(client, &data->fan_tach,
			REG_FAN_TACH_HI, REG_FAN_TACH_LO);
		read_fan_from_i2c(client, &data->fan_target,
			REG_FAN_TARGET_HI, REG_FAN_TARGET_LO);
		read_fan_config_from_i2c(client);

		data->last_updated = jiffies;
		data->valid = true;
	}

	mutex_unlock(&data->update_lock);

	return data;
}

static ssize_t
temp_show(struct device *dev, struct device_attribute *da, char *buf)
{
	int nr = to_sensor_dev_attr(da)->index;
	struct emc2103_data *data = emc2103_update_device(dev);
	int millidegrees = data->temp[nr].degrees * 1000
		+ data->temp[nr].fraction * 125;
	return sprintf(buf, "%d\n", millidegrees);
}

static ssize_t
temp_min_show(struct device *dev, struct device_attribute *da, char *buf)
{
	int nr = to_sensor_dev_attr(da)->index;
	struct emc2103_data *data = emc2103_update_device(dev);
	int millidegrees = data->temp_min[nr] * 1000;
	return sprintf(buf, "%d\n", millidegrees);
}

static ssize_t
temp_max_show(struct device *dev, struct device_attribute *da, char *buf)
{
	int nr = to_sensor_dev_attr(da)->index;
	struct emc2103_data *data = emc2103_update_device(dev);
	int millidegrees = data->temp_max[nr] * 1000;
	return sprintf(buf, "%d\n", millidegrees);
}

static ssize_t
temp_fault_show(struct device *dev, struct device_attribute *da, char *buf)
{
	int nr = to_sensor_dev_attr(da)->index;
	struct emc2103_data *data = emc2103_update_device(dev);
	bool fault = (data->temp[nr].degrees == -128);
	return sprintf(buf, "%d\n", fault ? 1 : 0);
}

static ssize_t
temp_min_alarm_show(struct device *dev, struct device_attribute *da,
		    char *buf)
{
	int nr = to_sensor_dev_attr(da)->index;
	struct emc2103_data *data = emc2103_update_device(dev);
	bool alarm = data->temp_min_alarm & (1 << nr);
	return sprintf(buf, "%d\n", alarm ? 1 : 0);
}

static ssize_t
temp_max_alarm_show(struct device *dev, struct device_attribute *da,
		    char *buf)
{
	int nr = to_sensor_dev_attr(da)->index;
	struct emc2103_data *data = emc2103_update_device(dev);
	bool alarm = data->temp_max_alarm & (1 << nr);
	return sprintf(buf, "%d\n", alarm ? 1 : 0);
}

static ssize_t temp_min_store(struct device *dev, struct device_attribute *da,
			      const char *buf, size_t count)
{
	int nr = to_sensor_dev_attr(da)->index;
	struct emc2103_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	long val;

	int result = kstrtol(buf, 10, &val);
	if (result < 0)
		return result;

	val = DIV_ROUND_CLOSEST(clamp_val(val, -63000, 127000), 1000);

	mutex_lock(&data->update_lock);
	data->temp_min[nr] = val;
	i2c_smbus_write_byte_data(client, REG_TEMP_MIN[nr], val);
	mutex_unlock(&data->update_lock);

	return count;
}

static ssize_t temp_max_store(struct device *dev, struct device_attribute *da,
			      const char *buf, size_t count)
{
	int nr = to_sensor_dev_attr(da)->index;
	struct emc2103_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	long val;

	int result = kstrtol(buf, 10, &val);
	if (result < 0)
		return result;

	val = DIV_ROUND_CLOSEST(clamp_val(val, -63000, 127000), 1000);

	mutex_lock(&data->update_lock);
	data->temp_max[nr] = val;
	i2c_smbus_write_byte_data(client, REG_TEMP_MAX[nr], val);
	mutex_unlock(&data->update_lock);

	return count;
}

static ssize_t
fan1_input_show(struct device *dev, struct device_attribute *da, char *buf)
{
	struct emc2103_data *data = emc2103_update_device(dev);
	int rpm = 0;
	if (data->fan_tach != 0)
		rpm = (FAN_RPM_FACTOR * data->fan_multiplier) / data->fan_tach;
	return sprintf(buf, "%d\n", rpm);
}

static ssize_t
fan1_div_show(struct device *dev, struct device_attribute *da, char *buf)
{
	struct emc2103_data *data = emc2103_update_device(dev);
	int fan_div = 8 / data->fan_multiplier;
	return sprintf(buf, "%d\n", fan_div);
}

/*
 * Note: we also update the fan target here, because its value is
 * determined in part by the fan clock divider.  This follows the principle
 * of least surprise; the user doesn't expect the fan target to change just
 * because the divider changed.
 */
static ssize_t fan1_div_store(struct device *dev, struct device_attribute *da,
			      const char *buf, size_t count)
{
	struct emc2103_data *data = emc2103_update_device(dev);
	struct i2c_client *client = data->client;
	int new_range_bits, old_div = 8 / data->fan_multiplier;
	long new_div;

	int status = kstrtol(buf, 10, &new_div);
	if (status < 0)
		return status;

	if (new_div == old_div) /* No change */
		return count;

	switch (new_div) {
	case 1:
		new_range_bits = 3;
		break;
	case 2:
		new_range_bits = 2;
		break;
	case 4:
		new_range_bits = 1;
		break;
	case 8:
		new_range_bits = 0;
		break;
	default:
		return -EINVAL;
	}

	mutex_lock(&data->update_lock);

	status = i2c_smbus_read_byte_data(client, REG_FAN_CONF1);
	if (status < 0) {
		dev_dbg(&client->dev, "reg 0x%02x, err %d\n",
			REG_FAN_CONF1, status);
		mutex_unlock(&data->update_lock);
		return status;
	}
	status &= 0x9F;
	status |= (new_range_bits << 5);
	i2c_smbus_write_byte_data(client, REG_FAN_CONF1, status);

	data->fan_multiplier = 8 / new_div;

	/* update fan target if high byte is not disabled */
	if ((data->fan_target & 0x1fe0) != 0x1fe0) {
		u16 new_target = (data->fan_target * old_div) / new_div;
		data->fan_target = min(new_target, (u16)0x1fff);
		write_fan_target_to_i2c(client, data->fan_target);
	}

	/* invalidate data to force re-read from hardware */
	data->valid = false;

	mutex_unlock(&data->update_lock);
	return count;
}

static ssize_t
fan1_target_show(struct device *dev, struct device_attribute *da, char *buf)
{
	struct emc2103_data *data = emc2103_update_device(dev);
	int rpm = 0;

	/* high byte of 0xff indicates disabled so return 0 */
	if ((data->fan_target != 0) && ((data->fan_target & 0x1fe0) != 0x1fe0))
		rpm = (FAN_RPM_FACTOR * data->fan_multiplier)
			/ data->fan_target;

	return sprintf(buf, "%d\n", rpm);
}

static ssize_t fan1_target_store(struct device *dev,
				 struct device_attribute *da, const char *buf,
				 size_t count)
{
	struct emc2103_data *data = emc2103_update_device(dev);
	struct i2c_client *client = data->client;
	unsigned long rpm_target;

	int result = kstrtoul(buf, 10, &rpm_target);
	if (result < 0)
		return result;

	/* Datasheet states 16384 as maximum RPM target (table 3.2) */
	rpm_target = clamp_val(rpm_target, 0, 16384);

	mutex_lock(&data->update_lock);

	if (rpm_target == 0)
		data->fan_target = 0x1fff;
	else
		data->fan_target = clamp_val(
			(FAN_RPM_FACTOR * data->fan_multiplier) / rpm_target,
			0, 0x1fff);

	write_fan_target_to_i2c(client, data->fan_target);

	mutex_unlock(&data->update_lock);
	return count;
}

static ssize_t
fan1_fault_show(struct device *dev, struct device_attribute *da, char *buf)
{
	struct emc2103_data *data = emc2103_update_device(dev);
	bool fault = ((data->fan_tach & 0x1fe0) == 0x1fe0);
	return sprintf(buf, "%d\n", fault ? 1 : 0);
}

static ssize_t
pwm1_enable_show(struct device *dev, struct device_attribute *da, char *buf)
{
	struct emc2103_data *data = emc2103_update_device(dev);
	return sprintf(buf, "%d\n", data->fan_rpm_control ? 3 : 0);
}

static ssize_t pwm1_enable_store(struct device *dev,
				 struct device_attribute *da, const char *buf,
				 size_t count)
{
	struct emc2103_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	long new_value;
	u8 conf_reg;

	int result = kstrtol(buf, 10, &new_value);
	if (result < 0)
		return result;

	mutex_lock(&data->update_lock);
	switch (new_value) {
	case 0:
		data->fan_rpm_control = false;
		break;
	case 3:
		data->fan_rpm_control = true;
		break;
	default:
		count = -EINVAL;
		goto err;
	}

	result = read_u8_from_i2c(client, REG_FAN_CONF1, &conf_reg);
	if (result < 0) {
		count = result;
		goto err;
	}

	if (data->fan_rpm_control)
		conf_reg |= 0x80;
	else
		conf_reg &= ~0x80;

	i2c_smbus_write_byte_data(client, REG_FAN_CONF1, conf_reg);
err:
	mutex_unlock(&data->update_lock);
	return count;
}

static SENSOR_DEVICE_ATTR_RO(temp1_input, temp, 0);
static SENSOR_DEVICE_ATTR_RW(temp1_min, temp_min, 0);
static SENSOR_DEVICE_ATTR_RW(temp1_max, temp_max, 0);
static SENSOR_DEVICE_ATTR_RO(temp1_fault, temp_fault, 0);
static SENSOR_DEVICE_ATTR_RO(temp1_min_alarm, temp_min_alarm, 0);
static SENSOR_DEVICE_ATTR_RO(temp1_max_alarm, temp_max_alarm, 0);

static SENSOR_DEVICE_ATTR_RO(temp2_input, temp, 1);
static SENSOR_DEVICE_ATTR_RW(temp2_min, temp_min, 1);
static SENSOR_DEVICE_ATTR_RW(temp2_max, temp_max, 1);
static SENSOR_DEVICE_ATTR_RO(temp2_fault, temp_fault, 1);
static SENSOR_DEVICE_ATTR_RO(temp2_min_alarm, temp_min_alarm, 1);
static SENSOR_DEVICE_ATTR_RO(temp2_max_alarm, temp_max_alarm, 1);

static SENSOR_DEVICE_ATTR_RO(temp3_input, temp, 2);
static SENSOR_DEVICE_ATTR_RW(temp3_min, temp_min, 2);
static SENSOR_DEVICE_ATTR_RW(temp3_max, temp_max, 2);
static SENSOR_DEVICE_ATTR_RO(temp3_fault, temp_fault, 2);
static SENSOR_DEVICE_ATTR_RO(temp3_min_alarm, temp_min_alarm, 2);
static SENSOR_DEVICE_ATTR_RO(temp3_max_alarm, temp_max_alarm, 2);

static SENSOR_DEVICE_ATTR_RO(temp4_input, temp, 3);
static SENSOR_DEVICE_ATTR_RW(temp4_min, temp_min, 3);
static SENSOR_DEVICE_ATTR_RW(temp4_max, temp_max, 3);
static SENSOR_DEVICE_ATTR_RO(temp4_fault, temp_fault, 3);
static SENSOR_DEVICE_ATTR_RO(temp4_min_alarm, temp_min_alarm, 3);
static SENSOR_DEVICE_ATTR_RO(temp4_max_alarm, temp_max_alarm, 3);

static DEVICE_ATTR_RO(fan1_input);
static DEVICE_ATTR_RW(fan1_div);
static DEVICE_ATTR_RW(fan1_target);
static DEVICE_ATTR_RO(fan1_fault);

static DEVICE_ATTR_RW(pwm1_enable);

/* sensors present on all models */
static struct attribute *emc2103_attributes[] = {
	&sensor_dev_attr_temp1_input.dev_attr.attr,
	&sensor_dev_attr_temp1_min.dev_attr.attr,
	&sensor_dev_attr_temp1_max.dev_attr.attr,
	&sensor_dev_attr_temp1_fault.dev_attr.attr,
	&sensor_dev_attr_temp1_min_alarm.dev_attr.attr,
	&sensor_dev_attr_temp1_max_alarm.dev_attr.attr,
	&sensor_dev_attr_temp2_input.dev_attr.attr,
	&sensor_dev_attr_temp2_min.dev_attr.attr,
	&sensor_dev_attr_temp2_max.dev_attr.attr,
	&sensor_dev_attr_temp2_fault.dev_attr.attr,
	&sensor_dev_attr_temp2_min_alarm.dev_attr.attr,
	&sensor_dev_attr_temp2_max_alarm.dev_attr.attr,
	&dev_attr_fan1_input.attr,
	&dev_attr_fan1_div.attr,
	&dev_attr_fan1_target.attr,
	&dev_attr_fan1_fault.attr,
	&dev_attr_pwm1_enable.attr,
	NULL
};

/* extra temperature sensors only present on 2103-2 and 2103-4 */
static struct attribute *emc2103_attributes_temp3[] = {
	&sensor_dev_attr_temp3_input.dev_attr.attr,
	&sensor_dev_attr_temp3_min.dev_attr.attr,
	&sensor_dev_attr_temp3_max.dev_attr.attr,
	&sensor_dev_attr_temp3_fault.dev_attr.attr,
	&sensor_dev_attr_temp3_min_alarm.dev_attr.attr,
	&sensor_dev_attr_temp3_max_alarm.dev_attr.attr,
	NULL
};

/* extra temperature sensors only present on 2103-2 and 2103-4 in APD mode */
static struct attribute *emc2103_attributes_temp4[] = {
	&sensor_dev_attr_temp4_input.dev_attr.attr,
	&sensor_dev_attr_temp4_min.dev_attr.attr,
	&sensor_dev_attr_temp4_max.dev_attr.attr,
	&sensor_dev_attr_temp4_fault.dev_attr.attr,
	&sensor_dev_attr_temp4_min_alarm.dev_attr.attr,
	&sensor_dev_attr_temp4_max_alarm.dev_attr.attr,
	NULL
};

static const struct attribute_group emc2103_group = {
	.attrs = emc2103_attributes,
};

static const struct attribute_group emc2103_temp3_group = {
	.attrs = emc2103_attributes_temp3,
};

static const struct attribute_group emc2103_temp4_group = {
	.attrs = emc2103_attributes_temp4,
};

static int
emc2103_probe(struct i2c_client *client)
{
	struct emc2103_data *data;
	struct device *hwmon_dev;
	int status, idx = 0;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return -EIO;

	data = devm_kzalloc(&client->dev, sizeof(struct emc2103_data),
			    GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	i2c_set_clientdata(client, data);
	data->client = client;
	mutex_init(&data->update_lock);

	/* 2103-2 and 2103-4 have 3 external diodes, 2103-1 has 1 */
	status = i2c_smbus_read_byte_data(client, REG_PRODUCT_ID);
	if (status == 0x24) {
		/* 2103-1 only has 1 external diode */
		data->temp_count = 2;
	} else {
		/* 2103-2 and 2103-4 have 3 or 4 external diodes */
		status = i2c_smbus_read_byte_data(client, REG_CONF1);
		if (status < 0) {
			dev_dbg(&client->dev, "reg 0x%02x, err %d\n", REG_CONF1,
				status);
			return status;
		}

		/* detect current state of hardware */
		data->temp_count = (status & 0x01) ? 4 : 3;

		/* force APD state if module parameter is set */
		if (apd == 0) {
			/* force APD mode off */
			data->temp_count = 3;
			status &= ~(0x01);
			i2c_smbus_write_byte_data(client, REG_CONF1, status);
		} else if (apd == 1) {
			/* force APD mode on */
			data->temp_count = 4;
			status |= 0x01;
			i2c_smbus_write_byte_data(client, REG_CONF1, status);
		}
	}

	/* sysfs hooks */
	data->groups[idx++] = &emc2103_group;
	if (data->temp_count >= 3)
		data->groups[idx++] = &emc2103_temp3_group;
	if (data->temp_count == 4)
		data->groups[idx++] = &emc2103_temp4_group;

	hwmon_dev = devm_hwmon_device_register_with_groups(&client->dev,
							   client->name, data,
							   data->groups);
	if (IS_ERR(hwmon_dev))
		return PTR_ERR(hwmon_dev);

	dev_info(&client->dev, "%s: sensor '%s'\n",
		 dev_name(hwmon_dev), client->name);

	return 0;
}

static const struct i2c_device_id emc2103_ids[] = {
	{ "emc2103", 0, },
	{ /* LIST END */ }
};
MODULE_DEVICE_TABLE(i2c, emc2103_ids);

/* Return 0 if detection is successful, -ENODEV otherwise */
static int
emc2103_detect(struct i2c_client *new_client, struct i2c_board_info *info)
{
	struct i2c_adapter *adapter = new_client->adapter;
	int manufacturer, product;

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return -ENODEV;

	manufacturer = i2c_smbus_read_byte_data(new_client, REG_MFG_ID);
	if (manufacturer != 0x5D)
		return -ENODEV;

	product = i2c_smbus_read_byte_data(new_client, REG_PRODUCT_ID);
	if ((product != 0x24) && (product != 0x26))
		return -ENODEV;

	strlcpy(info->type, "emc2103", I2C_NAME_SIZE);

	return 0;
}

static struct i2c_driver emc2103_driver = {
	.class		= I2C_CLASS_HWMON,
	.driver = {
		.name	= "emc2103",
	},
	.probe_new	= emc2103_probe,
	.id_table	= emc2103_ids,
	.detect		= emc2103_detect,
	.address_list	= normal_i2c,
};

module_i2c_driver(emc2103_driver);

MODULE_AUTHOR("Steve Glendinning <steve.glendinning@shawell.net>");
MODULE_DESCRIPTION("SMSC EMC2103 hwmon driver");
MODULE_LICENSE("GPL");
