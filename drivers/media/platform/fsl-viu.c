// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright 2008-2010 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 *  Freescale VIU video driver
 *
 *  Authors: Hongjun Chen <hong-jun.chen@freescale.com>
 *	     Porting to 2.6.35 by DENX Software Engineering,
 *	     Anatolij Gustschin <agust@denx.de>
 */

#include <linux/module.h>
#include <linux/clk.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <media/v4l2-common.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-event.h>
#include <media/videobuf-dma-contig.h>

#define DRV_NAME		"fsl_viu"
#define VIU_VERSION		"0.5.1"

#define BUFFER_TIMEOUT		msecs_to_jiffies(500)  /* 0.5 seconds */

#define	VIU_VID_MEM_LIMIT	4	/* Video memory limit, in Mb */

/* I2C address of video decoder chip is 0x4A */
#define VIU_VIDEO_DECODER_ADDR	0x25

static int info_level;

#define dprintk(level, fmt, arg...)					\
	do {								\
		if (level <= info_level)				\
			printk(KERN_DEBUG "viu: " fmt , ## arg);	\
	} while (0)

/*
 * Basic structures
 */
struct viu_fmt {
	u32   fourcc;		/* v4l2 format id */
	u32   pixelformat;
	int   depth;
};

static struct viu_fmt formats[] = {
	{
		.fourcc		= V4L2_PIX_FMT_RGB565,
		.pixelformat	= V4L2_PIX_FMT_RGB565,
		.depth		= 16,
	}, {
		.fourcc		= V4L2_PIX_FMT_RGB32,
		.pixelformat	= V4L2_PIX_FMT_RGB32,
		.depth		= 32,
	}
};

struct viu_dev;
struct viu_buf;

/* buffer for one video frame */
struct viu_buf {
	/* common v4l buffer stuff -- must be first */
	struct videobuf_buffer vb;
	struct viu_fmt *fmt;
};

struct viu_dmaqueue {
	struct viu_dev		*dev;
	struct list_head	active;
	struct list_head	queued;
	struct timer_list	timeout;
};

struct viu_status {
	u32 field_irq;
	u32 vsync_irq;
	u32 hsync_irq;
	u32 vstart_irq;
	u32 dma_end_irq;
	u32 error_irq;
};

struct viu_reg {
	u32 status_cfg;
	u32 luminance;
	u32 chroma_r;
	u32 chroma_g;
	u32 chroma_b;
	u32 field_base_addr;
	u32 dma_inc;
	u32 picture_count;
	u32 req_alarm;
	u32 alpha;
} __attribute__ ((packed));

struct viu_dev {
	struct v4l2_device	v4l2_dev;
	struct v4l2_ctrl_handler hdl;
	struct mutex		lock;
	spinlock_t		slock;
	int			users;

	struct device		*dev;
	/* various device info */
	struct video_device	*vdev;
	struct viu_dmaqueue	vidq;
	enum v4l2_field		capfield;
	int			field;
	int			first;
	int			dma_done;

	/* Hardware register area */
	struct viu_reg __iomem	*vr;

	/* Interrupt vector */
	int			irq;
	struct viu_status	irqs;

	/* video overlay */
	struct v4l2_framebuffer	ovbuf;
	struct viu_fmt		*ovfmt;
	unsigned int		ovenable;
	enum v4l2_field		ovfield;

	/* crop */
	struct v4l2_rect	crop_current;

	/* clock pointer */
	struct clk		*clk;

	/* decoder */
	struct v4l2_subdev	*decoder;

	v4l2_std_id		std;
};

struct viu_fh {
	/* must remain the first field of this struct */
	struct v4l2_fh		fh;
	struct viu_dev		*dev;

	/* video capture */
	struct videobuf_queue	vb_vidq;
	spinlock_t		vbq_lock; /* spinlock for the videobuf queue */

	/* video overlay */
	struct v4l2_window	win;
	struct v4l2_clip	clips[1];

	/* video capture */
	struct viu_fmt		*fmt;
	int			width, height, sizeimage;
	enum v4l2_buf_type	type;
};

static struct viu_reg reg_val;

/*
 * Macro definitions of VIU registers
 */

/* STATUS_CONFIG register */
enum status_config {
	SOFT_RST		= 1 << 0,

	ERR_MASK		= 0x0f << 4,	/* Error code mask */
	ERR_NO			= 0x00,		/* No error */
	ERR_DMA_V		= 0x01 << 4,	/* DMA in vertical active */
	ERR_DMA_VB		= 0x02 << 4,	/* DMA in vertical blanking */
	ERR_LINE_TOO_LONG	= 0x04 << 4,	/* Line too long */
	ERR_TOO_MANG_LINES	= 0x05 << 4,	/* Too many lines in field */
	ERR_LINE_TOO_SHORT	= 0x06 << 4,	/* Line too short */
	ERR_NOT_ENOUGH_LINE	= 0x07 << 4,	/* Not enough lines in field */
	ERR_FIFO_OVERFLOW	= 0x08 << 4,	/* FIFO overflow */
	ERR_FIFO_UNDERFLOW	= 0x09 << 4,	/* FIFO underflow */
	ERR_1bit_ECC		= 0x0a << 4,	/* One bit ECC error */
	ERR_MORE_ECC		= 0x0b << 4,	/* Two/more bits ECC error */

	INT_FIELD_EN		= 0x01 << 8,	/* Enable field interrupt */
	INT_VSYNC_EN		= 0x01 << 9,	/* Enable vsync interrupt */
	INT_HSYNC_EN		= 0x01 << 10,	/* Enable hsync interrupt */
	INT_VSTART_EN		= 0x01 << 11,	/* Enable vstart interrupt */
	INT_DMA_END_EN		= 0x01 << 12,	/* Enable DMA end interrupt */
	INT_ERROR_EN		= 0x01 << 13,	/* Enable error interrupt */
	INT_ECC_EN		= 0x01 << 14,	/* Enable ECC interrupt */

	INT_FIELD_STATUS	= 0x01 << 16,	/* field interrupt status */
	INT_VSYNC_STATUS	= 0x01 << 17,	/* vsync interrupt status */
	INT_HSYNC_STATUS	= 0x01 << 18,	/* hsync interrupt status */
	INT_VSTART_STATUS	= 0x01 << 19,	/* vstart interrupt status */
	INT_DMA_END_STATUS	= 0x01 << 20,	/* DMA end interrupt status */
	INT_ERROR_STATUS	= 0x01 << 21,	/* error interrupt status */

	DMA_ACT			= 0x01 << 27,	/* Enable DMA transfer */
	FIELD_NO		= 0x01 << 28,	/* Field number */
	DITHER_ON		= 0x01 << 29,	/* Dithering is on */
	ROUND_ON		= 0x01 << 30,	/* Round is on */
	MODE_32BIT		= 1UL << 31,	/* Data in RGBa888,
						 * 0 in RGB565
						 */
};

#define norm_maxw()	720
#define norm_maxh()	576

#define INT_ALL_STATUS	(INT_FIELD_STATUS | INT_VSYNC_STATUS | \
			 INT_HSYNC_STATUS | INT_VSTART_STATUS | \
			 INT_DMA_END_STATUS | INT_ERROR_STATUS)

