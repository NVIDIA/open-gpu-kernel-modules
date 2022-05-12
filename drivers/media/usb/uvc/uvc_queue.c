// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *      uvc_queue.c  --  USB Video Class driver - Buffers management
 *
 *      Copyright (C) 2005-2010
 *          Laurent Pinchart (laurent.pinchart@ideasonboard.com)
 */

#include <linux/atomic.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/videodev2.h>
#include <linux/vmalloc.h>
#include <linux/wait.h>
#include <media/videobuf2-v4l2.h>
#include <media/videobuf2-vmalloc.h>

#include "uvcvideo.h"

/* ------------------------------------------------------------------------
 * Video buffers queue management.
 *
 * Video queues is initialized by uvc_queue_init(). The function performs
 * basic initialization of the uvc_video_queue struct and never fails.
 *
 * Video buffers are managed by videobuf2. The driver uses a mutex to protect
 * the videobuf2 queue operations by serializing calls to videobuf2 and a
 * spinlock to protect the IRQ queue that holds the buffers to be processed by
 * the driver.
 */

static inline struct uvc_buffer *uvc_vbuf_to_buffer(struct vb2_v4l2_buffer *buf)
{
	return container_of(buf, struct uvc_buffer, buf);
}

/*
 * Return all queued buffers to videobuf2 in the requested state.
 *
 * This function must be called with the queue spinlock held.
 */
static void uvc_queue_return_buffers(struct uvc_video_queue *queue,
			       enum uvc_buffer_state state)
{
	enum vb2_buffer_state vb2_state = state == UVC_BUF_STATE_ERROR
					? VB2_BUF_STATE_ERROR
					: VB2_BUF_STATE_QUEUED;

	while (!list_empty(&queue->irqqueue)) {
		struct uvc_buffer *buf = list_first_entry(&queue->irqqueue,
							  struct uvc_buffer,
							  queue);
		list_del(&buf->queue);
		buf->state = state;
		vb2_buffer_done(&buf->buf.vb2_buf, vb2_state);
	}
}

/* -----------------------------------------------------------------------------
 * videobuf2 queue operations
 */

static int uvc_queue_setup(struct vb2_queue *vq,
			   unsigned int *nbuffers, unsigned int *nplanes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct uvc_video_queue *queue = vb2_get_drv_priv(vq);
	struct uvc_streaming *stream;
	unsigned int size;

	switch (vq->type) {
	case V4L2_BUF_TYPE_META_CAPTURE:
		size = UVC_METADATA_BUF_SIZE;
		break;

	default:
		stream = uvc_queue_to_stream(queue);
		size = stream->ctrl.dwMaxVideoFrameSize;
		break;
	}

	/*
	 * When called with plane sizes, validate them. The driver supports
	 * single planar formats only, and requires buffers to be large enough
	 * to store a complete frame.
	 */
	if (*nplanes)
		return *nplanes != 1 || sizes[0] < size ? -EINVAL : 0;

	*nplanes = 1;
	sizes[0] = size;
	return 0;
}

static int uvc_buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct uvc_video_queue *queue = vb2_get_drv_priv(vb->vb2_queue);
	struct uvc_buffer *buf = uvc_vbuf_to_buffer(vbuf);

	if (vb->type == V4L2_BUF_TYPE_VIDEO_OUTPUT &&
	    vb2_get_plane_payload(vb, 0) > vb2_plane_size(vb, 0)) {
		uvc_dbg(uvc_queue_to_stream(queue)->dev, CAPTURE,
			"[E] Bytes used out of bounds\n");
		return -EINVAL;
	}

	if (unlikely(queue->flags & UVC_QUEUE_DISCONNECTED))
		return -ENODEV;

	buf->state = UVC_BUF_STATE_QUEUED;
	buf->error = 0;
	buf->mem = vb2_plane_vaddr(vb, 0);
	buf->length = vb2_plane_size(vb, 0);
	if (vb->type != V4L2_BUF_TYPE_VIDEO_OUTPUT)
		buf->bytesused = 0;
	else
		buf->bytesused = vb2_get_plane_payload(vb, 0);

	return 0;
}

