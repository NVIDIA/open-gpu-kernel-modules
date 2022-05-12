// SPDX-License-Identifier: GPL-2.0-or-later
/*
    file operation functions
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo.com>
    Copyright (C) 2004  Chris Kennedy <c@groovy.org>
    Copyright (C) 2005-2007  Hans Verkuil <hverkuil@xs4all.nl>

 */

#include "ivtv-driver.h"
#include "ivtv-fileops.h"
#include "ivtv-i2c.h"
#include "ivtv-queue.h"
#include "ivtv-udma.h"
#include "ivtv-irq.h"
#include "ivtv-vbi.h"
#include "ivtv-mailbox.h"
#include "ivtv-routing.h"
#include "ivtv-streams.h"
#include "ivtv-yuv.h"
#include "ivtv-ioctl.h"
#include "ivtv-cards.h"
#include "ivtv-firmware.h"
#include <media/v4l2-event.h>
#include <media/i2c/saa7115.h>

/* This function tries to claim the stream for a specific file descriptor.
   If no one else is using this stream then the stream is claimed and
   associated VBI streams are also automatically claimed.
   Possible error returns: -EBUSY if someone else has claimed
   the stream or 0 on success. */
int ivtv_claim_stream(struct ivtv_open_id *id, int type)
{
	struct ivtv *itv = id->itv;
	struct ivtv_stream *s = &itv->streams[type];
	struct ivtv_stream *s_vbi;
	int vbi_type;

	if (test_and_set_bit(IVTV_F_S_CLAIMED, &s->s_flags)) {
		/* someone already claimed this stream */
		if (s->fh == &id->fh) {
			/* yes, this file descriptor did. So that's OK. */
			return 0;
		}
		if (s->fh == NULL && (type == IVTV_DEC_STREAM_TYPE_VBI ||
					 type == IVTV_ENC_STREAM_TYPE_VBI)) {
			/* VBI is handled already internally, now also assign
			   the file descriptor to this stream for external
			   reading of the stream. */
			s->fh = &id->fh;
			IVTV_DEBUG_INFO("Start Read VBI\n");
			return 0;
		}
		/* someone else is using this stream already */
		IVTV_DEBUG_INFO("Stream %d is busy\n", type);
		return -EBUSY;
	}
	s->fh = &id->fh;
	if (type == IVTV_DEC_STREAM_TYPE_VBI) {
		/* Enable reinsertion interrupt */
		ivtv_clear_irq_mask(itv, IVTV_IRQ_DEC_VBI_RE_INSERT);
	}

	/* IVTV_DEC_STREAM_TYPE_MPG needs to claim IVTV_DEC_STREAM_TYPE_VBI,
	   IVTV_ENC_STREAM_TYPE_MPG needs to claim IVTV_ENC_STREAM_TYPE_VBI
	   (provided VBI insertion is on and sliced VBI is selected), for all
	   other streams we're done */
	if (type == IVTV_DEC_STREAM_TYPE_MPG) {
		vbi_type = IVTV_DEC_STREAM_TYPE_VBI;
	} else if (type == IVTV_ENC_STREAM_TYPE_MPG &&
		   itv->vbi.insert_mpeg && !ivtv_raw_vbi(itv)) {
		vbi_type = IVTV_ENC_STREAM_TYPE_VBI;
	} else {
		return 0;
	}
	s_vbi = &itv->streams[vbi_type];

	if (!test_and_set_bit(IVTV_F_S_CLAIMED, &s_vbi->s_flags)) {
		/* Enable reinsertion interrupt */
		if (vbi_type == IVTV_DEC_STREAM_TYPE_VBI)
			ivtv_clear_irq_mask(itv, IVTV_IRQ_DEC_VBI_RE_INSERT);
	}
	/* mark that it is used internally */
	set_bit(IVTV_F_S_INTERNAL_USE, &s_vbi->s_flags);
	return 0;
}
EXPORT_SYMBOL(ivtv_claim_stream);

/* This function releases a previously claimed stream. It will take into
   account associated VBI streams. */
void ivtv_release_stream(struct ivtv_stream *s)
{
	struct ivtv *itv = s->itv;
	struct ivtv_stream *s_vbi;

	s->fh = NULL;
	if ((s->type == IVTV_DEC_STREAM_TYPE_VBI || s->type == IVTV_ENC_STREAM_TYPE_VBI) &&
		test_bit(IVTV_F_S_INTERNAL_USE, &s->s_flags)) {
		/* this stream is still in use internally */
		return;
	}
	if (!test_and_clear_bit(IVTV_F_S_CLAIMED, &s->s_flags)) {
		IVTV_DEBUG_WARN("Release stream %s not in use!\n", s->name);
		return;
	}

	ivtv_flush_queues(s);

	/* disable reinsertion interrupt */
	if (s->type == IVTV_DEC_STREAM_TYPE_VBI)
		ivtv_set_irq_mask(itv, IVTV_IRQ_DEC_VBI_RE_INSERT);

	/* IVTV_DEC_STREAM_TYPE_MPG needs to release IVTV_DEC_STREAM_TYPE_VBI,
	   IVTV_ENC_STREAM_TYPE_MPG needs to release IVTV_ENC_STREAM_TYPE_VBI,
	   for all other streams we're done */
	if (s->type == IVTV_DEC_STREAM_TYPE_MPG)
		s_vbi = &itv->streams[IVTV_DEC_STREAM_TYPE_VBI];
	else if (s->type == IVTV_ENC_STREAM_TYPE_MPG)
		s_vbi = &itv->streams[IVTV_ENC_STREAM_TYPE_VBI];
	else
		return;

	/* clear internal use flag */
	if (!test_and_clear_bit(IVTV_F_S_INTERNAL_USE, &s_vbi->s_flags)) {
		/* was already cleared */
		return;
	}
	if (s_vbi->fh) {
		/* VBI stream still claimed by a file descriptor */
		return;
	}
	/* disable reinsertion interrupt */
	if (s_vbi->type == IVTV_DEC_STREAM_TYPE_VBI)
		ivtv_set_irq_mask(itv, IVTV_IRQ_DEC_VBI_RE_INSERT);
	clear_bit(IVTV_F_S_CLAIMED, &s_vbi->s_flags);
	ivtv_flush_queues(s_vbi);
}
EXPORT_SYMBOL(ivtv_release_stream);

