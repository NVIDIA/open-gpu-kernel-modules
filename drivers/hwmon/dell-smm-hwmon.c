// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * dell-smm-hwmon.c -- Linux driver for accessing the SMM BIOS on Dell laptops.
 *
 * Copyright (C) 2001  Massimo Dal Zotto <dz@debian.org>
 *
 * Hwmon integration:
 * Copyright (C) 2011  Jean Delvare <jdelvare@suse.de>
 * Copyright (C) 2013, 2014  Guenter Roeck <linux@roeck-us.net>
 * Copyright (C) 2014, 2015  Pali Rohár <pali@kernel.org>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/cpu.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/dmi.h>
#include <linux/capability.h>
#include <linux/mutex.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/sched.h>
#include <linux/ctype.h>
#include <linux/smp.h>

#include <linux/i8k.h>

#define I8K_SMM_FN_STATUS	0x0025
#define I8K_SMM_POWER_STATUS	0x0069
#define I8K_SMM_SET_FAN		0x01a3
#define I8K_SMM_GET_FAN		0x00a3
#define I8K_SMM_GET_SPEED	0x02a3
#define I8K_SMM_GET_FAN_TYPE	0x03a3
#define I8K_SMM_GET_NOM_SPEED	0x04a3
#define I8K_SMM_GET_TEMP	0x10a3
#define I8K_SMM_GET_TEMP_TYPE	0x11a3
#define I8K_SMM_GET_DELL_SIG1	0xfea3
#define I8K_SMM_GET_DELL_SIG2	0xffa3

#define I8K_FAN_MULT		30
#define I8K_FAN_MAX_RPM		30000
#define I8K_MAX_TEMP		127

#define I8K_FN_NONE		0x00
#define I8K_FN_UP		0x01
#define I8K_FN_DOWN		0x02
#define I8K_FN_MUTE		0x04
#define I8K_FN_MASK		0x07
#define I8K_FN_SHIFT		8

#define I8K_POWER_AC		0x05
#define I8K_POWER_BATTERY	0x01

static DEFINE_MUTEX(i8k_mutex);
static char bios_version[4];
static char bios_machineid[16];
static struct device *i8k_hwmon_dev;
static u32 i8k_hwmon_flags;
static uint i8k_fan_mult = I8K_FAN_MULT;
static uint i8k_pwm_mult;
static uint i8k_fan_max = I8K_FAN_HIGH;
static bool disallow_fan_type_call;
static bool disallow_fan_support;
static unsigned int manual_fan;
static unsigned int auto_fan;

#define I8K_HWMON_HAVE_TEMP1	(1 << 0)
#define I8K_HWMON_HAVE_TEMP2	(1 << 1)
#define I8K_HWMON_HAVE_TEMP3	(1 << 2)
#define I8K_HWMON_HAVE_TEMP4	(1 << 3)
#define I8K_HWMON_HAVE_TEMP5	(1 << 4)
#define I8K_HWMON_HAVE_TEMP6	(1 << 5)
#define I8K_HWMON_HAVE_TEMP7	(1 << 6)
#define I8K_HWMON_HAVE_TEMP8	(1 << 7)
#define I8K_HWMON_HAVE_TEMP9	(1 << 8)
#define I8K_HWMON_HAVE_TEMP10	(1 << 9)
#define I8K_HWMON_HAVE_FAN1	(1 << 10)
#define I8K_HWMON_HAVE_FAN2	(1 << 11)
#define I8K_HWMON_HAVE_FAN3	(1 << 12)

