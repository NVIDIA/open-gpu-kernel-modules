// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Digital Audio (PCM) abstract layer
 *  Copyright (c) by Jaroslav Kysela <perex@perex.cz>
 *                   Abramo Bagnara <abramo@alsa-project.org>
 */

#include <linux/slab.h>
#include <linux/sched/signal.h>
#include <linux/time.h>
#include <linux/math64.h>
#include <linux/export.h>
#include <sound/core.h>
#include <sound/control.h>
#include <sound/tlv.h>
#include <sound/info.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/timer.h>

#include "pcm_local.h"

#ifdef CONFIG_SND_PCM_XRUN_DEBUG
#define CREATE_TRACE_POINTS
#include "pcm_trace.h"
#else
#define trace_hwptr(substream, pos, in_interrupt)
#define trace_xrun(substream)
#define trace_hw_ptr_error(substream, reason)
#define trace_applptr(substream, prev, curr)
#endif

static int fill_silence_frames(struct snd_pcm_substream *substream,
			       snd_pcm_uframes_t off, snd_pcm_uframes_t frames);

/*
 * fill ring buffer with silence
 * runtime->silence_start: starting pointer to silence area
 * runtime->silence_filled: size filled with silence
 * runtime->silence_threshold: threshold from application
 * runtime->silence_size: maximal size from application
 *
 * when runtime->silence_size >= runtime->boundary - fill processed area with silence immediately
 */
void snd_pcm_playback_silence(struct snd_pcm_substream *substream, snd_pcm_uframes_t new_hw_ptr)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_uframes_t frames, ofs, transfer;
	int err;

	if (runtime->silence_size < runtime->boundary) {
		snd_pcm_sframes_t noise_dist, n;
		snd_pcm_uframes_t appl_ptr = READ_ONCE(runtime->control->appl_ptr);
		if (runtime->silence_start != appl_ptr) {
			n = appl_ptr - runtime->silence_start;
			if (n < 0)
				n += runtime->boundary;
			if ((snd_pcm_uframes_t)n < runtime->silence_filled)
				runtime->silence_filled -= n;
			else
				runtime->silence_filled = 0;
			runtime->silence_start = appl_ptr;
		}
		if (runtime->silence_filled >= runtime->buffer_size)
			return;
		noise_dist = snd_pcm_playback_hw_avail(runtime) + runtime->silence_filled;
		if (noise_dist >= (snd_pcm_sframes_t) runtime->silence_threshold)
			return;
		frames = runtime->silence_threshold - noise_dist;
		if (frames > runtime->silence_size)
			frames = runtime->silence_size;
	} else {
		if (new_hw_ptr == ULONG_MAX) {	/* initialization */
			snd_pcm_sframes_t avail = snd_pcm_playback_hw_avail(runtime);
			if (avail > runtime->buffer_size)
				avail = runtime->buffer_size;
			runtime->silence_filled = avail > 0 ? avail : 0;
			runtime->silence_start = (runtime->status->hw_ptr +
						  runtime->silence_filled) %
						 runtime->boundary;
		} else {
			ofs = runtime->status->hw_ptr;
			frames = new_hw_ptr - ofs;
			if ((snd_pcm_sframes_t)frames < 0)
				frames += runtime->boundary;
			runtime->silence_filled -= frames;
			if ((snd_pcm_sframes_t)runtime->silence_filled < 0) {
				runtime->silence_filled = 0;
				runtime->silence_start = new_hw_ptr;
			} else {
				runtime->silence_start = ofs;
			}
		}
		frames = runtime->buffer_size - runtime->silence_filled;
	}
	if (snd_BUG_ON(frames > runtime->buffer_size))
		return;
	if (frames == 0)
		return;
	ofs = runtime->silence_start % runtime->buffer_size;
	while (frames > 0) {
		transfer = ofs + frames > runtime->buffer_size ? runtime->buffer_size - ofs : frames;
		err = fill_silence_frames(substream, ofs, transfer);
		snd_BUG_ON(err < 0);
		runtime->silence_filled += transfer;
		frames -= transfer;
		ofs = 0;
	}
}

#ifdef CONFIG_SND_DEBUG
void snd_pcm_debug_name(struct snd_pcm_substream *substream,
			   char *name, size_t len)
{
	snprintf(name, len, "pcmC%dD%d%c:%d",
		 substream->pcm->card->number,
		 substream->pcm->device,
		 substream->stream ? 'c' : 'p',
		 substream->number);
}
EXPORT_SYMBOL(snd_pcm_debug_name);
#endif

#define XRUN_DEBUG_BASIC	(1<<0)
#define XRUN_DEBUG_STACK	(1<<1)	/* dump also stack */
#define XRUN_DEBUG_JIFFIESCHECK	(1<<2)	/* do jiffies check */

#ifdef CONFIG_SND_PCM_XRUN_DEBUG

#define xrun_debug(substream, mask) \
			((substream)->pstr->xrun_debug & (mask))
#else
#define xrun_debug(substream, mask)	0
#endif

#define dump_stack_on_xrun(substream) do {			\
		if (xrun_debug(substream, XRUN_DEBUG_STACK))	\
			dump_stack();				\
	} while (0)

/* call with stream lock held */
void __snd_pcm_xrun(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	trace_xrun(substream);
	if (runtime->tstamp_mode == SNDRV_PCM_TSTAMP_ENABLE) {
		struct timespec64 tstamp;

		snd_pcm_gettime(runtime, &tstamp);
		runtime->status->tstamp.tv_sec = tstamp.tv_sec;
		runtime->status->tstamp.tv_nsec = tstamp.tv_nsec;
	}
	snd_pcm_stop(substream, SNDRV_PCM_STATE_XRUN);
	if (xrun_debug(substream, XRUN_DEBUG_BASIC)) {
		char name[16];
		snd_pcm_debug_name(substream, name, sizeof(name));
		pcm_warn(substream->pcm, "XRUN: %s\n", name);
		dump_stack_on_xrun(substream);
	}
}

#ifdef CONFIG_SND_PCM_XRUN_DEBUG
#define hw_ptr_error(substream, in_interrupt, reason, fmt, args...)	\
	do {								\
		trace_hw_ptr_error(substream, reason);	\
		if (xrun_debug(substream, XRUN_DEBUG_BASIC)) {		\
			pr_err_ratelimited("ALSA: PCM: [%c] " reason ": " fmt, \
					   (in_interrupt) ? 'Q' : 'P', ##args);	\
			dump_stack_on_xrun(substream);			\
		}							\
	} while (0)

#else /* ! CONFIG_SND_PCM_XRUN_DEBUG */

#define hw_ptr_error(substream, fmt, args...) do { } while (0)

#endif

int snd_pcm_update_state(struct snd_pcm_substream *substream,
			 struct snd_pcm_runtime *runtime)
{
	snd_pcm_uframes_t avail;

	avail = snd_pcm_avail(substream);
	if (avail > runtime->avail_max)
		runtime->avail_max = avail;
	if (runtime->status->state == SNDRV_PCM_STATE_DRAINING) {
		if (avail >= runtime->buffer_size) {
			snd_pcm_drain_done(substream);
			return -EPIPE;
		}
	} else {
		if (avail >= runtime->stop_threshold) {
			__snd_pcm_xrun(substream);
			return -EPIPE;
		}
	}
	if (runtime->twake) {
		if (avail >= runtime->twake)
			wake_up(&runtime->tsleep);
	} else if (avail >= runtime->control->avail_min)
		wake_up(&runtime->sleep);
	return 0;
}

static void update_audio_tstamp(struct snd_pcm_substream *substream,
				struct timespec64 *curr_tstamp,
				struct timespec64 *audio_tstamp)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	u64 audio_frames, audio_nsecs;
	struct timespec64 driver_tstamp;

	if (runtime->tstamp_mode != SNDRV_PCM_TSTAMP_ENABLE)
		return;

	if (!(substream->ops->get_time_info) ||
		(runtime->audio_tstamp_report.actual_type ==
			SNDRV_PCM_AUDIO_TSTAMP_TYPE_DEFAULT)) {

		/*
		 * provide audio timestamp derived from pointer position
		 * add delay only if requested
		 */

		audio_frames = runtime->hw_ptr_wrap + runtime->status->hw_ptr;

		if (runtime->audio_tstamp_config.report_delay) {
			if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
				audio_frames -=  runtime->delay;
			else
				audio_frames +=  runtime->delay;
		}
		audio_nsecs = div_u64(audio_frames * 1000000000LL,
				runtime->rate);
		*audio_tstamp = ns_to_timespec64(audio_nsecs);
	}

	if (runtime->status->audio_tstamp.tv_sec != audio_tstamp->tv_sec ||
	    runtime->status->audio_tstamp.tv_nsec != audio_tstamp->tv_nsec) {
		runtime->status->audio_tstamp.tv_sec = audio_tstamp->tv_sec;
		runtime->status->audio_tstamp.tv_nsec = audio_tstamp->tv_nsec;
		runtime->status->tstamp.tv_sec = curr_tstamp->tv_sec;
		runtime->status->tstamp.tv_nsec = curr_tstamp->tv_nsec;
	}


	/*
	 * re-take a driver timestamp to let apps detect if the reference tstamp
	 * read by low-level hardware was provided with a delay
	 */
	snd_pcm_gettime(substream->runtime, &driver_tstamp);
	runtime->driver_tstamp = driver_tstamp;
}

