// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Coda multi-standard codec IP - BIT processor functions
 *
 * Copyright (C) 2012 Vista Silicon S.L.
 *    Javier Martin, <javier.martin@vista-silicon.com>
 *    Xavier Duret
 * Copyright (C) 2012-2014 Philipp Zabel, Pengutronix
 */

#include <linux/clk.h>
#include <linux/irqreturn.h>
#include <linux/kernel.h>
#include <linux/log2.h>
#include <linux/platform_device.h>
#include <linux/ratelimit.h>
#include <linux/reset.h>
#include <linux/slab.h>
#include <linux/videodev2.h>

#include <media/v4l2-common.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-mem2mem.h>
#include <media/videobuf2-v4l2.h>
#include <media/videobuf2-dma-contig.h>
#include <media/videobuf2-vmalloc.h>

#include "coda.h"
#include "imx-vdoa.h"
#define CREATE_TRACE_POINTS
#include "trace.h"

#define CODA_PARA_BUF_SIZE	(10 * 1024)
#define CODA7_PS_BUF_SIZE	0x28000
#define CODA9_PS_SAVE_SIZE	(512 * 1024)

#define CODA_DEFAULT_GAMMA	4096
#define CODA9_DEFAULT_GAMMA	24576	/* 0.75 * 32768 */

static void coda_free_bitstream_buffer(struct coda_ctx *ctx);

static inline int coda_is_initialized(struct coda_dev *dev)
{
	return coda_read(dev, CODA_REG_BIT_CUR_PC) != 0;
}

static inline unsigned long coda_isbusy(struct coda_dev *dev)
{
	return coda_read(dev, CODA_REG_BIT_BUSY);
}

static int coda_wait_timeout(struct coda_dev *dev)
{
	unsigned long timeout = jiffies + msecs_to_jiffies(1000);

	while (coda_isbusy(dev)) {
		if (time_after(jiffies, timeout))
			return -ETIMEDOUT;
	}
	return 0;
}

static void coda_command_async(struct coda_ctx *ctx, int cmd)
{
	struct coda_dev *dev = ctx->dev;

	if (dev->devtype->product == CODA_HX4 ||
	    dev->devtype->product == CODA_7541 ||
	    dev->devtype->product == CODA_960) {
		/* Restore context related registers to CODA */
		coda_write(dev, ctx->bit_stream_param,
				CODA_REG_BIT_BIT_STREAM_PARAM);
		coda_write(dev, ctx->frm_dis_flg,
				CODA_REG_BIT_FRM_DIS_FLG(ctx->reg_idx));
		coda_write(dev, ctx->frame_mem_ctrl,
				CODA_REG_BIT_FRAME_MEM_CTRL);
		coda_write(dev, ctx->workbuf.paddr, CODA_REG_BIT_WORK_BUF_ADDR);
	}

	if (dev->devtype->product == CODA_960) {
		coda_write(dev, 1, CODA9_GDI_WPROT_ERR_CLR);
		coda_write(dev, 0, CODA9_GDI_WPROT_RGN_EN);
	}

	coda_write(dev, CODA_REG_BIT_BUSY_FLAG, CODA_REG_BIT_BUSY);

	coda_write(dev, ctx->idx, CODA_REG_BIT_RUN_INDEX);
	coda_write(dev, ctx->params.codec_mode, CODA_REG_BIT_RUN_COD_STD);
	coda_write(dev, ctx->params.codec_mode_aux, CODA7_REG_BIT_RUN_AUX_STD);

	trace_coda_bit_run(ctx, cmd);

	coda_write(dev, cmd, CODA_REG_BIT_RUN_COMMAND);
}

static int coda_command_sync(struct coda_ctx *ctx, int cmd)
{
	struct coda_dev *dev = ctx->dev;
	int ret;

	lockdep_assert_held(&dev->coda_mutex);

	coda_command_async(ctx, cmd);
	ret = coda_wait_timeout(dev);
	trace_coda_bit_done(ctx);

	return ret;
}

int coda_hw_reset(struct coda_ctx *ctx)
{
	struct coda_dev *dev = ctx->dev;
	unsigned long timeout;
	unsigned int idx;
	int ret;

	lockdep_assert_held(&dev->coda_mutex);

	if (!dev->rstc)
		return -ENOENT;

	idx = coda_read(dev, CODA_REG_BIT_RUN_INDEX);

	if (dev->devtype->product == CODA_960) {
		timeout = jiffies + msecs_to_jiffies(100);
		coda_write(dev, 0x11, CODA9_GDI_BUS_CTRL);
		while (coda_read(dev, CODA9_GDI_BUS_STATUS) != 0x77) {
			if (time_after(jiffies, timeout))
				return -ETIME;
			cpu_relax();
		}
	}

	ret = reset_control_reset(dev->rstc);
	if (ret < 0)
		return ret;

	if (dev->devtype->product == CODA_960)
		coda_write(dev, 0x00, CODA9_GDI_BUS_CTRL);
	coda_write(dev, CODA_REG_BIT_BUSY_FLAG, CODA_REG_BIT_BUSY);
	coda_write(dev, CODA_REG_RUN_ENABLE, CODA_REG_BIT_CODE_RUN);
	ret = coda_wait_timeout(dev);
	coda_write(dev, idx, CODA_REG_BIT_RUN_INDEX);

	return ret;
}

static void coda_kfifo_sync_from_device(struct coda_ctx *ctx)
{
	struct __kfifo *kfifo = &ctx->bitstream_fifo.kfifo;
	struct coda_dev *dev = ctx->dev;
	u32 rd_ptr;

	rd_ptr = coda_read(dev, CODA_REG_BIT_RD_PTR(ctx->reg_idx));
	kfifo->out = (kfifo->in & ~kfifo->mask) |
		      (rd_ptr - ctx->bitstream.paddr);
	if (kfifo->out > kfifo->in)
		kfifo->out -= kfifo->mask + 1;
}

static void coda_kfifo_sync_to_device_full(struct coda_ctx *ctx)
{
	struct __kfifo *kfifo = &ctx->bitstream_fifo.kfifo;
	struct coda_dev *dev = ctx->dev;
	u32 rd_ptr, wr_ptr;

	rd_ptr = ctx->bitstream.paddr + (kfifo->out & kfifo->mask);
	coda_write(dev, rd_ptr, CODA_REG_BIT_RD_PTR(ctx->reg_idx));
	wr_ptr = ctx->bitstream.paddr + (kfifo->in & kfifo->mask);
	coda_write(dev, wr_ptr, CODA_REG_BIT_WR_PTR(ctx->reg_idx));
}

static void coda_kfifo_sync_to_device_write(struct coda_ctx *ctx)
{
	struct __kfifo *kfifo = &ctx->bitstream_fifo.kfifo;
	struct coda_dev *dev = ctx->dev;
	u32 wr_ptr;

	wr_ptr = ctx->bitstream.paddr + (kfifo->in & kfifo->mask);
	coda_write(dev, wr_ptr, CODA_REG_BIT_WR_PTR(ctx->reg_idx));
}