#define NUM_FORMATS	ARRAY_SIZE(formats)

static irqreturn_t viu_intr(int irq, void *dev_id);

static struct viu_fmt *format_by_fourcc(int fourcc)
{
	int i;

	for (i = 0; i < NUM_FORMATS; i++) {
		if (formats[i].pixelformat == fourcc)
			return formats + i;
	}

	dprintk(0, "unknown pixelformat:'%4.4s'\n", (char *)&fourcc);
	return NULL;
}

static void viu_start_dma(struct viu_dev *dev)
{
	struct viu_reg __iomem *vr = dev->vr;

	dev->field = 0;

	/* Enable DMA operation */
	iowrite32be(SOFT_RST, &vr->status_cfg);
	iowrite32be(INT_FIELD_EN, &vr->status_cfg);
}

static void viu_stop_dma(struct viu_dev *dev)
{
	struct viu_reg __iomem *vr = dev->vr;
	int cnt = 100;
	u32 status_cfg;

	iowrite32be(0, &vr->status_cfg);

	/* Clear pending interrupts */
	status_cfg = ioread32be(&vr->status_cfg);
	if (status_cfg & 0x3f0000)
		iowrite32be(status_cfg & 0x3f0000, &vr->status_cfg);

	if (status_cfg & DMA_ACT) {
		do {
			status_cfg = ioread32be(&vr->status_cfg);
			if (status_cfg & INT_DMA_END_STATUS)
				break;
		} while (cnt--);

		if (cnt < 0) {
			/* timed out, issue soft reset */
			iowrite32be(SOFT_RST, &vr->status_cfg);
			iowrite32be(0, &vr->status_cfg);
		} else {
			/* clear DMA_END and other pending irqs */
			iowrite32be(status_cfg & 0x3f0000, &vr->status_cfg);
		}
	}

	dev->field = 0;
}

static int restart_video_queue(struct viu_dmaqueue *vidq)
{
	struct viu_buf *buf, *prev;

	dprintk(1, "%s vidq=%p\n", __func__, vidq);
	if (!list_empty(&vidq->active)) {
		buf = list_entry(vidq->active.next, struct viu_buf, vb.queue);
		dprintk(2, "restart_queue [%p/%d]: restart dma\n",
			buf, buf->vb.i);

		viu_stop_dma(vidq->dev);

		/* cancel all outstanding capture requests */
		list_for_each_entry_safe(buf, prev, &vidq->active, vb.queue) {
			list_del(&buf->vb.queue);
			buf->vb.state = VIDEOBUF_ERROR;
			wake_up(&buf->vb.done);
		}
		mod_timer(&vidq->timeout, jiffies+BUFFER_TIMEOUT);
		return 0;
	}

	prev = NULL;
	for (;;) {
		if (list_empty(&vidq->queued))
			return 0;
		buf = list_entry(vidq->queued.next, struct viu_buf, vb.queue);
		if (prev == NULL) {
			list_move_tail(&buf->vb.queue, &vidq->active);

			dprintk(1, "Restarting video dma\n");
			viu_stop_dma(vidq->dev);
			viu_start_dma(vidq->dev);

			buf->vb.state = VIDEOBUF_ACTIVE;
			mod_timer(&vidq->timeout, jiffies+BUFFER_TIMEOUT);
			dprintk(2, "[%p/%d] restart_queue - first active\n",
				buf, buf->vb.i);

		} else if (prev->vb.width  == buf->vb.width  &&
			   prev->vb.height == buf->vb.height &&
			   prev->fmt       == buf->fmt) {
			list_move_tail(&buf->vb.queue, &vidq->active);
			buf->vb.state = VIDEOBUF_ACTIVE;
			dprintk(2, "[%p/%d] restart_queue - move to active\n",
				buf, buf->vb.i);
		} else {
			return 0;
		}
		prev = buf;
	}
}

static void viu_vid_timeout(struct timer_list *t)
{
	struct viu_dev *dev = from_timer(dev, t, vidq.timeout);
	struct viu_buf *buf;
	struct viu_dmaqueue *vidq = &dev->vidq;

	while (!list_empty(&vidq->active)) {
		buf = list_entry(vidq->active.next, struct viu_buf, vb.queue);
		list_del(&buf->vb.queue);
		buf->vb.state = VIDEOBUF_ERROR;
		wake_up(&buf->vb.done);
		dprintk(1, "viu/0: [%p/%d] timeout\n", buf, buf->vb.i);
	}

	restart_video_queue(vidq);
}

/*
 * Videobuf operations
 */
static int buffer_setup(struct videobuf_queue *vq, unsigned int *count,
			unsigned int *size)
{
	struct viu_fh *fh = vq->priv_data;

	*size = fh->width * fh->height * fh->fmt->depth >> 3;
	if (*count == 0)
		*count = 32;

	while (*size * *count > VIU_VID_MEM_LIMIT * 1024 * 1024)
		(*count)--;

	dprintk(1, "%s, count=%d, size=%d\n", __func__, *count, *size);
	return 0;
}

static void free_buffer(struct videobuf_queue *vq, struct viu_buf *buf)
{
	struct videobuf_buffer *vb = &buf->vb;
	void *vaddr = NULL;

	videobuf_waiton(vq, &buf->vb, 0, 0);

	if (vq->int_ops && vq->int_ops->vaddr)
		vaddr = vq->int_ops->vaddr(vb);

	if (vaddr)
		videobuf_dma_contig_free(vq, &buf->vb);

	buf->vb.state = VIDEOBUF_NEEDS_INIT;
}

