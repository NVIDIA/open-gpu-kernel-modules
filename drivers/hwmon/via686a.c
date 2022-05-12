// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * via686a.c - Part of lm_sensors, Linux kernel modules
 *	       for hardware monitoring
 *
 * Copyright (c) 1998 - 2002  Frodo Looijaard <frodol@dds.nl>,
 *			      Kyösti Mälkki <kmalkki@cc.hut.fi>,
 *			      Mark Studebaker <mdsxyz123@yahoo.com>,
 *			      and Bob Dougherty <bobd@stanford.edu>
 *
 * (Some conversion-factor data were contributed by Jonathan Teh Soon Yew
 * <j.teh@iname.com> and Alex van Kaam <darkside@chello.nl>.)
 */

/*
 * Supports the Via VT82C686A, VT82C686B south bridges.
 * Reports all as a 686A.
 * Warning - only supports a single device.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/jiffies.h>
#include <linux/platform_device.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/sysfs.h>
#include <linux/acpi.h>
#include <linux/io.h>

/*
 * If force_addr is set to anything different from 0, we forcibly enable
 * the device at the given address.
 */
static unsigned short force_addr;
module_param(force_addr, ushort, 0);
MODULE_PARM_DESC(force_addr,
		 "Initialize the base address of the sensors");

static struct platform_device *pdev;

/*
 * The Via 686a southbridge has a LM78-like chip integrated on the same IC.
 * This driver is a customized copy of lm78.c
 */

/* Many VIA686A constants specified below */

/* Length of ISA address segment */
#define VIA686A_EXTENT		0x80
#define VIA686A_BASE_REG	0x70
#define VIA686A_ENABLE_REG	0x74

/* The VIA686A registers */
/* ins numbered 0-4 */
#define VIA686A_REG_IN_MAX(nr)	(0x2b + ((nr) * 2))
#define VIA686A_REG_IN_MIN(nr)	(0x2c + ((nr) * 2))
#define VIA686A_REG_IN(nr)	(0x22 + (nr))

/* fans numbered 1-2 */
#define VIA686A_REG_FAN_MIN(nr)	(0x3a + (nr))
#define VIA686A_REG_FAN(nr)	(0x28 + (nr))

/* temps numbered 1-3 */
static const u8 VIA686A_REG_TEMP[]	= { 0x20, 0x21, 0x1f };
static const u8 VIA686A_REG_TEMP_OVER[]	= { 0x39, 0x3d, 0x1d };
static const u8 VIA686A_REG_TEMP_HYST[]	= { 0x3a, 0x3e, 0x1e };
/* bits 7-6 */
#define VIA686A_REG_TEMP_LOW1	0x4b
/* 2 = bits 5-4, 3 = bits 7-6 */
#define VIA686A_REG_TEMP_LOW23	0x49

#define VIA686A_REG_ALARM1	0x41
#define VIA686A_REG_ALARM2	0x42
#define VIA686A_REG_FANDIV	0x47
#define VIA686A_REG_CONFIG	0x40
/*
 * The following register sets temp interrupt mode (bits 1-0 for temp1,
 * 3-2 for temp2, 5-4 for temp3).  Modes are:
 * 00 interrupt stays as long as value is out-of-range
 * 01 interrupt is cleared once register is read (default)
 * 10 comparator mode- like 00, but ignores hysteresis
 * 11 same as 00
 */
#define VIA686A_REG_TEMP_MODE		0x4b
/* We'll just assume that you want to set all 3 simultaneously: */
#define VIA686A_TEMP_MODE_MASK		0x3F
#define VIA686A_TEMP_MODE_CONTINUOUS	0x00

/*
 * Conversions. Limit checking is only done on the TO_REG
 * variants.
 *
 ******** VOLTAGE CONVERSIONS (Bob Dougherty) ********
 * From HWMon.cpp (Copyright 1998-2000 Jonathan Teh Soon Yew):
 * voltagefactor[0]=1.25/2628; (2628/1.25=2102.4)   // Vccp
 * voltagefactor[1]=1.25/2628; (2628/1.25=2102.4)   // +2.5V
 * voltagefactor[2]=1.67/2628; (2628/1.67=1573.7)   // +3.3V
 * voltagefactor[3]=2.6/2628;  (2628/2.60=1010.8)   // +5V
 * voltagefactor[4]=6.3/2628;  (2628/6.30=417.14)   // +12V
 * in[i]=(data[i+2]*25.0+133)*voltagefactor[i];
 * That is:
 * volts = (25*regVal+133)*factor
 * regVal = (volts/factor-133)/25
 * (These conversions were contributed by Jonathan Teh Soon Yew
 * <j.teh@iname.com>)
 */
