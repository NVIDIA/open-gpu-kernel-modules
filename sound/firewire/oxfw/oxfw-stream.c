// SPDX-License-Identifier: GPL-2.0-only
/*
 * oxfw_stream.c - a part of driver for OXFW970/971 based devices
 *
 * Copyright (c) 2014 Takashi Sakamoto
 */

#include "oxfw.h"
#include <linux/delay.h>

#define AVC_GENERIC_FRAME_MAXIMUM_BYTES	512
#define CALLBACK_TIMEOUT	200

/*
 * According to datasheet of Oxford Semiconductor:
 *  OXFW970: 32.0/44.1/48.0/96.0 Khz, 8 audio channels I/O
 *  OXFW971: 32.0/44.1/48.0/88.2/96.0/192.0 kHz, 16 audio channels I/O, MIDI I/O
 */
static const unsigned int oxfw_rate_table[] = {
	[0] = 32000,
	[1] = 44100,
	[2] = 48000,
	[3] = 88200,
	[4] = 96000,
	[5] = 192000,
};

/*
 * See Table 5.7 – Sampling frequency for Multi-bit Audio
 * in AV/C Stream Format Information Specification 1.1 (Apr 2005, 1394TA)
 */
static const unsigned int avc_stream_rate_table[] = {
	[0] = 0x02,
	[1] = 0x03,
	[2] = 0x04,
	[3] = 0x0a,
	[4] = 0x05,
	[5] = 0x07,
};

static int set_rate(struct snd_oxfw *oxfw, unsigned int rate)
{
	int err;

	err = avc_general_set_sig_fmt(oxfw->unit, rate,
				      AVC_GENERAL_PLUG_DIR_IN, 0);
	if (err < 0)
		goto end;

	if (oxfw->has_output)
		err = avc_general_set_sig_fmt(oxfw->unit, rate,
					      AVC_GENERAL_PLUG_DIR_OUT, 0);
end:
	return err;
}

static int set_stream_format(struct snd_oxfw *oxfw, struct amdtp_stream *s,
			     unsigned int rate, unsigned int pcm_channels)
{
	u8 **formats;
	struct snd_oxfw_stream_formation formation;
	enum avc_general_plug_dir dir;
	unsigned int len;
	int i, err;

	if (s == &oxfw->tx_stream) {
		formats = oxfw->tx_stream_formats;
		dir = AVC_GENERAL_PLUG_DIR_OUT;
	} else {
		formats = oxfw->rx_stream_formats;
		dir = AVC_GENERAL_PLUG_DIR_IN;
	}

	/* Seek stream format for requirements. */
	for (i = 0; i < SND_OXFW_STREAM_FORMAT_ENTRIES; i++) {
		err = snd_oxfw_stream_parse_format(formats[i], &formation);
		if (err < 0)
			return err;

		if ((formation.rate == rate) && (formation.pcm == pcm_channels))
			break;
	}
	if (i == SND_OXFW_STREAM_FORMAT_ENTRIES)
		return -EINVAL;

	/* If assumed, just change rate. */
	if (oxfw->assumed)
		return set_rate(oxfw, rate);

	/* Calculate format length. */
	len = 5 + formats[i][4] * 2;

	err = avc_stream_set_format(oxfw->unit, dir, 0, formats[i], len);
	if (err < 0)
		return err;

	/* Some requests just after changing format causes freezing. */
	msleep(100);

	return 0;
}

static int start_stream(struct snd_oxfw *oxfw, struct amdtp_stream *stream)
{
	struct cmp_connection *conn;
	int err;

	if (stream == &oxfw->rx_stream)
		conn = &oxfw->in_conn;
	else
		conn = &oxfw->out_conn;

	err = cmp_connection_establish(conn);
	if (err < 0)
		return err;

	err = amdtp_domain_add_stream(&oxfw->domain, stream,
				      conn->resources.channel, conn->speed);
	if (err < 0) {
		cmp_connection_break(conn);
		return err;
	}

	return 0;
}

static int check_connection_used_by_others(struct snd_oxfw *oxfw,
					   struct amdtp_stream *stream)
{
	struct cmp_connection *conn;
	bool used;
	int err;

	if (stream == &oxfw->tx_stream)
		conn = &oxfw->out_conn;
	else
		conn = &oxfw->in_conn;

