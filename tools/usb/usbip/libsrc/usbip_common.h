/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2005-2007 Takahiro Hirofuchi
 */

#ifndef __USBIP_COMMON_H
#define __USBIP_COMMON_H

#include <libudev.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <syslog.h>
#include <unistd.h>
#include <linux/usb/ch9.h>
#include <linux/usbip.h>

#ifndef USBIDS_FILE
#define USBIDS_FILE "/usr/share/hwdata/usb.ids"
#endif

#ifndef VHCI_STATE_PATH
#define VHCI_STATE_PATH "/var/run/vhci_hcd"
#endif

#define VUDC_DEVICE_DESCR_FILE "dev_desc"

/* kernel module names */
#define USBIP_CORE_MOD_NAME	"usbip-core"
#define USBIP_HOST_DRV_NAME	"usbip-host"
#define USBIP_DEVICE_DRV_NAME	"usbip-vudc"
#define USBIP_VHCI_DRV_NAME	"vhci_hcd"

/* sysfs constants */
#define SYSFS_MNT_PATH         "/sys"
#define SYSFS_BUS_NAME         "bus"
#define SYSFS_BUS_TYPE         "usb"
#define SYSFS_DRIVERS_NAME     "drivers"

#define SYSFS_PATH_MAX		256
#define SYSFS_BUS_ID_SIZE	32

/* Defines for op_code status in server/client op_common PDUs */
#define ST_OK	0x00
#define ST_NA	0x01
	/* Device requested for import is not available */
#define ST_DEV_BUSY	0x02
	/* Device requested for import is in error state */
#define ST_DEV_ERR	0x03
#define ST_NODEV	0x04
#define ST_ERROR	0x05

extern int usbip_use_syslog;
extern int usbip_use_stderr;
extern int usbip_use_debug ;

#define PROGNAME "usbip"

#define pr_fmt(fmt)	"%s: %s: " fmt "\n", PROGNAME
#define dbg_fmt(fmt)	pr_fmt("%s:%d:[%s] " fmt), "debug",	\
		        __FILE__, __LINE__, __func__

#define err(fmt, args...)						\
	do {								\
		if (usbip_use_syslog) {					\
			syslog(LOG_ERR, pr_fmt(fmt), "error", ##args);	\
		}							\
		if (usbip_use_stderr) {					\
			fprintf(stderr, pr_fmt(fmt), "error", ##args);	\
		}							\
	} while (0)

#define info(fmt, args...)						\
	do {								\
		if (usbip_use_syslog) {					\
			syslog(LOG_INFO, pr_fmt(fmt), "info", ##args);	\
		}							\
		if (usbip_use_stderr) {					\
			fprintf(stderr, pr_fmt(fmt), "info", ##args);	\
		}							\
	} while (0)

#define dbg(fmt, args...)						\
	do {								\
	if (usbip_use_debug) {						\
		if (usbip_use_syslog) {					\
			syslog(LOG_DEBUG, dbg_fmt(fmt), ##args);	\
		}							\
		if (usbip_use_stderr) {					\
			fprintf(stderr, dbg_fmt(fmt), ##args);		\
		}							\
	}								\
	} while (0)

#define BUG()						\
	do {						\
		err("sorry, it's a bug!");		\
		abort();				\
	} while (0)

struct usbip_usb_interface {
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t padding;	/* alignment */
} __attribute__((packed));

struct usbip_usb_device {
	char path[SYSFS_PATH_MAX];
	char busid[SYSFS_BUS_ID_SIZE];

	uint32_t busnum;
	uint32_t devnum;
	uint32_t speed;

	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;

	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bConfigurationValue;
	uint8_t bNumConfigurations;
	uint8_t bNumInterfaces;
} __attribute__((packed));

#define to_string(s)	#s

void dump_usb_interface(struct usbip_usb_interface *);
void dump_usb_device(struct usbip_usb_device *);
int read_usb_device(struct udev_device *sdev, struct usbip_usb_device *udev);
int read_attr_value(struct udev_device *dev, const char *name,
		    const char *format);
int read_usb_interface(struct usbip_usb_device *udev, int i,
		       struct usbip_usb_interface *uinf);

const char *usbip_speed_string(int num);
const char *usbip_status_string(int32_t status);
const char *usbip_op_common_status_string(int status);

int usbip_names_init(char *);
void usbip_names_free(void);
void usbip_names_get_product(char *buff, size_t size, uint16_t vendor,
			     uint16_t product);
void usbip_names_get_class(char *buff, size_t size, uint8_t class,
			   uint8_t subclass, uint8_t protocol);

#endif /* __USBIP_COMMON_H */
