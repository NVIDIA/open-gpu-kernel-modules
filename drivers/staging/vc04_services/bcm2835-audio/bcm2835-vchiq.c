// SPDX-License-Identifier: GPL-2.0
/* Copyright 2011 Broadcom Corporation.  All rights reserved. */

#include <linux/slab.h>
#include <linux/module.h>
#include <linux/completion.h>
#include "bcm2835.h"
#include "vc_vchi_audioserv_defs.h"

struct bcm2835_audio_instance {
	struct device *dev;
	unsigned int service_handle;
	struct completion msg_avail_comp;
	struct mutex vchi_mutex;
	struct bcm2835_alsa_stream *alsa_stream;
	int result;
	unsigned int max_packet;
	short peer_version;
};

static bool force_bulk;
module_param(force_bulk, bool, 0444);
MODULE_PARM_DESC(force_bulk, "Force use of vchiq bulk for audio");

static void bcm2835_audio_lock(struct bcm2835_audio_instance *instance)
{
	mutex_lock(&instance->vchi_mutex);
	vchiq_use_service(instance->service_handle);
}

static void bcm2835_audio_unlock(struct bcm2835_audio_instance *instance)
{
	vchiq_release_service(instance->service_handle);
	mutex_unlock(&instance->vchi_mutex);
}

static int bcm2835_audio_send_msg_locked(struct bcm2835_audio_instance *instance,
					 struct vc_audio_msg *m, bool wait)
{
	int status;

	if (wait) {
		instance->result = -1;
		init_completion(&instance->msg_avail_comp);
	}

	status = vchiq_queue_kernel_message(instance->service_handle,
					    m, sizeof(*m));
	if (status) {
		dev_err(instance->dev,
			"vchi message queue failed: %d, msg=%d\n",
			status, m->type);
		return -EIO;
	}

	if (wait) {
		if (!wait_for_completion_timeout(&instance->msg_avail_comp,
						 msecs_to_jiffies(10 * 1000))) {
			dev_err(instance->dev,
				"vchi message timeout, msg=%d\n", m->type);
			return -ETIMEDOUT;
		} else if (instance->result) {
			dev_err(instance->dev,
				"vchi message response error:%d, msg=%d\n",
				instance->result, m->type);
			return -EIO;
		}
	}

	return 0;
}

static int bcm2835_audio_send_msg(struct bcm2835_audio_instance *instance,
				  struct vc_audio_msg *m, bool wait)
{
	int err;

	bcm2835_audio_lock(instance);
	err = bcm2835_audio_send_msg_locked(instance, m, wait);
	bcm2835_audio_unlock(instance);
	return err;
}

static int bcm2835_audio_send_simple(struct bcm2835_audio_instance *instance,
				     int type, bool wait)
{
	struct vc_audio_msg m = { .type = type };

	return bcm2835_audio_send_msg(instance, &m, wait);
}

static enum vchiq_status audio_vchi_callback(enum vchiq_reason reason,
					     struct vchiq_header *header,
					     unsigned int handle, void *userdata)
{
	struct bcm2835_audio_instance *instance = vchiq_get_service_userdata(handle);
	struct vc_audio_msg *m;

	if (reason != VCHIQ_MESSAGE_AVAILABLE)
		return VCHIQ_SUCCESS;

	m = (void *)header->data;
	if (m->type == VC_AUDIO_MSG_TYPE_RESULT) {
		instance->result = m->result.success;
		complete(&instance->msg_avail_comp);
	} else if (m->type == VC_AUDIO_MSG_TYPE_COMPLETE) {
		if (m->complete.cookie1 != VC_AUDIO_WRITE_COOKIE1 ||
		    m->complete.cookie2 != VC_AUDIO_WRITE_COOKIE2)
			dev_err(instance->dev, "invalid cookie\n");
		else
			bcm2835_playback_fifo(instance->alsa_stream,
					      m->complete.count);
	} else {
		dev_err(instance->dev, "unexpected callback type=%d\n", m->type);
	}

	vchiq_release_message(handle, header);
	return VCHIQ_SUCCESS;
}

