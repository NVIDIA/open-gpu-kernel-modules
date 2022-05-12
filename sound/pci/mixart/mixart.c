// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Driver for Digigram miXart soundcards
 *
 * main file with alsa callbacks
 *
 * Copyright (c) 2003 by Digigram <alsa@digigram.com>
 */


#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>

#include <sound/core.h>
#include <sound/initval.h>
#include <sound/info.h>
#include <sound/control.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include "mixart.h"
#include "mixart_hwdep.h"
#include "mixart_core.h"
#include "mixart_mixer.h"

#define CARD_NAME "miXart"

MODULE_AUTHOR("Digigram <alsa@digigram.com>");
MODULE_DESCRIPTION("Digigram " CARD_NAME);
MODULE_LICENSE("GPL");

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;             /* Index 0-MAX */
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;              /* ID for this card */
static bool enable[SNDRV_CARDS] = SNDRV_DEFAULT_ENABLE_PNP;     /* Enable this card */

module_param_array(index, int, NULL, 0444);
MODULE_PARM_DESC(index, "Index value for Digigram " CARD_NAME " soundcard.");
module_param_array(id, charp, NULL, 0444);
MODULE_PARM_DESC(id, "ID string for Digigram " CARD_NAME " soundcard.");
module_param_array(enable, bool, NULL, 0444);
MODULE_PARM_DESC(enable, "Enable Digigram " CARD_NAME " soundcard.");

/*
 */

static const struct pci_device_id snd_mixart_ids[] = {
	{ PCI_VDEVICE(MOTOROLA, 0x0003), 0, }, /* MC8240 */
	{ 0, }
};

MODULE_DEVICE_TABLE(pci, snd_mixart_ids);


static int mixart_set_pipe_state(struct mixart_mgr *mgr,
				 struct mixart_pipe *pipe, int start)
{
	struct mixart_group_state_req group_state;
	struct mixart_group_state_resp group_state_resp;
	struct mixart_msg request;
	int err;
	u32 system_msg_uid;

	switch(pipe->status) {
	case PIPE_RUNNING:
	case PIPE_CLOCK_SET:
		if(start) return 0; /* already started */
		break;
	case PIPE_STOPPED:
		if(!start) return 0; /* already stopped */
		break;
	default:
		dev_err(&mgr->pci->dev,
			"error mixart_set_pipe_state called with wrong pipe->status!\n");
		return -EINVAL;      /* function called with wrong pipe status */
	}

	system_msg_uid = 0x12345678; /* the event ! (take care: the MSB and two LSB's have to be 0) */

	/* wait on the last MSG_SYSTEM_SEND_SYNCHRO_CMD command to be really finished */

	request.message_id = MSG_SYSTEM_WAIT_SYNCHRO_CMD;
	request.uid = (struct mixart_uid){0,0};
	request.data = &system_msg_uid;
	request.size = sizeof(system_msg_uid);

	err = snd_mixart_send_msg_wait_notif(mgr, &request, system_msg_uid);
	if(err) {
		dev_err(&mgr->pci->dev,
			"error : MSG_SYSTEM_WAIT_SYNCHRO_CMD was not notified !\n");
		return err;
	}

	/* start or stop the pipe (1 pipe) */

	memset(&group_state, 0, sizeof(group_state));
	group_state.pipe_count = 1;
	group_state.pipe_uid[0] = pipe->group_uid;

	if(start)
		request.message_id = MSG_STREAM_START_STREAM_GRP_PACKET;
	else
		request.message_id = MSG_STREAM_STOP_STREAM_GRP_PACKET;

	request.uid = pipe->group_uid; /*(struct mixart_uid){0,0};*/
	request.data = &group_state;
	request.size = sizeof(group_state);

	err = snd_mixart_send_msg(mgr, &request, sizeof(group_state_resp), &group_state_resp);
	if (err < 0 || group_state_resp.txx_status != 0) {
		dev_err(&mgr->pci->dev,
			"error MSG_STREAM_ST***_STREAM_GRP_PACKET err=%x stat=%x !\n",
			err, group_state_resp.txx_status);
		return -EINVAL;
	}

	if(start) {
		u32 stat = 0;

		group_state.pipe_count = 0; /* in case of start same command once again with pipe_count=0 */

		err = snd_mixart_send_msg(mgr, &request, sizeof(group_state_resp), &group_state_resp);
		if (err < 0 || group_state_resp.txx_status != 0) {
			dev_err(&mgr->pci->dev,
				"error MSG_STREAM_START_STREAM_GRP_PACKET err=%x stat=%x !\n",
				err, group_state_resp.txx_status);
 			return -EINVAL;
		}

		/* in case of start send a synchro top */

		request.message_id = MSG_SYSTEM_SEND_SYNCHRO_CMD;
		request.uid = (struct mixart_uid){0,0};
		request.data = NULL;
		request.size = 0;

		err = snd_mixart_send_msg(mgr, &request, sizeof(stat), &stat);
		if (err < 0 || stat != 0) {
			dev_err(&mgr->pci->dev,
				"error MSG_SYSTEM_SEND_SYNCHRO_CMD err=%x stat=%x !\n",
				err, stat);
			return -EINVAL;
		}

		pipe->status = PIPE_RUNNING;
	}
	else /* !start */
		pipe->status = PIPE_STOPPED;

	return 0;
}


static int mixart_set_clock(struct mixart_mgr *mgr,
			    struct mixart_pipe *pipe, unsigned int rate)
{
	struct mixart_msg request;
	struct mixart_clock_properties clock_properties;
	struct mixart_clock_properties_resp clock_prop_resp;
	int err;

	switch(pipe->status) {
	case PIPE_CLOCK_SET:
		break;
	case PIPE_RUNNING:
		if(rate != 0)
			break;
		fallthrough;
	default:
		if(rate == 0)
			return 0; /* nothing to do */
		else {
			dev_err(&mgr->pci->dev,
				"error mixart_set_clock(%d) called with wrong pipe->status !\n",
				rate);
			return -EINVAL;
		}
	}

	memset(&clock_properties, 0, sizeof(clock_properties));
	clock_properties.clock_generic_type = (rate != 0) ? CGT_INTERNAL_CLOCK : CGT_NO_CLOCK;
	clock_properties.clock_mode = CM_STANDALONE;
	clock_properties.frequency = rate;
	clock_properties.nb_callers = 1; /* only one entry in uid_caller ! */
	clock_properties.uid_caller[0] = pipe->group_uid;

