// SPDX-License-Identifier: GPL-2.0-only
/*
 * vivid-core.c - A Virtual Video Test Driver, core initialization
 *
 * Copyright 2014 Cisco Systems, Inc. and/or its affiliates. All rights reserved.
 */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/font.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/videodev2.h>
#include <linux/v4l2-dv-timings.h>
#include <media/videobuf2-vmalloc.h>
#include <media/videobuf2-dma-contig.h>
#include <media/v4l2-dv-timings.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-event.h>

#include "vivid-core.h"
#include "vivid-vid-common.h"
#include "vivid-vid-cap.h"
#include "vivid-vid-out.h"
#include "vivid-radio-common.h"
#include "vivid-radio-rx.h"
#include "vivid-radio-tx.h"
#include "vivid-sdr-cap.h"
#include "vivid-vbi-cap.h"
#include "vivid-vbi-out.h"
#include "vivid-osd.h"
#include "vivid-cec.h"
#include "vivid-ctrls.h"
#include "vivid-meta-cap.h"
#include "vivid-meta-out.h"
#include "vivid-touch-cap.h"

#define VIVID_MODULE_NAME "vivid"

/* The maximum number of vivid devices */
#define VIVID_MAX_DEVS CONFIG_VIDEO_VIVID_MAX_DEVS

MODULE_DESCRIPTION("Virtual Video Test Driver");
MODULE_AUTHOR("Hans Verkuil");
MODULE_LICENSE("GPL");

static unsigned n_devs = 1;
module_param(n_devs, uint, 0444);
MODULE_PARM_DESC(n_devs, " number of driver instances to create");

static int vid_cap_nr[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = -1 };
module_param_array(vid_cap_nr, int, NULL, 0444);
MODULE_PARM_DESC(vid_cap_nr, " videoX start number, -1 is autodetect");

static int vid_out_nr[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = -1 };
module_param_array(vid_out_nr, int, NULL, 0444);
MODULE_PARM_DESC(vid_out_nr, " videoX start number, -1 is autodetect");

static int vbi_cap_nr[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = -1 };
module_param_array(vbi_cap_nr, int, NULL, 0444);
MODULE_PARM_DESC(vbi_cap_nr, " vbiX start number, -1 is autodetect");

static int vbi_out_nr[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = -1 };
module_param_array(vbi_out_nr, int, NULL, 0444);
MODULE_PARM_DESC(vbi_out_nr, " vbiX start number, -1 is autodetect");

static int sdr_cap_nr[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = -1 };
module_param_array(sdr_cap_nr, int, NULL, 0444);
MODULE_PARM_DESC(sdr_cap_nr, " swradioX start number, -1 is autodetect");

static int radio_rx_nr[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = -1 };
module_param_array(radio_rx_nr, int, NULL, 0444);
MODULE_PARM_DESC(radio_rx_nr, " radioX start number, -1 is autodetect");

static int radio_tx_nr[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = -1 };
module_param_array(radio_tx_nr, int, NULL, 0444);
MODULE_PARM_DESC(radio_tx_nr, " radioX start number, -1 is autodetect");

static int meta_cap_nr[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = -1 };
module_param_array(meta_cap_nr, int, NULL, 0444);
MODULE_PARM_DESC(meta_cap_nr, " videoX start number, -1 is autodetect");

static int meta_out_nr[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = -1 };
module_param_array(meta_out_nr, int, NULL, 0444);
MODULE_PARM_DESC(meta_out_nr, " videoX start number, -1 is autodetect");

static int touch_cap_nr[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = -1 };
module_param_array(touch_cap_nr, int, NULL, 0444);
MODULE_PARM_DESC(touch_cap_nr, " v4l-touchX start number, -1 is autodetect");

static int ccs_cap_mode[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = -1 };
module_param_array(ccs_cap_mode, int, NULL, 0444);
MODULE_PARM_DESC(ccs_cap_mode, " capture crop/compose/scale mode:\n"
			   "\t\t    bit 0=crop, 1=compose, 2=scale,\n"
			   "\t\t    -1=user-controlled (default)");

static int ccs_out_mode[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = -1 };
module_param_array(ccs_out_mode, int, NULL, 0444);
MODULE_PARM_DESC(ccs_out_mode, " output crop/compose/scale mode:\n"
			   "\t\t    bit 0=crop, 1=compose, 2=scale,\n"
			   "\t\t    -1=user-controlled (default)");

static unsigned multiplanar[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = 1 };
module_param_array(multiplanar, uint, NULL, 0444);
MODULE_PARM_DESC(multiplanar, " 1 (default) creates a single planar device, 2 creates a multiplanar device.");

/*
 * Default: video + vbi-cap (raw and sliced) + radio rx + radio tx + sdr +
 * vbi-out + vid-out + meta-cap
 */
static unsigned int node_types[VIVID_MAX_DEVS] = {
	[0 ... (VIVID_MAX_DEVS - 1)] = 0xe1d3d
};
module_param_array(node_types, uint, NULL, 0444);
MODULE_PARM_DESC(node_types, " node types, default is 0xe1d3d. Bitmask with the following meaning:\n"
			     "\t\t    bit 0: Video Capture node\n"
			     "\t\t    bit 2-3: VBI Capture node: 0 = none, 1 = raw vbi, 2 = sliced vbi, 3 = both\n"
			     "\t\t    bit 4: Radio Receiver node\n"
			     "\t\t    bit 5: Software Defined Radio Receiver node\n"
			     "\t\t    bit 8: Video Output node\n"
			     "\t\t    bit 10-11: VBI Output node: 0 = none, 1 = raw vbi, 2 = sliced vbi, 3 = both\n"
			     "\t\t    bit 12: Radio Transmitter node\n"
			     "\t\t    bit 16: Framebuffer for testing overlays\n"
			     "\t\t    bit 17: Metadata Capture node\n"
			     "\t\t    bit 18: Metadata Output node\n"
			     "\t\t    bit 19: Touch Capture node\n");

/* Default: 4 inputs */
static unsigned num_inputs[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = 4 };
module_param_array(num_inputs, uint, NULL, 0444);
MODULE_PARM_DESC(num_inputs, " number of inputs, default is 4");

/* Default: input 0 = WEBCAM, 1 = TV, 2 = SVID, 3 = HDMI */
static unsigned input_types[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = 0xe4 };
module_param_array(input_types, uint, NULL, 0444);
MODULE_PARM_DESC(input_types, " input types, default is 0xe4. Two bits per input,\n"
			      "\t\t    bits 0-1 == input 0, bits 31-30 == input 15.\n"
			      "\t\t    Type 0 == webcam, 1 == TV, 2 == S-Video, 3 == HDMI");

/* Default: 2 outputs */
static unsigned num_outputs[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = 2 };
module_param_array(num_outputs, uint, NULL, 0444);
MODULE_PARM_DESC(num_outputs, " number of outputs, default is 2");

/* Default: output 0 = SVID, 1 = HDMI */
static unsigned output_types[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = 2 };
module_param_array(output_types, uint, NULL, 0444);
MODULE_PARM_DESC(output_types, " output types, default is 0x02. One bit per output,\n"
			      "\t\t    bit 0 == output 0, bit 15 == output 15.\n"
			      "\t\t    Type 0 == S-Video, 1 == HDMI");

unsigned vivid_debug;
module_param(vivid_debug, uint, 0644);
MODULE_PARM_DESC(vivid_debug, " activates debug info");

static bool no_error_inj;
module_param(no_error_inj, bool, 0444);
MODULE_PARM_DESC(no_error_inj, " if set disable the error injecting controls");

static unsigned int allocators[VIVID_MAX_DEVS] = { [0 ... (VIVID_MAX_DEVS - 1)] = 0 };
module_param_array(allocators, uint, NULL, 0444);
MODULE_PARM_DESC(allocators, " memory allocator selection, default is 0.\n"
			     "\t\t    0 == vmalloc\n"
			     "\t\t    1 == dma-contig");

static unsigned int cache_hints[VIVID_MAX_DEVS] = {
	[0 ... (VIVID_MAX_DEVS - 1)] = 0
};
module_param_array(cache_hints, uint, NULL, 0444);
MODULE_PARM_DESC(cache_hints, " user-space cache hints, default is 0.\n"
			     "\t\t    0 == forbid\n"
			     "\t\t    1 == allow");

static struct vivid_dev *vivid_devs[VIVID_MAX_DEVS];

const struct v4l2_rect vivid_min_rect = {
	0, 0, MIN_WIDTH, MIN_HEIGHT
};

const struct v4l2_rect vivid_max_rect = {
	0, 0, MAX_WIDTH * MAX_ZOOM, MAX_HEIGHT * MAX_ZOOM
};

static const u8 vivid_hdmi_edid[256] = {
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
	0x31, 0xd8, 0x34, 0x12, 0x00, 0x00, 0x00, 0x00,
	0x22, 0x1a, 0x01, 0x03, 0x80, 0x60, 0x36, 0x78,
	0x0f, 0xee, 0x91, 0xa3, 0x54, 0x4c, 0x99, 0x26,
	0x0f, 0x50, 0x54, 0x2f, 0xcf, 0x00, 0x31, 0x59,
	0x45, 0x59, 0x81, 0x80, 0x81, 0x40, 0x90, 0x40,
	0x95, 0x00, 0xa9, 0x40, 0xb3, 0x00, 0x08, 0xe8,
	0x00, 0x30, 0xf2, 0x70, 0x5a, 0x80, 0xb0, 0x58,
	0x8a, 0x00, 0xc0, 0x1c, 0x32, 0x00, 0x00, 0x1e,
	0x00, 0x00, 0x00, 0xfd, 0x00, 0x18, 0x55, 0x18,
	0x87, 0x3c, 0x00, 0x0a, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x76,
	0x69, 0x76, 0x69, 0x64, 0x0a, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x10,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x7b,

	0x02, 0x03, 0x3f, 0xf1, 0x51, 0x61, 0x60, 0x5f,
	0x5e, 0x5d, 0x10, 0x1f, 0x04, 0x13, 0x22, 0x21,
	0x20, 0x05, 0x14, 0x02, 0x11, 0x01, 0x23, 0x09,
	0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x6d, 0x03,
	0x0c, 0x00, 0x10, 0x00, 0x00, 0x3c, 0x21, 0x00,
	0x60, 0x01, 0x02, 0x03, 0x67, 0xd8, 0x5d, 0xc4,
	0x01, 0x78, 0x00, 0x00, 0xe2, 0x00, 0xca, 0xe3,
	0x05, 0x00, 0x00, 0xe3, 0x06, 0x01, 0x00, 0x4d,
	0xd0, 0x00, 0xa0, 0xf0, 0x70, 0x3e, 0x80, 0x30,
	0x20, 0x35, 0x00, 0xc0, 0x1c, 0x32, 0x00, 0x00,
	0x1e, 0x1a, 0x36, 0x80, 0xa0, 0x70, 0x38, 0x1f,
	0x40, 0x30, 0x20, 0x35, 0x00, 0xc0, 0x1c, 0x32,
	0x00, 0x00, 0x1a, 0x1a, 0x1d, 0x00, 0x80, 0x51,
	0xd0, 0x1c, 0x20, 0x40, 0x80, 0x35, 0x00, 0xc0,
	0x1c, 0x32, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82,
};