MODULE_AUTHOR("Massimo Dal Zotto (dz@debian.org)");
MODULE_AUTHOR("Pali Rohár <pali@kernel.org>");
MODULE_DESCRIPTION("Dell laptop SMM BIOS hwmon driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("i8k");

static bool force;
module_param(force, bool, 0);
MODULE_PARM_DESC(force, "Force loading without checking for supported models");

static bool ignore_dmi;
module_param(ignore_dmi, bool, 0);
MODULE_PARM_DESC(ignore_dmi, "Continue probing hardware even if DMI data does not match");

#if IS_ENABLED(CONFIG_I8K)
static bool restricted = true;
module_param(restricted, bool, 0);
MODULE_PARM_DESC(restricted, "Restrict fan control and serial number to CAP_SYS_ADMIN (default: 1)");

static bool power_status;
module_param(power_status, bool, 0600);
MODULE_PARM_DESC(power_status, "Report power status in /proc/i8k (default: 0)");
#endif

static uint fan_mult;
module_param(fan_mult, uint, 0);
MODULE_PARM_DESC(fan_mult, "Factor to multiply fan speed with (default: autodetect)");

static uint fan_max;
module_param(fan_max, uint, 0);
MODULE_PARM_DESC(fan_max, "Maximum configurable fan speed (default: autodetect)");

struct smm_regs {
	unsigned int eax;
	unsigned int ebx __packed;
	unsigned int ecx __packed;
	unsigned int edx __packed;
	unsigned int esi __packed;
	unsigned int edi __packed;
};

static inline const char *i8k_get_dmi_data(int field)
{
	const char *dmi_data = dmi_get_system_info(field);

	return dmi_data && *dmi_data ? dmi_data : "?";
}

/*
 * Call the System Management Mode BIOS. Code provided by Jonathan Buzzard.
 */
static int i8k_smm_func(void *par)
{
	int rc;
	struct smm_regs *regs = par;
	int eax = regs->eax;

#ifdef DEBUG
	int ebx = regs->ebx;
	unsigned long duration;
	ktime_t calltime, delta, rettime;

	calltime = ktime_get();
#endif

	/* SMM requires CPU 0 */
	if (smp_processor_id() != 0)
		return -EBUSY;

#if defined(CONFIG_X86_64)
	asm volatile("pushq %%rax\n\t"
		"movl 0(%%rax),%%edx\n\t"
		"pushq %%rdx\n\t"
		"movl 4(%%rax),%%ebx\n\t"
		"movl 8(%%rax),%%ecx\n\t"
		"movl 12(%%rax),%%edx\n\t"
		"movl 16(%%rax),%%esi\n\t"
		"movl 20(%%rax),%%edi\n\t"
		"popq %%rax\n\t"
		"out %%al,$0xb2\n\t"
		"out %%al,$0x84\n\t"
		"xchgq %%rax,(%%rsp)\n\t"
		"movl %%ebx,4(%%rax)\n\t"
		"movl %%ecx,8(%%rax)\n\t"
		"movl %%edx,12(%%rax)\n\t"
		"movl %%esi,16(%%rax)\n\t"
		"movl %%edi,20(%%rax)\n\t"
		"popq %%rdx\n\t"
		"movl %%edx,0(%%rax)\n\t"
		"pushfq\n\t"
		"popq %%rax\n\t"
		"andl $1,%%eax\n"
		: "=a"(rc)
		:    "a"(regs)
		:    "%ebx", "%ecx", "%edx", "%esi", "%edi", "memory");
#else
	asm volatile("pushl %%eax\n\t"
	    "movl 0(%%eax),%%edx\n\t"
	    "push %%edx\n\t"
	    "movl 4(%%eax),%%ebx\n\t"
	    "movl 8(%%eax),%%ecx\n\t"
	    "movl 12(%%eax),%%edx\n\t"
	    "movl 16(%%eax),%%esi\n\t"
	    "movl 20(%%eax),%%edi\n\t"
	    "popl %%eax\n\t"
	    "out %%al,$0xb2\n\t"
	    "out %%al,$0x84\n\t"
	    "xchgl %%eax,(%%esp)\n\t"
	    "movl %%ebx,4(%%eax)\n\t"
	    "movl %%ecx,8(%%eax)\n\t"
	    "movl %%edx,12(%%eax)\n\t"
	    "movl %%esi,16(%%eax)\n\t"
	    "movl %%edi,20(%%eax)\n\t"
	    "popl %%edx\n\t"
	    "movl %%edx,0(%%eax)\n\t"
	    "lahf\n\t"
	    "shrl $8,%%eax\n\t"
	    "andl $1,%%eax\n"
	    : "=a"(rc)
	    :    "a"(regs)
	    :    "%ebx", "%ecx", "%edx", "%esi", "%edi", "memory");
#endif
	if (rc != 0 || (regs->eax & 0xffff) == 0xffff || regs->eax == eax)
		rc = -EINVAL;

#ifdef DEBUG
	rettime = ktime_get();
	delta = ktime_sub(rettime, calltime);
	duration = ktime_to_ns(delta) >> 10;
	pr_debug("smm(0x%.4x 0x%.4x) = 0x%.4x  (took %7lu usecs)\n", eax, ebx,
		(rc ? 0xffff : regs->eax & 0xffff), duration);
#endif

	return rc;
}

/*
 * Call the System Management Mode BIOS.
 */
static int i8k_smm(struct smm_regs *regs)
{
	int ret;

	get_online_cpus();
	ret = smp_call_on_cpu(0, i8k_smm_func, regs, true);
	put_online_cpus();

	return ret;
}

/*
 * Read the fan status.
 */
static int i8k_get_fan_status(int fan)
{
	struct smm_regs regs = { .eax = I8K_SMM_GET_FAN, };

	if (disallow_fan_support)
		return -EINVAL;

	regs.ebx = fan & 0xff;
	return i8k_smm(&regs) ? : regs.eax & 0xff;
}

/*
 * Read the fan speed in RPM.
 */
static int i8k_get_fan_speed(int fan)
{
	struct smm_regs regs = { .eax = I8K_SMM_GET_SPEED, };

	if (disallow_fan_support)
		return -EINVAL;

	regs.ebx = fan & 0xff;
	return i8k_smm(&regs) ? : (regs.eax & 0xffff) * i8k_fan_mult;
}

/*
 * Read the fan type.
 */
static int _i8k_get_fan_type(int fan)
{
	struct smm_regs regs = { .eax = I8K_SMM_GET_FAN_TYPE, };

	if (disallow_fan_support || disallow_fan_type_call)
		return -EINVAL;

	regs.ebx = fan & 0xff;
	return i8k_smm(&regs) ? : regs.eax & 0xff;
}

static int i8k_get_fan_type(int fan)
{
	/* I8K_SMM_GET_FAN_TYPE SMM call is expensive, so cache values */
	static int types[3] = { INT_MIN, INT_MIN, INT_MIN };

	if (types[fan] == INT_MIN)
		types[fan] = _i8k_get_fan_type(fan);

	return types[fan];
}

/*
 * Read the fan nominal rpm for specific fan speed.
 */
static int i8k_get_fan_nominal_speed(int fan, int speed)
{
	struct smm_regs regs = { .eax = I8K_SMM_GET_NOM_SPEED, };

	if (disallow_fan_support)
		return -EINVAL;

	regs.ebx = (fan & 0xff) | (speed << 8);
	return i8k_smm(&regs) ? : (regs.eax & 0xffff) * i8k_fan_mult;
}

/*
 * Enable or disable automatic BIOS fan control support
 */
static int i8k_enable_fan_auto_mode(bool enable)
{
	struct smm_regs regs = { };

	if (disallow_fan_support)
		return -EINVAL;

	regs.eax = enable ? auto_fan : manual_fan;
	return i8k_smm(&regs);
}

/*
 * Set the fan speed (off, low, high). Returns the new fan status.
 */
static int i8k_set_fan(int fan, int speed)
{
	struct smm_regs regs = { .eax = I8K_SMM_SET_FAN, };

	if (disallow_fan_support)
		return -EINVAL;

	speed = (speed < 0) ? 0 : ((speed > i8k_fan_max) ? i8k_fan_max : speed);
	regs.ebx = (fan & 0xff) | (speed << 8);

	return i8k_smm(&regs) ? : i8k_get_fan_status(fan);
}

static int i8k_get_temp_type(int sensor)
{
	struct smm_regs regs = { .eax = I8K_SMM_GET_TEMP_TYPE, };

	regs.ebx = sensor & 0xff;
	return i8k_smm(&regs) ? : regs.eax & 0xff;
}

/*
 * Read the cpu temperature.
 */
static int _i8k_get_temp(int sensor)
{
	struct smm_regs regs = {
		.eax = I8K_SMM_GET_TEMP,
		.ebx = sensor & 0xff,
	};

	return i8k_smm(&regs) ? : regs.eax & 0xff;
}

static int i8k_get_temp(int sensor)
{
	int temp = _i8k_get_temp(sensor);

	/*
	 * Sometimes the temperature sensor returns 0x99, which is out of range.
	 * In this case we retry (once) before returning an error.
	 # 1003655137 00000058 00005a4b
	 # 1003655138 00000099 00003a80 <--- 0x99 = 153 degrees
	 # 1003655139 00000054 00005c52
	 */
	if (temp == 0x99) {
		msleep(100);
		temp = _i8k_get_temp(sensor);
	}
	/*
	 * Return -ENODATA for all invalid temperatures.
	 *
	 * Known instances are the 0x99 value as seen above as well as
	 * 0xc1 (193), which may be returned when trying to read the GPU
	 * temperature if the system supports a GPU and it is currently
	 * turned off.
	 */
	if (temp > I8K_MAX_TEMP)
		return -ENODATA;

	return temp;
}

static int i8k_get_dell_signature(int req_fn)
{
	struct smm_regs regs = { .eax = req_fn, };
	int rc;

	rc = i8k_smm(&regs);
	if (rc < 0)
		return rc;

	return regs.eax == 1145651527 && regs.edx == 1145392204 ? 0 : -1;
}

#if IS_ENABLED(CONFIG_I8K)

/*
 * Read the Fn key status.
 */
static int i8k_get_fn_status(void)
{
	struct smm_regs regs = { .eax = I8K_SMM_FN_STATUS, };
	int rc;

	rc = i8k_smm(&regs);
	if (rc < 0)
		return rc;

	switch ((regs.eax >> I8K_FN_SHIFT) & I8K_FN_MASK) {
	case I8K_FN_UP:
		return I8K_VOL_UP;
	case I8K_FN_DOWN:
		return I8K_VOL_DOWN;
	case I8K_FN_MUTE:
		return I8K_VOL_MUTE;
	default:
		return 0;
	}
}

/*
 * Read the power status.
 */
static int i8k_get_power_status(void)
{
	struct smm_regs regs = { .eax = I8K_SMM_POWER_STATUS, };
	int rc;

	rc = i8k_smm(&regs);
	if (rc < 0)
		return rc;

	return (regs.eax & 0xff) == I8K_POWER_AC ? I8K_AC : I8K_BATTERY;
}

/*
 * Procfs interface
 */

static int
i8k_ioctl_unlocked(struct file *fp, unsigned int cmd, unsigned long arg)
{
	int val = 0;
	int speed;
	unsigned char buff[16];
	int __user *argp = (int __user *)arg;

	if (!argp)
		return -EINVAL;

	switch (cmd) {
	case I8K_BIOS_VERSION:
		if (!isdigit(bios_version[0]) || !isdigit(bios_version[1]) ||
		    !isdigit(bios_version[2]))
			return -EINVAL;

		val = (bios_version[0] << 16) |
				(bios_version[1] << 8) | bios_version[2];
		break;

	case I8K_MACHINE_ID:
		if (restricted && !capable(CAP_SYS_ADMIN))
			return -EPERM;

		memset(buff, 0, sizeof(buff));
		strlcpy(buff, bios_machineid, sizeof(buff));
		break;

	case I8K_FN_STATUS:
		val = i8k_get_fn_status();
		break;

	case I8K_POWER_STATUS:
		val = i8k_get_power_status();
		break;

	case I8K_GET_TEMP:
		val = i8k_get_temp(0);
		break;

	case I8K_GET_SPEED:
		if (copy_from_user(&val, argp, sizeof(int)))
			return -EFAULT;

		val = i8k_get_fan_speed(val);
		break;

	case I8K_GET_FAN:
		if (copy_from_user(&val, argp, sizeof(int)))
			return -EFAULT;

		val = i8k_get_fan_status(val);
		break;

	case I8K_SET_FAN:
		if (restricted && !capable(CAP_SYS_ADMIN))
			return -EPERM;

		if (copy_from_user(&val, argp, sizeof(int)))
			return -EFAULT;

		if (copy_from_user(&speed, argp + 1, sizeof(int)))
			return -EFAULT;

		val = i8k_set_fan(val, speed);
		break;

	default:
		return -EINVAL;
	}

	if (val < 0)
		return val;

	switch (cmd) {
	case I8K_BIOS_VERSION:
		if (copy_to_user(argp, &val, 4))
			return -EFAULT;

		break;
	case I8K_MACHINE_ID:
		if (copy_to_user(argp, buff, 16))
			return -EFAULT;

		break;
	default:
		if (copy_to_user(argp, &val, sizeof(int)))
			return -EFAULT;

		break;
	}

	return 0;
}

static long i8k_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	long ret;

	mutex_lock(&i8k_mutex);
	ret = i8k_ioctl_unlocked(fp, cmd, arg);
	mutex_unlock(&i8k_mutex);

	return ret;
}