	dev_dbg(&mgr->pci->dev, "mixart_set_clock to %d kHz\n", rate);

	request.message_id = MSG_CLOCK_SET_PROPERTIES;
	request.uid = mgr->uid_console_manager;
	request.data = &clock_properties;
	request.size = sizeof(clock_properties);

	err = snd_mixart_send_msg(mgr, &request, sizeof(clock_prop_resp), &clock_prop_resp);
	if (err < 0 || clock_prop_resp.status != 0 || clock_prop_resp.clock_mode != CM_STANDALONE) {
		dev_err(&mgr->pci->dev,
			"error MSG_CLOCK_SET_PROPERTIES err=%x stat=%x mod=%x !\n",
			err, clock_prop_resp.status, clock_prop_resp.clock_mode);
		return -EINVAL;
	}

	if(rate)  pipe->status = PIPE_CLOCK_SET;
	else      pipe->status = PIPE_RUNNING;

	return 0;
}


/*
 *  Allocate or reference output pipe for analog IOs (pcmp0/1)
 */
struct mixart_pipe *
snd_mixart_add_ref_pipe(struct snd_mixart *chip, int pcm_number, int capture,
			int monitoring)
{
	int stream_count;
	struct mixart_pipe *pipe;
	struct mixart_msg request;

	if(capture) {
		if (pcm_number == MIXART_PCM_ANALOG) {
			pipe = &(chip->pipe_in_ana);  /* analog inputs */
		} else {
			pipe = &(chip->pipe_in_dig); /* digital inputs */
		}
		request.message_id = MSG_STREAM_ADD_OUTPUT_GROUP;
		stream_count = MIXART_CAPTURE_STREAMS;
	} else {
		if (pcm_number == MIXART_PCM_ANALOG) {
			pipe = &(chip->pipe_out_ana);  /* analog outputs */
		} else {
			pipe = &(chip->pipe_out_dig);  /* digital outputs */
		}
		request.message_id = MSG_STREAM_ADD_INPUT_GROUP;
		stream_count = MIXART_PLAYBACK_STREAMS;
	}

	/* a new stream is opened and there are already all streams in use */
	if( (monitoring == 0) && (pipe->references >= stream_count) ) {
		return NULL;
	}

	/* pipe is not yet defined */
	if( pipe->status == PIPE_UNDEFINED ) {
		int err, i;
		struct {
			struct mixart_streaming_group_req sgroup_req;
			struct mixart_streaming_group sgroup_resp;
		} *buf;

		dev_dbg(chip->card->dev,
			"add_ref_pipe audio chip(%d) pcm(%d)\n",
			chip->chip_idx, pcm_number);

		buf = kmalloc(sizeof(*buf), GFP_KERNEL);
		if (!buf)
			return NULL;

		request.uid = (struct mixart_uid){0,0};      /* should be StreamManagerUID, but zero is OK if there is only one ! */
		request.data = &buf->sgroup_req;
		request.size = sizeof(buf->sgroup_req);

		memset(&buf->sgroup_req, 0, sizeof(buf->sgroup_req));

		buf->sgroup_req.stream_count = stream_count;
		buf->sgroup_req.channel_count = 2;
		buf->sgroup_req.latency = 256;
		buf->sgroup_req.connector = pipe->uid_left_connector;  /* the left connector */

		for (i=0; i<stream_count; i++) {
			int j;
			struct mixart_flowinfo *flowinfo;
			struct mixart_bufferinfo *bufferinfo;
			
			/* we don't yet know the format, so config 16 bit pcm audio for instance */
			buf->sgroup_req.stream_info[i].size_max_byte_frame = 1024;
			buf->sgroup_req.stream_info[i].size_max_sample_frame = 256;
			buf->sgroup_req.stream_info[i].nb_bytes_max_per_sample = MIXART_FLOAT_P__4_0_TO_HEX; /* is 4.0f */

			/* find the right bufferinfo_array */
			j = (chip->chip_idx * MIXART_MAX_STREAM_PER_CARD) + (pcm_number * (MIXART_PLAYBACK_STREAMS + MIXART_CAPTURE_STREAMS)) + i;
			if(capture) j += MIXART_PLAYBACK_STREAMS; /* in the array capture is behind playback */

			buf->sgroup_req.flow_entry[i] = j;

			flowinfo = (struct mixart_flowinfo *)chip->mgr->flowinfo.area;
			flowinfo[j].bufferinfo_array_phy_address = (u32)chip->mgr->bufferinfo.addr + (j * sizeof(struct mixart_bufferinfo));
			flowinfo[j].bufferinfo_count = 1;               /* 1 will set the miXart to ring-buffer mode ! */

			bufferinfo = (struct mixart_bufferinfo *)chip->mgr->bufferinfo.area;
			bufferinfo[j].buffer_address = 0;               /* buffer is not yet allocated */
			bufferinfo[j].available_length = 0;             /* buffer is not yet allocated */

			/* construct the identifier of the stream buffer received in the interrupts ! */
			bufferinfo[j].buffer_id = (chip->chip_idx << MIXART_NOTIFY_CARD_OFFSET) + (pcm_number << MIXART_NOTIFY_PCM_OFFSET ) + i;
			if(capture) {
				bufferinfo[j].buffer_id |= MIXART_NOTIFY_CAPT_MASK;
			}
		}

		err = snd_mixart_send_msg(chip->mgr, &request, sizeof(buf->sgroup_resp), &buf->sgroup_resp);
		if((err < 0) || (buf->sgroup_resp.status != 0)) {
			dev_err(chip->card->dev,
				"error MSG_STREAM_ADD_**PUT_GROUP err=%x stat=%x !\n",
				err, buf->sgroup_resp.status);
			kfree(buf);
			return NULL;
		}

		pipe->group_uid = buf->sgroup_resp.group;     /* id of the pipe, as returned by embedded */
		pipe->stream_count = buf->sgroup_resp.stream_count;
		/* pipe->stream_uid[i] = buf->sgroup_resp.stream[i].stream_uid; */

		pipe->status = PIPE_STOPPED;
		kfree(buf);
	}

	if(monitoring)	pipe->monitoring = 1;
	else		pipe->references++;

	return pipe;
}