static int coda_h264_bitstream_pad(struct coda_ctx *ctx, u32 size)
{
	unsigned char *buf;
	u32 n;

	if (size < 6)
		size = 6;

	buf = kmalloc(size, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	coda_h264_filler_nal(size, buf);
	n = kfifo_in(&ctx->bitstream_fifo, buf, size);
	kfree(buf);

	return (n < size) ? -ENOSPC : 0;
}

int coda_bitstream_flush(struct coda_ctx *ctx)
{
	int ret;

	if (ctx->inst_type != CODA_INST_DECODER || !ctx->use_bit)
		return 0;

	ret = coda_command_sync(ctx, CODA_COMMAND_DEC_BUF_FLUSH);
	if (ret < 0) {
		v4l2_err(&ctx->dev->v4l2_dev, "failed to flush bitstream\n");
		return ret;
	}

	kfifo_init(&ctx->bitstream_fifo, ctx->bitstream.vaddr,
		   ctx->bitstream.size);
	coda_kfifo_sync_to_device_full(ctx);

	return 0;
}

static int coda_bitstream_queue(struct coda_ctx *ctx, const u8 *buf, u32 size)
{
	u32 n = kfifo_in(&ctx->bitstream_fifo, buf, size);

	return (n < size) ? -ENOSPC : 0;
}

static u32 coda_buffer_parse_headers(struct coda_ctx *ctx,
				     struct vb2_v4l2_buffer *src_buf,
				     u32 payload)
{
	u8 *vaddr = vb2_plane_vaddr(&src_buf->vb2_buf, 0);
	u32 size = 0;

	switch (ctx->codec->src_fourcc) {
	case V4L2_PIX_FMT_MPEG2:
		size = coda_mpeg2_parse_headers(ctx, vaddr, payload);
		break;
	case V4L2_PIX_FMT_MPEG4:
		size = coda_mpeg4_parse_headers(ctx, vaddr, payload);
		break;
	default:
		break;
	}

	return size;
}

static bool coda_bitstream_try_queue(struct coda_ctx *ctx,
				     struct vb2_v4l2_buffer *src_buf)
{
	unsigned long payload = vb2_get_plane_payload(&src_buf->vb2_buf, 0);
	u8 *vaddr = vb2_plane_vaddr(&src_buf->vb2_buf, 0);
	int ret;
	int i;

	if (coda_get_bitstream_payload(ctx) + payload + 512 >=
	    ctx->bitstream.size)
		return false;

	if (!vaddr) {
		v4l2_err(&ctx->dev->v4l2_dev, "trying to queue empty buffer\n");
		return true;
	}

	if (ctx->qsequence == 0 && payload < 512) {
		/*
		 * Add padding after the first buffer, if it is too small to be
		 * fetched by the CODA, by repeating the headers. Without
		 * repeated headers, or the first frame already queued, decoder
		 * sequence initialization fails with error code 0x2000 on i.MX6
		 * or error code 0x1 on i.MX51.
		 */
		u32 header_size = coda_buffer_parse_headers(ctx, src_buf,
							    payload);

		if (header_size) {
			coda_dbg(1, ctx, "pad with %u-byte header\n",
				 header_size);
			for (i = payload; i < 512; i += header_size) {
				ret = coda_bitstream_queue(ctx, vaddr,
							   header_size);
				if (ret < 0) {
					v4l2_err(&ctx->dev->v4l2_dev,
						 "bitstream buffer overflow\n");
					return false;
				}
				if (ctx->dev->devtype->product == CODA_960)
					break;
			}
		} else {
			coda_dbg(1, ctx,
				 "could not parse header, sequence initialization might fail\n");
		}

		/* Add padding before the first buffer, if it is too small */
		if (ctx->codec->src_fourcc == V4L2_PIX_FMT_H264)
			coda_h264_bitstream_pad(ctx, 512 - payload);
	}

	ret = coda_bitstream_queue(ctx, vaddr, payload);
	if (ret < 0) {
		v4l2_err(&ctx->dev->v4l2_dev, "bitstream buffer overflow\n");
		return false;
	}

	src_buf->sequence = ctx->qsequence++;

	/* Sync read pointer to device */
	if (ctx == v4l2_m2m_get_curr_priv(ctx->dev->m2m_dev))
		coda_kfifo_sync_to_device_write(ctx);

	/* Set the stream-end flag after the last buffer is queued */
	if (src_buf->flags & V4L2_BUF_FLAG_LAST)
		coda_bit_stream_end_flag(ctx);
	ctx->hold = false;

	return true;
}

void coda_fill_bitstream(struct coda_ctx *ctx, struct list_head *buffer_list)
{
	struct vb2_v4l2_buffer *src_buf;
	struct coda_buffer_meta *meta;
	u32 start;

	if (ctx->bit_stream_param & CODA_BIT_STREAM_END_FLAG)
		return;

	while (v4l2_m2m_num_src_bufs_ready(ctx->fh.m2m_ctx) > 0) {
		/*
		 * Only queue two JPEGs into the bitstream buffer to keep
		 * latency low. We need at least one complete buffer and the
		 * header of another buffer (for prescan) in the bitstream.
		 */
		if (ctx->codec->src_fourcc == V4L2_PIX_FMT_JPEG &&
		    ctx->num_metas > 1)
			break;

		if (ctx->num_internal_frames &&
		    ctx->num_metas >= ctx->num_internal_frames) {
			meta = list_first_entry(&ctx->buffer_meta_list,
						struct coda_buffer_meta, list);

			/*
			 * If we managed to fill in at least a full reorder
			 * window of buffers (num_internal_frames is a
			 * conservative estimate for this) and the bitstream
			 * prefetcher has at least 2 256 bytes periods beyond
			 * the first buffer to fetch, we can safely stop queuing
			 * in order to limit the decoder drain latency.
			 */
			if (coda_bitstream_can_fetch_past(ctx, meta->end))
				break;
		}

		src_buf = v4l2_m2m_next_src_buf(ctx->fh.m2m_ctx);

		/* Drop frames that do not start/end with a SOI/EOI markers */
		if (ctx->codec->src_fourcc == V4L2_PIX_FMT_JPEG &&
		    !coda_jpeg_check_buffer(ctx, &src_buf->vb2_buf)) {
			v4l2_err(&ctx->dev->v4l2_dev,
				 "dropping invalid JPEG frame %d\n",
				 ctx->qsequence);
			src_buf = v4l2_m2m_src_buf_remove(ctx->fh.m2m_ctx);
			if (buffer_list) {
				struct v4l2_m2m_buffer *m2m_buf;

				m2m_buf = container_of(src_buf,
						       struct v4l2_m2m_buffer,
						       vb);
				list_add_tail(&m2m_buf->list, buffer_list);
			} else {
				v4l2_m2m_buf_done(src_buf, VB2_BUF_STATE_ERROR);
			}
			continue;
		}

		/* Dump empty buffers */
		if (!vb2_get_plane_payload(&src_buf->vb2_buf, 0)) {
			src_buf = v4l2_m2m_src_buf_remove(ctx->fh.m2m_ctx);
			v4l2_m2m_buf_done(src_buf, VB2_BUF_STATE_DONE);
			continue;
		}

		/* Buffer start position */
		start = ctx->bitstream_fifo.kfifo.in;

		if (coda_bitstream_try_queue(ctx, src_buf)) {
			/*
			 * Source buffer is queued in the bitstream ringbuffer;
			 * queue the timestamp and mark source buffer as done
			 */
			src_buf = v4l2_m2m_src_buf_remove(ctx->fh.m2m_ctx);

			meta = kmalloc(sizeof(*meta), GFP_KERNEL);
			if (meta) {
				meta->sequence = src_buf->sequence;
				meta->timecode = src_buf->timecode;
				meta->timestamp = src_buf->vb2_buf.timestamp;
				meta->start = start;
				meta->end = ctx->bitstream_fifo.kfifo.in;
				meta->last = src_buf->flags & V4L2_BUF_FLAG_LAST;
				if (meta->last)
					coda_dbg(1, ctx, "marking last meta");
				spin_lock(&ctx->buffer_meta_lock);
				list_add_tail(&meta->list,
					      &ctx->buffer_meta_list);
				ctx->num_metas++;
				spin_unlock(&ctx->buffer_meta_lock);

				trace_coda_bit_queue(ctx, src_buf, meta);
			}

			if (buffer_list) {
				struct v4l2_m2m_buffer *m2m_buf;

				m2m_buf = container_of(src_buf,
						       struct v4l2_m2m_buffer,
						       vb);
				list_add_tail(&m2m_buf->list, buffer_list);
			} else {
				v4l2_m2m_buf_done(src_buf, VB2_BUF_STATE_DONE);
			}
		} else {
			break;
		}
	}
}

void coda_bit_stream_end_flag(struct coda_ctx *ctx)
{
	struct coda_dev *dev = ctx->dev;

	ctx->bit_stream_param |= CODA_BIT_STREAM_END_FLAG;

	/* If this context is currently running, update the hardware flag */
	if ((dev->devtype->product == CODA_960) &&
	    coda_isbusy(dev) &&
	    (ctx->idx == coda_read(dev, CODA_REG_BIT_RUN_INDEX))) {
		coda_write(dev, ctx->bit_stream_param,
			   CODA_REG_BIT_BIT_STREAM_PARAM);
	}
}

static void coda_parabuf_write(struct coda_ctx *ctx, int index, u32 value)
{
	struct coda_dev *dev = ctx->dev;
	u32 *p = ctx->parabuf.vaddr;

	if (dev->devtype->product == CODA_DX6)
		p[index] = value;
	else
		p[index ^ 1] = value;
}

static inline int coda_alloc_context_buf(struct coda_ctx *ctx,
					 struct coda_aux_buf *buf, size_t size,
					 const char *name)
{
	return coda_alloc_aux_buf(ctx->dev, buf, size, name, ctx->debugfs_entry);
}


static void coda_free_framebuffers(struct coda_ctx *ctx)
{
	int i;

	for (i = 0; i < CODA_MAX_FRAMEBUFFERS; i++)
		coda_free_aux_buf(ctx->dev, &ctx->internal_frames[i].buf);
}

static int coda_alloc_framebuffers(struct coda_ctx *ctx,
				   struct coda_q_data *q_data, u32 fourcc)
{
	struct coda_dev *dev = ctx->dev;
	unsigned int ysize, ycbcr_size;
	int ret;
	int i;

	if (ctx->codec->src_fourcc == V4L2_PIX_FMT_H264 ||
	    ctx->codec->dst_fourcc == V4L2_PIX_FMT_H264 ||
	    ctx->codec->src_fourcc == V4L2_PIX_FMT_MPEG4 ||
	    ctx->codec->dst_fourcc == V4L2_PIX_FMT_MPEG4)
		ysize = round_up(q_data->rect.width, 16) *
			round_up(q_data->rect.height, 16);
	else
		ysize = round_up(q_data->rect.width, 8) * q_data->rect.height;

	if (ctx->tiled_map_type == GDI_TILED_FRAME_MB_RASTER_MAP)
		ycbcr_size = round_up(ysize, 4096) + ysize / 2;
	else
		ycbcr_size = ysize + ysize / 2;

	/* Allocate frame buffers */
	for (i = 0; i < ctx->num_internal_frames; i++) {
		size_t size = ycbcr_size;
		char *name;

		/* Add space for mvcol buffers */
		if (dev->devtype->product != CODA_DX6 &&
		    (ctx->codec->src_fourcc == V4L2_PIX_FMT_H264 ||
		     (ctx->codec->src_fourcc == V4L2_PIX_FMT_MPEG4 && i == 0)))
			size += ysize / 4;
		name = kasprintf(GFP_KERNEL, "fb%d", i);
		if (!name) {
			coda_free_framebuffers(ctx);
			return -ENOMEM;
		}
		ret = coda_alloc_context_buf(ctx, &ctx->internal_frames[i].buf,
					     size, name);
		kfree(name);
		if (ret < 0) {
			coda_free_framebuffers(ctx);
			return ret;
		}
	}

	/* Register frame buffers in the parameter buffer */
	for (i = 0; i < ctx->num_internal_frames; i++) {
		u32 y, cb, cr, mvcol;

		/* Start addresses of Y, Cb, Cr planes */
		y = ctx->internal_frames[i].buf.paddr;
		cb = y + ysize;
		cr = y + ysize + ysize/4;
		mvcol = y + ysize + ysize/4 + ysize/4;
		if (ctx->tiled_map_type == GDI_TILED_FRAME_MB_RASTER_MAP) {
			cb = round_up(cb, 4096);
			mvcol = cb + ysize/2;
			cr = 0;
			/* Packed 20-bit MSB of base addresses */
			/* YYYYYCCC, CCyyyyyc, cccc.... */
			y = (y & 0xfffff000) | cb >> 20;
			cb = (cb & 0x000ff000) << 12;
		}
		coda_parabuf_write(ctx, i * 3 + 0, y);
		coda_parabuf_write(ctx, i * 3 + 1, cb);
		coda_parabuf_write(ctx, i * 3 + 2, cr);

		if (dev->devtype->product == CODA_DX6)
			continue;

		/* mvcol buffer for h.264 and mpeg4 */
		if (ctx->codec->src_fourcc == V4L2_PIX_FMT_H264)
			coda_parabuf_write(ctx, 96 + i, mvcol);
		if (ctx->codec->src_fourcc == V4L2_PIX_FMT_MPEG4 && i == 0)
			coda_parabuf_write(ctx, 97, mvcol);
	}

	return 0;
}

static void coda_free_context_buffers(struct coda_ctx *ctx)
{
	struct coda_dev *dev = ctx->dev;

	coda_free_aux_buf(dev, &ctx->slicebuf);
	coda_free_aux_buf(dev, &ctx->psbuf);
	if (dev->devtype->product != CODA_DX6)
		coda_free_aux_buf(dev, &ctx->workbuf);
	coda_free_aux_buf(dev, &ctx->parabuf);
}

static int coda_alloc_context_buffers(struct coda_ctx *ctx,
				      struct coda_q_data *q_data)
{
	struct coda_dev *dev = ctx->dev;
	size_t size;
	int ret;

	if (!ctx->parabuf.vaddr) {
		ret = coda_alloc_context_buf(ctx, &ctx->parabuf,
					     CODA_PARA_BUF_SIZE, "parabuf");
		if (ret < 0)
			return ret;
	}

	if (dev->devtype->product == CODA_DX6)
		return 0;

	if (!ctx->slicebuf.vaddr && q_data->fourcc == V4L2_PIX_FMT_H264) {
		/* worst case slice size */
		size = (DIV_ROUND_UP(q_data->rect.width, 16) *
			DIV_ROUND_UP(q_data->rect.height, 16)) * 3200 / 8 + 512;
		ret = coda_alloc_context_buf(ctx, &ctx->slicebuf, size,
					     "slicebuf");
		if (ret < 0)
			goto err;
	}

	if (!ctx->psbuf.vaddr && (dev->devtype->product == CODA_HX4 ||
				  dev->devtype->product == CODA_7541)) {
		ret = coda_alloc_context_buf(ctx, &ctx->psbuf,
					     CODA7_PS_BUF_SIZE, "psbuf");
		if (ret < 0)
			goto err;
	}

	if (!ctx->workbuf.vaddr) {
		size = dev->devtype->workbuf_size;
		if (dev->devtype->product == CODA_960 &&
		    q_data->fourcc == V4L2_PIX_FMT_H264)
			size += CODA9_PS_SAVE_SIZE;
		ret = coda_alloc_context_buf(ctx, &ctx->workbuf, size,
					     "workbuf");
		if (ret < 0)
			goto err;
	}

	return 0;

err:
	coda_free_context_buffers(ctx);
	return ret;
}

static int coda_encode_header(struct coda_ctx *ctx, struct vb2_v4l2_buffer *buf,
			      int header_code, u8 *header, int *size)
{
	struct vb2_buffer *vb = &buf->vb2_buf;
	struct coda_dev *dev = ctx->dev;
	struct coda_q_data *q_data_src;
	struct v4l2_rect *r;
	size_t bufsize;
	int ret;
	int i;

	if (dev->devtype->product == CODA_960)
		memset(vb2_plane_vaddr(vb, 0), 0, 64);

	coda_write(dev, vb2_dma_contig_plane_dma_addr(vb, 0),
		   CODA_CMD_ENC_HEADER_BB_START);
	bufsize = vb2_plane_size(vb, 0);
	if (dev->devtype->product == CODA_960)
		bufsize /= 1024;
	coda_write(dev, bufsize, CODA_CMD_ENC_HEADER_BB_SIZE);
	if (dev->devtype->product == CODA_960 &&
	    ctx->codec->dst_fourcc == V4L2_PIX_FMT_H264 &&
	    header_code == CODA_HEADER_H264_SPS) {
		q_data_src = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_OUTPUT);
		r = &q_data_src->rect;

		if (r->width % 16 || r->height % 16) {
			u32 crop_right = round_up(r->width, 16) -  r->width;
			u32 crop_bottom = round_up(r->height, 16) - r->height;

			coda_write(dev, crop_right,
				   CODA9_CMD_ENC_HEADER_FRAME_CROP_H);
			coda_write(dev, crop_bottom,
				   CODA9_CMD_ENC_HEADER_FRAME_CROP_V);
			header_code |= CODA9_HEADER_FRAME_CROP;
		}
	}
	coda_write(dev, header_code, CODA_CMD_ENC_HEADER_CODE);
	ret = coda_command_sync(ctx, CODA_COMMAND_ENCODE_HEADER);
	if (ret < 0) {
		v4l2_err(&dev->v4l2_dev, "CODA_COMMAND_ENCODE_HEADER timeout\n");
		return ret;
	}

	if (dev->devtype->product == CODA_960) {
		for (i = 63; i > 0; i--)
			if (((char *)vb2_plane_vaddr(vb, 0))[i] != 0)
				break;
		*size = i + 1;
	} else {
		*size = coda_read(dev, CODA_REG_BIT_WR_PTR(ctx->reg_idx)) -
			coda_read(dev, CODA_CMD_ENC_HEADER_BB_START);
	}
	memcpy(header, vb2_plane_vaddr(vb, 0), *size);

	return 0;
}

static u32 coda_slice_mode(struct coda_ctx *ctx)
{
	int size, unit;

	switch (ctx->params.slice_mode) {
	case V4L2_MPEG_VIDEO_MULTI_SLICE_MODE_SINGLE:
	default:
		return 0;
	case V4L2_MPEG_VIDEO_MULTI_SLICE_MODE_MAX_MB:
		size = ctx->params.slice_max_mb;
		unit = 1;
		break;
	case V4L2_MPEG_VIDEO_MULTI_SLICE_MODE_MAX_BYTES:
		size = ctx->params.slice_max_bits;
		unit = 0;
		break;
	}

	return ((size & CODA_SLICING_SIZE_MASK) << CODA_SLICING_SIZE_OFFSET) |
	       ((unit & CODA_SLICING_UNIT_MASK) << CODA_SLICING_UNIT_OFFSET) |
	       ((1 & CODA_SLICING_MODE_MASK) << CODA_SLICING_MODE_OFFSET);
}

