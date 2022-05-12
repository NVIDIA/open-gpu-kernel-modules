// SPDX-License-Identifier: GPL-2.0-only
/*
 * tegra_wm8753.c - Tegra machine ASoC driver for boards using WM8753 codec.
 *
 * Author: Stephen Warren <swarren@nvidia.com>
 * Copyright (C) 2010-2012 - NVIDIA, Inc.
 *
 * Based on code copyright/by:
 *
 * (c) 2009, 2010 Nvidia Graphics Pvt. Ltd.
 *
 * Copyright 2007 Wolfson Microelectronics PLC.
 * Author: Graeme Gregory
 *         graeme.gregory@wolfsonmicro.com or linux@wolfsonmicro.com
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>

#include <sound/core.h>
#include <sound/jack.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#include "../codecs/wm8753.h"

#include "tegra_asoc_utils.h"

#define DRV_NAME "tegra-snd-wm8753"

struct tegra_wm8753 {
	struct tegra_asoc_utils_data util_data;
};

static int tegra_wm8753_hw_params(struct snd_pcm_substream *substream,
					struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct snd_soc_dai *codec_dai = asoc_rtd_to_codec(rtd, 0);
	struct snd_soc_card *card = rtd->card;
	struct tegra_wm8753 *machine = snd_soc_card_get_drvdata(card);
	int srate, mclk;
	int err;

	srate = params_rate(params);
	switch (srate) {
	case 11025:
	case 22050:
	case 44100:
	case 88200:
		mclk = 11289600;
		break;
	default:
		mclk = 12288000;
		break;
	}

	err = tegra_asoc_utils_set_rate(&machine->util_data, srate, mclk);
	if (err < 0) {
		dev_err(card->dev, "Can't configure clocks\n");
		return err;
	}

	err = snd_soc_dai_set_sysclk(codec_dai, WM8753_MCLK, mclk,
					SND_SOC_CLOCK_IN);
	if (err < 0) {
		dev_err(card->dev, "codec_dai clock not set\n");
		return err;
	}

	return 0;
}

static const struct snd_soc_ops tegra_wm8753_ops = {
	.hw_params = tegra_wm8753_hw_params,
};

static const struct snd_soc_dapm_widget tegra_wm8753_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Headphone Jack", NULL),
	SND_SOC_DAPM_MIC("Mic Jack", NULL),
};

SND_SOC_DAILINK_DEFS(pcm,
	DAILINK_COMP_ARRAY(COMP_EMPTY()),
	DAILINK_COMP_ARRAY(COMP_CODEC(NULL, "wm8753-hifi")),
	DAILINK_COMP_ARRAY(COMP_EMPTY()));

static struct snd_soc_dai_link tegra_wm8753_dai = {
	.name = "WM8753",
	.stream_name = "WM8753 PCM",
	.ops = &tegra_wm8753_ops,
	.dai_fmt = SND_SOC_DAIFMT_I2S |
			SND_SOC_DAIFMT_NB_NF |
			SND_SOC_DAIFMT_CBS_CFS,
	SND_SOC_DAILINK_REG(pcm),
};

static struct snd_soc_card snd_soc_tegra_wm8753 = {
	.name = "tegra-wm8753",
	.owner = THIS_MODULE,
	.dai_link = &tegra_wm8753_dai,
	.num_links = 1,

	.dapm_widgets = tegra_wm8753_dapm_widgets,
	.num_dapm_widgets = ARRAY_SIZE(tegra_wm8753_dapm_widgets),
	.fully_routed = true,
};

static int tegra_wm8753_driver_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct snd_soc_card *card = &snd_soc_tegra_wm8753;
	struct tegra_wm8753 *machine;
	int ret;

	machine = devm_kzalloc(&pdev->dev, sizeof(struct tegra_wm8753),
			       GFP_KERNEL);
	if (!machine)
		return -ENOMEM;

	card->dev = &pdev->dev;
	snd_soc_card_set_drvdata(card, machine);

	ret = snd_soc_of_parse_card_name(card, "nvidia,model");
	if (ret)
		return ret;

	ret = snd_soc_of_parse_audio_routing(card, "nvidia,audio-routing");
	if (ret)
		return ret;

	tegra_wm8753_dai.codecs->of_node = of_parse_phandle(np,
			"nvidia,audio-codec", 0);
	if (!tegra_wm8753_dai.codecs->of_node) {
		dev_err(&pdev->dev,
			"Property 'nvidia,audio-codec' missing or invalid\n");
		return -EINVAL;
	}

	tegra_wm8753_dai.cpus->of_node = of_parse_phandle(np,
			"nvidia,i2s-controller", 0);
	if (!tegra_wm8753_dai.cpus->of_node) {
		dev_err(&pdev->dev,
			"Property 'nvidia,i2s-controller' missing or invalid\n");
		return -EINVAL;
	}

	tegra_wm8753_dai.platforms->of_node = tegra_wm8753_dai.cpus->of_node;

	ret = tegra_asoc_utils_init(&machine->util_data, &pdev->dev);
	if (ret)
		return ret;

	ret = devm_snd_soc_register_card(&pdev->dev, card);
	if (ret)
		return dev_err_probe(&pdev->dev, ret,
				     "snd_soc_register_card failed\n");

	return 0;
}

static const struct of_device_id tegra_wm8753_of_match[] = {
	{ .compatible = "nvidia,tegra-audio-wm8753", },
	{},
};

static struct platform_driver tegra_wm8753_driver = {
	.driver = {
		.name = DRV_NAME,
		.pm = &snd_soc_pm_ops,
		.of_match_table = tegra_wm8753_of_match,
	},
	.probe = tegra_wm8753_driver_probe,
};
module_platform_driver(tegra_wm8753_driver);

MODULE_AUTHOR("Stephen Warren <swarren@nvidia.com>");
MODULE_DESCRIPTION("Tegra+WM8753 machine ASoC driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" DRV_NAME);
MODULE_DEVICE_TABLE(of, tegra_wm8753_of_match);