int snd_mixart_kill_ref_pipe(struct mixart_mgr *mgr,
			     struct mixart_pipe *pipe, int monitoring)
{
	int err = 0;

	if(pipe->status == PIPE_UNDEFINED)
		return 0;

	if(monitoring)
		pipe->monitoring = 0;
	else
		pipe->references--;

	if((pipe->references <= 0) && (pipe->monitoring == 0)) {

		struct mixart_msg request;
		struct mixart_delete_group_resp delete_resp;

		/* release the clock */
		err = mixart_set_clock( mgr, pipe, 0);
		if( err < 0 ) {
			dev_err(&mgr->pci->dev,
				"mixart_set_clock(0) return error!\n");
		}

		/* stop the pipe */
		err = mixart_set_pipe_state(mgr, pipe, 0);
		if( err < 0 ) {
			dev_err(&mgr->pci->dev, "error stopping pipe!\n");
		}

		request.message_id = MSG_STREAM_DELETE_GROUP;
		request.uid = (struct mixart_uid){0,0};
		request.data = &pipe->group_uid;            /* the streaming group ! */
		request.size = sizeof(pipe->group_uid);

		/* delete the pipe */
		err = snd_mixart_send_msg(mgr, &request, sizeof(delete_resp), &delete_resp);
		if ((err < 0) || (delete_resp.status != 0)) {
			dev_err(&mgr->pci->dev,
				"error MSG_STREAM_DELETE_GROUP err(%x), status(%x)\n",
				err, delete_resp.status);
		}

		pipe->group_uid = (struct mixart_uid){0,0};
		pipe->stream_count = 0;
		pipe->status = PIPE_UNDEFINED;
	}

	return err;
}