static int coda_enc_param_change(struct coda_ctx *ctx)
{
	struct coda_dev *dev = ctx->dev;
	u32 change_enable = 0;
	u32 success;
	int ret;

	if (ctx->params.gop_size_changed) {
		change_enable |= CODA_PARAM_CHANGE_RC_GOP;
		coda_write(dev, ctx->params.gop_size,
			   CODA_CMD_ENC_PARAM_RC_GOP);
		ctx->gopcounter = ctx->params.gop_size - 1;
		ctx->params.gop_size_changed = false;
	}
	if (ctx->params.h264_intra_qp_changed) {
		coda_dbg(1, ctx, "parameter change: intra Qp %u\n",
			 ctx->params.h264_intra_qp);

		if (ctx->params.bitrate) {
			change_enable |= CODA_PARAM_CHANGE_RC_INTRA_QP;
			coda_write(dev, ctx->params.h264_intra_qp,
				   CODA_CMD_ENC_PARAM_RC_INTRA_QP);
		}
		ctx->params.h264_intra_qp_changed = false;
	}
	if (ctx->params.bitrate_changed) {
		coda_dbg(1, ctx, "parameter change: bitrate %u kbit/s\n",
			 ctx->params.bitrate);
		change_enable |= CODA_PARAM_CHANGE_RC_BITRATE;
		coda_write(dev, ctx->params.bitrate,
			   CODA_CMD_ENC_PARAM_RC_BITRATE);
		ctx->params.bitrate_changed = false;
	}
	if (ctx->params.framerate_changed) {
		coda_dbg(1, ctx, "parameter change: frame rate %u/%u Hz\n",
			 ctx->params.framerate & 0xffff,
			 (ctx->params.framerate >> 16) + 1);
		change_enable |= CODA_PARAM_CHANGE_RC_FRAME_RATE;
		coda_write(dev, ctx->params.framerate,
			   CODA_CMD_ENC_PARAM_RC_FRAME_RATE);
		ctx->params.framerate_changed = false;
	}
	if (ctx->params.intra_refresh_changed) {
		coda_dbg(1, ctx, "parameter change: intra refresh MBs %u\n",
			 ctx->params.intra_refresh);
		change_enable |= CODA_PARAM_CHANGE_INTRA_MB_NUM;
		coda_write(dev, ctx->params.intra_refresh,
			   CODA_CMD_ENC_PARAM_INTRA_MB_NUM);
		ctx->params.intra_refresh_changed = false;
	}
	if (ctx->params.slice_mode_changed) {
		change_enable |= CODA_PARAM_CHANGE_SLICE_MODE;
		coda_write(dev, coda_slice_mode(ctx),
			   CODA_CMD_ENC_PARAM_SLICE_MODE);
		ctx->params.slice_mode_changed = false;
	}

	if (!change_enable)
		return 0;

	coda_write(dev, change_enable, CODA_CMD_ENC_PARAM_CHANGE_ENABLE);

	ret = coda_command_sync(ctx, CODA_COMMAND_RC_CHANGE_PARAMETER);
	if (ret < 0)
		return ret;

	success = coda_read(dev, CODA_RET_ENC_PARAM_CHANGE_SUCCESS);
	if (success != 1)
		coda_dbg(1, ctx, "parameter change failed: %u\n", success);

	return 0;
}

static phys_addr_t coda_iram_alloc(struct coda_iram_info *iram, size_t size)
{
	phys_addr_t ret;

	size = round_up(size, 1024);
	if (size > iram->remaining)
		return 0;
	iram->remaining -= size;

	ret = iram->next_paddr;
	iram->next_paddr += size;

	return ret;
}

static void coda_setup_iram(struct coda_ctx *ctx)
{
	struct coda_iram_info *iram_info = &ctx->iram_info;
	struct coda_dev *dev = ctx->dev;
	int w64, w128;
	int mb_width;
	int dbk_bits;
	int bit_bits;
	int ip_bits;
	int me_bits;

	memset(iram_info, 0, sizeof(*iram_info));
	iram_info->next_paddr = dev->iram.paddr;
	iram_info->remaining = dev->iram.size;

	if (!dev->iram.vaddr)
		return;

	switch (dev->devtype->product) {
	case CODA_HX4:
		dbk_bits = CODA7_USE_HOST_DBK_ENABLE;
		bit_bits = CODA7_USE_HOST_BIT_ENABLE;
		ip_bits = CODA7_USE_HOST_IP_ENABLE;
		me_bits = CODA7_USE_HOST_ME_ENABLE;
		break;
	case CODA_7541:
		dbk_bits = CODA7_USE_HOST_DBK_ENABLE | CODA7_USE_DBK_ENABLE;
		bit_bits = CODA7_USE_HOST_BIT_ENABLE | CODA7_USE_BIT_ENABLE;
		ip_bits = CODA7_USE_HOST_IP_ENABLE | CODA7_USE_IP_ENABLE;
		me_bits = CODA7_USE_HOST_ME_ENABLE | CODA7_USE_ME_ENABLE;
		break;
	case CODA_960:
		dbk_bits = CODA9_USE_HOST_DBK_ENABLE | CODA9_USE_DBK_ENABLE;
		bit_bits = CODA9_USE_HOST_BIT_ENABLE | CODA7_USE_BIT_ENABLE;
		ip_bits = CODA9_USE_HOST_IP_ENABLE | CODA7_USE_IP_ENABLE;
		me_bits = 0;
		break;
	default: /* CODA_DX6 */
		return;
	}

	if (ctx->inst_type == CODA_INST_ENCODER) {
		struct coda_q_data *q_data_src;

		q_data_src = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_OUTPUT);
		mb_width = DIV_ROUND_UP(q_data_src->rect.width, 16);
		w128 = mb_width * 128;
		w64 = mb_width * 64;

		/* Prioritize in case IRAM is too small for everything */
		if (dev->devtype->product == CODA_HX4 ||
		    dev->devtype->product == CODA_7541) {
			iram_info->search_ram_size = round_up(mb_width * 16 *
							      36 + 2048, 1024);
			iram_info->search_ram_paddr = coda_iram_alloc(iram_info,
						iram_info->search_ram_size);
			if (!iram_info->search_ram_paddr) {
				pr_err("IRAM is smaller than the search ram size\n");
				goto out;
			}
			iram_info->axi_sram_use |= me_bits;
		}

		/* Only H.264BP and H.263P3 are considered */
		iram_info->buf_dbk_y_use = coda_iram_alloc(iram_info, w64);
		iram_info->buf_dbk_c_use = coda_iram_alloc(iram_info, w64);
		if (!iram_info->buf_dbk_c_use)
			goto out;
		iram_info->axi_sram_use |= dbk_bits;

		iram_info->buf_bit_use = coda_iram_alloc(iram_info, w128);
		if (!iram_info->buf_bit_use)
			goto out;
		iram_info->axi_sram_use |= bit_bits;

		iram_info->buf_ip_ac_dc_use = coda_iram_alloc(iram_info, w128);
		if (!iram_info->buf_ip_ac_dc_use)
			goto out;
		iram_info->axi_sram_use |= ip_bits;

		/* OVL and BTP disabled for encoder */
	} else if (ctx->inst_type == CODA_INST_DECODER) {
		struct coda_q_data *q_data_dst;

		q_data_dst = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_CAPTURE);
		mb_width = DIV_ROUND_UP(q_data_dst->width, 16);
		w128 = mb_width * 128;

		iram_info->buf_dbk_y_use = coda_iram_alloc(iram_info, w128);
		iram_info->buf_dbk_c_use = coda_iram_alloc(iram_info, w128);
		if (!iram_info->buf_dbk_c_use)
			goto out;
		iram_info->axi_sram_use |= dbk_bits;

		iram_info->buf_bit_use = coda_iram_alloc(iram_info, w128);
		if (!iram_info->buf_bit_use)
			goto out;
		iram_info->axi_sram_use |= bit_bits;

		iram_info->buf_ip_ac_dc_use = coda_iram_alloc(iram_info, w128);
		if (!iram_info->buf_ip_ac_dc_use)
			goto out;
		iram_info->axi_sram_use |= ip_bits;

		/* OVL and BTP unused as there is no VC1 support yet */
	}

out:
	if (!(iram_info->axi_sram_use & CODA7_USE_HOST_IP_ENABLE))
		coda_dbg(1, ctx, "IRAM smaller than needed\n");

	if (dev->devtype->product == CODA_HX4 ||
	    dev->devtype->product == CODA_7541) {
		/* TODO - Enabling these causes picture errors on CODA7541 */
		if (ctx->inst_type == CODA_INST_DECODER) {
			/* fw 1.4.50 */
			iram_info->axi_sram_use &= ~(CODA7_USE_HOST_IP_ENABLE |
						     CODA7_USE_IP_ENABLE);
		} else {
			/* fw 13.4.29 */
			iram_info->axi_sram_use &= ~(CODA7_USE_HOST_IP_ENABLE |
						     CODA7_USE_HOST_DBK_ENABLE |
						     CODA7_USE_IP_ENABLE |
						     CODA7_USE_DBK_ENABLE);
		}
	}
}

static u32 coda_supported_firmwares[] = {
	CODA_FIRMWARE_VERNUM(CODA_DX6, 2, 2, 5),
	CODA_FIRMWARE_VERNUM(CODA_HX4, 1, 4, 50),
	CODA_FIRMWARE_VERNUM(CODA_7541, 1, 4, 50),
	CODA_FIRMWARE_VERNUM(CODA_960, 2, 1, 5),
	CODA_FIRMWARE_VERNUM(CODA_960, 2, 1, 9),
	CODA_FIRMWARE_VERNUM(CODA_960, 2, 3, 10),
	CODA_FIRMWARE_VERNUM(CODA_960, 3, 1, 1),
};

static bool coda_firmware_supported(u32 vernum)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(coda_supported_firmwares); i++)
		if (vernum == coda_supported_firmwares[i])
			return true;
	return false;
}

int coda_check_firmware(struct coda_dev *dev)
{
	u16 product, major, minor, release;
	u32 data;
	int ret;

	ret = clk_prepare_enable(dev->clk_per);
	if (ret)
		goto err_clk_per;

	ret = clk_prepare_enable(dev->clk_ahb);
	if (ret)
		goto err_clk_ahb;

	coda_write(dev, 0, CODA_CMD_FIRMWARE_VERNUM);
	coda_write(dev, CODA_REG_BIT_BUSY_FLAG, CODA_REG_BIT_BUSY);
	coda_write(dev, 0, CODA_REG_BIT_RUN_INDEX);
	coda_write(dev, 0, CODA_REG_BIT_RUN_COD_STD);
	coda_write(dev, CODA_COMMAND_FIRMWARE_GET, CODA_REG_BIT_RUN_COMMAND);
	if (coda_wait_timeout(dev)) {
		v4l2_err(&dev->v4l2_dev, "firmware get command error\n");
		ret = -EIO;
		goto err_run_cmd;
	}

	if (dev->devtype->product == CODA_960) {
		data = coda_read(dev, CODA9_CMD_FIRMWARE_CODE_REV);
		v4l2_info(&dev->v4l2_dev, "Firmware code revision: %d\n",
			  data);
	}

	/* Check we are compatible with the loaded firmware */
	data = coda_read(dev, CODA_CMD_FIRMWARE_VERNUM);
	product = CODA_FIRMWARE_PRODUCT(data);
	major = CODA_FIRMWARE_MAJOR(data);
	minor = CODA_FIRMWARE_MINOR(data);
	release = CODA_FIRMWARE_RELEASE(data);

	clk_disable_unprepare(dev->clk_per);
	clk_disable_unprepare(dev->clk_ahb);

	if (product != dev->devtype->product) {
		v4l2_err(&dev->v4l2_dev,
			 "Wrong firmware. Hw: %s, Fw: %s, Version: %u.%u.%u\n",
			 coda_product_name(dev->devtype->product),
			 coda_product_name(product), major, minor, release);
		return -EINVAL;
	}

	v4l2_info(&dev->v4l2_dev, "Initialized %s.\n",
		  coda_product_name(product));

	if (coda_firmware_supported(data)) {
		v4l2_info(&dev->v4l2_dev, "Firmware version: %u.%u.%u\n",
			  major, minor, release);
	} else {
		v4l2_warn(&dev->v4l2_dev,
			  "Unsupported firmware version: %u.%u.%u\n",
			  major, minor, release);
	}

	return 0;

err_run_cmd:
	clk_disable_unprepare(dev->clk_ahb);
err_clk_ahb:
	clk_disable_unprepare(dev->clk_per);
err_clk_per:
	return ret;
}

static void coda9_set_frame_cache(struct coda_ctx *ctx, u32 fourcc)
{
	u32 cache_size, cache_config;

	if (ctx->tiled_map_type == GDI_LINEAR_FRAME_MAP) {
		/* Luma 2x0 page, 2x6 cache, chroma 2x0 page, 2x4 cache size */
		cache_size = 0x20262024;
		cache_config = 2 << CODA9_CACHE_PAGEMERGE_OFFSET;
	} else {
		/* Luma 0x2 page, 4x4 cache, chroma 0x2 page, 4x3 cache size */
		cache_size = 0x02440243;
		cache_config = 1 << CODA9_CACHE_PAGEMERGE_OFFSET;
	}
	coda_write(ctx->dev, cache_size, CODA9_CMD_SET_FRAME_CACHE_SIZE);
	if (fourcc == V4L2_PIX_FMT_NV12 || fourcc == V4L2_PIX_FMT_YUYV) {
		cache_config |= 32 << CODA9_CACHE_LUMA_BUFFER_SIZE_OFFSET |
				16 << CODA9_CACHE_CR_BUFFER_SIZE_OFFSET |
				0 << CODA9_CACHE_CB_BUFFER_SIZE_OFFSET;
	} else {
		cache_config |= 32 << CODA9_CACHE_LUMA_BUFFER_SIZE_OFFSET |
				8 << CODA9_CACHE_CR_BUFFER_SIZE_OFFSET |
				8 << CODA9_CACHE_CB_BUFFER_SIZE_OFFSET;
	}
	coda_write(ctx->dev, cache_config, CODA9_CMD_SET_FRAME_CACHE_CONFIG);
}

/*
 * Encoder context operations
 */

static int coda_encoder_reqbufs(struct coda_ctx *ctx,
				struct v4l2_requestbuffers *rb)
{
	struct coda_q_data *q_data_src;
	int ret;

	if (rb->type != V4L2_BUF_TYPE_VIDEO_OUTPUT)
		return 0;

	if (rb->count) {
		q_data_src = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_OUTPUT);
		ret = coda_alloc_context_buffers(ctx, q_data_src);
		if (ret < 0)
			return ret;
	} else {
		coda_free_context_buffers(ctx);
	}

	return 0;
}