static void uvc_buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct uvc_video_queue *queue = vb2_get_drv_priv(vb->vb2_queue);
	struct uvc_buffer *buf = uvc_vbuf_to_buffer(vbuf);
	unsigned long flags;

	spin_lock_irqsave(&queue->irqlock, flags);
	if (likely(!(queue->flags & UVC_QUEUE_DISCONNECTED))) {
		kref_init(&buf->ref);
		list_add_tail(&buf->queue, &queue->irqqueue);
	} else {
		/* If the device is disconnected return the buffer to userspace
		 * directly. The next QBUF call will fail with -ENODEV.
		 */
		buf->state = UVC_BUF_STATE_ERROR;
		vb2_buffer_done(vb, VB2_BUF_STATE_ERROR);
	}

	spin_unlock_irqrestore(&queue->irqlock, flags);
}

static void uvc_buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct uvc_video_queue *queue = vb2_get_drv_priv(vb->vb2_queue);
	struct uvc_streaming *stream = uvc_queue_to_stream(queue);
	struct uvc_buffer *buf = uvc_vbuf_to_buffer(vbuf);

	if (vb->state == VB2_BUF_STATE_DONE)
		uvc_video_clock_update(stream, vbuf, buf);
}

static int uvc_start_streaming(struct vb2_queue *vq, unsigned int count)
{
	struct uvc_video_queue *queue = vb2_get_drv_priv(vq);
	struct uvc_streaming *stream = uvc_queue_to_stream(queue);
	int ret;

	lockdep_assert_irqs_enabled();

	queue->buf_used = 0;

	ret = uvc_video_start_streaming(stream);
	if (ret == 0)
		return 0;

	spin_lock_irq(&queue->irqlock);
	uvc_queue_return_buffers(queue, UVC_BUF_STATE_QUEUED);
	spin_unlock_irq(&queue->irqlock);

	return ret;
}

static void uvc_stop_streaming(struct vb2_queue *vq)
{
	struct uvc_video_queue *queue = vb2_get_drv_priv(vq);

	lockdep_assert_irqs_enabled();

	if (vq->type != V4L2_BUF_TYPE_META_CAPTURE)
		uvc_video_stop_streaming(uvc_queue_to_stream(queue));

	spin_lock_irq(&queue->irqlock);
	uvc_queue_return_buffers(queue, UVC_BUF_STATE_ERROR);
	spin_unlock_irq(&queue->irqlock);
}

static const struct vb2_ops uvc_queue_qops = {
	.queue_setup = uvc_queue_setup,
	.buf_prepare = uvc_buffer_prepare,
	.buf_queue = uvc_buffer_queue,
	.buf_finish = uvc_buffer_finish,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = uvc_start_streaming,
	.stop_streaming = uvc_stop_streaming,
};

static const struct vb2_ops uvc_meta_queue_qops = {
	.queue_setup = uvc_queue_setup,
	.buf_prepare = uvc_buffer_prepare,
	.buf_queue = uvc_buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.stop_streaming = uvc_stop_streaming,
};

int uvc_queue_init(struct uvc_video_queue *queue, enum v4l2_buf_type type,
		    int drop_corrupted)
{
	int ret;

	queue->queue.type = type;
	queue->queue.io_modes = VB2_MMAP | VB2_USERPTR;
	queue->queue.drv_priv = queue;
	queue->queue.buf_struct_size = sizeof(struct uvc_buffer);
	queue->queue.mem_ops = &vb2_vmalloc_memops;
	queue->queue.timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC
		| V4L2_BUF_FLAG_TSTAMP_SRC_SOE;
	queue->queue.lock = &queue->mutex;

	switch (type) {
	case V4L2_BUF_TYPE_META_CAPTURE:
		queue->queue.ops = &uvc_meta_queue_qops;
		break;
	default:
		queue->queue.io_modes |= VB2_DMABUF;
		queue->queue.ops = &uvc_queue_qops;
		break;
	}

	ret = vb2_queue_init(&queue->queue);
	if (ret)
		return ret;

	mutex_init(&queue->mutex);
	spin_lock_init(&queue->irqlock);
	INIT_LIST_HEAD(&queue->irqqueue);
	queue->flags = drop_corrupted ? UVC_QUEUE_DROP_CORRUPTED : 0;

	return 0;
}