static int vidioc_querycap(struct file *file, void  *priv,
					struct v4l2_capability *cap)
{
	struct vivid_dev *dev = video_drvdata(file);

	strscpy(cap->driver, "vivid", sizeof(cap->driver));
	strscpy(cap->card, "vivid", sizeof(cap->card));
	snprintf(cap->bus_info, sizeof(cap->bus_info),
			"platform:%s", dev->v4l2_dev.name);

	cap->capabilities = dev->vid_cap_caps | dev->vid_out_caps |
		dev->vbi_cap_caps | dev->vbi_out_caps |
		dev->radio_rx_caps | dev->radio_tx_caps |
		dev->sdr_cap_caps | dev->meta_cap_caps |
		dev->meta_out_caps | dev->touch_cap_caps |
		V4L2_CAP_DEVICE_CAPS;
	return 0;
}

static int vidioc_s_hw_freq_seek(struct file *file, void *fh, const struct v4l2_hw_freq_seek *a)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_RADIO)
		return vivid_radio_rx_s_hw_freq_seek(file, fh, a);
	return -ENOTTY;
}

static int vidioc_enum_freq_bands(struct file *file, void *fh, struct v4l2_frequency_band *band)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_RADIO)
		return vivid_radio_rx_enum_freq_bands(file, fh, band);
	if (vdev->vfl_type == VFL_TYPE_SDR)
		return vivid_sdr_enum_freq_bands(file, fh, band);
	return -ENOTTY;
}

static int vidioc_g_tuner(struct file *file, void *fh, struct v4l2_tuner *vt)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_RADIO)
		return vivid_radio_rx_g_tuner(file, fh, vt);
	if (vdev->vfl_type == VFL_TYPE_SDR)
		return vivid_sdr_g_tuner(file, fh, vt);
	return vivid_video_g_tuner(file, fh, vt);
}

static int vidioc_s_tuner(struct file *file, void *fh, const struct v4l2_tuner *vt)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_RADIO)
		return vivid_radio_rx_s_tuner(file, fh, vt);
	if (vdev->vfl_type == VFL_TYPE_SDR)
		return vivid_sdr_s_tuner(file, fh, vt);
	return vivid_video_s_tuner(file, fh, vt);
}

static int vidioc_g_frequency(struct file *file, void *fh, struct v4l2_frequency *vf)
{
	struct vivid_dev *dev = video_drvdata(file);
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_RADIO)
		return vivid_radio_g_frequency(file,
			vdev->vfl_dir == VFL_DIR_RX ?
			&dev->radio_rx_freq : &dev->radio_tx_freq, vf);
	if (vdev->vfl_type == VFL_TYPE_SDR)
		return vivid_sdr_g_frequency(file, fh, vf);
	return vivid_video_g_frequency(file, fh, vf);
}

static int vidioc_s_frequency(struct file *file, void *fh, const struct v4l2_frequency *vf)
{
	struct vivid_dev *dev = video_drvdata(file);
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_RADIO)
		return vivid_radio_s_frequency(file,
			vdev->vfl_dir == VFL_DIR_RX ?
			&dev->radio_rx_freq : &dev->radio_tx_freq, vf);
	if (vdev->vfl_type == VFL_TYPE_SDR)
		return vivid_sdr_s_frequency(file, fh, vf);
	return vivid_video_s_frequency(file, fh, vf);
}

static int vidioc_overlay(struct file *file, void *fh, unsigned i)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_dir == VFL_DIR_RX)
		return vivid_vid_cap_overlay(file, fh, i);
	return vivid_vid_out_overlay(file, fh, i);
}

static int vidioc_g_fbuf(struct file *file, void *fh, struct v4l2_framebuffer *a)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_dir == VFL_DIR_RX)
		return vivid_vid_cap_g_fbuf(file, fh, a);
	return vivid_vid_out_g_fbuf(file, fh, a);
}

static int vidioc_s_fbuf(struct file *file, void *fh, const struct v4l2_framebuffer *a)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_dir == VFL_DIR_RX)
		return vivid_vid_cap_s_fbuf(file, fh, a);
	return vivid_vid_out_s_fbuf(file, fh, a);
}

static int vidioc_s_std(struct file *file, void *fh, v4l2_std_id id)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_dir == VFL_DIR_RX)
		return vivid_vid_cap_s_std(file, fh, id);
	return vivid_vid_out_s_std(file, fh, id);
}

static int vidioc_s_dv_timings(struct file *file, void *fh, struct v4l2_dv_timings *timings)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_dir == VFL_DIR_RX)
		return vivid_vid_cap_s_dv_timings(file, fh, timings);
	return vivid_vid_out_s_dv_timings(file, fh, timings);
}

static int vidioc_g_pixelaspect(struct file *file, void *fh,
				int type, struct v4l2_fract *f)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_dir == VFL_DIR_RX)
		return vivid_vid_cap_g_pixelaspect(file, fh, type, f);
	return vivid_vid_out_g_pixelaspect(file, fh, type, f);
}

static int vidioc_g_selection(struct file *file, void *fh,
			      struct v4l2_selection *sel)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_dir == VFL_DIR_RX)
		return vivid_vid_cap_g_selection(file, fh, sel);
	return vivid_vid_out_g_selection(file, fh, sel);
}

static int vidioc_s_selection(struct file *file, void *fh,
			      struct v4l2_selection *sel)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_dir == VFL_DIR_RX)
		return vivid_vid_cap_s_selection(file, fh, sel);
	return vivid_vid_out_s_selection(file, fh, sel);
}

static int vidioc_g_parm(struct file *file, void *fh,
			  struct v4l2_streamparm *parm)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_TOUCH)
		return vivid_g_parm_tch(file, fh, parm);
	if (vdev->vfl_dir == VFL_DIR_RX)
		return vivid_vid_cap_g_parm(file, fh, parm);
	return vivid_vid_out_g_parm(file, fh, parm);
}

static int vidioc_s_parm(struct file *file, void *fh,
			  struct v4l2_streamparm *parm)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_dir == VFL_DIR_RX)
		return vivid_vid_cap_s_parm(file, fh, parm);
	return -ENOTTY;
}

static int vidioc_log_status(struct file *file, void *fh)
{
	struct vivid_dev *dev = video_drvdata(file);
	struct video_device *vdev = video_devdata(file);

	v4l2_ctrl_log_status(file, fh);
	if (vdev->vfl_dir == VFL_DIR_RX && vdev->vfl_type == VFL_TYPE_VIDEO)
		tpg_log_status(&dev->tpg);
	return 0;
}

static ssize_t vivid_radio_read(struct file *file, char __user *buf,
			 size_t size, loff_t *offset)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_dir == VFL_DIR_TX)
		return -EINVAL;
	return vivid_radio_rx_read(file, buf, size, offset);
}

static ssize_t vivid_radio_write(struct file *file, const char __user *buf,
			  size_t size, loff_t *offset)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_dir == VFL_DIR_RX)
		return -EINVAL;
	return vivid_radio_tx_write(file, buf, size, offset);
}

static __poll_t vivid_radio_poll(struct file *file, struct poll_table_struct *wait)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_dir == VFL_DIR_RX)
		return vivid_radio_rx_poll(file, wait);
	return vivid_radio_tx_poll(file, wait);
}

static int vivid_enum_input(struct file *file, void *priv,
			    struct v4l2_input *inp)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_TOUCH)
		return vivid_enum_input_tch(file, priv, inp);
	return vidioc_enum_input(file, priv, inp);
}

static int vivid_g_input(struct file *file, void *priv, unsigned int *i)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_TOUCH)
		return vivid_g_input_tch(file, priv, i);
	return vidioc_g_input(file, priv, i);
}

static int vivid_s_input(struct file *file, void *priv, unsigned int i)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_TOUCH)
		return vivid_s_input_tch(file, priv, i);
	return vidioc_s_input(file, priv, i);
}

static int vivid_enum_fmt_cap(struct file *file, void  *priv,
			      struct v4l2_fmtdesc *f)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_TOUCH)
		return vivid_enum_fmt_tch(file, priv, f);
	return vivid_enum_fmt_vid(file, priv, f);
}

static int vivid_g_fmt_cap(struct file *file, void *priv,
			   struct v4l2_format *f)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_TOUCH)
		return vivid_g_fmt_tch(file, priv, f);
	return vidioc_g_fmt_vid_cap(file, priv, f);
}

static int vivid_try_fmt_cap(struct file *file, void *priv,
			     struct v4l2_format *f)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_TOUCH)
		return vivid_g_fmt_tch(file, priv, f);
	return vidioc_try_fmt_vid_cap(file, priv, f);
}

static int vivid_s_fmt_cap(struct file *file, void *priv,
			   struct v4l2_format *f)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_TOUCH)
		return vivid_g_fmt_tch(file, priv, f);
	return vidioc_s_fmt_vid_cap(file, priv, f);
}

static int vivid_g_fmt_cap_mplane(struct file *file, void *priv,
				  struct v4l2_format *f)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_TOUCH)
		return vivid_g_fmt_tch_mplane(file, priv, f);
	return vidioc_g_fmt_vid_cap_mplane(file, priv, f);
}

static int vivid_try_fmt_cap_mplane(struct file *file, void *priv,
				    struct v4l2_format *f)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_TOUCH)
		return vivid_g_fmt_tch_mplane(file, priv, f);
	return vidioc_try_fmt_vid_cap_mplane(file, priv, f);
}

static int vivid_s_fmt_cap_mplane(struct file *file, void *priv,
				  struct v4l2_format *f)
{
	struct video_device *vdev = video_devdata(file);

	if (vdev->vfl_type == VFL_TYPE_TOUCH)
		return vivid_g_fmt_tch_mplane(file, priv, f);
	return vidioc_s_fmt_vid_cap_mplane(file, priv, f);
}

static bool vivid_is_in_use(bool valid, struct video_device *vdev)
{
	unsigned long flags;
	bool res;

	if (!valid)
		return false;
	spin_lock_irqsave(&vdev->fh_lock, flags);
	res = !list_empty(&vdev->fh_list);
	spin_unlock_irqrestore(&vdev->fh_lock, flags);
	return res;
}

static bool vivid_is_last_user(struct vivid_dev *dev)
{
	unsigned int uses =
		vivid_is_in_use(dev->has_vid_cap, &dev->vid_cap_dev) +
		vivid_is_in_use(dev->has_vid_out, &dev->vid_out_dev) +
		vivid_is_in_use(dev->has_vbi_cap, &dev->vbi_cap_dev) +
		vivid_is_in_use(dev->has_vbi_out, &dev->vbi_out_dev) +
		vivid_is_in_use(dev->has_radio_rx, &dev->radio_rx_dev) +
		vivid_is_in_use(dev->has_radio_tx, &dev->radio_tx_dev) +
		vivid_is_in_use(dev->has_sdr_cap, &dev->sdr_cap_dev) +
		vivid_is_in_use(dev->has_meta_cap, &dev->meta_cap_dev) +
		vivid_is_in_use(dev->has_meta_out, &dev->meta_out_dev) +
		vivid_is_in_use(dev->has_touch_cap, &dev->touch_cap_dev);

	return uses == 1;
}