/*
 * Print the information for /proc/i8k.
 */
static int i8k_proc_show(struct seq_file *seq, void *offset)
{
	int fn_key, cpu_temp, ac_power;
	int left_fan, right_fan, left_speed, right_speed;

	cpu_temp	= i8k_get_temp(0);			/* 11100 µs */
	left_fan	= i8k_get_fan_status(I8K_FAN_LEFT);	/*   580 µs */
	right_fan	= i8k_get_fan_status(I8K_FAN_RIGHT);	/*   580 µs */
	left_speed	= i8k_get_fan_speed(I8K_FAN_LEFT);	/*   580 µs */
	right_speed	= i8k_get_fan_speed(I8K_FAN_RIGHT);	/*   580 µs */
	fn_key		= i8k_get_fn_status();			/*   750 µs */
	if (power_status)
		ac_power = i8k_get_power_status();		/* 14700 µs */
	else
		ac_power = -1;

	/*
	 * Info:
	 *
	 * 1)  Format version (this will change if format changes)
	 * 2)  BIOS version
	 * 3)  BIOS machine ID
	 * 4)  Cpu temperature
	 * 5)  Left fan status
	 * 6)  Right fan status
	 * 7)  Left fan speed
	 * 8)  Right fan speed
	 * 9)  AC power
	 * 10) Fn Key status
	 */
	seq_printf(seq, "%s %s %s %d %d %d %d %d %d %d\n",
		   I8K_PROC_FMT,
		   bios_version,
		   (restricted && !capable(CAP_SYS_ADMIN)) ? "-1" : bios_machineid,
		   cpu_temp,
		   left_fan, right_fan, left_speed, right_speed,
		   ac_power, fn_key);

	return 0;
}