static int coda_start_encoding(struct coda_ctx *ctx)
{
	struct coda_dev *dev = ctx->dev;
	struct v4l2_device *v4l2_dev = &dev->v4l2_dev;
	struct coda_q_data *q_data_src, *q_data_dst;
	u32 bitstream_buf, bitstream_size;
	struct vb2_v4l2_buffer *buf;
	int gamma, ret, value;
	u32 dst_fourcc;
	int num_fb;
	u32 stride;

	q_data_src = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_OUTPUT);
	q_data_dst = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_CAPTURE);
	dst_fourcc = q_data_dst->fourcc;

	buf = v4l2_m2m_next_dst_buf(ctx->fh.m2m_ctx);
	bitstream_buf = vb2_dma_contig_plane_dma_addr(&buf->vb2_buf, 0);
	bitstream_size = q_data_dst->sizeimage;

	if (!coda_is_initialized(dev)) {
		v4l2_err(v4l2_dev, "coda is not initialized.\n");
		return -EFAULT;
	}

	if (dst_fourcc == V4L2_PIX_FMT_JPEG) {
		if (!ctx->params.jpeg_qmat_tab[0])
			ctx->params.jpeg_qmat_tab[0] = kmalloc(64, GFP_KERNEL);
		if (!ctx->params.jpeg_qmat_tab[1])
			ctx->params.jpeg_qmat_tab[1] = kmalloc(64, GFP_KERNEL);
		coda_set_jpeg_compression_quality(ctx, ctx->params.jpeg_quality);
	}

	mutex_lock(&dev->coda_mutex);

	coda_write(dev, ctx->parabuf.paddr, CODA_REG_BIT_PARA_BUF_ADDR);
	coda_write(dev, bitstream_buf, CODA_REG_BIT_RD_PTR(ctx->reg_idx));
	coda_write(dev, bitstream_buf, CODA_REG_BIT_WR_PTR(ctx->reg_idx));
	switch (dev->devtype->product) {
	case CODA_DX6:
		coda_write(dev, CODADX6_STREAM_BUF_DYNALLOC_EN |
			CODADX6_STREAM_BUF_PIC_RESET, CODA_REG_BIT_STREAM_CTRL);
		break;
	case CODA_960:
		coda_write(dev, 0, CODA9_GDI_WPROT_RGN_EN);
		fallthrough;
	case CODA_HX4:
	case CODA_7541:
		coda_write(dev, CODA7_STREAM_BUF_DYNALLOC_EN |
			CODA7_STREAM_BUF_PIC_RESET, CODA_REG_BIT_STREAM_CTRL);
		break;
	}

	ctx->frame_mem_ctrl &= ~(CODA_FRAME_CHROMA_INTERLEAVE | (0x3 << 9) |
				 CODA9_FRAME_TILED2LINEAR);
	if (q_data_src->fourcc == V4L2_PIX_FMT_NV12)
		ctx->frame_mem_ctrl |= CODA_FRAME_CHROMA_INTERLEAVE;
	if (ctx->tiled_map_type == GDI_TILED_FRAME_MB_RASTER_MAP)
		ctx->frame_mem_ctrl |= (0x3 << 9) | CODA9_FRAME_TILED2LINEAR;
	coda_write(dev, ctx->frame_mem_ctrl, CODA_REG_BIT_FRAME_MEM_CTRL);

	if (dev->devtype->product == CODA_DX6) {
		/* Configure the coda */
		coda_write(dev, dev->iram.paddr,
			   CODADX6_REG_BIT_SEARCH_RAM_BASE_ADDR);
	}

	/* Could set rotation here if needed */
	value = 0;
	switch (dev->devtype->product) {
	case CODA_DX6:
		value = (q_data_src->rect.width & CODADX6_PICWIDTH_MASK)
			<< CODADX6_PICWIDTH_OFFSET;
		value |= (q_data_src->rect.height & CODADX6_PICHEIGHT_MASK)
			 << CODA_PICHEIGHT_OFFSET;
		break;
	case CODA_HX4:
	case CODA_7541:
		if (dst_fourcc == V4L2_PIX_FMT_H264) {
			value = (round_up(q_data_src->rect.width, 16) &
				 CODA7_PICWIDTH_MASK) << CODA7_PICWIDTH_OFFSET;
			value |= (round_up(q_data_src->rect.height, 16) &
				 CODA7_PICHEIGHT_MASK) << CODA_PICHEIGHT_OFFSET;
			break;
		}
		fallthrough;
	case CODA_960:
		value = (q_data_src->rect.width & CODA7_PICWIDTH_MASK)
			<< CODA7_PICWIDTH_OFFSET;
		value |= (q_data_src->rect.height & CODA7_PICHEIGHT_MASK)
			 << CODA_PICHEIGHT_OFFSET;
	}
	coda_write(dev, value, CODA_CMD_ENC_SEQ_SRC_SIZE);
	if (dst_fourcc == V4L2_PIX_FMT_JPEG)
		ctx->params.framerate = 0;
	coda_write(dev, ctx->params.framerate,
		   CODA_CMD_ENC_SEQ_SRC_F_RATE);

	ctx->params.codec_mode = ctx->codec->mode;
	switch (dst_fourcc) {
	case V4L2_PIX_FMT_MPEG4:
		if (dev->devtype->product == CODA_960)
			coda_write(dev, CODA9_STD_MPEG4,
				   CODA_CMD_ENC_SEQ_COD_STD);
		else
			coda_write(dev, CODA_STD_MPEG4,
				   CODA_CMD_ENC_SEQ_COD_STD);
		coda_write(dev, 0, CODA_CMD_ENC_SEQ_MP4_PARA);
		break;
	case V4L2_PIX_FMT_H264:
		if (dev->devtype->product == CODA_960)
			coda_write(dev, CODA9_STD_H264,
				   CODA_CMD_ENC_SEQ_COD_STD);
		else
			coda_write(dev, CODA_STD_H264,
				   CODA_CMD_ENC_SEQ_COD_STD);
		value = ((ctx->params.h264_disable_deblocking_filter_idc &
			  CODA_264PARAM_DISABLEDEBLK_MASK) <<
			 CODA_264PARAM_DISABLEDEBLK_OFFSET) |
			((ctx->params.h264_slice_alpha_c0_offset_div2 &
			  CODA_264PARAM_DEBLKFILTEROFFSETALPHA_MASK) <<
			 CODA_264PARAM_DEBLKFILTEROFFSETALPHA_OFFSET) |
			((ctx->params.h264_slice_beta_offset_div2 &
			  CODA_264PARAM_DEBLKFILTEROFFSETBETA_MASK) <<
			 CODA_264PARAM_DEBLKFILTEROFFSETBETA_OFFSET) |
			(ctx->params.h264_constrained_intra_pred_flag <<
			 CODA_264PARAM_CONSTRAINEDINTRAPREDFLAG_OFFSET) |
			(ctx->params.h264_chroma_qp_index_offset &
			 CODA_264PARAM_CHROMAQPOFFSET_MASK);
		coda_write(dev, value, CODA_CMD_ENC_SEQ_264_PARA);
		break;
	case V4L2_PIX_FMT_JPEG:
		coda_write(dev, 0, CODA_CMD_ENC_SEQ_JPG_PARA);
		coda_write(dev, ctx->params.jpeg_restart_interval,
				CODA_CMD_ENC_SEQ_JPG_RST_INTERVAL);
		coda_write(dev, 0, CODA_CMD_ENC_SEQ_JPG_THUMB_EN);
		coda_write(dev, 0, CODA_CMD_ENC_SEQ_JPG_THUMB_SIZE);
		coda_write(dev, 0, CODA_CMD_ENC_SEQ_JPG_THUMB_OFFSET);

		coda_jpeg_write_tables(ctx);
		break;
	default:
		v4l2_err(v4l2_dev,
			 "dst format (0x%08x) invalid.\n", dst_fourcc);
		ret = -EINVAL;
		goto out;
	}

	/*
	 * slice mode and GOP size registers are used for thumb size/offset
	 * in JPEG mode
	 */
	if (dst_fourcc != V4L2_PIX_FMT_JPEG) {
		value = coda_slice_mode(ctx);
		coda_write(dev, value, CODA_CMD_ENC_SEQ_SLICE_MODE);
		value = ctx->params.gop_size;
		coda_write(dev, value, CODA_CMD_ENC_SEQ_GOP_SIZE);
	}

	if (ctx->params.bitrate && (ctx->params.frame_rc_enable ||
				    ctx->params.mb_rc_enable)) {
		ctx->params.bitrate_changed = false;
		ctx->params.h264_intra_qp_changed = false;

		/* Rate control enabled */
		value = (ctx->params.bitrate & CODA_RATECONTROL_BITRATE_MASK)
			<< CODA_RATECONTROL_BITRATE_OFFSET;
		value |=  1 & CODA_RATECONTROL_ENABLE_MASK;
		value |= (ctx->params.vbv_delay &
			  CODA_RATECONTROL_INITIALDELAY_MASK)
			 << CODA_RATECONTROL_INITIALDELAY_OFFSET;
		if (dev->devtype->product == CODA_960)
			value |= BIT(31); /* disable autoskip */
	} else {
		value = 0;
	}
	coda_write(dev, value, CODA_CMD_ENC_SEQ_RC_PARA);

	coda_write(dev, ctx->params.vbv_size, CODA_CMD_ENC_SEQ_RC_BUF_SIZE);
	coda_write(dev, ctx->params.intra_refresh,
		   CODA_CMD_ENC_SEQ_INTRA_REFRESH);

	coda_write(dev, bitstream_buf, CODA_CMD_ENC_SEQ_BB_START);
	coda_write(dev, bitstream_size / 1024, CODA_CMD_ENC_SEQ_BB_SIZE);


	value = 0;
	if (dev->devtype->product == CODA_960)
		gamma = CODA9_DEFAULT_GAMMA;
	else
		gamma = CODA_DEFAULT_GAMMA;
	if (gamma > 0) {
		coda_write(dev, (gamma & CODA_GAMMA_MASK) << CODA_GAMMA_OFFSET,
			   CODA_CMD_ENC_SEQ_RC_GAMMA);
	}

	if (ctx->params.h264_min_qp || ctx->params.h264_max_qp) {
		coda_write(dev,
			   ctx->params.h264_min_qp << CODA_QPMIN_OFFSET |
			   ctx->params.h264_max_qp << CODA_QPMAX_OFFSET,
			   CODA_CMD_ENC_SEQ_RC_QP_MIN_MAX);
	}
	if (dev->devtype->product == CODA_960) {
		if (ctx->params.h264_max_qp)
			value |= 1 << CODA9_OPTION_RCQPMAX_OFFSET;
		if (CODA_DEFAULT_GAMMA > 0)
			value |= 1 << CODA9_OPTION_GAMMA_OFFSET;
	} else {
		if (CODA_DEFAULT_GAMMA > 0) {
			if (dev->devtype->product == CODA_DX6)
				value |= 1 << CODADX6_OPTION_GAMMA_OFFSET;
			else
				value |= 1 << CODA7_OPTION_GAMMA_OFFSET;
		}
		if (ctx->params.h264_min_qp)
			value |= 1 << CODA7_OPTION_RCQPMIN_OFFSET;
		if (ctx->params.h264_max_qp)
			value |= 1 << CODA7_OPTION_RCQPMAX_OFFSET;
	}
	coda_write(dev, value, CODA_CMD_ENC_SEQ_OPTION);

	if (ctx->params.frame_rc_enable && !ctx->params.mb_rc_enable)
		value = 1;
	else
		value = 0;
	coda_write(dev, value, CODA_CMD_ENC_SEQ_RC_INTERVAL_MODE);

	coda_setup_iram(ctx);

	if (dst_fourcc == V4L2_PIX_FMT_H264) {
		switch (dev->devtype->product) {
		case CODA_DX6:
			value = FMO_SLICE_SAVE_BUF_SIZE << 7;
			coda_write(dev, value, CODADX6_CMD_ENC_SEQ_FMO);
			break;
		case CODA_HX4:
		case CODA_7541:
			coda_write(dev, ctx->iram_info.search_ram_paddr,
					CODA7_CMD_ENC_SEQ_SEARCH_BASE);
			coda_write(dev, ctx->iram_info.search_ram_size,
					CODA7_CMD_ENC_SEQ_SEARCH_SIZE);
			break;
		case CODA_960:
			coda_write(dev, 0, CODA9_CMD_ENC_SEQ_ME_OPTION);
			coda_write(dev, 0, CODA9_CMD_ENC_SEQ_INTRA_WEIGHT);
		}
	}

	ret = coda_command_sync(ctx, CODA_COMMAND_SEQ_INIT);
	if (ret < 0) {
		v4l2_err(v4l2_dev, "CODA_COMMAND_SEQ_INIT timeout\n");
		goto out;
	}

	if (coda_read(dev, CODA_RET_ENC_SEQ_SUCCESS) == 0) {
		v4l2_err(v4l2_dev, "CODA_COMMAND_SEQ_INIT failed\n");
		ret = -EFAULT;
		goto out;
	}
	ctx->initialized = 1;

	if (dst_fourcc != V4L2_PIX_FMT_JPEG) {
		if (dev->devtype->product == CODA_960)
			ctx->num_internal_frames = 4;
		else
			ctx->num_internal_frames = 2;
		ret = coda_alloc_framebuffers(ctx, q_data_src, dst_fourcc);
		if (ret < 0) {
			v4l2_err(v4l2_dev, "failed to allocate framebuffers\n");
			goto out;
		}
		num_fb = 2;
		stride = q_data_src->bytesperline;
	} else {
		ctx->num_internal_frames = 0;
		num_fb = 0;
		stride = 0;
	}
	coda_write(dev, num_fb, CODA_CMD_SET_FRAME_BUF_NUM);
	coda_write(dev, stride, CODA_CMD_SET_FRAME_BUF_STRIDE);

	if (dev->devtype->product == CODA_HX4 ||
	    dev->devtype->product == CODA_7541) {
		coda_write(dev, q_data_src->bytesperline,
				CODA7_CMD_SET_FRAME_SOURCE_BUF_STRIDE);
	}
	if (dev->devtype->product != CODA_DX6) {
		coda_write(dev, ctx->iram_info.buf_bit_use,
				CODA7_CMD_SET_FRAME_AXI_BIT_ADDR);
		coda_write(dev, ctx->iram_info.buf_ip_ac_dc_use,
				CODA7_CMD_SET_FRAME_AXI_IPACDC_ADDR);
		coda_write(dev, ctx->iram_info.buf_dbk_y_use,
				CODA7_CMD_SET_FRAME_AXI_DBKY_ADDR);
		coda_write(dev, ctx->iram_info.buf_dbk_c_use,
				CODA7_CMD_SET_FRAME_AXI_DBKC_ADDR);
		coda_write(dev, ctx->iram_info.buf_ovl_use,
				CODA7_CMD_SET_FRAME_AXI_OVL_ADDR);
		if (dev->devtype->product == CODA_960) {
			coda_write(dev, ctx->iram_info.buf_btp_use,
					CODA9_CMD_SET_FRAME_AXI_BTP_ADDR);

			coda9_set_frame_cache(ctx, q_data_src->fourcc);

			/* FIXME */
			coda_write(dev, ctx->internal_frames[2].buf.paddr,
				   CODA9_CMD_SET_FRAME_SUBSAMP_A);
			coda_write(dev, ctx->internal_frames[3].buf.paddr,
				   CODA9_CMD_SET_FRAME_SUBSAMP_B);
		}
	}

	ret = coda_command_sync(ctx, CODA_COMMAND_SET_FRAME_BUF);
	if (ret < 0) {
		v4l2_err(v4l2_dev, "CODA_COMMAND_SET_FRAME_BUF timeout\n");
		goto out;
	}

	coda_dbg(1, ctx, "start encoding %dx%d %4.4s->%4.4s @ %d/%d Hz\n",
		 q_data_src->rect.width, q_data_src->rect.height,
		 (char *)&ctx->codec->src_fourcc, (char *)&dst_fourcc,
		 ctx->params.framerate & 0xffff,
		 (ctx->params.framerate >> 16) + 1);

	/* Save stream headers */
	buf = v4l2_m2m_next_dst_buf(ctx->fh.m2m_ctx);
	switch (dst_fourcc) {
	case V4L2_PIX_FMT_H264:
		/*
		 * Get SPS in the first frame and copy it to an
		 * intermediate buffer.
		 */
		ret = coda_encode_header(ctx, buf, CODA_HEADER_H264_SPS,
					 &ctx->vpu_header[0][0],
					 &ctx->vpu_header_size[0]);
		if (ret < 0)
			goto out;

		/*
		 * If visible width or height are not aligned to macroblock
		 * size, the crop_right and crop_bottom SPS fields must be set
		 * to the difference between visible and coded size.  This is
		 * only supported by CODA960 firmware. All others do not allow
		 * writing frame cropping parameters, so we have to manually
		 * fix up the SPS RBSP (Sequence Parameter Set Raw Byte
		 * Sequence Payload) ourselves.
		 */
		if (ctx->dev->devtype->product != CODA_960 &&
		    ((q_data_src->rect.width % 16) ||
		     (q_data_src->rect.height % 16))) {
			ret = coda_h264_sps_fixup(ctx, q_data_src->rect.width,
						  q_data_src->rect.height,
						  &ctx->vpu_header[0][0],
						  &ctx->vpu_header_size[0],
						  sizeof(ctx->vpu_header[0]));
			if (ret < 0)
				goto out;
		}

		/*
		 * Get PPS in the first frame and copy it to an
		 * intermediate buffer.
		 */
		ret = coda_encode_header(ctx, buf, CODA_HEADER_H264_PPS,
					 &ctx->vpu_header[1][0],
					 &ctx->vpu_header_size[1]);
		if (ret < 0)
			goto out;

		/*
		 * Length of H.264 headers is variable and thus it might not be
		 * aligned for the coda to append the encoded frame. In that is
		 * the case a filler NAL must be added to header 2.
		 */
		ctx->vpu_header_size[2] = coda_h264_padding(
					(ctx->vpu_header_size[0] +
					 ctx->vpu_header_size[1]),
					 ctx->vpu_header[2]);
		break;
	case V4L2_PIX_FMT_MPEG4:
		/*
		 * Get VOS in the first frame and copy it to an
		 * intermediate buffer
		 */
		ret = coda_encode_header(ctx, buf, CODA_HEADER_MP4V_VOS,
					 &ctx->vpu_header[0][0],
					 &ctx->vpu_header_size[0]);
		if (ret < 0)
			goto out;

		ret = coda_encode_header(ctx, buf, CODA_HEADER_MP4V_VIS,
					 &ctx->vpu_header[1][0],
					 &ctx->vpu_header_size[1]);
		if (ret < 0)
			goto out;

		ret = coda_encode_header(ctx, buf, CODA_HEADER_MP4V_VOL,
					 &ctx->vpu_header[2][0],
					 &ctx->vpu_header_size[2]);
		if (ret < 0)
			goto out;
		break;
	default:
		/* No more formats need to save headers at the moment */
		break;
	}

