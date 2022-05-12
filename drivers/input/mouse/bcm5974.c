// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Apple USB BCM5974 (Macbook Air and Penryn Macbook Pro) multitouch driver
 *
 * Copyright (C) 2008	   Henrik Rydberg (rydberg@euromail.se)
 * Copyright (C) 2015      John Horan (knasher@gmail.com)
 *
 * The USB initialization and package decoding was made by
 * Scott Shawcroft as part of the touchd user-space driver project:
 * Copyright (C) 2008	   Scott Shawcroft (scott.shawcroft@gmail.com)
 *
 * The BCM5974 driver is based on the appletouch driver:
 * Copyright (C) 2001-2004 Greg Kroah-Hartman (greg@kroah.com)
 * Copyright (C) 2005      Johannes Berg (johannes@sipsolutions.net)
 * Copyright (C) 2005	   Stelian Pop (stelian@popies.net)
 * Copyright (C) 2005	   Frank Arnold (frank@scirocco-5v-turbo.de)
 * Copyright (C) 2005	   Peter Osterlund (petero2@telia.com)
 * Copyright (C) 2005	   Michael Hanselmann (linux-kernel@hansmi.ch)
 * Copyright (C) 2006	   Nicolas Boichat (nicolas@boichat.ch)
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/usb/input.h>
#include <linux/hid.h>
#include <linux/mutex.h>
#include <linux/input/mt.h>

#define USB_VENDOR_ID_APPLE		0x05ac

/* MacbookAir, aka wellspring */
#define USB_DEVICE_ID_APPLE_WELLSPRING_ANSI	0x0223
#define USB_DEVICE_ID_APPLE_WELLSPRING_ISO	0x0224
#define USB_DEVICE_ID_APPLE_WELLSPRING_JIS	0x0225
/* MacbookProPenryn, aka wellspring2 */
#define USB_DEVICE_ID_APPLE_WELLSPRING2_ANSI	0x0230
#define USB_DEVICE_ID_APPLE_WELLSPRING2_ISO	0x0231
#define USB_DEVICE_ID_APPLE_WELLSPRING2_JIS	0x0232
/* Macbook5,1 (unibody), aka wellspring3 */
#define USB_DEVICE_ID_APPLE_WELLSPRING3_ANSI	0x0236
#define USB_DEVICE_ID_APPLE_WELLSPRING3_ISO	0x0237
#define USB_DEVICE_ID_APPLE_WELLSPRING3_JIS	0x0238
/* MacbookAir3,2 (unibody), aka wellspring5 */
#define USB_DEVICE_ID_APPLE_WELLSPRING4_ANSI	0x023f
#define USB_DEVICE_ID_APPLE_WELLSPRING4_ISO	0x0240
#define USB_DEVICE_ID_APPLE_WELLSPRING4_JIS	0x0241
/* MacbookAir3,1 (unibody), aka wellspring4 */
#define USB_DEVICE_ID_APPLE_WELLSPRING4A_ANSI	0x0242
#define USB_DEVICE_ID_APPLE_WELLSPRING4A_ISO	0x0243
#define USB_DEVICE_ID_APPLE_WELLSPRING4A_JIS	0x0244
/* Macbook8 (unibody, March 2011) */
#define USB_DEVICE_ID_APPLE_WELLSPRING5_ANSI	0x0245
#define USB_DEVICE_ID_APPLE_WELLSPRING5_ISO	0x0246
#define USB_DEVICE_ID_APPLE_WELLSPRING5_JIS	0x0247
/* MacbookAir4,1 (unibody, July 2011) */
#define USB_DEVICE_ID_APPLE_WELLSPRING6A_ANSI	0x0249
#define USB_DEVICE_ID_APPLE_WELLSPRING6A_ISO	0x024a
#define USB_DEVICE_ID_APPLE_WELLSPRING6A_JIS	0x024b
/* MacbookAir4,2 (unibody, July 2011) */
#define USB_DEVICE_ID_APPLE_WELLSPRING6_ANSI	0x024c
#define USB_DEVICE_ID_APPLE_WELLSPRING6_ISO	0x024d
#define USB_DEVICE_ID_APPLE_WELLSPRING6_JIS	0x024e
/* Macbook8,2 (unibody) */
#define USB_DEVICE_ID_APPLE_WELLSPRING5A_ANSI	0x0252
#define USB_DEVICE_ID_APPLE_WELLSPRING5A_ISO	0x0253
#define USB_DEVICE_ID_APPLE_WELLSPRING5A_JIS	0x0254
/* MacbookPro10,1 (unibody, June 2012) */
#define USB_DEVICE_ID_APPLE_WELLSPRING7_ANSI	0x0262
#define USB_DEVICE_ID_APPLE_WELLSPRING7_ISO	0x0263
#define USB_DEVICE_ID_APPLE_WELLSPRING7_JIS	0x0264
/* MacbookPro10,2 (unibody, October 2012) */
#define USB_DEVICE_ID_APPLE_WELLSPRING7A_ANSI	0x0259
#define USB_DEVICE_ID_APPLE_WELLSPRING7A_ISO	0x025a
#define USB_DEVICE_ID_APPLE_WELLSPRING7A_JIS	0x025b
/* MacbookAir6,2 (unibody, June 2013) */
#define USB_DEVICE_ID_APPLE_WELLSPRING8_ANSI	0x0290
#define USB_DEVICE_ID_APPLE_WELLSPRING8_ISO	0x0291
#define USB_DEVICE_ID_APPLE_WELLSPRING8_JIS	0x0292
/* MacbookPro12,1 (2015) */
#define USB_DEVICE_ID_APPLE_WELLSPRING9_ANSI	0x0272
#define USB_DEVICE_ID_APPLE_WELLSPRING9_ISO	0x0273
#define USB_DEVICE_ID_APPLE_WELLSPRING9_JIS	0x0274