static int i8k_open_fs(struct inode *inode, struct file *file)
{
	return single_open(file, i8k_proc_show, NULL);
}

static const struct proc_ops i8k_proc_ops = {
	.proc_open	= i8k_open_fs,
	.proc_read	= seq_read,
	.proc_lseek	= seq_lseek,
	.proc_release	= single_release,
	.proc_ioctl	= i8k_ioctl,
};

static void __init i8k_init_procfs(void)
{
	/* Register the proc entry */
	proc_create("i8k", 0, NULL, &i8k_proc_ops);
}

static void __exit i8k_exit_procfs(void)
{
	remove_proc_entry("i8k", NULL);
}

#else

static inline void __init i8k_init_procfs(void)
{
}

static inline void __exit i8k_exit_procfs(void)
{
}

#endif

/*
 * Hwmon interface
 */

static ssize_t i8k_hwmon_temp_label_show(struct device *dev,
					 struct device_attribute *devattr,
					 char *buf)
{
	static const char * const labels[] = {
		"CPU",
		"GPU",
		"SODIMM",
		"Other",
		"Ambient",
		"Other",
	};
	int index = to_sensor_dev_attr(devattr)->index;
	int type;

	type = i8k_get_temp_type(index);
	if (type < 0)
		return type;
	if (type >= ARRAY_SIZE(labels))
		type = ARRAY_SIZE(labels) - 1;
	return sprintf(buf, "%s\n", labels[type]);
}

static ssize_t i8k_hwmon_temp_show(struct device *dev,
				   struct device_attribute *devattr,
				   char *buf)
{
	int index = to_sensor_dev_attr(devattr)->index;
	int temp;

	temp = i8k_get_temp(index);
	if (temp < 0)
		return temp;
	return sprintf(buf, "%d\n", temp * 1000);
}

static ssize_t i8k_hwmon_fan_label_show(struct device *dev,
					struct device_attribute *devattr,
					char *buf)
{
	static const char * const labels[] = {
		"Processor Fan",
		"Motherboard Fan",
		"Video Fan",
		"Power Supply Fan",
		"Chipset Fan",
		"Other Fan",
	};
	int index = to_sensor_dev_attr(devattr)->index;
	bool dock = false;
	int type;

	type = i8k_get_fan_type(index);
	if (type < 0)
		return type;

	if (type & 0x10) {
		dock = true;
		type &= 0x0F;
	}

	if (type >= ARRAY_SIZE(labels))
		type = (ARRAY_SIZE(labels) - 1);

	return sprintf(buf, "%s%s\n", (dock ? "Docking " : ""), labels[type]);
}

static ssize_t i8k_hwmon_fan_show(struct device *dev,
				  struct device_attribute *devattr, char *buf)
{
	int index = to_sensor_dev_attr(devattr)->index;
	int fan_speed;

	fan_speed = i8k_get_fan_speed(index);
	if (fan_speed < 0)
		return fan_speed;
	return sprintf(buf, "%d\n", fan_speed);
}

static ssize_t i8k_hwmon_pwm_show(struct device *dev,
				  struct device_attribute *devattr, char *buf)
{
	int index = to_sensor_dev_attr(devattr)->index;
	int status;

	status = i8k_get_fan_status(index);
	if (status < 0)
		return -EIO;
	return sprintf(buf, "%d\n", clamp_val(status * i8k_pwm_mult, 0, 255));
}

