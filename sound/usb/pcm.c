// SPDX-License-Identifier: GPL-2.0-or-later
/*
 */

#include <linux/init.h>
#include <linux/slab.h>
#include <linux/bitrev.h>
#include <linux/ratelimit.h>
#include <linux/usb.h>
#include <linux/usb/audio.h>
#include <linux/usb/audio-v2.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>

#include "usbaudio.h"
#include "card.h"
#include "quirks.h"
#include "endpoint.h"
#include "helper.h"
#include "pcm.h"
#include "clock.h"
#include "power.h"
#include "media.h"
#include "implicit.h"

#define SUBSTREAM_FLAG_DATA_EP_STARTED	0
#define SUBSTREAM_FLAG_SYNC_EP_STARTED	1

/* return the estimated delay based on USB frame counters */
snd_pcm_uframes_t snd_usb_pcm_delay(struct snd_usb_substream *subs,
				    unsigned int rate)
{
	int current_frame_number;
	int frame_diff;
	int est_delay;

	if (!subs->last_delay)
		return 0; /* short path */

	current_frame_number = usb_get_current_frame_number(subs->dev);
	/*
	 * HCD implementations use different widths, use lower 8 bits.
	 * The delay will be managed up to 256ms, which is more than
	 * enough
	 */
	frame_diff = (current_frame_number - subs->last_frame_number) & 0xff;

	/* Approximation based on number of samples per USB frame (ms),
	   some truncation for 44.1 but the estimate is good enough */
	est_delay =  frame_diff * rate / 1000;
	if (subs->direction == SNDRV_PCM_STREAM_PLAYBACK)
		est_delay = subs->last_delay - est_delay;
	else
		est_delay = subs->last_delay + est_delay;

	if (est_delay < 0)
		est_delay = 0;
	return est_delay;
}

/*
 * return the current pcm pointer.  just based on the hwptr_done value.
 */
static snd_pcm_uframes_t snd_usb_pcm_pointer(struct snd_pcm_substream *substream)
{
	struct snd_usb_substream *subs = substream->runtime->private_data;
	unsigned int hwptr_done;

	if (atomic_read(&subs->stream->chip->shutdown))
		return SNDRV_PCM_POS_XRUN;
	spin_lock(&subs->lock);
	hwptr_done = subs->hwptr_done;
	substream->runtime->delay = snd_usb_pcm_delay(subs,
						substream->runtime->rate);
	spin_unlock(&subs->lock);
	return hwptr_done / (substream->runtime->frame_bits >> 3);
}

/*
 * find a matching audio format
 */
static const struct audioformat *
find_format(struct list_head *fmt_list_head, snd_pcm_format_t format,
	    unsigned int rate, unsigned int channels, bool strict_match,
	    struct snd_usb_substream *subs)
{
	const struct audioformat *fp;
	const struct audioformat *found = NULL;
	int cur_attr = 0, attr;

	list_for_each_entry(fp, fmt_list_head, list) {
		if (strict_match) {
			if (!(fp->formats & pcm_format_to_bits(format)))
				continue;
			if (fp->channels != channels)
				continue;
		}
		if (rate < fp->rate_min || rate > fp->rate_max)
			continue;
		if (!(fp->rates & SNDRV_PCM_RATE_CONTINUOUS)) {
			unsigned int i;
			for (i = 0; i < fp->nr_rates; i++)
				if (fp->rate_table[i] == rate)
					break;
			if (i >= fp->nr_rates)
				continue;
		}
		attr = fp->ep_attr & USB_ENDPOINT_SYNCTYPE;
		if (!found) {
			found = fp;
			cur_attr = attr;
			continue;
		}
		/* avoid async out and adaptive in if the other method
		 * supports the same format.
		 * this is a workaround for the case like
		 * M-audio audiophile USB.
		 */
		if (subs && attr != cur_attr) {
			if ((attr == USB_ENDPOINT_SYNC_ASYNC &&
			     subs->direction == SNDRV_PCM_STREAM_PLAYBACK) ||
			    (attr == USB_ENDPOINT_SYNC_ADAPTIVE &&
			     subs->direction == SNDRV_PCM_STREAM_CAPTURE))
				continue;
			if ((cur_attr == USB_ENDPOINT_SYNC_ASYNC &&
			     subs->direction == SNDRV_PCM_STREAM_PLAYBACK) ||
			    (cur_attr == USB_ENDPOINT_SYNC_ADAPTIVE &&
			     subs->direction == SNDRV_PCM_STREAM_CAPTURE)) {
				found = fp;
				cur_attr = attr;
				continue;
			}
		}
		/* find the format with the largest max. packet size */
		if (fp->maxpacksize > found->maxpacksize) {
			found = fp;
			cur_attr = attr;
		}
	}
	return found;
}

static const struct audioformat *
find_substream_format(struct snd_usb_substream *subs,
		      const struct snd_pcm_hw_params *params)
{
	return find_format(&subs->fmt_list, params_format(params),
			   params_rate(params), params_channels(params),
			   true, subs);
}

static int init_pitch_v1(struct snd_usb_audio *chip, int ep)
{
	struct usb_device *dev = chip->dev;
	unsigned char data[1];
	int err;

	data[0] = 1;
	err = snd_usb_ctl_msg(dev, usb_sndctrlpipe(dev, 0), UAC_SET_CUR,
			      USB_TYPE_CLASS|USB_RECIP_ENDPOINT|USB_DIR_OUT,
			      UAC_EP_CS_ATTR_PITCH_CONTROL << 8, ep,
			      data, sizeof(data));
	return err;
}

static int init_pitch_v2(struct snd_usb_audio *chip, int ep)
{
	struct usb_device *dev = chip->dev;
	unsigned char data[1];
	int err;

	data[0] = 1;
	err = snd_usb_ctl_msg(dev, usb_sndctrlpipe(dev, 0), UAC2_CS_CUR,
			      USB_TYPE_CLASS | USB_RECIP_ENDPOINT | USB_DIR_OUT,
			      UAC2_EP_CS_PITCH << 8, 0,
			      data, sizeof(data));
	return err;
}

/*
 * initialize the pitch control and sample rate
 */
int snd_usb_init_pitch(struct snd_usb_audio *chip,
		       const struct audioformat *fmt)
{
	int err;

	/* if endpoint doesn't have pitch control, bail out */
	if (!(fmt->attributes & UAC_EP_CS_ATTR_PITCH_CONTROL))
		return 0;

	usb_audio_dbg(chip, "enable PITCH for EP 0x%x\n", fmt->endpoint);

	switch (fmt->protocol) {
	case UAC_VERSION_1:
		err = init_pitch_v1(chip, fmt->endpoint);
		break;
	case UAC_VERSION_2:
		err = init_pitch_v2(chip, fmt->endpoint);
		break;
	default:
		return 0;
	}

	if (err < 0) {
		usb_audio_err(chip, "failed to enable PITCH for EP 0x%x\n",
			      fmt->endpoint);
		return err;
	}

	return 0;
}

