/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef __SOUND_TEA575X_TUNER_H
#define __SOUND_TEA575X_TUNER_H

/*
 *   ALSA driver for TEA5757/5759 Philips AM/FM tuner chips
 *
 *	Copyright (c) 2004 Jaroslav Kysela <perex@perex.cz>
 */

#include <linux/videodev2.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-dev.h>
#include <media/v4l2-device.h>

#define TEA575X_FMIF	10700
#define TEA575X_AMIF	  450

#define TEA575X_DATA	(1 << 0)
#define TEA575X_CLK	(1 << 1)
#define TEA575X_WREN	(1 << 2)
#define TEA575X_MOST	(1 << 3)

struct snd_tea575x;

struct snd_tea575x_ops {
	/* Drivers using snd_tea575x must either define read_ and write_val */
	void (*write_val)(struct snd_tea575x *tea, u32 val);
	u32 (*read_val)(struct snd_tea575x *tea);
	/* Or define the 3 pin functions */
	void (*set_pins)(struct snd_tea575x *tea, u8 pins);
	u8 (*get_pins)(struct snd_tea575x *tea);
	void (*set_direction)(struct snd_tea575x *tea, bool output);
};

struct snd_tea575x {
	struct v4l2_device *v4l2_dev;
	struct v4l2_file_operations fops;
	struct video_device vd;		/* video device */
	int radio_nr;			/* radio_nr */
	bool tea5759;			/* 5759 chip is present */
	bool has_am;			/* Device can tune to AM freqs */
	bool cannot_read_data;		/* Device cannot read the data pin */
	bool cannot_mute;		/* Device cannot mute */
	bool mute;			/* Device is muted? */
	bool stereo;			/* receiving stereo */
	bool tuned;			/* tuned to a station */
	unsigned int val;		/* hw value */
	u32 band;			/* 0: FM, 1: FM-Japan, 2: AM */
	u32 freq;			/* frequency */
	struct mutex mutex;
	const struct snd_tea575x_ops *ops;
	void *private_data;
	u8 card[32];
	u8 bus_info[32];
	struct v4l2_ctrl_handler ctrl_handler;
	int (*ext_init)(struct snd_tea575x *tea);
};

int snd_tea575x_enum_freq_bands(struct snd_tea575x *tea,
					struct v4l2_frequency_band *band);
int snd_tea575x_g_tuner(struct snd_tea575x *tea, struct v4l2_tuner *v);
int snd_tea575x_s_hw_freq_seek(struct file *file, struct snd_tea575x *tea,
				const struct v4l2_hw_freq_seek *a);
int snd_tea575x_hw_init(struct snd_tea575x *tea);
int snd_tea575x_init(struct snd_tea575x *tea, struct module *owner);
void snd_tea575x_exit(struct snd_tea575x *tea);
void snd_tea575x_set_freq(struct snd_tea575x *tea);

#endif /* __SOUND_TEA575X_TUNER_H */