static inline u8 IN_TO_REG(long val, int in_num)
{
	/*
	 * To avoid floating point, we multiply constants by 10 (100 for +12V).
	 * Rounding is done (120500 is actually 133000 - 12500).
	 * Remember that val is expressed in 0.001V/bit, which is why we divide
	 * by an additional 10000 (100000 for +12V): 1000 for val and 10 (100)
	 * for the constants.
	 */
	if (in_num <= 1)
		return (u8) clamp_val((val * 21024 - 1205000) / 250000, 0, 255);
	else if (in_num == 2)
		return (u8) clamp_val((val * 15737 - 1205000) / 250000, 0, 255);
	else if (in_num == 3)
		return (u8) clamp_val((val * 10108 - 1205000) / 250000, 0, 255);
	else
		return (u8) clamp_val((val * 41714 - 12050000) / 2500000, 0,
				      255);
}

static inline long IN_FROM_REG(u8 val, int in_num)
{
	/*
	 * To avoid floating point, we multiply constants by 10 (100 for +12V).
	 * We also multiply them by 1000 because we want 0.001V/bit for the
	 * output value. Rounding is done.
	 */
	if (in_num <= 1)
		return (long) ((250000 * val + 1330000 + 21024 / 2) / 21024);
	else if (in_num == 2)
		return (long) ((250000 * val + 1330000 + 15737 / 2) / 15737);
	else if (in_num == 3)
		return (long) ((250000 * val + 1330000 + 10108 / 2) / 10108);
	else
		return (long) ((2500000 * val + 13300000 + 41714 / 2) / 41714);
}

/********* FAN RPM CONVERSIONS ********/
/*
 * Higher register values = slower fans (the fan's strobe gates a counter).
 * But this chip saturates back at 0, not at 255 like all the other chips.
 * So, 0 means 0 RPM
 */
static inline u8 FAN_TO_REG(long rpm, int div)
{
	if (rpm == 0)
		return 0;
	rpm = clamp_val(rpm, 1, 1000000);
	return clamp_val((1350000 + rpm * div / 2) / (rpm * div), 1, 255);
}

#define FAN_FROM_REG(val, div) ((val) == 0 ? 0 : (val) == 255 ? 0 : 1350000 / \
				((val) * (div)))

/******** TEMP CONVERSIONS (Bob Dougherty) *********/
/*
 * linear fits from HWMon.cpp (Copyright 1998-2000 Jonathan Teh Soon Yew)
 *	if(temp<169)
 *		return double(temp)*0.427-32.08;
 *	else if(temp>=169 && temp<=202)
 *		return double(temp)*0.582-58.16;
 *	else
 *		return double(temp)*0.924-127.33;
 *
 * A fifth-order polynomial fits the unofficial data (provided by Alex van
 * Kaam <darkside@chello.nl>) a bit better.  It also give more reasonable
 * numbers on my machine (ie. they agree with what my BIOS tells me).
 * Here's the fifth-order fit to the 8-bit data:
 * temp = 1.625093e-10*val^5 - 1.001632e-07*val^4 + 2.457653e-05*val^3 -
 *	2.967619e-03*val^2 + 2.175144e-01*val - 7.090067e+0.
 *
 * (2000-10-25- RFD: thanks to Uwe Andersen <uandersen@mayah.com> for
 * finding my typos in this formula!)
 *
 * Alas, none of the elegant function-fit solutions will work because we
 * aren't allowed to use floating point in the kernel and doing it with
 * integers doesn't provide enough precision.  So we'll do boring old
 * look-up table stuff.  The unofficial data (see below) have effectively
 * 7-bit resolution (they are rounded to the nearest degree).  I'm assuming
 * that the transfer function of the device is monotonic and smooth, so a
 * smooth function fit to the data will allow us to get better precision.
 * I used the 5th-order poly fit described above and solved for
 * VIA register values 0-255.  I *10 before rounding, so we get tenth-degree
 * precision.  (I could have done all 1024 values for our 10-bit readings,
 * but the function is very linear in the useful range (0-80 deg C), so
 * we'll just use linear interpolation for 10-bit readings.)  So, temp_lut
 * is the temp at via register values 0-255:
 */