inline int buffer_activate(struct viu_dev *dev, struct viu_buf *buf)
{
	struct viu_reg __iomem *vr = dev->vr;
	int bpp;

	/* setup the DMA base address */
	reg_val.field_base_addr = videobuf_to_dma_contig(&buf->vb);

	dprintk(1, "buffer_activate [%p/%d]: dma addr 0x%lx\n",
		buf, buf->vb.i, (unsigned long)reg_val.field_base_addr);

	/* interlace is on by default, set horizontal DMA increment */
	reg_val.status_cfg = 0;
	bpp = buf->fmt->depth >> 3;
	switch (bpp) {
	case 2:
		reg_val.status_cfg &= ~MODE_32BIT;
		reg_val.dma_inc = buf->vb.width * 2;
		break;
	case 4:
		reg_val.status_cfg |= MODE_32BIT;
		reg_val.dma_inc = buf->vb.width * 4;
		break;
	default:
		dprintk(0, "doesn't support color depth(%d)\n",
			bpp * 8);
		return -EINVAL;
	}

	/* setup picture_count register */
	reg_val.picture_count = (buf->vb.height / 2) << 16 |
				buf->vb.width;

	reg_val.status_cfg |= DMA_ACT | INT_DMA_END_EN | INT_FIELD_EN;

	buf->vb.state = VIDEOBUF_ACTIVE;
	dev->capfield = buf->vb.field;

	/* reset dma increment if needed */
	if (!V4L2_FIELD_HAS_BOTH(buf->vb.field))
		reg_val.dma_inc = 0;

	iowrite32be(reg_val.dma_inc, &vr->dma_inc);
	iowrite32be(reg_val.picture_count, &vr->picture_count);
	iowrite32be(reg_val.field_base_addr, &vr->field_base_addr);
	mod_timer(&dev->vidq.timeout, jiffies + BUFFER_TIMEOUT);
	return 0;
}

static int buffer_prepare(struct videobuf_queue *vq,
			  struct videobuf_buffer *vb,
			  enum v4l2_field field)
{
	struct viu_fh  *fh  = vq->priv_data;
	struct viu_buf *buf = container_of(vb, struct viu_buf, vb);
	int rc;

	BUG_ON(fh->fmt == NULL);

	if (fh->width  < 48 || fh->width  > norm_maxw() ||
	    fh->height < 32 || fh->height > norm_maxh())
		return -EINVAL;
	buf->vb.size = (fh->width * fh->height * fh->fmt->depth) >> 3;
	if (buf->vb.baddr != 0 && buf->vb.bsize < buf->vb.size)
		return -EINVAL;

	if (buf->fmt       != fh->fmt	 ||
	    buf->vb.width  != fh->width  ||
	    buf->vb.height != fh->height ||
	    buf->vb.field  != field) {
		buf->fmt       = fh->fmt;
		buf->vb.width  = fh->width;
		buf->vb.height = fh->height;
		buf->vb.field  = field;
	}

	if (buf->vb.state == VIDEOBUF_NEEDS_INIT) {
		rc = videobuf_iolock(vq, &buf->vb, NULL);
		if (rc != 0)
			goto fail;

		buf->vb.width  = fh->width;
		buf->vb.height = fh->height;
		buf->vb.field  = field;
		buf->fmt       = fh->fmt;
	}

	buf->vb.state = VIDEOBUF_PREPARED;
	return 0;

fail:
	free_buffer(vq, buf);
	return rc;
}

static void buffer_queue(struct videobuf_queue *vq, struct videobuf_buffer *vb)
{
	struct viu_buf       *buf     = container_of(vb, struct viu_buf, vb);
	struct viu_fh        *fh      = vq->priv_data;
	struct viu_dev       *dev     = fh->dev;
	struct viu_dmaqueue  *vidq    = &dev->vidq;
	struct viu_buf       *prev;

	if (!list_empty(&vidq->queued)) {
		dprintk(1, "adding vb queue=%p\n", &buf->vb.queue);
		dprintk(1, "vidq pointer 0x%p, queued 0x%p\n",
				vidq, &vidq->queued);
		dprintk(1, "dev %p, queued: self %p, next %p, head %p\n",
			dev, &vidq->queued, vidq->queued.next,
			vidq->queued.prev);
		list_add_tail(&buf->vb.queue, &vidq->queued);
		buf->vb.state = VIDEOBUF_QUEUED;
		dprintk(2, "[%p/%d] buffer_queue - append to queued\n",
			buf, buf->vb.i);
	} else if (list_empty(&vidq->active)) {
		dprintk(1, "adding vb active=%p\n", &buf->vb.queue);
		list_add_tail(&buf->vb.queue, &vidq->active);
		buf->vb.state = VIDEOBUF_ACTIVE;
		mod_timer(&vidq->timeout, jiffies+BUFFER_TIMEOUT);
		dprintk(2, "[%p/%d] buffer_queue - first active\n",
			buf, buf->vb.i);

		buffer_activate(dev, buf);
	} else {
		dprintk(1, "adding vb queue2=%p\n", &buf->vb.queue);
		prev = list_entry(vidq->active.prev, struct viu_buf, vb.queue);
		if (prev->vb.width  == buf->vb.width  &&
		    prev->vb.height == buf->vb.height &&
		    prev->fmt       == buf->fmt) {
			list_add_tail(&buf->vb.queue, &vidq->active);
			buf->vb.state = VIDEOBUF_ACTIVE;
			dprintk(2, "[%p/%d] buffer_queue - append to active\n",
				buf, buf->vb.i);
		} else {
			list_add_tail(&buf->vb.queue, &vidq->queued);
			buf->vb.state = VIDEOBUF_QUEUED;
			dprintk(2, "[%p/%d] buffer_queue - first queued\n",
				buf, buf->vb.i);
		}
	}
}

static void buffer_release(struct videobuf_queue *vq,
				struct videobuf_buffer *vb)
{
	struct viu_buf *buf  = container_of(vb, struct viu_buf, vb);
	struct viu_fh  *fh   = vq->priv_data;
	struct viu_dev *dev  = (struct viu_dev *)fh->dev;

	viu_stop_dma(dev);
	free_buffer(vq, buf);
}

static const struct videobuf_queue_ops viu_video_qops = {
	.buf_setup      = buffer_setup,
	.buf_prepare    = buffer_prepare,
	.buf_queue      = buffer_queue,
	.buf_release    = buffer_release,
};

/*
 * IOCTL vidioc handling
 */
static int vidioc_querycap(struct file *file, void *priv,
			   struct v4l2_capability *cap)
{
	strscpy(cap->driver, "viu", sizeof(cap->driver));
	strscpy(cap->card, "viu", sizeof(cap->card));
	strscpy(cap->bus_info, "platform:viu", sizeof(cap->bus_info));
	return 0;
}

static int vidioc_enum_fmt(struct file *file, void  *priv,
					struct v4l2_fmtdesc *f)
{
	int index = f->index;

	if (f->index >= NUM_FORMATS)
		return -EINVAL;

	f->pixelformat = formats[index].fourcc;
	return 0;
}

static int vidioc_g_fmt_cap(struct file *file, void *priv,
					struct v4l2_format *f)
{
	struct viu_fh *fh = priv;

	f->fmt.pix.width        = fh->width;
	f->fmt.pix.height       = fh->height;
	f->fmt.pix.field        = fh->vb_vidq.field;
	f->fmt.pix.pixelformat  = fh->fmt->pixelformat;
	f->fmt.pix.bytesperline =
			(f->fmt.pix.width * fh->fmt->depth) >> 3;
	f->fmt.pix.sizeimage	= fh->sizeimage;
	f->fmt.pix.colorspace	= V4L2_COLORSPACE_SMPTE170M;
	return 0;
}

