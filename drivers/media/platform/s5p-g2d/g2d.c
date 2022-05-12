// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Samsung S5P G2D - 2D Graphics Accelerator Driver
 *
 * Copyright (c) 2011 Samsung Electronics Co., Ltd.
 * Kamil Debski, <k.debski@samsung.com>
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/of.h>

#include <linux/platform_device.h>
#include <media/v4l2-mem2mem.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-v4l2.h>
#include <media/videobuf2-dma-contig.h>

#include "g2d.h"
#include "g2d-regs.h"

#define fh2ctx(__fh) container_of(__fh, struct g2d_ctx, fh)

static struct g2d_fmt formats[] = {
	{
		.fourcc	= V4L2_PIX_FMT_RGB32,
		.depth	= 32,
		.hw	= COLOR_MODE(ORDER_XRGB, MODE_XRGB_8888),
	},
	{
		.fourcc	= V4L2_PIX_FMT_RGB565X,
		.depth	= 16,
		.hw	= COLOR_MODE(ORDER_XRGB, MODE_RGB_565),
	},
	{
		.fourcc	= V4L2_PIX_FMT_RGB555X,
		.depth	= 16,
		.hw	= COLOR_MODE(ORDER_XRGB, MODE_XRGB_1555),
	},
	{
		.fourcc	= V4L2_PIX_FMT_RGB444,
		.depth	= 16,
		.hw	= COLOR_MODE(ORDER_XRGB, MODE_XRGB_4444),
	},
	{
		.fourcc	= V4L2_PIX_FMT_RGB24,
		.depth	= 24,
		.hw	= COLOR_MODE(ORDER_XRGB, MODE_PACKED_RGB_888),
	},
};
#define NUM_FORMATS ARRAY_SIZE(formats)

static struct g2d_frame def_frame = {
	.width		= DEFAULT_WIDTH,
	.height		= DEFAULT_HEIGHT,
	.c_width	= DEFAULT_WIDTH,
	.c_height	= DEFAULT_HEIGHT,
	.o_width	= 0,
	.o_height	= 0,
	.fmt		= &formats[0],
	.right		= DEFAULT_WIDTH,
	.bottom		= DEFAULT_HEIGHT,
};

static struct g2d_fmt *find_fmt(struct v4l2_format *f)
{
	unsigned int i;
	for (i = 0; i < NUM_FORMATS; i++) {
		if (formats[i].fourcc == f->fmt.pix.pixelformat)
			return &formats[i];
	}
	return NULL;
}


static struct g2d_frame *get_frame(struct g2d_ctx *ctx,
				   enum v4l2_buf_type type)
{
	switch (type) {
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
		return &ctx->in;
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		return &ctx->out;
	default:
		return ERR_PTR(-EINVAL);
	}
}

static int g2d_queue_setup(struct vb2_queue *vq,
			   unsigned int *nbuffers, unsigned int *nplanes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct g2d_ctx *ctx = vb2_get_drv_priv(vq);
	struct g2d_frame *f = get_frame(ctx, vq->type);

	if (IS_ERR(f))
		return PTR_ERR(f);

	sizes[0] = f->size;
	*nplanes = 1;

	if (*nbuffers == 0)
		*nbuffers = 1;

	return 0;
}

static int g2d_buf_prepare(struct vb2_buffer *vb)
{
	struct g2d_ctx *ctx = vb2_get_drv_priv(vb->vb2_queue);
	struct g2d_frame *f = get_frame(ctx, vb->vb2_queue->type);

	if (IS_ERR(f))
		return PTR_ERR(f);
	vb2_set_plane_payload(vb, 0, f->size);
	return 0;
}

static void g2d_buf_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct g2d_ctx *ctx = vb2_get_drv_priv(vb->vb2_queue);
	v4l2_m2m_buf_queue(ctx->fh.m2m_ctx, vbuf);
}

