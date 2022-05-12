// SPDX-License-Identifier: GPL-2.0
/*
 * mt8173-rt5650-rt5514.c  --  MT8173 machine driver with RT5650/5514 codecs
 *
 * Copyright (c) 2016 MediaTek Inc.
 * Author: Koro Chen <koro.chen@mediatek.com>
 */

#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <sound/soc.h>
#include <sound/jack.h>
#include "../../codecs/rt5645.h"

#define MCLK_FOR_CODECS		12288000

static const struct snd_soc_dapm_widget mt8173_rt5650_rt5514_widgets[] = {
	SND_SOC_DAPM_SPK("Speaker", NULL),
	SND_SOC_DAPM_MIC("Int Mic", NULL),
	SND_SOC_DAPM_HP("Headphone", NULL),
	SND_SOC_DAPM_MIC("Headset Mic", NULL),
};

static const struct snd_soc_dapm_route mt8173_rt5650_rt5514_routes[] = {
	{"Speaker", NULL, "SPOL"},
	{"Speaker", NULL, "SPOR"},
	{"Sub DMIC1L", NULL, "Int Mic"},
	{"Sub DMIC1R", NULL, "Int Mic"},
	{"Headphone", NULL, "HPOL"},
	{"Headphone", NULL, "HPOR"},
	{"IN1P", NULL, "Headset Mic"},
	{"IN1N", NULL, "Headset Mic"},
};

static const struct snd_kcontrol_new mt8173_rt5650_rt5514_controls[] = {
	SOC_DAPM_PIN_SWITCH("Speaker"),
	SOC_DAPM_PIN_SWITCH("Int Mic"),
	SOC_DAPM_PIN_SWITCH("Headphone"),
	SOC_DAPM_PIN_SWITCH("Headset Mic"),
};

static int mt8173_rt5650_rt5514_hw_params(struct snd_pcm_substream *substream,
					  struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct snd_soc_dai *codec_dai;
	int i, ret;

	for_each_rtd_codec_dais(rtd, i, codec_dai) {
		/* pll from mclk 12.288M */
		ret = snd_soc_dai_set_pll(codec_dai, 0, 0, MCLK_FOR_CODECS,
					  params_rate(params) * 512);
		if (ret)
			return ret;

		/* sysclk from pll */
		ret = snd_soc_dai_set_sysclk(codec_dai, 1,
					     params_rate(params) * 512,
					     SND_SOC_CLOCK_IN);
		if (ret)
			return ret;
	}
	return 0;
}

static const struct snd_soc_ops mt8173_rt5650_rt5514_ops = {
	.hw_params = mt8173_rt5650_rt5514_hw_params,
};

static struct snd_soc_jack mt8173_rt5650_rt5514_jack;

static int mt8173_rt5650_rt5514_init(struct snd_soc_pcm_runtime *runtime)
{
	struct snd_soc_card *card = runtime->card;
	struct snd_soc_component *component = asoc_rtd_to_codec(runtime, 0)->component;
	int ret;

	rt5645_sel_asrc_clk_src(component,
				RT5645_DA_STEREO_FILTER |
				RT5645_AD_STEREO_FILTER,
				RT5645_CLK_SEL_I2S1_ASRC);

	/* enable jack detection */
	ret = snd_soc_card_jack_new(card, "Headset Jack",
				    SND_JACK_HEADPHONE | SND_JACK_MICROPHONE |
				    SND_JACK_BTN_0 | SND_JACK_BTN_1 |
				    SND_JACK_BTN_2 | SND_JACK_BTN_3,
				    &mt8173_rt5650_rt5514_jack, NULL, 0);
	if (ret) {
		dev_err(card->dev, "Can't new Headset Jack %d\n", ret);
		return ret;
	}

	return rt5645_set_jack_detect(component,
				      &mt8173_rt5650_rt5514_jack,
				      &mt8173_rt5650_rt5514_jack,
				      &mt8173_rt5650_rt5514_jack);
}

enum {
	DAI_LINK_PLAYBACK,
	DAI_LINK_CAPTURE,
	DAI_LINK_CODEC_I2S,
};