static bool stop_endpoints(struct snd_usb_substream *subs)
{
	bool stopped = 0;

	if (test_and_clear_bit(SUBSTREAM_FLAG_SYNC_EP_STARTED, &subs->flags)) {
		snd_usb_endpoint_stop(subs->sync_endpoint);
		stopped = true;
	}
	if (test_and_clear_bit(SUBSTREAM_FLAG_DATA_EP_STARTED, &subs->flags)) {
		snd_usb_endpoint_stop(subs->data_endpoint);
		stopped = true;
	}
	return stopped;
}

static int start_endpoints(struct snd_usb_substream *subs)
{
	int err;

	if (!subs->data_endpoint)
		return -EINVAL;

	if (!test_and_set_bit(SUBSTREAM_FLAG_DATA_EP_STARTED, &subs->flags)) {
		err = snd_usb_endpoint_start(subs->data_endpoint);
		if (err < 0) {
			clear_bit(SUBSTREAM_FLAG_DATA_EP_STARTED, &subs->flags);
			goto error;
		}
	}

	if (subs->sync_endpoint &&
	    !test_and_set_bit(SUBSTREAM_FLAG_SYNC_EP_STARTED, &subs->flags)) {
		err = snd_usb_endpoint_start(subs->sync_endpoint);
		if (err < 0) {
			clear_bit(SUBSTREAM_FLAG_SYNC_EP_STARTED, &subs->flags);
			goto error;
		}
	}

	return 0;

 error:
	stop_endpoints(subs);
	return err;
}

static void sync_pending_stops(struct snd_usb_substream *subs)
{
	snd_usb_endpoint_sync_pending_stop(subs->sync_endpoint);
	snd_usb_endpoint_sync_pending_stop(subs->data_endpoint);
}

/* PCM sync_stop callback */
static int snd_usb_pcm_sync_stop(struct snd_pcm_substream *substream)
{
	struct snd_usb_substream *subs = substream->runtime->private_data;

	sync_pending_stops(subs);
	return 0;
}

/* Set up sync endpoint */
int snd_usb_audioformat_set_sync_ep(struct snd_usb_audio *chip,
				    struct audioformat *fmt)
{
	struct usb_device *dev = chip->dev;
	struct usb_host_interface *alts;
	struct usb_interface_descriptor *altsd;
	unsigned int ep, attr, sync_attr;
	bool is_playback;
	int err;

	alts = snd_usb_get_host_interface(chip, fmt->iface, fmt->altsetting);
	if (!alts)
		return 0;
	altsd = get_iface_desc(alts);

	err = snd_usb_parse_implicit_fb_quirk(chip, fmt, alts);
	if (err > 0)
		return 0; /* matched */

	/*
	 * Generic sync EP handling
	 */

	if (altsd->bNumEndpoints < 2)
		return 0;

	is_playback = !(get_endpoint(alts, 0)->bEndpointAddress & USB_DIR_IN);
	attr = fmt->ep_attr & USB_ENDPOINT_SYNCTYPE;
	if ((is_playback && (attr == USB_ENDPOINT_SYNC_SYNC ||
			     attr == USB_ENDPOINT_SYNC_ADAPTIVE)) ||
	    (!is_playback && attr != USB_ENDPOINT_SYNC_ADAPTIVE))
		return 0;

	sync_attr = get_endpoint(alts, 1)->bmAttributes;

	/*
	 * In case of illegal SYNC_NONE for OUT endpoint, we keep going to see
	 * if we don't find a sync endpoint, as on M-Audio Transit. In case of
	 * error fall back to SYNC mode and don't create sync endpoint
	 */

	/* check sync-pipe endpoint */
	/* ... and check descriptor size before accessing bSynchAddress
	   because there is a version of the SB Audigy 2 NX firmware lacking
	   the audio fields in the endpoint descriptors */
	if ((sync_attr & USB_ENDPOINT_XFERTYPE_MASK) != USB_ENDPOINT_XFER_ISOC ||
	    (get_endpoint(alts, 1)->bLength >= USB_DT_ENDPOINT_AUDIO_SIZE &&
	     get_endpoint(alts, 1)->bSynchAddress != 0)) {
		dev_err(&dev->dev,
			"%d:%d : invalid sync pipe. bmAttributes %02x, bLength %d, bSynchAddress %02x\n",
			   fmt->iface, fmt->altsetting,
			   get_endpoint(alts, 1)->bmAttributes,
			   get_endpoint(alts, 1)->bLength,
			   get_endpoint(alts, 1)->bSynchAddress);
		if (is_playback && attr == USB_ENDPOINT_SYNC_NONE)
			return 0;
		return -EINVAL;
	}
	ep = get_endpoint(alts, 1)->bEndpointAddress;
	if (get_endpoint(alts, 0)->bLength >= USB_DT_ENDPOINT_AUDIO_SIZE &&
	    get_endpoint(alts, 0)->bSynchAddress != 0 &&
	    ((is_playback && ep != (unsigned int)(get_endpoint(alts, 0)->bSynchAddress | USB_DIR_IN)) ||
	     (!is_playback && ep != (unsigned int)(get_endpoint(alts, 0)->bSynchAddress & ~USB_DIR_IN)))) {
		dev_err(&dev->dev,
			"%d:%d : invalid sync pipe. is_playback %d, ep %02x, bSynchAddress %02x\n",
			   fmt->iface, fmt->altsetting,
			   is_playback, ep, get_endpoint(alts, 0)->bSynchAddress);
		if (is_playback && attr == USB_ENDPOINT_SYNC_NONE)
			return 0;
		return -EINVAL;
	}

	fmt->sync_ep = ep;
	fmt->sync_iface = altsd->bInterfaceNumber;
	fmt->sync_altsetting = altsd->bAlternateSetting;
	fmt->sync_ep_idx = 1;
	if ((sync_attr & USB_ENDPOINT_USAGE_MASK) == USB_ENDPOINT_USAGE_IMPLICIT_FB)
		fmt->implicit_fb = 1;

	dev_dbg(&dev->dev, "%d:%d: found sync_ep=0x%x, iface=%d, alt=%d, implicit_fb=%d\n",
		fmt->iface, fmt->altsetting, fmt->sync_ep, fmt->sync_iface,
		fmt->sync_altsetting, fmt->implicit_fb);

	return 0;
}

static int snd_usb_pcm_change_state(struct snd_usb_substream *subs, int state)
{
	int ret;

	if (!subs->str_pd)
		return 0;

	ret = snd_usb_power_domain_set(subs->stream->chip, subs->str_pd, state);
	if (ret < 0) {
		dev_err(&subs->dev->dev,
			"Cannot change Power Domain ID: %d to state: %d. Err: %d\n",
			subs->str_pd->pd_id, state, ret);
		return ret;
	}

	return 0;
}

int snd_usb_pcm_suspend(struct snd_usb_stream *as)
{
	int ret;

	ret = snd_usb_pcm_change_state(&as->substream[0], UAC3_PD_STATE_D2);
	if (ret < 0)
		return ret;

	ret = snd_usb_pcm_change_state(&as->substream[1], UAC3_PD_STATE_D2);
	if (ret < 0)
		return ret;

	return 0;
}