static const struct vb2_ops g2d_qops = {
	.queue_setup	= g2d_queue_setup,
	.buf_prepare	= g2d_buf_prepare,
	.buf_queue	= g2d_buf_queue,
	.wait_prepare	= vb2_ops_wait_prepare,
	.wait_finish	= vb2_ops_wait_finish,
};

static int queue_init(void *priv, struct vb2_queue *src_vq,
						struct vb2_queue *dst_vq)
{
	struct g2d_ctx *ctx = priv;
	int ret;

	src_vq->type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	src_vq->io_modes = VB2_MMAP | VB2_USERPTR;
	src_vq->drv_priv = ctx;
	src_vq->ops = &g2d_qops;
	src_vq->mem_ops = &vb2_dma_contig_memops;
	src_vq->buf_struct_size = sizeof(struct v4l2_m2m_buffer);
	src_vq->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_COPY;
	src_vq->lock = &ctx->dev->mutex;
	src_vq->dev = ctx->dev->v4l2_dev.dev;

	ret = vb2_queue_init(src_vq);
	if (ret)
		return ret;

	dst_vq->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	dst_vq->io_modes = VB2_MMAP | VB2_USERPTR;
	dst_vq->drv_priv = ctx;
	dst_vq->ops = &g2d_qops;
	dst_vq->mem_ops = &vb2_dma_contig_memops;
	dst_vq->buf_struct_size = sizeof(struct v4l2_m2m_buffer);
	dst_vq->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_COPY;
	dst_vq->lock = &ctx->dev->mutex;
	dst_vq->dev = ctx->dev->v4l2_dev.dev;

	return vb2_queue_init(dst_vq);
}

static int g2d_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct g2d_ctx *ctx = container_of(ctrl->handler, struct g2d_ctx,
								ctrl_handler);
	unsigned long flags;

	spin_lock_irqsave(&ctx->dev->ctrl_lock, flags);
	switch (ctrl->id) {
	case V4L2_CID_COLORFX:
		if (ctrl->val == V4L2_COLORFX_NEGATIVE)
			ctx->rop = ROP4_INVERT;
		else
			ctx->rop = ROP4_COPY;
		break;

	case V4L2_CID_HFLIP:
		ctx->flip = ctx->ctrl_hflip->val | (ctx->ctrl_vflip->val << 1);
		break;

	}
	spin_unlock_irqrestore(&ctx->dev->ctrl_lock, flags);
	return 0;
}

static const struct v4l2_ctrl_ops g2d_ctrl_ops = {
	.s_ctrl		= g2d_s_ctrl,
};

static int g2d_setup_ctrls(struct g2d_ctx *ctx)
{
	struct g2d_dev *dev = ctx->dev;

	v4l2_ctrl_handler_init(&ctx->ctrl_handler, 3);

	ctx->ctrl_hflip = v4l2_ctrl_new_std(&ctx->ctrl_handler, &g2d_ctrl_ops,
						V4L2_CID_HFLIP, 0, 1, 1, 0);

	ctx->ctrl_vflip = v4l2_ctrl_new_std(&ctx->ctrl_handler, &g2d_ctrl_ops,
						V4L2_CID_VFLIP, 0, 1, 1, 0);

	v4l2_ctrl_new_std_menu(
		&ctx->ctrl_handler,
		&g2d_ctrl_ops,
		V4L2_CID_COLORFX,
		V4L2_COLORFX_NEGATIVE,
		~((1 << V4L2_COLORFX_NONE) | (1 << V4L2_COLORFX_NEGATIVE)),
		V4L2_COLORFX_NONE);

	if (ctx->ctrl_handler.error) {
		int err = ctx->ctrl_handler.error;
		v4l2_err(&dev->v4l2_dev, "g2d_setup_ctrls failed\n");
		v4l2_ctrl_handler_free(&ctx->ctrl_handler);
		return err;
	}

	v4l2_ctrl_cluster(2, &ctx->ctrl_hflip);

	return 0;
}

