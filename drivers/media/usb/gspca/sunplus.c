// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *		Sunplus spca504(abc) spca533 spca536 library
 *		Copyright (C) 2005 Michel Xhaard mxhaard@magic.fr
 *
 * V4L2 by Jean-Francois Moine <http://moinejf.free.fr>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#define MODULE_NAME "sunplus"

#include "gspca.h"
#include "jpeg.h"

MODULE_AUTHOR("Michel Xhaard <mxhaard@users.sourceforge.net>");
MODULE_DESCRIPTION("GSPCA/SPCA5xx USB Camera Driver");
MODULE_LICENSE("GPL");

#define QUALITY 85

/* specific webcam descriptor */
struct sd {
	struct gspca_dev gspca_dev;	/* !! must be the first item */

	bool autogain;

	u8 bridge;
#define BRIDGE_SPCA504 0
#define BRIDGE_SPCA504B 1
#define BRIDGE_SPCA504C 2
#define BRIDGE_SPCA533 3
#define BRIDGE_SPCA536 4
	u8 subtype;
#define AiptekMiniPenCam13 1
#define LogitechClickSmart420 2
#define LogitechClickSmart820 3
#define MegapixV4 4
#define MegaImageVI 5

	u8 jpeg_hdr[JPEG_HDR_SZ];
};

static const struct v4l2_pix_format vga_mode[] = {
	{320, 240, V4L2_PIX_FMT_JPEG, V4L2_FIELD_NONE,
		.bytesperline = 320,
		.sizeimage = 320 * 240 * 3 / 8 + 590,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.priv = 2},
	{640, 480, V4L2_PIX_FMT_JPEG, V4L2_FIELD_NONE,
		.bytesperline = 640,
		.sizeimage = 640 * 480 * 3 / 8 + 590,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.priv = 1},
};

static const struct v4l2_pix_format custom_mode[] = {
	{320, 240, V4L2_PIX_FMT_JPEG, V4L2_FIELD_NONE,
		.bytesperline = 320,
		.sizeimage = 320 * 240 * 3 / 8 + 590,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.priv = 2},
	{464, 480, V4L2_PIX_FMT_JPEG, V4L2_FIELD_NONE,
		.bytesperline = 464,
		.sizeimage = 464 * 480 * 3 / 8 + 590,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.priv = 1},
};

static const struct v4l2_pix_format vga_mode2[] = {
	{176, 144, V4L2_PIX_FMT_JPEG, V4L2_FIELD_NONE,
		.bytesperline = 176,
		.sizeimage = 176 * 144 * 3 / 8 + 590,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.priv = 4},
	{320, 240, V4L2_PIX_FMT_JPEG, V4L2_FIELD_NONE,
		.bytesperline = 320,
		.sizeimage = 320 * 240 * 3 / 8 + 590,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.priv = 3},
	{352, 288, V4L2_PIX_FMT_JPEG, V4L2_FIELD_NONE,
		.bytesperline = 352,
		.sizeimage = 352 * 288 * 3 / 8 + 590,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.priv = 2},
	{640, 480, V4L2_PIX_FMT_JPEG, V4L2_FIELD_NONE,
		.bytesperline = 640,
		.sizeimage = 640 * 480 * 3 / 8 + 590,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.priv = 1},
};

#define SPCA50X_OFFSET_DATA 10
#define SPCA504_PCCAM600_OFFSET_SNAPSHOT 3
#define SPCA504_PCCAM600_OFFSET_COMPRESS 4
#define SPCA504_PCCAM600_OFFSET_MODE	 5
#define SPCA504_PCCAM600_OFFSET_DATA	 14
 /* Frame packet header offsets for the spca533 */
#define SPCA533_OFFSET_DATA	16
#define SPCA533_OFFSET_FRAMSEQ	15
/* Frame packet header offsets for the spca536 */
#define SPCA536_OFFSET_DATA	4
#define SPCA536_OFFSET_FRAMSEQ	1

struct cmd {
	u8 req;
	u16 val;
	u16 idx;
};

/* Initialisation data for the Creative PC-CAM 600 */
static const struct cmd spca504_pccam600_init_data[] = {
/*	{0xa0, 0x0000, 0x0503},  * capture mode */
	{0x00, 0x0000, 0x2000},
	{0x00, 0x0013, 0x2301},
	{0x00, 0x0003, 0x2000},
	{0x00, 0x0001, 0x21ac},
	{0x00, 0x0001, 0x21a6},
	{0x00, 0x0000, 0x21a7},	/* brightness */
	{0x00, 0x0020, 0x21a8},	/* contrast */
	{0x00, 0x0001, 0x21ac},	/* sat/hue */
	{0x00, 0x0000, 0x21ad},	/* hue */
	{0x00, 0x001a, 0x21ae},	/* saturation */
	{0x00, 0x0002, 0x21a3},	/* gamma */
	{0x30, 0x0154, 0x0008},
	{0x30, 0x0004, 0x0006},
	{0x30, 0x0258, 0x0009},
	{0x30, 0x0004, 0x0000},
	{0x30, 0x0093, 0x0004},
	{0x30, 0x0066, 0x0005},
	{0x00, 0x0000, 0x2000},
	{0x00, 0x0013, 0x2301},
	{0x00, 0x0003, 0x2000},
	{0x00, 0x0013, 0x2301},
	{0x00, 0x0003, 0x2000},
};

/* Creative PC-CAM 600 specific open data, sent before using the
 * generic initialisation data from spca504_open_data.
 */