	err = cmp_connection_check_used(conn, &used);
	if ((err >= 0) && used && !amdtp_stream_running(stream)) {
		dev_err(&oxfw->unit->device,
			"Connection established by others: %cPCR[%d]\n",
			(conn->direction == CMP_OUTPUT) ? 'o' : 'i',
			conn->pcr_index);
		err = -EBUSY;
	}

	return err;
}

static int init_stream(struct snd_oxfw *oxfw, struct amdtp_stream *stream)
{
	struct cmp_connection *conn;
	enum cmp_direction c_dir;
	enum amdtp_stream_direction s_dir;
	int err;

	if (stream == &oxfw->tx_stream) {
		conn = &oxfw->out_conn;
		c_dir = CMP_OUTPUT;
		s_dir = AMDTP_IN_STREAM;
	} else {
		conn = &oxfw->in_conn;
		c_dir = CMP_INPUT;
		s_dir = AMDTP_OUT_STREAM;
	}

	err = cmp_connection_init(conn, oxfw->unit, c_dir, 0);
	if (err < 0)
		return err;

	err = amdtp_am824_init(stream, oxfw->unit, s_dir, CIP_NONBLOCKING);
	if (err < 0) {
		cmp_connection_destroy(conn);
		return err;
	}

	/*
	 * OXFW starts to transmit packets with non-zero dbc.
	 * OXFW postpone transferring packets till handling any asynchronous
	 * packets. As a result, next isochronous packet includes more data
	 * blocks than IEC 61883-6 defines.
	 */
	if (stream == &oxfw->tx_stream) {
		oxfw->tx_stream.flags |= CIP_JUMBO_PAYLOAD;
		if (oxfw->wrong_dbs)
			oxfw->tx_stream.flags |= CIP_WRONG_DBS;
	}

	return 0;
}

static int keep_resources(struct snd_oxfw *oxfw, struct amdtp_stream *stream)
{
	enum avc_general_plug_dir dir;
	u8 **formats;
	struct snd_oxfw_stream_formation formation;
	struct cmp_connection *conn;
	int i;
	int err;

	if (stream == &oxfw->rx_stream) {
		dir = AVC_GENERAL_PLUG_DIR_IN;
		formats = oxfw->rx_stream_formats;
		conn = &oxfw->in_conn;
	} else {
		dir = AVC_GENERAL_PLUG_DIR_OUT;
		formats = oxfw->tx_stream_formats;
		conn = &oxfw->out_conn;
	}

	err = snd_oxfw_stream_get_current_formation(oxfw, dir, &formation);
	if (err < 0)
		return err;

	for (i = 0; i < SND_OXFW_STREAM_FORMAT_ENTRIES; i++) {
		struct snd_oxfw_stream_formation fmt;

		if (formats[i] == NULL)
			break;

		err = snd_oxfw_stream_parse_format(formats[i], &fmt);
		if (err < 0)
			return err;

		if (fmt.rate == formation.rate && fmt.pcm == formation.pcm &&
		    fmt.midi == formation.midi)
			break;
	}
	if (i == SND_OXFW_STREAM_FORMAT_ENTRIES)
		return -EINVAL;

	// The stream should have one pcm channels at least.
	if (formation.pcm == 0)
		return -EINVAL;

	err = amdtp_am824_set_parameters(stream, formation.rate, formation.pcm,
					 formation.midi * 8, false);
	if (err < 0)
		return err;

	return cmp_connection_reserve(conn, amdtp_stream_get_max_payload(stream));
}