out:
	mutex_unlock(&dev->coda_mutex);
	return ret;
}

static int coda_prepare_encode(struct coda_ctx *ctx)
{
	struct coda_q_data *q_data_src, *q_data_dst;
	struct vb2_v4l2_buffer *src_buf, *dst_buf;
	struct coda_dev *dev = ctx->dev;
	int force_ipicture;
	int quant_param = 0;
	u32 pic_stream_buffer_addr, pic_stream_buffer_size;
	u32 rot_mode = 0;
	u32 dst_fourcc;
	u32 reg;
	int ret;

	ret = coda_enc_param_change(ctx);
	if (ret < 0) {
		v4l2_warn(&ctx->dev->v4l2_dev, "parameter change failed: %d\n",
			  ret);
	}

	src_buf = v4l2_m2m_next_src_buf(ctx->fh.m2m_ctx);
	dst_buf = v4l2_m2m_next_dst_buf(ctx->fh.m2m_ctx);
	q_data_src = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_OUTPUT);
	q_data_dst = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_CAPTURE);
	dst_fourcc = q_data_dst->fourcc;

	src_buf->sequence = ctx->osequence;
	dst_buf->sequence = ctx->osequence;
	ctx->osequence++;

	force_ipicture = ctx->params.force_ipicture;
	if (force_ipicture)
		ctx->params.force_ipicture = false;
	else if (ctx->params.gop_size != 0 &&
		 (src_buf->sequence % ctx->params.gop_size) == 0)
		force_ipicture = 1;

	/*
	 * Workaround coda firmware BUG that only marks the first
	 * frame as IDR. This is a problem for some decoders that can't
	 * recover when a frame is lost.
	 */
	if (!force_ipicture) {
		src_buf->flags |= V4L2_BUF_FLAG_PFRAME;
		src_buf->flags &= ~V4L2_BUF_FLAG_KEYFRAME;
	} else {
		src_buf->flags |= V4L2_BUF_FLAG_KEYFRAME;
		src_buf->flags &= ~V4L2_BUF_FLAG_PFRAME;
	}

	if (dev->devtype->product == CODA_960)
		coda_set_gdi_regs(ctx);

	/*
	 * Copy headers in front of the first frame and forced I frames for
	 * H.264 only. In MPEG4 they are already copied by the CODA.
	 */
	if (src_buf->sequence == 0 || force_ipicture) {
		pic_stream_buffer_addr =
			vb2_dma_contig_plane_dma_addr(&dst_buf->vb2_buf, 0) +
			ctx->vpu_header_size[0] +
			ctx->vpu_header_size[1] +
			ctx->vpu_header_size[2];
		pic_stream_buffer_size = q_data_dst->sizeimage -
			ctx->vpu_header_size[0] -
			ctx->vpu_header_size[1] -
			ctx->vpu_header_size[2];
		memcpy(vb2_plane_vaddr(&dst_buf->vb2_buf, 0),
		       &ctx->vpu_header[0][0], ctx->vpu_header_size[0]);
		memcpy(vb2_plane_vaddr(&dst_buf->vb2_buf, 0)
			+ ctx->vpu_header_size[0], &ctx->vpu_header[1][0],
			ctx->vpu_header_size[1]);
		memcpy(vb2_plane_vaddr(&dst_buf->vb2_buf, 0)
			+ ctx->vpu_header_size[0] + ctx->vpu_header_size[1],
			&ctx->vpu_header[2][0], ctx->vpu_header_size[2]);
	} else {
		pic_stream_buffer_addr =
			vb2_dma_contig_plane_dma_addr(&dst_buf->vb2_buf, 0);
		pic_stream_buffer_size = q_data_dst->sizeimage;
	}

	if (force_ipicture) {
		switch (dst_fourcc) {
		case V4L2_PIX_FMT_H264:
			quant_param = ctx->params.h264_intra_qp;
			break;
		case V4L2_PIX_FMT_MPEG4:
			quant_param = ctx->params.mpeg4_intra_qp;
			break;
		case V4L2_PIX_FMT_JPEG:
			quant_param = 30;
			break;
		default:
			v4l2_warn(&ctx->dev->v4l2_dev,
				"cannot set intra qp, fmt not supported\n");
			break;
		}
	} else {
		switch (dst_fourcc) {
		case V4L2_PIX_FMT_H264:
			quant_param = ctx->params.h264_inter_qp;
			break;
		case V4L2_PIX_FMT_MPEG4:
			quant_param = ctx->params.mpeg4_inter_qp;
			break;
		default:
			v4l2_warn(&ctx->dev->v4l2_dev,
				"cannot set inter qp, fmt not supported\n");
			break;
		}
	}

	/* submit */
	if (ctx->params.rot_mode)
		rot_mode = CODA_ROT_MIR_ENABLE | ctx->params.rot_mode;
	coda_write(dev, rot_mode, CODA_CMD_ENC_PIC_ROT_MODE);
	coda_write(dev, quant_param, CODA_CMD_ENC_PIC_QS);

	if (dev->devtype->product == CODA_960) {
		coda_write(dev, 4/*FIXME: 0*/, CODA9_CMD_ENC_PIC_SRC_INDEX);
		coda_write(dev, q_data_src->bytesperline,
			   CODA9_CMD_ENC_PIC_SRC_STRIDE);
		coda_write(dev, 0, CODA9_CMD_ENC_PIC_SUB_FRAME_SYNC);

		reg = CODA9_CMD_ENC_PIC_SRC_ADDR_Y;
	} else {
		reg = CODA_CMD_ENC_PIC_SRC_ADDR_Y;
	}
	coda_write_base(ctx, q_data_src, src_buf, reg);

	coda_write(dev, force_ipicture << 1 & 0x2,
		   CODA_CMD_ENC_PIC_OPTION);

	coda_write(dev, pic_stream_buffer_addr, CODA_CMD_ENC_PIC_BB_START);
	coda_write(dev, pic_stream_buffer_size / 1024,
		   CODA_CMD_ENC_PIC_BB_SIZE);

	if (!ctx->streamon_out) {
		/* After streamoff on the output side, set stream end flag */
		ctx->bit_stream_param |= CODA_BIT_STREAM_END_FLAG;
		coda_write(dev, ctx->bit_stream_param,
			   CODA_REG_BIT_BIT_STREAM_PARAM);
	}

	if (dev->devtype->product != CODA_DX6)
		coda_write(dev, ctx->iram_info.axi_sram_use,
				CODA7_REG_BIT_AXI_SRAM_USE);

	trace_coda_enc_pic_run(ctx, src_buf);

	coda_command_async(ctx, CODA_COMMAND_PIC_RUN);

	return 0;
}

static char coda_frame_type_char(u32 flags)
{
	return (flags & V4L2_BUF_FLAG_KEYFRAME) ? 'I' :
	       (flags & V4L2_BUF_FLAG_PFRAME) ? 'P' :
	       (flags & V4L2_BUF_FLAG_BFRAME) ? 'B' : '?';
}