static const struct cmd spca504_pccam600_open_data[] = {
	{0x00, 0x0001, 0x2501},
	{0x20, 0x0500, 0x0001},	/* snapshot mode */
	{0x00, 0x0003, 0x2880},
	{0x00, 0x0001, 0x2881},
};

/* Initialisation data for the logitech clicksmart 420 */
static const struct cmd spca504A_clicksmart420_init_data[] = {
/*	{0xa0, 0x0000, 0x0503},  * capture mode */
	{0x00, 0x0000, 0x2000},
	{0x00, 0x0013, 0x2301},
	{0x00, 0x0003, 0x2000},
	{0x00, 0x0001, 0x21ac},
	{0x00, 0x0001, 0x21a6},
	{0x00, 0x0000, 0x21a7},	/* brightness */
	{0x00, 0x0020, 0x21a8},	/* contrast */
	{0x00, 0x0001, 0x21ac},	/* sat/hue */
	{0x00, 0x0000, 0x21ad},	/* hue */
	{0x00, 0x001a, 0x21ae},	/* saturation */
	{0x00, 0x0002, 0x21a3},	/* gamma */
	{0x30, 0x0004, 0x000a},
	{0xb0, 0x0001, 0x0000},

	{0xa1, 0x0080, 0x0001},
	{0x30, 0x0049, 0x0000},
	{0x30, 0x0060, 0x0005},
	{0x0c, 0x0004, 0x0000},
	{0x00, 0x0000, 0x0000},
	{0x00, 0x0000, 0x2000},
	{0x00, 0x0013, 0x2301},
	{0x00, 0x0003, 0x2000},
};

/* clicksmart 420 open data ? */
static const struct cmd spca504A_clicksmart420_open_data[] = {
	{0x00, 0x0001, 0x2501},
	{0x20, 0x0502, 0x0000},
	{0x06, 0x0000, 0x0000},
	{0x00, 0x0004, 0x2880},
	{0x00, 0x0001, 0x2881},

	{0xa0, 0x0000, 0x0503},
};

static const u8 qtable_creative_pccam[2][64] = {
	{				/* Q-table Y-components */
	 0x05, 0x03, 0x03, 0x05, 0x07, 0x0c, 0x0f, 0x12,
	 0x04, 0x04, 0x04, 0x06, 0x08, 0x11, 0x12, 0x11,
	 0x04, 0x04, 0x05, 0x07, 0x0c, 0x11, 0x15, 0x11,
	 0x04, 0x05, 0x07, 0x09, 0x0f, 0x1a, 0x18, 0x13,
	 0x05, 0x07, 0x0b, 0x11, 0x14, 0x21, 0x1f, 0x17,
	 0x07, 0x0b, 0x11, 0x13, 0x18, 0x1f, 0x22, 0x1c,
	 0x0f, 0x13, 0x17, 0x1a, 0x1f, 0x24, 0x24, 0x1e,
	 0x16, 0x1c, 0x1d, 0x1d, 0x22, 0x1e, 0x1f, 0x1e},
	{				/* Q-table C-components */
	 0x05, 0x05, 0x07, 0x0e, 0x1e, 0x1e, 0x1e, 0x1e,
	 0x05, 0x06, 0x08, 0x14, 0x1e, 0x1e, 0x1e, 0x1e,
	 0x07, 0x08, 0x11, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
	 0x0e, 0x14, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
	 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
	 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
	 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
	 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e}
};

/* FIXME: This Q-table is identical to the Creative PC-CAM one,
 *		except for one byte. Possibly a typo?
 *		NWG: 18/05/2003.
 */
static const u8 qtable_spca504_default[2][64] = {
	{				/* Q-table Y-components */
	 0x05, 0x03, 0x03, 0x05, 0x07, 0x0c, 0x0f, 0x12,
	 0x04, 0x04, 0x04, 0x06, 0x08, 0x11, 0x12, 0x11,
	 0x04, 0x04, 0x05, 0x07, 0x0c, 0x11, 0x15, 0x11,
	 0x04, 0x05, 0x07, 0x09, 0x0f, 0x1a, 0x18, 0x13,
	 0x05, 0x07, 0x0b, 0x11, 0x14, 0x21, 0x1f, 0x17,
	 0x07, 0x0b, 0x11, 0x13, 0x18, 0x1f, 0x22, 0x1c,
	 0x0f, 0x13, 0x17, 0x1a, 0x1f, 0x24, 0x24, 0x1e,
	 0x16, 0x1c, 0x1d, 0x1d, 0x1d /* 0x22 */ , 0x1e, 0x1f, 0x1e,
	 },
	{				/* Q-table C-components */
	 0x05, 0x05, 0x07, 0x0e, 0x1e, 0x1e, 0x1e, 0x1e,
	 0x05, 0x06, 0x08, 0x14, 0x1e, 0x1e, 0x1e, 0x1e,
	 0x07, 0x08, 0x11, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
	 0x0e, 0x14, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
	 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
	 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
	 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
	 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e}
};

/* read <len> bytes to gspca_dev->usb_buf */
static void reg_r(struct gspca_dev *gspca_dev,
		  u8 req,
		  u16 index,
		  u16 len)
{
	int ret;

	if (len > USB_BUF_SZ) {
		gspca_err(gspca_dev, "reg_r: buffer overflow\n");
		return;
	}
	if (gspca_dev->usb_err < 0)
		return;
	ret = usb_control_msg(gspca_dev->dev,
			usb_rcvctrlpipe(gspca_dev->dev, 0),
			req,
			USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			0,		/* value */
			index,
			len ? gspca_dev->usb_buf : NULL, len,
			500);
	if (ret < 0) {
		pr_err("reg_r err %d\n", ret);
		gspca_dev->usb_err = ret;
		/*
		 * Make sure the buffer is zeroed to avoid uninitialized
		 * values.
		 */
		memset(gspca_dev->usb_buf, 0, USB_BUF_SZ);
	}
}

