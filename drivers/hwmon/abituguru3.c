// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * abituguru3.c
 *
 * Copyright (c) 2006-2008 Hans de Goede <hdegoede@redhat.com>
 * Copyright (c) 2008 Alistair John Strachan <alistair@devzero.co.uk>
 */
/*
 * This driver supports the sensor part of revision 3 of the custom Abit uGuru
 * chip found on newer Abit uGuru motherboards. Note: because of lack of specs
 * only reading the sensors and their settings is supported.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/dmi.h>
#include <linux/io.h>

/* uGuru3 bank addresses */
#define ABIT_UGURU3_SETTINGS_BANK		0x01
#define ABIT_UGURU3_SENSORS_BANK		0x08
#define ABIT_UGURU3_MISC_BANK			0x09
#define ABIT_UGURU3_ALARMS_START		0x1E
#define ABIT_UGURU3_SETTINGS_START		0x24
#define ABIT_UGURU3_VALUES_START		0x80
#define ABIT_UGURU3_BOARD_ID			0x0A
/* uGuru3 sensor bank flags */			     /* Alarm if: */
#define ABIT_UGURU3_TEMP_HIGH_ALARM_ENABLE	0x01 /*  temp over warn */
#define ABIT_UGURU3_VOLT_HIGH_ALARM_ENABLE	0x02 /*  volt over max */
#define ABIT_UGURU3_VOLT_LOW_ALARM_ENABLE	0x04 /*  volt under min */
#define ABIT_UGURU3_TEMP_HIGH_ALARM_FLAG	0x10 /* temp is over warn */
#define ABIT_UGURU3_VOLT_HIGH_ALARM_FLAG	0x20 /* volt is over max */
#define ABIT_UGURU3_VOLT_LOW_ALARM_FLAG		0x40 /* volt is under min */
#define ABIT_UGURU3_FAN_LOW_ALARM_ENABLE	0x01 /*   fan under min */
#define ABIT_UGURU3_BEEP_ENABLE			0x08 /* beep if alarm */
#define ABIT_UGURU3_SHUTDOWN_ENABLE		0x80 /* shutdown if alarm */
/* sensor types */
#define ABIT_UGURU3_IN_SENSOR			0
#define ABIT_UGURU3_TEMP_SENSOR			1
#define ABIT_UGURU3_FAN_SENSOR			2

/*
 * Timeouts / Retries, if these turn out to need a lot of fiddling we could
 * convert them to params. Determined by trial and error. I assume this is
 * cpu-speed independent, since the ISA-bus and not the CPU should be the
 * bottleneck.
 */
#define ABIT_UGURU3_WAIT_TIMEOUT		250
/*
 * Normally the 0xAC at the end of synchronize() is reported after the
 * first read, but sometimes not and we need to poll
 */
#define ABIT_UGURU3_SYNCHRONIZE_TIMEOUT		5
/* utility macros */
#define ABIT_UGURU3_NAME			"abituguru3"
#define ABIT_UGURU3_DEBUG(format, arg...)		\
	do {						\
		if (verbose)				\
			pr_debug(format , ## arg);	\
	} while (0)

/* Macros to help calculate the sysfs_names array length */
#define ABIT_UGURU3_MAX_NO_SENSORS 26
/*
 * sum of strlen +1 of: in??_input\0, in??_{min,max}\0, in??_{min,max}_alarm\0,
 * in??_{min,max}_alarm_enable\0, in??_beep\0, in??_shutdown\0, in??_label\0
 */
#define ABIT_UGURU3_IN_NAMES_LENGTH \
				(11 + 2 * 9 + 2 * 15 + 2 * 22 + 10 + 14 + 11)
/*
 * sum of strlen +1 of: temp??_input\0, temp??_max\0, temp??_crit\0,
 * temp??_alarm\0, temp??_alarm_enable\0, temp??_beep\0, temp??_shutdown\0,
 * temp??_label\0
 */
#define ABIT_UGURU3_TEMP_NAMES_LENGTH (13 + 11 + 12 + 13 + 20 + 12 + 16 + 13)
/*
 * sum of strlen +1 of: fan??_input\0, fan??_min\0, fan??_alarm\0,
 * fan??_alarm_enable\0, fan??_beep\0, fan??_shutdown\0, fan??_label\0
 */
#define ABIT_UGURU3_FAN_NAMES_LENGTH (12 + 10 + 12 + 19 + 11 + 15 + 12)
/*
 * Worst case scenario 16 in sensors (longest names_length) and the rest
 * temp sensors (second longest names_length).
 */
#define ABIT_UGURU3_SYSFS_NAMES_LENGTH (16 * ABIT_UGURU3_IN_NAMES_LENGTH + \
	(ABIT_UGURU3_MAX_NO_SENSORS - 16) * ABIT_UGURU3_TEMP_NAMES_LENGTH)

/*
 * All the macros below are named identical to the openguru2 program
 * reverse engineered by Louis Kruger, hence the names might not be 100%
 * logical. I could come up with better names, but I prefer keeping the names
 * identical so that this driver can be compared with his work more easily.
 */
/* Two i/o-ports are used by uGuru */
#define ABIT_UGURU3_BASE			0x00E0
#define ABIT_UGURU3_CMD				0x00
#define ABIT_UGURU3_DATA			0x04
#define ABIT_UGURU3_REGION_LENGTH		5
/*
 * The wait_xxx functions return this on success and the last contents
 * of the DATA register (0-255) on failure.
 */
#define ABIT_UGURU3_SUCCESS			-1
/* uGuru status flags */
#define ABIT_UGURU3_STATUS_READY_FOR_READ	0x01
#define ABIT_UGURU3_STATUS_BUSY			0x02


/* Structures */
struct abituguru3_sensor_info {
	const char *name;
	int port;
	int type;
	int multiplier;
	int divisor;
	int offset;
};

/* Avoid use of flexible array members */
#define ABIT_UGURU3_MAX_DMI_NAMES 2

struct abituguru3_motherboard_info {
	u16 id;
	const char *dmi_name[ABIT_UGURU3_MAX_DMI_NAMES + 1];
	/* + 1 -> end of sensors indicated by a sensor with name == NULL */
	struct abituguru3_sensor_info sensors[ABIT_UGURU3_MAX_NO_SENSORS + 1];
};

/*
 * For the Abit uGuru, we need to keep some data in memory.
 * The structure is dynamically allocated, at the same time when a new
 * abituguru3 device is allocated.
 */
struct abituguru3_data {
	struct device *hwmon_dev;	/* hwmon registered device */
	struct mutex update_lock;	/* protect access to data and uGuru */
	unsigned short addr;		/* uguru base address */
	char valid;			/* !=0 if following fields are valid */
	unsigned long last_updated;	/* In jiffies */

	/*
	 * For convenience the sysfs attr and their names are generated
	 * automatically. We have max 10 entries per sensor (for in sensors)
	 */
	struct sensor_device_attribute_2 sysfs_attr[ABIT_UGURU3_MAX_NO_SENSORS
		* 10];