static int snd_pcm_update_hw_ptr0(struct snd_pcm_substream *substream,
				  unsigned int in_interrupt)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_uframes_t pos;
	snd_pcm_uframes_t old_hw_ptr, new_hw_ptr, hw_base;
	snd_pcm_sframes_t hdelta, delta;
	unsigned long jdelta;
	unsigned long curr_jiffies;
	struct timespec64 curr_tstamp;
	struct timespec64 audio_tstamp;
	int crossed_boundary = 0;

	old_hw_ptr = runtime->status->hw_ptr;

	/*
	 * group pointer, time and jiffies reads to allow for more
	 * accurate correlations/corrections.
	 * The values are stored at the end of this routine after
	 * corrections for hw_ptr position
	 */
	pos = substream->ops->pointer(substream);
	curr_jiffies = jiffies;
	if (runtime->tstamp_mode == SNDRV_PCM_TSTAMP_ENABLE) {
		if ((substream->ops->get_time_info) &&
			(runtime->audio_tstamp_config.type_requested != SNDRV_PCM_AUDIO_TSTAMP_TYPE_DEFAULT)) {
			substream->ops->get_time_info(substream, &curr_tstamp,
						&audio_tstamp,
						&runtime->audio_tstamp_config,
						&runtime->audio_tstamp_report);

			/* re-test in case tstamp type is not supported in hardware and was demoted to DEFAULT */
			if (runtime->audio_tstamp_report.actual_type == SNDRV_PCM_AUDIO_TSTAMP_TYPE_DEFAULT)
				snd_pcm_gettime(runtime, &curr_tstamp);
		} else
			snd_pcm_gettime(runtime, &curr_tstamp);
	}

	if (pos == SNDRV_PCM_POS_XRUN) {
		__snd_pcm_xrun(substream);
		return -EPIPE;
	}
	if (pos >= runtime->buffer_size) {
		if (printk_ratelimit()) {
			char name[16];
			snd_pcm_debug_name(substream, name, sizeof(name));
			pcm_err(substream->pcm,
				"invalid position: %s, pos = %ld, buffer size = %ld, period size = %ld\n",
				name, pos, runtime->buffer_size,
				runtime->period_size);
		}
		pos = 0;
	}
	pos -= pos % runtime->min_align;
	trace_hwptr(substream, pos, in_interrupt);
	hw_base = runtime->hw_ptr_base;
	new_hw_ptr = hw_base + pos;
	if (in_interrupt) {
		/* we know that one period was processed */
		/* delta = "expected next hw_ptr" for in_interrupt != 0 */
		delta = runtime->hw_ptr_interrupt + runtime->period_size;
		if (delta > new_hw_ptr) {
			/* check for double acknowledged interrupts */
			hdelta = curr_jiffies - runtime->hw_ptr_jiffies;
			if (hdelta > runtime->hw_ptr_buffer_jiffies/2 + 1) {
				hw_base += runtime->buffer_size;
				if (hw_base >= runtime->boundary) {
					hw_base = 0;
					crossed_boundary++;
				}
				new_hw_ptr = hw_base + pos;
				goto __delta;
			}
		}
	}
	/* new_hw_ptr might be lower than old_hw_ptr in case when */
	/* pointer crosses the end of the ring buffer */
	if (new_hw_ptr < old_hw_ptr) {
		hw_base += runtime->buffer_size;
		if (hw_base >= runtime->boundary) {
			hw_base = 0;
			crossed_boundary++;
		}
		new_hw_ptr = hw_base + pos;
	}
      __delta:
	delta = new_hw_ptr - old_hw_ptr;
	if (delta < 0)
		delta += runtime->boundary;

	if (runtime->no_period_wakeup) {
		snd_pcm_sframes_t xrun_threshold;
		/*
		 * Without regular period interrupts, we have to check
		 * the elapsed time to detect xruns.
		 */
		jdelta = curr_jiffies - runtime->hw_ptr_jiffies;
		if (jdelta < runtime->hw_ptr_buffer_jiffies / 2)
			goto no_delta_check;
		hdelta = jdelta - delta * HZ / runtime->rate;
		xrun_threshold = runtime->hw_ptr_buffer_jiffies / 2 + 1;
		while (hdelta > xrun_threshold) {
			delta += runtime->buffer_size;
			hw_base += runtime->buffer_size;
			if (hw_base >= runtime->boundary) {
				hw_base = 0;
				crossed_boundary++;
			}
			new_hw_ptr = hw_base + pos;
			hdelta -= runtime->hw_ptr_buffer_jiffies;
		}
		goto no_delta_check;
	}

	/* something must be really wrong */
	if (delta >= runtime->buffer_size + runtime->period_size) {
		hw_ptr_error(substream, in_interrupt, "Unexpected hw_ptr",
			     "(stream=%i, pos=%ld, new_hw_ptr=%ld, old_hw_ptr=%ld)\n",
			     substream->stream, (long)pos,
			     (long)new_hw_ptr, (long)old_hw_ptr);
		return 0;
	}

	/* Do jiffies check only in xrun_debug mode */
	if (!xrun_debug(substream, XRUN_DEBUG_JIFFIESCHECK))
		goto no_jiffies_check;

	/* Skip the jiffies check for hardwares with BATCH flag.
	 * Such hardware usually just increases the position at each IRQ,
	 * thus it can't give any strange position.
	 */
	if (runtime->hw.info & SNDRV_PCM_INFO_BATCH)
		goto no_jiffies_check;
	hdelta = delta;
	if (hdelta < runtime->delay)
		goto no_jiffies_check;
	hdelta -= runtime->delay;
	jdelta = curr_jiffies - runtime->hw_ptr_jiffies;
	if (((hdelta * HZ) / runtime->rate) > jdelta + HZ/100) {
		delta = jdelta /
			(((runtime->period_size * HZ) / runtime->rate)
								+ HZ/100);
		/* move new_hw_ptr according jiffies not pos variable */
		new_hw_ptr = old_hw_ptr;
		hw_base = delta;
		/* use loop to avoid checks for delta overflows */
		/* the delta value is small or zero in most cases */
		while (delta > 0) {
			new_hw_ptr += runtime->period_size;
			if (new_hw_ptr >= runtime->boundary) {
				new_hw_ptr -= runtime->boundary;
				crossed_boundary--;
			}
			delta--;
		}
		/* align hw_base to buffer_size */
		hw_ptr_error(substream, in_interrupt, "hw_ptr skipping",
			     "(pos=%ld, delta=%ld, period=%ld, jdelta=%lu/%lu/%lu, hw_ptr=%ld/%ld)\n",
			     (long)pos, (long)hdelta,
			     (long)runtime->period_size, jdelta,
			     ((hdelta * HZ) / runtime->rate), hw_base,
			     (unsigned long)old_hw_ptr,
			     (unsigned long)new_hw_ptr);
		/* reset values to proper state */
		delta = 0;
		hw_base = new_hw_ptr - (new_hw_ptr % runtime->buffer_size);
	}
 no_jiffies_check:
	if (delta > runtime->period_size + runtime->period_size / 2) {
		hw_ptr_error(substream, in_interrupt,
			     "Lost interrupts?",
			     "(stream=%i, delta=%ld, new_hw_ptr=%ld, old_hw_ptr=%ld)\n",
			     substream->stream, (long)delta,
			     (long)new_hw_ptr,
			     (long)old_hw_ptr);
	}

 no_delta_check:
	if (runtime->status->hw_ptr == new_hw_ptr) {
		runtime->hw_ptr_jiffies = curr_jiffies;
		update_audio_tstamp(substream, &curr_tstamp, &audio_tstamp);
		return 0;
	}

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK &&
	    runtime->silence_size > 0)
		snd_pcm_playback_silence(substream, new_hw_ptr);

	if (in_interrupt) {
		delta = new_hw_ptr - runtime->hw_ptr_interrupt;
		if (delta < 0)
			delta += runtime->boundary;
		delta -= (snd_pcm_uframes_t)delta % runtime->period_size;
		runtime->hw_ptr_interrupt += delta;
		if (runtime->hw_ptr_interrupt >= runtime->boundary)
			runtime->hw_ptr_interrupt -= runtime->boundary;
	}
	runtime->hw_ptr_base = hw_base;
	runtime->status->hw_ptr = new_hw_ptr;
	runtime->hw_ptr_jiffies = curr_jiffies;
	if (crossed_boundary) {
		snd_BUG_ON(crossed_boundary != 1);
		runtime->hw_ptr_wrap += runtime->boundary;
	}

	update_audio_tstamp(substream, &curr_tstamp, &audio_tstamp);

	return snd_pcm_update_state(substream, runtime);
}

/* CAUTION: call it with irq disabled */
int snd_pcm_update_hw_ptr(struct snd_pcm_substream *substream)
{
	return snd_pcm_update_hw_ptr0(substream, 0);
}

/**
 * snd_pcm_set_ops - set the PCM operators
 * @pcm: the pcm instance
 * @direction: stream direction, SNDRV_PCM_STREAM_XXX
 * @ops: the operator table
 *
 * Sets the given PCM operators to the pcm instance.
 */
void snd_pcm_set_ops(struct snd_pcm *pcm, int direction,
		     const struct snd_pcm_ops *ops)
{
	struct snd_pcm_str *stream = &pcm->streams[direction];
	struct snd_pcm_substream *substream;
	
	for (substream = stream->substream; substream != NULL; substream = substream->next)
		substream->ops = ops;
}
EXPORT_SYMBOL(snd_pcm_set_ops);

/**
 * snd_pcm_set_sync - set the PCM sync id
 * @substream: the pcm substream
 *
 * Sets the PCM sync identifier for the card.
 */
void snd_pcm_set_sync(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	
	runtime->sync.id32[0] = substream->pcm->card->number;
	runtime->sync.id32[1] = -1;
	runtime->sync.id32[2] = -1;
	runtime->sync.id32[3] = -1;
}
EXPORT_SYMBOL(snd_pcm_set_sync);

/*
 *  Standard ioctl routine
 */

static inline unsigned int div32(unsigned int a, unsigned int b, 
				 unsigned int *r)
{
	if (b == 0) {
		*r = 0;
		return UINT_MAX;
	}
	*r = a % b;
	return a / b;
}

static inline unsigned int div_down(unsigned int a, unsigned int b)
{
	if (b == 0)
		return UINT_MAX;
	return a / b;
}

static inline unsigned int div_up(unsigned int a, unsigned int b)
{
	unsigned int r;
	unsigned int q;
	if (b == 0)
		return UINT_MAX;
	q = div32(a, b, &r);
	if (r)
		++q;
	return q;
}

static inline unsigned int mul(unsigned int a, unsigned int b)
{
	if (a == 0)
		return 0;
	if (div_down(UINT_MAX, a) < b)
		return UINT_MAX;
	return a * b;
}

static inline unsigned int muldiv32(unsigned int a, unsigned int b,
				    unsigned int c, unsigned int *r)
{
	u_int64_t n = (u_int64_t) a * b;
	if (c == 0) {
		*r = 0;
		return UINT_MAX;
	}
	n = div_u64_rem(n, c, r);
	if (n >= UINT_MAX) {
		*r = 0;
		return UINT_MAX;
	}
	return n;
}

/**
 * snd_interval_refine - refine the interval value of configurator
 * @i: the interval value to refine
 * @v: the interval value to refer to
 *
 * Refines the interval value with the reference value.
 * The interval is changed to the range satisfying both intervals.
 * The interval status (min, max, integer, etc.) are evaluated.
 *
 * Return: Positive if the value is changed, zero if it's not changed, or a
 * negative error code.
 */
