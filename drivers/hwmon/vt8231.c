// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * vt8231.c - Part of lm_sensors, Linux kernel modules
 *	      for hardware monitoring
 *
 * Copyright (c) 2005 Roger Lucas <vt8231@hiddenengine.co.uk>
 * Copyright (c) 2002 Mark D. Studebaker <mdsxyz123@yahoo.com>
 *		      Aaron M. Marsh <amarsh@sdf.lonestar.org>
 */

/*
 * Supports VIA VT8231 South Bridge embedded sensors
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/jiffies.h>
#include <linux/platform_device.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/hwmon-vid.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/acpi.h>
#include <linux/io.h>

static int force_addr;
module_param(force_addr, int, 0);
MODULE_PARM_DESC(force_addr, "Initialize the base address of the sensors");

static struct platform_device *pdev;

#define VT8231_EXTENT 0x80
#define VT8231_BASE_REG 0x70
#define VT8231_ENABLE_REG 0x74

/*
 * The VT8231 registers
 *
 * The reset value for the input channel configuration is used (Reg 0x4A=0x07)
 * which sets the selected inputs marked with '*' below if multiple options are
 * possible:
 *
 *		    Voltage Mode	  Temperature Mode
 *	Sensor	      Linux Id	      Linux Id	      VIA Id
 *	--------      --------	      --------	      ------
 *	CPU Diode	N/A		temp1		0
 *	UIC1		in0		temp2 *		1
 *	UIC2		in1 *		temp3		2
 *	UIC3		in2 *		temp4		3
 *	UIC4		in3 *		temp5		4
 *	UIC5		in4 *		temp6		5
 *	3.3V		in5		N/A
 *
 * Note that the BIOS may set the configuration register to a different value
 * to match the motherboard configuration.
 */

/* fans numbered 0-1 */
#define VT8231_REG_FAN_MIN(nr)	(0x3b + (nr))
#define VT8231_REG_FAN(nr)	(0x29 + (nr))

/* Voltage inputs numbered 0-5 */

static const u8 regvolt[]    = { 0x21, 0x22, 0x23, 0x24, 0x25, 0x26 };
static const u8 regvoltmax[] = { 0x3d, 0x2b, 0x2d, 0x2f, 0x31, 0x33 };
static const u8 regvoltmin[] = { 0x3e, 0x2c, 0x2e, 0x30, 0x32, 0x34 };

/*
 * Temperatures are numbered 1-6 according to the Linux kernel specification.
 *
 * In the VIA datasheet, however, the temperatures are numbered from zero.
 * Since it is important that this driver can easily be compared to the VIA
 * datasheet, we will use the VIA numbering within this driver and map the
 * kernel sysfs device name to the VIA number in the sysfs callback.
 */

#define VT8231_REG_TEMP_LOW01	0x49
#define VT8231_REG_TEMP_LOW25	0x4d

static const u8 regtemp[]    = { 0x1f, 0x21, 0x22, 0x23, 0x24, 0x25 };
static const u8 regtempmax[] = { 0x39, 0x3d, 0x2b, 0x2d, 0x2f, 0x31 };
static const u8 regtempmin[] = { 0x3a, 0x3e, 0x2c, 0x2e, 0x30, 0x32 };

#define TEMP_FROM_REG(reg)		(((253 * 4 - (reg)) * 550 + 105) / 210)
#define TEMP_MAXMIN_FROM_REG(reg)	(((253 - (reg)) * 2200 + 105) / 210)
#define TEMP_MAXMIN_TO_REG(val)		(253 - ((val) * 210 + 1100) / 2200)

#define VT8231_REG_CONFIG 0x40
#define VT8231_REG_ALARM1 0x41
#define VT8231_REG_ALARM2 0x42
#define VT8231_REG_FANDIV 0x47
#define VT8231_REG_UCH_CONFIG 0x4a
#define VT8231_REG_TEMP1_CONFIG 0x4b
#define VT8231_REG_TEMP2_CONFIG 0x4c

/*
 * temps 0-5 as numbered in VIA datasheet - see later for mapping to Linux
 * numbering
 */
#define ISTEMP(i, ch_config) ((i) == 0 ? 1 : \
			      ((ch_config) >> ((i)+1)) & 0x01)
/* voltages 0-5 */
#define ISVOLT(i, ch_config) ((i) == 5 ? 1 : \
			      !(((ch_config) >> ((i)+2)) & 0x01))

#define DIV_FROM_REG(val) (1 << (val))