static const s16 temp_lut[] = {
	-709, -688, -667, -646, -627, -607, -589, -570, -553, -536, -519,
	-503, -487, -471, -456, -442, -428, -414, -400, -387, -375,
	-362, -350, -339, -327, -316, -305, -295, -285, -275, -265,
	-255, -246, -237, -229, -220, -212, -204, -196, -188, -180,
	-173, -166, -159, -152, -145, -139, -132, -126, -120, -114,
	-108, -102, -96, -91, -85, -80, -74, -69, -64, -59, -54, -49,
	-44, -39, -34, -29, -25, -20, -15, -11, -6, -2, 3, 7, 12, 16,
	20, 25, 29, 33, 37, 42, 46, 50, 54, 59, 63, 67, 71, 75, 79, 84,
	88, 92, 96, 100, 104, 109, 113, 117, 121, 125, 130, 134, 138,
	142, 146, 151, 155, 159, 163, 168, 172, 176, 181, 185, 189,
	193, 198, 202, 206, 211, 215, 219, 224, 228, 232, 237, 241,
	245, 250, 254, 259, 263, 267, 272, 276, 281, 285, 290, 294,
	299, 303, 307, 312, 316, 321, 325, 330, 334, 339, 344, 348,
	353, 357, 362, 366, 371, 376, 380, 385, 390, 395, 399, 404,
	409, 414, 419, 423, 428, 433, 438, 443, 449, 454, 459, 464,
	469, 475, 480, 486, 491, 497, 502, 508, 514, 520, 526, 532,
	538, 544, 551, 557, 564, 571, 578, 584, 592, 599, 606, 614,
	621, 629, 637, 645, 654, 662, 671, 680, 689, 698, 708, 718,
	728, 738, 749, 759, 770, 782, 793, 805, 818, 830, 843, 856,
	870, 883, 898, 912, 927, 943, 958, 975, 991, 1008, 1026, 1044,
	1062, 1081, 1101, 1121, 1141, 1162, 1184, 1206, 1229, 1252,
	1276, 1301, 1326, 1352, 1378, 1406, 1434, 1462
};

/*
 * the original LUT values from Alex van Kaam <darkside@chello.nl>
 * (for via register values 12-240):
 * {-50,-49,-47,-45,-43,-41,-39,-38,-37,-35,-34,-33,-32,-31,
 * -30,-29,-28,-27,-26,-25,-24,-24,-23,-22,-21,-20,-20,-19,-18,-17,-17,-16,-15,
 * -15,-14,-14,-13,-12,-12,-11,-11,-10,-9,-9,-8,-8,-7,-7,-6,-6,-5,-5,-4,-4,-3,
 * -3,-2,-2,-1,-1,0,0,1,1,1,3,3,3,4,4,4,5,5,5,6,6,7,7,8,8,9,9,9,10,10,11,11,12,
 * 12,12,13,13,13,14,14,15,15,16,16,16,17,17,18,18,19,19,20,20,21,21,21,22,22,
 * 22,23,23,24,24,25,25,26,26,26,27,27,27,28,28,29,29,30,30,30,31,31,32,32,33,
 * 33,34,34,35,35,35,36,36,37,37,38,38,39,39,40,40,41,41,42,42,43,43,44,44,45,
 * 45,46,46,47,48,48,49,49,50,51,51,52,52,53,53,54,55,55,56,57,57,58,59,59,60,
 * 61,62,62,63,64,65,66,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,83,84,
 * 85,86,88,89,91,92,94,96,97,99,101,103,105,107,109,110};
 *
 *
 * Here's the reverse LUT.  I got it by doing a 6-th order poly fit (needed
 * an extra term for a good fit to these inverse data!) and then
 * solving for each temp value from -50 to 110 (the useable range for
 * this chip).  Here's the fit:
 * viaRegVal = -1.160370e-10*val^6 +3.193693e-08*val^5 - 1.464447e-06*val^4
 * - 2.525453e-04*val^3 + 1.424593e-02*val^2 + 2.148941e+00*val +7.275808e+01)
 * Note that n=161:
 */
static const u8 via_lut[] = {
	12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 18, 19, 20, 20, 21, 22, 23,
	23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 35, 36, 37, 39, 40,
	41, 43, 45, 46, 48, 49, 51, 53, 55, 57, 59, 60, 62, 64, 66,
	69, 71, 73, 75, 77, 79, 82, 84, 86, 88, 91, 93, 95, 98, 100,
	103, 105, 107, 110, 112, 115, 117, 119, 122, 124, 126, 129,
	131, 134, 136, 138, 140, 143, 145, 147, 150, 152, 154, 156,
	158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180,
	182, 183, 185, 187, 188, 190, 192, 193, 195, 196, 198, 199,
	200, 202, 203, 205, 206, 207, 208, 209, 210, 211, 212, 213,
	214, 215, 216, 217, 218, 219, 220, 221, 222, 222, 223, 224,
	225, 226, 226, 227, 228, 228, 229, 230, 230, 231, 232, 232,
	233, 233, 234, 235, 235, 236, 236, 237, 237, 238, 238, 239,
	239, 240
};