/* write one byte */
static void reg_w_1(struct gspca_dev *gspca_dev,
		   u8 req,
		   u16 value,
		   u16 index,
		   u16 byte)
{
	int ret;

	if (gspca_dev->usb_err < 0)
		return;
	gspca_dev->usb_buf[0] = byte;
	ret = usb_control_msg(gspca_dev->dev,
			usb_sndctrlpipe(gspca_dev->dev, 0),
			req,
			USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			value, index,
			gspca_dev->usb_buf, 1,
			500);
	if (ret < 0) {
		pr_err("reg_w_1 err %d\n", ret);
		gspca_dev->usb_err = ret;
	}
}

/* write req / index / value */
static void reg_w_riv(struct gspca_dev *gspca_dev,
		     u8 req, u16 index, u16 value)
{
	struct usb_device *dev = gspca_dev->dev;
	int ret;

	if (gspca_dev->usb_err < 0)
		return;
	ret = usb_control_msg(dev,
			usb_sndctrlpipe(dev, 0),
			req,
			USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			value, index, NULL, 0, 500);
	if (ret < 0) {
		pr_err("reg_w_riv err %d\n", ret);
		gspca_dev->usb_err = ret;
		return;
	}
	gspca_dbg(gspca_dev, D_USBO, "reg_w_riv: 0x%02x,0x%04x:0x%04x\n",
		  req, index, value);
}

static void write_vector(struct gspca_dev *gspca_dev,
			const struct cmd *data, int ncmds)
{
	while (--ncmds >= 0) {
		reg_w_riv(gspca_dev, data->req, data->idx, data->val);
		data++;
	}
}

static void setup_qtable(struct gspca_dev *gspca_dev,
			const u8 qtable[2][64])
{
	int i;

	/* loop over y components */
	for (i = 0; i < 64; i++)
		reg_w_riv(gspca_dev, 0x00, 0x2800 + i, qtable[0][i]);

	/* loop over c components */
	for (i = 0; i < 64; i++)
		reg_w_riv(gspca_dev, 0x00, 0x2840 + i, qtable[1][i]);
}

static void spca504_acknowledged_command(struct gspca_dev *gspca_dev,
			     u8 req, u16 idx, u16 val)
{
	reg_w_riv(gspca_dev, req, idx, val);
	reg_r(gspca_dev, 0x01, 0x0001, 1);
	gspca_dbg(gspca_dev, D_FRAM, "before wait 0x%04x\n",
		  gspca_dev->usb_buf[0]);
	reg_w_riv(gspca_dev, req, idx, val);

	msleep(200);
	reg_r(gspca_dev, 0x01, 0x0001, 1);
	gspca_dbg(gspca_dev, D_FRAM, "after wait 0x%04x\n",
		  gspca_dev->usb_buf[0]);
}

static void spca504_read_info(struct gspca_dev *gspca_dev)
{
	int i;
	u8 info[6];

	if (gspca_debug < D_STREAM)
		return;

	for (i = 0; i < 6; i++) {
		reg_r(gspca_dev, 0, i, 1);
		info[i] = gspca_dev->usb_buf[0];
	}
	gspca_dbg(gspca_dev, D_STREAM,
		  "Read info: %d %d %d %d %d %d. Should be 1,0,2,2,0,0\n",
		  info[0], info[1], info[2],
		  info[3], info[4], info[5]);
}

static void spca504A_acknowledged_command(struct gspca_dev *gspca_dev,
			u8 req,
			u16 idx, u16 val, u8 endcode, u8 count)
{
	u16 status;

	reg_w_riv(gspca_dev, req, idx, val);
	reg_r(gspca_dev, 0x01, 0x0001, 1);
	if (gspca_dev->usb_err < 0)
		return;
	gspca_dbg(gspca_dev, D_FRAM, "Status 0x%02x Need 0x%02x\n",
		  gspca_dev->usb_buf[0], endcode);
	if (!count)
		return;
	count = 200;
	while (--count > 0) {
		msleep(10);
		/* gsmart mini2 write a each wait setting 1 ms is enough */
/*		reg_w_riv(gspca_dev, req, idx, val); */
		reg_r(gspca_dev, 0x01, 0x0001, 1);
		status = gspca_dev->usb_buf[0];
		if (status == endcode) {
			gspca_dbg(gspca_dev, D_FRAM, "status 0x%04x after wait %d\n",
				  status, 200 - count);
				break;
		}
	}
}

static void spca504B_PollingDataReady(struct gspca_dev *gspca_dev)
{
	int count = 10;

	while (--count > 0) {
		reg_r(gspca_dev, 0x21, 0, 1);
		if ((gspca_dev->usb_buf[0] & 0x01) == 0)
			break;
		msleep(10);
	}
}

static void spca504B_WaitCmdStatus(struct gspca_dev *gspca_dev)
{
	int count = 50;

	while (--count > 0) {
		reg_r(gspca_dev, 0x21, 1, 1);
		if (gspca_dev->usb_buf[0] != 0) {
			reg_w_1(gspca_dev, 0x21, 0, 1, 0);
			reg_r(gspca_dev, 0x21, 1, 1);
			spca504B_PollingDataReady(gspca_dev);
			break;
		}
		msleep(10);
	}
}

