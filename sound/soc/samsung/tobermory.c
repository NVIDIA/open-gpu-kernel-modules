// SPDX-License-Identifier: GPL-2.0+
//
// Tobermory audio support
//
// Copyright 2011 Wolfson Microelectronics

#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/jack.h>
#include <linux/gpio.h>
#include <linux/module.h>

#include "../codecs/wm8962.h"

static int sample_rate = 44100;

static int tobermory_set_bias_level(struct snd_soc_card *card,
					  struct snd_soc_dapm_context *dapm,
					  enum snd_soc_bias_level level)
{
	struct snd_soc_pcm_runtime *rtd;
	struct snd_soc_dai *codec_dai;
	int ret;

	rtd = snd_soc_get_pcm_runtime(card, &card->dai_link[0]);
	codec_dai = asoc_rtd_to_codec(rtd, 0);

	if (dapm->dev != codec_dai->dev)
		return 0;

	switch (level) {
	case SND_SOC_BIAS_PREPARE:
		if (dapm->bias_level == SND_SOC_BIAS_STANDBY) {
			ret = snd_soc_dai_set_pll(codec_dai, WM8962_FLL,
						  WM8962_FLL_MCLK, 32768,
						  sample_rate * 512);
			if (ret < 0)
				pr_err("Failed to start FLL: %d\n", ret);

			ret = snd_soc_dai_set_sysclk(codec_dai,
						     WM8962_SYSCLK_FLL,
						     sample_rate * 512,
						     SND_SOC_CLOCK_IN);
			if (ret < 0) {
				pr_err("Failed to set SYSCLK: %d\n", ret);
				snd_soc_dai_set_pll(codec_dai, WM8962_FLL,
						    0, 0, 0);
				return ret;
			}
		}
		break;

	default:
		break;
	}

	return 0;
}

static int tobermory_set_bias_level_post(struct snd_soc_card *card,
					       struct snd_soc_dapm_context *dapm,
					       enum snd_soc_bias_level level)
{
	struct snd_soc_pcm_runtime *rtd;
	struct snd_soc_dai *codec_dai;
	int ret;

	rtd = snd_soc_get_pcm_runtime(card, &card->dai_link[0]);
	codec_dai = asoc_rtd_to_codec(rtd, 0);

	if (dapm->dev != codec_dai->dev)
		return 0;

	switch (level) {
	case SND_SOC_BIAS_STANDBY:
		ret = snd_soc_dai_set_sysclk(codec_dai, WM8962_SYSCLK_MCLK,
					     32768, SND_SOC_CLOCK_IN);
		if (ret < 0) {
			pr_err("Failed to switch away from FLL: %d\n", ret);
			return ret;
		}

		ret = snd_soc_dai_set_pll(codec_dai, WM8962_FLL,
					  0, 0, 0);
		if (ret < 0) {
			pr_err("Failed to stop FLL: %d\n", ret);
			return ret;
		}
		break;

	default:
		break;
	}

	dapm->bias_level = level;

	return 0;
}

static int tobermory_hw_params(struct snd_pcm_substream *substream,
			      struct snd_pcm_hw_params *params)
{
	sample_rate = params_rate(params);

	return 0;
}

static struct snd_soc_ops tobermory_ops = {
	.hw_params = tobermory_hw_params,
};

SND_SOC_DAILINK_DEFS(cpu,
	DAILINK_COMP_ARRAY(COMP_CPU("samsung-i2s.0")),
	DAILINK_COMP_ARRAY(COMP_CODEC("wm8962.1-001a", "wm8962")),
	DAILINK_COMP_ARRAY(COMP_PLATFORM("samsung-i2s.0")));

static struct snd_soc_dai_link tobermory_dai[] = {
	{
		.name = "CPU",
		.stream_name = "CPU",
		.dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF
				| SND_SOC_DAIFMT_CBM_CFM,
		.ops = &tobermory_ops,
		SND_SOC_DAILINK_REG(cpu),
	},
};

static const struct snd_kcontrol_new controls[] = {
	SOC_DAPM_PIN_SWITCH("Main Speaker"),
	SOC_DAPM_PIN_SWITCH("DMIC"),
};

