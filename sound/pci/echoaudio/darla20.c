// SPDX-License-Identifier: GPL-2.0-only
/*
 *  ALSA driver for Echoaudio soundcards.
 *  Copyright (C) 2003-2004 Giuliano Pochini <pochini@shiny.it>
 */

#define ECHOGALS_FAMILY
#define ECHOCARD_DARLA20
#define ECHOCARD_NAME "Darla20"
#define ECHOCARD_HAS_MONITOR

/* Pipe indexes */
#define PX_ANALOG_OUT	0	/* 8 */
#define PX_DIGITAL_OUT	8	/* 0 */
#define PX_ANALOG_IN	8	/* 2 */
#define PX_DIGITAL_IN	10	/* 0 */
#define PX_NUM		10

/* Bus indexes */
#define BX_ANALOG_OUT	0	/* 8 */
#define BX_DIGITAL_OUT	8	/* 0 */
#define BX_ANALOG_IN	8	/* 2 */
#define BX_DIGITAL_IN	10	/* 0 */
#define BX_NUM		10


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
#include <linux/atomic.h>
#include "echoaudio.h"

MODULE_FIRMWARE("ea/darla20_dsp.fw");

#define FW_DARLA20_DSP	0

static const struct firmware card_fw[] = {
	{0, "darla20_dsp.fw"}
};

static const struct pci_device_id snd_echo_ids[] = {
	{0x1057, 0x1801, 0xECC0, 0x0010, 0, 0, 0},	/* DSP 56301 Darla20 rev.0 */
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
	.rates = SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_48000,
	.rate_min = 44100,
	.rate_max = 48000,
	.channels_min = 1,
	.channels_max = 2,
	.buffer_bytes_max = 262144,
	.period_bytes_min = 32,
	.period_bytes_max = 131072,
	.periods_min = 2,
	.periods_max = 220,
	/* One page (4k) contains 512 instructions. I don't know if the hw
	supports lists longer than this. In this case periods_max=220 is a
	safe limit to make sure the list never exceeds 512 instructions. */
};


#include "darla20_dsp.c"
#include "echoaudio_dsp.c"
#include "echoaudio.c"