static void spca50x_GetFirmware(struct gspca_dev *gspca_dev)
{
	u8 *data;

	if (gspca_debug < D_STREAM)
		return;

	data = gspca_dev->usb_buf;
	reg_r(gspca_dev, 0x20, 0, 5);
	gspca_dbg(gspca_dev, D_STREAM, "FirmWare: %d %d %d %d %d\n",
		  data[0], data[1], data[2], data[3], data[4]);
	reg_r(gspca_dev, 0x23, 0, 64);
	reg_r(gspca_dev, 0x23, 1, 64);
}

static void spca504B_SetSizeType(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;
	u8 Size;

	Size = gspca_dev->cam.cam_mode[gspca_dev->curr_mode].priv;
	switch (sd->bridge) {
	case BRIDGE_SPCA533:
		reg_w_riv(gspca_dev, 0x31, 0, 0);
		spca504B_WaitCmdStatus(gspca_dev);
		spca504B_PollingDataReady(gspca_dev);
		spca50x_GetFirmware(gspca_dev);

		reg_w_1(gspca_dev, 0x24, 0, 8, 2);		/* type */
		reg_r(gspca_dev, 0x24, 8, 1);

		reg_w_1(gspca_dev, 0x25, 0, 4, Size);
		reg_r(gspca_dev, 0x25, 4, 1);			/* size */
		spca504B_PollingDataReady(gspca_dev);

		/* Init the cam width height with some values get on init ? */
		reg_w_riv(gspca_dev, 0x31, 0x0004, 0x00);
		spca504B_WaitCmdStatus(gspca_dev);
		spca504B_PollingDataReady(gspca_dev);
		break;
	default:
/* case BRIDGE_SPCA504B: */
/* case BRIDGE_SPCA536: */
		reg_w_1(gspca_dev, 0x25, 0, 4, Size);
		reg_r(gspca_dev, 0x25, 4, 1);			/* size */
		reg_w_1(gspca_dev, 0x27, 0, 0, 6);
		reg_r(gspca_dev, 0x27, 0, 1);			/* type */
		spca504B_PollingDataReady(gspca_dev);
		break;
	case BRIDGE_SPCA504:
		Size += 3;
		if (sd->subtype == AiptekMiniPenCam13) {
			/* spca504a aiptek */
			spca504A_acknowledged_command(gspca_dev,
						0x08, Size, 0,
						0x80 | (Size & 0x0f), 1);
			spca504A_acknowledged_command(gspca_dev,
							1, 3, 0, 0x9f, 0);
		} else {
			spca504_acknowledged_command(gspca_dev, 0x08, Size, 0);
		}
		break;
	case BRIDGE_SPCA504C:
		/* capture mode */
		reg_w_riv(gspca_dev, 0xa0, (0x0500 | (Size & 0x0f)), 0x00);
		reg_w_riv(gspca_dev, 0x20, 0x01, 0x0500 | (Size & 0x0f));
		break;
	}
}

static void spca504_wait_status(struct gspca_dev *gspca_dev)
{
	int cnt;

	cnt = 256;
	while (--cnt > 0) {
		/* With this we get the status, when return 0 it's all ok */
		reg_r(gspca_dev, 0x06, 0x00, 1);
		if (gspca_dev->usb_buf[0] == 0)
			return;
		msleep(10);
	}
}

static void spca504B_setQtable(struct gspca_dev *gspca_dev)
{
	reg_w_1(gspca_dev, 0x26, 0, 0, 3);
	reg_r(gspca_dev, 0x26, 0, 1);
	spca504B_PollingDataReady(gspca_dev);
}

static void setbrightness(struct gspca_dev *gspca_dev, s32 val)
{
	struct sd *sd = (struct sd *) gspca_dev;
	u16 reg;

	reg = sd->bridge == BRIDGE_SPCA536 ? 0x20f0 : 0x21a7;
	reg_w_riv(gspca_dev, 0x00, reg, val);
}

static void setcontrast(struct gspca_dev *gspca_dev, s32 val)
{
	struct sd *sd = (struct sd *) gspca_dev;
	u16 reg;

	reg = sd->bridge == BRIDGE_SPCA536 ? 0x20f1 : 0x21a8;
	reg_w_riv(gspca_dev, 0x00, reg, val);
}

static void setcolors(struct gspca_dev *gspca_dev, s32 val)
{
	struct sd *sd = (struct sd *) gspca_dev;
	u16 reg;

	reg = sd->bridge == BRIDGE_SPCA536 ? 0x20f6 : 0x21ae;
	reg_w_riv(gspca_dev, 0x00, reg, val);
}

static void init_ctl_reg(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;
	int pollreg = 1;

	switch (sd->bridge) {
	case BRIDGE_SPCA504:
	case BRIDGE_SPCA504C:
		pollreg = 0;
		fallthrough;
	default:
/*	case BRIDGE_SPCA533: */
/*	case BRIDGE_SPCA504B: */
		reg_w_riv(gspca_dev, 0, 0x21ad, 0x00);	/* hue */
		reg_w_riv(gspca_dev, 0, 0x21ac, 0x01);	/* sat/hue */
		reg_w_riv(gspca_dev, 0, 0x21a3, 0x00);	/* gamma */
		break;
	case BRIDGE_SPCA536:
		reg_w_riv(gspca_dev, 0, 0x20f5, 0x40);
		reg_w_riv(gspca_dev, 0, 0x20f4, 0x01);
		reg_w_riv(gspca_dev, 0, 0x2089, 0x00);
		break;
	}
	if (pollreg)
		spca504B_PollingDataReady(gspca_dev);
}

