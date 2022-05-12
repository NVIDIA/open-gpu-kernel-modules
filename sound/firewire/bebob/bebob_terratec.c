// SPDX-License-Identifier: GPL-2.0-only
/*
 * bebob_terratec.c - a part of driver for BeBoB based devices
 *
 * Copyright (c) 2013-2014 Takashi Sakamoto
 */

#include "./bebob.h"

static const enum snd_bebob_clock_type phase88_rack_clk_src_types[] = {
	SND_BEBOB_CLOCK_TYPE_INTERNAL,
	SND_BEBOB_CLOCK_TYPE_EXTERNAL,	/* S/PDIF */
	SND_BEBOB_CLOCK_TYPE_EXTERNAL,	/* Word Clock */
};
static int
phase88_rack_clk_src_get(struct snd_bebob *bebob, unsigned int *id)
{
	unsigned int enable_ext, enable_word;
	int err;

	err = avc_audio_get_selector(bebob->unit, 0, 9, &enable_ext);
	if (err < 0)
		goto end;
	err = avc_audio_get_selector(bebob->unit, 0, 8, &enable_word);
	if (err < 0)
		goto end;

	if (enable_ext == 0)
		*id = 0;
	else if (enable_word == 0)
		*id = 1;
	else
		*id = 2;
end:
	return err;
}

static const struct snd_bebob_rate_spec phase_series_rate_spec = {
	.get	= &snd_bebob_stream_get_rate,
	.set	= &snd_bebob_stream_set_rate,
};

/* PHASE 88 Rack FW */
static const struct snd_bebob_clock_spec phase88_rack_clk = {
	.num	= ARRAY_SIZE(phase88_rack_clk_src_types),
	.types	= phase88_rack_clk_src_types,
	.get	= &phase88_rack_clk_src_get,
};
const struct snd_bebob_spec phase88_rack_spec = {
	.clock	= &phase88_rack_clk,
	.rate	= &phase_series_rate_spec,
	.meter	= NULL
};
