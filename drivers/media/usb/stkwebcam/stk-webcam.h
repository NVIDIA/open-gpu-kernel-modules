/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * stk-webcam.h : Driver for Syntek 1125 USB webcam controller
 *
 * Copyright (C) 2006 Nicolas VIVIEN
 * Copyright 2007-2008 Jaime Velasco Juan <jsagarribay@gmail.com>
 */

#ifndef STKWEBCAM_H
#define STKWEBCAM_H

#include <linux/usb.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-common.h>

#define DRIVER_VERSION		"v0.0.1"
#define DRIVER_VERSION_NUM	0x000001

#define MAX_ISO_BUFS		3
#define ISO_FRAMES_PER_DESC	16
#define ISO_MAX_FRAME_SIZE	3 * 1024
#define ISO_BUFFER_SIZE		(ISO_FRAMES_PER_DESC * ISO_MAX_FRAME_SIZE)

struct stk_iso_buf {
	void *data;
	int length;
	int read;
	struct urb *urb;
};

/* Streaming IO buffers */
struct stk_sio_buffer {
	struct v4l2_buffer v4lbuf;
	char *buffer;
	int mapcount;
	struct stk_camera *dev;
	struct list_head list;
};

enum stk_mode {MODE_VGA, MODE_SXGA, MODE_CIF, MODE_QVGA, MODE_QCIF};

struct stk_video {
	enum stk_mode mode;
	__u32 palette;
	int hflip;
	int vflip;
};

enum stk_status {
	S_PRESENT = 1,
	S_INITIALISED = 2,
	S_MEMALLOCD = 4,
	S_STREAMING = 8,
};
#define is_present(dev)		((dev)->status & S_PRESENT)
#define is_initialised(dev)	((dev)->status & S_INITIALISED)
#define is_streaming(dev)	((dev)->status & S_STREAMING)
#define is_memallocd(dev)	((dev)->status & S_MEMALLOCD)
#define set_present(dev)	((dev)->status = S_PRESENT)
#define unset_present(dev)	((dev)->status &= \
					~(S_PRESENT|S_INITIALISED|S_STREAMING))
#define set_initialised(dev)	((dev)->status |= S_INITIALISED)
#define unset_initialised(dev)	((dev)->status &= ~S_INITIALISED)
#define set_memallocd(dev)	((dev)->status |= S_MEMALLOCD)
#define unset_memallocd(dev)	((dev)->status &= ~S_MEMALLOCD)
#define set_streaming(dev)	((dev)->status |= S_STREAMING)
#define unset_streaming(dev)	((dev)->status &= ~S_STREAMING)

struct regval {
	unsigned reg;
	unsigned val;
};

struct stk_camera {
	struct v4l2_device v4l2_dev;
	struct v4l2_ctrl_handler hdl;
	struct video_device vdev;
	struct usb_device *udev;
	struct usb_interface *interface;
	int webcam_model;
	struct file *owner;
	struct mutex lock;
	int first_init;

	u8 isoc_ep;

	/* Not sure if this is right */
	atomic_t urbs_used;

	struct stk_video vsettings;

	enum stk_status status;

	spinlock_t spinlock;
	wait_queue_head_t wait_frame;

	struct stk_iso_buf *isobufs;

	int frame_size;
	/* Streaming buffers */
	int reading;
	unsigned int n_sbufs;
	struct stk_sio_buffer *sio_bufs;
	struct list_head sio_avail;
	struct list_head sio_full;
	unsigned sequence;
};

#define vdev_to_camera(d) container_of(d, struct stk_camera, vdev)

int stk_camera_write_reg(struct stk_camera *, u16, u8);
int stk_camera_read_reg(struct stk_camera *, u16, u8 *);

int stk_sensor_init(struct stk_camera *);
int stk_sensor_configure(struct stk_camera *);
int stk_sensor_sleep(struct stk_camera *dev);
int stk_sensor_wakeup(struct stk_camera *dev);
int stk_sensor_set_brightness(struct stk_camera *dev, int br);

#endif