SND_SOC_DAILINK_DEFS(playback,
	DAILINK_COMP_ARRAY(COMP_CPU("DL1")),
	DAILINK_COMP_ARRAY(COMP_DUMMY()),
	DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(capture,
	DAILINK_COMP_ARRAY(COMP_CPU("VUL")),
	DAILINK_COMP_ARRAY(COMP_DUMMY()),
	DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(codec,
	DAILINK_COMP_ARRAY(COMP_CPU("I2S")),
	DAILINK_COMP_ARRAY(COMP_CODEC(NULL, "rt5645-aif1"),
			   COMP_CODEC(NULL, "rt5514-aif1")),
	DAILINK_COMP_ARRAY(COMP_EMPTY()));

/* Digital audio interface glue - connects codec <---> CPU */
static struct snd_soc_dai_link mt8173_rt5650_rt5514_dais[] = {
	/* Front End DAI links */
	[DAI_LINK_PLAYBACK] = {
		.name = "rt5650_rt5514 Playback",
		.stream_name = "rt5650_rt5514 Playback",
		.trigger = {SND_SOC_DPCM_TRIGGER_POST, SND_SOC_DPCM_TRIGGER_POST},
		.dynamic = 1,
		.dpcm_playback = 1,
		SND_SOC_DAILINK_REG(playback),
	},
	[DAI_LINK_CAPTURE] = {
		.name = "rt5650_rt5514 Capture",
		.stream_name = "rt5650_rt5514 Capture",
		.trigger = {SND_SOC_DPCM_TRIGGER_POST, SND_SOC_DPCM_TRIGGER_POST},
		.dynamic = 1,
		.dpcm_capture = 1,
		SND_SOC_DAILINK_REG(capture),
	},
	/* Back End DAI links */
	[DAI_LINK_CODEC_I2S] = {
		.name = "Codec",
		.no_pcm = 1,
		.init = mt8173_rt5650_rt5514_init,
		.dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
			   SND_SOC_DAIFMT_CBS_CFS,
		.ops = &mt8173_rt5650_rt5514_ops,
		.ignore_pmdown_time = 1,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
		SND_SOC_DAILINK_REG(codec),
	},
};

static struct snd_soc_codec_conf mt8173_rt5650_rt5514_codec_conf[] = {
	{
		.name_prefix = "Sub",
	},
};

static struct snd_soc_card mt8173_rt5650_rt5514_card = {
	.name = "mtk-rt5650-rt5514",
	.owner = THIS_MODULE,
	.dai_link = mt8173_rt5650_rt5514_dais,
	.num_links = ARRAY_SIZE(mt8173_rt5650_rt5514_dais),
	.codec_conf = mt8173_rt5650_rt5514_codec_conf,
	.num_configs = ARRAY_SIZE(mt8173_rt5650_rt5514_codec_conf),
	.controls = mt8173_rt5650_rt5514_controls,
	.num_controls = ARRAY_SIZE(mt8173_rt5650_rt5514_controls),
	.dapm_widgets = mt8173_rt5650_rt5514_widgets,
	.num_dapm_widgets = ARRAY_SIZE(mt8173_rt5650_rt5514_widgets),
	.dapm_routes = mt8173_rt5650_rt5514_routes,
	.num_dapm_routes = ARRAY_SIZE(mt8173_rt5650_rt5514_routes),
};

static int mt8173_rt5650_rt5514_dev_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card = &mt8173_rt5650_rt5514_card;
	struct device_node *platform_node;
	struct snd_soc_dai_link *dai_link;
	int i, ret;

	platform_node = of_parse_phandle(pdev->dev.of_node,
					 "mediatek,platform", 0);
	if (!platform_node) {
		dev_err(&pdev->dev, "Property 'platform' missing or invalid\n");
		return -EINVAL;
	}

	for_each_card_prelinks(card, i, dai_link) {
		if (dai_link->platforms->name)
			continue;
		dai_link->platforms->of_node = platform_node;
	}

	mt8173_rt5650_rt5514_dais[DAI_LINK_CODEC_I2S].codecs[0].of_node =
		of_parse_phandle(pdev->dev.of_node, "mediatek,audio-codec", 0);
	if (!mt8173_rt5650_rt5514_dais[DAI_LINK_CODEC_I2S].codecs[0].of_node) {
		dev_err(&pdev->dev,
			"Property 'audio-codec' missing or invalid\n");
		return -EINVAL;
	}
	mt8173_rt5650_rt5514_dais[DAI_LINK_CODEC_I2S].codecs[1].of_node =
		of_parse_phandle(pdev->dev.of_node, "mediatek,audio-codec", 1);
	if (!mt8173_rt5650_rt5514_dais[DAI_LINK_CODEC_I2S].codecs[1].of_node) {
		dev_err(&pdev->dev,
			"Property 'audio-codec' missing or invalid\n");
		return -EINVAL;
	}
	mt8173_rt5650_rt5514_codec_conf[0].dlc.of_node =
		mt8173_rt5650_rt5514_dais[DAI_LINK_CODEC_I2S].codecs[1].of_node;

	card->dev = &pdev->dev;

	ret = devm_snd_soc_register_card(&pdev->dev, card);
	if (ret)
		dev_err(&pdev->dev, "%s snd_soc_register_card fail %d\n",
			__func__, ret);
	return ret;
}

static const struct of_device_id mt8173_rt5650_rt5514_dt_match[] = {
	{ .compatible = "mediatek,mt8173-rt5650-rt5514", },
	{ }
};
MODULE_DEVICE_TABLE(of, mt8173_rt5650_rt5514_dt_match);

static struct platform_driver mt8173_rt5650_rt5514_driver = {
	.driver = {
		   .name = "mtk-rt5650-rt5514",
		   .of_match_table = mt8173_rt5650_rt5514_dt_match,
#ifdef CONFIG_PM
		   .pm = &snd_soc_pm_ops,
#endif
	},
	.probe = mt8173_rt5650_rt5514_dev_probe,
};

module_platform_driver(mt8173_rt5650_rt5514_driver);

/* Module information */
MODULE_DESCRIPTION("MT8173 RT5650 and RT5514 SoC machine driver");
MODULE_AUTHOR("Koro Chen <koro.chen@mediatek.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:mtk-rt5650-rt5514");