static void vivid_reconnect(struct vivid_dev *dev)
{
	if (dev->has_vid_cap)
		set_bit(V4L2_FL_REGISTERED, &dev->vid_cap_dev.flags);
	if (dev->has_vid_out)
		set_bit(V4L2_FL_REGISTERED, &dev->vid_out_dev.flags);
	if (dev->has_vbi_cap)
		set_bit(V4L2_FL_REGISTERED, &dev->vbi_cap_dev.flags);
	if (dev->has_vbi_out)
		set_bit(V4L2_FL_REGISTERED, &dev->vbi_out_dev.flags);
	if (dev->has_radio_rx)
		set_bit(V4L2_FL_REGISTERED, &dev->radio_rx_dev.flags);
	if (dev->has_radio_tx)
		set_bit(V4L2_FL_REGISTERED, &dev->radio_tx_dev.flags);
	if (dev->has_sdr_cap)
		set_bit(V4L2_FL_REGISTERED, &dev->sdr_cap_dev.flags);
	if (dev->has_meta_cap)
		set_bit(V4L2_FL_REGISTERED, &dev->meta_cap_dev.flags);
	if (dev->has_meta_out)
		set_bit(V4L2_FL_REGISTERED, &dev->meta_out_dev.flags);
	if (dev->has_touch_cap)
		set_bit(V4L2_FL_REGISTERED, &dev->touch_cap_dev.flags);
	dev->disconnect_error = false;
}

static int vivid_fop_release(struct file *file)
{
	struct vivid_dev *dev = video_drvdata(file);
	struct video_device *vdev = video_devdata(file);

	mutex_lock(&dev->mutex);
	if (!no_error_inj && v4l2_fh_is_singular_file(file) &&
	    dev->disconnect_error && !video_is_registered(vdev) &&
	    vivid_is_last_user(dev)) {
		/*
		 * I am the last user of this driver, and a disconnect
		 * was forced (since this video_device is unregistered),
		 * so re-register all video_device's again.
		 */
		v4l2_info(&dev->v4l2_dev, "reconnect\n");
		vivid_reconnect(dev);
	}
	mutex_unlock(&dev->mutex);
	if (file->private_data == dev->overlay_cap_owner)
		dev->overlay_cap_owner = NULL;
	if (file->private_data == dev->radio_rx_rds_owner) {
		dev->radio_rx_rds_last_block = 0;
		dev->radio_rx_rds_owner = NULL;
	}
	if (file->private_data == dev->radio_tx_rds_owner) {
		dev->radio_tx_rds_last_block = 0;
		dev->radio_tx_rds_owner = NULL;
	}
	if (vdev->queue)
		return vb2_fop_release(file);
	return v4l2_fh_release(file);
}

static const struct v4l2_file_operations vivid_fops = {
	.owner		= THIS_MODULE,
	.open           = v4l2_fh_open,
	.release        = vivid_fop_release,
	.read           = vb2_fop_read,
	.write          = vb2_fop_write,
	.poll		= vb2_fop_poll,
	.unlocked_ioctl = video_ioctl2,
	.mmap           = vb2_fop_mmap,
};

static const struct v4l2_file_operations vivid_radio_fops = {
	.owner		= THIS_MODULE,
	.open           = v4l2_fh_open,
	.release        = vivid_fop_release,
	.read           = vivid_radio_read,
	.write          = vivid_radio_write,
	.poll		= vivid_radio_poll,
	.unlocked_ioctl = video_ioctl2,
};

static const struct v4l2_ioctl_ops vivid_ioctl_ops = {
	.vidioc_querycap		= vidioc_querycap,

	.vidioc_enum_fmt_vid_cap	= vivid_enum_fmt_cap,
	.vidioc_g_fmt_vid_cap		= vivid_g_fmt_cap,
	.vidioc_try_fmt_vid_cap		= vivid_try_fmt_cap,
	.vidioc_s_fmt_vid_cap		= vivid_s_fmt_cap,
	.vidioc_g_fmt_vid_cap_mplane	= vivid_g_fmt_cap_mplane,
	.vidioc_try_fmt_vid_cap_mplane	= vivid_try_fmt_cap_mplane,
	.vidioc_s_fmt_vid_cap_mplane	= vivid_s_fmt_cap_mplane,

	.vidioc_enum_fmt_vid_out	= vivid_enum_fmt_vid,
	.vidioc_g_fmt_vid_out		= vidioc_g_fmt_vid_out,
	.vidioc_try_fmt_vid_out		= vidioc_try_fmt_vid_out,
	.vidioc_s_fmt_vid_out		= vidioc_s_fmt_vid_out,
	.vidioc_g_fmt_vid_out_mplane	= vidioc_g_fmt_vid_out_mplane,
	.vidioc_try_fmt_vid_out_mplane	= vidioc_try_fmt_vid_out_mplane,
	.vidioc_s_fmt_vid_out_mplane	= vidioc_s_fmt_vid_out_mplane,

	.vidioc_g_selection		= vidioc_g_selection,
	.vidioc_s_selection		= vidioc_s_selection,
	.vidioc_g_pixelaspect		= vidioc_g_pixelaspect,

	.vidioc_g_fmt_vbi_cap		= vidioc_g_fmt_vbi_cap,
	.vidioc_try_fmt_vbi_cap		= vidioc_g_fmt_vbi_cap,
	.vidioc_s_fmt_vbi_cap		= vidioc_s_fmt_vbi_cap,

	.vidioc_g_fmt_sliced_vbi_cap    = vidioc_g_fmt_sliced_vbi_cap,
	.vidioc_try_fmt_sliced_vbi_cap  = vidioc_try_fmt_sliced_vbi_cap,
	.vidioc_s_fmt_sliced_vbi_cap    = vidioc_s_fmt_sliced_vbi_cap,
	.vidioc_g_sliced_vbi_cap	= vidioc_g_sliced_vbi_cap,

	.vidioc_g_fmt_vbi_out		= vidioc_g_fmt_vbi_out,
	.vidioc_try_fmt_vbi_out		= vidioc_g_fmt_vbi_out,
	.vidioc_s_fmt_vbi_out		= vidioc_s_fmt_vbi_out,

	.vidioc_g_fmt_sliced_vbi_out    = vidioc_g_fmt_sliced_vbi_out,
	.vidioc_try_fmt_sliced_vbi_out  = vidioc_try_fmt_sliced_vbi_out,
	.vidioc_s_fmt_sliced_vbi_out    = vidioc_s_fmt_sliced_vbi_out,

	.vidioc_enum_fmt_sdr_cap	= vidioc_enum_fmt_sdr_cap,
	.vidioc_g_fmt_sdr_cap		= vidioc_g_fmt_sdr_cap,
	.vidioc_try_fmt_sdr_cap		= vidioc_try_fmt_sdr_cap,
	.vidioc_s_fmt_sdr_cap		= vidioc_s_fmt_sdr_cap,

	.vidioc_overlay			= vidioc_overlay,
	.vidioc_enum_framesizes		= vidioc_enum_framesizes,
	.vidioc_enum_frameintervals	= vidioc_enum_frameintervals,
	.vidioc_g_parm			= vidioc_g_parm,
	.vidioc_s_parm			= vidioc_s_parm,

	.vidioc_enum_fmt_vid_overlay	= vidioc_enum_fmt_vid_overlay,
	.vidioc_g_fmt_vid_overlay	= vidioc_g_fmt_vid_overlay,
	.vidioc_try_fmt_vid_overlay	= vidioc_try_fmt_vid_overlay,
	.vidioc_s_fmt_vid_overlay	= vidioc_s_fmt_vid_overlay,
	.vidioc_g_fmt_vid_out_overlay	= vidioc_g_fmt_vid_out_overlay,
	.vidioc_try_fmt_vid_out_overlay	= vidioc_try_fmt_vid_out_overlay,
	.vidioc_s_fmt_vid_out_overlay	= vidioc_s_fmt_vid_out_overlay,
	.vidioc_g_fbuf			= vidioc_g_fbuf,
	.vidioc_s_fbuf			= vidioc_s_fbuf,

	.vidioc_reqbufs			= vb2_ioctl_reqbufs,
	.vidioc_create_bufs		= vb2_ioctl_create_bufs,
	.vidioc_prepare_buf		= vb2_ioctl_prepare_buf,
	.vidioc_querybuf		= vb2_ioctl_querybuf,
	.vidioc_qbuf			= vb2_ioctl_qbuf,
	.vidioc_dqbuf			= vb2_ioctl_dqbuf,
	.vidioc_expbuf			= vb2_ioctl_expbuf,
	.vidioc_streamon		= vb2_ioctl_streamon,
	.vidioc_streamoff		= vb2_ioctl_streamoff,

	.vidioc_enum_input		= vivid_enum_input,
	.vidioc_g_input			= vivid_g_input,
	.vidioc_s_input			= vivid_s_input,
	.vidioc_s_audio			= vidioc_s_audio,
	.vidioc_g_audio			= vidioc_g_audio,
	.vidioc_enumaudio		= vidioc_enumaudio,
	.vidioc_s_frequency		= vidioc_s_frequency,
	.vidioc_g_frequency		= vidioc_g_frequency,
	.vidioc_s_tuner			= vidioc_s_tuner,
	.vidioc_g_tuner			= vidioc_g_tuner,
	.vidioc_s_modulator		= vidioc_s_modulator,
	.vidioc_g_modulator		= vidioc_g_modulator,
	.vidioc_s_hw_freq_seek		= vidioc_s_hw_freq_seek,
	.vidioc_enum_freq_bands		= vidioc_enum_freq_bands,

	.vidioc_enum_output		= vidioc_enum_output,
	.vidioc_g_output		= vidioc_g_output,
	.vidioc_s_output		= vidioc_s_output,
	.vidioc_s_audout		= vidioc_s_audout,
	.vidioc_g_audout		= vidioc_g_audout,
	.vidioc_enumaudout		= vidioc_enumaudout,

	.vidioc_querystd		= vidioc_querystd,
	.vidioc_g_std			= vidioc_g_std,
	.vidioc_s_std			= vidioc_s_std,
	.vidioc_s_dv_timings		= vidioc_s_dv_timings,
	.vidioc_g_dv_timings		= vidioc_g_dv_timings,
	.vidioc_query_dv_timings	= vidioc_query_dv_timings,
	.vidioc_enum_dv_timings		= vidioc_enum_dv_timings,
	.vidioc_dv_timings_cap		= vidioc_dv_timings_cap,
	.vidioc_g_edid			= vidioc_g_edid,
	.vidioc_s_edid			= vidioc_s_edid,

	.vidioc_log_status		= vidioc_log_status,
	.vidioc_subscribe_event		= vidioc_subscribe_event,
	.vidioc_unsubscribe_event	= v4l2_event_unsubscribe,

	.vidioc_enum_fmt_meta_cap	= vidioc_enum_fmt_meta_cap,
	.vidioc_g_fmt_meta_cap		= vidioc_g_fmt_meta_cap,
	.vidioc_s_fmt_meta_cap		= vidioc_g_fmt_meta_cap,
	.vidioc_try_fmt_meta_cap	= vidioc_g_fmt_meta_cap,

	.vidioc_enum_fmt_meta_out       = vidioc_enum_fmt_meta_out,
	.vidioc_g_fmt_meta_out          = vidioc_g_fmt_meta_out,
	.vidioc_s_fmt_meta_out          = vidioc_g_fmt_meta_out,
	.vidioc_try_fmt_meta_out        = vidioc_g_fmt_meta_out,
};

/* -----------------------------------------------------------------
	Initialization and module stuff
   ------------------------------------------------------------------*/

