// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2017 Fuzhou Rockchip Electronics Co.Ltd
 * Author: Jacob Chen <jacob-chen@iotwrt.com>
 */

#include <linux/pm_runtime.h>

#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-mem2mem.h>
#include <media/videobuf2-dma-sg.h>
#include <media/videobuf2-v4l2.h>

#include "rga-hw.h"
#include "rga.h"

static int
rga_queue_setup(struct vb2_queue *vq,
		unsigned int *nbuffers, unsigned int *nplanes,
		unsigned int sizes[], struct device *alloc_devs[])
{
	struct rga_ctx *ctx = vb2_get_drv_priv(vq);
	struct rga_frame *f = rga_get_frame(ctx, vq->type);

	if (IS_ERR(f))
		return PTR_ERR(f);

	if (*nplanes)
		return sizes[0] < f->size ? -EINVAL : 0;

	sizes[0] = f->size;
	*nplanes = 1;

	return 0;
}

static int rga_buf_prepare(struct vb2_buffer *vb)
{
	struct rga_ctx *ctx = vb2_get_drv_priv(vb->vb2_queue);
	struct rga_frame *f = rga_get_frame(ctx, vb->vb2_queue->type);

	if (IS_ERR(f))
		return PTR_ERR(f);

	vb2_set_plane_payload(vb, 0, f->size);

	return 0;
}

static void rga_buf_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct rga_ctx *ctx = vb2_get_drv_priv(vb->vb2_queue);

	v4l2_m2m_buf_queue(ctx->fh.m2m_ctx, vbuf);
}

static void rga_buf_return_buffers(struct vb2_queue *q,
				   enum vb2_buffer_state state)
{
	struct rga_ctx *ctx = vb2_get_drv_priv(q);
	struct vb2_v4l2_buffer *vbuf;

	for (;;) {
		if (V4L2_TYPE_IS_OUTPUT(q->type))
			vbuf = v4l2_m2m_src_buf_remove(ctx->fh.m2m_ctx);
		else
			vbuf = v4l2_m2m_dst_buf_remove(ctx->fh.m2m_ctx);
		if (!vbuf)
			break;
		v4l2_m2m_buf_done(vbuf, state);
	}
}

static int rga_buf_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct rga_ctx *ctx = vb2_get_drv_priv(q);
	struct rockchip_rga *rga = ctx->rga;
	int ret;

	ret = pm_runtime_get_sync(rga->dev);
	if (ret < 0) {
		pm_runtime_put_noidle(rga->dev);
		rga_buf_return_buffers(q, VB2_BUF_STATE_QUEUED);
		return ret;
	}

	return 0;
}

static void rga_buf_stop_streaming(struct vb2_queue *q)
{
	struct rga_ctx *ctx = vb2_get_drv_priv(q);
	struct rockchip_rga *rga = ctx->rga;

	rga_buf_return_buffers(q, VB2_BUF_STATE_ERROR);
	pm_runtime_put(rga->dev);
}

const struct vb2_ops rga_qops = {
	.queue_setup = rga_queue_setup,
	.buf_prepare = rga_buf_prepare,
	.buf_queue = rga_buf_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = rga_buf_start_streaming,
	.stop_streaming = rga_buf_stop_streaming,
};

/* RGA MMU is a 1-Level MMU, so it can't be used through the IOMMU API.
 * We use it more like a scatter-gather list.
 */
void rga_buf_map(struct vb2_buffer *vb)
{
	struct rga_ctx *ctx = vb2_get_drv_priv(vb->vb2_queue);
	struct rockchip_rga *rga = ctx->rga;
	struct sg_table *sgt;
	struct scatterlist *sgl;
	unsigned int *pages;
	unsigned int address, len, i, p;
	unsigned int mapped_size = 0;

	if (vb->type == V4L2_BUF_TYPE_VIDEO_OUTPUT)
		pages = rga->src_mmu_pages;
	else
		pages = rga->dst_mmu_pages;

	/* Create local MMU table for RGA */
	sgt = vb2_plane_cookie(vb, 0);

	for_each_sg(sgt->sgl, sgl, sgt->nents, i) {
		len = sg_dma_len(sgl) >> PAGE_SHIFT;
		address = sg_phys(sgl);

		for (p = 0; p < len; p++) {
			dma_addr_t phys = address +
					  ((dma_addr_t)p << PAGE_SHIFT);

			pages[mapped_size + p] = phys;
		}

		mapped_size += len;
	}

	/* sync local MMU table for RGA */
	dma_sync_single_for_device(rga->dev, virt_to_phys(pages),
				   8 * PAGE_SIZE, DMA_BIDIRECTIONAL);
}