/*
 * NB  The values returned here are NOT temperatures.  The calibration curves
 *     for the thermistor curves are board-specific and must go in the
 *     sensors.conf file.  Temperature sensors are actually ten bits, but the
 *     VIA datasheet only considers the 8 MSBs obtained from the regtemp[]
 *     register.  The temperature value returned should have a magnitude of 3,
 *     so we use the VIA scaling as the "true" scaling and use the remaining 2
 *     LSBs as fractional precision.
 *
 *     All the on-chip hardware temperature comparisons for the alarms are only
 *     8-bits wide, and compare against the 8 MSBs of the temperature.  The bits
 *     in the registers VT8231_REG_TEMP_LOW01 and VT8231_REG_TEMP_LOW25 are
 *     ignored.
 */

/*
 ****** FAN RPM CONVERSIONS ********
 * This chip saturates back at 0, not at 255 like many the other chips.
 * So, 0 means 0 RPM
 */
static inline u8 FAN_TO_REG(long rpm, int div)
{
	if (rpm <= 0 || rpm > 1310720)
		return 0;
	return clamp_val(1310720 / (rpm * div), 1, 255);
}

#define FAN_FROM_REG(val, div) ((val) == 0 ? 0 : 1310720 / ((val) * (div)))

struct vt8231_data {
	unsigned short addr;
	const char *name;

	struct mutex update_lock;
	struct device *hwmon_dev;
	char valid;		/* !=0 if following fields are valid */
	unsigned long last_updated;	/* In jiffies */

	u8 in[6];		/* Register value */
	u8 in_max[6];		/* Register value */
	u8 in_min[6];		/* Register value */
	u16 temp[6];		/* Register value 10 bit, right aligned */
	u8 temp_max[6];		/* Register value */
	u8 temp_min[6];		/* Register value */
	u8 fan[2];		/* Register value */
	u8 fan_min[2];		/* Register value */
	u8 fan_div[2];		/* Register encoding, shifted right */
	u16 alarms;		/* Register encoding */
	u8 uch_config;
};

static struct pci_dev *s_bridge;
static int vt8231_probe(struct platform_device *pdev);
static int vt8231_remove(struct platform_device *pdev);
static struct vt8231_data *vt8231_update_device(struct device *dev);
static void vt8231_init_device(struct vt8231_data *data);

static inline int vt8231_read_value(struct vt8231_data *data, u8 reg)
{
	return inb_p(data->addr + reg);
}

static inline void vt8231_write_value(struct vt8231_data *data, u8 reg,
					u8 value)
{
	outb_p(value, data->addr + reg);
}

/* following are the sysfs callback functions */
static ssize_t in_show(struct device *dev, struct device_attribute *attr,
		       char *buf)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int nr = sensor_attr->index;
	struct vt8231_data *data = vt8231_update_device(dev);

	return sprintf(buf, "%d\n", ((data->in[nr] - 3) * 10000) / 958);
}

static ssize_t in_min_show(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int nr = sensor_attr->index;
	struct vt8231_data *data = vt8231_update_device(dev);

	return sprintf(buf, "%d\n", ((data->in_min[nr] - 3) * 10000) / 958);
}

static ssize_t in_max_show(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int nr = sensor_attr->index;
	struct vt8231_data *data = vt8231_update_device(dev);

	return sprintf(buf, "%d\n", (((data->in_max[nr] - 3) * 10000) / 958));
}

static ssize_t in_min_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int nr = sensor_attr->index;
	struct vt8231_data *data = dev_get_drvdata(dev);
	unsigned long val;
	int err;

	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	data->in_min[nr] = clamp_val(((val * 958) / 10000) + 3, 0, 255);
	vt8231_write_value(data, regvoltmin[nr], data->in_min[nr]);
	mutex_unlock(&data->update_lock);
	return count;
}

static ssize_t in_max_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int nr = sensor_attr->index;
	struct vt8231_data *data = dev_get_drvdata(dev);
	unsigned long val;
	int err;

	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	data->in_max[nr] = clamp_val(((val * 958) / 10000) + 3, 0, 255);
	vt8231_write_value(data, regvoltmax[nr], data->in_max[nr]);
	mutex_unlock(&data->update_lock);
	return count;
}

/* Special case for input 5 as this has 3.3V scaling built into the chip */
static ssize_t in5_input_show(struct device *dev,
			      struct device_attribute *attr, char *buf)
{
	struct vt8231_data *data = vt8231_update_device(dev);

	return sprintf(buf, "%d\n",
		(((data->in[5] - 3) * 10000 * 54) / (958 * 34)));
}

static ssize_t in5_min_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct vt8231_data *data = vt8231_update_device(dev);

	return sprintf(buf, "%d\n",
		(((data->in_min[5] - 3) * 10000 * 54) / (958 * 34)));
}