static void vivid_dev_release(struct v4l2_device *v4l2_dev)
{
	struct vivid_dev *dev = container_of(v4l2_dev, struct vivid_dev, v4l2_dev);

	vivid_free_controls(dev);
	v4l2_device_unregister(&dev->v4l2_dev);
#ifdef CONFIG_MEDIA_CONTROLLER
	media_device_cleanup(&dev->mdev);
#endif
	vfree(dev->scaled_line);
	vfree(dev->blended_line);
	vfree(dev->edid);
	vfree(dev->bitmap_cap);
	vfree(dev->bitmap_out);
	tpg_free(&dev->tpg);
	kfree(dev->query_dv_timings_qmenu);
	kfree(dev->query_dv_timings_qmenu_strings);
	kfree(dev);
}

#ifdef CONFIG_MEDIA_CONTROLLER
static int vivid_req_validate(struct media_request *req)
{
	struct vivid_dev *dev = container_of(req->mdev, struct vivid_dev, mdev);

	if (dev->req_validate_error) {
		dev->req_validate_error = false;
		return -EINVAL;
	}
	return vb2_request_validate(req);
}

static const struct media_device_ops vivid_media_ops = {
	.req_validate = vivid_req_validate,
	.req_queue = vb2_request_queue,
};
#endif

static int vivid_create_queue(struct vivid_dev *dev,
			      struct vb2_queue *q,
			      u32 buf_type,
			      unsigned int min_buffers_needed,
			      const struct vb2_ops *ops)
{
	if (buf_type == V4L2_BUF_TYPE_VIDEO_CAPTURE && dev->multiplanar)
		buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	else if (buf_type == V4L2_BUF_TYPE_VIDEO_OUTPUT && dev->multiplanar)
		buf_type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	else if (buf_type == V4L2_BUF_TYPE_VBI_CAPTURE && !dev->has_raw_vbi_cap)
		buf_type = V4L2_BUF_TYPE_SLICED_VBI_CAPTURE;
	else if (buf_type == V4L2_BUF_TYPE_VBI_OUTPUT && !dev->has_raw_vbi_out)
		buf_type = V4L2_BUF_TYPE_SLICED_VBI_OUTPUT;

	q->type = buf_type;
	q->io_modes = VB2_MMAP | VB2_DMABUF;
	q->io_modes |= V4L2_TYPE_IS_OUTPUT(buf_type) ?  VB2_WRITE : VB2_READ;
	if (allocators[dev->inst] != 1)
		q->io_modes |= VB2_USERPTR;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct vivid_buffer);
	q->ops = ops;
	q->mem_ops = allocators[dev->inst] == 1 ? &vb2_dma_contig_memops :
						  &vb2_vmalloc_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->min_buffers_needed = min_buffers_needed;
	q->lock = &dev->mutex;
	q->dev = dev->v4l2_dev.dev;
	q->supports_requests = true;
	q->allow_cache_hints = (cache_hints[dev->inst] == 1);

	return vb2_queue_init(q);
}

static int vivid_detect_feature_set(struct vivid_dev *dev, int inst,
				    unsigned node_type,
				    bool *has_tuner,
				    bool *has_modulator,
				    int *ccs_cap,
				    int *ccs_out,
				    unsigned in_type_counter[4],
				    unsigned out_type_counter[4])
{
	int i;

	/* do we use single- or multi-planar? */
	dev->multiplanar = multiplanar[inst] > 1;
	v4l2_info(&dev->v4l2_dev, "using %splanar format API\n",
			dev->multiplanar ? "multi" : "single ");

	/* how many inputs do we have and of what type? */
	dev->num_inputs = num_inputs[inst];
	if (dev->num_inputs < 1)
		dev->num_inputs = 1;
	if (dev->num_inputs >= MAX_INPUTS)
		dev->num_inputs = MAX_INPUTS;
	for (i = 0; i < dev->num_inputs; i++) {
		dev->input_type[i] = (input_types[inst] >> (i * 2)) & 0x3;
		dev->input_name_counter[i] = in_type_counter[dev->input_type[i]]++;
	}
	dev->has_audio_inputs = in_type_counter[TV] && in_type_counter[SVID];
	if (in_type_counter[HDMI] == 16) {
		/* The CEC physical address only allows for max 15 inputs */
		in_type_counter[HDMI]--;
		dev->num_inputs--;
	}
	dev->num_hdmi_inputs = in_type_counter[HDMI];

	/* how many outputs do we have and of what type? */
	dev->num_outputs = num_outputs[inst];
	if (dev->num_outputs < 1)
		dev->num_outputs = 1;
	if (dev->num_outputs >= MAX_OUTPUTS)
		dev->num_outputs = MAX_OUTPUTS;
	for (i = 0; i < dev->num_outputs; i++) {
		dev->output_type[i] = ((output_types[inst] >> i) & 1) ? HDMI : SVID;
		dev->output_name_counter[i] = out_type_counter[dev->output_type[i]]++;
		dev->display_present[i] = true;
	}
	dev->has_audio_outputs = out_type_counter[SVID];
	if (out_type_counter[HDMI] == 16) {
		/*
		 * The CEC physical address only allows for max 15 inputs,
		 * so outputs are also limited to 15 to allow for easy
		 * CEC output to input mapping.
		 */
		out_type_counter[HDMI]--;
		dev->num_outputs--;
	}
	dev->num_hdmi_outputs = out_type_counter[HDMI];

	/* do we create a video capture device? */
	dev->has_vid_cap = node_type & 0x0001;

	/* do we create a vbi capture device? */
	if (in_type_counter[TV] || in_type_counter[SVID]) {
		dev->has_raw_vbi_cap = node_type & 0x0004;
		dev->has_sliced_vbi_cap = node_type & 0x0008;
		dev->has_vbi_cap = dev->has_raw_vbi_cap | dev->has_sliced_vbi_cap;
	}

	/* do we create a meta capture device */
	dev->has_meta_cap = node_type & 0x20000;

	/* sanity checks */
	if ((in_type_counter[WEBCAM] || in_type_counter[HDMI]) &&
	    !dev->has_vid_cap && !dev->has_meta_cap) {
		v4l2_warn(&dev->v4l2_dev,
			  "Webcam or HDMI input without video or metadata nodes\n");
		return -EINVAL;
	}
	if ((in_type_counter[TV] || in_type_counter[SVID]) &&
	    !dev->has_vid_cap && !dev->has_vbi_cap && !dev->has_meta_cap) {
		v4l2_warn(&dev->v4l2_dev,
			  "TV or S-Video input without video, VBI or metadata nodes\n");
		return -EINVAL;
	}

	/* do we create a video output device? */
	dev->has_vid_out = node_type & 0x0100;

	/* do we create a vbi output device? */
	if (out_type_counter[SVID]) {
		dev->has_raw_vbi_out = node_type & 0x0400;
		dev->has_sliced_vbi_out = node_type & 0x0800;
		dev->has_vbi_out = dev->has_raw_vbi_out | dev->has_sliced_vbi_out;
	}

	/* do we create a metadata output device */
	dev->has_meta_out = node_type & 0x40000;

	/* sanity checks */
	if (out_type_counter[SVID] &&
	    !dev->has_vid_out && !dev->has_vbi_out && !dev->has_meta_out) {
		v4l2_warn(&dev->v4l2_dev,
			  "S-Video output without video, VBI or metadata nodes\n");
		return -EINVAL;
	}
	if (out_type_counter[HDMI] && !dev->has_vid_out && !dev->has_meta_out) {
		v4l2_warn(&dev->v4l2_dev,
			  "HDMI output without video or metadata nodes\n");
		return -EINVAL;
	}

	/* do we create a radio receiver device? */
	dev->has_radio_rx = node_type & 0x0010;

	/* do we create a radio transmitter device? */
	dev->has_radio_tx = node_type & 0x1000;

	/* do we create a software defined radio capture device? */
	dev->has_sdr_cap = node_type & 0x0020;

	/* do we have a TV tuner? */
	dev->has_tv_tuner = in_type_counter[TV];

	/* do we have a tuner? */
	*has_tuner = ((dev->has_vid_cap || dev->has_vbi_cap) && in_type_counter[TV]) ||
		      dev->has_radio_rx || dev->has_sdr_cap;

	/* do we have a modulator? */
	*has_modulator = dev->has_radio_tx;

	if (dev->has_vid_cap)
		/* do we have a framebuffer for overlay testing? */
		dev->has_fb = node_type & 0x10000;

	/* can we do crop/compose/scaling while capturing? */
	if (no_error_inj && *ccs_cap == -1)
		*ccs_cap = 7;

	/* if ccs_cap == -1, then the user can select it using controls */
	if (*ccs_cap != -1) {
		dev->has_crop_cap = *ccs_cap & 1;
		dev->has_compose_cap = *ccs_cap & 2;
		dev->has_scaler_cap = *ccs_cap & 4;
		v4l2_info(&dev->v4l2_dev, "Capture Crop: %c Compose: %c Scaler: %c\n",
			dev->has_crop_cap ? 'Y' : 'N',
			dev->has_compose_cap ? 'Y' : 'N',
			dev->has_scaler_cap ? 'Y' : 'N');
	}

	/* can we do crop/compose/scaling with video output? */
	if (no_error_inj && *ccs_out == -1)
		*ccs_out = 7;

	/* if ccs_out == -1, then the user can select it using controls */
	if (*ccs_out != -1) {
		dev->has_crop_out = *ccs_out & 1;
		dev->has_compose_out = *ccs_out & 2;
		dev->has_scaler_out = *ccs_out & 4;
		v4l2_info(&dev->v4l2_dev, "Output Crop: %c Compose: %c Scaler: %c\n",
			dev->has_crop_out ? 'Y' : 'N',
			dev->has_compose_out ? 'Y' : 'N',
			dev->has_scaler_out ? 'Y' : 'N');
	}

	/* do we create a touch capture device */
	dev->has_touch_cap = node_type & 0x80000;

	return 0;
}