/*
 * Converting temps to (8-bit) hyst and over registers
 * No interpolation here.
 * The +50 is because the temps start at -50
 */
static inline u8 TEMP_TO_REG(long val)
{
	return via_lut[val <= -50000 ? 0 : val >= 110000 ? 160 :
		      (val < 0 ? val - 500 : val + 500) / 1000 + 50];
}

/* for 8-bit temperature hyst and over registers */
#define TEMP_FROM_REG(val)	((long)temp_lut[val] * 100)

/* for 10-bit temperature readings */
static inline long TEMP_FROM_REG10(u16 val)
{
	u16 eight_bits = val >> 2;
	u16 two_bits = val & 3;

	/* no interpolation for these */
	if (two_bits == 0 || eight_bits == 255)
		return TEMP_FROM_REG(eight_bits);

	/* do some linear interpolation */
	return (temp_lut[eight_bits] * (4 - two_bits) +
		temp_lut[eight_bits + 1] * two_bits) * 25;
}

#define DIV_FROM_REG(val) (1 << (val))
#define DIV_TO_REG(val) ((val) == 8 ? 3 : (val) == 4 ? 2 : (val) == 1 ? 0 : 1)

/*
 * For each registered chip, we need to keep some data in memory.
 * The structure is dynamically allocated.
 */
struct via686a_data {
	unsigned short addr;
	const char *name;
	struct device *hwmon_dev;
	struct mutex update_lock;
	char valid;		/* !=0 if following fields are valid */
	unsigned long last_updated;	/* In jiffies */

	u8 in[5];		/* Register value */
	u8 in_max[5];		/* Register value */
	u8 in_min[5];		/* Register value */
	u8 fan[2];		/* Register value */
	u8 fan_min[2];		/* Register value */
	u16 temp[3];		/* Register value 10 bit */
	u8 temp_over[3];	/* Register value */
	u8 temp_hyst[3];	/* Register value */
	u8 fan_div[2];		/* Register encoding, shifted right */
	u16 alarms;		/* Register encoding, combined */
};

static struct pci_dev *s_bridge;	/* pointer to the (only) via686a */

static int via686a_probe(struct platform_device *pdev);
static int via686a_remove(struct platform_device *pdev);

static inline int via686a_read_value(struct via686a_data *data, u8 reg)
{
	return inb_p(data->addr + reg);
}

static inline void via686a_write_value(struct via686a_data *data, u8 reg,
				       u8 value)
{
	outb_p(value, data->addr + reg);
}

static struct via686a_data *via686a_update_device(struct device *dev);
static void via686a_init_device(struct via686a_data *data);

/* following are the sysfs callback functions */

