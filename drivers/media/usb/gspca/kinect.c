// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * kinect sensor device camera, gspca driver
 *
 * Copyright (C) 2011  Antonio Ospite <ospite@studenti.unina.it>
 *
 * Based on the OpenKinect project and libfreenect
 * http://openkinect.org/wiki/Init_Analysis
 *
 * Special thanks to Steven Toth and kernellabs.com for sponsoring a Kinect
 * sensor device which I tested the driver on.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#define MODULE_NAME "kinect"

#include "gspca.h"

#define CTRL_TIMEOUT 500

MODULE_AUTHOR("Antonio Ospite <ospite@studenti.unina.it>");
MODULE_DESCRIPTION("GSPCA/Kinect Sensor Device USB Camera Driver");
MODULE_LICENSE("GPL");

static bool depth_mode;

struct pkt_hdr {
	uint8_t magic[2];
	uint8_t pad;
	uint8_t flag;
	uint8_t unk1;
	uint8_t seq;
	uint8_t unk2;
	uint8_t unk3;
	uint32_t timestamp;
};

struct cam_hdr {
	uint8_t magic[2];
	__le16 len;
	__le16 cmd;
	__le16 tag;
};

/* specific webcam descriptor */
struct sd {
	struct gspca_dev gspca_dev; /* !! must be the first item */
	uint16_t cam_tag;           /* a sequence number for packets */
	uint8_t stream_flag;        /* to identify different stream types */
	uint8_t obuf[0x400];        /* output buffer for control commands */
	uint8_t ibuf[0x200];        /* input buffer for control commands */
};

#define MODE_640x480   0x0001
#define MODE_640x488   0x0002
#define MODE_1280x1024 0x0004

#define FORMAT_BAYER   0x0010
#define FORMAT_UYVY    0x0020
#define FORMAT_Y10B    0x0040

#define FPS_HIGH       0x0100

static const struct v4l2_pix_format depth_camera_mode[] = {
	{640, 480, V4L2_PIX_FMT_Y10BPACK, V4L2_FIELD_NONE,
	 .bytesperline = 640 * 10 / 8,
	 .sizeimage =  640 * 480 * 10 / 8,
	 .colorspace = V4L2_COLORSPACE_SRGB,
	 .priv = MODE_640x488 | FORMAT_Y10B},
};

static const struct v4l2_pix_format video_camera_mode[] = {
	{640, 480, V4L2_PIX_FMT_SGRBG8, V4L2_FIELD_NONE,
	 .bytesperline = 640,
	 .sizeimage = 640 * 480,
	 .colorspace = V4L2_COLORSPACE_SRGB,
	 .priv = MODE_640x480 | FORMAT_BAYER | FPS_HIGH},
	{640, 480, V4L2_PIX_FMT_UYVY, V4L2_FIELD_NONE,
	 .bytesperline = 640 * 2,
	 .sizeimage = 640 * 480 * 2,
	 .colorspace = V4L2_COLORSPACE_SRGB,
	 .priv = MODE_640x480 | FORMAT_UYVY},
	{1280, 1024, V4L2_PIX_FMT_SGRBG8, V4L2_FIELD_NONE,
	 .bytesperline = 1280,
	 .sizeimage = 1280 * 1024,
	 .colorspace = V4L2_COLORSPACE_SRGB,
	 .priv = MODE_1280x1024 | FORMAT_BAYER},
	{640, 488, V4L2_PIX_FMT_Y10BPACK, V4L2_FIELD_NONE,
	 .bytesperline = 640 * 10 / 8,
	 .sizeimage =  640 * 488 * 10 / 8,
	 .colorspace = V4L2_COLORSPACE_SRGB,
	 .priv = MODE_640x488 | FORMAT_Y10B | FPS_HIGH},
	{1280, 1024, V4L2_PIX_FMT_Y10BPACK, V4L2_FIELD_NONE,
	 .bytesperline = 1280 * 10 / 8,
	 .sizeimage =  1280 * 1024 * 10 / 8,
	 .colorspace = V4L2_COLORSPACE_SRGB,
	 .priv = MODE_1280x1024 | FORMAT_Y10B},
};