int snd_usb_pcm_resume(struct snd_usb_stream *as)
{
	int ret;

	ret = snd_usb_pcm_change_state(&as->substream[0], UAC3_PD_STATE_D1);
	if (ret < 0)
		return ret;

	ret = snd_usb_pcm_change_state(&as->substream[1], UAC3_PD_STATE_D1);
	if (ret < 0)
		return ret;

	return 0;
}

static void close_endpoints(struct snd_usb_audio *chip,
			    struct snd_usb_substream *subs)
{
	if (subs->data_endpoint) {
		snd_usb_endpoint_set_sync(chip, subs->data_endpoint, NULL);
		snd_usb_endpoint_close(chip, subs->data_endpoint);
		subs->data_endpoint = NULL;
	}

	if (subs->sync_endpoint) {
		snd_usb_endpoint_close(chip, subs->sync_endpoint);
		subs->sync_endpoint = NULL;
	}
}

static int configure_endpoints(struct snd_usb_audio *chip,
			       struct snd_usb_substream *subs)
{
	int err;

	if (subs->data_endpoint->need_setup) {
		/* stop any running stream beforehand */
		if (stop_endpoints(subs))
			sync_pending_stops(subs);
		err = snd_usb_endpoint_configure(chip, subs->data_endpoint);
		if (err < 0)
			return err;
		snd_usb_set_format_quirk(subs, subs->cur_audiofmt);
	}

	if (subs->sync_endpoint) {
		err = snd_usb_endpoint_configure(chip, subs->sync_endpoint);
		if (err < 0)
			return err;
	}

	return 0;
}

/*
 * hw_params callback
 *
 * allocate a buffer and set the given audio format.
 *
 * so far we use a physically linear buffer although packetize transfer
 * doesn't need a continuous area.
 * if sg buffer is supported on the later version of alsa, we'll follow
 * that.
 */
static int snd_usb_hw_params(struct snd_pcm_substream *substream,
			     struct snd_pcm_hw_params *hw_params)
{
	struct snd_usb_substream *subs = substream->runtime->private_data;
	struct snd_usb_audio *chip = subs->stream->chip;
	const struct audioformat *fmt;
	const struct audioformat *sync_fmt;
	int ret;

	ret = snd_media_start_pipeline(subs);
	if (ret)
		return ret;

	fmt = find_substream_format(subs, hw_params);
	if (!fmt) {
		usb_audio_dbg(chip,
			      "cannot find format: format=%s, rate=%d, channels=%d\n",
			      snd_pcm_format_name(params_format(hw_params)),
			      params_rate(hw_params), params_channels(hw_params));
		ret = -EINVAL;
		goto stop_pipeline;
	}

	if (fmt->implicit_fb) {
		sync_fmt = snd_usb_find_implicit_fb_sync_format(chip, fmt,
								hw_params,
								!substream->stream);
		if (!sync_fmt) {
			usb_audio_dbg(chip,
				      "cannot find sync format: ep=0x%x, iface=%d:%d, format=%s, rate=%d, channels=%d\n",
				      fmt->sync_ep, fmt->sync_iface,
				      fmt->sync_altsetting,
				      snd_pcm_format_name(params_format(hw_params)),
				      params_rate(hw_params), params_channels(hw_params));
			ret = -EINVAL;
			goto stop_pipeline;
		}
	} else {
		sync_fmt = fmt;
	}

	ret = snd_usb_lock_shutdown(chip);
	if (ret < 0)
		goto stop_pipeline;

	ret = snd_usb_pcm_change_state(subs, UAC3_PD_STATE_D0);
	if (ret < 0)
		goto unlock;

	if (subs->data_endpoint) {
		if (snd_usb_endpoint_compatible(chip, subs->data_endpoint,
						fmt, hw_params))
			goto unlock;
		close_endpoints(chip, subs);
	}

	subs->data_endpoint = snd_usb_endpoint_open(chip, fmt, hw_params, false);
	if (!subs->data_endpoint) {
		ret = -EINVAL;
		goto unlock;
	}

	if (fmt->sync_ep) {
		subs->sync_endpoint = snd_usb_endpoint_open(chip, sync_fmt,
							    hw_params,
							    fmt == sync_fmt);
		if (!subs->sync_endpoint) {
			ret = -EINVAL;
			goto unlock;
		}

		snd_usb_endpoint_set_sync(chip, subs->data_endpoint,
					  subs->sync_endpoint);
	}

	mutex_lock(&chip->mutex);
	subs->cur_audiofmt = fmt;
	mutex_unlock(&chip->mutex);

	ret = configure_endpoints(chip, subs);

 unlock:
	if (ret < 0)
		close_endpoints(chip, subs);

	snd_usb_unlock_shutdown(chip);
 stop_pipeline:
	if (ret < 0)
		snd_media_stop_pipeline(subs);

	return ret;
}

/*
 * hw_free callback
 *
 * reset the audio format and release the buffer
 */
static int snd_usb_hw_free(struct snd_pcm_substream *substream)
{
	struct snd_usb_substream *subs = substream->runtime->private_data;
	struct snd_usb_audio *chip = subs->stream->chip;

	snd_media_stop_pipeline(subs);
	mutex_lock(&chip->mutex);
	subs->cur_audiofmt = NULL;
	mutex_unlock(&chip->mutex);
	if (!snd_usb_lock_shutdown(chip)) {
		if (stop_endpoints(subs))
			sync_pending_stops(subs);
		close_endpoints(chip, subs);
		snd_usb_unlock_shutdown(chip);
	}

	return 0;
}

/*
 * prepare callback
 *
 * only a few subtle things...
 */
static int snd_usb_pcm_prepare(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_usb_substream *subs = runtime->private_data;
	struct snd_usb_audio *chip = subs->stream->chip;
	int ret;

	ret = snd_usb_lock_shutdown(chip);
	if (ret < 0)
		return ret;
	if (snd_BUG_ON(!subs->data_endpoint)) {
		ret = -EIO;
		goto unlock;
	}

	ret = configure_endpoints(chip, subs);
	if (ret < 0)
		goto unlock;

	/* reset the pointer */
	subs->hwptr_done = 0;
	subs->transfer_done = 0;
	subs->last_delay = 0;
	subs->last_frame_number = 0;
	runtime->delay = 0;

	/* for playback, submit the URBs now; otherwise, the first hwptr_done
	 * updates for all URBs would happen at the same time when starting */
	if (subs->direction == SNDRV_PCM_STREAM_PLAYBACK)
		ret = start_endpoints(subs);

 unlock:
	snd_usb_unlock_shutdown(chip);
	return ret;
}

/*
 * h/w constraints
 */

#ifdef HW_CONST_DEBUG
#define hwc_debug(fmt, args...) pr_debug(fmt, ##args)
#else
#define hwc_debug(fmt, args...) do { } while(0)
#endif

static const struct snd_pcm_hardware snd_usb_hardware =
{
	.info =			SNDRV_PCM_INFO_MMAP |
				SNDRV_PCM_INFO_MMAP_VALID |
				SNDRV_PCM_INFO_BATCH |
				SNDRV_PCM_INFO_INTERLEAVED |
				SNDRV_PCM_INFO_BLOCK_TRANSFER |
				SNDRV_PCM_INFO_PAUSE,
	.channels_min =		1,
	.channels_max =		256,
	.buffer_bytes_max =	1024 * 1024,
	.period_bytes_min =	64,
	.period_bytes_max =	512 * 1024,
	.periods_min =		2,
	.periods_max =		1024,
};