static int mixart_set_stream_state(struct mixart_stream *stream, int start)
{
	struct snd_mixart *chip;
	struct mixart_stream_state_req stream_state_req;
	struct mixart_msg request;

	if(!stream->substream)
		return -EINVAL;

	memset(&stream_state_req, 0, sizeof(stream_state_req));
	stream_state_req.stream_count = 1;
	stream_state_req.stream_info.stream_desc.uid_pipe = stream->pipe->group_uid;
	stream_state_req.stream_info.stream_desc.stream_idx = stream->substream->number;

	if (stream->substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		request.message_id = start ? MSG_STREAM_START_INPUT_STAGE_PACKET : MSG_STREAM_STOP_INPUT_STAGE_PACKET;
	else
		request.message_id = start ? MSG_STREAM_START_OUTPUT_STAGE_PACKET : MSG_STREAM_STOP_OUTPUT_STAGE_PACKET;

	request.uid = (struct mixart_uid){0,0};
	request.data = &stream_state_req;
	request.size = sizeof(stream_state_req);

	stream->abs_period_elapsed = 0;            /* reset stream pos      */
	stream->buf_periods = 0;
	stream->buf_period_frag = 0;

	chip = snd_pcm_substream_chip(stream->substream);

	return snd_mixart_send_msg_nonblock(chip->mgr, &request);
}

/*
 *  Trigger callback
 */

static int snd_mixart_trigger(struct snd_pcm_substream *subs, int cmd)
{
	struct mixart_stream *stream = subs->runtime->private_data;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:

		dev_dbg(subs->pcm->card->dev, "SNDRV_PCM_TRIGGER_START\n");

		/* START_STREAM */
		if( mixart_set_stream_state(stream, 1) )
			return -EINVAL;

		stream->status = MIXART_STREAM_STATUS_RUNNING;

		break;
	case SNDRV_PCM_TRIGGER_STOP:

		/* STOP_STREAM */
		if( mixart_set_stream_state(stream, 0) )
			return -EINVAL;

		stream->status = MIXART_STREAM_STATUS_OPEN;

		dev_dbg(subs->pcm->card->dev, "SNDRV_PCM_TRIGGER_STOP\n");

		break;

	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		/* TODO */
		stream->status = MIXART_STREAM_STATUS_PAUSE;
		dev_dbg(subs->pcm->card->dev, "SNDRV_PCM_PAUSE_PUSH\n");
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		/* TODO */
		stream->status = MIXART_STREAM_STATUS_RUNNING;
		dev_dbg(subs->pcm->card->dev, "SNDRV_PCM_PAUSE_RELEASE\n");
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int mixart_sync_nonblock_events(struct mixart_mgr *mgr)
{
	unsigned long timeout = jiffies + HZ;
	while (atomic_read(&mgr->msg_processed) > 0) {
		if (time_after(jiffies, timeout)) {
			dev_err(&mgr->pci->dev,
				"mixart: cannot process nonblock events!\n");
			return -EBUSY;
		}
		schedule_timeout_uninterruptible(1);
	}
	return 0;
}

/*
 *  prepare callback for all pcms
 */
static int snd_mixart_prepare(struct snd_pcm_substream *subs)
{
	struct snd_mixart *chip = snd_pcm_substream_chip(subs);
	struct mixart_stream *stream = subs->runtime->private_data;

	/* TODO de façon non bloquante, réappliquer les hw_params (rate, bits, codec) */

	dev_dbg(chip->card->dev, "snd_mixart_prepare\n");

	mixart_sync_nonblock_events(chip->mgr);

	/* only the first stream can choose the sample rate */
	/* the further opened streams will be limited to its frequency (see open) */
	if(chip->mgr->ref_count_rate == 1)
		chip->mgr->sample_rate = subs->runtime->rate;

	/* set the clock only once (first stream) on the same pipe */
	if(stream->pipe->references == 1) {
		if( mixart_set_clock(chip->mgr, stream->pipe, subs->runtime->rate) )
			return -EINVAL;
	}

	return 0;
}


static int mixart_set_format(struct mixart_stream *stream, snd_pcm_format_t format)
{
	int err;
	struct snd_mixart *chip;
	struct mixart_msg request;
	struct mixart_stream_param_desc stream_param;
	struct mixart_return_uid resp;

	chip = snd_pcm_substream_chip(stream->substream);

	memset(&stream_param, 0, sizeof(stream_param));

	stream_param.coding_type = CT_LINEAR;
	stream_param.number_of_channel = stream->channels;

	stream_param.sampling_freq = chip->mgr->sample_rate;
	if(stream_param.sampling_freq == 0)
		stream_param.sampling_freq = 44100; /* if frequency not yet defined, use some default */

	switch(format){
	case SNDRV_PCM_FORMAT_U8:
		stream_param.sample_type = ST_INTEGER_8;
		stream_param.sample_size = 8;
		break;
	case SNDRV_PCM_FORMAT_S16_LE:
		stream_param.sample_type = ST_INTEGER_16LE;
		stream_param.sample_size = 16;
		break;
	case SNDRV_PCM_FORMAT_S16_BE:
		stream_param.sample_type = ST_INTEGER_16BE;
		stream_param.sample_size = 16;
		break;
	case SNDRV_PCM_FORMAT_S24_3LE:
		stream_param.sample_type = ST_INTEGER_24LE;
		stream_param.sample_size = 24;
		break;
	case SNDRV_PCM_FORMAT_S24_3BE:
		stream_param.sample_type = ST_INTEGER_24BE;
		stream_param.sample_size = 24;
		break;
	case SNDRV_PCM_FORMAT_FLOAT_LE:
		stream_param.sample_type = ST_FLOATING_POINT_32LE;
		stream_param.sample_size = 32;
		break;
	case  SNDRV_PCM_FORMAT_FLOAT_BE:
		stream_param.sample_type = ST_FLOATING_POINT_32BE;
		stream_param.sample_size = 32;
		break;
	default:
		dev_err(chip->card->dev,
			"error mixart_set_format() : unknown format\n");
		return -EINVAL;
	}

	dev_dbg(chip->card->dev,
		"set SNDRV_PCM_FORMAT sample_type(%d) sample_size(%d) freq(%d) channels(%d)\n",
		   stream_param.sample_type, stream_param.sample_size, stream_param.sampling_freq, stream->channels);

	/* TODO: what else to configure ? */
	/* stream_param.samples_per_frame = 2; */
	/* stream_param.bytes_per_frame = 4; */
	/* stream_param.bytes_per_sample = 2; */

	stream_param.pipe_count = 1;      /* set to 1 */
	stream_param.stream_count = 1;    /* set to 1 */
	stream_param.stream_desc[0].uid_pipe = stream->pipe->group_uid;
	stream_param.stream_desc[0].stream_idx = stream->substream->number;

	request.message_id = MSG_STREAM_SET_INPUT_STAGE_PARAM;
	request.uid = (struct mixart_uid){0,0};
	request.data = &stream_param;
	request.size = sizeof(stream_param);

	err = snd_mixart_send_msg(chip->mgr, &request, sizeof(resp), &resp);
	if((err < 0) || resp.error_code) {
		dev_err(chip->card->dev,
			"MSG_STREAM_SET_INPUT_STAGE_PARAM err=%x; resp=%x\n",
			err, resp.error_code);
		return -EINVAL;
	}
	return 0;
}


/*
 *  HW_PARAMS callback for all pcms
 */
static int snd_mixart_hw_params(struct snd_pcm_substream *subs,
                                struct snd_pcm_hw_params *hw)
{
	struct snd_mixart *chip = snd_pcm_substream_chip(subs);
	struct mixart_mgr *mgr = chip->mgr;
	struct mixart_stream *stream = subs->runtime->private_data;
	snd_pcm_format_t format;
	int err;
	int channels;

	/* set up channels */
	channels = params_channels(hw);

	/*  set up format for the stream */
	format = params_format(hw);

	mutex_lock(&mgr->setup_mutex);

	/* update the stream levels */
	if( stream->pcm_number <= MIXART_PCM_DIGITAL ) {
		int is_aes = stream->pcm_number > MIXART_PCM_ANALOG;
		if( subs->stream == SNDRV_PCM_STREAM_PLAYBACK )
			mixart_update_playback_stream_level(chip, is_aes, subs->number);
		else
			mixart_update_capture_stream_level( chip, is_aes);
	}

	stream->channels = channels;

	/* set the format to the board */
	err = mixart_set_format(stream, format);
	if(err < 0) {
		mutex_unlock(&mgr->setup_mutex);
		return err;
	}

	if (subs->runtime->buffer_changed) {
		struct mixart_bufferinfo *bufferinfo;
		int i = (chip->chip_idx * MIXART_MAX_STREAM_PER_CARD) + (stream->pcm_number * (MIXART_PLAYBACK_STREAMS+MIXART_CAPTURE_STREAMS)) + subs->number;
		if( subs->stream == SNDRV_PCM_STREAM_CAPTURE ) {
			i += MIXART_PLAYBACK_STREAMS; /* in array capture is behind playback */
		}
		
		bufferinfo = (struct mixart_bufferinfo *)chip->mgr->bufferinfo.area;
		bufferinfo[i].buffer_address = subs->runtime->dma_addr;
		bufferinfo[i].available_length = subs->runtime->dma_bytes;
		/* bufferinfo[i].buffer_id  is already defined */

		dev_dbg(chip->card->dev,
			"snd_mixart_hw_params(pcm %d) : dma_addr(%x) dma_bytes(%x) subs-number(%d)\n",
			i, bufferinfo[i].buffer_address,
				bufferinfo[i].available_length,
				subs->number);
	}
	mutex_unlock(&mgr->setup_mutex);

	return 0;
}

static int snd_mixart_hw_free(struct snd_pcm_substream *subs)
{
	struct snd_mixart *chip = snd_pcm_substream_chip(subs);
	mixart_sync_nonblock_events(chip->mgr);
	return 0;
}



/*
 *  TODO CONFIGURATION SPACE for all pcms, mono pcm must update channels_max
 */
static const struct snd_pcm_hardware snd_mixart_analog_caps =
{
	.info             = ( SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_INTERLEAVED |
			      SNDRV_PCM_INFO_MMAP_VALID |
			      SNDRV_PCM_INFO_PAUSE),
	.formats	  = ( SNDRV_PCM_FMTBIT_U8 |
			      SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S16_BE |
			      SNDRV_PCM_FMTBIT_S24_3LE | SNDRV_PCM_FMTBIT_S24_3BE |
			      SNDRV_PCM_FMTBIT_FLOAT_LE | SNDRV_PCM_FMTBIT_FLOAT_BE ),
	.rates            = SNDRV_PCM_RATE_CONTINUOUS | SNDRV_PCM_RATE_8000_48000,
	.rate_min         = 8000,
	.rate_max         = 48000,
	.channels_min     = 1,
	.channels_max     = 2,
	.buffer_bytes_max = (32*1024),
	.period_bytes_min = 256,                  /* 256 frames U8 mono*/
	.period_bytes_max = (16*1024),
	.periods_min      = 2,
	.periods_max      = (32*1024/256),
};

static const struct snd_pcm_hardware snd_mixart_digital_caps =
{
	.info             = ( SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_INTERLEAVED |
			      SNDRV_PCM_INFO_MMAP_VALID |
			      SNDRV_PCM_INFO_PAUSE),
	.formats	  = ( SNDRV_PCM_FMTBIT_U8 |
			      SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S16_BE |
			      SNDRV_PCM_FMTBIT_S24_3LE | SNDRV_PCM_FMTBIT_S24_3BE |
			      SNDRV_PCM_FMTBIT_FLOAT_LE | SNDRV_PCM_FMTBIT_FLOAT_BE ),
	.rates            = SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_48000,
	.rate_min         = 32000,
	.rate_max         = 48000,
	.channels_min     = 1,
	.channels_max     = 2,
	.buffer_bytes_max = (32*1024),
	.period_bytes_min = 256,                  /* 256 frames U8 mono*/
	.period_bytes_max = (16*1024),
	.periods_min      = 2,
	.periods_max      = (32*1024/256),
};


static int snd_mixart_playback_open(struct snd_pcm_substream *subs)
{
	struct snd_mixart            *chip = snd_pcm_substream_chip(subs);
	struct mixart_mgr        *mgr = chip->mgr;
	struct snd_pcm_runtime *runtime = subs->runtime;
	struct snd_pcm *pcm = subs->pcm;
	struct mixart_stream     *stream;
	struct mixart_pipe       *pipe;
	int err = 0;
	int pcm_number;

	mutex_lock(&mgr->setup_mutex);

	if ( pcm == chip->pcm ) {
		pcm_number = MIXART_PCM_ANALOG;
		runtime->hw = snd_mixart_analog_caps;
	} else {
		snd_BUG_ON(pcm != chip->pcm_dig);
		pcm_number = MIXART_PCM_DIGITAL;
		runtime->hw = snd_mixart_digital_caps;
	}
	dev_dbg(chip->card->dev,
		"snd_mixart_playback_open C%d/P%d/Sub%d\n",
		chip->chip_idx, pcm_number, subs->number);

	/* get stream info */
	stream = &(chip->playback_stream[pcm_number][subs->number]);

	if (stream->status != MIXART_STREAM_STATUS_FREE){
		/* streams in use */
		dev_err(chip->card->dev,
			"snd_mixart_playback_open C%d/P%d/Sub%d in use\n",
			chip->chip_idx, pcm_number, subs->number);
		err = -EBUSY;
		goto _exit_open;
	}

	/* get pipe pointer (out pipe) */
	pipe = snd_mixart_add_ref_pipe(chip, pcm_number, 0, 0);

	if (pipe == NULL) {
		err = -EINVAL;
		goto _exit_open;
	}

	/* start the pipe if necessary */
	err = mixart_set_pipe_state(chip->mgr, pipe, 1);
	if( err < 0 ) {
		dev_err(chip->card->dev, "error starting pipe!\n");
		snd_mixart_kill_ref_pipe(chip->mgr, pipe, 0);
		err = -EINVAL;
		goto _exit_open;
	}

	stream->pipe        = pipe;
	stream->pcm_number  = pcm_number;
	stream->status      = MIXART_STREAM_STATUS_OPEN;
	stream->substream   = subs;
	stream->channels    = 0; /* not configured yet */

	runtime->private_data = stream;

	snd_pcm_hw_constraint_step(runtime, 0, SNDRV_PCM_HW_PARAM_PERIOD_BYTES, 32);
	snd_pcm_hw_constraint_step(runtime, 0, SNDRV_PCM_HW_PARAM_BUFFER_SIZE, 64);

	/* if a sample rate is already used, another stream cannot change */
	if(mgr->ref_count_rate++) {
		if(mgr->sample_rate) {
			runtime->hw.rate_min = runtime->hw.rate_max = mgr->sample_rate;
		}
	}

 _exit_open:
	mutex_unlock(&mgr->setup_mutex);

	return err;
}


static int snd_mixart_capture_open(struct snd_pcm_substream *subs)
{
	struct snd_mixart            *chip = snd_pcm_substream_chip(subs);
	struct mixart_mgr        *mgr = chip->mgr;
	struct snd_pcm_runtime *runtime = subs->runtime;
	struct snd_pcm *pcm = subs->pcm;
	struct mixart_stream     *stream;
	struct mixart_pipe       *pipe;
	int err = 0;
	int pcm_number;

	mutex_lock(&mgr->setup_mutex);

	if ( pcm == chip->pcm ) {
		pcm_number = MIXART_PCM_ANALOG;
		runtime->hw = snd_mixart_analog_caps;
	} else {
		snd_BUG_ON(pcm != chip->pcm_dig);
		pcm_number = MIXART_PCM_DIGITAL;
		runtime->hw = snd_mixart_digital_caps;
	}

	runtime->hw.channels_min = 2; /* for instance, no mono */

	dev_dbg(chip->card->dev, "snd_mixart_capture_open C%d/P%d/Sub%d\n",
		chip->chip_idx, pcm_number, subs->number);

	/* get stream info */
	stream = &(chip->capture_stream[pcm_number]);

	if (stream->status != MIXART_STREAM_STATUS_FREE){
		/* streams in use */
		dev_err(chip->card->dev,
			"snd_mixart_capture_open C%d/P%d/Sub%d in use\n",
			chip->chip_idx, pcm_number, subs->number);
		err = -EBUSY;
		goto _exit_open;
	}

	/* get pipe pointer (in pipe) */
	pipe = snd_mixart_add_ref_pipe(chip, pcm_number, 1, 0);

	if (pipe == NULL) {
		err = -EINVAL;
		goto _exit_open;
	}

	/* start the pipe if necessary */
	err = mixart_set_pipe_state(chip->mgr, pipe, 1);
	if( err < 0 ) {
		dev_err(chip->card->dev, "error starting pipe!\n");
		snd_mixart_kill_ref_pipe(chip->mgr, pipe, 0);
		err = -EINVAL;
		goto _exit_open;
	}

	stream->pipe        = pipe;
	stream->pcm_number  = pcm_number;
	stream->status      = MIXART_STREAM_STATUS_OPEN;
	stream->substream   = subs;
	stream->channels    = 0; /* not configured yet */

	runtime->private_data = stream;

	snd_pcm_hw_constraint_step(runtime, 0, SNDRV_PCM_HW_PARAM_PERIOD_BYTES, 32);
	snd_pcm_hw_constraint_step(runtime, 0, SNDRV_PCM_HW_PARAM_BUFFER_SIZE, 64);

	/* if a sample rate is already used, another stream cannot change */
	if(mgr->ref_count_rate++) {
		if(mgr->sample_rate) {
			runtime->hw.rate_min = runtime->hw.rate_max = mgr->sample_rate;
		}
	}

 _exit_open:
	mutex_unlock(&mgr->setup_mutex);

	return err;
}



static int snd_mixart_close(struct snd_pcm_substream *subs)
{
	struct snd_mixart *chip = snd_pcm_substream_chip(subs);
	struct mixart_mgr *mgr = chip->mgr;
	struct mixart_stream *stream = subs->runtime->private_data;

	mutex_lock(&mgr->setup_mutex);

	dev_dbg(chip->card->dev, "snd_mixart_close C%d/P%d/Sub%d\n",
		chip->chip_idx, stream->pcm_number, subs->number);

	/* sample rate released */
	if(--mgr->ref_count_rate == 0) {
		mgr->sample_rate = 0;
	}

	/* delete pipe */
	if (snd_mixart_kill_ref_pipe(mgr, stream->pipe, 0 ) < 0) {

		dev_err(chip->card->dev,
			"error snd_mixart_kill_ref_pipe C%dP%d\n",
			chip->chip_idx, stream->pcm_number);
	}

	stream->pipe      = NULL;
	stream->status    = MIXART_STREAM_STATUS_FREE;
	stream->substream = NULL;

	mutex_unlock(&mgr->setup_mutex);
	return 0;
}


static snd_pcm_uframes_t snd_mixart_stream_pointer(struct snd_pcm_substream *subs)
{
	struct snd_pcm_runtime *runtime = subs->runtime;
	struct mixart_stream   *stream  = runtime->private_data;

	return (snd_pcm_uframes_t)((stream->buf_periods * runtime->period_size) + stream->buf_period_frag);
}



static const struct snd_pcm_ops snd_mixart_playback_ops = {
	.open      = snd_mixart_playback_open,
	.close     = snd_mixart_close,
	.prepare   = snd_mixart_prepare,
	.hw_params = snd_mixart_hw_params,
	.hw_free   = snd_mixart_hw_free,
	.trigger   = snd_mixart_trigger,
	.pointer   = snd_mixart_stream_pointer,
};

static const struct snd_pcm_ops snd_mixart_capture_ops = {
	.open      = snd_mixart_capture_open,
	.close     = snd_mixart_close,
	.prepare   = snd_mixart_prepare,
	.hw_params = snd_mixart_hw_params,
	.hw_free   = snd_mixart_hw_free,
	.trigger   = snd_mixart_trigger,
	.pointer   = snd_mixart_stream_pointer,
};

static void preallocate_buffers(struct snd_mixart *chip, struct snd_pcm *pcm)
{
#if 0
	struct snd_pcm_substream *subs;
	int stream;

	for (stream = 0; stream < 2; stream++) {
		int idx = 0;
		for (subs = pcm->streams[stream].substream; subs; subs = subs->next, idx++)
			/* set up the unique device id with the chip index */
			subs->dma_device.id = subs->pcm->device << 16 |
				subs->stream << 8 | (subs->number + 1) |
				(chip->chip_idx + 1) << 24;
	}
#endif
	snd_pcm_set_managed_buffer_all(pcm, SNDRV_DMA_TYPE_DEV,
				       &chip->mgr->pci->dev,
				       32*1024, 32*1024);
}

/*
 */
static int snd_mixart_pcm_analog(struct snd_mixart *chip)
{
	int err;
	struct snd_pcm *pcm;
	char name[32];

	sprintf(name, "miXart analog %d", chip->chip_idx);
	if ((err = snd_pcm_new(chip->card, name, MIXART_PCM_ANALOG,
			       MIXART_PLAYBACK_STREAMS,
			       MIXART_CAPTURE_STREAMS, &pcm)) < 0) {
		dev_err(chip->card->dev,
			"cannot create the analog pcm %d\n", chip->chip_idx);
		return err;
	}

	pcm->private_data = chip;

	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &snd_mixart_playback_ops);
	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &snd_mixart_capture_ops);

	pcm->info_flags = 0;
	pcm->nonatomic = true;
	strcpy(pcm->name, name);

	preallocate_buffers(chip, pcm);

	chip->pcm = pcm;
	return 0;
}