static ssize_t i8k_hwmon_pwm_store(struct device *dev,
				   struct device_attribute *attr,
				   const char *buf, size_t count)
{
	int index = to_sensor_dev_attr(attr)->index;
	unsigned long val;
	int err;

	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;
	val = clamp_val(DIV_ROUND_CLOSEST(val, i8k_pwm_mult), 0, i8k_fan_max);

	mutex_lock(&i8k_mutex);
	err = i8k_set_fan(index, val);
	mutex_unlock(&i8k_mutex);

	return err < 0 ? -EIO : count;
}

static ssize_t i8k_hwmon_pwm_enable_store(struct device *dev,
					  struct device_attribute *attr,
					  const char *buf, size_t count)
{
	int err;
	bool enable;
	unsigned long val;

	if (!auto_fan)
		return -ENODEV;

	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;

	if (val == 1)
		enable = false;
	else if (val == 2)
		enable = true;
	else
		return -EINVAL;

	mutex_lock(&i8k_mutex);
	err = i8k_enable_fan_auto_mode(enable);
	mutex_unlock(&i8k_mutex);

	return err ? err : count;
}

static SENSOR_DEVICE_ATTR_RO(temp1_input, i8k_hwmon_temp, 0);
static SENSOR_DEVICE_ATTR_RO(temp1_label, i8k_hwmon_temp_label, 0);
static SENSOR_DEVICE_ATTR_RO(temp2_input, i8k_hwmon_temp, 1);
static SENSOR_DEVICE_ATTR_RO(temp2_label, i8k_hwmon_temp_label, 1);
static SENSOR_DEVICE_ATTR_RO(temp3_input, i8k_hwmon_temp, 2);
static SENSOR_DEVICE_ATTR_RO(temp3_label, i8k_hwmon_temp_label, 2);
static SENSOR_DEVICE_ATTR_RO(temp4_input, i8k_hwmon_temp, 3);
static SENSOR_DEVICE_ATTR_RO(temp4_label, i8k_hwmon_temp_label, 3);
static SENSOR_DEVICE_ATTR_RO(temp5_input, i8k_hwmon_temp, 4);
static SENSOR_DEVICE_ATTR_RO(temp5_label, i8k_hwmon_temp_label, 4);
static SENSOR_DEVICE_ATTR_RO(temp6_input, i8k_hwmon_temp, 5);
static SENSOR_DEVICE_ATTR_RO(temp6_label, i8k_hwmon_temp_label, 5);
static SENSOR_DEVICE_ATTR_RO(temp7_input, i8k_hwmon_temp, 6);
static SENSOR_DEVICE_ATTR_RO(temp7_label, i8k_hwmon_temp_label, 6);
static SENSOR_DEVICE_ATTR_RO(temp8_input, i8k_hwmon_temp, 7);
static SENSOR_DEVICE_ATTR_RO(temp8_label, i8k_hwmon_temp_label, 7);
static SENSOR_DEVICE_ATTR_RO(temp9_input, i8k_hwmon_temp, 8);
static SENSOR_DEVICE_ATTR_RO(temp9_label, i8k_hwmon_temp_label, 8);
static SENSOR_DEVICE_ATTR_RO(temp10_input, i8k_hwmon_temp, 9);
static SENSOR_DEVICE_ATTR_RO(temp10_label, i8k_hwmon_temp_label, 9);
static SENSOR_DEVICE_ATTR_RO(fan1_input, i8k_hwmon_fan, 0);
static SENSOR_DEVICE_ATTR_RO(fan1_label, i8k_hwmon_fan_label, 0);
static SENSOR_DEVICE_ATTR_RW(pwm1, i8k_hwmon_pwm, 0);
static SENSOR_DEVICE_ATTR_WO(pwm1_enable, i8k_hwmon_pwm_enable, 0);
static SENSOR_DEVICE_ATTR_RO(fan2_input, i8k_hwmon_fan, 1);
static SENSOR_DEVICE_ATTR_RO(fan2_label, i8k_hwmon_fan_label, 1);
static SENSOR_DEVICE_ATTR_RW(pwm2, i8k_hwmon_pwm, 1);
static SENSOR_DEVICE_ATTR_RO(fan3_input, i8k_hwmon_fan, 2);
static SENSOR_DEVICE_ATTR_RO(fan3_label, i8k_hwmon_fan_label, 2);
static SENSOR_DEVICE_ATTR_RW(pwm3, i8k_hwmon_pwm, 2);