int snd_interval_refine(struct snd_interval *i, const struct snd_interval *v)
{
	int changed = 0;
	if (snd_BUG_ON(snd_interval_empty(i)))
		return -EINVAL;
	if (i->min < v->min) {
		i->min = v->min;
		i->openmin = v->openmin;
		changed = 1;
	} else if (i->min == v->min && !i->openmin && v->openmin) {
		i->openmin = 1;
		changed = 1;
	}
	if (i->max > v->max) {
		i->max = v->max;
		i->openmax = v->openmax;
		changed = 1;
	} else if (i->max == v->max && !i->openmax && v->openmax) {
		i->openmax = 1;
		changed = 1;
	}
	if (!i->integer && v->integer) {
		i->integer = 1;
		changed = 1;
	}
	if (i->integer) {
		if (i->openmin) {
			i->min++;
			i->openmin = 0;
		}
		if (i->openmax) {
			i->max--;
			i->openmax = 0;
		}
	} else if (!i->openmin && !i->openmax && i->min == i->max)
		i->integer = 1;
	if (snd_interval_checkempty(i)) {
		snd_interval_none(i);
		return -EINVAL;
	}
	return changed;
}
EXPORT_SYMBOL(snd_interval_refine);

static int snd_interval_refine_first(struct snd_interval *i)
{
	const unsigned int last_max = i->max;

	if (snd_BUG_ON(snd_interval_empty(i)))
		return -EINVAL;
	if (snd_interval_single(i))
		return 0;
	i->max = i->min;
	if (i->openmin)
		i->max++;
	/* only exclude max value if also excluded before refine */
	i->openmax = (i->openmax && i->max >= last_max);
	return 1;
}

static int snd_interval_refine_last(struct snd_interval *i)
{
	const unsigned int last_min = i->min;

	if (snd_BUG_ON(snd_interval_empty(i)))
		return -EINVAL;
	if (snd_interval_single(i))
		return 0;
	i->min = i->max;
	if (i->openmax)
		i->min--;
	/* only exclude min value if also excluded before refine */
	i->openmin = (i->openmin && i->min <= last_min);
	return 1;
}

void snd_interval_mul(const struct snd_interval *a, const struct snd_interval *b, struct snd_interval *c)
{
	if (a->empty || b->empty) {
		snd_interval_none(c);
		return;
	}
	c->empty = 0;
	c->min = mul(a->min, b->min);
	c->openmin = (a->openmin || b->openmin);
	c->max = mul(a->max,  b->max);
	c->openmax = (a->openmax || b->openmax);
	c->integer = (a->integer && b->integer);
}

/**
 * snd_interval_div - refine the interval value with division
 * @a: dividend
 * @b: divisor
 * @c: quotient
 *
 * c = a / b
 *
 * Returns non-zero if the value is changed, zero if not changed.
 */
void snd_interval_div(const struct snd_interval *a, const struct snd_interval *b, struct snd_interval *c)
{
	unsigned int r;
	if (a->empty || b->empty) {
		snd_interval_none(c);
		return;
	}
	c->empty = 0;
	c->min = div32(a->min, b->max, &r);
	c->openmin = (r || a->openmin || b->openmax);
	if (b->min > 0) {
		c->max = div32(a->max, b->min, &r);
		if (r) {
			c->max++;
			c->openmax = 1;
		} else
			c->openmax = (a->openmax || b->openmin);
	} else {
		c->max = UINT_MAX;
		c->openmax = 0;
	}
	c->integer = 0;
}

/**
 * snd_interval_muldivk - refine the interval value
 * @a: dividend 1
 * @b: dividend 2
 * @k: divisor (as integer)
 * @c: result
  *
 * c = a * b / k
 *
 * Returns non-zero if the value is changed, zero if not changed.
 */
void snd_interval_muldivk(const struct snd_interval *a, const struct snd_interval *b,
		      unsigned int k, struct snd_interval *c)
{
	unsigned int r;
	if (a->empty || b->empty) {
		snd_interval_none(c);
		return;
	}
	c->empty = 0;
	c->min = muldiv32(a->min, b->min, k, &r);
	c->openmin = (r || a->openmin || b->openmin);
	c->max = muldiv32(a->max, b->max, k, &r);
	if (r) {
		c->max++;
		c->openmax = 1;
	} else
		c->openmax = (a->openmax || b->openmax);
	c->integer = 0;
}

/**
 * snd_interval_mulkdiv - refine the interval value
 * @a: dividend 1
 * @k: dividend 2 (as integer)
 * @b: divisor
 * @c: result
 *
 * c = a * k / b
 *
 * Returns non-zero if the value is changed, zero if not changed.
 */
void snd_interval_mulkdiv(const struct snd_interval *a, unsigned int k,
		      const struct snd_interval *b, struct snd_interval *c)
{
	unsigned int r;
	if (a->empty || b->empty) {
		snd_interval_none(c);
		return;
	}
	c->empty = 0;
	c->min = muldiv32(a->min, k, b->max, &r);
	c->openmin = (r || a->openmin || b->openmax);
	if (b->min > 0) {
		c->max = muldiv32(a->max, k, b->min, &r);
		if (r) {
			c->max++;
			c->openmax = 1;
		} else
			c->openmax = (a->openmax || b->openmin);
	} else {
		c->max = UINT_MAX;
		c->openmax = 0;
	}
	c->integer = 0;
}

/* ---- */


/**
 * snd_interval_ratnum - refine the interval value
 * @i: interval to refine
 * @rats_count: number of ratnum_t 
 * @rats: ratnum_t array
 * @nump: pointer to store the resultant numerator
 * @denp: pointer to store the resultant denominator
 *
 * Return: Positive if the value is changed, zero if it's not changed, or a
 * negative error code.
 */
int snd_interval_ratnum(struct snd_interval *i,
			unsigned int rats_count, const struct snd_ratnum *rats,
			unsigned int *nump, unsigned int *denp)
{
	unsigned int best_num, best_den;
	int best_diff;
	unsigned int k;
	struct snd_interval t;
	int err;
	unsigned int result_num, result_den;
	int result_diff;

	best_num = best_den = best_diff = 0;
	for (k = 0; k < rats_count; ++k) {
		unsigned int num = rats[k].num;
		unsigned int den;
		unsigned int q = i->min;
		int diff;
		if (q == 0)
			q = 1;
		den = div_up(num, q);
		if (den < rats[k].den_min)
			continue;
		if (den > rats[k].den_max)
			den = rats[k].den_max;
		else {
			unsigned int r;
			r = (den - rats[k].den_min) % rats[k].den_step;
			if (r != 0)
				den -= r;
		}
		diff = num - q * den;
		if (diff < 0)
			diff = -diff;
		if (best_num == 0 ||
		    diff * best_den < best_diff * den) {
			best_diff = diff;
			best_den = den;
			best_num = num;
		}
	}
	if (best_den == 0) {
		i->empty = 1;
		return -EINVAL;
	}
	t.min = div_down(best_num, best_den);
	t.openmin = !!(best_num % best_den);
	
	result_num = best_num;
	result_diff = best_diff;
	result_den = best_den;
	best_num = best_den = best_diff = 0;
	for (k = 0; k < rats_count; ++k) {
		unsigned int num = rats[k].num;
		unsigned int den;
		unsigned int q = i->max;
		int diff;
		if (q == 0) {
			i->empty = 1;
			return -EINVAL;
		}
		den = div_down(num, q);
		if (den > rats[k].den_max)
			continue;
		if (den < rats[k].den_min)
			den = rats[k].den_min;
		else {
			unsigned int r;
			r = (den - rats[k].den_min) % rats[k].den_step;
			if (r != 0)
				den += rats[k].den_step - r;
		}
		diff = q * den - num;
		if (diff < 0)
			diff = -diff;
		if (best_num == 0 ||
		    diff * best_den < best_diff * den) {
			best_diff = diff;
			best_den = den;
			best_num = num;
		}
	}
	if (best_den == 0) {
		i->empty = 1;
		return -EINVAL;
	}
	t.max = div_up(best_num, best_den);
	t.openmax = !!(best_num % best_den);
	t.integer = 0;
	err = snd_interval_refine(i, &t);
	if (err < 0)
		return err;

	if (snd_interval_single(i)) {
		if (best_diff * result_den < result_diff * best_den) {
			result_num = best_num;
			result_den = best_den;
		}
		if (nump)
			*nump = result_num;
		if (denp)
			*denp = result_den;
	}
	return err;
}
EXPORT_SYMBOL(snd_interval_ratnum);

/**
 * snd_interval_ratden - refine the interval value
 * @i: interval to refine
 * @rats_count: number of struct ratden
 * @rats: struct ratden array
 * @nump: pointer to store the resultant numerator
 * @denp: pointer to store the resultant denominator
 *
 * Return: Positive if the value is changed, zero if it's not changed, or a
 * negative error code.
 */
static int snd_interval_ratden(struct snd_interval *i,
			       unsigned int rats_count,
			       const struct snd_ratden *rats,
			       unsigned int *nump, unsigned int *denp)
{
	unsigned int best_num, best_diff, best_den;
	unsigned int k;
	struct snd_interval t;
	int err;

	best_num = best_den = best_diff = 0;
	for (k = 0; k < rats_count; ++k) {
		unsigned int num;
		unsigned int den = rats[k].den;
		unsigned int q = i->min;
		int diff;
		num = mul(q, den);
		if (num > rats[k].num_max)
			continue;
		if (num < rats[k].num_min)
			num = rats[k].num_max;
		else {
			unsigned int r;
			r = (num - rats[k].num_min) % rats[k].num_step;
			if (r != 0)
				num += rats[k].num_step - r;
		}
		diff = num - q * den;
		if (best_num == 0 ||
		    diff * best_den < best_diff * den) {
			best_diff = diff;
			best_den = den;
			best_num = num;
		}
	}
	if (best_den == 0) {
		i->empty = 1;
		return -EINVAL;
	}
	t.min = div_down(best_num, best_den);
	t.openmin = !!(best_num % best_den);
	
	best_num = best_den = best_diff = 0;
	for (k = 0; k < rats_count; ++k) {
		unsigned int num;
		unsigned int den = rats[k].den;
		unsigned int q = i->max;
		int diff;
		num = mul(q, den);
		if (num < rats[k].num_min)
			continue;
		if (num > rats[k].num_max)
			num = rats[k].num_max;
		else {
			unsigned int r;
			r = (num - rats[k].num_min) % rats[k].num_step;
			if (r != 0)
				num -= r;
		}
		diff = q * den - num;
		if (best_num == 0 ||
		    diff * best_den < best_diff * den) {
			best_diff = diff;
			best_den = den;
			best_num = num;
		}
	}
	if (best_den == 0) {
		i->empty = 1;
		return -EINVAL;
	}
	t.max = div_up(best_num, best_den);
	t.openmax = !!(best_num % best_den);
	t.integer = 0;
	err = snd_interval_refine(i, &t);
	if (err < 0)
		return err;

	if (snd_interval_single(i)) {
		if (nump)
			*nump = best_num;
		if (denp)
			*denp = best_den;
	}
	return err;
}

