// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * OSS compatible sequencer driver
 *
 * Timer control routines
 *
 * Copyright (C) 1998,99 Takashi Iwai <tiwai@suse.de>
 */

#include "seq_oss_timer.h"
#include "seq_oss_event.h"
#include <sound/seq_oss_legacy.h>
#include <linux/slab.h>

/*
 */
#define MIN_OSS_TEMPO		8
#define MAX_OSS_TEMPO		360
#define MIN_OSS_TIMEBASE	1
#define MAX_OSS_TIMEBASE	1000

/*
 */
static void calc_alsa_tempo(struct seq_oss_timer *timer);
static int send_timer_event(struct seq_oss_devinfo *dp, int type, int value);


/*
 * create and register a new timer.
 * if queue is not started yet, start it.
 */
struct seq_oss_timer *
snd_seq_oss_timer_new(struct seq_oss_devinfo *dp)
{
	struct seq_oss_timer *rec;

	rec = kzalloc(sizeof(*rec), GFP_KERNEL);
	if (rec == NULL)
		return NULL;

	rec->dp = dp;
	rec->cur_tick = 0;
	rec->realtime = 0;
	rec->running = 0;
	rec->oss_tempo = 60;
	rec->oss_timebase = 100;
	calc_alsa_tempo(rec);

	return rec;
}


/*
 * delete timer.
 * if no more timer exists, stop the queue.
 */
void
snd_seq_oss_timer_delete(struct seq_oss_timer *rec)
{
	if (rec) {
		snd_seq_oss_timer_stop(rec);
		kfree(rec);
	}
}


/*
 * process one timing event
 * return 1 : event proceseed -- skip this event
 *        0 : not a timer event -- enqueue this event
 */
int
snd_seq_oss_process_timer_event(struct seq_oss_timer *rec, union evrec *ev)
{
	abstime_t parm = ev->t.time;

	if (ev->t.code == EV_TIMING) {
		switch (ev->t.cmd) {
		case TMR_WAIT_REL:
			parm += rec->cur_tick;
			rec->realtime = 0;
			fallthrough;
		case TMR_WAIT_ABS:
			if (parm == 0) {
				rec->realtime = 1;
			} else if (parm >= rec->cur_tick) {
				rec->realtime = 0;
				rec->cur_tick = parm;
			}
			return 1;	/* skip this event */
			
		case TMR_START:
			snd_seq_oss_timer_start(rec);
			return 1;

		}
	} else if (ev->s.code == SEQ_WAIT) {
		/* time = from 1 to 3 bytes */
		parm = (ev->echo >> 8) & 0xffffff;
		if (parm > rec->cur_tick) {
			/* set next event time */
			rec->cur_tick = parm;
			rec->realtime = 0;
		}
		return 1;
	}

	return 0;
}


/*
 * convert tempo units
 */
static void
calc_alsa_tempo(struct seq_oss_timer *timer)
{
	timer->tempo = (60 * 1000000) / timer->oss_tempo;
	timer->ppq = timer->oss_timebase;
}


/*
 * dispatch a timer event
 */
static int
send_timer_event(struct seq_oss_devinfo *dp, int type, int value)
{
	struct snd_seq_event ev;

	memset(&ev, 0, sizeof(ev));
	ev.type = type;
	ev.source.client = dp->cseq;
	ev.source.port = 0;
	ev.dest.client = SNDRV_SEQ_CLIENT_SYSTEM;
	ev.dest.port = SNDRV_SEQ_PORT_SYSTEM_TIMER;
	ev.queue = dp->queue;
	ev.data.queue.queue = dp->queue;
	ev.data.queue.param.value = value;
	return snd_seq_kernel_client_dispatch(dp->cseq, &ev, 1, 0);
}

/*
 * set queue tempo and start queue
 */
int
snd_seq_oss_timer_start(struct seq_oss_timer *timer)
{
	struct seq_oss_devinfo *dp = timer->dp;
	struct snd_seq_queue_tempo tmprec;

	if (timer->running)
		snd_seq_oss_timer_stop(timer);

	memset(&tmprec, 0, sizeof(tmprec));
	tmprec.queue = dp->queue;
	tmprec.ppq = timer->ppq;
	tmprec.tempo = timer->tempo;
	snd_seq_set_queue_tempo(dp->cseq, &tmprec);

	send_timer_event(dp, SNDRV_SEQ_EVENT_START, 0);
	timer->running = 1;
	timer->cur_tick = 0;
	return 0;
}


/*
 * stop queue
 */
int
snd_seq_oss_timer_stop(struct seq_oss_timer *timer)
{
	if (! timer->running)
		return 0;
	send_timer_event(timer->dp, SNDRV_SEQ_EVENT_STOP, 0);
	timer->running = 0;
	return 0;
}


/*
 * continue queue
 */
int
snd_seq_oss_timer_continue(struct seq_oss_timer *timer)
{
	if (timer->running)
		return 0;
	send_timer_event(timer->dp, SNDRV_SEQ_EVENT_CONTINUE, 0);
	timer->running = 1;
	return 0;
}


/*
 * change queue tempo
 */
int
snd_seq_oss_timer_tempo(struct seq_oss_timer *timer, int value)
{
	if (value < MIN_OSS_TEMPO)
		value = MIN_OSS_TEMPO;
	else if (value > MAX_OSS_TEMPO)
		value = MAX_OSS_TEMPO;
	timer->oss_tempo = value;
	calc_alsa_tempo(timer);
	if (timer->running)
		send_timer_event(timer->dp, SNDRV_SEQ_EVENT_TEMPO, timer->tempo);
	return 0;
}


/*
 * ioctls
 */
int
snd_seq_oss_timer_ioctl(struct seq_oss_timer *timer, unsigned int cmd, int __user *arg)
{
	int value;

	if (cmd == SNDCTL_SEQ_CTRLRATE) {
		/* if *arg == 0, just return the current rate */
		if (get_user(value, arg))
			return -EFAULT;
		if (value)
			return -EINVAL;
		value = ((timer->oss_tempo * timer->oss_timebase) + 30) / 60;
		return put_user(value, arg) ? -EFAULT : 0;
	}

	if (timer->dp->seq_mode == SNDRV_SEQ_OSS_MODE_SYNTH)
		return 0;

	switch (cmd) {
	case SNDCTL_TMR_START:
		return snd_seq_oss_timer_start(timer);
	case SNDCTL_TMR_STOP:
		return snd_seq_oss_timer_stop(timer);
	case SNDCTL_TMR_CONTINUE:
		return snd_seq_oss_timer_continue(timer);
	case SNDCTL_TMR_TEMPO:
		if (get_user(value, arg))
			return -EFAULT;
		return snd_seq_oss_timer_tempo(timer, value);
	case SNDCTL_TMR_TIMEBASE:
		if (get_user(value, arg))
			return -EFAULT;
		if (value < MIN_OSS_TIMEBASE)
			value = MIN_OSS_TIMEBASE;
		else if (value > MAX_OSS_TIMEBASE)
			value = MAX_OSS_TIMEBASE;
		timer->oss_timebase = value;
		calc_alsa_tempo(timer);
		return 0;

	case SNDCTL_TMR_METRONOME:
	case SNDCTL_TMR_SELECT:
	case SNDCTL_TMR_SOURCE:
		/* not supported */
		return 0;
	}
	return 0;
}