static struct attribute *i8k_attrs[] = {
	&sensor_dev_attr_temp1_input.dev_attr.attr,	/* 0 */
	&sensor_dev_attr_temp1_label.dev_attr.attr,	/* 1 */
	&sensor_dev_attr_temp2_input.dev_attr.attr,	/* 2 */
	&sensor_dev_attr_temp2_label.dev_attr.attr,	/* 3 */
	&sensor_dev_attr_temp3_input.dev_attr.attr,	/* 4 */
	&sensor_dev_attr_temp3_label.dev_attr.attr,	/* 5 */
	&sensor_dev_attr_temp4_input.dev_attr.attr,	/* 6 */
	&sensor_dev_attr_temp4_label.dev_attr.attr,	/* 7 */
	&sensor_dev_attr_temp5_input.dev_attr.attr,	/* 8 */
	&sensor_dev_attr_temp5_label.dev_attr.attr,	/* 9 */
	&sensor_dev_attr_temp6_input.dev_attr.attr,	/* 10 */
	&sensor_dev_attr_temp6_label.dev_attr.attr,	/* 11 */
	&sensor_dev_attr_temp7_input.dev_attr.attr,	/* 12 */
	&sensor_dev_attr_temp7_label.dev_attr.attr,	/* 13 */
	&sensor_dev_attr_temp8_input.dev_attr.attr,	/* 14 */
	&sensor_dev_attr_temp8_label.dev_attr.attr,	/* 15 */
	&sensor_dev_attr_temp9_input.dev_attr.attr,	/* 16 */
	&sensor_dev_attr_temp9_label.dev_attr.attr,	/* 17 */
	&sensor_dev_attr_temp10_input.dev_attr.attr,	/* 18 */
	&sensor_dev_attr_temp10_label.dev_attr.attr,	/* 19 */
	&sensor_dev_attr_fan1_input.dev_attr.attr,	/* 20 */
	&sensor_dev_attr_fan1_label.dev_attr.attr,	/* 21 */
	&sensor_dev_attr_pwm1.dev_attr.attr,		/* 22 */
	&sensor_dev_attr_pwm1_enable.dev_attr.attr,	/* 23 */
	&sensor_dev_attr_fan2_input.dev_attr.attr,	/* 24 */
	&sensor_dev_attr_fan2_label.dev_attr.attr,	/* 25 */
	&sensor_dev_attr_pwm2.dev_attr.attr,		/* 26 */
	&sensor_dev_attr_fan3_input.dev_attr.attr,	/* 27 */
	&sensor_dev_attr_fan3_label.dev_attr.attr,	/* 28 */
	&sensor_dev_attr_pwm3.dev_attr.attr,		/* 29 */
	NULL
};

static umode_t i8k_is_visible(struct kobject *kobj, struct attribute *attr,
			      int index)
{
	if (disallow_fan_support && index >= 20)
		return 0;
	if (disallow_fan_type_call &&
	    (index == 21 || index == 25 || index == 28))
		return 0;
	if (index >= 0 && index <= 1 &&
	    !(i8k_hwmon_flags & I8K_HWMON_HAVE_TEMP1))
		return 0;
	if (index >= 2 && index <= 3 &&
	    !(i8k_hwmon_flags & I8K_HWMON_HAVE_TEMP2))
		return 0;
	if (index >= 4 && index <= 5 &&
	    !(i8k_hwmon_flags & I8K_HWMON_HAVE_TEMP3))
		return 0;
	if (index >= 6 && index <= 7 &&
	    !(i8k_hwmon_flags & I8K_HWMON_HAVE_TEMP4))
		return 0;
	if (index >= 8 && index <= 9 &&
	    !(i8k_hwmon_flags & I8K_HWMON_HAVE_TEMP5))
		return 0;
	if (index >= 10 && index <= 11 &&
	    !(i8k_hwmon_flags & I8K_HWMON_HAVE_TEMP6))
		return 0;
	if (index >= 12 && index <= 13 &&
	    !(i8k_hwmon_flags & I8K_HWMON_HAVE_TEMP7))
		return 0;
	if (index >= 14 && index <= 15 &&
	    !(i8k_hwmon_flags & I8K_HWMON_HAVE_TEMP8))
		return 0;
	if (index >= 16 && index <= 17 &&
	    !(i8k_hwmon_flags & I8K_HWMON_HAVE_TEMP9))
		return 0;
	if (index >= 18 && index <= 19 &&
	    !(i8k_hwmon_flags & I8K_HWMON_HAVE_TEMP10))
		return 0;

	if (index >= 20 && index <= 23 &&
	    !(i8k_hwmon_flags & I8K_HWMON_HAVE_FAN1))
		return 0;
	if (index >= 24 && index <= 26 &&
	    !(i8k_hwmon_flags & I8K_HWMON_HAVE_FAN2))
		return 0;
	if (index >= 27 && index <= 29 &&
	    !(i8k_hwmon_flags & I8K_HWMON_HAVE_FAN3))
		return 0;

	if (index == 23 && !auto_fan)
		return 0;

	return attr->mode;
}

static const struct attribute_group i8k_group = {
	.attrs = i8k_attrs,
	.is_visible = i8k_is_visible,
};
__ATTRIBUTE_GROUPS(i8k);

static int __init i8k_init_hwmon(void)
{
	int err;

	i8k_hwmon_flags = 0;

	/* CPU temperature attributes, if temperature type is OK */
	err = i8k_get_temp_type(0);
	if (err >= 0)
		i8k_hwmon_flags |= I8K_HWMON_HAVE_TEMP1;
	/* check for additional temperature sensors */
	err = i8k_get_temp_type(1);
	if (err >= 0)
		i8k_hwmon_flags |= I8K_HWMON_HAVE_TEMP2;
	err = i8k_get_temp_type(2);
	if (err >= 0)
		i8k_hwmon_flags |= I8K_HWMON_HAVE_TEMP3;
	err = i8k_get_temp_type(3);
	if (err >= 0)
		i8k_hwmon_flags |= I8K_HWMON_HAVE_TEMP4;
	err = i8k_get_temp_type(4);
	if (err >= 0)
		i8k_hwmon_flags |= I8K_HWMON_HAVE_TEMP5;
	err = i8k_get_temp_type(5);
	if (err >= 0)
		i8k_hwmon_flags |= I8K_HWMON_HAVE_TEMP6;
	err = i8k_get_temp_type(6);
	if (err >= 0)
		i8k_hwmon_flags |= I8K_HWMON_HAVE_TEMP7;
	err = i8k_get_temp_type(7);
	if (err >= 0)
		i8k_hwmon_flags |= I8K_HWMON_HAVE_TEMP8;
	err = i8k_get_temp_type(8);
	if (err >= 0)
		i8k_hwmon_flags |= I8K_HWMON_HAVE_TEMP9;
	err = i8k_get_temp_type(9);
	if (err >= 0)
		i8k_hwmon_flags |= I8K_HWMON_HAVE_TEMP10;

	/* First fan attributes, if fan status or type is OK */
	err = i8k_get_fan_status(0);
	if (err < 0)
		err = i8k_get_fan_type(0);
	if (err >= 0)
		i8k_hwmon_flags |= I8K_HWMON_HAVE_FAN1;

	/* Second fan attributes, if fan status or type is OK */
	err = i8k_get_fan_status(1);
	if (err < 0)
		err = i8k_get_fan_type(1);
	if (err >= 0)
		i8k_hwmon_flags |= I8K_HWMON_HAVE_FAN2;

	/* Third fan attributes, if fan status or type is OK */
	err = i8k_get_fan_status(2);
	if (err < 0)
		err = i8k_get_fan_type(2);
	if (err >= 0)
		i8k_hwmon_flags |= I8K_HWMON_HAVE_FAN3;

	i8k_hwmon_dev = hwmon_device_register_with_groups(NULL, "dell_smm",
							  NULL, i8k_groups);
	if (IS_ERR(i8k_hwmon_dev)) {
		err = PTR_ERR(i8k_hwmon_dev);
		i8k_hwmon_dev = NULL;
		pr_err("hwmon registration failed (%d)\n", err);
		return err;
	}
	return 0;
}

