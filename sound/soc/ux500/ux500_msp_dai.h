/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) ST-Ericsson SA 2012
 *
 * Author: Ola Lilja <ola.o.lilja@stericsson.com>,
 *         Roger Nilsson <roger.xr.nilsson@stericsson.com>
 *         for ST-Ericsson.
 *
 * License terms:
 */

#ifndef UX500_msp_dai_H
#define UX500_msp_dai_H

#include <linux/types.h>
#include <linux/spinlock.h>

#include "ux500_msp_i2s.h"

#define UX500_NBR_OF_DAI	4

#define UX500_I2S_RATES (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000 |	\
			SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_48000)

#define UX500_I2S_FORMATS (SNDRV_PCM_FMTBIT_S16_LE)

#define FRAME_PER_SINGLE_SLOT_8_KHZ		31
#define FRAME_PER_SINGLE_SLOT_16_KHZ	124
#define FRAME_PER_SINGLE_SLOT_44_1_KHZ	63
#define FRAME_PER_SINGLE_SLOT_48_KHZ	49
#define FRAME_PER_2_SLOTS				31
#define FRAME_PER_8_SLOTS				138
#define FRAME_PER_16_SLOTS				277

#define UX500_MSP_INTERNAL_CLOCK_FREQ  40000000
#define UX500_MSP1_INTERNAL_CLOCK_FREQ UX500_MSP_INTERNAL_CLOCK_FREQ

#define UX500_MSP_MIN_CHANNELS		1
#define UX500_MSP_MAX_CHANNELS		8

#define PLAYBACK_CONFIGURED		1
#define CAPTURE_CONFIGURED		2

enum ux500_msp_clock_id {
	UX500_MSP_MASTER_CLOCK,
};

struct ux500_msp_i2s_drvdata {
	struct ux500_msp *msp;
	struct regulator *reg_vape;
	unsigned int fmt;
	unsigned int tx_mask;
	unsigned int rx_mask;
	int slots;
	int slot_width;

	/* Clocks */
	unsigned int master_clk;
	struct clk *clk;
	struct clk *pclk;

	/* Regulators */
	int vape_opp_constraint;
};

int ux500_msp_dai_set_data_delay(struct snd_soc_dai *dai, int delay);

#endif