static int vidioc_try_fmt_cap(struct file *file, void *priv,
					struct v4l2_format *f)
{
	struct viu_fmt *fmt;
	unsigned int maxw, maxh;

	fmt = format_by_fourcc(f->fmt.pix.pixelformat);
	if (!fmt) {
		dprintk(1, "Fourcc format (0x%08x) invalid.",
			f->fmt.pix.pixelformat);
		return -EINVAL;
	}

	maxw  = norm_maxw();
	maxh  = norm_maxh();

	f->fmt.pix.field = V4L2_FIELD_INTERLACED;
	if (f->fmt.pix.height < 32)
		f->fmt.pix.height = 32;
	if (f->fmt.pix.height > maxh)
		f->fmt.pix.height = maxh;
	if (f->fmt.pix.width < 48)
		f->fmt.pix.width = 48;
	if (f->fmt.pix.width > maxw)
		f->fmt.pix.width = maxw;
	f->fmt.pix.width &= ~0x03;
	f->fmt.pix.bytesperline =
		(f->fmt.pix.width * fmt->depth) >> 3;
	f->fmt.pix.sizeimage = f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;

	return 0;
}

static int vidioc_s_fmt_cap(struct file *file, void *priv,
					struct v4l2_format *f)
{
	struct viu_fh *fh = priv;
	int ret;

	ret = vidioc_try_fmt_cap(file, fh, f);
	if (ret < 0)
		return ret;

	fh->fmt           = format_by_fourcc(f->fmt.pix.pixelformat);
	fh->width         = f->fmt.pix.width;
	fh->height        = f->fmt.pix.height;
	fh->sizeimage     = f->fmt.pix.sizeimage;
	fh->vb_vidq.field = f->fmt.pix.field;
	fh->type          = f->type;
	return 0;
}

static int vidioc_g_fmt_overlay(struct file *file, void *priv,
					struct v4l2_format *f)
{
	struct viu_fh *fh = priv;

	f->fmt.win = fh->win;
	return 0;
}

static int verify_preview(struct viu_dev *dev, struct v4l2_window *win)
{
	enum v4l2_field field;
	int maxw, maxh;

	if (dev->ovbuf.base == NULL)
		return -EINVAL;
	if (dev->ovfmt == NULL)
		return -EINVAL;
	if (win->w.width < 48 || win->w.height < 32)
		return -EINVAL;

	field = win->field;
	maxw  = dev->crop_current.width;
	maxh  = dev->crop_current.height;

	if (field == V4L2_FIELD_ANY) {
		field = (win->w.height > maxh/2)
			? V4L2_FIELD_INTERLACED
			: V4L2_FIELD_TOP;
	}
	switch (field) {
	case V4L2_FIELD_TOP:
	case V4L2_FIELD_BOTTOM:
		maxh = maxh / 2;
		break;
	case V4L2_FIELD_INTERLACED:
		break;
	default:
		return -EINVAL;
	}

	win->field = field;
	if (win->w.width > maxw)
		win->w.width = maxw;
	if (win->w.height > maxh)
		win->w.height = maxh;
	return 0;
}

inline void viu_activate_overlay(struct viu_reg __iomem *vr)
{
	iowrite32be(reg_val.field_base_addr, &vr->field_base_addr);
	iowrite32be(reg_val.dma_inc, &vr->dma_inc);
	iowrite32be(reg_val.picture_count, &vr->picture_count);
}

static int viu_setup_preview(struct viu_dev *dev, struct viu_fh *fh)
{
	int bpp;

	dprintk(1, "%s %dx%d\n", __func__,
		fh->win.w.width, fh->win.w.height);

	reg_val.status_cfg = 0;

	/* setup window */
	reg_val.picture_count = (fh->win.w.height / 2) << 16 |
				fh->win.w.width;

	/* setup color depth and dma increment */
	bpp = dev->ovfmt->depth / 8;
	switch (bpp) {
	case 2:
		reg_val.status_cfg &= ~MODE_32BIT;
		reg_val.dma_inc = fh->win.w.width * 2;
		break;
	case 4:
		reg_val.status_cfg |= MODE_32BIT;
		reg_val.dma_inc = fh->win.w.width * 4;
		break;
	default:
		dprintk(0, "device doesn't support color depth(%d)\n",
			bpp * 8);
		return -EINVAL;
	}

	dev->ovfield = fh->win.field;
	if (!V4L2_FIELD_HAS_BOTH(dev->ovfield))
		reg_val.dma_inc = 0;

	reg_val.status_cfg |= DMA_ACT | INT_DMA_END_EN | INT_FIELD_EN;

	/* setup the base address of the overlay buffer */
	reg_val.field_base_addr = (u32)(long)dev->ovbuf.base;

	return 0;
}

static int vidioc_s_fmt_overlay(struct file *file, void *priv,
					struct v4l2_format *f)
{
	struct viu_fh  *fh  = priv;
	struct viu_dev *dev = (struct viu_dev *)fh->dev;
	unsigned long  flags;
	int err;

	err = verify_preview(dev, &f->fmt.win);
	if (err)
		return err;

	fh->win = f->fmt.win;

	spin_lock_irqsave(&dev->slock, flags);
	viu_setup_preview(dev, fh);
	spin_unlock_irqrestore(&dev->slock, flags);
	return 0;
}

static int vidioc_try_fmt_overlay(struct file *file, void *priv,
					struct v4l2_format *f)
{
	return 0;
}

static int vidioc_overlay(struct file *file, void *priv, unsigned int on)
{
	struct viu_fh  *fh  = priv;
	struct viu_dev *dev = (struct viu_dev *)fh->dev;
	unsigned long  flags;

	if (on) {
		spin_lock_irqsave(&dev->slock, flags);
		viu_activate_overlay(dev->vr);
		dev->ovenable = 1;

		/* start dma */
		viu_start_dma(dev);
		spin_unlock_irqrestore(&dev->slock, flags);
	} else {
		viu_stop_dma(dev);
		dev->ovenable = 0;
	}

	return 0;
}

static int vidioc_g_fbuf(struct file *file, void *priv, struct v4l2_framebuffer *arg)
{
	struct viu_fh  *fh = priv;
	struct viu_dev *dev = fh->dev;
	struct v4l2_framebuffer *fb = arg;

	*fb = dev->ovbuf;
	fb->capability = V4L2_FBUF_CAP_LIST_CLIPPING;
	return 0;
}