static ssize_t in5_max_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct vt8231_data *data = vt8231_update_device(dev);

	return sprintf(buf, "%d\n",
		(((data->in_max[5] - 3) * 10000 * 54) / (958 * 34)));
}

static ssize_t in5_min_store(struct device *dev,
			     struct device_attribute *attr, const char *buf,
			     size_t count)
{
	struct vt8231_data *data = dev_get_drvdata(dev);
	unsigned long val;
	int err;

	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	data->in_min[5] = clamp_val(((val * 958 * 34) / (10000 * 54)) + 3,
				    0, 255);
	vt8231_write_value(data, regvoltmin[5], data->in_min[5]);
	mutex_unlock(&data->update_lock);
	return count;
}

static ssize_t in5_max_store(struct device *dev,
			     struct device_attribute *attr, const char *buf,
			     size_t count)
{
	struct vt8231_data *data = dev_get_drvdata(dev);
	unsigned long val;
	int err;

	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	data->in_max[5] = clamp_val(((val * 958 * 34) / (10000 * 54)) + 3,
				    0, 255);
	vt8231_write_value(data, regvoltmax[5], data->in_max[5]);
	mutex_unlock(&data->update_lock);
	return count;
}

static SENSOR_DEVICE_ATTR_RO(in0_input, in, 0);
static SENSOR_DEVICE_ATTR_RW(in0_min, in_min, 0);
static SENSOR_DEVICE_ATTR_RW(in0_max, in_max, 0);
static SENSOR_DEVICE_ATTR_RO(in1_input, in, 1);
static SENSOR_DEVICE_ATTR_RW(in1_min, in_min, 1);
static SENSOR_DEVICE_ATTR_RW(in1_max, in_max, 1);
static SENSOR_DEVICE_ATTR_RO(in2_input, in, 2);
static SENSOR_DEVICE_ATTR_RW(in2_min, in_min, 2);
static SENSOR_DEVICE_ATTR_RW(in2_max, in_max, 2);
static SENSOR_DEVICE_ATTR_RO(in3_input, in, 3);
static SENSOR_DEVICE_ATTR_RW(in3_min, in_min, 3);
static SENSOR_DEVICE_ATTR_RW(in3_max, in_max, 3);
static SENSOR_DEVICE_ATTR_RO(in4_input, in, 4);
static SENSOR_DEVICE_ATTR_RW(in4_min, in_min, 4);
static SENSOR_DEVICE_ATTR_RW(in4_max, in_max, 4);

static DEVICE_ATTR_RO(in5_input);
static DEVICE_ATTR_RW(in5_min);
static DEVICE_ATTR_RW(in5_max);

/* Temperatures */
static ssize_t temp1_input_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct vt8231_data *data = vt8231_update_device(dev);
	return sprintf(buf, "%d\n", data->temp[0] * 250);
}

static ssize_t temp1_max_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct vt8231_data *data = vt8231_update_device(dev);
	return sprintf(buf, "%d\n", data->temp_max[0] * 1000);
}

static ssize_t temp1_max_hyst_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct vt8231_data *data = vt8231_update_device(dev);
	return sprintf(buf, "%d\n", data->temp_min[0] * 1000);
}

