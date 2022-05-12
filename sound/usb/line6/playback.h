/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Line 6 Linux USB driver
 *
 * Copyright (C) 2004-2010 Markus Grabner (grabner@icg.tugraz.at)
 */

#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <sound/pcm.h>

#include "driver.h"

/*
 * When the TonePort is used with jack in full duplex mode and the outputs are
 * not connected, the software monitor produces an ugly noise since everything
 * written to the output buffer (i.e., the input signal) will be repeated in
 * the next period (sounds like a delay effect). As a workaround, the output
 * buffer is cleared after the data have been read, but there must be a better
 * solution. Until one is found, this workaround can be used to fix the
 * problem.
 */
#define USE_CLEAR_BUFFER_WORKAROUND 1

extern const struct snd_pcm_ops snd_line6_playback_ops;

extern int line6_create_audio_out_urbs(struct snd_line6_pcm *line6pcm);
extern int line6_submit_audio_out_all_urbs(struct snd_line6_pcm *line6pcm);

#endif