static void ivtv_dualwatch(struct ivtv *itv)
{
	struct v4l2_tuner vt;
	u32 new_stereo_mode;
	const u32 dual = 0x02;

	new_stereo_mode = v4l2_ctrl_g_ctrl(itv->cxhdl.audio_mode);
	memset(&vt, 0, sizeof(vt));
	ivtv_call_all(itv, tuner, g_tuner, &vt);
	if (vt.audmode == V4L2_TUNER_MODE_LANG1_LANG2 && (vt.rxsubchans & V4L2_TUNER_SUB_LANG2))
		new_stereo_mode = dual;

	if (new_stereo_mode == itv->dualwatch_stereo_mode)
		return;

	IVTV_DEBUG_INFO("dualwatch: change stereo flag from 0x%x to 0x%x.\n",
			   itv->dualwatch_stereo_mode, new_stereo_mode);
	if (v4l2_ctrl_s_ctrl(itv->cxhdl.audio_mode, new_stereo_mode))
		IVTV_DEBUG_INFO("dualwatch: changing stereo flag failed\n");
}

static void ivtv_update_pgm_info(struct ivtv *itv)
{
	u32 wr_idx = (read_enc(itv->pgm_info_offset) - itv->pgm_info_offset - 4) / 24;
	int cnt;
	int i = 0;

	if (wr_idx >= itv->pgm_info_num) {
		IVTV_DEBUG_WARN("Invalid PGM index %d (>= %d)\n", wr_idx, itv->pgm_info_num);
		return;
	}
	cnt = (wr_idx + itv->pgm_info_num - itv->pgm_info_write_idx) % itv->pgm_info_num;
	while (i < cnt) {
		int idx = (itv->pgm_info_write_idx + i) % itv->pgm_info_num;
		struct v4l2_enc_idx_entry *e = itv->pgm_info + idx;
		u32 addr = itv->pgm_info_offset + 4 + idx * 24;
		const int mapping[8] = { -1, V4L2_ENC_IDX_FRAME_I, V4L2_ENC_IDX_FRAME_P, -1,
			V4L2_ENC_IDX_FRAME_B, -1, -1, -1 };
					// 1=I, 2=P, 4=B

		e->offset = read_enc(addr + 4) + ((u64)read_enc(addr + 8) << 32);
		if (e->offset > itv->mpg_data_received) {
			break;
		}
		e->offset += itv->vbi_data_inserted;
		e->length = read_enc(addr);
		e->pts = read_enc(addr + 16) + ((u64)(read_enc(addr + 20) & 1) << 32);
		e->flags = mapping[read_enc(addr + 12) & 7];
		i++;
	}
	itv->pgm_info_write_idx = (itv->pgm_info_write_idx + i) % itv->pgm_info_num;
}

static struct ivtv_buffer *ivtv_get_buffer(struct ivtv_stream *s, int non_block, int *err)
{
	struct ivtv *itv = s->itv;
	struct ivtv_stream *s_vbi = &itv->streams[IVTV_ENC_STREAM_TYPE_VBI];
	struct ivtv_buffer *buf;
	DEFINE_WAIT(wait);

	*err = 0;
	while (1) {
		if (s->type == IVTV_ENC_STREAM_TYPE_MPG) {
			/* Process pending program info updates and pending VBI data */
			ivtv_update_pgm_info(itv);

			if (time_after(jiffies,
				       itv->dualwatch_jiffies +
				       msecs_to_jiffies(1000))) {
				itv->dualwatch_jiffies = jiffies;
				ivtv_dualwatch(itv);
			}

			if (test_bit(IVTV_F_S_INTERNAL_USE, &s_vbi->s_flags) &&
			    !test_bit(IVTV_F_S_APPL_IO, &s_vbi->s_flags)) {
				while ((buf = ivtv_dequeue(s_vbi, &s_vbi->q_full))) {
					/* byteswap and process VBI data */
					ivtv_process_vbi_data(itv, buf, s_vbi->dma_pts, s_vbi->type);
					ivtv_enqueue(s_vbi, buf, &s_vbi->q_free);
				}
			}
			buf = &itv->vbi.sliced_mpeg_buf;
			if (buf->readpos != buf->bytesused) {
				return buf;
			}
		}

		/* do we have leftover data? */
		buf = ivtv_dequeue(s, &s->q_io);
		if (buf)
			return buf;

		/* do we have new data? */
		buf = ivtv_dequeue(s, &s->q_full);
		if (buf) {
			if ((buf->b_flags & IVTV_F_B_NEED_BUF_SWAP) == 0)
				return buf;
			buf->b_flags &= ~IVTV_F_B_NEED_BUF_SWAP;
			if (s->type == IVTV_ENC_STREAM_TYPE_MPG)
				/* byteswap MPG data */
				ivtv_buf_swap(buf);
			else if (s->type != IVTV_DEC_STREAM_TYPE_VBI) {
				/* byteswap and process VBI data */
				ivtv_process_vbi_data(itv, buf, s->dma_pts, s->type);
			}
			return buf;
		}

		/* return if end of stream */
		if (s->type != IVTV_DEC_STREAM_TYPE_VBI && !test_bit(IVTV_F_S_STREAMING, &s->s_flags)) {
			IVTV_DEBUG_INFO("EOS %s\n", s->name);
			return NULL;
		}

		/* return if file was opened with O_NONBLOCK */
		if (non_block) {
			*err = -EAGAIN;
			return NULL;
		}

		/* wait for more data to arrive */
		mutex_unlock(&itv->serialize_lock);
		prepare_to_wait(&s->waitq, &wait, TASK_INTERRUPTIBLE);
		/* New buffers might have become available before we were added to the waitqueue */
		if (!s->q_full.buffers)
			schedule();
		finish_wait(&s->waitq, &wait);
		mutex_lock(&itv->serialize_lock);
		if (signal_pending(current)) {
			/* return if a signal was received */
			IVTV_DEBUG_INFO("User stopped %s\n", s->name);
			*err = -EINTR;
			return NULL;
		}
	}
}