static int vidioc_s_fbuf(struct file *file, void *priv, const struct v4l2_framebuffer *arg)
{
	struct viu_fh  *fh = priv;
	struct viu_dev *dev = fh->dev;
	const struct v4l2_framebuffer *fb = arg;
	struct viu_fmt *fmt;

	if (!capable(CAP_SYS_ADMIN) && !capable(CAP_SYS_RAWIO))
		return -EPERM;

	/* check args */
	fmt = format_by_fourcc(fb->fmt.pixelformat);
	if (fmt == NULL)
		return -EINVAL;

	/* ok, accept it */
	dev->ovbuf = *fb;
	dev->ovfmt = fmt;
	if (dev->ovbuf.fmt.bytesperline == 0) {
		dev->ovbuf.fmt.bytesperline =
			dev->ovbuf.fmt.width * fmt->depth / 8;
	}
	return 0;
}

static int vidioc_reqbufs(struct file *file, void *priv,
				struct v4l2_requestbuffers *p)
{
	struct viu_fh *fh = priv;

	return videobuf_reqbufs(&fh->vb_vidq, p);
}

static int vidioc_querybuf(struct file *file, void *priv,
					struct v4l2_buffer *p)
{
	struct viu_fh *fh = priv;

	return videobuf_querybuf(&fh->vb_vidq, p);
}

static int vidioc_qbuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
	struct viu_fh *fh = priv;

	return videobuf_qbuf(&fh->vb_vidq, p);
}

static int vidioc_dqbuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
	struct viu_fh *fh = priv;

	return videobuf_dqbuf(&fh->vb_vidq, p,
				file->f_flags & O_NONBLOCK);
}