static void vivid_set_capabilities(struct vivid_dev *dev)
{
	if (dev->has_vid_cap) {
		/* set up the capabilities of the video capture device */
		dev->vid_cap_caps = dev->multiplanar ?
			V4L2_CAP_VIDEO_CAPTURE_MPLANE :
			V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_VIDEO_OVERLAY;
		dev->vid_cap_caps |= V4L2_CAP_STREAMING | V4L2_CAP_READWRITE;
		if (dev->has_audio_inputs)
			dev->vid_cap_caps |= V4L2_CAP_AUDIO;
		if (dev->has_tv_tuner)
			dev->vid_cap_caps |= V4L2_CAP_TUNER;
	}
	if (dev->has_vid_out) {
		/* set up the capabilities of the video output device */
		dev->vid_out_caps = dev->multiplanar ?
			V4L2_CAP_VIDEO_OUTPUT_MPLANE :
			V4L2_CAP_VIDEO_OUTPUT;
		if (dev->has_fb)
			dev->vid_out_caps |= V4L2_CAP_VIDEO_OUTPUT_OVERLAY;
		dev->vid_out_caps |= V4L2_CAP_STREAMING | V4L2_CAP_READWRITE;
		if (dev->has_audio_outputs)
			dev->vid_out_caps |= V4L2_CAP_AUDIO;
	}
	if (dev->has_vbi_cap) {
		/* set up the capabilities of the vbi capture device */
		dev->vbi_cap_caps = (dev->has_raw_vbi_cap ? V4L2_CAP_VBI_CAPTURE : 0) |
				    (dev->has_sliced_vbi_cap ? V4L2_CAP_SLICED_VBI_CAPTURE : 0);
		dev->vbi_cap_caps |= V4L2_CAP_STREAMING | V4L2_CAP_READWRITE;
		if (dev->has_audio_inputs)
			dev->vbi_cap_caps |= V4L2_CAP_AUDIO;
		if (dev->has_tv_tuner)
			dev->vbi_cap_caps |= V4L2_CAP_TUNER;
	}
	if (dev->has_vbi_out) {
		/* set up the capabilities of the vbi output device */
		dev->vbi_out_caps = (dev->has_raw_vbi_out ? V4L2_CAP_VBI_OUTPUT : 0) |
				    (dev->has_sliced_vbi_out ? V4L2_CAP_SLICED_VBI_OUTPUT : 0);
		dev->vbi_out_caps |= V4L2_CAP_STREAMING | V4L2_CAP_READWRITE;
		if (dev->has_audio_outputs)
			dev->vbi_out_caps |= V4L2_CAP_AUDIO;
	}
	if (dev->has_sdr_cap) {
		/* set up the capabilities of the sdr capture device */
		dev->sdr_cap_caps = V4L2_CAP_SDR_CAPTURE | V4L2_CAP_TUNER;
		dev->sdr_cap_caps |= V4L2_CAP_STREAMING | V4L2_CAP_READWRITE;
	}
	/* set up the capabilities of the radio receiver device */
	if (dev->has_radio_rx)
		dev->radio_rx_caps = V4L2_CAP_RADIO | V4L2_CAP_RDS_CAPTURE |
				     V4L2_CAP_HW_FREQ_SEEK | V4L2_CAP_TUNER |
				     V4L2_CAP_READWRITE;
	/* set up the capabilities of the radio transmitter device */
	if (dev->has_radio_tx)
		dev->radio_tx_caps = V4L2_CAP_RDS_OUTPUT | V4L2_CAP_MODULATOR |
				     V4L2_CAP_READWRITE;

	/* set up the capabilities of meta capture device */
	if (dev->has_meta_cap) {
		dev->meta_cap_caps = V4L2_CAP_META_CAPTURE |
				     V4L2_CAP_STREAMING | V4L2_CAP_READWRITE;
		if (dev->has_audio_inputs)
			dev->meta_cap_caps |= V4L2_CAP_AUDIO;
		if (dev->has_tv_tuner)
			dev->meta_cap_caps |= V4L2_CAP_TUNER;
	}
	/* set up the capabilities of meta output device */
	if (dev->has_meta_out) {
		dev->meta_out_caps = V4L2_CAP_META_OUTPUT |
				     V4L2_CAP_STREAMING | V4L2_CAP_READWRITE;
		if (dev->has_audio_outputs)
			dev->meta_out_caps |= V4L2_CAP_AUDIO;
	}
	/* set up the capabilities of the touch capture device */
	if (dev->has_touch_cap) {
		dev->touch_cap_caps = V4L2_CAP_TOUCH | V4L2_CAP_STREAMING |
				      V4L2_CAP_READWRITE;
		dev->touch_cap_caps |= dev->multiplanar ?
			V4L2_CAP_VIDEO_CAPTURE_MPLANE : V4L2_CAP_VIDEO_CAPTURE;
	}
}

static void vivid_disable_unused_ioctls(struct vivid_dev *dev,
					bool has_tuner,
					bool has_modulator,
					unsigned in_type_counter[4],
					unsigned out_type_counter[4])
{
	/* disable invalid ioctls based on the feature set */
	if (!dev->has_audio_inputs) {
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_S_AUDIO);
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_G_AUDIO);
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_ENUMAUDIO);
		v4l2_disable_ioctl(&dev->vbi_cap_dev, VIDIOC_S_AUDIO);
		v4l2_disable_ioctl(&dev->vbi_cap_dev, VIDIOC_G_AUDIO);
		v4l2_disable_ioctl(&dev->vbi_cap_dev, VIDIOC_ENUMAUDIO);
		v4l2_disable_ioctl(&dev->meta_cap_dev, VIDIOC_S_AUDIO);
		v4l2_disable_ioctl(&dev->meta_cap_dev, VIDIOC_G_AUDIO);
		v4l2_disable_ioctl(&dev->meta_cap_dev, VIDIOC_ENUMAUDIO);
	}
	if (!dev->has_audio_outputs) {
		v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_S_AUDOUT);
		v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_G_AUDOUT);
		v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_ENUMAUDOUT);
		v4l2_disable_ioctl(&dev->vbi_out_dev, VIDIOC_S_AUDOUT);
		v4l2_disable_ioctl(&dev->vbi_out_dev, VIDIOC_G_AUDOUT);
		v4l2_disable_ioctl(&dev->vbi_out_dev, VIDIOC_ENUMAUDOUT);
		v4l2_disable_ioctl(&dev->meta_out_dev, VIDIOC_S_AUDOUT);
		v4l2_disable_ioctl(&dev->meta_out_dev, VIDIOC_G_AUDOUT);
		v4l2_disable_ioctl(&dev->meta_out_dev, VIDIOC_ENUMAUDOUT);
	}
	if (!in_type_counter[TV] && !in_type_counter[SVID]) {
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_S_STD);
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_G_STD);
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_ENUMSTD);
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_QUERYSTD);
	}
	if (!out_type_counter[SVID]) {
		v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_S_STD);
		v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_G_STD);
		v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_ENUMSTD);
	}
	if (!has_tuner && !has_modulator) {
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_S_FREQUENCY);
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_G_FREQUENCY);
		v4l2_disable_ioctl(&dev->vbi_cap_dev, VIDIOC_S_FREQUENCY);
		v4l2_disable_ioctl(&dev->vbi_cap_dev, VIDIOC_G_FREQUENCY);
		v4l2_disable_ioctl(&dev->meta_cap_dev, VIDIOC_S_FREQUENCY);
		v4l2_disable_ioctl(&dev->meta_cap_dev, VIDIOC_G_FREQUENCY);
	}
	if (!has_tuner) {
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_S_TUNER);
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_G_TUNER);
		v4l2_disable_ioctl(&dev->vbi_cap_dev, VIDIOC_S_TUNER);
		v4l2_disable_ioctl(&dev->vbi_cap_dev, VIDIOC_G_TUNER);
		v4l2_disable_ioctl(&dev->meta_cap_dev, VIDIOC_S_TUNER);
		v4l2_disable_ioctl(&dev->meta_cap_dev, VIDIOC_G_TUNER);
	}
	if (in_type_counter[HDMI] == 0) {
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_S_EDID);
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_G_EDID);
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_DV_TIMINGS_CAP);
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_G_DV_TIMINGS);
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_S_DV_TIMINGS);
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_ENUM_DV_TIMINGS);
		v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_QUERY_DV_TIMINGS);
	}
	if (out_type_counter[HDMI] == 0) {
		v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_G_EDID);
		v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_DV_TIMINGS_CAP);
		v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_G_DV_TIMINGS);
		v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_S_DV_TIMINGS);
		v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_ENUM_DV_TIMINGS);
	}
	if (!dev->has_fb) {
		v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_G_FBUF);
		v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_S_FBUF);
		v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_OVERLAY);
	}
	v4l2_disable_ioctl(&dev->vid_cap_dev, VIDIOC_S_HW_FREQ_SEEK);
	v4l2_disable_ioctl(&dev->vbi_cap_dev, VIDIOC_S_HW_FREQ_SEEK);
	v4l2_disable_ioctl(&dev->sdr_cap_dev, VIDIOC_S_HW_FREQ_SEEK);
	v4l2_disable_ioctl(&dev->meta_cap_dev, VIDIOC_S_HW_FREQ_SEEK);
	v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_S_FREQUENCY);
	v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_G_FREQUENCY);
	v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_ENUM_FRAMESIZES);
	v4l2_disable_ioctl(&dev->vid_out_dev, VIDIOC_ENUM_FRAMEINTERVALS);
	v4l2_disable_ioctl(&dev->vbi_out_dev, VIDIOC_S_FREQUENCY);
	v4l2_disable_ioctl(&dev->vbi_out_dev, VIDIOC_G_FREQUENCY);
	v4l2_disable_ioctl(&dev->meta_out_dev, VIDIOC_S_FREQUENCY);
	v4l2_disable_ioctl(&dev->meta_out_dev, VIDIOC_G_FREQUENCY);
	v4l2_disable_ioctl(&dev->touch_cap_dev, VIDIOC_S_PARM);
	v4l2_disable_ioctl(&dev->touch_cap_dev, VIDIOC_ENUM_FRAMESIZES);
	v4l2_disable_ioctl(&dev->touch_cap_dev, VIDIOC_ENUM_FRAMEINTERVALS);
}

static int vivid_init_dv_timings(struct vivid_dev *dev)
{
	int i;

	while (v4l2_dv_timings_presets[dev->query_dv_timings_size].bt.width)
		dev->query_dv_timings_size++;

	/*
	 * Create a char pointer array that points to the names of all the
	 * preset timings
	 */
	dev->query_dv_timings_qmenu = kmalloc_array(dev->query_dv_timings_size,
						    sizeof(char *), GFP_KERNEL);
	/*
	 * Create a string array containing the names of all the preset
	 * timings. Each name is max 31 chars long (+ terminating 0).
	 */
	dev->query_dv_timings_qmenu_strings =
		kmalloc_array(dev->query_dv_timings_size, 32, GFP_KERNEL);

	if (!dev->query_dv_timings_qmenu ||
	    !dev->query_dv_timings_qmenu_strings)
		return -ENOMEM;

	for (i = 0; i < dev->query_dv_timings_size; i++) {
		const struct v4l2_bt_timings *bt = &v4l2_dv_timings_presets[i].bt;
		char *p = dev->query_dv_timings_qmenu_strings + i * 32;
		u32 htot, vtot;

		dev->query_dv_timings_qmenu[i] = p;

		htot = V4L2_DV_BT_FRAME_WIDTH(bt);
		vtot = V4L2_DV_BT_FRAME_HEIGHT(bt);
		snprintf(p, 32, "%ux%u%s%u",
			bt->width, bt->height, bt->interlaced ? "i" : "p",
			(u32)bt->pixelclock / (htot * vtot));
	}

	return 0;
}