static void ivtv_setup_sliced_vbi_buf(struct ivtv *itv)
{
	int idx = itv->vbi.inserted_frame % IVTV_VBI_FRAMES;

	itv->vbi.sliced_mpeg_buf.buf = itv->vbi.sliced_mpeg_data[idx];
	itv->vbi.sliced_mpeg_buf.bytesused = itv->vbi.sliced_mpeg_size[idx];
	itv->vbi.sliced_mpeg_buf.readpos = 0;
}

static size_t ivtv_copy_buf_to_user(struct ivtv_stream *s, struct ivtv_buffer *buf,
		char __user *ubuf, size_t ucount)
{
	struct ivtv *itv = s->itv;
	size_t len = buf->bytesused - buf->readpos;

	if (len > ucount) len = ucount;
	if (itv->vbi.insert_mpeg && s->type == IVTV_ENC_STREAM_TYPE_MPG &&
	    !ivtv_raw_vbi(itv) && buf != &itv->vbi.sliced_mpeg_buf) {
		const char *start = buf->buf + buf->readpos;
		const char *p = start + 1;
		const u8 *q;
		u8 ch = itv->search_pack_header ? 0xba : 0xe0;
		int stuffing, i;

		while (start + len > p && (q = memchr(p, 0, start + len - p))) {
			p = q + 1;
			if ((char *)q + 15 >= buf->buf + buf->bytesused ||
			    q[1] != 0 || q[2] != 1 || q[3] != ch) {
				continue;
			}
			if (!itv->search_pack_header) {
				if ((q[6] & 0xc0) != 0x80)
					continue;
				if (((q[7] & 0xc0) == 0x80 && (q[9] & 0xf0) == 0x20) ||
				    ((q[7] & 0xc0) == 0xc0 && (q[9] & 0xf0) == 0x30)) {
					ch = 0xba;
					itv->search_pack_header = 1;
					p = q + 9;
				}
				continue;
			}
			stuffing = q[13] & 7;
			/* all stuffing bytes must be 0xff */
			for (i = 0; i < stuffing; i++)
				if (q[14 + i] != 0xff)
					break;
			if (i == stuffing && (q[4] & 0xc4) == 0x44 && (q[12] & 3) == 3 &&
					q[14 + stuffing] == 0 && q[15 + stuffing] == 0 &&
					q[16 + stuffing] == 1) {
				itv->search_pack_header = 0;
				len = (char *)q - start;
				ivtv_setup_sliced_vbi_buf(itv);
				break;
			}
		}
	}
	if (copy_to_user(ubuf, (u8 *)buf->buf + buf->readpos, len)) {
		IVTV_DEBUG_WARN("copy %zd bytes to user failed for %s\n", len, s->name);
		return -EFAULT;
	}
	/*IVTV_INFO("copied %lld %d %d %d %d %d vbi %d\n", itv->mpg_data_received, len, ucount,
			buf->readpos, buf->bytesused, buf->bytesused - buf->readpos - len,
			buf == &itv->vbi.sliced_mpeg_buf); */
	buf->readpos += len;
	if (s->type == IVTV_ENC_STREAM_TYPE_MPG && buf != &itv->vbi.sliced_mpeg_buf)
		itv->mpg_data_received += len;
	return len;
}

static ssize_t ivtv_read(struct ivtv_stream *s, char __user *ubuf, size_t tot_count, int non_block)
{
	struct ivtv *itv = s->itv;
	size_t tot_written = 0;
	int single_frame = 0;

	if (atomic_read(&itv->capturing) == 0 && s->fh == NULL) {
		/* shouldn't happen */
		IVTV_DEBUG_WARN("Stream %s not initialized before read\n", s->name);
		return -EIO;
	}

	/* Each VBI buffer is one frame, the v4l2 API says that for VBI the frames should
	   arrive one-by-one, so make sure we never output more than one VBI frame at a time */
	if (s->type == IVTV_DEC_STREAM_TYPE_VBI ||
	    (s->type == IVTV_ENC_STREAM_TYPE_VBI && !ivtv_raw_vbi(itv)))
		single_frame = 1;

	for (;;) {
		struct ivtv_buffer *buf;
		int rc;

		buf = ivtv_get_buffer(s, non_block, &rc);
		/* if there is no data available... */
		if (buf == NULL) {
			/* if we got data, then return that regardless */
			if (tot_written)
				break;
			/* EOS condition */
			if (rc == 0) {
				clear_bit(IVTV_F_S_STREAMOFF, &s->s_flags);
				clear_bit(IVTV_F_S_APPL_IO, &s->s_flags);
				ivtv_release_stream(s);
			}
			/* set errno */
			return rc;
		}
		rc = ivtv_copy_buf_to_user(s, buf, ubuf + tot_written, tot_count - tot_written);
		if (buf != &itv->vbi.sliced_mpeg_buf) {
			ivtv_enqueue(s, buf, (buf->readpos == buf->bytesused) ? &s->q_free : &s->q_io);
		}
		else if (buf->readpos == buf->bytesused) {
			int idx = itv->vbi.inserted_frame % IVTV_VBI_FRAMES;
			itv->vbi.sliced_mpeg_size[idx] = 0;
			itv->vbi.inserted_frame++;
			itv->vbi_data_inserted += buf->bytesused;
		}
		if (rc < 0)
			return rc;
		tot_written += rc;

		if (tot_written == tot_count || single_frame)
			break;
	}
	return tot_written;
}