/**
 * snd_interval_list - refine the interval value from the list
 * @i: the interval value to refine
 * @count: the number of elements in the list
 * @list: the value list
 * @mask: the bit-mask to evaluate
 *
 * Refines the interval value from the list.
 * When mask is non-zero, only the elements corresponding to bit 1 are
 * evaluated.
 *
 * Return: Positive if the value is changed, zero if it's not changed, or a
 * negative error code.
 */
int snd_interval_list(struct snd_interval *i, unsigned int count,
		      const unsigned int *list, unsigned int mask)
{
        unsigned int k;
	struct snd_interval list_range;

	if (!count) {
		i->empty = 1;
		return -EINVAL;
	}
	snd_interval_any(&list_range);
	list_range.min = UINT_MAX;
	list_range.max = 0;
        for (k = 0; k < count; k++) {
		if (mask && !(mask & (1 << k)))
			continue;
		if (!snd_interval_test(i, list[k]))
			continue;
		list_range.min = min(list_range.min, list[k]);
		list_range.max = max(list_range.max, list[k]);
        }
	return snd_interval_refine(i, &list_range);
}
EXPORT_SYMBOL(snd_interval_list);

/**
 * snd_interval_ranges - refine the interval value from the list of ranges
 * @i: the interval value to refine
 * @count: the number of elements in the list of ranges
 * @ranges: the ranges list
 * @mask: the bit-mask to evaluate
 *
 * Refines the interval value from the list of ranges.
 * When mask is non-zero, only the elements corresponding to bit 1 are
 * evaluated.
 *
 * Return: Positive if the value is changed, zero if it's not changed, or a
 * negative error code.
 */
int snd_interval_ranges(struct snd_interval *i, unsigned int count,
			const struct snd_interval *ranges, unsigned int mask)
{
	unsigned int k;
	struct snd_interval range_union;
	struct snd_interval range;

	if (!count) {
		snd_interval_none(i);
		return -EINVAL;
	}
	snd_interval_any(&range_union);
	range_union.min = UINT_MAX;
	range_union.max = 0;
	for (k = 0; k < count; k++) {
		if (mask && !(mask & (1 << k)))
			continue;
		snd_interval_copy(&range, &ranges[k]);
		if (snd_interval_refine(&range, i) < 0)
			continue;
		if (snd_interval_empty(&range))
			continue;

		if (range.min < range_union.min) {
			range_union.min = range.min;
			range_union.openmin = 1;
		}
		if (range.min == range_union.min && !range.openmin)
			range_union.openmin = 0;
		if (range.max > range_union.max) {
			range_union.max = range.max;
			range_union.openmax = 1;
		}
		if (range.max == range_union.max && !range.openmax)
			range_union.openmax = 0;
	}
	return snd_interval_refine(i, &range_union);
}
EXPORT_SYMBOL(snd_interval_ranges);

static int snd_interval_step(struct snd_interval *i, unsigned int step)
{
	unsigned int n;
	int changed = 0;
	n = i->min % step;
	if (n != 0 || i->openmin) {
		i->min += step - n;
		i->openmin = 0;
		changed = 1;
	}
	n = i->max % step;
	if (n != 0 || i->openmax) {
		i->max -= n;
		i->openmax = 0;
		changed = 1;
	}
	if (snd_interval_checkempty(i)) {
		i->empty = 1;
		return -EINVAL;
	}
	return changed;
}

/* Info constraints helpers */

/**
 * snd_pcm_hw_rule_add - add the hw-constraint rule
 * @runtime: the pcm runtime instance
 * @cond: condition bits
 * @var: the variable to evaluate
 * @func: the evaluation function
 * @private: the private data pointer passed to function
 * @dep: the dependent variables
 *
 * Return: Zero if successful, or a negative error code on failure.
 */
int snd_pcm_hw_rule_add(struct snd_pcm_runtime *runtime, unsigned int cond,
			int var,
			snd_pcm_hw_rule_func_t func, void *private,
			int dep, ...)
{
	struct snd_pcm_hw_constraints *constrs = &runtime->hw_constraints;
	struct snd_pcm_hw_rule *c;
	unsigned int k;
	va_list args;
	va_start(args, dep);
	if (constrs->rules_num >= constrs->rules_all) {
		struct snd_pcm_hw_rule *new;
		unsigned int new_rules = constrs->rules_all + 16;
		new = krealloc_array(constrs->rules, new_rules,
				     sizeof(*c), GFP_KERNEL);
		if (!new) {
			va_end(args);
			return -ENOMEM;
		}
		constrs->rules = new;
		constrs->rules_all = new_rules;
	}
	c = &constrs->rules[constrs->rules_num];
	c->cond = cond;
	c->func = func;
	c->var = var;
	c->private = private;
	k = 0;
	while (1) {
		if (snd_BUG_ON(k >= ARRAY_SIZE(c->deps))) {
			va_end(args);
			return -EINVAL;
		}
		c->deps[k++] = dep;
		if (dep < 0)
			break;
		dep = va_arg(args, int);
	}
	constrs->rules_num++;
	va_end(args);
	return 0;
}
EXPORT_SYMBOL(snd_pcm_hw_rule_add);

/**
 * snd_pcm_hw_constraint_mask - apply the given bitmap mask constraint
 * @runtime: PCM runtime instance
 * @var: hw_params variable to apply the mask
 * @mask: the bitmap mask
 *
 * Apply the constraint of the given bitmap mask to a 32-bit mask parameter.
 *
 * Return: Zero if successful, or a negative error code on failure.
 */
int snd_pcm_hw_constraint_mask(struct snd_pcm_runtime *runtime, snd_pcm_hw_param_t var,
			       u_int32_t mask)
{
	struct snd_pcm_hw_constraints *constrs = &runtime->hw_constraints;
	struct snd_mask *maskp = constrs_mask(constrs, var);
	*maskp->bits &= mask;
	memset(maskp->bits + 1, 0, (SNDRV_MASK_MAX-32) / 8); /* clear rest */
	if (*maskp->bits == 0)
		return -EINVAL;
	return 0;
}

/**
 * snd_pcm_hw_constraint_mask64 - apply the given bitmap mask constraint
 * @runtime: PCM runtime instance
 * @var: hw_params variable to apply the mask
 * @mask: the 64bit bitmap mask
 *
 * Apply the constraint of the given bitmap mask to a 64-bit mask parameter.
 *
 * Return: Zero if successful, or a negative error code on failure.
 */
int snd_pcm_hw_constraint_mask64(struct snd_pcm_runtime *runtime, snd_pcm_hw_param_t var,
				 u_int64_t mask)
{
	struct snd_pcm_hw_constraints *constrs = &runtime->hw_constraints;
	struct snd_mask *maskp = constrs_mask(constrs, var);
	maskp->bits[0] &= (u_int32_t)mask;
	maskp->bits[1] &= (u_int32_t)(mask >> 32);
	memset(maskp->bits + 2, 0, (SNDRV_MASK_MAX-64) / 8); /* clear rest */
	if (! maskp->bits[0] && ! maskp->bits[1])
		return -EINVAL;
	return 0;
}
EXPORT_SYMBOL(snd_pcm_hw_constraint_mask64);

/**
 * snd_pcm_hw_constraint_integer - apply an integer constraint to an interval
 * @runtime: PCM runtime instance
 * @var: hw_params variable to apply the integer constraint
 *
 * Apply the constraint of integer to an interval parameter.
 *
 * Return: Positive if the value is changed, zero if it's not changed, or a
 * negative error code.
 */
int snd_pcm_hw_constraint_integer(struct snd_pcm_runtime *runtime, snd_pcm_hw_param_t var)
{
	struct snd_pcm_hw_constraints *constrs = &runtime->hw_constraints;
	return snd_interval_setinteger(constrs_interval(constrs, var));
}
EXPORT_SYMBOL(snd_pcm_hw_constraint_integer);

/**
 * snd_pcm_hw_constraint_minmax - apply a min/max range constraint to an interval
 * @runtime: PCM runtime instance
 * @var: hw_params variable to apply the range
 * @min: the minimal value
 * @max: the maximal value
 * 
 * Apply the min/max range constraint to an interval parameter.
 *
 * Return: Positive if the value is changed, zero if it's not changed, or a
 * negative error code.
 */
int snd_pcm_hw_constraint_minmax(struct snd_pcm_runtime *runtime, snd_pcm_hw_param_t var,
				 unsigned int min, unsigned int max)
{
	struct snd_pcm_hw_constraints *constrs = &runtime->hw_constraints;
	struct snd_interval t;
	t.min = min;
	t.max = max;
	t.openmin = t.openmax = 0;
	t.integer = 0;
	return snd_interval_refine(constrs_interval(constrs, var), &t);
}
EXPORT_SYMBOL(snd_pcm_hw_constraint_minmax);

static int snd_pcm_hw_rule_list(struct snd_pcm_hw_params *params,
				struct snd_pcm_hw_rule *rule)
{
	struct snd_pcm_hw_constraint_list *list = rule->private;
	return snd_interval_list(hw_param_interval(params, rule->var), list->count, list->list, list->mask);
}		


/**
 * snd_pcm_hw_constraint_list - apply a list of constraints to a parameter
 * @runtime: PCM runtime instance
 * @cond: condition bits
 * @var: hw_params variable to apply the list constraint
 * @l: list
 * 
 * Apply the list of constraints to an interval parameter.
 *
 * Return: Zero if successful, or a negative error code on failure.
 */
int snd_pcm_hw_constraint_list(struct snd_pcm_runtime *runtime,
			       unsigned int cond,
			       snd_pcm_hw_param_t var,
			       const struct snd_pcm_hw_constraint_list *l)
{
	return snd_pcm_hw_rule_add(runtime, cond, var,
				   snd_pcm_hw_rule_list, (void *)l,
				   var, -1);
}
EXPORT_SYMBOL(snd_pcm_hw_constraint_list);

static int snd_pcm_hw_rule_ranges(struct snd_pcm_hw_params *params,
				  struct snd_pcm_hw_rule *rule)
{
	struct snd_pcm_hw_constraint_ranges *r = rule->private;
	return snd_interval_ranges(hw_param_interval(params, rule->var),
				   r->count, r->ranges, r->mask);
}