/* this function is called at probe time */
static int sd_config(struct gspca_dev *gspca_dev,
			const struct usb_device_id *id)
{
	struct sd *sd = (struct sd *) gspca_dev;
	struct cam *cam;

	cam = &gspca_dev->cam;

	sd->bridge = id->driver_info >> 8;
	sd->subtype = id->driver_info;

	if (sd->subtype == AiptekMiniPenCam13) {

		/* try to get the firmware as some cam answer 2.0.1.2.2
		 * and should be a spca504b then overwrite that setting */
		reg_r(gspca_dev, 0x20, 0, 1);
		switch (gspca_dev->usb_buf[0]) {
		case 1:
			break;		/* (right bridge/subtype) */
		case 2:
			sd->bridge = BRIDGE_SPCA504B;
			sd->subtype = 0;
			break;
		default:
			return -ENODEV;
		}
	}

	switch (sd->bridge) {
	default:
/*	case BRIDGE_SPCA504B: */
/*	case BRIDGE_SPCA504: */
/*	case BRIDGE_SPCA536: */
		cam->cam_mode = vga_mode;
		cam->nmodes = ARRAY_SIZE(vga_mode);
		break;
	case BRIDGE_SPCA533:
		cam->cam_mode = custom_mode;
		if (sd->subtype == MegaImageVI)		/* 320x240 only */
			cam->nmodes = ARRAY_SIZE(custom_mode) - 1;
		else
			cam->nmodes = ARRAY_SIZE(custom_mode);
		break;
	case BRIDGE_SPCA504C:
		cam->cam_mode = vga_mode2;
		cam->nmodes = ARRAY_SIZE(vga_mode2);
		break;
	}
	return 0;
}

/* this function is called at probe and resume time */
static int sd_init(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;

	switch (sd->bridge) {
	case BRIDGE_SPCA504B:
		reg_w_riv(gspca_dev, 0x1d, 0x00, 0);
		reg_w_riv(gspca_dev, 0x00, 0x2306, 0x01);
		reg_w_riv(gspca_dev, 0x00, 0x0d04, 0x00);
		reg_w_riv(gspca_dev, 0x00, 0x2000, 0x00);
		reg_w_riv(gspca_dev, 0x00, 0x2301, 0x13);
		reg_w_riv(gspca_dev, 0x00, 0x2306, 0x00);
		fallthrough;
	case BRIDGE_SPCA533:
		spca504B_PollingDataReady(gspca_dev);
		spca50x_GetFirmware(gspca_dev);
		break;
	case BRIDGE_SPCA536:
		spca50x_GetFirmware(gspca_dev);
		reg_r(gspca_dev, 0x00, 0x5002, 1);
		reg_w_1(gspca_dev, 0x24, 0, 0, 0);
		reg_r(gspca_dev, 0x24, 0, 1);
		spca504B_PollingDataReady(gspca_dev);
		reg_w_riv(gspca_dev, 0x34, 0, 0);
		spca504B_WaitCmdStatus(gspca_dev);
		break;
	case BRIDGE_SPCA504C:	/* pccam600 */
		gspca_dbg(gspca_dev, D_STREAM, "Opening SPCA504 (PC-CAM 600)\n");
		reg_w_riv(gspca_dev, 0xe0, 0x0000, 0x0000);
		reg_w_riv(gspca_dev, 0xe0, 0x0000, 0x0001);	/* reset */
		spca504_wait_status(gspca_dev);
		if (sd->subtype == LogitechClickSmart420)
			write_vector(gspca_dev,
				spca504A_clicksmart420_open_data,
				ARRAY_SIZE(spca504A_clicksmart420_open_data));
		else
			write_vector(gspca_dev, spca504_pccam600_open_data,
				ARRAY_SIZE(spca504_pccam600_open_data));
		setup_qtable(gspca_dev, qtable_creative_pccam);
		break;
	default:
/*	case BRIDGE_SPCA504: */
		gspca_dbg(gspca_dev, D_STREAM, "Opening SPCA504\n");
		if (sd->subtype == AiptekMiniPenCam13) {
			spca504_read_info(gspca_dev);

			/* Set AE AWB Banding Type 3-> 50Hz 2-> 60Hz */
			spca504A_acknowledged_command(gspca_dev, 0x24,
							8, 3, 0x9e, 1);
			/* Twice sequential need status 0xff->0x9e->0x9d */
			spca504A_acknowledged_command(gspca_dev, 0x24,
							8, 3, 0x9e, 0);

			spca504A_acknowledged_command(gspca_dev, 0x24,
							0, 0, 0x9d, 1);
			/******************************/
			/* spca504a aiptek */
			spca504A_acknowledged_command(gspca_dev, 0x08,
							6, 0, 0x86, 1);
/*			reg_write (dev, 0, 0x2000, 0); */
/*			reg_write (dev, 0, 0x2883, 1); */
/*			spca504A_acknowledged_command (gspca_dev, 0x08,
							6, 0, 0x86, 1); */
/*			spca504A_acknowledged_command (gspca_dev, 0x24,
							0, 0, 0x9D, 1); */
			reg_w_riv(gspca_dev, 0x00, 0x270c, 0x05);
							/* L92 sno1t.txt */
			reg_w_riv(gspca_dev, 0x00, 0x2310, 0x05);
			spca504A_acknowledged_command(gspca_dev, 0x01,
							0x0f, 0, 0xff, 0);
		}
		/* setup qtable */
		reg_w_riv(gspca_dev, 0, 0x2000, 0);
		reg_w_riv(gspca_dev, 0, 0x2883, 1);
		setup_qtable(gspca_dev, qtable_spca504_default);
		break;
	}
	return gspca_dev->usb_err;
}