static ssize_t ivtv_read_pos(struct ivtv_stream *s, char __user *ubuf, size_t count,
			loff_t *pos, int non_block)
{
	ssize_t rc = count ? ivtv_read(s, ubuf, count, non_block) : 0;
	struct ivtv *itv = s->itv;

	IVTV_DEBUG_HI_FILE("read %zd from %s, got %zd\n", count, s->name, rc);
	if (rc > 0)
		*pos += rc;
	return rc;
}

int ivtv_start_capture(struct ivtv_open_id *id)
{
	struct ivtv *itv = id->itv;
	struct ivtv_stream *s = &itv->streams[id->type];
	struct ivtv_stream *s_vbi;

	if (s->type == IVTV_ENC_STREAM_TYPE_RAD ||
	    s->type == IVTV_DEC_STREAM_TYPE_MPG ||
	    s->type == IVTV_DEC_STREAM_TYPE_YUV ||
	    s->type == IVTV_DEC_STREAM_TYPE_VOUT) {
		/* you cannot read from these stream types. */
		return -EINVAL;
	}

	/* Try to claim this stream. */
	if (ivtv_claim_stream(id, s->type))
		return -EBUSY;

	/* This stream does not need to start capturing */
	if (s->type == IVTV_DEC_STREAM_TYPE_VBI) {
		set_bit(IVTV_F_S_APPL_IO, &s->s_flags);
		return 0;
	}

	/* If capture is already in progress, then we also have to
	   do nothing extra. */
	if (test_bit(IVTV_F_S_STREAMOFF, &s->s_flags) || test_and_set_bit(IVTV_F_S_STREAMING, &s->s_flags)) {
		set_bit(IVTV_F_S_APPL_IO, &s->s_flags);
		return 0;
	}

	/* Start VBI capture if required */
	s_vbi = &itv->streams[IVTV_ENC_STREAM_TYPE_VBI];
	if (s->type == IVTV_ENC_STREAM_TYPE_MPG &&
	    test_bit(IVTV_F_S_INTERNAL_USE, &s_vbi->s_flags) &&
	    !test_and_set_bit(IVTV_F_S_STREAMING, &s_vbi->s_flags)) {
		/* Note: the IVTV_ENC_STREAM_TYPE_VBI is claimed
		   automatically when the MPG stream is claimed.
		   We only need to start the VBI capturing. */
		if (ivtv_start_v4l2_encode_stream(s_vbi)) {
			IVTV_DEBUG_WARN("VBI capture start failed\n");

			/* Failure, clean up and return an error */
			clear_bit(IVTV_F_S_STREAMING, &s_vbi->s_flags);
			clear_bit(IVTV_F_S_STREAMING, &s->s_flags);
			/* also releases the associated VBI stream */
			ivtv_release_stream(s);
			return -EIO;
		}
		IVTV_DEBUG_INFO("VBI insertion started\n");
	}

	/* Tell the card to start capturing */
	if (!ivtv_start_v4l2_encode_stream(s)) {
		/* We're done */
		set_bit(IVTV_F_S_APPL_IO, &s->s_flags);
		/* Resume a possibly paused encoder */
		if (test_and_clear_bit(IVTV_F_I_ENC_PAUSED, &itv->i_flags))
			ivtv_vapi(itv, CX2341X_ENC_PAUSE_ENCODER, 1, 1);
		return 0;
	}

	/* failure, clean up */
	IVTV_DEBUG_WARN("Failed to start capturing for stream %s\n", s->name);

	/* Note: the IVTV_ENC_STREAM_TYPE_VBI is released
	   automatically when the MPG stream is released.
	   We only need to stop the VBI capturing. */
	if (s->type == IVTV_ENC_STREAM_TYPE_MPG &&
	    test_bit(IVTV_F_S_STREAMING, &s_vbi->s_flags)) {
		ivtv_stop_v4l2_encode_stream(s_vbi, 0);
		clear_bit(IVTV_F_S_STREAMING, &s_vbi->s_flags);
	}
	clear_bit(IVTV_F_S_STREAMING, &s->s_flags);
	ivtv_release_stream(s);
	return -EIO;
}

ssize_t ivtv_v4l2_read(struct file * filp, char __user *buf, size_t count, loff_t * pos)
{
	struct ivtv_open_id *id = fh2id(filp->private_data);
	struct ivtv *itv = id->itv;
	struct ivtv_stream *s = &itv->streams[id->type];
	ssize_t rc;

	IVTV_DEBUG_HI_FILE("read %zd bytes from %s\n", count, s->name);

	if (mutex_lock_interruptible(&itv->serialize_lock))
		return -ERESTARTSYS;
	rc = ivtv_start_capture(id);
	if (!rc)
		rc = ivtv_read_pos(s, buf, count, pos, filp->f_flags & O_NONBLOCK);
	mutex_unlock(&itv->serialize_lock);
	return rc;
}