/**
 * snd_pcm_hw_constraint_ranges - apply list of range constraints to a parameter
 * @runtime: PCM runtime instance
 * @cond: condition bits
 * @var: hw_params variable to apply the list of range constraints
 * @r: ranges
 *
 * Apply the list of range constraints to an interval parameter.
 *
 * Return: Zero if successful, or a negative error code on failure.
 */
int snd_pcm_hw_constraint_ranges(struct snd_pcm_runtime *runtime,
				 unsigned int cond,
				 snd_pcm_hw_param_t var,
				 const struct snd_pcm_hw_constraint_ranges *r)
{
	return snd_pcm_hw_rule_add(runtime, cond, var,
				   snd_pcm_hw_rule_ranges, (void *)r,
				   var, -1);
}
EXPORT_SYMBOL(snd_pcm_hw_constraint_ranges);

static int snd_pcm_hw_rule_ratnums(struct snd_pcm_hw_params *params,
				   struct snd_pcm_hw_rule *rule)
{
	const struct snd_pcm_hw_constraint_ratnums *r = rule->private;
	unsigned int num = 0, den = 0;
	int err;
	err = snd_interval_ratnum(hw_param_interval(params, rule->var),
				  r->nrats, r->rats, &num, &den);
	if (err >= 0 && den && rule->var == SNDRV_PCM_HW_PARAM_RATE) {
		params->rate_num = num;
		params->rate_den = den;
	}
	return err;
}

/**
 * snd_pcm_hw_constraint_ratnums - apply ratnums constraint to a parameter
 * @runtime: PCM runtime instance
 * @cond: condition bits
 * @var: hw_params variable to apply the ratnums constraint
 * @r: struct snd_ratnums constriants
 *
 * Return: Zero if successful, or a negative error code on failure.
 */
int snd_pcm_hw_constraint_ratnums(struct snd_pcm_runtime *runtime, 
				  unsigned int cond,
				  snd_pcm_hw_param_t var,
				  const struct snd_pcm_hw_constraint_ratnums *r)
{
	return snd_pcm_hw_rule_add(runtime, cond, var,
				   snd_pcm_hw_rule_ratnums, (void *)r,
				   var, -1);
}
EXPORT_SYMBOL(snd_pcm_hw_constraint_ratnums);

static int snd_pcm_hw_rule_ratdens(struct snd_pcm_hw_params *params,
				   struct snd_pcm_hw_rule *rule)
{
	const struct snd_pcm_hw_constraint_ratdens *r = rule->private;
	unsigned int num = 0, den = 0;
	int err = snd_interval_ratden(hw_param_interval(params, rule->var),
				  r->nrats, r->rats, &num, &den);
	if (err >= 0 && den && rule->var == SNDRV_PCM_HW_PARAM_RATE) {
		params->rate_num = num;
		params->rate_den = den;
	}
	return err;
}

/**
 * snd_pcm_hw_constraint_ratdens - apply ratdens constraint to a parameter
 * @runtime: PCM runtime instance
 * @cond: condition bits
 * @var: hw_params variable to apply the ratdens constraint
 * @r: struct snd_ratdens constriants
 *
 * Return: Zero if successful, or a negative error code on failure.
 */
int snd_pcm_hw_constraint_ratdens(struct snd_pcm_runtime *runtime, 
				  unsigned int cond,
				  snd_pcm_hw_param_t var,
				  const struct snd_pcm_hw_constraint_ratdens *r)
{
	return snd_pcm_hw_rule_add(runtime, cond, var,
				   snd_pcm_hw_rule_ratdens, (void *)r,
				   var, -1);
}
EXPORT_SYMBOL(snd_pcm_hw_constraint_ratdens);

static int snd_pcm_hw_rule_msbits(struct snd_pcm_hw_params *params,
				  struct snd_pcm_hw_rule *rule)
{
	unsigned int l = (unsigned long) rule->private;
	int width = l & 0xffff;
	unsigned int msbits = l >> 16;
	const struct snd_interval *i =
		hw_param_interval_c(params, SNDRV_PCM_HW_PARAM_SAMPLE_BITS);

	if (!snd_interval_single(i))
		return 0;

	if ((snd_interval_value(i) == width) ||
	    (width == 0 && snd_interval_value(i) > msbits))
		params->msbits = min_not_zero(params->msbits, msbits);

	return 0;
}

/**
 * snd_pcm_hw_constraint_msbits - add a hw constraint msbits rule
 * @runtime: PCM runtime instance
 * @cond: condition bits
 * @width: sample bits width
 * @msbits: msbits width
 *
 * This constraint will set the number of most significant bits (msbits) if a
 * sample format with the specified width has been select. If width is set to 0
 * the msbits will be set for any sample format with a width larger than the
 * specified msbits.
 *
 * Return: Zero if successful, or a negative error code on failure.
 */
int snd_pcm_hw_constraint_msbits(struct snd_pcm_runtime *runtime, 
				 unsigned int cond,
				 unsigned int width,
				 unsigned int msbits)
{
	unsigned long l = (msbits << 16) | width;
	return snd_pcm_hw_rule_add(runtime, cond, -1,
				    snd_pcm_hw_rule_msbits,
				    (void*) l,
				    SNDRV_PCM_HW_PARAM_SAMPLE_BITS, -1);
}
EXPORT_SYMBOL(snd_pcm_hw_constraint_msbits);

static int snd_pcm_hw_rule_step(struct snd_pcm_hw_params *params,
				struct snd_pcm_hw_rule *rule)
{
	unsigned long step = (unsigned long) rule->private;
	return snd_interval_step(hw_param_interval(params, rule->var), step);
}

/**
 * snd_pcm_hw_constraint_step - add a hw constraint step rule
 * @runtime: PCM runtime instance
 * @cond: condition bits
 * @var: hw_params variable to apply the step constraint
 * @step: step size
 *
 * Return: Zero if successful, or a negative error code on failure.
 */
int snd_pcm_hw_constraint_step(struct snd_pcm_runtime *runtime,
			       unsigned int cond,
			       snd_pcm_hw_param_t var,
			       unsigned long step)
{
	return snd_pcm_hw_rule_add(runtime, cond, var, 
				   snd_pcm_hw_rule_step, (void *) step,
				   var, -1);
}
EXPORT_SYMBOL(snd_pcm_hw_constraint_step);

static int snd_pcm_hw_rule_pow2(struct snd_pcm_hw_params *params, struct snd_pcm_hw_rule *rule)
{
	static const unsigned int pow2_sizes[] = {
		1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7,
		1<<8, 1<<9, 1<<10, 1<<11, 1<<12, 1<<13, 1<<14, 1<<15,
		1<<16, 1<<17, 1<<18, 1<<19, 1<<20, 1<<21, 1<<22, 1<<23,
		1<<24, 1<<25, 1<<26, 1<<27, 1<<28, 1<<29, 1<<30
	};
	return snd_interval_list(hw_param_interval(params, rule->var),
				 ARRAY_SIZE(pow2_sizes), pow2_sizes, 0);
}		

/**
 * snd_pcm_hw_constraint_pow2 - add a hw constraint power-of-2 rule
 * @runtime: PCM runtime instance
 * @cond: condition bits
 * @var: hw_params variable to apply the power-of-2 constraint
 *
 * Return: Zero if successful, or a negative error code on failure.
 */
int snd_pcm_hw_constraint_pow2(struct snd_pcm_runtime *runtime,
			       unsigned int cond,
			       snd_pcm_hw_param_t var)
{
	return snd_pcm_hw_rule_add(runtime, cond, var, 
				   snd_pcm_hw_rule_pow2, NULL,
				   var, -1);
}
EXPORT_SYMBOL(snd_pcm_hw_constraint_pow2);

static int snd_pcm_hw_rule_noresample_func(struct snd_pcm_hw_params *params,
					   struct snd_pcm_hw_rule *rule)
{
	unsigned int base_rate = (unsigned int)(uintptr_t)rule->private;
	struct snd_interval *rate;

	rate = hw_param_interval(params, SNDRV_PCM_HW_PARAM_RATE);
	return snd_interval_list(rate, 1, &base_rate, 0);
}

/**
 * snd_pcm_hw_rule_noresample - add a rule to allow disabling hw resampling
 * @runtime: PCM runtime instance
 * @base_rate: the rate at which the hardware does not resample
 *
 * Return: Zero if successful, or a negative error code on failure.
 */
int snd_pcm_hw_rule_noresample(struct snd_pcm_runtime *runtime,
			       unsigned int base_rate)
{
	return snd_pcm_hw_rule_add(runtime, SNDRV_PCM_HW_PARAMS_NORESAMPLE,
				   SNDRV_PCM_HW_PARAM_RATE,
				   snd_pcm_hw_rule_noresample_func,
				   (void *)(uintptr_t)base_rate,
				   SNDRV_PCM_HW_PARAM_RATE, -1);
}
EXPORT_SYMBOL(snd_pcm_hw_rule_noresample);

static void _snd_pcm_hw_param_any(struct snd_pcm_hw_params *params,
				  snd_pcm_hw_param_t var)
{
	if (hw_is_mask(var)) {
		snd_mask_any(hw_param_mask(params, var));
		params->cmask |= 1 << var;
		params->rmask |= 1 << var;
		return;
	}
	if (hw_is_interval(var)) {
		snd_interval_any(hw_param_interval(params, var));
		params->cmask |= 1 << var;
		params->rmask |= 1 << var;
		return;
	}
	snd_BUG();
}

void _snd_pcm_hw_params_any(struct snd_pcm_hw_params *params)
{
	unsigned int k;
	memset(params, 0, sizeof(*params));
	for (k = SNDRV_PCM_HW_PARAM_FIRST_MASK; k <= SNDRV_PCM_HW_PARAM_LAST_MASK; k++)
		_snd_pcm_hw_param_any(params, k);
	for (k = SNDRV_PCM_HW_PARAM_FIRST_INTERVAL; k <= SNDRV_PCM_HW_PARAM_LAST_INTERVAL; k++)
		_snd_pcm_hw_param_any(params, k);
	params->info = ~0U;
}
EXPORT_SYMBOL(_snd_pcm_hw_params_any);

/**
 * snd_pcm_hw_param_value - return @params field @var value
 * @params: the hw_params instance
 * @var: parameter to retrieve
 * @dir: pointer to the direction (-1,0,1) or %NULL
 *
 * Return: The value for field @var if it's fixed in configuration space
 * defined by @params. -%EINVAL otherwise.
 */