static int g2d_open(struct file *file)
{
	struct g2d_dev *dev = video_drvdata(file);
	struct g2d_ctx *ctx = NULL;
	int ret = 0;

	ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;
	ctx->dev = dev;
	/* Set default formats */
	ctx->in		= def_frame;
	ctx->out	= def_frame;

	if (mutex_lock_interruptible(&dev->mutex)) {
		kfree(ctx);
		return -ERESTARTSYS;
	}
	ctx->fh.m2m_ctx = v4l2_m2m_ctx_init(dev->m2m_dev, ctx, &queue_init);
	if (IS_ERR(ctx->fh.m2m_ctx)) {
		ret = PTR_ERR(ctx->fh.m2m_ctx);
		mutex_unlock(&dev->mutex);
		kfree(ctx);
		return ret;
	}
	v4l2_fh_init(&ctx->fh, video_devdata(file));
	file->private_data = &ctx->fh;
	v4l2_fh_add(&ctx->fh);

	g2d_setup_ctrls(ctx);

	/* Write the default values to the ctx struct */
	v4l2_ctrl_handler_setup(&ctx->ctrl_handler);

	ctx->fh.ctrl_handler = &ctx->ctrl_handler;
	mutex_unlock(&dev->mutex);

	v4l2_info(&dev->v4l2_dev, "instance opened\n");
	return 0;
}

static int g2d_release(struct file *file)
{
	struct g2d_dev *dev = video_drvdata(file);
	struct g2d_ctx *ctx = fh2ctx(file->private_data);

	v4l2_ctrl_handler_free(&ctx->ctrl_handler);
	v4l2_fh_del(&ctx->fh);
	v4l2_fh_exit(&ctx->fh);
	kfree(ctx);
	v4l2_info(&dev->v4l2_dev, "instance closed\n");
	return 0;
}


static int vidioc_querycap(struct file *file, void *priv,
				struct v4l2_capability *cap)
{
	strscpy(cap->driver, G2D_NAME, sizeof(cap->driver));
	strscpy(cap->card, G2D_NAME, sizeof(cap->card));
	cap->bus_info[0] = 0;
	return 0;
}

static int vidioc_enum_fmt(struct file *file, void *prv, struct v4l2_fmtdesc *f)
{
	if (f->index >= NUM_FORMATS)
		return -EINVAL;
	f->pixelformat = formats[f->index].fourcc;
	return 0;
}

static int vidioc_g_fmt(struct file *file, void *prv, struct v4l2_format *f)
{
	struct g2d_ctx *ctx = prv;
	struct vb2_queue *vq;
	struct g2d_frame *frm;

	vq = v4l2_m2m_get_vq(ctx->fh.m2m_ctx, f->type);
	if (!vq)
		return -EINVAL;
	frm = get_frame(ctx, f->type);
	if (IS_ERR(frm))
		return PTR_ERR(frm);

	f->fmt.pix.width		= frm->width;
	f->fmt.pix.height		= frm->height;
	f->fmt.pix.field		= V4L2_FIELD_NONE;
	f->fmt.pix.pixelformat		= frm->fmt->fourcc;
	f->fmt.pix.bytesperline		= (frm->width * frm->fmt->depth) >> 3;
	f->fmt.pix.sizeimage		= frm->size;
	return 0;
}

static int vidioc_try_fmt(struct file *file, void *prv, struct v4l2_format *f)
{
	struct g2d_fmt *fmt;
	enum v4l2_field *field;

	fmt = find_fmt(f);
	if (!fmt)
		return -EINVAL;

	field = &f->fmt.pix.field;
	if (*field == V4L2_FIELD_ANY)
		*field = V4L2_FIELD_NONE;
	else if (*field != V4L2_FIELD_NONE)
		return -EINVAL;

	if (f->fmt.pix.width > MAX_WIDTH)
		f->fmt.pix.width = MAX_WIDTH;
	if (f->fmt.pix.height > MAX_HEIGHT)
		f->fmt.pix.height = MAX_HEIGHT;

	if (f->fmt.pix.width < 1)
		f->fmt.pix.width = 1;
	if (f->fmt.pix.height < 1)
		f->fmt.pix.height = 1;

	f->fmt.pix.bytesperline = (f->fmt.pix.width * fmt->depth) >> 3;
	f->fmt.pix.sizeimage = f->fmt.pix.height * f->fmt.pix.bytesperline;
	return 0;
}