void uvc_queue_release(struct uvc_video_queue *queue)
{
	mutex_lock(&queue->mutex);
	vb2_queue_release(&queue->queue);
	mutex_unlock(&queue->mutex);
}

/* -----------------------------------------------------------------------------
 * V4L2 queue operations
 */

int uvc_request_buffers(struct uvc_video_queue *queue,
			struct v4l2_requestbuffers *rb)
{
	int ret;

	mutex_lock(&queue->mutex);
	ret = vb2_reqbufs(&queue->queue, rb);
	mutex_unlock(&queue->mutex);

	return ret ? ret : rb->count;
}

int uvc_query_buffer(struct uvc_video_queue *queue, struct v4l2_buffer *buf)
{
	int ret;

	mutex_lock(&queue->mutex);
	ret = vb2_querybuf(&queue->queue, buf);
	mutex_unlock(&queue->mutex);

	return ret;
}

int uvc_create_buffers(struct uvc_video_queue *queue,
		       struct v4l2_create_buffers *cb)
{
	int ret;

	mutex_lock(&queue->mutex);
	ret = vb2_create_bufs(&queue->queue, cb);
	mutex_unlock(&queue->mutex);

	return ret;
}

int uvc_queue_buffer(struct uvc_video_queue *queue,
		     struct media_device *mdev, struct v4l2_buffer *buf)
{
	int ret;

	mutex_lock(&queue->mutex);
	ret = vb2_qbuf(&queue->queue, mdev, buf);
	mutex_unlock(&queue->mutex);

	return ret;
}

int uvc_export_buffer(struct uvc_video_queue *queue,
		      struct v4l2_exportbuffer *exp)
{
	int ret;

	mutex_lock(&queue->mutex);
	ret = vb2_expbuf(&queue->queue, exp);
	mutex_unlock(&queue->mutex);

	return ret;
}

int uvc_dequeue_buffer(struct uvc_video_queue *queue, struct v4l2_buffer *buf,
		       int nonblocking)
{
	int ret;

	mutex_lock(&queue->mutex);
	ret = vb2_dqbuf(&queue->queue, buf, nonblocking);
	mutex_unlock(&queue->mutex);

	return ret;
}

int uvc_queue_streamon(struct uvc_video_queue *queue, enum v4l2_buf_type type)
{
	int ret;

	mutex_lock(&queue->mutex);
	ret = vb2_streamon(&queue->queue, type);
	mutex_unlock(&queue->mutex);

	return ret;
}

int uvc_queue_streamoff(struct uvc_video_queue *queue, enum v4l2_buf_type type)
{
	int ret;

	mutex_lock(&queue->mutex);
	ret = vb2_streamoff(&queue->queue, type);
	mutex_unlock(&queue->mutex);

	return ret;
}

int uvc_queue_mmap(struct uvc_video_queue *queue, struct vm_area_struct *vma)
{
	return vb2_mmap(&queue->queue, vma);
}

#ifndef CONFIG_MMU
unsigned long uvc_queue_get_unmapped_area(struct uvc_video_queue *queue,
		unsigned long pgoff)
{
	return vb2_get_unmapped_area(&queue->queue, 0, 0, pgoff, 0);
}
#endif

__poll_t uvc_queue_poll(struct uvc_video_queue *queue, struct file *file,
			    poll_table *wait)
{
	__poll_t ret;

	mutex_lock(&queue->mutex);
	ret = vb2_poll(&queue->queue, file, wait);
	mutex_unlock(&queue->mutex);

	return ret;
}

/* -----------------------------------------------------------------------------
 *
 */

/*
 * Check if buffers have been allocated.
 */
int uvc_queue_allocated(struct uvc_video_queue *queue)
{
	int allocated;

	mutex_lock(&queue->mutex);
	allocated = vb2_is_busy(&queue->queue);
	mutex_unlock(&queue->mutex);

	return allocated;
}

/*
 * Cancel the video buffers queue.
 *
 * Cancelling the queue marks all buffers on the irq queue as erroneous,
 * wakes them up and removes them from the queue.
 *
 * If the disconnect parameter is set, further calls to uvc_queue_buffer will
 * fail with -ENODEV.
 *
 * This function acquires the irq spinlock and can be called from interrupt
 * context.
 */
