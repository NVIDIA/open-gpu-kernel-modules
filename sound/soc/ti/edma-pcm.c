// SPDX-License-Identifier: GPL-2.0-only
/*
 * edma-pcm.c - eDMA PCM driver using dmaengine for AM3xxx, AM4xxx
 *
 * Copyright (C) 2014 Texas Instruments, Inc.
 *
 * Author: Peter Ujfalusi <peter.ujfalusi@ti.com>
 *
 * Based on: sound/soc/tegra/tegra_pcm.c
 */

#include <linux/module.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/dmaengine_pcm.h>

#include "edma-pcm.h"

static const struct snd_pcm_hardware edma_pcm_hardware = {
	.info			= SNDRV_PCM_INFO_MMAP |
				  SNDRV_PCM_INFO_MMAP_VALID |
				  SNDRV_PCM_INFO_PAUSE | SNDRV_PCM_INFO_RESUME |
				  SNDRV_PCM_INFO_NO_PERIOD_WAKEUP |
				  SNDRV_PCM_INFO_INTERLEAVED,
	.buffer_bytes_max	= 128 * 1024,
	.period_bytes_min	= 32,
	.period_bytes_max	= 64 * 1024,
	.periods_min		= 2,
	.periods_max		= 19, /* Limit by edma dmaengine driver */
};

static const struct snd_dmaengine_pcm_config edma_dmaengine_pcm_config = {
	.pcm_hardware = &edma_pcm_hardware,
	.prepare_slave_config = snd_dmaengine_pcm_prepare_slave_config,
	.prealloc_buffer_size = 128 * 1024,
};

int edma_pcm_platform_register(struct device *dev)
{
	struct snd_dmaengine_pcm_config *config;

	if (dev->of_node)
		return devm_snd_dmaengine_pcm_register(dev,
						&edma_dmaengine_pcm_config, 0);

	config = devm_kzalloc(dev, sizeof(*config), GFP_KERNEL);
	if (!config)
		return -ENOMEM;

	*config = edma_dmaengine_pcm_config;

	config->chan_names[0] = "tx";
	config->chan_names[1] = "rx";

	return devm_snd_dmaengine_pcm_register(dev, config, 0);
}
EXPORT_SYMBOL_GPL(edma_pcm_platform_register);

MODULE_AUTHOR("Peter Ujfalusi <peter.ujfalusi@ti.com>");
MODULE_DESCRIPTION("eDMA PCM ASoC platform driver");
MODULE_LICENSE("GPL");