int snd_oxfw_stream_reserve_duplex(struct snd_oxfw *oxfw,
				   struct amdtp_stream *stream,
				   unsigned int rate, unsigned int pcm_channels,
				   unsigned int frames_per_period,
				   unsigned int frames_per_buffer)
{
	struct snd_oxfw_stream_formation formation;
	enum avc_general_plug_dir dir;
	int err;

	// Considering JACK/FFADO streaming:
	// TODO: This can be removed hwdep functionality becomes popular.
	err = check_connection_used_by_others(oxfw, &oxfw->rx_stream);
	if (err < 0)
		return err;
	if (oxfw->has_output) {
		err = check_connection_used_by_others(oxfw, &oxfw->tx_stream);
		if (err < 0)
			return err;
	}

	if (stream == &oxfw->tx_stream)
		dir = AVC_GENERAL_PLUG_DIR_OUT;
	else
		dir = AVC_GENERAL_PLUG_DIR_IN;

	err = snd_oxfw_stream_get_current_formation(oxfw, dir, &formation);
	if (err < 0)
		return err;
	if (rate == 0) {
		rate = formation.rate;
		pcm_channels = formation.pcm;
	}
	if (formation.rate != rate || formation.pcm != pcm_channels) {
		amdtp_domain_stop(&oxfw->domain);

		cmp_connection_break(&oxfw->in_conn);
		cmp_connection_release(&oxfw->in_conn);

		if (oxfw->has_output) {
			cmp_connection_break(&oxfw->out_conn);
			cmp_connection_release(&oxfw->out_conn);
		}
	}

	if (oxfw->substreams_count == 0 ||
	    formation.rate != rate || formation.pcm != pcm_channels) {
		err = set_stream_format(oxfw, stream, rate, pcm_channels);
		if (err < 0) {
			dev_err(&oxfw->unit->device,
				"fail to set stream format: %d\n", err);
			return err;
		}

		err = keep_resources(oxfw, &oxfw->rx_stream);
		if (err < 0)
			return err;

		if (oxfw->has_output) {
			err = keep_resources(oxfw, &oxfw->tx_stream);
			if (err < 0) {
				cmp_connection_release(&oxfw->in_conn);
				return err;
			}
		}

		err = amdtp_domain_set_events_per_period(&oxfw->domain,
					frames_per_period, frames_per_buffer);
		if (err < 0) {
			cmp_connection_release(&oxfw->in_conn);
			if (oxfw->has_output)
				cmp_connection_release(&oxfw->out_conn);
			return err;
		}
	}

	return 0;
}

int snd_oxfw_stream_start_duplex(struct snd_oxfw *oxfw)
{
	int err;

	if (oxfw->substreams_count == 0)
		return -EIO;

	if (amdtp_streaming_error(&oxfw->rx_stream) ||
	    amdtp_streaming_error(&oxfw->tx_stream)) {
		amdtp_domain_stop(&oxfw->domain);

		cmp_connection_break(&oxfw->in_conn);
		if (oxfw->has_output)
			cmp_connection_break(&oxfw->out_conn);
	}

	if (!amdtp_stream_running(&oxfw->rx_stream)) {
		err = start_stream(oxfw, &oxfw->rx_stream);
		if (err < 0) {
			dev_err(&oxfw->unit->device,
				"fail to prepare rx stream: %d\n", err);
			goto error;
		}

		if (oxfw->has_output &&
		    !amdtp_stream_running(&oxfw->tx_stream)) {
			err = start_stream(oxfw, &oxfw->tx_stream);
			if (err < 0) {
				dev_err(&oxfw->unit->device,
					"fail to prepare tx stream: %d\n", err);
				goto error;
			}
		}

		err = amdtp_domain_start(&oxfw->domain, 0);
		if (err < 0)
			goto error;

		// Wait first packet.
		if (!amdtp_stream_wait_callback(&oxfw->rx_stream,
						CALLBACK_TIMEOUT)) {
			err = -ETIMEDOUT;
			goto error;
		}

		if (oxfw->has_output) {
			if (!amdtp_stream_wait_callback(&oxfw->tx_stream,
							CALLBACK_TIMEOUT)) {
				err = -ETIMEDOUT;
				goto error;
			}
		}
	}

	return 0;
error:
	amdtp_domain_stop(&oxfw->domain);

	cmp_connection_break(&oxfw->in_conn);
	if (oxfw->has_output)
		cmp_connection_break(&oxfw->out_conn);

	return err;
}

void snd_oxfw_stream_stop_duplex(struct snd_oxfw *oxfw)
{
	if (oxfw->substreams_count == 0) {
		amdtp_domain_stop(&oxfw->domain);

		cmp_connection_break(&oxfw->in_conn);
		cmp_connection_release(&oxfw->in_conn);

		if (oxfw->has_output) {
			cmp_connection_break(&oxfw->out_conn);
			cmp_connection_release(&oxfw->out_conn);
		}
	}
}

static void destroy_stream(struct snd_oxfw *oxfw, struct amdtp_stream *stream)
{
	struct cmp_connection *conn;

	if (stream == &oxfw->tx_stream)
		conn = &oxfw->out_conn;
	else
		conn = &oxfw->in_conn;

	amdtp_stream_destroy(stream);
	cmp_connection_destroy(conn);
}