static struct snd_soc_dapm_widget widgets[] = {
	SND_SOC_DAPM_HP("Headphone", NULL),
	SND_SOC_DAPM_MIC("Headset Mic", NULL),

	SND_SOC_DAPM_MIC("DMIC", NULL),
	SND_SOC_DAPM_MIC("AMIC", NULL),

	SND_SOC_DAPM_SPK("Main Speaker", NULL),
};

static struct snd_soc_dapm_route audio_paths[] = {
	{ "Headphone", NULL, "HPOUTL" },
	{ "Headphone", NULL, "HPOUTR" },

	{ "Main Speaker", NULL, "SPKOUTL" },
	{ "Main Speaker", NULL, "SPKOUTR" },

	{ "Headset Mic", NULL, "MICBIAS" },
	{ "IN4L", NULL, "Headset Mic" },
	{ "IN4R", NULL, "Headset Mic" },

	{ "AMIC", NULL, "MICBIAS" },
	{ "IN1L", NULL, "AMIC" },
	{ "IN1R", NULL, "AMIC" },

	{ "DMIC", NULL, "MICBIAS" },
	{ "DMICDAT", NULL, "DMIC" },
};

static struct snd_soc_jack tobermory_headset;

/* Headset jack detection DAPM pins */
static struct snd_soc_jack_pin tobermory_headset_pins[] = {
	{
		.pin = "Headset Mic",
		.mask = SND_JACK_MICROPHONE,
	},
	{
		.pin = "Headphone",
		.mask = SND_JACK_MICROPHONE,
	},
};

static int tobermory_late_probe(struct snd_soc_card *card)
{
	struct snd_soc_pcm_runtime *rtd;
	struct snd_soc_component *component;
	struct snd_soc_dai *codec_dai;
	int ret;

	rtd = snd_soc_get_pcm_runtime(card, &card->dai_link[0]);
	component = asoc_rtd_to_codec(rtd, 0)->component;
	codec_dai = asoc_rtd_to_codec(rtd, 0);

	ret = snd_soc_dai_set_sysclk(codec_dai, WM8962_SYSCLK_MCLK,
				     32768, SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;

	ret = snd_soc_card_jack_new(card, "Headset", SND_JACK_HEADSET |
				    SND_JACK_BTN_0, &tobermory_headset,
				    tobermory_headset_pins,
				    ARRAY_SIZE(tobermory_headset_pins));
	if (ret)
		return ret;

	wm8962_mic_detect(component, &tobermory_headset);

	return 0;
}

static struct snd_soc_card tobermory = {
	.name = "Tobermory",
	.owner = THIS_MODULE,
	.dai_link = tobermory_dai,
	.num_links = ARRAY_SIZE(tobermory_dai),

	.set_bias_level = tobermory_set_bias_level,
	.set_bias_level_post = tobermory_set_bias_level_post,

	.controls = controls,
	.num_controls = ARRAY_SIZE(controls),
	.dapm_widgets = widgets,
	.num_dapm_widgets = ARRAY_SIZE(widgets),
	.dapm_routes = audio_paths,
	.num_dapm_routes = ARRAY_SIZE(audio_paths),
	.fully_routed = true,

	.late_probe = tobermory_late_probe,
};

static int tobermory_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card = &tobermory;
	int ret;

	card->dev = &pdev->dev;

	ret = devm_snd_soc_register_card(&pdev->dev, card);
	if (ret && ret != -EPROBE_DEFER)
		dev_err(&pdev->dev, "snd_soc_register_card() failed: %d\n",
			ret);

	return ret;
}

static struct platform_driver tobermory_driver = {
	.driver = {
		.name = "tobermory",
		.pm = &snd_soc_pm_ops,
	},
	.probe = tobermory_probe,
};

module_platform_driver(tobermory_driver);

MODULE_DESCRIPTION("Tobermory audio support");
MODULE_AUTHOR("Mark Brown <broonie@opensource.wolfsonmicro.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:tobermory");