/*
 */
static int snd_mixart_pcm_digital(struct snd_mixart *chip)
{
	int err;
	struct snd_pcm *pcm;
	char name[32];

	sprintf(name, "miXart AES/EBU %d", chip->chip_idx);
	if ((err = snd_pcm_new(chip->card, name, MIXART_PCM_DIGITAL,
			       MIXART_PLAYBACK_STREAMS,
			       MIXART_CAPTURE_STREAMS, &pcm)) < 0) {
		dev_err(chip->card->dev,
			"cannot create the digital pcm %d\n", chip->chip_idx);
		return err;
	}

	pcm->private_data = chip;

	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &snd_mixart_playback_ops);
	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &snd_mixart_capture_ops);

	pcm->info_flags = 0;
	pcm->nonatomic = true;
	strcpy(pcm->name, name);

	preallocate_buffers(chip, pcm);

	chip->pcm_dig = pcm;
	return 0;
}

static int snd_mixart_chip_free(struct snd_mixart *chip)
{
	kfree(chip);
	return 0;
}

static int snd_mixart_chip_dev_free(struct snd_device *device)
{
	struct snd_mixart *chip = device->device_data;
	return snd_mixart_chip_free(chip);
}


/*
 */
static int snd_mixart_create(struct mixart_mgr *mgr, struct snd_card *card, int idx)
{
	int err;
	struct snd_mixart *chip;
	static const struct snd_device_ops ops = {
		.dev_free = snd_mixart_chip_dev_free,
	};

	chip = kzalloc(sizeof(*chip), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	chip->card = card;
	chip->chip_idx = idx;
	chip->mgr = mgr;
	card->sync_irq = mgr->irq;

	if ((err = snd_device_new(card, SNDRV_DEV_LOWLEVEL, chip, &ops)) < 0) {
		snd_mixart_chip_free(chip);
		return err;
	}

	mgr->chip[idx] = chip;
	return 0;
}

int snd_mixart_create_pcm(struct snd_mixart* chip)
{
	int err;

	err = snd_mixart_pcm_analog(chip);
	if (err < 0)
		return err;

	if(chip->mgr->board_type == MIXART_DAUGHTER_TYPE_AES) {

		err = snd_mixart_pcm_digital(chip);
		if (err < 0)
			return err;
	}
	return err;
}


/*
 * release all the cards assigned to a manager instance
 */
static int snd_mixart_free(struct mixart_mgr *mgr)
{
	unsigned int i;

	for (i = 0; i < mgr->num_cards; i++) {
		if (mgr->chip[i])
			snd_card_free(mgr->chip[i]->card);
	}

	/* stop mailbox */
	snd_mixart_exit_mailbox(mgr);

	/* release irq  */
	if (mgr->irq >= 0)
		free_irq(mgr->irq, mgr);

	/* reset board if some firmware was loaded */
	if(mgr->dsp_loaded) {
		snd_mixart_reset_board(mgr);
		dev_dbg(&mgr->pci->dev, "reset miXart !\n");
	}

	/* release the i/o ports */
	for (i = 0; i < 2; ++i)
		iounmap(mgr->mem[i].virt);

	pci_release_regions(mgr->pci);

	/* free flowarray */
	if(mgr->flowinfo.area) {
		snd_dma_free_pages(&mgr->flowinfo);
		mgr->flowinfo.area = NULL;
	}
	/* free bufferarray */
	if(mgr->bufferinfo.area) {
		snd_dma_free_pages(&mgr->bufferinfo);
		mgr->bufferinfo.area = NULL;
	}

	pci_disable_device(mgr->pci);
	kfree(mgr);
	return 0;
}

/*
 * proc interface
 */

/*
  mixart_BA0 proc interface for BAR 0 - read callback
 */
static ssize_t snd_mixart_BA0_read(struct snd_info_entry *entry,
				   void *file_private_data,
				   struct file *file, char __user *buf,
				   size_t count, loff_t pos)
{
	struct mixart_mgr *mgr = entry->private_data;

	count = count & ~3; /* make sure the read size is a multiple of 4 bytes */
	if (copy_to_user_fromio(buf, MIXART_MEM(mgr, pos), count))
		return -EFAULT;
	return count;
}

/*
  mixart_BA1 proc interface for BAR 1 - read callback
 */
static ssize_t snd_mixart_BA1_read(struct snd_info_entry *entry,
				   void *file_private_data,
				   struct file *file, char __user *buf,
				   size_t count, loff_t pos)
{
	struct mixart_mgr *mgr = entry->private_data;

	count = count & ~3; /* make sure the read size is a multiple of 4 bytes */
	if (copy_to_user_fromio(buf, MIXART_REG(mgr, pos), count))
		return -EFAULT;
	return count;
}

static const struct snd_info_entry_ops snd_mixart_proc_ops_BA0 = {
	.read   = snd_mixart_BA0_read,
};

static const struct snd_info_entry_ops snd_mixart_proc_ops_BA1 = {
	.read   = snd_mixart_BA1_read,
};


static void snd_mixart_proc_read(struct snd_info_entry *entry, 
                                 struct snd_info_buffer *buffer)
{
	struct snd_mixart *chip = entry->private_data;        
	u32 ref; 

	snd_iprintf(buffer, "Digigram miXart (alsa card %d)\n\n", chip->chip_idx);

	/* stats available when embedded OS is running */
	if (chip->mgr->dsp_loaded & ( 1 << MIXART_MOTHERBOARD_ELF_INDEX)) {
		snd_iprintf(buffer, "- hardware -\n");
		switch (chip->mgr->board_type ) {
		case MIXART_DAUGHTER_TYPE_NONE     : snd_iprintf(buffer, "\tmiXart8 (no daughter board)\n\n"); break;
		case MIXART_DAUGHTER_TYPE_AES      : snd_iprintf(buffer, "\tmiXart8 AES/EBU\n\n"); break;
		case MIXART_DAUGHTER_TYPE_COBRANET : snd_iprintf(buffer, "\tmiXart8 Cobranet\n\n"); break;
		default:                             snd_iprintf(buffer, "\tUNKNOWN!\n\n"); break;
		}

		snd_iprintf(buffer, "- system load -\n");	 

		/* get perf reference */

		ref = readl_be( MIXART_MEM( chip->mgr, MIXART_PSEUDOREG_PERF_SYSTEM_LOAD_OFFSET));

		if (ref) {
			u32 mailbox   = 100 * readl_be( MIXART_MEM( chip->mgr, MIXART_PSEUDOREG_PERF_MAILBX_LOAD_OFFSET)) / ref;
			u32 streaming = 100 * readl_be( MIXART_MEM( chip->mgr, MIXART_PSEUDOREG_PERF_STREAM_LOAD_OFFSET)) / ref;
			u32 interr    = 100 * readl_be( MIXART_MEM( chip->mgr, MIXART_PSEUDOREG_PERF_INTERR_LOAD_OFFSET)) / ref;

			snd_iprintf(buffer, "\tstreaming          : %d\n", streaming);
			snd_iprintf(buffer, "\tmailbox            : %d\n", mailbox);
			snd_iprintf(buffer, "\tinterrupts handling : %d\n\n", interr);
		}
	} /* endif elf loaded */
}

static void snd_mixart_proc_init(struct snd_mixart *chip)
{
	struct snd_info_entry *entry;

	/* text interface to read perf and temp meters */
	snd_card_ro_proc_new(chip->card, "board_info", chip,
			     snd_mixart_proc_read);

	if (! snd_card_proc_new(chip->card, "mixart_BA0", &entry)) {
		entry->content = SNDRV_INFO_CONTENT_DATA;
		entry->private_data = chip->mgr;	
		entry->c.ops = &snd_mixart_proc_ops_BA0;
		entry->size = MIXART_BA0_SIZE;
	}
	if (! snd_card_proc_new(chip->card, "mixart_BA1", &entry)) {
		entry->content = SNDRV_INFO_CONTENT_DATA;
		entry->private_data = chip->mgr;
		entry->c.ops = &snd_mixart_proc_ops_BA1;
		entry->size = MIXART_BA1_SIZE;
	}
}
/* end of proc interface */


/*
 *    probe function - creates the card manager
 */
static int snd_mixart_probe(struct pci_dev *pci,
			    const struct pci_device_id *pci_id)
{
	static int dev;
	struct mixart_mgr *mgr;
	unsigned int i;
	int err;
	size_t size;

	/*
	 */
	if (dev >= SNDRV_CARDS)
		return -ENODEV;
	if (! enable[dev]) {
		dev++;
		return -ENOENT;
	}

	/* enable PCI device */
	if ((err = pci_enable_device(pci)) < 0)
		return err;
	pci_set_master(pci);

	/* check if we can restrict PCI DMA transfers to 32 bits */
	if (dma_set_mask(&pci->dev, DMA_BIT_MASK(32)) < 0) {
		dev_err(&pci->dev,
			"architecture does not support 32bit PCI busmaster DMA\n");
		pci_disable_device(pci);
		return -ENXIO;
	}

	/*
	 */
	mgr = kzalloc(sizeof(*mgr), GFP_KERNEL);
	if (! mgr) {
		pci_disable_device(pci);
		return -ENOMEM;
	}

	mgr->pci = pci;
	mgr->irq = -1;

	/* resource assignment */
	if ((err = pci_request_regions(pci, CARD_NAME)) < 0) {
		kfree(mgr);
		pci_disable_device(pci);
		return err;
	}
	for (i = 0; i < 2; i++) {
		mgr->mem[i].phys = pci_resource_start(pci, i);
		mgr->mem[i].virt = pci_ioremap_bar(pci, i);
		if (!mgr->mem[i].virt) {
			dev_err(&pci->dev, "unable to remap resource 0x%lx\n",
			       mgr->mem[i].phys);
			snd_mixart_free(mgr);
			return -EBUSY;
		}
	}

	if (request_threaded_irq(pci->irq, snd_mixart_interrupt,
				 snd_mixart_threaded_irq, IRQF_SHARED,
				 KBUILD_MODNAME, mgr)) {
		dev_err(&pci->dev, "unable to grab IRQ %d\n", pci->irq);
		snd_mixart_free(mgr);
		return -EBUSY;
	}
	mgr->irq = pci->irq;

	/* init mailbox  */
	mgr->msg_fifo_readptr = 0;
	mgr->msg_fifo_writeptr = 0;

	mutex_init(&mgr->lock);
	mutex_init(&mgr->msg_lock);
	init_waitqueue_head(&mgr->msg_sleep);
	atomic_set(&mgr->msg_processed, 0);

	/* init setup mutex*/
	mutex_init(&mgr->setup_mutex);

	/* card assignment */
	mgr->num_cards = MIXART_MAX_CARDS; /* 4  FIXME: configurable? */
	for (i = 0; i < mgr->num_cards; i++) {
		struct snd_card *card;
		char tmpid[16];
		int idx;

		if (index[dev] < 0)
			idx = index[dev];
		else
			idx = index[dev] + i;
		snprintf(tmpid, sizeof(tmpid), "%s-%d", id[dev] ? id[dev] : "MIXART", i);
		err = snd_card_new(&pci->dev, idx, tmpid, THIS_MODULE,
				   0, &card);

		if (err < 0) {
			dev_err(&pci->dev, "cannot allocate the card %d\n", i);
			snd_mixart_free(mgr);
			return err;
		}

		strcpy(card->driver, CARD_NAME);
		snprintf(card->shortname, sizeof(card->shortname),
			 "Digigram miXart [PCM #%d]", i);
		snprintf(card->longname, sizeof(card->longname),
			"Digigram miXart at 0x%lx & 0x%lx, irq %i [PCM #%d]",
			mgr->mem[0].phys, mgr->mem[1].phys, mgr->irq, i);

		if ((err = snd_mixart_create(mgr, card, i)) < 0) {
			snd_card_free(card);
			snd_mixart_free(mgr);
			return err;
		}

		if(i==0) {
			/* init proc interface only for chip0 */
			snd_mixart_proc_init(mgr->chip[i]);
		}

		if ((err = snd_card_register(card)) < 0) {
			snd_mixart_free(mgr);
			return err;
		}
	}

	/* init firmware status (mgr->dsp_loaded reset in hwdep_new) */
	mgr->board_type = MIXART_DAUGHTER_TYPE_NONE;

	/* create array of streaminfo */
	size = PAGE_ALIGN( (MIXART_MAX_STREAM_PER_CARD * MIXART_MAX_CARDS *
			    sizeof(struct mixart_flowinfo)) );
	if (snd_dma_alloc_pages(SNDRV_DMA_TYPE_DEV, &pci->dev,
				size, &mgr->flowinfo) < 0) {
		snd_mixart_free(mgr);
		return -ENOMEM;
	}
	/* init streaminfo_array */
	memset(mgr->flowinfo.area, 0, size);

	/* create array of bufferinfo */
	size = PAGE_ALIGN( (MIXART_MAX_STREAM_PER_CARD * MIXART_MAX_CARDS *
			    sizeof(struct mixart_bufferinfo)) );
	if (snd_dma_alloc_pages(SNDRV_DMA_TYPE_DEV, &pci->dev,
				size, &mgr->bufferinfo) < 0) {
		snd_mixart_free(mgr);
		return -ENOMEM;
	}
	/* init bufferinfo_array */
	memset(mgr->bufferinfo.area, 0, size);

	/* set up firmware */
	err = snd_mixart_setup_firmware(mgr);
	if (err < 0) {
		snd_mixart_free(mgr);
		return err;
	}

	pci_set_drvdata(pci, mgr);
	dev++;
	return 0;
}

static void snd_mixart_remove(struct pci_dev *pci)
{
	snd_mixart_free(pci_get_drvdata(pci));
}

static struct pci_driver mixart_driver = {
	.name = KBUILD_MODNAME,
	.id_table = snd_mixart_ids,
	.probe = snd_mixart_probe,
	.remove = snd_mixart_remove,
};

module_pci_driver(mixart_driver);