int snd_oxfw_stream_init_duplex(struct snd_oxfw *oxfw)
{
	int err;

	err = init_stream(oxfw, &oxfw->rx_stream);
	if (err < 0)
		return err;

	if (oxfw->has_output) {
		err = init_stream(oxfw, &oxfw->tx_stream);
		if (err < 0) {
			destroy_stream(oxfw, &oxfw->rx_stream);
			return err;
		}
	}

	err = amdtp_domain_init(&oxfw->domain);
	if (err < 0) {
		destroy_stream(oxfw, &oxfw->rx_stream);
		if (oxfw->has_output)
			destroy_stream(oxfw, &oxfw->tx_stream);
	}

	return err;
}

// This function should be called before starting the stream or after stopping
// the streams.
void snd_oxfw_stream_destroy_duplex(struct snd_oxfw *oxfw)
{
	amdtp_domain_destroy(&oxfw->domain);

	destroy_stream(oxfw, &oxfw->rx_stream);

	if (oxfw->has_output)
		destroy_stream(oxfw, &oxfw->tx_stream);
}

void snd_oxfw_stream_update_duplex(struct snd_oxfw *oxfw)
{
	amdtp_domain_stop(&oxfw->domain);

	cmp_connection_break(&oxfw->in_conn);

	amdtp_stream_pcm_abort(&oxfw->rx_stream);

	if (oxfw->has_output) {
		cmp_connection_break(&oxfw->out_conn);

		amdtp_stream_pcm_abort(&oxfw->tx_stream);
	}
}

int snd_oxfw_stream_get_current_formation(struct snd_oxfw *oxfw,
				enum avc_general_plug_dir dir,
				struct snd_oxfw_stream_formation *formation)
{
	u8 *format;
	unsigned int len;
	int err;

	len = AVC_GENERIC_FRAME_MAXIMUM_BYTES;
	format = kmalloc(len, GFP_KERNEL);
	if (format == NULL)
		return -ENOMEM;

	err = avc_stream_get_format_single(oxfw->unit, dir, 0, format, &len);
	if (err < 0)
		goto end;
	if (len < 3) {
		err = -EIO;
		goto end;
	}

	err = snd_oxfw_stream_parse_format(format, formation);
end:
	kfree(format);
	return err;
}

/*
 * See Table 6.16 - AM824 Stream Format
 *     Figure 6.19 - format_information field for AM824 Compound
 * in AV/C Stream Format Information Specification 1.1 (Apr 2005, 1394TA)
 * Also 'Clause 12 AM824 sequence adaption layers' in IEC 61883-6:2005
 */
int snd_oxfw_stream_parse_format(u8 *format,
				 struct snd_oxfw_stream_formation *formation)
{
	unsigned int i, e, channels, type;

	memset(formation, 0, sizeof(struct snd_oxfw_stream_formation));

	/*
	 * this module can support a hierarchy combination that:
	 *  Root:	Audio and Music (0x90)
	 *  Level 1:	AM824 Compound  (0x40)
	 */
	if ((format[0] != 0x90) || (format[1] != 0x40))
		return -ENXIO;

	/* check the sampling rate */
	for (i = 0; i < ARRAY_SIZE(avc_stream_rate_table); i++) {
		if (format[2] == avc_stream_rate_table[i])
			break;
	}
	if (i == ARRAY_SIZE(avc_stream_rate_table))
		return -ENXIO;

	formation->rate = oxfw_rate_table[i];

	for (e = 0; e < format[4]; e++) {
		channels = format[5 + e * 2];
		type = format[6 + e * 2];

		switch (type) {
		/* IEC 60958 Conformant, currently handled as MBLA */
		case 0x00:
		/* Multi Bit Linear Audio (Raw) */
		case 0x06:
			formation->pcm += channels;
			break;
		/* MIDI Conformant */
		case 0x0d:
			formation->midi = channels;
			break;
		/* IEC 61937-3 to 7 */
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		/* Multi Bit Linear Audio */
		case 0x07:	/* DVD-Audio */
		case 0x0c:	/* High Precision */
		/* One Bit Audio */
		case 0x08:	/* (Plain) Raw */
		case 0x09:	/* (Plain) SACD */
		case 0x0a:	/* (Encoded) Raw */
		case 0x0b:	/* (Encoded) SACD */
		/* SMPTE Time-Code conformant */
		case 0x0e:
		/* Sample Count */
		case 0x0f:
		/* Anciliary Data */
		case 0x10:
		/* Synchronization Stream (Stereo Raw audio) */
		case 0x40:
		/* Don't care */
		case 0xff:
		default:
			return -ENXIO;	/* not supported */
		}
	}