static int sd_start(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;
	int enable;

	/* create the JPEG header */
	jpeg_define(sd->jpeg_hdr, gspca_dev->pixfmt.height,
			gspca_dev->pixfmt.width,
			0x22);		/* JPEG 411 */
	jpeg_set_qual(sd->jpeg_hdr, QUALITY);

	if (sd->bridge == BRIDGE_SPCA504B)
		spca504B_setQtable(gspca_dev);
	spca504B_SetSizeType(gspca_dev);
	switch (sd->bridge) {
	default:
/*	case BRIDGE_SPCA504B: */
/*	case BRIDGE_SPCA533: */
/*	case BRIDGE_SPCA536: */
		switch (sd->subtype) {
		case MegapixV4:
		case LogitechClickSmart820:
		case MegaImageVI:
			reg_w_riv(gspca_dev, 0xf0, 0, 0);
			spca504B_WaitCmdStatus(gspca_dev);
			reg_r(gspca_dev, 0xf0, 4, 0);
			spca504B_WaitCmdStatus(gspca_dev);
			break;
		default:
			reg_w_riv(gspca_dev, 0x31, 0x0004, 0x00);
			spca504B_WaitCmdStatus(gspca_dev);
			spca504B_PollingDataReady(gspca_dev);
			break;
		}
		break;
	case BRIDGE_SPCA504:
		if (sd->subtype == AiptekMiniPenCam13) {
			spca504_read_info(gspca_dev);

			/* Set AE AWB Banding Type 3-> 50Hz 2-> 60Hz */
			spca504A_acknowledged_command(gspca_dev, 0x24,
							8, 3, 0x9e, 1);
			/* Twice sequential need status 0xff->0x9e->0x9d */
			spca504A_acknowledged_command(gspca_dev, 0x24,
							8, 3, 0x9e, 0);
			spca504A_acknowledged_command(gspca_dev, 0x24,
							0, 0, 0x9d, 1);
		} else {
			spca504_acknowledged_command(gspca_dev, 0x24, 8, 3);
			spca504_read_info(gspca_dev);
			spca504_acknowledged_command(gspca_dev, 0x24, 8, 3);
			spca504_acknowledged_command(gspca_dev, 0x24, 0, 0);
		}
		spca504B_SetSizeType(gspca_dev);
		reg_w_riv(gspca_dev, 0x00, 0x270c, 0x05);
							/* L92 sno1t.txt */
		reg_w_riv(gspca_dev, 0x00, 0x2310, 0x05);
		break;
	case BRIDGE_SPCA504C:
		if (sd->subtype == LogitechClickSmart420) {
			write_vector(gspca_dev,
				spca504A_clicksmart420_init_data,
				ARRAY_SIZE(spca504A_clicksmart420_init_data));
		} else {
			write_vector(gspca_dev, spca504_pccam600_init_data,
				ARRAY_SIZE(spca504_pccam600_init_data));
		}
		enable = (sd->autogain ? 0x04 : 0x01);
		reg_w_riv(gspca_dev, 0x0c, 0x0000, enable);
							/* auto exposure */
		reg_w_riv(gspca_dev, 0xb0, 0x0000, enable);
							/* auto whiteness */

		/* set default exposure compensation and whiteness balance */
		reg_w_riv(gspca_dev, 0x30, 0x0001, 800);	/* ~ 20 fps */
		reg_w_riv(gspca_dev, 0x30, 0x0002, 1600);
		spca504B_SetSizeType(gspca_dev);
		break;
	}
	init_ctl_reg(gspca_dev);
	return gspca_dev->usb_err;
}

static void sd_stopN(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;

	switch (sd->bridge) {
	default:
/*	case BRIDGE_SPCA533: */
/*	case BRIDGE_SPCA536: */
/*	case BRIDGE_SPCA504B: */
		reg_w_riv(gspca_dev, 0x31, 0, 0);
		spca504B_WaitCmdStatus(gspca_dev);
		spca504B_PollingDataReady(gspca_dev);
		break;
	case BRIDGE_SPCA504:
	case BRIDGE_SPCA504C:
		reg_w_riv(gspca_dev, 0x00, 0x2000, 0x0000);

		if (sd->subtype == AiptekMiniPenCam13) {
			/* spca504a aiptek */
/*			spca504A_acknowledged_command(gspca_dev, 0x08,
							 6, 0, 0x86, 1); */
			spca504A_acknowledged_command(gspca_dev, 0x24,
							0x00, 0x00, 0x9d, 1);
			spca504A_acknowledged_command(gspca_dev, 0x01,
							0x0f, 0x00, 0xff, 1);
		} else {
			spca504_acknowledged_command(gspca_dev, 0x24, 0, 0);
			reg_w_riv(gspca_dev, 0x01, 0x000f, 0x0000);
		}
		break;
	}
}

