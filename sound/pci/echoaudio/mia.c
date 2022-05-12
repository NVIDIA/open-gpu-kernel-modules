// SPDX-License-Identifier: GPL-2.0-only
/*
 *  ALSA driver for Echoaudio soundcards.
 *  Copyright (C) 2003-2004 Giuliano Pochini <pochini@shiny.it>
 */

#define ECHO24_FAMILY
#define ECHOCARD_MIA
#define ECHOCARD_NAME "Mia"
#define ECHOCARD_HAS_MONITOR
#define ECHOCARD_HAS_INPUT_NOMINAL_LEVEL
#define ECHOCARD_HAS_OUTPUT_NOMINAL_LEVEL
#define ECHOCARD_HAS_SUPER_INTERLEAVE
#define ECHOCARD_HAS_VMIXER
#define ECHOCARD_HAS_DIGITAL_IO
#define ECHOCARD_HAS_EXTERNAL_CLOCK
#define ECHOCARD_HAS_ADAT	false
#define ECHOCARD_HAS_STEREO_BIG_ENDIAN32
#define ECHOCARD_HAS_MIDI
#define ECHOCARD_HAS_LINE_OUT_GAIN

/* Pipe indexes */
#define PX_ANALOG_OUT	0	/* 8 */
#define PX_DIGITAL_OUT	8	/* 0 */
#define PX_ANALOG_IN	8	/* 2 */
#define PX_DIGITAL_IN	10	/* 2 */
#define PX_NUM		12

/* Bus indexes */
#define BX_ANALOG_OUT	0	/* 2 */
#define BX_DIGITAL_OUT	2	/* 2 */
#define BX_ANALOG_IN	4	/* 2 */
#define BX_DIGITAL_IN	6	/* 2 */
#define BX_NUM		8


#include <linux/delay.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/module.h>
#include <linux/firmware.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <sound/core.h>
#include <sound/info.h>
#include <sound/control.h>
#include <sound/tlv.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/asoundef.h>
#include <sound/initval.h>
#include <sound/rawmidi.h>
#include <linux/atomic.h>
#include "echoaudio.h"

MODULE_FIRMWARE("ea/loader_dsp.fw");
MODULE_FIRMWARE("ea/mia_dsp.fw");

#define FW_361_LOADER	0
#define FW_MIA_DSP	1

static const struct firmware card_fw[] = {
	{0, "loader_dsp.fw"},
	{0, "mia_dsp.fw"}
};

static const struct pci_device_id snd_echo_ids[] = {
	{0x1057, 0x3410, 0xECC0, 0x0080, 0, 0, 0},	/* DSP 56361 Mia rev.0 */
	{0x1057, 0x3410, 0xECC0, 0x0081, 0, 0, 0},	/* DSP 56361 Mia rev.1 */
	{0,}
};

static const struct snd_pcm_hardware pcm_hardware_skel = {
	.info = SNDRV_PCM_INFO_MMAP |
		SNDRV_PCM_INFO_INTERLEAVED |
		SNDRV_PCM_INFO_BLOCK_TRANSFER |
		SNDRV_PCM_INFO_MMAP_VALID |
		SNDRV_PCM_INFO_PAUSE |
		SNDRV_PCM_INFO_SYNC_START,
	.formats =	SNDRV_PCM_FMTBIT_U8 |
			SNDRV_PCM_FMTBIT_S16_LE |
			SNDRV_PCM_FMTBIT_S24_3LE |
			SNDRV_PCM_FMTBIT_S32_LE |
			SNDRV_PCM_FMTBIT_S32_BE,
	.rates = 	SNDRV_PCM_RATE_32000 |
			SNDRV_PCM_RATE_44100 |
			SNDRV_PCM_RATE_48000 |
			SNDRV_PCM_RATE_88200 |
			SNDRV_PCM_RATE_96000,
	.rate_min = 8000,
	.rate_max = 96000,
	.channels_min = 1,
	.channels_max = 8,
	.buffer_bytes_max = 262144,
	.period_bytes_min = 32,
	.period_bytes_max = 131072,
	.periods_min = 2,
	.periods_max = 220,
	/* One page (4k) contains 512 instructions. I don't know if the hw
	supports lists longer than this. In this case periods_max=220 is a
	safe limit to make sure the list never exceeds 512 instructions. */
};


#include "mia_dsp.c"
#include "echoaudio_dsp.c"
#include "echoaudio.c"
#include "midi.c"
