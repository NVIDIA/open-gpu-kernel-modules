// SPDX-License-Identifier: GPL-2.0-only
/*
 * Tegra machine ASoC driver for boards using a MAX90809 CODEC.
 *
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * Based on code copyright/by:
 *
 * Copyright (C) 2010-2012 - NVIDIA, Inc.
 * Copyright (C) 2011 The AC100 Kernel Team <ac100@lists.lauchpad.net>
 * (c) 2009, 2010 Nvidia Graphics Pvt. Ltd.
 * Copyright 2007 Wolfson Microelectronics PLC.
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

#include "tegra_asoc_utils.h"

#define DRV_NAME "tegra-snd-max98090"

struct tegra_max98090 {
	struct tegra_asoc_utils_data util_data;
	int gpio_hp_det;
	int gpio_mic_det;
};

static int tegra_max98090_asoc_hw_params(struct snd_pcm_substream *substream,
					struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct snd_soc_dai *codec_dai = asoc_rtd_to_codec(rtd, 0);
	struct snd_soc_card *card = rtd->card;
	struct tegra_max98090 *machine = snd_soc_card_get_drvdata(card);
	int srate, mclk;
	int err;

	srate = params_rate(params);
	switch (srate) {
	case 8000:
	case 16000:
	case 24000:
	case 32000:
	case 48000:
	case 64000:
	case 96000:
		mclk = 12288000;
		break;
	case 11025:
	case 22050:
	case 44100:
	case 88200:
		mclk = 11289600;
		break;
	default:
		mclk = 12000000;
		break;
	}

	err = tegra_asoc_utils_set_rate(&machine->util_data, srate, mclk);
	if (err < 0) {
		dev_err(card->dev, "Can't configure clocks\n");
		return err;
	}

	err = snd_soc_dai_set_sysclk(codec_dai, 0, mclk,
					SND_SOC_CLOCK_IN);
	if (err < 0) {
		dev_err(card->dev, "codec_dai clock not set\n");
		return err;
	}

	return 0;
}

static const struct snd_soc_ops tegra_max98090_ops = {
	.hw_params = tegra_max98090_asoc_hw_params,
};

static struct snd_soc_jack tegra_max98090_hp_jack;

static struct snd_soc_jack_pin tegra_max98090_hp_jack_pins[] = {
	{
		.pin = "Headphones",
		.mask = SND_JACK_HEADPHONE,
	},
};

static struct snd_soc_jack_gpio tegra_max98090_hp_jack_gpio = {
	.name = "Headphone detection",
	.report = SND_JACK_HEADPHONE,
	.debounce_time = 150,
	.invert = 1,
};

static struct snd_soc_jack tegra_max98090_mic_jack;

static struct snd_soc_jack_pin tegra_max98090_mic_jack_pins[] = {
	{
		.pin = "Mic Jack",
		.mask = SND_JACK_MICROPHONE,
	},
};

static struct snd_soc_jack_gpio tegra_max98090_mic_jack_gpio = {
	.name = "Mic detection",
	.report = SND_JACK_MICROPHONE,
	.debounce_time = 150,
	.invert = 1,
};

static const struct snd_soc_dapm_widget tegra_max98090_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Headphones", NULL),
	SND_SOC_DAPM_SPK("Speakers", NULL),
	SND_SOC_DAPM_MIC("Mic Jack", NULL),
	SND_SOC_DAPM_MIC("Int Mic", NULL),
};

static const struct snd_kcontrol_new tegra_max98090_controls[] = {
	SOC_DAPM_PIN_SWITCH("Headphones"),
	SOC_DAPM_PIN_SWITCH("Speakers"),
	SOC_DAPM_PIN_SWITCH("Mic Jack"),
	SOC_DAPM_PIN_SWITCH("Int Mic"),
};

static int tegra_max98090_asoc_init(struct snd_soc_pcm_runtime *rtd)
{
	struct tegra_max98090 *machine = snd_soc_card_get_drvdata(rtd->card);

	if (gpio_is_valid(machine->gpio_hp_det)) {
		snd_soc_card_jack_new(rtd->card, "Headphones",
				      SND_JACK_HEADPHONE,
				      &tegra_max98090_hp_jack,
				      tegra_max98090_hp_jack_pins,
				      ARRAY_SIZE(tegra_max98090_hp_jack_pins));

		tegra_max98090_hp_jack_gpio.gpio = machine->gpio_hp_det;
		snd_soc_jack_add_gpios(&tegra_max98090_hp_jack,
					1,
					&tegra_max98090_hp_jack_gpio);
	}

	if (gpio_is_valid(machine->gpio_mic_det)) {
		snd_soc_card_jack_new(rtd->card, "Mic Jack",
				      SND_JACK_MICROPHONE,
				      &tegra_max98090_mic_jack,
				      tegra_max98090_mic_jack_pins,
				      ARRAY_SIZE(tegra_max98090_mic_jack_pins));

		tegra_max98090_mic_jack_gpio.gpio = machine->gpio_mic_det;
		snd_soc_jack_add_gpios(&tegra_max98090_mic_jack,
				       1,
				       &tegra_max98090_mic_jack_gpio);
	}

	return 0;
}

SND_SOC_DAILINK_DEFS(pcm,
	DAILINK_COMP_ARRAY(COMP_EMPTY()),
	DAILINK_COMP_ARRAY(COMP_CODEC(NULL, "HiFi")),
	DAILINK_COMP_ARRAY(COMP_EMPTY()));

static struct snd_soc_dai_link tegra_max98090_dai = {
	.name = "max98090",
	.stream_name = "max98090 PCM",
	.init = tegra_max98090_asoc_init,
	.ops = &tegra_max98090_ops,
	.dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
			SND_SOC_DAIFMT_CBS_CFS,
	SND_SOC_DAILINK_REG(pcm),
};

static struct snd_soc_card snd_soc_tegra_max98090 = {
	.name = "tegra-max98090",
	.owner = THIS_MODULE,
	.dai_link = &tegra_max98090_dai,
	.num_links = 1,
	.controls = tegra_max98090_controls,
	.num_controls = ARRAY_SIZE(tegra_max98090_controls),
	.dapm_widgets = tegra_max98090_dapm_widgets,
	.num_dapm_widgets = ARRAY_SIZE(tegra_max98090_dapm_widgets),
	.fully_routed = true,
};

static int tegra_max98090_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct snd_soc_card *card = &snd_soc_tegra_max98090;
	struct tegra_max98090 *machine;
	int ret;

	machine = devm_kzalloc(&pdev->dev,
			sizeof(struct tegra_max98090), GFP_KERNEL);
	if (!machine)
		return -ENOMEM;

	card->dev = &pdev->dev;
	snd_soc_card_set_drvdata(card, machine);

	machine->gpio_hp_det = of_get_named_gpio(np, "nvidia,hp-det-gpios", 0);
	if (machine->gpio_hp_det == -EPROBE_DEFER)
		return -EPROBE_DEFER;

	machine->gpio_mic_det =
			of_get_named_gpio(np, "nvidia,mic-det-gpios", 0);
	if (machine->gpio_mic_det == -EPROBE_DEFER)
		return -EPROBE_DEFER;

	ret = snd_soc_of_parse_card_name(card, "nvidia,model");
	if (ret)
		return ret;

	ret = snd_soc_of_parse_audio_routing(card, "nvidia,audio-routing");
	if (ret)
		return ret;

	tegra_max98090_dai.codecs->of_node = of_parse_phandle(np,
			"nvidia,audio-codec", 0);
	if (!tegra_max98090_dai.codecs->of_node) {
		dev_err(&pdev->dev,
			"Property 'nvidia,audio-codec' missing or invalid\n");
		return -EINVAL;
	}

	tegra_max98090_dai.cpus->of_node = of_parse_phandle(np,
			"nvidia,i2s-controller", 0);
	if (!tegra_max98090_dai.cpus->of_node) {
		dev_err(&pdev->dev,
			"Property 'nvidia,i2s-controller' missing or invalid\n");
		return -EINVAL;
	}

	tegra_max98090_dai.platforms->of_node = tegra_max98090_dai.cpus->of_node;

	ret = tegra_asoc_utils_init(&machine->util_data, &pdev->dev);
	if (ret)
		return ret;

	ret = devm_snd_soc_register_card(&pdev->dev, card);
	if (ret)
		return dev_err_probe(&pdev->dev, ret,
				     "snd_soc_register_card failed\n");

	return 0;
}

static const struct of_device_id tegra_max98090_of_match[] = {
	{ .compatible = "nvidia,tegra-audio-max98090", },
	{},
};

static struct platform_driver tegra_max98090_driver = {
	.driver = {
		.name = DRV_NAME,
		.pm = &snd_soc_pm_ops,
		.of_match_table = tegra_max98090_of_match,
	},
	.probe = tegra_max98090_probe,
};
module_platform_driver(tegra_max98090_driver);

MODULE_AUTHOR("Stephen Warren <swarren@nvidia.com>");
MODULE_DESCRIPTION("Tegra max98090 machine ASoC driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRV_NAME);
MODULE_DEVICE_TABLE(of, tegra_max98090_of_match);