int snd_pcm_hw_param_value(const struct snd_pcm_hw_params *params,
			   snd_pcm_hw_param_t var, int *dir)
{
	if (hw_is_mask(var)) {
		const struct snd_mask *mask = hw_param_mask_c(params, var);
		if (!snd_mask_single(mask))
			return -EINVAL;
		if (dir)
			*dir = 0;
		return snd_mask_value(mask);
	}
	if (hw_is_interval(var)) {
		const struct snd_interval *i = hw_param_interval_c(params, var);
		if (!snd_interval_single(i))
			return -EINVAL;
		if (dir)
			*dir = i->openmin;
		return snd_interval_value(i);
	}
	return -EINVAL;
}
EXPORT_SYMBOL(snd_pcm_hw_param_value);

void _snd_pcm_hw_param_setempty(struct snd_pcm_hw_params *params,
				snd_pcm_hw_param_t var)
{
	if (hw_is_mask(var)) {
		snd_mask_none(hw_param_mask(params, var));
		params->cmask |= 1 << var;
		params->rmask |= 1 << var;
	} else if (hw_is_interval(var)) {
		snd_interval_none(hw_param_interval(params, var));
		params->cmask |= 1 << var;
		params->rmask |= 1 << var;
	} else {
		snd_BUG();
	}
}
EXPORT_SYMBOL(_snd_pcm_hw_param_setempty);

static int _snd_pcm_hw_param_first(struct snd_pcm_hw_params *params,
				   snd_pcm_hw_param_t var)
{
	int changed;
	if (hw_is_mask(var))
		changed = snd_mask_refine_first(hw_param_mask(params, var));
	else if (hw_is_interval(var))
		changed = snd_interval_refine_first(hw_param_interval(params, var));
	else
		return -EINVAL;
	if (changed > 0) {
		params->cmask |= 1 << var;
		params->rmask |= 1 << var;
	}
	return changed;
}


/**
 * snd_pcm_hw_param_first - refine config space and return minimum value
 * @pcm: PCM instance
 * @params: the hw_params instance
 * @var: parameter to retrieve
 * @dir: pointer to the direction (-1,0,1) or %NULL
 *
 * Inside configuration space defined by @params remove from @var all
 * values > minimum. Reduce configuration space accordingly.
 *
 * Return: The minimum, or a negative error code on failure.
 */
int snd_pcm_hw_param_first(struct snd_pcm_substream *pcm, 
			   struct snd_pcm_hw_params *params, 
			   snd_pcm_hw_param_t var, int *dir)
{
	int changed = _snd_pcm_hw_param_first(params, var);
	if (changed < 0)
		return changed;
	if (params->rmask) {
		int err = snd_pcm_hw_refine(pcm, params);
		if (err < 0)
			return err;
	}
	return snd_pcm_hw_param_value(params, var, dir);
}
EXPORT_SYMBOL(snd_pcm_hw_param_first);

static int _snd_pcm_hw_param_last(struct snd_pcm_hw_params *params,
				  snd_pcm_hw_param_t var)
{
	int changed;
	if (hw_is_mask(var))
		changed = snd_mask_refine_last(hw_param_mask(params, var));
	else if (hw_is_interval(var))
		changed = snd_interval_refine_last(hw_param_interval(params, var));
	else
		return -EINVAL;
	if (changed > 0) {
		params->cmask |= 1 << var;
		params->rmask |= 1 << var;
	}
	return changed;
}


/**
 * snd_pcm_hw_param_last - refine config space and return maximum value
 * @pcm: PCM instance
 * @params: the hw_params instance
 * @var: parameter to retrieve
 * @dir: pointer to the direction (-1,0,1) or %NULL
 *
 * Inside configuration space defined by @params remove from @var all
 * values < maximum. Reduce configuration space accordingly.
 *
 * Return: The maximum, or a negative error code on failure.
 */
int snd_pcm_hw_param_last(struct snd_pcm_substream *pcm, 
			  struct snd_pcm_hw_params *params,
			  snd_pcm_hw_param_t var, int *dir)
{
	int changed = _snd_pcm_hw_param_last(params, var);
	if (changed < 0)
		return changed;
	if (params->rmask) {
		int err = snd_pcm_hw_refine(pcm, params);
		if (err < 0)
			return err;
	}
	return snd_pcm_hw_param_value(params, var, dir);
}
EXPORT_SYMBOL(snd_pcm_hw_param_last);

static int snd_pcm_lib_ioctl_reset(struct snd_pcm_substream *substream,
				   void *arg)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	unsigned long flags;
	snd_pcm_stream_lock_irqsave(substream, flags);
	if (snd_pcm_running(substream) &&
	    snd_pcm_update_hw_ptr(substream) >= 0)
		runtime->status->hw_ptr %= runtime->buffer_size;
	else {
		runtime->status->hw_ptr = 0;
		runtime->hw_ptr_wrap = 0;
	}
	snd_pcm_stream_unlock_irqrestore(substream, flags);
	return 0;
}

static int snd_pcm_lib_ioctl_channel_info(struct snd_pcm_substream *substream,
					  void *arg)
{
	struct snd_pcm_channel_info *info = arg;
	struct snd_pcm_runtime *runtime = substream->runtime;
	int width;
	if (!(runtime->info & SNDRV_PCM_INFO_MMAP)) {
		info->offset = -1;
		return 0;
	}
	width = snd_pcm_format_physical_width(runtime->format);
	if (width < 0)
		return width;
	info->offset = 0;
	switch (runtime->access) {
	case SNDRV_PCM_ACCESS_MMAP_INTERLEAVED:
	case SNDRV_PCM_ACCESS_RW_INTERLEAVED:
		info->first = info->channel * width;
		info->step = runtime->channels * width;
		break;
	case SNDRV_PCM_ACCESS_MMAP_NONINTERLEAVED:
	case SNDRV_PCM_ACCESS_RW_NONINTERLEAVED:
	{
		size_t size = runtime->dma_bytes / runtime->channels;
		info->first = info->channel * size * 8;
		info->step = width;
		break;
	}
	default:
		snd_BUG();
		break;
	}
	return 0;
}

static int snd_pcm_lib_ioctl_fifo_size(struct snd_pcm_substream *substream,
				       void *arg)
{
	struct snd_pcm_hw_params *params = arg;
	snd_pcm_format_t format;
	int channels;
	ssize_t frame_size;

	params->fifo_size = substream->runtime->hw.fifo_size;
	if (!(substream->runtime->hw.info & SNDRV_PCM_INFO_FIFO_IN_FRAMES)) {
		format = params_format(params);
		channels = params_channels(params);
		frame_size = snd_pcm_format_size(format, channels);
		if (frame_size > 0)
			params->fifo_size /= (unsigned)frame_size;
	}
	return 0;
}

/**
 * snd_pcm_lib_ioctl - a generic PCM ioctl callback
 * @substream: the pcm substream instance
 * @cmd: ioctl command
 * @arg: ioctl argument
 *
 * Processes the generic ioctl commands for PCM.
 * Can be passed as the ioctl callback for PCM ops.
 *
 * Return: Zero if successful, or a negative error code on failure.
 */
int snd_pcm_lib_ioctl(struct snd_pcm_substream *substream,
		      unsigned int cmd, void *arg)
{
	switch (cmd) {
	case SNDRV_PCM_IOCTL1_RESET:
		return snd_pcm_lib_ioctl_reset(substream, arg);
	case SNDRV_PCM_IOCTL1_CHANNEL_INFO:
		return snd_pcm_lib_ioctl_channel_info(substream, arg);
	case SNDRV_PCM_IOCTL1_FIFO_SIZE:
		return snd_pcm_lib_ioctl_fifo_size(substream, arg);
	}
	return -ENXIO;
}
EXPORT_SYMBOL(snd_pcm_lib_ioctl);

/**
 * snd_pcm_period_elapsed - update the pcm status for the next period
 * @substream: the pcm substream instance
 *
 * This function is called from the interrupt handler when the
 * PCM has processed the period size.  It will update the current
 * pointer, wake up sleepers, etc.
 *
 * Even if more than one periods have elapsed since the last call, you
 * have to call this only once.
 */
void snd_pcm_period_elapsed(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime;
	unsigned long flags;

	if (snd_BUG_ON(!substream))
		return;

	snd_pcm_stream_lock_irqsave(substream, flags);
	if (PCM_RUNTIME_CHECK(substream))
		goto _unlock;
	runtime = substream->runtime;

	if (!snd_pcm_running(substream) ||
	    snd_pcm_update_hw_ptr0(substream, 1) < 0)
		goto _end;

#ifdef CONFIG_SND_PCM_TIMER
	if (substream->timer_running)
		snd_timer_interrupt(substream->timer, 1);
#endif
 _end:
	kill_fasync(&runtime->fasync, SIGIO, POLL_IN);
 _unlock:
	snd_pcm_stream_unlock_irqrestore(substream, flags);
}
EXPORT_SYMBOL(snd_pcm_period_elapsed);

/*
 * Wait until avail_min data becomes available
 * Returns a negative error code if any error occurs during operation.
 * The available space is stored on availp.  When err = 0 and avail = 0
 * on the capture stream, it indicates the stream is in DRAINING state.
 */
static int wait_for_avail(struct snd_pcm_substream *substream,
			      snd_pcm_uframes_t *availp)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	int is_playback = substream->stream == SNDRV_PCM_STREAM_PLAYBACK;
	wait_queue_entry_t wait;
	int err = 0;
	snd_pcm_uframes_t avail = 0;
	long wait_time, tout;

	init_waitqueue_entry(&wait, current);
	set_current_state(TASK_INTERRUPTIBLE);
	add_wait_queue(&runtime->tsleep, &wait);

	if (runtime->no_period_wakeup)
		wait_time = MAX_SCHEDULE_TIMEOUT;
	else {
		/* use wait time from substream if available */
		if (substream->wait_time) {
			wait_time = substream->wait_time;
		} else {
			wait_time = 10;

			if (runtime->rate) {
				long t = runtime->period_size * 2 /
					 runtime->rate;
				wait_time = max(t, wait_time);
			}
			wait_time = msecs_to_jiffies(wait_time * 1000);
		}
	}

	for (;;) {
		if (signal_pending(current)) {
			err = -ERESTARTSYS;
			break;
		}

		/*
		 * We need to check if space became available already
		 * (and thus the wakeup happened already) first to close
		 * the race of space already having become available.
		 * This check must happen after been added to the waitqueue
		 * and having current state be INTERRUPTIBLE.
		 */
		avail = snd_pcm_avail(substream);
		if (avail >= runtime->twake)
			break;
		snd_pcm_stream_unlock_irq(substream);

		tout = schedule_timeout(wait_time);

		snd_pcm_stream_lock_irq(substream);
		set_current_state(TASK_INTERRUPTIBLE);
		switch (runtime->status->state) {
		case SNDRV_PCM_STATE_SUSPENDED:
			err = -ESTRPIPE;
			goto _endloop;
		case SNDRV_PCM_STATE_XRUN:
			err = -EPIPE;
			goto _endloop;
		case SNDRV_PCM_STATE_DRAINING:
			if (is_playback)
				err = -EPIPE;
			else 
				avail = 0; /* indicate draining */
			goto _endloop;
		case SNDRV_PCM_STATE_OPEN:
		case SNDRV_PCM_STATE_SETUP:
		case SNDRV_PCM_STATE_DISCONNECTED:
			err = -EBADFD;
			goto _endloop;
		case SNDRV_PCM_STATE_PAUSED:
			continue;
		}
		if (!tout) {
			pcm_dbg(substream->pcm,
				"%s write error (DMA or IRQ trouble?)\n",
				is_playback ? "playback" : "capture");
			err = -EIO;
			break;
		}
	}
 _endloop:
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&runtime->tsleep, &wait);
	*availp = avail;
	return err;
}
	