static void sd_pkt_scan(struct gspca_dev *gspca_dev,
			u8 *data,			/* isoc packet */
			int len)			/* iso packet length */
{
	struct sd *sd = (struct sd *) gspca_dev;
	int i, sof = 0;
	static u8 ffd9[] = {0xff, 0xd9};

/* frames are jpeg 4.1.1 without 0xff escape */
	switch (sd->bridge) {
	case BRIDGE_SPCA533:
		if (data[0] == 0xff) {
			if (data[1] != 0x01) {	/* drop packet */
/*				gspca_dev->last_packet_type = DISCARD_PACKET; */
				return;
			}
			sof = 1;
			data += SPCA533_OFFSET_DATA;
			len -= SPCA533_OFFSET_DATA;
		} else {
			data += 1;
			len -= 1;
		}
		break;
	case BRIDGE_SPCA536:
		if (data[0] == 0xff) {
			sof = 1;
			data += SPCA536_OFFSET_DATA;
			len -= SPCA536_OFFSET_DATA;
		} else {
			data += 2;
			len -= 2;
		}
		break;
	default:
/*	case BRIDGE_SPCA504: */
/*	case BRIDGE_SPCA504B: */
		switch (data[0]) {
		case 0xfe:			/* start of frame */
			sof = 1;
			data += SPCA50X_OFFSET_DATA;
			len -= SPCA50X_OFFSET_DATA;
			break;
		case 0xff:			/* drop packet */
/*			gspca_dev->last_packet_type = DISCARD_PACKET; */
			return;
		default:
			data += 1;
			len -= 1;
			break;
		}
		break;
	case BRIDGE_SPCA504C:
		switch (data[0]) {
		case 0xfe:			/* start of frame */
			sof = 1;
			data += SPCA504_PCCAM600_OFFSET_DATA;
			len -= SPCA504_PCCAM600_OFFSET_DATA;
			break;
		case 0xff:			/* drop packet */
/*			gspca_dev->last_packet_type = DISCARD_PACKET; */
			return;
		default:
			data += 1;
			len -= 1;
			break;
		}
		break;
	}
	if (sof) {		/* start of frame */
		gspca_frame_add(gspca_dev, LAST_PACKET,
				ffd9, 2);

		/* put the JPEG header in the new frame */
		gspca_frame_add(gspca_dev, FIRST_PACKET,
			sd->jpeg_hdr, JPEG_HDR_SZ);
	}

	/* add 0x00 after 0xff */
	i = 0;
	do {
		if (data[i] == 0xff) {
			gspca_frame_add(gspca_dev, INTER_PACKET,
					data, i + 1);
			len -= i;
			data += i;
			*data = 0x00;
			i = 0;
		}
		i++;
	} while (i < len);
	gspca_frame_add(gspca_dev, INTER_PACKET, data, len);
}

static int sd_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct gspca_dev *gspca_dev =
		container_of(ctrl->handler, struct gspca_dev, ctrl_handler);
	struct sd *sd = (struct sd *)gspca_dev;

	gspca_dev->usb_err = 0;

	if (!gspca_dev->streaming)
		return 0;

	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
		setbrightness(gspca_dev, ctrl->val);
		break;
	case V4L2_CID_CONTRAST:
		setcontrast(gspca_dev, ctrl->val);
		break;
	case V4L2_CID_SATURATION:
		setcolors(gspca_dev, ctrl->val);
		break;
	case V4L2_CID_AUTOGAIN:
		sd->autogain = ctrl->val;
		break;
	}
	return gspca_dev->usb_err;
}

static const struct v4l2_ctrl_ops sd_ctrl_ops = {
	.s_ctrl = sd_s_ctrl,
};

static int sd_init_controls(struct gspca_dev *gspca_dev)
{
	struct v4l2_ctrl_handler *hdl = &gspca_dev->ctrl_handler;

	gspca_dev->vdev.ctrl_handler = hdl;
	v4l2_ctrl_handler_init(hdl, 4);
	v4l2_ctrl_new_std(hdl, &sd_ctrl_ops,
			V4L2_CID_BRIGHTNESS, -128, 127, 1, 0);
	v4l2_ctrl_new_std(hdl, &sd_ctrl_ops,
			V4L2_CID_CONTRAST, 0, 255, 1, 0x20);
	v4l2_ctrl_new_std(hdl, &sd_ctrl_ops,
			V4L2_CID_SATURATION, 0, 255, 1, 0x1a);
	v4l2_ctrl_new_std(hdl, &sd_ctrl_ops,
			V4L2_CID_AUTOGAIN, 0, 1, 1, 1);

	if (hdl->error) {
		pr_err("Could not initialize controls\n");
		return hdl->error;
	}
	return 0;
}

/* sub-driver description */
static const struct sd_desc sd_desc = {
	.name = MODULE_NAME,
	.config = sd_config,
	.init = sd_init,
	.init_controls = sd_init_controls,
	.start = sd_start,
	.stopN = sd_stopN,
	.pkt_scan = sd_pkt_scan,
};