static int vivid_create_queues(struct vivid_dev *dev)
{
	int ret;

	/* start creating the vb2 queues */
	if (dev->has_vid_cap) {
		/* initialize vid_cap queue */
		ret = vivid_create_queue(dev, &dev->vb_vid_cap_q,
					 V4L2_BUF_TYPE_VIDEO_CAPTURE, 2,
					 &vivid_vid_cap_qops);
		if (ret)
			return ret;
	}

	if (dev->has_vid_out) {
		/* initialize vid_out queue */
		ret = vivid_create_queue(dev, &dev->vb_vid_out_q,
					 V4L2_BUF_TYPE_VIDEO_OUTPUT, 2,
					 &vivid_vid_out_qops);
		if (ret)
			return ret;
	}

	if (dev->has_vbi_cap) {
		/* initialize vbi_cap queue */
		ret = vivid_create_queue(dev, &dev->vb_vbi_cap_q,
					 V4L2_BUF_TYPE_VBI_CAPTURE, 2,
					 &vivid_vbi_cap_qops);
		if (ret)
			return ret;
	}

	if (dev->has_vbi_out) {
		/* initialize vbi_out queue */
		ret = vivid_create_queue(dev, &dev->vb_vbi_out_q,
					 V4L2_BUF_TYPE_VBI_OUTPUT, 2,
					 &vivid_vbi_out_qops);
		if (ret)
			return ret;
	}

	if (dev->has_sdr_cap) {
		/* initialize sdr_cap queue */
		ret = vivid_create_queue(dev, &dev->vb_sdr_cap_q,
					 V4L2_BUF_TYPE_SDR_CAPTURE, 8,
					 &vivid_sdr_cap_qops);
		if (ret)
			return ret;
	}

	if (dev->has_meta_cap) {
		/* initialize meta_cap queue */
		ret = vivid_create_queue(dev, &dev->vb_meta_cap_q,
					 V4L2_BUF_TYPE_META_CAPTURE, 2,
					 &vivid_meta_cap_qops);
		if (ret)
			return ret;
	}

	if (dev->has_meta_out) {
		/* initialize meta_out queue */
		ret = vivid_create_queue(dev, &dev->vb_meta_out_q,
					 V4L2_BUF_TYPE_META_OUTPUT, 1,
					 &vivid_meta_out_qops);
		if (ret)
			return ret;
	}

	if (dev->has_touch_cap) {
		/* initialize touch_cap queue */
		ret = vivid_create_queue(dev, &dev->vb_touch_cap_q,
					 V4L2_BUF_TYPE_VIDEO_CAPTURE, 1,
					 &vivid_touch_cap_qops);
		if (ret)
			return ret;
	}

	if (dev->has_fb) {
		/* Create framebuffer for testing capture/output overlay */
		ret = vivid_fb_init(dev);
		if (ret)
			return ret;
		v4l2_info(&dev->v4l2_dev, "Framebuffer device registered as fb%d\n",
			  dev->fb_info.node);
	}
	return 0;
}

static int vivid_create_devnodes(struct platform_device *pdev,
				 struct vivid_dev *dev, int inst,
				 unsigned int cec_tx_bus_cnt,
				 v4l2_std_id tvnorms_cap,
				 v4l2_std_id tvnorms_out,
				 unsigned in_type_counter[4],
				 unsigned out_type_counter[4])
{
	struct video_device *vfd;
	int ret;

	if (dev->has_vid_cap) {
		vfd = &dev->vid_cap_dev;
		snprintf(vfd->name, sizeof(vfd->name),
			 "vivid-%03d-vid-cap", inst);
		vfd->fops = &vivid_fops;
		vfd->ioctl_ops = &vivid_ioctl_ops;
		vfd->device_caps = dev->vid_cap_caps;
		vfd->release = video_device_release_empty;
		vfd->v4l2_dev = &dev->v4l2_dev;
		vfd->queue = &dev->vb_vid_cap_q;
		vfd->tvnorms = tvnorms_cap;

		/*
		 * Provide a mutex to v4l2 core. It will be used to protect
		 * all fops and v4l2 ioctls.
		 */
		vfd->lock = &dev->mutex;
		video_set_drvdata(vfd, dev);

#ifdef CONFIG_MEDIA_CONTROLLER
		dev->vid_cap_pad.flags = MEDIA_PAD_FL_SINK;
		ret = media_entity_pads_init(&vfd->entity, 1, &dev->vid_cap_pad);
		if (ret)
			return ret;
#endif

#ifdef CONFIG_VIDEO_VIVID_CEC
		if (in_type_counter[HDMI]) {
			ret = cec_register_adapter(dev->cec_rx_adap, &pdev->dev);
			if (ret < 0) {
				cec_delete_adapter(dev->cec_rx_adap);
				dev->cec_rx_adap = NULL;
				return ret;
			}
			cec_s_phys_addr(dev->cec_rx_adap, 0, false);
			v4l2_info(&dev->v4l2_dev, "CEC adapter %s registered for HDMI input 0\n",
				  dev_name(&dev->cec_rx_adap->devnode.dev));
		}
#endif

		ret = video_register_device(vfd, VFL_TYPE_VIDEO, vid_cap_nr[inst]);
		if (ret < 0)
			return ret;
		v4l2_info(&dev->v4l2_dev, "V4L2 capture device registered as %s\n",
					  video_device_node_name(vfd));
	}

	if (dev->has_vid_out) {
#ifdef CONFIG_VIDEO_VIVID_CEC
		int i;
#endif
		vfd = &dev->vid_out_dev;
		snprintf(vfd->name, sizeof(vfd->name),
			 "vivid-%03d-vid-out", inst);
		vfd->vfl_dir = VFL_DIR_TX;
		vfd->fops = &vivid_fops;
		vfd->ioctl_ops = &vivid_ioctl_ops;
		vfd->device_caps = dev->vid_out_caps;
		vfd->release = video_device_release_empty;
		vfd->v4l2_dev = &dev->v4l2_dev;
		vfd->queue = &dev->vb_vid_out_q;
		vfd->tvnorms = tvnorms_out;

		/*
		 * Provide a mutex to v4l2 core. It will be used to protect
		 * all fops and v4l2 ioctls.
		 */
		vfd->lock = &dev->mutex;
		video_set_drvdata(vfd, dev);

#ifdef CONFIG_MEDIA_CONTROLLER
		dev->vid_out_pad.flags = MEDIA_PAD_FL_SOURCE;
		ret = media_entity_pads_init(&vfd->entity, 1, &dev->vid_out_pad);
		if (ret)
			return ret;
#endif

#ifdef CONFIG_VIDEO_VIVID_CEC
		for (i = 0; i < cec_tx_bus_cnt; i++) {
			ret = cec_register_adapter(dev->cec_tx_adap[i], &pdev->dev);
			if (ret < 0) {
				for (; i < cec_tx_bus_cnt; i++) {
					cec_delete_adapter(dev->cec_tx_adap[i]);
					dev->cec_tx_adap[i] = NULL;
				}
				return ret;
			}
			v4l2_info(&dev->v4l2_dev, "CEC adapter %s registered for HDMI output %d\n",
				  dev_name(&dev->cec_tx_adap[i]->devnode.dev), i);
			if (i < out_type_counter[HDMI])
				cec_s_phys_addr(dev->cec_tx_adap[i], (i + 1) << 12, false);
			else
				cec_s_phys_addr(dev->cec_tx_adap[i], 0x1000, false);
		}
#endif

		ret = video_register_device(vfd, VFL_TYPE_VIDEO, vid_out_nr[inst]);
		if (ret < 0)
			return ret;
		v4l2_info(&dev->v4l2_dev, "V4L2 output device registered as %s\n",
					  video_device_node_name(vfd));
	}

	if (dev->has_vbi_cap) {
		vfd = &dev->vbi_cap_dev;
		snprintf(vfd->name, sizeof(vfd->name),
			 "vivid-%03d-vbi-cap", inst);
		vfd->fops = &vivid_fops;
		vfd->ioctl_ops = &vivid_ioctl_ops;
		vfd->device_caps = dev->vbi_cap_caps;
		vfd->release = video_device_release_empty;
		vfd->v4l2_dev = &dev->v4l2_dev;
		vfd->queue = &dev->vb_vbi_cap_q;
		vfd->lock = &dev->mutex;
		vfd->tvnorms = tvnorms_cap;
		video_set_drvdata(vfd, dev);

#ifdef CONFIG_MEDIA_CONTROLLER
		dev->vbi_cap_pad.flags = MEDIA_PAD_FL_SINK;
		ret = media_entity_pads_init(&vfd->entity, 1, &dev->vbi_cap_pad);
		if (ret)
			return ret;
#endif

		ret = video_register_device(vfd, VFL_TYPE_VBI, vbi_cap_nr[inst]);
		if (ret < 0)
			return ret;
		v4l2_info(&dev->v4l2_dev, "V4L2 capture device registered as %s, supports %s VBI\n",
					  video_device_node_name(vfd),
					  (dev->has_raw_vbi_cap && dev->has_sliced_vbi_cap) ?
					  "raw and sliced" :
					  (dev->has_raw_vbi_cap ? "raw" : "sliced"));
	}

	if (dev->has_vbi_out) {
		vfd = &dev->vbi_out_dev;
		snprintf(vfd->name, sizeof(vfd->name),
			 "vivid-%03d-vbi-out", inst);
		vfd->vfl_dir = VFL_DIR_TX;
		vfd->fops = &vivid_fops;
		vfd->ioctl_ops = &vivid_ioctl_ops;
		vfd->device_caps = dev->vbi_out_caps;
		vfd->release = video_device_release_empty;
		vfd->v4l2_dev = &dev->v4l2_dev;
		vfd->queue = &dev->vb_vbi_out_q;
		vfd->lock = &dev->mutex;
		vfd->tvnorms = tvnorms_out;
		video_set_drvdata(vfd, dev);

#ifdef CONFIG_MEDIA_CONTROLLER
		dev->vbi_out_pad.flags = MEDIA_PAD_FL_SOURCE;
		ret = media_entity_pads_init(&vfd->entity, 1, &dev->vbi_out_pad);
		if (ret)
			return ret;
#endif

		ret = video_register_device(vfd, VFL_TYPE_VBI, vbi_out_nr[inst]);
		if (ret < 0)
			return ret;
		v4l2_info(&dev->v4l2_dev, "V4L2 output device registered as %s, supports %s VBI\n",
					  video_device_node_name(vfd),
					  (dev->has_raw_vbi_out && dev->has_sliced_vbi_out) ?
					  "raw and sliced" :
					  (dev->has_raw_vbi_out ? "raw" : "sliced"));
	}

	if (dev->has_sdr_cap) {
		vfd = &dev->sdr_cap_dev;
		snprintf(vfd->name, sizeof(vfd->name),
			 "vivid-%03d-sdr-cap", inst);
		vfd->fops = &vivid_fops;
		vfd->ioctl_ops = &vivid_ioctl_ops;
		vfd->device_caps = dev->sdr_cap_caps;
		vfd->release = video_device_release_empty;
		vfd->v4l2_dev = &dev->v4l2_dev;
		vfd->queue = &dev->vb_sdr_cap_q;
		vfd->lock = &dev->mutex;
		video_set_drvdata(vfd, dev);

#ifdef CONFIG_MEDIA_CONTROLLER
		dev->sdr_cap_pad.flags = MEDIA_PAD_FL_SINK;
		ret = media_entity_pads_init(&vfd->entity, 1, &dev->sdr_cap_pad);
		if (ret)
			return ret;
#endif

		ret = video_register_device(vfd, VFL_TYPE_SDR, sdr_cap_nr[inst]);
		if (ret < 0)
			return ret;
		v4l2_info(&dev->v4l2_dev, "V4L2 capture device registered as %s\n",
					  video_device_node_name(vfd));
	}

	if (dev->has_radio_rx) {
		vfd = &dev->radio_rx_dev;
		snprintf(vfd->name, sizeof(vfd->name),
			 "vivid-%03d-rad-rx", inst);
		vfd->fops = &vivid_radio_fops;
		vfd->ioctl_ops = &vivid_ioctl_ops;
		vfd->device_caps = dev->radio_rx_caps;
		vfd->release = video_device_release_empty;
		vfd->v4l2_dev = &dev->v4l2_dev;
		vfd->lock = &dev->mutex;
		video_set_drvdata(vfd, dev);

		ret = video_register_device(vfd, VFL_TYPE_RADIO, radio_rx_nr[inst]);
		if (ret < 0)
			return ret;
		v4l2_info(&dev->v4l2_dev, "V4L2 receiver device registered as %s\n",
					  video_device_node_name(vfd));
	}

	if (dev->has_radio_tx) {
		vfd = &dev->radio_tx_dev;
		snprintf(vfd->name, sizeof(vfd->name),
			 "vivid-%03d-rad-tx", inst);
		vfd->vfl_dir = VFL_DIR_TX;
		vfd->fops = &vivid_radio_fops;
		vfd->ioctl_ops = &vivid_ioctl_ops;
		vfd->device_caps = dev->radio_tx_caps;
		vfd->release = video_device_release_empty;
		vfd->v4l2_dev = &dev->v4l2_dev;
		vfd->lock = &dev->mutex;
		video_set_drvdata(vfd, dev);

		ret = video_register_device(vfd, VFL_TYPE_RADIO, radio_tx_nr[inst]);
		if (ret < 0)
			return ret;
		v4l2_info(&dev->v4l2_dev, "V4L2 transmitter device registered as %s\n",
					  video_device_node_name(vfd));
	}

	if (dev->has_meta_cap) {
		vfd = &dev->meta_cap_dev;
		snprintf(vfd->name, sizeof(vfd->name),
			 "vivid-%03d-meta-cap", inst);
		vfd->fops = &vivid_fops;
		vfd->ioctl_ops = &vivid_ioctl_ops;
		vfd->device_caps = dev->meta_cap_caps;
		vfd->release = video_device_release_empty;
		vfd->v4l2_dev = &dev->v4l2_dev;
		vfd->queue = &dev->vb_meta_cap_q;
		vfd->lock = &dev->mutex;
		vfd->tvnorms = tvnorms_cap;
		video_set_drvdata(vfd, dev);
#ifdef CONFIG_MEDIA_CONTROLLER
		dev->meta_cap_pad.flags = MEDIA_PAD_FL_SINK;
		ret = media_entity_pads_init(&vfd->entity, 1,
					     &dev->meta_cap_pad);
		if (ret)
			return ret;
#endif
		ret = video_register_device(vfd, VFL_TYPE_VIDEO,
					    meta_cap_nr[inst]);
		if (ret < 0)
			return ret;
		v4l2_info(&dev->v4l2_dev,
			  "V4L2 metadata capture device registered as %s\n",
			  video_device_node_name(vfd));
	}

	if (dev->has_meta_out) {
		vfd = &dev->meta_out_dev;
		snprintf(vfd->name, sizeof(vfd->name),
			 "vivid-%03d-meta-out", inst);
		vfd->vfl_dir = VFL_DIR_TX;
		vfd->fops = &vivid_fops;
		vfd->ioctl_ops = &vivid_ioctl_ops;
		vfd->device_caps = dev->meta_out_caps;
		vfd->release = video_device_release_empty;
		vfd->v4l2_dev = &dev->v4l2_dev;
		vfd->queue = &dev->vb_meta_out_q;
		vfd->lock = &dev->mutex;
		vfd->tvnorms = tvnorms_out;
		video_set_drvdata(vfd, dev);
#ifdef CONFIG_MEDIA_CONTROLLER
		dev->meta_out_pad.flags = MEDIA_PAD_FL_SOURCE;
		ret = media_entity_pads_init(&vfd->entity, 1,
					     &dev->meta_out_pad);
		if (ret)
			return ret;
#endif
		ret = video_register_device(vfd, VFL_TYPE_VIDEO,
					    meta_out_nr[inst]);
		if (ret < 0)
			return ret;
		v4l2_info(&dev->v4l2_dev,
			  "V4L2 metadata output device registered as %s\n",
			  video_device_node_name(vfd));
	}

	if (dev->has_touch_cap) {
		vfd = &dev->touch_cap_dev;
		snprintf(vfd->name, sizeof(vfd->name),
			 "vivid-%03d-touch-cap", inst);
		vfd->fops = &vivid_fops;
		vfd->ioctl_ops = &vivid_ioctl_ops;
		vfd->device_caps = dev->touch_cap_caps;
		vfd->release = video_device_release_empty;
		vfd->v4l2_dev = &dev->v4l2_dev;
		vfd->queue = &dev->vb_touch_cap_q;
		vfd->tvnorms = tvnorms_cap;
		vfd->lock = &dev->mutex;
		video_set_drvdata(vfd, dev);
#ifdef CONFIG_MEDIA_CONTROLLER
		dev->touch_cap_pad.flags = MEDIA_PAD_FL_SINK;
		ret = media_entity_pads_init(&vfd->entity, 1,
					     &dev->touch_cap_pad);
		if (ret)
			return ret;
#endif
		ret = video_register_device(vfd, VFL_TYPE_TOUCH,
					    touch_cap_nr[inst]);
		if (ret < 0)
			return ret;
		v4l2_info(&dev->v4l2_dev,
			  "V4L2 touch capture device registered as %s\n",
			  video_device_node_name(vfd));
	}

#ifdef CONFIG_MEDIA_CONTROLLER
	/* Register the media device */
	ret = media_device_register(&dev->mdev);
	if (ret) {
		dev_err(dev->mdev.dev,
			"media device register failed (err=%d)\n", ret);
		return ret;
	}
#endif
	return 0;
}