static int hw_check_valid_format(struct snd_usb_substream *subs,
				 struct snd_pcm_hw_params *params,
				 const struct audioformat *fp)
{
	struct snd_interval *it = hw_param_interval(params, SNDRV_PCM_HW_PARAM_RATE);
	struct snd_interval *ct = hw_param_interval(params, SNDRV_PCM_HW_PARAM_CHANNELS);
	struct snd_mask *fmts = hw_param_mask(params, SNDRV_PCM_HW_PARAM_FORMAT);
	struct snd_interval *pt = hw_param_interval(params, SNDRV_PCM_HW_PARAM_PERIOD_TIME);
	struct snd_mask check_fmts;
	unsigned int ptime;

	/* check the format */
	snd_mask_none(&check_fmts);
	check_fmts.bits[0] = (u32)fp->formats;
	check_fmts.bits[1] = (u32)(fp->formats >> 32);
	snd_mask_intersect(&check_fmts, fmts);
	if (snd_mask_empty(&check_fmts)) {
		hwc_debug("   > check: no supported format 0x%llx\n", fp->formats);
		return 0;
	}
	/* check the channels */
	if (fp->channels < ct->min || fp->channels > ct->max) {
		hwc_debug("   > check: no valid channels %d (%d/%d)\n", fp->channels, ct->min, ct->max);
		return 0;
	}
	/* check the rate is within the range */
	if (fp->rate_min > it->max || (fp->rate_min == it->max && it->openmax)) {
		hwc_debug("   > check: rate_min %d > max %d\n", fp->rate_min, it->max);
		return 0;
	}
	if (fp->rate_max < it->min || (fp->rate_max == it->min && it->openmin)) {
		hwc_debug("   > check: rate_max %d < min %d\n", fp->rate_max, it->min);
		return 0;
	}
	/* check whether the period time is >= the data packet interval */
	if (subs->speed != USB_SPEED_FULL) {
		ptime = 125 * (1 << fp->datainterval);
		if (ptime > pt->max || (ptime == pt->max && pt->openmax)) {
			hwc_debug("   > check: ptime %u > max %u\n", ptime, pt->max);
			return 0;
		}
	}
	return 1;
}

static int apply_hw_params_minmax(struct snd_interval *it, unsigned int rmin,
				  unsigned int rmax)
{
	int changed;

	if (rmin > rmax) {
		hwc_debug("  --> get empty\n");
		it->empty = 1;
		return -EINVAL;
	}

	changed = 0;
	if (it->min < rmin) {
		it->min = rmin;
		it->openmin = 0;
		changed = 1;
	}
	if (it->max > rmax) {
		it->max = rmax;
		it->openmax = 0;
		changed = 1;
	}
	if (snd_interval_checkempty(it)) {
		it->empty = 1;
		return -EINVAL;
	}
	hwc_debug("  --> (%d, %d) (changed = %d)\n", it->min, it->max, changed);
	return changed;
}

static int hw_rule_rate(struct snd_pcm_hw_params *params,
			struct snd_pcm_hw_rule *rule)
{
	struct snd_usb_substream *subs = rule->private;
	const struct audioformat *fp;
	struct snd_interval *it = hw_param_interval(params, SNDRV_PCM_HW_PARAM_RATE);
	unsigned int rmin, rmax, r;
	int i;

	hwc_debug("hw_rule_rate: (%d,%d)\n", it->min, it->max);
	rmin = UINT_MAX;
	rmax = 0;
	list_for_each_entry(fp, &subs->fmt_list, list) {
		if (!hw_check_valid_format(subs, params, fp))
			continue;
		if (fp->rate_table && fp->nr_rates) {
			for (i = 0; i < fp->nr_rates; i++) {
				r = fp->rate_table[i];
				if (!snd_interval_test(it, r))
					continue;
				rmin = min(rmin, r);
				rmax = max(rmax, r);
			}
		} else {
			rmin = min(rmin, fp->rate_min);
			rmax = max(rmax, fp->rate_max);
		}
	}

	return apply_hw_params_minmax(it, rmin, rmax);
}


static int hw_rule_channels(struct snd_pcm_hw_params *params,
			    struct snd_pcm_hw_rule *rule)
{
	struct snd_usb_substream *subs = rule->private;
	const struct audioformat *fp;
	struct snd_interval *it = hw_param_interval(params, SNDRV_PCM_HW_PARAM_CHANNELS);
	unsigned int rmin, rmax;

	hwc_debug("hw_rule_channels: (%d,%d)\n", it->min, it->max);
	rmin = UINT_MAX;
	rmax = 0;
	list_for_each_entry(fp, &subs->fmt_list, list) {
		if (!hw_check_valid_format(subs, params, fp))
			continue;
		rmin = min(rmin, fp->channels);
		rmax = max(rmax, fp->channels);
	}

	return apply_hw_params_minmax(it, rmin, rmax);
}

static int apply_hw_params_format_bits(struct snd_mask *fmt, u64 fbits)
{
	u32 oldbits[2];
	int changed;

	oldbits[0] = fmt->bits[0];
	oldbits[1] = fmt->bits[1];
	fmt->bits[0] &= (u32)fbits;
	fmt->bits[1] &= (u32)(fbits >> 32);
	if (!fmt->bits[0] && !fmt->bits[1]) {
		hwc_debug("  --> get empty\n");
		return -EINVAL;
	}
	changed = (oldbits[0] != fmt->bits[0] || oldbits[1] != fmt->bits[1]);
	hwc_debug("  --> %x:%x (changed = %d)\n", fmt->bits[0], fmt->bits[1], changed);
	return changed;
}

static int hw_rule_format(struct snd_pcm_hw_params *params,
			  struct snd_pcm_hw_rule *rule)
{
	struct snd_usb_substream *subs = rule->private;
	const struct audioformat *fp;
	struct snd_mask *fmt = hw_param_mask(params, SNDRV_PCM_HW_PARAM_FORMAT);
	u64 fbits;

	hwc_debug("hw_rule_format: %x:%x\n", fmt->bits[0], fmt->bits[1]);
	fbits = 0;
	list_for_each_entry(fp, &subs->fmt_list, list) {
		if (!hw_check_valid_format(subs, params, fp))
			continue;
		fbits |= fp->formats;
	}
	return apply_hw_params_format_bits(fmt, fbits);
}

static int hw_rule_period_time(struct snd_pcm_hw_params *params,
			       struct snd_pcm_hw_rule *rule)
{
	struct snd_usb_substream *subs = rule->private;
	const struct audioformat *fp;
	struct snd_interval *it;
	unsigned char min_datainterval;
	unsigned int pmin;