/* 7 voltage sensors */
static ssize_t in_show(struct device *dev, struct device_attribute *da,
		       char *buf) {
	struct via686a_data *data = via686a_update_device(dev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	int nr = attr->index;
	return sprintf(buf, "%ld\n", IN_FROM_REG(data->in[nr], nr));
}

static ssize_t in_min_show(struct device *dev, struct device_attribute *da,
			   char *buf) {
	struct via686a_data *data = via686a_update_device(dev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	int nr = attr->index;
	return sprintf(buf, "%ld\n", IN_FROM_REG(data->in_min[nr], nr));
}

static ssize_t in_max_show(struct device *dev, struct device_attribute *da,
			   char *buf) {
	struct via686a_data *data = via686a_update_device(dev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	int nr = attr->index;
	return sprintf(buf, "%ld\n", IN_FROM_REG(data->in_max[nr], nr));
}

static ssize_t in_min_store(struct device *dev, struct device_attribute *da,
			    const char *buf, size_t count) {
	struct via686a_data *data = dev_get_drvdata(dev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	int nr = attr->index;
	unsigned long val;
	int err;

	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	data->in_min[nr] = IN_TO_REG(val, nr);
	via686a_write_value(data, VIA686A_REG_IN_MIN(nr),
			data->in_min[nr]);
	mutex_unlock(&data->update_lock);
	return count;
}
static ssize_t in_max_store(struct device *dev, struct device_attribute *da,
			    const char *buf, size_t count) {
	struct via686a_data *data = dev_get_drvdata(dev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	int nr = attr->index;
	unsigned long val;
	int err;

	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	data->in_max[nr] = IN_TO_REG(val, nr);
	via686a_write_value(data, VIA686A_REG_IN_MAX(nr),
			data->in_max[nr]);
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

/* 3 temperatures */
static ssize_t temp_show(struct device *dev, struct device_attribute *da,
			 char *buf) {
	struct via686a_data *data = via686a_update_device(dev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	int nr = attr->index;
	return sprintf(buf, "%ld\n", TEMP_FROM_REG10(data->temp[nr]));
}
static ssize_t temp_over_show(struct device *dev, struct device_attribute *da,
			      char *buf) {
	struct via686a_data *data = via686a_update_device(dev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	int nr = attr->index;
	return sprintf(buf, "%ld\n", TEMP_FROM_REG(data->temp_over[nr]));
}
static ssize_t temp_hyst_show(struct device *dev, struct device_attribute *da,
			      char *buf) {
	struct via686a_data *data = via686a_update_device(dev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	int nr = attr->index;
	return sprintf(buf, "%ld\n", TEMP_FROM_REG(data->temp_hyst[nr]));
}
static ssize_t temp_over_store(struct device *dev,
			       struct device_attribute *da, const char *buf,
			       size_t count) {
	struct via686a_data *data = dev_get_drvdata(dev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	int nr = attr->index;
	long val;
	int err;

	err = kstrtol(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	data->temp_over[nr] = TEMP_TO_REG(val);
	via686a_write_value(data, VIA686A_REG_TEMP_OVER[nr],
			    data->temp_over[nr]);
	mutex_unlock(&data->update_lock);
	return count;
}
static ssize_t temp_hyst_store(struct device *dev,
			       struct device_attribute *da, const char *buf,
			       size_t count) {
	struct via686a_data *data = dev_get_drvdata(dev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	int nr = attr->index;
	long val;
	int err;

	err = kstrtol(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	data->temp_hyst[nr] = TEMP_TO_REG(val);
	via686a_write_value(data, VIA686A_REG_TEMP_HYST[nr],
			    data->temp_hyst[nr]);
	mutex_unlock(&data->update_lock);
	return count;
}

static SENSOR_DEVICE_ATTR_RO(temp1_input, temp, 0);
static SENSOR_DEVICE_ATTR_RW(temp1_max, temp_over, 0);
static SENSOR_DEVICE_ATTR_RW(temp1_max_hyst, temp_hyst, 0);
static SENSOR_DEVICE_ATTR_RO(temp2_input, temp, 1);
static SENSOR_DEVICE_ATTR_RW(temp2_max, temp_over, 1);
static SENSOR_DEVICE_ATTR_RW(temp2_max_hyst, temp_hyst, 1);
static SENSOR_DEVICE_ATTR_RO(temp3_input, temp, 2);
static SENSOR_DEVICE_ATTR_RW(temp3_max, temp_over, 2);
static SENSOR_DEVICE_ATTR_RW(temp3_max_hyst, temp_hyst, 2);

/* 2 Fans */
static ssize_t fan_show(struct device *dev, struct device_attribute *da,
			char *buf) {
	struct via686a_data *data = via686a_update_device(dev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	int nr = attr->index;
	return sprintf(buf, "%d\n", FAN_FROM_REG(data->fan[nr],
				DIV_FROM_REG(data->fan_div[nr])));
}
static ssize_t fan_min_show(struct device *dev, struct device_attribute *da,
			    char *buf) {
	struct via686a_data *data = via686a_update_device(dev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	int nr = attr->index;
	return sprintf(buf, "%d\n",
		FAN_FROM_REG(data->fan_min[nr],
			     DIV_FROM_REG(data->fan_div[nr])));
}
static ssize_t fan_div_show(struct device *dev, struct device_attribute *da,
			    char *buf) {
	struct via686a_data *data = via686a_update_device(dev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	int nr = attr->index;
	return sprintf(buf, "%d\n", DIV_FROM_REG(data->fan_div[nr]));
}
static ssize_t fan_min_store(struct device *dev, struct device_attribute *da,
			     const char *buf, size_t count) {
	struct via686a_data *data = dev_get_drvdata(dev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	int nr = attr->index;
	unsigned long val;
	int err;

	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	data->fan_min[nr] = FAN_TO_REG(val, DIV_FROM_REG(data->fan_div[nr]));
	via686a_write_value(data, VIA686A_REG_FAN_MIN(nr+1), data->fan_min[nr]);
	mutex_unlock(&data->update_lock);
	return count;
}
static ssize_t fan_div_store(struct device *dev, struct device_attribute *da,
			     const char *buf, size_t count) {
	struct via686a_data *data = dev_get_drvdata(dev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	int nr = attr->index;
	int old;
	unsigned long val;
	int err;

	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;

	mutex_lock(&data->update_lock);
	old = via686a_read_value(data, VIA686A_REG_FANDIV);
	data->fan_div[nr] = DIV_TO_REG(val);
	old = (old & 0x0f) | (data->fan_div[1] << 6) | (data->fan_div[0] << 4);
	via686a_write_value(data, VIA686A_REG_FANDIV, old);
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
	struct via686a_data *data = via686a_update_device(dev);
	return sprintf(buf, "%u\n", data->alarms);
}

static DEVICE_ATTR_RO(alarms);

static ssize_t alarm_show(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	int bitnr = to_sensor_dev_attr(attr)->index;
	struct via686a_data *data = via686a_update_device(dev);
	return sprintf(buf, "%u\n", (data->alarms >> bitnr) & 1);
}
static SENSOR_DEVICE_ATTR_RO(in0_alarm, alarm, 0);
static SENSOR_DEVICE_ATTR_RO(in1_alarm, alarm, 1);
static SENSOR_DEVICE_ATTR_RO(in2_alarm, alarm, 2);
static SENSOR_DEVICE_ATTR_RO(in3_alarm, alarm, 3);
static SENSOR_DEVICE_ATTR_RO(in4_alarm, alarm, 8);
static SENSOR_DEVICE_ATTR_RO(temp1_alarm, alarm, 4);
static SENSOR_DEVICE_ATTR_RO(temp2_alarm, alarm, 11);
static SENSOR_DEVICE_ATTR_RO(temp3_alarm, alarm, 15);
static SENSOR_DEVICE_ATTR_RO(fan1_alarm, alarm, 6);
static SENSOR_DEVICE_ATTR_RO(fan2_alarm, alarm, 7);

static ssize_t name_show(struct device *dev, struct device_attribute
			 *devattr, char *buf)
{
	struct via686a_data *data = dev_get_drvdata(dev);
	return sprintf(buf, "%s\n", data->name);
}
static DEVICE_ATTR_RO(name);

static struct attribute *via686a_attributes[] = {
	&sensor_dev_attr_in0_input.dev_attr.attr,
	&sensor_dev_attr_in1_input.dev_attr.attr,
	&sensor_dev_attr_in2_input.dev_attr.attr,
	&sensor_dev_attr_in3_input.dev_attr.attr,
	&sensor_dev_attr_in4_input.dev_attr.attr,
	&sensor_dev_attr_in0_min.dev_attr.attr,
	&sensor_dev_attr_in1_min.dev_attr.attr,
	&sensor_dev_attr_in2_min.dev_attr.attr,
	&sensor_dev_attr_in3_min.dev_attr.attr,
	&sensor_dev_attr_in4_min.dev_attr.attr,
	&sensor_dev_attr_in0_max.dev_attr.attr,
	&sensor_dev_attr_in1_max.dev_attr.attr,
	&sensor_dev_attr_in2_max.dev_attr.attr,
	&sensor_dev_attr_in3_max.dev_attr.attr,
	&sensor_dev_attr_in4_max.dev_attr.attr,
	&sensor_dev_attr_in0_alarm.dev_attr.attr,
	&sensor_dev_attr_in1_alarm.dev_attr.attr,
	&sensor_dev_attr_in2_alarm.dev_attr.attr,
	&sensor_dev_attr_in3_alarm.dev_attr.attr,
	&sensor_dev_attr_in4_alarm.dev_attr.attr,

	&sensor_dev_attr_temp1_input.dev_attr.attr,
	&sensor_dev_attr_temp2_input.dev_attr.attr,
	&sensor_dev_attr_temp3_input.dev_attr.attr,
	&sensor_dev_attr_temp1_max.dev_attr.attr,
	&sensor_dev_attr_temp2_max.dev_attr.attr,
	&sensor_dev_attr_temp3_max.dev_attr.attr,
	&sensor_dev_attr_temp1_max_hyst.dev_attr.attr,
	&sensor_dev_attr_temp2_max_hyst.dev_attr.attr,
	&sensor_dev_attr_temp3_max_hyst.dev_attr.attr,
	&sensor_dev_attr_temp1_alarm.dev_attr.attr,
	&sensor_dev_attr_temp2_alarm.dev_attr.attr,
	&sensor_dev_attr_temp3_alarm.dev_attr.attr,

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

static const struct attribute_group via686a_group = {
	.attrs = via686a_attributes,
};

static struct platform_driver via686a_driver = {
	.driver = {
		.name	= "via686a",
	},
	.probe		= via686a_probe,
	.remove		= via686a_remove,
};

/* This is called when the module is loaded */
static int via686a_probe(struct platform_device *pdev)
{
	struct via686a_data *data;
	struct resource *res;
	int err;

	/* Reserve the ISA region */
	res = platform_get_resource(pdev, IORESOURCE_IO, 0);
	if (!devm_request_region(&pdev->dev, res->start, VIA686A_EXTENT,
				 via686a_driver.driver.name)) {
		dev_err(&pdev->dev, "Region 0x%lx-0x%lx already in use!\n",
			(unsigned long)res->start, (unsigned long)res->end);
		return -ENODEV;
	}

	data = devm_kzalloc(&pdev->dev, sizeof(struct via686a_data),
			    GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	platform_set_drvdata(pdev, data);
	data->addr = res->start;
	data->name = "via686a";
	mutex_init(&data->update_lock);

	/* Initialize the VIA686A chip */
	via686a_init_device(data);

	/* Register sysfs hooks */
	err = sysfs_create_group(&pdev->dev.kobj, &via686a_group);
	if (err)
		return err;

	data->hwmon_dev = hwmon_device_register(&pdev->dev);
	if (IS_ERR(data->hwmon_dev)) {
		err = PTR_ERR(data->hwmon_dev);
		goto exit_remove_files;
	}

	return 0;

exit_remove_files:
	sysfs_remove_group(&pdev->dev.kobj, &via686a_group);
	return err;
}

static int via686a_remove(struct platform_device *pdev)
{
	struct via686a_data *data = platform_get_drvdata(pdev);

	hwmon_device_unregister(data->hwmon_dev);
	sysfs_remove_group(&pdev->dev.kobj, &via686a_group);

	return 0;
}

static void via686a_update_fan_div(struct via686a_data *data)
{
	int reg = via686a_read_value(data, VIA686A_REG_FANDIV);
	data->fan_div[0] = (reg >> 4) & 0x03;
	data->fan_div[1] = reg >> 6;
}

static void via686a_init_device(struct via686a_data *data)
{
	u8 reg;

	/* Start monitoring */
	reg = via686a_read_value(data, VIA686A_REG_CONFIG);
	via686a_write_value(data, VIA686A_REG_CONFIG, (reg | 0x01) & 0x7F);

	/* Configure temp interrupt mode for continuous-interrupt operation */
	reg = via686a_read_value(data, VIA686A_REG_TEMP_MODE);
	via686a_write_value(data, VIA686A_REG_TEMP_MODE,
			    (reg & ~VIA686A_TEMP_MODE_MASK)
			    | VIA686A_TEMP_MODE_CONTINUOUS);

	/* Pre-read fan clock divisor values */
	via686a_update_fan_div(data);
}

static struct via686a_data *via686a_update_device(struct device *dev)
{
	struct via686a_data *data = dev_get_drvdata(dev);
	int i;

	mutex_lock(&data->update_lock);

	if (time_after(jiffies, data->last_updated + HZ + HZ / 2)
	    || !data->valid) {
		for (i = 0; i <= 4; i++) {
			data->in[i] =
			    via686a_read_value(data, VIA686A_REG_IN(i));
			data->in_min[i] = via686a_read_value(data,
							     VIA686A_REG_IN_MIN
							     (i));
			data->in_max[i] =
			    via686a_read_value(data, VIA686A_REG_IN_MAX(i));
		}
		for (i = 1; i <= 2; i++) {
			data->fan[i - 1] =
			    via686a_read_value(data, VIA686A_REG_FAN(i));
			data->fan_min[i - 1] = via686a_read_value(data,
						     VIA686A_REG_FAN_MIN(i));
		}
		for (i = 0; i <= 2; i++) {
			data->temp[i] = via686a_read_value(data,
						 VIA686A_REG_TEMP[i]) << 2;
			data->temp_over[i] =
			    via686a_read_value(data,
					       VIA686A_REG_TEMP_OVER[i]);
			data->temp_hyst[i] =
			    via686a_read_value(data,
					       VIA686A_REG_TEMP_HYST[i]);
		}
		/*
		 * add in lower 2 bits
		 * temp1 uses bits 7-6 of VIA686A_REG_TEMP_LOW1
		 * temp2 uses bits 5-4 of VIA686A_REG_TEMP_LOW23
		 * temp3 uses bits 7-6 of VIA686A_REG_TEMP_LOW23
		 */
		data->temp[0] |= (via686a_read_value(data,
						     VIA686A_REG_TEMP_LOW1)
				  & 0xc0) >> 6;
		data->temp[1] |=
		    (via686a_read_value(data, VIA686A_REG_TEMP_LOW23) &
		     0x30) >> 4;
		data->temp[2] |=
		    (via686a_read_value(data, VIA686A_REG_TEMP_LOW23) &
		     0xc0) >> 6;

		via686a_update_fan_div(data);
		data->alarms =
		    via686a_read_value(data,
				       VIA686A_REG_ALARM1) |
		    (via686a_read_value(data, VIA686A_REG_ALARM2) << 8);
		data->last_updated = jiffies;
		data->valid = 1;
	}

	mutex_unlock(&data->update_lock);

	return data;
}

static const struct pci_device_id via686a_pci_ids[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_82C686_4) },
	{ }
};
MODULE_DEVICE_TABLE(pci, via686a_pci_ids);

static int via686a_device_add(unsigned short address)
{
	struct resource res = {
		.start	= address,
		.end	= address + VIA686A_EXTENT - 1,
		.name	= "via686a",
		.flags	= IORESOURCE_IO,
	};
	int err;

	err = acpi_check_resource_conflict(&res);
	if (err)
		goto exit;

	pdev = platform_device_alloc("via686a", address);
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

static int via686a_pci_probe(struct pci_dev *dev,
				       const struct pci_device_id *id)
{
	u16 address, val;

	if (force_addr) {
		address = force_addr & ~(VIA686A_EXTENT - 1);
		dev_warn(&dev->dev, "Forcing ISA address 0x%x\n", address);
		if (PCIBIOS_SUCCESSFUL !=
		    pci_write_config_word(dev, VIA686A_BASE_REG, address | 1))
			return -ENODEV;
	}
	if (PCIBIOS_SUCCESSFUL !=
	    pci_read_config_word(dev, VIA686A_BASE_REG, &val))
		return -ENODEV;

	address = val & ~(VIA686A_EXTENT - 1);
	if (address == 0) {
		dev_err(&dev->dev,
			"base address not set - upgrade BIOS or use force_addr=0xaddr\n");
		return -ENODEV;
	}

	if (PCIBIOS_SUCCESSFUL !=
	    pci_read_config_word(dev, VIA686A_ENABLE_REG, &val))
		return -ENODEV;
	if (!(val & 0x0001)) {
		if (!force_addr) {
			dev_warn(&dev->dev,
				 "Sensors disabled, enable with force_addr=0x%x\n",
				 address);
			return -ENODEV;
		}

		dev_warn(&dev->dev, "Enabling sensors\n");
		if (PCIBIOS_SUCCESSFUL !=
		    pci_write_config_word(dev, VIA686A_ENABLE_REG,
					  val | 0x0001))
			return -ENODEV;
	}

	if (platform_driver_register(&via686a_driver))
		goto exit;

	/* Sets global pdev as a side effect */
	if (via686a_device_add(address))
		goto exit_unregister;

	/*
	 * Always return failure here.  This is to allow other drivers to bind
	 * to this pci device.  We don't really want to have control over the
	 * pci device, we only wanted to read as few register values from it.
	 */
	s_bridge = pci_dev_get(dev);
	return -ENODEV;

exit_unregister:
	platform_driver_unregister(&via686a_driver);
exit:
	return -ENODEV;
}

static struct pci_driver via686a_pci_driver = {
	.name		= "via686a",
	.id_table	= via686a_pci_ids,
	.probe		= via686a_pci_probe,
};

static int __init sm_via686a_init(void)
{
	return pci_register_driver(&via686a_pci_driver);
}

static void __exit sm_via686a_exit(void)
{
	pci_unregister_driver(&via686a_pci_driver);
	if (s_bridge != NULL) {
		platform_device_unregister(pdev);
		platform_driver_unregister(&via686a_driver);
		pci_dev_put(s_bridge);
		s_bridge = NULL;
	}
}

MODULE_AUTHOR("Kyösti Mälkki <kmalkki@cc.hut.fi>, "
	      "Mark Studebaker <mdsxyz123@yahoo.com> "
	      "and Bob Dougherty <bobd@stanford.edu>");
MODULE_DESCRIPTION("VIA 686A Sensor device");
MODULE_LICENSE("GPL");

module_init(sm_via686a_init);
module_exit(sm_via686a_exit);