int ivtv_start_decoding(struct ivtv_open_id *id, int speed)
{
	struct ivtv *itv = id->itv;
	struct ivtv_stream *s = &itv->streams[id->type];
	int rc;

	if (atomic_read(&itv->decoding) == 0) {
		if (ivtv_claim_stream(id, s->type)) {
			/* someone else is using this stream already */
			IVTV_DEBUG_WARN("start decode, stream already claimed\n");
			return -EBUSY;
		}
		rc = ivtv_start_v4l2_decode_stream(s, 0);
		if (rc < 0) {
			if (rc == -EAGAIN)
				rc = ivtv_start_v4l2_decode_stream(s, 0);
			if (rc < 0)
				return rc;
		}
	}
	if (s->type == IVTV_DEC_STREAM_TYPE_MPG)
		return ivtv_set_speed(itv, speed);
	return 0;
}

static ssize_t ivtv_write(struct file *filp, const char __user *user_buf, size_t count, loff_t *pos)
{
	struct ivtv_open_id *id = fh2id(filp->private_data);
	struct ivtv *itv = id->itv;
	struct ivtv_stream *s = &itv->streams[id->type];
	struct yuv_playback_info *yi = &itv->yuv_info;
	struct ivtv_buffer *buf;
	struct ivtv_queue q;
	int bytes_written = 0;
	int mode;
	int rc;
	DEFINE_WAIT(wait);

	IVTV_DEBUG_HI_FILE("write %zd bytes to %s\n", count, s->name);

	if (s->type != IVTV_DEC_STREAM_TYPE_MPG &&
	    s->type != IVTV_DEC_STREAM_TYPE_YUV &&
	    s->type != IVTV_DEC_STREAM_TYPE_VOUT)
		/* not decoder streams */
		return -EINVAL;

	/* Try to claim this stream */
	if (ivtv_claim_stream(id, s->type))
		return -EBUSY;

	/* This stream does not need to start any decoding */
	if (s->type == IVTV_DEC_STREAM_TYPE_VOUT) {
		int elems = count / sizeof(struct v4l2_sliced_vbi_data);

		set_bit(IVTV_F_S_APPL_IO, &s->s_flags);
		return ivtv_write_vbi_from_user(itv,
		   (const struct v4l2_sliced_vbi_data __user *)user_buf, elems);
	}

	mode = s->type == IVTV_DEC_STREAM_TYPE_MPG ? OUT_MPG : OUT_YUV;

	if (ivtv_set_output_mode(itv, mode) != mode) {
	    ivtv_release_stream(s);
	    return -EBUSY;
	}
	ivtv_queue_init(&q);
	set_bit(IVTV_F_S_APPL_IO, &s->s_flags);

	/* Start decoder (returns 0 if already started) */
	rc = ivtv_start_decoding(id, itv->speed);
	if (rc) {
		IVTV_DEBUG_WARN("Failed start decode stream %s\n", s->name);

		/* failure, clean up */
		clear_bit(IVTV_F_S_STREAMING, &s->s_flags);
		clear_bit(IVTV_F_S_APPL_IO, &s->s_flags);
		return rc;
	}

retry:
	/* If possible, just DMA the entire frame - Check the data transfer size
	since we may get here before the stream has been fully set-up */
	if (mode == OUT_YUV && s->q_full.length == 0 && itv->dma_data_req_size) {
		while (count >= itv->dma_data_req_size) {
			rc = ivtv_yuv_udma_stream_frame(itv, (void __user *)user_buf);

			if (rc < 0)
				return rc;

			bytes_written += itv->dma_data_req_size;
			user_buf += itv->dma_data_req_size;
			count -= itv->dma_data_req_size;
		}
		if (count == 0) {
			IVTV_DEBUG_HI_FILE("Wrote %d bytes to %s (%d)\n", bytes_written, s->name, s->q_full.bytesused);
			return bytes_written;
		}
	}

	for (;;) {
		/* Gather buffers */
		while (q.length - q.bytesused < count && (buf = ivtv_dequeue(s, &s->q_io)))
			ivtv_enqueue(s, buf, &q);
		while (q.length - q.bytesused < count && (buf = ivtv_dequeue(s, &s->q_free))) {
			ivtv_enqueue(s, buf, &q);
		}
		if (q.buffers)
			break;
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;
		mutex_unlock(&itv->serialize_lock);
		prepare_to_wait(&s->waitq, &wait, TASK_INTERRUPTIBLE);
		/* New buffers might have become free before we were added to the waitqueue */
		if (!s->q_free.buffers)
			schedule();
		finish_wait(&s->waitq, &wait);
		mutex_lock(&itv->serialize_lock);
		if (signal_pending(current)) {
			IVTV_DEBUG_INFO("User stopped %s\n", s->name);
			return -EINTR;
		}
	}

	/* copy user data into buffers */
	while ((buf = ivtv_dequeue(s, &q))) {
		/* yuv is a pain. Don't copy more data than needed for a single
		   frame, otherwise we lose sync with the incoming stream */
		if (s->type == IVTV_DEC_STREAM_TYPE_YUV &&
		    yi->stream_size + count > itv->dma_data_req_size)
			rc  = ivtv_buf_copy_from_user(s, buf, user_buf,
				itv->dma_data_req_size - yi->stream_size);
		else
			rc = ivtv_buf_copy_from_user(s, buf, user_buf, count);

		/* Make sure we really got all the user data */
		if (rc < 0) {
			ivtv_queue_move(s, &q, NULL, &s->q_free, 0);
			return rc;
		}
		user_buf += rc;
		count -= rc;
		bytes_written += rc;

		if (s->type == IVTV_DEC_STREAM_TYPE_YUV) {
			yi->stream_size += rc;
			/* If we have a complete yuv frame, break loop now */
			if (yi->stream_size == itv->dma_data_req_size) {
				ivtv_enqueue(s, buf, &s->q_full);
				yi->stream_size = 0;
				break;
			}
		}

		if (buf->bytesused != s->buf_size) {
			/* incomplete, leave in q_io for next time */
			ivtv_enqueue(s, buf, &s->q_io);
			break;
		}
		/* Byteswap MPEG buffer */
		if (s->type == IVTV_DEC_STREAM_TYPE_MPG)
			ivtv_buf_swap(buf);
		ivtv_enqueue(s, buf, &s->q_full);
	}

	if (test_bit(IVTV_F_S_NEEDS_DATA, &s->s_flags)) {
		if (s->q_full.length >= itv->dma_data_req_size) {
			int got_sig;

			if (mode == OUT_YUV)
				ivtv_yuv_setup_stream_frame(itv);

			mutex_unlock(&itv->serialize_lock);
			prepare_to_wait(&itv->dma_waitq, &wait, TASK_INTERRUPTIBLE);
			while (!(got_sig = signal_pending(current)) &&
					test_bit(IVTV_F_S_DMA_PENDING, &s->s_flags)) {
				schedule();
			}
			finish_wait(&itv->dma_waitq, &wait);
			mutex_lock(&itv->serialize_lock);
			if (got_sig) {
				IVTV_DEBUG_INFO("User interrupted %s\n", s->name);
				return -EINTR;
			}

			clear_bit(IVTV_F_S_NEEDS_DATA, &s->s_flags);
			ivtv_queue_move(s, &s->q_full, NULL, &s->q_predma, itv->dma_data_req_size);
			ivtv_dma_stream_dec_prepare(s, itv->dma_data_req_offset + IVTV_DECODER_OFFSET, 1);
		}
	}
	/* more user data is available, wait until buffers become free
	   to transfer the rest. */
	if (count && !(filp->f_flags & O_NONBLOCK))
		goto retry;
	IVTV_DEBUG_HI_FILE("Wrote %d bytes to %s (%d)\n", bytes_written, s->name, s->q_full.bytesused);
	return bytes_written;
}