	if (formation->pcm  > AM824_MAX_CHANNELS_FOR_PCM ||
	    formation->midi > AM824_MAX_CHANNELS_FOR_MIDI)
		return -ENXIO;

	return 0;
}

static int
assume_stream_formats(struct snd_oxfw *oxfw, enum avc_general_plug_dir dir,
		      unsigned int pid, u8 *buf, unsigned int *len,
		      u8 **formats)
{
	struct snd_oxfw_stream_formation formation;
	unsigned int i, eid;
	int err;

	/* get format at current sampling rate */
	err = avc_stream_get_format_single(oxfw->unit, dir, pid, buf, len);
	if (err < 0) {
		dev_err(&oxfw->unit->device,
		"fail to get current stream format for isoc %s plug %d:%d\n",
			(dir == AVC_GENERAL_PLUG_DIR_IN) ? "in" : "out",
			pid, err);
		goto end;
	}

	/* parse and set stream format */
	eid = 0;
	err = snd_oxfw_stream_parse_format(buf, &formation);
	if (err < 0)
		goto end;

	formats[eid] = devm_kmemdup(&oxfw->card->card_dev, buf, *len,
				    GFP_KERNEL);
	if (!formats[eid]) {
		err = -ENOMEM;
		goto end;
	}

	/* apply the format for each available sampling rate */
	for (i = 0; i < ARRAY_SIZE(oxfw_rate_table); i++) {
		if (formation.rate == oxfw_rate_table[i])
			continue;

		err = avc_general_inquiry_sig_fmt(oxfw->unit,
						  oxfw_rate_table[i],
						  dir, pid);
		if (err < 0)
			continue;

		eid++;
		formats[eid] = devm_kmemdup(&oxfw->card->card_dev, buf, *len,
					    GFP_KERNEL);
		if (formats[eid] == NULL) {
			err = -ENOMEM;
			goto end;
		}
		formats[eid][2] = avc_stream_rate_table[i];
	}

	err = 0;
	oxfw->assumed = true;
end:
	return err;
}

static int fill_stream_formats(struct snd_oxfw *oxfw,
			       enum avc_general_plug_dir dir,
			       unsigned short pid)
{
	u8 *buf, **formats;
	unsigned int len, eid = 0;
	struct snd_oxfw_stream_formation dummy;
	int err;

	buf = kmalloc(AVC_GENERIC_FRAME_MAXIMUM_BYTES, GFP_KERNEL);
	if (buf == NULL)
		return -ENOMEM;

	if (dir == AVC_GENERAL_PLUG_DIR_OUT)
		formats = oxfw->tx_stream_formats;
	else
		formats = oxfw->rx_stream_formats;

	/* get first entry */
	len = AVC_GENERIC_FRAME_MAXIMUM_BYTES;
	err = avc_stream_get_format_list(oxfw->unit, dir, 0, buf, &len, 0);
	if (err == -ENXIO) {
		/* LIST subfunction is not implemented */
		len = AVC_GENERIC_FRAME_MAXIMUM_BYTES;
		err = assume_stream_formats(oxfw, dir, pid, buf, &len,
					    formats);
		goto end;
	} else if (err < 0) {
		dev_err(&oxfw->unit->device,
			"fail to get stream format %d for isoc %s plug %d:%d\n",
			eid, (dir == AVC_GENERAL_PLUG_DIR_IN) ? "in" : "out",
			pid, err);
		goto end;
	}

	/* LIST subfunction is implemented */
	while (eid < SND_OXFW_STREAM_FORMAT_ENTRIES) {
		/* The format is too short. */
		if (len < 3) {
			err = -EIO;
			break;
		}

		/* parse and set stream format */
		err = snd_oxfw_stream_parse_format(buf, &dummy);
		if (err < 0)
			break;

		formats[eid] = devm_kmemdup(&oxfw->card->card_dev, buf, len,
					    GFP_KERNEL);
		if (!formats[eid]) {
			err = -ENOMEM;
			break;
		}

		/* get next entry */
		len = AVC_GENERIC_FRAME_MAXIMUM_BYTES;
		err = avc_stream_get_format_list(oxfw->unit, dir, 0,
						 buf, &len, ++eid);
		/* No entries remained. */
		if (err == -EINVAL) {
			err = 0;
			break;
		} else if (err < 0) {
			dev_err(&oxfw->unit->device,
			"fail to get stream format %d for isoc %s plug %d:%d\n",
				eid, (dir == AVC_GENERAL_PLUG_DIR_IN) ? "in" :
									"out",
				pid, err);
			break;
		}
	}
end:
	kfree(buf);
	return err;
}