struct i8k_config_data {
	uint fan_mult;
	uint fan_max;
};

enum i8k_configs {
	DELL_LATITUDE_D520,
	DELL_PRECISION_490,
	DELL_STUDIO,
	DELL_XPS,
};

static const struct i8k_config_data i8k_config_data[] = {
	[DELL_LATITUDE_D520] = {
		.fan_mult = 1,
		.fan_max = I8K_FAN_TURBO,
	},
	[DELL_PRECISION_490] = {
		.fan_mult = 1,
		.fan_max = I8K_FAN_TURBO,
	},
	[DELL_STUDIO] = {
		.fan_mult = 1,
		.fan_max = I8K_FAN_HIGH,
	},
	[DELL_XPS] = {
		.fan_mult = 1,
		.fan_max = I8K_FAN_HIGH,
	},
};

static const struct dmi_system_id i8k_dmi_table[] __initconst = {
	{
		.ident = "Dell Inspiron",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Computer"),
			DMI_MATCH(DMI_PRODUCT_NAME, "Inspiron"),
		},
	},
	{
		.ident = "Dell Latitude",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Computer"),
			DMI_MATCH(DMI_PRODUCT_NAME, "Latitude"),
		},
	},
	{
		.ident = "Dell Inspiron 2",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_MATCH(DMI_PRODUCT_NAME, "Inspiron"),
		},
	},
	{
		.ident = "Dell Latitude D520",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_MATCH(DMI_PRODUCT_NAME, "Latitude D520"),
		},
		.driver_data = (void *)&i8k_config_data[DELL_LATITUDE_D520],
	},
	{
		.ident = "Dell Latitude 2",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_MATCH(DMI_PRODUCT_NAME, "Latitude"),
		},
	},
	{	/* UK Inspiron 6400  */
		.ident = "Dell Inspiron 3",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_MATCH(DMI_PRODUCT_NAME, "MM061"),
		},
	},
	{
		.ident = "Dell Inspiron 3",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_MATCH(DMI_PRODUCT_NAME, "MP061"),
		},
	},
	{
		.ident = "Dell Precision 490",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_MATCH(DMI_PRODUCT_NAME,
				  "Precision WorkStation 490"),
		},
		.driver_data = (void *)&i8k_config_data[DELL_PRECISION_490],
	},
	{
		.ident = "Dell Precision",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_MATCH(DMI_PRODUCT_NAME, "Precision"),
		},
	},
	{
		.ident = "Dell Vostro",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_MATCH(DMI_PRODUCT_NAME, "Vostro"),
		},
	},
	{
		.ident = "Dell Studio",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_MATCH(DMI_PRODUCT_NAME, "Studio"),
		},
		.driver_data = (void *)&i8k_config_data[DELL_STUDIO],
	},
	{
		.ident = "Dell XPS M140",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_MATCH(DMI_PRODUCT_NAME, "MXC051"),
		},
		.driver_data = (void *)&i8k_config_data[DELL_XPS],
	},
	{
		.ident = "Dell XPS",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_MATCH(DMI_PRODUCT_NAME, "XPS"),
		},
	},
	{ }
};

MODULE_DEVICE_TABLE(dmi, i8k_dmi_table);

/*
 * On some machines once I8K_SMM_GET_FAN_TYPE is issued then CPU fan speed
 * randomly going up and down due to bug in Dell SMM or BIOS. Here is blacklist
 * of affected Dell machines for which we disallow I8K_SMM_GET_FAN_TYPE call.
 * See bug: https://bugzilla.kernel.org/show_bug.cgi?id=100121
 */
static const struct dmi_system_id i8k_blacklist_fan_type_dmi_table[] __initconst = {
	{
		.ident = "Dell Studio XPS 8000",
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "Studio XPS 8000"),
		},
	},
	{
		.ident = "Dell Studio XPS 8100",
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "Studio XPS 8100"),
		},
	},
	{
		.ident = "Dell Inspiron 580",
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "Inspiron 580 "),
		},
	},
	{ }
};

/*
 * On some machines all fan related SMM functions implemented by Dell BIOS
 * firmware freeze kernel for about 500ms. Until Dell fixes these problems fan
 * support for affected blacklisted Dell machines stay disabled.
 * See bug: https://bugzilla.kernel.org/show_bug.cgi?id=195751
 */