static int kinect_write(struct usb_device *udev, uint8_t *data,
			uint16_t wLength)
{
	return usb_control_msg(udev,
			      usb_sndctrlpipe(udev, 0),
			      0x00,
			      USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			      0, 0, data, wLength, CTRL_TIMEOUT);
}

static int kinect_read(struct usb_device *udev, uint8_t *data, uint16_t wLength)
{
	return usb_control_msg(udev,
			      usb_rcvctrlpipe(udev, 0),
			      0x00,
			      USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			      0, 0, data, wLength, CTRL_TIMEOUT);
}

static int send_cmd(struct gspca_dev *gspca_dev, uint16_t cmd, void *cmdbuf,
		unsigned int cmd_len, void *replybuf, unsigned int reply_len)
{
	struct sd *sd = (struct sd *) gspca_dev;
	struct usb_device *udev = gspca_dev->dev;
	int res, actual_len;
	uint8_t *obuf = sd->obuf;
	uint8_t *ibuf = sd->ibuf;
	struct cam_hdr *chdr = (void *)obuf;
	struct cam_hdr *rhdr = (void *)ibuf;

	if (cmd_len & 1 || cmd_len > (0x400 - sizeof(*chdr))) {
		pr_err("send_cmd: Invalid command length (0x%x)\n", cmd_len);
		return -1;
	}

	chdr->magic[0] = 0x47;
	chdr->magic[1] = 0x4d;
	chdr->cmd = cpu_to_le16(cmd);
	chdr->tag = cpu_to_le16(sd->cam_tag);
	chdr->len = cpu_to_le16(cmd_len / 2);

	memcpy(obuf+sizeof(*chdr), cmdbuf, cmd_len);

	res = kinect_write(udev, obuf, cmd_len + sizeof(*chdr));
	gspca_dbg(gspca_dev, D_USBO, "Control cmd=%04x tag=%04x len=%04x: %d\n",
		  cmd,
		  sd->cam_tag, cmd_len, res);
	if (res < 0) {
		pr_err("send_cmd: Output control transfer failed (%d)\n", res);
		return res;
	}

	do {
		actual_len = kinect_read(udev, ibuf, 0x200);
	} while (actual_len == 0);
	gspca_dbg(gspca_dev, D_USBO, "Control reply: %d\n", actual_len);
	if (actual_len < (int)sizeof(*rhdr)) {
		pr_err("send_cmd: Input control transfer failed (%d)\n",
		       actual_len);
		return actual_len < 0 ? actual_len : -EREMOTEIO;
	}
	actual_len -= sizeof(*rhdr);

	if (rhdr->magic[0] != 0x52 || rhdr->magic[1] != 0x42) {
		pr_err("send_cmd: Bad magic %02x %02x\n",
		       rhdr->magic[0], rhdr->magic[1]);
		return -1;
	}
	if (rhdr->cmd != chdr->cmd) {
		pr_err("send_cmd: Bad cmd %02x != %02x\n",
		       rhdr->cmd, chdr->cmd);
		return -1;
	}
	if (rhdr->tag != chdr->tag) {
		pr_err("send_cmd: Bad tag %04x != %04x\n",
		       rhdr->tag, chdr->tag);
		return -1;
	}
	if (le16_to_cpu(rhdr->len) != (actual_len/2)) {
		pr_err("send_cmd: Bad len %04x != %04x\n",
		       le16_to_cpu(rhdr->len), (int)(actual_len/2));
		return -1;
	}

	if (actual_len > reply_len) {
		pr_warn("send_cmd: Data buffer is %d bytes long, but got %d bytes\n",
			reply_len, actual_len);
		memcpy(replybuf, ibuf+sizeof(*rhdr), reply_len);
	} else {
		memcpy(replybuf, ibuf+sizeof(*rhdr), actual_len);
	}

	sd->cam_tag++;

	return actual_len;
}

static int write_register(struct gspca_dev *gspca_dev, uint16_t reg,
			uint16_t data)
{
	uint16_t reply[2];
	__le16 cmd[2];
	int res;

	cmd[0] = cpu_to_le16(reg);
	cmd[1] = cpu_to_le16(data);

	gspca_dbg(gspca_dev, D_USBO, "Write Reg 0x%04x <= 0x%02x\n", reg, data);
	res = send_cmd(gspca_dev, 0x03, cmd, 4, reply, 4);
	if (res < 0)
		return res;
	if (res != 2) {
		pr_warn("send_cmd returned %d [%04x %04x], 0000 expected\n",
			res, reply[0], reply[1]);
	}
	return 0;
}