static int vidioc_s_fmt(struct file *file, void *prv, struct v4l2_format *f)
{
	struct g2d_ctx *ctx = prv;
	struct g2d_dev *dev = ctx->dev;
	struct vb2_queue *vq;
	struct g2d_frame *frm;
	struct g2d_fmt *fmt;
	int ret = 0;

	/* Adjust all values accordingly to the hardware capabilities
	 * and chosen format. */
	ret = vidioc_try_fmt(file, prv, f);
	if (ret)
		return ret;
	vq = v4l2_m2m_get_vq(ctx->fh.m2m_ctx, f->type);
	if (vb2_is_busy(vq)) {
		v4l2_err(&dev->v4l2_dev, "queue (%d) bust\n", f->type);
		return -EBUSY;
	}
	frm = get_frame(ctx, f->type);
	if (IS_ERR(frm))
		return PTR_ERR(frm);
	fmt = find_fmt(f);
	if (!fmt)
		return -EINVAL;
	frm->width	= f->fmt.pix.width;
	frm->height	= f->fmt.pix.height;
	frm->size	= f->fmt.pix.sizeimage;
	/* Reset crop settings */
	frm->o_width	= 0;
	frm->o_height	= 0;
	frm->c_width	= frm->width;
	frm->c_height	= frm->height;
	frm->right	= frm->width;
	frm->bottom	= frm->height;
	frm->fmt	= fmt;
	frm->stride	= f->fmt.pix.bytesperline;
	return 0;
}

static int vidioc_g_selection(struct file *file, void *prv,
			      struct v4l2_selection *s)
{
	struct g2d_ctx *ctx = prv;
	struct g2d_frame *f;

	f = get_frame(ctx, s->type);
	if (IS_ERR(f))
		return PTR_ERR(f);