static struct dmi_system_id i8k_blacklist_fan_support_dmi_table[] __initdata = {
	{
		.ident = "Dell Inspiron 7720",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "Inspiron 7720"),
		},
	},
	{
		.ident = "Dell Vostro 3360",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "Vostro 3360"),
		},
	},
	{
		.ident = "Dell XPS13 9333",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "XPS13 9333"),
		},
	},
	{
		.ident = "Dell XPS 15 L502X",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "Dell System XPS L502X"),
		},
	},
	{ }
};

struct i8k_fan_control_data {
	unsigned int manual_fan;
	unsigned int auto_fan;
};

enum i8k_fan_controls {
	I8K_FAN_34A3_35A3,
};

static const struct i8k_fan_control_data i8k_fan_control_data[] = {
	[I8K_FAN_34A3_35A3] = {
		.manual_fan = 0x34a3,
		.auto_fan = 0x35a3,
	},
};

static struct dmi_system_id i8k_whitelist_fan_control[] __initdata = {
	{
		.ident = "Dell Precision 5530",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "Precision 5530"),
		},
		.driver_data = (void *)&i8k_fan_control_data[I8K_FAN_34A3_35A3],
	},
	{
		.ident = "Dell Latitude 5480",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "Latitude 5480"),
		},
		.driver_data = (void *)&i8k_fan_control_data[I8K_FAN_34A3_35A3],
	},
	{
		.ident = "Dell Latitude E6440",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "Latitude E6440"),
		},
		.driver_data = (void *)&i8k_fan_control_data[I8K_FAN_34A3_35A3],
	},
	{
		.ident = "Dell Latitude E7440",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "Latitude E7440"),
		},
		.driver_data = (void *)&i8k_fan_control_data[I8K_FAN_34A3_35A3],
	},
	{ }
};

/*
 * Probe for the presence of a supported laptop.
 */
static int __init i8k_probe(void)
{
	const struct dmi_system_id *id, *fan_control;
	int fan, ret;

	/*
	 * Get DMI information
	 */
	if (!dmi_check_system(i8k_dmi_table)) {
		if (!ignore_dmi && !force)
			return -ENODEV;

		pr_info("not running on a supported Dell system.\n");
		pr_info("vendor=%s, model=%s, version=%s\n",
			i8k_get_dmi_data(DMI_SYS_VENDOR),
			i8k_get_dmi_data(DMI_PRODUCT_NAME),
			i8k_get_dmi_data(DMI_BIOS_VERSION));
	}

	if (dmi_check_system(i8k_blacklist_fan_support_dmi_table)) {
		pr_warn("broken Dell BIOS detected, disallow fan support\n");
		if (!force)
			disallow_fan_support = true;
	}

	if (dmi_check_system(i8k_blacklist_fan_type_dmi_table)) {
		pr_warn("broken Dell BIOS detected, disallow fan type call\n");
		if (!force)
			disallow_fan_type_call = true;
	}

	strlcpy(bios_version, i8k_get_dmi_data(DMI_BIOS_VERSION),
		sizeof(bios_version));
	strlcpy(bios_machineid, i8k_get_dmi_data(DMI_PRODUCT_SERIAL),
		sizeof(bios_machineid));

	/*
	 * Get SMM Dell signature
	 */
	if (i8k_get_dell_signature(I8K_SMM_GET_DELL_SIG1) &&
	    i8k_get_dell_signature(I8K_SMM_GET_DELL_SIG2)) {
		pr_err("unable to get SMM Dell signature\n");
		if (!force)
			return -ENODEV;
	}

	/*
	 * Set fan multiplier and maximal fan speed from dmi config
	 * Values specified in module parameters override values from dmi
	 */
	id = dmi_first_match(i8k_dmi_table);
	if (id && id->driver_data) {
		const struct i8k_config_data *conf = id->driver_data;
		if (!fan_mult && conf->fan_mult)
			fan_mult = conf->fan_mult;
		if (!fan_max && conf->fan_max)
			fan_max = conf->fan_max;
	}

	i8k_fan_max = fan_max ? : I8K_FAN_HIGH;	/* Must not be 0 */
	i8k_pwm_mult = DIV_ROUND_UP(255, i8k_fan_max);

	fan_control = dmi_first_match(i8k_whitelist_fan_control);
	if (fan_control && fan_control->driver_data) {
		const struct i8k_fan_control_data *data = fan_control->driver_data;

		manual_fan = data->manual_fan;
		auto_fan = data->auto_fan;
		pr_info("enabling support for setting automatic/manual fan control\n");
	}

	if (!fan_mult) {
		/*
		 * Autodetect fan multiplier based on nominal rpm
		 * If fan reports rpm value too high then set multiplier to 1
		 */
		for (fan = 0; fan < 2; ++fan) {
			ret = i8k_get_fan_nominal_speed(fan, i8k_fan_max);
			if (ret < 0)
				continue;
			if (ret > I8K_FAN_MAX_RPM)
				i8k_fan_mult = 1;
			break;
		}
	} else {
		/* Fan multiplier was specified in module param or in dmi */
		i8k_fan_mult = fan_mult;
	}

	return 0;
}

static int __init i8k_init(void)
{
	int err;

	/* Are we running on an supported laptop? */
	if (i8k_probe())
		return -ENODEV;

	err = i8k_init_hwmon();
	if (err)
		return err;

	i8k_init_procfs();
	return 0;
}

static void __exit i8k_exit(void)
{
	hwmon_device_unregister(i8k_hwmon_dev);
	i8k_exit_procfs();
}

module_init(i8k_init);
module_exit(i8k_exit);