/* this function is called at probe time */
static int sd_config_video(struct gspca_dev *gspca_dev,
		     const struct usb_device_id *id)
{
	struct sd *sd = (struct sd *) gspca_dev;
	struct cam *cam;

	sd->cam_tag = 0;

	sd->stream_flag = 0x80;

	cam = &gspca_dev->cam;

	cam->cam_mode = video_camera_mode;
	cam->nmodes = ARRAY_SIZE(video_camera_mode);

	gspca_dev->xfer_ep = 0x81;

#if 0
	/* Setting those values is not needed for video stream */
	cam->npkt = 15;
	gspca_dev->pkt_size = 960 * 2;
#endif

	return 0;
}

static int sd_config_depth(struct gspca_dev *gspca_dev,
		     const struct usb_device_id *id)
{
	struct sd *sd = (struct sd *) gspca_dev;
	struct cam *cam;

	sd->cam_tag = 0;

	sd->stream_flag = 0x70;

	cam = &gspca_dev->cam;

	cam->cam_mode = depth_camera_mode;
	cam->nmodes = ARRAY_SIZE(depth_camera_mode);

	gspca_dev->xfer_ep = 0x82;

	return 0;
}

/* this function is called at probe and resume time */
static int sd_init(struct gspca_dev *gspca_dev)
{
	gspca_dbg(gspca_dev, D_PROBE, "Kinect Camera device.\n");

	return 0;
}

static int sd_start_video(struct gspca_dev *gspca_dev)
{
	int mode;
	uint8_t fmt_reg, fmt_val;
	uint8_t res_reg, res_val;
	uint8_t fps_reg, fps_val;
	uint8_t mode_val;

	mode = gspca_dev->cam.cam_mode[gspca_dev->curr_mode].priv;

	if (mode & FORMAT_Y10B) {
		fmt_reg = 0x19;
		res_reg = 0x1a;
		fps_reg = 0x1b;
		mode_val = 0x03;
	} else {
		fmt_reg = 0x0c;
		res_reg = 0x0d;
		fps_reg = 0x0e;
		mode_val = 0x01;
	}

	/* format */
	if (mode & FORMAT_UYVY)
		fmt_val = 0x05;
	else
		fmt_val = 0x00;

	if (mode & MODE_1280x1024)
		res_val = 0x02;
	else
		res_val = 0x01;

	if (mode & FPS_HIGH)
		fps_val = 0x1e;
	else
		fps_val = 0x0f;


	/* turn off IR-reset function */
	write_register(gspca_dev, 0x105, 0x00);

	/* Reset video stream */
	write_register(gspca_dev, 0x05, 0x00);

	/* Due to some ridiculous condition in the firmware, we have to start
	 * and stop the depth stream before the camera will hand us 1280x1024
	 * IR.  This is a stupid workaround, but we've yet to find a better
	 * solution.
	 *
	 * Thanks to Drew Fisher for figuring this out.
	 */
	if (mode & (FORMAT_Y10B | MODE_1280x1024)) {
		write_register(gspca_dev, 0x13, 0x01);
		write_register(gspca_dev, 0x14, 0x1e);
		write_register(gspca_dev, 0x06, 0x02);
		write_register(gspca_dev, 0x06, 0x00);
	}

	write_register(gspca_dev, fmt_reg, fmt_val);
	write_register(gspca_dev, res_reg, res_val);
	write_register(gspca_dev, fps_reg, fps_val);

	/* Start video stream */
	write_register(gspca_dev, 0x05, mode_val);

	/* disable Hflip */
	write_register(gspca_dev, 0x47, 0x00);

	return 0;
}