typedef int (*pcm_transfer_f)(struct snd_pcm_substream *substream,
			      int channel, unsigned long hwoff,
			      void *buf, unsigned long bytes);

typedef int (*pcm_copy_f)(struct snd_pcm_substream *, snd_pcm_uframes_t, void *,
			  snd_pcm_uframes_t, snd_pcm_uframes_t, pcm_transfer_f);

/* calculate the target DMA-buffer position to be written/read */
static void *get_dma_ptr(struct snd_pcm_runtime *runtime,
			   int channel, unsigned long hwoff)
{
	return runtime->dma_area + hwoff +
		channel * (runtime->dma_bytes / runtime->channels);
}

/* default copy_user ops for write; used for both interleaved and non- modes */
static int default_write_copy(struct snd_pcm_substream *substream,
			      int channel, unsigned long hwoff,
			      void *buf, unsigned long bytes)
{
	if (copy_from_user(get_dma_ptr(substream->runtime, channel, hwoff),
			   (void __user *)buf, bytes))
		return -EFAULT;
	return 0;
}

/* default copy_kernel ops for write */
static int default_write_copy_kernel(struct snd_pcm_substream *substream,
				     int channel, unsigned long hwoff,
				     void *buf, unsigned long bytes)
{
	memcpy(get_dma_ptr(substream->runtime, channel, hwoff), buf, bytes);
	return 0;
}

/* fill silence instead of copy data; called as a transfer helper
 * from __snd_pcm_lib_write() or directly from noninterleaved_copy() when
 * a NULL buffer is passed
 */
static int fill_silence(struct snd_pcm_substream *substream, int channel,
			unsigned long hwoff, void *buf, unsigned long bytes)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	if (substream->stream != SNDRV_PCM_STREAM_PLAYBACK)
		return 0;
	if (substream->ops->fill_silence)
		return substream->ops->fill_silence(substream, channel,
						    hwoff, bytes);

	snd_pcm_format_set_silence(runtime->format,
				   get_dma_ptr(runtime, channel, hwoff),
				   bytes_to_samples(runtime, bytes));
	return 0;
}

/* default copy_user ops for read; used for both interleaved and non- modes */
static int default_read_copy(struct snd_pcm_substream *substream,
			     int channel, unsigned long hwoff,
			     void *buf, unsigned long bytes)
{
	if (copy_to_user((void __user *)buf,
			 get_dma_ptr(substream->runtime, channel, hwoff),
			 bytes))
		return -EFAULT;
	return 0;
}

/* default copy_kernel ops for read */
static int default_read_copy_kernel(struct snd_pcm_substream *substream,
				    int channel, unsigned long hwoff,
				    void *buf, unsigned long bytes)
{
	memcpy(buf, get_dma_ptr(substream->runtime, channel, hwoff), bytes);
	return 0;
}

/* call transfer function with the converted pointers and sizes;
 * for interleaved mode, it's one shot for all samples
 */
static int interleaved_copy(struct snd_pcm_substream *substream,
			    snd_pcm_uframes_t hwoff, void *data,
			    snd_pcm_uframes_t off,
			    snd_pcm_uframes_t frames,
			    pcm_transfer_f transfer)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	/* convert to bytes */
	hwoff = frames_to_bytes(runtime, hwoff);
	off = frames_to_bytes(runtime, off);
	frames = frames_to_bytes(runtime, frames);
	return transfer(substream, 0, hwoff, data + off, frames);
}

/* call transfer function with the converted pointers and sizes for each
 * non-interleaved channel; when buffer is NULL, silencing instead of copying
 */
static int noninterleaved_copy(struct snd_pcm_substream *substream,
			       snd_pcm_uframes_t hwoff, void *data,
			       snd_pcm_uframes_t off,
			       snd_pcm_uframes_t frames,
			       pcm_transfer_f transfer)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	int channels = runtime->channels;
	void **bufs = data;
	int c, err;

	/* convert to bytes; note that it's not frames_to_bytes() here.
	 * in non-interleaved mode, we copy for each channel, thus
	 * each copy is n_samples bytes x channels = whole frames.
	 */
	off = samples_to_bytes(runtime, off);
	frames = samples_to_bytes(runtime, frames);
	hwoff = samples_to_bytes(runtime, hwoff);
	for (c = 0; c < channels; ++c, ++bufs) {
		if (!data || !*bufs)
			err = fill_silence(substream, c, hwoff, NULL, frames);
		else
			err = transfer(substream, c, hwoff, *bufs + off,
				       frames);
		if (err < 0)
			return err;
	}
	return 0;
}

/* fill silence on the given buffer position;
 * called from snd_pcm_playback_silence()
 */
static int fill_silence_frames(struct snd_pcm_substream *substream,
			       snd_pcm_uframes_t off, snd_pcm_uframes_t frames)
{
	if (substream->runtime->access == SNDRV_PCM_ACCESS_RW_INTERLEAVED ||
	    substream->runtime->access == SNDRV_PCM_ACCESS_MMAP_INTERLEAVED)
		return interleaved_copy(substream, off, NULL, 0, frames,
					fill_silence);
	else
		return noninterleaved_copy(substream, off, NULL, 0, frames,
					   fill_silence);
}

/* sanity-check for read/write methods */
static int pcm_sanity_check(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime;
	if (PCM_RUNTIME_CHECK(substream))
		return -ENXIO;
	runtime = substream->runtime;
	if (snd_BUG_ON(!substream->ops->copy_user && !runtime->dma_area))
		return -EINVAL;
	if (runtime->status->state == SNDRV_PCM_STATE_OPEN)
		return -EBADFD;
	return 0;
}

static int pcm_accessible_state(struct snd_pcm_runtime *runtime)
{
	switch (runtime->status->state) {
	case SNDRV_PCM_STATE_PREPARED:
	case SNDRV_PCM_STATE_RUNNING:
	case SNDRV_PCM_STATE_PAUSED:
		return 0;
	case SNDRV_PCM_STATE_XRUN:
		return -EPIPE;
	case SNDRV_PCM_STATE_SUSPENDED:
		return -ESTRPIPE;
	default:
		return -EBADFD;
	}
}

/* update to the given appl_ptr and call ack callback if needed;
 * when an error is returned, take back to the original value
 */
int pcm_lib_apply_appl_ptr(struct snd_pcm_substream *substream,
			   snd_pcm_uframes_t appl_ptr)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_uframes_t old_appl_ptr = runtime->control->appl_ptr;
	int ret;

	if (old_appl_ptr == appl_ptr)
		return 0;

	runtime->control->appl_ptr = appl_ptr;
	if (substream->ops->ack) {
		ret = substream->ops->ack(substream);
		if (ret < 0) {
			runtime->control->appl_ptr = old_appl_ptr;
			return ret;
		}
	}

	trace_applptr(substream, old_appl_ptr, appl_ptr);

	return 0;
}

/* the common loop for read/write data */
snd_pcm_sframes_t __snd_pcm_lib_xfer(struct snd_pcm_substream *substream,
				     void *data, bool interleaved,
				     snd_pcm_uframes_t size, bool in_kernel)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_uframes_t xfer = 0;
	snd_pcm_uframes_t offset = 0;
	snd_pcm_uframes_t avail;
	pcm_copy_f writer;
	pcm_transfer_f transfer;
	bool nonblock;
	bool is_playback;
	int err;

	err = pcm_sanity_check(substream);
	if (err < 0)
		return err;

	is_playback = substream->stream == SNDRV_PCM_STREAM_PLAYBACK;
	if (interleaved) {
		if (runtime->access != SNDRV_PCM_ACCESS_RW_INTERLEAVED &&
		    runtime->channels > 1)
			return -EINVAL;
		writer = interleaved_copy;
	} else {
		if (runtime->access != SNDRV_PCM_ACCESS_RW_NONINTERLEAVED)
			return -EINVAL;
		writer = noninterleaved_copy;
	}

	if (!data) {
		if (is_playback)
			transfer = fill_silence;
		else
			return -EINVAL;
	} else if (in_kernel) {
		if (substream->ops->copy_kernel)
			transfer = substream->ops->copy_kernel;
		else
			transfer = is_playback ?
				default_write_copy_kernel : default_read_copy_kernel;
	} else {
		if (substream->ops->copy_user)
			transfer = (pcm_transfer_f)substream->ops->copy_user;
		else
			transfer = is_playback ?
				default_write_copy : default_read_copy;
	}

	if (size == 0)
		return 0;

	nonblock = !!(substream->f_flags & O_NONBLOCK);

	snd_pcm_stream_lock_irq(substream);
	err = pcm_accessible_state(runtime);
	if (err < 0)
		goto _end_unlock;

	runtime->twake = runtime->control->avail_min ? : 1;
	if (runtime->status->state == SNDRV_PCM_STATE_RUNNING)
		snd_pcm_update_hw_ptr(substream);

	/*
	 * If size < start_threshold, wait indefinitely. Another
	 * thread may start capture
	 */
	if (!is_playback &&
	    runtime->status->state == SNDRV_PCM_STATE_PREPARED &&
	    size >= runtime->start_threshold) {
		err = snd_pcm_start(substream);
		if (err < 0)
			goto _end_unlock;
	}

	avail = snd_pcm_avail(substream);

	while (size > 0) {
		snd_pcm_uframes_t frames, appl_ptr, appl_ofs;
		snd_pcm_uframes_t cont;
		if (!avail) {
			if (!is_playback &&
			    runtime->status->state == SNDRV_PCM_STATE_DRAINING) {
				snd_pcm_stop(substream, SNDRV_PCM_STATE_SETUP);
				goto _end_unlock;
			}
			if (nonblock) {
				err = -EAGAIN;
				goto _end_unlock;
			}
			runtime->twake = min_t(snd_pcm_uframes_t, size,
					runtime->control->avail_min ? : 1);
			err = wait_for_avail(substream, &avail);
			if (err < 0)
				goto _end_unlock;
			if (!avail)
				continue; /* draining */
		}
		frames = size > avail ? avail : size;
		appl_ptr = READ_ONCE(runtime->control->appl_ptr);
		appl_ofs = appl_ptr % runtime->buffer_size;
		cont = runtime->buffer_size - appl_ofs;
		if (frames > cont)
			frames = cont;
		if (snd_BUG_ON(!frames)) {
			err = -EINVAL;
			goto _end_unlock;
		}
		snd_pcm_stream_unlock_irq(substream);
		err = writer(substream, appl_ofs, data, offset, frames,
			     transfer);
		snd_pcm_stream_lock_irq(substream);
		if (err < 0)
			goto _end_unlock;
		err = pcm_accessible_state(runtime);
		if (err < 0)
			goto _end_unlock;
		appl_ptr += frames;
		if (appl_ptr >= runtime->boundary)
			appl_ptr -= runtime->boundary;
		err = pcm_lib_apply_appl_ptr(substream, appl_ptr);
		if (err < 0)
			goto _end_unlock;

		offset += frames;
		size -= frames;
		xfer += frames;
		avail -= frames;
		if (is_playback &&
		    runtime->status->state == SNDRV_PCM_STATE_PREPARED &&
		    snd_pcm_playback_hw_avail(runtime) >= (snd_pcm_sframes_t)runtime->start_threshold) {
			err = snd_pcm_start(substream);
			if (err < 0)
				goto _end_unlock;
		}
	}
 _end_unlock:
	runtime->twake = 0;
	if (xfer > 0 && err >= 0)
		snd_pcm_update_state(substream, runtime);
	snd_pcm_stream_unlock_irq(substream);
	return xfer > 0 ? (snd_pcm_sframes_t)xfer : err;
}
EXPORT_SYMBOL(__snd_pcm_lib_xfer);