static void coda_finish_encode(struct coda_ctx *ctx)
{
	struct vb2_v4l2_buffer *src_buf, *dst_buf;
	struct coda_dev *dev = ctx->dev;
	u32 wr_ptr, start_ptr;

	if (ctx->aborting)
		return;

	/*
	 * Lock to make sure that an encoder stop command running in parallel
	 * will either already have marked src_buf as last, or it will wake up
	 * the capture queue after the buffers are returned.
	 */
	mutex_lock(&ctx->wakeup_mutex);
	src_buf = v4l2_m2m_src_buf_remove(ctx->fh.m2m_ctx);
	dst_buf = v4l2_m2m_next_dst_buf(ctx->fh.m2m_ctx);

	trace_coda_enc_pic_done(ctx, dst_buf);

	/* Get results from the coda */
	start_ptr = coda_read(dev, CODA_CMD_ENC_PIC_BB_START);
	wr_ptr = coda_read(dev, CODA_REG_BIT_WR_PTR(ctx->reg_idx));

	/* Calculate bytesused field */
	if (dst_buf->sequence == 0 ||
	    src_buf->flags & V4L2_BUF_FLAG_KEYFRAME) {
		vb2_set_plane_payload(&dst_buf->vb2_buf, 0, wr_ptr - start_ptr +
					ctx->vpu_header_size[0] +
					ctx->vpu_header_size[1] +
					ctx->vpu_header_size[2]);
	} else {
		vb2_set_plane_payload(&dst_buf->vb2_buf, 0, wr_ptr - start_ptr);
	}

	coda_dbg(1, ctx, "frame size = %u\n", wr_ptr - start_ptr);

	coda_read(dev, CODA_RET_ENC_PIC_SLICE_NUM);
	coda_read(dev, CODA_RET_ENC_PIC_FLAG);

	dst_buf->flags &= ~(V4L2_BUF_FLAG_KEYFRAME |
			    V4L2_BUF_FLAG_PFRAME |
			    V4L2_BUF_FLAG_LAST);
	if (coda_read(dev, CODA_RET_ENC_PIC_TYPE) == 0)
		dst_buf->flags |= V4L2_BUF_FLAG_KEYFRAME;
	else
		dst_buf->flags |= V4L2_BUF_FLAG_PFRAME;
	dst_buf->flags |= src_buf->flags & V4L2_BUF_FLAG_LAST;

	v4l2_m2m_buf_copy_metadata(src_buf, dst_buf, false);

	v4l2_m2m_buf_done(src_buf, VB2_BUF_STATE_DONE);

	dst_buf = v4l2_m2m_dst_buf_remove(ctx->fh.m2m_ctx);
	coda_m2m_buf_done(ctx, dst_buf, VB2_BUF_STATE_DONE);
	mutex_unlock(&ctx->wakeup_mutex);

	ctx->gopcounter--;
	if (ctx->gopcounter < 0)
		ctx->gopcounter = ctx->params.gop_size - 1;

	coda_dbg(1, ctx, "job finished: encoded %c frame (%d)%s\n",
		 coda_frame_type_char(dst_buf->flags), dst_buf->sequence,
		 (dst_buf->flags & V4L2_BUF_FLAG_LAST) ? " (last)" : "");
}

static void coda_seq_end_work(struct work_struct *work)
{
	struct coda_ctx *ctx = container_of(work, struct coda_ctx, seq_end_work);
	struct coda_dev *dev = ctx->dev;

	mutex_lock(&ctx->buffer_mutex);
	mutex_lock(&dev->coda_mutex);

	if (ctx->initialized == 0)
		goto out;

	coda_dbg(1, ctx, "%s: sent command 'SEQ_END' to coda\n", __func__);
	if (coda_command_sync(ctx, CODA_COMMAND_SEQ_END)) {
		v4l2_err(&dev->v4l2_dev,
			 "CODA_COMMAND_SEQ_END failed\n");
	}

	/*
	 * FIXME: Sometimes h.264 encoding fails with 8-byte sequences missing
	 * from the output stream after the h.264 decoder has run. Resetting the
	 * hardware after the decoder has finished seems to help.
	 */
	if (dev->devtype->product == CODA_960)
		coda_hw_reset(ctx);

	kfifo_init(&ctx->bitstream_fifo,
		ctx->bitstream.vaddr, ctx->bitstream.size);

	coda_free_framebuffers(ctx);

	ctx->initialized = 0;

out:
	mutex_unlock(&dev->coda_mutex);
	mutex_unlock(&ctx->buffer_mutex);
}

static void coda_bit_release(struct coda_ctx *ctx)
{
	mutex_lock(&ctx->buffer_mutex);
	coda_free_framebuffers(ctx);
	coda_free_context_buffers(ctx);
	coda_free_bitstream_buffer(ctx);
	mutex_unlock(&ctx->buffer_mutex);
}

const struct coda_context_ops coda_bit_encode_ops = {
	.queue_init = coda_encoder_queue_init,
	.reqbufs = coda_encoder_reqbufs,
	.start_streaming = coda_start_encoding,
	.prepare_run = coda_prepare_encode,
	.finish_run = coda_finish_encode,
	.seq_end_work = coda_seq_end_work,
	.release = coda_bit_release,
};

/*
 * Decoder context operations
 */

static int coda_alloc_bitstream_buffer(struct coda_ctx *ctx,
				       struct coda_q_data *q_data)
{
	if (ctx->bitstream.vaddr)
		return 0;

	ctx->bitstream.size = roundup_pow_of_two(q_data->sizeimage * 2);
	ctx->bitstream.vaddr = dma_alloc_wc(ctx->dev->dev, ctx->bitstream.size,
					    &ctx->bitstream.paddr, GFP_KERNEL);
	if (!ctx->bitstream.vaddr) {
		v4l2_err(&ctx->dev->v4l2_dev,
			 "failed to allocate bitstream ringbuffer");
		return -ENOMEM;
	}
	kfifo_init(&ctx->bitstream_fifo,
		   ctx->bitstream.vaddr, ctx->bitstream.size);

	return 0;
}

static void coda_free_bitstream_buffer(struct coda_ctx *ctx)
{
	if (ctx->bitstream.vaddr == NULL)
		return;

	dma_free_wc(ctx->dev->dev, ctx->bitstream.size, ctx->bitstream.vaddr,
		    ctx->bitstream.paddr);
	ctx->bitstream.vaddr = NULL;
	kfifo_init(&ctx->bitstream_fifo, NULL, 0);
}

static int coda_decoder_reqbufs(struct coda_ctx *ctx,
				struct v4l2_requestbuffers *rb)
{
	struct coda_q_data *q_data_src;
	int ret;

	if (rb->type != V4L2_BUF_TYPE_VIDEO_OUTPUT)
		return 0;

	if (rb->count) {
		q_data_src = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_OUTPUT);
		ret = coda_alloc_context_buffers(ctx, q_data_src);
		if (ret < 0)
			return ret;
		ret = coda_alloc_bitstream_buffer(ctx, q_data_src);
		if (ret < 0) {
			coda_free_context_buffers(ctx);
			return ret;
		}
	} else {
		coda_free_bitstream_buffer(ctx);
		coda_free_context_buffers(ctx);
	}

	return 0;
}

static bool coda_reorder_enable(struct coda_ctx *ctx)
{
	struct coda_dev *dev = ctx->dev;
	int profile;

	if (dev->devtype->product != CODA_HX4 &&
	    dev->devtype->product != CODA_7541 &&
	    dev->devtype->product != CODA_960)
		return false;

	if (ctx->codec->src_fourcc == V4L2_PIX_FMT_JPEG)
		return false;

	if (ctx->codec->src_fourcc != V4L2_PIX_FMT_H264)
		return true;

	profile = coda_h264_profile(ctx->params.h264_profile_idc);
	if (profile < 0)
		v4l2_warn(&dev->v4l2_dev, "Unknown H264 Profile: %u\n",
			  ctx->params.h264_profile_idc);

	/* Baseline profile does not support reordering */
	return profile > V4L2_MPEG_VIDEO_H264_PROFILE_BASELINE;
}

static void coda_decoder_drop_used_metas(struct coda_ctx *ctx)
{
	struct coda_buffer_meta *meta, *tmp;

	/*
	 * All metas that end at or before the RD pointer (fifo out),
	 * are now consumed by the VPU and should be released.
	 */
	spin_lock(&ctx->buffer_meta_lock);
	list_for_each_entry_safe(meta, tmp, &ctx->buffer_meta_list, list) {
		if (ctx->bitstream_fifo.kfifo.out >= meta->end) {
			coda_dbg(2, ctx, "releasing meta: seq=%d start=%d end=%d\n",
				 meta->sequence, meta->start, meta->end);

			list_del(&meta->list);
			ctx->num_metas--;
			ctx->first_frame_sequence++;
			kfree(meta);
		}
	}
	spin_unlock(&ctx->buffer_meta_lock);
}

static int __coda_decoder_seq_init(struct coda_ctx *ctx)
{
	struct coda_q_data *q_data_src, *q_data_dst;
	u32 bitstream_buf, bitstream_size;
	struct coda_dev *dev = ctx->dev;
	int width, height;
	u32 src_fourcc, dst_fourcc;
	u32 val;
	int ret;

	lockdep_assert_held(&dev->coda_mutex);

	coda_dbg(1, ctx, "Video Data Order Adapter: %s\n",
		 ctx->use_vdoa ? "Enabled" : "Disabled");

	/* Start decoding */
	q_data_src = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_OUTPUT);
	q_data_dst = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_CAPTURE);
	bitstream_buf = ctx->bitstream.paddr;
	bitstream_size = ctx->bitstream.size;
	src_fourcc = q_data_src->fourcc;
	dst_fourcc = q_data_dst->fourcc;

	/* Update coda bitstream read and write pointers from kfifo */
	coda_kfifo_sync_to_device_full(ctx);

	ctx->frame_mem_ctrl &= ~(CODA_FRAME_CHROMA_INTERLEAVE | (0x3 << 9) |
				 CODA9_FRAME_TILED2LINEAR);
	if (dst_fourcc == V4L2_PIX_FMT_NV12 || dst_fourcc == V4L2_PIX_FMT_YUYV)
		ctx->frame_mem_ctrl |= CODA_FRAME_CHROMA_INTERLEAVE;
	if (ctx->tiled_map_type == GDI_TILED_FRAME_MB_RASTER_MAP)
		ctx->frame_mem_ctrl |= (0x3 << 9) |
			((ctx->use_vdoa) ? 0 : CODA9_FRAME_TILED2LINEAR);
	coda_write(dev, ctx->frame_mem_ctrl, CODA_REG_BIT_FRAME_MEM_CTRL);

	ctx->display_idx = -1;
	ctx->frm_dis_flg = 0;
	coda_write(dev, 0, CODA_REG_BIT_FRM_DIS_FLG(ctx->reg_idx));

	coda_write(dev, bitstream_buf, CODA_CMD_DEC_SEQ_BB_START);
	coda_write(dev, bitstream_size / 1024, CODA_CMD_DEC_SEQ_BB_SIZE);
	val = 0;
	if (coda_reorder_enable(ctx))
		val |= CODA_REORDER_ENABLE;
	if (ctx->codec->src_fourcc == V4L2_PIX_FMT_JPEG)
		val |= CODA_NO_INT_ENABLE;
	coda_write(dev, val, CODA_CMD_DEC_SEQ_OPTION);

	ctx->params.codec_mode = ctx->codec->mode;
	if (dev->devtype->product == CODA_960 &&
	    src_fourcc == V4L2_PIX_FMT_MPEG4)
		ctx->params.codec_mode_aux = CODA_MP4_AUX_MPEG4;
	else
		ctx->params.codec_mode_aux = 0;
	if (src_fourcc == V4L2_PIX_FMT_MPEG4) {
		coda_write(dev, CODA_MP4_CLASS_MPEG4,
			   CODA_CMD_DEC_SEQ_MP4_ASP_CLASS);
	}
	if (src_fourcc == V4L2_PIX_FMT_H264) {
		if (dev->devtype->product == CODA_HX4 ||
		    dev->devtype->product == CODA_7541) {
			coda_write(dev, ctx->psbuf.paddr,
					CODA_CMD_DEC_SEQ_PS_BB_START);
			coda_write(dev, (CODA7_PS_BUF_SIZE / 1024),
					CODA_CMD_DEC_SEQ_PS_BB_SIZE);
		}
		if (dev->devtype->product == CODA_960) {
			coda_write(dev, 0, CODA_CMD_DEC_SEQ_X264_MV_EN);
			coda_write(dev, 512, CODA_CMD_DEC_SEQ_SPP_CHUNK_SIZE);
		}
	}
	if (src_fourcc == V4L2_PIX_FMT_JPEG)
		coda_write(dev, 0, CODA_CMD_DEC_SEQ_JPG_THUMB_EN);
	if (dev->devtype->product != CODA_960)
		coda_write(dev, 0, CODA_CMD_DEC_SEQ_SRC_SIZE);

	ctx->bit_stream_param = CODA_BIT_DEC_SEQ_INIT_ESCAPE;
	ret = coda_command_sync(ctx, CODA_COMMAND_SEQ_INIT);
	ctx->bit_stream_param = 0;
	if (ret) {
		v4l2_err(&dev->v4l2_dev, "CODA_COMMAND_SEQ_INIT timeout\n");
		return ret;
	}
	ctx->sequence_offset = ~0U;
	ctx->initialized = 1;
	ctx->first_frame_sequence = 0;

	/* Update kfifo out pointer from coda bitstream read pointer */
	coda_kfifo_sync_from_device(ctx);

	/*
	 * After updating the read pointer, we need to check if
	 * any metas are consumed and should be released.
	 */
	coda_decoder_drop_used_metas(ctx);

	if (coda_read(dev, CODA_RET_DEC_SEQ_SUCCESS) == 0) {
		v4l2_err(&dev->v4l2_dev,
			"CODA_COMMAND_SEQ_INIT failed, error code = 0x%x\n",
			coda_read(dev, CODA_RET_DEC_SEQ_ERR_REASON));
		return -EAGAIN;
	}

	val = coda_read(dev, CODA_RET_DEC_SEQ_SRC_SIZE);
	if (dev->devtype->product == CODA_DX6) {
		width = (val >> CODADX6_PICWIDTH_OFFSET) & CODADX6_PICWIDTH_MASK;
		height = val & CODADX6_PICHEIGHT_MASK;
	} else {
		width = (val >> CODA7_PICWIDTH_OFFSET) & CODA7_PICWIDTH_MASK;
		height = val & CODA7_PICHEIGHT_MASK;
	}

	if (width > q_data_dst->bytesperline || height > q_data_dst->height) {
		v4l2_err(&dev->v4l2_dev, "stream is %dx%d, not %dx%d\n",
			 width, height, q_data_dst->bytesperline,
			 q_data_dst->height);
		return -EINVAL;
	}

	width = round_up(width, 16);
	height = round_up(height, 16);

	coda_dbg(1, ctx, "start decoding: %dx%d\n", width, height);

	ctx->num_internal_frames = coda_read(dev, CODA_RET_DEC_SEQ_FRAME_NEED);
	/*
	 * If the VDOA is used, the decoder needs one additional frame,
	 * because the frames are freed when the next frame is decoded.
	 * Otherwise there are visible errors in the decoded frames (green
	 * regions in displayed frames) and a broken order of frames (earlier
	 * frames are sporadically displayed after later frames).
	 */
	if (ctx->use_vdoa)
		ctx->num_internal_frames += 1;
	if (ctx->num_internal_frames > CODA_MAX_FRAMEBUFFERS) {
		v4l2_err(&dev->v4l2_dev,
			 "not enough framebuffers to decode (%d < %d)\n",
			 CODA_MAX_FRAMEBUFFERS, ctx->num_internal_frames);
		return -EINVAL;
	}

	if (src_fourcc == V4L2_PIX_FMT_H264) {
		u32 left_right;
		u32 top_bottom;

		left_right = coda_read(dev, CODA_RET_DEC_SEQ_CROP_LEFT_RIGHT);
		top_bottom = coda_read(dev, CODA_RET_DEC_SEQ_CROP_TOP_BOTTOM);

		q_data_dst->rect.left = (left_right >> 10) & 0x3ff;
		q_data_dst->rect.top = (top_bottom >> 10) & 0x3ff;
		q_data_dst->rect.width = width - q_data_dst->rect.left -
					 (left_right & 0x3ff);
		q_data_dst->rect.height = height - q_data_dst->rect.top -
					  (top_bottom & 0x3ff);
	}

	if (dev->devtype->product != CODA_DX6) {
		u8 profile, level;

		val = coda_read(dev, CODA7_RET_DEC_SEQ_HEADER_REPORT);
		profile = val & 0xff;
		level = (val >> 8) & 0x7f;

		if (profile || level)
			coda_update_profile_level_ctrls(ctx, profile, level);
	}

	return 0;
}