static int sd_start_depth(struct gspca_dev *gspca_dev)
{
	/* turn off IR-reset function */
	write_register(gspca_dev, 0x105, 0x00);

	/* reset depth stream */
	write_register(gspca_dev, 0x06, 0x00);
	/* Depth Stream Format 0x03: 11 bit stream | 0x02: 10 bit */
	write_register(gspca_dev, 0x12, 0x02);
	/* Depth Stream Resolution 1: standard (640x480) */
	write_register(gspca_dev, 0x13, 0x01);
	/* Depth Framerate / 0x1e (30): 30 fps */
	write_register(gspca_dev, 0x14, 0x1e);
	/* Depth Stream Control  / 2: Open Depth Stream */
	write_register(gspca_dev, 0x06, 0x02);
	/* disable depth hflip / LSB = 0: Smoothing Disabled */
	write_register(gspca_dev, 0x17, 0x00);

	return 0;
}

static void sd_stopN_video(struct gspca_dev *gspca_dev)
{
	/* reset video stream */
	write_register(gspca_dev, 0x05, 0x00);
}

static void sd_stopN_depth(struct gspca_dev *gspca_dev)
{
	/* reset depth stream */
	write_register(gspca_dev, 0x06, 0x00);
}

static void sd_pkt_scan(struct gspca_dev *gspca_dev, u8 *__data, int len)
{
	struct sd *sd = (struct sd *) gspca_dev;

	struct pkt_hdr *hdr = (void *)__data;
	uint8_t *data = __data + sizeof(*hdr);
	int datalen = len - sizeof(*hdr);

	uint8_t sof = sd->stream_flag | 1;
	uint8_t mof = sd->stream_flag | 2;
	uint8_t eof = sd->stream_flag | 5;

	if (len < 12)
		return;

	if (hdr->magic[0] != 'R' || hdr->magic[1] != 'B') {
		pr_warn("[Stream %02x] Invalid magic %02x%02x\n",
			sd->stream_flag, hdr->magic[0], hdr->magic[1]);
		return;
	}

	if (hdr->flag == sof)
		gspca_frame_add(gspca_dev, FIRST_PACKET, data, datalen);

	else if (hdr->flag == mof)
		gspca_frame_add(gspca_dev, INTER_PACKET, data, datalen);

	else if (hdr->flag == eof)
		gspca_frame_add(gspca_dev, LAST_PACKET, data, datalen);

	else
		pr_warn("Packet type not recognized...\n");
}

/* sub-driver description */
static const struct sd_desc sd_desc_video = {
	.name      = MODULE_NAME,
	.config    = sd_config_video,
	.init      = sd_init,
	.start     = sd_start_video,
	.stopN     = sd_stopN_video,
	.pkt_scan  = sd_pkt_scan,
	/*
	.get_streamparm = sd_get_streamparm,
	.set_streamparm = sd_set_streamparm,
	*/
};
static const struct sd_desc sd_desc_depth = {
	.name      = MODULE_NAME,
	.config    = sd_config_depth,
	.init      = sd_init,
	.start     = sd_start_depth,
	.stopN     = sd_stopN_depth,
	.pkt_scan  = sd_pkt_scan,
	/*
	.get_streamparm = sd_get_streamparm,
	.set_streamparm = sd_set_streamparm,
	*/
};

/* -- module initialisation -- */
static const struct usb_device_id device_table[] = {
	{USB_DEVICE(0x045e, 0x02ae)},
	{USB_DEVICE(0x045e, 0x02bf)},
	{}
};

MODULE_DEVICE_TABLE(usb, device_table);

/* -- device connect -- */
static int sd_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	if (depth_mode)
		return gspca_dev_probe(intf, id, &sd_desc_depth,
				       sizeof(struct sd), THIS_MODULE);
	else
		return gspca_dev_probe(intf, id, &sd_desc_video,
				       sizeof(struct sd), THIS_MODULE);
}

static struct usb_driver sd_driver = {
	.name       = MODULE_NAME,
	.id_table   = device_table,
	.probe      = sd_probe,
	.disconnect = gspca_disconnect,
#ifdef CONFIG_PM
	.suspend    = gspca_suspend,
	.resume     = gspca_resume,
	.reset_resume = gspca_resume,
#endif
};

module_usb_driver(sd_driver);

module_param(depth_mode, bool, 0644);
MODULE_PARM_DESC(depth_mode, "0=video 1=depth");