/*
 * standard channel mapping helpers
 */

/* default channel maps for multi-channel playbacks, up to 8 channels */
const struct snd_pcm_chmap_elem snd_pcm_std_chmaps[] = {
	{ .channels = 1,
	  .map = { SNDRV_CHMAP_MONO } },
	{ .channels = 2,
	  .map = { SNDRV_CHMAP_FL, SNDRV_CHMAP_FR } },
	{ .channels = 4,
	  .map = { SNDRV_CHMAP_FL, SNDRV_CHMAP_FR,
		   SNDRV_CHMAP_RL, SNDRV_CHMAP_RR } },
	{ .channels = 6,
	  .map = { SNDRV_CHMAP_FL, SNDRV_CHMAP_FR,
		   SNDRV_CHMAP_RL, SNDRV_CHMAP_RR,
		   SNDRV_CHMAP_FC, SNDRV_CHMAP_LFE } },
	{ .channels = 8,
	  .map = { SNDRV_CHMAP_FL, SNDRV_CHMAP_FR,
		   SNDRV_CHMAP_RL, SNDRV_CHMAP_RR,
		   SNDRV_CHMAP_FC, SNDRV_CHMAP_LFE,
		   SNDRV_CHMAP_SL, SNDRV_CHMAP_SR } },
	{ }
};
EXPORT_SYMBOL_GPL(snd_pcm_std_chmaps);

/* alternative channel maps with CLFE <-> surround swapped for 6/8 channels */
const struct snd_pcm_chmap_elem snd_pcm_alt_chmaps[] = {
	{ .channels = 1,
	  .map = { SNDRV_CHMAP_MONO } },
	{ .channels = 2,
	  .map = { SNDRV_CHMAP_FL, SNDRV_CHMAP_FR } },
	{ .channels = 4,
	  .map = { SNDRV_CHMAP_FL, SNDRV_CHMAP_FR,
		   SNDRV_CHMAP_RL, SNDRV_CHMAP_RR } },
	{ .channels = 6,
	  .map = { SNDRV_CHMAP_FL, SNDRV_CHMAP_FR,
		   SNDRV_CHMAP_FC, SNDRV_CHMAP_LFE,
		   SNDRV_CHMAP_RL, SNDRV_CHMAP_RR } },
	{ .channels = 8,
	  .map = { SNDRV_CHMAP_FL, SNDRV_CHMAP_FR,
		   SNDRV_CHMAP_FC, SNDRV_CHMAP_LFE,
		   SNDRV_CHMAP_RL, SNDRV_CHMAP_RR,
		   SNDRV_CHMAP_SL, SNDRV_CHMAP_SR } },
	{ }
};
EXPORT_SYMBOL_GPL(snd_pcm_alt_chmaps);

static bool valid_chmap_channels(const struct snd_pcm_chmap *info, int ch)
{
	if (ch > info->max_channels)
		return false;
	return !info->channel_mask || (info->channel_mask & (1U << ch));
}

static int pcm_chmap_ctl_info(struct snd_kcontrol *kcontrol,
			      struct snd_ctl_elem_info *uinfo)
{
	struct snd_pcm_chmap *info = snd_kcontrol_chip(kcontrol);

	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = info->max_channels;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = SNDRV_CHMAP_LAST;
	return 0;
}

/* get callback for channel map ctl element
 * stores the channel position firstly matching with the current channels
 */
static int pcm_chmap_ctl_get(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcm_chmap *info = snd_kcontrol_chip(kcontrol);
	unsigned int idx = snd_ctl_get_ioffidx(kcontrol, &ucontrol->id);
	struct snd_pcm_substream *substream;
	const struct snd_pcm_chmap_elem *map;

	if (!info->chmap)
		return -EINVAL;
	substream = snd_pcm_chmap_substream(info, idx);
	if (!substream)
		return -ENODEV;
	memset(ucontrol->value.integer.value, 0,
	       sizeof(long) * info->max_channels);
	if (!substream->runtime)
		return 0; /* no channels set */
	for (map = info->chmap; map->channels; map++) {
		int i;
		if (map->channels == substream->runtime->channels &&
		    valid_chmap_channels(info, map->channels)) {
			for (i = 0; i < map->channels; i++)
				ucontrol->value.integer.value[i] = map->map[i];
			return 0;
		}
	}
	return -EINVAL;
}

/* tlv callback for channel map ctl element
 * expands the pre-defined channel maps in a form of TLV
 */
static int pcm_chmap_ctl_tlv(struct snd_kcontrol *kcontrol, int op_flag,
			     unsigned int size, unsigned int __user *tlv)
{
	struct snd_pcm_chmap *info = snd_kcontrol_chip(kcontrol);
	const struct snd_pcm_chmap_elem *map;
	unsigned int __user *dst;
	int c, count = 0;

	if (!info->chmap)
		return -EINVAL;
	if (size < 8)
		return -ENOMEM;
	if (put_user(SNDRV_CTL_TLVT_CONTAINER, tlv))
		return -EFAULT;
	size -= 8;
	dst = tlv + 2;
	for (map = info->chmap; map->channels; map++) {
		int chs_bytes = map->channels * 4;
		if (!valid_chmap_channels(info, map->channels))
			continue;
		if (size < 8)
			return -ENOMEM;
		if (put_user(SNDRV_CTL_TLVT_CHMAP_FIXED, dst) ||
		    put_user(chs_bytes, dst + 1))
			return -EFAULT;
		dst += 2;
		size -= 8;
		count += 8;
		if (size < chs_bytes)
			return -ENOMEM;
		size -= chs_bytes;
		count += chs_bytes;
		for (c = 0; c < map->channels; c++) {
			if (put_user(map->map[c], dst))
				return -EFAULT;
			dst++;
		}
	}
	if (put_user(count, tlv + 1))
		return -EFAULT;
	return 0;
}

static void pcm_chmap_ctl_private_free(struct snd_kcontrol *kcontrol)
{
	struct snd_pcm_chmap *info = snd_kcontrol_chip(kcontrol);
	info->pcm->streams[info->stream].chmap_kctl = NULL;
	kfree(info);
}

/**
 * snd_pcm_add_chmap_ctls - create channel-mapping control elements
 * @pcm: the assigned PCM instance
 * @stream: stream direction
 * @chmap: channel map elements (for query)
 * @max_channels: the max number of channels for the stream
 * @private_value: the value passed to each kcontrol's private_value field
 * @info_ret: store struct snd_pcm_chmap instance if non-NULL
 *
 * Create channel-mapping control elements assigned to the given PCM stream(s).
 * Return: Zero if successful, or a negative error value.
 */
int snd_pcm_add_chmap_ctls(struct snd_pcm *pcm, int stream,
			   const struct snd_pcm_chmap_elem *chmap,
			   int max_channels,
			   unsigned long private_value,
			   struct snd_pcm_chmap **info_ret)
{
	struct snd_pcm_chmap *info;
	struct snd_kcontrol_new knew = {
		.iface = SNDRV_CTL_ELEM_IFACE_PCM,
		.access = SNDRV_CTL_ELEM_ACCESS_READ |
			SNDRV_CTL_ELEM_ACCESS_TLV_READ |
			SNDRV_CTL_ELEM_ACCESS_TLV_CALLBACK,
		.info = pcm_chmap_ctl_info,
		.get = pcm_chmap_ctl_get,
		.tlv.c = pcm_chmap_ctl_tlv,
	};
	int err;

	if (WARN_ON(pcm->streams[stream].chmap_kctl))
		return -EBUSY;
	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;
	info->pcm = pcm;
	info->stream = stream;
	info->chmap = chmap;
	info->max_channels = max_channels;
	if (stream == SNDRV_PCM_STREAM_PLAYBACK)
		knew.name = "Playback Channel Map";
	else
		knew.name = "Capture Channel Map";
	knew.device = pcm->device;
	knew.count = pcm->streams[stream].substream_count;
	knew.private_value = private_value;
	info->kctl = snd_ctl_new1(&knew, info);
	if (!info->kctl) {
		kfree(info);
		return -ENOMEM;
	}
	info->kctl->private_free = pcm_chmap_ctl_private_free;
	err = snd_ctl_add(pcm->card, info->kctl);
	if (err < 0)
		return err;
	pcm->streams[stream].chmap_kctl = info->kctl;
	if (info_ret)
		*info_ret = info;
	return 0;
}
EXPORT_SYMBOL_GPL(snd_pcm_add_chmap_ctls);