int snd_oxfw_stream_discover(struct snd_oxfw *oxfw)
{
	u8 plugs[AVC_PLUG_INFO_BUF_BYTES];
	struct snd_oxfw_stream_formation formation;
	u8 *format;
	unsigned int i;
	int err;

	/* the number of plugs for isoc in/out, ext in/out  */
	err = avc_general_get_plug_info(oxfw->unit, 0x1f, 0x07, 0x00, plugs);
	if (err < 0) {
		dev_err(&oxfw->unit->device,
		"fail to get info for isoc/external in/out plugs: %d\n",
			err);
		goto end;
	} else if ((plugs[0] == 0) && (plugs[1] == 0)) {
		err = -ENXIO;
		goto end;
	}

	/* use oPCR[0] if exists */
	if (plugs[1] > 0) {
		err = fill_stream_formats(oxfw, AVC_GENERAL_PLUG_DIR_OUT, 0);
		if (err < 0) {
			if (err != -ENXIO)
				return err;

			// The oPCR is not available for isoc communication.
			err = 0;
		} else {
			for (i = 0; i < SND_OXFW_STREAM_FORMAT_ENTRIES; i++) {
				format = oxfw->tx_stream_formats[i];
				if (format == NULL)
					continue;
				err = snd_oxfw_stream_parse_format(format,
								   &formation);
				if (err < 0)
					continue;

				/* Add one MIDI port. */
				if (formation.midi > 0)
					oxfw->midi_input_ports = 1;
			}

			oxfw->has_output = true;
		}
	}

	/* use iPCR[0] if exists */
	if (plugs[0] > 0) {
		err = fill_stream_formats(oxfw, AVC_GENERAL_PLUG_DIR_IN, 0);
		if (err < 0) {
			if (err != -ENXIO)
				return err;

			// The iPCR is not available for isoc communication.
			err = 0;
		} else {
			for (i = 0; i < SND_OXFW_STREAM_FORMAT_ENTRIES; i++) {
				format = oxfw->rx_stream_formats[i];
				if (format == NULL)
					continue;
				err = snd_oxfw_stream_parse_format(format,
								   &formation);
				if (err < 0)
					continue;

				/* Add one MIDI port. */
				if (formation.midi > 0)
					oxfw->midi_output_ports = 1;
			}

			oxfw->has_input = true;
		}
	}
end:
	return err;
}

void snd_oxfw_stream_lock_changed(struct snd_oxfw *oxfw)
{
	oxfw->dev_lock_changed = true;
	wake_up(&oxfw->hwdep_wait);
}

int snd_oxfw_stream_lock_try(struct snd_oxfw *oxfw)
{
	int err;

	spin_lock_irq(&oxfw->lock);

	/* user land lock this */
	if (oxfw->dev_lock_count < 0) {
		err = -EBUSY;
		goto end;
	}

	/* this is the first time */
	if (oxfw->dev_lock_count++ == 0)
		snd_oxfw_stream_lock_changed(oxfw);
	err = 0;
end:
	spin_unlock_irq(&oxfw->lock);
	return err;
}

void snd_oxfw_stream_lock_release(struct snd_oxfw *oxfw)
{
	spin_lock_irq(&oxfw->lock);

	if (WARN_ON(oxfw->dev_lock_count <= 0))
		goto end;
	if (--oxfw->dev_lock_count == 0)
		snd_oxfw_stream_lock_changed(oxfw);
end:
	spin_unlock_irq(&oxfw->lock);
}