static int vidioc_streamon(struct file *file, void *priv, enum v4l2_buf_type i)
{
	struct viu_fh *fh = priv;
	struct viu_dev *dev = fh->dev;

	if (fh->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	if (fh->type != i)
		return -EINVAL;

	if (dev->ovenable)
		dev->ovenable = 0;

	viu_start_dma(fh->dev);

	return videobuf_streamon(&fh->vb_vidq);
}

static int vidioc_streamoff(struct file *file, void *priv, enum v4l2_buf_type i)
{
	struct viu_fh  *fh = priv;

	if (fh->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	if (fh->type != i)
		return -EINVAL;

	viu_stop_dma(fh->dev);

	return videobuf_streamoff(&fh->vb_vidq);
}

#define decoder_call(viu, o, f, args...) \
	v4l2_subdev_call(viu->decoder, o, f, ##args)

static int vidioc_querystd(struct file *file, void *priv, v4l2_std_id *std_id)
{
	struct viu_fh *fh = priv;

	decoder_call(fh->dev, video, querystd, std_id);
	return 0;
}

static int vidioc_s_std(struct file *file, void *priv, v4l2_std_id id)
{
	struct viu_fh *fh = priv;

	fh->dev->std = id;
	decoder_call(fh->dev, video, s_std, id);
	return 0;
}

static int vidioc_g_std(struct file *file, void *priv, v4l2_std_id *std_id)
{
	struct viu_fh *fh = priv;

	*std_id = fh->dev->std;
	return 0;
}

/* only one input in this driver */
static int vidioc_enum_input(struct file *file, void *priv,
					struct v4l2_input *inp)
{
	struct viu_fh *fh = priv;

	if (inp->index != 0)
		return -EINVAL;

	inp->type = V4L2_INPUT_TYPE_CAMERA;
	inp->std = fh->dev->vdev->tvnorms;
	strscpy(inp->name, "Camera", sizeof(inp->name));
	return 0;
}

static int vidioc_g_input(struct file *file, void *priv, unsigned int *i)
{
	*i = 0;
	return 0;
}

static int vidioc_s_input(struct file *file, void *priv, unsigned int i)
{
	struct viu_fh *fh = priv;

	if (i)
		return -EINVAL;

	decoder_call(fh->dev, video, s_routing, i, 0, 0);
	return 0;
}

inline void viu_activate_next_buf(struct viu_dev *dev,
				struct viu_dmaqueue *viuq)
{
	struct viu_dmaqueue *vidq = viuq;
	struct viu_buf *buf;

	/* launch another DMA operation for an active/queued buffer */
	if (!list_empty(&vidq->active)) {
		buf = list_entry(vidq->active.next, struct viu_buf,
					vb.queue);
		dprintk(1, "start another queued buffer: 0x%p\n", buf);
		buffer_activate(dev, buf);
	} else if (!list_empty(&vidq->queued)) {
		buf = list_entry(vidq->queued.next, struct viu_buf,
					vb.queue);
		list_del(&buf->vb.queue);

		dprintk(1, "start another queued buffer: 0x%p\n", buf);
		list_add_tail(&buf->vb.queue, &vidq->active);
		buf->vb.state = VIDEOBUF_ACTIVE;
		buffer_activate(dev, buf);
	}
}

inline void viu_default_settings(struct viu_reg __iomem *vr)
{
	iowrite32be(0x9512A254, &vr->luminance);
	iowrite32be(0x03310000, &vr->chroma_r);
	iowrite32be(0x06600F38, &vr->chroma_g);
	iowrite32be(0x00000409, &vr->chroma_b);
	iowrite32be(0x000000ff, &vr->alpha);
	iowrite32be(0x00000090, &vr->req_alarm);
	dprintk(1, "status reg: 0x%08x, field base: 0x%08x\n",
		ioread32be(&vr->status_cfg), ioread32be(&vr->field_base_addr));
}

static void viu_overlay_intr(struct viu_dev *dev, u32 status)
{
	struct viu_reg __iomem *vr = dev->vr;

	if (status & INT_DMA_END_STATUS)
		dev->dma_done = 1;

	if (status & INT_FIELD_STATUS) {
		if (dev->dma_done) {
			u32 addr = reg_val.field_base_addr;

			dev->dma_done = 0;
			if (status & FIELD_NO)
				addr += reg_val.dma_inc;

			iowrite32be(addr, &vr->field_base_addr);
			iowrite32be(reg_val.dma_inc, &vr->dma_inc);
			iowrite32be((status & 0xffc0ffff) |
				 (status & INT_ALL_STATUS) |
				 reg_val.status_cfg, &vr->status_cfg);
		} else if (status & INT_VSYNC_STATUS) {
			iowrite32be((status & 0xffc0ffff) |
				 (status & INT_ALL_STATUS) |
				 reg_val.status_cfg, &vr->status_cfg);
		}
	}
}

static void viu_capture_intr(struct viu_dev *dev, u32 status)
{
	struct viu_dmaqueue *vidq = &dev->vidq;
	struct viu_reg __iomem *vr = dev->vr;
	struct viu_buf *buf;
	int field_num;
	int need_two;
	int dma_done = 0;

	field_num = status & FIELD_NO;
	need_two = V4L2_FIELD_HAS_BOTH(dev->capfield);

	if (status & INT_DMA_END_STATUS) {
		dma_done = 1;
		if (((field_num == 0) && (dev->field == 0)) ||
		    (field_num && (dev->field == 1)))
			dev->field++;
	}

	if (status & INT_FIELD_STATUS) {
		dprintk(1, "irq: field %d, done %d\n",
			!!field_num, dma_done);
		if (unlikely(dev->first)) {
			if (field_num == 0) {
				dev->first = 0;
				dprintk(1, "activate first buf\n");
				viu_activate_next_buf(dev, vidq);
			} else
				dprintk(1, "wait field 0\n");
			return;
		}

		/* setup buffer address for next dma operation */
		if (!list_empty(&vidq->active)) {
			u32 addr = reg_val.field_base_addr;

			if (field_num && need_two) {
				addr += reg_val.dma_inc;
				dprintk(1, "field 1, 0x%lx, dev field %d\n",
					(unsigned long)addr, dev->field);
			}
			iowrite32be(addr, &vr->field_base_addr);
			iowrite32be(reg_val.dma_inc, &vr->dma_inc);
			iowrite32be((status & 0xffc0ffff) |
				 (status & INT_ALL_STATUS) |
				 reg_val.status_cfg, &vr->status_cfg);
			return;
		}
	}

	if (dma_done && field_num && (dev->field == 2)) {
		dev->field = 0;
		buf = list_entry(vidq->active.next,
				 struct viu_buf, vb.queue);
		dprintk(1, "viu/0: [%p/%d] 0x%lx/0x%lx: dma complete\n",
			buf, buf->vb.i,
			(unsigned long)videobuf_to_dma_contig(&buf->vb),
			(unsigned long)ioread32be(&vr->field_base_addr));

		if (waitqueue_active(&buf->vb.done)) {
			list_del(&buf->vb.queue);
			buf->vb.ts = ktime_get_ns();
			buf->vb.state = VIDEOBUF_DONE;
			buf->vb.field_count++;
			wake_up(&buf->vb.done);
		}
		/* activate next dma buffer */
		viu_activate_next_buf(dev, vidq);
	}
}

static irqreturn_t viu_intr(int irq, void *dev_id)
{
	struct viu_dev *dev  = (struct viu_dev *)dev_id;
	struct viu_reg __iomem *vr = dev->vr;
	u32 status;
	u32 error;

	status = ioread32be(&vr->status_cfg);

	if (status & INT_ERROR_STATUS) {
		dev->irqs.error_irq++;
		error = status & ERR_MASK;
		if (error)
			dprintk(1, "Err: error(%d), times:%d!\n",
				error >> 4, dev->irqs.error_irq);
		/* Clear interrupt error bit and error flags */
		iowrite32be((status & 0xffc0ffff) | INT_ERROR_STATUS,
			    &vr->status_cfg);
	}

	if (status & INT_DMA_END_STATUS) {
		dev->irqs.dma_end_irq++;
		dev->dma_done = 1;
		dprintk(2, "VIU DMA end interrupt times: %d\n",
					dev->irqs.dma_end_irq);
	}

	if (status & INT_HSYNC_STATUS)
		dev->irqs.hsync_irq++;

	if (status & INT_FIELD_STATUS) {
		dev->irqs.field_irq++;
		dprintk(2, "VIU field interrupt times: %d\n",
					dev->irqs.field_irq);
	}

	if (status & INT_VSTART_STATUS)
		dev->irqs.vstart_irq++;

	if (status & INT_VSYNC_STATUS) {
		dev->irqs.vsync_irq++;
		dprintk(2, "VIU vsync interrupt times: %d\n",
			dev->irqs.vsync_irq);
	}

	/* clear all pending irqs */
	status = ioread32be(&vr->status_cfg);
	iowrite32be((status & 0xffc0ffff) | (status & INT_ALL_STATUS),
		    &vr->status_cfg);

	if (dev->ovenable) {
		viu_overlay_intr(dev, status);
		return IRQ_HANDLED;
	}

	/* Capture mode */
	viu_capture_intr(dev, status);
	return IRQ_HANDLED;
}

/*
 * File operations for the device
 */
static int viu_open(struct file *file)
{
	struct video_device *vdev = video_devdata(file);
	struct viu_dev *dev = video_get_drvdata(vdev);
	struct viu_fh *fh;
	struct viu_reg __iomem *vr;
	int minor = vdev->minor;
	u32 status_cfg;

	dprintk(1, "viu: open (minor=%d)\n", minor);

	dev->users++;
	if (dev->users > 1) {
		dev->users--;
		return -EBUSY;
	}

	vr = dev->vr;

	dprintk(1, "open minor=%d type=%s users=%d\n", minor,
		v4l2_type_names[V4L2_BUF_TYPE_VIDEO_CAPTURE], dev->users);

	if (mutex_lock_interruptible(&dev->lock)) {
		dev->users--;
		return -ERESTARTSYS;
	}

	/* allocate and initialize per filehandle data */
	fh = kzalloc(sizeof(*fh), GFP_KERNEL);
	if (!fh) {
		dev->users--;
		mutex_unlock(&dev->lock);
		return -ENOMEM;
	}

	v4l2_fh_init(&fh->fh, vdev);
	file->private_data = fh;
	fh->dev = dev;

	fh->type     = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fh->fmt      = format_by_fourcc(V4L2_PIX_FMT_RGB32);
	fh->width    = norm_maxw();
	fh->height   = norm_maxh();
	dev->crop_current.width  = fh->width;
	dev->crop_current.height = fh->height;

	dprintk(1, "Open: fh=%p, dev=%p, dev->vidq=%p\n", fh, dev, &dev->vidq);
	dprintk(1, "Open: list_empty queued=%d\n",
		list_empty(&dev->vidq.queued));
	dprintk(1, "Open: list_empty active=%d\n",
		list_empty(&dev->vidq.active));

	viu_default_settings(vr);

	status_cfg = ioread32be(&vr->status_cfg);
	iowrite32be(status_cfg & ~(INT_VSYNC_EN | INT_HSYNC_EN |
				INT_FIELD_EN | INT_VSTART_EN |
				INT_DMA_END_EN | INT_ERROR_EN | INT_ECC_EN),
		    &vr->status_cfg);

	status_cfg = ioread32be(&vr->status_cfg);
	iowrite32be(status_cfg | INT_ALL_STATUS, &vr->status_cfg);

	spin_lock_init(&fh->vbq_lock);
	videobuf_queue_dma_contig_init(&fh->vb_vidq, &viu_video_qops,
				       dev->dev, &fh->vbq_lock,
				       fh->type, V4L2_FIELD_INTERLACED,
				       sizeof(struct viu_buf), fh,
				       &fh->dev->lock);
	v4l2_fh_add(&fh->fh);
	mutex_unlock(&dev->lock);
	return 0;
}

static ssize_t viu_read(struct file *file, char __user *data, size_t count,
			loff_t *ppos)
{
	struct viu_fh *fh = file->private_data;
	struct viu_dev *dev = fh->dev;
	int ret = 0;

	dprintk(2, "%s\n", __func__);
	if (dev->ovenable)
		dev->ovenable = 0;

	if (fh->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
		if (mutex_lock_interruptible(&dev->lock))
			return -ERESTARTSYS;
		viu_start_dma(dev);
		ret = videobuf_read_stream(&fh->vb_vidq, data, count,
				ppos, 0, file->f_flags & O_NONBLOCK);
		mutex_unlock(&dev->lock);
		return ret;
	}
	return 0;
}

static __poll_t viu_poll(struct file *file, struct poll_table_struct *wait)
{
	struct viu_fh *fh = file->private_data;
	struct videobuf_queue *q = &fh->vb_vidq;
	struct viu_dev *dev = fh->dev;
	__poll_t req_events = poll_requested_events(wait);
	__poll_t res = v4l2_ctrl_poll(file, wait);

	if (V4L2_BUF_TYPE_VIDEO_CAPTURE != fh->type)
		return EPOLLERR;

	if (!(req_events & (EPOLLIN | EPOLLRDNORM)))
		return res;

	mutex_lock(&dev->lock);
	res |= videobuf_poll_stream(file, q, wait);
	mutex_unlock(&dev->lock);
	return res;
}

static int viu_release(struct file *file)
{
	struct viu_fh *fh = file->private_data;
	struct viu_dev *dev = fh->dev;
	int minor = video_devdata(file)->minor;

	mutex_lock(&dev->lock);
	viu_stop_dma(dev);
	videobuf_stop(&fh->vb_vidq);
	videobuf_mmap_free(&fh->vb_vidq);
	v4l2_fh_del(&fh->fh);
	v4l2_fh_exit(&fh->fh);
	mutex_unlock(&dev->lock);

	kfree(fh);

	dev->users--;
	dprintk(1, "close (minor=%d, users=%d)\n",
		minor, dev->users);
	return 0;
}

static void viu_reset(struct viu_reg __iomem *reg)
{
	iowrite32be(0, &reg->status_cfg);
	iowrite32be(0x9512a254, &reg->luminance);
	iowrite32be(0x03310000, &reg->chroma_r);
	iowrite32be(0x06600f38, &reg->chroma_g);
	iowrite32be(0x00000409, &reg->chroma_b);
	iowrite32be(0, &reg->field_base_addr);
	iowrite32be(0, &reg->dma_inc);
	iowrite32be(0x01e002d0, &reg->picture_count);
	iowrite32be(0x00000090, &reg->req_alarm);
	iowrite32be(0x000000ff, &reg->alpha);
}

static int viu_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct viu_fh *fh = file->private_data;
	struct viu_dev *dev = fh->dev;
	int ret;

	dprintk(1, "mmap called, vma=%p\n", vma);

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;
	ret = videobuf_mmap_mapper(&fh->vb_vidq, vma);
	mutex_unlock(&dev->lock);

	dprintk(1, "vma start=0x%08lx, size=%ld, ret=%d\n",
		(unsigned long)vma->vm_start,
		(unsigned long)vma->vm_end-(unsigned long)vma->vm_start,
		ret);

	return ret;
}

static const struct v4l2_file_operations viu_fops = {
	.owner		= THIS_MODULE,
	.open		= viu_open,
	.release	= viu_release,
	.read		= viu_read,
	.poll		= viu_poll,
	.unlocked_ioctl	= video_ioctl2, /* V4L2 ioctl handler */
	.mmap		= viu_mmap,
};

static const struct v4l2_ioctl_ops viu_ioctl_ops = {
	.vidioc_querycap	= vidioc_querycap,
	.vidioc_enum_fmt_vid_cap  = vidioc_enum_fmt,
	.vidioc_g_fmt_vid_cap     = vidioc_g_fmt_cap,
	.vidioc_try_fmt_vid_cap   = vidioc_try_fmt_cap,
	.vidioc_s_fmt_vid_cap     = vidioc_s_fmt_cap,
	.vidioc_enum_fmt_vid_overlay = vidioc_enum_fmt,
	.vidioc_g_fmt_vid_overlay = vidioc_g_fmt_overlay,
	.vidioc_try_fmt_vid_overlay = vidioc_try_fmt_overlay,
	.vidioc_s_fmt_vid_overlay = vidioc_s_fmt_overlay,
	.vidioc_overlay	      = vidioc_overlay,
	.vidioc_g_fbuf	      = vidioc_g_fbuf,
	.vidioc_s_fbuf	      = vidioc_s_fbuf,
	.vidioc_reqbufs       = vidioc_reqbufs,
	.vidioc_querybuf      = vidioc_querybuf,
	.vidioc_qbuf          = vidioc_qbuf,
	.vidioc_dqbuf         = vidioc_dqbuf,
	.vidioc_g_std         = vidioc_g_std,
	.vidioc_s_std         = vidioc_s_std,
	.vidioc_querystd      = vidioc_querystd,
	.vidioc_enum_input    = vidioc_enum_input,
	.vidioc_g_input       = vidioc_g_input,
	.vidioc_s_input       = vidioc_s_input,
	.vidioc_streamon      = vidioc_streamon,
	.vidioc_streamoff     = vidioc_streamoff,
	.vidioc_log_status    = v4l2_ctrl_log_status,
	.vidioc_subscribe_event = v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event = v4l2_event_unsubscribe,
};

static const struct video_device viu_template = {
	.name		= "FSL viu",
	.fops		= &viu_fops,
	.minor		= -1,
	.ioctl_ops	= &viu_ioctl_ops,
	.release	= video_device_release,

	.tvnorms        = V4L2_STD_NTSC_M | V4L2_STD_PAL,
	.device_caps	= V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING |
			  V4L2_CAP_VIDEO_OVERLAY | V4L2_CAP_READWRITE,
};

static int viu_of_probe(struct platform_device *op)
{
	struct viu_dev *viu_dev;
	struct video_device *vdev;
	struct resource r;
	struct viu_reg __iomem *viu_regs;
	struct i2c_adapter *ad;
	int ret, viu_irq;
	struct clk *clk;

	ret = of_address_to_resource(op->dev.of_node, 0, &r);
	if (ret) {
		dev_err(&op->dev, "Can't parse device node resource\n");
		return -ENODEV;
	}

	viu_irq = irq_of_parse_and_map(op->dev.of_node, 0);
	if (!viu_irq) {
		dev_err(&op->dev, "Error while mapping the irq\n");
		return -EINVAL;
	}

	/* request mem region */
	if (!devm_request_mem_region(&op->dev, r.start,
				     sizeof(struct viu_reg), DRV_NAME)) {
		dev_err(&op->dev, "Error while requesting mem region\n");
		ret = -EBUSY;
		goto err_irq;
	}

	/* remap registers */
	viu_regs = devm_ioremap(&op->dev, r.start, sizeof(struct viu_reg));
	if (!viu_regs) {
		dev_err(&op->dev, "Can't map register set\n");
		ret = -ENOMEM;
		goto err_irq;
	}

	/* Prepare our private structure */
	viu_dev = devm_kzalloc(&op->dev, sizeof(struct viu_dev), GFP_ATOMIC);
	if (!viu_dev) {
		dev_err(&op->dev, "Can't allocate private structure\n");
		ret = -ENOMEM;
		goto err_irq;
	}

	viu_dev->vr = viu_regs;
	viu_dev->irq = viu_irq;
	viu_dev->dev = &op->dev;

	/* init video dma queues */
	INIT_LIST_HEAD(&viu_dev->vidq.active);
	INIT_LIST_HEAD(&viu_dev->vidq.queued);

	snprintf(viu_dev->v4l2_dev.name,
		 sizeof(viu_dev->v4l2_dev.name), "%s", "VIU");
	ret = v4l2_device_register(viu_dev->dev, &viu_dev->v4l2_dev);
	if (ret < 0) {
		dev_err(&op->dev, "v4l2_device_register() failed: %d\n", ret);
		goto err_irq;
	}

	ad = i2c_get_adapter(0);
	if (!ad) {
		ret = -EFAULT;
		dev_err(&op->dev, "couldn't get i2c adapter\n");
		goto err_v4l2;
	}

	v4l2_ctrl_handler_init(&viu_dev->hdl, 5);
	if (viu_dev->hdl.error) {
		ret = viu_dev->hdl.error;
		dev_err(&op->dev, "couldn't register control\n");
		goto err_i2c;
	}
	/* This control handler will inherit the control(s) from the
	   sub-device(s). */
	viu_dev->v4l2_dev.ctrl_handler = &viu_dev->hdl;
	viu_dev->decoder = v4l2_i2c_new_subdev(&viu_dev->v4l2_dev, ad,
			"saa7113", VIU_VIDEO_DECODER_ADDR, NULL);

	timer_setup(&viu_dev->vidq.timeout, viu_vid_timeout, 0);
	viu_dev->std = V4L2_STD_NTSC_M;
	viu_dev->first = 1;

	/* Allocate memory for video device */
	vdev = video_device_alloc();
	if (vdev == NULL) {
		ret = -ENOMEM;
		goto err_hdl;
	}

	*vdev = viu_template;

	vdev->v4l2_dev = &viu_dev->v4l2_dev;

	viu_dev->vdev = vdev;

	/* initialize locks */
	mutex_init(&viu_dev->lock);
	viu_dev->vdev->lock = &viu_dev->lock;
	spin_lock_init(&viu_dev->slock);

	video_set_drvdata(viu_dev->vdev, viu_dev);

	mutex_lock(&viu_dev->lock);

	ret = video_register_device(viu_dev->vdev, VFL_TYPE_VIDEO, -1);
	if (ret < 0) {
		video_device_release(viu_dev->vdev);
		goto err_unlock;
	}

	/* enable VIU clock */
	clk = devm_clk_get(&op->dev, "ipg");
	if (IS_ERR(clk)) {
		dev_err(&op->dev, "failed to lookup the clock!\n");
		ret = PTR_ERR(clk);
		goto err_vdev;
	}
	ret = clk_prepare_enable(clk);
	if (ret) {
		dev_err(&op->dev, "failed to enable the clock!\n");
		goto err_vdev;
	}
	viu_dev->clk = clk;

	/* reset VIU module */
	viu_reset(viu_dev->vr);

	/* install interrupt handler */
	if (request_irq(viu_dev->irq, viu_intr, 0, "viu", (void *)viu_dev)) {
		dev_err(&op->dev, "Request VIU IRQ failed.\n");
		ret = -ENODEV;
		goto err_clk;
	}

	mutex_unlock(&viu_dev->lock);

	dev_info(&op->dev, "Freescale VIU Video Capture Board\n");
	return ret;

err_clk:
	clk_disable_unprepare(viu_dev->clk);
err_vdev:
	video_unregister_device(viu_dev->vdev);
err_unlock:
	mutex_unlock(&viu_dev->lock);
err_hdl:
	v4l2_ctrl_handler_free(&viu_dev->hdl);
err_i2c:
	i2c_put_adapter(ad);
err_v4l2:
	v4l2_device_unregister(&viu_dev->v4l2_dev);
err_irq:
	irq_dispose_mapping(viu_irq);
	return ret;
}

static int viu_of_remove(struct platform_device *op)
{
	struct v4l2_device *v4l2_dev = platform_get_drvdata(op);
	struct viu_dev *dev = container_of(v4l2_dev, struct viu_dev, v4l2_dev);
	struct v4l2_subdev *sdev = list_entry(v4l2_dev->subdevs.next,
					      struct v4l2_subdev, list);
	struct i2c_client *client = v4l2_get_subdevdata(sdev);

	free_irq(dev->irq, (void *)dev);
	irq_dispose_mapping(dev->irq);

	clk_disable_unprepare(dev->clk);

	v4l2_ctrl_handler_free(&dev->hdl);
	video_unregister_device(dev->vdev);
	i2c_put_adapter(client->adapter);
	v4l2_device_unregister(&dev->v4l2_dev);
	return 0;
}

#ifdef CONFIG_PM
static int viu_suspend(struct platform_device *op, pm_message_t state)
{
	struct v4l2_device *v4l2_dev = platform_get_drvdata(op);
	struct viu_dev *dev = container_of(v4l2_dev, struct viu_dev, v4l2_dev);

	clk_disable(dev->clk);
	return 0;
}

static int viu_resume(struct platform_device *op)
{
	struct v4l2_device *v4l2_dev = platform_get_drvdata(op);
	struct viu_dev *dev = container_of(v4l2_dev, struct viu_dev, v4l2_dev);

	clk_enable(dev->clk);
	return 0;
}
#endif

/*
 * Initialization and module stuff
 */
static const struct of_device_id mpc512x_viu_of_match[] = {
	{
		.compatible = "fsl,mpc5121-viu",
	},
	{},
};
MODULE_DEVICE_TABLE(of, mpc512x_viu_of_match);

static struct platform_driver viu_of_platform_driver = {
	.probe = viu_of_probe,
	.remove = viu_of_remove,
#ifdef CONFIG_PM
	.suspend = viu_suspend,
	.resume = viu_resume,
#endif
	.driver = {
		.name = DRV_NAME,
		.of_match_table = mpc512x_viu_of_match,
	},
};

module_platform_driver(viu_of_platform_driver);

MODULE_DESCRIPTION("Freescale Video-In(VIU)");
MODULE_AUTHOR("Hongjun Chen");
MODULE_LICENSE("GPL");
MODULE_VERSION(VIU_VERSION);
