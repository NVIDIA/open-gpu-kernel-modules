/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef __SOUND_PCM_OSS_H
#define __SOUND_PCM_OSS_H

/*
 *  Digital Audio (PCM) - OSS compatibility abstract layer
 *  Copyright (c) by Jaroslav Kysela <perex@perex.cz>
 */

struct snd_pcm_oss_setup {
	char *task_name;
	unsigned int disable:1,
		     direct:1,
		     block:1,
		     nonblock:1,
		     partialfrag:1,
		     nosilence:1,
		     buggyptr:1;
	unsigned int periods;
	unsigned int period_size;
	struct snd_pcm_oss_setup *next;
};

struct snd_pcm_oss_runtime {
	unsigned params: 1,			/* format/parameter change */
		 prepare: 1,			/* need to prepare the operation */
		 trigger: 1,			/* trigger flag */
		 sync_trigger: 1;		/* sync trigger flag */
	int rate;				/* requested rate */
	int format;				/* requested OSS format */
	unsigned int channels;			/* requested channels */
	unsigned int fragshift;
	unsigned int maxfrags;
	unsigned int subdivision;		/* requested subdivision */
	size_t period_bytes;			/* requested period size */
	size_t period_frames;			/* period frames for poll */
	size_t period_ptr;			/* actual write pointer to period */
	unsigned int periods;
	size_t buffer_bytes;			/* requested buffer size */
	size_t bytes;				/* total # bytes processed */
	size_t mmap_bytes;
	char *buffer;				/* vmallocated period */
	size_t buffer_used;			/* used length from period buffer */
	struct mutex params_lock;
	atomic_t rw_ref;		/* concurrent read/write accesses */
#ifdef CONFIG_SND_PCM_OSS_PLUGINS
	struct snd_pcm_plugin *plugin_first;
	struct snd_pcm_plugin *plugin_last;
#endif
	unsigned int prev_hw_ptr_period;
};

struct snd_pcm_oss_file {
	struct snd_pcm_substream *streams[2];
};

struct snd_pcm_oss_substream {
	unsigned oss: 1;			/* oss mode */
	struct snd_pcm_oss_setup setup;		/* active setup */
};

struct snd_pcm_oss_stream {
	struct snd_pcm_oss_setup *setup_list;	/* setup list */
	struct mutex setup_mutex;
#ifdef CONFIG_SND_VERBOSE_PROCFS
	struct snd_info_entry *proc_entry;
#endif
};

struct snd_pcm_oss {
	int reg;
	unsigned int reg_mask;
};

#endif /* __SOUND_PCM_OSS_H */