static int vivid_create_instance(struct platform_device *pdev, int inst)
{
	static const struct v4l2_dv_timings def_dv_timings =
					V4L2_DV_BT_CEA_1280X720P60;
	unsigned in_type_counter[4] = { 0, 0, 0, 0 };
	unsigned out_type_counter[4] = { 0, 0, 0, 0 };
	int ccs_cap = ccs_cap_mode[inst];
	int ccs_out = ccs_out_mode[inst];
	bool has_tuner;
	bool has_modulator;
	struct vivid_dev *dev;
	unsigned node_type = node_types[inst];
	v4l2_std_id tvnorms_cap = 0, tvnorms_out = 0;
	unsigned int cec_tx_bus_cnt = 0;
	int ret;
	int i;

	/* allocate main vivid state structure */
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	dev->inst = inst;

#ifdef CONFIG_MEDIA_CONTROLLER
	dev->v4l2_dev.mdev = &dev->mdev;

	/* Initialize media device */
	strscpy(dev->mdev.model, VIVID_MODULE_NAME, sizeof(dev->mdev.model));
	snprintf(dev->mdev.bus_info, sizeof(dev->mdev.bus_info),
		 "platform:%s-%03d", VIVID_MODULE_NAME, inst);
	dev->mdev.dev = &pdev->dev;
	media_device_init(&dev->mdev);
	dev->mdev.ops = &vivid_media_ops;
#endif

	/* register v4l2_device */
	snprintf(dev->v4l2_dev.name, sizeof(dev->v4l2_dev.name),
			"%s-%03d", VIVID_MODULE_NAME, inst);
	ret = v4l2_device_register(&pdev->dev, &dev->v4l2_dev);
	if (ret) {
		kfree(dev);
		return ret;
	}
	dev->v4l2_dev.release = vivid_dev_release;

	ret = vivid_detect_feature_set(dev, inst, node_type,
				       &has_tuner, &has_modulator,
				       &ccs_cap, &ccs_out,
				       in_type_counter, out_type_counter);
	if (ret)
		goto free_dev;

	vivid_set_capabilities(dev);

	ret = -ENOMEM;
	/* initialize the test pattern generator */
	tpg_init(&dev->tpg, 640, 360);
	if (tpg_alloc(&dev->tpg, array_size(MAX_WIDTH, MAX_ZOOM)))
		goto free_dev;
	dev->scaled_line = vzalloc(array_size(MAX_WIDTH, MAX_ZOOM));
	if (!dev->scaled_line)
		goto free_dev;
	dev->blended_line = vzalloc(array_size(MAX_WIDTH, MAX_ZOOM));
	if (!dev->blended_line)
		goto free_dev;

	/* load the edid */
	dev->edid = vmalloc(array_size(256, 128));
	if (!dev->edid)
		goto free_dev;

	ret = vivid_init_dv_timings(dev);
	if (ret < 0)
		goto free_dev;

	vivid_disable_unused_ioctls(dev, has_tuner, has_modulator,
				    in_type_counter, out_type_counter);

	/* configure internal data */
	dev->fmt_cap = &vivid_formats[0];
	dev->fmt_out = &vivid_formats[0];
	if (!dev->multiplanar)
		vivid_formats[0].data_offset[0] = 0;
	dev->webcam_size_idx = 1;
	dev->webcam_ival_idx = 3;
	tpg_s_fourcc(&dev->tpg, dev->fmt_cap->fourcc);
	dev->std_out = V4L2_STD_PAL;
	if (dev->input_type[0] == TV || dev->input_type[0] == SVID)
		tvnorms_cap = V4L2_STD_ALL;
	if (dev->output_type[0] == SVID)
		tvnorms_out = V4L2_STD_ALL;
	for (i = 0; i < MAX_INPUTS; i++) {
		dev->dv_timings_cap[i] = def_dv_timings;
		dev->std_cap[i] = V4L2_STD_PAL;
	}
	dev->dv_timings_out = def_dv_timings;
	dev->tv_freq = 2804 /* 175.25 * 16 */;
	dev->tv_audmode = V4L2_TUNER_MODE_STEREO;
	dev->tv_field_cap = V4L2_FIELD_INTERLACED;
	dev->tv_field_out = V4L2_FIELD_INTERLACED;
	dev->radio_rx_freq = 95000 * 16;
	dev->radio_rx_audmode = V4L2_TUNER_MODE_STEREO;
	if (dev->has_radio_tx) {
		dev->radio_tx_freq = 95500 * 16;
		dev->radio_rds_loop = false;
	}
	dev->radio_tx_subchans = V4L2_TUNER_SUB_STEREO | V4L2_TUNER_SUB_RDS;
	dev->sdr_adc_freq = 300000;
	dev->sdr_fm_freq = 50000000;
	dev->sdr_pixelformat = V4L2_SDR_FMT_CU8;
	dev->sdr_buffersize = SDR_CAP_SAMPLES_PER_BUF * 2;

	dev->edid_max_blocks = dev->edid_blocks = 2;
	memcpy(dev->edid, vivid_hdmi_edid, sizeof(vivid_hdmi_edid));
	dev->radio_rds_init_time = ktime_get();

	/* create all controls */
	ret = vivid_create_controls(dev, ccs_cap == -1, ccs_out == -1, no_error_inj,
			in_type_counter[TV] || in_type_counter[SVID] ||
			out_type_counter[SVID],
			in_type_counter[HDMI] || out_type_counter[HDMI]);
	if (ret)
		goto unreg_dev;

	/* enable/disable interface specific controls */
	if (dev->num_outputs && dev->output_type[0] != HDMI)
		v4l2_ctrl_activate(dev->ctrl_display_present, false);
	if (dev->num_inputs && dev->input_type[0] != HDMI) {
		v4l2_ctrl_activate(dev->ctrl_dv_timings_signal_mode, false);
		v4l2_ctrl_activate(dev->ctrl_dv_timings, false);
	} else if (dev->num_inputs && dev->input_type[0] == HDMI) {
		v4l2_ctrl_activate(dev->ctrl_std_signal_mode, false);
		v4l2_ctrl_activate(dev->ctrl_standard, false);
	}

	/*
	 * update the capture and output formats to do a proper initial
	 * configuration.
	 */
	vivid_update_format_cap(dev, false);
	vivid_update_format_out(dev);

	/* initialize overlay */
	dev->fb_cap.fmt.width = dev->src_rect.width;
	dev->fb_cap.fmt.height = dev->src_rect.height;
	dev->fb_cap.fmt.pixelformat = dev->fmt_cap->fourcc;
	dev->fb_cap.fmt.bytesperline = dev->src_rect.width * tpg_g_twopixelsize(&dev->tpg, 0) / 2;
	dev->fb_cap.fmt.sizeimage = dev->src_rect.height * dev->fb_cap.fmt.bytesperline;

	/* update touch configuration */
	dev->timeperframe_tch_cap.numerator = 1;
	dev->timeperframe_tch_cap.denominator = 10;
	vivid_set_touch(dev, 0);

	/* initialize locks */
	spin_lock_init(&dev->slock);
	mutex_init(&dev->mutex);

	/* init dma queues */
	INIT_LIST_HEAD(&dev->vid_cap_active);
	INIT_LIST_HEAD(&dev->vid_out_active);
	INIT_LIST_HEAD(&dev->vbi_cap_active);
	INIT_LIST_HEAD(&dev->vbi_out_active);
	INIT_LIST_HEAD(&dev->sdr_cap_active);
	INIT_LIST_HEAD(&dev->meta_cap_active);
	INIT_LIST_HEAD(&dev->meta_out_active);
	INIT_LIST_HEAD(&dev->touch_cap_active);

	INIT_LIST_HEAD(&dev->cec_work_list);
	spin_lock_init(&dev->cec_slock);
	/*
	 * Same as create_singlethread_workqueue, but now I can use the
	 * string formatting of alloc_ordered_workqueue.
	 */
	dev->cec_workqueue = alloc_ordered_workqueue("vivid-%03d-cec",
						     WQ_MEM_RECLAIM, inst);
	if (!dev->cec_workqueue) {
		ret = -ENOMEM;
		goto unreg_dev;
	}

	if (allocators[inst] == 1)
		dma_coerce_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));

	ret = vivid_create_queues(dev);
	if (ret)
		goto unreg_dev;