static void coda_dec_seq_init_work(struct work_struct *work)
{
	struct coda_ctx *ctx = container_of(work,
					    struct coda_ctx, seq_init_work);
	struct coda_dev *dev = ctx->dev;

	mutex_lock(&ctx->buffer_mutex);
	mutex_lock(&dev->coda_mutex);

	if (!ctx->initialized)
		__coda_decoder_seq_init(ctx);

	mutex_unlock(&dev->coda_mutex);
	mutex_unlock(&ctx->buffer_mutex);
}

static int __coda_start_decoding(struct coda_ctx *ctx)
{
	struct coda_q_data *q_data_src, *q_data_dst;
	struct coda_dev *dev = ctx->dev;
	u32 src_fourcc, dst_fourcc;
	int ret;

	if (!ctx->initialized) {
		ret = __coda_decoder_seq_init(ctx);
		if (ret < 0)
			return ret;
	}

	q_data_src = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_OUTPUT);
	q_data_dst = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_CAPTURE);
	src_fourcc = q_data_src->fourcc;
	dst_fourcc = q_data_dst->fourcc;

	coda_write(dev, ctx->parabuf.paddr, CODA_REG_BIT_PARA_BUF_ADDR);

	ret = coda_alloc_framebuffers(ctx, q_data_dst, src_fourcc);
	if (ret < 0) {
		v4l2_err(&dev->v4l2_dev, "failed to allocate framebuffers\n");
		return ret;
	}

	/* Tell the decoder how many frame buffers we allocated. */
	coda_write(dev, ctx->num_internal_frames, CODA_CMD_SET_FRAME_BUF_NUM);
	coda_write(dev, round_up(q_data_dst->rect.width, 16),
		   CODA_CMD_SET_FRAME_BUF_STRIDE);

	if (dev->devtype->product != CODA_DX6) {
		/* Set secondary AXI IRAM */
		coda_setup_iram(ctx);

		coda_write(dev, ctx->iram_info.buf_bit_use,
				CODA7_CMD_SET_FRAME_AXI_BIT_ADDR);
		coda_write(dev, ctx->iram_info.buf_ip_ac_dc_use,
				CODA7_CMD_SET_FRAME_AXI_IPACDC_ADDR);
		coda_write(dev, ctx->iram_info.buf_dbk_y_use,
				CODA7_CMD_SET_FRAME_AXI_DBKY_ADDR);
		coda_write(dev, ctx->iram_info.buf_dbk_c_use,
				CODA7_CMD_SET_FRAME_AXI_DBKC_ADDR);
		coda_write(dev, ctx->iram_info.buf_ovl_use,
				CODA7_CMD_SET_FRAME_AXI_OVL_ADDR);
		if (dev->devtype->product == CODA_960) {
			coda_write(dev, ctx->iram_info.buf_btp_use,
					CODA9_CMD_SET_FRAME_AXI_BTP_ADDR);

			coda_write(dev, -1, CODA9_CMD_SET_FRAME_DELAY);
			coda9_set_frame_cache(ctx, dst_fourcc);
		}
	}

	if (src_fourcc == V4L2_PIX_FMT_H264) {
		coda_write(dev, ctx->slicebuf.paddr,
				CODA_CMD_SET_FRAME_SLICE_BB_START);
		coda_write(dev, ctx->slicebuf.size / 1024,
				CODA_CMD_SET_FRAME_SLICE_BB_SIZE);
	}

	if (dev->devtype->product == CODA_HX4 ||
	    dev->devtype->product == CODA_7541) {
		int max_mb_x = 1920 / 16;
		int max_mb_y = 1088 / 16;
		int max_mb_num = max_mb_x * max_mb_y;

		coda_write(dev, max_mb_num << 16 | max_mb_x << 8 | max_mb_y,
				CODA7_CMD_SET_FRAME_MAX_DEC_SIZE);
	} else if (dev->devtype->product == CODA_960) {
		int max_mb_x = 1920 / 16;
		int max_mb_y = 1088 / 16;
		int max_mb_num = max_mb_x * max_mb_y;

		coda_write(dev, max_mb_num << 16 | max_mb_x << 8 | max_mb_y,
				CODA9_CMD_SET_FRAME_MAX_DEC_SIZE);
	}

	if (coda_command_sync(ctx, CODA_COMMAND_SET_FRAME_BUF)) {
		v4l2_err(&ctx->dev->v4l2_dev,
			 "CODA_COMMAND_SET_FRAME_BUF timeout\n");
		return -ETIMEDOUT;
	}

	return 0;
}

static int coda_start_decoding(struct coda_ctx *ctx)
{
	struct coda_dev *dev = ctx->dev;
	int ret;

	mutex_lock(&dev->coda_mutex);
	ret = __coda_start_decoding(ctx);
	mutex_unlock(&dev->coda_mutex);

	return ret;
}

static int coda_prepare_decode(struct coda_ctx *ctx)
{
	struct vb2_v4l2_buffer *dst_buf;
	struct coda_dev *dev = ctx->dev;
	struct coda_q_data *q_data_dst;
	struct coda_buffer_meta *meta;
	u32 rot_mode = 0;
	u32 reg_addr, reg_stride;

	dst_buf = v4l2_m2m_next_dst_buf(ctx->fh.m2m_ctx);
	q_data_dst = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_CAPTURE);

	/* Try to copy source buffer contents into the bitstream ringbuffer */
	mutex_lock(&ctx->bitstream_mutex);
	coda_fill_bitstream(ctx, NULL);
	mutex_unlock(&ctx->bitstream_mutex);

	if (coda_get_bitstream_payload(ctx) < 512 &&
	    (!(ctx->bit_stream_param & CODA_BIT_STREAM_END_FLAG))) {
		coda_dbg(1, ctx, "bitstream payload: %d, skipping\n",
			 coda_get_bitstream_payload(ctx));
		v4l2_m2m_job_finish(ctx->dev->m2m_dev, ctx->fh.m2m_ctx);
		return -EAGAIN;
	}

	/* Run coda_start_decoding (again) if not yet initialized */
	if (!ctx->initialized) {
		int ret = __coda_start_decoding(ctx);

		if (ret < 0) {
			v4l2_err(&dev->v4l2_dev, "failed to start decoding\n");
			v4l2_m2m_job_finish(ctx->dev->m2m_dev, ctx->fh.m2m_ctx);
			return -EAGAIN;
		} else {
			ctx->initialized = 1;
		}
	}

	if (dev->devtype->product == CODA_960)
		coda_set_gdi_regs(ctx);

	if (ctx->use_vdoa &&
	    ctx->display_idx >= 0 &&
	    ctx->display_idx < ctx->num_internal_frames) {
		vdoa_device_run(ctx->vdoa,
				vb2_dma_contig_plane_dma_addr(&dst_buf->vb2_buf, 0),
				ctx->internal_frames[ctx->display_idx].buf.paddr);
	} else {
		if (dev->devtype->product == CODA_960) {
			/*
			 * It was previously assumed that the CODA960 has an
			 * internal list of 64 buffer entries that contains
			 * both the registered internal frame buffers as well
			 * as the rotator buffer output, and that the ROT_INDEX
			 * register must be set to a value between the last
			 * internal frame buffers' index and 64.
			 * At least on firmware version 3.1.1 it turns out that
			 * setting ROT_INDEX to any value >= 32 causes CODA
			 * hangups that it can not recover from with the SRC VPU
			 * reset.
			 * It does appear to work however, to just set it to a
			 * fixed value in the [ctx->num_internal_frames, 31]
			 * range, for example CODA_MAX_FRAMEBUFFERS.
			 */
			coda_write(dev, CODA_MAX_FRAMEBUFFERS,
				   CODA9_CMD_DEC_PIC_ROT_INDEX);

			reg_addr = CODA9_CMD_DEC_PIC_ROT_ADDR_Y;
			reg_stride = CODA9_CMD_DEC_PIC_ROT_STRIDE;
		} else {
			reg_addr = CODA_CMD_DEC_PIC_ROT_ADDR_Y;
			reg_stride = CODA_CMD_DEC_PIC_ROT_STRIDE;
		}
		coda_write_base(ctx, q_data_dst, dst_buf, reg_addr);
		coda_write(dev, q_data_dst->bytesperline, reg_stride);

		rot_mode = CODA_ROT_MIR_ENABLE | ctx->params.rot_mode;
	}

	coda_write(dev, rot_mode, CODA_CMD_DEC_PIC_ROT_MODE);

	switch (dev->devtype->product) {
	case CODA_DX6:
		/* TBD */
	case CODA_HX4:
	case CODA_7541:
		coda_write(dev, CODA_PRE_SCAN_EN, CODA_CMD_DEC_PIC_OPTION);
		break;
	case CODA_960:
		/* 'hardcode to use interrupt disable mode'? */
		coda_write(dev, (1 << 10), CODA_CMD_DEC_PIC_OPTION);
		break;
	}

	coda_write(dev, 0, CODA_CMD_DEC_PIC_SKIP_NUM);

	coda_write(dev, 0, CODA_CMD_DEC_PIC_BB_START);
	coda_write(dev, 0, CODA_CMD_DEC_PIC_START_BYTE);

	if (dev->devtype->product != CODA_DX6)
		coda_write(dev, ctx->iram_info.axi_sram_use,
				CODA7_REG_BIT_AXI_SRAM_USE);

	spin_lock(&ctx->buffer_meta_lock);
	meta = list_first_entry_or_null(&ctx->buffer_meta_list,
					struct coda_buffer_meta, list);

	if (meta && ctx->codec->src_fourcc == V4L2_PIX_FMT_JPEG) {

		/* If this is the last buffer in the bitstream, add padding */
		if (meta->end == ctx->bitstream_fifo.kfifo.in) {
			static unsigned char buf[512];
			unsigned int pad;

			/* Pad to multiple of 256 and then add 256 more */
			pad = ((0 - meta->end) & 0xff) + 256;

			memset(buf, 0xff, sizeof(buf));

			kfifo_in(&ctx->bitstream_fifo, buf, pad);
		}
	}
	spin_unlock(&ctx->buffer_meta_lock);

	coda_kfifo_sync_to_device_full(ctx);

	/* Clear decode success flag */
	coda_write(dev, 0, CODA_RET_DEC_PIC_SUCCESS);

	/* Clear error return value */
	coda_write(dev, 0, CODA_RET_DEC_PIC_ERR_MB);

	trace_coda_dec_pic_run(ctx, meta);

	coda_command_async(ctx, CODA_COMMAND_PIC_RUN);

	return 0;
}