static int
vc_vchi_audio_init(struct vchiq_instance *vchiq_instance,
		   struct bcm2835_audio_instance *instance)
{
	struct vchiq_service_params_kernel params = {
		.version		= VC_AUDIOSERV_VER,
		.version_min		= VC_AUDIOSERV_MIN_VER,
		.fourcc			= VCHIQ_MAKE_FOURCC('A', 'U', 'D', 'S'),
		.callback		= audio_vchi_callback,
		.userdata		= instance,
	};
	int status;

	/* Open the VCHI service connections */
	status = vchiq_open_service(vchiq_instance, &params,
				    &instance->service_handle);

	if (status) {
		dev_err(instance->dev,
			"failed to open VCHI service connection (status=%d)\n",
			status);
		return -EPERM;
	}

	/* Finished with the service for now */
	vchiq_release_service(instance->service_handle);

	return 0;
}

static void vc_vchi_audio_deinit(struct bcm2835_audio_instance *instance)
{
	int status;

	mutex_lock(&instance->vchi_mutex);
	vchiq_use_service(instance->service_handle);

	/* Close all VCHI service connections */
	status = vchiq_close_service(instance->service_handle);
	if (status) {
		dev_err(instance->dev,
			"failed to close VCHI service connection (status=%d)\n",
			status);
	}

	mutex_unlock(&instance->vchi_mutex);
}

int bcm2835_new_vchi_ctx(struct device *dev, struct bcm2835_vchi_ctx *vchi_ctx)
{
	int ret;

	/* Initialize and create a VCHI connection */
	ret = vchiq_initialise(&vchi_ctx->instance);
	if (ret) {
		dev_err(dev, "failed to initialise VCHI instance (ret=%d)\n",
			ret);
		return -EIO;
	}

	ret = vchiq_connect(vchi_ctx->instance);
	if (ret) {
		dev_dbg(dev, "failed to connect VCHI instance (ret=%d)\n",
			ret);

		kfree(vchi_ctx->instance);
		vchi_ctx->instance = NULL;

		return -EIO;
	}

	return 0;
}

void bcm2835_free_vchi_ctx(struct bcm2835_vchi_ctx *vchi_ctx)
{
	/* Close the VCHI connection - it will also free vchi_ctx->instance */
	WARN_ON(vchiq_shutdown(vchi_ctx->instance));

	vchi_ctx->instance = NULL;
}

int bcm2835_audio_open(struct bcm2835_alsa_stream *alsa_stream)
{
	struct bcm2835_vchi_ctx *vchi_ctx = alsa_stream->chip->vchi_ctx;
	struct bcm2835_audio_instance *instance;
	int err;

	/* Allocate memory for this instance */
	instance = kzalloc(sizeof(*instance), GFP_KERNEL);
	if (!instance)
		return -ENOMEM;
	mutex_init(&instance->vchi_mutex);
	instance->dev = alsa_stream->chip->dev;
	instance->alsa_stream = alsa_stream;
	alsa_stream->instance = instance;

	err = vc_vchi_audio_init(vchi_ctx->instance,
				 instance);
	if (err < 0)
		goto free_instance;

	err = bcm2835_audio_send_simple(instance, VC_AUDIO_MSG_TYPE_OPEN,
					false);
	if (err < 0)
		goto deinit;

	bcm2835_audio_lock(instance);
	vchiq_get_peer_version(instance->service_handle,
			       &instance->peer_version);
	bcm2835_audio_unlock(instance);
	if (instance->peer_version < 2 || force_bulk)
		instance->max_packet = 0; /* bulk transfer */
	else
		instance->max_packet = 4000;

	return 0;

 deinit:
	vc_vchi_audio_deinit(instance);
 free_instance:
	alsa_stream->instance = NULL;
	kfree(instance);
	return err;
}