static ssize_t temp1_max_store(struct device *dev,
			       struct device_attribute *attr, const char *buf,
			       size_t count)
{
	struct vt8231_data *data = dev_get_drvdata(dev);
	long val;
	int err;

	err = kstrtol(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	data->temp_max[0] = clamp_val((val + 500) / 1000, 0, 255);
	vt8231_write_value(data, regtempmax[0], data->temp_max[0]);
	mutex_unlock(&data->update_lock);
	return count;
}
static ssize_t temp1_max_hyst_store(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
	struct vt8231_data *data = dev_get_drvdata(dev);
	long val;
	int err;

	err = kstrtol(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	data->temp_min[0] = clamp_val((val + 500) / 1000, 0, 255);
	vt8231_write_value(data, regtempmin[0], data->temp_min[0]);
	mutex_unlock(&data->update_lock);
	return count;
}

static ssize_t temp_show(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int nr = sensor_attr->index;
	struct vt8231_data *data = vt8231_update_device(dev);
	return sprintf(buf, "%d\n", TEMP_FROM_REG(data->temp[nr]));
}

static ssize_t temp_max_show(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int nr = sensor_attr->index;
	struct vt8231_data *data = vt8231_update_device(dev);
	return sprintf(buf, "%d\n", TEMP_MAXMIN_FROM_REG(data->temp_max[nr]));
}

static ssize_t temp_min_show(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int nr = sensor_attr->index;
	struct vt8231_data *data = vt8231_update_device(dev);
	return sprintf(buf, "%d\n", TEMP_MAXMIN_FROM_REG(data->temp_min[nr]));
}

static ssize_t temp_max_store(struct device *dev,
			      struct device_attribute *attr, const char *buf,
			      size_t count)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int nr = sensor_attr->index;
	struct vt8231_data *data = dev_get_drvdata(dev);
	long val;
	int err;

	err = kstrtol(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	data->temp_max[nr] = clamp_val(TEMP_MAXMIN_TO_REG(val), 0, 255);
	vt8231_write_value(data, regtempmax[nr], data->temp_max[nr]);
	mutex_unlock(&data->update_lock);
	return count;
}
static ssize_t temp_min_store(struct device *dev,
			      struct device_attribute *attr, const char *buf,
			      size_t count)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int nr = sensor_attr->index;
	struct vt8231_data *data = dev_get_drvdata(dev);
	long val;
	int err;

	err = kstrtol(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	data->temp_min[nr] = clamp_val(TEMP_MAXMIN_TO_REG(val), 0, 255);
	vt8231_write_value(data, regtempmin[nr], data->temp_min[nr]);
	mutex_unlock(&data->update_lock);
	return count;
}

/*
 * Note that these map the Linux temperature sensor numbering (1-6) to the VIA
 * temperature sensor numbering (0-5)
 */

static DEVICE_ATTR_RO(temp1_input);
static DEVICE_ATTR_RW(temp1_max);
static DEVICE_ATTR_RW(temp1_max_hyst);

static SENSOR_DEVICE_ATTR_RO(temp2_input, temp, 1);
static SENSOR_DEVICE_ATTR_RW(temp2_max, temp_max, 1);
static SENSOR_DEVICE_ATTR_RW(temp2_max_hyst, temp_min, 1);
static SENSOR_DEVICE_ATTR_RO(temp3_input, temp, 2);
static SENSOR_DEVICE_ATTR_RW(temp3_max, temp_max, 2);
static SENSOR_DEVICE_ATTR_RW(temp3_max_hyst, temp_min, 2);
static SENSOR_DEVICE_ATTR_RO(temp4_input, temp, 3);
static SENSOR_DEVICE_ATTR_RW(temp4_max, temp_max, 3);
static SENSOR_DEVICE_ATTR_RW(temp4_max_hyst, temp_min, 3);
static SENSOR_DEVICE_ATTR_RO(temp5_input, temp, 4);
static SENSOR_DEVICE_ATTR_RW(temp5_max, temp_max, 4);
static SENSOR_DEVICE_ATTR_RW(temp5_max_hyst, temp_min, 4);
static SENSOR_DEVICE_ATTR_RO(temp6_input, temp, 5);
static SENSOR_DEVICE_ATTR_RW(temp6_max, temp_max, 5);
static SENSOR_DEVICE_ATTR_RW(temp6_max_hyst, temp_min, 5);

/* Fans */
static ssize_t fan_show(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int nr = sensor_attr->index;
	struct vt8231_data *data = vt8231_update_device(dev);
	return sprintf(buf, "%d\n", FAN_FROM_REG(data->fan[nr],
				DIV_FROM_REG(data->fan_div[nr])));
}

static ssize_t fan_min_show(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int nr = sensor_attr->index;
	struct vt8231_data *data = vt8231_update_device(dev);
	return sprintf(buf, "%d\n", FAN_FROM_REG(data->fan_min[nr],
			DIV_FROM_REG(data->fan_div[nr])));
}

static ssize_t fan_div_show(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int nr = sensor_attr->index;
	struct vt8231_data *data = vt8231_update_device(dev);
	return sprintf(buf, "%d\n", DIV_FROM_REG(data->fan_div[nr]));
}

static ssize_t fan_min_store(struct device *dev,
			     struct device_attribute *attr, const char *buf,
			     size_t count)
{
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	int nr = sensor_attr->index;
	struct vt8231_data *data = dev_get_drvdata(dev);
	unsigned long val;
	int err;

	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	data->fan_min[nr] = FAN_TO_REG(val, DIV_FROM_REG(data->fan_div[nr]));
	vt8231_write_value(data, VT8231_REG_FAN_MIN(nr), data->fan_min[nr]);
	mutex_unlock(&data->update_lock);
	return count;
}

static ssize_t fan_div_store(struct device *dev,
			     struct device_attribute *attr, const char *buf,
			     size_t count)
{
	struct vt8231_data *data = dev_get_drvdata(dev);
	struct sensor_device_attribute *sensor_attr = to_sensor_dev_attr(attr);
	unsigned long val;
	int nr = sensor_attr->index;
	int old = vt8231_read_value(data, VT8231_REG_FANDIV);
	long min = FAN_FROM_REG(data->fan_min[nr],
				 DIV_FROM_REG(data->fan_div[nr]));
	int err;

	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	switch (val) {
	case 1:
		data->fan_div[nr] = 0;
		break;
	case 2:
		data->fan_div[nr] = 1;
		break;
	case 4:
		data->fan_div[nr] = 2;
		break;
	case 8:
		data->fan_div[nr] = 3;
		break;
	default:
		dev_err(dev,
			"fan_div value %ld not supported. Choose one of 1, 2, 4 or 8!\n",
			val);
		mutex_unlock(&data->update_lock);
		return -EINVAL;
	}

	/* Correct the fan minimum speed */
	data->fan_min[nr] = FAN_TO_REG(min, DIV_FROM_REG(data->fan_div[nr]));
	vt8231_write_value(data, VT8231_REG_FAN_MIN(nr), data->fan_min[nr]);

	old = (old & 0x0f) | (data->fan_div[1] << 6) | (data->fan_div[0] << 4);
	vt8231_write_value(data, VT8231_REG_FANDIV, old);
	mutex_unlock(&data->update_lock);
	return count;
}

static SENSOR_DEVICE_ATTR_RO(fan1_input, fan, 0);
static SENSOR_DEVICE_ATTR_RW(fan1_min, fan_min, 0);
static SENSOR_DEVICE_ATTR_RW(fan1_div, fan_div, 0);
static SENSOR_DEVICE_ATTR_RO(fan2_input, fan, 1);
static SENSOR_DEVICE_ATTR_RW(fan2_min, fan_min, 1);
static SENSOR_DEVICE_ATTR_RW(fan2_div, fan_div, 1);

/* Alarms */
static ssize_t alarms_show(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	struct vt8231_data *data = vt8231_update_device(dev);
	return sprintf(buf, "%d\n", data->alarms);
}
static DEVICE_ATTR_RO(alarms);

static ssize_t alarm_show(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	int bitnr = to_sensor_dev_attr(attr)->index;
	struct vt8231_data *data = vt8231_update_device(dev);
	return sprintf(buf, "%u\n", (data->alarms >> bitnr) & 1);
}
static SENSOR_DEVICE_ATTR_RO(temp1_alarm, alarm, 4);
static SENSOR_DEVICE_ATTR_RO(temp2_alarm, alarm, 11);
static SENSOR_DEVICE_ATTR_RO(temp3_alarm, alarm, 0);
static SENSOR_DEVICE_ATTR_RO(temp4_alarm, alarm, 1);
static SENSOR_DEVICE_ATTR_RO(temp5_alarm, alarm, 3);
static SENSOR_DEVICE_ATTR_RO(temp6_alarm, alarm, 8);
static SENSOR_DEVICE_ATTR_RO(in0_alarm, alarm, 11);
static SENSOR_DEVICE_ATTR_RO(in1_alarm, alarm, 0);
static SENSOR_DEVICE_ATTR_RO(in2_alarm, alarm, 1);
static SENSOR_DEVICE_ATTR_RO(in3_alarm, alarm, 3);
static SENSOR_DEVICE_ATTR_RO(in4_alarm, alarm, 8);
static SENSOR_DEVICE_ATTR_RO(in5_alarm, alarm, 2);
static SENSOR_DEVICE_ATTR_RO(fan1_alarm, alarm, 6);
static SENSOR_DEVICE_ATTR_RO(fan2_alarm, alarm, 7);

static ssize_t name_show(struct device *dev, struct device_attribute
			 *devattr, char *buf)
{
	struct vt8231_data *data = dev_get_drvdata(dev);
	return sprintf(buf, "%s\n", data->name);
}
static DEVICE_ATTR_RO(name);

static struct attribute *vt8231_attributes_temps[6][5] = {
	{
		&dev_attr_temp1_input.attr,
		&dev_attr_temp1_max_hyst.attr,
		&dev_attr_temp1_max.attr,
		&sensor_dev_attr_temp1_alarm.dev_attr.attr,
		NULL
	}, {
		&sensor_dev_attr_temp2_input.dev_attr.attr,
		&sensor_dev_attr_temp2_max_hyst.dev_attr.attr,
		&sensor_dev_attr_temp2_max.dev_attr.attr,
		&sensor_dev_attr_temp2_alarm.dev_attr.attr,
		NULL
	}, {
		&sensor_dev_attr_temp3_input.dev_attr.attr,
		&sensor_dev_attr_temp3_max_hyst.dev_attr.attr,
		&sensor_dev_attr_temp3_max.dev_attr.attr,
		&sensor_dev_attr_temp3_alarm.dev_attr.attr,
		NULL
	}, {
		&sensor_dev_attr_temp4_input.dev_attr.attr,
		&sensor_dev_attr_temp4_max_hyst.dev_attr.attr,
		&sensor_dev_attr_temp4_max.dev_attr.attr,
		&sensor_dev_attr_temp4_alarm.dev_attr.attr,
		NULL
	}, {
		&sensor_dev_attr_temp5_input.dev_attr.attr,
		&sensor_dev_attr_temp5_max_hyst.dev_attr.attr,
		&sensor_dev_attr_temp5_max.dev_attr.attr,
		&sensor_dev_attr_temp5_alarm.dev_attr.attr,
		NULL
	}, {
		&sensor_dev_attr_temp6_input.dev_attr.attr,
		&sensor_dev_attr_temp6_max_hyst.dev_attr.attr,
		&sensor_dev_attr_temp6_max.dev_attr.attr,
		&sensor_dev_attr_temp6_alarm.dev_attr.attr,
		NULL
	}
};

static const struct attribute_group vt8231_group_temps[6] = {
	{ .attrs = vt8231_attributes_temps[0] },
	{ .attrs = vt8231_attributes_temps[1] },
	{ .attrs = vt8231_attributes_temps[2] },
	{ .attrs = vt8231_attributes_temps[3] },
	{ .attrs = vt8231_attributes_temps[4] },
	{ .attrs = vt8231_attributes_temps[5] },
};

static struct attribute *vt8231_attributes_volts[6][5] = {
	{
		&sensor_dev_attr_in0_input.dev_attr.attr,
		&sensor_dev_attr_in0_min.dev_attr.attr,
		&sensor_dev_attr_in0_max.dev_attr.attr,
		&sensor_dev_attr_in0_alarm.dev_attr.attr,
		NULL
	}, {
		&sensor_dev_attr_in1_input.dev_attr.attr,
		&sensor_dev_attr_in1_min.dev_attr.attr,
		&sensor_dev_attr_in1_max.dev_attr.attr,
		&sensor_dev_attr_in1_alarm.dev_attr.attr,
		NULL
	}, {
		&sensor_dev_attr_in2_input.dev_attr.attr,
		&sensor_dev_attr_in2_min.dev_attr.attr,
		&sensor_dev_attr_in2_max.dev_attr.attr,
		&sensor_dev_attr_in2_alarm.dev_attr.attr,
		NULL
	}, {
		&sensor_dev_attr_in3_input.dev_attr.attr,
		&sensor_dev_attr_in3_min.dev_attr.attr,
		&sensor_dev_attr_in3_max.dev_attr.attr,
		&sensor_dev_attr_in3_alarm.dev_attr.attr,
		NULL
	}, {
		&sensor_dev_attr_in4_input.dev_attr.attr,
		&sensor_dev_attr_in4_min.dev_attr.attr,
		&sensor_dev_attr_in4_max.dev_attr.attr,
		&sensor_dev_attr_in4_alarm.dev_attr.attr,
		NULL
	}, {
		&dev_attr_in5_input.attr,
		&dev_attr_in5_min.attr,
		&dev_attr_in5_max.attr,
		&sensor_dev_attr_in5_alarm.dev_attr.attr,
		NULL
	}
};

static const struct attribute_group vt8231_group_volts[6] = {
	{ .attrs = vt8231_attributes_volts[0] },
	{ .attrs = vt8231_attributes_volts[1] },
	{ .attrs = vt8231_attributes_volts[2] },
	{ .attrs = vt8231_attributes_volts[3] },
	{ .attrs = vt8231_attributes_volts[4] },
	{ .attrs = vt8231_attributes_volts[5] },
};

static struct attribute *vt8231_attributes[] = {
	&sensor_dev_attr_fan1_input.dev_attr.attr,
	&sensor_dev_attr_fan2_input.dev_attr.attr,
	&sensor_dev_attr_fan1_min.dev_attr.attr,
	&sensor_dev_attr_fan2_min.dev_attr.attr,
	&sensor_dev_attr_fan1_div.dev_attr.attr,
	&sensor_dev_attr_fan2_div.dev_attr.attr,
	&sensor_dev_attr_fan1_alarm.dev_attr.attr,
	&sensor_dev_attr_fan2_alarm.dev_attr.attr,
	&dev_attr_alarms.attr,
	&dev_attr_name.attr,
	NULL
};

static const struct attribute_group vt8231_group = {
	.attrs = vt8231_attributes,
};

static struct platform_driver vt8231_driver = {
	.driver = {
		.name	= "vt8231",
	},
	.probe	= vt8231_probe,
	.remove	= vt8231_remove,
};

static const struct pci_device_id vt8231_pci_ids[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231_4) },
	{ 0, }
};