#define BCM5974_DEVICE(prod) {					\
	.match_flags = (USB_DEVICE_ID_MATCH_DEVICE |		\
			USB_DEVICE_ID_MATCH_INT_CLASS |		\
			USB_DEVICE_ID_MATCH_INT_PROTOCOL),	\
	.idVendor = USB_VENDOR_ID_APPLE,			\
	.idProduct = (prod),					\
	.bInterfaceClass = USB_INTERFACE_CLASS_HID,		\
	.bInterfaceProtocol = USB_INTERFACE_PROTOCOL_MOUSE	\
}

/* table of devices that work with this driver */
static const struct usb_device_id bcm5974_table[] = {
	/* MacbookAir1.1 */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING_JIS),
	/* MacbookProPenryn */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING2_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING2_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING2_JIS),
	/* Macbook5,1 */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING3_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING3_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING3_JIS),
	/* MacbookAir3,2 */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING4_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING4_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING4_JIS),
	/* MacbookAir3,1 */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING4A_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING4A_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING4A_JIS),
	/* MacbookPro8 */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING5_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING5_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING5_JIS),
	/* MacbookAir4,1 */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING6A_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING6A_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING6A_JIS),
	/* MacbookAir4,2 */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING6_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING6_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING6_JIS),
	/* MacbookPro8,2 */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING5A_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING5A_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING5A_JIS),
	/* MacbookPro10,1 */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING7_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING7_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING7_JIS),
	/* MacbookPro10,2 */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING7A_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING7A_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING7A_JIS),
	/* MacbookAir6,2 */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING8_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING8_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING8_JIS),
	/* MacbookPro12,1 */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING9_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING9_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING9_JIS),
	/* Terminating entry */
	{}
};
MODULE_DEVICE_TABLE(usb, bcm5974_table);

MODULE_AUTHOR("Henrik Rydberg");
MODULE_DESCRIPTION("Apple USB BCM5974 multitouch driver");
MODULE_LICENSE("GPL");