	/* Buffer to store the dynamically generated sysfs names */
	char sysfs_names[ABIT_UGURU3_SYSFS_NAMES_LENGTH];

	/* Pointer to the sensors info for the detected motherboard */
	const struct abituguru3_sensor_info *sensors;

	/*
	 * The abituguru3 supports up to 48 sensors, and thus has registers
	 * sets for 48 sensors, for convenience reasons / simplicity of the
	 * code we always read and store all registers for all 48 sensors
	 */

	/* Alarms for all 48 sensors (1 bit per sensor) */
	u8 alarms[48/8];

	/* Value of all 48 sensors */
	u8 value[48];

	/*
	 * Settings of all 48 sensors, note in and temp sensors (the first 32
	 * sensors) have 3 bytes of settings, while fans only have 2 bytes,
	 * for convenience we use 3 bytes for all sensors
	 */
	u8 settings[48][3];
};


/* Constants */
static const struct abituguru3_motherboard_info abituguru3_motherboards[] = {
	{ 0x000C, { NULL } /* Unknown, need DMI string */, {
		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR",		 1, 0, 10, 1, 0 },
		{ "DDR VTT",		 2, 0, 10, 1, 0 },
		{ "CPU VTT 1.2V",	 3, 0, 10, 1, 0 },
		{ "MCH & PCIE 1.5V",	 4, 0, 10, 1, 0 },
		{ "MCH 2.5V",		 5, 0, 20, 1, 0 },
		{ "ICH 1.05V",		 6, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (4-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "+3.3V",		10, 0, 20, 1, 0 },
		{ "5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "System",		25, 1, 1, 1, 0 },
		{ "PWM",		26, 1, 1, 1, 0 },
		{ "CPU Fan",		32, 2, 60, 1, 0 },
		{ "NB Fan",		33, 2, 60, 1, 0 },
		{ "SYS FAN",		34, 2, 60, 1, 0 },
		{ "AUX1 Fan",		35, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x000D, { NULL } /* Abit AW8, need DMI string */, {
		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR",		 1, 0, 10, 1, 0 },
		{ "DDR VTT",		 2, 0, 10, 1, 0 },
		{ "CPU VTT 1.2V",	 3, 0, 10, 1, 0 },
		{ "MCH & PCIE 1.5V",	 4, 0, 10, 1, 0 },
		{ "MCH 2.5V",		 5, 0, 20, 1, 0 },
		{ "ICH 1.05V",		 6, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (4-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "+3.3V",		10, 0, 20, 1, 0 },
		{ "5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "System",		25, 1, 1, 1, 0 },
		{ "PWM1",		26, 1, 1, 1, 0 },
		{ "PWM2",		27, 1, 1, 1, 0 },
		{ "PWM3",		28, 1, 1, 1, 0 },
		{ "PWM4",		29, 1, 1, 1, 0 },
		{ "CPU Fan",		32, 2, 60, 1, 0 },
		{ "NB Fan",		33, 2, 60, 1, 0 },
		{ "SYS Fan",		34, 2, 60, 1, 0 },
		{ "AUX1 Fan",		35, 2, 60, 1, 0 },
		{ "AUX2 Fan",		36, 2, 60, 1, 0 },
		{ "AUX3 Fan",		37, 2, 60, 1, 0 },
		{ "AUX4 Fan",		38, 2, 60, 1, 0 },
		{ "AUX5 Fan",		39, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x000E, { NULL } /* AL-8, need DMI string */, {
		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR",		 1, 0, 10, 1, 0 },
		{ "DDR VTT",		 2, 0, 10, 1, 0 },
		{ "CPU VTT 1.2V",	 3, 0, 10, 1, 0 },
		{ "MCH & PCIE 1.5V",	 4, 0, 10, 1, 0 },
		{ "MCH 2.5V",		 5, 0, 20, 1, 0 },
		{ "ICH 1.05V",		 6, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (4-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "+3.3V",		10, 0, 20, 1, 0 },
		{ "5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "System",		25, 1, 1, 1, 0 },
		{ "PWM",		26, 1, 1, 1, 0 },
		{ "CPU Fan",		32, 2, 60, 1, 0 },
		{ "NB Fan",		33, 2, 60, 1, 0 },
		{ "SYS Fan",		34, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x000F, { NULL } /* Unknown, need DMI string */, {

		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR",		 1, 0, 10, 1, 0 },
		{ "DDR VTT",		 2, 0, 10, 1, 0 },
		{ "CPU VTT 1.2V",	 3, 0, 10, 1, 0 },
		{ "MCH & PCIE 1.5V",	 4, 0, 10, 1, 0 },
		{ "MCH 2.5V",		 5, 0, 20, 1, 0 },
		{ "ICH 1.05V",		 6, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (4-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "+3.3V",		10, 0, 20, 1, 0 },
		{ "5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "System",		25, 1, 1, 1, 0 },
		{ "PWM",		26, 1, 1, 1, 0 },
		{ "CPU Fan",		32, 2, 60, 1, 0 },
		{ "NB Fan",		33, 2, 60, 1, 0 },
		{ "SYS Fan",		34, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x0010, { NULL } /* Abit NI8 SLI GR, need DMI string */, {
		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR",		 1, 0, 10, 1, 0 },
		{ "DDR VTT",		 2, 0, 10, 1, 0 },
		{ "CPU VTT 1.2V",	 3, 0, 10, 1, 0 },
		{ "NB 1.4V",		 4, 0, 10, 1, 0 },
		{ "SB 1.5V",		 6, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (4-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "+3.3V",		10, 0, 20, 1, 0 },
		{ "5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "SYS",		25, 1, 1, 1, 0 },
		{ "PWM",		26, 1, 1, 1, 0 },
		{ "CPU Fan",		32, 2, 60, 1, 0 },
		{ "NB Fan",		33, 2, 60, 1, 0 },
		{ "SYS Fan",		34, 2, 60, 1, 0 },
		{ "AUX1 Fan",		35, 2, 60, 1, 0 },
		{ "OTES1 Fan",		36, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x0011, { "AT8 32X", NULL }, {
		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR",		 1, 0, 20, 1, 0 },
		{ "DDR VTT",		 2, 0, 10, 1, 0 },
		{ "CPU VDDA 2.5V",	 6, 0, 20, 1, 0 },
		{ "NB 1.8V",		 4, 0, 10, 1, 0 },
		{ "NB 1.8V Dual",	 5, 0, 10, 1, 0 },
		{ "HTV 1.2",		 3, 0, 10, 1, 0 },
		{ "PCIE 1.2V",		12, 0, 10, 1, 0 },
		{ "NB 1.2V",		13, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (4-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "+3.3V",		10, 0, 20, 1, 0 },
		{ "5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "NB",			25, 1, 1, 1, 0 },
		{ "System",		26, 1, 1, 1, 0 },
		{ "PWM",		27, 1, 1, 1, 0 },
		{ "CPU Fan",		32, 2, 60, 1, 0 },
		{ "NB Fan",		33, 2, 60, 1, 0 },
		{ "SYS Fan",		34, 2, 60, 1, 0 },
		{ "AUX1 Fan",		35, 2, 60, 1, 0 },
		{ "AUX2 Fan",		36, 2, 60, 1, 0 },
		{ "AUX3 Fan",		37, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x0012, { NULL } /* Abit AN8 32X, need DMI string */, {
		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR",		 1, 0, 20, 1, 0 },
		{ "DDR VTT",		 2, 0, 10, 1, 0 },
		{ "HyperTransport",	 3, 0, 10, 1, 0 },
		{ "CPU VDDA 2.5V",	 5, 0, 20, 1, 0 },
		{ "NB",			 4, 0, 10, 1, 0 },
		{ "SB",			 6, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (4-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "+3.3V",		10, 0, 20, 1, 0 },
		{ "5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "SYS",		25, 1, 1, 1, 0 },
		{ "PWM",		26, 1, 1, 1, 0 },
		{ "CPU Fan",		32, 2, 60, 1, 0 },
		{ "NB Fan",		33, 2, 60, 1, 0 },
		{ "SYS Fan",		34, 2, 60, 1, 0 },
		{ "AUX1 Fan",		36, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x0013, { NULL } /* Abit AW8D, need DMI string */, {
		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR",		 1, 0, 10, 1, 0 },
		{ "DDR VTT",		 2, 0, 10, 1, 0 },
		{ "CPU VTT 1.2V",	 3, 0, 10, 1, 0 },
		{ "MCH & PCIE 1.5V",	 4, 0, 10, 1, 0 },
		{ "MCH 2.5V",		 5, 0, 20, 1, 0 },
		{ "ICH 1.05V",		 6, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (4-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "+3.3V",		10, 0, 20, 1, 0 },
		{ "5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "System",		25, 1, 1, 1, 0 },
		{ "PWM1",		26, 1, 1, 1, 0 },
		{ "PWM2",		27, 1, 1, 1, 0 },
		{ "PWM3",		28, 1, 1, 1, 0 },
		{ "PWM4",		29, 1, 1, 1, 0 },
		{ "CPU Fan",		32, 2, 60, 1, 0 },
		{ "NB Fan",		33, 2, 60, 1, 0 },
		{ "SYS Fan",		34, 2, 60, 1, 0 },
		{ "AUX1 Fan",		35, 2, 60, 1, 0 },
		{ "AUX2 Fan",		36, 2, 60, 1, 0 },
		{ "AUX3 Fan",		37, 2, 60, 1, 0 },
		{ "AUX4 Fan",		38, 2, 60, 1, 0 },
		{ "AUX5 Fan",		39, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x0014, { "AB9", "AB9 Pro", NULL }, {
		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR",		 1, 0, 10, 1, 0 },
		{ "DDR VTT",		 2, 0, 10, 1, 0 },
		{ "CPU VTT 1.2V",	 3, 0, 10, 1, 0 },
		{ "MCH & PCIE 1.5V",	 4, 0, 10, 1, 0 },
		{ "MCH 2.5V",		 5, 0, 20, 1, 0 },
		{ "ICH 1.05V",		 6, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (4-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "+3.3V",		10, 0, 20, 1, 0 },
		{ "5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "System",		25, 1, 1, 1, 0 },
		{ "PWM",		26, 1, 1, 1, 0 },
		{ "CPU Fan",		32, 2, 60, 1, 0 },
		{ "NB Fan",		33, 2, 60, 1, 0 },
		{ "SYS Fan",		34, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x0015, { NULL } /* Unknown, need DMI string */, {
		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR",		 1, 0, 20, 1, 0 },
		{ "DDR VTT",		 2, 0, 10, 1, 0 },
		{ "HyperTransport",	 3, 0, 10, 1, 0 },
		{ "CPU VDDA 2.5V",	 5, 0, 20, 1, 0 },
		{ "NB",			 4, 0, 10, 1, 0 },
		{ "SB",			 6, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (4-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "+3.3V",		10, 0, 20, 1, 0 },
		{ "5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "SYS",		25, 1, 1, 1, 0 },
		{ "PWM",		26, 1, 1, 1, 0 },
		{ "CPU Fan",		32, 2, 60, 1, 0 },
		{ "NB Fan",		33, 2, 60, 1, 0 },
		{ "SYS Fan",		34, 2, 60, 1, 0 },
		{ "AUX1 Fan",		33, 2, 60, 1, 0 },
		{ "AUX2 Fan",		35, 2, 60, 1, 0 },
		{ "AUX3 Fan",		36, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x0016, { "AW9D-MAX", NULL }, {
		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR2",		 1, 0, 20, 1, 0 },
		{ "DDR2 VTT",		 2, 0, 10, 1, 0 },
		{ "CPU VTT 1.2V",	 3, 0, 10, 1, 0 },
		{ "MCH & PCIE 1.5V",	 4, 0, 10, 1, 0 },
		{ "MCH 2.5V",		 5, 0, 20, 1, 0 },
		{ "ICH 1.05V",		 6, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (4-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "+3.3V",		10, 0, 20, 1, 0 },
		{ "5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "System",		25, 1, 1, 1, 0 },
		{ "PWM1",		26, 1, 1, 1, 0 },
		{ "PWM2",		27, 1, 1, 1, 0 },
		{ "PWM3",		28, 1, 1, 1, 0 },
		{ "PWM4",		29, 1, 1, 1, 0 },
		{ "CPU Fan",		32, 2, 60, 1, 0 },
		{ "NB Fan",		33, 2, 60, 1, 0 },
		{ "SYS Fan",		34, 2, 60, 1, 0 },
		{ "AUX1 Fan",		35, 2, 60, 1, 0 },
		{ "AUX2 Fan",		36, 2, 60, 1, 0 },
		{ "AUX3 Fan",		37, 2, 60, 1, 0 },
		{ "OTES1 Fan",		38, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x0017, { NULL } /* Unknown, need DMI string */, {
		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR2",		 1, 0, 20, 1, 0 },
		{ "DDR2 VTT",		 2, 0, 10, 1, 0 },
		{ "HyperTransport",	 3, 0, 10, 1, 0 },
		{ "CPU VDDA 2.5V",	 6, 0, 20, 1, 0 },
		{ "NB 1.8V",		 4, 0, 10, 1, 0 },
		{ "NB 1.2V ",		13, 0, 10, 1, 0 },
		{ "SB 1.2V",		 5, 0, 10, 1, 0 },
		{ "PCIE 1.2V",		12, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (4-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "ATX +3.3V",		10, 0, 20, 1, 0 },
		{ "ATX 5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "System",		26, 1, 1, 1, 0 },
		{ "PWM",		27, 1, 1, 1, 0 },
		{ "CPU FAN",		32, 2, 60, 1, 0 },
		{ "SYS FAN",		34, 2, 60, 1, 0 },
		{ "AUX1 FAN",		35, 2, 60, 1, 0 },
		{ "AUX2 FAN",		36, 2, 60, 1, 0 },
		{ "AUX3 FAN",		37, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x0018, { "AB9 QuadGT", NULL }, {
		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR2",		 1, 0, 20, 1, 0 },
		{ "DDR2 VTT",		 2, 0, 10, 1, 0 },
		{ "CPU VTT",		 3, 0, 10, 1, 0 },
		{ "MCH 1.25V",		 4, 0, 10, 1, 0 },
		{ "ICHIO 1.5V",		 5, 0, 10, 1, 0 },
		{ "ICH 1.05V",		 6, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (4-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "+3.3V",		10, 0, 20, 1, 0 },
		{ "5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "System",		25, 1, 1, 1, 0 },
		{ "PWM Phase1",		26, 1, 1, 1, 0 },
		{ "PWM Phase2",		27, 1, 1, 1, 0 },
		{ "PWM Phase3",		28, 1, 1, 1, 0 },
		{ "PWM Phase4",		29, 1, 1, 1, 0 },
		{ "PWM Phase5",		30, 1, 1, 1, 0 },
		{ "CPU Fan",		32, 2, 60, 1, 0 },
		{ "SYS Fan",		34, 2, 60, 1, 0 },
		{ "AUX1 Fan",		33, 2, 60, 1, 0 },
		{ "AUX2 Fan",		35, 2, 60, 1, 0 },
		{ "AUX3 Fan",		36, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x0019, { "IN9 32X MAX", NULL }, {
		{ "CPU Core",		 7, 0, 10, 1, 0 },
		{ "DDR2",		13, 0, 20, 1, 0 },
		{ "DDR2 VTT",		14, 0, 10, 1, 0 },
		{ "CPU VTT",		 3, 0, 20, 1, 0 },
		{ "NB 1.2V",		 4, 0, 10, 1, 0 },
		{ "SB 1.5V",		 6, 0, 10, 1, 0 },
		{ "HyperTransport",	 5, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	12, 0, 60, 1, 0 },
		{ "ATX +12V (4-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "ATX +3.3V",		10, 0, 20, 1, 0 },
		{ "ATX 5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "System",		25, 1, 1, 1, 0 },
		{ "PWM Phase1",		26, 1, 1, 1, 0 },
		{ "PWM Phase2",		27, 1, 1, 1, 0 },
		{ "PWM Phase3",		28, 1, 1, 1, 0 },
		{ "PWM Phase4",		29, 1, 1, 1, 0 },
		{ "PWM Phase5",		30, 1, 1, 1, 0 },
		{ "CPU FAN",		32, 2, 60, 1, 0 },
		{ "SYS FAN",		34, 2, 60, 1, 0 },
		{ "AUX1 FAN",		33, 2, 60, 1, 0 },
		{ "AUX2 FAN",		35, 2, 60, 1, 0 },
		{ "AUX3 FAN",		36, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x001A, { "IP35 Pro", "IP35 Pro XE", NULL }, {
		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR2",		 1, 0, 20, 1, 0 },
		{ "DDR2 VTT",		 2, 0, 10, 1, 0 },
		{ "CPU VTT 1.2V",	 3, 0, 10, 1, 0 },
		{ "MCH 1.25V",		 4, 0, 10, 1, 0 },
		{ "ICHIO 1.5V",		 5, 0, 10, 1, 0 },
		{ "ICH 1.05V",		 6, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (8-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "+3.3V",		10, 0, 20, 1, 0 },
		{ "5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "System",		25, 1, 1, 1, 0 },
		{ "PWM",		26, 1, 1, 1, 0 },
		{ "PWM Phase2",		27, 1, 1, 1, 0 },
		{ "PWM Phase3",		28, 1, 1, 1, 0 },
		{ "PWM Phase4",		29, 1, 1, 1, 0 },
		{ "PWM Phase5",		30, 1, 1, 1, 0 },
		{ "CPU Fan",		32, 2, 60, 1, 0 },
		{ "SYS Fan",		34, 2, 60, 1, 0 },
		{ "AUX1 Fan",		33, 2, 60, 1, 0 },
		{ "AUX2 Fan",		35, 2, 60, 1, 0 },
		{ "AUX3 Fan",		36, 2, 60, 1, 0 },
		{ "AUX4 Fan",		37, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x001B, { NULL } /* Unknown, need DMI string */, {
		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR3",		 1, 0, 20, 1, 0 },
		{ "DDR3 VTT",		 2, 0, 10, 1, 0 },
		{ "CPU VTT",		 3, 0, 10, 1, 0 },
		{ "MCH 1.25V",		 4, 0, 10, 1, 0 },
		{ "ICHIO 1.5V",		 5, 0, 10, 1, 0 },
		{ "ICH 1.05V",		 6, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (8-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "+3.3V",		10, 0, 20, 1, 0 },
		{ "5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "System",		25, 1, 1, 1, 0 },
		{ "PWM Phase1",		26, 1, 1, 1, 0 },
		{ "PWM Phase2",		27, 1, 1, 1, 0 },
		{ "PWM Phase3",		28, 1, 1, 1, 0 },
		{ "PWM Phase4",		29, 1, 1, 1, 0 },
		{ "PWM Phase5",		30, 1, 1, 1, 0 },
		{ "CPU Fan",		32, 2, 60, 1, 0 },
		{ "SYS Fan",		34, 2, 60, 1, 0 },
		{ "AUX1 Fan",		33, 2, 60, 1, 0 },
		{ "AUX2 Fan",		35, 2, 60, 1, 0 },
		{ "AUX3 Fan",		36, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x001C, { "IX38 QuadGT", NULL }, {
		{ "CPU Core",		 0, 0, 10, 1, 0 },
		{ "DDR2",		 1, 0, 20, 1, 0 },
		{ "DDR2 VTT",		 2, 0, 10, 1, 0 },
		{ "CPU VTT",		 3, 0, 10, 1, 0 },
		{ "MCH 1.25V",		 4, 0, 10, 1, 0 },
		{ "ICHIO 1.5V",		 5, 0, 10, 1, 0 },
		{ "ICH 1.05V",		 6, 0, 10, 1, 0 },
		{ "ATX +12V (24-Pin)",	 7, 0, 60, 1, 0 },
		{ "ATX +12V (8-pin)",	 8, 0, 60, 1, 0 },
		{ "ATX +5V",		 9, 0, 30, 1, 0 },
		{ "+3.3V",		10, 0, 20, 1, 0 },
		{ "5VSB",		11, 0, 30, 1, 0 },
		{ "CPU",		24, 1, 1, 1, 0 },
		{ "System",		25, 1, 1, 1, 0 },
		{ "PWM Phase1",		26, 1, 1, 1, 0 },
		{ "PWM Phase2",		27, 1, 1, 1, 0 },
		{ "PWM Phase3",		28, 1, 1, 1, 0 },
		{ "PWM Phase4",		29, 1, 1, 1, 0 },
		{ "PWM Phase5",		30, 1, 1, 1, 0 },
		{ "CPU Fan",		32, 2, 60, 1, 0 },
		{ "SYS Fan",		34, 2, 60, 1, 0 },
		{ "AUX1 Fan",		33, 2, 60, 1, 0 },
		{ "AUX2 Fan",		35, 2, 60, 1, 0 },
		{ "AUX3 Fan",		36, 2, 60, 1, 0 },
		{ NULL, 0, 0, 0, 0, 0 } }
	},
	{ 0x0000, { NULL }, { { NULL, 0, 0, 0, 0, 0 } } }
};


/* Insmod parameters */
static bool force;
module_param(force, bool, 0);
MODULE_PARM_DESC(force, "Set to one to force detection.");
/* Default verbose is 1, since this driver is still in the testing phase */
static bool verbose = 1;
module_param(verbose, bool, 0644);
MODULE_PARM_DESC(verbose, "Enable/disable verbose error reporting");

static const char *never_happen = "This should never happen.";
static const char *report_this =
	"Please report this to the abituguru3 maintainer (see MAINTAINERS)";

/* wait while the uguru is busy (usually after a write) */
static int abituguru3_wait_while_busy(struct abituguru3_data *data)
{
	u8 x;
	int timeout = ABIT_UGURU3_WAIT_TIMEOUT;

	while ((x = inb_p(data->addr + ABIT_UGURU3_DATA)) &
			ABIT_UGURU3_STATUS_BUSY) {
		timeout--;
		if (timeout == 0)
			return x;
		/*
		 * sleep a bit before our last try, to give the uGuru3 one
		 * last chance to respond.
		 */
		if (timeout == 1)
			msleep(1);
	}
	return ABIT_UGURU3_SUCCESS;
}

/* wait till uguru is ready to be read */
static int abituguru3_wait_for_read(struct abituguru3_data *data)
{
	u8 x;
	int timeout = ABIT_UGURU3_WAIT_TIMEOUT;

	while (!((x = inb_p(data->addr + ABIT_UGURU3_DATA)) &
			ABIT_UGURU3_STATUS_READY_FOR_READ)) {
		timeout--;
		if (timeout == 0)
			return x;
		/*
		 * sleep a bit before our last try, to give the uGuru3 one
		 * last chance to respond.
		 */
		if (timeout == 1)
			msleep(1);
	}
	return ABIT_UGURU3_SUCCESS;
}

/*
 * This synchronizes us with the uGuru3's protocol state machine, this
 * must be done before each command.
 */
static int abituguru3_synchronize(struct abituguru3_data *data)
{
	int x, timeout = ABIT_UGURU3_SYNCHRONIZE_TIMEOUT;

	x = abituguru3_wait_while_busy(data);
	if (x != ABIT_UGURU3_SUCCESS) {
		ABIT_UGURU3_DEBUG("synchronize timeout during initial busy "
			"wait, status: 0x%02x\n", x);
		return -EIO;
	}

	outb(0x20, data->addr + ABIT_UGURU3_DATA);
	x = abituguru3_wait_while_busy(data);
	if (x != ABIT_UGURU3_SUCCESS) {
		ABIT_UGURU3_DEBUG("synchronize timeout after sending 0x20, "
			"status: 0x%02x\n", x);
		return -EIO;
	}

	outb(0x10, data->addr + ABIT_UGURU3_CMD);
	x = abituguru3_wait_while_busy(data);
	if (x != ABIT_UGURU3_SUCCESS) {
		ABIT_UGURU3_DEBUG("synchronize timeout after sending 0x10, "
			"status: 0x%02x\n", x);
		return -EIO;
	}

	outb(0x00, data->addr + ABIT_UGURU3_CMD);
	x = abituguru3_wait_while_busy(data);
	if (x != ABIT_UGURU3_SUCCESS) {
		ABIT_UGURU3_DEBUG("synchronize timeout after sending 0x00, "
			"status: 0x%02x\n", x);
		return -EIO;
	}

	x = abituguru3_wait_for_read(data);
	if (x != ABIT_UGURU3_SUCCESS) {
		ABIT_UGURU3_DEBUG("synchronize timeout waiting for read, "
			"status: 0x%02x\n", x);
		return -EIO;
	}

	while ((x = inb(data->addr + ABIT_UGURU3_CMD)) != 0xAC) {
		timeout--;
		if (timeout == 0) {
			ABIT_UGURU3_DEBUG("synchronize timeout cmd does not "
				"hold 0xAC after synchronize, cmd: 0x%02x\n",
				x);
			return -EIO;
		}
		msleep(1);
	}
	return 0;
}

/*
 * Read count bytes from sensor sensor_addr in bank bank_addr and store the
 * result in buf
 */
static int abituguru3_read(struct abituguru3_data *data, u8 bank, u8 offset,
	u8 count, u8 *buf)
{
	int i, x;

	x = abituguru3_synchronize(data);
	if (x)
		return x;

	outb(0x1A, data->addr + ABIT_UGURU3_DATA);
	x = abituguru3_wait_while_busy(data);
	if (x != ABIT_UGURU3_SUCCESS) {
		ABIT_UGURU3_DEBUG("read from 0x%02x:0x%02x timed out after "
			"sending 0x1A, status: 0x%02x\n", (unsigned int)bank,
			(unsigned int)offset, x);
		return -EIO;
	}

	outb(bank, data->addr + ABIT_UGURU3_CMD);
	x = abituguru3_wait_while_busy(data);
	if (x != ABIT_UGURU3_SUCCESS) {
		ABIT_UGURU3_DEBUG("read from 0x%02x:0x%02x timed out after "
			"sending the bank, status: 0x%02x\n",
			(unsigned int)bank, (unsigned int)offset, x);
		return -EIO;
	}

	outb(offset, data->addr + ABIT_UGURU3_CMD);
	x = abituguru3_wait_while_busy(data);
	if (x != ABIT_UGURU3_SUCCESS) {
		ABIT_UGURU3_DEBUG("read from 0x%02x:0x%02x timed out after "
			"sending the offset, status: 0x%02x\n",
			(unsigned int)bank, (unsigned int)offset, x);
		return -EIO;
	}

	outb(count, data->addr + ABIT_UGURU3_CMD);
	x = abituguru3_wait_while_busy(data);
	if (x != ABIT_UGURU3_SUCCESS) {
		ABIT_UGURU3_DEBUG("read from 0x%02x:0x%02x timed out after "
			"sending the count, status: 0x%02x\n",
			(unsigned int)bank, (unsigned int)offset, x);
		return -EIO;
	}

	for (i = 0; i < count; i++) {
		x = abituguru3_wait_for_read(data);
		if (x != ABIT_UGURU3_SUCCESS) {
			ABIT_UGURU3_DEBUG("timeout reading byte %d from "
				"0x%02x:0x%02x, status: 0x%02x\n", i,
				(unsigned int)bank, (unsigned int)offset, x);
			break;
		}
		buf[i] = inb(data->addr + ABIT_UGURU3_CMD);
	}
	return i;
}

/*
 * Sensor settings are stored 1 byte per offset with the bytes
 * placed add consecutive offsets.
 */
static int abituguru3_read_increment_offset(struct abituguru3_data *data,
					    u8 bank, u8 offset, u8 count,
					    u8 *buf, int offset_count)
{
	int i, x;

	for (i = 0; i < offset_count; i++) {
		x = abituguru3_read(data, bank, offset + i, count,
				    buf + i * count);
		if (x != count) {
			if (x < 0)
				return x;
			return i * count + x;
		}
	}

	return i * count;
}

/*
 * Following are the sysfs callback functions. These functions expect:
 * sensor_device_attribute_2->index:   index into the data->sensors array
 * sensor_device_attribute_2->nr:      register offset, bitmask or NA.
 */
static struct abituguru3_data *abituguru3_update_device(struct device *dev);

static ssize_t show_value(struct device *dev,
	struct device_attribute *devattr, char *buf)
{
	int value;
	struct sensor_device_attribute_2 *attr = to_sensor_dev_attr_2(devattr);
	struct abituguru3_data *data = abituguru3_update_device(dev);
	const struct abituguru3_sensor_info *sensor;

	if (!data)
		return -EIO;

	sensor = &data->sensors[attr->index];

	/* are we reading a setting, or is this a normal read? */
	if (attr->nr)
		value = data->settings[sensor->port][attr->nr];
	else
		value = data->value[sensor->port];

	/* convert the value */
	value = (value * sensor->multiplier) / sensor->divisor +
		sensor->offset;

	/*
	 * alternatively we could update the sensors settings struct for this,
	 * but then its contents would differ from the windows sw ini files
	 */
	if (sensor->type == ABIT_UGURU3_TEMP_SENSOR)
		value *= 1000;

	return sprintf(buf, "%d\n", value);
}

static ssize_t show_alarm(struct device *dev,
	struct device_attribute *devattr, char *buf)
{
	int port;
	struct sensor_device_attribute_2 *attr = to_sensor_dev_attr_2(devattr);
	struct abituguru3_data *data = abituguru3_update_device(dev);

	if (!data)
		return -EIO;

	port = data->sensors[attr->index].port;

	/*
	 * See if the alarm bit for this sensor is set and if a bitmask is
	 * given in attr->nr also check if the alarm matches the type of alarm
	 * we're looking for (for volt it can be either low or high). The type
	 * is stored in a few readonly bits in the settings of the sensor.
	 */
	if ((data->alarms[port / 8] & (0x01 << (port % 8))) &&
			(!attr->nr || (data->settings[port][0] & attr->nr)))
		return sprintf(buf, "1\n");
	else
		return sprintf(buf, "0\n");
}

static ssize_t show_mask(struct device *dev,
	struct device_attribute *devattr, char *buf)
{
	struct sensor_device_attribute_2 *attr = to_sensor_dev_attr_2(devattr);
	struct abituguru3_data *data = dev_get_drvdata(dev);

	if (data->settings[data->sensors[attr->index].port][0] & attr->nr)
		return sprintf(buf, "1\n");
	else
		return sprintf(buf, "0\n");
}

static ssize_t show_label(struct device *dev,
	struct device_attribute *devattr, char *buf)
{
	struct sensor_device_attribute_2 *attr = to_sensor_dev_attr_2(devattr);
	struct abituguru3_data *data = dev_get_drvdata(dev);

	return sprintf(buf, "%s\n", data->sensors[attr->index].name);
}

static ssize_t show_name(struct device *dev,
	struct device_attribute *devattr, char *buf)
{
	return sprintf(buf, "%s\n", ABIT_UGURU3_NAME);
}

/* Sysfs attr templates, the real entries are generated automatically. */
static const
struct sensor_device_attribute_2 abituguru3_sysfs_templ[3][10] = { {
	SENSOR_ATTR_2(in%d_input, 0444, show_value, NULL, 0, 0),
	SENSOR_ATTR_2(in%d_min, 0444, show_value, NULL, 1, 0),
	SENSOR_ATTR_2(in%d_max, 0444, show_value, NULL, 2, 0),
	SENSOR_ATTR_2(in%d_min_alarm, 0444, show_alarm, NULL,
		ABIT_UGURU3_VOLT_LOW_ALARM_FLAG, 0),
	SENSOR_ATTR_2(in%d_max_alarm, 0444, show_alarm, NULL,
		ABIT_UGURU3_VOLT_HIGH_ALARM_FLAG, 0),
	SENSOR_ATTR_2(in%d_beep, 0444, show_mask, NULL,
		ABIT_UGURU3_BEEP_ENABLE, 0),
	SENSOR_ATTR_2(in%d_shutdown, 0444, show_mask, NULL,
		ABIT_UGURU3_SHUTDOWN_ENABLE, 0),
	SENSOR_ATTR_2(in%d_min_alarm_enable, 0444, show_mask, NULL,
		ABIT_UGURU3_VOLT_LOW_ALARM_ENABLE, 0),
	SENSOR_ATTR_2(in%d_max_alarm_enable, 0444, show_mask, NULL,
		ABIT_UGURU3_VOLT_HIGH_ALARM_ENABLE, 0),
	SENSOR_ATTR_2(in%d_label, 0444, show_label, NULL, 0, 0)
	}, {
	SENSOR_ATTR_2(temp%d_input, 0444, show_value, NULL, 0, 0),
	SENSOR_ATTR_2(temp%d_max, 0444, show_value, NULL, 1, 0),
	SENSOR_ATTR_2(temp%d_crit, 0444, show_value, NULL, 2, 0),
	SENSOR_ATTR_2(temp%d_alarm, 0444, show_alarm, NULL, 0, 0),
	SENSOR_ATTR_2(temp%d_beep, 0444, show_mask, NULL,
		ABIT_UGURU3_BEEP_ENABLE, 0),
	SENSOR_ATTR_2(temp%d_shutdown, 0444, show_mask, NULL,
		ABIT_UGURU3_SHUTDOWN_ENABLE, 0),
	SENSOR_ATTR_2(temp%d_alarm_enable, 0444, show_mask, NULL,
		ABIT_UGURU3_TEMP_HIGH_ALARM_ENABLE, 0),
	SENSOR_ATTR_2(temp%d_label, 0444, show_label, NULL, 0, 0)
	}, {
	SENSOR_ATTR_2(fan%d_input, 0444, show_value, NULL, 0, 0),
	SENSOR_ATTR_2(fan%d_min, 0444, show_value, NULL, 1, 0),
	SENSOR_ATTR_2(fan%d_alarm, 0444, show_alarm, NULL, 0, 0),
	SENSOR_ATTR_2(fan%d_beep, 0444, show_mask, NULL,
		ABIT_UGURU3_BEEP_ENABLE, 0),
	SENSOR_ATTR_2(fan%d_shutdown, 0444, show_mask, NULL,
		ABIT_UGURU3_SHUTDOWN_ENABLE, 0),
	SENSOR_ATTR_2(fan%d_alarm_enable, 0444, show_mask, NULL,
		ABIT_UGURU3_FAN_LOW_ALARM_ENABLE, 0),
	SENSOR_ATTR_2(fan%d_label, 0444, show_label, NULL, 0, 0)
} };

static struct sensor_device_attribute_2 abituguru3_sysfs_attr[] = {
	SENSOR_ATTR_2(name, 0444, show_name, NULL, 0, 0),
};

static int abituguru3_probe(struct platform_device *pdev)
{
	const int no_sysfs_attr[3] = { 10, 8, 7 };
	int sensor_index[3] = { 0, 1, 1 };
	struct abituguru3_data *data;
	int i, j, type, used, sysfs_names_free, sysfs_attr_i, res = -ENODEV;
	char *sysfs_filename;
	u8 buf[2];
	u16 id;

	data = devm_kzalloc(&pdev->dev, sizeof(struct abituguru3_data),
			    GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->addr = platform_get_resource(pdev, IORESOURCE_IO, 0)->start;
	mutex_init(&data->update_lock);
	platform_set_drvdata(pdev, data);

	/* Read the motherboard ID */
	i = abituguru3_read(data, ABIT_UGURU3_MISC_BANK, ABIT_UGURU3_BOARD_ID,
			    2, buf);
	if (i != 2)
		goto abituguru3_probe_error;

	/* Completely read the uGuru to see if one really is there */
	if (!abituguru3_update_device(&pdev->dev))
		goto abituguru3_probe_error;

	/* lookup the ID in our motherboard table */
	id = ((u16)buf[0] << 8) | (u16)buf[1];
	for (i = 0; abituguru3_motherboards[i].id; i++)
		if (abituguru3_motherboards[i].id == id)
			break;
	if (!abituguru3_motherboards[i].id) {
		pr_err("error unknown motherboard ID: %04X. %s\n",
		       (unsigned int)id, report_this);
		goto abituguru3_probe_error;
	}
	data->sensors = abituguru3_motherboards[i].sensors;

	pr_info("found Abit uGuru3, motherboard ID: %04X\n", (unsigned int)id);

	/* Fill the sysfs attr array */
	sysfs_attr_i = 0;
	sysfs_filename = data->sysfs_names;
	sysfs_names_free = ABIT_UGURU3_SYSFS_NAMES_LENGTH;
	for (i = 0; data->sensors[i].name; i++) {
		/* Fail safe check, this should never happen! */
		if (i >= ABIT_UGURU3_MAX_NO_SENSORS) {
			pr_err("Fatal error motherboard has more sensors then ABIT_UGURU3_MAX_NO_SENSORS. %s %s\n",
			       never_happen, report_this);
			res = -ENAMETOOLONG;
			goto abituguru3_probe_error;
		}
		type = data->sensors[i].type;
		for (j = 0; j < no_sysfs_attr[type]; j++) {
			used = snprintf(sysfs_filename, sysfs_names_free,
				abituguru3_sysfs_templ[type][j].dev_attr.attr.
				name, sensor_index[type]) + 1;
			data->sysfs_attr[sysfs_attr_i] =
				abituguru3_sysfs_templ[type][j];
			data->sysfs_attr[sysfs_attr_i].dev_attr.attr.name =
				sysfs_filename;
			data->sysfs_attr[sysfs_attr_i].index = i;
			sysfs_filename += used;
			sysfs_names_free -= used;
			sysfs_attr_i++;
		}
		sensor_index[type]++;
	}
	/* Fail safe check, this should never happen! */
	if (sysfs_names_free < 0) {
		pr_err("Fatal error ran out of space for sysfs attr names. %s %s\n",
		       never_happen, report_this);
		res = -ENAMETOOLONG;
		goto abituguru3_probe_error;
	}

	/* Register sysfs hooks */
	for (i = 0; i < sysfs_attr_i; i++)
		if (device_create_file(&pdev->dev,
				&data->sysfs_attr[i].dev_attr))
			goto abituguru3_probe_error;
	for (i = 0; i < ARRAY_SIZE(abituguru3_sysfs_attr); i++)
		if (device_create_file(&pdev->dev,
				&abituguru3_sysfs_attr[i].dev_attr))
			goto abituguru3_probe_error;

	data->hwmon_dev = hwmon_device_register(&pdev->dev);
	if (IS_ERR(data->hwmon_dev)) {
		res = PTR_ERR(data->hwmon_dev);
		goto abituguru3_probe_error;
	}

	return 0; /* success */

abituguru3_probe_error:
	for (i = 0; data->sysfs_attr[i].dev_attr.attr.name; i++)
		device_remove_file(&pdev->dev, &data->sysfs_attr[i].dev_attr);
	for (i = 0; i < ARRAY_SIZE(abituguru3_sysfs_attr); i++)
		device_remove_file(&pdev->dev,
			&abituguru3_sysfs_attr[i].dev_attr);
	return res;
}

static int abituguru3_remove(struct platform_device *pdev)
{
	int i;
	struct abituguru3_data *data = platform_get_drvdata(pdev);

	hwmon_device_unregister(data->hwmon_dev);
	for (i = 0; data->sysfs_attr[i].dev_attr.attr.name; i++)
		device_remove_file(&pdev->dev, &data->sysfs_attr[i].dev_attr);
	for (i = 0; i < ARRAY_SIZE(abituguru3_sysfs_attr); i++)
		device_remove_file(&pdev->dev,
			&abituguru3_sysfs_attr[i].dev_attr);
	return 0;
}

static struct abituguru3_data *abituguru3_update_device(struct device *dev)
{
	int i;
	struct abituguru3_data *data = dev_get_drvdata(dev);

	mutex_lock(&data->update_lock);
	if (!data->valid || time_after(jiffies, data->last_updated + HZ)) {
		/* Clear data->valid while updating */
		data->valid = 0;
		/* Read alarms */
		if (abituguru3_read_increment_offset(data,
				ABIT_UGURU3_SETTINGS_BANK,
				ABIT_UGURU3_ALARMS_START,
				1, data->alarms, 48/8) != (48/8))
			goto LEAVE_UPDATE;
		/* Read in and temp sensors (3 byte settings / sensor) */
		for (i = 0; i < 32; i++) {
			if (abituguru3_read(data, ABIT_UGURU3_SENSORS_BANK,
					ABIT_UGURU3_VALUES_START + i,
					1, &data->value[i]) != 1)
				goto LEAVE_UPDATE;
			if (abituguru3_read_increment_offset(data,
					ABIT_UGURU3_SETTINGS_BANK,
					ABIT_UGURU3_SETTINGS_START + i * 3,
					1,
					data->settings[i], 3) != 3)
				goto LEAVE_UPDATE;
		}
		/* Read temp sensors (2 byte settings / sensor) */
		for (i = 0; i < 16; i++) {
			if (abituguru3_read(data, ABIT_UGURU3_SENSORS_BANK,
					ABIT_UGURU3_VALUES_START + 32 + i,
					1, &data->value[32 + i]) != 1)
				goto LEAVE_UPDATE;
			if (abituguru3_read_increment_offset(data,
					ABIT_UGURU3_SETTINGS_BANK,
					ABIT_UGURU3_SETTINGS_START + 32 * 3 +
						i * 2, 1,
					data->settings[32 + i], 2) != 2)
				goto LEAVE_UPDATE;
		}
		data->last_updated = jiffies;
		data->valid = 1;
	}
LEAVE_UPDATE:
	mutex_unlock(&data->update_lock);
	if (data->valid)
		return data;
	else
		return NULL;
}

#ifdef CONFIG_PM_SLEEP
static int abituguru3_suspend(struct device *dev)
{
	struct abituguru3_data *data = dev_get_drvdata(dev);
	/*
	 * make sure all communications with the uguru3 are done and no new
	 * ones are started
	 */
	mutex_lock(&data->update_lock);
	return 0;
}

static int abituguru3_resume(struct device *dev)
{
	struct abituguru3_data *data = dev_get_drvdata(dev);
	mutex_unlock(&data->update_lock);
	return 0;
}

static SIMPLE_DEV_PM_OPS(abituguru3_pm, abituguru3_suspend, abituguru3_resume);
#define ABIT_UGURU3_PM	(&abituguru3_pm)
#else
#define ABIT_UGURU3_PM	NULL
#endif /* CONFIG_PM */

static struct platform_driver abituguru3_driver = {
	.driver = {
		.name	= ABIT_UGURU3_NAME,
		.pm	= ABIT_UGURU3_PM
	},
	.probe	= abituguru3_probe,
	.remove	= abituguru3_remove,
};

static int __init abituguru3_dmi_detect(void)
{
	const char *board_vendor, *board_name;
	int i, err = (force) ? 1 : -ENODEV;
	const char *const *dmi_name;
	size_t sublen;

	board_vendor = dmi_get_system_info(DMI_BOARD_VENDOR);
	if (!board_vendor || strcmp(board_vendor, "http://www.abit.com.tw/"))
		return err;

	board_name = dmi_get_system_info(DMI_BOARD_NAME);
	if (!board_name)
		return err;

	/*
	 * At the moment, we don't care about the part of the vendor
	 * DMI string contained in brackets. Truncate the string at
	 * the first occurrence of a bracket. Trim any trailing space
	 * from the substring.
	 */
	sublen = strcspn(board_name, "(");
	while (sublen > 0 && board_name[sublen - 1] == ' ')
		sublen--;

	for (i = 0; abituguru3_motherboards[i].id; i++) {
		dmi_name = abituguru3_motherboards[i].dmi_name;
		for ( ; *dmi_name; dmi_name++) {
			if (strlen(*dmi_name) != sublen)
				continue;
			if (!strncasecmp(board_name, *dmi_name, sublen))
				return 0;
		}
	}

	/* No match found */
	return 1;
}

/*
 * FIXME: Manual detection should die eventually; we need to collect stable
 *        DMI model names first before we can rely entirely on CONFIG_DMI.
 */

static int __init abituguru3_detect(void)
{
	/*
	 * See if there is an uguru3 there. An idle uGuru3 will hold 0x00 or
	 * 0x08 at DATA and 0xAC at CMD. Sometimes the uGuru3 will hold 0x05
	 * or 0x55 at CMD instead, why is unknown.
	 */
	u8 data_val = inb_p(ABIT_UGURU3_BASE + ABIT_UGURU3_DATA);
	u8 cmd_val = inb_p(ABIT_UGURU3_BASE + ABIT_UGURU3_CMD);
	if (((data_val == 0x00) || (data_val == 0x08)) &&
			((cmd_val == 0xAC) || (cmd_val == 0x05) ||
			 (cmd_val == 0x55)))
		return 0;

	ABIT_UGURU3_DEBUG("no Abit uGuru3 found, data = 0x%02X, cmd = "
		"0x%02X\n", (unsigned int)data_val, (unsigned int)cmd_val);

	if (force) {
		pr_info("Assuming Abit uGuru3 is present because of \"force\" parameter\n");
		return 0;
	}

	/* No uGuru3 found */
	return -ENODEV;
}

static struct platform_device *abituguru3_pdev;

static int __init abituguru3_init(void)
{
	struct resource res = { .flags = IORESOURCE_IO };
	int err;

	/* Attempt DMI detection first */
	err = abituguru3_dmi_detect();
	if (err < 0)
		return err;

	/*
	 * Fall back to manual detection if there was no exact
	 * board name match, or force was specified.
	 */
	if (err > 0) {
		err = abituguru3_detect();
		if (err)
			return err;

		pr_warn("this motherboard was not detected using DMI. "
			"Please send the output of \"dmidecode\" to the abituguru3 maintainer (see MAINTAINERS)\n");
	}

	err = platform_driver_register(&abituguru3_driver);
	if (err)
		goto exit;

	abituguru3_pdev = platform_device_alloc(ABIT_UGURU3_NAME,
						ABIT_UGURU3_BASE);
	if (!abituguru3_pdev) {
		pr_err("Device allocation failed\n");
		err = -ENOMEM;
		goto exit_driver_unregister;
	}

	res.start = ABIT_UGURU3_BASE;
	res.end = ABIT_UGURU3_BASE + ABIT_UGURU3_REGION_LENGTH - 1;
	res.name = ABIT_UGURU3_NAME;

	err = platform_device_add_resources(abituguru3_pdev, &res, 1);
	if (err) {
		pr_err("Device resource addition failed (%d)\n", err);
		goto exit_device_put;
	}

	err = platform_device_add(abituguru3_pdev);
	if (err) {
		pr_err("Device addition failed (%d)\n", err);
		goto exit_device_put;
	}

	return 0;

exit_device_put:
	platform_device_put(abituguru3_pdev);
exit_driver_unregister:
	platform_driver_unregister(&abituguru3_driver);
exit:
	return err;
}

static void __exit abituguru3_exit(void)
{
	platform_device_unregister(abituguru3_pdev);
	platform_driver_unregister(&abituguru3_driver);
}

MODULE_AUTHOR("Hans de Goede <hdegoede@redhat.com>");
MODULE_DESCRIPTION("Abit uGuru3 Sensor device");
MODULE_LICENSE("GPL");

module_init(abituguru3_init);
module_exit(abituguru3_exit);