	it = hw_param_interval(params, SNDRV_PCM_HW_PARAM_PERIOD_TIME);
	hwc_debug("hw_rule_period_time: (%u,%u)\n", it->min, it->max);
	min_datainterval = 0xff;
	list_for_each_entry(fp, &subs->fmt_list, list) {
		if (!hw_check_valid_format(subs, params, fp))
			continue;
		min_datainterval = min(min_datainterval, fp->datainterval);
	}
	if (min_datainterval == 0xff) {
		hwc_debug("  --> get empty\n");
		it->empty = 1;
		return -EINVAL;
	}
	pmin = 125 * (1 << min_datainterval);

	return apply_hw_params_minmax(it, pmin, UINT_MAX);
}

/* get the EP or the sync EP for implicit fb when it's already set up */
static const struct snd_usb_endpoint *
get_sync_ep_from_substream(struct snd_usb_substream *subs)
{
	struct snd_usb_audio *chip = subs->stream->chip;
	const struct audioformat *fp;
	const struct snd_usb_endpoint *ep;

	list_for_each_entry(fp, &subs->fmt_list, list) {
		ep = snd_usb_get_endpoint(chip, fp->endpoint);
		if (ep && ep->cur_audiofmt) {
			/* if EP is already opened solely for this substream,
			 * we still allow us to change the parameter; otherwise
			 * this substream has to follow the existing parameter
			 */
			if (ep->cur_audiofmt != subs->cur_audiofmt || ep->opened > 1)
				return ep;
		}
		if (!fp->implicit_fb)
			continue;
		/* for the implicit fb, check the sync ep as well */
		ep = snd_usb_get_endpoint(chip, fp->sync_ep);
		if (ep && ep->cur_audiofmt)
			return ep;
	}
	return NULL;
}

/* additional hw constraints for implicit feedback mode */
static int hw_rule_format_implicit_fb(struct snd_pcm_hw_params *params,
				      struct snd_pcm_hw_rule *rule)
{
	struct snd_usb_substream *subs = rule->private;
	const struct snd_usb_endpoint *ep;
	struct snd_mask *fmt = hw_param_mask(params, SNDRV_PCM_HW_PARAM_FORMAT);

	ep = get_sync_ep_from_substream(subs);
	if (!ep)
		return 0;

	hwc_debug("applying %s\n", __func__);
	return apply_hw_params_format_bits(fmt, pcm_format_to_bits(ep->cur_format));
}

static int hw_rule_rate_implicit_fb(struct snd_pcm_hw_params *params,
				    struct snd_pcm_hw_rule *rule)
{
	struct snd_usb_substream *subs = rule->private;
	const struct snd_usb_endpoint *ep;
	struct snd_interval *it;

	ep = get_sync_ep_from_substream(subs);
	if (!ep)
		return 0;

	hwc_debug("applying %s\n", __func__);
	it = hw_param_interval(params, SNDRV_PCM_HW_PARAM_RATE);
	return apply_hw_params_minmax(it, ep->cur_rate, ep->cur_rate);
}

static int hw_rule_period_size_implicit_fb(struct snd_pcm_hw_params *params,
					   struct snd_pcm_hw_rule *rule)
{
	struct snd_usb_substream *subs = rule->private;
	const struct snd_usb_endpoint *ep;
	struct snd_interval *it;

	ep = get_sync_ep_from_substream(subs);
	if (!ep)
		return 0;

	hwc_debug("applying %s\n", __func__);
	it = hw_param_interval(params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE);
	return apply_hw_params_minmax(it, ep->cur_period_frames,
				      ep->cur_period_frames);
}

static int hw_rule_periods_implicit_fb(struct snd_pcm_hw_params *params,
				       struct snd_pcm_hw_rule *rule)
{
	struct snd_usb_substream *subs = rule->private;
	const struct snd_usb_endpoint *ep;
	struct snd_interval *it;

	ep = get_sync_ep_from_substream(subs);
	if (!ep)
		return 0;

	hwc_debug("applying %s\n", __func__);
	it = hw_param_interval(params, SNDRV_PCM_HW_PARAM_PERIODS);
	return apply_hw_params_minmax(it, ep->cur_buffer_periods,
				      ep->cur_buffer_periods);
}

/*
 * set up the runtime hardware information.
 */

static int setup_hw_info(struct snd_pcm_runtime *runtime, struct snd_usb_substream *subs)
{
	const struct audioformat *fp;
	unsigned int pt, ptmin;
	int param_period_time_if_needed = -1;
	int err;

	runtime->hw.formats = subs->formats;

	runtime->hw.rate_min = 0x7fffffff;
	runtime->hw.rate_max = 0;
	runtime->hw.channels_min = 256;
	runtime->hw.channels_max = 0;
	runtime->hw.rates = 0;
	ptmin = UINT_MAX;
	/* check min/max rates and channels */
	list_for_each_entry(fp, &subs->fmt_list, list) {
		runtime->hw.rates |= fp->rates;
		if (runtime->hw.rate_min > fp->rate_min)
			runtime->hw.rate_min = fp->rate_min;
		if (runtime->hw.rate_max < fp->rate_max)
			runtime->hw.rate_max = fp->rate_max;
		if (runtime->hw.channels_min > fp->channels)
			runtime->hw.channels_min = fp->channels;
		if (runtime->hw.channels_max < fp->channels)
			runtime->hw.channels_max = fp->channels;
		if (fp->fmt_type == UAC_FORMAT_TYPE_II && fp->frame_size > 0) {
			/* FIXME: there might be more than one audio formats... */
			runtime->hw.period_bytes_min = runtime->hw.period_bytes_max =
				fp->frame_size;
		}
		pt = 125 * (1 << fp->datainterval);
		ptmin = min(ptmin, pt);
	}

	param_period_time_if_needed = SNDRV_PCM_HW_PARAM_PERIOD_TIME;
	if (subs->speed == USB_SPEED_FULL)
		/* full speed devices have fixed data packet interval */
		ptmin = 1000;
	if (ptmin == 1000)
		/* if period time doesn't go below 1 ms, no rules needed */
		param_period_time_if_needed = -1;

	err = snd_pcm_hw_constraint_minmax(runtime,
					   SNDRV_PCM_HW_PARAM_PERIOD_TIME,
					   ptmin, UINT_MAX);
	if (err < 0)
		return err;

	err = snd_pcm_hw_rule_add(runtime, 0, SNDRV_PCM_HW_PARAM_RATE,
				  hw_rule_rate, subs,
				  SNDRV_PCM_HW_PARAM_RATE,
				  SNDRV_PCM_HW_PARAM_FORMAT,
				  SNDRV_PCM_HW_PARAM_CHANNELS,
				  param_period_time_if_needed,
				  -1);
	if (err < 0)
		return err;

	err = snd_pcm_hw_rule_add(runtime, 0, SNDRV_PCM_HW_PARAM_CHANNELS,
				  hw_rule_channels, subs,
				  SNDRV_PCM_HW_PARAM_CHANNELS,
				  SNDRV_PCM_HW_PARAM_FORMAT,
				  SNDRV_PCM_HW_PARAM_RATE,
				  param_period_time_if_needed,
				  -1);
	if (err < 0)
		return err;
	err = snd_pcm_hw_rule_add(runtime, 0, SNDRV_PCM_HW_PARAM_FORMAT,
				  hw_rule_format, subs,
				  SNDRV_PCM_HW_PARAM_FORMAT,
				  SNDRV_PCM_HW_PARAM_RATE,
				  SNDRV_PCM_HW_PARAM_CHANNELS,
				  param_period_time_if_needed,
				  -1);
	if (err < 0)
		return err;
	if (param_period_time_if_needed >= 0) {
		err = snd_pcm_hw_rule_add(runtime, 0,
					  SNDRV_PCM_HW_PARAM_PERIOD_TIME,
					  hw_rule_period_time, subs,
					  SNDRV_PCM_HW_PARAM_FORMAT,
					  SNDRV_PCM_HW_PARAM_CHANNELS,
					  SNDRV_PCM_HW_PARAM_RATE,
					  -1);
		if (err < 0)
			return err;
	}

	/* additional hw constraints for implicit fb */
	err = snd_pcm_hw_rule_add(runtime, 0, SNDRV_PCM_HW_PARAM_FORMAT,
				  hw_rule_format_implicit_fb, subs,
				  SNDRV_PCM_HW_PARAM_FORMAT, -1);
	if (err < 0)
		return err;
	err = snd_pcm_hw_rule_add(runtime, 0, SNDRV_PCM_HW_PARAM_RATE,
				  hw_rule_rate_implicit_fb, subs,
				  SNDRV_PCM_HW_PARAM_RATE, -1);
	if (err < 0)
		return err;
	err = snd_pcm_hw_rule_add(runtime, 0, SNDRV_PCM_HW_PARAM_PERIOD_SIZE,
				  hw_rule_period_size_implicit_fb, subs,
				  SNDRV_PCM_HW_PARAM_PERIOD_SIZE, -1);
	if (err < 0)
		return err;
	err = snd_pcm_hw_rule_add(runtime, 0, SNDRV_PCM_HW_PARAM_PERIODS,
				  hw_rule_periods_implicit_fb, subs,
				  SNDRV_PCM_HW_PARAM_PERIODS, -1);
	if (err < 0)
		return err;

	return 0;
}