static void coda_finish_decode(struct coda_ctx *ctx)
{
	struct coda_dev *dev = ctx->dev;
	struct coda_q_data *q_data_src;
	struct coda_q_data *q_data_dst;
	struct vb2_v4l2_buffer *dst_buf;
	struct coda_buffer_meta *meta;
	int width, height;
	int decoded_idx;
	int display_idx;
	struct coda_internal_frame *decoded_frame = NULL;
	u32 src_fourcc;
	int success;
	u32 err_mb;
	int err_vdoa = 0;
	u32 val;

	if (ctx->aborting)
		return;

	/* Update kfifo out pointer from coda bitstream read pointer */
	coda_kfifo_sync_from_device(ctx);

	/*
	 * in stream-end mode, the read pointer can overshoot the write pointer
	 * by up to 512 bytes
	 */
	if (ctx->bit_stream_param & CODA_BIT_STREAM_END_FLAG) {
		if (coda_get_bitstream_payload(ctx) >= ctx->bitstream.size - 512)
			kfifo_init(&ctx->bitstream_fifo,
				ctx->bitstream.vaddr, ctx->bitstream.size);
	}

	q_data_src = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_OUTPUT);
	src_fourcc = q_data_src->fourcc;

	val = coda_read(dev, CODA_RET_DEC_PIC_SUCCESS);
	if (val != 1)
		pr_err("DEC_PIC_SUCCESS = %d\n", val);

	success = val & 0x1;
	if (!success)
		v4l2_err(&dev->v4l2_dev, "decode failed\n");

	if (src_fourcc == V4L2_PIX_FMT_H264) {
		if (val & (1 << 3))
			v4l2_err(&dev->v4l2_dev,
				 "insufficient PS buffer space (%d bytes)\n",
				 ctx->psbuf.size);
		if (val & (1 << 2))
			v4l2_err(&dev->v4l2_dev,
				 "insufficient slice buffer space (%d bytes)\n",
				 ctx->slicebuf.size);
	}

	val = coda_read(dev, CODA_RET_DEC_PIC_SIZE);
	width = (val >> 16) & 0xffff;
	height = val & 0xffff;

	q_data_dst = get_q_data(ctx, V4L2_BUF_TYPE_VIDEO_CAPTURE);

	/* frame crop information */
	if (src_fourcc == V4L2_PIX_FMT_H264) {
		u32 left_right;
		u32 top_bottom;

		left_right = coda_read(dev, CODA_RET_DEC_PIC_CROP_LEFT_RIGHT);
		top_bottom = coda_read(dev, CODA_RET_DEC_PIC_CROP_TOP_BOTTOM);

		if (left_right == 0xffffffff && top_bottom == 0xffffffff) {
			/* Keep current crop information */
		} else {
			struct v4l2_rect *rect = &q_data_dst->rect;

			rect->left = left_right >> 16 & 0xffff;
			rect->top = top_bottom >> 16 & 0xffff;
			rect->width = width - rect->left -
				      (left_right & 0xffff);
			rect->height = height - rect->top -
				       (top_bottom & 0xffff);
		}
	} else {
		/* no cropping */
	}

	err_mb = coda_read(dev, CODA_RET_DEC_PIC_ERR_MB);
	if (err_mb > 0) {
		if (__ratelimit(&dev->mb_err_rs))
			coda_dbg(1, ctx, "errors in %d macroblocks\n", err_mb);
		v4l2_ctrl_s_ctrl(ctx->mb_err_cnt_ctrl,
				 v4l2_ctrl_g_ctrl(ctx->mb_err_cnt_ctrl) + err_mb);
	}

	if (dev->devtype->product == CODA_HX4 ||
	    dev->devtype->product == CODA_7541) {
		val = coda_read(dev, CODA_RET_DEC_PIC_OPTION);
		if (val == 0) {
			/* not enough bitstream data */
			coda_dbg(1, ctx, "prescan failed: %d\n", val);
			ctx->hold = true;
			return;
		}
	}

	/* Wait until the VDOA finished writing the previous display frame */
	if (ctx->use_vdoa &&
	    ctx->display_idx >= 0 &&
	    ctx->display_idx < ctx->num_internal_frames) {
		err_vdoa = vdoa_wait_for_completion(ctx->vdoa);
	}

	ctx->frm_dis_flg = coda_read(dev,
				     CODA_REG_BIT_FRM_DIS_FLG(ctx->reg_idx));

	/* The previous display frame was copied out and can be overwritten */
	if (ctx->display_idx >= 0 &&
	    ctx->display_idx < ctx->num_internal_frames) {
		ctx->frm_dis_flg &= ~(1 << ctx->display_idx);
		coda_write(dev, ctx->frm_dis_flg,
				CODA_REG_BIT_FRM_DIS_FLG(ctx->reg_idx));
	}

	/*
	 * The index of the last decoded frame, not necessarily in
	 * display order, and the index of the next display frame.
	 * The latter could have been decoded in a previous run.
	 */
	decoded_idx = coda_read(dev, CODA_RET_DEC_PIC_CUR_IDX);
	display_idx = coda_read(dev, CODA_RET_DEC_PIC_FRAME_IDX);

	if (decoded_idx == -1) {
		/* no frame was decoded, but we might have a display frame */
		if (display_idx >= 0 && display_idx < ctx->num_internal_frames)
			ctx->sequence_offset++;
		else if (ctx->display_idx < 0)
			ctx->hold = true;
	} else if (decoded_idx == -2) {
		if (ctx->display_idx >= 0 &&
		    ctx->display_idx < ctx->num_internal_frames)
			ctx->sequence_offset++;
		/* no frame was decoded, we still return remaining buffers */
	} else if (decoded_idx < 0 || decoded_idx >= ctx->num_internal_frames) {
		v4l2_err(&dev->v4l2_dev,
			 "decoded frame index out of range: %d\n", decoded_idx);
	} else {
		int sequence;

		decoded_frame = &ctx->internal_frames[decoded_idx];

		val = coda_read(dev, CODA_RET_DEC_PIC_FRAME_NUM);
		if (ctx->sequence_offset == -1)
			ctx->sequence_offset = val;

		sequence = val + ctx->first_frame_sequence
			       - ctx->sequence_offset;
		spin_lock(&ctx->buffer_meta_lock);
		if (!list_empty(&ctx->buffer_meta_list)) {
			meta = list_first_entry(&ctx->buffer_meta_list,
					      struct coda_buffer_meta, list);
			list_del(&meta->list);
			ctx->num_metas--;
			spin_unlock(&ctx->buffer_meta_lock);
			/*
			 * Clamp counters to 16 bits for comparison, as the HW
			 * counter rolls over at this point for h.264. This
			 * may be different for other formats, but using 16 bits
			 * should be enough to detect most errors and saves us
			 * from doing different things based on the format.
			 */
			if ((sequence & 0xffff) != (meta->sequence & 0xffff)) {
				v4l2_err(&dev->v4l2_dev,
					 "sequence number mismatch (%d(%d) != %d)\n",
					 sequence, ctx->sequence_offset,
					 meta->sequence);
			}
			decoded_frame->meta = *meta;
			kfree(meta);
		} else {
			spin_unlock(&ctx->buffer_meta_lock);
			v4l2_err(&dev->v4l2_dev, "empty timestamp list!\n");
			memset(&decoded_frame->meta, 0,
			       sizeof(struct coda_buffer_meta));
			decoded_frame->meta.sequence = sequence;
			decoded_frame->meta.last = false;
			ctx->sequence_offset++;
		}

		trace_coda_dec_pic_done(ctx, &decoded_frame->meta);

		val = coda_read(dev, CODA_RET_DEC_PIC_TYPE) & 0x7;
		decoded_frame->type = (val == 0) ? V4L2_BUF_FLAG_KEYFRAME :
				      (val == 1) ? V4L2_BUF_FLAG_PFRAME :
						   V4L2_BUF_FLAG_BFRAME;

		decoded_frame->error = err_mb;
	}

	if (display_idx == -1) {
		/*
		 * no more frames to be decoded, but there could still
		 * be rotator output to dequeue
		 */
		ctx->hold = true;
	} else if (display_idx == -3) {
		/* possibly prescan failure */
	} else if (display_idx < 0 || display_idx >= ctx->num_internal_frames) {
		v4l2_err(&dev->v4l2_dev,
			 "presentation frame index out of range: %d\n",
			 display_idx);
	}

	/* If a frame was copied out, return it */
	if (ctx->display_idx >= 0 &&
	    ctx->display_idx < ctx->num_internal_frames) {
		struct coda_internal_frame *ready_frame;

		ready_frame = &ctx->internal_frames[ctx->display_idx];

		dst_buf = v4l2_m2m_dst_buf_remove(ctx->fh.m2m_ctx);
		dst_buf->sequence = ctx->osequence++;

		dst_buf->field = V4L2_FIELD_NONE;
		dst_buf->flags &= ~(V4L2_BUF_FLAG_KEYFRAME |
					     V4L2_BUF_FLAG_PFRAME |
					     V4L2_BUF_FLAG_BFRAME);
		dst_buf->flags |= ready_frame->type;
		meta = &ready_frame->meta;
		if (meta->last && !coda_reorder_enable(ctx)) {
			/*
			 * If this was the last decoded frame, and reordering
			 * is disabled, this will be the last display frame.
			 */
			coda_dbg(1, ctx, "last meta, marking as last frame\n");
			dst_buf->flags |= V4L2_BUF_FLAG_LAST;
		} else if (ctx->bit_stream_param & CODA_BIT_STREAM_END_FLAG &&
			   display_idx == -1) {
			/*
			 * If there is no designated presentation frame anymore,
			 * this frame has to be the last one.
			 */
			coda_dbg(1, ctx,
				 "no more frames to return, marking as last frame\n");
			dst_buf->flags |= V4L2_BUF_FLAG_LAST;
		}
		dst_buf->timecode = meta->timecode;
		dst_buf->vb2_buf.timestamp = meta->timestamp;

		trace_coda_dec_rot_done(ctx, dst_buf, meta);

		vb2_set_plane_payload(&dst_buf->vb2_buf, 0,
				      q_data_dst->sizeimage);

		if (ready_frame->error || err_vdoa)
			coda_m2m_buf_done(ctx, dst_buf, VB2_BUF_STATE_ERROR);
		else
			coda_m2m_buf_done(ctx, dst_buf, VB2_BUF_STATE_DONE);

		if (decoded_frame) {
			coda_dbg(1, ctx, "job finished: decoded %c frame %u, returned %c frame %u (%u/%u)%s\n",
				 coda_frame_type_char(decoded_frame->type),
				 decoded_frame->meta.sequence,
				 coda_frame_type_char(dst_buf->flags),
				 ready_frame->meta.sequence,
				 dst_buf->sequence, ctx->qsequence,
				 (dst_buf->flags & V4L2_BUF_FLAG_LAST) ?
				 " (last)" : "");
		} else {
			coda_dbg(1, ctx, "job finished: no frame decoded (%d), returned %c frame %u (%u/%u)%s\n",
				 decoded_idx,
				 coda_frame_type_char(dst_buf->flags),
				 ready_frame->meta.sequence,
				 dst_buf->sequence, ctx->qsequence,
				 (dst_buf->flags & V4L2_BUF_FLAG_LAST) ?
				 " (last)" : "");
		}
	} else {
		if (decoded_frame) {
			coda_dbg(1, ctx, "job finished: decoded %c frame %u, no frame returned (%d)\n",
				 coda_frame_type_char(decoded_frame->type),
				 decoded_frame->meta.sequence,
				 ctx->display_idx);
		} else {
			coda_dbg(1, ctx, "job finished: no frame decoded (%d) or returned (%d)\n",
				 decoded_idx, ctx->display_idx);
		}
	}

	/* The rotator will copy the current display frame next time */
	ctx->display_idx = display_idx;

	/*
	 * The current decode run might have brought the bitstream fill level
	 * below the size where we can start the next decode run. As userspace
	 * might have filled the output queue completely and might thus be
	 * blocked, we can't rely on the next qbuf to trigger the bitstream
	 * refill. Check if we have data to refill the bitstream now.
	 */
	mutex_lock(&ctx->bitstream_mutex);
	coda_fill_bitstream(ctx, NULL);
	mutex_unlock(&ctx->bitstream_mutex);
}

static void coda_decode_timeout(struct coda_ctx *ctx)
{
	struct vb2_v4l2_buffer *dst_buf;

	/*
	 * For now this only handles the case where we would deadlock with
	 * userspace, i.e. userspace issued DEC_CMD_STOP and waits for EOS,
	 * but after a failed decode run we would hold the context and wait for
	 * userspace to queue more buffers.
	 */
	if (!(ctx->bit_stream_param & CODA_BIT_STREAM_END_FLAG))
		return;

	dst_buf = v4l2_m2m_dst_buf_remove(ctx->fh.m2m_ctx);
	dst_buf->sequence = ctx->qsequence - 1;

	coda_m2m_buf_done(ctx, dst_buf, VB2_BUF_STATE_ERROR);
}

const struct coda_context_ops coda_bit_decode_ops = {
	.queue_init = coda_decoder_queue_init,
	.reqbufs = coda_decoder_reqbufs,
	.start_streaming = coda_start_decoding,
	.prepare_run = coda_prepare_decode,
	.finish_run = coda_finish_decode,
	.run_timeout = coda_decode_timeout,
	.seq_init_work = coda_dec_seq_init_work,
	.seq_end_work = coda_seq_end_work,
	.release = coda_bit_release,
};

irqreturn_t coda_irq_handler(int irq, void *data)
{
	struct coda_dev *dev = data;
	struct coda_ctx *ctx;

	/* read status register to attend the IRQ */
	coda_read(dev, CODA_REG_BIT_INT_STATUS);
	coda_write(dev, 0, CODA_REG_BIT_INT_REASON);
	coda_write(dev, CODA_REG_BIT_INT_CLEAR_SET,
		      CODA_REG_BIT_INT_CLEAR);

	ctx = v4l2_m2m_get_curr_priv(dev->m2m_dev);
	if (ctx == NULL) {
		v4l2_err(&dev->v4l2_dev,
			 "Instance released before the end of transaction\n");
		return IRQ_HANDLED;
	}

	trace_coda_bit_done(ctx);

	if (ctx->aborting) {
		coda_dbg(1, ctx, "task has been aborted\n");
	}

	if (coda_isbusy(ctx->dev)) {
		coda_dbg(1, ctx, "coda is still busy!!!!\n");
		return IRQ_NONE;
	}

	complete(&ctx->completion);

	return IRQ_HANDLED;
}