void uvc_queue_cancel(struct uvc_video_queue *queue, int disconnect)
{
	unsigned long flags;

	spin_lock_irqsave(&queue->irqlock, flags);
	uvc_queue_return_buffers(queue, UVC_BUF_STATE_ERROR);
	/* This must be protected by the irqlock spinlock to avoid race
	 * conditions between uvc_buffer_queue and the disconnection event that
	 * could result in an interruptible wait in uvc_dequeue_buffer. Do not
	 * blindly replace this logic by checking for the UVC_QUEUE_DISCONNECTED
	 * state outside the queue code.
	 */
	if (disconnect)
		queue->flags |= UVC_QUEUE_DISCONNECTED;
	spin_unlock_irqrestore(&queue->irqlock, flags);
}

/*
 * uvc_queue_get_current_buffer: Obtain the current working output buffer
 *
 * Buffers may span multiple packets, and even URBs, therefore the active buffer
 * remains on the queue until the EOF marker.
 */
static struct uvc_buffer *
__uvc_queue_get_current_buffer(struct uvc_video_queue *queue)
{
	if (list_empty(&queue->irqqueue))
		return NULL;

	return list_first_entry(&queue->irqqueue, struct uvc_buffer, queue);
}

struct uvc_buffer *uvc_queue_get_current_buffer(struct uvc_video_queue *queue)
{
	struct uvc_buffer *nextbuf;
	unsigned long flags;

	spin_lock_irqsave(&queue->irqlock, flags);
	nextbuf = __uvc_queue_get_current_buffer(queue);
	spin_unlock_irqrestore(&queue->irqlock, flags);

	return nextbuf;
}

/*
 * uvc_queue_buffer_requeue: Requeue a buffer on our internal irqqueue
 *
 * Reuse a buffer through our internal queue without the need to 'prepare'.
 * The buffer will be returned to userspace through the uvc_buffer_queue call if
 * the device has been disconnected.
 */
static void uvc_queue_buffer_requeue(struct uvc_video_queue *queue,
		struct uvc_buffer *buf)
{
	buf->error = 0;
	buf->state = UVC_BUF_STATE_QUEUED;
	buf->bytesused = 0;
	vb2_set_plane_payload(&buf->buf.vb2_buf, 0, 0);

	uvc_buffer_queue(&buf->buf.vb2_buf);
}

static void uvc_queue_buffer_complete(struct kref *ref)
{
	struct uvc_buffer *buf = container_of(ref, struct uvc_buffer, ref);
	struct vb2_buffer *vb = &buf->buf.vb2_buf;
	struct uvc_video_queue *queue = vb2_get_drv_priv(vb->vb2_queue);

	if ((queue->flags & UVC_QUEUE_DROP_CORRUPTED) && buf->error) {
		uvc_queue_buffer_requeue(queue, buf);
		return;
	}

	buf->state = buf->error ? UVC_BUF_STATE_ERROR : UVC_BUF_STATE_DONE;
	vb2_set_plane_payload(&buf->buf.vb2_buf, 0, buf->bytesused);
	vb2_buffer_done(&buf->buf.vb2_buf, VB2_BUF_STATE_DONE);
}

/*
 * Release a reference on the buffer. Complete the buffer when the last
 * reference is released.
 */
void uvc_queue_buffer_release(struct uvc_buffer *buf)
{
	kref_put(&buf->ref, uvc_queue_buffer_complete);
}

/*
 * Remove this buffer from the queue. Lifetime will persist while async actions
 * are still running (if any), and uvc_queue_buffer_release will give the buffer
 * back to VB2 when all users have completed.
 */
struct uvc_buffer *uvc_queue_next_buffer(struct uvc_video_queue *queue,
		struct uvc_buffer *buf)
{
	struct uvc_buffer *nextbuf;
	unsigned long flags;

	spin_lock_irqsave(&queue->irqlock, flags);
	list_del(&buf->queue);
	nextbuf = __uvc_queue_get_current_buffer(queue);
	spin_unlock_irqrestore(&queue->irqlock, flags);

	uvc_queue_buffer_release(buf);

	return nextbuf;
}