static int snd_usb_pcm_open(struct snd_pcm_substream *substream)
{
	int direction = substream->stream;
	struct snd_usb_stream *as = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_usb_substream *subs = &as->substream[direction];
	int ret;

	runtime->hw = snd_usb_hardware;
	runtime->private_data = subs;
	subs->pcm_substream = substream;
	/* runtime PM is also done there */

	/* initialize DSD/DOP context */
	subs->dsd_dop.byte_idx = 0;
	subs->dsd_dop.channel = 0;
	subs->dsd_dop.marker = 1;

	ret = setup_hw_info(runtime, subs);
	if (ret < 0)
		return ret;
	ret = snd_usb_autoresume(subs->stream->chip);
	if (ret < 0)
		return ret;
	ret = snd_media_stream_init(subs, as->pcm, direction);
	if (ret < 0)
		snd_usb_autosuspend(subs->stream->chip);
	return ret;
}

static int snd_usb_pcm_close(struct snd_pcm_substream *substream)
{
	int direction = substream->stream;
	struct snd_usb_stream *as = snd_pcm_substream_chip(substream);
	struct snd_usb_substream *subs = &as->substream[direction];
	int ret;

	snd_media_stop_pipeline(subs);

	if (!snd_usb_lock_shutdown(subs->stream->chip)) {
		ret = snd_usb_pcm_change_state(subs, UAC3_PD_STATE_D1);
		snd_usb_unlock_shutdown(subs->stream->chip);
		if (ret < 0)
			return ret;
	}

	subs->pcm_substream = NULL;
	snd_usb_autosuspend(subs->stream->chip);

	return 0;
}

/* Since a URB can handle only a single linear buffer, we must use double
 * buffering when the data to be transferred overflows the buffer boundary.
 * To avoid inconsistencies when updating hwptr_done, we use double buffering
 * for all URBs.
 */
static void retire_capture_urb(struct snd_usb_substream *subs,
			       struct urb *urb)
{
	struct snd_pcm_runtime *runtime = subs->pcm_substream->runtime;
	unsigned int stride, frames, bytes, oldptr;
	int i, period_elapsed = 0;
	unsigned long flags;
	unsigned char *cp;
	int current_frame_number;

	/* read frame number here, update pointer in critical section */
	current_frame_number = usb_get_current_frame_number(subs->dev);

	stride = runtime->frame_bits >> 3;

	for (i = 0; i < urb->number_of_packets; i++) {
		cp = (unsigned char *)urb->transfer_buffer + urb->iso_frame_desc[i].offset + subs->pkt_offset_adj;
		if (urb->iso_frame_desc[i].status && printk_ratelimit()) {
			dev_dbg(&subs->dev->dev, "frame %d active: %d\n",
				i, urb->iso_frame_desc[i].status);
			// continue;
		}
		bytes = urb->iso_frame_desc[i].actual_length;
		if (subs->stream_offset_adj > 0) {
			unsigned int adj = min(subs->stream_offset_adj, bytes);
			cp += adj;
			bytes -= adj;
			subs->stream_offset_adj -= adj;
		}
		frames = bytes / stride;
		if (!subs->txfr_quirk)
			bytes = frames * stride;
		if (bytes % (runtime->sample_bits >> 3) != 0) {
			int oldbytes = bytes;
			bytes = frames * stride;
			dev_warn_ratelimited(&subs->dev->dev,
				 "Corrected urb data len. %d->%d\n",
							oldbytes, bytes);
		}
		/* update the current pointer */
		spin_lock_irqsave(&subs->lock, flags);
		oldptr = subs->hwptr_done;
		subs->hwptr_done += bytes;
		if (subs->hwptr_done >= runtime->buffer_size * stride)
			subs->hwptr_done -= runtime->buffer_size * stride;
		frames = (bytes + (oldptr % stride)) / stride;
		subs->transfer_done += frames;
		if (subs->transfer_done >= runtime->period_size) {
			subs->transfer_done -= runtime->period_size;
			period_elapsed = 1;
		}
		/* capture delay is by construction limited to one URB,
		 * reset delays here
		 */
		runtime->delay = subs->last_delay = 0;

		/* realign last_frame_number */
		subs->last_frame_number = current_frame_number;
		subs->last_frame_number &= 0xFF; /* keep 8 LSBs */

		spin_unlock_irqrestore(&subs->lock, flags);
		/* copy a data chunk */
		if (oldptr + bytes > runtime->buffer_size * stride) {
			unsigned int bytes1 =
					runtime->buffer_size * stride - oldptr;
			memcpy(runtime->dma_area + oldptr, cp, bytes1);
			memcpy(runtime->dma_area, cp + bytes1, bytes - bytes1);
		} else {
			memcpy(runtime->dma_area + oldptr, cp, bytes);
		}
	}

	if (period_elapsed)
		snd_pcm_period_elapsed(subs->pcm_substream);
}

static inline void fill_playback_urb_dsd_dop(struct snd_usb_substream *subs,
					     struct urb *urb, unsigned int bytes)
{
	struct snd_pcm_runtime *runtime = subs->pcm_substream->runtime;
	unsigned int stride = runtime->frame_bits >> 3;
	unsigned int dst_idx = 0;
	unsigned int src_idx = subs->hwptr_done;
	unsigned int wrap = runtime->buffer_size * stride;
	u8 *dst = urb->transfer_buffer;
	u8 *src = runtime->dma_area;
	u8 marker[] = { 0x05, 0xfa };