#define dprintk(level, format, a...)\
	{ if (debug >= level) printk(KERN_DEBUG format, ##a); }

static int debug = 1;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "Activate debugging output");

/* button data structure */
struct bt_data {
	u8 unknown1;		/* constant */
	u8 button;		/* left button */
	u8 rel_x;		/* relative x coordinate */
	u8 rel_y;		/* relative y coordinate */
};

/* trackpad header types */
enum tp_type {
	TYPE1,			/* plain trackpad */
	TYPE2,			/* button integrated in trackpad */
	TYPE3,			/* additional header fields since June 2013 */
	TYPE4			/* additional header field for pressure data */
};

/* trackpad finger data offsets, le16-aligned */
#define HEADER_TYPE1		(13 * sizeof(__le16))
#define HEADER_TYPE2		(15 * sizeof(__le16))
#define HEADER_TYPE3		(19 * sizeof(__le16))
#define HEADER_TYPE4		(23 * sizeof(__le16))

/* trackpad button data offsets */
#define BUTTON_TYPE1		0
#define BUTTON_TYPE2		15
#define BUTTON_TYPE3		23
#define BUTTON_TYPE4		31

/* list of device capability bits */
#define HAS_INTEGRATED_BUTTON	1

/* trackpad finger data block size */
#define FSIZE_TYPE1		(14 * sizeof(__le16))
#define FSIZE_TYPE2		(14 * sizeof(__le16))
#define FSIZE_TYPE3		(14 * sizeof(__le16))
#define FSIZE_TYPE4		(15 * sizeof(__le16))

/* offset from header to finger struct */
#define DELTA_TYPE1		(0 * sizeof(__le16))
#define DELTA_TYPE2		(0 * sizeof(__le16))
#define DELTA_TYPE3		(0 * sizeof(__le16))
#define DELTA_TYPE4		(1 * sizeof(__le16))

/* usb control message mode switch data */
#define USBMSG_TYPE1		8, 0x300, 0, 0, 0x1, 0x8
#define USBMSG_TYPE2		8, 0x300, 0, 0, 0x1, 0x8
#define USBMSG_TYPE3		8, 0x300, 0, 0, 0x1, 0x8
#define USBMSG_TYPE4		2, 0x302, 2, 1, 0x1, 0x0

/* Wellspring initialization constants */
#define BCM5974_WELLSPRING_MODE_READ_REQUEST_ID		1
#define BCM5974_WELLSPRING_MODE_WRITE_REQUEST_ID	9

/* trackpad finger structure, le16-aligned */
struct tp_finger {
	__le16 origin;		/* zero when switching track finger */
	__le16 abs_x;		/* absolute x coodinate */
	__le16 abs_y;		/* absolute y coodinate */
	__le16 rel_x;		/* relative x coodinate */
	__le16 rel_y;		/* relative y coodinate */
	__le16 tool_major;	/* tool area, major axis */
	__le16 tool_minor;	/* tool area, minor axis */
	__le16 orientation;	/* 16384 when point, else 15 bit angle */
	__le16 touch_major;	/* touch area, major axis */
	__le16 touch_minor;	/* touch area, minor axis */
	__le16 unused[2];	/* zeros */
	__le16 pressure;	/* pressure on forcetouch touchpad */
	__le16 multi;		/* one finger: varies, more fingers: constant */
} __attribute__((packed,aligned(2)));

/* trackpad finger data size, empirically at least ten fingers */
#define MAX_FINGERS		16
#define MAX_FINGER_ORIENTATION	16384

/* device-specific parameters */
struct bcm5974_param {
	int snratio;		/* signal-to-noise ratio */
	int min;		/* device minimum reading */
	int max;		/* device maximum reading */
};

/* device-specific configuration */
struct bcm5974_config {
	int ansi, iso, jis;	/* the product id of this device */
	int caps;		/* device capability bitmask */
	int bt_ep;		/* the endpoint of the button interface */
	int bt_datalen;		/* data length of the button interface */
	int tp_ep;		/* the endpoint of the trackpad interface */
	enum tp_type tp_type;	/* type of trackpad interface */
	int tp_header;		/* bytes in header block */
	int tp_datalen;		/* data length of the trackpad interface */
	int tp_button;		/* offset to button data */
	int tp_fsize;		/* bytes in single finger block */
	int tp_delta;		/* offset from header to finger struct */
	int um_size;		/* usb control message length */
	int um_req_val;		/* usb control message value */
	int um_req_idx;		/* usb control message index */
	int um_switch_idx;	/* usb control message mode switch index */
	int um_switch_on;	/* usb control message mode switch on */
	int um_switch_off;	/* usb control message mode switch off */
	struct bcm5974_param p;	/* finger pressure limits */
	struct bcm5974_param w;	/* finger width limits */
	struct bcm5974_param x;	/* horizontal limits */
	struct bcm5974_param y;	/* vertical limits */
	struct bcm5974_param o;	/* orientation limits */
};

/* logical device structure */
struct bcm5974 {
	char phys[64];
	struct usb_device *udev;	/* usb device */
	struct usb_interface *intf;	/* our interface */
	struct input_dev *input;	/* input dev */
	struct bcm5974_config cfg;	/* device configuration */
	struct mutex pm_mutex;		/* serialize access to open/suspend */
	int opened;			/* 1: opened, 0: closed */
	struct urb *bt_urb;		/* button usb request block */
	struct bt_data *bt_data;	/* button transferred data */
	struct urb *tp_urb;		/* trackpad usb request block */
	u8 *tp_data;			/* trackpad transferred data */
	const struct tp_finger *index[MAX_FINGERS];	/* finger index data */
	struct input_mt_pos pos[MAX_FINGERS];		/* position array */
	int slots[MAX_FINGERS];				/* slot assignments */
};

/* trackpad finger block data, le16-aligned */
static const struct tp_finger *get_tp_finger(const struct bcm5974 *dev, int i)
{
	const struct bcm5974_config *c = &dev->cfg;
	u8 *f_base = dev->tp_data + c->tp_header + c->tp_delta;

	return (const struct tp_finger *)(f_base + i * c->tp_fsize);
}

#define DATAFORMAT(type)				\
	type,						\
	HEADER_##type,					\
	HEADER_##type + (MAX_FINGERS) * (FSIZE_##type),	\
	BUTTON_##type,					\
	FSIZE_##type,					\
	DELTA_##type,					\
	USBMSG_##type

/* logical signal quality */
#define SN_PRESSURE	45		/* pressure signal-to-noise ratio */
#define SN_WIDTH	25		/* width signal-to-noise ratio */
#define SN_COORD	250		/* coordinate signal-to-noise ratio */
#define SN_ORIENT	10		/* orientation signal-to-noise ratio */

/* device constants */
static const struct bcm5974_config bcm5974_config_table[] = {
	{
		USB_DEVICE_ID_APPLE_WELLSPRING_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING_JIS,
		0,
		0x84, sizeof(struct bt_data),
		0x81, DATAFORMAT(TYPE1),
		{ SN_PRESSURE, 0, 256 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4824, 5342 },
		{ SN_COORD, -172, 5820 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING2_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING2_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING2_JIS,
		0,
		0x84, sizeof(struct bt_data),
		0x81, DATAFORMAT(TYPE1),
		{ SN_PRESSURE, 0, 256 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4824, 4824 },
		{ SN_COORD, -172, 4290 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING3_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING3_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING3_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4460, 5166 },
		{ SN_COORD, -75, 6700 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING4_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING4_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING4_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4620, 5140 },
		{ SN_COORD, -150, 6600 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING4A_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING4A_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING4A_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4616, 5112 },
		{ SN_COORD, -142, 5234 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING5_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING5_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING5_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4415, 5050 },
		{ SN_COORD, -55, 6680 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING6_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING6_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING6_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4620, 5140 },
		{ SN_COORD, -150, 6600 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING5A_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING5A_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING5A_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4750, 5280 },
		{ SN_COORD, -150, 6730 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING6A_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING6A_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING6A_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4620, 5140 },
		{ SN_COORD, -150, 6600 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING7_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING7_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING7_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4750, 5280 },
		{ SN_COORD, -150, 6730 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING7A_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING7A_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING7A_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4750, 5280 },
		{ SN_COORD, -150, 6730 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING8_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING8_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING8_JIS,
		HAS_INTEGRATED_BUTTON,
		0, sizeof(struct bt_data),
		0x83, DATAFORMAT(TYPE3),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4620, 5140 },
		{ SN_COORD, -150, 6600 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING9_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING9_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING9_JIS,
		HAS_INTEGRATED_BUTTON,
		0, sizeof(struct bt_data),
		0x83, DATAFORMAT(TYPE4),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4828, 5345 },
		{ SN_COORD, -203, 6803 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{}
};

/* return the device-specific configuration by device */
static const struct bcm5974_config *bcm5974_get_config(struct usb_device *udev)
{
	u16 id = le16_to_cpu(udev->descriptor.idProduct);
	const struct bcm5974_config *cfg;

	for (cfg = bcm5974_config_table; cfg->ansi; ++cfg)
		if (cfg->ansi == id || cfg->iso == id || cfg->jis == id)
			return cfg;

	return bcm5974_config_table;
}

/* convert 16-bit little endian to signed integer */
static inline int raw2int(__le16 x)
{
	return (signed short)le16_to_cpu(x);
}

static void set_abs(struct input_dev *input, unsigned int code,
		    const struct bcm5974_param *p)
{
	int fuzz = p->snratio ? (p->max - p->min) / p->snratio : 0;
	input_set_abs_params(input, code, p->min, p->max, fuzz, 0);
}

/* setup which logical events to report */
static void setup_events_to_report(struct input_dev *input_dev,
				   const struct bcm5974_config *cfg)
{
	__set_bit(EV_ABS, input_dev->evbit);

	/* for synaptics only */
	input_set_abs_params(input_dev, ABS_PRESSURE, 0, 256, 5, 0);
	input_set_abs_params(input_dev, ABS_TOOL_WIDTH, 0, 16, 0, 0);

	/* finger touch area */
	set_abs(input_dev, ABS_MT_TOUCH_MAJOR, &cfg->w);
	set_abs(input_dev, ABS_MT_TOUCH_MINOR, &cfg->w);
	/* finger approach area */
	set_abs(input_dev, ABS_MT_WIDTH_MAJOR, &cfg->w);
	set_abs(input_dev, ABS_MT_WIDTH_MINOR, &cfg->w);
	/* finger orientation */
	set_abs(input_dev, ABS_MT_ORIENTATION, &cfg->o);
	/* finger position */
	set_abs(input_dev, ABS_MT_POSITION_X, &cfg->x);
	set_abs(input_dev, ABS_MT_POSITION_Y, &cfg->y);

	__set_bit(EV_KEY, input_dev->evbit);
	__set_bit(BTN_LEFT, input_dev->keybit);

	if (cfg->caps & HAS_INTEGRATED_BUTTON)
		__set_bit(INPUT_PROP_BUTTONPAD, input_dev->propbit);

	input_mt_init_slots(input_dev, MAX_FINGERS,
		INPUT_MT_POINTER | INPUT_MT_DROP_UNUSED | INPUT_MT_TRACK);
}

/* report button data as logical button state */
static int report_bt_state(struct bcm5974 *dev, int size)
{
	if (size != sizeof(struct bt_data))
		return -EIO;

	dprintk(7,
		"bcm5974: button data: %x %x %x %x\n",
		dev->bt_data->unknown1, dev->bt_data->button,
		dev->bt_data->rel_x, dev->bt_data->rel_y);

	input_report_key(dev->input, BTN_LEFT, dev->bt_data->button);
	input_sync(dev->input);

	return 0;
}

static void report_finger_data(struct input_dev *input, int slot,
			       const struct input_mt_pos *pos,
			       const struct tp_finger *f)
{
	input_mt_slot(input, slot);
	input_mt_report_slot_state(input, MT_TOOL_FINGER, true);

	input_report_abs(input, ABS_MT_TOUCH_MAJOR,
			 raw2int(f->touch_major) << 1);
	input_report_abs(input, ABS_MT_TOUCH_MINOR,
			 raw2int(f->touch_minor) << 1);
	input_report_abs(input, ABS_MT_WIDTH_MAJOR,
			 raw2int(f->tool_major) << 1);
	input_report_abs(input, ABS_MT_WIDTH_MINOR,
			 raw2int(f->tool_minor) << 1);
	input_report_abs(input, ABS_MT_ORIENTATION,
			 MAX_FINGER_ORIENTATION - raw2int(f->orientation));
	input_report_abs(input, ABS_MT_POSITION_X, pos->x);
	input_report_abs(input, ABS_MT_POSITION_Y, pos->y);
}

static void report_synaptics_data(struct input_dev *input,
				  const struct bcm5974_config *cfg,
				  const struct tp_finger *f, int raw_n)
{
	int abs_p = 0, abs_w = 0;

	if (raw_n) {
		int p = raw2int(f->touch_major);
		int w = raw2int(f->tool_major);
		if (p > 0 && raw2int(f->origin)) {
			abs_p = clamp_val(256 * p / cfg->p.max, 0, 255);
			abs_w = clamp_val(16 * w / cfg->w.max, 0, 15);
		}
	}

	input_report_abs(input, ABS_PRESSURE, abs_p);
	input_report_abs(input, ABS_TOOL_WIDTH, abs_w);
}

/* report trackpad data as logical trackpad state */
static int report_tp_state(struct bcm5974 *dev, int size)
{
	const struct bcm5974_config *c = &dev->cfg;
	const struct tp_finger *f;
	struct input_dev *input = dev->input;
	int raw_n, i, n = 0;

	if (size < c->tp_header || (size - c->tp_header) % c->tp_fsize != 0)
		return -EIO;

	raw_n = (size - c->tp_header) / c->tp_fsize;

	for (i = 0; i < raw_n; i++) {
		f = get_tp_finger(dev, i);
		if (raw2int(f->touch_major) == 0)
			continue;
		dev->pos[n].x = raw2int(f->abs_x);
		dev->pos[n].y = c->y.min + c->y.max - raw2int(f->abs_y);
		dev->index[n++] = f;
	}

	input_mt_assign_slots(input, dev->slots, dev->pos, n, 0);

	for (i = 0; i < n; i++)
		report_finger_data(input, dev->slots[i],
				   &dev->pos[i], dev->index[i]);

	input_mt_sync_frame(input);

	report_synaptics_data(input, c, get_tp_finger(dev, 0), raw_n);

	/* later types report button events via integrated button only */
	if (c->caps & HAS_INTEGRATED_BUTTON) {
		int ibt = raw2int(dev->tp_data[c->tp_button]);
		input_report_key(input, BTN_LEFT, ibt);
	}

	input_sync(input);

	return 0;
}

static int bcm5974_wellspring_mode(struct bcm5974 *dev, bool on)
{
	const struct bcm5974_config *c = &dev->cfg;
	int retval = 0, size;
	char *data;

	/* Type 3 does not require a mode switch */
	if (c->tp_type == TYPE3)
		return 0;

	data = kmalloc(c->um_size, GFP_KERNEL);
	if (!data) {
		dev_err(&dev->intf->dev, "out of memory\n");
		retval = -ENOMEM;
		goto out;
	}

	/* read configuration */
	size = usb_control_msg(dev->udev, usb_rcvctrlpipe(dev->udev, 0),
			BCM5974_WELLSPRING_MODE_READ_REQUEST_ID,
			USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
			c->um_req_val, c->um_req_idx, data, c->um_size, 5000);

	if (size != c->um_size) {
		dev_err(&dev->intf->dev, "could not read from device\n");
		retval = -EIO;
		goto out;
	}

	/* apply the mode switch */
	data[c->um_switch_idx] = on ? c->um_switch_on : c->um_switch_off;

	/* write configuration */
	size = usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
			BCM5974_WELLSPRING_MODE_WRITE_REQUEST_ID,
			USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
			c->um_req_val, c->um_req_idx, data, c->um_size, 5000);

	if (size != c->um_size) {
		dev_err(&dev->intf->dev, "could not write to device\n");
		retval = -EIO;
		goto out;
	}

	dprintk(2, "bcm5974: switched to %s mode.\n",
		on ? "wellspring" : "normal");

 out:
	kfree(data);
	return retval;
}

static void bcm5974_irq_button(struct urb *urb)
{
	struct bcm5974 *dev = urb->context;
	struct usb_interface *intf = dev->intf;
	int error;

	switch (urb->status) {
	case 0:
		break;
	case -EOVERFLOW:
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		dev_dbg(&intf->dev, "button urb shutting down: %d\n",
			urb->status);
		return;
	default:
		dev_dbg(&intf->dev, "button urb status: %d\n", urb->status);
		goto exit;
	}

	if (report_bt_state(dev, dev->bt_urb->actual_length))
		dprintk(1, "bcm5974: bad button package, length: %d\n",
			dev->bt_urb->actual_length);

exit:
	error = usb_submit_urb(dev->bt_urb, GFP_ATOMIC);
	if (error)
		dev_err(&intf->dev, "button urb failed: %d\n", error);
}

static void bcm5974_irq_trackpad(struct urb *urb)
{
	struct bcm5974 *dev = urb->context;
	struct usb_interface *intf = dev->intf;
	int error;

	switch (urb->status) {
	case 0:
		break;
	case -EOVERFLOW:
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		dev_dbg(&intf->dev, "trackpad urb shutting down: %d\n",
			urb->status);
		return;
	default:
		dev_dbg(&intf->dev, "trackpad urb status: %d\n", urb->status);
		goto exit;
	}

	/* control response ignored */
	if (dev->tp_urb->actual_length == 2)
		goto exit;

	if (report_tp_state(dev, dev->tp_urb->actual_length))
		dprintk(1, "bcm5974: bad trackpad package, length: %d\n",
			dev->tp_urb->actual_length);

exit:
	error = usb_submit_urb(dev->tp_urb, GFP_ATOMIC);
	if (error)
		dev_err(&intf->dev, "trackpad urb failed: %d\n", error);
}

/*
 * The Wellspring trackpad, like many recent Apple trackpads, share
 * the usb device with the keyboard. Since keyboards are usually
 * handled by the HID system, the device ends up being handled by two
 * modules. Setting up the device therefore becomes slightly
 * complicated. To enable multitouch features, a mode switch is
 * required, which is usually applied via the control interface of the
 * device.  It can be argued where this switch should take place. In
 * some drivers, like appletouch, the switch is made during
 * probe. However, the hid module may also alter the state of the
 * device, resulting in trackpad malfunction under certain
 * circumstances. To get around this problem, there is at least one
 * example that utilizes the USB_QUIRK_RESET_RESUME quirk in order to
 * receive a reset_resume request rather than the normal resume.
 * Since the implementation of reset_resume is equal to mode switch
 * plus start_traffic, it seems easier to always do the switch when
 * starting traffic on the device.
 */
static int bcm5974_start_traffic(struct bcm5974 *dev)
{
	int error;

	error = bcm5974_wellspring_mode(dev, true);
	if (error) {
		dprintk(1, "bcm5974: mode switch failed\n");
		goto err_out;
	}

	if (dev->bt_urb) {
		error = usb_submit_urb(dev->bt_urb, GFP_KERNEL);
		if (error)
			goto err_reset_mode;
	}

	error = usb_submit_urb(dev->tp_urb, GFP_KERNEL);
	if (error)
		goto err_kill_bt;

	return 0;

err_kill_bt:
	usb_kill_urb(dev->bt_urb);
err_reset_mode:
	bcm5974_wellspring_mode(dev, false);
err_out:
	return error;
}

static void bcm5974_pause_traffic(struct bcm5974 *dev)
{
	usb_kill_urb(dev->tp_urb);
	usb_kill_urb(dev->bt_urb);
	bcm5974_wellspring_mode(dev, false);
}

/*
 * The code below implements open/close and manual suspend/resume.
 * All functions may be called in random order.
 *
 * Opening a suspended device fails with EACCES - permission denied.
 *
 * Failing a resume leaves the device resumed but closed.
 */
static int bcm5974_open(struct input_dev *input)
{
	struct bcm5974 *dev = input_get_drvdata(input);
	int error;

	error = usb_autopm_get_interface(dev->intf);
	if (error)
		return error;

	mutex_lock(&dev->pm_mutex);

	error = bcm5974_start_traffic(dev);
	if (!error)
		dev->opened = 1;

	mutex_unlock(&dev->pm_mutex);

	if (error)
		usb_autopm_put_interface(dev->intf);

	return error;
}

static void bcm5974_close(struct input_dev *input)
{
	struct bcm5974 *dev = input_get_drvdata(input);

	mutex_lock(&dev->pm_mutex);

	bcm5974_pause_traffic(dev);
	dev->opened = 0;

	mutex_unlock(&dev->pm_mutex);

	usb_autopm_put_interface(dev->intf);
}

static int bcm5974_suspend(struct usb_interface *iface, pm_message_t message)
{
	struct bcm5974 *dev = usb_get_intfdata(iface);

	mutex_lock(&dev->pm_mutex);

	if (dev->opened)
		bcm5974_pause_traffic(dev);

	mutex_unlock(&dev->pm_mutex);

	return 0;
}

static int bcm5974_resume(struct usb_interface *iface)
{
	struct bcm5974 *dev = usb_get_intfdata(iface);
	int error = 0;

	mutex_lock(&dev->pm_mutex);

	if (dev->opened)
		error = bcm5974_start_traffic(dev);

	mutex_unlock(&dev->pm_mutex);

	return error;
}

static int bcm5974_probe(struct usb_interface *iface,
			 const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(iface);
	const struct bcm5974_config *cfg;
	struct bcm5974 *dev;
	struct input_dev *input_dev;
	int error = -ENOMEM;

	/* find the product index */
	cfg = bcm5974_get_config(udev);

	/* allocate memory for our device state and initialize it */
	dev = kzalloc(sizeof(struct bcm5974), GFP_KERNEL);
	input_dev = input_allocate_device();
	if (!dev || !input_dev) {
		dev_err(&iface->dev, "out of memory\n");
		goto err_free_devs;
	}

	dev->udev = udev;
	dev->intf = iface;
	dev->input = input_dev;
	dev->cfg = *cfg;
	mutex_init(&dev->pm_mutex);

	/* setup urbs */
	if (cfg->tp_type == TYPE1) {
		dev->bt_urb = usb_alloc_urb(0, GFP_KERNEL);
		if (!dev->bt_urb)
			goto err_free_devs;
	}

	dev->tp_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!dev->tp_urb)
		goto err_free_bt_urb;

	if (dev->bt_urb) {
		dev->bt_data = usb_alloc_coherent(dev->udev,
					  dev->cfg.bt_datalen, GFP_KERNEL,
					  &dev->bt_urb->transfer_dma);
		if (!dev->bt_data)
			goto err_free_urb;
	}

	dev->tp_data = usb_alloc_coherent(dev->udev,
					  dev->cfg.tp_datalen, GFP_KERNEL,
					  &dev->tp_urb->transfer_dma);
	if (!dev->tp_data)
		goto err_free_bt_buffer;

	if (dev->bt_urb)
		usb_fill_int_urb(dev->bt_urb, udev,
				 usb_rcvintpipe(udev, cfg->bt_ep),
				 dev->bt_data, dev->cfg.bt_datalen,
				 bcm5974_irq_button, dev, 1);

	usb_fill_int_urb(dev->tp_urb, udev,
			 usb_rcvintpipe(udev, cfg->tp_ep),
			 dev->tp_data, dev->cfg.tp_datalen,
			 bcm5974_irq_trackpad, dev, 1);

	/* create bcm5974 device */
	usb_make_path(udev, dev->phys, sizeof(dev->phys));
	strlcat(dev->phys, "/input0", sizeof(dev->phys));

	input_dev->name = "bcm5974";
	input_dev->phys = dev->phys;
	usb_to_input_id(dev->udev, &input_dev->id);
	/* report driver capabilities via the version field */
	input_dev->id.version = cfg->caps;
	input_dev->dev.parent = &iface->dev;

	input_set_drvdata(input_dev, dev);

	input_dev->open = bcm5974_open;
	input_dev->close = bcm5974_close;

	setup_events_to_report(input_dev, cfg);

	error = input_register_device(dev->input);
	if (error)
		goto err_free_buffer;

	/* save our data pointer in this interface device */
	usb_set_intfdata(iface, dev);

	return 0;

err_free_buffer:
	usb_free_coherent(dev->udev, dev->cfg.tp_datalen,
		dev->tp_data, dev->tp_urb->transfer_dma);
err_free_bt_buffer:
	if (dev->bt_urb)
		usb_free_coherent(dev->udev, dev->cfg.bt_datalen,
				  dev->bt_data, dev->bt_urb->transfer_dma);
err_free_urb:
	usb_free_urb(dev->tp_urb);
err_free_bt_urb:
	usb_free_urb(dev->bt_urb);
err_free_devs:
	usb_set_intfdata(iface, NULL);
	input_free_device(input_dev);
	kfree(dev);
	return error;
}

static void bcm5974_disconnect(struct usb_interface *iface)
{
	struct bcm5974 *dev = usb_get_intfdata(iface);

	usb_set_intfdata(iface, NULL);

	input_unregister_device(dev->input);
	usb_free_coherent(dev->udev, dev->cfg.tp_datalen,
			  dev->tp_data, dev->tp_urb->transfer_dma);
	if (dev->bt_urb)
		usb_free_coherent(dev->udev, dev->cfg.bt_datalen,
				  dev->bt_data, dev->bt_urb->transfer_dma);
	usb_free_urb(dev->tp_urb);
	usb_free_urb(dev->bt_urb);
	kfree(dev);
}

static struct usb_driver bcm5974_driver = {
	.name			= "bcm5974",
	.probe			= bcm5974_probe,
	.disconnect		= bcm5974_disconnect,
	.suspend		= bcm5974_suspend,
	.resume			= bcm5974_resume,
	.id_table		= bcm5974_table,
	.supports_autosuspend	= 1,
};

module_usb_driver(bcm5974_driver);
