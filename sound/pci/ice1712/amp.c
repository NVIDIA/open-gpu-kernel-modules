// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *   ALSA driver for ICEnsemble VT1724 (Envy24HT)
 *
 *   Lowlevel functions for Advanced Micro Peripherals Ltd AUDIO2000
 *
 *	Copyright (c) 2000 Jaroslav Kysela <perex@perex.cz>
 */      

#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <sound/core.h>

#include "ice1712.h"
#include "envy24ht.h"
#include "amp.h"

static void wm_put(struct snd_ice1712 *ice, int reg, unsigned short val)
{
	unsigned short cval;
	cval = (reg << 9) | val;
	snd_vt1724_write_i2c(ice, WM_DEV, cval >> 8, cval & 0xff);
}

static int snd_vt1724_amp_init(struct snd_ice1712 *ice)
{
	static const unsigned short wm_inits[] = {
		WM_ATTEN_L,	0x0000,	/* 0 db */
		WM_ATTEN_R,	0x0000,	/* 0 db */
		WM_DAC_CTRL,	0x0008,	/* 24bit I2S */
		WM_INT_CTRL,	0x0001, /* 24bit I2S */	
	};

	unsigned int i;

	/* only use basic functionality for now */

	/* VT1616 6ch codec connected to PSDOUT0 using packed mode */
	ice->num_total_dacs = 6;
	ice->num_total_adcs = 2;

	/* Chaintech AV-710 has another WM8728 codec connected to PSDOUT4
	   (shared with the SPDIF output). Mixer control for this codec
	   is not yet supported. */
	if (ice->eeprom.subvendor == VT1724_SUBDEVICE_AV710) {
		for (i = 0; i < ARRAY_SIZE(wm_inits); i += 2)
			wm_put(ice, wm_inits[i], wm_inits[i+1]);
	}

	return 0;
}

static int snd_vt1724_amp_add_controls(struct snd_ice1712 *ice)
{
	if (ice->ac97)
		/* we use pins 39 and 41 of the VT1616 for left and right
		read outputs */
		snd_ac97_write_cache(ice->ac97, 0x5a,
			snd_ac97_read(ice->ac97, 0x5a) & ~0x8000);
	return 0;
}


/* entry point */
struct snd_ice1712_card_info snd_vt1724_amp_cards[] = {
	{
		.subvendor = VT1724_SUBDEVICE_AV710,
		.name = "Chaintech AV-710",
		.model = "av710",
		.chip_init = snd_vt1724_amp_init,
		.build_controls = snd_vt1724_amp_add_controls,
	},
	{
		.subvendor = VT1724_SUBDEVICE_AUDIO2000,
		.name = "AMP Ltd AUDIO2000",
		.model = "amp2000",
		.chip_init = snd_vt1724_amp_init,
		.build_controls = snd_vt1724_amp_add_controls,
	},
	{ } /* terminator */
};