	switch (s->target) {
	case V4L2_SEL_TGT_CROP:
	case V4L2_SEL_TGT_CROP_DEFAULT:
	case V4L2_SEL_TGT_CROP_BOUNDS:
		if (s->type != V4L2_BUF_TYPE_VIDEO_OUTPUT)
			return -EINVAL;
		break;
	case V4L2_SEL_TGT_COMPOSE:
	case V4L2_SEL_TGT_COMPOSE_DEFAULT:
	case V4L2_SEL_TGT_COMPOSE_BOUNDS:
		if (s->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
			return -EINVAL;
		break;
	default:
		return -EINVAL;
	}

	switch (s->target) {
	case V4L2_SEL_TGT_CROP:
	case V4L2_SEL_TGT_COMPOSE:
		s->r.left = f->o_height;
		s->r.top = f->o_width;
		s->r.width = f->c_width;
		s->r.height = f->c_height;
		break;
	case V4L2_SEL_TGT_CROP_DEFAULT:
	case V4L2_SEL_TGT_CROP_BOUNDS:
	case V4L2_SEL_TGT_COMPOSE_DEFAULT:
	case V4L2_SEL_TGT_COMPOSE_BOUNDS:
		s->r.left = 0;
		s->r.top = 0;
		s->r.width = f->width;
		s->r.height = f->height;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int vidioc_try_selection(struct file *file, void *prv,
				const struct v4l2_selection *s)
{
	struct g2d_ctx *ctx = prv;
	struct g2d_dev *dev = ctx->dev;
	struct g2d_frame *f;

	f = get_frame(ctx, s->type);
	if (IS_ERR(f))
		return PTR_ERR(f);

	if (s->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
		if (s->target != V4L2_SEL_TGT_COMPOSE)
			return -EINVAL;
	} else if (s->type == V4L2_BUF_TYPE_VIDEO_OUTPUT) {
		if (s->target != V4L2_SEL_TGT_CROP)
			return -EINVAL;
	}

	if (s->r.top < 0 || s->r.left < 0) {
		v4l2_err(&dev->v4l2_dev,
			"doesn't support negative values for top & left\n");
		return -EINVAL;
	}

	return 0;
}

static int vidioc_s_selection(struct file *file, void *prv,
			      struct v4l2_selection *s)
{
	struct g2d_ctx *ctx = prv;
	struct g2d_frame *f;
	int ret;

	ret = vidioc_try_selection(file, prv, s);
	if (ret)
		return ret;
	f = get_frame(ctx, s->type);
	if (IS_ERR(f))
		return PTR_ERR(f);

	f->c_width	= s->r.width;
	f->c_height	= s->r.height;
	f->o_width	= s->r.left;
	f->o_height	= s->r.top;
	f->bottom	= f->o_height + f->c_height;
	f->right	= f->o_width + f->c_width;
	return 0;
}

static void device_run(void *prv)
{
	struct g2d_ctx *ctx = prv;
	struct g2d_dev *dev = ctx->dev;
	struct vb2_v4l2_buffer *src, *dst;
	unsigned long flags;
	u32 cmd = 0;

	dev->curr = ctx;

	src = v4l2_m2m_next_src_buf(ctx->fh.m2m_ctx);
	dst = v4l2_m2m_next_dst_buf(ctx->fh.m2m_ctx);

	clk_enable(dev->gate);
	g2d_reset(dev);

	spin_lock_irqsave(&dev->ctrl_lock, flags);

	g2d_set_src_size(dev, &ctx->in);
	g2d_set_src_addr(dev, vb2_dma_contig_plane_dma_addr(&src->vb2_buf, 0));

	g2d_set_dst_size(dev, &ctx->out);
	g2d_set_dst_addr(dev, vb2_dma_contig_plane_dma_addr(&dst->vb2_buf, 0));

	g2d_set_rop4(dev, ctx->rop);
	g2d_set_flip(dev, ctx->flip);

	if (ctx->in.c_width != ctx->out.c_width ||
		ctx->in.c_height != ctx->out.c_height) {
		if (dev->variant->hw_rev == TYPE_G2D_3X)
			cmd |= CMD_V3_ENABLE_STRETCH;
		else
			g2d_set_v41_stretch(dev, &ctx->in, &ctx->out);
	}

	g2d_set_cmd(dev, cmd);
	g2d_start(dev);

	spin_unlock_irqrestore(&dev->ctrl_lock, flags);
}

static irqreturn_t g2d_isr(int irq, void *prv)
{
	struct g2d_dev *dev = prv;
	struct g2d_ctx *ctx = dev->curr;
	struct vb2_v4l2_buffer *src, *dst;

	g2d_clear_int(dev);
	clk_disable(dev->gate);

	BUG_ON(ctx == NULL);

	src = v4l2_m2m_src_buf_remove(ctx->fh.m2m_ctx);
	dst = v4l2_m2m_dst_buf_remove(ctx->fh.m2m_ctx);

	BUG_ON(src == NULL);
	BUG_ON(dst == NULL);

	dst->timecode = src->timecode;
	dst->vb2_buf.timestamp = src->vb2_buf.timestamp;
	dst->flags &= ~V4L2_BUF_FLAG_TSTAMP_SRC_MASK;
	dst->flags |=
		src->flags & V4L2_BUF_FLAG_TSTAMP_SRC_MASK;

	v4l2_m2m_buf_done(src, VB2_BUF_STATE_DONE);
	v4l2_m2m_buf_done(dst, VB2_BUF_STATE_DONE);
	v4l2_m2m_job_finish(dev->m2m_dev, ctx->fh.m2m_ctx);

	dev->curr = NULL;
	return IRQ_HANDLED;
}

static const struct v4l2_file_operations g2d_fops = {
	.owner		= THIS_MODULE,
	.open		= g2d_open,
	.release	= g2d_release,
	.poll		= v4l2_m2m_fop_poll,
	.unlocked_ioctl	= video_ioctl2,
	.mmap		= v4l2_m2m_fop_mmap,
};

static const struct v4l2_ioctl_ops g2d_ioctl_ops = {
	.vidioc_querycap	= vidioc_querycap,

	.vidioc_enum_fmt_vid_cap	= vidioc_enum_fmt,
	.vidioc_g_fmt_vid_cap		= vidioc_g_fmt,
	.vidioc_try_fmt_vid_cap		= vidioc_try_fmt,
	.vidioc_s_fmt_vid_cap		= vidioc_s_fmt,

	.vidioc_enum_fmt_vid_out	= vidioc_enum_fmt,
	.vidioc_g_fmt_vid_out		= vidioc_g_fmt,
	.vidioc_try_fmt_vid_out		= vidioc_try_fmt,
	.vidioc_s_fmt_vid_out		= vidioc_s_fmt,

	.vidioc_reqbufs			= v4l2_m2m_ioctl_reqbufs,
	.vidioc_querybuf		= v4l2_m2m_ioctl_querybuf,
	.vidioc_qbuf			= v4l2_m2m_ioctl_qbuf,
	.vidioc_dqbuf			= v4l2_m2m_ioctl_dqbuf,

	.vidioc_streamon		= v4l2_m2m_ioctl_streamon,
	.vidioc_streamoff		= v4l2_m2m_ioctl_streamoff,

	.vidioc_g_selection		= vidioc_g_selection,
	.vidioc_s_selection		= vidioc_s_selection,
};

static const struct video_device g2d_videodev = {
	.name		= G2D_NAME,
	.fops		= &g2d_fops,
	.ioctl_ops	= &g2d_ioctl_ops,
	.minor		= -1,
	.release	= video_device_release,
	.vfl_dir	= VFL_DIR_M2M,
};

static const struct v4l2_m2m_ops g2d_m2m_ops = {
	.device_run	= device_run,
};

static const struct of_device_id exynos_g2d_match[];

static int g2d_probe(struct platform_device *pdev)
{
	struct g2d_dev *dev;
	struct video_device *vfd;
	struct resource *res;
	const struct of_device_id *of_id;
	int ret = 0;

	dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	spin_lock_init(&dev->ctrl_lock);
	mutex_init(&dev->mutex);
	atomic_set(&dev->num_inst, 0);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	dev->regs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(dev->regs))
		return PTR_ERR(dev->regs);

	dev->clk = clk_get(&pdev->dev, "sclk_fimg2d");
	if (IS_ERR(dev->clk)) {
		dev_err(&pdev->dev, "failed to get g2d clock\n");
		return -ENXIO;
	}

	ret = clk_prepare(dev->clk);
	if (ret) {
		dev_err(&pdev->dev, "failed to prepare g2d clock\n");
		goto put_clk;
	}

	dev->gate = clk_get(&pdev->dev, "fimg2d");
	if (IS_ERR(dev->gate)) {
		dev_err(&pdev->dev, "failed to get g2d clock gate\n");
		ret = -ENXIO;
		goto unprep_clk;
	}

	ret = clk_prepare(dev->gate);
	if (ret) {
		dev_err(&pdev->dev, "failed to prepare g2d clock gate\n");
		goto put_clk_gate;
	}

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		dev_err(&pdev->dev, "failed to find IRQ\n");
		ret = -ENXIO;
		goto unprep_clk_gate;
	}

	dev->irq = res->start;

	ret = devm_request_irq(&pdev->dev, dev->irq, g2d_isr,
						0, pdev->name, dev);
	if (ret) {
		dev_err(&pdev->dev, "failed to install IRQ\n");
		goto unprep_clk_gate;
	}

	vb2_dma_contig_set_max_seg_size(&pdev->dev, DMA_BIT_MASK(32));

	ret = v4l2_device_register(&pdev->dev, &dev->v4l2_dev);
	if (ret)
		goto unprep_clk_gate;
	vfd = video_device_alloc();
	if (!vfd) {
		v4l2_err(&dev->v4l2_dev, "Failed to allocate video device\n");
		ret = -ENOMEM;
		goto unreg_v4l2_dev;
	}
	*vfd = g2d_videodev;
	set_bit(V4L2_FL_QUIRK_INVERTED_CROP, &vfd->flags);
	vfd->lock = &dev->mutex;
	vfd->v4l2_dev = &dev->v4l2_dev;
	vfd->device_caps = V4L2_CAP_VIDEO_M2M | V4L2_CAP_STREAMING;

	platform_set_drvdata(pdev, dev);
	dev->m2m_dev = v4l2_m2m_init(&g2d_m2m_ops);
	if (IS_ERR(dev->m2m_dev)) {
		v4l2_err(&dev->v4l2_dev, "Failed to init mem2mem device\n");
		ret = PTR_ERR(dev->m2m_dev);
		goto rel_vdev;
	}

	def_frame.stride = (def_frame.width * def_frame.fmt->depth) >> 3;

	of_id = of_match_node(exynos_g2d_match, pdev->dev.of_node);
	if (!of_id) {
		ret = -ENODEV;
		goto free_m2m;
	}
	dev->variant = (struct g2d_variant *)of_id->data;

	ret = video_register_device(vfd, VFL_TYPE_VIDEO, 0);
	if (ret) {
		v4l2_err(&dev->v4l2_dev, "Failed to register video device\n");
		goto free_m2m;
	}
	video_set_drvdata(vfd, dev);
	dev->vfd = vfd;
	v4l2_info(&dev->v4l2_dev, "device registered as /dev/video%d\n",
		  vfd->num);

	return 0;

free_m2m:
	v4l2_m2m_release(dev->m2m_dev);
rel_vdev:
	video_device_release(vfd);
unreg_v4l2_dev:
	v4l2_device_unregister(&dev->v4l2_dev);
unprep_clk_gate:
	clk_unprepare(dev->gate);
put_clk_gate:
	clk_put(dev->gate);
unprep_clk:
	clk_unprepare(dev->clk);
put_clk:
	clk_put(dev->clk);

	return ret;
}

static int g2d_remove(struct platform_device *pdev)
{
	struct g2d_dev *dev = platform_get_drvdata(pdev);

	v4l2_info(&dev->v4l2_dev, "Removing " G2D_NAME);
	v4l2_m2m_release(dev->m2m_dev);
	video_unregister_device(dev->vfd);
	v4l2_device_unregister(&dev->v4l2_dev);
	vb2_dma_contig_clear_max_seg_size(&pdev->dev);
	clk_unprepare(dev->gate);
	clk_put(dev->gate);
	clk_unprepare(dev->clk);
	clk_put(dev->clk);
	return 0;
}

static struct g2d_variant g2d_drvdata_v3x = {
	.hw_rev = TYPE_G2D_3X, /* Revision 3.0 for S5PV210 and Exynos4210 */
};

static struct g2d_variant g2d_drvdata_v4x = {
	.hw_rev = TYPE_G2D_4X, /* Revision 4.1 for Exynos4X12 and Exynos5 */
};

static const struct of_device_id exynos_g2d_match[] = {
	{
		.compatible = "samsung,s5pv210-g2d",
		.data = &g2d_drvdata_v3x,
	}, {
		.compatible = "samsung,exynos4212-g2d",
		.data = &g2d_drvdata_v4x,
	},
	{},
};
MODULE_DEVICE_TABLE(of, exynos_g2d_match);

static struct platform_driver g2d_pdrv = {
	.probe		= g2d_probe,
	.remove		= g2d_remove,
	.driver		= {
		.name = G2D_NAME,
		.of_match_table = exynos_g2d_match,
	},
};

module_platform_driver(g2d_pdrv);

MODULE_AUTHOR("Kamil Debski <k.debski@samsung.com>");
MODULE_DESCRIPTION("S5P G2D 2d graphics accelerator driver");
MODULE_LICENSE("GPL");