#ifdef CONFIG_VIDEO_VIVID_CEC
	if (dev->has_vid_cap && in_type_counter[HDMI]) {
		struct cec_adapter *adap;

		adap = vivid_cec_alloc_adap(dev, 0, false);
		ret = PTR_ERR_OR_ZERO(adap);
		if (ret < 0)
			goto unreg_dev;
		dev->cec_rx_adap = adap;
	}

	if (dev->has_vid_out) {
		for (i = 0; i < dev->num_outputs; i++) {
			struct cec_adapter *adap;

			if (dev->output_type[i] != HDMI)
				continue;

			dev->cec_output2bus_map[i] = cec_tx_bus_cnt;
			adap = vivid_cec_alloc_adap(dev, cec_tx_bus_cnt, true);
			ret = PTR_ERR_OR_ZERO(adap);
			if (ret < 0) {
				for (i = 0; i < dev->num_outputs; i++)
					cec_delete_adapter(dev->cec_tx_adap[i]);
				goto unreg_dev;
			}

			dev->cec_tx_adap[cec_tx_bus_cnt] = adap;
			cec_tx_bus_cnt++;
		}
	}
#endif

	v4l2_ctrl_handler_setup(&dev->ctrl_hdl_vid_cap);
	v4l2_ctrl_handler_setup(&dev->ctrl_hdl_vid_out);
	v4l2_ctrl_handler_setup(&dev->ctrl_hdl_vbi_cap);
	v4l2_ctrl_handler_setup(&dev->ctrl_hdl_vbi_out);
	v4l2_ctrl_handler_setup(&dev->ctrl_hdl_radio_rx);
	v4l2_ctrl_handler_setup(&dev->ctrl_hdl_radio_tx);
	v4l2_ctrl_handler_setup(&dev->ctrl_hdl_sdr_cap);
	v4l2_ctrl_handler_setup(&dev->ctrl_hdl_meta_cap);
	v4l2_ctrl_handler_setup(&dev->ctrl_hdl_meta_out);
	v4l2_ctrl_handler_setup(&dev->ctrl_hdl_touch_cap);

	/* finally start creating the device nodes */
	ret = vivid_create_devnodes(pdev, dev, inst, cec_tx_bus_cnt,
				    tvnorms_cap, tvnorms_out,
				    in_type_counter, out_type_counter);
	if (ret)
		goto unreg_dev;

	/* Now that everything is fine, let's add it to device list */
	vivid_devs[inst] = dev;

	return 0;

unreg_dev:
	vb2_video_unregister_device(&dev->touch_cap_dev);
	vb2_video_unregister_device(&dev->meta_out_dev);
	vb2_video_unregister_device(&dev->meta_cap_dev);
	video_unregister_device(&dev->radio_tx_dev);
	video_unregister_device(&dev->radio_rx_dev);
	vb2_video_unregister_device(&dev->sdr_cap_dev);
	vb2_video_unregister_device(&dev->vbi_out_dev);
	vb2_video_unregister_device(&dev->vbi_cap_dev);
	vb2_video_unregister_device(&dev->vid_out_dev);
	vb2_video_unregister_device(&dev->vid_cap_dev);
	cec_unregister_adapter(dev->cec_rx_adap);
	for (i = 0; i < MAX_OUTPUTS; i++)
		cec_unregister_adapter(dev->cec_tx_adap[i]);
	if (dev->cec_workqueue) {
		vivid_cec_bus_free_work(dev);
		destroy_workqueue(dev->cec_workqueue);
	}
free_dev:
	v4l2_device_put(&dev->v4l2_dev);
	return ret;
}

/* This routine allocates from 1 to n_devs virtual drivers.

   The real maximum number of virtual drivers will depend on how many drivers
   will succeed. This is limited to the maximum number of devices that
   videodev supports, which is equal to VIDEO_NUM_DEVICES.
 */
static int vivid_probe(struct platform_device *pdev)
{
	const struct font_desc *font = find_font("VGA8x16");
	int ret = 0, i;

	if (font == NULL) {
		pr_err("vivid: could not find font\n");
		return -ENODEV;
	}

	tpg_set_font(font->data);

	n_devs = clamp_t(unsigned, n_devs, 1, VIVID_MAX_DEVS);

	for (i = 0; i < n_devs; i++) {
		ret = vivid_create_instance(pdev, i);
		if (ret) {
			/* If some instantiations succeeded, keep driver */
			if (i)
				ret = 0;
			break;
		}
	}

	if (ret < 0) {
		pr_err("vivid: error %d while loading driver\n", ret);
		return ret;
	}

	/* n_devs will reflect the actual number of allocated devices */
	n_devs = i;

	return ret;
}

static int vivid_remove(struct platform_device *pdev)
{
	struct vivid_dev *dev;
	unsigned int i, j;

	for (i = 0; i < n_devs; i++) {
		dev = vivid_devs[i];
		if (!dev)
			continue;

		if (dev->disconnect_error)
			vivid_reconnect(dev);
#ifdef CONFIG_MEDIA_CONTROLLER
		media_device_unregister(&dev->mdev);
#endif

		if (dev->has_vid_cap) {
			v4l2_info(&dev->v4l2_dev, "unregistering %s\n",
				video_device_node_name(&dev->vid_cap_dev));
			vb2_video_unregister_device(&dev->vid_cap_dev);
		}
		if (dev->has_vid_out) {
			v4l2_info(&dev->v4l2_dev, "unregistering %s\n",
				video_device_node_name(&dev->vid_out_dev));
			vb2_video_unregister_device(&dev->vid_out_dev);
		}
		if (dev->has_vbi_cap) {
			v4l2_info(&dev->v4l2_dev, "unregistering %s\n",
				video_device_node_name(&dev->vbi_cap_dev));
			vb2_video_unregister_device(&dev->vbi_cap_dev);
		}
		if (dev->has_vbi_out) {
			v4l2_info(&dev->v4l2_dev, "unregistering %s\n",
				video_device_node_name(&dev->vbi_out_dev));
			vb2_video_unregister_device(&dev->vbi_out_dev);
		}
		if (dev->has_sdr_cap) {
			v4l2_info(&dev->v4l2_dev, "unregistering %s\n",
				video_device_node_name(&dev->sdr_cap_dev));
			vb2_video_unregister_device(&dev->sdr_cap_dev);
		}
		if (dev->has_radio_rx) {
			v4l2_info(&dev->v4l2_dev, "unregistering %s\n",
				video_device_node_name(&dev->radio_rx_dev));
			video_unregister_device(&dev->radio_rx_dev);
		}
		if (dev->has_radio_tx) {
			v4l2_info(&dev->v4l2_dev, "unregistering %s\n",
				video_device_node_name(&dev->radio_tx_dev));
			video_unregister_device(&dev->radio_tx_dev);
		}
		if (dev->has_fb) {
			v4l2_info(&dev->v4l2_dev, "unregistering fb%d\n",
				dev->fb_info.node);
			unregister_framebuffer(&dev->fb_info);
			vivid_fb_release_buffers(dev);
		}
		if (dev->has_meta_cap) {
			v4l2_info(&dev->v4l2_dev, "unregistering %s\n",
				  video_device_node_name(&dev->meta_cap_dev));
			vb2_video_unregister_device(&dev->meta_cap_dev);
		}
		if (dev->has_meta_out) {
			v4l2_info(&dev->v4l2_dev, "unregistering %s\n",
				  video_device_node_name(&dev->meta_out_dev));
			vb2_video_unregister_device(&dev->meta_out_dev);
		}
		if (dev->has_touch_cap) {
			v4l2_info(&dev->v4l2_dev, "unregistering %s\n",
				  video_device_node_name(&dev->touch_cap_dev));
			vb2_video_unregister_device(&dev->touch_cap_dev);
		}
		cec_unregister_adapter(dev->cec_rx_adap);
		for (j = 0; j < MAX_OUTPUTS; j++)
			cec_unregister_adapter(dev->cec_tx_adap[j]);
		if (dev->cec_workqueue) {
			vivid_cec_bus_free_work(dev);
			destroy_workqueue(dev->cec_workqueue);
		}
		v4l2_device_put(&dev->v4l2_dev);
		vivid_devs[i] = NULL;
	}
	return 0;
}

static void vivid_pdev_release(struct device *dev)
{
}

static struct platform_device vivid_pdev = {
	.name		= "vivid",
	.dev.release	= vivid_pdev_release,
};

static struct platform_driver vivid_pdrv = {
	.probe		= vivid_probe,
	.remove		= vivid_remove,
	.driver		= {
		.name	= "vivid",
	},
};

static int __init vivid_init(void)
{
	int ret;

	ret = platform_device_register(&vivid_pdev);
	if (ret)
		return ret;

	ret = platform_driver_register(&vivid_pdrv);
	if (ret)
		platform_device_unregister(&vivid_pdev);

	return ret;
}

static void __exit vivid_exit(void)
{
	platform_driver_unregister(&vivid_pdrv);
	platform_device_unregister(&vivid_pdev);
}

module_init(vivid_init);
module_exit(vivid_exit);