ssize_t ivtv_v4l2_write(struct file *filp, const char __user *user_buf, size_t count, loff_t *pos)
{
	struct ivtv_open_id *id = fh2id(filp->private_data);
	struct ivtv *itv = id->itv;
	ssize_t res;

	if (mutex_lock_interruptible(&itv->serialize_lock))
		return -ERESTARTSYS;
	res = ivtv_write(filp, user_buf, count, pos);
	mutex_unlock(&itv->serialize_lock);
	return res;
}

__poll_t ivtv_v4l2_dec_poll(struct file *filp, poll_table *wait)
{
	struct ivtv_open_id *id = fh2id(filp->private_data);
	struct ivtv *itv = id->itv;
	struct ivtv_stream *s = &itv->streams[id->type];
	__poll_t res = 0;

	/* add stream's waitq to the poll list */
	IVTV_DEBUG_HI_FILE("Decoder poll\n");

	/* If there are subscribed events, then only use the new event
	   API instead of the old video.h based API. */
	if (!list_empty(&id->fh.subscribed)) {
		poll_wait(filp, &id->fh.wait, wait);
		/* Turn off the old-style vsync events */
		clear_bit(IVTV_F_I_EV_VSYNC_ENABLED, &itv->i_flags);
		if (v4l2_event_pending(&id->fh))
			res = EPOLLPRI;
	} else {
		/* This is the old-style API which is here only for backwards
		   compatibility. */
		poll_wait(filp, &s->waitq, wait);
		set_bit(IVTV_F_I_EV_VSYNC_ENABLED, &itv->i_flags);
		if (test_bit(IVTV_F_I_EV_VSYNC, &itv->i_flags) ||
		    test_bit(IVTV_F_I_EV_DEC_STOPPED, &itv->i_flags))
			res = EPOLLPRI;
	}

	/* Allow write if buffers are available for writing */
	if (s->q_free.buffers)
		res |= EPOLLOUT | EPOLLWRNORM;
	return res;
}

__poll_t ivtv_v4l2_enc_poll(struct file *filp, poll_table *wait)
{
	__poll_t req_events = poll_requested_events(wait);
	struct ivtv_open_id *id = fh2id(filp->private_data);
	struct ivtv *itv = id->itv;
	struct ivtv_stream *s = &itv->streams[id->type];
	int eof = test_bit(IVTV_F_S_STREAMOFF, &s->s_flags);
	__poll_t res = 0;

	/* Start a capture if there is none */
	if (!eof && !test_bit(IVTV_F_S_STREAMING, &s->s_flags) &&
			s->type != IVTV_ENC_STREAM_TYPE_RAD &&
			(req_events & (EPOLLIN | EPOLLRDNORM))) {
		int rc;

		mutex_lock(&itv->serialize_lock);
		rc = ivtv_start_capture(id);
		mutex_unlock(&itv->serialize_lock);
		if (rc) {
			IVTV_DEBUG_INFO("Could not start capture for %s (%d)\n",
					s->name, rc);
			return EPOLLERR;
		}
		IVTV_DEBUG_FILE("Encoder poll started capture\n");
	}

	/* add stream's waitq to the poll list */
	IVTV_DEBUG_HI_FILE("Encoder poll\n");
	poll_wait(filp, &s->waitq, wait);
	if (v4l2_event_pending(&id->fh))
		res |= EPOLLPRI;
	else
		poll_wait(filp, &id->fh.wait, wait);

	if (s->q_full.length || s->q_io.length)
		return res | EPOLLIN | EPOLLRDNORM;
	if (eof)
		return res | EPOLLHUP;
	return res;
}