MODULE_DEVICE_TABLE(pci, vt8231_pci_ids);

static int vt8231_pci_probe(struct pci_dev *dev,
				      const struct pci_device_id *id);

static struct pci_driver vt8231_pci_driver = {
	.name		= "vt8231",
	.id_table	= vt8231_pci_ids,
	.probe		= vt8231_pci_probe,
};

static int vt8231_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct vt8231_data *data;
	int err = 0, i;

	/* Reserve the ISA region */
	res = platform_get_resource(pdev, IORESOURCE_IO, 0);
	if (!devm_request_region(&pdev->dev, res->start, VT8231_EXTENT,
				 vt8231_driver.driver.name)) {
		dev_err(&pdev->dev, "Region 0x%lx-0x%lx already in use!\n",
			(unsigned long)res->start, (unsigned long)res->end);
		return -ENODEV;
	}

	data = devm_kzalloc(&pdev->dev, sizeof(struct vt8231_data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	platform_set_drvdata(pdev, data);
	data->addr = res->start;
	data->name = "vt8231";

	mutex_init(&data->update_lock);
	vt8231_init_device(data);

	/* Register sysfs hooks */
	err = sysfs_create_group(&pdev->dev.kobj, &vt8231_group);
	if (err)
		return err;

	/* Must update device information to find out the config field */
	data->uch_config = vt8231_read_value(data, VT8231_REG_UCH_CONFIG);

	for (i = 0; i < ARRAY_SIZE(vt8231_group_temps); i++) {
		if (ISTEMP(i, data->uch_config)) {
			err = sysfs_create_group(&pdev->dev.kobj,
						 &vt8231_group_temps[i]);
			if (err)
				goto exit_remove_files;
		}
	}

	for (i = 0; i < ARRAY_SIZE(vt8231_group_volts); i++) {
		if (ISVOLT(i, data->uch_config)) {
			err = sysfs_create_group(&pdev->dev.kobj,
						 &vt8231_group_volts[i]);
			if (err)
				goto exit_remove_files;
		}
	}

	data->hwmon_dev = hwmon_device_register(&pdev->dev);
	if (IS_ERR(data->hwmon_dev)) {
		err = PTR_ERR(data->hwmon_dev);
		goto exit_remove_files;
	}
	return 0;

exit_remove_files:
	for (i = 0; i < ARRAY_SIZE(vt8231_group_volts); i++)
		sysfs_remove_group(&pdev->dev.kobj, &vt8231_group_volts[i]);

	for (i = 0; i < ARRAY_SIZE(vt8231_group_temps); i++)
		sysfs_remove_group(&pdev->dev.kobj, &vt8231_group_temps[i]);

	sysfs_remove_group(&pdev->dev.kobj, &vt8231_group);
	return err;
}

static int vt8231_remove(struct platform_device *pdev)
{
	struct vt8231_data *data = platform_get_drvdata(pdev);
	int i;

	hwmon_device_unregister(data->hwmon_dev);

	for (i = 0; i < ARRAY_SIZE(vt8231_group_volts); i++)
		sysfs_remove_group(&pdev->dev.kobj, &vt8231_group_volts[i]);

	for (i = 0; i < ARRAY_SIZE(vt8231_group_temps); i++)
		sysfs_remove_group(&pdev->dev.kobj, &vt8231_group_temps[i]);

	sysfs_remove_group(&pdev->dev.kobj, &vt8231_group);

	return 0;
}

static void vt8231_init_device(struct vt8231_data *data)
{
	vt8231_write_value(data, VT8231_REG_TEMP1_CONFIG, 0);
	vt8231_write_value(data, VT8231_REG_TEMP2_CONFIG, 0);
}

static struct vt8231_data *vt8231_update_device(struct device *dev)
{
	struct vt8231_data *data = dev_get_drvdata(dev);
	int i;
	u16 low;

	mutex_lock(&data->update_lock);

	if (time_after(jiffies, data->last_updated + HZ + HZ / 2)
	    || !data->valid) {
		for (i = 0; i < 6; i++) {
			if (ISVOLT(i, data->uch_config)) {
				data->in[i] = vt8231_read_value(data,
						regvolt[i]);
				data->in_min[i] = vt8231_read_value(data,
						regvoltmin[i]);
				data->in_max[i] = vt8231_read_value(data,
						regvoltmax[i]);
			}
		}
		for (i = 0; i < 2; i++) {
			data->fan[i] = vt8231_read_value(data,
						VT8231_REG_FAN(i));
			data->fan_min[i] = vt8231_read_value(data,
						VT8231_REG_FAN_MIN(i));
		}

		low = vt8231_read_value(data, VT8231_REG_TEMP_LOW01);
		low = (low >> 6) | ((low & 0x30) >> 2)
		    | (vt8231_read_value(data, VT8231_REG_TEMP_LOW25) << 4);
		for (i = 0; i < 6; i++) {
			if (ISTEMP(i, data->uch_config)) {
				data->temp[i] = (vt8231_read_value(data,
						       regtemp[i]) << 2)
						| ((low >> (2 * i)) & 0x03);
				data->temp_max[i] = vt8231_read_value(data,
						      regtempmax[i]);
				data->temp_min[i] = vt8231_read_value(data,
						      regtempmin[i]);
			}
		}

		i = vt8231_read_value(data, VT8231_REG_FANDIV);
		data->fan_div[0] = (i >> 4) & 0x03;
		data->fan_div[1] = i >> 6;
		data->alarms = vt8231_read_value(data, VT8231_REG_ALARM1) |
			(vt8231_read_value(data, VT8231_REG_ALARM2) << 8);

		/* Set alarm flags correctly */
		if (!data->fan[0] && data->fan_min[0])
			data->alarms |= 0x40;
		else if (data->fan[0] && !data->fan_min[0])
			data->alarms &= ~0x40;

		if (!data->fan[1] && data->fan_min[1])
			data->alarms |= 0x80;
		else if (data->fan[1] && !data->fan_min[1])
			data->alarms &= ~0x80;

		data->last_updated = jiffies;
		data->valid = 1;
	}

	mutex_unlock(&data->update_lock);

	return data;
}

static int vt8231_device_add(unsigned short address)
{
	struct resource res = {
		.start	= address,
		.end	= address + VT8231_EXTENT - 1,
		.name	= "vt8231",
		.flags	= IORESOURCE_IO,
	};
	int err;

	err = acpi_check_resource_conflict(&res);
	if (err)
		goto exit;

	pdev = platform_device_alloc("vt8231", address);
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

static int vt8231_pci_probe(struct pci_dev *dev,
				const struct pci_device_id *id)
{
	u16 address, val;
	if (force_addr) {
		address = force_addr & 0xff00;
		dev_warn(&dev->dev, "Forcing ISA address 0x%x\n",
			 address);

		if (PCIBIOS_SUCCESSFUL !=
		    pci_write_config_word(dev, VT8231_BASE_REG, address | 1))
			return -ENODEV;
	}

	pci_read_config_word(dev, VT8231_BASE_REG, &val);
	if (val == (u16)~0)
		return -ENODEV;

	address = val & ~(VT8231_EXTENT - 1);
	if (address == 0) {
		dev_err(&dev->dev, "base address not set - upgrade BIOS or use force_addr=0xaddr\n");
		return -ENODEV;
	}

	pci_read_config_word(dev, VT8231_ENABLE_REG, &val);
	if (val == (u16)~0)
		return -ENODEV;

	if (!(val & 0x0001)) {
		dev_warn(&dev->dev, "enabling sensors\n");
		if (PCIBIOS_SUCCESSFUL !=
			pci_write_config_word(dev, VT8231_ENABLE_REG,
							val | 0x0001))
			return -ENODEV;
	}

	if (platform_driver_register(&vt8231_driver))
		goto exit;

	/* Sets global pdev as a side effect */
	if (vt8231_device_add(address))
		goto exit_unregister;

	/*
	 * Always return failure here.  This is to allow other drivers to bind
	 * to this pci device.  We don't really want to have control over the
	 * pci device, we only wanted to read as few register values from it.
	 */

	/*
	 * We do, however, mark ourselves as using the PCI device to stop it
	 * getting unloaded.
	 */
	s_bridge = pci_dev_get(dev);
	return -ENODEV;

exit_unregister:
	platform_driver_unregister(&vt8231_driver);
exit:
	return -ENODEV;
}

static int __init sm_vt8231_init(void)
{
	return pci_register_driver(&vt8231_pci_driver);
}

static void __exit sm_vt8231_exit(void)
{
	pci_unregister_driver(&vt8231_pci_driver);
	if (s_bridge != NULL) {
		platform_device_unregister(pdev);
		platform_driver_unregister(&vt8231_driver);
		pci_dev_put(s_bridge);
		s_bridge = NULL;
	}
}

MODULE_AUTHOR("Roger Lucas <vt8231@hiddenengine.co.uk>");
MODULE_DESCRIPTION("VT8231 sensors");
MODULE_LICENSE("GPL");

module_init(sm_vt8231_init);
module_exit(sm_vt8231_exit);