	/*
	 * The DSP DOP format defines a way to transport DSD samples over
	 * normal PCM data endpoints. It requires stuffing of marker bytes
	 * (0x05 and 0xfa, alternating per sample frame), and then expects
	 * 2 additional bytes of actual payload. The whole frame is stored
	 * LSB.
	 *
	 * Hence, for a stereo transport, the buffer layout looks like this,
	 * where L refers to left channel samples and R to right.
	 *
	 *   L1 L2 0x05   R1 R2 0x05   L3 L4 0xfa  R3 R4 0xfa
	 *   L5 L6 0x05   R5 R6 0x05   L7 L8 0xfa  R7 R8 0xfa
	 *   .....
	 *
	 */

	while (bytes--) {
		if (++subs->dsd_dop.byte_idx == 3) {
			/* frame boundary? */
			dst[dst_idx++] = marker[subs->dsd_dop.marker];
			src_idx += 2;
			subs->dsd_dop.byte_idx = 0;

			if (++subs->dsd_dop.channel % runtime->channels == 0) {
				/* alternate the marker */
				subs->dsd_dop.marker++;
				subs->dsd_dop.marker %= ARRAY_SIZE(marker);
				subs->dsd_dop.channel = 0;
			}
		} else {
			/* stuff the DSD payload */
			int idx = (src_idx + subs->dsd_dop.byte_idx - 1) % wrap;

			if (subs->cur_audiofmt->dsd_bitrev)
				dst[dst_idx++] = bitrev8(src[idx]);
			else
				dst[dst_idx++] = src[idx];

			subs->hwptr_done++;
		}
	}
	if (subs->hwptr_done >= runtime->buffer_size * stride)
		subs->hwptr_done -= runtime->buffer_size * stride;
}

static void copy_to_urb(struct snd_usb_substream *subs, struct urb *urb,
			int offset, int stride, unsigned int bytes)
{
	struct snd_pcm_runtime *runtime = subs->pcm_substream->runtime;

	if (subs->hwptr_done + bytes > runtime->buffer_size * stride) {
		/* err, the transferred area goes over buffer boundary. */
		unsigned int bytes1 =
			runtime->buffer_size * stride - subs->hwptr_done;
		memcpy(urb->transfer_buffer + offset,
		       runtime->dma_area + subs->hwptr_done, bytes1);
		memcpy(urb->transfer_buffer + offset + bytes1,
		       runtime->dma_area, bytes - bytes1);
	} else {
		memcpy(urb->transfer_buffer + offset,
		       runtime->dma_area + subs->hwptr_done, bytes);
	}
	subs->hwptr_done += bytes;
	if (subs->hwptr_done >= runtime->buffer_size * stride)
		subs->hwptr_done -= runtime->buffer_size * stride;
}

static unsigned int copy_to_urb_quirk(struct snd_usb_substream *subs,
				      struct urb *urb, int stride,
				      unsigned int bytes)
{
	__le32 packet_length;
	int i;

	/* Put __le32 length descriptor at start of each packet. */
	for (i = 0; i < urb->number_of_packets; i++) {
		unsigned int length = urb->iso_frame_desc[i].length;
		unsigned int offset = urb->iso_frame_desc[i].offset;

		packet_length = cpu_to_le32(length);
		offset += i * sizeof(packet_length);
		urb->iso_frame_desc[i].offset = offset;
		urb->iso_frame_desc[i].length += sizeof(packet_length);
		memcpy(urb->transfer_buffer + offset,
		       &packet_length, sizeof(packet_length));
		copy_to_urb(subs, urb, offset + sizeof(packet_length),
			    stride, length);
	}
	/* Adjust transfer size accordingly. */
	bytes += urb->number_of_packets * sizeof(packet_length);
	return bytes;
}

static void prepare_playback_urb(struct snd_usb_substream *subs,
				 struct urb *urb)
{
	struct snd_pcm_runtime *runtime = subs->pcm_substream->runtime;
	struct snd_usb_endpoint *ep = subs->data_endpoint;
	struct snd_urb_ctx *ctx = urb->context;
	unsigned int counts, frames, bytes;
	int i, stride, period_elapsed = 0;
	unsigned long flags;

	stride = runtime->frame_bits >> 3;

	frames = 0;
	urb->number_of_packets = 0;
	spin_lock_irqsave(&subs->lock, flags);
	subs->frame_limit += ep->max_urb_frames;
	for (i = 0; i < ctx->packets; i++) {
		counts = snd_usb_endpoint_next_packet_size(ep, ctx, i);
		/* set up descriptor */
		urb->iso_frame_desc[i].offset = frames * ep->stride;
		urb->iso_frame_desc[i].length = counts * ep->stride;
		frames += counts;
		urb->number_of_packets++;
		subs->transfer_done += counts;
		if (subs->transfer_done >= runtime->period_size) {
			subs->transfer_done -= runtime->period_size;
			subs->frame_limit = 0;
			period_elapsed = 1;
			if (subs->fmt_type == UAC_FORMAT_TYPE_II) {
				if (subs->transfer_done > 0) {
					/* FIXME: fill-max mode is not
					 * supported yet */
					frames -= subs->transfer_done;
					counts -= subs->transfer_done;
					urb->iso_frame_desc[i].length =
						counts * ep->stride;
					subs->transfer_done = 0;
				}
				i++;
				if (i < ctx->packets) {
					/* add a transfer delimiter */
					urb->iso_frame_desc[i].offset =
						frames * ep->stride;
					urb->iso_frame_desc[i].length = 0;
					urb->number_of_packets++;
				}
				break;
			}
		}
		/* finish at the period boundary or after enough frames */
		if ((period_elapsed ||
				subs->transfer_done >= subs->frame_limit) &&
		    !snd_usb_endpoint_implicit_feedback_sink(ep))
			break;
	}
	bytes = frames * ep->stride;

	if (unlikely(ep->cur_format == SNDRV_PCM_FORMAT_DSD_U16_LE &&
		     subs->cur_audiofmt->dsd_dop)) {
		fill_playback_urb_dsd_dop(subs, urb, bytes);
	} else if (unlikely(ep->cur_format == SNDRV_PCM_FORMAT_DSD_U8 &&
			   subs->cur_audiofmt->dsd_bitrev)) {
		/* bit-reverse the bytes */
		u8 *buf = urb->transfer_buffer;
		for (i = 0; i < bytes; i++) {
			int idx = (subs->hwptr_done + i)
				% (runtime->buffer_size * stride);
			buf[i] = bitrev8(runtime->dma_area[idx]);
		}

		subs->hwptr_done += bytes;
		if (subs->hwptr_done >= runtime->buffer_size * stride)
			subs->hwptr_done -= runtime->buffer_size * stride;
	} else {
		/* usual PCM */
		if (!subs->tx_length_quirk)
			copy_to_urb(subs, urb, 0, stride, bytes);
		else
			bytes = copy_to_urb_quirk(subs, urb, stride, bytes);
			/* bytes is now amount of outgoing data */
	}