void ivtv_stop_capture(struct ivtv_open_id *id, int gop_end)
{
	struct ivtv *itv = id->itv;
	struct ivtv_stream *s = &itv->streams[id->type];

	IVTV_DEBUG_FILE("close() of %s\n", s->name);

	/* 'Unclaim' this stream */

	/* Stop capturing */
	if (test_bit(IVTV_F_S_STREAMING, &s->s_flags)) {
		struct ivtv_stream *s_vbi = &itv->streams[IVTV_ENC_STREAM_TYPE_VBI];

		IVTV_DEBUG_INFO("close stopping capture\n");
		/* Special case: a running VBI capture for VBI insertion
		   in the mpeg stream. Need to stop that too. */
		if (id->type == IVTV_ENC_STREAM_TYPE_MPG &&
		    test_bit(IVTV_F_S_STREAMING, &s_vbi->s_flags) &&
		    !test_bit(IVTV_F_S_APPL_IO, &s_vbi->s_flags)) {
			IVTV_DEBUG_INFO("close stopping embedded VBI capture\n");
			ivtv_stop_v4l2_encode_stream(s_vbi, 0);
		}
		if ((id->type == IVTV_DEC_STREAM_TYPE_VBI ||
		     id->type == IVTV_ENC_STREAM_TYPE_VBI) &&
		    test_bit(IVTV_F_S_INTERNAL_USE, &s->s_flags)) {
			/* Also used internally, don't stop capturing */
			s->fh = NULL;
		}
		else {
			ivtv_stop_v4l2_encode_stream(s, gop_end);
		}
	}
	if (!gop_end) {
		clear_bit(IVTV_F_S_APPL_IO, &s->s_flags);
		clear_bit(IVTV_F_S_STREAMOFF, &s->s_flags);
		ivtv_release_stream(s);
	}
}

static void ivtv_stop_decoding(struct ivtv_open_id *id, int flags, u64 pts)
{
	struct ivtv *itv = id->itv;
	struct ivtv_stream *s = &itv->streams[id->type];

	IVTV_DEBUG_FILE("close() of %s\n", s->name);

	if (id->type == IVTV_DEC_STREAM_TYPE_YUV &&
		test_bit(IVTV_F_I_DECODING_YUV, &itv->i_flags)) {
		/* Restore registers we've changed & clean up any mess */
		ivtv_yuv_close(itv);
	}

	/* Stop decoding */
	if (test_bit(IVTV_F_S_STREAMING, &s->s_flags)) {
		IVTV_DEBUG_INFO("close stopping decode\n");

		ivtv_stop_v4l2_decode_stream(s, flags, pts);
		itv->output_mode = OUT_NONE;
	}
	clear_bit(IVTV_F_S_APPL_IO, &s->s_flags);
	clear_bit(IVTV_F_S_STREAMOFF, &s->s_flags);

	if (itv->output_mode == OUT_UDMA_YUV && id->yuv_frames)
		itv->output_mode = OUT_NONE;

	itv->speed = 0;
	clear_bit(IVTV_F_I_DEC_PAUSED, &itv->i_flags);
	ivtv_release_stream(s);
}

int ivtv_v4l2_close(struct file *filp)
{
	struct v4l2_fh *fh = filp->private_data;
	struct ivtv_open_id *id = fh2id(fh);
	struct ivtv *itv = id->itv;
	struct ivtv_stream *s = &itv->streams[id->type];

	IVTV_DEBUG_FILE("close %s\n", s->name);

	mutex_lock(&itv->serialize_lock);

	/* Stop radio */
	if (id->type == IVTV_ENC_STREAM_TYPE_RAD &&
			v4l2_fh_is_singular_file(filp)) {
		/* Closing radio device, return to TV mode */
		ivtv_mute(itv);
		/* Mark that the radio is no longer in use */
		clear_bit(IVTV_F_I_RADIO_USER, &itv->i_flags);
		/* Switch tuner to TV */
		ivtv_call_all(itv, video, s_std, itv->std);
		/* Select correct audio input (i.e. TV tuner or Line in) */
		ivtv_audio_set_io(itv);
		if (itv->hw_flags & IVTV_HW_SAA711X) {
			ivtv_call_hw(itv, IVTV_HW_SAA711X, video, s_crystal_freq,
					SAA7115_FREQ_32_11_MHZ, 0);
		}
		if (atomic_read(&itv->capturing) > 0) {
			/* Undo video mute */
			ivtv_vapi(itv, CX2341X_ENC_MUTE_VIDEO, 1,
					v4l2_ctrl_g_ctrl(itv->cxhdl.video_mute) |
					(v4l2_ctrl_g_ctrl(itv->cxhdl.video_mute_yuv) << 8));
		}
		/* Done! Unmute and continue. */
		ivtv_unmute(itv);
	}

	v4l2_fh_del(fh);
	v4l2_fh_exit(fh);

	/* Easy case first: this stream was never claimed by us */
	if (s->fh != &id->fh)
		goto close_done;

	/* 'Unclaim' this stream */

	if (s->type >= IVTV_DEC_STREAM_TYPE_MPG) {
		struct ivtv_stream *s_vout = &itv->streams[IVTV_DEC_STREAM_TYPE_VOUT];

		ivtv_stop_decoding(id, V4L2_DEC_CMD_STOP_TO_BLACK | V4L2_DEC_CMD_STOP_IMMEDIATELY, 0);

		/* If all output streams are closed, and if the user doesn't have
		   IVTV_DEC_STREAM_TYPE_VOUT open, then disable CC on TV-out. */
		if (itv->output_mode == OUT_NONE && !test_bit(IVTV_F_S_APPL_IO, &s_vout->s_flags)) {
			/* disable CC on TV-out */
			ivtv_disable_cc(itv);
		}
	} else {
		ivtv_stop_capture(id, 0);
	}
close_done:
	kfree(id);
	mutex_unlock(&itv->serialize_lock);
	return 0;
}