int bcm2835_audio_set_ctls(struct bcm2835_alsa_stream *alsa_stream)
{
	struct bcm2835_chip *chip = alsa_stream->chip;
	struct vc_audio_msg m = {};

	m.type = VC_AUDIO_MSG_TYPE_CONTROL;
	m.control.dest = chip->dest;
	if (!chip->mute)
		m.control.volume = CHIP_MIN_VOLUME;
	else
		m.control.volume = alsa2chip(chip->volume);

	return bcm2835_audio_send_msg(alsa_stream->instance, &m, true);
}

int bcm2835_audio_set_params(struct bcm2835_alsa_stream *alsa_stream,
			     unsigned int channels, unsigned int samplerate,
			     unsigned int bps)
{
	struct vc_audio_msg m = {
		 .type = VC_AUDIO_MSG_TYPE_CONFIG,
		 .config.channels = channels,
		 .config.samplerate = samplerate,
		 .config.bps = bps,
	};
	int err;

	/* resend ctls - alsa_stream may not have been open when first send */
	err = bcm2835_audio_set_ctls(alsa_stream);
	if (err)
		return err;

	return bcm2835_audio_send_msg(alsa_stream->instance, &m, true);
}

int bcm2835_audio_start(struct bcm2835_alsa_stream *alsa_stream)
{
	return bcm2835_audio_send_simple(alsa_stream->instance,
					 VC_AUDIO_MSG_TYPE_START, false);
}

int bcm2835_audio_stop(struct bcm2835_alsa_stream *alsa_stream)
{
	return bcm2835_audio_send_simple(alsa_stream->instance,
					 VC_AUDIO_MSG_TYPE_STOP, false);
}

/* FIXME: this doesn't seem working as expected for "draining" */
int bcm2835_audio_drain(struct bcm2835_alsa_stream *alsa_stream)
{
	struct vc_audio_msg m = {
		.type = VC_AUDIO_MSG_TYPE_STOP,
		.stop.draining = 1,
	};

	return bcm2835_audio_send_msg(alsa_stream->instance, &m, false);
}

int bcm2835_audio_close(struct bcm2835_alsa_stream *alsa_stream)
{
	struct bcm2835_audio_instance *instance = alsa_stream->instance;
	int err;

	err = bcm2835_audio_send_simple(alsa_stream->instance,
					VC_AUDIO_MSG_TYPE_CLOSE, true);

	/* Stop the audio service */
	vc_vchi_audio_deinit(instance);
	alsa_stream->instance = NULL;
	kfree(instance);

	return err;
}

int bcm2835_audio_write(struct bcm2835_alsa_stream *alsa_stream,
			unsigned int size, void *src)
{
	struct bcm2835_audio_instance *instance = alsa_stream->instance;
	struct vc_audio_msg m = {
		.type = VC_AUDIO_MSG_TYPE_WRITE,
		.write.count = size,
		.write.max_packet = instance->max_packet,
		.write.cookie1 = VC_AUDIO_WRITE_COOKIE1,
		.write.cookie2 = VC_AUDIO_WRITE_COOKIE2,
	};
	unsigned int count;
	int err, status;

	if (!size)
		return 0;

	bcm2835_audio_lock(instance);
	err = bcm2835_audio_send_msg_locked(instance, &m, false);
	if (err < 0)
		goto unlock;

	count = size;
	if (!instance->max_packet) {
		/* Send the message to the videocore */
		status = vchiq_bulk_transmit(instance->service_handle, src,
					     count, NULL,
					     VCHIQ_BULK_MODE_BLOCKING);
	} else {
		while (count > 0) {
			int bytes = min(instance->max_packet, count);

			status = vchiq_queue_kernel_message(instance->service_handle,
							    src, bytes);
			src += bytes;
			count -= bytes;
		}
	}

	if (status) {
		dev_err(instance->dev,
			"failed on %d bytes transfer (status=%d)\n",
			size, status);
		err = -EIO;
	}

 unlock:
	bcm2835_audio_unlock(instance);
	return err;
}