/* -- module initialisation -- */
#define BS(bridge, subtype) \
	.driver_info = (BRIDGE_ ## bridge << 8) \
			| (subtype)
static const struct usb_device_id device_table[] = {
	{USB_DEVICE(0x041e, 0x400b), BS(SPCA504C, 0)},
	{USB_DEVICE(0x041e, 0x4012), BS(SPCA504C, 0)},
	{USB_DEVICE(0x041e, 0x4013), BS(SPCA504C, 0)},
	{USB_DEVICE(0x0458, 0x7006), BS(SPCA504B, 0)},
	{USB_DEVICE(0x0461, 0x0821), BS(SPCA533, 0)},
	{USB_DEVICE(0x046d, 0x0905), BS(SPCA533, LogitechClickSmart820)},
	{USB_DEVICE(0x046d, 0x0960), BS(SPCA504C, LogitechClickSmart420)},
	{USB_DEVICE(0x0471, 0x0322), BS(SPCA504B, 0)},
	{USB_DEVICE(0x04a5, 0x3003), BS(SPCA504B, 0)},
	{USB_DEVICE(0x04a5, 0x3008), BS(SPCA533, 0)},
	{USB_DEVICE(0x04a5, 0x300a), BS(SPCA533, 0)},
	{USB_DEVICE(0x04f1, 0x1001), BS(SPCA504B, 0)},
	{USB_DEVICE(0x04fc, 0x500c), BS(SPCA504B, 0)},
	{USB_DEVICE(0x04fc, 0x504a), BS(SPCA504, AiptekMiniPenCam13)},
	{USB_DEVICE(0x04fc, 0x504b), BS(SPCA504B, 0)},
	{USB_DEVICE(0x04fc, 0x5330), BS(SPCA533, 0)},
	{USB_DEVICE(0x04fc, 0x5360), BS(SPCA536, 0)},
	{USB_DEVICE(0x04fc, 0xffff), BS(SPCA504B, 0)},
	{USB_DEVICE(0x052b, 0x1507), BS(SPCA533, MegapixV4)},
	{USB_DEVICE(0x052b, 0x1513), BS(SPCA533, MegapixV4)},
	{USB_DEVICE(0x052b, 0x1803), BS(SPCA533, MegaImageVI)},
	{USB_DEVICE(0x0546, 0x3155), BS(SPCA533, 0)},
	{USB_DEVICE(0x0546, 0x3191), BS(SPCA504B, 0)},
	{USB_DEVICE(0x0546, 0x3273), BS(SPCA504B, 0)},
	{USB_DEVICE(0x055f, 0xc211), BS(SPCA536, 0)},
	{USB_DEVICE(0x055f, 0xc230), BS(SPCA533, 0)},
	{USB_DEVICE(0x055f, 0xc232), BS(SPCA533, 0)},
	{USB_DEVICE(0x055f, 0xc360), BS(SPCA536, 0)},
	{USB_DEVICE(0x055f, 0xc420), BS(SPCA504, 0)},
	{USB_DEVICE(0x055f, 0xc430), BS(SPCA533, 0)},
	{USB_DEVICE(0x055f, 0xc440), BS(SPCA533, 0)},
	{USB_DEVICE(0x055f, 0xc520), BS(SPCA504, 0)},
	{USB_DEVICE(0x055f, 0xc530), BS(SPCA533, 0)},
	{USB_DEVICE(0x055f, 0xc540), BS(SPCA533, 0)},
	{USB_DEVICE(0x055f, 0xc630), BS(SPCA533, 0)},
	{USB_DEVICE(0x055f, 0xc650), BS(SPCA533, 0)},
	{USB_DEVICE(0x05da, 0x1018), BS(SPCA504B, 0)},
	{USB_DEVICE(0x06d6, 0x0031), BS(SPCA533, 0)},
	{USB_DEVICE(0x06d6, 0x0041), BS(SPCA504B, 0)},
	{USB_DEVICE(0x0733, 0x1311), BS(SPCA533, 0)},
	{USB_DEVICE(0x0733, 0x1314), BS(SPCA533, 0)},
	{USB_DEVICE(0x0733, 0x2211), BS(SPCA533, 0)},
	{USB_DEVICE(0x0733, 0x2221), BS(SPCA533, 0)},
	{USB_DEVICE(0x0733, 0x3261), BS(SPCA536, 0)},
	{USB_DEVICE(0x0733, 0x3281), BS(SPCA536, 0)},
	{USB_DEVICE(0x08ca, 0x0104), BS(SPCA533, 0)},
	{USB_DEVICE(0x08ca, 0x0106), BS(SPCA533, 0)},
	{USB_DEVICE(0x08ca, 0x2008), BS(SPCA504B, 0)},
	{USB_DEVICE(0x08ca, 0x2010), BS(SPCA533, 0)},
	{USB_DEVICE(0x08ca, 0x2016), BS(SPCA504B, 0)},
	{USB_DEVICE(0x08ca, 0x2018), BS(SPCA504B, 0)},
	{USB_DEVICE(0x08ca, 0x2020), BS(SPCA533, 0)},
	{USB_DEVICE(0x08ca, 0x2022), BS(SPCA533, 0)},
	{USB_DEVICE(0x08ca, 0x2024), BS(SPCA536, 0)},
	{USB_DEVICE(0x08ca, 0x2028), BS(SPCA533, 0)},
	{USB_DEVICE(0x08ca, 0x2040), BS(SPCA536, 0)},
	{USB_DEVICE(0x08ca, 0x2042), BS(SPCA536, 0)},
	{USB_DEVICE(0x08ca, 0x2050), BS(SPCA536, 0)},
	{USB_DEVICE(0x08ca, 0x2060), BS(SPCA536, 0)},
	{USB_DEVICE(0x0d64, 0x0303), BS(SPCA536, 0)},
	{}
};
MODULE_DEVICE_TABLE(usb, device_table);

/* -- device connect -- */
static int sd_probe(struct usb_interface *intf,
			const struct usb_device_id *id)
{
	return gspca_dev_probe(intf, id, &sd_desc, sizeof(struct sd),
				THIS_MODULE);
}

static struct usb_driver sd_driver = {
	.name = MODULE_NAME,
	.id_table = device_table,
	.probe = sd_probe,
	.disconnect = gspca_disconnect,
#ifdef CONFIG_PM
	.suspend = gspca_suspend,
	.resume = gspca_resume,
	.reset_resume = gspca_resume,
#endif
};

module_usb_driver(sd_driver);