static int ivtv_open(struct file *filp)
{
	struct video_device *vdev = video_devdata(filp);
	struct ivtv_stream *s = video_get_drvdata(vdev);
	struct ivtv *itv = s->itv;
	struct ivtv_open_id *item;
	int res = 0;

	IVTV_DEBUG_FILE("open %s\n", s->name);

	if (ivtv_init_on_first_open(itv)) {
		IVTV_ERR("Failed to initialize on device %s\n",
			 video_device_node_name(vdev));
		return -ENXIO;
	}

#ifdef CONFIG_VIDEO_ADV_DEBUG
	/* Unless ivtv_fw_debug is set, error out if firmware dead. */
	if (ivtv_fw_debug) {
		IVTV_WARN("Opening %s with dead firmware lockout disabled\n",
			  video_device_node_name(vdev));
		IVTV_WARN("Selected firmware errors will be ignored\n");
	} else {
#else
	if (1) {
#endif
		res = ivtv_firmware_check(itv, "ivtv_serialized_open");
		if (res == -EAGAIN)
			res = ivtv_firmware_check(itv, "ivtv_serialized_open");
		if (res < 0)
			return -EIO;
	}

	if (s->type == IVTV_DEC_STREAM_TYPE_MPG &&
		test_bit(IVTV_F_S_CLAIMED, &itv->streams[IVTV_DEC_STREAM_TYPE_YUV].s_flags))
		return -EBUSY;

	if (s->type == IVTV_DEC_STREAM_TYPE_YUV &&
		test_bit(IVTV_F_S_CLAIMED, &itv->streams[IVTV_DEC_STREAM_TYPE_MPG].s_flags))
		return -EBUSY;

	if (s->type == IVTV_DEC_STREAM_TYPE_YUV) {
		if (read_reg(0x82c) == 0) {
			IVTV_ERR("Tried to open YUV output device but need to send data to mpeg decoder before it can be used\n");
			/* return -ENODEV; */
		}
		ivtv_udma_alloc(itv);
	}

	/* Allocate memory */
	item = kzalloc(sizeof(struct ivtv_open_id), GFP_KERNEL);
	if (NULL == item) {
		IVTV_DEBUG_WARN("nomem on v4l2 open\n");
		return -ENOMEM;
	}
	v4l2_fh_init(&item->fh, &s->vdev);
	item->itv = itv;
	item->type = s->type;

	filp->private_data = &item->fh;
	v4l2_fh_add(&item->fh);

	if (item->type == IVTV_ENC_STREAM_TYPE_RAD &&
			v4l2_fh_is_singular_file(filp)) {
		if (!test_bit(IVTV_F_I_RADIO_USER, &itv->i_flags)) {
			if (atomic_read(&itv->capturing) > 0) {
				/* switching to radio while capture is
				   in progress is not polite */
				v4l2_fh_del(&item->fh);
				v4l2_fh_exit(&item->fh);
				kfree(item);
				return -EBUSY;
			}
		}
		/* Mark that the radio is being used. */
		set_bit(IVTV_F_I_RADIO_USER, &itv->i_flags);
		/* We have the radio */
		ivtv_mute(itv);
		/* Switch tuner to radio */
		ivtv_call_all(itv, tuner, s_radio);
		/* Select the correct audio input (i.e. radio tuner) */
		ivtv_audio_set_io(itv);
		if (itv->hw_flags & IVTV_HW_SAA711X) {
			ivtv_call_hw(itv, IVTV_HW_SAA711X, video, s_crystal_freq,
				SAA7115_FREQ_32_11_MHZ, SAA7115_FREQ_FL_APLL);
		}
		/* Done! Unmute and continue. */
		ivtv_unmute(itv);
	}

	/* YUV or MPG Decoding Mode? */
	if (s->type == IVTV_DEC_STREAM_TYPE_MPG) {
		clear_bit(IVTV_F_I_DEC_YUV, &itv->i_flags);
	} else if (s->type == IVTV_DEC_STREAM_TYPE_YUV) {
		set_bit(IVTV_F_I_DEC_YUV, &itv->i_flags);
		/* For yuv, we need to know the dma size before we start */
		itv->dma_data_req_size =
				1080 * ((itv->yuv_info.v4l2_src_h + 31) & ~31);
		itv->yuv_info.stream_size = 0;
	}
	return 0;
}

int ivtv_v4l2_open(struct file *filp)
{
	struct video_device *vdev = video_devdata(filp);
	int res;

	if (mutex_lock_interruptible(vdev->lock))
		return -ERESTARTSYS;
	res = ivtv_open(filp);
	mutex_unlock(vdev->lock);
	return res;
}

void ivtv_mute(struct ivtv *itv)
{
	if (atomic_read(&itv->capturing))
		ivtv_vapi(itv, CX2341X_ENC_MUTE_AUDIO, 1, 1);
	IVTV_DEBUG_INFO("Mute\n");
}

void ivtv_unmute(struct ivtv *itv)
{
	if (atomic_read(&itv->capturing)) {
		ivtv_msleep_timeout(100, 0);
		ivtv_vapi(itv, CX2341X_ENC_MISC, 1, 12);
		ivtv_vapi(itv, CX2341X_ENC_MUTE_AUDIO, 1, 0);
	}
	IVTV_DEBUG_INFO("Unmute\n");
}