	/* update delay with exact number of samples queued */
	runtime->delay = subs->last_delay;
	runtime->delay += frames;
	subs->last_delay = runtime->delay;

	/* realign last_frame_number */
	subs->last_frame_number = usb_get_current_frame_number(subs->dev);
	subs->last_frame_number &= 0xFF; /* keep 8 LSBs */

	if (subs->trigger_tstamp_pending_update) {
		/* this is the first actual URB submitted,
		 * update trigger timestamp to reflect actual start time
		 */
		snd_pcm_gettime(runtime, &runtime->trigger_tstamp);
		subs->trigger_tstamp_pending_update = false;
	}

	spin_unlock_irqrestore(&subs->lock, flags);
	urb->transfer_buffer_length = bytes;
	if (period_elapsed)
		snd_pcm_period_elapsed(subs->pcm_substream);
}

/*
 * process after playback data complete
 * - decrease the delay count again
 */
static void retire_playback_urb(struct snd_usb_substream *subs,
			       struct urb *urb)
{
	unsigned long flags;
	struct snd_pcm_runtime *runtime = subs->pcm_substream->runtime;
	struct snd_usb_endpoint *ep = subs->data_endpoint;
	int processed = urb->transfer_buffer_length / ep->stride;
	int est_delay;

	/* ignore the delay accounting when processed=0 is given, i.e.
	 * silent payloads are processed before handling the actual data
	 */
	if (!processed)
		return;

	spin_lock_irqsave(&subs->lock, flags);
	if (!subs->last_delay)
		goto out; /* short path */

	est_delay = snd_usb_pcm_delay(subs, runtime->rate);
	/* update delay with exact number of samples played */
	if (processed > subs->last_delay)
		subs->last_delay = 0;
	else
		subs->last_delay -= processed;
	runtime->delay = subs->last_delay;

	/*
	 * Report when delay estimate is off by more than 2ms.
	 * The error should be lower than 2ms since the estimate relies
	 * on two reads of a counter updated every ms.
	 */
	if (abs(est_delay - subs->last_delay) * 1000 > runtime->rate * 2)
		dev_dbg_ratelimited(&subs->dev->dev,
			"delay: estimated %d, actual %d\n",
			est_delay, subs->last_delay);

	if (!subs->running) {
		/* update last_frame_number for delay counting here since
		 * prepare_playback_urb won't be called during pause
		 */
		subs->last_frame_number =
			usb_get_current_frame_number(subs->dev) & 0xff;
	}

 out:
	spin_unlock_irqrestore(&subs->lock, flags);
}

static int snd_usb_substream_playback_trigger(struct snd_pcm_substream *substream,
					      int cmd)
{
	struct snd_usb_substream *subs = substream->runtime->private_data;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		subs->trigger_tstamp_pending_update = true;
		fallthrough;
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		snd_usb_endpoint_set_callback(subs->data_endpoint,
					      prepare_playback_urb,
					      retire_playback_urb,
					      subs);
		subs->running = 1;
		dev_dbg(&subs->dev->dev, "%d:%d Start Playback PCM\n",
			subs->cur_audiofmt->iface,
			subs->cur_audiofmt->altsetting);
		return 0;
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_STOP:
		stop_endpoints(subs);
		snd_usb_endpoint_set_callback(subs->data_endpoint,
					      NULL, NULL, NULL);
		subs->running = 0;
		dev_dbg(&subs->dev->dev, "%d:%d Stop Playback PCM\n",
			subs->cur_audiofmt->iface,
			subs->cur_audiofmt->altsetting);
		return 0;
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		/* keep retire_data_urb for delay calculation */
		snd_usb_endpoint_set_callback(subs->data_endpoint,
					      NULL,
					      retire_playback_urb,
					      subs);
		subs->running = 0;
		dev_dbg(&subs->dev->dev, "%d:%d Pause Playback PCM\n",
			subs->cur_audiofmt->iface,
			subs->cur_audiofmt->altsetting);
		return 0;
	}

	return -EINVAL;
}

static int snd_usb_substream_capture_trigger(struct snd_pcm_substream *substream,
					     int cmd)
{
	int err;
	struct snd_usb_substream *subs = substream->runtime->private_data;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		err = start_endpoints(subs);
		if (err < 0)
			return err;
		fallthrough;
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		snd_usb_endpoint_set_callback(subs->data_endpoint,
					      NULL, retire_capture_urb,
					      subs);
		subs->running = 1;
		dev_dbg(&subs->dev->dev, "%d:%d Start Capture PCM\n",
			subs->cur_audiofmt->iface,
			subs->cur_audiofmt->altsetting);
		return 0;
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_STOP:
		stop_endpoints(subs);
		fallthrough;
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		snd_usb_endpoint_set_callback(subs->data_endpoint,
					      NULL, NULL, NULL);
		subs->running = 0;
		dev_dbg(&subs->dev->dev, "%d:%d Stop Capture PCM\n",
			subs->cur_audiofmt->iface,
			subs->cur_audiofmt->altsetting);
		return 0;
	}

	return -EINVAL;
}

static const struct snd_pcm_ops snd_usb_playback_ops = {
	.open =		snd_usb_pcm_open,
	.close =	snd_usb_pcm_close,
	.hw_params =	snd_usb_hw_params,
	.hw_free =	snd_usb_hw_free,
	.prepare =	snd_usb_pcm_prepare,
	.trigger =	snd_usb_substream_playback_trigger,
	.sync_stop =	snd_usb_pcm_sync_stop,
	.pointer =	snd_usb_pcm_pointer,
};

static const struct snd_pcm_ops snd_usb_capture_ops = {
	.open =		snd_usb_pcm_open,
	.close =	snd_usb_pcm_close,
	.hw_params =	snd_usb_hw_params,
	.hw_free =	snd_usb_hw_free,
	.prepare =	snd_usb_pcm_prepare,
	.trigger =	snd_usb_substream_capture_trigger,
	.sync_stop =	snd_usb_pcm_sync_stop,
	.pointer =	snd_usb_pcm_pointer,
};

void snd_usb_set_pcm_ops(struct snd_pcm *pcm, int stream)
{
	const struct snd_pcm_ops *ops;

	ops = stream == SNDRV_PCM_STREAM_PLAYBACK ?
			&snd_usb_playback_ops : &snd_usb_capture_ops;
	snd_pcm_set_ops(pcm, stream, ops);
}

void snd_usb_preallocate_buffer(struct snd_usb_substream *subs)
{
	struct snd_pcm *pcm = subs->stream->pcm;
	struct snd_pcm_substream *s = pcm->streams[subs->direction].substream;
	struct device *dev = subs->dev->bus->sysdev;

	if (snd_usb_use_vmalloc)
		snd_pcm_set_managed_buffer(s, SNDRV_DMA_TYPE_VMALLOC,
					   NULL, 0, 0);
	else
		snd_pcm_set_managed_buffer(s, SNDRV_DMA_TYPE_DEV_SG,
					   dev, 64*1024, 512*1024);
}
